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
#ifndef AGFIREFIGHT_SPAWNER_H
#define AGFIREFIGHT_SPAWNER_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "extdll.h"
#include "vector.h"
#include <vector>

class AgFirefight;

struct FirefightSpawnInfo
{
	Vector vecOrigin;
	Vector vecAngles;
	string_t iszMonsterClass;
	int iCount = 1;
};

class AgWaveSpawner
{
public:
	static void SpawnWave(const std::vector<FirefightSpawnInfo>& waveSpawns, AgFirefight* pGameType);
};
#endif // AGFIREFIGHT_SPAWNER_H