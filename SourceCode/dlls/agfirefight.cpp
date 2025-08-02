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
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

#include "agfirefight.h"
#include "agfirefight_spawner.h"

AgFirefight* g_pFirefightController = nullptr;

void AgFirefight::Spawn()
{
	pev->nextthink = gpGlobals->time + 0.1f;
	SetThink(&AgFirefight::Think);

	Init();
}

void AgFirefight::Init()
{
	ALERT(at_console, "[Firefight] Initializing...\n");

	g_pFirefightController = this;

	m_bStarted = false;
	m_bInWave = false;

	m_iSharedLives = 10;

	m_iCurrentSet = 1;
	m_iCurrentRound = 1;
	m_iCurrentWave = 0;

	m_iMaxSets = 1;
	m_iRoundsPerSet = 1;
	m_iWavesPerRound = 10;

	m_vecMonstersAlive.clear();
	m_WaveMap.clear();

	LoadWaveDefinitions();
}

void AgFirefight::Think()
{
	if (!m_bStarted)
	{
		Start();
		m_bStarted = true;
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

void AgFirefight::Start()
{
	AgSay(NULL, "\n>> WELCOME TO FIREFIGHT <<\n");
	StartNextWave();
}

void AgFirefight::StartNextWave()
{
	if (m_bInWave)
		return;

	m_iCurrentWave++;
	if (m_iCurrentWave > m_iWavesPerRound)
	{
		m_iCurrentWave = 1;
		m_iCurrentRound++; 

		if (m_iCurrentRound > m_iRoundsPerSet)
		{
			m_iCurrentRound = 1;
			m_iCurrentSet++;

			if (m_iCurrentSet > m_iMaxSets)
			{
				AgSay(NULL, "Game over");
				return;
			}
			AgSay(NULL, UTIL_VarArgs("Set %d begins!", m_iCurrentSet));
		}
		AgSay(NULL, UTIL_VarArgs("Round %d begins!", m_iCurrentRound));
	}
	AgSay(NULL, UTIL_VarArgs("Wave %d begins!", m_iCurrentWave));

	m_bInWave = true;
	auto wave = GetWaveDefinition(m_iCurrentSet, m_iCurrentRound, m_iCurrentWave);
	AgWaveSpawner::SpawnWave(wave, this);
}

void AgFirefight::RegisterSpawnedMonster(CBaseEntity* pEnt)
{
	if (pEnt)
		m_vecMonstersAlive.push_back(EHANDLE(pEnt));
}

void AgFirefight::OnMonsterDied(CBaseEntity* pEnt)
{
	m_vecMonstersAlive.erase(std::remove_if(m_vecMonstersAlive.begin(), m_vecMonstersAlive.end(), [pEnt](EHANDLE& h) { return h == pEnt || h.Get() == nullptr;  }), m_vecMonstersAlive.end());

	CheckWaveComplete();
}

void AgFirefight::CheckWaveComplete()
{
	m_vecMonstersAlive.erase(std::remove_if(m_vecMonstersAlive.begin(), m_vecMonstersAlive.end(), [](EHANDLE& h) { return h.Get() == nullptr;  }), m_vecMonstersAlive.end());

	if (m_vecMonstersAlive.empty())
	{
		m_bInWave = false;
		AgSay(NULL, "Wave cleared!");

		SetThink(&AgFirefight::StartNextWave);
		pev->nextthink = gpGlobals->time + 5.0f; // 5 second delay before next wave begins
	}
}

void AgFirefight::PlayerDied(CBasePlayer* pPlayer)
{
	if (--m_iSharedLives <= 0)
	{
		AgSay(NULL, "Game over. All lives lost.");
		// TODO: Trigger intermission here.
	}
	else
	{
		AgSay(NULL, UTIL_VarArgs("Player down! Shared lives remaining: %d", m_iSharedLives));
	}
}

std::vector<FirefightSpawnInfo> AgFirefight::GetWaveDefinition(int set, int round, int wave)
{
	int index = (set - 1) * m_iRoundsPerSet * m_iWavesPerRound + (round - 1) * m_iWavesPerRound + wave;
	auto it = m_WaveMap.find(index);
	if (it != m_WaveMap.end())
		return it->second;

	return{};
}

bool AgFirefight::LoadWaveDefinitions()
{
	// STUB
	// TODO: parse ff/<mapname>.ff and populate m_WaveMap. . .
	AgSay(NULL, "[Firefight] No wave defs loaded, parser not implemented.\n");
	ALERT(at_console, "[FIREFIGHT] No wave defs loaded, parser not implemented.\n");
	return false;
}
