#include <pch.h>

#include "AllTimePeakHandler.h"


AllTimePeakHandler::AllTimePeakHandler(
	std::shared_ptr<IStatReader> statReader, 
	std::shared_ptr<IStatWriter> statWriter, 
	std::shared_ptr<PluginState> pluginState, 
	std::shared_ptr<ShotStats> shotStats) 
	: _statReader( statReader )
	, _statWriter( statWriter)
	, _pluginState( pluginState )
	, _currentStats( shotStats )

{
}

void copyGoalSpeedStats(const std::shared_ptr<IGoalSpeedProvider>& sourceProvider, const std::shared_ptr<FakeGoalSpeedProvider>& fakeStorage)
{
	fakeStorage->setFakeMin(sourceProvider->getMin());
	fakeStorage->setFakeMax(sourceProvider->getMax());
	fakeStorage->setFakeMean(sourceProvider->getMean());
	fakeStorage->setFakeMedian(sourceProvider->getMedian());
}

void copyGoalSpeedStatsIfHigher(const std::shared_ptr<IGoalSpeedProvider>& sourceProvider, const std::shared_ptr<FakeGoalSpeedProvider>& fakeStorage)
{
	if (fakeStorage->getMin() < sourceProvider->getMin()) { fakeStorage->setFakeMin(sourceProvider->getMin()); }
	if (fakeStorage->getMax() < sourceProvider->getMax()) { fakeStorage->setFakeMax(sourceProvider->getMax()); }
	if (fakeStorage->getMean() < sourceProvider->getMean()) { fakeStorage->setFakeMean(sourceProvider->getMean()); }
	if (fakeStorage->getMedian() < sourceProvider->getMedian()) { fakeStorage->setFakeMedian(sourceProvider->getMedian()); }
}

template<typename T>
void copyIfHigher(const T& newValue, T& localValue)
{
	if (newValue > localValue)
	{
		localValue = newValue;
	}
}
void copyMaxStats(const StatsData& source, StatsData& localStats, std::shared_ptr<FakeGoalSpeedProvider> localGoalSpeed)
{
	// Do not modify stats where comparing doesn't make much sense / would produce conflicting results
	// - Attempts
	// - Goals
	// - Initial Hits
	// - Last50Shots
	// - Last 50 shot percentage
	// - GoalStreakCounter
	// - MissStreakCounter
	// - Close Misses
	// - Total Flip Resets
	// - Close Miss Percentage
	// - Peak Shot number
	// - Latest Goal Speed
	// - LongestMissStreak (If the user does 5 shots with zero misses, and then 200 shots with one miss, is that really worse?)

	// Copy over any other stats
	copyIfHigher(source.Stats.LongestGoalStreak, localStats.Stats.LongestGoalStreak);
	copyGoalSpeedStatsIfHigher(source.Stats.GoalSpeedStats(), localGoalSpeed);
	copyIfHigher(source.Stats.MaxAirDribbleTouches, localStats.Stats.MaxAirDribbleTouches);
	copyIfHigher(source.Stats.MaxAirDribbleTime, localStats.Stats.MaxAirDribbleTime);
	copyIfHigher(source.Stats.MaxGroundDribbleTime, localStats.Stats.MaxGroundDribbleTime);
	copyIfHigher(source.Stats.DoubleTapGoals, localStats.Stats.DoubleTapGoals);
	copyIfHigher(source.Stats.MaxFlipResets, localStats.Stats.MaxFlipResets);
	copyIfHigher(source.Stats.FlipResetAttemptsScored, localStats.Stats.FlipResetAttemptsScored);

	// Copy over calculated data
	copyIfHigher(source.Data.AverageFlipResetsPerAttempt, localStats.Data.AverageFlipResetsPerAttempt);
	copyIfHigher(source.Data.DoubleTapGoalPercentage, localStats.Data.DoubleTapGoalPercentage);
	copyIfHigher(source.Data.FlipResetGoalPercentage, localStats.Data.FlipResetGoalPercentage);
	copyIfHigher(source.Data.InitialHitPercentage, localStats.Data.InitialHitPercentage);
	copyIfHigher(source.Data.PeakSuccessPercentage, localStats.Data.PeakSuccessPercentage);
	copyIfHigher(source.Data.SuccessPercentage, localStats.Data.SuccessPercentage);
}

void AllTimePeakHandler::reset()
{
	// Read the all time stats from the file. If that doesn't work, use the current stats (most likely empty) as peak base
	if (!readAllStatFile())
	{
		copyStats(*_currentStats, false /* stats weren't restored from file */);
	}
}

void restoreGoalSpeedFromFile(const PlayerStats& statsFromFile, const std::shared_ptr<FakeGoalSpeedProvider> fakeProvider)
{
	fakeProvider->setFakeMin(statsFromFile.MinGoalSpeedFromFile);
	fakeProvider->setFakeMax(statsFromFile.MaxGoalSpeedFromFile);
	fakeProvider->setFakeMean(statsFromFile.MeanGoalSpeedFromFile);
	fakeProvider->setFakeMedian(statsFromFile.MedianGoalSpeedFromFile);
}
void AllTimePeakHandler::copyStats(const ShotStats& source, bool statsWereRestoredFromFile)
{
	// Looks like this is called the first time, or the pack has changed. Store everything
	_allTimePeakStats = source;

	// Set attempts to 1. We only use it to detect whether or not we were able to restore the file
	_allTimePeakStats.AllShotStats.Stats.Attempts = 1;

	// We have to do a little more work for goal speed: We don't want to store the actual goal stats, but rather the
	// single values of min, max, mean and median distinctly
	_allTimePeakStats.AllShotStats.Stats.setGoalSpeedProvider(_allStatGoalSpeedProvider);
	if (statsWereRestoredFromFile)
	{
		restoreGoalSpeedFromFile(source.AllShotStats.Stats, _allStatGoalSpeedProvider);
	}
	else
	{
		copyGoalSpeedStats(source.AllShotStats.Stats.GoalSpeedStats(), _allStatGoalSpeedProvider);
	}

	_perShotGoalSpeedProviders.clear();
	for (auto shotNumber = 0; shotNumber < _allTimePeakStats.PerShotStats.size(); shotNumber++)
	{
		auto fakeProvider = std::make_shared<FakeGoalSpeedProvider>();
		if (statsWereRestoredFromFile)
		{
			restoreGoalSpeedFromFile(source.PerShotStats[shotNumber].Stats, fakeProvider);
		}
		else
		{
			copyGoalSpeedStats(source.PerShotStats[shotNumber].Stats.GoalSpeedStats(), fakeProvider);
		}
		_allTimePeakStats.PerShotStats[shotNumber].Stats.setGoalSpeedProvider(fakeProvider);
		_perShotGoalSpeedProviders.push_back(fakeProvider);
	}
}

void AllTimePeakHandler::updateMaximumStats()
{
	auto statsHaveBeenAdapted = false;
	if (_currentStats->PerShotStats.size() != _allTimePeakStats.PerShotStats.size())
	{
		// First call or the training pack changed, or the file got corrupted => reset
		reset();
		statsHaveBeenAdapted = true;
	}
	else
	{
		// Start storing peak stats only after hitting 20 shots. Otherwise, the first shot could set everything to 100% if it is a goal
		if (_currentStats->AllShotStats.Stats.Attempts >= 20)
		{
			copyMaxStats(_currentStats->AllShotStats, _allTimePeakStats.AllShotStats, _allStatGoalSpeedProvider);
			for (auto shotNumber = 0; shotNumber < _allTimePeakStats.PerShotStats.size(); shotNumber++)
			{
				copyMaxStats(_currentStats->PerShotStats.at(shotNumber), _allTimePeakStats.PerShotStats[shotNumber], _perShotGoalSpeedProviders[shotNumber]);
			}
			statsHaveBeenAdapted = true;
		}
	}

	if (statsHaveBeenAdapted)
	{
		writeAllStatFile();
	}
}

ShotStats AllTimePeakHandler::getPeakStats() const
{
	auto newStats = ShotStats{ _allTimePeakStats };

	// We don't want the stats to be automatically updated, so we need to replace fake goal speed providers
	auto fakeProvider = std::make_shared<FakeGoalSpeedProvider>();
	newStats.AllShotStats.Stats.setGoalSpeedProvider(fakeProvider);
	copyGoalSpeedStats(_allTimePeakStats.AllShotStats.Stats.GoalSpeedStats(), fakeProvider);

	for (auto shotNumber = 0; shotNumber < newStats.PerShotStats.size(); shotNumber++)
	{
		fakeProvider = std::make_shared<FakeGoalSpeedProvider>();
		newStats.PerShotStats[shotNumber].Stats.setGoalSpeedProvider(fakeProvider);
		copyGoalSpeedStats(_allTimePeakStats.PerShotStats[shotNumber].Stats.GoalSpeedStats(), fakeProvider);
	}

	return newStats;
}

bool AllTimePeakHandler::writeAllStatFile()
{
	if (_pluginState->TrainingPackCode.empty()) { return true; }

	auto fileName = _pluginState->TrainingPackCode;
	_statWriter->writeTrainingPackStatistics(_allTimePeakStats, _pluginState->TrainingPackCode);
	return true;
}

bool AllTimePeakHandler::readAllStatFile()
{
	if (_pluginState->TrainingPackCode.empty()) { return false; }

	auto stats = _statReader->readTrainingPackStatistics(_pluginState->TrainingPackCode);
	if (!stats.hasAttempts())
	{
		// We couldn't restore anything
		return false;
	}

	// Else: restore the all time peak stats from the file
	copyStats(stats, true /* stats were restored from file. */);
	return true;
}
