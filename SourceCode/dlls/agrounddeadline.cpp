/****
*
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Round Deadline
*
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "agrounddeadline.h"

#include <cmath>

AgRoundDeadline::AgRoundDeadline()
	: m_bActive(false),
	m_bExpired(false),
	m_flEndTime(0.0f),
	m_iLastAnnouncedSecond(-1)
{}

void AgRoundDeadline::Start(float flDuration, const char* pszLabel)
{
	Cancel();

	if (flDuration <= 0.0f)
		return;

	m_sLabel = pszLabel && pszLabel[0] ? pszLabel : "Round";

	m_bActive = true;
	m_bExpired = false;

	m_flEndTime = gpGlobals->time + flDuration;

	m_iLastAnnouncedSecond = -1;
}

void AgRoundDeadline::Cancel()
{
	m_bActive = false;
	m_bExpired = false;

	m_flEndTime = 0.0f;
	m_iLastAnnouncedSecond = -1;
}

bool AgRoundDeadline::IsActive() const
{
	return m_bActive;
}

bool AgRoundDeadline::HasExpired() const
{
	return m_bExpired;
}

float AgRoundDeadline::GetEndTime() const
{
	return m_flEndTime;
}

int AgRoundDeadline::GetSecondsRemaining() const
{
	if (!m_bActive)
		return 0;

	const float flRemaining = m_flEndTime - gpGlobals->time;

	if (flRemaining <= 0.0f)
	return 0;

	return static_cast<int>(ceil(flRemaining));
}

bool AgRoundDeadline::ShouldAnnounceSecond(
	int iSecondsRemaining) const
{
	if (iSecondsRemaining == 60)
		return true;

	if (iSecondsRemaining == 30)
		return true;

	if (iSecondsRemaining == 15)
		return true;

	if (iSecondsRemaining == 10)
		return true;

	if (iSecondsRemaining >= 1 &&
		iSecondsRemaining <= 5)
	{
		return true;
	}

	return false;
}

void AgRoundDeadline::Announce(
	int iSecondsRemaining)
{
	if (iSecondsRemaining <= 0)
		return;

	UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("%s ends in %d second%s", m_sLabel.c_str(), iSecondsRemaining, iSecondsRemaining == 1 ? "" : "s"));
}

void AgRoundDeadline::Think()
{
	if (!m_bActive ||
		m_bExpired)
	{
		return;
	}

	const int iSecondsRemaining =
		GetSecondsRemaining();

	if (iSecondsRemaining <= 0)
	{
		m_bActive = false;
		m_bExpired = true;

		return;
	}

	if (iSecondsRemaining ==
		m_iLastAnnouncedSecond)
	{
		return;
	}

	m_iLastAnnouncedSecond =
		iSecondsRemaining;

	if (ShouldAnnounceSecond(
		iSecondsRemaining))
	{
		Announce(
			iSecondsRemaining);
	}
}

bool AgRoundDeadline::ConsumeExpiry()
{
	if (!m_bExpired)
		return false;

	m_bExpired = false;
	return true;
}
