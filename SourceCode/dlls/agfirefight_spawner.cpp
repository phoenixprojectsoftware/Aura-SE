/****
*
* Copyright © 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Firefight: the Monster Spawner.
*
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "enginecallback.h"

#include "agfirefight_spawner.h"
#include "agfirefight.h"

void AgWaveSpawner::SpawnWave(const std::vector<FirefightSpawnInfo>& waveSpawns, AgFirefight* pGameType)  
{  
	if (!pGameType || waveSpawns.empty())  
		return;  

	for (const auto& spawn : waveSpawns)  
	{  
		for (int i = 0; i < spawn.iCount; ++i)  
		{  
			CBaseEntity* pEnt = CBaseEntity::Create(const_cast<char*>(STRING(spawn.iszMonsterClass)), spawn.vecOrigin, spawn.vecAngles, nullptr);   
		}  
	}  
}