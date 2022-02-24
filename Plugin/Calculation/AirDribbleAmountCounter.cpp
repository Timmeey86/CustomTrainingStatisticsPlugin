#include <pch.h>
#include "AirDribbleAmountCounter.h"

AirDribbleAmountCounter::AirDribbleAmountCounter(std::function<void(int)> setMaxTouchAmountFunc)
	: _setMaxTouchAmountFunc(setMaxTouchAmountFunc)
{
}

void AirDribbleAmountCounter::onAttemptStarted()
{
	_currentState = AirDribbleState::ResetLocalMaximum;
	_maximumAmountOfTouches = 0;
	finishShot();
}

void AirDribbleAmountCounter::onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit)
{
	if (_currentState == AirDribbleState::BallBouncedOnce)
	{
		// The ball was touched after a single bounce => count this touch and any further touches, and allow another bounce
		_currentState = AirDribbleState::CarInAir;
	}
	// This is not an else if intentionally!
	if (_currentState == AirDribbleState::CarInAir)
	{
		_currentAmountOfTouches++;
		if (_currentAmountOfTouches > _maximumAmountOfTouches) 
		{
			_maximumAmountOfTouches = _currentAmountOfTouches;
			_setMaxTouchAmountFunc(_maximumAmountOfTouches);
		}
	}
	// else: Ignore the ball hit in any other state
}

void AirDribbleAmountCounter::onBallSurfaceHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	if (_currentState == AirDribbleState::CarInAir)
	{
		// Allow one bounce
		_currentState = AirDribbleState::BallBouncedOnce;
	}
	else if (_currentState == AirDribbleState::BallBouncedOnce)
	{
		// This is the second ball touch => The player needs to land and lift off again
		_currentState = AirDribbleState::BallOnGround;
	}
	// any other state: ignore this
}

void AirDribbleAmountCounter::onCarLiftOff(TrainingEditorWrapper& trainingWrapper, CarWrapper& car)
{
	if (_currentState == AirDribbleState::WaitingForTakeoff)
	{
		_currentState = AirDribbleState::CarInAir;
	}
	// else: shouldn't be possible in theory. If it happens for whatever reason: ignore
}

void AirDribbleAmountCounter::onCarLandingOnSurface(TrainingEditorWrapper& trainingWrapper, CarWrapper& car)
{
	// no matter where we were, we need to wait for lift off now
	finishShot();
}

void AirDribbleAmountCounter::finishShot()
{
	_currentAmountOfTouches = 0;
	_currentState = AirDribbleState::WaitingForTakeoff;
}