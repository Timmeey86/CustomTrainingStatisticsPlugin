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
		std::shared_ptr<PlayerStats> playerStats,
		std::shared_ptr<CalculatedData> calculatedData,
		std::shared_ptr<PluginState> pluginState
	);

	// Inherited via IStatDisplay
	void renderOneFrame(CanvasWrapper canvas) override;

private:
	/** Builds a list of stats to be rendered, based on the current plugin state. 
	 *
	 * \returns		A list of string pairs, consisting of a label and a value string. These will be displayed in two columns.
	 */
	std::list<std::pair<std::string, std::string>> getStatsToBeRendered() const;

	std::shared_ptr<PlayerStats> _playerStats;			///< Statistics about the player
	std::shared_ptr<CalculatedData> _calculatedData;	///< Calculated data based on PlayerStats
	std::shared_ptr<PluginState> _pluginState;			///< The state of the plugin.
};

