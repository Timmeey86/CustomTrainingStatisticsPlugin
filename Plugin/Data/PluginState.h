#pragma once

/**
* Stores information about the current state of the plugin.
*/
class PluginState
{
public:
	PluginState() = default;

	bool PreviousAttemptWasAGoal = false;	///< If this is still false when a new attempt is started, it means the previous shot was a miss.
	bool GoalReplayIsActive = false;		///< True while a goal is being replayed. We ignore other events during that time.
	bool PluginIsEnabled = true;			///< True while the user has not disabled the plugin in the settings UI.
};
