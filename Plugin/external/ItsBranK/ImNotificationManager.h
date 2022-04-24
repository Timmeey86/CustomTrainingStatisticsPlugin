#pragma once

// Extracted from https://github.com/CodeRedModding/CodeRed-ImGui
// This class was adapted to be non-static in order to be able to trigger the callback outside of the render method

#include "ImNotification.h"

#include <map>
#include <memory>
#include <string>


namespace ItsBranK
{
	class ImNotificationManager : public ImInterface
	{
	private:
		std::map<std::string, std::shared_ptr<ImNotification>> CreatedNotifications;
		std::vector<std::shared_ptr<ImNotification>> ActiveNotifications;
		void updateNotificationState();

	public:
		ImNotificationManager(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallback);
		~ImNotificationManager() override;

	public:
		void OnAttach() override;
		void OnDetatch() override;
		void OnRender() override;

	public:
		void ToggleNotification(const std::string& windowName);
		std::shared_ptr<ImNotification> GetNotification(const std::string& windowName);
		std::shared_ptr<ImNotification> CreateNotification(ImNotification* notification);
		inline bool hasActiveNotifications() { return !ActiveNotifications.empty(); }
	};
}