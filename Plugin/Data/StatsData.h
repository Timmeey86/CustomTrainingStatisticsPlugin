#pragma once

#include "PlayerStats.h"
#include "CalculatedData.h"

class StatsData
{
public:
	StatsData() = default;
	StatsData(PlayerStats stats, CalculatedData data)
		: Stats(stats)
		, Data(data)
	{
	}

	PlayerStats Stats;		///< Statistics about the player
	CalculatedData Data;	///< Calculated data based on PlayerStats
};
