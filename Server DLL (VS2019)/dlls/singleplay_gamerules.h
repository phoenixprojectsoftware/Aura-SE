/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// multi_gamerules.h
//

//=========================================================
// CHalfLifeMultiplay - rules for the basic half life multiplayer
// competition
//=========================================================
//++ BulliT
#if !defined(_SINGLEPLAY_H_)
#define _SINGLEPLAY_H_
#include "aggamerules.h"
//-- Martin Webrant


//=========================================================
// CHalfLifeRules - rules for the single player Half-Life 
// game.
//=========================================================
class CHalfLifeRules : public AgGameRules
{
public:
	CHalfLifeRules(void);

	// GR_Think
	virtual void Think(void);
	virtual BOOL IsAllowedToSpawn(CBaseEntity* pEntity);
	virtual BOOL FAllowFlashlight(void) { return TRUE; };

	virtual BOOL FShouldSwitchWeapon(CBasePlayer* pPlayer, CBasePlayerItem* pWeapon);
	virtual BOOL GetNextBestWeapon(CBasePlayer* pPlayer, CBasePlayerItem* pCurrentWeapon);

	// Functions to verify the single/multiplayer status of a game
	virtual BOOL IsMultiplayer(void);
	virtual BOOL IsDeathmatch(void);
	virtual BOOL IsCoOp(void);

	// Client connection/disconnection
	virtual BOOL ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
	virtual void InitHUD(CBasePlayer* pl);		// the client dll is ready for updating
	virtual void ClientDisconnected(edict_t* pClient);

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer* pPlayer);

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer* pPlayer);
	virtual void PlayerThink(CBasePlayer* pPlayer);
	virtual BOOL FPlayerCanRespawn(CBasePlayer* pPlayer);
	virtual float FlPlayerSpawnTime(CBasePlayer* pPlayer);

	virtual BOOL AllowAutoTargetCrosshair(void);

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer* pAttacker, CBasePlayer* pKilled);
	virtual void PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor);
	virtual void DeathNotice(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor);

	// Weapon retrieval
	virtual void PlayerGotWeapon(CBasePlayer* pPlayer, CBasePlayerItem* pWeapon);

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem* pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem* pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem* pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem* pWeapon);

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer* pPlayer, CItem* pItem);
	virtual void PlayerGotItem(CBasePlayer* pPlayer, CItem* pItem);

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem* pItem);
	virtual float FlItemRespawnTime(CItem* pItem);
	virtual Vector VecItemRespawnSpot(CItem* pItem);

	// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer* pPlayer, char* szName, int iCount);

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo* pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo* pAmmo);
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo* pAmmo);

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void);
	virtual float FlHEVChargerRechargeTime(void);

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer* pPlayer);

	// What happens to a dead player's ammo	
	virtual int DeadPlayerAmmo(CBasePlayer* pPlayer);

	// Monsters
	virtual BOOL FAllowMonsters(void);

	// Teamplay stuff	
	virtual const char* GetTeamID(CBaseEntity* pEntity) { return ""; };
	virtual int PlayerRelationship(CBaseEntity* pPlayer, CBaseEntity* pTarget);
};

#endif //_SINGLEPLAY_H_
