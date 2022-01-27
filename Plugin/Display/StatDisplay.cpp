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

std::string to_float_string(float value, int precision = 2)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}

void StatDisplay::drawCenter(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const std::string& label) const
{
	int numCharsLeft = floor(label.length() * 0.5);
	auto leftTextBorder = (float)displayOpts.OverlayXPosition + ((_displayWidth / 2.0f) - (7.0f * numCharsLeft)) * displayOpts.TextWidthFactor;
	auto topTextBorder = (float)displayOpts.OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * displayOpts.TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(label, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
}

// Draws a stat consisting of a label and a string value into one "row"
void drawStat(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const SingleStatStrings& statStrings)
{
	auto leftTextBorder = (float)displayOpts.OverlayXPosition + 5.0f * displayOpts.TextWidthFactor;
	auto topTextBorder = (float)displayOpts.OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * displayOpts.TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(statStrings.Label, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
	canvas.SetPosition(Vector2F{ leftTextBorder + 140.0f * displayOpts.TextWidthFactor, topTextBorder });
	canvas.DrawString(statStrings.Value, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
	canvas.SetPosition(Vector2F{ leftTextBorder + 190.0f * displayOpts.TextWidthFactor, topTextBorder });
	canvas.DrawString(statStrings.Unit, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
}

std::list<SingleStatStrings> StatDisplay::GetStatsToBeRendered(const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	std::list<SingleStatStrings> statNamesAndValues;

	if (pluginState->AttemptsAndGoalsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Attempts:", std::to_string(statsData.Stats.Attempts), "" });
		statNamesAndValues.emplace_back(SingleStatStrings{ "Goals:", std::to_string(statsData.Stats.Goals), "" });
	}
	if (pluginState->InitialBallHitsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Initial Hits:", std::to_string(statsData.Stats.InitialHits), "" });
		statNamesAndValues.emplace_back(SingleStatStrings{ "Initial Hit Rate:", to_percentage_string(statsData.Data.InitialHitPercentage), "" });
	}
	if (pluginState->CurrentStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Current Goal Streak:", std::to_string(statsData.Stats.GoalStreakCounter), "" });
		statNamesAndValues.emplace_back(SingleStatStrings{ "Current Miss Streak:", std::to_string(statsData.Stats.MissStreakCounter), "" });
	}
	if (pluginState->TotalSuccessRateShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Total Success Rate:", to_percentage_string(statsData.Data.SuccessPercentage), "" });
	}
	if (pluginState->LongestStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Longest Goal Streak:", std::to_string(statsData.Stats.LongestGoalStreak), "" });
		statNamesAndValues.emplace_back(SingleStatStrings{ "Longest Miss Streak:", std::to_string(statsData.Stats.LongestMissStreak), "" });
	}
	if (pluginState->PeakInfoShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Peak Success Rate:", to_percentage_string(statsData.Data.PeakSuccessPercentage), "" });
		statNamesAndValues.emplace_back(SingleStatStrings{ "Peak At Shot#:", std::to_string(statsData.Data.PeakShotNumber), "" });
	}
	if (pluginState->LastNShotPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Last 50 Shots", to_percentage_string(statsData.Data.Last50ShotsPercentage), "" });
	}

	// Goal speed stats
	std::string speed_units = pluginState->IsMetric ? "km/h" : "mph";
	if (pluginState->MostRecentGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Latest Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMostRecent(pluginState->IsMetric)), speed_units });
	}
	if (pluginState->MaxGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Max Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMax(pluginState->IsMetric)), speed_units });
	}
	if (pluginState->MinGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Min Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMin(pluginState->IsMetric)), speed_units });
	}
	if (pluginState->MedianGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Median Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMedian(pluginState->IsMetric)), speed_units });
	}
	if (pluginState->MeanGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Mean Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMean(pluginState->IsMetric)), speed_units });
	}

	if (statNamesAndValues.empty())
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "What did you expect?", "  ;-)", "" });
	}

	return statNamesAndValues;
}

void StatDisplay::renderStatsData(CanvasWrapper& canvas, const DisplayOptions& opts, const StatsData& statsData)
{
	auto statsToBeRendered = GetStatsToBeRendered(statsData, _pluginState);
	bool isDisplayingSpeed = _pluginState->MostRecentGoalSpeedShallBeDisplayed ||
		_pluginState->MaxGoalSpeedShallBeDisplayed ||
		_pluginState->MinGoalSpeedShallBeDisplayed ||
		_pluginState->MedianGoalSpeedShallBeDisplayed ||
		_pluginState->MeanGoalSpeedShallBeDisplayed;
	
	_displayWidth = isDisplayingSpeed ? 230.0f : 200.0f;

	// Draw a panel so we can read the text on all kinds of maps
	canvas.SetColor(_pluginState->PanelColor);

	canvas.SetPosition(Vector2F{ (float)opts.OverlayXPosition, (float)opts.OverlayYPosition });
	canvas.FillBox(Vector2F{ _displayWidth * opts.TextWidthFactor, (10.0f + (statsToBeRendered.size() + 1) * 15.0f) * opts.TextHeightFactor }); // +1 for title

	// Now draw the text on top of it
	canvas.SetColor(_pluginState->FontColor);

	int counter = 0;
	drawCenter(canvas, opts, counter, opts.Title);
	counter++;
	for (const auto& statStrings : statsToBeRendered)
	{
		drawStat(canvas, opts, counter, statStrings);
		counter++;
	}
}

void StatDisplay::renderAllShotStats(CanvasWrapper& canvas)
{
	if (_pluginState->AllShotStatsShallBeDisplayed)
	{
		renderStatsData(canvas, _pluginState->AllShotsOpts, _shotStats->AllShotStats);
	}
}

void StatDisplay::renderPerShotStats(CanvasWrapper& canvas)
{
	if (_pluginState->PerShotStatsShallBeDisplayed)
	{
		// Check if CurrentRoundIndex has been set and if _statsDataPerShot has been initialized
		if (0 <= _pluginState->CurrentRoundIndex && _pluginState->CurrentRoundIndex < _shotStats->PerShotStats.size())
		{
			const auto& statsData = _shotStats->PerShotStats.at(_pluginState->CurrentRoundIndex);
			renderStatsData(canvas, _pluginState->PerShotOpts, statsData);
		}
	}
}

void StatDisplay::renderOneFrame(CanvasWrapper& canvas)
{
	// Draw the overlay when no menu is open, or at most one menu (the "pause" menu) is open
	// That way we don't clutter the settings, or the match/mode selection screen
	if (_pluginState->MenuStackSize < 2)
	{
		renderAllShotStats(canvas);
		renderPerShotStats(canvas);
	}
}