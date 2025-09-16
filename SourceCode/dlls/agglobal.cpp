//++ BulliT

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "trains.h"
#include "nodes.h"
#include "weapons.h"
#include "soundent.h"
#include "monsters.h"
#include "../engine/shake.h"
#include "agglobal.h"
#include "agcommand.h"
#include "agwallhack.h"
#include "gamerules.h"
#include <time.h>
#include <map>
#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif


void AgInitTimer();

#ifdef AG_NO_CLIENT_DLL
DLL_GLOBAL cvar_t	ag_version = { "sv_aura_version","2.4", FCVAR_SERVER };
#else
DLL_GLOBAL cvar_t	ag_version = { "sv_aura_version","2.4", FCVAR_SERVER };
#endif

DLL_GLOBAL cvar_t	ag_gamemode = { "sv_aura_gamemode","ffa", FCVAR_SERVER }; //The current gamemode
DLL_GLOBAL cvar_t	ag_gamemode_auto = { "sv_aura_gamemode_auto","1", FCVAR_SERVER }; // Detect the gamemode based on the map and switch gamemode automatically
DLL_GLOBAL cvar_t	ag_allowed_gamemodes = { "sv_aura_allowed_gamemodes","" };

DLL_GLOBAL cvar_t	ag_pure = { "sv_aura_pure","0",FCVAR_SERVER };     //Default off.

DLL_GLOBAL cvar_t	ag_match_running = { "sv_aura_match_running","0",FCVAR_SERVER | FCVAR_UNLOGGED };          //Default not match. Protects players from wondering into the server.

DLL_GLOBAL cvar_t	ag_oldphysics = { "sv_aura_oldphysics","1" };

DLL_GLOBAL cvar_t	ag_allow_timeout = { "sv_aura_allow_timeout","1" };       //Allow timeout.

// TODO: review if these should really be unlogged, I think it has some interest to log them and shouldn't be spammy
DLL_GLOBAL cvar_t	ag_allow_vote = { "sv_aura_allow_vote","1", FCVAR_SERVER | FCVAR_UNLOGGED };       //Voting is enabled by default.
DLL_GLOBAL cvar_t	ag_vote_start = { "sv_aura_vote_start","1", FCVAR_SERVER | FCVAR_UNLOGGED };                    //Start voting enabled by default
DLL_GLOBAL cvar_t	ag_vote_setting = { "sv_aura_vote_setting","1" };                  //Setting voting enabled by default
DLL_GLOBAL cvar_t	ag_vote_gamemode = { "sv_aura_vote_gamemode","1", FCVAR_SERVER | FCVAR_UNLOGGED };                 //Gamemode voting is enabled by default.
DLL_GLOBAL cvar_t	ag_vote_kick = { "sv_aura_vote_kick","0" };                     //Kick voting is disabled by default. // TODO: review if this should be defaulted to 1
DLL_GLOBAL cvar_t	ag_vote_allow = { "sv_aura_vote_allow","1" };                    //All voting is enabled by default.
DLL_GLOBAL cvar_t	ag_vote_admin = { "sv_aura_vote_admin","0" };                    //Admin voting is disabled by default.
DLL_GLOBAL cvar_t	ag_vote_map = { "sv_aura_vote_map","1", FCVAR_SERVER | FCVAR_UNLOGGED };                      //Map voting is enabled by default.
DLL_GLOBAL cvar_t	ag_vote_failed_time = { "sv_aura_vote_failed_time","30" };

DLL_GLOBAL cvar_t	ag_start_minplayers = { "sv_aura_start_minplayers","2" };

DLL_GLOBAL cvar_t	ag_vote_mp_timelimit_low = { "sv_aura_vote_mp_timelimit_low","10" };
DLL_GLOBAL cvar_t	ag_vote_mp_timelimit_high = { "sv_aura_vote_mp_timelimit_high","40" };
DLL_GLOBAL cvar_t	ag_vote_mp_fraglimit_low = { "sv_aura_vote_mp_fraglimit_low","0" };
DLL_GLOBAL cvar_t	ag_vote_mp_fraglimit_high = { "sv_aura_vote_mp_fraglimit_high","100" };
DLL_GLOBAL cvar_t	ag_vote_extra_timelimit = { "sv_aura_vote_extra_timelimit","30" };

DLL_GLOBAL cvar_t	ag_floodmsgs = { "sv_aura_floodmsgs","4" };
DLL_GLOBAL cvar_t	ag_floodpersecond = { "sv_aura_floodpersecond","4" };
DLL_GLOBAL cvar_t	ag_floodwaitdelay = { "sv_aura_floodwaitdelay","10" };

DLL_GLOBAL cvar_t	ag_player_id = { "sv_aura_player_id","5" };           //Default 5 seconds.
DLL_GLOBAL cvar_t	ag_auto_admin = { "sv_aura_auto_admin","1" };          //Default 1 = Autoauthenticate admins based on won id.

DLL_GLOBAL cvar_t	ag_lj_timer = { "sv_aura_lj_timer","0" };            //Default 0 = turned off.

DLL_GLOBAL cvar_t	ag_wallgauss = { "sv_aura_wallgauss","1" };           //Default 1 = Lame wallgauss on.
DLL_GLOBAL cvar_t	ag_headshot = { "sv_aura_headshot","3" };            //Default 3 = 3 times damage
DLL_GLOBAL cvar_t	ag_blastradius = { "sv_aura_blastradius","1" };         //Default 1 = Standard radius


DLL_GLOBAL cvar_t	ag_ban_crowbar = { "sv_aura_ban_crowbar","0" };
DLL_GLOBAL cvar_t	ag_ban_glock = { "sv_aura_ban_glock","0" };
DLL_GLOBAL cvar_t   ag_ban_oitc = { "sv_aura_ban_oitc", "1" };
DLL_GLOBAL cvar_t	ag_ban_357 = { "sv_aura_ban_357","0" };
DLL_GLOBAL cvar_t	ag_ban_mp5 = { "sv_aura_ban_mp5","0" };
DLL_GLOBAL cvar_t	ag_ban_shotgun = { "sv_aura_ban_shotgun", "0" };
DLL_GLOBAL cvar_t	ag_ban_crossbow = { "sv_aura_ban_crossbow", "0" };
DLL_GLOBAL cvar_t	ag_ban_rpg = { "sv_aura_ban_rpg","0" };
DLL_GLOBAL cvar_t	ag_ban_gauss = { "sv_aura_ban_gauss","0" };
DLL_GLOBAL cvar_t	ag_ban_egon = { "sv_aura_ban_egon","0" };
DLL_GLOBAL cvar_t	ag_ban_hornet = { "sv_aura_ban_hornet","0" };

DLL_GLOBAL cvar_t   ag_ban_penguin = { "sv_aura_ban_penguin", "0" };
DLL_GLOBAL cvar_t   ag_ban_m249 = { "sv_aura_ban_m249", "0" };
DLL_GLOBAL cvar_t   ag_ban_eagle = { "sv_aura_ban_eagle", "0" };
DLL_GLOBAL cvar_t   ag_ban_displacer = { "sv_aura_ban_displacer", "0" };
DLL_GLOBAL cvar_t   ag_ban_grapple = { "sv_aura_ban_grapple", "0" };
DLL_GLOBAL cvar_t   ag_ban_knife = { "sv_aura_ban_knife" ,"0" };
DLL_GLOBAL cvar_t   ag_ban_pipewrench = { "sv_aura_ban_pipewrench", "0" };
DLL_GLOBAL cvar_t   ag_ban_shockrifle = { "sv_aura_ban_shockrifle", "0" };
DLL_GLOBAL cvar_t   ag_ban_sniperrifle = { "sv_aura_ban_sniper", "0" };
DLL_GLOBAL cvar_t   ag_ban_sporelauncher = { "sv_aura_ban_sporelauncher", "0" };

DLL_GLOBAL cvar_t	ag_ban_hgrenade = { "sv_aura_ban_hgrenade","0" };
DLL_GLOBAL cvar_t	ag_ban_satchel = { "sv_aura_ban_satchel","0" };
DLL_GLOBAL cvar_t	ag_ban_tripmine = { "sv_aura_ban_tripmine","0" };
DLL_GLOBAL cvar_t	ag_ban_snark = { "sv_aura_ban_snark","0" };
DLL_GLOBAL cvar_t	ag_ban_longjump = { "sv_aura_ban_longjump","0" };
DLL_GLOBAL cvar_t	ag_ban_m203 = { "sv_aura_ban_m203","0" };
DLL_GLOBAL cvar_t	ag_ban_9mmar = { "sv_aura_ban_9mmar","0" };
DLL_GLOBAL cvar_t	ag_ban_bockshot = { "sv_aura_ban_bockshot","0" };
DLL_GLOBAL cvar_t	ag_ban_uranium = { "sv_aura_ban_uranium","0" };
DLL_GLOBAL cvar_t	ag_ban_bolts = { "sv_aura_ban_bolts","0" };
DLL_GLOBAL cvar_t	ag_ban_rockets = { "sv_aura_ban_rockets","0" };
DLL_GLOBAL cvar_t	ag_ban_357ammo = { "sv_aura_ban_357ammo","0" };
DLL_GLOBAL cvar_t   ag_ban_ammo556 = { "sv_aura_ban_ammo_556","0" };
DLL_GLOBAL cvar_t   ag_ban_ammo762 = { "sv_aura_ban_ammo_762","0" };
DLL_GLOBAL cvar_t   ag_ban_ammoSpore = { "sv_aura_ban_ammo_spore","0" };

DLL_GLOBAL cvar_t	ag_ban_armour = { "sv_aura_ban_armour","0" };
DLL_GLOBAL cvar_t	ag_ban_health = { "sv_aura_ban_health","0" };
DLL_GLOBAL cvar_t	ag_ban_recharg = { "sv_aura_ban_recharg","0" };

DLL_GLOBAL cvar_t	ag_start_crowbar = { "sv_aura_start_crowbar","1" };
DLL_GLOBAL cvar_t	ag_start_glock = { "sv_aura_start_glock","1" };
DLL_GLOBAL cvar_t   ag_start_oitc = { "sv_aura_start_oitc", "0" };
DLL_GLOBAL cvar_t	ag_start_357 = { "sv_aura_start_357","0" };
DLL_GLOBAL cvar_t	ag_start_mp5 = { "sv_aura_start_mp5","0" };
DLL_GLOBAL cvar_t	ag_start_shotgun = { "sv_aura_start_shotgun", "0" };
DLL_GLOBAL cvar_t	ag_start_crossbow = { "sv_aura_start_crossbow", "0" };
DLL_GLOBAL cvar_t	ag_start_rpg = { "sv_aura_start_rpg","0" };
DLL_GLOBAL cvar_t	ag_start_gauss = { "sv_aura_start_gauss","0" };
DLL_GLOBAL cvar_t	ag_start_egon = { "sv_aura_start_egon","0" };
DLL_GLOBAL cvar_t	ag_start_hornet = { "sv_aura_start_hornet","0" };

DLL_GLOBAL cvar_t	ag_start_hgrenade = { "sv_aura_start_hgrenade","0" };
DLL_GLOBAL cvar_t	ag_start_satchel = { "sv_aura_start_satchel","0" };
DLL_GLOBAL cvar_t	ag_start_tripmine = { "sv_aura_start_tripmine","0" };
DLL_GLOBAL cvar_t	ag_start_snark = { "sv_aura_start_snark","0" };
DLL_GLOBAL cvar_t	ag_start_longjump = { "sv_aura_start_longjump","0" };
DLL_GLOBAL cvar_t	ag_start_m203 = { "sv_aura_start_m203","0" };
DLL_GLOBAL cvar_t	ag_start_9mmar = { "sv_aura_start_9mmar","68" };
DLL_GLOBAL cvar_t	ag_start_bockshot = { "sv_aura_start_bockshot","0" };
DLL_GLOBAL cvar_t	ag_start_uranium = { "sv_aura_start_uranium","0" };
DLL_GLOBAL cvar_t	ag_start_bolts = { "sv_aura_start_bolts","0" };
DLL_GLOBAL cvar_t	ag_start_rockets = { "sv_aura_start_rockets","0" };
DLL_GLOBAL cvar_t	ag_start_357ammo = { "sv_aura_start_357ammo","0" };
DLL_GLOBAL cvar_t   ag_start_penguin = { "sv_aura_start_penguin", "0" };
DLL_GLOBAL cvar_t   ag_start_m249 = { "sv_aura_start_m249", "0" };
DLL_GLOBAL cvar_t   ag_start_eagle = { "sv_aura_start_eagle", "0" };
DLL_GLOBAL cvar_t   ag_start_displacer = { "sv_aura_start_displacer", "0" };
DLL_GLOBAL cvar_t   ag_start_grapple = { "sv_aura_start_grapple", "0" };
DLL_GLOBAL cvar_t   ag_start_knife = { "sv_aura_start_knife" ,"0" };
DLL_GLOBAL cvar_t   ag_start_pipewrench = { "sv_aura_start_pipewrench", "0" };
DLL_GLOBAL cvar_t   ag_start_shockrifle = { "sv_aura_start_shockrifle", "0" };
DLL_GLOBAL cvar_t   ag_start_sniperrifle = { "sv_aura_start_sniper", "0" };
DLL_GLOBAL cvar_t   ag_start_sporelauncher = { "sv_aura_start_sporelauncher", "0" };
DLL_GLOBAL cvar_t	ag_start_armour = { "sv_aura_start_armour","0" };
DLL_GLOBAL cvar_t	ag_start_health = { "sv_aura_start_health","100" };

// Opposing Force ammo
DLL_GLOBAL cvar_t   ag_start_ammo556 = { "sv_aura_start_ammo_556", "0" }; // this should be 200 in gamemodes
DLL_GLOBAL cvar_t   ag_start_ammo762 = { "sv_aura_start_ammo_762", "0" }; // this should be 15 in gamemodes
DLL_GLOBAL cvar_t   ag_start_ammoSpore = { "sv_aura_start_ammo_spore", "0" }; // this should be 20 in gamemodes

// Half-Life Weapons
DLL_GLOBAL cvar_t	ag_dmg_crowbar = { "sv_aura_dmg_crowbar","25" };
DLL_GLOBAL cvar_t	ag_dmg_glock = { "sv_aura_dmg_glock","12" };
DLL_GLOBAL cvar_t	ag_dmg_357 = { "sv_aura_dmg_357","50" };
DLL_GLOBAL cvar_t	ag_dmg_mp5 = { "sv_aura_dmg_mp5","12" };
DLL_GLOBAL cvar_t	ag_dmg_shotgun = { "sv_aura_dmg_shotgun", "20" };
DLL_GLOBAL cvar_t	ag_dmg_crossbow = { "sv_aura_dmg_crossbow", "20" };
DLL_GLOBAL cvar_t	ag_dmg_bolts = { "sv_aura_dmg_bolts","50" };
DLL_GLOBAL cvar_t	ag_dmg_rpg = { "sv_aura_dmg_rpg","120" };
DLL_GLOBAL cvar_t	ag_dmg_gauss = { "sv_aura_dmg_gauss","20" };
DLL_GLOBAL cvar_t	ag_dmg_egon_wide = { "sv_aura_dmg_egon_wide","20" };
DLL_GLOBAL cvar_t	ag_dmg_egon_narrow = { "sv_aura_dmg_egon_narrow","10" };
DLL_GLOBAL cvar_t	ag_dmg_hornet = { "sv_aura_dmg_hornet","10" };
DLL_GLOBAL cvar_t	ag_dmg_hgrenade = { "sv_aura_dmg_hgrenade","100" };
DLL_GLOBAL cvar_t	ag_dmg_satchel = { "sv_aura_dmg_satchel","120" };
DLL_GLOBAL cvar_t	ag_dmg_tripmine = { "sv_aura_dmg_tripmine","150" };
DLL_GLOBAL cvar_t	ag_dmg_m203 = { "sv_aura_dmg_m203","100" };

// Opposing Force Weapons
DLL_GLOBAL cvar_t ag_dmg_pipewrench = { "sv_aura_dmg_pipewrench", "20" };
DLL_GLOBAL cvar_t ag_dmg_pipewrench2 = { "sv_aura_dmg_pipewrench2", "40" }; // default "BIG SWING" damage value
DLL_GLOBAL cvar_t ag_dmg_knife = { "sv_aura_dmg_knife", "10" };
DLL_GLOBAL cvar_t ag_dmg_grapple = { "sv_aura_dmg_grapple", "25" };
DLL_GLOBAL cvar_t ag_dmg_eagle = { "sv_aura_dmg_eagle", "34" };
DLL_GLOBAL cvar_t ag_dmg_762 = { "sv_aura_dmg_sniperrifle", "101" }; // 762 == Sniper Bullet Type
DLL_GLOBAL cvar_t ag_dmg_556 = { "sv_aura_dmg_m249", "20" }; // 556 == M249 SAW Bullet Type
DLL_GLOBAL cvar_t ag_dmg_displacer_self = { "sv_aura_dmg_displacer_self", "5" }; // Self Displace
DLL_GLOBAL cvar_t ag_dmg_displacer_other = { "sv_aura_dmg_displacer_other", "250" }; // Displacer Primary Attack's Shockwave
DLL_GLOBAL cvar_t ag_displacer_radius = { "sv_aura_displacer_radius", "300" }; // Radius of shockwave
DLL_GLOBAL cvar_t ag_dmg_shockrifle_m = { "sv_aura_dmg_shockrifle_m", "15" };
DLL_GLOBAL cvar_t ag_dmg_shockrifle_s = { "sv_aura_dmg_shockrifle_s", "15" };
DLL_GLOBAL cvar_t ag_dmg_spore = { "sv_aura_dmg_spore", "50" };


DLL_GLOBAL cvar_t	ag_max_spectators = { "sv_aura_max_spectators","5" };
DLL_GLOBAL cvar_t	ag_spec_enable_disable = { "sv_aura_spec_enable_disable","0" };
DLL_GLOBAL cvar_t	ag_spectalk = { "ag_spectalk","1" };

DLL_GLOBAL cvar_t	ag_spawn_volume = { "sv_aura_spawn_volume","1" };
DLL_GLOBAL cvar_t	ag_show_gibs = { "sv_aura_show_gibs","1" };

DLL_GLOBAL cvar_t	ag_gametype = { "sv_aura_gametype","" };

DLL_GLOBAL cvar_t	ag_ctf_flag_resettime = { "sv_aura_ctf_flag_resettime","30" };  //the time that a dropped flag lays in the world before respawning
DLL_GLOBAL cvar_t	ag_ctf_capture_limit = { "sv_aura_ctf_capturelimit","10", FCVAR_SERVER };  //the number of captures before map ends.
DLL_GLOBAL cvar_t	ag_ctf_teamcapturepoints = { "sv_aura_ctf_teamcapturepoints","1" };  //the ammount of points his teammates get
DLL_GLOBAL cvar_t	ag_ctf_capturepoints = { "sv_aura_ctf_capturepoints","4" };  //the amount of points the capturer gets
DLL_GLOBAL cvar_t	ag_ctf_returnpoints = { "sv_aura_ctf_returnpoints","1" };  //the amount of points the returner gets.
DLL_GLOBAL cvar_t	ag_ctf_carrierkillpoints = { "sv_aura_ctf_carrierkillpoints","1" };  //the amount of points the killer gets.
DLL_GLOBAL cvar_t	ag_ctf_stealpoints = { "sv_aura_ctf_stealpoints","1" };  //the amount of points the stealer gets.
DLL_GLOBAL cvar_t	ag_ctf_defendpoints = { "sv_aura_ctf_defendpoints","1" };  //the amount of points the defender gets.
DLL_GLOBAL cvar_t	ag_ctf_roundbased = { "sv_aura_ctf_roundbased","0" };  //1 for roundbased CTF game.

//++ muphicks
DLL_GLOBAL cvar_t	ag_dom_mincontroltime = { "sv_aura_dom_mincontroltime","5" }; // number of seconds team must control point to score
DLL_GLOBAL cvar_t	ag_dom_controlpoints = { "sv_aura_dom_controlpoints", "1" }; // number of points scored when under teams control
DLL_GLOBAL cvar_t	ag_dom_resetscorelimit = { "sv_aura_dom_resetscorelimit", "6" }; // max time under 1 teams control 5*6 = 30 seconds
DLL_GLOBAL cvar_t	ag_dom_scorelimit = { "sv_aura_dom_scorelimit", "200" }; // max points a team needs to get to win the game
//-- muphicks

DLL_GLOBAL cvar_t	ag_gauss_fix = { "ag_gauss_fix","0" };            //Default 0 - no fix.
DLL_GLOBAL cvar_t	ag_rpg_fix = { "ag_rpg_fix","0" };            //Default 0 - no fix.

DLL_GLOBAL cvar_t	ag_spawn_system = { "ag_spawn_system", "0", FCVAR_SERVER };  // Default 0 - classic mode (select a random spawn from the next 5)
DLL_GLOBAL cvar_t	ag_spawn_history_entries = { "ag_spawn_history_entries", "25", FCVAR_SERVER };  // Default 25 - remember all of these spawnpoints that were last used

// This is to avoid repeating the last used spawnpoints, 0.25 means avoid the 25% of total spawns that were last used,
// so in crossfire there rare 17 spawns, the 25% of that is 4.25, it's rounded to 4, so it will avoid the last 4 spots where you spawned in crossfire
// It can be capped by `ag_spawn_history_entries`, so if its value is 3 (less than 4), it would avoid the last 3 instead
DLL_GLOBAL cvar_t	ag_spawn_avoid_last_spots = { "ag_spawn_avoid_last_spots", "0.25", FCVAR_SERVER };

// A fraction of the total spawnpoints, that will be used for the Far spawn system to choose a spawnpoint randomly
DLL_GLOBAL cvar_t	ag_spawn_far_spots = { "ag_spawn_far_spots", "0.25", FCVAR_SERVER };

// Probabilities for the Position-aware Spawn System to pick these spot categories for the next spawn
DLL_GLOBAL cvar_t	ag_spawn_pa_visible_chance = { "ag_spawn_pa_visible_chance", "10", FCVAR_SERVER }; // Default 10 - means 10%
DLL_GLOBAL cvar_t	ag_spawn_pa_audible_chance = { "ag_spawn_pa_audible_chance", "25", FCVAR_SERVER }; // Default 25 - means 25%
DLL_GLOBAL cvar_t	ag_spawn_pa_safe_chance    = { "ag_spawn_pa_safe_chance",    "65", FCVAR_SERVER }; // Default 65 - means 65%

// Some of these should be prefixed sv_aura_* instead of just ag_*, because it seems the convention
// was to have the non-votable ones with sv_aura_* and the votable ones with ag_*. But it might be
// quite confusing for actual usage, as you have to remember which ones are votable and which not.
// So I've decided to break the convention here to make it easier for players and admins to use these,
// as you can just type ag_fps_limit and tab or press down arrow to see all the different cvars, instead
// of having to do this with both sv_aura_fps_limit and ag_fps_limit. We'll see if it's the right decision
DLL_GLOBAL cvar_t	ag_fps_limit = { "ag_fps_limit", "0", FCVAR_SERVER };  // Default: 0 - Cap players' fps_max. Standard in 2021 is 144 (125 and 100 before; 250 for bhop)
DLL_GLOBAL cvar_t	ag_fps_limit_auto = { "ag_fps_limit_auto", "0", FCVAR_SERVER };  // Default: 0 - Whether to limit the fps to the most common fps among players
DLL_GLOBAL cvar_t	ag_fps_limit_auto_check_interval = { "ag_fps_limit_auto_check_interval", "10.0", FCVAR_SERVER };  // Default: 10 seconds - How often to check for changing the limit
DLL_GLOBAL cvar_t	ag_fps_limit_match_only = { "ag_fps_limit_match_only", "0", FCVAR_SERVER };  // Default: 0 - Whether to limit it only for players in a match
DLL_GLOBAL cvar_t	ag_fps_limit_steampipe = { "ag_fps_limit_steampipe", "1", FCVAR_SERVER };  // Default: 1 - Whether to account for the 0.5 fps added by the engine (steampipe)
DLL_GLOBAL cvar_t	ag_fps_limit_warnings = { "ag_fps_limit_warnings", "3", FCVAR_SERVER };  // Default: 3 - How many warnings before applying the punishment
DLL_GLOBAL cvar_t	ag_fps_limit_warnings_interval = { "ag_fps_limit_warnings_interval", "5.0", FCVAR_SERVER };  // Default: 5 seconds - Time between warnings
DLL_GLOBAL cvar_t	ag_fps_limit_punishment = { "ag_fps_limit_punishment", "kick", FCVAR_SERVER };  // Default: kick - Options: slap, kick, ban (3 mins)
DLL_GLOBAL cvar_t	ag_fps_limit_punishment_slap_intensity = { "ag_fps_limit_punishment_slap_intensity", "1.0", FCVAR_SERVER };  // Default: 1.0 - Multiplier for slap damage & punch
DLL_GLOBAL cvar_t	ag_fps_limit_punishment_slap_interval = { "ag_fps_limit_punishment_slap_interval", "1.0", FCVAR_SERVER };  // Default: 1 second - Time between slaps
DLL_GLOBAL cvar_t	ag_fps_limit_punishment_ban_time = { "ag_fps_limit_punishment_ban_time", "3", FCVAR_SERVER };  // Default: 3 minutes - How much time to ban them for

DLL_GLOBAL cvar_t	ag_satchel_destroyable = { "sv_aura_satchel_destroyable", "0", FCVAR_SERVER }; // Default: 0 - Cannot destroy satchels (only with the detonator)
DLL_GLOBAL cvar_t	ag_satchel_health      = { "sv_aura_satchel_health",     "15", FCVAR_SERVER }; // Default: 15 hp
DLL_GLOBAL cvar_t	ag_satchel_solid       = { "sv_aura_satchel_solid",       "1", FCVAR_SERVER }; // Default: 1 - Solid, players can collide with them

DLL_GLOBAL cvar_t	mm_agsay = { "mm_agsay","1", FCVAR_SERVER };


DLL_GLOBAL bool g_bLangame = false;
DLL_GLOBAL bool g_bUseTeamColors = false;
extern AgString g_sGamemode;

// Keep this sorted
std::vector<std::string> g_votableSettings = {
    "ag_fps_limit",
    "ag_fps_limit_auto",
    "ag_fps_limit_auto_check_interval",
    "ag_gauss_fix",
    "ag_rpg_fix",
    "ag_spawn_avoid_last_spots",
    "ag_spawn_far_spots",
    "ag_spawn_history_entries",
    "ag_spawn_pa_audible_chance",
    "ag_spawn_pa_safe_chance",
    "ag_spawn_pa_visible_chance",
    "ag_spawn_system",
    "mp_fraglimit",
    "mp_friendlyfire",
    "mp_timelimit",
    "mp_weaponstay",
};

void LoadGreetingMessages();

void AgInitGame()
{
    AgInitTimer();

    CVAR_REGISTER(&ag_version);
    CVAR_REGISTER(&ag_gamemode);
    CVAR_REGISTER(&ag_gamemode_auto);
    CVAR_REGISTER(&ag_allowed_gamemodes);

    CVAR_REGISTER(&ag_allow_vote);
    CVAR_REGISTER(&ag_pure);
    CVAR_REGISTER(&ag_match_running);

    CVAR_REGISTER(&ag_oldphysics);

    CVAR_REGISTER(&ag_player_id);
    CVAR_REGISTER(&ag_lj_timer);

    CVAR_REGISTER(&ag_auto_admin);
    CVAR_REGISTER(&ag_wallgauss);
    CVAR_REGISTER(&ag_headshot);
    CVAR_REGISTER(&ag_blastradius);

    CVAR_REGISTER(&ag_ban_crowbar);
    CVAR_REGISTER(&ag_ban_glock);
    CVAR_REGISTER(&ag_ban_oitc);
    CVAR_REGISTER(&ag_ban_357);
    CVAR_REGISTER(&ag_ban_mp5);
    CVAR_REGISTER(&ag_ban_shotgun);
    CVAR_REGISTER(&ag_ban_crossbow);
    CVAR_REGISTER(&ag_ban_rpg);
    CVAR_REGISTER(&ag_ban_gauss);
    CVAR_REGISTER(&ag_ban_egon);
    CVAR_REGISTER(&ag_ban_hornet);
    CVAR_REGISTER(&ag_ban_m249);
    CVAR_REGISTER(&ag_ban_penguin);
    CVAR_REGISTER(&ag_ban_eagle);
    CVAR_REGISTER(&ag_ban_displacer);
    CVAR_REGISTER(&ag_ban_grapple);
    CVAR_REGISTER(&ag_ban_knife);
    CVAR_REGISTER(&ag_ban_pipewrench);
    CVAR_REGISTER(&ag_ban_shockrifle);
    CVAR_REGISTER(&ag_ban_sniperrifle);
    CVAR_REGISTER(&ag_ban_sporelauncher);

    CVAR_REGISTER(&ag_ban_hgrenade);
    CVAR_REGISTER(&ag_ban_satchel);
    CVAR_REGISTER(&ag_ban_tripmine);
    CVAR_REGISTER(&ag_ban_snark);
    CVAR_REGISTER(&ag_ban_m203);
    CVAR_REGISTER(&ag_ban_longjump);
    CVAR_REGISTER(&ag_ban_9mmar);
    CVAR_REGISTER(&ag_ban_bockshot);
    CVAR_REGISTER(&ag_ban_uranium);
    CVAR_REGISTER(&ag_ban_bolts);
    CVAR_REGISTER(&ag_ban_rockets);
    CVAR_REGISTER(&ag_ban_357ammo);
    CVAR_REGISTER(&ag_ban_ammo556);
    CVAR_REGISTER(&ag_ban_ammo762);
    CVAR_REGISTER(&ag_ban_ammoSpore);

    CVAR_REGISTER(&ag_ban_armour);
    CVAR_REGISTER(&ag_ban_health);
    CVAR_REGISTER(&ag_ban_recharg);

    CVAR_REGISTER(&ag_start_crowbar);
    CVAR_REGISTER(&ag_start_glock);
    CVAR_REGISTER(&ag_start_oitc);
    CVAR_REGISTER(&ag_start_357);
    CVAR_REGISTER(&ag_start_mp5);
    CVAR_REGISTER(&ag_start_shotgun);
    CVAR_REGISTER(&ag_start_crossbow);
    CVAR_REGISTER(&ag_start_rpg);
    CVAR_REGISTER(&ag_start_gauss);
    CVAR_REGISTER(&ag_start_egon);
    CVAR_REGISTER(&ag_start_hornet);
    CVAR_REGISTER(&ag_start_penguin);
    CVAR_REGISTER(&ag_start_m249);
    CVAR_REGISTER(&ag_start_eagle);
    CVAR_REGISTER(&ag_start_displacer);
    CVAR_REGISTER(&ag_start_grapple);
    CVAR_REGISTER(&ag_start_knife);
    CVAR_REGISTER(&ag_start_pipewrench);
    CVAR_REGISTER(&ag_start_shockrifle);
    CVAR_REGISTER(&ag_start_sniperrifle);
    CVAR_REGISTER(&ag_start_sporelauncher);

    CVAR_REGISTER(&ag_start_hgrenade);
    CVAR_REGISTER(&ag_start_satchel);
    CVAR_REGISTER(&ag_start_tripmine);
    CVAR_REGISTER(&ag_start_snark);
    CVAR_REGISTER(&ag_start_m203);
    CVAR_REGISTER(&ag_start_longjump);
    CVAR_REGISTER(&ag_start_9mmar);
    CVAR_REGISTER(&ag_start_bockshot);
    CVAR_REGISTER(&ag_start_uranium);
    CVAR_REGISTER(&ag_start_bolts);
    CVAR_REGISTER(&ag_start_rockets);
    CVAR_REGISTER(&ag_start_357ammo);

    // Opposing Force ammo
    CVAR_REGISTER(&ag_start_ammo556);
    CVAR_REGISTER(&ag_start_ammo762);
    CVAR_REGISTER(&ag_start_ammoSpore);

    CVAR_REGISTER(&ag_start_armour);
    CVAR_REGISTER(&ag_start_health);

    // Half-Life Weapons
    CVAR_REGISTER(&ag_dmg_crowbar);
    CVAR_REGISTER(&ag_dmg_glock);
    CVAR_REGISTER(&ag_dmg_357);
    CVAR_REGISTER(&ag_dmg_mp5);
    CVAR_REGISTER(&ag_dmg_shotgun);
    CVAR_REGISTER(&ag_dmg_crossbow);
    CVAR_REGISTER(&ag_dmg_bolts);
    CVAR_REGISTER(&ag_dmg_rpg);
    CVAR_REGISTER(&ag_dmg_gauss);
    CVAR_REGISTER(&ag_dmg_egon_wide);
    CVAR_REGISTER(&ag_dmg_egon_narrow);
    CVAR_REGISTER(&ag_dmg_hornet);
    CVAR_REGISTER(&ag_dmg_hgrenade);
    CVAR_REGISTER(&ag_dmg_satchel);
    CVAR_REGISTER(&ag_dmg_tripmine);
    CVAR_REGISTER(&ag_dmg_m203);

    // Opposing Force Weapons
    CVAR_REGISTER(&ag_dmg_pipewrench);
    CVAR_REGISTER(&ag_dmg_knife);
    CVAR_REGISTER(&ag_dmg_grapple);
    CVAR_REGISTER(&ag_dmg_eagle);
    CVAR_REGISTER(&ag_dmg_762);
    CVAR_REGISTER(&ag_dmg_556);
    CVAR_REGISTER(&ag_dmg_displacer_self);
    CVAR_REGISTER(&ag_dmg_displacer_other);
    CVAR_REGISTER(&ag_displacer_radius);
    CVAR_REGISTER(&ag_dmg_shockrifle_m);
    CVAR_REGISTER(&ag_dmg_shockrifle_s);
    CVAR_REGISTER(&ag_dmg_spore);

    CVAR_REGISTER(&ag_allow_timeout);

    CVAR_REGISTER(&ag_vote_start);
    CVAR_REGISTER(&ag_vote_setting);
    CVAR_REGISTER(&ag_vote_gamemode);
    CVAR_REGISTER(&ag_vote_kick);
    CVAR_REGISTER(&ag_vote_allow);
    CVAR_REGISTER(&ag_vote_admin);
    CVAR_REGISTER(&ag_vote_map);
    CVAR_REGISTER(&ag_vote_failed_time);

    CVAR_REGISTER(&ag_start_minplayers);

    CVAR_REGISTER(&ag_vote_mp_timelimit_low);
    CVAR_REGISTER(&ag_vote_mp_timelimit_high);
    CVAR_REGISTER(&ag_vote_mp_fraglimit_low);
    CVAR_REGISTER(&ag_vote_mp_fraglimit_high);
    CVAR_REGISTER(&ag_vote_extra_timelimit);

    CVAR_REGISTER(&ag_floodmsgs);
    CVAR_REGISTER(&ag_floodpersecond);
    CVAR_REGISTER(&ag_floodwaitdelay);

    CVAR_REGISTER(&ag_max_spectators);
    CVAR_REGISTER(&ag_spec_enable_disable);
    CVAR_REGISTER(&ag_spectalk);

    CVAR_REGISTER(&ag_spawn_volume);
    CVAR_REGISTER(&ag_show_gibs);

    CVAR_REGISTER(&ag_gametype);

    CVAR_REGISTER(&ag_ctf_flag_resettime);
    CVAR_REGISTER(&ag_ctf_capturepoints);
    CVAR_REGISTER(&ag_ctf_teamcapturepoints);
    CVAR_REGISTER(&ag_ctf_capture_limit);
    CVAR_REGISTER(&ag_ctf_returnpoints);
    CVAR_REGISTER(&ag_ctf_carrierkillpoints);
    CVAR_REGISTER(&ag_ctf_stealpoints);
    CVAR_REGISTER(&ag_ctf_defendpoints);
    CVAR_REGISTER(&ag_ctf_roundbased);

    //++ muphicks
    CVAR_REGISTER(&ag_dom_mincontroltime);
    CVAR_REGISTER(&ag_dom_controlpoints);
    CVAR_REGISTER(&ag_dom_resetscorelimit);
    CVAR_REGISTER(&ag_dom_scorelimit);
    //-- muphicks

    CVAR_REGISTER(&ag_gauss_fix);
    CVAR_REGISTER(&ag_rpg_fix);

    CVAR_REGISTER(&ag_spawn_system);
    CVAR_REGISTER(&ag_spawn_history_entries);
    CVAR_REGISTER(&ag_spawn_avoid_last_spots);
    CVAR_REGISTER(&ag_spawn_far_spots);
    CVAR_REGISTER(&ag_spawn_pa_visible_chance);
    CVAR_REGISTER(&ag_spawn_pa_audible_chance);
    CVAR_REGISTER(&ag_spawn_pa_safe_chance);

    CVAR_REGISTER(&ag_fps_limit);
    CVAR_REGISTER(&ag_fps_limit_auto);
    CVAR_REGISTER(&ag_fps_limit_auto_check_interval);
    CVAR_REGISTER(&ag_fps_limit_match_only);
    CVAR_REGISTER(&ag_fps_limit_steampipe);
    CVAR_REGISTER(&ag_fps_limit_warnings);
    CVAR_REGISTER(&ag_fps_limit_warnings_interval);
    CVAR_REGISTER(&ag_fps_limit_punishment);
    CVAR_REGISTER(&ag_fps_limit_punishment_slap_intensity);
    CVAR_REGISTER(&ag_fps_limit_punishment_slap_interval);
    CVAR_REGISTER(&ag_fps_limit_punishment_ban_time);

    CVAR_REGISTER(&ag_satchel_destroyable);
    CVAR_REGISTER(&ag_satchel_health);
    CVAR_REGISTER(&ag_satchel_solid);

    CVAR_REGISTER(&mm_agsay);

    Command.Init();

    //Set up initial settings. Add "startup_" before
    char* servercfgfile = (char*)CVAR_GET_STRING("servercfgfile");

    if (servercfgfile && servercfgfile[0])
    {
        char szCommand[256];

        ALERT(at_console, "Executing dedicated server startup config file\n");
        sprintf(szCommand, "exec startup_%s\n", servercfgfile);
        SERVER_COMMAND(szCommand);
        SERVER_EXECUTE();
    }

    g_bLangame = 0 < CVAR_GET_FLOAT("sv_lan");

    GameMode.Init();
    Wallhack.Init();
    LoadGreetingMessages();
}


//TITLES FOR HALF-LIFE
// Position command $position x y 
// x & y are from 0 to 1 to be screen resolution independent
// -1 means center in each dimension
// Effect command $effect <effect number>
// effect 0 is fade in/fade out
// effect 1 is flickery credits
// effect 2 is write out (training room)
// Text color r g b command $color
// fadein time fadeout time / hold time
// $fadein (message fade in time - per character in effect 2)
// $fadeout (message fade out time)
// $holdtime (stay on the screen for this long)


void AgSay(CBasePlayer* pPlayer, const AgString& sText, float* pfFloodProtected, float fHoldTime, float x, float y, int iChannel)
{
    if (g_fGameOver)
        return;

    if (pfFloodProtected)
    {
        if (*pfFloodProtected > gpGlobals->time)
            return;

        *pfFloodProtected = gpGlobals->time + fHoldTime;
    }


    hudtextparms_t     hText;
    memset(&hText, 0, sizeof(hText));
    hText.channel = iChannel;
    // These X and Y coordinates are just above
    //  the health meter.
    hText.x = x;
    hText.y = y;

    hText.r1 = hText.g1 = hText.b1 = 180;
    hText.a1 = 0;

    hText.r2 = hText.g2 = hText.b2 = 0;
    hText.a2 = 0;

    hText.holdTime = fHoldTime - 0.30;

    hText.effect = 2;    // Fade in/out
    hText.fadeinTime = 0.01;
    hText.fadeoutTime = fHoldTime / 5;
    hText.fxTime = 0.25;

    if (pPlayer)
    {
        UTIL_HudMessage(pPlayer, hText, sText.c_str());
    }
    else
    {
        for (int i = 1; i <= gpGlobals->maxClients; i++)
        {
            CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
            if (pPlayerLoop)
                UTIL_HudMessage(pPlayerLoop, hText, sText.c_str());
        }
    }
}


CBasePlayer* AgPlayerByIndex(int iPlayerIndex)
{
    CBasePlayer* pPlayer = NULL;

    if (iPlayerIndex > 0 && iPlayerIndex <= gpGlobals->maxClients)
    {
        edict_t* pPlayerEdict = INDEXENT(iPlayerIndex);
        if (pPlayerEdict && !pPlayerEdict->free && pPlayerEdict->pvPrivateData)
        {
            CBaseEntity* pEnt = (CBaseEntity*)CBaseEntity::Instance(pPlayerEdict);
            if (pEnt && pEnt->pev && CLASS_PLAYER == pEnt->Classify())
            {
                if (pEnt->pev->netname && 0 != STRING(pEnt->pev->netname)[0])
                {
                    pPlayer = (CBasePlayer*)pEnt;
                }
            }
        }
    }

    return pPlayer;
}


CBasePlayer* AgPlayerByName(const AgString& sNameOrPlayerNumber)
{
    if (0 == sNameOrPlayerNumber.size())
        return NULL;

    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
        if (pPlayerLoop)
            if (0 == stricmp(pPlayerLoop->GetName(), sNameOrPlayerNumber.c_str()) ||
                "#" == sNameOrPlayerNumber.substr(0, 1) &&
                GETPLAYERUSERID(pPlayerLoop->edict()) == atoi(sNameOrPlayerNumber.substr(1).c_str())
                )
                return pPlayerLoop;
    }
    return NULL;
}

void AgChangelevel(const AgString& sLevelname)
{
    if (32 < sLevelname.size() || 0 == sLevelname.size())
        return;

    char szTemp[64];
    strcpy(szTemp, sLevelname.c_str());

    //Check if it exists.
    if (IS_MAP_VALID(szTemp))
        //Change the level
        CHANGE_LEVEL(szTemp, NULL);
}

void AgConsole(const AgString& sText, CBasePlayer* pPlayer)
{
    if (pPlayer && pPlayer->pev)
    {
        ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, UTIL_VarArgs("%s\n", sText.c_str()));
    }
    else
    {
        g_engfuncs.pfnServerPrint(UTIL_VarArgs("%s\n", sText.c_str()));
    }
}

void AgResetMap()
{
    CBaseEntity* pEntity = NULL;

    edict_t* pEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    for (int i = 1; i < gpGlobals->maxEntities; i++, pEdict++)
    {
        pEntity = CBaseEntity::Instance(pEdict);
        if (pEntity && pEntity->pev)
        {
            const char* pszClass = STRING(pEntity->pev->classname);

            if (pszClass && '\0' != pszClass[0])
            {
                if (0 == strncmp(pszClass, "weapon_", 7) ||
                    0 == strncmp(pszClass, "ammo_", 5) ||
                    0 == strncmp(pszClass, "item_", 5))
                {
                    pEntity->pev->nextthink = gpGlobals->time;
                }
            }
        }
    }

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "weaponbox")) != NULL)
        UTIL_Remove(pEntity);

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_satchel")) != NULL)
        UTIL_Remove(pEntity);

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_tripmine")) != NULL)
        UTIL_Remove(pEntity);

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_snark")) != NULL)
        UTIL_Remove(pEntity);

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "beam")) != NULL)
        UTIL_Remove(pEntity);

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "func_healthcharger")) != NULL)
        ((CBaseToggle*)pEntity)->Reset();

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "func_recharge")) != NULL)
        ((CBaseToggle*)pEntity)->Reset();

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "rpg_rocket")) != NULL)
        pEntity->pev->dmg = 0;

    pEntity = NULL;
    while ((pEntity = UTIL_FindEntityByClassname(pEntity, "grenade")) != NULL)
        pEntity->pev->dmg = 0;
}

char* AgStringToLower(char* pszString)
{
    if (NULL == pszString)
        return pszString;

    char* pszTemp = pszString;
    while ('\0' != pszTemp[0])
    {
        *pszTemp = tolower(*pszTemp);
        pszTemp++;
    }
    return pszString;
}

void AgToLower(AgString& strLower)
{
    size_t i = 0;
    while (i < strLower.size())
    {
        strLower[i] = tolower(strLower[i]);
        i++;
    }
}

void AgTrim(AgString& sTrim)
{
    if (0 == sTrim.size())
        return;

    int b = sTrim.find_first_not_of(" \t\r\n");
    int e = sTrim.find_last_not_of(" \t\r\n");
    if (b == -1) // No non-whitespaces
        sTrim = "";
    else
        sTrim = string(sTrim, b, e - b + 1);
}

void AgLog(const char* pszLog)
{
    char	szFile[MAX_PATH];
    sprintf(szFile, "%s/agslog.txt", AgGetDirectory());
    FILE* pFile = fopen(szFile, "a+");
    if (!pFile)
    {
        g_engfuncs.pfnServerPrint(UTIL_VarArgs("Couldn't create/save %s.", szFile));
        return;
    }

    time_t clock;
    time(&clock);
    fprintf(pFile, "%s : %s", pszLog, asctime(localtime(&clock)));
    fflush(pFile);
    fclose(pFile);
    g_engfuncs.pfnServerPrint(pszLog);
}

#ifndef WIN32
#include <sys/times.h>        
#endif

/*
float AgTime()
{
#ifdef WIN32
  return ((float)clock()) / ((float)CLOCKS_PER_SEC);
#else
  static tms t;
  return ((float)times(&t)) / 100.0; //Should be CLK_TCK defined in limits.h.
#endif
}
*/

void AgDirList(const AgString& sDir, AgStringSet& setFiles)
{
#ifdef _WIN32		
    WIN32_FIND_DATA FindData;
    char szSearchDirectory[_MAX_PATH];
    sprintf(szSearchDirectory, "%s/*.*", sDir.c_str());
    HANDLE hFind = FindFirstFile(szSearchDirectory, &FindData);

    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                AgString sFile;
                sFile = FindData.cFileName;
                setFiles.insert(sFile);
            }
        } while (FindNextFile(hFind, &FindData));
        FindClose(hFind);
    }
#else
    DIR* pDirectory = opendir(sDir.c_str());
    if (pDirectory)
    {
        struct dirent* pFile = NULL;

        while (NULL != (pFile = readdir(pDirectory)))
        {
            AgString sFile;
            sFile = pFile->d_name;
            setFiles.insert(sFile);
        }
        closedir(pDirectory);
    }
#endif
}

void AgStripColors(char* pszString)
{
    char* pszIt = pszString;
    while ('\0' != *pszIt)
    {
        if ('^' == *pszIt)
        {
            ++pszIt;
            if (*pszIt >= '0' && *pszIt <= '9')
            {
                --pszIt;
                memmove(pszIt, pszIt + 2, strlen(pszIt + 2) + 1);
            }
        }
        else
            ++pszIt;
    }
}

void AgGetDetails(char* pszDetails, int iMaxSize, int* piSize)
{
    int iBytes = 0;
    char* pszBuffer = pszDetails;

    //Write Match running
    iBytes += sprintf(&pszBuffer[iBytes], "match\\%d", ag_match_running.value > 0);
    pszBuffer[iBytes] = '\0';

    *piSize = iBytes;
}

void AgGetPlayerInfo(char* pszDetails, int iMaxSize, int* piSize)
{
    int iBytes = 0;
    char* pszBuffer = pszDetails;

    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        if (iBytes + 100 > iMaxSize)
            break;
        CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
        if (pPlayerLoop)
        {
            if (!pPlayerLoop->IsSpectator())
            {
                iBytes += sprintf(&pszBuffer[iBytes], "%s ", pPlayerLoop->GetName());
                pszBuffer[iBytes] = '\0';

                iBytes += sprintf(&pszBuffer[iBytes], "\t%s ", pPlayerLoop->TeamID());
                pszBuffer[iBytes] = '\0';

                iBytes += sprintf(&pszBuffer[iBytes], "\t%d ", (int)pPlayerLoop->pev->frags);
                pszBuffer[iBytes] = '\0';

                iBytes += sprintf(&pszBuffer[iBytes], "\t%d ", (int)pPlayerLoop->m_iDeaths);
                pszBuffer[iBytes] = '\0';

                iBytes += sprintf(&pszBuffer[iBytes], "\t%s ", pPlayerLoop->GetAuthID());
                pszBuffer[iBytes] = '\0';

                iBytes++;
            }
        }
    }

    *piSize = iBytes;
}

#ifndef _WIN32
#include <sys/utsname.h>
#endif

#ifdef _WIN32
static LARGE_INTEGER liTimerFreq;
void AgInitTimer()
{
    QueryPerformanceFrequency(&liTimerFreq);
}
double AgTime()
{
    LARGE_INTEGER liTime;
    QueryPerformanceCounter(&liTime);
    return ((double)liTime.QuadPart) / ((double)liTimerFreq.QuadPart);
}
#else
#include <sys/time.h> 
#include <unistd.h> 
#include <sys/times.h>        
static double dClockTicsPerSecond;
void AgInitTimer()
{
    dClockTicsPerSecond = sysconf(_SC_CLK_TCK);
}
double AgTime()
{
    tms t;
    clock_t time = times(&t);
    if (((clock_t)-1) == time)
    {
        struct timeval tod;
        gettimeofday(&tod, NULL);
        return tod.tv_sec + tod.tv_usec * 0.000001;
    }
    return ((double)time) / dClockTicsPerSecond;
}
#endif

AgString AgReadFile(const char* pszFile)
{
    AgString sData;

    char szFile[MAX_PATH];
    char szData[4096];
    sprintf(szFile, "%s/%s", AgGetDirectory(), pszFile);
    FILE* pFile = fopen(szFile, "r");
    if (!pFile)
        return "";

    int iRead = 0;
    while (0 != (iRead = fread(szData, sizeof(char), sizeof(szData) - 2, pFile)))
    {
        szData[iRead] = '\0';
        sData += szData;
        szData[0] = '\0';
    }
    fclose(pFile);

    return sData;
}

typedef map<AgString, AgString, less<AgString> > AgAuthIDToGreeting;
static AgAuthIDToGreeting s_mapGreeting;
void LoadGreetingMessages()
{
    AgString sGreetingMessages = AgReadFile("greeting.txt");

    int iPosNewLine = sGreetingMessages.find_first_of("\n");
    while (-1 != iPosNewLine)
    {
        AgString sAuthID, sGreeting;
        int iPosGreeting = sGreetingMessages.find_first_of(" \t");
        sAuthID = sGreetingMessages.substr(0, iPosGreeting);
        sGreeting = sGreetingMessages.substr(iPosGreeting + 1, iPosNewLine - iPosGreeting - 1);
        AgTrim(sGreeting);
        AgTrim(sAuthID);
        sGreeting += "\n";
        s_mapGreeting.insert(AgAuthIDToGreeting::value_type(sAuthID, sGreeting));

        sGreetingMessages = sGreetingMessages.substr(iPosNewLine + 1);
        iPosNewLine = sGreetingMessages.find_first_of("\n");
    }
}

void AgDisplayGreetingMessage(const char* pszAuthID)
{
    AgAuthIDToGreeting::iterator itrGreeting = s_mapGreeting.find(pszAuthID);
    if (itrGreeting != s_mapGreeting.end())
        UTIL_ClientPrintAll(HUD_PRINTTALK, (*itrGreeting).second.c_str());
}



bool AgIsCTFMap(const char* pszMap)
{
#define	LUMP_ENTITIES	0
#define	HEADER_LUMPS	15
#define	MAX_MAP_ENTSTRING	(128*1024)

    typedef struct
    {
        int		fileofs, filelen;
    } lump_t;

    typedef struct
    {
        int			  version;
        lump_t		lumps[HEADER_LUMPS];
    } dheader_t;

    if (0 == strncmp(pszMap, "agctf_", 6))
        return true;
    else if (0 == strncmp(pszMap, "hlectf_", 6))
        return true;

    char szMapFile[MAX_PATH];
    int iMapLength = 0;
    sprintf(szMapFile, "maps/%s.bsp", STRING(gpGlobals->mapname));
    legacy_byte* pMapData = LOAD_FILE_FOR_ME(szMapFile, &iMapLength);
    if (!pMapData)
        return false;

    AgString sMapEntityData;
    dheader_t* pHeader = (dheader_t*)pMapData;
    if (pHeader->version == 29 || pHeader->version == 30)
    {
        int iMapDataLength = pHeader->lumps[LUMP_ENTITIES].filelen;
        int iMapDataOffset = pHeader->lumps[LUMP_ENTITIES].fileofs;
        pMapData[iMapDataLength] = '\0';
        if (NULL != strstr((const char*)&pMapData[iMapDataOffset], "info_hmctfdetect")
            || NULL != strstr((const char*)&pMapData[iMapDataOffset], "info_ctfdetect"))
        {
            FREE_FILE(pMapData);
            return true;
        }
    }
    FREE_FILE(pMapData);
    return false;
}


//++ muphicks
// Check to see if we have a DOM map
bool AgIsDOMMap(const char* pszMap)
{
#define	LUMP_ENTITIES	0
#define	HEADER_LUMPS	15
#define	MAX_MAP_ENTSTRING	(128*1024)

    typedef struct
    {
        int		fileofs, filelen;
    } lump_t;

    typedef struct
    {
        int			  version;
        lump_t		lumps[HEADER_LUMPS];
    } dheader_t;

    if (0 == strncmp(pszMap, "agdom_", 6))
        return true;

    char szMapFile[MAX_PATH];
    int iMapLength = 0;
    sprintf(szMapFile, "maps/%s.bsp", STRING(gpGlobals->mapname));
    legacy_byte* pMapData = LOAD_FILE_FOR_ME(szMapFile, &iMapLength);
    if (!pMapData)
        return false;

    AgString sMapEntityData;
    dheader_t* pHeader = (dheader_t*)pMapData;
    if (pHeader->version == 29 || pHeader->version == 30)
    {
        int iMapDataLength = pHeader->lumps[LUMP_ENTITIES].filelen;
        int iMapDataOffset = pHeader->lumps[LUMP_ENTITIES].fileofs;
        pMapData[iMapDataLength] = '\0';
        if (NULL != strstr((const char*)&pMapData[iMapDataOffset], "info_hmdomdetect")
            || NULL != strstr((const char*)&pMapData[iMapDataOffset], "info_domdetect"))
        {
            FREE_FILE(pMapData);
            return true;
        }
    }
    FREE_FILE(pMapData);
    return false;
}
//--muphicks

void AgSound(CBasePlayer* pPlayer, const char* pszWave)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;
    if (!pPlayer->edict() || 0 == strlen(pszWave))
        return;

    CLIENT_COMMAND(pPlayer->edict(), "play %s\n", pszWave);
}

void AgPlayCountdown(CBasePlayer* pPlayer, int iSeconds)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    if (0 == iSeconds)
    {
        AgSound(pPlayer, "barney/ba_bring.wav");
    }
    else
    {

        if (!g_bLangame)
        {
            AgSound(pPlayer, "fvox/beep.wav");
        }
        else
        {
            if (1 == iSeconds)
                AgSound(pPlayer, "fvox/one.wav");
            else if (2 == iSeconds)
                AgSound(pPlayer, "fvox/two.wav");
            else if (3 == iSeconds)
                AgSound(pPlayer, "fvox/three.wav");
            else if (4 == iSeconds)
                AgSound(pPlayer, "fvox/four.wav");
            else if (5 == iSeconds)
                AgSound(pPlayer, "fvox/five.wav");
            else if (6 == iSeconds)
                AgSound(pPlayer, "fvox/six.wav");
            else if (7 == iSeconds)
                AgSound(pPlayer, "fvox/seven.wav");
            else if (8 == iSeconds)
                AgSound(pPlayer, "fvox/eight.wav");
            else if (9 == iSeconds)
                AgSound(pPlayer, "fvox/nine.wav");
            else if (10 == iSeconds)
                AgSound(pPlayer, "fvox/ten.wav");
        }
    }
}

bool AgIsLocalServer()
{
    return !(IS_DEDICATED_SERVER() && 0 == CVAR_GET_FLOAT("sv_lan"));
}


const char* AgGetGame()
{
    static char szGame[MAX_PATH];
    GET_GAME_DIR(szGame);
    char* pszGameDir = strrchr(szGame, '/');
    if (pszGameDir)
        return pszGameDir + 1;
    return szGame;
}

const char* AgGetDirectory()
{
    static char szGame[MAX_PATH];
    GET_GAME_DIR(szGame);
    char* pszGameDir = strrchr(szGame, '/');
    if (pszGameDir)
    {
        return szGame;
    }
    else
    {
        static char szDirectory[MAX_PATH] = "";
        if (strlen(szDirectory))
            return szDirectory;

#ifndef _WIN32
        getcwd(szDirectory, MAX_PATH);
#else
        ::GetCurrentDirectory(MAX_PATH, szDirectory);
#endif

        strcat(szDirectory, "/");
        strcat(szDirectory, szGame);
        return szDirectory;
    }
}

const char* AgGetDirectoryValve()
{
    static char szDirectory[MAX_PATH] = "";
    if (szDirectory[0] != '\0')
        return szDirectory;

    strcpy(szDirectory, AgGetDirectory());
    int iStart = strlen(szDirectory) - 1;
    while ('/' != szDirectory[iStart])
    {
        szDirectory[iStart] = '\0';
        iStart--;
        if (iStart == 0)
        {
            break;
        }
    }
    szDirectory[iStart] = '\0';
    return szDirectory;
}

//-- Martin Webrant
