#pragma once

// Stores statistics about scored goals, number of attempts, success percentage etc
class CounterStats
{
public:
	int Attempts;
	int Goals;
	int GoalStreakCounter;
	int MissStreakCounter;
	int LongestGoalStreak;
	int LongestMissStreak;
	bool IgnoreNextShotReset;
};

