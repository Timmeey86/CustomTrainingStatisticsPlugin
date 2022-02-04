#include <pch.h>
#include "StatFileReader.h"

#include <sstream>
#include <filesystem>

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

int StatFileReader::peekAttemptAmount(const std::string& resourcePath)
{
	return 0;
}
