#include <pch.h>
#include "StatDisplay.h"

#include <sstream>
#include <iomanip>

StatDisplay::StatDisplay(std::shared_ptr<PlayerStats> playerStats, std::shared_ptr<CalculatedData> calculatedData)
	: _playerStats( playerStats )
	, _calculatedData( calculatedData )
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

// Draws a stat consisting of a label and an integer value into one "row"
void drawIntStat(CanvasWrapper canvas, float yOffset, const std::string& label, int value)
{
	drawStat(canvas, yOffset, label, std::to_string(value));
}

// Draws a stat consisting of a label and a double value which represents a percentag into one "row"
void drawPercentageStat(CanvasWrapper canvas, float yOffset, const std::string& label, double percentageValue)
{
	drawStat(canvas, yOffset, label, to_percentage_string(percentageValue));
}

void StatDisplay::renderOneFrame(CanvasWrapper canvas)
{
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

	drawIntStat(canvas, 200.0, "Attempts:", _playerStats->Attempts);
	drawIntStat(canvas, 220.0, "Goals:", _playerStats->Goals);
	drawIntStat(canvas, 240.0, "Current Goal Streak:", _playerStats->GoalStreakCounter);
	drawIntStat(canvas, 260.0, "Current Miss Streak:", _playerStats->MissStreakCounter);
	drawPercentageStat(canvas, 280.0, "Total Success Rate:", _calculatedData->SuccessPercentage);
	drawIntStat(canvas, 300.0, "Longest Goal Streak:", _playerStats->LongestGoalStreak);
	drawIntStat(canvas, 320.0, "Longest Miss Streak:", _playerStats->LongestMissStreak);
	drawPercentageStat(canvas, 340.0, "Peak Success Rate:", _calculatedData->PeakSuccessPercentage);
	drawIntStat(canvas, 360.0, "Peak At Shot#:", _calculatedData->PeakShotNumber);
	drawPercentageStat(canvas, 380.0, "Last 50 Shots:", _calculatedData->Last50ShotsPercentage);
}
