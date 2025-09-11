/****
*
* Copyright (c) 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Busters Gametype
*
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "multiplay_gamerules.h"

#include "aggamerules.h"
#include "agglobal.h"
#include "agbusters.h"

#include "algo.h"

#define EGON_BUSTING_TIME 10

bool IsBustingGame()
{
		return (AgGametype() == BUSTERS);
}

bool IsPlayerBusting(CBaseEntity* pPlayer)
{
	if (!pPlayer || !pPlayer->IsPlayer() || !IsBustingGame())
		return false;

	return ((CBasePlayer*)pPlayer)->HasPlayerItemFromID(WEAPON_EGON);
}

bool BustingCanHaveItem(CBasePlayer* pPlayer, CBaseEntity* pItem)
{
	bool bIsWeaponOrAmmo = false;

	if (strstr(STRING(pItem->pev->classname), "weapon_") || strstr(STRING(pItem->pev->classname), "ammo_"))
		bIsWeaponOrAmmo = true;

	// busting players can't have ammo nor weapons
	if (IsPlayerBusting(pPlayer) && bIsWeaponOrAmmo)
		return false;

	return true;
}

AgBusters::AgBusters()
{
	m_flEgonBustingCheckTime = -1;
}

void AgBusters::Think()
{
	CheckForEgons();
}

int AgBusters::IPointsForKill(CBasePlayer* pAttacker, CBasePlayer* pKilled)
{
	// if the attacker is busting, they get a point per kill.
	if (IsPlayerBusting(pAttacker))
		return 1;

	// if the victim is busting, the attacker gets two points.
	if (IsPlayerBusting(pKilled))
		return 2;

	return 0;
}

void AgBusters::PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor)
{
	if (IsPlayerBusting(pVictim))
	{
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "THE BUSTER HAS BEEN KILLED!");

		// reset egon check time
		m_flEgonBustingCheckTime = -1.0f;

		CBasePlayer* peKiller = NULL;
		CBaseEntity* ktmp = CBaseEntity::Instance(pKiller);

		if (ktmp && (ktmp->Classify() == CLASS_PLAYER))
			peKiller = (CBasePlayer*)ktmp;

		/*
		else if (ktmp && (ktmp->Classify() == CLASS_VEHICLE))
		{
			CBasePlayer* pDriver = ((CFuncVehicle*)ktmp)->m_pDriver;

			if (pDriver != NULL)
			{
				peKiller = pDriver;
				ktmp = pDriver;
				pKiller = pDriver->pev;
			}
		}
		*/

		if (peKiller)
		{
			UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s has killed the Buster!", STRING((CBasePlayer*)peKiller->pev->netname)));
		}

		pVictim->pev->renderfx = kRenderFxNone;
		pVictim->pev->rendercolor = g_vecZero;
		pVictim->pev->effects &= ~EF_BRIGHTFIELD;
	}

	// g_pGameRules->PlayerKilled(pVictim, pKiller, pInflictor);
}

void AgBusters::DeathNotice(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pevInflictor)
{
	// only death notices that the buster was involved in
	if (!IsPlayerBusting(pVictim) && !IsPlayerBusting(CBaseEntity::Instance(pKiller)))
		return;

	g_pGameRules->DeathNotice(pVictim, pKiller, pevInflictor);
}

int AgBusters::WeaponShouldRespawn(CBasePlayerItem* pWeapon)
{
	if (pWeapon->m_iId == WEAPON_EGON)
		return GR_WEAPON_RESPAWN_NO;

	// return g_pGameRules->WeaponShouldRespawn(pWeapon);
}

//=========================================================
// CheckForEgons:
// Check to see if any player has an egon
// If they don't then get the lowest player on the scoreboard and give them one
// Then check to see if any weapon boxes out there has an egon, and delete it
//=========================================================
void AgBusters::CheckForEgons()
{
	if (m_flEgonBustingCheckTime <= 0.0f)
	{
		m_flEgonBustingCheckTime = gpGlobals->time + EGON_BUSTING_TIME;
		return;
	}

	if (m_flEgonBustingCheckTime <= gpGlobals->time)
	{
		m_flEgonBustingCheckTime = -1.0f; // reset

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);

			// someone is busting, no need to continue
			if (IsPlayerBusting(pPlayer))
				return;
		}

		int bBestFrags = 9999;
		CBasePlayer* pBestPlayer = NULL;

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);

			if (pPlayer && pPlayer->pev->frags <= bBestFrags)
			{
				bBestFrags = pPlayer->pev->frags;
				pBestPlayer = pPlayer;
			}
		}

		if (pBestPlayer)
		{
			pBestPlayer->GiveNamedItem("weapon_egon");

			CBaseEntity* pEntity = NULL;

			// find a weaponbox that includes an Egon, then destroy it.
			while ((pEntity = UTIL_FindEntityByClassname(pEntity, "weaponbox")) != NULL)
			{
				CWeaponBox* pWeaponBox = (CWeaponBox*)pEntity;

				if (pWeaponBox)
				{
					CBasePlayerItem* pWeapon;

					for (int i = 0; i < MAX_ITEM_TYPES; i++)
					{
						pWeapon = pWeaponBox->m_rgpPlayerItems[i];

						while (pWeapon)
						{
							// there you are, bye box
							if (pWeapon->m_iId == WEAPON_EGON)
							{
								pWeaponBox->Kill();
								break;
							}

							pWeapon = pWeapon->m_pNext;
						}
					}
				}
			}
		}
	}
}

BOOL AgBusters::CanHavePlayerItem(CBasePlayer* pPlayer, CBasePlayerItem* pItem)
{
	// buster cannot have more weapons nor ammo
	if (BustingCanHaveItem(pPlayer, pItem) == false)
	{
		return FALSE;
	}

	// return g_pGameRules->CanHavePlayerItem(pPlayer, pItem);
}

BOOL AgBusters::CanHaveItem(CBasePlayer* pPlayer, CItem* pItem)
{
	// Buster cannot have more weapons nor ammo
	if (BustingCanHaveItem(pPlayer, pItem) == false)
	{
		return FALSE;
	}

	// return g_pGameRules->CanHaveItem(pPlayer, pItem);
}

void AgBusters::PlayerGotWeapon(CBasePlayer* pPlayer, CBasePlayerItem* pWeapon)
{
	if (pWeapon->m_iId == WEAPON_EGON)
	{
		pPlayer->RemoveAllItems(false);

		UTIL_ClientPrintAll(HUD_PRINTCENTER, "LONG LIVE THE NEW BUSTER!");
		UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s IS BUSTING!\n", STRING((CBasePlayer*)pPlayer->pev->netname)));

		SetPlayerModel(pPlayer);

		pPlayer->pev->health = pPlayer->pev->max_health;
		pPlayer->pev->armorvalue = 100;

		pPlayer->pev->renderfx = kRenderFxGlowShell;
		pPlayer->pev->renderamt = 25;
		pPlayer->pev->rendercolor = Vector(0, 75, 250);

        CBasePlayerWeapon* pEgon = dynamic_cast<CBasePlayerWeapon*>(pWeapon);
		pEgon->m_iDefaultAmmo = 200;
		pPlayer->m_rgAmmo[pEgon->m_iPrimaryAmmoType] = pEgon->m_iDefaultAmmo;

		// set the buster's playermodel. Ivan for now, might change later.
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", "ivan");
	}
}

void AgBusters::ClientUserInfoChanged(CBasePlayer* pPlayer, char* infobuffer)
{
	SetPlayerModel(pPlayer);

	// TODO: set prefs
	// pPlayer->SetPrefsFromUserInfo(infobuffer);
}

void AgBusters::PlayerSpawn(CBasePlayer* pPlayer)
{
	// g_pGameRules->PlayerSpawn(pPlayer);
	SetPlayerModel(pPlayer);
}

void AgBusters::SetPlayerModel(CBasePlayer* pPlayer)
{
	if (IsPlayerBusting(pPlayer))
	{
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", "ivan");
	}
	else
	{
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", "skeleton");
	}
}