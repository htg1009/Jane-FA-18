// DO NOT MODIFY THIS FILE!!

//****************************************************************************************
// This file is a central repository for Global Variables
// This file will be parsed at compile time and create a vars.cpp and
// an extvars.h file
//****************************************************************************************

#include "F18.h"

//#define NewGrCopyRect8 GrCopyRect8
#define NewGrCopyRect8 GrCopyRectNoClip


//****************************************************************************************
// Truly global.  Used by many, many files
//****************************************************************************************

#define DEGREE (0xFFFF/360)
#define DEGREES * DEGREE
#define MAX_PLANE_TYPES 70
#define MAX_PLANES      150

extern PlaneType		 PlaneTypes[MAX_PLANE_TYPES];
extern PlaneParams	     Planes[MAX_PLANES],*CurrentPlane,*LastPlane,*PlayerPlane;
extern CameraInstance	 Camera1;

#define MAX_WEAPON_SLOTS	300
#define MAX_WEAPON_TYPES	120

extern WeaponType	 WeaponTypes[MAX_WEAPON_TYPES];
extern WeaponParams Weapons[MAX_WEAPON_SLOTS],*CurrentWeapon,*LastWeapon;

#define MAX_MOVINGVEHICLE_SLOTS		200
#define MAX_MOVINGVEHICLE_TYPES		100

extern MovingVehicleType MovingVehicleTypeList[MAX_MOVINGVEHICLE_TYPES];
extern MovingVehicleParams MovingVehicles[MAX_MOVINGVEHICLE_SLOTS],*CurrentMovingVehicle,*LastMovingVehicle;


extern WeaponReleaseSimType WeaponReleaseSim;

// Memory defines

#define MEM_CLASS_PERM		0
#define MEM_CLASS_3D_OBJ	1
#define MEM_CLASS_3D_MAP	2
#define MEM_CLASS_3D_TMP	3
#define MEM_CLASS_MATH	    4

extern FPoint Sun;
extern int SimPause ;
extern int JustLeftPausedState ;

extern unsigned int CurrentFrameTicks ;
extern unsigned int LastFrameTicks ;

extern unsigned int TickCount;
extern unsigned int LastTick;
extern unsigned int CurrentTick;
extern unsigned int CurrentLoop;
extern unsigned int DeltaTicks;
extern unsigned int GameLoop;
extern unsigned int GameLoopInTicks;
extern unsigned int LastGameLoop;
extern unsigned int GameLoopsPerFrame;
extern unsigned int FrameCount;
extern float seconds_per_frame;


//****************************************************************************************
// Multiplayer    -  Globals
//****************************************************************************************

#define JOIN 1
#define HOST 2

extern int MultiPlayer;

//****************************************************************************************
// behave.cpp    -  Globals
//****************************************************************************************

extern MBActions		ActList[200];
extern MBWayPoints	WayList[100];
extern int		iWayNumber ;   //  this is a temporary variable until I can get access to planes.h

//****************************************************************************************
// ufc.cpp   -  Globals
//****************************************************************************************

extern GrBuff UFCBuffs[MAX_UFC_CONTROLS*2] ;

extern UFCLedType	*LedPtr;
extern UFCLedType	*LastLedPtr;

#define MAX_MASTER_MODES 3
extern int CurrMasterModeIndex ;
extern int MasterModeOrder[] ;

extern UFCType UFC;

extern UFCLedType	UFCLed[10] ;

extern UFCPushButtonType UFCControl[MAX_UFC_CONTROLS] ;

extern /* ------------------------------------------2/16/98 12:28PM-------------------------------------------
 * Objects for misc 3D purposes
 * ----------------------------------------------------------------------------------------------------*/

ObjectHandler *LeftMavRack,*RightMavRack;
extern ObjectHandler *Meatball,*FuelBasket,*TowedDecoy;

//****************************************************************************************
// miscsprites  -  Globals
//****************************************************************************************

//typedef struct{
//	int		UpSprX, UpSprY, UpSprX1, UpSprY1;
//	int		SprW, SprH;
//	int		ScrX, ScrY;
//	int		Active;
// 	GrBuff  *Buff;
//   } MISCSpriteType;


extern MISCSpriteType FLIRSprites[MAX_FLIR_SPRITES] ;



//****************************************************************************************
// cockpit.cpp   -  Globals
//****************************************************************************************

extern int CockXOfs, CockYOfs;

extern int CurrentCockpit ;

#define NOTHING	0
#define STATIC  1
#define NORM_BLAST 2
#define FAST_BLAST 3
#define SPRITE	4
#define LINE	5

#define MAX_STATUS_LIGHTS	20

#define MASTER_CAUTION   0
#define LOW_ALT          1
#define FLAPS    	     2
#define SPDB     	     3
#define GEAR	         4
#define BRAKE	         5
#define EMIS	         6
#define AUTO_ALERT	     7
#define AA_DISPLAY       8
#define AG_DISPLAY       9
#define NAV_DISPLAY      10
#define INST_DISPLAY     11
#define AI_SAM           12
#define AI_WARN          13
#define SAM_WARN         14
#define PROG_CHAFF       15
#define OBST_LIGHT       16
#define FIRE_LEFT_LIGHT  17
#define FIRE_RIGHT_LIGHT 18
#define TF_FL_LIGHT      19


#define FRONT_SEAT	0
#define BACK_SEAT	1
#define NO_SEAT		2

extern int CockpitSeat ;

extern int	CockpitStatusLinks[MAX_DIFF_COCKPITS][MAX_STATUS_LIGHTS] ;


//****************************************************************************************
// hud.cpp  -  Globals
//****************************************************************************************

extern FMatrix	 HUDAttitude;

#define MAX_HUD_COLOR	11
//int HUDColorChoices[] = {57, 58, 59, 60, 61, 62, 63, 3, 67, 71, 255};
extern int HUDColorChoices[] ;
extern int CurrentHudColorIndex ;
extern int HUDColor ;

extern PlaneParams *OurPlane;
extern GrFontBuff  *LgHUDFont;
extern GrFontBuff  *SmHUDFont;
extern GrFontBuff  *LgLEDFont;
extern GrFontBuff  *MessageFont;
extern int DisplayFrameRate ;

//****************************************************************************************
// joystuff.cpp  -  Globals
//****************************************************************************************

extern JOYCAPS JoyCap1;
extern JOYINFOEX JoyInfoEx1;
extern DWORD dwDeadZone ;
extern WORD  wKeyHit ;
extern POINT ppCurrMouse;
extern POINT ppLastMouse;

extern short wMouseLB ;
extern short wMouseRB ;
extern short wLastMouseLB ;
extern short wLastMouseRB ;

//****************************************************************************************
// main.cpp      -  Globals
//****************************************************************************************

extern float FrameRate ;
extern float OneOverFrameRate ;
//GrFontBuff *FontBuff;

extern int wTransColor;
extern int bActive;

extern HINSTANCE hWinInstance;
extern int nWinCmdShow;

//****************************************************************************************
// views.cpp     -  Globals
//****************************************************************************************

extern int CurrentView;
extern int ViewChange;
extern int ViewChangeCountDown ;

#define COCKPIT_ACM_FRONT		0x00000400
#define COCKPIT_ACM_BACK		0x00000800

#define COCKPIT_ACM_LEFT		0x00001000
#define COCKPIT_ACM_RIGHT		0x00002000

#define COCKPIT_ACM_UP			0x00004000
#define COCKPIT_ACM_DOWN		0x00008000


extern float AcmViewAngles[] ;



//****************************************************************************************
// input.cpp     -  Globals
//****************************************************************************************

extern float sqrtDeltaTicks;

extern int RollStickPos;
extern int PitchStickPos;
extern int Throttle;
extern int Rudder ;

//****************************************************************************************
// flight.cpp     -  Globals
//****************************************************************************************

extern char TmpStr[128];

// Fuel Flow - (Lbs/min)	FF[mach(0.8-2.0)][altitude/1000(0-50)]

extern float FF[12][14] ;

// NEW THRUST DATA
// [ft = 0..70000][ide,Mil,MidAB,MaxAB] // at < 0.8 mach
extern float FFLowMach[15][4] ;

// [ft = 0..70000][ide,Mil,MidAB,MaxAB] // at > 1.1 mach
extern float FFHighMach[15][4] ;


//float AllowableSinkRateTable[19] = {600,600,600,600,600,600,600,600,552,504,468,432,402,372,348,324,294,276,252};
// original adjusted by  50 percent
//float AllowableSinkRateTable[19] = {900,900,900,900,900,900,900,900,828,756,702,648,603,558,522,486,441,414,378};

// Modified to allow carrier landings in f-18.  Bumped up 3x
//float AllowableSinkRateTable[19] = {2700,2700,2700,2700,2700,2700,2700,2700,2484,2268,2106,1944,1809,1674,1566,1458,1323,1242,1134};
extern float AllowableSinkRateTable[19] ;

// Altitude Thrust Data for Jet Engine Model

extern float ATD[3][6][6] ;


// Pressure in (slug/ft^3) per 500 ft.	(0 - 70,000 ft.)

extern double Pressure [] ;


extern StabilityDerivatives F18_Coefficients ;

extern ControlSurfaces F18_ControlLimits ;

extern PlaneStatistics F18_Statistics ;

extern InitPlaneParams F18_PlaneInit ;


extern float KeyBoardRudderPosition ;
extern float KeyCommandedRudderPos ;
extern float BankAngleTarget;

extern int NoseTrimOffset ;
extern int AileronTrimOffset ;
extern int RudderTrimOffset ;

extern VKCODE RDRLeft;
extern VKCODE RDRRight;

extern int CasualAP ;
extern void CheckEasyFlightStuff(PlaneParams *P);

extern DWORD g_dwOverGBitch;
extern DWORD g_dwYawRate ;
extern DWORD g_dwTireSqueel ;
extern DWORD g_dwObstacle ;
extern DWORD g_dwPlaneFireHandle ;

extern int TakeOff ;
extern int TakeOffOnGround;

extern int	TouchDownOneShot ;
extern float BrakeDistanceTraveled;
extern FPoint TouchDownInitialPoint;
extern FPoint TouchDownCurrentPoint;
extern int	BrakesLastFrame ;

extern float   HoldAltitude;
extern float   HoldAltitudeDelta;
extern float   HoldAttitude;
extern float   HoldBankAngle;

extern float BankAngleTargetCurrent;

extern float OldRdrAlt ;
extern float NavOldRdrAlt ;

extern float g_fBingoFuel;




//****************************************************************************************
// radar.cpp     -  Globals
//****************************************************************************************

extern RadarLadderType	AirToAirLadder;

//GrBuff RDRBuffs[NUM_RDR_SYMBOLS];

extern RDRSymType	RDRSym[NUM_RDR_SYMBOLS] ;

extern GrBuff *AARadarBackground;
extern GrBuff *EasyAARadarBackground;

//****************************************************************************************
// ssound.cpp     -  Globals
//****************************************************************************************

extern int iSoundsPaused;



extern DWORD	g_dwInternalEngineRumble ;
extern DWORD	g_dwExternalEngineRumble ;
extern DWORD	g_dwEngineGeneratorWine ;
extern DWORD	g_dwExternalEngineGeneratorWine;
extern DWORD	g_dwBurnerRumble ;
extern DWORD	g_dwExternalBurnerRumble ;
extern DWORD	g_dwWndNoise;
extern DWORD	g_dwAOASndHandle;
extern DWORD	g_dwAIMSndHandle;
extern DWORD	g_dwGunSndHandle;
extern DWORD	g_dwMissileLaunchSound;
extern DWORD	g_dwMissileWarnSound;


extern DWORD	g_dwAIMLockSndHandle;


extern int iWndMode;
extern long g_iPlayerSoundMode;	// 1=Internal,0=External

extern int g_iEngineSoundThrust;
extern int g_iOurPlanePan;
extern int g_iOurPlaneVolume;
extern int g_iProximityHit;
extern int g_iFlameOutTimer;

extern DWORD dwFlameOut1;
extern DWORD dwFlameOut2;

extern BOOL g_bBingoFuel;


extern int	g_iSoundLevelCaution;
extern int	g_iSoundLevelEngineRum;
extern int	g_iSoundLevelEngineGen;
extern int	g_iSoundLevelExternalSFX;
extern int g_iBaseSpeechSoundLevel;
extern int g_iBaseBettySoundLevel;

extern int g_iPrimeSounds;

extern DWORD g_dwExplosionSnd1;
extern DWORD g_dwExplosionSnd2;
extern DWORD g_dwExplosionSnd3;


//****************************************************************************************
// weapons.cpp     -  Globals
//****************************************************************************************

//BulletType Bullets[MAX_BULLETS],*LastBullet;
//BombType Bombs[MAX_BOMBS],*LastBomb;

//****************************************************************************************
// simsound.cpp     -  Globals
//****************************************************************************************

#define SIMSND_ENGINE1	0
#define SIMSND_ENGINE2	1
#define SIMSND_WARNING	2
#define SIMSND_OVERG	705
#define SIMSND_LOWALT	703
#define SIMSND_YAWRATE	707
#define SIMSND_900HZ4	6
#define SIMSND_900HZ10	7
#define SIMSND_250HZP	8
#define SIMSND_900HZP	9
#define SIMSND_VULC		10
#define SIMSND_900HZ	11
#define SIMSND_FUELLOW	709
#define SIMSND_BINGO	702
#define	SIMSND_TFFAIL	706
#define SIMSND_TFOBSTACLE	704
#define SIMSND_YAWRATE	707



//****************************************************************************************
// MPD.cpp     -  Globals
//****************************************************************************************

extern MpdType MPD;
extern MpdViewBoxType MpdViewBoxes[MAX_MPDS];

extern MpdModeInfoType Mpds[7];

extern ButtonClickInfoType ButtonClickInfo;
extern AATargetSelectInfoType AATargetSelectInfo;
extern AATargetSelectInfoType AGButtonClickInfo;
extern AATargetSelectInfoType FlirButtonClickInfo;

extern int PrevHUDMode ; // JLM this may need to be noved to a data structure

extern GrBuff *Buff; // JLM debug remove
extern GrBuff *MonoBuff; // JLM debug remove

extern int MonoPal[256]; // JLM debug

extern int Timer;   // JLM debug


// var used for clearing the mpds. Defines top, middle and bottom boxes
extern MpdClearType MpdClear[9] ;

extern int BYOff ;

extern int CornerX ;

extern ButtonOffsetType FrontButtonOffsets[MAX_MPD_BUTTONS] ;


  extern ButtonOffsetType BackButtonOffsets[MAX_MPD_BUTTONS] ;



extern ButtonOffsetType MpdButtonScreenPos[MAX_MPDS][MAX_MPD_BUTTONS] ;

//*********************************************************************************************
//************ LOOKDOWN SPRITES

extern GrBuff FireExtSwitchBuffs[NUM_FIRE_EXT_SWITCH_SPRITES];
extern GrBuff JetWeapSelectSwitchBuffs[NUM_JET_WEAP_SELECT_SWITCH_SPRITES];
extern GrBuff JetPushBuffs[NUM_JETTISON_PUSH_SPRITES];
extern GrBuff FuelSwitchBuffs[NUM_FUEL_SWITCH_SPRITES];
extern GrBuff LeftDownSwitchBuffs[NUM_LEFT_DOWN_SWITCH_SPRITES];
extern GrBuff RightDownSwitchBuffs[NUM_RIGHT_DOWN_SWITCH_SPRITES];
extern GrBuff DoubleFireBuffs[NUM_DOUBLE_FIRE_SPRITES];
extern GrBuff SingleFireBuffs[NUM_SINGLE_FIRE_SPRITES];
extern GrBuff MasterArmSwitchBuffs[NUM_MASTER_ARM_SPRITES];
extern GrBuff BingoArrowsBuffs[2];

extern LookDownSpriteType FireExtSwitch[NUM_FIRE_EXT_SWITCH_SPRITES] ;

extern LookDownSpriteType JetWeapSelectSwitch[NUM_JET_WEAP_SELECT_SWITCH_SPRITES] ;

extern LookDownSpriteType JetPush[NUM_JETTISON_PUSH_SPRITES] ;

extern LookDownSpriteType FuelSwitch[NUM_FUEL_SWITCH_SPRITES] ;

extern LookDownSpriteType LeftDownSwitch[NUM_LEFT_DOWN_SWITCH_SPRITES] ;

extern LookDownSpriteType RightDownSwitch[NUM_LEFT_DOWN_SWITCH_SPRITES] ;

extern LookDownSpriteType DoubleFire[NUM_DOUBLE_FIRE_SPRITES] ;

extern LookDownSpriteType SingleFire[NUM_SINGLE_FIRE_SPRITES] ;

extern LookDownSpriteType MasterArmSwitch[NUM_MASTER_ARM_SPRITES] ;

extern LookDownSpriteType BingoArrows[2] ;


//*****************************************************************************************************************************************
// HELP NOTEBOOK SPRITES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern GrBuff AvHelpWindowBuffs[AV_NUM_HELP_WINDOW_SPRITES];

extern AvGenericSpriteType AvHelpWindowSprites[AV_NUM_HELP_WINDOW_SPRITES] ;

//*****************************************************************************************************************************************
// ANIM SPRITES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern GrBuff AnimSpritesBuffs[NUM_ANIM_SPRITES];

// X1,Y1, X2,Y2, Width,Height, AnchX,AnchY, NumFrames, Repeat, TotalTime, TimePerFrame
extern AnimSpriteType AnimSprites[NUM_ANIM_SPRITES] ;

//*********************************************************************************************
//************  AA RADAR VARS

extern int AAScanMode;   // for sidewinders, set in AA PACS states are ON and OFF
extern int SniffOn;      // actually used by both AA and AG radars

//*********************************************************************************************
//************  AG RADAR VARS

extern GrBuff *AGScanBuffer;
extern GrBuff *GMTAGScanBuffer;
extern GrBuff *AGMapBuffer;
extern GrBuff *AGMonoMapBuffer;
extern GrBuff *AGMapBackBuffer;
extern int ScanBeamOn;
extern int CreatingHiResMap;

//*********************************************************************************************
//************  TEWS VARS

extern int ChaffTotal;
extern int FlareTotal;

//*********************************************************************************************
//************  MISC STRING VARS

extern char Str1[20];
extern char Str2[20];
extern char Str3[20];

//*********************************************************************************************
//************  MPD COLOR VARS

extern int MpdColors[2][MAX_MPD_COLOR_TYPES] ;

//***********************************************************************************************************************************
// ***** Mpd DamageVars

extern int MpdDamage[MAX_MPD_DAMAGE];

//***********************************************************************************************************************************
// ***** FLIR DamageVars

extern GrBuff *FlirMonoBuff;
extern GrBuff *GbuMonoBuff;
extern GrBuff *MavMonoBuff;

//***********************************************************************************************************************************
// ***** DayNight Cockpit Stuff

extern int UseDayCockpit;

//***********************************************************************************************************************************
// ***** Fuel Vars

extern float BingoPercent;

//***********************************************************************************************************************************
// ***** Time To Fire -- Used by Temperature readings

extern long lMaxTimeToFire;

//***********************************************************************************************************************************
// ***** MPD Programming Vars

extern int MpdProgrammingMatrix[MAX_UNIQUE_MPDS][MAX_NUM_MODES] ;

extern int DefaultMpdProgrammingMatrix[MAX_UNIQUE_MPDS][MAX_NUM_MODES] ;

//*****************************************************************************************************************************************
// WEAP ID GLOBAL TABLE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern int F18WeapIds[] ;