#pragma once

#include <string>
#include <optional>

/** Defines parameters related to a user configurable setting. */
class SettingsDefinition
{
public:		
	std::string VariableName;			///< The name of the variable to be used.
	std::string DisplayText;			///< The text to be displayed both in config.cfg and the settings UI
	std::string ToolTipText;			///< The tool tip text (if any - use this especially for checkboxes)
	std::optional<float> MinValue;		///< The minimum value 
	std::optional<float> MaxValue;		///< The maximum value 
	std::string DefaultValue;			///< The default value (should always be provided)
};

/** Defines settings which are available for this plugin. */
class GoalPercentageCounterSettings
{
public:
	static const SettingsDefinition StatsShallBeRecordedDef;	///< Definitions for the flag which turns stat recording on or off.
	static const SettingsDefinition StatsShallBeDisplayedDef;	///< Definitions for the flag which turns stat display on or off.
	static const SettingsDefinition DisplayStatDifference;		///< Definitions for the flag which displays the overlay for all shot stats
	static const SettingsDefinition DisplayAllShotStats;		///< Definitions for the flag which displays the overlay for all shot stats
	static const SettingsDefinition DisplayPerShotStats;		///< Definitions for the flag which displays the overlay for per shot stats

	static const SettingsDefinition DisplayAttemptsAndGoalsDef;		///< Definitions for the flag which toggles display of attempts and goals
	static const SettingsDefinition DisplayInitialBallHitsDef;		///< Definitions for the flag which toggles idsplay of initial ball hits
	static const SettingsDefinition DisplayCurrentStreaksDef;		///< Definitions for the flag which toggles display of the current miss/goal streaks
	static const SettingsDefinition DisplayTotalSuccessRateDef;		///< Definitions for the flag which toggles display of the total success percentage
	static const SettingsDefinition DisplayLongestStreaksDef;		///< Definitions for the flag which toggles display of the longest miss/goal streaks
	static const SettingsDefinition DisplayPeakInfoDef;				///< Definitions for the flag which toggles display of the peak success percentage and shot number
	static const SettingsDefinition DisplayLastNShotPercentageDef;	///< Definitions for the flag which toggles display of the last N shot percentage
	static const SettingsDefinition DisplayMostRecentGoalSpeedDef;	///< Definitions for the flag which toggles display of the most recent goal speed
	static const SettingsDefinition DisplayMaxGoalSpeedDef;			///< Definitions for the flag which toggles display of the maximum goal speed
	static const SettingsDefinition DisplayMinGoalSpeedDef;			///< Definitions for the flag which toggles display of the minimum goal speed
	static const SettingsDefinition DisplayMedianGoalSpeedDef;		///< Definitions for the flag which toggles display of the median goal speed
	static const SettingsDefinition DisplayMeanGoalSpeedDef;		///< Definitions for the flag which toggles display of the mean goal speed
	static const SettingsDefinition DisplayAirDribbleTouchesDef;	///< Definitions for the flag which toggles display of air dribble touches
	static const SettingsDefinition DisplayAirDribbleTimeDef;		///< Definitions for the flag which toggles display of air dribble time
	static const SettingsDefinition DisplayGroundDribbleDef;		///< Definitions for the flag which toggles display of ground dribble time
	static const SettingsDefinition DisplayDoubleTapGoalsDef;		///< Definitions for the flag which toggles display of double tap goals
	static const SettingsDefinition DisplayDoubleTapPercentageDef;	///< Definitions for the flag which toggles display of double tap goal percentage
	static const SettingsDefinition DisplayMaxFlipResetsDef;		///< Definitions for the flag which toggles display of maximum flip resets in one attempt.
	static const SettingsDefinition DisplayTotalFlipResetsDef;		///< Definitions for the flag which toggles display of total flip resets in one attempt.
	static const SettingsDefinition DisplayFlipResetsPerAttemptDef;	///< Definitions for the flag which toggles display of average flip resets per attempt.
	static const SettingsDefinition DisplayFlipResetPercentageDef;	///< Definitions for the flag which toggles display of flip reset goal percentage.
	static const SettingsDefinition DisplayCloseMissesDef;			///< Definitions for the flag which toggles display of close misses.
	static const SettingsDefinition DisplayCloseMissPercentageDef;	///< Definitions for the flag which toggles display of close miss percentage.

	static const SettingsDefinition AllShotXPositionDef;	///< Definitions for the X position of the all shot overlay
	static const SettingsDefinition AllShotYPositionDef;	///< Definitions for the Y position of the all shot overlay
	static const SettingsDefinition AllShotFontSizeDef;		///< Definitions for the font size of the all shot overlay

	static const SettingsDefinition PerShotXPositionDef;	///< Definitions for the X position of the per shot overlay
	static const SettingsDefinition PerShotYPositionDef;	///< Definitions for the Y position of the per shot overlay
	static const SettingsDefinition PerShotFontSizeDef;		///< Definitions for the font size of the per shot overlay

	static const SettingsDefinition PanelColorDef;			///< Definitions for the background color of the panel
	static const SettingsDefinition FontColorDef;			///< Definitions for the text color of the panel


	static const SettingsDefinition SummaryKeybindingDef;				///< Definitions for the summary window keybinding
	static const SettingsDefinition RestoreLastSessionKeybindingDef;	///< Definitions for the "Restore last session" keybinding
	static const SettingsDefinition ToggleLastAttemptKeybindingDef;		///< Definitions for the "Toggle last attempt" keybinding
	static const SettingsDefinition ToggleHeatmapKeybindingDef;			///< Definitions for the "Toggle Heatmap Display" keybinding
	static const SettingsDefinition ToggleShotLocationKeybindingDef;	///< Definitions for the "Toggle Shot Location" keybinding
	static constexpr int KeybindingsArraySize{ 127 };					///< Size of array of possible keybindings
	static const char* KeybindingsArray[];								///< List of possible keybindings
};