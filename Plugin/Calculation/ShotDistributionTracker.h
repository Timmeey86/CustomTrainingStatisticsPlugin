#pragma once

#include "../Core/AbstractEventReceiver.h"
#include "../Core/IStatDisplay.h"

/** This class tracks where the ball touched the backboard.
 * Unlike the other classes, this one uses a single boolean flag instead of a state machine, since only two states are relevant.
*/
class ShotDistributionTracker : public AbstractEventReceiver, public IStatDisplay
{
public:
	explicit ShotDistributionTracker();

	void onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode) override;
	void onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallGroundHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallCeilingHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) override;

	void renderOneFrame(CanvasWrapper& canvas) override;

private:
	bool _furtherWallHitsShallBeIgnored = false; ///< True while wall hits shall be ignored. This is necessary since rolling the ball up the wall would produce a myriad of hits.
	std::vector<Vector> _shotLocations; ///< Stores the hits against the backboard, or into the goal
};