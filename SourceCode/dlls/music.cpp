/****
*
* Copyright (c) 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* music system
*
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "agglobal.h"
#include "gamerules.h"
#include "aggamerules.h"
#include "music.h"

CMusicSystem g_MusicSystem;

char* AgGametypeName(float gametype)
{
	switch (AgGametype())
	{
	case ARENA: return "arena";
	case CTF: return "ctf";
	case LMS: return "lms";
	case ARCADE: return "arcade";
	case SGBOW: return "sgbow";
	case INSTAGIB: return "instagib";
	case DOM: return "dom";
	case HIDEANDSEEK: return "hideandseek";
	case FIREFIGHT: return "firefight";
	case FIESTAFIGHT: return "fiestafight";
	case FIESTA: return "fiesta";
	case SWAT: return "swat";
	case HLDM: return "hldm";
	case BUSTERS: return "busters";
	case CHILL: return "chill";
	case STANDARD:
	default:
		return "none";
	}
}

void CMusicSystem::Init()
{
	char szPath[256];
	char* pszGametype = AgGametypeName(AgGametype());

	// try gametype file first
	snprintf(szPath, sizeof(szPath), "mus/%s.mus", pszGametype);
	ALERT(at_console, "CMusicSystem: loading music file for gametype %s\n", pszGametype);
	if (!LOAD_FILE_FOR_ME(szPath, nullptr))
	{
		// try map
		snprintf(szPath, sizeof(szPath), "mus/%s.mus", STRING(gpGlobals->mapname));
		ALERT(at_console, "CMusicSystem: failed to load music file for gametype %s, trying map %s\n", pszGametype, STRING(gpGlobals->mapname));
		if (!LOAD_FILE_FOR_ME(szPath, nullptr))
		{
			//fallback to default
			snprintf(szPath, sizeof(szPath), "mus/default.mus");
			ALERT(at_console, "CMusicSystem: failed to load music file for gametype %s and map %s, falling back to default\n", pszGametype, STRING(gpGlobals->mapname));
		}
	}

	LoadFile(szPath);
}

void CMusicSystem::LoadFile(char* pszPath)
{
	int fileSize;
	char* pFile = (char*)LOAD_FILE_FOR_ME(pszPath, &fileSize);
	if (!pFile)
	{
		ALERT(at_console, "CMusicSystem: failed to load music file %s\n", pszPath);
		return;
	}

	// ensure null termination
	std::string buffer(pFile, fileSize);
	buffer.push_back('\0');
	FREE_FILE(pFile);

	char* ctx = nullptr;
	char* line = strtok_s(&buffer[0], "\r\n", &ctx);

	while (line)
	{
		if (*line == '#' || !*line)
		{
			line = strtok_s(nullptr, "\r\n", &ctx);
			continue;
		}

		char buf[256];
		snprintf(buf, sizeof(buf), "mp3 play sound/music/%s\n", line);
		m_Command = buf;
		break;
	}
}


void CMusicSystem::BroadcastCommand(char* pszCmd)
{
	if (!pszCmd || !*pszCmd)
		return;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{ 
		edict_t* pPlayer = INDEXENT(i);
		if (!FNullEnt(pPlayer) && pPlayer->v.flags & FL_CLIENT)
			CLIENT_COMMAND(pPlayer, "%s\n", pszCmd);
	}
}

void CMusicSystem::SendToClient(edict_t* pPlayer, char* pszCmd)
{
	if (!pszCmd || !*pszCmd || FNullEnt(pPlayer))
		return;

	if (pPlayer->v.flags & FL_CLIENT)
		CLIENT_COMMAND(pPlayer, "%s\n", pszCmd);
}

void CMusicSystem::Play()
{
	if (!m_Command.empty())
	{
		BroadcastCommand((char*)m_Command.c_str());
		ALERT(at_console, "CMusicSystem: playing music command: %s\n", m_Command.c_str());
		m_bIsPlaying = true;
	}
}

void CMusicSystem::Stop()
{
	BroadcastCommand("mp3 stop");
	m_bIsPlaying = false;
}

void CMusicSystem::OnClientConnect(edict_t* pPlayer)
{
	if (m_bIsPlaying && !m_Command.empty())
		SendToClient(pPlayer, (char*)m_Command.c_str());
}