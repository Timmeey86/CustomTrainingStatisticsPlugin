#pragma once

#include "../Core/IStatUpdater.h"
#include "../Data/CalculatedData.h"
#include "../Data/PlayerStats.h"
#include "../Data/PluginState.h"

class StatUpdater : public IStatUpdater
{
public:
	/** Creates a new object which is able to update statistics properly. */
	StatUpdater(
		std::shared_ptr<PlayerStats> playerStats,
		std::shared_ptr<CalculatedData> calculatedData,
		std::shared_ptr<PluginState> pluginState
	);

	// Inherited via IStatUpdater
	void processGoal() override;
	void processNewAttempt() override;
	void processShotReset() override;
	void processManualStatReset() override;
	void handleTrainingPackLoad() override;

private:
	/** Increases the goal counter, updates streaks and recalculates percentages. */
	void handleGoal();
	/** Increases the attempt counter, updates streaks and recalculates percentages. */
	void handleAttempt();
	/** Resets everything to zero. */
	void reset();
	/** Updates percentage values. */
	void recalculatePercentages();

	std::shared_ptr<PlayerStats> _playerStats;			///< Statistics about the player
	std::shared_ptr<CalculatedData> _calculatedData;	///< Calculated data based on PlayerStats
	std::shared_ptr<PluginState> _pluginState;			///< The current state of the plugin
};

