#pragma once

#include <string>
#include <functional>

#include "bakkesmod/plugin/PluginSettingsWindow.h"

/** Implements methods which create a settings UI. */
class PluginSettingsUI : public BakkesMod::Plugin::PluginSettingsWindow
{
public:
	/** Initializes the plugin settings UI. */
	void initPluginSettingsUi(std::function<void(const std::string&)> sendNotifierFunc, std::function<CVarWrapper(const std::string&)> getVariableWrapperFunc);


	/** Creates and configures the UI controls for the settings. */
	void RenderSettings() override;
	/** Retrieves the name of the plugin for the settings dialog. */
	std::string GetPluginName() override;
	/** Sets the UI context to the given context */
	void SetImGuiContext(uintptr_t ctx) override;

private:
	std::function<void(const std::string&)> _sendNotifierFunc; ///< A function which is able to send a notifier for which CVarManagerWrapper::registerNotifier has been called.
	std::function<CVarWrapper(const std::string&)> _getVariableWrapperFunc; ///< A function which is able to retrieve a CVarWrapper for a given variable.
};