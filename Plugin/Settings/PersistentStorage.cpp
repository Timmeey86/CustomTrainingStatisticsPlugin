#include <pch.h>
#include "PersistentStorage.h"
#include <fstream>
#include "SettingsDefinition.h"

PersistentStorage::PersistentStorage(
	BakkesMod::Plugin::BakkesModPlugin* plugin,
	const std::string& storage_file_name,
	const bool auto_write, 
	bool auto_load) :
	cv_(plugin->cvarManager),
	storage_file_(GetStorageFilePath(plugin->gameWrapper, storage_file_name)),
	auto_write_(auto_write)

{
	cv_->log(fmt::format("PersistentStorage: created and will store the data in {}", storage_file_.string()));
	cv_->registerNotifier("writeconfig", [this](...)
	{
		WritePersistentStorage();
	}, "", 0);
	if (auto_load)
	{
		plugin->gameWrapper->SetTimeout([this](...)
		{
			Load();
		}, 0.1f);
	}
}

PersistentStorage::~PersistentStorage()
{
	WritePersistentStorage();
}

void PersistentStorage::WritePersistentStorage()
{
	std::ofstream out(storage_file_);
	//LOG("PersistentStorage: Writing to file");
	for (const auto& [cvar, cvar_cache_item] : cvar_cache_)
	{
		out << fmt::format("{} \"{}\" //{}\n", cvar, cvar_cache_item.value, cvar_cache_item.description);
	}
}

void PersistentStorage::Load()
{
	cv_->log("PersistentStorage: Loading the persistent storage cfg");
	cv_->loadCfg(storage_file_.string());

	// We need to restore the order of stats to be displayed, but only once while being loaded.
	// The order is a vector encoded as a readable string
	if (auto settingsOrderCvar = cv_->getCvar(GoalPercentageCounterSettings::OrderedStatsCVarName))
	{
		auto potentialSettingsOrder = string_to_vector(settingsOrderCvar.getStringValue());
		std::scoped_lock lock(GoalPercentageCounterSettings::OrderedStatsMutex);
		if (potentialSettingsOrder.size() == GoalPercentageCounterSettings::OrderedStatsNames.size())
		{
			// The stats are valid and may be applied
			GoalPercentageCounterSettings::OrderedStatsNames = potentialSettingsOrder;
		}
		else
		{
			// The stats are from an older version or empty => apply current defaults
			settingsOrderCvar.setValue(vector_to_string(GoalPercentageCounterSettings::OrderedStatsNames));
			cv_->log("PersistentStorage: Could not restore stat order, applying default.");
		}
	}
	else
	{
		// Shouldn't happen
	}
	loaded_ = true;
}

CVarWrapper PersistentStorage::RegisterPersistentCvar(const std::string& cvar, const std::string& defaultValue,
	const std::string& desc,
	const bool searchAble, const bool hasMin, const float min,
	const bool hasMax, const float max, const bool saveToCfg)
{
	auto registered_cvar = cv_->registerCvar(cvar, defaultValue, desc, searchAble, hasMin, min, hasMax, max, saveToCfg);
	if (!registered_cvar)
	{
		cv_->log(fmt::format("PersistentStorage: Failed to register the cvar {}", cvar));
		return registered_cvar;
	}
	AddCVar(cvar);
	return registered_cvar;
}

void PersistentStorage::AddCVars(const std::initializer_list<std::string> cvars)
{
	for (const auto& cvar_name : cvars)
	{
		AddCVar(cvar_name);
	}
}

void PersistentStorage::AddCVar(const std::string& s)
{
	if (auto cvar = cv_->getCvar(s))
	{
		cvar_cache_.insert_or_assign(s, CvarCacheItem{ cvar });
		cvar.addOnValueChanged(
			[this](auto old, auto new_cvar)
		{
			OnPersistentCvarChanged(old, new_cvar);
		});
	}
	else
	{
		cv_->log(fmt::format("PersistentStorage: Warning the cvar {} should be registered before adding it to persistent storage", s));
	}
}

CVarWrapper PersistentStorage::GetCVar(const std::string& s)
{
	return cv_->getCvar(s);
}

std::filesystem::path PersistentStorage::GetStorageFilePath(const std::shared_ptr<GameWrapper>& gw, std::string file_name)
{
	return gw->GetBakkesModPath() / "cfg" / file_name.append(".cfg");
}

void PersistentStorage::OnPersistentCvarChanged(const std::string& old, CVarWrapper changed_cvar)
{
	const auto cvar_name = changed_cvar.getCVarName();
	if (auto it = cvar_cache_.find(cvar_name); it != cvar_cache_.end())
	{
		it->second = CvarCacheItem{ changed_cvar };
	}
	// If you Write to file before the file has been loaded. You will loose the data there.
	if (auto_write_ && loaded_)
	{
		WritePersistentStorage();
	}
}

PersistentStorage::CvarCacheItem::CvarCacheItem(CVarWrapper cvar) : value(cvar.getStringValue()),
description(cvar.getDescription())
{
}
