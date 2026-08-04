#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "agroundresult.h"

#include <vector>

struct AgRoundPlayerScore
{
	CBasePlayer* pPlayer;

	float flCombined;
	float flHealth;
	float flArmour;
};

static AgRoundPlayerScore
AgGetRoundPlayerScore(
	CBasePlayer* pPlayer)
{
	AgRoundPlayerScore score;

	score.pPlayer = pPlayer;
	score.flHealth = 0.0f;
	score.flArmour = 0.0f;
	score.flCombined = 0.0f;

	if (!pPlayer || !pPlayer->pev)
		return score;

	score.flHealth =
		max(0.0f, pPlayer->pev->health);

	score.flArmour =
		max(0.0f, pPlayer->pev->armorvalue);

	score.flCombined =
		score.flHealth +
		score.flArmour;

	return score;
}

static int AgCompareRoundPlayerScores(
	const AgRoundPlayerScore& left,
	const AgRoundPlayerScore& right)
{
	const float flTolerance = 0.01f;

	if (left.flCombined >
		right.flCombined + flTolerance)
	{
		return 1;
	}

	if (right.flCombined >
		left.flCombined + flTolerance)
	{
		return -1;
	}

	if (left.flHealth >
		right.flHealth + flTolerance)
	{
		return 1;
	}

	if (right.flHealth >
		left.flHealth + flTolerance)
	{
		return -1;
	}

	if (left.flArmour >
		right.flArmour + flTolerance)
	{
		return 1;
	}

	if (right.flArmour >
		left.flArmour + flTolerance)
	{
		return -1;
	}

	return 0;
}

AgRoundTimeoutResult
AgChooseRoundTimeoutWinner(
	const std::vector<CBasePlayer*>& players)
{
	CBasePlayer* pBestPlayer = NULL;
	AgRoundPlayerScore bestScore;

	bool bHaveBestPlayer = false;
	bool bTied = false;

	for (std::vector<CBasePlayer*>::const_iterator it =
		players.begin();
		it != players.end();
		++it)
	{
		CBasePlayer* pPlayer = *it;

		if (!pPlayer ||
			!pPlayer->pev)
		{
			continue;
		}

		if (!pPlayer->IsAlive())
			continue;

		const AgRoundPlayerScore currentScore =
			AgGetRoundPlayerScore(
				pPlayer);

		if (!bHaveBestPlayer)
		{
			bestScore = currentScore;
			pBestPlayer = pPlayer;
			bHaveBestPlayer = true;
			bTied = false;

			continue;
		}

		const int iComparison =
			AgCompareRoundPlayerScores(
				currentScore,
				bestScore);

		if (iComparison > 0)
		{
			bestScore = currentScore;
			pBestPlayer = pPlayer;
			bTied = false;
		}
		else if (iComparison == 0)
		{
			bTied = true;
		}
	}

	if (!bHaveBestPlayer ||
		bTied)
	{
		return AgRoundTimeoutResult();
	}

	return AgRoundTimeoutResult(
		pBestPlayer);
}
