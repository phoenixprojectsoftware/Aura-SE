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
		UTIL_PrecacheOther("monster_hgrunt");
	}
	else if (FStrEq(pszMonsterClass, "monster_alien_grunt"))
	{
		UTIL_PrecacheOther("monster_alien_grunt");
	}
	else if (FStrEq(pszMonsterClass, "monster_gargantua"))
	{
		UTIL_PrecacheOther("monster_gargantua");
	}
	else if (FStrEq(pszMonsterClass, "monster_bullchicken"))
	{
		UTIL_PrecacheOther("monster_bullchicken");
	}
	else if (FStrEq(pszMonsterClass, "monster_zombie"))
	{
		UTIL_PrecacheOther("monster_zombie");
	}
	else if (FStrEq(pszMonsterClass, "monster_zombie_torso"))
	{
		UTIL_PrecacheOther("monster_zombie_torso");
	}
	else if (FStrEq(pszMonsterClass, "monster_headcrab"))
	{
		UTIL_PrecacheOther("monster_headcrab");
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