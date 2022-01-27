#pragma once

#include "../Core/IStatDisplay.h"
#include "../Data/ShotStats.h"
#include "../Data/StatsData.h"
#include "../Data/PluginState.h"

/** Defines parts of what shall be displyed in a single line in the stat display. */
struct SingleStatStrings
{
public:
	std::string Label;
	std::string Value;
	std::string Unit;
};

/** Implements a simple stat display which draws text on a half-transparent background frame (configurable). */
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

	/** Builds a list of stats to be rendered, based on the current plugin state.
	 *
	 * \returns		A list of string pairs, consisting of a label and a value string. These will be displayed in two columns.
	 */
	static std::list<SingleStatStrings> GetStatsToBeRendered(const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState);
	static constexpr float DISPLAY_WIDTH = 215.0f;

private:
	void drawCenter(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const std::string& label) const;
	void renderStatsData(CanvasWrapper& canvas, const DisplayOptions& opts, const StatsData& statsData) const;
	void renderAllShotStats(CanvasWrapper& canvas) const;
	void renderPerShotStats(CanvasWrapper& canvas) const;

	const std::shared_ptr<const ShotStats> _shotStats;		///< Statistics and data for shots taken in custom training
	const std::shared_ptr<const PluginState> _pluginState;	///< The state of the plugin.
	
};

