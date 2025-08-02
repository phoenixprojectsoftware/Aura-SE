/****
*
* Copyright (c) 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Firefight: the Monster Spawner.
*
*
****/

#include "agfirefight_spawner.h"
#include "agfirefight.h"
#include "util.h"
#include "monsters.h"

void AgWaveSpawner::SpawnWave(const std::vector<FirefightSpawnInfo>& spawns, AgFirefight* pGameType)
{
	for (const auto& info : spawns)
	{
		for (int i = 0; i < info.iCount; ++i)
		{
			CBaseEntity* pEnt = CBaseEntity::Create(STRING(info.iszMonsterClass), info.vecOrigin, info.vecAngles, NULL);

			if (pEnt)
				pGameType->RegisterSpawnedMonster(pEnt);
		}
	}
}
