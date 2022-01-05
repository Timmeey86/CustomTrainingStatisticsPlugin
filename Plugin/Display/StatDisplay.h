#pragma once

#include "../Core/IStatDisplay.h"
#include "../Data/CalculatedData.h"
#include "../Data/PlayerStats.h"

class StatDisplay : public IStatDisplay
{
public:
	/** Creates a new object which is able to display statistics to the user. */
	StatDisplay(
		std::shared_ptr<PlayerStats> playerStats,
		std::shared_ptr<CalculatedData> calculatedData
	);

	// Inherited via IStatDisplay
	void renderOneFrame(CanvasWrapper canvas) override;

private:
	std::shared_ptr<PlayerStats> _playerStats;			///< Statistics about the player
	std::shared_ptr<CalculatedData> _calculatedData;	///< Calculated data based on PlayerStats
};

