#pragma once

#include "../DLLImportExport.h"
#include "../Data/ShotStats.h"
#include <string>
#include <vector>

/** The public interface of classes which allow reading statistics, e.g. from the file system. */
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT IStatReader
{
protected:
	IStatReader() = default;

public:
	virtual ~IStatReader() = default;

	/** Retrieves a list of available resource paths, ordered so the most recent one appears first in the vector. */
	virtual std::vector<std::string> getAvailableResourcePaths(const std::string& trainingPackCode) = 0;

	/** Reads stats from the given resource path. */
	virtual ShotStats readStats(const std::string& resourcePath, bool statsAboutToBeRestored) = 0;

	/** Peeks into the number of attempts which are stored for the given resource path. */
	virtual int peekAttemptAmount(const std::string& resourcePath) = 0;

	/** Reads the given shot stats for the training pack as a whole. This could e.g. be all time peak stats. They are identified by the training pack code. 
	 *
	 * The returned object will have zero attempts if there are no peak stats for this training pack yet.
	 */
	virtual ShotStats readTrainingPackStatistics(const std::string& trainingPackCode) = 0;
};
