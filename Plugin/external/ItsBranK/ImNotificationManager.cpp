#include <pch.h>
#include "ImNotificationManager.h"
#include "ImEnums.h"

namespace ItsBranK
{
	ImNotificationManager::ImNotificationManager(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallback)
		: ImInterface(title, name, toggleCallback, false) 
	{
		ImGuiIO& io = ImGui::GetIO();
		auto regularFont = io.Fonts->AddFontDefault(nullptr, 20.0f);
		auto notReallyBoldFont = io.Fonts->AddFontDefault(nullptr, 24.0f);
		Test::ImFontMap[TextStyles::Regular] = regularFont;
		Test::ImFontMap[TextStyles::Bold] = notReallyBoldFont;
		io.Fonts->Build();
	}

	ImNotificationManager::~ImNotificationManager() { OnDetatch(); }

	void ImNotificationManager::OnAttach()
	{
		SetAttached(true);
	}
	
	void ImNotificationManager::OnDetatch()
	{
		if (IsAttached())
		{
			ActiveNotifications.clear();

			for (auto& notification : CreatedNotifications)
			{
				if (notification.second)
				{
					notification.second->OnDetatch();
				}
			}

			CreatedNotifications.clear();
			SetAttached(false);
		}
	}

	void ImNotificationManager::updateNotificationState()
	{
		auto notificationsWereEmpty = ActiveNotifications.empty();
		if (!notificationsWereEmpty)
		{
			for (auto notificationIt = ActiveNotifications.begin(); notificationIt != ActiveNotifications.end();)
			{
				if (*notificationIt && !(*notificationIt)->ShouldRender())
				{
					notificationIt = ActiveNotifications.erase(notificationIt);
				}
				else
				{
					notificationIt++;
				}
			}
		}

		for (const auto& notification : CreatedNotifications)
		{
			if (notification.second && notification.second->ShouldRender())
			{
				bool alreadyActive = false;

				for (const auto& activeNotification : ActiveNotifications)
				{
					if (activeNotification == notification.second)
					{
						alreadyActive = true;
						break;
					}
				}

				if (!alreadyActive)
				{
					ActiveNotifications.push_back(notification.second);
				}
			}
		}

		if (notificationsWereEmpty != ActiveNotifications.empty() && ToggleCallback)
		{
			ToggleCallback(GetTitle(), !ActiveNotifications.empty());
		}
	}

	void ImNotificationManager::OnRender()
	{
		updateNotificationState();
		if (!ActiveNotifications.empty())
		{
			for (size_t i = 0; i < ActiveNotifications.size(); i++)
			{
				std::shared_ptr<ImNotification> notification = ActiveNotifications[i];

				if (notification)
				{
					float offset = 0.0f;

					if (i != 0)
					{
						offset = (i * 105.0f); // 85 being the notification's window height (75) plus padding (10).
					}

					notification->SetOffset(offset);
					notification->OnRender();
				}
			}
		}
	}

	void ImNotificationManager::ToggleNotification(const std::string& windowName)
	{
		std::shared_ptr<ImNotification> notification = GetNotification(windowName);

		if (notification)
		{
			notification->ToggleRender();
		}
		// Dirty hack: Render once to update the internal maps and send a callback if necessary
		updateNotificationState();
	}

	std::shared_ptr<ImNotification> ImNotificationManager::GetNotification(const std::string& windowName)
	{
		if (CreatedNotifications.find(windowName) != CreatedNotifications.end())
		{
			return CreatedNotifications[windowName];
		}

		return nullptr;
	}

	std::shared_ptr<ImNotification> ImNotificationManager::CreateNotification(ImNotification* notification)
	{
		if (notification)
		{
			std::string notificationName = notification->GetName();

			if (CreatedNotifications.find(notificationName) == CreatedNotifications.end())
			{
				CreatedNotifications.emplace(notificationName, std::shared_ptr<ImNotification>(notification)).first->second->OnAttach();
				return CreatedNotifications[notificationName];
			}
		}

		return nullptr;
	}
}