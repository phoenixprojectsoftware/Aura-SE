/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#include <weaponinfo.h>
#ifndef WEAPONS_CEAGLE_H
#define WEAPONS_CEAGLE_H

class CEagleLaser;

enum DesertEagleAnim
{
	EAGLE_IDLE1 = 0,
	EAGLE_IDLE2,
	EAGLE_IDLE3,
	EAGLE_IDLE4,
	EAGLE_IDLE5,
	EAGLE_SHOOT,
	EAGLE_SHOOT_EMPTY,
	EAGLE_RELOAD_NOSHOT,
	EAGLE_RELOAD,
	EAGLE_DRAW,
	EAGLE_HOLSTER
};

#ifndef WEAPONS_NO_CLASSES
class CEagle : public CBasePlayerWeapon
{
public:
	using BaseClass = CBasePlayerWeapon;

	void Precache() override;

	void Spawn() override;

	BOOL AddToPlayer(CBasePlayer* pPlayer) override;

	BOOL Deploy() override;

	void Holster(int skiplocal = 0) override;

	void WeaponIdle() override;

	void PrimaryAttack() override;

	void SecondaryAttack() override;

	void Reload() override;

	int iItemSlot() override;

	int GetItemInfo(ItemInfo* p) override;

	BOOL UseDecrement() override
	{
#if defined( CLIENT_WEAPONS )
		return TRUE;
#else
		return FALSE;
#endif
	}

	void GetWeaponData(weapon_data_t& data);

	void SetWeaponData(const weapon_data_t& data);

private:
	void UpdateLaser();

private:
	int m_iShell;
	unsigned short m_usFireEagle;

	bool m_bSpotVisible;
	bool m_bLaserActive;
	CEagleLaser* m_pLaser;
};
#endif

#endif