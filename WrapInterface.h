#ifndef __WRAPPER_INTERFACE__
#define __WRAPPER_INTERFACE__


/////////////////////////////////////////////////////////////////
// Sim defines passed from the wrappers to the game wnd 
//	on the CWM_STARTSIM message. 
/////////////////////////////////////////////////////////////////

#define	SIM_QUICK				0
#define	SIM_SINGLE				1
#define	SIM_TRAIN				2
#define	SIM_CAMPAIGN			3
#define SIM_MULTI				4
#define SIM_BUILDER				5

/////////////////////////////////////////////////////////////////
// Custom windows messages used across our windows
/////////////////////////////////////////////////////////////////

#define	CWM_START_SIM				(WM_USER + 0x0100)
#define CWM_SIM_DONE				(WM_USER + 0x0101)
#define CWM_EXIT					(WM_USER + 0x0102)
#define CWM_START_WRAPPERS			(WM_USER + 0x0103)
#define	CWM_STOP_WRAPPERS			(WM_USER + 0x0104)
#define CWM_SEGMENT_DONE			(WM_USER + 0x0105)
#define CWM_UPDATE_SCREEN			(WM_USER + 0x0106)
#define CWM_MULTI_UPDATE			(WM_USER + 0x0107)
#define CWM_MULTI_CHAT_UPDATE		(WM_USER + 0x0108)
#define CWM_MAPVISIBLE				(WM_USER + 0x0109)
//#define CWM_PILOT_UPDATE			(WM_USER + 0x010A)
#define	CWM_SAVELOADOUT				(WM_USER + 0x010B)
#define	CWM_LOADLOADOUT				(WM_USER + 0x010C)
#define CWM_DELETELOADOUT			(WM_USER + 0x010D)
#define CWM_FUELTIMER				(WM_USER + 0x010E)
#define CWM_FTP						(WM_USER + 0x010F)
#define CWM_KILL_TIMER				(WM_USER + 0x0110)
#define CWM_ROSTER_UPDATE			(WM_USER + 0x0111)
#define CWM_FTP_FINAL_STATUS		(WM_USER + 0x0112)
#define CWM_PREVIEW_MISSION			(WM_USER + 0x0113)
#define	CWM_JOINER_READY			(WM_USER + 0x0114)
#define CWM_PLAYER_JOINING			(WM_USER + 0x0115)
#define CWM_PLAYER_LEAVING			(WM_USER + 0x0116)
#define CWM_FTP_FLAGS				(WM_USER + 0x0117)
#define CWM_REQUEST_FTP_FLAGS		(WM_USER + 0x0118)
#define CWM_ROSTER_PICTURE_UPDATE	(WM_USER + 0x0119)
#define CWM_SET_RETURN_SCREEN		(WM_USER + 0x0120)
#define CWM_SQUADRON_EDIT_RESULT	(WM_USER + 0x0121)
#define CWM_PILOT_EDIT_RESULT		(WM_USER + 0x0122)
#define CWM_FTP_BLOCKS				(WM_USER + 0x0123)
#define CWM_QUERY_MISSION			(WM_USER + 0x0124)
#define CWM_QUERY_MISSION_RESULT	(WM_USER + 0x0125)
#define CWM_SET_HOST_DPID			(WM_USER + 0x0126)

/////////////////////////////////////////////////////////////////
// Custom function pointer types used to stop & start wrappers
/////////////////////////////////////////////////////////////////

typedef BOOL (CALLBACK *STARTWRAPPROC) (HWND, DWORD);
typedef void (CALLBACK *STOPWRAPPROC)  (DWORD);

// Data structure used for Janes Online Game Center hookups
typedef struct
{
	int			nMedia;				// always tcp for now
	int			nPlayers;			// not needed for now
	int			nPlayerId;			// not needed for now
	int			nUser1;				// not needed for now
	int			nUser2;				// not needed for now
	int			nUser3;				// not needed for now
	int			nUser4;				// not needed for now
	char		szPlayerName[256];	
	char		szData[256];		// for tcp, the ip of host in textstring format
} JOGCPLAYER, *LPJOGCPLAYER;


extern HMODULE			g_hWrapperModule;


#endif 


extern int g_iWrapperFlow;	// MJM 6/14/99 - AND THAT'S THE BOTTOM LINE - BECAUSE STONE COLD SAYS SO!
