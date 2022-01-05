#pragma once

#include <memory>
#include <bakkesmod/wrappers/gamewrapper.h>
#include <bakkesmod/wrappers/cvarmanagerwrapper.h>

#include "../Data/PluginState.h"

#include "IStatDisplay.h"
#include "IStatUpdater.h"

/** Hooks into various rocket league events and calls the appropriate interface methods. */
class EventListener
{
	/** Provides initialization parameters for the event listener. */
	EventListener(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState);

	/** Hooks into events which are supposed to update statistics. */
	void registerUpdateEvents(std::shared_ptr<IStatUpdater> statUpdater);
	
	/** Hooks into events related to rendering a UI to the user. */
	void registerRenderEvents(std::shared_ptr<IStatDisplay> statDisplay);

	/** Registers events which update the game state. */
	void registerGameStateEvents();
	
private:

	/** Returns true if events shall currently be sent. */
	bool statUpdatesShallBeSent();

	std::shared_ptr<GameWrapper> _gameWrapper; ///< Provides access to anything related to Rocket League
	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Provides access to custom variables
	std::shared_ptr<PluginState> _pluginState; ///< Stores the state of the plugin
};

