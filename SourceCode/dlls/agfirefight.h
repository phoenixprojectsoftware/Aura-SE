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

#ifndef FIREFIGHT_H
#define FIREFIGHT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER

class AgFirefight
{
public:
	AgFirefight();
	virtual ~AgFirefight();
	
	void SpawnWave(int waveIndex);

	void Think();
};

#endif // FIREFIGHT_H