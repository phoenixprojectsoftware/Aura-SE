/****
*
* Copyright (c) 2021-2026 The Phoenix Project Software. Some Rights Reserved.
*
* AURA
*
* Player name censorship
*
*
****/

#pragma once

#include "agglobal.h"

#include <sstream>
#include <string>
#include <vector>

namespace NameFilter
{
	// convert names into a comparable ascii representation.
	// handles case, common substitutions, and separators.
	inline std::string Normalize(const char* input)
	{
		std::string result;

		if (!input)
			return result;

		for (const unsigned char* p = reinterpret_cast<const unsigned char*>(input); *p; ++p)
		{
			unsigned char c = *p;

			if (c >= 'A' && c <= 'Z')
				c = c - 'A' + 'a';

			switch (c)
			{
			case '0': c = 'o'; break;
			case '1': c = 'i'; break;
			case '2': c = 'z'; break;
			case '3': c = 'e'; break;
			case '4': c = 'a'; break;
			case '5': c = 's'; break;
			case '6': c = 'g'; break;
			case '7': c = 't'; break;
			case '8': c = 'b'; break;
			case '9': c = 'g'; break;

			case '@': c = 'a'; break;
			case '$': c = 's'; break;
			case '!': c = 'i'; break;
			case '|': c = 'i'; break;

				// wildcards in submitted names.
			case '*':
			case '?':
				c = '?';
				break;

			default:
				break;
			}

			if ((c >= 'a' && c <= 'z') || c == '?')
			{
				result.push_back(static_cast<char>(c));
			}

			// other punctuation is ignored
		}

		return result;
	}

	inline const std::vector<std::string>& GetBlockedTerms()
	{
		static const std::vector<std::string> terms = []()
			{
				std::vector<std::string> result;

				const std::string file = AgReadFile("char.auc");

				std::istringstream stream(file);
				std::string line;

				while (std::getline(stream, line))
				{
					const size_t first = line.find_first_not_of(" \t\r");

					if (first == std::string::npos)
						continue;

					// comments begin with #
					if (line[first] == '#')
						continue;

					const std::string term = Normalize(line.c_str());

					// avoid short, ambiguous matches
					if (term.size() < 5 || term.find('?') != std::string::npos)
						continue;

					result.push_back(term);
				}

				if (result.empty())
				{
					ALERT(at_console, "[NameFilter] WARNING: No blocked terms loaded!\n");
				}
				else
				{
					ALERT(at_console, "[NameFilter] Loaded %u blocked terms.\n", static_cast<unsigned int>(result.size()));
				}

				return result;
			}();

		return terms;
	}

	inline bool IsBlocked(const char* name)
	{
		const std::string normalized = Normalize(name);

		if (normalized.empty())
			return;

		for (const std::string& term : GetBlockedTerms())
		{
			if (normalized.size() < term.size())
				continue;

			for (size_t i = 0; i + term.size() <= normalized.size(); ++i)
			{
				bool matches = true;
				size_t wildcards = 0;

				for (size_t j = 0; j < term.size(); ++j)
				{
					const char candidate = normalized[i + j];
					if (candidate == '?')
					{
						++wildcards;
						continue;
					}

					if (candidate != term[j])
					{
						matches = false;
						break;
					}
				}

				// limit wildcard matches to reduce false positives
				if (matches && wildcards <= 2)
					return true;
			}
		}

		return false;
	}
}
