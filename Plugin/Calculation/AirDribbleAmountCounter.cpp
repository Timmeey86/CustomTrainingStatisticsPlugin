#include <pch.h>
#include "AirDribbleAmountCounter.h"

AirDribbleAmountCounter::AirDribbleAmountCounter(
	std::function<void(int)> setMaxTouchAmountFunc, 
	std::function<void(float)> setMaxAirDribbleTimeFunc,
	std::function<void(int)> setMaxFlipResetsFunc)
	: _setMaxTouchAmountFunc(setMaxTouchAmountFunc)
	, _setMaxAirDribbleTimeFunc(setMaxAirDribbleTimeFunc)
	, _setMaxFlipResetsFunc(setMaxFlipResetsFunc)
{
}

void AirDribbleAmountCounter::onAttemptStarted()
{
	_currentState = AirDribbleState::ResetLocalMaximum;
	_maximumAmountOfTouches = 0;
	_maxAirDribbleTime = .0f;
	_maximumAmountOfFlipResets = 0;
	_flipResetState = FlipResetState::None;
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

		if (_firstBallTouchGameTime < .0f)
		{
			_firstBallTouchGameTime = trainingWrapper.GetTotalGameTimePlayed();
		}
		else
		{
			_lastBallTouchGameTime = trainingWrapper.GetTotalGameTimePlayed();
			auto dribbleDuration = _lastBallTouchGameTime - _firstBallTouchGameTime;
			if (dribbleDuration > _maxAirDribbleTime)
			{
				_maxAirDribbleTime = dribbleDuration;
				_setMaxAirDribbleTimeFunc(_maxAirDribbleTime);
			}
		}

		if (_flipResetState == FlipResetState::FlipActivated)
		{
			// The player got a flip reset, activated their flip and now touched the ball. This is the point where we consider it an actual flip reset
			_currentAmountOfFlipResets++;
			if (_currentAmountOfFlipResets > _maximumAmountOfFlipResets)
			{
				_maximumAmountOfFlipResets = _currentAmountOfFlipResets;
				_setMaxFlipResetsFunc(_maximumAmountOfFlipResets);
			}
			_flipResetState = FlipResetState::None;
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
	// any other state: if the player got a flip reset but didn't flip yet, it is too late
	_flipResetState = FlipResetState::None;
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

void AirDribbleAmountCounter::onCarLandingOnBall(TrainingEditorWrapper& trainingWrapper, CarWrapper& car, BallWrapper& ball)
{
	// only handle flip resets while both the car and the ball are in the air
	if (_currentState == AirDribbleState::CarInAir && _currentAmountOfTouches > 0)
	{
		_flipResetState = FlipResetState::FlipResetTriggered;
	}
}

void AirDribbleAmountCounter::onCarFlipped()
{
	if (_currentState == AirDribbleState::CarInAir && _flipResetState == FlipResetState::FlipResetTriggered)
	{
		_flipResetState = FlipResetState::FlipActivated;
	}
}

void AirDribbleAmountCounter::finishShot()
{
	_currentAmountOfTouches = 0;
	_firstBallTouchGameTime = -1.0f;
	_lastBallTouchGameTime = -1.0f;
	_currentAmountOfFlipResets = 0;
	_currentState = AirDribbleState::WaitingForTakeoff;
}