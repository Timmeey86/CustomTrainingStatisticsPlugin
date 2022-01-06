#include <pch.h>
#include "SettingsRegistration.h"

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

	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::XPositionDef, [](int) {}); // TODO Add Plugin State variable
	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::YPositionDef, [](int) {}); // TODO Add Plugin State variable
	registerFloatSliderSetting(cvarManager, GoalPercentageCounterSettings::FontWidthDef, [](float) {}); // TODO Add Plugin State variable
	registerFloatSliderSetting(cvarManager, GoalPercentageCounterSettings::FontHeightDef, [](float) {}); // TODO Add Plugin State variable
}

#undef SET_BOOL_VALUE_FUNC

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
