#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "Settings/PluginSettingsUI.h"
#include "Data/CalculatedData.h"
#include "Data/PlayerStats.h"
#include "Data/PluginState.h"
#include "Core/EventListener.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class GoalPercentageCounter: public BakkesMod::Plugin::BakkesModPlugin, public PluginSettingsUI
{

	//Boilerplate
	virtual void onLoad(); // REFACTOR register hooks
	virtual void onUnload(); // REFACTOR just logging

private:

	std::shared_ptr<PlayerStats> _playerStats = std::make_shared<PlayerStats>();
	std::shared_ptr<CalculatedData> _calculatedData = std::make_shared<CalculatedData>();
	std::shared_ptr<PluginState> _pluginState = std::make_shared<PluginState>();
	std::shared_ptr<EventListener> _eventListener;
};

