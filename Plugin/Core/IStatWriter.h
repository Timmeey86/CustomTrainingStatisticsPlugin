#pragma once

#include "../DLLImportExport.h"

#include <string>

/** The public interface of classes which allow writing statistics, e.g. to the file system. */
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT IStatWriter
{
protected:
	IStatWriter() = default;

public:
	virtual ~IStatWriter() = default;

	/** Initializes the storage by e.g. creating a file, database entry, REST PUT message or whatever. 
	 *
	 * This must be implemented in a way so it can be called multiple times. Calling it twice for the same code is supposed to create a new training pack.
	 */
	virtual void initializeStorage(const std::string& trainingPackCode) = 0;

	/** Writes the current stats to the storage. This should always override the output of the previous call until initializeStorage() is called again. 
	 *
	 * Implementers should ask for the shared stats pointer in the constructor.
	 */
	virtual void writeData() = 0;
};
