#pragma once

#include <vector>
#include <memory>
#include "IGoalSpeedProvider.h"
#include "GoalSpeed.h"

/**
 * Stores differences of goal speed values.
 */
struct GoalSpeedDiff
{
public:
	float MinValue;
	float MaxValue;
	float MedianValue;
	float MeanValue;
	float StdDevValue;
};

/**
* Stores raw gathered data which does not involve calculation
*/
class PlayerStats
{
public:
	PlayerStats() = default;

	int Attempts = 0;						///< Stores the number of attempts made
	int Goals = 0;							///< Stores the number of goals shot
	std::vector<bool> Last50Shots;			///< Stores the last 50 shots, where false means a miss and true means a goal
	int GoalStreakCounter = 0;				///< Stores the amount of goals since the last miss
	int MissStreakCounter = 0;				///< Stores the amount of misses since the last goal
	int LongestGoalStreak = 0;				///< Stores the largest amount of consecutively scored goals
	int LongestMissStreak = 0;				///< Stores the largest amount of consecutively scored misses
	int InitialHits = 0;					///< Stores the number of times the ball was hit at least once during an attempt.
	int MaxAirDribbleTouches = 0;			///< Stores the maximum amount of air dribble touches made during any attempt.
	float MaxAirDribbleTime = .0;			///< Stores the maximum air dribble time achieved during any attempt.
	float MaxGroundDribbleTime = .0;		///< Stores the maximum ground dribble time achieved during any attempt.
	int DoubleTapGoals = 0;					///< Stores the number of double tap goals scored.
	int TotalFlipResets = 0;				///< Stores the total number of flip resets made.
	int MaxFlipResets = 0;					///< Stores the maximum number of flip resets made during any attempt.
	int FlipResetAttemptsScored = 0;		///< Stores the number of attempts which included at least one flip reset and resulted in a goal
	int CloseMisses = 0;					///< Stores the number of attempts which almost resulted in a goal.

	inline void setGoalSpeedProvider(std::shared_ptr<IGoalSpeedProvider> provider) { _goalSpeedProvider = provider; } ///< Allows replacing the internal goal speed provider.

	inline std::shared_ptr<IGoalSpeedProvider> GoalSpeedStats() const { return _goalSpeedProvider; } ///< Provides statistics about the goal speed
	GoalSpeedDiff GoalSpeedDifference;		///< This is not the best place for these kind of statistics, but it avoids heavy refactoring

	// These are required for restoring goal speed values of the "all time peak stats". 
	// This could be made less ugly by moving the GoalSpeedProvider completely out of this class, and have some other class update PlayerStats whenever goal speed values change.
	float MinGoalSpeedFromFile = .0f;
	float MaxGoalSpeedFromFile = .0f;
	float MedianGoalSpeedFromFile = .0f;
	float MeanGoalSpeedFromFile = .0f;

	/** Compares the goal speed values of this object to other and returns the result as a GoalSpeedDiff instance. */
	GoalSpeedDiff getGoalSpeedDifferences(const PlayerStats& other) const
	{
		GoalSpeedDiff diff;
		diff.MinValue = _goalSpeedProvider->getMin() - other._goalSpeedProvider->getMin();
		diff.MaxValue = _goalSpeedProvider->getMax() - other._goalSpeedProvider->getMax();
		diff.MedianValue = _goalSpeedProvider->getMedian() - other._goalSpeedProvider->getMedian();
		diff.MeanValue = _goalSpeedProvider->getMean() - other._goalSpeedProvider->getMean();
		diff.StdDevValue = _goalSpeedProvider->getStdDev() - other._goalSpeedProvider->getStdDev();
		return diff;
	}

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
		diff.MaxAirDribbleTouches = MaxAirDribbleTouches - other.MaxAirDribbleTouches;
		diff.MaxAirDribbleTime = MaxAirDribbleTime - other.MaxAirDribbleTime;
		diff.MaxGroundDribbleTime = MaxGroundDribbleTime - other.MaxGroundDribbleTime;
		diff.DoubleTapGoals = DoubleTapGoals - other.DoubleTapGoals;
		diff.TotalFlipResets = TotalFlipResets - other.TotalFlipResets;
		diff.MaxFlipResets = MaxFlipResets - other.MaxFlipResets;
		diff.FlipResetAttemptsScored = FlipResetAttemptsScored - other.FlipResetAttemptsScored;
		diff.GoalSpeedDifference = getGoalSpeedDifferences(other);
		// we don't compare close misses since a lower number could be better (more goals scored) or worse (missed the goal completely more often)
		return diff;
	}

private:
	std::shared_ptr<IGoalSpeedProvider> _goalSpeedProvider = std::make_shared<GoalSpeed>();
};