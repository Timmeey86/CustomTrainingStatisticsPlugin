#include <pch.h>
#include "StatUpdater.h"

StatUpdater::StatUpdater(
	std::shared_ptr<ShotStats> shotStats,
	std::shared_ptr<ShotStats> differenceStats,
	std::shared_ptr<PluginState> pluginState,
	std::shared_ptr<IStatReader> statReader,
	std::shared_ptr<AllTimePeakHandler> peakHandler)
	: _externalShotStats(shotStats)
	, _differenceStats(differenceStats)
	, _pluginState(pluginState)
	, _statReader(statReader)
	, _peakHandler(peakHandler)
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

	_statsHaveJustBeenRestored = false;
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

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::processAttempt()
{
	_flipResetOccurredInCurrentAttempt = false;

	_internalShotStats.AllShotStats.Stats.Attempts++;

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		_internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex).Stats.Attempts++;
	}

	updateStatsBackup();
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

	updateStatsBackup();
}

void StatUpdater::processReset(int numberOfShots)
{
	// Reset total stats
	_internalShotStats.AllShotStats.Stats = PlayerStats();
	_internalShotStats.AllShotStats.Data = CalculatedData();
	_flipResetOccurredInCurrentAttempt = false;

	// Reset per shot stats
	_internalShotStats.PerShotStats.clear();
	for (auto index = 0; index < numberOfShots; index++)
	{
		_internalShotStats.PerShotStats.emplace_back();
	}

	// Replace the whole external object with our freshly reset copy
	*_externalShotStats = _internalShotStats;

	// Reset the stats backup (indirectly).
	updateStatsBackup();

	if (_peakHandler)
	{
		_peakHandler->reset();
	}

	_numberOfSessionsToBeSkipped = 0;
	if (_differenceStats)
	{
		updateCompareBase();
	}
}

void StatUpdater::updateData()
{
	// Note: This method relies on the state machine calling it at appropriate moments in time
	//       We do not calculate everything every time, but rather specifically when the state machine deems it necessary.
	//       As long as the state machine works correctly, it is enough to update only the current shot (and the summary object)

	_externalShotStats->AllShotStats = _internalShotStats.AllShotStats;
	recalculatePercentages(_externalShotStats->AllShotStats, _internalShotStats.AllShotStats);

	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		_externalShotStats->PerShotStats.at(_pluginState->CurrentRoundIndex) = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		recalculatePercentages(_externalShotStats->PerShotStats.at(_pluginState->CurrentRoundIndex), _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex));
	}

	if (_differenceStats)
	{
		*_differenceStats = retrieveSessionDiff();
	}
}

ShotStats getPreviousShotStats(std::shared_ptr<IStatReader> statReader, const std::string& trainingPackCode, bool statsAboutToBeRestored, const int numberOfSkips = 0)
{
	if (trainingPackCode.empty()) { return {}; }

	auto resourcePaths = statReader->getAvailableResourcePaths(trainingPackCode);
	auto skippedPacks = 0;
	for (const auto& resourcePath : resourcePaths)
	{
		// Skip any file which only has zero attempts stored
		if (statReader->peekAttemptAmount(resourcePath) == 0) { continue; }
		
		// The file seems valid, check if we're supposed to skip a couple of files
		if (numberOfSkips > skippedPacks)
		{
			skippedPacks++;
			continue; // skip the pack as requested
		}

		// At this point, we have skipped enough files (if any) and have found a valid pack
		return statReader->readStats(resourcePath, statsAboutToBeRestored);
	}

	// At this point, no valid pack has been found, or too many packs have been skipped
	return {};
}

void StatUpdater::restoreLastSession()
{
	auto stats = getPreviousShotStats(_statReader, _trainingPackCode, true);
	if (!stats.hasAttempts())
	{
		return; // We couldn't restore the last session
	}
	// else: we successfully retrieved the previous session stats. Update our data structures with this information

	_internalShotStats.AllShotStats = stats.AllShotStats;
	_internalShotStats.PerShotStats = std::vector<StatsData>(stats.PerShotStats);
	_externalShotStats->AllShotStats = stats.AllShotStats;
	_externalShotStats->PerShotStats = std::vector<StatsData>(stats.PerShotStats);

	recalculatePercentages(_externalShotStats->AllShotStats, _internalShotStats.AllShotStats);
	for (auto index = 0; index < _externalShotStats->PerShotStats.size(); index++)
	{
		recalculatePercentages(_externalShotStats->PerShotStats[index], _internalShotStats.PerShotStats[index]);
	}
	// We successfully restored statistics from the last session. The "Toggle last attempt" feature must be disabled until a goal or a miss was recorded
	// after restoring
	_statsHaveJustBeenRestored = true;
	_numberOfSessionsToBeSkipped = 1;

	// Since we restored the previous session, we must now compare against the one before that 
	// Only do this if we compare to the previous session rather than the all time peak stats, however.
	if (_differenceStats && !_pluginState->StatsShallBeComparedToAllTimePeak)
	{
		updateCompareBase();
	}
}

void StatUpdater::updateCompareBase()
{
	if (_pluginState->StatsShallBeComparedToAllTimePeak)
	{
		if (!_peakHandler)
		{
			return;
		}

		_compareBase = _peakHandler->getPeakStats();
	}
	else
	{
		// Retrieve the previous shot stats, unless the current session had been restored from that file already,
		// in which case we try retrieving the stats before that.
		_compareBase = getPreviousShotStats(_statReader, _trainingPackCode, false, _numberOfSessionsToBeSkipped);

		if (_numberOfSessionsToBeSkipped > 0 && !_compareBase.hasAttempts())
		{
			// There seems to be at most one attempt with valid stats, and we skipped it
			// => Try to fallback to use the session we restored from as a diff (better than nothing)
			_compareBase = getPreviousShotStats(_statReader, _trainingPackCode, false, 0);
		}
	}
	*_differenceStats = retrieveSessionDiff();
}

ShotStats StatUpdater::retrieveSessionDiff() const
{
	if (!_compareBase.hasAttempts())
	{
		// Nothing available to diff with
		return {};
	}

	if (_compareBase.PerShotStats.size() != _externalShotStats->PerShotStats.size())
	{
		// This should never happen since it would mean the amount of shots in the training pack changed between the last two sessions.
		// It could however happen if we don't initiaize something properly so it is here as a last resort.
		return {};
	}

	// At this point we have found a pack we can diff with. We abuse the ShotStats struct in order to store the differences in there
	// since we basically need the same amount of stats.
	ShotStats diffStats;
	diffStats.AllShotStats.Data = _externalShotStats->AllShotStats.Data.getDifferences(_compareBase.AllShotStats.Data);
	diffStats.AllShotStats.Stats = _externalShotStats->AllShotStats.Stats.getDifferences(_compareBase.AllShotStats.Stats);
	diffStats.AllShotStats.Stats.Attempts = 1;	// We set this so we can make use of ShotStats::hasAttempts() in order to check if this is a valid object
												// (yeah, that's kinda hacky).
	for (int index = 0; index < _compareBase.PerShotStats.size(); index++)
	{
		diffStats.PerShotStats.emplace_back();
		diffStats.PerShotStats.back().Data = _externalShotStats->PerShotStats[index].Data.getDifferences(_compareBase.PerShotStats[index].Data);
		diffStats.PerShotStats.back().Stats = _externalShotStats->PerShotStats[index].Stats.getDifferences(_compareBase.PerShotStats[index].Stats);
	}
	return diffStats;
}

void StatUpdater::publishTrainingPackCode(const std::string& trainingPackCode)
{
	_trainingPackCode = trainingPackCode;
}

double getPercentageValue(double attempts, double goals)
{
	return round((goals / attempts) * 10000.0) / 100.0;
}

void StatUpdater::recalculatePercentages(StatsData& statsData, StatsData& internalStatsData)
{
	auto successPercentage = .0;
	auto initialHitPercentage = .0;
	if (internalStatsData.Stats.Attempts > 0)
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = getPercentageValue(internalStatsData.Stats.Attempts, internalStatsData.Stats.Goals);
		initialHitPercentage = getPercentageValue(internalStatsData.Stats.Attempts, internalStatsData.Stats.InitialHits);
	}
	internalStatsData.Data.SuccessPercentage = successPercentage;
	internalStatsData.Data.InitialHitPercentage = initialHitPercentage;

	// Update the percentage for the last 50 shots
	// Ignore the event if this is a reset after a goal
	while (internalStatsData.Stats.Last50Shots.size() > 50)
	{
		internalStatsData.Stats.Last50Shots.erase(internalStatsData.Stats.Last50Shots.begin());
	}
	successPercentage = .0;
	if (!internalStatsData.Stats.Last50Shots.empty())
	{
		auto numberOfGoals = std::count(internalStatsData.Stats.Last50Shots.begin(), internalStatsData.Stats.Last50Shots.end(), true);
		successPercentage = getPercentageValue((double)internalStatsData.Stats.Last50Shots.size(), (double)numberOfGoals);
	}
	internalStatsData.Data.Last50ShotsPercentage = successPercentage;

	// Update the peak percentage only after 20 shots since otherwise a couple of lucky early shots would create a wrong impression
	if (internalStatsData.Stats.Attempts >= 20 && internalStatsData.Data.Last50ShotsPercentage > internalStatsData.Data.PeakSuccessPercentage)
	{
		internalStatsData.Data.PeakSuccessPercentage = internalStatsData.Data.Last50ShotsPercentage;
		internalStatsData.Data.PeakShotNumber = internalStatsData.Stats.Attempts;
	}

	// Update advanced stats
	if (internalStatsData.Stats.Goals > 0)
	{
		internalStatsData.Data.DoubleTapGoalPercentage = getPercentageValue(internalStatsData.Stats.Goals, internalStatsData.Stats.DoubleTapGoals);
		internalStatsData.Data.FlipResetGoalPercentage = getPercentageValue(internalStatsData.Stats.Goals, internalStatsData.Stats.FlipResetAttemptsScored);
	}
	if (internalStatsData.Stats.Attempts > 0)
	{
		internalStatsData.Data.AverageFlipResetsPerAttempt = getPercentageValue(internalStatsData.Stats.Attempts, internalStatsData.Stats.TotalFlipResets);
		internalStatsData.Data.CloseMissPercentage = getPercentageValue(internalStatsData.Stats.Attempts, internalStatsData.Stats.CloseMisses);
	}

	// Update external stats
	statsData = internalStatsData;
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

	statsData.Stats.GoalSpeedStats()->insert(_pluginState->getBallSpeed());

	if (_flipResetOccurredInCurrentAttempt)
	{
		statsData.Stats.FlipResetAttemptsScored++;
	}
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

void StatUpdater::updateStatsBackup()
{
	_previousShotStats.AllShotStats = _internalShotStats.AllShotStats;
	_previousShotStats.PerShotStats = std::vector<StatsData>(_internalShotStats.PerShotStats);
}

void StatUpdater::toggleLastAttempt()
{
	if (!_internalShotStats.hasAttempts() || _internalShotStats.AllShotStats.Stats.Last50Shots.empty())
	{
		return; // can't toggle anything in this case
	}

	if (0 > _pluginState->CurrentRoundIndex || _pluginState->CurrentRoundIndex >= _internalShotStats.PerShotStats.size())
	{
		return; // something is wrong with the current round index, or we are not initialized
	}

	if (_statsHaveJustBeenRestored)
	{
		return; // The user restored stats but didn't complete an attempt after that => there is no "last attempt" we could toggle
	}

	bool lastShotWasAGoal = _internalShotStats.AllShotStats.Stats.Last50Shots.back() > 0;
	_internalShotStats.AllShotStats = _previousShotStats.AllShotStats;
	_internalShotStats.PerShotStats = std::vector<StatsData>(_previousShotStats.PerShotStats);
	if (lastShotWasAGoal)
	{
		processMiss();
	}
	else
	{
		processGoal();
	}
	updateData();
}

void StatUpdater::processAirDribbleTime(float time)
{
	handleAirDribbleTimeUpdate(_internalShotStats.AllShotStats, time);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleAirDribbleTimeUpdate(currStatsData, time);
	}

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::processAirDribbleTouches(int touches)
{
	handleAirDribbleTouchesUpdate(_internalShotStats.AllShotStats, touches);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleAirDribbleTouchesUpdate(currStatsData, touches);
	}

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::processGroundDribbleTime(float time)
{
	handleGroundDribbleTimeUpdate(_internalShotStats.AllShotStats, time);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleGroundDribbleTimeUpdate(currStatsData, time);
	}

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::processDoubleTapGoal()
{
	handleDoubleTapGoalUpdate(_internalShotStats.AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleDoubleTapGoalUpdate(currStatsData);
	}

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::processFlipReset(int amount)
{
	handleFlipResetUpdate(_internalShotStats.AllShotStats, amount);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleFlipResetUpdate(currStatsData, amount);
	}

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::processCloseMiss()
{
	handleCloseMiss(_internalShotStats.AllShotStats);

	// Update per shot
	// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
	if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _internalShotStats.PerShotStats.size())
	{
		auto&& currStatsData = _internalShotStats.PerShotStats.at(_pluginState->CurrentRoundIndex);
		handleCloseMiss(currStatsData);
	}

	_statsHaveJustBeenRestored = false;
}

void StatUpdater::handleAirDribbleTimeUpdate(StatsData& statsData, float time)
{
	if (time > statsData.Stats.MaxAirDribbleTime)
	{
		statsData.Stats.MaxAirDribbleTime = time;
	}
}

void StatUpdater::handleAirDribbleTouchesUpdate(StatsData& statsData, int touches)
{
	if (touches > statsData.Stats.MaxAirDribbleTouches)
	{
		statsData.Stats.MaxAirDribbleTouches = touches;
	}
}

void StatUpdater::handleGroundDribbleTimeUpdate(StatsData& statsData, float time)
{
	if (time > statsData.Stats.MaxGroundDribbleTime)
	{
		statsData.Stats.MaxGroundDribbleTime = time;
	}
}

void StatUpdater::handleDoubleTapGoalUpdate(StatsData& statsData)
{
	statsData.Stats.DoubleTapGoals++;
}

void StatUpdater::handleFlipResetUpdate(StatsData& statsData, int amount)
{
	if (amount > statsData.Stats.MaxFlipResets)
	{
		statsData.Stats.MaxFlipResets = amount;
	}
	statsData.Stats.TotalFlipResets++;
	_flipResetOccurredInCurrentAttempt = true;
}

void StatUpdater::handleCloseMiss(StatsData& statsData)
{
	statsData.Stats.CloseMisses++;
}
