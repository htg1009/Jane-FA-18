#include "F18.h"
#include "Resources.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "spchcat.h"
#include "snddefs.h"


int MPDBroken(int MpdNum);

extern TimeExcel;
void MPDSystemMalfunction(int MpdNum);
extern void DrawWordAtMpdButton(int MpdDetectNum,int ButtonNum, char *Word, int DrawBox,int Color, float Shade);
extern void ResetToRWS();

extern GbuInIndirectMode;

extern AvionicsType Av;
extern CPitType Cpit;

// globals for player plane weight. should be moved
int nRawWeight,nInternalFuelWeight,nExternalFuelWeight;

void DrawIntAtLocation(int ScrX, int ScrY, int I, int Color, int Green=0, int Blue=0);
void DisplayNavInfo(int CenterX, int CenterY, int Color, int Green=0, int Blue=0);
void DisplayTacanInfo(int CenterX, int CenterY, int Color, int Green=0, int Blue=0);
void DrawFloatAtLocation(int ScrX, int ScrY, float I, int Color=0, int Green=0, int Blue=0);
void DrawTewsObjectEasyMode(int CenterX, int CenterY, FPoint EnemyPos, float EnemyHeading, float Range, int SymbolId, int Id, int UseId, int HasLock, int Draw, int IsTarget, PlaneParams *Plane, int Color, int Green=0, int Blue=0);
void DrawTewsObject(int MpdNum, int CenterX, int CenterY, FPoint EnemyPos, float EnemyHeading, float Range, int SymbolId, int Id, int UseId, int HasLock, int Draw, int IsTarget, PlaneParams *Plane, int Color, int Green=0, int Blue=0);
void DrawTewsObjects(int MpdNum, int CenterX, int CenterY, int Color, int Green=0, int Blue=0);
void DrawTEWSRadarSymbol(int ScrX, int ScrY, int RadarSymId, int TargetId, int UseId, int Color, int Green=0, int Blue=0);


void GrDrawSprite( TextureBuff *pTexture,int x, int y, float  r, float g, float b, int orientation, float u, float v, float u2, float v2);
void GrDrawSpritePolyBuf( GrBuff *dest, TextureBuff *pTexture, float x, float y, float r, float g, float b, int orientation);
int CeilToPow2( int num );
void DebugShowHotSpots( void);
void RotatPolyBuff( GrBuffPolygon *pBuffPoly, 	FMatrix *pMat, char scale=1 );

void InitTextSurface( void);
void ShutdownTextSurface( void);
void ClearTextOverlay(TextureBuff *pTexture);
void DrawTextOverlay( GrBuffPolygon *pBuffPoly, TextureBuff *pTexture,  char filteron= 1);




#define MPD_WIDTH	141//127//168 //256/
#define MPD_HEIGHT	141//119//164 //256

char ToggleDisplayTestMPDs=0; //Set this in a Keyfunc when keyfunc.cpp is checked in
GrBuffPolygon  MpdPolys[8], MpdTestPolys[5], MpdPopUpPolys[3];
TextureBuff *pTextOverlay, *pTextOverlay2, *pTextOverlay1;
extern float grTextureCenter;


void DisplayUFCMockup( void);


void InitVCHotspots( void);
void InitPolyMpds( void);
void InitTestPolyMpds( void);
void InitPopUpMpds( void);
void DrawHudFlir(FMatrix &mat,BOOL white_hot);
hspot *CheckMouseMPD( int mpdnum, float sx, float sy );



extern ObjectHandler *pMPDPopUp;

extern float D3DMaxZ;
extern float ooD3DMaxZ;
extern float StupidQ;

hspot *pLastHSpot;

extern void KillSound( DWORD *SndHandle );


/* ------------------------------------------2/11/98 2:47PM--------------------------------------------
 these are the coordinates for the various slot numbers
 *
 * Here's how the layout looks

	ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
	³ 0ÄÄÄÄÄÄ¿  							 4Ä3Ä5ÄÄÄÄÄÄ¿ÿÿÿ								  6ÄÄÄÄÄÄ¿ÿÿ³
	³ ³1ÄÄÄÄÄÄ¿   ÿÿ						 ³ ³ ³ 	 	³ÿ     								 7ÄÄÄÄÄÄ¿³	³
	³ ³³2ÄÄÄÄÄÄ¿                             ³ ³ ³      ³                                   8ÄÄÄÄÄÄ¿³³  ³
	³ ³³³ 	   ³                             ³ ³ ³      ³                                   ³ 	   ³³³  ³
	³ ³³³      ³                             ³ ³ ³      ³                                   ³      ³³³  ³
	³ ³³³      ³                             ³ ³ ³      ³                                   ³      ³³³  ³
	³ À´³      ³                             ÀÄÁÄÁÄÄÄÄÄÄÙ                                   ³      ³³Ù  ³
	³  À´      ³                                                                            ³      ³Ù   ³
	³   ÀÄÄÄÄÄÄÙ                                                                            ÀÄÄÄÄÄÄÙ    ³
	³                                                                                                   ³
	³                                                                                                   ³
	³                                                                                                   ³
	³                                                                                                   ³
	³                           9ÄÄÄÄÄÄ¿  		AÄÄÄÄÄÄ¿         BÄÄÄÄÄÄ¿                               ³
	³                           ³ 	   ³  		³      ³         ³ 	    ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ÀÄÄÄÄÄÄÙ  		ÀÄÄÄÄÄÄÙ         ÀÄÄÄÄÄÄÙ                               ³
	³                                                                                                   ³
	³                                                                                                   ³
	³                                                                                                   ³
	³                                                                                                   ³
	³                           CÄÄÄÄÄÄ¿  		DÄÄÄÄÄÄ¿         EÄÄÄÄÄÄ¿                               ³
	³                           ³ 	   ³  		³      ³         ³ 	    ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ³      ³  		³      ³         ³      ³                               ³
	³                           ÀÄÄÄÄÄÄÙ  		ÀÄÄÄÄÄÄÙ         ÀÄÄÄÄÄÄÙ                               ³
	³                                                                                                   ³
	³                                                                                                   ³
	³  11ÄÄÄÄÄ¿                             13Ä12Ä14ÄÄÄÄÄ¿                          		17ÄÄÄÄÄ¿    ³
	³ 10ÄÄÄÄÄ¿³                             ³  ³  ³ 	 ³                          		³16ÄÄÄÄÄ¿   ³
	³FÄÄÄÄÄÄ¿³³                             ³  ³  ³      ³                          		³³15ÄÄÄÄÄ¿  ³
	³³ 	    ³³³                             ³  ³  ³      ³                          		³³³ 	 ³  ³
	³³      ³³³                             ³  ³  ³      ³                          		³³³      ³  ³
	³³      ³³³                             ³  ³  ³      ³                          		³³³      ³  ³
	³³      ³³Ù                             ÀÄÄÁÄÄÁÄÄÄÄÄÄÙ                          		À´³      ³  ³
	³³      ³Ù                                                                               À´      ³  ³
	³ÀÄÄÄÄÄÄÙ                                                                                 ÀÄÄÄÄÄÄÙ  ³
	ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


 * ----------------------------------------------------------------------------------------------------*/



int SlotXY[] = {   0,  0,	//0
				  10, 10,   //1
				  20, 20,   //2

				 222,  0,   //3
				 212, 10,   //4
				 232, 10,   //5

				 464,  0,   //6
				 454, 10,   //7
				 444, 20,   //8

				  42, 42,	//9
				 232, 42,   //A
				 422, 42,   //B

				  42,258,	//C
				 232,258,   //D
				 422,258,   //E

				   0,301,   //F
				  10,291,   //10
				  20,281,   //11

				 222,301,   //12
				 212,291,   //13
				 232,291,   //14

				 464,301,	//15
				 454,291,	//16
				 444,281,	//17
				};



int MPDOrder[3] = {0,1,2};
int MPDSlot[3] = {0xC,0xD,0xE};
int MPDOriginalPositions[] = {412,108, 54,108, 232,286};
int OrderlyConstantToFuckedUpDefine[] = {FRONT_DOWN_RIGHT_MPD,FRONT_DOWN_LEFT_MPD,FRONT_DOWN_CENTER_MPD};
int MovingWhichMPD;
BOOL MovingMPD = FALSE;

extern g_nMissionType;

extern float Dh, Dp, Dr;
extern float ElevPos;

int FrontDownAdjustX = 0;
int FrontDownAdjustY = -200;

int ButtonWidth = 11;
int ButtonHeight = 11;

int MpdDetectToMpdMapping[9] = {MPD0,MPD1,MPD0,MPD1,MPD6,MPD5,MPD4,MPD3,MPD2};
//void ResetMpdForNonVirtual();
//void VirtualMpdInteractSetup();


/*********************************************************************************************/
// Weapon Control
/*********************************************************************************************/

#define MAX_FIRE_SEQ        8
#define NUM_PROGRAM_CHOICES 3

int FireSeq[MAX_FIRE_SEQ] = {WING_L, WING_R, WING_CL, WING_CR, FRONT_R, BACK_R, FRONT_L, BACK_L};

int ShortSeqIndex;
int MedSeqIndex;

int	FrontOrBackHas120s;

int CurrWeapMode = WEAP_OFF;
int PrevWeapMode = WEAP_OFF;

int MedRangeDisabled;

int ShortRangeOnBoard = TRUE;
int MedRangeOnBoard = TRUE;

int GunRate = HIGH;
int BombMethod = SNGL_BMB;
int BombInterval = 320;
int NumRplBmbs = 1;

int BombStations[MAX_F18E_STATIONS] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
					   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

int WeaponsLoaded;

void GetAAWeaponCount(int *ShortCnt, int *MedCnt);
int  CanSelectBombStation(int Station);
void ResetMissleStatus(void);
void SetSRMStatus(void);
void SetMRMStatus(void);


//***********************************************************************************************************************************
//*** AG LOADOUT
//***********************************************************************************************************************************

#define TOTAL_DETONATE_ALT 5
int CbuDetonateAlt[TOTAL_DETONATE_ALT] = {300,600,1000,1500,2000};
int CbuDetonateAltIndex = 2;

//***********************************************************************************************************************************
//*** ADI
//***********************************************************************************************************************************

float TurnRate;
int FirstCalcTurnRate;
int LastTimeCalcTurnRate;
FPoint PrevPosition;
FPoint PrevProjPosition;

//***********************************************************************************************************************************
//*** TEWS DATA
//***********************************************************************************************************************************

void DrawTEWSRadarSymbol(int ScrX, int ScrY, int RadarSymId, int TargetId, int UseId, int Color, int Green, int Blue);

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

typedef struct
{
  int DeclutterOn;
  int CurMode;
  int Range;
  int IcsStatus;
  int LeftArrowColor;
  int RightArrowColor;
  int UpArrowColor;
  int DownArrowColor;
} TewsInfoType;

TewsInfoType TewsInfo;

#define TEWS_MAX_ENTITIES 400

typedef struct
{
  int NumPlanes;
  PlaneParams *Planes[TEWS_MAX_ENTITIES];
} InboundMisslePlaneListType;

typedef struct
{
  int NumSAMObjs;
  BasicInstance *SAMObjs[TEWS_MAX_ENTITIES];
} InboundSAMObjListType;


typedef struct
{
  int NumVehicles;
  MovingVehicleParams *Vehicles[TEWS_MAX_ENTITIES];
} InboundVehicleListType;

InboundMisslePlaneListType PlanesFiring;
InboundSAMObjListType      SAMObjFiring;
InboundVehicleListType      VehiclesFiring;

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
	int Category;
} ThreatEntryType;

typedef struct
{
  int NumThreats;
  ThreatEntryType List[TEWS_MAX_ENTITIES];
}TewsThreatsType;

TewsThreatsType Threats;
TewsThreatsType PrevThreats;

ThreatEntryType *CurThreat;

extern DWORD g_dwMissileLaunchSound;
extern int g_iSoundLevelCaution;

int TewsBlink;
int LastTewsBlinkTime = 0;

int TewsLockTimer;
int TewsMissleLaunchWarnOn;

typedef struct
{
  int ChaffFlareButtonPressed;
  int TimeManChaffFlareReleased;
  int NumChaffToRelease;
  int NumFlareToRelease;
  int TimeAutoChaffFlareReleased;
  int PlaneLocked;
  int TimeForLockDelay;
  int AutoTimeStart;
} TewsCounterMeasuresType;

#define TEWS_SEMI_BLINK_TIME 2000
#define TEWS_AUTO_BLINK_TIME 200

TewsCounterMeasuresType CMeasures;

int ChaffSchedule[120]; // list of times
int NumChaffScheduled = 0;

int ChaffWarningGiven;
int FlareWarningGiven;

void ManualChaffRelease(void);
void ManualFlareRelease(void);

int GetNumChaffToRelease(void);
int GetNumFlareToRelease(void);

int PlaneLocked(void);
int IsMissleFiredAtPlayer(void);

//***********************************************************************************************************************************
// ***** Virtual Mpd Vars

int InteractCockpitMode;
int DrawPopMpds;
//int VirtualMpdsOn;
//GrBuff *MpdOverlay;

//***********************************************************************************************************************************
// ***** ADI Vars



//***********************************************************************************************************************************
// ***** Mpd Clear Vars

#define MPD_CORNER_SIZE 40

typedef struct
{
  GrBuff *Buff;
  int ScrX;
  int ScrY;
} MpdClearBuffs;

MpdClearBuffs MpdCorners[4][MAX_MPDS];


//***********************************************************************************************************************************
// ***** EASY MODE WEAP VARS

void InsertEasyAA(int Station, int WeapIndex);
void InsertEasyAG(int Station, int WeapIndex);
void RemoveEasyAA(int Station);
void RemoveEasyAG(int Station);
void InitEasyWeapStores(void);
void EasyModeMissleFire(void);
void SetEasyAGMode(void);
void SetEasyAAMode(void);
void SetEasyNAVMode(void);
void PrintEasyAA();
void PrintEasyAG();

void CleanupMiscMpdSprites(void);
void DrawFireInfo(void);


typedef struct
{
  int Station;
  int WeapIndex;
} EasyModeStoresEntryType;

typedef struct
{
  int NumWeaps;
  EasyModeStoresEntryType List[MAX_F18E_STATIONS];
  int CurWeap;
} EasyModeStoresType;

EasyModeStoresType EasyAAStores;
EasyModeStoresType EasyAGStores;

int UseEasyAAStores;

int EasyHasWeapons;

// FUEL LOADOUT VARS
int LeftSideHasFuelTank;
int RightSideHasFuelTank;


//***********************************************************************************************************************************
// ***** Counter Measure Code

void ManualChaffRelease(void)
{

  if( (TewsInfo.CurMode == TEWS_SEMI_AUTO_MODE) && (CMeasures.TimeManChaffFlareReleased == -1) )
  {
	CMeasures.ChaffFlareButtonPressed = TRUE;
  }

}

void ManualFlareRelease(void)
{
  CMeasures.ChaffFlareButtonPressed = TRUE;
}

int GetNumChaffToRelease()
{
  return(CMeasures.NumChaffToRelease);
}

int GetNumFlareToRelease()
{
  return(CMeasures.NumFlareToRelease);
}


//***********************************************************************************************************************************
// *****

void PlayerDropChaff()
{
  int NumChaffToDrop;
  int CurTime = GameLoopInTicks;
  int ChaffRelease = FALSE;

  NumChaffToDrop = GetNumChaffToRelease();

  for(int i=0; i<NumChaffToDrop; i++)
  {
    if(ChaffTotal - NumChaffScheduled > 0)
	{
	  ChaffRelease = TRUE;
      NumChaffScheduled++;
	  ChaffSchedule[NumChaffScheduled-1] = CurTime + i*300;
    }
  }

  if(ChaffRelease)
    ManualChaffRelease();

}

//***********************************************************************************************************************************
// *****

void SimChaffDrop()
{
  int *TimeWalk = &ChaffSchedule[0];
  int *LastTime = &ChaffSchedule[NumChaffScheduled-1];
  int Count     = NumChaffScheduled;
  int CurTime   = GameLoopInTicks;

  while(Count-- > 0)
  {
    if(CurTime > *TimeWalk)
	{
      *TimeWalk = *LastTime;
      NumChaffScheduled--;
	  if(cPlayerLimitWeapons)
	      ChaffTotal--;
  	  InstantiateCounterMeasure(PlayerPlane,CHAFF_FLARE,1);
	  return;               // found one to drop so lets exit.
    }


    TimeWalk++;

  }

}

//***********************************************************************************************************************************
// *****

int CalcChaffToDrop()
{
  return(2);
}

//***********************************************************************************************************************************
// *****

void ResetCounterMeasures()
{
  CMeasures.ChaffFlareButtonPressed = FALSE;
  CMeasures.TimeManChaffFlareReleased = -1;
  CMeasures.NumChaffToRelease = 0;
  CMeasures.NumFlareToRelease = 0;
  CMeasures.TimeAutoChaffFlareReleased = -1;
  CMeasures.PlaneLocked = FALSE;
  CMeasures.TimeForLockDelay = -1;
  CMeasures.AutoTimeStart = -1;
}

//***********************************************************************************************************************************
// *****

void SimCounterMeasuresF18()
{
  int BlinkTime;
  int AutoDropChaff = FALSE;

  // turn lights on in Warning Panel for Flare and Chaff
  // only if loadout shows chaff and flare exists
  if(PlayerPlane->WeapLoad[CHAFF_FLARE].Count > 0)
  {
    if(ChaffTotal < 20)
    {
	   if(!ChaffWarningGiven)
       {
         DisplayWarning(MASTER_CAUTION, ON, 0);
         SetMasterWarning(WARN_MINIMUM);
         SetMasterWarning(WARN_CHAFF);
         ChaffWarningGiven = TRUE;
	   }
    }

    if(FlareTotal < 20)
    {
	   if(!FlareWarningGiven)
	   {
         DisplayWarning(MASTER_CAUTION, ON, 0);
         SetMasterWarning(WARN_MINIMUM);
         SetMasterWarning(WARN_FLARE);
         FlareWarningGiven = TRUE;
	   }
    }
  }

  if(TewsInfo.CurMode == TEWS_MANUAL_MODE)  // trivial
  {
    CMeasures.NumChaffToRelease = 1;
    CMeasures.NumFlareToRelease = 1;
	DisplayWarning(PROG_CHAFF,OFF, 1);
	return;
  }

  // otherwise it must be in semi-auto or auto mode. Since they do the same thing with
  // some different parameters I am handling both modes with this code

  if(CMeasures.TimeForLockDelay != -1)
  {
    if(GameLoopInTicks - CMeasures.TimeForLockDelay > 1000)
	{
      CMeasures.PlaneLocked = TRUE;
      CMeasures.TimeForLockDelay = -1;
	  if(TewsInfo.CurMode == TEWS_AUTO_MODE)
        CMeasures.AutoTimeStart = GameLoopInTicks;
	}
  }
  else if( (!CMeasures.PlaneLocked) && (IsMissleFiredAtPlayer()) )
  {
    CMeasures.TimeForLockDelay = GameLoopInTicks;
  }
  else
   CMeasures.PlaneLocked = IsMissleFiredAtPlayer();

  int HaveChaff;

  // if we are in auto and have run out of chaff, don't show the light or do anything
  //HaveChaff = ( (TewsInfo.CurMode == TEWS_AUTO_MODE) && (ChaffTotal - NumChaffScheduled > 0) );
  HaveChaff = (ChaffTotal - NumChaffScheduled > 0);

  if( (TewsInfo.CurMode == TEWS_SEMI_AUTO_MODE) && (CMeasures.PlaneLocked) )
	  DisplayWarning(PROG_CHAFF, ON, 1);

  //if((!HaveChaff || (!CMeasures.PlaneLocked)) && (TewsInfo.CurMode == TEWS_AUTO_MODE) )
  //    DisplayWarning(PROG_CHAFF,OFF, 1);

  if(!HaveChaff || (!CMeasures.PlaneLocked))
      DisplayWarning(PROG_CHAFF,OFF, 1);

  if(TewsInfo.CurMode == TEWS_AUTO_MODE)
  {
    if(CMeasures.PlaneLocked)
	{
      if(GameLoopInTicks - CMeasures.AutoTimeStart > 1000)
	  {
        CMeasures.AutoTimeStart = GameLoopInTicks;
        CMeasures.TimeManChaffFlareReleased = GameLoopInTicks;
		AutoDropChaff = TRUE;
		PlayerPlane->AI.iAIFlags1 |= AIJAMMINGON;
	  }
	}
	else
	{
	 CMeasures.AutoTimeStart = -1;
     PlayerPlane->AI.iAIFlags1 &= ~AIJAMMINGON;
	}
  }

  if(CMeasures.PlaneLocked)
  {
    if(TewsInfo.CurMode == TEWS_AUTO_MODE)
      BlinkTime = TEWS_AUTO_BLINK_TIME;
	else
      BlinkTime = TEWS_SEMI_BLINK_TIME;

    CMeasures.NumChaffToRelease = CalcChaffToDrop();
    CMeasures.NumFlareToRelease = 1;

	if(CMeasures.ChaffFlareButtonPressed)
	{
      CMeasures.ChaffFlareButtonPressed   = FALSE;
	  CMeasures.TimeManChaffFlareReleased = GameLoopInTicks;
	}

	if(AutoDropChaff)
	{
      PlayerDropChaff();
	}

	if(CMeasures.TimeManChaffFlareReleased != -1)
	{
      if((int)GameLoopInTicks - CMeasures.TimeManChaffFlareReleased < BlinkTime)
	  {
        CMeasures.NumChaffToRelease = 0; // can't drop chaff while program is calculating
        DisplayWarning(PROG_CHAFF, OFF, 1);
	  }
	  else
	  {
		if(ChaffTotal - NumChaffScheduled > 0)
		   DisplayWarning(PROG_CHAFF, ON, 1);  // JLM debug temporary need to use real light
		CMeasures.TimeManChaffFlareReleased = -1;
      }

	}

  }

}

//***********************************************************************************************************************************
// *****  DEBUG ROUTINE

void DrawCircleDebug2(int XOfs, int YOfs, int Test)
{
  sprintf(TmpStr,"%d", Test);
  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs,YOfs, TmpStr,  61);
}

//***********************************************************************************************************************************
// *****  DEBUG ROUTINE

void FDrawCircleDebug2(int XOfs, int YOfs, double Test)
{

  float Tst1;

  Tst1 = Test;
  sprintf(TmpStr,"x    %f", Tst1);
  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs,YOfs, TmpStr,  61);
}

//***********************************************************************************************************************************
// *****

void LoadMpdProgramming()
{
  for(int i=NAV_MODE; i <= INST_MODE; i++)
  {
	for(int j=0; j < MAX_UNIQUE_MPDS; j++)
	{
      Mpds[j].ProgModeInfo.ModeList[i] = MpdProgrammingMatrix[j][i];
	}
  }
}

//***********************************************************************************************************************************
// *****

void LoadDefaultMpdProgramming()
{
  for(int i=NAV_MODE; i <= INST_MODE; i++)
  {
	for(int j=0; j < MAX_UNIQUE_MPDS; j++)
	{
      MpdProgrammingMatrix[j][i] = DefaultMpdProgrammingMatrix[j][i];
	}
  }
}

//***********************************************************************************************************************************
// *****

void SaveMpdProgramming()
{

  if(g_Settings.gp.nAARADAR != GP_TYPE_CASUAL)
  {
    for(int i=NAV_MODE; i <= INST_MODE; i++)
    {
	  for(int j=0; j < MAX_UNIQUE_MPDS; j++)
	  {
        MpdProgrammingMatrix[j][i] = Mpds[j].ProgModeInfo.ModeList[i];
	  }
    }
  }
  else
  {
    for(int i=NAV_MODE; i <= INST_MODE; i++)
    {
	  for(int j=0; j < MAX_UNIQUE_MPDS; j++)
	  {
        MpdProgrammingMatrix[j][i] = DefaultMpdProgrammingMatrix[j][i];
	  }
    }
  }
}

//***********************************************************************************************************************************
// *****

void SetMpdButtonSprites(MpdButtonSpriteType *ButtonSprites)
{
	int SpriteButtonWidth  =	12;
	int SpriteButtonHeight = 14;

	// left
	ButtonSprites[LEFT].UpSprX = 70; ButtonSprites[LEFT].UpSprY = 282;
	ButtonSprites[LEFT].DwnSprX = 83; ButtonSprites[LEFT].DwnSprY = 282;
	ButtonSprites[LEFT].LUpSprX = 70; ButtonSprites[LEFT].LUpSprY = 265;
	ButtonSprites[LEFT].LDwnSprX = 83; ButtonSprites[LEFT].LDwnSprY = 265;
	ButtonSprites[LEFT].SprW = SpriteButtonWidth;  ButtonSprites[LEFT].SprH = SpriteButtonHeight;

	// right
	ButtonSprites[RIGHT].UpSprX = 168; ButtonSprites[RIGHT].UpSprY = 282;
	ButtonSprites[RIGHT].DwnSprX = 155; ButtonSprites[RIGHT].DwnSprY = 282;
	ButtonSprites[RIGHT].LUpSprX = 168; ButtonSprites[RIGHT].LUpSprY = 265;
	ButtonSprites[RIGHT].LDwnSprX = 155; ButtonSprites[RIGHT].LDwnSprY = 265;
	ButtonSprites[RIGHT].SprW = SpriteButtonWidth;  ButtonSprites[RIGHT].SprH = SpriteButtonHeight;

	// top
	ButtonSprites[BOTTOM].UpSprX = 29; ButtonSprites[BOTTOM].UpSprY = 282;
	ButtonSprites[BOTTOM].DwnSprX = 42; ButtonSprites[BOTTOM].DwnSprY = 282;
	ButtonSprites[BOTTOM].LUpSprX = 29; ButtonSprites[BOTTOM].LUpSprY = 265;
	ButtonSprites[BOTTOM].LDwnSprX = 42; ButtonSprites[BOTTOM].LDwnSprY = 265;
	ButtonSprites[BOTTOM].SprW = SpriteButtonWidth; ButtonSprites[BOTTOM].SprH = SpriteButtonHeight;

	// bottom
	ButtonSprites[TOP].UpSprX = 113; ButtonSprites[TOP].UpSprY = 282;
	ButtonSprites[TOP].DwnSprX = 126; ButtonSprites[TOP].DwnSprY = 282;
	ButtonSprites[TOP].LUpSprX = 113; ButtonSprites[TOP].LUpSprY = 265;
	ButtonSprites[TOP].LDwnSprX = 126; ButtonSprites[TOP].LDwnSprY = 265;
	ButtonSprites[TOP].SprW = SpriteButtonWidth; ButtonSprites[TOP].SprH = SpriteButtonHeight;

}

//***********************************************************************************************************************************
void SetMpdCornerInfo()
{
	// init the corner sprite info
	MpdCorners[0][FRONT_LEFT_MPD].ScrX = 64;
	MpdCorners[0][FRONT_LEFT_MPD].ScrY = 320;
	MpdCorners[1][FRONT_LEFT_MPD].ScrX = 181;
	MpdCorners[1][FRONT_LEFT_MPD].ScrY = 320;
	MpdCorners[2][FRONT_LEFT_MPD].ScrX = 181;
	MpdCorners[2][FRONT_LEFT_MPD].ScrY = 425;
	MpdCorners[3][FRONT_LEFT_MPD].ScrX = 64;
	MpdCorners[3][FRONT_LEFT_MPD].ScrY = 425;

	MpdCorners[0][FRONT_RIGHT_MPD].ScrX = 424;
	MpdCorners[0][FRONT_RIGHT_MPD].ScrY = 320;
	MpdCorners[1][FRONT_RIGHT_MPD].ScrX = 532;
	MpdCorners[1][FRONT_RIGHT_MPD].ScrY = 320;
	MpdCorners[2][FRONT_RIGHT_MPD].ScrX = 532;
	MpdCorners[2][FRONT_RIGHT_MPD].ScrY = 423;
	MpdCorners[3][FRONT_RIGHT_MPD].ScrX = 424;
	MpdCorners[3][FRONT_RIGHT_MPD].ScrY = 423;

	MpdCorners[0][FRONT_DOWN_LEFT_MPD].ScrX = 64;
	MpdCorners[0][FRONT_DOWN_LEFT_MPD].ScrY = 121;
	MpdCorners[1][FRONT_DOWN_LEFT_MPD].ScrX = 181;
	MpdCorners[1][FRONT_DOWN_LEFT_MPD].ScrY = 121;
	MpdCorners[2][FRONT_DOWN_LEFT_MPD].ScrX = 181;
	MpdCorners[2][FRONT_DOWN_LEFT_MPD].ScrY = 225;
	MpdCorners[3][FRONT_DOWN_LEFT_MPD].ScrX = 64;
	MpdCorners[3][FRONT_DOWN_LEFT_MPD].ScrY = 225;

	MpdCorners[0][FRONT_DOWN_RIGHT_MPD].ScrX = 424;
	MpdCorners[0][FRONT_DOWN_RIGHT_MPD].ScrY = 120;
	MpdCorners[1][FRONT_DOWN_RIGHT_MPD].ScrX = 532;
	MpdCorners[1][FRONT_DOWN_RIGHT_MPD].ScrY = 120;
	MpdCorners[2][FRONT_DOWN_RIGHT_MPD].ScrX = 532;
	MpdCorners[2][FRONT_DOWN_RIGHT_MPD].ScrY = 223;
	MpdCorners[3][FRONT_DOWN_RIGHT_MPD].ScrX = 424;
	MpdCorners[3][FRONT_DOWN_RIGHT_MPD].ScrY = 223;

	MpdCorners[0][FRONT_DOWN_CENTER_MPD].ScrX = 244;
	MpdCorners[0][FRONT_DOWN_CENTER_MPD].ScrY = 298;
	MpdCorners[1][FRONT_DOWN_CENTER_MPD].ScrX = 350;
	MpdCorners[1][FRONT_DOWN_CENTER_MPD].ScrY = 298;
	MpdCorners[2][FRONT_DOWN_CENTER_MPD].ScrX = 350;
	MpdCorners[2][FRONT_DOWN_CENTER_MPD].ScrY = 398;
	MpdCorners[3][FRONT_DOWN_CENTER_MPD].ScrX = 244;
	MpdCorners[3][FRONT_DOWN_CENTER_MPD].ScrY = 398;

	MpdCorners[0][BACK_RIGHT_MPD].ScrX = 498;
	MpdCorners[0][BACK_RIGHT_MPD].ScrY = 328;
	MpdCorners[1][BACK_RIGHT_MPD].ScrX = 600;
	MpdCorners[1][BACK_RIGHT_MPD].ScrY = 328;
	MpdCorners[2][BACK_RIGHT_MPD].ScrX = 600;
	MpdCorners[2][BACK_RIGHT_MPD].ScrY = 428;
	MpdCorners[3][BACK_RIGHT_MPD].ScrX = 498;
	MpdCorners[3][BACK_RIGHT_MPD].ScrY = 428;

	MpdCorners[0][BACK_CENTER_RIGHT_MPD].ScrX = 332;
	MpdCorners[0][BACK_CENTER_RIGHT_MPD].ScrY = 320;
	MpdCorners[1][BACK_CENTER_RIGHT_MPD].ScrX = 436;
	MpdCorners[1][BACK_CENTER_RIGHT_MPD].ScrY = 320;
	MpdCorners[2][BACK_CENTER_RIGHT_MPD].ScrX = 436;
	MpdCorners[2][BACK_CENTER_RIGHT_MPD].ScrY = 420;
	MpdCorners[3][BACK_CENTER_RIGHT_MPD].ScrX = 332;
	MpdCorners[3][BACK_CENTER_RIGHT_MPD].ScrY = 420;

	MpdCorners[0][BACK_CENTER_LEFT_MPD].ScrX = 162;
	MpdCorners[0][BACK_CENTER_LEFT_MPD].ScrY = 320;
	MpdCorners[1][BACK_CENTER_LEFT_MPD].ScrX = 268;
	MpdCorners[1][BACK_CENTER_LEFT_MPD].ScrY = 320;
	MpdCorners[2][BACK_CENTER_LEFT_MPD].ScrX = 268;
	MpdCorners[2][BACK_CENTER_LEFT_MPD].ScrY = 420;
	MpdCorners[3][BACK_CENTER_LEFT_MPD].ScrX = 162;
	MpdCorners[3][BACK_CENTER_LEFT_MPD].ScrY = 420;

	MpdCorners[0][BACK_LEFT_MPD].ScrX = 0;
	MpdCorners[0][BACK_LEFT_MPD].ScrY = 328;
	MpdCorners[1][BACK_LEFT_MPD].ScrX = 104;
	MpdCorners[1][BACK_LEFT_MPD].ScrY = 328;
	MpdCorners[2][BACK_LEFT_MPD].ScrX = 104;
	MpdCorners[2][BACK_LEFT_MPD].ScrY = 428;
	MpdCorners[3][BACK_LEFT_MPD].ScrX = 0;
	MpdCorners[3][BACK_LEFT_MPD].ScrY = 428;
}

void SetMpdInfo()
{

	InitMavGbu();
	InitFlir();

	// JLM HERE TEMPORARILY, GOES IN SIMMAIN;
	InitJettisonInfo();

	ButtonClickInfo.ButtonPressed     = FALSE;
	AATargetSelectInfo.TargetSelected = FALSE;
//	VirtualMpdsOn                     = FALSE;
	DrawPopMpds                   = FALSE;

	FirstCalcTurnRate = TRUE;
	TurnRate = 0;

	MPD.Active = FALSE;

	MPD.MpdList[FRONT_LEFT_MPD ].Cockpit = FRONT_FORWARD_COCKPIT;
	MPD.MpdList[FRONT_RIGHT_MPD ].Cockpit = FRONT_FORWARD_COCKPIT;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].Cockpit = FRONT_DOWN_COCKPIT;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].Cockpit = FRONT_DOWN_COCKPIT;
	MPD.MpdList[BACK_RIGHT_MPD].Cockpit = BACK_FORWARD_COCKPIT;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].Cockpit = BACK_FORWARD_COCKPIT;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].Cockpit = BACK_FORWARD_COCKPIT;
	MPD.MpdList[BACK_LEFT_MPD].Cockpit = BACK_FORWARD_COCKPIT;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].Cockpit = FRONT_DOWN_COCKPIT;

	SetMpdCornerInfo();

	MPD.MpdList[FRONT_LEFT_MPD].ScrX = 0;
	MPD.MpdList[FRONT_LEFT_MPD].ScrY = 0;
	MPD.MpdList[FRONT_LEFT_MPD].CenterX =  70;//68;
	MPD.MpdList[FRONT_LEFT_MPD].CenterY =  70;//60;


	/* Old MPD coordinates
	MPD.MpdList[FRONT_LEFT_MPD].ScrX =  54;
	MPD.MpdList[FRONT_LEFT_MPD].ScrY =  309;
	MPD.MpdList[FRONT_LEFT_MPD].CenterX =  138;
	MPD.MpdList[FRONT_LEFT_MPD].CenterY =  386;
	*/

	MPD.MpdList[FRONT_RIGHT_MPD].ScrX = 412;
	MPD.MpdList[FRONT_RIGHT_MPD].ScrY = 309;
	MPD.MpdList[FRONT_RIGHT_MPD].CenterX = 496;
	MPD.MpdList[FRONT_RIGHT_MPD].CenterY = 386;

	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].ScrX = 412;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].ScrY = 108;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].CenterX =  496;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].CenterY =  185;

	MPD.MpdList[FRONT_DOWN_LEFT_MPD].ScrX	= 54;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].ScrY = 108;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].CenterX =  138;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].CenterY =  185;

	MPD.MpdList[BACK_RIGHT_MPD].ScrX =  488;
	MPD.MpdList[BACK_RIGHT_MPD].ScrY =  316;
	MPD.MpdList[BACK_RIGHT_MPD].CenterX =  570;
	MPD.MpdList[BACK_RIGHT_MPD].CenterY =  393;

	MPD.MpdList[BACK_CENTER_RIGHT_MPD].ScrX = 321;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].ScrY = 308;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].CenterX =  403;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].CenterY =  385;

	MPD.MpdList[BACK_CENTER_LEFT_MPD].ScrX = 153;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].ScrY = 308;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].CenterX =  235;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].CenterY =  385;

	MPD.MpdList[BACK_LEFT_MPD].ScrX =  -13;
	MPD.MpdList[BACK_LEFT_MPD].ScrY =	 316;
	MPD.MpdList[BACK_LEFT_MPD].CenterX = 69;
	MPD.MpdList[BACK_LEFT_MPD].CenterY = 393;

	MPD.MpdList[FRONT_DOWN_CENTER_MPD].ScrX =	 232;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].ScrY =	 286;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].CenterX =  316;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].CenterY =  362;

	/*

	MPD.MpdList[FRONT_LEFT_MPD].ScrX =  54;
	MPD.MpdList[FRONT_LEFT_MPD].ScrY =  309;
	MPD.MpdList[FRONT_LEFT_MPD].CenterX =  70 + 68;
	MPD.MpdList[FRONT_LEFT_MPD].CenterY =  324 + 67;

	MPD.MpdList[FRONT_RIGHT_MPD].ScrX = 412;
	MPD.MpdList[FRONT_RIGHT_MPD].ScrY = 309;
	MPD.MpdList[FRONT_RIGHT_MPD].CenterX = 428 + 68;
	MPD.MpdList[FRONT_RIGHT_MPD].CenterY = 324 + 67;

	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].ScrX = 412;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].ScrY = 108;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].CenterX =  428 + 68;
	MPD.MpdList[FRONT_DOWN_RIGHT_MPD].CenterY =  123 + 67;

	MPD.MpdList[FRONT_DOWN_LEFT_MPD].ScrX	= 54;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].ScrY = 108;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].CenterX =  70 + 68;
	MPD.MpdList[FRONT_DOWN_LEFT_MPD].CenterY =  123 + 67;

	MPD.MpdList[BACK_RIGHT_MPD].ScrX =  488;
	MPD.MpdList[BACK_RIGHT_MPD].ScrY =  316;
	MPD.MpdList[BACK_RIGHT_MPD].CenterX =  502 + 68;
	MPD.MpdList[BACK_RIGHT_MPD].CenterY =  331 + 67;

	MPD.MpdList[BACK_CENTER_RIGHT_MPD].ScrX = 321;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].ScrY = 308;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].CenterX =  335 + 68;
	MPD.MpdList[BACK_CENTER_RIGHT_MPD].CenterY =  323 + 67;

	MPD.MpdList[BACK_CENTER_LEFT_MPD].ScrX = 153;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].ScrY = 308;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].CenterX =  167 + 68;
	MPD.MpdList[BACK_CENTER_LEFT_MPD].CenterY =  323 + 67;

	MPD.MpdList[BACK_LEFT_MPD].ScrX =  -13;
	MPD.MpdList[BACK_LEFT_MPD].ScrY =	 316;
	MPD.MpdList[BACK_LEFT_MPD].CenterX = 1 + 68;
	MPD.MpdList[BACK_LEFT_MPD].CenterY = 331 + 67;

	MPD.MpdList[FRONT_DOWN_CENTER_MPD].ScrX =	 232;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].ScrY =	 286;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].CenterX =  248 + 68;
	MPD.MpdList[FRONT_DOWN_CENTER_MPD].CenterY =  301 + 67;

	*/


 for(int i=0; i<MAX_MPD_DAMAGE; i++)
	MpdDamage[i] = FALSE;

 for(i=0; i<MAX_UNIQUE_MPDS; i++)
 {
   Mpds[i].Mode = MAIN_MENU_MODE;
   Mpds[i].ProgModeInfo.CurButton   = 0;
   Mpds[i].ProgModeInfo.Active      = FALSE;
   Mpds[i].ProgModeInfo.ModeList[0] = MAIN_MENU_MODE;
   Mpds[i].ProgModeInfo.ModeList[1] = MAIN_MENU_MODE;
   Mpds[i].ProgModeInfo.ModeList[2] = MAIN_MENU_MODE;
   Mpds[i].ProgModeInfo.ModeList[3] = MAIN_MENU_MODE;


   Mpds[i].HsiInfo.Range = 80;
   Mpds[i].HsiInfo.Mode =  NAV;

   Mpds[i].AAInfo.TextChangeCountDown = 2;
   Mpds[i].AGInfo.TextChangeCountDown = 2;
   Mpds[i].AGInfo.CurMode             = RBM_MODE;
   Mpds[i].AGInfo.SelectMode    	  = MAP_MODE;
   Mpds[i].AGInfo.MapSubMode		  = MAP_SELECT_MODE;
   Mpds[i].AGInfo.ErrorCode     	  = NO_AG_ERROR;
   Mpds[i].AGInfo.ScanOn              = FALSE;
   Mpds[i].AGInfo.FreezeErrorStartTime= -1;
   Mpds[i].AGInfo.EmisErrorStartTime  = -1;
   Mpds[i].AGInfo.MMRadarError.ClickNum = 0;
   Mpds[i].AGInfo.MMRadarError.StartTime = -1;

   Mpds[i].WeapInfo.StartTime = -1;
   Mpds[i].WeapInfo.ModeToUse = MAVERIC_TYPE;

   Mpds[i].FlirInfo.StartTime = -1;

   if( (i == MPD2) || (i == MPD6) || (i == MPD3) )
   {
      Mpds[i].ColorInfo.TextColor = 12;
	  Mpds[i].ColorInfo.AdiGroundColor = 6;
	  Mpds[i].ColorInfo.AdiSkyColor = 94;
	  Mpds[i].ColorInfo.LineColor = 12;
	  Mpds[i].IsMono = FALSE;
   }
   else
   {
	  Mpds[i].ColorInfo.TextColor = 61;
	  Mpds[i].ColorInfo.AdiGroundColor = 61;
	  Mpds[i].ColorInfo.AdiSkyColor = 56;
	  Mpds[i].ColorInfo.LineColor = 61;
      Mpds[i].IsMono = TRUE;
     }
 }

  // set this so reprogramming will take place at beginning of sim
  PrevHUDMode = -1;

  // if no program mode has been saved, use the default
 // if(MpdProgrammingMatrix[0][0] == -1)
  // for f18 there is no programming, just set the default always
  LoadDefaultMpdProgramming();

  LoadMpdProgramming();

  /*
  Mpds[MPD0].ProgModeInfo.ModeList[AA_MODE] = AA_ARM_MODE;
  Mpds[MPD1].ProgModeInfo.ModeList[AA_MODE] = AA_RADAR_MODE;
  Mpds[MPD2].ProgModeInfo.ModeList[AA_MODE] = TSD_MODE;

  Mpds[MPD0].ProgModeInfo.ModeList[AG_MODE] = AG_ARM_MODE;
  Mpds[MPD1].ProgModeInfo.ModeList[AG_MODE] = AG_RADAR_MODE;
  Mpds[MPD2].ProgModeInfo.ModeList[AG_MODE] = TSD_MODE;

  Mpds[MPD0].ProgModeInfo.ModeList[NAV_MODE] = HSI_MODE;
  Mpds[MPD1].ProgModeInfo.ModeList[NAV_MODE] = AA_RADAR_MODE;
  Mpds[MPD2].ProgModeInfo.ModeList[NAV_MODE] = TSD_MODE;

  Mpds[MPD0].ProgModeInfo.ModeList[INST_MODE] = HSI_MODE;
  Mpds[MPD1].ProgModeInfo.ModeList[INST_MODE] = AA_RADAR_MODE;
  Mpds[MPD2].ProgModeInfo.ModeList[INST_MODE] = ADI_MODE;
  */

  for(i=0; i<MAX_MPDS; i++)
  {
	MPD.MpdList[i].Width = 168;
	MPD.MpdList[i].Height = 164;
    MPD.MpdList[i].TransX = 0;
    MPD.MpdList[i].TransY = 0;
  }

  for(i=0; i<MAX_MPDS; i++)
  {
	for(int j=0; j<MAX_MPD_BUTTONS; j++)
	{
	  MPD.MpdList[i].MpdButtons[j].Width = ButtonWidth;
	  MPD.MpdList[i].MpdButtons[j].Height = ButtonHeight;

	  MPD.MpdList[i].MpdButtons[j].State = RELEASE;
	  MPD.MpdList[i].MpdButtons[j].FramesToDraw = 0;

	  MPD.MpdList[i].MpdButtons[j].ScrX = MpdButtonScreenPos[i][j].XOff;
	  MPD.MpdList[i].MpdButtons[j].ScrY	= MpdButtonScreenPos[i][j].YOff;

	  MPD.MpdList[i].MpdButtons[j].ButtonDirection = int(floor(float(float(j)/float(5))));

	} // for

  }	// for

} // proc


//***********************************************************************************************************************************
void CalcMonoPalette()
{
   int Row;

   for(int i=0; i<256; i++)
   {
	  if( (i < 58) || (i > 63) )
	  {
	    Row = i % 16;
		MonoPal[i] = 63 - float(float(Row)/15.0)*6;
	  }
	  else
	  {
	    MonoPal[i] = i;
	  }
   }
}

//***********************************************************************************************************************************
// *****
/*
void InitMpdViewBoxInfo(void)
{
  MpdViewBoxes[FRONT_RIGHT_MPD].Left    = 440;
  MpdViewBoxes[FRONT_RIGHT_MPD].Right	= 560;
  MpdViewBoxes[FRONT_RIGHT_MPD].Top	    = 332;
  MpdViewBoxes[FRONT_RIGHT_MPD].Bottom	= 452;
  MpdViewBoxes[FRONT_RIGHT_MPD].Width 	= 560 - 440;
  MpdViewBoxes[FRONT_RIGHT_MPD].Height	= 452 - 332;

  MpdViewBoxes[FRONT_LEFT_MPD].Left	    =  78;
  MpdViewBoxes[FRONT_LEFT_MPD].Right    =  198;
  MpdViewBoxes[FRONT_LEFT_MPD].Top	   	=  332;
  MpdViewBoxes[FRONT_LEFT_MPD].Bottom	=  452;
  MpdViewBoxes[FRONT_LEFT_MPD].Width 	=  198-78;
  MpdViewBoxes[FRONT_LEFT_MPD].Height	=  452-332;

  MpdViewBoxes[FRONT_DOWN_RIGHT_MPD].Left   = 440;
  MpdViewBoxes[FRONT_DOWN_RIGHT_MPD].Right	= 552;
  MpdViewBoxes[FRONT_DOWN_RIGHT_MPD].Top	= 332 - 200;
  MpdViewBoxes[FRONT_DOWN_RIGHT_MPD].Bottom	= 450 - 200;
  MpdViewBoxes[FRONT_DOWN_RIGHT_MPD].Width 	= 552-440;
  MpdViewBoxes[FRONT_DOWN_RIGHT_MPD].Height	= (450 - 200) - (332 - 200);

  MpdViewBoxes[FRONT_DOWN_LEFT_MPD].Left	=  82;
  MpdViewBoxes[FRONT_DOWN_LEFT_MPD].Right   =  194;
  MpdViewBoxes[FRONT_DOWN_LEFT_MPD].Top	   	=  134;
  MpdViewBoxes[FRONT_DOWN_LEFT_MPD].Bottom	=  245;
  MpdViewBoxes[FRONT_DOWN_LEFT_MPD].Width 	=  194 - 82;
  MpdViewBoxes[FRONT_DOWN_LEFT_MPD].Height	=  245 - 134;

  MpdViewBoxes[FRONT_DOWN_CENTER_MPD].Left   =	260;
  MpdViewBoxes[FRONT_DOWN_CENTER_MPD].Right	 =	372;
  MpdViewBoxes[FRONT_DOWN_CENTER_MPD].Top	 =  313;
  MpdViewBoxes[FRONT_DOWN_CENTER_MPD].Bottom =	421;
  MpdViewBoxes[FRONT_DOWN_CENTER_MPD].Width  =	372 - 260;
  MpdViewBoxes[FRONT_DOWN_CENTER_MPD].Height =	421 - 313;

  MpdViewBoxes[BACK_RIGHT_MPD].Left     =	514;
  MpdViewBoxes[BACK_RIGHT_MPD].Right	=	626;
  MpdViewBoxes[BACK_RIGHT_MPD].Top	    =   343;
  MpdViewBoxes[BACK_RIGHT_MPD].Bottom	=	451;
  MpdViewBoxes[BACK_RIGHT_MPD].Width 	=	626 - 514;
  MpdViewBoxes[BACK_RIGHT_MPD].Height	=	451 - 343;

  MpdViewBoxes[BACK_LEFT_MPD].Left	    =    12;
  MpdViewBoxes[BACK_LEFT_MPD].Right     =	 125;
  MpdViewBoxes[BACK_LEFT_MPD].Top	   	=	 343;
  MpdViewBoxes[BACK_LEFT_MPD].Bottom	=	 451;
  MpdViewBoxes[BACK_LEFT_MPD].Width 	=	 125 - 12 ;
  MpdViewBoxes[BACK_LEFT_MPD].Height	=	 451 - 343;

  MpdViewBoxes[BACK_CENTER_RIGHT_MPD].Left   =  347;
  MpdViewBoxes[BACK_CENTER_RIGHT_MPD].Right	 =	459;
  MpdViewBoxes[BACK_CENTER_RIGHT_MPD].Top	 =  335;
  MpdViewBoxes[BACK_CENTER_RIGHT_MPD].Bottom =	443;
  MpdViewBoxes[BACK_CENTER_RIGHT_MPD].Width  =	459 - 347;
  MpdViewBoxes[BACK_CENTER_RIGHT_MPD].Height =	443 - 335;

  MpdViewBoxes[BACK_CENTER_LEFT_MPD].Left	=  179;
  MpdViewBoxes[BACK_CENTER_LEFT_MPD].Right  =  291;
  MpdViewBoxes[BACK_CENTER_LEFT_MPD].Top    =  335;
  MpdViewBoxes[BACK_CENTER_LEFT_MPD].Bottom	=  443;
  MpdViewBoxes[BACK_CENTER_LEFT_MPD].Width 	=  291 - 179;
  MpdViewBoxes[BACK_CENTER_LEFT_MPD].Height	=  443 - 335;

}
*/

//***********************************************************************************************************************************
// *****

void ClearMpd(int MpdDetectNum)
{
  //New Clear.. Just fill whole buffer

  GrFillRectNoClip(GrBuffFor3D, 0, 0, GrBuffFor3D->pGrBuffPolygon->Width2D+2, GrBuffFor3D->pGrBuffPolygon->Height2D+2, 0, 14, 0, 0);

}

//***********************************************************************************************************************************
// *****
int ShouldCalculateHudCamera()
{
   int RetVal;

   RetVal = FALSE;

   switch (CurrentCockpit)
   {
	  case FRONT_FORWARD_COCKPIT:
	  {
		 if	(Mpds[0].Mode == HUD_MODE)
			RetVal = TRUE;
		 else if (Mpds[1].Mode == HUD_MODE)
		    RetVal = TRUE;
		 break;
	  }

	  case FRONT_DOWN_COCKPIT:
	  {
		 if	(Mpds[0].Mode == HUD_MODE)
		    RetVal = TRUE;
		 else if (Mpds[1].Mode == HUD_MODE)
			RetVal = TRUE;
		 else if (Mpds[2].Mode == HUD_MODE)
		    RetVal = TRUE;
		 break;
	  }

	  case BACK_FORWARD_COCKPIT:
	  {
		 if	(Mpds[3].Mode == HUD_MODE)
		    RetVal = TRUE;
		 else if (Mpds[4].Mode == HUD_MODE)
			RetVal = TRUE;
		 else if (Mpds[5].Mode == HUD_MODE)
			RetVal = TRUE;
		 else if (Mpds[6].Mode == HUD_MODE)
			RetVal = TRUE;
		break;

	  }
   }     // end switch

   return (RetVal);

}

//***********************************************************************************************************************************
// *****
int AreMpdsInGivenMode(int Mode)
{
  int RetVal;

  RetVal = FALSE;

	if( (CurrentView & COCKPIT_VIRTUAL_SEAT) || (CurrentView & COCKPIT_FRONT) )
	{
		for(int i=0; i<7; i++)
		{
			if(Mpds[i].Mode == Mode)
			{
					RetVal = TRUE;
					break;
			}
		}
	}

  return (RetVal);
}

//***********************************************************************************************************************************
// *****
int MapMpdDetectToMpdMode(int MpdDetectNum)
{
	  int MappedVal;

	  // front left and right are the same
	  MappedVal = MpdDetectNum;

	  // all front down need mappings
	  if(MpdDetectNum == FRONT_DOWN_CENTER_MPD)
		MappedVal = MPD2;

	  if(MpdDetectNum == FRONT_DOWN_RIGHT_MPD)
		MappedVal = MPD0;

	  if(MpdDetectNum == FRONT_DOWN_LEFT_MPD)
		MappedVal = MPD1;

	   // the back mpds are 1 less
	  if( (MpdDetectNum >= BACK_RIGHT_MPD) &&
		  (MpdDetectNum <= BACK_LEFT_MPD)       )
		MappedVal = MpdDetectNum - 1;

	  return(MappedVal);

}

//***********************************************************************************************************************************
// *****

void InitVCInteractions( void )
{
	InitVCHotspots( );
	InitPolyMpds( );

	InitTestPolyMpds( );
	InitPopUpMpds( );
}

void InitMpd()
{
  SetMpdButtonSprites(&MPD.ButtonSprites[0]);

  SetMpdInfo();

  MPD.CurrentMpd = -1;
  MPD.CurrentMpdButton = -1;


  //InitMpdViewBoxInfo();// Is this used??
  CalcMonoPalette();

  // JLM debug
  //Buff = GrAllocGrBuff(116, 111, GR_INDEXED | GR_8BIT);	 // JLM debug remove
  //MonoBuff = GrAllocGrBuff(116, 111, GR_INDEXED | GR_8BIT);	 // JLM debug remove

  InitVCInteractions();
  InitTextSurface( );


}

//***********************************************************************************************************************************
// *****
void ProcessMpdButtonPress(int MpdNum, int MpdButtonNum)
{
   ButtonClickInfo.ButtonPressed = TRUE;
   ButtonClickInfo.MpdNum = MpdNum;
   ButtonClickInfo.ButtonNum = MpdButtonNum;
}

//***********************************************************************************************************************************
// *****

void GrabButtonPress(int MpdDetectNum,int *ButtonNum, int *Pressed)
{

   *ButtonNum = -1;
   *Pressed   = FALSE;

   if (ButtonClickInfo.ButtonPressed)
   {
	 if (MpdDetectNum == ButtonClickInfo.MpdNum)
	 {
	   ButtonClickInfo.ButtonPressed = FALSE;
	   *Pressed   = TRUE;
	   *ButtonNum = ButtonClickInfo.ButtonNum;
	 }

   }

}

//***********************************************************************************************************************************
// *****

void DetermineAATargetSelect(int MouseX, int MouseY, int MpdDetectNum)
{
/*
	int MpdNum;

	MpdNum = MpdDetectToMpdMapping[MpdDetectNum];

	if(Mpds[MpdNum].Mode == AA_RADAR_MODE)
	{
		AATargetSelectInfo.TargetSelected = TRUE;
		AATargetSelectInfo.ScrX = MouseX;
		AATargetSelectInfo.ScrY = MouseY;
		AATargetSelectInfo.MpdDetectNum = MpdDetectNum;
	}
*/
}

//***********************************************************************************************************************************
// *****

void DetermineAGTargetSelect(int MouseX, int MouseY, int MpdDetectNum)
{
	/*
  int MpdNum;

  MpdNum = MpdDetectToMpdMapping[MpdDetectNum];

  if(Mpds[MpdNum].Mode == AG_RADAR_MODE)
  {
    AGButtonClickInfo.TargetSelected = TRUE;
    AGButtonClickInfo.ScrX = MouseX;
    AGButtonClickInfo.ScrY = MouseY;
    AGButtonClickInfo.MpdDetectNum = MpdDetectNum;
  }
  */

}

//***********************************************************************************************************************************
// *****

void DetermineFlirTargetSelect(int MouseX, int MouseY, int MpdDetectNum)
{
  int MpdNum;

  MpdNum = MpdDetectToMpdMapping[MpdDetectNum];

  if(Mpds[MpdNum].Mode == TGT_IR_MODE)
  {
    FlirButtonClickInfo.TargetSelected = TRUE;
    FlirButtonClickInfo.ScrX = MouseX;
    FlirButtonClickInfo.ScrY = MouseY;
    FlirButtonClickInfo.MpdDetectNum = MpdDetectNum;
  }

}

//***********************************************************************************************************************************
// *****

int FindClosestUnOccupiedMPDSlot(int MouseX, int MouseY)
{
	int *xwalk,*ywalk;
	int slot;
	int closest_slot;
	float dist;
	float closest_dist;
	BOOL done = FALSE;

	slot = 0;

	xwalk = &SlotXY[0];
	ywalk = &SlotXY[1];

	closest_slot = -1;

	while(slot < 0x18)
	{
	   	if( IsPointInside(MouseX, MouseY, *ywalk, *ywalk + 178,*xwalk,*xwalk + 175))
		{
			if ((slot == MPDSlot[MovingWhichMPD]) || ((slot != MPDSlot[0]) && (slot != MPDSlot[1]) && (slot != MPDSlot[2])))
			{
				dist = QuickDistance((float)(MouseX - *xwalk-3/* - 88*/),(float)(MouseY - *ywalk-3/* - 89*/));
				if ((closest_slot == -1) || (dist < closest_dist))
				{
					closest_slot = slot;
					closest_dist = dist;
				}
			}

		}

		xwalk+=2;
		ywalk+=2;
		slot++;
	}

	return closest_slot;
}

//***********************************************************************************************************************************
int CheckMpd(int MouseX, int MouseY, int CurrentCockpit)
{
	return 0;
}

//***********************************************************************************************************************************
// *****
/*
// Assumes non zero length strings
int StringCompare(char *Str1, char *Str2)
{
	strncp
  char *P1,*P2;

  P1 = Str1;
  P2 = Str2;

  while( (*P1 == *P2) && ( (*P1 != '\x00') && (*P2 != '\x00')  ) )
  {
    P1++;
	P2++;
  }

  if( (*P1 == '\x00') && (*P2 == '\x00') )
	  return (TRUE);

  return(FALSE);

}
*/
//***********************************************************************************************************************************
// *****

void DrawVerticalWordAtLocation(int ScrX, int ScrY,  char *pMpdString, int BoxIt, int Color, int Green, int Blue)
{
  char *P;
  int Count;
  int VertPos;
  int CharCount;
  char Str[2];

  Str[1]  = '\x00';
  P       = pMpdString;
  VertPos =	ScrY;

  CharCount = 0;
  Count     = 0;
  while(*P != '\x00')
  {
	Str[0] = *P;
	GrDrawString(GrBuffFor3D, SmHUDFont, ScrX, VertPos, Str, Color);
	VertPos += 6;
	CharCount++;
	P++;
  }

  if(BoxIt)
    DrawBox(GrBuffFor3D, ScrX - 2, ScrY - 2, 6,CharCount*5 + 5, Color);

}


//***********************************************************************************************************************************
// *****

void DrawTextAtLocation(int ScrX, int ScrY,  char *pMpdString, int ColorRed, int Green, int Blue, int Pal8)
{

  sprintf(TmpStr, pMpdString);
  GrDrawString(GrBuffFor3D, SmHUDFont, ScrX, ScrY, TmpStr, ColorRed, Green, Blue, Pal8 );

}

//***********************************************************************************************************************************
// *****

void DrawIntAtLocation(int ScrX, int ScrY, int I, int ColorRed, int Green, int Blue )
{
  sprintf(TmpStr, "%d", I);
  GrDrawString(GrBuffFor3D, SmHUDFont, ScrX, ScrY, TmpStr, ColorRed, Green, Blue);
}

void DrawFloatAtLocation(int ScrX, int ScrY, float I, int ColorRed, int Green, int Blue)
{
  if(I - floor(I) > 0)
     sprintf(TmpStr, "%d.%d", (int)I,(int)((I-floor(I))*10.0));
  else
	 sprintf(TmpStr, "%d", (int)I);

  GrDrawString(GrBuffFor3D, SmHUDFont, ScrX, ScrY, TmpStr, ColorRed, Green, Blue);
}



//***********************************************************************************************************************************
// *****

void RightJustifyTextAtLocation(int ScrX, int ScrY,  char *pMpdString, int Color, int Green, int Blue)
{
  int TextScrX;
  int Len;

  Len = GrStrLen(pMpdString, SmHUDFont);

  TextScrX = ScrX - Len;

  sprintf(TmpStr, pMpdString);

  GrDrawString(GrBuffFor3D, SmHUDFont, TextScrX, ScrY, TmpStr, Color,Green,Blue,0);

}

//***********************************************************************************************************************************
// *****

void DrawTextAtCenter(int ScrX, int ScrY, int Width, int Height, char *pMpdString, int Color, int Green, int Blue)
{
  int TempX;
  int TempY;

  TempX = (ScrX + (((ScrX + Width) - ScrX) / 2) - 40);
  TempY = (ScrY + (((ScrY + Height) - ScrY) / 2) - 10);
  sprintf(TmpStr, pMpdString);
  GrDrawString(GrBuffFor3D, SmHUDFont, TempX, TempY, TmpStr, Color,Green,Blue,0);

}

//***********************************************************************************************************************************
// *****

void DrawWordAtButton(int MpdDetectNum,int ButtonNum, char *Word, int DrawBox, int Color, int Green, int Blue)
{
  int ScrX;
  int ScrY;
  int RightOff  = 154;
  int LeftOff   = 16;
  int TopOff    = 15;
  int BottomOff = 148;
  int Height    = 5;
  int HeightDiv2 = Height/2;
  int Len;
  int LenDiv2;
  int TextScrX;
  int TextScrY;
  int Col1Off = 42;
  int Col2Off = 64;
  int Col3Off = 86;
  int Col4Off = 108;
  int Col5Off = 130;

  /*
  if( (MpdDetectNum == BACK_CENTER_RIGHT_MPD) ||	(MpdDetectNum == BACK_CENTER_LEFT_MPD) )
  {
	Col1Off  -= 3;
    Col2Off  -= 3;
   	Col3Off  -= 3;
	Col4Off  -= 3;
    Col5Off  -= 2;
	LeftOff  -= 2;
	RightOff -= 2;
	BottomOff--;
  }

  if(MpdDetectNum == BACK_RIGHT_MPD)
  {
   	Col1Off  += 2;
    Col2Off  += 2;
   	Col3Off  += 2;
	Col4Off  += 2;
    Col5Off  += 2;
	LeftOff  -= 2;
	RightOff -= 2;
	BottomOff--;
  }

  if(MpdDetectNum == BACK_LEFT_MPD)
  {
	Col1Off  -= 7;
    Col2Off  -= 7;
   	Col3Off  -= 7;
	Col4Off  -= 7;
    Col5Off  -= 2;
	LeftOff  -= 2;
	RightOff -= 2;
	BottomOff--;
  }

  */

  ScrX = MPD.MpdList[MpdDetectNum].ScrX;
  ScrY = MPD.MpdList[MpdDetectNum].ScrY;

  Len = GrStrLen(Word, SmHUDFont);
  LenDiv2 = Len/2;

  switch (ButtonNum)
  {
      case 0:
		 TextScrX = ScrX + LeftOff + 2;
		 TextScrY = ScrY + 36 - HeightDiv2;
		 break;

      case 1:
	     TextScrX = ScrX + LeftOff + 2;
		 TextScrY = ScrY + 58 - HeightDiv2;
	     break;

      case 2:
	     TextScrX = ScrX + LeftOff + 2;
		 TextScrY = ScrY + 80 - HeightDiv2;
 	     break;

      case 3:
	    TextScrX = ScrX + LeftOff + 2;
		TextScrY = ScrY + 102 - HeightDiv2;
 		break;

      case 4:
  	    TextScrX = ScrX + LeftOff + 2;
	    TextScrY = ScrY + 124 - HeightDiv2;
 	    break;

      case 5:
		TextScrX = ScrX + Col1Off - LenDiv2;
		TextScrY = ScrY + BottomOff - Height;
 	    break;

      case 6:
	   	TextScrX = ScrX + Col2Off - LenDiv2;
		TextScrY = ScrY + BottomOff - Height;
 	    break;

      case 7:
	   	TextScrX = ScrX + Col3Off - LenDiv2;
		TextScrY = ScrY + BottomOff - Height;
	    break;

      case 8:
		TextScrX = ScrX + Col4Off - LenDiv2;
		TextScrY = ScrY + BottomOff - Height;
 	    break;

      case 9:
	    TextScrX = ScrX + Col5Off - LenDiv2;
		TextScrY = ScrY + BottomOff - Height;
 	    break;

      case 10:
	    TextScrX = ScrX + RightOff - Len - 2;
		TextScrY = ScrY + 124 - HeightDiv2;
 		break;

      case 11:
	    TextScrX = ScrX + RightOff - Len - 2;
		TextScrY = ScrY + 102 - HeightDiv2;
     	break;

      case 12:
 	    TextScrX = ScrX + RightOff - Len - 2;
		TextScrY = ScrY + 80 - HeightDiv2;
		break;

      case 13:
  	    TextScrX = ScrX + RightOff - Len - 2;
		TextScrY = ScrY + 58 - HeightDiv2;
 		break;

      case 14:
   	    TextScrX = ScrX + RightOff - Len - 2;
		TextScrY = ScrY + 36 - HeightDiv2;
 		break;

      case 15:
   	    TextScrX = ScrX + Col5Off - LenDiv2;
		TextScrY = ScrY + TopOff + 2;
		break;

      case 16:
		TextScrX = ScrX + Col4Off - LenDiv2;
		TextScrY = ScrY + TopOff + 2;
    	break;

      case 17:
  		TextScrX = ScrX + Col3Off - LenDiv2;
		TextScrY = ScrY + TopOff + 2;
  		break;

      case 18:
  		TextScrX = ScrX + Col2Off - LenDiv2;
		TextScrY = ScrY+TopOff + 2;
 		break;

      case 19:
		TextScrX = ScrX + Col1Off - LenDiv2;
		TextScrY = ScrY+TopOff + 2;
		break;


    }  // end switch

	DrawTextAtLocation(TextScrX, TextScrY, Word, Color);

	if(DrawBox)
	  DrawBoxAroundWord(Word, TextScrX, TextScrY,Color);

 }

//***********************************************************************************************************************************
// *****

void DrawBox(GrBuff *Buff, int X1,int Y1,int Width, int Height, int Color, int Green, int Blue)
{
  int X2;
  int Y2;

  X2 = X1 + Width;
  Y2 = Y1 + Height;

  GrDrawLine(Buff,X1,Y1,X2,Y1, Color, Green, Blue, 0);
  GrDrawLine(Buff,X2,Y1,X2,Y2, Color, Green, Blue, 0);
  GrDrawLine(Buff,X2,Y2,X1,Y2, Color, Green, Blue, 0);
  GrDrawLine(Buff,X1,Y2,X1,Y1, Color, Green, Blue, 0);
}

//***********************************************************************************************************************************
// *****

void DrawBoxAroundWord(char *Word, int ScrX, int ScrY, int Color, int Green, int Blue)
{
  int Len;
  int Height = 5;
  int X1,Y1;

  X1 = ScrX-2;
  Y1 = ScrY-2;

  Len = GrStrLen(Word, SmHUDFont);

  DrawBox(GrBuffFor3D, X1, Y1, Len + 3, Height + 4, Color, Green, Blue);

}

//***********************************************************************************************************************************
void DrawMenuSelections(int MpdDetectNum, int Color, int Green, int Blue)
{
  	DrawWordAtButton( MpdDetectNum, 0,  "ADI",		FALSE,	Color );
	DrawWordAtButton( MpdDetectNum, 1,  "A/A ARM",	FALSE,	Color );
    DrawWordAtButton( MpdDetectNum, 2,  "A/G ARM",	FALSE,	Color );
    DrawWordAtButton( MpdDetectNum, 3,  "HSI",		FALSE,	Color );
    DrawWordAtButton( MpdDetectNum, 4,  "TSD",		FALSE,	Color );
	DrawWordAtButton( MpdDetectNum, 5,  "PROG",		FALSE,	Color );
	DrawWordAtButton( MpdDetectNum, 11, "TGT IR",	FALSE,	Color );
    DrawWordAtButton( MpdDetectNum, 12, "TEWS",		FALSE,	Color );

	if( (MpdDetectNum != FRONT_DOWN_CENTER_MPD) && (MpdDetectNum != BACK_RIGHT_MPD)&& (MpdDetectNum != BACK_LEFT_MPD) )
    {
      DrawWordAtButton(MpdDetectNum, 13, "A/G RDR",	FALSE,	Color );
      DrawWordAtButton(MpdDetectNum, 14, "A/A RDR",	FALSE,	Color );
    }

    DrawWordAtButton(MpdDetectNum,	17, "ENG",	FALSE,	Color );
    DrawWordAtButton(MpdDetectNum,	18, "WPN",	FALSE,	Color );
}

//***********************************************************************************************************************************
void TurnOffAllRadars(int Mode)
{
  for(int i=0; i<MAX_UNIQUE_MPDS; i++)
  {
    if(Mpds[i].Mode == Mode)
	    Mpds[i].Mode = MAIN_MENU_MODE;
  }

}

//***********************************************************************************************************************************
// *****

int IsRadarModeOn(int Mode)
{
  for(int i=0; i<MAX_UNIQUE_MPDS; i++)
  {
    if(Mpds[i].Mode == Mode)
     return(TRUE);
  }

  return(FALSE);

}

//***********************************************************************************************************************************
// *****

void RadarErrorDelay(int MpdNum, int MpdDetectNum)
{
  int TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  if(Mpds[MpdNum].AGInfo.MMRadarError.StartTime != -1)
  {
    if(GetTickCount() - Mpds[MpdNum].AGInfo.MMRadarError.StartTime > 10000)
    {
      Mpds[MpdNum].AGInfo.MMRadarError.StartTime = -1;
	  Mpds[MpdNum].AGInfo.MMRadarError.ClickNum = 0;
    }
	else if( (Mpds[MpdNum].AGInfo.MMRadarError.ClickNum <= 2) && (GetTickCount() - Mpds[MpdNum].AGInfo.MMRadarError.StartTime < 2000) )
	{
		//ClearMpd(MpdDetectNum);
		DrawTextAtLocation(MPD.MpdList[MpdDetectNum].CenterX - 15,MPD.MpdList[MpdDetectNum].CenterY,"RADAR IN USE",TextColor);
	}
  }

  if(Mpds[MpdNum].AAInfo.MMRadarError.StartTime != -1)
  {
    if(GetTickCount() - Mpds[MpdNum].AAInfo.MMRadarError.StartTime > 10000)
    {
      Mpds[MpdNum].AAInfo.MMRadarError.StartTime = -1;
	  Mpds[MpdNum].AAInfo.MMRadarError.ClickNum = 0;
    }
	else if( (Mpds[MpdNum].AAInfo.MMRadarError.ClickNum <= 2) && (GetTickCount() - Mpds[MpdNum].AAInfo.MMRadarError.StartTime < 2000) )
	{
		//ClearMpd(MpdDetectNum);
		DrawTextAtLocation(MPD.MpdList[MpdDetectNum].CenterX - 15,MPD.MpdList[MpdDetectNum].CenterY,"RADAR IN USE",TextColor);
	}
  }

  if(Mpds[MpdNum].WeapInfo.StartTime != -1)
  {
    if(GetTickCount() - Mpds[MpdNum].WeapInfo.StartTime < 2000)
	{
	  //ClearMpd(MpdDetectNum);
	  DrawTextAtLocation(MPD.MpdList[MpdDetectNum].CenterX - 15,MPD.MpdList[MpdDetectNum].CenterY,"NO WEAPONS",TextColor);
	}
	else
      Mpds[MpdNum].WeapInfo.StartTime = -1;
  }

  if(Mpds[MpdNum].FlirInfo.StartTime != -1)
  {
    if(GetTickCount() - Mpds[MpdNum].FlirInfo.StartTime < 2000)
	{
	  //ClearMpd(MpdDetectNum);
	  DrawTextAtLocation(MPD.MpdList[MpdDetectNum].CenterX - 15,MPD.MpdList[MpdDetectNum].CenterY,"NO FLIR POD",TextColor);
	}
	else
      Mpds[MpdNum].FlirInfo.StartTime = -1;
  }

}

//***********************************************************************************************************************************
// *****

void ProcessAGRadarError(int MpdNum, int *CanGo, int ScrX, int ScrY)
{

}

//***********************************************************************************************************************************
// *****

void ProcessAARadarError(int MpdNum, int *CanGo, int ScrX, int ScrY)
{


}

//***********************************************************************************************************************************
// *****

void ProcessAGRadarClick(int MpdNum, int *CanGo)
{


}

//***********************************************************************************************************************************
// *****

void ProcessAARadarClick(int MpdNum, int *CanGo)
{

}

//***********************************************************************************************************************************
// *****

void DoAGRadarClick(int MpdNum, int MpdDetectNum, int *ModeToSet)
{

}

//***********************************************************************************************************************************
// *****

void DoAARadarClick(int MpdNum, int MpdDetectNum, int *ModeToSet)
{

}

//***********************************************************************************************************************************
// *****

int ProcessWeaponButtonClick(int MpdNum)
{
  return(0);

}

//***********************************************************************************************************************************
// *****

int ProcessFlirButtonClick(int MpdNum)
{

  int HasFlir;

  HasFlir = ( (PlayerPlane->WeapLoad[LANTIRN_L].Count > 0) && (PlayerPlane->WeapLoad[LANTIRN_L].WeapId == 46) );

  if(!HasFlir)
  {
      Mpds[MpdNum].FlirInfo.StartTime = GetTickCount();
	  return(MAIN_MENU_MODE);
  }

  return(TGT_IR_MODE);

}


//***********************************************************************************************************************************
void MainMenuModeF15(int MpdNum, int MpdDetectNum)
{


}

//***********************************************************************************************************************************
// *****

void DrawTews(int XOfs,int  YOfs,int  Width,int Height)
{
	XOfs+=15;
	YOfs+=15;
	PlaneParams *P = (PlaneParams *)Camera1.AttachedObject;
}

//***********************************************************************************************************************************
// *****

void CalcMonoCamera(GrBuff *src, GrBuff *dest)
{
   int had_to_locks = 0;
   int had_to_lockd = 0;
   unsigned char *src_buffer,*dest_buffer;
   int NumPixels;
   int *PalPtr;

	src_buffer =  GrGetBuffPointer(src,&had_to_locks);
    dest_buffer = GrGetBuffPointer(dest,&had_to_lockd);

	NumPixels = src->Width * src->Height;

	PalPtr = &(MonoPal[0]);

    _asm
	{
	  push edi
	  push esi

	  mov esi,src_buffer
	  mov edi,dest_buffer

	  mov eax,PalPtr

	  mov ecx,NumPixels
	  mov ebx,0
	  mov edx,0

PixLoop:

	  mov bl,[esi]
	  mov dl,[eax + 4*ebx]
	  mov [edi],dl
	  lea esi,[esi + 1]
	  dec ecx
	  lea edi,[edi + 1]
	  jnz PixLoop

	  pop esi
	  pop edi

	}

  if (had_to_locks)
	GrTieOffBuffPointer(src,src_buffer,had_to_locks);

 if (had_to_lockd)
	GrTieOffBuffPointer(dest,dest_buffer,had_to_lockd);

}

//***********************************************************************************************************************************
// *****

void ProcessWeaponMode(int MpdNum)
{
  int LeftSideHasMaverick  = IsAGMissleAvailableOnGivenSide(MAVERIC_TYPE,LEFT);
  int RightSideHasMaverick = IsAGMissleAvailableOnGivenSide(MAVERIC_TYPE,RIGHT);
  int LeftSideHasGbu       = IsAGMissleAvailableOnGivenSide(GBU15_TYPE,LEFT);
  int RightSideHasGbu      = IsAGMissleAvailableOnGivenSide(GBU15_TYPE,RIGHT);

  if(LeftSideHasMaverick || RightSideHasMaverick)
  {
     Mpds[MpdNum].WeapInfo.ModeToUse = MAVERIC_TYPE;
     GbuInIndirectMode = FALSE;
  }
  else if(LeftSideHasGbu || RightSideHasGbu || GbuInIndirectMode)
     Mpds[MpdNum].WeapInfo.ModeToUse = GBU15_TYPE;
  else
  {
     Mpds[MpdNum].WeapInfo.ModeToUse = MAVERIC_TYPE;
     GbuInIndirectMode = FALSE;
  }
}

//***********************************************************************************************************************************
// *****

void SetMpdModes()
{


}

//***********************************************************************************************************************************
// *****

void DrawProgramChoices(int MpdNum, int ScrX, int ScrY, int Color, int Green, int Blue)
{

}

//***********************************************************************************************************************************
// *****

void ProgramMode(int MpdNum, int MpdDetectNum)
{

}


//***********************************************************************************************************************************
// *****

char *GetArmState(int Station, int *BoxIt)
{
  char *RetStr;

  *BoxIt = FALSE;

  switch (PlayerPlane->WeapLoad[Station].Status)
  {
     case MRM_STAT:
		RetStr = "MRM";
		break;

     case SRM_STAT:
		RetStr = "SRM";
		break;

	 case RDY_STAT:
		RetStr = "RDY";
		*BoxIt = ((UFC.MasterArmState == ON) && (UFC.MasterMode != AG_MODE));
		break;

	 case STBY_STAT:
		RetStr = "STBY";
		break;

     default:
		RetStr = "";
  }

  return(RetStr);

}

//***********************************************************************************************************************************
// *****

char *GetAbbrev(int Station, int UseAirToAir)
{
  char *RetStr;
  int Index;

  Index = PlayerPlane->WeapLoad[Station].WeapIndex;

  if(!UseAirToAir)
  {
     if(PlayerPlane->WeapLoad[Station].Status == AIR_GROUND_STAT)
	  RetStr = pDBWeaponList[Index].sMPDAbbrev;
     else
	  RetStr = "";
  }
  else
  {
	 if( (PlayerPlane->WeapLoad[Station].Status != NO_WEAP_STAT) &&
	     (PlayerPlane->WeapLoad[Station].Status != AIR_GROUND_STAT)        )
	  RetStr = pDBWeaponList[Index].sMPDAbbrev;
     else
	  RetStr = "";
  }

   return(RetStr);
}

//***********************************************************************************************************************************
// *****

void ToggleAim9Scan()
{
  AAScanMode = !AAScanMode;
}

//***********************************************************************************************************************************
// *****
int IsAGWeaponSelected(void)
{
	int RetVal = FALSE;
	for (int i=0;i<MAX_F18E_STATIONS;i++)
	{
		  if(BombStations[i]==TRUE)
		  {
			  RetVal = TRUE;
			  break;
		  }
	}
	return(RetVal);
}

//***********************************************************************************************************************************
// *****

void InitAGArm()
{

  for (int i=0;i<MAX_F18E_STATIONS;i++)
  {
	BombStations[i] = FALSE;
  }

  GunRate = HIGH;

  BombMethod = SNGL_BMB;
  BombInterval = 320;
  NumRplBmbs = 1;
}

//***********************************************************************************************************************************
// *****

void DrawAgArm(int MpdNum, int MpdDetectNum)
{
  int CenterX,CenterY;
  int P1x,P1y,P2x,P2y,P3x,P3y;
  int LineColor;
  int TextColor;
  char *Str;
  int ShortCnt,MedCnt;

  TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  LineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  CenterX = MPD.MpdList[MpdDetectNum].CenterX;
  CenterY =	MPD.MpdList[MpdDetectNum].CenterY;

	//*****************************************************************************************************************************************
	// JLM F18 TEMP  -- *Header -- (search "*Header" to step through each section of file)
	//*****************************************************************************************************************************************
	/*
	P1x = CenterX	- 45; P1y = CenterY - 45;

	AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	AvWeapEntryType *Walk;

	Walk = FirstW;
	while(Walk <= LastW)
	{
		char *Str = pDBWeaponList[Walk->W->WeapIndex].sMPDAbbrev;
		DrawTextAtLocation(P1x,P1y,Str,TextColor);
		if(Walk->Selected)
		 DrawBoxAroundWord(Str,P1x,P1y,TextColor);
		P1y += 10;
		Walk++;
	}
	*/
	//*****************************************************************************************************************************************
	// JLM F18 TEMP  -- *Header -- (search "*Header" to step through each section of file)
	//*****************************************************************************************************************************************


  P1x = CenterX	- 55; P1y = CenterY - 59;
  Str = GetAbbrev(AG_LEFT,FALSE);
  DrawTextAtLocation(P1x,P1y,Str,TextColor);
  if(PlayerPlane->WeapLoad[AG_LEFT].Status == AG_STAT)
      DrawIntAtLocation(P1x + 10, P1y + 8,PlayerPlane->WeapLoad[AG_LEFT].Count,TextColor);
  if(BombStations[AG_LEFT])
    DrawBoxAroundWord(Str,P1x,P1y,TextColor);

  P1x = CenterX	- 31; P1y = CenterY - 59;
  Str = GetAbbrev(FRONT_L,FALSE);
  DrawTextAtLocation(P1x,P1y,Str,TextColor);
  if(PlayerPlane->WeapLoad[FRONT_L].Status == AG_STAT)
    DrawIntAtLocation(P1x + 10, P1y + 8,PlayerPlane->WeapLoad[FRONT_L].Count,TextColor);
  if(BombStations[FRONT_L])
     DrawBoxAroundWord(Str,P1x,P1y,TextColor);

  P1x = CenterX	- 7; P1y = CenterY  - 59;
  Str = GetAbbrev(AG_CENTER,FALSE);
  DrawTextAtLocation(P1x,P1y,Str,TextColor);
  if(PlayerPlane->WeapLoad[AG_CENTER].Status == AG_STAT)
    DrawIntAtLocation(P1x + 10, P1y + 8,PlayerPlane->WeapLoad[AG_CENTER].Count,TextColor);
  if(BombStations[AG_CENTER])
     DrawBoxAroundWord(Str,P1x,P1y,TextColor);

  P1x = CenterX	+ 17; P1y = CenterY - 59;
  Str = GetAbbrev(FRONT_R,FALSE);
  DrawTextAtLocation(P1x,P1y,Str,TextColor);
  if(PlayerPlane->WeapLoad[FRONT_R].Status == AG_STAT)
    DrawIntAtLocation(P1x + 10, P1y + 8,PlayerPlane->WeapLoad[FRONT_R].Count,TextColor);
  if(BombStations[FRONT_R])
     DrawBoxAroundWord(Str,P1x,P1y,TextColor);

  P1x = CenterX	+ 41; P1y = CenterY - 59;
  Str = GetAbbrev(AG_RIGHT,FALSE);
  DrawTextAtLocation(P1x,P1y,Str,TextColor);
  if(PlayerPlane->WeapLoad[AG_RIGHT].Status == AG_STAT)
    DrawIntAtLocation(P1x + 10, P1y + 8,PlayerPlane->WeapLoad[AG_RIGHT].Count,TextColor);
  if(BombStations[AG_RIGHT])
     DrawBoxAroundWord(Str,P1x,P1y,TextColor);

  P1x = CenterX	- 66; P1y = CenterY - 25;
  P2x = CenterX - 66; P2y = CenterY - 17;

  if(GunRate == HIGH)
	Str = "HIGH";
  else
	Str = "LOW";

  DrawTextAtLocation(P1x,P1y,Str,TextColor);
  DrawIntAtLocation(P2x,P2y,PlayerPlane->WeapLoad[GUN_STATION].Count,TextColor);

  P1x = CenterX	- 52; P1y = CenterY + 66;
  DrawTextAtLocation(P1x,P1y,"1/STA",TextColor);
  if(BombMethod == SNGL_BMB)
    DrawBoxAroundWord("1/STA",P1x,P1y,TextColor);

  P1x = CenterX	- 29; P1y = CenterY + 66;
  DrawTextAtLocation(P1x,P1y,"RP'SGL",TextColor);
  if(BombMethod == SNGL_RPL_BMB)
	  DrawBoxAroundWord("RP'SGL",P1x,P1y,TextColor);

  P1x = CenterX	- 4;  P1y = CenterY + 66;
  DrawTextAtLocation(P1x,P1y,"RP'MPL",TextColor);
  if(BombMethod == MLT_RPL_BMB)
	  DrawBoxAroundWord("RP'MPL",P1x,P1y,TextColor);

  P1x = CenterX	- 64; P1y = CenterY + 30;
  DrawTextAtLocation(P1x,P1y,"QTY",TextColor);

  P1x = CenterX	- 50; P1y = CenterY + 30;
  if(NumRplBmbs >= 10)
   DrawIntAtLocation(P1x,P1y,NumRplBmbs,TextColor);
  else
  {
   	sprintf(TmpStr, "0%d", NumRplBmbs);
 	DrawTextAtLocation(P1x,P1y,TmpStr,TextColor);
  }

  P1x = CenterX	- 64; P1y = CenterY + 45;
  switch(UFC.AGSubMode)
  {
    case CDIP_AG:	   DrawTextAtLocation(P1x,P1y,"CDIP",TextColor);      break;
    case AUTO_AG:      DrawTextAtLocation(P1x,P1y,"AUTO",TextColor);      break;
    case AUTO_LOFT_AG: DrawTextAtLocation(P1x,P1y,"AUTO LOFT",TextColor); break;
    case GUNS_AG:      DrawTextAtLocation(P1x,P1y,"GUN",TextColor);       break;
  }

  P1x = CenterX	- 68; P1y = CenterY + 3;
  P2x = CenterX	- 45; P2y = CenterY + 3;
  P3x = CenterX	- 45; P3y = CenterY + 28;

  GrDrawLine(GrBuffFor3D, P1x, P1y, P2x, P2y, LineColor);
  GrDrawLine(GrBuffFor3D, P2x, P2y, P3x, P3y, LineColor);

  P1x = CenterX	- 68; P1y = CenterY + 25;
  P2x = CenterX	- 49; P2y = CenterY + 25;
  P3x = CenterX	- 49; P3y = CenterY + 28;

  GrDrawLine(GrBuffFor3D, P1x, P1y, P2x, P2y, LineColor);
  GrDrawLine(GrBuffFor3D, P2x, P2y, P3x, P3y, LineColor);

  P1x = CenterX	+ 26; P1y = CenterY + 30;
  DrawTextAtLocation(P1x,P1y,"INTVL",TextColor);

  P1x = CenterX	+ 45; P1y = CenterY + 30;
  if(BombInterval >= 100)
   DrawIntAtLocation(P1x,P1y,BombInterval,TextColor);
  else
  {
  	if(BombInterval <= 0)
	  sprintf(TmpStr, "000", BombInterval);
	else
  	 sprintf(TmpStr, "0%d", BombInterval);

 	DrawTextAtLocation(P1x,P1y,TmpStr,TextColor);
  }

  P1x = CenterX + 68; P1y = CenterY + 3;
  P2x = CenterX	+ 46; P2y = CenterY + 3;
  P3x = CenterX	+ 46; P3y = CenterY + 28;

  GrDrawLine(GrBuffFor3D, P1x, P1y, P2x, P2y, LineColor);
  GrDrawLine(GrBuffFor3D, P2x, P2y, P3x, P3y, LineColor);

  P1x = CenterX	+ 68; P1y = CenterY + 25;
  P2x = CenterX	+ 50; P2y = CenterY + 25;
  P3x = CenterX	+ 50; P3y = CenterY + 28;

  GrDrawLine(GrBuffFor3D, P1x, P1y, P2x, P2y, LineColor);
  GrDrawLine(GrBuffFor3D, P2x, P2y, P3x, P3y, LineColor);

  GetAAWeaponCount(&ShortCnt, &MedCnt);
  if(ShortCnt > 0)
  {
    P1x = CenterX - 8; P1y = CenterY - 20;
    DrawIntAtLocation(P1x,P1y,ShortCnt,TextColor);
	P1x = CenterX + 2;
	DrawTextAtLocation(P1x,P1y,"SRM",TextColor);
  }

  if(MedCnt > 0)
  {
	P1x = CenterX - 8; P1y = CenterY - 13;
    DrawIntAtLocation(P1x,P1y,MedCnt,TextColor);
	P1x = CenterX + 2;
	DrawTextAtLocation(P1x,P1y,"MRM",TextColor);
  }

  P1x = CenterX + 41; P1y = CenterY - 25;
  DrawTextAtLocation(P1x,P1y,"CBU HOB",TextColor);
  P1x = CenterX + 50; P1y = CenterY - 17;


	sprintf(TmpStr,"%d",CbuDetonateAlt[CbuDetonateAltIndex]);
  DrawTextAtLocation(P1x,P1y,TmpStr,TextColor);

  P1x = CenterX	+ 40; P1y = CenterY + 55;
  DrawTextAtLocation(P1x,P1y,"RDY",TextColor);

  if( (UFC.MasterArmState == ON) && (UFC.MasterMode == AG_MODE) && (UFC.AGSubMode != GUNS_AG))
    DrawBoxAroundWord("RDY",P1x,P1y,TextColor);

}

//***********************************************************************************************************************************
// *****

void AaArmMode(int MpdNum, int MpdDetectNum)
{

}

//***********************************************************************************************************************************
// *****
void BumpMpdVal( int *pVal, int vInc, int vMin, int vMax )
{
	// If SHIFT key down, decrement, otherwise increment.
	//
	if (GetAsyncKeyState(VK_SHIFT) < 0)  vInc = -vInc;

	*pVal += vInc;

	if (*pVal > vMax)  *pVal = vMin;
	if (*pVal < vMin)  *pVal = vMax;
}

//***********************************************************************************************************************************
// ***** AG ARM KEY COMMANDS

void AGArmSelectStation(int Station)
{

  if(BombStations[Station] == TRUE)
	BombStations[Station] = FALSE;
  else
  {
	if(PlayerPlane->WeapLoad[Station].Status == AG_STAT)
	{
	   if( CanSelectBombStation(Station) )
		  BombStations[Station] = TRUE;
	 }
   }
}

void StepThroughBombMode()
{
   UFC.AGSubMode++;
   if(UFC.AGSubMode > AUTO_LOFT_AG)
      UFC.AGSubMode = CDIP_AG;
}

void ToggleGunRate()
{
  GunRate = !GunRate;
  PlayerPlane->GunFireRate = ((GunRate == HIGH) ? 1 : 2);
}

//***********************************************************************************************************************************
// *****

float NormDegree(float Angle)
{
  float RetAngle;

  RetAngle = Angle;

  if(RetAngle < 0.0)
	RetAngle = 360.0f + RetAngle;

  if(RetAngle >= 360.0)
	RetAngle = RetAngle - 360.0f;

  return(RetAngle);

}

//***********************************************************************************************************************************
// *****

void AngleDiff(float Direction, float GoalDir, float *AngleDiff, int *Dir)
{
   float DirToGoal;
   float GoalToDir;

   GoalToDir = NormDegree(360.0 - GoalDir + Direction);
   DirToGoal = NormDegree(360.0 - Direction + GoalDir);

   if(GoalToDir < DirToGoal)
   {
	  *Dir = RIGHT;
	  *AngleDiff = GoalToDir;
   }
   else
   {
	  *Dir = LEFT;
	  *AngleDiff = DirToGoal;
   }

}

//***********************************************************************************************************************************
// *****

void DrawTacan(int ScrX, int ScrY, int Color, int Green, int Blue)
{
   int P1x,P1y, P2x, P2y, P3x,P3y;

   GrDrawDotNoClip(GrBuffFor3D, ScrX, ScrY, Color);

   P1x = ScrX;      P1y = ScrY - 3;
   P2x = ScrX - 3;	P2y = ScrY + 3;
   P3x = ScrX + 3;  P3y = ScrY + 3;

   GrDrawLine(GrBuffFor3D, P1x,P1y, P2x,P2y, Color);
   GrDrawLine(GrBuffFor3D, P1x,P1y, P3x,P3y, Color);
   GrDrawLine(GrBuffFor3D, P2x,P2y, P3x,P3y, Color);

}

//***********************************************************************************************************************************
// *****

void DrawBullsEye(int ScrX, int ScrY, int Color, int Green, int Blue)
{
  GrDrawDot(GrBuffFor3D, ScrX, ScrY, Color);
  DrawCircleClip(GrBuffFor3D, ScrX, ScrY, 3, Color);
}

//***********************************************************************************************************************************
// *****

void DrawWaypoint(int ScrX, int ScrY, int Color, int Green, int Blue)
{
  GrDrawDotNoClip(GrBuffFor3D, ScrX, ScrY, Color);

  DrawCircleNoClip(GrBuffFor3D, ScrX, ScrY, 3, Color);

}

//***********************************************************************************************************************************
// *****

void DrawDot(int ScrX, int ScrY, int LineColor, int Green, int Blue)
{
  GrDrawDot(GrBuffFor3D, ScrX, ScrY, LineColor);
  GrDrawDot(GrBuffFor3D, ScrX+1, ScrY, LineColor);
  GrDrawDot(GrBuffFor3D, ScrX+1, ScrY+1, LineColor);
  GrDrawDot(GrBuffFor3D, ScrX, ScrY+1, LineColor);
}

//***********************************************************************************************************************************
// *****

void DrawErrorDots(int CenterX, int CenterY, float Direction, int LineColor, int Green, int Blue)
{
  int Px, Py;
  int Radius = 16;
  int Radius2 = 32;
  float NormDir;

  NormDir = NormDegree(Direction + 90.0);

  Px = CenterX + ICosTimes((NormDir*DEGREE), Radius);
  Py = CenterY - ISinTimes((NormDir*DEGREE), Radius);

  DrawDot(Px, Py, LineColor);

  Px = CenterX + ICosTimes((NormDir*DEGREE), Radius2);
  Py = CenterY - ISinTimes((NormDir*DEGREE), Radius2);

  DrawDot(Px, Py, LineColor);

  NormDir = NormDegree(Direction - 90.0);

  Px = CenterX + ICosTimes((NormDir*DEGREE), Radius);
  Py = CenterY - ISinTimes((NormDir*DEGREE), Radius);

  DrawDot(Px, Py, LineColor);

  Px = CenterX + ICosTimes((NormDir*DEGREE), Radius2);
  Py = CenterY - ISinTimes((NormDir*DEGREE), Radius2);

  DrawDot(Px, Py, LineColor);

}

//***********************************************************************************************************************************
// *****

void DrawBearingNeedle(int CenterX, int CenterY, float Direction, float GoalDirection, int LineColor, int Green, int Blue)
{
  int P1x, P1y, P2x, P2y,P3x, P3y, P4x, P4y,P5x,P5y,P6x,P6y,P7x,P7y;
  int Diameter = 86;
  int Radius = 40;
  int Radius2 = 35;
  int Radius3 = 25;
  float NormDir;
  float OppDir;
  float DeltaAngle;
  float NormAngle;
  float ErrorFrac;
  int ErrorDistance;
  int Dir;

  AngleDiff(Direction,GoalDirection, &DeltaAngle, &Dir);
  if(Dir == LEFT)
	NormAngle = -90.0;
  else
	NormAngle = 90.0;

  ErrorFrac = DeltaAngle/3.0;

  if(ErrorFrac > 1.0)
    ErrorFrac = 1.0;

  ErrorDistance = ErrorFrac*32.0;

  P1x = CenterX + ICosTimes((Direction*DEGREE), Radius);
  P1y = CenterY - ISinTimes((Direction*DEGREE), Radius);

  P2x = CenterX + ICosTimes((Direction*DEGREE), Radius2);
  P2y = CenterY - ISinTimes((Direction*DEGREE), Radius2);

  NormDir = NormDegree(Direction + 90.0);

  P3x = P2x + ICosTimes((NormDir*DEGREE), 3);
  P3y = P2y - ISinTimes((NormDir*DEGREE), 3);

  NormDir = NormDegree(Direction - 90.0);

  P4x = P2x + ICosTimes((NormDir*DEGREE), 3);
  P4y = P2y - ISinTimes((NormDir*DEGREE), 3);

  P5x = CenterX + ICosTimes((Direction*DEGREE), Radius3);
  P5y = CenterY - ISinTimes((Direction*DEGREE), Radius3);

  NormDir = NormDegree(Direction + NormAngle);
  P6x =	P5x + ICosTimes((NormDir*DEGREE), ErrorDistance);
  P6y = P5y - ISinTimes((NormDir*DEGREE), ErrorDistance);


  GrDrawLine(GrBuffFor3D, P1x,P1y, P3x,P3y, LineColor);
  GrDrawLine(GrBuffFor3D, P1x,P1y, P4x,P4y, LineColor);
  GrDrawLine(GrBuffFor3D, P3x,P3y, P4x,P4y, LineColor);
  GrDrawLine(GrBuffFor3D, P5x,P5y, P2x,P2y, LineColor);

  OppDir = NormDegree(Direction + 180.0);

  P1x = CenterX + ICosTimes((OppDir*DEGREE), Radius);
  P1y = CenterY - ISinTimes((OppDir*DEGREE), Radius);

  P2x = CenterX + ICosTimes((OppDir*DEGREE), Radius3);
  P2y = CenterY - ISinTimes((OppDir*DEGREE), Radius3);

  GrDrawLine(GrBuffFor3D, P1x,P1y, P2x,P2y, LineColor);

  NormDir = NormDegree(Direction + NormAngle);
  P7x =	P2x + ICosTimes((NormDir*DEGREE), ErrorDistance);
  P7y = P2y - ISinTimes((NormDir*DEGREE), ErrorDistance);

  GrDrawLine(GrBuffFor3D, P6x,P6y, P7x,P7y, LineColor);

}

//***********************************************************************************************************************************
// *****

void DrawPlaneSymbol(int CenterX, int CenterY, int LineColor, int Green, int Blue)
{

  GrDrawLine(GrBuffFor3D, CenterX, CenterY - 3,
	                       CenterX, CenterY + 5, LineColor);
  GrDrawLine(GrBuffFor3D, CenterX - 3, CenterY,
	                       CenterX + 3, CenterY, LineColor);
  GrDrawLine(GrBuffFor3D, CenterX - 1, CenterY + 4,
	                       CenterX + 1, CenterY + 4, LineColor);
}

//***********************************************************************************************************************************
// *****

void DisplayNavInfo(int CenterX, int CenterY, int Color, int Green, int Blue)
{
  int P1x,P1y;
  float TmpBearing;

  P1x = CenterX	+ 23; P1y = CenterY + 45;
  sprintf(TmpStr,"SP  %dA",UFC.DataCurrSP);
  DrawTextAtLocation(P1x,P1y,TmpStr, Color);

  P1x = CenterX + 23; P1y = CenterY + 51;
  TmpBearing = UFC.DataCurrSPMilBrg;
  sprintf(TmpStr,"%3d/%3.1f",(int)TmpBearing,UFC.DataCurrSPDistFeet*FTTONM);
  DrawTextAtLocation(P1x,P1y,TmpStr,Color);

  if (UFC.DataCurrSPTime == ETE)
		sprintf(TmpStr,"%02d:%02d:%02d",  UFC.DataCurrSPEteHr,
											   UFC.DataCurrSPEteMin,
											   UFC.DataCurrSPEteSec);
  else
		sprintf(TmpStr,"%02d:%02d:%02d",  UFC.DataCurrSPEtaHr,
											   UFC.DataCurrSPEtaMin,
											   UFC.DataCurrSPEtaSec);

  P1x = CenterX + 23; P1y = CenterY + 57;
  DrawTextAtLocation(P1x,P1y,TmpStr, Color);

  P1x = CenterX + 51; P1y = CenterY + 57;
  DrawTextAtLocation(P1x,P1y,"E", Color);

}

//***********************************************************************************************************************************
// *****

void DisplayTacanInfo(int CenterX, int CenterY, int Color, int Green, int Blue)
{
  int P1x,P1y;

  if ((!g_iNumTACANObjects) || (!UFC.CurrentTacanPtr)) return;

  P1x = CenterX	- 57; P1y = CenterY + 45;

  strcpy(TmpStr,"TCN ");
  strcat(TmpStr, UFC.CurrentTacanStation);
  strcat(TmpStr, ((UFC.CurrentTacanPtr->iType == AIRCRAFT) ? "X" : "Y"));

  DrawTextAtLocation(P1x,P1y,TmpStr, Color);

  P1x = CenterX - 57; P1y = CenterY + 51;

  sprintf(TmpStr,"%3d/%3.1f",(int)UFC.TacanMilBearing,UFC.TacanDistFeet*FTTONM);
  DrawTextAtLocation(P1x,P1y,TmpStr,Color);

  if (UFC.TacanEte == ETE)
		sprintf(TmpStr,"%02d:%02d:%02d E",  UFC.TacanEteHr,
										    UFC.TacanEteMin,
										    UFC.TacanEteSec);
  else
		sprintf(TmpStr,"%02d:%02d:%02d A",  UFC.TacanEtaHr,
										    UFC.TacanEtaMin,
										    UFC.TacanEtaSec);

  P1x = CenterX - 57; P1y = CenterY + 57;
  DrawTextAtLocation(P1x,P1y,TmpStr, Color);
}

//***********************************************************************************************************************************
// *****

void DrawObject(int   CenterX, int   CenterY, int   Radius, float Range,  float Distance,
				float Bearing, float WorldX,  float WorldY, int   Symbol, int   Color)
{
  float FtPerPixel;
  float RangeInFt;
  int   EndX, EndY;
  float Heading,RelBearing;

  RangeInFt = Range*NMTOFT;

  if( Distance > RangeInFt )
	return;

  FtPerPixel = RangeInFt/Radius;
  Heading    = NormDegree( (PlayerPlane->Heading/DEGREE));
  RelBearing = NormDegree(NormDegree(NormDegree(-Heading) + Bearing) + 90.0);

  EndX = CenterX + ((ICosTimes((RelBearing*DEGREE), Distance))/FtPerPixel);
  EndY = CenterY - ((ISinTimes((RelBearing*DEGREE), Distance))/FtPerPixel);

  if(Symbol == 1)
    DrawRadarSymbol(EndX, EndY, TACAN_SYMBOL,Color,GrBuffFor3D); // draw tacan
  else
    DrawWaypoint(EndX, EndY, Color);                 // draw waypoint

}

//***********************************************************************************************************************************
// *****

void DrawHsi(int MpdNum, int MpdDetectNum)
{
  int CenterX;
  int CenterY;
  float Angle;
  int TotalLength = 48;
  int LongLength = 43;
  int ShortLength = 46;
  int End1X,End1Y,End2X,End2Y;
  int LineColor, TextColor, SymbColor;
  int LongDash;
  float TotalAngle;
  float EAngle,NAngle,SAngle,WAngle;
  float Heading,RelHeading,PreciseHeading;
  int DrawDash;
  int P1x,P1y;
  int TempCenterX, TempCenterY;
  int Diameter = 86;              // JLM make a constant for other modes to use

  Heading = PlayerPlane->Heading/DEGREE;

  PreciseHeading = PlayerPlane->Heading*100;
  PreciseHeading = PreciseHeading/DEGREE;
  PreciseHeading = PreciseHeading/100.0;

  NAngle = NormDegree(360.0 - Heading + 90.0);
  SAngle = NormDegree(NAngle + 180.0);
  EAngle = NormDegree(NAngle + 270.0);
  WAngle = NormDegree(NAngle + 90.0);

  LineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

  CenterX = MPD.MpdList[MpdDetectNum].CenterX;
  CenterY = MPD.MpdList[MpdDetectNum].CenterY;

  TempCenterX = CenterX - 5;
  TempCenterY = CenterY - 3;

  Angle = NAngle;
  LongDash = TRUE;
  TotalAngle = 0.0;

  while(TotalAngle < 360.0)
  {

	End1X = TempCenterX + ICosTimes((Angle*DEGREE), TotalLength);
    End1Y = TempCenterY - ISinTimes((Angle*DEGREE), TotalLength);

    if(LongDash)
    {
      End2X = TempCenterX + ICosTimes((Angle*DEGREE), LongLength);
      End2Y = TempCenterY - ISinTimes((Angle*DEGREE), LongLength);
    }
    else
    {
      End2X = TempCenterX + ICosTimes((Angle*DEGREE), ShortLength);
      End2Y = TempCenterY - ISinTimes((Angle*DEGREE), ShortLength);
    }

    DrawDash = TRUE;

	if(Angle == NAngle)
	{
	  DrawDash= FALSE;
	  DrawTextAtLocation(End1X-2,End1Y-2,"N",TextColor);
	}

	if(Angle == SAngle)
	{
	  DrawDash = FALSE;
	  DrawTextAtLocation(End1X-2,End1Y-2,"S",TextColor);
	}

    if(Angle == EAngle)
	{
	  DrawDash = FALSE;
	  DrawTextAtLocation(End1X-2,End1Y-2,"E",TextColor);
	}

	if(Angle == WAngle)
	{
	  DrawDash = FALSE;
	  DrawTextAtLocation(End1X-2,End1Y-2,"W",TextColor);
	}

	if(DrawDash)
	 GrDrawLine(GrBuffFor3D, End1X, End1Y, End2X, End2Y, LineColor);

	LongDash = !LongDash;
	TotalAngle += 10.0;
	Angle = NormDegree(Angle + 10.0);

  }

  DrawPlaneSymbol(TempCenterX,TempCenterY, SymbColor);

  if((Mpds[MpdNum].HsiInfo.Mode == HSI_TACAN))
  {
	  RelHeading = NormDegree(NormDegree(NormDegree(-PreciseHeading) + UFC.TacanBearing) + 90.0);

	  DrawBearingNeedle(TempCenterX, TempCenterY,
		                RelHeading,90.0,LineColor);

	 DrawErrorDots(TempCenterX, TempCenterY,RelHeading, LineColor);
	 DrawWordAtButton(MpdDetectNum,5,"TCN",TRUE,TextColor);
	 DrawWordAtButton(MpdDetectNum,9,"NAV",FALSE,TextColor);
	 // Draw the Current Tacan point
     DrawObject(TempCenterX, TempCenterY, LongLength, Mpds[MpdNum].HsiInfo.Range,
	            UFC.TacanDistFeet,UFC.TacanBearing, UFC.TacanPos.X,
		        UFC.TacanPos.Z, 1, SymbColor);
  }
  else	// must be in NAV mode
  {
	 DrawWordAtButton(MpdDetectNum,5,"TCN",FALSE,TextColor);
     DrawWordAtButton(MpdDetectNum,9,"NAV",TRUE,TextColor);
	 // Draw the Current Waypoint
	 DrawObject(TempCenterX, TempCenterY, LongLength, Mpds[MpdNum].HsiInfo.Range,
	   		    UFC.DataCurrSPDistFeet,UFC.DataCurrSPBrg, PlayerPlane->AI.WayPosition.X,
				PlayerPlane->AI.WayPosition.Z, 0, SymbColor);
   }

  /* done drawing waypoint    */

  P1x = CenterX	+ 44; P1y = CenterY - 61;
  DrawTextAtLocation(P1x,P1y,"+",TextColor);

  P1x = CenterX	+ 22; P1y = CenterY - 63;
  DrawTextAtLocation(P1x,P1y,"-",TextColor);

  P1x = CenterX	+ 29; P1y = CenterY - 61;
  DrawTextAtLocation(P1x,P1y,"RNG",TextColor);

  P1x = CenterX	+ 31; P1y = CenterY - 55;
  sprintf(TmpStr,"%d", Mpds[MpdNum].HsiInfo.Range);
  DrawTextAtLocation(P1x,P1y,TmpStr,TextColor);

  if(!MpdDamage[MPD_DAMAGE_CEN_COMP])             // central comp damage knocks out Nav Info
  {
    DisplayNavInfo(CenterX, CenterY, TextColor);
    DisplayTacanInfo(CenterX, CenterY, TextColor);
  }

 }

//***********************************************************************************************************************************
// *****

void HsiModeF18(int MpdNum, int MpdDetectNum)
{
  int ButtonNum;
  int Pressed;
  int TextColor;

  //PushClipRegion();
  //SetClipRegion(0, 639,0,479, 240, 320);

  //ClearMpd(MpdDetectNum);

  TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  DrawWordAtButton(MpdDetectNum,10, "M", FALSE, TextColor);

  DrawHsi(MpdNum,MpdDetectNum);

  GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

  if (Pressed)
  {
	 switch (ButtonNum)
	 {
	   case 10:
	   {
	     Mpds[MpdNum].Mode = MAIN_MENU_MODE;
		 break;
	   }
       case 15:
	   {
		 HsiIncRange();
		 break;
	   }
       case 16:
	   {
		 HsiDecRange();
		 break;
	   }
       case 5:
	   {
		if ((g_iNumTACANObjects) && (UFC.CurrentTacanPtr))
			Mpds[MpdNum].HsiInfo.Mode = HSI_TACAN;
		 break;
	   }
	   case 9:
	   {
		 Mpds[MpdNum].HsiInfo.Mode = 0;
		 break;
	   }

	 }

  }

 // PopClipRegion();

}

//***********************************************************************************************************************************
// *****

void HsiIncRange()
{
  for(int i=0; i<MAX_UNIQUE_MPDS; i++)
  {
    if(Mpds[i].HsiInfo.Range < 160)
	  Mpds[i].HsiInfo.Range <<= 1;
  }
}

//***********************************************************************************************************************************
// *****

void HsiDecRange()
{
  for(int i=0; i<MAX_UNIQUE_MPDS; i++)
  {
    if(Mpds[i].HsiInfo.Range > 10)
	  Mpds[i].HsiInfo.Range >>= 1;
  }
}

void HsiSelectTacan()
{
  if ((g_iNumTACANObjects) && (UFC.CurrentTacanPtr))
  {
	for(int i=0; i<MAX_UNIQUE_MPDS; i++)
        Mpds[i].HsiInfo.Mode = HSI_TACAN;
  }
}

void HsiSelectNAV()
{

}


//***********************************************************************************************************************************
// ***** TSD CODE

#define TSD_BOT_DISPLAY 0
#define TSD_CTR_DISPLAY 1

#define TSD_RDR_DISPLAY  0
#define TSD_FLIR_DISPLAY 1

typedef struct
{
  int DisplayMode;
  MBWayPoints *Wpt;
  int Range;
  int DeclutterOn;
  int InvertOn;
  int DisplayObj;
  float DisplayObjAzim;
  float DisplayObjRange;
  int   DisplayObjInfo;
} TsdInfoType;

TsdInfoType TsdInfo;

FPoint PlaneScreenPos;

TerCache *MovingMapMediumCache;
TerCache *MovingMapHighCache;
FPointDouble MovingMapCenter;

void InitTsd()
{
  TsdInfo.DisplayMode = TSD_BOT_DISPLAY;
  TsdInfo.DisplayObj  = TSD_RDR_DISPLAY;
  TsdInfo.Wpt = NULL;
  TsdInfo.Range = 40;
  TsdInfo.DeclutterOn = FALSE;
  TsdInfo.InvertOn = FALSE;
  TsdInfo.DisplayObjAzim = 0.0;
  TsdInfo.DisplayObjRange= 0.0;
  TsdInfo.DisplayObjInfo = FALSE;
  MovingMapMediumCache = NULL;
  MovingMapHighCache = NULL;
}

//***********************************************************************************************************************************
// *****

void ClipTsdMapToTextureBuffer(int *FinalOutVerts,FPoint *WVerts, FPoint *SVerts)
{
  FPoint WP[8];
  FPoint SP[8];
  FPoint WBuff[4];
  FPoint *WPPtr,*WVertsPtr,*WBuffPtr,*SVertsPtr;
  FPoint P1,P2,P3,P4;
  FPoint IntersectPoint,ScreenIntersectPoint;

  int IsIn = TRUE;

  WPPtr     = WP;
  WVertsPtr = WVerts;
  WBuffPtr  = WBuff;
  SVertsPtr = SVerts;

  // get buffer world coordinates
  WBuffPtr[0].X = MediumHeightCache->BLoadedULX;
  WBuffPtr[0].Z = MediumHeightCache->BLoadedULY;
  WBuffPtr[1].X = MediumHeightCache->BLoadedULX + (255*MediumHeightCache->PixelSize);
  WBuffPtr[1].Z = WBuffPtr[0].Z;
  WBuffPtr[2].X = WBuffPtr[1].X;
  WBuffPtr[2].Z = MediumHeightCache->BLoadedULY + (255*MediumHeightCache->PixelSize);
  WBuffPtr[3].X = WBuffPtr[0].X;
  WBuffPtr[3].Z = WBuffPtr[2].Z;

  double TVal;
  int Parallel;

  int Sides = 0;
  int Index = 0;
  int OutIndex=0;
  int NextOutIndex=1;
  int InIndex=0;
  int NextInIndex=1;

  while(Sides < 4)
  {
    WBuffPtr = WBuff;
	int SideCount=0;
    int NextSide=1;
	int IntersectionFound=FALSE;

	while(!IntersectionFound && (SideCount < 4) )
	{
      GetTValForLineIntersect(&WVertsPtr[InIndex],&WVertsPtr[NextInIndex],&WBuffPtr[SideCount],&WBuffPtr[NextSide],&TVal,&Parallel);
      if( (TVal >= 0.0) && (TVal <= 1.0) && !Parallel )
        IntersectionFound = TRUE;
	  SideCount++;
	  NextSide = (SideCount < 3) ? SideCount+1 : 0;
	}

	if(IntersectionFound)
	{
	  IntersectPoint.X = WVertsPtr[InIndex].X + (WVertsPtr[NextInIndex].X - WVertsPtr[InIndex].X)*TVal;
      IntersectPoint.Z = WVertsPtr[InIndex].Z + (WVertsPtr[NextInIndex].Z - WVertsPtr[InIndex].Z)*TVal;
      IntersectPoint.Y = 0.0;

	  ScreenIntersectPoint.X = SVertsPtr[InIndex].X + (SVertsPtr[NextInIndex].X - SVertsPtr[InIndex].X)*TVal;
      ScreenIntersectPoint.Z = SVertsPtr[InIndex].Z + (SVertsPtr[NextInIndex].Z - SVertsPtr[InIndex].Z)*TVal;
      ScreenIntersectPoint.Y = 0.0;

      if(IsIn)
	  {
		WP[OutIndex]     = WVertsPtr[InIndex];
        WP[NextOutIndex] = WVertsPtr[NextInIndex];
        SP[OutIndex]     = SVertsPtr[InIndex];
        SP[NextOutIndex] = ScreenIntersectPoint;
        OutIndex++;
        NextOutIndex++;

        WP[OutIndex]     = WVertsPtr[InIndex];
        WP[NextOutIndex] = IntersectPoint;
        SP[OutIndex]     = SVertsPtr[InIndex];
        SP[NextOutIndex] = ScreenIntersectPoint;
  	  }
	  else
	  {
        WP[OutIndex]     = IntersectPoint;
        WP[NextOutIndex] = WVertsPtr[InIndex];
		SP[OutIndex]     = ScreenIntersectPoint;
        SP[NextOutIndex] = SVertsPtr[InIndex];
	  }

	  IsIn = !IsIn;
	}
	else
	{
        WP[OutIndex]     = WVertsPtr[InIndex];
        WP[NextOutIndex] = WVertsPtr[NextInIndex];
		SP[OutIndex]     = SVertsPtr[InIndex];
        SP[NextOutIndex] = SVertsPtr[NextInIndex];
	}

	OutIndex++;
	NextOutIndex++;
	InIndex++;
	NextInIndex = (InIndex < 3) ? InIndex+1 : 0;
	Sides++;
  }

   *FinalOutVerts = OutIndex;

   Index = 0;
   while(Index++ < OutIndex)
   {
     WVertsPtr[Index] = WP[Index];
     SVertsPtr[Index] = SP[Index];
   }

}



//***********************************************************************************************************************************
// *****

void DrawTsdMovingMap(int MpdNum, int XOfs, int YOfs, FPoint PlaneScreenPos)
{
//  int CornerDx,CornerDyUp, CornerDyDown;
//  double Dx,Dz;
//  float Dist,Angle,NewX,NewZ;
//  FPoint BoxWP[8];
//  FPoint BoxSP[8];
//  FPoint LocToUse;
//  float HeadingToUse;
//  unsigned char tmppal[256*3];

//  if(TimeExcel != 0) return;

//  // switch Y and Z of plane pos because the compute heading func uses Z.. shouldn't be but is
//  PlaneScreenPos.Z = PlaneScreenPos.Y;
//  PlaneScreenPos.Y = 0;

//  RotPoint3D points[8];
//  RotPoint3D *Points[8];

//  CornerDx     = 53;
//  CornerDyUp   = (YOfs - 56) - PlaneScreenPos.Z;
//  CornerDyDown = (YOfs + 51) - PlaneScreenPos.Z;

//  Points[0] = &points[0];
//  Points[1] = &points[1];
//  Points[2] = &points[2];

//  Points[3] = &points[3];
//  Points[4] = &points[0];
//  Points[5] = &points[2];

//  BoxSP[0].X = PlaneScreenPos.X - CornerDx;
//  BoxSP[0].Z = PlaneScreenPos.Z + CornerDyDown;
//  BoxSP[1].X = PlaneScreenPos.X - CornerDx;
//  BoxSP[1].Z = PlaneScreenPos.Z + CornerDyUp;
//  BoxSP[2].X = PlaneScreenPos.X + CornerDx;
//  BoxSP[2].Z = PlaneScreenPos.Z + CornerDyUp;
//  BoxSP[3].X = PlaneScreenPos.X + CornerDx;
//  BoxSP[3].Z = PlaneScreenPos.Z + CornerDyDown;

//  float ULX,ULY,UVConvert;
//  float OneOverPixelSize;

//  TerCache *MedCacheToUse;
//  TerCache *HighCacheToUse;

//  if(TsdInfo.Wpt == NULL)
//  {
//    MedCacheToUse  = MediumTextureCache;
//    HighCacheToUse = HighTextureCache;
//	LocToUse.X = PlayerPlane->WorldPosition.X;
//    LocToUse.Z = PlayerPlane->WorldPosition.Z;
//	HeadingToUse = PlayerPlane->Heading/DEGREE;
//  }
//  else
//  {
//    MedCacheToUse  = MovingMapMediumCache;
//    HighCacheToUse = MovingMapHighCache;
//	LocToUse.X = (TsdInfo.Wpt->lWPx*FTTOWU);
//    LocToUse.Z = (TsdInfo.Wpt->lWPz*FTTOWU);
//	HeadingToUse = 0;
//  }

//  if(TsdInfo.Range >= 40)
//  {
//    ULX       = MedCacheToUse->BLoadedULX;
//    ULY       = MedCacheToUse->BLoadedULY;
//	UVConvert = LP_UNITS_TO_MED_UV;
//	BuffForTexture = MedCacheToUse->Buffer;
//	SET_3DFX_TEXSLOT_PTR(MedCacheToUse->BufferPtr3DFX);
//	OneOverPixelSize = MedCacheToUse->OneOverPixelSize;
//	PalForTexture = &MedCacheToUse->Pal;
//  }
//  else
//  {
//    ULX       = HighCacheToUse->BLoadedULX;
//    ULY       = HighCacheToUse->BLoadedULY;
//	UVConvert = LP_UNITS_TO_HIGH_UV;
//	BuffForTexture = HighCacheToUse->Buffer;
//	SET_3DFX_TEXSLOT_PTR(HighCacheToUse->BufferPtr3DFX);
//    OneOverPixelSize = HighCacheToUse->OneOverPixelSize;
//	PalForTexture = &HighCacheToUse->Pal;
//  }

//  for(int i=0; i<4; i++)
//  {

// 	 Dx = BoxSP[i].X - PlaneScreenPos.X;
//	 Dz = -(BoxSP[i].Z - PlaneScreenPos.Z);

//	 Dist = sqrt(Dx*Dx + Dz*Dz);

//	 Angle = ComputeHeadingToPoint(PlaneScreenPos, 0, BoxSP[i], 1);
//   	 ProjectPoint(LocToUse.X, LocToUse.Z, NormDegree(HeadingToUse + Angle),Dist*(1/PixelsPerMile(TsdInfo.Range))*NMTOWU, &NewX, &NewZ);

//	 BoxWP[i].X = NewX;
//	 BoxWP[i].Z = NewZ;
//     BoxWP[i].Y = 0;
//  }

//  int Clipped = FALSE;
//  int NumOutVerts=4;
//  if(TsdInfo.Range > 40.0)
//  {
//     ClipTsdMapToTextureBuffer(&NumOutVerts,BoxWP,BoxSP);
//     if(NumOutVerts > 4) Clipped = TRUE;
//  }

//  if(Clipped)
//  {
//    for(i=0; i<NumOutVerts; i++)
//	   Points[i] = &points[i];
//  }

//  for(i=0; i<NumOutVerts; i++)
//  {
//	 points[i].iSX = BoxSP[i].X;
//     points[i].U   = (BoxWP[i].X - ULX)*UVConvert; //*OneOverPixelSize;
//     points[i].iSY = BoxSP[i].Z;
//     points[i].V   = (BoxWP[i].Z - ULY)*UVConvert; //OneOverPixelSize;

//	 points[i].fSX = points[i].iSX;
//     points[i].fSY = points[i].iSY;
//	 points[i].oow = -1,0/ZClipPlane;
//	 points[i].ooz = 0.0;
//	 points[i].Rotated.Z = ZClipPlane;
//	 points[i].fog = 0xFF;
//	 points[i].Flags = PT3_ROTATED | PT3_PERSPECTED | PT3_OOW_SET;
//  }

//  	if(Mpds[MpdNum].IsMono)
//		_3dxlPrepareForSpecialPalette(tmppal,MONO_GREEN_SHADING_TABLE,FX_GREENPAL);
//	else
//		_3dxlPrepareForSpecialPalette(tmppal,COLOR_MPD_SHADING_TABLE,FX_MAPTSDPAL);

// 	Set3DScreenClip();

//	if (_3dxl && (GrBuffFor3D == BackSurface))
//		_3dxlStartRenderingFunctions();
//	else
//  		if (!ThreeDOutputBuffer)
//  		{
//	 		GrGrabSurface(GrBuffFor3D);
//	 		ThreeDOutputBuffer = GrGetBuffPointer(GrBuffFor3D);
//  		}

// 	ShortenFPU();


//	if (_3dxl)
//		DrawPolyNoClip(PRIMITIVE_TEXTURE_DITHER_FLAT | PRIMITIVE_TEXTURE_DONT_PERSP,NumOutVerts,&Points[0],(int)(16.0f * LIGHT_MULTER));
//	else
//	{
//		if(Clipped)
//			DrawPolyNoClip(PRIMITIVE_TEXTURE_DITHER_FLAT | PRIMITIVE_TEXTURE_DONT_PERSP,NumOutVerts,&Points[0],(int)(16.0f * LIGHT_MULTER));
//		else
//		{
//			DrawPolyNoClip(PRIMITIVE_TEXTURE_DITHER_FLAT | PRIMITIVE_TEXTURE_DONT_PERSP,3,&Points[0],(int)(16.0f * LIGHT_MULTER));
//			DrawPolyNoClip(PRIMITIVE_TEXTURE_DITHER_FLAT | PRIMITIVE_TEXTURE_DONT_PERSP,3,&Points[3],(int)(16.0f * LIGHT_MULTER));
//		}
//	}

//    RestoreFPU();

//	if (_3dxl && (GrBuffFor3D == BackSurface))
//		_3dxlStopRenderingFunctions();
//	else
//  		if (ThreeDOutputBuffer)
//  		{
//			GrReleaseSurface(GrBuffFor3D);
//			ThreeDOutputBuffer = 0;
//  		}

//	_3dxlFinishSpecialPalette(tmppal);

  //StopRenderingFunctions();

}

//***********************************************************************************************************************************
// *****

void DrawTsdRadar(int MpdNum, int ScrX, int ScrY)
{

	int P1X,P2X,P3X,P4X,P5X;
    int P1Z,P2Z,P3Z,P4Z,P5Z;
	float fP1X,fP1Z,fP2X,fP2Z,fP3X,fP3Z;
	int PixelRadius;

	int LineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

	if(!ScanBeamOn)
		return;

    PixelRadius = GetAGRange()*PixelsPerMile(TsdInfo.Range);

    ProjectPoint(ScrX, ScrY, GetAGBeamAzimLimit(),PixelRadius, &fP1X, &fP1Z);
    ProjectPoint(ScrX, ScrY, 0,PixelRadius, &fP2X, &fP2Z);
    ProjectPoint(ScrX, ScrY, -GetAGBeamAzimLimit(),PixelRadius, &fP3X, &fP3Z);

	P1X = (int)fP1X; P2X = (int)fP2X; P3X = (int)fP3X;
    P1Z = (int)fP1Z; P2Z = (int)fP2Z; P3Z = (int)fP3Z;

	P1X = fP1X; P1Z = fP1Z;
    if(GrClipLine(GrBuffFor3D, &ScrX, &ScrY, &P1X, &P1Z))
      GrDrawLine(GrBuffFor3D, ScrX, ScrY, P1X, P1Z, LineColor);

    P3X = fP3X; P3Z = fP3Z;
    if(GrClipLine(GrBuffFor3D, &P3X, &P3Z, &ScrX, &ScrY))
        GrDrawLine(GrBuffFor3D, P3X, P3Z, ScrX, ScrY, LineColor);

	if(GetAGBeamAzimLimit() > 30.0)
	{
	  P1X = fP1X; P1Z = fP1Z; P4X = fP1X; P4Z = fP2Z;
	  if(GrClipLine(GrBuffFor3D, &P1X, &P1Z, &P4X, &P4Z))
        GrDrawLine(GrBuffFor3D, P1X, P1Z, P4X, P4Z, LineColor);

      P5X = fP3X; P5Z = fP2Z; P4X = fP1X; P4Z = fP2Z;
	  if(GrClipLine(GrBuffFor3D, &P4X, &P4Z, &P5X, &P5Z))
        GrDrawLine(GrBuffFor3D, P4X, P4Z, P5X, P5Z, LineColor);

      P5X = fP3X; P5Z = fP2Z; P3X = fP3X; P3Z = fP3Z;
	  if(GrClipLine(GrBuffFor3D, &P5X, &P5Z, &P3X, &P3Z))
        GrDrawLine(GrBuffFor3D, P5X, P5Z, P3X, P3Z, LineColor);
	}
	else
	{
      P1X = fP1X; P1Z = fP1Z; P2X = fP2X; P2Z = fP2Z;
	  if(GrClipLine(GrBuffFor3D, &P1X, &P1Z, &P2X, &P2Z))
        GrDrawLine(GrBuffFor3D, P1X, P1Z, P2X, P2Z, LineColor);

      P2X = fP2X; P2Z = fP2Z; P3X = fP3X; P3Z = fP3Z;
	  if(GrClipLine(GrBuffFor3D, &P2X, &P2Z, &P3X, &P3Z))
        GrDrawLine(GrBuffFor3D, P2X, P2Z, P3X, P3Z, LineColor);
	}
 }

//***********************************************************************************************************************************
// *****

void DrawTsdText(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
  int TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  if(TsdInfo.DisplayMode == TSD_BOT_DISPLAY)
    DrawWordAtButton(MpdDetectNum,5,"BOT",FALSE,TextColor);
  else
    DrawWordAtButton(MpdDetectNum,5,"CTR",FALSE,TextColor);

  if(TsdInfo.DisplayObj == TSD_RDR_DISPLAY)
    DrawWordAtButton(MpdDetectNum,6,"RDR",FALSE,TextColor);
  else
    DrawWordAtButton(MpdDetectNum,6,"FLIR",FALSE,TextColor);

  DrawWordAtButton(MpdDetectNum,17,"DCL",TsdInfo.DeclutterOn,TextColor);

  if(TsdInfo.Wpt == NULL)
  {
    DrawWordAtButton(MpdDetectNum,16,"PP",FALSE,TextColor);

	if(TsdInfo.DisplayObjInfo)
	{
      GrDrawString(GrBuffFor3D, SmHUDFont, XOfs - 60, YOfs-63, "AZ", TextColor);
	  if( ((int) TsdInfo.DisplayObjAzim > 0) && ((int) TsdInfo.DisplayObjAzim < 180) )
	   sprintf(TmpStr,"%dL",(int) TsdInfo.DisplayObjAzim);
	  else if( ((int)TsdInfo.DisplayObjAzim < 0) && ((int)TsdInfo.DisplayObjAzim > -180) )
  	   sprintf(TmpStr,"%dR",(int) fabs(TsdInfo.DisplayObjAzim));
 	  else
       sprintf(TmpStr,"%d",(int) TsdInfo.DisplayObjAzim);

      GrDrawString(GrBuffFor3D, SmHUDFont, XOfs -45, YOfs-63, TmpStr, TextColor);

	  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs - 60, YOfs-56, "RNG", TextColor);
      sprintf(TmpStr,"%d.%d",(int) TsdInfo.DisplayObjRange, (int)( (TsdInfo.DisplayObjRange - floor(TsdInfo.DisplayObjRange))*10.0));
	  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs - 45, YOfs-56, TmpStr, TextColor);
	}
  }
  else
  {
    sprintf(TmpStr,"%d", (TsdInfo.Wpt - &AIWayPoints[PlayerPlane->AI.startwpts]));
    DrawWordAtButton(MpdDetectNum,16,TmpStr,FALSE,TextColor);
  }

  DrawRadarSymbol(XOfs+67, YOfs-23, RDR_UP_ARROW,TextColor,GrBuffFor3D);
  DrawRadarSymbol(XOfs+67, YOfs-1,  RDR_DOWN_ARROW,TextColor,GrBuffFor3D);

  sprintf(TmpStr,"%d",TsdInfo.Range);
  DrawVerticalWordAtLocation(XOfs+66, YOfs-18,TmpStr,FALSE,TextColor);

}

//***********************************************************************************************************************************
// *****

void GetPlanePos(int XOfs, int YOfs, FPoint *PlaneScreenPos)
{
  FPoint Origin;
  float PixelRadius;
  float Frac;

  Origin.X = XOfs-1;
  Origin.Y = YOfs+54;

  PixelRadius = TsdInfo.Range*PixelsPerMile(TsdInfo.Range);

  PlaneScreenPos->X = Origin.X;

  Frac = (TsdInfo.DisplayMode == TSD_BOT_DISPLAY) ? 0.25 : 0.50;

  PlaneScreenPos->Y = Origin.Y - ((PixelRadius)*(Frac));

}

//***********************************************************************************************************************************
// *****

void DrawTsdRangeMarkersAndPlane(int MpdNum, int XOfs, int YOfs, FPoint PlaneScreenPos)
{
  FPoint Origin;
  int TickX,TickY;
  int PixelRadius;
  float Frac;
  int PlaneColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
  int TextColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  int MarkerColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  Origin.X = XOfs-1;
  Origin.Y = YOfs+54;

  TickX = Origin.X - 65;

  PixelRadius = TsdInfo.Range*PixelsPerMile(TsdInfo.Range);

  Frac = (TsdInfo.DisplayMode == TSD_BOT_DISPLAY) ? 0.25 : 0.50;

  GrDrawLine(GrBuffFor3D, TickX,PlaneScreenPos.Y,TickX + 2, PlaneScreenPos.Y, MarkerColor);
  DrawIntAtLocation(TickX + 4,PlaneScreenPos.Y-3,0, TextColor);

  if(TsdInfo.DisplayMode == TSD_BOT_DISPLAY)
  {
    TickY = PlaneScreenPos.Y - PixelRadius*0.25;
    GrDrawLine(GrBuffFor3D, TickX,TickY,TickX + 2, TickY, MarkerColor);
	DrawFloatAtLocation(TickX + 4, TickY-3, TsdInfo.Range*0.25, TextColor);

    TickY = PlaneScreenPos.Y - PixelRadius*0.50;
    GrDrawLine(GrBuffFor3D, TickX,TickY,TickX + 2, TickY, MarkerColor);
	DrawFloatAtLocation(TickX + 4, TickY-3, TsdInfo.Range*0.50, TextColor);

  }
  else
  {
    TickY = PlaneScreenPos.Y - PixelRadius*0.25;
    GrDrawLine(GrBuffFor3D, TickX,TickY,TickX + 2, TickY, MarkerColor);
    DrawFloatAtLocation(TickX + 4, TickY-3, TsdInfo.Range*0.25, TextColor);

    TickY = PlaneScreenPos.Y + PixelRadius*0.25;
    GrDrawLine(GrBuffFor3D, TickX,TickY,TickX + 2, TickY, 60);
    DrawFloatAtLocation(TickX + 4, TickY-3, TsdInfo.Range*0.25, TextColor);

  }

  if(TsdInfo.Wpt == NULL)
  {
     int OverlayColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

     DrawPlaneSymbol(PlaneScreenPos.X, PlaneScreenPos.Y, PlaneColor);

	 int PlaneInMil = 360.0 - (PlayerPlane->Heading/DEGREE);
	 if(PlaneInMil >= 360.0)
		 PlaneInMil = PlaneInMil - 360.0;
	 IntTo3DigitFont((int)(PlaneInMil),TmpStr);
     DrawTextAtLocation(PlaneScreenPos.X - 5,PlaneScreenPos.Y + 7,TmpStr,OverlayColor);
  }

}

//***********************************************************************************************************************************
// *****

 void DrawTsdSymbol(int MpdNum, int XOfs, int YOfs, FPoint WPPos, int *AnchX, int *AnchY, FPoint FromLoc, int UsePlaneHeading, int SymbolId, int SetObjInfo, char *MissionStr = NULL)
 {
   FPoint BoxPos,Origin;
   float DeltaAzim;
   float NewX, NewZ,Dist;
   float HeadingToUse;
   int   SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

   if(UsePlaneHeading)
	   HeadingToUse = PlayerPlane->Heading;
   else
	   HeadingToUse = 0;

   Origin.X = XOfs;
   Origin.Y = YOfs;

   Dist = sqrt( (FromLoc.X - WPPos.X)*(FromLoc.X - WPPos.X) +
			    (FromLoc.Z - WPPos.Z)*(FromLoc.Z - WPPos.Z)   );

   Dist *= WUTONM;

   DeltaAzim = ComputeHeadingToPoint(FromLoc,HeadingToUse,WPPos, 1);
   ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(TsdInfo.Range), &NewX, &NewZ);

   if(SetObjInfo)
   {
      TsdInfo.DisplayObjRange = Dist;
	  TsdInfo.DisplayObjAzim = DeltaAzim;
   }

   if(SymbolId >= 0)     // neg numbers reserved -1 for getting screen pos no draw, -2 for drawing mission text
      DrawRadarSymbol(NewX, NewZ, SymbolId, SymbColor,GrBuffFor3D);

   if(SymbolId == -2)    // patch fix (kludge) for drawing mission text
      GrDrawStringClipped(GrBuffFor3D, SmHUDFont,NewX,NewZ,MissionStr,SymbColor);

   *AnchX = NewX;
   *AnchY = NewZ;

 }

//***********************************************************************************************************************************
// *****

void ProcessWyptInfo(MBWayPoints *Wpt, FPoint *TgtLoc, int *WyptType)
{
  BombTarget *BombPtr;
  int Index;

  *WyptType = -1;

  if(Wpt->iNumActs > 0)
  {
    Index = Wpt->iStartAct;
	while(Index < Wpt->iStartAct + Wpt->iNumActs)
    {
      if( AIActions[Index].ActionID == ACTION_BOMB_TARGET )
      {

	    BombPtr = (BombTarget *)AIActions[Wpt->iStartAct].pAction;
        TgtLoc->X = BombPtr->FlightAttackList[0].X*FTTOWU;
        TgtLoc->Y = BombPtr->FlightAttackList[0].Y*FTTOWU;
	    TgtLoc->Z = BombPtr->FlightAttackList[0].Z*FTTOWU;

		*WyptType = ACTION_BOMB_TARGET;
		return;
      }
	  else if(AIActions[Index].ActionID == ACTION_LAND)
      {
        *WyptType = ACTION_LAND;
		return;
   	  }
	  else if(AIActions[Index].ActionID == ACTION_TAKEOFF)
      {
        *WyptType = ACTION_TAKEOFF;
		return;
      }

	  Index++;

	}

  }

}

//***********************************************************************************************************************************
// *****

void DrawTsdWaypoints(int XOfs, int YOfs, int MpdNum, FPoint FromLoc, int UsePlaneHeading)
{
  int ScrX,ScrY;
  int LastX;
  int LastY;
  int TempX, TempY;
  int FirstX,FirstY;
  int WyptType, SymbId;
  FPoint WPPos, TgtLoc;
  FPointDouble BullsEyeLoc;

  MBWayPoints *Start,*Last, *Cur;

  int LineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

  // JLM PATCH FIX
  // DRAW BULLSEYE
  if( (BullsEye.x >= 0) || (BullsEye.z >= 0) )
  {
    BullsEyeLoc.X = BullsEye.x;
    BullsEyeLoc.Z = BullsEye.z;
    BullsEyeLoc.Y = 0.0;
    BullsEyeLoc *= FTTOWU;
    DrawTsdSymbol(MpdNum,XOfs,YOfs,BullsEyeLoc,&ScrX,&ScrY,FromLoc,UsePlaneHeading,BULLSEYE_SYMBOL,FALSE);
  }
  // END PATCH FIX

  Start = &AIWayPoints[PlayerPlane->AI.startwpts];
  Last  = PlayerPlane->AI.CurrWay + PlayerPlane->AI.numwaypts - 1;
  Cur   = PlayerPlane->AI.CurrWay;

  WPPos.X = Cur->lWPx*FTTOWU;
  WPPos.Z = Cur->lWPz*FTTOWU;

  ProcessWyptInfo(Cur,&TgtLoc,&WyptType);

  if(WyptType == ACTION_BOMB_TARGET)
  {
	DrawTsdSymbol(MpdNum,XOfs,YOfs,TgtLoc,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_TARGET : TARGET_SYMB,FALSE);
    DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_START_BOMB_PNT : START_BOMBING_PNT,FALSE);
  }
  else
  {
    if(WyptType == ACTION_TAKEOFF)
		SymbId = (TsdInfo.Range > 20) ? SMALL_HOME : WYPT_HOME;
	else if(WyptType == ACTION_LAND)
		SymbId = (TsdInfo.Range > 20) ? SMALL_WYPT : WYPT_CIRCLE;
	else
		SymbId = (TsdInfo.Range > 20) ? SMALL_WYPT : WYPT_CIRCLE;

    DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos, &ScrX, &ScrY, FromLoc, UsePlaneHeading, SymbId, FALSE); // Draw first
  }

  sprintf(TmpStr,"%d",Cur-Start);
  GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+1, ScrY-2, TmpStr, LineColor);

  FirstX = ScrX;
  FirstY = ScrY;

  LastX = ScrX;
  LastY = ScrY;

  if(Cur != Last) // if you are at the last wpt
    Cur++;

  while(Cur != Last)
  {

     WPPos.X = Cur->lWPx*FTTOWU;
     WPPos.Z = Cur->lWPz*FTTOWU;

	 ProcessWyptInfo(Cur,&TgtLoc,&WyptType);

	 if(WyptType == ACTION_BOMB_TARGET)
     {
      DrawTsdSymbol(MpdNum,XOfs,YOfs,TgtLoc,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_TARGET : TARGET_SYMB,FALSE);
      DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_START_BOMB_PNT : START_BOMBING_PNT,FALSE);
     }
     else
     {
      if(WyptType == ACTION_TAKEOFF)
		  SymbId = (TsdInfo.Range > 20) ? SMALL_HOME : WYPT_HOME;
	  else if(WyptType == ACTION_LAND)
		  SymbId = (TsdInfo.Range > 20) ? SMALL_HOME : WYPT_CIRCLE;
	  else
		  SymbId = (TsdInfo.Range > 20) ? SMALL_WYPT :  WYPT_CIRCLE;

      DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos, &ScrX, &ScrY, FromLoc, UsePlaneHeading, SymbId, FALSE); // Draw first
     }

     sprintf(TmpStr,"%d",Cur-Start);
     GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+1, ScrY-2, TmpStr, LineColor);

	 TempX = ScrX;  // want to preserve original
	 TempY = ScrY;
	 if(GrClipLine(GrBuffFor3D, &LastX, &LastY, &TempX, &TempY))
        GrDrawLine(GrBuffFor3D, LastX, LastY, TempX, TempY, LineColor);

	 LastX = ScrX;
	 LastY = ScrY;
	 Cur++;
  }

  WPPos.X = Cur->lWPx*FTTOWU;
  WPPos.Z = Cur->lWPz*FTTOWU;

  ProcessWyptInfo(Cur,&TgtLoc, &WyptType);

  if(WyptType == ACTION_BOMB_TARGET)
  {
    DrawTsdSymbol(MpdNum,XOfs,YOfs,TgtLoc,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_TARGET : TARGET_SYMB,FALSE);
    DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_START_BOMB_PNT : START_BOMBING_PNT,FALSE);
  }
  else
  {
    if(WyptType == ACTION_TAKEOFF)
		SymbId = (TsdInfo.Range > 20) ? SMALL_HOME : WYPT_HOME;
	else if(WyptType == ACTION_LAND)
		SymbId = (TsdInfo.Range > 20) ? SMALL_WYPT : WYPT_CIRCLE;
	else
		SymbId = (TsdInfo.Range > 20) ? SMALL_WYPT :WYPT_CIRCLE;

    DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos, &ScrX, &ScrY, FromLoc, UsePlaneHeading, SymbId, FALSE); // Draw first
  }

  sprintf(TmpStr,"%d",Cur-Start);
  GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+1, ScrY-2, TmpStr, LineColor);

  TempX = ScrX;
  TempY = ScrY;
  if(GrClipLine(GrBuffFor3D, &LastX, &LastY, &TempX, &TempY))
       GrDrawLine(GrBuffFor3D, LastX, LastY, TempX, TempY, LineColor);

  LastX = ScrX;
  LastY = ScrY;

  Cur = Start;

  while(Cur != PlayerPlane->AI.CurrWay)
  {
    WPPos.X = Cur->lWPx*FTTOWU;
    WPPos.Z = Cur->lWPz*FTTOWU;

	ProcessWyptInfo(Cur,&TgtLoc, &WyptType);

	if(WyptType == ACTION_BOMB_TARGET)
    {
      DrawTsdSymbol(MpdNum,XOfs,YOfs,TgtLoc,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_TARGET : TARGET_SYMB,FALSE);
      DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos,&ScrX,&ScrY,FromLoc,UsePlaneHeading,(TsdInfo.Range > 20) ? SMALL_START_BOMB_PNT : START_BOMBING_PNT,FALSE);
    }
    else
    {
      if(WyptType == ACTION_TAKEOFF)
		  SymbId = (TsdInfo.Range > 20)  ? SMALL_HOME : WYPT_HOME;
	  else if(WyptType == ACTION_LAND)
	    SymbId =   (TsdInfo.Range > 20)  ? SMALL_WYPT : WYPT_CIRCLE;
	  else
		  SymbId = (TsdInfo.Range > 20)  ? SMALL_WYPT : WYPT_CIRCLE;

      DrawTsdSymbol(MpdNum,XOfs,YOfs,WPPos, &ScrX, &ScrY, FromLoc, UsePlaneHeading, SymbId, FALSE); // Draw first
    }

    sprintf(TmpStr,"%d",Cur-Start);
    GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+1, ScrY-2, TmpStr, LineColor);

    TempX = ScrX;  // want to preserve original
	TempY = ScrY;
	if(GrClipLine(GrBuffFor3D, &LastX, &LastY, &TempX, &TempY))
       GrDrawLine(GrBuffFor3D, LastX, LastY, TempX, TempY, LineColor);

	LastX = ScrX;
	LastY = ScrY;

    Cur++;
  }

  if(GrClipLine(GrBuffFor3D, &LastX, &LastY, &FirstX, &FirstY))
       GrDrawLine(GrBuffFor3D, LastX, LastY, FirstX, FirstY, LineColor);

}

//***********************************************************************************************************************************
// *****

void DrawArrowToLoc(int MpdNum, int FromX,int FromY, int ToX, int ToY)
{
  int OriginalToX = ToX;
  int OriginalToY = ToY;
  int ArrowColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

  if(GrClipLine(GrBuffFor3D, &FromX, &FromY, &ToX, &ToY))
  {
	   FPoint Start,End;
	   float NewX, NewZ;
	   float DeltaAzim;

	   Start.X = ToX;
	   Start.Z = ToY;
	   Start.Y = 0;

	   End.X = FromX;
	   End.Z = FromY;
	   End.Y = 0;

       GrDrawLine(GrBuffFor3D,FromX,FromY, ToX,ToY, ArrowColor);

       if( (OriginalToX == ToX) && (OriginalToY == ToY) )
       {

	     DeltaAzim = ComputeHeadingToPoint(Start,0,End, 1);

	     ProjectPoint(ToX, ToY, NormDegree(NormDegree(DeltaAzim) + 30),5, &NewX, &NewZ);
         GrDrawLine(GrBuffFor3D,ToX,ToY,NewX, NewZ, ArrowColor);
	     ProjectPoint(ToX, ToY, NormDegree(NormDegree(DeltaAzim) - 30),5, &NewX, &NewZ);
         GrDrawLine(GrBuffFor3D,ToX,ToY, NewX, NewZ, ArrowColor);
       }
  }

}

//***********************************************************************************************************************************
// *****

void DrawFLIRSym(int MpdNum, int XOfs, int YOfs, int ScrX, int ScrY, FPoint PlanePos)
{
  FPoint MapPos;
  int SymX, SymY;

  FPointDouble FlirPos;
  int TargetObtained;
  int MapColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

  GetFlirTargetPositionF15(&TargetObtained,&FlirPos);

  if(TargetObtained)
  {
	TsdInfo.DisplayObjInfo = TRUE;
    DrawTsdSymbol(MpdNum,ScrX,ScrY,FlirPos,&SymX,&SymY,PlanePos,TRUE,-1, TRUE);
    DrawArrowToLoc(MpdNum,ScrX,ScrY,SymX,SymY);
  }
  else
  {
    TsdInfo.DisplayObjInfo = FALSE;
  }

}

//***********************************************************************************************************************************
// *****

void DrawHiResMapSym(int MpdNum,int XOfs, int YOfs, int ScrX, int ScrY, FPoint PlanePos)
{
  FPoint MapPos;
  int SymX, SymY;

  int MapColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

  if( GetHiResMapLoc(&MapPos) )
  {
    TsdInfo.DisplayObjInfo = TRUE;

    DrawTsdSymbol(MpdNum,ScrX,ScrY,MapPos,&SymX, &SymY,PlanePos,TRUE,HRM_BOX1,TRUE);

    if(GrClipLine(GrBuffFor3D, &ScrX, &ScrY, &SymX, &SymY))
       GrDrawLine(GrBuffFor3D,ScrX,ScrY, SymX,SymY, MapColor);
  }
  else
  {
    TsdInfo.DisplayObjInfo = FALSE;
  }

}

//***********************************************************************************************************************************
// *****

void DrawTsdMissionText(int MpdNum, int XOfs, int YOfs, int ScrX, int ScrY, FPoint PlanePos)
{
  int Index = g_iNumMissionLabels;
  FPointDouble Loc;
  MissionLabelType *ML = pMissionLabelList;
  int SymX,SymY;

  while(Index-- > 0)
  {
	Loc.X = ML->lX*FTTOWU;
	Loc.Z = ML->lY*FTTOWU;
	Loc.Y = 0.0;
    DrawTsdSymbol(MpdNum,ScrX,ScrY,Loc,&SymX,&SymY,PlanePos,TRUE,-2,TRUE,ML->sLabel); // kludge, -2 means draw mission text
    ML++;
  }
}

//***********************************************************************************************************************************
// *****

void DrawTsd(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
   FPoint Origin;
   FPoint SymbolPos;

   int LineColor     = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
   int SoftLineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

   Origin.X = XOfs-(116/2) + 57;
   Origin.Y = YOfs-(111/2) + 109;

   //DrawBox(GrBuffFor3D,XOfs-53,YOfs-56,108,108,SoftLineColor);

   PushClipRegion();
   SetClipRegion(XOfs-54 + 2, XOfs + (108/2) - 4 + 5,YOfs-55,
		YOfs + (111/2) - 4, YOfs, XOfs);

   GetPlanePos(XOfs + 2,YOfs,&PlaneScreenPos);

   DrawTsdMovingMap(MpdNum,XOfs,YOfs, PlaneScreenPos);

   if(!TsdInfo.DeclutterOn)
   {
     if(TsdInfo.Wpt == NULL)
     {
       DrawTsdRadar(MpdNum,PlaneScreenPos.X, PlaneScreenPos.Y);
       DrawTsdWaypoints(PlaneScreenPos.X,PlaneScreenPos.Y,MpdNum, PlayerPlane->WorldPosition, TRUE);

	   if(TsdInfo.DisplayObj == TSD_FLIR_DISPLAY)
	     DrawFLIRSym(MpdNum,XOfs, YOfs, PlaneScreenPos.X,PlaneScreenPos.Y,PlayerPlane->WorldPosition);
	   else
         DrawHiResMapSym(MpdNum,XOfs, YOfs, PlaneScreenPos.X,PlaneScreenPos.Y,PlayerPlane->WorldPosition);
     }
     else
     {
	   FPoint Waypoint;

	   Waypoint.X = TsdInfo.Wpt->lWPx*FTTOWU;
       Waypoint.Z = TsdInfo.Wpt->lWPz*FTTOWU;
       Waypoint.Y = 0;

       DrawTsdWaypoints(PlaneScreenPos.X,PlaneScreenPos.Y,MpdNum, Waypoint, FALSE);
     }
   }
   else
   {
     FPoint TgtLoc, WPPos;
	 int WyptType;
	 int ScrX,ScrY;
	 int SymbId;

	 WPPos.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
     WPPos.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;

	 ProcessWyptInfo(PlayerPlane->AI.CurrWay,&TgtLoc,&WyptType);

	 if(WyptType == ACTION_BOMB_TARGET)
     {
      DrawTsdSymbol(MpdNum,PlaneScreenPos.X,PlaneScreenPos.Y,TgtLoc,&ScrX,&ScrY,PlayerPlane->WorldPosition,TRUE,(TsdInfo.Range > 20) ? SMALL_TARGET : TARGET_SYMB,FALSE);
      DrawTsdSymbol(MpdNum,PlaneScreenPos.X,PlaneScreenPos.Y,WPPos,&ScrX,&ScrY,PlayerPlane->WorldPosition,TRUE,(TsdInfo.Range > 20) ? SMALL_START_BOMB_PNT : START_BOMBING_PNT,FALSE);
     }
	 else
     {
      if(WyptType == ACTION_TAKEOFF)
		  SymbId = (TsdInfo.Range > 20) ? SMALL_HOME : WYPT_HOME;
	  else if(WyptType == ACTION_LAND)
		  SymbId = (TsdInfo.Range > 20) ? SMALL_HOME : WYPT_CIRCLE;
	  else
		  SymbId = (TsdInfo.Range > 20) ? SMALL_WYPT :  WYPT_CIRCLE;

      DrawTsdSymbol(MpdNum,PlaneScreenPos.X,PlaneScreenPos.Y,WPPos, &ScrX, &ScrY, PlayerPlane->WorldPosition, TRUE, SymbId, FALSE); // Draw first

	 }

     sprintf(TmpStr,"%d",PlayerPlane->AI.CurrWay-&AIWayPoints[PlayerPlane->AI.startwpts]);
     GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+1, ScrY-2, TmpStr, 60);

	 if(TsdInfo.DisplayObj == TSD_FLIR_DISPLAY)
	    DrawFLIRSym(MpdNum,XOfs,YOfs,PlaneScreenPos.X,PlaneScreenPos.Y,PlayerPlane->WorldPosition);
	  else
        DrawHiResMapSym(MpdNum,XOfs,YOfs,PlaneScreenPos.X,PlaneScreenPos.Y,PlayerPlane->WorldPosition);
   }

   DrawTsdMissionText(MpdNum,XOfs,YOfs,PlaneScreenPos.X,PlaneScreenPos.Y,PlayerPlane->WorldPosition);

   DrawTsdRangeMarkersAndPlane(MpdNum,XOfs,YOfs,PlaneScreenPos);

   PopClipRegion();
   Set3DScreenClip();

}

//***********************************************************************************************************************************
// *****

void TsdMode(int MpdNum, int MpdDetectNum)
{
  int ButtonNum;
  int Pressed;
  int TextColor;

  //ClearMpd(MpdDetectNum);

  TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  PushClipRegion();
  SetClipRegion(GrBuffFor3D->ClipLeft, GrBuffFor3D->ClipRight,GrBuffFor3D->ClipTop,
		479, 480>>1, GrBuffFor3D->MidX);

  DrawTsdText(MPD.MpdList[MpdDetectNum].CenterX-1,MPD.MpdList[MpdDetectNum].CenterY+4,MpdNum,MpdDetectNum);

  //DrawCircleDebug2(20,20,MediumTextureCache->ViewSize);

  PopClipRegion();

  DrawTsd(MPD.MpdList[MpdDetectNum].CenterX - 1 + 3,MPD.MpdList[MpdDetectNum].CenterY+4+8, MpdNum, MpdDetectNum);

  DrawWordAtButton(MpdDetectNum,10, "M", FALSE, TextColor);

  GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

  if (Pressed)
  {
	 switch(ButtonNum)
	 {
	   case 10:
	   {
	     Mpds[MpdNum].Mode = MAIN_MENU_MODE;
	     break;
	   }
	   case 5:
	   {
	     TsdInfo.DisplayMode = !TsdInfo.DisplayMode;
	     break;
	   }

	   case 6:
	   {
	     TsdInfo.DisplayObj= !TsdInfo.DisplayObj;
		 break;
	   }

	   case 12:
	   {
	     TsdInfo.Range >>= 1;
	     if (TsdInfo.Range < 10)
		   TsdInfo.Range = 10;
		 break;
	   }

	   case 13:
	   {
	     TsdInfo.Range <<= 1;
	     if (TsdInfo.Range > 80)
		   TsdInfo.Range = 80;
	//	 if(TsdInfo.Range == 80)
	//	   MediumTextureCache->SetViewDist(CACHE_HGT_DIST_FAR);
	//	 else
	//	   MediumHeightCache->SetViewDist(CACHE_HGT_DIST_NORM);
		 break;
	   }

	   case 16:
	   {
	     MBWayPoints *CurWpt  = PlayerPlane->AI.CurrWay;
	     MBWayPoints *LastWpt = PlayerPlane->AI.CurrWay + PlayerPlane->AI.numwaypts - 1;

	     if(TsdInfo.Wpt == NULL)
         {
		   if(CurWpt == LastWpt)
		     TsdInfo.Wpt = &AIWayPoints[PlayerPlane->AI.startwpts];
		   else
             TsdInfo.Wpt = CurWpt+1;
         }
	     else
         {
           if(TsdInfo.Wpt == LastWpt)
		     TsdInfo.Wpt = &AIWayPoints[PlayerPlane->AI.startwpts];
		   else
             TsdInfo.Wpt++;
	     }

	     if(TsdInfo.Wpt == CurWpt)
		   TsdInfo.Wpt = NULL;

	     if(TsdInfo.Wpt != NULL)
	     {
           //if(MovingMapMediumCache == NULL)
           //  MovingMapMediumCache = MediumTextureCache->CloneCache();
           //if(MovingMapHighCache == NULL)
            // MovingMapHighCache = HighTextureCache->CloneCache();

		   MovingMapCenter.X = TsdInfo.Wpt->lWPx*FTTOWU;
           MovingMapCenter.Z = TsdInfo.Wpt->lWPz*FTTOWU;
           MovingMapCenter.Y = 0;
           MovingMapMediumCache->NotifyCache(MovingMapCenter);
           MovingMapHighCache->NotifyCache(MovingMapCenter);
	     }
   		 break;
	   }

	   case 17:
	   {
         TsdInfo.DeclutterOn = !TsdInfo.DeclutterOn;
		 break;
	   }

	   case 19:
	   {
	     Mpds[MpdNum].Mode = MAIN_MENU_MODE;

	     delete MovingMapMediumCache;
         MovingMapMediumCache = NULL;
	     delete MovingMapHighCache;
         MovingMapHighCache = NULL;
		 break;
	   }
	 }
  }

}

//***********************************************************************************************************************************
// *****

void TsdDecRange(void)
{
   TsdInfo.Range >>= 1;
   if (TsdInfo.Range < 10)
	 TsdInfo.Range = 10;
}

void TsdIncRange(void)
{
   TsdInfo.Range <<= 1;
   if (TsdInfo.Range > 80)
    TsdInfo.Range = 80;
}

//***********************************************************************************************************************************
// ***** TSD CODE END


//***********************************************************************************************************************************
// *****

void TgtIrMode(int MpdNum, int MpdDetectNum)
{
  DoFlir(MPD.MpdList[MpdDetectNum].CenterX,MPD.MpdList[MpdDetectNum].CenterY+4, MpdNum, MpdDetectNum);
}

//***********************************************************************************************************************************
// ***** TEWS CODE

DBRadarType *GetRadarPtr(long id)
{
	DBRadarType *walker = pDBRadarList;
	int i = iNumRadarList;

	if (id)
	{
		while(i-- && (walker->lRadarID != id))
			walker++;

		if (i == -1)
			walker = NULL;
	}
	else
		walker = NULL;

	return walker;
}

//***********************************************************************************************************************************
// *****

void SimTewsBlink()
{
  if(GetTickCount() - LastTewsBlinkTime > 1000)
  {
    TewsBlink = !TewsBlink;
	LastTewsBlinkTime = GetTickCount();
  }
}

//***********************************************************************************************************************************
// *****

int IsPlaneFiringMissleAtPlayer(PlaneParams *P)
{
  PlaneParams **Planes = &PlanesFiring.Planes[0];
  int Index = PlanesFiring.NumPlanes;

  while(Index-- > 0)
  {
     if(*Planes == P)
		return (TRUE);
	 Planes++;
  }

  return (FALSE);

}

//***********************************************************************************************************************************
// *****

int IsVehicleFiringMissleAtPlayer(MovingVehicleParams *V)
{
  MovingVehicleParams **Veh = &VehiclesFiring.Vehicles[0];

	int Index = VehiclesFiring.NumVehicles;
  while(Index-- > 0)
  {
    if(*Veh == V)
		  return (TRUE);
	 Veh++;
  }
  return (FALSE);
}

//***********************************************************************************************************************************
// *****

int IsSAMObjFiringMissleAtPlayer(BasicInstance *Inst)
{
  BasicInstance **Sam = &SAMObjFiring.SAMObjs[0];

  int Index = SAMObjFiring.NumSAMObjs;

  while(Index-- > 0)
  {
     if(*Sam == Inst)
		return(TRUE);
	 Sam++;
  }

  return (FALSE);

}

//***********************************************************************************************************************************
// *****

void ProcessEnemyPlanes()
{
  PlaneParams *P = &Planes[0];
  int EasyModeOn = (g_Settings.gp.nTEWS == GP_TYPE_CASUAL);

  while(P <= LastPlane)
  {
	 if( (P->Status & PL_ACTIVE) && (P != PlayerPlane) && !(P->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP)) )
	 {
	  //if( (P->AI.AirTarget == PlayerPlane) || (MultiPlayer) )
		//{
		  if( (P->AI.iAIFlags2 & AI_RADAR_PING) )
		  {

				if( AIPutPlaneInTEWSCenter(P) )
				 //if( (P->AI.iAIFlags2 & AILOCKEDUP) && (P->AI.AirTarget == PlayerPlane) )
			 {
			   if(Threats.NumThreats < TEWS_MAX_ENTITIES-1)
			   {
 			     Threats.NumThreats++;
			     CurThreat->Type    = TEWS_PLANE_THREAT;
			     CurThreat->HasLock = TRUE;
			     CurThreat->P       = P;
			     long Index = pDBAircraftList[P->AI.iPlaneIndex].lRadarID;
           CurThreat->TEWSAbbrev = (GetRadarPtr(Index))->sNTEWS;
           CurThreat->Prog       = (GetRadarPtr(Index))->iTEWSType;
					 CurThreat->Category = TEWS_PLANE_THREAT;

			     if(IsPlaneFiringMissleAtPlayer(P))
					 {
				     CurThreat->Draw = TewsBlink;
				     CurThreat->Firing = TRUE;
					 }
			     else
					 {
	           CurThreat->Draw = TRUE;
				     CurThreat->Firing = FALSE;
					 }

			     CurThreat++;
			   }
			 }
			 else
			 {
			    if(Threats.NumThreats < TEWS_MAX_ENTITIES-1)
				{
				  Threats.NumThreats++;
				  CurThreat->Draw    = TRUE;
			    CurThreat->Firing  = FALSE;
					CurThreat->Type    = TEWS_PLANE_THREAT;
				  CurThreat->HasLock = FALSE;
				  CurThreat->P       = P;
				  CurThreat->Category = TEWS_PLANE_THREAT;

			    long Index = pDBAircraftList[P->AI.iPlaneIndex].lRadarID;
				  if(Index)
				  {
            CurThreat->TEWSAbbrev = (GetRadarPtr(Index))->sNTEWS;
            CurThreat->Prog       = (GetRadarPtr(Index))->iTEWSType;
				  }
				  CurThreat++;
				}
			 } // locked up

		  }

		//}
 	 }

	P++;
  }

}

//***********************************************************************************************************************************
//*********

void ProcessMissles()
{
  WeaponParams *W = &Weapons[0];
  PlaneParams **P = &PlanesFiring.Planes[0];
  BasicInstance **S = &SAMObjFiring.SAMObjs[0];
  MovingVehicleParams **V = &VehiclesFiring.Vehicles[0];
  int EasyModeOn = FALSE;

  int WeaponIndex,Seeker;

  while(W <= LastWeapon)
  {
    if( (W->Kind == MISSILE) && (W->Flags & WEAPON_INUSE) && ( (W->iTargetType == TARGET_PLANE) || (EasyModeOn) ) )
    {
		if( ((PlaneParams *)W->pTarget == PlayerPlane) || EasyModeOn )
		{
		   WeaponIndex = AIGetWeaponIndex(W->Type->TypeNumber);
		   Seeker = pDBWeaponList[WeaponIndex].iSeekerType;

		   if( ((Seeker == 1) || (Seeker == 7)) && ((PlaneParams *)W->pTarget == PlayerPlane) )
		   {
             if(W->LauncherType == AIRCRAFT)
             {
			   if(PlanesFiring.NumPlanes < TEWS_MAX_ENTITIES - 1)
			   {
			     PlanesFiring.NumPlanes++;
			     *P = W->P;
			     P++;
			   }
			 }
			 else if(W->LauncherType == GROUNDOBJECT)
             {
               BasicInstance *Inst;
			   InfoProviderInstance *ipinst;
			   AAWeaponInstance *weapinst;
			   GDRadarData *radardat;

			   Inst = (BasicInstance *)W->Launcher;
			   if(Inst->Family == FAMILY_AAWEAPON)
			   {
					weapinst = (AAWeaponInstance *)Inst;
					radardat = (GDRadarData *)weapinst->AIDataBuf;

					if((!((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF)))) && (W->Launcher != radardat->pWRadarSite)) && radardat->pWRadarSite)
					{
						Inst = (BasicInstance *)radardat->pWRadarSite;
						ipinst = (InfoProviderInstance *)Inst;
						radardat = (GDRadarData *)ipinst->AIDataBuf;
					}
			   }
			   else
			   {
					ipinst = (InfoProviderInstance *)Inst;
					radardat = (GDRadarData *)ipinst->AIDataBuf;
			   }

					if( (Inst->Family != FAMILY_AAWEAPON) && AIPutInfoProviderInTEWSCenter(ipinst,radardat) )
					{
				    if(SAMObjFiring.NumSAMObjs < TEWS_MAX_ENTITIES - 1)
						{
					  	SAMObjFiring.NumSAMObjs++;
				 	  	*S = (BasicInstance *)Inst;
					  	S++;
						}
					}
					else if( (Inst->Family == FAMILY_AAWEAPON) && AIPutWeaponProviderInTEWSCenter(weapinst,radardat) )
					{
				    if(SAMObjFiring.NumSAMObjs < TEWS_MAX_ENTITIES - 1)
						{
					  	SAMObjFiring.NumSAMObjs++;
				 	  	*S = (BasicInstance *)Inst;
					  	S++;
						}
					}

			 }
			 else if(W->LauncherType == MOVINGVEHICLE)
			 {
				if(VehiclesFiring.NumVehicles < TEWS_MAX_ENTITIES - 1)
				{
				  VehiclesFiring.NumVehicles++;
				  *V = (MovingVehicleParams *)W->Launcher;
				  V++;
				}
			 }

			 if( (W->Flags & FINAL_PHASE) && (Seeker == 1) )
			 {
			   if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
			   {
			     Threats.NumThreats++;
			     CurThreat->Draw     = TRUE;
				   CurThreat->Firing   = FALSE;
			     CurThreat->Type     = TEWS_MISSLE_THREAT;
			     CurThreat->HasLock  = TRUE;
			     CurThreat->W        = W;
			     CurThreat->TEWSAbbrev = pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)].sNTEWS;
			     CurThreat->Prog       = pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)].iTEWSType;
					 CurThreat->Category   = TEWS_MISSLE_THREAT;
			     CurThreat++;
			   }
			 }

		   }
 		}
	}

	W++;

  }

}

extern InfoProviderInstance *AllInfoProviders;
extern AAWeaponInstance *AllAAWeapons;

//***********************************************************************************************************************************
//*********

void ProcessSAMProviders()
{
  InfoProviderInstance *Prov = &AllInfoProviders[0];
  GDRadarData *Radar;

  while(Prov)
  {
    Radar = (GDRadarData *)Prov->AIDataBuf;

	  if(Radar->lRFlags1 & GD_RADAR_PLAYER_PING)
	  {
      if( AIPutInfoProviderInTEWSCenter(Prov,Radar) )
	    //if( (Radar->lRFlags1 & GD_RADAR_LOCK) && (Radar->Target == PlayerPlane) && (Radar->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) && (!(Radar->lRFlags1 & GD_I_AM_DEAD)))
	    {
          if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
		  {
		    Threats.NumThreats++;
   		  CurThreat->Type    = TEWS_SAM_PROV_THREAT;
		    CurThreat->HasLock = TRUE;
		    CurThreat->SP      = Prov;
		    CurThreat->TEWSAbbrev = (GetRadarPtr((BasicInstance *)Prov))->sNTEWS;
		    CurThreat->Prog       = (GetRadarPtr((BasicInstance *)Prov))->iTEWSType;
   		  CurThreat->Category   = TEWS_SAM_PROV_THREAT;

				if( (GetRadarPtr((BasicInstance *)Prov))->lRadarID == 16 || (GetRadarPtr((BasicInstance *)Prov))->lRadarID == 15 ) // in one case, the sam radar guides aaa
				{
   		     CurThreat->Category = TEWS_SAM_WEAP_THREAT;
				}

				if(IsSAMObjFiringMissleAtPlayer((BasicInstance *)Prov))
				{
           CurThreat->Draw = TewsBlink;
				   CurThreat->Firing = TRUE;
				}
     	  else
				{
		      CurThreat->Draw = TRUE;
				  CurThreat->Firing = FALSE;
				}

		    CurThreat++;
		  }
	    }
	    else
	    {
		   if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
		   {
		     Threats.NumThreats++;
		     CurThreat->Draw    = TRUE;
				 CurThreat->Firing  = FALSE;
		     CurThreat->Type    = TEWS_SAM_PROV_THREAT;
		     CurThreat->HasLock = FALSE;
		     CurThreat->SP      = Prov;
		     CurThreat->TEWSAbbrev = (GetRadarPtr((BasicInstance *)Prov))->sNTEWS;
		     CurThreat->Prog       = (GetRadarPtr((BasicInstance *)Prov))->iTEWSType;
   		   CurThreat->Category    = TEWS_SAM_PROV_THREAT;

				if( (GetRadarPtr((BasicInstance *)Prov))->lRadarID == 16 || (GetRadarPtr((BasicInstance *)Prov))->lRadarID == 15   ) // in one case, the sam radar guides aaa
				{
   		     CurThreat->Category = TEWS_SAM_WEAP_THREAT;
				}

		     CurThreat++;
		   }
	    } // locked up
	 }

     Prov = (InfoProviderInstance *)Prov->Basics.NextRelatedInstance;

  }

}


// jlm for aaa
/*

	if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
	{
		walker = (BasicInstance *)planepnt->AI.pGroundTarget;
		if(walker->Family == FAMILY_AAWEAPON)
		{
			weapon = GetWeaponPtr(walker);
			if(weapon)
			{
				if(weapon->iWeaponType == WEAPON_TYPE_GUN)
				{
					if(weapon->iSeekerType == 10)
					{
						attackspch = 1;
					}
					else
					{
						attackspch = 3;
					}
				}
				else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
				{
					attackspch = 2;
				}
			}
		}
	}
	*/

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetRadarCategoryFromWeaponInstance(AAWeaponInstance *AAWeap)
{
	DBWeaponType *Weapon;

	if(AAWeap->Basics.Family == FAMILY_AAWEAPON)
	{
		Weapon = GetWeaponPtr(&AAWeap->Basics);
		if(Weapon)
		{
			if(Weapon->iWeaponType == WEAPON_TYPE_GUN)
			{
				return(TEWS_SAM_WEAP_THREAT);
			}
			else if(Weapon->iWeaponType == WEAPON_TYPE_SAM)
			{
				return(TEWS_SAM_PROV_THREAT);
			}
		}
	}

	return(TEWS_SAM_PROV_THREAT); // set SAM as a default....shouldn't get this far
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

DBWeaponType *get_weapon_ptr(long id);

int GetRadarCategoryFromVehicle(MovingVehicleParams *Veh)
{
	DBWeaponType *weapon;
	int ReturnType = -1;

	for(int cnt = 0; cnt < 2; cnt ++)
	{
		if(!cnt)
		{
			weapon = get_weapon_ptr(pDBVehicleList[Veh->iVDBIndex].lWeaponType);
		}
		else
		{
			weapon = get_weapon_ptr(pDBVehicleList[Veh->iVDBIndex].lWeaponType2);
		}

		if(weapon)
		{
				if(weapon->iWeaponType == WEAPON_TYPE_SAM)
				{
	    		ReturnType = TEWS_SAM_PROV_THREAT;
				}
				else if((weapon->iWeaponType == WEAPON_TYPE_GUN))
				{
					if(ReturnType == -1)
	      		ReturnType = TEWS_SAM_WEAP_THREAT;
				}
		}
	}

	if(ReturnType == -1)
	  ReturnType = TEWS_SAM_PROV_THREAT;

	return(ReturnType);

}

//***********************************************************************************************************************************
//*********

void ProcessSAMWeapons()
{
  AAWeaponInstance *Weap = &AllAAWeapons[0];
  GDRadarData *Radar;

  while(Weap)
  {

    Radar = (GDRadarData *)Weap->AIDataBuf;

	if(Radar->lRFlags1 & GD_RADAR_PLAYER_PING)
	{
    if( AIPutWeaponProviderInTEWSCenter(Weap,Radar) )
		 //if( (Radar->lRFlags1 & GD_RADAR_LOCK) && (Radar->Target == PlayerPlane) && (Radar->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) && (!(Radar->lRFlags1 & GD_I_AM_DEAD)))
	   {
		   if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
		   {
 		     Threats.NumThreats++;
		     CurThreat->Type     = TEWS_SAM_WEAP_THREAT;
		     CurThreat->HasLock  = TRUE;
		     CurThreat->SW       = Weap;
		     CurThreat->TEWSAbbrev = (GetRadarPtr((BasicInstance *)Weap))->sNTEWS;
		     CurThreat->Prog       = (GetRadarPtr((BasicInstance *)Weap))->iTEWSType;
				 CurThreat->Category   = GetRadarCategoryFromWeaponInstance(Weap);

	         if(IsSAMObjFiringMissleAtPlayer((BasicInstance *)Weap))
					 {
	     	     CurThreat->Draw = TewsBlink;
				     CurThreat->Firing = TRUE;
					 }
		     else
				 {
	  	      CurThreat->Draw = TRUE;
				    CurThreat->Firing = FALSE;
				 }

             CurThreat++;
		   }

	    }
	    else
	    {
		   if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
		   {
		     Threats.NumThreats++;
		     CurThreat->Draw    = TRUE;
		     CurThreat->Firing  = FALSE;
		     CurThreat->Type    = TEWS_SAM_WEAP_THREAT;
		     CurThreat->HasLock = FALSE;
		     CurThreat->SW      = Weap;
		     CurThreat->TEWSAbbrev = (GetRadarPtr((BasicInstance *)Weap))->sNTEWS;
		     CurThreat->Prog       = (GetRadarPtr((BasicInstance *)Weap))->iTEWSType;
				 CurThreat->Category   = GetRadarCategoryFromWeaponInstance(Weap);
		     CurThreat++;
		   }
	    } // locked up
	}

   	Weap = (AAWeaponInstance *)Weap->Basics.NextRelatedInstance;

  }

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessVehicles()
{
	MovingVehicleParams *Veh = MovingVehicles;
	GDRadarData *Radar;
	int radarnum;
	DBRadarType *radarinfo;

  while(Veh <= LastMovingVehicle)
  {
		int NumRadar = 0;

		if(!Veh->iShipType)
		{
			NumRadar = 1;
		}
		else
		{
			NumRadar = pDBShipList[Veh->iVDBIndex].iNumDefenseItems;
			/*
		  for(int i=0; i< pDBShipList[Veh->iVDBIndex].iNumDefenseItems; i++)
			{
			  if(pDBShipList[Veh->iVDBIndex].DefenseList[i].lDefenseType == VS_DEFENSE_RADAR)
					NumRadar++;
			}
			 * */
		}

		Radar = &Veh->RadarWeaponData[0];

		int Index = NumRadar;
		while(Index-- > 0)
		{
			int Pos = Radar - &Veh->RadarWeaponData[0];

			if( (Veh->iShipType && (pDBShipList[Veh->iVDBIndex].DefenseList[Pos].lDefenseType == VS_DEFENSE_RADAR)) || !Veh->iShipType)
			{
		  		if(Radar->lRFlags1 & GD_RADAR_PLAYER_PING)
					{
					if(Veh->iShipType)
					{
						radarnum = pDBShipList[Veh->iVDBIndex].DefenseList[Pos].lTypeID;
						radarinfo = GetRadarPtr(radarnum);
					}
					else
					{
						radarnum = pDBVehicleList[Veh->iVDBIndex].lRadarID;
						radarinfo = GetRadarPtr(radarnum);
					}

					if( AIPutVehicleRadarInTEWSCenter(Veh,Radar) )

	   				//if( (Radar->lRFlags1 & GD_RADAR_LOCK) && (Radar->Target == PlayerPlane) && (Radar->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) && (!(Radar->lRFlags1 & GD_I_AM_DEAD)))
	   				{
						if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
		   				{
 		      				Threats.NumThreats++;
		     					CurThreat->Type     = TEWS_VEH_THREAT;
		     					CurThreat->HasLock  = TRUE;
		     					CurThreat->V        = Veh;
									if(Veh->iShipType)
										CurThreat->Category = TEWS_SAM_PROV_THREAT;
									else
                    CurThreat->Category = GetRadarCategoryFromVehicle(Veh);

								if(radarinfo)
								{
			     					CurThreat->TEWSAbbrev = radarinfo->sNTEWS;  //  pDBWeaponList[Radar->iWeaponIndex].sNTEWS;
			     					CurThreat->Prog       = radarinfo->iTEWSType;  //  pDBWeaponList[Radar->iWeaponIndex].iTEWSType;
								}
								else
								{
			     					CurThreat->TEWSAbbrev = 0;
			     					CurThreat->Prog       = 0;  //  pDBWeaponList[Radar->iWeaponIndex].iTEWSType;
								}
									CurThreat->NumRadar = NumRadar;

								if( AISeeIfStillVehicleMissiled(Veh,PlayerPlane,0,Radar, 1) )
									{
	     	     				CurThreat->Draw = TewsBlink;
				     				CurThreat->Firing = TRUE;
					 				}
     	  					else
									{
		      					CurThreat->Draw = TRUE;
				  					CurThreat->Firing = FALSE;
									}

					    	  CurThreat++;
							}
						}
	    			else
						{
		   				if(Threats.NumThreats < TEWS_MAX_ENTITIES - 1)
		   				{
		     				Threats.NumThreats++;
		     				CurThreat->Draw     = TRUE;
		     				CurThreat->Firing   = FALSE;
		     				CurThreat->Type     = TEWS_VEH_THREAT;
		     				CurThreat->HasLock  = FALSE;
		     				CurThreat->V        = Veh;

								if(Veh->iShipType)
									CurThreat->Category = TEWS_SAM_PROV_THREAT;
								else
                	CurThreat->Category = GetRadarCategoryFromVehicle(Veh);

							if(radarinfo)
							{
			     				CurThreat->TEWSAbbrev = radarinfo->sNTEWS;  //  pDBWeaponList[Radar->iWeaponIndex].sNTEWS;
			     				CurThreat->Prog       = radarinfo->iTEWSType;  //  pDBWeaponList[Radar->iWeaponIndex].iTEWSType;
							}
							else
							{
			     				CurThreat->TEWSAbbrev = 0;
			     				CurThreat->Prog       = 0;  //  pDBWeaponList[Radar->iWeaponIndex].iTEWSType;
							}
								CurThreat->NumRadar = NumRadar;

		     				CurThreat++;
		   				}
						}
					}
			}

			Radar++;
		}
		Veh++;
	}
}


//***********************************************************************************************************************************

extern AA_flash;
extern AA_shootQ;

//***********************************************************************************************************************************
// *****

void DrawTewsObjectEasyMode(int CenterX, int CenterY, FPoint EnemyPos, float EnemyHeading, float Range, int SymbolId, int Id, int UseId, int HasLock, int Draw, int IsTarget, PlaneParams *Plane, int Color, int Green, int Blue)
{
   FPoint PlanePos,BoxPos,Origin;
   float DeltaAzim;
   float NewX, NewZ,Dist;
   float Degree;

   if(!Draw) return;

   PlanePos.X = PlayerPlane->WorldPosition.X;
   PlanePos.Z = PlayerPlane->WorldPosition.Z;

   Dist = sqrt( (PlanePos.X - EnemyPos.X)*(PlanePos.X - EnemyPos.X) +
			    (PlanePos.Z - EnemyPos.Z)*(PlanePos.Z - EnemyPos.Z) );

   Dist *= WUTONM;

   float PixelsPerMile = 50.0/TewsInfo.Range;
   int   NumPixels     = (int)(Dist)*PixelsPerMile;

   if(NumPixels > 50)
	   return;

   DeltaAzim = ComputeHeadingToPoint(PlanePos,PlayerPlane->Heading,EnemyPos, 1);
   ProjectPoint(CenterX, CenterY, NormDegree(DeltaAzim),NumPixels, &NewX, &NewZ);

   // set intensity for arrows based on angle, done here so we don't have to recalculate angles
   Degree = NormDegree(NormDegree(DeltaAzim) + 90.0);

   if(HasLock)
   {

     if( (Degree > 85) && (Degree < 95) )
	   TewsInfo.DownArrowColor = 63;
     else if( (Degree > 175) && (Degree < 185) )
       TewsInfo.RightArrowColor = 63;
     else if( (Degree > 265) && (Degree < 275) )
	   TewsInfo.UpArrowColor = 63;
     else if( ( (Degree > 355) && (Degree < 360) ) || ( (Degree >= 0) && (Degree < 5) ) )
       TewsInfo.LeftArrowColor = 63;
   }

  /// char *Str;
   int  TewsId;

   switch(SymbolId)
   {
	   case TWS_AIRCRAFT_LOCK: TewsId = -1;          break; //Str = "AL"; break;
	   case TWS_AIRCRAFT:      TewsId = -1;          break; //Str = "A";  break;
	   case TWS_GROUND:        TewsId = TEWS_SAM;    break; //Str = "G";  break;
	   case TWS_MISSLE:                                    //Str = "M";  break;
	   case TWS_MISSLE_LOCK:   TewsId = TWS_MISSLE;  break; //Str = "ML"; break;
	   case TEWS_MISSLE0:      TewsId = -1;          break; //Str = "ML"; break;
   }

   if(TewsId == -1)
   {
     int Index;
	 float TempHeading = 360.0 - (PlayerPlane->Heading/DEGREE);
	 EnemyHeading = NormDegree(EnemyHeading + TempHeading);
	 EnemyHeading = 360.0 - EnemyHeading;  // flip it around since sprites are reversed

	 if(SymbolId != TEWS_MISSLE0)
	 {
	   Index = (EnemyHeading > 358.0) ? 0 : (int)((EnemyHeading / 360.0)*15.0);
	   Index = TEWS_AIRCRAFT0  + Index;
	 }
	 else
	 {
	   Index = (EnemyHeading > 358.0) ? 0 : (int)((EnemyHeading / 360.0)*11.0);
	   Index = TEWS_MISSLE0  + Index;
	 }

	 if(IsTarget && (UFC.MasterMode == AA_MODE) )
	 {
		int Draw = TRUE;

		if(AA_shootQ && !AA_flash)
			Draw = FALSE;

		if(Draw)
		{
	  	   TruncatePlaneName(TmpStr,pDBAircraftList[Plane->AI.iPlaneIndex].sName);
    	   DrawBox(GrBuffFor3D,NewX - RDRSym[Index].AnchX,NewZ - RDRSym[Index].AnchY,14,14,Color);
		   DrawTextAtLocation(NewX - RDRSym[Index].AnchX,NewZ - RDRSym[Index].AnchY + 16,TmpStr,Color);
		}
	 }

	 DrawRadarSymbol(NewX,NewZ,Index,Color,GrBuffFor3D);
   }
   else
     DrawRadarSymbol(NewX,NewZ,TewsId,5,GrBuffFor3D);
}

//***********************************************************************************************************************************
// *****

void DrawTewsObject(int MpdNum, int CenterX, int CenterY, FPoint EnemyPos, float EnemyHeading, float Range, int SymbolId, int Id, int UseId, int HasLock, int Draw, int IsTarget, PlaneParams *Plane, int Color, int Green, int Blue)
{
   FPoint PlanePos,BoxPos,Origin;
   float DeltaAzim;
   float NewX, NewZ,Dist;
   float Degree;

   if(g_Settings.gp.nTEWS == GP_TYPE_CASUAL)
   {
     DrawTewsObjectEasyMode(CenterX,CenterY,EnemyPos,EnemyHeading,Range,SymbolId,Id,UseId,HasLock,Draw,IsTarget,Plane,Color);
	 return;
   }

   if(!Draw) return;

   PlanePos.X = PlayerPlane->WorldPosition.X;
   PlanePos.Z = PlayerPlane->WorldPosition.Z;

   Dist = sqrt( (PlanePos.X - EnemyPos.X)*(PlanePos.X - EnemyPos.X) +
			    (PlanePos.Z - EnemyPos.Z)*(PlanePos.Z - EnemyPos.Z) );

   Dist *= WUTONM;

   DeltaAzim = ComputeHeadingToPoint(PlanePos,PlayerPlane->Heading,EnemyPos, 1);
   ProjectPoint(CenterX, CenterY, NormDegree(DeltaAzim),Range, &NewX, &NewZ);

   // set intensity for arrows based on angle, done here so we don't have to recalculate angles
   Degree = NormDegree(NormDegree(DeltaAzim) + 90.0);

   if(HasLock)
   {

     if( (Degree > 85) && (Degree < 95) )
	   TewsInfo.DownArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_ALERT];
     else if( (Degree > 175) && (Degree < 185) )
       TewsInfo.RightArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_ALERT];
     else if( (Degree > 265) && (Degree < 275) )
	   TewsInfo.UpArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_ALERT];
     else if( ( (Degree > 355) && (Degree < 360) ) || ( (Degree >= 0) && (Degree < 5) ) )
       TewsInfo.LeftArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_ALERT];
   }

   DrawTEWSRadarSymbol(NewX, NewZ, SymbolId, Id, UseId, Color);

}

//***********************************************************************************************************************************
// *****

void DrawTewsObjects(int MpdNum, int CenterX, int CenterY, int Color, int Green, int Blue)
{



}

//***********************************************************************************************************************************
// *****

void InitTewsInfo()
{
  TewsInfo.DeclutterOn = FALSE;

  if(g_Settings.gp.nTEWS != GP_TYPE_CASUAL)
     TewsInfo.CurMode = TEWS_MANUAL_MODE;
  else
     TewsInfo.CurMode = TEWS_AUTO_MODE;

  TewsInfo.Range = 40;

  ResetCounterMeasures();

  NumChaffScheduled = 0;

  PrevThreats.NumThreats = 0;
  Threats.NumThreats = 0;

  TewsLockTimer = -1;
  TewsMissleLaunchWarnOn = FALSE;

  ChaffWarningGiven = FALSE;
  FlareWarningGiven = FALSE;

}

//***********************************************************************************************************************************
int GetIcsStatus()
{
  if(UFC.EMISState)
    return(ICS_EMIS);
  else if(PlayerPlane->AI.iAIFlags1 & AIJAMMINGON)
	return(ICS_JAM);
  else
    return(ICS_STBY);
}

//***********************************************************************************************************************************
void DrawTEWSRadarSymbol(int ScrX, int ScrY, int RadarSymId, int TargetId, int UseId, int Color, int Green, int Blue)
{
	char *Str;
	int Found = FALSE;

	//DrawRadarSymbol(ScrX, ScrY, RadarSymId,Color,GrBuffFor3D);
	DrawRadarSymbolKeepColors(ScrX, ScrY,RadarSymId);

	if(MpdDamage[MPD_DAMAGE_CEN_COMP]) return;  // central computer damage, show no labels

	if(UseId)
	{
		switch(TargetId)
		{
		  case 0:  Str = "EW"; Found = TRUE; break;
		  case 1:  Str = "F1"; Found = TRUE; break;
		  case 4:  Str = "F4"; Found = TRUE; break;
		  case 5:  Str = "F5"; Found = TRUE; break;
		  case 12: Str = "SD"; Found = TRUE; break;
		  case 28: Str = "RO"; Found = TRUE; break;
		  case 30: Str = "HA"; Found = TRUE; break;
		  case 50: Str = "IR"; Found = TRUE; break;
		  case 51: Str = "SA"; Found = TRUE; break;
		  case 52: Str = "AR"; Found = TRUE; break;
		  case 99: Str = "AA"; Found = TRUE; break;
		  default: Str = "";
		}

		if(Found)
		  sprintf(TmpStr,"%s",Str);
		else
		  sprintf(TmpStr,"%d",TargetId);

		DrawTextAtLocation(ScrX - 1, ScrY,TmpStr,Color);
	}

}

//***********************************************************************************************************************************
// *****

int PlaneLocked()
{
  ThreatEntryType *T = &Threats.List[0];
  int Index = Threats.NumThreats;

  while(Index-- > 0)
  {
    if(T->HasLock)
     return(TRUE);
	T++;
  }

  return(FALSE);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CheckTewsWarnings()
{
  ThreatEntryType *T = &Threats.List[0];

	Cpit.AIVar = FALSE; Cpit.SAMVar = FALSE; Cpit.AAAVar = FALSE;

	int Index = Threats.NumThreats;
  while(Index-- > 0)
  {
    if(T->HasLock)
		{
			if(T->Category == TEWS_PLANE_THREAT)
			{
				if(T->Draw)
				   Cpit.AIVar   = TRUE;
			}
			if(T->Category == TEWS_SAM_PROV_THREAT)
			{
				if(T->Draw)
			 		 Cpit.SAMVar = TRUE;
			}
			if(T->Category == TEWS_SAM_WEAP_THREAT)
			{
				if(T->Draw)
			 	  Cpit.AAAVar = TRUE;
			}
		}
	  T++;
  }
}

//***********************************************************************************************************************************
// *****

void DisplayTewsWarnings()
{
  if(CurrentCockpit == BACK_FORWARD_COCKPIT)
  {
    if(PlaneLocked())
			Cpit.AIVar = TRUE;
		else
			Cpit.AIVar = FALSE;
  }
  else
  {
    if(PlaneLocked())
      DisplayWarning(AI_SAM, ON,  0);
    else
	  DisplayWarning(AI_SAM, OFF, 0);
  }
}


//***********************************************************************************************************************************
// *****

void DrawTews(int MpdNum, int MpdDetectNum)
{
	/*
  int CenterX;
  int CenterY;
  float Angle;
  int TotalLength = 55;
  int End1X,End1Y,End2X,End2Y,End3X,End3Y;
  int LineColor, TextColor, SymbColor;
  int LongDash;
  float TotalAngle;
  float EAngle,NAngle,SAngle,WAngle;
  float Heading,PreciseHeading;
  int DrawDash;
  int TempCenterX, TempCenterY;
  int Diameter = 86;              // JLM make a constant for other modes to use

  */

  //jlm
  return;



/*

  PushClipRegion();
  SetClipRegion(0, 639,0,479, 240, 320);

  Heading = PlayerPlane->Heading/DEGREE;

  PreciseHeading = PlayerPlane->Heading*100;
  PreciseHeading = PreciseHeading/DEGREE;
  PreciseHeading = PreciseHeading/100.0;

  NAngle = NormDegree(360.0 - Heading + 90.0);
  SAngle = NormDegree(NAngle + 180.0);
  EAngle = NormDegree(NAngle + 270.0);
  WAngle = NormDegree(NAngle + 90.0);

  LineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

  CenterX = MPD.MpdList[MpdDetectNum].CenterX;
  CenterY = MPD.MpdList[MpdDetectNum].CenterY;

  TempCenterX = CenterX;
  TempCenterY = CenterY + 3;




	//*****************************************************************************************************************************************
	// JLM F18 TEMP  -- *Header -- (search "*Header" to step through each section of file)
	//*****************************************************************************************************************************************

	DrawDoAllSensor(TempCenterX,TempCenterY);
	PopClipRegion();

//*****************************************************************************************************************************************
// JLM F18 TEMP  -- *Header -- (search "*Header" to step through each section of file)
//*****************************************************************************************************************************************

  // init lists for processing
  TewsInfo.LeftArrowColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_NO_ALERT];
  TewsInfo.RightArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_NO_ALERT];
  TewsInfo.UpArrowColor    = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_NO_ALERT];
  TewsInfo.DownArrowColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_TEWS_ARROW_NO_ALERT];

  SimTewsBlink();
  DrawTewsObjects(MpdNum, TempCenterX, TempCenterY, SymbColor);

  DrawRadarSymbol(TempCenterX + 67, TempCenterY, RDR_LEFT_ARROW, TewsInfo.LeftArrowColor,GrBuffFor3D);
  DrawRadarSymbol(TempCenterX - 67, TempCenterY, RDR_RIGHT_ARROW, TewsInfo.RightArrowColor,GrBuffFor3D);
  DrawRadarSymbol(TempCenterX, TempCenterY + 68, RDR_UP_ARROW, TewsInfo.UpArrowColor,GrBuffFor3D);
  DrawRadarSymbol(TempCenterX, TempCenterY - 64, RDR_DOWN_ARROW, TewsInfo.DownArrowColor,GrBuffFor3D);

  DrawTextAtLocation(TempCenterX + 39, TempCenterY-58,"CHF",TextColor);
  sprintf(TmpStr,"%d",ChaffTotal);
  DrawTextAtLocation(TempCenterX + 57, TempCenterY-58,TmpStr,TextColor);

  DrawTextAtLocation(TempCenterX + 39, TempCenterY-51,"FLR",TextColor);
  sprintf(TmpStr,"%d",FlareTotal);
  DrawTextAtLocation(TempCenterX + 57, TempCenterY-51,TmpStr,TextColor);


  char *Str;

  TewsInfo.IcsStatus = GetIcsStatus();
  switch(TewsInfo.IcsStatus)
  {
    case ICS_STBY:
		Str = "STBY";
		break;
    case ICS_JAM:
		Str = "JAM";
		break;
    case ICS_EMIS:
		Str = "EMIS";
		break;
  }

  DrawTextAtLocation(TempCenterX - 66, TempCenterY-52,"ICS",TextColor);
  DrawTextAtLocation(TempCenterX - 53, TempCenterY-52,Str,TextColor);

  if(g_Settings.gp.nTEWS != GP_TYPE_CASUAL)
     DrawVerticalWordAtLocation(TempCenterX - 66, TempCenterY - 30,"DCL",TewsInfo.DeclutterOn,TextColor);

  switch(TewsInfo.CurMode)
  {
    case TEWS_MANUAL_MODE:
	  Str = "MAN";
	  break;
    case TEWS_SEMI_AUTO_MODE:
	  Str = "SEMI";
	  break;
    case TEWS_AUTO_MODE:
	  Str = "AUTO";
	  break;
  }

  DrawWordAtButton(MpdDetectNum,19, Str, FALSE, TextColor);

  DrawCircleNoClip(GrBuffFor3D , TempCenterX, TempCenterY, 56, LineColor);

  if(g_Settings.gp.nTEWS == GP_TYPE_CASUAL)
  {
	sprintf(TmpStr,"%d",(int)TewsInfo.Range);
    DrawWordAtButton(MpdDetectNum,13,TmpStr,FALSE,TextColor);
  }

  if( (!TewsInfo.DeclutterOn) && (g_Settings.gp.nTEWS != GP_TYPE_CASUAL) )
  {

    int DrawDotCount = 0;
    Angle = NAngle;
    TotalAngle = 0.0;


  } // declutter not on

  DrawPlaneSymbol(TempCenterX,TempCenterY, LineColor);

  if(!MpdDamage[MPD_DAMAGE_CEN_COMP])       // central comp damage knocks out Nav Info
  {
    DisplayNavInfo(CenterX + 11, CenterY + 7, TextColor);
    DisplayTacanInfo(CenterX - 5, CenterY + 5, TextColor);
  }

  PopClipRegion();

  */

 }

//***********************************************************************************************************************************
// *****


//***********************************************************************************************************************************
// *****

void TewsIncRange()
{
  TewsInfo.Range *= 2;
  if(TewsInfo.Range > 80)
	TewsInfo.Range = 80;
}

//***********************************************************************************************************************************
// *****

void TewsDecRange()
{
  TewsInfo.Range /= 2;
  if(TewsInfo.Range < 10)
	TewsInfo.Range = 10;
}

void StepThroughTewsCmdMode()
{
  TewsInfo.CurMode++;
  if(TewsInfo.CurMode > 2)
	TewsInfo.CurMode = 0;
  if(TewsInfo.CurMode == TEWS_MANUAL_MODE)
  ResetCounterMeasures();
}
//***********************************************************************************************************************************
// ***** TEWS CODE END


//***********************************************************************************************************************************
// *****
extern float EMD_lSpin;
extern float EMD_rSpin;
extern float EMD_lTemp;
extern float EMD_rTemp;
extern float EMD_lFlow;
extern float EMD_rFlow;
extern float EMD_lOpen;
extern float EMD_rOpen;
extern float EMD_lOlio;
extern float EMD_rOlio;

void EngineText( int line, int x, int y, int color, char *label, char *left, char *right )
{
	if (line)  y += 5 + 10*line;	// 10 pixels per line plus 5 after heading

	RightJustifyTextAtLocation( x,   y, label, color );
			DrawTextAtLocation( x+15,y, left,  color );
			DrawTextAtLocation( x+45,y, right, color );
}

void EngineData( int line, int x, int y, int color, char *label, int left, int right )
{
	if (line)  y += 5 + 10*line;	// 10 pixels per line plus 5 after heading

	RightJustifyTextAtLocation( x,   y, label, color );
			 DrawIntAtLocation( x+15,y, left,  color );
			 DrawIntAtLocation( x+45,y, right, color );
}

void DrawEng(int MpdNum, int MpdDetectNum)
{
  //ClearMpd(MpdDetectNum);

  int x = MPD.MpdList[MpdDetectNum].CenterX - 20;
  int y = MPD.MpdList[MpdDetectNum].CenterY - 40;
  int c = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  EngineText( 0, x,y,c, "ENGINE",		"L",		"R"  );
  EngineData( 1, x,y,c, "RPM PER",		(int)EMD_lSpin, (int)EMD_rSpin );
  EngineData( 2, x,y,c, "TEMP C",		(int)EMD_lTemp, (int)EMD_rTemp );
  EngineData( 3, x,y,c, "FF/PPH",		(int)EMD_lFlow, (int)EMD_rFlow );
  EngineData( 4, x,y,c, "NOZ POS PER",	(int)EMD_lOpen, (int)EMD_rOpen );
  EngineData( 5, x,y,c, "OIL PSI",      (int)EMD_lOlio, (int)EMD_rOlio );

  DrawTextAtLocation(x - 8,y + 10 + 10*6,"GROSS WEIGHT",c);
  sprintf(TmpStr,"%d",(int)PlayerPlane->TotalWeight);
  DrawTextAtLocation(x,y + 18 + 10*6,TmpStr,c);

  DrawWordAtButton( MpdDetectNum,10, "M", FALSE, c );
}


//***********************************************************************************************************************************
// *****

void EngMode(int MpdNum, int MpdDetectNum)
{
  int ButtonNum;
  int Pressed;

  //ClearMpd(MpdDetectNum);

  DrawEng(MpdNum,MpdDetectNum);

  GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

  if (Pressed)
  {
	 if(ButtonNum == 10)
	 {
	   Mpds[MpdNum].Mode = MAIN_MENU_MODE;
	 }

  }

}

//***********************************************************************************************************************************
// *****

void WpnMode(int MpdNum, int MpdDetectNum)
{
  ProcessWeaponMode(MpdNum);
  if(Mpds[MpdNum].WeapInfo.ModeToUse == MAVERIC_TYPE)
     DoMaverick(MPD.MpdList[MpdDetectNum].CenterX-1,MPD.MpdList[MpdDetectNum].CenterY+4, MpdNum, MpdDetectNum);
  else
     DoGbu(MPD.MpdList[MpdDetectNum].CenterX-1,MPD.MpdList[MpdDetectNum].CenterY+4, MpdNum, MpdDetectNum);
}

//************************************************************************************************
void DisplayADIHeadingBar(int MpdNum, PlaneParams *P, int Left, int Right, int YPos)
{
	int LineColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
	int TextColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

	#define PIXELS_PER_DEGREE	4.1
	int WayOfs = -1;
	int Nums[5];
	int Pos[5] = {Left-3, Left+40-3, Right-40-3, Right-3, Right+40-3};
	int XPos, CurrentHeading;
	float CurrentWayPointHeading;
	ANGLE LeftHead, RightHead;

    int TranX = -3;
	int TranY = 0;
	int jaystop = 3;


	if ((UFC.MasterMode == AG_MODE) && ((P->AGDesignate.X != -1) && (P->AGDesignate.X != -1)))
	{
		CurrentWayPointHeading = UFC.DataCurrTGTAbsBrg;
	}
	else
	{
		CurrentWayPointHeading = UFC.DataCurrSPMilBrg;
	}


	CurrentWayPointHeading *= (float)(DEGREE);

	ANGLE TmpHeading = -(int)P->Heading;

	// Let's compute sprite offset based on heading	value

	CurrentHeading = Nums[2] = TmpHeading/DEGREE;

	int XOfsMod = (int)((TmpHeading % (10*DEGREE))*(1.0f/45.51041668f)); 	// heading num letters offset
	int SprOfs  = (int)((TmpHeading % ( 2*DEGREE))*(1.0f/45.51041668f)); 	// sprite bar offset

    // Compute extreme end values of heading bar for use in waypoint tick mark location

	LeftHead   = (ANGLE)((ANGLE)TmpHeading - (ANGLE)(17.8*(float)DEGREE));
	RightHead  = (ANGLE)((ANGLE)TmpHeading + (ANGLE)(17.8*(float)DEGREE));

	// Let's compute the display heading nums of interest

	Nums[2] /= 10;					// 0 - 35(350)
	if (!Nums[2]) Nums[2] = 36;		// 01(10) - 36(360)

	Nums[3] = (Nums[2] == 36) ?  1 : (Nums[2]+1);
	Nums[4] = (Nums[3] == 36) ?  1 : (Nums[3]+1);

	Nums[1] = (Nums[2] == 1)  ? 36 : (Nums[2]-1);
	Nums[0] = (Nums[1] == 1)  ? 36 : (Nums[1]-1);

  	// Let's determine where the numbers are positioned and draw them

	for (int i=0;i<5;i++)
	{
		int TmpPos = Pos[i]-XOfsMod;

		// Display heading position

		if ((TmpPos > (Left+8+9)) && (TmpPos < (Right+33-9)))
		{
			sprintf(TmpStr,(Nums[i] < 10) ? "0%d" : "%d", Nums[i]);
			GrDrawStringClipped(GrBuffFor3D, SmHUDFont, TmpPos+TranX, YPos-5+TranY, TmpStr, TextColor);
		}
	}

	// display waypoint tick mark

	XPos = -1;
	if (LeftHead < RightHead)
	{
		if ((CurrentWayPointHeading >= LeftHead) && (CurrentWayPointHeading <= RightHead))
			XPos = (int)(Left+8.0+  (((float)(CurrentWayPointHeading-LeftHead)/45.51041668)));
	}
	else
	{
		if ((CurrentWayPointHeading > LeftHead) && (CurrentWayPointHeading > RightHead))
			XPos = (int)(Left+8.0 +  (((float)(CurrentWayPointHeading-LeftHead)/45.51041668)));
		else
		if ((CurrentWayPointHeading < LeftHead) && (CurrentWayPointHeading < RightHead))
			XPos = (int)(Left+8.0 +  (((float)((0xffff-LeftHead)+CurrentWayPointHeading)/45.51041668)));
	}

	if ((XPos > Left+8+9) && (XPos < Right+33-9))
	{
		GrDrawLineClipped(GrBuffFor3D, XPos+TranX,   YPos+3+TranY, XPos+TranX,   YPos+9+TranY, LineColor);
		GrDrawLineClipped(GrBuffFor3D, XPos+1+TranX, YPos+3+TranY, XPos+1+TranX, YPos+9+TranY, LineColor);
	}
	else
	{
		// our waypoint is off heading bar - peg it to least extreme
		// normalize points for easier comparision

		float Cwp = (float)(CurrentWayPointHeading/DEGREE) - (float)CurrentHeading;	 // normalize current waypoint by fixing heading to zero
		if (Cwp < 0.0f) Cwp = 360.0f - Cwp;

		XPos = (Cwp <= 180.0) ? (Right+34-9) : (Left+7+9);

		GrDrawLineClipped(GrBuffFor3D, XPos+TranX,   YPos+3+TranY,  XPos+TranX,   YPos+9+TranY, LineColor);
		GrDrawLineClipped(GrBuffFor3D, XPos+1+TranX, YPos+3+TranY,  XPos+1+TranX, YPos+9+TranY, LineColor);

		if ((UFC.MasterMode == AG_MODE) && ((P->AGDesignate.X != -1) && (P->AGDesignate.X != -1)))
		{
			sprintf(TmpStr,"%3d", (int)UFC.DataCurrTGTAbsBrg);
		}
		else
		{
			sprintf(TmpStr,"%3d", (int)UFC.DataCurrSPMilBrg);
		}

		GrDrawStringClipped(GrBuffFor3D, LgHUDFont, XPos-7+TranX, YPos+11+TranY, TmpStr, TextColor);
	}

	// display heading ladder
	// display heading ladder


	extern void HUD_icon(int,int,int,int,int,int);
	HUD_icon( Right-45, YPos+2, 0, SprOfs,0, TextColor );
	HUD_icon( Right-45, YPos+3, 1,      0,0, TextColor );





/*
	GrCopyRectMask8Color(GrBuffFor3D, Left+16+TranX, YPos+TranY, HUDSym[SM_HEADING_BAR].Buff, SprOfs, 0,
					    (HUDSym[SM_HEADING_BAR].Width-16), HUDSym[SM_HEADING_BAR].Height,Color);

	GrCopyRectMask8Color(GrBuffFor3D, Right-42+TranX, YPos+4+TranY, HUDSym[SM_HEADING_PTR].Buff, 0, 0,
					     HUDSym[SM_HEADING_PTR].Width, HUDSym[SM_HEADING_PTR].Height,Color);
*/
}


//************************************************************************************************
void DisplayADIAltitude(int MpdNum, PlaneParams *P, int RadarStartX, int RadarStartY)
{
        int LineColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

	    int LAW = UFC.LowAltWarningLimit;
	    int AGL = (int)(P->HeightAboveGround * WUTOFT - 3.0);

		if(AGL > 1500) AGL = 1500;

		int Tick1=RadarStartY;
		int Tick2=Tick1+10;
		int Tick3=Tick2+18;
		int Tick1Sep = 10;
		int Tick2Sep = 18;
		int Tick3Sep = 45;

		int AltTickOfs;

		if (LAW > 1000)  AltTickOfs =  Tick1 + (1500-LAW) * Tick1Sep / 500;
		else
		if (LAW >  500)  AltTickOfs =  Tick2 + (1000-LAW) * Tick2Sep / 500;
		else			 AltTickOfs =  Tick3 + ( 500-LAW) * Tick3Sep / 500;

		// draw low altitude preset
		GrDrawLineClipped(GrBuffFor3D, RadarStartX+7,AltTickOfs, RadarStartX+11,AltTickOfs-3, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX+7,AltTickOfs, RadarStartX+11,AltTickOfs+3, LineColor);

		GrDrawStringClipped(GrBuffFor3D, SmHUDFont, RadarStartX-7,RadarStartY-2, "15", LineColor);
		GrDrawStringClipped(GrBuffFor3D, SmHUDFont, RadarStartX-7,RadarStartY+8, "10", LineColor);
		GrDrawStringClipped(GrBuffFor3D, SmHUDFont, RadarStartX-7,RadarStartY+26, "5", LineColor);

		// display tick marks (make this a sprite!)

		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY,   RadarStartX+5,RadarStartY,    LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+5, RadarStartX+3,RadarStartY+5,  LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+10,RadarStartX+5,RadarStartY+10, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+19,RadarStartX+3,RadarStartY+19, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+28,RadarStartX+5,RadarStartY+28, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+37,RadarStartX+3,RadarStartY+37, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+46,RadarStartX+3,RadarStartY+46, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+55,RadarStartX+3,RadarStartY+55, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+64,RadarStartX+3,RadarStartY+64, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX,RadarStartY+73,RadarStartX+3,RadarStartY+73, LineColor);

		// display altitude bar
		if (AGL > 1000)  AltTickOfs =  Tick1 + (1500-AGL) * Tick1Sep / 500;
		else
		if (AGL >  500)  AltTickOfs =  Tick2 + (1000-AGL) * Tick2Sep / 500;
		else			 AltTickOfs =  Tick3 + ( 500-AGL) * Tick3Sep / 500;

		GrDrawLineClipped(GrBuffFor3D, RadarStartX + 6,RadarStartY+73, RadarStartX + 6,AltTickOfs, LineColor);
		GrDrawLineClipped(GrBuffFor3D, RadarStartX + 7,RadarStartY+73, RadarStartX + 7,AltTickOfs, LineColor);

}

//***********************************************************************************************************************************
// *****

void DisplayADITurnRate(int MpdNum, PlaneParams *P, int ScrX, int ScrY)
{
  float NewX,NewZ;
  float AngleDiff;

  int LineColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  int SymbColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  if(FirstCalcTurnRate)
  {
    FirstCalcTurnRate = FALSE;
	LastTimeCalcTurnRate = GameLoopInTicks;
	PrevPosition = PlayerPlane->WorldPosition;
   	ProjectPoint(PrevPosition.X, PrevPosition.Z, PlayerPlane->Heading/DEGREE, 2000.0, &NewX, &NewZ);
    PrevProjPosition.X = NewX;
    PrevProjPosition.Z = NewZ;
    PrevProjPosition.Y = 0.0;
  }
  else
  {
  	if(GameLoopInTicks - LastTimeCalcTurnRate > 300)
	{
	  float Dt = GameLoopInTicks - LastTimeCalcTurnRate;
 	  AngleDiff = ComputeHeadingToPoint(PrevPosition,PlayerPlane->Heading, PrevProjPosition, 1);
	  ProjectPoint(PrevPosition.X, PrevPosition.Z,PlayerPlane->Heading/(float)DEGREE,2000.0,&NewX, &NewZ);
      PrevProjPosition.X = NewX;
      PrevProjPosition.Z = NewZ;
	  PrevProjPosition.Y = 0.0;

	  PrevPosition = PlayerPlane->WorldPosition;
      float TempTurnRate = AngleDiff/((float)Dt/50.0);
	  TurnRate = TempTurnRate;
	  if(TurnRate > 20.0) TurnRate = 20.0;
	  if(TurnRate < -20.0) TurnRate = -20.0;
      LastTimeCalcTurnRate = GameLoopInTicks;
	}

  }

  DrawRadarSymbol(ScrX + (int)(TurnRate*(27.0/20.0)),ScrY + 5,RDR_UP_ARROW, SymbColor,GrBuffFor3D);

  GrDrawLineClipped(GrBuffFor3D,ScrX-26,ScrY,ScrX+26,ScrY,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX,ScrY,ScrX,ScrY+3,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX+13,ScrY,ScrX+13,ScrY+3,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX+26,ScrY,ScrX+26,ScrY+3,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX-13,ScrY,ScrX-13,ScrY+3,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX-26,ScrY,ScrX-26,ScrY+3,LineColor);
}

//***********************************************************************************************************************************
// *****

void DrawADIInfo(int MpdNum, int ScrX, int ScrY)
{
 // char *Str;
  float AOA = 0; //DegAOAToUnits((float)RadToDeg(PlayerPlane->Alpha));
  int TextColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  if (AOA > 45.5f) AOA = 45.5f;
  if (AOA < 0.0f) AOA = 0.0f;


  IntTo3DigitFont(int(PlayerPlane->IndicatedAirSpeed),Str1);
  GrDrawString(GrBuffFor3D, SmHUDFont, ScrX - 65, ScrY - 50, Str1, TextColor);

  sprintf(TmpStr,"%4.1f",AOA);
  GrDrawString(GrBuffFor3D, SmHUDFont, ScrX - 65, ScrY - 40, TmpStr, TextColor);

  int IntAlt = (int)PlayerPlane->Altitude;
  int TmpAlt = IntAlt;

  int NumChars;
  int LgOfs = -1;
  int SmOfs = -1;
  char SaveChar;

	// Baro altitude
  if (IntAlt > 9999) NumChars = 5;
  else if (IntAlt > 999) NumChars = 4;
  else if (IntAlt > 99)  NumChars = 3;
  else if (IntAlt > 9) NumChars = 2;
  else NumChars = 1;

  sprintf(TmpStr,"%d",IntAlt);

  if (NumChars < 4)
  {
		// <1000 all letters are large
		GrDrawStringClipped(GrBuffFor3D, LgHUDFont, ScrX+40-((NumChars-1)*5), ScrY-45, TmpStr, TextColor);

  }else{

		if (NumChars == 5)
		{
			SaveChar = TmpStr[2];
			TmpStr[2] = 0;
			GrDrawStringClipped(GrBuffFor3D, LgHUDFont, ScrX+43,ScrY-45, TmpStr, TextColor);
			TmpStr[2] = SaveChar;
			GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+53,ScrY-43, &TmpStr[2], TextColor);
		}else{

			SaveChar = TmpStr[1];
			TmpStr[1] = 0;
			GrDrawStringClipped(GrBuffFor3D, LgHUDFont, ScrX+43,ScrY-45, TmpStr, TextColor);
			TmpStr[1] = SaveChar;
			GrDrawStringClipped(GrBuffFor3D, SmHUDFont, ScrX+53,ScrY-43, &TmpStr[1], TextColor);
		}
  }

  if( (PlayerPlane->LandingGearStatus) || (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) )
  {
    float VertVel = PlayerPlane->IfVelocity.Y;

    VertVel = VertVel - fmod(VertVel,100);

    GrDrawString(GrBuffFor3D, LgHUDFont, ScrX + 43, ScrY - 35, "W ", TextColor);
    sprintf(TmpStr,"%d",(int)VertVel);
    GrDrawString(GrBuffFor3D, SmHUDFont, ScrX + 53, ScrY - 35, TmpStr, TextColor);
  }

}

//***********************************************************************************************************************************
// *****

void DisplayADISlipIndicator(int MpdNum, PlaneParams *P, int ScrX, int ScrY)
{
  int XPos;
  float TempSlip = PlayerPlane->Beta;
  int BoldColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
  int LineColor   = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];


  if(TempSlip > 0.1)  TempSlip = 0.1;
  if(TempSlip < -0.1) TempSlip = -0.1;

  XPos = ScrX + TempSlip*(13.0/0.1);

  GrFillRectNoClip(GrBuffFor3D, XPos-2,ScrY+1,5,5,LineColor);

  GrDrawLineClipped(GrBuffFor3D,ScrX-15,ScrY,ScrX+15,ScrY,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX-15,ScrY+5,ScrX+15,ScrY+5,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX-15,ScrY,ScrX-15,ScrY+5,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX+15,ScrY,ScrX+15,ScrY+5,LineColor);

  GrDrawLineClipped(GrBuffFor3D,ScrX+3,ScrY,ScrX+3,ScrY+5,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX+9,ScrY,ScrX+9,ScrY+5,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX-3,ScrY,ScrX-3,ScrY+5,LineColor);
  GrDrawLineClipped(GrBuffFor3D,ScrX-9,ScrY,ScrX-9,ScrY+5,LineColor);

}

//***********************************************************************************************************************************
// *****

#define ADI_STEER_PIXELS 50
#define ADI_GLIDE_PIXELS 50

extern float ILS_glide;
extern float ILS_steer;

void DrawAdiILSInfo(int MpdNum, int XOfs,int YOfs)
{
  if (!UFC.ILSStatus) return;

  int     BoldLineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
  int     SoftLineColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

  float SteerOffset = (ILS_steer/3.0)*ADI_STEER_PIXELS*0.5;
  float GlideOffset = (ILS_glide/3.0)*ADI_GLIDE_PIXELS*0.5;
  float SteerX;
  float GlideY;

  if(SteerOffset > ADI_STEER_PIXELS*0.5)  SteerOffset = ADI_STEER_PIXELS*0.5;
  if(SteerOffset < -ADI_STEER_PIXELS*0.5) SteerOffset = -ADI_STEER_PIXELS*0.5;
  if(GlideOffset > ADI_GLIDE_PIXELS*0.5)  GlideOffset = ADI_GLIDE_PIXELS*0.5;
  if(GlideOffset < -ADI_GLIDE_PIXELS*0.5) GlideOffset = -ADI_GLIDE_PIXELS*0.5;

  SteerX  = XOfs + SteerOffset;
  GlideY  = YOfs + GlideOffset;

  GrDrawLine(GrBuffFor3D,SteerX,YOfs + 20,SteerX,YOfs - 20, BoldLineColor);
  GrDrawLine(GrBuffFor3D,XOfs + 20,GlideY,XOfs - 20,GlideY, BoldLineColor);

  int SideBarX = XOfs - 55;
  float SideBarY = YOfs - ADI_GLIDE_PIXELS*0.5;

  GrDrawLine(GrBuffFor3D,SideBarX,SideBarY,SideBarX,SideBarY + ADI_STEER_PIXELS, SoftLineColor);

  float PixelsPerStep = ADI_GLIDE_PIXELS/6.0;

  GrDrawLine(GrBuffFor3D,SideBarX,SideBarY,SideBarX+3,SideBarY,SoftLineColor);

  SideBarY += PixelsPerStep;

  int Index = 6;
  while(Index-- > 0)
  {
    GrDrawLine(GrBuffFor3D,SideBarX,round(SideBarY),SideBarX+3,round(SideBarY),SoftLineColor);
    SideBarY += PixelsPerStep;
  }


}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void UFCModeTemp(int MpdNum)
{
	DisplayUFCMockup();



}


 extern NumEasyModeObjects;
//***********************************************************************************************************************************
// *****

void DrawMpd(int MpdNum, int MpdDetectNum)
{
	int TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

	ClearMpd(MpdDetectNum);

	if(!MPDBroken(MpdNum))
		switch (Mpds[MpdNum].Mode)
		{
			case ADI_MODE	     :  AdiMode(MpdNum); break;
			case HSI_MODE	     :  HSIMode(MpdNum); break;
			case UFC_MODE	     :  UFCMode(MpdNum); break;
			case MAIN_MENU_MODE  :  MainMenuMode(MpdNum); break;
			case STORES_MODE     :  DoWeaponStoresMode(MpdNum); break;
			case BIT_MODE        :  BitMode(MpdNum); break;
			case ACL_MODE        :  AclMode(MpdNum); break;
			case FPAS_MODE       :  FpasMode(MpdNum); break;
			case CHECKLIST_MODE  :  CheckListMode(MpdNum); break;
			case FUEL_MODE       :  FuelMode(MpdNum); break;
			case EASY_CAM_MODE   :  EasyCamMode(MpdNum); break;
			case EASY_SENSOR_MODE:  EasySensorMode(MpdNum); break;
			case SA_MODE         :  SitAwareMode(MpdNum); break;
			case EDF_MODE        :  EdfMode(MpdNum); break;
			case HUD_MODE	     :  HudMode(MpdNum); break;
			case ENG_MODE	     :  EngMode(MpdNum); break;


			case TGT_IR_MODE:
				if(MpdDamage[MPD_DAMAGE_TARGET_IR])
					MPDSystemMalfunction(MpdNum);
				else
					FlirMode(MpdNum);
				break;

			case TEWS_MODE:
				if(MpdDamage[MPD_DAMAGE_TEWS])
					MPDSystemMalfunction(MpdNum);
				else
					TewsMode(MpdNum);
				break;

			case RADAR_MODE:
				if(MpdDamage[MPD_DAMAGE_RADAR])
				{
					MPDSystemMalfunction(MpdNum);
					if( (Av.AARadar.CurMode == AA_STT_MODE) || (Av.AARadar.CurMode == AA_ACQ_MODE) && (!Av.AARadar.SilentOn) )
					{
						ResetToRWS();
					}
					if(!Av.AARadar.SilentOn)
					{
						PlayerPlane->AADesignate = NULL;
					}
					if(!Av.AGRadar.SilenceOn)
					{
						BreakAGLock();
					}
					Av.AARadar.SilentOn = TRUE;
					Av.AGRadar.SilenceOn = TRUE;
				}
				else
				{
					if( (UFC.MasterMode == AA_MODE) || (UFC.MasterMode == NAV_MODE) )
						DoAirToAirRadar(MpdNum);
					else
						DoAGRadar(MpdNum);
				}
				break;

			case NAVFLIR_MODE:
				if(PlayerPlane->SystemInactive & DAMAGE_BIT_NAV_FLIR)
					MPDSystemMalfunction(MpdNum);
				else
					NavFlirMode(MpdNum);
			break;
		} // end switch
}

//***********************************************************************************************************************************
// *****

void CheckHUDMode()
{
  if(PrevHUDMode == UFC.MasterMode)	return;

	PrevHUDMode = UFC.MasterMode;

	for(int i=0; i<MAX_UNIQUE_MPDS; i++)
	{
		Mpds[i].Mode = Mpds[i].ProgModeInfo.ModeList[UFC.MasterMode];
	}

	ResetBombProgram(Av.Weapons.CurAGWeap);

	if(UFC.MasterMode != AG_MODE)
		ScanBeamOn = FALSE;
  else if( !( AreMpdsInGivenMode(RADAR_MODE) && (UFC.MasterMode == AG_MODE) ) )
		ScanBeamOn = FALSE;
	else
	{
		ScanBeamOn = TRUE;
	}
}

//***********************************************************************************************************************************
// *****

void CheckMpdDamage()
{
  MpdDamage[MPD_DAMAGE_TEWS] = PlayerPlane->SystemInactive & DAMAGE_BIT_TEWS;
  MpdDamage[MPD_DAMAGE_NAV_FLIR] = PlayerPlane->SystemInactive & DAMAGE_BIT_NAV_FLIR;
  MpdDamage[MPD_DAMAGE_ADC] = PlayerPlane->SystemInactive & DAMAGE_BIT_ADC;
  MpdDamage[MPD_DAMAGE_RADAR] = PlayerPlane->SystemInactive & DAMAGE_BIT_RADAR;
  MpdDamage[MPD_DAMAGE_CEN_COMP] = PlayerPlane->SystemInactive & DAMAGE_BIT_CEN_COMP;
  MpdDamage[MPD_DAMAGE_PACS] = PlayerPlane->SystemInactive & DAMAGE_BIT_PACS;
  MpdDamage[MPD_DAMAGE_TARGET_IR] = PlayerPlane->SystemInactive & DAMAGE_BIT_TARGET_IR;
  MpdDamage[MPD_DAMAGE_L_GEN] =  PlayerPlane->SystemInactive & DAMAGE_BIT_L_GEN;
  MpdDamage[MPD_DAMAGE_R_GEN] =  PlayerPlane->SystemInactive & DAMAGE_BIT_R_GEN;
}

//***********************************************************************************************************************************
// *****

void VirtualMpdSetup(BOOL blank_screens)
{
}

//***********************************************************************************************************************************
// *****

void ResetMpdForNonVirtual()
{
}

//***********************************************************************************************************************************
// *****

void ActivateVirtualMpds()
{
//  VirtualMpdsOn = TRUE;
 // VirtualMpdSetup(TRUE);
}

//***********************************************************************************************************************************
// *****

void DeActivateVirtualMpds()
{
  //VirtualMpdsOn = FALSE;
 // ResetMpdForNonVirtual();
}

//***********************************************************************************************************************************
// *****

void VirtualMpdInteractSetup()
{
}

//***********************************************************************************************************************************
// *****

void ToggleVirtualMpds()
{
	//if(!DrawPopMpds && (Camera1.SubType & COCKPIT_ZOOM_MPD))	// no pop
	///return;

	DrawPopMpds = !DrawPopMpds;
}

//***********************************************************************************************************************************
// *****

extern float JLMFuelFlow;
int JLMDebugWaitForOneG = FALSE;

void DrawMpds(int CurrentCockpit)
{
   CheckHUDMode();
   int which_mpd;


   switch (CurrentCockpit)
   {

	  case VIRTUAL_MODEL:
	  {
		if (MovingMPD)
		{
			ReadMouse();
			which_mpd = FindClosestUnOccupiedMPDSlot(ppCurrMouse.x, ppCurrMouse.y);
			if (which_mpd != -1)
			{
				GrDrawLine(GrBuffFor3D, SlotXY[which_mpd*2], SlotXY[which_mpd*2+1], SlotXY[which_mpd*2]+175, SlotXY[which_mpd*2+1], 0xFC);
				GrDrawLine(GrBuffFor3D, SlotXY[which_mpd*2]+175, SlotXY[which_mpd*2+1], SlotXY[which_mpd*2]+175, SlotXY[which_mpd*2+1]+178, 0xFC);
				GrDrawLine(GrBuffFor3D, SlotXY[which_mpd*2]+175, SlotXY[which_mpd*2+1]+178, SlotXY[which_mpd*2], SlotXY[which_mpd*2+1]+178, 0xFC);
				GrDrawLine(GrBuffFor3D, SlotXY[which_mpd*2], SlotXY[which_mpd*2+1]+178, SlotXY[which_mpd*2], SlotXY[which_mpd*2+1], 0xFC);
			}
		}

		break;
	  }
   }
}

//***********************************************************************************************************************************
// *****

void CleanupMiscMpdSprites()
{
	//GrFreeGrBuff(Buff);
	//GrFreeGrBuff(MonoBuff);

	CleanupMavGbu();
	CleanupFlir();

	ShutdownTextSurface( );

}

//***********************************************************************************************************************************
// *****

void ReleaseMpdButton()
{
  MPD.MpdList[MPD.CurrentMpd].MpdButtons[MPD.CurrentMpdButton].State = RELEASE;
  MPD.MpdList[MPD.CurrentMpd].MpdButtons[MPD.CurrentMpdButton].FramesToDraw = 2;
}

//***********************************************************************************************************************************
// ***** JETTISON CODE

extern JetWeapSelectState;
extern JetPushOn;
extern JetPushStartTime;
extern JetPushState;

int SimJettisonOn;
int JettisonAA;
int JettisonFuel;
int JettisonAG;
int JettAAStartTime;
int JettAGStartTime;

//***********************************************************************************************************************************
// *****

void InitJettisonInfo()
{
  JettAAStartTime = -1;
  JettAGStartTime = -1;
  SimJettisonOn = FALSE;
}
int PlaneAttitudeWithinConstraints(PlaneParams *P);
//***********************************************************************************************************************************
// *****

int JettAG()
{
	int Continue = FALSE;
	int IsAG = FALSE;

	if (!PlaneAttitudeWithinConstraints(PlayerPlane)) return (FALSE);

	if(JettAGStartTime != -1)
	{
	  if(GetTickCount() - JettAGStartTime < 300)
	    return(TRUE);
	  else
        JettAGStartTime = -1;
	}

	for(int i=0; i<MAX_F18E_STATIONS; i++)
    {
   	  if(PlayerPlane->WeapLoad[i].Count > 0)
	  {
		IsAG = FALSE;
		int Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;
		switch (Type)
		{
 		  case DUMB_AG:
		  case GUIDED_AG:
		  case AG_MISSILE:
		  case CLUSTER:
		  case DATA_LINK:
			  IsAG = TRUE;
		}

		if(IsAG)
		{
		  InstantiateBomb(PlayerPlane,i,0);
		  PlayerPlane->WeapLoad[i].Count--;
		  if(PlayerPlane->WeapLoad[i].Count > 0)
		  {
              JettAGStartTime = GetTickCount();
              Continue = TRUE;
		  }
		  else
		  {
			BombStations[i] = FALSE;
            PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
		  }
		}

	  }
	}

	return(Continue);
}

//***********************************************************************************************************************************
// *****

int JettAA()
{
	int Continue = FALSE;
	int IsAA = FALSE;

	if (!PlaneAttitudeWithinConstraints(PlayerPlane)) return (FALSE);

	if(JettAAStartTime != -1)
	{
	  if(GetTickCount() - JettAAStartTime < 300)
	    return(TRUE);
	  else
        JettAAStartTime = -1;
	}

	for(int i=0; i<MAX_F18E_STATIONS; i++)
    {
   	  if(PlayerPlane->WeapLoad[i].Count > 0)
	  {
		IsAA = FALSE;
		int Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;
		switch (Type)
		{
		  case SHORT_RANGE:
		  case MED_RANGE: IsAA = TRUE;
		}

		if(IsAA)
		{
		  InstantiateBomb(PlayerPlane,i,0);
		  PlayerPlane->WeapLoad[i].Count--;
		  if(PlayerPlane->WeapLoad[i].Count <= 0)
            PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
		  if(PlayerPlane->WeapLoad[i].Count > 0)
		  {
 		    Continue = TRUE;
		    if(PlayerPlane->WeapLoad[i].Count > 0)
              JettAAStartTime = GetTickCount();
		  }
		}

	  }
	}

	return(Continue);
}

//***********************************************************************************************************************************
// *****

int JettFuel()
{
	int WingDropped = FALSE;
	int CenterDropped = FALSE;

	if (!PlaneAttitudeWithinConstraints(PlayerPlane)) return (FALSE);

	for(int i=0; i<MAX_F18E_STATIONS; i++)
    {
   	  if(PlayerPlane->WeapLoad[i].Count > 0)
	  {
	    int Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;
		if(Type == FUEL_TANK)
		{
		  InstantiateBomb(PlayerPlane,i,0);
		  PlayerPlane->WeapLoad[i].Count--;
		  PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
		  if( (i == AG_LEFT) || (i == AG_RIGHT) )
		    WingDropped = TRUE;
		  else
            CenterDropped = TRUE;
		}
	  }
	}

	if(WingDropped)
	{
        PlayerPlane->WingDropFuel = 0;
	}

	if(CenterDropped)
	{
        PlayerPlane->CenterDropFuel = 0;
	}

	return(FALSE);
}

//***********************************************************************************************************************************
// *****

void SimJettison()
{
  if(!SimJettisonOn)
	return;

  if(JetPushOn)
  {
    if(GameLoopInTicks - JetPushStartTime > 500)
	{
		JetPushState = JETTISON_OUT;
        JetPush[JetPushState].FramesToDraw = 5;
        JetPushOn = FALSE;
        JetPushStartTime = -1;
	}
  }

  if(JettisonAA)
  {
	MedRangeOnBoard   = FALSE;
    ShortRangeOnBoard = FALSE;
    JettisonAA = JettAA();
  }

  if(JettisonAG)
  {
    JettisonAG = JettAG();
  }

  if(JettisonFuel)
  {
    JettisonFuel = JettFuel();
  }

  if(!JettisonAA && !JettisonAG && !JettisonFuel && !JetPushOn)
  {
    SimJettisonOn = FALSE;
  }
}

//***********************************************************************************************************************************
// ***** JETTISON CODE END


//***********************************************************************************************************************************
// *****

int CanSelectBombStation(int Station)
{
  char *StationStr;
  char *TempStr;
  int  Index;
  int  RetVal;

  RetVal     = TRUE;
  Index      = PlayerPlane->WeapLoad[Station].WeapIndex;
  StationStr = pDBWeaponList[Index].sMPDAbbrev;

  for(int i=0; i<MAX_F18E_STATIONS; i++)
  {
	if(BombStations[i] == TRUE)
	{
	  Index   = PlayerPlane->WeapLoad[i].WeapIndex;
	  TempStr = pDBWeaponList[Index].sMPDAbbrev;
	  if(strcmp(StationStr, TempStr))//!StringCompare(StationStr, TempStr))	//strncmp(StationStr, TempStr)
	  {
		RetVal = FALSE;
		break;
	  }
	}
  }

  if(PlayerPlane->WeapLoad[Station].Status != AG_STAT)
       RetVal = FALSE;

  if(PlayerPlane->WeapLoad[Station].Count <= 0)
       RetVal = FALSE;

  // check for fuel tanks
  int Type = pDBWeaponList[PlayerPlane->WeapLoad[Station].WeapIndex].iWeaponType;
  if(Type == FUEL_TANK)
	RetVal = FALSE;

  if(PlayerPlane->WeapLoad[Station].WeapId == 40)
	RetVal = FALSE;

  return(RetVal);
}

//***********************************************************************************************************************************
// *****

void GetAAWeaponCount(int *ShortCnt, int *MedCnt)
{
  int Index;

  *ShortCnt = 0;
  *MedCnt   = 0;

  for(int i=0; i<MAX_F18E_STATIONS; i++)
  {
   	  if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
  	  {
		 Index = PlayerPlane->WeapLoad[i].WeapIndex;

		 if(pDBWeaponList[Index].iWeaponType == MED_RANGE)
	        *MedCnt = *MedCnt + 1;

		 if(pDBWeaponList[Index].iWeaponType == SHORT_RANGE)
			 *ShortCnt = *ShortCnt + 1;
	  }
   }
}

//***********************************************************************************************************************************
// *****

#define MAV_ID1    3
#define MAV_ID2    4
#define GBU_MIN_ID 25
#define GBU_MAX_ID 28

//***********************************************************************************************************************************
// *****

int IsWeaponGBUType(int Station)
{
 return( (PlayerPlane->WeapLoad[Station].WeapId >= GBU_MIN_ID) &&
	     (PlayerPlane->WeapLoad[Station].WeapId <= GBU_MAX_ID)    );
}

//***********************************************************************************************************************************
// *****

int IsWeaponMaverickType(int Station)
{
 return( (PlayerPlane->WeapLoad[Station].WeapId == MAV_ID1) ||
	     (PlayerPlane->WeapLoad[Station].WeapId == MAV_ID2)    );
}

//***********************************************************************************************************************************
// *****

int IsAGMissleAvailableOnGivenSide(int Type, int Side) // Type is Maveric or GBU15, Side is Left or Right
{
  int Station1,Station2;
  int Station1Selected, Station2Selected;
  int RetVal = FALSE;

  if(Side == RIGHT)
  {
    Station1 = AG_RIGHT;
	Station2 = FRONT_R;
  }
  else
  {
    Station1 = AG_LEFT;
	Station2 = FRONT_L;
  }

  Station1Selected = BombStations[Station1];
  Station2Selected = BombStations[Station2];

  if(!Station1Selected && !Station2Selected) // if nothing is selected
	  return(FALSE);

  if(Type == MAVERIC_TYPE)
  {
     if(Station1Selected)
	 {
	   if(PlayerPlane->WeapLoad[Station1].Status != NO_WEAP_STAT)
		   if(IsWeaponMaverickType(Station1))
		     RetVal = TRUE;
     }

	 if(Station2Selected)
	 {
	   if(PlayerPlane->WeapLoad[Station2].Status != NO_WEAP_STAT)
		   if(IsWeaponMaverickType(Station2))
		     RetVal = TRUE;
 	 }

  }
  else // GBU15_TYPE
  {
	 if(Station1Selected)
	 {
       if(PlayerPlane->WeapLoad[Station1].Status != NO_WEAP_STAT)
	     if(IsWeaponGBUType(Station1))
		    RetVal = TRUE;
	 }

	 if(Station2Selected)
	 {
       if(PlayerPlane->WeapLoad[Station2].Status != NO_WEAP_STAT)
	     if(IsWeaponGBUType(Station2))
		    RetVal = TRUE;
	 }
   }

  return(RetVal);

}

//***********************************************************************************************************************************
// *****

void IncSeq(int Mode)
{
  int Count;
  int BayNum;
  int Index;
  int Type;

  switch (Mode)
  {
    case SRM_MODE:
    {
	   	// bump up to the next one in the list
	    ShortSeqIndex++;
        if(ShortSeqIndex > 3)
		    ShortSeqIndex = 0;

		// keep going through the list if the entry is not valid
		BayNum = FireSeq[ShortSeqIndex];
		Index  = PlayerPlane->WeapLoad[BayNum].WeapIndex;
		Type   = pDBWeaponList[Index].iWeaponType;
		Count = 0;
		while( ((Type != SHORT_RANGE) || (PlayerPlane->WeapLoad[BayNum].Status == NO_WEAP_STAT))
			    && (Count < 4) )
		{
		  ShortSeqIndex++;
		  if(ShortSeqIndex > 3)
		      ShortSeqIndex = 0;

		  BayNum = FireSeq[ShortSeqIndex];
		  Index  = PlayerPlane->WeapLoad[BayNum].WeapIndex;
		  Type   = pDBWeaponList[Index].iWeaponType;
		  Count++;
	 	}

		if(Count >= 4)               // if we went through the list with no success, then
		   ShortRangeOnBoard = FALSE; // there are no more of that type

		break;
    }
    case MRM_MODE:
    {
		MedSeqIndex++;
		if(MedSeqIndex > 7)
			 MedSeqIndex = 0;

		BayNum = FireSeq[MedSeqIndex];
		Index  = PlayerPlane->WeapLoad[BayNum].WeapIndex;
		Type   = pDBWeaponList[Index].iWeaponType;
		Count = 0;
		while( ((Type != MED_RANGE) || (PlayerPlane->WeapLoad[BayNum].Status == NO_WEAP_STAT))
                && (Count < 8) )
		{
		  MedSeqIndex++;
		  if(MedSeqIndex > 7)
			  MedSeqIndex = 0;

   		  BayNum = FireSeq[MedSeqIndex];
		  Index  = PlayerPlane->WeapLoad[BayNum].WeapIndex;
		  Type   = pDBWeaponList[Index].iWeaponType;
		  Count++;
	 	}

		if(Count >= 8)              // if we went through the list with no success, then
		   MedRangeOnBoard = FALSE; // there are no more of that type
    	 break;
    }
	default:
	  break;
   }

}

//***********************************************************************************************************************************
// *****

void SetSRMStatus()
{
  int Index;
  int BayNum;

  for(int i=0; i<MAX_F18E_STATIONS; i++)
  {
   	  if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
  	  {
		 Index = PlayerPlane->WeapLoad[i].WeapIndex;

		 if(pDBWeaponList[Index].iWeaponType == MED_RANGE)
	       PlayerPlane->WeapLoad[i].Status = MRM_STAT;

		 if(pDBWeaponList[Index].iWeaponType == SHORT_RANGE)
		 {
		     BayNum = FireSeq[ShortSeqIndex];
			 if( (i == BayNum) && (ShortRangeOnBoard) )
			   PlayerPlane->WeapLoad[i].Status = RDY_STAT;
	  		else
			   PlayerPlane->WeapLoad[i].Status = STBY_STAT;
		 }

	  }
   }

}

//***********************************************************************************************************************************
// *****

void SetMRMStatus()
{
  int Index;
  int BayNum;

  for(int i=0; i<MAX_F18E_STATIONS; i++)
  {
   	  if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
  	  {
		 Index = PlayerPlane->WeapLoad[i].WeapIndex;

		 if(pDBWeaponList[Index].iWeaponType == SHORT_RANGE)
	       PlayerPlane->WeapLoad[i].Status = SRM_STAT;

		 if(pDBWeaponList[Index].iWeaponType == MED_RANGE)
		 {
		     BayNum = FireSeq[MedSeqIndex];
			 if( (i == BayNum) && (MedRangeOnBoard) )
			   PlayerPlane->WeapLoad[i].Status = RDY_STAT;
			 else
			   PlayerPlane->WeapLoad[i].Status = STBY_STAT;
		 }

	  }

  }

}

//***********************************************************************************************************************************
// *****

void ResetMissleStatus()
{
 int Index;

 for(int i=0; i<MAX_F18E_STATIONS; i++)
 {
   	  if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
  	  {
		 Index = PlayerPlane->WeapLoad[i].WeapIndex;

		 if(pDBWeaponList[Index].iWeaponType == SHORT_RANGE)
	       PlayerPlane->WeapLoad[i].Status = SRM_STAT;

	     if(pDBWeaponList[Index].iWeaponType == MED_RANGE)
	       PlayerPlane->WeapLoad[i].Status = MRM_STAT;
  	  }

  }

}

//***********************************************************************************************************************************
// *****

int IsMissleReady(int Type)
{

 int Index;

 for(int i=0; i<MAX_F18E_STATIONS; i++)
 {
   	  if(PlayerPlane->WeapLoad[i].Status == RDY_STAT)
  	  {
		 Index = PlayerPlane->WeapLoad[i].WeapIndex;

		 if(Type == SRM_MODE)
		 {
		   if(pDBWeaponList[Index].iWeaponType == SHORT_RANGE)
	        return(TRUE);
		 }
		 else
		 {
		   if(pDBWeaponList[Index].iWeaponType == MED_RANGE)
	         return(TRUE);
		 }
  	  }
  }

  return(FALSE);

}

//***********************************************************************************************************************************
// *****

void ProcessAim7Fire(int Station)
{
	/*
	int RadarMode = GetAARadarSubMode();

   if( IsRadarModeOn(AA_RADAR_MODE) && ( (RadarMode == DTWS_MODE) || (RadarMode == NDTWS_MODE) ) )
   {
     if( (PlayerPlane->WeapLoad[Station].WeapId == 34) || (PlayerPlane->WeapLoad[Station].WeapId == 35) )
	 {
       Aim7Fired();
	 }
   }
   */
}

//***********************************************************************************************************************************
// *****

void ProcessAmraamFire(int Station,int *TargetFound, PlaneParams **Target)
{
	/*
   *TargetFound = FALSE;

   int RadarMode = GetAARadarSubMode();

   if( IsRadarModeOn(AA_RADAR_MODE) && (RadarMode == DTWS_MODE) )
   {
     if((PlayerPlane->WeapLoad[Station].WeapId == 38))
	 {
       *TargetFound = TRUE;
	   *Target = GetTargetForAmraam();
	 }
   }
   */
}

//***********************************************************************************************************************************
// *****

void SrmSelect()
{
  if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) return;  // do nothing if in easy mode

  if(UFC.MasterMode != AG_MODE)
  {
    CurrWeapMode = SRM_MODE;
    SetSRMStatus();
  }
}

//***********************************************************************************************************************************
// *****

void MrmSelect()
{
  if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) return;  // do nothing if in easy mode

  if(UFC.MasterMode != AG_MODE)
  {
	if(!MpdDamage[MPD_DAMAGE_CEN_COMP])  // damage to central computer disables med range missles
	{
      CurrWeapMode = MRM_MODE;
      SetMRMStatus();
	}
  }
}

//***********************************************************************************************************************************
// *****


void GunSelect()
{
   if((CurrWeapMode == SRM_MODE) || (CurrWeapMode == MRM_MODE) )
  		ResetMissleStatus();

   if (UFC.MasterMode == AG_MODE)
   {
		 if(CurrWeapMode != GUN_MODE)
		 {
			CurrWeapMode = GUN_MODE;
			UFC.AGSubMode = GUNS_AG;
		 }
		 else
		 {
			CurrWeapMode = WEAP_OFF;
			UFC.AGSubMode = CDIP_AG;
		 }
   }
   else

   if (UFC.MasterMode == AA_MODE)
   {
		CurrWeapMode = GUN_MODE;

		if (PlayerPlane->AADesignate == NULL)
		{
			UFC.AASubMode = AA_GUN_FNL;
			//if (AreMpdsInGivenMode(AA_RADAR_MODE))
			//  	SetAutoAcq(GUN_RDR_MODE);
		}
		else
		{
			if (++UFC.AASubMode > 2) UFC.AASubMode = 0;
		}
   }
}

/*

void GunSelect()
{
   if( (CurrWeapMode == SRM_MODE) || (CurrWeapMode == MRM_MODE) )
  	 ResetMissleStatus();

   if(UFC.MasterMode == AG_MODE)
   {

     if(CurrWeapMode != GUN_MODE)
	 {
      CurrWeapMode = GUN_MODE;
      UFC.AGSubMode = GUNS_AG;
	 }
     else
	 {
      CurrWeapMode = WEAP_OFF;
      UFC.AGSubMode = CDIP_AG;
	 }
   }
   else if(UFC.MasterMode == AA_MODE)
   {
     CurrWeapMode = GUN_MODE;
	 if(AreMpdsInGivenMode(AA_RADAR_MODE))
		SetAutoAcq(GUN_RDR_MODE);
   }

}
*/

//***********************************************************************************************************************************
// *****

void SequenceMissle()
{
  int BayNum;
  int Type;
  int Index;
  int ShouldFire;

  ShouldFire = TRUE;

  if(CurrWeapMode == MRM_MODE)
  {
	if(!MedRangeOnBoard)
	  ShouldFire = FALSE;
	else
	  BayNum = FireSeq[MedSeqIndex];
  }
  else if(CurrWeapMode == SRM_MODE)
  {
	if(!ShortRangeOnBoard)
	  ShouldFire = FALSE;
	else
	  BayNum = FireSeq[ShortSeqIndex];
  }

  if(ShouldFire)
  {
    Index  = PlayerPlane->WeapLoad[BayNum].WeapIndex;
	Type   = pDBWeaponList[Index].iWeaponType;
	if(cPlayerLimitWeapons)
	{
		PlayerPlane->WeapLoad[BayNum].Status = NO_WEAP_STAT;
		PlayerPlane->WeapLoad[BayNum].Count--;
	}
	IncSeq(CurrWeapMode);
	if(CurrWeapMode == SRM_MODE)
		SetSRMStatus();
	else
		SetMRMStatus();

	int TargetFound=0;
	PlaneParams *Target = NULL;

	ProcessAmraamFire(BayNum,&TargetFound, &Target);

	if(TargetFound)
      InstantiateMissile(PlayerPlane, BayNum, NULL, 0, Target);
	else
	 InstantiateMissile(PlayerPlane, BayNum);

	ProcessAim7Fire(BayNum);

	if(TargetFound && (Target != NULL ) )
      AddMissleToChasingMissleList(Target);
    else
	{
	  if(PlayerPlane->AADesignate != NULL)
	    AddMissleToChasingMissleList(PlayerPlane->AADesignate);
	}

  }
}

//***********************************************************************************************************************************
// *****

void Button1CPACSUpdate()
{
  if( (PlayerPlane->WeapLoad[GUN_STATION].Count > 0) && (UFC.MasterArmState == ON) )
  {
	 if(cPlayerLimitWeapons)
		 PlayerPlane->WeapLoad[GUN_STATION].Count-=2;
	 InstantiateBullet(PlayerPlane,TRUE);
  }
}

//***********************************************************************************************************************************
// *****

extern  MavInfo;

int FireDebug;
char FireStr[20];
int FiredType;

void DrawFireInfo()
{
		  switch(FireDebug)
		  {
		     case 0: sprintf(FireStr,"RANDOM KEYHOLE FAIL"); break;
		     case 1: sprintf(FireStr,"GROUND TARGET"); break;
		     case 2: sprintf(FireStr,"PLANE"); break;
			 case 3: sprintf(FireStr,"VEHICLE"); break;
		     case 4: sprintf(FireStr,"LOCATION"); break;
		     case 5: sprintf(FireStr,"NO TARGET"); break;
		  }

		  if(FiredType)
		  {
			  sprintf(Str1,"GBU : %s",FireStr);
			  DrawTextAtLocation(20,20,Str1,61);
		  }
		  else
		  {
			 sprintf(Str1,"MAV : %s",FireStr);
			 DrawTextAtLocation(20,20,Str1,61);
		  }
}

void Button2CPACSUpdate()
{
  Button2Press();
  return;
}

//********************************************************************************************/
//***********

void CopyTewsThreats()
{
  if(Threats.NumThreats <= 0)
  {
    PrevThreats.NumThreats = 0;
    return;
  }

  ThreatEntryType *PrevThreatPtr  = &PrevThreats.List[0];
  ThreatEntryType *CurThreat      = &Threats.List[0];

  PrevThreats.NumThreats = Threats.NumThreats;
  int Index = Threats.NumThreats;
  while(Index-- > 0)
  {
		memcpy(PrevThreatPtr,CurThreat,sizeof(ThreatEntryType));
    *PrevThreatPtr = *CurThreat;

    PrevThreatPtr++;
    CurThreat++;
  }

}

extern Debug2;

//********************************************************************************************/
//***********

int IsPlaneInPrevTews(PlaneParams *P)
{
  ThreatEntryType *PrevThreatPtr = &PrevThreats.List[0];

  int Index = PrevThreats.NumThreats;
  while(Index-- > 0)
  {
    if(PrevThreatPtr->P == P)
	{
 	  return(TRUE);
	}

	PrevThreatPtr++;
  }
  return(FALSE);
}

//********************************************************************************************/
//***********

int IsVehicleInPrevTews(MovingVehicleParams *V)
{
  ThreatEntryType *PrevThreatPtr = &PrevThreats.List[0];

  int Index = PrevThreats.NumThreats;
  while(Index-- > 0)
  {
    if(PrevThreatPtr->V == V)
	{
 	  return(TRUE);
	}

	PrevThreatPtr++;
  }
  return(FALSE);
}

//********************************************************************************************/
//***********

int IsMissleInPrevTews(WeaponParams *W)
{
  ThreatEntryType *PrevThreatPtr = &PrevThreats.List[0];
  int EasyModeOn = (g_Settings.gp.nAARADAR == GP_TYPE_CASUAL);

  int WeaponIndex = AIGetWeaponIndex(W->Type->TypeNumber);
  int Seeker = pDBWeaponList[WeaponIndex].iSeekerType;

  if( (W->Kind != MISSILE) || (W->iTargetType != TARGET_PLANE) ||
	  ((PlaneParams *)W->pTarget != PlayerPlane) ||
	  !(W->Flags & FINAL_PHASE) || (Seeker != 1) )
		   return(TRUE);

  int Index = PrevThreats.NumThreats;
  while(Index-- > 0)
  {
    if(PrevThreatPtr->W == W)
	{
	  return(TRUE);
	}

    PrevThreatPtr++;
  }
  return(FALSE);
}

//********************************************************************************************/
//***********

int IsSpInPrevTews(InfoProviderInstance *SP)
{
  ThreatEntryType *PrevThreatPtr = &PrevThreats.List[0];

  int Index = PrevThreats.NumThreats;
  while(Index-- > 0)
  {
    if(PrevThreatPtr->SP == SP)
		return(TRUE);

	PrevThreatPtr++;
  }
  return(FALSE);
}

//********************************************************************************************/
//***********

int IsSwInPrevTews(AAWeaponInstance *SW)
{
  ThreatEntryType *PrevThreatPtr = &PrevThreats.List[0];

  int Index = PrevThreats.NumThreats;
  while(Index-- > 0)
  {
    if(PrevThreatPtr->SW == SW)
		return(TRUE);

	PrevThreatPtr++;
  }
  return(FALSE);
}

//********************************************************************************************/
//***********

int ObjectHasLock(int *Type)
{
	ThreatEntryType *ThreatPtr = &Threats.List[0];

	int Index = Threats.NumThreats;
	while(Index-- > 0)
	{
		if(ThreatPtr->HasLock)
		{
			*Type = ThreatPtr->Category;
			return(TRUE);
		}
		ThreatPtr++;
	}
	return(FALSE);
}

//********************************************************************************************/
//***********

int IsMissleFiredAtPlayer()
{
	// JLM may need to check if SAM that is firing is still in the threat list
	return( (PlanesFiring.NumPlanes > 0) || (SAMObjFiring.NumSAMObjs > 0) || (VehiclesFiring.NumVehicles > 0) );
}

extern Debug2;

//********************************************************************************************/
//***********

void SoundTewsWarning()
{
	if(Threats.NumThreats <= 0)
	{
      // TURN OFF SOUND IF IT IS STILL ON
	  if (g_dwMissileLaunchSound)
	  {
			// Stop the missile launch sound from playing continuously.
			SndEndSound(g_dwMissileLaunchSound);
			g_dwMissileLaunchSound=0;
	  }
	  return;
	}

	int iLevel = g_iSoundLevelCaution-10;
	if (iLevel<0) iLevel=0;

	int InList = TRUE;

	// CATEGORY 1 -- IF NEW PLANE SAM SITE OR MISSLE HAS BEEN DETECTED, GIVE TONE.
	ThreatEntryType *CurThreatPtr = &Threats.List[0];

	int WarningTone = -1;

	int Index = Threats.NumThreats;

	while(Index-- > 0)
	{
		switch(CurThreatPtr->Type)
		{
			case TEWS_MISSLE_THREAT:   InList = IsMissleInPrevTews(CurThreatPtr->W);                                                     break;
			case TEWS_SAM_PROV_THREAT: InList = IsSpInPrevTews(CurThreatPtr->SP);     WarningTone = SND_TEWS_WARNING_NEW_GROUND_THREAT;  break;
			case TEWS_SAM_WEAP_THREAT: InList = IsSwInPrevTews(CurThreatPtr->SW);     WarningTone = SND_TEWS_WARNING_NEW_GROUND_THREAT;  break;
			case TEWS_PLANE_THREAT:    InList = IsPlaneInPrevTews(CurThreatPtr->P);   WarningTone = SND_TEWS_WARNING_NEW_AI_THREAT;      break;
			case TEWS_VEH_THREAT:      InList = IsVehicleInPrevTews(CurThreatPtr->V); WarningTone = SND_TEWS_WARNING_NEW_GROUND_THREAT;  break;
		}

		if(!InList) break;

		CurThreatPtr++;
	}

	if (!InList)
	{
		// New Emitter has been detected so play short tone once.
		if(WarningTone != -1)
		   SndQueueSound(WarningTone,1,iLevel,-1,-1,20);
	}

	// CATEGORY 2 -- OTHER PLANE OR SAM SITE HAS LOCK -- LOOP SOUND 5 TIMES, WAIT 10 SEC.
	int Type;

	if(ObjectHasLock(&Type))
	{
		if(TewsLockTimer == -1)
		{
			// Radar Lockon Sound - Play it Five Times
			if(!g_dwMissileLaunchSound)
			{
				if(Type == TEWS_SAM_WEAP_THREAT)
			    g_dwMissileWarnSound = SndQueueSound(SND_TEWS_WARNING_AAA,5,iLevel,-1,-1,20);
				else
			    g_dwMissileWarnSound = SndQueueSound(SND_TEWS_WARNING_MISSILE_ALERT,5,iLevel,-1,-1,20);

				TewsLockTimer = GameLoopInTicks;
			}
		}
		else
		{
			if(GameLoopInTicks - TewsLockTimer > 6000)
			{
				if(g_dwMissileWarnSound)
				{
		  		SndEndSound(g_dwMissileWarnSound);
	  			g_dwMissileWarnSound=0;
				}
				TewsLockTimer = -1;
			}
		}
	}
	else
	{
		if(g_dwMissileWarnSound)
		{
		  SndEndSound(g_dwMissileWarnSound);
	  	g_dwMissileWarnSound=0;
		}
	}



	// CATEGORY 3 -- OTHER PLANE OR SAM SITE LAUNCH DETECTED
	if(IsMissleFiredAtPlayer())
	{
		// No missile launch sound is currently playing, so start
		// this one playing continuously.
		if (!g_dwMissileLaunchSound)
		{
			g_dwMissileLaunchSound=SndQueueSound(SND_TEWS_WARNING_MISSILE_LAUNCH,0,iLevel,-1,-1,20);
		}
	}
	else
	{
		TewsMissleLaunchWarnOn = FALSE;
		if (g_dwMissileLaunchSound)
		{
			// Stop the missile launch sound from playing continuously.
			SndEndSound(g_dwMissileLaunchSound);
			g_dwMissileLaunchSound=0;
		}
	}

	if(g_dwMissileLaunchSound && g_dwMissileWarnSound)
	{
		SndEndSound(g_dwMissileWarnSound);
	  g_dwMissileWarnSound=0;
	}

	if(g_dwMissileWarnSound)
	{
		if(!SndIsSoundPlaying(g_dwMissileWarnSound))
		{
			KillSound( &g_dwMissileWarnSound );
		}
	}
	if(g_dwMissileLaunchSound)
	{
		if(!SndIsSoundPlaying(g_dwMissileLaunchSound))
		{
			KillSound( &g_dwMissileLaunchSound );
		}
	}
}

//********************************************************************************************/
//***********

void ProcessEnemyRadarActivity()
{
  if(MpdDamage[MPD_DAMAGE_TEWS])
  {
	Threats.NumThreats      = 0;
	PlanesFiring.NumPlanes  = 0;
	SAMObjFiring.NumSAMObjs = 0;
	VehiclesFiring.NumVehicles = 0;

	CurThreat = &Threats.List[0];

	SoundTewsWarning();
	return;
  }

  CopyTewsThreats();           // first make a copy ofthe previous frames TEWS list

  Threats.NumThreats      = 0;
  PlanesFiring.NumPlanes  = 0;
  SAMObjFiring.NumSAMObjs = 0;
  VehiclesFiring.NumVehicles = 0;

  CurThreat               = &Threats.List[0];

	SimTewsBlink();

  ProcessMissles();
  ProcessEnemyPlanes();
  ProcessSAMProviders();
  ProcessSAMWeapons();
	ProcessVehicles();
  SetAndSortTewsThreats();

	SimHarmSP();

  CheckTewsWarnings();
  SoundTewsWarning();

  SimCounterMeasures();
  SimChaffDrop();

  //JLM TEMP MOVE
  SimJettison();
}






//********************************************************************************************/
//********************************************************************************************/
//********************************************************************************************/
//********************************************************************************************/
//********************************************************************************************/



// This is how the Cockpit hotspots work.
//  There are 6 mpds hotspots.  A mpdhotspot includes the bounding box of the mpd and the spheressurronding buttons.
//  each hotspot has a bounding sphere as a qucik out check.
//	So first we check in the click is in the bounding sphere, then we check if it hits any of the hotspots( buttons, mpd, etc)
//
// Buttons are labels 0 -19  ccw starting from the top button on the left side

typedef struct _CockpitHotSpot
{
	hspot MPDA[24];					// first hotspot is always a bounding sphere for the rest of the hotspots
	hspot MPDB[2];
	hspot MPDC[24];
	hspot MPDD[2];
	hspot MPDE[24];
	hspot MPDF[2];
	hspot UFC[2];
	hspot CockpitButtons[41];//34];
	hspot HUDFLIR[2];

	hspot MPDPopUp[24];


}CockpitHotSpot;


int RayHitSphere( FPoint &pos, FPoint &nvector, FPoint &Center, float radius );
int RayHitPoly( FPoint &Start,FPoint &MouseVect, int numpts, hspot *pHSpot );
FPoint HeadOffset;

hspot *CheckHotSpotList( int num, hspot *pHSpot, FPoint *pMouseVector);

void parseHotSpot( FILE *stream, hspot *pHotSpot );
void ParseMPDPopUpHotspotFile( char *file);
void ParseHotspotFile( char *file);

CockpitHotSpot CockpitHotSpots;

void InitVCHotspots( void)
{
	switch (g_Settings.gr.dwGraph & (GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT))
	{
		case GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT:
		default:
			ParseHotspotFile( "Vitpit.dat");
			break;

		case 0:
			ParseHotspotFile( "Vitpitm.dat");
			break;

		case GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT:
			ParseHotspotFile( "Vitpitl.dat");
			break;
	}

	ParseMPDPopUpHotspotFile( "vframe.dat");
}



hspot *CheckHotSpotList( int num, hspot *pHSpot, FPoint *pMouseVector)
{
	int i;

	if( pHSpot->Type & HST_SPHERE)
	{
		if( !RayHitSphere( HeadOffset, *pMouseVector, pHSpot->Center, pHSpot->Radius ))
			return NULL;
		else
		{
			pHSpot++;
			i = 1;
			while( (i++)<num)
			{
				if( pHSpot->Type & HST_SPHERE)
				{
					if( RayHitSphere( HeadOffset, *pMouseVector, pHSpot->Center, pHSpot->Radius  ))
						return( pHSpot);
				}
				else if( pHSpot->Type & HST_POLY)
				{
					if( RayHitPoly(HeadOffset, *pMouseVector, 4, pHSpot) )
						return( pHSpot);
				}

			pHSpot++;
			}

		}
	}

	if( pHSpot->Type & HST_POLY)
	{
		if( RayHitPoly(HeadOffset, *pMouseVector, 4, pHSpot) )
			return( pHSpot);
	}
	return NULL;


}



int RayHitPoly(FPoint &start, FPoint &MouseVect, int numpts, hspot *pHSpot)
{
	FPoint A, B, Normal;
	FPoint StartPos, EndPos;
	FPoint PointonPlane;
	float currentdist, nextdist;
	int currentin, nextin;

	StartPos = start;
	EndPos = StartPos;

	EndPos.AddScaledVector(500 FEET,MouseVect);


	A.X = pHSpot->LR.X;
	A.Y = pHSpot->UL.Y;
	A.Z = pHSpot->UL.Z;
	A -=pHSpot->UL;

	B.X = pHSpot->UL.X;
	B.Y = pHSpot->LR.Y;
	B.Z = pHSpot->LR.Z;
	B -= pHSpot->UL;

	Normal = pHSpot->Normal;


	FPoint StartVect = StartPos;
	StartVect -= pHSpot->UL;//pHSpot->PolyPts[0];
	FPoint EndVect = EndPos;
	EndVect   -= pHSpot->UL;//pHSpot->PolyPts[0];

	currentdist  =  Normal * StartVect;
    currentin	 =  (currentdist >= 0);

	nextdist = Normal * EndVect;
    nextin   = (nextdist >= 0);

	if( nextin != currentin)
	{
		float scale;

		scale    = (currentdist) / (nextdist- currentdist);
		scale = fabs(scale);

		PointonPlane.X = (scale * (EndPos.X - StartPos.X)) + StartPos.X;
		PointonPlane.Y = (scale * (EndPos.Y - StartPos.Y)) + StartPos.Y;
		PointonPlane.Z = (scale * (EndPos.Z - StartPos.Z)) + StartPos.Z;

		PointonPlane -= pHSpot->UL;//pHSpot->PolyPts[0];

		float alen = A.Length();
		float blen = B.Length();
		A.Normalize();
		B.Normalize();

		pHSpot->tu = (PointonPlane * A)/ alen;
		pHSpot->tv = (PointonPlane * B)/ blen;

		//test
		if( pHSpot->tu>= 0.0f && pHSpot->tu <=1.0f && pHSpot->tv>=0.0f && pHSpot->tv<=1.0f)
		{
			return 1;
		}
	}
	pHSpot->tu = 0.0f;
	pHSpot->tv = 0.0f;
	return 0;
}




int RayHitSphere( FPoint &pos, FPoint &nvector, FPoint &Center, float radius )
{
	FPoint Dist;
	float  v;
	float c2;

	if( radius)
	{
		Dist = Center;
		Dist -= pos;

		v  = Dist * nvector;
		c2 = Dist * Dist;				// hyplength^2

		if( (radius*radius - ( c2 - v*v)) <0)
			return 0;
		else
			return 1;
	}
	return 0;

}

int RayHitSphere( FPointDouble &pos, FPointDouble &nvector, FPointDouble &Center, double radius )
{
	FPointDouble Dist;
	double  v;
	double c2;

	if( radius)
	{
		Dist.MakeVectorToFrom(Center,pos);

		v  = Dist * nvector;
		c2 = Dist * Dist;				// hyplength^2

		if( (radius*radius - ( c2 - v*v)) <0)
			return 0;
		else
			return 1;
	}
	return 0;

}


hspot *CheckMpdPopUp(float sx, float sy )
{
	hspot *pHSpot= NULL;
	static hspot MpdHSpot;
	FMatrix Mat;
	FPoint mousevector;
	int oldmidx, oldmidy;

	oldmidx = GrBuffFor3D->MidX;
	oldmidy = GrBuffFor3D->MidY;



	Mat.Identity(); // Test for now.
	//Mat = PlayerPlane->Attitude;
	//Mat	*= ViewMatrix;
	//Mat.Transpose();


	//Get Point and Unit Vecotr;
	float x = MpdPopUpPolys[0].Points[0].X+ 70;
	float y = MpdPopUpPolys[0].Points[0].Y- 70;


	GrBuffFor3D->MidX = x + GrBuffFor3D->MidX;
	GrBuffFor3D->MidY = -y + GrBuffFor3D->MidY;

	mousevector.X = sx - GrBuffFor3D->MidX ;
	mousevector.Y = GrBuffFor3D->MidY - sy ;
	mousevector.Z = EyeToScreen;//-512;
	mousevector *= Mat;
	mousevector.Normalize();

	HeadOffset = 0.0f;

	//DebugShowHotSpots( );


	pHSpot = &CockpitHotSpots.MPDPopUp[0];
	if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 0;
		pHSpot->Poly = &MpdPopUpPolys[0];
		GrBuffFor3D->MidX = oldmidx;
		GrBuffFor3D->MidY = oldmidy;
		return pHSpot;
	}



	GrBuffFor3D->MidX = oldmidx;
	GrBuffFor3D->MidY = oldmidy;

	x = MpdPopUpPolys[1].Points[0].X+ 70;
	y = MpdPopUpPolys[1].Points[0].Y- 70;


	GrBuffFor3D->MidX = x + GrBuffFor3D->MidX;
	GrBuffFor3D->MidY = -y + GrBuffFor3D->MidY;

	mousevector.X = sx - GrBuffFor3D->MidX ;
	mousevector.Y = GrBuffFor3D->MidY - sy ;
	mousevector.Z = EyeToScreen;//-512;
	mousevector.RotateInto( mousevector, Mat);
	mousevector.Normalize();

	//DebugShowHotSpots( );

	pHSpot = &CockpitHotSpots.MPDPopUp[0];
	if( (pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector)) )
	{
		pHSpot->MpdNum = 1;
		pHSpot->Poly = &MpdPopUpPolys[1];


		//this is hack to get the samve hspot vars as the VC.
		memcpy(&MpdHSpot, pHSpot, sizeof( hspot));
		pHSpot= &MpdHSpot;
		pHSpot->Var +=20;

		GrBuffFor3D->MidX = oldmidx;
		GrBuffFor3D->MidY = oldmidy;
		return pHSpot;

	}

/*	REMOVED MIDDLE HotSpot

	GrBuffFor3D->MidX = oldmidx;
	GrBuffFor3D->MidY = oldmidy;

	x = MpdPopUpPolys[2].Points[0].X+ 70;
	y = MpdPopUpPolys[2].Points[0].Y- 70;


	GrBuffFor3D->MidX = x + GrBuffFor3D->MidX;
	GrBuffFor3D->MidY = -y + GrBuffFor3D->MidY;

	mousevector.X = sx - GrBuffFor3D->MidX ;
	mousevector.Y = GrBuffFor3D->MidY - sy ;
	mousevector.Z = EyeToScreen;//-512;
	mousevector.RotateInto( mousevector, Mat);
	mousevector.Normalize();

	//DebugShowHotSpots( );


	pHSpot = &CockpitHotSpots.MPDPopUp[0];
	if( (pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector)) )
	{
		pHSpot->MpdNum = 2;
		//this is hack to get the samve hspot vars as the VC.
		memcpy(&MpdHSpot, pHSpot, sizeof( hspot));
		pHSpot= &MpdHSpot;
		pHSpot->Var +=40;

		GrBuffFor3D->MidX = oldmidx;
		GrBuffFor3D->MidY = oldmidy;
		return pHSpot;
	}
*/


	GrBuffFor3D->MidX = oldmidx;
	GrBuffFor3D->MidY = oldmidy;

	return NULL;
}

hspot *CheckMouseMPD( int mpdnum, float sx, float sy )
{
	hspot *pHSpot=NULL;
	FPoint mousevector;
	FMatrix Mat;



	//Mat.Identity(); // Test for now.
	Mat = PlayerPlane->Attitude;
	Mat	*= ViewMatrix;
	Mat.Transpose();


	//Get Point and Unit Vecotr;
	mousevector.X = sx - GrBuffFor3D->MidX ;
	mousevector.Y = GrBuffFor3D->MidY - sy ;
	mousevector.Z = EyeToScreen;//-512;
	mousevector *= Mat;
	mousevector.Normalize();

	HeadOffset.SetValues(VC.xdelta,VC.ydelta,VC.zdelta);
	HeadOffset *= -100.0f;

	switch( mpdnum)
	{
		case 0:
			pHSpot = &CockpitHotSpots.MPDA[0];
			if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
				pHSpot->MpdNum = 0;
			break;

		case 3:
			pHSpot = &CockpitHotSpots.MPDB[0];
			if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
				pHSpot->MpdNum = 3;
			break;

		case 1:
			pHSpot = &CockpitHotSpots.MPDC[0];
			if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
				pHSpot->MpdNum = 1;
			break;

		case 4:
			pHSpot = &CockpitHotSpots.MPDD[0];
			if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
				pHSpot->MpdNum = 4;
			break;

		case 2:
			pHSpot = &CockpitHotSpots.MPDE[0];
			if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
				pHSpot->MpdNum = 2;
			break;

		case 5:
			pHSpot = &CockpitHotSpots.MPDF[0];
			if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
				pHSpot->MpdNum = 5;
			break;

		case 6:
			pHSpot = &CockpitHotSpots.UFC[0];
			if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
				pHSpot->MpdNum = 6;
			break;

		case 7:
			pHSpot = &CockpitHotSpots.HUDFLIR[0];
			if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
				pHSpot->MpdNum = 7;
			break;


	}


	return pHSpot;
}



hspot *CheckMouseClick( float sx, float sy )
{
	hspot *pHSpot=NULL;
	FPoint mousevector;
	FMatrix Mat;



	//Mat.Identity(); // Test for now.
	Mat = PlayerPlane->Attitude;
	Mat	*= ViewMatrix;
	Mat.Transpose();


	//Get Point and Unit Vecotr;
	mousevector.X = sx - GrBuffFor3D->MidX ;
	mousevector.Y = GrBuffFor3D->MidY - sy ;
	mousevector.Z = EyeToScreen;//-512;
	mousevector *= Mat;
	mousevector.Normalize();

	HeadOffset.SetValues(VC.xdelta,VC.ydelta,VC.zdelta);
	HeadOffset *= -100.0f;


	if( DrawPopMpds)			//pop mpds
	{
		pHSpot = CheckMpdPopUp( sx,  sy );
		return pHSpot;		// no cockpit interatcion when in popup mode
	}



	pHSpot = &CockpitHotSpots.MPDA[0];
	if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 0;
		return pHSpot;
	}

	if( Av.Ufc.Mode == UFC_MDI_MODE)
	{
		pHSpot = &CockpitHotSpots.UFC[0];
		if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
		{
			pHSpot->MpdNum = 6;
			return pHSpot;
		}
	}



	pHSpot = &CockpitHotSpots.MPDB[0];
	if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 3;
		return pHSpot;
	}

	pHSpot = &CockpitHotSpots.MPDC[0];
	if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 1;
		return pHSpot;
	}

	pHSpot = &CockpitHotSpots.MPDD[0];
	if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 4;
		return pHSpot;
	}

	pHSpot = &CockpitHotSpots.MPDE[0];
	if( pHSpot = CheckHotSpotList( 22, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 2;
		return pHSpot;
	}

	pHSpot = &CockpitHotSpots.MPDF[0];
	if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 5;
		return pHSpot;
	}


	pHSpot = &CockpitHotSpots.HUDFLIR[0];
	if( pHSpot = CheckHotSpotList( 2, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = 7;
		return pHSpot;
	}



	pHSpot = &CockpitHotSpots.CockpitButtons[0];
	if( pHSpot = CheckHotSpotList( 41, pHSpot, &mousevector) )
	{
		pHSpot->MpdNum = -1;  // Not An MPD
		return pHSpot;
	}

	return 0;
}




void SetupPolyBuff(  GrBuffPolygon  *pMPDPoly, hspot *pHSpot )
{
	double ulx, uly, lrx, lry, ulz, lrz;

	ulx = pHSpot->UL.X;
	uly = pHSpot->UL.Y;
	lrx = pHSpot->LR.X;
	lry = pHSpot->LR.Y;
	lrz = pHSpot->LR.Z;
	ulz = pHSpot->UL.Z;

	pMPDPoly->Points[0].SetValues( ulx,	uly, 	ulz);
	pMPDPoly->Points[1].SetValues( lrx,	uly,	ulz);
	pMPDPoly->Points[2].SetValues( lrx,	lry,	lrz);
	pMPDPoly->Points[3].SetValues( ulx,	lry,	lrz);
	pMPDPoly->x2D = 0;
	pMPDPoly->y2D = 0;
	pMPDPoly->Width2D = MPD_WIDTH;
	pMPDPoly->Height2D= MPD_HEIGHT;

}

void InitPolyMpds( void)
{
	hspot *pHSpot;
	GrBuffPolygon *pMPDPoly;

	//Note:  swaped y z  and -x  from Max's MultiGen Coords Sys * FEET Scale

	pMPDPoly = &MpdPolys[0];
	pHSpot = &CockpitHotSpots.MPDA[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pMPDPoly->Width2D = 139;
	pHSpot->Poly = pMPDPoly;


	pMPDPoly = &MpdPolys[1];
	pHSpot = &CockpitHotSpots.MPDB[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pMPDPoly->Width2D = 177;				// the ufc is wider than all the other mpds
	pHSpot->Poly = pMPDPoly;


	pMPDPoly = &MpdPolys[2];
	pHSpot = &CockpitHotSpots.MPDC[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pMPDPoly->Width2D = 139;
	pHSpot->Poly = pMPDPoly;


	pMPDPoly = &MpdPolys[3];
	pHSpot = &CockpitHotSpots.MPDD[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pMPDPoly->Width2D  = 108;
	pMPDPoly->Height2D = 78;
	pHSpot->Poly = pMPDPoly;

	pMPDPoly = &MpdPolys[4];
	pHSpot = &CockpitHotSpots.MPDE[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pHSpot->Poly = pMPDPoly;

	//pMPDPoly = &MpdPolys[5];
	//pHSpot = &CockpitHotSpots.MPDF[1];
	//SetupPolyBuff( pMPDPoly, pHSpot );
	//pHSpot->Poly = pMPDPoly;

	pMPDPoly = &MpdPolys[6];
	pHSpot = &CockpitHotSpots.HUDFLIR[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pHSpot->Poly = pMPDPoly;


	pMPDPoly = &MpdPolys[7];
	pHSpot = &CockpitHotSpots.UFC[1];
	SetupPolyBuff( pMPDPoly, pHSpot );
	pHSpot->Poly = pMPDPoly;




}

void SetBuffPoly(  GrBuffPolygon  *pMpdPoly, int sx, int sy, int width, int height)
{
	pMpdPoly->Points[0].SetValues( sx,			sy,			EyeToScreen );
	pMpdPoly->Points[1].SetValues( sx+width,	sy,			EyeToScreen );
	pMpdPoly->Points[2].SetValues( sx+width,	sy-height,	EyeToScreen );
	pMpdPoly->Points[3].SetValues( sx,			sy-height,	EyeToScreen );



	pMpdPoly->Offset.SetValues( 0.0f, 0.0f, 0.0f);

	pMpdPoly->x2D = 0;
	pMpdPoly->y2D = 0;
	pMpdPoly->Width2D = width;
	pMpdPoly->Height2D= height;
}

void InitPopUpMpds( void)
{
	GrBuffPolygon  *pMpdPoly;
	int midx, midy;


	midx = GrBuffFor3D->MidX;
	midy = GrBuffFor3D->MidY;		//midy not init yet??
	if( ScreenSize.cy == 600)
		midy = 146;
	else
	if( ScreenSize.cy == 768)
		midy = 187;
	else
		midy = 117;



	//Left  CW
	int x, y;
	x = 20 - midx;  //ul corner
	y = midy - (ScreenSize.cy-160);
	pMpdPoly = &MpdPopUpPolys[0];
	SetBuffPoly(  pMpdPoly, x, y, MPD_WIDTH, MPD_HEIGHT);

	//Right  CW
	x = (ScreenSize.cx-160) - midx;  //ul corner
	y = midy - (ScreenSize.cy-160);//20
	pMpdPoly = &MpdPopUpPolys[1];
	SetBuffPoly(  pMpdPoly, x, y, MPD_WIDTH, MPD_HEIGHT);


	//Middle
	//x = (midx-80) - midx;  //ul corner
	//y = midy - (ScreenSize.cy-160) ;
	//pMpdPoly = &MpdPopUpPolys[2];
	//SetBuffPoly(  pMpdPoly, x, y, MPD_WIDTH, MPD_HEIGHT);
}


void InitTestPolyMpds( void)
{
	GrBuffPolygon  *pMpdPoly;

	//Top Left  CW
	int x, y;
	x = -300;  //ul corner
	y = 20;
	pMpdPoly = &MpdTestPolys[0];
	SetBuffPoly(  pMpdPoly, x, y, MPD_WIDTH, MPD_HEIGHT);


	//Top Right  CW
	x = -300+200;  //ul corner
	y = 20;
	pMpdPoly = &MpdTestPolys[1];
	SetBuffPoly(  pMpdPoly, x, y, MPD_WIDTH, MPD_HEIGHT);


	// Bottom Middle
	x = -300+400;  //ul corner
	y = 20;
	pMpdPoly = &MpdTestPolys[2];
	SetBuffPoly(  pMpdPoly, x, y, MPD_WIDTH, MPD_HEIGHT);

}

void RotatPolyBuff( GrBuffPolygon *pBuffPoly, 	FMatrix *pMat, char scale )
{
	FPoint *pPt, Pts[4];

	//Rotate Corner Pts
	pPt = &Pts[0];
	for( int i=0; i<4; i++)
	{
		*pPt = pBuffPoly->Points[i];
		if(scale)
			pPt->AddScaledVector(100.0,*(FPoint *)&VC.xdelta);
		*pPt *= *pMat;
		pPt++;
	}

	// Get Edge Vectors
	pBuffPoly->ULRotPoint =Pts[0];

	pBuffPoly->LRVector  = Pts[1];
	pBuffPoly->LRVector -= Pts[0];

	pBuffPoly->UDVector  = Pts[3];
	pBuffPoly->UDVector -= Pts[0];

	pBuffPoly->Offset = 0.0f;
}

void RenderMPDPopUp( float sx, float sy, BYTE *vars );


void DisplayPopUpMpds(void)
{
	OurShowCursor( TRUE );


	SetD3DStates(0);
	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   FALSE);
	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
	D3DStates |= (PERM_NO_ZFUNC_CHANGES|PERM_NO_ZBUFFER);


	//Move this initilizations
	//static int test=0;
	//if( !test)
	//	InitPopUpMpds( );
	//test = 1;

	FMatrix mat;
	mat.Identity();




	RotatPolyBuff( &MpdPopUpPolys[0], &mat, 0 );
	GrBuffFor3D->pGrBuffPolygon = &MpdPopUpPolys[0];			// Set to Poly
	pTextOverlay = pTextOverlay1;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD0,FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay, 0 );


	RotatPolyBuff( &MpdPopUpPolys[1], &mat, 0 );
	GrBuffFor3D->pGrBuffPolygon = &MpdPopUpPolys[1];			// Set to Poly
	pTextOverlay = pTextOverlay2;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD1,FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay, 0 );


//	RotatPolyBuff( &MpdPopUpPolys[2], &mat );
//	GrBuffFor3D->pGrBuffPolygon = &MpdPopUpPolys[2];			// Set to Poly
//	DrawMpd(MPD2, FRONT_LEFT_MPD);


	GrBuffFor3D->pGrBuffPolygon = NULL;

	D3DStates &= ~(PERM_NO_ZFUNC_CHANGES|PERM_NO_ZBUFFER);
   	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, 	   D3DCMP_LESS);
   	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   TRUE);

	SetD3DStates(0);


	if( DrawPopMpds)
	{
		BYTE vars[256];//, *pVar;

		ZeroMemory( &vars[0], sizeof(vars));


		if( pLastHSpot && pLastHSpot->MpdNum==0 && pLastHSpot->Var>=128 && pLastHSpot->Var<=187 )
			vars[pLastHSpot->Var] = 1;
		RenderMPDPopUp( MpdPopUpPolys[0].Points[0].X+ 70, MpdPopUpPolys[0].Points[0].Y- 70, &vars[0]);
		if( pLastHSpot)
			vars[pLastHSpot->Var] = 0;


		if( pLastHSpot && pLastHSpot->MpdNum==1 && pLastHSpot->Var>=128 && pLastHSpot->Var<=187 )
			vars[pLastHSpot->Var] = 1;
		RenderMPDPopUp( MpdPopUpPolys[1].Points[0].X+ 70, MpdPopUpPolys[1].Points[0].Y- 70, &vars[0]);
		pLastHSpot = NULL;

	}



}


void DisplayTestMPDS(void)
{

	_3dxlStopZBuffer();

	//Move this initilizations
	//static int test=0;
	//if( !test)
	//	InitTestPolyMpds( );
	//test = 1;

	FMatrix mat;
	mat.Identity();

	// Render Test
	RotatPolyBuff( &MpdTestPolys[0], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdTestPolys[0];			// Set to Poly
	DrawMpd(MPD0,FRONT_LEFT_MPD);
	GrDrawLineClipped( GrBuffFor3D, MpdTestPolys[0].x2D, MpdTestPolys[0].y2D, MpdTestPolys[0].x2D+ MpdTestPolys[0].Width2D, MpdTestPolys[0].y2D, 255, 1, 1, 0 );
	GrDrawLineClipped( GrBuffFor3D, MpdTestPolys[0].x2D, MpdTestPolys[0].y2D, MpdTestPolys[0].x2D, MpdTestPolys[0].y2D+ MpdTestPolys[0].Height2D, 1, 255, 0, 0 );

	RotatPolyBuff( &MpdTestPolys[1], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdTestPolys[1];			// Set to Poly
	DrawMpd(MPD1,FRONT_LEFT_MPD);
	GrDrawLine( GrBuffFor3D, MpdTestPolys[1].x2D, MpdTestPolys[1].y2D, MpdTestPolys[1].x2D+ MpdTestPolys[1].Width2D, MpdTestPolys[1].y2D, 255, 1, 1,0 );
	GrDrawLine( GrBuffFor3D, MpdTestPolys[1].x2D, MpdTestPolys[1].y2D, MpdTestPolys[1].x2D, MpdTestPolys[1].y2D+ MpdTestPolys[1].Height2D, 1, 1, 0,0 );

	RotatPolyBuff( &MpdTestPolys[2], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdTestPolys[2];			// Set to Poly
	DrawMpd(MPD2, FRONT_LEFT_MPD);
	GrDrawLine( GrBuffFor3D, MpdTestPolys[2].x2D, MpdTestPolys[2].y2D, MpdTestPolys[2].x2D+ MpdTestPolys[2].Width2D, MpdTestPolys[2].y2D, 0, 1, 0 );
	GrDrawLine( GrBuffFor3D, MpdTestPolys[2].x2D, MpdTestPolys[2].y2D, MpdTestPolys[2].x2D, MpdTestPolys[2].y2D+ MpdTestPolys[2].Height2D, 1, 1, 0 );


	GrBuffFor3D->pGrBuffPolygon = NULL;


	GrDrawLineClipped( GrBuffFor3D, MpdTestPolys[0].x2D, MpdTestPolys[0].y2D, MpdTestPolys[0].x2D+ MpdTestPolys[0].Width2D, MpdTestPolys[0].y2D, 255, 1, 1, 0 );
	GrDrawLineClipped( GrBuffFor3D, MpdTestPolys[0].x2D, MpdTestPolys[0].y2D, MpdTestPolys[0].x2D, MpdTestPolys[0].y2D+ MpdTestPolys[0].Height2D, 1, 255, 0, 0 );


   	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   TRUE);

}

int CursorInMpd(int MpdNum,float *X,float *Y)
{
	hspot *pHSpot;

	if(InteractCockpitMode)
	{

		ReadMouse();
		pHSpot = CheckMouseMPD( MpdNum,ppCurrMouse.x, ppCurrMouse.y);
		if( pHSpot)
		{
			if( pHSpot->Type & HST_POLY  && pHSpot->Poly)
			{
				 *X = (pHSpot->tu*pHSpot->Poly->Width2D);
				 *Y = (pHSpot->tv*pHSpot->Poly->Height2D);
				 return(TRUE);
			}
		}
	}
	return(FALSE);
}



void CheckVirtualCockpitInteraction( void)
{
	static char down=0;
	int mouse;

	mouse = GetMouseLB();
	if( !mouse)
		down = 0;

	if( mouse && !down )//GetMouseLB() & 1)
	{
		down = 1;

		ReadMouse();
		pLastHSpot = CheckMouseClick( ppCurrMouse.x, ppCurrMouse.y);
		if( pLastHSpot)
		{
			if( pLastHSpot->Var < 128+60)   //mpd buttons check
			{
				int mpdnum, button;
				mpdnum = pLastHSpot->MpdNum; //(pLastHSpot->Var - 128) * 1.0f/20.0f;//
				button = (pLastHSpot->Var - 128) - (mpdnum *20);
				MpdButtonPress(mpdnum, button);
			}

			if( pLastHSpot->Type & HST_POLY  && pLastHSpot->Poly)
				SetTDCMpd( pLastHSpot->MpdNum,(pLastHSpot->tu*pLastHSpot->Poly->Width2D),(pLastHSpot->tv*pLastHSpot->Poly->Height2D));


			if( pLastHSpot->MpdNum == 6 && pLastHSpot->Type & HST_POLY  && pLastHSpot->Poly)
				CheckUFCMpdClick(pLastHSpot->MpdNum,(pLastHSpot->tu*pLastHSpot->Poly->Width2D),(pLastHSpot->tv*pLastHSpot->Poly->Height2D));

			if( pLastHSpot->MpdNum == 7  &&  pLastHSpot->Type & HST_POLY  && pLastHSpot->Poly)
				DesignateAGPoint( PlayerPlane, GetMouseX(), GetMouseY() );



			CockpitItemClick( pLastHSpot->Var );


		}
	}


}

void MovingMPDPopUps( void)
{
	/*

	if(DrawPopMpds)
		{
			if (MovingMPD)
			{
				slot = FindClosestUnOccupiedMPDSlot(MouseX, MouseY);
				if (slot != -1)
				{
					MPDSlot[MovingWhichMPD] = slot;
					VirtualMpdInteractSetup();
				}

				MovingMPD = FALSE;

				return (FALSE);
			}

			walker = &MPDOrder[0];
   			MpdCount = 0;

			while((walker < &MPDOrder[3]) && (MpdNum == -1))
			{
				MpdPtr = &MPD.MpdList[OrderlyConstantToFuckedUpDefine[*walker]];
	   			if( IsPointInside(MouseX, MouseY, MpdPtr->ScrY-8, MpdPtr->ScrY + 171,MpdPtr->ScrX-3, MpdPtr->ScrX + 173) )
				{
					MpdNum = OrderlyConstantToFuckedUpDefine[*walker];

					// move this MPD to the top
					if (MpdCount)
						if (MpdCount == 1)
							Swap4(&MPDOrder[0],&MPDOrder[1]);
						else
						{
							Swap4(&MPDOrder[0],&MPDOrder[1]);
							Swap4(&MPDOrder[0],&MPDOrder[2]);
						}

					// If inside green triangle, we want to reposition
	   				if( IsPointInside(MouseX, MouseY, MpdPtr->ScrY-8, MpdPtr->ScrY + 5,MpdPtr->ScrX-3, MpdPtr->ScrX + 11) )
					{
						if ((MouseY - (MpdPtr->ScrY-8)) < (15-(MouseX - (MpdPtr->ScrX-3))))
						{
							MovingMPD = TRUE;
							MovingWhichMPD = MPDOrder[0];
							return FALSE;
						}
					}

				}
				walker++;
				MpdCount++;
			}
		}
		*/

}



void DisplayMPDS(void)
{

	SetD3DStates(0);


	//Move this initilizations
	//static int test=0;
	//if( !test)
	//{
	//	InitTestPolyMpds( );
	//	InitVCHotspots( );
	//	InitPolyMpds( );
	//}
	//test = 1;

	//CheckVirtualCockpitInteraction( );


	//_3dxlStopZBuffer();
	//lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 		   FALSE);
	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   FALSE);
	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);

	D3DStates |= (PERM_NO_ZFUNC_CHANGES|PERM_NO_ZBUFFER);



	FMatrix mat;
	mat = PlayerPlane->Attitude;
	mat	*= ViewMatrix;


	//Force MPD TEST
	//	Mpds[MPD0].Mode = TGT_IR_MODE;//ADI_MODE;


	// Render MPDS
	RotatPolyBuff( &MpdPolys[0], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdPolys[0];			// Set to Poly
	pTextOverlay =pTextOverlay1;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD0,FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay);


	RotatPolyBuff( &MpdPolys[1], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdPolys[1];			// Set to Poly
		pTextOverlay =pTextOverlay2;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD3,FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay);


	RotatPolyBuff( &MpdPolys[2], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdPolys[2];			// Set to Poly
	pTextOverlay =pTextOverlay1;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD1, FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay);


	RotatPolyBuff( &MpdPolys[3], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdPolys[3];
	pTextOverlay =pTextOverlay2;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD4, FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay);


	RotatPolyBuff( &MpdPolys[4], &mat );
	GrBuffFor3D->pGrBuffPolygon = &MpdPolys[4];
	pTextOverlay =pTextOverlay1;
	ClearTextOverlay( pTextOverlay );
	DrawMpd(MPD2, FRONT_LEFT_MPD);
	DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay);


	// DISABLED asper GK
	//RotatPolyBuff( &MpdPolys[5], &mat );
	//GrBuffFor3D->pGrBuffPolygon = &MpdPolys[5];
 	//DrawMpd(MPD5, FRONT_LEFT_MPD);

/// JLM UFC MODE WHERE WE HAVE A MPD UP ON TOP OF UFC)
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
			RotatPolyBuff( &MpdPolys[7], &mat );
			GrBuffFor3D->pGrBuffPolygon = &MpdPolys[7];
			pTextOverlay =pTextOverlay2;
			ClearTextOverlay( pTextOverlay );
 			DrawMpd(MPD6,FRONT_LEFT_MPD);
			DrawTextOverlay( GrBuffFor3D->pGrBuffPolygon , pTextOverlay);

	}

	if ((UFC.NavFlirStatus) && (!(PlayerPlane->SystemInactive & DAMAGE_BIT_NAV_FLIR)))
	{
		RotatPolyBuff( &MpdPolys[6], &mat );
		GrBuffFor3D->pGrBuffPolygon = &MpdPolys[6];
		DrawHudFlir(mat,(BOOL)(UFC.NavFlirStatus == 1));
	}

	if( ToggleDisplayTestMPDs )//set in a keyfunc when keyfunc.cpp is back up
		DisplayTestMPDS();


	GrBuffFor3D->pGrBuffPolygon = NULL;



//	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 		   TRUE);
	D3DStates &= ~(PERM_NO_ZFUNC_CHANGES|PERM_NO_ZBUFFER);
   	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, 	   D3DCMP_LESS);
   	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   TRUE);

	SetD3DStates(0);

}


void DebugShowHotSpots( void)
{

	FMatrix mat;
	mat = PlayerPlane->Attitude;
	mat	*= ViewMatrix;

	//DEBUG CODE
	//DrawCircleClip(GrBuffFor3D, 320, 240, 50, 56);
	float hsx, hsy;
	hspot *pHSpot;
	FPoint rot;

	mat.Identity();
	pHSpot = &CockpitHotSpots.MPDPopUp[0];// &CockpitHotSpots.MPDA[0];
	while( pHSpot< &CockpitHotSpots.MPDPopUp[24])
	{
		rot.RotateInto( pHSpot->Center, mat);
		rot.Perspect( &hsx, &hsy);
		//pHSpot->Center.Perspect( &hsx, &hsy);
		if( pHSpot->Center.Z)
			//if( id == pHSpot->id)
				DrawCircleClip(GrBuffFor3D, hsx, hsy, (pHSpot->Radius/pHSpot->Center.Z) *  EyeToScreen , 255);
			//else
			//	DrawCircleClip(GrBuffFor3D, hsx, hsy, (pHSpot->Radius/pHSpot->Center.Z) *  EyeToScreen , 56);
		pHSpot++;
	}

}

int CeilToPow2( int num )
{
	int pow;

	if( num ==0)
		return 0;

	pow = 1;
	while(pow < num)
		pow <<= 1;

	return pow;
}



#define MAX_LINE_SIZE  256
#define MAX_DESCRIPTOR 64

void ParseMPDPopUpHotspotFile( char *file )
{
	FILE *stream;
	hspot *pHotSpot;

	if( (stream = fopen( RegPath("data", file), "r" )) != NULL )
	{
		pHotSpot = &CockpitHotSpots.MPDPopUp[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.MPDPopUp[0];
		while( pHotSpot < &CockpitHotSpots.MPDPopUp[24])
		{	
			if( ScreenSize.cx == 1024)
			{
				pHotSpot->Center.Z = -36;
				pHotSpot->LR.Z= -36;
				pHotSpot->UL.Z= -36;
			}
			else
			if( ScreenSize.cx == 800)
			{
				pHotSpot->Center.Z = -28;
				pHotSpot->LR.Z= -28;
				pHotSpot->UL.Z= -28;
			}
			else
			{
				pHotSpot->Center.Z = -22;
				pHotSpot->LR.Z= -22;
				pHotSpot->UL.Z= -22;
			}

			pHotSpot++;
		}



		fclose( stream);
	}

}


void ParseHotspotFile( char *file)
{
	FILE *stream;
	hspot *pHotSpot;


	if( (stream = fopen( RegPath("data", file), "r" )) != NULL )
	{
		// Scan MPD0
		pHotSpot = &CockpitHotSpots.MPDA[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.MPDB[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.MPDC[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.MPDD[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.MPDE[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.MPDF[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.HUDFLIR[0];
		parseHotSpot( stream, pHotSpot);

		pHotSpot = &CockpitHotSpots.UFC[0];
		parseHotSpot( stream, pHotSpot);


		pHotSpot = &CockpitHotSpots.CockpitButtons[0];
		parseHotSpot( stream, pHotSpot);


		fclose( stream );
	}


}

void parseHotSpot( FILE *stream, hspot *pHotSpot )
{
	int done =0;
	int found =0;
	float maxx=-999999.0f, maxy=-999999.0f, maxz=-999999.0f;
	float minx=999999.0f, miny=999999.0f, minz=999999.0f;
	float radius=0;
	char line[MAX_LINE_SIZE];
	char descriptor[MAX_DESCRIPTOR];
	hspot *pBoundSphere ;
	FPoint A, B;


	pBoundSphere = pHotSpot++;			// first hspot should be the bounding sphere

	while( !done && fgets( &line[0], MAX_LINE_SIZE, stream ) != NULL)
	{
		if( sscanf( &line[0], "%s", &descriptor) != EOF )
		{
			if( !strcmp( descriptor, "MPD") )
			{
				//MPD 0: 		UL (-11.571555138,-9.350901604,-23.156612396) 	LR (-5.375528336,-15.546928406,-23.156612396)
				sscanf( &line[0], "MPD %s UL (%f,%f,%f) LR (%f,%f,%f)", &descriptor, &(pHotSpot->UL.X), &(pHotSpot->UL.Y), &(pHotSpot->UL.Z), &(pHotSpot->LR.X), &(pHotSpot->LR.Y), &(pHotSpot->LR.Z) );
				pHotSpot->Type = HST_RECTANGLE;

				// Calc Normal
				A.SetValues(  pHotSpot->LR.X, pHotSpot->UL.Y, pHotSpot->UL.Z);//= pHotSpot->PolyPts[1];
				A -= pHotSpot->UL;//PolyPts[0];	// left to right vector
				//B.SetValues(  pHotSpot->UL.X, pHotSpot->LR.Y, pHotSpot->UL.Z);//pHotSpot->PolyPts[3];
				B.SetValues(  pHotSpot->UL.X, pHotSpot->LR.Y, pHotSpot->LR.Z);//pHotSpot->PolyPts[3];
				B -= pHotSpot->UL;//PolyPts[0];	// up to down vecotr

				pHotSpot->Normal = B;
				pHotSpot->Normal %= A;
				pHotSpot->Normal.Normalize();
				pHotSpot->Var = 0;//

				pHotSpot++;
				found =1;
			}
			else
			if( !strcmp( descriptor, "Button") )
			{
				//Button var 145 	Center (-8.473541260,-8.953891754,-22.982501984) 	Radius 0.342285007 	States 0 1
				sscanf( &line[0], "Button var %d Center (%f,%f,%f) Radius %f  States", &(pHotSpot->Var), &(pHotSpot->Center.X), &(pHotSpot->Center.Y), &(pHotSpot->Center.Z),&pHotSpot->Radius );
				pHotSpot->Type = HST_SPHERE;
				pHotSpot++;
				found =1;
			}

		}
		else if( found )
			done =1;
	}




	pHotSpot--;

	while( pHotSpot != pBoundSphere)
	{
		if(	pHotSpot->Type == HST_SPHERE )
		{
			if( (pHotSpot->Center.X + pHotSpot->Radius) > maxx)
				maxx = (pHotSpot->Center.X + pHotSpot->Radius);
			if( (pHotSpot->Center.X - pHotSpot->Radius) < minx)
				minx = (pHotSpot->Center.X - pHotSpot->Radius);

			if( (pHotSpot->Center.Y + pHotSpot->Radius) > maxy)
				maxy = (pHotSpot->Center.Y + pHotSpot->Radius);
			if( (pHotSpot->Center.Y - pHotSpot->Radius) < miny)
				miny = (pHotSpot->Center.Y - pHotSpot->Radius);

			if( (pHotSpot->Center.Z + pHotSpot->Radius) > maxz)
				maxz = (pHotSpot->Center.Z + pHotSpot->Radius);
			if( (pHotSpot->Center.Z - pHotSpot->Radius) < minz)
				minz = (pHotSpot->Center.Z - pHotSpot->Radius);


			//maxz = pHotSpot->Center.Z;

		}
		else if( pHotSpot->Type == HST_RECTANGLE )
		{
			if( pHotSpot->UL.X > maxx)
				maxx = pHotSpot->UL.X;
			if( pHotSpot->LR.X > maxx)
				maxx = pHotSpot->LR.X;

			if( pHotSpot->LR.X < minx)
				minx = pHotSpot->LR.X;
			if( pHotSpot->UL.X < minx)
				minx = pHotSpot->UL.X;


			if( pHotSpot->UL.Y > maxy)
				maxy = pHotSpot->UL.Y;
			if( pHotSpot->LR.Y > maxy)
				maxy = pHotSpot->LR.Y;

			if( pHotSpot->LR.Y < miny)
				miny = pHotSpot->LR.Y;
			if( pHotSpot->UL.Y < miny)
				miny = pHotSpot->UL.Y;


			if( pHotSpot->UL.Z > maxz)
				maxz = pHotSpot->UL.Z;
			if( pHotSpot->LR.Z > maxz)
				maxz = pHotSpot->LR.Z;


			if( pHotSpot->LR.Z < minz)
				minz = pHotSpot->LR.Z;
			if( pHotSpot->UL.Z < minz)
				minz = pHotSpot->UL.Z;


			//maxz = pHotSpot->UL.Z;


		}

		pHotSpot--;
	}

	FPoint Radius;
	Radius.SetValues( (maxx-minx) * 0.5f, (maxy-miny) * 0.5f, (maxz-minz) * 0.5f);//0.0f);
	pBoundSphere->Type = HST_SPHERE;
	pBoundSphere->Center.SetValues( Radius.X + minx,Radius.Y + miny, Radius.Z + minz),
	pBoundSphere->Radius = Radius.Length();

}



void DisplayUFCMockup( void)
{
	int r=0, g=100, b=0;
	GrBuff *pGrBuff= GrBuffFor3D;
	int center=12;

	DrawBox(pGrBuff, 2,		8,		98,		11,  r, g, b);


	DrawBox(pGrBuff, 2,		22,		27,		28,  r, g, b); //1
	DrawBox(pGrBuff, 2,		53,		27,		28,  r, g, b); //4
	DrawBox(pGrBuff, 2,		83,		27,		28,  r, g, b); //7
	DrawBox(pGrBuff, 2,		114,	27,		27,  r, g, b); //clr
	DrawTextAtLocation(0 + center,	22 + center,    "1",		r, g, b, 0);
	DrawTextAtLocation(0 + center,	53 + center,    "4",		r, g, b, 0);
	DrawTextAtLocation(0 + center,	83 + center,    "7",		r, g, b, 0);
	DrawTextAtLocation(0 + center,	114 + center,   "CLR",		r, g, b, 0);


	DrawBox(pGrBuff, 36,	22,		29,		28,  r, g, b); //2
	DrawBox(pGrBuff, 36,	53,		29,		28,  r, g, b); //5
	DrawBox(pGrBuff, 36,	83,		29,		28,  r, g, b); //8
	DrawBox(pGrBuff, 36,	114,	29,		27,  r, g, b); //0
	DrawTextAtLocation(36 + center,	22 + center,    "2",		r, g, b, 0);
	DrawTextAtLocation(36 + center,	53 + center,    "5",		r, g, b, 0);
	DrawTextAtLocation(36 + center,	83 + center,    "8",		r, g, b, 0);
	DrawTextAtLocation(36 + center,	114 + center,   "0",		r, g, b, 0);



	DrawBox(pGrBuff, 71,	22,		29,		28,  r, g, b); //3
	DrawBox(pGrBuff, 71,	53,		29,		28,  r, g, b); //6
	DrawBox(pGrBuff, 71,	83,		29,		28,  r, g, b); //9
	DrawBox(pGrBuff, 71,	114,	29,		27,  r, g, b); //ent
	DrawTextAtLocation(71 + center,	22 + center,    "3",		r, g, b, 0);
	DrawTextAtLocation(71 + center,	53 + center,    "6",		r, g, b, 0);
	DrawTextAtLocation(71 + center,	83 + center,    "9",		r, g, b, 0);
	DrawTextAtLocation(71 + center,	114 + center,   "ENT",		r, g, b, 0);


	DrawBox(pGrBuff, 103,	8,		32,		22,  r, g, b); //ils
	DrawBox(pGrBuff, 103,	28,		32,		30,  r, g, b); //a/p
	DrawBox(pGrBuff, 103,	57,		32,		30,  r, g, b); //tcn
	DrawBox(pGrBuff, 103,	86,		32,		30,  r, g, b); //iff
	DrawBox(pGrBuff, 103,	114,	32,		27,  r, g, b); //mdi
	DrawTextAtLocation(103 + center,	0 + center,    "ILS",		r, g, b, 0);
	DrawTextAtLocation(103 + center,	28 + center,    "A/P",		r, g, b, 0);
	DrawTextAtLocation(103 + center,	57 + center,    "TCN",		r, g, b, 0);
	DrawTextAtLocation(103 + center,	86 + center,   "IFF",		r, g, b, 0);
	DrawTextAtLocation(103 + center,	114 + center,   "MDI",		r, g, b, 0);




	DrawBox(pGrBuff, 138,	8,		32,		22,  r, g, b);
	DrawBox(pGrBuff, 138,	28,		32,		30,  r, g, b); //ralt
	DrawBox(pGrBuff, 138,	57,		32,		30,  r, g, b); //ew
	DrawBox(pGrBuff, 138,	86,		32,		30,  r, g, b); // flir
	DrawBox(pGrBuff, 138,	114,	32,		27,  r, g, b);
	DrawTextAtLocation(138 + center,	0 + center,		"",			r, g, b, 0);
	DrawTextAtLocation(138 + center,	28 + center,    "RALT",		r, g, b, 0);
	DrawTextAtLocation(138 + center,	57 + center,    "EW",		r, g, b, 0);
	DrawTextAtLocation(138 + center,	86 + center,	"FLIR",		r, g, b, 0);
	DrawTextAtLocation(138 + center,	114 + center,   "",			r, g, b, 0);



}


void SetScale(float new_scale);

FPoint mpd_ambient(0.5,0.5,0.5);

void RenderMPDPopUp( float sx, float sy, BYTE *vars )
{

	FMatrix mat, tempmat;
	FPoint pos;
	float old_max_z;
	GrBuff old_3d_buff;
	FPoint old_amb_color;
	float old_amb_intensity;
	BYTE local_vars[256];

//if (Camera1.SubType & COCKPIT_ZOOM_MPD)	// no pop
	//	return;


	if( pMPDPopUp )
	{
		old_3d_buff = *GrBuffFor3D;

		if (!vars)
		{
			vars = local_vars;
			ZeroMemory(local_vars,sizeof(local_vars));
		}


		GrBuffFor3D->MidX = sx + GrBuffFor3D->MidX;
		GrBuffFor3D->MidY = -sy + GrBuffFor3D->MidY;

		pos.X = 0.0f;
		pos.Y = 0.0f;
		pos.Z = ((6 INCHES) * EyeToScreen)/196.0f;//-15.0f;//EyeToScreen;//-512;//ZNearClipPlane//ZClipPlane;//EyeToScreen

		//test to fix pop up offsets for manual screenshots
		if( ScreenSize.cx == 800) pos.Z += 0.0047;// magic number to  scale object just right	scalez;



		SetScale(100.0f);
		ZClipPlane = pos.Z*0.8f;//(-1.0f/12.0f) FEET;
		old_max_z = D3DMaxZ;
		D3DMaxZ = (-150.0f FEET)/0.9999f;
		ooD3DMaxZ = 1.0f/D3DMaxZ;
		StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);

		tempmat = ViewMatrix;
		mat.Identity();
		mat.SetViewMatrix();



		float scale = pos.Z /EyeToScreen;
		pos.X *= scale;
		pos.Y *= scale;



		vars[64] =  (unsigned char)(255.0f * 0.75f);
		vars[65] =  (unsigned char)(255.0f * 0.75f);

		old_amb_color = SkyLight.Color;
		old_amb_intensity = SkyLight.Intensity;

		SkyLight.Color.ReplaceWithLargerOf(mpd_ambient);
		if (0.05f > SkyLight.Intensity)
			SkyLight.Intensity = 0.05f;

		Draw3DObjectMatrix( pMPDPopUp, pos, mat, (unsigned char *)vars );

		SkyLight.Color = old_amb_color;
		SkyLight.Intensity = old_amb_intensity;

		tempmat.Transpose();
		tempmat.SetViewMatrix();

		SetScale(1.0f);
		ZClipPlane = -1.9f;
		D3DMaxZ = old_max_z;
		ooD3DMaxZ = 1.0f/D3DMaxZ;
		StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);


		*GrBuffFor3D = old_3d_buff;

	}

}

void MPDSystemMalfunction(int MpdNum)
{
	if(MpdNum != MPD6)
	  DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawTextAtLoc(35,65,"SYSTEM MALFUNCTION",AV_GREEN,1.0);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
		}
	}
}

int MPDBroken(int MpdNum)
{
	switch(MpdNum)
	{
		case 0:
			return(0);
		case 1:
			if(Cpit.Damage[AV_NO_R_MDI])
				return(1);
			if(PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP)
				return(1);
			break;
		case 2:
			if(PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP)
				return(1);
			break;
		case 3:
			if(PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP)
				return(1);
			break;
		case 4:
			if(PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP)
				return(1);
			break;
		default:
			return(0);
	}

	return(0);
}




// This is a 2d Poly to overlay onto the 3D Mpds...
// attempting to fixup the text especially at 640x480
void InitTextSurface( void)
{
	TextureRef our_ref;
	float width, height;
	width = 141;
	height = 141;


	ZeroMemory(&our_ref,sizeof(our_ref));
	our_ref.CellWidth = 1.0f;
	our_ref.CellHeight = 1.0f;

	pTextOverlay  = NULL;
	// alloc a 141x141 texture...power 2?
	pTextOverlay1 = CreateTexture( &our_ref,  CeilToPow2( width ),	CeilToPow2( height),	UT_PURE_ALPHA,0 );
	pTextOverlay1->WidthMulter  = (float)width/(float)CeilToPow2( width);
	pTextOverlay1->HeightMulter = (float)height/(float)CeilToPow2( pTextOverlay1->Height);

	pTextOverlay2 = CreateTexture( &our_ref,  CeilToPow2( width ),	CeilToPow2( height),	UT_PURE_ALPHA,0 );
	pTextOverlay2->WidthMulter  = (float)width/(float)CeilToPow2( width);
	pTextOverlay2->HeightMulter = (float)height/(float)CeilToPow2( pTextOverlay2->Height);



}

void ShutdownTextSurface( void)
{
	pTextOverlay = NULL;

	if( pTextOverlay1)
	{
		RemoveTexture( pTextOverlay1);
		EasyFree(pTextOverlay1);
		pTextOverlay1 = NULL;
	}

	if( pTextOverlay2)
	{
		RemoveTexture( pTextOverlay2);
		EasyFree(pTextOverlay2);
		pTextOverlay2 = NULL;
	}



}

void ClearTextOverlay(TextureBuff *pTexture)
{
	DDBLTFX		BltFX;
	BltFX.dwSize = sizeof (BltFX);
	BltFX.dwFillColor = 0x0;
	pTexture->D3DSurface->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &BltFX);

}



void DrawTextOverlay( GrBuffPolygon *pBuffPoly, TextureBuff *pTexture, char filteron  )
{
	// clear the buffer
	RotPoint3D Points[4], *pPoint;
	RotPoint3D *points[4];
	int clip_flags,draw_flags;
	FPoint LeftVect, DownVect;
	float width, height;
	float ULU,ULV,LRU,LRV;
	int i;

	float red, green, blue;
	float colorscale = 1.0f/255.0f;
	red = 0.0f;
	green = 0.0f;
	blue = 1.0f;

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,  1.0f, &Triple );

	//blue  = GET_CURRENT_BLUE( AV_GREEN);
	//green =	6.0fGET_CURRENT_GREEN( AV_GREEN);
	//red   = GET_CURRENT_RED( AV_GREEN);
	red   = Triple.Red * colorscale;
	green = Triple.Green  * colorscale;
	blue  = Triple.Blue * colorscale;



	width = pBuffPoly->Width2D;//141;
	height= pBuffPoly->Height2D;//141;


	// Poly Buff should already be rotated and have a LRVect, UDVect and ULPoint;
	clip_flags = 0;
	draw_flags = -1;

	float x, y, x2, y2;
	x = 0.0f;
	y = 0.0f;
	x2 = width;
	y2 = height;

	float scale;
	scale = 1.0f/ pBuffPoly->Width2D; //dest->Width;
	x  *= scale;
	x2 *= scale;

	scale = 1.0f/ pBuffPoly->Height2D; //dest->Height;
	y  *= scale;
	y2 *= scale;


	LeftVect = pBuffPoly->LRVector;
	LeftVect *= x;
	DownVect = pBuffPoly->UDVector;
	DownVect *= y;
	Points[0].Rotated = LeftVect;
	Points[0].Rotated += DownVect;
	Points[0].Rotated += pBuffPoly->ULRotPoint;

	LeftVect = pBuffPoly->LRVector;
	LeftVect *= x2;
	DownVect = pBuffPoly->UDVector;
	DownVect *= y;
	Points[1].Rotated = LeftVect;
	Points[1].Rotated += DownVect;
	Points[1].Rotated += pBuffPoly->ULRotPoint;


	LeftVect = pBuffPoly->LRVector;
	LeftVect *= x2;
	DownVect = pBuffPoly->UDVector;
	DownVect *= y2;
	Points[2].Rotated = LeftVect;
	Points[2].Rotated += DownVect;
	Points[2].Rotated += pBuffPoly->ULRotPoint;

	LeftVect = pBuffPoly->LRVector;
	LeftVect *= x;
	DownVect = pBuffPoly->UDVector;
	DownVect *= y2;
	Points[3].Rotated = LeftVect;
	Points[3].Rotated += DownVect;
	Points[3].Rotated += pBuffPoly->ULRotPoint;


//	for( i=0; i<4; i++)
//		Points[i].Rotated = pBuffPoly->Points[i];


	// Setup Texture Cordinates
	ULU = 0.0f;
	LRU= ULU + width  * (1.0f/ pTexture->Width);

	ULV = 0.0f;
	LRV= ULV + height * (1.0f/ pTexture->Height);

	Points[0].U = ULU;
	Points[0].V = ULV;
	Points[2].U = LRU;
	Points[2].V = LRV;

	Points[1].U = Points[2].U;
	Points[1].V = Points[0].V;
	Points[3].U = Points[0].U;
	Points[3].V = Points[2].V;

	CurrentTexture = pTexture;



	// Rot/Perspect Pts
	pPoint = &Points[0];
	for( i=0; i<4; i++)
	{
		points[i] = pPoint;

		if (pPoint->Rotated.Z > ZClipPlane)
			pPoint->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			pPoint->Flags = PT3_ROTATED;
			PerspectPoint(pPoint);
		}
		//pPoint->Flags |= PT3_OOW_SET;
		clip_flags |= pPoint->Flags;
		draw_flags &= pPoint->Flags;
		pPoint->Diffuse.SetValues( (const float)red, (const float)green, (const float)blue);
		pPoint->Specular.SetValues( 0.0f);
		pPoint->Alpha = 1.0f;
		pPoint->oow  = 1.0f;
		pPoint->ooz  = 1.0f;
		pPoint->fog  = 1.0f;
		pPoint++;
	}


	if (!(draw_flags & PT3_NODRAW))
	{
		if (filteron && (( Camera1.SubType & COCKPIT_ZOOM_MPD) ||  (ScreenSize.cx > 640)) )//|| (Camera1.Pitch + Camera1.Heading + Camera1.Pitch) )
			DrawPolyClipped(PRIM_TEXTURE | PRIM_ALPHAIMM,CL_CLIP_UV,4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);
		else
			DrawPolyClipped(PRIM_POINT_FILTER |PRIM_TEXTURE | PRIM_ALPHAIMM,CL_CLIP_UV,4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);

	}


}
