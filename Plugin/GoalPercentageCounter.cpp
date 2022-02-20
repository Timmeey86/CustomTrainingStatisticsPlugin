#include "pch.h"
#include "GoalPercentageCounter.h"
#include "Calculation/StatUpdater.h"
#include "Display/StatDisplay.h"
#include "Core/EventListener.h"
#include "Core/StatUpdaterEventBridge.h"
#include "Settings/SettingsRegistration.h"
#include "Settings/PersistentStorage.h"
#include "Storage/StatFileWriter.h"
#include "Storage/StatFileReader.h"

// Note: In order to keep the automatic update chain working for users, this plugin is still called "Goal Percentage Counter" internally.
//       It will however display as "Custom Training Statistics" in the settings menu.
BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

void GoalPercentageCounter::onLoad()
{
	// Define a function which can trigger a command manually
	auto commandExecutionFunction = [this](const std::string& commandName) {
		gameWrapper->Execute([this, commandName](const GameWrapper*) {
			cvarManager->executeCommand(commandName);
		});
	};

	// Prevents settings from being cleared when we update (or recompile) the plugin
	auto persistentStorage = std::make_shared<PersistentStorage>(this, "CustomTrainingStatistics", true, true);

	// Register CVars before doing anything else, so they are properly restored from the config
	SettingsRegistration::registerCVars(commandExecutionFunction, cvarManager, persistentStorage, _pluginState);

	// Initialize the Settings page of the bakkesmod menu (F2)
	initPluginSettingsUi(commandExecutionFunction, cvarManager);

	auto differenceData = std::make_shared<ShotStats>(); // will store the difference between the previous session and the current one

	// Initialize the stats summary page
	initSummaryUi(cvarManager, _shotStats, differenceData, _pluginState);

	// Create handler classes
	auto statReader = std::make_shared<StatFileReader>(gameWrapper);
	auto statUpdater = std::make_shared<StatUpdater>(_shotStats, differenceData, _pluginState, statReader);

	// Enable rendering of output
	auto statDisplay = std::make_shared<StatDisplay>(_shotStats, differenceData, _pluginState);

	// Enable storage of stats on the file system (crash recovery / maybe training trend in future)
	auto statWriter = std::make_shared<StatFileWriter>(gameWrapper, _shotStats);


	// Set up event registration
	_eventListener = std::make_shared<EventListener>(gameWrapper, cvarManager, _pluginState);

	// Register any event receivers before hooking into the events (otherwise they won't receive the events)
	_eventListener->addEventReceiver(std::make_shared<StatUpdaterEventBridge>(statUpdater, _pluginState));

	// Hook into events now 
	_eventListener->registerGameStateEvents();
	_eventListener->registerUpdateEvents(statUpdater, statWriter);
	_eventListener->registerRenderEvents(statDisplay);

	cvarManager->log("Loaded GoalPercentageCounter plugin");
}

void GoalPercentageCounter::onUnload()
{
	_eventListener.reset(); // Stop listening to events before destroying anything else
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}