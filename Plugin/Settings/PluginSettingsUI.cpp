#include <pch.h>
#include "PluginSettingsUI.h"
#include "../Data/ConfigurationOptions.h"

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "../IMGUI/imgui.h"

void PluginSettingsUI::initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState)
{
	_sendNotifierFunc = sendNotifierFunc;
	_cvarManager = cvarManager;
	_pluginState = pluginState;
}

std::string PluginSettingsUI::GetPluginName() 
{
	return "Goal Percentage Counter";
}

void PluginSettingsUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void PluginSettingsUI::createCheckbox(const std::string& variableName, const std::string& displayText, const std::string& tooltipText, std::function<void(bool)> setValueFunc)
{
	CVarWrapper cvar = _cvarManager->getCvar(variableName);
	if (!cvar) { return; }

	if (bool enabled = cvar.getBoolValue();
		ImGui::Checkbox(displayText.c_str(), &enabled))
	{
		cvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) 
	{
		ImGui::SetTooltip(tooltipText.c_str());
	}
}

void PluginSettingsUI::createSlider(const std::string& variableName, const std::string& displayText, int minValue, int maxValue, int defaultValue)
{
}

#define SET_VALUE_FUNC(propertyName) [this](bool value) { _pluginState->propertyName = value; }

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> GoalPercentageCounter
void PluginSettingsUI::RenderSettings() 
{	
	if (ImGui::CollapsingHeader("General"))
	{
		ImGui::TextUnformatted("Goal Percentage Counter plugin settings");

		createCheckbox(ConfigurationOptions::EnablePlugin, "Enable Plugin", "Toggle Goal Percentage Counter Plugin", SET_VALUE_FUNC(PluginIsEnabled));

		// Add a button for resetting statistics
		if (ImGui::Button("Reset Statistics"))
		{
			_sendNotifierFunc(ConfigurationOptions::ResetStatistics);
		}
	}

	if (ImGui::CollapsingHeader("Display"))
	{
		// Add an option for enabling or disabling the Attempts/Goals display
		createCheckbox(
			ConfigurationOptions::DisplayAttemptsAndGoals, 
			"Attempts/Goals",
			"Toggle display of attempts and goals in the stat display", 
			SET_VALUE_FUNC(AttemptsAndGoalsShallBeDisplayed));

		createCheckbox(
			ConfigurationOptions::DisplayCurrentStreaks,
			"Current Miss/Goal Streaks",
			"Toggle display of current miss and goal streaks in the stat display",
			SET_VALUE_FUNC(CurrentStreaksShallBeDisplayed));

		createCheckbox(
			ConfigurationOptions::DisplayTotalSuccessRate,
			"Total Success Rate",
			"Toggle display of total success rate in the stat display",
			SET_VALUE_FUNC(TotalSuccessRateShallBeDisplayed));

		createCheckbox(
			ConfigurationOptions::DisplayLongestStreaks,
			"Longest Miss/Goal Streaks",
			"Toggle display of longest miss and goal streaks in the stat display",
			SET_VALUE_FUNC(LongestStreaksShallBeDisplayed));

		createCheckbox(
			ConfigurationOptions::DisplayPeakInfo,
			"Peak Success Rate",
			"Toggle display of peak success rate and peak shot number in the stat display",
			SET_VALUE_FUNC(PeakInfoShallBeDisplayed));

		createCheckbox(
			ConfigurationOptions::DisplayLastNShotPercentage,
			"Last N Shots",
			"Toggle display of the last n shots percentage in the stat display",
			SET_VALUE_FUNC(LastNShotPercentageShallBeDisplayed));


	}
}
