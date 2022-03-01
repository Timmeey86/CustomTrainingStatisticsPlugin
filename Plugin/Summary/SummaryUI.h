# pragma once

#include <memory>

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "Data/ShotStats.h"
#include "Data/PluginState.h"

class SummaryUI : public BakkesMod::Plugin::PluginWindow
{
public:
	/** Initializes the stats summary UI. */
	void initSummaryUi(
		const std::shared_ptr<CVarManagerWrapper> cvarManager,
		const std::shared_ptr<const ShotStats> shotStats,
		const std::shared_ptr<const ShotStats> diffData,
		const std::shared_ptr<const PluginState> pluginState);

	/** Do ImGui rendering here */
	void Render() override;

	/** Name of the menu that is used to toggle the window. */
	std::string GetMenuName() override;

	/** Title to give the menu */
	std::string GetMenuTitle() override;

	/** Don't call this yourself, BM will call this function with a pointer to the current ImGui context */
	void SetImGuiContext(uintptr_t ctx) override;

	/** Should events such as mouse clicks/key inputs be blocked so they won't reach the game */
	bool ShouldBlockInput() override;

	/** Return true if overlay which isn't interacted with */
	bool IsActiveOverlay() override;

	/** Called when window is opened */
	void OnOpen() override;

	/** Called when window is closed */
	void OnClose() override;

	void ToggleMenu();
	
	void OpenMenu();

	void CloseMenu();


	static const std::string MenuName;

private:
	void renderSummary();
	void copyTrainingPackCode() const;
	void copyStatisticsSummary() const;

	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Allows registering and retrieving custom variables.
	std::shared_ptr<const ShotStats> _shotStats;
	std::shared_ptr<const ShotStats> _diffData;
	std::shared_ptr<const PluginState> _pluginState;
	bool _shouldBlockInput = false;
	bool _isWindowOpen = false;
};