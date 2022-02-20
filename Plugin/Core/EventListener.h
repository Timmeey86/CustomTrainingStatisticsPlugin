#pragma once

#include <memory>
#include <bakkesmod/wrappers/GameWrapper.h>
#include <bakkesmod/wrappers/cvarmanagerwrapper.h>

#include "../Data/PluginState.h"

#include "IStatDisplay.h"
#include "IStatUpdater.h"
#include "IStatReader.h"
#include "CustomTrainingStateMachine.h"
#include "AbstractEventReceiver.h"


/** Hooks into various rocket league events and calls the appropriate interface methods. */
class EventListener
{
public:
	/** Provides initialization parameters for the event listener. */
	EventListener(std::shared_ptr<GameWrapper> gameWrapper, std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<PluginState> pluginState);

	/** Hooks into events which are supposed to update statistics. */
	void registerUpdateEvents(std::shared_ptr<IStatUpdater> statUpdater, std::shared_ptr<IStatWriter> statWriter);
	
	/** Hooks into events related to rendering a UI to the user. */
	void registerRenderEvents(std::shared_ptr<IStatDisplay> statDisplay);

	/** Registers events which update the game state. */
	void registerGameStateEvents();

	/** Registers an event receiver which wants to get notified about any occurring events. */
	void addEventReceiver(std::shared_ptr<AbstractEventReceiver> eventReceiver);
	
private:

	/** Returns true if events shall currently be sent. */
	bool statUpdatesShallBeSent();

	std::shared_ptr<IStatReader> _statReader; ///< Allows reading statistics from previous sessions
	std::shared_ptr<GameWrapper> _gameWrapper; ///< Provides access to anything related to Rocket League
	std::shared_ptr<CVarManagerWrapper> _cvarManager; ///< Provides access to custom variables
	std::shared_ptr<PluginState> _pluginState; ///< Stores the state of the plugin
	std::shared_ptr<CustomTrainingStateMachine> _stateMachine; ///< Keeps track of the current state of the custom training (attempt not started, attempt started etc)


	std::vector<std::shared_ptr<AbstractEventReceiver>> _eventReceivers; ///< Stores pointers to objects which might want to process events
};

