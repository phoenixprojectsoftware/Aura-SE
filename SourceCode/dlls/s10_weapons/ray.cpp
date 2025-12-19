/****
*
* Copyright (c) 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* railgun ray: super cool laser thingy
*
*
****/
#ifndef _HALO

#include "../extdll.h"
#include "../util.h"
#include "../cbase.h"
#include "../weapons.h"
#include "../player.h"
#include "../effects.h"
#include "../skill.h"
#include "../decals.h"
#include "../gamerules.h"

#include "ray.h"

LINK_ENTITY_TO_CLASS(railgun_ray, CRailgunRay);

void CRailgunRay::Spawn()
{
	Precache();

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->gravity = 0;
	pev->effects |= EF_NODRAW;

	SET_MODEL(ENT(pev), "sprites/laserdot.spr");

	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	SetThink(&CRailgunRay::RayThink);
	SetTouch(&CRailgunRay::RayTouch);

	pev->nextthink = gpGlobals->time;
}

void CRailgunRay::Precache()
{
	PRECACHE_MODEL("sprites/laserdot.spr");
}

void CRailgunRay::RayThink(void)
{
	pev->nextthink = gpGlobals->time;

	if (UTIL_PointContents(pev->origin + pev->velocity.Normalize() * 2) == CONTENTS_WATER)
	{
		if (pev->velocity.z != pev->vuser1.z)
			pev->velocity.z = pev->vuser1.z;
	}
}

void CRailgunRay::RayTouch(CBaseEntity* pOther)
{
	SetTouch(NULL);
	SetThink(NULL);

	if (pOther->pev->takedamage)
	{
		TraceResult tr = UTIL_GetGlobalTrace();
		entvars_t* pevOwner = VARS(pev->owner);

		ClearMultiDamage();

		pOther->TraceAttack(pevOwner, 25, pev->velocity.Normalize(), &tr, DMG_BULLET | DMG_NEVERGIB);
		ApplyMultiDamage(pev, pevOwner);

		pev->velocity = Vector(0, 0, 0);

		if (UTIL_PointContents(pev->origin) != CONTENTS_WATER)
		{
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z - 16);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(5);
			WRITE_BYTE(12);
			MESSAGE_END();
		}
		else
			UTIL_Bubbles(pev->origin - Vector(16, 16, 32), pev->origin + Vector(16, 16, 32), 50);
	}

	UTIL_Remove(this);
}

#endif // _HALO
