#pragma once

#include "PlayerStats.h"
#include "CalculatedData.h"

class StatsData
{
public:
	StatsData() = default;
	StatsData(std::shared_ptr<PlayerStats> stats, std::shared_ptr<CalculatedData> data)
		: Stats(stats)
		, Data(data)
	{
	}

	std::shared_ptr<PlayerStats> Stats;		///< Statistics about the player
	std::shared_ptr<CalculatedData> Data;	///< Calculated data based on PlayerStats
};
