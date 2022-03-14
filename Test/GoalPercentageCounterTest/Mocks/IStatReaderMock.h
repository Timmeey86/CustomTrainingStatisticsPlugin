#pragma once

#include <gmock/gmock.h>
#include <Plugin/Core/IStatReader.h>

class IStatReaderMock : public IStatReader
{
public:
	MOCK_METHOD(std::vector<std::string>, getAvailableResourcePaths, (const std::string&), (override));
	MOCK_METHOD(ShotStats, readStats, (const std::string&, bool), (override));
	MOCK_METHOD(int, peekAttemptAmount, (const std::string&), (override));
	MOCK_METHOD(ShotStats, readTrainingPackStatistics, (const ShotStats& shotStats, const std::string& trainingPackCode), (override));
};