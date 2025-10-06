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
//
// teamplay_gamerules.cpp
//
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
//++ BulliT
#include "multiplay_gamerules.h"
#include "aggamerules.h"
//-- Martin Webrant
#include "game.h"

static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
static int num_teams = 0;
//++ BulliT
static char s_szLeastPlayers[MAX_TEAMNAME_LENGTH];
//-- Martin Webrant

extern DLL_GLOBAL bool g_fGameOver;

CHalfLifeTeamplay ::CHalfLifeTeamplay()
{
	m_DisableDeathMessages = false;
	m_DisableDeathPenalty = false;

	memset(team_names, 0, sizeof(team_names));
	memset(team_scores, 0, sizeof(team_scores));
	num_teams = 0;

	// Copy over the team from the server config
	m_szTeamList[0] = 0;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	strncpy(m_szTeamList, teamlist.string, TEAMPLAY_TEAMLISTLENGTH);

	edict_t *pWorld = INDEXENT(0);
	if (pWorld && pWorld->v.team)
	{
		//++ BulliT
		if (CTF == AgGametype())
		{
			sprintf(m_szTeamList, "%s;%s", CTF_TEAM1_NAME, CTF_TEAM2_NAME);
		}
		//-- Martin Webrant
		else if (teamoverride.value)
		{
			const char *pTeamList = STRING(pWorld->v.team);
			if (pTeamList && strlen(pTeamList))
			{
				strncpy(m_szTeamList, pTeamList, TEAMPLAY_TEAMLISTLENGTH);
			}
		}
	}
	// Has the server set teams
	if (strlen(m_szTeamList))
		m_teamLimit = true;
	else
		m_teamLimit = false;

	RecountTeams();
}

extern cvar_t timeleft, fragsleft;

#include "voice_gamemgr.h"
extern CVoiceGameMgr g_VoiceGameMgr;

void CHalfLifeTeamplay ::Think(void)
{
	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	g_VoiceGameMgr.Update(gpGlobals->frametime);
	//++ BulliT
	if (!AgGameRules::AgThink())
		return;
	//-- Martin Webrant

	if (g_fGameOver) // someone else quit the game already
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	//++ BulliT
	/*
	  float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;

	  time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);

	  if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
	  {
		  GoToIntermission();
		  return;
	  }
	*/

	if (!m_Timer.TimeRemaining(time_remaining))
	{
		if (!m_SuddenDeath.IsSuddenDeath())
		{
			// Go intermission.
			GoToIntermission();
			return;
		}
		else
		{
			// Sudden death!
			time_remaining = 0;
		}
	}

	if (CTF == AgGametype())
	{
		if (m_CTF.CaptureLimit())
		{
			GoToIntermission();
			return;
		}
	} //++ muphicks
	else if (DOM == AgGametype())
		if (m_DOM.ScoreLimit())
		{
			GoToIntermission();
			return;
		}
	//-- muphicks
	//-- Martin Webrant

	float flFragLimit = fraglimit.value;
	if (flFragLimit)
	{
		int bestfrags = 9999;
		int remain;

		// check if any team is over the frag limit
		for (int i = 0; i < num_teams; i++)
		{
			if (team_scores[i] >= flFragLimit)
			{
				GoToIntermission();
				return;
			}

			remain = flFragLimit - team_scores[i];
			if (remain < bestfrags)
			{
				bestfrags = remain;
			}
		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if (frags_remaining != last_frags)
	{
		g_engfuncs.pfnCvar_DirectSet(&fragsleft, UTIL_VarArgs("%i", frags_remaining));
	}

	// Updates once per second
	if (timeleft.value != last_time)
	{
		g_engfuncs.pfnCvar_DirectSet(&timeleft, UTIL_VarArgs("%i", time_remaining));
	}

	last_frags = frags_remaining;
	last_time = time_remaining;
}

//=========================================================
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
bool CHalfLifeTeamplay ::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	//++ BulliT
	if (CHalfLifeMultiplay::ClientCommand(pPlayer, pcmd))
		return true;
	//-- Martin Webrant
	if (g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
		return true;

	if (FStrEq(pcmd, "menuselect"))
	{
		if (CMD_ARGC() < 2)
			return true;

		int slot = atoi(CMD_ARGV(1));

		// select the item from the current menu

		return true;
	}

	return false;
}

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;
extern int gmsgTeamNames;
extern int gmsgScoreInfo;

void CHalfLifeTeamplay ::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(1); // game mode teamplay
	MESSAGE_END();
}

const char *CHalfLifeTeamplay::SetDefaultPlayerTeam(CBasePlayer *pPlayer)
{
	// copy out the team name from the model
	char *mdls = g_engfuncs.pfnInfoKeyValue(g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model");
	strncpy(pPlayer->m_szTeamName, mdls, TEAM_NAME_LENGTH);
	//++ BulliT
	if (TEAM_NAME_LENGTH <= strlen(pPlayer->m_szTeamName))
		pPlayer->m_szTeamName[TEAM_NAME_LENGTH - 1] = '\0';
	AgStringToLower(pPlayer->m_szTeamName);

	if (pPlayer->pev->flags & FL_PROXY)
	{
		strcpy(pPlayer->m_szTeamName, "");
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName);
	}
	else if (pPlayer->HasModelEnforced())
	{
		strcpy(pPlayer->m_szTeamName, pPlayer->m_enforcedModel.c_str());
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName);
	}
	else if (0 == strlen(pPlayer->m_szTeamName))
	{
		strcpy(pPlayer->m_szTeamName, "gordon");
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName);
	}
	//-- Martin Webrant

	RecountTeams();

	// update the current player of the team he is joining
	if (pPlayer->m_szTeamName[0] == '\0' || !IsValidTeam(pPlayer->m_szTeamName) || defaultteam.value)
	{
		const char *pTeamName = NULL;

		if (defaultteam.value)
		{
			pTeamName = team_names[0];
		}
		else
		{
			pTeamName = TeamWithFewestPlayers();
		}
		strncpy(pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH);
	}

	return pPlayer->m_szTeamName;
}

//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD(CBasePlayer *pPlayer)
{
	int i;

	SetDefaultPlayerTeam(pPlayer);
	CHalfLifeMultiplay::InitHUD(pPlayer);

	// Send down the team names
	MESSAGE_BEGIN(MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict());
	WRITE_BYTE(num_teams);
	for (i = 0; i < num_teams; i++)
	{
		WRITE_STRING(team_names[i]);
	}
	MESSAGE_END();
	//++ BulliT
	pPlayer->m_iNumTeams = num_teams;
	//-- Martin Webrant

	RecountTeams();

	char *mdls = g_engfuncs.pfnInfoKeyValue(g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model");
	// update the current player of the team he is joining
	char text[1024];
	if (!strcmp(mdls, pPlayer->m_szTeamName))
	{
		sprintf(text, "* you are on team \'%s\'\n", pPlayer->m_szTeamName);
	}
	else
	{
		sprintf(text, "* assigned to team %s\n", pPlayer->m_szTeamName);
	}

	ChangePlayerTeam(pPlayer, pPlayer->m_szTeamName, false, false);
	UTIL_SayText(text, pPlayer);
	int clientIndex = pPlayer->entindex();
	RecountTeams();
	// update this player with all the other players team info
	// loop through all active players and send their team info to the new client
	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *plr = UTIL_PlayerByIndex(i);
		if (plr && IsValidTeam(plr->TeamID()))
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict());
			WRITE_BYTE(plr->entindex());
			WRITE_STRING(plr->TeamID());
			MESSAGE_END();
		}
	}
	//++ BulliT
	if (CTF == AgGametype())
		m_CTF.PlayerInitHud(pPlayer);
	//++ muphicks
	else if (DOM == AgGametype())
		m_DOM.PlayerInitHud(pPlayer);
	//-- muphicks

#define MENU_TEAM 2
	if (strlen(m_szTeamList))
		pPlayer->ShowVGUI(MENU_TEAM);
	//-- Martin Webrant
}

void CHalfLifeTeamplay::ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, bool bKill, bool bGib)
{
	int damageFlags = DMG_GENERIC;
	int clientIndex = pPlayer->entindex();

	if (!bGib)
	{
		damageFlags |= DMG_NEVERGIB;
	}
	else
	{
		damageFlags |= DMG_ALWAYSGIB;
	}

	if (bKill)
	{
		// kill the player,  remove a death,  and let them start on the new team
		m_DisableDeathMessages = true;
		m_DisableDeathPenalty = true;

		if (ag_match_running.value != 0.0f && ag_match_teamchange_suicide_penalty.value >= 1.0f)
		{
			m_DisableDeathPenalty = false;

			// -1 because a PlayerKilled() will already be triggered due to m_DisableDeathPenalty being false
			const auto extraSuicides = static_cast<int>(ag_match_teamchange_suicide_penalty.value) - 1;

			pPlayer->m_iDeaths += extraSuicides;
			pPlayer->pev->frags -= extraSuicides;
		}

		entvars_t *pevWorld = VARS(INDEXENT(0));
		pPlayer->TakeDamage(pevWorld, pevWorld, 900, damageFlags);

		m_DisableDeathMessages = false;
		m_DisableDeathPenalty = false;
	}

	// copy out the team name from the model
	strncpy(pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH);

	g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
	g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName);

	//++ BulliT
	RecountTeams();
	//-- Martin Webrant

	// notify everyone's HUD of the team change
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
	WRITE_BYTE(clientIndex);
	//++ BulliT
	WRITE_STRING(pPlayer->TeamID());
	//		WRITE_STRING( pPlayer->m_szTeamName );
	//-- Martin Webrant
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
	WRITE_BYTE(clientIndex);
	WRITE_SHORT(pPlayer->pev->frags);
	WRITE_SHORT(pPlayer->m_iDeaths);
	//++ BulliT
	WRITE_SHORT(g_teamplay);
	WRITE_SHORT(g_pGameRules->GetTeamIndex(pPlayer->m_szTeamName) + 1);
	//-- Martin Webrant
	MESSAGE_END();

	// Spectator
	MESSAGE_BEGIN(MSG_ALL, gmsgSpectator);
	WRITE_BYTE(ENTINDEX(pPlayer->edict()));
	WRITE_BYTE(pPlayer->IsSpectator() ? 1 : 0);
	MESSAGE_END();
}

//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
	//++ BulliT
	AgGameRules::ClientUserInfoChanged(pPlayer, infobuffer);
	//-- Martin Webrant
	char text[1024];

	// prevent skin/color/model changes
	char *mdls = g_engfuncs.pfnInfoKeyValue(infobuffer, "model");

	if (!stricmp(mdls, pPlayer->m_szTeamName))
		return;

	int clientIndex = pPlayer->entindex();

	if (defaultteam.value)
	{
		g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
		g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName);
		if (!pPlayer->IsBot())
		{
			sprintf(text, "* Not allowed to change teams in this game!\n");
			UTIL_SayText(text, pPlayer);
		}
		return;
	}

	if (defaultteam.value || !IsValidTeam(mdls))
	{
		g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
		if (!pPlayer->IsBot())
		{
			sprintf(text, "* Can't change team to \'%s\'\n", mdls);
			UTIL_SayText(text, pPlayer);
			sprintf(text, "* Server limits teams to \'%s\'\n", m_szTeamList);
			UTIL_SayText(text, pPlayer);
		}
		return;
	}

	if (pPlayer->HasModelFlooded())
	{
		// Restore the model they had before changing
		g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "model", pPlayer->m_szTeamName);
		g_engfuncs.pfnSetClientKeyValue(clientIndex, g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName);
		// FIXME: when i do `setinfo` in the client, i can see the model that i set even though the server
		// denies the change and restores the previous model. So it's not really restored and it makes it
		// confusing on the client because when you have the model as red in the setinfo, but you're blue,
		// and want to change it to red, it won't send anything to the server when you type `model red`,
		// and you won't see it changing and you won't see any flood protection warning ('cos the server doesn't know).
		// So you're typing in stuff and nothing happens, that's not a good user experience...
		return;
	}

	if (pPlayer->HasModelEnforced())
	{
		pPlayer->ChangeTeam(pPlayer->m_enforcedModel.c_str(), true);
		return;
	}

	// notify everyone of the team change
	// TODO: investigate why this only gets executed sometimes when a player joins,
	// between the "... connected, address ..." and "... STEAM USERID validated" log messages
	sprintf(text, "* %s has changed to team \'%s\'\n", pPlayer->GetName(), mdls);
	UTIL_DispatchChat(nullptr, ChatType::TEAM_CHANGE, text);

	UTIL_LogPrintf("\"%s<%i><%s><%s>\" joined team \"%s\"\n",
	    pPlayer->GetName(),
	    GETPLAYERUSERID(pPlayer->edict()),
	    GETPLAYERAUTHID(pPlayer->edict()),
	    pPlayer->m_szTeamName,
	    mdls);

	ChangePlayerTeam(pPlayer, mdls, true, true);

	pPlayer->m_flLastModelChange = AgTime();

	// recount stuff
	//++ BulliT
	// RecountTeams( true );
	//	RecountTeams();
	// MSGTEST ResendScoreBoard();
	//-- Martin Webrant
}

extern int gmsgDeathMsg;

//=========================================================
// Deathnotice.
//=========================================================
void CHalfLifeTeamplay::DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor)
{
	if (m_DisableDeathMessages)
		return;

	if (pVictim && pKiller && pKiller->flags & FL_CLIENT)
	{
		CBasePlayer *pk = (CBasePlayer *)CBaseEntity::Instance(pKiller);

		if (pk)
		{
			if ((pk != pVictim) && pVictim->IsTeammate(pk))
			{
				MESSAGE_BEGIN(MSG_ALL, gmsgDeathMsg);
				WRITE_BYTE(ENTINDEX(ENT(pKiller))); // the killer
				WRITE_BYTE(ENTINDEX(pVictim->edict())); // the victim
				WRITE_STRING("teammate"); // flag this as a teammate kill
				MESSAGE_END();
				return;
			}
		}
	}

	CHalfLifeMultiplay::DeathNotice(pVictim, pKiller, pevInflictor);
}

//=========================================================
//=========================================================
void CHalfLifeTeamplay ::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor)
{
	//++ BulliT
	if (pVictim && CTF == AgGametype())
		m_CTF.PlayerKilled(pVictim, pKiller);

	if (pVictim && LMS == AgGametype())
		pVictim->SetIngame(false); // Cant respawn
	//-- Martin Webrant
	if (!m_DisableDeathPenalty)
	{
		CHalfLifeMultiplay::PlayerKilled(pVictim, pKiller, pInflictor);
		RecountTeams();
	}
}

//=========================================================
// IsTeamplay
//=========================================================
bool CHalfLifeTeamplay::IsTeamplay(void)
{
	return true;
}

bool CHalfLifeTeamplay::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (pAttacker && pPlayer->IsTeammate(pAttacker))
	{
		// my teammate hit me.
		if ((friendlyfire.value == 0) && (pAttacker != pPlayer))
		{
			// friendly fire is off, and this hit came from someone other than myself,  then don't get hurt
			return false;
		}
	}

	return CHalfLifeMultiplay::FPlayerCanTakeDamage(pPlayer, pAttacker);
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget)
{
	//++ BulliT
	if (ARENA == AgGametype())
		return GR_NOTTEAMMATE;
	//-- Martin Webrant
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if (!pPlayer || !pTarget || !pTarget->IsPlayer())
		return GR_NOTTEAMMATE;

	if ((*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp(GetTeamID(pPlayer), GetTeamID(pTarget)))
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
bool CHalfLifeTeamplay::ShouldAutoAim(CBasePlayer *pPlayer, edict_t *target)
{
	// always autoaim, unless target is a teammate
	CBaseEntity *pTgt = CBaseEntity::Instance(target);
	if (pTgt && pTgt->IsPlayer())
	{
		if (pPlayer->IsTeammate(pTgt))
			return false; // don't autoaim at teammates
	}

	return CHalfLifeMultiplay::ShouldAutoAim(pPlayer, target);
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled)
{
	//++ BulliT
	AgGameRules::IPointsForKill(pAttacker, pKilled);
	//-- Martin Webrant
	if (!pKilled)
		return 0;

	if (!pAttacker)
		return 1;

	if (pAttacker != pKilled && pAttacker->IsTeammate(pKilled))
		return -1;

	return 1;
}

//=========================================================
//=========================================================
const char *CHalfLifeTeamplay::GetTeamID(CBaseEntity *pEntity)
{
	if (pEntity == NULL || pEntity->pev == NULL)
		return "";

	// return their team name
	return pEntity->TeamID();
}

int CHalfLifeTeamplay::GetTeamIndex(const char *pTeamName)
{
	if (pTeamName && *pTeamName != 0)
	{
		// try to find existing team
		for (int tm = 0; tm < num_teams; tm++)
		{
			if (!stricmp(team_names[tm], pTeamName))
				return tm;
		}
	}

	return -1; // No match
}

const char *CHalfLifeTeamplay::GetIndexedTeamName(int teamIndex)
{
	if (teamIndex < 0 || teamIndex >= num_teams)
		return "";

	return team_names[teamIndex];
}

bool CHalfLifeTeamplay::IsValidTeam(const char *pTeamName)
{
	if (!m_teamLimit) // Any team is valid if the teamlist isn't set
		return true;

	return (GetTeamIndex(pTeamName) != -1) ? true : false;
}

const char *CHalfLifeTeamplay::TeamWithFewestPlayers(void)
{
	//++ BulliT
	if (strlen(s_szLeastPlayers))
		return s_szLeastPlayers;
	//-- Martin Webrant
	int i;
	int minPlayers = MAX_TEAMS;
	int teamCount[MAX_TEAMS];
	char *pTeamName = NULL;

	memset(teamCount, 0, MAX_TEAMS * sizeof(int));

	// loop through all clients, count number of players on each team
	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *plr = UTIL_PlayerByIndex(i);

		if (plr)
		{
			int team = GetTeamIndex(plr->TeamID());
			if (team >= 0)
				teamCount[team]++;
		}
	}

	// Find team with least players
	for (i = 0; i < num_teams; i++)
	{
		if (teamCount[i] < minPlayers)
		{
			minPlayers = teamCount[i];
			pTeamName = team_names[i];
		}
	}

	return pTeamName;
}

//=========================================================
//=========================================================
//++ BulliT
// void CHalfLifeTeamplay::RecountTeams( bool bResendInfo )
void CHalfLifeTeamplay::RecountTeams()
//-- Martin Webrant
{
	char *pName;
	char teamlist[TEAMPLAY_TEAMLISTLENGTH];
	int i = 0;
	// loop through all teams, recounting everything
	num_teams = 0;

	// Copy all of the teams from the teamlist
	// make a copy because strtok is destructive
	strcpy(teamlist, m_szTeamList);
	pName = teamlist;
	pName = strtok(pName, ";");
	while (pName != NULL && *pName)
	{
		if (GetTeamIndex(pName) < 0)
		{
			strcpy(team_names[num_teams], pName);
			num_teams++;
		}
		pName = strtok(NULL, ";");
	}

	if (num_teams < 2)
	{
		num_teams = 0;
		m_teamLimit = false;
	}

	//++ BulliT
	s_szLeastPlayers[0] = '\0';
	if (m_teamLimit)
	{
		int teamCount[MAX_TEAMS];
		int minPlayers = MAX_TEAMS;
		int i = 0;
		memset(teamCount, 0, MAX_TEAMS * sizeof(int));

		// loop through all clients, count number of players on each team
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer *pPlayerLoop = AgPlayerByIndex(i);
			if (pPlayerLoop)
			{
				int team = GetTeamIndex(pPlayerLoop->m_szTeamName);
				if (team >= 0)
					teamCount[team]++;
			}
		}

		// Find team with least players
		for (i = 0; i < num_teams; i++)
		{
			if (teamCount[i] < minPlayers)
			{
				minPlayers = teamCount[i];
				strcpy(s_szLeastPlayers, team_names[i]);
			}
		}
	}
	else
	{
		strcpy(team_names[num_teams++], CTF_TEAM1_NAME);
		strcpy(team_names[num_teams++], CTF_TEAM2_NAME);
	}
	//-- Martin Webrant

	// Sanity check
	memset(team_scores, 0, sizeof(team_scores));

	// loop through all clients
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *plr = UTIL_PlayerByIndex(i);

		if (plr)
		{
			const char *pTeamName = plr->TeamID();
			// try add to existing team
			int tm = GetTeamIndex(pTeamName);

			if (tm < 0) // no team match found
			{
				if (!m_teamLimit)
				{
					// add to new team
					tm = num_teams;
					num_teams++;
					team_scores[tm] = 0;
					strncpy(team_names[tm], pTeamName, MAX_TEAMNAME_LENGTH);
				}
			}

			if (tm >= 0)
			{
				team_scores[tm] += plr->pev->frags;
			}

			//++ BulliT
			plr->edict()->v.team = tm + 1;
			//-- Martin Webrant
		}
	}

	//++ BulliT
	// loop through all clients and send team names
	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayerLoop = AgPlayerByIndex(i);
		if (pPlayerLoop)
		{
			if (pPlayerLoop->m_iNumTeams != num_teams)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgTeamNames, NULL, pPlayerLoop->edict());
				WRITE_BYTE(num_teams);
				for (i = 0; i < num_teams; i++)
				{
					WRITE_STRING(team_names[i]);
				}
				MESSAGE_END();
				pPlayerLoop->m_iNumTeams = num_teams;
			}
		}
	}
	//-- Martin Webrant
}
