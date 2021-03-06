#pragma once

#include <vector>
#include <utility>

#include "../DLLImportExport.h"
#include "../Core/IStatUpdater.h"
#include "../Core/IStatReader.h"
#include "../Data/ShotStats.h"
#include "../Data/PluginState.h"
#include "AllTimePeakHandler.h"

/** This class currently:
	- updates statistics whenever they change
	- calculates percentages based on gathered data
	- restores the previous session
	- calculates comparison results to the previous session, or the all time peak stats

	Likewise, this class is a good candidate for being refactored. It currently has too many responsibilities to be maintainable.
*/
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT StatUpdater : public IStatUpdater
{
public:
	/** Creates a new object which is able to update statistics properly. */
	StatUpdater(
		std::shared_ptr<ShotStats> shotStats,
		std::shared_ptr<ShotStats> differenceStats,
		std::shared_ptr<PluginState> pluginState,
		std::shared_ptr<IStatReader> statReader,
		std::shared_ptr<AllTimePeakHandler> peakHandler
	);

	// Inherited via IStatUpdater
	void processAttempt() override;
	void processGoal() override;
	void processMiss() override;
	void processInitialBallHit() override;
	void processReset(int numberOfShots) override;
	void updateData() override;
	void restoreLastSession() override;

	void publishTrainingPackCode(const std::string& trainingPackCode) override;
	void toggleLastAttempt() override;

	void processAirDribbleTime(float time) override;
	void processAirDribbleTouches(int touches) override;
	void processGroundDribbleTime(float time) override;
	void processDoubleTapGoal() override; 
	void processFlipReset(int amount) override;
	void processCloseMiss() override;

	void updateCompareBase() override;

private:
	/** Increases the goal counter and updates streaks. */
	void handleGoal(StatsData& statsData);
	/** Increases the miss counter and updates streaks. */
	void handleMiss(StatsData& statsData);

	void handleAirDribbleTimeUpdate(StatsData& statsData, float time);
	void handleAirDribbleTouchesUpdate(StatsData& statsData, int touches);
	void handleGroundDribbleTimeUpdate(StatsData& statsData, float time);
	void handleDoubleTapGoalUpdate(StatsData& statsData);
	void handleFlipResetUpdate(StatsData& statsData, int amount);
	void handleCloseMiss(StatsData& statsData);

	/** Updates percentage values. */
	void recalculatePercentages(StatsData& statsData, StatsData& internalStatsData);
	/** Updates the internal backup of stats. This is used for the "toggle last attempt" feature. */
	void updateStatsBackup();
	/** Retrieves the differences between the current session and the previous one, or if stats had been restored from the previous session,
	 * between the current one and the one before the previous one. */
	ShotStats retrieveSessionDiff() const;
		
	ShotStats _internalShotStats; ///< A cache of the current stats (we don't use calculated data here, though)
	ShotStats _previousShotStats; ///< This is used in order to properly implement the "toggle last attempt" feature without messing up streaks/peaks
	ShotStats _compareBase; ///< Session differences are compared to this object.
	std::shared_ptr<ShotStats> _externalShotStats;	///< The current stats as seen by everything outside of this class.
	std::shared_ptr<ShotStats> _differenceStats; ///< Stores the differences between the current and the previous session.

	std::shared_ptr<PluginState> _pluginState;	///< The current state of the plugin
	std::shared_ptr<IStatReader> _statReader; ///< Used for restoring previous state
	std::shared_ptr<AllTimePeakHandler> _peakHandler; ///< The handler for peak stats.
	std::string _trainingPackCode; ///< The code of the currently active training pack

	bool _statsHaveJustBeenRestored = false; ///< This prevents the "toggle last attempt" feature from being used after restoring the last session
	bool _flipResetOccurredInCurrentAttempt = false; ///< This is required for detection of flip reset goals.

	int _numberOfSessionsToBeSkipped = false; ///< Stores the number of sessions to be skipped when comparing to the previous session.
};

