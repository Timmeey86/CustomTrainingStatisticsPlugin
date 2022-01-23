#include "StatUpdaterTestFixture.h"

void StatUpdaterTestFixture::expectTotalStats(const PlayerStats& expectedStats) const
{
	auto allShotStats = _shotStats->AllShotStats.Stats;

	EXPECT_EQ(allShotStats.Attempts, expectedStats.Attempts);
	EXPECT_EQ(allShotStats.Goals, expectedStats.Goals);
	EXPECT_EQ(allShotStats.InitialHits, expectedStats.InitialHits);
	EXPECT_EQ(allShotStats.GoalStreakCounter, expectedStats.GoalStreakCounter);
	EXPECT_EQ(allShotStats.MissStreakCounter, expectedStats.MissStreakCounter);
	EXPECT_EQ(allShotStats.LongestGoalStreak, expectedStats.LongestGoalStreak);
	EXPECT_EQ(allShotStats.LongestMissStreak, expectedStats.LongestMissStreak);
	if (expectedStats.Last50Shots.empty())
	{
		EXPECT_EQ(allShotStats.Last50Shots.empty(), true);
	}
	else
	{
		EXPECT_EQ(allShotStats.Last50Shots.size(), expectedStats.Last50Shots.size());
		EXPECT_EQ(allShotStats.Last50Shots.back(), expectedStats.Last50Shots.back());
	}
}
void StatUpdaterTestFixture::expectPerShotStats(const PlayerStats& expectedStats, int shotNumber) const
{
	// Note: We could reduce the code here by extracting the common parts of expectedTotalStats and expectPerShotStats, but that would 
	// then require custom failure messages as otherwise we wouldn't know which struct is wrong in the test output.
	ASSERT_GT(_shotStats->PerShotStats.size(), shotNumber);

	auto perShotStats = _shotStats->PerShotStats[shotNumber].Stats;
	EXPECT_EQ(perShotStats.Attempts, expectedStats.Attempts);
	EXPECT_EQ(perShotStats.Goals, expectedStats.Goals);
	EXPECT_EQ(perShotStats.InitialHits, expectedStats.InitialHits);
	EXPECT_EQ(perShotStats.GoalStreakCounter, expectedStats.GoalStreakCounter);
	EXPECT_EQ(perShotStats.MissStreakCounter, expectedStats.MissStreakCounter);
	EXPECT_EQ(perShotStats.LongestGoalStreak, expectedStats.LongestGoalStreak);
	EXPECT_EQ(perShotStats.LongestMissStreak, expectedStats.LongestMissStreak);
	if (expectedStats.Last50Shots.empty())
	{
		EXPECT_EQ(perShotStats.Last50Shots.empty(), true);
	}
	else
	{
		EXPECT_EQ(perShotStats.Last50Shots.size(), expectedStats.Last50Shots.size());
		EXPECT_EQ(perShotStats.Last50Shots.back(), expectedStats.Last50Shots.back());
	}
}