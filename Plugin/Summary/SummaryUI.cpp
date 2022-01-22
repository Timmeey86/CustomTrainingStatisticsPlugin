#include "pch.h"
#include "SummaryUI.h"
#include "IMGUI/imgui.h"
#include "Display/StatDisplay.h"

#include <string>

void SummaryUI::initSummaryUi(
	const std::shared_ptr<CVarManagerWrapper> cvarManager,
	const std::shared_ptr<const ShotStats> shotStats,
	const std::shared_ptr<const PluginState> pluginState)
{
	_cvarManager = cvarManager;
	_shotStats = shotStats;
	_pluginState = pluginState;
}

void SummaryUI::Render()
{
	if (!_isWindowOpen)
	{
		ToggleMenu();
		return;
	}

	ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(FLT_MAX, FLT_MAX));
	ImGuiWindowFlags windowFlags = 0 | ImGuiWindowFlags_MenuBar; //| ImGuiWindowFlags_NoCollapse;

	if (!ImGui::Begin(GetMenuTitle().c_str(), &_isWindowOpen, windowFlags))
	{
		// Early out if the window is collapsed, as an optimization.
		_shouldBlockInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::BeginChild("#GoalPercentageCounterSummaryStatsHeader", ImVec2(0, ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y * 2 + ImGui::GetStyle().FramePadding.y), false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysUseWindowPadding);

	auto allShotStatNamesAndValues = StatDisplay::getStatsToBeRendered(_shotStats->AllShotStats, _pluginState);
	int numColumns = allShotStatNamesAndValues.size() + 1; // +1 for shot number

	ImGui::Columns(numColumns, "goal_percentage_counter_summary_stats_header");
	ImGui::Separator();

	ImGui::Text("Shot Number"); ImGui::NextColumn();
	for (const auto& nv : allShotStatNamesAndValues)
	{
		ImGui::Text(nv.first.c_str());
		ImGui::NextColumn();
	}

	ImGui::Separator();
	ImGui::EndChild();

	ImGui::BeginChild("#GoalPercentageCounterSummaryStatsBody", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysUseWindowPadding);
	ImGui::Columns(numColumns, "goal_percentage_counter_summary_stats_body");
	ImGui::Separator();

	ImGuiListClipper clipper(_shotStats->PerShotStats.size()+1); // +1 for AllShotStats
	while (clipper.Step())
	{
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
		{
			StatsData statsData;
			if (i < _shotStats->PerShotStats.size())
			{
				ImGui::Text(std::to_string(i + 1).c_str()); ImGui::NextColumn(); // Shot number

				statsData = _shotStats->PerShotStats.at(i);
			}
			else
			{
				ImGui::Text("All Shots"); ImGui::NextColumn();
				statsData = _shotStats->AllShotStats;
			}

			auto statNamesAndValues = StatDisplay::getStatsToBeRendered(statsData, _pluginState);
			for (const auto& nv : statNamesAndValues)
			{
				ImGui::Text(nv.second.c_str());
				ImGui::NextColumn();
			}
			
			ImGui::Separator();
		}
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
	_shouldBlockInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
	ImGui::End();
}

std::string SummaryUI::GetMenuName()
{
	return "goal_percentage_counter_summary";
}

std::string SummaryUI::GetMenuTitle()
{
	return "Goal Percentage Counter Summary";
}

void SummaryUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
	ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
}

bool SummaryUI::ShouldBlockInput()
{
	return _shouldBlockInput;
}

bool SummaryUI::IsActiveOverlay()
{
	return true;
}

void SummaryUI::OnOpen()
{
	_isWindowOpen = true;

}

void SummaryUI::OnClose()
{
	_isWindowOpen = false;
}

void SummaryUI::ToggleMenu()
{
	_cvarManager->executeCommand("togglemenu " + GetMenuName());
}

void SummaryUI::OpenMenu()
{
	if (!_isWindowOpen)
	{
		ToggleMenu();
	}
}

void SummaryUI::CloseMenu()
{
	if (_isWindowOpen)
	{
		ToggleMenu();
	}
}
