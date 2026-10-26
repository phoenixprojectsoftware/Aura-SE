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

#ifndef FIREFIGHT_H
#define FIREFIGHT_H

#pragma once

#include <vector>
#include <string>
#include <set>

enum FirefightState
{
	FF_WAITING,
	FF_SPAWNING,
	FF_FIGHTING,
	FF_ROUND_OVER,
	FF_GAME_OVER
};

struct AgFFWaveSpawn
{
	int waveNumber;
	Vector origin;
};

struct ActiveSpawn
{
	Vector origin;
	int remaining;
};

class AgFirefightFileItem
{
public:
	AgFirefightFileItem();
	~AgFirefightFileItem();

	char m_szName[64];
	Vector m_vOrigin;
	Vector m_vAngles;
	char m_szWaveID[32];
};

class AgFirefightFileCache
{
public:
	AgFirefightFileCache();
	~AgFirefightFileCache();

	void Load();
	void PrecacheAllMonsters();
	const std::vector<AgFFWaveSpawn>& GetWaveSpawns(int waveNumber) const;
	int GetMaxWave() const;

private:
	std::vector<AgFirefightFileItem*> m_lstFileItems;
	std::set<std::string> m_PrecachedMonsters;
	std::vector<AgFFWaveSpawn> m_spawnPoints;
};


class AgFirefight
{
public:
	AgFirefight();
	~AgFirefight();

	void Precache();
	void Think();

	void SendCommand(const char* cmd,  ...);
	void PlayMusic(int state);

	void SetAuthoringWave(int wave);
	int GetAuthoringWave() const { return m_iAuthoringWave; }

	void OnMonsterKilled(CBaseMonster* pMonster);

	void InitHUD(CBasePlayer* pPlayer);

	bool m_bAuthoring = false;

	bool IsFirefightMonster(CBaseMonster* pMonster) const;
	const char* GetWaveMonsterName() const;

	void UpdateMonsterTargets();
	bool IsValidFirefightTarget(CBaseEntity* pEnt) const;
	CBasePlayer* FindNearestPlayer(const Vector& origin) const;

private:
	int GetSpawnsPerPoint(int wave) const;
	Vector RandomMonsterAngles();
	const char* PickRandomMonster();
	void RandomMusic();
	void StartNextWave();
	void SpawnWaveEnemies();
	void TrySpawnNext();
	void CheckWaveStatus();
	void EndRound();
	void GameOver();

	void UpdateFinalEnemyWaypoints();
	void SendFinalEnemyWaypoints(CBasePlayer* pPlayer = NULL);
	void ClearFinalEnemyWaypoints();

	bool m_bMusicSet1; // War Games
	bool m_bMusicSet2; // Set 2 (stub)
	bool m_bMusicSet3; // Set 3 (stub)
	bool m_bMusicSet4; // spooky01

	float m_flFirstWaveDelay;
	bool m_bFirstWaveMusicPlayed;
	float m_flNextThinkTime;
	float m_flWaveStartTime;
	int m_iWaveNumber;
	int m_iEnemiesRemaining;

	int m_iAuthoringWave = 1;

	FirefightState m_State;

	std::vector<EHANDLE> m_Enemies;
	AgFirefightFileCache m_FileCache;

	std::vector<ActiveSpawn> m_ActiveSpawns;
	int m_iAliveMonsters;

	int m_iWaypointTargets[3];
	int m_iWaypointTargetCount;
};

extern AgFirefight g_AgFirefight;

#endif // FIREFIGHT_H
