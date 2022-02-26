#include <pch.h>
#include "ShotDistributionTracker.h"

const auto YThreshold = 4900.0f;
const auto YDrawLocation = 5100.0f;
const auto XBracketWidth = 8000 / ShotDistributionTracker::XBrackets;
const auto ZBracketHeight = 4000 / ShotDistributionTracker::ZBrackets;

ShotDistributionTracker::ShotDistributionTracker()
{
}

void ShotDistributionTracker::onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode)
{
	_furtherWallHitsShallBeIgnored = false;

	for (auto x = 0; x < XBrackets; x++)
	{
		for (auto z = 0; z < ZBrackets; z++)
		{
			_heatmapData[x][z] = 0;
		}
	}
	_maximumValue = 0;
}

void ShotDistributionTracker::incrementHeatmapEntry(Vector ballLocation)
{
	// Get the array bracket for the X dimension
	auto xBracket = (int)(ballLocation.X + 4000) / XBracketWidth;
	// Get the array bracket for the Z dimension
	auto zBracket = (int)ballLocation.Z / ZBracketHeight;

	if (xBracket >= 0 && xBracket < XBrackets && zBracket >= 0 && zBracket < ZBrackets)
	{
		auto& newValue = _heatmapData[xBracket][zBracket];
		newValue++;
		if (newValue > _maximumValue)
		{
			_maximumValue = newValue;
		}
	}
}

void ShotDistributionTracker::onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	auto location = ball.GetLocation();
	incrementHeatmapEntry(location);
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	if (_furtherWallHitsShallBeIgnored) { return; }

	auto location = ball.GetLocation();
	if (location.Y > YThreshold)
	{
		incrementHeatmapEntry(location);
		// We get four events for each wall bounce, and we get a multitude when the ball rolls up the wall
		// we are only interested in the first hit in these cases.
		_furtherWallHitsShallBeIgnored = true;
	}
}

void ShotDistributionTracker::onBallGroundHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::onBallCeilingHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit)
{
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::renderOneFrame(CanvasWrapper& canvas)
{
	canvas.SetPosition(Vector2{});

	for (auto x = 0; x < XBrackets; x++)
	{
		for (auto z = 0; z < ZBrackets; z++)
		{
			auto numberOfHitsInBracket = _heatmapData[x][z];

			auto leftBorder = (float)(x * XBracketWidth - 4000);
			auto rightBorder = leftBorder + XBracketWidth;
			auto bottomBorder = (float)z * ZBracketHeight;
			auto topBorder = bottomBorder + ZBracketHeight;


			auto bottomLeft = Vector{ leftBorder, YDrawLocation, bottomBorder };
			auto bottomRight = Vector{ rightBorder, YDrawLocation, bottomBorder };
			auto topRight = Vector{ rightBorder, YDrawLocation, topBorder };
			auto topLeft = Vector{ leftBorder, YDrawLocation, topBorder };

			// If there are zero hits, don't paint it
			// If there is at least one hit, make it blue for one hit, green for the maximum amount of hits, and something inbetween for the remainder
			if (numberOfHitsInBracket > 0)
			{
				auto adjustedMax = _maximumValue - 1;
				auto blueFactor = .0f;
				auto greenFactor = .0f;
				if (adjustedMax == 0)
				{
					blueFactor = 1.0f; 
					greenFactor = 1.0f;
				}
				else
				{
					greenFactor = (float)(numberOfHitsInBracket - 1) / (float)adjustedMax;
					blueFactor = 1.0f - greenFactor;
				}
				auto heatColor = LinearColor{ .0f, 255.0f * greenFactor,  255.0f * blueFactor, 100.0f };

				auto bottomLeftProj = canvas.Project(bottomLeft);
				auto bottomRightProj = canvas.Project(bottomRight);
				auto topRightProj = canvas.Project(topRight);
				auto topLeftProj = canvas.Project(topLeft);
				canvas.SetColor(heatColor);

				// We're using rectangles instead of triangles right now since it triangles unfortunately ignore transparency
				//canvas.FillTriangle(bottomLeftProj, topRightProj, bottomRightProj);
				//canvas.FillTriangle(bottomLeftProj, topLeftProj, topRightProj);

				canvas.DrawRect(bottomLeftProj, topRightProj);
			}
		}
	}

	//canvas.SetColor(LinearColor{ 255.0f, 255.0f, 0.0f, 200.0f });

	//for (auto closeMissLocation : _shotLocations)
	//{
	//	auto screenLocation = canvas.Project(closeMissLocation);
	//	canvas.DrawRect(Vector2{ screenLocation.X - 5, screenLocation.Y - 5, }, Vector2{ screenLocation.X + 5, screenLocation.Y + 5 });
	//}
}
