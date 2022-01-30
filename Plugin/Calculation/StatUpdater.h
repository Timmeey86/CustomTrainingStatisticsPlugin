#pragma once

#include <vector>
#include <utility>

#include "../DLLImportExport.h"
#include "../Core/IStatUpdater.h"
#include "../Data/ShotStats.h"
#include "../Data/PluginState.h"

class GOALPERCENTAGECOUNTER_IMPORT_EXPORT StatUpdater : public IStatUpdater
{
public:
	/** Creates a new object which is able to update statistics properly. */
	StatUpdater(
		std::shared_ptr<ShotStats> shotStats,
		std::shared_ptr<PluginState> pluginState
	);

	// Inherited via IStatUpdater
	void processAttempt() override;
	void processGoal() override;
	void processMiss() override;
	void processInitialBallHit() override;
	void processReset(int numberOfShots) override;
	void updateData() override;

private:
	/** Increases the goal counter and updates streaks. */
	void handleGoal(StatsData& statsData);
	/** Increases the miss counter and updates streaks. */
	void handleMiss(StatsData& statsData);
	/** Updates percentage values. */
	void recalculatePercentages(StatsData& statsData);
		
	ShotStats _internalShotStats; ///< A cache of the current stats (we don't use calculated data here, though)
	std::shared_ptr<ShotStats> _externalShotStats;	///< The current stats as seen by everything outside of this class.

	std::shared_ptr<PluginState> _pluginState;	///< The current state of the plugin
};

