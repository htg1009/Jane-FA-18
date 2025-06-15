#ifndef __MULTI_MSG_HEADER_FILE
#define __MULTI_MSG_HEADER_FILE

#include <dplay.h>

//*****************************************************************************************
//*  Message IDs
//*****************************************************************************************
#define NET_PLANEDATA					1
#define NET_CHATTER						2
#define NET_PAUSED						3
#define NET_MISSILE						4
#define NET_TARGET						5
#define NET_DAMAGE						6
#define NET_DECOY						7
#define NET_EJECT						8
#define NET_CRASH						9
#define NET_STRIKE						10
#define NET_AI_POS						11	
#define NET_MISSILE_GENERAL				12
#define NET_GROUND_LOCK					13
#define NET_BOMB_GENERAL				14
#define NET_AG_MISSILE_GENERAL			15
#define NET_SLOTREQUEST					16
#define NET_PLANEDATA_SM				17
#define NET_CHATTER_SM					18
#define NET_MISSILE_SM					19
#define NET_TARGET_SM					20
#define NET_DAMAGE_SM					21
#define NET_DECOY_SM					22
#define NET_EJECT_SM					23
#define NET_CRASH_SM					24
#define NET_STRIKE_SM					25
#define NET_AI_POS_SM					26	
#define NET_MISSILE_GENERAL_SM			27
#define NET_GROUND_LOCK_SM				28
#define NET_BOMB_GENERAL_SM				29
#define NET_AG_MISSILE_GENERAL_SM		30
#define NET_BIG_PACKET					31
#define NET_GROUND_EXPLODE				32
#define NET_GROUND_EXPLODE_SM			33
#define NET_ZONES						34
#define NET_ZONES_SM					35
#define NET_AI_COMMS					36
#define NET_AI_COMMS_SM					37
#define NET_AI_HEADING					38
#define NET_AI_HEADING_SM				39
#define NET_CAS_DATA					40
#define NET_CAS_DATA_SM					41
#define NET_GENERIC_1					42
#define NET_GENERIC_1_SM				43
#define NET_GENERIC_2					44
#define NET_GENERIC_2_SM				45
#define NET_GENERIC_3					46
#define NET_GENERIC_3_SM				47
#define NET_GENERIC_4					48
#define NET_GENERIC_4_SM				49
#define NET_DF_UPDATE					50
#define NET_DF_UPDATE_CPOS				51
#define NET_DF_UPDATE_SM				52
#define NET_DF_UPDATE_CPOS_SM			53
#define NET_MISSILE_BREAK				54
#define NET_MISSILE_BREAK_SM			55
#define NET_WEAPON_G_EXPLODE			56
#define NET_WEAPON_G_EXPLODE_SM			57
#define NET_GENERIC_2_LONG				58
#define NET_GENERIC_2_LONG_SM			59
#define NET_GENERIC_3_LONG				60
#define NET_GENERIC_3_LONG_SM			61
#define NET_GENERIC_2_FPOINT			62
#define NET_GENERIC_2_FPOINT_SM			63
#define NET_GENERIC_OBJ_POINT			64
#define NET_GENERIC_OBJ_POINT_SM		65
#define NET_FTP							66
#define NET_START_SIM					67
#define NET_PREVIEW_MISSION				68
#define NET_READY_TO_FLY				69
#define NET_VGROUND_LOCK				70
#define NET_VGROUND_LOCK_SM				71
#define NET_GENERIC_2_FLOAT				72
#define NET_GENERIC_2_FLOAT_SM			73
#define NET_GENERIC_3_FLOAT				74
#define NET_GENERIC_3_FLOAT_SM			75
#define NET_CREATE_TOKEN_PILOT			76
#define NET_FTP_FLAGS					77
#define NET_REQUEST_FTP_FLAGS			78
#define NET_CHECK_DATA_HACK				79
#define NET_CHECK_DATA_HACK_SM			80
#define NET_FTP_TOTAL_BLOCKS			81
#define NET_QUERY_MISSION				82
#define NET_QUERY_MISSION_RESULT		83
#define NET_BULLET_STRIKE_SM			84
#define NET_GENERIC_4_LONG				85
#define NET_GENERIC_4_LONG_SM			86

typedef struct _DPMSG_F18GENERIC
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
} DPMSG_F18GENERIC;

//**********************************************************************************
//  Multiplayer synchronization // jjd
//**********************************************************************************

// Sent when host has determined recipient has received necessary mission & resource files
// this allows joiner to peruse the wrappers...when done, they will send joiner ready message
typedef struct _DPMSG_PREVIEW_MISSION
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	int		nWrapperScreen;// screen to start mission preview at
} DPMSG_PREVIEW_MISSION;	// 10 bytes

// sent to host by joiner to indicate wrappers have been reviewed, pilot is ready to fly
// pilots will all wait until each has checked in with host, and host will send start sim message
typedef struct _DPMSG_JOINER_READY
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	DPID	dpidJoiner;
} DPMSG_JOINER_READY;	// 6 bytes

// Sent by host when all joiners have previewed mission and responded with 'ready' message
// this causes all participants to actually start the sim.
typedef struct _DPMSG_START_SIM
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	int		nSimType;// mission type
	int		nReturnScreen;// screen to return to when going back to wrappers.
} DPMSG_STARTSIM;	// 14 bytes

// Used by both NET_FTP_FLAGS & NET_REQUEST_FTP_FLAGS
typedef struct _DPMSG_FTP_FLAGS
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	DPID	dpid;	// dpid of sender
	int		nFlags;	// ftp flags of sender
} DPMSG_FTP_FLAGS;

typedef struct _DPMSG_TOTAL_FTP_BLOCKS
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	int		blocks;	// total blocks in mission, resource and dat files combined
} DPMSG_FTP_BLOCKS;

typedef struct _DPMSG_QUERY_MISSION
{
	BYTE	type;			// message ID
	int		time;			// system time stamp (ms)
	BYTE	slot;			// slot/planeid
	DPID	dpidHost;		// host's dpid
	DPID	dpidJoiner;		// destination dpid
	int		nMissionType;	// 0 for single, 1 for campaign
	char	szMission[260];	// mission file name
} DPMSG_QUERY_MISSION;

typedef struct _DPMSG_QUERY_MISSION_RESULT
{
	BYTE	type;			// message ID
	int		time;			// system time stamp (ms)
	BYTE	slot;			// slot/planeid
	DPID	dpidHost;		// destination dpid
	DPID	dpidJoiner;		// sender's dpid
	BOOL	bExists;		// FALSE indicates whole mission needs to be sent, Nonzero indicates just mission chunk needs to be sent
} DPMSG_QUERY_MISSION_RESULT;
 
#endif