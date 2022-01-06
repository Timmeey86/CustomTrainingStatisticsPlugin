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
void drawStat(CanvasWrapper canvas, const PluginState* const pluginState, int rowNumber, const std::string& label, const std::string& value)
{
	auto leftTextBorder = (float)pluginState->OverlayXPosition + 5.0f * pluginState->TextWidthFactor;
	auto topTextBorder = (float)pluginState->OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * pluginState->TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(label, pluginState->TextWidthFactor, pluginState->TextHeightFactor, false);
	canvas.SetPosition(Vector2F{ leftTextBorder + 140.0f * pluginState->TextWidthFactor, topTextBorder });
	canvas.DrawString(value, pluginState->TextWidthFactor, pluginState->TextHeightFactor, false);
}

std::list<std::pair<std::string, std::string>> StatDisplay::getStatsToBeRendered() const
{
	std::list<std::pair<std::string, std::string>> statNamesAndValues;

	auto goalName = std::string{ _pluginState->TrackInitialBallHitInsteadOfGoal ? "Hit" : "Goal" };

	if (_pluginState->AttemptsAndGoalsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Attempts:", std::to_string(_playerStats->Attempts ));
		statNamesAndValues.emplace_back(goalName + "s:", std::to_string(_playerStats->Goals));
	}
	if (_pluginState->CurrentStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Current " + goalName + " Streak:", std::to_string(_playerStats->GoalStreakCounter));
		statNamesAndValues.emplace_back("Current Miss Streak:", std::to_string(_playerStats->MissStreakCounter));
	}
	if (_pluginState->TotalSuccessRateShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Total Success Rate:", to_percentage_string(_calculatedData->SuccessPercentage));
	}
	if (_pluginState->LongestStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Longest " + goalName + " Streak:", std::to_string(_playerStats->LongestGoalStreak));
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

	canvas.SetPosition(Vector2F{ (float)_pluginState->OverlayXPosition, (float)_pluginState->OverlayYPosition });
	canvas.FillBox(Vector2F{ 200.0f * _pluginState->TextWidthFactor, (10.0f + statNamesAndValues.size() * 15.0f) * _pluginState->TextHeightFactor });

	// Now draw the text on top of it
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	auto counter = 0;
	for (const auto& [statName, value] : statNamesAndValues)
	{
		drawStat(canvas, _pluginState.get(), counter, statName, value);
		counter++;
	}
}
