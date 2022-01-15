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
	float DefaultValue;					///< The default value (should always be provided)
};

/** Defines settings which are available for this plugin. */
class GoalPercentageCounterSettings
{
public:
	static const SettingsDefinition EnableFlagDef;						///< Definitions for the global enable flag
	static const SettingsDefinition TrackInitialHitsInsteadOfGoalsDef;	///< Definitions for the flag which allows tracking the initial ball hit instead of the goal
	static const SettingsDefinition DisplayAllShotStats;				///< Definitions for the flag which displays the overlay for all shot stats
	static const SettingsDefinition DisplayPerShotStats;				///< Definitions for the flag which displays the overlay for per shot stats

	static const SettingsDefinition DisplayAttemptsAndGoalsDef;		///< Definitions for the flag which toggles display of attempts and goals
	static const SettingsDefinition DisplayCurrentStreaksDef;		///< Definitions for the flag which toggles display of the current miss/goal streaks
	static const SettingsDefinition DisplayTotalSuccessRateDef;		///< Definitions for the flag which toggles display of the total success percentage
	static const SettingsDefinition DisplayLongestStreaksDef;		///< Definitions for the flag which toggles display of the longest miss/goal streaks
	static const SettingsDefinition DisplayPeakInfoDef;				///< Definitions for the flag which toggles display of the peak success percentage and shot number
	static const SettingsDefinition DisplayLastNShotPercentageDef;	///< Definitions for the flag which toggles display of the last N shot percentage

	static const SettingsDefinition AllShotXPositionDef; ///< Definitions for the X position of the all shot overlay
	static const SettingsDefinition AllShotYPositionDef; ///< Definitions for the Y position of the all shot overlay
	static const SettingsDefinition AllShotFontWidthDef; ///< Definitions for the font width of the all shot overlay
	static const SettingsDefinition AllShotFontHeightDef; ///< Definitions for the font height of the all shot overlay

	static const SettingsDefinition PerShotXPositionDef; ///< Definitions for the X position of the per shot overlay
	static const SettingsDefinition PerShotYPositionDef; ///< Definitions for the Y position of the per shot overlay
	static const SettingsDefinition PerShotFontWidthDef; ///< Definitions for the font width of the per shot overlay
	static const SettingsDefinition PerShotFontHeightDef; ///< Definitions for the font height of the per shot overlay
};