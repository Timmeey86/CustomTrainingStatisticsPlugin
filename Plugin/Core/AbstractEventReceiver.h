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
	 * \param	trainingPackCode	The code of the new training pack.
	 */
	virtual void onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode) { (void)trainingWrapper; /* ignore event unless overridden. */ }

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
	virtual void onGoalScored(TrainingEditorWrapper& trainingWrapper) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever the ball was hit during an attempt. 
	 * 
	 * \param	trainingWrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 * \param	isInitialHit		true if this ist he first hit of the ball during the attempt.
	 */
	virtual void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) { (void)trainingWrapper; /* ignore event unless overridden. */ }

	// Future extensions:
	// - Backboard gets hit
	// - Ball hits ground
	// - Car touches ground
	// - Car lifts off ground
	// - Ball lifts off ground (might not be required)
	// - Ball touches wall
	// - Car touches wall
	// - Ball touches ceiling
	// - Car touches ceiling
	// - Car wheels touch ball (flip reset, not sure if we can detect this)
};