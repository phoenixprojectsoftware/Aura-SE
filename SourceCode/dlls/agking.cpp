/****
*
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* King of the Hill
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
#include "agking.h"

#include <string.h>

extern int gmsgTeamScore;

FILE_GLOBAL int s_iKOTHTeam1Score;
FILE_GLOBAL int s_iKOTHTeam2Score;

#define MAXHILLS 32
#define HILLTIME 60
#define SCOREINTERVAL 1
#define POINTS 1

AgKing::AgKing()
{
	m_iActiveHill = -1;

	m_iTeam1Score = 0;
	m_iTeam2Score = 0;
	s_iKOTHTeam1Score = 0;
	s_iKOTHTeam2Score = 0;

	m_flNextThink = 0.0f;
	m_flNextScoreTime = 0.0f;
	m_flNextHillChange = 0.0f;

	m_iBeamSprite = 0;
	m_flNextRenderTime = 0.0f;

	m_bLoaded = false;
}

AgKing::~AgKing() {}

void AgKing::Reset()
{
	m_iHillCount = 0;
	m_iActiveHill = -1;
	m_Hills.clear();

	m_iTeam1Score = 0;
	m_iTeam2Score = 0;
	s_iKOTHTeam1Score = 0;
	s_iKOTHTeam2Score = 0;

	m_flNextThink = gpGlobals->time;
	m_flNextScoreTime = gpGlobals->time + 1.0f;
	m_flNextHillChange = gpGlobals->time + HILLTIME;

	m_flNextRenderTime = 0.0f;

	m_bLoaded = false;
}

void AgKing::Load()
{
	Precache();
	m_Hills.clear();
	m_iHillCount = 0;
	m_iActiveHill = -1;

	char szFile[MAX_PATH];
	sprintf(szFile, "%s/koth/%s.koth", AgGetDirectory(), STRING(gpGlobals->mapname));

	FILE* pFile = fopen(szFile, "r");
	if (!pFile)
	{
		ALERT(at_console, "KING: this map does not support King of the Hill! Missing %s\n", szFile);
		return;
	}

	char line[256];

	while (fgets(line, sizeof(line), pFile))
	{
		if (m_iHillCount >= MAXHILLS)
			break;

		if (line[0] == '#' || line[0] == '/' || line[0] == '\n' || line[0] == '\r')
			continue;

		AgKingFile hill;
		memset(&hill, 0, sizeof(hill));

		// .koth File Format:
		// name x y z sizeX size Y size Z
		// Example:
		// Courtyard 128 512 -64 256 256 128
		int parsed = sscanf(
			line,
			"%63s %f %f %f %f %f %f",
			hill.m_szName,
			&hill.m_vOrigin.x,
			&hill.m_vOrigin.y,
			&hill.m_vOrigin.z,
			&hill.m_vSize.x,
			&hill.m_vSize.y,
			&hill.m_vSize.z
		);

		if (parsed != 7)
			continue;

		hill.m_vMins = hill.m_vOrigin - hill.m_vSize * 0.5f;
		hill.m_vMaxs = hill.m_vOrigin + hill.m_vSize * 0.5f;

		m_Hills.push_back(hill);
		m_iHillCount = (int)m_Hills.size();

		ALERT(
			at_console,
			"KING: Loaded hill %d '%s' origin %.1f %.1f %.1f size %.1f %.1f %.1f\n",
			m_iHillCount,
			hill.m_szName,
			hill.m_vOrigin.x,
			hill.m_vOrigin.y,
			hill.m_vOrigin.z,
			hill.m_vSize.x,
			hill.m_vSize.y,
			hill.m_vSize.z
		);

		m_iHillCount++;
	}

	fclose(pFile);

	m_bLoaded = true;
	ALERT(at_console, "KING: loaded %d hill zones from %s\n", m_iHillCount, szFile);

	if (m_iHillCount > 0)
		SetActiveHill(0);
}

void AgKing::Precache()
{
	m_iBeamSprite = PRECACHE_MODEL("sprites/laserbeam.spr");
}

void AgKing::Think()
{
	if (!g_pGameRules)
		return;

	if (!m_bLoaded)
	{
		Load();

		m_flNextThink = gpGlobals->time + 0.1f;
		m_flNextScoreTime = gpGlobals->time + SCOREINTERVAL;
		m_flNextHillChange = gpGlobals->time + HILLTIME;
	}

	if (m_Hills.empty())
		return;

	if (gpGlobals->time < m_flNextThink)
		return;

	m_flNextThink = gpGlobals->time + 0.1f;

	// we've used all the hills; loop back around.
	if (m_iActiveHill < 0 || m_iActiveHill >= (int)m_Hills.size())
		SetActiveHill(0);

	if (gpGlobals->time >= m_flNextHillChange)
		RotateHill();

	if (gpGlobals->time >= m_flNextScoreTime)
	{
		UpdateHillControl();

		float flInterval = SCOREINTERVAL;
		if (flInterval <= 0.0f)
			flInterval = 1.0f;

		m_flNextScoreTime = gpGlobals->time + flInterval;
	}

	RenderActiveHill();
	SendTeamScores();
}

void AgKing::SetActiveHill(int index)
{
	if (index < 0 || index >= (int)m_Hills.size())
	{
		ALERT(at_console, "KING: SetActiveHill invalid index %d, hill vector size %d, hill count %d\n",
			index,
			(int)m_Hills.size(),
			m_iHillCount);

		m_iActiveHill = -1;
		return;
	}

	m_iActiveHill = index;
	m_iHillCount = (int)m_Hills.size();

	float flHillTime = HILLTIME;
	if (flHillTime <= 0.0f)
		flHillTime = 60.0f;

	m_flNextHillChange = gpGlobals->time + flHillTime;

	AgKingFile& hill = m_Hills[m_iActiveHill];

	char szText[192];
	snprintf(szText, sizeof(szText), "Hill active: %s", hill.m_szName);

	AgConsole(szText);
	UTIL_ClientPrintAll(HUD_PRINTCENTER, szText);

	ALERT(at_console, "KING: Active hill is now '%s'\n", hill.m_szName);
}

void AgKing::RotateHill()
{
	if (m_Hills.empty())
		return;

	int nextHill = m_iActiveHill + 1;

	if (nextHill < 0 || nextHill >= (int)m_Hills.size())
		nextHill = 0;

	SetActiveHill(nextHill);
}

bool AgKing::IsPlayerInsideHill(CBasePlayer* pPlayer, const AgKingFile& hill)
{
	if (!pPlayer)
		return false;

	if (!pPlayer->pev)
		return false;

	if (!pPlayer->IsAlive())
		return false;

	Vector pos = pPlayer->pev->origin;

	if (pos.x < hill.m_vMins.x || pos.x > hill.m_vMaxs.x)
		return false;

	if (pos.y < hill.m_vMins.y || pos.y > hill.m_vMaxs.y)
		return false;

	if (pos.z < hill.m_vMins.z || pos.z > hill.m_vMaxs.z)
		return false;

	return true;
}

int AgKing::GetControllingTeam(bool& bContested)
{
	bContested = false;

	if (m_iActiveHill < 0 || m_iActiveHill >= (int)m_Hills.size())
		return KOTH_TEAM_NONE;

	bool bTeam1Inside = false;
	bool bTeam2Inside = false;

	AgKingFile& hill = m_Hills[m_iActiveHill];

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);

		if (!pPlayer || !pPlayer->pev || !pPlayer->IsAlive())
			continue;

		if (!IsPlayerInsideHill(pPlayer, hill))
			continue;

		if (FStrEq(pPlayer->m_szTeamName, KOTH_TEAM1_NAME))
			bTeam1Inside = true;
		else if (FStrEq(pPlayer->m_szTeamName, KOTH_TEAM2_NAME))
			bTeam2Inside = true;
	}

	if (bTeam1Inside && bTeam2Inside)
	{
		bContested = true;
		return 0;
	}

	if (bTeam1Inside)
		return KOTH_TEAM1;

	if (bTeam2Inside)
		return KOTH_TEAM2;

	return 0;
}

void AgKing::UpdateHillControl()
{
	bool bContested = false;
	int controllingTeam = GetControllingTeam(bContested);

	if (bContested)
	{
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "Hill contested!");
		return;
	}

	int points = (int)POINTS;
	if (points <= 0)
		points = 1;

	if (controllingTeam == KOTH_TEAM1)
	{
		s_iKOTHTeam1Score += points;
		GiveHillPointsToPlayers(KOTH_TEAM1_NAME, points);
	}
	else if (controllingTeam == KOTH_TEAM2)
	{
		s_iKOTHTeam2Score += points;
		GiveHillPointsToPlayers(KOTH_TEAM2_NAME, points);
	}
}

void AgKing::GiveHillPointsToPlayers(const char* pszTeamName, int points)
{
	if (!pszTeamName)
		return;

	if (points <= 0)
		return;

	if (m_iActiveHill < 0 || m_iActiveHill >= (int)m_Hills.size())
		return;

	AgKingFile& hill = m_Hills[m_iActiveHill];

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);

		if (!pPlayer || !pPlayer->pev || !pPlayer->IsAlive())
			continue;

		if (!FStrEq(pPlayer->m_szTeamName, pszTeamName))
			continue;

		if (!IsPlayerInsideHill(pPlayer, hill))
			continue;

		pPlayer->AddPoints(points, TRUE);
	}
}

void AgKing::SendTeamScores()
{
	if (m_iTeam1Score != s_iKOTHTeam1Score)
	{
		m_iTeam1Score = s_iKOTHTeam1Score;

		MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
			WRITE_STRING(KOTH_TEAM1_NAME);
			WRITE_SHORT(m_iTeam1Score);
			WRITE_SHORT(0);
		MESSAGE_END();
	}

	if (m_iTeam2Score != s_iKOTHTeam2Score)
	{
		m_iTeam2Score = s_iKOTHTeam2Score;

		MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
			WRITE_STRING(KOTH_TEAM2_NAME);
			WRITE_SHORT(m_iTeam2Score);
			WRITE_SHORT(0);
		MESSAGE_END();
	}
}

void AgKing::PlayerInitHud(CBasePlayer* pPlayer)
{
	ASSERT(NULL != pPlayer);
	if (!pPlayer)
		return;

	ASSERT(NULL != pPlayer->pev);
	if (!pPlayer->pev)
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, NULL, pPlayer->pev);
	WRITE_STRING(KOTH_TEAM1_NAME);
	WRITE_SHORT(s_iKOTHTeam1Score);
	WRITE_SHORT(0);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, NULL, pPlayer->pev);
	WRITE_STRING(KOTH_TEAM2_NAME);
	WRITE_SHORT(s_iKOTHTeam2Score);
	WRITE_SHORT(0);
	MESSAGE_END();

	if (m_iActiveHill >= 0 && m_iActiveHill < (int)m_Hills.size())
	{
		char szText[192];
		snprintf(szText, sizeof(szText), "Hill active: %s", m_Hills[m_iActiveHill].m_szName);
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, szText);
	}
}

void AgKing::SendScores(CBasePlayer* pPlayer)
{
	PlayerInitHud(pPlayer);
}

bool AgKing::ScoreLimit(void)
{
	return true;
	// stub for now
}

void AgKing::DrawBeamLine(const Vector& start, const Vector& end, int r, int g, int b)
{
	if (!m_iBeamSprite)
		return;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);

		WRITE_COORD(start.x);
		WRITE_COORD(start.y);
		WRITE_COORD(start.z);

		WRITE_COORD(end.x);
		WRITE_COORD(end.y);
		WRITE_COORD(end.z);

		WRITE_SHORT(m_iBeamSprite); // sprite index
		WRITE_BYTE(0);              // start frame
		WRITE_BYTE(0);              // frame rate
		WRITE_BYTE(12);             // life, tenths of a second
		WRITE_BYTE(8);              // width
		WRITE_BYTE(0);              // noise

		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		WRITE_BYTE(255);            // brightness
		WRITE_BYTE(0);              // speed
	MESSAGE_END();
}

void AgKing::RenderActiveHill()
{
#ifdef _DEBUG
	ALERT(at_console, "KING: RednerActiveHill called. active=%d count=%d sprite=%d time=%.2f next=%.2f\n", m_iActiveHill, m_iHillCount, m_iBeamSprite, gpGlobals->time, m_flNextRenderTime);
#endif

	if (m_iActiveHill < 0 || m_iActiveHill >= (int)m_Hills.size())
		return;

	if (gpGlobals->time < m_flNextRenderTime)
		return;

	m_flNextRenderTime = gpGlobals->time + 0.5f;

	AgKingFile& hill = m_Hills[m_iActiveHill];

	Vector mins = hill.m_vMins;
	Vector maxs = hill.m_vMaxs;

	Vector p000(mins.x, mins.y, mins.z);
	Vector p001(mins.x, mins.y, maxs.z);
	Vector p010(mins.x, maxs.y, mins.z);
	Vector p011(mins.x, maxs.y, maxs.z);

	Vector p100(maxs.x, mins.y, mins.z);
	Vector p101(maxs.x, mins.y, maxs.z);
	Vector p110(maxs.x, maxs.y, mins.z);
	Vector p111(maxs.x, maxs.y, maxs.z);

	bool bContested = false;
	int team = GetControllingTeam(bContested);

	int r = 255;
	int g = 220;
	int b = 64;

	if (bContested)
	{
		r = 255;
		g = 255;
		b = 255;
	}
	else if (team == KOTH_TEAM1)
	{
		r = 64;
		g = 128;
		b = 255; // blue mate bluuuuuuue
	}
	else if (team == KOTH_TEAM2)
	{
		r = 255;
		g = 64;
		b = 64; // red mate reeeeeeeeeed
	}

	// Bottom square
	DrawBeamLine(p000, p100, r, g, b);
	DrawBeamLine(p100, p110, r, g, b);
	DrawBeamLine(p110, p010, r, g, b);
	DrawBeamLine(p010, p000, r, g, b);

	// Top square
	DrawBeamLine(p001, p101, r, g, b);
	DrawBeamLine(p101, p111, r, g, b);
	DrawBeamLine(p111, p011, r, g, b);
	DrawBeamLine(p011, p001, r, g, b);

	// Vertical edges
	DrawBeamLine(p000, p001, r, g, b);
	DrawBeamLine(p100, p101, r, g, b);
	DrawBeamLine(p110, p111, r, g, b);
	DrawBeamLine(p010, p011, r, g, b);

	// Marker
	Vector bottom = hill.m_vOrigin;
	Vector top = hill.m_vOrigin;

	bottom.z = hill.m_vMins.z;
	top.z = hill.m_vMaxs.z + 128;

	DrawBeamLine(bottom, top, r, g, b);
}
