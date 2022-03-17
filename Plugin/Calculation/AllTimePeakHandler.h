#pragma once

#include "../Core/IStatReader.h"
#include "../Core/IStatWriter.h"
#include "../Data/PluginState.h"
#include "../Data/FakeGoalSpeedProvider.h"

#include <memory>

/** This class is responsible for storing the "all time peak" of any stat for each shot/pack. 

	This allows detection of breaking personal records, and will be a better indicator than comparing to e.g. a session which was aborted, but not continued.
*/
class AllTimePeakHandler
{
public:
	/** Constructor. */
	AllTimePeakHandler(
		std::shared_ptr<IStatReader> statReader,
		std::shared_ptr<IStatWriter> statWriter,
		std::shared_ptr<PluginState> pluginState,
		std::shared_ptr<ShotStats> shotStats
	);

	/** Resets, e.g. after switching to a new training pack. */
	void reset();

	/** Updates any stat which is considered better if higher and writes them to the file in case they were improved. */
	void updateMaximumStats();

	/** Retrieves the peak stats for the current training pack. Empty struct if not in a training pack, or during the first time. */
	ShotStats getPeakStats() const;
private:

	void copyStats(const ShotStats& source, bool statsWereRestoredFromFile);
	bool writeAllStatFile();
	bool readAllStatFile();

	std::shared_ptr<IStatReader> _statReader;		///< Allows reading the all time max from a file.
	std::shared_ptr<IStatWriter> _statWriter;		///< Allows writing the all time max to a file.
	std::shared_ptr<PluginState> _pluginState;		///< The current settings of the plugin.
	std::shared_ptr<ShotStats> _currentStats;		///< The stats of the current training session.
	ShotStats _allTimePeakStats;	///< The current best stats for a pack. Note that for some stats, a lower value might be better.
	std::shared_ptr<FakeGoalSpeedProvider> _allStatGoalSpeedProvider = std::make_shared<FakeGoalSpeedProvider>(); ///< Allows setting fake values for min, max, median and mean goal speed.
	std::vector<std::shared_ptr<FakeGoalSpeedProvider>> _perShotGoalSpeedProviders; ///< Allows setting fake values for min, max, median and mean goal speed for single shots.
};