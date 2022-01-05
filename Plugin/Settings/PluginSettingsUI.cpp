#include <pch.h>
#include "PluginSettingsUI.h"

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "../IMGUI/imgui.h"

void PluginSettingsUI::initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::function<CVarWrapper(const std::string&)> getVariableWrapperFunc)
{
	_sendNotifierFunc = sendNotifierFunc;
	_getVariableWrapperFunc = getVariableWrapperFunc;
}

std::string PluginSettingsUI::GetPluginName() 
{
	return "Goal Percentage Counter";
}

void PluginSettingsUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> GoalPercentageCounter
void PluginSettingsUI::RenderSettings() 
{
	ImGui::TextUnformatted("Goal Percentage Counter plugin settings");
	if (ImGui::Button("Reset Statistics"))
	{
		_sendNotifierFunc("goalpercentagecounter_reset");
	}

	CVarWrapper enableCvar =  _getVariableWrapperFunc("goalpercentagecounter_enabled");
	if (!enableCvar) { return; }
	
	if(bool enabled = enableCvar.getBoolValue(); ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Goal Percentage Counter Plugin");
	}
}
