#pragma once

#include "SettingsDefinition.h"
#include "../Data/PluginState.h"

#include <bakkesmod/wrappers/cvarmanagerwrapper.h>

/** This class is responsible for registering CVars to the CVar Manager and forwarding CVar changes to the appropriate structs. */
class SettingsRegistration
{
public:
	/** Registers any CVars which can be modified by the user. */
	static void registerCVars(std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState);

private:
	/** Registers a single CVar based on the given settings definition. */
	static CVarWrapper registerCVar(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition);
	/** Registers a variable for a check box. */
	static void registerCheckboxSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(bool)> setValueFunc);
	/** Registers a variable for an integer range slider. */
	static void registerIntSliderSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(int)> setValueFunc);
	/** Registers a variable for a float range slider. */
	static void registerFloatSliderSetting(std::shared_ptr<CVarManagerWrapper> cvarManager, const SettingsDefinition& settingsDefinition, std::function<void(float)> setValueFunc);
};