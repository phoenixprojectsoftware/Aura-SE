/****
* 
* Copyright © 2021-2025 The Phoenix Project Software. Some Rights Reserved.
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

#include "agglobal.h"
#include "agfirefight.h"
#include "agfirefight_spawner.h"

#include "algo.h"

#include <vector>
#include <algorithm>

extern cvar_t coopmode;
bool m_bCoopStarted = false;

AgFirefight::AgFirefight()
{
	ALERT(at_console, "fart\n");
}

AgFirefight::~AgFirefight()
{

}

void AgFirefight::Init()
{
	ALERT(at_console, "Firefight gametype initialized.\n");
}

std::vector<FirefightSpawnInfo> AgFirefight::GetWaveDefinition(int set, int round, int wave)
{
	std::vector<FirefightSpawnInfo> result;

	if (set == 1 && round == 1 && wave == 1)
	{
		result.push_back(FirefightSpawnInfo{Vector(512,512,0), Vector(0,0,0), MAKE_STRING("monster_hgrunt")});
	}

	return result;
}

void AgFirefight::StartNextWave()
{
	if (m_bInWave)
		return;

	m_bInWave = true;
	m_iCurrentWave++;
	if (m_iCurrentWave > m_iWavesPerSound)
	{
		m_iCurrentWave = 1;
		m_iCurrentRound++;

		if (m_iCurrentRound > m_iRoundsPerSet)
		{
			m_iCurrentRound = 1;
			m_iCurrentSet++;

			if (m_iCurrentSet > m_iMaxSets)
			{
				AgSay(NULL, "All sets completed! Game over.");
				// TODO: SET INTERMISSION  
				return;
			}

			char setbuffer[128];
			snprintf(setbuffer, sizeof(setbuffer), "Set %d begins.", m_iCurrentSet);
			AgSay(NULL, setbuffer);
		}

		char roundbuffer[128];
		snprintf(roundbuffer, sizeof(roundbuffer), "Round %d begins.", m_iCurrentRound);
		AgSay(NULL, roundbuffer);
	}
	char wavebuffer[128];
	snprintf(wavebuffer, sizeof(wavebuffer), "Wave %d begins.", m_iCurrentWave);
	AgSay(NULL, wavebuffer);

	auto wave = GetWaveDefinition(m_iCurrentSet, m_iCurrentRound, m_iCurrentWave);
	AgWaveSpawner::SpawnWave(wave, this);
}

void AgFirefight::CheckWaveComplete()
{
	m_vecMonstersAlive.erase(
		std::remove_if(m_vecMonstersAlive.begin(), m_vecMonstersAlive.end(),
			[](EHANDLE& h) { return h.Get() == nullptr; }),
		m_vecMonstersAlive.end());

	if (m_vecMonstersAlive.empty())
	{
		m_bInWave = false;
		AgSay(NULL, "Wave cleared!");

		// Delay before next wave
		SetThink(&AgFirefight::StartNextWave);
		pev->nextthink = gpGlobals->time + 5.0f;
	}
}


void AgFirefight::RegisterSpawnedMonster(CBaseEntity* pEnt)
{
	EHANDLE handle;
	handle = pEnt;

	m_vecMonstersAlive.push_back(handle);
}

void AgFirefight::OnMonsterDied(CBaseEntity* pEnt)
{
	m_vecMonstersAlive.erase(
		std::remove_if(
			m_vecMonstersAlive.begin(),
			m_vecMonstersAlive.end(),
			[pEnt](EHANDLE& h)
			{
				return h == pEnt;
			}
		), m_vecMonstersAlive.end());

	CheckWaveComplete();
}

void AgFirefight::PlayerDied(CBasePlayer* pPlayer)
{
	if (--m_iSharedLives <= 0)
	{
		AgSay(NULL, "All lives lost! Game over.");
		// TODO: SET INTERMISSION
	}
	else
	{
		char pdownbuffer[128];
		snprintf(pdownbuffer, sizeof(pdownbuffer), "Player down! Shared lives remaining: %d", m_iSharedLives);
		AgSay(NULL, pdownbuffer);
	}
}

void AgFirefight::Start()
{
	AgSay(NULL, "WELCOME TO FIREFIGHT!");
	StartNextWave();
}

void AgFirefight::Think()
{
	if( !m_bCoopStarted)
		Start();
	m_bCoopStarted = true;
}