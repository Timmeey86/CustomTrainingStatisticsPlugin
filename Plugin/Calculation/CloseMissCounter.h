#pragma once

#include <functional>
#include <vector>
#include "../Core/AbstractEventReceiver.h"

/** Defines states of a close miss. */
enum class CloseMissState
{
	WaitingForBackboardTouchNearGoal, ///< Most of the time we'll be in this state
	WaitingForEndOfAttempt, ///< The ball has touched the wall or the goal posts close to the backboard. If the attempt ends without a goal, we will treat this as a close miss.
};

/** This class is reponsible for detecting close misses. 
 *
 * A close miss is a goal post hit, or a bounce against the backboard, which then does not result in a goal.
 */
class CloseMissCounter : public AbstractEventReceiver
{
public:
	/** Creates an object which calls notifyCloseMissFunc at the end of an attempt if the ball bounced near the goal, but didn't get in. */
	explicit CloseMissCounter(std::function<void()> notifyCloseMissFunc);

	void onAttemptStarted() override;
	void onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onAttemptFinishedWithoutGoal(TrainingEditorWrapper& trainingWrapper) override;

private:
	std::function<void()> _notifyCloseMissFunc; ///< This function will be called at the end of an attempt when a close miss occurred and the player didn't score after that
	CloseMissState _currentState;
};