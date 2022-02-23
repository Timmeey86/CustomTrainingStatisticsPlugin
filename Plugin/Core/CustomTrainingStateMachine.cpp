#include <pch.h>
#include "CustomTrainingStateMachine.h"

#include <sstream>
#include <iosfwd>

CustomTrainingStateMachine::CustomTrainingStateMachine(
	std::shared_ptr<CVarManagerWrapper> cvarManager, 
	std::shared_ptr<IStatWriter> statWriter,
	std::shared_ptr<PluginState> pluginState)
	: _cvarManager( cvarManager )
	, _statWriter( statWriter )
	, _pluginState( pluginState )
{
	_currentState = CustomTrainingState::NotInCustomTraining;
}

void CustomTrainingStateMachine::hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	// Happens whenever a goal was scored
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this, gameWrapper, eventReceivers](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		// Prevent additional goal events which occur during goal replay from being processed
		// The variable will be reset when the player starts the next attempt
		if (!_goalWasScoredInCurrentAttempt)
		{
			auto gameServer = gameWrapper->GetGameEventAsServer();
			if (gameServer.IsNull()) { return; }
			auto ball = gameServer.GetBall();
			if (ball.IsNull()) { return; }
			TrainingEditorWrapper trainingWrapper(gameServer.memory_address);
			if (trainingWrapper.IsNull()) { return; }

			_pluginState->setBallSpeed(ball.GetVelocity().magnitude());

			if (ball.GetLocation().Y > 0)
			{
				processOnHitGoal(trainingWrapper, eventReceivers);
			}
		}
	});

	// Happens whenever the ball is being touched
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnCarTouch", [this, gameWrapper, eventReceivers](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		auto gameServer = gameWrapper->GetGameEventAsServer();
		if (gameServer.IsNull()) { return; }
		TrainingEditorWrapper trainingWrapper(gameServer.memory_address);
		if (trainingWrapper.IsNull()) { return; }

		processOnCarTouch(trainingWrapper, eventReceivers);
	});

	// Happens whenever a button was pressed after loading a new shot
	gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this, gameWrapper, eventReceivers](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		auto gameServer = gameWrapper->GetGameEventAsServer();
		if (gameServer.IsNull()) { return; }
		TrainingEditorWrapper trainingWrapper(gameServer.memory_address);
		if (trainingWrapper.IsNull()) { return; }

		processTrainingShotAttempt(trainingWrapper, eventReceivers);
	});

	// Happens whenever a shot is changed or loaded in custom training
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GameEvent_TrainingEditor_TA.EventRoundChanged",
		[this, gameWrapper, eventReceivers](ActorWrapper caller, void*, const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }

		TrainingEditorWrapper trainingWrapper(caller.memory_address);
		processEventRoundChanged(trainingWrapper, eventReceivers);
	});

	// Happens whenever the current custom training map gets unloaded, e.g. because of leaving to the main menu or loading a different training pack
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed",
		[this, gameWrapper, eventReceivers](ActorWrapper caller, void*, const std::string&) {

		TrainingEditorWrapper trainingWrapper(caller.memory_address);
		if (trainingWrapper.IsNull()) { return; }

		// Finish the current attempt if an attempt was started, otherwise ignore the event
		if (_currentState == CustomTrainingState::AttemptInProgress)
		{
			processEventRoundChanged(trainingWrapper, eventReceivers);
		}

		// Set the training pack code to empty so a click on "Restore" won't do anything
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onTrainingModeLoaded(trainingWrapper, {});
		}
	});

	// Happens whenever the ball touches the ground, the wall, or the ceiling. 
	gameWrapper->HookEventWithCallerPost<BallWrapper>("Function TAGame.Ball_TA.IsGroundHit",
		[this, gameWrapper, eventReceivers](BallWrapper ball, void*, const std::string&) {
		// We only process this while an attempt is active, in order to exclude goal replay etc
		if (!_pluginState->PluginIsEnabled || _currentState != CustomTrainingState::AttemptInProgress) { return; }

		auto gameServer = gameWrapper->GetGameEventAsServer();
		if (gameServer.IsNull()) { return; }
		TrainingEditorWrapper trainingWrapper(gameServer.memory_address);
		if (trainingWrapper.IsNull()) { return; }

		if (ball.IsNull()) { return; }

		auto location = ball.GetLocation();

		// When the ball touches the ground, it mostly has  Z of about 93.5, but sometimes it jumps to 95 or even 97, dependent on when the event comes.
		// TODO: Is there a more reliable location in the event parameters maybe?
		if (location.Z <= 100.0f)
		{
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onBallGroundHit(trainingWrapper, ball);
			}
		}
		else if (location.Z >= 1950.0f)
		{
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onBallCeilingHit(trainingWrapper, ball);
			}
		}
		else
		{
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onBallWallHit(trainingWrapper, ball);
			}
		}
		_cvarManager->log(fmt::format("X: {}, Y: {}, Z: {}", location.X, location.Y, location.Z));
	});

	// Make sure the state machine has been properly initialized when the user (or the VS plugin project) reloads the plugin while being in custom training
	if (gameWrapper->IsInCustomTraining())
	{
		auto serverWrapper = gameWrapper->GetGameEventAsServer();
		if (serverWrapper.IsNull()) { return; }

		auto trainingWrapper = TrainingEditorWrapper(serverWrapper.memory_address);
		auto trainingPackCode = trainingWrapper.GetTrainingData().GetTrainingData().GetCode().ToString();
		processOnTrainingModeLoaded(trainingWrapper, trainingPackCode, eventReceivers);
		processEventRoundChanged(trainingWrapper, eventReceivers);
	}
	
	// Note: The calling class hooks into OnTrainingModeLoaded
}

void CustomTrainingStateMachine::processOnTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	// Jump to the resetting state from whereever we were before - it doesn't matter since we reset everything anyway
	setCurrentState(CustomTrainingState::Resetting);
	_pluginState->TotalRounds = trainingWrapper.GetTotalRounds();
	_pluginState->CurrentRoundIndex = -1;

	// The player reloaded the same, or loaded a different training pack => Reset statistics
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onTrainingModeLoaded(trainingWrapper, trainingPackCode);
	}

	// Initialize the data storage (most likely a file in the file system)
	_statWriter->initializeStorage(trainingPackCode);
	_statWriter->writeData();

}

void CustomTrainingStateMachine::processEventRoundChanged(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
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
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onAttemptFinishedWithGoal(trainingWrapper);
			}
		}
		else
		{
			// Temporarily enter pseudo state "Processing Miss"
			setCurrentState(CustomTrainingState::ProcessingMiss);
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onAttemptFinishedWithoutGoal(trainingWrapper);
			}
		}
		
		// Automatically transition to the next state after updating calculations
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onAttemptFinished(trainingWrapper);
		}
		setCurrentState(CustomTrainingState::PreparingNewShot);
	}
	// Else: Ignore the event. This e.g. happens before OnTrainingModeLoaded

	_pluginState->CurrentRoundIndex = newRoundIndex;
	
	// Store data at the begin of every round so we can try to restore data after a crash
	_statWriter->writeData();
}

void CustomTrainingStateMachine::processTrainingShotAttempt(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
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

	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onAttemptStarted();
	}
}

void CustomTrainingStateMachine::processOnCarTouch(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onBallHit(trainingWrapper, !_ballWasHitInCurrentAttempt);
	}

	if (!_ballWasHitInCurrentAttempt)
	{
		_ballWasHitInCurrentAttempt = true;
	}
	// else: The ball was hit more often, or we are in goal replay => nothing to do here, but others might want to know

}

void CustomTrainingStateMachine::processOnHitGoal(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	_goalWasScoredInCurrentAttempt = true;

	// Note: We do not process the goal yet. This will happen when leaving the current state
	
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onGoalScored(trainingWrapper);
	}
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
