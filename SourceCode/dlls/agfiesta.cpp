/****
*
* Copyright (c) 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Fiesta: the fiesta gametype class.
*
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

#include "aggamerules.h"
#include "agglobal.h"
#include "agfiesta.h"

#include "algo.h"

AgFiesta::AgFiesta()
{
	ALERT(at_console, "Fiesta gametype initialized.\n");
}

AgFiesta::~AgFiesta()
{
	ALERT(at_console, "Fiesta gametype destroyed.\n");
}

void AgFiesta::Think()
{
	// ALERT(at_console, "Fiesta gametype thinking...\n");
}