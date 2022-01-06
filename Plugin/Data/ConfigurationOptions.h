#pragma once



/** Defines options which can be configured by the user. The strings are not displayed to the user, but used as internal keys. */
class ConfigurationOptions
{
public:
	static const char* EnablePlugin;
	static const char* ResetStatistics;
	static const char* DisplayAttemptsAndGoals;
	static const char* DisplayTotalSuccessRate;
	static const char* DisplayCurrentStreaks;
	static const char* DisplayLongestStreaks;
	static const char* DisplayPeakInfo;
	static const char* DisplayLastNShotPercentage;
	static const char* OverlayXPosition;
	static const char* OverlayYPosition;
	static const char* FontHeightFactor;
	static const char* FontWidthFactor;
};
