#include <pch.h>
#include "PluginSettingsUI.h"
#include "../Data/TriggerNames.h"

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include <IMGUI/imgui.h>

void PluginSettingsUI::initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	_sendNotifierFunc = sendNotifierFunc;
	_cvarManager = cvarManager;
}

std::string PluginSettingsUI::GetPluginName()
{
	return "Custom Training Stats";
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

void PluginSettingsUI::createColorEdit(const SettingsDefinition& settingsDefinition)
{
	auto cvar = _cvarManager->getCvar(settingsDefinition.VariableName);
	if (!cvar) { return; }

	auto currentValue = cvar.getColorValue();
	if (ImGui::ColorEdit4(settingsDefinition.DisplayText.c_str(), &currentValue.R))
	{
		cvar.setValue(currentValue);
	}
}

void PluginSettingsUI::createDropdownMenu(const SettingsDefinition& settingsDefinition, const char* items[], int numItems)
{
	auto cvar = _cvarManager->getCvar(settingsDefinition.VariableName);
	if (!cvar) { return; }

	int currentIndex = cvar.getIntValue();
	if (ImGui::Combo(settingsDefinition.DisplayText.c_str(), &currentIndex, items, numItems))
	{
		cvar.setValue(currentIndex);
	}
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> GoalPercentageCounter
void PluginSettingsUI::RenderSettings()
{
	ImGui::TextUnformatted("Plugin settings");

	if (ImGui::CollapsingHeader("General"))
	{
		createCheckbox(GoalPercentageCounterSettings::StatsShallBeDisplayedDef);
		createCheckbox(GoalPercentageCounterSettings::StatsShallBeRecordedDef);

		ImGui::Separator();

		ImGui::Text("Bindings (this creates bindings in the Bindings tab)");
		const auto keybindingAmount = 127;
		createDropdownMenu(GoalPercentageCounterSettings::SummaryKeybindingDef, GoalPercentageCounterSettings::KeybindingsArray, keybindingAmount);
		createDropdownMenu(GoalPercentageCounterSettings::RestoreLastSessionKeybindingDef, GoalPercentageCounterSettings::KeybindingsArray, keybindingAmount);
		createDropdownMenu(GoalPercentageCounterSettings::ToggleLastAttemptKeybindingDef, GoalPercentageCounterSettings::KeybindingsArray, keybindingAmount);
		createDropdownMenu(GoalPercentageCounterSettings::ToggleHeatmapKeybindingDef, GoalPercentageCounterSettings::KeybindingsArray, keybindingAmount);
		createDropdownMenu(GoalPercentageCounterSettings::ToggleShotLocationKeybindingDef, GoalPercentageCounterSettings::KeybindingsArray, keybindingAmount);

		ImGui::Separator();

		// Add a button for resetting statistics
		if (ImGui::Button("Reset Statistics"))
		{
			_sendNotifierFunc(TriggerNames::ResetStatistics);
		}
	}

	if (ImGui::CollapsingHeader("Stats"))
	{
		// Add an option for enabling or disabling the Attempts/Goals display
		createCheckbox(GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayInitialBallHitsDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayCurrentStreaksDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayTotalSuccessRateDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayLongestStreaksDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayPeakInfoDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayLastNShotPercentageDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayMostRecentGoalSpeedDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayMaxGoalSpeedDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayMinGoalSpeedDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayMedianGoalSpeedDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayMeanGoalSpeedDef);

	}
	if (ImGui::CollapsingHeader("Appearance"))
	{
		// Add coloring options (Both panels will use the same colors)
		createColorEdit(GoalPercentageCounterSettings::PanelColorDef);
		createColorEdit(GoalPercentageCounterSettings::FontColorDef);

		ImGui::Separator();

		// Add options for the overlay panels
		createCheckbox(GoalPercentageCounterSettings::DisplayAllShotStats);
		createIntSlider(GoalPercentageCounterSettings::AllShotXPositionDef);
		createIntSlider(GoalPercentageCounterSettings::AllShotYPositionDef);
		createFloatSlider(GoalPercentageCounterSettings::AllShotFontSizeDef);

		ImGui::Separator();

		createCheckbox(GoalPercentageCounterSettings::DisplayPerShotStats);
		createIntSlider(GoalPercentageCounterSettings::PerShotXPositionDef);
		createIntSlider(GoalPercentageCounterSettings::PerShotYPositionDef);
		createFloatSlider(GoalPercentageCounterSettings::PerShotFontSizeDef);

		ImGui::Separator();

		createCheckbox(GoalPercentageCounterSettings::DisplayStatDifference);
	}
	if (ImGui::CollapsingHeader("Advanced Stats"))
	{
		ImGui::Text("Air Dribble Stats");
		ImGui::Text("An air dribble will be considered such once the car is in the air and has touched the ball at least twice.");
		ImGui::Text("Landing with all four wheels on either the ground, a wall or the ceilling will finish the air dribble.");
		ImGui::Text("The ball may bounce off the ground, a wall or the ceiling once and the air dribble may start/continue from there.");
		ImGui::Text("If the ball touches the ground twice without being hit by the car, you need to land and take off again for a new attempt.");
		ImGui::Text("You may attempt several dribbles during one shot, if time allows.");

		createCheckbox(GoalPercentageCounterSettings::DisplayAirDribbleTouchesDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayAirDribbleTimeDef);

		ImGui::Separator();

		ImGui::Text("Ground Dribble Stats");
		ImGui::Text("A ground dribble will be considered such once the car has touched the ball at least twice, and the ball hasn't touched the ground since.");
		ImGui::Text("The ball is allowed to touch the walls and ceiling indefinitely.");
		ImGui::Text("Letting the ball touch the ground will finish the ground dribble.");
		ImGui::Text("Note that the car's position will not influence the dribble time, so transitioning from a ground to an air dribble will count as both a ground and an air dribble.");
		ImGui::Text("You may attempt several dribbles during one shot, if time allows.");

		createCheckbox(GoalPercentageCounterSettings::DisplayGroundDribbleDef);

		ImGui::Separator();

		ImGui::Text("Double Tap Goals");
		ImGui::Text("A double tap will be considered such if the player touches the ball while in the air, bounces the ball off the ground, a wall or the ceiling, ");
		ImGui::Text("and then scores a goal while still in the air.");
		ImGui::Text("If the player's car touches the ground after the double tap, the ball must fly into the goal directly, otherwise it won't count.");
		ImGui::Text("Landing anywhere before getting the double tap will fail the attempt.");
		ImGui::Text("You may attempt several double taps during one shot, if time allows.");
		ImGui::Text("Note that the double tap goal percentage is shown in relation to the number of goals rather than the number of attempts made.");

		createCheckbox(GoalPercentageCounterSettings::DisplayDoubleTapGoalsDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayDoubleTapPercentageDef);

		ImGui::Separator();

		ImGui::Text("Flip Resets");
		ImGui::Text("Flip Resets are counted during one uninterrupted aerial. Landing on the ground, a wall or the ceiling will finish the attempt.");
		ImGui::Text("You may attempt several flip reset chains during one shot, if time allows.");
		ImGui::Text("Note that the flip reset goal percentage is shown in relation to the number of goals rather than the number of attempts made.");

		createCheckbox(GoalPercentageCounterSettings::DisplayTotalFlipResetsDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayMaxFlipResetsDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayFlipResetsPerAttemptDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayFlipResetPercentageDef);

		ImGui::Separator();

		ImGui::Text("Close Misses");
		ImGui::Text("Sometimes you just want to see how often you almost scored a goal.");

		createCheckbox(GoalPercentageCounterSettings::DisplayCloseMissesDef);
		createCheckbox(GoalPercentageCounterSettings::DisplayCloseMissPercentageDef);

		ImGui::Separator();

	}
}
