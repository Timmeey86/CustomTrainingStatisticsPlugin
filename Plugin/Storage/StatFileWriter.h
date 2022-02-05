#pragma once

#include "../Core/IStatWriter.h"
#include "../Data/ShotStats.h"
#include <bakkesmod/wrappers/GameWrapper.h>

/** Writes StatsData objects to the file system .*/
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT StatFileWriter : public IStatWriter
{
public:
	StatFileWriter(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<ShotStats> shotStats);

	// Inherited via IStatWriter
	void initializeStorage(const std::string& trainingPackCode) override;
	void writeData() override;

private:
	std::shared_ptr<GameWrapper> _gameWrapper;
	std::shared_ptr<ShotStats> _currentStats;
	std::filesystem::path _outputFilePath;
};