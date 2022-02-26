#include <pch.h>
#include "CloseMissCounter.h"

const auto GoalYThreshold = 5000.0f;

CloseMissCounter::CloseMissCounter(std::function<void()> notifyCloseMissFunc)
	: _notifyCloseMissFunc(notifyCloseMissFunc)
{
}

void CloseMissCounter::onAttemptStarted()
{
	_currentState = CloseMissState::WaitingForBackboardTouchNearGoal;
}

void CloseMissCounter::onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	// only ever react to one close miss per attempt (we're not counting the amount of misses in one attempt)
	if (_currentState != CloseMissState::WaitingForBackboardTouchNearGoal)
	{
		return;
	}

	auto location = ball.GetLocation();
	if (location.Y > GoalYThreshold && abs(location.X) < 1200.0f && location.Z < 900.0f)
	{
		// The ball bounced off the backboard near the orange goal (or the goal posts)
		_currentState = CloseMissState::WaitingForEndOfAttempt;
	}
}

void CloseMissCounter::onAttemptFinishedWithoutGoal(TrainingEditorWrapper& trainingWrapper)
{
	// The player reset the shot, changed the shot, or let the timer run out and watched the full goal replay
	// The player didn't score in the shot before
	if (_currentState == CloseMissState::WaitingForEndOfAttempt)
	{
		// A close miss happened in this attempt
		_notifyCloseMissFunc();
		// Note: onAttemptStarted will reset to the initial state
	}
}
