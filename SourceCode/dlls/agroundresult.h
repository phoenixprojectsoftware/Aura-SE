#ifndef AGROUNDRESULT_H
#define AGROUNDRESULT_H

#include <vector>

class CBasePlayer;

enum AgRoundTimeoutOutcome
{
	AG_ROUND_TIMEOUT_DRAW = 0,
	AG_ROUND_TIMEOUT_WINNER
};

struct AgRoundTimeoutResult
{
	AgRoundTimeoutOutcome m_Outcome;
	CBasePlayer* m_pWinner;

	AgRoundTimeoutResult()
		: m_Outcome(
			AG_ROUND_TIMEOUT_DRAW),
		m_pWinner(NULL)
	{}

	explicit AgRoundTimeoutResult(
		CBasePlayer* pWinner)
		: m_Outcome(
			pWinner ?
			AG_ROUND_TIMEOUT_WINNER :
			AG_ROUND_TIMEOUT_DRAW),
		m_pWinner(pWinner)
	{}

	bool HasWinner() const
	{
		return
			m_Outcome ==
			AG_ROUND_TIMEOUT_WINNER &&
			m_pWinner != NULL;
	}
};

AgRoundTimeoutResult AgChooseRoundTimeoutWinner(
	const std::vector<CBasePlayer*>& players);

#endif
