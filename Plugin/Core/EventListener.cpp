#include <pch.h>
#include "EventListener.h"

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

	// Happens whenever a button was pressed after loading a new shot
	_gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this, statUpdater](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		statUpdater->processNewAttempt();
	});

	// Happens whenever a car spawns, i.e. usually a shot reset
	_gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", [this, statUpdater](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		// Future extension: Process miss, unless a goal was scored since the previous attempt
		statUpdater->processShotReset();
	});

	// Allow resetting statistics to zero attempts/goals manually
	_cvarManager->registerNotifier("goalpercentagecounter_reset", [this, statUpdater](const std::vector<std::string>&) {
		if (!_gameWrapper->IsInCustomTraining()) { return; }
		
		// Note: Manual reset is allowed even with the plugin disabled, or during a goal replay (because why not?)
		statUpdater->processManualStatReset();
	}, "Reset the statistics.", PERMISSION_ALL);

	// Reset automatically when loading a new training pack, or when resetting it
	_gameWrapper->HookEventPost("Function TAGame.GameEvent_TrainingEditor_TA.OnInit", [this, statUpdater](const std::string&) {
		if (!_pluginState->PluginIsEnabled) { return; }
		// Note: While loading a training pack, we are not in custom training, so we can't use statUpdatesShallBeSent() here

		statUpdater->handleTrainingPackLoad();
	});
}
void EventListener::registerRenderEvents( std::shared_ptr<IStatDisplay> statDisplay )
{
	if (!statDisplay) { return; }

	_gameWrapper->RegisterDrawable([this, statDisplay](const CanvasWrapper& canvas) {
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