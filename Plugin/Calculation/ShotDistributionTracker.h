#pragma once

#include <array>
#include <vector>

#include "../Core/AbstractEventReceiver.h"
#include "../Core/IStatDisplay.h"

#include <bakkesmod/wrappers/wrapperstructs.h>

/** This class tracks where the ball touched the backboard.
 * Unlike the other classes, this one uses a single boolean flag instead of a state machine, since only two states are relevant.
*/
class ShotDistributionTracker : public AbstractEventReceiver, public IStatDisplay
{

public:
	/** Creates a new object which keeps track of locations on the backboard or goal surface which were hit by the ball. */
	explicit ShotDistributionTracker();

	// Resets the tracked shot when a new pack gets loaded
	void onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode) override;
	// Remembers the location of the goal which was hit and triggers a heatmap update
	void onGoalScored(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	// Remembers the location of the first wall hit (until the ball touches the ground or ceiling or car)
	void onBallWallHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	// The following three reset the flag which prevents further wall touches from being processed
	void onBallGroundHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallCeilingHit(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball) override;
	void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) override;

	// Renders the heat map and/or shot location overlays
	void renderOneFrame(CanvasWrapper& canvas) override;

	
	static const int XBrackets = 160; ///< Defines the number of brackets in X dimension. The number 8000 should be dividable by this number.
	static const int ZBrackets = 80; ///< Defines the number of brackets in Z dimension. The number 4000 should be dividable by this number.

	/** Shows or hides the heat map. */
	inline void setHeatMapVisible(bool visible) { _heatMapIsVisible = visible; }
	/** Shows or hides the shot locations. */
	inline void setShotLocationsVisible(bool visible) { _shotLocationsAreVisible = visible; }

private:
	/** Increments the heat map entry at the shot location by 1 and slightly increments brackets around that location. */
	void incrementHeatmapEntry(Vector ballLocation);
	/** Draws the rectangle for the given cell. */
	void drawRectangle(int x, int z, CanvasWrapper& canvas);
	/** Retrieves the heatmap color for the given number of hits, in relation to the maximum value of all brackets. */
	LinearColor getHeatmapColor(float numberOfHitsInBracket);
	/** Renders the locations of actual impact points. */
	void renderShotLocations(CanvasWrapper& canvas);
	/** Renders the heat map. */
	void renderHeatMap(CanvasWrapper& canvas);

	std::array<std::array<float, ZBrackets>, XBrackets> _heatmapData; ///< Stores the number of hits in each cell
	float _maximumValue = 0; ///< The maximum value of all brackets
	bool _heatMapIsVisible = false; ///< Used for showing or hiding the heat map.

	std::vector<Vector> _shotLocations; ///< Stores the locations of goals/bounces of the current training pack.
	bool _shotLocationsAreVisible = false; ///< Used for showing or hiding the shot locations

	bool _furtherWallHitsShallBeIgnored = false; ///< True while wall hits shall be ignored. This is necessary since rolling the ball up the wall would produce a myriad of hits.
};