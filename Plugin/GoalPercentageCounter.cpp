#include "pch.h"
#include "GoalPercentageCounter.h"
#include <sstream>
#include <iomanip>

BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

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

	initPluginSettingsUi(commandExecutionFunction, cvarRetrievalFunction);
		

	_globalCvarManager = cvarManager;
	cvarManager->log("Loaded GoalPercentageCounter plugin");
	cvarManager->registerCvar("goalpercentagecounter_enabled", "1", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		_pluginState.PluginIsEnabled = cvar.getBoolValue();
	});

	// REFACTOR - IStatUpdater
	// React to scored goals
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (_pluginState.GoalReplayIsActive) { return; }
		if (!_pluginState.PluginIsEnabled) { return; }

		update(true, false); // This is a goal, and it is not a stat reset
	});

	// REFACTOR - IStatUpdater
	// Count every shot as soon as any button is being pressed
	gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (_pluginState.GoalReplayIsActive) { return; }
		if (!_pluginState.PluginIsEnabled) { return; }

		update(false, false); // This is not a goal (but a miss), and it is not a stat reset
	});

	// REFACTOR - IStatUpdater
	// Update statistics on each car spawn
	// TODO: Maybe implement a mini state machine: Idle -> Attempt -> Goal -> Reset -> Idle, or Idle -> Attempt -> Miss (Reset without goal) -> Idle
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (!_pluginState.PluginIsEnabled) { return; }

		// Update percentages, but do not modify attempts or goals
		recalculatePercentages();
	});
	
	// REFACTOR - IStatUpdater
	// Allow resetting statistics to zero attempts/goals manually
	cvarManager->registerNotifier("goalpercentagecounter_reset", [this](const std::vector<std::string>&)
	{
		if (!gameWrapper->IsInCustomTraining()) { return; }
		// Note: Reset is allowed even with the plugin disabled (because why not?)
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
	}, "Reset the statistics.", PERMISSION_ALL);

	// REFACTOR - IStatUpdater
	// Reset automatically when loading a new training pack, or when resetting it
	gameWrapper->HookEventPost("Function TAGame.GameEvent_TrainingEditor_TA.OnInit", [this](const std::string&) {
		if (!_pluginState.PluginIsEnabled) { return; }
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
	});

	// REFACTOR - Directly within in EventListener (no need for an extra class)
	// Allow ignoring events which occur during a goal replay, it would otherwise spam us with goal events, and one reset event
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [this](const std::string&) {
		_pluginState.GoalReplayIsActive = true;
	});
	// UNVERIFIED ASSUMPTION: There is no way for leaving ReplayPlayback without EndState being called
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [this](const std::string&) {
		_pluginState.GoalReplayIsActive = false;
	});

	// REFACTOR - IStatDisplay
	// Enable rendering of output
	gameWrapper->RegisterDrawable(std::bind(&GoalPercentageCounter::render, this, std::placeholders::_1));


}

void GoalPercentageCounter::onUnload()
{
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}

void GoalPercentageCounter::reset()
{
	_playerStats = PlayerStats();
	_calculatedData = CalculatedData();

	// Do not reset anything which can be changed by user settings - Those settings shall of course not be reset.
	// Also do not reset anything which is based on the Rocket League state
	_pluginState.PreviousAttemptWasAGoal = false;
}
void GoalPercentageCounter::update(bool isGoal, bool isReset)
{
	if (!isReset)
	{
		// The function was called after resetting a shot or scoring a goal => Update statistics
		registerAttempt(isGoal);
	}
	recalculatePercentages();
}

double getPercentageValue(double attempts, double goals)
{
	return round((goals / attempts) * 10000.0) / 100.0;
}
void GoalPercentageCounter::registerAttempt(bool isGoal)
{
	_globalCvarManager->log("Registering attempt: " + std::to_string(isGoal));
	if (isGoal)
	{
		handleGoal();
	}
	else
	{
		handleAttempt();
	}
}

void GoalPercentageCounter::recalculatePercentages()
{
	auto successPercentage = .0;
	if (_playerStats.Attempts > 0)
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = getPercentageValue(_playerStats.Attempts, _playerStats.Goals);
	}
	_calculatedData.SuccessPercentage = successPercentage;

	// Update the percentage for the last 50 shots
	// Ignore the event if this is a reset after a goal
	while (_playerStats.Last50Shots.size() > 50)
	{
		_playerStats.Last50Shots.pop_front();
	}
	successPercentage = .0;
	if (!_playerStats.Last50Shots.empty())
	{
		auto numberOfGoals = std::count(_playerStats.Last50Shots.begin(), _playerStats.Last50Shots.end(), true);
		successPercentage = getPercentageValue((double)_playerStats.Last50Shots.size(), (double)numberOfGoals);
	}
	_calculatedData.Last50ShotsPercentage = successPercentage;

	// Update the peak percentage only after 20 shots since otherwise a couple of lucky early shots would create a wrong impression
	if (_playerStats.Attempts > 20 && _calculatedData.Last50ShotsPercentage > _calculatedData.PeakSuccessPercentage)
	{
		_calculatedData.PeakSuccessPercentage = _calculatedData.Last50ShotsPercentage;
		_calculatedData.PeakShotNumber = _playerStats.Attempts;
	}
}

void GoalPercentageCounter::handleGoal()
{
	if (!_playerStats.Last50Shots.empty())
	{
		_playerStats.Last50Shots.back() = true; // Replace the current attempt with a goal entry
	}
	_playerStats.MissStreakCounter = 0;
	_playerStats.GoalStreakCounter++;
	_playerStats.Goals++;

	// When starting the next attempt, we don't know whether the previous one was a goal or a miss without this flag
	// (there is no trigger for a miss, unless we incorporate the reset trigger for it (we might actually do that in future))
	_pluginState.PreviousAttemptWasAGoal = true;

	if (_playerStats.GoalStreakCounter > _playerStats.LongestGoalStreak)
	{
		_playerStats.LongestGoalStreak = _playerStats.GoalStreakCounter;
	}
}

void GoalPercentageCounter::handleAttempt()
{
	_playerStats.Last50Shots.push_back(false);

	// Count the shot attempt in any case
	_playerStats.Attempts++;

	if (_pluginState.PreviousAttemptWasAGoal)
	{
		// A goal was scored, and then reset was pressed. No further action required, but do not ignore any further resets.
		_pluginState.PreviousAttemptWasAGoal = false;
	}
	else
	{
		_playerStats.MissStreakCounter++;
		_playerStats.GoalStreakCounter = 0;

		if (_playerStats.MissStreakCounter > _playerStats.LongestMissStreak)
		{
			_playerStats.LongestMissStreak = _playerStats.MissStreakCounter;
		}
	}
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
	if (!_pluginState.PluginIsEnabled) { return; }

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

	drawIntStat(canvas, 200.0, "Attempts:", _playerStats.Attempts);
	drawIntStat(canvas, 220.0, "Goals:", _playerStats.Goals);
	drawIntStat(canvas, 240.0, "Current Goal Streak:", _playerStats.GoalStreakCounter);
	drawIntStat(canvas, 260.0, "Current Miss Streak:", _playerStats.MissStreakCounter);
	drawPercentageStat(canvas, 280.0, "Total Success Rate:", _calculatedData.SuccessPercentage);
	drawIntStat(canvas, 300.0, "Longest Goal Streak:", _playerStats.LongestGoalStreak);
	drawIntStat(canvas, 320.0, "Longest Miss Streak:", _playerStats.LongestMissStreak);
	drawPercentageStat(canvas, 340.0, "Peak Success Rate:", _calculatedData.PeakSuccessPercentage);
	drawIntStat(canvas, 360.0, "Peak At Shot#:", _calculatedData.PeakShotNumber);
	drawPercentageStat(canvas, 380.0, "Last 50 Shots:", _calculatedData.Last50ShotsPercentage);
}