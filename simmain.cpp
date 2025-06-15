/*****************************************************************************
*	Jane's F/A-18
*
*   Simmain.cpp  
*-----------------------------------------------------------------------------
*	Copyright (c) 1999 by Electronic Arts, All Rights Reserved.
*****************************************************************************/
#include "F18.h"
#include <dsound.h>
#include "keystuff.h"
#include "GameSettings.h"
#include "3dfxF18.h"
#include "particle.h"
#include "SkunkFF.h"
#include "Sprite.h"
#include "clouds.h"
#include "MemMap.h"
#include "Seatstf.h"


//***********************************************************************************************************************************

#ifdef __DEMO__
#define REGKEY_CONFIG			"Software\\Jane's Combat Simulations\\F18 Demo\\Config"
#else
#define REGKEY_CONFIG			"Software\\Jane's Combat Simulations\\F18\\Config"
#endif

#ifdef __DEMO__
	extern GrBuff *DemoLogo;
#endif


#define REGKEY_KEYSETTINGS		"Settings"
#define REGKEY_MPDPROGGIE		"MPDProggie"

#define ACM_TEST 1   // numpad test

#define	VIDDRIVER_3DFX		"3Dfx (Glide)"
#define VIDDRIVER_SW		"Software"
#define	REGKEY_VIDDRIVER	"VideoDriver"

extern char g_szCurrentVideoDriver[256];

int gkgeodist = 25;	// who puts variables in the game with their initials? What a loser!
int gktexdist = 22;

extern char		g_szCurrentVideoDriver[256];
extern int		MySlot;
extern BOOL		g_bIAmHost;
extern int		CurrentCockpit;
extern int		bActive;
extern BOOL		g_bInSim;
//tern HWND		g_hSimWnd;
extern HWND		g_hGameWnd;
extern int		g_nMissionType;
extern HINSTANCE  g_hGameInst;
extern char		g_szExePath[260];

extern BYTE LiftLineFade;

// for showing the proper switch bitmap
extern RightDownSwitchState;

GrBuff *TestBuff2;

//extern int	KeyBoardRudderState;
extern int HUDColor;
extern float Trim;
extern float TmpRudder;
extern int CurrentView;
extern int ViewChange;

//defined in mpd.cpp
extern int MPDOrder[];
extern int MPDSlot[];

extern CameraInstance	 OldCamera;
extern int OldCockpitSeat, OldCurrentCockpit, OldCursor;

extern HINSTANCE hWinInstance;
extern int nWinCmdShow;

int					iMHShowCallSign=0;
IVIBESeatData *		pSeatData = NULL;
int					ShowPosition;
int					paused = 1;
int					Pause = 0;
int					TimeExcel;
GAMESETTINGS		g_Settings;							// Global settings
CMemMap				g_MemMap;

extern BOOL MouseChanged();
BOOL InitSound (void);
void ShutdownSound (void);
//void FixBridges();
void NetFreeOpenSlot (int nSlot);
void InitWeather();
void ShutdownClouds( void);
void DisplayPopUpMpds(void);
void DrawDirectionArrows();

void DisplayInitText(char *sTxt, int SkipLine, int showrelease=0);

extern void SetSeatVars();


extern int GregTest;
DWORD RandomSeedHelper;

BOOL DoingSimSetup = FALSE;

int iCapture=0;

extern int g_iWrapperMode;

extern void LoadWorldDBNeeds();
extern void FreeLoadDB();
extern void NewScreenGrab( LPDIRECTDRAWSURFACE4 lpDDS );
void DrawDestructoText();
void DoAARadarScan();


extern void	InitSoundEffectsVariables();
void camera_mode_fixed_chase(VKCODE vk);//void camera_mode_chase(VKCODE vk);
void CheckForPlaneCollisions();

char CaptureScreenAtEndofFrame =0;

void SimShutdown (void);
int SimMain(void);
void NetRefreshComList(void);
BOOL NetRadioCheckKey (int keyflag, WPARAM wParam);
void UseMultiSettings (void);
//void ResetMpdForNonVirtual();
void VirtualMpdInteractSetup();
extern int VirtualMpdsOn;
void SeduceEngineData();
extern int OldAmountToDither;
extern int DrawPopMpds;
float RTDeltaTicks;
float RTSqrtDeltaTicks;
int GameLoopsThisFrame;

BOOL InitDirectDrawStuff();
void InitLandTexturePals(void);
void ClearLandTexturePals(void);
void DestroyD3D(void);
void SetCurrentVideoDriver (char *pszNewDriver);

void DebugShowHotSpots( void);

void DisplayMPDS(void);
void grDrawCursor( void );
extern int CursorIsVisible;
char bShowCursorPoly=0;

extern long g_iWeaponID;
extern void LogData();
extern void ShutdownEventLog();
extern void InitMissionStatistics();
extern void CleanupMissionStatistics();
// These should be moved
extern InfoProviderInstance *AllInfoProviders;
extern InfoProviderInstance *AllAAAProviders;
extern AAWeaponInstance *AllAAWeapons;
extern void DumpFlightStatistics();
void ReleaseShadowEntry(void *pt);
extern BOOL AATargetUpdated;
extern void WriteDumpLog(char *szString);

BOOL SingleStepMode = FALSE;

DDSURFACEDESC2 	ddsdDisplayMode;


BOOL g_bActive = FALSE;



void DrawGEffects(float pilot_g);

/***************************************************************************************
 *	Func:		StartSim
 *	Purpose:	Set game status to 'sim', get sim window running.
 *	Parms:		LPVOID lpUser - Pointer used to pass whatever information needed
 *				into the sim window itself as part of the WM_CREATE message it receives.
 *				This value is found in the CREATESTRUCT pointer parm.
  **************************************************************************************/
void StartSim (LPVOID lpUser)
{
	hwnd = g_hGameWnd;			// using gamewnd turns allows us to ALT_TAB
	SimMain();
//#ifndef _DEBUG
//#else
//	g_hSimWnd = hwnd ;		//using simwind allows us to hit break(int3)
//#endif

	if (hwnd)
	{
		g_bInSim  = TRUE;

		void NetGrabPlanes();
 		if (MultiPlayer)
		{
			NetGrabPlanes();
			NetRefreshComList();
		}
	}
}

/***************************************************************************************
 *	Func:		StopSim
 *	Purpose:	Reset game status out of 'sim'
  **************************************************************************************/
void StopSim (void)
{
	ShutdownSound ();
	SimShutdown ();

//#ifdef _DEBUG
//	DestroyWindow ( hwnd );
//	hwnd = NULL;
//#endif
	g_bInSim = FALSE;
}

//***********************************************************************************************************************************
void ScreenCapture()
{
	if( _3dxl == _3DXL_GLIDE )
	{
		ScreenCapture3Dfx( );
		return;
	}

	//
	// NewScreenGrab saves as .BMP to be consistent with ScreenCapture3Dfx & ScreenCapture Wrappers
	//


	//NewScreenGrab (g_bInSim);
	CaptureScreenAtEndofFrame = 1;

	return;

	/*

	long filefind;
	long filestat=0;
	struct _finddata_t fileinfo;
	char filename[20];
	char txtnum[5];

	int num=0;
	int stat=0;

	filefind = _findfirst("DUMP*.PCX", &fileinfo);

	lpDD->FlipToGDISurface();

	filestat=filefind;

	while(filestat != -1)
	{
		filestat = _findnext(filefind, &fileinfo);
		num++;
	}
	strcpy(filename,"DUMP");
	sprintf(txtnum,"%03d",num);
	strcat(filename,txtnum);
	strcat(filename,".PCX");

	unsigned char NewPalette[256*3];
	PALETTEENTRY TmpPal[256];
	lpDDSPalette->GetEntries(0,0,256,TmpPal);

	for (int i=0; i<256; i++)
	{
		NewPalette[(i*3)+0]=TmpPal[i].peRed;
		NewPalette[(i*3)+1]=TmpPal[i].peGreen;
		NewPalette[(i*3)+2]=TmpPal[i].peBlue;
	}

	GrBuff *GrTempPage = GrAllocGrBuff(640,480,GR_8BIT);
	GrCopyBuff(GrTempPage,VisibleSurface);

	WritePCXGrBuff(GrTempPage,filename,NewPalette);
	GrFreeGrBuff(GrTempPage);

	_findclose(filefind);
	*/
}

//***********************************************************************************************************************************
void RestartTicks(void)
{
	GameLoop   = 0;
	GameLoopInTicks = 0;
	LastGameLoop = 0;
	TickCount  = 0;
	FrameCount = 0;
	DeltaTicks = 0;
	TimeExcel  = 0;
	LastTick = CurrentTick = GetTickCount();
	SimPause = 0;
	JustLeftPausedState = 0;
}

void UpdateSounds();


void DisplayCaptureMarks()
{
	if (g_iWrapperMode==SIM_BUILDER)
	{
		int iX0,iX1=0;
		int iZ0,iZ1=0;
		int iOffset=5;
		if (ScreenSize.cx==640)
		{
			iX0=128-(iOffset);
			iZ0=120-(iOffset);
			iX1=iX0+(384+(iOffset*2));
			iZ1=iZ0+(240+(iOffset*2));
		}
		else if (ScreenSize.cx == 800)
		{
			iX0=208-(iOffset);
			iZ0=360-(iOffset);
			iX1=iX0+(384+(iOffset*2));
			iZ1=iZ0+(240+(iOffset*2));
		}
		else if (ScreenSize.cx == 1024)
		{
			iX0=320-(iOffset);		// Width Centered
			iZ0=264-(iOffset);
			iX1=iX0+(384+(iOffset*2));
			iZ1=iZ0+(240+(iOffset*2));
		}

		HUDtextLg(iX0,iZ0, "+");
		HUDtextLg(iX1,iZ0, "+");
		HUDtextLg(iX1,iZ1, "+");
		HUDtextLg(iX0,iZ1, "+");
	}
}

void DisplayDebugClock()
{
#ifdef _DEBUG

		BOOL pm;
		int hour = ((int)floor(WorldParams.WorldTime*(1.0f/3600.0f)))%24;
		if (hour >= 12)
			pm = TRUE;
		else
			pm = FALSE;

		hour %= 12;
		if (!hour)
			hour = 12;
		int minute = ((int)floor(WorldParams.WorldTime*(1.0f/60.0f)))%60;

		sprintf(TmpStr,"%i:%02i %s",hour,minute,(pm) ? "PM" : "AM");

		GrDrawStringClipped(GrBuffFor3D, LgHUDFont, 20,ScreenSize.cy*3/4, TmpStr, 160,128,128,FALSE);
#endif
}

void DrawUnlitAddBillboardRel(FPoint &btm,FPoint &top,float width,TextureBuff *texture,float alpha = 1.0f,FPoint *color = NULL,BOOL mip_maps = FALSE);
extern TextureBuff *Tracer2;

BOOL dclready;
FMatrix dclmat;
FPointDouble dclpoint;

void SetDebugCameraLine(FPointDouble &location,FMatrix &mat)
{
	dclready = TRUE;
	dclmat = mat;
	dclmat.Transpose();
	dclpoint = location;
}


void DisplayDebugCameraLine()
{
	FPoint p1,p2;

	p1.MakeVectorToFrom(dclpoint,Camera1.CameraLocation);
	p2 = p1;
	p1.AddScaledVector(-10 MILES,*(FPoint *)&dclmat.m_Data.RC.R2C0);

	DrawUnlitAddBillboardRel(p2,p1,10 FEET,Tracer2,TRUE);

}

void CheckForRedoutBlackout()
{
	if (
			(
				(Camera1.AttachedObjectType == CO_PLANE) &&
				(Camera1.AttachedObject == (void *)PlayerPlane)
			) &&
			(
				(Camera1.CameraMode == CAMERA_COCKPIT) ||
				(Camera1.CameraMode == CAMERA_CHASE) ||
				(Camera1.CameraMode == CAMERA_FIXED_CHASE) ||
				(Camera1.CameraMode == CAMERA_TACTICAL)
			) &&
			(g_Settings.gp.dwFlight & GP_FLIGHT_READOUT_BLACKOUT)
		)
		DrawGEffects(PlayerPlane->PilotGLoad);
}


//***********************************************************************************************************************************
void DoGame(void)
{
	long orgdticks, workticks, finalticks;
	long startticks, endticks, moveloops;
	int maxticks = 200;
	long originalaccel = TimeExcel;

	if(iInJump)
	{
		DoJumpGame();
		return;
	}

	LastTick     = CurrentTick;
	CurrentTick  = GetTickCount();
	if (SingleStepMode)
			LastTick = CurrentTick - 20;
	DeltaTicks   = CurrentTick - LastTick;

	if (!MultiPlayer)
	{
		if (DeltaTicks > 250.0)
			DeltaTicks = 250.0;
		LastTick = CurrentTick - DeltaTicks;
	}

	orgdticks = DeltaTicks;

	RTDeltaTicks = (float)DeltaTicks;

	if (RTDeltaTicks > 200.0f)
		RTDeltaTicks = 200.0f;

	RTSqrtDeltaTicks = sqrt(RTDeltaTicks);

	DeltaTicks <<= TimeExcel;
	sqrtDeltaTicks = (float)sqrt((float)DeltaTicks);
	finalticks = DeltaTicks;

//	seconds_per_frame = DeltaTicks/1000.0f;
	seconds_per_frame = 0.0f;
	
	FrameRate = 1000.0f / DeltaTicks;
	OneOverFrameRate = 1.0f / FrameRate;

	ReadJoyVarD(dwDeadZone);

	if(!iEndGameState)
		KeyScanButtonLayout();
	else
		KeyScanButtonLayoutForEndMenu();

	CheckDestructoCam();

	AATargetUpdated = FALSE;

	// Following redundant variables are both used by flames.cpp
	//
	GameLoopsPerFrame = 0;
	LastGameLoop = GameLoop;  // save for GameLoops this frame

	_3dxlBeginFrame();

	//  Added for intelliVIBE support
	if(pSeatData)
		SetSeatVars();

	if(iEndGameState)
	{

		if (DrawPopMpds)    //
		{
			ToggleVirtualMpds();
			/*
			LONG x = GetSystemMetrics(SM_CXSCREEN)>>1;
			LONG y = GetSystemMetrics(SM_CYSCREEN)>>1;

			SetCursorPos(x, y);
			ppLastMouse.x = x;
			ppLastMouse.y = y;
			ppCurrMouse.x = 0;
			ppCurrMouse.y = 0;

			OurShowCursor( FALSE );
			*/
		}

		VKCODE vk;
		vk.vkCode = 0;
		vk.wFlags = 0;
		vk.dwExtraCode = 0;

		if( Camera1.CameraMode == CAMERA_COCKPIT)
			camera_mode_fixed_chase(vk);		//camera_mode_chase( vk);


//		srand(GameLoop + RandomSeedHelper);

		OurShowCursor( TRUE );

		Draw3DWorld(&Camera1);
		//UFCCalcCentralComputerData();

		FlushAlphaLists();
		ResetAlphaLists();

		SetD3DStates(0);

		DisplayHUD((PlaneParams *)Camera1.AttachedObject);
		if( DrawPopMpds )
			DisplayPopUpMpds();

		AICDoEndBox(iEndGameState); //iam: This is the Exit box function - when exiting a flight.

#ifdef __DEMO__
	  	GrCopyRectMask(BackSurface, 495,16, DemoLogo, 0,0, 128,48);
#endif
		if (bShowCursorPoly && CursorIsVisible)
			grDrawCursor( );

		_3dxlEndFrame();
		if( CaptureScreenAtEndofFrame )
 			NewScreenGrab(lpDDSPrimary);  //screen grab outside begin endsce
		CaptureScreenAtEndofFrame = 0;


//		FixBridges();

		SndPauseAllSounds();

		GrFlipPage();
	}
	else if (!SimPause)
	{
		TickCount += DeltaTicks;

		if((TimeExcel) || (orgdticks > maxticks))
		{
			if(orgdticks > maxticks)
			{
				DeltaTicks = maxticks;
			}
			else
			{
				DeltaTicks = orgdticks;
			}
		}

		ControlPlanes();
		VMoveVehicles();

		DeltaTicks = finalticks;

 //		ConsumeFuel(PlayerPlane, DeltaTicks);

		if(orgdticks > maxticks)
		{
			workticks = maxticks;
		}
		else if(TimeExcel)
		{
			workticks = orgdticks;
		}

		moveloops = 0;
		startticks = CurrentTick;
		while (TickCount >= 20)
   		{
			/* ------------------------------------------3/5/98 9:42AM---------------------------------------------
			 * This will keep the random numbers in sync when the game is paused, and allow the game to be retested
			 * ----------------------------------------------------------------------------------------------------*/
//			srand(GameLoop + RandomSeedHelper + rand());

			GameLoopsPerFrame++;
			GameLoop++;
			GameLoopInTicks += 20;
			moveloops++;
			MovePlanes();
			CheckForPlaneCollisions();
			MoveWeapons();
			MoveAAAStreamVertexes();
			UpdateScheduledExplosions(); //This should, maybe, be moved outside loop.

			if(ScanBeamOn)
		      DoScanBeam();

			//Detect collisions here
			TickCount -= 20;
			WorldParams.WorldTime += 0.02;
			if (WorldParams.WorldTime >= (24.0f HOURS))
				WorldParams.WorldTime = 0.0f;
			if((TimeExcel) || (orgdticks > maxticks))
			{
				workticks -= 20;
				if(workticks < 0)
				{
					if(orgdticks > maxticks)
					{
						workticks += maxticks;
						if(TickCount > (DWORD)maxticks)
						{
							DeltaTicks = maxticks;
						}
						else
						{
							DeltaTicks = (orgdticks << TimeExcel) % maxticks;
						}
					}
					else
					{
						workticks += orgdticks;
						DeltaTicks = orgdticks;
					}

					ControlPlanes();
					VMoveVehicles();
					DeltaTicks = finalticks;
				}
			}
		}

		seconds_per_frame = GameLoopsPerFrame/50.0f;
		endticks  = GetTickCount();

		if(lAfterJump)
		{
			RestoreFromJump();
			lAfterJump = 0;
		}

		if(UFC.APStatus & 0x2)
		{
			UFC.APStatus &= ~0x2;
			if(UFC.APStatus)
			{
				AutoPilotOnOff();
			}
		}

		// Send my plane's data, get net planes & move net planes
		//
		void NetWork();
		if (MultiPlayer)  NetWork();

		SndServiceSound();

		MoveGenerators();
		MoveCanisters();
		MoveRain();

		MoveSprites();

		UpdateOrphanedSmokeTrails();
		//UpdateVirtualMpd( );

//#ifdef _D3DLINES
//		if (TerrainYesNos & YN_NO_TRANSITIONS)
//			GrFillBuff(BackSurface);
//#endif

		Draw3DWorld(&Camera1);

		UFCCalcCentralComputerData();

		FlushAlphaLists();
		ResetAlphaLists();

		DoAARadarScan();
		SetSensorAwareness();

		ProcessEnemyRadarActivity();
		MainAvionicsTick();

		CheckMpdDamage();

		//no cockpit

		ConsumeFuel(PlayerPlane, DeltaTicks);
		float GetF18WeaponDrag(PlaneParams *P);
		PlayerPlane->WeaponDrag = GetF18WeaponDrag(PlayerPlane);

//		FixBridges();

//		SeduceEngineData();

		if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
		{
			D3DRECT rect;

			rect.lX1 = GrBuffFor3D->ClipLeft;
			rect.lY1 = GrBuffFor3D->ClipTop;

			rect.lX2 = GrBuffFor3D->ClipRight+1;
			rect.lY2 = GrBuffFor3D->ClipBottom+1;

			lpD3DViewport->Clear2(1,&rect, D3DCLEAR_ZBUFFER ,0,1.0f,0);

			CheckHUDMode();

			DetectCockpitInteractions();
			if ( CurrentView & COCKPIT_VIRTUAL_SEAT )
				DisplayMPDS();

			DrawDynamicCockpit();

			FlushAlphaLists();
			ResetAlphaLists();

	    	DisplayHUD((PlaneParams *)Camera1.AttachedObject);

			if( DrawPopMpds )
				DisplayPopUpMpds();


			//DebugShowHotSpots( );

		}

		DrawDirectionArrows();

		AICDoMessagesAndComms();
		DrawDestructoText();


		void NetData();
		void NetRadioChatMenu();
		if (MultiPlayer)
		{
			NetData();
			NetRadioChatMenu();
		}

		CheckForRedoutBlackout();

		DisplayCaptureMarks();

		if (bShowCursorPoly && CursorIsVisible)
			grDrawCursor( );

//		DisplayDebugClock();

		_3dxlEndFrame();
		if( CaptureScreenAtEndofFrame )
 			NewScreenGrab(lpDDSPrimary);  //screen grab outside begin endsce
		CaptureScreenAtEndofFrame = 0;


#ifdef __DEMO__
	  	GrCopyRectMask(BackSurface, 495,16, DemoLogo, 0,0, 128,48);
#endif
		GrFlipPage();
		FrameCount++;

		LogData();

		UpdateSounds();

		SimFF();

		if (JustLeftPausedState)
		{
			SndResumeAllSounds();
			JustLeftPausedState = 0;
		}

		if(TimeExcel > 0)
		{
			lLastAverageMovePlane = (endticks - startticks) / (float)moveloops;
			lLastAverageMovePlane ++;
			lLastAverageMovePlane <<= TimeExcel;

			if((lLastAverageMovePlane) > 20)
			{
				TimeExcel--;
			}
		}
	}
	else
	{
//		srand(GameLoop + RandomSeedHelper);

//#ifdef _D3DLINES
//		if (TerrainYesNos & YN_NO_TRANSITIONS)
//			GrFillBuff(BackSurface);
//#endif

		Draw3DWorld(&Camera1);

#ifdef _DEBUG
//		DisplayDebugCameraLine();
#endif

		UFCCalcCentralComputerData();

		FlushAlphaLists();
		ResetAlphaLists();

		//no cockpit

//		FixBridges();

		DoAARadarScan();

		SetSensorAwareness();
		MainAvionicsTick();

		if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
		{

			D3DRECT rect;

			rect.lX1 = GrBuffFor3D->ClipLeft;
			rect.lY1 = GrBuffFor3D->ClipTop;

			rect.lX2 = GrBuffFor3D->ClipRight+1;
			rect.lY2 = GrBuffFor3D->ClipBottom+1;

			lpD3DViewport->Clear2(1,&rect, D3DCLEAR_ZBUFFER ,0,1.0f,0);

			DetectCockpitInteractions();
			CheckHUDMode();
			if ( CurrentView & COCKPIT_VIRTUAL_SEAT )
				DisplayMPDS();

			DrawDynamicCockpit();

			FlushAlphaLists();
			ResetAlphaLists();

			DisplayHUD((PlaneParams *)Camera1.AttachedObject);

			if( DrawPopMpds )
				DisplayPopUpMpds();

			//DebugShowHotSpots( );


		}

		//no cockpit
		DetectCockpitInteractions();

		DrawDirectionArrows();

		AICDisplayPausedRadioMsgs();
 		DrawDestructoText();

		CheckForRedoutBlackout();

		DisplayCaptureMarks();

#ifdef __DEMO__
	  	GrCopyRectMask(BackSurface, 495,16, DemoLogo, 0,0, 128,48);
#endif
		if (bShowCursorPoly && CursorIsVisible)
			grDrawCursor( );

//		DisplayDebugClock();

// debuging cameras and flir  jlm

		#if 0

		extern FPointDouble FlirPos;
		extern AvionicsType Av;

		if( AreMpdsInGivenMode(TGT_IR_MODE) )
		{
				FMatrix mat;
	  		mat.SetRPH(0,Av.Flir.SlewUpDown*DEGREE,Av.Flir.SlewLeftRight*DEGREE);

	  		mat *= PlayerPlane->Attitude;
				mat.Transpose();
				DisplayDebugCameraLine(FlirPos,mat);
		}

		#endif


		_3dxlEndFrame();
		if( CaptureScreenAtEndofFrame )
 			NewScreenGrab(lpDDSPrimary);  //screen grab outside begin endsce
		CaptureScreenAtEndofFrame = 0;


		if(gAICommMenu.lTimer != -9999)
		{
			AICRestoreFromCommKeys();
		}


		SndPauseAllSounds();
		GrFlipPage();
	}


}
//***********************************************************************************************************************************
void InitGameStuff(void)
{
	memset(&Camera1,0,sizeof(CameraInstance));

	Camera1.SubType = COCKPIT_FRONT;//FRONT_FORWARD_COCKPIT;
	Camera1.CameraMode = CAMERA_COCKPIT;
//	LoadCockpit(FRONT_FORWARD_COCKPIT);
	SetupView( COCKPIT_FRONT);//FRONT_FORWARD_COCKPIT);
	CockpitSeat = FRONT_SEAT;
	LoadAfterBurnerTexture();

	TimeExcel = 0;	// kill last games setting
}

GAMEPLAY	g_PreMultiOpts;

//***********************************************************************************************************************************
void InitGameSettings(void)
{
	HKEY			hKey;
	DWORD			dwDisp;
	DWORD			dwStrLen;

	// force intro video incase read fail
	g_Settings.misc.dwMisc |= GP_MISC_INTRO_VIDEO;

	if (RegCreateKeyEx (HKEY_LOCAL_MACHINE, REGKEY_CONFIG, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &hKey, &dwDisp)!=ERROR_SUCCESS)
		return;
	dwStrLen = sizeof(GAMESETTINGS);
	RegQueryValueEx (hKey, REGKEY_KEYSETTINGS, 0, NULL, (BYTE *)&g_Settings, &dwStrLen);


	// Read MPD programmed settings
	//dwStrLen = sizeof(int) * MAX_UNIQUE_MPDS * MAX_NUM_MODES;
	//lError = RegQueryValueEx (hKey, REGKEY_MPDPROGGIE, 0, NULL, (BYTE *)MpdProgrammingMatrix, &dwStrLen);
	AvionicsLoadData(hKey);

	RegCloseKey (hKey);

	// Update MPD positions to global vars
	CopyMemory (MPDOrder, g_Settings.mpdloc.MPDOrder, sizeof(int) * 3);
	CopyMemory (MPDSlot, g_Settings.mpdloc.MPDSlot, sizeof(int) * 3);

	// Save off previous game play settings, since we may muck them up with
	if (MultiPlayer)
		memcpy (&g_PreMultiOpts, &g_Settings.gp, sizeof (GAMEPLAY));
}
//***********************************************************************************************************************************
void CloseGameSettings(void)
{
	LONG		lError;
	HKEY		hKey;
	DWORD		dwDisp;

	// save MPD positions
	CopyMemory (g_Settings.mpdloc.MPDOrder, MPDOrder,  sizeof(int) * 3);
	CopyMemory (g_Settings.mpdloc.MPDSlot, MPDSlot, sizeof(int) * 3);

	lError = RegCreateKeyEx (HKEY_LOCAL_MACHINE, REGKEY_CONFIG, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &hKey, &dwDisp);

	// Save Game Settings
	lError = RegSetValueEx (hKey, REGKEY_KEYSETTINGS, 0, REG_BINARY, (BYTE *)&g_Settings, sizeof(GAMESETTINGS));

	// Save MPD programmed settings
 //	lError = RegSetValueEx (hKey, REGKEY_MPDPROGGIE, 0, REG_BINARY, (BYTE *)MpdProgrammingMatrix, sizeof(int)*MAX_UNIQUE_MPDS*MAX_NUM_MODES);
	AvionicsSaveData(hKey);

	RegCloseKey (hKey);
}
//***********************************************************************************************************************************
void CloseSimStuff(void)
{
	if (MultiPlayer)
		memcpy (&g_Settings.gp, &g_PreMultiOpts, sizeof (GAMEPLAY));

	for (int i=0; i<MAX_PLANE_TYPES; i++)
	{
		if (PlaneTypes[i].Model )  Free3DObject (PlaneTypes[i].Model );
		if (PlaneTypes[i].DestroyedModel )  Free3DObject (PlaneTypes[i].DestroyedModel );
		if (PlaneTypes[i].Shadow)  Free3DTexture(PlaneTypes[i].Shadow);

		PlaneTypes[i].Model  = NULL;
		PlaneTypes[i].DestroyedModel  = NULL;
		PlaneTypes[i].Shadow = NULL;

		int j = MAX_CONNECTED_PARTS;
		while(j--)
		{
			if (PlaneTypes[i].SubParts[j])  Free3DObject(PlaneTypes[i].SubParts[j]);
			PlaneTypes[i].SubParts[j] = NULL;
		}
	}

	for (i=0; i<MAX_WEAPON_TYPES; i++)
	{
		if (WeaponTypes[i].Model) Free3DObject(WeaponTypes[i].Model);
		WeaponTypes[i].TypeNumber=-1;
		WeaponTypes[i].Model=NULL;
	}

	for (i=0; i<MAX_MOVINGVEHICLE_TYPES; i++)
	{
		if (MovingVehicleTypeList[i].Model) Free3DObject(MovingVehicleTypeList[i].Model);
		MovingVehicleTypeList[i].TypeNumber=-1;
		MovingVehicleTypeList[i].Model=NULL;
	}

	for (i = 0;i < MAX_PLANES;i++)
	{
		if (Planes[i].PlaneCopy)
		{
			free(Planes[i].PlaneCopy);
			Planes[i].PlaneCopy = NULL;
		}

		if (Planes[i].Shadow)
		{
			ReleaseShadowEntry(&Planes[i].Shadow);
			Planes[i].Shadow = NULL;
		}
	}

	for (i = 0;i < MAX_MOVINGVEHICLE_SLOTS;i++)
	{
		if (MovingVehicles[i].WakeData)
		{
			free(MovingVehicles[i].WakeData);
			MovingVehicles[i].WakeData = NULL;
		}

		if (MovingVehicles[i].Shadow)
			ReleaseShadowEntry(&MovingVehicles[i].Shadow);
		MovingVehicles[i].Shadow = NULL;

	}


//	GrDestroyFont(FontBuff);
	CloseFMath();

	CloseTerrain();
	CleanupHUDSprites();
	CleanupRadarStuff();
	CleanupUFCSprites();
	CleanUpLookDownSprites();
	CleanupAvionicsSensor();
	CleanupMiscMpdSprites();
	CleanupVirutalCockpit();
	ShutDown3DSystem();

//	EasyFreeAll();
}




/***************************************************************************************
 *	Func:		SimWndProc
 *	Purpose:	Serve as simulated sim window
 **************************************************************************************/

HRESULT WINAPI RestoreAllSurfacesCallBack(   LPDIRECTDRAWSURFACE4 lpDDSurface,  LPDDSURFACEDESC2 lpDDSurfaceDesc,  LPVOID lpContext )
{
	if( lpDDSurface->IsLost() )
		lpDDSurface->Restore();
	return DDENUMRET_OK ;
}

LRESULT CALLBACK SimWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if 0   // NOT BEING USED ANYMORE
	// This is only used in DEBUG mode.. WndProc is now in main.cpp  SimWnd no longer exists  Tw: 10/01/99
/*

	PAINTSTRUCT		ps;

	switch (uMsg)
	{
		case WM_ACTIVATEAPP:
            // Pause if minimized or not the top window
            g_bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
            return 0L;

		case WM_ACTIVATE:		// This code lets ALT-TAB to work in the game
		{
			WORD fActive	= LOWORD(wParam);        // activation flag
			BOOL fMinimized	= (BOOL) HIWORD(wParam); // minimized flag
			if( ( fActive== WA_INACTIVE) || fMinimized)
			{
				//ClearLandTextures();

				//Free3DTexture( FREE_ALL_TEXTURES );

				//DestroyD3D();
				//DestroyDDPalette();
				//DestroyDDSurface();
			//	ShowWindow (SW_MINIMIZE);

				deactivated = TRUE;


			}
			else
			{
				if( deactivated )
				{
					activateApp = 1;
					//RestoreWindow

				//	ShowWindow (SW_RESTORE);



					//DDSURFACEDESC2 	ddsd;

					//ZeroMemory(&ddsd,sizeof(ddsd));
					//ddsd.dwSize = sizeof(ddsd);
					//lpDD->GetDisplayMode( &ddsd );

					//lpDD->SetDisplayMode(ScreenSize.cx, ScreenSize.cy, 16,0,0);


					//if( lpDDSBack->IsLost() )//implicit defined
					//	lpDDSBack->Restore();
					/*
					if( lpDDSPrimary->IsLost() )
						lpDDSPrimary->Restore();
					if( lpDDSZBuffer->IsLost() )
						lpDDSZBuffer->Restore();

					//lpDD->RestoreAllSurfaces();
					lpDD->EnumSurfaces(  DDENUMSURFACES_DOESEXIST |DDENUMSURFACES_ALL , NULL, NULL, RestoreAllSurfacesCallBack);


					//InitDirectDrawStuff();
				}
				deactivated = FALSE;
			}
			return 0;
		}
		break;
		*/
/*
		case WM_SYSCOMMAND:
			if (wParam==SC_SCREENSAVE)
				return 0;
			break;

		case WM_PAINT:
			BeginPaint (hWnd, &ps);
			EndPaint   (hWnd, &ps);
			return 0;

		case WM_LBUTTONDOWN:
			return 0;

		case WM_SYSKEYDOWN:

			if (NetRadioCheckKey (0, wParam))
				return 0;

			if (!KeyExcecuteKey( wParam ) ) // Trap ALT-key combinations
			{
				// NetWork debugging
				switch(wParam)
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
					case '9':  netKnob = (wParam - '0') / 10.0;  break;
					case 'D':  GetKeyState( VK_SHIFT )<0 ? ShowUp() : ShowDn();  break;
				}
			}
			return 0;

		case WM_KEYUP:
			AICCheckAIKeyUp(0, wParam);
			return 0;

		case WM_CHAR:
			NetRadioCheckKey (1, wParam);
			return 0;

		case WM_KEYDOWN:
			AICCheckAIComm(0, wParam);

			if (NetRadioCheckKey (0, wParam))
				return 0;

			if (!KeyExcecuteKey( wParam ) )
			{
				switch(wParam)
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
							OldCockpitSeat    = CockpitSeat;
							OldCurrentCockpit = CurrentCockpit;
							memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));
							OldCursor = CursorIsVisible;


							iEndGameState = 4;

							if(MultiPlayer)
							{
								NetSendPlayerExit(PlayerPlane);
							}

						}
						break;

				}
			}
			return 0;


		case WM_CLOSE:
			DestroyWindow (hWnd);
			return 0;
	}
	return (DefWindowProc (hWnd, uMsg, wParam, lParam));
	*/
#endif
	return 0;
}


//***********************************************************************************************************************************
BOOL CreateSimWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS	  wc;

	#define NAME "BigBird"
	#define TITLE "F-18"

	// Standard windows initialization

    wc.style			 = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		 = SimWndProc;
    wc.cbClsExtra		 = 0;
    wc.cbWndExtra        = 0;
    wc.hInstance         = hInstance;
    wc.hIcon             = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor           = LoadCursor( NULL, IDC_CROSS );
    wc.hbrBackground     = NULL;
    wc.lpszMenuName      = NAME;
    wc.lpszClassName     = NAME;
    RegisterClass(&wc);

    // Create an Extended style window

	if (hwnd == NULL)
	{
#ifdef _DEBUG
		hwnd = CreateWindowEx(WS_EX_TOPMOST&0,
#else
		hwnd = CreateWindowEx(WS_EX_TOPMOST,
#endif
							  NAME,
							  TITLE,
							  WS_POPUP,// | WS_VISIBLE,
							  0, 0,
							  1024, //800,//640,//
							  768,//600,//480,//
							  NULL,//g_hGameWnd,
							  NULL,
							  hInstance,
							  NULL);

		if (!hwnd)
		{
			ddDisplayMessage("CreateWindowEX - Could not create Window");
			return FALSE;
		}

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);
	}

	//SetCapture(hwnd);
	//SetCursor(wc.hCursor);
	SetFocus(hwnd);
	return TRUE;

}

int InitGlide3Dfx( void )
{
	if( Init3DFX( &TmpPalette[0]) )
	{
#ifndef _DEBUG
		RECT Rect;
		Rect.left = 0;	Rect.top  = 0;	Rect.right = ScreenSize.cx;	Rect.bottom = ScreenSize.cy;
		ClipCursor( &Rect );
#endif
		return TRUE;
	}
	return FALSE;
}

/**************************************************************************
  DDInit

  Description:
    initialize all the DirectDraw/Direct3D specific stuff
 **************************************************************************/


/**************************************************************************
 * FindDeviceCallback
 **************************************************************************/

BOOL CALLBACK FindDeviceCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam)
{
    char ach[128];
    char * szFind = (char *)lParam;

	wsprintf(ach,"%s (%s)",szName, szDevice);


    if (lstrcmpi(szFind, szDevice) == 0 || lstrcmpi(szFind, ach) == 0)
    {
		if( lpGUID)
    		CopyMemory(&g_DDrawGuid, lpGUID, sizeof(GUID));
       	g_pDDrawGuid = &g_DDrawGuid;


        return DDENUMRET_CANCEL;
    }
    return DDENUMRET_OK;

	/*

	if (szFind && *szFind)
	{
		wsprintf(ach,"%s (%s)",szName, szDevice);

    	if (lstrcmpi(szFind, szDevice) == 0 || lstrcmpi(szFind, ach) == 0)
    	{
        	lstrcpy(szFind, ach);
        	CopyMemory(&g_DDrawGuid, lpGUID, sizeof(GUID));
        	g_pDDrawGuid = &g_DDrawGuid;
        	return DDENUMRET_CANCEL;
    	}

	}
	else
	{
    	HRESULT         hr;
    	IDirectDraw     *pDD = NULL;
    	DDCAPS          DriverCaps;
    	DDCAPS          HELCaps;

    	hr = DirectDrawCreate(lpGUID, &pDD, NULL);

    	if (FAILED(hr))
		    return D3DENUMRET_OK;

    	ZeroMemory(&DriverCaps, sizeof(DDCAPS));
    	DriverCaps.dwSize = sizeof(DDCAPS);

    	ZeroMemory(&HELCaps, sizeof(DDCAPS));
    	HELCaps.dwSize = sizeof(DDCAPS);

    	hr = pDD->GetCaps(&DriverCaps, &HELCaps);

    	if (!FAILED(hr))
		{
    		if (DriverCaps.dwCaps & DDCAPS_3D)
    		{
        		// A secondary 3d hardware device found.  Return the DD object
				if( lpGUID)
        			CopyMemory(&g_DDrawGuid, lpGUID, sizeof(GUID));
        		g_pDDrawGuid = &g_DDrawGuid;
        		lstrcpy(szFind, ach);
    			pDD->Release();
    			return D3DENUMRET_CANCEL;
    		}
		}
    	pDD->Release();
	}

    return D3DENUMRET_OK;
	*/
}

int InitDirectDraw( int PixelDepth )
{
	/*
	HRESULT		  ddrval;
	LPDIRECTDRAW  lpDDOld;

	// Create a Direct Draw Object
	if (lpDD == NULL)
	{
		g_pDDrawGuid = NULL;

		ddrval = DirectDrawCreate(g_pDDrawGuid, &lpDDOld, NULL);	// param1 = g_pDDrawGuid = the device selected by the user or a default 3D device
															    // param2 = pointer to a DD object
															    // param3 = NULL = undefined for now
		if (ddrval != DD_OK)
			DisplayDirectDrawError(ddrval,"DirectDrawCreate",0);
	}
	else
		ddrval = DD_OK;

	// ddrval is now a pointer to the iDirectDraw interface (lpDD is pointer)
	if(ddrval == DD_OK)
	{
	    // Set degree to which Direct Draw takes over hardware.
		// param1 = windows handle to app's main window (set above)
		// param2 = specifies cooperation level

		//Use to be g_hGameWnd

		ddrval = lpDDOld->QueryInterface(IID_IDirectDraw4,(LPVOID *)&lpDD);

	    if(ddrval == DD_OK)
	    {
			lpDDOld->Release();

	    	ddrval = lpDD->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );//| DDSCL_NOWINDOWCHANGES);

			if (ddrval != DD_OK)
				DisplayDirectDrawError(ddrval,"SetCooperativeLevel",0);

			// Change display mode of graphics hardware
			// param1 = x resolution
			// param2 = y resolution
			// param3 = bits/pixel  (8 = 256 color indexed mode)

			ddrval = lpDD->SetDisplayMode(ScreenSize.cx, ScreenSize.cy, PixelDepth,0,0);

			if (ddrval != DD_OK)
				DisplayDirectDrawError(ddrval,"SetDisplayMode",0);

	        if(ddrval == DD_OK)  // if mode is possible
				return (TRUE);
		}
	}
	//DisplayDirectDrawError(ddrval,"grddraw.cpp",0);

	*/
	return (FALSE);

}

HWND d3dHwnd;
BOOL CALLBACK BuildDeviceMenuCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam)
{
    HWND hwnd = (HWND)lParam;
    char ach[128];

    HRESULT         hr;
    IDirectDraw     *pDD = NULL;
    DDCAPS          DriverCaps;

    hr = DirectDrawCreate(lpGUID, &pDD, NULL);

    if (FAILED(hr))
		return D3DENUMRET_OK;

    ZeroMemory(&DriverCaps, sizeof(DDCAPS));
    DriverCaps.dwSize = sizeof(DDCAPS);

    hr = pDD->GetCaps(&DriverCaps, NULL);

    if (!FAILED(hr))
	{
    	if ((DriverCaps.dwCaps & DDCAPS_3D) && !(DriverCaps.dwCaps2 & DDCAPS2_NO2DDURING3DSCENE))
    	{
			wsprintf(ach,"%s (%s)",szName, szDevice);
			SendMessage( hwnd, LB_ADDSTRING, 0, (LPARAM) ach);
    	}
	}

    pDD->Release();

    return DDENUMRET_OK;
}

void ListD3DDevices()
{
    DirectDrawEnumerate(BuildDeviceMenuCallback, (LPVOID)d3dHwnd);
}

BOOL CALLBACK DeviceDlgProc( HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT selected;
	switch( iMsg)
	{
		case WM_INITDIALOG:
			d3dHwnd = GetDlgItem(hDlg, IDC_LIST2 );
			ListD3DDevices();
			return TRUE;
		case WM_COMMAND:
			switch( LOWORD( wParam) )
			{
				case IDOK:
					selected = SendMessage( d3dHwnd, LB_GETCURSEL, 0, 0);
					SendMessage( d3dHwnd, LB_GETTEXT,  (WPARAM )selected,  (LPARAM)&bD3DDriverName[0]);
				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;

			}
			break;
	}
	return FALSE;
}



/*
void ShowD3DDevices( void  )
{
	DialogBox( hWinInstance, MAKEINTRESOURCE( IDD_D3DSELECTION), hwnd, (DLGPROC)DeviceDlgProc);

}
*/


int InitDirect3D( )
{
	HRESULT	ddrval;
	DDCAPS	ddcaps;
	LPDIRECTDRAW  lpDDOld;


	if (lpDD != NULL)
	{
		lpDD->Release();
		lpDD = NULL;
	}

	g_pDDrawGuid = NULL;
	DirectDrawEnumerate(FindDeviceCallback,(LPVOID)g_szCurrentVideoDriver);
	if (!g_pDDrawGuid)
		return (FALSE);

	// Draw a texture cursor if we are  in glide or a non primary display device..ie D3D3dfx
	bShowCursorPoly = 0;
	if(strcmp(g_szCurrentVideoDriver, "Primary Display Driver (display)") )	//if( g_pDDrawGuid )
		bShowCursorPoly = 1;


	ddrval = DirectDrawCreate(g_pDDrawGuid, &lpDDOld, NULL);
	if (ddrval != DD_OK)
		return (FALSE);
										//hwnd //g_hGameWnd
	//ddrval = lpDDOld->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT |DDSCL_NOWINDOWCHANGES);

	//if (ddrval != DD_OK)
	//{
	//	lpDDOld->Release();
	//	lpDD = NULL;
	//	g_pDDrawGuid = NULL;
	//	return(FALSE);
	//}
	//else
	//{
	ddrval = lpDDOld->QueryInterface(IID_IDirectDraw4,(LPVOID *)&lpDD);
	if (ddrval != DD_OK)
	{
		lpDD = NULL;
		g_pDDrawGuid = NULL;
		return(FALSE);
	}

	lpDDOld->Release();

	//}

	ddrval = lpDD->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT |DDSCL_NOWINDOWCHANGES);
	if (ddrval != DD_OK)
	{
		if( lpDD)
			lpDD->Release();
		lpDD = NULL;
		g_pDDrawGuid = NULL;
		return(FALSE);
	}


	ZeroMemory(&ddcaps,sizeof(ddcaps));
	ddcaps.dwSize = sizeof(ddcaps);
	lpDD->GetCaps(&ddcaps,NULL);
	if (ddcaps.dwCaps2 & DDCAPS2_NONLOCALVIDMEM)
		TerrainYesNos |= YN_CARD_USES_AGP;


	ddrval = lpDD->QueryInterface(IID_IDirect3D3, (void**)&lpD3D);
	if (ddrval != DD_OK)
	{
		lpDD->Release();
		lpDD = NULL;
		g_pDDrawGuid = NULL;
		return(FALSE);
	}


	ddrval = lpDD->SetDisplayMode(ScreenSize.cx, ScreenSize.cy, 32,0,0); // changed bpp from 16
	if (ddrval != DD_OK)
	{
		lpD3D->Release();
		lpD3D = NULL;
		lpDD->Release();
		lpDD = NULL;
		g_pDDrawGuid = NULL;
		return(FALSE);
	}
	else
		return (TRUE);

	return (FALSE);

}

int LowerResolution( void)
{
	g_Settings.gr.dwGraph &=  ~(GP_GRAPH_RESOLUTION_800|GP_GRAPH_RESOLUTION_1024);

	if( ScreenSize.cx ==  1024)
	{
		ScreenSize.cx =  800;
		ScreenSize.cy =  600;
		g_Settings.gr.dwGraph |=  GP_GRAPH_RESOLUTION_800;
		return 0;
	}
	else
	if( ScreenSize.cx ==  800)
	{
		ScreenSize.cx =  640;
		ScreenSize.cy =  480;
		return 0;
	}
	return 1;

}


BOOL InitDirectDrawStuff()
{
	int done = 0;

	while( !done )
	{
		if(InitDirect3D())
		{
			if (CreateD3DSurface())
			{
				ddCreatePalette(NULL);
				ChooseD3DTextureFormat();
				return (TRUE);
			}
			else
			{

				DestroyD3D();
				DestroyDDPalette();
				DestroyDDSurface();
				done = LowerResolution( );
			}
		}
		else
			done = LowerResolution( );
	}

	//WriteDumpLog( "Error:	Direct Draw Initialization Failure.");
	return FALSE;
}





// It's ugly and against the law...I'll take care of it
extern void	LoadWeaponObjects();
extern void FixUpPlaneWeapons();

/* should go to XVARS.BLD whenever it's free */
GenericFamilyDef *WorldFamilyDefs;
GenericInstanceDef *WorldInstanceDefs;
GenericInstanceDef *MissionInstanceDefs;


//***********************************************************************************************************************************
void LoadAllWorldFiles(const char *world_name,const char *world_path)
{
	LoadWorld(world_name,world_path);
	WorldFamilyDefs = LoadFamilyFile(world_path,world_name);
	WorldInstanceDefs = LoadWorldInstanceDefs(world_path,world_name);
	FixupInstanceDefLinks(WorldFamilyDefs,WorldInstanceDefs);
}


//***********************************************************************************************************************************
// This is temp. - so leave it here
GrBuff *GrTempBack = NULL;

int iTextYPos=0;
int iTextXPos=10;
//***********************************************************************************************************************************
void InitMessageFont()
{
	MessageFont = GrLoadFont(RegPath("Cockpits","mess.fnt"));
}

//***********************************************************************************************************************************
void DestroyMessageFont()
{
	if (MessageFont)
	{
  		GrDestroyFont(MessageFont);
		MessageFont=NULL;
	}
}

//***********************************************************************************************************************************
void DisplayInitText(char *sTxt, int SkipLine, int showrelease )
{
	int debugon = 0;
#ifdef _DEBUG
	debugon = 1;
#endif
	
/*
	if ((showrelease | debugon )&& MessageFont)	// GrTempBack &&
	{
		GrDrawStringClipped(VisibleSurface,MessageFont,10,iTextYPos,sTxt,255);
		iTextYPos+=10;   // Take this out after below
		//if (SkipLine) // This needs to Go Back in once everyone is setting Skipline
		//	iTextYPos+=10;
	}
*/

	if (MessageFont)
	{
		if (!debugon)
		{
			GrDrawStringClipped(VisibleSurface,MessageFont,iTextXPos,iTextYPos,".",255);
			iTextXPos +=  5;
		}
		else
			if (!showrelease)
			{	
				iTextYPos+=10; 
				GrDrawStringClipped(VisibleSurface,MessageFont,10,iTextYPos,sTxt,255);
			}

		if (showrelease)
		{
			GrDrawStringClipped(VisibleSurface,MessageFont,10,iTextYPos,sTxt,255);
			iTextXPos += (sizeof(sTxt)*22);
		}
	}
	WriteDumpLog(sTxt);
	WriteDumpLog("\n");
}

int GetF18Version( char* );

extern void StartEngineSounds();
extern BOOL FastLand;

//***********************************************************************************************************************************

void SetGraphicsSettings()
{

 
	//if( !strcmp( g_szCurrentVideoDriver, VIDDRIVER_3DFX) )
	//	_3dxlChangeStatus(_3DXL_GLIDE);
	//else if( !strcmp( g_szCurrentVideoDriver, VIDDRIVER_SW) )
	//	_3dxlChangeStatus(_3DXL_SOFTWARE );
	//else

	_3dxlChangeStatus(_3DXL_D3D );
	if( g_Settings.gr.dwGraph & GP_GRAPH_RESOLUTION_800)
	{
		ScreenSize.cx =  800;
		ScreenSize.cy =  600;
	}
	else if( g_Settings.gr.dwGraph & GP_GRAPH_RESOLUTION_1024 )
	{
		ScreenSize.cx =  1280;
		ScreenSize.cy =  1024; //iam: temp changed from 640x480 until res detection is automated and using a list. Wrapper dialog has also been updated.
	}
	else
	{
		ScreenSize.cx =  1920; 
		ScreenSize.cy =  1080; //iam: temp changed from 1024x768 until res detection is automated and using a list. Wrapper dialog has also been updated.
	}


	if (g_Settings.gr.dwGraph & GP_GRAPH_HIGH_DETAIL_TEXTURE)
		FastLand = 0;
	else
		FastLand = 1;

	TerrainYesNos = 0;
	if (g_Settings.gr.dwGraph & GP_GRAPH_SHADOWS)
		TerrainYesNos |= YN_DRAW_SHADOWS;

	if (g_Settings.gr.dwGraph & GP_GRAPH_ROADS)
		TerrainYesNos |= YN_DRAW_ROADS;

	if (!(g_Settings.gr.dwGraph & GP_GRAPH_SPECTRAL_LIGHTING))
		TerrainYesNos |= YN_NO_SPECTRAL;

	if (!(g_Settings.gr.dwGraph & GP_GRAPH_TEXTURE_TRANSITIONS))
		TerrainYesNos |= YN_NO_TRANSITIONS;

	//if (g_Settings.gr.byFlags & GP_NOPAGEFLIPPING)
	//	TerrainYesNos |= YN_FLIP_WITH_BLAST;

	if (g_Settings.gr.dwGraph & GP_GRAPH_LOW_LEVEL_NOISE)// GP_TERRAIN_DETAIL)
		TerrainYesNos |= YN_DRAW_DETAIL_MAPS;
	else
		TerrainYesNos &= ~YN_DRAW_DETAIL_MAPS;


	if (g_Settings.gr.dwGraph & GP_GRAPH_LOW_LEVEL_NOISE)// GP_WATER_DETAIL)
		TerrainYesNos |= YN_DRAW_WATER_SURFACE;
	else
		TerrainYesNos &= ~YN_DRAW_WATER_SURFACE;

	if (g_Settings.gr.dwGraph & GP_GRAPH_SPARKLING_WATER)
		TerrainYesNos |= YN_DRAW_SPARKLES;
	else
		TerrainYesNos &= ~YN_DRAW_SPARKLES;


	if (g_Settings.gr.dwGraph & GP_GRAPH_LIGHT_MAPS)
		TerrainYesNos |= YN_DRAW_LIGHT_MAPS;
	else
		TerrainYesNos &= ~YN_DRAW_LIGHT_MAPS;

	if (g_Settings.gr.dwGraph & GP_GRAPH_POINT_LIGHT_SRC	)
		TerrainYesNos |= YN_DRAW_POINT_LIGHTS;
	else
		TerrainYesNos &= ~YN_DRAW_POINT_LIGHTS;

	g_Settings.gr.dwGraph &= ~GP_GRAPH_LIFT_LINE;
	LiftLineFade = 0xFF;


	AmountToDither = 3;

	switch(g_Settings.gr.nNumGroundObjs)
	{
		case GP_REQUIRED_ONLY :
			AmountToObject = BI_DETAIL_0;
			break;

		case GP_SOME :
			AmountToObject = BI_DETAIL_2;
			break;

		case GP_ALL :
			AmountToObject = BI_DETAIL_3;
			break;
	}

	switch(g_Settings.gr.nObjectDetail)
	{
		case GP_DLOW :
			SetHighest3DLOD(2);
			break;
		case GP_DMEDIUM :
			SetHighest3DLOD(1);
			break;
		case GP_DHIGH :
		default:
			SetHighest3DLOD(0);
			break;
	}

	switch(3)//g_Settings.gr.nTerrainTexturing)
	{
		case GP_NONE :
			AmountToTexture =  0;
			break;
		case GP_LOW :
			AmountToTexture =  1;
			break;
		case GP_MEDIUM :
			AmountToTexture =  2;
			break;
		case GP_HIGH :
		default:

			AmountToTexture =  3;
			break;
	}

	switch (g_Settings.gr.nTerrainDistance)
	{
		case GP_NEAR :
			AmountToDraw = DETAIL_HIGH_ONLY;
			break;

		case GP_DMEDIUM :
			AmountToDraw = DETAIL_FIRST_TWO;
			break;

		case GP_FAR :
		default:
			AmountToDraw = DETAIL_NORMAL;
			break;
	}

	if (g_Settings.gr.nGroundSmoke == 1)
		g_Settings.gr.nGroundSmoke = 0;
	else
		if (g_Settings.gr.nGroundSmoke == 0)
			g_Settings.gr.nGroundSmoke = 1;
}


extern void	SetInitialLocationView();


void GetGraphicsSettings()
{
	g_Settings.gr.dwGraph &= ~(GP_GRAPH_SHADOWS | GP_GRAPH_ROADS | GP_GRAPH_SPECTRAL_LIGHTING |  GP_GRAPH_TEXTURE_TRANSITIONS);

	if (g_Settings.gr.nGroundSmoke == 1)
		g_Settings.gr.nGroundSmoke = 0;
	else
		if (g_Settings.gr.nGroundSmoke == 0)
			g_Settings.gr.nGroundSmoke = 1;

	if (TerrainYesNos & YN_DRAW_SHADOWS)
		g_Settings.gr.dwGraph |= GP_GRAPH_SHADOWS;

	if (TerrainYesNos & YN_DRAW_ROADS)
		g_Settings.gr.dwGraph |= GP_GRAPH_ROADS;

	if (!(TerrainYesNos & YN_NO_SPECTRAL))
		g_Settings.gr.dwGraph |= GP_GRAPH_SPECTRAL_LIGHTING;

	if (!(TerrainYesNos & YN_NO_TRANSITIONS))
		g_Settings.gr.dwGraph |= GP_GRAPH_TEXTURE_TRANSITIONS;

	if (TerrainYesNos & YN_DRAW_DETAIL_MAPS)
		g_Settings.gr.dwGraph |= GP_GRAPH_LOW_LEVEL_NOISE;//GP_TERRAIN_DETAIL;

	if (TerrainYesNos & YN_DRAW_SPARKLES)
		g_Settings.gr.dwGraph |= GP_GRAPH_SPARKLING_WATER;

	if (TerrainYesNos & YN_DRAW_WATER_SURFACE)
		g_Settings.gr.dwGraph |= GP_GRAPH_LOW_LEVEL_NOISE;//GP_WATER_DETAIL;

	if (TerrainYesNos & YN_DRAW_POINT_LIGHTS )
		g_Settings.gr.dwGraph |= GP_GRAPH_POINT_LIGHT_SRC;
	else
		g_Settings.gr.dwGraph &= ~GP_GRAPH_POINT_LIGHT_SRC;

	if (TerrainYesNos & YN_DRAW_LIGHT_MAPS)
		g_Settings.gr.dwGraph |= GP_GRAPH_LIGHT_MAPS;
	else
		g_Settings.gr.dwGraph &= ~GP_GRAPH_LIGHT_MAPS;


	switch(AmountToObject & BI_DETAIL)
	{
		case BI_DETAIL_0: g_Settings.gr.nNumGroundObjs = GP_REQUIRED_ONLY; break;
		case BI_DETAIL_1: g_Settings.gr.nNumGroundObjs = GP_REQUIRED_ONLY; break;
		case BI_DETAIL_2: g_Settings.gr.nNumGroundObjs = GP_SOME; break;
		case BI_DETAIL_3: g_Settings.gr.nNumGroundObjs = GP_ALL; break;
	}

	/*
	switch ( AmountToTexture)
	{
		case  0: g_Settings.gr.nTerrainTexturing = GP_NONE; break;
		case  1: g_Settings.gr.nTerrainTexturing = GP_LOW; break;
		case  2: g_Settings.gr.nTerrainTexturing = GP_MEDIUM; break;
		case  3: g_Settings.gr.nTerrainTexturing = GP_HIGH; break;
	}
	*/

	switch ( GetHighest3DLOD())
	{
		case 2: g_Settings.gr.nObjectDetail = GP_DLOW; break;
		case 1: g_Settings.gr.nObjectDetail = GP_DMEDIUM; break;
		case 0: g_Settings.gr.nObjectDetail = GP_DHIGH; break;
	}

	switch ( AmountToDraw )
	{
		case DETAIL_HIGH_ONLY:	g_Settings.gr.nTerrainDistance = GP_NEAR; break;
		case DETAIL_FIRST_TWO:  g_Settings.gr.nTerrainDistance = GP_DMEDIUM; break;
		case DETAIL_NORMAL:     g_Settings.gr.nTerrainDistance = GP_FAR; break;
	}

}

int HasNavFlir(void);


char sDebugTxt[512];


//TEST
void CalcD3DTextureCenter( void);


extern MissionInfoType MissionInfo;  // And that's the bottom line, because...you know the rest

extern void InitDamageVehicles();


//*********************************************************************************************************************
void SetBuilderPhotoView()
{
	if (MissionInfo.lObjectStatus)
	{
		ViewActionType pViewAction;

		pViewAction.lObjectID=MissionInfo.lObjectiveID;
		pViewAction.iViewTypeFlag=1;
		pViewAction.lCameraView=0;
		pViewAction.lEyePointIndex=-1;
		strncpy(pViewAction.sFunction,"VIEW_FIXED_CHASE",35);
		pViewAction.lTime=-1;
		pViewAction.iDefaultView=0;

		switch(MissionInfo.lObjectiveType)
		{
			case AIRCRAFT:
				pViewAction.lTrackingFlag=1;
			break;
			case GROUNDOBJECT:
				pViewAction.lTrackingFlag=4;
			break;
			case MOVINGVEHICLE:
			case SHIP:
				pViewAction.lTrackingFlag=2;
			break;
		}
		AIDoViewAction(&pViewAction);
	}
}

void StartVibeSeat (void)
{
	BOOL					bResult;
	STARTUPINFO				siInfo;
	PROCESS_INFORMATION		prInfo;
	char					tempstr[256];
	char					szPath[260];

	pSeatData = NULL;

	if (GetRegValueL (INTELLIVIBE_REG_STRING))
	{
		if (g_MemMap.Create (IVIBE_DATAFILE, TRUE, sizeof (IVIBESeatData)))
		{
			pSeatData = (IVIBESeatData *) g_MemMap.GetPointer ();
			if (pSeatData)
			{
				memset (pSeatData, 0, sizeof (IVIBESeatData));

				pSeatData->SimPause = -1;

				memset (&siInfo, 0, sizeof (siInfo));
				memset (&prInfo, 0, sizeof (prInfo));

				siInfo.cb = sizeof (siInfo);
				siInfo.dwFlags = STARTF_USESHOWWINDOW;
				siInfo.wShowWindow = SW_MINIMIZE;	// SW_HIDE;

				sprintf (szPath, "%s\\IVibe_F18.exe", g_szExePath);
				sprintf (tempstr, "%d", g_MemMap.GetMapHandle ());

				bResult = CreateProcess (szPath, tempstr, NULL, NULL, TRUE, 0/*DEBUG_PROCESS*/, NULL, NULL, &siInfo, &prInfo);
				sprintf (tempstr, "Starting IVibe...Successful? : %d\n", bResult);
				WriteDumpLog (tempstr);
			}
		}
	}
}

//*********************************************************************************************************************
int SimMain()
{
	char					tempstr[256];

	StartVibeSeat ();

	DoingSimSetup = TRUE;

	_3dxlChangeStatus(_3DXL_NONE);

	RandomSeedHelper = GetTickCount();

	OurShowCursor(FALSE);

	if( g_Settings.ff.bEnable )
		ForceFeedbackInit (g_hGameInst, hwnd);


//#ifdef _DEBUG
//	if (!CreateSimWindow(hWinInstance, nWinCmdShow))
//		return(FALSE);
//#endif

	InitGameSettings();			// Game Settings

	if (MultiPlayer)
		UseMultiSettings ();

	SetGraphicsSettings();

	// Initialze 3Dxl Render
	if( (_3dxl==_3DXL_SOFTWARE) || (_3dxl==_3DXL_D3D))
		if (!InitDirectDrawStuff())
			return (FALSE);


	ValidateRect(g_hGameWnd,NULL);


	//TEST
	CalcD3DTextureCenter( );


	/* -----------------12/17/98 12:42PM-------------------------------------------------------------------
	/* now we'll do the terrain palette
	/* ----------------------------------------------------------------------------------------------------*/

	FillPaletteSlot(FT_FIRE_PALETTE,"firepal");
	FillPaletteSlot(FT_TERRAIN_PALETTE,"landpal");
	FillPaletteSlot(FT_OBJECT_PALETTE,"objpal");
	FillPaletteSlot(FT_COCKPIT_PALETTE,"cpitpal");

	SetTextureFormat(FT_TERRAIN_PALETTE);

	/* -----------------1/12/99 9:59AM---------------------------------------------------------------------
	/* get back any textures that we hid from the D3D release
	/* ----------------------------------------------------------------------------------------------------*/
	RestoreTexturesFromHiding();

	/* -----------------1/12/99 6:28PM---------------------------------------------------------------------
	/* load in the terrain textures for this world
	/* ----------------------------------------------------------------------------------------------------*/

	SetupLandTextures();

	InitSound();

	InitMessageFont();


	char *pszTmp = GetRegValue ("callsign");
	if (pszTmp)
	{
		iMHShowCallSign=1;
	} else {
		iMHShowCallSign=0;
	}


	GrTempBack = NULL;
	iTextYPos=10;

	char	sTxt[80];
	char *	pszPath = GetRegValue ("wrapper");
	char	szFile[260];
	RECT	rcDst;
	RECT	rcSrc;
	DDBLTFX	ddBltFx;

/*
	******
	  **		jjd - Commented out 16 Apr 99, until we decide we have different loading screens going into sim
	******

	char	ch;

#ifdef __DEMO__
	switch (g_iLanguageId)
	{
		case MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH):
			strcpy (sTxt, "fr");
			break;

		case MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN):
			strcpy (sTxt, "gm");
			break;

		default:
			strcpy (sTxt, "en");
			break;
	}

	sprintf (szFile, "%s\\Load_%s.pcx", pszPath, sTxt);
#else
	switch (g_nMissionType)
	{
		case SIM_QUICK:
			ch = 'i'; break;

		case SIM_TRAIN:
			ch = 't'; break;

		case SIM_CAMPAIGN:
			ch = 'c'; break;

		case SIM_MULTI:
			ch = 'm'; break;

		default:
		case SIM_SINGLE:
			ch = 's'; break;
	}
*/

	sprintf (szFile, "%s\\Load_s.bmp", pszPath);
	CSprite		spriteLoadScreen;
	spriteLoadScreen.Load (lpDD, szFile, FALSE);

	SetRect (&rcDst, 0, 0, ScreenSize.cx, ScreenSize.cy);
	SetRect (&rcSrc, 0, 0, 800, 600);
	ddBltFx.dwSize = sizeof (ddBltFx);
	lpDDSPrimary->Blt (&rcDst, spriteLoadScreen.m_pSurf, &rcSrc, DDBLT_WAIT, &ddBltFx);
	spriteLoadScreen.Free ();

	/*  jjd -commented out 16 apr 99 in lieu of bitmap code above
	GrTempBack = _3dxlGetNewOffscreenImage(RegPath("wrapper", szFile));
	*/

	if (GrTempBack)
		_3dxlDisplayPage(GrTempBack);

	CurrentCockpit=-1;

	// DataBase Stuff
	// This should be before all other game initialization code
	iTextXPos=10;
	GetF18Version(sTxt);
	char TmpStr[100];
	sprintf(TmpStr,"Loading %s", sTxt);
	DisplayInitText(TmpStr,1, 1);

	DisplayInitText("Loading Database Info...",1);
	InitDB();		// Load Database info

	//***********************************************************************8
	DisplayInitText(g_szCurrentVideoDriver,1);

	sprintf( tempstr,"%dx%d", ScreenSize.cx, ScreenSize.cy );
	DisplayInitText( tempstr,1);


	InitSoundEffectsVariables();


	//StartReaderThread();
   	InitJoy();
	InitMouse();

   	InitFMath();

	DisplayInitText("Init Renderer...",1);
	InitRenderer();
	DisplayInitText("Init Game Stuff...",1);
  	InitGameStuff();

	DisplayInitText("Init Default Key Assignments...",1);
	InitDefaultKeyAssignments();

//	LoadPaletteForWorld(GetRegValue("WorldName"),GetRegValue("WorldPath"),&WorldPal[0],FadingTable);

	// This must go before WorldInstanceDefs is converted and free'd
	DisplayInitText("Load World DB Requirements...",1);
	LoadWorldDBNeeds();


	DisplayInitText("Converting Family Defs...",1);
	AllFamilies = ConvertFamilyDefs(WorldFamilyDefs);

	DisplayInitText("Converting Instance Defs...",1);
	AllInstances = ConvertInstanceDefs(WorldInstanceDefs);

	DisplayInitText("Creating Instance Links...",1);
	FixupInstanceLinks(AllFamilies,AllInstances);

	free(WorldFamilyDefs);
	WorldFamilyDefs = NULL;

	DisplayInitText("FreeInstanceDefs...",1);
	FreeInstanceDefs(WorldInstanceDefs,FALSE);
	WorldInstanceDefs = NULL;


	//**********************  These two lines need to be after the world is loaded.

	DisplayInitText("AICInitAIRadio...",1);

	AICInitAIRadio();

	DisplayInitText("AICInitAIRadio Complete...",1);

	if(!LANGGetTransMessage(tempstr, 256, AIC_LOADING_MISSION, g_iLanguageId))
	{
		return(FALSE);
	}

//	DisplayInitText(tempstr,0);


	DisplayInitText("InitWayPoints",1);

	InitWaypoints();  //  Moved to after world is loaded so terrain height is correct  SRE

	// Load sky textures
	DisplayInitText("Init Weather...",1);

	//Don't remove these lines, even though the AssignCameraSubject appears to do the same thing, it doesn't.
	PlayerPlane = (PlaneParams *)Camera1.AttachedObject;  //  If this variable is needed, make sure it is after InitWaypoints
	Camera1.AttachedObjectType = CO_PLANE;

	InitWeather();

	DisplayInitText("Preloading Land...",1);
	TerrainLoaded = FALSE;
	PreLoadLand(PlayerPlane->WorldPosition);

//	InitBullets();
	DisplayInitText("Init Weapons...",1);
	InitWeapons();
	DisplayInitText("Fixup Plane Weapons...",1);
	FixUpPlaneWeapons();	// This will write over anything that j is doing

	// This must go after InitWaypoints and the assignment of PlayerPlane
	DisplayInitText("Load Player Weapon Info...",1);
	InitF18LoadoutInfo();
	F18LoadPlayerWeaponInfo();
	DisplayInitText("Load Wingman Weapon Info...",1);
	LoadWingmenWeaponInfo();

	DisplayInitText("Load Weapon Objects...",1);
	LoadWeaponObjects();

	DisplayInitText("Init Ground Defenses...",1);
	GDInitDefenses();

	InitMissionStatistics();

	CurrentView = -1;
	Camera1.SubType = COCKPIT_FRONT;

	// This will get all the info we need for later
	AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

	// set cockpit day/night according to current time of day
    UseDayCockpit = ( (WorldParams.WorldTime <= 19.0 HOURS) && (WorldParams.WorldTime >= 5.0 HOURS ) );
	if (!UseDayCockpit)
    	CockpitLightOn();

	WorldParams.CurrentPaletteNumber = -1;
	SetTimeOfDay(&WorldParams,50000.0);

	InitRoads();
	InitCockpit(PlayerPlane);

	Camera1.TargetHeading = 0.0;
	Camera1.TargetPitch = 0.0;
	Camera1.TargetRoll = 0.0;

	AssignCameraSubject(PlayerPlane,CO_PLANE);

	Camera1.CameraMode = CAMERA_COCKPIT;
	Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
	LoadCockpit( VIRTUAL_MODEL );
	SetupView( Camera1.SubType );
	CockpitSeat = FRONT_SEAT;
	//if (!DrawPopMpds)
	//	OurShowCursor( FALSE );


#if 1
	InitRadar();
	InitUFC();
	InitHUD();
#endif

	InitFFVars();
	InitAvionicsSensor();

	if( _3dxl != _3DXL_D3D)
		if (!UseDayCockpit && HasNavFlir())
			UFC.NavFlirStatus = 1;

	LoadUFCSprites();

	//ForceFeedbackFree();

	//InitLandTexturePals();


	StartEngineSounds();

	//AmountToTexture = LP_DETAIL_HIGH;  THESE should be gamesettings already SetGraphicsSettings
	//AmountToDither  = LP_DETAIL_HIGH;
	//AmountToDraw    = LP_DETAIL_HIGH;

	InitEventLog();
	g_iWeaponID=0;

	FreeLoadDB(); 						// Load Database info
	DoingSimSetup = FALSE;

//	OurShowCursor( TRUE);

	PreViewActionCamera = Camera1;
	PreViewActionCockpitSeat = CockpitSeat;
	PreViewActionCurrentCockpit = CurrentCockpit;

#ifdef __DEMO__
	UFC.APStatus = 1;
	DisplayWarning(AUTO_ALERT,ON,0);
#endif


	SetInitialLocationView();

	if (g_iWrapperMode==SIM_CAMPAIGN)
	{
		InitDamageVehicles();
	}

	if (GrTempBack)
	{
		GrFillBuff(VisibleSurface,0);
		//GrCopyBuff(VisibleSurface,GrTempBack);
		GrFreeGrBuff(GrTempBack);
		GrTempBack=NULL;
	}


	RestartTicks();						// put this last so CurrentTick is accurate

	if (g_iWrapperMode==SIM_BUILDER)
	{
		SimPause ^= 1;
		SetBuilderPhotoView();
	}

	return (1);
}

void ShutdownVibeSeat (void)
{
	if (pSeatData)
	{
		// Send message to IVibe app to close down.
		SendMessage (FindWindow (NULL, "F18_IVibe"), IVIBE_EXIT_MESSAGE, 0, 0);

		// stop seat executable here
		g_MemMap.Close ();
		
		pSeatData = NULL;
	}
}

void SimShutdown (void)
{
	ShutdownVibeSeat ();

	GetGraphicsSettings();
	ShutdownSound ();

	//StopReaderThread();
	GrFillBuff(BackSurface,0);
	GrFlipPage();
	GrFillBuff(BackSurface,0);
	GrFlipPage();

	SndFlushSpeechQueue( CHANNEL_BETTY );
	SndFlushSpeechQueue( CHANNEL_WSO );
	SndFlushSpeechQueue( CHANNEL_WINGMEN );
	SndFlushSpeechQueue( CHANNEL_SPEECH );
	SndEndAllSounds();

	EvaluateMission();

	DumpFlightStatistics();

	ShutdownEventLog();

	ClearLandTextures();
	ShutdownClouds();

//	ClearLandTexturePals();

	CloseSimStuff();
//	if (lpD3DDevice)
//		ReleaseD3DTextureSpace();

	CloseRenderer();

	ShutDown3DFX();

	CloseGameSettings();

	FreeDefaultKeyAssignments();
	ShutdownMouse();

	CleanupMissionStatistics();

	FreeDB();
	CleanUpMissionData();

	DestroyMessageFont();

	ForceFeedbackFree();

	DestroyD3D();
	DestroyDDPalette();
	DestroyDDSurface();
	OurShowCursor(TRUE);

	_3dxlChangeStatus(_3DXL_NONE);
}