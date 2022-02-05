#pragma once

#include "../DLLImportExport.h"

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
};