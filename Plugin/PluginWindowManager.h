#pragma once

#include <Notifications/INotificationDisplay.h>

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"
#include "bakkesmod/wrappers/gamewrapper.h"

#include <external/ItsBranK/ImNotificationManager.h>
#include <Summary/SummaryUI.h>

#include <unordered_map>

/** Allows using multiple plugin windows like the stat summary and notifications, for example. */
class PluginWindowManager : public BakkesMod::Plugin::PluginWindow, INotificationDisplay
{
public:
	void initPluginWindowManager(
		std::shared_ptr<SummaryUI> summaryUi,
		std::shared_ptr<CVarManagerWrapper> cvarManager,
		std::shared_ptr<GameWrapper> gameWrapper);

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

	// Inherited via INotificationDisplay
	void registerNotification(const std::string& uniqueName, const std::string& title) override;
	void displayNotification(const std::string& uniqueName, const std::string& detailedInfo) override;

private:

	void onNotificationToggled();
	void toggleMenuIfNecessary(bool notificationActiveStateChanged);

	void toggleMenu();

	bool _statSummaryShallBeShown = false;
	bool _notificationsShallBeShown = true;
	bool _notificationsAreActive = false;
	bool _menuIsVisible = false;


	std::unique_ptr<ItsBranK::ImNotificationManager> _notificationManager;
	std::shared_ptr<SummaryUI> _summaryUi;
	std::shared_ptr<CVarManagerWrapper> _cvarManager;
	std::shared_ptr<GameWrapper> _gameWrapper;
	std::unordered_map<std::string, std::shared_ptr<ItsBranK::ImNotification>> _notifications;
};