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
#include "gamerules.h"

#include "aggamerules.h"
#include "agglobal.h"
//#include "agarena.h"
#include "agfirefight.h"

#include "algo.h"

extern cvar_t coopmode;

AgFirefight::AgFirefight()
{
	ALERT(at_console, "fart\n");
}

AgFirefight::~AgFirefight()
{

}

void AgFirefight::SpawnWave(int waveIndex)
{
	// This function should spawn the enemies for the given wave index
	ALERT(at_console, "Spawning wave %d\n", waveIndex);
}

void AgFirefight::Think()
{

}