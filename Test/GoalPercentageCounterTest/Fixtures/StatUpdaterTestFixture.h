#pragma once

#include <memory>

#include <gmock/gmock.h>

#include <Plugin/Data/PluginState.h>
#include <Plugin/Data/ShotStats.h>
#include <Plugin/Calculation/StatUpdater.h>

class StatUpdaterTestFixture : public ::testing::Test
{
public:
	std::shared_ptr<ShotStats> _shotStats = std::make_shared<ShotStats>();
	std::shared_ptr<PluginState> _pluginState = std::make_shared<PluginState>();

	std::shared_ptr<StatUpdater> statUpdater;

	void SetUp() override
	{
		statUpdater = std::make_shared<StatUpdater>(_shotStats, _pluginState);
		_pluginState->TotalRounds = 2;
		_pluginState->CurrentRoundIndex = 0;
		_shotStats->PerShotStats.emplace_back();
		_shotStats->PerShotStats.emplace_back();
	}

	void expectTotalStats(const PlayerStats& expectedStats) const;

	void expectPerShotStats(const PlayerStats& expectedStats, int shotNumber) const;
};