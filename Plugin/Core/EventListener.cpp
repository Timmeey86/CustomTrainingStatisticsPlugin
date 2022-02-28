#include <pch.h>
#include "EventListener.h"
#include "../Data/TriggerNames.h"

EventListener::EventListener(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState)
	: _gameWrapper(gameWrapper)
	, _cvarManager(cvarManager)
	, _pluginState(pluginState)
{
}

void EventListener::registerUpdateEvents(std::shared_ptr<IStatUpdater> statUpdater, std::shared_ptr<IStatWriter> statWriter)
{
	if (!statUpdater) { return; }

	_stateMachine = std::make_shared<CustomTrainingStateMachine>(_cvarManager, statWriter, _pluginState);
	_stateMachine->hookToEvents(_gameWrapper, _eventReceivers);

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier(TriggerNames::ResetStatistics, [this, statWriter](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }

		for (auto eventReceiver : _eventReceivers)
		{
			eventReceiver->onResetStatisticsTriggered();
		}

		// After manually resetting we have to overwrite the current file with zero attempts since otherwise
		// a stat restore after the reset would restore the session which was reset, rather than the one before.
		// We want to support the use case where the player started an attempt, then remembered they intended to restore the previous session
		// which they can't, since they started already. With this change, they can press reset, and then restore the previous session,
		// since the current session's file will be skipped as it has no attempts
		statWriter->writeData();
	}, "Reset the statistics.", PERMISSION_ALL);

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier(TriggerNames::RestoreStatistics, [this](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }

		for (auto eventReceiver : _eventReceivers)
		{
			eventReceiver->onRestorePreviousSessionTriggered();
		}
	}, "Restore the statistics.", PERMISSION_ALL);

	// Allow toggling the last attempt between miss and goal
	_cvarManager->registerNotifier(TriggerNames::ToggleLastAttempt, [this](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }

		for (auto eventReceiver : _eventReceivers)
		{
			eventReceiver->onTogglePreviousAttemptTriggered();
		}
	}, "Toggle the last attempt to be a goal or a miss", PERMISSION_ALL);

	// Happens when custom taining mode is loaded or restarted
	_gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function GameEvent_TrainingEditor_TA.WaitingToPlayTest.OnTrainingModeLoaded",
		[this, statUpdater](ActorWrapper caller, void*, const std::string&) {
		if (!_pluginState->PluginIsEnabled) { return; }

		// Update the state machine with this event
		if (TrainingEditorWrapper trainingWrapper(caller.memory_address);
			!trainingWrapper.IsNull())
		{
			auto trainingPackCode = trainingWrapper.GetTrainingData().GetTrainingData().GetCode().ToString();
			for (auto eventReceiver : _eventReceivers)
			{
				eventReceiver->onTrainingModeLoaded(trainingWrapper, trainingPackCode);
			}

			_stateMachine->processOnTrainingModeLoaded(trainingWrapper, trainingPackCode, _eventReceivers);
		}

		// Reset other state variables
		_pluginState->MenuStackSize = 0;
		_pluginState->IsMetric = _gameWrapper->GetbMetric();
	});

	_gameWrapper->HookEvent("Function TAGame.CarComponent_Dodge_TA.EventActivateDodge",
		[this](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		for (auto eventReceiver : _eventReceivers)
		{
			eventReceiver->onCarFlipped();
		}
	});

	// Happens whenever a menu is opened (also when opening a nested menu)
	_gameWrapper->HookEvent("Function TAGame.GFxData_MenuStack_TA.PushMenu", [this](const std::string&) {
		_pluginState->MenuStackSize++;
	});
	// Happens whenever a menu is closed (also when closing a nested menu)
	_gameWrapper->HookEvent("Function TAGame.GFxData_MenuStack_TA.PopMenu", [this](const std::string&) {
		// Decrement the menu stack size but don't let it go below zero (This is just here as a safeguard in case the menu stack size ever gets reset while a menu is still open)
		if (_pluginState->MenuStackSize > 0)
		{
			_pluginState->MenuStackSize--;
		}
		_pluginState->IsMetric = _gameWrapper->GetbMetric(); // Check for change of metric setting
	});
}

void EventListener::registerRenderEvents(std::vector<std::shared_ptr<IStatDisplay>> statDisplays)
{
	if (statDisplays.empty()) { return; }

	_gameWrapper->RegisterDrawable([this, statDisplays](CanvasWrapper canvas) {
		// Draw the overlay when no menu is open, or at most one menu (the "pause" menu) is open
		// That way we don't clutter the settings, or the match/mode selection screen
		if (_pluginState->PluginIsEnabled && _gameWrapper->IsInCustomTraining() && _pluginState->MenuStackSize < 2)
		{
			for (auto statDisplay : statDisplays)
			{
				statDisplay->renderOneFrame(canvas);
			}
		}
	});
}

void EventListener::registerGameStateEvents()
{
}

bool EventListener::statUpdatesShallBeSent()
{
	return _gameWrapper->IsInCustomTraining() && _pluginState->PluginIsEnabled;
}

void EventListener::addEventReceiver(std::shared_ptr<AbstractEventReceiver> eventReceiver)
{
	_eventReceivers.emplace_back(eventReceiver);
}