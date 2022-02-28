#include <pch.h>
#include "GroundDribbleTimeCounter.h"

GroundDribbleTimeCounter::GroundDribbleTimeCounter(std::function<void(float)> setMaxGroundDribbleTimeFunc)
	: _setMaxGroundDribbleTimeFunc( setMaxGroundDribbleTimeFunc )
{
}

void GroundDribbleTimeCounter::onAttemptStarted()
{
	_currentState = GroundDribbleState::WaitingForInitialTouch;
	_maxGroundDribbleTime = .0f;
	_firstBallTouchGameTime = .0f;
}

void GroundDribbleTimeCounter::onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit)
{
	if (_currentState == GroundDribbleState::WaitingForInitialTouch)
	{
		_firstBallTouchGameTime = trainingWrapper.GetTotalGameTimePlayed();
		_currentState = GroundDribbleState::BallTouchedOnce;
	}
	else
	{
		if (_currentState == GroundDribbleState::BallTouchedOnce)
		{
			_currentState = GroundDribbleState::Dribbling;
		}
		auto timeDifference = trainingWrapper.GetTotalGameTimePlayed() - _firstBallTouchGameTime;
		if (timeDifference > _maxGroundDribbleTime)
		{
			_maxGroundDribbleTime = timeDifference;
			_setMaxGroundDribbleTimeFunc(timeDifference);
		}
	}
}

void GroundDribbleTimeCounter::onBallGroundHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	_currentState = GroundDribbleState::WaitingForInitialTouch;
	_firstBallTouchGameTime = .0f;
}
