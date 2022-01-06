#pragma once

#include <vector>

/**
* Stores raw gathered data which does not involve calculation
*/
class PlayerStats
{
public:
	PlayerStats() = default;

	int Attempts = 0;				///< Stores the number of attempts made
	int Goals = 0;					///< Stores the number of goals shot
	std::vector<bool> Last50Shots;	///< Stores the last 50 shots, where false means a miss and true means a goal
	int GoalStreakCounter = 0;		///< Stores the amount of goals since the last miss
	int MissStreakCounter = 0;		///< Stores the amount of misses since the last goal
	int LongestGoalStreak = 0;		///< Stores the largest amount of consecutively scored goals
	int LongestMissStreak = 0;		///< Stores the largest amount of consecutively scored misses
};