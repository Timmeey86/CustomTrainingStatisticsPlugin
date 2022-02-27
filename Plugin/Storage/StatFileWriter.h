#pragma once

#include <bakkesmod/wrappers/GameWrapper.h>

#include "../Core/IStatWriter.h"
#include "../Data/ShotStats.h"
#include "../Calculation/ShotDistributionTracker.h"

/** Writes StatsData objects to the file system .*/
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT StatFileWriter : public IStatWriter
{
public:
	StatFileWriter(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<ShotStats> shotStats, std::shared_ptr<ShotDistributionTracker> tracker);

	// Inherited via IStatWriter
	void initializeStorage(const std::string& trainingPackCode) override;
	void writeStatsData(std::ofstream& stream, const StatsData& statsData);
	void writeData() override;

private:
	std::shared_ptr<GameWrapper> _gameWrapper;
	std::shared_ptr<ShotStats> _currentStats;
	std::filesystem::path _outputFilePath; 
	
	std::shared_ptr<ShotDistributionTracker> _shotDistributionTracker; ///< This is used for writing shot locations and heat map data to the file
};