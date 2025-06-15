#ifndef __F18E_TAILFINS__
#define __F18E_TAILFINS__
#include "F18Weap.h"
#include <crtdbg.h>
#include <dplay.h>

#define SINGLE_RESFILE		"F18Mission.rsc"
#define SINGLE_RESFILE_CMP	"F18Mission.cnk"
#define QUICK_RESFILE		"F18Quick.rsc"
#define AVAIL_AIRFRAME		64
#define AVAIL_F18E			64
#define	MAX_ACTIVE_AIRCRAFT	64

#define EMPTY_SLOT			(-1)
#define WEAP_UNLIMITED		(-1)
/*
 *	Defines for states of Airframes (AFSTATUS...) & FlightCrew (FCSTATUS...)
 */

#define	AFSTATUS_OK				0
#define AFSTATUS_MIA			1
#define AFSTATUS_DESTROYED		2
#define AFSTATUS_DAMAGED		3
								
#define FCSTATUS_OK				0
#define FCSTATUS_MIA			1
#define FCSTATUS_KIA			2
#define FCSTATUS_WOUNDED		3
#define	FCSTATUS_ASSIGNED		4
#define FCSTATUS_HUMAN			5
								
#define RANK_2LT				0
#define RANK_1LT				1
#define RANK_CAP				2
#define RANK_MAJ				3
#define RANK_LTC				4
#define RANK_COL				5	
								
#define SQUADRON_MO_391			0
#define SQUADRON_SJ_336			1
#define SQUADRON_SJ_335			2
#define SQUADRON_SJ_333			3
#define SQUADRON_SJ_334			4
#define SQUADRON_AK_90			5
#define SQUADRON_LN_494			6
#define SQUADRON_LN_492			7
								
#define FLAG_USA				0
#define FLAG_IRAN				1
#define FLAG_IRAQ				2

// Mission types for initial resource allocation
#define MISSION_HISTORICAL		0
#define MISSION_HYPOTHETICAL	1
#define MISSION_SINGLE			2
#define MISSION_TRAINING		3
#define MISSION_QUICK			4

typedef struct _PILOTFILE
{
	_PILOTFILE *		pNext;
	char				szName[260];
	int					nStatus;
	DPID				dpid;
	int					nSlot;
} PILOTFILE;

typedef struct __SQUADRONPATH 
{	
	char				szName[260];
	__SQUADRONPATH *	pNext;
} SQUADRONPATH;

extern PILOTFILE *		g_pPilotFileList;
extern SQUADRONPATH *	g_pSquadronFileList;

struct _MISS_FILE_
{
	char			szFile[260];
	int			iID;
	_MISS_FILE_ *	pNext;
	BOOL			bUsed;
};
typedef struct _MISS_FILE_ MISSION_FILE_FOUND;


/* 
 * Total number of F-15E's currently in existence 
 */

#define TOTAL_F18E			64

// Used to represent ONE aircraft
typedef struct
{
	int		nID;				// Ordinal 
	int		nReserved1;			// used to be nAssigned;	
	int		nDebriefID;
	int		nReserved2;
	int		nReserved3;
	int		nReserved4;
	short	sYear;				// Year manufactured
	short	sSerial;			// Serial number
	short	sStatus;			// Flight status (AFSTATUS_...)
	short	sRepair;			// Extent of damage
} AIRFRAME, *PAIRFRAME;

// Used to represent TWO people (1 team): 1 pilot & 1 WSO
typedef struct
{	
	PILOTFILE *		pPilotFile;				// Ordinal
	int				nVoice;				// Voice key?
	int				nReserved1;			
	int				nReserved2;
	int				nReserved3;
	int				nReserved4;
	char			szPilot[32];		// Pilot's Full Name
	BYTE			byPilotRank;		// Pilot's Rank (RANK_...)
	short			sStatus;			// Pilot's status
	char			szFilename[260];	// Pilot's Filename
	GUID			guid;
} FLIGHTCREW, *PFLIGHTCREW;

// Used to represent 1 of up to 8 loaded aircraft, complete with flight crew
typedef struct
{
	int			nWeight;
	int			nGamePlaneId;
	int			nFlightGroup;			
	DPID		dpid;
	int			nReserved3;
	int			nReserved4;
	AIRFRAME	AirFrame;
	F18Loadout	Loadout;
	FLIGHTCREW	FlightCrew;
	int			nSquadron;
} F18RESOURCES, *PF18RESOURCES;

// Used by campaign and single missions to track status of ALL resources
typedef struct 
{
	F18RESOURCES	ActiveAircraft[MAX_ACTIVE_AIRCRAFT];// (Up to) 64 currently selected aircraft, flight crew & armament
	int				anUserAirframes[AVAIL_F18E];	// Ordinals of (up to) 24 aircraft currently visible by user
	AIRFRAME		Airframes[AVAIL_AIRFRAME];		// ALL 206 existing F18-E airframes, w/status
//	int				anUserFlightCrew[AVAIL_F18E];	// Ordinals of (up to) 24 Pilot/WSO tandems currently visible by user
//	FLIGHTCREW		FlightCrew[AVAIL_AIRFRAME];		// ALL 206 pairs of Pilot/WSO's, w/status
	int				anInventory[MAX_WEAPONS];		// weapon inventory; weapon id inferred by index
	DWORD			dwReserved5;					// dwSquadronId;					// Squadron Id
	DWORD			dwReserved;						// Future use
	DWORD			dwReserved1;					// Future use
	DWORD			dwReserved2;					// Future use
	DWORD			dwReserved3;					// Future use
	DWORD			dwReserved4;					// Future use
	char			szSquadron0[260];	
	char			szSquadron1[260];	
	char			szSquadron2[260];	
	char			szSquadron3[260];	
	char			szSquadron4[260];	
	char			szSquadron5[260];	
	char			szSquadron6[260];	
	char			szSquadron7[260];	
	char			szReserved[260];
} RESOURCEFILE, *PRESOURCEFILE;

/*
 * Function prototypes
 */

// Creates a brand new copy of a clean resource file
BOOL	CopyToNewFile (char *pszDstFile, char *pszSrcFile);

// Call this to start resource management; returns file handle & fills struct
HANDLE	LoadResourceFile (char *pszFile, RESOURCEFILE *pResources);	// opens with read/write priveleges
HANDLE	ReadResourceFile (char *pszFile, RESOURCEFILE *pResources);	// opens with read only priveleges

// Call these two functions to set minimum visible resources
//int		SetFlightCrewResources (RESOURCEFILE *pRes, int nTotal);		jjd 1Apr99 removed
int		SetAirframeResources (RESOURCEFILE *pRes, int nTotal);					// jjd 1Apr99 removed last parameter, BOOL bInclude91Airframes = TRUE);

// Call to write modified resource struct to disk
BOOL	WriteResourceFile (HANDLE hFile, RESOURCEFILE *pResources);

// Call to close access to resource file
BOOL	CloseResourceFile (HANDLE hFile);

// One stop shopping to get player's aircraft loadout, complete with split middle bays
BOOL	GetPlayerLoadout (char *pszFile, F18Loadout *pLoadout);

// Internal functions
BOOL	AddFlightCrew (RESOURCEFILE *pRes, int nTandemID);
BOOL	AddAirframe (RESOURCEFILE *pRes, int nAirframe);

BOOL	ReturnWeapon (int nWeaponId, int nCount, RESOURCEFILE *pRes);
BOOL	CreateNewResourceFile (char *pszDestFile, int nMissionType, int nAircraftVisible, int nFlightGroups, int *pnAircraftInGroups);
BOOL	SaveLoadout (char *pszLoadoutFile, F18Loadout *pLoadout, char *pszDescription, BOOL bOverwrite);
BOOL	LoadoutInStock (F18Loadout *pLoadout, RESOURCEFILE *pRes, BOOL bRemove);
BOOL	LoadAircraft (F18Loadout *pLoadout, F18RESOURCES *pAircraft, RESOURCEFILE *pRes);
BOOL	UnloadAircraft (F18RESOURCES *pAircraft, RESOURCEFILE *pRes);
BOOL	GetLoadout (char *pszLoadoutFile, F18Loadout *pLoadout, char *pszDesc = NULL);
void	SplitCenterBays (F18Loadout *pLoadout);
void	ConsolidateCenterBays (F18Loadout *pLoadout);
BOOL	GetSquadronResources (char *pszResFile, F18RESOURCES Squadron[8]);
BOOL	WriteSquadronResources (char *pszResFile, F18RESOURCES Squadron[8]);

// Happy functions for campaign resource management
BOOL	ModifyAirframeStatus (int nAirFrameId, int nNewStatus, RESOURCEFILE *pRes);
BOOL	ModifyFlightCrewStatus (int nFlightCrewId, int nNewStatus, RESOURCEFILE *pRes);
int		GetAirframesAvailable (RESOURCEFILE *pRes);
int		GetFlightCrewAvailable (RESOURCEFILE *pRes);

// Wrapper specific utility functions
#ifdef _AFXDLL
void	PrimeTempResources (RESOURCEFILE *pTempRes);
void	WriteTempResources (RESOURCEFILE *pTempRes);

// Aircrew Inventory Mgmt functions
void	ReturnAircrewToInventory (RESOURCEFILE *pResFile, int nSrcBay);
BOOL	TakeAircrewFromAirframe (RESOURCEFILE *pResFile, int nDstBay, int m_nSourceDragBay);
//BOOL	TakeAircrewFromInventory (RESOURCEFILE *pResFile, int nDstBay, int nAircrewId);
//BOOL	TakeAircrewFromInventory (RESOURCEFILE *pResFile, int nDstBay, PILOTFILE *pPilotFile);
BOOL	AssignCrewToAirframe (RESOURCEFILE *pResFile, F18RESOURCES *pF18Resources);
void	PopulateLocalPilots (RESOURCEFILE *pRes);

// Loadout Mgmt functions
BOOL	TakeWeaponsFromInventory (RESOURCEFILE *pResFile, int nDstBay, char *pszLoadoutFile);
void	ReturnWeaponsToInventory (RESOURCEFILE *pResFile, int nSrcBay);
BOOL	TakeWeaponsFromAirframe (RESOURCEFILE *pResFile, int nDstBay, int nSrcBay);
BOOL	AirframeLoaded (RESOURCEFILE *pResFile, int nSquadronAirframe);
BOOL	SetAircraftWeights (RESOURCEFILE *pResFile);
void	ProvideStockLoadout (RESOURCEFILE *pRes, F18Loadout *pLoadout, int nMissionType, BOOL bPreserveExistingLoadouts = FALSE);

// Airframe Mgmt functions
void	TakePlane (RESOURCEFILE *pResFile, int nBayNumber, int nPlaneOrdinal);
void	ReturnPlane (RESOURCEFILE *pResFile, int nBayNumber);
void	StealAirframe (RESOURCEFILE *pResFile, int nDstBay, int nSrcBay);
int		ActivateAircraft (RESOURCEFILE *pRes, int nFlightGroups, int *pnAircraftInGroup);
int		GetAirframeCount (RESOURCEFILE *pRes);

// Generic all case management function to ensure there is at least one craft & all planes are piloted
void	CheckAllPlanes (RESOURCEFILE *pRes, int nMissionType);
void	CompressSquadron (RESOURCEFILE *pRes);
void	CompressFlightGroup (RESOURCEFILE *pRes, int nCurrentTab);

int		GetFlightGroupAndAircraftCounts (int *pnAircraftInFlightGroups);
int		GetNumBriefingGroups (void);
int		CreatePilotList (void);
void	FreePilotList (void);

void	FreeSquadronList (void);
void	CreateSquadronList (void);

BOOL	LoadoutIsEmpty (F18Loadout *pLoadout);
int		GetNextAvailableAirframe (RESOURCEFILE *pRes, BOOL bTakeFromInventory);
void	ReturnAvailableAirframe (RESOURCEFILE *pRes, int nID);

// Utility functions
int		CountCraftInResourceFlightGroup (RESOURCEFILE *pRes, int nCurrentTab);
int		GetFlightGroupFromTab (int nCurrentTab);
BOOL	ProvideSquadrons (RESOURCEFILE *pRes);
char *	GetSquadronName (RESOURCEFILE *pRes, int nFlightGroup);
char *	GetSquadronPath (RESOURCEFILE *pRes, int nFlightGroup);
BOOL	GetTotalFuelWeight (F18Loadout *pLoadout, int *pnCurrentWeight, int *pnMaxWeight);
BOOL	WriteIndexFile (char *pszPath, RESOURCEFILE *pRes);
#endif

#endif