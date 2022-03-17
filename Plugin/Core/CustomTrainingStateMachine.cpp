#include <pch.h>
#include "CustomTrainingStateMachine.h"

#include <sstream>
#include <iosfwd>

CustomTrainingStateMachine::CustomTrainingStateMachine(
	std::shared_ptr<CVarManagerWrapper> cvarManager,
	std::shared_ptr<IStatWriter> statWriter,
	std::shared_ptr<AllTimePeakHandler> peakHandler,
	std::shared_ptr<PluginState> pluginState)
	: _cvarManager(cvarManager)
	, _statWriter(statWriter)
	, _peakHandler(peakHandler)
	, _pluginState(pluginState)
{
	_currentState = CustomTrainingState::NotInCustomTraining;
}

// Allows passing Vectors to fmt::format
template <> struct fmt::formatter<Vector> {
	char presentation = 'f';

	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin();
		it++;
		return it;
	}

	template <typename FormatContext>
	auto format(const Vector& v, FormatContext& ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "X: {}, Y : {}, Z : {}", v.X, v.Y, v.Z);
	}
};

// Calculates the distance between two vectors
float distance(const Vector& v1, const Vector& v2)
{
	return fabsf(
		sqrtf(
			powf(v2.X - v1.X, 2.0f) +
			powf(v2.Y - v1.Y, 2.0f) +
			powf(v2.Z - v1.Z, 2.0f)
		)
	);
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
				processOnHitGoal(trainingWrapper, ball, eventReceivers);
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
		if (!gameWrapper->IsInCustomTraining() || _currentState != CustomTrainingState::AttemptInProgress) { return; }

		auto gameServer = gameWrapper->GetGameEventAsServer();
		if (gameServer.IsNull()) { return; }
		TrainingEditorWrapper trainingWrapper(gameServer.memory_address);
		if (trainingWrapper.IsNull()) { return; }
		if (ball.IsNull()) { return; }

		processBallSurfaceHit(ball, eventReceivers, trainingWrapper);

	});

	// Happens whenever the car lifts off the ground, wall or ceiling and then "lands" on any of these again 
	gameWrapper->HookEventWithCallerPost<CarWrapper>("Function TAGame.Car_TA.OnGroundChanged",
		[this, gameWrapper, eventReceivers](CarWrapper car, void*, const std::string&) {

		// We only process this while an attempt is active, in order to exclude goal replay etc
		if (!gameWrapper->IsInCustomTraining() || _currentState != CustomTrainingState::AttemptInProgress) { return; }

		auto gameServer = gameWrapper->GetGameEventAsServer();
		if (gameServer.IsNull()) { return; }
		TrainingEditorWrapper trainingWrapper(gameServer.memory_address);
		if (trainingWrapper.IsNull()) { return; }

		processOnGroundChanged(car, trainingWrapper, eventReceivers);
	});


	// Make sure the state machine has been properly initialized when the user (or the VS plugin project) reloads the plugin while being in custom training
	if (gameWrapper->IsInCustomTraining())
	{
		auto serverWrapper = gameWrapper->GetGameEventAsServer();
		if (serverWrapper.IsNull()) { return; }

		auto trainingWrapper = TrainingEditorWrapper(serverWrapper.memory_address);
		auto trainingData = trainingWrapper.GetTrainingData().GetTrainingData();
		processOnTrainingModeLoaded(trainingWrapper, &trainingData, eventReceivers);
		processEventRoundChanged(trainingWrapper, eventReceivers);
	}

	// Note: The calling class hooks into OnTrainingModeLoaded
}

void CustomTrainingStateMachine::processBallSurfaceHit(BallWrapper& ball, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers, TrainingEditorWrapper& trainingWrapper)
{
	if (!_pluginState->StatsShallBeRecorded) { return; }

	// When the ball touches the ground, it mostly has  Z of about 93.5, but sometimes it jumps to 95 or even 97, dependent on when the event comes.
	if (auto location = ball.GetLocation(); location.Z <= 100.0f)
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
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onBallSurfaceHit(trainingWrapper, ball);
	}
}

void CustomTrainingStateMachine::processOnGroundChanged(CarWrapper& car, TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	if (!_pluginState->StatsShallBeRecorded) { return; }

	if (!car.IsOnGround() && !car.IsOnWall())
	{
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onCarLiftOff(trainingWrapper, car);
		}
		return;
	}

	if (auto ball = trainingWrapper.GetBall();
		!ball.IsNull() && distance(car.GetLocation(), ball.GetLocation()) < 108.0f)
	{
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onCarLandingOnBall(trainingWrapper, car, ball);
		}
		return;
	}

	if (car.IsOnWall())
	{
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onCarLandingOnWall(trainingWrapper, car);
		}
	}
	else if (car.IsOnGround())
	{
		if (car.GetLocation().Z >= 1950.0f)
		{
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onCarLandingOnCeiling(trainingWrapper, car);
			}
		}
		else
		{
			for (auto eventReceiver : eventReceivers)
			{
				eventReceiver->onCarLandingOnGround(trainingWrapper, car);
			}
		}
	}
	// Send an additional event for listeners only interested in the car landing anywhere (except for the ball)
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onCarLandingOnSurface(trainingWrapper, car);
	}
}

void CustomTrainingStateMachine::processOnTrainingModeLoaded(
	TrainingEditorWrapper& trainingWrapper, 
	TrainingEditorSaveDataWrapper* trainingData,
	const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	// Jump to the resetting state from whereever we were before - it doesn't matter since we reset everything anyway
	setCurrentState(CustomTrainingState::Resetting);
	_pluginState->TotalRounds = trainingWrapper.GetTotalRounds();
	_pluginState->CurrentRoundIndex = -1;

	// The player reloaded the same, or loaded a different training pack => Reset statistics
	// We forward this event even if stat recording is turned off since event receivers might have to do initialization here, 
	// and hopefully nobody will be counting the amount of training packs loaded per day or anything.
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->onTrainingModeLoaded(trainingWrapper, trainingData);
	}

	if (trainingData != nullptr)
	{
		// Initialize the data storage (most likely a file in the file system)
		// We do this even if stat recording is turned off since the player could turn it on any time.
		_statWriter->initializeStorage(trainingData->GetCode().ToString());
		_statWriter->writeData();
	}

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
		if (_pluginState->StatsShallBeRecorded)
		{
			processGoalOrMiss(eventReceivers, trainingWrapper);
		}
		// advance the state machine even if stats recording is turned off so switching it on in the middle of a shot will still work
		setCurrentState(CustomTrainingState::PreparingNewShot);
	}
	// Else: Ignore the event. This e.g. happens before OnTrainingModeLoaded

	_pluginState->CurrentRoundIndex = newRoundIndex;

	if (_pluginState->StatsShallBeRecorded)
	{
		// Store data at the begin of every round so we can try to restore data after a crash
		_statWriter->writeData();
		_peakHandler->updateMaximumStats();
	}
}

void CustomTrainingStateMachine::processGoalOrMiss(const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers, TrainingEditorWrapper& trainingWrapper)
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
	for (auto eventReceiver : eventReceivers)
	{
		eventReceiver->attemptAboutToBeReset();
	}
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

	if (_pluginState->StatsShallBeRecorded)
	{
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onAttemptStarted();
		}
	}
}

void CustomTrainingStateMachine::processOnCarTouch(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	if (_pluginState->StatsShallBeRecorded)
	{
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onBallHit(trainingWrapper, !_ballWasHitInCurrentAttempt);
		}
	}

	if (!_ballWasHitInCurrentAttempt)
	{
		_ballWasHitInCurrentAttempt = true;
	}
	// else: The ball was hit more often, or we are in goal replay => nothing to do here, but others might want to know

}

void CustomTrainingStateMachine::processOnHitGoal(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers)
{
	_goalWasScoredInCurrentAttempt = true;

	// Note: We do not process the goal yet. This will happen when leaving the current state

	if (_pluginState->StatsShallBeRecorded)
	{
		for (auto eventReceiver : eventReceivers)
		{
			eventReceiver->onGoalScored(trainingWrapper, ball);
		}
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
