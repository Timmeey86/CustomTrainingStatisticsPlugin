#include <pch.h>
#include "StatDisplay.h"

#include <sstream>
#include <iomanip>
#include <cmath>

StatDisplay::StatDisplay(
	const std::shared_ptr<const ShotStats> shotStats,
	const std::shared_ptr<const PluginState> pluginState)
		: _shotStats(shotStats)
		, _pluginState(pluginState)
{
}

// Converts e.g. 12.7531 into "12.75". It is recomended to do rounding before calling this
std::string to_percentage_string(double value)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << value << "%";
	return stream.str();
}

void drawCenter(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const std::string& label)
{
	int numCharsLeft = floor(label.length() * 0.5);
	auto leftTextBorder = (float)displayOpts.OverlayXPosition + (100.0f - 7.0f * numCharsLeft) * displayOpts.TextWidthFactor;
	auto topTextBorder = (float)displayOpts.OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * displayOpts.TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(label, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
}

// Draws a stat consisting of a label and a string value into one "row"
void drawStat(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const std::string& label, const std::string& value)
{
	auto leftTextBorder = (float)displayOpts.OverlayXPosition + 5.0f * displayOpts.TextWidthFactor;
	auto topTextBorder = (float)displayOpts.OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * displayOpts.TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(label, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
	canvas.SetPosition(Vector2F{ leftTextBorder + 140.0f * displayOpts.TextWidthFactor, topTextBorder });
	canvas.DrawString(value, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
}

std::list<std::pair<std::string, std::string>> StatDisplay::getStatsToBeRendered(const std::shared_ptr<const StatsData> statsData) const
{
	std::list<std::pair<std::string, std::string>> statNamesAndValues;

	auto goalName = std::string{ _pluginState->TrackInitialBallHitInsteadOfGoal ? "Hit" : "Goal" };

	if (_pluginState->AttemptsAndGoalsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Attempts:", std::to_string(statsData->Stats->Attempts ));
		statNamesAndValues.emplace_back(goalName + "s:", std::to_string(statsData->Stats->Goals));
	}
	if (_pluginState->CurrentStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Current " + goalName + " Streak:", std::to_string(statsData->Stats->GoalStreakCounter));
		statNamesAndValues.emplace_back("Current Miss Streak:", std::to_string(statsData->Stats->MissStreakCounter));
	}
	if (_pluginState->TotalSuccessRateShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Total Success Rate:", to_percentage_string(statsData->Data->SuccessPercentage));
	}
	if (_pluginState->LongestStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Longest " + goalName + " Streak:", std::to_string(statsData->Stats->LongestGoalStreak));
		statNamesAndValues.emplace_back("Longest Miss Streak:", std::to_string(statsData->Stats->LongestMissStreak));
	}
	if (_pluginState->PeakInfoShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Peak Success Rate:", to_percentage_string(statsData->Data->PeakSuccessPercentage));
		statNamesAndValues.emplace_back("Peak At Shot#:", std::to_string(statsData->Data->PeakShotNumber));
	}
	if (_pluginState->LastNShotPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back("Last 50 Shots", to_percentage_string(statsData->Data->Last50ShotsPercentage));
	}

	if (statNamesAndValues.empty())
	{
		statNamesAndValues.emplace_back("What did you expect?", "  ;-)");
	}

	return statNamesAndValues;
}

void StatDisplay::render(CanvasWrapper& canvas, const DisplayOptions& opts, const std::shared_ptr<const StatsData> statsData) const
{
	auto statNamesAndValues = getStatsToBeRendered(statsData);

	// Draw a panel so we can read the text on all kinds of maps
	LinearColor colors;
	colors.R = 100;
	colors.G = 100;
	colors.B = 100;
	colors.A = 200;
	canvas.SetColor(colors);

	canvas.SetPosition(Vector2F{ (float)opts.OverlayXPosition, (float)opts.OverlayYPosition });
	canvas.FillBox(Vector2F{ 200.0f * opts.TextWidthFactor, (10.0f + (statNamesAndValues.size() + 1) * 15.0f) * opts.TextHeightFactor }); // +1 for title

	// Now draw the text on top of it
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	int counter = 0;
	drawCenter(canvas, opts, counter, opts.Title);
	counter++;
	for (const auto& [statName, value] : statNamesAndValues)
	{
		drawStat(canvas, opts, counter, statName, value);
		counter++;
	}
}

void StatDisplay::renderAllShotStats(CanvasWrapper& canvas) const
{
	if (_pluginState->AllShotStatsShallBeDisplayed)
	{
		render(canvas, _pluginState->AllShotsOpts, _shotStats->AllShotStats);
	}
}

void StatDisplay::renderPerShotStats(CanvasWrapper& canvas) const
{
	if (_pluginState->PerShotStatsShallBeDisplayed)
	{
		// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
		if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _shotStats->PerShotStats->size())
		{
			const auto& statsData = _shotStats->PerShotStats->at(_pluginState->CurrentRoundIndex);
			render(canvas, _pluginState->PerShotOpts, statsData);
		}
	}
}

void StatDisplay::renderOneFrame(CanvasWrapper& canvas) const
{
	renderAllShotStats(canvas);
	renderPerShotStats(canvas);
}