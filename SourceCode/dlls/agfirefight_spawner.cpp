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

#include "agfirefight_spawner.h"

void AgWaveSpawner::SpawnWave(const std::vector<FirefightSpawnInfo>& waveSpawns, AgFirefight* pGameType)  
{  
	if (!pGameType || waveSpawns.empty())  
		return;  

	for (const auto& spawn : waveSpawns)  
	{  
		for (int i = 0; i < spawn.iCount; ++i)  
		{  
			CBaseEntity* pEnt = CBaseEntity::Create(const_cast<char*>STRING(spawn.iszMonsterClass), spawn.vecOrigin, spawn.vecAngles, nullptr);   
		}  
	}  
}