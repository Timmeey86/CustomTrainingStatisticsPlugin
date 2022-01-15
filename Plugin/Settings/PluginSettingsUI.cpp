#include <pch.h>
#include "PluginSettingsUI.h"
#include "../Data/TriggerNames.h"

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "../IMGUI/imgui.h"

void PluginSettingsUI::initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	_sendNotifierFunc = sendNotifierFunc;
	_cvarManager = cvarManager;
}

std::string PluginSettingsUI::GetPluginName()
{
	return "Goal Percentage Counter";
}

void PluginSettingsUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void PluginSettingsUI::createCheckbox(const SettingsDefinition& settingsDefinition)
{
	CVarWrapper cvar = _cvarManager->getCvar(settingsDefinition.VariableName);
	if (!cvar) { return; }

	if (bool enabled = cvar.getBoolValue();
		ImGui::Checkbox(settingsDefinition.DisplayText.c_str(), &enabled))
	{
		cvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered() && !settingsDefinition.ToolTipText.empty())
	{
		ImGui::SetTooltip(settingsDefinition.ToolTipText.c_str());
	}
}

void PluginSettingsUI::createIntSlider(const SettingsDefinition& settingsDefinition)
{
	auto cvar = _cvarManager->getCvar(settingsDefinition.VariableName);
	if (!cvar) { return; }

	auto currentValue = cvar.getIntValue();
	auto minValue = (int)std::round(settingsDefinition.MinValue.value());
	auto maxValue = (int)std::round(settingsDefinition.MaxValue.value());
	if (ImGui::SliderInt(settingsDefinition.DisplayText.c_str(), &currentValue, minValue, maxValue))
	{
		cvar.setValue(currentValue);
	}
}

void PluginSettingsUI::createFloatSlider(const SettingsDefinition& settingsDefinition)
{
	auto cvar = _cvarManager->getCvar(settingsDefinition.VariableName);
	if (!cvar) { return; }

	auto currentValue = cvar.getFloatValue();
	auto minValue = settingsDefinition.MinValue.value();
	auto maxValue = settingsDefinition.MaxValue.value();
	if (ImGui::SliderFloat(settingsDefinition.DisplayText.c_str(), &currentValue, minValue, maxValue))
	{
		cvar.setValue(currentValue);
	}
}
// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> GoalPercentageCounter
void PluginSettingsUI::RenderSettings()
{
	ImGui::TextUnformatted("Goal Percentage Counter plugin settings");

	if (ImGui::CollapsingHeader("General"))
	{
		createCheckbox(GoalPercentageCounterSettings::EnableFlagDef);
		createCheckbox(GoalPercentageCounterSettings::TrackInitialHitsInsteadOfGoalsDef);

		// Add a button for resetting statistics
		if (ImGui::Button("Reset Statistics"))
		{
			_sendNotifierFunc(TriggerNames::ResetStatistics);
		}
	}

	if (ImGui::CollapsingHeader("Display"))
	{
		// Add an option for enabling or disabling the Attempts/Goals display
		createCheckbox(GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayCurrentStreaksDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayTotalSuccessRateDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayLongestStreaksDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayPeakInfoDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayLastNShotPercentageDef);

		// Add options for the overlay panels
		createCheckbox(GoalPercentageCounterSettings::DisplayAllShotStats);
		createIntSlider(GoalPercentageCounterSettings::AllShotXPositionDef);
		createIntSlider(GoalPercentageCounterSettings::AllShotYPositionDef);
		createFloatSlider(GoalPercentageCounterSettings::AllShotFontWidthDef);
		createFloatSlider(GoalPercentageCounterSettings::AllShotFontHeightDef);
		
		createCheckbox(GoalPercentageCounterSettings::DisplayPerShotStats);
		createIntSlider(GoalPercentageCounterSettings::PerShotXPositionDef);
		createIntSlider(GoalPercentageCounterSettings::PerShotYPositionDef);
		createFloatSlider(GoalPercentageCounterSettings::PerShotFontWidthDef);
		createFloatSlider(GoalPercentageCounterSettings::PerShotFontHeightDef);
	}
}
