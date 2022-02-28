#include "pch.h"
#include "GoalPercentageCounter.h"
#include "Calculation/StatUpdater.h"
#include "Calculation/AirDribbleAmountCounter.h"
#include "Calculation/GroundDribbleTimeCounter.h"
#include "Calculation/DoubleTapGoalCounter.h"
#include "Calculation/CloseMissCounter.h"
#include "Calculation/ShotDistributionTracker.h"
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
	auto shotDistributionTracker = std::make_shared<ShotDistributionTracker>(gameWrapper);
	auto statReader = std::make_shared<StatFileReader>(gameWrapper, shotDistributionTracker);
	auto statUpdater = std::make_shared<StatUpdater>(_shotStats, differenceData, _pluginState, statReader);

	// Enable storage of stats on the file system (crash recovery / maybe training trend in future)
	auto statWriter = std::make_shared<StatFileWriter>(gameWrapper, _shotStats, shotDistributionTracker);


	// Set up event registration
	_eventListener = std::make_shared<EventListener>(gameWrapper, cvarManager, _pluginState);

	// Register any event receivers before hooking into the events (otherwise they won't receive the events)
	_eventListener->addEventReceiver(std::make_shared<StatUpdaterEventBridge>(statUpdater, _pluginState));

	auto airDribbleCounter = std::make_shared<AirDribbleAmountCounter>(
		[this, statUpdater](int amount) { statUpdater->processAirDribbleTouches(amount); },
		[this, statUpdater](float time) { statUpdater->processAirDribbleTime(time); },
		[this, statUpdater](int amount) { statUpdater->processFlipReset(amount); }
	);
	_eventListener->addEventReceiver(airDribbleCounter);

	auto groundDribbleCounter = std::make_shared<GroundDribbleTimeCounter>(
		[this, statUpdater](float time) { statUpdater->processGroundDribbleTime(time); }
	);
	_eventListener->addEventReceiver(groundDribbleCounter);

	auto doubleTapGoalCounter = std::make_shared<DoubleTapGoalCounter>(
		[this, statUpdater]() { statUpdater->processDoubleTapGoal(); },
		cvarManager
	);
	_eventListener->addEventReceiver(doubleTapGoalCounter);

	auto closeMissCounter = std::make_shared<CloseMissCounter>(
		[this, statUpdater]() { statUpdater->processCloseMiss(); }
	);
	_eventListener->addEventReceiver(closeMissCounter);

	shotDistributionTracker->registerNotifiers(cvarManager);
	_eventListener->addEventReceiver(shotDistributionTracker);

	// Hook into events now 
	_eventListener->registerGameStateEvents();
	_eventListener->registerUpdateEvents(statUpdater, statWriter);


	// Enable rendering of output
	auto statDisplay = std::make_shared<StatDisplay>(_shotStats, differenceData, _pluginState);
	_eventListener->registerRenderEvents({ statDisplay, shotDistributionTracker });

	cvarManager->log("Loaded GoalPercentageCounter plugin");
}

void GoalPercentageCounter::onUnload()
{
	_eventListener.reset(); // Stop listening to events before destroying anything else
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}