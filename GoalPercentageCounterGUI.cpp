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
}

/*
// Do ImGui rendering here
void GoalPercentageCounter::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string GoalPercentageCounter::GetMenuName()
{
	return "GoalPercentageCounter";
}

// Title to give the menu
std::string GoalPercentageCounter::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void GoalPercentageCounter::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool GoalPercentageCounter::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool GoalPercentageCounter::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void GoalPercentageCounter::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void GoalPercentageCounter::OnClose()
{
	isWindowOpen_ = false;
}
*/
