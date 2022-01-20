#include <pch.h>
#include "SettingsDefinition.h"

const SettingsDefinition GoalPercentageCounterSettings::EnableFlagDef = {
	"goalpercentagecounter_enable",
	"Enable Plugin",
	"Toggle Goal Percentage Counter Plugin",
	.0f,
	1.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::DisplayAllShotStats = {
	"goalpercentagecounter_display_all_shot_stats",
	"Display All Shots Statistics",
	"Displays the overlay which shows statistics for every shot recorded in the training session",
	.0f,
	1.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::DisplayPerShotStats = {
	"goalpercentagecounter_display_per_shot_stats",
	"Display Per Shot Statistics",
	"Displays the overlay which shows statistics for the current shot in the training session",
	.0f,
	1.0f,
	".0f"
};


const SettingsDefinition GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef = {
	"goalpercentagecounter_display_attempts_and_goals",
	"Attempts/Goals",
	"Toggle display of attempts and goals in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayInitialBallHitsDef = {
	"goalpercentagecounter_track_initial_hits",
	"Display initial ball hits",
	"Allows checking consistency with mechanics where the goal is to touch the ball, e.g. speed flip training packs",
	.0f,
	1.0f,
	".0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayCurrentStreaksDef = {
	"goalpercentagecounter_display_total_success_rate",
	"Current Miss/Goal Streaks",
	"Toggle display of current miss and goal streaks in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayTotalSuccessRateDef = {
	"goalpercentagecounter_display_current_streaks",
	"Total Success Rate",
	"Toggle display of total success rate in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLongestStreaksDef = {
	"goalpercentagecounter_display_longest_streaks",
	"Longest Miss/Goal Streaks",
	"Toggle display of longest miss and goal streaks in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayPeakInfoDef = {
	"goalpercentagecounter_display_peak_info",
	"Peak Success Rate (Minimum of 20 Attempts)",
	"Toggle display of peak success rate and peak shot number in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLastNShotPercentageDef = {
	"goalpercentagecounter_display_last_n_shots",
	"Last N Shots",
	"Toggle display of the last n shots percentage in the stat display",
	.0f,
	1.0f,
	"1.0f"
};


const SettingsDefinition GoalPercentageCounterSettings::AllShotXPositionDef = {
	"goalpercentagecounter_all_shot_overlay_x_position",
	"All Shots Overlay X Position",
	"",
	.0f,
	3840.0f,
	"5.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::AllShotYPositionDef = {
	"goalpercentagecounter_all_shot_overlay_y_position",
	"All Shots Overlay Y Position",
	"",
	.0f,
	2160.0f,
	"205.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::AllShotFontSizeDef = {
	"goalpercentagecounter_all_shot_font_size_factor",
	"All Shots Text Size Factor",
	"",
	0.5f,
	2.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::PerShotXPositionDef = {
	"goalpercentagecounter_per_shot_overlay_x_position",
	"Per Shot Overlay X Position",
	"",
	.0f,
	3840.0f,
	"5.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::PerShotYPositionDef = {
	"goalpercentagecounter_per_shot_overlay_y_position",
	"Per Shot Overlay Y Position",
	"",
	.0f,
	2160.0f,
	"467.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::PerShotFontSizeDef = {
	"goalpercentagecounter_per_shot_font_height_factor",
	"Per Shot Text Size Factor",
	"",
	0.5f,
	2.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::PanelColorDef = {
	"goalpercentagecounter_panel_color",
	"Color of the background panel",
	"",
	std::optional<float>(),
	std::optional<float>(),
	"(0.4, 0.4, 0.4, 0.8)" // RGBA Color string (expected by CVarManager in this format)
};

const SettingsDefinition GoalPercentageCounterSettings::FontColorDef = {
	"goalpercentagecounter_text_color",
	"Color of the text",
	"",
	std::optional<float>(),
	std::optional<float>(),
	"(1.0, 1.0, 1.0, 1.0)" // RGBA Color string (expected by CVarManager in this format)
};