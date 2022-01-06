#pragma once

#include <string>
#include <functional>
#include <unordered_set>

#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "../Data/PluginState.h"

/** Implements methods which create a settings UI. */
class PluginSettingsUI : public BakkesMod::Plugin::PluginSettingsWindow
{
public:
	/** Initializes the plugin settings UI. */
	void initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState);


	/** Creates and configures the UI controls for the settings. */
	void RenderSettings() override;
	/** Retrieves the name of the plugin for the settings dialog. */
	std::string GetPluginName() override;
	/** Sets the UI context to the given context */
	void SetImGuiContext(uintptr_t ctx) override;

private:
	/** Creates a check box in the UI, and performs a one-time registration of the associated cvar if it hasn't been done yet. */
	void createCheckbox(const std::string& variableName, const std::string& displayText, const std::string& tooltipText, std::function<void(bool)> setValueFunc);

	void createSlider(const std::string& variableName, const std::string& displayText, int minValue, int maxValue, int defaultValue);

	std::function<void(const std::string&)> _sendNotifierFunc; ///< A function which is able to send a notifier for which CVarManagerWrapper::registerNotifier has been called.
	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Allows registering and retrieving custom variables.
	std::shared_ptr<PluginState> _pluginState; ///< Stores the state of the plugin.
};