//++ BulliT
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamerules.h"
#include "player.h"
#include "game.h"
#include "aggamemode.h"
#include "agglobal.h"
#include "agsettings.h"
#ifdef AGSTATS
#include "agstats.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DLL_GLOBAL bool g_bMapchange = false;
DLL_GLOBAL AgString g_sNextMap;
DLL_GLOBAL AgString g_sNextRules;
DLL_GLOBAL AgString g_sNextMapMode;

extern DLL_GLOBAL AgString g_sNextmode;
extern DLL_GLOBAL AgString g_sGamemode;

extern cvar_t timeleft, fragsleft;
extern int gmsgNextmap;

AgSettings::AgSettings()
{
	m_bChangeNextLevel = false;
	g_bMapchange = false;

	m_bCheckNextMap = true;
	m_bCalcNextMap = true;

	m_fNextCheck = gpGlobals->time + 10.0;
}

AgSettings::~AgSettings()
{

}

bool AgSettings::Think()
{
	if (!g_pGameRules)
		return false;

	if (g_bMapchange)
		return false;

	if (m_bChangeNextLevel)
	{
		m_bChangeNextLevel = false;
		g_bMapchange = true;
		//Change the map.
		AgChangelevel(g_sNextMap);

		if (g_sNextRules.size())
		{
			SERVER_COMMAND((char*)g_sNextRules.c_str());
			g_sNextRules = "";
		}
		return false;
	}

	if (g_fGameOver)
		return true;

	//No need to do rest of this every frame.
	if (m_fNextCheck > gpGlobals->time)
		return true;

	if (m_bCalcNextMap)
		CalcNextMap();

	m_fNextCheck = gpGlobals->time + 5.0; //Every 5 seconds.

	//Check if to display next map.
	if (m_bCheckNextMap && timelimit.value || m_bCheckNextMap && fraglimit.value)
	{
		if (timeleft.value && 60 > timeleft.value || fraglimit.value && 2 > fragsleft.value)
		{
			AnnounceNextMap();

			m_bCheckNextMap = false;
		}
	}

	return true;
}

bool AgSettings::AdminSetting(const AgString& sSetting, const AgString& sValue)
{
	if (0 == strnicmp(sSetting.c_str(), "ag_", 3)
		|| 0 == strnicmp(sSetting.c_str(), "mp_timelimit", 12)
		|| 0 == strnicmp(sSetting.c_str(), "mp_fraglimit", 12)
		)
	{
		CVAR_SET_STRING(sSetting.c_str(), sValue.c_str());
		return true;
	}
	return false;
}


void AgSettings::Changelevel(const AgString& sMap)
{
	if (32 < sMap.size() || 0 == sMap.size())
		return;

	char szTemp[64];
	strcpy(szTemp, sMap.c_str());

	//Check if it exists.
	if (IS_MAP_VALID(szTemp))
	{
		g_sNextMap = sMap;
		g_sNextRules = "";
		g_pGameRules->GoToIntermission();

#ifdef AGSTATS
		Stats.OnChangeLevel();
#endif
	}
}


void AgSettings::SetNextLevel(const AgString& sMap)
{
	if (32 < sMap.size() || 0 == sMap.size())
		return;

	char szTemp[64];

	strcpy(szTemp, sMap.c_str());

	if (!IS_MAP_VALID(szTemp))
		return;

	g_sNextMap = sMap;

	g_sNextMapMode = "";

	AnnounceNextMap();
}

AgString AgSettings::GetNextLevel()
{
	return g_sNextMap;
}

void AgSettings::ChangeNextLevel()
{
	if (32 < g_sNextMap.size() || 0 == g_sNextMap.size())
		return;

	m_bChangeNextLevel = true;
}

/*
void AgSettings::CalcNextMap()
{
  //Calc next map, wont work with maps that are in more than one place in mapcycle file.
  typedef list<AgString> AgMapList;
  AgMapList lstMaps;

  char *pszMapFile = (char*) CVAR_GET_STRING( "mapcyclefile" );
  ASSERT( pszMapFile != NULL );

  // Load the file
  int nLength = 0;
  char* pFileList = (char *)LOAD_FILE_FOR_ME(pszMapFile,&nLength);

  if (pFileList && nLength)
  {
	// Loop while there are lines
	char *pFileCur = pFileList;
	while (nLength > 0)
	{
	  // Get the next line
	  char szLine [256];
	  char *pszLine = szLine;
	  while (nLength > 0 && *pFileCur != '\n')
	  {
		char c = *pFileCur++;
		if (c > ' ' && c < 127) *pszLine++ = c;
		nLength--;
	  }

	  // Remove the LF
	  if (nLength > 0)
	  {
		nLength--;
		pFileCur++;
	  }

	  // Terminate the line
	  *pszLine++ = 0;

	  // If there is anything in the line, add to map list
	  if (szLine[0] && IS_MAP_VALID(szLine))
		lstMaps.push_back(szLine);
	}

	// Free the file
	FREE_FILE(pFileList);
  }

  //Find the next map. Ain't there a find function in stl? weird..
  AgMapList::iterator itrMaps = lstMaps.begin();
  for ( ;itrMaps != lstMaps.end() && *itrMaps != g_sNextMap; ++itrMaps)
  {
  }

  if (itrMaps == lstMaps.end())
  {
	if (0 == lstMaps.size())
	{
	  //No maps in list. Set the current.
	  g_sNextMap = STRING(gpGlobals->mapname);
	}
	else
	{
	  //Map aint in list. Do default to first map.
	  g_sNextMap = *lstMaps.begin();
	}
  }
  else
  {
	++itrMaps;
	if (itrMaps == lstMaps.end())
	{
	  //End of list, use first map in list.
	  g_sNextMap = *lstMaps.begin();
	}
	else
	{
	  //Set next map.
	  g_sNextMap = *itrMaps;
	}

  }

  //Still empty? Should not be so... - this is VERY defenisive programming :)
  if (0 == g_sNextMap.size())
	g_sNextMap = STRING(gpGlobals->mapname);

  //No need to calc more.
  m_bCalcNextMap = false;
  lstMaps.clear();
}
*/


#define MAX_RULE_BUFFER 1024

typedef struct mapcycle_item_s
{
	struct mapcycle_item_s* next;

	char mapname[32];
	int  minplayers, maxplayers;
	char rulebuffer[MAX_RULE_BUFFER];
} mapcycle_item_t;

typedef struct mapcycle_s
{
	struct mapcycle_item_s* items;
	struct mapcycle_item_s* next_item;
} mapcycle_t;

/*
==============
DestroyMapCycle

Clean up memory used by mapcycle when switching it
==============
*/
void AgDestroyMapCycle(mapcycle_t* cycle)
{
	mapcycle_item_t* p, * n, * start;
	p = cycle->items;
	if (p)
	{
		start = p;
		p = p->next;
		while (p != start)
		{
			n = p->next;
			delete p;
			p = n;
		}

		delete cycle->items;
	}
	cycle->items = NULL;
	cycle->next_item = NULL;
}

static char com_token[1500];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char* AgCOM_Parse(char* data)
{
	int             c;
	int             len;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

	// skip whitespace
skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}

	// skip // comments
	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}


	// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c == '\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

	// parse single characters
	if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data + 1;
	}

	// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
			break;
	} while (c > 32);

	com_token[len] = 0;
	return data;
}

/*
==============
COM_TokenWaiting

Returns 1 if additional data is waiting to be processed on this line
==============
*/
int AgCOM_TokenWaiting(char* buffer)
{
	char* p;

	p = buffer;
	while (*p && *p != '\n')
	{
		if (!isspace(*p) || isalnum(*p))
			return 1;

		p++;
	}

	return 0;
}



/*
==============
ReloadMapCycleFile


Parses mapcycle.txt file into mapcycle_t structure
==============
*/
int AgReloadMapCycleFile(char* filename, mapcycle_t* cycle)
{
	char szBuffer[MAX_RULE_BUFFER];
	char szMap[32];
	int length;
	char* pFileList;
	char* aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(filename, &length);
	int hasbuffer;
	mapcycle_item_s* item, * newlist = NULL, * next;

	if (pFileList && length)
	{
		// the first map name in the file becomes the default
		while (1)
		{
			hasbuffer = 0;
			memset(szBuffer, 0, MAX_RULE_BUFFER);

			pFileList = AgCOM_Parse(pFileList);
			if (strlen(com_token) <= 0)
				break;

			strcpy(szMap, com_token);

			// Any more tokens on this line?
			if (AgCOM_TokenWaiting(pFileList))
			{
				pFileList = AgCOM_Parse(pFileList);
				if (strlen(com_token) > 0)
				{
					hasbuffer = 1;
					strcpy(szBuffer, com_token);
				}
			}

			// Check map
			if (IS_MAP_VALID(szMap))
			{
				// Create entry
				char* s;

				item = new mapcycle_item_s;

				strcpy(item->mapname, szMap);

				item->minplayers = 0;
				item->maxplayers = 0;

				memset(item->rulebuffer, 0, MAX_RULE_BUFFER);

				if (hasbuffer)
				{
					s = g_engfuncs.pfnInfoKeyValue(szBuffer, "minplayers");
					if (s && s[0])
					{
						item->minplayers = atoi(s);
						item->minplayers = max(item->minplayers, 0);
						item->minplayers = min(item->minplayers, gpGlobals->maxClients);
					}
					s = g_engfuncs.pfnInfoKeyValue(szBuffer, "maxplayers");
					if (s && s[0])
					{
						item->maxplayers = atoi(s);
						item->maxplayers = max(item->maxplayers, 0);
						item->maxplayers = min(item->maxplayers, gpGlobals->maxClients);
					}

					// Remove keys
					//
					g_engfuncs.pfnInfo_RemoveKey(szBuffer, "minplayers");
					g_engfuncs.pfnInfo_RemoveKey(szBuffer, "maxplayers");

					strcpy(item->rulebuffer, szBuffer);
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
			{
				ALERT(at_console, "Skipping %s from mapcycle, not a valid map\n", szMap);
			}

		}

		FREE_FILE(aFileList);
	}

	// Fixup circular list pointer
	item = cycle->items;

	// Reverse it to get original order
	while (item)
	{
		next = item->next;
		item->next = newlist;
		newlist = item;
		item = next;
	}
	cycle->items = newlist;
	item = cycle->items;

	// Didn't parse anything
	if (!item)
	{
		return 0;
	}

	while (item->next)
	{
		item = item->next;
	}
	item->next = cycle->items;

	cycle->next_item = item->next;

	return 1;
}

/*
==============
CountPlayers

Determine the current # of active players on the server for map cycling logic
==============
*/
int AgCountPlayers(void)
{
	int	num = 0;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity* pEnt = UTIL_PlayerByIndex(i);

		if (pEnt)
		{
			num = num + 1;
		}
	}

	return num;
}

/*
==============
ExtractCommandString

Parse commands/key value pairs to issue right after map xxx command is issued on server
 level transition
==============
*/
void AgExtractCommandString(char* s, char* szCommand)
{
	// Now make rules happen
	char	pkey[512];
	char	value[512];	// use two buffers so compares
								// work without stomping on each other
	char* o;

	if (*s == '\\')
		s++;

	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;

		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		strcat(szCommand, pkey);
		if (strlen(value) > 0)
		{
			strcat(szCommand, " ");
			strcat(szCommand, value);
		}
		strcat(szCommand, "\n");

		if (!*s)
			return;
		s++;
	}
}

/*
==============
ChangeLevel

Server is changing to a new level, check mapcycle.txt for map name and setup info
==============
*/
void AgSettings::CalcNextMap()
{
	static char szPreviousMapCycleFile[256] = "";
	static mapcycle_t mapcycle = {};

	char szNextMap[32];
	char szFirstMapInList[32];
	char szCommands[1500];
	char szRules[1500];

	int minplayers = 0;
	int maxplayers = 0;

	strcpy(
		szFirstMapInList,
		"boot_camp");

	strcpy(
		szNextMap,
		STRING(gpGlobals->mapname));

	szCommands[0] = '\0';
	szRules[0] = '\0';

	const int curplayers =
		AgCountPlayers();

	BOOL do_cycle = TRUE;

	// ---------------------------------------------------------
	// Resolve Aura mapcycle filename.
	// ---------------------------------------------------------

	char userInputMapCycleFile[256];

	const char* userInput =
		CVAR_GET_STRING(
			"mapcyclefile");

	char szMapcycleName[256];

	strncpy(
		szMapcycleName,
		userInput ? userInput : "",
		sizeof(szMapcycleName) - 1);

	szMapcycleName[
		sizeof(szMapcycleName) - 1] = '\0';

	/*
		Remove a trailing .txt if supplied.

		Do this on our own copy. Never modify the string returned
		by CVAR_GET_STRING().
	*/
	const size_t len =
		strlen(szMapcycleName);

	if (len > 4 &&
		stricmp(
			&szMapcycleName[len - 4],
			".txt") == 0)
	{
		szMapcycleName[len - 4] =
			'\0';
	}

	snprintf(
		userInputMapCycleFile,
		sizeof(userInputMapCycleFile),
		"mapcycles/%s.mc",
		szMapcycleName);

	userInputMapCycleFile[
		sizeof(userInputMapCycleFile) - 1] =
		'\0';

		// ---------------------------------------------------------
		// Reload mapcycle if necessary.
		// ---------------------------------------------------------

		if (stricmp(
			userInputMapCycleFile,
			szPreviousMapCycleFile))
		{
			strncpy(
				szPreviousMapCycleFile,
				userInputMapCycleFile,
				sizeof(szPreviousMapCycleFile) - 1);

			szPreviousMapCycleFile[
				sizeof(szPreviousMapCycleFile) - 1] =
				'\0';

				AgDestroyMapCycle(
					&mapcycle);

				if (!AgReloadMapCycleFile(
					userInputMapCycleFile,
					&mapcycle) ||
					!mapcycle.items)
				{
					ALERT(
						at_console,
						"Unable to load map cycle file %s\n",
						userInputMapCycleFile);

					do_cycle = FALSE;
				}
		}

		// ---------------------------------------------------------
		// Select next map.
		// ---------------------------------------------------------

		if (do_cycle &&
			mapcycle.items)
		{
			mapcycle_item_s* pSelected =
				NULL;

			const int iOrder =
				(int)ag_mapcycle_order.value;

			/*
				Determine whether a cycle item is eligible for the
				current number of players.
			*/
			auto IsEligible =
				[curplayers](
					mapcycle_item_s* pItem) -> bool
				{
					if (!pItem)
						return false;

					if (pItem->minplayers != 0 &&
						curplayers <
						pItem->minplayers)
					{
						return false;
					}

					if (pItem->maxplayers != 0 &&
						curplayers >
						pItem->maxplayers)
					{
						return false;
					}

					return true;
				};

			// -----------------------------------------------------
			// RANDOM
			// -----------------------------------------------------

			if (iOrder == 2)
			{
				std::vector<
					mapcycle_item_s*> candidates;

				mapcycle_item_s* pStart =
					mapcycle.items;

				mapcycle_item_s* pItem =
					pStart;

				do
				{
					if (IsEligible(pItem) &&
						stricmp(
							pItem->mapname,
							STRING(
								gpGlobals->mapname)) != 0)
					{
						candidates.push_back(
							pItem);
					}

					pItem =
						pItem->next;
				} while (pItem &&
					pItem != pStart);

				if (!candidates.empty())
				{
					const int iRandom =
						RANDOM_LONG(
							0,
							(int)candidates.size() -
							1);

					pSelected =
						candidates[iRandom];
				}
				else
				{
					/*
						There is no eligible map other than the current
						map. This is the only case where avoiding the
						current map is impossible.

						Fall back to the first eligible item.
					*/
					pItem =
						pStart;

					do
					{
						if (IsEligible(pItem))
						{
							pSelected =
								pItem;

							break;
						}

						pItem =
							pItem->next;
					} while (pItem &&
						pItem != pStart);
				}
			}

			// -----------------------------------------------------
			// SEQUENTIAL
			// -----------------------------------------------------

			else
			{
				mapcycle_item_s* pStart =
					mapcycle.next_item
					? mapcycle.next_item
					: mapcycle.items;

				mapcycle_item_s* pItem =
					pStart;

				do
				{
					if (IsEligible(pItem))
					{
						pSelected =
							pItem;

						break;
					}

					pItem =
						pItem->next;
				} while (pItem &&
					pItem != pStart);

				/*
					If none of the conditional entries matched,
					preserve the old Aura behavior and use the
					current cycle pointer.
				*/
				if (!pSelected)
				{
					pSelected =
						pStart;
				}
			}

			// -----------------------------------------------------
			// Apply selected entry.
			// -----------------------------------------------------

			if (pSelected)
			{
				/*
					Keep the sequence pointer meaningful even when
					random mode is being used. If the server later
					switches back to sequential mode, traversal can
					continue from here.
				*/
				mapcycle.next_item =
					pSelected->next;

				strncpy(
					szNextMap,
					pSelected->mapname,
					sizeof(szNextMap) - 1);

				szNextMap[
					sizeof(szNextMap) - 1] =
					'\0';

					minplayers =
						pSelected->minplayers;

					maxplayers =
						pSelected->maxplayers;

					AgExtractCommandString(
						pSelected->rulebuffer,
						szCommands);

					strncpy(
						szRules,
						pSelected->rulebuffer,
						sizeof(szRules) - 1);

					szRules[
						sizeof(szRules) - 1] =
						'\0';

						/*
							See whether this mapcycle entry explicitly changes
							the gamemode.

							Mapcycle rule buffers are info strings, so this
							supports entries containing:

							\\sv_aura_gamemode\\arcade
						*/
						const char* pszMode =
							g_engfuncs.pfnInfoKeyValue(
								pSelected->rulebuffer,
								"sv_aura_gamemode");

						if (pszMode &&
							pszMode[0] &&
							GameMode.IsGamemode(
								pszMode))
						{
							g_sNextMapMode =
								pszMode;
						}
						else
						{
							g_sNextMapMode = "";
						}
			}
		}

		// ---------------------------------------------------------
		// Validate final map.
		// ---------------------------------------------------------

		if (!IS_MAP_VALID(szNextMap))
		{
			strcpy(
				szNextMap,
				szFirstMapInList);
		}

		ALERT(
			at_console,
			"NEXT MAP: %s\n",
			szNextMap);

		if (minplayers ||
			maxplayers)
		{
			ALERT(
				at_console,
				"PLAYER COUNT: min %i max %i current %i\n",
				minplayers,
				maxplayers,
				curplayers);
		}

		if (strlen(szRules) > 0)
		{
			ALERT(
				at_console,
				"RULES: %s\n",
				szRules);
		}

		g_sNextMap =
			szNextMap;

		g_sNextRules =
			szCommands;

		m_bCalcNextMap =
			false;
}

//-- Martin Webrant

AgString AgSettings::GetNextMode()
{
	if (g_sNextMapMode.size())
		return g_sNextMapMode;

	if (g_sNextmode.size())
		return g_sNextmode;

	if (g_sGamemode.size())
		return g_sGamemode;

	return CVAR_GET_STRING("sv_aura_gamemode");
}

void AgSettings::AnnounceNextMap()
{
	if (!g_sNextMap.size())
		return;

	const AgString sNextMode = GetNextMode();
	const AgString sModeName = GameMode.GetGamemodeName(sNextMode);

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgNextmap);
		WRITE_STRING(g_sNextMap.c_str());
		WRITE_STRING(sModeName.c_str());
	MESSAGE_END();
}
