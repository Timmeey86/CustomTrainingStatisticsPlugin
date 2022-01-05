#include <pch.h>
#include "StatUpdater.h"

StatUpdater::StatUpdater(std::shared_ptr<PlayerStats> playerStats, std::shared_ptr<CalculatedData> calculatedData, std::shared_ptr<PluginState> pluginState)
	: _playerStats(playerStats)
	, _calculatedData(calculatedData)
	, _pluginState(pluginState)
{
}

void StatUpdater::processGoal()
{
	handleGoal();
	recalculatePercentages();
}

void StatUpdater::processNewAttempt()
{
	handleAttempt();
	recalculatePercentages();
}

void StatUpdater::processShotReset()
{
	recalculatePercentages();
}

void StatUpdater::processManualStatReset()
{
	reset();
	recalculatePercentages();
}

void StatUpdater::handleTrainingPackLoad()
{
	// This is currently not different from manually resetting.
	processManualStatReset();
}

void StatUpdater::reset()
{
	*_playerStats = PlayerStats();
	*_calculatedData = CalculatedData();

	// Do not reset anything which can be changed by user settings - Those settings shall of course not be reset.
	// Also do not reset anything which is based on the Rocket League state
	_pluginState->PreviousAttemptWasAGoal = false;
}

double getPercentageValue(double attempts, double goals)
{
	return round((goals / attempts) * 10000.0) / 100.0;
}

void StatUpdater::recalculatePercentages()
{
	auto successPercentage = .0;
	if (_playerStats->Attempts > 0)
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = getPercentageValue(_playerStats->Attempts, _playerStats->Goals);
	}
	_calculatedData->SuccessPercentage = successPercentage;

	// Update the percentage for the last 50 shots
	// Ignore the event if this is a reset after a goal
	while (_playerStats->Last50Shots.size() > 50)
	{
		_playerStats->Last50Shots.pop_front();
	}
	successPercentage = .0;
	if (!_playerStats->Last50Shots.empty())
	{
		auto numberOfGoals = std::count(_playerStats->Last50Shots.begin(), _playerStats->Last50Shots.end(), true);
		successPercentage = getPercentageValue((double)_playerStats->Last50Shots.size(), (double)numberOfGoals);
	}
	_calculatedData->Last50ShotsPercentage = successPercentage;

	// Update the peak percentage only after 20 shots since otherwise a couple of lucky early shots would create a wrong impression
	if (_playerStats->Attempts > 20 && _calculatedData->Last50ShotsPercentage > _calculatedData->PeakSuccessPercentage)
	{
		_calculatedData->PeakSuccessPercentage = _calculatedData->Last50ShotsPercentage;
		_calculatedData->PeakShotNumber = _playerStats->Attempts;
	}
}

void StatUpdater::handleGoal()
{
	if (!_playerStats->Last50Shots.empty())
	{
		_playerStats->Last50Shots.back() = true; // Replace the current attempt with a goal entry
	}
	_playerStats->MissStreakCounter = 0;
	_playerStats->GoalStreakCounter++;
	_playerStats->Goals++;

	// When starting the next attempt, we don't know whether the previous one was a goal or a miss without this flag
	// (there is no trigger for a miss, unless we incorporate the reset trigger for it (we might actually do that in future))
	_pluginState->PreviousAttemptWasAGoal = true;

	if (_playerStats->GoalStreakCounter > _playerStats->LongestGoalStreak)
	{
		_playerStats->LongestGoalStreak = _playerStats->GoalStreakCounter;
	}
}

void StatUpdater::handleAttempt()
{
	_playerStats->Last50Shots.push_back(false);

	// Count the shot attempt in any case
	_playerStats->Attempts++;

	if (_pluginState->PreviousAttemptWasAGoal)
	{
		// A goal was scored, and then reset was pressed. No further action required, but do not ignore any further resets.
		_pluginState->PreviousAttemptWasAGoal = false;
	}
	else
	{
		_playerStats->MissStreakCounter++;
		_playerStats->GoalStreakCounter = 0;

		if (_playerStats->MissStreakCounter > _playerStats->LongestMissStreak)
		{
			_playerStats->LongestMissStreak = _playerStats->MissStreakCounter;
		}
	}
}