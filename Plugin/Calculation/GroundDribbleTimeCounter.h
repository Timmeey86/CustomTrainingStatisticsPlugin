#pragma once

#include <functional>
#include "../Core/AbstractEventReceiver.h"


/** Defines states of ground dribbling. */
enum class GroundDribbleState
{
	WaitingForInitialTouch, ///< The car has not touched the ball since the start, or since the ball hit the ground
	BallTouchedOnce, ///< The car has touched the ball once. We remember this as the start time, but we don't know yet whether or not the player is dribbling
	Dribbling, ///< The car has touched the ball at least twice, without the ball hitting the ground since. We're updating the time on every touch.
};

/** This class keeps track of the longest time the player carried the ball during the current attempt.
 *
 * Counting starts when the car hits the ball twice, without the ball hitting the ground between the two touches.
 * From that point on, we count the time between the first touch, and the current one, each time the car touches the ball, until the ball touches the ground.
 */
class GroundDribbleTimeCounter : public AbstractEventReceiver
{
public:
	/** Creates a new ground dribble amount counter, which will call setMaxGroundTimeFunc whenever a new maximum has been reached during the current attempt. */
	explicit GroundDribbleTimeCounter(std::function<void(float)> setMaxGroundDribbleTimeFunc);


	// Resets the time counter whenever a new attempt starts, and treats the car and ball as being on the ground
	void onAttemptStarted() override;

	// Remembers the time of the first ball hit since the ball last touched the ground, and measures the different to the most recent ball touch since,
	// starting from the second touch
	void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) override;

	// Stops counting dribbling time
	void onBallGroundHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;

private:
	float _firstBallTouchGameTime = -1.0f; ///< The point in time where the car first touched the ball (since it hit the ground)
	float _maxGroundDribbleTime = .0f; ///< The maximum air dribble duration between the first and the last touch.
	std::function<void(float)> _setMaxGroundDribbleTimeFunc; ///< The function to be called when a new maximum air dribble time has been reached.

	GroundDribbleState _currentState = GroundDribbleState::WaitingForInitialTouch;
};