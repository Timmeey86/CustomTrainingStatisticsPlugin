#pragma once

#include <vector>
#include "GoalSpeed.h"

/**
* Stores raw gathered data which does not involve calculation
*/
class PlayerStats
{
public:
	PlayerStats() = default;

	int Attempts = 0;					///< Stores the number of attempts made
	int Goals = 0;						///< Stores the number of goals shot
	std::vector<bool> Last50Shots;		///< Stores the last 50 shots, where false means a miss and true means a goal
	int GoalStreakCounter = 0;			///< Stores the amount of goals since the last miss
	int MissStreakCounter = 0;			///< Stores the amount of misses since the last goal
	int LongestGoalStreak = 0;			///< Stores the largest amount of consecutively scored goals
	int LongestMissStreak = 0;			///< Stores the largest amount of consecutively scored misses
	int InitialHits = 0;				///< Stores the number of times the ball was hit at least once during an attempt.
	GoalSpeed GoalSpeedStats;			///< Stores statistics about the goal speed
	int MaxAirDribbleTouches = 0;		///< Stores the maximum amount of air dribble touches made during any attempt.
	float MaxAirDribbleTime = .0;		///< Stores the maximum air dribble time achieved during any attempt.
	float MaxGroundDribbleTime = .0;	///< Stores the maximum ground dribble time achieved during any attempt.
	int DoubleTapGoals = 0;				///< Stores the number of double tap goals scored.
	int TotalFlipResets = 0;			///< Stores the total number of flip resets made.
	int MaxFlipResets = 0;				///< Stores the maximum number of flip resets made during any attempt.
	int FlipResetAttemptsScored = 0;	///< Stores the number of attempts which included at least one flip reset and resulted in a goal

	/** Compares this object to other and returns the result as a new PlayerStats instance.
	 *  The resulting percentages will be positive if "this" is better than "other".
	 * 
	 * We only compare values which make sense to be compared, i.e. we don't compare stats which reference only a part of the shots made.
	 * We also don't compare goal speed stats at the moment since we currently can't restore them anyway.
	 */
	PlayerStats getDifferences(const PlayerStats& other) const 
	{
		PlayerStats diff;
		diff.Goals = Goals - other.Goals;
		diff.LongestGoalStreak = LongestGoalStreak - other.LongestGoalStreak;
		diff.LongestMissStreak = other.LongestMissStreak - LongestMissStreak; // We turn this value around so a positive value is something good, like with the other stats
		diff.InitialHits = InitialHits - other.InitialHits;
		return diff;
	}
};