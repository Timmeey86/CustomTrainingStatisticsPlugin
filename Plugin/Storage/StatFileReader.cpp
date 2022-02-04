#include <pch.h>
#include "StatFileReader.h"
#include "StatFileDefs.h"

#include <sstream>
#include <filesystem>
#include <fstream>

StatFileReader::StatFileReader(std::shared_ptr<GameWrapper> gameWrapper)
	: _gameWrapper(gameWrapper)
{
}

std::vector<std::string> StatFileReader::getAvailableResourcePaths(const std::string& trainingPackCode)
{
	// Read the folder for the current training pack
	std::ostringstream stringStream;
	stringStream << _gameWrapper->GetBakkesModPath().u8string() << u8"\\data\\CustomTrainingStatistics\\" << trainingPackCode;
	auto folderPath = std::filesystem::u8path(stringStream.str());

	std::vector<std::string> filePaths;
	if (std::filesystem::exists(folderPath))
	{
		try
		{

			for (const auto& entry : std::filesystem::directory_iterator(folderPath))
			{
				if (entry.is_regular_file())
				{
					filePaths.emplace_back(entry.path().u8string());
				}
			}
		}
		catch(const std::filesystem::filesystem_error&)
		{
			// treat this case like there would be no files
		}
	}

	return filePaths;
}

ShotStats StatFileReader::readStats(const std::string& resourcePath)
{
	return ShotStats();
}

std::pair<std::string, std::string> getLineValues(const std::string& line)
{
	std::string delimiter = "\t";

	auto end = line.find(delimiter);
	if (end == std::string::npos)
	{
		return {};
	}

	return { line.substr(0, end), line.substr(end + 1, line.size() - end + 1) };
}

int StatFileReader::peekAttemptAmount(const std::string& resourcePath)
{
	// Try opening the file
	std::ifstream fileStream(resourcePath);
	if (fileStream.fail()) { return 0; }

	// Try reading the version from the file
	std::string currentLine;
	if (!std::getline(fileStream, currentLine)) {return 0; }

	auto [versionTag, versionNumber] = getLineValues(currentLine);
	if (versionTag != StatFileDefs::Version || versionNumber != StatFileDefs::CurrentVersionNumber)
	{
		return 0; // When there is a new file format version, we'll have to implement a data patcher
	}

	// This looks like an actually supported file => Currently, the version will be followed by the amount of shots, a separator line and then the attempts
	// We're only interested in the attempts here

	std::getline(fileStream, currentLine);
	std::getline(fileStream, currentLine);
	std::getline(fileStream, currentLine);

	auto [attemptsTag, attemptAmount] = getLineValues(currentLine);
	auto attempts = std::stoi(attemptAmount);
	if (attemptsTag != StatFileDefs::Attempts || attempts < 0)
	{
		// The file is invalid, maybe someone messed with it
		return 0;
	}

	return attempts;
}
