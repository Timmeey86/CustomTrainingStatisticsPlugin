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
void StatUpdaterEventBridge::onCompareBaseToggled()
{
	_statUpdater->updateCompareBase();
}
void StatUpdaterEventBridge::onTrainingModeLoaded(TrainingEditorWrapper& trainingWrapper, TrainingEditorSaveDataWrapper* trainingData)
{
	(void)trainingWrapper;

	std::string trainingPackCode = {};
	if (trainingData)
	{
		trainingPackCode = trainingData->GetCode().ToString();
		_pluginState->TrainingPackCreator = trainingData->GetCreatorName().ToString();
		_pluginState->TrainingPackName = trainingData->GetTM_Name().ToString();
	}
	else
	{
		_pluginState->TrainingPackCreator = {};
		_pluginState->TrainingPackName = {};
	}
	_pluginState->TrainingPackCode = trainingPackCode;
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

void StatUpdaterEventBridge::onBallHit(TrainingEditorWrapper& trainingWrapper, bool isInitialHit)
{
	(void)trainingWrapper;

	if (isInitialHit)
	{
		_statUpdater->processInitialBallHit();
	}
	// else: ignore anything beyond the first touch of the current attempt
}

void StatUpdaterEventBridge::attemptAboutToBeReset()
{
	_statUpdater->updateData();
}
