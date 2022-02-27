#pragma once

#include <functional>
#include "../Core/AbstractEventReceiver.h"

/** Defines states of air dribbling. */
enum class AirDribbleState
{
	WaitingForTakeoff,  ///< We're waiting for the player to lift off the ground/wall/ceiling.
	CarInAir,	  ///< The car is in the air, and the ball hasn't touched the ground since.
	BallBouncedOnce, ///< The ball has bounced once, while the car was in the air. We allow this so you can try and air dribble off bounces.
	BallOnGround, ///< The ball touched the ground/wall/ceiling twice. We're waiting for the car to land in order for trying a 2nd time maybe.
	ResetLocalMaximum, ///< The player reset/changed the shot and started a new attempt.
};

/** This class keeps track of the highest amount of air dribble touches during an attempt.
 *
 *	Counting starts when the car lifts off the ground, and stops when the car touches lands with all four wheels on the ground, a wall or the ceiling,
 *  or when the ball touches the ground. In the latter case, counting will continue once the car has landed and lifted off again.
 *  Note that the ball touching the wall or ceiling doesn't reset the counter since technically you could still continue the same dribble after that.
 * 
 *  States and transitions are documented in /img/AirDribbleStateChart.png
 */
class AirDribbleAmountCounter : public AbstractEventReceiver
{
public:
	/** Creates a new air dribble amount counter, which will call setMaxTouchAmountFunc each time a new maximum has been reached during the current attempt. */
	AirDribbleAmountCounter(
		std::function<void(int)> setMaxTouchAmountFunc,
		std::function<void(float)> setMaxAirDribbleTimeFunc,
		std::function<void(int)> setMaxFlipResetsFunc);

	// Resets the touch counter whenever a new attempt starts, and treats the car as being on the ground.
	void onAttemptStarted() override;

	// Counts a ball touch, in case the car is in the air, and the ball hasn't touched the ground more than once since the last touch
	void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) override;
	// Resets the ball touches, until the car has landed and lifted off again.
	void onBallSurfaceHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	// Counting will only start after this has been called. 
	void onCarLiftOff(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) override;
	// Resets the counters and stops counting ball hits until the car has lifted off again.
	void onCarLandingOnSurface(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) override;
	// Remembers flip resets
	void onCarLandingOnBall(TrainingEditorWrapper& trainingWrapper, CarWrapper& car, BallWrapper& ball) override;
	// Actually increments the flip reset counter: Only count the flip reset if a flip comes later
	void onCarFlipped() override;

private:
	/** Resets the current touch amount and transitions back to WaitingForTakeoff state. */
	void finishShot();

	int _currentAmountOfTouches = 0; ///< The current amount of ball touches after lifting off the ground.
	int _maximumAmountOfTouches = 0; ///< The maximum amount of touches during the current attempt.
	std::function<void(int)> _setMaxTouchAmountFunc; ///< The function to be called when a new maximum amount of touches has been reached.

	float _firstBallTouchGameTime = -1.0f; ///< The point in time where the car lifted off.
	float _lastBallTouchGameTime = -1.0F; ///< The point in time where the car last touched the ball (while dribbling).
	float _maxAirDribbleTime = .0f; ///< The maximum air dribble duration between the first and the last touch.
	std::function<void(float)> _setMaxAirDribbleTimeFunc; ///< The function to be called when a new maximum air dribble time has been reached.
	
	int _currentAmountOfFlipResets = 0; ///< The current amount of flip resets while in the air.
	int _maximumAmountOfFlipResets = 0; ///< The maximum amount of flip resets during the current attempt.
	std::function<void(int)> _setMaxFlipResetsFunc; ///< The function to be called when a new maximum flip reset amount has been reached.

	bool _waitingForFlip = false; ///< This is used in order to count a flip only if you get the reset, and then flip after that (before the ball touches ground/ceiling/walls)
	AirDribbleState _currentState = AirDribbleState::WaitingForTakeoff;
};