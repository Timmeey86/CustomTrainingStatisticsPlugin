#pragma once

#include "../Notifications/INotificationDisplay.h"

#include <memory>

/** This class is responsible for displaying notifications related to stat changes. */
class StatNotificationManager
{
public:
	// Note: NotificationDisplay is a raw pointer since the main plugin class needs to inherit from this, and there is no shared pointer for it
	StatNotificationManager(INotificationDisplay* notificationDisplay); 
	
	/** Displays a notification that a new all-time peak has been achieved.
	 *
	 * \param	statName	the name of the stat which has improved.
	 * \param	value		the value of the stat which has improved.
	 */
	void displayPeakChange(const std::string& statName, const std::string& value);

	/** Displays a notification about a completed air dribble. */
	void displayAirDribbleResult(double airDribbleTime, int numberOfTouches);
private:
	INotificationDisplay* _notificationDisplay; ///< Used for actually displaying the notifications
};