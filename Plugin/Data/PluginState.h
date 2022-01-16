#pragma once

class DisplayOptions
{
public:
	std::string Title;
	int OverlayXPosition;		///< Stores the user-defined value for the X position of the overlay
	int OverlayYPosition;		///< Stores the user-defined value for the Y position of the overlay
	float TextSizeFactor;	///< Stores the user-defined value for the text size factor of the overlay
	float TextWidthFactor;	///< Stores the calculated value for the width factor of the overlay
	float TextHeightFactor;	///< Stores the calculated value for the height factor of the overlay
};

/**
* Stores information about the current state of the plugin.
*/
class PluginState
{
public:
	PluginState() = default;

	bool PreviousAttemptWasAGoal = false;					///< If this is still false when a new attempt is started, it means the previous shot was a miss.
	bool GoalReplayIsActive = false;						///< True while a goal is being replayed. We ignore other events during that time.
	bool PluginIsEnabled = true;							///< True while the user has not disabled the plugin in the settings UI.
	bool AllShotStatsShallBeDisplayed = true;				///< The overlay for all shot stats will be displayed while true
	bool PerShotStatsShallBeDisplayed = false;				///< The overlay for per shot stats will be displayed while true
	bool AttemptsAndGoalsShallBeDisplayed = true;			///< True while attempts and goals shall appear in the stat display.
	bool TotalSuccessRateShallBeDisplayed = true;			///< True while the total success rate shall appear in the stat display.
	bool CurrentStreaksShallBeDisplayed = true;				///< True while the current miss/goal streaks shall appear in the stat display.
	bool LongestStreaksShallBeDisplayed = true;				///< True while the longest miss/goal streaks shall appear in the stat display.
	bool PeakInfoShallBeDisplayed = true;					///< True while the peak percentage and shot number shall appear in the stat display.
	bool LastNShotPercentageShallBeDisplayed = true;		///< True while success percentage of the last N shots shall appear in the stat display.
	bool TrackInitialBallHitInsteadOfGoal = false;			///< True if the initial ball hits shall be tracked, rather than goals.
	bool BallWasHitAtLeastOnceWithinCurrentAttempt = false;	///< True if the ball was hit at least once during the current attempt.
	int CurrentRoundIndex = -1;								///< The index of the current round, -1 when not initialized
	int TotalRounds = -1;									///< The total number of rounds in the current training back

	DisplayOptions AllShotsOpts{"All Shots Statistics", 5, 205, 1.0f, 2.0f, 1.5f }; ///< Stores the user-defined value for the overlay and text for all shots stats
	DisplayOptions PerShotOpts{"Per Shot Statistics", 5, 467, 1.0f, 2.0f, 1.5f};  ///< Stores the user-defined value for the overlay and text for per round stats
};
