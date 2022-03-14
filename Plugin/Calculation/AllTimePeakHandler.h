#pragma once

#include "../Core/IStatReader.h"
#include "../Core/IStatWriter.h"
#include "../Data/PluginState.h"

#include <memory>

/** This class is responsible for storing the "all time peak" of any stat for each shot/pack. 

	This allows detection of breaking personal records, and will be a better indicator than comparing to e.g. a session which was aborted, but not continued.
*/
class AllTimePeakHandler
{
public:
	/** Updates any stat which is considered better if higher and writes them to the file in case they were improved. */
	void updateMaximumStats(const std::shared_ptr<ShotStats>& currentStats);

	/** Updates any stat which is considered better if lower and writes them to the file in case they were improved. It is recommended to call this only when
	 * the player can no longer make the stat worse. 
	 */
	void updateMinimumStats(const std::shared_ptr<ShotStats>& currentStats);

	/** Retrieves the peak stats for the current training pack. Empty struct if not in a training pack, or during the first time. */
	ShotStats getPeakStats() const;
private:
	bool writeAllStatFile();
	bool readAllStatFile();

	std::shared_ptr<IStatReader> _statReader;		///< Allows reading the all time max from a file.
	std::shared_ptr<IStatWriter> _statWriter;		///< Allows writing the all time max to a file.
	std::shared_ptr<PluginState> _pluginState;		///< The current settings of the plugin.
	ShotStats _allTimePeakStats;	///< The current best stats for a pack. Note that for some stats, a lower value might be better.
};