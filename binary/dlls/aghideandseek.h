// agarena.h: interface for the AgHideandseek class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_AGARENA_H__1929C55A_3034_4C89_8398_1F8243B83499__INCLUDED_)
#define AFX_AGARENA_H__1929C55A_3034_4C89_8398_1F8243B83499__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AgHideandseek
{
    typedef list<int> AgWaitList;
    enum ArenaStatus { Waiting, Countdown, Spawning, Playing, PlayerDied };

    AgWaitList  m_lstWaitList;
    ArenaStatus m_Status;

    float m_fNextCountdown;
    float m_fMatchStart;
    float m_fNextSay;

    EHANDLE m_Hider;
    EHANDLE m_Seeker;

    AgString m_sWinner;

    CBasePlayer* GetHider();
    CBasePlayer* GetSeeker();

    void Add(CBasePlayer* pPlayer);
    void Remove(CBasePlayer* pPlayer);

public:
    AgHideandseek();
    virtual ~AgHideandseek();

    void Think();

    void Ready(CBasePlayer* pPlayer);
    void NotReady(CBasePlayer* pPlayer);

    void ClientDisconnected(CBasePlayer* pPlayer);
    void ClientConnected(CBasePlayer* pPlayer);

    bool CanTakeDamage();
    bool CanHaveItem();

};


inline  CBasePlayer* AgHideandseek::GetHider()
{
    if (m_Hider)
        return (CBasePlayer*)(CBaseEntity*)m_Hider;
    else
        return NULL;
};

inline CBasePlayer* AgHideandseek::GetSeeker()
{
    if (m_Seeker)
        return (CBasePlayer*)(CBaseEntity*)m_Seeker;
    else
        return NULL;
};

inline bool AgHideandseek::CanTakeDamage()
{
    return Playing == m_Status || PlayerDied == m_Status;
}

inline bool AgHideandseek::CanHaveItem()
{
    return Spawning == m_Status;
}


//#endif // !defined(AFX_AGARENA_H__1929C55A_3034_4C89_8398_1F8243B83499__INCLUDED_)
