#include "pch.h"
#include "PluginWindowManager.h"
#include "../data/TriggerNames.h"

void PluginWindowManager::initPluginWindowManager(
	std::shared_ptr<SummaryUI> summaryUi,
	std::shared_ptr<CVarManagerWrapper> cvarManager,
	std::shared_ptr<GameWrapper> gameWrapper)
{
	_summaryUi = std::move(summaryUi);
	_cvarManager = std::move(cvarManager);
	_gameWrapper = std::move(gameWrapper);

	_notificationManager = std::make_unique<ItsBranK::ImNotificationManager>("Notification Manager", "Notification Manager", [this](const std::string&, bool) {
		onNotificationToggled();
	});
	_notificationManager->OnAttach();

	_cvarManager->registerNotifier(TriggerNames::ToggleSummaryDisplay, [this](const std::vector<std::string>&) {
		_statSummaryShallBeShown = !_statSummaryShallBeShown;
		toggleMenuIfNecessary(false);
	}, "Toggles display of the stat summary", PERMISSION_ALL);

	auto notification = new ItsBranK::ImNotification("unused", "Notification Name", nullptr);
	_notificationManager->CreateNotification(notification)->SetInformation("Woop Woop", "BranK's da best!", ItsBranK::TextColors::Orange, ItsBranK::CornerPositions::TopRight);
	_cvarManager->registerNotifier("gpc_test", [this](const std::vector<std::string>&) {
		_notificationManager->ToggleNotification("Notification Name");
	}, "Temporary test", PERMISSION_ALL);

}

void PluginWindowManager::onNotificationToggled()
{
	auto notificationsWereActive = _notificationsAreActive;

	_notificationsAreActive = _notificationManager->hasActiveNotifications();

	auto notificationActiveStateChanged = notificationsWereActive != _notificationsAreActive;
	_cvarManager->log(fmt::format("Notification toggled: {} {}", notificationsWereActive, _notificationsAreActive));
	toggleMenuIfNecessary(notificationActiveStateChanged);
}

void PluginWindowManager::toggleMenuIfNecessary(bool notificationActiveStateChanged)
{
	// Toggle the menu in the following cases:
	// - Neither notifications nor the stat summary were visible, but notifications are now visible (-> open the menu so we can see the notifications)
	// - Neither notifications nor the stat summary were visible, but the stat summary is now visible (-> open the menu so we can see the stat summary)
	// - Notifications were visible and are now no longer visible, and the stat summary is not visible (-> close the menu so the mouse cursor disappears)
	// - Stat summary was visible and was now closed, and there are no notifications (-> close the menu so the mouse cursor disappears)
	// In any other case, we don't want to change the state

	_cvarManager->log(fmt::format("toggleMenuIfNecessary: {} {} {} {}", notificationActiveStateChanged, _statSummaryShallBeShown, _notificationsAreActive, _menuIsVisible));

	if (notificationActiveStateChanged && !_statSummaryShallBeShown 
		|| !_notificationsAreActive && (_statSummaryShallBeShown != _menuIsVisible))
	{
		_cvarManager->log("Toggling");
		toggleMenu();
	}
	else
	{
		_cvarManager->log("Not toggling");
	}

}

void PluginWindowManager::toggleMenu()
{
	_cvarManager->executeCommand(fmt::format("togglemenu {};", GetMenuName()));
}

void PluginWindowManager::Render()
{
	_notificationManager->OnRender();
	if (_statSummaryShallBeShown)
	{
		_summaryUi->Render();
	}
}

std::string PluginWindowManager::GetMenuName()
{
	return "customtrainingstatistics_window_manager";
}

std::string PluginWindowManager::GetMenuTitle()
{
	return _summaryUi->GetMenuTitle();
}

void PluginWindowManager::SetImGuiContext(uintptr_t ctx)
{
	_summaryUi->SetImGuiContext(ctx);
}

bool PluginWindowManager::ShouldBlockInput()
{
	return _summaryUi->ShouldBlockInput();
}

bool PluginWindowManager::IsActiveOverlay()
{
	return _summaryUi->IsActiveOverlay();
}

void PluginWindowManager::OnOpen()
{
	_cvarManager->log("OnOpen");
	_summaryUi->OnOpen();
	_menuIsVisible = _summaryUi->isWindowOpen();
	toggleMenuIfNecessary(false);
}

void PluginWindowManager::OnClose()
{
	_statSummaryShallBeShown = false; // No matter if the last notification closed, or the summary window was closed: At this point, the user does not want to see the stat summary window any longer (or still not).
	_cvarManager->log("OnClose");
	_summaryUi->OnClose();
	_menuIsVisible = _summaryUi->isWindowOpen();
	toggleMenuIfNecessary(false);
}