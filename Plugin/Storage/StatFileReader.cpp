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
		catch (const std::filesystem::filesystem_error&)
		{
			// treat this case like there would be no files
		}
	}

	return filePaths;
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
	if (!std::getline(fileStream, currentLine)) { return 0; }

	auto [versionTag, versionNumber] = getLineValues(currentLine);
	if (versionTag != StatFileDefs::Version || versionNumber != StatFileDefs::CurrentVersionNumber)
	{
		return 0; // When there is a new file format version, we'll have to implement a data patcher
	}

	// This looks like an actually supported file => Currently, the version will be followed by the amount of shots, a separator line and then the attempts
	// We're only interested in the attempts here

	for (int i = 0; i < 3; i++)
	{
		if (!std::getline(fileStream, currentLine)) { return 0; }
	}

	auto [attemptsTag, attemptAmount] = getLineValues(currentLine);
	auto attempts = std::stoi(attemptAmount);
	if (attemptsTag != StatFileDefs::Attempts || attempts < 0)
	{
		// The file is invalid, maybe someone messed with it
		return 0;
	}

	return attempts;
}

bool readValueIntoField(std::ifstream& stream, int* valuePointer)
{
	std::string currentLine;
	if (!std::getline(stream, currentLine)) { return false; }
	if (currentLine.empty()) { return false; }

	auto [key, value] = getLineValues(currentLine);
	if (key.empty() || value.empty()) { return false; }

	auto valueAsInt = std::stoi(value);
	if (valueAsInt < 0)
	{
		return false;
	}
	*valuePointer = valueAsInt;
	return true;
}
bool readValueIntoField(std::ifstream& stream, double* valuePointer)
{
	std::string currentLine;
	if (!std::getline(stream, currentLine)) { return false; }
	if (currentLine.empty()) { return false; }

	auto [key, value] = getLineValues(currentLine);
	if (key.empty() || value.empty()) { return false; }

	auto valueAsFloat = std::stod(value);
	if (valueAsFloat < .0)
	{
		return false;
	}
	*valuePointer = valueAsFloat;
	return true;
}


ShotStats StatFileReader::readStats(const std::string& resourcePath)
{
	// Try opening the file
	std::ifstream fileStream(resourcePath);
	if (fileStream.fail()) { return {}; }

	std::string currentLine;

	// Skip the version, we checked that in peekAmount() already
	if (!std::getline(fileStream, currentLine)) { return {}; }
	// Actually read the number of shots
	if (!std::getline(fileStream, currentLine)) { return {}; }

	auto [numberOfShotsTag, numberOfShotsValue] = getLineValues(currentLine);
	auto numberOfShots = std::stoi(numberOfShotsValue);
	if (numberOfShotsTag != StatFileDefs::NumberOfShots || numberOfShots <= 0)
	{
		return {};
	}

	// Build a vector which points to the all stats object and then the shot stats objects. That way we can read everything in a loop
	ShotStats stats;
	for (int shotNumber = 0; shotNumber < numberOfShots; shotNumber++)
	{
		stats.PerShotStats.emplace_back();
	}

	std::vector<StatsData*> statsDataObjectsToBeFilled = { &stats.AllShotStats };
	for (int shotNumber = 0; shotNumber < numberOfShots; shotNumber++)
	{
		statsDataObjectsToBeFilled.emplace_back(&stats.PerShotStats[shotNumber]);
	}

	// Lazy approach: We got the version and this looks like a valid file, so we ignore the labels
	//                A more robust approach would obviously be to create a map of key,value pairs and then distribute stats based on that
	for (int shotNumber = -1; shotNumber < numberOfShots; shotNumber++)
	{
		auto vectorIndex = shotNumber + 1;
		auto statsDataPointer = statsDataObjectsToBeFilled[vectorIndex];

		// Skip the dashes lines
		if (!std::getline(fileStream, currentLine)) { return {}; }

		// Read stats
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.Attempts)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.Goals)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.InitialHits)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.GoalStreakCounter)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.MissStreakCounter)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.LongestGoalStreak)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Stats.LongestMissStreak)) { return {}; }

		// last N shot: we need to add true/false values for each 1/0 in the string
		if (!std::getline(fileStream, currentLine)) { return {}; }
		auto [key, boolArrayString] = getLineValues(currentLine);
		// Note: boolArrayString might be empty if the last session didn't include at least one of the shots

		for (int index = 0; index < boolArrayString.size(); index++)
		{
			statsDataPointer->Stats.Last50Shots.emplace_back((boolArrayString.at(index) == '1' ? true : false));
		}

		// We can't restore goal speeds (we would have to export all the single goal values in order to properly restore mean/median)
		if (!std::getline(fileStream, currentLine)) { return {}; } // latest speed
		if (!std::getline(fileStream, currentLine)) { return {}; } // max speed
		if (!std::getline(fileStream, currentLine)) { return {}; } // min speed
		if (!std::getline(fileStream, currentLine)) { return {}; } // median speed
		if (!std::getline(fileStream, currentLine)) { return {}; } // mean speed

		if (!readValueIntoField(fileStream, &statsDataPointer->Data.InitialHitPercentage)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Data.SuccessPercentage)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Data.PeakSuccessPercentage)) { return {}; }
		if (!readValueIntoField(fileStream, &statsDataPointer->Data.PeakShotNumber)) { return {}; }
	}
	return stats;
}