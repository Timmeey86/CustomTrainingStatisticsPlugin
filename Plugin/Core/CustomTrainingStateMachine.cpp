#include <pch.h>
#include "CustomTrainingStateMachine.h"

#include <sstream>
#include <iosfwd>

CustomTrainingStateMachine::CustomTrainingStateMachine(
	std::shared_ptr<CVarManagerWrapper> cvarManager, 
	std::shared_ptr<IStatUpdater> statUpdater,
	std::shared_ptr<IStatWriter> statWriter,
	std::shared_ptr<PluginState> pluginState)
	: _cvarManager( cvarManager )
	, _statUpdater( statUpdater )
	, _statWriter( statWriter )
	, _pluginState( pluginState )
{
	_currentState = CustomTrainingState::NotInCustomTraining;
}

void CustomTrainingStateMachine::hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper)
{
	// Happens whenever a goal was scored
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this, gameWrapper](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		auto gameServer = gameWrapper->GetGameEventAsServer();
		if (gameServer.IsNull()) { return; }
		auto ball = gameServer.GetBall();
		if (ball.IsNull()) { return; }

		_pluginState->setBallSpeed(ball.GetVelocity().magnitude());

		if (ball.GetLocation().Y > 0)
		{
			processOnHitGoal();
		}
	});

	// Happens whenever the ball is being touched
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnCarTouch", [this, gameWrapper](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		processOnCarTouch();
	});

	// Happens whenever a button was pressed after loading a new shot
	gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this, gameWrapper](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		processTrainingShotAttempt();
	});

	// Happens whenever a shot is changed or loaded in custom training
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GameEvent_TrainingEditor_TA.EventRoundChanged",
		[this, gameWrapper](ActorWrapper caller, void*, const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		TrainingEditorWrapper trainingWrapper(caller.memory_address);
		processEventRoundChanged(trainingWrapper);
	});

	// Happens whenever the current custom training map gets unloaded, e.g. because of leaving to the main menu or loading a different training pack
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed",
		[this, gameWrapper](ActorWrapper caller, void*, const std::string&) {

		// Finish the current attempt if an attempt was started, otherwise ignore the event
		if (_currentState == CustomTrainingState::AttemptInProgress)
		{
			TrainingEditorWrapper trainingWrapper(caller.memory_address);
			processEventRoundChanged(trainingWrapper);
		}

		// Set the training pack code to empty so a click on "Restore" won't do anything
		_statUpdater->publishTrainingPackCode({});
	});

	// Make sure the state machine has been properly initialized when the user (or the VS plugin project) reloads the plugin while being in custom training
	if (gameWrapper->IsInCustomTraining())
	{
		auto serverWrapper = gameWrapper->GetGameEventAsServer();
		if (serverWrapper.IsNull()) { return; }

		auto trainingWrapper = TrainingEditorWrapper(serverWrapper.memory_address);
		processOnTrainingModeLoaded(trainingWrapper);
		processEventRoundChanged(trainingWrapper);
	}
	
	// Note: The calling class hooks into OnTrainingModeLoaded
}

void CustomTrainingStateMachine::processOnTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper)
{
	// Jump to the resetting state from whereever we were before - it doesn't matter since we reset everything anyway
	setCurrentState(CustomTrainingState::Resetting);
	_pluginState->TotalRounds = trainingWrapper.GetTotalRounds();
	_pluginState->CurrentRoundIndex = -1;

	// The player reloaded the same, or loaded a different training pack => Reset statistics
	auto trainingPackCode = trainingWrapper.GetTrainingData().GetTrainingData().GetCode().ToString();
	_statUpdater->publishTrainingPackCode(trainingPackCode);
	_statUpdater->processReset(_pluginState->TotalRounds);

	// Initialize the data storage (most likely a file in the file system)
	_statWriter->initializeStorage(trainingPackCode);
	_statWriter->writeData();

}

void CustomTrainingStateMachine::processEventRoundChanged(TrainingEditorWrapper& trainingWrapper)
{
	auto newRoundIndex = trainingWrapper.GetActiveRoundNumber();
	if (_currentState == CustomTrainingState::Resetting)
	{
		// Automatic event after loading a training pack => Nothing special to be done
		setCurrentState(CustomTrainingState::PreparingNewShot);
	}
	else if (_currentState == CustomTrainingState::PreparingNewShot)
	{
		// The player must have switched to a different shot before starting their attempt
		if (_pluginState->CurrentRoundIndex == newRoundIndex)
		{
			// This could be a bug in the state machine: The player can't press reset before starting a new attempt, and can't switch to the same shot
			_cvarManager->log("[Custom Training State Machine] [WARNING] Detected an unexpected shot reset before starting an attempt.");
		}
	}
	else if (_currentState == CustomTrainingState::AttemptInProgress)
	{
		if (_goalWasScoredInCurrentAttempt)
		{
			// Temporarily enter pseudo state "Processing Goal"
			setCurrentState(CustomTrainingState::ProcessingGoal);
			_statUpdater->processGoal();
		}
		else
		{
			// Temporarily enter pseudo state "Processing Miss"
			setCurrentState(CustomTrainingState::ProcessingMiss);
			_statUpdater->processMiss();
		}
		
		// Automatically transition to the next state after updating calculations
		_statUpdater->updateData();
		setCurrentState(CustomTrainingState::PreparingNewShot);
	}
	// Else: Ignore the event. This e.g. happens before OnTrainingModeLoaded

	_pluginState->CurrentRoundIndex = newRoundIndex;
	
	// Store data at the begin of every round so we can try to restore data after a crash
	_statWriter->writeData();
}

void CustomTrainingStateMachine::processTrainingShotAttempt()
{
#if DEBUG_STATE_MACHINE
	if (_currentState != CustomTrainingState::PreparingNewShot)
	{
		_cvarManager->log("[Custom Training State Machine] [WARNING] Ignoring TrainingShotAttempt event while in " + to_string(_currentState));
		return;
	}
#endif

	setCurrentState(CustomTrainingState::AttemptInProgress);
	_goalWasScoredInCurrentAttempt = false;
	_ballWasHitInCurrentAttempt = false;
	_statUpdater->processAttempt();
}

void CustomTrainingStateMachine::processOnCarTouch()
{
	if (!_ballWasHitInCurrentAttempt)
	{
		_ballWasHitInCurrentAttempt = true;
		_statUpdater->processInitialBallHit();
	}
	// else: The ball was hit more often, or we are in goal replay => ignore the event
}

void CustomTrainingStateMachine::processOnHitGoal()
{
	if (!_goalWasScoredInCurrentAttempt)
	{
		_goalWasScoredInCurrentAttempt = true;

		// Note: We do not process the goal yet. This will happen when leaving the current state
	}
	// else: We are most likely in goal replay => ignore the event
}

void CustomTrainingStateMachine::setCurrentState(CustomTrainingState newState)
{
#if DEBUG_STATE_MACHINE
	std::ostringstream stream;
	stream << "[Custom Training State Machine] Transitioning from '" << to_string(_currentState) << "' to '" << to_string(newState) << "'";
	_cvarManager->log(stream.str());
#endif
	_currentState = newState;
}
