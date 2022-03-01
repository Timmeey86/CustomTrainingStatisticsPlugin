#pragma once

#include "AbstractEventReceiver.h"
#include "IStatUpdater.h"
#include "../Data/PluginState.h"

#include <memory>

/** This class was introduced in order to connect the existing IStatUpdater interface to the new event registration system,
 * without breaking existing functionality. It might get merged with IStatUpdater at some point.
 */
class StatUpdaterEventBridge : public AbstractEventReceiver
{
public:
	StatUpdaterEventBridge(std::shared_ptr<IStatUpdater> statUpdater, std::shared_ptr<PluginState> pluginState);

	void onResetStatisticsTriggered() override;
	void onRestorePreviousSessionTriggered() override;
	void onTogglePreviousAttemptTriggered() override;
	void onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, TrainingEditorSaveDataWrapper* trainingData) override;
	void onRoundChanged(TrainingEditorWrapper& trainingWrapper) override;
	void onAttemptStarted() override;
	void onAttemptFinishedWithGoal(TrainingEditorWrapper& trainingWrapper) override;
	void onAttemptFinishedWithoutGoal(TrainingEditorWrapper& trainingWrapper) override;
	void onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit) override;
	void attemptAboutToBeReset() override;

private:
	std::shared_ptr<IStatUpdater> _statUpdater;
	std::shared_ptr<PluginState> _pluginState;
};