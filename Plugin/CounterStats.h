#pragma once

#include <queue>

/**
* Stores raw gathered data which does not involve calculation
*/
class Stats
{
public:
	int Attempts = 0; ///< Stores the number of attempts made
	int Goals = 0; ///< Stores the number of goals shot
	std::list<bool> Last50Shots; ///< Stores the last 50 shots, where false means a miss and true means a goal
	int GoalStreakCounter = 0; ///< Stores the amount of goals since the last miss
	int MissStreakCounter = 0; ///< Stores the amount of misses since the last goal
	int LongestGoalStreak = 0; ///< Stores the largest amount of consecutively scored goals
	int LongestMissStreak = 0; ///< Stores the largest amount of consecutively scored misses
};

/**
* Stores any kind of data which has been calculated based on the raw stats
*/
class CalculatedData
{
public:
	double SuccessPercentage = .0;
	double PeakSuccessPercentage = .0;
	int PeakShotNumber = 0;
	double Last50ShotsPercentage = .0;
};

/**
* Stores information about the current state of the plugin.
*/
class State
{
public:
	bool PreviousAttemptWasAGoal = false; ///< If this is still false when a new attempt is started, it means the previous shot was a miss.
	bool GoalReplayIsActive = false; ///< True while a goal is being replayed. We ignore other events during that time.
	bool PluginIsEnabled = true; ///< True while the user has not disabled the plugin in the settings UI.
};
