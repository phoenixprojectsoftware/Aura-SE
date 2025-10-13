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
#pragma once
#include <string>
#include <map>

class CMusicSystem
{
public:
	void Init();
	void Play();
	void Stop();
	void OnClientConnect(edict_t* pPlayer);

private:
	void LoadFile(char* pszPath);
	void BroadcastCommand(char* pszCmd);
	void SendToClient(edict_t* pPlayer, char* pszCmd);

	std::string m_Command;
	bool m_bIsPlaying = false;
};

extern CMusicSystem g_MusicSystem;