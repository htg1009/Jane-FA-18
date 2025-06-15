/*****************************************************************************
*
*	NetWork.cpp  --  F18 multi-player code
*
*	David McKibbin		2Sep97
*
*-----------------------------------------------------------------------------
*	Copyright (c) 1997 by Origin Systems, Inc., All Rights Reserved.
*****************************************************************************/

#include <windows.h>
//#include <dplay.h>
#include "dplay.h"		// delete this when DX5 to all
#include "gamesettings.h"
#include "MultiDefs.h"
#include "F18.h"
#include "WrapInterface.h"
#include "Sprite.h"
#include "Resources.h"
#include "MultiPlayer.h"
#include "WrapInterface.h"
#include <crtdbg.h>
#include "keystuff.h"
#include "keysmsg.h"
#include "MultiPlayer.h"

//#undef   DPSEND_GUARANTEED		// disable guaranteed messages
//#define  DPSEND_GUARANTEED 0

//
//	SkunkNet currently has a "C" linkage because I can find no way to get
//
//			--- F18API __declspec(naked) UINT P5time() ---
//
//	to compile in a CPP file.  ERROR: not all control paths return a value.
//	In a C file we get:  "warning C4035: 'P5time' : no return value"
//
#define    F18LIB
#include "SkunkNet.h"

///////////////////////
//					 //
//   Local Defines   //
//					 //
///////////////////////

#define PLAYERS			MAX_PLAYERS
#define CHANNELS	   (MAX_PLAYERS + 2)

#define INT_MAX			((unsigned)-1 >> 1);

#define SlotOK(x)		(((UINT)(x)) < PLAYERS)

void NetRefreshComList(void);
void NetGetAll (void);
void NetGetFtp (DPMSG_GENERIC *pGeneric);
//void RecreatePlayerList (void);
void NetStartSim (DPMSG_STARTSIM *pMsg);
void NetPreviewMission (DPMSG_PREVIEW_MISSION *pMsg);
void NetReadyToFly (DPMSG_JOINER_READY *pMsg);
void NetFtpFlags (DPMSG_FTP_FLAGS *pMsg);
void NetRequestFtpFlags (DPMSG_FTP_FLAGS *pMsg);
void NetFtpTotalBlocks (DPMSG_FTP_BLOCKS *pMsg);
void NetQueryMission (DPMSG_QUERY_MISSION *pMsg);
void NetQueryMissionResult (DPMSG_QUERY_MISSION_RESULT *pMsg);
void InitAvSensorData();
extern void ReleaseShadowEntry(void *pt);
extern DetectedPlaneListType CurFramePlanes;
extern BOOL PrimaryIsAOT(DetectedPlaneListType *Plist);

extern void StopFFForPauseOrEnd();

extern BOOL g_bInSim;
extern BOOL SnowOn, doLightning, doRain;

 // The following parameter values may be overriden via the registry.
//
int					netRate =  50;	// network packet time allowance (ms)
int					netFork =   2;	// minimum packet branching factor
int					netMask =  -1;	// dpSend flag mask
double				netKnob = 0.5;	// feedback loop control valve (50%)
DPID				g_adpidPlayers[MAX_PLAYERS];	// array of 8 directplay id's for host adminstration
//HUMANPILOT *		g_pHumanList;
//DWORD				g_dwHumanListLastModified;
IDirectPlay3A	*	g_lpDirectPlay;

//////////////////////////
//					    //
//   Global Variables   //
//						//
//////////////////////////

extern AvionicsType Av;
extern TargetInfoType *Primary;

				 //  Connection counters
int					cRecv;		// Network messages received
int					cPoll;		// Total polls in SIM
int					cSpin;		// Total polls not in SIM
int					newTime;	// current physics tick, ie. T1
int					MySlot = -1;// my player's slot index [0..N], -1 is uninitialized
int					NetWeapIX;	// weapon GUID (unique system-wide) - HIWORD(slot#) | LOWORD(seq#)
int					PlayerCount;	// save data from dpPlayers()
char  **			PlayerName;
BOOL				g_bIAmHost;
MULTI_FTP_LIST *	g_pTempMultiPlayerList;	// Used by wrappers to keep multiplayer list while in sim...restored to wrappers after sim exits

extern	int			iInGameSlots;
extern NetSlot		Slotarray[MAX_HUMANS + 1];

NetSlot				Slot[MAXAIOBJS];  //  was PLAYERS

typedef struct _DPMSG_SLOTREQUEST
{
	int		type;		// message ID
	int		time;		// system time stamp (ms)
	int		slot;		// player plane slot#
	int		newslot;	// target plane slot#    
	DPID	dpidHost;	// Host dpid
} DPMSG_SLOTREQUEST;

DPMSG_SLOTREQUEST NetSlotRequest;	// static packet for sending

void NetPutSlotRequest (DPID dpTo, int nNewSlot)
{
	g_adpidPlayers[nNewSlot] = dpTo;

	NetSlotRequest.type = NET_SLOTREQUEST;
	NetSlotRequest.time   = GetTickCount ();
	NetSlotRequest.slot	  = MySlot;
	NetSlotRequest.newslot = nNewSlot;
	NetSlotRequest.dpidHost = dpWhoAmID ();

	dpSendGuaranteed ( dpTo, DPSEND_GUARANTEED, &NetSlotRequest, sizeof(NetSlotRequest) );
}

//============================================================================
//		GAME SESSION DATA MANAGEMENT
//============================================================================
//
//	Game Session Description from DirectPlay.
//
DPSESSIONDESC2	GameInfo;
char			GameName[32];	// DPSESSIONNAMELEN

//	GameInfo.dwUser3/4 are used as an 8-byte array of kills
//
#define  NetTally  ((char *)&GameInfo.dwUser3)

void FreeMultiPlayerList (void)
{
	MULTI_FTP_LIST *		pList = g_pTempMultiPlayerList;
	MULTI_FTP_LIST *		pNext;

	while (pList)
	{
		pNext = pList->pNext;
		free (pList);
		pList = pNext;
	}

	g_pTempMultiPlayerList = NULL;
}

BOOL SaveMultiPlayerList (MULTI_FTP_LIST *pWrappersList)
{
	MULTI_FTP_LIST *		pNewEntry;
	BOOL					bOk = TRUE;

	FreeMultiPlayerList ();
	
	while (pWrappersList)
	{
		pNewEntry = (MULTI_FTP_LIST *) malloc (sizeof (MULTI_FTP_LIST));
		_ASSERT (pNewEntry);

		if (pNewEntry)
		{
			memcpy (pNewEntry, pWrappersList, sizeof (MULTI_FTP_LIST));
			pNewEntry->pNext = g_pTempMultiPlayerList;
			g_pTempMultiPlayerList = pNewEntry;
		}
		else
			bOk = FALSE;

		pWrappersList = pWrappersList->pNext;
	}

	return (bOk);
}

BOOL RestoreMultiPlayerList (MULTI_FTP_LIST *pWrapperList)
{
	MULTI_FTP_LIST *		pTemp;

	if (!g_pTempMultiPlayerList || !pWrapperList)
		return (FALSE);

	while (pWrapperList)
	{
		pTemp = g_pTempMultiPlayerList;
		while (pTemp)
		{
			if (pWrapperList->dpid == pTemp->dpid)
			{
				// Copy over whatever other info we need to restore here...
				// be careful not to restore any variables that are out of context, like nMissionExists, for instance
				pWrapperList->nFtpFlags = pTemp->nFtpFlags;
				pTemp = NULL;
			}
			else
				pTemp = pTemp->pNext;
		}

		pWrapperList = pWrapperList->pNext;
	}

	return (TRUE);
}

/*----------------------------------------------------------------------------
 *
 *	NetSetHostFlag ()
 *
 *		Called by wrappers or multi-player message filter to set this machine as host
 *
 */

void NetSetHostFlag (BOOL bIAmHost)
{
	g_bIAmHost = bIAmHost;
	if(g_bIAmHost)
	{
//		RecreatePlayerList ();

		for(PlaneParams *planepnt = Planes; planepnt <= LastPlane; planepnt ++)
		{
			if((planepnt->Status & (AL_AI_DRIVEN|PL_ACTIVE)) == (AL_AI_DRIVEN|PL_ACTIVE))
			{
				if(!((Planes[planepnt->AI.iMultiPlayerAIController].Status & (AL_DEVICE_DRIVEN|AL_COMM_DRIVEN)) && (Planes[planepnt->AI.iMultiPlayerAIController].Status & (PL_ACTIVE))))
				{
					if(planepnt->AI.iAICombatFlags1 & AI_MULTI_REMOVEABLE)
					{
						planepnt->Status = 0;
					}
					else
					{
						planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
						planepnt->AI.iMultiPlayerAIController = PlayerPlane - Planes;
					}
				}
			}
		}
	}

	// clean out our array of directplay slots
	memset (g_adpidPlayers, 0, sizeof (DPID) * MAX_PLAYERS);

	if (MySlot != -1)
		g_adpidPlayers[MySlot] = dpWhoAmID ();
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGameInfo()
 *
 *		Update our GameInfo struct from new DirectPlay data.
 *
 */
void NetPutGameInfo( DPSESSIONDESC2 *dpsd )
{
	if (dpsd)
	{
		GameInfo = *dpsd;
		strncpy( GameName, dpsd->lpszSessionNameA, sizeof(GameName)-1 );
		GameInfo.lpszSessionNameA = GameName;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGameInfo()
 *
 *		Update our GameInfo struct from host.
 *
 */
void NetGetGameInfo()
{
	DPSESSIONDESC2  *dpsd;
	int hr = dpGetSessionDesc( -1, (void**)&dpsd );
	if SUCCEEDED(hr)  NetPutGameInfo( dpsd );
}

/*----------------------------------------------------------------------------
 *
 *	NetSetGameInfo()
 *
 *		Update DP host's SessionDesc from our GameInfo struct.
 *
 */
void NetSetGameInfo()
{
	dpSetSessionDesc( (void*)&GameInfo );
}


//============================================================================
//		HIT/KILL LOGGING & DISPLAY
//============================================================================
/*----------------------------------------------------------------------------
 *
 *	NetRadio()
 *
 *		Display a radio message (and echo debug string)
 *
 *		Note: varargs must be strings
 *
 */
void NetRadio( int msgid, ... )
{
	char msg[256];

    char *argv = (char *)(&msgid+1);	// va_start( argv, msgid );

	FormatMessage( FORMAT_MESSAGE_FROM_HMODULE, NULL, msgid,
					g_iLanguageId, msg, sizeof(msg), &argv );

	if (g_bInSim)  AICAddAIRadioMsgs( msg, 69 );

	strcat( msg, ".\n" );
	OutputDebugString( msg );
}

/*----------------------------------------------------------------------------
 *
 *	KillVerb()  --  return random KILL verb
 *
 *		killed, fried, incinerated, wasted, splashed, nailed, barbecued, splattered
 *
 */
char *KillVerb()
{
	static char verb[32];

	FormatMessage( FORMAT_MESSAGE_FROM_HMODULE, NULL, NET_KILL_VERB+(GameLoop&7),
					g_iLanguageId, verb, sizeof(verb), NULL );
	return verb;
}

/*----------------------------------------------------------------------------
 *
 *	NetSetKill()
 *
 *		Record author of damage to target
 *
 *		To keep compatibility with F15v102f, the "noMsg" argument was added.
 *		NET_DAMAGE messages from old versions will still set the kill, but
 *		will not generate radio messages.  So kill tallys will still work
 *		across different versions.  Patched versions will get kill messages
 *		in all cases and will get hit messages from other patched versions.
 *
 */
void NetSetKill( int src, int tgt, int noMsg)
{
	Slot[tgt].lastHit = src;

	if(src == -1)
		return;

	if (noMsg)  return;	// F15v102f compatibility

	if (src == tgt)
		return;
	if (NetGetSlotFromPlaneIndex(src) == MySlot)
	{
		if((Planes[tgt].Status & PL_ACTIVE) && (strlen(Slot[tgt].name) < 20))
		{
			if(strlen(Slot[tgt].name) == 0)
			{
				NetRadio( NET_HIT_YOU, pDBAircraftList[Planes[tgt].AI.iPlaneIndex].sName );
			}
			else
			{
				NetRadio( NET_HIT_YOU, Slot[tgt].name );
			}
		}
	}
	if (NetGetSlotFromPlaneIndex(tgt) == MySlot)
	{
		if((Planes[src].Status & PL_ACTIVE) && (strlen(Slot[src].name) < 20))
		{
			if(strlen(Slot[src].name) == 0)
			{
				NetRadio( NET_HIT_ME, pDBAircraftList[Planes[src].AI.iPlaneIndex].sName );
			}
			else
			{
				NetRadio( NET_HIT_ME,  Slot[src].name );
			}
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetLogKill()
 *
 *		Only the HOST can set the session descriptor,
 *		but for simplicity all players log the kill.
 *
 */
void NetLogKill( int tgt )		 // tgt: index of crash'd plane
{
	int src = Slot[tgt].lastHit; // src: source of last hit
	int srcslot, tgtslot;

	srcslot = NetGetSlotFromPlaneIndex(src, 1);
	tgtslot = NetGetSlotFromPlaneIndex(tgt, 1);

	Slot[tgt].lastHit = -1;	// clear damage doer

	if((src == -1) || (!SlotOK(srcslot)))	// crash w/o damage
	{
		if((!(Planes[tgt].FlightStatus & PL_OUT_OF_CONTROL)) && ((Planes[tgt].Status & PL_ACTIVE) && (strlen(Slot[tgt].name) < 20)))
			NetRadio( NET_CRASHED, Slot[tgt].name );
		return;
	}

	if((srcslot != -1) || (tgtslot != -1))
	{
		if (srcslot == MySlot)
		{
			if((Planes[tgt].Status & PL_ACTIVE) && (strlen(Slot[tgt].name) < 20))
			{
				if(strlen(Slot[tgt].name) == 0)
				{
					NetRadio( NET_KILL_YOU, KillVerb(), pDBAircraftList[Planes[tgt].AI.iPlaneIndex].sName );
				}
				else
				{
					NetRadio( NET_KILL_YOU, KillVerb(), Slot[tgt].name );
				}
			}
		}
		else
		{
			if(tgtslot == MySlot)
			{
				if((Planes[src].Status & PL_ACTIVE) && (strlen(Slot[src].name) < 20))
				{
					if(strlen(Slot[src].name) == 0)
					{
						NetRadio( NET_KILL_ME,  KillVerb(), pDBAircraftList[Planes[src].AI.iPlaneIndex].sName );
					}
					else
					{
						NetRadio( NET_KILL_ME,  KillVerb(), Slot[src].name );
					}
				}
			}
			else
			{
				if((Planes[tgt].Status & PL_ACTIVE) && (strlen(Slot[tgt].name) < 20) && (Planes[src].Status & PL_ACTIVE) && (strlen(Slot[src].name) < 20))
				{
					char srcname[64], tgtname[64];

					if(strlen(Slot[src].name) == 0)
						strcpy(srcname, pDBAircraftList[Planes[src].AI.iPlaneIndex].sName);
					else
						strcpy(srcname, Slot[src].name);

					if(strlen(Slot[tgt].name) == 0)
						strcpy(tgtname, pDBAircraftList[Planes[tgt].AI.iPlaneIndex].sName);
					else
						strcpy(tgtname, Slot[tgt].name);

					NetRadio( NET_KILL_BY,  KillVerb(), srcname, tgtname );
				}
			}
		}
	}

	if(srcslot != -1)
	{
		NetGetGameInfo();	// make sure we are current (host could have migrated)
		 NetTally[NetGetSlotFromPlaneIndex(src)]++;	// bump killer's tally
		NetSetGameInfo();
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetLetKill()
 *
 *		We want to permanently enable the kill tally for a slot for the
 *		entire duration of the session.  Even if players drop out and
 *		others rejoin, we need a continuous tally.  So at host creation
 *		time the tallys are set to -1 to indicate disabled.  As a player
 *		joins/rejoins the tally is enabled as necessary.
 *
 */
void NetLetKill( int src )
{
	NetGetGameInfo();	// make sure we are current (host could have migrated)
	NetSetGameInfo();
}


//============================================================================
//		SLOT MANAGEMENT
//============================================================================
/*----------------------------------------------------------------------------
 *
 *	NetQuitSlot()
 *
 *		Mark a slot EMPTY and init for future assignment.
 *
 *		Called at DESTROYPLAYER.
 *
 */
void NetQuitSlot( int ix )
{
	int planenum;

	if (!SlotOK(ix))  return;

	planenum = iSlotToPlane[ix];
	if(planenum == -1) return;

	NetSlot  *S = &Slot[planenum];

	char name[20];	// preserve name across "quit" for Debrief kill stats
	strcpy( name, S->name );
	int team = S->x.chTeam;
	int life = S->nLives;

	ZeroMemory( S, sizeof(NetSlot) );

	S->nLives = life;
	S->x.chTeam = team;
	strcpy( S->name, name );

	S->x.iSlot  = -1;	// set slot inactive for wrappers
	S->lastHit  = -1;	// clear damage doer

	S->minDelta = INT_MAX;
	S->netDelta = INT_MAX;

	// Make plane inactive & comm (my available mark)
	//
//	Planes[planenum].Status = PL_COMM_DRIVEN;

	NewGenerator(PLANE_EXPLODES,Planes[planenum].WorldPosition,0.0,2.0f,50);
	Planes[planenum].Status = 0;
	Planes[planenum].AI.iAIFlags2 = 0;

	OrphanAllPlaneSmoke(&Planes[planenum]);

	NetRefreshComList();
}

/*----------------------------------------------------------------------------
 *
 *	NetInitSlot()
 *
 *		ZERO slotand init for future assignment.
 *
 *		Called at session startup.
 *
 */
void NetInitSlot( int ix )
{
	int planex;

	if (!SlotOK(ix))  return;

	planex = NetGetPlaneIndexFromSlot(ix);

	if(planex == -1)	return;

	iSlotToPlane[ix] = -1;

	ZeroMemory( &Slot[planex], sizeof(Slot[planex]) );

	NetQuitSlot( ix );
}

/*----------------------------------------------------------------------------
 *
 *	NetSetSlotData()
 *
 *		Broadcast this slot's data to the world.
 *
 */
void NetSetSlotData( int ix, int nGuarantee )
{
//	if (SlotOK(ix) && (iSlotToPlane[ix] != -1))
	if (SlotOK(ix))
//		dpSetPlayerData( &Slot[ix].x, sizeof(dpSlot), nGuarantee );
		dpSetPlayerData( &Slot[NetGetPlaneIndexFromSlot(ix)].x, sizeof(dpSlot), nGuarantee );
}


/*----------------------------------------------------------------------------
 *
 *	NetGetSlotData()
 *
 *		Return a pointer to this slot's data structure.
 *
 *		Even return free slots so wrappers can just do this once.
 *
 */
dpSlot *NetGetSlotData( int ix )
{
	if(NetGetPlaneIndexFromSlot(ix) == -1) return(NULL);

	return  SlotOK(ix) ? &Slot[NetGetPlaneIndexFromSlot(ix)].x : NULL;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetSlotDataEx ()
 *
 *		Return a pointer to this slot's data structure.
 *		Even return free slots so wrappers can just do this once.
 *
 */
NetSlot *NetGetSlotDataEx ( int ix )
{
	if(NetGetPlaneIndexFromSlot(ix) == -1) return(NULL);

	return  SlotOK(ix) ? &Slot[NetGetPlaneIndexFromSlot(ix)] : NULL;
}

/*----------------------------------------------------------------------------
 *
 *	NetSetSlot()
 *
 *		Assign this slot:
 *
 *		* Copy the DirectPlay player data into our struct.
 *		* Save the player's DPID.
 *		* Save the player's name.
 *		* Reset the 1st packet trigger
 *		* Reset the damage doer
 *
 *		Called at session startup for each player returned by dpPlayers()
 *		and also at SETPLAYERDATA for each player data update
 *
 */
void NetSetSlot( dpSlot *X, int dpid )
{
	int ix = X->iSlot;
	int planenum;

	if (!SlotOK(ix))  return;

	planenum = NetGetPlaneIndexFromSlot(ix);
	if(planenum == -1) return;

	NetSlot *S = &Slot[planenum];

	S->x = *X;	// copy the broadcast struct into the slot

	S->dpid = dpid;		// set the slot active

	S->nPacks = 0;		// reset the 1st packet trigger

	S->lastHit = -1;	// clear damage doer

	DPNAME *dpName = NULL;
	dpGetPlayerName( dpid, (void**)&dpName );

	if (dpName)
		strncpy( S->name, dpName->lpszShortNameA, sizeof(S->name)-1 );

	NetRefreshComList();
}

/*----------------------------------------------------------------------------
 *
 *	NetInitSlots()
 *
 *		Empty all slots and update player and game data from the net.
 *
 */
void NetInitSlots()
{
	 //	Empty all the slots.
	//
	for (int i=0; i<PLAYERS; i++)  NetInitSlot( i );

	 // Get current player list.
	//
	dpPlayers( -1, &PlayerCount, &PlayerName );

	 // Update plane assignments.
	//
	for (i=0; i<PlayerCount; i++)
	{
		dpSlot data;
		int size = sizeof(dpSlot);
		int dpid = dpIndex2ID(i);
		int hr = dpGetPlayerData( dpid, &data, (DWORD*)&size );
		if (hr == 0 && size == sizeof(dpSlot))
			NetSetSlot( &data, dpid );
	}

	NetGetGameInfo();
}

/*----------------------------------------------------------------------------
 *
 *	NetGetSlots()		*** INITIALIZE NETWORK ***
 *
 *		Query network for players and their plane assignments
 *		and update all slot assignments to reflect this state.
 *		Assign a slot for ME.  First choice is my dpPlayer index.
 *		If that is taken, take the first available slot.
 *
 *		SAFETY:  If there are now more players than when we started,
 *				 start over.  We are allowing the host to passively
 *		arbitrate slot assignments.  dpPlayers() seems to order the
 *		player list is reverse order of joining the session, ie.
 *		the last player to join is listed first and the first player
 *		to join (ie. the host) is listed last.  So we use that order
 *		as a globally broadcast slot order.
 *
 */
int NetGetSlots()
{
	int planenum;

	do	// Assign slots until player count is stable.
	{
		NetInitSlots();
	}
	while (PlayerCount != (int)GameInfo.dwCurrentPlayers);

	// If this is the first time through as host, set our slot now
	if (g_bIAmHost && MySlot == -1)
	{
		MySlot = 0;
		g_adpidPlayers[MySlot] = dpWhoAmID ();
	}

	// commented out 11/12/98
	int dpIndex = dpWhoAmI();
	if ((UINT)dpIndex >= (UINT)PlayerCount)
		return -1;	// error

	//	My first slot choice is my DP join order#
	//
	//MySlot = PlayerCount - dpIndex - 1;

	//if (Slot[MySlot].dpid)		// MySlot is already assigned
	//	for (int i=0; i<PLAYERS; i++)
	//		if (Slot[i].dpid == 0)  { MySlot = i; break; }


	//	Broadcast my slot assignment.
	//
	planenum = NetGetPlaneIndexFromSlot(MySlot);
	Slot[planenum].x.iSlot = MySlot;
	NetSetSlotData( MySlot, TRUE );

	//	Init my player's name for wrappers immediate use.
	//	Can't wait for echo of DP_SYS_MSG.
	strncpy( Slot[planenum].name, PlayerName[dpIndex], sizeof(Slot[planenum].name)-1 );

	NetWeapIX = (MySlot << 16) + 256;	// Init my GUID

	Planes[planenum].Status = PL_DEVICE_DRIVEN;

	return MySlot;	// return my slot# [0..7]
}

/*----------------------------------------------------------------------------
 *
 *	NetUnLockWeapons()
 *
 *		Unlock any weapons going after regenning plane.
 *
 */
void NetUnLockWeapons( int planenum )
{
	PlaneParams *planepnt = &Planes[planenum];
	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
	{
		if(W->pTarget == planepnt)
		{
			W->pTarget		= NULL;
			W->iTargetType	= NONE;

			W->Flags &= ~(BOOST_PHASE|ACTIVE_SEARCH|ACTIVE_SEEKER);
			W->Flags |= LOSING_LOCK|LOST_LOCK;
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetUnDamage()
 *
 *		Reset damage variables outside of PlaneParams.
 *
 */
void NetUnDamage( int planenum )
{
	int ix;
	int i;

	ix = NetGetSlotFromPlaneIndex(planenum);

	if(ix != -1)
	{
		for (i=0; i<40; i++)
		{
			cPlayerArmor [ix][i] = (char)125;
			cPlayerDamage[ix][i] = 0;
		}
	}

	for (i=0; i<MAX_DELAYED_DAMAGE; i++)
	{
		if (gDamageEvents[i].iPlaneNum != planenum)  continue;

		gDamageEvents[i].iPlaneNum		= -1;
		gDamageEvents[i].lDamageTimer	= -1;
		gDamageEvents[i].lDamagedSystem	=  0;
	}

	NetUnLockWeapons(planenum);

	// added 11/13/98
	OrphanAllPlaneSmoke(&Planes[planenum]);
}

/*----------------------------------------------------------------------------
 *
 *	NetKillChutes()
 *
 *		Delete all weapons assigned to my plane.
 */
void NetKillChutes( int planenum )
{
	for (WeaponParams *W = Weapons; W <= LastWeapon; W++)
		if (W->Flags & WEAPON_INUSE
		&&  W->P    == &Planes[planenum]
		&&  W->Kind == EJECTION_SEAT)  DeleteBomb(W);

	if(PlayerPlane == &Planes[planenum])
	{
		PlayerPlane->AI.iAIFlags1 &= ~AI_HAS_EJECTED;
		pPlayerChute = NULL;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetSetPlane()
 *
 *		Activate the plane and init its parameters.
 *
 *		Called from NetGrabPlanes(), NetRegenerate() and NetGetPlane() at 1st data
 *
 */
void NetSetPlane( int ix )
{
	int planenum;
	int cnt;
	PlaneParams *planepnt;
	int msgOK = 1;
	BYTE bworkvar;
	FPoint fpworkvar;

	 // Always jam the team# since host could migrate
	//

	planenum = NetGetPlaneIndexFromSlot(ix);

	Planes[planenum].AI.iSide = Slot[planenum].x.chTeam;

	if (Slot[planenum].nLives++ == 0)	//	first life
	{
		if(ix != MySlot)
		{
			pPlaneLoadUpdate[ix] = PlayerPlane;
			iLoadUpdateStation[ix] = 0;
			iLoadUpdateStatus[ix] = 1;
			lLoadUpdateTimer[ix] = 30000;
			if(!iLoadUpdateStatus[MySlot])
			{
				iLoadUpdateStatus[MySlot] = 1;
			}
			if(g_bIAmHost)
			{
				if(!(iNetHasWeather & (1<<ix)))
				{
					iMAISendTo = ix;
					bworkvar = 0;
					if(SnowOn)
					{
						bworkvar |= 0x1;
					}
					if(doLightning)
					{
						bworkvar |= 0x2;
					}
					if(doRain)
					{
						bworkvar |= 0x4;
					}
					fpworkvar.X = WorldParams.Weather;
					fpworkvar.Y = WorldParams.CloudAlt;
					fpworkvar.Z = WorldParams.Visibility;
					NetPutGenericMessage2FPoint(NULL, GM2FP_WEATHER_INFO, fpworkvar, bworkvar);
					iMAISendTo = -1;
				}

				if(iAllowRegen)
				{
					iMAISendTo = ix;
					NetPutGenericMessage1(PlayerPlane, GM_ALLOW_REGEN);
					iMAISendTo = -1;
				}
			}
		}
		else if(!g_bIAmHost)
		{
			NetPutGenericMessage1(PlayerPlane, GM_REQUEST_WEATHER);
		}

		iSlotToPlane[ix] = planenum;

		NetLetKill( ix );	// enable kill tally

		Planes[planenum].Status |= ( PL_ACTIVE | PL_NEED_ATTITUDE );

//		Planes[planenum+100] = Planes[planenum];	// save copy of plane in hanger
		if(ix < MAX_HUMANS)
		{
			NetRegenPlanes[ix] = Planes[planenum];
		}
#if 0   //  Since Planes could be more than 100 do something else.  Also AI_MULTI_ACTIVE SHOULD ALREADY BE SET

		PlaneParams *leadplane;
		if(g_bIAmHost)
		{
			for(planepnt = Planes; planepnt <= LastPlane; planepnt ++)
			{
				if(planepnt->Status & PL_ACTIVE)
				{
					if((planepnt->Status & AL_AI_DRIVEN) && (!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
					{
						leadplane = AIGetLeader(planepnt);
						if((leadplane->Status & AL_AI_DRIVEN) && (!(leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
						{
							planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
						}
					}
				}
			}
		}
#endif

		if(planenum == (PlayerPlane - Planes))
		{
			if(GetRegValueL("mpdebug") == 1)
			{
				hNetDebugFile=_open("netmlog.txt",_O_CREAT | _O_TRUNC | _O_TEXT | _O_WRONLY, _S_IWRITE | _S_IREAD);
				hNetPacketFile=_open("netpslog.txt",_O_CREAT | _O_TRUNC | _O_TEXT | _O_WRONLY, _S_IWRITE | _S_IREAD);
			}
			else
			{
				hNetDebugFile=-1;
				hNetPacketFile=-1;
			}

			lTotalSecs = 0;
			for(cnt = 0; cnt < 4; cnt ++)
			{
				lLastBytes[cnt] = 0;
			}

			for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
			{
				pLastBPPos[cnt] = NULL;
			}
		}

//*******************  if we ever do Arena then we will need to do something like this, but not now.
//		NetSetAIPlanes(planenum);

		if((ix == 0) && (&Planes[planenum] == PlayerPlane))  //  Have first plane start controlling Ground Defenses.
		{
			lAINetFlags1 |= NGAI_ACTIVE;
		}

		if(ix != MySlot)
		{
			NetPutZones(ix);

			lArenaUpdateTimers[ix] = -1;
			pArenaUpdatePlane[ix] = NULL;
			for(planepnt = Planes; planepnt <= LastPlane; planepnt ++)
			{
				if(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)
				{
					lArenaUpdateTimers[ix] = 500;
					pArenaUpdatePlane[ix] = planepnt;
					break;
				}
			}
		}
	}
	else
	{
		if(iAllowRegen)
		{
			NetUnDamage( planenum );		// reset all damage from past life
			NetKillChutes( planenum );

			//		Planes[planenum] = Planes[planenum+100];	// restore plane from hanger
			//  Only do if they allow regens and check carrier stuff.  May need to hack things
			//  To restart player on CAP, or just start them in the air.
			if(ix < MAX_HUMANS)
			{
				if (Planes[planenum].PlaneCopy)
				{
					free(Planes[planenum].PlaneCopy);
					Planes[planenum].PlaneCopy = NULL;
				}

				if (Planes[planenum].Shadow)
				{
					ReleaseShadowEntry(&Planes[planenum].Shadow);
					Planes[planenum].Shadow = NULL;
				}

				OrphanAllPlaneSmoke(&Planes[planenum]);

				Planes[planenum] = NetRegenPlanes[ix];

				if(PlayerPlane != &Planes[planenum])
				{
					if(Planes[planenum].Status & PL_AI_DRIVEN)
					{
						EndCasualAutoPilotPlane(&Planes[planenum]);
					}
				}

				if(Planes[planenum].OnGround == 1)
				{
					Planes[planenum].WorldPosition.Y += (5000.0f * FTTOWU);
					Planes[planenum].BfLinVel.X = Planes[planenum].V = (double) (300 / (FTSEC_TO_MLHR*MLHR_TO_KNOTS));
				}
				else if(Planes[planenum].OnGround == 2)
				{
					if(Planes[planenum].AI.iAICombatFlags1 & AI_HOME_CARRIER)
					{
						MovingVehicleParams *carrier = &MovingVehicles[Planes[planenum].AI.iHomeBaseId];
						Planes[planenum].WorldPosition = carrier->WorldPosition;
						Planes[planenum].WorldPosition.Y += (5000.0f * FTTOWU);
					}
					else
					{
						Planes[planenum].WorldPosition.Y += (5000.0f * FTTOWU);
					}
					Planes[planenum].BfLinVel.X = Planes[planenum].V = (double) (300 / (FTSEC_TO_MLHR*MLHR_TO_KNOTS));
				}
				else
				{
					Planes[planenum].WorldPosition.Y += (2000.0f * FTTOWU);
				}
				Planes[planenum].OnGround = 0;

				Planes[planenum].LastWorldPosition = Planes[planenum].WorldPosition;

				if(Planes[planenum].Status & PL_AI_DRIVEN)
				{
					EndCasualAutoPilotPlane(&Planes[planenum]);
				}
				Planes[planenum].PlaneCopy = NULL;
				Planes[planenum].Shadow = NULL;
				Planes[planenum].AI.iAICombatFlags1 &= ~(AI_CARRIER_LANDING);
				Planes[planenum].AI.iAIFlags2 &= ~(AILANDING);
				Planes[planenum].Brakes = 0;
				if(PlayerPlane == &Planes[planenum])
					DisplayWarning(BRAKE, OFF, 1);
				Planes[planenum].AI.Behaviorfunc = AIFormationFlying;
				Planes[planenum].AI.lVar3 = 0;

				for(cnt = 0; cnt < MAX_PLANE_SMOKE_TRAILS; cnt ++)
				{
					Planes[planenum].Smoke[cnt] = NULL;
				}
			}

//*********  We may want to do something like this for regen in single player so you can comm with wingmen, but 
//*********  this is if we added extra AI wingmen for Arena.
//			NetReconnectAIPlanes(planenum);
		}
		else
		{
			msgOK = 0;
		}
	}

	// Always jam the team# since host could migrate
	//
	Planes[planenum].AI.iSide = Slot[planenum].x.chTeam;

	if ((ix != MySlot) && (msgOK))
		NetRadio( NET_IN_FLIGHT, Slot[planenum].name );
}

/*----------------------------------------------------------------------------
 *
 *	NetGrabPlanes()
 *
 *		Assign command of planes for MultiPlayer.
 *
 */
void NetGrabPlanes()
{
	int cnt;
	 // Activate MY plane, others will activate at first PLANEDATA
	//
	NetSetPlane( MySlot );

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		lArenaUpdateTimers[cnt] = -1;
		pArenaUpdatePlane[cnt] = NULL;
	}

/*
	for (int i=0; i<PLAYERS; i++)
		if (Slot[i].dpid)
			NetSetPlane( i );
*/
}

BOOL WriteToDisk (DWORD dwFlags, char *pszString)
{
	DWORD		dwWritten;
	HANDLE		hFile;

	hFile = CreateFile ("regen.log", GENERIC_READ|GENERIC_WRITE, 0,
		NULL, dwFlags, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		SetFilePointer (hFile, 0, 0, FILE_END);
		WriteFile (hFile, pszString, strlen (pszString), &dwWritten, NULL);
		CloseHandle (hFile);

		return (TRUE);
	}
	return (FALSE);
}


/*----------------------------------------------------------------------------
 *
 *	NetRegenerate()
 *
 *		PlayerPlane has CRASHed, so REGENERATE.
 *
 */
void NetRegenerate()
{
	int planenum = NetGetPlaneIndexFromSlot(MySlot);

	// Delay before rebirth.
	int *mark = &Slot[planenum].nPacks;

	if (*mark == 0)
		*mark = newTime;

	if ((newTime - *mark) < 9000)
		return;

	// This will reset "nPacks" remotely causing a full update.
	NetSetSlotData( MySlot, FALSE );

	NetSetPlane( MySlot );

	InitAvSensorData();
	InitAvionicsModes();
	InitMpdInteractions();
	F18LoadPlayerWeaponInfo();

//	InitCockpit( PlayerPlane );  //  I think this causes bad things on regen.

	WeaponType *ppChaffType			= pChaffType;
	WeaponType *ppFlareType			= pFlareType;
	WeaponType *ppEjectioSeatType	= pEjectioSeatType;
	WeaponType *ppFriendlyChuteType	= pFriendlyChuteType;
	WeaponType *ppEnemyChuteType	= pEnemyChuteType;
	WeaponType *ppGuyOnGroundType	= pGuyOnGroundType;

	AIInitGlobals();

	pChaffType			= ppChaffType;
	pFlareType			= ppFlareType;
	pEjectioSeatType	= ppEjectioSeatType;
	pFriendlyChuteType	= ppFriendlyChuteType;
	pEnemyChuteType		= ppEnemyChuteType;
	pGuyOnGroundType	= ppGuyOnGroundType;

	InitUFC();
	InitUFCMainMenu();
	UFCSetNAVMode();		// probably not needed, (patch safe)
	ResetAARadarToSearchMode();
//	void InitUFCDataMenu(void);

	void InitHUDvars();
	InitHUDvars();

	VKCODE vk;
	vk.vkCode = 0;									// virtual key code (may be joy button)
	vk.wFlags = 0;									// vkCode Type
	vk.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

	void cam_front_view(VKCODE vk);
	cam_front_view(vk);

	void Do_BreakAALock(VKCODE vk);
	Do_BreakAALock(vk);

	F18LoadPlayerWeaponVars();
	NetUnLockWeapons(PlayerPlane - Planes);

	OffDiskTerrainInfo->m_Location = PlayerPlane->WorldPosition;
	OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
	PreLoadLand(PlayerPlane->WorldPosition);
	iHotZoneCheckFlags |= (ZONES_GET_MOVERS|ZONES_SEND_FIRST);
}


//============================================================================
//		WRAPPER COMMUNICATION
//============================================================================
/*
	Function exports:
	----------------
		NetConnect
		NetClose
		NetExit
		NetWrapWnd
		NetPutChat
		NetGetSlots
		NetGetSlotData
		NetSetSlotData
 */

/*----------------------------------------------------------------------------
 *
 *	NetWrapWnd()
 *
 *		Set Wrapper HWND for KickJohn.
 *
 */
void NetWrapWnd( HWND hWnd )
{
	hWrap = hWnd;
}

/*----------------------------------------------------------------------------
 *
 *	KickJohn()
 *
 *		Alert MultiPlayer Arena wrappers of new PlayerData.
 *
 */
void KickJohn( int hot )
{
	if (hWrap)
		PostMessage( hWrap, CWM_MULTI_UPDATE, hot, 0 );
}


//============================================================================
//		MISC MESSAGE PROCESSING
//============================================================================
void NetShowMsg( char *io, DPMSG_F18GENERIC *lpMsg );

/*----------------------------------------------------------------------------
 *
 *	NetSend()  --  hook "dpSend()" for local processing first
 *
 */
int NetSend( int dpID, int flags, void *lpMsg, int size )
{
#ifdef _DEBUG
	NetShowMsg( "OUT", (DPMSG_F18GENERIC *) lpMsg );
	MAINetDebugMessage((DPMSG_F18GENERIC *)lpMsg, dpID, size);
#endif
	DPMSG_F18GENERIC *temp = (DPMSG_F18GENERIC *)lpMsg;

	if((temp->type > NET_BIG_PACKET) || (temp->type == 0))
	{
		return(DP_OK);
	}

	return dpSend( dpID, flags&netMask, lpMsg, size );
}

/*----------------------------------------------------------------------------
 *
 *	NetSendEx ()  --  Unfiltered send functionality
 *
 */
int NetSendEx ( int dpID, int flags, void *lpMsg, int size )
{
#ifdef _DEBUG
	NetShowMsg( "OUT", (DPMSG_F18GENERIC *) lpMsg );
	MAINetDebugMessage((DPMSG_F18GENERIC *)lpMsg, dpID, size);
#endif
	DPMSG_F18GENERIC *temp = (DPMSG_F18GENERIC *)lpMsg;

	if((temp->type > NET_BIG_PACKET) || (temp->type == 0))
	{
		return(DP_OK);
	}

	return dpSend( dpID, flags&netMask, lpMsg, size );
}

//============================================================================
//		PAUSE MESSAGE PROCESSING
//============================================================================

DPMSG_PAUSED  NetPause;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutPause()
 *
 *		Tell the world that I am paused.
 *
 */
void NetPutPause()
{
	NetPause.type = NET_PAUSED;
	NetPause.time = newTime;
//	NetPause.slot = PlayerPlane - Planes;
	NetPause.slot = MySlot;

	//  SimPause Not Toggled Yet.
	if(!SimPause)
	{
		NetPause.slot |= 0x80;
		StopFFForPauseOrEnd();
	}

//	NetSend( DPID_ALLPLAYERS, 0, &NetPause, sizeof(NetPause) );
	dpSendGuaranteed (  DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetPause, sizeof(NetPause) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetPause()
 *
 *		Tell the world that I am paused.
 *
 */
void NetGetPause( DPMSG_PAUSED *Net )
{
//	SimPause ^= 1;

	iAllowMultiPause = 1;

	if(Net->slot & 0x80)
	{
		SimPause = 1;
		StopFFForPauseOrEnd();
	}
	else
	{
		SimPause = 0;
	}

	if (!SimPause)
		JustLeftPausedState = 1;
//	KeySysMessageKey((int)SimPause);

	char szMsg[MAX_PATH];

	*szMsg = NULL;

	if(SimPause)
	{
		if (KeyGetKeyMessage(KSYS_PAUSE,szMsg,(int)SimPause) && *szMsg)
		{
			if(SimPause)
			{
				AICAddAIRadioMsgs(szMsg, -2);
			}
			else
			{
				AICAddAIRadioMsgs(szMsg, -1);
			}
		}
	}
}


//============================================================================
//		MISSILE MESSAGE PROCESSING
//============================================================================

DPMSG_MISSILE  NetMissile;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutMissile()
 *
 *		Tell the world that we fired a missile.
 *
 *		Hook "InstantiateMissile()" and send a guaranteed message.
 *
 */
int NetPutMissile( PlaneParams *P, int station, PlaneParams *pTarget )
{
	if(iMultiCast >= 0)
	{
		return(NetPutMissileSmall(P, station, pTarget));
	}

	NetMissile.type   = NET_MISSILE;
	NetMissile.time   = newTime;
//	NetMissile.slot	  = MySlot;   //  Works OK if not AI.
	NetMissile.slot	  = P - Planes;
	NetMissile.target = pTarget ? (pTarget - Planes) : -1;
	NetMissile.weapon = station;
	NetMissile.weapIX = ++NetWeapIX;	// assign next GUID

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetMissile, sizeof(NetMissile) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetSlotRequest()
 *
 *		Retrieve our new MYSLOT variable value
 *
 */
void NetGetSlotRequest( DPMSG_SLOTREQUEST *Net )
{
	MySlot = Net->newslot;
	Slot[MySlot].x.iSlot = MySlot;
	NetSetSlotData( MySlot, TRUE );

	_ASSERT (hWrap);
	if (hWrap)
		SendMessage (hWrap, CWM_SET_HOST_DPID, Net->dpidHost, 0);
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMissile()
 *
 *		Fire a missile for a remote player.
 *
 */
void NetGetMissile( DPMSG_MISSILE *Net )
{
	PlaneParams *P = &Planes[Net->slot];

	P->AADesignate = (Net->target == -1) ? NULL : &Planes[Net->target];

	InstantiateMissile( P, Net->weapon, NULL,0, P->AADesignate, Net->weapIX );
}


//============================================================================
//		TARGET MESSAGE PROCESSING
//============================================================================

DPMSG_TARGET  NetTarget;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutTarget()
 *
 *		Tell the world that we locked a target.
 *
 *		Hook "SendNetWeaponTargetChange()" and send a guaranteed message.
 *
 *		void SendNetWeaponTargetChange(int weaponid, int targetid)
 */
void NetPutTarget( int weapIX, int target  )
{
	if(iMultiCast >= 0)
	{
		NetPutTargetSmall(weapIX, target);
		return;
	}

	NetTarget.type   = NET_TARGET;
	NetTarget.time   = newTime;
	NetTarget.slot   = MySlot;
	NetTarget.target = target;
	NetTarget.weapIX = weapIX;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetTarget, sizeof(NetTarget) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetTarget()
 *
 *		Lock a target for a remote player.
 *
 *		void UpdateNetWeaponTarget(int weaponid, int targetid)
 *
 */
void NetGetTarget( DPMSG_TARGET *Net )
{
	UpdateNetWeaponTarget( Net->weapIX, Net->target );
}


//============================================================================
//		STRIKE MESSAGE PROCESSING
//============================================================================
DPMSG_STRIKE  NetStrike;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutStrike()
 *
 *		Intercept DamagePlane().
 *
 */
void NetPutStrike( PlaneParams *P )
{
	if(iMultiCast >= 0)
	{
		NetPutStrikeSmall(P);
		return;
	}

	// Assign credit for strike
	NetSetKill( PlayerPlane - Planes, P-Planes );  //  was (MySlot, P-Planes)

	NetStrike.type	 = NET_STRIKE;
	NetStrike.time	 = newTime;
	NetStrike.slot	 = PlayerPlane - Planes;  //  MySlot;
	NetStrike.target = P - Planes;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetStrike, sizeof(NetStrike) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetStrike()
 *
 *		Tally strike for network planes.
 *
 */
void NetGetStrike( DPMSG_STRIKE *Net )
{
	// Assign credit for Strike
	NetSetKill( Net->slot, Net->target );
}


//============================================================================
//		DAMAGE MESSAGE PROCESSING
//============================================================================
DPMSG_DAMAGE  NetDamage;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutDamage()
 *
 *		Intercept DestroyPlaneItem().
 *
 */
void NetPutDamage( PlaneParams *P, long itemid, int critical )
{
	if (netCall)  return;	// don't recurse RPC

	if(iMultiCast >= 0)
	{
		NetPutDamageSmall(P, itemid, critical);
		return;
	}

	// Assign credit for strike

	//  I removed this since we can't be sure where hit came from with delayed damage
	//  If we REALLY need it I can work something out, but with every strike being
	//  sent I'd rather not.

//	NetSetKill(iWeaponFiredBy ,P-Planes, 1 );	// F15v102f compatibility  //  was (MySlot, P-Planes, 1)

	// RPC: Remote::DestroyPlaneItem()

	NetDamage.type	 = NET_DAMAGE;
	NetDamage.time	 = newTime;
	NetDamage.slot	 = iWeaponFiredBy;
	NetDamage.target = P - Planes;
	NetDamage.system = itemid;
	NetDamage.status = critical;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetDamage, sizeof(NetDamage) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDamage()
 *
 *		Inflict damage to network planes.
 *
 */
void NetGetDamage( DPMSG_DAMAGE *Net )
{
	// Assign credit for Strike

	//  I removed this since we can't be sure where hit came from with delayed damage
	//  If we REALLY need it I can work something out, but with every strike being
	//  sent I'd rather not.

//	NetSetKill( Net->slot, Net->target, 1 );	// F15v102f compatibility

	netCall = 1;	// prevent recursion for remote procedure calls

	DestroyPlaneItem( &Planes[Net->target], Net->system, Net->status );

	netCall = 0;
}


//============================================================================
//		CRASH MESSAGE PROCESSING
//============================================================================
DPMSG_CRASH  NetCrash;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutCrash()  --  *** This version only used for CRASHes ***
 *
 *		Intercept CrashPlane().
 *
 */
int NetPutCrash( PlaneParams *P, int flight_status, int failed_system )
{
	if (netCall)  return TRUE;	// don't recurse RPC

	if(iMultiCast >= 0)
	{
		return(NetPutCrashSmall(P, flight_status, failed_system));
	}

	 // Allow DEATH SPIRAL to crash locally.
	//
	if (P->Status & PL_AI_DRIVEN  )  return TRUE;

	 // Don't crash network planes.  Let the owner do it.
	//
	if (P->Status & PL_COMM_DRIVEN)  return FALSE;

	// I crashed, log the kill to the last one to hit me.
	NetLogKill( MySlot );

	// RPC: Remote::CrashPlane()

	NetCrash.type	= NET_CRASH;
	NetCrash.time	= newTime;
	NetCrash.slot	= PlayerPlane - Planes;  //  MySlot;
	NetCrash.system = failed_system;
	NetCrash.status = flight_status;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetCrash, sizeof(NetCrash) );

	return TRUE;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetCrash()
 *
 *		Inflict Crash to network planes.
 *
 */
void NetGetCrash( DPMSG_CRASH *Net )
{
	// Log the kill
	NetLogKill( Net->slot );

	netCall = 1;	// prevent recursion for remote procedure calls

	CrashPlane( &Planes[Net->slot], Net->status, Net->system );

	netCall = 0;
}


//============================================================================
//		DECOY MESSAGE PROCESSING
//============================================================================
DPMSG_DECOY  NetDecoy;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutDecoy()
 *
 *		Tell the world that we fired a Decoy.
 *
 *		Hook "InstantiateCounterMeasure()" and send a guaranteed message.
 *
 */
int NetPutDecoy( PlaneParams *P, int cmtype )
{
	if(iMultiCast >= 0)
	{
		return(NetPutDecoySmall(P, cmtype));
	}

	NetDecoy.type   = NET_DECOY;
	NetDecoy.time   = newTime;
	NetDecoy.slot	= P - Planes;
	NetDecoy.cmtype = cmtype;
	NetDecoy.weapIX = ++NetWeapIX;	// assign next GUID

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetDecoy, sizeof(NetDecoy) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDecoy()
 *
 *		Fire a Decoy for a remote player.
 *
 */
void NetGetDecoy( DPMSG_DECOY *Net )
{
	PlaneParams *P = &Planes[Net->slot];

	if(Net->cmtype >= 16)
	{
		InstantiateCounterMeasure( P, (int)(Net->cmtype - 16), -1, Net->weapIX );
	}
	else
	{
		InstantiateCounterMeasure( P, CHAFF_FLARE, Net->cmtype, Net->weapIX );
	}
}


//============================================================================
//		EJECT MESSAGE PROCESSING
//============================================================================
DPMSG_EJECT  NetEject;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	NetPutEject()
 *
 *		Tell the world that we Ejected.
 *
 *		Hook "InstantiateEjectionSeat()" and send a guaranteed message.
 *
 */
int NetPutEject( PlaneParams *P )
{
	if(iMultiCast >= 0)
	{
		return(NetPutEjectSmall(P));
	}

	NetEject.type   = NET_EJECT;
	NetEject.time   = newTime;
	NetEject.slot	= P - Planes;
	NetEject.weapIX = ++NetWeapIX;	// assign next GUID

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetEject, sizeof(NetEject) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetEject()
 *
 *		Eject a remote player.
 *
 */
void NetGetEject( DPMSG_EJECT *Net )
{
	PlaneParams *P = &Planes[Net->slot];

	InstantiateEjectionSeat( P, 1, 0, NULL, Net->weapIX );
}


//============================================================================
//		PLANE DATA MESSAGE PROCESSING
//============================================================================
DPMSG_PLANEDATA  NetPut;	// static packet for sending

/*----------------------------------------------------------------------------
 *
 *	### float vs. double ###
 *
 *	To save space in the PLANEDATA packet, we send WorldPosition as a float.
 *	This gives a resolution of 1/8th WU as follows:
 *
 *	World is 64x64 tile blocks of 86x86 tiles @ 1/2 Km per tile
 *	1/2Km * 1000m * 100cm / 2.54in / 12ft / 5.7435WU = 1,572,015WU wide/high
 *
 *	floats are 1+23 bits mantissa, next power of 2 above 1,572,015 is 2**21
 *	which leaves 3 bits for binary fraction, ie. 1/8ths
 */

/*----------------------------------------------------------------------------
 *
 *	NetTEWSSet()
 *
 */
void NetTEWSSet()
{
	for (int i=0; i<PLAYERS; i++)
	{
		if(NetGetPlaneIndexFromSlot(i, 1) == -1)
			continue;

		PlaneParams *P = &Planes[NetGetPlaneIndexFromSlot(i)];

		P->AI.iAIFlags2 &= ~AI_RADAR_PING;	// *** init: reset the PING bit ***

		if (P == PlayerPlane)  continue;

		if (!(P->Status & PL_ACTIVE))  continue;

		if (P->FlightStatus & (PL_STATUS_CRASHED|PL_OUT_OF_CONTROL))  continue;

		if (!(P->AI.iAIFlags1 & AIRADARON))  continue;

		if(P->Status & PL_AI_DRIVEN)	continue;

		if(P->OnGround)	continue;

		if (P->AI.iAIFlags2 & AILOCKEDUP)
		{
			 if (P->AI.AirTarget == PlayerPlane)  P->AI.iAIFlags2 |= AI_RADAR_PING;
			 continue;
		}

		if((P->AI.iSide == PlayerPlane->AI.iSide) && (iMShowFriend == 0))  continue;

		FPointDouble tVector;	// make a vector from bogey -> me
		tVector  = PlayerPlane->WorldPosition;
		tVector -= P->WorldPosition;

		double dist = tVector.Normalize();

		if ( dist > 80*NMTOWU)  continue;

		// Dot with bogey's heading to get cosine of included angle.
		// cosine(60°) = 0.5, so cos<0.5 is out-of-range

		double dCos = tVector * P->Orientation.I;	// cosine of included angle

		if (dCos < 0.5)  continue;

		FPointDouble target = P->WorldPosition;
		target.Y += 500*FTTOWU;

		if (!AILOS( PlayerPlane->WorldPosition, target ))  continue;

		P->AI.iAIFlags2 |= AI_RADAR_PING;	// *** Ran the gauntlet: set the PING bit ***
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutTEWS()
 *
 */
char NetPutTEWS()
{
	extern RadarInfoType RadarInfo;
	extern CurSelectedTargetInfoType CurTargetInfo;


#if 0
	if (UFC.EMISState || SniffOn)  return -1;

	if (RadarInfo.CurMode == SGL_TRT_MODE
	&&  CurTargetInfo.CurTargetId >= 0)  return CurTargetInfo.CurTargetId+1;
#else

	if (UFC.EMISState || Av.AARadar.SilentOn)  return -1;

	if((Av.AARadar.CurMode == AA_STT_MODE) && (Primary->Id >= 0))
	{
		if(!PrimaryIsAOT(&CurFramePlanes))
		{
			return(Primary->Id + 1);
		}
	}
#endif

	return 0;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetTEWS()
 *
 */
void NetGetTEWS( PlaneParams *P, int radar )
{
	if (radar >= 0) P->AI.iAIFlags1 |=  AIRADARON;	// radar ON
	else			P->AI.iAIFlags1 &= ~AIRADARON;	// radar OFF

	if (radar > 0)	P->AI.iAIFlags2 |=  AILOCKEDUP;	//  STT  ON
	else		   	P->AI.iAIFlags2 &= ~AILOCKEDUP;	//  STT  OFF

	if (radar > 0)	P->AI.AirTarget	 = &Planes[radar-1];
	else			P->AI.AirTarget	 = NULL;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutPlane()
 *
 *		Send my plane's data to other players.
 *
 *		OUT-OF-CONTROL planes become AI_DRIVEN and then
 *		perform the DeathSpiral.  Keep sending AI_DRIVEN
 *		packets since these are not guaranteed and we use
 *		the first received packet to log the kill.
 *
 */
void NetPutPlane()
{
	int planenum;

	if((PlayerPlane->Status & PL_AI_DRIVEN) && (PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch))
	{
		return;
	}

#if 1
	if(iMultiCast >= 0)
	{
		NetPutPlaneSmall();
		return;
	}
#endif

	PlaneParams *P = PlayerPlane;

	// First alert of death spiral; ie. AI this frame but not last
//	if ( (   P->Status & PL_AI_DRIVEN)
//	&& !(NetPut.Status & PL_AI_DRIVEN) )
//		NetLogKill( P-Planes );

	NetPut.type = NET_PLANEDATA;
	NetPut.time = newTime;
	NetPut.slot = PlayerPlane - Planes;

	NetPut.Status    			= P->Status;
	NetPut.Flaps	    		= P->Flaps;
	NetPut.Brakes				= P->Brakes;
	NetPut.LandingGear			= P->LandingGear;

	NetPut.StickX				= P->StickX;
	NetPut.StickY				= P->StickY;
	NetPut.Trigger1				= P->Trigger1;
//	NetPut.Trigger2				= P->Trigger2;

	NetPut.GunFireRate	 		= P->GunFireRate;
	NetPut.ThrottlePos	 		= P->ThrottlePos;
	NetPut.RudderPedals			= P->RudderPedals;
	NetPut.SpeedBrakeState		= P->SpeedBrakeState;

	NetPut.InternalFuel			= P->InternalFuel;
	NetPut.WingDropFuel			= P->WingDropFuel;
	NetPut.CenterDropFuel		= P->CenterDropFuel;
	NetPut.AircraftDryWeight	= P->AircraftDryWeight;

	NetPut.TotalFuel			= P->TotalFuel;
	NetPut.TotalMass			= P->TotalMass;
	NetPut.TotalWeight			= P->TotalWeight;

	NetPut.BfLinVelX			= P->BfLinVel.X;
	NetPut.HeightAboveGround	= P->HeightAboveGround;

	NetPut.WorldPosition		= P->WorldPosition;
/*
	P->DoAttitude( P );	// make sure RPH are current

	NetPut.Roll					= P->Roll;
	NetPut.Pitch				= P->Pitch;
	NetPut.Heading				= P->Heading;
*/
	NetPut.Orientation.I		= P->Orientation.I;
	NetPut.Orientation.J		= P->Orientation.J;
	NetPut.Orientation.K		= P->Orientation.K;

	NetPut.Radar = NetPutTEWS();

	for (int i=0; i<PLAYERS; i++)
	{
		planenum = NetGetPlaneIndexFromSlot(i);

		if(planenum == -1)	continue;
		if (i==MySlot || Slot[planenum].dpid==0)  continue;

		NetPut.netDelta = Slot[planenum].netDelta;

		if(iMultiCast < 0)
		{
			NetSend( Slot[planenum].dpid, 0, &NetPut, sizeof(NetPut) );
		}
		else
		{
			NetAddToBigPacket(i, &NetPut, sizeof(NetPut) );
		}
	}

//	NetSend( DPID_ALLPLAYERS, 0, &NetPut, sizeof(NetPut) );

	cSend++;
}

/*----------------------------------------------------------------------------
 *
 *	NetSetDeath()
 *
 *		Initiate the DEATH SPIRAL for network planes and log the kill.
 *
 */
void NetSetDeath( PlaneParams *P )
{
	// Log the kill
	NetLogKill( P-Planes );

	P->Status		   &= ~AL_COMM_DRIVEN;
	P->Status		   |=  AL_AI_DRIVEN;
	P->FlightStatus	   |= PL_OUT_OF_CONTROL;
	P->AI.Behaviorfunc	= AIDeathSpiral;
	P->DoControlSim		= CalcF18ControlSurfacesSimple;
	P->DoCASSim			= CalcF18CASUpdatesSimple;
	P->DoForces			= CalcAeroForcesSimple;
	P->DoPhysics		= CalcAeroDynamicsSimple;
	P->UpdateRate		= HIGH_AERO;
	P->dt				= 0;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetPlane()
 *
 *		Get a network plane's data.
 *
 *		Update all inputs and states and save position/time for later.
 *
 *
 *		Discard out-of-sequence packet.
 *
 *		N.B.  "if (this-last < 0)" is NOT the same as "if (this < last)"
 *
 *		The first emits "JS exit", the second emits "JB/JL exit"
 *
 *		Unsigned comparisons break if we bridge 0xFFFF.FFFF
 *  	  Signed comparisons break if we bridge 0x7FFF.FFFF
 *
 *		If time were infinite and hence monotonic this would be true,
 *  	but even 32 bits runs out eventually (49+ days).  So to cover
 *		the case of wrap around we intentionally do a difference and
 *  	then a signed comparison.  This treats the difference as a
 *		signed number (+/- 25 days) that is negative if the new packet
 *  	is older than the last one.
 *
 */
DPMSG_PLANEDATA *NetGet;

void NetGetPlane( DPMSG_PLANEDATA *Net )
{
	int ix;

	NetGet = Net;
	int planenum = Net->slot;

	ix = NetGetSlotFromPlaneIndex(planenum);

	if(planenum == -1)
		return;

	NetSlot     *S = &Slot[planenum];
	PlaneParams *P = &Planes[planenum];

	if((P->Status & PL_AI_DRIVEN) && (P->AI.Behaviorfunc == CTWaitingForLaunch))
	{
		return;
	}

	// Discard out-of-sequence packet.
	if ( (Net->time - S->netTime) < 0)  return;

	// Save most recent Wpos for next "MovePlane"
	S->netTime = Net->time;
	S->netWPos = Net->WorldPosition;

	// Activate the plane at first update.
	if (S->nPacks++ == 0)
	{
		NetSetPlane( ix );
		P->WorldPosition = Net->WorldPosition;
	}

	if (((P->Status & PL_AI_DRIVEN) && (P->FlightStatus & (PL_STATUS_CRASHED|PL_OUT_OF_CONTROL))) && (!(Net->Status & PL_AI_DRIVEN)))
	{
		NetLogKill( planenum );
//		NetRegenerate();
	}

	// AI driven now, no need for data update.
	if (P->Status & AL_AI_DRIVEN)  return;

	// First alert of death spiral  I THINK THIS IS CAUSING BIG PROBLEMS WITH CARRIER STUFF.
//	if (Net->Status & AL_AI_DRIVEN)  NetSetDeath( P );

	// Update GearDown bit
	P->FlightStatus		   &= ~PL_GEAR_DOWN_LOCKED;
	if (Net->LandingGear == 255)
	P->FlightStatus		   |=  PL_GEAR_DOWN_LOCKED;

	P->Flaps				= Net->Flaps;
	P->Brakes				= Net->Brakes;
	P->LandingGear			= Net->LandingGear;

	P->StickX				= Net->StickX;
	P->StickY				= Net->StickY;
	P->Trigger1				= Net->Trigger1;
//	P->Trigger2				= Net->Trigger2;

	P->GunFireRate			= Net->GunFireRate;
	P->ThrottlePos			= Net->ThrottlePos;
	P->RudderPedals			= Net->RudderPedals;
	P->SpeedBrakeState		= Net->SpeedBrakeState;

	P->InternalFuel			= Net->InternalFuel;
	P->WingDropFuel			= Net->WingDropFuel;
	P->CenterDropFuel		= Net->CenterDropFuel;
	P->AircraftDryWeight	= Net->AircraftDryWeight;

	P->TotalFuel			= Net->TotalFuel;
	P->TotalMass			= Net->TotalMass;
	P->TotalWeight			= Net->TotalWeight;

	P->BfLinVel.X			= Net->BfLinVelX;

	P->HeightAboveGround	= Net->HeightAboveGround;

	P->Orientation.I		= Net->Orientation.I;
	P->Orientation.J		= Net->Orientation.J;
	P->Orientation.K		= Net->Orientation.K;
/*
	P->Roll					= Net->Roll;
	P->Pitch				= Net->Pitch;
	P->Heading				= Net->Heading;

	void RPH_to_Orientation( PlaneParams *P );
	RPH_to_Orientation( P );
*/
	NetGetTEWS( P, Net->Radar );

	S->tPing = abs( S->netDelta + Net->netDelta );
}

/*----------------------------------------------------------------------------
 *
 *	RPH_to_Orientation()
 *
 *		Roll:  math ANGLE of horizon to right wing line (+/-180°)
 *				( dip left wing for +roll )
 *
 *		Pitch: math ANGLE of horizon to fuselage axis (+/-90°)
 *				( lift nose for +pitch )
 *
 *		Heading: math ANGLE from compass NORTH (+/-180°)
 *				( compass 350° == heading 10° )
 *
 */
void RPH_to_Orientation( PlaneParams *P )
{
	FPointDouble iVec, jVec, kVec;

	double rSin, rCos;
	double pSin, pCos;
	double hSin, hCos;

	sincosA( &rSin, &rCos, P->Roll    );
	sincosA( &pSin, &pCos, P->Pitch   );
	sincosA( &hSin, &hCos, P->Heading );

	iVec.X = pCos * -hSin;
	iVec.Y = pSin;
	iVec.Z = pCos * -hCos;

	jVec.X = rCos *  hCos;
	jVec.Y = rSin;
	jVec.Z = rCos * -hSin;

	kVec  = iVec;
	kVec %= jVec;

	P->Orientation.I = iVec;
	P->Orientation.J = jVec;
	P->Orientation.K = kVec;
}

/*----------------------------------------------------------------------------
 *
 *	NetMovePlane()
 *
 *		At the end of the physics loop, we adjust the WorldPosition
 *		of all network planes.  Linearly interpolate between our
 *		last physics loop and this physics loop to find the point
 *		corresponding to the latest network input time stamp.
 *		Take a portion of the difference and add it into our
 *		current position.
 *
 *		Network inputs are asynchronous to the physics loop.
 *		Every input updates our net states/inputs and saves the
 *		WorldPosition and time stamp.  So when NetMovePlane()
 *		executes we use the latest XYZ even if multiple packets
 *		came in since our last physics loop.
 *
 *		If we have received no new inputs since our last physics
 *		loop we want to carry our last error term forward.  This
 *		is indicated by a "netTime" of -1, in which case the
 *		"netWPos" value is the error term from last time.
 *
 */
void NetMovePlane( int ix )
{
	int planenum = NetGetPlaneIndexFromSlot(ix);

	if(planenum == -1)
		return;

	NetSlot     *S = &Slot[planenum];
	PlaneParams *P = &Planes[planenum];

	int	T0 = S->oldTime;	// time at last frame
	int	Tn = S->netTime;	// time at net  frame
	int	T1 = newTime;		// time at this frame

	if (T0 && T0!=T1)
	{
		if (Tn)	// compute new error term
		{
			double dt = (double)(Tn - T0) / (T1 - T0);	// parametric value [0..1]

#if 0
			if ((dt<0) || (dt>1))
			{
				char sTxt[120];
				sprintf(sTxt,"Plane %s DT VALUE: %2.2f",S->name,dt);
				AICAddAIRadioMsgs(sTxt,50);
			}
#endif

			if (dt<0) dt=0.0f;  // AND THAT'S THE BOTTOM LINE, BECAUSE STONE COLD SAYS SO!!!
			if (dt>1) dt=1.0f;

			FPointDouble  nuPos;	// WPos interpolated at Tn

			// move oldWPos from T0 to Tn
			nuPos  = P->WorldPosition;
			nuPos -= S->oldWPos;
			nuPos *= dt;
			nuPos += S->oldWPos;	// nuPos = our value interpolated at Tn

			S->netWPos -= nuPos;	// error term: actual minus physics
			S->netDiff  = S->netWPos.QuickLength();
			S->netTime  = 0;		// mark net input as consumed
		}

		S->netWPos *= netKnob;			// scale down error term for feedback

		P->WorldPosition += S->netWPos;	// add in feedback
	}

	S->oldWPos = P->WorldPosition;	// save time/place for next bump
	S->oldTime = newTime;
}

/*----------------------------------------------------------------------------
 *
 *	NetMovePlanes()
 *
 *		Adjust XYZ of all network planes.
 *
 */
void NetMovePlanes()
{
	int planenum;

	if (PlayerPlane->FlightStatus & PL_STATUS_CRASHED)
//		if (GameInfo.dwUser1 & MULTIMASK_REGEN)
		if(iAllowRegen)
			NetRegenerate();

	if(iHotZoneCheckFlags & ZONES_GET_MOVERS)
	{
		MAISetAHotZone((float)PlayerPlane->WorldPosition.X, (float)PlayerPlane->WorldPosition.Z);
	}

	for (int i=0; i<PLAYERS; i++)
	{
		planenum = NetGetPlaneIndexFromSlot(i);

		if(planenum == -1)
			continue;

		PlaneParams *P = &Planes[planenum];

		if (P->FlightStatus & PL_STATUS_CRASHED)  continue;
		if (P->FlightStatus & PL_OUT_OF_CONTROL)  continue;

		if ( (P->Status & PL_ACTIVE) && (P->Status & PL_COMM_DRIVEN) )
			NetMovePlane( i );

//		SRE commented out 11/23/98, not quite sure why.
//		if (P->Status & PL_ACTIVE)
//			P->WeapLoad[GUN_STATION].Count = 500;
	}
}


//============================================================================
//		CHAT MESSAGE PROCESSING
//============================================================================
DPMSG_CHATTER  NetChat;	// static packet for sending

/////////////////////////
//					   //
//   Local Variables   //
//					   //
/////////////////////////
struct _NETCHANNELINF
{
	char	name[20];
	DPID	dpid;

} g_aNetChannels[CHANNELS];

int g_nNumChannels;		// max number of channels to send messages to
int g_nChanFreq[2];		// char radio's frequency
int XmitChannel;		// channel to transmit text

extern char XmitText[MAX_PATH];		// transmit string

/*----------------------------------------------------------------------------
 *
 *	NetPutChat()
 *
 *		Send a chat message to selected player(s).
 *
 */
void NetPutChat( int dpid, char *chat )
{
#if 0  //  Need to do something to handle groups and "all"
	if(iMultiCast >= 0)
	{
		NetPutChatSmall(dpid, chat);
		return;
	}
#endif

	NetChat.type = NET_CHATTER;
	NetChat.time = newTime;
	NetChat.slot = PlayerPlane - Planes;
	strncpy( NetChat.chat, chat, sizeof(NetChat.chat)-1 );

	NetSend( dpid, 0, &NetChat, sizeof(NetChat) - sizeof(NetChat.chat) + strlen(NetChat.chat) + 1 );
}

/*----------------------------------------------------------------------------
 *
 *	EchoMessage()
 *
 *		ECHO network chat message
 *
 */
void EchoMessage( char *text )
{
	char szMsg[MAX_PATH];

	sprintf( szMsg,"%s: %s", Slot[PlayerPlane - Planes].name, text );
	AICAddAIRadioMsgs( szMsg, 50 );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetChat()
 *
 *		Display a new CHAT message.
 *
 */
void NetGetChat( DPMSG_CHATTER *Net )
{
	static char szMsg[MAX_PATH];

	if((Net->slot >= 0) && (strlen(Slot[Net->slot].name) < 20))
	{
		sprintf( szMsg,"%s: %s", Slot[Net->slot].name, Net->chat );
	}
	else
	{
		strcpy (szMsg, Net->chat);
	}
	AICAddAIRadioMsgs( szMsg, 50 );

	extern int g_iSoundLevelCaution;
	SndQueueSound( 304, 1, g_iSoundLevelCaution );	// "1000pair.wav"

	if (hWrap)  SendMessage( hWrap, CWM_MULTI_CHAT_UPDATE, (WPARAM)szMsg, 0 );
}

void NetGetFtp (DPMSG_GENERIC *pGeneric)
{
	if (hWrap)
		SendMessage (hWrap, CWM_FTP, (WPARAM) pGeneric, 0);
}

/*----------------------------------------------------------------------------
 *
 *	NetRefreshComList()
 *
 *		refresh list of DPID's to communicate
 *
 */
void NetRefreshComList(void)
{
	int planenum;

	g_nNumChannels = 2;

	// first two channels are always: BROADCAST, TEAM(player group)
	strcpy(g_aNetChannels[0].name,"ALL");		/**/ // TODO: get 'ALL' from language file
	g_aNetChannels[0].dpid = DPID_ALLPLAYERS;

	strcpy(g_aNetChannels[1].name,"TEAM");		/**/ // TODO: get 'TEAM' from language file
	g_aNetChannels[1].dpid = DPID_ALLPLAYERS;

	for (int i=0; i<PLAYERS; i++)
	{
		planenum = NetGetPlaneIndexFromSlot(i);

		if(planenum == -1)
			continue;

		if (i==MySlot || Slot[planenum].dpid==0)  continue;

		strcpy( g_aNetChannels[g_nNumChannels].name, Slot[planenum].name );
		strupr( g_aNetChannels[g_nNumChannels].name );
		g_aNetChannels[g_nNumChannels].name[8] = NULL; // UFC allows max 8 chars
		g_aNetChannels[g_nNumChannels].dpid = Slot[planenum].dpid;
		g_nNumChannels++;
	}

	// first radio defaults to broadcast to all
	if (g_nChanFreq[0]>=g_nNumChannels)
		g_nChanFreq[0] = 0;

	// second radio defaults to team
	if (g_nChanFreq[1]>=g_nNumChannels)
		g_nChanFreq[1] = 1;
}


void TransmitText(int nChannel, char *text)
{
	int planenum;

	// check if transmitting text to team
	if (g_nChanFreq[nChannel-1] == 1)
	{
		for (int i=0; i<PLAYERS; i++)
		{
			planenum = NetGetPlaneIndexFromSlot(i);

			if(planenum == -1)
				continue;

			NetSlot *S = &Slot[planenum];
			if (S->dpid==0 || S->x.chTeam != Slot[PlayerPlane - Planes].x.chTeam)  continue;
			NetPutChat(S->dpid,text);
		}
	}
	else
	// send text directly to player or broadcast
	{
		NetPutChat(g_aNetChannels[g_nChanFreq[nChannel-1]].dpid,text);
	}
}


#define	SLOT0_TAKEN		0x00000080
#define SLOT1_TAKEN		0x00008000
#define SLOT2_TAKEN		0x00800000
#define SLOT3_TAKEN		0x80000000
#define	SLOT4_TAKEN		SLOT0_TAKEN
#define SLOT5_TAKEN		SLOT1_TAKEN
#define SLOT6_TAKEN		SLOT2_TAKEN
#define SLOT7_TAKEN		SLOT3_TAKEN

#define SLOT0_VIRGIN	0x0000007F
#define SLOT1_VIRGIN	0x00007F00
#define SLOT2_VIRGIN	0x007F0000
#define SLOT3_VIRGIN	0x7F000000
#define SLOT4_VIRGIN	SLOT0_VIRGIN
#define SLOT5_VIRGIN	SLOT1_VIRGIN
#define SLOT6_VIRGIN	SLOT2_VIRGIN
#define SLOT7_VIRGIN	SLOT3_VIRGIN

void NetEnableNewPlayers (BOOL bEnable)
{
	HRESULT				hResult;
	LPVOID				pData;
	DPSESSIONDESC2 *	pDesc;
	BOOL				bChange = FALSE;

	hResult = dpGetSessionDesc (-1, &pData);
	if (hResult == DP_OK)
	{
		pDesc = (DPSESSIONDESC2 *)pData;

		if (!bEnable && ((pDesc->dwFlags & DPSESSION_NEWPLAYERSDISABLED) == 0))
		{
			pDesc->dwFlags |= DPSESSION_NEWPLAYERSDISABLED;
			bChange = TRUE;
		}
		else if (bEnable && (pDesc->dwFlags & DPSESSION_NEWPLAYERSDISABLED))
		{
			pDesc->dwFlags ^= DPSESSION_NEWPLAYERSDISABLED;
			bChange = TRUE;
		}

		if (bChange)
			dpSetSessionDesc (pDesc);
	}
}

void NetFreeOpenSlot (int nSlot)
{
	HRESULT				hResult;
	LPVOID				pData;
	DWORD				dwUser3, dwUser4;
	DPSESSIONDESC2 *	pDesc;

	hResult = dpGetSessionDesc (-1, &pData);
	if (hResult == DP_OK)
	{
		pDesc = (DPSESSIONDESC2 *)pData;
		dwUser3 = pDesc->dwUser3;
		dwUser4 = pDesc->dwUser4;

		switch (nSlot)
		{
			case 0:
				pDesc->dwUser3 ^= SLOT0_TAKEN;
				break;

			case 1:
				pDesc->dwUser3 ^= SLOT1_TAKEN;
				break;

			case 2:
				pDesc->dwUser3 ^= SLOT2_TAKEN;
				break;

			case 3:
				pDesc->dwUser3 ^= SLOT3_TAKEN;
				break;

			case 4:
				pDesc->dwUser4 ^= SLOT4_TAKEN;
				break;

			case 5:
				pDesc->dwUser4 ^= SLOT5_TAKEN;
				break;


			case 6:
				pDesc->dwUser4 ^= SLOT6_TAKEN;
				break;

			case 7:
				pDesc->dwUser4 ^= SLOT7_TAKEN;
				break;
		}

		// Erase the dpid that set for this slot
		g_adpidPlayers[nSlot] = NULL;

		hResult = dpSetSessionDesc (pData);
	}
}

int NetDispatchNewSlot (DPID dpNewPlayer)
{
	HRESULT				hResult;
	LPVOID				pData;
	DWORD				dwUser3, dwUser4;
	DPSESSIONDESC2 *	pDesc;
	int					nNewId = -1;

	hResult = dpGetSessionDesc (-1, &pData);
	if (hResult == DP_OK)
	{
		pDesc = (DPSESSIONDESC2 *)pData;
		dwUser3 = pDesc->dwUser3;
		dwUser4 = pDesc->dwUser4;

		if ((dwUser3 & SLOT0_TAKEN) == 0)
		{
			if ((pDesc->dwUser3 & SLOT0_VIRGIN) == SLOT0_VIRGIN)
				pDesc->dwUser3 ^= SLOT0_VIRGIN;

			pDesc->dwUser3 |= SLOT0_TAKEN;
			nNewId = 0;
		}
		else if ((dwUser3 & SLOT1_TAKEN) == 0)
		{
			if ((pDesc->dwUser3 & SLOT1_VIRGIN) == SLOT1_VIRGIN)
				pDesc->dwUser3 ^= SLOT1_VIRGIN;

			pDesc->dwUser3 |= SLOT1_TAKEN;
			nNewId = 1;
		}
		else if ((dwUser3 & SLOT2_TAKEN) == 0)
		{
			if ((pDesc->dwUser3 & SLOT2_VIRGIN) == SLOT2_VIRGIN)
				pDesc->dwUser3 ^= SLOT2_VIRGIN;

			pDesc->dwUser3 |= SLOT2_TAKEN;
			nNewId = 2;
		}
		else if ((dwUser3 & SLOT3_TAKEN) == 0)
		{
			if ((pDesc->dwUser3 & SLOT3_VIRGIN) == SLOT3_VIRGIN)
				pDesc->dwUser3 ^= SLOT3_VIRGIN;

			pDesc->dwUser3 |= SLOT3_TAKEN;
			nNewId = 3;
		}
		else if ((dwUser4 & SLOT4_TAKEN) == 0)
		{
			if ((pDesc->dwUser4 & SLOT4_VIRGIN) == SLOT4_VIRGIN)
				pDesc->dwUser4 ^= SLOT4_VIRGIN;

			pDesc->dwUser4 |= SLOT4_TAKEN;
			nNewId = 4;
		}
		else if ((dwUser4 & SLOT5_TAKEN) == 0)
		{
			if ((pDesc->dwUser4 & SLOT5_VIRGIN) == SLOT5_VIRGIN)
				pDesc->dwUser4 ^= SLOT5_VIRGIN;

			pDesc->dwUser4 |= SLOT5_TAKEN;
			nNewId = 5;
		}
		else if ((dwUser4 & SLOT6_TAKEN) == 0)
		{
			if ((pDesc->dwUser4 & SLOT6_VIRGIN) == SLOT6_VIRGIN)
				pDesc->dwUser4 ^= SLOT6_VIRGIN;

			pDesc->dwUser4 |= SLOT6_TAKEN;
			nNewId = 6;
		}
		else if ((dwUser4 & SLOT7_TAKEN) == 0)
		{
			if ((pDesc->dwUser4 & SLOT7_VIRGIN) == SLOT7_VIRGIN)
				pDesc->dwUser4 ^= SLOT7_VIRGIN;

			pDesc->dwUser4 |= SLOT7_TAKEN;
			nNewId = 7;
		}

		// Set the new 'slots taken' fields
		hResult = dpSetSessionDesc (pData);

		// Send new player their slot number
		NetPutSlotRequest ( dpNewPlayer, nNewId);
	}

	return (nNewId);
}

int NetWaitForId (void)
{
	DWORD		dwNow, dwStarted;

	dwStarted = GetTickCount ();

	do
	{
		NetGetAll ();
		dwNow = GetTickCount ();
	}
	while (MySlot == -1 && (dwNow < dwStarted + 90000));

	return (MySlot);
}


int NetCycleChannel(int nChannel)
{
	g_nChanFreq[nChannel-1]++;

	if (g_nChanFreq[nChannel-1] >= g_nNumChannels)
		g_nChanFreq[nChannel-1]  = 0;

	return g_nChanFreq[nChannel-1];
}


char * NetGetChannelName(int nChannel)
{
	return g_aNetChannels[g_nChanFreq[nChannel-1]].name;
}


void NetInitChatVars(void)
{
	g_nChanFreq[0] = 0;
	g_nChanFreq[1] = 1;
	XmitChannel = 0;
	*XmitText = NULL;
}

int NetFindSlot (DPID dpID)
{
	int		i;

	if (!dpID)
		return -1;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_adpidPlayers[i] == dpID)
			return (i);
	}

	return -1;
}

/*
 *      START OF WRAPPER NETWORK SUPPORT FUNCTIONS
 */

/*
void GetHumanPlayerInfo (void ** ppdwTimeLastModified, void **ppPilotList)
{
	*ppdwTimeLastModified = &g_dwHumanListLastModified;
	*ppPilotList = &g_pHumanList;
}

BOOL FAR PASCAL WrapperPlayerEnumProc (DPID dpId,  DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext  )
{
	HUMANPILOT *		pNew = (HUMANPILOT *) calloc (1, sizeof (HUMANPILOT));
	dpSlot 				Slot;
	HUMANPILOT **		ppList = (HUMANPILOT **) lpContext;
	HRESULT				hResult;
	DWORD				dwSize = sizeof (dpSlot);
	DPID				dpIdMe = dpWhoAmID ();
	int					i;

	if (pNew)
	{
		pNew->dpid = dpId;
		strcpy (pNew->szName, lpName->lpszShortNameA);
		strcpy (pNew->szPilotFile, lpName->lpszLongNameA);

		hResult = g_lpDirectPlay->GetPlayerData (dpId, &Slot, &dwSize, 0);
		if (hResult == DP_OK)
		{
			pNew->nFtpFlags = Slot.nFtpFlags;
			pNew->nSlot = Slot.iSlot;

			if (g_bIAmHost)
			{
				if (dpId != dpIdMe)
				{
					for (i = 0; i < 8; i++)
					{
						if (g_adpidPlayers[i] == dpId)
						{
							pNew->nSlot = i;
							break;
						}
					}

					_ASSERT (pNew->nSlot != 0);
				}
			}
		}

		if (!*ppList)
			*ppList = pNew;
		else
		{
			pNew->pNext = *ppList;
			*ppList = pNew;
		}
	}

	return (TRUE);
}
*/

/*
void FreePlayerList (void)
{
	HUMANPILOT *		pList = g_pHumanList;
	HUMANPILOT *		pNext;

	while (pList)
	{
		pNext = pList->pNext;
		free (pList);
		pList = pNext;
	}

	g_pHumanList = NULL;
}
*/
/*
void RecreatePlayerList (void)
{
	g_lpDirectPlay = (IDirectPlay3A *) GetDirectPlayInterface ();

	FreePlayerList ();

	if (g_lpDirectPlay)
	{
		g_lpDirectPlay->EnumPlayers (NULL, WrapperPlayerEnumProc, &g_pHumanList, DPENUMPLAYERS_ALL);
		g_dwHumanListLastModified = GetTickCount ();
	}
}
*/

void NotifyWrappers (WORD wMsg, WPARAM wParm, LPARAM lParm)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, wMsg, wParm, lParm);
}

/*
 *      END OF WRAPPER NETWORK SUPPORT FUNCTIONS
 */

//============================================================================
//		MESSAGE PROCESSING CODE
//============================================================================
/*----------------------------------------------------------------------------
 *
 *	DoSysMsg()
 *
 *		Process a single system message.
 *
 */
void DoSysMsg( NetPack *lpNet )
{
	int					nSlot;
	int					planenum;
	LPDPMSG_GENERIC		lpMsg = (LPDPMSG_GENERIC)lpNet->lpData;

	if (lpNet->size < sizeof(DPMSG_GENERIC))
		return;

#ifdef _DEBUG
	Dprintf( "SYS_IN: time:%d: type=%d \n", GetTickCount(), lpMsg->dwType );
#endif

//	Dprintf( "*** DPsys: msg type %d, length %d,  from %d to %d\n",
//				lpMsg->dwType, lpNet->size, lpNet->idFrom, lpNet->idTo );

	KickJohn( lpMsg->dwType != DPSYS_SETPLAYERORGROUPDATA );

	switch (lpMsg->dwType)
	{
		case DPSYS_HOST:
			NetSetHostFlag (TRUE);
			NetRadio( NET_NEW_HOST );
			break;

		case DPSYS_SESSIONLOST:
			NetRadio( NET_LOST_HOST );
			break;

		case DPSYS_SETSESSIONDESC:
		{
			LPDPMSG_SETSESSIONDESC lp = (LPDPMSG_SETSESSIONDESC)lpMsg;

			NetPutGameInfo( &lp->dpDesc );
			break;
		}

		case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP lp = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;

			NetRadio( NET_JOIN_GAME, lp->dpnName.lpszShortNameA );
			if (g_bIAmHost)
			{
				NetDispatchNewSlot (lp->dpId);
			}

			if (lp->dwPlayerType == DPPLAYERTYPE_PLAYER)
			{
				_ASSERT (hWrap);
				if (hWrap)
					SendMessage (hWrap, CWM_PLAYER_JOINING, NULL, (LPARAM) lp);
			}
			break;
		}

		case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;

			NetRadio( NET_QUIT_GAME, lp->dpnName.lpszShortNameA );

			if (lp->dwPlayerType == DPPLAYERTYPE_PLAYER)
			{
				SendMessage (hWrap, CWM_PLAYER_LEAVING, (WPARAM) lp, NULL);

				if (lp->lpRemoteData &&  lp->dwRemoteDataSize == sizeof(dpSlot))
				{
					dpSlot *S = (dpSlot*)lp->lpRemoteData;

					if(lp->dpId)
					{
						if(S->iSlot == 0)  //  End game if host quits
						{
							iEndGameState = 5;
							GetAsyncKeyState(VK_ESCAPE);
							GetAsyncKeyState(VK_RETURN);
						}
					}

					planenum = NetGetPlaneIndexFromSlot(S->iSlot);
					if((g_bIAmHost) && (planenum != -1))
					{
						for(PlaneParams *planepnt = Planes; planepnt <= LastPlane; planepnt ++)
						{
							if(planepnt->AI.iMultiPlayerAIController == planenum)
							{
								if(planepnt->AI.iAICombatFlags1 & AI_MULTI_REMOVEABLE)
								{
									planepnt->Status = 0;
								}
								else
								{
									planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
									planepnt->AI.iMultiPlayerAIController = PlayerPlane - Planes;
								}
							}
						}
					}

					NetQuitSlot( S->iSlot );
					iSlotToPlane[S->iSlot] = -1;
					Dprintf( "Slot %d reset.\n", S->iSlot );
					if (g_bIAmHost)
						NetFreeOpenSlot (S->iSlot);
				}
				else
				{
					if (g_bIAmHost)
					{
						nSlot = NetFindSlot (lp->dpId);
						if (nSlot != -1)
						{
							if(nSlot == 0)  //  End game if host quits
							{
								iEndGameState = 5;
								GetAsyncKeyState(VK_ESCAPE);
								GetAsyncKeyState(VK_RETURN);
							}

							planenum = NetGetPlaneIndexFromSlot(nSlot);

							if(planenum == -1)
							{
								//  This is bad.
								return;
							}

							for(PlaneParams *planepnt = Planes; planepnt <= LastPlane; planepnt ++)
							{
								if(planepnt->AI.iMultiPlayerAIController == planenum)
								{
									if(planepnt->AI.iAICombatFlags1 & AI_MULTI_REMOVEABLE)
									{
										planepnt->Status = 0;
									}
									else
									{
										planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
										planepnt->AI.iMultiPlayerAIController = PlayerPlane - Planes;
									}
								}
							}

							iSlotToPlane[nSlot] = -1;
							NetFreeOpenSlot (nSlot);
							Dprintf( "Slot %d manually reset.\n", nSlot);
						}
						else
							Dprintf( "Slot not manually reset.\n");
					}
				}
			}
			break;
		}

		case DPSYS_SETPLAYERORGROUPDATA:
		{
		   LPDPMSG_SETPLAYERORGROUPDATA lp = (LPDPMSG_SETPLAYERORGROUPDATA)lpMsg;

			if (lp->dwPlayerType == DPPLAYERTYPE_PLAYER
			&&  lp->lpData
			&&  lp->dwDataSize == sizeof(dpSlot))
			{
				dpSlot *S = (dpSlot*)lp->lpData;
				NetSetSlot( S, lp->dpId );
				Dprintf( "Slot %d update.\n", S->iSlot );
			}
			break;
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	DoAppMsg()
 *
 *		Process a single application message.
 *
 *		The game needs to define its own messages.  For each type a unique
 *		ID should be #defined and its struct typedef'd.  All messages start
 *		with a "DWORD dwType;" and after that it is entirely user specified.
 *		Look in DPMSG_GENERIC to get started.
 *
 *		Drift:  Since we are storing the minimum delta, there is a problem
 *			    when one machine's clock slows down relative to the other.
 *		In this case the slow machine will get a decreasing delta and will
 *		track fine.  The fast machine however will be getting an increasing
 *		delta and hence not update "netDelta".  Hence the slow machine's
 *		packets will appear to come from further and further in the past.
 *		Not good. We need some way to update the fast machine's delta.
 *
 *		Data:  The relative drift between two machines in my office is
 *			   greater than 1 part in 10,000, ie. worse than +/- 0.01% !!!
 *		That is one tick (ms) drift every 10 seconds.  Actually for my
 *		two machines it is more like 1 tick per 8 seconds.  The fast
 *		(in both senses) machine is a P2-233.  The slow machine is a P5-90.
 *
 *
 *		Fix #1:  if (++loop > 100)  netDelta++;
 *
 *			If we haven't gotten a "newDelta" less than or equal to
 *			"netDelta" in 100 inputs, bump "netDelta" by one.
 *
 *			Problem: If the faster (relative) machine is also faster MHz then
 *			it will be getting fewer packets per second and may take almost
 *			a minute for 100 packets to arrive.  Way too slow if we just bump
 *			by one tick.
 *
 *
 *		Fix#2:   if (tNow - tMark > 10*1000)  netDelta++;
 *
 *			Each time we get a "newDelta" less than or equal to "netDelta"
 *			we update "netDelta" and mark the time.  If more than 10 seconds
 *			expires between updates the our delta is stale and we bump it one.
 *
 *			Problem: We still are statically choosing a maximum drift that
 *			we will compensate for, ie. one bump per 10,000 ticks or 0.01%
 *
 *
 *		Fix#3:   if (tNow - tMark > 10*1000)  netDelta = minDelta;
 *
 *			Here we keep a local minimum and if ten seconds goes by without
 *			an update, then we update "netDelta" with the local minimum.
 *			This will allow dynamically handling any amount of drift at the
 *			expense of more jitter.  Choose the timeout to balance keeping
 *			drift managable without throwing away useful sync info.  Even
 *			my bad machines only drift 3 ticks in 21 seconds so lets start
 *			there.
 *
 *
 *		Caveat:  This scheme fails if the time difference between machines
 *				 jitters across INT_MAX (2^^31-1) or 25 days.  We really
 *		need two schemes.  Use INT_MAX and signed integers if the two
 *		machines are within +/- 2^^30ms (+/-12 days) of each other, and
 *		UINT_MAX with unsigned integers	it they are not.
 *
 */
void DoAppMsg( NetPack *lpNet )
{
	 // Cast net packet to our generic data type.
	//
	DPMSG_F18GENERIC *lpData = (DPMSG_F18GENERIC *)lpNet->lpData;

	if (lpNet->size < sizeof(DPMSG_F18GENERIC))
		return; // too small, throw it back

	if (MySlot == -1 && lpData->type != NET_SLOTREQUEST)
		return;

	// NET_CHATTER & Slot request messages are allowed to pass
	if (!g_bInSim)
	{
		switch (lpData->type)
		{
			case NET_SLOTREQUEST:
			case NET_CHATTER:
			case NET_FTP:
			case NET_PREVIEW_MISSION:
			case NET_READY_TO_FLY:
			case NET_START_SIM:
			case NET_FTP_FLAGS:
			case NET_REQUEST_FTP_FLAGS:
			case NET_FTP_TOTAL_BLOCKS:
			case NET_QUERY_MISSION:
			case NET_QUERY_MISSION_RESULT:
				break;

			default:
				return; // Stop processing here! Sim not running yet!
		}
	}

	 //	What's the time difference between NOW and when the packet was sent ?
	//
	int ix		 = lpData->slot;		// sender's slot# --> plane index
	iPacketFrom = ix;  //  this is now the index into the planes structure.
	int tNow	 = GetTickCount();		// current time
	int newDelta = tNow - lpData->time;	// delta T for this packet


	NetSlot   *S = &Slot[ix];

	 // Update our local minimum.
	//
	if (newDelta < S->minDelta)  S->minDelta = newDelta;

	 //	New minimum OR stale minimum ?
	//
	if (S->minDelta <= S->netDelta || tNow-S->tMark > 21*1000)
	{
		S->netDelta = S->minDelta;	// Yes, so update my smallest
		S->minDelta = INT_MAX;		//   reset local min
		S->tMark    = tNow;			//   and time stamp
	}

	 // Now adjust the packet to my time frame reference.
	//
	lpData->time += S->netDelta;

#ifdef _DEBUG
	NetShowMsg( "IN", lpData );
#endif

	switch (lpData->type)
	{
		case NET_PLANEDATA:				NetGetPlane(		(DPMSG_PLANEDATA *)						lpData);	break;
		case NET_CHATTER:				NetGetChat(			(DPMSG_CHATTER   *)						lpData);	break;
		case NET_FTP:					NetGetFtp (			(DPMSG_GENERIC   *)						lpData);	break;
		case NET_PAUSED:				NetGetPause(		(DPMSG_PAUSED    *)						lpData);	break;
		case NET_MISSILE:				NetGetMissile(		(DPMSG_MISSILE   *)						lpData);	break;
		case NET_TARGET:				NetGetTarget(		(DPMSG_TARGET    *)						lpData);	break;
		case NET_STRIKE:				NetGetStrike(		(DPMSG_STRIKE    *)						lpData);	break;
		case NET_DAMAGE:				NetGetDamage(		(DPMSG_DAMAGE    *)						lpData);	break;
		case NET_CRASH:					NetGetCrash(		(DPMSG_CRASH     *)						lpData);	break;
		case NET_DECOY:					NetGetDecoy(		(DPMSG_DECOY     *)						lpData);	break;
		case NET_EJECT:					NetGetEject(		(DPMSG_EJECT     *)						lpData);	break;
		case NET_AI_POS:				NetGetAIPos(   (DPMSG_AI_POS    *)							lpData);	break;
		case NET_MISSILE_GENERAL:		NetGetMissileGeneral((DPMSG_MISSILE_GENERAL *)				lpData);	break;
		case NET_GROUND_LOCK:			NetGetGroundLock((DPMSG_GROUND_LOCK *)						lpData);	break;
		case NET_BOMB_GENERAL:			NetGetBombGeneral((DPMSG_BOMB_GENERAL *)					lpData);	break;
		case NET_AG_MISSILE_GENERAL:	NetGetAGMissileGeneral((DPMSG_AG_MISSILE_GENERAL    *)		lpData);	break;
		case NET_SLOTREQUEST:			NetGetSlotRequest(	(DPMSG_SLOTREQUEST *)					lpData);	break;
		case NET_BIG_PACKET:			NetGetBigPacket((DPMSG_BIG_PACKET *)						lpData);	break;
		case NET_ZONES:					NetGetZones((DPMSG_CONTROL_ZONES *)							lpData);	break;
		case NET_AI_COMMS:				NetGetAIComms((DPMSG_AI_COMMS *)							lpData);	break;
		case NET_AI_HEADING:			NetGetDesiredHeading((DPMSG_DESIRED_HEADING *)				lpData);	break;
		case NET_CAS_DATA:				NetGetCASData((DPMSG_CAS_DATA *)							lpData);	break;
		case NET_GENERIC_1:				NetGetGenericMessage1((DPMSG_GENERIC_1 *)					lpData);	break;
		case NET_GENERIC_2:				NetGetGenericMessage2((DPMSG_GENERIC_2 *)					lpData);	break;
		case NET_GENERIC_3:				NetGetGenericMessage3((DPMSG_GENERIC_3 *)					lpData);	break;
		case NET_GENERIC_4:				NetGetGenericMessage4((DPMSG_GENERIC_4 *)					lpData);	break;
		case NET_DF_UPDATE:				NetGetDogfightUpdate((DPMSG_DOGFIGHT_UPDATE*)				lpData);	break;
		case NET_DF_UPDATE_CPOS:		NetGetDogfightCPosUpdate((DPMSG_DOGFIGHT_UPDATE_CPOS*)		lpData);	break;
		case NET_MISSILE_BREAK:			NetGetMissileBreak((DPMSG_MISSILE_BREAK *)					lpData);	break;
		case NET_WEAPON_G_EXPLODE:		NetGetWeaponGExplode((DPMSG_WEAPON_G_EXPLODE*)				lpData);	break;
		case NET_GENERIC_2_LONG:		NetGetGenericMessage2Long((DPMSG_GENERIC_2_LONG *)			lpData);	break;
		case NET_GENERIC_3_LONG:		NetGetGenericMessage3Long((DPMSG_GENERIC_3_LONG *)			lpData);	break;
		case NET_GENERIC_2_FPOINT:		NetGetGenericMessage2FPoint((DPMSG_GENERIC_2_FPOINT *)		lpData);	break;
		case NET_GENERIC_OBJ_POINT:		NetGetGenericMessageObjectPoint((DPMSG_GENERIC_OBJ_POINT *)	lpData);	break;
		case NET_START_SIM:				NetStartSim((DPMSG_STARTSIM *)								lpData);	break;
		case NET_PREVIEW_MISSION:		NetPreviewMission ((DPMSG_PREVIEW_MISSION *)				lpData);	break;
		case NET_READY_TO_FLY:			NetReadyToFly ((DPMSG_JOINER_READY *)						lpData);	break;
		case NET_VGROUND_LOCK:			NetGetVehicleGroundLock((DPMSG_VGROUND_LOCK *)				lpData);	break;
		case NET_GENERIC_2_FLOAT:		NetGetGenericMessage2Float((DPMSG_GENERIC_2_FLOAT *)		lpData);	break;
		case NET_GENERIC_3_FLOAT:		NetGetGenericMessage3Float((DPMSG_GENERIC_3_FLOAT *)		lpData);	break;
		case NET_FTP_FLAGS:				NetFtpFlags ((DPMSG_FTP_FLAGS *)							lpData);	break;
		case NET_REQUEST_FTP_FLAGS:		NetRequestFtpFlags ((DPMSG_FTP_FLAGS *)						lpData);	break;
		case NET_CHECK_DATA_HACK:		NetGetDBInfo((DPMSG_WEAPON_HACK_CHECK *)					lpData);	break;
		case NET_FTP_TOTAL_BLOCKS:		NetFtpTotalBlocks ((DPMSG_FTP_BLOCKS *)						lpData);	break;
		case NET_QUERY_MISSION:			NetQueryMission ((DPMSG_QUERY_MISSION *)					lpData);	break;
		case NET_QUERY_MISSION_RESULT:	NetQueryMissionResult ((DPMSG_QUERY_MISSION_RESULT*)		lpData);	break;
		case NET_GENERIC_4_LONG:		NetGetGenericMessage4Long((DPMSG_GENERIC_4_LONG *)			lpData);	break;
		default:
			int badtype = lpData->type;
			break;
	}
}


/********  Show rendering ratio  ********
static int time1;
static int time2;
static int FrameRatio;
static int phase;
static int mark;

void NetRatio()
{
	int time = GetTickCount() - mark;
	mark += time;

	if (FrameCount < 100)  return;	// let sim get started

	if (++phase & 1)
		time1 = time;
	else
		time2 = time;

	FrameRatio = time1 * 100 / (time1 + time2);
}
*/

/*----------------------------------------------------------------------------
 *
 *	NetGetAll()
 *
 *		Process all messages in the DirectPlay input queue.
 *
 */
void NetGetAll()
{
	int cnt;
	NetPack *lpNet;

	if((!(g_bInSim)) && (iInGameSlots))
	{
		iInGameSlots = 0;
		for(cnt = 0; cnt < MAXAIOBJS; cnt ++)
		{
			if(cnt < MAX_HUMANS)
			{
				Slot[cnt] = Slotarray[cnt];
			}
			else
			{
				Slot[cnt] = Slotarray[MAX_HUMANS];
			}
		}
	}

	while (lpNet = dpReceive())
	{
		if (lpNet->idFrom == DPID_SYSMSG)
			DoSysMsg( lpNet );
		else
			DoAppMsg( lpNet );

		cRecv++;
	}

	if (g_bInSim)  cPoll++;  else  cSpin++;

/*
	 // Check for undocumented error code from dpReceive()
	//
	HRESULT hr;
	_asm  mov eax,[esp-12]
	_asm  mov hr,eax

	DWORD dwCount = 0;
	IDirectPlay3A *lpDirectPlay3A = *(IDirectPlay3A	**)0x11018134;
	if (g_bInSim)  lpDirectPlay3A->GetMessageCount( dpWhoAmID(), &dwCount );

	if (dwCount || (hr != DPERR_NOMESSAGES))
	{
		char szMsg[256];
		sprintf( szMsg, "DirectPlay::Receive() returned error code %X (%d) w/ %d messages", hr, HRESULT_CODE(hr), dwCount );
		MessageBox( NULL, szMsg, "Rolaids >> critical error ...", MB_OK );
	}
*/
}

/*----------------------------------------------------------------------------
 *
 *	NetWork()
 *
 *		Perform ALL network chores for DoGame().
 *
 *			***  The TAO of NetWork  ***
 *
 *		NetWork() is called at the end of the physics loop.
 *		This is the only window when our time is real to the
 *		outside world and the outside world's time is real to
 *		us.  Our world view says that the physics loop adjusts
 *		our position/forces to make up for the delays incurred
 *		by the last render.  Hence time is now real and thus
 *		comparable to other computers' time.  We do a single
 *		GetTickCount() so that all of our operations are in
 *		sync.
 *
 *		The main message pump calls NetGetAll() prior to DoGame()
 *		so that the physics loop can have the most recent inputs
 *		for the network planes.  We call NetGetAll() again just
 *		in case new data came in while we were physics-ing.
 *		For a little extra overhead (two receives/frame) we have
 *		assured the best possible data to both our operations.
 *
 *		MovePlanes() will adjust our just computed position for
 *		network planes by adding in an error term computed from
 *		the difference between the network input XYZ and our
 *		interpolated XYZ at that time.  If no new network input
 *		since last frame, then we simply carry the previous
 *		error term forward.
 *
 */
void NetWork()
{
	static int putTime = 0;
	int cnt, cnt2;
	int planenum;
	PlaneParams *planepnt;
	long longsec;

	newTime = GetTickCount();	// use same time for all

	// DirectPlay/network seems to bottleneck at 30Hz frame rate 1v1
	// ie. 30 packets in and 30 packets out per second.
	// The number of packets sent/received per frame is Nplayers - 1
	// So, try to limit network traffic to 20 packets per second.

	int nWay = g_nNumChannels - 2;	// branching factor

	if (nWay < netFork)  nWay = netFork;

	for(cnt = 0; cnt < 4; cnt ++)
	{
		lByteTimer[cnt] -= DeltaTicks;
		if(lByteTimer[cnt] < 0)
		{
			lLastBytes[cnt] = lCurrBytes[cnt];
			if(cnt == 3)
			{
				longsec = (lLastBytes[0] > lLastBytes[1]) ? lLastBytes[0] : lLastBytes[1];
				longsec = (lLastBytes[2] > longsec) ? lLastBytes[2] : longsec;
				longsec = (lLastBytes[3] > longsec) ? lLastBytes[3] : longsec;

				if(longsec)
				{
					lTotalSecs ++;
					if(hNetPacketFile != -1)
					{
						char tstr[256];
						sprintf(tstr, "%4d, %4d, %4d, %4d,  MAX:%4d*, GL %ld", lLastBytes[0], lLastBytes[1], lLastBytes[2], lLastBytes[3], longsec, GameLoop);
						_write(hNetPacketFile,tstr,(strlen(tstr)));  // + 1

						sprintf(tstr, "\n");
						_write(hNetPacketFile,tstr,(strlen(tstr)));  // + 1
					}
				}
			}

			lByteTimer[cnt] += 1000;
			if(lCurrBytes[cnt] > lMaxBytes)
			{
				lMaxBytes = lCurrBytes[cnt];
			}
			lCurrBytes[cnt] = 0;
		}
	}

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if((pArenaUpdatePlane[cnt]) && (pArenaUpdatePlane[cnt] <= LastPlane))
		{
			lArenaUpdateTimers[cnt] = lArenaUpdateTimers[cnt] - DeltaTicks;
			if(lArenaUpdateTimers[cnt] < 0)
			{
				iMAISendTo = cnt;
				NetPutFullAIUpdate(pArenaUpdatePlane[cnt]);
				for(planepnt = pArenaUpdatePlane[cnt] + 1; planepnt <= LastPlane; planepnt ++)
				{
					if(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)
					{
						lArenaUpdateTimers[cnt] = 1000;
						pArenaUpdatePlane[cnt] = planepnt;
						break;
					}
				}
				if(planepnt > LastPlane)
				{
					lArenaUpdateTimers[cnt] = -1;
					pArenaUpdatePlane[cnt] = NULL;
				}
				iMAISendTo = -1;
			}
		}
		else
		{
			lArenaUpdateTimers[cnt] = -1;
			pArenaUpdatePlane[cnt] = NULL;
		}
	}

	if ((newTime - putTime) > nWay*netRate)
	{
		NetPutPlane();
		putTime = newTime;
	}

	NetGetAll();
	NetMovePlanes();
	NetTEWSSet();
	NetCheckZones();

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		planenum = NetGetPlaneIndexFromSlot(cnt);
		if(planenum == -1)
			continue;

		if(dwLastBPSent[cnt])
		{
			if((dwMaxPacketDelay + dwLastBPSent[cnt]) < (DWORD)newTime)
			{
				BigPackets[cnt].time = newTime;;

				if(iSlotToPlane[MySlot] >= 0)
					BigPackets[cnt].slot = iSlotToPlane[MySlot];

				if(cnt == MySlot)
				{
					NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &BigPackets[cnt], iBigPacketSize[cnt]);
				}
				else
				{
					NetSend( Slot[planenum].dpid, DPSEND_GUARANTEED, &BigPackets[cnt], iBigPacketSize[cnt]);
				}

				for(cnt2 = 0; cnt2 < 4; cnt2 ++)
				{
					lCurrBytes[cnt2] += iBigPacketSize[cnt];
				}
				lTotalBytes += iBigPacketSize[cnt];

				iBigPacketSize[cnt] = sizeof(DPMSG_BIG_PACKET_HEAD);
				iStartNextPacket[cnt] = 0;
				dwLastBPSent[cnt] = 0;
				BigPackets[cnt].num_packets = 0;
				pLastBPPos[cnt] = NULL;
			}
		}
	}
}


//============================================================================
//		INIT/EXIT MULTI-PLAYER
//============================================================================
/*----------------------------------------------------------------------------
 *
 *	NetInitVars()
 *
 */
void NetInitVars()
{
	char *rate = GetRegValue( "NetRate" );
	if (rate)  netRate = atoi( rate );

	char *fork = GetRegValue( "NetFork" );
	if (fork)  netFork = atoi( fork );

	char *mask = GetRegValue( "NetMask" );
	if (mask)  netMask = atoi( mask );

	char *knob = GetRegValue( "NetKnob" );
	if (knob)  netKnob = atoi( knob ) / 100.0;

	cSend = cRecv = cPoll = cSpin = 0;	// zero counters

	netCall = 0;	// reset RPC semaphore

	NetInitChatVars();
}

/*----------------------------------------------------------------------------
 *
 *	NetConnect()
 *
 *		Create a DirectPlay object and connect
 *		with the requested service provider.
 *		Also begin the async session polling.
 *
 *		N.B.  Make sure input "iSP" maps to NET_IPX, etc.
 */
HRESULT NetConnect( int iSP, void *info )
{
	// Win98 hangs sometimes if we init too many DirectX components
	// too quickly.  So for lobby launches, sleep for a	bit first.
	//
	Sleep(1998);	// let DirectX settle down

	void *lpAddress = dpCreateAddress( iSP, info );
	if (!lpAddress)  return DPERR_NOSERVICEPROVIDER;

	HRESULT hr = dpCreate();
	if FAILED(hr)  return DPERR_UNAVAILABLE;

	hr = dpConnect( lpAddress );
	if FAILED(hr)  return hr;

	NetInitVars();

	MultiPlayer = 1;

	return hr;
}

void SetNetConnectVariables (void)
{
	NetInitVars ();
	MultiPlayer = 1;
}

/*----------------------------------------------------------------------------
 *
 *	NetExit()
 *
 *		Shut down DirectPlay and all network connects.
 *
 */
void NetExit()
{
	 // Show statistics.
	//
	Dprintf( "DirectPlay stats:\n" );
	Dprintf( "   sends:  %d \n", cSend );
	Dprintf( "   recvs:  %d \n", cRecv );
	Dprintf( "   polls:  %d \n", cPoll );
	Dprintf( "   spins:  %d \n", cSpin );

	if (g_bIAmHost)
	{
		g_bIAmHost = FALSE;
		NetFreeOpenSlot (MySlot);
	}

	MySlot = -1;


	NetWrapWnd( NULL );
	MultiPlayer = 0;
	dpExit();

	// Last minute fix.  CRTlib initially zeros Planes[].
	// At sim exit it is explicitely zeroed.  We should
	// really not touch these until NetGrabPlanes(), but
	// am not sure of the implications of initing 8 planes
	// of PL_DEVICE_DRIVEN.  So for now, leave it alone
	// and ensure that we zero all before leaving Mult.
	//
	for (int i=0; i<PLAYERS; i++)  Planes[i].Status = 0;
}

/*----------------------------------------------------------------------------
 *
 *	NetClose()
 *
 *		Close the current game session.
 *
 */
void NetClose( void )
{
	NetWrapWnd( NULL );
//	MultiPlayer = 0;
	dpClose();

	for (int i=0; i<PLAYERS; i++)  Planes[i].Status = 0;
}

/*----------------------------------------------------------------------------
 *
 *	SetBit()
 *
 */
void SetBit( DWORD *lpDWord, int bitmask, int state )
{
	if (state)	*lpDWord |=  bitmask;	//   set the bit
	else		*lpDWord &= ~bitmask;	// clear the bit
}

/*----------------------------------------------------------------------------
 *
 *	UseMultiSettings()
 *
 */
void UseMultiSettings (void)
{
	DWORD	dwUser1 = GameInfo.dwUser1;
	DWORD	dwUser2 = GameInfo.dwUser2;

	DWORD	*lpFlight  = &g_Settings.gp.dwFlight;
	DWORD	*lpWeapons = &g_Settings.gp.dwWeapons;
	DWORD	*lpCheats  = &g_Settings.gp.dwCheats;



/*  Not copied
( set.gp.dwFlight & GP_FLIGHT_REALISTIC_AUTOPILOTS		) 
( set.gp.dwFlight & GP_FLIGHT_GFORCE_BREATH				) 
( set.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL	) 
set.gp.dwCheats, GP_CHEATS_UNLIMITED_AMMO
*/




	 // Class 1: Always FORCE
	//
//	dwUser2 &= ~(MULTIMASK_AMMO | MULTIMASK_LIMITFUEL);


	// Class 2: Always copy from HOST  (Flight physics must match)
	//

	g_Settings.gp.nEnemyAircraft = dwUser1 & MULTIMASK_EAIR_SKILL;
	g_Settings.gp.nGroundDefense = (dwUser1 & MULTIMASK_EGRND_SKILL) >> 2;

	SetBit( lpFlight, GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_TOLERANCE,			dwUser1 & MULTIMASK_TANK_TOL   );
	SetBit( lpFlight, GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_DURATION,			dwUser1 & MULTIMASK_TANK_DUR   );
	bMultiLabels = 0;
	SetBit( lpCheats, GP_CHEATS_AIRCRAFT_LABELS,			dwUser1 & MULTIMASK_A_LABELS   );
	if(dwUser1 & MULTIMASK_A_LABELS)
	{
		bMultiLabels = 1;
	}

	SetBit( lpCheats, GP_CHEATS_GRND_TARG_LABELS,			dwUser1 & MULTIMASK_G_LABELS   );
	if(dwUser1 & MULTIMASK_G_LABELS)
	{
		bMultiLabels |= 2;
	}

	SetBit( lpCheats, GP_CHEATS_MULTI_REARM,			dwUser1 & MULTIMASK_REARM   );

	SetBit( lpFlight, GP_FLIGHT_CRASHES,			dwUser2 & MULTIMASK_CRASHES   );
	SetBit( lpFlight, GP_FLIGHT_AIR_COLLISIONS,		dwUser2 & MULTIMASK_COLLISION );
	//SetBit( lpFlight, GP_FLIGHT_REALISTIC_FLIGHT,	dwUser2 & MULTIMASK_FLIGHT	  );
	SetBit( lpFlight, GP_FLIGHT_WEAPON_WEIGHT_DRAG, dwUser2 & MULTIMASK_WEAPDRAG  );
	SetBit( lpFlight, GP_FLIGHT_REALISTIC_LANDINGS,	dwUser2 & MULTIMASK_LANDINGS  );


	 // Class 3: Only copy from HOST if configuration LOCKED
	//
	SetBit( lpFlight,  GP_FLIGHT_READOUT_BLACKOUT,		dwUser2 & MULTIMASK_REDOUT	  );

	//SetBit( lpWeapons, GP_WEAPON_LIMITED_AMMO,			dwUser2 & MULTIMASK_AMMO	  );
	SetBit( lpCheats, GP_CHEATS_UNLIMITED_AMMO,			!(dwUser2 & MULTIMASK_AMMO)	  );
	SetBit( lpFlight,  GP_FLIGHT_LIMITED_FUEL,			dwUser2 & MULTIMASK_LIMITFUEL );
	SetBit( lpWeapons, GP_WEAPON_REAL_GUN_ACCURACY,		dwUser2 & MULTIMASK_GUNACCY   );
	SetBit( lpWeapons, GP_WEAPON_REAL_BOMB_ACCURACY,	dwUser2 & MULTIMASK_BOMBACCY  );
	SetBit( lpWeapons, GP_WEAPON_REAL_WEAPON_DAMAGE,	dwUser2 & MULTIMASK_WPNDAMAGE );
	SetBit( lpWeapons, GP_WEAPON_REAL_MISSILE_ACCURACY, dwUser2 & MULTIMASK_MISSACCY  );

//	g_Settings.gp.nTEWS = (dwUser2 & MULTIMASK_TEWS) >> 14;


	 // Class 4: Allow local choice.
	//
//	g_Settings.gp.nAARADAR = (dwUser2 & MULTIMASK_AARADAR) >> 12;
//	g_Settings.gp.nAGRADAR = (dwUser2 & MULTIMASK_AARADAR) >> 12;

//	g_Settings.gp.nFLIR =			// not used-leave alone
//	g_Settings.gp.nEnemyAircraft =	// leave alone
//	g_Settings.gp.nGroundDefense =	// leave alone
}


//============================================================================
//=================   D E V E L O P M E N T    C O D E   =====================
//============================================================================

void NetShow( char *type, char *io, int time, int slot, char *buff )
{
	time %= 100000;

	Dprintf( ">>> time:%d >>>  (%2d.%03ds) NET_%s_%s %d <%s> \n",
			GetTickCount(), time/1000, time%1000, type, io,	slot, buff );
}

/////////////////// jjd Multiplayer Simulation Kickoff Code ///////////////////////

void NetStartSim (DPMSG_STARTSIM *pMsg)
{
	extern HWND g_hGameWnd;
	PostMessage (g_hGameWnd, CWM_START_SIM, pMsg->nSimType, pMsg->nReturnScreen);
}

void NetPreviewMission (DPMSG_PREVIEW_MISSION *pMsg)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_PREVIEW_MISSION, (WPARAM) pMsg, NULL);
}

void NetReadyToFly (DPMSG_JOINER_READY *pMsg)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_JOINER_READY, (WPARAM) pMsg, 0);
}

void NetFtpFlags (DPMSG_FTP_FLAGS *pMsg)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_FTP_FLAGS, (WPARAM) pMsg, NULL);
} 

void NetFtpTotalBlocks (DPMSG_FTP_BLOCKS *pMsg)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_FTP_BLOCKS, pMsg->blocks, NULL);
}

void NetQueryMission (DPMSG_QUERY_MISSION *pMsg)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_QUERY_MISSION, (WPARAM) pMsg, NULL);
}

void NetQueryMissionResult (DPMSG_QUERY_MISSION_RESULT *pMsg)
{
	_ASSERT (hWrap);
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_QUERY_MISSION_RESULT, (WPARAM) pMsg, NULL);
}

void NetRequestFtpFlags (DPMSG_FTP_FLAGS *pMsg)
{
	if (hWrap != NULL)
		SendMessage (hWrap, CWM_REQUEST_FTP_FLAGS, (WPARAM) pMsg, 0);
}

/////////////////// End of Multiplayer Simulation Kickoff Code ////////////////

char *guidTxt( int guid )
{
	static char buff[64];

	if (guid < 0)  sprintf( buff, "%d", guid );
	else		   sprintf( buff, "%d/%d", HIWORD(guid), LOBYTE(guid) );

	return buff;
}

/*----------------------------------------------------------------------------
 *
 *	NetShowMsg()
 *
 */
void NetShowMsg( char *io, DPMSG_F18GENERIC *lpMsg )
{
	switch (lpMsg->type)
	{
		char buff[256];

		case NET_MISSILE:
		{
			DPMSG_MISSILE *lpDat = (DPMSG_MISSILE *)lpMsg;
			sprintf( buff, "target=%d, station=%d, weapon=%s", lpDat->target, lpDat->weapon, guidTxt(lpDat->weapIX) );
			NetShow( "MISSILE", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		case NET_TARGET:
		{
			DPMSG_TARGET *lpDat = (DPMSG_TARGET *)lpMsg;
			sprintf( buff, "weapon=%s, target=%X", guidTxt(lpDat->weapIX), lpDat->target );
			NetShow( "TARGET", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		case NET_STRIKE:
		{
			DPMSG_STRIKE *lpDat = (DPMSG_STRIKE *)lpMsg;
			sprintf( buff, "target=%d", lpDat->target );
			NetShow( "STRIKE", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		case NET_DAMAGE:
		{
			DPMSG_DAMAGE *lpDat = (DPMSG_DAMAGE *)lpMsg;
			sprintf( buff, "target=%d, system=%X, status=%X", lpDat->target, lpDat->system, lpDat->status );
			NetShow( "DAMAGE", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		case NET_CRASH:
		{
			DPMSG_CRASH *lpDat = (DPMSG_CRASH *)lpMsg;
			sprintf( buff, "system=%X, status=%X", lpDat->system, lpDat->status );
			NetShow( "CRASH", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		case NET_DECOY:
		{
			DPMSG_DECOY *lpDat = (DPMSG_DECOY *)lpMsg;
			sprintf( buff, "weapon=%s, cmtype=%d", guidTxt(lpDat->weapIX), lpDat->cmtype );
			NetShow( "DECOY", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		case NET_EJECT:
		{
			DPMSG_EJECT *lpDat = (DPMSG_EJECT *)lpMsg;
			sprintf( buff, "weapon=%s", guidTxt(lpDat->weapIX) );
			NetShow( "EJECT", io, lpMsg->time, lpMsg->slot, buff );
			break;
		}

		default:
		{
			break;
			if (*io == 'I')
				Dprintf( ">>> time:%d >>>  NET_IN: type:%d / time:%d / slot:%d \n",
						GetTickCount(), lpMsg->type, lpMsg->time, lpMsg->slot );
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	WMprint()
 *
 *		Debug print a "WM" message by name.
 *
 */
void WMprint( int message )
{
	FILE	*fp;
	int		mnum;
	char	mtxt[66];
	char	line[256];

//	if (!WMtrace)  return;

	fp = fopen( "D:\\MSdev\\include\\WinUser.h", "r" );
	if (fp == NULL)  return;

	while (fgets( line,256, fp ))
	{
		if (sscanf(line, "#define WM_%s 0x%x", mtxt, &mnum ) == 2)
		{
			if (mnum == message)
			{
				Dprintf( "%04X: WM_%s @ %d\n", mnum, mtxt, FrameCount );
				break;
			}
		}
	}
	fclose( fp );
}

#pragma comment(lib, "version.lib")

/*----------------------------------------------------------------------------
 *
 *	GetFileVersion()  --
 *
 *		N.B. - Get the magic cookie from RCfile "Version/Block Header"
 *
 */
int GetFileVersion( char *fname, char *fver )
{
	void  *pData;
	void  *verData;
	int   rval, size;

	pData = NULL;

	rval = ((size = GetFileVersionInfoSize( fname, (DWORD*)&size ))
		&&  (pData = malloc( size ))
		&&	GetFileVersionInfo( fname, 0, size, pData )
		&&	VerQueryValue( pData, "\\StringFileInfo\\040904B0\\FileVersion", &verData, (UINT*)&size )
		&&  memcpy( fver, verData, size ) );

	if (pData)  free( pData );

	return rval;
}

/*----------------------------------------------------------------------------
 *
 *	GetF18Version()  --
 *
 */
int GetF18Version( char *fver )
{
	char fname[256];
	GetModuleFileName( NULL, fname, sizeof(fname) );
	return GetFileVersion( fname, fver );
}

/*----------------------------------------------------------------------------
 *
 *	NetData()  --  put debugging info to the screen
 *
 */
void NetData()
{
#ifdef _DEBUG
	static int time0;		// time at start
	char text[256];
	int line = 0;
	int tempval;
	int myplane;

	if (!time0)  time0 = newTime - 100;

	sprintf( text, "< %s > (%d%%) %d*%dms / frame %d ",
				GameName,
				(int)(netKnob*100.1),
				g_nNumChannels-2, netRate,
				FrameCount );

	GrDrawString( GrBuffFor3D, LgHUDFont, 400,  10*line++, text, HUDColor );

	sprintf( text, "Max %ld, Total %ld ",
				lMaxBytes,
				lTotalBytes);

	GrDrawString( GrBuffFor3D, LgHUDFont, 400,  10*line++, text, HUDColor );

	for (int i=0; i<PLAYERS; i++)
	{
//		NetSlot *S = &Slot[i];
		tempval = iSlotToPlane[i];
		myplane = iSlotToPlane[MySlot];

		if(tempval < 0)
			continue;

		NetSlot *S = &Slot[tempval];

		if (S->dpid == 0)  continue;

		int len = sprintf( text, "%d: %s,%d (%X/%X)", i, S->name, S->nLives, Planes[tempval].Status, Planes[tempval].FlightStatus );

		if (i != MySlot)
		{
			float rdist, rdx, rdy, rdz;
			int head = AIComputeHeadingToPoint( &Planes[myplane], Planes[tempval].WorldPosition, &rdist, &rdx, &rdy, &rdz, 1 );

			extern int ShowID;
			switch (ShowID)
			{
			case 0:  sprintf( text+len, " %dwu, %ddg, %dnm", (int)S->netDiff, head, (int)(rdist * WUTONM) );  break;
			case 1:  sprintf( text+len, " in:%d %dms %d", S->nPacks , S->tPing, S->netDelta );  break;
			case 2:  sprintf( text+len, " net: %7.1f %7.1f %7.1f", S->netWPos.X , S->netWPos.Y, S->netWPos.Z );  break;
			case 3:  sprintf( text+len, " old: %7.1f %7.1f %7.1f", S->oldWPos.X , S->oldWPos.Y, S->oldWPos.Z );  break;
			}
		}

		GrDrawString( GrBuffFor3D, LgHUDFont, 400, 10*line++, text, HUDColor );
	}
#endif _DEBUG
}