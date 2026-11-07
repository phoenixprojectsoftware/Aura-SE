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

#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

namespace NameFilter
{
    struct FilterData
    {
        bool ready = false;
        std::vector<std::regex> patterns;
    };

    inline const FilterData& GetFilter()
    {
        static const FilterData filter = []()
            {
                FilterData data;

                // AgReadFile reads relative to Aura's game directory.
                std::string contents = AgReadFile("char.auc");

                if (contents.empty())
                {
                    ALERT(
                        at_console,
                        "[NameFilter] ERROR: char.auc is missing or empty!\n");

                    return data;
                }

                // Remove the UTF-8 BOM, if present.
                if (contents.compare(0, 3, "\xEF\xBB\xBF") == 0)
                    contents.erase(0, 3);

                std::istringstream stream(contents);
                std::string line;
                unsigned int lineNumber = 0;

                while (std::getline(stream, line))
                {
                    ++lineNumber;

                    // Remove Windows CRLF terminators.
                    if (!line.empty() && line.back() == '\r')
                        line.pop_back();

                    // Trim outer whitespace only.
                    const size_t first =
                        line.find_first_not_of(" \t");

                    if (first == std::string::npos)
                        continue;

                    const size_t last =
                        line.find_last_not_of(" \t");

                    line = line.substr(first, last - first + 1);

                    // Ignore comments.
                    if (line[0] == '#')
                        continue;

                    // Limit configuration complexity.
                    if (line.size() > 256 ||
                        data.patterns.size() >= 256)
                    {
                        ALERT(
                            at_console,
                            "[NameFilter] ERROR: Invalid limits at line %u.\n",
                            lineNumber);

                        return FilterData{};
                    }

                    try
                    {
                        data.patterns.emplace_back(
                            line,
                            std::regex_constants::ECMAScript |
                            std::regex_constants::icase);
                    }
                    catch (const std::regex_error& error)
                    {
                        ALERT(
                            at_console,
                            "[NameFilter] Invalid regex at line %u: %s\n",
                            lineNumber,
                            error.what());

                        // Fail closed: do not use an incomplete
                        // moderation configuration.
                        return FilterData{};
                    }
                }

                if (data.patterns.empty())
                {
                    ALERT(
                        at_console,
                        "[NameFilter] ERROR: No valid rules loaded!\n");

                    return data;
                }

                data.ready = true;

                ALERT(
                    at_console,
                    "[NameFilter] Loaded %u prohibited-name patterns.\n",
                    static_cast<unsigned int>(data.patterns.size()));

                return data;
            }();

        return filter;
    }

    inline bool IsReady()
    {
        return GetFilter().ready;
    }

    inline bool IsBlocked(const char* name)
    {
        // Missing rules or malformed names are rejected.
        if (!IsReady() || !name || !name[0])
            return true;

        // GoldSrc player names are normally much shorter.
        // This prevents excessively long regex inputs.
        if (std::strlen(name) > 63)
            return true;

        for (const std::regex& pattern : GetFilter().patterns)
        {
            try
            {
                if (std::regex_search(name, pattern))
                    return true;
            }
            catch (const std::regex_error&)
            {
                ALERT(
                    at_console,
                    "[NameFilter] ERROR: Regex matching failed.\n");

                // Fail closed if a regex cannot be evaluated.
                return true;
            }
        }

        return false;
    }
}
