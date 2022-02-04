﻿#include <pch.h>

#include <sstream>
#include <ostream>
#include <filesystem>
#include <chrono>
#include <fstream>


#include "StatFileWriter.h"
#include "StatFileDefs.h"


using sysclock_t = std::chrono::system_clock;

static const char* const CurrentVersion = "1.0";

StatFileWriter::StatFileWriter(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<ShotStats> shotStats)
	: _gameWrapper(gameWrapper)
	, _currentStats(shotStats)
{
}

// https://stackoverflow.com/a/41826232/808151
std::string currentDate()
{
	std::time_t now = sysclock_t::to_time_t(sysclock_t::now());

	char buf[32] = { 0 };
	std::strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", ::localtime(&now));

	return std::string(buf);
}

void StatFileWriter::initializeStorage(const std::string& trainingPackCode)
{
	// Create a folder for each training pack
	std::ostringstream stringStream;
	stringStream << _gameWrapper->GetBakkesModPath().u8string() << u8"\\data\\CustomTrainingStatistics\\" << trainingPackCode;

	// TODO:
	// - Actually write stats
	// - Write version number
	// - Implement reader
	auto folderPath = std::filesystem::u8path(stringStream.str());
	if (!std::filesystem::exists(folderPath))
	{
		if (!std::filesystem::create_directories(folderPath))
		{
			// We can't write data => this feature simply won't be supported
			_currentStats = nullptr;
			return;
		}
	}
	
	// Create a file for each time initializeStorage() is called
	stringStream << "\\" << currentDate() << ".txt";
	_outputFilePath = std::filesystem::u8path(stringStream.str());

	std::ofstream outputFileStream;
	outputFileStream.open(_outputFilePath, std::ios::out);
	if (outputFileStream.fail())
	{
		// We can't write data => this feature simply won't be supported
		_currentStats = nullptr;
		return;
	}
	// Else: We successfully created a file (which is empty)
	outputFileStream.close();
}

void writeLine(std::ofstream& stream, const std::string& label, const std::string& value)
{
	stream << label << "\t" << value << std::endl;
}

std::string bool_vector_to_string(std::vector<bool> values)
{
	std::ostringstream stream;
	for (auto value : values)
	{
		stream << (value ? "1" : "0");
	}
	return stream.str();
}

void writeStatsData(std::ofstream& stream, const StatsData& statsData)
{
	stream << StatFileDefs::ShotSeparator << std::endl;

	// Player Stats
	writeLine(stream, StatFileDefs::Attempts, std::to_string(statsData.Stats.Attempts));
	writeLine(stream, StatFileDefs::Goals, std::to_string(statsData.Stats.Goals));
	writeLine(stream, StatFileDefs::InitialHits, std::to_string(statsData.Stats.InitialHits));
	writeLine(stream, StatFileDefs::CurrentGoalStreak, std::to_string(statsData.Stats.GoalStreakCounter));
	writeLine(stream, StatFileDefs::CurrentMissStreak, std::to_string(statsData.Stats.MissStreakCounter));
	writeLine(stream, StatFileDefs::LongestGoalStreak, std::to_string(statsData.Stats.LongestGoalStreak));
	writeLine(stream, StatFileDefs::LongestMissStreak, std::to_string(statsData.Stats.LongestMissStreak));
	writeLine(stream, StatFileDefs::LongestMissStreak, bool_vector_to_string(statsData.Stats.Last50Shots));
	writeLine(stream, StatFileDefs::LatestGoalSpeed, std::to_string(statsData.Stats.GoalSpeedStats.getMostRecent()));
	writeLine(stream, StatFileDefs::MaxGoalSpeed, std::to_string(statsData.Stats.GoalSpeedStats.getMax()));
	writeLine(stream, StatFileDefs::MinGoalSpeed, std::to_string(statsData.Stats.GoalSpeedStats.getMin()));
	writeLine(stream, StatFileDefs::MedianGoalSpeed, std::to_string(statsData.Stats.GoalSpeedStats.getMedian()));
	writeLine(stream, StatFileDefs::MeanGoalSpeed, std::to_string(statsData.Stats.GoalSpeedStats.getMean()));

	// Calculated stats
	writeLine(stream, StatFileDefs::InitialHitPercentage, std::to_string(statsData.Data.InitialHitPercentage));
	writeLine(stream, StatFileDefs::TotalSuccessRate, std::to_string(statsData.Data.SuccessPercentage));
	writeLine(stream, StatFileDefs::PeakSuccessRate, std::to_string(statsData.Data.PeakSuccessPercentage));
	writeLine(stream, StatFileDefs::PeakAtShotNumber, std::to_string(statsData.Data.PeakShotNumber));
}

void StatFileWriter::writeData()
{
	if (!_currentStats)
	{
		return;
	}

	// Open the file with write access and replace anything that might have been in it
	std::ofstream outputFileStream;
	outputFileStream.open(_outputFilePath, std::ios::out | std::ios::trunc);
	if (outputFileStream.fail())
	{
		_currentStats = nullptr;
		return;
	}

	writeLine(outputFileStream, StatFileDefs::Version, StatFileDefs::CurrentVersionNumber);
	writeLine(outputFileStream, StatFileDefs::NumberOfShots, std::to_string(_currentStats->PerShotStats.size()));

	writeStatsData(outputFileStream, _currentStats->AllShotStats);

	for (auto shotStats : _currentStats->PerShotStats)
	{
		writeStatsData(outputFileStream, shotStats);
	}
}