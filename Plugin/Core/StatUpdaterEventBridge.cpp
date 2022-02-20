#include <pch.h>
#include "StatUpdaterEventBridge.h"

StatUpdaterEventBridge::StatUpdaterEventBridge(std::shared_ptr<IStatUpdater> statUpdater, std::shared_ptr<PluginState> pluginState)
	: _statUpdater( statUpdater )
	, _pluginState( pluginState )
{

}

void StatUpdaterEventBridge::onResetStatisticsTriggered()
{
	_statUpdater->processReset(_pluginState->TotalRounds);
}

void StatUpdaterEventBridge::onRestorePreviousSessionTriggered()
{
	_statUpdater->restoreLastSession();
}
void StatUpdaterEventBridge::onTogglePreviousAttemptTriggered()
{
	_statUpdater->toggleLastAttempt();
}
void StatUpdaterEventBridge::onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, const std::string& trainingPackCode)
{
	(void)trainingWrapper;

	_statUpdater->publishTrainingPackCode(trainingPackCode);
	_statUpdater->processReset(_pluginState->TotalRounds);
}

void StatUpdaterEventBridge::onRoundChanged(TrainingEditorWrapper& trainingWrapper)
{
	(void)trainingWrapper;
}

void StatUpdaterEventBridge::onAttemptStarted()
{
	_statUpdater->processAttempt();
}

void StatUpdaterEventBridge::onAttemptFinishedWithGoal(TrainingEditorWrapper& trainingWrapper)
{
	(void)trainingWrapper;
	_statUpdater->processGoal();
}

void StatUpdaterEventBridge::onAttemptFinishedWithoutGoal(TrainingEditorWrapper& trainingWrapper)
{
	(void)trainingWrapper;
	_statUpdater->processMiss();
}

void StatUpdaterEventBridge::onAttemptFinished(TrainingEditorWrapper& trainingWrapper)
{
	(void)trainingWrapper;
	_statUpdater->updateData();
}

void StatUpdaterEventBridge::onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit)
{
	(void)trainingWrapper;

	if (isInitialHit)
	{
		_statUpdater->processInitialBallHit();
	}
	// else: ignore anything beyond the first touch of the current attempt
}
