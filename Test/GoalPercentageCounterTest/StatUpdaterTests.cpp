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

	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded

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
	// Everything else should be default

	// Act
	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded
	statUpdater->processAttempt();
	statUpdater->updateData();

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
	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded
	statUpdater->processAttempt();
	statUpdater->processInitialBallHit(); // Need to touch the ball to shoot a goal, right?
	statUpdater->processGoal();
	statUpdater->updateData();

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
	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded
	statUpdater->processAttempt();
	statUpdater->processMiss();
	statUpdater->updateData();

	// Assert
	expectTotalStats(expectedStats);
	expectPerShotStats(expectedStats, 0);
	expectPerShotStats(defaultStats, 1);
}
