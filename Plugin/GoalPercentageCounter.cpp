#include "pch.h"
#include "GoalPercentageCounter.h"
#include "Calculation/StatUpdater.h"
#include "Core/EventListener.h"

#include <sstream>
#include <iomanip>

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
	cvarManager->registerCvar("goalpercentagecounter_enabled", "1", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](const std::string&, CVarWrapper cvar) {
		_pluginState->PluginIsEnabled = cvar.getBoolValue();
	});

	// REFACTOR - IStatDisplay
	// Enable rendering of output
	gameWrapper->RegisterDrawable(std::bind(&GoalPercentageCounter::render, this, std::placeholders::_1));

	// Create handler classes
	auto statUpdater = std::make_shared<StatUpdater>(_playerStats, _calculatedData, _pluginState);

	// Set up event registration
	_eventListener = std::make_shared<EventListener>(gameWrapper, cvarManager, _pluginState);
	_eventListener->registerGameStateEvents();
	_eventListener->registerUpdateEvents(statUpdater);
	_eventListener->registerRenderEvents(nullptr); // REFACTOR TODO Supply IStatDisplay
}

void GoalPercentageCounter::onUnload()
{
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}


std::string to_percentage_string(double value)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << value << "%";
	return stream.str();
}

void drawStat(CanvasWrapper canvas, float yOffset, const std::string& label, const std::string& value)
{
	canvas.SetPosition(Vector2F{ 10.0, yOffset });
	canvas.DrawString(label, 2.0f, 1.5f, false);
	canvas.SetPosition(Vector2F{ 290.0, yOffset });
	canvas.DrawString(value, 2.0f, 1.5f, false);
}

void drawIntStat(CanvasWrapper canvas, float yOffset, const std::string& label, int value)
{
	drawStat(canvas, yOffset, label, std::to_string(value));
}

void drawPercentageStat(CanvasWrapper canvas, float yOffset, const std::string& label, double percentageValue)
{
	drawStat(canvas, yOffset, label, to_percentage_string(percentageValue));
}

void GoalPercentageCounter::render(CanvasWrapper canvas) const
{
	if (!gameWrapper->IsInCustomTraining()) { return; }
	if (!_pluginState->PluginIsEnabled) { return; }

	// Draw a panel so we can read the text on all kinds of maps
	LinearColor colors;
	colors.R = 100;
	colors.G = 100;
	colors.B = 100;
	colors.A = 200;
	canvas.SetColor(colors);

	canvas.SetPosition(Vector2F{ 5.0, 195.0 });
	canvas.FillBox(Vector2F{ 400.0, 210.0 });

	// Now draw the text on top of it
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	drawIntStat(canvas, 200.0, "Attempts:", _playerStats->Attempts);
	drawIntStat(canvas, 220.0, "Goals:", _playerStats->Goals);
	drawIntStat(canvas, 240.0, "Current Goal Streak:", _playerStats->GoalStreakCounter);
	drawIntStat(canvas, 260.0, "Current Miss Streak:", _playerStats->MissStreakCounter);
	drawPercentageStat(canvas, 280.0, "Total Success Rate:", _calculatedData->SuccessPercentage);
	drawIntStat(canvas, 300.0, "Longest Goal Streak:", _playerStats->LongestGoalStreak);
	drawIntStat(canvas, 320.0, "Longest Miss Streak:", _playerStats->LongestMissStreak);
	drawPercentageStat(canvas, 340.0, "Peak Success Rate:", _calculatedData->PeakSuccessPercentage);
	drawIntStat(canvas, 360.0, "Peak At Shot#:", _calculatedData->PeakShotNumber);
	drawPercentageStat(canvas, 380.0, "Last 50 Shots:", _calculatedData->Last50ShotsPercentage);
}