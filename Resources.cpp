#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <io.h>
#include "resources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _AFXDLL
#include "MultiSetupDefs.h"
#include "Chunk.h"
#include "MultiPlayer.h"
#include "dataform.h"
#include "Pilot.h"
typedef		void						(*GETHUMANPLAYERINFO)(void **, void **);
typedef		char*						(*GETREG)(char *);
typedef		int							(*NETFINDSLOT)(DPID);
typedef		int (*GETMULTIPLAYERSTATUS) (void);
extern		GETMULTIPLAYERSTATUS			pfnGetMultiPlayerStatus;
extern		GETHUMANPLAYERINFO			pfnGetHumanPlayerInfo;
extern		GETREG						pfnGetRegValue;
extern		NETFINDSLOT					pfnNetFindSlot;
MULTI_FTP_LIST *			GetMultiPilot (DPID dpid);
#else
#include "regit.h"
#endif

// Defines
#define FC_ACTIVE			1
#define FC_DISABLED			2
#define FC_AVAILABLE		3

// Prototypes
int							GetNumFlightMembers (int iGroup);
int							GetNumBriefingGroups (void);
int							FindFlightFromIndex (int nGroup, int nCraft);
int							GetFlightLeaderIndex (int nGroup);
void						ModifyNumberInFlight(int iPlaneNum, int iNumInFlight);
char *						FixedGetRegValue (char *pszKey);
int							CreateTokenPilots (int nNewPilotsNeeded);
BOOL						FindPlane (RESOURCEFILE *pResFile, int nBayNumber);
void						ReturnWeaponsToInventory (RESOURCEFILE *pResFile, F18Loadout *pLoadout);

// Global variables
PILOTFILE *					g_pPilotFileList;
SQUADRONPATH *				g_pSquadronFileList;
F18Loadout					g_StockLoadout = 
{
	0, 14400, 0, 
	{ 
		{ 37, 1 },		// bay 1: 1 x aim9m
		{ 38, 1 },		// bay 2: 1 x aim120
		{ 37, 2 },		// bay 3: 2 x aim9m
		{ 32, 2 },		// bay 4: 2 x mk82
		{ 138, 1 },		// bay 5: 1 x aas38a (targeting flir)
		{ 143, 1 },		// bay 6: 1 x 480 gallon fuel
		{ 137, 1 },		// bay 7: 1 x aar50 tins (nav flir)
		{ 32, 2 },		// bay 8: 2 x mk82
		{ 37, 2, },		// bay 9: 2 x aim9m
		{ 38, 1, },		// bay 10: 1 x aim120
		{ 37, 1 },		// bay 11: 1 x aim9m
		{ 1, 1 },		// bay 12: 1 x an/ale-40 chaff flare....note this should change to an/ale50 when available
		{ 44, 1 },		// bay 13: 1 x pgu28 60mm cannon
		{ 0, 0 },
		{ 0, 0 },
	}, 
};

#ifndef _AFXDLL
extern "C" __declspec ( dllexport ) long	dpWhoAmID (void);

BOOL GetPlayerLoadout (char *pszFile, F18Loadout *pLoadout)
{
	RESOURCEFILE	res;
	HANDLE			hFile = LoadResourceFile (pszFile, &res);
	DPID			dpid;
	int				n;
	int				nMyIndex = 0;
	extern int		MultiPlayer;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (MultiPlayer)
		{
			dpid = dpWhoAmID ();
			for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
			{
				if (res.ActiveAircraft[n].dpid == dpid)
				{
					nMyIndex = n;
					break;
				}
			}
		}
	
		// grab first element in squadron array; this will be the player
		memcpy (pLoadout, &res.ActiveAircraft[nMyIndex].Loadout, sizeof (F18Loadout));
		CloseResourceFile (hFile);
		return (TRUE);
	}

	return (FALSE);
}
#endif

#ifdef _AFXDLL

void ProvideStockLoadouts (RESOURCEFILE *pRes, int nMissionType)
{
	F18Loadout *pLoadout = &g_StockLoadout;;

	for (int i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
	{
		// If there is a plane here,
		if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
		{
			// Check to see if the stock loadout can be filled from the iventory of weapons
			if (LoadoutInStock (pLoadout, pRes, TRUE))
			{
				// If so, copy it over
				memcpy (&pRes->ActiveAircraft[i].Loadout, pLoadout, sizeof (F18Loadout));
			}
		}
	}
}

BOOL LoadoutIsEmpty (F18Loadout *pLoadout)
{
	for (int i = 0; i < MAX_F18E_STATIONS; i++)
	{
		if (pLoadout->Station[i].iWeaponId > 0 && pLoadout->Station[i].iWeaponCnt > 0)
			return (FALSE);
	}

	return (TRUE);
}

void ProvideStockLoadout (RESOURCEFILE *pRes, F18Loadout *pLoadout, int nMissionType, BOOL bPreserveLoadout)
{
	F18Loadout *	pGetLoadout;

	if (!LoadoutIsEmpty (pLoadout) && bPreserveLoadout)
		return;

	// Check to see if the stock loadout can be filled from the iventory of weapons
	/*
	if (nMissionType == MISSION_HISTORICAL)
		pGetLoadout = &g_IraqStockLoadout;
	else if (nMissionType == MISSION_HYPOTHETICAL)
		pGetLoadout = &g_IranStockLoadout;
	else
	*/
	pGetLoadout = &g_StockLoadout;

	if (LoadoutInStock (pGetLoadout, pRes, TRUE))
	{
		// If so, copy it over
		memcpy (pLoadout, pGetLoadout, sizeof (F18Loadout));
	}
}

BOOL CreateNewResourceFile (char *pszDestFile, int nMissionType, int nAircraftVisible, int nFlightGroups, int *pnAircraftInFlights)
{
	RESOURCEFILE	res;
	HANDLE			hFile;
	char *			pszSrc;

	switch (nMissionType)
	{
		case MISSION_HISTORICAL:
		case MISSION_HYPOTHETICAL:
		case MISSION_QUICK:
		case MISSION_SINGLE:
		case MISSION_TRAINING:
		default:
			pszSrc = "Template.rsc";
			break;
	}

	CopyToNewFile (pszDestFile, pszSrc);
	hFile = LoadResourceFile (pszDestFile, &res);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		res.szSquadron0[0] = res.szSquadron1[0] = res.szSquadron2[0] = res.szSquadron3[0] = NULL;
		res.szSquadron4[0] = res.szSquadron5[0] = res.szSquadron6[0] = res.szSquadron7[0] = NULL;

		// Function no longer exists
		// SetFlightCrewResources (&res,nAircraftVisible);
		SetAirframeResources (&res, nAircraftVisible);

		if (nMissionType != MISSION_HISTORICAL)
		{
			ActivateAircraft (&res, nFlightGroups, pnAircraftInFlights);
			ProvideStockLoadouts (&res, nMissionType);
		}

		WriteResourceFile (hFile, &res);
		CloseResourceFile (hFile);
	}

	return (hFile != INVALID_HANDLE_VALUE);
}

BOOL ProvideSquadrons (RESOURCEFILE *pRes)
{
	char *				pszPath;
	struct _finddata_t	fileData;
	long				lSearch, lResult;
	char				szFile[260];
	char				szSquadron[260];
	int					nCount = 0;
	char *				FixedGetRegValue (char *pszKey);
	DWORD				dwAttributes;

	pszPath = FixedGetRegValue ("resource");
	strcpy (szFile, pszPath);
	strcpy (szSquadron, pszPath);
	strcat (szSquadron, "\\Squadron\\");
	strcat (szFile, "\\Squadron\\*.*");
	lSearch = lResult = _findfirst (szFile, &fileData);
	while (lResult != -1)
	{
		dwAttributes = GetFileAttributes (fileData.name);
		if ((fileData.name[0] != '.') && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			strcat (szSquadron, fileData.name);

			if (!pRes->szSquadron0[0])
				strcpy (pRes->szSquadron0, szSquadron);

			if (!pRes->szSquadron1[0])
				strcpy (pRes->szSquadron1, szSquadron);

			if (!pRes->szSquadron2[0])
				strcpy (pRes->szSquadron2, szSquadron);

			if (!pRes->szSquadron3[0])
				strcpy (pRes->szSquadron3, szSquadron);

			if (!pRes->szSquadron4[0])
				strcpy (pRes->szSquadron4, szSquadron);

			if (!pRes->szSquadron5[0])
				strcpy (pRes->szSquadron5, szSquadron);

			if (!pRes->szSquadron6[0])
				strcpy (pRes->szSquadron6, szSquadron);

			if (!pRes->szSquadron7[0])
				strcpy (pRes->szSquadron7, szSquadron);
			return (TRUE);
		}

		lResult = _findnext (lSearch, &fileData);
	}

	return (FALSE);
}

BOOL SetAircraftWeights (RESOURCEFILE *pResFile)
{
	if (!pResFile)
		return (FALSE);

	for (int i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
	{
		if (pResFile->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
			pResFile->ActiveAircraft[i].nWeight = GetWeightNoFuel (&pResFile->ActiveAircraft[i].Loadout);
		else
			pResFile->ActiveAircraft[i].nWeight = 0;
	}

	return (TRUE);
}

char * GetSquadronName (RESOURCEFILE *pRes, int nFlightGroup)
{
	char *		pszSquadFile = GetSquadronPath (pRes, nFlightGroup);
	char *		pszResult = NULL;

	if (pszSquadFile)
		pszResult = strrchr (pszSquadFile, '\\');

	if (pszResult)
		pszResult++;

	return (pszResult);
}

char * GetSquadronPath (RESOURCEFILE *pRes, int nFlightGroup)
{
	char *		pszResult = NULL;

	switch (nFlightGroup)
	{
		case 0:
			pszResult = pRes->szSquadron0;
			break;

		case 1:
			pszResult = pRes->szSquadron1;
			break;

		case 2:
			pszResult = pRes->szSquadron2;
			break;

		case 3:
			pszResult = pRes->szSquadron3;
			break;

		case 4:
			pszResult = pRes->szSquadron4;
			break;
		
		case 5:
			pszResult = pRes->szSquadron5;
			break;
		
		case 6:
			pszResult = pRes->szSquadron6;
			break;

		case 7:
			pszResult = pRes->szSquadron7;
			break;
	}

	return (pszResult);
}

int ActivateAircraft (RESOURCEFILE *pRes, int nFlightGroups, int *pnAircraft)
{
	int			n, j;
	int			nFirstIndex;
	int			nCountFilled = 0;

	// Free up any aircraft sitting around from previous use of file
	// 8/12/99 Don't need this...actually gets in the way...
	/*
	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
	{
		if (pRes->ActiveAircraft[n].AirFrame.nID != EMPTY_SLOT)
			ReturnPlane (pRes, n);
	}
	*/

	for (n = 0; n < nFlightGroups; n++)
	{
		nFirstIndex = n << 3;
		for (j = 0; j < pnAircraft[n]; j++)
		{
			FindPlane (pRes, nFirstIndex + j);
			nCountFilled++;
		}
	}

	// For each airframe activated, give it an aircrew
	// & count the number of airframes filled for return value
	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
	{
		if (pRes->ActiveAircraft[n].AirFrame.nID != EMPTY_SLOT)
			AssignCrewToAirframe (pRes, &pRes->ActiveAircraft[n]);
	}

	return (nCountFilled);
}

int ActivateAircraftInGroup (RESOURCEFILE *pRes, int nGroup, int *pnAircraft)
{
	int			n, j;
	int			nFirstIndex;
	int			nCountFilled = 0;

	nFirstIndex = nGroup << 3;
	for (j = 0; j < pnAircraft[nGroup]; j++)
	{
		FindPlane (pRes, nFirstIndex + j);
		nCountFilled++;
	}

	// For each airframe activated, give it an aircrew
	// & count the number of airframes filled for return value
	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
	{
		if (pRes->ActiveAircraft[n].AirFrame.nID != EMPTY_SLOT)
			AssignCrewToAirframe (pRes, &pRes->ActiveAircraft[n]);
	}

	return (nCountFilled);
}


int GetAircraftInGroup (RESOURCEFILE *pRes, int nGroup)
{
	int		nStartPlane;
	int		nEndPlane;
	int		n;
	int		nCount = 0;

	nStartPlane = nGroup << 3;
	nEndPlane = nStartPlane + 8;

	for (n = nStartPlane; n < nEndPlane; n++)
	{
		if (pRes->ActiveAircraft[n].AirFrame.nID != EMPTY_SLOT)
			nCount++;
	}

	return (nCount);
}

BOOL FormulatePilotFilePath (PILOTFILE *pPilotFile, char *pszBuffer)
{
	char *				pszResource;
	MULTI_FTP_LIST *	pPlayerList = g_pMultiPlayerList;

	if (!pPilotFile || !pszBuffer)
		return (FALSE);

	pszResource = FixedGetRegValue ("resource");
	if (pPilotFile->dpid)
	{
		while (pPlayerList)
		{
			if (pPlayerList->dpid == pPilotFile->dpid)
			{
				strcpy (pszBuffer, pPlayerList->szFullPilotPath);
				return (TRUE);
			}
			else
				pPlayerList = pPlayerList->pNext;
		}

		return (FALSE);
	}
	else
		sprintf (pszBuffer, "%s\\pilots\\%s.fpf", pszResource, pPilotFile->szName);

	return (TRUE);
}

void PopulateAllPlanes (RESOURCEFILE *pRes)
{
	int				i, j, k;
	int				nGroups = GetNumBriefingGroups();
	int				nGroupLeader;
	int				nAircraftInGroup;
	int				nFirstCraft, nStop;
	char			szFile[260];
	CChunkFile		chunkFile;
	CHUNKPILOT		pilotRec;
	DWORD			dwSize;

	for (i = 0; i < 8; i++)
	{
		if (i < nGroups)
		{
			nGroupLeader = GetFlightLeaderIndex (i + 1);
			nAircraftInGroup = GetAircraftInGroup (pRes, i);

			if (nGroupLeader != -1)
			{
				ModifyNumberInFlight (nGroupLeader, nAircraftInGroup);

				nFirstCraft = i << 3;
				nStop = nFirstCraft + nAircraftInGroup;

				for (k = 0, j = nFirstCraft; j < nStop; j++, k++)
					pRes->ActiveAircraft[j].nGamePlaneId = FindFlightFromIndex (i + 1, k);
			}
		}
	}

	for (i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
	{
		if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
		{
			_ASSERT (pRes->ActiveAircraft[i].FlightCrew.pPilotFile);

			// Set to random value first in case looking it up fails
			pRes->ActiveAircraft[i].FlightCrew.nVoice = (rand () % 13);

			if (FormulatePilotFilePath (pRes->ActiveAircraft[i].FlightCrew.pPilotFile, szFile))
			{
				if (chunkFile.Open (szFile, FALSE, CHUNK_PILOT_FILE))
				{
					dwSize = chunkFile.GetChunkSize (PILOTCHK_PILOT, 0);
					if (dwSize == sizeof (pilotRec))
					{
						if (chunkFile.GetChunk (PILOTCHK_PILOT, 0, &pilotRec))
							pRes->ActiveAircraft[i].FlightCrew.nVoice = pilotRec.nVoice;
					}

					chunkFile.Close ();
				}
			}
 
			pRes->ActiveAircraft[i].AirFrame.nDebriefID = pRes->ActiveAircraft[i].AirFrame.nID;
			AssignCrewToAirframe (pRes, &pRes->ActiveAircraft[i]);
		}
		else
			pRes->ActiveAircraft[i].AirFrame.nDebriefID = EMPTY_SLOT;
	}
}

BOOL WriteIndexFile (char *pszPath, RESOURCEFILE *pRes)
{
	int			i, anPilotArray[8];
	HANDLE		hFile;
	char		szFile[260];
	DWORD		dwBytes = 0;
	BOOL		bReturn = FALSE;
	PILOTFILE *	pPilotFile;

	for (i = 0; i < 8; i++)
		anPilotArray[i] = -1;

	for (i = 0; i < 64; i++)
	{
		if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT && pRes->ActiveAircraft[i].FlightCrew.pPilotFile)
		{
			pPilotFile = pRes->ActiveAircraft[i].FlightCrew.pPilotFile;

			if (pPilotFile->dpid != NULL)
			{	
				pPilotFile->nSlot = pfnNetFindSlot (pPilotFile->dpid);
				_ASSERT (pPilotFile->nSlot != -1);
				anPilotArray[pPilotFile->nSlot] = pRes->ActiveAircraft[i].nGamePlaneId;

				// Mark off our dpid for future reference
				pRes->ActiveAircraft[i].dpid = pPilotFile->dpid;
			}
		}
	}

	sprintf (szFile, "%s\\pindex.dat", pszPath);
	hFile = CreateFile (szFile, GENERIC_READ|GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		bReturn = WriteFile (hFile, anPilotArray, sizeof (int) * 8, &dwBytes, NULL);
		CloseHandle (hFile);
	}
	
	return (bReturn);
}

void CheckAllPlanes (RESOURCEFILE *pRes, int nMissionType)
{
	int		nAircraft[8];
	int		nAirFrameCount[8];
	int		nFlightGroups=0;
	int		iNumAircraft=0;
	int		nFirstCraftInGroup=0;

	// If we have more than one flight group, then lets make sure
	// everyone can Mombo.  The rule is that if a flight group (Briefing
	// group A..H exists, then there must be atleast one aircraft in that
	// group - AND THAT'S THE BOTTOM LINE, BECAUSE STONE COLD SAYS SO!!!

	nFlightGroups = GetFlightGroupAndAircraftCounts (nAircraft);

	for (int i=0; i<nFlightGroups; i++)
	{
		iNumAircraft=CountCraftInResourceFlightGroup (pRes,i);
		if (!iNumAircraft)
		{
			nAirFrameCount[i] = 1;
			ActivateAircraftInGroup (pRes,i,nAirFrameCount);
			nFirstCraftInGroup = (i << 3);
			ProvideStockLoadout (pRes, &pRes->ActiveAircraft[nFirstCraftInGroup].Loadout, nMissionType);
		}
	}

	// Below is the old code that used to do the work, it should still be a good
	// safe guard if anything slips on by

	int		nCount = GetAirframeCount (pRes);

	if (!nCount)
	{
		nAirFrameCount[0] = 1;
		ActivateAircraft (pRes, 1, nAirFrameCount);
		ProvideStockLoadout (pRes, &pRes->ActiveAircraft[0].Loadout, nMissionType);
	}
	
	PopulateAllPlanes (pRes);
}

void CompressSquadron (RESOURCEFILE *pRes)
{
	int					nGroup;
	int					nNextSlot = 0;

	for (int i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
	{
		if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
		{
			if (i != nNextSlot)
			{
				nGroup = (i >> 3) + 1;
				pRes->ActiveAircraft[i].nFlightGroup = nGroup;
				_ASSERT (pRes->ActiveAircraft[i].nGamePlaneId != -1);
//				pRes->ActiveAircraft[i].nGamePlaneId = FindFlightFromIndex (nGroup, MAX_ACTIVE_AIRCRAFT - (i >> 3));

				memcpy (&pRes->ActiveAircraft[nNextSlot], &pRes->ActiveAircraft[i], sizeof (F18RESOURCES));
				memset (&pRes->ActiveAircraft[i], 0, sizeof (F18RESOURCES));
				pRes->ActiveAircraft[i].AirFrame.nID = EMPTY_SLOT;
			}

			nNextSlot++;
		}
	}
}

void FreeSquadronList (void)
{
	SQUADRONPATH	*	pList = g_pSquadronFileList;
	SQUADRONPATH	*	pNext;

	while (pList)
	{
		pNext = pList->pNext;
		free (pList);
		pList = pNext;
	}

	g_pSquadronFileList = NULL;
}

void CreateSquadronList (void)
{
	char *				pszSquadPath;
	struct _finddata_t	fileData;
	long				lSearch, lResult;
	char				szPath[260];
	SQUADRONPATH *		pNew;
	int					nCount = 0;

	FreeSquadronList ();

	pszSquadPath = FixedGetRegValue("resource");
	strcpy( szPath, pszSquadPath );
	strcat( szPath, "\\squadron\\*.*");
	lSearch = lResult = _findfirst (szPath, &fileData);
	while (lResult != -1)
	{
		// Add first element
		if( fileData.attrib == _A_SUBDIR && strcmp( fileData.name, "." ) != 0 && strcmp( fileData.name, ".." ) != 0 )
		{
			pNew = (SQUADRONPATH *) malloc (sizeof (SQUADRONPATH));
			if (pNew)
			{
				strcpy (pNew->szName, fileData.name);
				pNew->pNext = NULL;
			}

			if (!g_pSquadronFileList)
				g_pSquadronFileList = pNew;
			else
			{
				pNew->pNext= g_pSquadronFileList;
				g_pSquadronFileList = pNew;
			}

			nCount++;
		}

		lResult = _findnext (lSearch, &fileData);
	}
}

void RemovePilotFromList (PILOTFILE *pPilotFile)
{
	PILOTFILE *			pList = g_pPilotFileList;
	PILOTFILE *			pPrev = NULL;

	if (pPilotFile)
	{
		while (pList)
		{
			if (pList == pPilotFile)
			{
				if (!pPrev)
					g_pPilotFileList = pPilotFile->pNext;
				else
					pPrev->pNext = pPilotFile->pNext;

				free (pPilotFile);
				return;
			}
			else
			{
				pPrev = pList;
				pList = pList->pNext;
			}
		}
	}
}

void AddHumanToPilotList (MULTI_FTP_LIST *pHumanPilot)
{
	PILOTFILE *			pNew = NULL;

	if (pHumanPilot)
	{
		pNew = (PILOTFILE *) calloc (1, sizeof (PILOTFILE));
		if (pNew)
		{
			pNew->nSlot = pHumanPilot->nSlot;
			pNew->dpid = pHumanPilot->dpid;
			pNew->nStatus = FCSTATUS_OK;
			strcpy (pNew->szName, pHumanPilot->szPilotName);
		}
	}

	if (pNew)
	{
		pNew->pNext = g_pPilotFileList;
		g_pPilotFileList = pNew;
	}
}

void AddFileToPilotList (char *pszName)
{
	PILOTFILE *			pNew = NULL;
	PILOTFILE *			pList = g_pPilotFileList;
	PILOTFILE *			pPrev = NULL;

	if (!pszName)
		return;

	pNew = (PILOTFILE *) calloc (1, sizeof (PILOTFILE));
	if (pNew)
	{
		pNew->nStatus = FCSTATUS_OK;
		strcpy (pNew->szName, pszName);

		// Add newly created pilot to end of list
		if (!pList)
			g_pPilotFileList = pNew;
		else
		{
			while (pList)
			{
				pPrev = pList;
				pList = pList->pNext;
			}

			pPrev->pNext = pNew;
		}
	}
}

void UpdatePilotList (void)
{
	PILOTFILE *					aOldHumanPilots[8];
	PILOTFILE *					pList = g_pPilotFileList;
	MULTI_FTP_LIST *			aNewHumanPilots[8];
	MULTI_FTP_LIST *			pTmp;
	extern MULTI_FTP_LIST *		g_pMultiPlayerList;
	int							i, j;
	int							nOldCount = 0;
	int							nNewCount = 0;

	memset (aOldHumanPilots, 0, sizeof (PILOTFILE *) * 8);
	memset (aNewHumanPilots, 0, sizeof (MULTI_FTP_LIST *) * 8);

	// Walk through list looking for human players; put them into an array based on their slots
	while (pList)
	{
		if (pList->dpid)
			aOldHumanPilots[nOldCount++] = pList;

		pList = pList->pNext;
	}

	// Walk through list of multiplayers, put them into new array, again based on their slots
	pTmp = g_pMultiPlayerList;
	while (pTmp)
	{
		_ASSERT (pTmp->dpid);
		if (pTmp->dpid)
			aNewHumanPilots[nNewCount++] = pTmp;

		pTmp = pTmp->pNext;
	}

	// for each pilot that previously existed...check to see if it is still there in the new list
	// for each matching pair found, simply remove each from their lists
	for (i = 0; i < nOldCount; i++)
	{
		for (j = 0; j < nNewCount; j++)
		{
			if (aOldHumanPilots[i] && aNewHumanPilots[j] && (aOldHumanPilots[i]->dpid == aNewHumanPilots[j]->dpid))
			{
				// Updating slot information in case pilot was added before it had slot
				aOldHumanPilots[i]->nSlot = aNewHumanPilots[j]->nSlot;
				aOldHumanPilots[i] = NULL;
				aNewHumanPilots[j] = NULL;
				j = nNewCount;
			}
		}
	}

	// When done, those left in the old list need to be removed;
	for (i = 0; i < nOldCount; i++)
	{
		if (aOldHumanPilots[i])
			RemovePilotFromList (aOldHumanPilots[i]);
	}

	// And those left in the new list need to be added
	for (j = 0; j < nNewCount; j++)
	{
		if (aNewHumanPilots[j])
			AddHumanToPilotList (aNewHumanPilots[j]);
	}
}

extern MULTIOPTIONS	g_MultiOptions;

int CreatePilotList (void)
{
	char *				pszPath;
	char *				pSuffix;
	struct _finddata_t	fileData;
	long				lSearch, lResult;
	char				szFile[260];
	int					nCount = 0;
	PILOTFILE *			pNew;
	char *				FixedGetRegValue (char *pszKey);
	LPDWORD				pdwNew = NULL;

	FreePilotList ();

	pszPath = FixedGetRegValue ("resource");
	strcpy (szFile, pszPath);
	strcat (szFile, "\\Pilots\\*.fpf");
	lSearch = lResult = _findfirst (szFile, &fileData);
	while (lResult != -1)
	{
		pNew = (PILOTFILE *) calloc (1, sizeof (PILOTFILE));
		if (pNew)
		{
			strcpy (pNew->szName, fileData.name);
			pSuffix = strrchr (pNew->szName, '.');
			if (pSuffix)
				*pSuffix = NULL;

			pNew->nStatus = FCSTATUS_OK;

			if (!strcmpi (pNew->szName, g_MultiOptions.szPilotFile))
				pNew->dpid = g_MultiOptions.dpidMe;

			pNew->pNext = NULL;

			if (!g_pPilotFileList)
				g_pPilotFileList = pNew;
			else
			{
				pNew->pNext = g_pPilotFileList;
				g_pPilotFileList = pNew;
			}

			nCount++;
		}

		lResult = _findnext (lSearch, &fileData);
	}

	if (nCount<64)
	{
		CreateTokenPilots (64-nCount);
	}

	// Finally, add Multiplayer pilots to list
	if (pfnGetMultiPlayerStatus ())
		UpdatePilotList ();

	return (nCount);
}

void FreePilotList (void)
{
	PILOTFILE *			pList = g_pPilotFileList;
	PILOTFILE *			pNext;

	while (pList)
	{
		pNext = pList->pNext;
		free (pList);
		pList = pNext;
	}

	g_pPilotFileList = NULL;
}

BOOL AddAirframe (RESOURCEFILE *pRes, int nAirframeIndex)
{
	int		n;

	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
	{
		if (pRes->anUserAirframes[n] == EMPTY_SLOT)
		{
			pRes->anUserAirframes[n] = nAirframeIndex;
			return (TRUE);
		}
	}

	return (FALSE);
}

int SetAirframeResources (RESOURCEFILE *pRes, int nRequested)
{
	int				anStatus[MAX_ACTIVE_AIRCRAFT];
	int				n;
	int				nCurrent;
	int				nIndex;
	int				nCount;
	int				nReady;

	// seed random generator
	srand ((unsigned)time (NULL));

	// set all aircraft slots to empty
	memset (anStatus, 0, sizeof (int) * MAX_ACTIVE_AIRCRAFT);

	// go through currently visible aircraft & tally usable planes
	for (n = nCurrent = nCount = 0; n < AVAIL_F18E; n++)
	{
		// Determine which entry is in this slot
		nIndex = pRes->anUserAirframes[n];
		if (nIndex != EMPTY_SLOT)
		{
			// If slot isn't empty, check their flight status
			if (pRes->Airframes[nIndex].sStatus == AFSTATUS_OK)
			{
				// If healthy, increment count
				nCount++;
				anStatus[nIndex] = FC_ACTIVE;
			}
//			else 
//			{
//				// Otherwise, remove them from flight status
//				pRes->anUserAirframes[n] = EMPTY_SLOT;
//				anStatus[nIndex] = FC_DISABLED;
//			}
		}
	}

	nCount += GetAirframeCount(pRes);

	if (nCount < nRequested)
	{
		// Get status of all aircraft
		for (nReady = n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
		{
			// If we didn't already check this entry
			if (!anStatus[n])
			{
				if (pRes->Airframes[n].sStatus == AFSTATUS_OK)
				{
					anStatus[n] = FC_AVAILABLE;
					nReady++;
				}
				else
					anStatus[n] = FC_DISABLED;
			}
		}

		// If we can't possibly fulfill request, set max we can
		if (nReady + nCount < nRequested)
			nRequested = nCount + nReady;
	}

	// Based on new information of troop status,
	// Check if we need to continue
	if (nCount < nRequested)
	{
		for (n = 0; ((n < MAX_ACTIVE_AIRCRAFT) && (nCount < nRequested)); n++)
		{
			int nRandom = rand () % AVAIL_AIRFRAME;

			while (anStatus[nRandom] != FC_AVAILABLE)
			{
				nRandom++;
				if (nRandom >= AVAIL_AIRFRAME)
					nRandom = 0;
			}
			
			AddAirframe (pRes, nRandom);
			anStatus[nRandom] = FC_ACTIVE;
			nCount++;
		}
	}

	return (nCount);
}

int GetWeaponIndex(long lWeaponID);

BOOL GetTotalFuelWeight (F18Loadout *pLoadout, int *pnCurrentFuelWeight, int *pnMaxFuelWeight)
{
	int				nWeight;
	StationInfo		*pStation;
	int				nIndex;
	int				nMaxFuelWeight;

	if (!pLoadout || !pnCurrentFuelWeight || !pnMaxFuelWeight)
		return (FALSE);

	nMaxFuelWeight = WEIGHT_MAX_INTERNALFUEL;
	nWeight = pLoadout->nInternalFuelWeight;

	for (int i = 0; i < MAX_F18E_STATIONS; i++)
	{
		pStation = &pLoadout->Station[i];
		if (pStation->iWeaponCnt)
		{
			nIndex = GetWeaponIndex (pStation->iWeaponId);

			if (pStation->iWeaponId == ID_330GALLON_TANK)	
			{
				nWeight += WEIGHT_330_GALLONS_FUEL;	
				nMaxFuelWeight += WEIGHT_330_GALLONS_FUEL;	
			}
			else if (pStation->iWeaponId == ID_480GALLON_TANK)	
			{
				nWeight += WEIGHT_480_GALLONS_FUEL;	
				nMaxFuelWeight += WEIGHT_480_GALLONS_FUEL;	
			}
		}
	}

	*pnCurrentFuelWeight = nWeight;
	*pnMaxFuelWeight = nMaxFuelWeight;

	return (TRUE);
}

int GetWeight (F18Loadout *pLoadout)
{
	int				nWeight;
	StationInfo		*pStation;
	int				nWeaponWeight;
	int				nIndex;

	nWeight = pLoadout->nInternalFuelWeight + WEIGHT_EMPTY_CRAFT;	// Initial weight of craft plus internal fuel

	for (int i = 0; i < MAX_F18E_STATIONS; i++)
	{
		pStation = &pLoadout->Station[i];
		if (pStation->iWeaponCnt)
		{
			nIndex = GetWeaponIndex (pStation->iWeaponId);
			nWeaponWeight = pWeaponDB[nIndex].iWeight;

			// If this is a fuel tank, use the database to get the weight of the empty tank,
			// and add the weight of the actual fuel here.
			if (pStation->iWeaponId == ID_330GALLON_TANK)	
				nWeight += WEIGHT_330_GALLONS_FUEL;	
			else if (pStation->iWeaponId == ID_480GALLON_TANK)	
				nWeight += WEIGHT_480_GALLONS_FUEL;	

			nWeight += pStation->iWeaponCnt * nWeaponWeight;
		}
	}

	return (nWeight);
}

int GetWeightNoFuel (F18Loadout *pLoadout)
{
	int				nWeight = WEIGHT_EMPTY_CRAFT;	// Initial weight of craft without any fuel
	StationInfo		*pStation;
	int				nWeaponWeight;
	int				nIndex;

	for (int i = 0; i < 13; i++)
	{
		pStation = &pLoadout->Station[i];
		if (pStation->iWeaponCnt)
		{
			nIndex = GetWeaponIndex (pStation->iWeaponId);

			if ((pStation->iWeaponId != ID_330GALLON_TANK) && (pStation->iWeaponId != ID_480GALLON_TANK))
			{
				nWeaponWeight = pWeaponDB[nIndex].iWeight;
				nWeight += pStation->iWeaponCnt * nWeaponWeight;
			}
		}
	}

	return (nWeight);
}


#endif

HANDLE ReadResourceFile (char *pszFile, RESOURCEFILE *pResources)
{
	DWORD		dwBytesRead;
	BOOL		bOk;
	char *		pszPath;
	char		szFullPath[260];
	HANDLE		hFile = INVALID_HANDLE_VALUE;

	if (pszFile || pResources)
	{
#ifdef _AFXDLL
		pszPath = pfnGetRegValue("resource");
#else
		pszPath = GetRegValue ("resource");
#endif
		sprintf (szFullPath, "%s\\%s", pszPath, pszFile);

		hFile = CreateFile (szFullPath, GENERIC_READ, 
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (pResources)
			{
				bOk = ReadFile (hFile, pResources, sizeof (RESOURCEFILE), 
					&dwBytesRead, NULL);

				if (!bOk || dwBytesRead != sizeof (RESOURCEFILE))
				{
					CloseHandle (hFile);
					hFile = NULL;
				}
			}
		}
	}

	return (hFile);
}

HANDLE LoadResourceFile (char *pszFile, RESOURCEFILE *pResources)
{
	DWORD		dwBytesRead;
	BOOL		bOk;
	char *		pszPath;
	char		szFullPath[260];
	HANDLE		hFile = INVALID_HANDLE_VALUE;

	if (pszFile || pResources)
	{
#ifdef _AFXDLL
		pszPath = pfnGetRegValue("resource");
#else
		pszPath = GetRegValue ("resource");
#endif
		sprintf (szFullPath, "%s\\%s", pszPath, pszFile);

		hFile = CreateFile (szFullPath, GENERIC_READ|GENERIC_WRITE, 
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (pResources)
			{
				bOk = ReadFile (hFile, pResources, sizeof (RESOURCEFILE), 
					&dwBytesRead, NULL);

				if (!bOk || dwBytesRead != sizeof (RESOURCEFILE))
				{
					CloseHandle (hFile);
					hFile = NULL;
				}
			}
		}
	}

	return (hFile);
}

BOOL WriteResourceFile (HANDLE hFile, RESOURCEFILE *pResources)
{
	DWORD	dwBytesRead;
	BOOL	bOk = FALSE;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		SetFilePointer (hFile, 0, NULL, FILE_BEGIN);

		bOk = WriteFile (hFile, pResources, sizeof (RESOURCEFILE), 
			&dwBytesRead, NULL);
	}

	return (bOk);
}

BOOL CloseResourceFile (HANDLE hFile)
{
	return (CloseHandle (hFile));
}

BOOL CopyToNewFile (char *pszDstFile, char *pszSrcFile)
{
	char	szSrc[260];
	char	szDst[260];
	BOOL	bOk;

#ifdef _AFXDLL
	char *pszWrapper = pfnGetRegValue("resource");
#else
	char *pszWrapper = GetRegValue ("resource");
#endif

	sprintf (szSrc, "%s\\%s", pszWrapper, pszSrcFile);
	sprintf (szDst, "%s\\%s", pszWrapper, pszDstFile);

	bOk = CopyFile (szSrc, szDst, FALSE);
	if (bOk)
		bOk = SetFileAttributes (szDst, FILE_ATTRIBUTE_NORMAL);

	return (bOk);
}

BOOL SaveLoadout (char *pszLoadoutFile, F18Loadout *pLoadout, char *pszDescription, BOOL bOverwrite)
{
	HANDLE				hFile;
	DWORD				dwBytesRead;
	BOOL				bOk = FALSE;
	F18LoadoutFile		ldtFile;
	char				szBuffer[260];
	char				szFullPath[260];
	int					nLength;
	char *				pszSuffix;
	char *				pCopy;
	char *				pszResource;

#ifdef _AFXDLL
		pszResource = pfnGetRegValue("resource");
#else
		pszResource = GetRegValue ("resource");
#endif

	memset (&ldtFile, 0, sizeof (F18LoadoutFile));

	if (!pszLoadoutFile || !pLoadout)
		return (FALSE);

	if (pszDescription)
	{
		nLength = strlen (pszDescription);
		if (nLength > (MAX_LDT_DESCRIPTION - 1))
		{
			strncpy (ldtFile.szDescription, pszDescription, MAX_LDT_DESCRIPTION - 1);
			ldtFile.szDescription[MAX_LDT_DESCRIPTION - 1] = NULL;
		}
		else 
			strcpy (ldtFile.szDescription, pszDescription);
	}
	else
		ldtFile.szDescription[0] = NULL;

	// Get rid of file suffix
	strcpy (szBuffer, pszLoadoutFile);
	pszSuffix = strrchr (szBuffer, '.');
	if (pszSuffix)
		*pszSuffix = NULL;

	// Get rid of path
	pszSuffix = strrchr (szBuffer, '\\');
	if (pszSuffix)
		pCopy = pszSuffix + 1;
	else
		pCopy = szBuffer;
	strcpy (ldtFile.szLoadoutName, pCopy);

	// Copy loadout info over
	memcpy (&ldtFile.Loadout, pLoadout, sizeof (F18Loadout));

	sprintf (szFullPath, "%s\\%s", pszResource, pszLoadoutFile);

	// Open file
	hFile = CreateFile (szFullPath, GENERIC_READ|GENERIC_WRITE,
		0, NULL, bOverwrite ? CREATE_ALWAYS : CREATE_NEW, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		bOk = WriteFile (hFile, &ldtFile, sizeof (F18LoadoutFile), &dwBytesRead, NULL);
		CloseHandle (hFile);
	}

	return (bOk);
}

BOOL LoadoutInStock (F18Loadout *pLoadout, RESOURCEFILE *pRes, BOOL bRemove)
{
	int				Inventory[MAX_WEAPONS];
	int				nWeapon, nCount, n;
	BOOL			bOutOfStock, bInStock;

	// Prime return value to error condition
	bInStock = FALSE;

	if (pLoadout && pRes)
	{
		// Make copy of inventory to mark up
		memcpy (Inventory, &pRes->anInventory, sizeof (int) * MAX_WEAPONS);

		// As long as we're not out of stock for a weapon
		// Go through each of the weapon stations
		for (bOutOfStock = FALSE, n = 0; !bOutOfStock && (n < MAX_F18E_STATIONS); n++)
		{
			// Get count of weapons on this station
			nCount = pLoadout->Station[n].iWeaponCnt;

			// If station has armament on it...
			if (nCount > 0)
			{
				nWeapon = pLoadout->Station[n].iWeaponId;

				// remove it from inventory
				if (Inventory[nWeapon] != -1)
				{
					if (Inventory[nWeapon] >= nCount)
						Inventory[nWeapon] -= nCount;
					else // Otherwise, we can't fulfill order
						bOutOfStock = TRUE;
				}
			}
		}

		if (!bOutOfStock)
		{
			bInStock = TRUE;
	
			// If we can fill order && we're supposed to remove it from stock,
			// Copy modified inventory back to master inventory list
			if (bRemove)
				memcpy (&pRes->anInventory, Inventory, sizeof (int) * MAX_WEAPONS);
		}
	}

	return (bInStock);
}

BOOL LoadAircraft (F18Loadout *pLoadout, F18RESOURCES *pAircraft, RESOURCEFILE *pRes)
{
	BOOL	bOk = FALSE;

	if (pLoadout && pAircraft && pRes)
	{
		if (LoadoutInStock (pLoadout, pRes, TRUE))
		{
			memcpy (&pAircraft->Loadout, pLoadout, sizeof (F18Loadout));
			bOk = TRUE;
		}
	}

	return (bOk);
}

BOOL UnloadAircraft (F18RESOURCES *pAircraft, RESOURCEFILE *pRes)
{
	BOOL	bOk;
	int		nCount;

	if (pAircraft && pRes)
	{
		for (int n = 0; n < MAX_F18E_STATIONS; n++)
		{
			// Get count of weapons on this station
			nCount = pAircraft->Loadout.Station[n].iWeaponCnt;

			// If station has armament on it...
			// Send it back to master inventory
			if (nCount > 0)
				ReturnWeapon (pAircraft->Loadout.Station[n].iWeaponId, nCount, pRes);
		}

		bOk = TRUE;
	}
	else
		bOk = FALSE;

	return (bOk);
}

BOOL ReturnWeapon (int nWeaponId, int nCount, RESOURCEFILE *pRes)
{
	if (pRes && nWeaponId < MAX_WEAPONS && nCount > 0)
	{
		// Unless we have unlimited weapons of this type, 
		// Add it to inventory 
		if (pRes->anInventory[nWeaponId] != WEAPCOUNT_UNLIMITED)
			pRes->anInventory[nWeaponId] += nCount;

		return (TRUE);
	}
	
	return (FALSE);
}
/*
typedef struct
{
	int			nLoadoutType;
	int			nInternalFuelWeight;
	DWORD		dwReserved;
	StationInfo Station[MAX_F18E_STATIONS];
} F18Loadout;

typedef struct
{
	F18Loadout	Loadout;
	char		szLoadoutName[260];
	char		szDescription[MAX_LDT_DESCRIPTION];
	int			nReserved1;
	int			nReserved2;
	int			nReserved3;
	int			nReserved4;
} F18LoadoutFile;
*/
BOOL GetLoadout (char *pszLoadoutFile, F18Loadout *pLoadout, char *pszDescBuffer)
{
	F18LoadoutFile		tempLoadoutFile;
	HANDLE				hFile;
	DWORD				dwBytesRead;
	BOOL				bOk = FALSE;
	char *				pszPath;
	char				szFile[260];

#ifdef _AFXDLL
		pszPath = pfnGetRegValue("resource");
#else
		pszPath = GetRegValue ("resource");
#endif

	sprintf (szFile, "%s\\%s", pszPath, pszLoadoutFile);

	hFile = CreateFile (szFile, GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		bOk = ReadFile (hFile, &tempLoadoutFile, sizeof (F18LoadoutFile), &dwBytesRead, NULL);
		_ASSERT (dwBytesRead == sizeof (F18LoadoutFile));

		memcpy (pLoadout, &tempLoadoutFile, sizeof (F18Loadout));
		if (pszDescBuffer)
			strcpy (pszDescBuffer, tempLoadoutFile.szDescription);

		CloseHandle (hFile);
	}

	return (bOk);
}

BOOL ModifyAirframeStatus (int nAirFrameId, int nNewStatus, RESOURCEFILE *pRes)
{
	int				i;

	// Bounce out bogus requests
	if (nAirFrameId < 0 || nAirFrameId >= AVAIL_AIRFRAME)
		return (FALSE);

	if (nNewStatus < AFSTATUS_OK || nNewStatus > AFSTATUS_DAMAGED)
		return (FALSE);

	pRes->Airframes[nAirFrameId].sStatus = nNewStatus;

	if (nNewStatus != AFSTATUS_OK)
	{
		// Remove tandom from active squadron (if there)
		for (i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
		{
			if (pRes->ActiveAircraft[i].AirFrame.nID == nAirFrameId)
			{
				pRes->ActiveAircraft[i].AirFrame.nID = EMPTY_SLOT;
				break;
			}
		}
	}

	return (TRUE);
}

int GetAirframesAvailable (RESOURCEFILE *pRes)
{
	AIRFRAME *	pAirFrame;
	int			nId;
	int			nCount = 0;

	for (int i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
	{
		nId = pRes->anUserAirframes[i];
		if (nId != EMPTY_SLOT)
		{
			pAirFrame = &pRes->Airframes[nId];
			if (pAirFrame->sStatus == AFSTATUS_OK)
				nCount++;
		}
	}

	return (nCount);
}

/*
BOOL GetSquadronResources (char *pszResFile, F18RESOURCES Squadron[8])
{
	RESOURCEFILE	res;
	HANDLE			hFile = LoadResourceFile (pszResFile, &res);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		// grab first element in squadron array; this will be the player
		memcpy (Squadron, &res.ActiveAircraft, sizeof (F18RESOURCES) * 8);
		CloseResourceFile (hFile);
		return (TRUE);
	}

	return (FALSE);
}

BOOL WriteSquadronResources (char *pszResFile, F18RESOURCES Squadron[8])
{
	RESOURCEFILE	res;
	HANDLE			hFile = LoadResourceFile (pszResFile, &res);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		// grab first element in squadron array; this will be the player
		memcpy (&res.ActiveAircraft, Squadron, sizeof (F18RESOURCES) * 8);
		WriteResourceFile (hFile, &res);
		CloseResourceFile (hFile);
		return (TRUE);
	}

	return (FALSE);
}
*/

void ConsolidateCenterBays (F18Loadout *pLoadout)
{
	pLoadout->Station[6].iWeaponCnt += pLoadout->Station[13].iWeaponCnt;
	pLoadout->Station[13].iWeaponCnt = 0;

	pLoadout->Station[7].iWeaponCnt += pLoadout->Station[14].iWeaponCnt;
	pLoadout->Station[14].iWeaponCnt = 0;
}

void SplitCenterBays (F18Loadout *pLoadout)
{
	int		nTotal;

	// Split weapons sitting in bay 6 with bay 13
	// (for front and back left-side racks)
	nTotal = pLoadout->Station[6].iWeaponCnt + pLoadout->Station[13].iWeaponCnt;
	pLoadout->Station[13].iWeaponCnt = nTotal >> 1;
	pLoadout->Station[6].iWeaponCnt = nTotal - (nTotal >> 1);
	pLoadout->Station[13].iWeaponId = pLoadout->Station[6].iWeaponId;

	// Split weapons sitting in bay 7 with bay 14
	// (for front and back right-side racks)
	nTotal = pLoadout->Station[7].iWeaponCnt + pLoadout->Station[14].iWeaponCnt;
	pLoadout->Station[14].iWeaponCnt = nTotal >> 1;
	pLoadout->Station[7].iWeaponCnt = nTotal - (nTotal >> 1);
	pLoadout->Station[14].iWeaponId = pLoadout->Station[7].iWeaponId;
}

void ReturnAircrewToInventory (RESOURCEFILE *pResFile, int nSrcBay)
{
	if (pResFile->ActiveAircraft[nSrcBay].FlightCrew.pPilotFile)
	{
		pResFile->ActiveAircraft[nSrcBay].FlightCrew.pPilotFile->nStatus = FCSTATUS_OK;
		pResFile->ActiveAircraft[nSrcBay].FlightCrew.pPilotFile = NULL;
	}
}

/////////////////////////////////////////////
// Wrapper specific functions
/////////////////////////////////////////////
#ifdef _AFXDLL
extern	RESOURCEFILE	g_resFile;

void PrimeTempResources (RESOURCEFILE *pTempRes)
{
	memcpy (pTempRes, &g_resFile, sizeof (RESOURCEFILE));
}

void WriteTempResources (RESOURCEFILE *pTempRes)
{
	memcpy (&g_resFile, pTempRes, sizeof (RESOURCEFILE));
}

/////////////////////////
// Loadout functions
/////////////////////////

BOOL TakeWeaponsFromInventory (RESOURCEFILE *pResFile, F18Loadout *pLoadout, char *pszLoadoutFile)
{
	F18Loadout	Weapons;
	BOOL		bOk = FALSE;

	// First, get rid of old weapons destination plane has
	ReturnWeaponsToInventory (pResFile, pLoadout);

	// Determine exactly what the loadout consists of by getting
	// name of loadout file & loading it up
	if (pszLoadoutFile)
	{
		if (GetLoadout (pszLoadoutFile, &Weapons))
		{
			if (LoadoutInStock (&Weapons, pResFile, TRUE))
			{
				memcpy (pLoadout, &Weapons, sizeof (F18Loadout));
				bOk = TRUE;
			}
		}
	}

	return (bOk);
}

void ReturnWeaponsToInventory (RESOURCEFILE *pResFile, F18Loadout *pLoadout)
{
	int			i, nCount;
	int			nWeapon;

	for (i = 0; i < MAX_F18E_STATIONS; i++)
	{
		nWeapon = pLoadout->Station[i].iWeaponId;
		pLoadout->Station[i].iWeaponId = 0;

		nCount = pLoadout->Station[i].iWeaponCnt;
		pLoadout->Station[i].iWeaponCnt = 0;

		// If there were weapons in this bay & we DON'T have an unlimited quantity of these 
		// in inventory (== -1) than add these back to inventory
		if (nCount > 0 && nWeapon > 0 && (pResFile->anInventory[nWeapon] != -1))
			pResFile->anInventory[nWeapon] += nCount;
	}
}

void ReturnWeaponsToInventory (RESOURCEFILE *pResFile, int nSrcBay)
{
	int			i, nCount;
	int			nWeapon;
	F18Loadout*	pLoadout = &pResFile->ActiveAircraft[nSrcBay].Loadout;

	for (i = 0; i < MAX_F18E_STATIONS; i++)
	{
		nWeapon = pLoadout->Station[i].iWeaponId;
		pLoadout->Station[i].iWeaponId = 0;

		nCount = pLoadout->Station[i].iWeaponCnt;
		pLoadout->Station[i].iWeaponCnt = 0;

		// If there were weapons in this bay & we DON'T have an unlimited quantity of these 
		// in inventory (== -1) than add these back to inventory
		if (nCount > 0 && nWeapon > 0 && (pResFile->anInventory[nWeapon] != -1))
			pResFile->anInventory[nWeapon] += nCount;
	}
}

BOOL TakeWeaponsFromAirframe (RESOURCEFILE *pResFile, int nDstBay, int nSrcBay)
{
	BOOL	bOk;

	if ((nDstBay >= 0) && (nDstBay < MAX_ACTIVE_AIRCRAFT) && (nSrcBay >= 0) && (nSrcBay < MAX_ACTIVE_AIRCRAFT))
	{
		F18Loadout *pSrc = &pResFile->ActiveAircraft[nSrcBay].Loadout;
		F18Loadout *pDst = &pResFile->ActiveAircraft[nDstBay].Loadout;

		// First, get rid of old weapons destination plane has
		ReturnWeaponsToInventory (pResFile, nDstBay);

		// Copy weapons over
		memcpy (pDst, pSrc, sizeof (F18Loadout));

		// Clean out source airframe's weapons
		for (int i = 0; i < MAX_F18E_STATIONS; i++)
		{
			pSrc->Station[i].iWeaponId = -1;		
			pSrc->Station[i].iWeaponCnt = 0;		
		}

		bOk = TRUE;
	}
	else
		bOk = FALSE;

	return (bOk);
}

BOOL AirframeLoaded (RESOURCEFILE *pResFile, int nAirframe)
{
	int			n;
	F18Loadout *pLoadout;

	if (nAirframe < 0 || nAirframe >= MAX_ACTIVE_AIRCRAFT)
		return (FALSE);

	pLoadout = &pResFile->ActiveAircraft[nAirframe].Loadout;

	for (n = 0; n < MAX_F18E_STATIONS; n++)
	{
		if (pLoadout->Station[n].iWeaponCnt != 0 && pLoadout->Station[n].iWeaponId != -1)
			return (TRUE);
	}

	return (FALSE);
}

/////////////////////////
// Airframe functions
/////////////////////////

BOOL FindPlane (RESOURCEFILE *pResFile, int nBayNumber)
{
	int			nPlaneId = -1;
	int			i;
	int			nID;
	int			nAirframeIndex;
	FLIGHTCREW	flightcrew;
	F18Loadout	loadout;
	AIRFRAME	*pSrcAircraft = NULL;
	AIRFRAME	*pDstAircraft = &pResFile->ActiveAircraft[nBayNumber].AirFrame;

	memset (&flightcrew, 0, sizeof (FLIGHTCREW));
	flightcrew.pPilotFile = NULL;
	memset (&loadout, 0, sizeof (F18Loadout));

	if (pDstAircraft->nID != EMPTY_SLOT)
	{
		////////////////////////////////////////////////
		//
		// KEEP EXISTING PILOTS (IF WE HAVE ANY)
		//
		////////////////////////////////////////////////
		if (pResFile->ActiveAircraft[nBayNumber].FlightCrew.pPilotFile != NULL)
			memcpy (&flightcrew, &pResFile->ActiveAircraft[nBayNumber].FlightCrew, sizeof (FLIGHTCREW));

		// erase flight crew so it can't be returned by ReturnPlane ();
		memset (&pResFile->ActiveAircraft[nBayNumber].FlightCrew, 0, sizeof (FLIGHTCREW));

		////////////////////////////////////////////////
		//
		// KEEP EXISTING LOADOUT (IF WE HAVE ANY)
		//
		////////////////////////////////////////////////
		if (AirframeLoaded (pResFile, nBayNumber))
			memcpy (&loadout, &pResFile->ActiveAircraft[nBayNumber].Loadout, sizeof (F18Loadout));

		// Erase loadout so it can't be returned by ReturnPlane ();
		memset (&pResFile->ActiveAircraft[nBayNumber].Loadout, 0, sizeof (F18Loadout));

		// Return plane to inventory...
		nID = pResFile->ActiveAircraft[nBayNumber].AirFrame.nID;
		ReturnPlane (pResFile, nBayNumber);
		ReturnAvailableAirframe (pResFile, nID);
	}

	// Remove aircraft from list of available craft
	for (i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
	{
		nAirframeIndex = pResFile->anUserAirframes[i];
		if (nAirframeIndex != EMPTY_SLOT)
		{
			if (pResFile->Airframes[nAirframeIndex].sStatus == AFSTATUS_OK)
			{
				pSrcAircraft = &pResFile->Airframes[nAirframeIndex];
				pResFile->anUserAirframes[i] = EMPTY_SLOT;
				break;
			}
		}
	}

	if (!pSrcAircraft)
		return (FALSE);

	// Set aircraft in current squadron
	memcpy (pDstAircraft, pSrcAircraft, sizeof (AIRFRAME));

	// Copy in the original flight crew (if we had one)
	memcpy (&pResFile->ActiveAircraft[nBayNumber].FlightCrew, &flightcrew, sizeof (FLIGHTCREW));

	// Copy in the original loadout (if we had one)
	memcpy (&pResFile->ActiveAircraft[nBayNumber].Loadout, &loadout, sizeof (F18Loadout));
	return (TRUE);
}

void ReturnPlane (RESOURCEFILE *pResFile, int nBayNumber)
{
	int		nID;

	if (pResFile->ActiveAircraft[nBayNumber].AirFrame.nID != EMPTY_SLOT)
	{
		AIRFRAME	*pAircraft = &pResFile->ActiveAircraft[nBayNumber].AirFrame;
		nID = pAircraft->nID;

		// Put any assets this craft had back into inventory 
		ReturnWeaponsToInventory (pResFile, nBayNumber);
		ReturnAircrewToInventory (pResFile, nBayNumber);

//		pResFile->Airframes[nID].nAssigned = FALSE;
		memset (pAircraft, 0, sizeof (AIRFRAME));
		pAircraft->nID = EMPTY_SLOT;
	}
}

void StealAirframe (RESOURCEFILE *pResFile, int nDstBay, int nSrcBay)
{
	if (nDstBay >= 0 && nDstBay < MAX_ACTIVE_AIRCRAFT && nSrcBay >= 0 && nSrcBay < MAX_ACTIVE_AIRCRAFT && nSrcBay != nDstBay)
	{
		if (pResFile->ActiveAircraft[nSrcBay].AirFrame.nID != EMPTY_SLOT)
		{
			ReturnPlane (pResFile, nDstBay);
			memcpy (&pResFile->ActiveAircraft[nDstBay], &pResFile->ActiveAircraft[nSrcBay], sizeof (F18RESOURCES));
			memset (&pResFile->ActiveAircraft[nSrcBay], 0, sizeof (F18RESOURCES));
			pResFile->ActiveAircraft[nSrcBay].AirFrame.nID = EMPTY_SLOT;
		}
	}
}


int GetFlightGroupAndAircraftCounts (int *pnAircraftInFlightGroups)
{
	int		nFlightGroups;

	if (!pnAircraftInFlightGroups)
		return -1;

	nFlightGroups = GetNumBriefingGroups ();
	for (int i = 0; i < 8; i++)
		pnAircraftInFlightGroups [i] = (i < nFlightGroups) ? GetNumFlightMembers (i + 1) : 0;

	return (nFlightGroups);
}

MULTI_FTP_LIST *	GetMultiPilot (DPID dpid)
{
	MULTI_FTP_LIST *			pList;

	pList = g_pMultiPlayerList;
	while (pList)
	{
		if (pList->dpid == dpid)
			return (pList);
		else
			pList = pList->pNext;
	}

	return (NULL);
}

BOOL FindMultiPlayerPilot (RESOURCEFILE *pRes, F18RESOURCES *pAircraftResources)
{
	MULTI_FTP_LIST *			pPlayers;
	PILOTFILE *					pList;
	BOOL						bPilotFileFound = FALSE;
	BOOL						bPilotPtrSetup = FALSE;

	if (!pAircraftResources)
		return (FALSE);

	if (!pAircraftResources->dpid)
		return (FALSE);

	// Copy in filename of pilot file
	pPlayers = g_pMultiPlayerList;
	while (pPlayers && !bPilotFileFound)
	{
		if (pPlayers->dpid == pAircraftResources->dpid)
		{
			strcpy (pAircraftResources->FlightCrew.szFilename, pPlayers->szFullPilotPath);
			bPilotFileFound = TRUE;
		}
		else
			pPlayers = pPlayers->pNext;
	}

	// Setup pPilotFile pointer in aircraft
	pList = g_pPilotFileList;
	while (pList && !bPilotPtrSetup)
	{
		if (pList->dpid == pAircraftResources->dpid)
		{
			pAircraftResources->FlightCrew.pPilotFile = pList;
			pList->nStatus = FCSTATUS_ASSIGNED;
			bPilotPtrSetup = TRUE;
		}
		else
			pList = pList->pNext;
	}

	return (bPilotFileFound && bPilotPtrSetup);
}

void PopulateLocalPilots (RESOURCEFILE *pRes)
{
	int			nCrew = GetFlightCrewAvailable (pRes);
	int			nCraft = GetAirframeCount (pRes);
	int			nDiff = nCraft - nCrew;

	if (nDiff > 0)
		CreateTokenPilots (nDiff);

	for (int i = 0; i < AVAIL_AIRFRAME; i++)
	{
		if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
		{
			if (!pRes->ActiveAircraft[i].dpid)
				AssignCrewToAirframe (pRes, &pRes->ActiveAircraft[i]);
			else
				FindMultiPlayerPilot (pRes, &pRes->ActiveAircraft[i]);
		}
	}
}

BOOL AssignCrewToAirframe (RESOURCEFILE *pRes, F18RESOURCES *pAircraftResources)
{
	PILOTFILE *		pTravel;
	char *			pResourcePath;
	int				nCrew;
	static char		s_szPilotPath[260];
	MULTI_FTP_LIST *pMultiPlayer;

	if (!pAircraftResources)
		return (FALSE);

	pAircraftResources->FlightCrew.szFilename[0] = NULL;

	nCrew = GetFlightCrewAvailable (pRes);
	if (!nCrew)
		CreateTokenPilots (8);

	if (pAircraftResources->FlightCrew.pPilotFile == NULL)
	{
		// Go through entire list looking for MULTIPLAYER Pilots first
		pTravel = g_pPilotFileList;
		while (pTravel)
		{
			if (pTravel->nStatus == FCSTATUS_OK && pTravel->dpid)
			{
				pTravel->nStatus = FCSTATUS_ASSIGNED;
				pAircraftResources->FlightCrew.pPilotFile = pTravel;
				return (TRUE);
			}
			else
				pTravel = pTravel->pNext;
		}

		// If we got here, we're out of multiplayer pilots...try stock file type pilots instead
		pTravel = g_pPilotFileList;
		while (pTravel)
		{
			if (pTravel->nStatus == FCSTATUS_OK)
			{
				pTravel->nStatus = FCSTATUS_ASSIGNED;
				pAircraftResources->FlightCrew.pPilotFile = pTravel;
				return (TRUE);
			}
			else
				pTravel = pTravel->pNext;
		}
	}

	if (pAircraftResources->FlightCrew.pPilotFile)
	{
		if (!s_szPilotPath[0])
		{
			pResourcePath = FixedGetRegValue ("resource");
			strcpy (s_szPilotPath, pResourcePath);
			strcat (s_szPilotPath, "\\Pilots\\");
		}

		if (pAircraftResources->FlightCrew.pPilotFile->dpid)
		{
			pMultiPlayer = GetMultiPilot (pAircraftResources->FlightCrew.pPilotFile->dpid);
			if (pMultiPlayer)
				strcpy (pAircraftResources->FlightCrew.szFilename, pMultiPlayer->szFullPilotPath);
		}
		else if (!pAircraftResources->FlightCrew.szFilename[0])
			sprintf (pAircraftResources->FlightCrew.szFilename, "%s%s.fpf", s_szPilotPath, pAircraftResources->FlightCrew.pPilotFile->szName);

		if (strlen (pAircraftResources->FlightCrew.pPilotFile->szName) > 31)
		{
			strncpy (pAircraftResources->FlightCrew.szPilot, pAircraftResources->FlightCrew.pPilotFile->szName, 31);
			pAircraftResources->FlightCrew.szPilot[31] = NULL;
		}
		else
			strcpy (pAircraftResources->FlightCrew.szPilot, pAircraftResources->FlightCrew.pPilotFile->szName);
	}

	return (FALSE);
}

int GetNextAvailableAirframe (RESOURCEFILE *pRes, BOOL bTakeFromInventory)
{
	int		n;
	int		nID = EMPTY_SLOT;

	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
	{
		if (pRes->anUserAirframes[n] != EMPTY_SLOT)
		{
			nID = pRes->anUserAirframes[n];
			if (bTakeFromInventory)
				pRes->anUserAirframes[n] = EMPTY_SLOT;

			return (nID);
		}
	}

	return (EMPTY_SLOT);
}

int GetFirstActiveAirframeSlot (int nCurrentTab)
{
	int			nTest = 0x01;
	int			nStartIndex = 0;

	if (!nCurrentTab)
		return 0;

	// Determine which 8 slots in total 64 we need to save our current pilots off into
	while ((nCurrentTab & nTest) == 0)
	{
		nStartIndex++;
		nTest <<= 1;
	}
	_ASSERT (nStartIndex < 8);

	nStartIndex <<= 3;
	return (nStartIndex);
}

void CompressFlightGroup (RESOURCEFILE *pRes, int nCurrentTab)
{
	int				m, n;
	int				nFirst = GetFirstActiveAirframeSlot (nCurrentTab);
	F18RESOURCES *	pFillMe = NULL;
	F18RESOURCES *	pSrc = NULL;
	int				nLast = nFirst + 8;

	for (n = nFirst; n < nLast; n++)
	{
		if (pRes->ActiveAircraft[n].AirFrame.nID == EMPTY_SLOT)
		{
			pFillMe = &pRes->ActiveAircraft[n];

			for (m = n + 1; m < nLast; m++)
			{
				pSrc = &pRes->ActiveAircraft[m];
				if (pSrc->AirFrame.nID != EMPTY_SLOT)
				{
					memcpy (pFillMe, pSrc, sizeof (F18RESOURCES));
					memset (pSrc, 0, sizeof (F18RESOURCES));
					pSrc->AirFrame.nID = EMPTY_SLOT;
					m = nLast;
				}
			}
		}
	}
}

#endif


void ReturnAvailableAirframe (RESOURCEFILE *pRes, int nID)
{
	int		n;

	if (nID < 0)
		return;

#ifdef _DEBUG
	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
		_ASSERT (pRes->anUserAirframes[n] != nID);
#endif

	for (n = 0; n < MAX_ACTIVE_AIRCRAFT; n++)
	{
		if (pRes->anUserAirframes[n] == EMPTY_SLOT)
		{
			pRes->anUserAirframes[n] = nID;
			return;
		}
	}

	return;
}

int GetAirframeCount (RESOURCEFILE *pRes)
{
	int		nCount = 0;

	if (pRes)
	{
		for (int i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
		{
			if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
				nCount++;
		}
	}

	return (nCount);
}

int GetAircraftIndex (RESOURCEFILE *pRes, int nRequestedIndex)
{
	int		nCount = -1;
	
	if (pRes)
	{
		for (int i = 0; i < MAX_ACTIVE_AIRCRAFT; i++)
		{
			if (pRes->ActiveAircraft[i].AirFrame.nID != EMPTY_SLOT)
				nCount++;

			if (nCount == nRequestedIndex)
				return i;
		}
	}

	return (-1);
}

/*   Don't need anymore
BOOL ModifyFlightCrewStatus (PILOTFILE *pPilotFile, int nNewStatus)
{
	int				i;

	// Bounce out bogus requests
	if (!pPilotFile)
		return (FALSE);

	if (nNewStatus < FCSTATUS_OK || nNewStatus > FCSTATUS_WOUNDED)
		return (FALSE);

	pPilotFile->nStatus = nNewStatus;
	return (TRUE);
}
*/

int GetFlightCrewAvailable (RESOURCEFILE *pRes)
{
	PILOTFILE *	pList = g_pPilotFileList;
	int			nCount = 0;

	while (pList)
	{
		if (pList->nStatus == FCSTATUS_OK)
			nCount++;

		pList = pList->pNext;
	}

	return (nCount);
}

/////////////////////////
// Aircrew functions
/////////////////////////
BOOL TakeAircrewFromInventory (RESOURCEFILE *pResFile, int nDstBay, PILOTFILE *pPilotFile)
{
	if (!pPilotFile)
		return (FALSE);

	if (pResFile->ActiveAircraft[nDstBay].FlightCrew.pPilotFile != NULL)
		pResFile->ActiveAircraft[nDstBay].FlightCrew.pPilotFile->nStatus = FCSTATUS_OK;

	pResFile->ActiveAircraft[nDstBay].FlightCrew.pPilotFile = pPilotFile;
	pResFile->ActiveAircraft[nDstBay].FlightCrew.pPilotFile->nStatus = FCSTATUS_ASSIGNED;
	return (TRUE);
}

BOOL TakeAircrewFromAirframe (RESOURCEFILE *pResFile, int nDstBay, int nSrcBay)
{
	return (FALSE);

	if (pResFile->ActiveAircraft[nSrcBay].FlightCrew.pPilotFile)
	{
		if (pResFile->ActiveAircraft[nDstBay].FlightCrew.pPilotFile != NULL)
			pResFile->ActiveAircraft[nDstBay].FlightCrew.pPilotFile->nStatus = FCSTATUS_OK;

		memcpy (&pResFile->ActiveAircraft[nDstBay].FlightCrew, &pResFile->ActiveAircraft[nSrcBay].FlightCrew, sizeof (FLIGHTCREW));
		memset (&pResFile->ActiveAircraft[nSrcBay].FlightCrew, 0, sizeof (FLIGHTCREW));
		return (TRUE);
	}

	return (FALSE);
}

int CountCraftInResourceFlightGroup (RESOURCEFILE *pRes, int nFlightGroup)
{
	int		n;
	int		nFirstCraft = (nFlightGroup << 3);
	int		nLastCraft = nFirstCraft + 8;
	int		nCount = 0;

	_ASSERT (pRes);
	if (pRes)
	{
		for (n = nFirstCraft; n < nLastCraft; n++)
		{
			if (pRes->ActiveAircraft[n].AirFrame.nID != EMPTY_SLOT)
				nCount++;
		}
	}

	return (nCount);
}

int GetFlightGroupFromTab (int nCurrentTab)
{
	int		nTest = 0x01;
	int		nFlightGroup = 0;

	_ASSERT (nCurrentTab);
	if (!nCurrentTab)
		return 0;

	while ((nTest & nCurrentTab) == 0)
	{
		nTest <<= 1;
		nFlightGroup++;
	}
	
	return (nFlightGroup);
}

