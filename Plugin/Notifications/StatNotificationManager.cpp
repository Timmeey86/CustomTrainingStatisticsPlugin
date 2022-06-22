#include <pch.h>
#include "StatNotificationManager.h"

struct NotificationId
{
public:
	static const std::string AirDribbles;
};
const std::string NotificationId::AirDribbles = "NOTIFICATION_AIRDRIBBLES";

StatNotificationManager::StatNotificationManager(INotificationDisplay* notificationDisplay)
	: _notificationDisplay{ notificationDisplay }
{
	notificationDisplay->registerNotification(NotificationId::AirDribbles, "Air Dribble Result");
}

void StatNotificationManager::displayPeakChange(const std::string& statName, const std::string& value)
{
	if (!_notificationDisplay->hasNotification(statName))
	{
		_notificationDisplay->registerNotification(statName, "New personal best!");
	}
	_notificationDisplay->displayNotification(
		statName,
		fmt::format("{}:\r\n{}", statName, value));
}

void StatNotificationManager::displayAirDribbleResult(double airDribbleTime, int numberOfTouches)
{
	_notificationDisplay->displayNotification(
		NotificationId::AirDribbles, 
		fmt::format("Air Dribble Time: {:.2f}s\r\nAir Dribble Touches: {}", airDribbleTime, numberOfTouches));
}
