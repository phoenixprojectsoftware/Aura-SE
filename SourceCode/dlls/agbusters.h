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

#ifndef BUSTERS_H
#define BUSTERS_H

#pragma once

class AgBusters
{
public:
	AgBusters();

	void Think();
	int IPointsForKill(CBasePlayer* pAttacker, CBasePlayer* pKilled);
	void PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor);
	void DeathNotice(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pevInflictor);
	int WeaponShouldRespawn(CBasePlayerItem* pWeapon);
	BOOL CanHavePlayerItem(CBasePlayer* pPlayer, CBasePlayerItem* pWeapon);
	BOOL CanHaveItem(CBasePlayer* pPlayer, CItem* pItem);
	void PlayerGotWeapon(CBasePlayer* pPlayer, CBasePlayerItem* pWeapon);
	void ClientUserInfoChanged(CBasePlayer* pPlayer, char* infobuffer);
	void PlayerSpawn(CBasePlayer* pPlayer);

	void SetPlayerModel(CBasePlayer* pPlayer);

protected:
	float m_flEgonBustingCheckTime = -1.0f;
	void CheckForEgons();
};
#endif
