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

#pragma once

#include <vector>

enum KingTeams
{
	KOTH_TEAM_NONE = 0,
	KOTH_TEAM1,
	KOTH_TEAM2
};

#define KOTH_TEAM1_NAME "blue"
#define KOTH_TEAM2_NAME "red"

struct AgKingFile
{
	char m_szName[64];
	Vector m_vOrigin;
	Vector m_vSize;
	Vector m_vMins;
	Vector m_vMaxs;
};

class AgKing
{
public:
	AgKing();
	virtual ~AgKing();

	void Precache();
	void Think();
	void Load();
	void Reset();

	void PlayerInitHud(CBasePlayer* pPlayer);
	void SendScores(CBasePlayer* pPlayer);

	bool ScoreLimit(void);

private:
	void RotateHill();
	void SetActiveHill(int index);
	void UpdateHillControl();

	bool IsPlayerInsideHill(CBasePlayer* pPlayer, const AgKingFile& hill);
	int GetControllingTeam(bool& contested);

	void GiveHillPointsToPlayers(const char* pszTeamName, int points);
	void SendTeamScores();
	
	int m_iBeamSprite;
	float m_flNextRenderTime;

	void RenderActiveHill();
	void DrawBeamLine(const Vector& start, const Vector& end, int r, int g, int b);

	std::vector<AgKingFile> m_Hills;

	int m_iHillCount;
	int m_iActiveHill;
	int m_iTeam1Score;
	int m_iTeam2Score;

	float m_flNextThink;
	float m_flNextScoreTime;
	float m_flNextHillChange;

	bool m_bLoaded;
};
