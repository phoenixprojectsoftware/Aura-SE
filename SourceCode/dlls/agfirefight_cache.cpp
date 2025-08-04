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
	if (FStrEq(pszMonsterClass, "monster_hgrunt"))
	{
		PRECACHE_MODEL("models/hgrunt.mdl");

		PRECACHE_SOUND("hgrunt/gr_mgun1.wav");
		PRECACHE_SOUND("hgrunt/gr_mgun2.wav");

		PRECACHE_SOUND("hgrunt/gr_die1.wav");
		PRECACHE_SOUND("hgrunt/gr_die2.wav");
		PRECACHE_SOUND("hgrunt/gr_die3.wav");

		PRECACHE_SOUND("hgrunt/gr_pain1.wav");
		PRECACHE_SOUND("hgrunt/gr_pain2.wav");
		PRECACHE_SOUND("hgrunt/gr_pain3.wav");
		PRECACHE_SOUND("hgrunt/gr_pain4.wav");
		PRECACHE_SOUND("hgrunt/gr_pain5.wav");

		PRECACHE_SOUND("hgrunt/gr_reload1.wav");

		PRECACHE_SOUND("weapons/glauncher.wav");

		PRECACHE_SOUND("weapons/sbarrel1.wav");

		PRECACHE_SOUND("zombie/claw_miss2.wav");// because we use the basemonster SWIPE animation event

		PRECACHE_SOUND("hgrunt/gr_kick.wav");
	}
	else if (FStrEq(pszMonsterClass, "monster_alien_grunt"))
	{
		PRECACHE_MODEL("models/agrunt.mdl");
		PRECACHE_SOUND("hassault/hw_shoot1.wav");
	}
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