#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "f18weap.h"
#include "snddefs.h"

extern AvionicsType Av;

/* this is an E3 hack */
extern int CurLoadOut;// = 0;
extern ObjectHandler *LoadOuts[5][MAX_SUBPARTS];

//extern int VirtualMpdsOn;
extern int DrawPopMpds;
VKCODE MasterPress;
void ToggleVirtualMpds();

extern LeftSideHasFuelTank;
extern RightSideHasFuelTank;

extern void ShutLeftEngineDown(void);
extern void ShutRightEngineDown(void);


extern ChaffTotal;
extern FlareTotal;

// USED FOR ADI
#define MPD_GROUND_COLOR  6
#define MPD_SKY_COLOR  94
#define BACKUP_GROUND_COLOR  44
#define BACKUP_SKY_COLOR   8

#define BACKUP_GROUND_COLOR_NIGHT  47
#define BACKUP_SKY_COLOR_NIGHT     40


void LoadLookDownSprites(void);
void CleanUpLookDownSprites(void);
void UpdateLookDownSprites(void);
void DrawLookDownSprites(void);
void InitLookDown();
void LoadMiscMpdSprites();
void DrawVertVelGauge(int ScrX, int ScrY, int Length, double Val, double Max);
void DrawAirspeedGauge(int ScrX, int ScrY, int Length, double Val, double Max);
int GetSingleFireState();
int GetFireExtState();

void CheckVirtualCockpitInteraction(void);


void CtrClick(int *State);
void LIClick(int *State);
void RIClick(int *State);
void LMClick(int *State);
void RMClick(int *State);
void LOClick(int *State);
void ROClick(int *State);
void JettisonClick(int *State);
void JettSwitchClick(int *State);
void LeftFireClick(int *State);
void FireLeftDischargeClick(int *State);
void FireRightDischargeClick(int *State);
void ApuFiresClick( int *State);
void RightFireClick(int *State);
void ArmSafeClick(int *State);
void AAClick(int *State);
void AGClick(int *State);
void LandingGearClick(int *State);
void NavFlirClick(int *State);
void HudColorClick(int *State);
void HookClick(int *State);
void BingoUpClick(int *State);
void BingoDownClick(int *State);
void HSelUpClick(int *State);
void HSelDownClick(int *State);
void CSelUpClick(int *State);
void CSelDownClick(int *State);
void EmisClick(int *State);
void MasterCautionClick(int *State);
void AltClick(int *State);
void SymRejectClick(int *State);
void CollLightClick(int *State);
void FormationLightClick(int *State);
void RefuelClick(int *State);
void FlapsClick(int *State);
void ReadyDischargeClick( int *State);
extern VKCODE DisableGLimit;


CpitIType CpitItems[] =
{
 {	213,	0,	CTR_ITEM,				CtrClick},
 {	214,	0,	LI_ITEM,				LIClick},
 {	215,	0,	RI_ITEM,				RIClick},
 {	216,	0,	LM_ITEM,				LMClick},
 {	217,	0,	RM_ITEM,				RMClick},
 {	218,	0,	LO_ITEM,				LOClick},
 {	219,	0,	RO_ITEM,				ROClick},
 {	202,	0,	JETTISON_ITEM,			JettisonClick},
 {	254,	1,	JETTISON_SWITCH_ITEM,	JettSwitchClick},
 {	203,	0,	ARM_SAFE_ITEM,			ArmSafeClick},
 {	204,	1,	AA_ITEM,				AAClick},
 {	205,	0,	AG_ITEM,				AGClick},
 {	208,	0,	LEFT_FIRE_ITEM,			LeftFireClick},
 {	248,	0,	FIRE_LEFT_DISCHARGE_ITEM,ReadyDischargeClick},				//FireLeftDischargeClick},
 {	209,	0,	RIGHT_FIRE_ITEM,		RightFireClick},
 {	0,		0,	FIRE_RIGHT_DISCHARGE_ITEM,NULL},
 {	253,	0,	LANDING_GEAR_ITEM,		LandingGearClick},
 {	196,	2,	NAVFLIR_ITEM,			NavFlirClick},
 {	197,	2,	HUD_COLOR_ITEM,			HudColorClick},
 {	123,	0,	HOOK_ITEM,				HookClick},
 {	207,	0,	BINGO_UP_ITEM,			BingoUpClick},
 {	206,	0,	BINGO_DOWN_ITEM,		BingoDownClick},
 {	118,	0,	HSEL_DOWN_ITEM,			HSelUpClick},
 {	117,	0,	HSEL_UP_ITEM,			HSelDownClick},
 {	120,	0,	CSEL_DOWN_ITEM,			CSelUpClick},
 {	119,	0,	CSEL_UP_ITEM,			CSelDownClick},
 {	199,	0,	EMIS_ITEM,				EmisClick},
 {	227,	2,	MASTER_CAUTION_ITEM,	MasterCautionClick},
 {	194,	0,	ALT_ITEM,				AltClick},
 {	195,	0,	SYM_REJECT_ITEM,		SymRejectClick},

 {	255,	1,	COLL_LIGHT_ITEM,		CollLightClick},
 {	124,	2,	FORM_LIGHT_ITEM,		FormationLightClick},
 {	126,	0,	REFUEL_ITEM,			RefuelClick},
 {	125,	0,	FLAPS_ITEM,				FlapsClick},

 {	210,	0,	APU_FIRE_ITEM,			ApuFiresClick},


 {	-1,		-1,	-1,NULL},
};



//  copied from avmodes.cpp
typedef struct
{
	char Label[20];
	int  Index;
	int  Category;
	int  MessageIndex;
} BitType;

extern BitType BitList[];
extern BitType *BitDamageList[];
extern int NumDamagedBit;
void RemoveDamageFromBit(int Damage,int MessageType);


void SimFireExtinguish(void);

void CheckHydStuff();




// for pushing the GrBuffFor3D Buffer
GrBuff PushBuff;


PlayerPitchRollInfoType PitchRollInfo;

FMatrix	 CockpitAttitude;			 // JLM remove eventually
RotPoint3D  CockpitP1, CockpitP2;
FPoint CockpitRawP1;
FPoint CockpitRawP2;
int  CentX,CentY;

RotPoint3D  BoxVerts[7];
RotPoint3D  PolyVerts[7];
RotPoint3D *pPolyVerts[7];

RotPoint3D  NewBoxVerts[7];
RotPoint3D  NewPolyVerts[7];
RotPoint3D *pNewPolyVerts[7];

MasterADIType MasterADI;

ADIEntryType ADIList[MAX_MPDS];

// for determining if the cockpit is front or back
int IsCockpitFront;

int PreviousMasterPress;

// dashed circle type
typedef struct {

	int X;
	int Y;

} DashedCircleBufferType;

DashedCircleBufferType DashedCircleBuffer[2400];
DashedCircleBufferType DashedCircleArcBuffer[300];

int RadarRadius    = 10;	// JLM testing vars should eventually remove
int RadarDirection = 1;

#define NONE_HS	0
#define UFC_HS	1
#define MPD_HS	2
#define MISC_HS	3

int CockpitHotSpot = NONE_HS;


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawGenericSprite(int X,int Y,AvGenericSpriteType *Sprite)
{
  GrCopyRectMask8(GrBuffFor3D,X,Y,Sprite->Buff,0,0,Sprite->Width,Sprite->Height,0);
}

//*****************************************************************************************************************************************
// ANIM SPRITES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

AnimSpriteInstType FlashingReticle;

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimAnimSprite(AnimSpriteInstType *Sprite)
{

  if(Sprite->CurFrame == -1)
  {
    Sprite->CurFrame = 0;
    Sprite->LastTime = GameLoopInTicks;
	return;
  }

  int Dt = GameLoopInTicks - Sprite->LastTime;
  if(Dt > Sprite->SpriteClass->TimePerFrame)
  {
    Sprite->LastTime = GameLoopInTicks;
	if(Sprite->CurFrame + 1 >= Sprite->SpriteClass->NumFrames)
	{
      if(Sprite->SpriteClass->Repeat)
          Sprite->CurFrame = 0;
	}
	else
	{
      Sprite->CurFrame++;
	}
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitFlashingReticleSpriteAnim(AnimSpriteInstType *Sprite)
{
  Sprite->SpriteClass = &AnimSprites[0];
  Sprite->CurFrame = 0;
  Sprite->Complete = FALSE;
  Sprite->LastTime = GameLoopInTicks;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAnimSpriteFrame(int X,int Y,int Type, int FrameNum)
{
	AnimSpriteType *Ptr = &AnimSprites[Type];
  Ptr->Buff->CurrentBuffNum = FrameNum;
  GrCopyRectMask8(GrBuffFor3D,X - Ptr->AnchX,Y - Ptr->AnchY,Ptr->Buff,0,0,Ptr->Width,Ptr->Height,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAnimSprite(int X,int Y,AnimSpriteInstType *Sprite)
{
  Sprite->SpriteClass->Buff->CurrentBuffNum = Sprite->CurFrame;
  GrCopyRectMask8(GrBuffFor3D,X - Sprite->SpriteClass->AnchX,Y - Sprite->SpriteClass->AnchY,Sprite->SpriteClass->Buff,0,0,Sprite->SpriteClass->Width,Sprite->SpriteClass->Height,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void LoadAnimSprites()
{
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CleanupAnimSprites()
{
}


//***********************************************************************************************************************************
//*** LOOKDOWN SPRITES VARS

int FireExtState;
SpriteBoundBoxType FireExtSwitchBoundBox[3];

int JetWeapSelectState;
int JetWeapSelectDirection;
SpriteBoundBoxType JetWeapSelectSwitchBoundBox;

int JetPushState;
SpriteBoundBoxType JetPushBoundBox;

int FuelSwitchState;
SpriteBoundBoxType FuelSwitchBoundBox;

int LeftDownSwitchState;
SpriteBoundBoxType LeftDownSwitchBoundBox;

int RightDownSwitchState;
SpriteBoundBoxType RightDownSwitchBoundBox;

int DoubleFireState;
SpriteBoundBoxType DoubleFireBoundBox[2];

int SingleFireState;
SpriteBoundBoxType SingleFireBoundBox;

int MasterArmSwitchState;
SpriteBoundBoxType MasterArmSwitchBoundBox;

SpriteBoundBoxType BingoArrowsBoundBox[2];

int BingoUpOn,BingoDownOn;
int BingoUpStartTime,BingoDownStartTime;

FireExtStateType FEButtons[3];

int JetPushOn;
int JetPushStartTime;

int CanSimFireRight = FALSE;
int CanSimFireLeft = FALSE;
int	RightExtinguishOn = FALSE;
int LeftExtinguishOn = FALSE;

extern SimJettisonOn;
extern JettisonAA;
extern JettisonFuel;
extern JettisonAG;




//***********************************************************************************************************************************
// *****  DEBUG ROUTINES
void DrawCircleDebug(int XOfs, int YOfs, int Test)
{
//  Tst1 = Test;
//  sprintf(TmpStr,"x    %f", Tst1);
//  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs,YOfs, TmpStr,  61);

}

void FDrawCircleDebug(int XOfs, int YOfs, double Test)
{
//  Tst1 = Test;
//  sprintf(TmpStr,"x    %f", Tst1);
//  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs,YOfs, TmpStr,  61);

}


//***********************************************************************************************************************************
// *****  Clip Region Operations ***********

void PushClipRegion()
{
   PushBuff.ClipLeft   = GrBuffFor3D->ClipLeft;
   PushBuff.ClipRight  = GrBuffFor3D->ClipRight;
   PushBuff.ClipTop    = GrBuffFor3D->ClipTop;
   PushBuff.ClipBottom = GrBuffFor3D->ClipBottom;
   PushBuff.MidY       = GrBuffFor3D->MidY;
   PushBuff.MidX       = GrBuffFor3D->MidX;
}

void PopClipRegion()
{
  GrBuffFor3D->ClipLeft   = 	PushBuff.ClipLeft;
  GrBuffFor3D->ClipRight  =  	PushBuff.ClipRight;
  GrBuffFor3D->ClipTop    =    	PushBuff.ClipTop;
  GrBuffFor3D->ClipBottom = 	PushBuff.ClipBottom;
  GrBuffFor3D->MidY       =     PushBuff.MidY;
  GrBuffFor3D->MidX       =     PushBuff.MidX;
  if(  _3dxl==_3DXL_GLIDE)
	  SetClipRegion3Dfx();

}


void SetClipRegion(int Left, int Right, int Top, int Bottom, int MidY, int MidX)
{
  GrBuffFor3D->ClipLeft   =  Left;
  GrBuffFor3D->ClipRight  =  Right;
  GrBuffFor3D->ClipTop    =  Top;
  GrBuffFor3D->ClipBottom =  Bottom;
  GrBuffFor3D->MidY       =  MidY;
  GrBuffFor3D->MidX       =	 MidX;
}



void InitADIInfo()
{

  MasterADI.BoxVerts[0].iSX = 274;
  MasterADI.BoxVerts[0].iSY = 335;
  MasterADI.BoxVerts[1].iSX = 351;
  MasterADI.BoxVerts[1].iSY = 335;
  MasterADI.BoxVerts[2].iSX = 351;
  MasterADI.BoxVerts[2].iSY = 411;
  MasterADI.BoxVerts[3].iSX = 274;
  MasterADI.BoxVerts[3].iSY = 411;

  ADIList[0].BoxVerts[0].iSX = 121;
  ADIList[0].BoxVerts[0].iSY = 322;
  ADIList[0].BoxVerts[1].iSX = 160;
  ADIList[0].BoxVerts[1].iSY = 322;
  ADIList[0].BoxVerts[2].iSX = 160;
  ADIList[0].BoxVerts[2].iSY = 352;
  ADIList[0].BoxVerts[3].iSX = 121;
  ADIList[0].BoxVerts[3].iSY = 352;
  ADIList[0].GroundColor     = BACKUP_GROUND_COLOR;
  ADIList[0].SkyColor        = BACKUP_SKY_COLOR;

  ADIList[1].BoxVerts[0].iSX = 274;
  ADIList[1].BoxVerts[0].iSY = 335;
  ADIList[1].BoxVerts[1].iSX = 351;
  ADIList[1].BoxVerts[1].iSY = 335;
  ADIList[1].BoxVerts[2].iSX = 351;
  ADIList[1].BoxVerts[2].iSY = 411;
  ADIList[1].BoxVerts[3].iSX = 274;
  ADIList[1].BoxVerts[3].iSY = 411;
  ADIList[1].GroundColor     = MPD_GROUND_COLOR;
  ADIList[1].SkyColor        = MPD_SKY_COLOR;

}

//***********************************************************************************************************************************
void LoadCockpit(int Type)
{
	static int LastType;

	if( Type == VIRTUAL_MODEL)
	{
		CurrentCockpit = VIRTUAL_MODEL;
		ActivateVirtualMpds();
		LoadVirtualCockpit();
		return;
	}

	if (Type == FRONT_NO_COCKPIT)
	{
	   	CurrentCockpit = FRONT_NO_COCKPIT;
		return;
	}
}
//***********************************************************************************************************************************
void UpdateDynamicGauges(void)
{
}


float EMD_lSpin;
float EMD_rSpin;

float EMD_lTemp = 350.0;
float EMD_rTemp = 350.0;

float EMD_Amad_lTemp = 100.0;
float EMD_Amad_rTemp = 100.0;

float EMD_lFlow;
float EMD_rFlow;

float EMD_lOpen;
float EMD_rOpen;

float EMD_lOlio;
float EMD_rOlio;

void SeduceEngineData()
{
	PlaneParams *P = PlayerPlane;
//gk	float fuelflow = GetFuelFlow(P) * 60;	// (pph)
	float lfuelflow = UFC.LeftFuelFlowRate * 60;	// (pph)
	float rfuelflow = UFC.RightFuelFlowRate * 60;	// (pph)

	int lSpin, lTemp, lFlow, lOpen, lOlio;
	int rSpin, rTemp, rFlow, rOpen, rOlio;

	lSpin = P->LeftThrustPercent  * 1.10 - (FrameCount >> 6 & 1);
	rSpin = P->RightThrustPercent * 1.09 + (FrameCount >> 7 & 1);

	lTemp = 100 + 8.5 * EMD_lSpin - 0.006*P->Altitude;
	rTemp = 100 + 8.4 * EMD_rSpin - 0.005*P->Altitude;

	int sum = 1 + (EMD_lSpin + EMD_rSpin);

//gk	lFlow = fuelflow * EMD_lSpin / sum;
//gk	rFlow = fuelflow * EMD_rSpin / sum;

	lFlow = rFlow = 0;
	if (LeftEngineOn)  lFlow = lfuelflow;
	if (RightEngineOn) rFlow = rfuelflow;

  	EMD_lFlow = (float)lFlow;
	EMD_rFlow = (float)rFlow;

	lOlio = 0.1 * EMD_lSpin * (P->GForce + 3);
	rOlio = 0.1 * EMD_rSpin * (P->GForce + 3);

	// if the engine is turned off, then no hydraulics so it opens up

	if (LeftEngineOn)
	{
		if (P->LeftThrustPercent < 80)
		 	lOpen = 0.8*(100-P->LeftThrustPercent);	// 80% --> 16%
		else lOpen = 3.0*(P->LeftThrustPercent-75 );	// 16% --> 75%
	}
	else
		lOpen = 100.0f;

	if (RightEngineOn)
	{
		if (P->RightThrustPercent < 80)
		 	rOpen = 0.8*(100-P->RightThrustPercent);	// 80% --> 16%
		else rOpen = 3.0*(P->RightThrustPercent-75 );	// 16% --> 75%
	}
	else
		rOpen = 100.0f;


//	EMD_lSpin += ((float)lSpin - EMD_lSpin)*0.012f*sqrtDeltaTicks;
//	EMD_rSpin += ((float)rSpin - EMD_rSpin)*0.012f*sqrtDeltaTicks;

//	EMD_lTemp += ((float)lTemp - EMD_lTemp)*0.009f*sqrtDeltaTicks;
//	EMD_rTemp += ((float)rTemp - EMD_rTemp)*0.009f*sqrtDeltaTicks;

	EMD_lOpen += ((float)lOpen - EMD_lOpen)*0.035f*sqrtDeltaTicks;
	EMD_rOpen += ((float)rOpen - EMD_rOpen)*0.035f*sqrtDeltaTicks;

//	EMD_lOlio += ((float)lOlio - EMD_lOlio)*0.025f*sqrtDeltaTicks;
//	EMD_rOlio += ((float)rOlio - EMD_rOlio)*0.025f*sqrtDeltaTicks;
}
//***********************************************************************************************************************************
void DrawDynamicCockpit(void)
{
	if (CurrentView & COCKPIT_NOART)
		return;

	if ( CurrentView & COCKPIT_VIRTUAL_SEAT )
		DrawVirtualCockpit( );
}


//***********************************************************************************************************************************
void DisplayWarning(int Gauge, int On_Off, int UpdateStatic)
{
}

//***********************************************************************************************************************************
void Display6DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color)
{
	if ((UINT)Val > 999999)  Val = 0;

    sprintf( TmpStr,"%06d", Val );

    GrDrawString(GrBuffFor3D, SmHUDFont, ScrX,ScrY, TmpStr,  Color);
}
//***********************************************************************************************************************************
void Display5DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color)
{
	if ((UINT)Val > 99999)  Val = 0;

    sprintf( TmpStr,"%05d", Val );

    GrDrawString(GrBuffFor3D, SmHUDFont, ScrX,ScrY, TmpStr,  Color);
}
//***********************************************************************************************************************************
void Display4DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color)
{
	if ((UINT)Val > 9999)  Val = 0;

    sprintf( TmpStr,"%04d", Val );

    GrDrawString(GrBuffFor3D, SmHUDFont, ScrX,ScrY, TmpStr,  Color);
}
//***********************************************************************************************************************************
void Display3DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color)
{
	if ((UINT)Val > 999)  Val = 0;

    sprintf( TmpStr,"%03d", Val );

    GrDrawString(GrBuffFor3D, SmHUDFont, ScrX,ScrY, TmpStr,  Color);
}
//***********************************************************************************************************************************
void Display2DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color)
{
	if ((UINT)Val > 99)  Val = 0;

    sprintf( TmpStr,"%02d", Val );

    GrDrawString(GrBuffFor3D, SmHUDFont, ScrX,ScrY, TmpStr,  Color);
}

//***********************************************************************************************************************************
void InitCockpit(PlaneParams *P)
{
	DisplayWarning(NAV_DISPLAY,ON,1);

	// Flap Light
	if (P->Flaps != 0.0)
		DisplayWarning(FLAPS, ON, 1);

	// Green Lights
	if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
		DisplayWarning(GEAR, ON, 1);

	if (P->SpeedBrake != 0.0)
		DisplayWarning(SPDB, ON, 1);

	if (P->Brakes)
		DisplayWarning(BRAKE, ON, 1);

	// Clear Master caution

	InitLookDown();

	//LoadLookDownSprites();

	InitMpd(); // JLM should be put elsewhere
	InitADIInfo(); // JLM move

	CockpitRawP1.X = -5000.0; CockpitRawP1.Y = 0.0;  CockpitRawP1.Z = -5.0*1024.0;
	CockpitRawP2.X = 5000.0; CockpitRawP2.Y = 0.0;  CockpitRawP2.Z = -5.0*1024.0;
}
//***********************************************************************************************************************************
void DetectCockpitInteractions(void)
{
	//if(CurrentCockpit == VIRTUAL_MODEL)
		CheckVirtualCockpitInteraction();
}

//***********************************************************************************************************************************
//*	CODE FOR LOOKDOWN SPRITES
//***********************************************************************************************************************************

void DoFireExtSwitchUp(void)
{


}

//***********************************************************************************************************************************

void DoFireExtSwitchMiddle(void)
{



}

/*
int JLMDarray[5];
int JLMDarrayIndex = -1;
*/

//***********************************************************************************************************************************

void DoFireExtSwitchDown(void)
{


}

//***********************************************************************************************************************************

int GetSingleFireState()
{
  return(0);
}

//***********************************************************************************************************************************

void DoSingleFire(void)
{

}

//***********************************************************************************************************************************

int GetFireExtState()
{
	return(0);
}

//***********************************************************************************************************************************

void DoDoubleFireLeft(void)
{

}

//***********************************************************************************************************************************

void DoDoubleFireRight(void)
{

}

//***********************************************************************************************************************************

void DoRightDownSwitch(void)
{



}

//***********************************************************************************************************************************

void DoLeftDownSwitch(void)
{


}

//***********************************************************************************************************************************

void DoFuelSwitch(void)
{
  FuelSwitch[FuelSwitchState].FramesToDraw = 0;
  FuelSwitchState++;
  if(FuelSwitchState > SWITCH_FUEL_CENTER)
	 FuelSwitchState = SWITCH_FUEL_LEFT_RIGHT;

  FuelSwitch[FuelSwitchState].FramesToDraw = 2;
}

//***********************************************************************************************************************************

void DoJetPush(void)
{
  if ((PlayerPlane->OnGround) || (PlayerPlane->AGL <= 20))  return;	// Disable ALL ordnance till take-off.

  JetPush[JetPushState].FramesToDraw = 0;
  JetPushState = JETTISON_IN;
  JetPush[JetPushState].FramesToDraw = 2;

  switch(JetWeapSelectState)
  {
    case JETTISON_OFF:    JettisonAA   = FALSE;
                          JettisonFuel = FALSE;
                          JettisonAG   = FALSE;
	                      break;
   	case JETTISON_ALL:    JettisonAA   = TRUE;
                          JettisonFuel = TRUE;
                          JettisonAG   = TRUE;
                          break;
 	case JETTISON_AG:     JettisonAA   = FALSE;
                          JettisonFuel = FALSE;
                          JettisonAG   = TRUE;
                          break;
	case JETTISON_AA:     JettisonAA   = TRUE;
                          JettisonFuel = FALSE;
                          JettisonAG   = FALSE;
                          break;
 	case JETTISON_COMBAT: JettisonAA   = FALSE;
                          JettisonFuel = TRUE;
                          JettisonAG   = FALSE;
                          break;
  }

  SimJettisonOn = TRUE;
  JetPushOn = TRUE;
  JetPushStartTime = GameLoopInTicks;
}

//***********************************************************************************************************************************

void DoJetWeapSelect(void)
{
  JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw = 0;

  JetWeapSelectState += JetWeapSelectDirection;

  if( (JetWeapSelectState > JETTISON_AA) || (JetWeapSelectState < JETTISON_OFF) )
  {
    if(JetWeapSelectState > JETTISON_AA)
     JetWeapSelectState -= 2;

    if(JetWeapSelectState < JETTISON_OFF)
     JetWeapSelectState += 2;

    JetWeapSelectDirection = -JetWeapSelectDirection;
  }

  JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw = 2;

}

//***********************************************************************************************************************************

void DoMasterArmSwitch(void)
{
  MasterArmSwitch[MasterArmSwitchState].FramesToDraw = 0;

  MasterArmSwitchState++;
  if(MasterArmSwitchState > ON)
	  MasterArmSwitchState = OFF;

  MasterArmSwitch[MasterArmSwitchState].FramesToDraw = 2;
  //ViewChangeCountDown = 2;

  UFC.MasterArmState = MasterArmSwitchState;
}

//***********************************************************************************************************************************

void DoBingoArrowUp()
{
  BingoPercent += 0.1;
  if(BingoPercent > 1.0) BingoPercent = 1.0;

  if(!BingoUpOn)
  {
    BingoUpStartTime = (GameLoopInTicks /*John changed this from GetTickCount()*/);
    BingoUpOn = TRUE;
  }
}

//***********************************************************************************************************************************

void DoBingoArrowDown()
{
  BingoPercent -= 0.1;
  if(BingoPercent < 0.0) BingoPercent = 0.0;

  if(!BingoDownOn)
  {
    BingoDownStartTime = (GameLoopInTicks /*John changed this from GetTickCount()*/);
    BingoDownOn = TRUE;
  }
}


//***********************************************************************************************************************************
//** KEY COMMANDS FOR LOOK DOWN SPRITES

void ToggleMasterArmSwitch()
{
  DoMasterArmSwitch();
}

void JettisonGivenLoad(int State)
{
  JetWeapSelectState = State;
  JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw = 2;

  DoJetPush();
}

void ActivateAmadFireExt()
{

	ReadyDischargeClick( &(CpitItems[34].State));
	/*
	FEButtons[0].PushState = PUSH;
	SingleFireState =  GetSingleFireState();
	SingleFire[SingleFireState].FramesToDraw = 2;
	DoFireExtSwitchUp();
	*/
}

void ActivateLeftEngFireExt()
{
	ReadyDischargeClick( &(CpitItems[34].State));

	/*
	FEButtons[1].PushState = PUSH;
	DoubleFireState =  GetFireExtState();
	DoubleFire[DoubleFireState].FramesToDraw = 2;
	DoFireExtSwitchUp();
	*/
}

void ActivateRightEngFireExt()
{
	ReadyDischargeClick( &(CpitItems[34].State));

	/*
	FEButtons[2].PushState = PUSH;
	DoubleFireState =  GetFireExtState();
	DoubleFire[DoubleFireState].FramesToDraw = 2;
	DoFireExtSwitchUp();
	*/
}


//***********************************************************************************************************************************

void UpdateLookDownSprites()
{
  int PointerX, PointerY;

  PointerX = ppCurrMouse.x;
  PointerY = ppCurrMouse.y;

  int Offset = (CurrentCockpit == FRONT_DOWN_COCKPIT) ? 0 : 201;

  if(IsPointInside(PointerX,PointerY, FireExtSwitchBoundBox[0].Top + Offset, FireExtSwitchBoundBox[0].Bottom + Offset, FireExtSwitchBoundBox[0].Left, FireExtSwitchBoundBox[0].Right))
  {
	  DoFireExtSwitchMiddle();
	  return;
  }
  if(IsPointInside(PointerX,PointerY, FireExtSwitchBoundBox[1].Top + Offset, FireExtSwitchBoundBox[1].Bottom + Offset, FireExtSwitchBoundBox[1].Left, FireExtSwitchBoundBox[1].Right))
  {
	  DoFireExtSwitchDown();
	  return;
  }
  if(IsPointInside(PointerX,PointerY, FireExtSwitchBoundBox[2].Top + Offset, FireExtSwitchBoundBox[2].Bottom + Offset, FireExtSwitchBoundBox[2].Left, FireExtSwitchBoundBox[2].Right))
  {
	  DoFireExtSwitchUp();
	  return;
  }

  if(IsPointInside(PointerX,PointerY, LeftDownSwitchBoundBox.Top + Offset, LeftDownSwitchBoundBox.Bottom + Offset, LeftDownSwitchBoundBox.Left, LeftDownSwitchBoundBox.Right))
  {
	  DoLeftDownSwitch();
	  return;
  }

  if(IsPointInside(PointerX,PointerY, RightDownSwitchBoundBox.Top + Offset, RightDownSwitchBoundBox.Bottom + Offset, RightDownSwitchBoundBox.Left, RightDownSwitchBoundBox.Right))
  {
	  DoRightDownSwitch();
	  return;
  }

  if(IsPointInside(PointerX,PointerY, DoubleFireBoundBox[0].Top + Offset, DoubleFireBoundBox[0].Bottom + Offset, DoubleFireBoundBox[0].Left, DoubleFireBoundBox[0].Right))
  {
	  DoDoubleFireLeft();
	  return;
  }

  if(IsPointInside(PointerX,PointerY, DoubleFireBoundBox[1].Top + Offset, DoubleFireBoundBox[1].Bottom + Offset, DoubleFireBoundBox[1].Left, DoubleFireBoundBox[1].Right))
  {
	  DoDoubleFireRight();
	  return;
  }

  if(IsPointInside(PointerX,PointerY, SingleFireBoundBox.Top + Offset, SingleFireBoundBox.Bottom + Offset, SingleFireBoundBox.Left, SingleFireBoundBox.Right))
  {
	  DoSingleFire();
	  return;
  }

  if(CurrentCockpit == FRONT_DOWN_COCKPIT)
  {

    if(IsPointInside(PointerX,PointerY, JetWeapSelectSwitchBoundBox.Top + Offset, JetWeapSelectSwitchBoundBox.Bottom + Offset, JetWeapSelectSwitchBoundBox.Left, JetWeapSelectSwitchBoundBox.Right))
    {
	  DoJetWeapSelect();
	  return;
    }

    if(IsPointInside(PointerX,PointerY, JetPushBoundBox.Top + Offset, JetPushBoundBox.Bottom + Offset, JetPushBoundBox.Left, JetPushBoundBox.Right))
    {
	  DoJetPush();
	  return;
    }

    if(IsPointInside(PointerX,PointerY, FuelSwitchBoundBox.Top + Offset, FuelSwitchBoundBox.Bottom + Offset, FuelSwitchBoundBox.Left, FuelSwitchBoundBox.Right))
    {
	  DoFuelSwitch();
	  return;
    }

    if(IsPointInside(PointerX,PointerY, MasterArmSwitchBoundBox.Top + Offset, MasterArmSwitchBoundBox.Bottom + Offset, MasterArmSwitchBoundBox.Left, MasterArmSwitchBoundBox.Right))
    {
	  DoMasterArmSwitch();
	  return;
    }

    if(IsPointInside(PointerX,PointerY, BingoArrowsBoundBox[0].Top + Offset, BingoArrowsBoundBox[0].Bottom + Offset, BingoArrowsBoundBox[0].Left, BingoArrowsBoundBox[0].Right))
    {
	  DoBingoArrowUp();
	  return;
    }

	if(IsPointInside(PointerX,PointerY, BingoArrowsBoundBox[1].Top + Offset, BingoArrowsBoundBox[1].Bottom + Offset, BingoArrowsBoundBox[1].Left, BingoArrowsBoundBox[1].Right))
    {
	  DoBingoArrowDown();
	  return;
    }


  }

}

//***********************************************************************************************************************************

void DrawLookDownSprites()
{
	/*
	int Offset;

	if(ViewChangeCountDown > 0)
	{
	  FireExtSwitch[FireExtState].FramesToDraw = 5;
      JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw = 5;
	  JetPush[JetPushState].FramesToDraw = 5;
      FuelSwitch[FuelSwitchState].FramesToDraw = 5;
	  LeftDownSwitch[LeftDownSwitchState].FramesToDraw = 5;
	  RightDownSwitch[RightDownSwitchState].FramesToDraw = 5;
	  MasterArmSwitch[MasterArmSwitchState].FramesToDraw = 5;
      DoubleFire[DoubleFireState].FramesToDraw = 5;
      SingleFire[SingleFireState].FramesToDraw = 5;
	}

    if(FireExtSwitch[FireExtState].FramesToDraw > 0)
	{
	   Offset = (CurrentCockpit == FRONT_DOWN_COCKPIT) ? 0 : 201;

	   GrCopyRectNoClip(GrBuffFor3D, FireExtSwitch[FireExtState].ScreenX,FireExtSwitch[FireExtState].ScreenY + Offset, FireExtSwitch[FireExtState].Buff, 0,0,
		                FireExtSwitch[FireExtState].Width, FireExtSwitch[FireExtState].Height);
       if(FireExtSwitch[FireExtState].FramesToDraw > 0)
		   FireExtSwitch[FireExtState].FramesToDraw--;
	}

    if(LeftDownSwitch[LeftDownSwitchState].FramesToDraw > 0)
	{
	   Offset = (CurrentCockpit == FRONT_DOWN_COCKPIT) ? 0 : 201;

	   GrCopyRectNoClip(GrBuffFor3D,LeftDownSwitch[LeftDownSwitchState].ScreenX,LeftDownSwitch[LeftDownSwitchState].ScreenY + Offset, LeftDownSwitch[LeftDownSwitchState].Buff, 0,0,
		                LeftDownSwitch[LeftDownSwitchState].Width, LeftDownSwitch[LeftDownSwitchState].Height);
       if(LeftDownSwitch[LeftDownSwitchState].FramesToDraw > 0)
		     LeftDownSwitch[LeftDownSwitchState].FramesToDraw--;
   	}

	if(RightDownSwitch[RightDownSwitchState].FramesToDraw > 0)
	{
	   Offset = (CurrentCockpit == FRONT_DOWN_COCKPIT) ? 0 : 201;

	   GrCopyRectNoClip(GrBuffFor3D,RightDownSwitch[RightDownSwitchState].ScreenX,RightDownSwitch[RightDownSwitchState].ScreenY + Offset, RightDownSwitch[RightDownSwitchState].Buff, 0,0,
		                RightDownSwitch[RightDownSwitchState].Width, RightDownSwitch[RightDownSwitchState].Height);
       if(RightDownSwitch[RightDownSwitchState].FramesToDraw > 0)
		     RightDownSwitch[RightDownSwitchState].FramesToDraw--;
   	}

    if(SingleFire[SingleFireState].FramesToDraw > 0)
	{
	   Offset = (CurrentCockpit == FRONT_DOWN_COCKPIT) ? 0 : 201;

	   GrCopyRectNoClip(GrBuffFor3D,SingleFire[SingleFireState].ScreenX,SingleFire[SingleFireState].ScreenY + Offset, SingleFire[SingleFireState].Buff, 0,0,
		                SingleFire[SingleFireState].Width, SingleFire[SingleFireState].Height);
       if(SingleFire[SingleFireState].FramesToDraw > 0)
		     SingleFire[SingleFireState].FramesToDraw--;
   	}

	if(DoubleFire[DoubleFireState].FramesToDraw > 0)
	{
	   Offset = (CurrentCockpit == FRONT_DOWN_COCKPIT) ? 0 : 201;

	   GrCopyRectNoClip(GrBuffFor3D,DoubleFire[DoubleFireState].ScreenX,DoubleFire[DoubleFireState].ScreenY + Offset, DoubleFire[DoubleFireState].Buff, 0,0,
		                DoubleFire[DoubleFireState].Width, DoubleFire[DoubleFireState].Height);
       if(DoubleFire[DoubleFireState].FramesToDraw > 0)
		     DoubleFire[DoubleFireState].FramesToDraw--;
   	}

	if(CurrentCockpit == FRONT_DOWN_COCKPIT)
	{
	  if(JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw > 0)
	  {
	     GrCopyRectNoClip(GrBuffFor3D,JetWeapSelectSwitch[JetWeapSelectState].ScreenX,JetWeapSelectSwitch[JetWeapSelectState].ScreenY, JetWeapSelectSwitch[JetWeapSelectState].Buff, 0,0,
		                JetWeapSelectSwitch[JetWeapSelectState].Width, JetWeapSelectSwitch[JetWeapSelectState].Height);
         if(JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw > 0)
		   JetWeapSelectSwitch[JetWeapSelectState].FramesToDraw--;
   	  }

	  if(JetPush[JetPushState].FramesToDraw > 0)
	  {
	     GrCopyRectNoClip(GrBuffFor3D,JetPush[JetPushState].ScreenX,JetPush[JetPushState].ScreenY, JetPush[JetPushState].Buff, 0,0,
		                JetPush[JetPushState].Width, JetPush[JetPushState].Height);
         if(JetPush[JetPushState].FramesToDraw > 0)
		     JetPush[JetPushState].FramesToDraw--;
   	  }

	  if(FuelSwitch[FuelSwitchState].FramesToDraw > 0)
	  {
	     GrCopyRectNoClip(GrBuffFor3D,FuelSwitch[FuelSwitchState].ScreenX,FuelSwitch[FuelSwitchState].ScreenY, FuelSwitch[FuelSwitchState].Buff, 0,0,
		                FuelSwitch[FuelSwitchState].Width, FuelSwitch[FuelSwitchState].Height);
         if(FuelSwitch[FuelSwitchState].FramesToDraw > 0)
		     FuelSwitch[FuelSwitchState].FramesToDraw--;
   	  }

	  if(MasterArmSwitch[MasterArmSwitchState].FramesToDraw > 0)
	  {
	     GrCopyRectNoClip(GrBuffFor3D,MasterArmSwitch[MasterArmSwitchState].ScreenX,MasterArmSwitch[MasterArmSwitchState].ScreenY, MasterArmSwitch[MasterArmSwitchState].Buff, 0,0,
		                MasterArmSwitch[MasterArmSwitchState].Width, MasterArmSwitch[MasterArmSwitchState].Height);
         if(MasterArmSwitch[MasterArmSwitchState].FramesToDraw > 0)
		     MasterArmSwitch[MasterArmSwitchState].FramesToDraw--;
   	  }

	  if(BingoUpStartTime != -1)
	  {
        if((GameLoopInTicks ) - BingoUpStartTime < 300)//John changed this from GetTickCount()
		{
	       GrCopyRectNoClip(GrBuffFor3D,BingoArrows[0].ScreenX,BingoArrows[0].ScreenY, BingoArrows[0].Buff, 0,0,
		                BingoArrows[0].Width, BingoArrows[0].Height);
		}
		else
		{
		  BingoUpStartTime    = -1;
		  BingoUpOn           = FALSE;
          ViewChangeCountDown = 2;
		}
	  }

	  if(BingoDownStartTime != -1)
	  {
        if((GameLoopInTicks ) - BingoDownStartTime < 300)///John changed this from GetTickCount()
		{
	       GrCopyRectNoClip(GrBuffFor3D,BingoArrows[1].ScreenX,BingoArrows[1].ScreenY, BingoArrows[1].Buff, 0,0,
		                BingoArrows[1].Width, BingoArrows[1].Height);
		}
		else
		{
		  BingoDownStartTime  = -1;
		  BingoDownOn         = FALSE;
          ViewChangeCountDown = 2;
		}
      }

	}
	*/
}

//***********************************************************************************************************************************

void InitLookDown()
{
	FireExtState = SWITCH_MIDDLE;
    JetWeapSelectState = JETTISON_OFF;
	JetWeapSelectDirection = 1;
    JetPushState = JETTISON_OUT;
    LeftDownSwitchState =  SWITCH_HUD_DECLUTTER_OFF;
    RightDownSwitchState = (UseDayCockpit) ? SWITCH_DAY : SWITCH_NIGHT; // UseDayCockpit set in simmain
	SingleFireState = 0;
	DoubleFireState = 12;
	MasterArmSwitchState = ON;
	BingoUpOn = BingoDownOn = FALSE;
    BingoUpStartTime = BingoDownStartTime = -1;
	BingoPercent = 0.285;

	FEButtons[0].PushState = RELEASE;
    FEButtons[0].LightOn   = FALSE;
	FEButtons[1].PushState = RELEASE;
    FEButtons[1].LightOn   = FALSE;
	FEButtons[2].PushState = RELEASE;
    FEButtons[2].LightOn   = FALSE;
	JetPushOn = FALSE;
	JetPushStartTime = -1;

	CanSimFireRight = TRUE;
	CanSimFireLeft = TRUE;
	RightExtinguishOn = FALSE;
    LeftExtinguishOn = FALSE;
 }

//***********************************************************************************************************************************

void LoadLookDownSprites()
{



}

 //***********************************************************************************************************************************
//*	CODE FOR FIRE EXTINGUISHER
//***********************************************************************************************************************************

/*
#define HEAT_UP_LEFT_ENGINE		0x00000001
#define FIRE_LEFT_ENGINE		0x00000002
#define HEAT_UP_RIGHT_ENGINE	0x00000010
#define FIRE_RIGHT_ENGINE		0x00000020
#define HEAT_UP_AMAD_LEFT		0x00000100
#define FIRE_AMAD_LEFT			0x00000200
#define HEAT_UP_AMAD_RIGHT		0x00001000
#define FIRE_AMAD_RIGHT			0x00002000
*/

/*
long                lLeftTimeToFire = -1;
long                lRightTimeToFire = -1;
long				lLAMADTimeToFire = -1;
long				lRAMADTimeToFire = -1;
long				lLeftTimeToExplode = 90000;
long				lRightTimeToExplode = 90000;
long				lLAMADTimeToExplode = 90000;
long				lRAMADTimeToExplode = 90000;

  */

void SimFireExtinguish()
{
	int BaseFactor = 10;

	if(lFireFlags & FIRE_LEFT_ENGINE)
	{
  		int RandNum = rand() % 100;
		//RandNum = RandNum - 15*(float)(90000 - lLeftTimeToExplode)/90000.0;
		if(RandNum > BaseFactor)
			 ADEngineFireOut(TRUE);
		else
			 ADEngineFireTempOut(TRUE);

		BaseFactor += 30; // make it harder to turn off both engiines
	}

	if(lFireFlags & FIRE_RIGHT_ENGINE)
	{
		int RandNum = rand() % 100;
		//RandNum = RandNum - 15*(float)(90000 - lLeftTimeToExplode)/90000.0;
		if(RandNum > BaseFactor)
			ADEngineFireOut(FALSE);
		else
			ADEngineFireTempOut(FALSE);

		BaseFactor += 30; // make it harder to turn off both engiines
	}

    if(lFireFlags & (FIRE_AMAD_LEFT|FIRE_AMAD_RIGHT) )
    {
  	    int RandNum = rand() % 100;
		//RandNum = RandNum - 15*(float)(180000 - lLAMADTimeToExplode)/90000.0;
	    if(RandNum > BaseFactor)
			ADEngineFireOut(FALSE,1);
		else
			ADEngineFireTempOut(FALSE,1);
    }

}


//***********************************************************************************************************************************

void CleanUpLookDownSprites()
{
	int i;

	PauseForDiskHit();

    for (i=0;i<NUM_FIRE_EXT_SWITCH_SPRITES;i++)
       GrFreeGrBuff(FireExtSwitch[i].Buff);
   	for (i=0;i<NUM_JET_WEAP_SELECT_SWITCH_SPRITES;i++)
	   GrFreeGrBuff(JetWeapSelectSwitch[i].Buff);
	for (i=0;i<NUM_JETTISON_PUSH_SPRITES;i++)
	   GrFreeGrBuff(JetPush[i].Buff);
	for (i=0;i<NUM_FUEL_SWITCH_SPRITES;i++)
	   GrFreeGrBuff(FuelSwitch[i].Buff);
	for (i=0;i<NUM_LEFT_DOWN_SWITCH_SPRITES;i++)
	   GrFreeGrBuff(LeftDownSwitch[i].Buff);
	for (i=0;i<NUM_RIGHT_DOWN_SWITCH_SPRITES;i++)
	   GrFreeGrBuff(RightDownSwitch[i].Buff);
	for (i=0;i<NUM_DOUBLE_FIRE_SPRITES;i++)
       GrFreeGrBuff(DoubleFire[i].Buff);
	for (i=0;i<NUM_SINGLE_FIRE_SPRITES;i++)
       GrFreeGrBuff(SingleFire[i].Buff);
	for (i=0;i<NUM_MASTER_ARM_SPRITES;i++)
       GrFreeGrBuff(MasterArmSwitch[i].Buff);
	for (i=0;i<2;i++)
       GrFreeGrBuff(BingoArrows[i].Buff);

	UnPauseFromDiskHit();

}

//***********************************************************************************************************************************

void SetDayNightCockpit()
{
  if( (WorldParams.WorldTime < TOD_DAWN) || (WorldParams.WorldTime > TOD_DUSK) )
    DoRightDownSwitch();
 }


//***********************************************************************************************************************************
void SetMasterWarning(int Warning)
{
	if (g_iSoundLevelCaution)
	{
		int iLevel=g_iSoundLevelCaution-20;
		if (iLevel<0) iLevel=5;
		SndQueueSound(SND_MASTER_CAUTION,1,iLevel);
		SndServiceSound();
	}

#if 0
   switch(Warning)
   {
	  case WARN_L_ENG_CONT :
		  SetBackSeatWarning(BCK_WARN_L_ENG);
		  break;
	  case WARN_R_ENG_CONT :
		  SetBackSeatWarning(BCK_WARN_R_ENG);
		  break;
      case WARN_MAIN_HYD :
		  SetBackSeatWarning(BCK_WARN_HYD_PC1);
		  break;
	  case WARN_ENER_HYD :
		  SetBackSeatWarning(BCK_WARN_HYD_PC2);
		  break;
	  case WARN_L_GEN :
		  SetBackSeatWarning(BCK_WARN_L_GEN);
		  break;
	  case WARN_R_GEN :
		  SetBackSeatWarning(BCK_WARN_R_GEN);
		  break;
	  case WARN_CHAFF:
		  SetBackSeatWarning(BCK_WARN_CHAFF);
		  break;
	  case WARN_FLARE:
		  SetBackSeatWarning(BCK_WARN_FLR);
		  break;
	  case  WARN_FLT_CONT:
		  SetBackSeatWarning(BCK_WARN_FLT_CONT);
		  break;
	  case  WARN_FUEL_LOW:
		  SetBackSeatWarning(BCK_WARN_FUEL_LOW);
		  break;
	  case  WARN_MINIMUM:
		  SetBackSeatWarning(BCK_WARN_MIN);
		  break;
	  case WARN_AUTOPILOT:
		  SetBackSeatWarning(BCK_WARN_A_P);
		  break;
	  case WARN_CEN_COMP:
		  SetBackSeatWarning(BCK_WARN_C_C);
		  break;
   }
#endif

}

//***********************************************************************************************************************************
void ClearMasterWarning(int Warning)
{			   
#if 0
   switch(Warning)
   {
	  case WARN_L_ENG_CONT :
		  ClearBackSeatWarning(BCK_WARN_L_ENG);
		  break;
	  case WARN_R_ENG_CONT :
		  ClearBackSeatWarning(BCK_WARN_R_ENG);
		  break;
	  case WARN_MAIN_HYD :
		  ClearBackSeatWarning(BCK_WARN_HYD_PC1);
		  break;
	  case WARN_ENER_HYD :
		  ClearBackSeatWarning(BCK_WARN_HYD_PC2);
		  break;
	  case WARN_L_GEN :
		  ClearBackSeatWarning(BCK_WARN_L_GEN);
		  break;
	  case WARN_R_GEN :
		  ClearBackSeatWarning(BCK_WARN_R_GEN);
		  break;
	  case WARN_CHAFF:
		  ClearBackSeatWarning(BCK_WARN_CHAFF);
		  break;
	  case WARN_FLARE:
		  ClearBackSeatWarning(BCK_WARN_FLR);
		  break;
	  case  WARN_FLT_CONT:
		  ClearBackSeatWarning(BCK_WARN_FLT_CONT);
		  break;
	  case  WARN_FUEL_LOW:
		  ClearBackSeatWarning(BCK_WARN_FUEL_LOW);
		  break;
	  case  WARN_MINIMUM:
		  ClearBackSeatWarning(BCK_WARN_MIN);
		  break;
   }
#endif
}

//***********************************************************************************************************************************
//* CIRCLE DRAWING ROUTINES

void DrawQuadrantsNoClip(GrBuff *Buff, int X,int Y, int OriginX, int OriginY, int ColorRed, int Green, int Blue, int PAL8) //Color)
{

	GrDrawDotNoClip(Buff,OriginX + X, OriginY - Y,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX + Y, OriginY - X,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX + Y, OriginY + X,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX + X, OriginY + Y,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX - X, OriginY + Y,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX - Y, OriginY + X,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX - Y, OriginY - X,	ColorRed, Green, Blue, PAL8 );//Color);
	GrDrawDotNoClip(Buff,OriginX - X, OriginY - Y,	ColorRed, Green, Blue, PAL8 );//Color);

}


void DrawCircleNoClip(GrBuff *Buff, int OriginX, int OriginY, int Radius, int ColorRed, int Green, int Blue, int PAL8)//Color)
{

  int TempX   = 0;
  int TempY   = Radius;
  int Error   = 1 - Radius;
  int DeltaSE = ((-2)*Radius) + 5;
  int DeltaE = 3;

  DrawQuadrantsNoClip(Buff,TempX,TempY,OriginX,OriginY, ColorRed, Green, Blue, PAL8);//Color);

  while(TempY > TempX)
  {
    if(Error < 0)
    {
      Error += DeltaE;
      DeltaE += 2;
      DeltaSE += 2;
      TempX++;
    }
    else
    {
      Error += DeltaSE;
      DeltaE += 2;
      DeltaSE += 4;
      TempX++;
      TempY--;
    }

	DrawQuadrantsNoClip(Buff,TempX,TempY,OriginX,OriginY, ColorRed, Green, Blue, PAL8 );//Color);

  }


}

int IsPointInside(int X,int Y, int Top, int Bottom, int Left, int Right)
{
   if(X > Left)
	if(X < Right)
	  if(Y > Top)
	    if(Y < Bottom)
		  return(TRUE);

   return(FALSE);
}


void DrawQuadrantsClip(GrBuff *Buff, int X,int Y, int OriginX, int OriginY, int ColorRed, int Green, int Blue, int Pal8)
{


  if(IsPointInside(OriginX + X, OriginY - Y, Buff->ClipTop,
	               Buff->ClipBottom, Buff->ClipLeft, Buff->ClipRight))
      GrDrawDotNoClip(Buff,OriginX + X, OriginY - Y,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX + Y,OriginY - X, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
      GrDrawDotNoClip(Buff,OriginX + Y, OriginY - X,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX + Y,OriginY + X, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
	  GrDrawDotNoClip(Buff,OriginX + Y, OriginY + X,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX + X,OriginY + Y, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
      GrDrawDotNoClip(Buff,OriginX + X, OriginY + Y,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX - X,OriginY + Y, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
	  GrDrawDotNoClip(Buff,OriginX - X, OriginY + Y,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX - Y,OriginY + X, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
	  GrDrawDotNoClip(Buff,OriginX - Y, OriginY + X,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX - Y,OriginY - X, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
	  GrDrawDotNoClip(Buff,OriginX - Y, OriginY - X,ColorRed, Green, Blue, Pal8);

  if(IsPointInside(OriginX - X,OriginY - Y, Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
	  GrDrawDotNoClip(Buff,OriginX - X, OriginY - Y,ColorRed, Green, Blue, Pal8);

}


void DrawCircleClip(GrBuff *Buff, int OriginX, int OriginY, int Radius, int ColorRed, int Green, int Blue, int PAL8)
{

  int TempX   = 0;
  int TempY   = Radius;
  int Error   = 1 - Radius;
  int DeltaSE = ((-2)*Radius) + 5;
  int DeltaE = 3;

  int P1Inside,P2Inside,P3Inside,P4Inside;

  // top
  P1Inside = IsPointInside(OriginX + TempX, OriginY - TempY,
	                       Buff->ClipTop, Buff->ClipBottom,
						   Buff->ClipLeft, Buff->ClipRight);
  // right
  P2Inside = IsPointInside(OriginX + TempY, OriginY + TempX,
	                       Buff->ClipTop, Buff->ClipBottom,
						   Buff->ClipLeft, Buff->ClipRight);
  // bottom
  P3Inside = IsPointInside(OriginX - TempX, OriginY + TempY,
	                       Buff->ClipTop, Buff->ClipBottom,
						   Buff->ClipLeft, Buff->ClipRight);
  // left
  P4Inside = IsPointInside(OriginX - TempY, OriginY - TempX,
	                       Buff->ClipTop, Buff->ClipBottom,
						   Buff->ClipLeft, Buff->ClipRight);

  // if circle is fully within clip region, then use the "NoClip" routine and
  // return immediately
  if(P1Inside && P2Inside && P3Inside && P4Inside)
  {
 	DrawCircleNoClip(Buff, OriginX, OriginY, Radius, ColorRed, Green, Blue, PAL8);
     	return;
  }

  // if all points are outside the clip region, return immediately
  if(!P1Inside && !P2Inside && !P3Inside && !P4Inside)
	  return;

  DrawQuadrantsClip(Buff,TempX,TempY,OriginX,OriginY, ColorRed, Green, Blue, PAL8);

  while(TempY > TempX)
  {
    if(Error < 0)
    {
      Error += DeltaE;
      DeltaE += 2;
      DeltaSE += 2;
      TempX++;
    }
    else
    {
      Error += DeltaSE;
      DeltaE += 2;
      DeltaSE += 4;
      TempX++;
      TempY--;
    }

	DrawQuadrantsClip(Buff, TempX,TempY, OriginX, OriginY, ColorRed, Green, Blue, PAL8);

  }


}



//** DASHED CIRCLE *******************************************************************


void AddPixelToList(int X, int Y, int Index)
{
  DashedCircleArcBuffer[Index].X = X;
  DashedCircleArcBuffer[Index].Y = Y;
}

void CreateDashedCircleArray(int PixelTotal, int OriginX, int OriginY, int XVal, int YVal)
{
  int i;
  int Y_Space = 200;

  DashedCircleBufferType *BufPtr;
  DashedCircleBufferType *FirstBufPtr;
  DashedCircleBufferType *LastBufPtr;

  FirstBufPtr = &DashedCircleArcBuffer[0];
  LastBufPtr = &DashedCircleArcBuffer[PixelTotal - 1];

  BufPtr = FirstBufPtr;
  for(i=0; i<PixelTotal; i++)
  {
    DashedCircleBuffer[i].X = OriginX + BufPtr->X;
	DashedCircleBuffer[i].Y = OriginY - BufPtr->Y;
    BufPtr++;
  }



  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 1); i>PixelTotal; i--)  // (PT*2); (PT)
  {
    DashedCircleBuffer[i].X = OriginX + BufPtr->Y;
	DashedCircleBuffer[i].Y = OriginY - BufPtr->X;
	BufPtr++;
  }


  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 1); i<(PixelTotal << 1) + PixelTotal; i++) //(PT*2); (PT*3);
  {
    DashedCircleBuffer[i].X = OriginX + BufPtr->Y;
	DashedCircleBuffer[i].Y = OriginY + BufPtr->X;
	BufPtr++;
  }


  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 2); i>(PixelTotal << 1) + PixelTotal; i--) //(PT*4); (PT*3);
  {
    DashedCircleBuffer[i].X = OriginX + BufPtr->X;
	DashedCircleBuffer[i].Y = OriginY + BufPtr->Y;
	BufPtr++;
  }


  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 2); i<(PixelTotal << 2) + PixelTotal; i++) // (PT*4); (PT*5);
  {
    DashedCircleBuffer[i].X = OriginX - BufPtr->X;
	DashedCircleBuffer[i].Y = OriginY + BufPtr->Y;
	BufPtr++;
  }


  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 2) + (PixelTotal << 1); i>(PixelTotal << 2) + PixelTotal; i--)
  {															  //(PT*6); (PT*5);
    DashedCircleBuffer[i].X = OriginX - BufPtr->Y;
	DashedCircleBuffer[i].Y = OriginY + BufPtr->X;
	BufPtr++;
  }


  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 2) + (PixelTotal << 1); i<(PixelTotal << 3) - PixelTotal; i++)
  {														     //(PT*6); (PT*7);
    DashedCircleBuffer[i].X = OriginX - BufPtr->Y;
	DashedCircleBuffer[i].Y = OriginY - BufPtr->X;
	BufPtr++;
  }


  BufPtr = FirstBufPtr;
  for(i=(PixelTotal << 3); i>(PixelTotal << 3) - PixelTotal; i--)
  {													        //(PT*8); (PT*7)
    DashedCircleBuffer[i].X = OriginX - BufPtr->X;
	DashedCircleBuffer[i].Y = OriginY - BufPtr->Y;
	BufPtr++;
  }

  DashedCircleBuffer[PixelTotal].X = OriginX + XVal;
  DashedCircleBuffer[PixelTotal].Y = OriginY - YVal;

  DashedCircleBuffer[(PixelTotal << 1) + PixelTotal].X = OriginX + XVal;
  DashedCircleBuffer[(PixelTotal << 1) + PixelTotal].Y = OriginY + YVal;

  DashedCircleBuffer[(PixelTotal << 2) + PixelTotal].X = OriginX - XVal;
  DashedCircleBuffer[(PixelTotal << 2) + PixelTotal].Y = OriginY + YVal;

  DashedCircleBuffer[(PixelTotal << 3) - PixelTotal].X = OriginX - XVal;
  DashedCircleBuffer[(PixelTotal << 3) - PixelTotal].Y = OriginY - YVal;

}


void DrawDashedCircle(GrBuff *Buff, int DrawP, int BlankP, int Start,  int End, int  ColorRed, int Green, int Blue, int Pal8, int Clip)
{
  int i;
  int Draw;

  Draw = 1;
  if(Clip)
  {
    for(i=Start; i<End; i++)
    {
	  if(Draw > 0)
	  {
   		if(IsPointInside(DashedCircleBuffer[i].X,DashedCircleBuffer[i].Y,Buff->ClipTop,Buff->ClipBottom,
				   Buff->ClipLeft, Buff->ClipRight))
	       GrDrawDotNoClip(Buff, DashedCircleBuffer[i].X, DashedCircleBuffer[i].Y, ColorRed, Green, Blue, Pal8);

		if(Draw == DrawP)
			 Draw = -BlankP;
  	  }
	  Draw++;
    }
  }
  else
  {
    for(i=Start; i<End; i++)
    {
	  if(Draw > 0)
	  {
   		GrDrawDotNoClip(Buff, DashedCircleBuffer[i].X, DashedCircleBuffer[i].Y, ColorRed, Green, Blue, Pal8);
	    if(Draw == DrawP)
	       Draw = -BlankP;
  	  }
	  Draw++;
    }
  }

}

void DrawCircleDashed(GrBuff *Buff, int OriginX, int OriginY, int Radius, int ColorRed, int Green, int Blue, int PAL8, int Clip)
{
  int TempX   = 0;
  int TempY   = Radius;
  int Error   = 1 - Radius;
  int DeltaSE = -(Radius << 1) + 5;	 // (-2)*Radius + 5
  int DeltaE = 3;
  int PixelNum;	   // used as an index
  int PixelTotal;  // number of pixels in 1/8th arc
  int Diameter;	   // num pixels in the circle
  int DrawP;	   // num pixels for dash
  int BlankP;  // num pixels for empty space between dashes
  int Rem;
  int Start,End; // where in circle drawing should start. This is used to take care
                 // of any overlap at the ned of the circle

  PixelNum=0;
  AddPixelToList(TempX,TempY, PixelNum);

  PixelTotal = 1;

  while(TempY > TempX)
  {
    if(Error < 0)
    {
      Error += DeltaE;
      DeltaE += 2;
      DeltaSE += 2;
      TempX++;
    }
    else
    {
      Error += DeltaSE;
      DeltaE += 2;
      DeltaSE += 4;
      TempX++;
      TempY--;
    }

    AddPixelToList(TempX,TempY, ++PixelNum);
	PixelTotal++;

  }

  DrawP = 8;
  BlankP = 5;

  Diameter = PixelTotal << 3; // PixelTotal*8
  Rem = int(Diameter) % (DrawP + BlankP);

  if(((Rem >= 2) && (Rem <= 5)) || ((Rem >= 10) && (Rem <= 13)))
	  Start = BlankP;
  else
	  Start = 0;

  End = PixelTotal << 3; // PixelTotal*8

  CreateDashedCircleArray(PixelTotal, OriginX, OriginY, TempX, TempY);
  DrawDashedCircle(Buff, DrawP, BlankP, Start, End, ColorRed, Green, Blue, PAL8, Clip);
}


void ToggleRadar()
{
	return;

   if(RadarDirection)
	RadarRadius += 5;
   else
	RadarRadius -= 5;

   if(RadarRadius > 90)
	   RadarDirection = !RadarDirection;
   if(RadarRadius < 10)
	   RadarDirection = !RadarDirection;
}

//** ITERATION #1 end *******************************************************************

void DrawGauge(int ScrX, int ScrY, int Length, double Val, double Max, int Wrap, float StartDegree)
{
  FPoint IndicatedNeedleEndPoint;
  double Percent;
  double IndicatedAngle;

  Percent = (Val/Max);

  if(Percent > 1.0)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 1.0;

  if(Wrap)	// if needle is meant to wrap around instead of be clamped
   IndicatedAngle = Percent*350.0;	// needle does not go all the way around
  else
   IndicatedAngle = Percent*360.0;


  IndicatedAngle = StartDegree + IndicatedAngle;

  if(IndicatedAngle >= 360.0)
	IndicatedAngle -= 360.0;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  IndicatedNeedleEndPoint.Y = ScrY + ISinTimes((IndicatedAngle*DEGREE), Length);


  GrDrawLine(GrBuffFor3D, ScrX, ScrY, IndicatedNeedleEndPoint.X,
	  IndicatedNeedleEndPoint.Y, UseDayCockpit ? 53 : 40);


}


void DrawAOAGauge(int ScrX, int ScrY, int Length, double Val, double Max)
{
  FPoint IndicatedNeedleEndPoint;
  double Percent;
  double IndicatedAngle;

  Percent = (Val/Max);

  if(Percent > 1.0)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 1.0;

  IndicatedAngle = Percent*350.0;	// needle does not go all the way around

  IndicatedAngle = 180.0 - IndicatedAngle;

  if(IndicatedAngle < 0.0)
	IndicatedAngle += 360.0;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  IndicatedNeedleEndPoint.Y = ScrY + ISinTimes((IndicatedAngle*DEGREE), Length);


  GrDrawLine(GrBuffFor3D, ScrX, ScrY, IndicatedNeedleEndPoint.X,
	          IndicatedNeedleEndPoint.Y, (UseDayCockpit ? 53 : 40));

}

void DrawVertVelGauge(int ScrX, int ScrY, int Length, double Val, double Max)
{
  FPoint IndicatedNeedleEndPoint;
  double Percent;
  double IndicatedAngle;

  Percent = (Val/Max);

  if(Percent > 1.0)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 1.0;
  if(Percent < -1.0)
    Percent = -1.0;

  IndicatedAngle = Percent*180.0;	// needle does not go all the way around

  if(IndicatedAngle < 0.0)
    IndicatedAngle = 180.0 + IndicatedAngle;
  else
    IndicatedAngle = 180.0 + IndicatedAngle;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  IndicatedNeedleEndPoint.Y = ScrY + ISinTimes((IndicatedAngle*DEGREE), Length);

  GrDrawLine(GrBuffFor3D, ScrX, ScrY, IndicatedNeedleEndPoint.X,
	          IndicatedNeedleEndPoint.Y, (UseDayCockpit ? 53 : 40));
}

void DrawAirspeedGauge(int ScrX, int ScrY, int Length, double Val, double Max)
{
  FPoint IndicatedNeedleEndPoint;
  double Percent;
  double IndicatedAngle;

  Percent = (Val/Max);

  if(Percent > 1.0)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 1.0;

  IndicatedAngle = Percent*340.0;	// needle does not go all the way around

  IndicatedAngle = 280.0 + IndicatedAngle;

  if(IndicatedAngle > 360.0)
   IndicatedAngle = IndicatedAngle - 360.0;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  IndicatedNeedleEndPoint.Y = ScrY + ISinTimes((IndicatedAngle*DEGREE), Length);

  GrDrawLine(GrBuffFor3D, ScrX, ScrY, IndicatedNeedleEndPoint.X,
	          IndicatedNeedleEndPoint.Y, (UseDayCockpit ? 53 : 40));
}


void DrawHalfGauge(int ScrX, int ScrY, int Length, double Val, double Max)
{
  FPoint IndicatedNeedleEndPoint;
  double Percent;
  double IndicatedAngle;

  Percent = (Val/Max);

  if(Percent > 0.9)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 0.9;

  IndicatedAngle = Percent*90.0;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  IndicatedNeedleEndPoint.Y = ScrY + ISinTimes((IndicatedAngle*DEGREE), Length);


  GrDrawLine(GrBuffFor3D, ScrX, ScrY, IndicatedNeedleEndPoint.X,
	          IndicatedNeedleEndPoint.Y, (UseDayCockpit ? 53 : 40));


}


void DrawFuelGauge(int ScrX, int ScrY, int Length, double Val, double Max)
{
  FPoint IndicatedNeedleEndPoint;
  FPoint StartDash;
  double Percent;
  double IndicatedAngle;

  Percent = (Val/Max);

  if(Percent > 0.9)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 0.9;

  IndicatedAngle = Percent*250.0;

  IndicatedAngle = 215.0 - IndicatedAngle;

  if(IndicatedAngle < 0.0)
	IndicatedAngle += 360.0 ;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  IndicatedNeedleEndPoint.Y = ScrY - ISinTimes((IndicatedAngle*DEGREE), Length);

  GrDrawLine(GrBuffFor3D, ScrX, ScrY, IndicatedNeedleEndPoint.X,
	          IndicatedNeedleEndPoint.Y, (UseDayCockpit ? 53 : 40));

  Percent = (BingoPercent);

  if(Percent > 0.9)	   // clamp percent to 90 so we don't go past the limit
	  Percent = 0.9;

  IndicatedAngle = Percent*250.0;

  IndicatedAngle = 215.0 - IndicatedAngle;

  if(IndicatedAngle < 0.0)
	IndicatedAngle += 360.0;

  IndicatedNeedleEndPoint.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length + 3);
  IndicatedNeedleEndPoint.Y = ScrY - ISinTimes((IndicatedAngle*DEGREE), Length + 3);

  StartDash.X = ScrX + ICosTimes((IndicatedAngle*DEGREE), Length);
  StartDash.Y = ScrY - ISinTimes((IndicatedAngle*DEGREE), Length);

  IndicatedAngle = (Percent*250.0) + 55.0;

  ANGLE Ang  = (IndicatedAngle*DEGREE);
  ANGLE Ang1 = (Ang + (ANGLE)(4*DEGREE));
  ANGLE Ang2 = (Ang - (ANGLE)(4*DEGREE));

  int X1 = ScrX - (signed int)(ISinTimes(Ang, Length+3));
  int Y1 = ScrY + (signed int)(ICosTimes(Ang, Length+3));

  int X2 = ScrX - (signed int)(ISinTimes(Ang1,Length));
  int Y2 = ScrY + (signed int)(ICosTimes(Ang1,Length));

  int X3 = ScrX - (signed int)(ISinTimes(Ang2,Length));
  int Y3 = ScrY + (signed int)(ICosTimes(Ang2,Length));

  GrDrawLine(GrBuffFor3D,X1,Y1,X2,Y2, 3);
  GrDrawLine(GrBuffFor3D,X1,Y1,X3,Y3, 3);

}

//** ADI GAUGES  *******************************************************************

int ClipTParam(float Denom, float Numer, float *TEnter, float *TLeave)
{
  int Accept;
  float T;

  Accept=TRUE;

  if (Denom > 0.0)
  {
	 T = (Numer)/(Denom);

	 if (T > (*TLeave))
	   Accept = FALSE;
	 else
	   if (T > (*TEnter))
	     (*TEnter) = T;
  }
  else
  {
	if (Denom < 0)
	{
	  T = (Numer)/(Denom);

	  if (T < (*TEnter))
	    Accept = FALSE;
	  else
	    if (T < (*TLeave))
	      (*TLeave) = T;
	}
	else
	{
	   if (Numer > 0)
		Accept = FALSE;

	}  // end if else


  } // end if else

    return(Accept);

}  // end proc




void ClipLineToRect(int *X0, int *Y0, int *X1, int *Y1, float XMin, float XMax,
					float YMin, float YMax, int *Visible, int *P1Edge, int *P2Edge)
{
  float DeltaX, DeltaY;
  float TEnter, TLeave, OldTLeave, OldTEnter;
  int Count = 0;

  *Visible = FALSE;

  DeltaX = *X1 - *X0;
  DeltaY = *Y1 - *Y0;

  if ((DeltaX == 0) && (DeltaY == 0))
  {
   	 *Visible = TRUE;
	 return;
  }

  TEnter = 0.0;
  TLeave = 1.0;

  OldTLeave = TLeave;
  OldTEnter = TEnter;

  if (ClipTParam(DeltaX, XMin - (*X0), &TEnter, &TLeave))		  // left edge
  {

 	 if(TEnter != OldTEnter)
		*P1Edge = 0;
     if(TLeave != OldTLeave)
		*P2Edge = 0;

	 OldTLeave = TLeave;
     OldTEnter = TEnter;


 	 if (ClipTParam(-DeltaX, (*X0) - XMax, &TEnter, &TLeave))	 // right edge
	 {

		if(TEnter != OldTEnter)
	    	*P1Edge = 2;
        if(TLeave != OldTLeave)
	     	*P2Edge = 2;

		OldTLeave = TLeave;
        OldTEnter = TEnter;

 	   if (ClipTParam(DeltaY, YMin - (*Y0), &TEnter, &TLeave))	 // top
	   {

		  if(TEnter != OldTEnter)
	       	*P1Edge = 1;
          if(TLeave != OldTLeave)
	     	*P2Edge = 1;

		  OldTLeave = TLeave;
          OldTEnter = TEnter;

	   	  if (ClipTParam(-DeltaY, (*Y0) - YMax, &TEnter, &TLeave)) // bottom
		  {

			 if(TEnter != OldTEnter)
	        	*P1Edge = 3;
             if(TLeave != OldTLeave)
	        	*P2Edge = 3;

		     OldTLeave = TLeave;
             OldTEnter = TEnter;


			 *Visible = TRUE;

	         if (TLeave < 1.0)
		     {
		    	*X1 = *X0 + TLeave * DeltaX;
			    *Y1 = *Y0 + TLeave * DeltaY;
		     }

		     if (TEnter > 0.0)
		     {
			   *X0 = *X0 + TEnter * DeltaX;
			   *Y0 = *Y0 + TEnter * DeltaY;
		     }

           }	// bottom
	   } // top
	 } // right edge
  } // left edge

} // proc



//void TestRectClip(int Left, int Right, int Top, int Bottom, int X0, int Y0, int X1, int Y1)
//{
//   int Visible;
//   int Index, PolyIndex;
//   int LeftIndex = -1;
//   int RightIndex = -1;
//   int RollDegree;
//   int DeltaX, DeltaY;
//   int DistanceX, DistanceY;
//   float Frac;
//   int DrawGround;

//   ClipLineToRect(&X0, &Y0, &X1, &Y1, Left, Right, Top, Bottom, &Visible,
//	              &LeftIndex, &RightIndex);

//   GrFillRectNoClip(GrBuffFor3D, Left, Top, (Right - Left), (Bottom - Top), MPD_SKY_COLOR);

//   GrFillRectNoClip(GrBuffFor3D, 112, 313, 56, 54, BACKUP_SKY_COLOR);

//   if(!Visible)	//if no intersection with the clip region, must fill in all sky or all ground
//   {

//  	 RollDegree = PlayerPlane->Roll/DEGREE;

//     DrawGround = FALSE;

//	 if(CentY < Top)
//     {
//	   if( ((RollDegree >= 270) && (RollDegree < 360)) || (RollDegree <= 90) )
//	   {
//	     DrawGround = TRUE;
//   	   }

//     }

//     if(CentY > Bottom)
//     {
//	   if( (RollDegree > 90) && (RollDegree <= 270) )
//	   {
//	     DrawGround = TRUE;
//	   }

//     }

//     if(CentX < Left)
//     {
//	   if( (RollDegree > 0) && (RollDegree < 180) )
//	   {
//	     DrawGround = TRUE;
//	   }


//     }

//     if(CentX > Right)
//     {
//	   if( (RollDegree > 180) && (RollDegree < 360) )
//	   {
//	     DrawGround = TRUE;
//       }


//     }

//     if(DrawGround)
//     {
//       GrFillRectNoClip(GrBuffFor3D, Left, Top, (Right - Left), (Bottom - Top), MPD_GROUND_COLOR);
//       GrFillRectNoClip(GrBuffFor3D, 112, 313, 56, 54, BACKUP_GROUND_COLOR);
//       return;
//     }

//	 return; // already drew sky

//   }

//   BoxVerts[0].iSX = Left;
//   BoxVerts[0].iSY = Top;
//   BoxVerts[1].iSX = Right;
//   BoxVerts[1].iSY = Top;
//   BoxVerts[2].iSX = Right;
//   BoxVerts[2].iSY = Bottom;
//   BoxVerts[3].iSX = Left;
//   BoxVerts[3].iSY = Bottom;

//   NewBoxVerts[0].iSX = 121;
//   NewBoxVerts[0].iSY = 322;
//   NewBoxVerts[1].iSX = 160;
//   NewBoxVerts[1].iSY = 322;
//   NewBoxVerts[2].iSX = 160;
//   NewBoxVerts[2].iSY = 352;
//   NewBoxVerts[3].iSX = 121;
//   NewBoxVerts[3].iSY = 352;


//   if( (LeftIndex == -1) || (RightIndex == -1) )
//   	   return;

//   PolyVerts[0].iSX = X1;	                 // first vert is the right vertex
//   PolyVerts[0].iSY = Y1;

//   Index = RightIndex;                   // go around the box clockwise
//   PolyIndex = 1;  // array index				 // starting at second element of the array
//   do
//   {
//	 PolyVerts[PolyIndex].iSX = BoxVerts[Index].iSX;  // fill in box verts
//     PolyVerts[PolyIndex].iSY = BoxVerts[Index].iSY;;

//	 Index++;
//	 if(Index == 4)		              // wrap around
//		Index = 0;

//	 PolyIndex++;
//   }  while(Index != LeftIndex);      // until we have reached the vert that the left vertex
//                                      // is pointing to

//   PolyVerts[PolyIndex].iSX = X0;     // set the left vert
//   PolyVerts[PolyIndex].iSY = Y0;


//   for (int j=0; j<PolyIndex+1; j++)
//   {
//	PolyVerts[j].Flags = PT3_ROTATED | PT3_PERSPECTED;
//	NewPolyVerts[j].Flags = PT3_ROTATED | PT3_PERSPECTED;
//	 pPolyVerts[j] = &(PolyVerts[j]);
//	 pNewPolyVerts[j] = &(NewPolyVerts[j]);

//   }





//  //* Draw the other Ball by using the first one as a reference

//   switch(RightIndex)
//   {
//     case 1 :
//     {

//       DeltaX = BoxVerts[1].iSX - BoxVerts[0].iSX;
//	   DistanceX = PolyVerts[0].iSX - BoxVerts[0].iSX;

//	   Frac = float(DistanceX)/float(DeltaX);

//	   NewPolyVerts[0].iSX = NewBoxVerts[0].iSX + Frac * (NewBoxVerts[1].iSX - NewBoxVerts[0].iSX);
//	   NewPolyVerts[0].iSY = NewBoxVerts[0].iSY;


//	   break;
//     }

//     case 2 :
//     {

//	   DeltaY = BoxVerts[2].iSY - BoxVerts[1].iSY;
//	   DistanceY = PolyVerts[0].iSY - BoxVerts[1].iSY;

//	   Frac = float(DistanceY)/float(DeltaY);

//	   NewPolyVerts[0].iSX = NewBoxVerts[1].iSX;
//	   NewPolyVerts[0].iSY = NewBoxVerts[0].iSY + Frac * (NewBoxVerts[2].iSY - NewBoxVerts[1].iSY);

//	   break;

//     }

//     case 3 :
//     {

//	   DeltaX = BoxVerts[2].iSX - BoxVerts[3].iSX;
//	   DistanceX = PolyVerts[0].iSX - BoxVerts[3].iSX;

//	   Frac = float(DistanceX)/float(DeltaX);

//	   NewPolyVerts[0].iSX = NewBoxVerts[3].iSX + Frac * (NewBoxVerts[2].iSX - NewBoxVerts[3].iSX);
//	   NewPolyVerts[0].iSY = NewBoxVerts[3].iSY;
//	    break;

//     }


//     case 0 :
//     {

//	   DeltaY = BoxVerts[3].iSY - BoxVerts[0].iSY;
//	   DistanceY =  PolyVerts[0].iSY  - BoxVerts[0].iSY;

//	   Frac = float(DistanceY)/float(DeltaY);

//	   NewPolyVerts[0].iSX = NewBoxVerts[0].iSX;
//	   NewPolyVerts[0].iSY = NewBoxVerts[0].iSY + Frac * (NewBoxVerts[3].iSY - NewBoxVerts[0].iSY);
//	   break;

//     }

//   }

//   switch(LeftIndex)
//   {
//     case 1 :
//     {

//       DeltaX = BoxVerts[1].iSX - BoxVerts[0].iSX;
//	   DistanceX = PolyVerts[PolyIndex].iSX - BoxVerts[0].iSX;

//	   Frac = float(DistanceX)/float(DeltaX);

//	   NewPolyVerts[PolyIndex].iSX = NewBoxVerts[0].iSX + Frac * (NewBoxVerts[1].iSX - NewBoxVerts[0].iSX);
//	   NewPolyVerts[PolyIndex].iSY = NewBoxVerts[0].iSY;
//	   break;
//     }

//     case 2 :
//     {

//	   DeltaY = BoxVerts[2].iSY - BoxVerts[1].iSY;
//	   DistanceY =  PolyVerts[PolyIndex].iSY - BoxVerts[1].iSY;

//	   Frac = float(DistanceY)/float(DeltaY);

//	   NewPolyVerts[PolyIndex].iSX = NewBoxVerts[1].iSX;
//	   NewPolyVerts[PolyIndex].iSY = NewBoxVerts[1].iSY + Frac * (NewBoxVerts[2].iSY - NewBoxVerts[1].iSY);
//	   break;

//     }

//     case 3 :
//     {

//	   DeltaX = BoxVerts[2].iSX - BoxVerts[3].iSX;
//	   DistanceX = PolyVerts[PolyIndex].iSX - BoxVerts[3].iSX;

//	   Frac = float(DistanceX)/float(DeltaX);

//	   NewPolyVerts[PolyIndex].iSX = NewBoxVerts[3].iSX + Frac * (NewBoxVerts[2].iSX - NewBoxVerts[3].iSX);
//	   NewPolyVerts[PolyIndex].iSY = NewBoxVerts[3].iSY;
//	    break;

//     }


//     case 0 :
//     {

//	   DeltaY = BoxVerts[3].iSY - BoxVerts[0].iSY;
//	   DistanceY =  PolyVerts[PolyIndex].iSY - BoxVerts[0].iSY;

//	   Frac = float(DistanceY)/float(DeltaY);

//	   NewPolyVerts[PolyIndex].iSX = NewBoxVerts[0].iSX;
//	   NewPolyVerts[PolyIndex].iSY = NewBoxVerts[0].iSY + Frac * (NewBoxVerts[3].iSY - NewBoxVerts[0].iSY);
//	   break;

//     }

//   }

//   Index = RightIndex;
//   PolyIndex = 1;

//   do
//   {
//	 NewPolyVerts[PolyIndex].iSX = NewBoxVerts[Index].iSX;  // fill in box verts
//     NewPolyVerts[PolyIndex].iSY = NewBoxVerts[Index].iSY;

//	 Index++;
//	 if(Index == 4)		              // wrap around
//		Index = 0;

//	 PolyIndex++;
//   }  while(Index != LeftIndex);      // until we have reached the vert that the left vertex


//   for (j=0; j<PolyIndex+1; j++)
//   {
//	  pNewPolyVerts[j] = &(NewPolyVerts[j]);
//   }

//// END Draw the other Ball by using the first one as a reference  END


//   DrawPolyNoClip(PRIM_DEFAULT, PolyIndex+1, pPolyVerts, MPD_GROUND_COLOR);
//   DrawPolyNoClip(PRIM_DEFAULT, PolyIndex+1, pNewPolyVerts, BACKUP_GROUND_COLOR);

//	if (_3dxl && (GrBuffFor3D == BackSurface))
//		_3dxlStopRenderingFunctions();
//	else
//		if (ThreeDOutputBuffer)
//		{
//			GrReleaseSurface(GrBuffFor3D);
//			ThreeDOutputBuffer = 0;
//		}

//  }



void CalculatePitchAndRollLine()
{
  signed short TmpPitch;
  int DeltaX, DeltaY;

  TmpPitch = (signed short)((signed short)PlayerPlane->Pitch/(10));

  if ((signed short)TmpPitch > (signed short)(6*DEGREE)) TmpPitch = (signed short)(6*DEGREE);
  else
  if ((signed short)TmpPitch < -(signed short)(6*DEGREE)) TmpPitch = -(signed short)(6*DEGREE);


  FMatrix TmpAttitude((ANGLE)PlayerPlane->Roll,(ANGLE)TmpPitch,(ANGLE)0);

  TmpAttitude.Transpose();

  CockpitP1.iSX = 0;
  CockpitP1.iSY = 0;

  CockpitP1.Rotated.RotateAndPerspect((int *)&CockpitP1.iSX,
	                                  (int *)&CockpitP1.iSY,
									  &TmpAttitude,
									  &CockpitRawP1);

  CockpitP2.Rotated.RotateAndPerspect((int *)&CockpitP2.iSX,
	                                  (int *)&CockpitP2.iSY,
									  &TmpAttitude,
									  &CockpitRawP2);

  CockpitP1.iSX -= 9;
  CockpitP1.iSY += 132;
  CockpitP2.iSX -= 9;
  CockpitP2.iSY += 132;

  DeltaX = CockpitP2.iSX - CockpitP1.iSX;
  DeltaY = CockpitP2.iSY - CockpitP1.iSY;
  CentX = CockpitP1.iSX + DeltaX/2;
  CentY = CockpitP1.iSY + DeltaY/2;

 }


void CalculateADI(ADIEntryType *ADIPtr)
{
   int DeltaX, DeltaY;
   int DistanceX, DistanceY;
   float Frac;
   int PolyIndex;
   int Index;


   ADIPtr->DrawFullGround = MasterADI.DrawFullGround;
   ADIPtr->DrawFullSky = MasterADI.DrawFullSky;

   switch(MasterADI.RightIndex)
   {
     case 1 :
     {
       DeltaX = MasterADI.BoxVerts[1].iSX - MasterADI.BoxVerts[0].iSX;
	   DistanceX = MasterADI.PolyVerts[0].iSX - MasterADI.BoxVerts[0].iSX;

	   Frac = float(DistanceX)/float(DeltaX);

	   ADIPtr->PolyVerts[0].iSX = ADIPtr->BoxVerts[0].iSX + Frac * (ADIPtr->BoxVerts[1].iSX - ADIPtr->BoxVerts[0].iSX);
	   ADIPtr->PolyVerts[0].iSY = ADIPtr->BoxVerts[0].iSY;
	   ADIPtr->PolyVerts[0].fSX = ADIPtr->PolyVerts[0].iSX;
	   ADIPtr->PolyVerts[0].fSY = ADIPtr->PolyVerts[0].iSY;
	   break;
     }

     case 2 :
     {
	   DeltaY = MasterADI.BoxVerts[2].iSY - MasterADI.BoxVerts[1].iSY;
	   DistanceY = MasterADI.PolyVerts[0].iSY - MasterADI.BoxVerts[1].iSY;

	   Frac = float(DistanceY)/float(DeltaY);

	   ADIPtr->PolyVerts[0].iSX = ADIPtr->BoxVerts[1].iSX;
	   ADIPtr->PolyVerts[0].iSY = ADIPtr->BoxVerts[0].iSY + Frac * (ADIPtr->BoxVerts[2].iSY - ADIPtr->BoxVerts[1].iSY);
	   ADIPtr->PolyVerts[0].fSX = ADIPtr->PolyVerts[0].iSX;
	   ADIPtr->PolyVerts[0].fSY = ADIPtr->PolyVerts[0].iSY;
	   break;
     }

     case 3 :
     {
	   DeltaX = MasterADI.BoxVerts[2].iSX - MasterADI.BoxVerts[3].iSX;
	   DistanceX = MasterADI.PolyVerts[0].iSX - MasterADI.BoxVerts[3].iSX;

	   Frac = float(DistanceX)/float(DeltaX);

	   ADIPtr->PolyVerts[0].iSX = ADIPtr->BoxVerts[3].iSX + Frac * (ADIPtr->BoxVerts[2].iSX - ADIPtr->BoxVerts[3].iSX);
	   ADIPtr->PolyVerts[0].iSY = ADIPtr->BoxVerts[3].iSY;
	   ADIPtr->PolyVerts[0].fSX = ADIPtr->PolyVerts[0].iSX;
	   ADIPtr->PolyVerts[0].fSY = ADIPtr->PolyVerts[0].iSY;
	   break;
     }

     case 0 :
     {

	   DeltaY = MasterADI.BoxVerts[3].iSY - MasterADI.BoxVerts[0].iSY;
	   DistanceY =  MasterADI.PolyVerts[0].iSY  - MasterADI.BoxVerts[0].iSY;

	   Frac = float(DistanceY)/float(DeltaY);

	   ADIPtr->PolyVerts[0].iSX = ADIPtr->BoxVerts[0].iSX;
	   ADIPtr->PolyVerts[0].iSY = ADIPtr->BoxVerts[0].iSY + Frac * (ADIPtr->BoxVerts[3].iSY - ADIPtr->BoxVerts[0].iSY);
	   ADIPtr->PolyVerts[0].fSX = ADIPtr->PolyVerts[0].iSX;
	   ADIPtr->PolyVerts[0].fSY = ADIPtr->PolyVerts[0].iSY;
	   break;
     }

   }

   PolyIndex = MasterADI.NumVerts - 1; // JLM debug

   switch(MasterADI.LeftIndex)
   {
     case 1 :
     {

       DeltaX = MasterADI.BoxVerts[1].iSX - MasterADI.BoxVerts[0].iSX;
	   DistanceX = MasterADI.PolyVerts[PolyIndex].iSX - MasterADI.BoxVerts[0].iSX;

	   Frac = float(DistanceX)/float(DeltaX);

	   ADIPtr->PolyVerts[PolyIndex].iSX = ADIPtr->BoxVerts[0].iSX + Frac * (ADIPtr->BoxVerts[1].iSX - ADIPtr->BoxVerts[0].iSX);
	   ADIPtr->PolyVerts[PolyIndex].iSY = ADIPtr->BoxVerts[0].iSY;
	   ADIPtr->PolyVerts[PolyIndex].fSX = ADIPtr->PolyVerts[PolyIndex].iSX;
	   ADIPtr->PolyVerts[PolyIndex].fSY = ADIPtr->PolyVerts[PolyIndex].iSY;
	   break;
     }

     case 2 :
     {

	   DeltaY = MasterADI.BoxVerts[2].iSY - MasterADI.BoxVerts[1].iSY;
	   DistanceY =  MasterADI.PolyVerts[PolyIndex].iSY - MasterADI.BoxVerts[1].iSY;

	   Frac = float(DistanceY)/float(DeltaY);

	   ADIPtr->PolyVerts[PolyIndex].iSX = ADIPtr->BoxVerts[1].iSX;
	   ADIPtr->PolyVerts[PolyIndex].iSY = ADIPtr->BoxVerts[1].iSY + Frac * (ADIPtr->BoxVerts[2].iSY - ADIPtr->BoxVerts[1].iSY);
	   ADIPtr->PolyVerts[PolyIndex].fSX = ADIPtr->PolyVerts[PolyIndex].iSX;
	   ADIPtr->PolyVerts[PolyIndex].fSY = ADIPtr->PolyVerts[PolyIndex].iSY;
	   break;
     }

     case 3 :
     {

	   DeltaX = MasterADI.BoxVerts[2].iSX - MasterADI.BoxVerts[3].iSX;
	   DistanceX = MasterADI.PolyVerts[PolyIndex].iSX - MasterADI.BoxVerts[3].iSX;

	   Frac = float(DistanceX)/float(DeltaX);

	   ADIPtr->PolyVerts[PolyIndex].iSX = ADIPtr->BoxVerts[3].iSX + Frac * (ADIPtr->BoxVerts[2].iSX - ADIPtr->BoxVerts[3].iSX);
	   ADIPtr->PolyVerts[PolyIndex].iSY = ADIPtr->BoxVerts[3].iSY;
	   ADIPtr->PolyVerts[PolyIndex].fSX = ADIPtr->PolyVerts[PolyIndex].iSX;
	   ADIPtr->PolyVerts[PolyIndex].fSY = ADIPtr->PolyVerts[PolyIndex].iSY;
	   break;
     }

     case 0 :
     {

	   DeltaY = MasterADI.BoxVerts[3].iSY - MasterADI.BoxVerts[0].iSY;
	   DistanceY =  MasterADI.PolyVerts[PolyIndex].iSY - MasterADI.BoxVerts[0].iSY;

	   Frac = float(DistanceY)/float(DeltaY);

	   ADIPtr->PolyVerts[PolyIndex].iSX = ADIPtr->BoxVerts[0].iSX;
	   ADIPtr->PolyVerts[PolyIndex].iSY = ADIPtr->BoxVerts[0].iSY + Frac * (ADIPtr->BoxVerts[3].iSY - ADIPtr->BoxVerts[0].iSY);
	   ADIPtr->PolyVerts[PolyIndex].fSX = ADIPtr->PolyVerts[PolyIndex].iSX;
	   ADIPtr->PolyVerts[PolyIndex].fSY = ADIPtr->PolyVerts[PolyIndex].iSY;
	   break;
     }

   }

   Index = MasterADI.RightIndex;
   PolyIndex = 1;

   do
   {
	 ADIPtr->PolyVerts[PolyIndex].iSX = ADIPtr->BoxVerts[Index].iSX;  // fill in box verts
     ADIPtr->PolyVerts[PolyIndex].iSY = ADIPtr->BoxVerts[Index].iSY;
	 ADIPtr->PolyVerts[PolyIndex].fSX = ADIPtr->PolyVerts[PolyIndex].iSX;
     ADIPtr->PolyVerts[PolyIndex].fSY = ADIPtr->PolyVerts[PolyIndex].iSY;

	 Index++;
	 if(Index == 4)		              // wrap around
		Index = 0;

	 PolyIndex++;
   }  while(Index != MasterADI.LeftIndex);      // until we have reached the vert that the left vertex

   ADIPtr->NumVerts = PolyIndex + 1;

   for (int j=0; j<PolyIndex+1; j++)
   {
	  ADIPtr->pPolyVerts[j] = &(ADIPtr->PolyVerts[j]);
   }

}


void CalculateMasterADI()
{
   int Visible;
   int Index, PolyIndex;
   int LeftIndex = -1;
   int RightIndex = -1;
   int RollDegree;
   int DrawGround;
   int Left,Right,Top,Bottom;
   int X0,X1,Y0,Y1;

   // First we must calculate the pitch and roll line so we can get intersection points with our
   // box. Note that the Clipping region must be set up prior to rotate and perspect
   // (in CalculatePitchandRollLine)
   PushClipRegion();
   SetClipRegion(0,639,0,479,240,320);
   Set3DScreenClip();
   CalculatePitchAndRollLine();
   PopClipRegion();
   Set3DScreenClip();

   // set X0,X1,Y0,Y1 to the global variables calculated in "CalculatePitchAndRollLine". These
   // values are global for performance reasons (the structures are to big to elaborate each
   // time this function is called
   X0 = CockpitP1.iSX;
   Y0 = CockpitP1.iSY;
   X1 = CockpitP2.iSX;
   Y1 = CockpitP2.iSY;


   // now set the borders of the box
   Left   = MasterADI.BoxVerts[0].iSX;
   Right  = MasterADI.BoxVerts[1].iSX;
   Top    = MasterADI.BoxVerts[0].iSY;
   Bottom = MasterADI.BoxVerts[3].iSY;

   MasterADI.DrawFullGround = FALSE;
   MasterADI.DrawFullSky    = FALSE;

   ClipLineToRect(&X0, &Y0, &X1, &Y1, Left, Right, Top, Bottom, &Visible,
	              &LeftIndex, &RightIndex);

   if(!Visible)	//if no intersection with the clip region, must fill in all sky or all ground
   {

  	 RollDegree = PlayerPlane->Roll/DEGREE;

     DrawGround = FALSE;

	 if(CentY < Top)
     {
	   if( ((RollDegree >= 270) && (RollDegree < 360)) || (RollDegree <= 90) )
	   {
	     DrawGround = TRUE;
   	   }

     }

     if(CentY > Bottom)
     {
	   if( (RollDegree > 90) && (RollDegree <= 270) )
	   {
	     DrawGround = TRUE;
	   }

     }

     if(CentX > Right)
     {
	   if( (RollDegree > 0) && (RollDegree < 180) )
	   {
	     DrawGround = TRUE;
	   }
     }

     if(CentX < Left)
     {
	   if( (RollDegree > 180) && (RollDegree < 360) )
	   {
	     DrawGround = TRUE;
       }
     }

     if(DrawGround)
       MasterADI.DrawFullGround = TRUE;
	 else
	   MasterADI.DrawFullSky = TRUE;

	 return; // no need to go further, no intersection existed

   }

   MasterADI.PolyVerts[0].iSX = X1;	                 // first vert is the right vertex
   MasterADI.PolyVerts[0].iSY = Y1;
   MasterADI.PolyVerts[0].fSX = X1;
   MasterADI.PolyVerts[0].fSY = Y1;

   MasterADI.RightIndex = RightIndex;

   Index = RightIndex;                   // go around the box clockwise
   PolyIndex = 1;  // array index				 // starting at second element of the array
   do
   {
 	 MasterADI.PolyVerts[PolyIndex].iSX = MasterADI.BoxVerts[Index].iSX;  // fill in box verts
     MasterADI.PolyVerts[PolyIndex].iSY = MasterADI.BoxVerts[Index].iSY;
 	 MasterADI.PolyVerts[PolyIndex].fSX = MasterADI.BoxVerts[Index].iSX;  // fill in box verts
     MasterADI.PolyVerts[PolyIndex].fSY = MasterADI.BoxVerts[Index].iSY;

	 Index++;
	 if(Index == 4)		              // wrap around
		Index = 0;

	 PolyIndex++;
   }  while(Index != LeftIndex);      // until we have reached the vert that the left vertex
                                      // is pointing to

   MasterADI.NumVerts  = PolyIndex + 1;

   MasterADI.LeftIndex =  LeftIndex;

   MasterADI.PolyVerts[PolyIndex].iSX = X0;     // set the left vert
   MasterADI.PolyVerts[PolyIndex].iSY = Y0;
   MasterADI.PolyVerts[PolyIndex].fSX = X0;     // set the left vert
   MasterADI.PolyVerts[PolyIndex].fSY = Y0;

   for (int j=0; j<PolyIndex+1; j++)
   {
	 MasterADI.PolyVerts[j].Flags = PT3_ROTATED | PT3_PERSPECTED;
	 MasterADI.pPolyVerts[j] = &(MasterADI.PolyVerts[j]);
   }

}





void DrawADI(ADIEntryType *ADIPtr, int GroundColor, int SkyColor)
{

//        GrFillRectNoClip(GrBuffFor3D, ADIPtr->BoxVerts[0].iSX,
//	  		        ADIPtr->BoxVerts[0].iSY, (ADIPtr->BoxVerts[1].iSX - ADIPtr->BoxVerts[0].iSX + 1),
//	   				(ADIPtr->BoxVerts[2].iSY - ADIPtr->BoxVerts[1].iSY), SkyColor);

//		if(ADIPtr->DrawFullGround)

//			GrFillRectNoClip(GrBuffFor3D, ADIPtr->BoxVerts[0].iSX,
//	 		         ADIPtr->BoxVerts[0].iSY, (ADIPtr->BoxVerts[1].iSX - ADIPtr->BoxVerts[0].iSX),
//	 				 (ADIPtr->BoxVerts[2].iSY - ADIPtr->BoxVerts[1].iSY), GroundColor);
//		else
//		{
//		   if(!ADIPtr->DrawFullSky)
//		   {

//			 PushClipRegion();
//             SetClipRegion(0,639,0,479,240,320);
//             Set3DScreenClip();

//			if (_3dxl && (GrBuffFor3D == BackSurface))
//				_3dxlStartRenderingFunctions();
//			else
//			 	if (!ThreeDOutputBuffer)
//             	{
//	            	GrGrabSurface(GrBuffFor3D);
//	            	ThreeDOutputBuffer = GrGetBuffPointer(GrBuffFor3D);
//             	}

//			 // fSx and fSy Must be set in all Polys going to 3Dfx
//			RotPoint3D *pRot3D = ADIPtr->PolyVerts;
//			for( int i=0; i<ADIPtr->NumVerts; i++)
//			{
//				pRot3D->fSX = pRot3D->iSX;
//				pRot3D->fSY = pRot3D->iSY;
//				pRot3D->Rotated.Z = 10.0f;
//				pRot3D->
//				pRot3D++;
//			}

//			DrawPolyNoClip(PRIM_DEFAULT, ADIPtr->NumVerts, ADIPtr->pPolyVerts, GroundColor);

//			if (_3dxl && (GrBuffFor3D == BackSurface))
//				_3dxlStopRenderingFunctions();
//			else
//             if (ThreeDOutputBuffer)
//             {
//	            GrReleaseSurface(GrBuffFor3D);
//		        ThreeDOutputBuffer = 0;
//             }

//			 PopClipRegion();
//             Set3DScreenClip();


//		   } // if

//	   } // else

}

//*****************************************************************************************************************************************
// F18 COCKPIT CODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


CPitType Cpit;
int JettButtons[7];
int FireExtUsed;

extern int HudDeclutter;
extern int HudAlt;

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitF18Cockpit()
{
	FireExtUsed = 0;

	// MUST INIT THE STATE EACH TIME
	CpitItems[0].State  = 0;
	CpitItems[1].State  = 0;
	CpitItems[2].State  = 0;
	CpitItems[3].State  = 0;
	CpitItems[4].State  = 0;
	CpitItems[5].State  = 0;
	CpitItems[6].State  = 0;
	CpitItems[7].State  = 0;
	CpitItems[8].State  = 1;
	CpitItems[9].State  = 0;
	CpitItems[10].State = 0;
	CpitItems[11].State = 0;
	CpitItems[12].State = 0;
	CpitItems[13].State = 0;
	CpitItems[14].State = 0;
	CpitItems[15].State = 0;
	CpitItems[16].State = 0;
	CpitItems[17].State = 2;
	CpitItems[18].State = 2;
	CpitItems[19].State = 0;
	CpitItems[20].State = 0;
	CpitItems[21].State = 0;
	CpitItems[22].State = 0;
	CpitItems[23].State = 0;
	CpitItems[24].State = 0;
	CpitItems[25].State = 0;
	CpitItems[26].State = 0;
	CpitItems[27].State = 2;
	CpitItems[28].State = 0;
	CpitItems[29].State = 0;

	CpitItems[30].State = 1;
	CpitItems[31].State = 2;
	CpitItems[32].State = 0;
	CpitItems[33].State = 0;

	CpitItems[34].State = 0;



	Cpit.MasterCautionOn = FALSE;

	ZeroMemory(Cpit.Damage,sizeof(int)*35);
	ZeroMemory(JettButtons,sizeof(int)*7);

	Cpit.DecoyDeployVar = FALSE;
	Cpit.DecoyOnVar = FALSE;
	Cpit.LaserVar = FALSE;
	Cpit.AIVar = FALSE;
	Cpit.SAMVar = FALSE;
	Cpit.AAAVar = FALSE;
	Cpit.LBleedVar = FALSE;
	Cpit.RBleedVar = FALSE;
	Cpit.StbyVar  = FALSE;
	Cpit.RecVar   = FALSE;
	Cpit.XmitVar  = FALSE;
	Cpit.AutoVar  = FALSE;
	Cpit.ShootVar = FALSE;
	Cpit.LockVar  = FALSE;
	Cpit.DispVar  = FALSE;

	Cpit.CpitItem.NavFlirSwitchState  = 2;
	Cpit.CpitItem.NavFlirDir = 0;

	Cpit.CpitItem.JettSwitchState  = AV_JET_OFF;
	Cpit.CpitItem.JettSwitchDir = RIGHT;
	Cpit.CpitItem.FireLeftPanelUp = FALSE;
	Cpit.CpitItem.FireRightPanelUp = FALSE;

	Cpit.SimJettOn = FALSE;

	Cpit.RightExtinguishOn = FALSE;
	Cpit.LeftExtinguishOn = FALSE;
	Cpit.CanSimFireRight = FALSE;
	Cpit.CanSimFireLeft = FALSE;
	Cpit.LeftFireReadyVar = FALSE;
	Cpit.RightFireReadyVar = FALSE;

	Cpit.NoseVar = FALSE;
	Cpit.LeftVar = FALSE;
	Cpit.RightVar = FALSE;
	Cpit.FlapsVar = FALSE;
	Cpit.HalfVar = FALSE;
	Cpit.FullVar = FALSE;
	Cpit.SpeedBrakeVar = FALSE;
	Cpit.BrakeVar= FALSE;
	Cpit.APVar = FALSE;
	Cpit.LBar = FALSE;

	Cpit.HSelClicked = FALSE;
	Cpit.CSelClicked = FALSE;
	Cpit.HSelVar = FALSE;
	Cpit.CSelVar = FALSE;

	Cpit.CpitItem.HudDeclutterDir = 0;
	HudDeclutter = 0;
	HudAlt = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetMasterModeLights(int Mode)
{
	if(Mode == NAV_MODE)
	{
		CpitItems[10].State = 0;
	  CpitItems[11].State = 0;
	}
	else if(Mode == AA_MODE)
	{
	  CpitItems[10].State = 1;
	  CpitItems[11].State = 0;
	}
	else
	{
    CpitItems[10].State = 0;
		CpitItems[11].State = 1;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AAClick(int *State)
{
	if(*State == 1)
	{
		UFC.MasterMode = NAV_MODE;
		UFCSetNAVMode();
		*State = 0;
	}
	else
	{
		UFC.MasterMode = AA_MODE;
		UFCSetAAMode();
		*State = 1;
		CpitItems[11].State = 0;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGClick(int *State)
{
	if(*State == 1)
	{
		UFC.MasterMode = NAV_MODE;
		UFCSetNAVMode();
		*State = 0;
	}
	else
	{
		UFC.MasterMode = AG_MODE;
		UFCSetAGMode();
		*State = 1;
		CpitItems[10].State = 0;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ArmSafeClick(int *State)
{
	*State = !(*State);

	UFC.MasterArmState = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void LandingGearClick(int *State)
{
	


	ToggleGear();
	//if( PlayerPlane->LandingGearStatus == PL_LG_REQUEST_OPEN )
	//	*State = 1;
	//else
	//if( PlayerPlane->LandingGearStatus == PL_LG_REQUEST_CLOSED)
	//	*State = 0;

	//*State = !(*State);
	//RaiseLowerLandingGear(PlayerPlane,(*State) ? LOWER_LANDING_GEAR : RAISE_LANDING_GEAR);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void ToggleHook();

void HookClick(int *State)
{
	if( (*State == 0) && (PlayerPlane->TailHookState == 0) )
	{
		ToggleHook();

		if(PlayerPlane->TailHookState == 1)
	    *State = 1;
	}
	else if( (*State == 1) && (PlayerPlane->TailHookState == 1) )
	{
		ToggleHook();

		if(PlayerPlane->TailHookState == 0)
	    *State = 0;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void NavFlirClick(int *State)
{
	if(Cpit.CpitItem.NavFlirDir == 0)
	{
	  Cpit.CpitItem.NavFlirSwitchState--;
   	if(Cpit.CpitItem.NavFlirSwitchState < 0)
		{
			Cpit.CpitItem.NavFlirSwitchState = 1;
		  	Cpit.CpitItem.NavFlirDir = 1;
		}
	}
	else
	{
	  Cpit.CpitItem.NavFlirSwitchState++;
   	if(Cpit.CpitItem.NavFlirSwitchState > 2)
		{
		  	Cpit.CpitItem.NavFlirSwitchState = 1;
			Cpit.CpitItem.NavFlirDir = 0;
		}
	}

	*State = Cpit.CpitItem.NavFlirSwitchState;

	if(Av.Weapons.HasNavFlir)
	{
		switch(*State)
		{
			case 0: UFC.NavFlirStatus = 1; break;
			case 1: UFC.NavFlirStatus = 2; break;
			case 2: UFC.NavFlirStatus = 0; break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

extern AvRGB *HUDColorRGB;
extern AvRGB HUDColorList[10];

void HudColorClick(int *State)
{
	*State = (*State) + 1;
	if(*State > 10) *State = 0;

  HUDColorRGB++;
  if(HUDColorRGB - &HUDColorList[0] > 10)
	  HUDColorRGB = &HUDColorList[0];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void BingoUpClick(int *State)
{
  int Total = 14075.0;  // PlayerPlane->InternalFuel + PlayerPlane->CenterDropFuel + PlayerPlane->WingDropFuel;
	Av.Fuel.BingoVal += 500;
	if(Av.Fuel.BingoVal > Total)
	  Av.Fuel.BingoVal = Total;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void BingoDownClick(int *State)
{
	Av.Fuel.BingoVal -= 500;
	if(Av.Fuel.BingoVal < 0)
	  Av.Fuel.BingoVal = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HSelUpClick(int *State)
{
	Av.Hsi.HSelect += 10;
	if(Av.Hsi.HSelect >= 360)
		Av.Hsi.HSelect = 0;

	Cpit.HSelClicked = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HSelDownClick(int *State)
{
	Av.Hsi.HSelect -= 10;
	if(Av.Hsi.HSelect < 0)
		Av.Hsi.HSelect = 350;

	Cpit.HSelClicked = 2;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CSelUpClick(int *State)
{
	Av.Hsi.CSelect += 10;
	if(Av.Hsi.CSelect >= 360)
		Av.Hsi.CSelect = 0;

	Cpit.CSelClicked = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CSelDownClick(int *State)
{
	Av.Hsi.CSelect -= 10;
	if(Av.Hsi.CSelect < 0)
		Av.Hsi.CSelect = 350;

	Cpit.CSelClicked = 2;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MasterCautionClick(int *State)
{
	if(*State == 2) return;

	*State = 2;

	if(Cpit.MasterCautionOn)
	{
	  Cpit.MasterCautionOn = FALSE;
		if( !Cpit.MasterCautionShowBitTimer.IsActive() )
			Cpit.MasterCautionShowBitTimer.Set(3.0,GameLoopInTicks);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AltClick(int *State)
{
	*State = !(*State);
	HudAlt = *State;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SymRejectClick(int *State)
{
	if(Cpit.CpitItem.HudDeclutterDir == 0)
	{
		if(*State <= 1)
			*State = *State+1;
		else
		{
			*State = 1;
			Cpit.CpitItem.HudDeclutterDir = 1;
		}
	}
	else
	{
		if(*State >= 1)
			*State = *State-1;
		else
		{
			*State = 1;
			Cpit.CpitItem.HudDeclutterDir = 0;
		}
	}

	HudDeclutter = *State;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void EmisClick(int *State)
{
	*State = !(*State);
	UFC.EMISState = *State;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CollLightClick(int *State)
{

}
//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FormationLightClick(int *State)
{

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RefuelClick(int *State)
{

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlapsClick(int *State)
{

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RemoveDamageFromBit(int Damage,int MessageType)
{
	int copydown;
	BitType *Ptr = &BitList[0];

	int i=0;
	while(i < AV_MAX_DAMAGE)
	{
		 if(Ptr->Index == Damage)
		 {
			 Ptr->MessageIndex = MessageType;
		 }
		 i++;
		 Ptr++;
	}

	int j = 0;
	copydown = 0;

	while(j < NumDamagedBit)
	{
		if(BitDamageList[j]->Index == Damage)
		{
			copydown ++;
		}
		else if(copydown)
		{
			BitDamageList[j-copydown] = BitDamageList[j];
		}
		j++;
	}
	if(copydown)
		NumDamagedBit -= copydown;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

class Timer DebugTimer;

void CheckDamageForMasterCaution()
{
	int CautionOn = FALSE;

	/*

	// debug,,, randomly cause damage
	if(!DebugTimer.IsActive())
		DebugTimer.Set(10.0,GameLoopInTicks);

	if(DebugTimer.TimeUp(GameLoopInTicks) )
	{
		int Dam = rand() % 32;

		DestroyPlaneItem(PlayerPlane,Dam,TRUE);
	  DebugTimer.Set(10.0,GameLoopInTicks);
	}
	 *
	 * */


	if(!Cpit.MasterCautionTimer.IsActive())
	{
		Cpit.MasterCautionTimer.Set(0.5,GameLoopInTicks);
	}
	else
	{
		if(!Cpit.MasterCautionTimer.TimeUp(GameLoopInTicks)) return;
		else Cpit.MasterCautionTimer.Set(0.5,GameLoopInTicks);
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_L_BLEED_AIR)
	{
		if(!Cpit.Damage[AV_L_BLEED_AIR])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_L_BLEED_AIR] = TRUE;
			AddDamageToBit(AV_L_BLEED_AIR,AV_BIT_NO_GO);
		}
	}
	else 
	{
	  	if(Cpit.Damage[AV_L_BLEED_AIR])
		{
			Cpit.Damage[AV_L_BLEED_AIR] = FALSE;
			RemoveDamageFromBit(AV_L_BLEED_AIR,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_L_ENG_CONT)
	{
		if(!Cpit.Damage[AV_L_ENG_CONT])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_L_ENG_CONT] = TRUE;
		    AddDamageToBit(AV_L_ENG_CONT,AV_BIT_NO_GO);
		}
	}
	else 
	{
		if(Cpit.Damage[AV_L_ENG_CONT])
		{
			Cpit.Damage[AV_L_ENG_CONT] = FALSE;
		    RemoveDamageFromBit(AV_L_ENG_CONT,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_L_OIL_PRESS)
	{
		if(!Cpit.Damage[AV_L_OIL_PRESS])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_L_OIL_PRESS] = TRUE;
			AddDamageToBit(AV_L_OIL_PRESS,AV_BIT_NO_GO);
		}
	}
	else 
	{
		if(Cpit.Damage[AV_L_OIL_PRESS])
		{
			Cpit.Damage[AV_L_OIL_PRESS] = FALSE;
			RemoveDamageFromBit(AV_L_OIL_PRESS,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_L_BURNER)
	{
		if(!Cpit.Damage[AV_L_BURNER])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_L_BURNER] = TRUE;
			AddDamageToBit(AV_L_BURNER,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_L_BURNER])
		{
			Cpit.Damage[AV_L_BURNER] = FALSE;
			RemoveDamageFromBit(AV_L_BURNER,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_L_FUEL_PUMP)
	{
		if(!Cpit.Damage[AV_L_FUEL_PUMP])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_L_FUEL_PUMP] = TRUE;
			AddDamageToBit(AV_L_FUEL_PUMP,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_L_FUEL_PUMP])
		{
			Cpit.Damage[AV_L_FUEL_PUMP] = FALSE;
			RemoveDamageFromBit(AV_L_FUEL_PUMP,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_L_GEN)
	{
		if(!Cpit.Damage[AV_L_GEN])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_L_GEN] = TRUE;
			AddDamageToBit(AV_L_GEN,AV_BIT_NO_GO);
			if((PlayerPlane->SystemInactive & (DAMAGE_BIT_L_GEN|DAMAGE_BIT_R_GEN)) == (DAMAGE_BIT_L_GEN|DAMAGE_BIT_R_GEN))
			{
				PlayerPlane->SystemInactive |= DAMAGE_BIT_RADAR;
			}
		}
	}
	else
	{
		if(Cpit.Damage[AV_L_GEN])
		{
			Cpit.Damage[AV_L_GEN] = FALSE;
			RemoveDamageFromBit(AV_L_GEN,AV_BIT_GO);
			if(!(PlayerPlane->DamageFlags & DAMAGE_BIT_RADAR))
			{
				PlayerPlane->SystemInactive &= ~(DAMAGE_BIT_RADAR);
			}
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_LO_ENGINE)
	{
		if(!Cpit.Damage[AV_NO_HYD1B])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_NO_HYD1B] = TRUE;
		    AddDamageToBit(AV_NO_HYD1B,AV_BIT_NO_GO);
		}
		if(!Cpit.Damage[AV_L_MAIN_HYD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_L_MAIN_HYD] = TRUE;
		    AddDamageToBit(AV_L_MAIN_HYD,AV_BIT_NO_GO);
		}
	}
	else if((PlayerPlane->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) || (PlayerPlane->DamageFlags & DAMAGE_BIT_L_MAIN_HYD))
	{
//		int Left = (rand() % 10 < 5);
		int Left = (rand() & 0x1);

		if(((Left) && (!Cpit.Damage[AV_L_MAIN_HYD])) || (Cpit.Damage[AV_NO_HYD1B]))
		{
				if(!Cpit.Damage[AV_L_MAIN_HYD])
				{
		 			CautionOn = TRUE;
					Cpit.Damage[AV_L_MAIN_HYD] = TRUE;
					if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_MAIN_HYD)
					{
						Cpit.Damage[AV_L_MAIN_HYD] = 2;
					}

		      		AddDamageToBit(AV_L_MAIN_HYD,AV_BIT_NO_GO);
					CheckHydStuff();
				}

				if(!Cpit.Damage[AV_NO_HYD1B])
				{
					PlayerPlane->SystemInactive &= ~DAMAGE_BIT_L_MAIN_HYD;
					PlayerPlane->DamageFlags &= ~DAMAGE_BIT_L_MAIN_HYD;
				}
		}
		else
		{
				if(!Cpit.Damage[AV_NO_HYD1B])
				{
		 			CautionOn = TRUE;
					Cpit.Damage[AV_NO_HYD1B] = TRUE;
					if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_MAIN_HYD)
					{
						Cpit.Damage[AV_NO_HYD1B] = 2;
					}
		      		AddDamageToBit(AV_NO_HYD1B,AV_BIT_NO_GO);
					CheckHydStuff();
				}
				if(!Cpit.Damage[AV_L_MAIN_HYD])
				{
					PlayerPlane->SystemInactive &= ~DAMAGE_BIT_L_MAIN_HYD;
					PlayerPlane->DamageFlags &= ~DAMAGE_BIT_L_MAIN_HYD;
				}
		}
	}
	else if((!(PlayerPlane->DamageFlags & DAMAGE_BIT_L_MAIN_HYD)) && (!(PlayerPlane->DamageFlags & DAMAGE_BIT_LO_ENGINE)))
	{
//		int Left = (rand() % 10 < 5);

		if(Cpit.Damage[AV_L_MAIN_HYD] == TRUE)
		{
			Cpit.Damage[AV_L_MAIN_HYD] = FALSE;
      		RemoveDamageFromBit(AV_L_MAIN_HYD,AV_BIT_GO);
		}

		if(Cpit.Damage[AV_NO_HYD1B] == TRUE)
		{
			Cpit.Damage[AV_NO_HYD1B] = FALSE;
      		RemoveDamageFromBit(AV_NO_HYD1B,AV_BIT_GO);
		}
	}


	if(PlayerPlane->SystemInactive & DAMAGE_BIT_FLT_CONT)
	{
		if(!Cpit.Damage[AV_FLT_CONT])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_FLT_CONT] = TRUE;
			AddDamageToBit(AV_FLT_CONT,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_FLT_CONT])
		{
			Cpit.Damage[AV_FLT_CONT] = FALSE;
			RemoveDamageFromBit(AV_FLT_CONT,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_TARGET_IR)
	{
		if(Av.Weapons.HasTargetIR)
		{
				if(!Cpit.Damage[AV_TARGET_IR])
				{
		 			CautionOn = TRUE;
					Cpit.Damage[AV_TARGET_IR] = TRUE;
					AddDamageToBit(AV_TARGET_IR,AV_BIT_NO_GO);
				}
		}
	}
	else
	{
		if(Av.Weapons.HasTargetIR)
		{
				if(Cpit.Damage[AV_TARGET_IR])
				{
					Cpit.Damage[AV_TARGET_IR] = FALSE;
      				RemoveDamageFromBit(AV_TARGET_IR,AV_BIT_GO);
				}
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_PACS)
	{
		if(!Cpit.Damage[AV_PACS])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_PACS] = TRUE;
			AddDamageToBit(AV_PACS,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_PACS])
		{
			Cpit.Damage[AV_PACS] = FALSE;
			RemoveDamageFromBit(AV_PACS,AV_BIT_GO);
		}
	}

	if((PlayerPlane->SystemInactive & DAMAGE_BIT_CEN_COMP) || (PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP))
	{
//		int Left = (rand() % 10 < 5);
		int Left = (rand() & 0x1);

		if(((Left) && (!Cpit.Damage[AV_CEN_COMP])) || (Cpit.Damage[AV_MC2]))
		{
				if(!Cpit.Damage[AV_CEN_COMP])
				{
		 			CautionOn = TRUE;
					Cpit.Damage[AV_CEN_COMP] = TRUE;

					if(PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP)
					{
						Cpit.Damage[AV_CEN_COMP] = 2;
					}

      				AddDamageToBit(AV_CEN_COMP,AV_BIT_NO_GO);
					if(rand() & 0x1)
					{
						Cpit.Damage[AV_NO_R_MDI] = TRUE;
	      				AddDamageToBit(AV_NO_R_MDI,AV_BIT_NO_GO);
					}
				}
				if(!Cpit.Damage[AV_MC2])
				{
					PlayerPlane->SystemInactive &= ~DAMAGE_BIT_CEN_COMP;
					PlayerPlane->DamageFlags &= ~DAMAGE_BIT_CEN_COMP;
				}
		}
		else
		{
				if(!Cpit.Damage[AV_MC2])
				{
		 			CautionOn = TRUE;
					Cpit.Damage[AV_MC2] = TRUE;
					if(PlayerPlane->DamageFlags & DAMAGE_BIT_CEN_COMP)
					{
						Cpit.Damage[AV_MC2] = 2;
					}
					AddDamageToBit(AV_MC2,AV_BIT_NO_GO);
				}
				if(!Cpit.Damage[AV_CEN_COMP])
				{
					PlayerPlane->SystemInactive &= ~DAMAGE_BIT_CEN_COMP;
					PlayerPlane->DamageFlags &= ~DAMAGE_BIT_CEN_COMP;
				}
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_RADAR)
	{
		if(!Cpit.Damage[AV_RADAR])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_RADAR] = TRUE;
			AddDamageToBit(AV_RADAR,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_RADAR])
		{
			Cpit.Damage[AV_RADAR] = FALSE;
			RemoveDamageFromBit(AV_RADAR,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_ADC)
	{
		if(!Cpit.Damage[AV_ADC])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_ADC] = TRUE;
			AddDamageToBit(AV_ADC,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_ADC])
		{
			Cpit.Damage[AV_ADC] = FALSE;
			RemoveDamageFromBit(AV_ADC,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_OXYGEN)
	{
		if(!Cpit.Damage[AV_OXYGEN])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_OXYGEN] = TRUE;
			AddDamageToBit(AV_OXYGEN,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_OXYGEN])
		{
			Cpit.Damage[AV_OXYGEN] = FALSE;
			RemoveDamageFromBit(AV_OXYGEN,AV_BIT_GO);
		}
	}


	/*
	if(PlayerPlane->SystemInactive & DAMAGE_BIT_LO_ENGINE)
	{
	 if(!Cpit.Damage[AV_LO_ENGINE])
	 {
		 CautionOn = TRUE;
		 Cpit.Damage[AV_LO_ENGINE] = TRUE;
     AddDamageToBit(AV_LO_ENGINE,AV_BIT_NO_GO);
	 }
	}
	*/

 	if(PlayerPlane->SystemInactive & DAMAGE_BIT_LO_ENGINE)
	{
		if(!Cpit.Damage[AV_L_ENGINE])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_L_ENGINE] = TRUE;
			AddDamageToBit(AV_L_ENGINE,AV_BIT_NO_GO);
		}
		if(!Cpit.Damage[AV_NO_L_AMAD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_NO_L_AMAD] = TRUE;
			if(BitList[AV_NO_L_AMAD].MessageIndex == AV_BIT_GO)
			{
				AddDamageToBit(AV_NO_L_AMAD,AV_BIT_NO_GO);
			}
			else
			{
				SetBitMessage(AV_NO_L_AMAD,AV_BIT_NO_GO);
			}
		}
	}
 	else
	{
		if(Cpit.Damage[AV_L_ENGINE])
		{
			Cpit.Damage[AV_L_ENGINE] = FALSE;
			RemoveDamageFromBit(AV_L_ENGINE,AV_BIT_GO);
		}
		if(Cpit.Damage[AV_NO_L_AMAD])
		{
			Cpit.Damage[AV_NO_L_AMAD] = FALSE;
			RemoveDamageFromBit(AV_NO_L_AMAD,AV_BIT_GO);
		}
	}


	if(lFireFlags & HEAT_UP_AMAD_LEFT)
	{
		if(BitList[AV_NO_L_AMAD].MessageIndex == AV_BIT_GO)
		{
			CautionOn = TRUE;
			AddDamageToBit(AV_NO_L_AMAD,AV_BIT_DEGD);
		}
	}


	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_BLEED_AIR)
	{
		if(!Cpit.Damage[AV_R_BLEED_AIR])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_R_BLEED_AIR] = TRUE;
			AddDamageToBit(AV_R_BLEED_AIR,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_BLEED_AIR])
		{
			Cpit.Damage[AV_R_BLEED_AIR] = FALSE;
			RemoveDamageFromBit(AV_R_BLEED_AIR,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_ENG_CONT)
	{
		if(!Cpit.Damage[AV_R_ENG_CONT])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_ENG_CONT] = TRUE;
			AddDamageToBit(AV_R_ENG_CONT,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_ENG_CONT])
		{
			Cpit.Damage[AV_R_ENG_CONT] = FALSE;
			RemoveDamageFromBit(AV_R_ENG_CONT,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_OIL_PRESS)
	{
		if(!Cpit.Damage[AV_R_OIL_PRESS])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_OIL_PRESS] = TRUE;
			AddDamageToBit(AV_R_OIL_PRESS,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_OIL_PRESS])
		{
			Cpit.Damage[AV_R_OIL_PRESS] = FALSE;
			RemoveDamageFromBit(AV_R_OIL_PRESS,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_BURNER)
	{
		if(!Cpit.Damage[AV_R_BURNER])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_BURNER] = TRUE;
			AddDamageToBit(AV_R_BURNER,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_BURNER])
		{
			Cpit.Damage[AV_R_BURNER] = FALSE;
			RemoveDamageFromBit(AV_R_BURNER,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_FUEL_PUMP)
	{
		if(!Cpit.Damage[AV_R_FUEL_PUMP])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_FUEL_PUMP] = TRUE;
			AddDamageToBit(AV_R_FUEL_PUMP,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_FUEL_PUMP])
		{
			Cpit.Damage[AV_R_FUEL_PUMP] = FALSE;
			RemoveDamageFromBit(AV_R_FUEL_PUMP,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_GEN)
	{
		if(!Cpit.Damage[AV_R_GEN])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_GEN] = TRUE;
			AddDamageToBit(AV_R_GEN,AV_BIT_NO_GO);
			if((PlayerPlane->SystemInactive & (DAMAGE_BIT_L_GEN|DAMAGE_BIT_R_GEN)) == (DAMAGE_BIT_L_GEN|DAMAGE_BIT_R_GEN))
			{
				PlayerPlane->SystemInactive |= DAMAGE_BIT_RADAR;
			}
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_GEN])
		{
			Cpit.Damage[AV_R_GEN] = FALSE;
			RemoveDamageFromBit(AV_R_GEN,AV_BIT_GO);
			if(!(PlayerPlane->DamageFlags & DAMAGE_BIT_RADAR))
			{
				PlayerPlane->SystemInactive &= ~(DAMAGE_BIT_RADAR);
			}
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_EMER_HYD)
	{
		if(!Cpit.Damage[AV_EMER_HYD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_EMER_HYD] = TRUE;
			AddDamageToBit(AV_EMER_HYD,AV_BIT_NO_GO);
			CheckHydStuff();
		}
	}
	else if(PlayerPlane->SystemInactive & DAMAGE_BIT_RO_ENGINE)
	{
		if(!Cpit.Damage[AV_EMER_HYD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_EMER_HYD] = TRUE;
			AddDamageToBit(AV_EMER_HYD,AV_BIT_NO_GO);
			CheckHydStuff();
		}
	}
 	else
	{
		if(Cpit.Damage[AV_EMER_HYD])
		{
			Cpit.Damage[AV_EMER_HYD] = FALSE;
			RemoveDamageFromBit(AV_EMER_HYD,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_AUTOPILOT)
	{
		if(!Cpit.Damage[AV_AUTOPILOT])
		{
			if(UFC.APStatus)
			{
				AutoPilotOnOff();
			}
		 	CautionOn = TRUE;
			Cpit.Damage[AV_AUTOPILOT] = TRUE;
			AddDamageToBit(AV_AUTOPILOT,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_AUTOPILOT])
		{
			Cpit.Damage[AV_AUTOPILOT] = FALSE;
			RemoveDamageFromBit(AV_AUTOPILOT,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_NAV_FLIR)
	{
		if(Av.Weapons.HasNavFlir)
		{
			if(!Cpit.Damage[AV_NAV_FLIR])
			{
		 		CautionOn = TRUE;
		 		Cpit.Damage[AV_NAV_FLIR] = TRUE;
     			AddDamageToBit(AV_NAV_FLIR,AV_BIT_NO_GO);
	 		}
		}
	}
	else
	{
		if(Av.Weapons.HasNavFlir)
		{
			if(Cpit.Damage[AV_NAV_FLIR])
			{
		 		Cpit.Damage[AV_NAV_FLIR] = FALSE;
     			RemoveDamageFromBit(AV_NAV_FLIR,AV_BIT_GO);
	 		}
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_GUN)
	{
		if(!Cpit.Damage[AV_GUN])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_GUN] = TRUE;
			AddDamageToBit(AV_GUN,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_GUN])
		{
			Cpit.Damage[AV_GUN] = FALSE;
			RemoveDamageFromBit(AV_GUN,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_TEWS)
	{
		if(!Cpit.Damage[AV_TEWS])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_TEWS] = TRUE;
			AddDamageToBit(AV_TEWS,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_TEWS])
		{
			Cpit.Damage[AV_TEWS] = FALSE;
			RemoveDamageFromBit(AV_TEWS,AV_BIT_GO);
		}
	}

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_HUD)
	{
		if(!Cpit.Damage[AV_HUD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_HUD] = TRUE;
			AddDamageToBit(AV_HUD,AV_BIT_NO_GO);
		}
	}
	else
	{
		if(Cpit.Damage[AV_HUD])
		{
			Cpit.Damage[AV_HUD] = FALSE;
			RemoveDamageFromBit(AV_HUD,AV_BIT_GO);
		}
	}

	/*
	if(PlayerPlane->SystemInactive & DAMAGE_BIT_FUEL_TANKS)
	{
		if(!Cpit.Damage[AV_FUEL_TANKS])
		{
		 	CautionOn = TRUE;
		  Cpit.Damage[AV_FUEL_TANKS] = TRUE;
      AddDamageToBit(AV_FUEL_TANKS,AV_BIT_NO_GO);
		}
	}

  */

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_R_MAIN_HYD)
	{
		if(!Cpit.Damage[AV_R_MAIN_HYD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_MAIN_HYD] = TRUE;
			AddDamageToBit(AV_R_MAIN_HYD,AV_BIT_NO_GO);
			CheckHydStuff();
		}
	}
	else if(PlayerPlane->SystemInactive & DAMAGE_BIT_RO_ENGINE)
	{
		if(!Cpit.Damage[AV_R_MAIN_HYD])
		{
		 	CautionOn = TRUE;
			Cpit.Damage[AV_R_MAIN_HYD] = TRUE;
			AddDamageToBit(AV_R_MAIN_HYD,AV_BIT_NO_GO);
			CheckHydStuff();
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_MAIN_HYD])
		{
			Cpit.Damage[AV_R_MAIN_HYD] = FALSE;
			RemoveDamageFromBit(AV_R_MAIN_HYD,AV_BIT_GO);
		}
	}

  /*
	if(PlayerPlane->SystemInactive & DAMAGE_BIT_RO_ENGINE)
	{
		if(!Cpit.Damage[AV_RO_ENGINE])
		{
		 CautionOn = TRUE;
		 Cpit.Damage[AV_RO_ENGINE] = TRUE;
     AddDamageToBit(AV_RO_ENGINE,AV_BIT_NO_GO);
		}
	}

  */

	if(PlayerPlane->SystemInactive & DAMAGE_BIT_RO_ENGINE)
	{
		if(!Cpit.Damage[AV_R_ENGINE])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_R_ENGINE] = TRUE;
			AddDamageToBit(AV_R_ENGINE,AV_BIT_NO_GO);
		}
		if(!Cpit.Damage[AV_NO_R_AMAD])
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_NO_R_AMAD] = TRUE;
			if(BitList[AV_NO_R_AMAD].MessageIndex == AV_BIT_GO)
			{
				AddDamageToBit(AV_NO_R_AMAD,AV_BIT_NO_GO);
			}
			else
			{
				SetBitMessage(AV_NO_R_AMAD,AV_BIT_NO_GO);
			}
		}
	}
	else
	{
		if(Cpit.Damage[AV_R_ENGINE])
		{
			Cpit.Damage[AV_R_ENGINE] = FALSE;
			RemoveDamageFromBit(AV_R_ENGINE,AV_BIT_GO);
		}
		if(Cpit.Damage[AV_NO_R_AMAD])
		{
			Cpit.Damage[AV_NO_R_AMAD] = FALSE;
			RemoveDamageFromBit(AV_NO_R_AMAD,AV_BIT_GO);
		}
	}

	if(lFireFlags & HEAT_UP_AMAD_RIGHT)
	{
		if(BitList[AV_NO_R_AMAD].MessageIndex == AV_BIT_GO)
		{
			CautionOn = TRUE;
			AddDamageToBit(AV_NO_R_AMAD,AV_BIT_DEGD);
		}
	}

	if(!Cpit.Damage[AV_NO_CHAFF])
	{
			if(ChaffTotal <= 0)
			{
				CautionOn = TRUE;
				Cpit.Damage[AV_NO_CHAFF] = TRUE;
				AddDamageToBit(AV_NO_CHAFF,AV_BIT_EMPTY);
			}
	}
	else
	{
			if(ChaffTotal > 0)
			{
				Cpit.Damage[AV_NO_CHAFF] = FALSE;
				RemoveDamageFromBit(AV_NO_CHAFF,AV_BIT_GO);
			}
	}


	if(!Cpit.Damage[AV_NO_FLARE])
	{
			if(FlareTotal <= 0)
			{
				CautionOn = TRUE;
				Cpit.Damage[AV_NO_FLARE] = TRUE;
				AddDamageToBit(AV_NO_FLARE,AV_BIT_EMPTY);
			}
	}
	else
	{
			if(FlareTotal > 0)
			{
				Cpit.Damage[AV_NO_FLARE] = FALSE;
				RemoveDamageFromBit(AV_NO_FLARE,AV_BIT_GO);
			}
	}

	if(!Cpit.Damage[AV_NO_DECOY])
	{
			if(Av.Tews.CM.CurCMDDS == -1)
			{
				CautionOn = TRUE;
				Cpit.Damage[AV_NO_DECOY] = TRUE;
				AddDamageToBit(AV_NO_DECOY,AV_BIT_EMPTY);
			}
	}
	else 
	{
			if(Av.Tews.CM.CurCMDDS > -1)
			{
				Cpit.Damage[AV_NO_DECOY] = FALSE;
				RemoveDamageFromBit(AV_NO_DECOY,AV_BIT_GO);
			}
	}

	if(!Cpit.Damage[AV_G_LIMIT])
	{
		if (GetVkStatus(DisableGLimit))
		{
			CautionOn = TRUE;
			Cpit.Damage[AV_G_LIMIT] = TRUE;
			AddDamageToBit(AV_G_LIMIT,AV_BIT_OVRD);
		}
	}

	if(!Cpit.MasterCautionOn && CautionOn)
	{
	  Cpit.MasterCautionOn = CautionOn;
	  SetCockpitItemState(227,1);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MasterCautionKeypress()
{
	Cpit.MasterCautionOn = FALSE;
	SetCockpitItemState(227,2);
	Mpds[0].Mode = BIT_MODE;
 // jlm may have to reset the state for whatever mode was running. in mpd 0
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
extern int AA_shootQ;
extern int AA_flash;

#define TEWS_AUTO_MODE      2
#define TEWS_MANUAL_MODE    0

void SetCockpitVars()
{
	Cpit.DecoyOnVar     =  ( (Av.Tews.CM.CurCMDDS != -1) && (Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_JAM) );
	if (UFC.EMISState)
		Cpit.DecoyOnVar=0;

	Cpit.DecoyDeployVar = ( (Av.Tews.CM.CurCMDDS != -1) && (PlayerPlane->TowedDecoyPosition > 0.0) );

	Cpit.StbyVar        = Av.Fuel.DumpFuel;

	if (Av.Tews.CurMode==TEWS_MANUAL_MODE)
	{
		Cpit.XmitVar        = (IsICSJamming() && !(UFC.EMISState));
//		Cpit.XmitVar        = IsICSJamming();                       // ics
		Cpit.RecVar         = (UFC.EMISState);
	} else {
		Cpit.XmitVar        = (IsICSJamming() && !(UFC.EMISState));
		Cpit.RecVar         = (UFC.EMISState || (!IsICSJamming()));
	}

	Cpit.ShootVar = ( AreMpdsInGivenMode(RADAR_MODE) && (UFC.MasterMode != AG_MODE) && (GetCurrentPrimaryTarget() != -1) && AA_shootQ && AA_flash);
	Cpit.LockVar  = ( AreMpdsInGivenMode(RADAR_MODE) && (UFC.MasterMode != AG_MODE) &&(GetCurrentAARadarMode() == AA_STT_MODE) );

	Cpit.LaserVar = (Av.Flir.LaserState == FLIR_LSR_FIRING);

	Cpit.LBleedVar = (PlayerPlane->DamageFlags & DAMAGE_BIT_L_BLEED_AIR) ? 1 : 0;
	Cpit.RBleedVar = (PlayerPlane->DamageFlags & DAMAGE_BIT_R_BLEED_AIR) ? 1 : 0;
	Cpit.AutoVar   = UFC.APStatus;

  if(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)
	   Cpit.NoseVar = Cpit.LeftVar = Cpit.RightVar = TRUE;
	else
	   Cpit.NoseVar = Cpit.LeftVar = Cpit.RightVar = FALSE;

  Cpit.FlapsVar = (PlayerPlane->Flaps);

	Cpit.HalfVar = (PlayerPlane->FlapsMode & FLAPS_HALF);
	Cpit.FullVar = (PlayerPlane->FlapsMode & FLAPS_FULL);

	Cpit.LeftFireReadyVar  = (Cpit.CanSimFireLeft);
	Cpit.RightFireReadyVar = (Cpit.CanSimFireRight);

	Cpit.SpeedBrakeVar = PlayerPlane->SpeedBrake;
	Cpit.BrakeVar      = PlayerPlane->Brakes;
	Cpit.APVar         = UFC.APStatus;
	Cpit.LBar          = ( (PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch) && (PlayerPlane->AI.lVar3 == 100) );
	Cpit.DispVar       = Av.Tews.CM.AutoTimer.IsActive();

	// Engine Fire Left/Right
	int onFire;

	onFire = (lFireFlags & FIRE_LEFT_ENGINE);
	if( onFire && CpitItems[12].State == 0 )
			CpitItems[12].State = 1;
	if( onFire && CpitItems[12].State == 2 )
			CpitItems[12].State = 3;

	onFire = (lFireFlags & FIRE_RIGHT_ENGINE);
	if( onFire && CpitItems[14].State == 0 )
			CpitItems[14].State = 1;
	if( onFire && CpitItems[14].State == 2 )
			CpitItems[14].State = 3;

	onFire = (lFireFlags & (FIRE_AMAD_LEFT|FIRE_AMAD_RIGHT)	);
	if( onFire && CpitItems[34].State == 0)
		CpitItems[34].State = 1;



/*
	
	if(!Cpit.CpitItem.FireLeftPanelUp)
	{
		CpitItems[12].State = ( Cpit.FireLeftVar || Cpit.LeftApuVar) ? 1 : 0;
	}
	else
	{
		if(!Cpit.CanSimFireLeft)
		  CpitItems[12].State = ( Cpit.FireLeftVar || Cpit.LeftApuVar) ? 3 : 2;
		else
		  CpitItems[12].State = ( Cpit.FireLeftVar || Cpit.LeftApuVar) ? 5 : 4;
	}

	if(!Cpit.CpitItem.FireRightPanelUp)
	{
		CpitItems[14].State = ( Cpit.FireRightVar || Cpit.RightApuVar) ? 1 : 0;
	}
	else
	{
		if(!Cpit.CanSimFireRight)
		  CpitItems[14].State = ( Cpit.FireRightVar || Cpit.RightApuVar) ? 3 : 2;
		else
		  CpitItems[14].State = ( Cpit.FireRightVar || Cpit.RightApuVar) ? 5 : 4;
	}
	*/


	if(Cpit.HSelClicked)
	{
		if(Cpit.HSelClicked == 1)
			Cpit.HSelVar = 0;
		else
			Cpit.HSelVar = 2;

		Cpit.HSelClicked = 0;
	}
	else
		Cpit.HSelVar = 1;

	if(Cpit.CSelClicked)
	{
		if(Cpit.CSelClicked == 1)
			Cpit.CSelVar = 0;
		else
			Cpit.CSelVar = 2;

	  Cpit.CSelClicked = 0;
	}
	else
		Cpit.CSelVar = 1;


	if( Cpit.MasterCautionShowBitTimer.IsActive() )
	{
	  if( Cpit.MasterCautionShowBitTimer.TimeUp(GameLoopInTicks) )
		{
			if( GetMouseLB() )
			    Mpds[0].Mode = BIT_MODE;	  // jlm may have to reset the state for whatever mode was running. in mpd 0
			Cpit.MasterCautionShowBitTimer.Disable();
		}
	}

}

//*****************************************************************************************************************************************
// JETTISON  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void JettisonStationPressed(int Station)
{
	JettButtons[Station] = !JettButtons[Station];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CtrClick(int *State)
{
	JettisonStationPressed(AV_CTR_LIGHT);
	*State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void LIClick(int *State)
{
	JettisonStationPressed(AV_LI_LIGHT);
  *State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RIClick(int *State)
{
	JettisonStationPressed(AV_RI_LIGHT);
	*State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void LMClick(int *State)
{
	JettisonStationPressed(AV_LM_LIGHT);
	*State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RMClick(int *State)
{
	JettisonStationPressed(AV_RM_LIGHT);
	*State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void LOClick(int *State)
{
	JettisonStationPressed(AV_LO_LIGHT);
	*State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ROClick(int *State)
{
	JettisonStationPressed(AV_RO_LIGHT);
	*State = !(*State);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JettSwitchClick(int *State)
{
	if(Cpit.SimJettOn) return;

	if(Cpit.CpitItem.JettSwitchDir == LEFT)
	{
    Cpit.CpitItem.JettSwitchState--;
		if(Cpit.CpitItem.JettSwitchState < AV_JET_EMERGENCY)
		{
			 Cpit.CpitItem.JettSwitchState = AV_JET_OFF;
			 Cpit.CpitItem.JettSwitchDir = RIGHT;
		}
	}
	else
	{
    Cpit.CpitItem.JettSwitchState++;
		if(Cpit.CpitItem.JettSwitchState > AV_JET_STORES)
		{
			 Cpit.CpitItem.JettSwitchState = AV_JET_RIGHT_FUSE;
			 Cpit.CpitItem.JettSwitchDir = LEFT;
		}
	}

	*State = Cpit.CpitItem.JettSwitchState;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JettisonClick(int *State)
{
  if ((PlayerPlane->OnGround) || (PlayerPlane->AGL <= 20))  return;	// Disable ALL ordnance till take-off.

  Cpit.SimJettOn = TRUE;
	Cpit.JettTimer.Set(0.3,GameLoopInTicks);

	*State = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int PlaneAttitudeWithinConstraints(PlaneParams *P);

int JettAll()
{
	int Continue = FALSE;

	if (!PlaneAttitudeWithinConstraints(PlayerPlane)) return (TRUE);
  if( !Cpit.JettTimer.TimeUp(GameLoopInTicks) ) return (TRUE);

	for(int i=0; i<MAX_F18E_STATIONS; i++)
	{
		if(i > RIGHT11_STATION) break;  // only use main stations

		if(!IsWeapPod(PlayerPlane->WeapLoad[i].WeapId) && (i != LEFT1_STATION) && (i != RIGHT11_STATION) )
		{
   			if((PlayerPlane->WeapLoad[i].Count > 0) || ((PlayerPlane->WeapLoad[i].bNumPods > 0) && ((PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU10_ID)) || (PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU68_ID)))))
	  		{
	        if( !((PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU10_ID)) || (PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU68_ID))) )
					{
			    	int Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;
						if(Type == WEAPON_TYPE_FUEL_TANK)
							DropFuelTankForDisplay(i);
		  		  InstantiateBomb(PlayerPlane,i,0);
		  		  PlayerPlane->WeapLoad[i].Count--;
					}
					else
					{
 		        WJettisonRocketPod(PlayerPlane,i);
					}

		  		if(PlayerPlane->WeapLoad[i].Count > 0)
		  		{
						Cpit.JettTimer.Set(0.3,GameLoopInTicks);
        		Continue = TRUE;
		  		}
		  		else
		  		{
						if( (Av.Weapons.CurAASeqWeap) && (&PlayerPlane->WeapLoad[i] == Av.Weapons.CurAASeqWeap->W) )
			 				Av.Weapons.CurAASeqWeap = NULL;
						if( (Av.Weapons.CurAAWeap) && (&PlayerPlane->WeapLoad[i] == Av.Weapons.CurAAWeap->W) )
			 				Av.Weapons.CurAAWeap = NULL;
					 //	if( (Av.Weapons.CurAGWeap) && (&PlayerPlane->WeapLoad[i] == Av.Weapons.CurAGWeap->W) )
			 		 //		Av.Weapons.CurAGWeap = NULL;

        		PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
		  		}
				}
		}
	}

	return(Continue);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int JettFuse()
{
	int Continue = FALSE;

	if (!PlaneAttitudeWithinConstraints(PlayerPlane)) return (TRUE);
	if( !Cpit.JettTimer.TimeUp(GameLoopInTicks) ) return (TRUE);

	int Station = (Cpit.CpitItem.JettSwitchState == AV_JET_LEFT_FUSE) ? LEFT5_STATION : RIGHT7_STATION;

  if(PlayerPlane->WeapLoad[Station].Count > 0)
	{
	  if(!IsWeapPod(PlayerPlane->WeapLoad[Station].WeapId))
		{
	      if( !((PlayerPlane->WeapLoad[Station].WeapId == GetWeapId(LAU10_ID)) || (PlayerPlane->WeapLoad[Station].WeapId == GetWeapId(LAU68_ID))) )
				{
			    int Type = pDBWeaponList[PlayerPlane->WeapLoad[Station].WeapIndex].iWeaponType;
					if(Type == WEAPON_TYPE_FUEL_TANK)
						DropFuelTankForDisplay(Station);
				  InstantiateBomb(PlayerPlane,Station,0);
				  PlayerPlane->WeapLoad[Station].Count--;
				}
				else
				{
 		       WJettisonRocketPod(PlayerPlane,Station);
				}

				if(PlayerPlane->WeapLoad[Station].Count > 0)
				{
		  		Cpit.JettTimer.Set(0.3,GameLoopInTicks);
					Continue = TRUE;
				}
				else
				{
					if( (Av.Weapons.CurAASeqWeap) && (&PlayerPlane->WeapLoad[Station] == Av.Weapons.CurAASeqWeap->W) )
			 			Av.Weapons.CurAASeqWeap = NULL;
					if( (Av.Weapons.CurAAWeap) && (&PlayerPlane->WeapLoad[Station] == Av.Weapons.CurAAWeap->W) )
			 			Av.Weapons.CurAAWeap = NULL;
				 //	if( (Av.Weapons.CurAGWeap) && (&PlayerPlane->WeapLoad[Station] == Av.Weapons.CurAGWeap->W) )
			 	 //		Av.Weapons.CurAGWeap = NULL;

      		PlayerPlane->WeapLoad[Station].Status = NO_WEAP_STAT;
				}
		}
	}

  return(Continue);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int AvJettStationMap[] = {CENTER6_STATION,LEFT4_STATION,RIGHT8_STATION,LEFT3_STATION,RIGHT9_STATION,LEFT2_STATION,RIGHT10_STATION,-1};

int JettStores()
{
	int Continue = FALSE;

	if (!PlaneAttitudeWithinConstraints(PlayerPlane)) return (TRUE);
  if( !Cpit.JettTimer.TimeUp(GameLoopInTicks) ) return (TRUE);

	int *Walk = &AvJettStationMap[0];
	while(*Walk != -1)
	{
		for(int i=0; i<MAX_F18E_STATIONS; i++)
		{
			if(i > RIGHT11_STATION) break;  // only use main stations

			if(i == *Walk)
			{
					if(JettButtons[Walk - &AvJettStationMap[0]])
					{
							if( !IsWeapPod(PlayerPlane->WeapLoad[i].WeapId) )
							{
   								if(PlayerPlane->WeapLoad[i].Count > 0)
	  							{
	                  if( !((PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU10_ID)) || (PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU68_ID))) )
										{
			                int Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;
											if(Type == WEAPON_TYPE_FUEL_TANK)
												DropFuelTankForDisplay(i);
		  							  InstantiateBomb(PlayerPlane,i,0);
		  							  PlayerPlane->WeapLoad[i].Count--;
										}
										else
										{
 										  WJettisonRocketPod(PlayerPlane,i);
										}

		  							if(PlayerPlane->WeapLoad[i].Count > 0)
		  							{
											Cpit.JettTimer.Set(0.3,GameLoopInTicks);
        							Continue = TRUE;
		  							}
		  							else
		  							{
											if( (Av.Weapons.CurAASeqWeap) && (&PlayerPlane->WeapLoad[i] == Av.Weapons.CurAASeqWeap->W) )
			 									Av.Weapons.CurAASeqWeap = NULL;
											if( (Av.Weapons.CurAAWeap) && (&PlayerPlane->WeapLoad[i] == Av.Weapons.CurAAWeap->W) )
			 									Av.Weapons.CurAAWeap = NULL;
											//if( (Av.Weapons.CurAGWeap) && (&PlayerPlane->WeapLoad[i] == Av.Weapons.CurAGWeap->W) )
			 								//	Av.Weapons.CurAGWeap = NULL;

        							PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
		  							}
									}
							} // if is pod
					}
			}
		}

		Walk++;
	}

	return(Continue);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetJettFromKeyboard(int Mode)
{
	// mode == 2 is jet all
	// mode == 3 is jet selected

  if ((PlayerPlane->OnGround) || (PlayerPlane->AGL <= 20))  return;	// Disable ALL ordnance till take-off.

	Cpit.SimJettOn = Mode;
	Cpit.JettTimer.Set(0.3,GameLoopInTicks);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimJett()
{
  if(!Cpit.SimJettOn) return;

	// I am using "simjetton" for keypress func. if == 2 then jett all  if == 3 then jet selected stores
	if(Cpit.CpitItem.JettSwitchState == AV_JET_OFF && (Cpit.SimJettOn == 1) )
	{
		Cpit.SimJettOn = FALSE;
	  return;
	}

	int Result = FALSE;

	if(Cpit.CpitItem.JettSwitchState == AV_JET_EMERGENCY || (Cpit.SimJettOn == 2) )
		Result = JettAll();
	else if( (Cpit.CpitItem.JettSwitchState == AV_JET_LEFT_FUSE) || (Cpit.CpitItem.JettSwitchState == AV_JET_RIGHT_FUSE) )
		Result = JettFuse();
	else if(Cpit.CpitItem.JettSwitchState == AV_JET_STORES || (Cpit.SimJettOn == 3) )
		Result = JettStores();

	if(!Result)
	{
		Cpit.SimJettOn = FALSE;
	  FixupAALoadout();
		FixupAGLoadout();
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void ApuFiresClick( int *State)
{
	int onFire = (lFireFlags & (FIRE_AMAD_LEFT| FIRE_AMAD_RIGHT) );

	if( onFire && *State == 1 ) 
	{
		if( !FireExtUsed )
			SetCockpitItemState( 248, 1);			// SET READY LIGHT
		*State = 3;
	}
}


void ReadyDischargeClick( int *State)
{
	if( !(*State) || (*State == 3))//not on or already discharged
		return;

	*State = 3; //Discharge

	// Attempt to Turn off fires here
	// state 5 is slectione on the left/right state 3 APU fire buttons
	SimFireExtinguish();

	SetCockpitItemState( 208, 0);
	SetCockpitItemState( 209, 0);
	SetCockpitItemState( 210, 0);

	FireExtUsed = 1;
}


void LeftFireClick(int *State)
{
	int onFire;

	onFire = (lFireFlags & FIRE_LEFT_ENGINE);

	if( !onFire )
	{
		// 0 <-> 2 
		if( *State == 0)
			*State = 2;
		else 
			*State = 0;
	}
	else
	{
		// 1->3->5     
		if(*State == 1)
			*State = 3;
		else if(*State == 3)
		{
			*State = 5;
			ShutLeftEngineDown();	//Shut down fuel flow

			if( !FireExtUsed )
				SetCockpitItemState( 248, 1);			// SET READY LIGHT
		}
		else if(*State == 5)
			*State = 3;

	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RightFireClick(int *State)
{
	int onFire;

	onFire = (lFireFlags & FIRE_RIGHT_ENGINE);


	if( !onFire )
	{
		// 0 <-> 2 
		if( *State == 0)
			*State = 2;
		else 
			*State = 0;
	}
	else
	{
		// 1->3->5     
		if(*State == 1)
			*State = 3;
		else if(*State == 3)
		{
			*State = 5;
			ShutRightEngineDown();

			if( !FireExtUsed )
				SetCockpitItemState( 248, 1);			// SET READY LIGHT
		}
		else if(*State == 5)
			*State = 3;

	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FireLeftDischargeClick(int *State)
{
	if(Cpit.LeftFireReadyVar)
	{
			Cpit.LeftExtinguishOn = TRUE;
			*State = 1;
	}

	if(Cpit.RightFireReadyVar)
	{
			Cpit.RightExtinguishOn = TRUE;
			*State = 1;
	}
}
//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
/*
void SimFireExtinguisher()
{
  int BaseFactor = 10;

  if(lFireFlags & (FIRE_LEFT_ENGINE  | FIRE_AMAD_LEFT))
	  	 Cpit.FireLeftVar = TRUE;

	if(lFireFlags & (FIRE_RIGHT_ENGINE | FIRE_AMAD_RIGHT))
	  	 Cpit.FireRightVar = TRUE;

	if(lFireFlags & FIRE_AMAD_RIGHT)
	  	 Cpit.RightApuVar = TRUE;

	if(lFireFlags & FIRE_AMAD_LEFT)
	  	 Cpit.LeftApuVar = TRUE;

	if(!Cpit.CanSimFireLeft && !Cpit.CanSimFireRight) return;

  if(Cpit.CanSimFireLeft)
  {
	  if( (Cpit.LeftExtinguishOn) && !(lFireFlags & FIRE_LEFT_ENGINE) )
	  {
	     ADEngineFireOut(TRUE);
       Cpit.CanSimFireLeft = FALSE;
	  }
    else if(lFireFlags & FIRE_LEFT_ENGINE)
    {
	  		if(Cpit.LeftExtinguishOn)
	  		{
  	    	int RandNum = rand() % 100;
				  RandNum = RandNum - 15*(float)(90000 - lLeftTimeToExplode)/90000.0;
	    		if(RandNum > BaseFactor)
					{
		 		    ADEngineFireOut(TRUE);
					}
				  else
					{
		 		    ADEngineFireTempOut(TRUE);
					}

					Cpit.CanSimFireLeft = FALSE;
				}
		}
  }

  if(Cpit.CanSimFireRight)
  {
	  if( (Cpit.RightExtinguishOn) && !(lFireFlags & FIRE_RIGHT_ENGINE) )
	  {
	    ADEngineFireOut(FALSE);
      Cpit.CanSimFireRight = FALSE;
	  }
    else if(lFireFlags & FIRE_RIGHT_ENGINE)
    {
	  		if(Cpit.RightExtinguishOn)
	  		{
  	    	int RandNum = rand() % 100;
				  RandNum = RandNum - 15*(float)(90000 - lRightTimeToExplode)/90000.0;
	    		if(RandNum > BaseFactor)
					{
		 		    ADEngineFireOut(FALSE);
					}
				  else
					{
		 		    ADEngineFireTempOut(FALSE);
					}

        	Cpit.CanSimFireRight = FALSE;
				}
		}
  }

  if(Cpit.CanSimFireLeft)
  {
    if(lFireFlags & HEAT_UP_AMAD_LEFT)
    {
	  	if(Cpit.LeftExtinguishOn)
	  	{
	    	ADEngineFireOut(TRUE,1);
        Cpit.CanSimFireLeft = FALSE;
	  	}
    }

    if(lFireFlags & FIRE_AMAD_LEFT)
    {
	  	if(Cpit.LeftExtinguishOn)
	  	{
  	    int RandNum = rand() % 100;
			  RandNum = RandNum - 15*(float)(180000 - lLAMADTimeToExplode)/90000.0;
	    	if(RandNum > BaseFactor)
				{
		 	    ADEngineFireOut(TRUE,1);
				}
		  	else
				{
		 	    ADEngineFireTempOut(TRUE,1);
				}
        Cpit.CanSimFireLeft = FALSE;
			}
		}
  }

  if(Cpit.CanSimFireRight)
  {
    if(lFireFlags & HEAT_UP_AMAD_RIGHT)
    {
	  	if(Cpit.RightExtinguishOn)
	  	{
	    	ADEngineFireOut(FALSE,1);
        Cpit.CanSimFireRight = FALSE;
	  	}
    }

    if(lFireFlags & FIRE_AMAD_RIGHT)
    {
	  	if(Cpit.RightExtinguishOn)
	  	{
  	    int RandNum = rand() % 100;
			  RandNum = RandNum - 15*(float)(180000 - lLAMADTimeToExplode)/90000.0;
	    	if(RandNum > BaseFactor)
				{
		 	    ADEngineFireOut(FALSE,1);
				}
			  else
				{
		 	    ADEngineFireTempOut(FALSE,1);
				}
        Cpit.CanSimFireRight = FALSE;
			}
		}
  }

}
*/
//*****************************************************************************************************************************************
// COCKPIT INTERACTIONS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void CockpitItemClick(int Var)
{
	CpitIType *Walk = &CpitItems[0];

	while(Walk->Var != -1)
	{
		if(Walk->Var == Var)
		{
			if( Walk->CpitClickFunc )
				Walk->CpitClickFunc(&Walk->State);
		}
		Walk++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetCockpitItemState(int Var)
{
	CpitIType *Walk = &CpitItems[0];

	while(Walk->Var != -1)
	{
		if(Walk->Var == Var)
			return(Walk->State);
		Walk++;
	}

	return(0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetCockpitItemState(int Var, int State)
{
	CpitIType *Walk = &CpitItems[0];

	while(Walk->Var != -1)
	{
		if(Walk->Var == Var)
		{
			Walk->State = State;
			return;
		}
		Walk++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetCockpitItemVars( char *pVars )
{
	CpitIType *pCType = &CpitItems[0];

	while(pCType->Var != -1)
	{
		*(pVars+pCType->Var ) = pCType->State;
		pCType++;
	}

	// special case for certain pushbuttons

	// set push button back up if pressed
	if(CpitItems[7].State == 1)  // eject button
		 CpitItems[7].State = 0;

	//if(CpitItems[13].State == 1)  // left discharge
	//	 CpitItems[13].State = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CheckHydStuff()
{
	int cnt;

	if(!(PlayerPlane->SystemInactive & DAMAGE_BIT_RADAR))
	{
		cnt = 0;
		if(Cpit.Damage[AV_L_MAIN_HYD])
			cnt ++;
		if(Cpit.Damage[AV_NO_HYD1B])
			cnt ++;
		if(Cpit.Damage[AV_EMER_HYD])
			cnt ++;
		if(Cpit.Damage[AV_R_MAIN_HYD])
			cnt ++;
		if(cnt >= 3)
			PlayerPlane->SystemInactive |= DAMAGE_BIT_RADAR;
	}
}

