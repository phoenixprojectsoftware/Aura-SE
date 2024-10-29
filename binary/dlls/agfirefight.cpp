/****
* 
* Copyright © 2021-2024 The Phoenix Project Software. Some Rights Reserved.
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
#include "player.h"
#include "weapons.h"

#include "aggamerules.h"
#include "agglobal.h"
#include "agfirefight.h"

#include "algo.h"

extern cvar_t coopmode;

AgFirefight::AgFirefight()
{

}

AgFirefight::~AgFirefight()
{

}

void AgFirefight::InitMonsters()
{
	edict_t* pEdict = FIND_ENTITY_BY_STRING(nullptr, "targetname", "FIREFIGHT");
	while (!FNullEnt(pEdict))
	{
		RespawnInfo respawnInfo;
		respawnInfo.edict = pEdict;
		respawnInfo.spawnPosition = pEdict->v.origin; // Store original position
		respawnInfo.respawnTime = 0.0f; // Initialize respawn time to 0

		m_respawnQueue.push_back(respawnInfo);
		pEdict = FIND_ENTITY_BY_STRING(pEdict, "targetname", "FIREFIGHT");
	}
}

void AgFirefight::Think()
{
    float currentTime = gpGlobals->time;

    for (auto& respawnInfo : m_respawnQueue) {
        if (respawnInfo.respawnTime > 0.0f && respawnInfo.respawnTime <= currentTime) {
            // Respawn the monster
            CBaseEntity* pNewMonster = CBaseEntity::Instance(g_engfuncs.pfnCreateNamedEntity(MAKE_STRING("monster_name")));
            if (pNewMonster) {
                pNewMonster->pev->origin = respawnInfo.spawnPosition; // Set the spawn position
                pNewMonster->Spawn(); // Spawn the new monster

                // Reset the respawn timer
                respawnInfo.respawnTime = 0.0f;
            }
        }

        // Check if the monster is dead
        if (respawnInfo.edict->free) {
            respawnInfo.respawnTime = currentTime + 5.0f; // Set respawn time after 5 seconds
        }
    }

    CGameRules::Think();
}