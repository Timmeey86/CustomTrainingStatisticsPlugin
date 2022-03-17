#pragma once

#include <memory>

#include <bakkesmod/plugin/bakkesmodsdk.h>
#include <bakkesmod/wrappers/GameWrapper.h>
#include <bakkesmod/wrappers/cvarmanagerwrapper.h>

#include "../DLLImportExport.h"
#include "../Data/PluginState.h"
#include "../Calculation/AllTimePeakHandler.h"
#include "IStatWriter.h"
#include "CustomTrainingState.h"
#include "AbstractEventReceiver.h"

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
		std::shared_ptr<IStatWriter> statWriter,
		std::shared_ptr<AllTimePeakHandler> peakHandler,
		std::shared_ptr<PluginState> pluginState);

	/** Hooks to any events whic are related to state transitions 
	 *
	 * \param	eventReceivers		objects which want to be notified about this event.
	 **/
	void hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);

	void processBallSurfaceHit(BallWrapper& ball, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers, TrainingEditorWrapper& trainingWrapper);

	/** Processes (or ignores) an OnTrainingModeLoaded event.
	 *
	 * This is public since the same hook needs to be used for more than this class, and registering to every hook is only possible once per plugin.
	 * That way, the parent class can hook into the event instead, and call this method and whatever else.
	 * We could work around this by making the state machine an abstract event receiver, or by moving such processing out of this class. There are currently no plans to do so, though.
	 * 
	 * \param	trainingWrapper		provides access to the amount of total rounds etc. Not const since getters are not const in the SDK.
	 * \param	trainingData		provides information like the code of the current training pack.
	 * \param	eventReceivers		objects which want to be notified about this event.
	 **/
	void processOnTrainingModeLoaded(
		TrainingEditorWrapper& trainingWrapper, 
		TrainingEditorSaveDataWrapper* trainingData, 
		const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);

private:
	/** Processes (or ignores) an EventRoundChanged event.
	 *
	 * \param	trainingWrapper		provides access to the amount of total rounds etc. Not const since getters are not const in the SDK.
	 * \param	eventReceivers		objects which want to be notified about this event.
	 **/
	void processEventRoundChanged(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);
	void processGoalOrMiss(const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers, TrainingEditorWrapper& trainingWrapper);
	/** Processes (or ignores) a TrainingShotAttempt event. */
	void processTrainingShotAttempt(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);
	/** Processes (or ignores) an OnCarTouch event. */
	void processOnCarTouch(TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);
	/** Processes (or ignores) an OnHitGoal event. */
	void processOnHitGoal(TrainingEditorWrapper& trainingWrapper, BallWrapper& ball, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);
	/** Processes (or ignores) an OnGroundChanged event, where "ground" can also be wall, ceiling, or the ball. */
	void processOnGroundChanged(CarWrapper& car, TrainingEditorWrapper& trainingWrapper, const std::vector<std::shared_ptr<AbstractEventReceiver>>& eventReceivers);
	/** Updates the current state. */
	void setCurrentState(CustomTrainingState newState);


	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Allows logging.
	std::shared_ptr<IStatWriter> _statWriter; ///< Stores the object which allows storing stats permanently.
	std::shared_ptr<AllTimePeakHandler> _peakHandler; ///< Stores the object which reads and writes all time peak stats.
	std::shared_ptr<PluginState> _pluginState; ///< Stores other state parameters of the plugin, not related to the custom training state

	CustomTrainingState _currentState; ///< Stores the currently active state
	bool _goalWasScoredInCurrentAttempt = false; ///< True if a goal has been scored while in TrainingShotAttempt state.
	bool _ballWasHitInCurrentAttempt = false; ///< True if the ball was hit at least once while in TrainingShotAttempt state.
};