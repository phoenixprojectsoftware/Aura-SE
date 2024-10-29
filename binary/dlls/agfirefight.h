/****
*
* Copyright © 2021-2024 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Firefight: the firefight gametype class.
*
*
****/

#include "aggamerules.h"
#include <vector>

#ifndef FIREFIGHT_H
#define FIREFIGHT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER

class AgFirefight : public CGameRules
{
public:
	AgFirefight();
	virtual ~AgFirefight();

	void Think();
	void InitMonsters();

private:
	struct RespawnInfo
	{
		edict_t* edict;
		vec3_t spawnPosition;
		float respawnTime;
	};

	std::vector<RespawnInfo> m_respawnQueue;
};

#endif // FIREFIGHT_H