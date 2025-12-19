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

#pragma once

#ifndef _HALO

class CRailgunRay : public CBaseEntity
{
public:
	void Spawn(void);
	void Precache(void);
	void EXPORT RayThink(void);
	void EXPORT RayTouch(CBaseEntity* pOther);
};

#endif
