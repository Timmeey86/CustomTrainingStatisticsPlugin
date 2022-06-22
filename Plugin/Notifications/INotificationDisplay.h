#pragma once

#include <string>

/** Defines the interface for something which can display notifications. */
class INotificationDisplay
{
protected:
	INotificationDisplay() = default;

public:
	virtual ~INotificationDisplay() = default;

	/** Registers a notification which could be displayed. */
	virtual void registerNotification(const std::string& uniqueName, const std::string& title) = 0;

	/** Display a registered notification using the given information. */
	virtual void displayNotification(const std::string& uniqueName, const std::string& detailedInfo) = 0;

	/** Checks if the notification is known already. */
	virtual bool hasNotification(const std::string& uniqueName) = 0;
};