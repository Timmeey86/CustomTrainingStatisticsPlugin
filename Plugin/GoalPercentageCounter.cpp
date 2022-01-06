#include "pch.h"
#include "GoalPercentageCounter.h"
#include "Calculation/StatUpdater.h"
#include "Display/StatDisplay.h"
#include "Core/EventListener.h"
#include "Settings/SettingsRegistration.h"

BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

void GoalPercentageCounter::onLoad()
{
	// Define a function which can trigger a command manually
	auto commandExecutionFunction = [this](const std::string& commandName) {
		gameWrapper->Execute([this, commandName](const GameWrapper*) {
			cvarManager->executeCommand(commandName);
		});
	};

	// Register CVars before doing anything else, so they are properly restored from the config
	SettingsRegistration::registerCVars(commandExecutionFunction, cvarManager, _pluginState);	

	// Initialize the Settings page of the bakkesmod menu (F2)
	initPluginSettingsUi(commandExecutionFunction, cvarManager);

	// Enable rendering of output
	auto statDisplay = std::make_shared<StatDisplay>(_playerStats, _calculatedData, _pluginState);

	// Create handler classes
	auto statUpdater = std::make_shared<StatUpdater>(_playerStats, _calculatedData, _pluginState);

	// Set up event registration
	_eventListener = std::make_shared<EventListener>(gameWrapper, cvarManager, _pluginState);
	_eventListener->registerGameStateEvents();
	_eventListener->registerUpdateEvents(statUpdater);
	_eventListener->registerRenderEvents(statDisplay);

	cvarManager->log("Loaded GoalPercentageCounter plugin");
}

void GoalPercentageCounter::onUnload()
{
	_eventListener.reset(); // Stop listening to events before destroying anything else
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}