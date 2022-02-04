#pragma once

#include <string>

/** Defines strings to be used within the stat files. */
class StatFileDefs
{
public:

	static const std::string CurrentVersionNumber;

	static const std::string Version;

	static const std::string NumberOfShots;
	static const std::string ShotSeparator;

	static const std::string Attempts;
	static const std::string Goals;
	static const std::string InitialHits;
	static const std::string InitialHitPercentage;
	static const std::string CurrentGoalStreak;
	static const std::string CurrentMissStreak;
	static const std::string TotalSuccessRate;
	static const std::string LongestGoalStreak;
	static const std::string LongestMissStreak;
	static const std::string PeakSuccessRate;
	static const std::string PeakAtShotNumber;
	static const std::string LastNShotsPercentage;
	static const std::string LatestGoalSpeed;
	static const std::string MaxGoalSpeed;
	static const std::string MinGoalSpeed;
	static const std::string MedianGoalSpeed;
	static const std::string MeanGoalSpeed;
};