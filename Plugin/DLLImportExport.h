#pragma once

#if GOALPERCENTAGECOUNTER_EXPORTS
	#define GOALPERCENTAGECOUNTER_IMPORT_EXPORT __declspec(dllexport) // Create a .lib and a .exp file when compiling GoalPercentageCounter
#else
	#define GOALPERCENTAGECOUNTER_IMPORT_EXPORT __declspec(dllimport) // Look for an existing .lib when using the DLL somewhere, e.g. in a unit test project
#endif

#ifndef NO_EXPORT
	#define NO_EXPORT // Allows documenting that a class shall not be accessible from outside the DLL
#endif 