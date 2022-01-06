#include <pch.h>
#include "SettingsDefinition.h"
#include "Data/ConfigurationOptions.h"

const SettingsDefinition GoalPercentageCounterSettings::EnableFlagDef = {
	ConfigurationOptions::EnablePlugin,
	"Enable Plugin",
	"Toggle Goal Percentage Counter Plugin",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef = {
	ConfigurationOptions::DisplayAttemptsAndGoals,
	"Attempts/Goals",
	"Toggle display of attempts and goals in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayCurrentStreaksDef = {
	ConfigurationOptions::DisplayCurrentStreaks,
	"Current Miss/Goal Streaks",
	"Toggle display of current miss and goal streaks in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayTotalSuccessRateDef = {
	ConfigurationOptions::DisplayTotalSuccessRate,
	"Total Success Rate",
	"Toggle display of total success rate in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLongestStreaksDef = {
	ConfigurationOptions::DisplayLongestStreaks,
	"Longest Miss/Goal Streaks",
	"Toggle display of longest miss and goal streaks in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayPeakInfoDef = {
	ConfigurationOptions::DisplayPeakInfo,
	"Peak Success Rate",
	"Toggle display of peak success rate and peak shot number in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLastNShotPercentageDef = {
	ConfigurationOptions::DisplayLastNShotPercentage,
	"Last N Shots",
	"Toggle display of the last n shots percentage in the stat display",
	.0f,
	1.0f,
	1.0f
};


const SettingsDefinition GoalPercentageCounterSettings::XPositionDef = {
	ConfigurationOptions::OverlayXPosition,
	"Overlay X Position",
	"",
	.0f,
	3840.0f,
	5.0f
};
const SettingsDefinition GoalPercentageCounterSettings::YPositionDef = {
	ConfigurationOptions::OverlayYPosition,
	"Overlay Y Position",
	"",
	.0f,
	2160.0f,
	195.0f
};
const SettingsDefinition GoalPercentageCounterSettings::FontWidthDef = {
	ConfigurationOptions::FontWidthFactor,
	"Font Width Factor",
	"",
	1.0f,
	4.0f,
	2.0f
};
const SettingsDefinition GoalPercentageCounterSettings::FontHeightDef = {
	ConfigurationOptions::FontHeightFactor,
	"Font Height Factor",
	"",
	1.0f,
	4.0f,
	1.5f
};