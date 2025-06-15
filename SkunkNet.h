/*****************************************************************************
*
*   SkunkNet.h  
*
*	David McKibbin		28Aug97
*
*-----------------------------------------------------------------------------
*	Copyright (c) 1998 by Origin Systems, Inc., All Rights Reserved.
*****************************************************************************/

#ifndef __SKUNKNET_H__
#define __SKUNKNET_H__
#include <dplay.h>

///////////////////////
//					 //
//   Define F18LIB   //
//					 //
///////////////////////

#ifdef  __cplusplus
#define CLINK   extern "C"
#else
#define CLINK
#endif

#ifdef	F18LIB			// to use F18lib.dll, define F18LIB
#define F18API	CLINK __declspec( dllimport )
#else
#ifdef	F18DLL			// to make F18lib.dll, define F18DLL
#define F18API	CLINK __declspec( dllexport )
#else
#define	F18API	CLINK	// to use F18lib.lib, do nothing
#endif
#endif


///////////////////////
//					 //
//   Define Limits   //
//					 //
///////////////////////

#define		MAX_PLAYERS		8

#define		NET_IPX			0
#define		NET_TCPIP		1
#define		NET_MODEM		2
#define		NET_SERIAL		3


/////////////////////////
//					   //
//   NetWork structs   //
//					   //
/////////////////////////

//
//	Generic F18 network packet
//
typedef struct _NetPack
{
	int   idFrom;		// DPID_SYSMSG (0) or player ID
	int	  idTo;
	int	  size;			// size of following data buffer
	void *lpData;		// data buffer
} NetPack;

//
//	Slot broadcast structure
//
typedef struct _dpSlot
{
	int		iSlot;		// slot index [0..7]
	short	sLong;		// longitude, in pels, 0 - ~460
	short	sLat;		// latitude,  in pels, 0 - ~460
	char	chTeam;		// team id
	char	chReady;	// ready to play [0..1]
	int		nAltitude;	// 0 = ground level; ie. airstrip
	int		nFtpFlags;	
} dpSlot;

//
// SlotName/PlayerName access as follows:
//
//		dpSlot *lpSlot = NetGetSlotData( slot_index );
//		char   *lpName = SlotName( lpSlot );
//
#define SlotName( lpSlot )	( (char*) ((lpSlot)+1) )


#ifndef F18DLL
//
//	Master SLOT structure.
//
typedef struct _NetSlot
{
	dpSlot		  x;	// player data broadcast to all

	char   name[20];	// DPSHORTNAMELEN

	int        dpid;	// DirectPlay ID; 0 for empty

	int		lastHit;	// player# to last inflict damage to me

	int		 nLives;	// life# (regeneration)
	int		 nPacks;	// total PlaneParam packets

	int    netDelta;	// MIN( My time - Net time )
	int	   minDelta;	// local minimum
	int       tMark;	// time (ms) "netDelta" last updated

	int		  tPing;	// best round trip ping time (ms)

	int		oldTime;	// last   physics tick, ie. T0
	int		netTime;	//  net   physics tick, ie. Tn
	float   netDiff;	// last distance error in WUs

	FPointDouble  oldWPos;	// WPos at "oldTime"
	FPointDouble  netWPos;	// WPos at "netTime"

} NetSlot;				// sizeof( NetSlot ) = 128

#endif

////////////////////////////
//						  //
//   NetWork prototypes   //
//						  //
////////////////////////////
#ifdef _AFXDLL

int			NetGetSlots( void );
void		NetSetSlotData( int ix, int nGuarantee );
dpSlot *	NetGetSlotData( int ix );
NetSlot	*	NetGetSlotDataEx ( int ix );
void		NetWrapWnd( HWND );
void		NetClose( void );
void		NetExit( void );
			
void		NetPutChat( int dpid, char *chat );

#endif


///////////////////////////
//						 //
//   F18LIB prototypes   //
//						 //
///////////////////////////

/*    
/* F18PLAY.CPP 
.*/   
F18API long dpClose(void);
F18API long dpCreate(void);
F18API int  dp_Threads(void);
F18API long dpConnections(int *,char ***  );
F18API void *dpGetAddress( int );
F18API void *dpCreateAddress( int, void * );
F18API long dpConnect(void *);
F18API long dpSPinfo(void);
F18API long dpSessions(int *,char ***  );
F18API long dpPlayers(int,int *,char ***  );
F18API long dpIndex2ID(int);
F18API long dpID2index(unsigned long);
F18API long dpWhoAmID(void);
F18API long dpWhoAmI(void);
F18API long dpHostSessionEx(void *pDesc,char *pszPlayer, char *pszPilot, LPVOID pData, DWORD dwSize);
F18API long dpJoinSession(int nSession,char *pszPlayer,char *pszPilot, LPVOID pData, DWORD dwSize);
F18API long dpGetSessionDesc(int,void ** );
F18API long dpSetSessionDesc(void *);
F18API long dpGetPlayerName(unsigned long,void ** );
F18API long dpGetPlayerData(unsigned long,void *,unsigned long *);
F18API long dpSetPlayerData(void *,unsigned long, int nGuarantee);
F18API long dpSend(unsigned long,unsigned long,void *,unsigned long);
F18API long dpSendGuaranteed (unsigned long,unsigned long,void *,unsigned long);
F18API NetPack *dpReceive(void);
F18API void dpExit(void);
F18API HRESULT CreatePlayer (char *pszPlayerName, char *pszPilotName);

/*    
/* SKUNKNET.C 
.*/   
F18API void Dprintf(const char *fmt,...);
F18API unsigned int P5time(void);
F18API __int64 P5timex(void);
F18API void ID_CPU(void);

#endif
