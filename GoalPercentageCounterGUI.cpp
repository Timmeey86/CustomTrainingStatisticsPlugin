#include "pch.h"
#include "GoalPercentageCounter.h"

std::string GoalPercentageCounter::GetPluginName() {
	return "Goal Percentage Counter";
}

void GoalPercentageCounter::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> GoalPercentageCounter
void GoalPercentageCounter::RenderSettings() {
	ImGui::TextUnformatted("Goal Percentage Counter plugin settings");
	if (ImGui::Button("Reset Statistics"))
	{
		gameWrapper->Execute([this](GameWrapper* wrapper) {
			cvarManager->executeCommand("goalpercentagecounter_reset");
		});
	}

	CVarWrapper enableCvar = cvarManager->getCvar("goalpercentagecounter_enabled");
	if (!enableCvar) { return; }
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Goal Percentage Counter Plugin");
	}
}
