#include <pch.h>
#include "StatDisplay.h"
#include "version.h"

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

		// Switch to OS locale temporarily in order to convert the value string back to double
		const std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
		std::setlocale(LC_NUMERIC, "");
		double diffValue = std::stod(statStrings.DiffValue.value());
		std::setlocale(LC_NUMERIC, oldLocale.c_str());

		if (diffValue >= .0)
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

void addAttempts(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->AttemptsAndGoalsShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Attempts:", std::to_string(statsData.Stats.Attempts), "" });
	}
}
void addGoals(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->AttemptsAndGoalsShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Goals:", std::to_string(statsData.Stats.Goals), "" });
	}
}
void addInitialBallHits(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->InitialBallHitsShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Initial Hits:", std::to_string(statsData.Stats.InitialHits), "" });
	}
}
void addCurrentGoalStreak(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->CurrentStreaksShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Current Goal Streak:", std::to_string(statsData.Stats.GoalStreakCounter), "" });
	}
}
void addCurrentMissStreak(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->CurrentStreaksShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Current Miss Streak:", std::to_string(statsData.Stats.MissStreakCounter), "" });
	}
}
void addLongestGoalStreak(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->LongestStreaksShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Longest Goal Streak:", std::to_string(statsData.Stats.LongestGoalStreak), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.LongestGoalStreak);
		}
	}
}
void addLongestMissStreak(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->LongestStreaksShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Longest Miss Streak:", std::to_string(statsData.Stats.LongestMissStreak), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.LongestMissStreak);
		}
	}
}
void addAirDribbleTouches(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->AirDribbleTouchesShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Max. Air Dribbles:", std::to_string(statsData.Stats.MaxAirDribbleTouches), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.MaxAirDribbleTouches);
		}
	}
}
void addAirDribbleTime(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->AirDribbleTimeShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Max. ADribble Time:", to_float_string(statsData.Stats.MaxAirDribbleTime), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.MaxAirDribbleTime);
		}
	}
}
void addGroundDribbleTime(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->GroundDribbleTimeShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Max. GDribble Time:", to_float_string(statsData.Stats.MaxGroundDribbleTime), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.MaxGroundDribbleTime);
		}
	}
}
void addTotalFlipResets(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->TotalFlipResetsShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Total Flip Resets:", std::to_string(statsData.Stats.TotalFlipResets), "" });
	}
}
void addMaxFlipResets(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->GroundDribbleTimeShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Max. GDribble Time:", to_float_string(statsData.Stats.MaxGroundDribbleTime), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.MaxGroundDribbleTime);
		}
	}
}
void addDoubleTapGoals(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->DoubleTapGoalsShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Double Tap Goals:", std::to_string(statsData.Stats.DoubleTapGoals), "" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.DoubleTapGoals);
		}
	}
}
void addCloseMisses(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->CloseMissesShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Close Misses:", std::to_string(statsData.Stats.CloseMisses), "" });
	}
}
void addTotalSuccessRate(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->TotalSuccessRateShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Total Success Rate:", to_percentage_string(statsData.Data.SuccessPercentage), "%" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_percentage_string(diffData->Data.SuccessPercentage);
		}
	}
}
void addInitialHitRate(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->InitialBallHitsShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Initial Hit Rate:", to_percentage_string(statsData.Data.InitialHitPercentage), "%" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_percentage_string(diffData->Data.InitialHitPercentage);
		}
	}
}
void addLastNShotPercentage(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->LastNShotPercentageShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Last 50 Shots", to_percentage_string(statsData.Data.Last50ShotsPercentage), "%" });
	}
}
void addPeakSuccessRate(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->PeakInfoShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Peak Success Rate:", to_percentage_string(statsData.Data.PeakSuccessPercentage), "%" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_percentage_string(diffData->Data.PeakSuccessPercentage);
		}
	}
}
void addPeakAtShotNumber(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->PeakInfoShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Peak At Shot#:", std::to_string(statsData.Data.PeakShotNumber), "" });
	}
}

void addLatestGoalSpeed(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const std::string& speed_units)
{
	if (pluginState->MostRecentGoalSpeedShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Latest Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats()->getMostRecent(pluginState->IsMetric)), speed_units });
	}
}
void addMinimumGoalSpeed(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData, const std::string& speed_units)
{
	if (pluginState->MinGoalSpeedShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Min Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats()->getMin(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MinValue);
		}
	}
}
void addMedianGoalSpeed(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData, const std::string& speed_units)
{
	if (pluginState->MedianGoalSpeedShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Median Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats()->getMedian(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MedianValue);
		}
	}
}
void addMaximumGoalSpeed(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData, const std::string& speed_units)
{
	if (pluginState->MaxGoalSpeedShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Max Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats()->getMax(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MaxValue);
		}
	}
}
void addMeanGoalSpeed(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData, const std::string& speed_units)
{
	if (pluginState->MeanGoalSpeedShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Mean Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats()->getMean(pluginState->IsMetric)), speed_units });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_value_string(diffData->Stats.GoalSpeedDifference.MeanValue);
		}
	}
}
void addStdDevGoalSpeed(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const std::string& speed_units)
{
	if (pluginState->StdDevGoalSpeedShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Std Dev Goal Speed:", to_float_string(statsData.Stats.GoalSpeedStats()->getStdDev(pluginState->IsMetric)), speed_units });
	}
}
void addFlipResetsPerAttempt(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->FlipResetsPerAttemptShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "FResets/Attempt:", to_percentage_string(statsData.Data.AverageFlipResetsPerAttempt), "%" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_percentage_string(diffData->Data.AverageFlipResetsPerAttempt);
		}
	}
}
void addFlipResetPercentage(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->FlipResetPercentageShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "FReset Goal Rate:", to_percentage_string(statsData.Data.FlipResetGoalPercentage), "%" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_percentage_string(diffData->Data.FlipResetGoalPercentage);
		}
	}
}
void addDoubleTapPercentage(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	if (pluginState->DoubleTapPercentageShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Dbl Tap Goal Rate:", to_percentage_string(statsData.Data.DoubleTapGoalPercentage), "%" });
		if (diffData)
		{
			statList.back().DiffValue = to_diff_percentage_string(diffData->Data.DoubleTapGoalPercentage);
		}
	}
}
void addCloseMissRate(std::list<SingleStatStrings>& statList, const StatsData& statsData, const std::shared_ptr<const PluginState> pluginState)
{
	if (pluginState->CloseMissPercentageShallBeDisplayed)
	{
		statList.emplace_back(SingleStatStrings{ "Close Miss Rate:", to_percentage_string(statsData.Data.CloseMissPercentage), "%" });
	}
}

std::list<SingleStatStrings> StatDisplay::GetStatsToBeRendered(StatsData statsData, const std::shared_ptr<const PluginState> pluginState, const StatsData* const diffData)
{
	std::list<SingleStatStrings> statNamesAndValues;

	// Apply the number format used by the OS (some regions, especially in europe write 3,47 instead of 3.47)
	std::locale::global(std::locale(""));
	setlocale(LC_ALL, ""); // uses the OS locale

	// Copy the settings vector to be thread safe
	std::vector<std::string> OrderedStats;
	{
		std::scoped_lock lock(GoalPercentageCounterSettings::OrderedStatsMutex);
		OrderedStats = std::vector<std::string>(GoalPercentageCounterSettings::OrderedStatsNames);
	}
	if (OrderedStats.empty()) { return statNamesAndValues; }

	std::string speed_units = pluginState->IsMetric ? "km/h" : "mph";
	for (const auto& setting : OrderedStats)
	{
		if (setting == GoalPercentageCounterSettings::DisplayAttemptsAndGoalsDef.DisplayText)
		{
			addAttempts(statNamesAndValues, statsData, pluginState);
			addGoals(statNamesAndValues, statsData, pluginState);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayInitialBallHitsDef.DisplayText)
		{
			addInitialBallHits(statNamesAndValues, statsData, pluginState);
			addInitialHitRate(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayCurrentStreaksDef.DisplayText)
		{
			addCurrentGoalStreak(statNamesAndValues, statsData, pluginState);
			addCurrentMissStreak(statNamesAndValues, statsData, pluginState);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayLongestStreaksDef.DisplayText)
		{
			addLongestGoalStreak(statNamesAndValues, statsData, pluginState, diffData);
			addLongestMissStreak(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayAirDribbleTouchesDef.DisplayText)
		{
			addAirDribbleTouches(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayAirDribbleTimeDef.DisplayText)
		{
			addAirDribbleTime(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayGroundDribbleDef.DisplayText)
		{
			addGroundDribbleTime(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayTotalFlipResetsDef.DisplayText)
		{
			addTotalFlipResets(statNamesAndValues, statsData, pluginState);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayMaxFlipResetsDef.DisplayText)
		{
			addMaxFlipResets(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayDoubleTapGoalsDef.DisplayText)
		{
			addDoubleTapGoals(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayCloseMissesDef.DisplayText)
		{
			addCloseMisses(statNamesAndValues, statsData, pluginState);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayTotalSuccessRateDef.DisplayText)
		{
			addTotalSuccessRate(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayLastNShotPercentageDef.DisplayText)
		{
			addLastNShotPercentage(statNamesAndValues, statsData, pluginState);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayPeakInfoDef.DisplayText)
		{
			addPeakSuccessRate(statNamesAndValues, statsData, pluginState, diffData);
			addPeakAtShotNumber(statNamesAndValues, statsData, pluginState);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayMostRecentGoalSpeedDef.DisplayText)
		{
			addLatestGoalSpeed(statNamesAndValues, statsData, pluginState, speed_units);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayMinGoalSpeedDef.DisplayText)
		{
			addMinimumGoalSpeed(statNamesAndValues, statsData, pluginState, diffData, speed_units);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayMedianGoalSpeedDef.DisplayText)
		{
			addMedianGoalSpeed(statNamesAndValues, statsData, pluginState, diffData, speed_units);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayMaxGoalSpeedDef.DisplayText)
		{
			addMaximumGoalSpeed(statNamesAndValues, statsData, pluginState, diffData, speed_units);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayMeanGoalSpeedDef.DisplayText)
		{
			addMeanGoalSpeed(statNamesAndValues, statsData, pluginState, diffData, speed_units);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayStdDevGoalSpeedDef.DisplayText)
		{
			addStdDevGoalSpeed(statNamesAndValues, statsData, pluginState, speed_units);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayFlipResetsPerAttemptDef.DisplayText)
		{
			addFlipResetsPerAttempt(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayFlipResetPercentageDef.DisplayText)
		{
			addFlipResetPercentage(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayDoubleTapPercentageDef.DisplayText)
		{
			addDoubleTapPercentage(statNamesAndValues, statsData, pluginState, diffData);
		}
		else if (setting == GoalPercentageCounterSettings::DisplayCloseMissPercentageDef.DisplayText)
		{
			addCloseMissRate(statNamesAndValues, statsData, pluginState);
		}
	}

	if (statNamesAndValues.empty())
	{
		statNamesAndValues.emplace_back(SingleStatStrings{ "What did you expect?", "  ;-)", "" });
	}
	return statNamesAndValues;
}

void StatDisplay::renderStatsData(CanvasWrapper& canvas, const DisplayOptions& opts, const StatsData& statsData, const StatsData* const diffData, bool brandingShallBeDrawn)
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

	auto displayHeight = (10.0f + (statsToBeRendered.size() + 1) * 15.0f);
	if (brandingShallBeDrawn)
	{
		displayHeight += 15.0f;
	}
	displayHeight *= opts.TextHeightFactor;

	// Draw a panel so we can read the text on all kinds of maps
	canvas.SetColor(_pluginState->PanelColor);

	canvas.SetPosition(Vector2F{ (float)opts.OverlayXPosition, (float)opts.OverlayYPosition });
	canvas.FillBox(Vector2F{ _displayWidth * opts.TextWidthFactor, displayHeight }); // +1 for title

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

	if (brandingShallBeDrawn)
	{
		// Draw branding info
		canvas.SetColor(_pluginState->FontColor);
		auto leftTextBorder = (float)opts.OverlayXPosition + 5.0f * opts.TextWidthFactor;
		auto topTextBorder = (float)opts.OverlayYPosition + displayHeight - 15.0f * opts.TextHeightFactor;

		canvas.SetPosition(Vector2F{ leftTextBorder, topTextBorder });
		canvas.DrawString(
			fmt::format("Custom Training Statistics v{}.{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD),
			opts.TextWidthFactor,
			opts.TextHeightFactor,
			false
		);
	}
}

void StatDisplay::renderAllShotStats(CanvasWrapper& canvas)
{
	if (_pluginState->AllShotStatsShallBeDisplayed)
	{
		auto diffStats = (_pluginState->PreviousSessionDiffShallBeDisplayed && _diffStats->hasAttempts() ? &_diffStats->AllShotStats : nullptr);
		renderStatsData(canvas, _pluginState->AllShotsOpts, _shotStats->AllShotStats, diffStats, true);
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
			renderStatsData(canvas, _pluginState->PerShotOpts, statsData, diffStats, !_pluginState->AllShotStatsShallBeDisplayed);
		}
	}
}

void StatDisplay::renderOneFrame(CanvasWrapper& canvas)
{
	renderAllShotStats(canvas);
	renderPerShotStats(canvas);
}