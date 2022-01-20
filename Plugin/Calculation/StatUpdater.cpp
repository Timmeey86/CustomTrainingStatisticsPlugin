#include <pch.h>
#include "StatUpdater.h"

StatUpdater::StatUpdater(
	std::shared_ptr<ShotStats> shotStats,
	std::shared_ptr<PluginState> pluginState)
	: _shotStats(shotStats)
	, _pluginState(pluginState)
{
}

void StatUpdater::processGoal()
{
	// Update total stats
	handleGoal(_shotStats->AllShotStats);
	recalculatePercentages(_shotStats->AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _shotStats->PerShotStats.size())
	{
		auto&& currStatsData = _shotStats->PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleGoal(currStatsData);
		recalculatePercentages(currStatsData);
	}
}

void StatUpdater::processNewAttempt()
{
	// Check if _statsDataPerShot is initialized and init if not
	// Could not be initialized if plugin was loaded while in training
	if (_pluginState->TotalRounds > 0 && _pluginState->TotalRounds != _shotStats->PerShotStats.size())
	{
		initStatsDataPerShot();
	}

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _shotStats->PerShotStats.size())
	{
		auto&& currStatsData = _shotStats->PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleAttempt(currStatsData, false);
		recalculatePercentages(currStatsData);
	}

	// Update total stats
	// update total stats last because handleAttempt needs to change plugin state, which might not always be the case for per shot stats
	handleAttempt(_shotStats->AllShotStats, true);
	recalculatePercentages(_shotStats->AllShotStats);
}

void StatUpdater::processShotReset()
{
	// Update total stats
	recalculatePercentages(_shotStats->AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex &&_pluginState->CurrentRoundIndex < _shotStats->PerShotStats.size())
	{
		auto&& currStatsData = _shotStats->PerShotStats.at(_pluginState->CurrentRoundIndex);
		recalculatePercentages(currStatsData);
	}
}

void StatUpdater::processInitialBallHit()
{
	// Increase total hit counter and update total stats
	_shotStats->AllShotStats.Stats.InitialHits++;
	recalculatePercentages(_shotStats->AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _shotStats->PerShotStats.size())
	{
		auto&& currStatsData = _shotStats->PerShotStats.at(_pluginState->CurrentRoundIndex);
		currStatsData.Stats.InitialHits++;
		recalculatePercentages(currStatsData);
	}
}

void StatUpdater::processManualStatReset()
{
	reset();

	// Recalculate total stats
	recalculatePercentages(_shotStats->AllShotStats);

	// Recalculate per shot stats
	for (auto&& statsData : _shotStats->PerShotStats)
	{
		recalculatePercentages(statsData);
	}
}

void StatUpdater::initStatsDataPerShot()
{
	// Delete pointers
	_shotStats->PerShotStats.clear();

	// Create pointers
	for (int i = 0; i < _pluginState->TotalRounds; ++i)
	{
		_shotStats->PerShotStats.emplace_back();
	}
}

void StatUpdater::handleTrainingPackLoad()
{
	// This is currently not different from manually resetting.
	processManualStatReset();
}

void StatUpdater::reset()
{
	// Reset total stats
	_shotStats->AllShotStats.Stats = PlayerStats();
	_shotStats->AllShotStats.Data = CalculatedData();

	// Reset per shot stats
	if (_pluginState->TotalRounds > 0)
	{
		if (_pluginState->TotalRounds == _shotStats->PerShotStats.size())
		{
			for (auto&& statsData : _shotStats->PerShotStats)
			{
				statsData.Stats = PlayerStats();
				statsData.Data = CalculatedData();
			}
		}
		else
		{
			initStatsDataPerShot();
		}
	}

	// Do not reset anything which can be changed by user settings - Those settings shall of course not be reset.
	// Also do not reset anything which is based on the Rocket League state
	_pluginState->PreviousAttemptWasAGoal = false;
	_pluginState->BallWasHitAtLeastOnceWithinCurrentAttempt = false;
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
	if (!statsData.Stats.Last50Shots.empty())
	{
		statsData.Stats.Last50Shots.back() = true; // Replace the current attempt with a goal entry
	}
	statsData.Stats.MissStreakCounter = 0;
	statsData.Stats.GoalStreakCounter++;
	statsData.Stats.Goals++;

	// When starting the next attempt, we don't know whether the previous one was a goal or a miss without this flag
	// (there is no trigger for a miss, unless we incorporate the reset trigger for it (we might actually do that in future))
	_pluginState->PreviousAttemptWasAGoal = true;

	if (statsData.Stats.GoalStreakCounter > statsData.Stats.LongestGoalStreak)
	{
		statsData.Stats.LongestGoalStreak = statsData.Stats.GoalStreakCounter;
	}
}

void StatUpdater::handleAttempt(StatsData& statsData, bool changePluginState)
{
	statsData.Stats.Last50Shots.push_back(false);

	// Count the shot attempt in any case
	statsData.Stats.Attempts++;

	if (_pluginState->PreviousAttemptWasAGoal)
	{
		// Only change plugin state on last call to handleAttempt (called for all shots, and per shot)
		if (changePluginState)
		{
			// A goal was scored, and then reset was pressed. No further action required, but do not ignore any further resets.
			_pluginState->PreviousAttemptWasAGoal = false;
		}
	}
	else
	{
		statsData.Stats.MissStreakCounter++;
		statsData.Stats.GoalStreakCounter = 0;

		if (statsData.Stats.MissStreakCounter > statsData.Stats.LongestMissStreak)
		{
			statsData.Stats.LongestMissStreak = statsData.Stats.MissStreakCounter;
		}
	}
}