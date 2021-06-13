#pragma once

// TODO: refactor AgCTF's EntSelectCTFSpawnPoint() to put this inside the class
BOOL IsSpawnPointValid(CBasePlayer* pPlayer, CBaseEntity* pSpot);

extern std::map<int, std::string> spawnSystemName;

class CSpawnChooser
{
private:
	static constexpr unsigned int SPOT_UNKNOWN = 0;
	static constexpr unsigned int SPOT_VISIBLE = (1 << 0);
	static constexpr unsigned int SPOT_AUDIBLE = (1 << 1);
	static constexpr unsigned int SPOT_BARELY_AUDIBLE = (1 << 2);
	static constexpr unsigned int SPOT_SAFE = (1 << 3);

	CBasePlayer* m_spawningPlayer;
	CBaseEntity* m_lastSpawn;

	// Spot categories related to enemies' awareness of the different spots
	// They contain just the spot number (e.g.: from 1 to 17 in crossfire)
	// The audible category will contain spots that are ONLY audible,
	// if a spot is noth audible and visible, then it goes to visible spots
	std::vector<int> m_visibleSpots;
	std::vector<int> m_audibleSpots;
	std::vector<int> m_barelyAudibleSpots; // this is a subset of safe spots
	std::vector<int> m_safeSpots;

	// Maps the spot number to a bitmask of that spot's state
	std::map<int, int> m_spotsState;

	std::vector<int> CSpawnChooser::SelectSpotsFallingBack(std::vector<std::vector<int>> allSpots);
	std::vector<int> CSpawnChooser::GetRecentlyUsedSpots();
	void ClassifySpots();

public:
	CSpawnChooser(CBasePlayer* player)
	{
		m_spawningPlayer = player;
	}

	CSpawnChooser(CBasePlayer* player, CBaseEntity* lastSpot)
	{
		m_spawningPlayer = player;
		m_lastSpawn = lastSpot;
	}

	// Uses the original spawn system, the only one that AG has had prior to version 6.7,
	// to give you the next spot where you can spawn (one out of the next 1 to 5 spots)
	CBaseEntity* GetClassicSpawnPoint();

	// Gives you a random spawn spot, it can be any in the map, no conditions
	CBaseEntity* GetRandomSpawnPoint();

	// Gives you a random spawn spot that is far away from any opponent,
	// or maybe it's kind of close to some opponent, but it will be among the furthest spots anyways
	CBaseEntity* GetFarSpawnPoint();

	// Gives you a random spawn spot, with higher chance of it being a spot that is safe
	// from the opponents (that they can't see/hear it), with lesser chance of being only
	// audible, and with a slim chance of it being a spot that some opponent can see
	CBaseEntity* GetPositionAwareSpawnPoint();

	// Classifies spawn spots based on opponents' PVS/PAS, that is,
	// whether the spots are visible, and/or audible, or are safe from them
	void SetupSpotAwareness();
};