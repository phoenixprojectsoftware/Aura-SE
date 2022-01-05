#include <map>
#include <memory>

// FIXME: having a hard time getting everything to work
// due to includes being weird, or me not knowing how this works exactly
// Including these doesn't seem right, but I couldn't get it to work
// with only including cvardef.h and enginecallback.h, the later one being
// especially problematic
#include "extdll.h"
#include "util.h"

#include "cvar.h"

namespace CVar
{
	namespace
	{
		struct CustomCVar
		{
			std::string dllValue; // value set by engine or game dll before server startup (on cvar creation)
			std::string startupValue; // value set once at server startup
			std::string serverValue; // value set on every map change, before gamemode cfg
			std::string gamemodeValue; // value set by the gamemode's cfg
			int customFlags;

			CustomCVar()
			{
				customFlags = 0;
			}

			CustomCVar(cvar_t cvar, std::string dllValue, int customFlags = 0)
				: dllValue(dllValue),
				  customFlags(customFlags)
			{}
		};

		std::string BOGUS_VALUE = "__BOGUS_VALUE__";

		// We need this variable because startup changes are harder to record. The config at
		// that point is not executed immediately even if we do SERVER_EXECUTE(), and we
		// have to wait until InstallGameRules() right before we start recording the next step
		// (game.cfg and server.cfg). But that one is executed on every map change, and the
		// startup recording is only done once on server start, so we need to know if it's
		// actually recording the startup before stopping to record which messes up stuff.
		// So it will be recording the startup if it's the server start and it won't be
		// if it's a map change
		bool isRecording = false;

		std::unordered_map<std::string, CustomCVar> cvars;

		// CVar name->value mappings to know default values in every scope
		// Startup is separated from server cvars mostly due to different execution moments
		std::unordered_map<std::string, std::string> snapshot;
		std::unordered_map<std::string, std::string> dllCVars; // startup_server.cfg.
		std::unordered_map<std::string, std::string> startupCVars; // startup_server.cfg.
		std::unordered_map<std::string, std::string> serverCVars; // game.cfg, server.cfg, etc.
		std::unordered_map<std::string, std::string> gamemodeCVars; // gamemode cfg (e.g.: hlccl.cfg)


		std::unordered_map<std::string, CustomCVar> CVar(std::string name, CustomCVar customCVar)
		{
			// Ugly hack, because cvars are created at a time where you
			// may not have a member initialized, or it simply does not
			// let me emplace stuff, so we make it static and it always
			// returns the map, so that we can take it out of this local
			// scope later when members are initialized
			static std::unordered_map<std::string, CustomCVar> cvars;

			if (!name.empty())
			{
				cvars.emplace(name, customCVar);
			}

			return cvars;
		}

		std::unordered_map<std::string, CustomCVar> GetCVars()
		{
			if (cvars.empty())
				return CVar("", CustomCVar());
			else
				return cvars;
		}

		void TrackCVar(std::string name, int customFlags = 0)
		{
			auto cvar = CVAR_GET_POINTER(name.c_str());
			std::string dllValue(cvar->string);
			cvars.emplace(name, CustomCVar(*cvar, dllValue, customFlags));
		}

		void TrackEngineCVars()
		{
			TrackCVar("edgefriction", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			//TrackCVar("mapcyclefile"); // engine appends ".txt" to its value automatically
			//TrackCVar("motdfile"); // if it's not votable nor it is a gamemode cvar, it's useless to track it for now
			//TrackCVar("mp_logdetail"); // cannot detect changes, it's autosanitized by the engine to empty string
			//TrackCVar("mp_logfile");
			//TrackCVar("mp_logecho");
			TrackCVar("mp_footsteps", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_accelerate", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_airaccelerate", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			//TrackCVar("sv_alltalk", CCVAR_VOTABLE); // cannot detect changes, it's autosanitized by the engine to empty string
			TrackCVar("sv_bounce", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_friction", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_gravity", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_maxspeed", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_maxvelocity", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_spectatormaxspeed", CCVAR_VOTABLE);
			TrackCVar("sv_stepsize", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_stopspeed", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_wateraccelerate", CCVAR_VOTABLE | CCVAR_GAMEMODE);
			TrackCVar("sv_wateramp", CCVAR_VOTABLE);
			TrackCVar("sv_zmax", CCVAR_VOTABLE | CCVAR_GAMEMODE);
		}


		void ApplyStartupChanges()
		{
			for (const auto& entry : startupCVars)
			{
				const auto name = entry.first;
				const auto value = entry.second;
				CVAR_SET_STRING(name.c_str(), value.c_str());

				auto& cvar = cvars[name];
				cvar.startupValue = value;
			}
		}

		void ApplyServerChanges()
		{
			for (const auto& entry : serverCVars)
			{
				const auto name = entry.first;
				const auto value = entry.second;
				CVAR_SET_STRING(name.c_str(), value.c_str());

				auto& cvar = cvars[name];
				cvar.serverValue = value;
			}
		}

		void ApplyGamemodeChanges()
		{
			for (const auto& entry : gamemodeCVars)
			{
				const auto name = entry.first;
				const auto value = entry.second;
				CVAR_SET_STRING(name.c_str(), value.c_str());

				auto& cvar = cvars[name];
				cvar.gamemodeValue = value;
			}
		}

		void ClearStartupDefaultValues()
		{
			for (const auto& entry : cvars)
			{
				auto var = entry.second;
				var.startupValue = "";
			}
		}

		void ClearServerDefaultValues()
		{
			for (const auto& entry : cvars)
			{
				auto var = entry.second;
				var.serverValue = "";
			}
		}

		void ClearGamemodeDefaultValues()
		{
			for (const auto& entry : cvars)
			{
				auto var = entry.second;
				var.gamemodeValue = "";
			}
		}

		void MakeBogus()
		{
			// Just some random number that I can tell it's generated here and it's not a real value
			BOGUS_VALUE = std::to_string(RANDOM_LONG(6000000, 7000000));
			for (const auto entry : cvars)
			{
				CVAR_SET_STRING(entry.first.c_str(), BOGUS_VALUE.c_str());
			}
		}

		void RestoreFromSnapshot()
		{
			for (const auto entry : snapshot)
			{
				CVAR_SET_STRING(entry.first.c_str(), entry.second.c_str());
			}
		}

		void Save(std::unordered_map<std::string, std::string>& cvarMap)
		{
			cvarMap.clear();
			for (const auto entry : cvars)
			{
				const auto value = CVAR_GET_STRING(entry.first.c_str());
				if (!strstr(value, BOGUS_VALUE.c_str()))
				{
					// Assumes MakeBogus() was called previously and then some config(s) was executed
					cvarMap.emplace(entry.first, value);
				}
			}
		}

		void SaveDll()
		{
			dllCVars.clear();
			for (const auto entry : cvars)
			{
				dllCVars.emplace(entry.first, entry.second.dllValue);
			}
		}

		void SaveStartup()
		{
			Save(startupCVars);
		}

		void SaveServer()
		{
			Save(serverCVars);
		}

		void SaveGamemode()
		{
			Save(gamemodeCVars);
		}

		void TakeSnapshot()
		{
			snapshot.clear();
			for (const auto entry : cvars)
			{
				const auto value = CVAR_GET_STRING(entry.first.c_str());
				snapshot.emplace(entry.first, value);
			}
		}
	}

	cvar_t Create(char* name, char* value, int flags, int customFlags)
	{
		cvar_t var = { name, value, flags };

		auto cvars = CVar(name, CustomCVar(var, value, customFlags));

		return var;
	}

	void Load()
	{
		cvars.clear();

		// Pass them from the local context to the outer one, so that
		// we don't have to call CVar() everytime to retrieve them
		cvars = GetCVars();

		TrackEngineCVars();

		SaveDll();
	}

	bool IsTracked(std::string cvarName)
	{
		return cvars.count(cvarName) > 0;
	}

	bool IsVotable(std::string cvarName)
	{
		if (cvars.count(cvarName) == 0)
			return false;

		return cvars[cvarName].customFlags & CCVAR_VOTABLE;
	}

	bool IsBasicSetting(std::string cvarName)
	{
		if (cvars.count(cvarName) == 0)
			return false;

		return cvars[cvarName].customFlags & CCVAR_BASIC_PRIVILEGES;
	}

	std::vector<ChangedCVar> GetChangesOverGamemode()
	{
		std::vector<ChangedCVar> result;

		for (const auto entry : cvars)
		{
			const auto name = entry.first;
			const auto var = entry.second;

			if (!(var.customFlags & CCVAR_GAMEMODE))
				continue;

			const auto currValue = CVAR_GET_STRING(name.c_str());

			// Try to get the default value, in order, from gamemode or server or engine/gamedll
			std::string defaultValue;
			if (gamemodeCVars.count(name) == 1)
				defaultValue = var.gamemodeValue;
			else if (serverCVars.count(name) == 1)
				defaultValue = var.serverValue;
			else if (startupCVars.count(name) == 1)
				defaultValue = var.startupValue;
			else if (dllCVars.count(name) == 1)
				defaultValue = var.dllValue;
			else
				continue;

			if (!FStrEq(currValue, defaultValue.c_str()))
			{
				result.push_back({name, defaultValue, currValue});
			}
		}

		return result;
	}

	std::unordered_map<std::string, std::string> GetGamemodeCVars()
	{
		std::unordered_map<std::string, std::string> result;

		for (const auto entry : gamemodeCVars)
		{
			result.emplace(entry.first, CVAR_GET_STRING(entry.first.c_str()));
		}

		return result;
	}

	void StartRecordingChanges()
	{
		isRecording = true;

        TakeSnapshot();
        MakeBogus();
	}

	void StopRecordingStartupChanges()
	{
		if (snapshot.empty() || !isRecording)
			return; // Nothing was recorded? then avoid messing cvars up

        SaveStartup();
        RestoreFromSnapshot();
		ClearStartupDefaultValues();
        ApplyStartupChanges();

		isRecording = false;
	}

	void StopRecordingServerChanges()
	{
		if (snapshot.empty() || !isRecording)
			return; // Nothing was recorded? then avoid messing cvars up

        SaveServer();
        RestoreFromSnapshot();
		ClearServerDefaultValues();
        ApplyServerChanges();

		isRecording = false;
	}

	void StopRecordingGamemodeChanges()
	{
		if (snapshot.empty() || !isRecording)
			return; // Nothing was recorded? then avoid messing cvars up

        SaveGamemode();
        RestoreFromSnapshot();
		ClearGamemodeDefaultValues();
        ApplyGamemodeChanges();

		isRecording = false;
	}

	void RestoreAllToGamemodeValue()
	{
		for (const auto changedCVar : GetChangesOverGamemode())
		{
			CVAR_SET_STRING(changedCVar.name.c_str(), changedCVar.defaultValue.c_str());
		}
	}
}
