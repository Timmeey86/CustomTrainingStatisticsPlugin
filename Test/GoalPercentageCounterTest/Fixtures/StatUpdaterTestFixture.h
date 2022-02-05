#pragma once

#include <memory>

#include <gmock/gmock.h>

#include <Plugin/Data/PluginState.h>
#include <Plugin/Data/ShotStats.h>
#include <Plugin/Calculation/StatUpdater.h>

#include "../Mocks/IStatReaderMock.h"

class StatUpdaterTestFixture : public ::testing::Test
{
public:
	std::shared_ptr<ShotStats> _shotStats = std::make_shared<ShotStats>();
	std::shared_ptr<PluginState> _pluginState = std::make_shared<PluginState>();

	std::shared_ptr<StatUpdater> statUpdater;
	std::shared_ptr<IStatReaderMock> _statReader;

	static const std::string FakeTrainingPackCode;

	void SetUp() override
	{
		_statReader = std::make_shared<::testing::StrictMock<IStatReaderMock>>();
		statUpdater = std::make_shared<StatUpdater>(_shotStats, _pluginState, _statReader);
		statUpdater->publishTrainingPackCode(FakeTrainingPackCode);
		_pluginState->TotalRounds = 2;
		_pluginState->CurrentRoundIndex = 0;
	}

	void expectTotalStats(const PlayerStats& expectedStats) const;

	void expectPerShotStats(const PlayerStats& expectedStats, int shotNumber) const;
};