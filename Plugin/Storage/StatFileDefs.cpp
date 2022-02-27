#include <pch.h>
#include "StatFileDefs.h"


const std::vector<std::string> StatFileDefs::SupportedVersionNumbers = {
	"1.0",
	"1.1"
};
const std::string StatFileDefs::CurrentVersionNumber = "1.1";
const std::string StatFileDefs::Version = "Version";
const std::string StatFileDefs::NumberOfShots = "NumberOfShots";
const std::string StatFileDefs::ShotSeparator = "----------------------------------------------------------";
const std::string StatFileDefs::Attempts = "Attempts";
const std::string StatFileDefs::Goals = "Goals";
const std::string StatFileDefs::InitialHits = "InitialHits";
const std::string StatFileDefs::InitialHitPercentage = "InitialHitPercentage";
const std::string StatFileDefs::CurrentGoalStreak = "CurrentGoalStreak";
const std::string StatFileDefs::CurrentMissStreak = "CurrentMissStreak";
const std::string StatFileDefs::TotalSuccessRate = "TotalSuccessRate";
const std::string StatFileDefs::LongestGoalStreak = "LongestGoalStreak";
const std::string StatFileDefs::LongestMissStreak = "LongestMissStreak";
const std::string StatFileDefs::PeakSuccessRate = "PeakSuccessRate";
const std::string StatFileDefs::PeakAtShotNumber = "PeakAtShotNumber";
const std::string StatFileDefs::LastNShotsPercentage = "LastNShotsPercentage";
const std::string StatFileDefs::LatestGoalSpeed = "LatestGoalSpeed";
const std::string StatFileDefs::MaxGoalSpeed = "MaxGoalSpeed";
const std::string StatFileDefs::MinGoalSpeed = "MinGoalSpeed";
const std::string StatFileDefs::MedianGoalSpeed = "MedianGoalSpeed";
const std::string StatFileDefs::MeanGoalSpeed = "MeanGoalSpeed";
// V1.1 and beyond
const std::string StatFileDefs::AirDribbleTouches = "AirDribbleTouches";
const std::string StatFileDefs::AirDribbleTime = "AirDribbleTime";
const std::string StatFileDefs::GroundDribbleTime = "GroundDribbleTime";
const std::string StatFileDefs::DoubleTapGoals = "DoubleTapGoals";
const std::string StatFileDefs::DoubleTapPercentage = "DoubleTapPercentage";
const std::string StatFileDefs::MaxFlipResets = "MaxFlipResets";
const std::string StatFileDefs::TotalFlipResets = "TotalFlipResets";
const std::string StatFileDefs::FlipResetsPerAttempt = "FlipResetsPerAttempt";
const std::string StatFileDefs::FlipResetPercentage = "FlipResetPercentage";
const std::string StatFileDefs::CloseMisses = "CloseMisses";
const std::string StatFileDefs::CloseMissPercentage = "CloseMissPercentage";
