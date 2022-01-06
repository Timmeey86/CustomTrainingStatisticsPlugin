#pragma once

/**
* Stores information about the current state of the plugin.
*/
class PluginState
{
public:
	PluginState() = default;

	bool PreviousAttemptWasAGoal = false;				///< If this is still false when a new attempt is started, it means the previous shot was a miss.
	bool GoalReplayIsActive = false;					///< True while a goal is being replayed. We ignore other events during that time.
	bool PluginIsEnabled = true;						///< True while the user has not disabled the plugin in the settings UI.
	bool AttemptsAndGoalsShallBeDisplayed = true;		///< True while attemps and goals shall appear in the stat display.
	bool TotalSuccessRateShallBeDisplayed = true;		///< True while the total success rate shall appear in the stat display.
	bool CurrentStreaksShallBeDisplayed = true;			///< True while the current miss/goal streaks shall appear in the stat display.
	bool LongestStreaksShallBeDisplayed = true;			///< True while the longest miss/goal streaks shall appear in the stat display.
	bool PeakInfoShallBeDisplayed = true;				///< True while the peak percentage and shot number shall appear in the stat display.
	bool LastNShotPercentageShallBeDisplayed = true;	///< True while success percentage of the last N shots shall appear in the stat display.
};