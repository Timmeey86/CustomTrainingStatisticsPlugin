#include <pch.h>
#include "SettingsRegistration.h"

// These macros just remove the syntactic overhead of extremely similar lambda function definitions
// If you are a lowlevel template expert and you know a better solution, feel free to propose a pull request ;-)
#define SET_BOOL_VALUE_FUNC(propertyName) [pluginState](bool value) { pluginState->propertyName = value; }
#define SET_INT_VALUE_FUNC(propertyName) [pluginState](int value) { pluginState->propertyName = value; }
#define SET_FLOAT_VALUE_FUNC(propertyName) [pluginState](float value) { pluginState->propertyName = value; }

void SettingsRegistration::registerCVars(std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState)
{
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::EnableFlagDef, SET_BOOL_VALUE_FUNC(PluginIsEnabled));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef, SET_BOOL_VALUE_FUNC(AttemptsAndGoalsShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayCurrentStreaksDef, SET_BOOL_VALUE_FUNC(CurrentStreaksShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayTotalSuccessRateDef, SET_BOOL_VALUE_FUNC(TotalSuccessRateShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayLongestStreaksDef, SET_BOOL_VALUE_FUNC(LongestStreaksShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayPeakInfoDef, SET_BOOL_VALUE_FUNC(PeakInfoShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayLastNShotPercentageDef, SET_BOOL_VALUE_FUNC(LastNShotPercentageShallBeDisplayed));

	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::XPositionDef, SET_INT_VALUE_FUNC(OverlayXPosition));
	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::YPositionDef, SET_INT_VALUE_FUNC(OverlayYPosition));
	registerFloatSliderSetting(cvarManager, GoalPercentageCounterSettings::FontWidthDef, SET_FLOAT_VALUE_FUNC(TextWidthFactor));
	registerFloatSliderSetting(cvarManager, GoalPercentageCounterSettings::FontHeightDef, SET_FLOAT_VALUE_FUNC(TextHeightFactor));
}

#undef SET_BOOL_VALUE_FUNC
#undef SET_INT_VALUE_FUNC
#undef SET_FLOAT_VALUE_FUNC

CVarWrapper SettingsRegistration::registerCVar(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition)
{
	return cvarManager->registerCvar(
		settingsDefinition.VariableName,
		std::to_string(settingsDefinition.DefaultValue),
		settingsDefinition.DisplayText,
		true,
		settingsDefinition.MinValue.has_value(),
		settingsDefinition.MinValue.value_or(.0f),
		settingsDefinition.MaxValue.has_value(),
		settingsDefinition.MaxValue.value_or(.0f)
	);
}
void SettingsRegistration::registerCheckboxSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(bool)> setValueFunc)
{
	registerCVar(cvarManager, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getBoolValue()); });
}

void SettingsRegistration::registerIntSliderSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(int)> setValueFunc)
{
	registerCVar(cvarManager, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getIntValue()); });
}

void SettingsRegistration::registerFloatSliderSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(float)> setValueFunc)
{
	registerCVar(cvarManager, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getFloatValue()); });
}
