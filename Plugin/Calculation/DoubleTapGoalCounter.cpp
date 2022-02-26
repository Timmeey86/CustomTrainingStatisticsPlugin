#include <pch.h>
#include "DoubleTapGoalCounter.h"



// Allows passing DoubleTapGoalState to fmt::format
template <> struct fmt::formatter<DoubleTapGoalState> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin();
		it++;
		return it;
	}

	template <typename FormatContext>
	auto format(const DoubleTapGoalState& state, FormatContext& ctx) -> decltype(ctx.out()) {
		std::string str_rep;
		switch (state)
		{
		case DoubleTapGoalState::CarInAir:
			str_rep = "CarInAir";
			break;
		case DoubleTapGoalState::Final:
			str_rep = "Final";
			break;
		case DoubleTapGoalState::WaitingForBounce:
			str_rep = "WaitingForBounce";
			break;
		case DoubleTapGoalState::WaitingForDirectGoalInAir:
			str_rep = "WaitingForDirectGoalInAir";
			break;
		case DoubleTapGoalState::WaitingForDirectGoalOnGround:
			str_rep = "WaitingForDirectGoalOnGround";
			break;
		case DoubleTapGoalState::WaitingForDoubleTap:
			str_rep = "WaitingForDoubleTap";
			break;
		case DoubleTapGoalState::WaitingForGoal:
			str_rep = "WaitingForGoal";
			break;
		case DoubleTapGoalState::WaitingForTakeOff:
			str_rep = "WaitingForTakeOff";
			break;
		}
		return format_to(ctx.out(), "{}", str_rep);
	}
};

DoubleTapGoalCounter::DoubleTapGoalCounter(
	std::function<void()> notifyDoubleTapGoalFunc,
	std::shared_ptr<CVarManagerWrapper> cvarManager
	) :
	_notifyDoubleTapGoalFunc(notifyDoubleTapGoalFunc),
	_cvarManager(cvarManager)
{
}

void DoubleTapGoalCounter::onAttemptStarted()
{
	// If a new attempt was started, we reset the machine, no matter where it was before
	setCurrentState(DoubleTapGoalState::WaitingForTakeOff);
}

void DoubleTapGoalCounter::onCarLiftOff(TrainingEditorWrapper& trainingWrapper, CarWrapper& car)
{
	if (_currentState == DoubleTapGoalState::Final)
	{
		return;
	}

	if (_currentState == DoubleTapGoalState::WaitingForTakeOff)
	{
		setCurrentState(DoubleTapGoalState::CarInAir);
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDirectGoalOnGround)
	{
		setCurrentState(DoubleTapGoalState::WaitingForDirectGoalInAir);
	}
	// Any other state should be impossible since the player needs to land before taking off again, and landing will transition to either
	// WaitingForTakeOff or WaitingForDirectionalGoalOnGround in all cases.
}

void DoubleTapGoalCounter::onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit)
{
	(void)trainingWrapper;
	(void)isInitialHit;

	if (_currentState == DoubleTapGoalState::Final)
	{
		return;
	}

	if (_currentState == DoubleTapGoalState::CarInAir)
	{
		setCurrentState(DoubleTapGoalState::WaitingForBounce);
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDoubleTap)
	{
		setCurrentState(DoubleTapGoalState::WaitingForGoal);
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDirectGoalOnGround)
	{
		// player has messed up but might have another go at it
		setCurrentState(DoubleTapGoalState::WaitingForTakeOff); 
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDirectGoalInAir)
	{
		// player has messed up, and jumped before hitting the ball
		setCurrentState(DoubleTapGoalState::CarInAir);
	}
	// we don't care about ball touches in other states
}

void DoubleTapGoalCounter::onBallSurfaceHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	(void)trainingWrapper;
	(void)ball;

	if (_currentState == DoubleTapGoalState::Final)
	{
		return;
	}

	if (_currentState == DoubleTapGoalState::WaitingForBounce)
	{
		setCurrentState(DoubleTapGoalState::WaitingForDoubleTap);
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDirectGoalOnGround)
	{
		// player has messed up but might have another go at it
		setCurrentState(DoubleTapGoalState::WaitingForTakeOff);
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDirectGoalInAir)
	{
		// player has messed up, and jumped after landing
		setCurrentState(DoubleTapGoalState::CarInAir);
	}
	// we don't care about bounces in other states
}

void DoubleTapGoalCounter::onCarLandingOnSurface(TrainingEditorWrapper& trainingWrapper, CarWrapper& car)
{
	(void)trainingWrapper;
	(void)car;

	if (_currentState == DoubleTapGoalState::Final)
	{
		return;
	}

	if (_currentState == DoubleTapGoalState::WaitingForGoal)
	{
		setCurrentState(DoubleTapGoalState::WaitingForDirectGoalOnGround);
	}
	else if (_currentState == DoubleTapGoalState::WaitingForDirectGoalInAir)
	{
		setCurrentState(DoubleTapGoalState::WaitingForDirectGoalOnGround);
	}
	else
	{
		// Any other state: Whatever the player did, it doesn't count as a double tap
		setCurrentState(DoubleTapGoalState::WaitingForTakeOff);
	}
}


void DoubleTapGoalCounter::onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	(void)trainingWrapper;

	if (_currentState == DoubleTapGoalState::Final)
	{
		return;
	}

	if (_currentState == DoubleTapGoalState::WaitingForGoal ||
		_currentState == DoubleTapGoalState::WaitingForDirectGoalInAir ||
		_currentState == DoubleTapGoalState::WaitingForDirectGoalOnGround)
	{
		// Instead of actually entering the "NotifyListeners" state, we just call the notify function directly and finish the state machine
		_notifyDoubleTapGoalFunc();
	}
	// No matter which state we were, after scoring, we're not interested in any events until the player starts a new attempt
	setCurrentState(DoubleTapGoalState::Final);
}

void DoubleTapGoalCounter::setCurrentState(DoubleTapGoalState newState)
{
	//_cvarManager->log(fmt::format("DoubleTapGoalState: Transitioning from {} to {}", _currentState, newState));
	_currentState = newState;
}