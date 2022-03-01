#include <pch.h>
#include "SettingsRegistration.h"
#include "../Data/TriggerNames.h"
#include "Summary/SummaryUI.h"

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

bool indexIsValid(const std::string& indexString, int& outVar)
{
	try
	{
		outVar = std::stoi(indexString);
		return true;
	}
	catch (const std::invalid_argument&)
	{
		return false;
	}
}

void handleBindingChange(std::shared_ptr<CVarManagerWrapper> cvarManager, const std::string& oldValue, CVarWrapper cvar, const std::string& bindCommand)
{
	// Check if oldIdx is valid (it should always be. If it hasn't been set, it is "0" (index of 'None') rather than an empty string or anything
	int oldIdx;
	if (!indexIsValid(oldValue, oldIdx))
	{
		return;
	}

	int newIdx = cvar.getIntValue();

	// Check bounds of indices
	if (0 <= oldIdx && oldIdx < GoalPercentageCounterSettings::KeybindingsArraySize &&
		0 <= newIdx && newIdx < GoalPercentageCounterSettings::KeybindingsArraySize)
	{
		auto oldBind = std::string(GoalPercentageCounterSettings::KeybindingsArray[oldIdx]);
		auto newBind = std::string(GoalPercentageCounterSettings::KeybindingsArray[newIdx]);

		// Unbind old value unless it is "None"
		if (!oldBind.empty() && oldBind != GoalPercentageCounterSettings::KeybindingsArray[0])
		{
			cvarManager->executeCommand("unbind " + oldBind, false);
		}

		// Bind new value unless it is "None"
		if (newBind != GoalPercentageCounterSettings::KeybindingsArray[0])
		{
			cvarManager->setBind(newBind, bindCommand);
		}
	}
}

void SettingsRegistration::registerCVars(
	std::function<void(const std::string&)> sendNotifierFunc, 
	std::shared_ptr<CVarManagerWrapper> cvarManager, 
	std::shared_ptr<PersistentStorage> persistentStorage,
	std::shared_ptr<PluginState> pluginState)
{
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::StatsShallBeDisplayedDef, SET_BOOL_VALUE_FUNC(StatsShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::StatsShallBeRecordedDef, SET_BOOL_VALUE_FUNC(StatsShallBeRecorded));

	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef, SET_BOOL_VALUE_FUNC(AttemptsAndGoalsShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayInitialBallHitsDef, SET_BOOL_VALUE_FUNC(InitialBallHitsShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayCurrentStreaksDef, SET_BOOL_VALUE_FUNC(CurrentStreaksShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayTotalSuccessRateDef, SET_BOOL_VALUE_FUNC(TotalSuccessRateShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayLongestStreaksDef, SET_BOOL_VALUE_FUNC(LongestStreaksShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayPeakInfoDef, SET_BOOL_VALUE_FUNC(PeakInfoShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayLastNShotPercentageDef, SET_BOOL_VALUE_FUNC(LastNShotPercentageShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayMostRecentGoalSpeedDef, SET_BOOL_VALUE_FUNC(MostRecentGoalSpeedShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayMaxGoalSpeedDef, SET_BOOL_VALUE_FUNC(MaxGoalSpeedShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayMinGoalSpeedDef, SET_BOOL_VALUE_FUNC(MinGoalSpeedShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayMedianGoalSpeedDef, SET_BOOL_VALUE_FUNC(MedianGoalSpeedShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayMeanGoalSpeedDef, SET_BOOL_VALUE_FUNC(MeanGoalSpeedShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayAirDribbleTouchesDef, SET_BOOL_VALUE_FUNC(AirDribbleTouchesShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayAirDribbleTimeDef, SET_BOOL_VALUE_FUNC(AirDribbleTimeShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayGroundDribbleDef, SET_BOOL_VALUE_FUNC(GroundDribbleTimeShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayDoubleTapGoalsDef, SET_BOOL_VALUE_FUNC(DoubleTapGoalsShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayDoubleTapPercentageDef, SET_BOOL_VALUE_FUNC(DoubleTapPercentageShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayTotalFlipResetsDef, SET_BOOL_VALUE_FUNC(TotalFlipResetsShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayMaxFlipResetsDef, SET_BOOL_VALUE_FUNC(MaxFlipResetsShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayFlipResetsPerAttemptDef, SET_BOOL_VALUE_FUNC(FlipResetsPerAttemptShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayFlipResetPercentageDef, SET_BOOL_VALUE_FUNC(FlipResetPercentageShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayCloseMissesDef, SET_BOOL_VALUE_FUNC(CloseMissesShallBeDisplayed));
	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayCloseMissPercentageDef, SET_BOOL_VALUE_FUNC(CloseMissPercentageShallBeDisplayed));

	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayAllShotStats, SET_BOOL_VALUE_FUNC(AllShotStatsShallBeDisplayed));
	registerIntSliderSetting(persistentStorage, GoalPercentageCounterSettings::AllShotXPositionDef, SET_INT_VALUE_FUNC(AllShotsOpts.OverlayXPosition));
	registerIntSliderSetting(persistentStorage, GoalPercentageCounterSettings::AllShotYPositionDef, SET_INT_VALUE_FUNC(AllShotsOpts.OverlayYPosition));
	registerFloatSliderSetting(persistentStorage, GoalPercentageCounterSettings::AllShotFontSizeDef, [pluginState](float value) {
		pluginState->AllShotsOpts.TextSizeFactor = value;
		pluginState->AllShotsOpts.TextWidthFactor = 2.0f * value;
		pluginState->AllShotsOpts.TextHeightFactor = 1.5f * value;
		});

	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayPerShotStats, SET_BOOL_VALUE_FUNC(PerShotStatsShallBeDisplayed));
	registerIntSliderSetting(persistentStorage, GoalPercentageCounterSettings::PerShotXPositionDef, SET_INT_VALUE_FUNC(PerShotOpts.OverlayXPosition));
	registerIntSliderSetting(persistentStorage, GoalPercentageCounterSettings::PerShotYPositionDef, SET_INT_VALUE_FUNC(PerShotOpts.OverlayYPosition));
	registerFloatSliderSetting(persistentStorage, GoalPercentageCounterSettings::PerShotFontSizeDef, [pluginState](float value) {
		pluginState->PerShotOpts.TextSizeFactor = value;
		pluginState->PerShotOpts.TextWidthFactor = 2.0f * value;
		pluginState->PerShotOpts.TextHeightFactor = 1.5f * value;
		});

	registerColorEditSetting(persistentStorage, GoalPercentageCounterSettings::PanelColorDef, SET_COLOR_VALUE_FUNC(PanelColor));
	registerColorEditSetting(persistentStorage, GoalPercentageCounterSettings::FontColorDef, SET_COLOR_VALUE_FUNC(FontColor));

	registerCheckboxSetting(persistentStorage, GoalPercentageCounterSettings::DisplayStatDifference, SET_BOOL_VALUE_FUNC(PreviousSessionDiffShallBeDisplayed));

	registerDropdownMenuSetting(persistentStorage, GoalPercentageCounterSettings::SummaryKeybindingDef, [persistentStorage, cvarManager](const std::string& oldValue, CVarWrapper cvar) {
		handleBindingChange(cvarManager, oldValue, cvar, "togglemenu " + SummaryUI::MenuName + ";");
		});

	registerDropdownMenuSetting(persistentStorage, GoalPercentageCounterSettings::RestoreLastSessionKeybindingDef, [persistentStorage, cvarManager](const std::string& oldValue, CVarWrapper cvar) {
		handleBindingChange(cvarManager, oldValue, cvar, std::string{ TriggerNames::RestoreStatistics } + ";");
	});

	registerDropdownMenuSetting(persistentStorage, GoalPercentageCounterSettings::ToggleLastAttemptKeybindingDef, [persistentStorage, cvarManager](const std::string& oldValue, CVarWrapper cvar) {
		handleBindingChange(cvarManager, oldValue, cvar, std::string{ TriggerNames::ToggleLastAttempt } + ";");
	});

	registerDropdownMenuSetting(persistentStorage, GoalPercentageCounterSettings::ToggleHeatmapKeybindingDef, [persistentStorage, cvarManager](const std::string& oldValue, CVarWrapper cvar) {
		handleBindingChange(cvarManager, oldValue, cvar, std::string{ TriggerNames::ToggleHeatmapDisplay } + ";");
	});

	registerDropdownMenuSetting(persistentStorage, GoalPercentageCounterSettings::ToggleShotLocationKeybindingDef, [persistentStorage, cvarManager](const std::string& oldValue, CVarWrapper cvar) {
		handleBindingChange(cvarManager, oldValue, cvar, std::string{ TriggerNames::ToggleImpactLocationDisplay } + ";");
	});
}

#undef SET_BOOL_VALUE_FUNC
#undef SET_INT_VALUE_FUNC
#undef SET_FLOAT_VALUE_FUNC
#undef SET_COLOR_VALUE_FUNC

CVarWrapper SettingsRegistration::registerCVar(std::shared_ptr<PersistentStorage> persistentStorage, const SettingsDefinition& settingsDefinition)
{
	return persistentStorage->RegisterPersistentCvar(
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
void SettingsRegistration::registerCheckboxSetting(std::shared_ptr<PersistentStorage> persistentStorage, const SettingsDefinition& settingsDefinition, std::function<void(bool)> setValueFunc)
{
	registerCVar(persistentStorage, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getBoolValue()); });
}

void SettingsRegistration::registerIntSliderSetting(std::shared_ptr<PersistentStorage> persistentStorage, const SettingsDefinition& settingsDefinition, std::function<void(int)> setValueFunc)
{
	registerCVar(persistentStorage, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getIntValue()); });
}

void SettingsRegistration::registerFloatSliderSetting(std::shared_ptr<PersistentStorage> persistentStorage, const SettingsDefinition& settingsDefinition, std::function<void(float)> setValueFunc)
{
	registerCVar(persistentStorage, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getFloatValue()); });
}

void SettingsRegistration::registerColorEditSetting(std::shared_ptr<PersistentStorage> persistentStorage, const SettingsDefinition& settingsDefinition, std::function<void(const LinearColor&)> setValueFunc)
{
	registerCVar(persistentStorage, settingsDefinition).addOnValueChanged([setValueFunc](const std::string&, CVarWrapper cvar) { setValueFunc(cvar.getColorValue()); });
}

void SettingsRegistration::registerDropdownMenuSetting(std::shared_ptr<PersistentStorage> persistentStorage, const SettingsDefinition& settingsDefinition, std::function<void(const std::string&, CVarWrapper)> handleDropdown)
{
	registerCVar(persistentStorage, settingsDefinition).addOnValueChanged([handleDropdown](const std::string& oldValue, CVarWrapper cvar) { handleDropdown(oldValue, cvar); });
}
