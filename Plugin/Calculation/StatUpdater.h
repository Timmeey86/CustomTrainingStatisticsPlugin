#pragma once

#include <vector>
#include <utility>

#include "../DLLImportExport.h"
#include "../Core/IStatUpdater.h"
#include "../Data/ShotStats.h"
#include "../Data/StatsData.h"
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
	void processGoal() override;
	void processNewAttempt() override;
	void processShotReset() override;
	void processInitialBallHit() override;
	void processManualStatReset() override;
	void handleTrainingPackLoad() override;

private:
	/** Increases the goal counter, updates streaks and recalculates percentages. */
	void handleGoal(StatsData& statsData);
	/** Increases the attempt counter, updates streaks and recalculates percentages. */
	void handleAttempt(StatsData& statsData, bool changePluginState);
	/** Resets everything to zero. */
	void reset();
	/** Updates percentage values. */
	void recalculatePercentages(StatsData& statsData);
	/** Initializes the per shot vector */
	void initStatsDataPerShot();

	std::shared_ptr<ShotStats> _shotStats;		///< Statistics and data for shots taken in custom training
	std::shared_ptr<PluginState> _pluginState;	///< The current state of the plugin
};

