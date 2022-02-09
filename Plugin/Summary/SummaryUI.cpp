#include "pch.h"
#include "SummaryUI.h"
#include "IMGUI/imgui.h"
#include "Display/StatDisplay.h"

#include <string>

const std::string SummaryUI::MenuName = "custom_training_statistics_summary";

void SummaryUI::initSummaryUi(
	const std::shared_ptr<CVarManagerWrapper> cvarManager,
	const std::shared_ptr<const ShotStats> shotStats,
	const std::shared_ptr<const ShotStats> diffData,
	const std::shared_ptr<const PluginState> pluginState)
{
	_cvarManager = cvarManager;
	_shotStats = shotStats;
	_diffData = diffData;
	_pluginState = pluginState;
}

void SummaryUI::renderSummary()
{
	ImGui::BeginChild(
		"#CustomTrainingStatisticsSummaryStats",
		ImVec2(0, 0),
		//ImVec2(0, ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y * 2 + ImGui::GetStyle().FramePadding.y),
		false,
		ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysUseWindowPadding);

	auto statsToBeRendered = StatDisplay::GetStatsToBeRendered(_shotStats->AllShotStats, _pluginState);
	int numColumns = (int)statsToBeRendered.size() + 1; // +1 for shot number
	ImGui::Columns(numColumns, "custom_training_statistics_summary_stats_header");
	ImGui::Separator();

	ImGui::Text("Shot Number:");
	ImGui::NextColumn();
	for (const auto& statStrings : statsToBeRendered)
	{
		ImGui::Text(statStrings.Label.c_str());
		ImGui::NextColumn();
	}

	ImGui::Separator();
	ImGui::Separator();

	ImGuiListClipper clipper((int)_shotStats->PerShotStats.size() + 1); // +1 for AllShotStats
	while (clipper.Step())
	{
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
		{
			StatsData statsData;
			const StatsData* statsDataDiff = nullptr;
			// Get row data
			if (i < _shotStats->PerShotStats.size())
			{
				ImGui::Text(std::to_string(i + 1).c_str()); // Shot number
				ImGui::NextColumn();

				statsData = _shotStats->PerShotStats.at(i);
				if (_diffData && _pluginState->PreviousSessionDiffShallBeDisplayed && _diffData->hasAttempts())
				{
					statsDataDiff = &_diffData->PerShotStats.at(i);
				}
			}
			else
			{
				ImGui::Text("All Shots");
				ImGui::NextColumn();
				statsData = _shotStats->AllShotStats;
				if (_diffData && _pluginState->PreviousSessionDiffShallBeDisplayed && _diffData->hasAttempts())
				{
					statsDataDiff = &_diffData->AllShotStats;
				}
			}

			auto statsToBeRendered = StatDisplay::GetStatsToBeRendered(statsData, _pluginState, statsDataDiff);
			for (auto&& statStrings : statsToBeRendered)
			{
				// ImGui displays text as a c string so percent signs are escaped
				// Need to add an extra percent sign so it will display
				if (statStrings.Unit.back() == '%')
				{
					statStrings.Unit.push_back('%');
				}

				ImGui::Text((statStrings.Value + " " + statStrings.Unit).c_str());

				if (statStrings.DiffValue.has_value())
				{
					auto textColor = (std::stod(statStrings.DiffValue.value()) >= .0 ? ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f } : ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
					ImGui::SameLine();
					ImGui::TextColored(textColor, statStrings.DiffValue.value().c_str());
				}
				ImGui::NextColumn();
			}

			ImGui::Separator();
		}
	}

	ImGui::EndChild();
}

void SummaryUI::Render()
{
	if (!_isWindowOpen)
	{
		ToggleMenu();
		return;
	}

	// Init GUI
	ImGui::SetNextWindowSizeConstraints(ImVec2(560, 320), ImVec2(FLT_MAX, FLT_MAX));
	ImGuiWindowFlags windowFlags = 0 | ImGuiWindowFlags_MenuBar;
	if (!ImGui::Begin(GetMenuTitle().c_str(), &_isWindowOpen, windowFlags))
	{
		// Early out if the window is collapsed, as an optimization.
		_shouldBlockInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
		ImGui::End();
		return;
	}

	// Render data
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	renderSummary();
	ImGui::PopStyleVar();

	// End GUI
	_shouldBlockInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
	ImGui::End();
}

std::string SummaryUI::GetMenuName()
{
	return MenuName;
}

std::string SummaryUI::GetMenuTitle()
{
	return "Custom Training Statistics Summary";
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
	_cvarManager->executeCommand("togglemenu " + MenuName);
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
