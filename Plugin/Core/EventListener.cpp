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

	// Happens whenever a goal was scored
	_gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this, statUpdater](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		statUpdater->processGoal();
	});

	// Happens whenever the ball is being touched
	_gameWrapper->HookEvent("Function TAGame.Ball_TA.OnCarTouch", [this, statUpdater](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		// Remember if the ball was touched at least once within the current attempt
		// This allows e.g. tracking the success of speedflip attempts with a close timer, independent of whether a goal was or wasn't scored
		if (!_pluginState->BallWasHitAtLeastOnceWithinCurrentAttempt)
		{
			_pluginState->BallWasHitAtLeastOnceWithinCurrentAttempt = true;
			statUpdater->processInitialBallHit();
		}
	});

	// Happens whenever a button was pressed after loading a new shot
	_gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this, statUpdater](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		_pluginState->BallWasHitAtLeastOnceWithinCurrentAttempt = false;
		statUpdater->processNewAttempt();
	});

	// Happens whenever a shot is changed or loaded in custom training
	_gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GameEvent_TrainingEditor_TA.EventRoundChanged",
		[this](ActorWrapper caller, void*, const std::string&) {

			TrainingEditorWrapper trainingWrapper(caller.memory_address);
			_pluginState->CurrentRoundIndex = trainingWrapper.GetActiveRoundNumber();
			_pluginState->TotalRounds = trainingWrapper.GetTotalRounds();
		});

	// Happens whenever a car spawns, i.e. usually a shot reset
	_gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", [this, statUpdater](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		// Future extension: Process miss, unless a goal was scored since the previous attempt
		statUpdater->processShotReset();
	});

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier(TriggerNames::ResetStatistics, [this, statUpdater](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }
		
		// Note: Manual reset is allowed even with the plugin disabled, or during a goal replay (because why not?)
		statUpdater->processManualStatReset();
	}, "Reset the statistics.", PERMISSION_ALL);

	// Happens when custom taining mode is loaded or restarted, but the total rounds is loaded
	// Reset automatically when loading a new training pack, or when resetting it
	_gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function GameEvent_TrainingEditor_TA.WaitingToPlayTest.OnTrainingModeLoaded",
		[this, statUpdater](ActorWrapper caller, void*, const std::string&) {
			if (!_pluginState->PluginIsEnabled) { return; }
			// Note: While loading a training pack, we are not in custom training, so we can't use statUpdatesShallBeSent() here 
			// Note may not be true from since this was changed from OnInit to OnTrainingModeLoaded

			TrainingEditorWrapper trainingWrapper(caller.memory_address);
			_pluginState->TotalRounds = trainingWrapper.GetTotalRounds();

			statUpdater->handleTrainingPackLoad();
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