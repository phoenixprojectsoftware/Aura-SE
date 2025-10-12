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
#include "../extdll.h"
#include "../util.h"
#include "../cbase.h"
#include "../player.h"
#include "../weapons.h"
#include "../gamerules.h"
#include "../weapon_hierarchy.h"

#include "CSniperRifle.h"

#ifndef CLIENT_DLL
TYPEDESCRIPTION	CSniperRifle::m_SaveData[] =
{
	DEFINE_FIELD(CSniperRifle, m_flReloadStart, FIELD_TIME),
	DEFINE_FIELD(CSniperRifle, m_bReloading, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CSniperRifle, CSniperRifle::BaseClass);
#endif

LINK_ENTITY_TO_CLASS(weapon_sniperrifle, CSniperRifle);

void CSniperRifle::Precache()
{
	pev->classname = MAKE_STRING("weapon_sniperrifle");

	BaseClass::Precache();

	m_iId = WEAPON_SNIPERRIFLE;

#ifndef _HALO
	// Cross Product
	PRECACHE_MODEL("models/w_m40a1.mdl");
	PRECACHE_MODEL("models/v_m40a1.mdl");
	PRECACHE_MODEL("models/p_m40a1.mdl");
#else
	// Halo: GoldSource
	PRECACHE_MODEL("models/w_sniper.mdl");
	PRECACHE_MODEL("models/v_sniper.mdl");
	PRECACHE_MODEL("models/p_sniper.mdl");
	PRECACHE_SOUND("weapons/sniper_zoom2.wav");
#endif
	PRECACHE_SOUND("weapons/sniper_fire.wav");
	PRECACHE_SOUND("weapons/sniper_zoom.wav");
	PRECACHE_SOUND("weapons/sniper_reload_first_seq.wav");
	PRECACHE_SOUND("weapons/sniper_reload_second_seq.wav");
	PRECACHE_SOUND("weapons/sniper_miss.wav");
	PRECACHE_SOUND("weapons/sniper_bolt1.wav");
	PRECACHE_SOUND("weapons/sniper_bolt2.wav");

	m_usSniper = PRECACHE_EVENT(1, "events/sniper.sc");
}

void CSniperRifle::Spawn()
{
	Precache();

#ifdef _HALO
	SET_MODEL(edict(), "models/w_sniper.mdl");
	m_iDefaultAmmo = CROSSBOW_DEFAULT_GIVE;
#else
	SET_MODEL(edict(), "models/w_m40a1.mdl");
	m_iDefaultAmmo = SNIPERRIFLE_DEFAULT_GIVE;
#endif

	FallInit(); // get ready to fall down.
}

BOOL CSniperRifle::AddToPlayer(CBasePlayer* pPlayer)
{
	if (BaseClass::AddToPlayer(pPlayer))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, pPlayer->edict());
		WRITE_BYTE(m_iId);
		MESSAGE_END();
		return true;
	}
	return false;
}

BOOL CSniperRifle::Deploy()
{
#ifndef _HALO
	return BaseClass::DefaultDeploy("models/v_m40a1.mdl", "models/p_m40a1.mdl", SNIPERRIFLE_DRAW, "bow");
#else
	return BaseClass::DefaultDeploy("models/v_sniper.mdl", "models/p_sniper.mdl", SNIPERRIFLE_DRAW1, "bow");
#endif

}

void CSniperRifle::Holster(int skiplocal)
{
	m_fInReload = false;// cancel any reload in progress.

	if (m_bInZoom)
		SecondaryAttack();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.25;

#ifndef _HALO
	SendWeaponAnim(SNIPERRIFLE_HOLSTER);
#else
	SendWeaponAnim(SNIPERRIFLE_HOLSTER1);
#endif
}

void CSniperRifle::WeaponIdle()
{
	//Update autoaim
	m_pPlayer->GetAutoaimVector(AUTOAIM_2DEGREES);

	ResetEmptySound();

	if (m_bReloading && gpGlobals->time >= m_flReloadStart + 2.324)
	{
#ifndef _HALO
		SendWeaponAnim(SNIPERRIFLE_RELOAD2);
#else
		SendWeaponAnim(SNIPERRIFLE_RELOAD);
#endif
		m_bReloading = false;
	}

	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
#ifndef _HALO
		if (m_iClip)
			SendWeaponAnim(SNIPERRIFLE_SLOWIDLE);
		else
			SendWeaponAnim(SNIPERRIFLE_SLOWIDLE2);
#else
		if (m_iClip)
			SendWeaponAnim(SNIPERRIFLE_FIDGET1);
		else
			SendWeaponAnim(SNIPERRIFLE_FIDGET2);
#endif

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.348;
	}
}

void CSniperRifle::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == WATERLEVEL_HEAD)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0f;
		return;
	}

	if (!m_iClip)
	{
		PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	--m_iClip;

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

	UTIL_MakeVectors(vecAngles);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_2DEGREES);

	//TODO: 8192 constant should be defined somewhere - Solokiller
	Vector vecShot = m_pPlayer->FireBulletsPlayer(1,
		vecSrc, vecAiming, g_vecZero,
		8192, BULLET_PLAYER_762, 0, 0,
		m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(FEV_NOTHOST,
		m_pPlayer->edict(), m_usSniper, 0,
		(float*)&g_vecZero, (float*)&g_vecZero,
		vecShot.x, vecShot.y,
		m_iClip, m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()],
		0, 0);

#ifndef _HALO
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0f;
#else
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.65f;
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
}

void CSniperRifle::SecondaryAttack()
{
	if (!m_bInZoom)
		EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/sniper_zoom.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	else
		EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/sniper_zoom2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	m_bInZoom = !m_bInZoom;

	ToggleZoom();

	//TODO: use UTIL_WeaponTimeBase() - Solokiller
	//TODO: this doesn't really make sense
	pev->nextthink = 0.0 + 0.1;

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1 + 0.5;
}

void CSniperRifle::Reload()
{
	if (m_pPlayer->ammo_762 > 0)
	{
		if (m_pPlayer->m_iFOV != 0)
		{
			ToggleZoom();
		}

		if (m_iClip)
		{
#ifdef _HALO
			if (DefaultReload(CROSSBOW_MAX_CLIP, SNIPERRIFLE_RELOAD, 4.0))
			{
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 4.0;
			}
#else
			if (DefaultReload(SNIPERRIFLE_MAX_CLIP, SNIPERRIFLE_RELOAD3, 2.324, 1))
			{
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.324;
			}
#endif
		}
#ifdef _HALO
		else if (DefaultReload(CROSSBOW_MAX_CLIP, SNIPERRIFLE_RELOAD, 4.0))
		{
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 4.0;
			m_flReloadStart = gpGlobals->time;
			m_bReloading = true;
		}
#else
		else if (DefaultReload(SNIPERRIFLE_MAX_CLIP, SNIPERRIFLE_RELOAD1, 2.324, 1))
		{
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 4.102;
			m_flReloadStart = gpGlobals->time;
			m_bReloading = true;
		}
#endif
		else
		{
			m_bReloading = false;
		}
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.102;
}

int CSniperRifle::iItemSlot()
{
	return  4;
}

int CSniperRifle::GetItemInfo(ItemInfo* p)
{
	p->pszAmmo1 = "762";
#ifdef _HALO
	p->iMaxAmmo1 = BOLT_MAX_CARRY;
#else
	p->iMaxAmmo1 = SNIPERRIFLE_MAX_CARRY;
#endif
	p->pszName = STRING(pev->classname);
	p->pszAmmo2 = 0;
	p->iMaxAmmo2 = WEAPON_NOCLIP;
#ifdef _HALO
	p->iMaxClip = BOLT_MAX_CARRY;
	p->iSlot = WPN_PRECISION_SLOT;
#else
	p->iMaxClip = SNIPERRIFLE_MAX_CLIP;
	p->iSlot = WPN_GEARBOX1_SLOT;
#endif
	p->iPosition = WPN_SNIPERRIFLE_POS;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SNIPERRIFLE;
	p->iWeight = SNIPERRIFLE_WEIGHT;
	return true;
}

void CSniperRifle::ToggleZoom()
{
	if (m_pPlayer->pev->fov == 0)
	{
		m_pPlayer->pev->fov = 18;
		m_pPlayer->m_iFOV = 18;

		m_bInZoom = true;
	}
	else
	{
		m_pPlayer->pev->fov = 0;
		m_pPlayer->m_iFOV = 0;

		m_bInZoom = false;
	}
}

class CSniperRifleAmmo : public CBasePlayerAmmo
{
public:
	using BaseClass = CBasePlayerAmmo;

	void Spawn(void) override
	{
		Precache();
		SET_MODEL(edict(), "models/w_m40a1clip.mdl");
		CBasePlayerAmmo::Spawn();
	}

	void Precache(void) override
	{
		PRECACHE_MODEL("models/w_m40a1clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}

	BOOL AddAmmo(CBaseEntity* pOther) override
	{
#ifdef _HALO
		if (pOther->GiveAmmo(AMMO_CROSSBOWCLIP_GIVE, "762", BOLT_MAX_CARRY) != -1)
#else
		if (pOther->GiveAmmo(AMMO_SNIPERRIFLE_GIVE, "762", SNIPERRIFLE_MAX_CARRY) != -1)
#endif
		{
			EMIT_SOUND(edict(), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);

			return true;
		}

		return false;
	}
};

LINK_ENTITY_TO_CLASS(ammo_762, CSniperRifleAmmo);