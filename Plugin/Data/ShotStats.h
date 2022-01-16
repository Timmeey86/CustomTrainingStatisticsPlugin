#pragma once

#include <vector>

#include "StatsData.h"

class ShotStats
{
public:
	ShotStats() = default;

	StatsData AllShotStats;								///< Statistics for all shots
	std::vector<StatsData> PerShotStats;	///< Statistics for each shot individually
};
