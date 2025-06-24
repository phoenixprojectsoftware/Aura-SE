/****
*
* Copyright © 2021-2025 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Fiesta: the fiesta gametype class.
*
*
****/

#ifndef FIESTA_H
#define FIESTA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER

class AgFiesta
{
public:
	AgFiesta();
	virtual ~AgFiesta();

	void Think();
};

#endif // FIESTA_H