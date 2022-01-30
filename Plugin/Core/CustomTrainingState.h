#pragma once

#include "../DLLImportExport.h"

/** This class defines the states the custom training can be in (at least the ones we worry about). 

	The states are described in img/CustomTrainingStateChart.png
*/
enum class CustomTrainingState
{
	NotInCustomTraining, ///< Active whenever the player is not in custom training
	Resetting, ///< Gets entered whenever OnTrainingModeLoaded is triggered (from any source state)
	/** Gets entered whenever EventRoundChanged is triggered.
	* 
	* There are currently three cases how this can happen:
	* - EventRoundChanged got sent automatically while in the "Resetting" state (We ignore the event which gets sent before OnTrainingModeLoaded)
	* - The player pressed the buttons which switch to a different shot while in PreparingNewShot state, before starting an attempt
	* - The player pressed reset shot, switch to a different shot, or let the timer run out (with an optional goal/miss replay).
	*   In this case the "ProcessingMiss" or "ProcessingGoal" states will automatically transition back to PreparingNewShot.
	*/
	PreparingNewShot, 
	AttemptInProgress, ///< Gets entered when TrainingShotAttempt gets sent, with PreparingNewShot being the source state.
	ProcessingMiss, ///< Gets entered when EventRoundChanged was received in AttemptInProgress, and no goal had been recorded
	ProcessingGoal, ///< Gets entered when EventRoundChanged was received in AttemptInProgress, and a goal had been recorded
};

inline std::string to_string(CustomTrainingState state)
{
	switch (state)
	{
	case CustomTrainingState::AttemptInProgress:
		return "AttemptInProgress";
	case CustomTrainingState::NotInCustomTraining:
		return "NotInCustomTraining";
	case CustomTrainingState::PreparingNewShot:
		return "PreparingNewShot";
	case CustomTrainingState::ProcessingGoal:
		return "ProcessingGoal";
	case CustomTrainingState::ProcessingMiss:
		return "ProcessingMiss";
	case CustomTrainingState::Resetting:
		return "Resetting";
	default:
		return "UNKNOWN STATE";
	}
}