#pragma once

#include <functional>
#include "../Core/AbstractEventReceiver.h"

/** Defines states of double tap scoring. */
enum class DoubleTapGoalState
{
	WaitingForTakeOff, ///< The initial state. Gets entered on a new attempt or after messing up.
	CarInAir, ///< The player lifted off the ground and hasn't landed since, but also hasn't touched the ball.
	WaitingForBounce, ///< The car is still in the air and the ball has been touched at least once, so we wait in this state for a wall/ceiling/ground touch
	WaitingForDoubleTap, ///< The car is still in the air, the ball has been touched, and it has bounced off the wall/ground/ceiling. We're waiting for the double tap now.
	WaitingForGoal, ///< The player has satisfied the double tap conditions. We'll wait to see if they can score it before their car lands.
	WaitingForDirectGoalOnGround, ///< The player has satisfied the double tap conditions, but then their car landed. We'll still count the goal in case the ball flies directly into it.
	WaitingForDirectGoalInAir, ///< Same, but the player took off again after landing.
	// Note: "NotifyListeners" is a pseudo state, we don't need it in the implementation
	Final, ///< A goal has been scored. We wait in this state for a new attempt
};

/** This class is responsible for tracking double tap goals.
 *
 * We currently define a double tap goal as this:
 * - The player must be in the air during the last touch before the wall bounce, and any touch between the wall bounce and the goal.
 * - The player may bounce the ball off the wall, ceiling or even ground so long as they stay in the air.
 * - The sequence must be: Car in air -> any # of ball touches -> wall/ceiling/ground bounce -> any # of ball touches -> goal
 * - Landing on the ground/wall/ceiling with all four wheels cancels the attempt.
 *   Exception: Landing after the double tap will be allowed if the ball goes directly into the goal after that, without touching wall/ceiling/ground
 * 
 * The states and transitions are defined in /img/DoubleTapStateChart.png
 */
class DoubleTapGoalCounter : public AbstractEventReceiver
{
public:

	/** Creates an object which calls a notify function whenever the player scores a double tap goal. */
	explicit DoubleTapGoalCounter(std::function<void()> notifyDoubleTapGoalFunc, std::shared_ptr<CVarManagerWrapper> cvarManager);

	void onAttemptStarted() override;
	void onCarLiftOff(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) override;
	void onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) override;
	void onBallSurfaceHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onCarLandingOnSurface(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) override;
private:

	/** Applies the new state, and potentially does logging. */
	void setCurrentState(DoubleTapGoalState newState);

	std::function<void()> _notifyDoubleTapGoalFunc; ///< This function will be called once a double tap goal has been scored.
	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Used for logging only
	DoubleTapGoalState _currentState = DoubleTapGoalState::WaitingForTakeOff;
};