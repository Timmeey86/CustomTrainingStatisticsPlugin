#pragma once

#include <memory>

#include <bakkesmod/plugin/bakkesmodsdk.h>
#include <bakkesmod/wrappers/GameWrapper.h>
#include <bakkesmod/wrappers/cvarmanagerwrapper.h>

#include "../DLLImportExport.h"
#include "../Data/PluginState.h"
#include "IStatUpdater.h"
#include "IStatWriter.h"
#include "CustomTrainingState.h"

/** This class is responsible for progressing to the appropriate follow-up states in case of events.
 * The goal is to have anything related to the current state in this class, while keeping all others free of it.
 *
 * The state machine will ignore any event which is not relevant for the currently active state.
 *
 * States and transitions are modelled in img/CustomTrainingStateChart.png
 */
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT CustomTrainingStateMachine
{
public:
	/** Creates a new state machine for the given stat updater. */
	CustomTrainingStateMachine(
		std::shared_ptr<CVarManagerWrapper> cvarManager, 
		std::shared_ptr<IStatUpdater> statUpdater, 
		std::shared_ptr<IStatWriter> statWriter,
		std::shared_ptr<PluginState> pluginState);

	/** Hooks to any events whic are related to state transitions */
	void hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper);

	/** Processes (or ignores) an OnTrainingModeLoaded event.
	 *
	 * This is public since the same hook needs to be used for more than this class, and registering to every hook is only possible once per plugin.
	 * That way, the parent class can hook into the event instead, and call this method and whatever else.
	 * 
	 * \param	trainingWrapper		provides access to the amount of total rounds etc. Not const since getters are not const in the SDK.
	 **/
	void processOnTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper);

private:
	/** Processes (or ignores) an EventRoundChanged event.
	 *
	 * \param	trainingWrapper		provides access to the amount of total rounds etc. Not const since getters are not const in the SDK.
	 **/
	void processEventRoundChanged(TrainingEditorWrapper& trainingWrapper);
	/** Processes (or ignores) a TrainingShotAttempt event. */
	void processTrainingShotAttempt();
	/** Processes (or ignores) an OnCarTouch event. */
	void processOnCarTouch();
	/** Processes (or ignores) an OnHitGoal event. */
	void processOnHitGoal();
	/** Updates the current state. */
	void setCurrentState(CustomTrainingState newState);


	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Allows logging.
	std::shared_ptr<IStatUpdater> _statUpdater; ///< Stores the object which keeps track of statistics.
	std::shared_ptr<IStatWriter> _statWriter; ///< Stores the object which allows storing stats permanently.
	std::shared_ptr<PluginState> _pluginState; ///< Stores other state parameters of the plugin, not related to the custom training state

	CustomTrainingState _currentState; ///< Stores the currently active state
	bool _goalWasScoredInCurrentAttempt = false; ///< True if a goal has been scored while in TrainingShotAttempt state.
	bool _ballWasHitInCurrentAttempt = false; ///< True if the ball was hit at least once while in TrainingShotAttempt state.
};