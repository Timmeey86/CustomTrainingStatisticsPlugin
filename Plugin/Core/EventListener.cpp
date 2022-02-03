#include <pch.h>
#include "EventListener.h"
#include "../Data/TriggerNames.h"

EventListener::EventListener(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState)
	: _gameWrapper( gameWrapper )
	, _cvarManager( cvarManager )
	, _pluginState( pluginState )
{
}

void EventListener::registerUpdateEvents( std::shared_ptr<IStatUpdater> statUpdater, std::shared_ptr<IStatWriter> statWriter )
{
	if (!statUpdater) { return; }

	_stateMachine = std::make_shared<CustomTrainingStateMachine>(_cvarManager, statUpdater, statWriter, _pluginState);
	_stateMachine->hookToEvents(_gameWrapper);

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier(TriggerNames::ResetStatistics, [this, statUpdater](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }
		
		statUpdater->processReset(_pluginState->TotalRounds);
	}, "Reset the statistics.", PERMISSION_ALL);

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier(TriggerNames::RestoreStatistics, [this, statUpdater](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }

		statUpdater->restoreLastSession();
	}, "Restore the statistics.", PERMISSION_ALL);

	// Happens when custom taining mode is loaded or restarted
	_gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function GameEvent_TrainingEditor_TA.WaitingToPlayTest.OnTrainingModeLoaded",
		[this, statUpdater](ActorWrapper caller, void*, const std::string&) {
			if (!_pluginState->PluginIsEnabled) { return; }
						
			// Update the state machine with this event
			if (TrainingEditorWrapper trainingWrapper(caller.memory_address); 
				!trainingWrapper.IsNull())
			{
				_stateMachine->processOnTrainingModeLoaded(trainingWrapper);
			}

			// Reset other state variables
			_pluginState->MenuStackSize = 0;
			_pluginState->IsMetric = _gameWrapper->GetbMetric();
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

void EventListener::registerRenderEvents( std::shared_ptr<IStatDisplay> statDisplay )
{
	if (!statDisplay) { return; }

	_gameWrapper->RegisterDrawable([this, statDisplay](CanvasWrapper canvas) {
		if (_pluginState->PluginIsEnabled && _gameWrapper->IsInCustomTraining())
		{
			statDisplay->renderOneFrame(canvas);
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