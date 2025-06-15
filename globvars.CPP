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

PlaneType		 PlaneTypes[MAX_PLANE_TYPES];
PlaneParams	     Planes[MAX_PLANES],*CurrentPlane,*LastPlane,*PlayerPlane;
CameraInstance	 Camera1;

#define MAX_WEAPON_SLOTS	300
#define MAX_WEAPON_TYPES	120

WeaponType	 WeaponTypes[MAX_WEAPON_TYPES];
WeaponParams Weapons[MAX_WEAPON_SLOTS],*CurrentWeapon,*LastWeapon;

#define MAX_MOVINGVEHICLE_SLOTS		200
#define MAX_MOVINGVEHICLE_TYPES		100

MovingVehicleType MovingVehicleTypeList[MAX_MOVINGVEHICLE_TYPES];
MovingVehicleParams MovingVehicles[MAX_MOVINGVEHICLE_SLOTS],*CurrentMovingVehicle,*LastMovingVehicle;


WeaponReleaseSimType WeaponReleaseSim;

// Memory defines

#define MEM_CLASS_PERM		0
#define MEM_CLASS_3D_OBJ	1
#define MEM_CLASS_3D_MAP	2
#define MEM_CLASS_3D_TMP	3
#define MEM_CLASS_MATH	    4

FPoint Sun;
int SimPause = 0;
int JustLeftPausedState = 0;

unsigned int CurrentFrameTicks = 0;
unsigned int LastFrameTicks = 0;

unsigned int TickCount;
unsigned int LastTick;
unsigned int CurrentTick;
unsigned int CurrentLoop;
unsigned int DeltaTicks;
unsigned int GameLoop;
unsigned int GameLoopInTicks;
unsigned int LastGameLoop;
unsigned int GameLoopsPerFrame;
unsigned int FrameCount;
float seconds_per_frame;


//****************************************************************************************
// Multiplayer    -  Globals
//****************************************************************************************

#define JOIN 1
#define HOST 2

int MultiPlayer;

//****************************************************************************************
// behave.cpp    -  Globals
//****************************************************************************************

MBActions		ActList[200];
MBWayPoints	WayList[100];
int		iWayNumber = 0;   //  this is a temporary variable until I can get access to planes.h

//****************************************************************************************
// ufc.cpp   -  Globals
//****************************************************************************************

GrBuff UFCBuffs[MAX_UFC_CONTROLS*2] = {NULL};

UFCLedType	*LedPtr;
UFCLedType	*LastLedPtr;

#define MAX_MASTER_MODES 3
int CurrMasterModeIndex = 0;
int MasterModeOrder[] = {0,1,2};

UFCType UFC;

UFCLedType	UFCLed[10] =
{
	{{255,320,383},	 286, NULL, 0},
	{{255,320,383},	 301, NULL, 0},
	{{256,320,382},	 315, NULL, 0},
	{{257,320,381},	 329, NULL, 0},
	{{257,320,381},	 342, NULL, 0},
 //	{{258,320,380},	 356, NULL, 0},

 // {{320,320,380},	 356, NULL, 0},
	{{257,320,381},	 342, NULL, 0},
	{{257,320,381},	 329, NULL, 0},
	{{256,320,382},	 315, NULL, 0},
	{{255,320,383},	 301, NULL, 0},
	{{255,320,383},	 286, NULL, 0},
};

UFCPushButtonType UFCControl[MAX_UFC_CONTROLS] =
{

	{10, 136, 27, 152,
	10, 136-78, 27, 152-78,
	0,0,
	269, 366,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{30, 136, 47, 152,
	30, 136-78, 47, 152-78,
	0,0,
	289, 366,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{50, 136, 67, 152,
	50, 136-78, 67, 152-78,
	0,0,
	309, 366,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{71, 136, 88, 152,
	71, 136-78, 88, 152-78,
	0,0,
	330, 366,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{92, 136, 109, 152,
	92, 136-78, 109, 152-78,
	0,0,
	351, 366,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{10, 155, 27,  171,
	10, 155-78, 27,  171-78,
	0,0,
	269, 385,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{30, 155, 47,  171,
	30, 155-78, 47,  171-78,
	0,0,
	289, 385,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{50, 155, 67,  171,
	50, 155-78, 67,  171-78,
	0,0,
	309, 385,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{71, 155, 88,  171,
	71, 155-78, 88,  171-78,
	0,0,
	330, 385,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{92, 155, 109, 171,
	92, 155-78, 109, 171-78,
	0,0,
	351, 385,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{10, 173, 27,  189,
	10, 173-78, 27,  189-78,
	0,0,
	269, 403,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{30, 173, 47,  189,
	30, 173-78, 47,  189-78,
	0,0,
	289, 403,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{50, 173, 67,  189,
	50, 173-78, 67,  189-78,
	0,0,
	309, 403,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{71, 173, 88,  189,
	71, 173-78, 88,  189-78,
	0,0,
	330, 403,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{92, 173, 109, 189,
	92, 173-78, 109, 189-78,
	0,0,
	351, 403,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{10, 191, 27,  207,			   // a/p button
	10, 191-78, 27,  207-78,
	0,0,
	269, 421,
	0,
	0,
	NULL,
	NULL,
	AutoPilotOnOff},


	{30, 191, 47,  207,
	30, 191-78, 47,  207-78,
	0,0,
	289, 421,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{50, 191, 67,  207,
	50, 191-78, 67,  207-78,
	0,0,
	309, 421,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{71, 191, 88,  207,			  // data button
	71, 191-78, 88,  207-78,
	0,0,
	330, 421,
	0,
	0,
	NULL,
	NULL,
	InitUFCDataMenu},

	{92, 191, 109, 207,				// main	button
	92, 191-78, 109, 207-78,
	0,0,
	351, 421,
	0,
	0,
	NULL,
	NULL,
	InitUFCMainMenu},


	{137,66,154,81,			// ems limit button
	157,66,174,81,
	0,0,
	383,420,
	0,
	0,
	NULL,
	NULL,
	UFCToggleEMIS},

	{114,16,130,23,			// a/a button
	 114,16,130,23,
	0,0,
	274,469,
	0,
	0,
	NULL,
	NULL,
	UFCSetAAMode},

	{138,16,154,23, 	// a/g button
   	 138,16,154,23,
	0,0,
	299,469,
	0,
	0,
	NULL,
	NULL,
	UFCSetAGMode},

	{162,16,179,23, 	// nav button
   	 162,16,179,23,
	0,0,
	322,469,
	0,
	0,
	NULL,
	NULL,
	UFCSetNAVMode},

	{188,16,204,23, 		// inst button
	 187,6,204,13,
	0,0,
	347,469,
	0,
	0,
	NULL,
	NULL,
	UFCSetINSTMode},

	{135,37,147,47,			// ufcl1 button
	121,37,133,47,
	0,0,
	230,280,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{135,37,147,47,	 		// ufcl2 button
	121,37,133,47,
	0,0,
	231,294,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{135,37,147,47,			// ufcl3 button
	121,37,133,47,
	0,0,
	232,308,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{135,37,147,47,			// ufcl4 button
	121,37,133,47,
	0,0,
	232,322,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{135,37,147,47,			// ufcl5 button
	121,37,133,47,
	0,0,
	233,336,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{159,37,171,47,			// ufcr5 button
	 173,37,185,47,
	0,0,
	391,336,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{159,37,171,47,			// ufcr4 button
	 173,37,185,47,
	0,0,
	392,322,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{159,37,171,47,			// ufcr3 button
	 173,37,185,47,
	0,0,
	392,308,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{159,37,171,47,			// ufcr2 button
	 173,37,185,47,
	0,0,
	393,294,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

	{159,37,171,47,			// ufcr1 button
	 173,37,185,47,
	0,0,
	394,280,
	0,
	0,
	NULL,
	NULL,
	UFCTestFunction},

};

/* ------------------------------------------2/16/98 12:28PM-------------------------------------------
 * Objects for misc 3D purposes
 * ----------------------------------------------------------------------------------------------------*/

ObjectHandler *LeftMavRack,*RightMavRack;
ObjectHandler *Meatball,*FuelBasket,*TowedDecoy;

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


MISCSpriteType FLIRSprites[MAX_FLIR_SPRITES] =
{
   {2, 16, 34, 95,    	0,0,   	0, 0,    	FALSE,NULL},
   {37, 16, 69, 95,    	0,0,   	250, 0,    	FALSE,NULL},
   {2, 97, 34, 176,    	0,0,   	0, 103,    	FALSE,NULL},
   {37, 97, 69, 176,   	0,0,   	250,103,    FALSE,NULL}
};



//****************************************************************************************
// cockpit.cpp   -  Globals
//****************************************************************************************

int CockXOfs, CockYOfs;

int CurrentCockpit = -1;

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

int CockpitSeat = FRONT_SEAT;

int	CockpitStatusLinks[MAX_DIFF_COCKPITS][MAX_STATUS_LIGHTS] =
{
	{21, 22, 23, 24, 25, 26,30,31,32,33,34,35,36,37,-1,37,38,-1,-1,39},
	{ 9,  8, 12, 13, 10, 11,14,15,16,17,18,19,20,-1,-1,21,22,-1,-1,39},
	{10, -1, -1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1,11,12,-1,13,39,40,41}
};


//****************************************************************************************
// hud.cpp  -  Globals
//****************************************************************************************

FMatrix	 HUDAttitude;

#define MAX_HUD_COLOR	11
//int HUDColorChoices[] = {57, 58, 59, 60, 61, 62, 63, 3, 67, 71, 255};
int HUDColorChoices[] = {57, 58, 59, 60, 61, 62, 63, 249, 252, 248, 253};
int CurrentHudColorIndex = 5;
int HUDColor = 59;

PlaneParams *OurPlane;
GrFontBuff  *LgHUDFont;
GrFontBuff  *SmHUDFont;
GrFontBuff  *LgLEDFont;
GrFontBuff  *MessageFont;
int DisplayFrameRate = 0;

//****************************************************************************************
// joystuff.cpp  -  Globals
//****************************************************************************************

JOYCAPS JoyCap1;
JOYINFOEX JoyInfoEx1;
DWORD dwDeadZone = 0;
WORD  wKeyHit = 0;
POINT ppCurrMouse;
POINT ppLastMouse;

short wMouseLB = 0;
short wMouseRB = 0;
short wLastMouseLB = 0;
short wLastMouseRB = 0;

//****************************************************************************************
// main.cpp      -  Globals
//****************************************************************************************

float FrameRate = 10.0;
float OneOverFrameRate = 0.10;
//GrFontBuff *FontBuff;

int wTransColor=0;
int bActive=0;

HINSTANCE hWinInstance;
int nWinCmdShow;

//****************************************************************************************
// views.cpp     -  Globals
//****************************************************************************************

int CurrentView;
int ViewChange;
int ViewChangeCountDown = 0;

#define COCKPIT_ACM_FRONT		0x00000400
#define COCKPIT_ACM_BACK		0x00000800

#define COCKPIT_ACM_LEFT		0x00001000
#define COCKPIT_ACM_RIGHT		0x00002000

#define COCKPIT_ACM_UP			0x00004000
#define COCKPIT_ACM_DOWN		0x00008000


float AcmViewAngles[] ={(float)0x0000, (float)0x0000,	//COCKPIT_ACM_FRONT
						(float)0x0000, (float)0x0000,	//COCKPIT_ACM_FRONT
						(float)0x8000, (float)0x0000,	//COCKPIT_ACM_BACK
						(float)0x0000, (float)0x0000,

						(float)0x4000, (float)0x0000,	//COCKPIT_ACM_LEFT
						(float)0x2000, (float)0x0000,	//COCKPIT_ACM_LEFT | COCKPIT_ACM_FRONT
						(float)0x6D00, (float)0x0000,	//COCKPIT_ACM_LEFT | COCKPIT_ACM_BACK
						(float)0x4000, (float)0x0000,

						(float)-0x4000,(float)0x0000,	//COCKPIT_ACM_RIGHT
						(float)-0x2000,(float)0x0000,	//COCKPIT_ACM_RIGHT | COCKPIT_ACM_FRONT
						(float)-0x6D00,(float)0x0000,	//COCKPIT_ACM_RIGHT | COCKPIT_ACM_BACK
						(float)-0x4000,(float)0x0000,

						(float)0x0000, (float)0x0000,
						(float)0x0000, (float)0x0000,
						(float)0x8000, (float)0x0000,
						(float)0x0000, (float)0x0000,

						(float)0x0000, (float)0x3800,	//COCKPIT_ACM_UP
						(float)0x0000, (float)0x2000,	//COCKPIT_ACM_UP | COCKPIT_ACM_FRONT
						(float)0x8000, (float)0x1600,	//COCKPIT_ACM_UP | COCKPIT_ACM_BACK
						(float)0x0000, (float)0x3800,

						(float)0x4000, (float)0x2000,	//COCKPIT_ACM_UP | COCKPIT_ACM_LEFT
						(float)0x2000, (float)0x2000,	//COCKPIT_ACM_UP | COCKPIT_ACM_LEFT | COCKPIT_ACM_FRONT
						(float)0x6D00, (float)0x1800,	//COCKPIT_ACM_UP | COCKPIT_ACM_LEFT | COCKPIT_ACM_BACK
						(float)0x4000, (float)0x2000,

						(float)-0x4000,(float)0x2000,	//COCKPIT_ACM_UP | COCKPIT_ACM_RIGHT
						(float)-0x2000,(float)0x2000,	//COCKPIT_ACM_UP | COCKPIT_ACM_RIGHT | COCKPIT_ACM_FRONT
						(float)-0x6D00,(float)0x1800,	//COCKPIT_ACM_UP | COCKPIT_ACM_RIGHT | COCKPIT_ACM_BACK
						(float)-0x4000,(float)0x2000,

						(float)0x0000, (float)0x3800,
						(float)0x0000, (float)0x2000,
						(float)0x8000, (float)0x2000,
						(float)0x0000, (float)0x3800,

						(float)0x0000, (float)-0x1800,	//COCKPIT_ACM_DOWN
						(float)0x0000, (float)-0x1800,  //COCKPIT_ACM_DOWN | COCKPIT_ACM_FRONT
						(float)0x8000, (float)-0x1800,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_BACK
						(float)0x0000, (float)0x0000,

						(float)0x4000, (float)-0x1800,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_LEFT
						(float)0x2000, (float)-0x640,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_LEFT | COCKPIT_ACM_FRONT
						(float)0x6D00, (float)-0x1800,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_LEFT | COCKPIT_ACM_BACK
						(float)0x4000, (float)-0x1800,

						(float)-0x4000,(float)-0x1800,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_RIGHT
						(float)-0x2000,(float)-0x640,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_RIGHT | COCKPIT_ACM_FRONT
						(float)-0x6D00,(float)-0x1800,	//COCKPIT_ACM_DOWN | COCKPIT_ACM_RIGHT | COCKPIT_ACM_BACK
						(float)-0x4000,(float)-0x1800,

						(float)0x0000, (float)0x0000,
						(float)0x0000, (float)0x0000,
						(float)0x8000, (float)-0x1800,
						(float)0x0000, (float)0x0000,
					};



//****************************************************************************************
// input.cpp     -  Globals
//****************************************************************************************

float sqrtDeltaTicks;

int RollStickPos=0;
int PitchStickPos=0;
int Throttle=0;
int Rudder = 0;

//****************************************************************************************
// flight.cpp     -  Globals
//****************************************************************************************

char TmpStr[128];

// Fuel Flow - (Lbs/min)	FF[mach(0.8-2.0)][altitude/1000(0-50)]

float FF[12][14] =

	{{280.0f, 350.0f, 1000.0f, 2200.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f,	3000.0f, 3000.0f},
	 {220.0f, 300.0f, 625.0f,  1800.0f, 2500.0f, 2700.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f,	3000.0f, 3000.0f},
	 {180.0f, 250.0f, 450.0f,  1300.0f, 1900.0f, 2300.0f, 2700.0f, 2850.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f,	3000.0f, 3000.0f},
	 {150.0f, 200.0f, 360.0f,  900.0f,  1350.0f, 1800.0f, 2250.0f, 2625.0f, 3000.0f, 3000.0f, 3000.0f, 3000.0f,	3000.0f, 3000.0f},
	 {125.0f, 160.0f, 300.0f,  700.0f,  1000.0f, 1350.0f, 1700.0f, 2125.0f, 2550.0f, 2775.0f, 3000.0f, 3000.0f,	3000.0f, 3000.0f},
	 {100.0f, 140.0f, 250.0f,  540.0f,  730.0f,	 990.0f,  1250.0f, 1575.0f, 1900.0f, 2200.0f, 2500.0f, 2590.0f,	2680.0f, 2680.0f},
	 {90.0f,  120.0f, 200.0f,  420.0f,  550.0f,	 725.0f,  900.0f,  1140.0f, 1380.0f, 1690.0f, 2000.0f, 2100.0f,	2300.0f, 2300.0f},
	 {80.0f,  100.0f, 170.0f,  355.0f,  450.0f,	 565.0f,  680.0f,  830.0f,  980.0f,	 1190.0f, 1400.0f, 1600.0f,	1900.0f, 1900.0f},
	 {80.0f,  80.0f,  170.0f,  320.0f,  380.0f,	 465.0f,  550.0f,  665.0f,  780.0f,	 940.0f,  1100.0f, 1300.0f,	1500.0f, 1500.0f},
	 {140.0f, 100.0f, 200.0f,  330.0f,  360.0f,	 420.0f,  480.0f,  550.0f,  620.0f,	 755.0f,   890.0f, 1045.0f,	1200.0f, 1200.0f},
	 {300.0f, 200.0f, 280.0f,  350.0f,  380.0f,	 410.0f,  440.0f,  495.0f,  550.0f,	 650.0f,   750.0f, 875.0f,	1000.0f, 1000.0f},
	 {300.0f, 200.0f, 280.0f,  350.0f,  380.0f,	 410.0f,  440.0f,  495.0f,  550.0f,	 650.0f,   750.0f, 875.0f,	1000.0f, 1000.0f}};

// NEW THRUST DATA
// [ft = 0..70000][ide,Mil,MidAB,MaxAB] // at < 0.8 mach
float FFLowMach[15][4] = {15 ,220 ,700 ,1075,
                          14 ,195 ,605 ,940,
						  12 ,170 ,523 ,810,
						  11 ,150 ,450 ,695,
						  9  ,125 ,395 ,585,
						  8  ,110 ,320 ,480,
						  7  ,90  ,265 ,390,
						  5  ,73  ,210 ,300,
						  4  ,60  ,160 ,230,
						  4  ,50  ,120 ,160,
						  3  ,40  ,90  ,120,
						  3  ,30  ,65  ,90,
						  2  ,20  ,50  ,65,
						  2  ,13  ,40  ,50,
						  1  ,8   ,30  ,40     };

// [ft = 0..70000][ide,Mil,MidAB,MaxAB] // at > 1.1 mach
float FFHighMach[15][4] = {17 ,240 ,860 ,1300,
                           15 ,220 ,740 ,1150,
						   14 ,195 ,640 ,1000,
						   12 ,170 ,550 ,860,
						   11 ,150 ,470 ,725,
						   9  ,125 ,400 ,605,
						   8  ,110 ,330 ,500,
						   7  ,90  ,275 ,400,
						   6  ,73  ,220 ,320,
						   5  ,60  ,165 ,220,
						   4  ,50  ,120 ,165,
						   4  ,40  ,100 ,120,
						   3  ,30  ,80  ,100,
						   3  ,20  ,60  ,80,
						   2  ,13  ,40  ,60     };


//float AllowableSinkRateTable[19] = {600,600,600,600,600,600,600,600,552,504,468,432,402,372,348,324,294,276,252};
// original adjusted by  50 percent
//float AllowableSinkRateTable[19] = {900,900,900,900,900,900,900,900,828,756,702,648,603,558,522,486,441,414,378};

// Modified to allow carrier landings in f-18.  Bumped up 3x
//float AllowableSinkRateTable[19] = {2700,2700,2700,2700,2700,2700,2700,2700,2484,2268,2106,1944,1809,1674,1566,1458,1323,1242,1134};
	//iam: commented following line:
	//float AllowableSinkRateTable[19] = {1035,1035,1035,1035,1035,1035,1035,1035,952,869,786,786,786,786,786,786,786,786,786};
// iam: Modified sink rate table to fix the 800fpm carrier trap explosion.
// iam: Need official data for this, but according to google research:
// iam:	 -800fpm is the "operational" fpm limit for carrier landing
// iam:  -600 to -1000fpm is the "acceptable range"
// iam:  -1500fpm is the structural limit
// iam:  Test table based on unofficial information - retail values bumped up by 80%
float AllowableSinkRateTable[19] = {1863,1863,1863,1863,1863,1863,1863,1863,1713,1564,1414,1414,1414,1414,1414,1414,1414,1414,1414};

// Altitude Thrust Data for Jet Engine Model

float ATD[3][6][6] = {

	//Idle
	{{1060.0,670.0,880.0,1140.0,1500.0,1860.0},
	 {635.0,425.0,690.0,1010.0,1330.0,1700.0},
	 {60.0,25.0,345.0,755.0,1130.0,1525.0},
	 {-1020.0,-710.0,-300.0,350.0,910.0,1360.0},
	 {-2700.0,-1900.0,-1300.0,-247.0,600.0,1100.0},
	 {-3600.0,-1400.0,-595.0,-342.0,-200.0,700.0}},

	//Mil
	{{12680.0,9150.0,6200.0,3950.0,2450.0,1400.0},
	 {12680.0,9150.0,6313.0,4040.0,2470.0,1400.0},
	 {12610.0,9312.0,6610.0,4290.0,2600.0,1560.0},
	 {12640.0,9839.0,7090.0,4660.0,2840.0,1660.0},
	 {12390.0,10176.0,7750.0,5320.0,3250.0,1930.0},
	 {11680.0,9848.0,8050.0,6100.0,3800.0,2310.0}},

	//Max
	{{20000.0,15000.0,10800.0,7000.0,4000.0,2500.0},
	{21420.0,15700.0,11225.0,7323.0,4435.0,2600.0},
	{22700.0,16860.0,12250.0,8154.0,5000.0,2835.0},
	{24240.0,18910.0,13760.0,9285.0,5700.0,3215.0},
	{26070.0,21075.0,15975.0,11115.0,6860.0,3950.0},
	{28886.0,23319.0,18300.0,13484.0,8642.0,5057.0}}
};


// Pressure in (slug/ft^3) per 500 ft.	(0 - 70,000 ft.)

double Pressure [] = {

	0.0023769,0.0023423,0.0023081,0.0022743,0.0022409,0.0022079,0.0021752,0.0021429,0.0021110,
	0.0020794,0.0020482,0.0020174,0.0019869,0.0019567,0.0019270,0.0018975,0.0018685,0.0018397,
	0.0018113,0.0017833,0.0017556,0.0017282,0.0017011,0.0016744,0.0016480,0.0016219,0.0015961,
	0.0015707,0.0015455,0.0015207,0.0014962,0.0014719,0.0014480,0.0014244,0.0014011,0.0013781,
	0.0013553,0.0013329,0.0013107,0.0012889,0.0012673,0.0012459,0.0012249,0.0012041,0.0011836,
	0.0011634,0.0011435,0.0011238,0.0011043,0.0010852,0.0010663,0.0010476,0.0010292,0.0010110,
	0.00099311,0.00097544,0.00095801,0.00094082,0.00092387,0.00090716,0.00089068,0.00087443,
	0.00085841,0.00084261,0.00082704,0.00081169,0.00079656,0.00078165,0.00076696,0.00075247,
	0.00073820,0.00072413,0.00071028,0.00069443,0.00067800,0.00066196,0.00064629,0.00063100,
	0.00061608,0.00060150,0.00058727,0.00057338,0.00055982,0.00054658,0.00053365,0.00052103,
	0.00050871,0.00049668,0.00048493,0.00047346,0.00046227,0.00045134,0.00044067,0.00043025,
	0.00042008,0.00041015,0.00040045,0.00039099,0.00038175,0.00037272,0.00036391,0.00035531,
	0.00034692,0.00033872,0.00033072,0.00032290,0.00031527,0.00030782,0.00030055,0.00029345,
	0.00028652,0.00027975,0.00027314,0.00026669,0.00026039,0.00025424,0.00024824,0.00024238,
	0.00023665,0.00023107,0.00022561,0.00022028,0.00021508,0.00021001,0.00020505,0.00020021,
	0.00019548,0.00019087,0.00018636,0.00018196,0.00017767,0.00017348,0.00016938,0.00016539,
	0.00016148,0.00015767,0.00015395,0.00015032,0.00014678,0.00014331,0.00013993,0.00013993
};


StabilityDerivatives F18_Coefficients = {

//  Value		// term     - units

	0.0184,		//CD0		- none
	1.9384,		//CDa		- 1/radians * alpha
	-0.0212,	//CDb		- 1/radians
	0.1277,		//CDdde		- 1/radians
	0.0788,		//CDdf		- 1/radians
	0.0660,		//CDdsb		- 1/radians

//	0.0142,		//CL0		- none
	0.07,		//CL0		- none
//	3.823,		//CLa		- 1/radians
	4.823,		//CLa		- 1/radians
	14.0,		//CLq		- 1/radians
    0.6738,		//CLdde		- 1/radians
	0.1625,		//CLdf		- 1/radians

	-0.9056,   //CYb		- 1/radians	  		-0.7947,
	-0.0701,   //CYdde		- 1/radians	  		-0.0944,
	 0.1492,   //CYdr		- 1/radians	  		0.1387,
	-0.0047,   //CYda		- 1/radians	  		-0.0126,

	-0.1197,	//Clb		- 1/radians
	-0.29,		//Clp		- 1/radians
	0.04,		//Clr		- 1/radians
	0.0080,		//Cldr		- 1/radians
	0.0837,		//Clda		- 1/radians
	0.1039,		//Cldde		- 1/radians

	0.02,		//Cm0		- none
	0.0,		//Cmu		- 1/radians
	-0.1908,	//Cma		- 1/radians
	-3.8,		//Cmq		- 1/radians
	-0.6778,	//Cmdde		- 1/radians
	0.0271,		//Cmdf		- 1/radians
	0.02354,	//Cmdsb		- 1/radians
	-0.0040,	//Cmdlg		- 1/radians

   	 0.1638,	//Cnb		- 1/radians	    		0.1438,
   	-0.0232,	//Cnp		- 1/radians	    		-0.1,
   	-0.2915,	//Cnr		- 1/radians	    		-0.4,
   	-0.0712,	//Cndr		- 1/radians	    		-0.0653,
   	 0.00206,	//Cnda		- 1/radians	   			0.0103,
   	 0.0291,	//Cndde		- 1/radians	    		0.0489,
};

ControlSurfaces F18_ControlLimits = {

	100.0,			//AileronRate     - deg./sec.
	105.0,			//RudderRate      - deg./sec.
	46.0,			//ElevatorRate    - deg./sec.
	40.0,			//FlapRate        - deg./sec.
	40.0,			//FlapRate        - deg./sec.
	30.0,			//SpeedBrakeRate  - deg./sec.

//	{-25.0, 25.0},	//AileronRange
	{-40.0, 40.0},	//AileronRange
	{-30.0, 30.0},	//RudderRange
	{-24.0, 20.0},	//SymetricalElevatorRange
	{-22.0, 22.0},	//DifferentialElevatorRange
	{  0.0, 40.0},	//FlapRange
	{  0.0, 34.0},	//LEFlapRange
	{  0.0, 45.0}	//SpeedBrafkeRange
};

PlaneStatistics F18_Statistics = {

	 48100.0,			//Weight (lbs.)
	 48100.0/32.0,  	//Mass (weight/gravity)
	 25500.0,			//Ix
	 166800.0,			//Iy
	 187100.0,			//Iz
	 608.0,				//S
	 42.83,				//b
	 15.94,				//c
};

InitPlaneParams F18_PlaneInit = {

	// General Header/Flag Stuff
	PL_DEVICE_DRIVEN,												//int		Status;
	(PL_LEFT_ENGINE_ON | PL_RIGHT_ENGINE_ON),						//int		EngineStatus;


	// timing variables
	HIGH_AERO,														//int		UpdateRate;
	HIGH_AERO,		   												//int		AeroComplexity;
	HIGH_FREQ,	 													//float	dt;

	// Body Frame Data
	-999.0, 														//float			LastAlpha;		// previous angle of attack for time delta derivatives

	// Inertial Frame Data
	UNITPLANENORTH,													//InitRefFrame	Orientation;	// aircraft orientation vectors in inertial frame - normalized
	9.0,															//float			AvailGForce;	// currently computed max g-force that will over g the plane

	// Airframe Systems
	PL_AP_TRIMMED,													//int				AutoPilotMode;
	FLAPS_FULL,
	FLAPS_FULL,
	0.0,															//float				CurrentTrimValue;

	// Misc weapons system stuff
	1,																//int				GunFireRate;			// rate loop runs at 50hz. so init to 5 for a 1/10 sec

	// Pointers to Data for Simulation Model
	&F18_Statistics,												//PlaneStatistics			*Stats;
	&F18_Coefficients,												//StabilityDerivatives	*C;
	&F18_ControlLimits,												//ControlSurfaces			*CS;

	// Data for Simple AI Flight Model
	0.0,															//float			MaxSpeedAccel;				// ft/second^2
	0.0,															//float			SpeedPercentage;			// (0.0 - 1.0) -> 0.5 is 50% of MaxSpeed Accelerat

	45.0,															//float			MaxRollRate;				// degrees/second
	1.0,															//float			RollPercentage;				// (0.0 - 1.0) -> 0.5 is 50% of MaxRollRate

	45.0,															//float			MaxPitchRate;				// degrees/second
	1.0,															//float			PitchPercentage;			// (0.0 - 1.0) -> 0.5 is 50% of MaxPitchRate

	20.0,															//float			YawRate;					// degrees/second
	1,																//int				RollYawCouple;				// 1 for YES, 0 for NO

	//	Pointers to Functions for Simulation Model

	GetF18StickPos,													//void (*DoGetPlayerInputs)(struct _PlaneParams *P);

    CalcF18ControlSurfaces,											//void (*DoControlSim)(struct _PlaneParams *P);
    CalcF18CASUpdates,												//void (*DoCASSim)(struct _PlaneParams *P);
    CalcAeroForces,													//void (*DoForces)(struct _PlaneParams *P);
    CalcAeroDynamics,												//void (*DoPhysics)(struct _PlaneParams *P);
	CalcAttitude,													//void (*DoAttitude)(struct _PlaneParams *P);

	CalcSimpleFlight,												//void (*DoSimpleFlight)(struct _PlaneParams *P);
	CalcSimpleAttitude												//void (*DoSimpleAttitude)(struct _PlaneParams *P);
};


float KeyBoardRudderPosition = 0.0f;
float KeyCommandedRudderPos = 0.0f;
float BankAngleTarget;

int NoseTrimOffset = 0;
int AileronTrimOffset = 0;
int RudderTrimOffset = 0;

VKCODE RDRLeft;
VKCODE RDRRight;

int CasualAP = 0;
void CheckEasyFlightStuff(PlaneParams *P);

DWORD g_dwOverGBitch=0;
DWORD g_dwYawRate = 0;
DWORD g_dwTireSqueel = 0;
DWORD g_dwObstacle = 0;
DWORD g_dwPlaneFireHandle = 0;

int TakeOff = 0;
int TakeOffOnGround;

int	TouchDownOneShot = 0;
float BrakeDistanceTraveled;
FPoint TouchDownInitialPoint;
FPoint TouchDownCurrentPoint;
int	BrakesLastFrame = 0;

float   HoldAltitude;
float   HoldAltitudeDelta;
float   HoldAttitude;
float   HoldBankAngle;

float BankAngleTargetCurrent;

float OldRdrAlt = -1.0;
float NavOldRdrAlt = -1.0;

float g_fBingoFuel=4000.0f;




//****************************************************************************************
// radar.cpp     -  Globals
//****************************************************************************************

RadarLadderType	AirToAirLadder;

//GrBuff RDRBuffs[NUM_RDR_SYMBOLS];

RDRSymType	RDRSym[NUM_RDR_SYMBOLS] =
{
	{ NULL, 0,0,4,4,       0,0, 2,2 },
	{ NULL, 6,0,9,6,       0,0, 0,3 },
	{ NULL, 11,0,17,3,     0,0, 2,3 },
	{ NULL, 19,0,24,8,     0,0, 4,3 },
	{ NULL, 29,1,35,6,     0,0, 3,2 },
	{ NULL, 41,2,45,4,     0,0, 2,2 },
	{ NULL, 50,2,53,4,     0,0, 2,2 },
	{ NULL, 62,1,68,6,     0,0, 3,2 },
	{ NULL, 72,2,76,4,     0,0, 2,1 },
	{ NULL, 82,2,86,4,     0,0, 2,1 },
	{ NULL, 90,1,92,5,     0,0, 1,2 },
	{ NULL, 96,1,98,5,     0,0, 1,2 },
	{ NULL, 157,1,171,15,  0,0, 5,7 },
    { NULL, 181,3,193,15,  0,0, 4,7 },
    { NULL, 200,1,214,15,  0,0, 5,5 },
    { NULL, 231,2,243,15,  0,0, 6,5 },
	{ NULL, 270,4,284,18,  0,0, 7,7 },
    { NULL, 308,6,320,18,  0,0, 6,6 },
    { NULL, 327,7,335,18,  0,0, 4,5 },
    { NULL, 343,7,353,17,  0,0, 5,6 },
    { NULL, 416,8,427,20,  0,0, 6,6 },
    { NULL, 440,8,450,17,  0,0, 5,4 },
    { NULL, 461,8,471,17,  0,0, 5,4 },
    { NULL, 477,7,495,17,  0,0, 8,5 },
    { NULL, 313,26,319,32, 0,0, 3,3 },
    { NULL, 329,27,335,34, 0,0, 3,3 },
    { NULL, 343,27,349,34, 0,0, 3,4 },
    { NULL, 367,28,375,36, 0,0, 4,4 },
    { NULL, 447,35,455,39, 0,0, 4,0 },
    { NULL, 466,32,470,40, 0,0, 4,4 },
    { NULL, 483,34,491,38, 0,0, 4,4 },
    { NULL, 501,31,505,39, 0,0, 0,4 },
    { NULL, 449,53,459,63, 0,0, 5,5 },
    { NULL, 8,15,11,21,    0,0, 3,3 },
    { NULL, 18,16,22,20,   0,0, 2,2 },
	{ NULL, 31,10,52,28,   0,0, 10,9},
	{ NULL, 70,16,74,20,   0,0, 2,2 },
	{ NULL, 100,16,108,22, 0,0, 3,4 },
    { NULL, 84,18,92,22,   0,0, 4,0 },
    { NULL, 111,8,135,32,  0,0, 12,12 },
    { NULL, 146,20,150,27, 0,0, 0,0 },
    { NULL, 153,20,161,27, 0,0, 0,0 },

    { NULL, 136,211,149,224, 0,0, 7,5 },
    { NULL, 152,211,165,224, 0,0, 7,5 },
    { NULL, 168,211,181,224, 0,0, 7,5 },
    { NULL, 184,211,197,224, 0,0, 7,5 },
    { NULL, 200,211,213,224, 0,0, 7,5 },
    { NULL, 200,227,213,240, 0,0, 7,5 },
    { NULL, 200,243,213,256, 0,0, 7,5 },
    { NULL, 200,259,213,272, 0,0, 7,5 },
    { NULL, 200,275,213,288, 0,0, 7,5 },
    { NULL, 184,275,197,288, 0,0, 7,5 },
    { NULL, 168,275,181,288, 0,0, 7,5 },
    { NULL, 152,275,165,288, 0,0, 7,5 },
    { NULL, 136,275,149,288, 0,0, 7,5 },
    { NULL, 136,259,149,272, 0,0, 7,5 },
    { NULL, 136,243,149,256, 0,0, 7,5 },
    { NULL, 136,227,149,240, 0,0, 7,5 },
    { NULL, 72,179, 85,192,  0,0, 6,6 },
  	{ NULL, 104,179,117,192, 0,0, 7,6 },
    { NULL, 72,211,85,224,   0,0, 6,6 },
    { NULL, 299,174,305,180, 0,0, 3,3 },
    { NULL, 307,174,313,180, 0,0, 3,3 },
    { NULL, 315,174,321,180, 0,0, 3,3 },
    { NULL, 323,174,329,180, 0,0, 3,3 },
    { NULL, 299,182,305,188, 0,0, 3,3 },
    { NULL, 307,182,313,188, 0,0, 3,3 },
    { NULL, 315,182,321,188, 0,0, 3,3 },
    { NULL, 323,182,329,188, 0,0, 3,3 },
    { NULL, 299,190,305,196, 0,0, 3,3 },
    { NULL, 307,190,313,196, 0,0, 3,3 },
    { NULL, 315,190,321,196, 0,0, 3,3 },
    { NULL, 323,190,329,196, 0,0, 3,3 },
    { NULL, 203,22,213,32,   0,0, 5,5 },
    { NULL, 79, 28,85, 33,   0,0, 3,3 },
    { NULL, 528,181,532,185, 0,0, 2,2 },  //   AV_AA_CLOSED_STAR
    { NULL, 545,181,549,185, 0,0, 2,2 },   //   AV_AA_CLOSED_DIAMOND
    { NULL, 521,366,634,473, 0,0, 0,0 },   //   AV_AG_BACKGROUND
    { NULL, 447,210,587,350, 0,0, 70,70}   // ADI BACKGROUND
	};

GrBuff *AARadarBackground;
GrBuff *EasyAARadarBackground;

//****************************************************************************************
// ssound.cpp     -  Globals
//****************************************************************************************

int iSoundsPaused=0;



DWORD	g_dwInternalEngineRumble = 0;
DWORD	g_dwExternalEngineRumble = 0;
DWORD	g_dwEngineGeneratorWine = 0;
DWORD	g_dwExternalEngineGeneratorWine=0;
DWORD	g_dwBurnerRumble = 0;
DWORD	g_dwExternalBurnerRumble = 0;
DWORD	g_dwWndNoise=0;
DWORD	g_dwAOASndHandle=0;
DWORD	g_dwAIMSndHandle=0;
DWORD	g_dwGunSndHandle=0;
DWORD	g_dwMissileLaunchSound=0;
DWORD	g_dwMissileWarnSound=0;


DWORD	g_dwAIMLockSndHandle=0;


int iWndMode=0;
long g_iPlayerSoundMode=1;	// 1=Internal,0=External

int g_iEngineSoundThrust=0;
int g_iOurPlanePan=0;
int g_iOurPlaneVolume=0;
int g_iProximityHit=0;
int g_iFlameOutTimer=0;

DWORD dwFlameOut1=0;
DWORD dwFlameOut2=0;

BOOL g_bBingoFuel=FALSE;


int	g_iSoundLevelCaution;
int	g_iSoundLevelEngineRum;
int	g_iSoundLevelEngineGen;
int	g_iSoundLevelExternalSFX;
int g_iBaseSpeechSoundLevel;
int g_iBaseBettySoundLevel;

int g_iPrimeSounds=0;

DWORD g_dwExplosionSnd1=0;
DWORD g_dwExplosionSnd2=0;
DWORD g_dwExplosionSnd3=0;


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

MpdType MPD;
MpdViewBoxType MpdViewBoxes[MAX_MPDS];

MpdModeInfoType Mpds[7];

ButtonClickInfoType ButtonClickInfo;
AATargetSelectInfoType AATargetSelectInfo;
AATargetSelectInfoType AGButtonClickInfo;
AATargetSelectInfoType FlirButtonClickInfo;

int PrevHUDMode = -1; // JLM this may need to be noved to a data structure

GrBuff *Buff; // JLM debug remove
GrBuff *MonoBuff; // JLM debug remove

int MonoPal[256]; // JLM debug

int Timer;   // JLM debug


// var used for clearing the mpds. Defines top, middle and bottom boxes
MpdClearType MpdClear[9] = {

 { 439,323,116,9,  428,332,138,120,  438,451,121,8}, // front right
 { 77,324,122,5,   70,329,138,123,  77,452,122,7},	 // front left
 { 439,323-201,116,9,  428,332-201,138,120,  438,451-201,121,8}, // front down right
 { 77,324-201,122,5,   70,329-201,138,123,  77,452-201,122,7},	// front down left
 { 514,331,116,9,  502,339,138,121, 511,453,121,12}, // back right
 { 347,323,116,9, 335,331,138,121, 344,445,121,12}, // back right center
 { 179,323,116,9, 167,331,138,121, 176,445,121,12}, // back left center
 { 13,331,116,9,  1,339,138,121,  10,453,121,12}, // back left
 { 260,301,116,8,  248,309,138,114, 257,423,116,13},		   // front down center

};

int BYOff = -201;

int CornerX = 54, CornerY = 108;

ButtonOffsetType FrontButtonOffsets[MAX_MPD_BUTTONS] = 	{

   { (54 - CornerX),  (138 - CornerY) },
   { (54 - CornerX),  (160 - CornerY) },
   { (54 - CornerX),  (182 - CornerY) },
   { (54 - CornerX),  (204 - CornerY) },
   { (54 - CornerX),  (226 - CornerY) },

   { (90 - CornerX),  (261 - CornerY) },
   { (112 - CornerX), (261 - CornerY) },
   { (134 - CornerX), (261 - CornerY) },
   { (156 - CornerX), (261 - CornerY) },
   { (178 - CornerX), (261 - CornerY) },

   { (211 - CornerX), (226 - CornerY) },
   { (211 - CornerX), (204 - CornerY) },
   { (211 - CornerX), (182 - CornerY) },
   { (211 - CornerX), (160 - CornerY) },
   { (211 - CornerX), (138 - CornerY) },

   { (178 - CornerX), (108 - CornerY) },
   { (156 - CornerX), (108 - CornerY) },
   { (134 - CornerX), (108 - CornerY) },
   { (112 - CornerX), (108 - CornerY) },
   { (90 - CornerX),  (108 - CornerY) },

  };


  ButtonOffsetType BackButtonOffsets[MAX_MPD_BUTTONS] = 	{

   { (54 - CornerX),  (138 - CornerY) },
   { (54 - CornerX),  (160 - CornerY) },
   { (54 - CornerX),  (182 - CornerY) },
   { (54 - CornerX),  (204 - CornerY) },
   { (54 - CornerX),  (226 - CornerY) },

   { (90 - CornerX),  (263 - CornerY) },
   { (112 - CornerX), (263 - CornerY) },
   { (134 - CornerX), (263 - CornerY) },
   { (156 - CornerX), (263 - CornerY) },
   { (178 - CornerX), (263 - CornerY) },

   { (207 - CornerX), (226 - CornerY) },
   { (207 - CornerX), (204 - CornerY) },
   { (207 - CornerX), (182 - CornerY) },
   { (207 - CornerX), (160 - CornerY) },
   { (207 - CornerX), (138 - CornerY) },

   { (178 - CornerX), (108 - CornerY) },
   { (156 - CornerX), (108 - CornerY) },
   { (134 - CornerX), (108 - CornerY) },
   { (112 - CornerX), (108 - CornerY) },
   { (90 - CornerX),  (108 - CornerY) },

  };



ButtonOffsetType MpdButtonScreenPos[MAX_MPDS][MAX_MPD_BUTTONS] = 	{


 // FRONT RIGHT

{ {412,339},	{412,361},	{412,383}, {412,405}, {412,427},  // left

 {447,462},	{469,462},	{491,462}, {513,462},  {535,462},  // bottom

 {570,427},	{570,405},	{570,383}, {570,361},  {570,339},  // right

 {535,309},	 {513,309},	{491, 309}, {469 ,309},  {447,309} },  // top


 // FRONT LEFT

{ {54,339}, {54,361}, {54,383}, {54,405}, {54,427},  // left

 {90,462}, {112,462}, {134,462}, {156,462}, {178,462},  // bottom

 {211,427},	{211,405}, {211,383}, {211,361},  {211,339},  // right

 {178,309},	 {156,309},	{134,309}, {112,309},  {90,309} },  // top


 // FRONT DOWN RIGHT
{ {412,339 + BYOff},	{412,361 + BYOff},	{412,383 + BYOff}, {412,405 + BYOff}, {412,427 + BYOff},  // left

 {447,462 + BYOff},	{469,462 + BYOff},	{491,462 + BYOff}, {513,462 + BYOff},  {535,462 + BYOff},  // bottom

 {570,427 + BYOff},	{570,405 + BYOff},	{570,383 + BYOff}, {570,361 + BYOff},  {570,339 + BYOff},  // right

 {535,309 + BYOff},	 {513,309 + BYOff},	{491, 309 + BYOff}, {469 ,309 + BYOff},  {447,309 + BYOff} },  // top


 // FRONT DOWN LEFT
 { {54,339 + BYOff}, {54,361 + BYOff}, {54,383 + BYOff}, {54,405 + BYOff}, {54,427 + BYOff},  // left

 {90,462 + BYOff}, {112,462 + BYOff}, {134,462 + BYOff}, {156,462 + BYOff}, {178,462 + BYOff},  // bottom

 {211,427 + BYOff},	{211,405 + BYOff}, {211,383 + BYOff}, {211,361 + BYOff},  {211,339 + BYOff},  // right

 {178,309 + BYOff},	 {156,309 + BYOff},	{134,309 + BYOff}, {112,309 + BYOff},  {90,309 + BYOff} },  // top




// BACK RIGHT
{ {488,346},	{488,368},	{488, 390},	{488,412}, {488,434},  // left

 {526,471},	{548,471},	{570, 471}, {592,471},  {614,471},  // bottom

 {635,434}, {635,412}, {635,390}, {635,368}, {635,346},   // right

 {614,316},	 {592,316},	{570, 316}, {548,316},  {526,316} }, // top


 //BACK CENTER RIGHT
{ {321,338},	{321,360},	{321,382},	{321,404}, {321,426},  // left

 {354,463},	{376,463},	{398, 463}, {420,463},  {442,463},  // bottom

 {474,426},	{474,404},	{474,382 }, {474,360},  {474,338},  // right

 {442,308},	 {420,308},	{398,308}, {376,308},  {354,308} },  // top



 // BACK CENTER LEFT
{ {153,338},	{153,360},	{153, 382},	{153,404}, {153,426},  // left

 {186,463},	{208,463},	{230, 463}, {252,463},  {274,463},  // bottom

 {306,426},	{306,404},	{306, 382}, {306,360},  {306,338},  // right

 {274,308},	 {252,308},	{230, 308}, {208,308},  {186,308} },  // top




// BACK LEFT
{ {-5,346},	{-5,368},	{-5,390},	{-5,412}, {-5,434},  // left

 {16,471},	{38,471},	{60, 471}, {82,471},  {104,471},  // bottom

 {140,434},	{140,412},	{140,390}, {140,368},  {140,346},  // right

 {104,316},	 {82,316},	{60,316}, {38,316},  {16,316} },  // top



 // DOWN CENTER
 { {232,316},	{232,338},	{232,360},	{232,382}, {232,404},  // left

 {267,441},	{289,441},	{311, 441}, {333,441},  {355,441},  // bottom

 {390,404},	{390,382},	{390, 360}, {390,338},  {390,316},  // right

 {355,285},	 {333,285},	{311, 285}, {289,285},  {267,285} },  // top



};

//*********************************************************************************************
//************ LOOKDOWN SPRITES

GrBuff FireExtSwitchBuffs[NUM_FIRE_EXT_SWITCH_SPRITES];
GrBuff JetWeapSelectSwitchBuffs[NUM_JET_WEAP_SELECT_SWITCH_SPRITES];
GrBuff JetPushBuffs[NUM_JETTISON_PUSH_SPRITES];
GrBuff FuelSwitchBuffs[NUM_FUEL_SWITCH_SPRITES];
GrBuff LeftDownSwitchBuffs[NUM_LEFT_DOWN_SWITCH_SPRITES];
GrBuff RightDownSwitchBuffs[NUM_RIGHT_DOWN_SWITCH_SPRITES];
GrBuff DoubleFireBuffs[NUM_DOUBLE_FIRE_SPRITES];
GrBuff SingleFireBuffs[NUM_SINGLE_FIRE_SPRITES];
GrBuff MasterArmSwitchBuffs[NUM_MASTER_ARM_SPRITES];
GrBuff BingoArrowsBuffs[2];

LookDownSpriteType FireExtSwitch[NUM_FIRE_EXT_SWITCH_SPRITES] =
{
  &FireExtSwitchBuffs[0],  2, 2,26,29,    0,0, 10,160,0,
  &FireExtSwitchBuffs[1],  30,2,54,29,    0,0, 10,160,0,
  &FireExtSwitchBuffs[2],  58,2,82,29,    0,0, 10,160,0
};

LookDownSpriteType JetWeapSelectSwitch[NUM_JET_WEAP_SELECT_SWITCH_SPRITES] =
{
  &JetWeapSelectSwitchBuffs[0],  33,149,60,177,     0,0, 0,327,0,
  &JetWeapSelectSwitchBuffs[1],  95,149,122,177,    0,0, 0,327,0,
  &JetWeapSelectSwitchBuffs[2],  64,149,91,177,     0,0, 0,327,0,
  &JetWeapSelectSwitchBuffs[3],  2,149,29,177,      0,0, 0,327,0,
  &JetWeapSelectSwitchBuffs[4],  126,149,153,177,   0,0, 0,327,0
};

LookDownSpriteType JetPush[NUM_JETTISON_PUSH_SPRITES] =
{
  &JetPushBuffs[0],  44,181,82,221,  0,0, 191,371,0,
  &JetPushBuffs[1],  2,181,40,221,   0,0, 191,371,0
};

LookDownSpriteType FuelSwitch[NUM_FUEL_SWITCH_SPRITES] =
{
  &FuelSwitchBuffs[0],  41,250,76,282,  0,0, 574,401,0,
  &FuelSwitchBuffs[1],  2, 250,37,282,  0,0, 574,401,0,
};

LookDownSpriteType LeftDownSwitch[NUM_LEFT_DOWN_SWITCH_SPRITES] =
{
  &LeftDownSwitchBuffs[0],  17,225,28,246,  0,0, 289,244,0,
  &LeftDownSwitchBuffs[1],  2, 225,13,246,  0,0, 289,244,0,
};

LookDownSpriteType RightDownSwitch[NUM_LEFT_DOWN_SWITCH_SPRITES] =
{
  &RightDownSwitchBuffs[0],  32,225,44,246,  0,0, 330,244,0,
  &RightDownSwitchBuffs[1],  48,225,60,246,  0,0, 330,244,0,
};

LookDownSpriteType DoubleFire[NUM_DOUBLE_FIRE_SPRITES] =
{
  &DoubleFireBuffs[0],   2,33,47,58,      0,0, 0,226,0,
  &DoubleFireBuffs[1],   51,33,96,58,     0,0, 0,226,0,
  &DoubleFireBuffs[2],   100,33,145,58,   0,0, 0,226,0,
  &DoubleFireBuffs[3],   149,33,194,58,   0,0, 0,226,0,
  &DoubleFireBuffs[4],   198,33,243,58,   0,0, 0,226,0,
  &DoubleFireBuffs[5],   2,62,47,87,      0,0, 0,226,0,
  &DoubleFireBuffs[6],   51,62,96,87,     0,0, 0,226,0,
  &DoubleFireBuffs[7],   100,62,145,87,   0,0, 0,226,0,
  &DoubleFireBuffs[8],   149,62,194,87,   0,0, 0,226,0,
  &DoubleFireBuffs[9],   198,62,243,87,   0,0, 0,226,0,
  &DoubleFireBuffs[10],  2,91,47,116,     0,0, 0,226,0,
  &DoubleFireBuffs[11],  51,91,96,116,    0,0, 0,226,0,
  &DoubleFireBuffs[12],  100,91,145,116,  0,0, 0,226,0,
  &DoubleFireBuffs[13],  2,120,47,145,    0,0, 0,226,0,
  &DoubleFireBuffs[14],  51,120,96,145,   0,0, 0,226,0,
  &DoubleFireBuffs[15],  100,120,145,145, 0,0, 0,226,0,
};

LookDownSpriteType SingleFire[NUM_SINGLE_FIRE_SPRITES] =
{
  &SingleFireBuffs[0],  149,91,194,116,  0,0, 0,195,0,
  &SingleFireBuffs[1],  198,91,243,116,  0,0, 0,195,0,
  &SingleFireBuffs[2],  198,120,243,145, 0,0, 0,195,0,
  &SingleFireBuffs[3],  149,120,194,145, 0,0, 0,195,0,
};

LookDownSpriteType MasterArmSwitch[NUM_MASTER_ARM_SPRITES] =
{
  &MasterArmSwitchBuffs[0], 2, 300,48, 344, 0,0, 1,374,0,
  &MasterArmSwitchBuffs[1], 54,300,100,344, 0,0, 1,374,0,
};

LookDownSpriteType BingoArrows[2] =
{
  &BingoArrowsBuffs[0], 2, 286,10,295,  0,0, 620,335,0,
  &BingoArrowsBuffs[1], 14,286,22,295,  0,0, 620,390,0,
};


//*****************************************************************************************************************************************
// HELP NOTEBOOK SPRITES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

GrBuff AvHelpWindowBuffs[AV_NUM_HELP_WINDOW_SPRITES];

AvGenericSpriteType AvHelpWindowSprites[AV_NUM_HELP_WINDOW_SPRITES] =
{
  &AvHelpWindowBuffs[0],  0,0,15,26,     0,0,
  &AvHelpWindowBuffs[1],  0,263,8,448,   0,0,
  &AvHelpWindowBuffs[2],  0,449,131,452, 0,0,
  &AvHelpWindowBuffs[3],  0,27,4,261,    0,0,
};

//*****************************************************************************************************************************************
// ANIM SPRITES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

GrBuff AnimSpritesBuffs[NUM_ANIM_SPRITES];

// X1,Y1, X2,Y2, Width,Height, AnchX,AnchY, NumFrames, Repeat, TotalTime, TimePerFrame
AnimSpriteType AnimSprites[NUM_ANIM_SPRITES] =
{
  &AnimSpritesBuffs[0], 0,0,   63,63,  0,0, 32,32, 4, TRUE, 1000, 0,
  &AnimSpritesBuffs[1], 0,320, 12,332, 0,0, 0,0,   4, TRUE, 1000, 0,  // checkbox
  &AnimSpritesBuffs[2], 0,335, 43,355, 0,0, 0,0,   3, TRUE, 1000, 0,  // left button
  &AnimSpritesBuffs[3], 0,358, 43,378, 0,0, 0,0,   3, TRUE, 1000, 0,  // right button
  &AnimSpritesBuffs[4], 0,381, 66,401, 0,0, 0,0,   3, TRUE, 1000, 0,  // cancel button
};

//*********************************************************************************************
//************  AA RADAR VARS

int AAScanMode;   // for sidewinders, set in AA PACS states are ON and OFF
int SniffOn;      // actually used by both AA and AG radars

//*********************************************************************************************
//************  AG RADAR VARS

GrBuff *AGScanBuffer;
GrBuff *GMTAGScanBuffer;
GrBuff *AGMapBuffer;
GrBuff *AGMonoMapBuffer;
GrBuff *AGMapBackBuffer;
int ScanBeamOn;
int CreatingHiResMap;

//*********************************************************************************************
//************  TEWS VARS

int ChaffTotal;
int FlareTotal;

//*********************************************************************************************
//************  MISC STRING VARS

char Str1[20];
char Str2[20];
char Str3[20];

//*********************************************************************************************
//************  MPD COLOR VARS

int MpdColors[2][MAX_MPD_COLOR_TYPES] = {53,253,53,53,253,56,53,53,53,53,61,249,61,61,249,56,61,61,61,61};

//***********************************************************************************************************************************
// ***** Mpd DamageVars

int MpdDamage[MAX_MPD_DAMAGE];

//***********************************************************************************************************************************
// ***** FLIR DamageVars

GrBuff *FlirMonoBuff;
GrBuff *GbuMonoBuff;
GrBuff *MavMonoBuff;

//***********************************************************************************************************************************
// ***** DayNight Cockpit Stuff

int UseDayCockpit;

//***********************************************************************************************************************************
// ***** Fuel Vars

float BingoPercent;

//***********************************************************************************************************************************
// ***** Time To Fire -- Used by Temperature readings

long lMaxTimeToFire;

//***********************************************************************************************************************************
// ***** MPD Programming Vars

int MpdProgrammingMatrix[MAX_UNIQUE_MPDS][MAX_NUM_MODES] =
{
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
};

int DefaultMpdProgrammingMatrix[MAX_UNIQUE_MPDS][MAX_NUM_MODES] =
{
  STORES_MODE,     STORES_MODE,   STORES_MODE,   HSI_MODE,
  RADAR_MODE,      RADAR_MODE,    RADAR_MODE,    HSI_MODE,
  HSI_MODE,        HSI_MODE,	  HSI_MODE,      RADAR_MODE,
  UFC_MODE,        UFC_MODE,      UFC_MODE,      UFC_MODE,
  EDF_MODE,        EDF_MODE,      EDF_MODE,      EDF_MODE,
  NONE_MODE,       NONE_MODE,     NONE_MODE,     NONE_MODE,
  NONE_MODE,       NONE_MODE,     NONE_MODE,     NONE_MODE
};

//*****************************************************************************************************************************************
// WEAP ID GLOBAL TABLE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int F18WeapIds[] = {34,35,108,36,37,109,38,110,111,4,112,
                    114,87,116,117,118,119,24,120,30,29,122,
                    123,124,32,8,126,127,128,33,9,10,131,15,132,
                    133,176,135,136,137,138,139,140,142,143,1,2,7,44,-1};