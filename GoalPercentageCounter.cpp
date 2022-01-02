#include "pch.h"
#include "GoalPercentageCounter.h"


BAKKESMOD_PLUGIN(GoalPercentageCounter, "Goal Percentage Counter", plugin_version, PLUGINTYPE_CUSTOM_TRAINING)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool _goalReplayIsActive = false;

void GoalPercentageCounter::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Loaded GoalPercentageCounter plugin");

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
	
	// Allow resetting statistics to zero attempts/goals
	cvarManager->registerNotifier("goalpercentagecounter_reset", [this](const std::vector<std::string>&)
	{
		if (!gameWrapper->IsInCustomTraining()) { return; }
		reset();
		update(false, true); // This is not a goal, and it is a stat reset
	}, "Reset the statistics.", PERMISSION_ALL);

	// Allow ignoring events which occur during a goal replay, it would otherwise spam us with goal events, and one reset event

	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [](const std::string&) {
		_goalReplayIsActive = true;
	});
	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [](const std::string&) {
		_goalReplayIsActive = false;
	});


	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	cvarManager->log("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&GoalPercentageCounter::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&GoalPercentageCounter::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&GoalPercentageCounter::YourPluginMethod, this);
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
	// else: Update the UI but don't recalculate anything

	// TBD
	_globalCvarManager->log("Attempts: " + std::to_string(_stats.Attempts));
	_globalCvarManager->log("Goals: " + std::to_string(_stats.Goals));
	_globalCvarManager->log("Longest Goal Streak: " + std::to_string(_stats.LongestGoalStreak));
	_globalCvarManager->log("Longest Miss Streak: " + std::to_string(_stats.LongestMissStreak));
	_globalCvarManager->log("Success Rate: " + std::to_string(successPercentage));
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