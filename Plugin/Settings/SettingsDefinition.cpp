#include <pch.h>
#include "SettingsDefinition.h"
#include "Data/ConfigurationOptions.h"

const SettingsDefinition EnableFlagDef = {
	ConfigurationOptions::EnablePlugin,
	"Enable Plugin",
	"Toggle Goal Percentage Counter Plugin",
	"0",
	"1",
	"1"
};
const SettingsDefinition DisplayAttemptsAndGoalsDef = {
	ConfigurationOptions::DisplayAttemptsAndGoals,
	"Attempts/Goals",
	"Toggle display of attempts and goals in the stat display",
	"0",
	"1",
	"1"
};
const SettingsDefinition DisplayCurrentStreaksDef = {
	ConfigurationOptions::DisplayCurrentStreaks,
	"Current Miss/Goal Streaks",
	"Toggle display of current miss and goal streaks in the stat display",
	"0",
	"1",
	"1"
};
const SettingsDefinition DisplayTotalSuccessRateDef = {
	ConfigurationOptions::DisplayTotalSuccessRate,
	"Total Success Rate",
	"Toggle display of total success rate in the stat display",
	"0",
	"1",
	"1"
};
const SettingsDefinition DisplayLongestStreaksDef = {
	ConfigurationOptions::DisplayLongestStreaks,
	"Longest Miss/Goal Streaks",
	"Toggle display of longest miss and goal streaks in the stat display",
	"0",
	"1",
	"1"
};
const SettingsDefinition DisplayPeakInfoDef = {
	ConfigurationOptions::DisplayPeakInfo,
	"Peak Success Rate",
	"Toggle display of peak success rate and peak shot number in the stat display",
	"0",
	"1",
	"1"
};
const SettingsDefinition DisplayLastNShotPercentageDef = {
	ConfigurationOptions::DisplayLastNShotPercentage,
	"Last N Shots",
	"Toggle display of the last n shots percentage in the stat display",
	"0",
	"1",
	"1"
};


const SettingsDefinition XPositionDef = {
	ConfigurationOptions::OverlayXPosition,
	"Overlay X Position",
	"",
	"0",
	"3840",
	"5"
};
const SettingsDefinition YPositionDef = {
	ConfigurationOptions::OverlayYPosition,
	"Overlay Y Position",
	"",
	"0",
	"2160",
	"195"
};
const SettingsDefinition FontWidthDef = {
	ConfigurationOptions::FontWidthFactor,
	"Font Width Factor",
	"",
	"1.0",
	"4.0",
	"2.0"
};
const SettingsDefinition FontHeightDef = {
	ConfigurationOptions::FontHeightFactor,
	"Font Height Factor",
	"",
	"1.0",
	"4.0",
	"2.0"
};