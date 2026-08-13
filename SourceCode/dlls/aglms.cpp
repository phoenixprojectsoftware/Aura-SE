//++ BulliT

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

#include "aggamerules.h"
#include "agglobal.h"
#include "aglms.h"
#include "agplayertargets.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int gmsgCountdown;

AgLMS::AgLMS()
{
    m_fMatchStart = 0.0;
    m_fNextCountdown = 0.0;
    m_Status = Waiting;
    m_bFinalStageActive = false;
    m_bFinalStageExpired = false;
    m_iPreviousAlivePlayerCount = 0;
}

AgLMS::~AgLMS()
{

}

void AgLMS::Think()
{
    if (!g_pGameRules)
        return;

    if (Playing == m_Status)
    {
        if (m_fNextCountdown > gpGlobals->time)
            return;
        m_fNextCountdown = gpGlobals->time + 0.5;

        if (g_pGameRules->IsTeamplay())
        {
            //Teams.
            AgStringSet setTeams;
            for (int i = 1; i <= gpGlobals->maxClients; i++)
            {
                CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
                if (pPlayerLoop)
                {
                    if (!pPlayerLoop->IsAlive())
                    {
                        pPlayerLoop->SetIngame(false); //Cant respawn
                        if (!pPlayerLoop->IsSpectator())
                        {
                            //Quake1 teleport splash around him.
                            MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
                            WRITE_BYTE(TE_TELEPORT);
                            WRITE_COORD(pPlayerLoop->pev->origin.x);
                            WRITE_COORD(pPlayerLoop->pev->origin.y);
                            WRITE_COORD(pPlayerLoop->pev->origin.z);
                            MESSAGE_END();

                            pPlayerLoop->Spectate_Start(false);
                            pPlayerLoop->Spectate_SetMode(OBS_IN_EYE);
                        }
                    }
                    else
                    {
                        setTeams.insert(pPlayerLoop->m_szTeamName);
                    }
                }
            }

            if (!(setTeams.size() > 1))
            {
                CancelFinalStageDeadline();
                m_iPreviousAlivePlayerCount = 0;

                m_sWinner = "";

                AgStringSet::iterator itrTeams = setTeams.begin();
                if (itrTeams != setTeams.end())
                {
                    m_sWinner = *itrTeams;
                }
                m_Status = Waiting;
            }
        }
        else
        {
            std::vector<CBasePlayer*> alivePlayers;

            for (int i = 1;
                i <= gpGlobals->maxClients;
                ++i)
            {
                CBasePlayer* pPlayerLoop =
                    AgPlayerByIndex(i);

                if (!pPlayerLoop)
                    continue;

                /*
                    Only players participating in the current LMS round
                    should count. This prevents waiting spectators or late
                    joiners from affecting FINAL_DUEL_QUOTA.
                */
                if (!pPlayerLoop->IsIngame())
                    continue;

                if (!pPlayerLoop->IsAlive())
                {
                    pPlayerLoop->SetIngame(false);

                    if (!pPlayerLoop->IsSpectator())
                    {
                        MESSAGE_BEGIN(
                            MSG_BROADCAST,
                            SVC_TEMPENTITY);

                        WRITE_BYTE(TE_TELEPORT);
                        WRITE_COORD(
                            pPlayerLoop->pev->origin.x);
                        WRITE_COORD(
                            pPlayerLoop->pev->origin.y);
                        WRITE_COORD(
                            pPlayerLoop->pev->origin.z);

                        MESSAGE_END();

                        pPlayerLoop->Spectate_Start(false);
                        pPlayerLoop->Spectate_SetMode(
                            OBS_IN_EYE);
                    }

                    continue;
                }

                alivePlayers.push_back(
                    pPlayerLoop);
            }

            /*
                Normal LMS resolution remains authoritative.
                The deadline never chooses or eliminates a winner.
            */
            if (alivePlayers.size() <= 1)
            {
                CancelFinalStageDeadline();

                m_sWinner = "";

                if (alivePlayers.size() == 1)
                {
                    m_sWinner =
                        alivePlayers.front()->GetName();
                }

                m_Status = Waiting;
                return;
            }

            UpdateFinalStageDeadline(alivePlayers);
            UpdateOvertimeWaypoints(alivePlayers);
        }
    }
    else
    {
        //We only update status once every second.
        if (m_fNextCountdown > gpGlobals->time)
            return;
        m_fNextCountdown = gpGlobals->time + 1.0;

        //Handle the status
        if (Waiting == m_Status)
        {
            if (g_pGameRules->IsTeamplay())
            {
                //Teams.
                AgStringSet setTeams;
                for (int i = 1; i <= gpGlobals->maxClients; i++)
                {
                    CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
                    if (pPlayerLoop && pPlayerLoop->m_bReady)
                    {
                        setTeams.insert(pPlayerLoop->m_szTeamName);
                    }
                }

                if (setTeams.size() > 1)
                {
                    m_Status = Countdown;
                    m_fMatchStart = gpGlobals->time + 8.0;
                    m_fNextCountdown = gpGlobals->time + 3.0;
                }
            }
            else
            {
                int iPlayers = 0;
                for (int i = 1; i <= gpGlobals->maxClients; i++)
                {
                    CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
                    if (pPlayerLoop && pPlayerLoop->m_bReady)
                    {
                        iPlayers++;
                    }
                }
                if (iPlayers > 1)
                {
                    m_Status = Countdown;
                    m_fMatchStart = gpGlobals->time + 8.0;
                    m_fNextCountdown = gpGlobals->time + 3.0;
                }
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
            AgSay(NULL, "Waiting for players to get ready!\n", &m_fNextSay, 2, 0.4, 0.5);
#else
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
            if (m_fMatchStart < gpGlobals->time)
            {
                //Clear out the map
                AgResetMap();

                CancelFinalStageDeadline();
                m_iPreviousAlivePlayerCount = 0;

                int i = 1;

                for (i = 1; i <= gpGlobals->maxClients; i++)
                {
                    CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
                    if (pPlayerLoop && pPlayerLoop->m_bReady)
                    {
                        pPlayerLoop->SetIngame(true);
                        g_pGameRules->m_ScoreCache.UpdateScore(pPlayerLoop);
                    }
                }

                m_Status = Spawning;
                m_sWinner = "";

                //Time to start playing.
                for (i = 1; i <= gpGlobals->maxClients; i++)
                {
                    CBasePlayer* pPlayerLoop = AgPlayerByIndex(i);
                    if (pPlayerLoop && pPlayerLoop->m_bReady)
                    {
                        if (pPlayerLoop->IsSpectator())
                        {
                            pPlayerLoop->Spectate_Stop();
                        }
                        else
                            pPlayerLoop->RespawnMatch();
                    }
                }

                m_Status = Playing;

                //Stop countdown
#ifndef AG_NO_CLIENT_DLL
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
                sprintf(szMatchStart, "Match will begin in %d seconds!\n", (int)(m_fMatchStart - gpGlobals->time));
                AgSay(NULL, szMatchStart, &m_fNextSay, 1, 0.3, 0.5);
#else
                MESSAGE_BEGIN(MSG_ALL, gmsgCountdown);
                WRITE_BYTE((int)(m_fMatchStart - gpGlobals->time));
                WRITE_BYTE(1);
                WRITE_STRING("");
                WRITE_STRING("");
                MESSAGE_END();
#endif
            }
        }
    }
}

void AgLMS::ClientConnected(CBasePlayer* pPlayer)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    //Set status
    pPlayer->SetIngame(false);
}


void AgLMS::ClientDisconnected(CBasePlayer* pPlayer)
{
    ASSERT(NULL != pPlayer);
    if (!pPlayer)
        return;
    ASSERT(NULL != pPlayer->pev);
    if (!pPlayer->pev)
        return;

    //Set status
    pPlayer->SetIngame(false);
}

//-- Martin Webrant

bool AgLMS::IsFinalStageActive() const
{
    return m_bFinalStageActive;
}

bool AgLMS::HasFinalStageDeadlineExpired() const
{
    return m_bFinalStageExpired;
}

void AgLMS::StartFinalStageDeadline(
    int iAliveCount)
{
    if (m_bFinalStageActive)
        return;

    const float flDuration =
        ag_lms_final_timelimit.value;

    if (flDuration <= 0.0f)
    {
        m_FinalStageDeadline.Cancel();

        m_bFinalStageActive = false;
        m_bFinalStageExpired = false;

        return;
    }

    m_bFinalStageActive = true;
    m_bFinalStageExpired = false;

    m_FinalStageDeadline.Start(
        flDuration,
        "Final stage");

    UTIL_ClientPrintAll(
        HUD_PRINTCENTER,
        UTIL_VarArgs(
            "%d players remain\nFinal-stage timer started",
            iAliveCount));

    UTIL_ClientPrintAll(
        HUD_PRINTTALK,
        UTIL_VarArgs(
            "* %d players remain. The LMS final-stage timer has started.\n",
            iAliveCount));
}

void AgLMS::CancelFinalStageDeadline()
{
    m_FinalStageDeadline.Cancel();

    m_bFinalStageActive = false;
    m_bFinalStageExpired = false;

    ClearOvertimeWaypoints();
}

void AgLMS::OnFinalStageDeadlineExpired()
{
    if (!m_bFinalStageActive ||
        m_bFinalStageExpired)
    {
        return;
    }

    m_bFinalStageExpired = true;
    m_bOvertimeWaypointsActive = true;
    m_iLastWaypointAliveCount = -1;

    UTIL_ClientPrintAll(
        HUD_PRINTCENTER,
        "Final-stage time expired\nRemaining players have been revealed");

    UTIL_ClientPrintAll(
        HUD_PRINTTALK,
        "* The LMS final-stage timer expired. All remaining players are now revealed.\n");
}

void AgLMS::UpdateFinalStageDeadline(
    const std::vector<CBasePlayer*>& alivePlayers)
{
    const int iAliveCount =
        static_cast<int>(
            alivePlayers.size());

    if (iAliveCount < 2)
    {
        CancelFinalStageDeadline();

        m_iPreviousAlivePlayerCount =
            iAliveCount;

        return;
    }

    if (!m_bFinalStageActive &&
        iAliveCount <= FINAL_DUEL_QUOTA)
    {
        StartFinalStageDeadline(
            iAliveCount);
    }

    /*
        Defensive recovery in case eliminated players somehow
        re-enter the active round.
    */
    if (m_bFinalStageActive &&
        iAliveCount > FINAL_DUEL_QUOTA)
    {
        CancelFinalStageDeadline();

        m_iPreviousAlivePlayerCount =
            iAliveCount;

        return;
    }

    if (m_bFinalStageActive &&
        !m_bFinalStageExpired)
    {
        m_FinalStageDeadline.Think();

        if (m_FinalStageDeadline.ConsumeExpiry())
        {
            OnFinalStageDeadlineExpired();
        }
    }

    m_iPreviousAlivePlayerCount =
        iAliveCount;
}

void AgLMS::UpdateOvertimeWaypoints(
    const std::vector<CBasePlayer*>& alivePlayers)
{
    if (!m_bOvertimeWaypointsActive ||
        !m_bFinalStageExpired)
    {
        return;
    }

    const int iAliveCount =
        static_cast<int>(
            alivePlayers.size());

    if (iAliveCount <= 1)
    {
        ClearOvertimeWaypoints();
        return;
    }

    if (iAliveCount ==
        m_iLastWaypointAliveCount)
    {
        return;
    }

    m_iLastWaypointAliveCount =
        iAliveCount;

    for (std::vector<CBasePlayer*>::const_iterator it =
        alivePlayers.begin();
        it != alivePlayers.end();
        ++it)
    {
        CBasePlayer* pRecipient = *it;

        if (!pRecipient)
            continue;

        AgSendPlayerTargets(
            pRecipient,
            alivePlayers);
    }
}

void AgLMS::ClearOvertimeWaypoints()
{
    const bool bWasActive =
        m_bOvertimeWaypointsActive ||
        m_iLastWaypointAliveCount >= 0;

    m_bOvertimeWaypointsActive = false;
    m_iLastWaypointAliveCount = -1;

    if (bWasActive)
    {
        AgClearAllPlayerTargets();
    }
}

void AgLMS::InitHUD(
    CBasePlayer* pPlayer)
{
    if (!pPlayer)
        return;

    if (!m_bOvertimeWaypointsActive ||
        !m_bFinalStageExpired ||
        !pPlayer->IsIngame() ||
        !pPlayer->IsAlive())
    {
        AgClearPlayerTargets(pPlayer);
        return;
    }

    std::vector<CBasePlayer*> alivePlayers;

    for (int i = 1;
        i <= gpGlobals->maxClients;
        ++i)
    {
        CBasePlayer* pAlive =
            AgPlayerByIndex(i);

        if (!pAlive ||
            !pAlive->IsIngame() ||
            !pAlive->IsAlive())
        {
            continue;
        }

        alivePlayers.push_back(pAlive);
    }

    AgSendPlayerTargets(
        pPlayer,
        alivePlayers);
}
