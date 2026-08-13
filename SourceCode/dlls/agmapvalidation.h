/****
*
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Map Validation
*
*
****/

#pragma once

#include "agglobal.h"

enum AgMapValidationFailure
{
	AG_MAP_VALID = 0,

	AG_MAP_TOO_FEW_DEATHMATCH_SPAWNS,
	AG_MAP_INVALID_CTF_CONFIG,
	AG_MAP_INVALID_DOM_CONFIG,
	AG_MAP_INVALID_FF_CONFIG
};

struct AgMapValidationResult
{
	AgMapValidationFailure m_Failure;
	int m_iActualCount;
	int m_iRequiredCount;
	AgString m_sDescription;

	AgMapValidationResult() : m_Failure(AG_MAP_VALID), m_iActualCount(0), m_iRequiredCount(0)
	{}

	AgMapValidationResult(
		AgMapValidationFailure failure,
		const AgString& description,
		int actualCount = 0,
		int requiredCount = 0)
		: m_Failure(failure),
		m_iActualCount(actualCount),
		m_iRequiredCount(requiredCount),
		m_sDescription(description)
	{}

	bool IsValid() const
	{
		return m_Failure == AG_MAP_VALID;
	}
};
