#pragma once

#include <bakkesmod/wrappers/wrapperstructs.h> // for LinearColor

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

	bool StatsShallBeRecorded = true;					///< True while statistics are supposed to be recorded.
	bool StatsShallBeDisplayed = true;					///< True while stats shall be displayed.
	bool RecordingIconShallBeDisplayed = true;			///< True while a recording icon shall be displayed (only when stats display is of).
	bool StatsShallBeComparedToAllTimePeak = true;		///< True if comparison shall be done vs all time peak stats rather than the previous session.

	bool IsMetric = true;								///< Whether or not the ball speed is in metric or imperial
	bool AllShotStatsShallBeDisplayed = true;			///< The overlay for all shot stats will appear while true
	bool PerShotStatsShallBeDisplayed = true;			///< The overlay for per shot stats will appear while true
	bool AttemptsAndGoalsShallBeDisplayed = true;		///< True while attempts and goals shall appear in the stat display.
	bool TotalSuccessRateShallBeDisplayed = true;		///< True while the total success rate shall appear in the stat display.
	bool CurrentStreaksShallBeDisplayed = true;			///< True while the current miss/goal streaks shall appear in the stat display.
	bool LongestStreaksShallBeDisplayed = true;			///< True while the longest miss/goal streaks shall appear in the stat display.
	bool PeakInfoShallBeDisplayed = true;				///< True while the peak percentage and shot number shall appear in the stat display.
	bool LastNShotPercentageShallBeDisplayed = true;	///< True while success percentage of the last N shots shall appear in the stat display.
	bool MostRecentGoalSpeedShallBeDisplayed = true;	///< True while the most recent goal speed shall appear in the stat display.
	bool MaxGoalSpeedShallBeDisplayed = true;			///< True while the maximum goal speed shall appear in the stat display.
	bool MinGoalSpeedShallBeDisplayed = true;			///< True while the minimum recent goal speed shall appear in the stat display.
	bool MedianGoalSpeedShallBeDisplayed = true;		///< True while the median goal speed shall appear in the stat display.
	bool MeanGoalSpeedShallBeDisplayed = false;			///< True while the mean goal speed shall appear in the stat display.
	bool StdDevGoalSpeedShallBeDisplayed = false;		///< True while the standard deviation goal speed shall appear in the stat display.
	bool AirDribbleTouchesShallBeDisplayed = false;		///< True while max. number of air dribbles shall appear in the stat display.
	bool AirDribbleTimeShallBeDisplayed = false;		///< True while max. air dribble time shall appear in the stat display.
	bool GroundDribbleTimeShallBeDisplayed = false;		///< True while max. ground dribble time shall appear in the stat display.
	bool DoubleTapGoalsShallBeDisplayed = false;		///< True while double tap goals shall appear in the stat display.
	bool DoubleTapPercentageShallBeDisplayed = false;	///< True while double tap goal percentage shall appear in the stat display.
	bool MaxFlipResetsShallBeDisplayed = false;			///< True while maximum number of flip resets shall appear in the stat display.
	bool TotalFlipResetsShallBeDisplayed = false;		///< True while the total amount of flip resets shall appear in the stat display.
	bool FlipResetsPerAttemptShallBeDisplayed = false;	///< True while average flip resets per attempt shall appear in the stat display.
	bool FlipResetPercentageShallBeDisplayed = false;	///< True while flip reset percentage shall appear in the stat display.
	bool CloseMissesShallBeDisplayed = false;			///< True while close misses shall appear in the stat display.
	bool CloseMissPercentageShallBeDisplayed = false;	///< True while the percentage of close misses shall appear in the stat display.
	bool PreviousSessionDiffShallBeDisplayed = true;	///< True while the percentage differences to the previous session shall appear.

	/** True while initial ball hits and ball hit percentage shall appear in the stat display. 
	 */
	bool InitialBallHitsShallBeDisplayed = true;
	int CurrentRoundIndex = -1;								///< The index of the current round, -1 when not initialized
	int TotalRounds = -1;									///< The total number of rounds in the current training pack
	int MenuStackSize = 0;									///< The total number of open menus (1 for the "Pause" Menu in custom training, 2 for "Settings" or "Change Mode/Match")

	DisplayOptions AllShotsOpts{"All Shots Statistics", 5, 205, .8f, 1.6f, 1.2f }; ///< Stores the user-defined value for the overlay and text for all shots stats
	DisplayOptions PerShotOpts{"Per Shot Statistics", 5, 520, .8f, 1.6f, 1.2f};  ///< Stores the user-defined value for the overlay and text for per round stats

	LinearColor PanelColor = LinearColor{ 41.0f, 41.0f, 41.0f, 204.0f };
	LinearColor FontColor = LinearColor{ 255.0f, 255.0f, 255.0f, 255.0f };

	std::string TrainingPackCode = {}; ///< The code for the current training pack.
	std::string TrainingPackCreator = {}; ///< The creator of the current training pack.
	std::string TrainingPackName = {}; ///< The name of the current training pack.

	// Using setter and getter for ball speed because of conversion
	static constexpr float UE_UNITS_TO_KPH{ 0.036f };	///< Ball speed is originally in a different unit specific to the game engine
	float getBallSpeed() const
	{
		return _ballSpeed * UE_UNITS_TO_KPH;
	}

	inline void setBallSpeed(float ballSpeed)
	{
		_ballSpeed = ballSpeed;
	}
private:
	float _ballSpeed; ///< The current speed of the ball in the game engine units
};
