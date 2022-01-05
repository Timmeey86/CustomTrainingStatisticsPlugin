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
	virtual void onLoad();
	virtual void onUnload();

	// Inherited via PluginSettingsWindow
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

private:

	void render(CanvasWrapper canvas) const; // Copy is intentional since we don't want to modify the object owned by the caller.

	void reset();
	void update(bool isGoal, bool isReset);
	void registerAttempt(bool isGoal);
	void recalculatePercentages();
	void handleGoal();
	void handleShotReset();

	CounterStats _stats;
	bool _goalReplayIsActive = false;
	bool _enabled = true;

};
