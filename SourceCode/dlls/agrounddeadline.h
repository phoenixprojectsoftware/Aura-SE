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

#pragma once

class AgRoundDeadline
{
public:
	AgRoundDeadline();

	void Start(float flDuration, const char* pszLabel = "Round");
	void Cancel();

	void Think();

	bool IsActive() const;
	bool HasExpired() const;

	float GetEndTime() const;
	int GetSecondsRemaining() const;

	bool ConsumeExpiry();

	AgString m_sLabel;

private:
	bool ShouldAnnounceSecond(int iSecondsRemaining) const;
	void Announce(int iSecondsRemaining);

private:
	bool m_bActive;
	bool m_bExpired;

	float m_flEndTime;
	int m_iLastAnnouncedSecond;
};
