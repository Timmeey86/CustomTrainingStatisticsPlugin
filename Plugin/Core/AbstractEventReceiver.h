#pragma once

#include <bakkesmod/wrappers/GameEvent/TrainingEditorWrapper.h>

/** This class allows implementing only those event listener methods one is interested in.
 * The advantages to manually hooking into gametrainingWrapper events directly are:
 * - You can have several listeners for an event (gametrainingWrapper only allows a single listener)
 * - You get meaningful parameters since the calling class does the error handling and parameter extraction for you.
 */
class AbstractEventReceiver
{
protected:
	AbstractEventReceiver() = default;
	// Prevent slicing: If you need a copy, you need to copy the actual type rather than one of its interfaces
	AbstractEventReceiver(const AbstractEventReceiver&) = delete;
	AbstractEventReceiver(const AbstractEventReceiver&&) = delete;
	AbstractEventReceiver operator&(const AbstractEventReceiver&) = delete;

public:
	virtual ~AbstractEventReceiver() = default;

	/** This gets called whenever the user manually resets statistics. */
	virtual void onResetStatisticsTriggered() { /* ignore event unless overridden. */ }

	/** This gets called whenever the user restores the previous session. */
	virtual void onRestorePreviousSessionTriggered() { /* ignore event unless overridden. */ }

	/** This gets called whenever the user toggles the previous attempt between miss and goal. */
	virtual void onTogglePreviousAttemptTriggered() { /* ignore event unless overridden. */ }

	/** This gets called whenever the custom training mode gets loaded (also after reloading training, changing the pack etc). 
	 * Note that this also gets called when the user leaves training, with an empty training pack code in that case.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	trainingData		Provides information like the training pack code
	 */
	virtual void onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, TrainingEditorSaveDataWrapper* trainingData) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever the user either changed the shot, or reset the shot (or the game itself loaded the next shot after a goal replay)
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onRoundChanged(TrainingEditorWrapper& trainingWrapper) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever the user started the attempt by pressing pretty much anything on their controller or mouse/keyboard. */
	virtual void onAttemptStarted() { /* ignore event unless overridden. */ }

	/** This gets called whenever an attempt was reset with a goal happening during that attempt.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onAttemptFinishedWithGoal(TrainingEditorWrapper& trainingWrapper) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever an attempt was reset without any goal happening during that attempt.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onAttemptFinishedWithoutGoal(TrainingEditorWrapper& trainingWrapper) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever an attempt was finished, e.g. after resetting the shot, switching the shot or exiting training
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onAttemptFinished(TrainingEditorWrapper& trainingWrapper) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever a goal was scored (at most once during an attempt, so goal replay will not send these).
	 * Note that only goals in the orange goal are considered.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever the ball was hit during an attempt. 
	 * 
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	isInitialHit		true if this ist he first hit of the ball during the attempt.
	 */
	virtual void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) { (void)trainingWrapper; (void)isInitialHit; /* ignore event unless overridden. */ }

	/** This gets called whenever the ball touches the ground, excluding wall and ceiling.
	 * 
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	ball				Provides access to information about the ball. Goes out of scope after this call.
	 */
	virtual void onBallGroundHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) { (void)trainingWrapper; (void)ball; /* ignore event unless overridden. */ }

	/** This gets called whenever the ball touches the wall. Note that wall hits very close to the ground or the ceiling might not get classified as a wall hit.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	ball				Provides access to information about the ball. Goes out of scope after this call.
	 */
	virtual void onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) { (void)trainingWrapper; (void)ball; /* ignore event unless overridden. */ }

	/** This gets called whenever the ball touches the ceiling.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	ball				Provides access to information about the ball. Goes out of scope after this call.
	 */
	virtual void onBallCeilingHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) { (void)trainingWrapper; (void)ball; /* ignore event unless overridden. */ }

	/** This gets called whenever the ball touches the ground, wall or celiing. Note that this gets called in addition to the other onBallXYZHit hooks.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	ball				Provides access to information about the ball. Goes out of scope after this call.
	 */
	virtual void onBallSurfaceHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) { (void)trainingWrapper; (void)ball; /* ignore event unless overridden. */ }

	/** This gets called whenever the car lifts off the ground (or the ball!).
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	car					Provides access to information about the car. Goes out of scope after this call.
	*/
	virtual void onCarLiftOff(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) { (void)trainingWrapper; (void)car; /* ignore event unless overridden. */ }

	/** This gets called whenever the car "lands" on the ball, i.e. the player gets a flip reset on the ball.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	car					Provides access to information about the car. Goes out of scope after this call.
	 * \param	ball				Provides access to information about the ball. Goes out of scope after this call.
	*/
	virtual void onCarLandingOnBall(TrainingEditorWrapper& trainingWrapper, CarWrapper& car, BallWrapper& ball) { (void)trainingWrapper; (void)car; (void)ball; /* ignore event unless overridden. */ }

	/** This gets called whenever the car "lands" on the ground (excluding wall and ceiling)
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	car					Provides access to information about the car. Goes out of scope after this call.
	*/
	virtual void onCarLandingOnGround(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) { (void)trainingWrapper; (void)car; /* ignore event unless overridden. */ }

	/** This gets called whenever the car "lands" on the wall (also the walls inside the goal area)
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	car					Provides access to information about the car. Goes out of scope after this call.
	*/
	virtual void onCarLandingOnWall(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) { (void)trainingWrapper; (void)car; /* ignore event unless overridden. */ }

	/** This gets called whenever the car "lands" on the ceiling (this currently does not work for the goal ceiling part most likely.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	car					Provides access to information about the car. Goes out of scope after this call.
	*/
	virtual void onCarLandingOnCeiling(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) { (void)trainingWrapper; (void)car; /* ignore event unless overridden. */ }

	/** This gets called whenever the car lands on any surface which is not the ball. Note that this gets called in addition to the other onCarLandingXYZ hooks.
	 *
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	car					Provides access to information about the car. Goes out of scope after this call.
	*/
	virtual void onCarLandingOnSurface(TrainingEditorWrapper& trainingWrapper, CarWrapper& car) { (void)trainingWrapper; (void)car; /* ignore event unless overridden. */ }

	/** This gets called as a last actoin before an attempt is actually reset internally. This gives you a chance to do any calculations after any kind of stats have already been gathered. */
	virtual void attemptAboutToBeReset() { /* ignore event unless overridden. */ }
	/** This gets called whenever the player activates a flip, and that flip is currently allowed by the game. */
	virtual void onCarFlipped() { /* ignore event unless overridden.*/ }

};