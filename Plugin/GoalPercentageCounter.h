#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
#include "CounterStats.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class GoalPercentageCounter: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{

	//Boilerplate
	virtual void onLoad(); // REFACTOR register hooks
	virtual void onUnload(); // REFACTOR just logging

	// REFACTOR - Move to own header - Settings UI

	// Inherited via PluginSettingsWindow
	void RenderSettings() override; 
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

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
	CounterStats _stats;
};

