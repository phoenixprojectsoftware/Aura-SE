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
#ifndef AGFIREFIGHT_SPAWNER_H
#define AGFIREFIGHT_SPAWNER_H

#pragma once

#include <vector>

class AgFirefight;
struct FirefightSpawnInfo;

class AgWaveSpawner
{
public:
	static void SpawnWave(const std::vector<FirefightSpawnInfo>& spawns, AgFirefight* pGameType);
};

#endif
