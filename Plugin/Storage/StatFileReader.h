#pragma once

#include "../Core/IStatReader.h"

class GOALPERCENTAGECOUNTER_IMPORT_EXPORT StatFileReader : public IStatReader
{
public:
	StatFileReader(std::shared_ptr<GameWrapper> gameWrapper);

	// Inherited via IStatReader
	std::vector<std::string> getAvailableResourcePaths(const std::string& trainingPackCode) override;
	ShotStats readStats(const std::string& resourcePath) override;

	int peekAttemptAmount(const std::string& resourcePath) override;

private:

	/** Reads the stat block which was available in version 1.0. So far, we only extend the block so we can read it the same way in v1.0 files and later files. */
	bool readVersion_1_0(std::ifstream& fileStream, StatsData* const statsDataPointer);
	/** Reads attributes which were added in version 1.1. */
	bool readVersion_1_1_additions(std::ifstream& fileStream, StatsData* const statsDataPointer);

	std::shared_ptr<GameWrapper> _gameWrapper;
};