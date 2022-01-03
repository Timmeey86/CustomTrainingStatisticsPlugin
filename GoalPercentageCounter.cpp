#include "pch.h"
#include "GoalPercentageCounter.h"
#include <sstream>
#include <iomanip>

BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void GoalPercentageCounter::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Loaded GoalPercentageCounter plugin");
	cvarManager->registerCvar("goalpercentagecounter_enabled", "1", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		_enabled = cvar.getBoolValue();
	});

	// React to scored goals
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (_goalReplayIsActive) { return; }
		if (!_enabled) { return; }

		update(true, false); // This is a goal, and it is not a stat reset
	});

	// Count every shot as soon as any button is being pressed
	gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingShotAttempt", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (_goalReplayIsActive) { return; }
		if (!_enabled) { return; }

		update(false, false); // This is not a goal (but a miss), and it is not a stat reset
	});

	// Update statistics on each car spawn
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (!_enabled) { return; }

		// Update percentages, but do not modify attempts or goals
		recalculatePercentages();
	});
	
	// Allow resetting statistics to zero attempts/goals manually
	cvarManager->registerNotifier("goalpercentagecounter_reset", [this](const std::vector<std::string>&)
	{
		if (!gameWrapper->IsInCustomTraining()) { return; }
		// Note: Reset is allowed even with the plugin disabled (because why not?)
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
	}, "Reset the statistics.", PERMISSION_ALL);

	// Reset automatically when loading a new training pack, or when resetting it
	gameWrapper->HookEventPost("Function TAGame.GameEvent_TrainingEditor_TA.OnInit", [this](const std::string&) {
		if (!_enabled) { return; }
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
	});

	// Allow ignoring events which occur during a goal replay, it would otherwise spam us with goal events, and one reset event
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [this](const std::string&) {
		_goalReplayIsActive = true;
	});
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [this](const std::string&) {
		_goalReplayIsActive = false;
	});

	// Enable rendering of output
	gameWrapper->RegisterDrawable(std::bind(&GoalPercentageCounter::render, this, std::placeholders::_1));


}

void GoalPercentageCounter::onUnload()
{
	cvarManager->log("Unloaded GoalPercentageCounter plugin");
}

void GoalPercentageCounter::reset()
{
	_stats.Attempts = 0;
	_stats.Goals = 0;
	_stats.GoalStreakCounter = 0;
	_stats.MissStreakCounter = 0;
	_stats.LongestGoalStreak = 0;
	_stats.LongestMissStreak = 0;
	_stats.IgnoreNextShotReset = false;
	_stats.SuccessPercentage = 0;
	_stats.PeakSuccessPercentage = 0;
	_stats.Last50Shots.clear();
	_stats.Last50ShotsPercentage = 0;
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
		if (!_stats.Last50Shots.empty())
		{
			_stats.Last50Shots.back() = true; // Replace the current attempt with a goal entry
		}
		handleGoal();
	}
	else
	{
		_stats.Last50Shots.push_back(false);
		handleShotReset();
	}
}

void GoalPercentageCounter::recalculatePercentages()
{
	auto successPercentage = .0;
	if (_stats.Attempts > 0)
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = getPercentageValue(_stats.Attempts, _stats.Goals);
	}
	_stats.SuccessPercentage = successPercentage;

	// Update the peak percentage only after 20 shots since otherwise a couple of lucky early shots would create a wrong impression
	if (_stats.Attempts > 20 && _stats.SuccessPercentage > _stats.PeakSuccessPercentage)
	{
		_stats.PeakSuccessPercentage = _stats.SuccessPercentage;
	}

	// Update the percentage for the last 50 shots
	// Ignore the event if this is a reset after a goal
	while (_stats.Last50Shots.size() > 50)
	{
		_stats.Last50Shots.pop_front();
	}
	successPercentage = .0;
	if (!_stats.Last50Shots.empty())
	{
		auto numberOfGoals = std::count(_stats.Last50Shots.begin(), _stats.Last50Shots.end(), true);
		successPercentage = getPercentageValue((double)_stats.Last50Shots.size(), (double)numberOfGoals);
	}
	_stats.Last50ShotsPercentage = successPercentage;
}

void GoalPercentageCounter::handleGoal()
{
	_stats.MissStreakCounter = 0;
	_stats.GoalStreakCounter++;
	_stats.Goals++;

	// If we press reset after a goal, we mustn't treat it as a miss.
	_stats.IgnoreNextShotReset = true;

	if (_stats.GoalStreakCounter > _stats.LongestGoalStreak)
	{
		_stats.LongestGoalStreak = _stats.GoalStreakCounter;
	}
}

void GoalPercentageCounter::handleShotReset()
{
	// Count the shot attempt in any case
	_stats.Attempts++;

	if (_stats.IgnoreNextShotReset)
	{
		// A goal was scored, and then reset was pressed. No further action required, but do not ignore any further resets.
		_stats.IgnoreNextShotReset = false;
	}
	else
	{
		_stats.MissStreakCounter++;
		_stats.GoalStreakCounter = 0;

		if (_stats.MissStreakCounter > _stats.LongestMissStreak)
		{
			_stats.LongestMissStreak = _stats.MissStreakCounter;
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
	if (!_enabled) { return; }

	// Draw a panel so we can read the text on all kinds of maps
	LinearColor colors;
	colors.R = 100;
	colors.G = 100;
	colors.B = 100;
	colors.A = 200;
	canvas.SetColor(colors);

	canvas.SetPosition(Vector2F{ 5.0, 195.0 });
	canvas.FillBox(Vector2F{ 400.0, 190.0 });

	// Now draw the text on top of it
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	drawIntStat(canvas, 200.0, "Attempts:", _stats.Attempts);
	drawIntStat(canvas, 220.0, "Goals:", _stats.Goals);
	drawIntStat(canvas, 240.0, "Current Goal Streak:", _stats.GoalStreakCounter);
	drawIntStat(canvas, 260.0, "Current Miss Streak:", _stats.MissStreakCounter);
	drawPercentageStat(canvas, 280.0, "Total Success Rate:", _stats.SuccessPercentage);
	drawIntStat(canvas, 300.0, "Longest Goal Streak:", _stats.LongestGoalStreak);
	drawIntStat(canvas, 320.0, "Longest Miss Streak:", _stats.LongestMissStreak);
	drawPercentageStat(canvas, 340.0, "Peak Success Rate:", _stats.PeakSuccessPercentage);
	drawPercentageStat(canvas, 360.0, "Last 50 Shots:", _stats.Last50ShotsPercentage);
}