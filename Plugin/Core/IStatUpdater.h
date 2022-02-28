#pragma once

#include "../DLLImportExport.h"
#include "../Data/ShotStats.h"

/** The public interface of classes which update statistics data in case of certain events. */
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT IStatUpdater
{
protected:
	IStatUpdater() = default;

public:
	virtual ~IStatUpdater() = default;

	/** Increases the attempt counter. */
	virtual void processAttempt() = 0;

	/** Increases the goal counter and adjusts streak counters. */
	virtual void processGoal() = 0;

	/** Increases the miss counter and adjusts streak counters.. */
	virtual void processMiss() = 0;

	/** Increases the number of initial ball hits. */
	virtual void processInitialBallHit() = 0;

	/** Handles a reset by the state machine or the user. */
	virtual void processReset(int numberOfShots) = 0;

	/** Updates the ShotStats object which is being used by other classes. This should not be updated before calling this method. */
	virtual void updateData() = 0;

	/** Restores any previously saved settings. */
	virtual void restoreLastSession() = 0;

	/** Lets the stat updater know the code of the current training pack. This is useful for restoring state, for example. */
	virtual void publishTrainingPackCode(const std::string& trainingPackCode) = 0; 
	
	/** Allows manually toggling your last attempt. This is useful for training mechanics where only the player can decide wheter that counts or not.
	 *
	 * For example, the player might be training flip resets, mess up the flip reset, but still score. With this feature, they can make the attempt not count.
	 * Also we all know kids will want to use this so they can pretend to be better at the pack than they actually are ;)
	 */
	virtual void toggleLastAttempt() = 0;

	/** Processes a new air dribble attempt time. */
	virtual void processAirDribbleTime(float time) = 0;

	/** Processes the number of air dribble touches. Note that this may get called multiple times per attempt (whenever a new maximum has been reached). */
	virtual void processAirDribbleTouches(int touches) = 0;

	/** Processes a new ground dribble attempt time. */
	virtual void processGroundDribbleTime(float time) = 0;

	/** Processes a double tap goal. */
	virtual void processDoubleTapGoal() = 0;

	/** Processes a flip reset. Note that this may get called multiple times per attempt (whenever a new maximum has been reached). */
	virtual void processFlipReset(int amount) = 0;

	/** Processes a close miss. */
	virtual void processCloseMiss() = 0;
};