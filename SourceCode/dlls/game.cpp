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
#include "extdll.h"
#include "net_profiling.h"
#include "eiface.h"
#include "util.h"
#include "game.h"

//++ BulliT
#include "agglobal.h"
//-- Martin Webrant
cvar_t	displaysoundlist = {"displaysoundlist","0"};

// multiplayer server rules
cvar_t	fragsleft	= {"mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	timeleft	= {"mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "

// multiplayer server rules
cvar_t	teamplay	= {"mp_teamplay","0", FCVAR_SERVER };
cvar_t	fraglimit	= {"mp_fraglimit","0", FCVAR_SERVER };
cvar_t	timelimit	= {"mp_timelimit","0", FCVAR_SERVER };
cvar_t	friendlyfire= {"mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage	= {"mp_falldamage","0", FCVAR_SERVER };
cvar_t	weaponstay	= {"mp_weaponstay","0", FCVAR_SERVER };
cvar_t	forcerespawn= {"mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight	= {"mp_flashlight","0", FCVAR_SERVER };
cvar_t	aimcrosshair= {"mp_autocrosshair","1", FCVAR_SERVER };
cvar_t	decalfrequency = {"decalfrequency","30", FCVAR_SERVER };
cvar_t	teamlist = {"mp_teamlist","hgrunt;scientist", FCVAR_SERVER };
cvar_t	teamoverride = {"mp_teamoverride","1" };
cvar_t	defaultteam = {"mp_defaultteam","0" };
cvar_t  coopmode = { "sv_aura_coop", "0", FCVAR_SERVER };
cvar_t	allowmonsters={"mp_allowmonsters","0", FCVAR_SERVER };
cvar_t  mp5_old_rate = { "sv_aura_mp5_old_rate", "1", FCVAR_SERVER };
cvar_t fire_weapons_underwater = { "sv_aura_fire_weapons_underwater", "0", FCVAR_SERVER };
cvar_t motionsensor = { "sv_aura_motionsensor", "1", FCVAR_SERVER };

// mp_chattime is like the minimum intermission time, you can't skip it, it's meant
// to have some time for saying gg, etc. before changing map, it's part of the intermission
// mp_intermission_time is the maximum intermission time, so after that it will automatically
// change map if no one in the server has skipped it before
// mp_intermission_skip_auto is for skipping the whole intermission (includes chattime) when
// appropriate, like when there's just 1 player (or none) in the serve
cvar_t  mp_chattime = {"mp_chattime","6", FCVAR_SERVER };
cvar_t  mp_intermission_time = {"mp_intermission_time","15", FCVAR_SERVER };
cvar_t mp_intermission_skip_auto = { "mp_intermission_skip_auto", "1", FCVAR_SERVER };


// sv_singleplayer 1: enables entities and stuff that allows playing singleplayer campaigns properly
cvar_t	singleplayer = { "sv_singleplayer", "0", FCVAR_SERVER };

// These 2 cvars for loading and gauss charging are used to disable clientside prediction (cl_lw 0) when they're in a specific state,
// which is necessary for weapons and specially gauss to work correctly through saveloads (e.g.: keep gaus charge)
// Credits to h0boken for finding out that disabling clientside prediction kinda fixes the issue
cvar_t	sploading = { "sv_sp_loading", "0", FCVAR_SERVER }; // only true when loading (changelevel)
cvar_t	spgausscharging = { "sv_sp_gauss_charging", "0", FCVAR_SERVER }; // only true when gauss is charging (secondary fire)


// Engine Cvars
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_psv_aim = NULL;
cvar_t  *g_psv_allow_autoaim = NULL;
cvar_t	*g_footsteps = NULL;

//Macros to make skill cvars easier to define
#define DECLARE_SKILL_CVARS( name )					\
cvar_t	sk_##name##1 = { "sk_" #name "1", "0" };	\
cvar_t	sk_##name##2 = { "sk_" #name "2", "0" };	\
cvar_t	sk_##name##3 = { "sk_" #name "3", "0" }

#define REGISTER_SKILL_CVARS( name )	\
CVAR_REGISTER( &sk_##name##1 );			\
CVAR_REGISTER( &sk_##name##2 );			\
CVAR_REGISTER( &sk_##name##3 )

//CVARS FOR SKILL LEVEL SETTINGS
// Agrunt
cvar_t	sk_agrunt_health1 = {"sk_agrunt_health1","0"};
cvar_t	sk_agrunt_health2 = {"sk_agrunt_health2","0"};
cvar_t	sk_agrunt_health3 = {"sk_agrunt_health3","0"};

cvar_t	sk_agrunt_dmg_punch1 = {"sk_agrunt_dmg_punch1","0"};
cvar_t	sk_agrunt_dmg_punch2 = {"sk_agrunt_dmg_punch2","0"};
cvar_t	sk_agrunt_dmg_punch3 = {"sk_agrunt_dmg_punch3","0"};

// Apache
cvar_t	sk_apache_health1	= {"sk_apache_health1","0"};
cvar_t	sk_apache_health2	= {"sk_apache_health2","0"};
cvar_t	sk_apache_health3	= {"sk_apache_health3","0"};

// Barney
cvar_t	sk_barney_health1	= {"sk_barney_health1","0"};
cvar_t	sk_barney_health2	= {"sk_barney_health2","0"};
cvar_t	sk_barney_health3	= {"sk_barney_health3","0"};

// Otis
DECLARE_SKILL_CVARS(otis_health);

// Bullsquid
cvar_t	sk_bullsquid_health1 = {"sk_bullsquid_health1","0"};
cvar_t	sk_bullsquid_health2 = {"sk_bullsquid_health2","0"};
cvar_t	sk_bullsquid_health3 = {"sk_bullsquid_health3","0"};

cvar_t	sk_bullsquid_dmg_bite1 = {"sk_bullsquid_dmg_bite1","0"};
cvar_t	sk_bullsquid_dmg_bite2 = {"sk_bullsquid_dmg_bite2","0"};
cvar_t	sk_bullsquid_dmg_bite3 = {"sk_bullsquid_dmg_bite3","0"};

cvar_t	sk_bullsquid_dmg_whip1 = {"sk_bullsquid_dmg_whip1","0"};
cvar_t	sk_bullsquid_dmg_whip2 = {"sk_bullsquid_dmg_whip2","0"};
cvar_t	sk_bullsquid_dmg_whip3 = {"sk_bullsquid_dmg_whip3","0"};

cvar_t	sk_bullsquid_dmg_spit1 = {"sk_bullsquid_dmg_spit1","0"};
cvar_t	sk_bullsquid_dmg_spit2 = {"sk_bullsquid_dmg_spit2","0"};
cvar_t	sk_bullsquid_dmg_spit3 = {"sk_bullsquid_dmg_spit3","0"};

// Pit Drone
DECLARE_SKILL_CVARS(pitdrone_health);
DECLARE_SKILL_CVARS(pitdrone_dmg_bite);
DECLARE_SKILL_CVARS(pitdrone_dmg_whip);
DECLARE_SKILL_CVARS(pitdrone_dmg_spit);


// Big Momma
cvar_t	sk_bigmomma_health_factor1 = {"sk_bigmomma_health_factor1","1.0"};
cvar_t	sk_bigmomma_health_factor2 = {"sk_bigmomma_health_factor2","1.0"};
cvar_t	sk_bigmomma_health_factor3 = {"sk_bigmomma_health_factor3","1.0"};

cvar_t	sk_bigmomma_dmg_slash1 = {"sk_bigmomma_dmg_slash1","50"};
cvar_t	sk_bigmomma_dmg_slash2 = {"sk_bigmomma_dmg_slash2","50"};
cvar_t	sk_bigmomma_dmg_slash3 = {"sk_bigmomma_dmg_slash3","50"};

cvar_t	sk_bigmomma_dmg_blast1 = {"sk_bigmomma_dmg_blast1","100"};
cvar_t	sk_bigmomma_dmg_blast2 = {"sk_bigmomma_dmg_blast2","100"};
cvar_t	sk_bigmomma_dmg_blast3 = {"sk_bigmomma_dmg_blast3","100"};

cvar_t	sk_bigmomma_radius_blast1 = {"sk_bigmomma_radius_blast1","250"};
cvar_t	sk_bigmomma_radius_blast2 = {"sk_bigmomma_radius_blast2","250"};
cvar_t	sk_bigmomma_radius_blast3 = {"sk_bigmomma_radius_blast3","250"};

// Shock Roach
DECLARE_SKILL_CVARS(shockroach_health);
DECLARE_SKILL_CVARS(shockroach_dmg_bite);
DECLARE_SKILL_CVARS(shockroach_lifespan);

// Gargantua
cvar_t	sk_gargantua_health1 = {"sk_gargantua_health1","0"};
cvar_t	sk_gargantua_health2 = {"sk_gargantua_health2","0"};
cvar_t	sk_gargantua_health3 = {"sk_gargantua_health3","0"};

cvar_t	sk_gargantua_dmg_slash1	= {"sk_gargantua_dmg_slash1","0"};
cvar_t	sk_gargantua_dmg_slash2	= {"sk_gargantua_dmg_slash2","0"};
cvar_t	sk_gargantua_dmg_slash3	= {"sk_gargantua_dmg_slash3","0"};

cvar_t	sk_gargantua_dmg_fire1 = {"sk_gargantua_dmg_fire1","0"};
cvar_t	sk_gargantua_dmg_fire2 = {"sk_gargantua_dmg_fire2","0"};
cvar_t	sk_gargantua_dmg_fire3 = {"sk_gargantua_dmg_fire3","0"};

cvar_t	sk_gargantua_dmg_stomp1	= {"sk_gargantua_dmg_stomp1","0"};
cvar_t	sk_gargantua_dmg_stomp2	= {"sk_gargantua_dmg_stomp2","0"};
cvar_t	sk_gargantua_dmg_stomp3	= {"sk_gargantua_dmg_stomp3","0"};


// Hassassin
cvar_t	sk_hassassin_health1 = {"sk_hassassin_health1","0"};
cvar_t	sk_hassassin_health2 = {"sk_hassassin_health2","0"};
cvar_t	sk_hassassin_health3 = {"sk_hassassin_health3","0"};


// Headcrab
cvar_t	sk_headcrab_health1 = {"sk_headcrab_health1","0"};
cvar_t	sk_headcrab_health2 = {"sk_headcrab_health2","0"};
cvar_t	sk_headcrab_health3 = {"sk_headcrab_health3","0"};

cvar_t	sk_headcrab_dmg_bite1 = {"sk_headcrab_dmg_bite1","0"};
cvar_t	sk_headcrab_dmg_bite2 = {"sk_headcrab_dmg_bite2","0"};
cvar_t	sk_headcrab_dmg_bite3 = {"sk_headcrab_dmg_bite3","0"};


// Hgrunt 
cvar_t	sk_hgrunt_health1 = {"sk_hgrunt_health1","0"};
cvar_t	sk_hgrunt_health2 = {"sk_hgrunt_health2","0"};
cvar_t	sk_hgrunt_health3 = {"sk_hgrunt_health3","0"};

cvar_t	sk_hgrunt_kick1 = {"sk_hgrunt_kick1","0"};
cvar_t	sk_hgrunt_kick2 = {"sk_hgrunt_kick2","0"};
cvar_t	sk_hgrunt_kick3 = {"sk_hgrunt_kick3","0"};

cvar_t	sk_hgrunt_pellets1 = {"sk_hgrunt_pellets1","0"};
cvar_t	sk_hgrunt_pellets2 = {"sk_hgrunt_pellets2","0"};
cvar_t	sk_hgrunt_pellets3 = {"sk_hgrunt_pellets3","0"};

cvar_t	sk_hgrunt_gspeed1 = {"sk_hgrunt_gspeed1","0"};
cvar_t	sk_hgrunt_gspeed2 = {"sk_hgrunt_gspeed2","0"};
cvar_t	sk_hgrunt_gspeed3 = {"sk_hgrunt_gspeed3","0"};

// Hgrunt Ally
DECLARE_SKILL_CVARS(hgrunt_ally_health);
DECLARE_SKILL_CVARS(hgrunt_ally_kick);
DECLARE_SKILL_CVARS(hgrunt_ally_pellets);
DECLARE_SKILL_CVARS(hgrunt_ally_gspeed);

// Hgrunt Medic
DECLARE_SKILL_CVARS(medic_ally_health);
DECLARE_SKILL_CVARS(medic_ally_kick);
DECLARE_SKILL_CVARS(medic_ally_pellets);
DECLARE_SKILL_CVARS(medic_ally_gspeed);
DECLARE_SKILL_CVARS(medic_ally_heal);

// Hgrunt Torch
DECLARE_SKILL_CVARS(torch_ally_health);
DECLARE_SKILL_CVARS(torch_ally_kick);
DECLARE_SKILL_CVARS(torch_ally_pellets);
DECLARE_SKILL_CVARS(torch_ally_gspeed);

// Male Assassin
DECLARE_SKILL_CVARS(massassin_health);
DECLARE_SKILL_CVARS(massassin_kick);
DECLARE_SKILL_CVARS(massassin_pellets);
DECLARE_SKILL_CVARS(massassin_gspeed);

// Shock Trooper
DECLARE_SKILL_CVARS(shocktrooper_health);
DECLARE_SKILL_CVARS(shocktrooper_kick);
DECLARE_SKILL_CVARS(shocktrooper_gspeed);
DECLARE_SKILL_CVARS(shocktrooper_maxcharge);
DECLARE_SKILL_CVARS(shocktrooper_rchgspeed);

// Houndeye
cvar_t	sk_houndeye_health1 = {"sk_houndeye_health1","0"};
cvar_t	sk_houndeye_health2 = {"sk_houndeye_health2","0"};
cvar_t	sk_houndeye_health3 = {"sk_houndeye_health3","0"};

cvar_t	sk_houndeye_dmg_blast1 = {"sk_houndeye_dmg_blast1","0"};
cvar_t	sk_houndeye_dmg_blast2 = {"sk_houndeye_dmg_blast2","0"};
cvar_t	sk_houndeye_dmg_blast3 = {"sk_houndeye_dmg_blast3","0"};


// ISlave
cvar_t	sk_islave_health1 = {"sk_islave_health1","0"};
cvar_t	sk_islave_health2 = {"sk_islave_health2","0"};
cvar_t	sk_islave_health3 = {"sk_islave_health3","0"};

cvar_t	sk_islave_dmg_claw1 = {"sk_islave_dmg_claw1","0"};
cvar_t	sk_islave_dmg_claw2 = {"sk_islave_dmg_claw2","0"};
cvar_t	sk_islave_dmg_claw3 = {"sk_islave_dmg_claw3","0"};

cvar_t	sk_islave_dmg_clawrake1	= {"sk_islave_dmg_clawrake1","0"};
cvar_t	sk_islave_dmg_clawrake2	= {"sk_islave_dmg_clawrake2","0"};
cvar_t	sk_islave_dmg_clawrake3	= {"sk_islave_dmg_clawrake3","0"};
	
cvar_t	sk_islave_dmg_zap1 = {"sk_islave_dmg_zap1","0"};
cvar_t	sk_islave_dmg_zap2 = {"sk_islave_dmg_zap2","0"};
cvar_t	sk_islave_dmg_zap3 = {"sk_islave_dmg_zap3","0"};


// Icthyosaur
cvar_t	sk_ichthyosaur_health1	= {"sk_ichthyosaur_health1","0"};
cvar_t	sk_ichthyosaur_health2	= {"sk_ichthyosaur_health2","0"};
cvar_t	sk_ichthyosaur_health3	= {"sk_ichthyosaur_health3","0"};

cvar_t	sk_ichthyosaur_shake1	= {"sk_ichthyosaur_shake1","0"};
cvar_t	sk_ichthyosaur_shake2	= {"sk_ichthyosaur_shake2","0"};
cvar_t	sk_ichthyosaur_shake3	= {"sk_ichthyosaur_shake3","0"};


// Leech
cvar_t	sk_leech_health1 = {"sk_leech_health1","0"};
cvar_t	sk_leech_health2 = {"sk_leech_health2","0"};
cvar_t	sk_leech_health3 = {"sk_leech_health3","0"};

cvar_t	sk_leech_dmg_bite1 = {"sk_leech_dmg_bite1","0"};
cvar_t	sk_leech_dmg_bite2 = {"sk_leech_dmg_bite2","0"};
cvar_t	sk_leech_dmg_bite3 = {"sk_leech_dmg_bite3","0"};

// Controller
cvar_t	sk_controller_health1 = {"sk_controller_health1","0"};
cvar_t	sk_controller_health2 = {"sk_controller_health2","0"};
cvar_t	sk_controller_health3 = {"sk_controller_health3","0"};

cvar_t	sk_controller_dmgzap1 = {"sk_controller_dmgzap1","0"};
cvar_t	sk_controller_dmgzap2 = {"sk_controller_dmgzap2","0"};
cvar_t	sk_controller_dmgzap3 = {"sk_controller_dmgzap3","0"};

cvar_t	sk_controller_speedball1 = {"sk_controller_speedball1","0"};
cvar_t	sk_controller_speedball2 = {"sk_controller_speedball2","0"};
cvar_t	sk_controller_speedball3 = {"sk_controller_speedball3","0"};

cvar_t	sk_controller_dmgball1 = {"sk_controller_dmgball1","0"};
cvar_t	sk_controller_dmgball2 = {"sk_controller_dmgball2","0"};
cvar_t	sk_controller_dmgball3 = {"sk_controller_dmgball3","0"};

// Nihilanth
cvar_t	sk_nihilanth_health1 = {"sk_nihilanth_health1","0"};
cvar_t	sk_nihilanth_health2 = {"sk_nihilanth_health2","0"};
cvar_t	sk_nihilanth_health3 = {"sk_nihilanth_health3","0"};

cvar_t	sk_nihilanth_zap1 = {"sk_nihilanth_zap1","0"};
cvar_t	sk_nihilanth_zap2 = {"sk_nihilanth_zap2","0"};
cvar_t	sk_nihilanth_zap3 = {"sk_nihilanth_zap3","0"};

// Scientist
cvar_t	sk_scientist_health1 = {"sk_scientist_health1","0"};
cvar_t	sk_scientist_health2 = {"sk_scientist_health2","0"};
cvar_t	sk_scientist_health3 = {"sk_scientist_health3","0"};

// Cleansuit Scientist
DECLARE_SKILL_CVARS(cleansuit_scientist_health);

// Snark
cvar_t	sk_snark_health1 = {"sk_snark_health1","0"};
cvar_t	sk_snark_health2 = {"sk_snark_health2","0"};
cvar_t	sk_snark_health3 = {"sk_snark_health3","0"};

cvar_t	sk_snark_dmg_bite1 = {"sk_snark_dmg_bite1","0"};
cvar_t	sk_snark_dmg_bite2 = {"sk_snark_dmg_bite2","0"};
cvar_t	sk_snark_dmg_bite3 = {"sk_snark_dmg_bite3","0"};

cvar_t	sk_snark_dmg_pop1 = {"sk_snark_dmg_pop1","0"};
cvar_t	sk_snark_dmg_pop2 = {"sk_snark_dmg_pop2","0"};
cvar_t	sk_snark_dmg_pop3 = {"sk_snark_dmg_pop3","0"};

// Voltigore
DECLARE_SKILL_CVARS(voltigore_health);
DECLARE_SKILL_CVARS(voltigore_dmg_punch);
DECLARE_SKILL_CVARS(voltigore_dmg_beam);

// Baby Voltigore
DECLARE_SKILL_CVARS(babyvoltigore_health);
DECLARE_SKILL_CVARS(babyvoltigore_dmg_punch);

// Pit Worm
DECLARE_SKILL_CVARS(pitworm_health);
DECLARE_SKILL_CVARS(pitworm_dmg_swipe);
DECLARE_SKILL_CVARS(pitworm_dmg_beam);

// Gene Worm
DECLARE_SKILL_CVARS(geneworm_health);
DECLARE_SKILL_CVARS(geneworm_dmg_spit);
DECLARE_SKILL_CVARS(geneworm_dmg_hit);

// Zombie
cvar_t	sk_zombie_health1 = {"sk_zombie_health1","0"};
cvar_t	sk_zombie_health2 = {"sk_zombie_health2","0"};
cvar_t	sk_zombie_health3 = {"sk_zombie_health3","0"};

cvar_t	sk_zombie_dmg_one_slash1 = {"sk_zombie_dmg_one_slash1","0"};
cvar_t	sk_zombie_dmg_one_slash2 = {"sk_zombie_dmg_one_slash2","0"};
cvar_t	sk_zombie_dmg_one_slash3 = {"sk_zombie_dmg_one_slash3","0"};

cvar_t	sk_zombie_dmg_both_slash1 = {"sk_zombie_dmg_both_slash1","0"};
cvar_t	sk_zombie_dmg_both_slash2 = {"sk_zombie_dmg_both_slash2","0"};
cvar_t	sk_zombie_dmg_both_slash3 = {"sk_zombie_dmg_both_slash3","0"};


// Zombie Barney
DECLARE_SKILL_CVARS(zombie_barney_health);
DECLARE_SKILL_CVARS(zombie_barney_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_barney_dmg_both_slash);

// Zombie Soldier
DECLARE_SKILL_CVARS(zombie_soldier_health);
DECLARE_SKILL_CVARS(zombie_soldier_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_soldier_dmg_both_slash);

// Gonome
DECLARE_SKILL_CVARS(gonome_dmg_guts);
DECLARE_SKILL_CVARS(gonome_health);
DECLARE_SKILL_CVARS(gonome_dmg_one_slash);
DECLARE_SKILL_CVARS(gonome_dmg_one_bite);

//Turret
cvar_t	sk_turret_health1 = {"sk_turret_health1","0"};
cvar_t	sk_turret_health2 = {"sk_turret_health2","0"};
cvar_t	sk_turret_health3 = {"sk_turret_health3","0"};


// MiniTurret
cvar_t	sk_miniturret_health1 = {"sk_miniturret_health1","0"};
cvar_t	sk_miniturret_health2 = {"sk_miniturret_health2","0"};
cvar_t	sk_miniturret_health3 = {"sk_miniturret_health3","0"};


// Sentry Turret
cvar_t	sk_sentry_health1 = {"sk_sentry_health1","0"};
cvar_t	sk_sentry_health2 = {"sk_sentry_health2","0"};
cvar_t	sk_sentry_health3 = {"sk_sentry_health3","0"};


// PLAYER WEAPONS

// Crowbar whack
cvar_t	sk_plr_crowbar1 = {"sk_plr_crowbar1","0"};
cvar_t	sk_plr_crowbar2 = {"sk_plr_crowbar2","0"};
cvar_t	sk_plr_crowbar3 = {"sk_plr_crowbar3","0"};

// Glock Round
cvar_t	sk_plr_9mm_bullet1 = {"sk_plr_9mm_bullet1","0"};
cvar_t	sk_plr_9mm_bullet2 = {"sk_plr_9mm_bullet2","0"};
cvar_t	sk_plr_9mm_bullet3 = {"sk_plr_9mm_bullet3","0"};

// 357 Round
cvar_t	sk_plr_357_bullet1 = {"sk_plr_357_bullet1","0"};
cvar_t	sk_plr_357_bullet2 = {"sk_plr_357_bullet2","0"};
cvar_t	sk_plr_357_bullet3 = {"sk_plr_357_bullet3","0"};

// MP5 Round
cvar_t	sk_plr_9mmAR_bullet1 = {"sk_plr_9mmAR_bullet1","0"};
cvar_t	sk_plr_9mmAR_bullet2 = {"sk_plr_9mmAR_bullet2","0"};
cvar_t	sk_plr_9mmAR_bullet3 = {"sk_plr_9mmAR_bullet3","0"};


// M203 grenade
cvar_t	sk_plr_9mmAR_grenade1 = {"sk_plr_9mmAR_grenade1","0"};
cvar_t	sk_plr_9mmAR_grenade2 = {"sk_plr_9mmAR_grenade2","0"};
cvar_t	sk_plr_9mmAR_grenade3 = {"sk_plr_9mmAR_grenade3","0"};


// Shotgun buckshot
cvar_t	sk_plr_buckshot1 = {"sk_plr_buckshot1","0"};
cvar_t	sk_plr_buckshot2 = {"sk_plr_buckshot2","0"};
cvar_t	sk_plr_buckshot3 = {"sk_plr_buckshot3","0"};


// Crossbow
cvar_t	sk_plr_xbow_bolt_client1 = {"sk_plr_xbow_bolt_client1","0"};
cvar_t	sk_plr_xbow_bolt_client2 = {"sk_plr_xbow_bolt_client2","0"};
cvar_t	sk_plr_xbow_bolt_client3 = {"sk_plr_xbow_bolt_client3","0"};

cvar_t	sk_plr_xbow_bolt_monster1 = {"sk_plr_xbow_bolt_monster1","0"};
cvar_t	sk_plr_xbow_bolt_monster2 = {"sk_plr_xbow_bolt_monster2","0"};
cvar_t	sk_plr_xbow_bolt_monster3 = {"sk_plr_xbow_bolt_monster3","0"};


// RPG
cvar_t	sk_plr_rpg1 = {"sk_plr_rpg1","0"};
cvar_t	sk_plr_rpg2 = {"sk_plr_rpg2","0"};
cvar_t	sk_plr_rpg3 = {"sk_plr_rpg3","0"};


// Zero Point Generator
cvar_t	sk_plr_gauss1 = {"sk_plr_gauss1","0"};
cvar_t	sk_plr_gauss2 = {"sk_plr_gauss2","0"};
cvar_t	sk_plr_gauss3 = {"sk_plr_gauss3","0"};


// Tau Cannon
cvar_t	sk_plr_egon_narrow1 = {"sk_plr_egon_narrow1","0"};
cvar_t	sk_plr_egon_narrow2 = {"sk_plr_egon_narrow2","0"};
cvar_t	sk_plr_egon_narrow3 = {"sk_plr_egon_narrow3","0"};

cvar_t	sk_plr_egon_wide1 = {"sk_plr_egon_wide1","0"};
cvar_t	sk_plr_egon_wide2 = {"sk_plr_egon_wide2","0"};
cvar_t	sk_plr_egon_wide3 = {"sk_plr_egon_wide3","0"};


// Hand Grendade
cvar_t	sk_plr_hand_grenade1 = {"sk_plr_hand_grenade1","0"};
cvar_t	sk_plr_hand_grenade2 = {"sk_plr_hand_grenade2","0"};
cvar_t	sk_plr_hand_grenade3 = {"sk_plr_hand_grenade3","0"};


// Satchel Charge
cvar_t	sk_plr_satchel1	= {"sk_plr_satchel1","0"};
cvar_t	sk_plr_satchel2	= {"sk_plr_satchel2","0"};
cvar_t	sk_plr_satchel3	= {"sk_plr_satchel3","0"};


// Tripmine
cvar_t	sk_plr_tripmine1 = {"sk_plr_tripmine1","0"};
cvar_t	sk_plr_tripmine2 = {"sk_plr_tripmine2","0"};
cvar_t	sk_plr_tripmine3 = {"sk_plr_tripmine3","0"};

// Pipe Wrench
DECLARE_SKILL_CVARS(plr_pipewrench);

// Knife
DECLARE_SKILL_CVARS(plr_knife);

// Grapple
DECLARE_SKILL_CVARS(plr_grapple);

// Desert Eagle
DECLARE_SKILL_CVARS(plr_eagle);

// Sniper Rifle
DECLARE_SKILL_CVARS(plr_762_bullet);

// M249
DECLARE_SKILL_CVARS(plr_556_bullet);

// Displacer
DECLARE_SKILL_CVARS(plr_displacer_self);
DECLARE_SKILL_CVARS(plr_displacer_other);
DECLARE_SKILL_CVARS(plr_displacer_radius);

// Shock Roach
DECLARE_SKILL_CVARS(plr_shockroachs);
DECLARE_SKILL_CVARS(plr_shockroachm);

// Spore Launcher
DECLARE_SKILL_CVARS(plr_spore);


// WORLD WEAPONS
cvar_t	sk_12mm_bullet1 = {"sk_12mm_bullet1","0"};
cvar_t	sk_12mm_bullet2 = {"sk_12mm_bullet2","0"};
cvar_t	sk_12mm_bullet3 = {"sk_12mm_bullet3","0"};

cvar_t	sk_9mmAR_bullet1 = {"sk_9mmAR_bullet1","0"};
cvar_t	sk_9mmAR_bullet2 = {"sk_9mmAR_bullet2","0"};
cvar_t	sk_9mmAR_bullet3 = {"sk_9mmAR_bullet3","0"};

cvar_t	sk_9mm_bullet1 = {"sk_9mm_bullet1","0"};
cvar_t	sk_9mm_bullet2 = {"sk_9mm_bullet2","0"};
cvar_t	sk_9mm_bullet3 = {"sk_9mm_bullet3","0"};


// HORNET
cvar_t	sk_hornet_dmg1 = {"sk_hornet_dmg1","0"};
cvar_t	sk_hornet_dmg2 = {"sk_hornet_dmg2","0"};
cvar_t	sk_hornet_dmg3 = {"sk_hornet_dmg3","0"};

// HEALTH/CHARGE
cvar_t	sk_suitcharger1	= { "sk_suitcharger1","0" };
cvar_t	sk_suitcharger2	= { "sk_suitcharger2","0" };		
cvar_t	sk_suitcharger3	= { "sk_suitcharger3","0" };		

cvar_t	sk_battery1	= { "sk_battery1","0" };			
cvar_t	sk_battery2	= { "sk_battery2","0" };			
cvar_t	sk_battery3	= { "sk_battery3","0" };			

cvar_t	sk_healthcharger1	= { "sk_healthcharger1","0" };		
cvar_t	sk_healthcharger2	= { "sk_healthcharger2","0" };		
cvar_t	sk_healthcharger3	= { "sk_healthcharger3","0" };		

cvar_t	sk_healthkit1	= { "sk_healthkit1","0" };		
cvar_t	sk_healthkit2	= { "sk_healthkit2","0" };		
cvar_t	sk_healthkit3	= { "sk_healthkit3","0" };		

cvar_t	sk_scientist_heal1	= { "sk_scientist_heal1","0" };	
cvar_t	sk_scientist_heal2	= { "sk_scientist_heal2","0" };	
cvar_t	sk_scientist_heal3	= { "sk_scientist_heal3","0" };	

DECLARE_SKILL_CVARS(cleansuit_scientist_heal);


// monster damage adjusters
cvar_t	sk_monster_head1	= { "sk_monster_head1","2" };
cvar_t	sk_monster_head2	= { "sk_monster_head2","2" };
cvar_t	sk_monster_head3	= { "sk_monster_head3","2" };

cvar_t	sk_monster_chest1	= { "sk_monster_chest1","1" };
cvar_t	sk_monster_chest2	= { "sk_monster_chest2","1" };
cvar_t	sk_monster_chest3	= { "sk_monster_chest3","1" };

cvar_t	sk_monster_stomach1	= { "sk_monster_stomach1","1" };
cvar_t	sk_monster_stomach2	= { "sk_monster_stomach2","1" };
cvar_t	sk_monster_stomach3	= { "sk_monster_stomach3","1" };

cvar_t	sk_monster_arm1	= { "sk_monster_arm1","1" };
cvar_t	sk_monster_arm2	= { "sk_monster_arm2","1" };
cvar_t	sk_monster_arm3	= { "sk_monster_arm3","1" };

cvar_t	sk_monster_leg1	= { "sk_monster_leg1","1" };
cvar_t	sk_monster_leg2	= { "sk_monster_leg2","1" };
cvar_t	sk_monster_leg3	= { "sk_monster_leg3","1" };

// player damage adjusters
cvar_t	sk_player_head1	= { "sk_player_head1","2" };
cvar_t	sk_player_head2	= { "sk_player_head2","2" };
cvar_t	sk_player_head3	= { "sk_player_head3","2" };

cvar_t	sk_player_chest1	= { "sk_player_chest1","1" };
cvar_t	sk_player_chest2	= { "sk_player_chest2","1" };
cvar_t	sk_player_chest3	= { "sk_player_chest3","1" };

cvar_t	sk_player_stomach1	= { "sk_player_stomach1","1" };
cvar_t	sk_player_stomach2	= { "sk_player_stomach2","1" };
cvar_t	sk_player_stomach3	= { "sk_player_stomach3","1" };

cvar_t	sk_player_arm1	= { "sk_player_arm1","1" };
cvar_t	sk_player_arm2	= { "sk_player_arm2","1" };
cvar_t	sk_player_arm3	= { "sk_player_arm3","1" };

cvar_t	sk_player_leg1	= { "sk_player_leg1","1" };
cvar_t	sk_player_leg2	= { "sk_player_leg2","1" };
cvar_t	sk_player_leg3	= { "sk_player_leg3","1" };

// save original pointer
decltype(g_engfuncs.pfnMessageBegin) g_originalMessageBegin = nullptr;

// wrapper for MessageBegin
void Au_MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed)
{
	NetProfiling::Submit(msg_type);
	g_originalMessageBegin(msg_dest, msg_type, pOrigin, ed);
}

// BlueNightHawk : Infinite Ammo
cvar_t	sv_aura_infinite_ammo = { "sv_aura_infinite_ammo","0", FCVAR_SERVER };

// BlueNightHawk : Suit Energy Regeneration
cvar_t sv_aura_regeneration = { "sv_aura_regeneration", "1", FCVAR_SERVER };
cvar_t sv_aura_regeneration_rate = { "sv_aura_regeneration_rate", "1", FCVAR_SERVER };
cvar_t sv_aura_regeneration_wait = { "sv_aura_regeneration_wait", "0.03", FCVAR_SERVER };

// END Cvars for Skill Level settings

cvar_t sv_pushable_fixed_tick_fudge = { "sv_pushable_fixed_tick_fudge", "15" };

// NETPROFILING MSGS START
extern int gmsgShake;
extern int gmsgFade;
extern int gmsgSelAmmo;
extern int gmsgFlashlight;
extern int gmsgFlashBattery;
extern int gmsgResetHUD;
extern int gmsgInitHUD;
extern int gmsgShowGameTitle;
extern int gmsgHealth;
extern int gmsgDamage;
extern int gmsgBattery;
extern int gmsgTrain;
extern int gmsgLogo;
extern int gmsgWeaponList;
extern int gmsgHudText;
extern int gmsgDeathMsg;
extern int gmsgScoreInfo;
extern int gmsgTeamInfo;
extern int gmsgTeamScore;
extern int gmsgGameMode;
extern int gmsgMOTD;
extern int gmsgServerName;
extern int gmsgAmmoPickup;
extern int gmsgWeapPickup;
extern int gmsgItemPickup;
extern int gmsgHideWeapon;
extern int gmsgSetCurWeap;
extern int gmsgSayText;
extern int gmsgTextMsg;
extern int gmsgSetFOV;
extern int gmsgShowMenu;
extern int gmsgGeigerRange;
extern int gmsgTeamNames;
extern int gmsgSetSky;

extern int gmsgStatusText;
extern int gmsgStatusValue;

//++ BulliT
extern int gmsgAllowSpec;
extern int gmsgSpectator;
extern int gmsgVGUIMenu;
extern int gmsgCheatCheck;
extern int gmsgSplash;
extern int gmsgCountdown;
extern int gmsgTimer;
extern int gmsgPlayerId;
extern int gmsgSettings;
extern int gmsgSuddenDeath;
extern int gmsgLongjump;
extern int gmsgTimeout;
extern int gmsgPlaySound;
extern int gmsgVote;
extern int gmsgNextmap;
extern int gmsgInitLoc;
extern int gmsgLocation;
extern int gmsgWallhack;
extern int gmsgSpikeCheck;
extern int gmsgGametype;
extern int gmsgStatusIcon;
extern int gmsgCTF;
extern int gmsgAuthID;
extern int gmsgCTFSound;
extern int gmsgMapList;
extern int gmsgCTFFlag;
extern int gmsgCRC32;
//-- Martin Webrant

extern int gmsgLaserSpot;

extern int gmsgCurWeapon;
extern int gmsgAmmoX;
extern int gmsgSayText;
extern int gmsgStatusText;
// NETPROFILING MSGS END

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	// store original function
	g_originalMessageBegin = g_engfuncs.pfnMessageBegin;

	// replace with our wrapper
	g_engfuncs.pfnMessageBegin = Au_MessageBegin;

#ifdef _DEBUG
	// Register readable names
	NetProfiling::RegisterName(gmsgHealth, "gmsgHealth");
	NetProfiling::RegisterName(gmsgDamage, "gmsgDamage");
	NetProfiling::RegisterName(gmsgCurWeapon, "gmsgCurWeapon");
	NetProfiling::RegisterName(gmsgAmmoX, "gmsgAmmoX");
	NetProfiling::RegisterName(gmsgSayText, "gmsgSayText");
	NetProfiling::RegisterName(gmsgStatusText, "gmsgStatusText");
	NetProfiling::RegisterName(gmsgShake, "gmsgShake");
	NetProfiling::RegisterName(gmsgFade, "gmsgFade");
	NetProfiling::RegisterName(gmsgSelAmmo, "gmsgSelAmmo");
	NetProfiling::RegisterName(gmsgFlashlight, "gmsgFlashlight");
	NetProfiling::RegisterName(gmsgFlashBattery, "gmsgFlashBattery");
	NetProfiling::RegisterName(gmsgResetHUD, "gmsgResetHUD");
	NetProfiling::RegisterName(gmsgInitHUD, "gmsgInitHUD");
	NetProfiling::RegisterName(gmsgShowGameTitle, "gmsgShowGameTitle");
	NetProfiling::RegisterName(gmsgTrain, "gmsgTrain");
	NetProfiling::RegisterName(gmsgLogo, "gmsgLogo");
	NetProfiling::RegisterName(gmsgWeaponList, "gmsgWeaponList");
	NetProfiling::RegisterName(gmsgHudText, "gmsgHudText");
	NetProfiling::RegisterName(gmsgDeathMsg, "gmsgDeathMsg");
	NetProfiling::RegisterName(gmsgScoreInfo, "gmsgScoreInfo");
	NetProfiling::RegisterName(gmsgTeamInfo, "gmsgTeamInfo");
	NetProfiling::RegisterName(gmsgTeamScore, "gmsgTeamScore");
	NetProfiling::RegisterName(gmsgGameMode, "gmsgGameMode");
	NetProfiling::RegisterName(gmsgMOTD, "gmsgMOTD");
	NetProfiling::RegisterName(gmsgServerName, "gmsgServerName");
	NetProfiling::RegisterName(gmsgAmmoPickup, "gmsgAmmoPickup");
	NetProfiling::RegisterName(gmsgWeapPickup, "gmsgWeapPickup");
	NetProfiling::RegisterName(gmsgItemPickup, "gmsgItemPickup");
	NetProfiling::RegisterName(gmsgHideWeapon, "gmsgHideWeapon");
	NetProfiling::RegisterName(gmsgSetCurWeap, "gmsgSetCurWeap");
	NetProfiling::RegisterName(gmsgTextMsg, "gmsgTextMsg");
	NetProfiling::RegisterName(gmsgSetFOV, "gmsgSetFOV");
	NetProfiling::RegisterName(gmsgShowMenu, "gmsgShowMenu");
	NetProfiling::RegisterName(gmsgGeigerRange, "gmsgGeigerRange");
	NetProfiling::RegisterName(gmsgTeamNames, "gmsgTeamNames");
	NetProfiling::RegisterName(gmsgSetSky, "gmsgSetSky");
	NetProfiling::RegisterName(gmsgStatusText, "gmsgStatusText");
	NetProfiling::RegisterName(gmsgStatusValue, "gmsgStatusValue");
	//++ BulliT
	NetProfiling::RegisterName(gmsgAllowSpec, "gmsgAllowSpec");
	NetProfiling::RegisterName(gmsgSpectator, "gmsgSpectator");
	NetProfiling::RegisterName(gmsgVGUIMenu, "gmsgVGUIMenu");
	NetProfiling::RegisterName(gmsgCheatCheck, "gmsgCheatCheck");
	NetProfiling::RegisterName(gmsgSplash, "gmsgSplash");
	NetProfiling::RegisterName(gmsgCountdown, "gmsgCountdown");
	NetProfiling::RegisterName(gmsgTimer, "gmsgTimer");
	NetProfiling::RegisterName(gmsgPlayerId, "gmsgPlayerId");
	NetProfiling::RegisterName(gmsgSettings, "gmsgSettings");
	NetProfiling::RegisterName(gmsgSuddenDeath, "gmsgSuddenDeath");
	NetProfiling::RegisterName(gmsgLongjump, "gmsgLongjump");
	NetProfiling::RegisterName(gmsgTimeout, "gmsgTimeout");
	NetProfiling::RegisterName(gmsgPlaySound, "gmsgPlaySound");
	NetProfiling::RegisterName(gmsgVote, "gmsgVote");
	NetProfiling::RegisterName(gmsgNextmap, "gmsgNextmap");
	NetProfiling::RegisterName(gmsgInitLoc, "gmsgInitLoc");
	NetProfiling::RegisterName(gmsgLocation, "gmsgLocation");
	NetProfiling::RegisterName(gmsgWallhack, "gmsgWallhack");
	NetProfiling::RegisterName(gmsgSpikeCheck, "gmsgSpikeCheck");
	NetProfiling::RegisterName(gmsgGametype, "gmsgGametype");
	NetProfiling::RegisterName(gmsgStatusIcon, "gmsgStatusIcon");
	NetProfiling::RegisterName(gmsgCTF, "gmsgCTF");
	NetProfiling::RegisterName(gmsgAuthID, "gmsgAuthID");
	NetProfiling::RegisterName(gmsgCTFSound, "gmsgCTFSound");
	NetProfiling::RegisterName(gmsgMapList, "gmsgMapList");
	NetProfiling::RegisterName(gmsgCTFFlag, "gmsgCTFFlag");
	NetProfiling::RegisterName(gmsgCRC32, "gmsgCRC32");
	//-- Martin Webrant
	NetProfiling::RegisterName(gmsgLaserSpot, "gmsgLaserSpot");
	NetProfiling::RegisterName(gmsgSayText, "gmsgSayText");
	NetProfiling::RegisterName(gmsgStatusText, "gmsgStatusText");
#endif // _DEBUG


	// Register cvars here:
#ifdef _DEBUG
	g_engfuncs.pfnAddServerCommand("dump_net_stats", []()
		{
			NetProfiling::GetProfiler().DumpStats();
		});
#endif

	g_psv_gravity = CVAR_GET_POINTER( "sv_gravity" );
	g_psv_aim = CVAR_GET_POINTER( "sv_aim" );
	g_psv_allow_autoaim = CVAR_GET_POINTER("sv_allow_autoaim");
	g_footsteps = CVAR_GET_POINTER( "mp_footsteps" );

	CVAR_REGISTER (&displaysoundlist);

	CVAR_REGISTER (&teamplay);
	CVAR_REGISTER (&fraglimit);
	CVAR_REGISTER (&timelimit);

	CVAR_REGISTER (&fragsleft);
	CVAR_REGISTER (&timeleft);

	CVAR_REGISTER (&friendlyfire);
	CVAR_REGISTER (&falldamage);
	CVAR_REGISTER (&weaponstay);
	CVAR_REGISTER (&forcerespawn);
	CVAR_REGISTER (&flashlight);
	CVAR_REGISTER (&aimcrosshair);
	CVAR_REGISTER (&decalfrequency);
	CVAR_REGISTER (&teamlist);
	CVAR_REGISTER (&teamoverride);
	CVAR_REGISTER (&defaultteam);
	CVAR_REGISTER(&coopmode);
	CVAR_REGISTER (&allowmonsters);

	CVAR_REGISTER (&mp5_old_rate);
	CVAR_REGISTER(&fire_weapons_underwater);

	CVAR_REGISTER (&mp_chattime);
	CVAR_REGISTER (&mp_intermission_time);
	CVAR_REGISTER(&mp_intermission_skip_auto);

	CVAR_REGISTER (&singleplayer);
	CVAR_REGISTER (&sploading);
	CVAR_REGISTER (&spgausscharging);

// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Agrunt
	CVAR_REGISTER ( &sk_agrunt_health1 );// {"sk_agrunt_health1","0"};
	CVAR_REGISTER ( &sk_agrunt_health2 );// {"sk_agrunt_health2","0"};
	CVAR_REGISTER ( &sk_agrunt_health3 );// {"sk_agrunt_health3","0"};

	CVAR_REGISTER ( &sk_agrunt_dmg_punch1 );// {"sk_agrunt_dmg_punch1","0"};
	CVAR_REGISTER ( &sk_agrunt_dmg_punch2 );// {"sk_agrunt_dmg_punch2","0"};
	CVAR_REGISTER ( &sk_agrunt_dmg_punch3 );// {"sk_agrunt_dmg_punch3","0"};

	// Apache
	CVAR_REGISTER ( &sk_apache_health1 );// {"sk_apache_health1","0"};
	CVAR_REGISTER ( &sk_apache_health2 );// {"sk_apache_health2","0"};
	CVAR_REGISTER ( &sk_apache_health3 );// {"sk_apache_health3","0"};

	// Barney
	CVAR_REGISTER ( &sk_barney_health1 );// {"sk_barney_health1","0"};
	CVAR_REGISTER ( &sk_barney_health2 );// {"sk_barney_health2","0"};
	CVAR_REGISTER ( &sk_barney_health3 );// {"sk_barney_health3","0"};

	// Otis
	REGISTER_SKILL_CVARS(otis_health);

	// Bullsquid
	CVAR_REGISTER ( &sk_bullsquid_health1 );// {"sk_bullsquid_health1","0"};
	CVAR_REGISTER ( &sk_bullsquid_health2 );// {"sk_bullsquid_health2","0"};
	CVAR_REGISTER ( &sk_bullsquid_health3 );// {"sk_bullsquid_health3","0"};

	CVAR_REGISTER ( &sk_bullsquid_dmg_bite1 );// {"sk_bullsquid_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_bite2 );// {"sk_bullsquid_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_bite3 );// {"sk_bullsquid_dmg_bite3","0"};

	CVAR_REGISTER ( &sk_bullsquid_dmg_whip1 );// {"sk_bullsquid_dmg_whip1","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_whip2 );// {"sk_bullsquid_dmg_whip2","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_whip3 );// {"sk_bullsquid_dmg_whip3","0"};

	CVAR_REGISTER ( &sk_bullsquid_dmg_spit1 );// {"sk_bullsquid_dmg_spit1","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_spit2 );// {"sk_bullsquid_dmg_spit2","0"};
	CVAR_REGISTER ( &sk_bullsquid_dmg_spit3 );// {"sk_bullsquid_dmg_spit3","0"};


		// Pit Drone
	REGISTER_SKILL_CVARS(pitdrone_health);
	REGISTER_SKILL_CVARS(pitdrone_dmg_bite);
	REGISTER_SKILL_CVARS(pitdrone_dmg_whip);
	REGISTER_SKILL_CVARS(pitdrone_dmg_spit);


	// Big Momma
	CVAR_REGISTER ( &sk_bigmomma_health_factor1 );// {"sk_bigmomma_health_factor1","1.0"};
	CVAR_REGISTER ( &sk_bigmomma_health_factor2 );// {"sk_bigmomma_health_factor2","1.0"};
	CVAR_REGISTER ( &sk_bigmomma_health_factor3 );// {"sk_bigmomma_health_factor3","1.0"};

	CVAR_REGISTER ( &sk_bigmomma_dmg_slash1 );// {"sk_bigmomma_dmg_slash1","50"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_slash2 );// {"sk_bigmomma_dmg_slash2","50"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_slash3 );// {"sk_bigmomma_dmg_slash3","50"};

	CVAR_REGISTER ( &sk_bigmomma_dmg_blast1 );// {"sk_bigmomma_dmg_blast1","100"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_blast2 );// {"sk_bigmomma_dmg_blast2","100"};
	CVAR_REGISTER ( &sk_bigmomma_dmg_blast3 );// {"sk_bigmomma_dmg_blast3","100"};

	CVAR_REGISTER ( &sk_bigmomma_radius_blast1 );// {"sk_bigmomma_radius_blast1","250"};
	CVAR_REGISTER ( &sk_bigmomma_radius_blast2 );// {"sk_bigmomma_radius_blast2","250"};
	CVAR_REGISTER ( &sk_bigmomma_radius_blast3 );// {"sk_bigmomma_radius_blast3","250"};

	// Gargantua
	CVAR_REGISTER ( &sk_gargantua_health1 );// {"sk_gargantua_health1","0"};
	CVAR_REGISTER ( &sk_gargantua_health2 );// {"sk_gargantua_health2","0"};
	CVAR_REGISTER ( &sk_gargantua_health3 );// {"sk_gargantua_health3","0"};

	CVAR_REGISTER ( &sk_gargantua_dmg_slash1 );// {"sk_gargantua_dmg_slash1","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_slash2 );// {"sk_gargantua_dmg_slash2","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_slash3 );// {"sk_gargantua_dmg_slash3","0"};

	CVAR_REGISTER ( &sk_gargantua_dmg_fire1 );// {"sk_gargantua_dmg_fire1","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_fire2 );// {"sk_gargantua_dmg_fire2","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_fire3 );// {"sk_gargantua_dmg_fire3","0"};

	CVAR_REGISTER ( &sk_gargantua_dmg_stomp1 );// {"sk_gargantua_dmg_stomp1","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_stomp2 );// {"sk_gargantua_dmg_stomp2","0"};
	CVAR_REGISTER ( &sk_gargantua_dmg_stomp3	);// {"sk_gargantua_dmg_stomp3","0"};


	// Hassassin
	CVAR_REGISTER ( &sk_hassassin_health1 );// {"sk_hassassin_health1","0"};
	CVAR_REGISTER ( &sk_hassassin_health2 );// {"sk_hassassin_health2","0"};
	CVAR_REGISTER ( &sk_hassassin_health3 );// {"sk_hassassin_health3","0"};


	// Headcrab
	CVAR_REGISTER ( &sk_headcrab_health1 );// {"sk_headcrab_health1","0"};
	CVAR_REGISTER ( &sk_headcrab_health2 );// {"sk_headcrab_health2","0"};
	CVAR_REGISTER ( &sk_headcrab_health3 );// {"sk_headcrab_health3","0"};

	CVAR_REGISTER ( &sk_headcrab_dmg_bite1 );// {"sk_headcrab_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_headcrab_dmg_bite2 );// {"sk_headcrab_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_headcrab_dmg_bite3 );// {"sk_headcrab_dmg_bite3","0"};

	// Shock Roach
	REGISTER_SKILL_CVARS(shockroach_health);
	REGISTER_SKILL_CVARS(shockroach_dmg_bite);
	REGISTER_SKILL_CVARS(shockroach_lifespan);


	// Hgrunt 
	CVAR_REGISTER ( &sk_hgrunt_health1 );// {"sk_hgrunt_health1","0"};
	CVAR_REGISTER ( &sk_hgrunt_health2 );// {"sk_hgrunt_health2","0"};
	CVAR_REGISTER ( &sk_hgrunt_health3 );// {"sk_hgrunt_health3","0"};

	CVAR_REGISTER ( &sk_hgrunt_kick1 );// {"sk_hgrunt_kick1","0"};
	CVAR_REGISTER ( &sk_hgrunt_kick2 );// {"sk_hgrunt_kick2","0"};
	CVAR_REGISTER ( &sk_hgrunt_kick3 );// {"sk_hgrunt_kick3","0"};

	CVAR_REGISTER ( &sk_hgrunt_pellets1 );
	CVAR_REGISTER ( &sk_hgrunt_pellets2 );
	CVAR_REGISTER ( &sk_hgrunt_pellets3 );

	CVAR_REGISTER ( &sk_hgrunt_gspeed1 );
	CVAR_REGISTER ( &sk_hgrunt_gspeed2 );
	CVAR_REGISTER ( &sk_hgrunt_gspeed3 );

	// Hgrunt Ally
	REGISTER_SKILL_CVARS(hgrunt_ally_health);
	REGISTER_SKILL_CVARS(hgrunt_ally_kick);
	REGISTER_SKILL_CVARS(hgrunt_ally_pellets);
	REGISTER_SKILL_CVARS(hgrunt_ally_gspeed);

	// Hgrunt Medic
	REGISTER_SKILL_CVARS(medic_ally_health);
	REGISTER_SKILL_CVARS(medic_ally_kick);
	REGISTER_SKILL_CVARS(medic_ally_pellets);
	REGISTER_SKILL_CVARS(medic_ally_gspeed);
	REGISTER_SKILL_CVARS(medic_ally_heal);

	// Hgrunt Torch
	REGISTER_SKILL_CVARS(torch_ally_health);
	REGISTER_SKILL_CVARS(torch_ally_kick);
	REGISTER_SKILL_CVARS(torch_ally_pellets);
	REGISTER_SKILL_CVARS(torch_ally_gspeed);

	// Male Assassin
	REGISTER_SKILL_CVARS(massassin_health);
	REGISTER_SKILL_CVARS(massassin_kick);
	REGISTER_SKILL_CVARS(massassin_pellets);
	REGISTER_SKILL_CVARS(massassin_gspeed);

	// Shock Trooper
	REGISTER_SKILL_CVARS(shocktrooper_health);
	REGISTER_SKILL_CVARS(shocktrooper_kick);
	REGISTER_SKILL_CVARS(shocktrooper_gspeed);
	REGISTER_SKILL_CVARS(shocktrooper_maxcharge);
	REGISTER_SKILL_CVARS(shocktrooper_rchgspeed);

	// Houndeye
	CVAR_REGISTER ( &sk_houndeye_health1 );// {"sk_houndeye_health1","0"};
	CVAR_REGISTER ( &sk_houndeye_health2 );// {"sk_houndeye_health2","0"};
	CVAR_REGISTER ( &sk_houndeye_health3 );// {"sk_houndeye_health3","0"};

	CVAR_REGISTER ( &sk_houndeye_dmg_blast1 );// {"sk_houndeye_dmg_blast1","0"};
	CVAR_REGISTER ( &sk_houndeye_dmg_blast2 );// {"sk_houndeye_dmg_blast2","0"};
	CVAR_REGISTER ( &sk_houndeye_dmg_blast3 );// {"sk_houndeye_dmg_blast3","0"};


	// ISlave
	CVAR_REGISTER ( &sk_islave_health1 );// {"sk_islave_health1","0"};
	CVAR_REGISTER ( &sk_islave_health2 );// {"sk_islave_health2","0"};
	CVAR_REGISTER ( &sk_islave_health3 );// {"sk_islave_health3","0"};

	CVAR_REGISTER ( &sk_islave_dmg_claw1 );// {"sk_islave_dmg_claw1","0"};
	CVAR_REGISTER ( &sk_islave_dmg_claw2 );// {"sk_islave_dmg_claw2","0"};
	CVAR_REGISTER ( &sk_islave_dmg_claw3 );// {"sk_islave_dmg_claw3","0"};

	CVAR_REGISTER ( &sk_islave_dmg_clawrake1	);// {"sk_islave_dmg_clawrake1","0"};
	CVAR_REGISTER ( &sk_islave_dmg_clawrake2	);// {"sk_islave_dmg_clawrake2","0"};
	CVAR_REGISTER ( &sk_islave_dmg_clawrake3	);// {"sk_islave_dmg_clawrake3","0"};
		
	CVAR_REGISTER ( &sk_islave_dmg_zap1 );// {"sk_islave_dmg_zap1","0"};
	CVAR_REGISTER ( &sk_islave_dmg_zap2 );// {"sk_islave_dmg_zap2","0"};
	CVAR_REGISTER ( &sk_islave_dmg_zap3 );// {"sk_islave_dmg_zap3","0"};


	// Icthyosaur
	CVAR_REGISTER ( &sk_ichthyosaur_health1	);// {"sk_ichthyosaur_health1","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_health2	);// {"sk_ichthyosaur_health2","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_health3	);// {"sk_ichthyosaur_health3","0"};

	CVAR_REGISTER ( &sk_ichthyosaur_shake1	);// {"sk_ichthyosaur_health3","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_shake2	);// {"sk_ichthyosaur_health3","0"};
	CVAR_REGISTER ( &sk_ichthyosaur_shake3	);// {"sk_ichthyosaur_health3","0"};



	// Leech
	CVAR_REGISTER ( &sk_leech_health1 );// {"sk_leech_health1","0"};
	CVAR_REGISTER ( &sk_leech_health2 );// {"sk_leech_health2","0"};
	CVAR_REGISTER ( &sk_leech_health3 );// {"sk_leech_health3","0"};

	CVAR_REGISTER ( &sk_leech_dmg_bite1 );// {"sk_leech_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_leech_dmg_bite2 );// {"sk_leech_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_leech_dmg_bite3 );// {"sk_leech_dmg_bite3","0"};


	// Controller
	CVAR_REGISTER ( &sk_controller_health1 );
	CVAR_REGISTER ( &sk_controller_health2 );
	CVAR_REGISTER ( &sk_controller_health3 );

	CVAR_REGISTER ( &sk_controller_dmgzap1 );
	CVAR_REGISTER ( &sk_controller_dmgzap2 );
	CVAR_REGISTER ( &sk_controller_dmgzap3 );

	CVAR_REGISTER ( &sk_controller_speedball1 );
	CVAR_REGISTER ( &sk_controller_speedball2 );
	CVAR_REGISTER ( &sk_controller_speedball3 );

	CVAR_REGISTER ( &sk_controller_dmgball1 );
	CVAR_REGISTER ( &sk_controller_dmgball2 );
	CVAR_REGISTER ( &sk_controller_dmgball3 );

	// Nihilanth
	CVAR_REGISTER ( &sk_nihilanth_health1 );// {"sk_nihilanth_health1","0"};
	CVAR_REGISTER ( &sk_nihilanth_health2 );// {"sk_nihilanth_health2","0"};
	CVAR_REGISTER ( &sk_nihilanth_health3 );// {"sk_nihilanth_health3","0"};

	CVAR_REGISTER ( &sk_nihilanth_zap1 );
	CVAR_REGISTER ( &sk_nihilanth_zap2 );
	CVAR_REGISTER ( &sk_nihilanth_zap3 );

	// Scientist
	CVAR_REGISTER ( &sk_scientist_health1 );// {"sk_scientist_health1","0"};
	CVAR_REGISTER ( &sk_scientist_health2 );// {"sk_scientist_health2","0"};
	CVAR_REGISTER ( &sk_scientist_health3 );// {"sk_scientist_health3","0"};

	// Cleansuit Scientist
	REGISTER_SKILL_CVARS(cleansuit_scientist_health);


	// Snark
	CVAR_REGISTER ( &sk_snark_health1 );// {"sk_snark_health1","0"};
	CVAR_REGISTER ( &sk_snark_health2 );// {"sk_snark_health2","0"};
	CVAR_REGISTER ( &sk_snark_health3 );// {"sk_snark_health3","0"};

	CVAR_REGISTER ( &sk_snark_dmg_bite1 );// {"sk_snark_dmg_bite1","0"};
	CVAR_REGISTER ( &sk_snark_dmg_bite2 );// {"sk_snark_dmg_bite2","0"};
	CVAR_REGISTER ( &sk_snark_dmg_bite3 );// {"sk_snark_dmg_bite3","0"};

	CVAR_REGISTER ( &sk_snark_dmg_pop1 );// {"sk_snark_dmg_pop1","0"};
	CVAR_REGISTER ( &sk_snark_dmg_pop2 );// {"sk_snark_dmg_pop2","0"};
	CVAR_REGISTER ( &sk_snark_dmg_pop3 );// {"sk_snark_dmg_pop3","0"};

	// Voltigore
	REGISTER_SKILL_CVARS(voltigore_health);
	REGISTER_SKILL_CVARS(voltigore_dmg_punch);
	REGISTER_SKILL_CVARS(voltigore_dmg_beam);

	// Baby Voltigore
	REGISTER_SKILL_CVARS(babyvoltigore_health);
	REGISTER_SKILL_CVARS(babyvoltigore_dmg_punch);



	// Zombie
	CVAR_REGISTER ( &sk_zombie_health1 );// {"sk_zombie_health1","0"};
	CVAR_REGISTER ( &sk_zombie_health2 );// {"sk_zombie_health3","0"};
	CVAR_REGISTER ( &sk_zombie_health3 );// {"sk_zombie_health3","0"};

	CVAR_REGISTER ( &sk_zombie_dmg_one_slash1 );// {"sk_zombie_dmg_one_slash1","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_one_slash2 );// {"sk_zombie_dmg_one_slash2","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_one_slash3 );// {"sk_zombie_dmg_one_slash3","0"};

	CVAR_REGISTER ( &sk_zombie_dmg_both_slash1 );// {"sk_zombie_dmg_both_slash1","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_both_slash2 );// {"sk_zombie_dmg_both_slash2","0"};
	CVAR_REGISTER ( &sk_zombie_dmg_both_slash3 );// {"sk_zombie_dmg_both_slash3","0"};


	// Zombie Barney
	REGISTER_SKILL_CVARS(zombie_barney_health);
	REGISTER_SKILL_CVARS(zombie_barney_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_barney_dmg_both_slash);

	// Zombie Soldier
	REGISTER_SKILL_CVARS(zombie_soldier_health);
	REGISTER_SKILL_CVARS(zombie_soldier_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_soldier_dmg_both_slash);

	// Gonome
	REGISTER_SKILL_CVARS(gonome_dmg_guts);
	REGISTER_SKILL_CVARS(gonome_health);
	REGISTER_SKILL_CVARS(gonome_dmg_one_slash);
	REGISTER_SKILL_CVARS(gonome_dmg_one_bite);



	//Turret
	CVAR_REGISTER ( &sk_turret_health1 );// {"sk_turret_health1","0"};
	CVAR_REGISTER ( &sk_turret_health2 );// {"sk_turret_health2","0"};
	CVAR_REGISTER ( &sk_turret_health3 );// {"sk_turret_health3","0"};


	// MiniTurret
	CVAR_REGISTER ( &sk_miniturret_health1 );// {"sk_miniturret_health1","0"};
	CVAR_REGISTER ( &sk_miniturret_health2 );// {"sk_miniturret_health2","0"};
	CVAR_REGISTER ( &sk_miniturret_health3 );// {"sk_miniturret_health3","0"};


	// Sentry Turret
	CVAR_REGISTER ( &sk_sentry_health1 );// {"sk_sentry_health1","0"};
	CVAR_REGISTER ( &sk_sentry_health2 );// {"sk_sentry_health2","0"};
	CVAR_REGISTER ( &sk_sentry_health3 );// {"sk_sentry_health3","0"};


	// PLAYER WEAPONS

	// Crowbar whack
	CVAR_REGISTER ( &sk_plr_crowbar1 );// {"sk_plr_crowbar1","0"};
	CVAR_REGISTER ( &sk_plr_crowbar2 );// {"sk_plr_crowbar2","0"};
	CVAR_REGISTER ( &sk_plr_crowbar3 );// {"sk_plr_crowbar3","0"};

	// Glock Round
	CVAR_REGISTER ( &sk_plr_9mm_bullet1 );// {"sk_plr_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_plr_9mm_bullet2 );// {"sk_plr_9mm_bullet2","0"};
	CVAR_REGISTER ( &sk_plr_9mm_bullet3 );// {"sk_plr_9mm_bullet3","0"};

	// 357 Round
	CVAR_REGISTER ( &sk_plr_357_bullet1 );// {"sk_plr_357_bullet1","0"};
	CVAR_REGISTER ( &sk_plr_357_bullet2 );// {"sk_plr_357_bullet2","0"};
	CVAR_REGISTER ( &sk_plr_357_bullet3 );// {"sk_plr_357_bullet3","0"};

	// MP5 Round
	CVAR_REGISTER ( &sk_plr_9mmAR_bullet1 );// {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_bullet2 );// {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_bullet3 );// {"sk_plr_9mmAR_bullet3","0"};


	// M203 grenade
	CVAR_REGISTER ( &sk_plr_9mmAR_grenade1 );// {"sk_plr_9mmAR_grenade1","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_grenade2 );// {"sk_plr_9mmAR_grenade2","0"};
	CVAR_REGISTER ( &sk_plr_9mmAR_grenade3 );// {"sk_plr_9mmAR_grenade3","0"};


	// Shotgun buckshot
	CVAR_REGISTER ( &sk_plr_buckshot1 );// {"sk_plr_buckshot1","0"};
	CVAR_REGISTER ( &sk_plr_buckshot2 );// {"sk_plr_buckshot2","0"};
	CVAR_REGISTER ( &sk_plr_buckshot3 );// {"sk_plr_buckshot3","0"};


	// Crossbow
	CVAR_REGISTER ( &sk_plr_xbow_bolt_monster1 );// {"sk_plr_xbow_bolt1","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_monster2 );// {"sk_plr_xbow_bolt2","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_monster3 );// {"sk_plr_xbow_bolt3","0"};

	CVAR_REGISTER ( &sk_plr_xbow_bolt_client1 );// {"sk_plr_xbow_bolt1","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_client2 );// {"sk_plr_xbow_bolt2","0"};
	CVAR_REGISTER ( &sk_plr_xbow_bolt_client3 );// {"sk_plr_xbow_bolt3","0"};


	// RPG
	CVAR_REGISTER ( &sk_plr_rpg1 );// {"sk_plr_rpg1","0"};
	CVAR_REGISTER ( &sk_plr_rpg2 );// {"sk_plr_rpg2","0"};
	CVAR_REGISTER ( &sk_plr_rpg3 );// {"sk_plr_rpg3","0"};


	// Gauss Gun
	CVAR_REGISTER ( &sk_plr_gauss1 );// {"sk_plr_gauss1","0"};
	CVAR_REGISTER ( &sk_plr_gauss2 );// {"sk_plr_gauss2","0"};
	CVAR_REGISTER ( &sk_plr_gauss3 );// {"sk_plr_gauss3","0"};


	// Egon Gun
	CVAR_REGISTER ( &sk_plr_egon_narrow1 );// {"sk_plr_egon_narrow1","0"};
	CVAR_REGISTER ( &sk_plr_egon_narrow2 );// {"sk_plr_egon_narrow2","0"};
	CVAR_REGISTER ( &sk_plr_egon_narrow3 );// {"sk_plr_egon_narrow3","0"};

	CVAR_REGISTER ( &sk_plr_egon_wide1 );// {"sk_plr_egon_wide1","0"};
	CVAR_REGISTER ( &sk_plr_egon_wide2 );// {"sk_plr_egon_wide2","0"};
	CVAR_REGISTER ( &sk_plr_egon_wide3 );// {"sk_plr_egon_wide3","0"};


	// Hand Grendade
	CVAR_REGISTER ( &sk_plr_hand_grenade1 );// {"sk_plr_hand_grenade1","0"};
	CVAR_REGISTER ( &sk_plr_hand_grenade2 );// {"sk_plr_hand_grenade2","0"};
	CVAR_REGISTER ( &sk_plr_hand_grenade3 );// {"sk_plr_hand_grenade3","0"};


	// Satchel Charge
	CVAR_REGISTER ( &sk_plr_satchel1 );// {"sk_plr_satchel1","0"};
	CVAR_REGISTER ( &sk_plr_satchel2 );// {"sk_plr_satchel2","0"};
	CVAR_REGISTER ( &sk_plr_satchel3 );// {"sk_plr_satchel3","0"};


	// Tripmine
	CVAR_REGISTER ( &sk_plr_tripmine1 );// {"sk_plr_tripmine1","0"};
	CVAR_REGISTER ( &sk_plr_tripmine2 );// {"sk_plr_tripmine2","0"};
	CVAR_REGISTER ( &sk_plr_tripmine3 );// {"sk_plr_tripmine3","0"};

	// Pipe Wrench
	REGISTER_SKILL_CVARS(plr_pipewrench);

	// Knife
	REGISTER_SKILL_CVARS(plr_knife);

	// Grapple
	REGISTER_SKILL_CVARS(plr_grapple);

	// Desert Eagle
	REGISTER_SKILL_CVARS(plr_eagle);

	// Sniper Rifle
	REGISTER_SKILL_CVARS(plr_762_bullet);

	// M249
	REGISTER_SKILL_CVARS(plr_556_bullet);

	// Displacer
	REGISTER_SKILL_CVARS(plr_displacer_self);
	REGISTER_SKILL_CVARS(plr_displacer_other);
	REGISTER_SKILL_CVARS(plr_displacer_radius);

	// Shock Roach
	REGISTER_SKILL_CVARS(plr_shockroachs);
	REGISTER_SKILL_CVARS(plr_shockroachm);

	// Spore Launcher
	REGISTER_SKILL_CVARS(plr_spore);


	// WORLD WEAPONS
	CVAR_REGISTER ( &sk_12mm_bullet1 );// {"sk_12mm_bullet1","0"};
	CVAR_REGISTER ( &sk_12mm_bullet2 );// {"sk_12mm_bullet2","0"};
	CVAR_REGISTER ( &sk_12mm_bullet3 );// {"sk_12mm_bullet3","0"};

	CVAR_REGISTER ( &sk_9mmAR_bullet1 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_9mmAR_bullet2 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_9mmAR_bullet3 );// {"sk_9mm_bullet1","0"};

	CVAR_REGISTER ( &sk_9mm_bullet1 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER ( &sk_9mm_bullet2 );// {"sk_9mm_bullet2","0"};
	CVAR_REGISTER ( &sk_9mm_bullet3 );// {"sk_9mm_bullet3","0"};


	// HORNET
	CVAR_REGISTER ( &sk_hornet_dmg1 );// {"sk_hornet_dmg1","0"};
	CVAR_REGISTER ( &sk_hornet_dmg2 );// {"sk_hornet_dmg2","0"};
	CVAR_REGISTER ( &sk_hornet_dmg3 );// {"sk_hornet_dmg3","0"};

	// HEALTH/SUIT CHARGE DISTRIBUTION
	CVAR_REGISTER ( &sk_suitcharger1 );
	CVAR_REGISTER ( &sk_suitcharger2 );
	CVAR_REGISTER ( &sk_suitcharger3 );

	CVAR_REGISTER ( &sk_battery1 );
	CVAR_REGISTER ( &sk_battery2 );
	CVAR_REGISTER ( &sk_battery3 );

	CVAR_REGISTER ( &sk_healthcharger1 );
	CVAR_REGISTER ( &sk_healthcharger2 );
	CVAR_REGISTER ( &sk_healthcharger3 );

	CVAR_REGISTER ( &sk_healthkit1 );
	CVAR_REGISTER ( &sk_healthkit2 );
	CVAR_REGISTER ( &sk_healthkit3 );

	CVAR_REGISTER ( &sk_scientist_heal1 );
	CVAR_REGISTER ( &sk_scientist_heal2 );
	CVAR_REGISTER ( &sk_scientist_heal3 );

	REGISTER_SKILL_CVARS(cleansuit_scientist_heal);

// monster damage adjusters
	CVAR_REGISTER ( &sk_monster_head1 );
	CVAR_REGISTER ( &sk_monster_head2 );
	CVAR_REGISTER ( &sk_monster_head3 );

	CVAR_REGISTER ( &sk_monster_chest1 );
	CVAR_REGISTER ( &sk_monster_chest2 );
	CVAR_REGISTER ( &sk_monster_chest3 );

	CVAR_REGISTER ( &sk_monster_stomach1 );
	CVAR_REGISTER ( &sk_monster_stomach2 );
	CVAR_REGISTER ( &sk_monster_stomach3 );

	CVAR_REGISTER ( &sk_monster_arm1 );
	CVAR_REGISTER ( &sk_monster_arm2 );
	CVAR_REGISTER ( &sk_monster_arm3 );

	CVAR_REGISTER ( &sk_monster_leg1 );
	CVAR_REGISTER ( &sk_monster_leg2 );
	CVAR_REGISTER ( &sk_monster_leg3 );

// player damage adjusters
	CVAR_REGISTER ( &sk_player_head1 );
	CVAR_REGISTER ( &sk_player_head2 );
	CVAR_REGISTER ( &sk_player_head3 );

	CVAR_REGISTER ( &sk_player_chest1 );
	CVAR_REGISTER ( &sk_player_chest2 );
	CVAR_REGISTER ( &sk_player_chest3 );

	CVAR_REGISTER ( &sk_player_stomach1 );
	CVAR_REGISTER ( &sk_player_stomach2 );
	CVAR_REGISTER ( &sk_player_stomach3 );

	CVAR_REGISTER ( &sk_player_arm1 );
	CVAR_REGISTER ( &sk_player_arm2 );
	CVAR_REGISTER ( &sk_player_arm3 );

	CVAR_REGISTER ( &sk_player_leg1 );
	CVAR_REGISTER ( &sk_player_leg2 );
	CVAR_REGISTER ( &sk_player_leg3 );

	// BlueNightHawk : Infinite Ammo
	CVAR_REGISTER(&sv_aura_infinite_ammo);

	// BlueNightHawk : Suit Energy Regeneration
	CVAR_REGISTER(&sv_aura_regeneration);
	CVAR_REGISTER(&sv_aura_regeneration_rate);
	CVAR_REGISTER(&sv_aura_regeneration_wait);

// END REGISTER CVARS FOR SKILL LEVEL STUFF

	CVAR_REGISTER(&sv_pushable_fixed_tick_fudge);
 
	//++ BulliT
	AgInitGame();
	//-- Martin Webrant
	SERVER_COMMAND( "exec skill.cfg\n" );
	SERVER_COMMAND("exec skillopfor.cfg\n");
}

