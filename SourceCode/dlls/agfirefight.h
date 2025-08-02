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

#ifndef AGFIREFIGHT_H
#define AGFIREFIGHT_H

#pragma once

#include <vector>
#include <map>

struct FirefightSpawnInfo
{
	Vector vecOrigin;
	Vector vecAngles;
	string_t iszMonsterClass;
	int iCount = 1;
};

class AgFirefight : public CBaseEntity
{
public:
	void Spawn() override;
	void Think() override;

	void Init();
	void Start(); // starts the first wave
	void StartNextWave();
	void CheckWaveComplete();

	void RegisterSpawnedMonster(CBaseEntity* pEnt);
	void OnMonsterDied(CBaseEntity* pEnt);
	void PlayerDied(CBasePlayer* pPlayer);

	bool LoadWaveDefinitions();
	std::vector<FirefightSpawnInfo> GetWaveDefinition(int set, int round, int wave);

private:
	bool m_bInWave = false;
	bool m_bStarted = false;

	int m_iSharedLives = 10;

	int m_iCurrentSet = 1;
	int m_iCurrentRound = 1;
	int m_iCurrentWave = 0;

	int m_iMaxSets = 1;
	int m_iRoundsPerSet = 1;
	int m_iWavesPerRound = 10;

	std::map<int, std::vector<FirefightSpawnInfo>> m_WaveMap;
	std::vector<EHANDLE> m_vecMonstersAlive;
};

extern AgFirefight* g_pFirefightController;

#endif // AGFIREFIGHT_H
