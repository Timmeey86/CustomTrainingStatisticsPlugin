#include <pch.h>

#include "AllTimePeakHandler.h"

void AllTimePeakHandler::updateMaximumStats(const std::shared_ptr<ShotStats>& currentStats)
{
}

void AllTimePeakHandler::updateMinimumStats(const std::shared_ptr<ShotStats>& currentStats)
{
}

ShotStats AllTimePeakHandler::getPeakStats() const
{
	return ShotStats();
}

bool AllTimePeakHandler::writeAllStatFile()
{
	// TODO: Do we need to handle StatsShallBeRecorded now?
	if (_pluginState->TrainingPackCode.empty()) { return true; }

	auto fileName = _pluginState->TrainingPackCode;
	_statWriter->writeTrainingPackStatistics(_allTimePeakStats, _pluginState->TrainingPackCode);
}

bool AllTimePeakHandler::readAllStatFile()
{
	// TODO: Make sure the training pack file is skipped when reading normal stats, and offer a method in stat reader to read that explicitly
	return false;
}
