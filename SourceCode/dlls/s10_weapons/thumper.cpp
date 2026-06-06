/****
*
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* HX-40 THUMPER - M203 GRENADE LAUNCHER WEAPON
*
*
****/

#ifndef _HALO

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "weapon_hierarchy.h"
#include "gamerules.h"

LINK_ENTITY_TO_CLASS(weapon_thumper, CThumper);

void CThumper::Spawn()
{
	Precache();

	m_iId = WEAPON_THUMPER;
	SET_MODEL(ENT(pev), "models/w_railgun.mdl");

	m_iDefaultAmmo = THUMPER_DEFAULT_GIVE;

	FallInit();
}

void CThumper::Precache()
{
	PRECACHE_MODEL("models/weapons/thumper/v_rock2.mdl");
	PRECACHE_MODEL("models/w_railgun.mdl");
	PRECACHE_MODEL("models/weapons/thumper/p_rock2.mdl");

	PRECACHE_SOUND("weapons/glauncher.wav");

	PRECACHE_MODEL("models/grenade.mdl");

	m_usThumper = PRECACHE_EVENT(1, "events/thumper.sc");
}

int CThumper::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ARgrenades";
	p->iMaxAmmo1 = M203_GRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = WEAPON_NOCLIP;
	p->iMaxClip = THUMPER_MAX_CLIP;
	p->iSlot = WPN_FOREIGN_SLOT;
	p->iPosition = WPN_THUMPER_POS;
	p->iId = WEAPON_THUMPER;
	p->iFlags = 0;
	p->iWeight = THUMPER_WEIGHT;

	return 1;
}

void CThumper::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.35f;
		return;
	}

	m_iClip--;

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 6 - gpGlobals->v_up * 4;

	Vector vecVelocity = gpGlobals->v_forward * 900;

	CGrenade::ShootContact(m_pPlayer->pev, vecSrc, vecVelocity);

	PLAYBACK_EVENT_FULL(0, edict(), m_usThumper, 0.0, (float*)&g_vecZero, (float*)&g_vecZero, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	SendWeaponAnim(THUMPER_FIRE);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0f;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
}

void CThumper::Reload()
{
	if (m_iClip >= 1)
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	DefaultReload(1, THUMPER_RELOAD, 2.0f);
}

void CThumper::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(THUMPER_IDLE);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10.0f, 15.0f);
}
#endif //_HALO
