#pragma once

#include <bakkesmod/wrappers/GameEvent/TrainingEditorWrapper.h>

/** This class allows implementing only those event listener methods one is interested in.
 * The advantages to manually hooking into gamewrapper events directly are:
 * - You can have several listeners for an event (gamewrapper only allows a single listener)
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
	 *
	 * \param	wrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onTrainingModeLoaded(TrainingEditorWrapper& wrapper) { (void)wrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever the user either changed the shot, or reset the shot (or the game itself loaded the next shot after a goal replay)
	 *
	 * \param	wrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onRoundChanged(TrainingEditorWrapper& wrapper) { (void)wrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever the user started the attempt by pressing pretty much anything on their controller or mouse/keyboard. */
	virtual void onTrainingShotAttemptStarted() { /* ignore event unless overridden. */ }

	/** This gets called whenever an attempt was reset with a goal happening during that attempt.
	 *
	 * \param	wrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onAttemptFinishedWithGoal(TrainingEditorWrapper& wrapper) { (void)wrapper; /* ignore event unless overridden. */ }

	/** This gets called whenever an attempt was reset without any goal happening during that attempt.
	 *
	 * \param	wrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onAttemptFinishedWithoutGoal(TrainingEditorWrapper& wrapper) { (void)wrapper; /* ignore event unless overridden. */ }
	
	/** This gets called whenever a goal was scored (at most once during an attempt, so goal replay will not send these).
	 * Note that only goals in the orange goal are considered.
	 *
	 * \param	wrapper		Provides access to information about the current training pack. Goes out of scope after this call.
	 */
	virtual void onGoalScored(TrainingEditorWrapper& wrapper) { (void)wrapper; /* ignore event unless overridden. */ }

	// Future extensions:
	// - Backboard gets hit
	// - Ball hits ground
	// - Car touches ground
	// - Car touches ball
	// - Car lifts off ground
	// - Ball lifts off ground (might not be required)
	// - Car touches wall
	// - Car touches ceiling
	// - Car wheels touch ball (flip reset, not sure if we can detect this)
};