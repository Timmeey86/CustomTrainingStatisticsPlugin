#pragma once

#include <string>
#include <memory>

#include <bakkesmod/wrappers/GameWrapper.h>

/** Defines strings to be used within the stat files. */
class StatFileDefs
{
public:
	static const std::vector<std::string> SupportedVersionNumbers;
	static const std::string CurrentVersionNumber;

	static const std::string Version;

	static const std::string NumberOfShots;
	static const std::string ShotSeparator;

	static const std::string Attempts;
	static const std::string Goals;
	static const std::string InitialHits;
	static const std::string InitialHitPercentage;
	static const std::string CurrentGoalStreak;
	static const std::string CurrentMissStreak;
	static const std::string TotalSuccessRate;
	static const std::string LongestGoalStreak;
	static const std::string LongestMissStreak;
	static const std::string PeakSuccessRate;
	static const std::string PeakAtShotNumber;
	static const std::string LastNShotsPercentage;
	static const std::string LatestGoalSpeed;
	static const std::string MaxGoalSpeed;
	static const std::string MinGoalSpeed;
	static const std::string MedianGoalSpeed;
	static const std::string MeanGoalSpeed; 
	
	static const std::string AirDribbleTouches;
	static const std::string AirDribbleTime;
	static const std::string GroundDribbleTime;
	static const std::string DoubleTapGoals;
	static const std::string DoubleTapPercentage;
	static const std::string MaxFlipResets;
	static const std::string TotalFlipResets;
	static const std::string FlipResetsPerAttempt;
	static const std::string FlipResetPercentage;
	static const std::string CloseMisses;
	static const std::string CloseMissPercentage;

	static const std::string ImpactLocations;

	static const std::string GoalSpeedValues;



	/** Retrieves the path to the training pack data folder. */
	static std::string getTrainingFolder(const std::shared_ptr<GameWrapper>& gameWrapper, const std::string& trainingPackCode);
};