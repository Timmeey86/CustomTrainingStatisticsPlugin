#include <pch.h>
#include "StatUpdater.h"

StatUpdater::StatUpdater(
	std::shared_ptr<ShotStats> shotStats,
	std::shared_ptr<PluginState> pluginState,
	std::shared_ptr<IStatReader> statReader)
	: _externalShotStats(shotStats)
	, _pluginState(pluginState)
	, _statReader(statReader)
{
}

void StatUpdater::processGoal()
{
	handleGoal(_internalShotStats.AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleGoal(currStatsData);
	}
}

void StatUpdater::processMiss()
{
	handleMiss(_internalShotStats.AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleMiss(currStatsData);
	}
}

void StatUpdater::processAttempt()
{
	_internalShotStats.AllShotStats.Stats.Attempts++;

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		_internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex).Stats.Attempts++;
	}
}

void StatUpdater::processInitialBallHit()
{
	_internalShotStats.AllShotStats.Stats.InitialHits++;

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		_internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex).Stats.InitialHits++;
	}
}

void StatUpdater::processReset(int numberOfShots)
{
	// Reset total stats
	_internalShotStats.AllShotStats.Stats = PlayerStats();
	_internalShotStats.AllShotStats.Data = CalculatedData();

	// Reset per shot stats
	_internalShotStats.PerShotStats.clear();
	for (auto index = 0; index < numberOfShots; index++)
	{
		_internalShotStats.PerShotStats.emplace_back();
	}

	// Replace the whole external object with our freshly reset copy
	*_externalShotStats = _internalShotStats;
}

void StatUpdater::updateData()
{
	// Note: This method relies on the state machine calling it at appropriate moments in time
	//       We do not calculate everything every time, but rather specifically when the state machine deems it necessary.
	//       As long as the state machine works correctly, it is enough to update only the current shot (and the summary object)

	_externalShotStats->AllShotStats = _internalShotStats.AllShotStats;
	recalculatePercentages(_externalShotStats->AllShotStats);

	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		_externalShotStats->PerShotStats.at(_pluginState->CurrentRoundIndex) = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		recalculatePercentages(_externalShotStats->PerShotStats.at(_pluginState->CurrentRoundIndex));
	}
}

void StatUpdater::restoreLastSession()
{
	if (_trainingPackCode.empty()) { return; }
	auto resourcePaths = _statReader->getAvailableResourcePaths(_trainingPackCode);
	for (const auto& resourcePath : resourcePaths)
	{
		// Skip any file which only has zero attempts stored
		if (_statReader->peekAttemptAmount(resourcePath) == 0) { continue; }

		// We found a promising file to be restored
		auto stats = _statReader->readStats(resourcePath);
		_internalShotStats.AllShotStats = stats.AllShotStats;
		_internalShotStats.PerShotStats = std::vector<StatsData>(stats.PerShotStats);
		updateData();
		return;
	}

	// Reaching this point means there either are no files, or all of them have zero attempts => we can't do anything in this case
}

void StatUpdater::publishTrainingPackCode(const std::string& trainingPackCode)
{
	_trainingPackCode = trainingPackCode;
}

double getPercentageValue(double attempts, double goals)
{
	return round((goals / attempts) * 10000.0) / 100.0;
}

void StatUpdater::recalculatePercentages(StatsData& statsData)
{
	auto successPercentage = .0;
	auto initialHitPercentage = .0;
	if (statsData.Stats.Attempts > 0)
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = getPercentageValue(statsData.Stats.Attempts, statsData.Stats.Goals);
		initialHitPercentage = getPercentageValue(statsData.Stats.Attempts, statsData.Stats.InitialHits);
	}
	statsData.Data.SuccessPercentage = successPercentage;
	statsData.Data.InitialHitPercentage = initialHitPercentage;

	// Update the percentage for the last 50 shots
	// Ignore the event if this is a reset after a goal
	while (statsData.Stats.Last50Shots.size() > 50)
	{
		statsData.Stats.Last50Shots.erase(statsData.Stats.Last50Shots.begin());
	}
	successPercentage = .0;
	if (!statsData.Stats.Last50Shots.empty())
	{
		auto numberOfGoals = std::count(statsData.Stats.Last50Shots.begin(), statsData.Stats.Last50Shots.end(), true);
		successPercentage = getPercentageValue((double)statsData.Stats.Last50Shots.size(), (double)numberOfGoals);
	}
	statsData.Data.Last50ShotsPercentage = successPercentage;

	// Update the peak percentage only after 20 shots since otherwise a couple of lucky early shots would create a wrong impression
	if (statsData.Stats.Attempts > 20 && statsData.Data.Last50ShotsPercentage > statsData.Data.PeakSuccessPercentage)
	{
		statsData.Data.PeakSuccessPercentage = statsData.Data.Last50ShotsPercentage;
		statsData.Data.PeakShotNumber = statsData.Stats.Attempts;
	}
}

void StatUpdater::handleGoal(StatsData& statsData)
{
	statsData.Stats.Last50Shots.push_back(true);
	statsData.Stats.MissStreakCounter = 0;
	statsData.Stats.GoalStreakCounter++;
	statsData.Stats.Goals++;

	if (statsData.Stats.GoalStreakCounter > statsData.Stats.LongestGoalStreak)
	{
		statsData.Stats.LongestGoalStreak = statsData.Stats.GoalStreakCounter;
	}

	statsData.Stats.GoalSpeedStats.insert(_pluginState->getBallSpeed());
}

void StatUpdater::handleMiss(StatsData& statsData)
{
	statsData.Stats.Last50Shots.push_back(false);
	statsData.Stats.GoalStreakCounter = 0;
	statsData.Stats.MissStreakCounter++;

	if (statsData.Stats.MissStreakCounter > statsData.Stats.LongestMissStreak)
	{
		statsData.Stats.LongestMissStreak = statsData.Stats.MissStreakCounter;
	}
}