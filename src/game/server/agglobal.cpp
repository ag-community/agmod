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
#include "gamerules.h"
#include "cvar.h"
#include <time.h>
#include <map>
#ifdef _WIN32
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#include <windows.h>
#endif
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

void AgInitTimer();

DLL_GLOBAL cvar_t ag_version = CVar::Create("sv_ag_version", "7.0", FCVAR_SERVER);

// ## General Gameplay ##

DLL_GLOBAL cvar_t ag_gametype = CVar::Create("sv_ag_gametype", "");

// The current gamemode
DLL_GLOBAL cvar_t ag_gamemode = CVar::Create("sv_ag_gamemode", "ffa", FCVAR_SERVER);

// Detect the gamemode based on the map and switch gamemode automatically
DLL_GLOBAL cvar_t ag_gamemode_auto = CVar::Create("sv_ag_gamemode_auto", "1", FCVAR_SERVER, CCVAR_VOTABLE);

DLL_GLOBAL cvar_t ag_allowed_gamemodes = CVar::Create("sv_ag_allowed_gamemodes", "", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default off
DLL_GLOBAL cvar_t ag_pure = CVar::Create("sv_ag_pure", "0", FCVAR_SERVER, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default not match. Protects players from wandering into the server
DLL_GLOBAL cvar_t ag_match_running = CVar::Create("sv_ag_match_running", "0", FCVAR_SERVER | FCVAR_UNLOGGED);

// Bhop cap off
DLL_GLOBAL cvar_t ag_oldphysics = CVar::Create("sv_ag_oldphysics", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// Allow timeout
DLL_GLOBAL cvar_t ag_allow_timeout = CVar::Create("sv_ag_allow_timeout", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

DLL_GLOBAL cvar_t ag_start_minplayers = CVar::Create("sv_ag_start_minplayers", "2", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default 5 seconds
DLL_GLOBAL cvar_t ag_player_id = CVar::Create("sv_ag_player_id", "5", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE);

// Default 0 = turned off
DLL_GLOBAL cvar_t ag_lj_timer = CVar::Create("sv_ag_lj_timer", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default 1 = Lame wallgauss on
DLL_GLOBAL cvar_t ag_wallgauss = CVar::Create("sv_ag_wallgauss", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default 3 = 3 times damage
DLL_GLOBAL cvar_t ag_headshot = CVar::Create("sv_ag_headshot", "3", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default 1 = Standard radius
DLL_GLOBAL cvar_t ag_blastradius = CVar::Create("sv_ag_blastradius", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

DLL_GLOBAL cvar_t ag_max_spectators = CVar::Create("sv_ag_max_spectators", "5", FCVAR_SERVER | FCVAR_UNLOGGED);
DLL_GLOBAL cvar_t ag_spec_enable_disable = CVar::Create("sv_ag_spec_enable_disable", "0", FCVAR_SERVER | FCVAR_UNLOGGED);
DLL_GLOBAL cvar_t ag_spectalk = CVar::Create("ag_spectalk", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

DLL_GLOBAL cvar_t ag_spawn_volume = CVar::Create("sv_ag_spawn_volume", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_show_gibs = CVar::Create("sv_ag_show_gibs", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE);

// Default 0 - no fix. 1 = ignore direct selfgauss; 2 = ignore every selfgauss
DLL_GLOBAL cvar_t ag_gauss_fix = CVar::Create("ag_gauss_fix", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE | CCVAR_BASIC_PRIVILEGES);

// Default 0 - no fix. 1 = improve rocket's start position when moving backwards and shooting RPG
DLL_GLOBAL cvar_t ag_rpg_fix = CVar::Create("ag_rpg_fix", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE | CCVAR_BASIC_PRIVILEGES);

// Default: 0 - Don't force clients to record matches
DLL_GLOBAL cvar_t ag_force_match_recording = CVar::Create("sv_ag_force_match_recording", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 0 - Don't force clients to take screenshots at the end of the map
DLL_GLOBAL cvar_t ag_force_take_end_screenshot = CVar::Create("sv_ag_force_take_end_screenshot", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 0 - Don't force clients to take screenshots when they die, only a single screenshot is taken
// per map or per match (if a match has been started with `agstart`)
DLL_GLOBAL cvar_t ag_force_take_death_screenshot = CVar::Create("sv_ag_force_take_death_screenshot", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 10 - Chance of taking a screenshot at death
DLL_GLOBAL cvar_t ag_force_take_death_screenshot_chance = CVar::Create("sv_ag_force_take_death_screenshot_chance", "10", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 0.75 - avg @ 144 fps was 0.83s, but sometimes it went down to 0.7s...
DLL_GLOBAL cvar_t ag_min_respawn_time = CVar::Create("sv_ag_min_respawn_time", "0.75", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 5 - in seconds
DLL_GLOBAL cvar_t ag_forcerespawn_time = CVar::Create("sv_ag_forcerespawn_time", "5", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 5 - How many AG bots at max.
DLL_GLOBAL cvar_t ag_bot_limit = CVar::Create("sv_ag_bot_limit", "5", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 1 - How many suicides will trigger for the player changing team during a match. 0 -> no penalty, 1 -> -1 frag & +1 death, 5 -> -5 frags & +5 deaths
DLL_GLOBAL cvar_t ag_match_teamchange_suicide_penalty = CVar::Create("sv_ag_match_teamchange_suicide_penalty", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 0 - Whether to have gauss/egon spend ammo or not
DLL_GLOBAL cvar_t ag_unlimited_uranium = CVar::Create("sv_ag_unlimited_uranium", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 1 - How much self-damage players take. 1 means normal, 0.5 means half
DLL_GLOBAL cvar_t ag_selfdamage = CVar::Create("sv_ag_selfdamage", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 1 - How much boost you get from damage. Works even when self-damage is disabled
DLL_GLOBAL cvar_t ag_selfdamage_boost = CVar::Create("sv_ag_selfdamage_boost", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

DLL_GLOBAL cvar_t ag_unstuck_on_changelevel = CVar::Create("sv_ag_unstuck_on_changelevel", "1");

// 100 attempts works for the pipe after c2p, some pipe after meatgrab and vortigaunts the before barney skip
DLL_GLOBAL cvar_t ag_unstuck_max_attempts = CVar::Create("sv_ag_unstuck_max_attempts", "100");

// ## Voting ##

// Voting is enabled by default. If 0, disables every vote, even if specific ones are enabled
DLL_GLOBAL cvar_t ag_allow_vote = CVar::Create("sv_ag_allow_vote", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 0 - Bots don't take part in votes
DLL_GLOBAL cvar_t ag_bot_allow_vote = CVar::Create("sv_ag_bot_allow_vote", "0", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 0 - Don't restrict votes
DLL_GLOBAL cvar_t ag_restrict_vote = CVar::Create("sv_ag_restrict_vote", "0", FCVAR_SERVER | FCVAR_UNLOGGED);

// `agstart` vote enable/disable
DLL_GLOBAL cvar_t ag_vote_start = CVar::Create("sv_ag_vote_start", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// On 1 allows voting settings like mp_timelimit and a few others
// On 2 allows voting a lot more mp_* cvars, like mp_forcerespawn, sv_ag_wallgauss, etc.
DLL_GLOBAL cvar_t ag_vote_setting = CVar::Create("sv_ag_vote_setting", "1", FCVAR_SERVER);

// Allows voting gamemodes
DLL_GLOBAL cvar_t ag_vote_gamemode = CVar::Create("sv_ag_vote_gamemode", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// `agkick` vote enable/disable
DLL_GLOBAL cvar_t ag_vote_kick = CVar::Create("sv_ag_vote_kick", "1", FCVAR_SERVER);

// `agallow` vote enable/disable
DLL_GLOBAL cvar_t ag_vote_allow = CVar::Create("sv_ag_vote_allow", "1", FCVAR_SERVER);

// `agadmin` vote enable/disable
DLL_GLOBAL cvar_t ag_vote_admin = CVar::Create("sv_ag_vote_admin", "0");

// `agmap` vote enable/disable
DLL_GLOBAL cvar_t ag_vote_map = CVar::Create("sv_ag_vote_map", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

DLL_GLOBAL cvar_t ag_vote_mp_timelimit_low = CVar::Create("sv_ag_vote_mp_timelimit_low", "10", FCVAR_SERVER | FCVAR_UNLOGGED);
DLL_GLOBAL cvar_t ag_vote_mp_timelimit_high = CVar::Create("sv_ag_vote_mp_timelimit_high", "40", FCVAR_SERVER | FCVAR_UNLOGGED);
DLL_GLOBAL cvar_t ag_vote_mp_fraglimit_low = CVar::Create("sv_ag_vote_mp_fraglimit_low", "0", FCVAR_SERVER | FCVAR_UNLOGGED);
DLL_GLOBAL cvar_t ag_vote_mp_fraglimit_high = CVar::Create("sv_ag_vote_mp_fraglimit_high", "100", FCVAR_SERVER | FCVAR_UNLOGGED);
DLL_GLOBAL cvar_t ag_vote_extra_timelimit = CVar::Create("sv_ag_vote_extra_timelimit", "30", FCVAR_SERVER | FCVAR_UNLOGGED);

// `spawnbot` vote enable/disable. Allow adding bots
DLL_GLOBAL cvar_t ag_vote_bot = CVar::Create("sv_ag_vote_bot", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// `agforceteamup` vote enable/disable. Allow voting to force someone to team up
DLL_GLOBAL cvar_t ag_vote_team = CVar::Create("sv_ag_vote_team", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// `agforcespectator` vote enable/disable. Allow voting to force someone into spectator mode
DLL_GLOBAL cvar_t ag_vote_spectator = CVar::Create("sv_ag_vote_spectator", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

DLL_GLOBAL cvar_t ag_vote_failed_time = CVar::Create("sv_ag_vote_failed_time", "30", FCVAR_SERVER | FCVAR_UNLOGGED);

// ## Weapon Bans ##

DLL_GLOBAL cvar_t ag_ban_crowbar = CVar::Create("sv_ag_ban_crowbar", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_glock = CVar::Create("sv_ag_ban_glock", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_357 = CVar::Create("sv_ag_ban_357", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_mp5 = CVar::Create("sv_ag_ban_mp5", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_shotgun = CVar::Create("sv_ag_ban_shotgun", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_crossbow = CVar::Create("sv_ag_ban_crossbow", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_rpg = CVar::Create("sv_ag_ban_rpg", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_gauss = CVar::Create("sv_ag_ban_gauss", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_egon = CVar::Create("sv_ag_ban_egon", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_hornet = CVar::Create("sv_ag_ban_hornet", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Ammo Bans ## (satchels and others are considered ammo)

DLL_GLOBAL cvar_t ag_ban_hgrenade = CVar::Create("sv_ag_ban_hgrenade", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_satchel = CVar::Create("sv_ag_ban_satchel", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_tripmine = CVar::Create("sv_ag_ban_tripmine", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_snark = CVar::Create("sv_ag_ban_snark", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_longjump = CVar::Create("sv_ag_ban_longjump", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_m203 = CVar::Create("sv_ag_ban_m203", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_9mmar = CVar::Create("sv_ag_ban_9mmar", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_bockshot = CVar::Create("sv_ag_ban_bockshot", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_uranium = CVar::Create("sv_ag_ban_uranium", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_bolts = CVar::Create("sv_ag_ban_bolts", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_rockets = CVar::Create("sv_ag_ban_rockets", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_357ammo = CVar::Create("sv_ag_ban_357ammo", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Other Bans ##

DLL_GLOBAL cvar_t ag_ban_armour = CVar::Create("sv_ag_ban_armour", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_health = CVar::Create("sv_ag_ban_health", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_ban_recharg = CVar::Create("sv_ag_ban_recharg", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Starting Weapons ##

DLL_GLOBAL cvar_t ag_start_crowbar = CVar::Create("sv_ag_start_crowbar", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_glock = CVar::Create("sv_ag_start_glock", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_357 = CVar::Create("sv_ag_start_357", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_mp5 = CVar::Create("sv_ag_start_mp5", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_shotgun = CVar::Create("sv_ag_start_shotgun", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_crossbow = CVar::Create("sv_ag_start_crossbow", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_rpg = CVar::Create("sv_ag_start_rpg", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_gauss = CVar::Create("sv_ag_start_gauss", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_egon = CVar::Create("sv_ag_start_egon", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_hornet = CVar::Create("sv_ag_start_hornet", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Starting Ammo ##

DLL_GLOBAL cvar_t ag_start_hgrenade = CVar::Create("sv_ag_start_hgrenade", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_satchel = CVar::Create("sv_ag_start_satchel", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_tripmine = CVar::Create("sv_ag_start_tripmine", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_snark = CVar::Create("sv_ag_start_snark", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_longjump = CVar::Create("sv_ag_start_longjump", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_m203 = CVar::Create("sv_ag_start_m203", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_9mmar = CVar::Create("sv_ag_start_9mmar", "68", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_bockshot = CVar::Create("sv_ag_start_bockshot", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_uranium = CVar::Create("sv_ag_start_uranium", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_bolts = CVar::Create("sv_ag_start_bolts", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_rockets = CVar::Create("sv_ag_start_rockets", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_357ammo = CVar::Create("sv_ag_start_357ammo", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Other Starting Elements ##

DLL_GLOBAL cvar_t ag_start_armour = CVar::Create("sv_ag_start_armour", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_start_health = CVar::Create("sv_ag_start_health", "100", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Weapon Damage ##

DLL_GLOBAL cvar_t ag_dmg_crowbar = CVar::Create("sv_ag_dmg_crowbar", "25", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_glock = CVar::Create("sv_ag_dmg_glock", "12", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_357 = CVar::Create("sv_ag_dmg_357", "40", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_mp5 = CVar::Create("sv_ag_dmg_mp5", "12", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_shotgun = CVar::Create("sv_ag_dmg_shotgun", "20", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_crossbow = CVar::Create("sv_ag_dmg_crossbow", "20", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_bolts = CVar::Create("sv_ag_dmg_bolts", "50", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_rpg = CVar::Create("sv_ag_dmg_rpg", "120", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_gauss = CVar::Create("sv_ag_dmg_gauss", "20", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_egon_wide = CVar::Create("sv_ag_dmg_egon_wide", "20", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_egon_narrow = CVar::Create("sv_ag_dmg_egon_narrow", "10", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_hornet = CVar::Create("sv_ag_dmg_hornet", "10", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_hgrenade = CVar::Create("sv_ag_dmg_hgrenade", "100", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_satchel = CVar::Create("sv_ag_dmg_satchel", "120", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_tripmine = CVar::Create("sv_ag_dmg_tripmine", "150", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_dmg_m203 = CVar::Create("sv_ag_dmg_m203", "100", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## CTF ##

// The time that a dropped flag lays in the world before respawning
DLL_GLOBAL cvar_t ag_ctf_flag_resettime = CVar::Create("sv_ag_ctf_flag_resettime", "30", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The number of captures before map ends
DLL_GLOBAL cvar_t ag_ctf_capture_limit = CVar::Create("sv_ag_ctf_capturelimit", "10", FCVAR_SERVER, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The ammount of points his teammates get
DLL_GLOBAL cvar_t ag_ctf_teamcapturepoints = CVar::Create("sv_ag_ctf_teamcapturepoints", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The amount of points the capturer gets
DLL_GLOBAL cvar_t ag_ctf_capturepoints = CVar::Create("sv_ag_ctf_capturepoints", "4", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The amount of points the returner gets
DLL_GLOBAL cvar_t ag_ctf_returnpoints = CVar::Create("sv_ag_ctf_returnpoints", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The amount of points the killer gets
DLL_GLOBAL cvar_t ag_ctf_carrierkillpoints = CVar::Create("sv_ag_ctf_carrierkillpoints", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The amount of points the stealer gets
DLL_GLOBAL cvar_t ag_ctf_stealpoints = CVar::Create("sv_ag_ctf_stealpoints", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// The amount of points the defender gets
DLL_GLOBAL cvar_t ag_ctf_defendpoints = CVar::Create("sv_ag_ctf_defendpoints", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// 1 for roundbased CTF game.
DLL_GLOBAL cvar_t ag_ctf_roundbased = CVar::Create("sv_ag_ctf_roundbased", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## DOM ##

//++ muphicks

// Number of seconds team must control point to score
DLL_GLOBAL cvar_t ag_dom_mincontroltime = CVar::Create("sv_ag_dom_mincontroltime", "5", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Number of points scored when under teams control
DLL_GLOBAL cvar_t ag_dom_controlpoints = CVar::Create("sv_ag_dom_controlpoints", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Max time under 1 teams control 5*6 = 30 seconds
DLL_GLOBAL cvar_t ag_dom_resetscorelimit = CVar::Create("sv_ag_dom_resetscorelimit", "6", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Max points a team needs to get to win the game
DLL_GLOBAL cvar_t ag_dom_scorelimit = CVar::Create("sv_ag_dom_scorelimit", "200", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

//-- muphicks

// ## Spawn System ##

// Default 0 - classic mode (select a random spawn from the next 5)
DLL_GLOBAL cvar_t ag_spawn_system = CVar::Create("ag_spawn_system", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default 25 - remember all of these spawnpoints that were last used
DLL_GLOBAL cvar_t ag_spawn_history_entries = CVar::Create("ag_spawn_history_entries", "25", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// This is to avoid repeating the last used spawnpoints, 0.25 means avoid the 25% of total spawns that were last used,
// so in crossfire there are 17 spawns, the 25% of that is 4.25, it's rounded to 4, so it will avoid the last 4 spots where you spawned in crossfire
// It can be capped by `ag_spawn_history_entries`, so if its value is 3 (less than 4), it would avoid the last 3 instead
DLL_GLOBAL cvar_t ag_spawn_avoid_last_spots = CVar::Create("ag_spawn_avoid_last_spots", "0.25", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// A fraction of the total spawnpoints, that will be used for the Far spawn system to choose a spawnpoint randomly
DLL_GLOBAL cvar_t ag_spawn_far_spots = CVar::Create("ag_spawn_far_spots", "0.25", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Probabilities for the Position-aware Spawn System to pick these spot categories for the next spawn
// 10 - means 10%, 25 = 25%, 65 = 65%
DLL_GLOBAL cvar_t ag_spawn_pa_visible_chance = CVar::Create("ag_spawn_pa_visible_chance", "10", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_spawn_pa_audible_chance = CVar::Create("ag_spawn_pa_audible_chance", "25", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_spawn_pa_safe_chance = CVar::Create("ag_spawn_pa_safe_chance", "65", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 0 - No changes to seeding. It will use the random functions provided by the engine, whose seeding depends on the epoch time
// and it has its own algorithm to provide a random number from that seed
// If set to any other value, that value will be used as the seed for the spawn selection
DLL_GLOBAL cvar_t ag_spawn_seed = CVar::Create("ag_spawn_seed", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## FPS Limiter ##

// Some of these should be prefixed sv_ag_* instead of just ag_*, because it seems the convention
// was to have the non-votable ones with sv_ag_* and the votable ones with ag_*. But it might be
// quite confusing for actual usage, as you have to remember which ones are votable and which not.
// So I've decided to break the convention here to make it easier for players and admins to use these,
// as you can just type ag_fps_limit and tab or press down arrow to see all the different cvars, instead
// of having to do this with both sv_ag_fps_limit and ag_fps_limit. We'll see if it's the right decision

// Default: 0 - Cap players' fps_max. Standard in 2021 is 144 (125 and 100 before; 250 for bhop)
DLL_GLOBAL cvar_t ag_fps_limit = CVar::Create("ag_fps_limit", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 0 - Whether to limit the fps to the most common fps among players
DLL_GLOBAL cvar_t ag_fps_limit_auto = CVar::Create("ag_fps_limit_auto", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 10 seconds - How often to check for changing the limit
DLL_GLOBAL cvar_t ag_fps_limit_check_interval = CVar::Create("ag_fps_limit_check_interval", "10.0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE);

// Default: 0 - Whether to limit it only for players in a match
DLL_GLOBAL cvar_t ag_fps_limit_match_only = CVar::Create("ag_fps_limit_match_only", "0", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 1 - Whether to account for the 0.5 fps added by the engine (steampipe)
DLL_GLOBAL cvar_t ag_fps_limit_steampipe = CVar::Create("ag_fps_limit_steampipe", "1", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 3 - How many warnings before applying the punishment
DLL_GLOBAL cvar_t ag_fps_limit_warnings = CVar::Create("ag_fps_limit_warnings", "3", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 5 seconds - Time between warnings
DLL_GLOBAL cvar_t ag_fps_limit_warnings_interval = CVar::Create("ag_fps_limit_warnings_interval", "5.0", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: kick - Options: slap, kick, ban (3 mins)
DLL_GLOBAL cvar_t ag_fps_limit_punishment = CVar::Create("ag_fps_limit_punishment", "kick", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 1.0 - Multiplier for slap damage & punch
DLL_GLOBAL cvar_t ag_fps_limit_punishment_slap_intensity = CVar::Create("ag_fps_limit_punishment_slap_intensity", "1.0", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 1 second - Time between slaps
DLL_GLOBAL cvar_t ag_fps_limit_punishment_slap_interval = CVar::Create("ag_fps_limit_punishment_slap_interval", "1.0", FCVAR_SERVER | FCVAR_UNLOGGED);

// Default: 3 minutes - How much time to ban them for
DLL_GLOBAL cvar_t ag_fps_limit_punishment_ban_time = CVar::Create("ag_fps_limit_punishment_ban_time", "3", FCVAR_SERVER | FCVAR_UNLOGGED);

// ## Satchels ##

// Default: 0 - Cannot destroy satchels (only with the detonator)
DLL_GLOBAL cvar_t ag_satchel_destroyable = CVar::Create("sv_ag_satchel_destroyable", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 15 hp
DLL_GLOBAL cvar_t ag_satchel_health = CVar::Create("sv_ag_satchel_health", "15", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default: 1 - Solid, players can collide with them
DLL_GLOBAL cvar_t ag_satchel_solid = CVar::Create("sv_ag_satchel_solid", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Flood Protection / Mute ##

DLL_GLOBAL cvar_t ag_floodmsgs = CVar::Create("sv_ag_floodmsgs", "4");
DLL_GLOBAL cvar_t ag_floodpersecond = CVar::Create("sv_ag_floodpersecond", "4");
DLL_GLOBAL cvar_t ag_floodwaitdelay = CVar::Create("sv_ag_floodwaitdelay", "10");

// Default: 2 - Seconds to wait before changing their name again
DLL_GLOBAL cvar_t ag_flood_name_cooldown = CVar::Create("sv_ag_flood_name_cooldown", "2");

// Default: 60 - Seconds to wait before changing their name again when on spec
DLL_GLOBAL cvar_t ag_flood_name_spec_cooldown = CVar::Create("sv_ag_flood_name_spec_cooldown", "60");

// Default: 2 - Seconds to wait before changing their model again
DLL_GLOBAL cvar_t ag_flood_model_cooldown = CVar::Create("sv_ag_flood_model_cooldown", "2");

// Default: 60 - Seconds to wait before changing their model again when on spec
DLL_GLOBAL cvar_t ag_flood_model_spec_cooldown = CVar::Create("sv_ag_flood_model_spec_cooldown", "60");

// Default: 15 - Seconds to wait before target can change their model or specmode again
DLL_GLOBAL cvar_t ag_enforcement_cooldown = CVar::Create("sv_agforce_cooldown", "15");

// Default: 0 - Don't mute chat messages during match
DLL_GLOBAL cvar_t ag_match_mute = CVar::Create("sv_ag_match_mute", "0", FCVAR_SERVER | FCVAR_UNLOGGED);

// ## Nukes / Ghostmining / Lampgaussing ##

DLL_GLOBAL cvar_t ag_allow_nuke = CVar::Create("sv_ag_allow_nuke", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_grenade = CVar::Create("sv_ag_nuke_grenade", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_crossbow = CVar::Create("sv_ag_nuke_crossbow", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_rpg = CVar::Create("sv_ag_nuke_rpg", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_gauss = CVar::Create("sv_ag_nuke_gauss", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_egon = CVar::Create("sv_ag_nuke_egon", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_tripmine = CVar::Create("sv_ag_nuke_tripmine", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_satchel = CVar::Create("sv_ag_nuke_satchel", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_snark = CVar::Create("sv_ag_nuke_snark", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_nuke_other = CVar::Create("sv_ag_nuke_other", "1", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// There's some weird code in CGrenade::Explode that pulls the explosion out of the surface, sometimes WAY too much, to the point where
// it can go and explode at the other side of a wall/ceiling/floor, like when you put satchels on lamps in boot_camp. And how far it
// pulls it out of the surface depends on the damage it deals, which I don't think is correct. So this fixes that, but it has the side-effect
// of fixing most of the ghostmining you can do with explosives (satchels, tripmines, rockets, and grenades in general), which should in theory
// be handled by the ag_nuke* specific cvars, so yeah there's some functionality overlap here, but it's not 100% since this fixes satchels
// on lamps in boot_camp exploding at the other side of the ceiling, which doesn't produce a nuke, so it's not ignorable with sv_ag_nuke_satchel 0;
// and the nuke cvars should prevent ANY case of nuking, and there might be cases leading to a nuke where the explosion fix has nothing to do.
// Default 0 - no fix. 1 = fix the explosion origin
DLL_GLOBAL cvar_t ag_explosion_fix = CVar::Create("sv_ag_explosion_fix", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// ## Others ##

// Default 1 = Autoauthenticate admins based on won id
DLL_GLOBAL cvar_t ag_auto_admin = CVar::Create("sv_ag_auto_admin", "1");

DLL_GLOBAL cvar_t mm_agsay = CVar::Create("mm_agsay", "1", FCVAR_SERVER);

// Default 0 - Disabled. On 1 it shows the time it took to finish a fraglimit-based game
DLL_GLOBAL cvar_t ag_speedrun = CVar::Create("sv_ag_speedrun", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE);

// Default 0 - Default to gametype default. If 1 or higher then that will be the countdown before a match/game starts
DLL_GLOBAL cvar_t ag_countdown = CVar::Create("sv_ag_countdown", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

// Default 0 - Don't log all the gamemode-related cvars at the start of the map.
// On 1 it will log the changed cvars at the start and at the end of the multiplayer game
// On 2 it will log all the gamemode cvars with their current values at the start,
// and also the changed cvars at the start and at the end of the multiplayer game
// On 3 it will log every single bogus change to cvars at the start, plus the changed
// cvars at the end of the multiplayer game
DLL_GLOBAL cvar_t ag_log_cvars = CVar::Create("sv_ag_log_cvars", "0", FCVAR_SERVER | FCVAR_UNLOGGED);

// When doing `delay_changelevel`, on 1 it prints the map change message and delay status to chat instead of console
DLL_GLOBAL cvar_t ag_say_on_changelevel_delay = CVar::Create("sv_ag_say_on_changelevel_delay", "1");

// ## HL25 Gameplay changes ##

DLL_GLOBAL cvar_t ag_hl25_satchel_controls = CVar::Create("sv_ag_hl25_satchel_controls", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_hl25_handgrenade_throw = CVar::Create("sv_ag_hl25_handgrenade_throw", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_hl25_mp5_max_initial_ammo = CVar::Create("sv_ag_hl25_mp5_max_initial_ammo", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);
DLL_GLOBAL cvar_t ag_hl25_mp5_spread = CVar::Create("sv_ag_hl25_mp5_spread", "0", FCVAR_SERVER | FCVAR_UNLOGGED, CCVAR_VOTABLE | CCVAR_GAMEMODE);

DLL_GLOBAL bool g_bLangame = false;
DLL_GLOBAL bool g_bUseTeamColors = false;

std::vector<CBaseEntity *> g_spawnPoints;
std::vector<CBaseEntity *> g_spawnHistory;

DLL_GLOBAL float g_flSpeedrunStartTime = 0.0f;

DLL_GLOBAL AgRandom g_spawnRNG;

void LoadGreetingMessages();

void AgInitGame()
{
	AgInitTimer();

	CVAR_REGISTER(&ag_version);

	// ## General Gameplay ##
	CVAR_REGISTER(&ag_gametype);
	CVAR_REGISTER(&ag_gamemode);
	CVAR_REGISTER(&ag_gamemode_auto);
	CVAR_REGISTER(&ag_allowed_gamemodes);
	CVAR_REGISTER(&ag_pure);
	CVAR_REGISTER(&ag_match_running);
	CVAR_REGISTER(&ag_oldphysics);
	CVAR_REGISTER(&ag_allow_timeout);
	CVAR_REGISTER(&ag_start_minplayers);
	CVAR_REGISTER(&ag_player_id);
	CVAR_REGISTER(&ag_lj_timer);
	CVAR_REGISTER(&ag_wallgauss);
	CVAR_REGISTER(&ag_headshot);
	CVAR_REGISTER(&ag_blastradius);
	CVAR_REGISTER(&ag_max_spectators);
	CVAR_REGISTER(&ag_spec_enable_disable);
	CVAR_REGISTER(&ag_spectalk);
	CVAR_REGISTER(&ag_spawn_volume);
	CVAR_REGISTER(&ag_show_gibs);
	CVAR_REGISTER(&ag_gauss_fix);
	CVAR_REGISTER(&ag_rpg_fix);
	CVAR_REGISTER(&ag_force_match_recording);
	CVAR_REGISTER(&ag_force_take_end_screenshot);
	CVAR_REGISTER(&ag_force_take_death_screenshot);
	CVAR_REGISTER(&ag_force_take_death_screenshot_chance);
	CVAR_REGISTER(&ag_min_respawn_time);
	CVAR_REGISTER(&ag_forcerespawn_time);
	CVAR_REGISTER(&ag_bot_limit);
	CVAR_REGISTER(&ag_match_teamchange_suicide_penalty);
	CVAR_REGISTER(&ag_unlimited_uranium);
	CVAR_REGISTER(&ag_selfdamage);
	CVAR_REGISTER(&ag_selfdamage_boost);
	CVAR_REGISTER(&ag_unstuck_on_changelevel);
	CVAR_REGISTER(&ag_unstuck_max_attempts);

	// ## Voting ##
	CVAR_REGISTER(&ag_allow_vote);
	CVAR_REGISTER(&ag_bot_allow_vote);
	CVAR_REGISTER(&ag_restrict_vote);
	CVAR_REGISTER(&ag_vote_start);
	CVAR_REGISTER(&ag_vote_setting);
	CVAR_REGISTER(&ag_vote_gamemode);
	CVAR_REGISTER(&ag_vote_kick);
	CVAR_REGISTER(&ag_vote_allow);
	CVAR_REGISTER(&ag_vote_admin);
	CVAR_REGISTER(&ag_vote_map);
	CVAR_REGISTER(&ag_vote_mp_timelimit_low);
	CVAR_REGISTER(&ag_vote_mp_timelimit_high);
	CVAR_REGISTER(&ag_vote_mp_fraglimit_low);
	CVAR_REGISTER(&ag_vote_mp_fraglimit_high);
	CVAR_REGISTER(&ag_vote_extra_timelimit);
	CVAR_REGISTER(&ag_vote_bot);
	CVAR_REGISTER(&ag_vote_team);
	CVAR_REGISTER(&ag_vote_spectator);
	CVAR_REGISTER(&ag_vote_failed_time);

	// ## Weapon Bans ##
	CVAR_REGISTER(&ag_ban_crowbar);
	CVAR_REGISTER(&ag_ban_glock);
	CVAR_REGISTER(&ag_ban_357);
	CVAR_REGISTER(&ag_ban_mp5);
	CVAR_REGISTER(&ag_ban_shotgun);
	CVAR_REGISTER(&ag_ban_crossbow);
	CVAR_REGISTER(&ag_ban_rpg);
	CVAR_REGISTER(&ag_ban_gauss);
	CVAR_REGISTER(&ag_ban_egon);
	CVAR_REGISTER(&ag_ban_hornet);

	// ## Ammo Bans ##
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

	// ## Other Bans ##
	CVAR_REGISTER(&ag_ban_armour);
	CVAR_REGISTER(&ag_ban_health);
	CVAR_REGISTER(&ag_ban_recharg);

	// ## Starting Weapons ##
	CVAR_REGISTER(&ag_start_crowbar);
	CVAR_REGISTER(&ag_start_glock);
	CVAR_REGISTER(&ag_start_357);
	CVAR_REGISTER(&ag_start_mp5);
	CVAR_REGISTER(&ag_start_shotgun);
	CVAR_REGISTER(&ag_start_crossbow);
	CVAR_REGISTER(&ag_start_rpg);
	CVAR_REGISTER(&ag_start_gauss);
	CVAR_REGISTER(&ag_start_egon);
	CVAR_REGISTER(&ag_start_hornet);

	// ## Starting Ammo ##
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

	// ## Other Starting Elements ##
	CVAR_REGISTER(&ag_start_armour);
	CVAR_REGISTER(&ag_start_health);

	// ## Weapon Damage ##
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

	// ## CTF ##
	CVAR_REGISTER(&ag_ctf_flag_resettime);
	CVAR_REGISTER(&ag_ctf_capturepoints);
	CVAR_REGISTER(&ag_ctf_teamcapturepoints);
	CVAR_REGISTER(&ag_ctf_capture_limit);
	CVAR_REGISTER(&ag_ctf_returnpoints);
	CVAR_REGISTER(&ag_ctf_carrierkillpoints);
	CVAR_REGISTER(&ag_ctf_stealpoints);
	CVAR_REGISTER(&ag_ctf_defendpoints);
	CVAR_REGISTER(&ag_ctf_roundbased);

	// ## DOM ##
	//++ muphicks
	CVAR_REGISTER(&ag_dom_mincontroltime);
	CVAR_REGISTER(&ag_dom_controlpoints);
	CVAR_REGISTER(&ag_dom_resetscorelimit);
	CVAR_REGISTER(&ag_dom_scorelimit);
	//-- muphicks

	// ## Spawn System ##
	CVAR_REGISTER(&ag_spawn_system);
	CVAR_REGISTER(&ag_spawn_history_entries);
	CVAR_REGISTER(&ag_spawn_avoid_last_spots);
	CVAR_REGISTER(&ag_spawn_far_spots);
	CVAR_REGISTER(&ag_spawn_pa_visible_chance);
	CVAR_REGISTER(&ag_spawn_pa_audible_chance);
	CVAR_REGISTER(&ag_spawn_pa_safe_chance);
	CVAR_REGISTER(&ag_spawn_seed);

	// ## FPS Limiter ##
	CVAR_REGISTER(&ag_fps_limit);
	CVAR_REGISTER(&ag_fps_limit_auto);
	CVAR_REGISTER(&ag_fps_limit_check_interval);
	CVAR_REGISTER(&ag_fps_limit_match_only);
	CVAR_REGISTER(&ag_fps_limit_steampipe);
	CVAR_REGISTER(&ag_fps_limit_warnings);
	CVAR_REGISTER(&ag_fps_limit_warnings_interval);
	CVAR_REGISTER(&ag_fps_limit_punishment);
	CVAR_REGISTER(&ag_fps_limit_punishment_slap_intensity);
	CVAR_REGISTER(&ag_fps_limit_punishment_slap_interval);
	CVAR_REGISTER(&ag_fps_limit_punishment_ban_time);

	// ## Satchels ##
	CVAR_REGISTER(&ag_satchel_destroyable);
	CVAR_REGISTER(&ag_satchel_health);
	CVAR_REGISTER(&ag_satchel_solid);

	// ## Flood Protection / Mute ##
	CVAR_REGISTER(&ag_floodmsgs);
	CVAR_REGISTER(&ag_floodpersecond);
	CVAR_REGISTER(&ag_floodwaitdelay);
	CVAR_REGISTER(&ag_flood_name_cooldown);
	CVAR_REGISTER(&ag_flood_name_spec_cooldown);
	CVAR_REGISTER(&ag_flood_model_cooldown);
	CVAR_REGISTER(&ag_flood_model_spec_cooldown);
	CVAR_REGISTER(&ag_enforcement_cooldown);
	CVAR_REGISTER(&ag_match_mute);

	// ## Nukes / Ghostmining / Lampgaussing ##
	CVAR_REGISTER(&ag_allow_nuke);
	CVAR_REGISTER(&ag_nuke_grenade);
	CVAR_REGISTER(&ag_nuke_crossbow);
	CVAR_REGISTER(&ag_nuke_rpg);
	CVAR_REGISTER(&ag_nuke_gauss);
	CVAR_REGISTER(&ag_nuke_egon);
	CVAR_REGISTER(&ag_nuke_tripmine);
	CVAR_REGISTER(&ag_nuke_satchel);
	CVAR_REGISTER(&ag_nuke_snark);
	CVAR_REGISTER(&ag_nuke_other);
	CVAR_REGISTER(&ag_explosion_fix);

	// ## Others ##
	CVAR_REGISTER(&ag_auto_admin);
	CVAR_REGISTER(&mm_agsay);
	CVAR_REGISTER(&ag_speedrun);
	CVAR_REGISTER(&ag_countdown);
	CVAR_REGISTER(&ag_log_cvars);
	CVAR_REGISTER(&ag_say_on_changelevel_delay);

	// ## HL25 Gameplay changes ##
	CVAR_REGISTER(&ag_hl25_satchel_controls);
	CVAR_REGISTER(&ag_hl25_handgrenade_throw);
	CVAR_REGISTER(&ag_hl25_mp5_max_initial_ammo);
	CVAR_REGISTER(&ag_hl25_mp5_spread);

	Command.Init();

	CVar::StartRecordingChanges();

	// Set up initial settings. Add "startup_" before
	char *servercfgfile = (char *)CVAR_GET_STRING("servercfgfile");

	if (servercfgfile && servercfgfile[0])
	{
		ALERT(at_console, "Executing dedicated server startup config file\n");
		SERVER_COMMAND(UTIL_VarArgs("exec startup_%s\n", servercfgfile));
		SERVER_EXECUTE_HL();
	}

	g_bLangame = 0 < CVAR_GET_FLOAT("sv_lan");

	GameMode.Init();
	LoadGreetingMessages();
}

// TITLES FOR HALF-LIFE
//  Position command $position x y
//  x & y are from 0 to 1 to be screen resolution independent
//  -1 means center in each dimension
//  Effect command $effect <effect number>
//  effect 0 is fade in/fade out
//  effect 1 is flickery credits
//  effect 2 is write out (training room)
//  Text color r g b command $color
//  fadein time fadeout time / hold time
//  $fadein (message fade in time - per character in effect 2)
//  $fadeout (message fade out time)
//  $holdtime (stay on the screen for this long)

void AgSay(CBasePlayer *pPlayer, const AgString &sText, float *pfFloodProtected, float fHoldTime, float x, float y, int iChannel, int r, int g, int b)
{
	if (g_fGameOver)
		return;

	if (pfFloodProtected)
	{
		if (*pfFloodProtected > gpGlobals->time)
			return;

		*pfFloodProtected = gpGlobals->time + fHoldTime;
	}

	hudtextparms_t hText;
	memset(&hText, 0, sizeof(hText));
	hText.channel = iChannel;
	// These X and Y coordinates are just above
	//  the health meter.
	hText.x = x;
	hText.y = y;

	hText.r1 = r;
	hText.g1 = g;
	hText.b1 = b;
	// hText.r1 = hText.g1 = hText.b1 = 180;
	hText.a1 = 0;

	hText.r2 = hText.g2 = hText.b2 = 0;
	hText.a2 = 0;

	hText.holdTime = fHoldTime - 0.30;

	hText.effect = 2; // Fade in/out
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
			CBasePlayer *pPlayerLoop = AgPlayerByIndex(i);
			if (pPlayerLoop)
				UTIL_HudMessage(pPlayerLoop, hText, sText.c_str());
		}
	}
}

CBasePlayer *AgPlayerByIndex(int iPlayerIndex)
{
	CBasePlayer *pPlayer = NULL;

	if (iPlayerIndex > 0 && iPlayerIndex <= gpGlobals->maxClients)
	{
		edict_t *pPlayerEdict = INDEXENT(iPlayerIndex);
		if (pPlayerEdict && !pPlayerEdict->free && pPlayerEdict->pvPrivateData)
		{
			CBaseEntity *pEnt = (CBaseEntity *)CBaseEntity::Instance(pPlayerEdict);
			if (pEnt && pEnt->pev && CLASS_PLAYER == pEnt->Classify())
			{
				if (pEnt->pev->netname && 0 != STRING(pEnt->pev->netname)[0])
				{
					pPlayer = (CBasePlayer *)pEnt;
				}
			}
		}
	}

	return pPlayer;
}

CBasePlayer *AgPlayerByName(const AgString &sNameOrPlayerNumber, CBasePlayer *pPlayer)
{
	if (sNameOrPlayerNumber.empty())
	{
		AgConsole("Please, provide a player to vote for.", pPlayer);
		return NULL;
	}

	std::vector<CBasePlayer *> matchedPlayers;
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayerLoop = AgPlayerByIndex(i);
		if (pPlayerLoop)
		{
			if (0 == stricmp(pPlayerLoop->GetName(), sNameOrPlayerNumber.c_str()) || "#" == sNameOrPlayerNumber.substr(0, 1) && GETPLAYERUSERID(pPlayerLoop->edict()) == atoi(sNameOrPlayerNumber.substr(1).c_str()))
			{
				return pPlayerLoop;
			}
			else
			{
				// TODO: improve all of this. Just trying to match case-insensitively
				AgString targetName = sNameOrPlayerNumber;
				AgString playerName = pPlayerLoop->GetName();

				AgToLower(targetName);
				AgToLower(playerName);

				auto matched = strstr(playerName.c_str(), targetName.c_str());
				if (matched)
				{
					// TODO: if it matches the colored name partially, should it have precedence
					// over the uncolored match? if there's "stat^5ic" and "Static" in the server
					// and I do "agallow stat^5", right now it matches both, one through the
					// colored partial match and the other through the uncolored match, and I think
					// you probably want to vote the colored one only since you were very specific
					// when calling the vote, as you included color codes in the name...
					matchedPlayers.push_back(pPlayerLoop);
					continue;
				}

				// Try without colors codes now
				RemoveColorCodes(targetName);
				RemoveColorCodes(playerName);

				matched = strstr(playerName.c_str(), targetName.c_str());
				if (matched)
					matchedPlayers.push_back(pPlayerLoop);
			}
		}
	}

	if (matchedPlayers.empty())
		AgConsole("No such player exist on server.", pPlayer);
	else if (matchedPlayers.size() == 1)
	{
		const auto result = matchedPlayers[0];
		return result; // avoid C26816 warning
	}
	else
	{
		AgString msg;
		msg.append("Several players matched that name, which one did you mean to vote?\n");
		for (const auto match : matchedPlayers)
		{
			msg.append(UTIL_VarArgs("-> %s (#%d)\n", match->GetName(), GETPLAYERUSERID(match->edict())));
		}
		// Split the message into chunks, because only the first 127 bytes of the message are displayed
		// on the client's console, and this is potentially longer than that
		for (size_t i = 0; i < msg.size(); i += 127)
		{
			AgString chunk = msg.substr(i, 127);
			AgConsole(chunk, pPlayer);
		}
	}

	return NULL;
}

CBasePlayer *AgPlayerByAuthID(const AgString &authID)
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayerLoop = AgPlayerByIndex(i);
		if (pPlayerLoop)
		{
			if (FStrEq(pPlayerLoop->GetAuthID(), authID.c_str()))
				return pPlayerLoop;
		}
	}
	return NULL;
}

void RemoveColorCodes(AgString &sInput)
{
	size_t i = 0;
	while (i < sInput.size())
	{
		if (sInput[i] == '^' && i + 1 < sInput.size() && isdigit(sInput[i + 1]))
		{
			sInput.erase(i, 2);
		}
		else
		{
			i++;
		}
	}
}

void AgChangelevel(const AgString &sLevelname)
{
	if (32 < sLevelname.size() || 0 == sLevelname.size())
		return;

	char szTemp[64];
	strcpy(szTemp, sLevelname.c_str());

	// Check if it exists.
	if (IS_MAP_VALID(szTemp))
		// Change the level
		CHANGE_LEVEL(szTemp, NULL);
}

void AgConsole(const AgString &sText, CBasePlayer *pPlayer)
{
	// We add a line break if there isn't one already
	const AgString &msg = UTIL_EndsWith(sText, "\n") ? sText.c_str() : UTIL_VarArgs("%s\n", sText.c_str());

	// Split the message into chunks, because only the first 127 bytes of the message are displayed
	// on the client's console, and this is potentially longer than that
	for (size_t i = 0; i < msg.size(); i += 127)
	{
		AgString chunk = msg;
		if (msg.size() > 127)
			chunk = msg.substr(i, 127);

		if (pPlayer && pPlayer->pev)
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, chunk.c_str());
		else
			g_engfuncs.pfnServerPrint(chunk.c_str());
	}
}

void AgResetMap()
{
	CBaseEntity *pEntity = NULL;

	edict_t *pEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
	for (int i = 1; i < gpGlobals->maxEntities; i++, pEdict++)
	{
		pEntity = CBaseEntity::Instance(pEdict);
		if (pEntity && pEntity->pev)
		{
			const char *pszClass = STRING(pEntity->pev->classname);

			if (pszClass && '\0' != pszClass[0])
			{
				if (0 == strncmp(pszClass, "weapon_", 7) || 0 == strncmp(pszClass, "ammo_", 5) || 0 == strncmp(pszClass, "item_", 5))
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
		((CBaseToggle *)pEntity)->Reset();

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "func_recharge")) != NULL)
		((CBaseToggle *)pEntity)->Reset();

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "rpg_rocket")) != NULL)
		pEntity->pev->dmg = 0;

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "grenade")) != NULL)
		pEntity->pev->dmg = 0;
}

char *AgStringToLower(char *pszString)
{
	if (NULL == pszString)
		return pszString;

	char *pszTemp = pszString;
	while ('\0' != pszTemp[0])
	{
		*pszTemp = tolower(*pszTemp);
		pszTemp++;
	}
	return pszString;
}

void AgToLower(AgString &strLower)
{
	size_t i = 0;
	while (i < strLower.size())
	{
		strLower[i] = tolower(strLower[i]);
		i++;
	}
}

void AgTrim(AgString &sTrim)
{
	if (0 == sTrim.size())
		return;

	int b = sTrim.find_first_not_of(" \t\r\n");
	int e = sTrim.find_last_not_of(" \t\r\n");
	if (b == -1) // No non-whitespaces
		sTrim = "";
	else
		sTrim = std::string(sTrim, b, e - b + 1);
}

void AgLogError(const char *pszLog)
{
	char szFile[MAX_PATH];
	sprintf(szFile, "%s/agslog.txt", AgGetDirectory());
	FILE *pFile = fopen(szFile, "a+");
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

void AgDirList(const AgString &sDir, AgStringSet &setFiles)
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
	DIR *pDirectory = opendir(sDir.c_str());
	if (pDirectory)
	{
		struct dirent *pFile = NULL;

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

void AgStripColors(char *pszString)
{
	char *pszIt = pszString;
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

void AgGetDetails(char *pszDetails, int iMaxSize, int *piSize)
{
	int iBytes = 0;
	char *pszBuffer = pszDetails;

	// Write Match running
	iBytes += sprintf(&pszBuffer[iBytes], "match\\%d", ag_match_running.value > 0);
	pszBuffer[iBytes] = '\0';

	*piSize = iBytes;
}

void AgGetPlayerInfo(char *pszDetails, int iMaxSize, int *piSize)
{
	int iBytes = 0;
	char *pszBuffer = pszDetails;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (iBytes + 100 > iMaxSize)
			break;
		CBasePlayer *pPlayerLoop = AgPlayerByIndex(i);
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

char *AgOSVersion()
{
#ifdef _WIN32
	static char verbuf[256];
#else
	static char verbuf[4 * SYS_NMLN + 4];
#endif

#ifdef _WIN32
	OSVERSIONINFO VersionInfo;

	VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
	if (GetVersionEx(&VersionInfo))
	{
		if (strlen(VersionInfo.szCSDVersion) > 200)
			VersionInfo.szCSDVersion[100] = 0;
		sprintf(verbuf, "Windows %d.%d build%d PlatformId %d SP=\"%s\"\n",
		    VersionInfo.dwMajorVersion,
		    VersionInfo.dwMinorVersion,
		    VersionInfo.dwBuildNumber,
		    VersionInfo.dwPlatformId,
		    VersionInfo.szCSDVersion);
	}
	else
	{
		strcpy(verbuf, "WINDOWS UNKNOWN\n");
	}
#else
	struct utsname ubuf;

	if (uname(&ubuf))
	{
		strcpy(verbuf, "LINUX UNKNOWN\n");
	}
	else
	{
		sprintf(verbuf, "%s %s %s %s\n",
		    ubuf.sysname,
		    ubuf.release,
		    ubuf.version,
		    ubuf.machine);
	}
#endif
	return verbuf;
}

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

AgString AgReadFile(const char *pszFile)
{
	AgString sData;

	char szFile[MAX_PATH];
	char szData[4096];
	sprintf(szFile, "%s/%s", AgGetDirectory(), pszFile);
	FILE *pFile = fopen(szFile, "r");
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

typedef std::map<AgString, AgString, std::less<AgString>> AgAuthIDToGreeting;
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

void AgDisplayGreetingMessage(const char *pszAuthID)
{
	AgAuthIDToGreeting::iterator itrGreeting = s_mapGreeting.find(pszAuthID);
	if (itrGreeting != s_mapGreeting.end())
		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, (*itrGreeting).second.c_str());
}

bool AgIsCTFMap(const char *pszMap)
{
#define LUMP_ENTITIES     0
#define HEADER_LUMPS      15
#define MAX_MAP_ENTSTRING (128 * 1024)

	typedef struct
	{
		int fileofs, filelen;
	} lump_t;

	typedef struct
	{
		int version;
		lump_t lumps[HEADER_LUMPS];
	} dheader_t;

	if (0 == strncmp(pszMap, "agctf_", 6))
		return true;
	else if (0 == strncmp(pszMap, "hlectf_", 6))
		return true;

	char szMapFile[MAX_PATH];
	int iMapLength = 0;
	sprintf(szMapFile, "maps/%s.bsp", STRING(gpGlobals->mapname));
	byte *pMapData = LOAD_FILE_FOR_ME(szMapFile, &iMapLength);
	if (!pMapData)
		return false;

	AgString sMapEntityData;
	dheader_t *pHeader = (dheader_t *)pMapData;
	if (pHeader->version == 29 || pHeader->version == 30)
	{
		int iMapDataLength = pHeader->lumps[LUMP_ENTITIES].filelen;
		int iMapDataOffset = pHeader->lumps[LUMP_ENTITIES].fileofs;
		pMapData[iMapDataLength] = '\0';
		if (NULL != strstr((const char *)&pMapData[iMapDataOffset], "info_hmctfdetect") || NULL != strstr((const char *)&pMapData[iMapDataOffset], "info_ctfdetect"))
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
bool AgIsDOMMap(const char *pszMap)
{
#define LUMP_ENTITIES     0
#define HEADER_LUMPS      15
#define MAX_MAP_ENTSTRING (128 * 1024)

	typedef struct
	{
		int fileofs, filelen;
	} lump_t;

	typedef struct
	{
		int version;
		lump_t lumps[HEADER_LUMPS];
	} dheader_t;

	if (0 == strncmp(pszMap, "agdom_", 6))
		return true;

	char szMapFile[MAX_PATH];
	int iMapLength = 0;
	sprintf(szMapFile, "maps/%s.bsp", STRING(gpGlobals->mapname));
	byte *pMapData = LOAD_FILE_FOR_ME(szMapFile, &iMapLength);
	if (!pMapData)
		return false;

	AgString sMapEntityData;
	dheader_t *pHeader = (dheader_t *)pMapData;
	if (pHeader->version == 29 || pHeader->version == 30)
	{
		int iMapDataLength = pHeader->lumps[LUMP_ENTITIES].filelen;
		int iMapDataOffset = pHeader->lumps[LUMP_ENTITIES].fileofs;
		pMapData[iMapDataLength] = '\0';
		if (NULL != strstr((const char *)&pMapData[iMapDataOffset], "info_hmdomdetect") || NULL != strstr((const char *)&pMapData[iMapDataOffset], "info_domdetect"))
		{
			FREE_FILE(pMapData);
			return true;
		}
	}
	FREE_FILE(pMapData);
	return false;
}
//--muphicks

void AgSound(CBasePlayer *pPlayer, const char *pszWave)
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

void AgPlayCountdown(CBasePlayer *pPlayer, int iSeconds)
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

const char *AgGetGame()
{
	static char szGame[MAX_PATH];
	GET_GAME_DIR(szGame);
	char *pszGameDir = strrchr(szGame, '/');
	if (pszGameDir)
		return pszGameDir + 1;
	return szGame;
}

const char *AgGetDirectory()
{
	static char szGame[MAX_PATH];
	GET_GAME_DIR(szGame);
	char *pszGameDir = strrchr(szGame, '/');
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
		(void)!getcwd(szDirectory, MAX_PATH);
#else
		::GetCurrentDirectory(MAX_PATH, szDirectory);
#endif

		strcat(szDirectory, "/");
		strcat(szDirectory, szGame);
		return szDirectory;
	}
}

const char *AgGetDirectoryValve()
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

bool IsNukeAllowed(entvars_t *pevInflictor)
{
	if (ag_allow_nuke.value == 0.0f)
		return false;

	if (FClassnameIs(pevInflictor, "grenade"))
	{
		// TODO: differentiate between handgrenade and smg nade
		return ag_nuke_grenade.value != 0.0f;
	}
	else if (FClassnameIs(pevInflictor, "bolt"))
		return ag_nuke_crossbow.value != 0.0f; // untested, 30-Dec-2021

	else if (FClassnameIs(pevInflictor, "weapon_gauss"))
		return ag_nuke_gauss.value != 0.0f;

	else if (FClassnameIs(pevInflictor, "weapon_egon"))
		return ag_nuke_egon.value != 0.0f;

	else if (FClassnameIs(pevInflictor, "rpg_rocket"))
		return ag_nuke_rpg.value != 0.0f;

	else if (FClassnameIs(pevInflictor, "monster_satchel"))
		return ag_nuke_satchel.value != 0.0f;

	else if (FClassnameIs(pevInflictor, "monster_tripmine"))
		return ag_nuke_tripmine.value != 0.0f;

	else if (FClassnameIs(pevInflictor, "monster_snark"))
		return ag_nuke_snark.value != 0.0f; // untested, 30-Dec-2021

	return ag_nuke_other.value != 0.0f;
}

void ReseedSpawnSystem()
{
	auto seed = static_cast<int>(ag_spawn_seed.value);
	if (seed != 0)
	{
		// Seed for spawn selection
		g_spawnRNG.SeedRNG(std::abs(seed));
		g_spawnHistory.clear();

		AgConsole(UTIL_VarArgs("Using seed %d for the spawns", g_spawnRNG.GetSeed()));
	}
}

float GetCountdownTimeOrDefault(float defaultValue)
{
	float countdown;
	if (ag_countdown.value == 0.0f)
		countdown = defaultValue;
	else if (ag_countdown.value < MIN_COUNTDOWN_TIME)
	{
		countdown = MIN_COUNTDOWN_TIME;
		CVAR_SET_FLOAT("sv_ag_countdown", MIN_COUNTDOWN_TIME);
	}
	else
		countdown = ag_countdown.value;

	return countdown;
}
