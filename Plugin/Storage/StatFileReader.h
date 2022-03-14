#pragma once

#include "../Core/IStatReader.h"
#include "../Calculation/ShotDistributionTracker.h"

class GOALPERCENTAGECOUNTER_IMPORT_EXPORT StatFileReader : public IStatReader
{
public:
	StatFileReader(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<ShotDistributionTracker> shotDistributionTracker);

	// Inherited via IStatReader
	std::vector<std::string> getAvailableResourcePaths(const std::string& trainingPackCode) override;
	ShotStats readStats(const std::string& resourcePath, bool statsAboutToBeRestored) override;

	ShotStats readTrainingPackStatistics(const ShotStats& shotStats, const std::string& trainingPackCode) override;

	int peekAttemptAmount(const std::string& resourcePath) override;

private:

	/** Reads the stat block which was available in version 1.0. So far, we only extend the block so we can read it the same way in v1.0 files and later files. */
	bool readVersion_1_0(std::ifstream& fileStream, StatsData* const statsDataPointer);
	/** Reads attributes which were added in version 1.1. */
	bool readVersion_1_1_additions(std::ifstream& fileStream, StatsData* const statsDataPointer);
	/** Reads attributes which were added in verison 1.2 (heat map). */
	bool readVersion_1_2_additions(std::ifstream& fileStream, bool statsAboutToBeRestored);
	/** Reads attributes which were added in version 1.3 (goal speed). */
	bool readVersion_1_3_additions(std::ifstream& fileStream, StatsData* const statsDataPointer);

	std::shared_ptr<GameWrapper> _gameWrapper;
	std::shared_ptr<ShotDistributionTracker> _shotDistributionTracker;
};