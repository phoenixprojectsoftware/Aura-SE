#include <algorithm>

#include "extdll.h"
#include "util.h"

#include "cbase.h"
#include "player.h"
#include "game.h"
#include "spawnchooser.h"

extern std::vector<CBaseEntity*> g_spawnPoints;
extern std::vector<CBaseEntity*> g_spawnHistory;

extern void SetupVisibility(edict_t* pViewEntity, edict_t* pClient, unsigned char** pvs, unsigned char** pas);

inline int FNullEnt(CBaseEntity* ent) { return (ent == NULL) || FNullEnt(ent->edict()); }

std::map<int, std::string> spawnSystemName = {
    {0, std::string("Classic")},
    {1, std::string("Random")},
    {2, std::string("Far")},
	// To make it "Position-aware" we have to move the Settings HUD a bit to the left in the clientside, because it's too much text
    {3, std::string("PA")},
};

// Checks if the spot is clear of players
BOOL IsSpawnPointValid(CBasePlayer* pPlayer, CBaseEntity* pSpot)
{
	CBaseEntity* ent = NULL;

	if (!pSpot->IsTriggered(pPlayer))
		return FALSE;

	while ((ent = UTIL_FindEntityInSphere(ent, pSpot->pev->origin, 128)) != NULL)
	{
		// If ent is a client, don't spawn on them
		if (ent->IsPlayer() && ent != pPlayer)
			return FALSE;
	}

	return TRUE;
}

CBaseEntity* CSpawnChooser::GetClassicSpawnPoint()
{
	if (!m_spawningPlayer)
	{
		ALERT(at_warning, "Failed to get a classic spawn point, spawning player info is missing! Relying on the random spawn system to get the next spawn spot...\n");

		if (g_spawnPoints.empty())
		{
			ALERT(at_error, "No deathmatch spawn spots detected! Can't rely on the random spawn system...\n");
			return nullptr;
		}
		return GetRandomSpawnPoint();
	}

	CBaseEntity* pSpot = m_lastSpawn;

	// Randomize the start spot
	for (int i = RANDOM_LONG(1, 5); i > 0; i--)
		pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch");

	if (FNullEnt(pSpot))  // skip over the null point
		pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch");

	if (FNullEnt(pSpot) && singleplayer.value > 0.0f)
		pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_start");

	CBaseEntity* pFirstSpot = pSpot;

	do
	{
		if (pSpot)
		{
			// check if pSpot is valid
			if (IsSpawnPointValid(m_spawningPlayer, pSpot))
			{
				if (pSpot->pev->origin == Vector(0, 0, 0))
				{
					pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch");
					continue;
				}

				return pSpot;
			}
		}
		// increment pSpot
		pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch");
	} while (pSpot != pFirstSpot); // loop if we're not back to the start

	return nullptr;
}

// Get one of the spawn points that is furthest from enemies
// Take the furthest 25% of all spawn points and choose of those randomly,
// and if the 25% is less than 3 spawns, then make it 3 spawns minimum,
// or make it the total spawn count if there are less than 3 spawns in the map
CBaseEntity* CSpawnChooser::GetFarSpawnPoint()
{
	// TODO: think about how to refactor this error handling code
	if (g_spawnPoints.empty())
	{
		ALERT(at_warning, "No deathmatch spawn spots detected! Relying on the classic spawn system to get the next spawn spot...\n");
		return GetClassicSpawnPoint();
	}
	if (!m_spawningPlayer)
	{
		ALERT(at_warning, "Failed to get a far spawn point, spawning player info is missing! Relying on the random spawn system to get the next spawn spot...\n");
		return GetRandomSpawnPoint();
	}
	std::vector<CBasePlayer*> enemies = m_spawningPlayer->GetPlayingEnemies();

	if (enemies.empty())
	{
		ALERT(at_aiconsole, "No enemy found. Selecting some random spawn without distance checks...\n");
		return GetRandomSpawnPoint();
	}
	ALERT(at_aiconsole, "Enemies: %d\n", enemies.size());


	std::map<CBaseEntity*, float> distancesFromEnemy;
	std::vector<float> distancesToSort;

	std::vector<int> spotsToDiscard = GetRecentlyUsedSpots();
	std::vector<CBaseEntity*> filteredSpawnPoints;

	auto idx = 0;
	for (const auto spawnPoint : g_spawnPoints)
	{
		if (find(spotsToDiscard.begin(), spotsToDiscard.end(), idx) != spotsToDiscard.end())
			ALERT(at_aiconsole, "Ignoring spot #%d, it was used recently\n", idx + 1);
		else
			filteredSpawnPoints.push_back(spawnPoint);

		idx++;
	}
	const auto numberOfFarSpawns = V_max((unsigned long)std::lround(filteredSpawnPoints.size() * ag_spawn_far_spots.value), V_min(filteredSpawnPoints.size(), 3));

	ALERT(at_aiconsole, "Number of far spawns to choose a random one from: %d\n", numberOfFarSpawns);

	// Get shortest distances from enemies to spawns
	for (const auto spawnPoint : filteredSpawnPoints)
	{
		auto closestEnemyDistance = 99999.0f;

		// Let's see what are the distances from enemies to this spawn point
		// We wanna get the spawns that have the furthest distance to the closest enemy
		for (const auto enemy : enemies)
		{
			const auto dist = enemy->DistanceTo(spawnPoint);
			if (dist < closestEnemyDistance)
			{
				closestEnemyDistance = dist;
			}
		}
		distancesFromEnemy.insert({ spawnPoint, closestEnemyDistance });
		distancesToSort.push_back(closestEnemyDistance);
	}
	std::sort(distancesToSort.begin(), distancesToSort.end(), std::greater<float>());

	for (unsigned long i = 0; i < numberOfFarSpawns; i++)
	{
		ALERT(at_aiconsole, "dist[%d]: %.2f\n", i, distancesToSort[i]);
	}

	// The distances are now sorted, first one is the longest and last one the shortest,
	// so now we just take a random one out of the first N spawns
	const auto selectedDistance = distancesToSort[RANDOM_LONG(0, numberOfFarSpawns - 1)];
	ALERT(at_aiconsole, "Selected distance: %.2f\n", selectedDistance);

	CBaseEntity* pSpot = nullptr;

	// Figure out what spawn spot corresponds to that distance, because we know what
	// distance we want it to be but we don't know the spawn spot, as it seemed easier
	// to me sorting the vector than the map, so now we gotta do this step
	for (const auto& entry : distancesFromEnemy)
	{
		if (entry.second == selectedDistance)
		{
			pSpot = entry.first;
			break;
		}
	}
	return pSpot;
}

// Get a random spawn point
// It might even be the same where you spawned last time
CBaseEntity* CSpawnChooser::GetRandomSpawnPoint()
{
	if (g_spawnPoints.empty())
	{
		ALERT(at_warning, "No deathmatch spawn spots detected! Relying on the classic spawn system to get the next spawn spot...\n");
		// If this gets to find a spot then there's some bug, the vector containing spawns shouldn't be empty
		return GetClassicSpawnPoint();
	}
	return g_spawnPoints[RANDOM_LONG(0, g_spawnPoints.size() - 1)];
}

// Get a random spawn point with increased chances that it will be a spawn far from some opponent,
// that is, a spot that is not visible nor audible by any opponent
// TODO: take into account teammates' distances to spawn points too
// TODO: make another spawn system that is also aware of players' inventory (how strong they are, potentially) and nearby items/weapons
CBaseEntity* CSpawnChooser::GetPositionAwareSpawnPoint()
{
	if (g_spawnPoints.empty())
	{
		ALERT(at_warning, "No deathmatch spawn spots detected! Relying on the classic spawn system to get the next spawn spot...\n");
		return GetClassicSpawnPoint();
	}

	if (!m_spawningPlayer)
	{
		ALERT(at_warning, "Cannot set up spawn point awareness, spawning player info is missing! Gonna relay on the random spawn system\n");
		return GetRandomSpawnPoint();
	}
	SetupSpotAwareness();

	std::vector<int> spotsToSelect;
	CBaseEntity* pSpot = nullptr;
	auto total = ag_spawn_pa_safe_chance.value + ag_spawn_pa_audible_chance.value + ag_spawn_pa_visible_chance.value;
	auto chance = RANDOM_FLOAT(0, total);
	auto safeChanceLowerBound = ag_spawn_pa_audible_chance.value + ag_spawn_pa_visible_chance.value;

	ALERT(at_aiconsole, "Chance number: %.2f out of %.2f\n", chance, total);
	if (chance > safeChanceLowerBound)
	{
		// By default high chance of spawning in a spot that is safe (neither visible nor audible from an enemy position)
		ALERT(at_aiconsole, "Selecting a safe spot\n");

		if (m_safeSpots.empty())
			ALERT(at_aiconsole, "No safe spots found, so we're falling back to the next category\n");

		spotsToSelect = SelectSpotsFallingBack({ m_safeSpots, m_barelyAudibleSpots, m_audibleSpots, m_visibleSpots });
	}
	else if (chance > ag_spawn_pa_visible_chance.value)
	{
		// By default less chance of spawning in a spot that might be audible from some enemy position
		// Note: the game actually considers lots of world leafs to be potentially audible...
		// So it's not that small of a chance because anyways most of these spots will probably
		// be fine and should have been considered good spots. I guess it depends on how well
		// the map was compiled and whether the mapper used proper skip/hint faces to limit PVS/PAS
		ALERT(at_aiconsole, "Selecting an audible spot\n");

		if (m_audibleSpots.empty())
			ALERT(at_aiconsole, "No audible spots found, so we're falling back to the next category\n");

		spotsToSelect = SelectSpotsFallingBack({ m_audibleSpots, m_visibleSpots, m_barelyAudibleSpots, m_safeSpots });
	}
	else
	{
		// By default slim chance for a visible spot to be selected
		ALERT(at_aiconsole, "Selecting an visible spot\n");

		if (m_visibleSpots.empty())
			ALERT(at_aiconsole, "No visible spots found, so we're falling back to the next category\n");

		spotsToSelect = SelectSpotsFallingBack({ m_visibleSpots, m_audibleSpots, m_barelyAudibleSpots, m_safeSpots });
	}

	// TODO: avoid spawning at a spot where a player is already there? should be a cvar?
	//       telefrag them right away or only if there's no free spot?

	int spotIdx;
	if (!spotsToSelect.empty())
		spotIdx = spotsToSelect[RANDOM_LONG(0, spotsToSelect.size() - 1)];
	else
	{
		ALERT(at_error, "No spot found, gotta select a random spot then (this shouldn't have happened, review code)\n");
		return GetRandomSpawnPoint();
	}
	pSpot = g_spawnPoints[spotIdx];

	ALERT(at_aiconsole, "%s spawning at spot #%d\n", m_spawningPlayer->GetName(), spotIdx + 1);

	return pSpot;
}

// Warning: this method is based on the argument being ordered correctly, so it will try to get
// the spots from the 1st list, then from the 2nd, etc. until it finds one that is not empty
std::vector<int> CSpawnChooser::SelectSpotsFallingBack(std::vector<std::vector<int>> allSpots)
{
	std::vector<int> result;
	for (const auto spots : allSpots)
	{
		if (!spots.empty())
		{
			result = spots;
			break;
		}
	}
	return result;
}

void CSpawnChooser::SetupSpotAwareness()
{
	ALERT(at_aiconsole, "Spawning player: %s\n", m_spawningPlayer->GetName());

	// The engine works with only one player at a time, so it takes that player's info
	// to check PVS, but it's usually the player that has just made a kill. Here we want
	// to check other players' PVS relying on the functions provided by the engine,
	// so we have to make a little hack here and change engine's current player's position
	// to the different spawn points and check if, from those points, other players
	// are in the PVS or PAS. This assumes it's not possible to have Player 1 in the PVS/PAS
	// of Player 2 without the opposite being true (that P2 is also in the PVS/PAS of P1)

	// FIXME: There's an issue where the engine doesn't have any player loaded at the start
	// of a game. My findings when testing with a bot is that when I first spawn (I'm alone)
	// there's no engine player, then I go for a walk around the map, spawn the bot, and
	// there's no engine player yet. It's only present by the time I kill the bot for 1st time,
	// so basically the initial spawns cannot use spot awareness to decide the next spawn spot, which
	// I think is not a big deal TBH, as we can just rely on the Classic, Random or Far spawn systems

	// Save this to restore later
	const auto currPlayerIdx = ENGINE_CURRENT_PLAYER();
	CBasePlayer* enginePlayer = AgPlayerByIndex(currPlayerIdx);
	std::vector<CBasePlayer*> enemies = m_spawningPlayer->GetPlayingEnemies();

	if (FNullEnt(enginePlayer))
	{
		ALERT(at_aiconsole, "Engine player not found... skipping spawn point awareness setup\n");
		return;
	}
	else
		ALERT(at_aiconsole, "Engine player: %s\n", enginePlayer->GetName());

	if (enemies.empty())
	{
		ALERT(at_aiconsole, "No enemy found. Skipping spawn point awareness setup\n");
		return;
	}

	Vector originalOrigin = enginePlayer->pev->origin;

	unsigned char* pvs = nullptr;
	unsigned char* pas = nullptr;

	// Check visibility from different spawn points
	auto j = 0;
	for (const auto spawnPoint : g_spawnPoints)
	{
		ALERT(at_aiconsole, "PVS/PAS checks for spot #%d:\n", j + 1); // +1 'cos 1-based index for any player reading this

		m_spotsState[j] = SPOT_UNKNOWN;

		for (const auto enemy : enemies)
		{
			// We move the player that is used by the engine, to this spawnpoint, to check the PVS/PAS from there
			enginePlayer->pev->origin = spawnPoint->pev->origin;
			SetupVisibility(nullptr, enginePlayer->edict(), &pvs, &pas);

			// TODO: maybe predict the position where the enemy will be in a second,
			// and also check PVS and PAS from there as long as it's inbounds?
			const auto visible = ENGINE_CHECK_VISIBILITY(enemy->edict(), pvs);
			const auto audible = ENGINE_CHECK_VISIBILITY(enemy->edict(), pas);

			const auto enemyPosition = enginePlayer->edict() == enemy->edict() ? originalOrigin : enemy->pev->origin;
			const auto spawnDistToEnemy = (enemyPosition - spawnPoint->pev->origin).Length();

			ALERT(at_aiconsole, "- %s ", enemy->GetName());
			if (!visible && !audible)
			{
				// Potentially good spot, not visible nor audible for this enemy
				if (!(m_spotsState[j] & (SPOT_VISIBLE | SPOT_AUDIBLE)))
				{
					// But it will only be safe if it's not visible nor audible by other enemies
					m_spotsState[j] |= SPOT_SAFE;
				}
				ALERT(at_aiconsole, "is far from there");
			}
			else
			{
				if (visible)
				{
					m_spotsState[j] |= SPOT_VISIBLE;
					m_spotsState[j] &= ~SPOT_SAFE;
					ALERT(at_aiconsole, "can see it");
				}
				if (audible)
				{
					if (visible)
						ALERT(at_aiconsole, ", and ");

					if ((spawnDistToEnemy >= BARELY_AUDIBLE_DIST) && !(m_spotsState[j] & SPOT_AUDIBLE))
					{
						m_spotsState[j] |= SPOT_BARELY_AUDIBLE;
						ALERT(at_aiconsole, "can barely hear it");
					}
					else
					{
						m_spotsState[j] |= SPOT_AUDIBLE;

						// It's clearly audible now, and not safe anymore
						m_spotsState[j] &= ~SPOT_BARELY_AUDIBLE;
						m_spotsState[j] &= ~SPOT_SAFE;

						ALERT(at_aiconsole, "can hear it");
					}
				}
			}
			ALERT(at_aiconsole, " (dist: %.2f)\n", spawnDistToEnemy);

			j++;
		}
		// Another line break to clearly separate the different enemies' list of spawn checks
		ALERT(at_aiconsole, "\n");

		// Restore PVS/PAS
		enginePlayer->pev->origin = originalOrigin;
		SetupVisibility(nullptr, enginePlayer->edict(), &pvs, &pas);
	}

	ClassifySpots();
}

std::vector<int> CSpawnChooser::GetRecentlyUsedSpots()
{
	// We don't want to spawn in one of the last used spots, so we'll calculate how many of the
	// last ones we have to discard, and then simply not put them in any list so they can't be chosen
	const auto totalToDiscard = static_cast<int>(V_min(g_spawnHistory.size(), ag_spawn_avoid_last_spots.value * g_spawnPoints.size()));
	std::vector<int> result;

	ALERT(at_aiconsole, "Number of spots to ignore: %d\n", totalToDiscard);

	for (auto i = g_spawnHistory.size(); i--;)
	{
		const auto usedSpot = g_spawnHistory[i];
		auto idx = -1;

		// Get the spot index
		for (auto i = g_spawnPoints.size(); i--;)
		{
			if (g_spawnPoints[i] == usedSpot)
			{
				idx = i;
				break;
			}
		}

		// We only want unique spots, no duplicates, so we're gonna prepare a vector
		// with all the correct spots to discard, or to ignore in the rest of the process
		if (find(result.begin(), result.end(), idx) != result.end())
		{
			ALERT(at_aiconsole, "Duplicate spot #%d detected in the spot history\n", idx + 1);
			continue;
		}
		else
		{
			ALERT(at_aiconsole, "Adding spot #%d to ignorables due to recent use\n", idx + 1);
			result.push_back(idx);
		}

		if (result.size() == totalToDiscard)
			break;
	}

	return result;
}

// Classify spots into some lists to have the info nicely stored
// and make the position-aware spawn system easier to its read code
void CSpawnChooser::ClassifySpots()
{
	std::vector<int> spotsToDiscard = GetRecentlyUsedSpots();

	for (auto const& spot : m_spotsState)
	{
		auto idx = spot.first;
		if (find(spotsToDiscard.begin(), spotsToDiscard.end(), idx) != spotsToDiscard.end())
		{
			ALERT(at_aiconsole, "Ignoring spot #%d due to being recently used, will try not to use it as next spot\n", idx + 1);
			continue; // Ignore this spot, as it has been recently used
		}

		auto state = spot.second;

		ALERT(at_aiconsole, "Spot #%d, state: %d", idx + 1, state);

		// These lists will be exclusive, a spot won't be in 2 lists at a time, but just in one of them
		if (state & SPOT_VISIBLE)
		{
			// TODO: we probably want to remove false positives from the visible list, by tracing some rays to that spot
			m_visibleSpots.push_back(idx);
			ALERT(at_aiconsole, " -> goes to visible spots");
		}
		else if (state & SPOT_AUDIBLE)
		{
			m_audibleSpots.push_back(idx);
			ALERT(at_aiconsole, " -> goes to audible spots");
		}
		else if (state & (SPOT_SAFE | SPOT_BARELY_AUDIBLE))
		{
			// Not just `else` because who knows if more states will be added
			m_safeSpots.push_back(idx);
			ALERT(at_aiconsole, " -> goes to safe spots");

			if (state & SPOT_BARELY_AUDIBLE)
				m_barelyAudibleSpots.push_back(idx);
		}
		ALERT(at_aiconsole, "\n");
	}
}
