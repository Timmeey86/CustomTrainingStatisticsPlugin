#include <pch.h>

#include <CBRenderingTools/Objects/Frustum.h>

#include "ShotDistributionTracker.h"
#include "../Data/TriggerNames.h"

const auto YThreshold = 4900.0f;
const auto YDrawLocation = 5100.0f;
const auto XBracketWidth = 8000 / ShotDistributionTracker::XBrackets;
const auto ZBracketHeight = 4000 / ShotDistributionTracker::ZBrackets;


ShotDistributionTracker::ShotDistributionTracker(std::shared_ptr<GameWrapper> gameWrapper)
	: _gameWrapper(gameWrapper)
{
}

void ShotDistributionTracker::registerNotifiers(std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	// Toggle display of the heatmap of goals / backboard bounces
	cvarManager->registerNotifier(TriggerNames::ToggleHeatmapDisplay, [this](const std::vector<std::string>&) {
		_heatMapIsVisible = !_heatMapIsVisible;
	}, "Toggle display of a heatmap of goal / backboard bounce locations", PERMISSION_ALL);

	cvarManager->registerNotifier(TriggerNames::ToggleImpactLocationDisplay, [this](const std::vector<std::string>&) {
		_shotLocationsAreVisible = !_shotLocationsAreVisible;
	}, "Toggle display of a impact locations of goals / backboard bounces", PERMISSION_ALL);
}
void ShotDistributionTracker::onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, TrainingEditorSaveDataWrapper* trainingData)
{
	_furtherWallHitsShallBeIgnored = false;

	for (auto x = 0; x < XBrackets; x++)
	{
		for (auto z = 0; z < ZBrackets; z++)
		{
			_heatmapData[x][z] = .0f;
		}
	}
	_maximumValue = .0f;
	_shotLocations.clear();
}

void ShotDistributionTracker::registerImpactLocation(Vector ballLocation)
{
	_shotLocations.emplace_back(Vector(ballLocation.X, YDrawLocation - 10.0f, ballLocation.Z));

	// Get the array bracket for the X dimension
	auto xBracket = (int)(ballLocation.X + 4000) / XBracketWidth;
	// Get the array bracket for the Z dimension
	auto zBracket = (int)ballLocation.Z / ZBracketHeight;

	for (auto x = xBracket - 5; x < XBrackets && x <= xBracket + 5; x++)
	{
		auto xDifference = abs((float)(x - xBracket));
		auto xMagnitude = 0.5f / pow(2.0f, xDifference); 

		for (auto z = zBracket - 5; z < ZBrackets && z <= zBracket + 5; z++)
		{
			auto zDifference = abs((float)(z - zBracket));
			auto zMagnitude = 0.5f / pow(2.0f, zDifference);

			if (x >= 0 && z >= 0)
			{
				auto& newValue = _heatmapData[x][z];
				newValue += (xMagnitude + zMagnitude);
				if (newValue > _maximumValue)
				{
					_maximumValue = newValue;
				}
			}
		}
	}
}

void ShotDistributionTracker::onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	auto location = ball.GetLocation();
	registerImpactLocation(location);
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	if (_furtherWallHitsShallBeIgnored) { return; }

	auto location = ball.GetLocation();
	if (location.Y > YThreshold)
	{
		registerImpactLocation(location);
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

	if (_heatMapIsVisible)
	{
		renderHeatMap(canvas);
	}

	if (_shotLocationsAreVisible)
	{
		renderShotLocations(canvas);
	}
}

void drawRect(float left, float bottom, float width, float height, float y, const CameraWrapper& camera, CanvasWrapper& canvas)
{
	auto bottomLeft = Vector{ left, y, bottom };
	auto topRight = Vector{ left + width, y, bottom + height };

	// Skip drawing if either of the points is not in the currently visible area
	auto currentCameraFrustum = RT::Frustum(canvas, camera);
	if (!currentCameraFrustum.IsInFrustum(bottomLeft) || !currentCameraFrustum.IsInFrustum(topRight))
	{
		return;
	}
	auto bottomLeftProj = canvas.Project(bottomLeft);
	auto topRightProj = canvas.Project(topRight);

	// Unfortunately we can't use triangles since these will ignore the alpha channel
	canvas.DrawRect(bottomLeftProj, topRightProj);
}

void ShotDistributionTracker::renderShotLocations(CanvasWrapper& canvas)
{
	canvas.SetColor(LinearColor{ 255.0f, 0.0f, 255.0f, 200.0f });

	for (auto closeMissLocation : _shotLocations)
	{
		drawRect(closeMissLocation.X - 15.0f, closeMissLocation.Z - 15.0f, 30.0f, 30.0f, YDrawLocation - 5.0f, _gameWrapper->GetCamera(), canvas);
	}
}

void ShotDistributionTracker::renderHeatMap(CanvasWrapper& canvas)
{
	for (auto x = 0; x < XBrackets; x++)
	{
		for (auto z = 0; z < ZBrackets; z++)
		{
			auto numberOfHitsInBracket = _heatmapData[x][z];

			// If there are zero hits, don't paint it
			if (numberOfHitsInBracket > 0)
			{
				canvas.SetColor(getHeatmapColor(numberOfHitsInBracket));
				drawRectangle(x, z, canvas);
			}
		}
	}
}

void ShotDistributionTracker::drawRectangle(int x, int z, CanvasWrapper& canvas)
{
	auto leftBorder = (float)(x * XBracketWidth - 4000);
	auto bottomBorder = (float)z * ZBracketHeight;

	drawRect(leftBorder, bottomBorder, XBracketWidth, ZBracketHeight, YDrawLocation, _gameWrapper->GetCamera(), canvas);
}

LinearColor ShotDistributionTracker::getHeatmapColor(float numberOfHitsInBracket)
{
	// calculate the color like this:
	// 0% of max value: black
	// 20% of max value: pure Blue
	// 40% of max value: blue + green (cyan)
	// 60% of max value: pure green
	// 80% of max value: green + red (yellow)
	// 100% of max value: pure red

	auto percentageOfMaximum = numberOfHitsInBracket / _maximumValue;

	// Red: 0%-60% = 0, 80%-100% = 1
	// Green: 0-20% = 0, 40-60% = 1, 80-100% = 0
	// Blue: 0% = 0, 20-40% = 1, 60-100% = 0
	float redFactor, greenFactor, blueFactor;
	if (percentageOfMaximum <= .2f)
	{
		redFactor = .0f;
		greenFactor = .0f;
		blueFactor = percentageOfMaximum * 5.0f; // Scale to 0-100
	}
	else if (percentageOfMaximum <= .4f)
	{
		redFactor = .0f;
		greenFactor = (percentageOfMaximum - .2f) * 5.0f;
		blueFactor = 1.0f;
	}
	else if (percentageOfMaximum <= .6f)
	{
		redFactor = .0f;
		greenFactor = 1.0f;
		blueFactor = 1.0f - ((percentageOfMaximum - .4f) * 5.0f);
	}
	else if (percentageOfMaximum <= .8f)
	{
		redFactor = (percentageOfMaximum - .6f) * 5.0f;
		greenFactor = 1.0f;
		blueFactor = .0f;
	}
	else
	{
		redFactor = 1.0f;
		greenFactor = 1.0f - ((percentageOfMaximum - .8f) * 5.0f);
		blueFactor = .0f;
	}
	return LinearColor{ 255.0f * redFactor, 255.0f * greenFactor, 255.0f * blueFactor, 50.0f + percentageOfMaximum * 200.0f };
}
