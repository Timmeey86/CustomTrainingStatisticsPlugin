#include <pch.h>
#include "EventListener.h"
#include "../Data/TriggerNames.h"

EventListener::EventListener(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState)
	: _gameWrapper( gameWrapper )
	, _cvarManager( cvarManager )
	, _pluginState( pluginState )
{
}

void EventListener::registerUpdateEvents( std::shared_ptr<IStatUpdater> statUpdater )
{
	if (!statUpdater) { return; }

	// Happens every tick
	_gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", [this](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		ServerWrapper server = _gameWrapper->GetGameEventAsServer();
		if (server.IsNull()) return;
		BallWrapper ball = server.GetBall();
		if (ball.IsNull()) return;

		_pluginState->setBallSpeed(ball.GetVelocity().magnitude());
	});

	_stateMachine = std::make_shared<CustomTrainingStateMachine>(_cvarManager, statUpdater);
	_stateMachine->hookToEvents(_gameWrapper);

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier(TriggerNames::ResetStatistics, [this, statUpdater](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }
		
		// Note: Manual reset is allowed even with the plugin disabled, or during a goal replay (because why not?)
		statUpdater->processManualStatReset();
	}, "Reset the statistics.", PERMISSION_ALL);

	// Happens when custom taining mode is loaded or restarted
	_gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function GameEvent_TrainingEditor_TA.WaitingToPlayTest.OnTrainingModeLoaded",
		[this, statUpdater](ActorWrapper caller, void*, const std::string&) {
			if (!_pluginState->PluginIsEnabled) { return; }
						
			// Update the state machine with this event
			TrainingEditorWrapper trainingWrapper(caller.memory_address);
			if (!trainingWrapper.IsNull())
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
	// REFACTOR - Directly within in EventListener (no need for an extra class)
	// Allow ignoring events which occur during a goal replay, it would otherwise spam us with goal events, and one reset event
	_gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [this](const std::string&) {
		_pluginState->GoalReplayIsActive = true;
	});
	// UNVERIFIED ASSUMPTION: There is no way for leaving ReplayPlayback without EndState being called (e.g. resetting a training pack during goal replay or whatever)
	_gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [this](const std::string&) {
		_pluginState->GoalReplayIsActive = false;
	});
}

bool EventListener::statUpdatesShallBeSent()
{
	return _gameWrapper->IsInCustomTraining() && !_pluginState->GoalReplayIsActive && _pluginState->PluginIsEnabled;
}