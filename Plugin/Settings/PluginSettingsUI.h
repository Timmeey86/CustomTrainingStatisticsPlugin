#pragma once

#include <string>
#include <functional>
#include <unordered_set>

#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "SettingsDefinition.h"
#include "../Data/PluginState.h"

/** Implements methods which create a settings UI. */
class PluginSettingsUI : public BakkesMod::Plugin::PluginSettingsWindow
{
public:
	/** Initializes the plugin settings UI. */
	void initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::shared_ptr<CVarManagerWrapper> cvarManager);


	/** Creates and configures the UI controls for the settings. */
	void RenderSettings() override;
	/** Retrieves the name of the plugin for the settings dialog. */
	std::string GetPluginName() override;
	/** Sets the UI context to the given context */
	void SetImGuiContext(uintptr_t ctx) override;

private:
	/** Creates a check box in the UI. */
	void createCheckbox(const SettingsDefinition& settingsDefinition);
	/** Creates a slider control for integer values. */
	void createIntSlider(const SettingsDefinition& settingsDefinition);
	/** Creates a slider control for float values. */
	void createFloatSlider(const SettingsDefinition& settingsDefinition);
	/** Creates a control which allows configuring the color. */
	void createColorEdit(const SettingsDefinition& settingsDefinition);
	/** Creates a dropdown menu. */
	void createDropdownMenu(const SettingsDefinition& settingsDefinition, const char* items[], int numItmes);

	std::function<void(const std::string&)> _sendNotifierFunc; ///< A function which is able to send a notifier for which CVarManagerWrapper::registerNotifier has been called.
	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Allows registering and retrieving custom variables.
};