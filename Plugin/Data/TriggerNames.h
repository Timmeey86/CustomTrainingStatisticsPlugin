#pragma once



/** Defines triggers which can be activated indirectly by the user. The strings are not displayed to the user, but used as internal keys. */
class TriggerNames
{
public:
	static const char* ResetStatistics;
	static const char* RestoreStatistics;
	static const char* ToggleLastAttempt;
	static const char* ToggleHeatmapDisplay;
	static const char* ToggleImpactLocationDisplay;
	static const char* CompareBaseChanged;
};
