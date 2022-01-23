#pragma once

#include "../DLLImportExport.h"

/** The public interface of classes which update statistics data in case of certain events. */
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT IStatUpdater
{
protected:
	IStatUpdater() = default;

public:
	virtual ~IStatUpdater() = default;

	/** Handles a goal. */
	virtual void processGoal() = 0;

	/** Handles the start of an attempt. */
	virtual void processNewAttempt() = 0;

	// Possible future extension: Handles a miss (= a reset after the start of an attempt, without scoring a goal)
	// Implementing this should increase liability and reduce complexity of the remaining code
	// It would also make processShotReset() obsolete most likely
	// virtual void processMiss() = 0;

	/** Handles a shot reset. */
	virtual void processShotReset() = 0;

	/** Handles the initial touch of the ball (at most once per attempt). */
	virtual void processInitialBallHit() = 0;

	/** Handles a stat reset by the user (or some code). */
	virtual void processManualStatReset() = 0;

	/** Handles loading of a new training pack. */
	virtual void handleTrainingPackLoad() = 0;

};