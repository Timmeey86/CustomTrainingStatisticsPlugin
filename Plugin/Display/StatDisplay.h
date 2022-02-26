#pragma once

#include <optional>

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
	std::optional<std::string> DiffValue;	
};

/** Implements a simple stat display which draws text on a half-transparent background frame (configurable). */
class StatDisplay : public IStatDisplay
{
public:
	/** Creates a new object which is able to display statistics to the user. */
	StatDisplay(
		const std::shared_ptr<const ShotStats> shotStats,
		const std::shared_ptr<const ShotStats> diffStats,
		const std::shared_ptr<const PluginState> pluginState
	);

	// Inherited via IStatDisplay
	void renderOneFrame(CanvasWrapper& canvas) override;

	/** Builds a list of stats to be rendered, based on the current plugin state.
	 *
	 * \returns		A list of SingleStatStrings, consisting of a label, value, and unit string. These will be displayed in three columns.
	 */
	static std::list<SingleStatStrings> GetStatsToBeRendered(const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData = nullptr);

private:
	void drawCenter(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const std::string& label) const;
	void renderStatsData(CanvasWrapper& canvas, const DisplayOptions& opts, const StatsData& statsData, const StatsData* const diffData);
	void renderAllShotStats(CanvasWrapper& canvas);
	void renderPerShotStats(CanvasWrapper& canvas);

	const std::shared_ptr<const ShotStats> _shotStats;		///< Statistics and data for shots taken in custom training.
	const std::shared_ptr<const ShotStats> _diffStats;		///< The difference statistics with the previous session, if available.
	const std::shared_ptr<const PluginState> _pluginState;	///< The state of the plugin.

	float _displayWidth{ 230.0f };
};

