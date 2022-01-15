#include <pch.h>
#include "StatUpdater.h"

StatUpdater::StatUpdater(
	std::shared_ptr<PlayerStats> playerStats,
	std::shared_ptr<CalculatedData> calculatedData,
	std::shared_ptr<std::vector<std::pair<std::shared_ptr<PlayerStats>, std::shared_ptr<CalculatedData>>>> statsDataPerShot,
	std::shared_ptr<PluginState> pluginState)
	: _playerStats(playerStats)
	, _calculatedData(calculatedData)
	, _statsDataPerShot(statsDataPerShot)
	, _pluginState(pluginState)
{
}

void StatUpdater::processGoal()
{
	// Update total stats
	handleGoal(_playerStats);
	recalculatePercentages(_playerStats, _calculatedData);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _statsDataPerShot->size())
	{
		auto& currStatsData = _statsDataPerShot->at(_pluginState->CurrentRoundIndex);
		handleGoal(currStatsData.first);
		recalculatePercentages(currStatsData.first, currStatsData.second);
	}
}

void StatUpdater::processNewAttempt()
{
	// Check if _statsDataPerShot is initialized and init if not
	// Could not be initialized if plugin was loaded while in training
	if (_pluginState->TotalRounds > 0 && _pluginState->TotalRounds != _statsDataPerShot->size())
	{
		initStatsDataPerShot();
	}

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _statsDataPerShot->size())
	{
		auto& currStatsData = _statsDataPerShot->at(_pluginState->CurrentRoundIndex);
		handleAttempt(currStatsData.first, false);
		recalculatePercentages(currStatsData.first, currStatsData.second);
	}

	// Update total stats
	// update total stats last because handleAttempt needs to change plugin state, which might not always be the case for per shot stats
	handleAttempt(_playerStats, true);
	recalculatePercentages(_playerStats, _calculatedData);
}

void StatUpdater::processShotReset()
{
	// Update total stats
	recalculatePercentages(_playerStats, _calculatedData);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex &&_pluginState->CurrentRoundIndex < _statsDataPerShot->size())
	{
		auto& currStatsData = _statsDataPerShot->at(_pluginState->CurrentRoundIndex);
		recalculatePercentages(currStatsData.first, currStatsData.second);
	}
}

void StatUpdater::processManualStatReset()
{
	reset();

	// Recalculate total stats
	recalculatePercentages(_playerStats, _calculatedData);

	// Recalculate per shot stats
	for (int i = 0; i < _statsDataPerShot->size(); ++i)
	{
		auto& statsData = _statsDataPerShot->at(i);
		recalculatePercentages(statsData.first, statsData.second);
	}
}

void StatUpdater::initStatsDataPerShot()
{
	// Delete pointers
	_statsDataPerShot->clear();

	// Create pointers
	for (int i = 0; i < _pluginState->TotalRounds; ++i)
	{
		_statsDataPerShot->emplace_back(std::make_shared<PlayerStats>(), std::make_shared<CalculatedData>());
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
	*_playerStats = PlayerStats();
	*_calculatedData = CalculatedData();

	// Reset per shot stats
	if (_pluginState->TotalRounds > 0)
	{
		if (_pluginState->TotalRounds == _statsDataPerShot->size())
		{
			for (int i = 0; i < _statsDataPerShot->size(); ++i)
			{
				auto& statsData = _statsDataPerShot->at(i);
				*(statsData.first) = PlayerStats();
				*(statsData.second) = CalculatedData();
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

void StatUpdater::recalculatePercentages(std::shared_ptr<PlayerStats> playerStats, std::shared_ptr<CalculatedData> calculatedData)
{
	auto successPercentage = .0;
	if (playerStats->Attempts > 0)
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = getPercentageValue(playerStats->Attempts, playerStats->Goals);
	}
	calculatedData->SuccessPercentage = successPercentage;

	// Update the percentage for the last 50 shots
	// Ignore the event if this is a reset after a goal
	while (playerStats->Last50Shots.size() > 50)
	{
		playerStats->Last50Shots.erase(playerStats->Last50Shots.begin());
	}
	successPercentage = .0;
	if (!playerStats->Last50Shots.empty())
	{
		auto numberOfGoals = std::count(playerStats->Last50Shots.begin(), playerStats->Last50Shots.end(), true);
		successPercentage = getPercentageValue((double)playerStats->Last50Shots.size(), (double)numberOfGoals);
	}
	calculatedData->Last50ShotsPercentage = successPercentage;

	// Update the peak percentage only after 20 shots since otherwise a couple of lucky early shots would create a wrong impression
	if (playerStats->Attempts > 20 && calculatedData->Last50ShotsPercentage > calculatedData->PeakSuccessPercentage)
	{
		calculatedData->PeakSuccessPercentage = calculatedData->Last50ShotsPercentage;
		calculatedData->PeakShotNumber = playerStats->Attempts;
	}
}

void StatUpdater::handleGoal(std::shared_ptr<PlayerStats> playerStats)
{
	if (!playerStats->Last50Shots.empty())
	{
		playerStats->Last50Shots.back() = true; // Replace the current attempt with a goal entry
	}
	playerStats->MissStreakCounter = 0;
	playerStats->GoalStreakCounter++;
	playerStats->Goals++;

	// When starting the next attempt, we don't know whether the previous one was a goal or a miss without this flag
	// (there is no trigger for a miss, unless we incorporate the reset trigger for it (we might actually do that in future))
	_pluginState->PreviousAttemptWasAGoal = true;

	if (playerStats->GoalStreakCounter > playerStats->LongestGoalStreak)
	{
		playerStats->LongestGoalStreak = playerStats->GoalStreakCounter;
	}
}

void StatUpdater::handleAttempt(std::shared_ptr<PlayerStats> playerStats, bool changePluginState)
{
	playerStats->Last50Shots.push_back(false);

	// Count the shot attempt in any case
	playerStats->Attempts++;

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
		playerStats->MissStreakCounter++;
		playerStats->GoalStreakCounter = 0;

		if (playerStats->MissStreakCounter > playerStats->LongestMissStreak)
		{
			playerStats->LongestMissStreak = playerStats->MissStreakCounter;
		}
	}
}