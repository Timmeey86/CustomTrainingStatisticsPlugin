#include "Fixtures/StatUpdaterTestFixture.h"

using ::testing::Return;

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

TEST_F(StatUpdaterTestFixture, the_51st_goal_removes_oldest_last_50_shots_entry)
{
	// Arrange
	PlayerStats defaultStats;
	PlayerStats expectedStats;
	expectedStats.Attempts = 51;
	expectedStats.Goals = 50;
	expectedStats.GoalStreakCounter = 50;
	expectedStats.InitialHits = 50;
	expectedStats.LongestGoalStreak = 50;
	expectedStats.LongestMissStreak = 1;
	expectedStats.MissStreakCounter = 0;
	
	// Act
	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded

	// Record one miss and then 50 goals
	statUpdater->processAttempt();
	statUpdater->processMiss();
	// We don't push the miss to Last50Shots since we expect it to be gone after 50 goals
	
	for (auto index = 0; index < 50; index++)
	{
		statUpdater->processAttempt();
		statUpdater->processInitialBallHit();
		statUpdater->processGoal();
		statUpdater->updateData();
		expectedStats.Last50Shots.push_back(true);
	}

	expectTotalStats(expectedStats);
	expectPerShotStats(expectedStats, 0);
	expectPerShotStats(defaultStats, 1);
}

TEST_F(StatUpdaterTestFixture, restoringStats_when_noFilesArePresent_will_returnDefaultStats)
{
	// Arrange
	// We expect the stat updater to ask the stat reader for the list of storage files for the current training pack.
	// In this test, we act like no files would be present
	EXPECT_CALL(*_statReader, getAvailableResourcePaths(FakeTrainingPackCode))
		.WillOnce(Return(std::vector<std::string>{}));

	PlayerStats defaultStats;

	// We expect the other methods to not be called in this case (::testing::StrictMock will check this for us)

	// Act
	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded
	statUpdater->restoreLastSession();

	// Assert
	expectTotalStats(defaultStats);
	expectPerShotStats(defaultStats, 0);
	expectPerShotStats(defaultStats, 1);
}

TEST_F(StatUpdaterTestFixture, restoringStats_when_allFilesHaveZeroAttempts_will_returnDefaultStats)
{
	// Arrange
	const std::string firstFilePath = "path/to/first/file";
	const std::string secondFilePath = u8"здравствуйте/こんにちは"; // We use this to make sure we don't accidentally lose UTF-8

	PlayerStats defaultStats;

	// We expect the stat updater to ask the stat reader for the list of storage files for the current training pack.
	// In this test, we act like two files would be present
	EXPECT_CALL(*_statReader, getAvailableResourcePaths(FakeTrainingPackCode))
		.WillOnce(Return(std::vector<std::string>{ firstFilePath, secondFilePath }));

	// Since we provide two files, we expect the stat updater to peek for the amount of attempts in these files
	EXPECT_CALL(*_statReader, peekAttemptAmount(firstFilePath))
		.WillOnce(Return(0));
	EXPECT_CALL(*_statReader, peekAttemptAmount(secondFilePath))
		.WillOnce(Return(0));

	// Since neither of the files provide an amount, we expect no call to readStats() (::testing::StrictMock will check this for us)

	// Act
	statUpdater->processReset(_pluginState->TotalRounds); // A reset is always sent when a new training pack is being loaded
	statUpdater->restoreLastSession();

	// Assert
	expectTotalStats(defaultStats);
	expectPerShotStats(defaultStats, 0);
	expectPerShotStats(defaultStats, 1);
}

TEST_F(StatUpdaterTestFixture, restoringStats_when_firstFileHasMoreThanZeroAttempts_will_readFirstFile)
{
	// Arrange
	const std::string firstFilePath = "path/to/first/file";
	const std::string secondFilePath = u8"здравствуйте/こんにちは"; // We use this to make sure we don't accidentally lose UTF-8

	// For now we only test a small number of fields and assume that if they are reported correctly, then the rest will be reported correctly, too.
	// The main point of this test is to test the control flow
	ShotStats dummyStats;
	dummyStats.PerShotStats.emplace_back();
	dummyStats.AllShotStats.Stats.Attempts = 42;
	dummyStats.PerShotStats[0].Stats.Attempts = 21;

	// We expect the stat updater to ask the stat reader for the list of storage files for the current training pack.
	// In this test, we act like two files would be present
	EXPECT_CALL(*_statReader, getAvailableResourcePaths(FakeTrainingPackCode))
		.WillOnce(Return(std::vector<std::string>{ firstFilePath, secondFilePath }));

	// Since the first file will provide an amount, we expect only one call on peekAttemptAmount
	EXPECT_CALL(*_statReader, peekAttemptAmount(firstFilePath))
		.WillOnce(Return(dummyStats.AllShotStats.Stats.Attempts));

	// We expect a call which tries to read from the first file
	EXPECT_CALL(*_statReader, readStats(firstFilePath, true))
		.WillOnce(Return(dummyStats));

	// Act
	statUpdater->processReset(1); // Simulate a pack with only one shot
	statUpdater->restoreLastSession();

	// Assert
	expectTotalStats(dummyStats.AllShotStats.Stats);
	expectPerShotStats(dummyStats.PerShotStats[0].Stats, 0);
}

// This test case is actually the most common use case:
// When loading a training pack, a new file will instantly be created, with default stats (zero attempts)
// If the user clicks restore now, they'll want to restore the state of the day before, which means the second file has to be read, rather than the most recent one.
TEST_F(StatUpdaterTestFixture, restoringStats_when_secondFileHasMoreThanZeroAttempts_will_readSecondFile)
{
	// Arrange
	const std::string firstFilePath = "path/to/first/file";
	const std::string secondFilePath = u8"здравствуйте/こんにちは"; // We use this to make sure we don't accidentally lose UTF-8

	// For now we only test a small number of fields and assume that if they are reported correctly, then the rest will be reported correctly, too.
	// The main point of this test is to test the control flow
	ShotStats dummyStats;
	dummyStats.PerShotStats.emplace_back();
	dummyStats.PerShotStats.emplace_back();
	dummyStats.AllShotStats.Stats.Attempts = 42;
	dummyStats.PerShotStats[0].Stats.Attempts = 21;
	dummyStats.PerShotStats[1].Stats.Attempts = 84;

	// We expect the stat updater to ask the stat reader for the list of storage files for the current training pack.
	// In this test, we act like two files would be present
	EXPECT_CALL(*_statReader, getAvailableResourcePaths(FakeTrainingPackCode))
		.WillOnce(Return(std::vector<std::string>{ firstFilePath, secondFilePath }));

	// Since the first file will provide zero attempts, the second one shall be read instead
	EXPECT_CALL(*_statReader, peekAttemptAmount(firstFilePath))
		.WillOnce(Return(0));
	EXPECT_CALL(*_statReader, peekAttemptAmount(secondFilePath))
		.WillOnce(Return(dummyStats.AllShotStats.Stats.Attempts));

	// We expect a call which tries to read from the first file
	EXPECT_CALL(*_statReader, readStats(secondFilePath, true))
		.WillOnce(Return(dummyStats));

	// Act
	statUpdater->processReset(2); // Simulate a pack with two shots
	statUpdater->restoreLastSession();

	// Assert
	expectTotalStats(dummyStats.AllShotStats.Stats);
	expectPerShotStats(dummyStats.PerShotStats[0].Stats, 0);
	expectPerShotStats(dummyStats.PerShotStats[1].Stats, 1);
}

TEST_F(StatUpdaterTestFixture, restoringStats_when_noTrainingPackCodeIsSet_will_notReadFiles)
{
	// Note: Since statUpdater uses a strict mock of statReader in the test fixture, and we set no expectations,
	//       any call on statReader would fail the test

	statUpdater->processReset(0);
	statUpdater->publishTrainingPackCode({});
	statUpdater->restoreLastSession();
}

TEST_F(StatUpdaterTestFixture, togglingLastShot_when_noAttemptsHaveBeenMade_will_notChangeAnything)
{
	PlayerStats defaultStats;

	statUpdater->processReset(2); // Simulate a pack with two shots
	statUpdater->toggleLastAttempt();

	expectTotalStats(defaultStats);
	expectPerShotStats(defaultStats, 0);
	expectPerShotStats(defaultStats, 1);
}

TEST_F(StatUpdaterTestFixture, togglingLastShot_when_attemptsHaveBeenMade_will_adaptStats)
{
	// Expect a goal
	PlayerStats oneGoalStats;
	PlayerStats defaultStats;
	oneGoalStats.Attempts = 1;
	oneGoalStats.Goals = 1;
	oneGoalStats.InitialHits = 1;
	oneGoalStats.GoalStreakCounter = 1;
	oneGoalStats.LongestGoalStreak = 1;
	oneGoalStats.Last50Shots.push_back(true);

	const auto goalSpeed = 100.0f;
	oneGoalStats.GoalSpeedStats.insert(goalSpeed);
	_pluginState->setBallSpeed(goalSpeed / PluginState::UE_UNITS_TO_KPH); // We need to convert it to Unreal Engine units when setting it

	// Record a miss and then toggle it
	statUpdater->processReset(2); // Simulate a pack with two shots
	statUpdater->processAttempt();
	statUpdater->processInitialBallHit();
	statUpdater->processMiss();
	statUpdater->updateData();

	ASSERT_EQ(_shotStats->AllShotStats.Stats.GoalSpeedStats.getCount(), 0);

	statUpdater->toggleLastAttempt();


	expectTotalStats(oneGoalStats);
	expectPerShotStats(oneGoalStats, 0);
	expectPerShotStats(defaultStats, 1);

	EXPECT_EQ(_shotStats->AllShotStats.Stats.GoalSpeedStats.getCount(), 1);
	EXPECT_EQ(_shotStats->AllShotStats.Stats.GoalSpeedStats.getMax(), goalSpeed);
	EXPECT_EQ(_shotStats->PerShotStats[0].Stats.GoalSpeedStats.getCount(), 1);
	EXPECT_EQ(_shotStats->PerShotStats[0].Stats.GoalSpeedStats.getMax(), goalSpeed);
}

// Peak percentage is supposed to not be calculated before 20 attempts have been made
TEST_F(StatUpdaterTestFixture, scoring_when_lessThan20AttemptsHaveBeenMade_will_notAffectPeakPercentage)
{
	statUpdater->processReset(2);
	for (auto i = 0; i < 19; i++)
	{
		statUpdater->processAttempt();
		statUpdater->processInitialBallHit();
		statUpdater->processGoal();
		statUpdater->updateData();

		EXPECT_EQ(_shotStats->AllShotStats.Data.PeakSuccessPercentage, 0);
	}
}

// Peak percentage should be calculated when exactly 20 shots have been made
TEST_F(StatUpdaterTestFixture, finishingAttempt_when_doing20thAttempt_will_calculatePeakPercentage)
{
	// Score one goal and 19 misses
	statUpdater->processReset(2);
	statUpdater->processAttempt();
	statUpdater->processInitialBallHit();
	statUpdater->processGoal();
	statUpdater->updateData();
	
	for (auto i = 0; i < 19; i++)
	{
		statUpdater->processAttempt();
		statUpdater->processMiss();
		statUpdater->updateData();
	}

	EXPECT_FLOAT_EQ(_shotStats->AllShotStats.Data.PeakSuccessPercentage, 5.0f);
}

// Peak percentage should not go down when the total percentage goes down
TEST_F(StatUpdaterTestFixture, missing_when_doing21stAttempt_will_notDecreasePeakPercentage)
{
	// Score one goal and 20 misses
	statUpdater->processReset(2);
	statUpdater->processAttempt();
	statUpdater->processInitialBallHit();
	statUpdater->processGoal();
	statUpdater->updateData();

	for (auto i = 0; i < 20; i++)
	{
		statUpdater->processAttempt();
		statUpdater->processMiss();
		statUpdater->updateData();
	}

	// While the total success rate should be less than 5% (1/21), the peak should remain at 5%
	EXPECT_FLOAT_EQ(_shotStats->AllShotStats.Data.PeakSuccessPercentage, 5.0f);
}