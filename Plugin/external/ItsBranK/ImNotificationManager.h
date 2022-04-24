#pragma once

// Extracted from https://github.com/CodeRedModding/CodeRed-ImGui

#include "ImNotification.h"

#include <map>
#include <memory>
#include <string>


namespace ItsBranK
{
	class ImNotificationManager : public ImInterface
	{
	private:
		static inline std::map<std::string, std::shared_ptr<ImNotification>> CreatedNotifications;
		static inline std::vector<std::shared_ptr<ImNotification>> ActiveNotifications;

	public:
		ImNotificationManager(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallback);
		~ImNotificationManager() override;

	public:
		void OnAttach() override;
		void OnDetatch() override;
		void OnRender() override;

	public:
		static void ToggleNotification(const std::string& windowName);
		static std::shared_ptr<ImNotification> GetNotification(const std::string& windowName);
		static std::shared_ptr<ImNotification> CreateNotification(ImNotification* notification);
	};
}