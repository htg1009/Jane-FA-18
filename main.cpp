/*****************************************************************************
*	Jane's F/A-18
*
*   Main.cpp  
*-----------------------------------------------------------------------------
*	Copyright (c) 1999 by Electronic Arts, All Rights Reserved.
*****************************************************************************/
#include "F18.h"
#include "crtdbg.h"
#include "resource.h"
#include "WrapScreens.h"
#include "SmkVideo.h"
#include "movplyr.h"
#include "GameSettings.h"
#include "LoopProcess.h"
#include "MultiDefs.h"
#include "MultiSetupDefs.h"
#include "DebugHelper.h"	//iam: Added.

#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __DEMO__
#define REGKEY_CONFIG		"Software\\Jane's Combat Simulations\\F18 Demo\\Config"
#else
#define REGKEY_CONFIG		"Software\\Jane's Combat Simulations\\F18\\Config"
#endif

int InitWindowsStuff(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					  LPSTR lpCmdLine, int nCmdShow);
void WriteDumpLog (char *pszString);
void NetExit(void);
void NetGetAll(void);
void DoGame (void);
BOOL InitSound (void);
void ShutdownSound (void);
void PrepareExecutablePath (void);
void InitGraphicVariables (void);
void NewScreenGrab (LPDIRECTDRAWSURFACE4 lpDDS );
void InitLanguage (LPSTR lpCmdLine);
void InitGameSettings (void);
BOOL IsCDROMOK (void);
BOOL OkCheckCDROM (void);
BOOL VerifyVideoMode (void);
BOOL EstablishRegistrySettings (void);
BOOL ParseParameters (LPSTR lpCmdLine);
int InitVideoDriver (void);
BOOL NetRadioCheckKey (int keyflag, WPARAM wParam);


/***************************************************************************************
 *	Global variables
 **************************************************************************************/

// Game variables
HWND			g_hGameWnd = NULL;
HINSTANCE		g_hGameInst = NULL;
BOOL			g_bInSim = FALSE;
char			g_szExePath[260];
char			g_szResourceFile[260];
int				g_nScreenWidth;
int				g_nScreenHeight;
int				g_nMissionType;	// from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
CSmkVideo *		g_pSmkVideo = NULL;
MoviePlayer *	g_pUVPlayer = NULL;
JOGCPLAYER		g_jogcPlayer;
char *			g_pszVideoDriverList = NULL;
char			g_szCurrentVideoDriver[256];

// Wrapper variables
HINSTANCE		g_hWrapInst = NULL;
STARTWRAPPROC	g_pfnStartWrappers = NULL;
STOPWRAPPROC	g_pfnStopWrappers = NULL;
PROCESSLOOPPROC	g_PfnProcessLoopProcess = NULL;
MULTIOPTIONS	g_MultiOptions;

int g_iWrapperMode=SIM_SINGLE;

// Sim variables
//HWND			g_hSimWnd = NULL;

#pragma optimize("",off)

#ifdef __DEMO__
#define REGKEY_MAIN			"Software\\Jane's Combat Simulations\\F18 Demo\\"
#else
#define REGKEY_MAIN			"Software\\Jane's Combat Simulations\\F18\\"
#endif

#define	VIDDRIVER_3DFX		"3Dfx (Glide)"
#define VIDDRIVER_SW		"Software"
#define REGKEY_VIDDRIVER	"VideoDriver"


char			g_szMissionFileName[260];		// File Name of Current Mission
char			g_szLocationFileName[260];		// Location File

#define		LOCATION_MODE_OFF		0
#define		LOCATION_MODE_EXTERNAL	1
#define		LOCATION_MODE_MB		2

int				g_iLocationMode=LOCATION_MODE_OFF;

void	CleanTempDirectory();

extern void InitDumpLog();
extern void CloseDumpLog();

extern CameraInstance	 OldCamera;
extern int OldCockpitSeat, OldCurrentCockpit,OldCursor;
extern int CursorIsVisible;
extern char OldInteract;
extern int InteractCockpitMode;



extern BOOL g_bActive;



/***************************************************************************************
 *	Func:		WinMain
 *	Purpose:	Coordinate appropriate message processing through 2 message pumps.
 **************************************************************************************/
BOOL ScreenSaverSet;
BOOL PowerSaverSet;

void RestoreScreenSaver()
{
	if (ScreenSaverSet)
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,TRUE,0,0);

	if (PowerSaverSet)
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE,TRUE,0,0);
}

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShow)
{
#ifndef _NAKED_BUILD_
	int			nStartScreen;
	MSG			msg;
	BOOL screen_save_active;
	BOOL power_off_active;

	atexit(RestoreScreenSaver);

	if (SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,0,&screen_save_active,0) && screen_save_active)
	{
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,FALSE,0,0);
		ScreenSaverSet = TRUE;
	}

	if (SystemParametersInfo(SPI_GETPOWEROFFACTIVE,0,&power_off_active,0) && power_off_active)
	{
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE,FALSE,0,0);
		PowerSaverSet = TRUE;
	}


	CoInitialize( NULL );

#ifdef _DEBUG
	int flags;
	flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	flags |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flags);
#endif

	if (!InitRegistryValues())
	{
		WriteDumpLog ("InitRegistryValues Failed! Exiting!\n");
		exit(-1);
	}

	InitDumpLog();

	InitLanguage (lpCmdLine);

	if (!InitVideoDriver ())
	{
		WriteDumpLog ("InitVideoDriver Failed! Exiting!\n");
		exit(-1);
	}

	if (!VerifyVideoMode())
	{
		WriteDumpLog ("VerifyVideoMode Failed! Exiting!\n");
		exit(-1);
	}

	InitGraphicVariables ();

	// Determine path we're executing in so we can find wrappers
	PrepareExecutablePath ();

	// Determine if this session was spawned by the browser
	g_iLocationMode=LOCATION_MODE_OFF;

	nStartScreen = ParseParameters (lpCmdLine);

	CleanTempDirectory();

	while (!IsCDROMOK())
	{
		if (!OkCheckCDROM())
		{
			WriteDumpLog ("CheckCDRom Failed! Exiting!\n");
			exit(-1);
		}
	}

	// Init math & 3D world tables
	InitFMath();
	Init3DSystem();

	InitWindowsStuff (hInst, hPrev, lpCmdLine, nShow);

	if (GetRegValue("sound")==NULL)
	{
		ddDisplayMessage("Registry paths not found");
		DestroyDDSurface();
		WriteDumpLog ("GetRegValue Sound Failed! Exiting!\n");
		exit(-1);
	}

	// Memorize our instance
	g_hGameInst = hInst;

	// Check to see if there is already an instance of our game running.
	// If so, we will bail out. We can also set that window to top if so desired.
	if (GameAlreadyRunning ())
		return (0);

	// Register window classes
	InitInstance ();

	// Create master window to run the whole show
	CreateGameWindow ();

	// Read the game settings to check if we want to see the intro animation
	InitGameSettings ();

	// If not a multi-player web game, determine what screen to start wrappers at
	if (nStartScreen == -1)
		nStartScreen = (g_Settings.misc.dwMisc & (GP_MISC_INTRO_VIDEO|GP_MISC_JANES_VIDEO)) ? WRAP_OPEN_ANIM : WRAP_LEGAL_SCREEN;	//WRAP_MAIN_MENU;

#ifdef __DEMO__
	nStartScreen = WRAP_DEMO_OPENSPLASH;
#endif

	// Post message to our main window to kick off the whole show by starting with the wrappers
	PostMessage (g_hGameWnd, CWM_START_WRAPPERS, nStartScreen, 0);


	// Main message pump
	while (1)
	{
		SndServiceSound ();						// discard spurious sound timers
		SndServiceStream ();

		// process any real time loop processessing in the wrapper state.
		if (!g_bInSim && g_PfnProcessLoopProcess)
			g_PfnProcessLoopProcess();
													  // This was to remove useless timer messages from the Miles sound libs.  ChannelWare needs them to be processed.
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))// && msg.message != WM_TIMER)
		{
			if (msg.message == WM_QUIT)  break;

			// ALT-S Screen capture for the wrappers
			if ( !g_bInSim && msg.message == WM_SYSKEYDOWN && msg.wParam == 'S' && GetAsyncKeyState(VK_MENU)<0)
			{
				NewScreenGrab( NULL );//g_bInSim);
				continue;
			}

			TranslateMessage (&msg);	// takes about 300 machine cycles
			DispatchMessage (&msg);
		}
		else	// no window messages
		{

			if (MultiPlayer)
				NetGetAll();	// service the DirectPlay queue

			if (g_bInSim && g_bActive )
				DoGame ();	// This is where the sim gets it's rendering routine called
			else
			{
				if (g_pUVPlayer)
				{
					g_pUVPlayer->update ();
					g_pUVPlayer->draw ();
				}

				if (g_pSmkVideo)
					g_pSmkVideo->Service ();
			}
		}
	}

	if (MultiPlayer)  NetExit();

	ShutdownSound ();
	CloseFMath();
	CleanTempDirectory();

	CloseDumpLog();

	CloseRegistryValues();

	DestroyWindow (g_hGameWnd);
	g_hGameWnd = NULL;

	CoUninitialize();

#endif	// _NAKED_BUILD_
	return (1);
}

void SaveMultiPlayerSetup (MULTIOPTIONS *pMultiOptions)
{
	if (pMultiOptions)
		memcpy (&g_MultiOptions, pMultiOptions, sizeof (MULTIOPTIONS));
	else
		memset (&g_MultiOptions, 0, sizeof (MULTIOPTIONS));
}

MULTIOPTIONS * GetMultiPlayerSetup (void)
{
	return (&g_MultiOptions);
}

//***********************************************************************************************************************************
void CleanTempDirectory (void)
{
	char				szPath[260];
	char				szDelete[260];
	char *				pszTemp;
	struct _finddata_t	fileData;
	long				lSearch, lResult;


	pszTemp = GetRegValue ("resource");
	if (pszTemp)
	{
		strcpy(szDelete,pszTemp);
		strcat(szDelete,"\\Temp\\");

		// Attempt to create directory in case it isn't there

		strcpy (szPath, pszTemp);
		strcat (szPath,"\\Temp");
		_mkdir (szPath);

		// Delete any .wav files that are still around in the directory
		strcat (szPath, "\\*.wav");
		lSearch = lResult = _findfirst (szPath, &fileData);
		while (lResult != -1)
		{
			sprintf (szPath, "%s%s", szDelete, fileData.name);
			DeleteFile (szPath);
			lResult = _findnext (lSearch, &fileData);
		}
		strcpy (szPath, pszTemp);
		strcat (szPath,"\\Temp"); //iam: Added - function was looking for temp BMPs in /resource instead of /resource/temp

		// Delete any .bmp files that are still around in the directory
		strcat (szPath, "\\*.bmp");
		lSearch = lResult = _findfirst (szPath, &fileData);
		while (lResult != -1)
		{
			sprintf (szPath, "%s%s", szDelete, fileData.name);
			DeleteFile (szPath);
			lResult = _findnext (lSearch, &fileData);
		}
	}
}

//***********************************************************************************************************************************
char *GetCurrentMissionName()
{
	return(g_szMissionFileName);
};

//***********************************************************************************************************************************
void SetCurrentMissionName(char *szFile)
{
	strcpy(g_szMissionFileName,szFile);
}


#define IS_SPACE(c)   ((c) == ' ' || (c) == '\r' || (c) == '\n' || (c) == '\t' || (c) == 'x')

/*****************************************************************************************************************/
// Function:	ParseParameters
// Purpose:		Read command line and determine if we're being launched from browser as result of Janes Online
//				Game Center (jogc)
// NEW AND IMPROVED!!! - Now able to handle mission starts from the command line....tell your friends
//
//
// Returns:		Wrapper screen to start the game up with, or -1 if normal game
//
// Parameter conventions:
//
//		/d /j /l /m /h
//		
//
//		/m mission file name
//
//		if hosting a game:
//			/h tcp /nPILOTNAME							example:	/h tcp /nJohnDugan
//
//		if joining a game:
//			/j tcp IP_ADDRESS_OF_HOST /nPILOTNAME		example:	/j tcp 206.202.15.32 /nJohnDugan
//
//		iam: Added /s : to log display settings
//		iam: Added /f  : to output FNTs in PCX format
/******************************************************************************************************************/
int ParseParameters (LPSTR lpCmdLine)
{
	char		szBuffer[512];
	char *		pszToken;
	int			nDelta;
	int			nLength, j;
	int			iMission=0;
	int			iStartScreen=-1;

	strcpy(szBuffer,lpCmdLine);
	pszToken = strtok(szBuffer,"/");

	// For some reason...C6's version of strtok is broken! It is injecting a NULL in the wrong spot of buffer!
	// This sucks! I'll have to parse the jogc junk by hand SLOWLY!

    while(pszToken)
	{
        switch (*pszToken)
        {
			case 's':
				logDisplaySettings(); //iam: log display settings
			break;
			case 'h':
				strcpy (szBuffer, lpCmdLine);
				nLength = strlen (szBuffer);
				for (j = 0; j < nLength; j++)
					szBuffer[j] = toupper (szBuffer[j]);

				pszToken = strstr (szBuffer, "/H");
				if (!pszToken)
					return -1;
				else
					pszToken += 2;

				pszToken = strstr (pszToken, "TCP ");
				if (!pszToken)
					return -1;
				else
					pszToken += 4;

				g_jogcPlayer.nMedia = PROTOCOL_TCPIP;
				while (isspace (*pszToken))
					pszToken++;

				// get past /n field
				pszToken += 2;
				nDelta = pszToken - szBuffer;
				strcpy (g_jogcPlayer.szPlayerName, lpCmdLine + nDelta);
				g_jogcPlayer.nPlayers = 8;
				// we are the host, so clear host IP out
				g_jogcPlayer.szData[0] = NULL;
				return (WRAP_MULTI_HOST_OPTS);

			case 'j':
				strcpy (szBuffer, lpCmdLine);
				nLength = strlen (szBuffer);
				for (j = 0; j < nLength; j++)
					szBuffer[j] = toupper (szBuffer[j]);

				pszToken = strstr (szBuffer, "/J");
				if (!pszToken)
					return -1;
				else
					pszToken += 2;

				pszToken = strstr (pszToken, "TCP ");
				if (!pszToken)
					return -1;
				else
					pszToken += 4;

				g_jogcPlayer.nMedia = PROTOCOL_TCPIP;
				while (isspace (*pszToken))
					pszToken++;

				nLength = 0;
				while (!isspace (*pszToken))
				{
					g_jogcPlayer.szData[nLength] = *pszToken;
					nLength++;
					pszToken++;
				}
				g_jogcPlayer.szData[nLength] = NULL;

				while (isspace (*pszToken))
					pszToken++;

				// get past /n field
				pszToken += 2;
				nDelta = pszToken - szBuffer;
				strcpy (g_jogcPlayer.szPlayerName, lpCmdLine + nDelta);

				return (WRAP_MULTI_JOIN_OPTS);

			case 'd':
			{
				pszToken++;
				while (IS_SPACE(*pszToken)) pszToken++;

				if (!*pszToken)
					break;

				char sMissionName[260];
				char *tptr = GetRegValue("mission");
				strcpy(sMissionName,tptr);
				strcat(sMissionName,"\\");
				strcat(sMissionName,pszToken);


				int hMissionFile=-1;
				if ((hMissionFile=_open(sMissionName,_O_BINARY | _O_RDONLY)) == -1)
				{
					break;

				}
				strcpy(g_szMissionFileName,sMissionName);

				_close(hMissionFile);

				iStartScreen=WRAP_DEBRIEF;
			}
			break;
			case 'm':
			{
				pszToken++;
				while (IS_SPACE(*pszToken)) pszToken++;

				if (!*pszToken)
					break;

				char sMissionName[260];
				char *tptr = GetRegValue("mission");
				strcpy(sMissionName,tptr);
				strcat(sMissionName,"\\");
				strcat(sMissionName,pszToken);


				int hMissionFile=-1;
				if ((hMissionFile=_open(sMissionName,_O_BINARY | _O_RDONLY)) == -1)
				{
					break;

				}
				strcpy(g_szMissionFileName,pszToken);

				_close(hMissionFile);

				iStartScreen=WRAP_SIM_LAUNCH;
			}
			break;
			case 'l':
				// The location option tells the simulation to load a location file and then
				// put the sim into a special view mode.
				// This mode loads the simulation using a default mission.  The default mission
				// has to have the minimum amount of data needed to start a mission (just the player aircraft.)
				// The sim will then set the first viewpoint to be the first position from the
				// location file.  The simulation will be paused.

				pszToken++;
				while (IS_SPACE(*pszToken)) pszToken++;

				if (!*pszToken)
					break;


				int hMissionFile=-1;
				if ((hMissionFile=_open(pszToken,_O_BINARY | _O_RDONLY)) == -1)
				{
					break;
				}
				strcpy(g_szLocationFileName,pszToken);

				_close(hMissionFile);

				g_iLocationMode=LOCATION_MODE_EXTERNAL;
			break;
        }
		pszToken=strtok(NULL,"/");
    }
	return(iStartScreen);
}


void * GetJOGCPlayer (void)
{
	// If there is a player name then we have a jogc game
	if (g_jogcPlayer.szPlayerName[0])
		return (void *)(&g_jogcPlayer);
	else
		return (NULL);
}

void InitGraphicVariables (void)
{
	g_nScreenWidth = GetSystemMetrics (SM_CXSCREEN);
	g_nScreenHeight =GetSystemMetrics (SM_CYSCREEN);  
	
	if(g_nScreenWidth < 1024)		// DestktopRes GameResTNT(640) ->Voodo(1024) fix
		g_nScreenWidth  = 1024;
	if( g_nScreenHeight < 768)
		g_nScreenHeight =768;

}

void PrepareExecutablePath (void)
{
	char	*pPath;

	// Store off path so we can load wrapper.dll explicitly
	GetModuleFileName (NULL, g_szExePath, 260);
	pPath = strrchr (g_szExePath, '\\');

	// Terminate filename so only path remains (without trailing backslash)
	if (pPath)
		*pPath = NULL;

	SetCurrentDirectory( g_szExePath );
}

//***********************************************************************************************************************************
int InitWindowsStuff(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					  LPSTR lpCmdLine, int nCmdShow)
{
	#define	PixelDepth 16
	int iReturnCode=0;

	hWinInstance = hInstance;
	nWinCmdShow = nCmdShow;

#if (_DEBUG)
	SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
#else
	SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
#endif

	return(iReturnCode);
}


/***************************************************************************************
 *	Func:		GameWndProc
 *	Purpose:	Serve as game's master wndproc
 **************************************************************************************/

LRESULT CALLBACK GameWndProc (HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParm)
{
	PAINTSTRUCT		ps;
	static DWORD		dwWrapperState = 0;	// Stores next screen wrappers will come back to

	switch (uMsg)
	{
		case WM_ACTIVATEAPP:
            // Pause if minimized or not the top window
            g_bActive = (wParm == WA_ACTIVE) || (wParm == WA_CLICKACTIVE);
			if(!g_bActive && IsWindowVisible (g_hGameWnd) )
				ShowWindow (g_hGameWnd, SW_MINIMIZE);

            return 0L;

		case WM_PAINT:
			BeginPaint (hWnd, &ps);
			EndPaint   (hWnd, &ps);
			return 0;

		case WM_LBUTTONDOWN:
			return 0;

		case WM_SYSKEYDOWN:

			if (NetRadioCheckKey (0, wParm))
				return 0;

			if (!KeyExcecuteKey( wParm ) ) // Trap ALT-key combinations
			{
				// NetWork debugging
				switch(wParm)
				{extern double netKnob;
				 extern void ShowUp();
				 extern void ShowDn();
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':  netKnob = (wParm - '0') / 10.0;  break;
					case 'D':  GetKeyState( VK_SHIFT )<0 ? ShowUp() : ShowDn();  break;
				}
			}
			return 0;

		case WM_KEYUP:
			AICCheckAIKeyUp(0, wParm);
			return 0;

		case WM_CHAR:
			NetRadioCheckKey (1, wParm);
			return 0;

		case WM_KEYDOWN:
			AICCheckAIComm(0, wParm);

			if (NetRadioCheckKey (0, wParm))
				return 0;

			if (!KeyExcecuteKey( wParm ) )
			{
				switch(wParm)
				{
					case VK_ESCAPE:
						if(iEndGameState)
						{
							if((!MultiPlayer) || (iEndGameState < 4))
							{
								iEndGameState = 0;
								JustLeftPausedState = 1;
								RestorePostExitView();
							}

						}
						else
						{
							if((!MultiPlayer) || (iEscOpt == 0) || (iEscOpt > 2) || ((iEscOpt == 1) && (GetAsyncKeyState(VK_SHIFT)<0)))
							{
								OldCockpitSeat    = CockpitSeat;
								OldCurrentCockpit = CurrentCockpit;
								memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));
								OldCursor = CursorIsVisible;
								OldInteract = InteractCockpitMode ;


								iEndGameState = 4;

								if(MultiPlayer)
									NetSendPlayerExit(PlayerPlane);
							}
						}
						break;

				}
			}
			return 0;


		//GAME WND OLD
		case WM_SYSCOMMAND:
			if (wParm==SC_SCREENSAVE)
				return 0;
			break;

		case WM_CLOSE:
			DestroyWindow (hWnd);
			return 0;

		case CWM_START_WRAPPERS:
			// Start the wrappers
			LoadWrappers (wParm);	// usually wParm = WRAP_MAINMENU
			return 0;

		case CWM_STOP_WRAPPERS:
			// If we exited while in the wrappers, free the wrappers library
			FreeWrappers ();
			return 0;

		case CWM_EXIT:
			// User wants to QUIT altogether
			if (g_pszVideoDriverList)
			{
				free (g_pszVideoDriverList);
				g_pszVideoDriverList = NULL;
			}

			ShowWindow (hWnd, SW_MAXIMIZE);

			if (g_hWrapInst != NULL)
				FreeWrappers ();

			if (g_bInSim)
				StopSim ();

			PostQuitMessage (1);
			return 0;

		case CWM_START_SIM:	// Posted from wrappers

			// Note next wrapper screen to return back to
			dwWrapperState = lParm;

			// Note type of mission we will be flying
			g_nMissionType = wParm;

			// Perform switch
			LeaveWrappersEnterSim (g_nMissionType);
			return 0;

		case CWM_SIM_DONE:	// Posted from sim
			// Do whatever prep is needed here
			// ***NOTE*** wParm is flag indicating status of game
			// ***NOTE*** lParm could be pointer to whatever else is needed from sim
			LeaveSimEnterWrappers (dwWrapperState);
			return 0;

		default:
			break;
	}

	return (DefWindowProc (hWnd, uMsg, wParm, lParm));
}

/***************************************************************************************
 *	Func:		LeaveWrappersEnterSim
 *	Purpose:	Coordinate shutting down wrappers, loading sim, as well
 *				as putting up black 'splash-screen' to make the transition smooth.
 **************************************************************************************/
void LeaveWrappersEnterSim (DWORD dwSimState)
{
	char	szOutput[260];

	sprintf (szOutput, "Leaving Wrappers...state = %d\n", dwSimState);
	WriteDumpLog (szOutput);

	// Put our black window up to serve as 'curtain'
	
	//SetWindowPos (g_hGameWnd, HWND_TOP, 0, 0, g_nScreenWidth, g_nScreenHeight, SWP_SHOWWINDOW);
	//SetWindowPos (g_hGameWnd, HWND_TOPMOST, 0, 0, g_nScreenWidth, g_nScreenHeight, SWP_SHOWWINDOW);
	ShowWindow (g_hGameWnd, SW_SHOW);//SW_MAXIMIZE);
	UpdateWindow(g_hGameWnd);

	// make sure any loaded textures will stay loaded after we switch
	HideTexturesFromD3DRelease();

//	ShowWindow (g_hGameWnd, SW_MAXIMIZE);

	// Free up wrapper engine & start sim up.
	FreeWrappers ();

	WriteDumpLog ("Wrappers Freed...Starting Sim\n");

	// Load up sim
	StartSim (NULL);

	WriteDumpLog ("StartSim Completed\n");

	// Remove 'curtain' now that sim is ready
	//ShowWindow (g_hGameWnd, SW_HIDE);
}

/***************************************************************************************
 *	Func:		LeaveSimEnterWrappers
 *	Purpose:	Coordinate shutting down sim, loading wrappers, as well
 *				as putting up black 'splash-screen' to make the transition smooth.
 **************************************************************************************/
void LeaveSimEnterWrappers (DWORD dwWrapperState)
{
	char			szOutput[260];

	sprintf (szOutput, "Leaving Sim, Entering Wrappers; Wrapper = %d\n", dwWrapperState);
	WriteDumpLog (szOutput);

	// Put our black window up to serve as 'curtain'
	//ShowWindow (g_hGameWnd, SW_MAXIMIZE);
	//SetWindowPos (g_hGameWnd, HWND_TOPMOST, 0, 0, g_nScreenWidth, g_nScreenHeight, SWP_SHOWWINDOW);
	ShowWindow (g_hGameWnd, SW_SHOW);//SW_MAXIMIZE);
	UpdateWindow(g_hGameWnd);


	// Stop sim loop
	StopSim ();

	WriteDumpLog ("StopSim Completed\n");

	// Restart and come into wrappers at screen we saved off before.
	LoadWrappers (dwWrapperState);

	WriteDumpLog ("LoadWrappers Completed\n");

	ShowWindow (g_hGameWnd, SW_HIDE);
}

/***************************************************************************************
 *	Func:		GameAlreadyRunning
 *	Purpose:	Find a window with our class and window titles already running.
 *				If found, that window will be activated, and we will kill ourselves,
 *				to ensure no more than 1 game is running at once.
 *	Returns:	TRUE if a game is already running, FALSE if not.
 **************************************************************************************/

BOOL GameAlreadyRunning (void)
{
	HWND	hWndPrev = FindWindow (GAMEWNDCLASS, GAMETITLE);

	if (hWndPrev)
		::MessageBox (NULL, "Jane's F18 is already running", "Can't play 2 sessions at once", MB_OK);
//		SetForegroundWindow (hWndPrev);

	return (hWndPrev != NULL);
}


/***************************************************************************************
 *	Func:		LoadWrappers
 *	Purpose:	Load dll which contains all of the wrapper code, create wrappers window
 *					and store result in global variable.
 *	Returns:	TRUE if wrappers dll loaded successfully, FALSE if not.
 **************************************************************************************/
BOOL LoadWrappers (DWORD dwStartScreen)
{
	char	szWrapper[260];
	BOOL	bRet = FALSE;

	_ASSERT (g_hWrapInst == NULL);

	if (!g_hWrapInst)
	{
		sprintf (szWrapper, "%s\\WRAPPER.DLL", g_szExePath);
		g_hWrapInst = LoadLibraryEx (szWrapper, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (!g_hWrapInst)
			g_hWrapInst = LoadLibraryEx ("WRAPPER.DLL", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (g_hWrapInst)
		{
			g_pfnStartWrappers = (STARTWRAPPROC) GetProcAddress (g_hWrapInst, "StartWrappers");
			g_pfnStopWrappers = (STOPWRAPPROC) GetProcAddress (g_hWrapInst, "StopWrappers");
			g_PfnProcessLoopProcess = (PROCESSLOOPPROC) GetProcAddress (g_hWrapInst, "ProcessLoopProcess");
		}
		else
		{
			LPVOID		lpMsgBuf;

			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
			    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL);

#ifdef _DEBUG
			char	szDebug[260];
			GetCurrentDirectory (260, szDebug);
			MessageBox (NULL, (char *)lpMsgBuf, szDebug, MB_OK);
#else
			MessageBox (NULL, (char *)lpMsgBuf, "Problem Loading Library", MB_OK);
#endif

			LocalFree (lpMsgBuf);

 		}

		// If we get pointers to start and stop wrappers, create wrapper window
		if (g_pfnStartWrappers && g_pfnStopWrappers)
			bRet = (*g_pfnStartWrappers) (g_hGameWnd, dwStartScreen);
	}

	return (bRet);
}

/***************************************************************************************
 *	Func:		FreeWrappers
 *	Purpose:	Stops, Closes and Frees dll which contains all of the wrapper code
  **************************************************************************************/
void FreeWrappers (void)
{
//	_ASSERT (g_hWrapInst != NULL);

	// If function to stop wrappers is loaded (it should be), call it
	if (g_pfnStopWrappers)
		(*g_pfnStopWrappers) (0);

	// If library handle is present (it should be), free it
	if (g_hWrapInst)
	{
		FreeLibrary (g_hWrapInst);

		g_hWrapInst = NULL;
		g_pfnStartWrappers = NULL;
		g_pfnStopWrappers = NULL;
		g_PfnProcessLoopProcess = NULL;
	}

	g_pSmkVideo = NULL;
	g_pUVPlayer = NULL;
}


/***************************************************************************************
 *	Func:		InitInstance
 *	Purpose:	Register our required wndclasses
 *	Returns:	TRUE if successful; FALSE if not
 **************************************************************************************/
BOOL InitInstance (void)
{
	// Register GameWnd's window class
	WNDCLASS		wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) GameWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hGameInst;
	wc.hIcon = LoadIcon (g_hGameInst, "JANESF18");
	wc.hCursor = LoadCursor (NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GAMEWNDCLASS;

	if (!RegisterClass (&wc))
		return (FALSE);

	// Register SimWnd's window class
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) SimWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hGameInst;
	wc.hIcon = LoadIcon (g_hGameInst, "JANESF18");
	wc.hCursor = LoadCursor (NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH) GetStockObject (NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = SIMWNDCLASS;

	if (!RegisterClass (&wc))
		return (FALSE);

	return (TRUE);
}

/***************************************************************************************
 *	Func:		CreateGameWindow
 *	Purpose:	Register our wndclass and create a toplevel window from it.
 *	Returns:	TRUE if a game window is successfully created; FALSE if not
 **************************************************************************************/

BOOL CreateGameWindow (void)
{
	_ASSERT (!g_hGameWnd);
	DWORD flag = 0;

#ifndef _DEBUG
	flag = WS_EX_TOPMOST;
#endif

	g_hGameWnd = CreateWindowEx ( flag, GAMEWNDCLASS, GAMETITLE, WS_POPUP,
		0, 0, g_nScreenWidth, g_nScreenHeight, NULL, NULL, g_hGameInst, NULL);

	return (g_hGameWnd != NULL);
}

/***************************************************************************************
 *	Func:		CreateSimWindow
 *	Purpose:	Register our wndclass and create a sim window from it.
 *	Parms:		Pointer to user defined data. Will be accessible in WM_CREATE message.
 *	Returns:	TRUE if a sim window is successfully created; FALSE if not
 **************************************************************************************/

BOOL CreateSimWindow (LPVOID lpUser)
{
	/*
	_ASSERT (!g_hSimWnd);



	g_hSimWnd = CreateWindowEx (0, SIMWNDCLASS, GAMETITLE, WS_POPUP|WS_VISIBLE,
		0, 0, 1024, 768, NULL, NULL, g_hGameInst, NULL);// lpUser);/*FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT,

	return (g_hSimWnd != NULL);
	*/

	return 0;
}

BOOL InitSound (void)
{
	char *		pszPath;
	char		szFindPath[260];
	BOOL		bRet = FALSE;

	if (!SndInit (22050,16,2))
		MessageBox (NULL, "Sound Error", "Error Starting Sound Engine", MB_OK);
	else
	{
		pszPath = GetRegValue ("sound");
		sprintf (szFindPath, "%s\\%s", pszPath, "soundz.rwv");

		bRet = SndOpenVolume (szFindPath);
	}

	return (bRet);
}

void ShutdownSound (void)
{
	SndCloseVolume ();
	SndFree ();
}

BOOL CALLBACK CreateDriverList(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam)
{
    char			ach[128];
    HRESULT         hr;
    IDirectDraw     *pDD = NULL;
    DDCAPS          DriverCaps;
	char *			pszList = (char *) lParam;

    hr = DirectDrawCreate(lpGUID, &pDD, NULL);

    if (FAILED(hr))
		return D3DENUMRET_OK;

    ZeroMemory(&DriverCaps, sizeof(DDCAPS));
    DriverCaps.dwSize = sizeof(DDCAPS);

    hr = pDD->GetCaps(&DriverCaps, NULL);
    if (!FAILED(hr))
	{
    	if( !strncmp( szName, "Voodoo",6) || ( (DriverCaps.dwCaps & DDCAPS_3D) && !(DriverCaps.dwCaps2 & DDCAPS2_NO2DDURING3DSCENE) ) )
    	{
			sprintf(ach,"%s (%s)\n",szName, szDevice);
			//if( strncmp( szName, "3Dfx",4) && strncmp( szName, "Voodoo",6) ) // attempt to remove 3dfx and voodoo drivers from D3D list and only allow Glide
			strcat (pszList, ach);
		}

	}

    pDD->Release();

    return DDENUMRET_OK;
}

void ReadVideoRegistry (void)
{
	LONG				lError;
	HKEY				hKey;
	DWORD				dwDisp;
	DWORD				dwSize = sizeof (int);
	DWORD				dwStrLen;

	lError = RegCreateKeyEx (HKEY_LOCAL_MACHINE, REGKEY_CONFIG,
		0, "", REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL, &hKey, &dwDisp);

	// Friendly loadout
	dwStrLen = 256;
	lError = RegQueryValueEx (hKey, REGKEY_VIDDRIVER, 0, NULL, (BYTE *)g_szCurrentVideoDriver, &dwStrLen);
	RegCloseKey (hKey);
}

BOOL Is3DFXAround(void)
{
	char			str[512];
	HINSTANCE		hInst = NULL;

	GetWindowsDirectory(str,sizeof(str)-1);
	strcat(str,"\\glide2x.dll");
	hInst = LoadLibrary(str);
	if (!hInst)
	{
		GetSystemDirectory(str,sizeof(str)-1);
		strcat(str,"\\glide2x.dll");
		hInst = LoadLibrary(str);
		if (!hInst)
			return FALSE;				// No Glide2x.dll
	}

	FreeLibrary(hInst);
	return TRUE;
}

void GetVideoDrivers (char *pszDriverList, char *pszCurrentDriver)
{
	if (pszDriverList)
		strcpy (pszDriverList, g_pszVideoDriverList);

	if (pszCurrentDriver)
		strcpy (pszCurrentDriver, g_szCurrentVideoDriver);
}

void SetCurrentVideoDriver (char *pszNewDriver)
{
	HKEY		hKey;
	DWORD		dwDisp;
	int			nLength;

	strcpy (g_szCurrentVideoDriver, pszNewDriver);
	nLength = strlen (g_szCurrentVideoDriver) + 1;

	RegCreateKeyEx (HKEY_LOCAL_MACHINE, REGKEY_CONFIG, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &hKey, &dwDisp);
	RegSetValueEx (hKey, REGKEY_VIDDRIVER, 0, REG_SZ, (BYTE *)g_szCurrentVideoDriver, nLength);
	RegCloseKey (hKey);
}

int InitVideoDriver (void)
{
	char		szDriverList[2048];
	char		szLastDriver[260];
	char		*pszNextString;

	szDriverList[0] = NULL;
    DirectDrawEnumerate(CreateDriverList, szDriverList);

	if (!szDriverList[0])
	{
		MessageBox(NULL,"Could not find display driver list", "F-18 Display Error", MB_OK);
		return(0);
	}

//  Removed - NO MORE SOFTWARE MODE - AND THAT'S THE BOTTOM LINE, BECAUSE STONE COLD...
//	char		szBuffer[256];
//	sprintf (szBuffer, "%s\n", VIDDRIVER_SW);
//	strcat (szDriverList, szBuffer);

	g_pszVideoDriverList = (char *)malloc (strlen (szDriverList) + 1);
	if (g_pszVideoDriverList)
		strcpy (g_pszVideoDriverList, szDriverList);

	ReadVideoRegistry ();

	// check to see if the reg driver is still a vaild card. Card/Driver might've been removed
	int found = 0;
	pszNextString = strtok (szDriverList, "\n");
	while( pszNextString )
	{
		if( !strcmp(g_szCurrentVideoDriver, pszNextString) )
			found = 1;
		pszNextString = strtok (NULL, "\n");
	}

	if(!g_szCurrentVideoDriver[0] || !found )
	{
		pszNextString = strtok (szDriverList, "\n");
		while (pszNextString)
		{
			strcpy(szLastDriver,pszNextString);
			pszNextString = strtok (NULL, "\n");
		}
		SetCurrentVideoDriver(szLastDriver);
	}
	return(1);
}

/***************************************************************************************
 *	Func:		IsCDROMOK
 *	Purpose:	Checks for the presence of the F18 CD-ROM, only for Min and Typical installs
 *	Parms:		None
 *	Returns:	TRUE F18 CD-ROM present or we have a full Install
 **************************************************************************************/

BOOL IsCDROMOK (void)
{
	LPSTR lp;

	// Check for full install. Full installs do not do CD-Rom check
	if ((lp = GetRegValue("InstallType"))!=NULL)
	{
		if (atoi(lp)==2)
			return TRUE;
	}

	// Try and find the F18.EXE on the cdrom
	if ((lp = GetRegValue("CDROMPath"))!=NULL)
	{
		char path[MAX_PATH];

		strcpy(path,lp);
		if (path[strlen(path)-1]!='\\')
			strcat(path,"\\");
		strcat(path,"F18.EXE");

		HANDLE hFile = CreateFile (path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile==INVALID_HANDLE_VALUE)
			return FALSE;
		CloseHandle (hFile);
	}

	return TRUE;
}


BOOL OkCheckCDROM (void)
{
	char *p;

	switch (g_iLanguageId)
	{
		case MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US):
			p = "Please insert the F-18 CDROM then press OK to continue.";
			break;

		case MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH):
			p = "Veuillez insérer le CD-ROM de Jane's F-18, puis appuyez sur OK pour continuer.";
			break;

		case MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN):
			p = "Legen Sie bitte die F-18 CD-ROM ein und klicken Sie auf OK, um fortzufahren.";
			break;
	}

	if (MessageBox(NULL, p, "F-18", MB_OKCANCEL)==IDCANCEL)
		return FALSE;

	return TRUE;
}


/***************************************************************************************
 *	Func:		VerifyVideoMode
 *	Purpose:	Checks for the presence a REAL video card in a REAL mode, (not 16color mode)
 *	Parms:		None
 *	Returns:	TRUE if in greater than 16 color mode
 **************************************************************************************/

BOOL VerifyVideoMode (void)
{
	HDC hdc;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	int bbp = GetDeviceCaps(hdc, BITSPIXEL);
	DeleteDC(hdc);

	if (bbp<8)
	{
		char *p;

		switch (g_iLanguageId)
		{
			case MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US):
				p = "Before running Jane's F-18, please change your video mode to at least 256 colors.";
				break;

			case MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH):
				p = "Avant de démarrer Jane's F-18, veuillez changer pour le mode vidéo 256 couleurs.";
				break;

			case MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN):
				p = "Bevor Sie Jane's F-18 starten, müssen Sie die Farbpalette auf mindestens 256 Farben einstellen.";
				break;
		}

		MessageBox(NULL, p, "F-18", MB_OK);

		return FALSE;
	}

	return TRUE;

}

//=================================================================
//====================  D L L   E X P O R T S  ====================
//=================================================================
//
//	To export a function from an EXE/DLL you can do the full
//	linkage spec, ie.  "extern "C" __declspec(dllexport)" or
//	you can simply add the function name to the EXPORTS block
//	of a DEF file.  The DEF file method forces both the
//	extern "C" linkage as well as the __declspec(dllexport).
//

void SetVideoPlayer (void *pVideoObj, BOOL bInstall)
{
	// If asking to replace previous video object with new one, no problem
	if (bInstall)
		g_pSmkVideo = (CSmkVideo *)pVideoObj;
	else
	{
		// however, if they want to yank the video object out, make sure that
		//	we have the same object currently playing; otherwise we could yank
		//	the current vid player errantly
		if (g_pSmkVideo == pVideoObj)
			g_pSmkVideo = NULL;
	}
}

void SetUVPlayer (MoviePlayer *pVideo, BOOL bInstall)
{
	// If asking to replace previous video object with new one, no problem
	if (bInstall)
		g_pUVPlayer = pVideo;
	else
	{
		// however, if they want to yank the video object out, make sure that
		//	we have the same object currently playing; otherwise we could yank
		//	the current vid player errantly
		if (g_pUVPlayer == pVideo)
			g_pUVPlayer = NULL;
	}
}

void SetResourceFile (char *pszFile)
{
	strcpy (g_szResourceFile, pszFile);
}

int GetMultiPlayerStatus (void)
{
	return (MultiPlayer);
}

void SetWrapperMode(int iWrapperMode)
{
	g_iWrapperMode=iWrapperMode;
}
int GetWrapperMode()
{
	return(g_iWrapperMode);
}



#pragma optimize("",on)