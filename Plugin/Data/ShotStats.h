#pragma once

#include <vector>

#include "StatsData.h"

class ShotStats
{
public:
	ShotStats()
	{
		AllShotStats = std::make_shared<StatsData>(std::make_shared<PlayerStats>(), std::make_shared<CalculatedData>());
		PerShotStats = std::make_shared<std::vector<std::shared_ptr<StatsData>>>();
	};

	std::shared_ptr<StatsData> AllShotStats;								///< Statistics for all shots
	std::shared_ptr<std::vector<std::shared_ptr<StatsData>>> PerShotStats;	///< Statistics for each shot individually
};
