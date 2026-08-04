// aglms.h: interface for the AgLMS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__AG_LMS_H__)
#define __AG_LMS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef FINAL_DUEL_QUOTA
#define FINAL_DUEL_QUOTA 2
#endif

#if FINAL_DUEL_QUOTA < 2
#error FINAL_DUEL_QUOTA must be at least 2.
#endif

#include "agrounddeadline.h"
#include "agroundresult.h"

class AgLMS
{
    enum LMSStatus { Waiting, Countdown, Spawning, Playing };
    LMSStatus m_Status;
    float m_fNextCountdown;
    float m_fMatchStart;
    float m_fNextSay;
    AgString m_sWinner;

    AgRoundDeadline m_FinalDuelDeadline;

    AgRoundDeadline m_FinalStageDeadline;

    bool m_bFinalStageActive;
    bool m_bFinalStageExpired;

    int m_iPreviousAlivePlayerCount;

    void UpdateFinalStageDeadline(
        const std::vector<CBasePlayer*>& alivePlayers);

    void StartFinalStageDeadline(int iAliveCount);
    void CancelFinalStageDeadline();

    void OnFinalStageDeadlineExpired();

    bool IsFinalStageActive() const;
    bool HasFinalStageDeadlineExpired() const;

public:
    AgLMS();
    virtual ~AgLMS();

    void Think();

    void ClientDisconnected(CBasePlayer* pPlayer);
    void ClientConnected(CBasePlayer* pPlayer);

    bool CanTakeDamage();
};

inline bool AgLMS::CanTakeDamage()
{
    return Playing == m_Status;
}


#endif // !defined(__AG_LMS_H__)
