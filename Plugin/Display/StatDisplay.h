#pragma once

#include "../Core/IStatDisplay.h"
#include "../Data/CalculatedData.h"
#include "../Data/PlayerStats.h"
#include "../Data/PluginState.h"

class StatDisplay : public IStatDisplay
{
public:
	/** Creates a new object which is able to display statistics to the user. */
	StatDisplay(
		const std::shared_ptr<const PlayerStats> playerStats,
		const std::shared_ptr<const CalculatedData> calculatedData,
		const std::shared_ptr<const std::vector<std::pair<std::shared_ptr<PlayerStats>, std::shared_ptr<CalculatedData>>>> statsDataPerShot,
		const std::shared_ptr<const PluginState> pluginState
	);

	// Inherited via IStatDisplay
	void renderOneFrame(CanvasWrapper& canvas) const override;

private:
	/** Builds a list of stats to be rendered, based on the current plugin state. 
	 *
	 * \returns		A list of string pairs, consisting of a label and a value string. These will be displayed in two columns.
	 */
	std::list<std::pair<std::string, std::string>> getStatsToBeRendered(const std::shared_ptr<const PlayerStats> playerStats, const std::shared_ptr<const CalculatedData> calculatedData) const;

	void render(CanvasWrapper& canvas, const DisplayOptions& opts, const std::shared_ptr<const PlayerStats> playerStats, const std::shared_ptr<const CalculatedData> calculatedData) const;
	void renderAllShotStats(CanvasWrapper& canvas) const;
	void renderPerShotStats(CanvasWrapper& canvas) const;

	const std::shared_ptr<const PlayerStats> _playerStats;			///< Statistics about the player
	const std::shared_ptr<const CalculatedData> _calculatedData;	///< Calculated data based on PlayerStats
	/// Each element of the vector represents the stats and data for each shot
	const std::shared_ptr<const std::vector<std::pair<std::shared_ptr<PlayerStats>, std::shared_ptr<CalculatedData>>>> _statsDataPerShot;
	const std::shared_ptr<const PluginState> _pluginState;			///< The state of the plugin.
};

