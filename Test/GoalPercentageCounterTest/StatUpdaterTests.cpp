#include "Fixtures/StatUpdaterTestFixture.h"


TEST_F(StatUpdaterTestFixture, verify_fixture)
{
	// Make sure all the stats are zero/empty at the start. This allows creating a default object of PlayerStats and only setting expectations for
	// values which are not zero/empty.
	PlayerStats expectedStats;
	expectedStats.Attempts = 0;
	expectedStats.Goals = 0;
	expectedStats.InitialHits = 0;
	expectedStats.GoalStreakCounter = 0;
	expectedStats.MissStreakCounter = 0;
	expectedStats.LongestGoalStreak = 0;
	expectedStats.LongestMissStreak = 0;
	expectedStats.Last50Shots.clear();

	expectTotalStats(expectedStats);
	expectPerShotStats(expectedStats, 0);
	expectPerShotStats(expectedStats, 1);
}

TEST_F(StatUpdaterTestFixture, new_attempt_increases_attempts)
{
	// Arrange
	PlayerStats defaultStats;
	PlayerStats expectedStats;
	expectedStats.Attempts = 1;
	expectedStats.Last50Shots.push_back(false); // Currently, an attempt is pushed as false, and then changed to true if it is a goal
	// Everything else should be default

	// Act
	statUpdater->processNewAttempt();

	// Assert
	expectTotalStats(expectedStats);
	expectPerShotStats(expectedStats, 0);
	expectPerShotStats(defaultStats, 1);
}
TEST_F(StatUpdaterTestFixture, goal_attempt_increases_goals)
{
	// Arrange
	PlayerStats defaultStats;
	PlayerStats expectedStats;
	expectedStats.Attempts = 1;
	expectedStats.Goals = 1;
	expectedStats.InitialHits = 1;
	expectedStats.GoalStreakCounter = 1;
	expectedStats.LongestGoalStreak = 1;
	expectedStats.Last50Shots.push_back(true);

	// Act
	statUpdater->processNewAttempt();
	statUpdater->processInitialBallHit(); // Need to touch the ball to shoot a goal, right?
	statUpdater->processGoal();

	// Assert
	expectTotalStats(expectedStats);
	expectPerShotStats(expectedStats, 0);
	expectPerShotStats(defaultStats, 1);
}

TEST_F(StatUpdaterTestFixture, miss_attempt_increases_misses)
{
	// Arrange
	PlayerStats defaultStats;
	PlayerStats expectedStats;
	expectedStats.Attempts = 1;
	expectedStats.InitialHits = 0; // We simulate a complete miss
	expectedStats.MissStreakCounter = 1;
	expectedStats.LongestMissStreak = 1;
	expectedStats.Last50Shots.push_back(false);

	// Act
	statUpdater->processNewAttempt();
	statUpdater->processShotReset();

	// Assert
	expectTotalStats(expectedStats);
	expectPerShotStats(expectedStats, 0);
	expectPerShotStats(defaultStats, 1);
}

TEST_F(StatUpdaterTestFixture, manual_stat_reset_resets_everything)
{
	// Arrange
	ASSERT_TRUE(_shotStats != nullptr);
	ASSERT_GE(_shotStats->PerShotStats.size(), 2);

	// Set arbitrary values in all stats data structures
	auto& allShotStats = _shotStats->AllShotStats;
	auto& firstShotStats = _shotStats->PerShotStats[0];
	auto& secondShotStats = _shotStats->PerShotStats[1];
	
	auto fillShotStatsFunc = [](StatsData& statsData) {
		statsData.Stats.Attempts = 42;
		statsData.Stats.Goals = 42;
		statsData.Stats.GoalStreakCounter = 42;
		statsData.Stats.InitialHits = 42;
		statsData.Stats.Last50Shots.push_back(false);
		statsData.Stats.Last50Shots.push_back(true);
		statsData.Stats.Last50Shots.push_back(true);
		statsData.Stats.Last50Shots.push_back(false);
		statsData.Stats.LongestGoalStreak = 42;
		statsData.Stats.LongestMissStreak = 42;
		statsData.Stats.MissStreakCounter = 42;
	};
	fillShotStatsFunc(allShotStats);
	fillShotStatsFunc(firstShotStats);
	fillShotStatsFunc(secondShotStats);

	PlayerStats defaultStats;

	// Act
	statUpdater->processManualStatReset();

	// Assert
	expectTotalStats(defaultStats);
	expectPerShotStats(defaultStats, 0);
	expectPerShotStats(defaultStats, 1);
}
