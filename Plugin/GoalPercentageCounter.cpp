#include "pch.h"
#include "GoalPercentageCounter.h"
#include "Calculation/StatUpdater.h"
#include "Display/StatDisplay.h"
#include "Core/EventListener.h"
#include "Data/ConfigurationOptions.h"


BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

void GoalPercentageCounter::onLoad()
{
	// Define a function which can trigger a command manually
	auto commandExecutionFunction = [this](const std::string& commandName) {
		gameWrapper->Execute([this, commandName](const GameWrapper*) {
			cvarManager->executeCommand(commandName);
		});
	};
	// Define a function which is able to retrieve a cvar without having to know the cvar manager
	auto cvarRetrievalFunction = [this](const std::string& variableName) { return cvarManager->getCvar(variableName); };

	// Initialize the Settings page of the bakkesmod menu (F2)
	initPluginSettingsUi(commandExecutionFunction, cvarRetrievalFunction);
		
	// Initialize variables which can be configured by the user (currently this is only a single enabled flag. Create a new class if there's more)
	cvarManager->log("Loaded GoalPercentageCounter plugin");
	cvarManager->registerCvar(ConfigurationOptions::EnablePlugin, "1", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](const std::string&, CVarWrapper cvar) {
		_pluginState->PluginIsEnabled = cvar.getBoolValue();
	});

	// Enable rendering of output
	auto statDisplay = std::make_shared<StatDisplay>(_playerStats, _calculatedData);

	// Create handler classes
	auto statUpdater = std::make_shared<StatUpdater>(_playerStats, _calculatedData, _pluginState);

	// Set up event registration
	_eventListener = std::make_shared<EventListener>(gameWrapper, cvarManager, _pluginState);
	_eventListener->registerGameStateEvents();
	_eventListener->registerUpdateEvents(statUpdater);
	_eventListener->registerRenderEvents(statDisplay);
}

void GoalPercentageCounter::onUnload()
{
	_eventListener.reset(); // Stop listening to events before destroying anything else
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}