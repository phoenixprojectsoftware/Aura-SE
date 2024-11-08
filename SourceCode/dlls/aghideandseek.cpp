// Sabian Roberts for TPPS! \\// 12-04-2023

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

#include "aggamerules.h"
#include "agglobal.h"
//#include "agarena.h"
#include "aghideandseek.h"

#include "algo.h"

extern int gmsgCountdown;

AgHideandseek::AgHideandseek()
{
	m_fMatchStart = 0.0;
	m_fNextCountdown = 0.0;
	m_fNextSay = 0.0;
	m_Hider = NULL;
	m_Seeker = NULL;
	m_Status = Waiting;
	// This section is based on AgArena which tells the client to spectate if
	// no one has joined yet
}

AgHideandseek::~AgHideandseek()
{

}

void AgHideandseek::Think()
{
	if (!g_pGameRules)
		return;

	if (Playing == m_Status)
	{
		CBasePlayer* pHider = GetHider();
		CBasePlayer* pSeeker = GetSeeker();

		if (!pSeeker || pSeeker && !pSeeker->IsAlive() || !pHider || pHider && !pHider->IsAlive())
		{
			m_Status = PlayerDied;
			m_fNextCountdown = gpGlobals->time + 3.0; //effect plays for 3 secs

			if (pHider)
				pHider->SetIngame(false);//Hider can't respawn after being killed/found
			if (pSeeker)
				pSeeker->SetIngame(true);
		}
	}
	else
	{
		if (m_fNextCountdown > gpGlobals->time)
			return;
		m_fNextCountdown = gpGlobals->time + 1.0;

		if (Waiting == m_Status)
		{
			//Get new hider
			if (!GetHider())
			{
				if (0 != m_lstWaitList.size())
				{
					m_Hider = AgPlayerByIndex(m_lstWaitList.front());
					m_lstWaitList.pop_front();
				}
			}

			if (!GetSeeker())
			{
				//Get one from top of list.
				if (0 != m_lstWaitList.size())
				{
					m_Seeker = AgPlayerByIndex(m_lstWaitList.front());
					m_lstWaitList.pop_front();
				}
			}

			if (GetHider() && GetSeeker())
			{
				m_Status = Countdown;
				m_fMatchStart = gpGlobals->time + 5.0;
				m_fNextCountdown = gpGlobals->time + 2.0;
			}



            //Write waiting message
#ifdef AG_NO_CLIENT_DLL
            if (0 != m_sWinner.size())
            {
                AgString s;
                s = "Last match won by " + m_sWinner;
                AgSay(NULL, s.c_str(), NULL, 10, 0.4, 0.1, 2);
                m_sWinner = "";
            }
            AgSay(NULL, "Welcome to Hide and Seek!\n", &m_fNextSay, 2, 0.4, 0.5);
#else
            //AgSay(NULL, "Welcome to Hide and Seek!\n", &m_fNextSay, 2, 0.4, 0.5);
            MESSAGE_BEGIN(MSG_ALL, gmsgCountdown);
            WRITE_BYTE(50);
            WRITE_BYTE(1);
            WRITE_STRING(m_sWinner.c_str());
            WRITE_STRING("");
            MESSAGE_END();
#endif
        }
        else if (Countdown == m_Status)
        {
            if (!GetHider() || !GetSeeker())
            {
                m_Status = Waiting; //Someone left in middle of countdown. Go back to waiting.
                return;
            }

            if (m_fMatchStart < gpGlobals->time)
            {
                //Clear out the map
                AgResetMap();

                GetHider()->SetIngame(true);
                GetSeeker()->SetIngame(true);
                g_pGameRules->m_ScoreCache.UpdateScore(GetHider());
                g_pGameRules->m_ScoreCache.UpdateScore(GetSeeker());

                m_Status = Spawning;
                m_sWinner = "";

                //Time to start playing.
                if (GetHider()->IsSpectator())
                {
                    GetHider()->Spectate_Stop();
                }
                else
                    GetHider()->RespawnMatch();

                if (GetSeeker()->IsSpectator())
                {
                    GetSeeker()->Spectate_Stop();
                }
                else
                    GetSeeker()->RespawnMatch();

                m_Status = Playing;

#ifndef AG_NO_CLIENT_DLL
                //Stop countdown
                MESSAGE_BEGIN(MSG_ALL, gmsgCountdown);
                WRITE_BYTE(-1);
                WRITE_BYTE(0);
                WRITE_STRING("");
                WRITE_STRING("");
                MESSAGE_END();
#endif
            }
            else
            {
                //Write countdown message.
#ifdef AG_NO_CLIENT_DLL
                char szMatchStart[128];
                sprintf(szMatchStart, "Match %s vs %s in %d seconds!\n", GetHider()->GetName(), GetSeeker()->GetName(), (int)(m_fMatchStart - gpGlobals->time));
                AgSay(NULL, szMatchStart, &m_fNextSay, 1, 0.3, 0.5);
#else
                MESSAGE_BEGIN(MSG_ALL, gmsgCountdown);
                WRITE_BYTE((int)(m_fMatchStart - gpGlobals->time));
                WRITE_BYTE(1);
                WRITE_STRING(GetHider()->GetName());
                WRITE_STRING(GetSeeker()->GetName());
                MESSAGE_END();
#endif
            }
        }
        else if (PlayerDied == m_Status)
        {
            CBasePlayer* pHider = GetHider();
            CBasePlayer* pSeeker = GetSeeker();

            m_Status = Waiting;
            m_sWinner = "";

            if (pHider && !pHider->IsAlive())
            {
                if (!pHider->IsSpectator())
                {
                    pHider->Spectate_Start(false);
                    pHider->Spectate_Follow(m_Seeker, OBS_IN_EYE);
                }
                m_lstWaitList.push_back(pHider->entindex());
                pHider->SetIngame(false);
                m_Hider = NULL;

                if (pSeeker)
                {
                    pSeeker->SetIngame(false);
                    if (pSeeker->IsAlive())
                        m_sWinner = pSeeker->GetName();
                }
            }

            if (pSeeker && !pSeeker->IsAlive())
            {
                if (!pSeeker->IsSpectator())
                {
                    pSeeker->Spectate_Start(false);
                    pSeeker->Spectate_Follow(m_Hider, OBS_IN_EYE);
                }
                m_lstWaitList.push_back(pSeeker->entindex());
                pSeeker->SetIngame(false);
                m_Seeker = NULL;
                if (pHider)
                {
                    pHider->SetIngame(false);
                    if (pHider->IsAlive())
                        m_sWinner = pHider->GetName();
                }
            }

            //Stop sounds.
            for (int i = 1; i <= gpGlobals->maxClients; i++)
            {
                CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
                if (pPlayerLoop)
                {
                    CLIENT_COMMAND(pPlayerLoop->edict(), "stopsound\n");
                }
            }
        }
    }
}

void AgHideandseek::Add(CBasePlayer* pPlayer)
{
    if (GetHider() == pPlayer || GetSeeker() == pPlayer)
        return;

    if (0 == m_lstWaitList.size())
    {
        m_lstWaitList.push_back(pPlayer->entindex());
    }
    else
    {
        AgWaitList::iterator itrWaitlist = ALGO_H::find(m_lstWaitList.begin(), m_lstWaitList.end(), pPlayer->entindex());
        if (itrWaitlist == m_lstWaitList.end())
            m_lstWaitList.push_back(pPlayer->entindex());
    }
}

void AgHideandseek::Remove(CBasePlayer* pPlayer)
{
    if (GetHider() == pPlayer)
    {
        m_Hider = NULL;
    }
    else if (GetSeeker() == pPlayer)
    {
        m_Seeker = NULL;
    }
    else if (0 != m_lstWaitList.size())
    {
        AgWaitList::iterator itrWaitlist = ALGO_H::find(m_lstWaitList.begin(), m_lstWaitList.end(), pPlayer->entindex());
        if (itrWaitlist == m_lstWaitList.end())
            m_lstWaitList.erase(itrWaitlist);
        //m_lstWaitList.remove(pPlayer->entindex());
    }
}

void AgHideandseek::Ready(CBasePlayer* pPlayer)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    if (Countdown != m_Status)
    {
        Add(pPlayer);
        ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Changed mode to READY.\n");
    }
    else
    {
        ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Can not change ready state at this point.\n");
    }
}

void AgHideandseek::NotReady(CBasePlayer* pPlayer)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    if (Countdown != m_Status)
    {
        pPlayer->SetIngame(false);
        if (!pPlayer->IsSpectator())
        {
            pPlayer->Spectate_Start(false);
        }
        Remove(pPlayer);
        ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Changed mode to NOT READY.\n");
    }
    else
    {
        ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Can not change ready state at this point.\n");
    }
}

void AgHideandseek::ClientConnected(CBasePlayer* pPlayer)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    //Set status
    pPlayer->SetIngame(false);

    if (!pPlayer->IsProxy())
        Add(pPlayer);
}


void AgHideandseek::ClientDisconnected(CBasePlayer* pPlayer)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    //Set status
    pPlayer->SetIngame(false);

    //Just blank if active in arena. 
    //This will put the status into waiting mode within a second.
    if (!pPlayer->IsProxy())
        Remove(pPlayer);
}