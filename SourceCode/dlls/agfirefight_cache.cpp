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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "agfirefight.h"
#include <set>
#include <string>

AgFirefightFileItem::AgFirefightFileItem()
{
	m_szName[0] = '\0';
	m_szWaveID[0] = '\0';
	m_vOrigin = g_vecZero;
	m_vAngles = g_vecZero;
}

AgFirefightFileItem::~AgFirefightFileItem()
{

}

AgFirefightFileCache::AgFirefightFileCache()
{

}

AgFirefightFileCache::~AgFirefightFileCache()
{

}

void PrecacheMonsterResources(const char* pszMonsterClass)
{
	UTIL_PrecacheOther(pszMonsterClass);
}

void AgFirefightFileCache::PrecacheAllMonsters()
{
	for (auto* pItem : m_lstFileItems)
	{
		if (!pItem || !pItem->m_szName[0])
			continue;

		const std::string monsterClass = pItem->m_szName;

		if (m_PrecachedMonsters.find(monsterClass) == m_PrecachedMonsters.end())
		{
			PrecacheMonsterResources(monsterClass.c_str());
			m_PrecachedMonsters.insert(monsterClass);
		}
	}
}

int AgFirefightFileCache::GetMaxWave() const
{
	int maxWave = 0;

	for (const auto& sp : m_spawnPoints)
	{
		if (sp.waveNumber > maxWave)
			maxWave = sp.waveNumber;
	}

	return maxWave;
}