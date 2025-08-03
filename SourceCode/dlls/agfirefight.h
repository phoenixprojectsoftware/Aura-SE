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

#ifndef FIREFIGHT_H
#define FIREFIGHT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER

#include <vector>

class CBaseEntity;
class CBasePlayer;
struct FirefightSpawnInfo;

class AgFirefight : public CBaseEntity
{
public:
	AgFirefight();
	virtual ~AgFirefight();

	void Think();
	void Init();
	void Start();
	void PlayerDied(CBasePlayer* pPlayer);

	void StartNextWave();
	void RegisterSpawnedMonster(CBaseEntity* pEnt);
	void OnMonsterDied(CBaseEntity* pEnt);
	void CheckWaveComplete();

	std::vector<FirefightSpawnInfo> GetWaveDefinition(int set, int round, int wave);

private:
	int m_iCurrentSet = 1;
	int m_iCurrentRound = 1;
	int m_iCurrentWave = 0;

	int m_iMaxSets = 3;
	int m_iRoundsPerSet = 3;
	int m_iWavesPerSound = 5;

	int m_iSharedLives = 7;
	bool m_bInWave = false;

	std::vector<EHANDLE> m_vecMonstersAlive;
};

#endif // FIREFIGHT_H