/****
*
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* PHOENIX devs
*
*
****/

#pragma once

#include <cstdio>
#include <cstring>

namespace Phoenix
{
	enum class IdentityStatus
	{
		Unverified,
		Player,
		Developer
	};

	// annoyingly this must be kept in sync with the id list in cl.
	// maybe there's another way
	// fuck it, I'm a lazy
	static const char* const DeveloperSteamIDs[] =
	{
		"0:1:420777771", // sabian
		"0:0:141116975", // ryley
		"0:1:419649759", // glitchgod
		"0:0:192404376", // enzo
		"0:1:54795934", // jan
		"0:1:63945665", // rafael
		"0:1:484919737", // frostlander
		"0:1:516167454", // hitoshii
		"0:0:421607015", // galexion
		"0:0:105253777", // finger
		"0:1:77747696", // playmann
		"0:0:102486805", // ivan naii_
		"0:1:175018298", // maxresdefault
		"0:0:461917277" // Georgie L.
	};

	inline IdentityStatus GetIdentityStatus(const char* authID)
	{
		if (!authID || !authID[0])
			return IdentityStatus::Unverified;

		// engine returns STEAM_ in front of the id
		const char* steamID = authID;

		if (std::strncmp(steamID, "STEAM_", 6) == 0)
			steamID += 6;

		unsigned int universe = 0;
		unsigned int authServer = 0;
		unsigned long long accountNumber = 0;
		char trailing = '\0';

		// reject pending, lan, bot, and malformed ids
		if (std::sscanf(
			steamID,
			"%u:%u:%llu%c",
			&universe,
			&authServer,
			&accountNumber,
			&trailing) != 3)
		{
			return IdentityStatus::Unverified;
		}

		if (universe > 1 ||
			authServer > 1 ||
			accountNumber == 0)
		{
			return IdentityStatus::Unverified;
		}

		// compare y:z so that steam_0:y:z and
		// steam_1:y:z identify the same account
		const char* accountPart = std::strchr(steamID, ':');

		if (!accountPart)
			return IdentityStatus::Unverified;

		for (const char* developerID : DeveloperSteamIDs)
		{
			const char* developerAccount =
				std::strchr(developerID, ':');

			if (developerAccount &&
				std::strcmp(accountPart, developerAccount) == 0)
			{
				return IdentityStatus::Developer;
			}
		}

		return IdentityStatus::Player;
	}
}
