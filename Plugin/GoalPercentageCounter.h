#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "Settings/PluginSettingsUI.h"
#include "Data/CalculatedData.h"
#include "Data/PlayerStats.h"
#include "Data/PluginState.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class GoalPercentageCounter: public BakkesMod::Plugin::BakkesModPlugin, public PluginSettingsUI
{

	//Boilerplate
	virtual void onLoad(); // REFACTOR register hooks
	virtual void onUnload(); // REFACTOR just logging

private:

	// REFACTOR UI overlay
	void render(CanvasWrapper canvas) const; // Copy is intentional since we don't want to modify the object owned by the caller.

	// REFACTOR maybe add constructor to data structures
	void reset();

	// REFACTOR control flow
	void update(bool isGoal, bool isReset);
	void registerAttempt(bool isGoal);

	// REFACTOR calculation
	void recalculatePercentages();

	// REFACTOR stat update
	void handleGoal();
	void handleAttempt();

	// REFACTOR state
	CalculatedData _calculatedData;
	PlayerStats _playerStats;
	PluginState _pluginState;
};

