#include <pch.h>
#include "ShotDistributionTracker.h"

const auto YThreshold = 4900.0f;
const auto YDrawLocation = 5100.0f;

ShotDistributionTracker::ShotDistributionTracker()
{
}

void ShotDistributionTracker::onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode)
{
	_shotLocations.clear();
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	auto location = ball.GetLocation();
	_shotLocations.emplace_back(Vector{ location.X, YDrawLocation, location.Z });
	_furtherWallHitsShallBeIgnored = false;
}

void ShotDistributionTracker::onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball)
{
	if (_furtherWallHitsShallBeIgnored) { return; }

	auto location = ball.GetLocation();
	if (location.Y > YThreshold)
	{
		_shotLocations.emplace_back(Vector{ location.X, YDrawLocation, location.Z });
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
	canvas.SetColor(LinearColor{ 255.0f, 255.0f, 0.0f, 200.0f });
	canvas.SetPosition(Vector2{});

	for (auto closeMissLocation : _shotLocations)
	{
		auto screenLocation = canvas.Project(closeMissLocation);
		canvas.DrawRect(Vector2{ screenLocation.X - 5, screenLocation.Y - 5, }, Vector2{ screenLocation.X + 5, screenLocation.Y + 5 });
	}
}
