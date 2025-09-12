/****
* 
* Copyright (c) 2021-2025 The Phoenix Project Software. Some Rights Reserved.
* 
* AURA
* 
* Firefight: the firefight gametype class.
* 
* 
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamerules.h"
#include "agfirefight.h"
#include "aggamerules.h"
#include "agglobal.h"
#include "monsters.h" // for spawning monsters

// AgFirefight g_AgFirefight;

void AgFirefight::RandomMusic()
{
	m_bMusicSet1 = false;
	m_bMusicSet2 = false;
	m_bMusicSet3 = false;
	m_bMusicSet4 = false;


	int choice = RANDOM_LONG(0, 3);

	switch (choice)
	{
	case 0:
		m_bMusicSet1 = true;
		ALERT(at_console, "SET 1\n");
		break;
	case 1:
		m_bMusicSet2 = true;
		ALERT(at_console, "SET 2\n");
		break;
	case 2:
		m_bMusicSet3 = true;
		ALERT(at_console, "SET 3\n");
		break;
	case 3:
		m_bMusicSet4 = true;
		ALERT(at_console, "SET 4\n");
	}
}

CBaseMonster* UTIL_SpawnMonster(const char* pszClassname, const Vector& vecOrigin, const Vector& vecAngles)
{
	edict_t* pent = CREATE_NAMED_ENTITY(MAKE_STRING(pszClassname));
	if (!pent)
		return nullptr;

	CBaseEntity* pEnt = CBaseEntity::Instance(pent);
	if (!pEnt)
		return nullptr;

	pEnt->pev->origin = vecOrigin;
	pEnt->pev->angles = vecAngles;

	DispatchSpawn(pEnt->edict());

	return pEnt->MyMonsterPointer(); // returns null if not a monster
}

void AgFirefightFileCache::Load()
{
	char szFile[MAX_PATH];
	sprintf(szFile, "%s/ff/%s.ff", AgGetDirectory(), STRING(gpGlobals->mapname));

	FILE* pFile = fopen(szFile, "r");
	if (!pFile)
	{
		ALERT(at_console, "This map does not support Firefight!\n");
		return;
	}

	char line[256];
	while (fgets(line, sizeof(line), pFile))
	{
		if (line[0] == '#' || line[0] == '\n') // skip comments & empty lines
			continue;

		AgFFWaveSpawn spawn;
		sscanf(line, "%d %f %f %f %f %f %f %s %d", &spawn.waveNumber, &spawn.origin.x, &spawn.origin.y, &spawn.origin.z, &spawn.angles.x, &spawn.angles.y, &spawn.angles.z, spawn.monsterClass, &spawn.difficulty);

		m_spawnPoints.push_back(spawn);
	}

	fclose(pFile);
}

const std::vector<AgFFWaveSpawn>& AgFirefightFileCache::GetWaveSpawns(int waveNumber) const
{
	static std::vector<AgFFWaveSpawn> empty;

	// could cache results if needed, for now just filter on the fly
	static std::vector<AgFFWaveSpawn> result;

	result.clear();

	for (const auto& spawn : m_spawnPoints)
	{
		if (spawn.waveNumber == waveNumber)
			result.push_back(spawn);
	}

	return result;
}

AgFirefightFileCache g_FirefightFileCache;

AgFirefight::AgFirefight()
{
	RandomMusic();
	m_flFirstWaveDelay = gpGlobals->time + 45.0f;
	m_bFirstWaveMusicPlayed = false;
	m_State = FF_WAITING;
	m_flNextThinkTime = gpGlobals->time;
	m_flWaveStartTime = 0.0f;
	m_iWaveNumber = 0;
	m_iEnemiesRemaining = 0;
	m_FileCache.Load();
}

AgFirefight::~AgFirefight()
{
}

void AgFirefight::Precache()
{
	m_FileCache.Load();
}

void AgFirefight::Think()
{
	if (gpGlobals->time < m_flNextThinkTime)
		return;

	m_flNextThinkTime = gpGlobals->time + 0.1f; // think every 0.1s

	switch (m_State)
	{
	case FF_WAITING:
		if (UTIL_IsMultiplayer())
		{
			int secondsLeft = (int)(m_flFirstWaveDelay - gpGlobals->time);

			if (!m_bFirstWaveMusicPlayed && secondsLeft < 40) // first tick of countdown
			{
				// send music to all connected clients
				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					edict_t* pPlayer = INDEXENT(i);
					if (!FNullEnt(pPlayer) && pPlayer->v.flags & FL_CLIENT) // ensure this is a valid client
					{
						if (m_bMusicSet1 == true)
							CLIENT_COMMAND(pPlayer, "mp3 play sound/music/FirefightIntroGlue02.mp3\n");
						else if (m_bMusicSet2 == true)
							CLIENT_COMMAND(pPlayer, "mp3 play sound/music/WarGamesFirefightDub.mp3\n");
						else if (m_bMusicSet3 == true)
							CLIENT_COMMAND(pPlayer, "mp3 play sound/music/FirefightIntro1.mp3\n");
						else if (m_bMusicSet4 == true)
							CLIENT_COMMAND(pPlayer, "mp3 play sound/music/Spooky01.mp3\n");
					}
				}

				m_bFirstWaveMusicPlayed = true;
			}

			if (gpGlobals->time >= m_flFirstWaveDelay)
			{
				m_iWaveNumber = 1;
				StartNextWave();
			}
#ifdef _DEBUG // debugger for now cos this will probably cause overflows...
			else
			{
				UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("FIRST WAVE BEGINS IN %d - SCAVENGE WEAPONS FROM THE BATTLEFIELD", secondsLeft));
			}
#endif
		}
		break;

	case FF_SPAWNING:
		SpawnWaveEnemies();
		m_State = FF_FIGHTING;
		break;

	case FF_FIGHTING:
		CheckWaveStatus();
		break;

	case FF_ROUND_OVER:
		if (gpGlobals->time > m_flWaveStartTime + 5.0f)
		{
			++m_iWaveNumber;
			StartNextWave();
		}
		break;

	case FF_GAME_OVER:
		// do intermission here?
		break;
	}
}

void AgFirefight::StartNextWave()
{
	m_Enemies.clear();
	m_State = FF_SPAWNING;
	m_flWaveStartTime = gpGlobals->time;

	// g_FirefightFileCache.PrecacheAllMonsters(); // precache the monsters before spawning them

	// could add dynamic difficulty based on wave number
	UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("Wave %d starting", m_iWaveNumber));
}

void AgFirefight::SpawnWaveEnemies()
{
	m_Enemies.clear();

	const auto& spawns = m_FileCache.GetWaveSpawns(m_iWaveNumber);

	for (const auto& spawn : spawns)
	{
		CBaseMonster* pMonster = UTIL_SpawnMonster(spawn.monsterClass, spawn.origin, spawn.angles);
		if (pMonster)
		{
			EHANDLE h;
			h = pMonster;
			m_Enemies.push_back(h);
		}
	}

	m_iEnemiesRemaining = m_Enemies.size();
}

void AgFirefight::CheckWaveStatus()
{
	m_iEnemiesRemaining = 0;

	for (auto& hEnt : m_Enemies)
	{
		CBaseEntity* pEnt = hEnt;  // EHANDLE -> CBaseEntity*
		if (pEnt && pEnt->IsAlive())
		{
			m_iEnemiesRemaining++;
		}
	}

	if (m_iEnemiesRemaining == 0)
	{
		UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("Wave %d cleared", m_iWaveNumber));
		m_State = FF_ROUND_OVER;
		m_flWaveStartTime = gpGlobals->time;
	}
}

void AgFirefight::GameOver()
{
	UTIL_ClientPrintAll(HUD_PRINTCENTER, "Game over");
	m_State = FF_GAME_OVER;
}

void AgFirefight::EndRound()
{
	// do we need more cleanup here??
	m_State = FF_ROUND_OVER;
}
