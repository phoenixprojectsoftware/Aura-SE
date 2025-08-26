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
	Vector angles;
	char monsterClass[64];
	int difficulty;
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

private:
	void StartNextWave();
	void SpawnWaveEnemies();
	void CheckWaveStatus();
	void EndRound();
	void GameOver();

	float m_flFirstWaveDelay;
	bool m_bFirstWaveMusicPlayed;
	float m_flNextThinkTime;
	float m_flWaveStartTime;
	int m_iWaveNumber;
	int m_iEnemiesRemaining;

	FirefightState m_State;

	std::vector<EHANDLE> m_Enemies;
	AgFirefightFileCache m_FileCache;
};

extern AgFirefight g_AgFirefight;

#endif // FIREFIGHT_H
