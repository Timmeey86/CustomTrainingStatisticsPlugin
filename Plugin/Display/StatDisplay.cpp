#include <pch.h>
#include "StatDisplay.h"

#include <sstream>
#include <iomanip>

StatDisplay::StatDisplay(std::shared_ptr<PlayerStats> playerStats, std::shared_ptr<CalculatedData> calculatedData, std::shared_ptr<PluginState> pluginState)
	: _playerStats( playerStats )
	, _calculatedData( calculatedData )
	, _pluginState( pluginState )
{
}

// Converts e.g. 12.7531 into "12.75". It is recomended to do rounding before calling this
std::string to_percentage_string(double value)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << value << "%";
	return stream.str();
}

// Draws a stat consisting of a label and a string value into one "row"
void drawStat(CanvasWrapper canvas, float yOffset, const std::string& label, const std::string& value)
{
	canvas.SetPosition(Vector2F{ 10.0, yOffset });
	canvas.DrawString(label, 2.0f, 1.5f, false);
	canvas.SetPosition(Vector2F{ 290.0, yOffset });
	canvas.DrawString(value, 2.0f, 1.5f, false);
}

std::list<std::pair<std::string, std::string>> StatDisplay::getStatsToBeRendered() const
{
	std::list<std::pair<std::string, std::string>> statNamesAndValues;

	if (_pluginState->AttemptsAndGoalsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Attempts:", std::to_string(_playerStats->Attempts ));
		statNamesAndValues.emplace_back("Goals:", std::to_string(_playerStats->Goals));
	}
	if (_pluginState->CurrentStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Current Goal Streak:", std::to_string(_playerStats->GoalStreakCounter));
		statNamesAndValues.emplace_back("Current Miss Streak:", std::to_string(_playerStats->MissStreakCounter));
	}
	if (_pluginState->TotalSuccessRateShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Total Success Rate:", to_percentage_string(_calculatedData->SuccessPercentage));
	}
	if (_pluginState->LongestStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Longest Goal Streak:", std::to_string(_playerStats->LongestGoalStreak));
		statNamesAndValues.emplace_back("Longest Miss Streak:", std::to_string(_playerStats->LongestMissStreak));
	}
	if (_pluginState->PeakInfoShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Peak Success Rate:", to_percentage_string(_calculatedData->PeakSuccessPercentage));
		statNamesAndValues.emplace_back("Peak At Shot#:", std::to_string(_calculatedData->PeakShotNumber));
	}
	if (_pluginState->LastNShotPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Last 50 Shots", to_percentage_string(_calculatedData->Last50ShotsPercentage));
	}

	if (statNamesAndValues.empty())
	{
		statNamesAndValues.emplace_back("What did you expect?", "  ;-)");
	}

	return statNamesAndValues;
}
void StatDisplay::renderOneFrame(CanvasWrapper canvas)
{
	auto statNamesAndValues = getStatsToBeRendered();

	// Draw a panel so we can read the text on all kinds of maps
	LinearColor colors;
	colors.R = 100;
	colors.G = 100;
	colors.B = 100;
	colors.A = 200;
	canvas.SetColor(colors);

	canvas.SetPosition(Vector2F{ 5.0f, 195.0f });
	canvas.FillBox(Vector2F{ 400.0f, 10.0f + statNamesAndValues.size() * 20.0f });

	// Now draw the text on top of it
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	auto counter = .0f;
	for (const auto& [statName, value] : statNamesAndValues)
	{
		drawStat(canvas, 200.0f + counter * 20.0f, statName, value);
		counter += 1.0f;
	}
}
