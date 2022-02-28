#include <pch.h>
#include "StatDisplay.h"

#include <sstream>
#include <iomanip>
#include <cmath>

StatDisplay::StatDisplay(
	const std::shared_ptr<const ShotStats> shotStats,
	const std::shared_ptr<const ShotStats> diffStats,
	const std::shared_ptr<const PluginState> pluginState)
	: _shotStats(shotStats)
	, _diffStats(diffStats)
	, _pluginState(pluginState)
{
}

// Converts e.g. 12.7531 into "12.75". It is recomended to do rounding before calling this
std::string to_percentage_string(double value)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << value;
	return stream.str();
}

std::string to_float_string(float value, int precision = 2)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}

std::string to_diff_percentage_string(double value)
{
	std::ostringstream stream;
	stream << (value >= 0 ? "+" : "") << std::fixed << std::setprecision(2) << value;
	return stream.str();
}

std::string to_diff_value_string(int value)
{
	std::ostringstream stream;
	stream << (value >= 0 ? "+" : "") << value;
	return stream.str();
}
std::string to_diff_value_string(float value)
{
	std::ostringstream stream;
	stream << (value >= 0 ? "+" : "") << to_float_string(value);
	return stream.str();
}
void StatDisplay::drawCenter(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const std::string& label) const
{
	int numCharsLeft = (int)floor((double)label.length() * 0.5);
	auto leftTextBorder = (float)displayOpts.OverlayXPosition + ((_displayWidth / 2.0f) - (7.0f * numCharsLeft)) * displayOpts.TextWidthFactor;
	auto topTextBorder = (float)displayOpts.OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * displayOpts.TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(label, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
}

// Draws a stat consisting of a label and a string value into one "row"
void drawStat(CanvasWrapper& canvas, const DisplayOptions& displayOpts, int rowNumber, const SingleStatStrings& statStrings, float diffDataBorder)
{
	auto leftTextBorder = (float)displayOpts.OverlayXPosition + 5.0f * displayOpts.TextWidthFactor;
	auto topTextBorder = (float)displayOpts.OverlayYPosition + (5.0f + (float)rowNumber * 15.0f) * displayOpts.TextHeightFactor;

	canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
	canvas.DrawString(statStrings.Label, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
	canvas.SetPosition(Vector2F{ leftTextBorder + 140.0f * displayOpts.TextWidthFactor, topTextBorder });
	canvas.DrawString(statStrings.Value, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
	canvas.SetPosition(Vector2F{ leftTextBorder + 190.0f * displayOpts.TextWidthFactor, topTextBorder });
	canvas.DrawString(statStrings.Unit, displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);

	if (statStrings.DiffValue.has_value())
	{
		auto currentColor = canvas.GetColor();
		LinearColor diffColor;
		diffColor.B = .0;
		diffColor.A = 255.0;
		if (std::stod(statStrings.DiffValue.value()) >= .0)
		{
			diffColor.R = .0;
			diffColor.G = 255.0;
		}
		else
		{
			diffColor.R = 255.0;
			diffColor.G = .0;
		}
		canvas.SetColor(diffColor);
		canvas.SetPosition(Vector2F{ displayOpts.OverlayXPosition + diffDataBorder * displayOpts.TextWidthFactor, topTextBorder });
		canvas.DrawString(statStrings.DiffValue.value(), displayOpts.TextWidthFactor, displayOpts.TextHeightFactor, false);
		canvas.SetColor(currentColor);
	}
}

std::list<SingleStatStrings> StatDisplay::GetStatsToBeRendered(const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
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
	}
	if (pluginState->CurrentStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Current Goal Streak:", std::to_string(statsData.Stats.GoalStreakCounter), "" });
		statNamesAndValues.emplace_back(SingleStatStrings{ "Current Miss Streak:", std::to_string(statsData.Stats.MissStreakCounter), "" });
	}
	if (pluginState->LongestStreaksShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Longest Goal Streak:", std::to_string(statsData.Stats.LongestGoalStreak), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.LongestGoalStreak);
		}
		statNamesAndValues.emplace_back(SingleStatStrings{ "Longest Miss Streak:", std::to_string(statsData.Stats.LongestMissStreak), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.LongestMissStreak);
		}
	}
	if (pluginState->AirDribbleTouchesShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Max. Air Dribbles:", std::to_string(statsData.Stats.MaxAirDribbleTouches), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.MaxAirDribbleTouches);
		}
	}
	if (pluginState->AirDribbleTimeShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Max. ADribble Time:", to_float_string(statsData.Stats.MaxAirDribbleTime), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.MaxAirDribbleTime);
		}
	}
	if (pluginState->GroundDribbleTimeShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Max. GDribble Time:", to_float_string(statsData.Stats.MaxGroundDribbleTime), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.MaxGroundDribbleTime);
		}
	}
	if (pluginState->TotalFlipResetsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Total Flip Resets:", std::to_string(statsData.Stats.TotalFlipResets), "" });
		if (diffData)
		{
			//statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.TotalFlipResets);
		}
	}
	if (pluginState->MaxFlipResetsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Max. Flip Resets:", std::to_string(statsData.Stats.MaxFlipResets), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.MaxFlipResets);
		}
	}
	if (pluginState->DoubleTapGoalsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Double Tap Goals:", std::to_string(statsData.Stats.DoubleTapGoals), "" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.DoubleTapGoals);
		}
	}
	if (pluginState->CloseMissesShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Close Misses:", std::to_string(statsData.Stats.CloseMisses), "" });
	}


	// Calculated percentages and averages
	if (pluginState->TotalSuccessRateShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Total Success Rate:", to_percentage_string(statsData.Data.SuccessPercentage), "%" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_percentage_string(diffData->Data.SuccessPercentage);
		}
	}
	if (pluginState->InitialBallHitsShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Initial Hit Rate:", to_percentage_string(statsData.Data.InitialHitPercentage), "%" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_percentage_string(diffData->Data.InitialHitPercentage);
		}
	}
	if (pluginState->LastNShotPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Last 50 Shots", to_percentage_string(statsData.Data.Last50ShotsPercentage), "%" });
	}
	if (pluginState->PeakInfoShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Peak Success Rate:", to_percentage_string(statsData.Data.PeakSuccessPercentage), "%" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_percentage_string(diffData->Data.PeakSuccessPercentage);
		}
		statNamesAndValues.emplace_back(SingleStatStrings{ "Peak At Shot#:", std::to_string(statsData.Data.PeakShotNumber), "" });
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
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MaxValue);
		}
	}
	if (pluginState->MinGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Min Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMin(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MinValue);
		}
	}
	if (pluginState->MedianGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Median Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMedian(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MedianValue);
		}
	}
	if (pluginState->MeanGoalSpeedShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Mean Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats.getMean(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MeanValue);
		}
	}
	if (pluginState->FlipResetsPerAttemptShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "FResets/Attempt:", to_percentage_string(statsData.Data.AverageFlipResetsPerAttempt), "%" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_percentage_string(diffData->Data.AverageFlipResetsPerAttempt);
		}
	}
	if (pluginState->FlipResetPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "FReset Goal Rate:", to_percentage_string(statsData.Data.FlipResetGoalPercentage), "%" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_percentage_string(diffData->Data.FlipResetGoalPercentage);
		}
	}
	if (pluginState->DoubleTapPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Dbl Tap Goal Rate:", to_percentage_string(statsData.Data.DoubleTapGoalPercentage), "%" });
		if (diffData)
		{
			statNamesAndValues.back().DiffValue = to_diff_percentage_string(diffData->Data.DoubleTapGoalPercentage);
		}
	}
	if (pluginState->CloseMissPercentageShallBeDisplayed)
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "Close Miss Rate:", to_percentage_string(statsData.Data.CloseMissPercentage), "%" });
	}

	if (statNamesAndValues.empty())
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "What did you expect?", "  ;-)", "" });
	}

	return statNamesAndValues;
}

void StatDisplay::renderStatsData(CanvasWrapper& canvas, const DisplayOptions& opts, const StatsData& statsData, const StatsData* const diffData)
{
	auto statsToBeRendered = GetStatsToBeRendered(statsData, _pluginState, diffData);
	bool isDisplayingSpeed = _pluginState->MostRecentGoalSpeedShallBeDisplayed ||
		_pluginState->MaxGoalSpeedShallBeDisplayed ||
		_pluginState->MinGoalSpeedShallBeDisplayed ||
		_pluginState->MedianGoalSpeedShallBeDisplayed ||
		_pluginState->MeanGoalSpeedShallBeDisplayed;

	_displayWidth = 215.0f;
	auto diffDataBorder = _displayWidth + 5.0f;
	if (isDisplayingSpeed)
	{
		_displayWidth += 20.0f;
	}
	if (diffData && _pluginState->PreviousSessionDiffShallBeDisplayed)
	{
		_displayWidth += 60.0f;
		if (isDisplayingSpeed)
		{
			diffDataBorder += 20.0f;
		}
	}

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
		drawStat(canvas, opts, counter, statStrings, diffDataBorder);
		counter++;
	}
}

void StatDisplay::renderAllShotStats(CanvasWrapper& canvas)
{
	if (_pluginState->AllShotStatsShallBeDisplayed)
	{
		auto diffStats = (_pluginState->PreviousSessionDiffShallBeDisplayed && _diffStats->hasAttempts() ? &_diffStats->AllShotStats : nullptr);
		renderStatsData(canvas, _pluginState->AllShotsOpts, _shotStats->AllShotStats, diffStats);
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
			auto diffStats = (_pluginState->PreviousSessionDiffShallBeDisplayed && _diffStats->hasAttempts() ? &_diffStats->PerShotStats[_pluginState->CurrentRoundIndex] : nullptr);
			renderStatsData(canvas, _pluginState->PerShotOpts, statsData, diffStats);
		}
	}
}

void StatDisplay::renderOneFrame(CanvasWrapper& canvas)
{
	renderAllShotStats(canvas);
	renderPerShotStats(canvas);
}