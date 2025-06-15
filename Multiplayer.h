#ifndef __F18_MULTIPLAYER_STRUCT_HEADER__
#define __F18_MULTIPLAYER_STRUCT_HEADER__

#include <dplay.h>
#include "FtpSession.h"

#define	QUERY_MISSION_NOT_ASKED		0		// haven't sent msg to joiner to see if file exists yet
#define	QUERY_MISSION_ASKED			1		// sent msg to joiner asking if file exists...waiting for response
#define	QUERY_MISSION_EXISTS		2		// got answer from joiner...file exists
#define	QUERY_MISSION_NONEXISTANT	3		// got answer from joiner...file does not exist
#define MISSION_UPDATE_FILENAME		"update.cnk"

typedef struct _MLT_FTP_LST
{
	DPID				dpid;
	int					nFtpFlags;
	CFtpSession *		pFtpMission;
	CFtpSession *		pFtpDat;
	CFtpSession *		pFtpResource;
	CFtpSession *		pFtpPilot;
	CFtpSession *		pFtpSquadron;
	CFtpSession *		pFtpShipDamage;
	CFtpSession *		pFtpGroundDamage;
	DWORD				dwLastHit;
	DWORD				dwLastFtpFlagRequest;
	BOOL				bNew;
	BOOL				bMissionPreviewSent;
	BOOL				bPilotFileCreated;
	BOOL				bPhotoAdded;
	char				szPlayerName[260];
	char				szPilotName[260];
	char				szFullPilotPath[260];
	int					nSlot;
	BOOL				bReady;
	int					nTotalBlocks;
	int					nBlocksRecd;
	int					nMissionExists;	// from defines above
	_MLT_FTP_LST *		pNext;
} MULTI_FTP_LIST;

typedef struct
{
	DPID		dpid;
	CSprite *	pSprite;
} PARTYPILOT;

extern MULTI_FTP_LIST *	g_pMultiPlayerList;

// Prototypes
extern "C"
{
__declspec(dllimport) void * GetDirectPlayInterface (void);
}

#endif
