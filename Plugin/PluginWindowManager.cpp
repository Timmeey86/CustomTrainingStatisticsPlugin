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

	_notificationManager = std::make_unique<ItsBranK::ImNotificationManager>("Notification Manager", "Notification Manager", nullptr);
	_notificationManager->OnAttach();

	_cvarManager->registerNotifier(TriggerNames::ToggleSummaryDisplay, [this](const std::vector<std::string>&) {
		_statSummaryShallBeShown = !_statSummaryShallBeShown;
	}, "Toggles display of the stat summary", PERMISSION_ALL);

	ItsBranK::ImNotificationManager::CreateNotification(new ItsBranK::ImNotification("unused", "Notification Name"))->SetInformation("Woop Woop", "BranK's da best!", ItsBranK::TextColors::Orange, ItsBranK::CornerPositions::TopRight);
	_cvarManager->registerNotifier("gpc_test", [](const std::vector<std::string>&) {
		ItsBranK::ImNotificationManager::ToggleNotification("Notification Name");
	}, "Temporary test", PERMISSION_ALL);

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
	return _summaryUi->GetMenuName();
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
	_summaryUi->OnOpen();
}

void PluginWindowManager::OnClose()
{
	_summaryUi->OnClose();
}