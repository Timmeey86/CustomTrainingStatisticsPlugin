#include <pch.h>
#include "CustomTrainingStateMachine.h"

#include <sstream>
#include <iosfwd>

CustomTrainingStateMachine::CustomTrainingStateMachine(std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<IStatUpdater> statUpdater)
	: _cvarManager( cvarManager )
	, _statUpdater( statUpdater )
{
	_currentState = CustomTrainingState::NotInCustomTraining;
}

// TODO. Note: Checking for active Goal Replay is no longer required
bool statUpdatesShallBeSent() { return true; }
bool pluginIsEnabled() { return true; }

void CustomTrainingStateMachine::hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper)
{
	// TODO: Can we detect a plugin reload while in custom training?

	// Happens whenever a goal was scored
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		processOnHitGoal();
	});

	// Happens whenever the ball is being touched
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnCarTouch", [this](const std::string&) {
		if (!statUpdatesShallBeSent()) { return; }

		processOnCarTouch();
	});

	// Happens whenever a button was pressed after loading a new shot
	gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this](const std::string&) {
		if (!pluginIsEnabled()) { return; }

		processTrainingShotAttempt();
	});

	// Happens whenever a shot is changed or loaded in custom training
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GameEvent_TrainingEditor_TA.EventRoundChanged",
		[this](ActorWrapper caller, void*, const std::string&) {
		if (!pluginIsEnabled()) { return; }

		TrainingEditorWrapper trainingWrapper(caller.memory_address);
		processsEventRoundChanged(trainingWrapper);
	});

	// Note: The calling class hooks into OnTrainingModeLoaded
}

void CustomTrainingStateMachine::processOnTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper)
{
	// Jump to the resetting state from whereever we were before - it doesn't matter since we reset everything anyway
	setCurrentState(CustomTrainingState::Resetting);
	_totalRounds = trainingWrapper.GetTotalRounds();
	_currentRoundIndex = -1;

	// The player reloaded the same, or loaded a different training pack => Reset statistics
#if DEBUG_STATE_MACHINE
	_cvarManager->log("[Custom Training State Machine] Resetting statistics");
#endif
}

void CustomTrainingStateMachine::processsEventRoundChanged(TrainingEditorWrapper& trainingWrapper)
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

#if DEBUG_STATE_MACHINE
		if (_currentRoundIndex == newRoundIndex)
		{
			// This could be a bug in the state machine: The player can't press reset before starting a new attempt, and can't switch to the same shot
			_cvarManager->log("[Custom Training State Machine] [WARNING] Detected an unexpected shot reset before starting an attempt.");
		}
#endif
	}
	else if (_currentState == CustomTrainingState::AttemptInProgress)
	{
		if (_goalWasScoredInCurrentAttempt)
		{
			// Temporarily enter pseudo state "Processing Goal"
			setCurrentState(CustomTrainingState::ProcessingGoal);
#if DEBUG_STATE_MACHINE
			_cvarManager->log("[Custom Training State Machine] Previous attempt resulted in a goal");
#endif
		}
		else
		{
			// Temporarily enter pseudo state "Processing Miss"
			setCurrentState(CustomTrainingState::ProcessingMiss);
#if DEBUG_STATE_MACHINE
			_cvarManager->log("[Custom Training State Machine] Previous attempt resulted in a miss");
#endif
		}
#if DEBUG_STATE_MACHINE
		_cvarManager->log("[Custom Training State Machine] Updating calculations");
#endif
		// Automatically transition to the next state
		setCurrentState(CustomTrainingState::PreparingNewShot);
	}
	// Else: Ignore the event. This e.g. happens before OnTrainingModeLoaded

	_currentRoundIndex = newRoundIndex;
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
}

void CustomTrainingStateMachine::processOnCarTouch()
{
	if (!_ballWasHitInCurrentAttempt)
	{
		_ballWasHitInCurrentAttempt = true;
#if DEBUG_STATE_MACHINE
		_cvarManager->log("[Custom Training State Machine] Processing initial ball hit");
#endif
	}
	// else: The ball was hit more often, or we are in goal replay => ignore the event
}

void CustomTrainingStateMachine::processOnHitGoal()
{
	if (!_goalWasScoredInCurrentAttempt)
	{
		_goalWasScoredInCurrentAttempt = true;
#if DEBUG_STATE_MACHINE
		_cvarManager->log("[Custom Training State Machine] Processing goal within current attempt");
#endif
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
