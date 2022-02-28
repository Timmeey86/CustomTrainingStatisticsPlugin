#include <pch.h>

#include <sstream>
#include <ostream>
#include <filesystem>
#include <chrono>
#include <fstream>


#include "StatFileWriter.h"
#include "StatFileDefs.h"


using sysclock_t = std::chrono::system_clock;

static const char* const CurrentVersion = "1.0";

StatFileWriter::StatFileWriter(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<ShotStats> shotStats, std::shared_ptr<ShotDistributionTracker> tracker)
	: _gameWrapper(gameWrapper)
	, _currentStats(shotStats)
	, _shotDistributionTracker(tracker)
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

std::string bool_vector_to_string(const std::vector<bool>& values)
{
	std::ostringstream stream;
	for (auto value : values)
	{
		stream << (value ? "1" : "0");
	}
	return stream.str();
}

std::string float_vector_to_string(const std::vector<float>& values)
{
	std::ostringstream stream;
	const char vectorSeparator = '|';
	stream << std::to_string(values.size()) + vectorSeparator;
	for (auto value : values)
	{
		stream << value << vectorSeparator;
	}
	return stream.str();
}

std::string shot_location_vector_to_string(const std::vector<Vector>& values)
{
	std::ostringstream stream;
	const char vectorSeparator = '|';
	stream << std::to_string(values.size()) + vectorSeparator;
	for (auto vector : values)
	{
		stream << fmt::format("{},{},{}{}", vector.X, vector.Y, vector.Z, vectorSeparator);
	}
	return stream.str();
}

void StatFileWriter::writeStatsData(std::ofstream& stream, const StatsData& statsData, bool isSummaryData)
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
	writeLine(stream, StatFileDefs::LastNShotsPercentage, bool_vector_to_string(statsData.Stats.Last50Shots));
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

	// v1.1 stats
	writeLine(stream, StatFileDefs::AirDribbleTouches, std::to_string(statsData.Stats.MaxAirDribbleTouches));
	writeLine(stream, StatFileDefs::AirDribbleTime, std::to_string(statsData.Stats.MaxAirDribbleTime));
	writeLine(stream, StatFileDefs::GroundDribbleTime, std::to_string(statsData.Stats.MaxGroundDribbleTime));
	writeLine(stream, StatFileDefs::DoubleTapGoals, std::to_string(statsData.Stats.DoubleTapGoals));
	writeLine(stream, StatFileDefs::DoubleTapPercentage, std::to_string(statsData.Data.DoubleTapGoalPercentage));
	writeLine(stream, StatFileDefs::MaxFlipResets, std::to_string(statsData.Stats.MaxFlipResets));
	writeLine(stream, StatFileDefs::TotalFlipResets, std::to_string(statsData.Stats.TotalFlipResets));
	writeLine(stream, StatFileDefs::FlipResetsPerAttempt, std::to_string(statsData.Data.AverageFlipResetsPerAttempt));
	writeLine(stream, StatFileDefs::FlipResetPercentage, std::to_string(statsData.Data.FlipResetGoalPercentage));
	writeLine(stream, StatFileDefs::CloseMisses, std::to_string(statsData.Stats.CloseMisses));
	writeLine(stream, StatFileDefs::CloseMissPercentage, std::to_string(statsData.Data.CloseMissPercentage));

	// v1.2 stats - Shot locations are only available once for the training pack rather than for every shot
	auto shotLocations = isSummaryData ? _shotDistributionTracker->getImpactLocations() : std::vector<Vector>();
	writeLine(stream, StatFileDefs::ImpactLocations, shot_location_vector_to_string(shotLocations));

	// v1.3 stats
	writeLine(stream, StatFileDefs::GoalSpeedValues, float_vector_to_string(statsData.Stats.GoalSpeedStats.getAllShotValues()));
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

	writeStatsData(outputFileStream, _currentStats->AllShotStats, true);

	for (const auto& shotStats : _currentStats->PerShotStats)
	{
		writeStatsData(outputFileStream, shotStats, false);
	}
}
