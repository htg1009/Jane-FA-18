#ifndef __AVLOCAL_H__
#define __AVLOCAL_H__


#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "Resources.h"

#include "engine.h"
#include "flightfx.h"


extern int NumMisslesChasing;
extern ChasingMisslesType MisslesInFlight[];

//*****************************************************************************************************************************************
// AV DEFINES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define FLIR_TGT_MODE  			0
#define FLIR_GS_MODE   			1
#define FLIR_VV_MODE   			2
#define FLIR_LOCK_MODE 			3
#define FLIR_BST_MODE  			4
#define FLIR_LS_MODE        5
#define FLIR_AA_LOCK_MODE   6
#define FLIR_LOS_POINT_MODE 7

#define MAX_TEWS_PROGS 10

#define AA_7F  34
#define AA_7M  35
#define AA_9L  36
#define AA_9M  37
#define AA_120 38
#define AA_7MH 108
#define AA_9X  109

//*****************************************************************************************************************************************
// EXTERNS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern AvionicsType Av;

extern int TRelSet;

extern int NumDTWSTargets;
extern int DTWSTargets[];
extern int LimitCount;

extern class Timer InterleaveTimer;

#define SCAN_LINE_LENGTH 131 //111
#define NUM_SCAN_LINES	 70	// update this number from UpdateRbm call

typedef struct
{
  signed int  X;
  BYTE  Valid;
  float ColorPercent;  // 0 - 0.75 so we can have contrasting green overlay symbols
} PixelListType;

typedef struct _ScanEdge
{
	float t;
	int MaxY;
	PixelListType Edge[SCAN_LINE_LENGTH];
}ScanEdge;

extern ScanEdge ScanBeamEdges[NUM_SCAN_LINES];
extern int ScanBeamEdge;

extern int ScanBeamOn;

extern AvRGB *HUDColorRGB;
extern AvRGB HUDColorList[11];


typedef struct
{
	int MpdClick;
	int MpdNum;
	int X;
	int Y;
} MpdClickType;

extern MpdClickType MpdClick;

extern WeapStoresType WeapStores;

//*****************************************************************************************************************************************
// PRIMARY AND SECONDARY TARGETS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern TargetInfoType Targets[];
extern TargetInfoType *Primary;
extern TargetInfoType *Secondary;

//*****************************************************************************************************************************************
// TARGET LISTS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
#define AA_MAX_AGE_PLANES 10

extern DetectedPlaneListType Temp2CurFramePlanes;
extern DetectedPlaneListType TempCurFramePlanes;
extern DetectedPlaneListType CurFramePlanes;
extern DetectedPlaneListType PrevFramePlanes;
extern DetectedPlaneListType BlinkPlanes;
extern DetectedPlaneListType AgePlanes[AA_MAX_AGE_PLANES];
extern DetectedPlaneListType Age1Planes;  // JLM langtext has this externed. have Scott switch to AgePlanes[0]

//extern DetectedGroundObjListType LToRGroundObjs;
//extern DetectedGroundObjListType RToLGroundObjs;

//extern DetectedVehicleListType LToRVehicles;
//extern DetectedVehicleListType RToLVehicles;

extern DetectedGroundObjListType AGGroundObjs;
extern DetectedVehicleListType   AGVehicles;

//*****************************************************************************************************************************************
// AA DEFINES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
#define TDC_INDICATOR 			(130),(5)

#define AA_RADAR_EXTREME_TOP 16.0
#define AA_RADAR_LEFT        15.0
#define AA_RADAR_RIGHT       125.0
#define AA_RADAR_TOP         25.0
#define AA_RADAR_BOTTOM      124.0
#define AA_RADAR_CENTER_X    70.0
#define AA_RADAR_CENTER_Y    75.0
#define AA_RADAR_LENGTH      99.0
#define AA_RADAR_WIDTH       110.0
#define AA_RADAR_HALF_WIDTH  55.0

#define TOP_NOTCH_SPACE      ( (AA_RADAR_CENTER_X - AA_RADAR_LEFT)/2.33333 )
#define TOP_NOTCH_0X         ( AA_RADAR_CENTER_X - 2.0*(TOP_NOTCH_SPACE) )
#define TOP_NOTCH_1X         ( AA_RADAR_CENTER_X - TOP_NOTCH_SPACE )
#define TOP_NOTCH_2X         ( AA_RADAR_CENTER_X )
#define TOP_NOTCH_3X         ( AA_RADAR_CENTER_X + TOP_NOTCH_SPACE )
#define TOP_NOTCH_4X         ( AA_RADAR_CENTER_X + 2.0*(TOP_NOTCH_SPACE) )
#define TOP_NOTCH_LENGTH     6

#define SIDE_NOTCH_SPACE      ( AA_RADAR_LENGTH/4.0 )
#define SIDE_NOTCH_0Y         ( AA_RADAR_BOTTOM - (SIDE_NOTCH_SPACE) )
#define SIDE_NOTCH_1Y         ( AA_RADAR_BOTTOM - 2.0*(SIDE_NOTCH_SPACE) )
#define SIDE_NOTCH_2Y         ( AA_RADAR_BOTTOM - 3.0*(SIDE_NOTCH_SPACE) )
#define SIDE_NOTCH_WIDTH     9

#define SMALL_SIDE_NOTCH_SPACE  ( (SIDE_NOTCH_0Y - SIDE_NOTCH_1Y)/3.0 )
#define SMALL_SIDE_NOTCH_WIDTH  3

//*****************************************************************************************************************************************
// GLOBALS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern int ScanStart;
extern CaptBarsType CaptBars;
extern CaptBarsType ScanAreaControl;

extern VKCODE CaptBarsVk;

extern VKCODE TdcLeftVk;
extern VKCODE TdcRightVk;
extern VKCODE TdcUpVk;
extern VKCODE TdcDownVk;

extern int NumIffPlanes;
extern int IffPlanes[];

extern AwacsTargetListType AvAwacs;

extern AARankingListType Ranks;

extern int AARadarBaseSense;
extern int PrevAARadarBaseSense;
extern int MinBase;
extern int BaseCount;

extern int AARadarShowCrossSig;

extern AvRGB AvPalette[AV_PALETTE_MAX_COLORS][AV_PALETTE_NUM_SHADES];

extern MpdButtonPressType MpdButton;

extern int BarList[];
extern int *AABars;

extern class Timer AA1SecBlink;
extern int AA1SecBlinkState;

extern int NumTewsDraw;

extern int TDCMpdNum;   // set to the current mpd that has control for capt bars and other things

extern FPointDouble ScanAreas[3][4];
extern FPointDouble ScanAreasWorld[3][4];
extern FPointDouble ScanAreasPixel[3][4];
extern FPointDouble ScanAreasZoomPixel[3][4];

extern FPointDouble ScanAreasZoomWorld[3][4];

extern FPointDouble ScanBoundPixel[4];

extern FPointDouble ScanBoundZoomWorld[3][4];
extern FPointDouble ScanBoundZoomPixel[3][4];

extern FPointDouble ScreenZoomWorld[4];

extern float  ScanBoundClipLeft;
extern float  ScanBoundClipRight;
extern float  ScanBoundClipTop;
extern float  ScanBoundClipBottom;

#define AV_MAX_GPS_LABELS 100
extern int AvNumGpsLabels;
extern MissionLabelType *AvGpsLabels[AV_MAX_GPS_LABELS];

// jlm debug
extern int DebugTewsTest;

//*****************************************************************************************************************************************
// PROTOTYPES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

float GetAngularCoverage(int Range, int Bars);
int PlaneVisibleByCriticalAspectRegion(PlaneParams *P, PlaneParams *Target);

int  IsPlaneInList(DetectedPlaneListType *PList,int Id);
int  GetPlanePosInList(DetectedPlaneListType *PList,int Id);
int  GetPlaneRankPosInList(DetectedPlaneListType *PList,int Rank);
void CopyPlaneEntry(DetectedPlaneType *From, DetectedPlaneType *To);
void AddPlaneToList(DetectedPlaneListType *PList, DetectedPlaneType *TempPlaneInfo);
void RemovePlaneFromList(DetectedPlaneListType *PList, int Id);
void CopyPlaneList(DetectedPlaneListType *From, DetectedPlaneListType *To);
void DrawRadarBlip(float X, float Y, int Ramp, float Shade);
void DrawCaptBars(float X, float Y, int Ramp, float Shade);
void ResetPrimary();
void ResetSecondary();

void GetAltHeightsAtRange(float Range, float *AltHigh, float *AltLow);

void DrawWordAtMpdButton(int MpdDetectNum,int ButtonNum, char *Word, int DrawBox,int Color, float Shade);

void SetAGTerrainAvoidance();

int  IsObjInList(DetectedGroundObjListType *List,int Id);
int  GetGroundObjPosInList(DetectedGroundObjListType *List,int Id);
void CopyGroundObjEntry(DetectedGroundObjListType *From, DetectedGroundObjListType *To);
void AddGroundObjToList(DetectedGroundObjListType *List,DetectedGroundObjListType *TempInfo);
void RemoveGroundObjFromList(DetectedGroundObjListType *List, int Id);

int  IsVehicleInList(DetectedVehicleType *List,int Id);
int  GetVehiclePosInList(DetectedVehicleType *List,int Id);
void CopyVehicleEntry(DetectedVehicleType *From, DetectedVehicleType *To);
void AddVehicleToList(DetectedVehicleType *List,DetectedVehicleType *TempInfo);
void RemoveVehicleFromList(DetectedVehicleType *List, int Id);

void DrawAGObjects();
void SetAGObjects();
void FilterAGObjects();
void DrawAGObjects();

void SetAGVehicles();
void DrawAGVehicles();

void DrawHsiCircle(float X,float Y,int ColorIndex,float Shade);

void AGScanPixelToWorld(FPointDouble Source,FPointDouble *Loc);
void AGScanWorldToPixel(FPointDouble Source,FPointDouble *Loc);
void AGZoomInWorldToPixel(FPointDouble Source,FPoint *Loc);

void SetMapModeWorldToPixel(int Mode);
void SetMapModePixelToWorld(int Mode);
void SetBoundPixelToWorld();
void SetMapModeWorldToZoomPixel(int Mode);
void AGSetZoomBoundArea(int Mode);
void SetZoomBoundToWorld(int Mode);

void MoveScanAreaInDirection(int Mode,int Dir, float TimeElapsed);

void ResetAGRadar(int Mode);

void SetSecondPass( bool on, FMatrix mat );
SubObjectHandler *GetSubObjects(PlaneParams *P);

void InitCM();

void AGM65WeapPage(int MpdNum);
void AGM65EWeapPage(int MpdNum);

void WalleyeWeapPage(int MpdNum);
void HarmWeapPage(int MpdNum);

void ResetMaveric();
void ResetGbu();
void InitFlir();
void InitHarm();

void DrawDiamond(float X,float Y,float Length,float Width,int Color,float Shade);

void DrawTDCIndicator(int MpdNum);

void GrDrawRampedPolyBuff( GrBuff *dest, int numpts, float *pXs, float *pYs, FPointData *pColor, float alpha);

void GetCamPos(WeapCamType *Cam);

void CalcCameraViewVector(float LeftRight, float UpDown, FPointDouble *Origin, FPointDouble *ViewVec,float Heading);

void ToggleIDECM();

void DrawTriangleRot(float X,float Y,int Size,float Angle,int Color,float Shade,int Fill);

int IsCrossSectionValid(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, int PlaneIndex);

int GetMpdClick(int MpdNum,int *X,int *Y);

void InitTews();
void InitUfc();
void InitHsi();
void InitSA();
void InitBit();
void InitAcl();

void DrawHarmMode();

int CursorInMpd(int MpdNum,float *X,float *Y);

void GrDrawColorPoly( GrBuff *dest, int numpts, float *pXs, float *pYs, FPointData *Color,  float alpha);
void GrDrawPolyCircleArcPolyBuff( GrBuff *Buff, int OriginX, int OriginY, int inradius, int outradius, float startangle, float endangle, int Segments, float ColorRed, float Green, float Blue, int PAL8);

//*****************************************************************************************************************************************
// INIT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define XMinPos  (110.0)
#define XMaxPos  (450.0)
#define ZPos     (6.0*1024.0)
#define DownSideLine (40)


extern RadarLadderType ADIAirToAirLadder;

extern class Timer AcqTimer;

extern float AA_Rmax;
extern float AA_Rmin;
extern float AA_ROpt;
extern float AA_Rtr;
extern int   AA_shootQ;
extern int   AA_breakX;
extern int   AA_flash;
extern int	 AA_Tpre;
extern float AA_range;
extern int   AA_PosX;		//   azimuth (pixels)
extern int   AA_PosY;		//   altitude (pixels)
extern int   AA_DrawASE;
extern float AA_ASESize;

//*****************************************************************************************************************************************
// WEAPON STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define WS_LEFT_OUTLINE_P1				(18),(65)
#define WS_LEFT_OUTLINE_P2				(60),(34)
#define WS_LEFT_OUTLINE_P3				(60),(24)
#define WS_LEFT_OUTLINE_P4				(54),(24)

#define WS_RIGHT_OUTLINE_P1				(124),(65)
#define WS_RIGHT_OUTLINE_P2				(82),(34)
#define WS_RIGHT_OUTLINE_P3				(82),(24)
#define WS_RIGHT_OUTLINE_P4				(88),(24)

#define WS_LEFT_STATION1(x,y)			(9+x),(71+y)
#define WS_LEFT_STATION2(x,y)			(25+x),(58+y)
#define WS_LEFT_STATION3(x,y)			(41+x),(47+y)
#define WS_LEFT_STATION4(x,y)			(58+x),(35+y)
#define WS_LEFT_STATION5(x,y)			(37+x),(22+y)

#define WS_CENTER_STATION6(x,y)		(71+x),(21+y)

#define WS_RIGHT_STATION7(x,y)		(91+x),(22+y)
#define WS_RIGHT_STATION8(x,y)		(84+x),(35+y)
#define WS_RIGHT_STATION9(x,y)		(101+x),(47+y)
#define WS_RIGHT_STATION10(x,y)	  (117+x),(58+y)
#define WS_RIGHT_STATION11(x,y)		(133+x),(71+y)


#define WS_TABLE(x,y)			        (21+x),(94+y)

#define WS_STATION_DIAMOND_Y_OFF  6

#define WS_STATION_NUMWEAP_X_OFF  -3
#define WS_STATION_NUMWEAP_Y_OFF  10

#define WS_STATION_WEAPTYPE_X_OFF  -7
#define WS_STATION_WEAPTYPE_Y_OFF  17

#define WS_TABLE_ROW1_COL1(x,y)		(14+x),(102+y)
#define WS_TABLE_ROW1_COL2(x,y)		(40+x),(102+y)
#define WS_TABLE_ROW1_COL3(x,y)		(79+x),(102+y)
#define WS_TABLE_ROW1_COL4(x,y)		(105+x),(102+y)

#define WS_TABLE_ROW2_COL1(x,y)		(14+x),(108+y)
#define WS_TABLE_ROW2_COL2(x,y)		(40+x),(108+y)
#define WS_TABLE_ROW2_COL3(x,y)		(79+x),(108+y)
#define WS_TABLE_ROW2_COL4(x,y)		(105+x),(108+y)

#define WS_TABLE_ROW3_COL1(x,y)		(14+x),(114+y)
#define WS_TABLE_ROW3_COL2(x,y)		(40+x),(114+y)
#define WS_TABLE_ROW3_COL3(x,y)		(79+x),(114+y)
#define WS_TABLE_ROW3_COL4(x,y)		(105+x),(114+y)

#define WS_TABLE_LINE_P1(x,y)		(8+x),(100+y)
#define WS_TABLE_LINE_P2(x,y)		(129+x),(100+y)

#define WS_TABLE_CENTER(x,y)		(64+x),(93+y)

#define WS_MISSILE_SYMBOL 0
#define WS_DIAMOND_SYMBOL 1

#define MAX_BOMB_PROGS 5


typedef struct
{
	float X,Y;
} WSStationPointType;
//*****************************************************************************************************************************************
// WEAP STORES GLOBAL  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


extern WSStationPointType StationPoints[];

// based on initial loadout, doesn't change after init.
extern int StationPointSymbols[ MAX_F18E_STATIONS];

extern BombProgType BombProgs[MAX_BOMB_PROGS];
extern BombProgType SavedBombProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

extern BombProgType ClusterBombProgs[MAX_BOMB_PROGS];
extern BombProgType SavedClusterBombProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

extern HarpoonProgType HarpoonProgs[MAX_BOMB_PROGS];
extern HarpoonProgType SavedHarpoonProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

extern SlamerProgType SlamerProgs[MAX_BOMB_PROGS];
extern SlamerProgType SavedSlamerProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

extern JsowProgType JsowProgs[MAX_BOMB_PROGS];
extern JsowProgType SavedJsowProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

extern JsowProgType JdamProgs[MAX_BOMB_PROGS];
extern JsowProgType SavedJdamProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

extern HarmProgType HarmProgs[MAX_BOMB_PROGS];
extern HarmProgType SavedHarmProgs[MAX_BOMB_PROGS];

extern TewsProgType TewsProgs[MAX_TEWS_PROGS];
extern TewsProgType SavedTewsProgs[MAX_TEWS_PROGS];

//*****************************************************************************************************************************************
// HARM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define HARM_LAND     0
#define HARM_SEA      1
#define HARM_LAND_SEA 2

#define TEWS_CENTER_X  71
#define TEWS_CENTER_Y  71

#define TEWS_MISSLE_THREAT   0
#define TEWS_PLANE_THREAT    1
#define TEWS_SAM_PROV_THREAT 2
#define TEWS_SAM_WEAP_THREAT 3
#define TEWS_VEH_THREAT      4
#define TEWS_SHIP_THREAT     5

#define TEWS_MANUAL_MODE    0
#define TEWS_SEMI_AUTO_MODE 1
#define TEWS_AUTO_MODE      2

#define ICS_STBY 0
#define ICS_JAM  1
#define ICS_EMIS 2

#define TEWS_NORM       0
#define TEWS_SEA        1
#define TEWS_LAND       2
#define TEWS_AIR        3
#define TEWS_EW         4
#define TEWS_FRIENDLY   5

#define TEWS_CHAFF_SELECT    0
#define TEWS_FLARE_SELECT    1
#define TEWS_REPEAT_SELECT   2
#define TEWS_INTERVAL_SELECT 3

#define TEWS_MAX_ENTITIES 400

typedef struct
{
	int Firing;
  int Draw;
  int Type;
  int HasLock;
  PlaneParams          *P;
  WeaponParams         *W;
  InfoProviderInstance *SP;
  AAWeaponInstance     *SW;
	MovingVehicleParams  *V;
	int NumRadar;            // only for vehicles
  char *TEWSAbbrev;
	int  Prog;
	int  Category;
} ThreatEntryType;

typedef struct
{
  int NumThreats;
  ThreatEntryType List[TEWS_MAX_ENTITIES];
}TewsThreatsType;

extern TewsThreatsType Threats;
extern TewsThreatsType PrevThreats;
extern ThreatEntryType *CurThreat;

typedef struct
{
  void   *Obj;
	float  XOff;
	float  YOff;
	int    Type;
	char   *TEWSAbbrev;
	int    InHarmFOV;
	int    HasLock;
} HarmObjectType;

#define AV_MAX_HARM_OBJECTS 300

extern int NumHarmObjects;
extern HarmObjectType HarmObjects[AV_MAX_HARM_OBJECTS];


//*****************************************************************************************************************************************
// MAVERIC CAM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define SLEW_UP    0
#define SLEW_DOWN  1
#define SLEW_LEFT  2
#define SLEW_RIGHT 3

#define MSL_NAV    0
#define MSL_FLIR   1
#define MSL_HUD    2

#define MSL_MANUAL 0
#define MSL_AUTO   1

#define WFOV   0
#define NFOV   1
#define ENFOV  2

#define MSL_NONE   0
#define MSL_STBY   1
#define MSL_RDY    2
#define MSL_FIRED  3

#define PIX_PER_SLEW_LR (54.0/40.0)
#define PIX_PER_SLEW_UD (54.0/40.0)

#define GBU_MANUAL 0
#define GBU_AUTO   1

#define GBU_DIRECT     0
#define GBU_INDIRECT   1

#define GBU_NORM  0
#define GBU_TRANS 1
#define GBU_TERM  2

#define FLIR_AG 0
#define FLIR_AA 1

#define FLIR_ALAS 0
#define FLIR_MLAS 1

#define WHOT 0
#define BHOT 1

#define CAM_PROJ_DIST (40.0*NMTOWU)

#define PIX_PER_SLEW_LR (54.0/40.0)
#define PIX_PER_SLEW_UD (54.0/40.0)

#define FLIR_CENTER_X 70
#define FLIR_CENTER_Y 70

#define CAM_CENTER_X 70
#define CAM_CENTER_Y 70


typedef struct
{
  int   Slewing;
  int   SlewDirection;
  float SlewOffset;
  class Timer SlewTimer;
  int   MpdNum;
	int   LastDirection;
} WeapCamSlewType;

extern WeapCamSlewType MavWeapCamSlew;
extern WeapCamSlewType GbuWeapCamSlew;
extern WeapCamSlewType FlirSlew;

extern FPointDouble LaserLoc;
extern FPointDouble FlirPos;

//*****************************************************************************************************************************************
// UFC MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int  ULX;
	int  ULY;
	int  Width;
	int  Height;
	int  SULX;   // symbol x,y
	int  SULY;
	int  IsString;
	int  Symbol;
	char String[5];
	int Action;
} UFCButtonType;

#define CNI_TEXT 			0
#define CNI_1    			1
#define CNI_2    			2
#define CNI_3    			3
#define CNI_4    			4
#define CNI_5    			5
#define CNI_6    			6
#define CNI_7    			7
#define CNI_8    			8
#define CNI_9    			9
#define CNI_0    			10

#define CNI_CLR     	11
#define CNI_ENT     	12
#define CNI_ILS     	13
#define CNI_AP      	14
#define CNI_TCN     	15
#define CNI_IFF     	16
#define CNI_MDI     	17
#define CNI_BLANK1  	18
#define CNI_RALT    	19
#define CNI_EW        20
#define CNI_FLIR      21
#define CNI_BLANK2    22
#define CNI_RALT_UP   23
#define CNI_RALT_DOWN 24
#define CNI_TACAN_UP   25
#define CNI_TACAN_DOWN 26


#define MDI_SMS     25
#define MDI_HUD     26
#define MDI_RALT    27
#define MDI_UP      28
#define MDI_DWN     29
#define MDI_BLANK1  30
#define MDI_EW      31
#define MDI_FLR     32
#define MDI_CNI     33
#define MDI_BLANK2  34


#define UFC_NONE 200

#define ACTION_CNI_1 			 0
#define ACTION_CNI_2 			 1
#define ACTION_CNI_3 			 2
#define ACTION_CNI_4 			 3
#define ACTION_CNI_5 			 4
#define ACTION_CNI_6 			 5
#define ACTION_CNI_7 			 6
#define ACTION_CNI_8 			 7
#define ACTION_CNI_9 			 8
#define ACTION_CNI_0 			 9
#define ACTION_CNI_CLR     10
#define ACTION_CNI_ENT     11
#define ACTION_CNI_ILS     12
#define ACTION_CNI_AP      13
#define ACTION_CNI_TCN     14
#define ACTION_CNI_IFF     15
#define ACTION_CNI_MDI     16
#define ACTION_CNI_BLANK1  17
#define ACTION_CNI_RALT    18
#define ACTION_CNI_EW      19
#define ACTION_CNI_FLIR    20
#define ACTION_CNI_BLANK2  21
#define ACTION_CNI_RALT_UP 22
#define ACTION_CNI_RALT_DOWN  23

#define ACTION_MDI_SMS     24
#define ACTION_MDI_HUD     25
#define ACTION_MDI_RALT    26
#define ACTION_MDI_UP      27
#define ACTION_MDI_DWN     28
#define ACTION_MDI_BLANK1  29
#define ACTION_MDI_EW      30
#define ACTION_MDI_FLR     31
#define ACTION_MDI_CNI     32
#define ACTION_MDI_BLANK2  33

#define ACTION_AP_CPL      34
#define ACTION_AP_BALT     35
#define ACTION_AP_RALT     36
#define ACTION_AP_FPAH     37
#define ACTION_AP_CNI      38
#define ACTION_AP_BLANK1   39
#define ACTION_AP_ROLL     40
#define ACTION_AP_GTRK     41
#define ACTION_AP_HDG      42
#define ACTION_AP_BLANK2   43

#define ACTION_TACAN_UP    44
#define ACTION_TACAN_DOWN  45

extern UFCButtonType CNIButtons[];
extern UFCButtonType MDIButtons[];
extern UFCButtonType RaltButtons[];
extern UFCButtonType APButtons[];
extern UFCButtonType TacanButtons[];

void DoRaltUp(void);
void DoRaltDown(void);



//*****************************************************************************************************************************************
// AG DEFINES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AG_RADAR_TOP 		 20.0
#define AG_RADAR_LEFT        14.0
#define AG_RADAR_RIGHT       128.0
#define AG_RADAR_BOTTOM      127.0
#define AG_RADAR_CENTER_X    70.0
#define AG_RADAR_CENTER_Y    73.0

#define AG_RADAR_ORIGIN_X	 AG_RADAR_CENTER_X
#define AG_RADAR_ORIGIN_Y    AG_RADAR_BOTTOM

#define SCAN_AREA_WIDTH  54.0
#define SCAN_AREA_HEIGHT 54.0

#define AG_SCAN_PIXELS_PER_MILE(range)  (114.0/range)
#define AG_SCAN_MILES_PER_PIXEL(range)  (range/114.0)

#define AG_PIXELS_FROM_ORIGIN_TO_TOP_LEFT  sqrt( ( (AG_RADAR_ORIGIN_X-AG_RADAR_LEFT)*(AG_RADAR_ORIGIN_X-AG_RADAR_LEFT) ) + ( (AG_RADAR_TOP - AG_RADAR_ORIGIN_Y)*(AG_RADAR_TOP - AG_RADAR_ORIGIN_Y) ) )
#define AG_PIXELS_FROM_ORIGIN_TO_TOP_RIGHT sqrt( ( (AG_RADAR_ORIGIN_X-AG_RADAR_RIGHT)*(AG_RADAR_ORIGIN_X-AG_RADAR_RIGHT) ) + ( (AG_RADAR_TOP - AG_RADAR_ORIGIN_Y)*(AG_RADAR_TOP - AG_RADAR_ORIGIN_Y) ) )

#define AG_PIXELS_FROM_ORIGIN_TO_LEFT (AG_RADAR_ORIGIN_X-AG_RADAR_LEFT)
#define AG_PIXELS_FROM_ORIGIN_TO_RIGHT (AG_RADAR_RIGHT - AG_RADAR_ORIGIN_X)

#define RADAR_HEADING_X		65

void FilterVehiclesForDirection();
void FilterGroundObjectsForDirection();

#define MAX_AGRADAR_LOS_OBJS 150

typedef struct
{
	int Type;
	BasicInstance *Obj;
	MovingVehicleParams *Veh;
	int Frame;
	int MaxFrame;
	int LOS;
} AGRadarLOSType;

extern int NumAGRadarLOS;
extern AGRadarLOSType AGRadarLOSList[MAX_AGRADAR_LOS_OBJS];

extern int NumProcessedLOS; // so that game can never grind to a halt because of too many LOS



#endif