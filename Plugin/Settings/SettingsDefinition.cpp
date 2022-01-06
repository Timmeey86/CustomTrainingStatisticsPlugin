#include <pch.h>
#include "SettingsDefinition.h"

const SettingsDefinition GoalPercentageCounterSettings::EnableFlagDef = {
	"goalpercentagecounter_enable",
	"Enable Plugin",
	"Toggle Goal Percentage Counter Plugin",
	.0f,
	1.0f,
	1.0f
};

const SettingsDefinition GoalPercentageCounterSettings::TrackInitialHitsInsteadOfGoalsDef = {
	"goalpercentagecounter_track_initial_hits",
	"Count Initial Ball Hits instead of Goals",
	"Allows checking consistency with mechanics which end by touching the ball, e.g. speed flip training packs",
	.0f,
	1.0f,
	.0f
};


const SettingsDefinition GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef = {
	"goalpercentagecounter_display_attempts_and_goals",
	"Attempts/Goals",
	"Toggle display of attempts and goals in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayCurrentStreaksDef = {
	"goalpercentagecounter_display_total_success_rate",
	"Current Miss/Goal Streaks",
	"Toggle display of current miss and goal streaks in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayTotalSuccessRateDef = {
	"goalpercentagecounter_display_current_streaks",
	"Total Success Rate",
	"Toggle display of total success rate in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLongestStreaksDef = {
	"goalpercentagecounter_display_longest_streaks",
	"Longest Miss/Goal Streaks",
	"Toggle display of longest miss and goal streaks in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayPeakInfoDef = {
	"goalpercentagecounter_display_peak_info",
	"Peak Success Rate",
	"Toggle display of peak success rate and peak shot number in the stat display",
	.0f,
	1.0f,
	1.0f
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLastNShotPercentageDef = {
	"goalpercentagecounter_display_last_n_shots",
	"Last N Shots",
	"Toggle display of the last n shots percentage in the stat display",
	.0f,
	1.0f,
	1.0f
};


const SettingsDefinition GoalPercentageCounterSettings::XPositionDef = {
	"goalpercentagecounter_overlay_x_position",
	"Overlay X Position",
	"",
	.0f,
	3840.0f,
	5.0f
};
const SettingsDefinition GoalPercentageCounterSettings::YPositionDef = {
	"goalpercentagecounter_overlay_y_position",
	"Overlay Y Position",
	"",
	.0f,
	2160.0f,
	195.0f
};
const SettingsDefinition GoalPercentageCounterSettings::FontWidthDef = {
	"goalpercentagecounter_font_height_factor",
	"Font Width Factor",
	"",
	1.0f,
	4.0f,
	2.0f
};
const SettingsDefinition GoalPercentageCounterSettings::FontHeightDef = {
	"goalpercentagecounter_font_width_factor",
	"Font Height Factor",
	"",
	1.0f,
	4.0f,
	1.5f
};