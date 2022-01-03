#include "pch.h"
#include "GoalPercentageCounter.h"
#include <sstream>
#include <iomanip>

BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool _goalReplayIsActive = false;
bool _isFirstSpawn = false;

void GoalPercentageCounter::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Loaded GoalPercentageCounter plugin");

	// TODO: Auto Reset when changing training packs
	// TODO: Do not draw canvas while not in custom training

	// React to scored goals
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (_goalReplayIsActive) { return; }

		update(true, false); // This is a goal, and it is not a stat reset
	});

	// React to car spawns
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", [this](const std::string&) {
		if (!gameWrapper->IsInCustomTraining()) { return; }
		if (_goalReplayIsActive) { return; }

		update(false, false); // This is not a goal (but a miss), and it is not a stat reset
	});
	
	// Allow resetting statistics to zero attempts/goals manually
	cvarManager->registerNotifier("goalpercentagecounter_reset", [this](const std::vector<std::string>&)
	{
		if (!gameWrapper->IsInCustomTraining()) { return; }
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
	}, "Reset the statistics.", PERMISSION_ALL);

	// Reset automatically when loading a new training pack, or when resetting it
	gameWrapper->HookEventPost("Function TAGame.GameEvent_TrainingEditor_TA.OnInit", [this](const std::string&) {
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
		_isFirstSpawn = true;
	});

	// Allow ignoring events which occur during a goal replay, it would otherwise spam us with goal events, and one reset event
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [](const std::string&) {
		_goalReplayIsActive = true;
	});
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [](const std::string&) {
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
}
void GoalPercentageCounter::update(bool isGoal, bool isReset)
{
	double successPercentage = .0;
	if (!isReset)
	{
		// The function was called after resetting a shot or scoring a goal => Update statistics
		recalculateStats(isGoal, successPercentage);
	}
	_stats.SuccessPercentage = successPercentage;
}
void GoalPercentageCounter::recalculateStats(bool isGoal, double& successPercentage)
{
	if (isGoal)
	{
		handleGoal();
	}
	else
	{
		handleShotReset();
	}

	successPercentage = 0;
	if (_stats.Attempts > 0) 
	{
		// Calculate the success percentage in percent, including two decimal digits
		successPercentage = round(((double)_stats.Goals / (double)_stats.Attempts) * 10000.0) / 100.0;
	}
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
	if (_isFirstSpawn)
	{
		// Do not count the initial spawn as an attempt, but rather count the attempt once the car was reset
		_isFirstSpawn = false;
		return;
	}

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

void GoalPercentageCounter::render(CanvasWrapper canvas) const
{
	if (!gameWrapper->IsInCustomTraining()) { return; }

	// Draw a panel so we can read the text on all kinds of maps
	LinearColor colors;
	colors.R = 100;
	colors.G = 100;
	colors.B = 100;
	colors.A = 200;
	canvas.SetColor(colors);

	canvas.SetPosition(Vector2F{ 5.0, 195.0 });
	canvas.FillBox(Vector2F{ 330.0, 110.0 });

	// Now draw the text on top of it
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	canvas.SetPosition(Vector2F{ 10.0, 200.0 });
	canvas.DrawString("Attempts: " + std::to_string(_stats.Attempts), 2.0f, 1.5f, false);

	canvas.SetPosition(Vector2F{ 10.0, 220.0 });
	canvas.DrawString("Goals: " + std::to_string(_stats.Goals), 2.0f, 1.5f, false);

	canvas.SetPosition(Vector2F{ 10.0, 240.0 });
	canvas.DrawString("Longest Goal Streak: " + std::to_string(_stats.LongestGoalStreak), 2.0f, 1.5f, false);

	canvas.SetPosition(Vector2F{ 10.0, 260.0 });
	canvas.DrawString("Longest Miss Streak: " + std::to_string(_stats.LongestMissStreak), 2.0f, 1.5f, false);

	canvas.SetPosition(Vector2F{ 10.0, 280.0 });
	std::ostringstream successRateStream;
	successRateStream << std::fixed << std::setprecision(2) << _stats.SuccessPercentage;
	auto successRateString = successRateStream.str();
	canvas.DrawString("Success Rate: " + successRateString, 2.0f, 1.5f, false);
}