#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "agplayertargets.h"

extern int gmsgPlayerTargets;

void AgSendPlayerTargets(CBasePlayer* pRecipient, const std::vector<CBasePlayer*>& players)
{
	if (!pRecipient || !pRecipient->edict())
	{
		return;
	}

	std::vector<int> targets;

	for (std::vector<CBasePlayer*>::const_iterator it = players.begin(); it != players.end(); ++it)
	{
		CBasePlayer* pTarget = *it;

		if (!pTarget || !pTarget->edict())
			continue;

		// never reveal the recipient to themselves.
		if (pTarget == pRecipient)
			continue;

		const int iPlayerIndex = pTarget->entindex();

		if (iPlayerIndex <= 0 || iPlayerIndex > gpGlobals->maxClients)
		{
			continue;
		}

		targets.push_back(iPlayerIndex);
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgPlayerTargets, NULL, pRecipient->edict());
	WRITE_BYTE(static_cast<int>(targets.size()));
	for (std::vector<int>::const_iterator it = targets.begin(); it != targets.end(); ++it)
	{
		WRITE_BYTE(*it);
	}

	MESSAGE_END();
}

void AgClearPlayerTargets(CBasePlayer* pRecipient)
{
	if (!pRecipient || !pRecipient->edict())
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgPlayerTargets, NULL, pRecipient->edict());
	WRITE_BYTE(0);
	MESSAGE_END();
}

void AgClearAllPlayerTargets()
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer* pPlayer = AgPlayerByIndex(i);

		if (!pPlayer)
			continue;

		AgClearPlayerTargets(pPlayer);
	}
}
