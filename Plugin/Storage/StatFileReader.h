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
	std::shared_ptr<GameWrapper> _gameWrapper;
};