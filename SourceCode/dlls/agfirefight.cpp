/****
* 
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
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

bool IsSpawnBlockedByPlayer(const Vector& origin)
{
	const float radius = 64.0f;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		edict_t* pPlayer = INDEXENT(i);
		if (FNullEnt(pPlayer))
			continue;

		if (!(pPlayer->v.flags & FL_CLIENT))
			continue;

		if (!pPlayer->v.deadflag)
		{
			if ((pPlayer->v.origin - origin).Length() < radius)
				return true;
		}
	}

	return false;
}

void FF_SpawnTeleportEffect(const Vector& origin)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_TELEPORT);
		WRITE_COORD(origin.x);
		WRITE_COORD(origin.y);
		WRITE_COORD(origin.z);
	MESSAGE_END();
}

const char* easyMonsters[] = {
	"monster_zombie",
	"monster_headcrab",
	"monster_alien_slave",
	"monster_bullsquid",
	"monster_houndeye"
};
const char* hardMonsters[] = {
	"monster_hgrunt",
	"monster_hassassin",
	"monster_agrunt",
	"monster_controller"
};

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
	FF_SpawnTeleportEffect(vecOrigin);
	EMIT_SOUND(pEnt->edict(), CHAN_BODY, "player/friend_online.wav", VOL_NORM, ATTN_NORM);

	return pEnt->MyMonsterPointer(); // returns null if not a monster
}

void AgFirefightFileCache::Load()
{
	m_spawnPoints.clear();

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
		if (line[0] == '#' || line[0] == '\n')
			continue;

		AgFFWaveSpawn spawn;
		if (sscanf(line, "%d %f %f %f", &spawn.waveNumber, &spawn.origin.x, &spawn.origin.y, &spawn.origin.z) == 4)
		{
			m_spawnPoints.push_back(spawn);
		}
	}

	fclose(pFile);
}

Vector AgFirefight::RandomMonsterAngles()
{
	return Vector(0.0f, RANDOM_FLOAT(0.0f, 360.0f), 0.0f);
}

const char* AgFirefight::PickRandomMonster()
{
	bool bHard = (m_iWaveNumber >= 5);

	if (!bHard)
	{
		int count = ARRAYSIZE(easyMonsters);
		return easyMonsters[RANDOM_LONG(0, count - 1)];
	}
	else
	{
		int count = ARRAYSIZE(hardMonsters);
		return hardMonsters[RANDOM_LONG(0, count - 1)];
	}
}

void AgFirefight::SetAuthoringWave(int wave)
{
	if (wave < 0)
		wave = 0;

	m_iAuthoringWave = wave;
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
	m_iAliveMonsters = 0;
	m_FileCache.Load(); // no duplicate needed. maybe.
}

int AgFirefight::GetSpawnsPerPoint(int wave) const
{
	return 1;
}

AgFirefight::~AgFirefight()
{
}

void AgFirefight::Precache()
{
	// m_FileCache.Load();
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
			PlayMusic(FirefightState::FF_WAITING);

			if (gpGlobals->time >= m_flFirstWaveDelay)
			{
				m_iWaveNumber = 1;
				StartNextWave();
			}
#ifdef _DEBUG // debugger for now cos this will probably cause overflows...
			else
			{
				UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("FIRST WAVE BEGINS IN %d", secondsLeft));
			}
#endif
		}
		break;

	case FF_SPAWNING:
		TrySpawnNext();
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

void AgFirefight::SendCommand(const char* cmd, ...)
{
	char* szFmt = (char*)cmd;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		edict_t* pPlayer = INDEXENT(i);

		if (!FNullEnt(pPlayer) && pPlayer->v.flags & FL_CLIENT)
			CLIENT_COMMAND(pPlayer, szFmt);
	}
}

void AgFirefight::PlayMusic(int state)
{
	int secondsLeft = (int)(m_flFirstWaveDelay - gpGlobals->time);
	if (state == FirefightState::FF_WAITING)
	{
		if (!m_bFirstWaveMusicPlayed && secondsLeft < 40) // first tick of countdown
		{
			if (m_bMusicSet1 == true)
				SendCommand("mp3 play sound/music/MX_SK_GLU02.mp3\n");
			else if (m_bMusicSet2 == true)
				SendCommand("mp3 play sound/music/MX_C4_DUB.mp3\n");
			else if (m_bMusicSet3 == true)
				SendCommand("mp3 play sound/music/MX_SK_INT.mp3\n");
			else if (m_bMusicSet4 == true)
				SendCommand("mp3 play sound/music/MX_SPACEY02_DUB.mp3\n");
			m_bFirstWaveMusicPlayed = true;
		}
	}

	if (state == 5)
		SendCommand("mp3 play sound/music/MX_OFF2_P2_FULL.mp3\n");
}

void AgFirefight::TrySpawnNext()
{
	for (auto& sp : m_ActiveSpawns)
	{
		if (sp.remaining <= 0)
			continue;

		if (IsSpawnBlockedByPlayer(sp.origin))
			continue;

		const char* pszMonster = PickRandomMonster();
		Vector angles = RandomMonsterAngles();

		CBaseMonster* pMonster = UTIL_SpawnMonster(pszMonster, sp.origin, angles);

		if (!pMonster)
			continue;

		EHANDLE h;
		h = pMonster;
		m_Enemies.push_back(h);

		sp.remaining--;
		m_iAliveMonsters++;
		return; // one spawn per tick.
	}
}

void AgFirefight::StartNextWave()
{
	m_Enemies.clear();
	m_ActiveSpawns.clear();
	m_iAliveMonsters = 0;
	m_State = FF_SPAWNING;
	m_flWaveStartTime = gpGlobals->time;

	// if we are out of waves in the .ff file, go back to the start.
	int maxWave = m_FileCache.GetMaxWave();

	if (m_iWaveNumber > maxWave)
	{
		ALERT(at_console, "FF: looping waves (%d -> 1)\n", m_iWaveNumber);
		m_iWaveNumber = 1;
	}

	const auto& spawns = m_FileCache.GetWaveSpawns(m_iWaveNumber);
	int repeats = GetSpawnsPerPoint(m_iWaveNumber);

	for (const auto& s : spawns)
	{
		ActiveSpawn sp;
		sp.origin = s.origin;
		sp.remaining = repeats;
		m_ActiveSpawns.push_back(sp);
	}

	for (int i = 0; i < 2; ++i)
		TrySpawnNext();

	m_State = FF_FIGHTING;
	m_flWaveStartTime = gpGlobals->time;

	// could add dynamic difficulty based on wave number
	UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("Wave %d starting", m_iWaveNumber));

	if (m_iWaveNumber == 5)
		PlayMusic(5);
}

void AgFirefight::SpawnWaveEnemies()
{
	m_Enemies.clear();

	const auto& spawns = m_FileCache.GetWaveSpawns(m_iWaveNumber);

	for (const auto& spawn : spawns)
	{
		const char* pszMonster = PickRandomMonster();
		Vector vecAngles = RandomMonsterAngles();

		CBaseMonster* pMonster = UTIL_SpawnMonster(pszMonster, spawn.origin, vecAngles);

		if (pMonster)
		{
			EHANDLE h;
			h = pMonster;
			m_Enemies.push_back(h);
		}
	}

	m_iEnemiesRemaining = m_Enemies.size();
}

void AgFirefight::OnMonsterKilled(CBaseMonster* pMonster)
{
	m_iAliveMonsters--;
	if (m_iAliveMonsters < 0)
		m_iAliveMonsters = 0;

#ifdef _DEBUG
	ALERT(at_console, "Firefight monster killed\n");
#endif

	pMonster->GibMonster();
	//pMonster->FadeMonster();

	TrySpawnNext();
}

void AgFirefight::CheckWaveStatus()
{
	TrySpawnNext();

	if (m_iAliveMonsters > 0)
		return;

	for (const auto& sp : m_ActiveSpawns)
	{
		if (sp.remaining > 0)
			return;
	}

	UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("Wave %d cleared", m_iWaveNumber));

	m_State = FF_ROUND_OVER;
	m_flWaveStartTime = gpGlobals->time;
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
