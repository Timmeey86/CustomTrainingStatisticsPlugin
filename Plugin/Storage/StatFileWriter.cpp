#include <pch.h>
#include "StatFileWriter.h"

#include <sstream>
#include <ostream>
#include <filesystem>
#include <chrono>
#include <fstream>

using sysclock_t = std::chrono::system_clock;

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

	auto folderPath = std::filesystem::path(stringStream.str());
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

void StatFileWriter::writeData()
{
	if (!_currentStats)
	{
		return;
	}
}
