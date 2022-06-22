#pragma once

#include <string>

/** Defines the interface for something which can display notifications. */
class INotificationDisplay
{
public:
	virtual ~INotificationDisplay() = default;

protected:
	INotificationDisplay() = default;

	/** Registers a notification which could be displayed. */
	virtual void registerNotification(const std::string& uniqueName, const std::string& title) = 0;

	/** Display a registered notification using the given information. */
	virtual void displayNotification(const std::string& uniqueName, const std::string& detailedInfo) = 0;
};