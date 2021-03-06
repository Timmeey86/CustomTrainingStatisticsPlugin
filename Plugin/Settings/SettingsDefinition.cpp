#include <pch.h>
#include "SettingsDefinition.h"

const SettingsDefinition GoalPercentageCounterSettings::StatsShallBeDisplayedDef = {
	"customtrainingstatistics_enable_stat_display",
	"Display Statistics Overlay",
	"Toggle Custom Training Statistics Display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::StatsShallBeRecordedDef = {
	"customtrainingstatistics_enable_stat_recording",
	"Record Statistics",
	"Stop or start recording statistics. Can also be used to temporarily pause recording",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::RecordingIconShallBeDisplayedDef = {
	"customtrainingstatistics_enable_recording_icon",
	"Display recording icon when tracking stats in background",
	"Displays a \"REC\" icon if \"Display Statistics Overlay\" is turned off, and \"Record Statistics\" is turned on",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::StatsShallBeComparedToAllTimePeakDef = {
	"customtrainingstatistics_compare_to_all_time_peak",
	"Switch between comparing vs 'all time peak stats' or 'previous session'",
	"If this is set, stats will be compared to the all time peak statistics. If it isn't, the previous session will be used instead",
	.0f,
	1.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::DisplayStatDifference = {
	"customtrainingstatistics_display_stat_difference",
	"Display difference to previous session",
	"Display the difference between the current and the previous session",
	.0f,
	1.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::DisplayAllShotStats = {
	"customtrainingstatistics_display_all_shot_stats",
	"Display All Shots Statistics",
	"Displays the overlay which shows statistics for every shot recorded in the training session",
	.0f,
	1.0f,
	"1.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::DisplayPerShotStats = {
	"customtrainingstatistics_display_per_shot_stats",
	"Display Per Shot Statistics",
	"Displays the overlay which shows statistics for the current shot in the training session",
	.0f,
	1.0f,
	"1.0f"
};


const SettingsDefinition GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef = {
	"customtrainingstatistics_display_attempts_and_goals",
	"Attempts/Goals",
	"Toggle display of attempts and goals in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayInitialBallHitsDef = {
	"customtrainingstatistics_track_initial_hits",
	"Display initial ball hits",
	"Allows checking consistency with mechanics where the goal is to touch the ball, e.g. speed flip training packs",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayCurrentStreaksDef = {
	"customtrainingstatistics_display_current_streaks",
	"Current Miss/Goal Streaks",
	"Toggle display of current miss and goal streaks in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayTotalSuccessRateDef = {
	"customtrainingstatistics_display_total_success_rate",
	"Total Success Rate",
	"Toggle display of total success rate in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLongestStreaksDef = {
	"customtrainingstatistics_display_longest_streaks",
	"Longest Miss/Goal Streaks",
	"Toggle display of longest miss and goal streaks in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayPeakInfoDef = {
	"customtrainingstatistics_display_peak_info",
	"Peak Success Rate (Minimum of 20 Attempts)",
	"Toggle display of peak success rate and peak shot number in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayLastNShotPercentageDef = {
	"customtrainingstatistics_display_last_n_shots",
	"Last N Shots",
	"Toggle display of the last n shots percentage in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayMostRecentGoalSpeedDef = {
	"customtrainingstatistics_display_most_recent_goal_speed",
	"Most Recent Goal Speed",
	"Toggle display of the most recent goal speed in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayMaxGoalSpeedDef = {
	"customtrainingstatistics_display_max_goal_speed",
	"Maximum Goal Speed",
	"Toggle display of the maximum goal speed in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayMinGoalSpeedDef = {
	"customtrainingstatistics_display_min_goal_speed",
	"Minimum Goal Speed",
	"Toggle display of the minimum goal speed in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayMedianGoalSpeedDef = {
	"customtrainingstatistics_display_median_goal_speed",
	"Median Goal Speed",
	"Toggle display of the median goal speed in the stat display",
	.0f,
	1.0f,
	"1.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayMeanGoalSpeedDef = {
	"customtrainingstatistics_display_mean_goal_speed",
	"Mean Goal Speed",
	"Toggle display of the mean goal speed in the stat display",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayStdDevGoalSpeedDef = {
	"customtrainingstatistics_display_std_dev_goal_speed",
	"Standard Deviation Goal Speed",
	"Toggle display of the standard deviation goal speed in the stat display",
	.0f,
	1.0f,
	"0.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::DisplayAirDribbleTouchesDef = {
	"customtrainingstatistics_display_air_dribble_touches",
	"Max. Air Dribble Touches",
	"Toggle display of the maximum number of air dribble touches in the stat display",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayAirDribbleTimeDef = {
	"customtrainingstatistics_display_air_dribble_time",
	"Max. Air Dribble Time",
	"Toggle display of the maximum air dribble time. The time between the first and last ball touch is counted.",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayGroundDribbleDef = {
	"customtrainingstatistics_display_ground_dribble_time",
	"Max. Ground Dribble Time",
	"Toggle display of the maximum ground dribble time. The time between the first and last ball touch is counted. Switching to an air dribble does not interrupt the ground dribble.",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayDoubleTapGoalsDef = {
	"customtrainingstatistics_display_double_tap_goals",
	"Double tap Goals",
	"Toggle display of the number of double tap goals.",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayDoubleTapPercentageDef = {
	"customtrainingstatistics_display_double_tap_goal_percentage",
	"Double tap Goal Percentage",
	"Toggle display of double tap goal percentage. The double tap goal percentage will be in relation to the number of goals rather than the number of attempts.",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayMaxFlipResetsDef = {
	"customtrainingstatistics_display_max_flip_resets",
	"Maximum number of flip resets",
	"Toggle display of maximum flip resets achieved during any attempt (whether scored or not).",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayTotalFlipResetsDef = {
	"customtrainingstatistics_display_total_flip_resets",
	"Total number of flip resets",
	"Toggle display of maximum flip resets achieved during any attempt (whether scored or not).",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayFlipResetsPerAttemptDef = {
	"customtrainingstatistics_display_flip_resets_per_attempt",
	"Average number of flip resets per attempt",
	"Toggle display of the average amount of flip resets per attempt (whether scored or not).",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayFlipResetPercentageDef = {
	"customtrainingstatistics_display_flip_reset_goal_percentage",
	"Percentage of flip reset goals",
	"Toggle display of percentage of attempts which included a flip reset and resulted in a goal.",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayCloseMissesDef = {
	"customtrainingstatistics_display_close_misses",
	"Close Misses",
	"Toggle display of the number of times you almost scored a goal.",
	.0f,
	1.0f,
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::DisplayCloseMissPercentageDef = {
	"customtrainingstatistics_display_close_miss_percentage",
	"Close Miss percentage",
	"Toggle display of the percentage of attempts which almost resulted in a goal.",
	.0f,
	1.0f,
	"0.0f"
};


const SettingsDefinition GoalPercentageCounterSettings::AllShotXPositionDef = {
	"customtrainingstatistics_all_shot_overlay_x_position",
	"All Shots Overlay X Position",
	"",
	.0f,
	3840.0f,
	"5.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::AllShotYPositionDef = {
	"customtrainingstatistics_all_shot_overlay_y_position",
	"All Shots Overlay Y Position",
	"",
	.0f,
	2160.0f,
	"205.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::AllShotFontSizeDef = {
	"customtrainingstatistics_all_shot_font_size_factor",
	"All Shots Text Size Factor",
	"",
	0.5f,
	2.0f,
	"0.8f"
};

const SettingsDefinition GoalPercentageCounterSettings::PerShotXPositionDef = {
	"customtrainingstatistics_per_shot_overlay_x_position",
	"Per Shot Overlay X Position",
	"",
	.0f,
	3840.0f,
	"5.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::PerShotYPositionDef = {
	"customtrainingstatistics_per_shot_overlay_y_position",
	"Per Shot Overlay Y Position",
	"",
	.0f,
	2160.0f,
	"525.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::PerShotFontSizeDef = {
	"customtrainingstatistics_per_shot_font_height_factor",
	"Per Shot Text Size Factor",
	"",
	0.5f,
	2.0f,
	"0.8f"
};

const SettingsDefinition GoalPercentageCounterSettings::PanelColorDef = {
	"customtrainingstatistics_panel_color",
	"Color of the background panel",
	"",
	std::optional<float>(),
	std::optional<float>(),
	"(0.15, 0.15, 0.15, 0.8)" // RGBA Color string (expected by CVarManager in this format)
};

const SettingsDefinition GoalPercentageCounterSettings::FontColorDef = {
	"customtrainingstatistics_text_color",
	"Color of the text",
	"",
	std::optional<float>(),
	std::optional<float>(),
	"(1.0, 1.0, 1.0, 1.0)" // RGBA Color string (expected by CVarManager in this format)
};

const SettingsDefinition GoalPercentageCounterSettings::SummaryKeybindingDef = {
	"customtrainingstatistics_summary_keybinding",
	"Open/Close Statistics Summary",
	"Keybinding to toggle the summary window",
	0.0f,
	static_cast<float>(GoalPercentageCounterSettings::KeybindingsArraySize - 1),
	"0.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::RestoreLastSessionKeybindingDef = {
	"customtrainingstatistics_restorelastsession_keybinding",
	"Restore Previous Session",
	"Keybinding to restore statistics from the previous session of the same pack. You need to press this key/button before starting your first attempt.",
	0.0f,
	static_cast<float>(GoalPercentageCounterSettings::KeybindingsArraySize - 1),
	"0.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::ToggleLastAttemptKeybindingDef = {
	"customtrainingstatistics_togglelastattempt_keybinding",
	"Toggle Last Goal/Miss",
	"Keybinding to toggle the last attempt. This is useful for complex mechanics or defender packs.",
	0.0f,
	static_cast<float>(GoalPercentageCounterSettings::KeybindingsArraySize - 1),
	"0.0f"
};

const SettingsDefinition GoalPercentageCounterSettings::ToggleHeatmapKeybindingDef = {
	"customtrainingstatistics_toggleheatmap_keybinding",
	"Toggle Goal/Backboard Heatmap Display",
	"Keybinding to toggle display of a heatmap of Goals and Backboard bounces. Might affect performance when visible.",
	0.0f,
	static_cast<float>(GoalPercentageCounterSettings::KeybindingsArraySize - 1),
	"0.0f"
};
const SettingsDefinition GoalPercentageCounterSettings::ToggleShotLocationKeybindingDef = {
	"customtrainingstatistics_toggleshotlocation_keybinding",
	"Toggle Goal/Backboard Impact Display",
	"Keybinding to toggle display of a the impact locations of Goals and Backboard bounces. Might affect performance when visible.",
	0.0f,
	static_cast<float>(GoalPercentageCounterSettings::KeybindingsArraySize - 1),
	"0.0f"
};

const char* GoalPercentageCounterSettings::KeybindingsArray[] = { "None","F1","F3","F4","F5","F7","F8","F9","F10","F11","F12","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","Escape","Tab","Tilde","ScrollLock","Pause","one","two","three","four","five","six","seven","eight","nine","zero","Underscore","Equals","Backslash","LeftBracket","RightBracket","Enter","CapsLock","Semicolon","Quote","LeftShift","Comma","Period","Slash","RightShift","LeftControl","LeftAlt","SpaceBar","RightAlt","RightControl","Left","Up","Down","Right","Home","End","Insert","PageUp","Delete","PageDown","NumLock","Divide","Multiply","Subtract","Add","NumPadOne","NumPadTwo","NumPadThree","NumPadFour","NumPadFive","NumPadSix","NumPadSeven","NumPadEight","NumPadNine","NumPadZero","Decimal","LeftMouseButton","RightMouseButton","ThumbMouseButton","ThumbMouseButton2","MouseScrollUp","MouseScrollDown","MouseX","MouseY","XboxTypeS_LeftThumbStick","XboxTypeS_RightThumbStick","XboxTypeS_DPad_Up","XboxTypeS_DPad_Left","XboxTypeS_DPad_Right","XboxTypeS_DPad_Down","XboxTypeS_Back","XboxTypeS_Start","XboxTypeS_Y","XboxTypeS_X","XboxTypeS_B","XboxTypeS_A","XboxTypeS_LeftShoulder","XboxTypeS_RightShoulder","XboxTypeS_LeftTrigger","XboxTypeS_RightTrigger","XboxTypeS_LeftTriggerAxis","XboxTypeS_RightTriggerAxis","XboxTypeS_LeftX","XboxTypeS_LeftY","XboxTypeS_RightX","XboxTypeS_RightY" };


// This is initialized later on in order to make sure every setting has been initialized
std::vector<std::string> GoalPercentageCounterSettings::OrderedStatsNames;
std::mutex GoalPercentageCounterSettings::OrderedStatsMutex;
const std::string GoalPercentageCounterSettings::OrderedStatsCVarName = "customtrainingstatistics_stats_order";


std::string vector_to_string(const std::vector<std::string>& values)
{
	std::ostringstream stream;
	auto separator = '|';
	stream << values.size() << separator;
	for (auto value : values)
	{
		stream << value << separator;
	}
	return stream.str();
}
std::vector<std::string> string_to_vector(const std::string& vectorAsString)
{
	if (vectorAsString.empty()) { return {}; }

	const char separator = '|';

	// read until the first separator
	auto separatorPos = vectorAsString.find(separator);
	if (separatorPos == std::string::npos) { return {}; }

	// Try to read the size of the vector	
	std::vector<std::string> result;
	if (auto size = std::stoi(vectorAsString.substr(0, separatorPos)); size > 0)
	{
		size_t offset = separatorPos + 1;
		for (auto index = 0; index < size; index++)
		{
			auto valueSeparatorPos = vectorAsString.find(separator, offset);
			if (valueSeparatorPos == std::string::npos) { return {}; }

			result.push_back(vectorAsString.substr(offset, valueSeparatorPos - offset));
			offset = valueSeparatorPos + 1;
		}
	}
	return result;
}