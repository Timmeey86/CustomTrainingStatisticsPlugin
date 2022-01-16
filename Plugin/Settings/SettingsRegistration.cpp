#include <pch.h>
#include "SettingsRegistration.h"
#include "../Data/TriggerNames.h"

// These macros just remove the syntactic overhead of extremely similar lambda function definitions
// If you are a lowlevel template expert and you know a better solution, feel free to propose a pull request ;-)
#define SET_BOOL_VALUE_FUNC(propertyName) [pluginState](bool value) { pluginState->propertyName = value; }
#define SET_INT_VALUE_FUNC(propertyName) [pluginState](int value) { pluginState->propertyName = value; }
#define SET_FLOAT_VALUE_FUNC(propertyName) [pluginState](float value) { pluginState->propertyName = value; }
#define SET_COLOR_VALUE_FUNC(propertyName) [pluginState](const LinearColor& value) { \
	pluginState->propertyName.R = value.R * 255.0f; \
	pluginState->propertyName.G = value.G * 255.0f; \
	pluginState->propertyName.B = value.B * 255.0f; \
	pluginState->propertyName.A = value.A * 255.0f; \
	}

void SettingsRegistration::registerCVars(std::function<void(const std::string&)> sendNotifierFunc, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState)
{
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::EnableFlagDef, SET_BOOL_VALUE_FUNC(PluginIsEnabled));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::TrackInitialHitsInsteadOfGoalsDef, [pluginState, sendNotifierFunc](bool value) {
		pluginState->TrackInitialBallHitInsteadOfGoal = value;
		// In addition to changing the state variable, we need to send a reset since mixing goal counts with ball hit counts doesn't make sense
		sendNotifierFunc(TriggerNames::ResetStatistics);
	});

	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef, SET_BOOL_VALUE_FUNC(AttemptsAndGoalsShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayCurrentStreaksDef, SET_BOOL_VALUE_FUNC(CurrentStreaksShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayTotalSuccessRateDef, SET_BOOL_VALUE_FUNC(TotalSuccessRateShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayLongestStreaksDef, SET_BOOL_VALUE_FUNC(LongestStreaksShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayPeakInfoDef, SET_BOOL_VALUE_FUNC(PeakInfoShallBeDisplayed));
	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayLastNShotPercentageDef, SET_BOOL_VALUE_FUNC(LastNShotPercentageShallBeDisplayed));

	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayAllShotStats, SET_BOOL_VALUE_FUNC(AllShotStatsShallBeDisplayed));
	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::AllShotXPositionDef, SET_INT_VALUE_FUNC(AllShotsOpts.OverlayXPosition));
	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::AllShotYPositionDef, SET_INT_VALUE_FUNC(AllShotsOpts.OverlayYPosition));
	registerFloatSliderSetting(cvarManager, GoalPercentageCounterSettings::AllShotFontSizeDef, [pluginState](float value) {
		pluginState->AllShotsOpts.TextSizeFactor = value;
		pluginState->AllShotsOpts.TextWidthFactor = 2.0f * value;
		pluginState->AllShotsOpts.TextHeightFactor = 1.5f * value;
		});

	registerCheckboxSetting(cvarManager, GoalPercentageCounterSettings::DisplayPerShotStats, SET_BOOL_VALUE_FUNC(PerShotStatsShallBeDisplayed));
	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::PerShotXPositionDef, SET_INT_VALUE_FUNC(PerShotOpts.OverlayXPosition));
	registerIntSliderSetting(cvarManager, GoalPercentageCounterSettings::PerShotYPositionDef, SET_INT_VALUE_FUNC(PerShotOpts.OverlayYPosition));
	registerFloatSliderSetting(cvarManager, GoalPercentageCounterSettings::PerShotFontSizeDef, [pluginState](float value) {
		pluginState->PerShotOpts.TextSizeFactor = value;
		pluginState->PerShotOpts.TextWidthFactor = 2.0f * value;
		pluginState->PerShotOpts.TextHeightFactor = 1.5f * value;
		});

	registerColorEditSetting(cvarManager, GoalPercentageCounterSettings::PanelColorDef, SET_COLOR_VALUE_FUNC(PanelColor));
	registerColorEditSetting(cvarManager, GoalPercentageCounterSettings::FontColorDef, SET_COLOR_VALUE_FUNC(FontColor));
}

#undef SET_BOOL_VALUE_FUNC
#undef SET_INT_VALUE_FUNC
#undef SET_FLOAT_VALUE_FUNC
#undef SET_COLOR_VALUE_FUNC

CVarWrapper SettingsRegistration::registerCVar(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition)
{
	return cvarManager->registerCvar(
		settingsDefinition.VariableName,
		settingsDefinition.DefaultValue,
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

void SettingsRegistration::registerColorEditSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(const LinearColor&)> setValueFunc)
{
	registerCVar(cvarManager, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getColorValue()); });
	
	// Apply the initial color from the settings
	auto cvar = cvarManager->getCvar(settingsDefinition.VariableName);
	if (cvar)
	{
		setValueFunc(cvar.getColorValue());
	}
}
