#pragma once

#include <string>

/** Defines parameters related to a user configurable setting. */
class SettingsDefinition
{
public:
	std::string VariableName;	///< The name of the variable to be used.
	std::string DisplayText;	///< The text to be displayed both in config.cfg and the settings UI
	std::string ToolTipText;	///< The tool tip text (if any - use this especially for checkboxes)
	std::string MinValue;		///< The minimum value (empty string if there is none)
	std::string MaxValue;		///< The maximum value (empty string if there is none)
	std::string DefaultValue;	///< The default value (should always be provided)
};

/** Defines settings which are available for this plugin. */
class GoalPercentageCounterSettings
{
public:
	static const SettingsDefinition EnableFlagDef;					///< Definitions for the global enable flag
	static const SettingsDefinition DisplayAttemptsAndGoalsDef;		///< Definitions for the flag which toggles display of attempts and goals
	static const SettingsDefinition DisplayCurrentStreaksDef;		///< Definitions for the flag which toggles display of the current miss/goal streaks
	static const SettingsDefinition DisplayTotalSuccessRateDef;		///< Definitions for the flag which toggles display of the total success percentage
	static const SettingsDefinition DisplayLongestStreaksDef;		///< Definitions for the flag which toggles display of the longest miss/goal streaks
	static const SettingsDefinition DisplayPeakInfoDef;				///< Definitions for the flag which toggles display of the peak success percentage and shot number
	static const SettingsDefinition DisplayLastNShotPercentageDef;	///< Definitions for the flag which toggles display of the last N shot percentage

	static const SettingsDefinition XPositionDef; ///< Definitions for the X position of the overlay
	static const SettingsDefinition YPositionDef; ///< Definitions for the Y position of the overlay
	static const SettingsDefinition FontWidthDef; ///< Definitions for the font width
	static const SettingsDefinition FontHeightDef; ///< Definitions for the font height
};