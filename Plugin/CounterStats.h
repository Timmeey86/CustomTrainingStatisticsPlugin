#pragma once

#include <queue>

// Stores statistics about scored goals, number of attempts, success percentage etc
class CounterStats
{
public:
	int Attempts = 0;
	int Goals = 0;
	int GoalStreakCounter = 0;
	int MissStreakCounter = 0;
	int LongestGoalStreak = 0;
	int LongestMissStreak = 0;
	bool IgnoreNextShotReset = false;
	double SuccessPercentage = .0;
	double PeakSuccessPercentage = .0;
	int PeakShotNumber = 0;
	std::list<bool> Last50Shots;
	double Last50ShotsPercentage = .0;
};
