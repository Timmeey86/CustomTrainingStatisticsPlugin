#pragma once

#include "../Core/IStatDisplay.h"
#include "../Data/ShotStats.h"
#include "../Data/StatsData.h"
#include "../Data/PluginState.h"

class StatDisplay : public IStatDisplay
{
public:
	/** Creates a new object which is able to display statistics to the user. */
	StatDisplay(
		const std::shared_ptr<const ShotStats> shotStats,
		const std::shared_ptr<const PluginState> pluginState
	);

	// Inherited via IStatDisplay
	void renderOneFrame(CanvasWrapper& canvas) const override;

private:
	/** Builds a list of stats to be rendered, based on the current plugin state. 
	 *
	 * \returns		A list of string pairs, consisting of a label and a value string. These will be displayed in two columns.
	 */
	std::list<std::pair<std::string, std::string>> getStatsToBeRendered(const std::shared_ptr<const StatsData> statsData) const;

	void render(CanvasWrapper& canvas, const DisplayOptions& opts, const std::shared_ptr<const StatsData> statsData) const;
	void renderAllShotStats(CanvasWrapper& canvas) const;
	void renderPerShotStats(CanvasWrapper& canvas) const;

	const std::shared_ptr<const ShotStats> _shotStats;		///< Statistics and data for shots taken in custom training
	const std::shared_ptr<const PluginState> _pluginState;	///< The state of the plugin.
};

