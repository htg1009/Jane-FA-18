#include "avlocal.h"


int                NumMisslesChasing;
ChasingMisslesType MisslesInFlight[100];

int NumDTWSTargets = 0;
int DTWSTargets[100];

int LimitCount;

class Timer InterleaveTimer;

MpdClickType MpdClick;

WeapStoresType WeapStores;
int AVFoundOtherLaser(PlaneParams *planepnt, FPointDouble *rposition);
extern int	MySlot;		// my player's slot index [0..N]
void DrawMavTicks(WeapCamType *Cam,int Type,int IsMavLaser);
long lHARMToF = -1;
long lHARMLaunched = -1;
float AV_CurrAGMissileToF();
DWORD RadarFrame;
DetectedPlaneType *GetPlanePtrInList(DetectedPlaneListType *PList,int Id);
float GetCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);
BOOL PrimaryIsAOT(DetectedPlaneListType *Plist);
extern int Aim120_Id;
extern int		AA_Tact;		//	 prelaunch time-to-active (sec)
BOOL AVIsNoiseJamming(PlaneParams *planepnt);
void GetMPDButtonXY(int MpdDetectNum,int ButtonNum,char *Word,int &TextScrX,int &TextScrY);
void ProjectPointDouble(double X, double Z, float Degrees, float Magnitude, double *NewX, double *NewZ);

void SetRadarBaseSense();

void SetDebugCameraLine(FPointDouble &location,FMatrix &mat);

//*****************************************************************************************************************************************
// PRIMARY AND SECONDARY TARGETS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
TargetInfoType Targets[2];
TargetInfoType *Primary   = &Targets[0];
TargetInfoType *Secondary = &Targets[1];

//*****************************************************************************************************************************************
// TARGET LISTS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
DetectedPlaneListType Temp2CurFramePlanes;
DetectedPlaneListType TempCurFramePlanes;
DetectedPlaneListType CurFramePlanes;
DetectedPlaneListType PrevFramePlanes;
DetectedPlaneListType BlinkPlanes;
DetectedPlaneListType AgePlanes[AA_MAX_AGE_PLANES];
DetectedPlaneListType Age1Planes;  // JLM langtext has this externed. have Scott switch to AgePlanes[0]

DetectedGroundObjListType AGGroundObjs;
DetectedVehicleListType   AGVehicles;

DWORD AGRadarFrame;

void GetTargetClosestToCaptBars(DetectedPlaneType **Closest,float *Dist = NULL);
void DrawAvDot(float X, float Y, int Color, float Shade);

//*****************************************************************************************************************************************
// GLOBALS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int ScanStart;
CaptBarsType CaptBars;
CaptBarsType ScanAreaControl;

VKCODE CaptBarsVk;

VKCODE TdcLeftVk;
VKCODE TdcRightVk;
VKCODE TdcUpVk;
VKCODE TdcDownVk;
extern class Timer HudFlashTimer;
float RaidDistRange;
float ooRaidDistRange;
float RaidAngleRange;
float ooRaidAngleRange;

float STTDistLargerThanThisWillBreakLock;
class Timer STTBreakLockTimer;

//int NumIffPlanes;
//int IffPlanes[AA_MAX_DET_PLANES];

AwacsTargetListType AvAwacs;

AARankingListType Ranks;

//float AARadarBaseSense;
//float MinBase;
int BaseCount;
int RadarLastFrameSweepAngle;

int AARadarShowCrossSig;

AvRGB AvPalette[AV_PALETTE_MAX_COLORS][AV_PALETTE_NUM_SHADES];

MpdButtonPressType MpdButton;

int BarList[] = {1,2,4,6};
int *AABars = &BarList[1];

class Timer AA1SecBlink;
int AA1SecBlinkState = FALSE;

int NumTewsDraw;

int TDCMpdNum;   // set to the current mpd that has control for capt bars and other things

FPointDouble ScanAreas[3][4];
FPointDouble ScanAreasWorld[3][4];
FPointDouble ScanAreasPixel[3][4];
FPointDouble ScanAreasZoomPixel[3][4];

FPointDouble ScanAreasZoomWorld[3][4];

FPointDouble ScanBoundPixel[4];

FPointDouble ScanBoundZoomWorld[3][4];
FPointDouble ScanBoundZoomPixel[3][4];

FPointDouble ScreenZoomWorld[4];

float  ScanBoundClipLeft;
float  ScanBoundClipRight;
float  ScanBoundClipTop;
float  ScanBoundClipBottom;

#define AV_MAX_GPS_LABELS 100
int AvNumGpsLabels;
MissionLabelType *AvGpsLabels[AV_MAX_GPS_LABELS];
void UpdateAATarget(PlaneParams *P);

// jlm debug
int DebugTewsTest;

void HudHarmSPMode(PlaneParams *P);

RadarLadderType ADIAirToAirLadder;

class Timer AcqTimer;

long lShowLostTimer = 5000;

void InitAARadar()
{
	Av.AARadar.AzimLimit = 20;
	Av.AARadar.CosAzimLimit = cos((Av.AARadar.AzimLimit)*DEGREES_TO_RADIANS);
	Av.AARadar.LeftAzimEdge = Av.AARadar.AzimLimit ;
	Av.AARadar.RightAzimEdge = -Av.AARadar.AzimLimit ;
	Av.AARadar.Range = 40;			// miles
	AABars = &BarList[2];
	Av.AARadar.Bars = *AABars;
	Av.AARadar.BarCount = Av.AARadar.Bars;
	Av.AARadar.ElevLimit = GetAngularCoverage(Av.AARadar.Range,Av.AARadar.BarCount)/2.0;
	Av.AARadar.CosElevLimit = cos((Av.AARadar.ElevLimit)*DEGREES_TO_RADIANS);
	Av.AARadar.AntElevation = Av.AARadar.ElevLimit;
	Av.AARadar.UpElevEdge   = Av.AARadar.ElevLimit;
	Av.AARadar.DownElevEdge = Av.AARadar.UpElevEdge - (Av.AARadar.ElevLimit*2);

	Av.AARadar.RightAzimEdge = -Av.AARadar.AzimLimit ;

	Av.AARadar.SlewUpDown  = 0;
	Av.AARadar.SlewLeftRight = 0;
	Av.AARadar.ScanRate = 14.0;
	Av.AARadar.SearchPrf = SEARCH_I_MODE;
	Av.AARadar.FirstTime = 1;
	Av.AARadar.DiffCount = 0;
	Av.AARadar.FrameStoreCount = 3;
	Av.AARadar.AntAzimuth = 30.0;
	Av.AARadar.ScanDirection = LTOR;
	Av.AARadar.Prf = HPRF;
	Av.AARadar.PrfFrameStart	=  HPRF;
	Av.AARadar.CurMode = AA_RWS_MODE;
	Av.AARadar.RaidOn = FALSE;
	Av.AARadar.NCTROn = FALSE;
	Av.AARadar.AgeFrequency = 2.0;
	Av.AARadar.SilentOn = FALSE;
	Av.AARadar.Declutter = 0;
	Av.AARadar.MSI = OFF;
	Av.AARadar.AutoSlewOn = TRUE;
	Av.AARadar.VsLimit = 2400.0;
	Av.AARadar.AutoOn = TRUE;
	Av.AARadar.IffMode = AA_IFF_AUTO;
	Av.AARadar.SttJamming = FALSE;

	ResetPrimary();
 	ResetSecondary();

	CaptBars.Active = FALSE;
	CaptBars.Pos.X  = AA_RADAR_CENTER_X;
	CaptBars.Pos.Y = AA_RADAR_CENTER_Y;

	ResetPrimary();
	ResetSecondary();

//	NumIffPlanes = 0;

	AvAwacs.NumTargets = 0;

	AirToAirLadder.RawRadarPoint[0].X = -XMaxPos;
	AirToAirLadder.RawRadarPoint[0].Y = 0;
	AirToAirLadder.RawRadarPoint[0].Z = -ZPos;

	AirToAirLadder.RawRadarPoint[1].X = -XMinPos;
	AirToAirLadder.RawRadarPoint[1].Y = 0;
	AirToAirLadder.RawRadarPoint[1].Z = -ZPos;

	AirToAirLadder.RawRadarPoint[2].X = XMinPos;
	AirToAirLadder.RawRadarPoint[2].Y = 0;
	AirToAirLadder.RawRadarPoint[2].Z = -ZPos;

	AirToAirLadder.RawRadarPoint[3].X = XMaxPos;
	AirToAirLadder.RawRadarPoint[3].Y = 0;
	AirToAirLadder.RawRadarPoint[3].Z = -ZPos;

	AirToAirLadder.RawRadarPoint[4].X = AirToAirLadder.RawRadarPoint[0].X;
	AirToAirLadder.RawRadarPoint[4].Y = AirToAirLadder.RawRadarPoint[0].Y - DownSideLine;
	AirToAirLadder.RawRadarPoint[4].Z = AirToAirLadder.RawRadarPoint[0].Z;

	AirToAirLadder.RawRadarPoint[5].X = AirToAirLadder.RawRadarPoint[3].X;
	AirToAirLadder.RawRadarPoint[5].Y = AirToAirLadder.RawRadarPoint[3].Y - DownSideLine;
	AirToAirLadder.RawRadarPoint[5].Z = AirToAirLadder.RawRadarPoint[3].Z;


	ADIAirToAirLadder.RawRadarPoint[0].X = -XMaxPos - 200;
	ADIAirToAirLadder.RawRadarPoint[0].Y = 0;
	ADIAirToAirLadder.RawRadarPoint[0].Z = -ZPos;

	ADIAirToAirLadder.RawRadarPoint[1].X = -XMinPos;
	ADIAirToAirLadder.RawRadarPoint[1].Y = 0;
	ADIAirToAirLadder.RawRadarPoint[1].Z = -ZPos;

	ADIAirToAirLadder.RawRadarPoint[2].X = XMinPos;
	ADIAirToAirLadder.RawRadarPoint[2].Y = 0;
	ADIAirToAirLadder.RawRadarPoint[2].Z = -ZPos;

	ADIAirToAirLadder.RawRadarPoint[3].X = XMaxPos + 200;
	ADIAirToAirLadder.RawRadarPoint[3].Y = 0;
	ADIAirToAirLadder.RawRadarPoint[3].Z = -ZPos;

	ADIAirToAirLadder.RawRadarPoint[4].X = ADIAirToAirLadder.RawRadarPoint[0].X;
	ADIAirToAirLadder.RawRadarPoint[4].Y = ADIAirToAirLadder.RawRadarPoint[0].Y - 120;
	ADIAirToAirLadder.RawRadarPoint[4].Z = ADIAirToAirLadder.RawRadarPoint[0].Z;

	ADIAirToAirLadder.RawRadarPoint[5].X = ADIAirToAirLadder.RawRadarPoint[3].X;
	ADIAirToAirLadder.RawRadarPoint[5].Y = ADIAirToAirLadder.RawRadarPoint[3].Y - 120;
	ADIAirToAirLadder.RawRadarPoint[5].Z = ADIAirToAirLadder.RawRadarPoint[3].Z;


	for(int i=0; i<AA_MAX_AGE_PLANES; i++)
		AgePlanes[i].NumPlanes=0;

	CurFramePlanes.NumPlanes = 0;
	PrevFramePlanes.NumPlanes = 0;

 	NumMisslesChasing = 0;

//	AARadarBaseSense = 9;
	Av.AARadar.AOTDamping = 4.0f;
	BaseCount = 0;

	AARadarShowCrossSig = FALSE;
}

//*****************************************************************************************************************************************
// cleanup  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void CleanupAARadar()
{



}

//*****************************************************************************************************************************************
// RESET  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void ResetPlaneLists()
{
	TempCurFramePlanes.NumPlanes = 0;
	CurFramePlanes.NumPlanes = 0;
	BlinkPlanes.NumPlanes = 0;

	DetectedPlaneListType *P = &AgePlanes[0];
	int Index = AA_MAX_AGE_PLANES;
	while(Index-- > 0)
		P->NumPlanes = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetPrimary()
{

	DetectedPlaneType *P = GetPlanePtrInList(&PrevFramePlanes,Primary->Id);

	if(P)
		P->Designation &= ~AA_PRIMARY;


	P = GetPlanePtrInList(&CurFramePlanes,Primary->Id);

	if(P)
		P->Designation &= ~AA_PRIMARY;

	Primary->Id = -1;
	PlayerPlane->AADesignate = NULL;
	Primary->NCTRPercent = 0;

	Primary->CriticalAspectTimer.Disable();
	Primary->Reaquire.BreakLockTimer.Disable();
	Primary->Reaquire.BrokeLockReaquire = FALSE;

 	Primary->Jammer.JammerTimer.Disable();
	Primary->Jammer.JammerFakeOut     = FALSE;
	Primary->Jammer.TargetJamming     = FALSE;
	Primary->IDLevel = AA_UNKNOWN;

	Av.AARadar.SttJamming = FALSE;

	ResetBrokeLockHudTimer();

	FPointDouble NullPos;
	NullPos.X = -1; NullPos.Y = -1; NullPos.Z = -1;
	SetHudTDInfo(NullPos,0); // 0 = primary, 1 = secondary
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetSecondary()
{

	DetectedPlaneType *P = GetPlanePtrInList(&CurFramePlanes,Secondary->Id);

	if(P)
		P->Designation &= ~AA_SECONDARY;

	P = GetPlanePtrInList(&PrevFramePlanes,Secondary->Id);

	if(P)
		P->Designation &= ~AA_SECONDARY;

	Secondary->Id = -1;
	FPointDouble NullPos;
	SetHudTDInfo(NullPos,1); // 0 = primary, 1 = secondary


}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClearAgePlanes()
{
	AgePlanes[0].NumPlanes = 0;
	AgePlanes[1].NumPlanes = 0;
	AgePlanes[2].NumPlanes = 0;
	AgePlanes[3].NumPlanes = 0;
	AgePlanes[4].NumPlanes = 0;
	AgePlanes[5].NumPlanes = 0;
	AgePlanes[6].NumPlanes = 0;
	AgePlanes[7].NumPlanes = 0;
	AgePlanes[8].NumPlanes = 0;
	AgePlanes[9].NumPlanes = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetAARadar(int Mode,int ResetRange=1, int ResetAzim=1, int ResetBar=1)
{

	STTBreakLockTimer.Disable();

	switch(Mode)
	{
		case AA_RWS_MODE:
			if(ResetAzim)
				Av.AARadar.AzimLimit = 20;
			Av.AARadar.CosAzimLimit = cos((Av.AARadar.AzimLimit)*DEGREES_TO_RADIANS);
			Av.AARadar.LeftAzimEdge = Av.AARadar.AzimLimit ;
			Av.AARadar.RightAzimEdge = -Av.AARadar.AzimLimit ;
			if(ResetRange)
			Av.AARadar.Range = 40;

			if(ResetAzim)
				AABars = &BarList[2];

			Av.AARadar.Bars = *AABars;
			Av.AARadar.BarCount = Av.AARadar.Bars;
			Av.AARadar.ElevLimit = GetAngularCoverage(Av.AARadar.Range,Av.AARadar.BarCount)/2.0;
			Av.AARadar.CosElevLimit = cos((Av.AARadar.ElevLimit)*DEGREES_TO_RADIANS);
			Av.AARadar.AntElevation = Av.AARadar.ElevLimit;
			Av.AARadar.UpElevEdge   = Av.AARadar.ElevLimit;
			Av.AARadar.DownElevEdge = Av.AARadar.UpElevEdge - (Av.AARadar.ElevLimit*2);

			Av.AARadar.RightAzimEdge = -Av.AARadar.AzimLimit ;

			Av.AARadar.SlewUpDown  = 0;
			Av.AARadar.SlewLeftRight = 0;
			Av.AARadar.ScanRate = 14.0;
			Av.AARadar.SearchPrf = SEARCH_I_MODE;
			Av.AARadar.FirstTime = 1;
			Av.AARadar.DiffCount = 0;
			Av.AARadar.AntAzimuth = 30.0;
			Av.AARadar.ScanDirection = LTOR;
			Av.AARadar.Prf = HPRF;
			Av.AARadar.PrfFrameStart	=  HPRF;
			Av.AARadar.SlewUpDown    = 0;
		break;

		case AA_VS_MODE:
			ResetPlaneLists();
			Av.AARadar.Bars = *AABars;

		break;

		case AA_STT_MODE:
			if (Primary && (Primary->Id != -1))
			{
//				float dist = Planes[Primary->Id].WorldPosition / PlayerPlane->WorldPosition;

//				dist /= 80.0 * NMTOWU;

				STTDistLargerThanThisWillBreakLock = frand();
//				STTDistLargerThanThisWillBreakLock *= STTDistLargerThanThisWillBreakLock;
//				STTDistLargerThanThisWillBreakLock *= 65.0f*NMTOWU;
//				STTDistLargerThanThisWillBreakLock += 15.0f*NMTOWU;

				float skill = Planes[Primary->Id].AI.iSkill;
				skill = 2.5f*skill+2.0f;
				skill += skill * frand();

				STTBreakLockTimer.Set(skill,GameLoopInTicks);
			}
			else
				STTDistLargerThanThisWillBreakLock = 1000.0*NMTOWU;

			break;

		case AA_TWS_MODE:
			if (ResetAzim)
			{
				Av.AARadar.AzimLimit = 20.0f;
				AABars = BarList + 2;
			}
			else
			{
				if(Av.AARadar.AzimLimit > 40.0)
					Av.AARadar.AzimLimit = 10;

				if(ResetBar)
				{
					if(Av.AARadar.AzimLimit == 10.0)
						AABars = BarList + 3;
					else if(Av.AARadar.AzimLimit == 20.0)
						AABars = BarList + 2;
					else if(Av.AARadar.AzimLimit == 30.0)
						AABars = BarList + 1;
					else if(Av.AARadar.AzimLimit == 40.0)
						AABars = BarList + 1;

				}
			}
			Av.AARadar.AutoOn = TRUE;
			Av.AARadar.SlewUpDown    = 0;
	 		Av.AARadar.Bars = *AABars;
		break;

		case AA_ACQ_MODE:
		break;
	}

	if (Mode != AA_TWS_MODE)
	{
		ResetSecondary();
		PrevFramePlanes.NumPlanes = 0;
	}

	Av.AARadar.RaidOn = FALSE;
	Av.AARadar.LeftAzimEdge  = Av.AARadar.AzimLimit;
	Av.AARadar.RightAzimEdge = -Av.AARadar.AzimLimit;
	Av.AARadar.BarCount = Av.AARadar.Bars;
	Av.AARadar.AntAzimuth = 0;
	Av.AARadar.AntElevation = Av.AARadar.ElevLimit;
	Av.AARadar.UpElevEdge   = Av.AARadar.ElevLimit;
	Av.AARadar.DownElevEdge = -Av.AARadar.ElevLimit;//(Av.AARadar.UpElevEdge - (Av.AARadar.ElevLimit*2))  + Av.AARadar.SlewUpDown;
	Av.AARadar.SlewLeftRight = 0;
	Av.AARadar.ElevStep = Av.AARadar.ElevLimit*2.0/(float)(Av.AARadar.Bars);

	Av.AARadar.ScanDirection = LTOR;
	Av.AARadar.RaidOn = FALSE;
	Av.AARadar.SttJamming = FALSE;
//	AARadarBaseSense = 9;
//	CurFramePlanes.NumPlanes = 0;
	ClearAgePlanes();
}

//*****************************************************************************************************************************************
// NCTR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

float CalcNCTRPercentAddition(PlaneParams *Enemy)
{
	float AzimFraction;
	float RangeFraction;
	float DistToPrime;
	float Dot;
	FPointDouble EnemyToPlayerVec;

	EnemyToPlayerVec.MakeVectorToFrom(PlayerPlane->WorldPosition,Enemy->WorldPosition);

	DistToPrime = EnemyToPlayerVec.Normalize()*WUTONM;

	Dot = EnemyToPlayerVec * Enemy->Orientation.I;

	if (Dot > 0.985) //10 degrees
		AzimFraction = 1.75f;
	else
		if (Dot > 0.867f) //30 degrees
			AzimFraction = (Dot-0.867f)*(1.75f/(0.985f-0.867f));
		else
			return 0.0f;

	if (DistToPrime < 20.0f)
		RangeFraction = 1.75f;
	else
		if (DistToPrime < 40.0f)
			RangeFraction = (40.0f-DistToPrime)*(1.75f/20.0f);
		else
			return 0.0f;

	return AzimFraction * RangeFraction;

}

void SimNCTR()
{
	if((Av.AARadar.NCTROn) && (Primary->Id != -1))
	{
		if(Primary->NCTRTimer.IsActive())
		{
			if( Primary->NCTRTimer.TimeUp(GameLoopInTicks) )
			{
				DetectedPlaneType *p = GetPlanePtrInList(&CurFramePlanes,Primary->Id);

				if(p && !p->AOT && (p->Designation & AA_ON_RADAR))
				{
					Primary->NCTRPercent += CalcNCTRPercentAddition(&Planes[p->PlaneId]);

					if(Primary->NCTRPercent > 100.0)
						Primary->NCTRPercent = 100.0;
				}

				Primary->NCTRTimer.Set(1.0,GameLoopInTicks);
			}
		}
		else
			Primary->NCTRTimer.Set(1.0,GameLoopInTicks);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimSTTNCTR(BOOL aot)
{
	if((Av.AARadar.NCTROn) && (Primary->Id != -1))
	{
		if(Primary->NCTRTimer.IsActive())
		{
			if( Primary->NCTRTimer.TimeUp(GameLoopInTicks) && !aot)
			{
				Primary->NCTRPercent += CalcNCTRPercentAddition(&Planes[Primary->Id]);

				if(Primary->NCTRPercent > 100.0)
					Primary->NCTRPercent = 100.0;

		  		Primary->NCTRTimer.Set(1.0,GameLoopInTicks);
			}
		}
		else
			Primary->NCTRTimer.Set(1.0,GameLoopInTicks);
	}
}

//*****************************************************************************************************************************************
// AWACS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AddTargetToAwacs(int PlaneIndex, FPointDouble WorldPosition, float Heading, float RelVelocity, int IDLevel)
{
	if(AvAwacs.NumTargets + 1 >= AA_MAX_AWACS_TARGETS) return;

	AwacsTargetType *Target = &AvAwacs.Targets[0];
	int Index = AvAwacs.NumTargets;
	while(Index-- > 0)
	{
	 if(Target->PlaneId == PlaneIndex)
		return;
	 Target++;
	}

	AvAwacs.NumTargets++;
	Target = &AvAwacs.Targets[AvAwacs.NumTargets-1];
	Target->PlaneId = PlaneIndex;
	Target->WorldPosition = WorldPosition;
	Target->IDLevel = IDLevel;
	Target->RelVel = RelVelocity;
	Target->RelAngle = Heading;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClearAwacsTargetList()
{
  AvAwacs.NumTargets = 0;
}

//*****************************************************************************************************************************************
// IFF  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AddIffPlane(int Id)
{
//	if(NumIffPlanes + 1 >= AA_MAX_DET_PLANES) return;

//	int Found = FALSE;
//	int *Ptr = &IffPlanes[0];

//	int Index = NumIffPlanes;
//	while(Index-- > 0)
//	{
//		if(*Ptr == Id)
//		{
//		 Found = TRUE;
//		 break;
//		}
//	  Ptr++;
//	}

//	if(!Found)
//	{
//		 NumIffPlanes++;
//		 IffPlanes[NumIffPlanes-1] = Id;
//	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InterrogateIffPlanes()
{
	if(Av.AARadar.IffMode == AA_IFF_INTERROGATE)
		Av.AARadar.IffMode = AA_IFF_AUTO;
	else
	{
		Av.AARadar.IffMode = AA_IFF_INTERROGATE;
		Av.AARadar.IffTimer.Set(5.0, GetTickCount() );
	}
}
//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
// this call should not be used. Auto is always on
void AutoIffPlanes()
{
	Av.AARadar.IffMode = (Av.AARadar.IffMode == AA_IFF_AUTO) ? AA_IFF_OFF : AA_IFF_AUTO;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimIff()
{
	if(Av.AARadar.IffMode != AA_IFF_INTERROGATE)
		Av.AARadar.IffTimer.Disable();

	if( Av.AARadar.IffTimer.IsActive() )
	{
   		DetectedPlaneType *P = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes];

   		while(P-- > CurFramePlanes.Planes)
		{
			PlaneParams *Targ = &Planes[P->PlaneId];

			if(Targ->AI.iSide == AI_FRIENDLY)
				P->IDLevel = AA_FRIENDLY;
		}

		if (Av.AARadar.CurMode == AA_TWS_MODE)
		{

   			DetectedPlaneType *P = &PrevFramePlanes.Planes[PrevFramePlanes.NumPlanes];

   			while(P-- > PrevFramePlanes.Planes)
			{
				PlaneParams *Targ = &Planes[P->PlaneId];

				if(Targ->AI.iSide == AI_FRIENDLY)
					P->IDLevel = AA_FRIENDLY;
			}
		}


		if( Av.AARadar.IffTimer.TimeUp( GetTickCount() ) )
		{
			Av.AARadar.IffMode = AA_IFF_AUTO;
			Av.AARadar.IffTimer.Disable();
		}
	}
}

void ClearIFF()
{
	if( !Av.AARadar.IffTimer.IsActive() )
	{
   		DetectedPlaneType *P = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes];

   		while(P-- > CurFramePlanes.Planes)
		{
			if (!(P->Designation & AA_AWACS))
				P->IDLevel = AA_UNKNOWN;
		}
	}
}

//*****************************************************************************************************************************************
// TARGET SELECT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AASelectClosestTarget()
{
	 DetectedPlaneType *P;
	 DetectedPlaneType *Closest = NULL;
	 int l;

	float MinDist = 100000000;

	P = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes];
	while(P-- > CurFramePlanes.Planes)
	{
		if ((P->Designation & AA_ON_RADAR) || !(P->Designation & AA_NO_LANDS))
		{
			float DistPlayerToPlane = PlayerPlane->WorldPosition / (&Planes[P->PlaneId])->WorldPosition;

			if(DistPlayerToPlane < MinDist)
			{
				MinDist = DistPlayerToPlane;
				Closest = P;
				l = P->IDLevel;
			}
		}
	}

	ResetPrimary();
	if (Closest)
	{
	 	Primary->Id = Closest->PlaneId;
		Primary->IDLevel = l;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AASelectNextTarget()
{
	DetectedPlaneType *P;
	DetectedPlaneType *Closest;
	int l;

	float CurTargetDist,MinDist;

	CurTargetDist = (Primary->Id == -1) ? 0 : PlayerPlane->WorldPosition / (&Planes[Primary->Id])->WorldPosition;

	Closest = NULL;
	MinDist = 100000000;

	P = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes];
	while(P-- > CurFramePlanes.Planes)
	{
		if((P->PlaneId != Primary->Id) && ((P->Designation & AA_ON_RADAR) || !(P->Designation & AA_NO_LANDS)))
		{
			float DistFromPlayerToPlane = PlayerPlane->WorldPosition / (&Planes[P->PlaneId])->WorldPosition;

			if((DistFromPlayerToPlane > CurTargetDist) && (DistFromPlayerToPlane < MinDist))
			{
				MinDist = DistFromPlayerToPlane;
				Closest = P;
				l = P->IDLevel;
			}
		}
	}

	if(Closest)
	{
		ResetPrimary();
		Primary->Id = Closest->PlaneId;
		Primary->IDLevel = l;
	}
	else
		AASelectClosestTarget();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


BOOL AutoGrabSeekTarget()
{
	DetectedPlaneType *P;
	GetTargetClosestToCaptBars(&P,NULL);

	if (P && ((P->Designation & AA_ON_RADAR) || (!Av.AARadar.SilentOn && !UFC.EMISState && !(P->Designation & AA_NO_LANDS))))
	{
		ResetPrimary();
	 	Primary->Id = P->PlaneId;
		Primary->IDLevel = P->IDLevel;
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RWSTargetSelect(DetectedPlaneType *P)
{
	if((P->PlaneId == Primary->Id) && ((P->Designation & AA_ON_RADAR) || (!Av.AARadar.SilentOn && !UFC.EMISState && !(P->Designation & AA_NO_LANDS))))
	{
		Av.AARadar.CurMode = AA_STT_MODE;
		ResetAARadar(AA_STT_MODE);
	}
	else
	{
		ResetPrimary();
		Primary->Id = P->PlaneId;
		Primary->IDLevel = P->IDLevel;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TWSTargetSelect(DetectedPlaneType *P)
{
	if((P->PlaneId == Primary->Id) && ((P->Designation & AA_ON_RADAR) || (!Av.AARadar.SilentOn && !UFC.EMISState && !(P->Designation & AA_NO_LANDS))))
	{
		Av.AARadar.CurMode = AA_STT_MODE;
		ResetAARadar(AA_STT_MODE);
	}
	else
	{
		if(Primary->Id == -1)
		{
			ResetPrimary();
			Primary->Id = P->PlaneId;
			Primary->IDLevel = P->IDLevel;
		}
		else
		{
			 int PrimeId = Primary->Id;
			 int pl = Primary->IDLevel;

			 ResetPrimary();
			 Primary->Id = P->PlaneId;
			 Primary->IDLevel = P->IDLevel;

			 ResetSecondary();
			 Secondary->Id = PrimeId;
			 Secondary->IDLevel = pl;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RWSUndesignateTarget()
{
	if(Primary->Id != -1)
		AASelectNextTarget();
	else
	 	AASelectClosestTarget();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TWSUndesignateTarget()
{
	if(Secondary->Id != -1)
	{
		int Temp = Primary->Id;
		int l = Primary->IDLevel;
		ResetPrimary();
 		Primary->Id = Secondary->Id;
		Primary->IDLevel = Secondary->IDLevel;
		Secondary->Id = Temp;
		Secondary->IDLevel = l;
	}
	else
		if(Primary->Id != -1)
	  		AASelectNextTarget();
		else
			AASelectClosestTarget();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetToRWS()
{
	Av.AARadar.CurMode = AA_RWS_MODE;
	ResetAARadar(AA_RWS_MODE);
	ResetPrimary();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AARadarUnDesignateTarget()
{
  	if( (Av.AARadar.CurMode == AA_RWS_MODE) || (Av.AARadar.CurMode == AA_VS_MODE) )
		RWSUndesignateTarget();
	else if(Av.AARadar.CurMode == AA_TWS_MODE)
		TWSUndesignateTarget();
	else if( (Av.AARadar.CurMode == AA_STT_MODE) || (Av.AARadar.CurMode == AA_ACQ_MODE) )
		ResetToRWS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetTargetClosestToCaptBars(DetectedPlaneType **Closest,float *Dist)
{
	DetectedPlaneListType *PList = (Av.AARadar.CurMode == AA_TWS_MODE) ? &PrevFramePlanes : &CurFramePlanes;
	DetectedPlaneType *P = &PList->Planes[PList->NumPlanes];
	*Closest = NULL;

	float MinDist = 10000000;
	int   Found = FALSE;

	while(P-- > PList->Planes)
  	{
		FPointDouble PlanePos,CursorPos;

		if ((P->Designation & AA_ON_RADAR) || !(P->Designation & AA_NO_LANDS))
		{
			PlanePos.X = P->OfsX;
			PlanePos.Z = P->OfsY;
			PlanePos.Y = 0;

	  		CursorPos.X = CaptBars.Pos.X;
			CursorPos.Z = CaptBars.Pos.Y;
			CursorPos.Y = 0;


			float dist = Dist2D(&PlanePos,&CursorPos);

			if(dist < MinDist)
			{
				MinDist = dist;
				*Closest = P;
			}
		}
	}

	if (Dist)
		*Dist = MinDist;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AARadarAutoAcqTarget()
{
	if(Primary->Id != -1)
	{
		DetectedPlaneListType *PList = (Av.AARadar.CurMode == AA_TWS_MODE) ? &PrevFramePlanes : &CurFramePlanes;
		DetectedPlaneType *P;

		if (P = GetPlanePtrInList(PList,Primary->Id))
		{
			if ((P->Designation & AA_ON_RADAR) || (!Av.AARadar.SilentOn && !UFC.EMISState && !(P->Designation & AA_NO_LANDS)))
			{
				Av.AARadar.CurMode = AA_STT_MODE;
				ResetAARadar(AA_STT_MODE);
			}
			else
			{
				ResetPrimary();
				Primary->Id = P->PlaneId;
				Primary->IDLevel = P->IDLevel;
			}
		}
		else
		{
			int pid = Primary->Id;
			int plevel = Primary->IDLevel;


			ResetPrimary();

			Primary->Id = pid;
			Primary->IDLevel = plevel;
		}
  	}
	else
	{
		if (AutoGrabSeekTarget())
		{
			Av.AARadar.CurMode = AA_STT_MODE;
			ResetAARadar(AA_STT_MODE);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AARadarDesignateTarget()
{
	DetectedPlaneType *Closest;

	float MinDist;

  	GetTargetClosestToCaptBars(&Closest,&MinDist);

	if( (Closest) && (MinDist <= 7) )
	{
		if( (Av.AARadar.CurMode == AA_RWS_MODE) || (Av.AARadar.CurMode == AA_VS_MODE) )
			RWSTargetSelect(Closest);
		else if(Av.AARadar.CurMode == AA_TWS_MODE)
			TWSTargetSelect(Closest);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AAResetTargets()
{
  if( (Av.AARadar.CurMode == AA_RWS_MODE) || (Av.AARadar.CurMode == AA_VS_MODE) )
	{
	  ResetPrimary();
	  ResetSecondary();
	}
	else if(Av.AARadar.CurMode == AA_TWS_MODE)
	{
	  ResetSecondary();
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AABreakLock()
{
	if(Av.AARadar.CurMode == AA_STT_MODE)
	{
  		Av.AARadar.CurMode = AA_RWS_MODE;
  		ResetAARadar(AA_RWS_MODE);
  		ResetPrimary();
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int TWSCheckPrimaryAndSecondaryTargets()
{
	int Changed = FALSE;

	if( !IsPlaneInList(&PrevFramePlanes,Primary->Id) )
	{
		if( IsPlaneInList(&PrevFramePlanes,Secondary->Id) )  // if you have secondary, it becomes the primary
		{
			ResetPrimary();
			Primary->Id = Secondary->Id;
			Primary->IDLevel = Secondary->IDLevel;
			Secondary->Id = -1;
			Changed = TRUE;
		}
		else  // set the nearest ranking
	 	{
			for(int i=1; i<=8; i++)
			{
				int Index = GetPlaneRankPosInList(&PrevFramePlanes,i);

				if(Index != -1) // found
				{
					ResetPrimary();
					Primary->Id = PrevFramePlanes.Planes[Index].PlaneId;
					Primary->IDLevel = PrevFramePlanes.Planes[Index].IDLevel;
					Secondary->Id = -1;
					Changed = TRUE;
				}
			}
		}

    if(!Changed)
		{
			ResetPrimary();
			Secondary->Id = -1;
		}
	}

  return(Changed);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int RWSCheckPrimaryAndSecondaryTargets()
{
	if(Primary->Id == -1) return(FALSE);

	int Changed = FALSE;

	if( !IsPlaneInList(&CurFramePlanes,Primary->Id) )
	{
		ResetPrimary();
		Changed = TRUE;
	}

  return(Changed);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetTargetPrimarySecondary()
{
  	DetectedPlaneType *P;
  	DetectedPlaneListType *PList = (Av.AARadar.CurMode == AA_TWS_MODE) ? &PrevFramePlanes : &CurFramePlanes;

   	P = &PList->Planes[PList->NumPlanes];
  	while(P-- > PList->Planes)
	{
		P->Designation &= ~(AA_PRIMARY | AA_SECONDARY);

		if(P->PlaneId == Primary->Id)
			P->Designation |= AA_PRIMARY;

		if(P->PlaneId == Secondary->Id)
			P->Designation |= AA_SECONDARY;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetupPlanesForFiltering()  // this func makes sure the prime and sec targets are at 0 and 1 element of the list, so they don't get range resolutioned out
{
	 DetectedPlaneType *Prime   = &CurFramePlanes.Planes[0];
	 DetectedPlaneType *Sec     = &CurFramePlanes.Planes[1];
   DetectedPlaneType *P;

	 if(Prime->PlaneId != Primary->Id)
	 {
	    P = &CurFramePlanes.Planes[1];
			int Index = 1;
			while(Index < CurFramePlanes.NumPlanes)
			{
				if(P->PlaneId == Primary->Id)
				{
					DetectedPlaneType Temp;
          CopyPlaneEntry(Prime,&Temp);
          CopyPlaneEntry(P,Prime);
          CopyPlaneEntry(&Temp,P);
					break;
				}
			  P++;
			  Index++;
			}
	 }

	 if(Sec->PlaneId != Primary->Id)
	 {
	    P = &CurFramePlanes.Planes[2];
			int Index = 2;
			while(Index < CurFramePlanes.NumPlanes)
			{
				if(P->PlaneId == Secondary->Id)
				{
					DetectedPlaneType Temp;
          CopyPlaneEntry(Sec,&Temp);
          CopyPlaneEntry(P,Sec);
          CopyPlaneEntry(&Temp,P);
					break;
				}
			  P++;
			  Index++;
			}
	 }
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TrackCaptBarsTWS()
{
	float RelAngle =  -((CaptBars.Pos.X - AA_RADAR_CENTER_X)/(AA_RADAR_HALF_WIDTH))*70.0;

	if(Av.AARadar.AzimLimit < 70.0)
	{
		if( (RelAngle + Av.AARadar.AzimLimit <= 70.0) && (RelAngle - Av.AARadar.AzimLimit >= -70.0) )
		{
			int Diff = (int)RelAngle - Av.AARadar.SlewLeftRight;
			Av.AARadar.AntAzimuth += Diff;
			Av.AARadar.SlewLeftRight = RelAngle;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TrackTarget(float addto)
{
	DetectedPlaneType *P = &PrevFramePlanes.Planes[PrevFramePlanes.NumPlanes];

	while(P-- > PrevFramePlanes.Planes)
	{
		if(P->PlaneId == Primary->Id)
		{
			if( (P->RelAngle + addto <= 70.0) && (P->RelAngle - addto >= -70.0) )
			{
				Av.AARadar.AntAzimuth += (int)((int)P->RelAngle - Av.AARadar.SlewLeftRight);
				Av.AARadar.SlewLeftRight = P->RelAngle;
			}

			if( (P->RelPitch + Av.AARadar.ElevLimit <= 70.0) && (P->RelPitch - Av.AARadar.ElevLimit >= -70.0) )
				 Av.AARadar.SlewUpDown = P->RelPitch;

			break;
 		}
	}
}

void TrackPrimeTargetTWS()
{
	TrackTarget(Av.AARadar.AzimLimit);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

#define RAID_AZIM_EXTENT 2.0   // 1/2 FULL AZIM

void TrackPrimeTargetRAID()
{
	TrackTarget(RAID_AZIM_EXTENT);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetElevEdges()
{
	Av.AARadar.UpElevEdge   = Av.AARadar.SlewUpDown + Av.AARadar.ElevLimit;
  Av.AARadar.DownElevEdge = Av.AARadar.SlewUpDown - Av.AARadar.ElevLimit;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetAzimEdges()
{
	Av.AARadar.LeftAzimEdge   = Av.AARadar.SlewLeftRight + Av.AARadar.AzimLimit;
  Av.AARadar.RightAzimEdge  = Av.AARadar.SlewLeftRight - Av.AARadar.AzimLimit;
}

//*****************************************************************************************************************************************
// CAPT BARS SIM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int GetJoystickMapDirection()
{
  float Dir = GetJoystickDirection();

	if( (Dir <= 45) || (Dir > 315) )
		return(TOP);
	else if( (Dir > 45) && (Dir <= 135) )
		return(LEFT);
	else if( (Dir > 135) && (Dir <= 225) )
		return(BOTTOM);
	else
		return(RIGHT);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

#define MAX_CAPT_BAR_RATE 38   // 38 PIXELS PER SEC

void SimCaptBarsForKeyboard()
{
	//if( !( (GetTDCMode() == RADAR_MODE) && (UFC.MasterMode != AG_MODE) ) ) return;

	if( (GetTDCMode() != RADAR_MODE) ) return;

	int PrevActive  = CaptBars.KeyActive;
	int Left,Right,Up,Down;

	Up    = GetVkStatus(TdcUpVk);
	Down  = GetVkStatus(TdcDownVk);
	Left  = GetVkStatus(TdcLeftVk);
	Right = GetVkStatus(TdcRightVk);

	CaptBars.KeyActive = CaptBars.KeyActive || Left || Right || Up || Down;

	if(!PrevActive && !CaptBars.KeyActive) return;

	if(CaptBars.KeyActive && !PrevActive)
	{
		CaptBars.Timer.Set(15.0,GetTickCount());
	}
	else if(CaptBars.KeyActive)
	{
		float TimeSinceLastFrame = CaptBars.Timer.TimeElapsed( GetTickCount() );

		CaptBars.Timer.Set(15.0, GetTickCount() );

		float Dx,Dy;

		Dx = Dy = 0;

		if(Left || Right)
		  Dx = TimeSinceLastFrame*( (Left) ? -(1.0*MAX_CAPT_BAR_RATE) : (1.0*MAX_CAPT_BAR_RATE) );
		else if(Up || Down)
		  Dy = TimeSinceLastFrame*( (Up)   ? -(1.0*MAX_CAPT_BAR_RATE) : (1.0*MAX_CAPT_BAR_RATE) );

		Dx = __max(Dx,-2.0f);
		Dx = __min(Dx,2.0f);

		Dy = __max(Dy,-2.0f);
		Dy = __min(Dy,2.0f);

		CaptBars.Pos.X += Dx;
		CaptBars.Pos.Y += Dy;
	}
	else
	{
		CaptBars.Timer.Disable();
 	}

	if(CaptBars.Pos.X - 5 < AA_RADAR_LEFT) CaptBars.Pos.X = AA_RADAR_LEFT + 5;
	if(CaptBars.Pos.X + 5 > AA_RADAR_RIGHT) CaptBars.Pos.X = AA_RADAR_RIGHT - 5;

	if (UFC.MasterMode == AG_MODE)
	{
		if(CaptBars.Pos.Y - 5 < AG_RADAR_TOP) CaptBars.Pos.Y = AG_RADAR_TOP + 5;
		if(CaptBars.Pos.Y + 5 > AG_RADAR_BOTTOM) CaptBars.Pos.Y = AG_RADAR_BOTTOM - 5;
	}
	else
	{
		if(CaptBars.Pos.Y - 5 < AA_RADAR_TOP) CaptBars.Pos.Y = AA_RADAR_TOP + 5;
		if(CaptBars.Pos.Y + 5 > AA_RADAR_BOTTOM) CaptBars.Pos.Y = AA_RADAR_BOTTOM - 5;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int SimCaptBars()
{
  //if( !( (GetTDCMode() == RADAR_MODE) && (UFC.MasterMode != AG_MODE) ) ) return(0);

  if( (GetTDCMode() != RADAR_MODE) ) return(0);

	int PrevActive  = CaptBars.Active;
	CaptBars.Active = GetVkStatus(CaptBarsVk);

	if(!PrevActive && !CaptBars.Active) return(0);

	if(CaptBars.Active && !PrevActive)
	{
		CaptBars.Timer.Set(15.0,GetTickCount());
	}
	else if(CaptBars.Active)
	{
		int IsLeft,IsUp;
		float HorzPercent,VertPercent;
		float TimeSinceLastFrame = CaptBars.Timer.TimeElapsed( GetTickCount() );

		CaptBars.Timer.Set(15.0, GetTickCount() );

		GetJoyStickExtentsInPercent(&IsLeft,&IsUp,&HorzPercent,&VertPercent);

		float Dx,Dy;

		Dx = TimeSinceLastFrame*( (IsLeft) ? -(HorzPercent*MAX_CAPT_BAR_RATE) : (HorzPercent*MAX_CAPT_BAR_RATE) );
		Dy = TimeSinceLastFrame*( (IsUp)   ? -(VertPercent*MAX_CAPT_BAR_RATE) : (VertPercent*MAX_CAPT_BAR_RATE) );


		Dx = __max(Dx,-2.0f);
		Dx = __min(Dx,2.0f);

		Dy = __max(Dy,-2.0f);
		Dy = __min(Dy,2.0f);


		CaptBars.Pos.X += Dx;
		CaptBars.Pos.Y += Dy;
	}
	else
	{
		return(0);
		CaptBars.Timer.Disable();
 	}

	if(CaptBars.Pos.X - 5 < AA_RADAR_LEFT) CaptBars.Pos.X = AA_RADAR_LEFT + 5;
	if(CaptBars.Pos.X + 5 > AA_RADAR_RIGHT) CaptBars.Pos.X = AA_RADAR_RIGHT - 5;

	if (UFC.MasterMode == AG_MODE)
	{
		if(CaptBars.Pos.Y - 5 < AG_RADAR_TOP) CaptBars.Pos.Y = AG_RADAR_TOP + 5;
		if(CaptBars.Pos.Y + 5 > AG_RADAR_BOTTOM) CaptBars.Pos.Y = AG_RADAR_BOTTOM - 5;
	}
	else
	{
		if(CaptBars.Pos.Y - 5 < AA_RADAR_TOP) CaptBars.Pos.Y = AA_RADAR_TOP + 5;
		if(CaptBars.Pos.Y + 5 > AA_RADAR_BOTTOM) CaptBars.Pos.Y = AA_RADAR_BOTTOM - 5;
	}

	return(1);
}


//*****************************************************************************************************************************************
// UTILS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

float GetAngularCoverage(int Range, int Bars)
{
   float Angle;

   switch (Bars)
   {
     case 1:
	          Angle = 0.0;
	          break;
     case 2:
	          if(Range > 20)
		 				  Angle = 3.5;
	   				else if (Range > 10)
		 				  Angle = 5.0;
	   				else
							Angle = 5.9;
	   				break;
     case 4:
	   				if(Range > 20)
		 				  Angle = 5.5;
	   				else if (Range > 10)
		 				  Angle = 10.0;
	   				else
							Angle = 12.7;
	   				break;
     case 6:
	   				if(Range > 20)
		 				  Angle = 7.5;
	   				else if (Range > 10)
		 				  Angle = 15.0;
	   				else
							Angle = 19.5;
	          break;
	   case 8:
	   				if(Range > 20)
		 				  Angle = 10.5;
	   				else if (Range > 10)
		 				  Angle = 20.0;
	   				else Angle = 25.5;
	   				break;
   }

   return(Angle);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float CalcRelativeVelocity(int TargetId)
{
  if(TargetId == -1)
	  return(0.0);

  return ( AAClosureRate( PlayerPlane,  &Planes[TargetId]) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetPlanes()
{
	ZeroMemory(&CurFramePlanes,sizeof(CurFramePlanes));
	ZeroMemory(&TempCurFramePlanes,sizeof(TempCurFramePlanes));
}

//*****************************************************************************************************************************************
// SCAN FUNCTS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void UpdateRadarScan(FPointDouble *ScanEnd)
{
  float Yaw,Pitch;

  RadarLastFrameSweepAngle = 0;

  if(Av.AARadar.FirstTime)
	{
		Av.AARadar.LstTicks = Av.AARadar.ThisTicks = GameLoopInTicks;
		Av.AARadar.FirstTime = 0;
	}else{

		Av.AARadar.ThisTicks = GameLoopInTicks;
	}

	Av.AARadar.DiffCount += (Av.AARadar.ThisTicks - Av.AARadar.LstTicks);
	Av.AARadar.LstTicks = Av.AARadar.ThisTicks;

	while(Av.AARadar.DiffCount > Av.AARadar.ScanRate) // scan rate is (1 deg/0.01428 sec)
	{
		RadarLastFrameSweepAngle++;
		Av.AARadar.DiffCount -= Av.AARadar.ScanRate;

		if ((Av.AARadar.CurMode == AA_ACQ_MODE) && (Av.AARadar.AcqMode == AA_VERTICAL_ACQ_MODE))
		{
			Av.AARadar.AntElevation += (Av.AARadar.ScanDirection == LTOR) ? -1.0 : 1.0;

  			if(Av.AARadar.AntElevation >= Av.AARadar.UpElevEdge)
			{
				Av.AARadar.ScanDirection = LTOR;
				Av.AARadar.Prf ^= 1;
				Av.AARadar.AntElevation = Av.AARadar.UpElevEdge;
				Av.AARadar.AntAzimuth -= Av.AARadar.ElevStep;
				Av.AARadar.BarCount--;
			}
			else if(Av.AARadar.AntElevation <= Av.AARadar.DownElevEdge)
			{
				Av.AARadar.ScanDirection = RTOL;
				Av.AARadar.Prf ^= 1;
				Av.AARadar.AntElevation = Av.AARadar.DownElevEdge;
				Av.AARadar.AntAzimuth -= Av.AARadar.ElevStep;
				Av.AARadar.BarCount--;
			}

			if (Av.AARadar.BarCount < 0)
			{
		  		Av.AARadar.BarCount = Av.AARadar.Bars;
				Av.AARadar.AntAzimuth = Av.AARadar.LeftAzimEdge;
				Av.AARadar.PrfFrameStart	^= 1;
				Av.AARadar.Prf = Av.AARadar.PrfFrameStart;

		   		ScanStart = TRUE;
				RadarFrame++;
			}
		}
		else
		{
			Av.AARadar.AntAzimuth += (Av.AARadar.ScanDirection == LTOR) ? -1.0 : 1.0;

  			if(Av.AARadar.AntAzimuth >= Av.AARadar.LeftAzimEdge)
			{
				Av.AARadar.ScanDirection = LTOR;
				Av.AARadar.Prf ^= 1;
				Av.AARadar.AntElevation -= Av.AARadar.ElevStep;
				Av.AARadar.AntAzimuth = Av.AARadar.LeftAzimEdge;
				Av.AARadar.BarCount--;
			}
			else if(Av.AARadar.AntAzimuth <= Av.AARadar.RightAzimEdge)
			{
				Av.AARadar.ScanDirection = RTOL;
				Av.AARadar.Prf ^= 1;
				Av.AARadar.AntElevation -= Av.AARadar.ElevStep;
				Av.AARadar.AntAzimuth = Av.AARadar.RightAzimEdge;
				Av.AARadar.BarCount--;
			}

			if (Av.AARadar.BarCount < 0)
			{
		  		Av.AARadar.BarCount = Av.AARadar.Bars;
				Av.AARadar.AntElevation = Av.AARadar.UpElevEdge;
				Av.AARadar.PrfFrameStart	^= 1;
				Av.AARadar.Prf = Av.AARadar.PrfFrameStart;

		   		ScanStart = TRUE;
				RadarFrame++;
			}
		}
  }

  if(Av.AARadar.SearchPrf == SEARCH_M_MODE) Av.AARadar.Prf = MPRF;
  else if (Av.AARadar.SearchPrf == SEARCH_H_MODE) Av.AARadar.Prf = HPRF;

	FMatrix Mat;

  	if (Av.AARadar.CurMode == AA_ACQ_MODE)
  	{
  		Yaw   = DEGREES_TO_RADIANS * (Av.AARadar.SlewLeftRight + Av.AARadar.AntAzimuth);
  		Pitch = DEGREES_TO_RADIANS * (Av.AARadar.AntElevation + Av.AARadar.SlewUpDown);
		Mat.SetRadRPH(0,Pitch,Yaw);
		Mat *= PlayerPlane->Attitude;
  	}
  	else
	{
  		Yaw   = DEGREES_TO_RADIANS * (fANGLE_TO_DEGREES(PlayerPlane->Heading) + Av.AARadar.SlewLeftRight + Av.AARadar.AntAzimuth);
  		Pitch = DEGREES_TO_RADIANS * (Av.AARadar.AntElevation + Av.AARadar.SlewUpDown);
		Mat.SetRadRPH(0,Pitch,Yaw);
	}

	float Dist = -(Av.AARadar.Range*NMTOWU);

//#ifdef _DEBUG
//	SetDebugCameraLine(PlayerPlane->WorldPosition,Mat);
//#endif

	Mat.GetZNormal(*ScanEnd);
	*ScanEnd *= Dist;

//	ScanEnd->SetValues(Dist*Mat.m_Data.RC.R0C2,Dist*Mat.m_Data.RC.R1C2,Dist*Mat.m_Data.RC.R2C2);

	*ScanEnd += PlayerPlane->WorldPosition;
}

//*****************************************************************************************************************************************

typedef enum
{
	SA_NOTHING = 0,
	SA_LIMIT,
	SA_MASKED,
} SLEW_ACTIONS;

float RLAngle(float angle)
{
	angle = NormDegree(angle);

	if(angle > 180)
		angle -= 360;
	else
		if(angle < -180)
			angle += 360;

	return angle;
}


double ComputePitchFromFPoint(FPoint &pos)
{
	float offangle;
	float Distance;

	Distance = sqrt(pos.X*pos.X + pos.Z*pos.Z);

	offangle = atan2(pos.Y, Distance) * RADIANS_TO_DEGREES;

	while(offangle > 180)
		offangle -= 360;

	while(offangle < -180)
		offangle += 360;

	if(offangle > 90.0)
	  offangle = 180 - offangle;

	if(offangle < -90.0)
	  offangle = -(180 + offangle);

  	return((double) offangle);

}

 float GetRelPitch(FMatrix mat,float azim,float elev)
 {
	FMatrix new_mat;
	FPoint new_vec;

	new_mat.SetRadRPH(0,DEGREES_TO_RADIANS * elev,DEGREES_TO_RADIANS * azim);
	new_mat.GetZNormal(new_vec);
	new_vec *= -1.0f;

	mat.Transpose();
	new_vec *= mat;

	return ComputePitchFromFPoint(new_vec);
 }

SLEW_ACTIONS CheckAnglesWithinBounds(FMatrix parent_mat,float azim,float elev,float max_azim,float max_elev,float min_azim,float min_elev,float *new_azim,float *new_elev)
{
	FPoint new_vec;
	float rel_azim,rel_elev;
	FMatrix new_mat;
	SLEW_ACTIONS ret_val = SA_NOTHING;

	new_mat.SetRadRPH(0,DEGREES_TO_RADIANS * elev,DEGREES_TO_RADIANS * azim);
	new_mat.GetZNormal(new_vec);
	new_vec *= -1.0f;

	parent_mat.Transpose();
	new_vec *= parent_mat;

	rel_azim = RLAngle(atan2(-new_vec.X, -new_vec.Z) * RADIANS_TO_DEGREES);

	rel_elev = ComputePitchFromFPoint(new_vec);

	//Now we have the pitch and heading relative to the camera position
	//figure out if they're in bounds

	if (rel_elev < min_elev)
	{
		ret_val = SA_LIMIT;
		rel_elev = min_elev;
	}
	else
		if (rel_elev > max_elev)
		{
			ret_val = SA_LIMIT;
			rel_elev = max_elev;
		}

	if (rel_azim < min_azim)
	{
		ret_val = SA_LIMIT;
		rel_azim = min_azim;
	}
	else
		if (rel_azim > max_azim)
		{
			ret_val = SA_LIMIT;
			rel_azim = max_azim;
		}

	// if not, clip them to the current bounds and recast that in
	// world coordinates

	if (ret_val && (new_azim || new_elev))
	{
		new_mat.SetRadRPH(0,rel_elev*DEGREES_TO_RADIANS,rel_azim*DEGREES_TO_RADIANS);
		new_mat.GetZNormal(new_vec);
		new_vec *= -1.0f;

		parent_mat.Transpose();
		new_vec *= parent_mat;

		if (new_azim)
			*new_azim = RLAngle(atan2(-new_vec.X, -new_vec.Z) * RADIANS_TO_DEGREES);

		if (new_elev)
			*new_elev = ComputePitchFromFPoint(new_vec);
	}
	else
	{
		if (new_azim)
			*new_azim = azim;

		if (new_elev)
			*new_elev = elev;
	}

	return ret_val;

}

//*****************************************************************************************************************************************

BOOL UpdateSTTScan(int *BrokeLock)
{
	float DistToEnemy;
	float YFrac;
	float XFrac;
	float XPos,YPos;
	int   PlaneIndex;
	float   CrossSignature;
	float   CrossSignature1;
	PlaneParams *Target;
	float DeltaAzim, DeltaElev;
	DetectedPlaneType *ptest;
	BOOL break_lock = TRUE;
	BOOL jammer_hot = FALSE;
	BOOL dist_ok;
	BOOL check_timer = FALSE;
	BOOL go_with_aot = TRUE;
	BOOL passes_critical_aspect;
	BOOL passes_distance;
	BOOL passes_cross_section;
	BOOL lock_broken = FALSE;
	FPointDouble PlayerToEnemy;

//	if(!InterleaveTimer.IsActive())
//		InterleaveTimer.Set(1.0,GameLoopInTicks);

//	if( InterleaveTimer.TimeUp(GameLoopInTicks) )
//	{
//		Av.AARadar.Prf ^= 1;
//		InterleaveTimer.Set(1.0,GameLoopInTicks);
//	}

	CurFramePlanes.NumPlanes = 0;

	PlaneIndex = Primary->Id;
	if(PlaneIndex != -1)
	{
		Target = &Planes[PlaneIndex];
		if (!(Target->FlightStatus & PL_STATUS_CRASHED))
		{
  			if(Av.AARadar.SttJamming)
			{
				if(!Primary->Jammer.TargetJamming)
				{
					Primary->Jammer.TargetJamming = TRUE;
					Primary->Jammer.JammerTimer.Set(3.0,GameLoopInTicks);
				}
			}
			else
			{
		 		if(Primary->Jammer.TargetJamming)
		 		{
					Primary->Jammer.TargetJamming = FALSE;
					Primary->Jammer.JammerTimer.Disable();
		 		}
			}

			if (Target->AI.iAIFlags2 & AI_SO_JAMMING)
				jammer_hot = TRUE;
			else
				if(AVIsNoiseJamming(Target))  // ((Target->AI.iAIFlags1 & AIJAMMINGON) && (Target->AI.iAICombatFlags2 & AI_NOISE_JAMMER))
				{
					if (STTBreakLockTimer.IsActive() && STTBreakLockTimer.TimeUp(GameLoopInTicks))
					{
						lock_broken = TRUE;
						jammer_hot = TRUE;
					}
					else
						jammer_hot = FALSE;
				}
				else
					jammer_hot = FALSE;

//			jammer_hot = ( ((Target->AI.iAIFlags1 & AIJAMMINGON) && (Target->AI.iAICombatFlags2 & AI_NOISE_JAMMER)) || (Target->AI.iAIFlags2 & AI_SO_JAMMING));

			if (!Av.AARadar.SilentOn && !UFC.EMISState)
				if (PlaneVisibleByCriticalAspectRegion(PlayerPlane,Target))
				{
					Primary->CriticalAspectTimer.Disable();
					passes_critical_aspect = TRUE;
				}
				else
				{
					if (!Primary->CriticalAspectTimer.IsActive())
					{
						Primary->CriticalAspectTimer.Set(10.0,GameLoopInTicks);
						passes_critical_aspect = TRUE;
					}
					else
						if( !Primary->CriticalAspectTimer.TimeUp(GameLoopInTicks) )
							passes_critical_aspect = TRUE;
						else
							passes_critical_aspect = FALSE;
				}
			else
			{
				Primary->CriticalAspectTimer.Disable();
				passes_critical_aspect = FALSE;
			}

			if (jammer_hot || passes_critical_aspect)
			{

				PlayerToEnemy.MakeVectorToFrom(Target->WorldPosition,PlayerPlane->WorldPosition);

				DistToEnemy = PlayerToEnemy.Normalize();

				passes_distance = (DistToEnemy <= 80.0f*NMTOWU);

				if (jammer_hot)
					dist_ok = (DistToEnemy < (100.0f * NMTOWU));
				else
					dist_ok = passes_distance;

				if (dist_ok)
				{
					float pphead = fANGLE_TO_DEGREES(PlayerPlane->Heading);

					DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Target->WorldPosition,TRUE);
					Primary->RelAngle = DeltaAzim;

					DeltaElev = -ComputePitchToPoint(PlayerPlane, Target->WorldPosition);

					if (!CheckAnglesWithinBounds(PlayerPlane->Attitude,RLAngle(pphead+DeltaAzim),DeltaElev,70,70,-70,-70,&Av.AARadar.AntAzimuth,&Av.AARadar.AntElevation))
					{
						float RelVel;
						float DeltaAlt;
						RelVel         = CalcRelativeVelocity(PlaneIndex);
						DeltaAlt       = (Target->WorldPosition.Y - PlayerPlane->WorldPosition.Y)*WUTOFT;

						Av.AARadar.AntAzimuth = RLAngle(Av.AARadar.AntAzimuth-pphead);

						CrossSignature = GetCrossSection(pDBAircraftList[Target->AI.iPlaneIndex].iRadarCrossSignature,Av.AARadar.Prf,DeltaAlt,RelVel,DistToEnemy*WUTONM,FALSE);
						CrossSignature1 = GetCrossSection(pDBAircraftList[Target->AI.iPlaneIndex].iRadarCrossSignature,Av.AARadar.Prf^1,DeltaAlt,RelVel,DistToEnemy*WUTONM,FALSE);

						if (CrossSignature1 > CrossSignature)
						{
							CrossSignature = CrossSignature1;
							Av.AARadar.Prf^=1;
						}

						if ((Target->Type->TypeNumber == 61) && MultiPlayer)
							CrossSignature += 0.5f;

						passes_cross_section = ((CrossSignature+(frand()*0.1)) >=  Av.AARadar.AOTDamping);

						if(jammer_hot || passes_cross_section)
						{
	        				FPointDouble Intersection;

	   						if(LOS(&(PlayerPlane->WorldPosition),&(Planes[PlaneIndex].WorldPosition), &Intersection, MED_RES_LOS, FALSE))
	   						{
								ptest = CurFramePlanes.Planes;
								CurFramePlanes.NumPlanes = 1;

								if (jammer_hot && passes_distance && passes_critical_aspect && passes_cross_section)
								{
									if (lock_broken)
									{
										float sttdist = (float)CrossSignature-1.0f;

										if (sttdist < 0.0f)
											sttdist = 0.0f;

										sttdist	*= 1.0f/8.0f;

										sttdist *= sttdist;
										sttdist *= 30.0f+30.0f*STTDistLargerThanThisWillBreakLock;
										sttdist += 7.0f*(1.0f+(STTDistLargerThanThisWillBreakLock * 0.5f));

										sttdist *= NMTOWU;

										if (DistToEnemy < sttdist)
											jammer_hot = FALSE;
									}
									else
										jammer_hot = FALSE;
								}

								if (!passes_cross_section)
									STTDistLargerThanThisWillBreakLock = 1000.0f; //make sure we won't be fooled at close range

								XFrac = (70.0f-Primary->RelAngle) * (1.0f / 140.0f);

								if (ptest->AOT = jammer_hot)
								{
					 				YPos = AA_RADAR_TOP - 5;
									ptest->ShouldDraw = TRUE;
								}
								else
								{

									/* --------------11/1/99 8:51PM------------------------------------------------------------
									/* do auto range calculation.
									/* ----------------------------------------------------------------------------------------*/
									Av.AARadar.Range = (DistToEnemy * WUTONM);

									if (Av.AARadar.Range < 3.5f)
										Av.AARadar.Range = 5.0f;
									else
										if (Av.AARadar.Range < 8.5f)
											Av.AARadar.Range = 10.0f;
										else
											if (Av.AARadar.Range < 17.0f)
												Av.AARadar.Range = 20.0f;
											else
												if (Av.AARadar.Range < 35.0f)
													Av.AARadar.Range = 40.0f;
												else
													Av.AARadar.Range = 80.0f;

		  		   					YFrac = (DistToEnemy*WUTONM)/Av.AARadar.Range;

  									ptest->ShouldDraw = ((YFrac <= 1.0) && (YFrac >= 0.0));

									YFrac = __max(0.0f,YFrac);
									YFrac = __min(1.0f,YFrac);
									YPos = LERP(AA_RADAR_BOTTOM,AA_RADAR_TOP,YFrac);

									// auto range

								}

  								ptest->ShouldDraw &= ((XFrac <= 1.0) && (XFrac >= 0.0));

								XFrac = __max(0.0f,XFrac);
								XFrac = __min(1.0f,XFrac);

	  							XPos = LERP(AA_RADAR_LEFT,AA_RADAR_RIGHT, XFrac);

								if( (Primary->Jammer.TargetJamming) && (!Primary->Jammer.JammerFakeOut) )
								{
		 							if( Primary->Jammer.JammerTimer.TimeUp(GameLoopInTicks) )
		 							{
										Primary->Jammer.JammerFakeOut   = TRUE;
		          						float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Target->WorldPosition,TRUE);
										Primary->Jammer.JammerDirection = ( (DeltaAzim > 90.0) || (DeltaAzim < -90) ) ? UP : DOWN;
										Primary->Jammer.JammerTimer.Set(5.0,GameLoopInTicks);
										Primary->Jammer.JammerYPos = Primary->YOfs;
										Primary->Jammer.JammerXPos = Primary->XOfs;
										Primary->Jammer.Heading = Target->Heading/DEGREE;
		 							}
								}

								if(Primary->Jammer.JammerFakeOut)
								{
									float TimeElapsed = Primary->Jammer.JammerTimer.TimeElapsed(GameLoopInTicks);
									float Frac = TimeElapsed * (1.0/3.0);
									if(Frac > 1.0) Frac = 1.0;

									XPos = Primary->Jammer.JammerYPos + (20*(Frac)*((Primary->Jammer.JammerDirection == UP) ? -1.0 : 1.0));
									YPos = Primary->Jammer.JammerXPos;

									if(! Primary->Jammer.JammerTimer.TimeUp(GameLoopInTicks) )
										break_lock = TRUE;
									else
									{
//		  								Av.AARadar.CurMode = AA_RWS_MODE;
//		  								ResetAARadar( AA_RWS_MODE);
//		  								ResetPrimary();
									}

									ptest->ShouldDraw = ( (YPos > AA_RADAR_TOP) && (YPos < AA_RADAR_BOTTOM) ) && ((XPos > AA_RADAR_LEFT) & (XPos < AA_RADAR_RIGHT));

									ptest->AOT = FALSE;

								}
								else
									break_lock = FALSE;

								Primary->ShouldDraw = ptest->ShouldDraw;

								Primary->Id 		 = ptest->PlaneId       = PlaneIndex;
								Primary->XOfs 		 = ptest->OfsX          = XPos;
								Primary->YOfs 		 = ptest->OfsY          = YPos;
								ptest->Designation   = AA_ON_RADAR;
//								ptest->IDLevel       = AA_UNKNOWN;
								ptest->RelAngle      = Primary->RelAngle;

								ptest->RelPitch      = -ComputePitchToPoint(PlayerPlane,Target->WorldPosition);

								ptest->Heading       = Target->Heading/DEGREE;
								ptest->Color         = AV_GREEN;
								ptest->Shade         = 1.0;
								ptest->Loc           = Target->WorldPosition;
//								ptest->AwacsReported = FALSE;
								ptest->RadarFrame    = RadarFrame;

							} // Los maintained
						}

					}
					else
						Av.AARadar.AntAzimuth = RLAngle(Av.AARadar.AntAzimuth-pphead);
				}
			}
		}
	}


	*BrokeLock = break_lock;


	return jammer_hot;
}


//*****************************************************************************************************************************************
// PLANE LIST MANAGE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
int IsPlaneInList(DetectedPlaneListType *PList,int Id)
{
	DetectedPlaneType *p = &PList->Planes[PList->NumPlanes];

	while(p-- > PList->Planes)
		if (p->PlaneId == Id)
			return TRUE;

	return FALSE;
}

DetectedPlaneType *GetPlanePtrInList(DetectedPlaneListType *PList,int Id)
{
	DetectedPlaneType *p = &PList->Planes[PList->NumPlanes];

	while(p-- > PList->Planes)
		if (p->PlaneId == Id)
			return p;

	return NULL;
}


//*****************************************************************************************************************************************
int GetPlanePosInList(DetectedPlaneListType *PList,int Id)
{

	int c = PList->NumPlanes;
	DetectedPlaneType *p = &PList->Planes[c];

	while(c--)
	{
		p--;
		if (p->PlaneId == Id)
			break;
	}

	return c;
}

//*****************************************************************************************************************************************
int GetPlaneRankPosInList(DetectedPlaneListType *PList,int Rank)
{
	int c = PList->NumPlanes;
	DetectedPlaneType *p = &PList->Planes[c];

	while(c--)
	{
		p--;
		if (p->Rank == Rank)
			break;
	}

	return c;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CopyPlaneEntry(DetectedPlaneType *From, DetectedPlaneType *To)
{
	memcpy(To,From,sizeof(DetectedPlaneType));
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddPlaneToList (DetectedPlaneListType *PList, DetectedPlaneType *TempPlaneInfo)
{
	if(PList->NumPlanes + 1 < AA_MAX_DET_PLANES)
	{
	  PList->NumPlanes++;
	  CopyPlaneEntry(TempPlaneInfo, &(PList->Planes[PList->NumPlanes-1]));
	}
}

void StickPlaneInList(DetectedPlaneListType *PList,DetectedPlaneType *plane)
{
	DetectedPlaneType *p = &PList->Planes[PList->NumPlanes];

	while(p-- > PList->Planes)
		if (p->PlaneId == plane->PlaneId)
		{
			memcpy(p,plane,sizeof(DetectedPlaneType));
			return;
		}

	if(PList->NumPlanes < (AA_MAX_DET_PLANES-1))
		memcpy(&PList->Planes[PList->NumPlanes++],plane,sizeof(DetectedPlaneType));

}

void RefreshList(DetectedPlaneListType *PList,DWORD time)
{
	DetectedPlaneType *p = &PList->Planes[PList->NumPlanes];
	DetectedPlaneType *list_end = p-1;
	BOOL at_end = TRUE;

	while(p-- > PList->Planes)
	{
		if ((RadarFrame - p->RadarFrame) > time)
		{
			if (!at_end)
				memcpy(p,list_end,sizeof(DetectedPlaneType));
			list_end--;

			PList->NumPlanes--;
		}
		else
			at_end = FALSE;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RemovePlaneFromList(DetectedPlaneListType *PList, int Id)
{
	DetectedPlaneType *p = &PList->Planes[PList->NumPlanes];

	while(p-- > PList->Planes)
	if (p->PlaneId == Id)
	{
		if (PList->NumPlanes-- > 1)
		  	CopyPlaneEntry(&(PList->Planes[PList->NumPlanes]),p);
		break;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CopyPlaneList(DetectedPlaneListType *From, DetectedPlaneListType *To)
{
  To->NumPlanes = From->NumPlanes;
  To->ColorFadePercent = From->ColorFadePercent;
  To->FadeTimer = From->FadeTimer;
  for(int i=0; i<From->NumPlanes; i++)
	 CopyPlaneEntry(&(From->Planes[i]),&(To->Planes[i]));
}

//*****************************************************************************************************************************************
// TARGET DETECT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define RANGE_RESOLVE_DIST 750.0

void FilterForRangeResolution()
{
	float MaxRange = 80.0;  // JLM until database is up
	FPointDouble P1,P2;
	int NumToRemove = 0;
	int RemoveList[AA_MAX_DET_PLANES];

	for(int i=0; i<CurFramePlanes.NumPlanes-1; i++)
	{
		for(int j=i+1; j<CurFramePlanes.NumPlanes; j++)
		{
			if(!(CurFramePlanes.Planes[i].Designation & AA_AWACS))    // AWACS PLANES ARE NOT FILTERED
			{
					P1 = Planes[CurFramePlanes.Planes[i].PlaneId].WorldPosition;
					P2 = Planes[CurFramePlanes.Planes[j].PlaneId].WorldPosition;

					float Dist = P1 / P2;
					Dist *= WUTONM;

					float DistInFt = Dist*NMTOFT;

					float DistFromPlayer = PlayerPlane->WorldPosition / P1;
					DistFromPlayer *=  WUTONM;

					if(DistFromPlayer > MaxRange)
						DistFromPlayer = MaxRange;

					float FilterDist = (DistFromPlayer/MaxRange)*RANGE_RESOLVE_DIST;

					if(Dist*NMTOFT < FilterDist)
					{
						// make sure item is not in the list
						int InList=FALSE;
						int *R = &RemoveList[0];
						int Index = NumToRemove;
						while(Index-- > 0)
						{
							if(*R == CurFramePlanes.Planes[j].PlaneId)
							{
								InList = TRUE;
								break;
							}
							R++;
						}
						// add it to the list if it isn't already there
						if(!InList)
						{
					  	NumToRemove++;
					  	RemoveList[NumToRemove-1] = CurFramePlanes.Planes[j].PlaneId;
						}
					}

			} // IF NOT AWACS
		}
	}

	// now go through and remove all planes that are in the remove list;
	int *Ptr = &RemoveList[0];
	int Index = NumToRemove;
	while(Index-- > 0)
	{
		RemovePlaneFromList(&CurFramePlanes,*Ptr);
		Ptr++;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

// jlm debug for Chris to test aa radar
int DebugCrossSig;
int DebugPrf;
int DebugDeltaAlt;
int DebugRelVel;
float DebugTargetDist;
int DebugFinalCrossSig;
int DebugBase;
float DebugRangeAdj, DebugVelAdj, DebugAltAdj, DebugAvgAdj, DebugAdditive;

void PrintCrossSectionInfo()
{
  //TargetDist *= FTTONM;

	AvRGB Triple;
  GetRGBFromAVPalette(AV_RED,1.0,&Triple);

  sprintf(TmpStr,"CrossSig  %d",DebugCrossSig);
  DrawTextAtLocation(20,20,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"Prf  %s",(DebugPrf == HPRF) ? "Hi" : ((DebugPrf == MPRF) ? "Med" : "Low") );
  DrawTextAtLocation(20,30,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"DeltaAlt  %d",(int)DebugDeltaAlt);
  DrawTextAtLocation(20,40,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"RelVel  %d",(int)DebugRelVel);
  DrawTextAtLocation(20,50,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"DistInMiles  %5.2f",(DebugTargetDist));
  DrawTextAtLocation(20,60,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"FinalAdj  %5.2f",(DebugAvgAdj));
  DrawTextAtLocation(20,70,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"RangeAdj  %5.2f",(DebugRangeAdj));
  DrawTextAtLocation(20,80,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"VelocityAdj  %5.2f",(DebugVelAdj));
  DrawTextAtLocation(20,90,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"AltitudeAdj  %5.2f",(DebugAltAdj));
  DrawTextAtLocation(20,100,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"Additive  %d",(int)(DebugAdditive));
  DrawTextAtLocation(20,110,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"Base  %d",(int)(DebugBase));
  DrawTextAtLocation(20,120,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

  sprintf(TmpStr,"FinalCrossSig  %d",(int)(DebugFinalCrossSig));
  DrawTextAtLocation(20,130,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);

}
//*****************************************************************************************************************************************
float GetCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange)
{
	float Alt,Velocity, Distance;
	float RangeAdj, VelAdj, AltAdj, AvgAdj,MaxRange;

	if(orgMaxRange > 0)
	{
		MaxRange = orgMaxRange;
	}
	else
	{
		MaxRange = 80.0;   // JLM get max range from database
	}

	Alt      = DeltaAlt;
	Velocity = fabs(RelVel);
	Distance = TargetDist;

	if(Prf == HPRF)
	{
		if(Velocity > 2250.0)
			Velocity = 2250.0;
		VelAdj = (Velocity/2250.0)*3.5;

		//jlm
		DebugVelAdj = VelAdj;
	} else {
		if(Distance > MaxRange)
			Distance = MaxRange;
//		float Modifier = (Prf == MPRF) ? 0.75 : 0.50;
		float Modifier = (Prf == MPRF) ? 0.75 : 1.00;
		float ModifiedMaxRange = MaxRange * Modifier;
		RangeAdj = ((ModifiedMaxRange - Distance)/ModifiedMaxRange)*2.0;

		// jlm
		DebugRangeAdj = RangeAdj;
	}

  if(Alt > 20000)
	  Alt = 20000;
  else if(Alt < -20000) Alt = -20000;

	Alt += 20000;
	AltAdj   = (Alt/40000)*((Prf == LPRF) ? 3.0 : 2.0);

	// jlm
	DebugAltAdj = AltAdj;

  if(Prf == HPRF)
    AvgAdj = (VelAdj + AltAdj)/2.0;
  else
    AvgAdj = (RangeAdj + AltAdj)/2.0;

	DebugAvgAdj = AvgAdj;

  float Additive = 0;
  if(TargetDist <= MaxRange*0.1)
  {
    if( (TargetDist > MaxRange*0.05))
	    Additive = 1;
	  else if( (TargetDist > MaxRange*0.01) && (TargetDist <= MaxRange*0.05) )
	   Additive = 2;
	  else
	   Additive = 3;
  }
  else if(TargetDist >= MaxRange*0.65)
  {
    if( (TargetDist >= MaxRange*0.91))
	    Additive = -3;
	  else if(TargetDist >= MaxRange*0.81)
	    Additive = -2;
	  else
	   Additive = -1;
  }

	// jlm
	DebugAdditive = Additive;

  int FinalCrossSignature = /*round(*/ (AvgAdj*(float)CrossSignature) + Additive /*)*/;

  return(FinalCrossSignature);
}


//*****************************************************************************************************************************************
int IsCrossSectionValid(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, int PlaneIndex)
{
	return(GetCrossSection(CrossSignature,Prf,DeltaAlt,RelVel,TargetDist,UseSTT)+(0.1*frand()) >= Av.AARadar.AOTDamping);
}



//*****************************************************************************************************************************************
#if 1
#define CRITICAL_ASPECT_THRESHOLD_MPRF 0.033       // 3 degrees on each side = 3/90
#define CRITICAL_ASPECT_THRESHOLD_HPRF 0.044      // 4 degrees on each side = 4/90

int PlaneVisibleByCriticalAspectRegion(PlaneParams *P, PlaneParams *Target)
{

	FPointDouble FromPlaneToTarget;

	if (PlayerPlane->WorldPosition.Y <= Target->WorldPosition.Y) return (TRUE);

	FromPlaneToTarget.MakeVectorToFrom(Target->WorldPosition,PlayerPlane->WorldPosition);

	FromPlaneToTarget.Normalize();	//  Added so vector length is 1

  FPointDouble ProjPoint;
//	ProjPoint.SetValues(-2000.0*Target->Attitude.m_Data.RC.R0C2,-2000.0*Target->Attitude.m_Data.RC.R1C2,
//							                     -2000.0*Target->Attitude.m_Data.RC.R2C2);

	Target->Attitude.GetZNormal(ProjPoint);
//	ProjPoint.SetValues(Target->Attitude.m_Data.RC.R0C2,Target->Attitude.m_Data.RC.R1C2,
//							                     Target->Attitude.m_Data.RC.R2C2);

	float Dot = FromPlaneToTarget * ProjPoint;

	float Threshold = (Av.AARadar.Prf == HPRF) ? CRITICAL_ASPECT_THRESHOLD_HPRF : CRITICAL_ASPECT_THRESHOLD_MPRF;

	return( fabs(Dot) > Threshold );

}
#else
#define CRITICAL_ASPECT_THRESHOLD_MPRF 0.033       // 3 degrees on each side = 3/90
#define CRITICAL_ASPECT_THRESHOLD_HPRF 0.044      // 4 degrees on each side = 4/90

int PlaneVisibleByCriticalAspectRegion(PlaneParams *P, PlaneParams *Target)
{
	if(Target->WorldPosition.Y >= P->WorldPosition.Y) return(TRUE);

	FPointDouble FromPlaneToTarget;

	FromPlaneToTarget.MakeVectorToFrom(Target->WorldPosition,PlayerPlane->WorldPosition);

  FPointDouble ProjPoint;
	ProjPoint.SetValues(-2000.0*Target->Attitude.m_Data.RC.R0C2,-2000.0*Target->Attitude.m_Data.RC.R1C2,
							                     -2000.0*Target->Attitude.m_Data.RC.R2C2);

	float Dot = FromPlaneToTarget * ProjPoint;

	float Threshold = (Av.AARadar.Prf == HPRF) ? CRITICAL_ASPECT_THRESHOLD_HPRF : CRITICAL_ASPECT_THRESHOLD_MPRF;

	return( fabs(Dot) > Threshold );

}
#endif

float fZoomAngle=0.50f;
float RaidLeftRel;
float RaidNear;

//*****************************************************************************************************************************************
void FilterPlanesFronCone(FPointDouble ScanEnd,int CalcVelocity,int KeepIDLevel)
{
	float DistToRay,DistToEnemy;
	float YFrac;
	float XFrac;
	float XPos,YPos;
	int	  Cnt;
	int   PlaneIndex;
	AvObjListEntryType *Plane;
	PlaneParams *Target;
	float cone_size;
	BOOL InField;
	float yaw,pitch;
	DetectedPlaneType *ptest;
	BOOL jammer_hot;
	BOOL dist_ok;
	BOOL passes_critical_aspect;
	BOOL passes_distance;
	BOOL passes_cross_section;

	if(UFC.EMISState || PlayerPlane->OnGround)
	{
		CurFramePlanes.NumPlanes = 0;
		return; // if emis is on, no emitting radar. objects should fade out
	}

	if (SimPause)
	{
		cone_size = 0.15f;
		RadarFrame++;
	}
	else
	{
		if (RadarLastFrameSweepAngle < 3)
			cone_size = 0.999328f;
		else
			cone_size = cos(DEGREES_TO_RADIANS * ((float)RadarLastFrameSweepAngle)*0.8);
	}

	if ((Av.AARadar.RaidOn) && (Primary->Id != -1))
	{
		Target = &Planes[Primary->Id];
  		RaidNear = (Target->WorldPosition/PlayerPlane->WorldPosition)*WUTONM - (RaidDistRange*0.5f);

		Plane = &Av.Planes.List[Av.Planes.NumPlanes];

		while(Plane-- > Av.Planes.List)
		{
			if (Plane->Index == Primary->Id)
				break;
		}

		if (Plane >= Av.Planes.List)
			RaidLeftRel = Plane->RelAngle;
		else
			RaidLeftRel = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Target->WorldPosition,TRUE);

		RaidLeftRel += RaidAngleRange * 0.5f;
	}

	Plane = &Av.Planes.List[0];
	Cnt = 0;

//	AARadarBaseSense = 9;

	while(Cnt < Av.Planes.NumPlanes)
	{
		PlaneIndex = Plane->Index;

		Target = (PlaneParams *)Plane->Obj;

		if (!(Target->FlightStatus & PL_STATUS_CRASHED))
		{
			jammer_hot = ( AVIsNoiseJamming(Target) || (Target->AI.iAIFlags2 & AI_SO_JAMMING));

			if (!(ptest = GetPlanePtrInList(&CurFramePlanes, PlaneIndex)) || jammer_hot || (ptest->RadarFrame < RadarFrame))
			{
				// this clears the radar bit for planes which are only
				// seen by AWACS
				if (ptest && ((RadarFrame - ptest->TimeAdded) > 1))
					ptest->Designation &= ~AA_ON_RADAR;

				passes_critical_aspect = (!Av.AARadar.SilentOn && !UFC.EMISState && PlaneVisibleByCriticalAspectRegion(PlayerPlane,Target));

				if (jammer_hot || passes_critical_aspect)
				{
					FPointDouble PlayerToScan,PlayerToEnemy;

					PlayerToScan.MakeVectorToFrom(ScanEnd,PlayerPlane->WorldPosition);
					PlayerToEnemy.MakeVectorToFrom(Target->WorldPosition,PlayerPlane->WorldPosition);

					DistToRay = PlayerToScan.Normalize();
					DistToEnemy = PlayerToEnemy.Normalize();

					passes_distance = (DistToEnemy <= DistToRay);

					if (jammer_hot)
						dist_ok = (DistToEnemy < (100.0f * NMTOWU));
					else
						dist_ok = passes_distance;

					if ((dist_ok) && ((PlayerToScan * PlayerToEnemy) >= cone_size))
					{
						/* -----------------10/28/99 11:07AM-------------------------------------------------------------------
						/* we're in the cone of the beam, but are we within the antenna sweep field?
						/* ----------------------------------------------------------------------------------------------------*/

						InField = FALSE;

  						if (Av.AARadar.CurMode == AA_ACQ_MODE)
  						{
							FPointDouble newp;
							FMatrix mat;

							newp = PlayerToEnemy;
							mat = PlayerPlane->Attitude;
							mat.Transpose();
							newp *= mat;

							yaw = atan2(-newp.X,-newp.Z)*RADIANS_TO_DEGREES;
							yaw = RLAngle(yaw - Av.AARadar.SlewLeftRight);
							if (fabs(yaw) <= Av.AARadar.AzimLimit)
							{
								pitch = atan2(newp.Y,sqrt(newp.X*newp.X + newp.Z*newp.Z))*RADIANS_TO_DEGREES;
								pitch = RLAngle(pitch - Av.AARadar.SlewUpDown);
								if (fabs(pitch) <= Av.AARadar.ElevLimit)
									InField = TRUE;

							}
  						}
  						else
						{
							yaw = atan2(-PlayerToEnemy.X,-PlayerToEnemy.Z)*RADIANS_TO_DEGREES;
							yaw = RLAngle(yaw - (fANGLE_TO_DEGREES(PlayerPlane->Heading) + Av.AARadar.SlewLeftRight));
							if (fabs(yaw) <= Av.AARadar.AzimLimit)
							{
								pitch = atan2(PlayerToEnemy.Y,sqrt(PlayerToEnemy.X*PlayerToEnemy.X + PlayerToEnemy.Z*PlayerToEnemy.Z))*RADIANS_TO_DEGREES;
								pitch = RLAngle(pitch - Av.AARadar.SlewUpDown);
								if (fabs(pitch) <= Av.AARadar.ElevLimit)
									InField = TRUE;
							}
						}

						if (InField)
						{

							float RelVel;
							float DeltaAlt;
							RelVel         = CalcRelativeVelocity(PlaneIndex);
							DeltaAlt       = (Target->WorldPosition.Y - PlayerPlane->WorldPosition.Y)*WUTOFT;
							float CrossSignature = GetCrossSection(pDBAircraftList[Target->AI.iPlaneIndex].iRadarCrossSignature,Av.AARadar.Prf,DeltaAlt,RelVel,DistToEnemy*WUTONM,FALSE);

							if ((Target->Type->TypeNumber == 61) && MultiPlayer)
								CrossSignature += 0.5f;

							passes_cross_section = ((CrossSignature+(frand()*0.1)) >=  Av.AARadar.AOTDamping);

							if(jammer_hot || passes_cross_section)
							{
	        					FPointDouble Intersection;

	   							if(LOS(&(PlayerPlane->WorldPosition),&(Planes[PlaneIndex].WorldPosition), &Intersection, MED_RES_LOS, FALSE))
	   							{
									if (jammer_hot && passes_distance && passes_critical_aspect && passes_cross_section)
										jammer_hot = FALSE;

									if (!jammer_hot || !ptest || (ptest->RadarFrame < RadarFrame))
									{
										if (!ptest)
										{
											if(CurFramePlanes.NumPlanes < (AA_MAX_DET_PLANES-1))
												ptest = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes++];
											else
											{
												Cnt++;
												Plane++;
												continue;
											}

											ptest->Designation = AA_NORMAL;
											ptest->IDLevel = AA_UNKNOWN;
										}
										else
											ptest->Designation &= ~AA_AWACS;


										/* --------------11/1/99 11:57AM-----------------------------------------------------------
										/* okay, now we have to figure out if the damn thing is visible as a regular
										/* (instead of AOT) target.
										/* ----------------------------------------------------------------------------------------*/

										if(Av.AARadar.RaidOn)
											XFrac = (RaidLeftRel-Plane->RelAngle) *  ooRaidAngleRange;
										else
											XFrac = (70.0f-Plane->RelAngle) * (1.0f / 140.0f);

										if(!CalcVelocity)
											if (Av.AARadar.RaidOn)
		  		   								YFrac = ((DistToEnemy*WUTONM) - RaidNear) * (ooRaidDistRange);
											else
		  		   								YFrac = (DistToEnemy*WUTONM)/Av.AARadar.Range;
	     								else
	     								{
											RelVel = CalcRelativeVelocity(PlaneIndex);
											YFrac = RelVel/Av.AARadar.VsLimit;                      // knts
			//								if(YFrac > 1.0) YFrac = 1.0;
	     								}

										if (ptest->AOT = jammer_hot)
										{
					 						YPos = AA_RADAR_TOP - 5;
											ptest->ShouldDraw = TRUE;
										}
										else
										{
  											ptest->ShouldDraw = ((YFrac <= 1.0) && (YFrac >= 0.0));

											YFrac = __max(0.0f,YFrac);
											YFrac = __min(1.0f,YFrac);
											YPos = LERP(AA_RADAR_BOTTOM,AA_RADAR_TOP,YFrac);
										}

  										ptest->ShouldDraw &= ((XFrac <= 1.0) && (XFrac >= 0.0));

										XFrac = __max(0.0f,XFrac);
										XFrac = __min(1.0f,XFrac);

	  									XPos = LERP(AA_RADAR_LEFT,AA_RADAR_RIGHT, XFrac);

										ptest->PlaneId       = PlaneIndex;
										ptest->OfsX          = XPos;
										ptest->OfsY          = YPos;
										ptest->RelAngle      = Plane->RelAngle;

										ptest->RelPitch      = -ComputePitchToPoint(PlayerPlane,Target->WorldPosition);

										ptest->Designation   |= AA_ON_RADAR;
										ptest->Designation   &= ~AA_NO_LANDS;

										if (!KeepIDLevel)
											ptest->IDLevel = AA_UNKNOWN;

										ptest->Heading       = Target->Heading/DEGREE;
										ptest->Color         = AV_GREEN;
										ptest->Shade         = 1.0;
										ptest->Loc           = Target->WorldPosition;
	//									ptest->AwacsReported = FALSE;
										ptest->TimeAdded	 = RadarFrame;
										ptest->RadarFrame    = RadarFrame;

										// track radar toward primary target if in TWS
									}

								} // Los maintained
							}
						}
					}
				}
			}
		}
		Cnt++;
		Plane++;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FilterAwacsPlanes(int CalcVelocity)
{
	float DistToEnemy;
	float RelVel;
	DetectedPlaneType TempPlaneInfo,*ptest;
	AvObjListEntryType *Awacs;
	float YFrac;
	float XFrac;
	float XPos,YPos;

	// this function must be called after FilterPlanesFromCone

	if(!Av.AARadar.MSI) return;

	Awacs = &Av.Planes.List[0];
	int Index = Av.Planes.NumPlanes;
	while(Index-- > 0)
	{
		PlaneParams *target = (PlaneParams *)Awacs->Obj;
		int PlaneId = ( target - &Planes[0] );
		PlaneParams *AwacsPlane = (PlaneParams *)Awacs->Obj;

		int IDLevel;

		switch (AwacsPlane->AI.iSide)
		{
			case AI_ENEMY:
				IDLevel = AA_HOSTILE;
				break;
			case AI_FRIENDLY:
				IDLevel = AA_FRIENDLY;
				break;
			case AI_NEUTRAL:
				IDLevel = AA_UNKNOWN;
				break;
		}

		if(AwacsPlane->AI.iAICombatFlags2 & AI_AWACS_DETECT)
		{
			float pphead = fANGLE_TO_DEGREES(PlayerPlane->Heading);

			float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,target->WorldPosition,TRUE);
			float DeltaElev = -ComputePitchToPoint(PlayerPlane, target->WorldPosition);

			ptest = GetPlanePtrInList(&CurFramePlanes, PlaneId);

			if (!CheckAnglesWithinBounds(PlayerPlane->Attitude,RLAngle(pphead+DeltaAzim),DeltaElev,70,70,-70,-70,NULL,NULL))
			{
//				if (!ptest || !(ptest->Designation & AA_AWACS))
				{
					if (ptest)// && (ptest->RadarFrame == RadarFrame))
					{
						ptest->IDLevel = IDLevel;
						ptest->Designation |= AA_AWACS;

						if(Av.AARadar.CurMode == AA_TWS_MODE)
					 		StickPlaneInList(&PrevFramePlanes,ptest);
					}
					else
					{
						if(CurFramePlanes.NumPlanes < (AA_MAX_DET_PLANES-1))
							ptest = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes++];
						else
						{
							Awacs++;
							continue;
						}

 		  				DistToEnemy = Awacs->Dist;

						if(Av.AARadar.RaidOn)
							XFrac = (RaidLeftRel-Awacs->RelAngle) *  (1.0f/2.0f);
						else
							XFrac = (70.0f-Awacs->RelAngle) * (1.0f / 140.0f);

						if(!CalcVelocity)
							if (Av.AARadar.RaidOn)
		  		   				YFrac = ((DistToEnemy*WUTONM) - RaidNear) * (1.0f/0.7f);
							else
		  		   				YFrac = (DistToEnemy*WUTONM)/Av.AARadar.Range;
	     				else
	     				{
							RelVel = CalcRelativeVelocity(PlaneId);
							YFrac = RelVel/Av.AARadar.VsLimit;                      // knts
	     				}

  						ptest->ShouldDraw = ((YFrac <= 1.0) && (YFrac >= 0.0)) && ((XFrac <= 1.0) && (XFrac >= 0.0));

						XFrac = __max(0.0f,XFrac);
						XFrac = __min(1.0f,XFrac);

						YFrac = __max(0.0f,YFrac);
						YFrac = __min(1.0f,YFrac);

	  					XPos = LERP(AA_RADAR_LEFT,AA_RADAR_RIGHT, XFrac);
						YPos = LERP(AA_RADAR_BOTTOM,AA_RADAR_TOP,YFrac);

						ptest->PlaneId       	= PlaneId;
						ptest->OfsX          	= XPos;
						ptest->OfsY          	= YPos;
						ptest->Designation   	= AA_AWACS;
						ptest->Heading       	= target->Heading/DEGREE;
						ptest->Color     		= AV_GREEN;
						ptest->Shade     		= 1.0;
						ptest->IDLevel       	= IDLevel;
						ptest->AOT           	= FALSE;
						ptest->Loc       	 	= target->WorldPosition;
	//					ptest->AwacsReported 	= TRUE;
						ptest->RadarFrame		= RadarFrame;

						if(Av.AARadar.CurMode == AA_TWS_MODE)
					 		StickPlaneInList(&PrevFramePlanes,ptest);
					}
				}
			}
			else
			{
				if (ptest)
				{
					ptest->Designation &= ~AA_AWACS;
					ptest->Designation |= AA_NO_LANDS;
				}
			}
		}

		Awacs++;
 	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

//int IsTargetInCone(int PlaneIndex)
//{
//	float DistToEnemy;
//	float YFrac;
//	int   InCone;

//	float MaxRange = 80.0; // jlm get from database

//	if(PlaneIndex == -1) return(FALSE);

//	DistToEnemy = PlayerPlane->WorldPosition / Planes[PlaneIndex].WorldPosition;

//	InCone = (DistToEnemy <= Av.AARadar.Range*NMTOWU);

//	if(!InCone)
//	{
//	  // auto range
//	  if(Av.AARadar.Range < MaxRange)
//		Av.AARadar.Range *= 2;

//		InCone = (DistToEnemy <= Av.AARadar.Range*NMTOWU);

//	}
//	else
//	{
//	// auto range
//		while( (DistToEnemy <= (0.45*Av.AARadar.Range*NMTOWU)) && (Av.AARadar.Range > 10.0) )
//			Av.AARadar.Range /= 2.0;

//		YFrac = (DistToEnemy*WUTONM)/Av.AARadar.Range;
//		if(YFrac <= 1.0)
//			Primary->ShouldDraw = TRUE;
//	else
//			Primary->ShouldDraw = FALSE;


//		float XFrac = Primary->RelAngle/70.0;

//		if( XFrac > 1.0 )
//			XFrac = 1.0;

//		int XPos;
//		if(XFrac < 0.0)
//		  XPos = LERP(AA_RADAR_CENTER_X,AA_RADAR_RIGHT,-XFrac);
//		else
//		  XPos = LERP(AA_RADAR_CENTER_X,AA_RADAR_LEFT,XFrac);

//		int YPos;
//		float YFrac = (DistToEnemy*WUTONM)/Av.AARadar.Range;

//		YPos = LERP(AA_RADAR_BOTTOM,AA_RADAR_TOP,YFrac);

//		Primary->ShouldDraw = ( (YFrac <= 1.0) && (YFrac >= 0.0) ) ? TRUE : FALSE;

//		Primary->XOfs = XPos;
//		Primary->YOfs = YPos;
//	}

//	return(InCone);

//}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float RelAngleToMagnitude(float Angle,float Magnitude)
{
 if( fabs(Angle) > 90)
	Angle = 180.0 - fabs(Angle);

 float Final = 1.0 - (Angle/90.0);

 if( (Angle > 90.0) || (Angle < -90) )
	Final = -Final;

 return(Final*Magnitude);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float CalcTTG(PlaneParams *P1, PlaneParams *P2)
{
 float Vel1,Vel2;

 float RelAngle = ComputeHeadingToPointDouble(P1->WorldPosition,P1->Heading,P2->WorldPosition,1);

 Vel1 = RelAngleToMagnitude(RelAngle,P1->IfHorzVelocity);

 RelAngle = ComputeHeadingToPointDouble(P2->WorldPosition,P2->Heading,P1->WorldPosition,1);

 Vel2 = RelAngleToMagnitude(RelAngle,P2->IfHorzVelocity);

 return(Vel1 + Vel2);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SortByIdLevel()
{
	AARankingType *R1 = &Ranks.Planes[0];
	AARankingType *R2;

	for(int i=0; i<Ranks.NumPlanes-1;i++)
	{
		R2 = &Ranks.Planes[i+1];
		for(int j=i+1; j<Ranks.NumPlanes; j++)
		{
			if(R1->IDLevel > R2->IDLevel)   // AA_FRIENDLY HAS THE GREATEST VALUE...DON'T CHANGE THOSE DEFINES!!
			{
				AARankingType Temp;
				memcpy(&Temp,R1,sizeof(AARankingType));
				memcpy(R1,R2,sizeof(AARankingType));
				memcpy(R2,&Temp,sizeof(AARankingType));
			}
			R2++;
		}
		R1++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SortByTTG(int Start,int End)
{
	AARankingType *R1 = &Ranks.Planes[Start];
	AARankingType *R2;

	for(int i=Start; i<End;i++)
	{
		R2 = &Ranks.Planes[i+1];
		for(int j=i+1; j<=End; j++)
		{
			if(R1->TTG > R2->TTG)   // AA_FRIENDLY HAS THE GREATEST VALUE...DON'T CHANGE THOSE DEFINES!!
			{
				AARankingType Temp;
				memcpy(&Temp,R1,sizeof(AARankingType));
				memcpy(R1,R2,sizeof(AARankingType));
				memcpy(R2,&Temp,sizeof(AARankingType));
			}
			R2++;
		}
		R1++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SortByAOT(int Start,int End)
{
	AARankingType *R1 = &Ranks.Planes[Start];
	AARankingType *R2;

	for(int i=Start; i<End;i++)
	{
		R2 = &Ranks.Planes[i+1];
		for(int j=i+1; j<=End; j++)
		{
			if( (R1->AOT == TRUE) && (R2->AOT == FALSE) )
			{
				AARankingType Temp;
				memcpy(&Temp,R1,sizeof(AARankingType));
				memcpy(R1,R2,sizeof(AARankingType));
				memcpy(R2,&Temp,sizeof(AARankingType));
			}
			R2++;
		}
		R1++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SortPlanesAccordingToRank()
{
	AARankingType *R = &Ranks.Planes[0];

	int Index = Ranks.NumPlanes;
	while(Index-- > 0)
	{
		PlaneParams *P = &Planes[R->P->PlaneId];
		R->TTG = CalcTTG(PlayerPlane,P);
		R++;
	}

	SortByIdLevel();

	R = &Ranks.Planes[0];
	int StartIdLevel = R->IDLevel;
	int FriendlyFound = FALSE;

	Index = 0;
	while(Index < Ranks.NumPlanes)
	{
		if(R->IDLevel != StartIdLevel)
		{
			FriendlyFound = TRUE;
			break;
		}
		R++;
		Index++;
	}

	if(FriendlyFound)
	{
		SortByTTG(0,Index-1);
		SortByTTG(Index,Ranks.NumPlanes-1);
		SortByAOT(0,Index-1);
		SortByAOT(Index,Ranks.NumPlanes-1);
	}
	else
	{
		SortByTTG(0,Ranks.NumPlanes-1);
		SortByAOT(0,Ranks.NumPlanes-1);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetPlaneRankings()
{
	// add planes that need ranking to the rank list
	Ranks.NumPlanes = 0;

	DetectedPlaneType *P;

	P = &PrevFramePlanes.Planes[PrevFramePlanes.NumPlanes];

	while(P-- > PrevFramePlanes.Planes)
	{
		if (!(P->Designation & (AA_PRIMARY | AA_SECONDARY | AA_AWACS)))
		{
			// default to NORMAL
			P->Designation &= ~AA_RANKED;

			if(Ranks.NumPlanes + 1 < AA_MAX_RANKED)
			{
				Ranks.NumPlanes++;
				Ranks.Planes[Ranks.NumPlanes-1].P = P;
				Ranks.Planes[Ranks.NumPlanes-1].IDLevel = P->IDLevel;
				Ranks.Planes[Ranks.NumPlanes-1].AOT = P->AOT;    // JLM TEMPORARY. NEED TO FIND OUT IF TARGET HAS UNKNOWN RANGE (JAMMED)
			}
		}
	}

	// now sort the list
	SortPlanesAccordingToRank();

	DetectedPlaneType *Dest = &TempCurFramePlanes.Planes[0];

	// now make the list, Primary, Secondary, Awacs, Ranked, NonRanked
	if(PrevFramePlanes.Planes[0].Designation & AA_PRIMARY)
	{
		memcpy(Dest,&PrevFramePlanes.Planes[0],sizeof(DetectedPlaneType));
	  	Dest++;
	}

	if(PrevFramePlanes.Planes[1].Designation & AA_SECONDARY)
	{
		memcpy(Dest,&PrevFramePlanes.Planes[1],sizeof(DetectedPlaneType));
	  	Dest++;
	}

	// add awacs
	P = &PrevFramePlanes.Planes[PrevFramePlanes.NumPlanes];

	while(P-- > PrevFramePlanes.Planes)
	{
		if(P->Designation & AA_AWACS)
		{
			memcpy(Dest,P,sizeof(DetectedPlaneType));
			Dest++;
		}
	}

	// add ranked (first 8)
	AARankingType *R = &Ranks.Planes[0];
	int Index = 0;
	while(Index < Ranks.NumPlanes)
	{
		if(Index >= 8) break; // max ranked planes is 8
		R->P->Designation |= AA_RANKED;
		R->P->Rank = Index+1;
		memcpy(Dest,R->P,sizeof(DetectedPlaneType));
		R++;
		Dest++;
		Index++;
	}

	// now add the rest
	P = &PrevFramePlanes.Planes[PrevFramePlanes.NumPlanes];

	while(P-- > PrevFramePlanes.Planes)
	{
		if (!(P->Designation & (AA_PRIMARY | AA_SECONDARY | AA_AWACS | AA_RANKED)))
		{
			memcpy(Dest,P,sizeof(DetectedPlaneType));
			Dest++;
		}
	}

	// you should now have the full list, in TempCurFramePlanes..lets copy it back to CurFramePlanes
	memcpy(&PrevFramePlanes.Planes[0],&TempCurFramePlanes.Planes[0],sizeof(DetectedPlaneType)*PrevFramePlanes.NumPlanes);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FadeAgePlanes()
{
  DetectedPlaneListType *P = &AgePlanes[0];
  int Index = AA_MAX_AGE_PLANES;
  while(Index-- > 0)
  {
	  if( P->FadeTimer.IsActive() )
		{
		  float Frac = ( P->FadeTimer.TimeElapsed(GameLoopInTicks) )/Av.AARadar.AgeFrequency;
		  P->ColorFadePercent = 1.0 - Frac;
		  if(P->ColorFadePercent < 0.0) P->ColorFadePercent = 0;
		  if(P->ColorFadePercent <= 0.3)
			  P->NumPlanes=0;
		  P++;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AgeDetectedPlanes()
{
  DetectedPlaneListType *P = &AgePlanes[AA_MAX_AGE_PLANES-1];
  DetectedPlaneListType *Next = P-1;

  int Index = AA_MAX_AGE_PLANES-1;
  while(Index-- > 0)
  {
		CopyPlaneList(Next,P);
		Next--;
		P--;
	}

	if (Av.AARadar.CurMode != AA_TWS_MODE)
  		CopyPlaneList(&CurFramePlanes,&AgePlanes[0]);
	else
  		CopyPlaneList(&PrevFramePlanes,&AgePlanes[0]);

	AgePlanes[0].FadeTimer.Set(Av.AARadar.AgeFrequency,GameLoopInTicks);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetPrimarySecondaryForHud()
{
	FPointDouble NullPos;

	NullPos.X = -1; NullPos.Y = -1; NullPos.Z = -1;

	if(Primary->Id == -1)
	{
	  SetHudTDInfo(NullPos,0);
	}

	if(Secondary->Id == -1)
	{
	  SetHudTDInfo(NullPos,1);
	}

	PlayerPlane->AADesignate = (Primary->Id != -1) ? &Planes[Primary->Id] : NULL;

  DetectedPlaneType *P;
  DetectedPlaneListType *PList = (Av.AARadar.CurMode == AA_TWS_MODE) ? &PrevFramePlanes : &CurFramePlanes;

   P = &PList->Planes[PList->NumPlanes];
  while(P-- > PList->Planes)
  {
		if(P->PlaneId == Primary->Id)
			SetHudTDInfo(P->Loc,0); // 0 = primary, 1 = secondary, 3 = stt

		if(P->PlaneId == Secondary->Id)
			SetHudTDInfo(P->Loc,1); // 0 = primary, 1 = secondary, 3 = stt
	}
}

//*****************************************************************************************************************************************
// PROCESS LOST TARGETS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void ProcessLostTargets()
{
//   int Index=0;
//   while(Index < PrevFramePlanes.NumPlanes)
//   {
//	   int PlaneIndex = PrevFramePlanes.Planes[Index].PlaneId;
//	   if(!IsPlaneInList(&CurFramePlanes, PlaneIndex))
//	     RemovePlaneFromList(&PrevFramePlanes, PlaneIndex);
//	   else
//	     Index++;
//   }
}


//*****************************************************************************************************************************************
// BLINKS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SimBlinks()
{
	if( !AA1SecBlink.IsActive() )
		AA1SecBlink.Set(1.0,GameLoopInTicks);
	else
	{
		if( AA1SecBlink.TimeUp(GameLoopInTicks) )
		{
			AA1SecBlinkState = !AA1SecBlinkState;
			AA1SecBlink.Set(1.0,GameLoopInTicks);
		}
	}
}

//*****************************************************************************************************************************************
// STT FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
 // jlm test var

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsPlaneDetected(int PlaneId)
{
	if(Av.AARadar.CurMode == AA_ACQ_MODE) return(FALSE);

	if(Av.AARadar.CurMode == AA_STT_MODE)
	{
		if(Primary->Id == PlaneId)
			return(TRUE);
	}
	else if(Av.AARadar.CurMode == AA_TWS_MODE)
	{
    for(int i=0; i<PrevFramePlanes.NumPlanes; i++)
    {
	    if(PrevFramePlanes.Planes[i].PlaneId == PlaneId)
	    	return(TRUE);
    }
	}

	return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void PlayerSttTargetJamming()
{
	if(Primary->Id != -1)
		Av.AARadar.SttJamming = TRUE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

//BOOL SimSTTJamming()
//{
//	BOOL ret_value = TRUE;
//  if(Av.AARadar.SttJamming)
//	{
//		if(!Primary->Jammer.TargetJamming)
//		{
//			Primary->Jammer.TargetJamming = TRUE;
//			Primary->Jammer.JammerTimer.Set(3.0,GameLoopInTicks);
//		}
//	}
//	else
//	{
//		 if(Primary->Jammer.TargetJamming)
//		 {
//			Primary->Jammer.TargetJamming = FALSE;
//			Primary->Jammer.JammerTimer.Disable();
//		 }
//	}

//	if( (Primary->Jammer.TargetJamming) && (!Primary->Jammer.JammerFakeOut) )
//	{
//		 if( Primary->Jammer.JammerTimer.TimeUp(GameLoopInTicks) )
//		 {
//				//if( rand() % 5 > 2)
//			  // dice rolls occur on scott's side..always true for now
//			  if(TRUE)
//				{
//					Primary->Jammer.JammerFakeOut   = TRUE;
//		          float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Planes[Primary->Id].WorldPosition,TRUE);
//					Primary->Jammer.JammerDirection = ( (DeltaAzim > 90.0) || (DeltaAzim < -90) ) ? UP : DOWN;
//					Primary->Jammer.JammerTimer.Set(5.0,GameLoopInTicks);
//					Primary->Jammer.JammerYPos = Primary->YOfs;
//					Primary->Jammer.JammerXPos = Primary->XOfs;
//					Primary->Jammer.Heading = Planes[Primary->Id].Heading/DEGREE;
//				}
//		 }
//	}

//	if(Primary->Jammer.JammerFakeOut)
//	{
//		float TimeElapsed = Primary->Jammer.JammerTimer.TimeElapsed(GameLoopInTicks);
//		float Frac = TimeElapsed/3.0;
//		if(Frac > 1.0) Frac = 1.0;

//		Primary->YOfs = Primary->Jammer.JammerYPos + (20*(Frac)*((Primary->Jammer.JammerDirection == UP) ? -1.0 : 1.0));
//		Primary->XOfs = Primary->Jammer.JammerXPos;

//		if( Primary->Jammer.JammerTimer.TimeUp(GameLoopInTicks) )
//		{
//		  Av.AARadar.CurMode = AA_RWS_MODE;
//		  ResetAARadar( AA_RWS_MODE);
//		  ResetPrimary();
//		}

//		ret_value = FALSE;
//		Primary->ShouldDraw = ( (Primary->YOfs > AA_RADAR_TOP) && (Primary->YOfs < AA_RADAR_BOTTOM) );

//	}

//}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimSTTLockReaquire()
{
	if(Primary->Reaquire.BrokeLockReaquire)
	{
		 if( !Primary->Reaquire.BreakLockTimer.IsActive() )
		 {
			 Primary->Reaquire.BreakLockTimer.Set(5.0,GameLoopInTicks);
			 Primary->Reaquire.LastPos = Planes[Primary->Id].LastWorldPosition;
			 Primary->Reaquire.CurPos = Planes[Primary->Id].WorldPosition;
			 Primary->Reaquire.Velocity = Planes[Primary->Id].V;
			 Primary->Reaquire.Velocity *= FTTOWU;
			 Primary->Reaquire.ReaquireXPos = Primary->XOfs;
			 Primary->Reaquire.ReaquireYPos = Primary->YOfs;
			 Primary->Reaquire.Heading = Planes[Primary->Id].Heading/DEGREE;

       		SetBrokeLockHudTimer(5.0,Planes[Primary->Id].WorldPosition);
		 }

		 FPointDouble ProjPos;
		 ProjPos = Primary->Reaquire.CurPos;
		 ProjPos -= Primary->Reaquire.LastPos;
		 ProjPos.Normalize();
		 float TimeElapsed = Primary->Reaquire.BreakLockTimer.TimeElapsed(GameLoopInTicks);
		 ProjPos *= Primary->Reaquire.Velocity*TimeElapsed;

//	     float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,ProjPos,TRUE);
//		   float DeltaElev = -ComputePitchToPoint(PlayerPlane, ProjPos);

		float pphead = fANGLE_TO_DEGREES(PlayerPlane->Heading);

		float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,ProjPos,TRUE);

		float DeltaElev = -ComputePitchToPoint(PlayerPlane, ProjPos);

		CheckAnglesWithinBounds(PlayerPlane->Attitude,RLAngle(pphead+DeltaAzim),DeltaElev,70,70,-70,-70,&Av.AARadar.AntAzimuth,&Av.AARadar.AntElevation);

		 if(Primary->Reaquire.BreakLockTimer.TimeUp(GameLoopInTicks) )
		 {
		   Av.AARadar.CurMode = AA_RWS_MODE;
		   ResetAARadar(AA_RWS_MODE);
		   ResetPrimary();

     	 	Primary->XOfs = Primary->Reaquire.ReaquireXPos;
	   	 	Primary->YOfs	= Primary->Reaquire.ReaquireYPos;
		 	Primary->ShouldDraw = TRUE;AA1SecBlinkState;

		 	CurFramePlanes.NumPlanes = 0;

		 }
		 else
		 {
     	 	Primary->XOfs = Primary->Reaquire.ReaquireXPos;
	   	 	Primary->YOfs	= Primary->Reaquire.ReaquireYPos;
		 	Primary->ShouldDraw = TRUE;AA1SecBlinkState;

		 	CurFramePlanes.NumPlanes = 1;
		 	CurFramePlanes.Planes[0].PlaneId = Primary->Id;
		 	CurFramePlanes.Planes[0].OfsX = Primary->XOfs;
		 	CurFramePlanes.Planes[0].OfsY = Primary->YOfs;
		 	CurFramePlanes.Planes[0].RelAngle = Primary->RelAngle;
		 }
	}
	else
	{
		if( Primary->Reaquire.BreakLockTimer.IsActive() )
		{
			Primary->Reaquire.BreakLockTimer.Disable();
		}
	}
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

PlaneParams *GetTargetForAim120()
{
  int Index=0;
  BOOL check_primary;
  PlaneParams *TargetPlane;

  check_primary = ((Primary->Id != -1) && !PrimaryIsAOT(&CurFramePlanes));

  // check primary plane first
  if (check_primary)
  {
    TargetPlane = &Planes[Primary->Id];
    if( !AISeeIfStillMissiled(PlayerPlane,TargetPlane,0) )
		  return(TargetPlane);
  }

  while(Index < NumDTWSTargets)
  {
	  TargetPlane = &Planes[DTWSTargets[Index]];
	  if( !AISeeIfStillMissiled(PlayerPlane, TargetPlane, 0) )
    	  return(TargetPlane);
	  Index ++;
  }

  // nobody free, shoot at primary
  if(check_primary)
  {
    TargetPlane = &Planes[Primary->Id];
    return(TargetPlane);
  }

  return(NULL);
}


//*****************************************************************************************************************************************
// DRAW UTILS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawRadarSense()
{
	float print_sense;

	if (Av.AARadar.AOTDamping <= 4.0f)
		print_sense = 9.0f;
	else
		if (Av.AARadar.AOTDamping <= 7.0f)
			print_sense = 9.0f - (Av.AARadar.AOTDamping-4.0f)*2.0f;
		else
			if (Av.AARadar.AOTDamping >= 9.0f)
				print_sense = 1.0f;
			else
				print_sense = 3.0f - (Av.AARadar.AOTDamping-7.0f);

	sprintf(TmpStr,"%1.f",print_sense);
	DrawTextAtLoc(7,121,TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAANIRDTriangle(float X,float Y,float Radius,float Degree)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin;

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	ProjectPoint(X,Y,NormDegree(Degree),Radius+1,&Origin.X,&Origin.Y);

	Degree = NormDegree(Degree - 15);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10,&NewX1,&NewY1);

	Degree = NormDegree(Degree + 30);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10,&NewX2,&NewY2);

  GrDrawLine(GrBuffFor3D,Origin.X,Origin.Y,NewX1,NewY1,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,Origin.X,Origin.Y,NewX2,NewY2,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,NewX1,NewY1,NewX2,NewY2,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAANIRDDiamond(float X,float Y,float Radius,float Degree)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin,Origin2;

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	ProjectPoint(X,Y,NormDegree(Degree),Radius+1,&Origin.X,&Origin.Y);

	Degree = NormDegree(Degree - 15);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10,&NewX1,&NewY1);

	Degree = NormDegree(Degree + 30);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10,&NewX2,&NewY2);

	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),19.3,&Origin2.X,&Origin2.Y);   // 19.3 = 10*cos(30degrees)*2

  GrDrawLine(GrBuffFor3D,Origin.X,Origin.Y,NewX1,NewY1,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,Origin.X,Origin.Y,NewX2,NewY2,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,NewX1,NewY1,Origin2.X,Origin2.Y,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,NewX2,NewY2,Origin2.X,Origin2.Y,Color.Red,Color.Green,Color.Blue,0);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAANIRDTick(float X,float Y,float Radius,float Degree,int Inside)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin;
	float OriginalDegree = Degree;

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	ProjectPoint(X,Y,NormDegree(Degree),Radius,&Origin.X,&Origin.Y);

	if(Inside)
	  Degree = NormDegree(Degree+180);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6,&NewX1,&NewY1);
  GrDrawLine(GrBuffFor3D,Origin.X,Origin.Y,NewX1,NewY1,Color.Red,Color.Green,Color.Blue,0);

	Degree = NormDegree(Degree+1);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6,&NewX2,&NewY2);
  GrDrawLine(GrBuffFor3D,Origin.X,Origin.Y,NewX2,NewY2,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAANIRDTicks(int X,int Y,int Radius)
{
	// RMin,RMax,RAero
	DrawAANIRDTriangle(X,Y,Radius,285.5);
	DrawAANIRDTriangle(X,Y,Radius,180.0);
	DrawAANIRDTriangle(X,Y,Radius,225.0);

	// Range
	float DegreePerRange = 60.0/(AA_Rmax-AA_Rmin);
	float RangeTickDeg   = 285.0 - (AA_range - AA_Rmin)*DegreePerRange;

	if(RangeTickDeg < 10)
		RangeTickDeg = 10;
	else if(RangeTickDeg > 360.0)
		RangeTickDeg = 360.0;

	RangeTickDeg = NormDegree(RangeTickDeg);
	DrawAANIRDTick(X,Y,Radius,RangeTickDeg,TRUE);

	// ROpt
	RangeTickDeg   = 285.0 - (AA_ROpt - AA_Rmin)*DegreePerRange;

	if(RangeTickDeg < 10)
		RangeTickDeg = 10;
	else if(RangeTickDeg > 360.0)
		RangeTickDeg = 360.0;

	RangeTickDeg = NormDegree(RangeTickDeg);
	DrawAANIRDTriangle(X,Y,Radius,RangeTickDeg);

	// Gun Tick
	int AARadarMode = GetAARadarMode();
	if(AARadarMode != -1)
	{
		if(AARadarMode == AA_STT_MODE)
		{
			if(AA_range <= 12000*FTTONM)
			{
				 RangeTickDeg   = 285.0 - ((12000*FTTONM) - AA_Rmin)*DegreePerRange;

				 if(RangeTickDeg < 10)
						RangeTickDeg = 10;
				 else if(RangeTickDeg > 360.0)
						RangeTickDeg = 360.0;

					RangeTickDeg = NormDegree(RangeTickDeg);
					DrawAANIRDTick(X,Y,Radius,RangeTickDeg,FALSE);
			}
		}
	}
}

//*****************************************************************************************************************************************
// DRAW ASE CIRCLE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawAAASEcircle(int X,int Y, int radius)
{
	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

    int DrawType = 0;
	int ShouldDraw = TRUE;
	switch(Av.Weapons.CurrWeapMode)
	{
  	case AIM_9P:    ShouldDraw = FALSE; break;
  	case AIM9_STAT: DrawType = 0;       break;
  	case AIM7_STAT: DrawType = 0;       break;
  	case AIM_120A:	DrawType = 1;       break;
	}

	if(ShouldDraw)
	{
		if(DrawType == 0)
		  GrDrawPolyCircle(GrBuffFor3D,X,Y,radius,20,Color.Red,Color.Green,Color.Blue,0);
		else
		  GrDrawPolyCircle(GrBuffFor3D,X,Y,radius,20,Color.Red,Color.Green,Color.Blue,0); // needs to be dashed jlm

	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAARadarHudSymbology()
{
	AvRGB Color;
  	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	 AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

	 if( (CurAAWeap != NULL) && (Primary->Id != -1) )
	 {
		 if(AA_breakX)
		 {
			 if(AA_flash)
			 {
	       GrDrawLine(GrBuffFor3D,AA_RADAR_CENTER_X-20,AA_RADAR_CENTER_Y-20,AA_RADAR_CENTER_X+20,AA_RADAR_CENTER_Y+20,Color.Red,Color.Green,Color.Blue,0);
	       GrDrawLine(GrBuffFor3D,AA_RADAR_CENTER_X-20,AA_RADAR_CENTER_Y+20,AA_RADAR_CENTER_X+20,AA_RADAR_CENTER_Y-20,Color.Red,Color.Green,Color.Blue,0);
			 }
		 }

		if (AA_shootQ && (UFC.MasterMode != AG_MODE))		// flash cue @ 2Hz w/ 50% duty cycle
		{
			if( HudFlashTimer.TimeUp(GameLoopInTicks) )
			{
				AA_flash = !AA_flash;
				HudFlashTimer.Set(0.25,GameLoopInTicks);
			}


			if ( ( (Av.Weapons.CurrWeapMode == AIM7_STAT) || (Av.Weapons.CurrWeapMode == AIM120_STAT) ) && (AA_range > AA_Rtr))
				AA_flash = TRUE;	// MRMs only flash within RTR
			else if ((Av.Weapons.CurrWeapMode == AIM9_STAT) && (AA_range > AA_Rtr))
				AA_flash = TRUE;	// SRMs only flash within RTR

			if (AA_flash)
			{
				if(Av.Weapons.UseAASeq)
				{
					if ((Av.Weapons.CurAASeqWeap == NULL) || (!Av.Weapons.CurAASeqWeap->Selected) || (Av.Weapons.CurAASeqWeap->W->Count <= 0))
						AA_flash = FALSE;
					else
						if ((Av.Weapons.CurAASeqWeap->W->WeapId == Aim120_Id) && (WeapStores.Aim120VisualOn))
							AA_flash = FALSE;
				}
				else
				{
					if ((Av.Weapons.CurAAWeap == NULL) || (!Av.Weapons.CurAAWeap->Selected) || (Av.Weapons.CurAAWeap->W->Count <= 0))
						AA_flash = FALSE;
					else
						if ((Av.Weapons.CurAAWeap->W->WeapId == Aim120_Id) && (WeapStores.Aim120VisualOn))
							AA_flash = FALSE;
				}
			}

		 	if(AA_flash)
			{
			 	if( (CurAAWeap->WeapPage == AIM7_WEAP_PAGE) || ((CurAAWeap->WeapPage == AIM120_WEAP_PAGE) && (AA_Tact <= 0)))
			 	{
				 	sprintf(TmpStr,"%s %02d","SHOOT",AA_Tpre);
	  			 	DrawTextAtLoc(59,126,TmpStr,AV_GREEN,1.0);
			 	}
			 	else if(CurAAWeap->WeapPage == AIM120_WEAP_PAGE)
			 	{
				 	sprintf(TmpStr,"%s %02dA","SHOOT",AA_Tact);
	  			 	DrawTextAtLoc(59,126,TmpStr,AV_GREEN,1.0);
			 	}
			 	else
		  		 	DrawTextAtLoc(59,126,"SHOOT",AV_GREEN,1.0);
			}
//			else
//		   		GrDrawStringClipped( GrBuffFor3D, LgHUDFont,PosX-8,PosY+9, "NO RDR", HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);

		}
	 }

	 if(AA_DrawASE)
	 {
	   float radius = 27.0 * (0.75 * (0.75 * AA_Rmax / AA_range));
		 if(radius > 27) radius = 27;
		 DrawAAASEcircle(AA_RADAR_CENTER_X,AA_RADAR_CENTER_Y,radius);
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAABullseye()
{
	FPointDouble BELoc;
	float TempX,TempZ;

	if( (BullsEye.x <= 0) && (BullsEye.z <= 0) ) return;

	AvRGB Color;
  	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	BELoc.X = BullsEye.x*FTTOWU;
	BELoc.Z = BullsEye.z*FTTOWU;
	BELoc.Y = 0;

  	float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,BELoc,1);
	float Dist      = Dist2D(&PlayerPlane->WorldPosition,&BELoc);
	Dist *= WUTONM;

  	float Bearing = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,BELoc,0);
	Bearing = NormDegree(360.0 - NormDegree(Bearing));

	float XFrac,YFrac;
	int XPos,YPos;
  	YFrac = (Dist)/Av.AARadar.Range;

	if(  (YFrac <= 1.0) && (YFrac >= 0.0)  )
	{

		YPos = LERP(AA_RADAR_BOTTOM,AA_RADAR_TOP,YFrac);

		XFrac = (DeltaAzim / 70.0 );

  		if(  fabs(XFrac) <= 1.0  )
		{

			if(XFrac < 0.0)
	  		XPos = LERP(AA_RADAR_CENTER_X,AA_RADAR_RIGHT, -XFrac);
			else
	  		XPos = LERP(AA_RADAR_CENTER_X,AA_RADAR_LEFT,XFrac);

			GrDrawPolyCircle(GrBuffFor3D,XPos,YPos,4,20,Color.Red,Color.Green,Color.Blue,0);

    		DeltaAzim = NormDegree(0 + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));

			ProjectPoint(XPos,YPos,DeltaAzim,11,&TempX,&TempZ);
			GrDrawLine(GrBuffFor3D,XPos,YPos,TempX,TempZ,Color.Red,Color.Green,Color.Blue,0);
			DrawTriangleRot(TempX,TempZ,3,DeltaAzim,AV_GREEN,1.0,TRUE);
		}
	}

	FPointDouble C,B;

	B.X = CaptBars.Pos.X-1.0;

	B.X -= AA_RADAR_CENTER_X;

	B.X /= AA_RADAR_LEFT-AA_RADAR_RIGHT;
	B.X *= 140.0f;

	B.X += fANGLE_TO_DEGREES(PlayerPlane->Heading);

	B.Y = AA_RADAR_BOTTOM - (CaptBars.Pos.Y-1.0);
	B.Y /= AA_RADAR_BOTTOM - AA_RADAR_TOP;
	B.Y *= Av.AARadar.Range * NMTOWU;

	ProjectPoint(0.0,0.0,B.X,B.Y,&TempX,&TempZ);

	C.SetValues(TempX,0.0f,TempZ);

	B.MakeVectorToFrom(BELoc,PlayerPlane->WorldPosition);

  	Bearing = ComputeHeadingToPoint(B,0,C,0);
	Bearing = NormDegree(360.0 - Bearing);
	Dist      = Dist2D(&C,&B)*WUTONM;
	sprintf(TmpStr,"%d/%2.f",(int)Bearing,Dist);
  	DrawTextAtLoc(30,10,TmpStr,AV_GREEN,1.0); //was 25
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawSelectedWeapon()
{
	 AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

	 if(CurAAWeap != NULL)
	 {
		 int Found = TRUE;

		 AvWeapEntryType *Weap = CurAAWeap;

		 int Count = WeapQuantity(Weap->W->WeapId);

		 switch(Weap->W->WeapId)
		 {
		  case AA_7F:  sprintf(TmpStr,"7F %d",Count);  break;
		  case AA_7M : sprintf(TmpStr,"7M %d",Count);  break;
		  case AA_7MH: sprintf(TmpStr,"7MH %d",Count); break;
		  case AA_9L:  sprintf(TmpStr,"9L %d",Count);  break;
		  case AA_9M:  sprintf(TmpStr,"9M %d",Count);  break;
		  case AA_9X:  sprintf(TmpStr,"9X %d",Count);  break;
		  case AA_120: sprintf(TmpStr,"AM %d",Count);  break;
		  default:
				Found = FALSE;
		 }

		 if(Found)
		 {
			 DrawTextAtLoc(AA_RADAR_RIGHT - 18,AA_RADAR_EXTREME_TOP - 6,TmpStr,AV_GREEN,1.0);
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTargetProfile(float X)
{
	if(Av.Weapons.CurAAWeap == NULL)    return;
	if(!Av.Weapons.CurAAWeap->Selected) return;
	if(Av.Weapons.CurAAWeap->W->Count <= 0) return;

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	if( (Av.AARadar.CurMode == AA_TWS_MODE) && Av.AARadar.RaidOn) return;
	if(Av.AARadar.CurMode == VS_MODE) return;

	float PixelsPerMile = ( (AA_RADAR_BOTTOM+1) - (AA_RADAR_TOP-1) )/Av.AARadar.Range;

	float Y1,Y2,Y3,Y4,ClipTop;

	Y1 = (AA_RADAR_BOTTOM-1) - (AA_Rmin*PixelsPerMile);
	Y2 = (AA_RADAR_BOTTOM-1) - (AA_ROpt*PixelsPerMile);
	Y3 = (AA_RADAR_BOTTOM-1) - (AA_Rtr*PixelsPerMile);
	Y4 = (AA_RADAR_BOTTOM-1) - (AA_Rmax*PixelsPerMile);

	if(Y1 < AA_RADAR_TOP+1) return;

	ClipTop = Y4;

	if(ClipTop < AA_RADAR_TOP+1)
		ClipTop = AA_RADAR_TOP+1;

	GrDrawLineClipped(GrBuffFor3D,X,ClipTop,X,Y1,Color.Red,Color.Green,Color.Blue,0);

	if(Y1 >= AA_RADAR_TOP+1)
	  GrDrawLineClipped(GrBuffFor3D,X-3,Y1,X+3,Y1,Color.Red,Color.Green,Color.Blue,0);
	if(Y2 >= AA_RADAR_TOP+1)
	  GrDrawLineClipped(GrBuffFor3D,X-3,Y2,X+3,Y2,Color.Red,Color.Green,Color.Blue,0);
	if(Y3 >= AA_RADAR_TOP+1)
	  GrDrawLineClipped(GrBuffFor3D,X-3,Y3,X+3,Y3,Color.Red,Color.Green,Color.Blue,0);
	if(Y4 >= AA_RADAR_TOP+1)
	  GrDrawLineClipped(GrBuffFor3D,X-3,Y4,X+3,Y4,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawMissles(int ScrX,int ScrY,int PlaneId)
{
	ChasingMisslesType *Cm = &MisslesInFlight[0];
	int Index = 0;
	float Frac;
	PlaneParams *P = NULL;
	char tempstr[32];

	if(PlaneId != -1)
		P = &Planes[PlaneId];

	while(Index < NumMisslesChasing)
	{
		float YPos;

		if(Cm->Target == P)
		{
			if(Cm->EstimatedTime <= 0)
				YPos = ScrY;
			else
			{
				Frac = (float)(GameLoopInTicks - Cm->TimeFired)/(float)(Cm->EstimatedTime);
				if(Frac > 1.0) Frac = 1.0;
				YPos = (AA_RADAR_BOTTOM - 4) - (( (AA_RADAR_BOTTOM - 4) - ScrY)*Frac);
			}

			DrawTriangleRot(ScrX,YPos,4,0,AV_GREEN,1.0,0);

			if(Cm->WeaponPage == AIM120_WEAP_PAGE)
			{
				long lworkticks = (long)Cm->ActivateTime - (GameLoopInTicks - (long)Cm->TimeFired);
				if(lworkticks <= 0)
				{
					DrawTextAtLoc(ScrX - 2,AA_RADAR_BOTTOM - 4, "A",AV_GREEN,1.0);
				}
				else
				{
					lworkticks /= 1000;
					if(lworkticks > 60)
						lworkticks = 60;
					sprintf(tempstr, "%ld", lworkticks);
					if(lworkticks <= 9)
					{
						DrawTextAtLoc(ScrX - 2,AA_RADAR_BOTTOM - 4, tempstr, AV_GREEN,1.0);
					}
					else
					{
						DrawTextAtLoc(ScrX - 4,AA_RADAR_BOTTOM - 4, tempstr, AV_GREEN,1.0);
					}
				}
			}

			if(YPos == ScrY)
				RemoveMissleFromChasingMissleList(Index);
			else
			{
				Index++;
				Cm++;
			}
		}
		else
		{
			Index++;
			Cm++;
		}
	} // while
}

//*****************************************************************************************************************************************
void DrawPlaneVelocityVector(float X,float Y,float Heading,int Color, float Shade)
{
	float NewX1,NewY1;
	float NewX2,NewY2;

	AvRGB Triple;

	GetRGBFromAVPalette(Color,Shade,&Triple);

	float DeltaAzim = NormDegree(Heading + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));

	ProjectPoint(X, Y,NormDegree(DeltaAzim),4,&NewX1,&NewY1);
	ProjectPoint(X, Y,NormDegree(DeltaAzim),10,&NewX2,&NewY2);

	GrDrawLine(GrBuffFor3D,NewX1,NewY1,NewX2,NewY2,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************

#define HAFUGETSETTER(x) (x&AA_ON_RADAR),(x&AA_AWACS)

void DrawHAFU(float X, float Y,int Type,int radar,int awacs,int IDLevel,int Rank, float Heading, PlaneParams *P, int do_heading_line = 1,float fade = 1.0f,int iAOT=0)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,fade,&Triple);

	if(Type & AA_PRIMARY)
		DrawAvRadarSymbol(X,Y,AV_AA_CLOSED_STAR,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	if(Type & AA_SECONDARY)
		DrawAvRadarSymbol(X,Y,AV_AA_CLOSED_DIAMOND,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	if(Type & AA_RANKED)
	{
		sprintf(TmpStr,"%d",Rank);
		DrawTextAtLoc(X-1,Y-2,TmpStr,AV_GREEN,fade);
	}

	if ((Type & (AA_PRIMARY | AA_SECONDARY)) && !iAOT && (fade == 1.0f) )
	{
		if(Av.AARadar.Declutter < 2)
		{
			sprintf(TmpStr,"%2.1f",P->Mach);
			DrawTextAtLoc(X-16,Y-2,TmpStr,AV_GREEN,fade);
			sprintf(TmpStr,"%d",(int)(P->Altitude/1000));
			DrawTextAtLoc(X+6,Y-2,TmpStr,AV_GREEN,fade);
		}
	}

	switch(IDLevel)
	{
		case AA_HOSTILE:
			if(radar)
			{
		  		GrDrawLine(GrBuffFor3D,X-4,Y,X,Y-4,Triple.Red,Triple.Green,Triple.Blue,0);
		  		GrDrawLine(GrBuffFor3D,X+4,Y,X,Y-4,Triple.Red,Triple.Green,Triple.Blue,0);
			}

			if(awacs)
			{
				GrDrawLine(GrBuffFor3D,X-4,Y,X,Y+4,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+4,Y,X,Y+4,Triple.Red,Triple.Green,Triple.Blue,0);
			}

			break;

		case AA_UNKNOWN:
			if(radar)
			{
				GrDrawLine(GrBuffFor3D,X-3,Y-3,X+4,Y-3,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X-3,Y-3,X-3,Y,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+4,Y-3,X+4,Y,Triple.Red,Triple.Green,Triple.Blue,0);
			}

			if(awacs)
			{
				GrDrawLine(GrBuffFor3D,X-3,Y+4,X+4,Y+4,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X-3,Y+4,X-3,Y,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+4,Y+4,X+4,Y,Triple.Red,Triple.Green,Triple.Blue,0);
			}

			break;

		case AA_FRIENDLY:
			if(radar)
			{
				GrDrawLine(GrBuffFor3D,X+4,Y,X+4,Y-2,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+4,Y-2,X+2,Y-4,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+2,Y-4,X-2,Y-4,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X-2,Y-4,X-4,Y-2,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X-4,Y-2,X-4,Y,Triple.Red,Triple.Green,Triple.Blue,0);
			}

			if(awacs)
			{
				GrDrawLine(GrBuffFor3D,X-4,Y,X-4,Y+2,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X-4,Y+2,X-2,Y+4,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X-2,Y+4,X+2,Y+4,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+2,Y+4,X+4,Y+2,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,X+4,Y+2,X+4,Y,Triple.Red,Triple.Green,Triple.Blue,0);
			}

			break;
	}

	if ((Av.AARadar.Declutter < 2) && (!iAOT) && ((Type & (AA_PRIMARY | AA_SECONDARY)) || do_heading_line))
		DrawPlaneVelocityVector(X,Y,Heading,AV_GREEN,fade);

}

//*****************************************************************************************************************************************
void DrawPlanesInList(DetectedPlaneListType *PList,int draw_heading_line)
{
	float X,Y;
	int   Color;
	float Shade;
	PlaneParams *P;
	BOOL history = FALSE;
	DetectedPlaneType *dp;

	if( (PList != &CurFramePlanes) && (PList != &PrevFramePlanes) )
		history = TRUE;

	dp = &PList->Planes[PList->NumPlanes];

	while(dp-- > PList->Planes)
	{
		if (dp->ShouldDraw && ((Av.AARadar.MSI == ON) || (dp->Designation & AA_ON_RADAR)))
		{
			P = &Planes[dp->PlaneId];

			X = dp->OfsX;
			Y = dp->OfsY;

			Color = dp->Color;
			Shade = dp->Shade;

			if (history)
				Shade = PList->ColorFadePercent;

				// for IFF
			if(AA1SecBlinkState && (dp->IDLevel == AA_FRIENDLY) && !(dp->Designation & AA_AWACS) && ( (Av.AARadar.CurMode == AA_RWS_MODE) || (Av.AARadar.CurMode == AA_VS_MODE) ) )
			{
				DrawHsiCircle(dp->OfsX,dp->OfsY, Color, Shade);
				continue;
			}

			if(dp->PlaneId == Primary->Id)
			{
				if (!history)
				{
					if ((Av.AARadar.CurMode != AA_VS_MODE) && (Primary->Id !=-1) && (!dp->AOT))
						DrawTargetProfile(X);
					DrawMissles(X,Y,dp->PlaneId);
				}
				DrawHAFU(X,Y,dp->Designation,HAFUGETSETTER(dp->Designation),dp->IDLevel,-1,dp->Heading,P,draw_heading_line,Shade,dp->AOT);
			}
		   	else
				if(dp->PlaneId == Secondary->Id)
				{
					if((Av.AARadar.Declutter < 2) && !history)
					{
						if ((Av.AARadar.CurMode != AA_VS_MODE) && (Primary->Id !=-1) && (!dp->AOT))
							DrawTargetProfile(X);
						DrawMissles(X,Y,dp->PlaneId);
					}
					DrawHAFU(X,Y,dp->Designation,HAFUGETSETTER(dp->Designation),dp->IDLevel,-1,dp->Heading,P,draw_heading_line,Shade,dp->AOT);
				}
				else
				{
					int d = dp->Designation;

					if (Av.AARadar.CurMode != AA_TWS_MODE)
						DrawRadarBlip(X,Y,Color,Shade);
					else
						if (!(d & AA_RANKED))
	      					DrawAvDot(X,Y,Color,Shade);

					if (Av.AARadar.CurMode != AA_VS_MODE)
					{

						if (Av.AARadar.CurMode == AA_RWS_MODE)
							d &= ~AA_ON_RADAR;

						DrawHAFU(X,Y,d,HAFUGETSETTER(d),dp->IDLevel,dp->Rank,dp->Heading,P,draw_heading_line,Shade,dp->AOT);

					}
				}
	   	}
   }
}

//*****************************************************************************************************************************************
void DrawTargetedPlane(BOOL aot)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if((Primary->Id != -1))// && CurFramePlanes.NumPlanes)
	{
//		int Friendly = (Planes[Primary->Id].AI.iSide == AI_FRIENDLY) ? AA_FRIENDLY : AA_UNKNOWN;

//		if ((Friendly == AA_UNKNOWN) && (Planes[Primary->Id].AI.iAICombatFlags2 & AI_AWACS_DETECT))
//			Friendly == AA_ENEMY;

 		if(Primary->ShouldDraw)
		{
			DrawHAFU(Primary->XOfs,Primary->YOfs,AA_PRIMARY,TRUE,FALSE,Primary->IDLevel,-1,( (Primary->Reaquire.BrokeLockReaquire) || (Primary->Jammer.JammerFakeOut) ) ? Primary->Reaquire.Heading : Planes[Primary->Id].Heading/DEGREE,&Planes[Primary->Id],TRUE,1.0f,aot);

			DrawMissles(Primary->XOfs,Primary->YOfs,Primary->Id);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawPlanes()
{
  //DrawPlanesInList(&Age1Planes,FALSE);

  DetectedPlaneListType *P = &AgePlanes[0];
  int Index = AA_MAX_AGE_PLANES;
  while(Index-- > 0)
  {
		if(P->NumPlanes > 0)
			DrawPlanesInList(P,FALSE);

		P++;
	}

  DrawPlanesInList(&CurFramePlanes,FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTWSPlanes()
{
  //DrawPlanesInList(&Age1Planes,FALSE);

  DetectedPlaneListType *P = &AgePlanes[0];
  int Index = AA_MAX_AGE_PLANES;
  while(Index-- > 0)
  {
		if(P->NumPlanes > 0)
			DrawPlanesInList(P,TRUE);

		P++;
	}

  DrawPlanesInList(&PrevFramePlanes,TRUE);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawVelocityVector()
{
//  if(Av.AARadar.Declutter > 0) return;

	/*
	int		  PosX, PosY;
	FPoint	VelVector;

	if (P->Knots < 1)  return;

	VelVector.X =  P->BfLinVel.Y;
	VelVector.Y = -P->BfLinVel.Z;
  VelVector.Z = -P->BfLinVel.X;

	VelVector.Perspect( &PosX, &PosY );

	int NrmlX = PosX - GrBuffFor3D->MidX;
	int NrmlY = PosY - GrBuffFor3D->MidY - 20;

	if (hypot( NrmlX, NrmlY ) > 85)				// 70 screen pizels = 8.8° HUD
	{
		int cycle = GetTickCount() - VxV_mark;
												// flash @ 2Hz w/ 50% duty cycle
		if (cycle < 250)  return;				// OFF first half, then ON
		if (cycle > 500)  VxV_mark += cycle;	// set for next cycle
	}

	 // Translate virtual cockpit from physical screen center to HUD center
	//
	if (CurrentView & COCKPIT_VIRTUAL_SEAT)
	{
		PosX += HUD_MIDX + TranX - GrBuffFor3D->MidX;
		PosY += HUD_MIDY + TranY - GrBuffFor3D->MidY;
	}

 	VelVectorX = PosX;		// save position for VV related symbols
	VelVectorY = PosY;

	if (CheckInHUD( PosX, PosY ))
		CRTicon( PosX, PosY, SM_VELOCITY_VECTOR );
	 *
	 */
}

//*****************************************************************************************************************************************
void DrawDiffAltitude()
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	float Frac = Av.AARadar.AntElevation/70.0;
	float YPos =	LERP(AA_RADAR_CENTER_Y,AA_RADAR_TOP,Frac);

	if(Av.AARadar.Declutter == 2) return;

	if(Primary->Id == -1) return;
	int Index;
	if(Av.AARadar.CurMode != AA_STT_MODE)
	{
		Index = GetPlanePosInList(&CurFramePlanes,Primary->Id);
		if(Index != -1)
			Index = CurFramePlanes.Planes[Index].PlaneId;
		else
			if (CurFramePlanes.NumPlanes)
				return;
			else
				Index = Primary->Id;
	} else
		Index = Primary->Id;

	float AltDiff = (Planes[Index].WorldPosition.Y - PlayerPlane->WorldPosition.Y)*WUTOFT;
	AltDiff /= 1000.0;

	sprintf(TmpStr,"%2.1f",AltDiff);
	DrawTextAtLoc(AA_RADAR_LEFT + 5,YPos - 2,TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
void DrawPrimaryRelVelCaret(BOOL CalcVelocity = FALSE)
{
	if((Av.AARadar.Declutter < 2) && (Primary->Id != -1))
	{
		AvRGB Triple;
		GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);
		float Vc;
		float Y;
		BOOL found_place = FALSE;

		if(Av.AARadar.CurMode == AA_STT_MODE)
		{
			Y = Primary->YOfs;
			Vc = CalcRelativeVelocity(Primary->Id);
		}
		else
		{
			if (CurFramePlanes.NumPlanes)
			{
				DetectedPlaneType *P = &CurFramePlanes.Planes[CurFramePlanes.NumPlanes];
				while(P-- > CurFramePlanes.Planes)
				{
					if(P->PlaneId == Primary->Id)
					{
						Y = P->OfsY;
						found_place = TRUE;
						break;
					}
				}

				if (found_place)
					Vc = CalcRelativeVelocity(Primary->Id);
				else
					return;
			}
			else
			{
				float YFrac;

				PlaneParams *p = &Planes[Primary->Id];
				if(!CalcVelocity)
				{
					YFrac = ((PlayerPlane->WorldPosition / p->WorldPosition)*WUTONM)/Av.AARadar.Range;
					Vc = CalcRelativeVelocity(Primary->Id);
				}
	    		else
	    		{
					Vc = CalcRelativeVelocity(Primary->Id);
					YFrac = Vc/Av.AARadar.VsLimit;                      // knts
					if(YFrac > 1.0)
						YFrac = 1.0;
	    		}

				Y = LERP(AA_RADAR_BOTTOM,AA_RADAR_TOP,YFrac);
			}

		}

		sprintf(TmpStr,"%d",(int)Vc);

		if(Av.AARadar.RaidOn)
			Y=AA_RADAR_CENTER_Y;

		GrDrawLine(GrBuffFor3D,AA_RADAR_RIGHT,Y,AA_RADAR_RIGHT - 3,Y-3,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,AA_RADAR_RIGHT,Y,AA_RADAR_RIGHT - 3,Y+3,Triple.Red,Triple.Green,Triple.Blue,0);
		RightJustifyTextAtLocation(AA_RADAR_RIGHT - 4,Y - 2,TmpStr,Triple.Red,Triple.Green,Triple.Blue);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawOwnShipAltitude()
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"%d",(int)(PlayerPlane->WorldPosition.Y*WUTOFT));
	RightJustifyTextAtLocation(AA_RADAR_RIGHT - 5,AA_RADAR_BOTTOM + 2,TmpStr,Triple.Red,Triple.Green,Triple.Blue);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawVelVec()
{
  if(Av.AARadar.Declutter > 0) return;

	/*
	int		  PosX, PosY;
	FPoint	VelVector;

	if (P->Knots < 1)  return;

	VelVector.X =  P->BfLinVel.Y;
	VelVector.Y = -P->BfLinVel.Z;
  VelVector.Z = -P->BfLinVel.X;

	VelVector.Perspect( &PosX, &PosY );

	int NrmlX = PosX - GrBuffFor3D->MidX;
	int NrmlY = PosY - GrBuffFor3D->MidY - 20;

	if (hypot( NrmlX, NrmlY ) > 85)				// 70 screen pizels = 8.8° HUD
	{
		int cycle = GetTickCount() - VxV_mark;
												// flash @ 2Hz w/ 50% duty cycle
		if (cycle < 250)  return;				// OFF first half, then ON
		if (cycle > 500)  VxV_mark += cycle;	// set for next cycle
	}

	 // Translate virtual cockpit from physical screen center to HUD center
	//
	if (CurrentView & COCKPIT_VIRTUAL_SEAT)
	{
		PosX += HUD_MIDX + TranX - GrBuffFor3D->MidX;
		PosY += HUD_MIDY + TranY - GrBuffFor3D->MidY;
	}

 	VelVectorX = PosX;		// save position for VV related symbols
	VelVectorY = PosY;

	if (CheckInHUD( PosX, PosY ))
		CRTicon( PosX, PosY, SM_VELOCITY_VECTOR );
	 *
	 */
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

extern SIZE ScreenSize;

void DrawAttitudeIndicator(float X,float Y)
{
	float XOfs = X;
	float YOfs = Y;

	signed short TmpPitch = (signed short)((signed short)PlayerPlane->Pitch/(10));
	ANGLE TmpRoll =  PlayerPlane->Roll;
	extern inline float RadToDeg(float Rad);
	ANGLE TmpHeading = -RadToDeg(PlayerPlane->Beta)*DEGREE;

	if ((signed short)TmpPitch > (signed short)(5*DEGREE)) TmpPitch = (signed short)(5*DEGREE);
	else
	if ((signed short)TmpPitch < -(signed short)(5*DEGREE)) TmpPitch = -(signed short)(5*DEGREE);

	FMatrix	 RadarAttitude;
	RadarAttitude.SetHPR(0,-TmpPitch,-TmpRoll);

	for (int i=0; i<6; i++)
	{
		AirToAirLadder.RadarPoints[i].Rotated.RotateAndPerspect((int *)&AirToAirLadder.RadarPoints[i].iSX,
																(int *)&AirToAirLadder.RadarPoints[i].iSY,
																	   &RadarAttitude,
																	   &AirToAirLadder.RawRadarPoint[i]);

		// Translate Rotated points to corrent MPD
		AirToAirLadder.RadarPoints[i].iSX -= GrBuffFor3D->MidX-XOfs;//320
		AirToAirLadder.RadarPoints[i].iSY -= GrBuffFor3D->MidY-YOfs;//116
	}

	GrDrawLine(GrBuffFor3D, AirToAirLadder.RadarPoints[4].iSX, AirToAirLadder.RadarPoints[4].iSY,
			   			AirToAirLadder.RadarPoints[0].iSX, AirToAirLadder.RadarPoints[0].iSY, 1,140,1,0);

	GrDrawLine(GrBuffFor3D, AirToAirLadder.RadarPoints[0].iSX, AirToAirLadder.RadarPoints[0].iSY,
			   			AirToAirLadder.RadarPoints[1].iSX, AirToAirLadder.RadarPoints[1].iSY, 1,140,1,0);

	GrDrawLine(GrBuffFor3D, AirToAirLadder.RadarPoints[2].iSX, AirToAirLadder.RadarPoints[2].iSY,
			   			AirToAirLadder.RadarPoints[3].iSX, AirToAirLadder.RadarPoints[3].iSY, 1,140,1,0);

	GrDrawLine(GrBuffFor3D, AirToAirLadder.RadarPoints[3].iSX, AirToAirLadder.RadarPoints[3].iSY,
							AirToAirLadder.RadarPoints[5].iSX, AirToAirLadder.RadarPoints[5].iSY, 1,140,1,0);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAARadarBackGround()
{
	// BORDER
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,AA_RADAR_EXTREME_TOP,AA_RADAR_RIGHT,AA_RADAR_EXTREME_TOP,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,AA_RADAR_TOP,AA_RADAR_RIGHT,AA_RADAR_TOP,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,AA_RADAR_EXTREME_TOP,AA_RADAR_LEFT,AA_RADAR_BOTTOM,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,AA_RADAR_BOTTOM,AA_RADAR_RIGHT,AA_RADAR_BOTTOM,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_RIGHT,AA_RADAR_BOTTOM,AA_RADAR_RIGHT,AA_RADAR_EXTREME_TOP,1,140,1,0);

	// TOP NOTCHES
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_0X,AA_RADAR_TOP,TOP_NOTCH_0X,AA_RADAR_TOP + TOP_NOTCH_LENGTH,1,140,1,0);
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_1X,AA_RADAR_TOP,TOP_NOTCH_1X,AA_RADAR_TOP + TOP_NOTCH_LENGTH,1,140,1,0);
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_2X,AA_RADAR_TOP,TOP_NOTCH_2X,AA_RADAR_TOP + TOP_NOTCH_LENGTH + 3,1,140,1,0); // CENTER
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_3X,AA_RADAR_TOP,TOP_NOTCH_3X,AA_RADAR_TOP + TOP_NOTCH_LENGTH,1,140,1,0);
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_4X,AA_RADAR_TOP,TOP_NOTCH_4X,AA_RADAR_TOP + TOP_NOTCH_LENGTH,1,140,1,0);;

	// BOTTOM NOTCHES
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_0X,AA_RADAR_BOTTOM,TOP_NOTCH_0X,AA_RADAR_BOTTOM - TOP_NOTCH_LENGTH,1,140,1,0);
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_1X,AA_RADAR_BOTTOM,TOP_NOTCH_1X,AA_RADAR_BOTTOM - TOP_NOTCH_LENGTH,1,140,1,0);
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_2X,AA_RADAR_BOTTOM,TOP_NOTCH_2X,AA_RADAR_BOTTOM - TOP_NOTCH_LENGTH - 3,1,140,1,0); // CENTER
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_3X,AA_RADAR_BOTTOM,TOP_NOTCH_3X,AA_RADAR_BOTTOM - TOP_NOTCH_LENGTH,1,140,1,0);
	GrDrawLine(GrBuffFor3D,TOP_NOTCH_4X,AA_RADAR_BOTTOM,TOP_NOTCH_4X,AA_RADAR_BOTTOM - TOP_NOTCH_LENGTH,1,140,1,0);

	// LEFT SIDE NOTCHES
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_0Y,AA_RADAR_LEFT + SIDE_NOTCH_WIDTH,SIDE_NOTCH_0Y,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_1Y,AA_RADAR_LEFT + SIDE_NOTCH_WIDTH,SIDE_NOTCH_1Y,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_2Y,AA_RADAR_LEFT + SIDE_NOTCH_WIDTH,SIDE_NOTCH_2Y,1,140,1,0);

	// RIGHT SIDE NOTCHES
	GrDrawLine(GrBuffFor3D,AA_RADAR_RIGHT,SIDE_NOTCH_0Y,AA_RADAR_RIGHT - SIDE_NOTCH_WIDTH,SIDE_NOTCH_0Y,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_RIGHT,SIDE_NOTCH_1Y,AA_RADAR_RIGHT - SIDE_NOTCH_WIDTH,SIDE_NOTCH_1Y,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_RIGHT,SIDE_NOTCH_2Y,AA_RADAR_RIGHT - SIDE_NOTCH_WIDTH,SIDE_NOTCH_2Y,1,140,1,0);

	float SmallNotch1 = SMALL_SIDE_NOTCH_SPACE;

	float SmallNotch2 = SIDE_NOTCH_0Y;
	float SmallNotch3 = SIDE_NOTCH_1Y;
	float SmallNotch4 = (SmallNotch2 - SmallNotch3)/3.0;

	// SMALL LEFT SIDE NOTCHES
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_0Y - SMALL_SIDE_NOTCH_SPACE,AA_RADAR_LEFT - SMALL_SIDE_NOTCH_WIDTH,SIDE_NOTCH_0Y - SMALL_SIDE_NOTCH_SPACE,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_0Y - (2.0*SMALL_SIDE_NOTCH_SPACE),AA_RADAR_LEFT - SMALL_SIDE_NOTCH_WIDTH,SIDE_NOTCH_0Y - (2.0*SMALL_SIDE_NOTCH_SPACE),1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_1Y - SMALL_SIDE_NOTCH_SPACE,AA_RADAR_LEFT - SMALL_SIDE_NOTCH_WIDTH,SIDE_NOTCH_1Y - SMALL_SIDE_NOTCH_SPACE,1,140,1,0);
	GrDrawLine(GrBuffFor3D,AA_RADAR_LEFT,SIDE_NOTCH_1Y - (2.0*SMALL_SIDE_NOTCH_SPACE),AA_RADAR_LEFT - SMALL_SIDE_NOTCH_WIDTH,SIDE_NOTCH_1Y - (2.0*SMALL_SIDE_NOTCH_SPACE),1,140,1,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawRadarBlip(float X, float Y, int Ramp, float Shade)
{
  AvRGB Color;

  GetRGBFromAVPalette(Ramp,Shade,&Color);

  GrDrawLineClipped(GrBuffFor3D,X-2,Y-1,X+2,Y-1,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLineClipped(GrBuffFor3D,X-2,Y,X+2,Y,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLineClipped(GrBuffFor3D,X-2,Y+1,X+2,Y+1,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
void DrawCaptBars(float X, float Y, int Ramp, float Shade)
{
	AvRGB Color;

	GetRGBFromAVPalette(Ramp,Shade,&Color);

	GrDrawLine(GrBuffFor3D,X-3,Y-3,X-3,Y+3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,X-4,Y-3,X-4,Y+3,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,X+3,Y-3,X+3,Y+3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,X+4,Y-3,X+4,Y+3,Color.Red,Color.Green,Color.Blue,0);

	if(UFC.MasterMode != AG_MODE)
	{
		float High,Low,Range;

		Range = (Av.AARadar.Range)*((AA_RADAR_BOTTOM - Y)/(AA_RADAR_BOTTOM - AA_RADAR_TOP));

		GetAltHeightsAtRange(Range,&High,&Low);

		if(High < 0) High = 0;
		if(Low < 0)  Low = 0;

		sprintf(TmpStr,"%d",(int)(High/1000));
		DrawTextAtLoc(X - 2,Y - 11,TmpStr,AV_GREEN,1.0);
		sprintf(TmpStr,"%d",(int)(Low/1000));
		DrawTextAtLoc(X - 2,Y + 5,TmpStr,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
void DrawAntennaLine()
{
	float Frac;
	float XPos;

	if(Av.AARadar.RaidOn) return;

	Frac = RLAngle(Av.AARadar.AntAzimuth + Av.AARadar.SlewLeftRight);

	if (fabs(Frac) < 70.0f)
	{
		Frac = (Frac - 70.0f) *(-1.0f/140.0f);

		XPos = LERP(AA_RADAR_LEFT,AA_RADAR_RIGHT, Frac);

		GrDrawLine(GrBuffFor3D,XPos,AA_RADAR_BOTTOM,XPos,AA_RADAR_TOP,1,140,1,0);
	}
}

//*****************************************************************************************************************************************
void DrawElevCaret()
{
	float Frac = (Av.AARadar.AntElevation + Av.AARadar.SlewUpDown)/70.0;
	float YPos =	LERP(AA_RADAR_CENTER_Y,AA_RADAR_TOP,Frac);

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DrawAvRadarSymbol(AA_RADAR_LEFT,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);
}

//*****************************************************************************************************************************************
char *NctrVis[] = { "FTR", "HELO", "ATTK", "LRG" };

void DrawNCTRText(int MpdNum)
{
	if(!Av.AARadar.NCTROn) return;

	if(Primary->NCTRPercent < 50)
	{
		// sprintf(TmpStr,"UNK %d",(int)Primary->NCTRPercent);

		DrawTextAtLoc(85,3,"UNK",AV_GREEN,1.0);

		sprintf(TmpStr,"%d%",(int)Primary->NCTRPercent);
		DrawTextAtLoc(85,9,TmpStr,AV_GREEN,1.0);
	}
	else if(Primary->NCTRPercent < 80)
	{
		int Index = Planes[Primary->Id].AI.iPlaneIndex;
		int Vis   = pDBAircraftList[Index].iVisibility;

		if(Vis > 3) Vis = 3;
		if(Vis < 0) Vis = 0;

		//sprintf(TmpStr,"%s %d",NctrVis[Vis],(int)Primary->NCTRPercent);

		sprintf(TmpStr,"%s",NctrVis[Vis]);
		DrawTextAtLoc(85,3,TmpStr,AV_GREEN,1.0);
		sprintf(TmpStr,"%d%",(int)Primary->NCTRPercent);
		DrawTextAtLoc(85,9,TmpStr,AV_GREEN,1.0);
	}
	else
	{
		int Index  = Planes[Primary->Id].AI.iPlaneIndex;
		char *Str  = pDBAircraftList[Index].sNCTR;

		// sprintf(TmpStr,"%s %d",Str,(int)Primary->NCTRPercent);

		sprintf(TmpStr,"%s",Str);
		DrawTextAtLoc(85,3,TmpStr,AV_GREEN,1.0);
		sprintf(TmpStr,"%d%",(int)Primary->NCTRPercent);
		DrawTextAtLoc(85,9,TmpStr,AV_GREEN,1.0);
	}
	//DrawWordAtMpdButton(MpdNum,16,TmpStr,FALSE,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
void DrawAAWeaponInfo()
{
	 AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

	 if(CurAAWeap != NULL)
	 {
		 int Found = TRUE;
		 AvWeapEntryType *Weap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

		 switch(Weap->W->WeapId)
		 {
		  case AA_7F:  sprintf(TmpStr,"7F %d",Weap->W->Count);  break;
		  case AA_7M : sprintf(TmpStr,"7M %d",Weap->W->Count);  break;
		  case AA_7MH: sprintf(TmpStr,"7MH %d",Weap->W->Count); break;
		  case AA_9L:  sprintf(TmpStr,"9L %d",Weap->W->Count);  break;
		  case AA_9M:  sprintf(TmpStr,"9M %d",Weap->W->Count);  break;
		  case AA_9X:  sprintf(TmpStr,"9X %d",Weap->W->Count);  break;
		  case AA_120: sprintf(TmpStr,"AM %d",Weap->W->Count);  break;
		  default:
				Found = FALSE;
		 }

		 if(Found)
		 {
			 DrawTextAtLoc(AA_RADAR_RIGHT - 24,AA_RADAR_EXTREME_TOP - 7,TmpStr,AV_GREEN,1.0);
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawMdiArrow(float X, float Y, int Dir)
{
	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(Dir == 0) // up
	{
	  GrDrawLine(GrBuffFor3D,X,Y,X,Y+8,Triple.Red,Triple.Green,Triple.Blue,0);
	  GrDrawLine(GrBuffFor3D,X-1,Y+1,X+1,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
	  GrDrawLine(GrBuffFor3D,X-2,Y+2,X+2,Y+2,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	else
	{
	  GrDrawLine(GrBuffFor3D,X,Y,X,Y-8,Triple.Red,Triple.Green,Triple.Blue,0);
	  GrDrawLine(GrBuffFor3D,X-1,Y-1,X+1,Y-1,Triple.Red,Triple.Green,Triple.Blue,0);
	  GrDrawLine(GrBuffFor3D,X-2,Y-2,X+2,Y-2,Triple.Red,Triple.Green,Triple.Blue,0);
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawIronCross(float X, float Y)
{
	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	FPoint P1,P2,P3,P4,P5,P6,P7,P8;

	P1.X = X - 12;
	P1.Y = Y + 1;

	P2.X = X - 12;
	P2.Y = Y - 1;

	P3.X = X - 1;
	P3.Y = Y - 12;

	P4.X = X + 1;
	P4.Y = Y - 12;

	P5.X = X + 12;
	P5.Y = Y - 1;

	P6.X = X + 12;
	P6.Y = Y + 1;

	P7.X = X + 1;
	P7.Y = Y + 12;

	P8.X = X - 1;
	P8.Y = Y + 12;

	GrDrawLine(GrBuffFor3D,P1.X,P1.Y,P2.X,P2.Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,P3.X,P3.Y,P4.X,P4.Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,P5.X,P5.Y,P6.X,P6.Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,P7.X,P7.Y,P8.X,P8.Y,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,P1.X,P1.Y,P5.X,P5.Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,P2.X,P2.Y,P6.X,P6.Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,P3.X,P3.Y,P7.X,P7.Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,P4.X,P4.Y,P8.X,P8.Y,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawRWSModeText(int MpdNum,BOOL aot)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"RWS");
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",Av.AARadar.Range);
	DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AARadar.AzimLimit*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// RSET
	if(Av.AARadar.RsetTimer.IsActive())
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
	 if(Av.AARadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AARadar.RsetTimer.Disable();
	}
	else
	  DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);

	// NCTR
	DrawVerticalWordAtButton(MpdNum,10,"NCTR",(Av.AARadar.NCTROn),AV_GREEN,1.0);

	// PRF
	switch(Av.AARadar.SearchPrf)
	{
		case SEARCH_I_MODE: sprintf(TmpStr,"INT"); break;
		case SEARCH_H_MODE: sprintf(TmpStr,"HI");  break;
		case SEARCH_M_MODE: sprintf(TmpStr,"MED"); break;
	}
	DrawWordAtMpdButton(MpdNum,4,TmpStr,FALSE,AV_GREEN,1.0);

	//SILENT
	DrawWordAtMpdButton(MpdNum,18,"SIL",(Av.AARadar.SilentOn),AV_GREEN,1.0);

	//BARS
	sprintf(TmpStr,"%dB",Av.AARadar.Bars);
	DrawWordAtMpdButton(MpdNum,19,TmpStr,FALSE,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AA_RADAR_LEFT - 12,AA_RADAR_EXTREME_TOP - 6,( (Av.AARadar.SilentOn) || (UFC.EMISState) || PlayerPlane->OnGround ) ? "STDBY" : "OPR",AV_GREEN,1.0); //was 7

	//ERASE
	DrawWordAtMpdButton(MpdNum,17,"ERASE",FALSE,AV_GREEN,1.0);

	// FADE FREQ
	sprintf(TmpStr,"%d",(int)Av.AARadar.AgeFrequency);
	DrawWordAtMpdButton(MpdNum,15,TmpStr,FALSE,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
	DrawTextAtLoc(AA_RADAR_LEFT - 4,AA_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	// OWNSHIP ALT
	DrawOwnShipAltitude();

	// DECLUTTER
	switch(Av.AARadar.Declutter)
	{
		case 0: sprintf(TmpStr,"DCLTR");break;
		case 1: sprintf(TmpStr,"DCLTR1"); break;
		case 2: sprintf(TmpStr,"DLCTR2"); break;
	}
	DrawWordAtMpdButton(MpdNum,8,TmpStr,(Av.AARadar.Declutter > 0) ? TRUE : FALSE,AV_GREEN,1.0);

	// OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
	DrawTextAtLoc(RADAR_HEADING_X,AA_RADAR_EXTREME_TOP - 6,TmpStr,AV_GREEN,1.0);

	// NCTR
	DrawNCTRText(MpdNum);

	// MCI
	DrawWordAtMpdButton(MpdNum,9,"MSI",(Av.AARadar.MSI == ON),AV_GREEN,1.0);

	if (Av.AARadar.Declutter<2)
	{
		if((Primary->Id != -1) && !aot)
		{
			sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (Planes[Primary->Id].Heading/DEGREE))));
			DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_TOP + 12,TmpStr,AV_GREEN,1.0);
		}
	}

	DrawMdiArrow(137,32,0);
	DrawMdiArrow(137,52,1);

	DrawSelectedWeapon();

	DrawRadarSense();
}

//*****************************************************************************************************************************************
void DrawRaidRange()
{
	if (Primary->Id!=-1)
	{
		sprintf(TmpStr,"%2.f",RaidNear+RaidDistRange);
		DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_TOP,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%2.f",RaidNear);
		DrawTextAtLoc(AA_RADAR_RIGHT + 1,AA_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
void DrawTWSModeText(int MpdNum,BOOL aot)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"TWS");
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);


	if(!Av.AARadar.RaidOn)
	{
		// RANGE
		sprintf(TmpStr,"%d",Av.AARadar.Range);
		DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

		// PRF
		switch(Av.AARadar.SearchPrf)
		{
			case SEARCH_I_MODE:
				sprintf(TmpStr,"INT");
			break;
			case SEARCH_H_MODE:
				sprintf(TmpStr,"HI");
			break;
			case SEARCH_M_MODE:
				sprintf(TmpStr,"MED");
				break;
		}
		DrawWordAtMpdButton(MpdNum,4,TmpStr,FALSE,AV_GREEN,1.0);

		//BARS
		sprintf(TmpStr,"%dB",Av.AARadar.Bars);
		DrawWordAtMpdButton(MpdNum,19,TmpStr,FALSE,AV_GREEN,1.0);

		// FADE FREQ
		sprintf(TmpStr,"%d",(int)Av.AARadar.AgeFrequency);
		DrawWordAtMpdButton(MpdNum,15,TmpStr,FALSE,AV_GREEN,1.0);

		// MCI
		DrawWordAtMpdButton(MpdNum,9,"MSI",(Av.AARadar.MSI == ON),AV_GREEN,1.0);

		DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_CENTER_Y - 8,"AUTO",AV_GREEN,1.0);
		if(Av.AARadar.AutoOn)
			DrawBoxAroundWord("AUTO",AA_RADAR_RIGHT + 2,AA_RADAR_CENTER_Y - 8,Triple.Red,Triple.Green,Triple.Blue);

		DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_CENTER_Y + 2,"MAN",AV_GREEN,1.0);
		if(!Av.AARadar.AutoOn)
			DrawBoxAroundWord("MAN",AA_RADAR_RIGHT + 2,AA_RADAR_CENTER_Y + 2,Triple.Red,Triple.Green,Triple.Blue);


	}
	else
	{
		DrawTextAtLoc(AA_RADAR_CENTER_X - 18,AA_RADAR_BOTTOM - 7,"SCAN RAID",AV_GREEN,1.0);
		DrawRaidRange();
	}


	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AARadar.AzimLimit*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// RSET
	if(Av.AARadar.RsetTimer.IsActive())
	{
		DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
		if(Av.AARadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AARadar.RsetTimer.Disable();
	}
	else
		DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);

	// NCTR
	DrawVerticalWordAtButton(MpdNum,10,"NCTR",(Av.AARadar.NCTROn),AV_GREEN,1.0);

	//SILENT
	DrawWordAtMpdButton(MpdNum,18,"SIL",(Av.AARadar.SilentOn),AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AA_RADAR_LEFT - 12,AA_RADAR_EXTREME_TOP - 6,( (Av.AARadar.SilentOn) || (UFC.EMISState) || PlayerPlane->OnGround ) ? "STDBY" : "OPR",AV_GREEN,1.0);

	//ERASE
	DrawWordAtMpdButton(MpdNum,17,"ERASE",FALSE,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWNSHIP ALT
	DrawOwnShipAltitude();

	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
	DrawTextAtLoc(AA_RADAR_LEFT - 4,AA_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	// OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AA_RADAR_EXTREME_TOP - 6,TmpStr,AV_GREEN,1.0);

	// DECLUTTER
	switch(Av.AARadar.Declutter)
	{
		case 0: sprintf(TmpStr,"DCLTR");break;
		case 1: sprintf(TmpStr,"DCLTR1"); break;
		case 2: sprintf(TmpStr,"DLCTR2"); break;
	}
	DrawWordAtMpdButton(MpdNum,8,TmpStr,(Av.AARadar.Declutter > 0),AV_GREEN,1.0);

	// NCTR
	DrawNCTRText(MpdNum);

	// RAID
	DrawVerticalWordAtButton(MpdNum,1,"RAID",(Av.AARadar.RaidOn),AV_GREEN,1.0);

	if (Av.AARadar.Declutter<2)
	{
		if((Primary->Id != -1) && (!aot))
		{
		//		float Heading = NormDegree( 360.0 - NormDegree( (/DEGREE) + Primary->RelAngle ));
		//		sprintf(TmpStr,"%d",(int)Heading);

			sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (Planes[Primary->Id].Heading/DEGREE))));
			DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_TOP + 12,TmpStr,AV_GREEN,1.0);
		}
	}

	DrawMdiArrow(137,32,0);
	DrawMdiArrow(137,52,1);
	DrawSelectedWeapon();

	DrawRadarSense();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawVSModeText(int MpdNum,BOOL aot)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"VS");
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",(int) Av.AARadar.VsLimit);
	DrawVertWordAtLocation(AA_RADAR_RIGHT + 4,AA_RADAR_TOP - 4,TmpStr,FALSE,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AARadar.AzimLimit*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// RSET
	if(Av.AARadar.RsetTimer.IsActive())
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
	 if(Av.AARadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AARadar.RsetTimer.Disable();
	}
	else
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);
	}

	// PRF
	switch(Av.AARadar.SearchPrf)
	{
	  case SEARCH_I_MODE: sprintf(TmpStr,"INT");break;
		case SEARCH_H_MODE: sprintf(TmpStr,"HI"); break;
		case SEARCH_M_MODE: sprintf(TmpStr,"MED"); break;
	}
	DrawWordAtMpdButton(MpdNum,4,TmpStr,FALSE,AV_GREEN,1.0);

	//SILENT
	DrawWordAtMpdButton(MpdNum,18,"SIL",(Av.AARadar.SilentOn),AV_GREEN,1.0);

	//BARS
	sprintf(TmpStr,"%dB",Av.AARadar.Bars);
	DrawWordAtMpdButton(MpdNum,19,TmpStr,FALSE,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AA_RADAR_LEFT - 12,AA_RADAR_EXTREME_TOP - 6,( (Av.AARadar.SilentOn) || (UFC.EMISState) || PlayerPlane->OnGround ) ? "STDBY" : "OPR",AV_GREEN,1.0);

	//ERASE
	DrawWordAtMpdButton(MpdNum,17,"ERASE",FALSE,AV_GREEN,1.0);

	// FADE FREQ
	sprintf(TmpStr,"%d",(int)Av.AARadar.AgeFrequency);
	DrawWordAtMpdButton(MpdNum,15,TmpStr,FALSE,AV_GREEN,1.0);

	// OWN AIRSPEED
  sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
 	DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWNSHIP ALT
	DrawOwnShipAltitude();

	 // OWN MACH
  sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 	DrawTextAtLoc(AA_RADAR_LEFT - 4,AA_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	// OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AA_RADAR_EXTREME_TOP - 6,TmpStr,AV_GREEN,1.0);

	// DECLUTTER
	switch(Av.AARadar.Declutter)
	{
	  case 0: sprintf(TmpStr,"DCLTR");break;
		case 1: sprintf(TmpStr,"DCLTR1"); break;
		case 2: sprintf(TmpStr,"DLCTR2"); break;
	}
	DrawWordAtMpdButton(MpdNum,8,TmpStr,(Av.AARadar.Declutter > 0) ? TRUE : FALSE,AV_GREEN,1.0);

	if((Primary->Id != -1) && !aot)
	{
//		float Heading = NormDegree( 360.0 - NormDegree( (PlayerPlane->Heading/DEGREE) + Primary->RelAngle ));
//		sprintf(TmpStr,"%d",(int)Heading);
		sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (Planes[Primary->Id].Heading/DEGREE))));
		DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_TOP + 12,TmpStr,AV_GREEN,1.0);
	}

  DrawMdiArrow(137,32,0);
  DrawMdiArrow(137,52,1);
  DrawSelectedWeapon();
  DrawRadarSense();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawSTTModeText(int MpdNum,BOOL aot)
{
	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	int sx,sy;

   strcpy(TmpStr,"RTS");
   GetMPDButtonXY(MpdNum,0,TmpStr,sx,sy);

   DrawTextAtLoc(sx,sy-1,TmpStr,AV_GREEN,1.0);
   strcpy(TmpStr,"TWS");
   DrawTextAtLoc(sx,sy+6,TmpStr,AV_GREEN,1.0);

   DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// DECLUTTER
	switch(Av.AARadar.Declutter)
	{
	  case 0: sprintf(TmpStr,"DCLTR");break;
		case 1: sprintf(TmpStr,"DCLTR1"); break;
		case 2: sprintf(TmpStr,"DLCTR2"); break;
	}
	DrawWordAtMpdButton(MpdNum,8,TmpStr,(Av.AARadar.Declutter > 0) ? TRUE : FALSE,AV_GREEN,1.0);

	 // RANGE
	 sprintf(TmpStr,"%d",Av.AARadar.Range);
	 DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);


	// NCTR
	DrawVerticalWordAtButton(MpdNum,10,"NCTR",(Av.AARadar.NCTROn),AV_GREEN,1.0);

	// PRF
	switch(Av.AARadar.SearchPrf)
	{
	  case SEARCH_I_MODE: sprintf(TmpStr,"INT");break;
		case SEARCH_H_MODE: sprintf(TmpStr,"HI"); break;
		case SEARCH_M_MODE: sprintf(TmpStr,"MED"); break;
	}
	DrawWordAtMpdButton(MpdNum,4,TmpStr,FALSE,AV_GREEN,1.0);

	//SILENT
	DrawWordAtMpdButton(MpdNum,18,"SIL",(Av.AARadar.SilentOn),AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AA_RADAR_LEFT - 12,AA_RADAR_EXTREME_TOP - 6,( (Av.AARadar.SilentOn) || (UFC.EMISState) || PlayerPlane->OnGround ) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// FADE FREQ
	sprintf(TmpStr,"%d",(int)Av.AARadar.AgeFrequency);
	DrawWordAtMpdButton(MpdNum,15,TmpStr,FALSE,AV_GREEN,1.0);

	// OWNSHIP ALT
	DrawOwnShipAltitude();

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
 	DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AA_RADAR_EXTREME_TOP - 6,TmpStr,AV_GREEN,1.0);

	// NCTR
	DrawNCTRText(MpdNum);

	 // OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 	DrawTextAtLoc(AA_RADAR_LEFT - 4,AA_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	if((Primary->Id != -1) && !aot && !Primary->Reaquire.BrokeLockReaquire)
	{
//		float Heading = NormDegree( 360.0 - NormDegree( (PlayerPlane->Heading/DEGREE) + Primary->RelAngle ));
//		sprintf(TmpStr,"%d",(int)Heading);
		sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (Planes[Primary->Id].Heading/DEGREE))));
		DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_TOP + 12,TmpStr,AV_GREEN,1.0);
	}

	DrawSelectedWeapon();
	DrawRadarSense();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawACQModeText(int MpdNum)
{
	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  sprintf(TmpStr,"TO RWS");
	DrawVerticalWordAtButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",Av.AARadar.Range);
	DrawTextAtLoc(AA_RADAR_RIGHT + 2,AA_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AARadar.AzimLimit*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// PRF
	switch(Av.AARadar.SearchPrf)
	{
	    case SEARCH_I_MODE: sprintf(TmpStr,"INT");break;
		case SEARCH_H_MODE: sprintf(TmpStr,"HI"); break;
		case SEARCH_M_MODE: sprintf(TmpStr,"MED"); break;
	}
	DrawWordAtMpdButton(MpdNum,4,TmpStr,FALSE,AV_GREEN,1.0);

	//SILENT
	DrawWordAtMpdButton(MpdNum,18,"SIL",(Av.AARadar.SilentOn),AV_GREEN,1.0);

	//BARS
	sprintf(TmpStr,"%dB",Av.AARadar.Bars);
	DrawWordAtMpdButton(MpdNum,19,TmpStr,FALSE,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AA_RADAR_LEFT - 12,AA_RADAR_EXTREME_TOP - 6,( (Av.AARadar.SilentOn) || (UFC.EMISState) || PlayerPlane->OnGround ) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// FADE FREQ
	sprintf(TmpStr,"%d",(int)Av.AARadar.AgeFrequency);
	DrawWordAtMpdButton(MpdNum,15,TmpStr,FALSE,AV_GREEN,1.0);

	// OWN AIRSPEED
  sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
 	DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

  // OWN MACH
  sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 	DrawTextAtLoc(AA_RADAR_LEFT - 4,AA_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	// OWNSHIP ALT
	DrawOwnShipAltitude();

	// DECLUTTER
	switch(Av.AARadar.Declutter)
	{
	  case 0: sprintf(TmpStr,"DCLTR");break;
		case 1: sprintf(TmpStr,"DCLTR1"); break;
		case 2: sprintf(TmpStr,"DLCTR2"); break;
	}
	DrawWordAtMpdButton(MpdNum,8,TmpStr,(Av.AARadar.Declutter > 0) ? TRUE : FALSE,AV_GREEN,1.0);

	// OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AA_RADAR_EXTREME_TOP - 6,TmpStr,AV_GREEN,1.0);

//	if(Primary->Id != -1)
//	{
////		float Heading = NormDegree( (PlayerPlane->Heading/DEGREE) + Primary->RelAngle );
////		sprintf(TmpStr,"%d",(int)Heading);
//		sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (Planes[Primary->Id].Heading/DEGREE))));
//		DrawTextAtLoc(AA_RADAR_LEFT + 2,AA_RADAR_TOP + 12,TmpStr,AV_GREEN,1.0);
//	}
}


//*****************************************************************************************************************************************
// BUTTON FUNCTIONS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
void AAChangeMode()
{
	Av.AARadar.CurMode++;

	if(Av.AARadar.CurMode > AA_VS_MODE)
		Av.AARadar.CurMode = AA_RWS_MODE;

	switch(Av.AARadar.CurMode)
	{
		case AA_RWS_MODE:
		break;    // do stuff for init,
		case AA_TWS_MODE:
			break;
		case AA_VS_MODE:
			Av.AARadar.SearchPrf = SEARCH_H_MODE; // only hi in vs mode
		break;
	}

	ResetAARadar(Av.AARadar.CurMode);
}

//*****************************************************************************************************************************************
void SlewAzim(int Dir)
{
	if(Dir == LTOR)
	{
		 if( (Av.AARadar.SlewLeftRight - 2.0 + Av.AARadar.AzimLimit <= 70.0) && (Av.AARadar.SlewLeftRight - 2.0 + Av.AARadar.AzimLimit <= 70.0) )
		 {
			 Av.AARadar.SlewLeftRight -= 2.0;
		 }
	}
	else
	{
		 if( (Av.AARadar.SlewLeftRight + 2.0 + Av.AARadar.AzimLimit <= 70.0) && (Av.AARadar.SlewLeftRight + 2.0 + Av.AARadar.AzimLimit <= 70.0) )
		 {
			 Av.AARadar.SlewLeftRight += 2.0;
		 }
	}
}

//*****************************************************************************************************************************************
void CenterAARadar()
{
	Av.AARadar.SlewUpDown = 0.0;
}

//*****************************************************************************************************************************************
void SlewElev(int Dir)
{
	if(Dir == DOWN)
	{
		 if( (Av.AARadar.SlewUpDown - 2.0 + Av.AARadar.ElevLimit <= 70.0) && (Av.AARadar.SlewUpDown - 2.0 + Av.AARadar.ElevLimit <= 70.0) )
		 {
			 Av.AARadar.SlewUpDown -= 2.0;
		 }
	}
	else
	{
		 if( (Av.AARadar.SlewUpDown + 2.0 + Av.AARadar.ElevLimit <= 70.0) && (Av.AARadar.SlewUpDown + 2.0 + Av.AARadar.ElevLimit <= 70.0) )
		 {
			 Av.AARadar.SlewUpDown += 2.0;
		 }
	}
}

//*****************************************************************************************************************************************
void AzimScanChange()
{
	if(Av.AARadar.AzimLimit < 40.0)
		Av.AARadar.AzimLimit += 10.0;
	else
	{
		if(Av.AARadar.AzimLimit == 40.0)
			Av.AARadar.AzimLimit = 70.0;
		else
			Av.AARadar.AzimLimit = 10.0;
	}

	Av.AARadar.AzimLimit -= fmod(Av.AARadar.AzimLimit,10.0f);

	ResetAARadar(Av.AARadar.CurMode,FALSE,FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void BarChange()
{
	if( (Av.AARadar.CurMode == AA_RWS_MODE) || (Av.AARadar.CurMode == AA_VS_MODE) )
	{
		if (++AABars >= &BarList[4])
			AABars = BarList;
	}
	else if(Av.AARadar.CurMode == AA_TWS_MODE)
	{
		if(Av.AARadar.AzimLimit == 10.0)
		{
			if (++AABars >= &BarList[4])
				AABars = &BarList[1];
		}
		else if(Av.AARadar.AzimLimit == 20.0)
		{
			if (++AABars >= &BarList[4])
				AABars = BarList;

			while( (*AABars != 2) && (*AABars != 4) )
				if (++AABars >= &BarList[4])
					AABars = BarList;
		}
		else if( (Av.AARadar.AzimLimit == 30.0) || (Av.AARadar.AzimLimit == 40.0) )
			AABars = &BarList[1];

	}

	ResetAARadar(Av.AARadar.CurMode,FALSE,FALSE,FALSE);
}


//*****************************************************************************************************************************************
void AARangeUp()
{
	if(Av.AARadar.CurMode != AA_VS_MODE)
	{
			Av.AARadar.Range *= 2.0;
			if(Av.AARadar.Range > 80.0)
				Av.AARadar.Range = 80.0;

			if(Av.AARadar.Range <= 10.0)
			{
				if( (Av.AARadar.CurMode == AA_TWS_MODE) || (Av.AARadar.CurMode == AA_STT_MODE) )
				{
					Av.AARadar.SearchPrf = SEARCH_M_MODE;
				}
				else if( (Av.AARadar.Range <= 5.0) && (Av.AARadar.CurMode == AA_RWS_MODE) )
					Av.AARadar.SearchPrf = SEARCH_M_MODE;
			}
	}
	else
	{
		if(Av.AARadar.VsLimit == 800.0)
			Av.AARadar.VsLimit = 2400.0;
	}

	CurFramePlanes.NumPlanes = 0;
  ClearAgePlanes();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AARangeDown()
{
	if(Av.AARadar.CurMode != AA_VS_MODE)
	{
			Av.AARadar.Range /= 2.0;
			if(Av.AARadar.Range < 5.0)
				Av.AARadar.Range = 5.0;
	}
	else
	{
		if(Av.AARadar.VsLimit == 2400.0)
			Av.AARadar.VsLimit = 800.0;
	}

	CurFramePlanes.NumPlanes = 0;
  ClearAgePlanes();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RSET()
{
	if(Av.AARadar.CurMode == AA_STT_MODE)
	{
		// primary stays the same
		Secondary->Id = -1;
		// do any reseting to get back to RWS mode
	}
	else if( (Av.AARadar.CurMode == AA_RWS_MODE) || (Av.AARadar.CurMode == AA_VS_MODE) )
	{
		Primary->Id = -1;
		Secondary->Id = -1;
	}
	else if(Av.AARadar.CurMode == Av.AARadar.CurMode == AA_TWS_MODE)
	{
		Secondary->Id = -1;
  }

	Av.AARadar.RsetTimer.Set(2.0,GetTickCount());

}

//*****************************************************************************************************************************************
void ToggleNctr()
{
 	Av.AARadar.NCTROn = !Av.AARadar.NCTROn;
}

//*****************************************************************************************************************************************
void ToggleRAID()
{
	if(Av.AARadar.CurMode != AA_TWS_MODE) return;

	Av.AARadar.RaidOn = !Av.AARadar.RaidOn;
	RaidDistRange = 1.0f;
	ooRaidDistRange = 1.0f/RaidDistRange;
	RaidAngleRange = 2.0f;
	ooRaidAngleRange = 1.0f/RaidAngleRange;
}

//*****************************************************************************************************************************************
void ToggleMSI()
{
	Av.AARadar.MSI = !Av.AARadar.MSI;
}

//*****************************************************************************************************************************************
void TargetAgingUp()
{
	Av.AARadar.AgeFrequency *= 2.0;

	if(Av.AARadar.AgeFrequency > 32.0)
		Av.AARadar.AgeFrequency = 2.0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ChangeDeclutter()
{
	Av.AARadar.Declutter++;
	if(Av.AARadar.Declutter > 2)
		Av.AARadar.Declutter = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ChangePrf()
{
	if( (Av.AARadar.CurMode == AA_RWS_MODE) && (Av.AARadar.Range <= 5.0) ) return;
	if(Av.AARadar.CurMode == AA_VS_MODE) return;    // only hi in vs mode
	if( (Av.AARadar.CurMode == AA_TWS_MODE) && (Av.AARadar.Range <= 10.0) ) return;
	if( (Av.AARadar.CurMode == AA_STT_MODE) && (Av.AARadar.Range <= 10.0) ) return;

	Av.AARadar.SearchPrf++;
	if(Av.AARadar.SearchPrf > SEARCH_I_MODE)
		Av.AARadar.SearchPrf = SEARCH_H_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleSilent()
{
  	Av.AARadar.SilentOn = !Av.AARadar.SilentOn;

	if( (Av.AARadar.CurMode == AA_STT_MODE) || (Av.AARadar.CurMode == AA_ACQ_MODE) && (Av.AARadar.SilentOn) )
		ResetToRWS();

	Av.AGRadar.SilenceOn = !Av.AGRadar.SilenceOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AAErase()
{
  ResetPlanes();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AAMenu()
{

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleAutoSlew()
{
	Av.AARadar.AutoOn = !Av.AARadar.AutoOn;

	if(!Av.AARadar.AutoOn)
	{
	  CurFramePlanes.NumPlanes = 0;
		PrevFramePlanes.NumPlanes = 0;
		ClearAgePlanes();
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlewAAAntenna(int Dir)
{
	if(UFC.MasterMode == AG_MODE)         return;
	if( !AreMpdsInGivenMode(RADAR_MODE) ) return;

	if(Dir <= 1)
	{
		if(Dir == 0)
			 SlewElev(UP);
		else
			SlewElev(DOWN);
	}
	else
	{
		if(Av.AARadar.AutoOn) return;
		if(Av.AARadar.CurMode != AA_TWS_MODE) return;

		if(Dir == 2)
			 SlewElev(RTOL);
		else
			SlewElev(LTOR);
	}
}

//*****************************************************************************************************************************************
// RWS MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

BOOL PrimaryIsAOT(DetectedPlaneListType *Plist)
{
	DetectedPlaneType *ptest;
	BOOL aot;

	if (Primary && (Primary->Id != -1))
	{
		if ((Av.AARadar.CurMode == AA_STT_MODE) && Primary->CriticalAspectTimer.IsActive())
			aot = (BOOL)((CurFramePlanes.Planes->PlaneId == Primary->Id) && (CurFramePlanes.Planes->AOT));
		else
		{
			ptest = GetPlanePtrInList(Plist, Primary->Id);
			aot = (BOOL)(ptest && ptest->AOT);
		}
	}
	else
		aot = FALSE;

	return aot;
}


void RWSMode(int MpdNum)
{
	DrawAARadarBackGround();

	BOOL aot = PrimaryIsAOT(&CurFramePlanes);

  	DrawAABullseye();

	if( (Av.AARadar.SilentOn) || (PlayerPlane->OnGround) || UFC.EMISState )
		DrawIronCross(AA_RADAR_LEFT + 13,AA_RADAR_BOTTOM-13);

	DrawAntennaLine();

	DrawElevCaret();

	DrawPlanes();

	DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,0.5);

 //	sprintf(TmpStr,"%d -- %d %d %d %d %d %d %d %d %d",CurFramePlanes.NumPlanes,CurFramePlanes.Planes[0].PlaneId,CurFramePlanes.Planes[1].PlaneId,CurFramePlanes.Planes[2].PlaneId,CurFramePlanes.Planes[3].PlaneId,CurFramePlanes.Planes[4].PlaneId,CurFramePlanes.Planes[5].PlaneId,CurFramePlanes.Planes[6].PlaneId,CurFramePlanes.Planes[7].PlaneId,CurFramePlanes.Planes[8].PlaneId);
 //	DrawTextAtLocation(AA_RADAR_LEFT + 5,AA_RADAR_TOP - 6,TmpStr,61);

	if(Av.AARadar.Declutter == 0)
	   DrawAttitudeIndicator(AA_RADAR_CENTER_X,AA_RADAR_CENTER_Y);

	if (!aot)
	{
		DrawDiffAltitude();
		DrawPrimaryRelVelCaret();
  		DrawAARadarHudSymbology();
	}

	DrawRWSModeText(MpdNum,aot);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:  AAChangeMode();
			          break;
			case  4:  ChangePrf();
								break;
			case  6:  AzimScanChange();
			          break;
			case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
								break;
			case  8:  ChangeDeclutter();
								break;
			case  9:  ToggleMSI();
								break;
			case 10:  ToggleNctr();
			          break;
			case 11:  RSET();
								break;
  		case 13:  AARangeDown();
			          break;
			case 14:  AARangeUp();
			          break;
			case 15:  TargetAgingUp();
								break;
			case 17:  AAErase();
			          break;
			case 18:  ToggleSilent();
			          break;
			case 19:  BarChange();
			          break;
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TWSMode(int MpdNum)
{
	DrawAARadarBackGround();

	BOOL aot = PrimaryIsAOT(&PrevFramePlanes);

//	FadeAgePlanes();

	DrawAABullseye();

	DrawAntennaLine();

	DrawElevCaret();

	if( (Av.AARadar.SilentOn) || (PlayerPlane->OnGround) || UFC.EMISState )
		DrawIronCross(AA_RADAR_LEFT + 13,AA_RADAR_BOTTOM-13);


	DrawTWSPlanes();

	DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,0.5);

	if(Av.AARadar.Declutter == 0)
	   DrawAttitudeIndicator(AA_RADAR_CENTER_X,AA_RADAR_CENTER_Y);

	if (!aot)
	{
		DrawDiffAltitude();
		DrawPrimaryRelVelCaret();
		DrawAARadarHudSymbology();
	}

	DrawTWSModeText(MpdNum,aot);

	// so Scotts code from F-15 can be compatible
	NumDTWSTargets = 0;
	DetectedPlaneType *P = &PrevFramePlanes.Planes[0];
	int Index = PrevFramePlanes.NumPlanes;
	while(Index-- > 0)
	{
		if (!P->AOT)
		{
			NumDTWSTargets++;
			DTWSTargets[NumDTWSTargets-1] = P->PlaneId;
		}
		P++;
	}

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		if (Av.AARadar.RaidOn)
			switch(ButtonPress)
			{
				case  0:  AAChangeMode();
					  	break;
				case  1:  ToggleRAID();
		 					break;
				case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							break;
				case  8:  ChangeDeclutter();
							break;
				case 10:  ToggleNctr();
					  		break;
				case 11:  RSET();
							 break;
				case 13:  if (RaidDistRange > 1.9f)
						  {
						  	RaidDistRange -= 1.0f;
						  	RaidAngleRange -= 1.0f;
						  }
						  else
						  {
						  	RaidDistRange = 1.0f;
						  	RaidAngleRange = 2.0f;
						  }
							ooRaidAngleRange = 1.0f/RaidAngleRange;
							ooRaidDistRange = 1.0f/RaidDistRange;

					  		break;
				case 14: if (RaidDistRange < 9.1f)
						 {
						 	RaidDistRange += 1.0f;
						 	RaidAngleRange += 1.0f;
						 }
						 else
						 {
						 	RaidDistRange = 10.0f;
						 	RaidAngleRange = 12.0f;
						 }
							ooRaidAngleRange = 1.0f/RaidAngleRange;
							ooRaidDistRange = 1.0f/RaidDistRange;
					  	break;
				case 17:  AAErase();
					  	break;
				case 18:  ToggleSilent();
					  	break;
			}
		else
			switch(ButtonPress)
			{
				case  0:  AAChangeMode();
					  	break;
				case  1:  ToggleRAID();
		 					break;
				case  4:  ChangePrf();
									break;
				case  6:  AzimScanChange();
					  	break;
				case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
									break;
				case  8:  ChangeDeclutter();
									break;
				case  9:  ToggleMSI();
									break;
				case 10:  ToggleNctr();
					  	break;
				case 11:  RSET();
									break;
				case 12:  ToggleAutoSlew();
							break;
				case 13:
						AARangeDown();
					  	break;
				case 14:
						AARangeUp();
					  	break;
				case 15:  TargetAgingUp();
									break;
				case 17:  AAErase();
					  	break;
				case 18:  ToggleSilent();
					  	break;
				case 19:  BarChange();
					  	break;
			}
	}
}

//*****************************************************************************************************************************************
void STTMode(int MpdNum)
{
	DrawAARadarBackGround();

	BOOL aot = PrimaryIsAOT(&CurFramePlanes);

	DrawElevCaret();

	DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,0.5);

	DrawAntennaLine();

	if((Primary->Id != -1) && (!aot) && !Primary->Reaquire.BrokeLockReaquire)
		DrawTargetProfile(Primary->XOfs);

	if (SimPause || !Primary->Reaquire.BrokeLockReaquire || ((GetTickCount() % 500) < 250))
	 	DrawTargetedPlane(aot);

	if (!aot && !Primary->Reaquire.BrokeLockReaquire)
	{
		DrawDiffAltitude();
		DrawPrimaryRelVelCaret();
		DrawAARadarHudSymbology();
	}

	DrawSTTModeText(MpdNum,aot);

	if(Av.AARadar.Declutter == 0)
		DrawAttitudeIndicator(AA_RADAR_CENTER_X,AA_RADAR_CENTER_Y);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:
				Av.AARadar.CurMode = AA_TWS_MODE;
               	ResetAARadar(Av.AARadar.CurMode);
			break;
			case  7:
				if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			break;
			case  8:
				ChangeDeclutter();
			break;
			case 18:
				ToggleSilent();
			break;
			case 10:
				ToggleNctr();
			break;
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void VSMode(int MpdNum)
{
 	DrawAARadarBackGround();


	BOOL aot = PrimaryIsAOT(&CurFramePlanes);

	DrawAABullseye();

	if( (Av.AARadar.SilentOn) || (PlayerPlane->OnGround) || UFC.EMISState )
		DrawIronCross(AA_RADAR_LEFT + 13,AA_RADAR_BOTTOM-13);

//	FadeAgePlanes();

	DrawAntennaLine();


	DrawElevCaret();

	DrawPlanes();

	DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,0.5);

	if(Av.AARadar.Declutter == 0)
	   DrawAttitudeIndicator(AA_RADAR_CENTER_X,AA_RADAR_CENTER_Y);

	if (!aot)
	{
		DrawDiffAltitude();
		DrawPrimaryRelVelCaret(TRUE);
		DrawAARadarHudSymbology();
	}

	DrawVSModeText(MpdNum,aot);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:  AAChangeMode();
								ResetPlaneLists();
			          break;
			case  6:  AzimScanChange();
			          break;
			case  7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
								break;
			case  8:  ChangeDeclutter();
								break;
			case 11:  RSET();
								break;
			case 13:  AARangeDown();
			          break;
			case 14:  AARangeUp();
			          break;
			case 17:  AAErase();
			          break;
			case 18:  ToggleSilent();
			          break;
			case 19:  BarChange();
			          break;
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ACQMode(int MpdNum)
{
	int NewPrimarySecondary = FALSE;
	FPointDouble ScanEnd;

	DrawAARadarBackGround();

	DrawAntennaLine();

	DrawElevCaret();

	if(Av.AARadar.Declutter == 0)
		DrawAttitudeIndicator(AA_RADAR_CENTER_X,AA_RADAR_CENTER_Y);

	DrawACQModeText(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:
				ResetToRWS();
			break;
			case  4:
				ChangePrf();
			break;
			case  7:
				Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			break;
			case  8:
				ChangeDeclutter();
			break;
			case 18:
				ToggleSilent();
			break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int NumDebugEdges = 0;
EdgeListEntryType DebugEdgeList[800];
int   ShowLOS = FALSE;
int   Intersect;
int  LOSMaintained;
float TInter;
float InterHeight;
FPointDouble Intersection;

void DrawLOSHeights()
{
	EdgeListEntryType *Walker = &DebugEdgeList[1];
	int XPos1,YPos1, XPos2, YPos2;
	int Color=60;

	if(NumDebugEdges == 0)
	  return;

	int Width = AA_RADAR_RIGHT - AA_RADAR_LEFT;
	int Height = AA_RADAR_BOTTOM - AA_RADAR_TOP;

	for(int i=0; i<NumDebugEdges; i++)
	{
	  DebugEdgeList[i].Height *= WUTOFT;
	}

	while(Walker <= &DebugEdgeList[NumDebugEdges-2])
	{
		XPos1 = AA_RADAR_LEFT + (Width*(Walker-1)->Tval);
		YPos1 = AA_RADAR_BOTTOM - ( ((Walker-1)->Height/10000)*Height );

		if( (YPos1 <= AA_RADAR_BOTTOM) && (YPos1 >= AA_RADAR_TOP) )
			GrDrawLine(GrBuffFor3D, XPos1, AA_RADAR_BOTTOM, XPos1,YPos1, Color);

		XPos2 = AA_RADAR_LEFT + (Width*(Walker)->Tval);
		YPos2 = AA_RADAR_BOTTOM - ( ( (Walker)->Height/10000)*Height );

		if( (YPos1 <= AA_RADAR_BOTTOM) && (YPos2 <= AA_RADAR_BOTTOM) && (YPos1 >= AA_RADAR_TOP) && (YPos2 >= AA_RADAR_TOP) )
		{
			GrDrawLine(GrBuffFor3D, XPos1, YPos1, XPos2,YPos2, Color);
		}
		Walker++;
	}

	XPos1 = AA_RADAR_LEFT + Width*(Walker)->Tval;
	YPos1 = AA_RADAR_BOTTOM - ( ((Walker)->Height/10000)*Height );

	if( (YPos1 <= AA_RADAR_BOTTOM) && (YPos1 >= AA_RADAR_TOP) )
		GrDrawLine(GrBuffFor3D, XPos1,AA_RADAR_BOTTOM, XPos1,YPos1, 60);

	if( (YPos1 <= AA_RADAR_BOTTOM) && (YPos2 <= AA_RADAR_BOTTOM) && (YPos1 >= AA_RADAR_TOP) && (YPos2 >= AA_RADAR_TOP) )
		GrDrawLine(GrBuffFor3D, XPos1, YPos1, XPos2,YPos2, 60);

	/*
  FPointDouble P1 = PlayerPlane->WorldPosition;
  FPointDouble P2 = Planes[Primary->Id].WorldPosition;

  if(Intersect)
     GrDrawLine(GrBuffFor3D, AA_RADAR_LEFT, AA_RADAR_BOTTOM - (PlayerPlane->WorldPosition.Y/10000.0)*Height,
	          AA_RADAR_LEFT + (Width*TInter), AA_RADAR_BOTTOM - (InterHeight/10000.0)*Height,3);
  else
    GrDrawLine(GrBuffFor3D, AA_RADAR_LEFT, AA_RADAR_BOTTOM - (PlayerPlane->WorldPosition.Y/10000.0)*Height,
	          AA_RADAR_LEFT + Width - 1, AA_RADAR_BOTTOM - (Planes[Primary->Id].WorldPosition.Y/10000.0)*Height, 60);


  GrDrawDot(GrBuffFor3D,AA_RADAR_LEFT + Width - 1,AA_RADAR_BOTTOM - (Planes[Primary->Id].WorldPosition.Y/10000.0)*Height,3);
  GrDrawDot(GrBuffFor3D,AA_RADAR_LEFT + 1,AA_RADAR_BOTTOM - (PlayerPlane->WorldPosition.Y/10000.0)*Height,3);
	 * */
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawLOS()
{
 //  	if((Primary->Id != -1) )
  //  {
		   float NewX,NewZ;
       ProjectPoint(PlayerPlane->WorldPosition.X,PlayerPlane->WorldPosition.Z,NormDegree(PlayerPlane->Heading/DEGREE),40.0*NMTOWU,&NewX,&NewZ);
       FPointDouble Temp;

        Temp = Planes[Primary->Id].WorldPosition;

        Planes[Primary->Id].WorldPosition.X = NewX;
        Planes[Primary->Id].WorldPosition.Y = PlayerPlane->WorldPosition.Y;
        Planes[Primary->Id].WorldPosition.Z = NewZ;

	      LOSMaintained = LOS(&(PlayerPlane->WorldPosition),&(Planes[Primary->Id].WorldPosition), &Intersection, MED_RES_LOS, TRUE);

			 	CopyLOSEdges(&DebugEdgeList[0], &NumDebugEdges, &TInter, &InterHeight, &Intersect);
        DrawLOSHeights();

        Planes[Primary->Id].WorldPosition = Temp;

  // }
}

//*****************************************************************************************************************************************
// DO AIR TO AIR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DoAirToAirRadar(int MpdNum)
{
	//if(Primary->Id != -1)
	//{
	////		DrawLOS();
	//		return;
	//}

	if( (Av.AARadar.SilentOn) || (PlayerPlane->OnGround) || UFC.EMISState )
		DrawIronCross(AA_RADAR_LEFT + 13,AA_RADAR_BOTTOM-13);

	// if guns seelcted, always go to guns acq mode
	if( (WeapStores.GunsOn) && !( (Av.AARadar.CurMode == AA_ACQ_MODE) && (Av.AARadar.AcqMode == AA_GUN_ACQ_MODE) ) && (Av.AARadar.CurMode != AA_STT_MODE) )
	{
  		SetAcqMode(AA_GUN_ACQ_MODE);
		return;
	}

	// so Scotts code from F-15 can be compatible..this is set in TWS mode
	NumDTWSTargets = 0;

//	SimIff();
//	SimNCTR();
//	SimBlinks();

	switch (Av.AARadar.CurMode)
	{
		case AA_RWS_MODE:
		{
			RWSMode(MpdNum);
			break;
		}
		case AA_STT_MODE:
		{
			STTMode(MpdNum);
			break;
		}
		case AA_TWS_MODE:
		{
			TWSMode(MpdNum);
			break;
		}
		case AA_VS_MODE:
		{
			VSMode(MpdNum);
			break;
		}
		default:
		{
			ACQMode(MpdNum);
			break;
		}
	}
	DrawTDCIndicator(MpdNum);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetAcqMode(int Mode)
{
	if( (Mode < AA_WIDE_ACQ_MODE) || (Mode > AA_GUN_ACQ_MODE) ) return;

	PlayerPlane->AADesignate = NULL;
	TempCurFramePlanes.NumPlanes = 0;
	CurFramePlanes.NumPlanes = 0;
	PrevFramePlanes.NumPlanes = 0;

	Av.AARadar.FirstTime = 1;
	Av.AARadar.DiffCount = 0;
	Av.AARadar.AcqTimer.Set(3.0,GameLoopInTicks);

	switch(Mode)
	{
		case AA_WIDE_ACQ_MODE:
			Av.AARadar.Range = 10.0;
			Av.AARadar.Bars  = 6;
			Av.AARadar.SearchPrf = SEARCH_M_MODE;
			Av.AARadar.Prf = MPRF;
			Av.AARadar.AzimLimit = 60.0;
			Av.AARadar.SlewUpDown = 0;
			Av.AARadar.SlewLeftRight = 0;
			Av.AARadar.ElevLimit = GetAngularCoverage(Av.AARadar.Range,Av.AARadar.Bars)/2.0;
		break;

		case AA_VERTICAL_ACQ_MODE:
			Av.AARadar.Range = 5.0;
			Av.AARadar.Bars  = 2;
			Av.AARadar.SearchPrf = SEARCH_M_MODE;
			Av.AARadar.Prf = MPRF;
			Av.AARadar.ElevLimit  = 30;
			Av.AARadar.SlewUpDown = 17;
			Av.AARadar.SlewLeftRight = 0;
			Av.AARadar.AzimLimit = GetAngularCoverage(Av.AARadar.Range,Av.AARadar.Bars)/2.0;
		break;

		case AA_BORE_SIGHT_MODE:
			Av.AARadar.AzimLimit = 2;
			Av.AARadar.Range = 10.0;
			Av.AARadar.Bars  = 2;
			Av.AARadar.SearchPrf = SEARCH_M_MODE;
			Av.AARadar.Prf = MPRF;
			Av.AARadar.ElevLimit = 2.0;
			Av.AARadar.SlewUpDown = 0;
			Av.AARadar.SlewLeftRight = 0;
			//Av.AARadar.ElevLimit = GetAngularCoverage(Av.AARadar.Range,Av.AARadar.Bars)/2.0;
		break;

	    case AA_GUN_ACQ_MODE:
			Av.AARadar.Range = 5.0;
			Av.AARadar.AzimLimit = 10;
			Av.AARadar.Bars  = 5;
			Av.AARadar.SearchPrf = SEARCH_M_MODE;
			Av.AARadar.Prf = MPRF;
			Av.AARadar.ElevLimit = 10.0;
			Av.AARadar.SlewUpDown = -4;
			Av.AARadar.SlewLeftRight = 0;
			//Av.AARadar.ElevLimit = GetAngularCoverage(Av.AARadar.Range,Av.AARadar.Bars)/2.0;
		break;
	 }

//	Av.AARadar.CosElevLimit = cos((Av.AARadar.ElevLimit)*DEGREES_TO_RADIANS);
//	Av.AARadar.CosAzimLimit = cos((Av.AARadar.AzimLimit)*DEGREES_TO_RADIANS);
	Av.AARadar.LeftAzimEdge  = Av.AARadar.AzimLimit;
	Av.AARadar.RightAzimEdge = -Av.AARadar.AzimLimit;
	Av.AARadar.BarCount = Av.AARadar.Bars;
	Av.AARadar.AntAzimuth = 0;
	Av.AARadar.AntElevation = Av.AARadar.ElevLimit;
	Av.AARadar.UpElevEdge   = Av.AARadar.ElevLimit/* + Av.AARadar.SlewUpDown*/;
	Av.AARadar.DownElevEdge = -Av.AARadar.ElevLimit;//Av.AARadar.UpElevEdge - (Av.AARadar.ElevLimit*2));
	Av.AARadar.SlewLeftRight = 0;

	Av.AARadar.ScanDirection = LTOR;
	Av.AARadar.RaidOn = FALSE;

	ResetPrimary();

	Av.AARadar.CurMode = AA_ACQ_MODE;
	Av.AARadar.AcqMode = Mode;

	ResetAARadar(AA_ACQ_MODE);

	if (Mode ==  AA_VERTICAL_ACQ_MODE)
	{
		Av.AARadar.AntAzimuth = Av.AARadar.AzimLimit;
		Av.AARadar.AntElevation = 0;
		Av.AARadar.ElevStep = Av.AARadar.AzimLimit*2.0f/(float)(Av.AARadar.Bars);
	}

	AcqTimer.Set(3.0,GameLoopInTicks);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTDCIndicator(int MpdNum)
{
	if(TDCMpdNum == MpdNum)
		DrawDiamond(TDC_INDICATOR,5,5,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int SetTDCMpd(int MpdNum, int X, int Y)
{
	if(MpdNum == -1) return(TRUE);

	int OldTdc = TDCMpdNum;

	if( (MpdNum == TDCMpdNum) || (MpdNum == MPD3) )
	{
		MpdClick.MpdClick = TRUE;
		MpdClick.MpdNum = MpdNum;
		MpdClick.X = X;
		MpdClick.Y = Y;
	}

	int RetVal = (TDCMpdNum == MpdNum);

	TDCMpdNum = MpdNum;

	if(TDCMpdNum == MPD6)
	{
	  if(Mpds[TDCMpdNum].Mode == NONE_MODE)
				TDCMpdNum = OldTdc;
	}

	if(TDCMpdNum == MPD3)
	{
		TDCMpdNum = OldTdc;
	}
	return(RetVal);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTDCMpd()
{
	TDCMpdNum++;

	if(TDCMpdNum > MPD6)
		TDCMpdNum = 0;
	else
	{
		if(TDCMpdNum == 3)
			TDCMpdNum = MPD6;
	}

	if(TDCMpdNum == MPD6)
	{
	  if( (Av.Ufc.Mode != UFC_MDI_MODE)  )
				TDCMpdNum = 0;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetMpdClick(int MpdNum,int *X,int *Y)
{
	if(!MpdClick.MpdClick) return(FALSE);
	if(MpdNum != MpdClick.MpdNum) return(FALSE);

	*X = MpdClick.X;
	*Y = MpdClick.Y;

	MpdClick.MpdClick = FALSE;

	return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetTDCMode()
{
	if(TDCMpdNum == -1)
		return(-1);
	return( Mpds[TDCMpdNum].Mode );
}

//*****************************************************************************************************************************************
void InitAvionicsModes()
{
	InitAARadar();
	InitAGRadar();
	InitTews();
	InitUfc();
	InitHsi();
	InitSA();
	InitBit();
	InitAcl();

	InitDestructoCam();
	InitDirectionArrow();
}

//*****************************************************************************************************************************************
int GetAARadarMode()
{
	if (UFC.MasterMode == AA_MODE)
		for(int i=0; i<7; i++)
		{
			if( Mpds[i].Mode == RADAR_MODE)
				return(Av.AARadar.CurMode);
		}
	return(-1);
}

//*****************************************************************************************************************************************
int GetCurrentAARadarMode()
{
	return(Av.AARadar.CurMode);
}

//*****************************************************************************************************************************************
int GetCurrentPrimaryTarget()
{
	return(Primary->Id);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int SetAARadarToSTTMode()
{
	if(Primary->Id == -1) return(FALSE);

	Av.AARadar.CurMode = AA_STT_MODE;
	ResetAARadar(AA_STT_MODE);

	return(TRUE);
}

//*****************************************************************************************************************************************
// MISC DRAW PRIMITVES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawDiamond(float X,float Y,float Length,float Width,int Color,float Shade)
{
	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);

	GrDrawLine(GrBuffFor3D,X-Width,Y,X,Y-Length,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+Width,Y,X,Y-Length,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X-Width,Y,X,Y+Length,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+Width,Y,X,Y+Length,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
// COLORS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void CreateAvColorRamp()
{
	AvRGB Pal[4];

	Pal[AV_GREEN].Red   = 0;
	Pal[AV_GREEN].Green = 255;
	Pal[AV_GREEN].Blue  = 0;

	Pal[AV_RED].Red   = 255;
	Pal[AV_RED].Green = 0;
	Pal[AV_RED].Blue  = 0;

	Pal[AV_YELLOW].Red   = 255;
	Pal[AV_YELLOW].Green = 255;
	Pal[AV_YELLOW].Blue  = 0;

	Pal[AV_BLUE].Red   = 0;
	Pal[AV_BLUE].Green = 0;
	Pal[AV_BLUE].Blue  = 255;

	for(int i=0; i<4; i++)
	{
		float StepRed   = (float)Pal[i].Red/(float)AV_PALETTE_NUM_SHADES;
		float StepGreen = (float)Pal[i].Green/(float)AV_PALETTE_NUM_SHADES;
		float StepBlue  = (float)Pal[i].Blue/(float)AV_PALETTE_NUM_SHADES;

		float Red   = 1;
		float Green = 0;
		float Blue  = 0;

		for(int j=0; j<AV_PALETTE_NUM_SHADES; j++)
		{
		  AvPalette[i][j].Red   = (int)Red;
		  AvPalette[i][j].Green = (int)Green;
		  AvPalette[i][j].Blue  = (int)Blue;

		  Red   += StepRed;
		  Green += StepGreen;
		  Blue  += StepBlue;
		}
	}

	// init hud colors
	HUDColorRGB = &HUDColorList[0];

	GetRGBFromAVPalette(AV_GREEN,0.2, &HUDColorList[9]);
	GetRGBFromAVPalette(AV_GREEN,0.3, &HUDColorList[10]);
	if((iHUDR < 0) || (iHUDG < 0) || (iHUDB < 0))
	{
		GetRGBFromAVPalette(AV_GREEN,0.7, &HUDColorList[0]);
	}
	else
	{
		HUDColorList[0].Red = iHUDR;
		HUDColorList[0].Green = iHUDG;
		HUDColorList[0].Blue = iHUDB;
	}
	GetRGBFromAVPalette(AV_GREEN,0.9, &HUDColorList[1]);
	GetRGBFromAVPalette(AV_GREEN,1.0, &HUDColorList[2]);
	GetRGBFromAVPalette(AV_RED,0.5,   &HUDColorList[3]);
	GetRGBFromAVPalette(AV_RED,1.0,   &HUDColorList[4]);
	GetRGBFromAVPalette(AV_YELLOW,0.5,&HUDColorList[5]);
	GetRGBFromAVPalette(AV_YELLOW,0.7,&HUDColorList[6]);
	GetRGBFromAVPalette(AV_YELLOW,1.0,&HUDColorList[7]);
	GetRGBFromAVPalette(AV_GREEN,0.0, &HUDColorList[8]);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetRGBFromAVPalette(int Color, float Shade,AvRGB *Triple,int Normalize)
{
	int ColorIndex = (int)( Shade*((float)(AV_PALETTE_NUM_SHADES-1)) );

	Triple->Red   = AvPalette[Color][ColorIndex].Red;
	Triple->Green = AvPalette[Color][ColorIndex].Green;
	Triple->Blue  = AvPalette[Color][ColorIndex].Blue;

	if(Normalize)
	{
		float OneOver255 = 1.0/255.0;
		Triple->Red *= OneOver255;
		Triple->Green *= OneOver255;
		Triple->Blue *= OneOver255;
	}
}

//*****************************************************************************************************************************************
// MPD INTERACTIONS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

FPoint MpdButtons[20];

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetMPDButtonXY(int MpdDetectNum,int ButtonNum,char *Word,int &TextScrX,int &TextScrY)
{
	int Height = 5;
	int HeightDiv2 = Height/2;
	int Len;
	int LenDiv2;

	Len = GrStrLen(Word, SmHUDFont);
	LenDiv2 = Len/2;

	switch (ButtonNum)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:

				TextScrX = MpdButtons[ButtonNum].X;
				TextScrY = MpdButtons[ButtonNum].Y - HeightDiv2;
				break;

		case 5:
		case 6:
		case 7:
		case 8:
		case 9:

				TextScrX = MpdButtons[ButtonNum].X - LenDiv2;
				TextScrY = MpdButtons[ButtonNum].Y - Height - 1;
				break;

		case 10:
		case 11:
		case 12:
		case 13:
		case 14:

				TextScrX = MpdButtons[ButtonNum].X - Len-1;
				TextScrY = MpdButtons[ButtonNum].Y - HeightDiv2;
				break;

		case 15:
		case 16:
		case 17:
		case 18:
		case 19:

				TextScrX = MpdButtons[ButtonNum].X - LenDiv2;
				TextScrY = MpdButtons[ButtonNum].Y;
				break;
	}

}

void DrawWordAtMpdButton(int MpdDetectNum,int ButtonNum, char *Word, int DrawBox,int Color, float Shade)
{
	int TextScrX;
	int TextScrY;

	GetMPDButtonXY(MpdDetectNum,ButtonNum,Word,TextScrX,TextScrY);

	DrawTextAtLoc(TextScrX,TextScrY,Word,Color,Shade);

	if(DrawBox)
	{
		AvRGB Triple;
		GetRGBFromAVPalette(Color,Shade,&Triple);
		DrawBoxAroundWord(Word,TextScrX,TextScrY,Triple.Red,Triple.Green,Triple.Blue);
	}
}


//*****************************************************************************************************************************************
void DrawVerticalWordAtButton(int MpdDetectNum,int ButtonNum, char *Word, int BoxIt,int Color, float Shade)
{
	int Height = 5;
	int HeightDiv2 = Height/2;
	int TextScrX;
	int TextScrY;
	int DoNothing = FALSE;

	Height = (strlen(Word)*6);
	HeightDiv2 = Height/2;

	switch (ButtonNum)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			TextScrX = MpdButtons[ButtonNum].X;
			TextScrY = MpdButtons[ButtonNum].Y - HeightDiv2;
		break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			TextScrX = MpdButtons[ButtonNum].X - 6;
			TextScrY = MpdButtons[ButtonNum].Y - HeightDiv2;
		break;
		default:
			DoNothing = TRUE;
		break;
	}

	DrawVertWordAtLocation(TextScrX,TextScrY,Word,BoxIt,Color,Shade);

	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);
//	if(BoxIt)
//    DrawBox(GrBuffFor3D, TextScrX - 2, TextScrY - 2,8,strlen(Word)*5 + 6,Triple.Red,Triple.Green,Triple.Blue);
}

//*****************************************************************************************************************************************
void InitMpdButtons()
{
	float Step = 140.0/6.0;
	float XPos = 2;
	float YPos = Step;

	for(int i=0; i<5; i++)
	{
		MpdButtons[i].X = XPos;
		MpdButtons[i].Y = YPos;

		YPos += Step;
	}

	MpdButtons[0].X = XPos;
	MpdButtons[0].Y = 24;

	MpdButtons[1].X = XPos;
	MpdButtons[1].Y = 50;

	MpdButtons[2].X = XPos;
	MpdButtons[2].Y = 72;

	MpdButtons[3].X = XPos;
	MpdButtons[3].Y = 95;

	MpdButtons[4].X = XPos;
	MpdButtons[4].Y = 117;

	YPos = 140;
	XPos = Step;

	for(i=5; i<10; i++)
	{
		MpdButtons[i].X = XPos;
		MpdButtons[i].Y = YPos;

		XPos += Step;
	}

	XPos = 140;
	YPos = Step*5;


	for(i=10; i<15; i++)
	{
		MpdButtons[i].X = XPos;
		MpdButtons[i].Y = YPos;

		YPos -= Step;
	}

	MpdButtons[14].X = XPos;
	MpdButtons[14].Y = 24;

	MpdButtons[13].X = XPos;
	MpdButtons[13].Y = 50;

	MpdButtons[12].X = XPos;
	MpdButtons[12].Y = 72;

	MpdButtons[11].X = XPos;
	MpdButtons[11].Y = 95;

	MpdButtons[10].X = XPos;
	MpdButtons[10].Y = 117;

	YPos = 2;
	XPos = Step*5;

	for(i=15; i<20; i++)
	{
		MpdButtons[i].X = XPos;
		MpdButtons[i].Y = YPos;

		XPos -= Step;
	}
}

//*****************************************************************************************************************************************
void InitMpdInteractions()
{
    MpdButton.Pressed = FALSE;
		InitMpdButtons();
		TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
void MpdButtonPress(int MpdNum, int ButtonNum)
{
	MpdButton.Pressed   = TRUE;
	MpdButton.MpdNum    = MpdNum;
	MpdButton.ButtonNum = ButtonNum;
}

//*****************************************************************************************************************************************
int GetButtonPress(int MpdNum)
{
	int RetVal = -1;

	if(MpdButton.Pressed)
	{
		if(MpdNum == MpdButton.MpdNum)
		{
			RetVal = MpdButton.ButtonNum;
	        MpdButton.Pressed = FALSE;
		}
	}

	return(RetVal);
}

//*****************************************************************************************************************************************
// MISC MPD UTILS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawTextAtLoc(int ScrX, int ScrY,  char *pMpdString,int Color, float Shade, int UseLarge)
{
	AvRGB Triple;
  GetRGBFromAVPalette(Color,Shade,&Triple);

  GrDrawStringClipped(GrBuffFor3D, (UseLarge) ? LgHUDFont : SmHUDFont, ScrX, ScrY,pMpdString,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawVertWordAtLocation(int ScrX, int ScrY,  char *pMpdString, int BoxIt,int Color, float Shade)
{
  char *P;
  int Count;
  int VertPos;
  int CharCount;
  char Str[2];

  AvRGB Triple;
  GetRGBFromAVPalette(Color,Shade,&Triple);

  Str[1]  = '\x00';
  P       = pMpdString;
  VertPos =	ScrY;

  CharCount = 0;
  Count     = 0;
  while(*P != '\x00')
  {
		Str[0] = *P;
		GrDrawString(GrBuffFor3D, SmHUDFont, ScrX, VertPos, Str,Triple.Red,Triple.Green,Triple.Blue,0);
		VertPos += 6;
		CharCount++;
		P++;
  }

  if(BoxIt)
    DrawBox(GrBuffFor3D, ScrX - 2, ScrY - 2, 8,CharCount*6 + 4,Triple.Red,Triple.Green,Triple.Blue);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CrossOutVertWordAtLocation(int ScrX, int ScrY,  char *pMpdString, int Color, float Shade)
{
  char *P;
  int Count;
  int VertPos;
  int CharCount;
  char Str[2];

	AvRGB Triple;
  GetRGBFromAVPalette(Color,Shade,&Triple);

  Str[1]  = '\x00';
  P       = pMpdString;
  VertPos =	ScrY;

  CharCount = 0;
  Count     = 0;
  while(*P != '\x00')
  {
		Str[0] = *P;
		GrDrawString(GrBuffFor3D, SmHUDFont, ScrX, VertPos, Str,Triple.Red,Triple.Green,Triple.Blue,0);
		VertPos += 6;
		CharCount++;
		P++;
  }

	GrDrawLine(GrBuffFor3D,ScrX-2,ScrY-2,ScrX+6,VertPos+8,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,ScrX+8,ScrY-2,ScrX-2,VertPos+8,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
// AA RADAR UTILS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void GetAltHeightsAtRange(float Range, float *AltHigh, float *AltLow)
{
	float Pitch;

	Pitch = NormDegree(Av.AARadar.SlewUpDown + Av.AARadar.ElevLimit)*DEGREE;

	*AltHigh =  (PlayerPlane->WorldPosition.Y + ISinTimes(Pitch,Range*NMTOWU))*WUTOFT;

	Pitch    =  NormDegree(Av.AARadar.SlewUpDown - Av.AARadar.ElevLimit)*DEGREE;

	*AltLow  =  (PlayerPlane->WorldPosition.Y + ISinTimes(Pitch,Range*NMTOWU))*WUTOFT;
}

//*****************************************************************************************************************************************
// MAIN MENU  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void MainMenuMode(int MpdNum)
{
	DrawVerticalWordAtButton(MpdNum,0,"SMS",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,1,"RDR",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,2,"HUD",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,3,"HSI",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,5,"FUEL",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,6,"FPAS",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,8,"EW",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,9,"ADI",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,11,"ACL",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,12,"SA",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,13,"ENG",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,14,"CHK",FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,17,"BIT",FALSE,AV_GREEN,1.0);

	if(Av.Weapons.HasNavFlir)
		DrawWordAtMpdButton(MpdNum,18,"NFLR",FALSE,AV_GREEN,1.0);

	if(Av.Weapons.HasTargetIR)
		DrawWordAtMpdButton(MpdNum,19,"FLIR",FALSE,AV_GREEN,1.0);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:  Mpds[MpdNum].Mode = STORES_MODE;
								ResetBombProgram(Av.Weapons.CurAGWeap);
								break;
			case  1:  Mpds[MpdNum].Mode = RADAR_MODE;
								break;
			case  2:  Mpds[MpdNum].Mode = HUD_MODE;
								break;
			case  3:  Mpds[MpdNum].Mode = HSI_MODE;
								break;
			case  5:  Mpds[MpdNum].Mode = FUEL_MODE;
								break;
			case  6:  Mpds[MpdNum].Mode = FPAS_MODE;
								break;
			case  8:  Mpds[MpdNum].Mode = TEWS_MODE;
								break;
			case  9:  Mpds[MpdNum].Mode = ADI_MODE;
								break;
			case  11: Mpds[MpdNum].Mode = ACL_MODE;
								break;
			case  12: Mpds[MpdNum].Mode = SA_MODE;
								break;
			case  13: Mpds[MpdNum].Mode = ENG_MODE;
								break;
			case  14: Mpds[MpdNum].Mode = CHECKLIST_MODE;
								break;
			case  17: Mpds[MpdNum].Mode = BIT_MODE;
								break;
			case  18: if(Av.Weapons.HasNavFlir)
				          Mpds[MpdNum].Mode = NAVFLIR_MODE;
								break;
			case  19: if(Av.Weapons.HasTargetIR)
				          Mpds[MpdNum].Mode = TGT_IR_MODE;
							  break;
		}
	}
}

//*****************************************************************************************************************************************
// AG RADAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


int NumAGRadarLOS;
AGRadarLOSType AGRadarLOSList[MAX_AGRADAR_LOS_OBJS];

int NumProcessedLOS; // so that game can never grind to a halt because of too many LOS


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAGBeam()
{
  AvRGB Triple;
 	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	GrDrawLineClipped(GrBuffFor3D,AG_RADAR_ORIGIN_X,AG_RADAR_ORIGIN_Y,Av.AGRadar.BeamEndPointPixel.X,Av.AGRadar.BeamEndPointPixel.Z,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
// AG SCAN AREA SLEW -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int AGSimScanArea()
{
  if( (GetTDCMode() != RADAR_MODE) ) return(0);

	int PrevActive  = ScanAreaControl.Active;
	ScanAreaControl.Active = GetVkStatus(CaptBarsVk);

	if(!PrevActive && !ScanAreaControl.KeyActive)
		return 0;

	if(ScanAreaControl.Active && !PrevActive)
	{
		ScanAreaControl.Timer.Set(15.0,GetTickCount());
	}
	else if(ScanAreaControl.Active)
	{
		int IsLeft,IsUp;
		float HorzPercent,VertPercent;
		float TimeSinceLastFrame = ScanAreaControl.Timer.TimeElapsed( GetTickCount() );

		ScanAreaControl.Timer.Set(15.0, GetTickCount() );

		GetJoyStickExtentsInPercent(&IsLeft,&IsUp,&HorzPercent,&VertPercent);

		if( (Av.AGRadar.CurMode == AG_MAP_MODE) && (Av.AGRadar.MapMode != -1) )
		{
		if(HorzPercent > VertPercent)
			MoveScanAreaInDirection(Av.AGRadar.MapMode,(IsLeft) ? 2 : 3,TimeSinceLastFrame);
		else
    		MoveScanAreaInDirection(Av.AGRadar.MapMode,(IsUp) ? 1 : 0,TimeSinceLastFrame);
		}
		else
		{
			float Dx,Dy;
			if (TimeSinceLastFrame > 1.0f/15.0f)
				TimeSinceLastFrame = 1.0f/15.0f;

			if(HorzPercent > VertPercent)
			{
		  	Dx = TimeSinceLastFrame*( (IsLeft) ? -(HorzPercent*MAX_CAPT_BAR_RATE) : (HorzPercent*MAX_CAPT_BAR_RATE) );
		  	Dy = 0;
			}
			else
			{
		  	Dx = 0;
		  	Dy = TimeSinceLastFrame*( (IsUp)   ? -(VertPercent*MAX_CAPT_BAR_RATE) : (VertPercent*MAX_CAPT_BAR_RATE) );
			}

			Av.AGRadar.BoxTranslate += Dy;
			Av.AGRadar.BoxRot       += (fabs(Dx)/70.0)*((Dx < 0) ? 60.0 : -60.0);
		}
	}
	else
	{
		ScanAreaControl.Timer.Disable();
		return 0;
	}

	return(1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimScanAreaForKeyboard()
{
	//if( !( (GetTDCMode() == RADAR_MODE) && (UFC.MasterMode != AG_MODE) ) ) return;

	if( (GetTDCMode() != RADAR_MODE) ) return;

	int PrevActive  = ScanAreaControl.KeyActive;
	int Left,Right,Up,Down;

	Up    = GetVkStatus(TdcUpVk);
	Down  = GetVkStatus(TdcDownVk);
	Left  = GetVkStatus(TdcLeftVk);
	Right = GetVkStatus(TdcRightVk);

	ScanAreaControl.KeyActive = ScanAreaControl.KeyActive || Left || Right || Up || Down;

	if(PrevActive || ScanAreaControl.KeyActive)
	{

		if(ScanAreaControl.KeyActive && !PrevActive)
		{
			ScanAreaControl.Timer.Set(15.0,GetTickCount());
		}
		else if(ScanAreaControl.KeyActive)
		{
			float TimeSinceLastFrame = ScanAreaControl.Timer.TimeElapsed( GetTickCount() );

			ScanAreaControl.Timer.Set(15.0, GetTickCount() );

			if( (Av.AGRadar.CurMode == AG_MAP_MODE) && (Av.AGRadar.MapMode != -1) )
			{
		 		if(Left)
					MoveScanAreaInDirection(Av.AGRadar.MapMode,2,TimeSinceLastFrame);
				else if(Right)
					MoveScanAreaInDirection(Av.AGRadar.MapMode,3,TimeSinceLastFrame);
				else if(Up)
					MoveScanAreaInDirection(Av.AGRadar.MapMode,1,TimeSinceLastFrame);
				else if(Down)
					MoveScanAreaInDirection(Av.AGRadar.MapMode,0,TimeSinceLastFrame);
			}
			else
			{
					float Dx,Dy;

					Dx = Dy = 0;

					if (TimeSinceLastFrame > 1.0f/15.0f)
						TimeSinceLastFrame = 1.0f/15.0f;

					if(Left || Right)
		  			Dx = TimeSinceLastFrame*( (Left) ? -(1.0*MAX_CAPT_BAR_RATE) : (1.0*MAX_CAPT_BAR_RATE) );
					else if(Up || Down)
		  			Dy = TimeSinceLastFrame*( (Up)   ? -(1.0*MAX_CAPT_BAR_RATE) : (1.0*MAX_CAPT_BAR_RATE) );

					Av.AGRadar.BoxTranslate += Dy;
					Av.AGRadar.BoxRot       += (fabs(Dx)/70.0)*((Dx < 0) ? 60.0 : -60.0);
			}
		}
		else
			ScanAreaControl.Timer.Disable();
	}

}

void FixScanBoxBoundaries()
{
	if(Av.AGRadar.BoxTranslate < -120)
		Av.AGRadar.BoxTranslate = -120;

	float biggestw,bigh;

	if (Av.AGRadar.MapMode == AG_EXP1)
	{
		biggestw = SCAN_AREA_WIDTH*0.25;
		bigh = -SCAN_AREA_WIDTH*0.5f;
	}
	else
		if (Av.AGRadar.MapMode == AG_EXP2)
		{
			biggestw = SCAN_AREA_WIDTH*0.25*0.25;
			bigh = -SCAN_AREA_WIDTH*0.5f*0.25f;
		}
		else
		{
			biggestw = SCAN_AREA_WIDTH*0.25*0.25*0.25;
			bigh = -SCAN_AREA_WIDTH*0.5f*0.25f*0.25f;
		}

	bigh += -biggestw*(1.0f/1.73f);

	if(Av.AGRadar.BoxTranslate > bigh)
		Av.AGRadar.BoxTranslate = bigh;

	if(Av.AGRadar.BoxRot < -60)
		Av.AGRadar.BoxRot = -60;
	if(Av.AGRadar.BoxRot > 60)
		Av.AGRadar.BoxRot = 60;

	double out = -Av.AGRadar.BoxTranslate * AG_SCAN_MILES_PER_PIXEL(Av.AGRadar.Range)*NMTOWU;

	ProjectPointDouble(PlayerPlane->WorldPosition.X, PlayerPlane->WorldPosition.Z, fANGLE_TO_DEGREES(PlayerPlane->Heading)+Av.AGRadar.BoxRot,
		out, &Av.AGRadar.WorldBoxCenter.X, &Av.AGRadar.WorldBoxCenter.Z);


}

//*****************************************************************************************************************************************
// AG  DESIGNATE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

BOOL GetAGRadarTargetPos(FPointDouble &point)
{
	BOOL ret_val = TRUE;

	if (Av.AGRadar.Target && (Av.AGRadar.CurMode == AG_FTT_MODE))
	{
		if(Av.AGRadar.TargetType == AG_REGISTERED_OBJECT)
			point  = ((BasicInstance *)Av.AGRadar.Target)->Position;
		else
			if (Av.AGRadar.TargetType == AG_VEHICLE)
				point = ((MovingVehicleParams *)Av.AGRadar.Target)->WorldPosition;
			else
				ret_val = FALSE;
	}
	else
		ret_val = FALSE;

	return ret_val;
}

void ProcessCaptBarClick()
{
	if(Av.AGRadar.SilenceOn || UFC.EMISState) return;

	DetectedGroundObjType *Ptr = &AGGroundObjs.GroundObjs[0];

	int Index = AGGroundObjs.NumGroundObj;
	while(Index-- > 0)
	{
		int Inside = IsPointInside(CaptBars.Pos.X,CaptBars.Pos.Y,Ptr->OfsY-5,Ptr->OfsY+5,Ptr->OfsX-5,Ptr->OfsX+5);

		if(Inside)
		{
			Av.AGRadar.TargetType = AG_REGISTERED_OBJECT;
			Av.AGRadar.Target = (BasicInstance *)Ptr->Obj;
			Av.AGRadar.TargetMoving = FALSE;
			Av.AGRadar.PrevMode = (Av.AGRadar.CurMode <= 2 ) ? Av.AGRadar.CurMode : -1;
			Av.AGRadar.CurMode = AG_FTT_MODE;
      		ResetAGRadar(Av.AGRadar.CurMode);
			FlirUnDesignate();
			return;
		}

		Ptr++;
	}

  	DetectedVehicleType *Ptr2 = &AGVehicles.Vehicles[0];

	Index = AGVehicles.NumVehicles;
	while(Index-- > 0)
	{
		int Inside = IsPointInside(CaptBars.Pos.X,CaptBars.Pos.Y,Ptr2->OfsY-5,Ptr2->OfsY+5,Ptr2->OfsX-5,Ptr2->OfsX+5);

		if(Inside)
		{
			Av.AGRadar.TargetType = AG_VEHICLE;
			Av.AGRadar.Target = (MovingVehicleParams *)Ptr2->Veh;
			Av.AGRadar.TargetMoving = Ptr2->IsMoving;
			Av.AGRadar.PrevMode = (Av.AGRadar.CurMode <= 2 ) ? Av.AGRadar.CurMode : -1;
			Av.AGRadar.CurMode = AG_FTT_MODE;
      		ResetAGRadar(Av.AGRadar.CurMode);
			FlirUnDesignate();
			return;
		}

		Ptr2++;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGRadarDesignateTarget()
{
	if(Av.AGRadar.CaptBarsOn)
	{
		ProcessCaptBarClick();
		return;
	}

	if(Av.AGRadar.CurMode == AG_RBM_MODE)
	{
		if(Av.AGRadar.MapMode != -1)
		{
			SetMapModePixelToWorld(AG_EXP1);
	    	SetMapModePixelToWorld(AG_EXP2);
	    	SetMapModePixelToWorld(AG_EXP3);
	    	SetBoundPixelToWorld();

			Av.AGRadar.ZoomMode = Av.AGRadar.MapMode;

			Av.AGRadar.CurMode = AG_MAP_MODE;
		}
	}
	else if(Av.AGRadar.CurMode == AG_MAP_MODE)
	{
		Av.AGRadar.ZoomMode = Av.AGRadar.MapMode;
    	SetZoomBoundToWorld(Av.AGRadar.MapMode);
		if(Av.AGRadar.MapMode == AG_EXP3)
		{
			Av.AGRadar.MapMode = -1;
			Av.AGRadar.ZoomMode = -1;
		}
	}

  ResetAGRadar(Av.AGRadar.CurMode);
}

//*****************************************************************************************************************************************
// GLOBALS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// AG INITS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void InitScanAreas()
{
	// BIG
	ScanAreas[0][0].X = -(SCAN_AREA_WIDTH*0.5);
	ScanAreas[0][0].Z = -(SCAN_AREA_HEIGHT*0.5);
	ScanAreas[0][0].Y = 0;

	ScanAreas[0][1].X = -(SCAN_AREA_WIDTH*0.25);
	ScanAreas[0][1].Z = (SCAN_AREA_HEIGHT*0.5);
	ScanAreas[0][0].Y = 0;

	ScanAreas[0][2].X = (SCAN_AREA_WIDTH*0.25);
	ScanAreas[0][2].Z = (SCAN_AREA_HEIGHT*0.5);
	ScanAreas[0][0].Y = 0;

	ScanAreas[0][3].X = (SCAN_AREA_WIDTH*0.5);
	ScanAreas[0][3].Z = -(SCAN_AREA_HEIGHT*0.5);
	ScanAreas[0][0].Y = 0;

	// MED
	ScanAreas[1][0] = ScanAreas[0][0];
	ScanAreas[1][0] *= 0.25;
	ScanAreas[1][1] = ScanAreas[0][1];
	ScanAreas[1][1] *= 0.25;
	ScanAreas[1][2] = ScanAreas[0][2];
	ScanAreas[1][2] *= 0.25;
	ScanAreas[1][3] = ScanAreas[0][3];
	ScanAreas[1][3] *= 0.25;

	// SMALL
	ScanAreas[2][0] = ScanAreas[1][0];
	ScanAreas[2][0] *= 0.5;
	ScanAreas[2][1] = ScanAreas[1][1];
	ScanAreas[2][1] *= 0.5;
	ScanAreas[2][2] = ScanAreas[1][2];
	ScanAreas[2][2] *= 0.5;
	ScanAreas[2][3] = ScanAreas[1][3];
	ScanAreas[2][3] *= 0.5;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ZoomRBMap( FPoint pt1, FPoint pt2, FPoint pt3, FPoint pt4);
void ZoomRBMapOff(void);

void InitAGRadar()
{
  Av.AGRadar.CurMode = AG_RBM_MODE;
	Av.AGRadar.DeclutterOn = FALSE;
	Av.AGRadar.RSETOn = FALSE;
	Av.AGRadar.FreezeOn = FALSE;
	Av.AGRadar.SilenceOn = FALSE;
  Av.AGRadar.MapMode = -1;
  Av.AGRadar.ZoomMode = -1;
  Av.AGRadar.PrevMode = -1;
	Av.AGRadar.InterleaveOn = FALSE;
	Av.AGRadar.Range = 40.0;
	Av.AGRadar.Azim  = 60.0;

	Av.AGRadar.BoxRot = 0.0;
	Av.AGRadar.BoxTranslate = -54.0;

	Av.AGRadar.VideoGain = 1;

	Av.AGRadar.ScanRate = 40.0;
	Av.AGRadar.FirstTime = 1;
  Av.AGRadar.DiffCount = 0;
	Av.AGRadar.AntAzimuth = 60.0;
	Av.AGRadar.ScanDirection = LTOR;

	Av.AGRadar.TargetType = -1;
	Av.AGRadar.CaptBarsOn = FALSE;

	AGGroundObjs.NumGroundObj = 0;
	AGVehicles.NumVehicles    = 0;

	ResetAGRadar(Av.AGRadar.CurMode);
	InitScanAreas();
	ScanBeamOn = FALSE;

  NumAGRadarLOS = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CleanupAGRadar()
{


}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetAGRadar(int Mode)
{
	//ScanBeamOn = FALSE;

	ScanBeamOn = TRUE;

	switch(Mode)
	{
    	case AG_GMT_MODE:
		case AG_SEA_MODE:
			if (Av.AGRadar.Azim >= 50)
				Av.AGRadar.Azim = 10;
	    	break;

    	case AG_TA_MODE :
			if(Av.AGRadar.Range > 10.0)
				Av.AGRadar.Range = 10.0;
			Av.AGRadar.Azim = 45.0;
			break;
	}

	if(Av.AGRadar.Range >= 70.0)
		MediumHeightCache->SetViewDist(CACHE_HGT_DIST_FAR);
	else
		MediumHeightCache->SetViewDist(CACHE_HGT_DIST_NORM);

	ResetAGRadarScanInfo();
	ZoomRBMapOff();

	AGGroundObjs.NumGroundObj = 0;
	AGVehicles.NumVehicles    = 0;

}

//*****************************************************************************************************************************************
// AG DRAW  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AGDrawOwnShipAltitude()
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"%d",(int)(PlayerPlane->WorldPosition.Y*WUTOFT));
	RightJustifyTextAtLocation(AG_RADAR_RIGHT - 5,AG_RADAR_BOTTOM + 2,TmpStr,Triple.Red,Triple.Green,Triple.Blue);
}

//*****************************************************************************************************************************************
// AG MAP FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


/*
FPoint ScanAreas[3][4];
FPoint ScanAreasWorld[3][4];
FPoint ScanAreasPixel[3][4];

FPoint ScanBoundPixel[4];
FPoint ScreenZoomWorld[4];
FPoint RBMBoundWorld[4];
*/

void AGScanPixelToWorld(FPointDouble Source,FPointDouble *Loc)
{
	float MilesPerPixel = AG_SCAN_MILES_PER_PIXEL(Av.AGRadar.Range);

	FPointDouble From,To,Temp;

	From.X = AG_RADAR_ORIGIN_X;
	From.Z = AG_RADAR_ORIGIN_Y;
	From.Y = 0;

	To.X = Source.X;
	To.Z = Source.Z;
	To.Y = 0;

	Temp = To;
	Temp -= From;

	double Dist = Temp.Length();
	Dist *= MilesPerPixel;
	Dist *= NMTOWU;

	float DeltaAzim = ComputeHeadingToPoint(From,0,To,TRUE);

	float NewX,NewZ;
	ProjectPoint(PlayerPlane->WorldPosition.X,PlayerPlane->WorldPosition.Z,NormDegree( (PlayerPlane->Heading/DEGREE) + DeltaAzim),Dist,&NewX,&NewZ);

	Loc->X = NewX;
	Loc->Y = 0;
	Loc->Z = NewZ;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGScanWorldToPixel(FPointDouble Source,FPointDouble *Loc)
{
    float NewX,NewZ;

	float PixelsPerMile = AG_SCAN_PIXELS_PER_MILE(Av.AGRadar.Range);

	float Dist = Dist2D(&PlayerPlane->WorldPosition,&Source);
	Dist *= WUTONM;
	Dist *= PixelsPerMile;

	float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,Source,TRUE);

	ProjectPoint(AG_RADAR_ORIGIN_X,AG_RADAR_ORIGIN_Y,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);

	Loc->X = NewX;
	Loc->Y = 0;
	Loc->Z = NewZ;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGZoomInWorldToPixel(FPointDouble Source,FPointDouble *Loc)
{
	FPointDouble UL,LL,UR,LR;

	UL = ScreenZoomWorld[0];
	LL = ScreenZoomWorld[1];
	LR = ScreenZoomWorld[2];
	UR = ScreenZoomWorld[3];

	FPointDouble FromULToUR;
	FromULToUR.MakeVectorToFrom(UR,UL);
	float WorldWidth = FromULToUR.Normalize();

	FPointDouble FromULToLL;
	FromULToLL.MakeVectorToFrom(LL,UL);
	float WorldHeight = FromULToLL.Normalize();

	float PixelPerWorldWidth  = (AG_RADAR_RIGHT  - AG_RADAR_LEFT)/(WorldWidth);
	float PixelPerWorldHeight = (AG_RADAR_BOTTOM - AG_RADAR_TOP)/(WorldHeight);

	FPointDouble FromULToSource;

	FromULToSource.MakeVectorToFrom(Source,UL);

	float DotWidth  = FromULToSource * FromULToUR;
	float DotHeight = FromULToSource * FromULToLL;

	Loc->X = AG_RADAR_LEFT + (PixelPerWorldWidth*DotWidth);
	Loc->Z = AG_RADAR_TOP  + (PixelPerWorldHeight*DotHeight);
	Loc->Y = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetMapModeWorldToPixel(int Mode)
{
	FPointDouble *Source = &ScanAreasWorld[Mode][0];
	FPointDouble *Box    = &ScanAreasPixel[Mode][0];

	for(int i=0; i<4; i++)
	{
	  AGScanWorldToPixel(*Source,Box);
		Source++;
	  Box++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetMapModePixelToWorld(int Mode)
{
  FPointDouble *Source = &ScanAreasPixel[Mode][0];
	FPointDouble *Box    = &ScanAreasWorld[Mode][0];

	for(int i=0; i<4; i++)
	{
		AGScanPixelToWorld(*Source,Box);

		Source++;
	  Box++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetBoundPixelToWorld()
{
	FPointDouble *Source = &ScanBoundPixel[0];
	FPointDouble *Box    = &ScreenZoomWorld[0];

	for(int i=0; i<4; i++)
	{
	  AGScanPixelToWorld(*Source,Box);
		Source++;
	  Box++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetMapModeWorldToZoomPixel(int Mode)
{
  FPointDouble *Source =  &ScanAreasWorld[Mode][0];
	FPointDouble *Box    =  &ScanAreasZoomPixel[Mode][0];

	for(int i=0; i<4; i++)
	{
	  AGZoomInWorldToPixel(*Source,Box);
		Source++;
	  Box++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetBoundZoomWorldToPixel(int Mode)
{
  FPointDouble *Source =  &ScanBoundZoomWorld[Mode][0];
	FPointDouble *Box    =  &ScanBoundZoomPixel[Mode][0];

	for(int i=0; i<4; i++)
	{
	  AGZoomInWorldToPixel(*Source,Box);
		Source++;
	  Box++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetZoomBoundToWorld(int Mode)
{
    FPointDouble *Source =  &ScanBoundZoomWorld[Mode][0];
	FPointDouble *Box    =  &ScreenZoomWorld[0];

	memcpy(Box,Source,sizeof(FPointDouble)*4);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
#define SCAN_AREA_PIXELS_PER_SECOND  10.0

void MoveScanAreaInDirection(int Mode,int Dir, float TimeElapsed)
{
	float Frac;

	if (TimeElapsed > (1.0f/15.0f))
		TimeElapsed = 1.0f/15.0f;

	float MoveDist = ((SCAN_AREA_PIXELS_PER_SECOND*AG_SCAN_MILES_PER_PIXEL(Av.AGRadar.Range)*NMTOWU)*TimeElapsed);

	FPointDouble UL,LL,UR,LR;

	UL = ScreenZoomWorld[0];
	LL = ScreenZoomWorld[1];
	LR = ScreenZoomWorld[2];
	UR = ScreenZoomWorld[3];

	FPointDouble FromULToUR;

	FromULToUR.MakeVectorToFrom(UR,UL);

	float WorldWidth = FromULToUR.Length();

	Frac = MoveDist/WorldWidth;
    if(Dir == 2)
		Frac *= -1.0;

	FromULToUR *= Frac;

	FPointDouble FromULToLL;

	FromULToLL.MakeVectorToFrom(LL,UL);

	float WorldHeight = FromULToLL.Length();

	Frac = MoveDist/WorldHeight;
    if(Dir == 1)
		Frac *= -1.0;

	FromULToLL *= Frac;

    // test to make sure its inside the screen
	FPointDouble TempScanWorld[3][4];
	memcpy(TempScanWorld,ScanAreasWorld,sizeof(FPointDouble)*12);

	for(int i=0; i<4; i++)
	{
    if(Dir > 1)
		{
       ScanAreasWorld[AG_EXP2][i] += FromULToUR;
       ScanAreasWorld[AG_EXP3][i] += FromULToUR;
		}
	  else
		{
       ScanAreasWorld[AG_EXP2][i] += FromULToLL;
       ScanAreasWorld[AG_EXP3][i] += FromULToLL;
		}
	}

	AGSetZoomBoundArea(Mode);
 	SetBoundZoomWorldToPixel(Mode);

	int Outside = FALSE;

	int MinX,MinZ,MaxX,MaxZ;

	MinX = MinZ = 10000000;
	MaxX = MaxZ = -10000000;

	for(i=0; i<4; i++)
	{
      if( ScanBoundZoomPixel[Mode][i].X < MinX)
		    MinX = ScanBoundZoomPixel[Mode][i].X;
      if( ScanBoundZoomPixel[Mode][i].Z < MinZ)
		    MinZ = ScanBoundZoomPixel[Mode][i].Z;

			if( ScanBoundZoomPixel[Mode][i].X > MaxX)
		    MaxX = ScanBoundZoomPixel[Mode][i].X;
      if( ScanBoundZoomPixel[Mode][i].Z > MaxZ)
		    MaxZ = ScanBoundZoomPixel[Mode][i].Z;
	}

  if(Dir > 1)
	{
      if(MinX - AG_RADAR_LEFT < 5)
		    Outside = TRUE;
	    else if(AG_RADAR_RIGHT - MaxX < 5)
		    Outside = TRUE;
	}
	else
	{
      if(MinZ - AG_RADAR_TOP < 5)
		    Outside = TRUE;
	    else if(AG_RADAR_BOTTOM - MaxZ < 5)
		    Outside = TRUE;
	}

	if(Outside)
	{
 	  memcpy(ScanAreasWorld,TempScanWorld,sizeof(FPointDouble)*12);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawZoomBoundArea(int Mode)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	for(int i=0; i<3; i++)
 	   GrDrawLine(GrBuffFor3D,ScanBoundZoomPixel[Mode][i].X,ScanBoundZoomPixel[Mode][i].Z,ScanBoundZoomPixel[Mode][i+1].X,ScanBoundZoomPixel[Mode][i+1].Z,Triple.Red,Triple.Green,Triple.Blue,0);

    GrDrawLine(GrBuffFor3D,ScanBoundZoomPixel[Mode][3].X,ScanBoundZoomPixel[Mode][3].Z,ScanBoundZoomPixel[Mode][0].X,ScanBoundZoomPixel[Mode][0].Z,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawBoundArea()
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	for(int i=0; i<3; i++)
	   GrDrawLine(GrBuffFor3D,ScanBoundPixel[i].X,ScanBoundPixel[i].Z,ScanBoundPixel[i+1].X,ScanBoundPixel[i+1].Z,Triple.Red,Triple.Green,Triple.Blue,0);

  GrDrawLine(GrBuffFor3D,ScanBoundPixel[3].X,ScanBoundPixel[3].Z,ScanBoundPixel[0].X,ScanBoundPixel[0].Z,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSetBoundArea(int mode)
{
	if(Av.AGRadar.MapMode == -1) return;

  FPointDouble *ScanPtr = &ScanAreasPixel[mode][0];
  FPointDouble *BoundPtr = &ScanBoundPixel[0];

	float MinX,MaxX,MinZ,MaxZ;

	MinX = MinZ = 100000000;
	MaxX = MaxZ = -100000;

	for(int i=0; i<4; i++)
	{
		if(ScanPtr->X < MinX)
		{
			MinX = ScanPtr->X;
		}

		if(ScanPtr->X > MaxX)
		{
			MaxX = ScanPtr->X;
		}

		if(ScanPtr->Z < MinZ)
		{
			MinZ = ScanPtr->Z;
		}

		if(ScanPtr->Z > MaxZ)
		{
			MaxZ = ScanPtr->Z;
		}

		ScanPtr++;
	}

	if(MinX < AG_RADAR_LEFT)
        ScanBoundClipLeft = AG_RADAR_LEFT;

	if(MaxX > AG_RADAR_RIGHT)
		ScanBoundClipRight = AG_RADAR_RIGHT;

	if(MinZ < AG_RADAR_TOP)
		ScanBoundClipTop = AG_RADAR_TOP;

	if(MaxZ > AG_RADAR_BOTTOM)
		ScanBoundClipBottom = AG_RADAR_BOTTOM;

	BoundPtr[0].X = MinX;
	BoundPtr[0].Y = 0;
	BoundPtr[0].Z = MinZ;

	BoundPtr[1].X = MinX;
	BoundPtr[1].Y = 0;
	BoundPtr[1].Z = MaxZ;

	BoundPtr[2].X = MaxX;
	BoundPtr[2].Y = 0;
	BoundPtr[2].Z = MaxZ;

	BoundPtr[3].X = MaxX;
	BoundPtr[3].Y = 0;
	BoundPtr[3].Z = MinZ;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSetZoomBoundArea(int Mode)
{
 	FPointDouble *ScanPtr  = &ScanAreasWorld[Mode][0];

	FPointDouble TempBoundPixel[4];
	FPointDouble *BoundPtr = &TempBoundPixel[0];

	for(int i=0; i<4; i++)
	{
	  AGZoomInWorldToPixel(*ScanPtr,BoundPtr);
		ScanPtr++;
		BoundPtr++;
	}

	BoundPtr = &TempBoundPixel[0];

	float MinX,MaxX,MinZ,MaxZ;

	MinX = MinZ = 100000000;
	MaxX = MaxZ = -100000;

	for(i=0; i<4; i++)
	{
		if(BoundPtr->X < MinX)
		{
			MinX = BoundPtr->X;
		}

		if(BoundPtr->X > MaxX)
		{
			MaxX = BoundPtr->X;
		}

		if(BoundPtr->Z < MinZ)
		{
			MinZ = BoundPtr->Z;
		}

		if(BoundPtr->Z > MaxZ)
		{
			MaxZ = BoundPtr->Z;
		}

		BoundPtr++;
	}

  FPointDouble UL,LL,UR,LR;

	UL.X = AG_RADAR_LEFT;
	UL.Z = AG_RADAR_TOP;
	UL.Y = 0;

	LL.X = UL.X;
	LL.Z = AG_RADAR_BOTTOM;
	LL.Y = 0;

	LR.X = AG_RADAR_RIGHT;
	LR.Z = LL.Z;
	LR.Y = 0;

	FPointDouble FromULToUR;
	FPointDouble FromULToLL;

	float Width  = LR.X - UL.X;
	float Height = LL.Z - UL.Z;

	float MinXFrac = (MinX - UL.X)/Width;
	float MaxXFrac = (MaxX - UL.X)/Width;
	float MinZFrac = (MinZ - UL.Z)/Height;
	float MaxZFrac = (MaxZ - UL.Z)/Height;

	UL = ScreenZoomWorld[0];
	LL = ScreenZoomWorld[1];
	LR = ScreenZoomWorld[2];
	UR = ScreenZoomWorld[3];

	FromULToUR.MakeVectorToFrom(UR,UL);
	FromULToLL.MakeVectorToFrom(LL,UL);

	FPointDouble *ZoomBound = &ScanBoundZoomWorld[Mode][0];

	FPointDouble MinXVec,MaxXVec,MinZVec,MaxZVec;

	MinXVec = FromULToUR;
	MinXVec *= MinXFrac;

	MaxXVec = FromULToUR;
	MaxXVec *= MaxXFrac;

	MinZVec = FromULToLL;
	MinZVec *= MinZFrac;

	MaxZVec = FromULToLL;
	MaxZVec *= MaxZFrac;

	ZoomBound[0] =  UL;
	ZoomBound[0] += MinXVec;
	ZoomBound[0] += MinZVec;

	ZoomBound[1] =  UL;
	ZoomBound[1] += MinXVec;
	ZoomBound[1] += MaxZVec;

	ZoomBound[2] =  UL;
	ZoomBound[2] += MaxXVec;
	ZoomBound[2] += MaxZVec;

	ZoomBound[3] =  UL;
	ZoomBound[3] += MaxXVec;
	ZoomBound[3] += MinZVec;

}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSetScanAreas()
{
	FPointDouble Box[4];
	FPointDouble RotBox[4];

	for(int i=0; i<3; i++)
	{
			memcpy(Box,&ScanAreas[i][0],sizeof(FPointDouble)*4);

			for(int j=0; j<4; j++)
			{
	  		Box[j].Z += Av.AGRadar.BoxTranslate;
			}

			FMatrix Mat;

			Mat.SetHPR( NormDegree(Av.AGRadar.BoxRot)*DEGREE,0,0);

			for(j=0; j<4; j++)
			{
	  		RotBox[j].RotateInto(Box[j],Mat);
      		RotBox[j].X += AG_RADAR_CENTER_X;
	  		RotBox[j].Z += AG_RADAR_BOTTOM;
			}

			memcpy(&ScanAreasPixel[i][0],RotBox,sizeof(FPointDouble)*4);
	}

}

//*****************************************************************************************************************************************
// AG BUTTON PRESS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AGAzimScanChange()
{
	if(Av.AGRadar.Azim == 60.0)
		Av.AGRadar.Azim = 10.0;
	else if(Av.AGRadar.Azim == 10.0)
		Av.AGRadar.Azim = 22.5;
	else if(Av.AGRadar.Azim == 22.5)
		Av.AGRadar.Azim = 45.0;
	else
		Av.AGRadar.Azim = 60.0;

	ResetAGRadar(Av.AGRadar.CurMode);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGRangeUp()
{
 	Av.AGRadar.Range *= 2.0;

	if(Av.AGRadar.CurMode == AG_GMT_MODE)
	{
			if(Av.AGRadar.Range > 40.0)
				Av.AGRadar.Range = 40.0;
	}
	else if(Av.AGRadar.CurMode == AG_TA_MODE)
	{
			if(Av.AGRadar.Range > 10.0)
				Av.AGRadar.Range = 10.0;
	}
	else
	{
			if(Av.AGRadar.Range > 80.0)
				Av.AGRadar.Range = 80.0;
	}

	ResetAGRadar(Av.AGRadar.CurMode);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGRangeDown()
{
	Av.AGRadar.Range /= 2.0;
	if(Av.AGRadar.Range < 5.0)
		Av.AGRadar.Range = 5.0;

	ResetAGRadar(Av.AGRadar.CurMode);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGRSET()
{
	Av.AGRadar.RsetTimer.Set(2.0,GetTickCount());
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGToggleSilent()
{
  	Av.AARadar.SilentOn = !Av.AARadar.SilentOn;

	if( (Av.AARadar.CurMode == AA_STT_MODE) || (Av.AARadar.CurMode == AA_ACQ_MODE) && (Av.AARadar.SilentOn) )
		ResetToRWS();

	Av.AGRadar.SilenceOn = !Av.AGRadar.SilenceOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGToggleFreeze()
{
	Av.AGRadar.FreezeOn = !Av.AGRadar.FreezeOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGToggleInterleave()
{
	Av.AGRadar.InterleaveOn = !Av.AGRadar.InterleaveOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGToggleDeclutter()
{
	Av.AGRadar.DeclutterOn = !Av.AGRadar.DeclutterOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGChangeMode()
{
	BOOL fix_range;
	if(Av.AGRadar.CurMode == AG_FTT_MODE)
	{
		BreakAGLock();
		fix_range = FALSE;
	}
	else
		fix_range = TRUE;

	Av.AGRadar.CurMode++;

	if(Av.AGRadar.CurMode > AG_TA_MODE)
		 Av.AGRadar.CurMode = AG_RBM_MODE;

	ResetAGRadar(Av.AGRadar.CurMode);

	if (fix_range)
		if ((Av.AGRadar.CurMode != AG_FTT_MODE) && (Av.AGRadar.CurMode != AG_TA_MODE))
		{
			Av.AGRadar.Range = 40.0;
			if (Av.AGRadar.CurMode == AG_RBM_MODE)
				Av.AGRadar.Azim = 60;
			else
				Av.AGRadar.Azim = 45;
		}

	Av.AGRadar.AntAzimuth = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSetExp1()
{
	if (Av.AGRadar.CurMode == AG_MAP_MODE)
	{

		Av.AGRadar.MapMode = -1;
		Av.AGRadar.ZoomMode = -1;
		Av.AGRadar.CurMode = AG_RBM_MODE;
		ResetAGRadar(Av.AGRadar.CurMode);
		return;

		switch(Av.AGRadar.MapMode)
		{
			case AG_EXP1:
				Av.AGRadar.MapMode = -1;
				Av.AGRadar.ZoomMode = -1;
				Av.AGRadar.CurMode = AG_RBM_MODE;
				ResetAGRadar(Av.AGRadar.CurMode);
				return;
			break;

			case AG_EXP2:
			case AG_EXP3:
				SetMapModePixelToWorld(AG_EXP1);
	    		SetMapModePixelToWorld(AG_EXP2);
	    		SetMapModePixelToWorld(AG_EXP3);
	    		SetBoundPixelToWorld();
	   			Av.AGRadar.MapMode = AG_EXP1;
				Av.AGRadar.ZoomMode = AG_EXP1;
				return;
			break;
		}
	}
	else
		if (Av.AGRadar.CurMode == AG_RBM_MODE)
	   		Av.AGRadar.MapMode = AG_EXP1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSetExp2()
{
	if (Av.AGRadar.CurMode == AG_MAP_MODE)
	{
		Av.AGRadar.MapMode = -1;
		Av.AGRadar.ZoomMode = -1;
		Av.AGRadar.CurMode = AG_RBM_MODE;
		ResetAGRadar(Av.AGRadar.CurMode);
		return;

		switch(Av.AGRadar.MapMode)
		{
			case AG_EXP3:
				SetMapModePixelToWorld(AG_EXP1);
	    		SetMapModePixelToWorld(AG_EXP2);
	    		SetMapModePixelToWorld(AG_EXP3);
	    		SetBoundPixelToWorld();
	   			Av.AGRadar.MapMode = AG_EXP2;
				Av.AGRadar.ZoomMode = AG_EXP2;
				return;
			break;

			case AG_EXP2:
				Av.AGRadar.MapMode = -1;
				Av.AGRadar.ZoomMode = -1;
				Av.AGRadar.CurMode = AG_RBM_MODE;
				ResetAGRadar(Av.AGRadar.CurMode);
				return;
			break;

			case AG_EXP1:
	   			Av.AGRadar.MapMode = AG_EXP2;
				return;
			break;
		}
	}
	else
		if (Av.AGRadar.CurMode == AG_RBM_MODE)
			Av.AGRadar.MapMode = AG_EXP2;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSetExp3()
{
	if (Av.AGRadar.CurMode == AG_MAP_MODE)
	{
		Av.AGRadar.MapMode = -1;
		Av.AGRadar.ZoomMode = -1;
		Av.AGRadar.CurMode = AG_RBM_MODE;
		ResetAGRadar(Av.AGRadar.CurMode);
		return;

		switch(Av.AGRadar.MapMode)
		{
			case AG_EXP1:
			case AG_EXP2:
	   			Av.AGRadar.MapMode = AG_EXP3;
				return;
			break;

			case AG_EXP3:
				Av.AGRadar.MapMode = -1;
				Av.AGRadar.ZoomMode = -1;
				Av.AGRadar.CurMode = AG_RBM_MODE;
				ResetAGRadar(Av.AGRadar.CurMode);
				return;
			break;
		}
	}
	else
		if (Av.AGRadar.CurMode == AG_RBM_MODE)
   			Av.AGRadar.MapMode = AG_EXP3;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGMenu()
{


}

//*****************************************************************************************************************************************
// AG DRAW  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawScanArea()
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(Av.AGRadar.MapMode == -1) return;

  FPointDouble *Box = &ScanAreasPixel[Av.AGRadar.MapMode][0];

	for(int j=0; j<3; j++)
 	  GrDrawLineClipped(GrBuffFor3D,Box[j].X,Box[j].Z,Box[j+1].X,Box[j+1].Z,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLineClipped(GrBuffFor3D,Box[3].X,Box[3].Z,Box[0].X,Box[0].Z,Triple.Red,Triple.Green,Triple.Blue,0);

		/*
	for(int i=0; i<3; i++)
	{
	    FPointDouble *Box = &ScanAreasPixel[i][0];

			for(int j=0; j<3; j++)
 	  		GrDrawLine(GrBuffFor3D,Box[j].X,Box[j].Z,Box[j+1].X,Box[j+1].Z,Triple.Red,Triple.Green,Triple.Blue,0);

			GrDrawLine(GrBuffFor3D,Box[3].X,Box[3].Z,Box[0].X,Box[0].Z,Triple.Red,Triple.Green,Triple.Blue,0);
	}
		 * */

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawScanAreaInZoom(int Mode)
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(Av.AGRadar.MapMode == -1) return;

	FPointDouble *Box = &ScanAreasZoomPixel[Mode][0];

	for(int j=0; j<3; j++)
 	  GrDrawLineClipped(GrBuffFor3D,Box[j].X,Box[j].Z,Box[j+1].X,Box[j+1].Z,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLineClipped(GrBuffFor3D,Box[3].X,Box[3].Z,Box[0].X,Box[0].Z,Triple.Red,Triple.Green,Triple.Blue,0);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAGBullseye()
{
	FPointDouble BELoc,ScreenLoc;

	if( (BullsEye.x <= 0) && (BullsEye.z <= 0) ) return;

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	BELoc.X = BullsEye.x*FTTOWU;
	BELoc.Z = BullsEye.z*FTTOWU;
	BELoc.Y = 0;

	if(Av.AGRadar.CurMode == AG_MAP_MODE)
		AGZoomInWorldToPixel(BELoc,&ScreenLoc);
	else
	  AGScanWorldToPixel(BELoc,&ScreenLoc);

	int Inside = IsPointInside(ScreenLoc.X,ScreenLoc.Z,AG_RADAR_TOP,AG_RADAR_BOTTOM,AG_RADAR_LEFT,AG_RADAR_RIGHT);

	if(!Inside) return;

	GrDrawPolyCircle(GrBuffFor3D,ScreenLoc.X,ScreenLoc.Z,4,20,Color.Red,Color.Green,Color.Blue,0);

	float TempX,TempZ;
	float AngleToNorth = 360.0 - (PlayerPlane->Heading/DEGREE);

	ProjectPoint(ScreenLoc.X,ScreenLoc.Z,NormDegree(AngleToNorth),11,&TempX,&TempZ);
	GrDrawLine(GrBuffFor3D,ScreenLoc.X,ScreenLoc.Z,TempX,TempZ,Color.Red,Color.Green,Color.Blue,0);
	DrawTriangleRot(TempX,TempZ,3,NormDegree(AngleToNorth),AV_GREEN,1.0,TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAGTRel()
{
	if(TRelSet)
	{
		int TempTRel = UFC.TRelCountDown;
		if(TempTRel > 99)
			TempTRel = 99;
		if(TempTRel < 0)
			TempTRel = 0;

		sprintf(TmpStr,"%d %s",TempTRel,"REL" );
		DrawTextAtLoc(93,9,TmpStr,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
// AG TEXT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AGRBMModeText(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  // MODE
  switch(Av.AGRadar.CurMode)
	{
	  case AG_RBM_MODE: sprintf(TmpStr,"MAP"); break;
		case AG_GMT_MODE: sprintf(TmpStr,"GMT"); break;
		case AG_SEA_MODE: sprintf(TmpStr,"SEA"); break;
	  case AG_TA_MODE:  sprintf(TmpStr,"TA");  break;
		case AG_MAP_MODE:  sprintf(TmpStr,"MAP"); break;
	}
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",(int)Av.AGRadar.Range);
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AGRadar.Azim*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// RSET
	if(Av.AGRadar.RsetTimer.IsActive())
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
	  if(Av.AGRadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AGRadar.RsetTimer.Disable();
	}
	else
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);
	}

	// SIL
	DrawVerticalWordAtButton(MpdNum,10,"SIL",(Av.AGRadar.SilenceOn),AV_GREEN,1.0);

	// FREEZE
	DrawVerticalWordAtButton(MpdNum,12,"FRZ",(Av.AGRadar.FreezeOn),AV_GREEN,1.0);

	// MAPS
	DrawWordAtMpdButton(MpdNum,19,"EXP1",(Av.AGRadar.MapMode == AG_EXP1),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,18,"EXP2",(Av.AGRadar.MapMode == AG_EXP2),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,17,"EXP3",(Av.AGRadar.MapMode == AG_EXP3),AV_GREEN,1.0);

	 // OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AG_RADAR_TOP - 7,TmpStr,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AG_RADAR_LEFT + 2,AG_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
	DrawTextAtLoc(AG_RADAR_LEFT - 4,AG_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AG_RADAR_LEFT - 12,AG_RADAR_TOP - 7,(Av.AGRadar.SilenceOn || UFC.EMISState) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// OWNSHIP ALT
	AGDrawOwnShipAltitude();

	// DECLUTTER
	DrawWordAtMpdButton(MpdNum,8,"DCLTR",(Av.AGRadar.DeclutterOn) ? TRUE : FALSE,AV_GREEN,1.0);

	// MENU
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawAGTRel();

	DrawMdiArrow(137,32,0);
	DrawMdiArrow(137,52,1);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGGMTModeText(int MpdNum)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	// MODE
	switch(Av.AGRadar.CurMode)
	{
		case AG_RBM_MODE: sprintf(TmpStr,"MAP"); break;
		case AG_GMT_MODE: sprintf(TmpStr,"GMT"); break;
		case AG_SEA_MODE: sprintf(TmpStr,"SEA"); break;
		case AG_TA_MODE:  sprintf(TmpStr,"TA");  break;
		case AG_MAP_MODE:  sprintf(TmpStr,"MAP"); break;
	}
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",(int)Av.AGRadar.Range);
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AGRadar.Azim*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// RSET
	if(Av.AGRadar.RsetTimer.IsActive())
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
	  if(Av.AGRadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AGRadar.RsetTimer.Disable();
	}
	else
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);
	}

	// SIL
	DrawVerticalWordAtButton(MpdNum,10,"SIL",(Av.AGRadar.SilenceOn || UFC.EMISState),AV_GREEN,1.0);

	// FREEZE
	DrawVerticalWordAtButton(MpdNum,12,"FRZ",(Av.AGRadar.FreezeOn),AV_GREEN,1.0);

	// Interleave
	DrawWordAtMpdButton(MpdNum,19,"INTL",(Av.AGRadar.InterleaveOn),AV_GREEN,1.0);

	 // OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AG_RADAR_TOP - 7,TmpStr,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AG_RADAR_LEFT + 2,AG_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
	DrawTextAtLoc(AG_RADAR_LEFT - 4,AG_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AG_RADAR_LEFT - 12,AG_RADAR_TOP - 7,(Av.AGRadar.SilenceOn || UFC.EMISState) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// OWNSHIP ALT
	AGDrawOwnShipAltitude();

	// DECLUTTER
	DrawWordAtMpdButton(MpdNum,8,"DCLTR",(Av.AGRadar.DeclutterOn) ? TRUE : FALSE,AV_GREEN,1.0);

	// MENU
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawAGTRel();

	DrawMdiArrow(137,32,0);
	DrawMdiArrow(137,52,1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGFTTModeText(int MpdNum)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(( (Av.AGRadar.TargetType == AG_REGISTERED_OBJECT) || (!Av.AGRadar.TargetMoving) ) && (Av.AGRadar.PrevMode != AG_GMT_MODE))
		sprintf(TmpStr,"FTT");
	else
		sprintf(TmpStr,"GMTT");

	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",(int)Av.AGRadar.Range);
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AGRadar.Azim*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	 // OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AG_RADAR_TOP - 7,TmpStr,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AG_RADAR_LEFT + 2,AG_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);


	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
	DrawTextAtLoc(AG_RADAR_LEFT - 4,AG_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AG_RADAR_LEFT - 12,AG_RADAR_TOP - 7,(Av.AGRadar.SilenceOn || UFC.EMISState) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// OWNSHIP ALT
	AGDrawOwnShipAltitude();

	// DECLUTTER
	DrawWordAtMpdButton(MpdNum,8,"DCLTR",(Av.AGRadar.DeclutterOn) ? TRUE : FALSE,AV_GREEN,1.0);

	// MENU
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// TRACK
	DrawTextAtLoc(61,118,"TRACK",AV_GREEN,1.0);

	DrawAGTRel();

}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSEAModeText(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  // MODE
  switch(Av.AGRadar.CurMode)
	{
		case AG_RBM_MODE: sprintf(TmpStr,"MAP"); break;
		case AG_GMT_MODE: sprintf(TmpStr,"GMT"); break;
		case AG_SEA_MODE: sprintf(TmpStr,"SEA"); break;
		case AG_TA_MODE:  sprintf(TmpStr,"TA");  break;
		case AG_MAP_MODE:  sprintf(TmpStr,"MAP"); break;
	}
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",(int)Av.AGRadar.Range);
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	// SCAN LIMIT
	sprintf(TmpStr,"%d",(int)(Av.AGRadar.Azim*2.0));
	DrawWordAtMpdButton(MpdNum,6,TmpStr,FALSE,AV_GREEN,1.0);

	// RSET
	if(Av.AGRadar.RsetTimer.IsActive())
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
	  if(Av.AGRadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AGRadar.RsetTimer.Disable();
	}
	else
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);
	}

	// Interleave
	DrawWordAtMpdButton(MpdNum,19,"INTL",(Av.AGRadar.InterleaveOn),AV_GREEN,1.0);

	// SIL
	DrawVerticalWordAtButton(MpdNum,10,"SIL",(Av.AGRadar.SilenceOn),AV_GREEN,1.0);

	// FREEZE
	DrawVerticalWordAtButton(MpdNum,12,"FRZ",(Av.AGRadar.FreezeOn),AV_GREEN,1.0);

	 // OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AG_RADAR_TOP - 7,TmpStr,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AG_RADAR_LEFT + 2,AG_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 	DrawTextAtLoc(AG_RADAR_LEFT - 4,AG_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AG_RADAR_LEFT - 12,AG_RADAR_TOP - 7,(Av.AGRadar.SilenceOn || UFC.EMISState) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// OWNSHIP ALT
	AGDrawOwnShipAltitude();

	// DECLUTTER
	DrawWordAtMpdButton(MpdNum,8,"DCLTR",(Av.AGRadar.DeclutterOn) ? TRUE : FALSE,AV_GREEN,1.0);

	// MENU
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawAGTRel();

	DrawMdiArrow(137,32,0);
	DrawMdiArrow(137,52,1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGTAModeText(int MpdNum)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	// MODE
	switch(Av.AGRadar.CurMode)
	{
		case AG_RBM_MODE: sprintf(TmpStr,"MAP"); break;
		case AG_GMT_MODE: sprintf(TmpStr,"GMT"); break;
		case AG_SEA_MODE: sprintf(TmpStr,"SEA"); break;
		case AG_TA_MODE:  sprintf(TmpStr,"TA");  break;
		case AG_MAP_MODE:  sprintf(TmpStr,"MAP"); break;
	}
	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	// RANGE
	sprintf(TmpStr,"%d",(int)Av.AGRadar.Range);
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	 // OWNSHIP HEADING
	sprintf(TmpStr,"%03d",(int)(NormDegree(360.0 - (PlayerPlane->Heading/DEGREE))));
 	DrawTextAtLoc(RADAR_HEADING_X,AG_RADAR_TOP - 7,TmpStr,AV_GREEN,1.0);

	// OWN AIRSPEED
	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(AG_RADAR_LEFT + 2,AG_RADAR_BOTTOM + 2,TmpStr,AV_GREEN,1.0);

	// OWN MACH
	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 	DrawTextAtLoc(AG_RADAR_LEFT - 4,AG_RADAR_BOTTOM + 8,TmpStr,AV_GREEN,1.0);

	//OPERATIONAL
	DrawTextAtLoc(AG_RADAR_LEFT - 12,AG_RADAR_TOP - 7,(Av.AGRadar.SilenceOn || UFC.EMISState) ? "STDBY" : "OPR",AV_GREEN,1.0);

	// OWNSHIP ALT
	AGDrawOwnShipAltitude();

	// DECLUTTER
	DrawWordAtMpdButton(MpdNum,8,"DCLTR",(Av.AGRadar.DeclutterOn) ? TRUE : FALSE,AV_GREEN,1.0);

	// MENU
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

  DrawMdiArrow(137,32,0);
  DrawMdiArrow(137,52,1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGMAPModeText(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);
  float smallest_dist,biggest_dist,dist;
  FPoint center;

  switch(Av.AGRadar.CurMode)
	{
	  case AG_RBM_MODE: sprintf(TmpStr,"MAP"); break;
		case AG_GMT_MODE: sprintf(TmpStr,"GMT"); break;
		case AG_SEA_MODE: sprintf(TmpStr,"SEA"); break;
	  case AG_TA_MODE:  sprintf(TmpStr,"TA");  break;
		case AG_MAP_MODE:  sprintf(TmpStr,"MAP"); break;
	}

	DrawWordAtMpdButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	int i;

	i = 4;
	center = 0.0;

	while (i--)
	{
		dist = PlayerPlane->WorldPosition / ScanAreasWorld[Av.AGRadar.MapMode][i];
//		center +=ScanAreasWorld[Av.AGRadar.MapMode][i];
		if (i==3)
		{
			smallest_dist = dist;
			biggest_dist = dist;
		}
		else
		{
			if (dist < smallest_dist)
				smallest_dist = dist;
			if (dist > biggest_dist)
				biggest_dist = dist;
		}
	}

	smallest_dist *= WUTONM;
	biggest_dist *= WUTONM;
//	center *= 0.25;

    // RANGES
	sprintf(TmpStr,"%2d",(int)(biggest_dist + 0.5f));
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_TOP + 2,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%2d",(int)(smallest_dist + 0.5f));
	DrawTextAtLoc(AG_RADAR_RIGHT + 2,AG_RADAR_BOTTOM + 4,TmpStr,AV_GREEN,1.0);

	//Bearing
	float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Av.AGRadar.WorldBoxCenter,1);

	if (DeltaAzim >= 0.5)
		sprintf(TmpStr,"%2dL",(int)floor(DeltaAzim + 0.5f));
	else
		if (DeltaAzim < -0.5)
			sprintf(TmpStr,"%2dR",(int)-floor(DeltaAzim + 0.5f));
		else
			sprintf(TmpStr,"0");

	DrawTextAtLoc(AG_RADAR_CENTER_X - 2*strlen(TmpStr),AG_RADAR_TOP - 7,TmpStr,AV_GREEN,1.0);

	// RSET
	if(Av.AGRadar.RsetTimer.IsActive())
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",TRUE,AV_GREEN,1.0);
	  if(Av.AGRadar.RsetTimer.TimeUp( GetTickCount() ) )
			Av.AGRadar.RsetTimer.Disable();
	}
	else
	{
	  DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);
	}

    // MAPS
	DrawWordAtMpdButton(MpdNum,19,"EXP1",(Av.AGRadar.MapMode == AG_EXP1),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,18,"EXP2",(Av.AGRadar.MapMode == AG_EXP2),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,17,"EXP3",(Av.AGRadar.MapMode == AG_EXP3),AV_GREEN,1.0);

	// SIL
	DrawVerticalWordAtButton(MpdNum,10,"SIL",(Av.AGRadar.SilenceOn),AV_GREEN,1.0);

	// FREEZE
	DrawVerticalWordAtButton(MpdNum,12,"FRZ",(Av.AGRadar.FreezeOn),AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//**********z*******************************************************************************************************************************
void DrawScanBeamEdgeTable();
void CallDrawScanBeamEdgeTable( void)
{
	PushClipRegion();

	if( GrBuffFor3D->pGrBuffPolygon)
	{
		int XOfs = 70;
		int YOfs = 70;
		//calc a mid pt for the displays
		GrBuffFor3D->ClipLeft =AG_RADAR_LEFT;// XOfs - (140 >> 1);
		GrBuffFor3D->ClipRight = AG_RADAR_RIGHT;//GrBuffFor3D->ClipLeft + 140;

		GrBuffFor3D->ClipTop = AG_RADAR_TOP;//YOfs - (140 >> 1);
		GrBuffFor3D->ClipBottom = AG_RADAR_BOTTOM;//GrBuffFor3D->ClipTop + 140;

		GrBuffFor3D->MidX = XOfs;
		GrBuffFor3D->MidY = YOfs;

		// Turn on 2nd pass
	 	SetSecondPass( TRUE, NULL );

		Set3DScreenClip();

	}


	DrawScanBeamEdgeTable();

	  if( GrBuffFor3D->pGrBuffPolygon)
		 SetSecondPass( FALSE, NULL);


  PopClipRegion();
  Set3DScreenClip();
  PushClipRegion();
}

void DrawMaskPolys();
void CallDrawMaskPolys(void)
{
	if(Av.AGRadar.CurMode == AG_MAP_MODE)
	{
		PushClipRegion();

		if( GrBuffFor3D->pGrBuffPolygon)
		{
			int XOfs = 70;
			int YOfs = 70;
			//calc a mid pt for the displays
			GrBuffFor3D->ClipLeft =AG_RADAR_LEFT;// XOfs - (140 >> 1);
			GrBuffFor3D->ClipRight = AG_RADAR_RIGHT;//GrBuffFor3D->ClipLeft + 140;

			GrBuffFor3D->ClipTop = AG_RADAR_TOP;//YOfs - (140 >> 1);
			GrBuffFor3D->ClipBottom = AG_RADAR_BOTTOM;//GrBuffFor3D->ClipTop + 140;

			GrBuffFor3D->MidX = XOfs;
			GrBuffFor3D->MidY = YOfs;

			// Turn on 2nd pass
	 		SetSecondPass( TRUE, NULL );

			Set3DScreenClip();

		}
		DrawMaskPolys();

	  	if( GrBuffFor3D->pGrBuffPolygon)
		 	SetSecondPass( FALSE, NULL);


  		PopClipRegion();
  		Set3DScreenClip();
  		PushClipRegion();
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DisplayAGRadarBackground()
{
  	AvRGB Triple;
  	GetRGBFromAVPalette(AV_GREEN,0.75,&Triple);

	float pix,count;

	count = (AG_RADAR_BOTTOM - AG_RADAR_TOP)/4.0f;
	pix = count;
	int i = 4;
	float left,right;

	while(i--)
	{
		if (pix > (AG_RADAR_CENTER_X-AG_RADAR_LEFT))
		{
			left = 180.0f + acos((float)(AG_RADAR_CENTER_X-AG_RADAR_LEFT)/pix)*RADIANS_TO_DEGREES;
			right = 360.0f - acos((float)(AG_RADAR_RIGHT-AG_RADAR_CENTER_X)/pix)*RADIANS_TO_DEGREES;
		}
		else
		{
			left = 210;
			right = 330;
		}

		GrDrawPolyCircleArcPolyBuff( GrBuffFor3D, AG_RADAR_CENTER_X,AG_RADAR_BOTTOM, pix-1, pix,  left, right, 64,Triple.Red,Triple.Green,Triple.Blue,FALSE);//110
		pix += count;
	}

  	GrDrawLine(GrBuffFor3D,AG_RADAR_CENTER_X,AG_RADAR_BOTTOM,AG_RADAR_CENTER_X,AG_RADAR_TOP,Triple.Red,Triple.Green,Triple.Blue,0);

  	GrDrawLine(GrBuffFor3D,AG_RADAR_CENTER_X,AG_RADAR_BOTTOM,AG_RADAR_RIGHT,AG_RADAR_BOTTOM - (AG_RADAR_RIGHT-AG_RADAR_CENTER_X)*0.5774,Triple.Red,Triple.Green,Triple.Blue,0);//30 degree
  	GrDrawLine(GrBuffFor3D,AG_RADAR_CENTER_X,AG_RADAR_BOTTOM,AG_RADAR_LEFT,AG_RADAR_BOTTOM - (AG_RADAR_RIGHT-AG_RADAR_CENTER_X)*0.5774,Triple.Red,Triple.Green,Triple.Blue,0);

  	GrDrawLine(GrBuffFor3D,AG_RADAR_CENTER_X,AG_RADAR_BOTTOM,AG_RADAR_RIGHT,AG_RADAR_BOTTOM - (AG_RADAR_RIGHT-AG_RADAR_CENTER_X)*1.7321,Triple.Red,Triple.Green,Triple.Blue,0);  // 60 degree
  	GrDrawLine(GrBuffFor3D,AG_RADAR_CENTER_X,AG_RADAR_BOTTOM,AG_RADAR_LEFT,AG_RADAR_BOTTOM - (AG_RADAR_RIGHT-AG_RADAR_CENTER_X)*1.7321,Triple.Red,Triple.Green,Triple.Blue,0);

}

void AGRBMMode(int MpdNum)
{
	// jlm debug los
	//DrawLOS();
	//return;

	if(!Av.AGRadar.SilenceOn && !UFC.EMISState)
    CallDrawScanBeamEdgeTable();


  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	PushClipRegion();
	SetClipRegion(AG_RADAR_LEFT, AG_RADAR_RIGHT, AG_RADAR_TOP, AG_RADAR_BOTTOM,GrBuffFor3D->MidY,GrBuffFor3D->MidX);

	DisplayAGRadarBackground();
//  DrawAvRadarSymbol(AG_RADAR_LEFT,AG_RADAR_TOP,AV_AG_BACKGROUND,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	if(Av.AGRadar.MapMode == -1)
	{
	  Av.AGRadar.CaptBarsOn = TRUE;

		int JoystickUsed = SimCaptBars();
    if(!JoystickUsed)
      SimCaptBarsForKeyboard();
	}
	else
	{
	  	Av.AGRadar.CaptBarsOn = FALSE;

    	int JoystickUsed = AGSimScanArea();

		if(!JoystickUsed)
			 SimScanAreaForKeyboard();

		FixScanBoxBoundaries();
	}



  FPointDouble World,Screen;
  Screen.X = 48;
  Screen.Z = 49;

  FPointDouble Temp = PlayerPlane->WorldPosition;

  AGScanPixelToWorld(Screen,&World);
  AGScanWorldToPixel(World,&Screen);
  AGScanPixelToWorld(Screen,&World);

  PlayerPlane->WorldPosition = Temp;

  AGSetScanAreas();
  AGSetBoundArea(Av.AGRadar.MapMode);

  DrawScanArea();
  //DrawBoundArea();

  if(!Av.AGRadar.DeclutterOn)
	 DrawAttitudeIndicator(AG_RADAR_CENTER_X,AG_RADAR_CENTER_Y);

	if(Av.AGRadar.MapMode == -1)
	   DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,1.0);

	DrawAGBeam();

	DrawAGBullseye();

	PopClipRegion();

  AGRBMModeText(MpdNum);


	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
               break;
		//	case  2: AGSetAirMode();
			case  6: AGAzimScanChange();
			         break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  8: AGToggleDeclutter();
			         break;
			case  9: break;
			case 10: AGToggleSilent();
			         break;
			case 11: AGRSET();
			         break;
  		case 12: AGToggleFreeze();
			         break;
			case 13: AGRangeDown();
							 break;
			case 14: AGRangeUp();
			         break;
			case 17: AGSetExp3();
			         break;
			case 18: AGSetExp2();
			         break;
			case 19: AGSetExp1();
			         break;

		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGGMTMode(int MpdNum)
{
	if( (!Av.AGRadar.SilenceOn && !UFC.EMISState) && (Av.AGRadar.InterleaveOn) )
    CallDrawScanBeamEdgeTable();

  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DisplayAGRadarBackground();
//  DrawAvRadarSymbol(AG_RADAR_LEFT,AG_RADAR_TOP,AV_AG_BACKGROUND,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	PushClipRegion();
	SetClipRegion(AG_RADAR_LEFT, AG_RADAR_RIGHT, AG_RADAR_TOP, AG_RADAR_BOTTOM,GrBuffFor3D->MidY,GrBuffFor3D->MidX);

  Av.AGRadar.CaptBarsOn = TRUE;

  int JoystickUsed = SimCaptBars();
  if(!JoystickUsed)
    SimCaptBarsForKeyboard();

  if(!Av.AGRadar.DeclutterOn)
	 DrawAttitudeIndicator(AG_RADAR_CENTER_X,AG_RADAR_CENTER_Y);

	DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,1.0);

	DrawAGBeam();

	PopClipRegion();

  AGGMTModeText(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
               break;
		//	case  2: AGSetAirMode();
			case  6: AGAzimScanChange();
			         break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  8: AGToggleDeclutter();
			         break;
			case  9: break;
			case 10: AGToggleSilent();
			         break;
			case 11: AGRSET();
			         break;
  		case 12: AGToggleFreeze();
			         break;
			case 13: AGRangeDown();
							 break;
			case 14: AGRangeUp();
			         break;
			case 19:  AGToggleInterleave();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGSEAMode(int MpdNum)
{
	if( (!Av.AGRadar.SilenceOn && !UFC.EMISState) )
    CallDrawScanBeamEdgeTable();

	PushClipRegion();
	SetClipRegion(AG_RADAR_LEFT, AG_RADAR_RIGHT, AG_RADAR_TOP, AG_RADAR_BOTTOM,GrBuffFor3D->MidY,GrBuffFor3D->MidX);

	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DisplayAGRadarBackground();
//  DrawAvRadarSymbol(AG_RADAR_LEFT,AG_RADAR_TOP,AV_AG_BACKGROUND,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

  Av.AGRadar.CaptBarsOn = TRUE;

  int JoystickUsed = SimCaptBars();
  if(!JoystickUsed)
    SimCaptBarsForKeyboard();

  if(!Av.AGRadar.DeclutterOn)
	 DrawAttitudeIndicator(AG_RADAR_CENTER_X,AG_RADAR_CENTER_Y);

	DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,1.0);

	DrawAGBeam();

	DrawAGBullseye();

  PopClipRegion();

  AGSEAModeText(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
               break;
		//	case  2: AGSetAirMode();
			case  6: AGAzimScanChange();
			         break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  8: AGToggleDeclutter();
			         break;
			case  9: break;
			case 10: AGToggleSilent();
			         break;
			case 11: AGRSET();
			         break;
  		case 12: AGToggleFreeze();
			         break;
			case 13: AGRangeDown();
							 break;
			case 14: AGRangeUp();
			         break;
			case 19:  AGToggleInterleave();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGTAMode(int MpdNum)
{
	if(!Av.AGRadar.SilenceOn && !UFC.EMISState)
		CallDrawScanBeamEdgeTable();

	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DisplayAGRadarBackground();
//  DrawAvRadarSymbol(AG_RADAR_LEFT,AG_RADAR_TOP,AV_AG_BACKGROUND,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	PushClipRegion();
	SetClipRegion(AG_RADAR_LEFT, AG_RADAR_RIGHT, AG_RADAR_TOP, AG_RADAR_BOTTOM,GrBuffFor3D->MidY,GrBuffFor3D->MidX);

	Av.AGRadar.CaptBarsOn = FALSE;

  if(!Av.AGRadar.DeclutterOn)
	 DrawAttitudeIndicator(AG_RADAR_CENTER_X,AG_RADAR_CENTER_Y);

  AGTAModeText(MpdNum);

  DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,1.0);
	DrawAGBeam();

	DrawAGBullseye();

	PopClipRegion();

  AGTAModeText(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
               break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  8: AGToggleDeclutter();
			         break;
			case 13: AGRangeDown();
							 break;
			case 14: AGRangeUp();
			         break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Exp1SubMode()
{
	 FPointDouble Points[4];

	 memcpy(Points,&ScanAreasPixel[AG_EXP1][0],sizeof(FPointDouble)*4);

	 FPointDouble *Ptr = &Points[0];

	 while(Ptr < &Points[4])
	 {
		 Ptr->Y = Ptr->Z;
		 Ptr->Z = 0;
		 Ptr++;
	 }

	 Ptr = &Points[0];

	 ZoomRBMap(*Ptr,*(Ptr+1),*(Ptr+2),*(Ptr+3));

	 if(!Av.AGRadar.SilenceOn && !UFC.EMISState)
     	CallDrawScanBeamEdgeTable();

//   SetMapModeWorldToZoomPixel(AG_EXP1);
//   SetBoundZoomWorldToPixel(AG_EXP1);

   	SetMapModeWorldToZoomPixel(AG_EXP1);

   	AGSetZoomBoundArea(AG_EXP1);
   	SetBoundZoomWorldToPixel(AG_EXP1);

   //DrawScanAreaInZoom(AG_EXP1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Exp2SubMode()
{
	 FPointDouble Points[4];

	 memcpy(Points,&ScanAreasPixel[AG_EXP2][0],sizeof(FPointDouble)*4);

	 FPointDouble *Ptr = &Points[0];

	 while(Ptr < &Points[4])
	 {
		 Ptr->Y = Ptr->Z;
		 Ptr->Z = 0;
		 Ptr++;
	 }

	 Ptr = &Points[0];

	 ZoomRBMap(*Ptr,*(Ptr+1),*(Ptr+2),*(Ptr+3));
   if(!Av.AGRadar.SilenceOn && !UFC.EMISState)
	   CallDrawScanBeamEdgeTable();

   SetMapModeWorldToZoomPixel(AG_EXP2);
//   DrawScanAreaInZoom(AG_EXP2);

   AGSetZoomBoundArea(AG_EXP2);
   SetBoundZoomWorldToPixel(AG_EXP2);
   //DrawZoomBoundArea(AG_EXP2);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Exp3SubMode()
{
	 FPointDouble Points[4];

	 memcpy(Points,&ScanAreasPixel[AG_EXP3][0],sizeof(FPointDouble)*4);

	 FPointDouble *Ptr = &Points[0];

	 while(Ptr < &Points[4])
	 {
		 Ptr->Y = Ptr->Z;
		 Ptr->Z = 0;
		 Ptr++;
	 }

	 Ptr = &Points[0];

	 ZoomRBMap(*Ptr,*(Ptr+1),*(Ptr+2),*(Ptr+3));
	 if(!Av.AGRadar.SilenceOn && !UFC.EMISState)
  	 CallDrawScanBeamEdgeTable();

   SetMapModeWorldToZoomPixel(AG_EXP3);

   AGSetZoomBoundArea(AG_EXP3);
   SetBoundZoomWorldToPixel(AG_EXP3);
   //DrawZoomBoundArea(AG_EXP3);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGMAPMode(int MpdNum)
{
	int ButtonPress = GetButtonPress(MpdNum);

	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 17: AGSetExp3();
			         break;
			case 18: AGSetExp2();
			         break;
			case 19: AGSetExp1();
			         break;
		}
	}

	Av.AGRadar.BoxRot = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition, PlayerPlane->Heading, Av.AGRadar.WorldBoxCenter, TRUE);
	Av.AGRadar.BoxTranslate = -Dist2D(&PlayerPlane->WorldPosition,&Av.AGRadar.WorldBoxCenter)*WUTONM*AG_SCAN_PIXELS_PER_MILE(Av.AGRadar.Range);

	if(Av.AGRadar.ZoomMode == Av.AGRadar.MapMode)
	{
	    Av.AGRadar.CaptBarsOn = TRUE;

  		int JoystickUsed = SimCaptBars();
  		if(!JoystickUsed)
     		SimCaptBarsForKeyboard();
	}
	else
	{
    //AGSimScanArea();
	  Av.AGRadar.CaptBarsOn = FALSE;

    	int JoystickUsed = AGSimScanArea();
		if(!JoystickUsed)
			 SimScanAreaForKeyboard();
	}

	FixScanBoxBoundaries();


  	FPointDouble World,Screen;
  	Screen.X = 48;
  	Screen.Z = 49;

  	FPointDouble Temp = PlayerPlane->WorldPosition;

  	AGScanPixelToWorld(Screen,&World);
  	AGScanWorldToPixel(World,&Screen);
  	AGScanPixelToWorld(Screen,&World);

  	PlayerPlane->WorldPosition = Temp;

  	AGSetScanAreas();
  	AGSetBoundArea(Av.AGRadar.ZoomMode);


	switch(Av.AGRadar.ZoomMode)
	{
		case AG_EXP1: Exp1SubMode(); break;
		case AG_EXP2: Exp2SubMode(); break;
		case AG_EXP3: Exp3SubMode(); break;
	}

	AGMAPModeText(MpdNum);

	if(Av.AGRadar.ZoomMode == Av.AGRadar.MapMode)
	{
	  DrawCaptBars(CaptBars.Pos.X,CaptBars.Pos.Y,AV_GREEN,1.0);
	}
	else
	   DrawScanAreaInZoom(Av.AGRadar.MapMode);



	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
				       break;

			case  6: break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  8: break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGGmttMode(int MpdNum)
{

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
               break;
			case  6: break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  8: break;

			case  9: break;

			case 10: break;

			case 11: break;

  		case 13: break;

			case 14: break;

			case 17: break;

		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void BreakAGLock()
{
	if(Av.AGRadar.CurMode != AG_FTT_MODE) return;

  PlayerPlane->AGDesignate.X = -1;
	Av.AGRadar.TargetType = -1;
	Av.AGRadar.Target = NULL;
	if(Av.AGRadar.PrevMode != -1)
	{
		Av.AGRadar.CurMode = Av.AGRadar.PrevMode;
	  ResetAGRadar(Av.AGRadar.CurMode);
		Av.AGRadar.PrevMode = -1;
	}
	else
	{
		Av.AGRadar.CurMode = AG_RBM_MODE;
	  ResetAGRadar(Av.AGRadar.CurMode);
		Av.AGRadar.PrevMode = -1;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGFttMode(int MpdNum)
{
	int breaklock = 0;
	long lstatus;

	Av.AGRadar.CaptBarsOn = FALSE;

  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DisplayAGRadarBackground();
//  DrawAvRadarSymbol(AG_RADAR_LEFT,AG_RADAR_TOP,AV_AG_BACKGROUND,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	PushClipRegion();
	SetClipRegion(AG_RADAR_LEFT, AG_RADAR_RIGHT, AG_RADAR_TOP, AG_RADAR_BOTTOM,GrBuffFor3D->MidY,GrBuffFor3D->MidX);

  //CallDrawScanBeamEdgeTable();

	FPointDouble ScreenLoc,Intersection;

	if(Av.AGRadar.TargetType == AG_REGISTERED_OBJECT)
	{
	  AGScanWorldToPixel( ((BasicInstance *)Av.AGRadar.Target)->Position,&ScreenLoc);
	  PlayerPlane->AGDesignate = ((BasicInstance *)Av.AGRadar.Target)->Position;
    PlayerPlane->AGDesignate.Y += 5.0*FTTOWU;  //  WUTOFT;
		PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_RADAR;
	}
	else
	{
   		AGScanWorldToPixel( ((MovingVehicleParams *)Av.AGRadar.Target)->WorldPosition,&ScreenLoc);
	  	PlayerPlane->AGDesignate = ((MovingVehicleParams *)Av.AGRadar.Target)->WorldPosition;

		if (((MovingVehicleParams *)Av.AGRadar.Target)->iShipType)
		{
			if  (PlayerPlane->AGDesignate.Y < (3 FEET))
				PlayerPlane->AGDesignate.Y = 3 FEET;
			else
				PlayerPlane->AGDesignate.Y += 3 FEET;
		}

		PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_RADAR ;

		if(Av.AGRadar.PrevMode == AG_GMT_MODE)
		{
			lstatus = ((MovingVehicleParams *)Av.AGRadar.Target)->Status;
			if(!(( ((MovingVehicleParams *)Av.AGRadar.Target)->fSpeedFPS > 2.0) && (!(lstatus & (VL_DESTROYED|VL_WAITING|VL_DONE_MOVING))) && (lstatus & VL_MOVING)))
			{
				breaklock = 1;
			}
		}
	}

	int LOSMaintained = LOS(&(PlayerPlane->WorldPosition),&PlayerPlane->AGDesignate, &Intersection, MED_RES_LOS, FALSE);

  float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,PlayerPlane->AGDesignate,TRUE);
  if( (DeltaAzim > 60.0) || (DeltaAzim < -60.0)  )
		 LOSMaintained = FALSE;

	if(!LOSMaintained || Av.AGRadar.SilenceOn || UFC.EMISState || breaklock)
	{
		BreakAGLock();
	}

	// auto range
	if(Av.AGRadar.Target)
	{
		float Dist = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);

		while( (Dist <= (0.35*Av.AGRadar.Range*NMTOWU)) && (Av.AGRadar.Range > 5.0) )
			Av.AGRadar.Range /= 2.0;
	}

	if(Av.AGRadar.TargetType == AG_REGISTERED_OBJECT)
      DrawRadarBlip(ScreenLoc.X,ScreenLoc.Z,AV_GREEN,1.0);
	else
	{
      DrawRadarBlip(ScreenLoc.X,ScreenLoc.Z,AV_GREEN,1.0);

			if(Av.AGRadar.Target)
			{
					sprintf(TmpStr,"%2.1f",((MovingVehicleParams *)Av.AGRadar.Target)->fSpeedFPS*FTTONM*3600.0);
					DrawTextAtLoc(ScreenLoc.X-16,ScreenLoc.Z-2,TmpStr,AV_GREEN,1.0);
					sprintf(TmpStr,"%d",(int)( NormDegree( 360.0 - ((MovingVehicleParams *)Av.AGRadar.Target)->Heading/DEGREE) ) );
					DrawTextAtLoc(ScreenLoc.X+6,ScreenLoc.Z-2,TmpStr,AV_GREEN,1.0);
			}
	}

	if( (Av.AGRadar.Target) && (Av.AGRadar.TargetMoving) )
     DrawPlaneVelocityVector(ScreenLoc.X,ScreenLoc.Z,( (MovingVehicleParams *)Av.AGRadar.Target)->Heading/DEGREE,AV_GREEN,1.0);

  if(!Av.AGRadar.DeclutterOn)
	 DrawAttitudeIndicator(AG_RADAR_CENTER_X,AG_RADAR_CENTER_Y);

	PopClipRegion();

  AGFTTModeText(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: AGChangeMode();
               break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


void DoAGRadar(int MpdNum)
{

  NumProcessedLOS = 0;


	int ShowObjects = TRUE;

	Av.AGRadar.CaptBarsOn = TRUE;

  switch (Av.AGRadar.CurMode)
  {
    case AG_RBM_MODE:
    {
      AGRBMMode(MpdNum);
	    break;
    }
    case AG_GMT_MODE:
    {
	    AGGMTMode(MpdNum);
	    break;
    }
    case AG_SEA_MODE:
    {
	    AGSEAMode(MpdNum);
      break;
    }
    case AG_TA_MODE:
    {
	    AGTAMode(MpdNum);
			ShowObjects = FALSE;
	    break;
    }
    case AG_MAP_MODE:
    {
	    AGMAPMode(MpdNum);
	    break;
    }
    case AG_FTT_MODE:
		{
			AGFttMode(MpdNum);
			ShowObjects = FALSE;
			break;
		}
	}

	if(ShowObjects && !Av.AGRadar.SilenceOn && !UFC.EMISState)
	{
		if( (Av.AGRadar.CurMode != AG_GMT_MODE) && (Av.AGRadar.CurMode != AG_SEA_MODE) && (Av.AGRadar.CurMode != AG_TA_MODE)  )
		{
			FilterAGObjects();
			SetAGObjects();
  			DrawAGObjects();
		}

		if( (Av.AGRadar.CurMode == AG_GMT_MODE) || (Av.AGRadar.CurMode == AG_SEA_MODE) || (Av.AGRadar.CurMode == AG_RBM_MODE) || (Av.AGRadar.CurMode == AG_MAP_MODE)  )
		{
  			SetAGVehicles();
     		DrawAGVehicles();
		}


	}

	if(Av.AGRadar.CurMode == AG_MAP_MODE)
		CallDrawMaskPolys();

	if(Av.AGRadar.SilenceOn || UFC.EMISState)
	{
		AGGroundObjs.NumGroundObj = 0;
		AGVehicles.NumVehicles = 0;
	}


	if (Av.AGRadar.CurMode != AG_MAP_MODE)
	{
		AvRGB Triple;
		GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

		GrDrawLine(GrBuffFor3D,AG_RADAR_LEFT,AG_RADAR_TOP,AG_RADAR_LEFT,AG_RADAR_BOTTOM,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,AG_RADAR_LEFT,AG_RADAR_BOTTOM,AG_RADAR_RIGHT,AG_RADAR_BOTTOM,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,AG_RADAR_RIGHT,AG_RADAR_BOTTOM,AG_RADAR_RIGHT,AG_RADAR_TOP,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,AG_RADAR_RIGHT,AG_RADAR_TOP,AG_RADAR_LEFT,AG_RADAR_TOP,Triple.Red,Triple.Green,Triple.Blue,0);
	}

	DrawTDCIndicator(MpdNum);
}

//*****************************************************************************************************************************************
// GROUND OBJ LIST MANAGE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


int IsGroundObjInList(DetectedGroundObjListType *List,int Id)
{
	DetectedGroundObjType *w = &List->GroundObjs[List->NumGroundObj];
	while(w-- > List->GroundObjs)
		if (w->Id == Id)
			return TRUE;
	return FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetGroundObjPosInList(DetectedGroundObjListType *List,int Id)
{
  for(int i=0; i <List->NumGroundObj; i++)
  {
   if(List->GroundObjs[i].Id == Id)
     return(i);
  }
  return(-1);
}

DetectedGroundObjType *GetGroundObjPtrInList(DetectedGroundObjListType *List,int Id)
{
	DetectedGroundObjType *w = &List->GroundObjs[List->NumGroundObj];
	while(w-- > List->GroundObjs)
		if (w->Id == Id)
			return w;
	return NULL;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CopyGroundObjEntry(DetectedGroundObjType *From, DetectedGroundObjType *To)
{
	memcpy(To,From,sizeof(DetectedGroundObjType));
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddGroundObjToList(DetectedGroundObjListType *List,DetectedGroundObjType *TempInfo)
{
	if(List->NumGroundObj + 1 < AG_MAX_DET_GROUND_OBJS)
	{
	  List->NumGroundObj++;
	  CopyGroundObjEntry(TempInfo, &(List->GroundObjs[List->NumGroundObj-1]));
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RemoveGroundObjFromList(DetectedGroundObjListType *List, int Id)
{
	DetectedGroundObjType *w = &List->GroundObjs[List->NumGroundObj];

	while(w-- > List->GroundObjs)
	if (w->Id == Id)
	{
		if (List->NumGroundObj-- > 1)
		  	memcpy(w,&(List->GroundObjs[List->NumGroundObj]),sizeof(DetectedGroundObjType));
		break;
	}
}

void StickGroundObjInList(DetectedGroundObjListType *List,DetectedGroundObjType *go)
{
	DetectedGroundObjType *w = &List->GroundObjs[List->NumGroundObj];

	while(w-- > List->GroundObjs)
		if (w->Id == go->Id)
		{
			memcpy(w,go,sizeof(DetectedGroundObjType));
			return;
		}

	if(List->NumGroundObj < (AG_MAX_DET_GROUND_OBJS-1))
		memcpy(&List->GroundObjs[List->NumGroundObj++],go,sizeof(DetectedGroundObjType));

}

void RefreshGroundObjList(DetectedGroundObjListType *List,DWORD time)
{
	DetectedGroundObjType *w = &List->GroundObjs[List->NumGroundObj];
	DetectedGroundObjType *list_end = w-1;
	BOOL at_end = TRUE;

	while(w-- > List->GroundObjs)
	{
		if ((AGRadarFrame - w->RadarFrame) > time)
		{
			if (!at_end)
				memcpy(w,list_end,sizeof(DetectedGroundObjType));
			list_end--;

			List->NumGroundObj--;
		}
		else
			at_end = FALSE;
	}
}



//*****************************************************************************************************************************************
//  VEHICLE LIST MANAGE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


int IsVehicleInList(DetectedVehicleListType *List,int Id)
{
	DetectedVehicleType *w = &List->Vehicles[List->NumVehicles];
	while(w-- > List->Vehicles)
		if (w->Id == Id)
			return TRUE;
	return FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetVehiclePosInList(DetectedVehicleListType *List,int Id)
{
  for(int i=0; i <List->NumVehicles; i++)
  {
   if(List->Vehicles[i].Id == Id)
     return(i);
  }
  return(-1);
}

DetectedVehicleType *GetVehiclePtrInList(DetectedVehicleListType *List,int Id)
{
	DetectedVehicleType *w = &List->Vehicles[List->NumVehicles];
	while(w-- > List->Vehicles)
		if (w->Id == Id)
			return w;
	return NULL;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CopyVehicleEntry(DetectedVehicleType *From, DetectedVehicleType *To)
{
	memcpy(To,From,sizeof(DetectedVehicleType));
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddVehicleToList(DetectedVehicleListType *List,DetectedVehicleType *TempInfo)
{
	if((List->NumVehicles + 1) < AG_MAX_DET_VEHICLES)
	{
	  List->NumVehicles++;
	  CopyVehicleEntry(TempInfo, &(List->Vehicles[List->NumVehicles-1]));
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RemoveVehicleFromList(DetectedVehicleListType *List, int Id)
{
	DetectedVehicleType *w = &List->Vehicles[List->NumVehicles];

	while(w-- > List->Vehicles)
	if (w->Id == Id)
	{
		if (List->NumVehicles-- > 1)
		  	memcpy(w,&(List->Vehicles[List->NumVehicles]),sizeof(DetectedVehicleType));
		break;
	}
}

void StickVehicleInList(DetectedVehicleListType *List,DetectedVehicleType *go)
{
	DetectedVehicleType *w = &List->Vehicles[List->NumVehicles];

	while(w-- > List->Vehicles)
		if (w->Id == go->Id)
		{
			memcpy(w,go,sizeof(DetectedVehicleType));
			return;
		}

	if(List->NumVehicles < (AG_MAX_DET_VEHICLES-1))
		memcpy(&List->Vehicles[List->NumVehicles++],go,sizeof(DetectedVehicleType));

}

void RefreshVehicleList(DetectedVehicleListType *List,DWORD time)
{
	DetectedVehicleType *w = &List->Vehicles[List->NumVehicles];
	DetectedVehicleType *list_end = w-1;
	BOOL at_end = TRUE;

	while(w-- > List->Vehicles)
	{
		if ((AGRadarFrame - w->RadarFrame) > time)
		{
			if (!at_end)
				memcpy(w,list_end,sizeof(DetectedVehicleType));
			list_end--;

			List->NumVehicles--;
		}
		else
			at_end = FALSE;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void UpdateAGRadarScan()
{
  	Av.AGRadar.AntAzimuth += (Av.AGRadar.ScanDirection == LTOR)
		                  ? -(1.8) : (1.8);

	//Adjust scale line here....
	ScanBeamEdges[ScanBeamEdge].t = NUM_SCAN_LINES*2.0f;

	if( Av.AGRadar.ScanDirection == LTOR )
		ScanBeamEdge++;
	else
		ScanBeamEdge--;

	if(ScanBeamEdge < 0)  ScanBeamEdge = 0;
	if(ScanBeamEdge > 68) ScanBeamEdge = 68;

	for(int i=0; i<NUM_SCAN_LINES; i++)
	{
		 ScanBeamEdges[i].t--;
		 if( ScanBeamEdges[i].t < 0.0f)
			 ScanBeamEdges[i].t =0.0f;
	}

	if(Av.AGRadar.AntAzimuth >= Av.AGRadar.Azim)
	{
	   Av.AGRadar.ScanDirection = LTOR;
	   AGRadarFrame++;
	}
	else if(Av.AGRadar.AntAzimuth <= -Av.AGRadar.Azim)
	{
	   Av.AGRadar.ScanDirection = RTOL;
	   AGRadarFrame++;
	}

  	ProjectPoint(AG_RADAR_ORIGIN_X, AG_RADAR_ORIGIN_Y, NormDegree(Av.AGRadar.AntAzimuth),Av.AGRadar.Range*AG_SCAN_PIXELS_PER_MILE(Av.AGRadar.Range), &Av.AGRadar.BeamEndPointPixel.X, &Av.AGRadar.BeamEndPointPixel.Z);

  	ProjectPointDouble(PlayerPlane->WorldPosition.X, PlayerPlane->WorldPosition.Z, NormDegree(Av.AGRadar.AntAzimuth + fANGLE_TO_DEGREES(PlayerPlane->Heading)),Av.AGRadar.Range*NMTOWU, &Av.AGRadar.BeamEndPointWorld.X, &Av.AGRadar.BeamEndPointWorld.Z);
  	Av.AGRadar.BeamEndPointWorld.Y = 0.0f;

	RefreshGroundObjList(&AGGroundObjs,1);
	RefreshVehicleList(&AGVehicles,1);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsInAGRadarLOSList(int Type, void *Obj, int *Pos)
{
	AGRadarLOSType *Walk = &AGRadarLOSList[0];
	int Index = NumAGRadarLOS;
	while(Index-- > 0)
	{
		if(Walk->Type == Type)
		{
			if(Type == AG_REGISTERED_OBJECT)
			{
				if(Obj == Walk->Obj)
				{
					 *Pos = Walk - &AGRadarLOSList[0];
		       return(TRUE);
				}
			}
			else
			{
				if(Obj == Walk->Veh)
				{
					 *Pos = Walk - &AGRadarLOSList[0];
					 return(TRUE);
				}
			}
		}
		Walk++;
	}

	return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddToAGRadarLOSList(int Type, void *Obj, int LOS)
{
	if(NumAGRadarLOS + 1 < MAX_AGRADAR_LOS_OBJS)
	{
		NumAGRadarLOS++;
		AGRadarLOSType *To = &AGRadarLOSList[NumAGRadarLOS-1];
		To->Type = Type;
		To->Frame = 0;
		To->LOS   = LOS;
		To->MaxFrame = 3 + (rand() % 5);
		if(Type == AG_REGISTERED_OBJECT)
		{
			To->Obj = (BasicInstance *)Obj;
			To->Veh = NULL;
		}
		else
		{
			To->Obj = NULL;
			To->Veh = (MovingVehicleParams *)Obj;
		}
	}
	else
	{
			int Pos = 0;
			int Max = -1;

			AGRadarLOSType *Walk = &AGRadarLOSList[0];
			int Index = NumAGRadarLOS;
			while(Index-- > 0)
			{
				if(Walk->Frame > Max)
				{
					Max = Walk->Frame;
					Pos = Walk -  &AGRadarLOSList[0];
				}
				Walk++;
			}

			if(Max != -1)
			{
				AGRadarLOSType *To = &AGRadarLOSList[Pos];
				To->Frame = 0;
		    To->MaxFrame = 3 + (rand() % 5);
				To->Type = Type;
				To->LOS = LOS;
				if(Type == AG_REGISTERED_OBJECT)
				{
					To->Obj = (BasicInstance *)Obj;
					To->Veh = NULL;
				}
				else
				{
					To->Obj = NULL;
					To->Veh = (MovingVehicleParams *)Obj;
				}
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int AGRadarLOSCheck(int Type, void *Obj)
{
	// cap number processed
  NumProcessedLOS++;
	if(NumProcessedLOS > 150)
	{
		return(FALSE);
	}

	int Pos;
	int InList = IsInAGRadarLOSList(Type,Obj,&Pos);

	// if its in the list, LOS has been checked, just return true and remove from list after frame count goes beyond max
	if(InList)
	{
		AGRadarLOSList[Pos].Frame++;
		int RetVal = AGRadarLOSList[Pos].LOS;
		if(AGRadarLOSList[Pos].Frame > AGRadarLOSList[Pos].MaxFrame)
		{
			AGRadarLOSType *To   = &AGRadarLOSList[Pos];
			AGRadarLOSType *From = &AGRadarLOSList[NumAGRadarLOS-1];
			memcpy(To,From,sizeof(AGRadarLOSType));
			NumAGRadarLOS--;
		}
		return(RetVal);
	}
	else // do LOS check
	{
		FPointDouble TargetLoc,Intersection;

		if(Type == AG_REGISTERED_OBJECT)
			 TargetLoc = ((BasicInstance *)Obj)->Position;
		else
			 TargetLoc = ((MovingVehicleParams *)Obj)->WorldPosition;

		TargetLoc.Y += 50.0*FTTOWU;

	  int LOSMaintained = LOS(&(PlayerPlane->WorldPosition),&TargetLoc, &Intersection,MED_RES_LOS,FALSE);

    AddToAGRadarLOSList(Type,Obj,LOSMaintained);

		return(LOSMaintained);
	}
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FilterAGObjects()
{
  RegisteredObject *walker;
  BasicInstance *binst;
//  float dist,delx,dely,delz,DeltaAzim;
  float Distance;

  walker = &RegisteredObjects[0];

  while(walker <= LastRegisteredObject)
  {
		walker->Flags &= ~RO_IN_RBM_CONE;

		if (walker->Flags & RO_OBJECT_FALLING)
			binst = ((FallingObject *)walker->Instance)->Instance;
		else
			binst = walker->Instance;

		if (binst)
		{
     		if (!(walker->Flags & RO_LAYS_FLAT)) // used to be LAYS_FLAT flag check
	 		{
         		Distance = (float)(walker->DistanceFromCamera);// (binst->Position - PlayerPlane->WorldPosition);
				if (!Distance)
					Distance = (binst->Position - PlayerPlane->WorldPosition);

				if(Distance < Av.AGRadar.Range*NMTOWU)
        		{
					FPointDouble PlayerToObj;

//					Beam.MakeVectorToFrom(Av.AGRadar.BeamEndPointWorld,PlayerPlane->WorldPosition);
					PlayerToObj.MakeVectorToFrom(binst->Position,PlayerPlane->WorldPosition);

		  			if((PlayerPlane->Orientation.I * PlayerToObj) > 0 )
		  			{
		  			  	walker->Flags |= RO_IN_RBM_CONE;
//		    			DeltaAzim = AIComputeHeadingToPoint(PlayerPlane, binst->Position, &dist, &delx, &dely, &delz, 1);

//		    			if( (DeltaAzim <= Av.AGRadar.Azim) && (DeltaAzim >= -Av.AGRadar.Azim) )
					}
				}
			}

		}

	    walker++;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void make_rb_offsets(float &xoffset,float &yoffset,float &xscale, float &yscale,float &midx, float &midu);

#define RBX(x) ((x-midx)*xscale+xoffset)
#define RBY(y) ((y-midy)*yscale+yoffset)

void SetAGObjects()
{
  	RegisteredObject *walker,*first;
  	BasicInstance *binst;
	DetectedGroundObjType  GroundObj;
	DetectedGroundObjType *new_obj;
	float td,tx,ty,tz;
	int Id;

  	walker = &RegisteredObjects[0];
	first  = walker;

  if (SimPause)
	AGGroundObjs.NumGroundObj = 0;

  while(walker <= LastRegisteredObject)
  {
		// remove any objects that are completely out of range
	Id = walker-first;

	if( !(walker->Flags & RO_IN_RBM_CONE) || !(walker->Instance) )
		RemoveGroundObjFromList(&AGGroundObjs,Id);
	else
	{
		if (walker->Flags & RO_OBJECT_FALLING)
			binst = ((FallingObject *)walker->Instance)->Instance;
		else
			binst = walker->Instance;

		if(!((((StructureInstance *)binst)->Type->Basics.Flags & BF_LAYS_FLAT) ))
		{
			new_obj = GetGroundObjPtrInList(&AGGroundObjs,Id);

			if (!new_obj || (new_obj->RadarFrame != AGRadarFrame))
			{

				float DeltaAzim = AIComputeHeadingToPoint(PlayerPlane, binst->Position, &td, &tx, &ty, &tz, 1);

				if ((DeltaAzim < Av.AGRadar.Azim) && (DeltaAzim > -Av.AGRadar.Azim))
				{
					int right_direction;

					if(Av.AGRadar.ScanDirection == LTOR)
						right_direction = (SimPause || (DeltaAzim >= Av.AGRadar.AntAzimuth));
					else
						right_direction = (SimPause || (DeltaAzim <= Av.AGRadar.AntAzimuth));

					if (right_direction)
					{

						if ((Dist2D(&PlayerPlane->WorldPosition,&binst->Position)*WUTONM) < Av.AGRadar.Range)
						{

							if (AGRadarLOSCheck(AG_REGISTERED_OBJECT,binst))
							{
								FPointDouble ScreenLoc;

								if(Av.AGRadar.CurMode == AG_MAP_MODE)
									AGZoomInWorldToPixel(binst->Position,&ScreenLoc);
								else
									AGScanWorldToPixel(binst->Position,&ScreenLoc);

								if (!new_obj)
									new_obj = &GroundObj;

								new_obj->Id = Id;
								new_obj->OfsX = ScreenLoc.X;
								new_obj->OfsY = ScreenLoc.Z;
								new_obj->ShouldDraw = TRUE;
								new_obj->Heading = 0;
								new_obj->RelAngle = DeltaAzim;
								new_obj->Color = AV_GREEN;
								new_obj->Shade = 1.0;
								new_obj->Obj   = binst;
								new_obj->RadarFrame = AGRadarFrame;

								if (new_obj == &GroundObj)
									AddGroundObjToList(&AGGroundObjs,&GroundObj);
							}
						}
					}
				}
			}
		}
	}

  	walker++;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetAGVehicles()
{

  	AvObjListEntryType *Vehicle = &Av.Vehicles.List[0];
  	MovingVehicleParams *First  = &MovingVehicles[0];
	DetectedVehicleType  VehicleObj;
	DetectedVehicleType *new_obj;
	long lstatus;
	int Id;

  	if (SimPause)
		AGVehicles.NumVehicles = 0;

	int Index = Av.Vehicles.NumVehicles;
	while(Index-- > 0)
	{

		Id = (MovingVehicleParams *)Vehicle->Obj - First;

		if((Av.AGRadar.CurMode != AG_SEA_MODE) || ((MovingVehicleParams *)Vehicle->Obj)->iShipType)
		{
			lstatus = ((MovingVehicleParams *)Vehicle->Obj)->Status;

			lstatus = (( ((MovingVehicleParams *)Vehicle->Obj)->fSpeedFPS > 2.0) && (!(lstatus & (VL_DESTROYED|VL_WAITING|VL_DONE_MOVING))) && (lstatus & VL_MOVING));

			if((Av.AGRadar.CurMode != AG_GMT_MODE) || lstatus)
			{
				new_obj = GetVehiclePtrInList(&AGVehicles,Id);

				if (!new_obj || (new_obj->RadarFrame != AGRadarFrame))
				{
					float DeltaAzim = Vehicle->RelAngle;

					if ((DeltaAzim < Av.AGRadar.Azim) && (DeltaAzim > -Av.AGRadar.Azim))
					{
						int right_direction;

						if(Av.AGRadar.ScanDirection == LTOR)
							right_direction = (SimPause || (DeltaAzim >= Av.AGRadar.AntAzimuth));
						else
							right_direction = (SimPause || (DeltaAzim <= Av.AGRadar.AntAzimuth));

						if (right_direction)
						{
							if (Vehicle->Dist*WUTONM < Av.AGRadar.Range)
							{
								if (AGRadarLOSCheck(AG_VEHICLE,Vehicle->Obj))
								{
									FPointDouble ScreenLoc;

									if(Av.AGRadar.CurMode == AG_MAP_MODE)
										AGZoomInWorldToPixel(((MovingVehicleParams *)Vehicle->Obj)->WorldPosition,&ScreenLoc);
									else
										AGScanWorldToPixel( ((MovingVehicleParams *)Vehicle->Obj)->WorldPosition,&ScreenLoc);

									if (!new_obj)
										new_obj = &VehicleObj;

									new_obj->Id = Id;
									new_obj->OfsX = ScreenLoc.X;
									new_obj->OfsY = ScreenLoc.Z;
									new_obj->ShouldDraw = TRUE;
									new_obj->Heading = 0;
									new_obj->RelAngle = DeltaAzim;
									new_obj->Color = AV_GREEN;
									new_obj->Shade = 1.0;
									new_obj->Veh = ((MovingVehicleParams *)Vehicle->Obj);
									new_obj->IsMoving = lstatus;
									new_obj->RadarFrame = AGRadarFrame;

									if (new_obj == &VehicleObj)
										AddVehicleToList(&AGVehicles,&VehicleObj);
								}
							}
						}
					}
				}
			}
		}

		Vehicle++;
  }

}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawObject(RegisteredObject *object, int X, int Z,float width_to_pixels,float height_to_pixels)
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  BasicInstance *binst;
  float Width,Height,WidthInPixels,HeightInPixels;
  float DiamondHeight, DiamondWidth;
  int Draw;

  // at greater than 40 miles out just draw a pixel, too far to see any detail
//  if(Av.AGRadar.Range >= 80.0)
//  {
//     GrDrawDot(GrBuffFor3D,X,Z,Triple.Red,Triple.Green,Triple.Blue,0);
//	   return;
//  }

	float PolyXs[6];
	float PolyYs[6];
	FPointData PolyColors[6];

  binst = object->Instance;


  Width  = fabs(object->ExtentRight  - object->ExtentLeft);
  Height = fabs(object->ExtentBottom - object->ExtentTop);

  DiamondWidth  = abs(ICosTimes(PlayerPlane->Heading, Width)  + ISinTimes(PlayerPlane->Heading, Height));
  DiamondHeight = abs(ICosTimes(PlayerPlane->Heading, Height) + ISinTimes(PlayerPlane->Heading, Width));

  WidthInPixels = DiamondWidth * width_to_pixels;
  HeightInPixels = DiamondHeight * height_to_pixels;

//  WidthInPixels  = (DiamondWidth*WUTONM)*PixelsPerMile(Distance);
//  HeightInPixels = (DiamondHeight*WUTONM)*PixelsPerMile(Distance);

  if(WidthInPixels > 5)
	  WidthInPixels = 5;

	if(HeightInPixels > 5)
	  HeightInPixels = 5;

	// jlm Test code
//	WidthInPixels = 5;

  if( (WidthInPixels <= 2.0) && (HeightInPixels <= 2.0) )
  {
     GrDrawDot(GrBuffFor3D,X,Z,Triple.Red,Triple.Green,Triple.Blue,0);
  }
  else
  {
	  Draw = TRUE;

		PolyXs[0] =  X - WidthInPixels/2.0;
		PolyYs[0] =  Z;
		PolyColors[0].X = 0;
		PolyColors[0].Y = 1.0;
		PolyColors[0].Z = 0;

		PolyXs[1] =  X;
		PolyYs[1] =  Z - HeightInPixels/2.0;
		PolyColors[1].X = 0;
		PolyColors[1].Y = 0.0;
		PolyColors[1].Z = 0;

    if( PolyYs[1] < 0)
	    Draw = FALSE;

		PolyXs[2] =  X + WidthInPixels/2.0;;
		PolyYs[2] =  Z;
		PolyColors[2].X = 0;
		PolyColors[2].Y = 1.0;
		PolyColors[2].Z = 0;

		PolyXs[3] =  X;// + WidthInPixels/2.0;
		PolyYs[3] =  Z + HeightInPixels/2.0;
		PolyColors[3].X = 0;
		PolyColors[3].Y = 0.5;
		PolyColors[3].Z = 0;

	  if( PolyYs[3] > 110)
	    Draw = FALSE;

  	float Dy,Dx;

		int i=0;
		while( (i < 4) && (Draw) )
		{
	  		Dy   = AG_RADAR_BOTTOM - PolyYs[i];
	  		Dx   = PolyXs[i] - AG_RADAR_CENTER_X;

			Draw = (Dy > (fabs(Dx)*0.5774));
	  		i++;
		}

		if(Draw)
    {
	   		if(PolyXs[0] > 0)
   	     if(PolyYs[1] > 0)
		  		if(PolyXs[2] < 116)
		    	 if(PolyYs[3] < 110)
					 {
	            GrDrawRampedPolyBuff(GrBuffFor3D,4,&PolyXs[0],&PolyYs[0],&PolyColors[0],1.0);
					 }
		}
	}
}

void DrawVehicle(MovingVehicleParams *v,int X, int Z, float width_to_pixels,float height_to_pixels,FMatrix &pmat)
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

//  	if(Av.AGRadar.Range >= 80.0)
//     	GrDrawDot(GrBuffFor3D,X,Z,Triple.Red,Triple.Green,Triple.Blue,0);
//	else
//	{
		FPoint	ps[4];
		FPoint  *pwalk;

		FPoint  pmin,pmax;

		FMatrix mat = v->Attitude;

  		float WidthInPixels,HeightInPixels;
  		float Dy,Dx;
  		int Draw,i;

		mat *= pmat;

		ps[0] = v->Type->LowUL;
		ps[2] = v->Type->HighLR;

		ps[0].Y = 0.0f;
		ps[1].Y = 0.0f;
		ps[2].Y = 0.0f;
		ps[3].Y = 0.0f;

		ps[1].SetValues(ps[2].X,0.0f,ps[0].Z);
		ps[3].SetValues(ps[0].X,0.0f,ps[2].Z);

		pmin.SetValues(10000.0f,0.0f,10000.0f);
		pmax.SetValues(-10000.0f,0.0f,-10000.0f);

		pwalk = &ps[4];
		while(pwalk-- > ps)
		{
			*pwalk *= mat;
			if (ps->X < pmin.X)
				pmin.X = ps->X;
			if (ps->X > pmax.X)
				pmax.X = ps->X;
			if (ps->Z < pmin.Z)
				pmin.Z = ps->Z;
			if (ps->Z > pmax.Z)
				pmax.Z = ps->Z;
		}

  		// at greater than 40 miles out just draw a pixel, too far to see any detail

		float PolyXs[6];
		float PolyYs[6];
		FPointData PolyColors[6];

  		WidthInPixels = (pmax.X - pmin.X) * width_to_pixels;
  		HeightInPixels = (pmax.Z - pmin.Z) * height_to_pixels;

		WidthInPixels = __min(WidthInPixels,5);
		HeightInPixels = __min(HeightInPixels,5);

  		if( (WidthInPixels <= 2.0) && (HeightInPixels <= 2.0) )
     		GrDrawDot(GrBuffFor3D,X,Z,Triple.Red,Triple.Green,Triple.Blue,0);
  		else
  		{
			PolyXs[0] =  X - WidthInPixels/2.0;
			PolyYs[0] =  Z;
			PolyColors[0].X = 0;
			PolyColors[0].Y = 1.0;
			PolyColors[0].Z = 0;

			PolyXs[1] =  X;
			PolyYs[1] =  Z - HeightInPixels/2.0;
			PolyColors[1].X = 0;
			PolyColors[1].Y = 0.0;
			PolyColors[1].Z = 0;

    		if( PolyYs[1] >= 0)
			{
				PolyXs[2] =  X + WidthInPixels/2.0;;
				PolyYs[2] =  Z;
				PolyColors[2].X = 0;
				PolyColors[2].Y = 1.0;
				PolyColors[2].Z = 0;

				PolyXs[3] =  X;// + WidthInPixels/2.0;
				PolyYs[3] =  Z + HeightInPixels/2.0;
				PolyColors[3].X = 0;
				PolyColors[3].Y = 0.5;
				PolyColors[3].Z = 0;

	  			if( PolyYs[3] <= 110)
				{

	  				Draw = TRUE;

					i=0;
					while( (i < 4) && (Draw) )
					{
	  					Dy   = AG_RADAR_BOTTOM - PolyYs[i];
	  					Dx   = PolyXs[i] - AG_RADAR_CENTER_X;

						Draw = (Dy > (fabs(Dx)*0.5774));
	  					i++;
					}

	   				if(Draw && (PolyXs[0] > 0) && (PolyYs[1] > 0) && (PolyXs[2] < 116) && (PolyYs[3] < 110))
	        			GrDrawRampedPolyBuff(GrBuffFor3D,4,&PolyXs[0],&PolyYs[0],&PolyColors[0],1.0);
				}
			}
		}
//	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAGObjects()
{
	float PixelPerWorldWidth;
	float PixelPerWorldHeight;

	if(Av.AGRadar.CurMode == AG_MAP_MODE)
	{
		FPointDouble UL,LL,UR,LR;

		UL = ScreenZoomWorld[0];
		LL = ScreenZoomWorld[1];
		LR = ScreenZoomWorld[2];
		UR = ScreenZoomWorld[3];

		FPointDouble FromULToUR;

		FromULToUR.MakeVectorToFrom(UR,UL);

		float WorldWidth = FromULToUR.Length();

		FPointDouble FromULToLL;

		FromULToLL.MakeVectorToFrom(LL,UL);

		float WorldHeight = FromULToLL.Length();

		PixelPerWorldWidth  = (AG_RADAR_RIGHT  - AG_RADAR_LEFT)/(WorldWidth);
		PixelPerWorldHeight = (AG_RADAR_BOTTOM - AG_RADAR_TOP)/(WorldHeight);
	}
	else
		PixelPerWorldHeight = PixelPerWorldWidth = AG_SCAN_PIXELS_PER_MILE(Av.AGRadar.Range)*WUTONM;

  	DetectedGroundObjType *Ptr = &AGGroundObjs.GroundObjs[0];

	if( (Av.AGRadar.CurMode != AG_RBM_MODE) && (Av.AGRadar.CurMode != AG_MAP_MODE) ) return;

	int Index = AGGroundObjs.NumGroundObj;
	while(Index-- > 0)
	{
		//sprintf(TmpStr,"%d",Ptr->Id);
	  //DrawTextAtLoc(Ptr->OfsX-1,Ptr->OfsY-2,TmpStr,AV_GREEN,1.0);

		int Inside = IsPointInside(Ptr->OfsX,Ptr->OfsY,AG_RADAR_TOP,AG_RADAR_BOTTOM,AG_RADAR_LEFT,AG_RADAR_RIGHT);
		if(Inside)
		  DrawObject(&RegisteredObjects[Ptr->Id],Ptr->OfsX,Ptr->OfsY,PixelPerWorldWidth,PixelPerWorldHeight);

		Ptr++;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAGVehicles()
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	FMatrix mat1;
	mat1.SetRPH(0,0,PlayerPlane->Heading);
	mat1.Transpose();

	float PixelPerWorldWidth;
	float PixelPerWorldHeight;

	if(Av.AGRadar.CurMode == AG_MAP_MODE)
	{
		FPointDouble UL,LL,UR,LR;

		UL = ScreenZoomWorld[0];
		LL = ScreenZoomWorld[1];
		LR = ScreenZoomWorld[2];
		UR = ScreenZoomWorld[3];

		FPointDouble FromULToUR;

		FromULToUR.MakeVectorToFrom(UR,UL);

		float WorldWidth = FromULToUR.Length();

		FPointDouble FromULToLL;

		FromULToLL.MakeVectorToFrom(LL,UL);

		float WorldHeight = FromULToLL.Length();

		PixelPerWorldWidth  = (AG_RADAR_RIGHT  - AG_RADAR_LEFT)/(WorldWidth);
		PixelPerWorldHeight = (AG_RADAR_BOTTOM - AG_RADAR_TOP)/(WorldHeight);
	}
	else
		PixelPerWorldHeight = PixelPerWorldWidth = AG_SCAN_PIXELS_PER_MILE(Av.AGRadar.Range)*WUTONM;


  	DetectedVehicleType *Ptr = &AGVehicles.Vehicles[0];

	int Index = AGVehicles.NumVehicles;
	while(Index-- > 0)
	{
		int Draw = (Av.AGRadar.CurMode != AG_GMT_MODE) ? (Ptr->ShouldDraw) : (Ptr->IsMoving);

		if( (Av.AGRadar.CurMode == AG_SEA_MODE) && !Ptr->Veh->iShipType)
				Draw = FALSE;

		if(Draw)
		{
		  int Inside = IsPointInside(Ptr->OfsX,Ptr->OfsY,AG_RADAR_TOP,AG_RADAR_BOTTOM,AG_RADAR_LEFT,AG_RADAR_RIGHT);
		  if(Inside)
			{
				if( (Av.AGRadar.CurMode == AG_RBM_MODE) || (Av.AGRadar.CurMode == AG_MAP_MODE) )
				  DrawVehicle(Ptr->Veh,Ptr->OfsX,Ptr->OfsY,PixelPerWorldWidth,PixelPerWorldHeight,mat1);
				else
				  DrawRadarBlip(Ptr->OfsX,Ptr->OfsY,AV_GREEN,1.0);
			}
		}

		Ptr++;
	}

}

//*****************************************************************************************************************************************
// WEAP STORES GLOBAL  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

WSStationPointType StationPoints[] = { {14,65},{26,60},{41,47},{58,35},{37,22},{71,21},{91,22},{84,35},{101,47},{116,60},{128,65} };
//WSStationPointType StationPoints[] = { {9,71},{25,58},{41,47},{58,35},{37,22},{71,21},{91,22},{84,35},{101,47},{117,58},{133,71} };

// based on initial loadout, doesn't change after init.
int StationPointSymbols[ MAX_F18E_STATIONS];

BombProgType BombProgs[MAX_BOMB_PROGS];
BombProgType SavedBombProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

BombProgType ClusterBombProgs[MAX_BOMB_PROGS];
BombProgType SavedClusterBombProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

HarpoonProgType HarpoonProgs[MAX_BOMB_PROGS];
HarpoonProgType SavedHarpoonProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

SlamerProgType SlamerProgs[MAX_BOMB_PROGS];
SlamerProgType SavedSlamerProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

JsowProgType JsowProgs[MAX_BOMB_PROGS];
JsowProgType SavedJsowProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

JsowProgType JdamProgs[MAX_BOMB_PROGS];
JsowProgType SavedJdamProgs[MAX_BOMB_PROGS]; // this structure is what is written to file.

HarmProgType HarmProgs[MAX_BOMB_PROGS];
HarmProgType SavedHarmProgs[MAX_BOMB_PROGS];

TewsProgType TewsProgs[MAX_TEWS_PROGS];
TewsProgType SavedTewsProgs[MAX_TEWS_PROGS];

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DefaultAvionicsData()
{
	BombProgs[0].Mode = WEAP_STORES_CCIP_MODE;
	BombProgs[0].Qty  = 1;
	BombProgs[0].Mult = 1;
	BombProgs[0].Inv  = 200;

	BombProgs[1].Mode = WEAP_STORES_CCIP_MODE;
	BombProgs[1].Qty  = 1;
	BombProgs[1].Mult = 1;
	BombProgs[1].Inv  = 200;

	BombProgs[2].Mode = WEAP_STORES_CCIP_MODE;
	BombProgs[2].Qty  = 1;
	BombProgs[2].Mult = 1;
	BombProgs[2].Inv  = 200;

	BombProgs[3].Mode = WEAP_STORES_CCIP_MODE;
	BombProgs[3].Qty  = 1;
	BombProgs[3].Mult = 1;
	BombProgs[3].Inv  = 200;

	BombProgs[4].Mode = WEAP_STORES_CCIP_MODE;
	BombProgs[4].Qty  = 1;
	BombProgs[4].Mult = 1;
	BombProgs[4].Inv  = 200;

	 // jlm temp, here is where we should be loading it from disk
	ClusterBombProgs[0].Mode = WEAP_STORES_CCIP_MODE;
	ClusterBombProgs[0].Qty  = 1;
	ClusterBombProgs[0].Mult = 1;
	ClusterBombProgs[0].Inv  = 200;
	ClusterBombProgs[0].Hob  = 500;

	ClusterBombProgs[1].Mode = WEAP_STORES_CCIP_MODE;
	ClusterBombProgs[1].Qty  = 1;
	ClusterBombProgs[1].Mult = 1;
	ClusterBombProgs[1].Inv  = 200;
	ClusterBombProgs[1].Hob  = 500;

	ClusterBombProgs[2].Mode = WEAP_STORES_CCIP_MODE;
	ClusterBombProgs[2].Qty  = 1;
	ClusterBombProgs[2].Mult = 1;
	ClusterBombProgs[2].Inv  = 200;
	ClusterBombProgs[2].Hob  = 500;

	ClusterBombProgs[3].Mode = WEAP_STORES_CCIP_MODE;
	ClusterBombProgs[3].Qty  = 1;
	ClusterBombProgs[3].Mult = 1;
	ClusterBombProgs[3].Inv  = 200;
	ClusterBombProgs[3].Hob  = 500;

	ClusterBombProgs[4].Mode = WEAP_STORES_CCIP_MODE;
	ClusterBombProgs[4].Qty  = 1;
	ClusterBombProgs[4].Mult = 1;
	ClusterBombProgs[4].Inv  = 200;
	ClusterBombProgs[4].Hob  = 500;

	for(int i=0; i<5; i++)
	{
			HarpoonProgs[i].Mode = HARPOON_RBL_MODE;
			HarpoonProgs[i].Flt  = WEAP_STORES_HIGH;
			HarpoonProgs[i].Term = WEAP_STORES_SKIM;
			HarpoonProgs[i].Seek = WEAP_STORES_SMALL;
			HarpoonProgs[i].Search = 10;
	}

	for(i=0; i<5; i++)
	{
	   SlamerProgs[i].Mode = WEAP_STORES_PB_MODE;
	   SlamerProgs[i].Flt  = WEAP_STORES_HIGH;
	   SlamerProgs[i].Term = WEAP_STORES_SKIM;
	   SlamerProgs[i].Tgt  = -1;
	}

	for(i=0; i<5; i++)
	{
	   JsowProgs[i].Mode = WEAP_STORES_PB_MODE;
	   JsowProgs[i].Tgt  = -1;
	}

	for(i=0; i<5; i++)
	{
	   JdamProgs[i].Mode = WEAP_STORES_PB_MODE;
	   JdamProgs[i].Tgt  = -1;
	}

	for(i=0; i<5; i++)
	{
	   HarmProgs[i].Tgt = -1;
	}

	for(i=0; i<MAX_TEWS_PROGS; i++)
	{
		TewsProgs[i].NumChaff   = 0;
		TewsProgs[i].NumFlares  = 0;
		TewsProgs[i].Repeat     = 0;
		TewsProgs[i].Interval   = 0;
	}

	// hardcode semi and auto mode progs
	TewsProgs[5].NumChaff   = 3;  TewsProgs[6].NumChaff   = 3;  TewsProgs[7].NumChaff   = 2;   TewsProgs[8].NumChaff  = 1;       TewsProgs[9].NumChaff   = 1;
	TewsProgs[5].NumFlares  = 0;  TewsProgs[6].NumFlares  = 0;  TewsProgs[7].NumFlares  = 0;   TewsProgs[8].NumFlares = 2;       TewsProgs[9].NumFlares  = 0;
	TewsProgs[5].Repeat     = 4;  TewsProgs[6].Repeat     = 3;  TewsProgs[7].Repeat     = 2;   TewsProgs[8].Repeat    = 4;       TewsProgs[9].Repeat     = 4;
	TewsProgs[5].Interval   = 4;  TewsProgs[6].Interval   = 6;  TewsProgs[7].Interval   = 6;   TewsProgs[8].Interval  = 4;       TewsProgs[9].Interval   = 8;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

#define REGKEY_AV_DATA	"AvionicsSaveData"

BYTE *LoadBuffer;

void AvionicsSaveData(HKEY hKey)
{
	BYTE *Ptr;

	int TotalBytes = sizeof(BombProgType)*2*MAX_BOMB_PROGS + sizeof(HarpoonProgType)*MAX_BOMB_PROGS + sizeof(SlamerProgType)*MAX_BOMB_PROGS
		               + sizeof(JsowProgType)*2*MAX_BOMB_PROGS + sizeof(HarmProgType)*MAX_BOMB_PROGS + sizeof(TewsProgType)*MAX_TEWS_PROGS;

	LoadBuffer = (BYTE *)malloc(TotalBytes);

	Ptr = LoadBuffer;

	memcpy(Ptr,SavedBombProgs,sizeof(BombProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(BombProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedClusterBombProgs,sizeof(BombProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(BombProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedHarpoonProgs,sizeof(HarpoonProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(HarpoonProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedSlamerProgs,sizeof(SlamerProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(SlamerProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedJsowProgs,sizeof(JsowProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(JsowProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedJdamProgs,sizeof(JsowProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(JsowProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedHarmProgs,sizeof(HarmProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(HarmProgType)*MAX_BOMB_PROGS;

	memcpy(Ptr,SavedTewsProgs,sizeof(TewsProgType)*MAX_TEWS_PROGS);

	int lError = RegSetValueEx (hKey,REGKEY_AV_DATA, 0, REG_BINARY, (BYTE *)LoadBuffer,TotalBytes);

	free(LoadBuffer);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AvionicsLoadData(HKEY hKey)
{
	BYTE *Ptr;

	unsigned long TotalBytes = sizeof(BombProgType)*2*MAX_BOMB_PROGS + sizeof(HarpoonProgType)*MAX_BOMB_PROGS + sizeof(SlamerProgType)*MAX_BOMB_PROGS
		               + sizeof(JsowProgType)*2*MAX_BOMB_PROGS + sizeof(HarmProgType)*MAX_BOMB_PROGS + sizeof(TewsProgType)*MAX_TEWS_PROGS;

	LoadBuffer = (BYTE *)malloc(TotalBytes);

	int lError = RegQueryValueEx (hKey, REGKEY_AV_DATA, 0, NULL, (BYTE *)LoadBuffer, &TotalBytes);

	if (lError!=ERROR_SUCCESS)
	{
    DefaultAvionicsData();
		return;
	}

	Ptr = LoadBuffer;

	memcpy(BombProgs,Ptr,sizeof(BombProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(BombProgType)*MAX_BOMB_PROGS;

	memcpy(ClusterBombProgs,Ptr,sizeof(BombProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(BombProgType)*MAX_BOMB_PROGS;

	memcpy(HarpoonProgs,Ptr,sizeof(HarpoonProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(HarpoonProgType)*MAX_BOMB_PROGS;

	memcpy(SlamerProgs,Ptr,sizeof(SlamerProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(SlamerProgType)*MAX_BOMB_PROGS;

	memcpy(JsowProgs,Ptr,sizeof(JsowProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(JsowProgType)*MAX_BOMB_PROGS;

	memcpy(JdamProgs,Ptr,sizeof(JsowProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(JsowProgType)*MAX_BOMB_PROGS;

	memcpy(HarmProgs,Ptr,sizeof(HarmProgType)*MAX_BOMB_PROGS);
	Ptr += sizeof(HarmProgType)*MAX_BOMB_PROGS;

	memcpy(TewsProgs,Ptr,sizeof(TewsProgType)*MAX_TEWS_PROGS);

	free(LoadBuffer);
}

//*****************************************************************************************************************************************
// WEAP STORES INIT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SetStationPointSymbols()
{
  for (int i=0; i<MAX_F18E_STATIONS; i++)
	{
		if(PlayerPlane->WeapLoad[i].Count == 0)
			StationPointSymbols[i] = -1;
		else
		{
		  int Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;
		  if( (Type == SHORT_RANGE) || (Type == MED_RANGE) )
			{
				StationPointSymbols[i] = WS_MISSILE_SYMBOL;
			}
			else if(PlayerPlane->WeapLoad[i].Count > 0)
			{
				StationPointSymbols[i] = WS_DIAMOND_SYMBOL;
			}
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitWeapStores()
{
	int i;

	for(i=0; i<5; i++)
	{
 		JdamProgs[i].Tgt = -1;
	}

	for(i=0; i<5; i++)
	{
 		JsowProgs[i].Tgt = -1;
	}

	for(i=0; i<5; i++)
	{
 		SlamerProgs[i].Tgt = -1;
	}

	SetStationPointSymbols();

	WeapStores.Aim7LoftOn = FALSE;
	WeapStores.Aim120VisualOn = FALSE;
	WeapStores.AGunOn  = FALSE;
	WeapStores.FunnelOn  = FALSE;
	WeapStores.DSTBOn  = FALSE;
	WeapStores.GunsDataOn = FALSE;
	WeapStores.HarmOverride = FALSE;
	WeapStores.FloodOn = FALSE;
	WeapStores.GunsOn = FALSE;
	WeapStores.AGunCanFire = FALSE;

	WeapStores.GunRate = HIGH;
	PlayerPlane->GunFireRate = 1;

	WeapStores.DumbBomb.ReadyForProg = -1;
  WeapStores.DumbBomb.CurProg = 0;
	WeapStores.ClusterBomb.ReadyForProg = -1;
  WeapStores.ClusterBomb.CurProg = 0;
	WeapStores.Harpoon.ReadyForProg = -1;
  WeapStores.Harpoon.CurProg = 0;
	WeapStores.Slamer.ReadyForProg = -1;
  WeapStores.Slamer.CurProg = 0;
	WeapStores.Jsow.ReadyForProg = -1;
  WeapStores.Jsow.CurProg = 0;
	WeapStores.Jdam.ReadyForProg = -1;
  WeapStores.Jdam.CurProg = 0;
	WeapStores.Harm.ReadyForProg = -1;
  WeapStores.Harm.CurProg = 0;

  BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

	WeapStores.RocketPod.Mode = WEAP_STORES_CCIP_MODE;
	WeapStores.RocketPod.FireMethod = ROCKET_POD_SINGLE;
	WeapStores.RocketPod.NumStations = 1;

	// jlm need data for detonate height
  SetBombingAttributes(B->Qty,B->Mult,B->Inv,100);

	TewsProgs[5].NumChaff   = 3;  TewsProgs[6].NumChaff   = 3;  TewsProgs[7].NumChaff   = 2;   TewsProgs[8].NumChaff  = 1;       TewsProgs[9].NumChaff   = 1;
	TewsProgs[5].NumFlares  = 0;  TewsProgs[6].NumFlares  = 0;  TewsProgs[7].NumFlares  = 0;   TewsProgs[8].NumFlares = 2;       TewsProgs[9].NumFlares  = 0;
	TewsProgs[5].Repeat     = 4;  TewsProgs[6].Repeat     = 3;  TewsProgs[7].Repeat     = 2;   TewsProgs[8].Repeat    = 4;       TewsProgs[9].Repeat     = 4;
	TewsProgs[5].Interval   = 4;  TewsProgs[6].Interval   = 6;  TewsProgs[7].Interval   = 6;   TewsProgs[8].Interval  = 4;       TewsProgs[9].Interval   = 8;

	memcpy(SavedBombProgs,BombProgs,sizeof(BombProgType)*MAX_BOMB_PROGS);
	memcpy(SavedClusterBombProgs,ClusterBombProgs,sizeof(BombProgType)*MAX_BOMB_PROGS);
	memcpy(SavedHarpoonProgs,HarpoonProgs,sizeof(HarpoonProgType)*MAX_BOMB_PROGS);
	memcpy(SavedSlamerProgs,SlamerProgs,sizeof(SlamerProgType)*MAX_BOMB_PROGS);
	memcpy(SavedJsowProgs,JsowProgs,sizeof(JsowProgType)*MAX_BOMB_PROGS);
	memcpy(SavedJdamProgs,JdamProgs,sizeof(JsowProgType)*MAX_BOMB_PROGS);
	memcpy(SavedHarmProgs,HarmProgs,sizeof(HarmProgType)*MAX_BOMB_PROGS);
	memcpy(SavedTewsProgs,TewsProgs,sizeof(TewsProgType)*MAX_TEWS_PROGS);

	ResetMaveric();
	ResetWalleye();
	InitFlir();
	InitHarm();
	WeapStores.LastCamStation = -1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetAcquiredTarget(void **Target, int *TargetType, FPointDouble *Loc)
{
	Loc->X = -1;

	if(PlayerPlane->AGDesignate.X == -1)
	{
		*Target = NULL;
		Loc->X = -1;
	  *TargetType = GROUNDOBJECT;
		return;
	}

	if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_RADAR)
	{
		if(Av.AGRadar.TargetType == AG_REGISTERED_OBJECT)
		{
	  	*Target  = ((BasicInstance *)Av.AGRadar.Target);
			*TargetType = GROUNDOBJECT;
		}
		else
		{
	  	*Target = ((MovingVehicleParams *)Av.AGRadar.Target);
			*TargetType = MOVINGVEHICLE;
		}
	}
	else if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_FLIR)
	{
		if(!Av.Flir.TargetObtained)
		{
			*Target = NULL;
			Loc->X = -1;
      *TargetType = GROUNDOBJECT;
			return;
		}

  	if(Av.Flir.TargetType == FLIR_CAM_GROUND_TARGET)
		{
  		*Target = ((BasicInstance *)Av.Flir.Target);
			*TargetType = GROUNDOBJECT;
		}
  	else if(Av.Flir.TargetType == FLIR_CAM_VEH_TARGET)
		{
    	*Target = ((MovingVehicleParams *)Av.Flir.Target);
			*TargetType = MOVINGVEHICLE;
		}
  	else if(Av.Flir.TargetType == FLIR_CAM_PLANE_TARGET)
		{
    	*Target = ((PlaneParams *)Av.Flir.Target);
			*TargetType = AIRCRAFT;
		}
  	else
		{
			*Loc         = Av.Flir.TargetLoc;
    	*Target     = NULL;
			*TargetType = GROUNDOBJECT;
		}
	}
	else // HUD
	{
		*Loc        = PlayerPlane->AGDesignate;
    *Target     = NULL;
		*TargetType = GROUNDOBJECT;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float GetJdamImpactTime( PlaneParams *P, double orgVertVel, FPoint *ImpactPoint, FPointDouble TargLoc )
{
	double VertVel;
	float  gravity = 32.0f;
	double dheight = P->HeightAboveGround;
 	float  weapondrag = 1.0;

	if (TargLoc.X == -1) return(10000);

  dheight = P->WorldPosition.Y - TargLoc.Y;

	if(Av.Weapons.CurAGWeap)
	  weapondrag = pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex].fDrag;

	if(weapondrag > 1.0)
	{
		gravity /= weapondrag;
		weapondrag = 1.0;
	}

	VertVel = orgVertVel * weapondrag;

	float tImpact = (float)(1.0f/gravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*gravity)*(dheight * WUTOFT))));

	if (ImpactPoint)
	{
		(*ImpactPoint).SetValues( (float)(P->WorldPosition.X + (P->IfVelocity.X * weapondrag * tImpact)*FTTOWU),
								  (float)(P->WorldPosition.Y-dheight),
								  (float)(P->WorldPosition.Z + (P->IfVelocity.Z * weapondrag * tImpact)*FTTOWU));
	}

	return tImpact;
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int TargetInZone(int Page, int *Condition,int *TimeToMaxInSec)
{
	 FPointDouble Loc;
	 void *Target;
	 int  GroundTargetType;
	 FPointDouble TargetLoc;

	 if(Page == HARPOON_WEAP_PAGE)
	 {
     HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

		 if(B->Mode == HARPOON_RBL_MODE)
		 {
			 GetAcquiredTarget(&Target,&GroundTargetType,&Loc);

			 if( (Target == NULL) && (Loc.X == -1) )
			 {
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
			 }

			 if(GroundTargetType == GROUNDOBJECT)
			 {
				 if(Loc.X == -1)
					 TargetLoc = ( (BasicInstance *)Target)->Position;
				 else
					 TargetLoc = Loc;
			 }
			 else if(GroundTargetType == AIRCRAFT)
			 {
				 TargetLoc = ((PlaneParams *)Target)->WorldPosition;
			 }
			 else if(GroundTargetType == MOVINGVEHICLE)
			 {
				 TargetLoc = ((MovingVehicleParams *)Target)->WorldPosition;
			 }
		 }
		 else if(B->Mode == HARPOON_BOL_MODE)
		 {
			 return(TRUE);
		 }
	 }
	 else if(Page == SLAMER_WEAP_PAGE)
	 {
     SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

		 if(B->Mode == WEAP_STORES_TOO_MODE)
		 {
			 GetAcquiredTarget(&Target,&GroundTargetType,&Loc);

			 if( (Target == NULL) && (Loc.X == -1) )
			 {
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
			 }

			 if(GroundTargetType == GROUNDOBJECT)
			 {
				 if(Loc.X == -1)
					 TargetLoc = ( (BasicInstance *)Target)->Position;
				 else
					 TargetLoc = Loc;
			 }
			 else if(GroundTargetType == AIRCRAFT)
			 {
				 TargetLoc = ((PlaneParams *)Target)->WorldPosition;
			 }
			 else if(GroundTargetType == MOVINGVEHICLE)
			 {
				 TargetLoc = ((MovingVehicleParams *)Target)->WorldPosition;
			 }
		 }
		 else
		 {
				if(B->Tgt >= 0)
				{
						TargetLoc.X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						TargetLoc.Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						TargetLoc.Y = LandHeight(TargetLoc.X,TargetLoc.Z);
				}
				else
				{
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
				}
		 }
	 }
	 else if(Page == JSOW_WEAP_PAGE)
	 {
     JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

		 if(B->Mode == WEAP_STORES_TOO_MODE)
		 {
			 GetAcquiredTarget(&Target,&GroundTargetType,&Loc);

			 if( (Target == NULL) && (Loc.X == -1) )
			 {
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
			 }

			 if(GroundTargetType == GROUNDOBJECT)
			 {
				 if(Loc.X == -1)
					 TargetLoc = ( (BasicInstance *)Target)->Position;
				 else
					 TargetLoc = Loc;
			 }
			 else if(GroundTargetType == AIRCRAFT)
			 {
				 TargetLoc = ((PlaneParams *)Target)->WorldPosition;
			 }
			 else if(GroundTargetType == MOVINGVEHICLE)
			 {
				 TargetLoc = ((MovingVehicleParams *)Target)->WorldPosition;
			 }
		 }
		 else
		 {
				if(B->Tgt >= 0)
				{
						TargetLoc.X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						TargetLoc.Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						TargetLoc.Y = LandHeight(TargetLoc.X,TargetLoc.Z);
				}
				else
				{
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
				}
		 }
	 }
	 else if(Page == JDAM_WEAP_PAGE)
	 {
     JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

		 if(B->Mode == WEAP_STORES_TOO_MODE)
		 {
			 GetAcquiredTarget(&Target,&GroundTargetType,&Loc);

			 if( (Target == NULL) && (Loc.X == -1) )
			 {
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
			 }

			 if(GroundTargetType == GROUNDOBJECT)
			 {
				 if(Loc.X == -1)
					 TargetLoc = ( (BasicInstance *)Target)->Position;
				 else
					 TargetLoc = Loc;
			 }
			 else if(GroundTargetType == AIRCRAFT)
			 {
				 TargetLoc = ((PlaneParams *)Target)->WorldPosition;
			 }
			 else if(GroundTargetType == MOVINGVEHICLE)
			 {
				 TargetLoc = ((MovingVehicleParams *)Target)->WorldPosition;
			 }
		 }
		 else
		 {
				if(B->Tgt >= 0)
				{
						TargetLoc.X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						TargetLoc.Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						TargetLoc.Y = LandHeight(TargetLoc.X,TargetLoc.Z);
				}
				else
				{
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
				  return(FALSE);
				}
		 }
	 }
	 else if(Page == HARM_WEAP_PAGE)
	 {
			if(Av.Harm.Mode == HARM_TOO_MODE)
			{
					if(!Av.Harm.CurHarmObj)
					{
  					*Condition = WEAP_STORES_NO_TARGET_ERROR;
						return(FALSE);
					}

					if(Av.Harm.CurHarmObjType == TEWS_SAM_PROV_THREAT)
					{
	  				TargetLoc = ((InfoProviderInstance *)Av.Harm.CurHarmObj)->Basics.Position;
					}
					else if(Av.Harm.CurHarmObjType == TEWS_SAM_WEAP_THREAT)
					{
	  		  	TargetLoc = ((AAWeaponInstance *)Av.Harm.CurHarmObj)->Basics.Position;
					}
					else if(Av.Harm.CurHarmObjType == TEWS_VEH_THREAT)
					{
	  		  	TargetLoc = ((MovingVehicleParams *)Av.Harm.CurHarmObj)->WorldPosition;
					}
			}
			else if(Av.Harm.Mode == HARM_PB_MODE)
			{
				if(AvNumGpsLabels <= 0)
				{
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
				}

				HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

				if(H->Tgt >= 0)
				{
						TargetLoc.X  = AvGpsLabels[H->Tgt]->lX*FTTOWU;
						TargetLoc.Z  = AvGpsLabels[H->Tgt]->lY*FTTOWU;
						TargetLoc.Y  = LandHeight(TargetLoc.X,TargetLoc.Z);
				}
				else
				{
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
				}
			}
 	 }
	 else
	 {
		 if(Av.Weapons.CurAGWeap)
		 {
			 if(pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex].iWeaponType != WEAPON_TYPE_AG_MISSILE)
				 return(FALSE);

			 GetAcquiredTarget(&Target,&GroundTargetType,&Loc);

			 if( (Target == NULL) && (Loc.X == -1) )
			 {
					*Condition = WEAP_STORES_NO_TARGET_ERROR;
					return(FALSE);
			 }

			 if(GroundTargetType == GROUNDOBJECT)
			 {
				 if(Loc.X == -1)
					 TargetLoc = ( (BasicInstance *)Target)->Position;
				 else
					 TargetLoc = Loc;
			 }
			 else if(GroundTargetType == AIRCRAFT)
			 {
				 TargetLoc = ((PlaneParams *)Target)->WorldPosition;
			 }
			 else if(GroundTargetType == MOVINGVEHICLE)
			 {
				 TargetLoc = ((MovingVehicleParams *)Target)->WorldPosition;
			 }
		 }
		 else
		 {
			 return(FALSE);
		 }
	 }

	 // check Target loc
	 float Dist,DeltaAzim;
	 float MinRange,MaxRange;

	 DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
   WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

   /*
	 if(Page == JDAM_WEAP_PAGE)
	 {
			double upSpeed = PlayerPlane->IfVelocity.Y;

			FPoint	ImpactPoint;
			GetJdamImpactTime( PlayerPlane, upSpeed, &ImpactPoint,TargetLoc );

			float TRelCountDown = ((ImpactPoint - TargetLoc) * WUTOFT) / PlayerPlane->IfHorzVelocity;

			if( fabs(TRelCountDown) > 2)
			{
		    *Condition =  WEAP_STORES_MAX_RANGE_ERROR;
				return(FALSE);
			}
	 }
	 */

	 Dist = Dist2D(&PlayerPlane->WorldPosition,&TargetLoc);
	 Dist *= WUTOFT;

	 if(Dist > MaxRange)
	 {
		 float Diff = Dist - MaxRange;
		 *TimeToMaxInSec = (int)( (Diff)/PlayerPlane->IfHorzVelocity);
		 *Condition      =  WEAP_STORES_MAX_RANGE_ERROR;
		 return(FALSE);
	 }

	 if(Dist < MinRange)
	 {
		 *Condition = WEAP_STORES_MIN_RANGE_ERROR;
		 return(FALSE);
	 }

	 DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,TargetLoc,TRUE);

	 if( fabs(DeltaAzim) > 30 )
	 {
		 *Condition =  WEAP_STORES_OFF_AXIS_ERROR ;
		 return(FALSE);
	 }

	 if(PlayerPlane->Altitude < 200)
	 {
		 *Condition = WEAP_STORES_ALT_ERROR;
		 return(FALSE);
	 }

	 return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CheckWeaponsForInZone()
{
	 if(Av.Weapons.CurAGWeap == NULL) return;

	 if(Av.Weapons.CurAGWeap->WeapPage == HARPOON_WEAP_PAGE)
    	WeapStores.Harpoon.InZone = TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&WeapStores.Harpoon.Condition,&WeapStores.Harpoon.TimeToMaxInSec);
	 else if(Av.Weapons.CurAGWeap->WeapPage == SLAMER_WEAP_PAGE)
    	WeapStores.Slamer.InZone = TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&WeapStores.Slamer.Condition,&WeapStores.Slamer.TimeToMaxInSec);
	 else if(Av.Weapons.CurAGWeap->WeapPage == JSOW_WEAP_PAGE)
    	WeapStores.Jsow.InZone = TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&WeapStores.Jsow.Condition,&WeapStores.Jsow.TimeToMaxInSec);
	 else if(Av.Weapons.CurAGWeap->WeapPage == JDAM_WEAP_PAGE)
    	WeapStores.Jdam.InZone = TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&WeapStores.Jdam.Condition,&WeapStores.Jdam.TimeToMaxInSec);
	 else if(Av.Weapons.CurAGWeap->WeapPage == HARM_WEAP_PAGE)
    	WeapStores.Harm.InZone = TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&WeapStores.Harm.Condition,&WeapStores.Harm.TimeToMaxInSec);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetGPSLocation(int Page, FPointDouble *Loc)
{
	 if(Page == SLAMER_WEAP_PAGE)
	 {
     SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

		 if(B->Mode == WEAP_STORES_PB_MODE)
		 {
				if(B->Tgt >= 0)
				{
						Loc->X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						Loc->Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);
						return(TRUE);
				}
		 }
	 }
	 else if(Page == JSOW_WEAP_PAGE)
	 {
     JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

		 if(B->Mode == WEAP_STORES_PB_MODE)
		 {
				if(B->Tgt >= 0)
				{
						Loc->X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						Loc->Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);
						return(TRUE);
				}
		 }
	 }
	 else if(Page == JDAM_WEAP_PAGE)
	 {
     JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

		 if(B->Mode == WEAP_STORES_PB_MODE)
		 {
				if(B->Tgt >= 0)
				{
						Loc->X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						Loc->Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);
						return(TRUE);
				}
		 }
	 }
	 else if(Page == HARM_WEAP_PAGE)
	 {
			if(Av.Harm.Mode == HARM_PB_MODE)
			{
				HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

				if(H->Tgt >= 0)
				{
						Loc->X  = AvGpsLabels[H->Tgt]->lX*FTTOWU;
						Loc->Z  = AvGpsLabels[H->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);
						return(TRUE);
				}
			}
 	 }

	 return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetWeaponData(int Page,int *FlightProfile,void **Target, int *TargetType, FPointDouble *Loc)
{
	 if(Page == HARPOON_WEAP_PAGE)
	 {
     HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

		 if(B->Mode == HARPOON_RBL_MODE)
		 {
			 GetAcquiredTarget(Target,TargetType,Loc);
		 }
		 else if(B->Mode == HARPOON_BOL_MODE)
		 {
			 *Target = NULL;
			 Loc->X = -1;
			 *TargetType = GROUNDOBJECT;
		 }

		 if(B->Flt == WEAP_STORES_LOW)
		 {
			 if(B->Term == WEAP_STORES_POP)
				*FlightProfile = LOW_POP;
			 else
				*FlightProfile = LOW_LOW;
		 }
		 else
		 {
			 if(B->Flt == WEAP_STORES_MED)
				*FlightProfile = MED_ATTACK;
			 else
				*FlightProfile = HIGH_ATTACK;
		 }
	 }
	 else if(Page == SLAMER_WEAP_PAGE)
	 {
     SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

		 if(B->Mode == WEAP_STORES_TOO_MODE)
		 {
			 GetAcquiredTarget(Target,TargetType,Loc);
		 }
		 else
		 {
				if(B->Tgt >= 0)
				{
						Loc->X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						Loc->Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);
						*Target = NULL;
			      *TargetType = GROUNDOBJECT;
				}
				else
				{
			 		*Target = NULL;
			 		Loc->X = -1;
			 		*TargetType = GROUNDOBJECT;
				}
		 }

		 if(B->Flt == WEAP_STORES_LOW)
		 {
			 if(B->Term == WEAP_STORES_POP)
				*FlightProfile = LOW_POP;
			 else
				*FlightProfile = LOW_LOW;
		 }
		 else
		 {
			 if(B->Flt == WEAP_STORES_MED)
				*FlightProfile = MED_ATTACK;
			 else
				*FlightProfile = HIGH_ATTACK;
		 }
	 }
	 else if(Page == JSOW_WEAP_PAGE)
	 {
     JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

		 if(B->Mode == WEAP_STORES_TOO_MODE)
		 {
			 GetAcquiredTarget(Target,TargetType,Loc);
		 }
		 else
		 {
				if(B->Tgt >= 0)
				{
						Loc->X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						Loc->Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);
						*Target = NULL;
			      *TargetType = GROUNDOBJECT;
				}
				else
				{
			 		*Target = NULL;
			 		Loc->X = -1;
			 		*TargetType = GROUNDOBJECT;
				}
		 }
		 *FlightProfile = -1;
	 }
	 else if(Page == JDAM_WEAP_PAGE)
	 {
     JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

		 if(B->Mode == WEAP_STORES_TOO_MODE)
		 {
			 GetAcquiredTarget(Target,TargetType,Loc);
		 }
		 else
		 {
				if(B->Tgt >= 0)
				{
						Loc->X = AvGpsLabels[B->Tgt]->lX*FTTOWU;
						Loc->Z = AvGpsLabels[B->Tgt]->lY*FTTOWU;
						Loc->Y = LandHeight(Loc->X,Loc->Z);  // we do not neccessarily have land height, so Scott will check every frame.
						*Target = NULL;
			      *TargetType = GROUNDOBJECT;
				}
				else
				{
			 		*Target = NULL;
			 		Loc->X = -1;
			 		*TargetType = GROUNDOBJECT;
				}
		 }
		*FlightProfile = -1;
	 }
	 else if(Page == HARM_WEAP_PAGE)
	 {
			if( (Av.Harm.Mode == HARM_TOO_MODE) || (Av.Harm.Mode == HARM_SP_MODE) )
			{
					if(!Av.Harm.CurHarmObj)
					{
			 			*Target = NULL;
			 			Loc->X = -1;
			 			*TargetType = GROUNDOBJECT;
					}

					if(Av.Harm.CurHarmObjType == TEWS_SAM_PROV_THREAT)
					{
	  				*Target = (BasicInstance *) &((InfoProviderInstance *)Av.Harm.CurHarmObj)->Basics;
						*TargetType = GROUNDOBJECT;
						Loc->X = -1;
					}
					else if(Av.Harm.CurHarmObjType == TEWS_SAM_WEAP_THREAT)
					{
	  		  		*Target = (BasicInstance *) &((AAWeaponInstance *)Av.Harm.CurHarmObj)->Basics;
							*TargetType = GROUNDOBJECT;
							Loc->X = -1;
					}
					else if(Av.Harm.CurHarmObjType == TEWS_VEH_THREAT)
					{
	  		  		*Target = (MovingVehicleParams *)Av.Harm.CurHarmObj;
							*TargetType = MOVINGVEHICLE;
							Loc->X = -1;
					}
			}
			else if(Av.Harm.Mode == HARM_PB_MODE)
			{
				if(AvNumGpsLabels <= 0)
				{
			 		*Target = NULL;
			 		Loc->X = -1;
			 		*TargetType = GROUNDOBJECT;
				}

				HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

				if(H->Tgt >= 0)
				{
						Loc->X  = AvGpsLabels[H->Tgt]->lX*FTTOWU;
						Loc->Z  = AvGpsLabels[H->Tgt]->lY*FTTOWU;
						Loc->Y  = LandHeight(Loc->X,Loc->Z);
						*Target = NULL;
			      *TargetType = GROUNDOBJECT;
				}
				else
				{
			 		*Target = NULL;
			 		Loc->X = -1;
			 		*TargetType = GROUNDOBJECT;
				}
			}
 	 }
	 return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetHarpoonData(float *RangeInMiles, int *Seek)
{
   HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

	 *RangeInMiles = B->Search;
	 *Seek         = B->Seek;
}

//*****************************************************************************************************************************************
// BOMB ATTRIBUTES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void ResetBombProgram(AvWeapEntryType *Weap)
{
		BombProgType *Dumb;
		BombProgType *Cluster;

		if(Weap == NULL) return;

		switch(Weap->WeapPage)
		{
			case DUMB_WEAP_PAGE:        Dumb = &BombProgs[WeapStores.DumbBomb.CurProg];
                                  SetBombingAttributes(Dumb->Qty,Dumb->Mult,Dumb->Inv,-1);
																	SetAGWeapon(Weap->W->WeapId,Dumb->Mult);
	 																if( (Dumb->Mult == 1) && (Dumb->Qty > 1) )
     																SelectStationsForCount(GetSelectedAGWeaponId(),Dumb->Qty);
							                  	break;

			case CLUSTER_WEAP_PAGE:     Cluster = &ClusterBombProgs[WeapStores.ClusterBomb.CurProg];
                                  SetBombingAttributes(Cluster->Qty,Cluster->Mult,Cluster->Inv,Cluster->Hob);
																	SetAGWeapon(Weap->W->WeapId,Cluster->Mult);
	 																if( (Cluster->Mult == 1) && (Cluster->Qty > 1) )
     																SelectStationsForCount(GetSelectedAGWeaponId(),Cluster->Qty);
																	break;

			case ROCKET_POD_WEAP_PAGE:  SetAGWeapon(Weap->W->WeapId,WeapStores.RocketPod.NumStations);
																	break;


			case TALD_WEAP_PAGE:  SetBombingAttributes(1,1,0,-1);
				                          SetAGWeapon(Weap->W->WeapId,WeapStores.RocketPod.NumStations);
																	break;
			default :
					 SetAGWeapon(Weap->W->WeapId,1);
					 break;
		}
}

//*****************************************************************************************************************************************
// AG BUTTON CLICKS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AGLeftSelect()
{
	if(Av.Weapons.BombStations.Left)
	{
		 ResetBombProgram(Av.Weapons.BombStations.Left);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGFrontLeftSelect()
{
	if(Av.Weapons.BombStations.FrontLeft)
	{
		 ResetBombProgram(Av.Weapons.BombStations.FrontLeft);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGCenterSelect()
{
	if(Av.Weapons.BombStations.Center)
	{
		 ResetBombProgram(Av.Weapons.BombStations.Center);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGFrontRightSelect()
{
	if(Av.Weapons.BombStations.FrontRight)
	{
		 ResetBombProgram(Av.Weapons.BombStations.FrontRight);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGRightSelect()
{
	if(Av.Weapons.BombStations.Right)
	{
		 ResetBombProgram(Av.Weapons.BombStations.Right);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleHarmOverride()
{
  WeapStores.HarmOverride = !WeapStores.HarmOverride;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AAToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;

	if(!WeapStores.GunsOn)
		ResetToRWS();

	if(WeapStores.GunsOn && AreMpdsInGivenMode(RADAR_MODE) && (GetCurrentPrimaryTarget() == -1) )
	{
  	SetAcqMode(AA_GUN_ACQ_MODE);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawWSDiamondAtLoc(float X,float Y)
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	GrDrawLine(GrBuffFor3D,X-2,Y,X,Y-2,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X,Y-2,X+2,Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+2,Y,X,Y+2,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X,Y+2,X-2,Y,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawWSMissileAtLoc(float X,float Y)
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	GrDrawLine(GrBuffFor3D,X-1,Y-1,X+1,Y-1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+1,Y-1,X+1,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+1,Y+1,X-1,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X-1,Y+1,X-1,Y-1,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,X-3,Y-3,X+1,Y-1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+2,Y-3,X+1,Y-1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+3,Y+3,X+1,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X-3,Y+3,X-1,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawWeaponsOutline()
{
  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	GrDrawLine(GrBuffFor3D,WS_LEFT_OUTLINE_P1,WS_LEFT_OUTLINE_P2,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,WS_LEFT_OUTLINE_P2,WS_LEFT_OUTLINE_P3,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,WS_LEFT_OUTLINE_P3,WS_LEFT_OUTLINE_P4,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,WS_RIGHT_OUTLINE_P1,WS_RIGHT_OUTLINE_P2,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,WS_RIGHT_OUTLINE_P2,WS_RIGHT_OUTLINE_P3,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,WS_RIGHT_OUTLINE_P3,WS_RIGHT_OUTLINE_P4,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawWeapAtStation(int Station)
{
	WSStationPointType StationPos;
	char *Str;

	  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(PlayerPlane->WeapLoad[Station].Count <= 0) return;

	StationPos.X = StationPoints[Station].X;
	StationPos.Y = StationPoints[Station].Y;

	int BoxIt = FALSE;

	int Type = pDBWeaponList[PlayerPlane->WeapLoad[Station].WeapIndex].iWeaponType;

	if( (Type == SHORT_RANGE) || (Type == MED_RANGE) )
	{
			if(Av.Weapons.UseAASeq)
			{
				if(Av.Weapons.CurAASeqWeap)
				{
	  			  if( (Av.Weapons.CurAASeqWeap->Selected) && (Av.Weapons.CurAASeqWeap->Station == Station) && (UFC.MasterMode == AA_MODE) )
		  			BoxIt = TRUE;
				}
			}
			else
			{
				int Id = PlayerPlane->WeapLoad[Station].WeapId;

				if(Av.Weapons.CurAAWeap)
				{
		  		  if( (Av.Weapons.CurAAWeap->Selected) && (Av.Weapons.CurAAWeap->Station == Station) && (UFC.MasterMode == AA_MODE) )
			  		BoxIt = TRUE;
				}
			}
	}
	else
	{
		  if(Av.Weapons.CurAGWeap)
			{
	    		AvWeapEntryType *W = &Av.Weapons.AGList[0];
					int Index = Av.Weapons.NumAGWeapons;
					while(Index-- > 0)
					{
						if(W->W->WeapId == Av.Weapons.CurAGWeap->W->WeapId)
						{
							if( (W->Station == Station) && (W->Selected) && (UFC.MasterMode == AG_MODE) )
							{
								BoxIt = TRUE;
								break;
							}
						}
						W++;
					}
			}
	}

	if( (Station == CENTER6_STATION) || ( (Station >= LEFT2_STATION) && (Station <= LEFT4_STATION) ) || ( (Station >= RIGHT8_STATION) && (Station <= RIGHT10_STATION) ) )
	{
		float XOff = 0;
		float YOff = 6;

		if(StationPointSymbols[Station] == WS_MISSILE_SYMBOL)
			DrawWSMissileAtLoc(StationPos.X + XOff,StationPos.Y + YOff);
		else if(StationPointSymbols[Station] == WS_DIAMOND_SYMBOL)
			DrawWSDiamondAtLoc(StationPos.X + XOff,StationPos.Y + YOff);

		XOff = -1;
		YOff = 10;

		sprintf(TmpStr,"%d",PlayerPlane->WeapLoad[Station].Count);
	  	DrawTextAtLoc(StationPos.X + XOff,StationPos.Y + YOff,TmpStr,AV_GREEN,1.0);

		YOff = 17;

		Str = pDBWeaponList[PlayerPlane->WeapLoad[Station].WeapIndex].sMPDAbbrev;

		XOff = strlen(Str)*2;
	  	DrawTextAtLoc(StationPos.X - XOff,StationPos.Y + YOff,Str,AV_GREEN,1.0);

		if(BoxIt)
	     DrawBoxAroundWord(Str,StationPos.X - XOff,StationPos.Y + YOff,Triple.Red,Triple.Green,Triple.Blue);
	}
	else if( (Station == LEFT1_STATION) || (Station == RIGHT11_STATION) )
	{
		float XOff = (Station == LEFT1_STATION) ? -2 : -3;
	 	float YOff = -10;

		if(StationPointSymbols[Station] == WS_MISSILE_SYMBOL)
  		DrawWSMissileAtLoc(StationPos.X,StationPos.Y);

	 	Str = pDBWeaponList[PlayerPlane->WeapLoad[Station].WeapIndex].sMPDAbbrev;
		XOff = strlen(Str)*2;
	 	DrawTextAtLoc(StationPos.X - XOff,StationPos.Y + YOff,Str,AV_GREEN,1.0);

		if(BoxIt)
	    DrawBoxAroundWord(Str,StationPos.X - XOff,StationPos.Y + YOff,Triple.Red,Triple.Green,Triple.Blue);
  }
	else
	{
	  float XOff = (Station == LEFT5_STATION) ? 12 : 1;
		float YOff = 0;

		if(StationPointSymbols[Station] == WS_MISSILE_SYMBOL)
			DrawWSMissileAtLoc(StationPos.X + XOff,StationPos.Y + YOff);

		XOff = (Station == LEFT5_STATION) ? 3 : 11;
		YOff = -2;

	 	Str = pDBWeaponList[PlayerPlane->WeapLoad[Station].WeapIndex].sMPDAbbrev;
		XOff -= strlen(Str)*2;
	 	DrawTextAtLoc(StationPos.X + XOff,StationPos.Y + YOff,Str,AV_GREEN,1.0);

		if(BoxIt)
	     DrawBoxAroundWord(Str,StationPos.X + XOff,StationPos.Y + YOff,Triple.Red,Triple.Green,Triple.Blue);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawStations()
{
  for (int i=0; i<11; i++)
	{
		DrawWeapAtStation(i);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawWeaponSelections(int MpdNum)
{
 	AvWeapEntryType **Weap;
	char *Str;
	int Button = 19;
	int PrevId = -1;

	Weap = &Av.Weapons.BombStations.Left;

	int Index = 5;
	while(Index-- > 0)
	{
			if(*Weap)
			{
				int Id = GetWeapIndexFromId( (*Weap)->W->WeapId);
				int WeapOnBoard = IsWeaponOnBoard(Id);

	  		if(WeapOnBoard)
				{
		  		Str = pDBWeaponList[(*Weap)->W->WeapIndex].sMPDAbbrev;
					int BoxIt = FALSE;
					if(Av.Weapons.CurAGWeap)
					{
						if(Av.Weapons.CurAGWeap->W->WeapId == (*Weap)->W->WeapId)
							BoxIt = TRUE;
					}
					DrawWordAtMpdButton(MpdNum,Button,Str,BoxIt,AV_GREEN,1.0);
				}
			}
			Button--;
			Weap++;
	}
}

//*****************************************************************************************************************************************
// TALD WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void TaldStep()
{
  AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TaldWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,10,"STP",FALSE,AV_GREEN,1.0);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: TaldStep();
							 break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
			case 14: AAToggleGuns();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
// JDAM WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void JdamPageToggleMode()
{
   if(WeapStores.Jdam.ReadyForProg == WEAP_STORES_MODE_READY)
		 WeapStores.Jdam.ReadyForProg = -1;
	 else
		 WeapStores.Jdam.ReadyForProg = WEAP_STORES_MODE_READY;
}

//*****************************************************************************************************************************************
//* ****************************************************************************************************************************************

void JdamPageToggleTgt()
{
  if(WeapStores.Jdam.ReadyForProg == WEAP_STORES_TGT_READY)
		 WeapStores.Jdam.ReadyForProg = -1;
	else
		 WeapStores.Jdam.ReadyForProg = WEAP_STORES_TGT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamPageStep()
{
  AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamPageChangeProg()
{
  WeapStores.Jdam.CurProg++;
  if(WeapStores.Jdam.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.Jdam.CurProg = 0;

  JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamPageSaveProg()
{
	memcpy(SavedJdamProgs,JdamProgs,sizeof(JsowProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamPageUp()
{
	 if(WeapStores.Jdam.ReadyForProg == -1) return;

   JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

	 if(WeapStores. Jdam.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode++;
		 if(B->Mode > WEAP_STORES_TOO_MODE)
				 B->Mode = WEAP_STORES_TOO_MODE;
   }
	 else if(WeapStores.Jdam.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		   B->Tgt++;
		   if(B->Tgt >= AvNumGpsLabels)
				 B->Tgt = AvNumGpsLabels-1;
			 Av.Hsi.CurGps = B->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamPageDown()
{
	 if(WeapStores.Jdam.ReadyForProg == -1) return;

   JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

	 if(WeapStores. Jdam.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode--;
		 if(B->Mode < WEAP_STORES_PB_MODE)
				 B->Mode = WEAP_STORES_PB_MODE;
   }
	 else if(WeapStores.Jdam.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		 		B->Tgt--;
		 		if(B->Tgt < 0)
				 		B->Tgt = 0;
   		  Av.Hsi.CurGps = B->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamPageToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"TGT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"LABEL:",AV_GREEN,1.0);

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.Jdam.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

  GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

  JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

	switch(B->Mode)
	{
		case WEAP_STORES_PB_MODE: sprintf(TmpStr,"PB"); break;
    case WEAP_STORES_TOO_MODE:  sprintf(TmpStr,"TOO"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),TmpStr,AV_GREEN,1.0);

	// get target numbers ex 5.2
	if(B->Tgt >= 0)
	{
  		sprintf(TmpStr,"%d",B->Tgt+1);
			DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);
	}

	// GET LABEL
	if(B->Tgt >= 0)
	{
  	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),AvGpsLabels[B->Tgt]->sGPSLabel,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JdamWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
		DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
		DrawHarmMode();
	}

	DrawVerticalWordAtButton(MpdNum,0,"MODE",(WeapStores.Jdam.ReadyForProg == WEAP_STORES_MODE_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,4,"TGT",(WeapStores.Jdam.ReadyForProg == WEAP_STORES_TGT_READY),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if(WeapStores.Jdam.InZone)
	  DrawTextAtLoc(106,12,"IN ZONE",AV_GREEN,1.0);
	else if(WeapStores.Jdam.Condition == WEAP_STORES_MIN_RANGE_ERROR)
	  DrawTextAtLoc(106,12,"MIN RNG",AV_GREEN,1.0);
	else if(WeapStores.Jdam.Condition == WEAP_STORES_ALT_ERROR)
	  DrawTextAtLoc(106,12,"ALT",AV_GREEN,1.0);
	else if(WeapStores.Jdam.Condition == WEAP_STORES_OFF_AXIS_ERROR)
	  DrawTextAtLoc(98,12,"OFF AXIS",AV_GREEN,1.0);
	else if(WeapStores.Jdam.Condition == WEAP_STORES_NO_TARGET_ERROR)
	  DrawTextAtLoc(94,12,"NO TARGET",AV_GREEN,1.0);

	JdamTable(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: JdamPageToggleMode();
							 break;
			case  4: JdamPageToggleTgt();
							 break;
			case  5: JdamPageChangeProg();
							 break;
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: JdamPageSaveProg();
			         break;
			case 11: JdamPageStep();
							 break;
			case 12: JdamPageDown();
			         break;
			case 13: JdamPageUp();
			         break;
			case 14: JdamPageToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}


//*****************************************************************************************************************************************
// JSOW WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void JsowPageToggleMode()
{
  if(WeapStores.Jsow.ReadyForProg == WEAP_STORES_MODE_READY)
		 WeapStores.Jsow.ReadyForProg = -1;
	else
		 WeapStores.Jsow.ReadyForProg = WEAP_STORES_MODE_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageToggleTgt()
{
  if(WeapStores.Jsow.ReadyForProg == WEAP_STORES_TGT_READY)
		 WeapStores.Jsow.ReadyForProg = -1;
	else
		 WeapStores.Jsow.ReadyForProg = WEAP_STORES_TGT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageStep()
{
  AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageChangeProg()
{
  WeapStores.Jsow.CurProg++;
  if(WeapStores.Jsow.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.Jsow.CurProg = 0;

  JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageSaveProg()
{
	memcpy(SavedJsowProgs,JsowProgs,sizeof(JsowProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageUp()
{
	 if(WeapStores.Jsow.ReadyForProg == -1) return;

   JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

	 if(WeapStores. Jsow.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode++;
		 if(B->Mode > WEAP_STORES_TOO_MODE)
				 B->Mode = WEAP_STORES_TOO_MODE;
   }
	 else if(WeapStores.Jsow.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		   B->Tgt++;
		   if(B->Tgt >= AvNumGpsLabels)
				 B->Tgt = AvNumGpsLabels-1;
			 Av.Hsi.CurGps = B->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageDown()
{
	 if(WeapStores.Jsow.ReadyForProg == -1) return;

   JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

	 if(WeapStores. Jsow.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode--;
		 if(B->Mode < WEAP_STORES_PB_MODE)
				 B->Mode = WEAP_STORES_PB_MODE;
   }
	 else if(WeapStores.Jsow.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		 		B->Tgt--;
		 		if(B->Tgt < 0)
				 		B->Tgt = 0;
			  Av.Hsi.CurGps = B->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowPageToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"TGT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"LABEL:",AV_GREEN,1.0);

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.Jsow.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

  GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

  JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

	switch(B->Mode)
	{
		case WEAP_STORES_PB_MODE: sprintf(TmpStr,"PB"); break;
    case WEAP_STORES_TOO_MODE:  sprintf(TmpStr,"TOO"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),TmpStr,AV_GREEN,1.0);

	// get target numbers ex 5.2
	if(B->Tgt >= 0)
	{
  		sprintf(TmpStr,"%d",B->Tgt+1);
			DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);
	}

	// GET LABEL
	if(B->Tgt >= 0)
	{
  	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),AvGpsLabels[B->Tgt]->sGPSLabel,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void JsowWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,0,"MODE",(WeapStores.Jsow.ReadyForProg == WEAP_STORES_MODE_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,4,"TGT",(WeapStores.Jsow.ReadyForProg == WEAP_STORES_TGT_READY),AV_GREEN,1.0);
  DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if(WeapStores.Jsow.InZone)
	  DrawTextAtLoc(106,12,"IN ZONE",AV_GREEN,1.0);
	else if(WeapStores.Jsow.Condition == WEAP_STORES_MIN_RANGE_ERROR)
	  DrawTextAtLoc(106,12,"MIN RNG",AV_GREEN,1.0);
	else if(WeapStores.Jsow.Condition == WEAP_STORES_ALT_ERROR)
	  DrawTextAtLoc(106,12,"ALT",AV_GREEN,1.0);
	else if(WeapStores.Jsow.Condition == WEAP_STORES_OFF_AXIS_ERROR)
	  DrawTextAtLoc(98,12,"OFF AXIS",AV_GREEN,1.0);
	else if(WeapStores.Jsow.Condition == WEAP_STORES_NO_TARGET_ERROR)
	  DrawTextAtLoc(94,12,"NO TARGET",AV_GREEN,1.0);

  JsowTable(MpdNum);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: JsowPageToggleMode();
							 break;
			case  4: JsowPageToggleTgt();
							 break;
			case  5: JsowPageChangeProg();
							 break;
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: JsowPageSaveProg();
			         break;
			case 11: JsowPageStep();
							 break;
			case 12: JsowPageDown();
			         break;
			case 13: JsowPageUp();
			         break;
			case 14: JsowPageToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
// SLAMER WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SlamerPageToggleMode()
{
  if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_MODE_READY)
		 WeapStores.Slamer.ReadyForProg = -1;
	else
		 WeapStores.Slamer.ReadyForProg = WEAP_STORES_MODE_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageToggleFlt()
{
  if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_FLT_READY)
		 WeapStores.Slamer.ReadyForProg = -1;
	else
		 WeapStores.Slamer.ReadyForProg = WEAP_STORES_FLT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageToggleTerm()
{
  if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TERM_READY)
		 WeapStores.Slamer.ReadyForProg = -1;
	else
		 WeapStores.Slamer.ReadyForProg = WEAP_STORES_TERM_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageToggleTgt()
{
  if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TGT_READY)
		 WeapStores.Slamer.ReadyForProg = -1;
	else
		 WeapStores.Slamer.ReadyForProg = WEAP_STORES_TGT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageStep()
{
  AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageChangeProg()
{
  WeapStores.Slamer.CurProg++;
  if(WeapStores.Slamer.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.Slamer.CurProg = 0;

  SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageSaveProg()
{
	memcpy(SavedSlamerProgs,SlamerProgs,sizeof(SlamerProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageUp()
{
	 if(WeapStores.Slamer.ReadyForProg == -1) return;

   SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

	 if(WeapStores. Slamer.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode++;
		 if(B->Mode > WEAP_STORES_TOO_MODE)
				 B->Mode = WEAP_STORES_TOO_MODE;
   }
	 else if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_FLT_READY)
	 {
		 B->Flt++;
		 if(B->Flt > WEAP_STORES_LOW)
				 B->Flt = WEAP_STORES_LOW;
	 }
	 else if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TERM_READY)
	 {
		 B->Term++;
		 if(B->Term > WEAP_STORES_POP)
				 B->Term = WEAP_STORES_POP;
	 }
	 else if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		   B->Tgt++;
		   if(B->Tgt >= AvNumGpsLabels)
				 B->Tgt = AvNumGpsLabels-1;
			 Av.Hsi.CurGps = B->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageDown()
{
	 if(WeapStores.Slamer.ReadyForProg == -1) return;

   SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

	 if(WeapStores. Slamer.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode--;
		 if(B->Mode < WEAP_STORES_PB_MODE)
				 B->Mode = WEAP_STORES_PB_MODE;
   }
	 else if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_FLT_READY)
	 {
		 B->Flt--;
		 if(B->Flt < WEAP_STORES_HIGH)
				 B->Flt = WEAP_STORES_HIGH;
	 }
	 else if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TERM_READY)
	 {
		 B->Term--;
		 if(B->Term < WEAP_STORES_SKIM)
				 B->Term = WEAP_STORES_SKIM;
	 }
	 else if(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		 		B->Tgt--;
		 		if(B->Tgt < 0)
				 		B->Tgt = 0;
			  Av.Hsi.CurGps = B->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerPageToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL1(0,0),"FLT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW3_COL1(0,0),"TERM:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"TGT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"LABEL:",AV_GREEN,1.0);

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.Slamer.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

  GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

  SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

	switch(B->Mode)
	{
		case WEAP_STORES_PB_MODE: sprintf(TmpStr,"PB"); break;
    case WEAP_STORES_TOO_MODE:  sprintf(TmpStr,"TOO"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),TmpStr,AV_GREEN,1.0);

	switch(B->Flt)
	{
		case WEAP_STORES_HIGH: sprintf(TmpStr,"HIGH"); break;
    case WEAP_STORES_MED:  sprintf(TmpStr,"MED"); break;
    case WEAP_STORES_LOW:  sprintf(TmpStr,"LOW"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW2_COL2(0,0),TmpStr,AV_GREEN,1.0);

	switch(B->Term)
	{
		case WEAP_STORES_SKIM: sprintf(TmpStr,"SKIM"); break;
    case WEAP_STORES_POP:  sprintf(TmpStr,"POP"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW3_COL2(0,0),TmpStr,AV_GREEN,1.0);

	// get target numbers ex 5.2
	if(B->Tgt >= 0)
	{
  		sprintf(TmpStr,"%d",B->Tgt+1);
			DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);
	}

	// GET LABEL
	if(B->Tgt >= 0)
	{
  	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),AvGpsLabels[B->Tgt]->sGPSLabel,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SlamerWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,0,"MODE",(WeapStores.Slamer.ReadyForProg == WEAP_STORES_MODE_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,1,"FLT",(WeapStores.Slamer.ReadyForProg == WEAP_STORES_FLT_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,2,"TRM",(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TERM_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,4,"TGT",(WeapStores.Slamer.ReadyForProg == WEAP_STORES_TGT_READY),AV_GREEN,1.0);
  DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if(WeapStores.Slamer.InZone)
		DrawTextAtLoc(106,12,"IN ZONE",AV_GREEN,1.0);
	else if(WeapStores.Slamer.Condition == WEAP_STORES_MAX_RANGE_ERROR)
	{
#if 0
		int Min,Sec;
		Min = WeapStores.Slamer.TimeToMaxInSec;
		Min /= 60;
		if(Min > 59) Min = 59;
		Sec = WeapStores.Slamer.TimeToMaxInSec % 60;
		sprintf(TmpStr,"%02d:%02d TTMR",Min,Sec);
#else
		int Sec;
		Sec = WeapStores.Slamer.TimeToMaxInSec;
		if(Sec > 99)
			Sec = 99;
		sprintf(TmpStr,"%02d TTMR",Sec);
		DrawTextAtLoc(106,12,TmpStr,AV_GREEN,1.0);
#endif
	}
	else if(WeapStores.Slamer.Condition == WEAP_STORES_MIN_RANGE_ERROR)
	  DrawTextAtLoc(106,12,"MIN RNG",AV_GREEN,1.0);
	else if(WeapStores.Slamer.Condition == WEAP_STORES_ALT_ERROR)
		DrawTextAtLoc(106,12,"ALT",AV_GREEN,1.0);
	else if(WeapStores.Slamer.Condition == WEAP_STORES_OFF_AXIS_ERROR)
		DrawTextAtLoc(98,12,"OFF AXIS",AV_GREEN,1.0);
	else if(WeapStores.Slamer.Condition == WEAP_STORES_NO_TARGET_ERROR)
		DrawTextAtLoc(94,12,"NO TARGET",AV_GREEN,1.0);

  SlamerTable(MpdNum);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: SlamerPageToggleMode();
							 break;
			case  1: SlamerPageToggleFlt();
							 break;
			case  2: SlamerPageToggleTerm();
							 break;
			case  4: SlamerPageToggleTgt();
							 break;
			case  5: SlamerPageChangeProg();
							 break;
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: SlamerPageSaveProg();
			         break;
			case 11: SlamerPageStep();
							 break;
			case 12: SlamerPageDown();
			         break;
			case 13: SlamerPageUp();
			         break;
			case 14: SlamerPageToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}



//*****************************************************************************************************************************************
// HARPOON WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void HarpoonPageToggleMode()
{
  if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_MODE_READY)
		 WeapStores.Harpoon.ReadyForProg = -1;
	else
		 WeapStores.Harpoon.ReadyForProg = WEAP_STORES_MODE_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageToggleFlt()
{
  if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_FLT_READY)
		 WeapStores.Harpoon.ReadyForProg = -1;
	else
		 WeapStores.Harpoon.ReadyForProg = WEAP_STORES_FLT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageToggleTerm()
{
  if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_TERM_READY)
		 WeapStores.Harpoon.ReadyForProg = -1;
	else
		 WeapStores.Harpoon.ReadyForProg = WEAP_STORES_TERM_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageToggleSeek()
{
  if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEEK_READY)
		 WeapStores.Harpoon.ReadyForProg = -1;
	else
		 WeapStores.Harpoon.ReadyForProg = WEAP_STORES_SEEK_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageToggleSearch()
{
  if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEARCH_READY)
		 WeapStores.Harpoon.ReadyForProg = -1;
	else
		 WeapStores.Harpoon.ReadyForProg = WEAP_STORES_SEARCH_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageStep()
{
  AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageChangeProg()
{
  WeapStores.Harpoon.CurProg++;
  if(WeapStores.Harpoon.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.Harpoon.CurProg = 0;

  HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageSaveProg()
{
	memcpy(SavedHarpoonProgs,HarpoonProgs,sizeof(HarpoonProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageUp()
{
	 if(WeapStores.Harpoon.ReadyForProg == -1) return;

   HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

	 if(WeapStores. Harpoon.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode++;
		 if(B->Mode > HARPOON_BOL_MODE)
				 B->Mode = HARPOON_BOL_MODE;
   }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_FLT_READY)
	 {
		 B->Flt++;
		 if(B->Flt > WEAP_STORES_LOW)
				 B->Flt = WEAP_STORES_LOW;
	 }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_TERM_READY)
	 {
		 B->Term++;
		 if(B->Term > WEAP_STORES_POP)
				 B->Term = WEAP_STORES_POP;
	 }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEEK_READY)
	 {
		 B->Seek++;
		 if(B->Seek > WEAP_STORES_SMALL)
				 B->Seek = WEAP_STORES_SMALL;
	 }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEARCH_READY)
	 {
		 B->Search += 10;
		 if(B->Search > 100)
				 B->Search = 100;
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageDown()
{
	 if(WeapStores.Harpoon.ReadyForProg == -1) return;

   HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

	 if(WeapStores. Harpoon.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode--;
		 if(B->Mode < HARPOON_RBL_MODE)
				 B->Mode = HARPOON_RBL_MODE;
   }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_FLT_READY)
	 {
		 B->Flt--;
		 if(B->Flt < WEAP_STORES_HIGH)
				 B->Flt = WEAP_STORES_HIGH;
	 }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_TERM_READY)
	 {
		 B->Term--;
		 if(B->Term < WEAP_STORES_SKIM)
				 B->Term = WEAP_STORES_SKIM;
	 }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEEK_READY)
	 {
		 B->Seek--;
		 if(B->Seek < WEAP_STORES_LARGE)
				 B->Seek = WEAP_STORES_LARGE;
	 }
	 else if(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEARCH_READY)
	 {
		 B->Search -= 10;
		 if(B->Search < 10)
				 B->Search = 10;
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonPageToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL1(0,0),"FLT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW3_COL1(0,0),"TERM:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"SEEK:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"SRCH:",AV_GREEN,1.0);

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.Harpoon.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

  GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

  HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

	switch(B->Mode)
	{
		case HARPOON_RBL_MODE: sprintf(TmpStr,"R/BL"); break;
    case HARPOON_BOL_MODE:  sprintf(TmpStr,"BOL"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),TmpStr,AV_GREEN,1.0);

	switch(B->Flt)
	{
		case WEAP_STORES_HIGH: sprintf(TmpStr,"HIGH"); break;
    case WEAP_STORES_MED:  sprintf(TmpStr,"MED"); break;
    case WEAP_STORES_LOW:  sprintf(TmpStr,"LOW"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW2_COL2(0,0),TmpStr,AV_GREEN,1.0);

	switch(B->Term)
	{
		case WEAP_STORES_SKIM: sprintf(TmpStr,"SKIM"); break;
    case WEAP_STORES_POP:  sprintf(TmpStr,"POP"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW3_COL2(0,0),TmpStr,AV_GREEN,1.0);

	switch(B->Seek)
	{
		case WEAP_STORES_LARGE: sprintf(TmpStr,"LARGE"); break;
    case WEAP_STORES_MED:   sprintf(TmpStr,"MED"); break;
    case WEAP_STORES_SMALL: sprintf(TmpStr,"SMALL"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);

  sprintf(TmpStr,"%d NM",B->Search);
	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarpoonWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,0,"MODE",(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_MODE_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,1,"FLT",(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_FLT_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,2,"TRM",(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_TERM_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,3,"SEK",(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEEK_READY),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,4,"SRCH",(WeapStores.Harpoon.ReadyForProg == WEAP_STORES_SEARCH_READY),AV_GREEN,1.0);
  DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if((PlayerPlane->Altitude < 200) && (HarpoonProgs[WeapStores.Harpoon.CurProg].Mode == HARPOON_BOL_MODE))
		DrawTextAtLoc(106,12,"ALT",AV_GREEN,1.0);
	else if(WeapStores.Harpoon.InZone)
		DrawTextAtLoc(106,12,"IN ZONE",AV_GREEN,1.0);
	else if( (WeapStores.Harpoon.Condition == WEAP_STORES_MAX_RANGE_ERROR) && (HarpoonProgs[WeapStores.Harpoon.CurProg].Mode == HARPOON_RBL_MODE) )
	{
#if 0
		int Min,Sec;
		Min = WeapStores.Harpoon.TimeToMaxInSec;
		Min /= 60;
		if(Min > 59) Min = 59;
		Sec = WeapStores.Harpoon.TimeToMaxInSec % 60;
		sprintf(TmpStr,"%02d:%02d TTMR",Min,Sec);
#else
		int Sec;
		Sec = WeapStores.Harpoon.TimeToMaxInSec;
		if(Sec > 99)
			Sec = 99;
		sprintf(TmpStr,"%02d TTMR",Sec);
#endif
		DrawTextAtLoc(106,12,TmpStr,AV_GREEN,1.0);
	}
	else if(WeapStores.Harpoon.Condition == WEAP_STORES_MIN_RANGE_ERROR)
	  DrawTextAtLoc(106,12,"MIN RNG",AV_GREEN,1.0);
	else if(WeapStores.Harpoon.Condition == WEAP_STORES_ALT_ERROR)
		DrawTextAtLoc(106,12,"ALT",AV_GREEN,1.0);
	else if(WeapStores.Harpoon.Condition == WEAP_STORES_OFF_AXIS_ERROR)
		DrawTextAtLoc(98,12,"OFF AXIS",AV_GREEN,1.0);
	else if(WeapStores.Harpoon.Condition == WEAP_STORES_NO_TARGET_ERROR)
		DrawTextAtLoc(94,12,"NO TARGET",AV_GREEN,1.0);

  HarpoonTable(MpdNum);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: HarpoonPageToggleMode();
							 break;
			case  1: HarpoonPageToggleFlt();
							 break;
			case  2: HarpoonPageToggleTerm();
							 break;
			case  3: HarpoonPageToggleSeek();
							 break;
			case  4: HarpoonPageToggleSearch();
							 break;
			case  5: HarpoonPageChangeProg();
							 break;
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: HarpoonPageSaveProg();
			         break;
			case 11: HarpoonPageStep();
							 break;
			case 12: HarpoonPageDown();
			         break;
			case 13: HarpoonPageUp();
			         break;
			case 14: HarpoonPageToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}


//*****************************************************************************************************************************************
// ROCKET POD WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void RocketPodCCIP()
{
	WeapStores.RocketPod.Mode = WEAP_STORES_CCIP_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RocketPodMan()
{
	WeapStores.RocketPod.Mode = WEAP_STORES_MAN_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RocketPodSgl()
{
	WeapStores.RocketPod.FireMethod = ROCKET_POD_SINGLE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RocketPodSal()
{
	WeapStores.RocketPod.FireMethod = ROCKET_POD_SALVO;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RocketPodGun()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RocketPodStep()
{
	int WeapId     = GetSelectedAGWeaponId();
  int Unselected = GetNumStationsUnselected(WeapId);

	if(Unselected == 0)
	{
		WeapStores.RocketPod.NumStations = 1;
  	SetAGWeapon(WeapId,WeapStores.RocketPod.NumStations);
	}
	else
	{
		WeapStores.RocketPod.NumStations++;
		if(WeapStores.RocketPod.NumStations > 6)
			WeapStores.RocketPod.NumStations = 1;

		SetAGWeapon(WeapId,WeapStores.RocketPod.NumStations);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void RocketPodWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,0,"CCIP",(WeapStores.RocketPod.Mode == WEAP_STORES_CCIP_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,1,"MAN",(WeapStores.RocketPod.Mode == WEAP_STORES_MAN_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,2,"SGL",(WeapStores.RocketPod.FireMethod == ROCKET_POD_SINGLE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,3,"SAL",(WeapStores.RocketPod.FireMethod == ROCKET_POD_SALVO),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",(WeapStores.GunsOn),AV_GREEN,1.0);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:  RocketPodCCIP();
							  break;
			case  1:  RocketPodMan();
						    break;
			case  2:  RocketPodSgl();
							  break;
			case  3:  RocketPodSal();
							  break;
			case  9:  ToggleHarmOverride();
							  break;
			case  11: RocketPodStep();
							  break;
			case  14: RocketPodGun();
							  break;
			case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							  break;
			case 19:  AGLeftSelect();
			          break;
			case 18:  AGFrontLeftSelect();
							  break;
			case 17:  AGCenterSelect();
							  break;
			case 16:  AGFrontRightSelect();
							  break;
			case 15:  AGRightSelect();
							  break;
		}
	}
}

//*****************************************************************************************************************************************
// CLUSTER BOMB WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void ClusterBombPageToggleMode()
{
  if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MODE_READY)
		 WeapStores.ClusterBomb.ReadyForProg = -1;
	else
		 WeapStores.ClusterBomb.ReadyForProg = WEAP_STORES_MODE_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageToggleHob()
{
  if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_HOB_READY)
		 WeapStores.ClusterBomb.ReadyForProg = -1;
	else
		 WeapStores.ClusterBomb.ReadyForProg = WEAP_STORES_HOB_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageToggleQty()
{
  if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_QTY_READY)
		 WeapStores.ClusterBomb.ReadyForProg = -1;
	else
		 WeapStores.ClusterBomb.ReadyForProg = WEAP_STORES_QTY_READY;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageToggleMult()
{
  if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MULT_READY)
		 WeapStores.ClusterBomb.ReadyForProg = -1;
	else
		 WeapStores.ClusterBomb.ReadyForProg = WEAP_STORES_MULT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageToggleInv()
{
  if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_INV_READY)
		 WeapStores.ClusterBomb.ReadyForProg = -1;
	else
		 WeapStores.ClusterBomb.ReadyForProg = WEAP_STORES_INV_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageChangeProg()
{
  WeapStores.ClusterBomb.CurProg++;
  if(WeapStores.ClusterBomb.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.ClusterBomb.CurProg = 0;

  BombProgType *B = &ClusterBombProgs[WeapStores.ClusterBomb.CurProg];

	// jlm need data for detonate height
  SetBombingAttributes(B->Qty,B->Mult,B->Inv,B->Hob);
  SetAGWeapon(GetSelectedAGWeaponId(),B->Mult);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageSaveProg()
{
	memcpy(SavedClusterBombProgs,ClusterBombProgs,sizeof(BombProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageUp()
{
	 if(WeapStores.ClusterBomb.ReadyForProg == -1) return;

   BombProgType *B = &ClusterBombProgs[WeapStores.ClusterBomb.CurProg];

	 if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode++;
		 if(B->Mode > WEAP_STORES_MAN_MODE)
				 B->Mode = WEAP_STORES_MAN_MODE;
 	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_QTY_READY)
	 {
		 B->Qty++;
		 if(B->Qty > 12)
				 B->Qty = 12;
	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_HOB_READY)
	 {
		 B->Hob += 100;
		 if(B->Hob > 2500)
				 B->Hob = 2500;
	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MULT_READY)
	 {
		 B->Mult++;
		 if(B->Mult > 12)
				 B->Mult = 12;
	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_INV_READY)
	 {
		 B->Inv += 50;
		 if(B->Inv > 950)
				 B->Inv = 950;
	 }

	 // jlm need data for detonate height
   SetBombingAttributes(B->Qty,B->Mult,B->Inv,B->Hob);
	 SetAGWeapon(GetSelectedAGWeaponId(),B->Mult);

	 if( (B->Mult == 1) && (B->Qty > 1) )
     SelectStationsForCount(GetSelectedAGWeaponId(),B->Qty);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ClusterBombPageDown()
{
	 if(WeapStores.ClusterBomb.ReadyForProg == -1) return;

   BombProgType *B = &ClusterBombProgs[WeapStores.ClusterBomb.CurProg];

	 if(WeapStores. ClusterBomb.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode--;
		 if(B->Mode < WEAP_STORES_CCIP_MODE)
				 B->Mode = WEAP_STORES_CCIP_MODE;
   }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_QTY_READY)
	 {
		 B->Qty--;
		 if(B->Qty < 1)
				 B->Qty = 1;
	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_HOB_READY)
	 {
		 B->Hob -= 100;
		 if(B->Hob < 100)
				 B->Hob = 100;
	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MULT_READY)
	 {
		 B->Mult--;
		 if(B->Mult < 1)
				 B->Mult = 1;
	 }
	 else if(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_INV_READY)
	 {
		 B->Inv -= 50;
		 if(B->Inv < 50)
				 B->Inv = 50;
	 }

	 // jlm need data for detonate height
   SetBombingAttributes(B->Qty,B->Mult,B->Inv,B->Hob);
	 SetAGWeapon(GetSelectedAGWeaponId(),B->Mult);

	 if( (B->Mult == 1) && (B->Qty > 1) )
     SelectStationsForCount(GetSelectedAGWeaponId(),B->Qty);
}

//*****************************************************************************************************************************************
void ClusterBombPageToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
void ClusterBombTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL1(0,0),"HOB:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW3_COL1(0,0),"SEQ:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"QTY:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"MULT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW3_COL3(0,0),"INV:",AV_GREEN,1.0);

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.ClusterBomb.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

	GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

	BombProgType *B = &ClusterBombProgs[WeapStores.ClusterBomb.CurProg];


	switch(B->Mode)
	{
		case WEAP_STORES_AUTO_MODE: sprintf(TmpStr,"AUTO"); break;
	    case WEAP_STORES_CCIP_MODE: sprintf(TmpStr,"CCIP"); break;
		case WEAP_STORES_MAN_MODE : sprintf(TmpStr,"MAN"); break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d FT",B->Hob);
	DrawTextAtLoc(WS_TABLE_ROW2_COL2(0,0),TmpStr,AV_GREEN,1.0);

	if( (B->Mult == 1) && (B->Qty == 1) )
		sprintf(TmpStr,"SINGLE");
	else if( (B->Mult > 1) && (B->Qty <= B->Mult) )
		sprintf(TmpStr,"SALVO");
	else if( (B->Mult == 1) && (B->Qty > 1) )
		sprintf(TmpStr,"RPL SGL");
	else if( (B->Qty > B->Mult) && (B->Mult > 1) )
		sprintf(TmpStr,"RPL MULT");

	DrawTextAtLoc(WS_TABLE_ROW3_COL2(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",B->Qty);
	DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",B->Mult);
	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d FT",B->Inv);
	DrawTextAtLoc(WS_TABLE_ROW3_COL4(0,0),TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
void ClusterBombWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

	DrawVerticalWordAtButton(MpdNum,0,"MODE",(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MODE_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,1,"HOB",(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_HOB_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,2,"QTY",(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_QTY_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,3,"MLT",(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_MULT_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,4,"INV",(WeapStores.ClusterBomb.ReadyForProg == WEAP_STORES_INV_READY),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);


	DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	ClusterBombTable(MpdNum);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: ClusterBombPageToggleMode();
							 break;
			case  1: ClusterBombPageToggleHob();
							 break;
			case  2: ClusterBombPageToggleQty();
							 break;
			case  3: ClusterBombPageToggleMult();
							 break;
			case  4: ClusterBombPageToggleInv();
							 break;
			case  5: ClusterBombPageChangeProg();
							 break;
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: ClusterBombPageSaveProg();
			         break;
			case 12: ClusterBombPageDown();
			         break;
			case 13: ClusterBombPageUp();
			         break;
			case 14: ClusterBombPageToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
// DUMB/GUIDED WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DumbBombPageToggleMode()
{
  if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MODE_READY)
		 WeapStores.DumbBomb.ReadyForProg = -1;
	else
		 WeapStores.DumbBomb.ReadyForProg = WEAP_STORES_MODE_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageToggleDrag()
{
  if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_DRAG_READY)
		 WeapStores.DumbBomb.ReadyForProg = -1;
	else
		 WeapStores.DumbBomb.ReadyForProg = WEAP_STORES_DRAG_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageToggleQty()
{
  if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_QTY_READY)
		 WeapStores.DumbBomb.ReadyForProg = -1;
	else
		 WeapStores.DumbBomb.ReadyForProg = WEAP_STORES_QTY_READY;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageToggleMult()
{
  if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MULT_READY)
		 WeapStores.DumbBomb.ReadyForProg = -1;
	else
		 WeapStores.DumbBomb.ReadyForProg = WEAP_STORES_MULT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageToggleInv()
{
  if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_INV_READY)
		 WeapStores.DumbBomb.ReadyForProg = -1;
	else
		 WeapStores.DumbBomb.ReadyForProg = WEAP_STORES_INV_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageChangeProg()
{
  WeapStores.DumbBomb.CurProg++;
  if(WeapStores.DumbBomb.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.DumbBomb.CurProg = 0;

  BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

	// jlm need data for detonate height
  SetBombingAttributes(B->Qty,B->Mult,B->Inv,100);
  SetAGWeapon(GetSelectedAGWeaponId(),B->Mult);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageSaveProg()
{
	memcpy(SavedBombProgs,BombProgs,sizeof(BombProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageUp()
{
	 if(WeapStores.DumbBomb.ReadyForProg == -1) return;

   BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

	 if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode++;
		 if(B->Mode > WEAP_STORES_MAN_MODE)
				 B->Mode = WEAP_STORES_MAN_MODE;

		if(Av.Weapons.CurAGWeap)
		{
			if(Av.Weapons.CurAGWeap->W->WeapId == 116)  //  AGM-123 Skipper
			{
				B->Mode = WEAP_STORES_MAN_MODE;
			}
		}
 	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_QTY_READY)
	 {
		 B->Qty++;
		 if(B->Qty > 12)
				 B->Qty = 12;
	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_DRAG_READY)
	 {
		 B->Drag++;
		 if(B->Drag > WEAP_STORES_RETARDED_MODE)
				 B->Drag = WEAP_STORES_RETARDED_MODE;
	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MULT_READY)
	 {
		 B->Mult++;
		 if(B->Mult > 12)
				 B->Mult = 12;
	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_INV_READY)
	 {
		 B->Inv += 50;
		 if(B->Inv > 950)
				 B->Inv = 950;
	 }

	 // jlm need data for detonate height
   SetBombingAttributes(B->Qty,B->Mult,B->Inv,-1);
	 SetAGWeapon(GetSelectedAGWeaponId(),B->Mult);

	 if( (B->Mult == 1) && (B->Qty > 1) )
     SelectStationsForCount(GetSelectedAGWeaponId(),B->Qty);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageDown()
{
	 if(WeapStores.DumbBomb.ReadyForProg == -1) return;

   BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

	 if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MODE_READY)
	 {
		 B->Mode--;
		 if(B->Mode < WEAP_STORES_CCIP_MODE)
				 B->Mode = WEAP_STORES_CCIP_MODE;
		if(Av.Weapons.CurAGWeap)
		{
			if(Av.Weapons.CurAGWeap->W->WeapId == 116)  //  AGM-123 Skipper
			{
				B->Mode = WEAP_STORES_MAN_MODE;
			}
		}
	}
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_QTY_READY)
	 {
		 B->Qty--;
		 if(B->Qty < 1)
				 B->Qty = 1;
	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_DRAG_READY)
	 {
		 B->Drag--;
		 if(B->Drag < WEAP_STORES_FREE_FALL_MODE)
				 B->Drag = WEAP_STORES_FREE_FALL_MODE;
	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MULT_READY)
	 {
		 B->Mult--;
		 if(B->Mult < 1)
				 B->Mult = 1;
	 }
	 else if(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_INV_READY)
	 {
		 B->Inv -= 50;
		 if(B->Inv < 50)
				 B->Inv = 50;
	 }

	 // jlm need data for detonate height
   SetBombingAttributes(B->Qty,B->Mult,B->Inv,-1);
	 SetAGWeapon(GetSelectedAGWeaponId(),B->Mult);

	 if( (B->Mult == 1) && (B->Qty > 1) )
     SelectStationsForCount(GetSelectedAGWeaponId(),B->Qty);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombPageToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
	if(!WeapStores.GunsOn)
		ResetToRWS();
}

//*****************************************************************************************************************************************
void DumbBombTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL1(0,0),"DRAG:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW3_COL1(0,0),"SEQ:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"QTY:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"MULT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW3_COL3(0,0),"INV:",AV_GREEN,1.0);

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.DumbBomb.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

	GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

	BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

	if(Av.Weapons.CurAGWeap)
	{
		if(Av.Weapons.CurAGWeap->W->WeapId == 116)  //  AGM-123 Skipper
		{
			B->Mode = WEAP_STORES_MAN_MODE;
		}
	}

	switch(B->Mode)
	{
		case WEAP_STORES_AUTO_MODE:
			sprintf(TmpStr,"AUTO");
		break;
		case WEAP_STORES_CCIP_MODE:
			sprintf(TmpStr,"CCIP");
		break;
		case WEAP_STORES_MAN_MODE:
			sprintf(TmpStr,"MAN");
		break;
	}
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),TmpStr,AV_GREEN,1.0);

	switch(B->Drag)
	{
		case WEAP_STORES_FREE_FALL_MODE:
			sprintf(TmpStr,"FF");
		break;
		case WEAP_STORES_RETARDED_MODE:
			sprintf(TmpStr,"RETARD");
		break;
	}
	DrawTextAtLoc(WS_TABLE_ROW2_COL2(0,0),TmpStr,AV_GREEN,1.0);

	if( (B->Mult == 1) && (B->Qty == 1) )
		sprintf(TmpStr,"SINGLE");
	else if( (B->Mult > 1) && (B->Qty <= B->Mult) )
		sprintf(TmpStr,"SALVO");
	else if( (B->Mult == 1) && (B->Qty > 1) )
		sprintf(TmpStr,"RPL SGL");
	else if( (B->Qty > B->Mult) && (B->Mult > 1) )
		sprintf(TmpStr,"RPL MULT");

	DrawTextAtLoc(WS_TABLE_ROW3_COL2(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",B->Qty);
	DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",B->Mult);
	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d FT",B->Inv);
	DrawTextAtLoc(WS_TABLE_ROW3_COL4(0,0),TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DumbBombWeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
		DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
		DrawHarmMode();
	}

	DrawVerticalWordAtButton(MpdNum,0,"MODE",(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MODE_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,1,"DRG",(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_DRAG_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,2,"QTY",(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_QTY_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,3,"MLT",(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_MULT_READY),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,4,"INV",(WeapStores.DumbBomb.ReadyForProg == WEAP_STORES_INV_READY),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	DumbBombTable(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);

	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:
				DumbBombPageToggleMode();
			break;
			case  1:
				DumbBombPageToggleDrag();
			break;
			case  2:
				DumbBombPageToggleQty();
			break;
			case  3:
				DumbBombPageToggleMult();
			break;
			case  4:
				DumbBombPageToggleInv();
			break;
			case  5:
				DumbBombPageChangeProg();
			break;
			case  7:
				if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			break;
			case  9:
				ToggleHarmOverride();
			break;
			case 10:
				DumbBombPageSaveProg();
			break;
			case 12:
				DumbBombPageDown();
			break;
			case 13:
				DumbBombPageUp();
			break;
			case 14:
				DumbBombPageToggleGuns();
			break;
			case 19:
				AGLeftSelect();
			break;
			case 18:
				AGFrontLeftSelect();
			break;
			case 17:
				AGCenterSelect();
			break;
			case 16:
				AGFrontRightSelect();
			break;
			case 15:
				AGRightSelect();
			break;
		}
	}
}


//*****************************************************************************************************************************************
// AIM-9 And AIM120 WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AIM9And120WeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 11: MissleReject();
			         break;
			case 14: AAToggleGuns();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
// AIM-7 WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void ToggleAim7Loft()
{
	WeapStores.Aim7LoftOn = !WeapStores.Aim7LoftOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AIM7WeapPage(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,5,"LOFT",WeapStores.Aim7LoftOn,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 11: MissleReject();
			         break;
			case 5: ToggleAim7Loft();
							 break;
			case 14: AAToggleGuns();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
// GUNS WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void ToggleGunFireRate()
{
 if(WeapStores.GunRate == HIGH)
 {
	WeapStores.GunRate = LOW;
	PlayerPlane->GunFireRate = 2;
 }
 else
 {
	WeapStores.GunRate = HIGH;
	PlayerPlane->GunFireRate = 1;
 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleAGun()
{
	WeapStores.AGunOn = !WeapStores.AGunOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleFunnel()
{
	WeapStores.FunnelOn = !WeapStores.FunnelOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GunsWeapStores(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(UFC.MasterMode != AG_MODE)
		DrawVerticalWordAtButton(MpdNum,2,"AGUN",(WeapStores.AGunOn),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,(WeapStores.GunRate == HIGH) ? "HIGH" : "LOW" ,FALSE,AV_GREEN,1.0);
	//DrawWordAtMpdButton(MpdNum,6,"DSTB",(WeapStores.DSTBOn),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);
  DrawWordAtMpdButton(MpdNum,6,"FNNL",(WeapStores.FunnelOn),AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"%03d",PlayerPlane->WeapLoad[GUNS_STATION].Count);
	DrawTextAtLoc(64,14,TmpStr,AV_GREEN,1.0);
	if(WeapStores.GunsOn)
	  DrawBoxAroundWord(TmpStr,64,14,Triple.Red,Triple.Green,Triple.Blue);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  2:
							if(UFC.MasterMode != AG_MODE)
								ToggleAGun();
							 break;
			case  5: ToggleGunFireRate();
							 break;
			case  6: ToggleFunnel();
							 break;
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 14: AAToggleGuns();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DisplayNoWeap(int MpdNum)
{
	DrawWeaponsOutline();
	DrawStations();

	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,"%03d",PlayerPlane->WeapLoad[GUNS_STATION].Count);
	DrawTextAtLoc(64,14,TmpStr,AV_GREEN,1.0);
	if(WeapStores.GunsOn)
  	DrawBoxAroundWord(TmpStr,64,14,Triple.Red,Triple.Green,Triple.Blue);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  14: AAToggleGuns();
								break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoWeaponStoresMode(int MpdNum)
{
	if(WeapStores.GunsOn)
	{
	  DrawTextAtLoc(62,74,(UFC.MasterArmState == ON) ? "ARM" : "SAFE",AV_GREEN,1.0,TRUE);
	  DrawWeaponSelections(MpdNum);
		GunsWeapStores(MpdNum);
		return;
	}

  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(UFC.MasterMode == AA_MODE)
	{
	  DrawTextAtLoc(62,74,(UFC.MasterArmState == ON) ? "ARM" : "SAFE",AV_GREEN,1.0,TRUE);

		sprintf(TmpStr,"%03d",PlayerPlane->WeapLoad[GUNS_STATION].Count);
		DrawTextAtLoc(64,14,TmpStr,AV_GREEN,1.0);
	  if(WeapStores.GunsOn)
		  DrawBoxAroundWord(TmpStr,64,14,Triple.Red,Triple.Green,Triple.Blue);

		if(Av.Weapons.UseAASeq)
		{
				if(Av.Weapons.CurAASeqWeap)
				{
	  					switch(Av.Weapons.CurAASeqWeap->WeapPage)
							{
								case AIM7_WEAP_PAGE:   AIM7WeapPage(MpdNum); break;
								case AIM9_WEAP_PAGE:
								case AIM120_WEAP_PAGE: AIM9And120WeapPage(MpdNum); break;
							}
				}
				else
           DisplayNoWeap(MpdNum);
		}
		else
		{
				if(Av.Weapons.CurAAWeap)
				{
	  					switch(Av.Weapons.CurAAWeap->WeapPage)
							{
							  case AIM7_WEAP_PAGE:   AIM7WeapPage(MpdNum); break;
						      case AIM9_WEAP_PAGE:
							  case AIM120_WEAP_PAGE: AIM9And120WeapPage(MpdNum); break;
							}
				}
				else
					DisplayNoWeap(MpdNum);
		}
	}
	else
	{
	  DrawWeaponSelections(MpdNum);

		if(Av.Gbu.WeapFired)
		{
			WalleyeWeapPage(MpdNum);
		}
		else
		{

				if(Av.Weapons.CurAGWeap)
				{
	    			switch(Av.Weapons.CurAGWeap->WeapPage)
						{
							case DUMB_WEAP_PAGE:
							case CLUSTER_WEAP_PAGE:
							case ROCKET_POD_WEAP_PAGE:
							case TALD_WEAP_PAGE:
																					sprintf(TmpStr,"%03d",PlayerPlane->WeapLoad[GUNS_STATION].Count);
																					DrawTextAtLoc(64,14,TmpStr,AV_GREEN,1.0);
	                                        if(WeapStores.GunsOn)
																					  DrawBoxAroundWord(TmpStr,64,14,Triple.Red,Triple.Green,Triple.Blue);
																					break;
						}

	    			switch(Av.Weapons.CurAGWeap->WeapPage)
						{
							case DUMB_WEAP_PAGE:
							case CLUSTER_WEAP_PAGE:
							case ROCKET_POD_WEAP_PAGE:
							case HARPOON_WEAP_PAGE:
							case SLAMER_WEAP_PAGE:
							case JSOW_WEAP_PAGE:
							case JDAM_WEAP_PAGE:
							case TALD_WEAP_PAGE:

 	                                   DrawTextAtLoc(62,74,(UFC.MasterArmState == ON) ? "ARM" : "SAFE",AV_GREEN,1.0,TRUE);
																		 break;
						}

						switch(Av.Weapons.CurAGWeap->WeapPage)
						{
							case DUMB_WEAP_PAGE:       DumbBombWeapPage(MpdNum); break;
							case CLUSTER_WEAP_PAGE:    ClusterBombWeapPage(MpdNum); break;
							case ROCKET_POD_WEAP_PAGE: RocketPodWeapPage(MpdNum); break;
							case AGM_65_WEAP_PAGE:     AGM65WeapPage(MpdNum); break;
							case AGM_65F_WEAP_PAGE:    AGM65WeapPage(MpdNum); break;
							case AGM_65E_WEAP_PAGE:    AGM65EWeapPage(MpdNum); break;
							case HARM_WEAP_PAGE:       HarmWeapPage(MpdNum); break;
							case HARPOON_WEAP_PAGE:    HarpoonWeapPage(MpdNum); break;
							case SLAMER_WEAP_PAGE:     SlamerWeapPage(MpdNum); break;
							case WALLEYE_WEAP_PAGE:    WalleyeWeapPage(MpdNum); break;
							case JSOW_WEAP_PAGE:       JsowWeapPage(MpdNum); break;
							case JDAM_WEAP_PAGE:       JdamWeapPage(MpdNum); break;
							case TALD_WEAP_PAGE:       TaldWeapPage(MpdNum); break;
						}
				}
				else
					DisplayNoWeap(MpdNum);
		}

	}

	DrawTDCIndicator(MpdNum);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmChangeProg();

void AGChangeProgram()
{
	if(Av.Weapons.CurAGWeap == NULL)  return;

	switch(Av.Weapons.CurAGWeap->WeapPage)
	{
		case DUMB_WEAP_PAGE:       DumbBombPageChangeProg();    break;
		case CLUSTER_WEAP_PAGE:    ClusterBombPageChangeProg(); break;
		case HARPOON_WEAP_PAGE:    HarpoonPageChangeProg();     break;
		case SLAMER_WEAP_PAGE:     SlamerPageChangeProg();      break;
		case JSOW_WEAP_PAGE:       JsowPageChangeProg();        break;
		case JDAM_WEAP_PAGE:       JdamPageChangeProg();        break;
    case HARM_WEAP_PAGE:       HarmChangeProg();            break;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetAGWeapPageAndModeForHud(int *Page, int *Mode)
{
   if(Av.Weapons.CurAGWeap == NULL)
			return(FALSE);

	 *Page = Av.Weapons.CurAGWeap->WeapPage;
	 *Mode = -1; // assume mode isn't valid

	 if( (*Page == DUMB_WEAP_PAGE) || (*Page == CLUSTER_WEAP_PAGE) )
	 {
      *Mode = (*Page == DUMB_WEAP_PAGE) ? BombProgs[WeapStores.DumbBomb.CurProg].Mode : ClusterBombProgs[WeapStores.ClusterBomb.CurProg].Mode;
	 }
	 else if(*Page == HARM_WEAP_PAGE)
	 {
		  *Mode = Av.Harm.Mode;
	 }

	 return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetHarmInfoForHud(FPointDouble *TargetLocation,float *MinRange, float *MaxRange)
{
	if(Av.Harm.Mode == HARM_TOO_MODE)
	{
			if(!Av.Harm.CurHarmObj)
			{
				return(FALSE);
			}

			if(Av.Harm.CurHarmObjType == TEWS_SAM_PROV_THREAT)
			{
	  		*TargetLocation = ((InfoProviderInstance *)Av.Harm.CurHarmObj)->Basics.Position;
			}
			else if(Av.Harm.CurHarmObjType == TEWS_SAM_WEAP_THREAT)
			{
	  		  *TargetLocation   = ((AAWeaponInstance *)Av.Harm.CurHarmObj)->Basics.Position;
			}
			else if(Av.Harm.CurHarmObjType == TEWS_VEH_THREAT)
			{
	  		  *TargetLocation = ((MovingVehicleParams *)Av.Harm.CurHarmObj)->WorldPosition;
			}
	}
	else if(Av.Harm.Mode == HARM_PB_MODE)
	{
		if(AvNumGpsLabels <= 0)
		{
			return(FALSE);
		}

		HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

		if(H->Tgt >= 0)
		{
				TargetLocation->X = AvGpsLabels[H->Tgt]->lX*FTTOWU;
				TargetLocation->Z = AvGpsLabels[H->Tgt]->lY*FTTOWU;
				TargetLocation->Y = LandHeight(TargetLocation->X,TargetLocation->Z);
		}
		else
		{
			return(FALSE);
		}
	}

	DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
	WGetGBUMavMinMax(PlayerPlane,pDBWeapon,MinRange,MaxRange);

	// convert from ft to word units
	(*MinRange) = (*MinRange) * FTTOWU;
	(*MaxRange) = (*MaxRange) * FTTOWU;

	return(TRUE);
}

//*****************************************************************************************************************************************
// HARM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************



int NumHarmObjects;
HarmObjectType HarmObjects[AV_MAX_HARM_OBJECTS];

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitHarm()
{
	Av.Harm.Mode = HARM_TOO_MODE;
	Av.Harm.SurfaceFilter = HARM_LAND_SEA;
	Av.Harm.LimitOn = FALSE;
	Av.Harm.CurHarmObj = NULL;
	Av.Harm.SelfProtectOn = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int HarmInList(void *Obj)
{
	if(NumHarmObjects <= 0) return(FALSE);

	HarmObjectType *Ptr = &HarmObjects[0];

	while(Ptr <= &HarmObjects[NumHarmObjects-1])
	{
		if(Ptr->Obj == Obj)
			return(TRUE);
		Ptr++;
	}

	return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddObjectToHarmList( void *Obj,int Type,char *TEWSAbbrev,int InFOV,float XOff,float YOff,int HasLock)
{
  HarmObjectType *Ptr;

	if( (Av.Harm.LimitOn) && (LimitCount > 5) ) return;

	if(NumHarmObjects+1 >= AV_MAX_HARM_OBJECTS) return;

	if( HarmInList(Obj) ) return;

	int UseLand = (Av.Harm.SurfaceFilter == HARM_LAND_SEA) || (Av.Harm.SurfaceFilter == HARM_LAND);
	int UseSea  = (Av.Harm.SurfaceFilter == HARM_LAND_SEA) || (Av.Harm.SurfaceFilter == HARM_SEA);

	if(Type == TEWS_SAM_PROV_THREAT)
	{
		if(!UseLand) return;
	}
	else if(Type == TEWS_SAM_WEAP_THREAT)
	{
		if(!UseLand) return;
	}
	else
	{
		if( ((MovingVehicleParams *)Obj)->iShipType )
		{
			if(!UseSea) return;
		}
		else
		{
			if(!UseLand) return;
		}
	}

	NumHarmObjects++;

	Ptr = &HarmObjects[NumHarmObjects - 1];

	if(Type == TEWS_SAM_PROV_THREAT)
	{
		Ptr->Obj = (InfoProviderInstance *)Obj;
	}
	else if(Type == TEWS_SAM_WEAP_THREAT)
	{
		Ptr->Obj = (AAWeaponInstance *)Obj;
	}
	else
	{
		Ptr->Obj = (MovingVehicleParams *)Obj;
	}

	Ptr->XOff = XOff;
	Ptr->YOff = YOff;
	Ptr->Type = Type;
	Ptr->TEWSAbbrev = TEWSAbbrev;
	Ptr->InHarmFOV = InFOV;
	Ptr->HasLock = HasLock;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmClick(int XOff,int YOff)
{
	int Found = FALSE;
	HarmObjectType *Walk = &HarmObjects[0];
	HarmObjectType *Last = &HarmObjects[NumHarmObjects - 1];
  HarmObjectType *Obj;

	while(Walk <= Last)
	{
		int Inside = IsPointInside(Walk->XOff,Walk->YOff,YOff - 10,YOff + 10,XOff - 10,XOff + 10);
		if(Inside)
		{
			Found = TRUE;
			Obj = Walk;
			break;
		}
		Walk++;
	}

	if(!Found) return;

	if(Obj->Type == TEWS_SAM_PROV_THREAT)
	{
		Av.Harm.CurHarmObjType = TEWS_SAM_PROV_THREAT;
	  Av.Harm.CurHarmObj     = (InfoProviderInstance *)(Obj->Obj);
	}
	else if(Obj->Type == TEWS_SAM_WEAP_THREAT)
	{
		Av.Harm.CurHarmObjType = TEWS_SAM_WEAP_THREAT;
	  Av.Harm.CurHarmObj     = (AAWeaponInstance *)(Obj->Obj);
	}
	else if(Obj->Type == TEWS_VEH_THREAT)
	{
		Av.Harm.CurHarmObjType = TEWS_VEH_THREAT;
	  Av.Harm.CurHarmObj     = (MovingVehicleParams *)(Obj->Obj);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmUnDesignate()
{
	int Found = FALSE;
	HarmObjectType *Walk = &HarmObjects[0];
	HarmObjectType *Last = &HarmObjects[NumHarmObjects - 1];

	while(Walk <= Last)
	{
		if(Walk->Obj == Av.Harm.CurHarmObj)
		{
			if(Walk->InHarmFOV)
			{
				Found = TRUE;
				break;
			}
		}
		Walk++;
	}

	if(!Found)
	{
		if(NumHarmObjects <= 0)
		{
  		Av.Harm.CurHarmObj = NULL;
		}
		else
		{
			Av.Harm.CurHarmObjType = HarmObjects[0].Type;
			Av.Harm.CurHarmObj = HarmObjects[0].Obj;
		}
		return;
	}

	Walk++;
	if(Walk > Last)
		Walk = &HarmObjects[0];

	while(Walk->Obj != Av.Harm.CurHarmObj)
	{
		if(Walk->InHarmFOV)
		{
			Av.Harm.CurHarmObjType = Walk->Type;
			Av.Harm.CurHarmObj = Walk->Obj;
			break;
		}

		Walk++;
	  if(Walk > Last)
		Walk = &HarmObjects[0];
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHarmObject(float ScrX,float ScrY,ThreatEntryType *T)
{
	float WordX,WordY;
	int IsDesignated;

	int UseLand = (Av.Harm.SurfaceFilter == HARM_LAND_SEA) || (Av.Harm.SurfaceFilter == HARM_LAND);
	int UseSea  = (Av.Harm.SurfaceFilter == HARM_LAND_SEA) || (Av.Harm.SurfaceFilter == HARM_SEA);

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if( (T->Type == TEWS_VEH_THREAT) && (T->V->iShipType))
	{
		if(!UseSea)
			return;
	}
	else if(!UseLand)
	{
		return;
	}

	if( (T->Type == TEWS_VEH_THREAT) && (T->V->iShipType) )
	{
			WordX = ScrX - 3; WordY = ScrY - 2;

	    DrawTextAtLoc(WordX,WordY,T->TEWSAbbrev,AV_GREEN,1.0);

			GrDrawLine(GrBuffFor3D,WordX - 2,WordY + 4,WordX + 0,WordY + 6,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,WordX + 0,WordY + 6,WordX + 6,WordY + 6,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,WordX + 6,WordY + 6,WordX + 8,WordY + 4,Triple.Red,Triple.Green,Triple.Blue,0);

	    IsDesignated = (Av.Harm.CurHarmObj == T->V);

			if(IsDesignated)
			 DrawBox(GrBuffFor3D,ScrX - 10,ScrY - 10,20,20,Triple.Red,Triple.Green,Triple.Blue);
	}
	else if( (T->Type == TEWS_VEH_THREAT) || (T->Type == TEWS_SAM_PROV_THREAT) )
	{
		WordX = ScrX - 3; WordY = ScrY - 2;
	  DrawTextAtLoc(WordX,WordY,T->TEWSAbbrev,AV_GREEN,1.0);

		if(T->Type == TEWS_SAM_PROV_THREAT)
	    IsDesignated = (Av.Harm.CurHarmObj == T->SP);
		else
	    IsDesignated = (Av.Harm.CurHarmObj == T->V);

		if(IsDesignated)
			DrawBox(GrBuffFor3D,ScrX - 10,ScrY - 10,20,20,Triple.Red,Triple.Green,Triple.Blue);
	}
	else if(T->Type == TEWS_SAM_WEAP_THREAT)
	{
		WordX = ScrX - 3; WordY = ScrY - 2;
//		sprintf(TmpStr,"A");
//	  DrawTextAtLoc(WordX,WordY,TmpStr,AV_GREEN,1.0);
	  DrawTextAtLoc(WordX,WordY,T->TEWSAbbrev,AV_GREEN,1.0);

	  IsDesignated = (Av.Harm.CurHarmObj == T->SW);

		if(IsDesignated)
			DrawBox(GrBuffFor3D,ScrX - 10,ScrY - 10,20,20,Triple.Red,Triple.Green,Triple.Blue);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHarmArrow(int Dir)
{
	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);


	if(Dir == LEFT)
	{
		int X = 14;
		int Y = 60;

		GrDrawLine(GrBuffFor3D,X,Y,X+13,Y,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X,Y+1,X+13,Y+1,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X+1,Y,X+1,Y+1,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X+2,Y-1,X+2,Y+3,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X+3,Y-1,X+3,Y+3,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X+4,Y-2,X+4,Y+4,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X+5,Y-2,X+5,Y+4,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X+6,Y-3,X+6,Y+5,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X+7,Y-3,X+7,Y+5,Color.Red,Color.Green,Color.Blue,0);
	}
	else
	{
		int X = 125;
		int Y = 60;

		GrDrawLine(GrBuffFor3D,X,Y,X-13,Y,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X,Y+1,X-13,Y+1,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X-1,Y,X-1,Y+1,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X-2,Y-1,X-2,Y+3,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X-3,Y-1,X-3,Y+3,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X-4,Y-2,X-4,Y+4,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X-5,Y-2,X-5,Y+4,Color.Red,Color.Green,Color.Blue,0);

    GrDrawLine(GrBuffFor3D,X-6,Y-3,X-6,Y+5,Color.Red,Color.Green,Color.Blue,0);
    GrDrawLine(GrBuffFor3D,X-7,Y-3,X-7,Y+5,Color.Red,Color.Green,Color.Blue,0);

	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessHarmView()
{
	ThreatEntryType *T = &Threats.List[0];
	void *Obj;
	int ArrowDraw = FALSE;

	LimitCount = 0;

	NumHarmObjects = 0;

	int Index = Threats.NumThreats;
	while( (Index-- > 0) )
	{
		 if( (T->Type == TEWS_SAM_PROV_THREAT) || (T->Type == TEWS_SAM_WEAP_THREAT) || (T->Type == TEWS_VEH_THREAT) )
		 {
		 		FPointDouble RadarPos;
				float ScrX,ScrY;

				if(T->Type == TEWS_VEH_THREAT)
					RadarPos = T->V->WorldPosition;
				else
				  RadarPos = (T->Type == TEWS_SAM_PROV_THREAT) ? T->SP->Basics.Position : T->SW->Basics.Position;

        FPoint Vec;
        Vec.MakeVectorToFrom(RadarPos,Av.Harm.CamPos);

				int InView;
				FMatrix Rot;

				Rot = PlayerPlane->Attitude;
				Rot.Transpose();

				PushClipRegion();
        SetClipRegion(0,140,0,140,71,71);

				InView = Vec.BenignRotateAndPerspect(&ScrX,&ScrY,Rot,30.0*(float)DEGREE,GrBuffFor3D);

				PopClipRegion();

			 	if(T->Type == TEWS_SAM_PROV_THREAT)
					Obj = (InfoProviderInstance *)T->SP;
		   	else if(T->Type == TEWS_SAM_WEAP_THREAT)
					Obj = (AAWeaponInstance *)T->SW;
			  else
					Obj = (MovingVehicleParams *)T->V;

				if(InView)
				{
						DrawHarmObject(ScrX,ScrY,T);
            AddObjectToHarmList(Obj,T->Type,T->TEWSAbbrev,TRUE,ScrX,ScrY,T->HasLock);
						LimitCount++;
						if(Av.Harm.LimitOn && LimitCount >= 5) break;
				}
				else
				{
					// if(T->HasLock)
					// {
						 if(!ArrowDraw)
						 {
							   ArrowDraw = TRUE;
								 FPointDouble Loc;

			 					 if(T->Type == TEWS_SAM_PROV_THREAT)
									Loc = ( (InfoProviderInstance *)T->SP)->Basics.Position;
		   					 else if(T->Type == TEWS_SAM_WEAP_THREAT)
									Loc = ( (AAWeaponInstance *)T->SW)->Basics.Position;
			  				 else
									Loc = ( (MovingVehicleParams *)T->V)->WorldPosition;

                 float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,Loc,1);

								 if(DeltaAzim >= 0)
								 {
                   DrawHarmArrow(LEFT);
								 }
								 else
								 {
                   DrawHarmArrow(RIGHT);
 								 }
						 }
					// }
				}
		 }
		 T++;
	}

	// check for selected emitter no longer in the list
	int Found = FALSE;
	HarmObjectType *Walk = &HarmObjects[0];
	HarmObjectType *Last = &HarmObjects[NumHarmObjects - 1];

	while(Walk <= Last)
	{
		if(Walk->Obj == Av.Harm.CurHarmObj)
		{
			if(Walk->InHarmFOV)
			{
				Found = TRUE;
				break;
			}
		}
		Walk++;
	}

	if(!Found)
	{
		Av.Harm.CurHarmObj = NULL;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetFirstHarmObject(int UseLock = 1)
{
	if(NumHarmObjects <= 0) return;

	// go from last to first since tews is ordered from low priority to high for drawing purposes
	int Found = FALSE;

	HarmObjectType *Walk  = &HarmObjects[NumHarmObjects - 1];
	HarmObjectType *First = &HarmObjects[0];

	while(Walk >= First)
	{
		if(Walk->InHarmFOV && (UseLock ? Walk->HasLock : TRUE) )
		{
			Found = TRUE;
			break;
		}
		Walk--;
	}

	if(Found)
	{
		Av.Harm.CurHarmObj = Walk->Obj;
		Av.Harm.CurHarmObjType = Walk->Type;
	}
	else
	{
		Av.Harm.CurHarmObj = NULL;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimHarmSP()
{
	if(Av.Harm.Mode != HARM_SP_MODE) return;
	if( !(IsWeaponOnBoard(AGM88_ID)) ) return;

	ThreatEntryType *T = &Threats.List[0];
	float MinRange = 1000000;
	FPoint Vec;

	int Found = FALSE;
	int Index = Threats.NumThreats;
	while(Index-- > 0)
	{
		 if(T->HasLock)
		 {
		 		if( (T->Category == TEWS_SAM_PROV_THREAT) || (T->Category == TEWS_SAM_WEAP_THREAT) || (T->Category == TEWS_VEH_THREAT) )
		 		{
						Found = TRUE;
						break;
		 		}
		 }
		 T++;
	}

	if(Found && !WeapStores.HarmOverride)
	{
		UFCSetAGMode();
		Av.Harm.SelfProtectOn = TRUE;
		int WeapId = GetWeapId(AGM88_ID);
		SetAGWeapon(WeapId,1);
	}
	else if(Found)
	{
		Av.Harm.SelfProtectOn = TRUE;
//		HudHarmSPMode((PlaneParams *)Camera1.AttachedObject);
	}
	else
	{
		Av.Harm.SelfProtectOn = FALSE;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmRSET()
{


}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmToggleLimit()
{
	Av.Harm.LimitOn = !Av.Harm.LimitOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmToggleSurfaceFilter()
{
	Av.Harm.SurfaceFilter++;
	if(Av.Harm.SurfaceFilter > HARM_LAND_SEA)
		Av.Harm.SurfaceFilter = HARM_LAND;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmToggleTgt()
{
   if(WeapStores.Harm.ReadyForProg == WEAP_STORES_TGT_READY)
		 WeapStores.Harm.ReadyForProg = -1;
	 else
		 WeapStores.Harm.ReadyForProg = WEAP_STORES_TGT_READY;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmStep()
{
  AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmChangeProg()
{
  WeapStores.Harm.CurProg++;
  if(WeapStores.Harm.CurProg >= MAX_BOMB_PROGS)
	  WeapStores.Harm.CurProg = 0;

  HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmSaveProg()
{
	memcpy(SavedHarmProgs,HarmProgs,sizeof(HarmProgType)*MAX_BOMB_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmPageUp()
{
	 if(WeapStores.Harm.ReadyForProg == -1) return;

   HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

	 if(WeapStores.Harm.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		   H->Tgt++;
		   if(H->Tgt >= AvNumGpsLabels)
				 H->Tgt = AvNumGpsLabels-1;
			 Av.Hsi.CurGps = H->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmPageDown()
{
	 if(WeapStores.Harm.ReadyForProg == -1) return;

   HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

	 if(WeapStores.Harm.ReadyForProg == WEAP_STORES_TGT_READY)
	 {
		 if(AvNumGpsLabels > 0)
		 {
		 		H->Tgt--;
		 		if(H->Tgt < 0)
				 		H->Tgt = 0;
			 Av.Hsi.CurGps = H->Tgt;
		 }
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetHarmSPMode()
{
	Av.Harm.Mode = HARM_SP_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetHarmTOOMode()
{
	Av.Harm.Mode = HARM_TOO_MODE;
	Av.Harm.SelfProtectOn = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetHarmPBMode()
{
	Av.Harm.Mode = HARM_PB_MODE;
	Av.Harm.SelfProtectOn = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmPBTable(int MpdNum)
{
	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"MODE:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"TGT:",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(0,0),"LABEL:",AV_GREEN,1.0);

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"PROGRAM %d",WeapStores.Harm.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-17,0),TmpStr,AV_GREEN,1.0);

  GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);

  HarmProgType *H = &HarmProgs[WeapStores.Harm.CurProg];

	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),"PB",AV_GREEN,1.0);

	// get target numbers ex 5.2
	if(H->Tgt >= 0)
	{
  		sprintf(TmpStr,"%d",H->Tgt+1);
			DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),TmpStr,AV_GREEN,1.0);
	}

	// GET LABEL
	if(H->Tgt >= 0)
	{
  	DrawTextAtLoc(WS_TABLE_ROW2_COL4(0,0),AvGpsLabels[H->Tgt]->sGPSLabel,AV_GREEN,1.0);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

long ltempoffy = 36;

void HarmPBMode(int MpdNum)
{
	DBWeaponType *pDBWeapon;

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	DrawWeaponsOutline();
	DrawStations();

	HarmPBTable(MpdNum);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
		DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
		DrawHarmMode();
	}


	if(PlayerPlane->AGDesignate.X != -1)
	{
		float MinRange,MaxRange;
		FPointDouble TargetLoc;

		pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange, &MaxRange);

//  	GetWeapCamTargetPosition(&Av.Mav,&TargetLoc);

		float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);
		DistToTarget *= WUTOFT;

		int Time = (DistToTarget - MaxRange)/PlayerPlane->IfHorzVelocity;

		if(Time > 99)
			Time = 99;
		if(Time < 0)
			Time = 0;

		sprintf(TmpStr,"%d %s", Time, "TTMR" );

		if( (DistToTarget < MaxRange) && (DistToTarget > MinRange) )
			RightJustifyTextAtLocation(125,13,"IN RANGE",Color.Red,Color.Green,Color.Blue);
//		else
//			RightJustifyTextAtLocation(125,21,TmpStr,Color.Red,Color.Green,Color.Blue);

		char tempstr[32];
		int min, secs, secs2, secs3;

		long lworkticks = (long)lHARMToF - (GameLoopInTicks - (long)lHARMLaunched);

		if(lworkticks >= 0)
		{
			secs2 = secs = lHARMToF / 1000;
			min = secs / 60;
			secs = secs % 60;
			sprintf(tempstr, "TOF %02d:%02d", min, secs);
			DrawTextAtLoc(93, 28, tempstr, AV_GREEN,1.0);

			secs3 = secs = lworkticks / 1000;
			min = secs / 60;
			secs = secs % 60;
			sprintf(tempstr, "FLT %02d:%02d", min, secs);
			DrawTextAtLoc(93, 34, tempstr, AV_GREEN,1.0);

			secs = secs2 - secs3;
			if(secs < 0)
				secs = 0;
			min = secs / 60;
			secs = secs % 60;
			sprintf(tempstr, "%02d:%02d", min, secs);
			DrawTextAtLoc(109, 41, tempstr, AV_GREEN,1.0);
		}
		else
		{
			secs = AV_CurrAGMissileToF();
			min = secs / 60;
			secs = secs % 60;
			sprintf(tempstr, "TOF %02d:%02d", min, secs);
			DrawTextAtLoc(93, 28, tempstr, AV_GREEN,1.0);

			sprintf(tempstr, "%02d:%02d", min, secs);
			DrawTextAtLoc(109, 41, tempstr, AV_GREEN,1.0);
		}

		GrDrawLine(GrBuffFor3D, 108, 40, 127, 40, Color.Red,Color.Green,Color.Blue,0);
	}

  DrawVerticalWordAtButton(MpdNum,0,"SP",(Av.Harm.Mode == HARM_SP_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,1,"TOO",(Av.Harm.Mode == HARM_TOO_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,2,"PB",(Av.Harm.Mode == HARM_PB_MODE),AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,4,"TGT",(WeapStores.Harm.ReadyForProg == WEAP_STORES_TGT_READY),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,"PROG",FALSE,AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,10,"SAVE",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: SetHarmSPMode();
							 break;
		  case  1: SetHarmTOOMode();
							 break;
			case  2: SetHarmPBMode();
							 break;
			case  4: HarmToggleTgt();
							 break;
			case  5: HarmChangeProg();
							 break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: HarmSaveProg();
			         break;
			case 11: HarmStep();
							 break;
			case 12: HarmPageDown();
			         break;
			case 13: HarmPageUp();
			         break;
			case 14: HarmToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmSPMode(int MpdNum)
{
	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,0,"SP",(Av.Harm.Mode == HARM_SP_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,1,"TOO",(Av.Harm.Mode == HARM_TOO_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,2,"PB",(Av.Harm.Mode == HARM_PB_MODE),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,"LIMIT",(Av.Harm.LimitOn),AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,10,"RST",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);

	switch(Av.Harm.SurfaceFilter)
	{
		case HARM_LAND:     sprintf(TmpStr,"LAND");
										    break;
		case HARM_SEA:      sprintf(TmpStr,"SEA");
										    break;
		case HARM_LAND_SEA: sprintf(TmpStr,"L/S");
										    break;
	}
	DrawVerticalWordAtButton(MpdNum,13,TmpStr,WeapStores.GunsOn,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	// STATION NUMBER
	sprintf(TmpStr,"STA %d",Av.Weapons.CurAGWeap->Station + 1);
	DrawTextAtLoc(TEWS_CENTER_X - 50,TEWS_CENTER_Y - 50,TmpStr,AV_GREEN,1.0);

	// crosshair
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y - 3,TEWS_CENTER_X,TEWS_CENTER_Y + 3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 3,TEWS_CENTER_Y,TEWS_CENTER_X + 3,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);

	// side brackets
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 60,TEWS_CENTER_Y,TEWS_CENTER_X - 60 + 3,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 60,TEWS_CENTER_Y - 1,TEWS_CENTER_X - 60,TEWS_CENTER_Y + 1,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X + 60,TEWS_CENTER_Y,TEWS_CENTER_X + 60 - 3,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X + 60,TEWS_CENTER_Y - 1,TEWS_CENTER_X + 60,TEWS_CENTER_Y + 1,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y - 60,TEWS_CENTER_X,TEWS_CENTER_Y - 60 + 3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 1,TEWS_CENTER_Y - 60,TEWS_CENTER_X + 1,TEWS_CENTER_Y - 60,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y + 60,TEWS_CENTER_X,TEWS_CENTER_Y + 60 - 3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 1,TEWS_CENTER_Y + 60,TEWS_CENTER_X + 1,TEWS_CENTER_Y + 60,Color.Red,Color.Green,Color.Blue,0);

  ProcessHarmView();
	GetFirstHarmObject();

//	if(Av.Harm.CurHarmObj == NULL)
//	  Av.Harm.SelfProtectOn = FALSE;

	if(Av.Harm.SelfProtectOn)
	{
	  AvRGB Triple;
    GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

    GrDrawString(GrBuffFor3D, LgHUDFont, TEWS_CENTER_X - 10,TEWS_CENTER_Y - 20,"HARM",Triple.Red,Triple.Green,Triple.Blue,0);
	}

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: SetHarmSPMode();
							 break;
		  case  1: SetHarmTOOMode();
							 break;
			case  2: SetHarmPBMode();
							 break;
			case  5: HarmToggleLimit();
							 break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: HarmRSET();
			         break;
			case 11: HarmStep();
							 break;
			case 13: HarmToggleSurfaceFilter();
			         break;
			case 14: HarmToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHarmMode()
{
	switch(Av.Harm.Mode)
	{
	 	case  HARM_SP_MODE:   sprintf(TmpStr,"SP"); break;
	 	case  HARM_TOO_MODE:  sprintf(TmpStr,"TOO"); break;
	 	case  HARM_PB_MODE:   sprintf(TmpStr,"PB"); break;
	}

	DrawTextAtLoc(110,126,TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmTOOMode(int MpdNum)
{
	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	ProcessHarmView();

	if(Av.Harm.CurHarmObj == NULL)
		  GetFirstHarmObject(FALSE);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,0,"SP",(Av.Harm.Mode == HARM_SP_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,1,"TOO",(Av.Harm.Mode == HARM_TOO_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,2,"PB",(Av.Harm.Mode == HARM_PB_MODE),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,"LIMIT",(Av.Harm.LimitOn),AV_GREEN,1.0);

  DrawVerticalWordAtButton(MpdNum,10,"RST",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);

	switch(Av.Harm.SurfaceFilter)
	{
		case HARM_LAND:     sprintf(TmpStr,"LAND");
										    break;
		case HARM_SEA:      sprintf(TmpStr,"SEA");
										    break;
		case HARM_LAND_SEA: sprintf(TmpStr,"L/S");
										    break;
	}
	DrawVerticalWordAtButton(MpdNum,13,TmpStr,WeapStores.GunsOn,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	// STATION NUMBER
	sprintf(TmpStr,"STA %d",Av.Weapons.CurAGWeap->Station + 1);
	DrawTextAtLoc(TEWS_CENTER_X - 50,TEWS_CENTER_Y - 50,TmpStr,AV_GREEN,1.0);

	// crosshair
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y - 3,TEWS_CENTER_X,TEWS_CENTER_Y + 3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 3,TEWS_CENTER_Y,TEWS_CENTER_X + 3,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);

	// side brackets
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 60,TEWS_CENTER_Y,TEWS_CENTER_X - 60 + 3,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 60,TEWS_CENTER_Y - 1,TEWS_CENTER_X - 60,TEWS_CENTER_Y + 1,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X + 60,TEWS_CENTER_Y,TEWS_CENTER_X + 60 - 3,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X + 60,TEWS_CENTER_Y - 1,TEWS_CENTER_X + 60,TEWS_CENTER_Y + 1,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y - 60,TEWS_CENTER_X,TEWS_CENTER_Y - 60 + 3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 1,TEWS_CENTER_Y - 60,TEWS_CENTER_X + 1,TEWS_CENTER_Y - 60,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y + 60,TEWS_CENTER_X,TEWS_CENTER_Y + 60 - 3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 1,TEWS_CENTER_Y + 60,TEWS_CENTER_X + 1,TEWS_CENTER_Y + 60,Color.Red,Color.Green,Color.Blue,0);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0: SetHarmSPMode();
							 break;
		  case  1: SetHarmTOOMode();
							 break;
			case  2: SetHarmPBMode();
							 break;
			case  5: HarmToggleLimit();
							 break;
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 10: HarmRSET();
			         break;
			case 11: HarmStep();
							 break;
			case 13: HarmToggleSurfaceFilter();
			         break;
			case 14: HarmToggleGuns();
			         break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HarmWeapPage(int MpdNum)
{
	DrawWeaponSelections(MpdNum);
	GetCamPos(&Av.Harm);

	if(Av.Harm.Mode == HARM_PB_MODE)
   HarmPBMode(MpdNum);
	else if(Av.Harm.Mode == HARM_SP_MODE)
	 HarmSPMode(MpdNum);
	else
	 HarmTOOMode(MpdNum);
}

//*****************************************************************************************************************************************
// MAVERIC CAM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

WeapCamSlewType MavWeapCamSlew;
WeapCamSlewType GbuWeapCamSlew;
WeapCamSlewType FlirSlew;

FPointDouble LaserLoc;
FPointDouble FlirPos;

//*****************************************************************************************************************************************
// 3D WEAP CAM FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int CameraPerspect(WeapCamType *Cam,FPointDouble TargetPos,float *ScrX,float *ScrY,int UseFlir = 0)
{
	FPointDouble RadarPos;
    FPoint Vec;

	if(UseFlir)
	  Vec.MakeVectorToFrom(TargetPos,FlirPos);
	else
	  Vec.MakeVectorToFrom(TargetPos,Cam->CamPos);

	int InView;
	FMatrix Rot;

	if(UseFlir)
	{
	  Rot.SetRadRPH(0,Av.Flir.SlewUpDown*DEGREES_TO_RADIANS,(fANGLE_TO_DEGREES(PlayerPlane->Heading) + Av.Flir.SlewLeftRight)*DEGREES_TO_RADIANS);
	  Rot.Transpose();
	}
	else
	{
	  Rot.SetRadRPH(0,Cam->SlewUpDown*DEGREES_TO_RADIANS,Cam->SlewLeftRight*DEGREES_TO_RADIANS);
	  Rot *= PlayerPlane->Attitude;
	  Rot.Transpose();
	}

	PushClipRegion();
  	SetClipRegion(0,140,0,140,70,70);

    int Magnify = (UseFlir) ? Av.Flir.Magnify : Cam->Magnify;

	InView = Vec.BenignRotateAndPerspect(ScrX,ScrY,Rot,Magnify,GrBuffFor3D);

	PopClipRegion();

	return(InView);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FireMaveric(int *TargetType, FPointDouble *TargetLoc,void **Target,int*InKeyhole)
{
   TargetLoc->X = -1;

	 if(!Av.Mav.TargetObtained)
	    *TargetType = FLIR_CAM_NO_TARGET;
   else
   {
    if(Av.Mav.TargetType == FLIR_CAM_GROUND_TARGET)
      *Target = (BasicInstance *)Av.Mav.Target;
    else if(Av.Mav.TargetType == FLIR_CAM_VEH_TARGET)
      *Target = (MovingVehicleParams *)Av.Mav.Target;
    else if(Av.Mav.TargetType == FLIR_CAM_PLANE_TARGET)
      *Target = (PlaneParams *)Av.Mav.Target;
    else if(Av.Mav.TargetType == FLIR_CAM_LOC_TARGET )
      *TargetLoc = Av.Mav.TargetLoc;

	   *TargetType = Av.Mav.TargetType;
   }

		// jlm add for when weapapage is not up, but mavs are selected
	 /*
   if(!AreMpdsInGivenMode(WPN_MODE))
   {
     if(PlayerPlane->AGDesignate.X != -1)
	 {
       *TargetType = FLIR_CAM_LOC_TARGET;
       *TargetLoc = PlayerPlane->AGDesignate;
	 }
   }
		* */

   *InKeyhole = Av.Mav.TargetInKeyhole;
   Av.Mav.WeapFired = TRUE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FireWalleye(int *TargetType, FPointDouble *TargetLoc,void **Target)
{
	TargetLoc->X = -1;

  if(!Av.Gbu.TargetObtained)
	  *TargetType = FLIR_CAM_NO_TARGET;
  else
  {
    if(Av.Gbu.TargetType == FLIR_CAM_GROUND_TARGET)
  	  *Target = (BasicInstance *)Av.Gbu.Target;
    else if(Av.Gbu.TargetType == FLIR_CAM_VEH_TARGET)
      *Target = (MovingVehicleParams *)Av.Gbu.Target;
    else if(Av.Gbu.TargetType == FLIR_CAM_PLANE_TARGET)
      *Target = (PlaneParams *)Av.Gbu.Target;
	else if(Av.Gbu.TargetType == FLIR_CAM_LOC_TARGET )
      *TargetLoc = Av.Gbu.TargetLoc;

	 *TargetType = Av.Gbu.TargetType;
  }

  if(*TargetType == FLIR_CAM_PLANE_TARGET)
  {
    *TargetType = FLIR_CAM_LOC_TARGET;
	  *TargetLoc  = ((PlaneParams *)Av.Gbu.Target)->WorldPosition;
  }

  Av.Gbu.WeapFired = TRUE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawCamera(int MpdNum,int XOfs,int YOfs,float SlewLeftRight,float SlewUpDown,int UseHiDetail,int DrawAsFlir,int GreenHot,FPointDouble CamLoc,int UseGbu,WeaponParams *Weap,int Magnify,WeapCamType *Cam)
{

  FPointDouble CamOffset;
  GrBuff *realbuff;
  int *TempCam;

	// kludge looks like I got whitehot and blackhot back wards...so I'll reverse it here instead of everywhere else jlm
	//GreenHot = !GreenHot;

  PushClipRegion();

	if( GrBuffFor3D->pGrBuffPolygon)
	{
		//calc a mid pt for the displays
		GrBuffFor3D->ClipLeft = XOfs - (140 >> 1);
		GrBuffFor3D->ClipRight = GrBuffFor3D->ClipLeft + 140;

		GrBuffFor3D->ClipTop = YOfs - (140 >> 1);
		GrBuffFor3D->ClipBottom = GrBuffFor3D->ClipTop + 140;

		GrBuffFor3D->MidX = XOfs;
		GrBuffFor3D->MidY = YOfs;

		realbuff = GrBuffFor3D;

	 	SetSecondPass( TRUE, NULL );

	}

	TempCam = (int *)Camera1.AttachedObject;

  Camera1.AttachedObject = NULL;


	 // JLM if Gbu, use its heading instead of the planes
	  if(UseGbu)
	  {
		   DrawLimitedView(realbuff,CamLoc,NormDegree(fANGLE_TO_DEGREES(Weap->Heading) + SlewLeftRight),NormDegree(SlewUpDown),Magnify,Mpds[MpdNum].IsMono,DrawAsFlir,GreenHot,UseHiDetail,TRUE,0,Weap);
	  }
	  else
	  {
			if(DrawAsFlir && (Cam == NULL))
		  		DrawLimitedView(realbuff,CamLoc,NormDegree(fANGLE_TO_DEGREES(PlayerPlane->Heading) + SlewLeftRight),NormDegree(SlewUpDown),Magnify,Mpds[MpdNum].IsMono,DrawAsFlir,GreenHot,UseHiDetail,TRUE,22,NULL); //22 keeps the flir pod out of view
			else
			{
		  		Cam->RelMatrix.SetRadRPH(0,DEGREES_TO_RADIANS * SlewUpDown,DEGREES_TO_RADIANS * SlewLeftRight);
		  		Cam->Matrix = Cam->RelMatrix;
		  		Cam->Matrix *= PlayerPlane->Attitude;
		  		DrawLimitedViewMatrix(realbuff,CamLoc,Cam->Matrix,Magnify,Mpds[MpdNum].IsMono,DrawAsFlir,GreenHot,UseHiDetail,TRUE,Cam->DotNum,NULL);
			}
	  }

	  //Turn off 2nd pass
	  if( GrBuffFor3D->pGrBuffPolygon)
		 SetSecondPass( FALSE, NULL);

 // DrawBox(realbuff,realbuff->ClipLeft,realbuff->ClipTop,115,110,(Mpds[MpdNum].IsMono) ? 63 : 53);

  if (realbuff == GrBuffFor3D)
  {
  	PopClipRegion();
	  Set3DScreenClip();
  	PushClipRegion();
  }

//  SetClipRegion(0, 639,0,479, 240, 320);

  Camera1.AttachedObject = TempCam;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetCamPos(WeapCamType *Cam)
{
	if(!Av.Weapons.CurAGWeap) return;

  int Station = Av.Weapons.CurAGWeap->Station;

  int dot = GetHardPoint(PlayerPlane,Station,PlayerPlane->WeapLoad[Station].Count);

  Cam->DotNum = dot;

  if(WeapStores.LastCamStation != dot)
  {
	   FMatrix TempMat;

	   TempMat.Identity();

     WeapStores.CamPosition.SetValues(0.0,0.0,0.0);
     FindDot(PlayerPlane->Type->Model, dot,GetSubObjects(PlayerPlane),WeapStores.CamPosition,TempMat);
     WeapStores.CamPosition.Z -= 2.5 FEET;
	   WeapStores.LastCamStation = dot;
  }

  Cam->CamPos.RotateInto(WeapStores.CamPosition,PlayerPlane->Attitude);
  Cam->CamPos += PlayerPlane->WorldPosition;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsPointInsideObjBoundBox(FPointDouble *P, RegisteredObject *Obj)
{
  if(P->X >= Obj->Instance->Position.X + Obj->ExtentLeft - 20)
    if(P->X <= Obj->Instance->Position.X + Obj->ExtentRight + 20)
	  if(P->Z >= Obj->Instance->Position.Z + Obj->ExtentTop - 20)
	    if(P->Z <= Obj->Instance->Position.Z + Obj->ExtentBottom + 20)
		  if(P->Y <= Obj->Instance->Position.Y + Obj->ExtentHeight + 20)
		      return(TRUE);

  return(FALSE);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int RayHitSphere( FPointDouble &pos, FPointDouble &nvector, FPointDouble &Center, double radius );

int DoesBeamIntersectBoundingBoxRotated(FPointDouble *Start, FPointDouble *End, RegisteredObject *Obj,FPointDouble *Intersect)
{
  BasicInstance *instance;
  ObjectTypeInfo *object_type;
  FPointDouble point;

  instance = Obj->Instance;

  if(!(Obj->Flags & RO_OBJECT_FALLING))
  {
       object_type = GetObjectTypeInfoFromInstance(instance);

	   if(object_type)
	   {
//			FPointDouble vec;
//			vec.MakeVectorToFrom(*End,*Start);
//			vec.Normalize();

//			if (RayHitSphere(*Start,vec,instance->Position,(double)GetObjectRadius(object_type->Model)))
	     	if(IntersectBoundingBox(Obj->Matrix,instance->Position,object_type->XYZExtentHigh,object_type->XYZExtentLow,*Start,*End,Intersect))
			{
				PointOnLineClosestToPoint(*Start,*End,instance->Position,NULL,Intersect);
	       		return(TRUE);
			}
	   }
  }

  return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int DoesBeamIntersectBoundingBox(FPointDouble *Start, FPointDouble *End, RegisteredObject *Obj,FPointDouble *Intersect)
{
  FPointDouble  P0,P1,Norm,TempIntersect;
  double NegD;
  double TVal;
  int Result;
  int Found = FALSE;
  double MinTVal = 100.0;

  float Left   = Obj->Instance->Position.X + Obj->ExtentLeft;
  float Right  = Obj->Instance->Position.X + Obj->ExtentRight;
  float Top    = Obj->Instance->Position.Z + Obj->ExtentTop ;
  float Bottom = Obj->Instance->Position.Z + Obj->ExtentBottom;
  float Up     = Obj->Instance->Position.Y + Obj->ExtentHeight;

  // do left face
  P0.X  = Left; P0.Y = Obj->Instance->Position.Y; P0.Z = Top;
  Norm = P0;
  Norm.X = -1; Norm.Y = 0; Norm.Z = 0;
  NegD = (*Start)*Norm;

  Result = DoesLineIntersectPlane(Start,End,&P0,&Norm,&TVal,&TempIntersect);

  if(Result)
  {
     if(IsPointInsideObjBoundBox(&TempIntersect,Obj))
	 {
	   Found = TRUE;
	   if(TVal < MinTVal)
	   {
         MinTVal = TVal;
		 *Intersect = TempIntersect;
	   }
	 }
  }

  // do front face
  P0.X  = Left; P0.Y = Obj->Instance->Position.Y; P0.Z = Bottom;
  Norm = P0;
  Norm.X = 0; Norm.Y = 0; Norm.Z = 1;
  NegD = (*Start)*Norm;

  Result = DoesLineIntersectPlane(Start,End,&P0,&Norm,&TVal,&TempIntersect);

  if(Result)
  {
     if(IsPointInsideObjBoundBox(&TempIntersect,Obj))
	 {
	   Found = TRUE;
	   if(TVal < MinTVal)
	   {
         MinTVal = TVal;
		 *Intersect = TempIntersect;
	   }
	 }
  }

    // do right face
  P0.X  = Right; P0.Y = Obj->Instance->Position.Y; P0.Z = Top;
  Norm = P0;
  Norm.X = 1; Norm.Y = 0; Norm.Z = 0;
  NegD = (*Start)*Norm;

  Result = DoesLineIntersectPlane(Start,End,&P0,&Norm,&TVal,&TempIntersect);

  if(Result)
  {
     if(IsPointInsideObjBoundBox(&TempIntersect,Obj))
	 {
	   Found = TRUE;
	   if(TVal < MinTVal)
	   {
         MinTVal = TVal;
		 *Intersect = TempIntersect;
	   }
	 }
  }

    // do Back face
  P0.X  = Left; P0.Y = Obj->Instance->Position.Y; P0.Z = Bottom;
  Norm = P0;
  Norm.X = 0; Norm.Y = 0; Norm.Z = -1;
  NegD = (*Start)*Norm;

  Result = DoesLineIntersectPlane(Start,End,&P0,&Norm,&TVal,&TempIntersect);

  if(Result)
  {
     if(IsPointInsideObjBoundBox(&TempIntersect,Obj))
	 {
	   Found = TRUE;
	   if(TVal < MinTVal)
	   {
         MinTVal = TVal;
		 *Intersect = TempIntersect;
	   }
	 }
  }
    // do Top face
  P0.X  = Left; P0.Y = Up; P0.Z = Top;
  Norm = P0;
  Norm.X = 0; Norm.Y = 1; Norm.Z = 0;
  NegD = (*Start)*Norm;

  Result = DoesLineIntersectPlane(Start,End,&P0,&Norm,&TVal,&TempIntersect);

  if(Result)
  {
     if(IsPointInsideObjBoundBox(&TempIntersect,Obj))
	 {
	   Found = TRUE;
	   if(TVal < MinTVal)
	   {
         MinTVal = TVal;
		 *Intersect = TempIntersect;
	   }
	 }
  }

  return(Found);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetCloseVehicleOrObject(FPointDouble *Loc,RegisteredObject **GroundObject,MovingVehicleParams **Veh,int *Type)
{
  float Left,Right,Top,Bottom,Dist;
  float VehMinDist = 1000000;

  *Veh = NULL;

  // get vehicles in box
  Dist   = 5000.0*FTTOWU;
  Left   = Loc->X - Dist;
  Right  = Loc->X + Dist;
  Top    = Loc->Z - Dist;
  Bottom = Loc->Z + Dist;

  AvObjListEntryType *VehiclePtr = &Av.Vehicles.List[0];
  int Index = Av.Vehicles.NumVehicles;

  while(Index-- > 0)
  {
    if( ((MovingVehicleParams *)VehiclePtr->Obj)->Status & VL_ACTIVE)
	  {
      if(!(((MovingVehicleParams *)VehiclePtr->Obj)->Status & VL_INVISIBLE))
	    {
				 FPointDouble VehLoc = ((MovingVehicleParams *)VehiclePtr->Obj)->WorldPosition;
         if(VehLoc.X > Left)
          if(VehLoc.X < Right)
           if(VehLoc.Z < Bottom)
            if(VehLoc.Z > Top)
		        {
			   				Dist = *Loc / ((MovingVehicleParams *)VehiclePtr->Obj)->WorldPosition;
			   				if(Dist < VehMinDist)
			   				{
				  				VehMinDist = Dist;
                  *Veh = (MovingVehicleParams *)VehiclePtr->Obj;
			   				}
						}
			}
		}
	  VehiclePtr++;
   }

   RegisteredObject *Rwalker;
   BasicInstance *binst;

   Rwalker = FirstInRangeObject;

   float ObjMinDist = 1000000000.0;

   *GroundObject = NULL;

   while(Rwalker)
   {
     if (Rwalker->Flags & RO_WAS_DRAWN)
     {
        binst = Rwalker->Instance;

        if(binst->Position.X > Left)
          if(binst->Position.X < Right)
           if(binst->Position.Z < Bottom)
            if(binst->Position.Z > Top)
		    		{
			   			Dist = *Loc / (binst->Position);
			   			if(Dist < ObjMinDist)
			   			{
                ObjMinDist = Dist;
 			     			*GroundObject = Rwalker;
							}
						}
   	 }
     Rwalker = Rwalker->NextObject;
   }

   if( (*GroundObject == NULL) && (*Veh == NULL) )
	   return(FALSE);
   else
   {
     if( (*GroundObject) && (*Veh) )
	   {
				if(ObjMinDist < VehMinDist)
		  		*Type = FLIR_CAM_GROUND_TARGET;
				else
		  		*Type = FLIR_CAM_VEH_TARGET;
		 }
	 	 else
		 {
       	if(*Veh)
	     	  *Type = FLIR_CAM_VEH_TARGET;
	   	  else
	     	  *Type = FLIR_CAM_GROUND_TARGET;
		 }
   }

   return(TRUE);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsPlaneDetectedByFlirCam(FPointDouble *P1, FPointDouble *P2,int PlaneIndex, float *Dist)
{
//		double WX,WY,WZ,EX,EY,EZ;
//		double DxS,DyS,DzS,DxE,DyE,DzE;
//		double DistToEnemy,DistToRay;
//		double Dot,Angle;
		PlaneParams *p = &Planes[PlaneIndex];
		FPointDouble intersect;
		float r;

		if((p != PlayerPlane) && (p->Type) && (p->Type->Model))
		{
			if (PointOnLineClosestToPoint(*P1,*P2,p->WorldPosition,NULL,&intersect))
			{
				r = GetObjectRadius(p->Type->Model);
				r*=r*1.5;
				if ((p->WorldPosition ^ intersect) <= r)
				{
					*Dist = *P1^intersect;
					return TRUE;
				}
			}
		}

		return FALSE;

//	  WX = P1->X;
//    WY = P1->Y;
//    WZ = P1->Z;

//    EX = Planes[PlaneIndex].WorldPosition.X;
//    EY = Planes[PlaneIndex].WorldPosition.Y;
//    EZ = Planes[PlaneIndex].WorldPosition.Z;

//    DxS = P2->X - WX;
//    DyS = P2->Y - WY;
//    DzS = P2->Z - WZ;

//    DistToRay = sqrt(DxS*DxS + DyS*DyS + DzS*DzS);

//    DxE = EX - WX;
//    DyE = EY - WY;
//    DzE = EZ - WZ;

//    DistToEnemy = sqrt(DxE*DxE + DyE*DyE + DzE*DzE);

//    Dot = DxE*DxS + DyE*DyS + DzE*DzS;

//    Angle = RadToDeg(acos(Dot/(DistToRay*DistToEnemy)));

//	*Dist = DistToEnemy;

//	return( (Angle >= 0) && (Angle <= 2.0) && (DistToEnemy < (40.0 *NMTOWU)));
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GrabPlaneInFlirWeapCam(FPointDouble *P1, FPointDouble *P2, int *PlaneIndex, float *Dist)
{
	float Distance;
	float MinDist = (40.0 * NMTOWU)*(40.0 * NMTOWU);
	int Found = FALSE;

	*Dist = MinDist;

	AvObjListEntryType *P = &Av.Planes.List[0];
	int Index = Av.Planes.NumPlanes;
	while(Index-- > 0)
	{
		if (IsPlaneDetectedByFlirCam(P1,P2,( (PlaneParams *)P->Obj - &Planes[0]),&Distance) )
		{
			if(Distance < MinDist)
			{
				MinDist = Distance;
				*Dist   = Distance;
				*PlaneIndex = ( (PlaneParams *)P->Obj - &Planes[0]);
				Found = TRUE;
			}
		}
		P++;
	}

	*Dist = sqrt(*Dist);
	return(Found);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetFlirWeapCamVehicle(FPointDouble Reference,FPointDouble LocOnTerrain,MovingVehicleParams **Target,float *Dist)
{
   float MinVehDist = 1000000000.0;
   int ObjFound = FALSE;
   int InsideBoundingBox;

	 if(LocOnTerrain.X == -1) return(FALSE);

	 *Dist = 1000000.0;

   AvObjListEntryType *FilteredVehiclePtr = &Av.Vehicles.List[0];
   int   Index = Av.Vehicles.NumVehicles;
   float Left,Right,Top,Bottom,Height;
   float DistToVeh;

   while(Index-- > 0)
   {
   			FPointDouble VehPos;

	 			VehPos = ((MovingVehicleParams *)FilteredVehiclePtr->Obj)->WorldPosition;

				FPointDouble Vec;
				DistToVeh = VehPos ^ Reference;

				InsideBoundingBox = FALSE;

	 			Left    = VehPos.X - 25.0;
   			Right   = VehPos.X + 25.0;
   			Top     = VehPos.Z - 25.0;
   			Bottom  = VehPos.Z + 25.0;
	 			Height  = VehPos.Y + 25.0;

	 			if(LocOnTerrain.X > Left)
	   			if(LocOnTerrain.X < Right)
	     			if(LocOnTerrain.Z > Top)
		   			if(LocOnTerrain.Z < Bottom)
		     			if(LocOnTerrain.Y < Height)
		       			 InsideBoundingBox = TRUE;

	 			if(InsideBoundingBox)
	 			{
    			if(DistToVeh < MinVehDist)
					{
		  			*Target = (MovingVehicleParams *)FilteredVehiclePtr->Obj;
		          	MinVehDist = DistToVeh;
		  			ObjFound = TRUE;
					}
	  		}
	  		FilteredVehiclePtr++;
   }

   if (ObjFound)
		*Dist = sqrt(MinVehDist);

   return(ObjFound);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetFlirWeapCamTarget(FPointDouble *P1,FPointDouble *P2,RegisteredObject **Target, float *Dist,FPointDouble *Intersect)
{
   RegisteredObject *Rwalker;
   float MinTargetDist = 1000000000.0;
   //BasicInstance *binst;
   int ObjFound = FALSE;
   FPointDouble TempIntersect;

   FPointDouble PointAlongBeam;

   Rwalker = FirstInRangeObject;

   *Dist = 1000000.0;

   double BeamDist = (*P2 ^ *P1);

   while(Rwalker)
   {
     if (Rwalker->Flags & RO_WAS_DRAWN)
     {
			 double DistToObj = (*P1 ^ Rwalker->Instance->Position);

       if(DistToObj <= BeamDist)
	     {
		     if( DoesBeamIntersectBoundingBoxRotated(P1,P2,Rwalker,&TempIntersect) )
		     {
      			if(DistToObj < MinTargetDist)
						{
			   			//binst = Rwalker->Instance;
			   			//*Target = binst;
							 *Target = Rwalker;

			              MinTargetDist = DistToObj;
				   			ObjFound = TRUE;
				   			*Intersect = TempIntersect;
						}
		     }
   	   }
   	 }
     Rwalker = Rwalker->NextObject;
   }

   if (ObjFound)
   		*Dist = sqrt(MinTargetDist);


   return(ObjFound);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetMaveric()
{
  Av.Mav.AquireMode = MSL_AUTO;
  Av.Mav.TrackOn = FALSE;
  Av.Mav.SlewLeftRight = 0.0;
  Av.Mav.SlewUpDown    = -12.0;
  Av.Mav.TargetSelected = FALSE;
  Av.Mav.TargetObtained = FALSE;
  Av.Mav.TargetInKeyhole = TRUE;
  Av.Mav.TrackPosObtained = FALSE;
  Av.Mav.MarkerLight = TRUE;
  Av.Mav.WithinSlewLimits = FALSE;
  Av.Mav.MarkersOn = FALSE;
  Av.Mav.DotNum = 0;

  Av.Mav.Magnify = (float)1.67*(float)DEGREE;
  Av.Mav.FOVToUse = NFOV;

  Av.Mav.DrawCross = TRUE;

	Av.Mav.ForceCorrelateOn = FALSE;
	Av.Mav.Polarity = WHOT;
	Av.Mav.CurCHPos = 5;

	Av.Mav.CurCHPosLeft  = 5;
	Av.Mav.CurCHPosRight = 5;
	Av.Mav.CurCHPosUp    = 5;
	Av.Mav.CurCHPosDown  = 5;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetWalleye()
{
  Av.Gbu.AquireMode = MSL_AUTO;
  Av.Gbu.TrackOn = FALSE;
  Av.Gbu.SlewLeftRight = 0.0;
  Av.Gbu.SlewUpDown    = -3.0;
  Av.Gbu.TargetSelected = FALSE;
  Av.Gbu.TargetObtained = FALSE;
  Av.Gbu.WeapFired = FALSE;
  Av.Gbu.Weap = NULL;
  Av.Gbu.AGDesignate.X = -1;
  Av.Gbu.AGDesignate.Y = -1;
  Av.Gbu.AGDesignate.Z = -1;

  Av.Gbu.TargetInKeyhole = TRUE;
  Av.Gbu.TrackPosObtained = FALSE;
  Av.Gbu.MarkerLight = TRUE;
  Av.Gbu.WithinSlewLimits = FALSE;
  Av.Gbu.MarkersOn = FALSE;
  Av.Gbu.DotNum = 0;

  Av.Gbu.Magnify = (float)1.67*(float)DEGREE;
  Av.Gbu.FOVToUse = NFOV;

  Av.Gbu.DrawCross = TRUE;

	Av.Gbu.ForceCorrelateOn = FALSE;
	Av.Gbu.Polarity = WHOT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetupWeapCamSlew (int Direction,int MpdNum, int Type)
{
  if(Type == MAVERIC_TYPE)
  {
    MavWeapCamSlew.Slewing = TRUE;
    MavWeapCamSlew.SlewDirection = Direction;
    MavWeapCamSlew.SlewOffset = 0.0;
    MavWeapCamSlew.SlewTimer.Set(15.0,GetTickCount());
    MavWeapCamSlew.MpdNum = MpdNum;
  }
  else if(Type == GBU15_TYPE)
  {
    GbuWeapCamSlew.Slewing = TRUE;
    GbuWeapCamSlew.SlewDirection = Direction;
    GbuWeapCamSlew.SlewOffset = 0.0;
    GbuWeapCamSlew.SlewTimer.Set(15.0,GetTickCount());
    GbuWeapCamSlew.MpdNum = MpdNum;
  }
  else // must be a flir type
  {
    FlirSlew.Slewing = TRUE;
    FlirSlew.SlewDirection = Direction;
    FlirSlew.SlewOffset = 0.0;
    FlirSlew.SlewTimer.Set(15.0,GetTickCount());
    FlirSlew.MpdNum = MpdNum;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


SLEW_ACTIONS IsCamWithinSlewBounds(WeapCamType *cam,float LeftRight, float UpDown,float *new_azim,float *new_elev)
{
	FMatrix mat;
	FMatrix *pmat;
	SLEW_ACTIONS ret_val;
	float heading_to_use;

	if (Av.Gbu.WeapFired)
	{
		FPoint x,y,z;

		z.SetValues(-cam->Weap->Vel.X,-cam->Weap->Vel.Y,-cam->Weap->Vel.Z);
		z.Normalize();

		if (fabs(z.Y) < 0.98f)
			y.SetValues(0.0f,1.0f,0.0f);
		else
			y.SetValues(0.0f,0.0f,1.0f);

		x = y;
		x %= z;
		x.Normalize();

		y = z;
		y %= x;
		y.Normalize();

		mat.m_Data.RC.R0C0 = x.X;
		mat.m_Data.RC.R1C0 = x.Y;
		mat.m_Data.RC.R2C0 = x.Z;

		mat.m_Data.RC.R0C1 = y.X;
		mat.m_Data.RC.R1C1 = y.Y;
		mat.m_Data.RC.R2C1 = y.Z;

		mat.m_Data.RC.R0C2 = z.X;
		mat.m_Data.RC.R1C2 = z.Y;
		mat.m_Data.RC.R2C2 = z.Z;

		pmat = &mat;

		heading_to_use = fANGLE_TO_DEGREES(cam->Weap->Heading);

	}
	else
	{
		pmat = &PlayerPlane->Attitude;
		heading_to_use = fANGLE_TO_DEGREES(PlayerPlane->Heading);
	}

//	check_azim = NormDegree(LeftRight + heading_to_use);

	ret_val = CheckAnglesWithinBounds(*pmat,RLAngle(LeftRight+heading_to_use),UpDown,40.0f,28.0f,-40.0f,-52.0f,new_azim,new_elev);

	if (new_azim)
		*new_azim = RLAngle(*new_azim - heading_to_use);

	return ret_val;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

//make sure we get the correct heading and pitch.



//void CalcNewFlirCamHeadingAndPitch(FPointDouble &Pos,float *Heading,float *Pitch, FPointDouble &Target,WeapCamType *cam)
//{
//	FMatrix wmat;
//	FPointDouble local_point;


//	if (cam)
//		wmat.SetRadRPH(0.0f,0.0f,ANGLE_TO_RADIANS(cam->Weap->Heading));
//	else
//		wmat = PlayerPlane->Attitude;

//	wmat.Transpose();

//	local_point.MakeVectorToFrom(Target,Pos);
//	local_point *= wmat;

//	*Heading = atan2(-local_point.X, -local_point.Z) * RADIANS_TO_DEGREES;

//	while(*Heading > 180)
//		*Heading -= 360;

//	while(*Heading < -180)
//		*Heading += 360;

//	*Pitch = -ComputePitchFromFPointDouble(local_point);

//}

void CalcNewFlirCamHeadingAndPitch(FPointDouble &Pos,float *Heading, float *Pitch, FPointDouble &Target,WeaponParams *Weap)
{
  float DeltaAzim,DeltaElev,HeadingToUse;

  if(Weap != NULL)
    HeadingToUse = Weap->Heading;
  else
    HeadingToUse = PlayerPlane->Heading;

  DeltaAzim = ComputeHeadingToPointDouble(Pos,HeadingToUse,Target,1);
  DeltaElev = -ComputePitchFromPointToPoint(Pos,Target);

  *Heading = DeltaAzim;
  *Pitch   = DeltaElev;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void PointWeapCamToCurrentAGTarget(WeapCamType *Cam)
{
  if(Cam->AquireMode == MSL_MANUAL) return;
  if(PlayerPlane->AGDesignate.X <= 0) return;

  if( (Cam->AGDesignate.X != PlayerPlane->AGDesignate.X) ||
      (Cam->AGDesignate.Y != PlayerPlane->AGDesignate.Y) ||
      (Cam->AGDesignate.Z != PlayerPlane->AGDesignate.Z) || Cam->AutoSet)
  {
    float Heading,Pitch;

	  if(Cam->Weap != NULL)
	  {
        CalcNewFlirCamHeadingAndPitch(Cam->CamPos, &Heading, &Pitch, PlayerPlane->AGDesignate,Cam->Weap);
	  }
	  else
	  {
	    FPointDouble EndPoint;
	    EndPoint.MakeVectorToFrom(PlayerPlane->AGDesignate,Cam->CamPos);

	    FMatrix TempMatrix(PlayerPlane->Attitude);

	    TempMatrix.Transpose();

	    EndPoint *= TempMatrix;

	    Heading = atan2(-EndPoint.X,-EndPoint.Z);
	    Pitch   = atan2(EndPoint.Y,sqrt(EndPoint.Z*EndPoint.Z + EndPoint.X*EndPoint.X));

	    Heading = RadToDeg(Heading);
	    Pitch   = RadToDeg(Pitch);
	  }

	  float LeftRight = Heading;
    float UpDown    = Pitch;

	  Cam->AutoSet = FALSE;

    if(!IsCamWithinSlewBounds(Cam,LeftRight,UpDown,NULL,NULL))
	  {
        	Cam->SlewLeftRight    = LeftRight;
        	Cam->SlewUpDown       = UpDown;
		    Cam->WithinSlewLimits = TRUE;

        	Cam->AGDesignate.X = PlayerPlane->AGDesignate.X;
        	Cam->AGDesignate.Y = PlayerPlane->AGDesignate.Y;
        	Cam->AGDesignate.Z = PlayerPlane->AGDesignate.Z;

		    Cam->TrackOn = TRUE;
        	Cam->TrackPosObtained = FALSE;
		    Cam->TargetObtained = FALSE;
 	  }
	  else
	  {
		  Cam->WithinSlewLimits = FALSE;
	  }
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetWeapCamTargetPosition(WeapCamType *CamInfo, FPointDouble *Target)
{
	if (CamInfo->Target)
	{
  		if(CamInfo->TargetType == FLIR_CAM_GROUND_TARGET)
  			*Target = ( (BasicInstance *)CamInfo->Target)->Position;
  		else if(CamInfo->TargetType == FLIR_CAM_VEH_TARGET)
    		*Target = ( (MovingVehicleParams *)CamInfo->Target)->WorldPosition;
  		else if(CamInfo->TargetType == FLIR_CAM_PLANE_TARGET)
    		*Target = ( (PlaneParams *)CamInfo->Target)->WorldPosition;
  		else
    		*Target = CamInfo->TargetLoc;
	}
	else
  		*Target = CamInfo->TargetLoc;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawSeekerPointer(WeapCamType *Cam,int Type,int IsMavLaser = 0)
{
  int ScrX,ScrY;
  int AdjSlewUpDown;

  // compensate for centerline being -12 degrees
  AdjSlewUpDown = Cam->SlewUpDown + 12.0;

  ScrX = CAM_CENTER_X + -Cam->SlewLeftRight*(PIX_PER_SLEW_LR);
  ScrY = CAM_CENTER_Y + -AdjSlewUpDown*(PIX_PER_SLEW_UD);

  AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(IsMavLaser)
	{
		if(Av.Mav.TrackOn && Av.Mav.TrackPosObtained && Av.Mav.TargetObtained)
		{
        DrawAvRadarSymbol(ScrX,ScrY,RDR_CLOSE_BOX,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);
		}
		else
		{
			GrDrawLine(GrBuffFor3D,ScrX - 3,ScrY - 3,ScrX + 3,ScrY + 3,Triple.Red,Triple.Green,Triple.Blue,0);
    		GrDrawLine(GrBuffFor3D,ScrX + 3,ScrY - 3,ScrX - 3,ScrY + 3,Triple.Red,Triple.Green,Triple.Blue,0);
		}
	}
  else if(Type == MAVERIC_TYPE)
  {
		GrDrawLine(GrBuffFor3D,ScrX,ScrY - 6,ScrX,ScrY+6,Triple.Red,Triple.Green,Triple.Blue,0);
    GrDrawLine(GrBuffFor3D,ScrX - 6,ScrY,ScrX+6,ScrY,Triple.Red,Triple.Green,Triple.Blue,0);
	int workx = CAM_CENTER_X;
	int worky = 0;
	int cnt;
	cnt = 5;
	worky = cnt*(PIX_PER_SLEW_UD);
	while((fabs(worky) <= 50) && (cnt <= 15))
	{
	    GrDrawLine(GrBuffFor3D,workx - 4,CAM_CENTER_Y + worky,workx+4,CAM_CENTER_Y + worky,Triple.Red,Triple.Green,Triple.Blue,0);
		cnt += 5;
		worky = cnt*(PIX_PER_SLEW_UD);
	}
  }
  else
  {
    DrawAvRadarSymbol(ScrX,ScrY,RDR_CLOSE_BOX,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawMavTicks(WeapCamType *Cam,int Type,int IsMavLaser = 0)
{
	AvRGB Triple;

	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  if(Type == MAVERIC_TYPE)
  {
	int workx = CAM_CENTER_X;
	int worky = 0;
	int cnt;
	cnt = 5;
	worky = cnt*(PIX_PER_SLEW_UD);
	while((fabs(worky) <= 50) && (cnt <= 15))
	{
	    GrDrawLine(GrBuffFor3D,workx - 4,CAM_CENTER_Y + worky,workx+4,CAM_CENTER_Y + worky,Triple.Red,Triple.Green,Triple.Blue,0);
		cnt += 5;
		worky = cnt*(PIX_PER_SLEW_UD);
	}
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsTargetInKeyHole(WeapCamType *Cam)
{
  float Dist;
  int ScrX,ScrY;

  ScrX = CAM_CENTER_X + -Cam->SlewLeftRight*(PIX_PER_SLEW_LR);  // JLM need to use target
  ScrY = CAM_CENTER_Y + -Cam->SlewUpDown*(PIX_PER_SLEW_UD);

  if( (ScrX > CAM_CENTER_X-20) && (ScrX < CAM_CENTER_X+20) )
	if( (ScrY > CAM_CENTER_Y+25) && (ScrY < CAM_CENTER_Y+35) )
	  return(TRUE);

  Dist = sqrt((float)(CAM_CENTER_X-ScrX)*(CAM_CENTER_X-ScrX) + (float)(CAM_CENTER_Y-ScrY)*(CAM_CENTER_Y-ScrY));
  if(Dist < 35)
   return(TRUE);

  return(FALSE);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessPointer(WeapCamType *Cam,int Type,int IsMavLaser = 0)
{

	if(Type == MAVERIC_TYPE)
	  Cam->TargetInKeyhole = IsTargetInKeyHole(Cam);
	else
	  Cam->TargetInKeyhole = TRUE; // no keyhole for gbu15

	if(!Cam->TargetInKeyhole)
	{
      if(!Cam->CrossTimer.IsActive())
	        Cam->CrossTimer.Set(15.0,GetTickCount());
	}

	if(Cam->CrossTimer.IsActive() )
	{
    if(Cam->CrossTimer.TimeElapsed(GetTickCount()) > 0.5)
	  {
			Cam->CrossTimer.Set(15.0,GetTickCount());
			Cam->DrawCross = !Cam->DrawCross;
	  }

	  if(Cam->TargetInKeyhole)
	  {
		   Cam->CrossTimer.Disable();
       Cam->DrawCross = TRUE;
	  }
	}

	if(Cam->DrawCross)
	  DrawSeekerPointer(Cam,Type,IsMavLaser);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessWeapCamSlew(int MpdNum,WeapCamSlewType *CamSlew,WeapCamType *Cam,int Type)
{
	// make sure slew is allowable by TDC
	if( GetTDCMode() != STORES_MODE )
	{
		return;
	}
	else                                   // I admit, this is a little kludgy, but it works....
	{
		if((!Av.Gbu.WeapFired) || (Av.Weapons.CurAGWeap))
		{
			if(Av.Weapons.CurAGWeap == NULL)
			{
				return;
			}
			else
			{
				if( (Av.Weapons.CurAGWeap->WeapPage != AGM_65_WEAP_PAGE) && (Av.Weapons.CurAGWeap->WeapPage != AGM_65F_WEAP_PAGE) && (Av.Weapons.CurAGWeap->WeapPage != WALLEYE_WEAP_PAGE) && (!Av.Gbu.WeapFired) )
						return;

				if( ((Av.Weapons.CurAGWeap->WeapPage == AGM_65_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == AGM_65F_WEAP_PAGE)) && (Cam != &Av.Mav) )
							return;
				else if( (Av.Weapons.CurAGWeap->WeapPage == WALLEYE_WEAP_PAGE) && (Cam != &Av.Gbu) )
							return;
			}
		}
	}

	int JoystickForSlew = GetVkStatus(CaptBarsVk);
	int JoyDir = -1;

	if(JoystickForSlew)
	{
	  JoyDir = GetJoystickMapDirection();
		if(JoyDir == -1)
			JoystickForSlew = FALSE;
	}

	int Left  = (JoyDir == LEFT);
	int Right = (JoyDir == RIGHT);
	int Up    = (JoyDir == TOP);
	int Down  = (JoyDir == BOTTOM);

	// process slew
  if(GetVkStatus(TdcUpVk) || (JoystickForSlew && Up) )
  {
    if( ((!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_UP))  && (!Cam->TrackOn) )
	  SetupWeapCamSlew(SLEW_UP, MpdNum, Type);
  }

  if(GetVkStatus(TdcDownVk) || (JoystickForSlew && Down) )
  {
    if( ((!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_DOWN)) && (!Cam->TrackOn) )
	  SetupWeapCamSlew(SLEW_DOWN, MpdNum, Type);
  }

  if(GetVkStatus(TdcLeftVk) || (JoystickForSlew && Left) )
  {
     if( ((!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_LEFT)) && (!Cam->TrackOn) )
	   SetupWeapCamSlew(SLEW_LEFT, MpdNum, Type);
  }

  if(GetVkStatus(TdcRightVk) || (JoystickForSlew && Right) )
  {
     if( ((!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_RIGHT)) && (!Cam->TrackOn) )
	   SetupWeapCamSlew(SLEW_RIGHT, MpdNum, Type);
  }

  if(CamSlew->Slewing)
  {
			if(CamSlew->MpdNum == MpdNum)
			{
	  		float SlewAmount;

	  		if(Cam->FOVToUse == WFOV)
		  		SlewAmount = 4.0;
	  		else if(Cam->FOVToUse == NFOV)
          		SlewAmount = 0.75;
	  		else if(Cam->FOVToUse == ENFOV)
        		SlewAmount = 0.15;

	  		CamSlew->SlewOffset = CamSlew->SlewTimer.TimeElapsed(GetTickCount())*SlewAmount;

      	switch(CamSlew->SlewDirection)
 	  		{
	    		case SLEW_UP   :  Cam->SlewUpDown    += CamSlew->SlewOffset;
			                   		if(Cam->SlewUpDown > 28.0)	Cam->SlewUpDown = 28.0;
			                   		break;
        	case SLEW_DOWN :  Cam->SlewUpDown    -= CamSlew->SlewOffset;
				                 		if(Cam->SlewUpDown < -52.0) Cam->SlewUpDown = -52.0;
			                   		break;
				  case SLEW_LEFT :  Cam->SlewLeftRight += CamSlew->SlewOffset;
					               		if(Cam->SlewLeftRight > 40.0) Cam->SlewLeftRight = 40.0;
			                   		break;
				  case SLEW_RIGHT:  Cam->SlewLeftRight -= CamSlew->SlewOffset;
						             		if(Cam->SlewLeftRight < -40.0) Cam->SlewLeftRight = -40.0;
			                   		break;
	  		}

	  		int KeypressSlew = ( (GetVkStatus(TdcUpVk)) || GetVkStatus(TdcDownVk)) ||
		                   		 (GetVkStatus(TdcLeftVk)) || (GetVkStatus(TdcRightVk) || (JoystickForSlew) );

	  		if( (GetMouseLB() == 0) && (!KeypressSlew) )
	    		CamSlew->Slewing = FALSE;
			}
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetPerspectBoundForObjects(int Type, WeapCamType *Cam, FPointDouble TargetLoc, FMatrix &TargetMat, float *MinX, float *MaxX, float *MinY, float *MaxY,int UseFlir = 0)
{
	float ScrX,ScrY;

	float TempMinX = 10000000;
	float TempMinY = 1000000;
	float TempMaxX = -1000000;
	float TempMaxY = -10000000;
	BOOL in_camera = FALSE;

  FPoint PlaneBound[8];

	FPoint LL,UR;

	if(Type == FLIR_CAM_PLANE_TARGET)
	{
		PlaneParams *P = (UseFlir) ? (PlaneParams *)Av.Flir.Target : (PlaneParams *)Cam->Target;

		LL.X = P->Type->ShadowULXOff;
		LL.Z = P->Type->ShadowULZOff;
		LL.Y = -P->Type->GearDownHeight;

		UR.X = P->Type->ShadowLRXOff;
		UR.Z = P->Type->ShadowLRZOff;
		UR.Y = P->Type->TailHeight;
	}
	else if(Type == FLIR_CAM_VEH_TARGET)
	{
		MovingVehicleParams *V = (UseFlir) ? (MovingVehicleParams *)Av.Flir.Target : (MovingVehicleParams *)Cam->Target;

		LL.X = V->Type->LowUL.X;
		LL.Z = V->Type->LowUL.Z;
		LL.Y = V->Type->LowUL.Y;

		UR.X = V->Type->HighLR.X;
		UR.Z = V->Type->HighLR.Z;
		UR.Y = V->Type->HighLR.Y;
	}

	PlaneBound[0].X = LL.X;		PlaneBound[0].Y = LL.Y;  		PlaneBound[0].Z = LL.Z;
	PlaneBound[1].X = LL.X;		PlaneBound[1].Y = UR.Y;  		PlaneBound[1].Z = LL.Z;
	PlaneBound[2].X = UR.X;		PlaneBound[2].Y = UR.Y;	 		PlaneBound[2].Z = LL.Z;
	PlaneBound[3].X = UR.X;	    PlaneBound[3].Y = LL.Y;	 		PlaneBound[3].Z = LL.Z;

	PlaneBound[4].X = UR.X;		PlaneBound[4].Y = UR.Y;  		PlaneBound[4].Z = UR.Z;
	PlaneBound[5].X = UR.X;		PlaneBound[5].Y = LL.Y;  		PlaneBound[5].Z = UR.Z;
	PlaneBound[6].X = LL.X;		PlaneBound[6].Y = LL.Y;	 		PlaneBound[6].Z = UR.Z;
	PlaneBound[7].X = LL.X;	    PlaneBound[7].Y = UR.Y;	 		PlaneBound[7].Z = UR.Z;

	for(int i=0; i<8; i++)
	{
	      FPointDouble Rot;
		  FPointDouble FinalPos;

		  Rot = PlaneBound[i];

		  Rot.RotateInto(Rot,TargetMat);
		  Rot += TargetLoc;

	 	if( CameraPerspect(Cam,Rot,&ScrX,&ScrY,UseFlir) )
	 	{
			in_camera = TRUE;
			if(ScrX < TempMinX)
					TempMinX = ScrX;
			if(ScrX > TempMaxX)
					TempMaxX = ScrX;
			if(ScrY < TempMinY)
					TempMinY = ScrY;
			if(ScrY > TempMaxY)
					TempMaxY = ScrY;
	 	}
	}

	if (in_camera)
	{
		if ((TempMinX <= 140) && (TempMaxX >= 0) && (TempMinY <= 140) && (TempMaxY >= 0))
		{

			TempMinX = __max(TempMinX,0);
			TempMinX = __min(TempMinX,140);

			TempMaxX = __max(TempMaxX,0);
			TempMaxX = __min(TempMaxX,140);

			TempMinY = __max(TempMinY,0);
			TempMinY = __min(TempMinY,140);

			TempMaxY = __max(TempMaxY,0);
			TempMaxY = __min(TempMaxY,140);

			*MinX = TempMinX;
			*MaxX = TempMaxX;
			*MinY = TempMinY;
			*MaxY = TempMaxY;

			return TRUE;

		}
		else
			return FALSE;
	}
	else
		return FALSE;


}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetCrossHairsSize(int Type, void *Obj,FPointDouble Loc)
{
  BasicInstance *Instance;
  ObjectTypeInfo *ObjectType;
  FPointDouble Point;
  float MinX,MaxX,MinY,MaxY,Max;

	if(Type == FLIR_CAM_GROUND_TARGET)
	{
	  Instance   = ((RegisteredObject *)Obj)->Instance;
      ObjectType = GetObjectTypeInfoFromInstance(Instance);

	    if(ObjectType == NULL)
				Av.Mav.CHRadius = 5;
			else
			{
					FPointDouble Target;
					float ScrX,ScrY;
					MinX = 140;
					MaxX = 0;
					MinY = 140;
					MaxY = 0;
					Max = 5;

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentRight;
					Target.Y += Av.Mav.CHMavTarget->ExtentHeight;
					Target.Z += Av.Mav.CHMavTarget->ExtentTop;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentLeft;
					Target.Y += Av.Mav.CHMavTarget->ExtentHeight;
					Target.Z += Av.Mav.CHMavTarget->ExtentTop;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentLeft;
					Target.Y += Av.Mav.CHMavTarget->ExtentHeight;
					Target.Z += Av.Mav.CHMavTarget->ExtentBottom;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentRight;
					Target.Y += Av.Mav.CHMavTarget->ExtentHeight;
					Target.Z += Av.Mav.CHMavTarget->ExtentBottom;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentRight;
					Target.Z += Av.Mav.CHMavTarget->ExtentTop;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentLeft;
					Target.Z += Av.Mav.CHMavTarget->ExtentTop;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentLeft;
					Target.Z += Av.Mav.CHMavTarget->ExtentBottom;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Target = Av.Mav.CHMavTarget->Instance->Position;

					Target.X += Av.Mav.CHMavTarget->ExtentRight;
					Target.Z += Av.Mav.CHMavTarget->ExtentBottom;

					if( CameraPerspect(&Av.Mav,Target,&ScrX,&ScrY) )
					{
						if(ScrX < MinX)
							 MinX = ScrX;
						if(ScrX > MaxX)
							 MaxX = ScrX;
						if(ScrY < MinY)
							 MinY = ScrY;
						if(ScrY > MaxY)
							 MaxY = ScrY;
					}

					Av.Mav.CHRadiusLeft  = fabs(70 - MinX);
					Av.Mav.CHRadiusRight = fabs(70 - MaxX);
					Av.Mav.CHRadiusUp    = fabs(70 - MinY);
					Av.Mav.CHRadiusDown  = fabs(70 - MaxY);

					Av.Mav.CHRadius = Max;
				}
	}
	else if(Type == FLIR_CAM_PLANE_TARGET)
	{
    PlaneParams *P = ( (PlaneParams *)Av.Mav.Target);

    int InScreen = GetPerspectBoundForObjects(FLIR_CAM_PLANE_TARGET,&Av.Mav,P->WorldPosition,P->Attitude,&MinX,&MaxX,&MinY,&MaxY);

		if(!InScreen)
			Av.Mav.CHRadius = 5;
		else
		{
			Av.Mav.CHRadiusLeft  = __max(70 - MinX,0);
			Av.Mav.CHRadiusRight = __max(MaxX - 70,0);
			Av.Mav.CHRadiusUp    = __max(70 - MinY,0);
			Av.Mav.CHRadiusDown  = __max(MaxY-70,0);
		}
	}
	else if(Type == FLIR_CAM_VEH_TARGET)
	{
    MovingVehicleParams *V = ( (MovingVehicleParams *)Av.Mav.Target);

    int InScreen = GetPerspectBoundForObjects(FLIR_CAM_VEH_TARGET,&Av.Mav,V->WorldPosition,V->Attitude,&MinX,&MaxX,&MinY,&MaxY);

		if(!InScreen)
			Av.Mav.CHRadius = 5;
		else
		{
			Av.Mav.CHRadiusLeft  = __max(70 - MinX,0);
			Av.Mav.CHRadiusRight = __max(MaxX - 70,0);
			Av.Mav.CHRadiusUp    = __max(70 - MinY,0);
			Av.Mav.CHRadiusDown  = __max(MaxY-70,0);
		}
	}
	else if(Type == FLIR_CAM_LOC_TARGET)
	{
		Av.Mav.CHRadius = 50;
		Av.Mav.CHRadiusLeft  = 50;
		Av.Mav.CHRadiusRight = 50;
		Av.Mav.CHRadiusUp    = 50;
		Av.Mav.CHRadiusDown  = 50;
	}

	Av.Mav.CHTimer.Set(2.0,GetTickCount());
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimCrossHairs()
{
	if( !Av.Mav.CHTimer.IsActive() ) return;

	int AllFinished = TRUE;

	float *RadPtr =&Av.Mav.CHRadiusLeft;
	float *PosPtr =&Av.Mav.CurCHPosLeft;

	int Index = 4;
	while(Index-- > 0)
	{
			int Dir = (*RadPtr - *PosPtr > 0) ? 0.5 : -0.5;

			(*PosPtr) = (*PosPtr) + (70.0*Av.Mav.CHTimer.TimeElapsed(GetTickCount())*Dir); // 30 pixels per 2 seconds

			if(Dir == 1)
			{
				if(*PosPtr > *RadPtr)
				{
					*PosPtr = *RadPtr;
				}
				else
					AllFinished = FALSE;
			}
			else
			{
				if(*PosPtr < *RadPtr)
				{
					*PosPtr = *RadPtr;
				}
				else
					AllFinished = FALSE;
			}
			PosPtr++;
			RadPtr++;
	}

	if(AllFinished)
		 Av.Mav.CHTimer.Disable();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawCrossHairs()
{
	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

  GrDrawLine(GrBuffFor3D,10,CAM_CENTER_Y,CAM_CENTER_X - Av.Mav.CurCHPosLeft,CAM_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,130,CAM_CENTER_Y,CAM_CENTER_X + Av.Mav.CurCHPosRight,CAM_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,CAM_CENTER_X,10,CAM_CENTER_X,CAM_CENTER_Y - Av.Mav.CurCHPosUp,Color.Red,Color.Green,Color.Blue,0);
  GrDrawLine(GrBuffFor3D,CAM_CENTER_X,130,CAM_CENTER_X,CAM_CENTER_Y + Av.Mav.CurCHPosDown,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavTogglePolarity()
{
	Av.Mav.Polarity++;
	if(Av.Mav.Polarity > BHOT)
		Av.Mav.Polarity = WHOT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavToggleFC()
{
	Av.Mav.ForceCorrelateOn = !Av.Mav.ForceCorrelateOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavToggleFOV()
{
  Av.Mav.FOVToUse++;
	if(Av.Mav.FOVToUse > NFOV) Av.Mav.FOVToUse = WFOV;

	if(Av.Mav.FOVToUse == WFOV)
	  Av.Mav.Magnify = (float)5.8*(float)DEGREE;
	else
	  Av.Mav.Magnify = (float)1.67*(float)DEGREE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavStep()
{
	AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavDesignate()
{
	Av.Mav.TrackOn = TRUE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MavUnDesignate()
{
	Av.Mav.TrackOn = FALSE;
	Av.Mav.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGM65WeapPage(int MpdNum)
{
	DBWeaponType *pDBWeapon;
	int ForceCorrelate = ( (GetSelectedAGWeaponId() == GetWeapId(AGM65G_ID)) && (Av.Mav.ForceCorrelateOn) );

	SimCrossHairs();

	ProcessWeapCamSlew(MpdNum,&MavWeapCamSlew,&Av.Mav,MAVERIC_TYPE);

	GetCamPos(&Av.Mav);

	if(Av.Mav.AquireMode == MSL_AUTO)
	{
		PointWeapCamToCurrentAGTarget(&Av.Mav);
	}

	if(Av.Mav.TrackOn)
	{
		if(Av.Mav.TargetObtained)
		{
			float Heading,Pitch;
			FPointDouble Target;

			if(Av.Mav.TrackPosObtained)
				Target = Av.Mav.TrackPos;
			else
				GetWeapCamTargetPosition(&Av.Mav,&Target);

			FPointDouble EndPoint;
			EndPoint.MakeVectorToFrom(Target,Av.Mav.CamPos);

			FMatrix TempMatrix(PlayerPlane->Attitude);

			TempMatrix.Transpose();

			EndPoint *= TempMatrix;

			Heading = atan2(-EndPoint.X,-EndPoint.Z);
			Pitch   = atan2(EndPoint.Y,sqrt(EndPoint.Z*EndPoint.Z + EndPoint.X*EndPoint.X));

			Heading = RadToDeg(Heading);
			Pitch   = RadToDeg(Pitch);

			if(!IsCamWithinSlewBounds(&Av.Mav,Heading,Pitch,NULL,NULL))
			{
				Av.Mav.SlewLeftRight = Heading;
				Av.Mav.SlewUpDown    = Pitch;
			}
			else
			{
				Av.Mav.TargetObtained = FALSE;
				Av.Mav.TrackOn        = FALSE;
			}

			DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Mav.SlewLeftRight,Av.Mav.SlewUpDown,FALSE,TRUE,(Av.Mav.Polarity == WHOT) ? TRUE : FALSE,Av.Mav.CamPos,FALSE,NULL,Av.Mav.Magnify,&Av.Mav);
		}
		else
		{
			FPointDouble ProjPoint;
			FPointDouble Intersect;
			FPointDouble TgtLoc;
			FPointDouble Target;
			int PlaneIndex;
			int LosVal;
			FPointDouble NullLoc;

			NullLoc.SetValues(0.0,0.0,0.0);

			DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Mav.SlewLeftRight,Av.Mav.SlewUpDown,FALSE,TRUE,(Av.Mav.Polarity == WHOT) ? TRUE : FALSE,Av.Mav.CamPos,FALSE,NULL,Av.Mav.Magnify,&Av.Mav);

			ProjPoint.SetValues(-CAM_PROJ_DIST*Av.Mav.Matrix.m_Data.RC.R0C2,
					-CAM_PROJ_DIST*Av.Mav.Matrix.m_Data.RC.R1C2,
					-CAM_PROJ_DIST*Av.Mav.Matrix.m_Data.RC.R2C2);
			ProjPoint += Av.Mav.CamPos;

			LosVal = LOS(&Av.Mav.CamPos,&ProjPoint,&Intersect, HI_RES_LOS, TRUE);

			if(LosVal)
				Intersect.X = -1;  // nullify location if no LOS

			Av.Mav.TrackPosObtained = !LosVal;
			Av.Mav.TrackPos = Intersect;
			Av.Mav.CHMavTarget = NULL;

			int PlaneFound,VehFound,ObjFound;
			FPointDouble ObjTarget,PlaneTarget,VehTarget;
			float ObjDist,PlaneDist,VehDist;
			RegisteredObject *GrndTrgt;
			MovingVehicleParams *VehTrgt;

			ObjFound   = GetFlirWeapCamTarget(&Av.Mav.CamPos,&ProjPoint,&GrndTrgt,&ObjDist,&TgtLoc);

			PlaneFound = GrabPlaneInFlirWeapCam(&Av.Mav.CamPos,&ProjPoint,&PlaneIndex,&PlaneDist);

			if(LosVal)
				VehFound   = GetFlirWeapCamVehicle(Av.Mav.CamPos,Intersect,&VehTrgt,&VehDist);
			else
				VehFound = FALSE;

			if(!ObjFound && !PlaneFound && !VehFound && !Av.Mav.TrackPosObtained)
			{
				Av.Mav.TrackOn = FALSE;
			}
			else
			{
				if( ObjFound && (!PlaneFound || (ObjDist < PlaneDist)) && (!VehFound || (ObjDist < VehDist)) && !ForceCorrelate )      // force correlate forces ground intersect to be used
				{
					Av.Mav.TargetObtained = TRUE;
					Av.Mav.TargetType = FLIR_CAM_GROUND_TARGET;  //  FLIR_CAM_LOC_TARGET;
					Av.Mav.TargetLoc = TgtLoc;
					Av.Mav.CHMavTarget = GrndTrgt;
					Av.Mav.Target = GrndTrgt->Instance;
					SetCrossHairsSize(FLIR_CAM_GROUND_TARGET,GrndTrgt,NullLoc);
				}
				else if(PlaneFound && (!ObjFound || (PlaneDist < ObjDist)) && (!VehFound || (PlaneDist < VehDist)) && !ForceCorrelate )
				{
					Av.Mav.TargetObtained = TRUE;
					Av.Mav.TargetType = FLIR_CAM_PLANE_TARGET;
					Av.Mav.Target = (PlaneParams *)&Planes[PlaneIndex];
					SetCrossHairsSize(FLIR_CAM_PLANE_TARGET,Av.Mav.Target,NullLoc);
				}
				else if( VehFound && (!ObjFound || (VehDist < ObjDist)) && (!PlaneFound || (VehDist < PlaneDist)) && !ForceCorrelate )
				{
					Av.Mav.TargetObtained = TRUE;
					Av.Mav.TargetType = FLIR_CAM_VEH_TARGET;
					Av.Mav.Target = (MovingVehicleParams *)VehTrgt;
					SetCrossHairsSize(FLIR_CAM_VEH_TARGET,Av.Mav.Target,NullLoc);
				}
				else if(!LosVal)
				{
					int CloseTargetType;

					// since mavs are tracking the ground, get closest object within 400 ft and track it
					if( !ForceCorrelate && GetCloseVehicleOrObject(&Intersect,&GrndTrgt,&VehTrgt,&CloseTargetType) )
					{
						Av.Mav.TargetObtained = TRUE;
						if(CloseTargetType == FLIR_CAM_GROUND_TARGET)
						{
							Av.Mav.TargetType = FLIR_CAM_GROUND_TARGET;
							Av.Mav.Target     = GrndTrgt->Instance;
							Av.Mav.CHMavTarget = GrndTrgt;
							Av.Mav.Target = GrndTrgt->Instance;
							SetCrossHairsSize(FLIR_CAM_GROUND_TARGET,GrndTrgt,NullLoc);
						}
						else
						{
							Av.Mav.TargetType = FLIR_CAM_VEH_TARGET;
							Av.Mav.Target    = (MovingVehicleParams *)VehTrgt;
							SetCrossHairsSize(FLIR_CAM_VEH_TARGET,Av.Mav.Target,NullLoc);
						}
					}
					else if(!ForceCorrelate)
					{
						Av.Mav.TrackOn = FALSE;
						Av.Mav.TargetObtained = FALSE;
					}
					else
					{
						Av.Mav.TargetObtained = TRUE;
						Av.Mav.TargetType     = FLIR_CAM_LOC_TARGET;
						Av.Mav.TargetLoc      = Intersect;
						SetCrossHairsSize(FLIR_CAM_LOC_TARGET,NULL,Av.Mav.TargetLoc);
					}

					if(Av.Mav.TargetObtained && ( (Av.Mav.TargetType == FLIR_CAM_PLANE_TARGET) || (Av.Mav.TargetType == FLIR_CAM_VEH_TARGET)  || (Av.Mav.TargetType == FLIR_CAM_GROUND_TARGET)) )
					{
						Av.Mav.TrackPosObtained = FALSE;
					}
				}  // !los
			}  // object found
		} // target obtained
	} // track on
	else if(ForceCorrelate)
	{
		Av.Mav.CHRadius = 50;
		Av.Mav.CHRadiusLeft  = 50;
		Av.Mav.CHRadiusRight = 50;
		Av.Mav.CHRadiusUp    = 50;
		Av.Mav.CHRadiusDown  = 50;
		if((Av.Mav.CurCHPosLeft < 50) || (Av.Mav.CurCHPosRight < 50) || (Av.Mav.CurCHPosUp < 50) || (Av.Mav.CurCHPosDown < 50))
		{
			Av.Mav.CHTimer.Set(2.0,GetTickCount());
		}
	}
	else
	{
		Av.Mav.CHRadius = 5;
		Av.Mav.CHRadiusLeft  = 5;
		Av.Mav.CHRadiusRight = 5;
		Av.Mav.CHRadiusUp    = 5;
		Av.Mav.CHRadiusDown  = 5;
		if((Av.Mav.CurCHPosLeft > 5) || (Av.Mav.CurCHPosRight > 5) || (Av.Mav.CurCHPosUp > 5) || (Av.Mav.CurCHPosDown > 5))
		{
			Av.Mav.CHTimer.Set(2.0,GetTickCount());
		}
	}

	if(!Av.Mav.TrackOn)
	{
		DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Mav.SlewLeftRight,Av.Mav.SlewUpDown,FALSE,TRUE,(Av.Mav.Polarity == WHOT) ? TRUE : FALSE,Av.Mav.CamPos,FALSE,NULL,Av.Mav.Magnify,&Av.Mav);
	}

	//DrawAltAndElev(MpdNum, XOfs,YOfs, &Av.Mav.;

	//if(pDBWeapon != NULL)
	//{
		// float MinRange,MaxRange;
		//WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange, &MaxRange);
		//DrawMavGbuRangeMarkers(MpdNum,&Av.Mav.XOfs,YOfs,MinRange,MaxRange);
	//}

	// jlm for testing
	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	if(ForceCorrelate)
	{
		GrDrawLine(GrBuffFor3D,CAM_CENTER_X,CAM_CENTER_Y - 5,CAM_CENTER_X,CAM_CENTER_Y + 5,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,CAM_CENTER_X-5,CAM_CENTER_Y,CAM_CENTER_X+5,CAM_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	}

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
		DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
		DrawHarmMode();
	}

	DrawVerticalWordAtButton(MpdNum,10,"FOV",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);

	if(GetSelectedAGWeaponId() == GetWeapId(AGM65G_ID))
		DrawVerticalWordAtButton(MpdNum,13,"F/C",Av.Mav.ForceCorrelateOn,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

//	DrawWordAtMpdButton(MpdNum,5,(Av.Mav.Polarity == WHOT) ? "WHITE" : "BLACK",FALSE,AV_GREEN,1.0);
	if(Av.Mav.Polarity == WHOT)
	{
	 	sprintf(TmpStr,"WHITE");
	}
	else
	{
	 	sprintf(TmpStr,"BLACK");
	}

	int TextScrX;
	int TextScrY;
	GetMPDButtonXY(MpdNum,5,TmpStr,TextScrX,TextScrY);

	DrawTextAtLoc(TextScrX,126,TmpStr,AV_GREEN,1.0f);
	DrawWordAtMpdButton(MpdNum,5,"HOT",FALSE,AV_GREEN,1.0);

	// STATION NUMBER
	sprintf(TmpStr,"STA %d",Av.Weapons.CurAGWeap->Station + 1);
	DrawTextAtLoc(TEWS_CENTER_X - 50,TEWS_CENTER_Y - 50,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(103,14,(Av.Mav.AquireMode == MSL_MANUAL) ? "UNCAGED" : "CAGED",AV_GREEN,1.0);

	// IN RANGE
	if(PlayerPlane->AGDesignate.X != -1)
	{
		float MinRange,MaxRange;
		FPointDouble TargetLoc;

		pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange, &MaxRange);

//  	GetWeapCamTargetPosition(&Av.Mav,&TargetLoc);

		float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);
		DistToTarget *= WUTOFT;

		int Time = (DistToTarget - MaxRange)/PlayerPlane->IfHorzVelocity;

		if(Time > 99)
			Time = 99;
		if(Time < 0)
			Time = 0;

		sprintf(TmpStr,"%d %s", Time, "TTMR" );

		if( (DistToTarget < MaxRange) && (DistToTarget > MinRange) )
			RightJustifyTextAtLocation(125,21,"IN RANGE",Color.Red,Color.Green,Color.Blue);
		else
			RightJustifyTextAtLocation(125,21,TmpStr,Color.Red,Color.Green,Color.Blue);
	}

	DrawCrossHairs();

	DrawWeaponSelections(MpdNum);

	if( (Av.Mav.TrackOn) && (Av.Mav.TargetObtained) )
	{
		if(Av.Mav.CHMavTarget != NULL)
		{
			if(!Av.Mav.CHReAdjustTimer.IsActive())
				Av.Mav.CHReAdjustTimer.Set(2.0,GetTickCount());
			else
			{
				if(Av.Mav.CHReAdjustTimer.TimeUp(GetTickCount()))
				{
					FPointDouble NullLoc;
					NullLoc.SetValues(0.0,0.0,0.0);

					Av.Mav.CHReAdjustTimer.Set(2.0,GetTickCount());
					SetCrossHairsSize(Av.Mav.TargetType,Av.Mav.CHMavTarget,NullLoc);
				}
			}
		}
	}
	else
	{
		Av.Mav.CHMavTarget = NULL;
	}

	ProcessPointer(&Av.Mav,MAVERIC_TYPE);

	if(!ForceCorrelate)
		DrawMavTicks(&Av.Mav,MAVERIC_TYPE);

	if(Av.Mav.FOVToUse == WFOV)
	{
		GrDrawLine(GrBuffFor3D,41,37,36,37,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,36,37,36,42,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,97,37,102,37,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,102,37,102,42,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,97,103,102,103,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,102,103,102,98,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,41,103,36,103,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,36,103,36,98,Color.Red,Color.Green,Color.Blue,0);
	}

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 5:
				MavTogglePolarity();
				break;
			case  7:
				Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				break;
			case  9:
				ToggleHarmOverride();
				break;
			case 10:
				MavToggleFOV();
				break;
			case 11:
				MavStep();
				break;
			case 13:
				if(GetSelectedAGWeaponId() == GetWeapId(AGM65G_ID))
					MavToggleFC();
				break;
			case 14:
				MavToggleGuns();
				break;
			case 19:
				AGLeftSelect();
			    break;
			case 18:
				AGFrontLeftSelect();
				break;
			case 17:
				AGCenterSelect();
				break;
			case 16:
				AGFrontRightSelect();
				break;
			case 15:
				AGRightSelect();
				break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void PointWeapCamAtLoc(WeapCamType *Cam,FPointDouble Loc)
{
		FPointDouble EndPoint;
		EndPoint.MakeVectorToFrom(Loc,Cam->CamPos);

		FMatrix TempMatrix(PlayerPlane->Attitude);

		TempMatrix.Transpose();

		EndPoint *= TempMatrix;

		float Heading = atan2(-EndPoint.X,-EndPoint.Z);
		float Pitch   = atan2(EndPoint.Y,sqrt(EndPoint.Z*EndPoint.Z + EndPoint.X*EndPoint.X));

		Heading = RadToDeg(Heading);
		Pitch   = RadToDeg(Pitch);

	  float LeftRight = Heading;
    float UpDown    = Pitch;

    if(!IsCamWithinSlewBounds(Cam,LeftRight,UpDown,NULL,NULL))
	  {
        Cam->SlewLeftRight    = LeftRight;
        Cam->SlewUpDown       = UpDown;
		    Cam->WithinSlewLimits = TRUE;
 	  }
	  else
	  {
		  Cam->WithinSlewLimits = FALSE;
	  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AGM65EWeapPage(int MpdNum)
{
	FPointDouble fpd_position;
	SimCrossHairs();

  ProcessWeapCamSlew(MpdNum,&MavWeapCamSlew,&Av.Mav,MAVERIC_TYPE);

	GetCamPos(&Av.Mav);

//	if( (Av.Flir.Mode == FLIR_LOCK_MODE) && (Av.Flir.LaserState == FLIR_LSR_FIRING) )
	if( (Av.Flir.TrackOn) && (Av.Flir.LaserState == FLIR_LSR_FIRING) )
	{

    PointWeapCamAtLoc(&Av.Mav,LaserLoc);

		if(Av.Mav.WithinSlewLimits)
		{
			Av.Mav.TrackOn = TRUE;
			Av.Mav.TrackPosObtained = TRUE;
			Av.Mav.TargetObtained = TRUE;
			Av.Mav.TargetType = FLIR_CAM_LOC_TARGET;
			Av.Mav.TargetLoc = LaserLoc;
			Av.Mav.TrackPos = LaserLoc;
		}
		else
		{
			Av.Mav.TrackOn = FALSE;
			Av.Mav.TrackPosObtained = FALSE;
			Av.Mav.TargetObtained = FALSE;
			Av.Mav.SlewLeftRight = 0;
			Av.Mav.SlewUpDown    = 0;
		}
	}
	else
	{
		int foundother = 0;
		if((Av.Mav.AquireMode == MSL_MANUAL) && (AVFoundOtherLaser(PlayerPlane, &fpd_position)))
		{
			PointWeapCamAtLoc(&Av.Mav,fpd_position);

			if(Av.Mav.WithinSlewLimits)
			{
				Av.Mav.TrackOn = TRUE;
				Av.Mav.TrackPosObtained = TRUE;
				Av.Mav.TargetObtained = TRUE;
				Av.Mav.TargetType = FLIR_CAM_LOC_TARGET;
				Av.Mav.TargetLoc = fpd_position;
				Av.Mav.TrackPos = fpd_position;
				foundother = 1;
			}
			else
			{
				Av.Mav.TrackOn = FALSE;
				Av.Mav.TrackPosObtained = FALSE;
				Av.Mav.TargetObtained = FALSE;
				Av.Mav.SlewLeftRight = 0;
				Av.Mav.SlewUpDown    = 0;
			}
		}
		else
		{
			Av.Mav.TrackOn = FALSE;
			Av.Mav.TrackPosObtained = FALSE;
			Av.Mav.TargetObtained = FALSE;
		}

		if(!foundother)
		{
			if(Av.Flir.TrackOn)
    		PointWeapCamAtLoc(&Av.Mav,Av.Flir.TrackPos);

			if(Av.Mav.WithinSlewLimits && Av.Flir.TrackOn)
			{
				Av.Mav.TrackOn = TRUE;
				Av.Mav.TrackPosObtained = FALSE;
				Av.Mav.TargetObtained = FALSE;
			}
			else
			{
				Av.Mav.SlewLeftRight = 0;
				Av.Mav.SlewUpDown    = 0;
			}
		}
 	}

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	// STATION NUMBER
	sprintf(TmpStr,"STA %d",Av.Weapons.CurAGWeap->Station + 1);
	DrawTextAtLoc(TEWS_CENTER_X - 50,TEWS_CENTER_Y - 50,TmpStr,AV_GREEN,1.0);

	int tempval = 0;
	int ttm = 0;

	if(TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&tempval,&ttm))
	{
		RightJustifyTextAtLocation(125,21,"IN RANGE",Color.Red,Color.Green,Color.Blue);
	}

	DrawTextAtLoc(103,14,(Av.Mav.AquireMode == MSL_MANUAL) ? "UNCAGED" : "CAGED",AV_GREEN,1.0);

	// show if locked
	if(Av.Mav.TrackOn && Av.Mav.TrackPosObtained && Av.Mav.TargetObtained)
	{
			AvRGB Triple;
  		GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);
  		GrDrawString(GrBuffFor3D, LgHUDFont, TEWS_CENTER_X - 20,TEWS_CENTER_Y - 45,"LOCKED",Triple.Red,Triple.Green,Triple.Blue,0);
	}

	GrDrawPolyCircle(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y,35,21,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X-4,TEWS_CENTER_Y-10,TEWS_CENTER_X+4,TEWS_CENTER_Y-10,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X-4,TEWS_CENTER_Y-20,TEWS_CENTER_X+4,TEWS_CENTER_Y-20,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X-4,TEWS_CENTER_Y+10,TEWS_CENTER_X+4,TEWS_CENTER_Y+10,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X-4,TEWS_CENTER_Y+20,TEWS_CENTER_X+4,TEWS_CENTER_Y+20,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y-3,TEWS_CENTER_X,TEWS_CENTER_Y-3-10,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y-18,TEWS_CENTER_X,TEWS_CENTER_Y-18-12,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y+3,TEWS_CENTER_X,TEWS_CENTER_Y+3+10,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X,TEWS_CENTER_Y+18,TEWS_CENTER_X,TEWS_CENTER_Y+18+12,Color.Red,Color.Green,Color.Blue,0);

	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X-3,TEWS_CENTER_Y,TEWS_CENTER_X-3-10,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X-18,TEWS_CENTER_Y,TEWS_CENTER_X-18-12,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X+3,TEWS_CENTER_Y,TEWS_CENTER_X+3+10,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X+18,TEWS_CENTER_Y,TEWS_CENTER_X+18+12,TEWS_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);




	ProcessPointer(&Av.Mav,MAVERIC_TYPE,TRUE);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 11: MavStep();
							 break;
			case 14: MavToggleGuns();
							 break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
// WALLEYE WEAP PAGE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void GbuStep()
{
	AGStationReject();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GbuToggleGuns()
{
	WeapStores.GunsOn = !WeapStores.GunsOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GbuDesignate()
{
	Av.Gbu.TrackOn = TRUE;
	Av.Gbu.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GbuUnDesignate()
{
	if(Av.Gbu.AquireMode == MSL_AUTO) return;

	Av.Gbu.TrackOn = FALSE;
	Av.Gbu.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void WalleyeWeapPage(int MpdNum)
{
  DBWeaponType *pDBWeapon;
  FPointDouble GbuPos;
  int GbuOnMissile = FALSE;

  ProcessWeapCamSlew(MpdNum,&GbuWeapCamSlew,&Av.Gbu,GBU15_TYPE);

	GetCamPos(&Av.Gbu);

	GbuPos = Av.Gbu.CamPos;

	if(Av.Gbu.WeapFired)
	{
		if(!Av.Weapons.HasDataLink)
		{
			ResetWalleye();
			return;
		}

	  if(pCurrentGBU != NULL)
	  {
	    Av.Gbu.Weap = pCurrentGBU; // must do this because it takes time to create weapon
		  Av.Gbu.AquireMode = MSL_MANUAL;
	  }

    if(pCurrentGBU)
	  {
		  FPoint RelPos;

		  RelPos.SetValues(0,0,-2.5);

	    GbuPos = pCurrentGBU->Pos;
		  RelPos.RotateInto(RelPos,pCurrentGBU->Attitude);
	    RelPos += pCurrentGBU->Pos;
		  GbuOnMissile = TRUE;
	  }
 	}

	if(Av.Gbu.Weap != pCurrentGBU)
	{
	  if(Av.Gbu.Weap != NULL)
        ResetWalleye();
  }

	if(Av.Gbu.TrackOn && pCurrentGBU)
	{
  		int GroundTargetType;

   	switch(Av.Gbu.TargetType)
		{
		  case FLIR_CAM_GROUND_TARGET: GroundTargetType = GROUNDOBJECT;
				                           break;
			case FLIR_CAM_PLANE_TARGET:	 GroundTargetType = AIRCRAFT;
				                           break;
      case FLIR_CAM_VEH_TARGET:    GroundTargetType = MOVINGVEHICLE;
					                         break;
		  case FLIR_CAM_LOC_TARGET:    Av.Gbu.Target = NULL;
                                   GroundTargetType = GROUNDOBJECT;
				                           break;
		}

  	if(GroundTargetType == GROUNDOBJECT)
		 	SetGroundTarget(pCurrentGBU,(BasicInstance *)Av.Gbu.Target,Av.Gbu.TargetLoc,GroundTargetType);
  	else if(GroundTargetType == AIRCRAFT)
		 	SetGroundTarget(pCurrentGBU,(PlaneParams *)Av.Gbu.Target,Av.Gbu.TargetLoc,GroundTargetType);
		else if(GroundTargetType == MOVINGVEHICLE)
		{
		  	SetGroundTarget(pCurrentGBU,(MovingVehicleParams *)Av.Gbu.Target,Av.Gbu.TargetLoc,GroundTargetType);
			MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)Av.Gbu.Target;
			if(Av.Gbu.TrackOn && Av.Gbu.TargetObtained && Av.Gbu.TrackPosObtained)
			{
				Av.Gbu.TrackPos = vehiclepnt->WorldPosition;
			}
		}
	}

	if(Av.Gbu.AquireMode == MSL_AUTO)
	{
	  PointWeapCamToCurrentAGTarget(&Av.Gbu);
	}

  if(Av.Gbu.TrackOn)
  {
	  if(Av.Gbu.TargetObtained)
	  {
	    float Heading,Pitch;
      FPointDouble Target;

		  if(Av.Gbu.TrackPosObtained)
	      Target = Av.Gbu.TrackPos;
      else
	      GetWeapCamTargetPosition(&Av.Gbu,&Target);

			if(Av.Gbu.Weap)
          CalcNewFlirCamHeadingAndPitch(GbuPos,&Heading,&Pitch,Target,Av.Gbu.Weap);
			else
			{
		  		FPointDouble EndPoint;
		  		EndPoint.MakeVectorToFrom(Target,GbuPos);

		  		FMatrix TempMatrix(PlayerPlane->Attitude);

		  		TempMatrix.Transpose();

		  		EndPoint *= TempMatrix;

					Heading = atan2(-EndPoint.X,-EndPoint.Z);
					Pitch   = atan2(EndPoint.Y,sqrt(EndPoint.Z*EndPoint.Z + EndPoint.X*EndPoint.X));

					Heading = RadToDeg(Heading);
					Pitch   = RadToDeg(Pitch);
			}

	    if(!IsCamWithinSlewBounds(&Av.Gbu,Heading,Pitch,NULL,NULL))
	    {
	      Av.Gbu.SlewLeftRight = Heading;
		   	Av.Gbu.SlewUpDown    = Pitch;
	    }
      else
	    {
        Av.Gbu.TargetObtained = FALSE;
        Av.Gbu.TrackOn        = FALSE;
	    }

		  DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Gbu.SlewLeftRight,Av.Gbu.SlewUpDown,FALSE,FALSE,FALSE,GbuPos,GbuOnMissile,Av.Gbu.Weap,Av.Gbu.Magnify,&Av.Gbu);
	  }
    else
	  {
		  FPointDouble ProjPoint;
      FPointDouble Intersect;
		  FPointDouble TgtLoc;
	    FPointDouble Target;
      int PlaneIndex;
		  int LosVal;

	  	DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Gbu.SlewLeftRight,Av.Gbu.SlewUpDown,FALSE,FALSE,FALSE,GbuPos,GbuOnMissile,Av.Gbu.Weap,Av.Gbu.Magnify,&Av.Gbu);

			if(GbuOnMissile)
			  CalcCameraViewVector(Av.Gbu.SlewLeftRight,Av.Gbu.SlewUpDown,&GbuPos,&ProjPoint,Av.Gbu.Weap->Heading);
			else
			{
      		ProjPoint.SetValues(-CAM_PROJ_DIST*Av.Gbu.Matrix.m_Data.RC.R0C2,
			                    		-CAM_PROJ_DIST*Av.Gbu.Matrix.m_Data.RC.R1C2,
							            		-CAM_PROJ_DIST*Av.Gbu.Matrix.m_Data.RC.R2C2);
		  		ProjPoint += GbuPos;
			}

			LosVal = LOS(&GbuPos,&ProjPoint,&Intersect, HI_RES_LOS, TRUE);

			if(LosVal)
				Intersect.X = -1;  // nullify location if no LOS

		  Av.Gbu.TrackPosObtained = !LosVal;
  		Av.Gbu.TrackPos = Intersect;

		  int PlaneFound,VehFound,ObjFound;
	    FPointDouble ObjTarget,PlaneTarget,VehTarget;
		  float ObjDist,PlaneDist,VehDist;
		  RegisteredObject *GrndTrgt;
      MovingVehicleParams *VehTrgt;

		  ObjFound   = GetFlirWeapCamTarget(&GbuPos,&ProjPoint,&GrndTrgt,&ObjDist,&TgtLoc);
		  PlaneFound = GrabPlaneInFlirWeapCam(&GbuPos,&ProjPoint,&PlaneIndex,&PlaneDist);
		  VehFound   = GetFlirWeapCamVehicle(GbuPos,Intersect,&VehTrgt,&VehDist);

			if(!ObjFound && !PlaneFound && !VehFound && !Av.Gbu.TrackPosObtained)
				Av.Gbu.TrackOn = FALSE;
			else
			{
				if((ObjFound) && (!PlaneFound || (ObjDist < PlaneDist)) && (!VehFound ||(ObjDist < VehDist)) )
				{
					Av.Gbu.TargetObtained = TRUE;
					Av.Gbu.TargetType = FLIR_CAM_LOC_TARGET;
					Av.Gbu.TargetLoc = TgtLoc;
				}
				else
					if(PlaneFound && (!ObjFound || (PlaneDist < ObjDist)) && (!VehFound || (PlaneDist < VehDist)) )
					{
						Av.Gbu.TargetObtained = TRUE;
						Av.Gbu.TargetType = FLIR_CAM_PLANE_TARGET;
						Av.Gbu.Target = (PlaneParams *)&Planes[PlaneIndex];
					}
					else
						if(VehFound && (!ObjFound || (VehDist < ObjDist)) && (!PlaneFound || (VehDist < PlaneDist)) )
						{
							Av.Gbu.TargetObtained = TRUE;
							Av.Gbu.TargetType = FLIR_CAM_VEH_TARGET;
							Av.Gbu.Target = (MovingVehicleParams *)VehTrgt;
						}
						else if(!LosVal)
						{
						// int CloseTargetType;

						// since Gbus are tracking the ground, get closest object within 400 ft and track it
						//	 	if( GetCloseVehicleOrObject(&Intersect,&GrndTrgt,&VehTrgt,&CloseTargetType) )
						//	 	{
						//       Av.Gbu.TargetObtained = TRUE;
						// 			if(CloseTargetType == FLIR_CAM_GROUND_TARGET)
						// 			{
						//         		Av.Gbu.TargetType = FLIR_CAM_GROUND_TARGET;
						//         		Av.Gbu.Target     = GrndTrgt->Instance;
						// 			}
						// 			else
						// 			{
						//         		Av.Gbu.TargetType = FLIR_CAM_VEH_TARGET;
						//         		Av.Gbu.Target    = (MovingVehicleParams *)VehTrgt;
						// 			}
						//	}
						//	else
						//{
							Av.Gbu.TargetObtained = TRUE;
							Av.Gbu.TargetType     = FLIR_CAM_LOC_TARGET;
							Av.Gbu.TargetLoc      = Intersect;
						//	}

							if(Av.Gbu.TargetObtained && ( (Av.Gbu.TargetType == FLIR_CAM_PLANE_TARGET) || (Av.Gbu.TargetType == FLIR_CAM_VEH_TARGET)  || (Av.Gbu.TargetType == FLIR_CAM_GROUND_TARGET)) )
								Av.Gbu.TrackPosObtained = FALSE;
						}  // !los
			}  // object found
		} // target obtained
	} // track on

	if(!Av.Gbu.TrackOn)
	{
	  DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Gbu.SlewLeftRight,Av.Gbu.SlewUpDown,FALSE,FALSE,FALSE,GbuPos,GbuOnMissile,Av.Gbu.Weap,Av.Gbu.Magnify,&Av.Gbu);
	}

	//DrawAltAndElev(MpdNum, XOfs,YOfs, &Av.Gbu.;

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
	  DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
	  DrawHarmMode();
	}

  DrawVerticalWordAtButton(MpdNum,11,"STP",FALSE,AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,14,"GUN",WeapStores.GunsOn,AV_GREEN,1.0);

	DrawWeaponSelections(MpdNum);

	ProcessPointer(&Av.Gbu,GBU15_TYPE);

	  AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	// STATION NUMBER
	if(Av.Weapons.CurAGWeap)
	{
		sprintf(TmpStr,"STA %d",Av.Weapons.CurAGWeap->Station + 1);
		DrawTextAtLoc(TEWS_CENTER_X - 50,TEWS_CENTER_Y - 50,TmpStr,AV_GREEN,1.0);
	}

	if(pCurrentGBU)
	{
		RightJustifyTextAtLocation(125,14,(Av.Gbu.TrackOn) ? "CAGED" : "UNCAGED",Color.Red,Color.Green,Color.Blue);
	}
	else
	{
		RightJustifyTextAtLocation(125,14,(Av.Gbu.AquireMode == MSL_AUTO) ? "CAGED" : "UNCAGED",Color.Red,Color.Green,Color.Blue);
	}

	if((PlayerPlane->AGDesignate.X != -1) && (Av.Weapons.CurAGWeap))
	{
			// IN RANGE AND CAGED INFO
			float MinRange,MaxRange;

			pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
			WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange, &MaxRange);

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);
			DistToTarget *= WUTOFT;

		  int Time = (DistToTarget - MaxRange)/PlayerPlane->IfHorzVelocity;

			if(Time > 99)
			  Time = 99;
		  if(Time < 0)
			  Time = 0;

			sprintf(TmpStr,"%d %s",Time, "TTMR" );

			if( (DistToTarget < MaxRange) && (DistToTarget > MinRange) )
	  		RightJustifyTextAtLocation(125,21,"IN RANGE",Color.Red,Color.Green,Color.Blue);
			else
				RightJustifyTextAtLocation(125,21,TmpStr,Color.Red,Color.Green,Color.Blue);
	}

	// crosshair
	GrDrawLine(GrBuffFor3D,0,TEWS_CENTER_Y - 5,140,TEWS_CENTER_Y - 5,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,0,TEWS_CENTER_Y + 5,140,TEWS_CENTER_Y + 5,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X - 5,0,TEWS_CENTER_X - 5,140,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,TEWS_CENTER_X + 5,0,TEWS_CENTER_X + 5,140,Color.Red,Color.Green,Color.Blue,0);

  //DrawGbuOuterText(XOfs,YOfs,MpdNum,MpdNum);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 11: GbuStep();
							 break;
			case 14: GbuToggleGuns();
							 break;
			case 19: AGLeftSelect();
			         break;
			case 18: AGFrontLeftSelect();
							 break;
			case 17: AGCenterSelect();
							 break;
			case 16: AGFrontRightSelect();
							 break;
			case 15: AGRightSelect();
							 break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CageUnCageWeapon(int Type)
{
	if(Type == MAVERIC_TYPE)
	{
		 Av.Mav.AquireMode = (Av.Mav.AquireMode == MSL_MANUAL) ? MSL_AUTO : MSL_MANUAL;
		 Av.Mav.TargetObtained = FALSE;
		 Av.Mav.TrackOn = FALSE;
		 Av.Mav.AGDesignate.X = Av.Mav.AGDesignate.Y = Av.Mav.AGDesignate.Z = -1;
	}
	else
	{
     Av.Gbu.AquireMode = (Av.Gbu.AquireMode == MSL_MANUAL) ? MSL_AUTO : MSL_MANUAL;
		 Av.Gbu.TargetObtained = FALSE;
		 Av.Gbu.TrackOn = FALSE;
		 Av.Gbu.AGDesignate.X = Av.Gbu.AGDesignate.Y = Av.Gbu.AGDesignate.Z = -1;
	}
}

//*****************************************************************************************************************************************
// FLIR CODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

FPointDouble TFlirOffset;
FPointDouble NavFlirOffset;

int PrevFlirTrackState;  // to know when flir track state changes for showing the "designate" text
FMatrix MavCamMatrix;

#define FLIR_LASE_POS(x,y)		    (60+x),(14+y)
#define FLIR_AZIM_ELEV_POS(x,y)		(10+x),(14+y)
#define FLIR_TRACK_POS(x,y)		    (90+x),(14+y)

extern FPointDouble AG_ImpactPoint;
extern BOOL DrawMeatball;

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float FlirMagMult; // debug for Chris Martin

void InitFlir()
{
	FlirMagMult = 1.0;

	if(GetRegValueL("FlirMagVal") > 0)
	{
		FlirMagMult = GetRegValueL("FlirMagVal");
		FlirMagMult /= 100.0;
	}

	Av.Flir.Mode = FLIR_VV_MODE;
 	Av.Flir.FOVToUse = WFOV;
	Av.Flir.Magnify = (float)4.0*FlirMagMult*(float)DEGREE;
 	Av.Flir.SlewLeftRight = 0;
 	Av.Flir.SlewUpDown = 0;
 	Av.Flir.Brightness = 2;
 	Av.Flir.Contrast = 5;
 	Av.Flir.Polarity = WHOT;
 	Av.Flir.TrackLS = FALSE;
 	Av.Flir.ReticleOn = TRUE;
 	Av.Flir.DeclutterOn = FALSE;
 	Av.Flir.LaserOption = FLIR_ALAS;
 	Av.Flir.LaserState = FLIR_LSR_ARMED;
 	Av.Flir.MaskOn = FALSE;
 	Av.Flir.SnowplowOn = FALSE;
 	Av.Flir.MtgtOn = FALSE;
	Av.Flir.TrackOn = FALSE;
	Av.Flir.LastMode = AG_MODE;
	Av.Flir.TargetObtained = FALSE;
	Av.Flir.AADesignate = NULL;

 	Av.NavFlir.Brightness = 2;
 	Av.NavFlir.Contrast = 5;
 	Av.NavFlir.DeclutterOn = FALSE;

	LaserLoc.X = LaserLoc.Y = LaserLoc.Z = -1;

	DrawMeatball = FALSE;

  	TFlirOffset.SetValues(-4.266 FEET,-2.752 FEET,5.0 FEET); //should be 5.188 feet in z but this leaves it just in front of the lens

	NavFlirOffset.SetValues(4.195 FEET,-2.352 FEET, 5.70 FEET);  // should be 5.876 in z

//  FlirOffset.X = -4.5*FTTOWU;
//  FlirOffset.Y = -3.5*FTTOWU;
//  FlirOffset.Z = -7.0*FTTOWU;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetFlir(int Mode)
{
	if(Mode == AA_MODE)
	{
 			Av.Flir.Mode = FLIR_BST_MODE;
 			Av.Flir.FOVToUse = WFOV;
			Av.Flir.Magnify = (float)4.0*FlirMagMult*(float)DEGREE;
 			Av.Flir.SlewLeftRight = 0;
 			Av.Flir.SlewUpDown = 0;
 			Av.Flir.TrackLS = FALSE;
 			Av.Flir.ReticleOn = TRUE;
 			Av.Flir.DeclutterOn = FALSE;
 			Av.Flir.LaserOption = FLIR_ALAS;
  		Av.Flir.LaserState = FLIR_LSR_ARMED;
 			Av.Flir.MaskOn = FALSE;
 			Av.Flir.SnowplowOn = FALSE;
 			Av.Flir.TrackOn = FALSE;
     	Av.Flir.TargetObtained = FALSE;
	    Av.Flir.AADesignate = NULL;
	}
	else
	{
 			Av.Flir.Mode = FLIR_VV_MODE;
 			Av.Flir.FOVToUse = WFOV;
			Av.Flir.Magnify = (float)5.85*FlirMagMult*(float)DEGREE;
 			Av.Flir.SlewLeftRight = 0;
 			Av.Flir.SlewUpDown = 0;
 			Av.Flir.TrackLS = FALSE;
 			Av.Flir.ReticleOn = TRUE;
 			Av.Flir.DeclutterOn = FALSE;
 			Av.Flir.LaserOption = FLIR_ALAS;
  		Av.Flir.LaserState = FLIR_LSR_ARMED;
 			Av.Flir.MaskOn = FALSE;
 			Av.Flir.SnowplowOn = FALSE;
 	    Av.Flir.MtgtOn = FALSE;
 			Av.Flir.TrackOn = FALSE;
	    Av.Flir.TargetObtained = FALSE;
	    Av.Flir.AADesignate = NULL;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetFlirAABoreSight()
{
 Av.Flir.Mode = FLIR_LOS_POINT_MODE;
 Av.Flir.TrackOn = FALSE;
 Av.Flir.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetFlirBoreSight()
{
 if(Av.Flir.Mode == FLIR_LOCK_MODE)
	 PlayerPlane->AGDesignate.X = -1;

 Av.Flir.Mode = FLIR_VV_MODE;
 Av.Flir.SnowplowOn = FALSE;
 Av.Flir.TrackOn = FALSE;
 Av.Flir.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirBoreSightKeypress()
{
 Av.Flir.SlewLeftRight = 0;
 Av.Flir.SlewUpDown = 0;
 Av.Flir.TrackOn = FALSE;
 Av.Flir.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MatSetRPH(FMatrix *Mat,double roll,double pitch,double heading)
{
	double h = DEGREES_TO_RADIANS*heading;
	double p = DEGREES_TO_RADIANS*pitch;
	double r = DEGREES_TO_RADIANS*roll;

	double SH,SR,SP;
	double CH,CR,CP;
	double SRSP,CRSP;

	SH = sin(h);
	SR = sin(r);
	SP = sin(p);

	CH = cos(h);
	CR = cos(r);
	CP = cos(p);

	SRSP = SR*SP;
	CRSP = CR*SP;

	Mat->m_Data.RC.R0C0 = CR*CH+SRSP*SH;
	Mat->m_Data.RC.R0C1 = CRSP*SH-SR*CH;
	Mat->m_Data.RC.R0C2 = CP*SH;

	Mat->m_Data.RC.R1C0 = SR*CP;
	Mat->m_Data.RC.R1C1 = CR*CP;
	Mat->m_Data.RC.R1C2 = -SP;

	Mat->m_Data.RC.R2C0 = SRSP*CH-CR*SH;
	Mat->m_Data.RC.R2C1 = SR*SH+CRSP*CH;
	Mat->m_Data.RC.R2C2 = CP*CH;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetDebugCameraLine(FPointDouble &location,FMatrix &mat);

void CalcCameraViewVector(float LeftRight, float UpDown, FPointDouble *Origin, FPointDouble *ViewVec,float Heading)
{
  float Yaw,Pitch;
  FPointDouble Local;

  Local.X = 0;
  Local.Y = 0;
  Local.Z = -CAM_PROJ_DIST;

  double RetAngle;
  RetAngle = UpDown;
  if(RetAngle < 0.0f)
	RetAngle = 360.0 + RetAngle;
  if(RetAngle >= 360.0)
	RetAngle = RetAngle - 360.0f;

  Pitch = RetAngle;

  RetAngle = fANGLE_TO_DEGREES(Heading) + LeftRight;
  if(RetAngle < 0.0f)
	RetAngle = 360.0 + RetAngle;
  if(RetAngle >= 360.0)
	RetAngle = RetAngle - 360.0f;

  Yaw = RetAngle;

  MavCamMatrix.SetRadRPH(0,DEGREES_TO_RADIANS * Pitch,DEGREES_TO_RADIANS * Yaw);

// uncomment to see where the camera is focused
//SetDebugCameraLine(*Origin,MavCamMatrix);

  Local.RotateInto(Local,MavCamMatrix);

  ViewVec->X =  Origin->X + Local.X;
  ViewVec->Z =  Origin->Z + Local.Z;
  ViewVec->Y =  Origin->Y + Local.Y;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

SLEW_ACTIONS IsFlirWithinSlewBounds(float LeftRight, float UpDown,float *new_azim,float *new_elev)
{
	FMatrix mat;
	SLEW_ACTIONS ret_val;
	float heading_to_use;
	float hi_el;

	hi_el = 10;

	if ((LeftRight < 40) && (LeftRight > -7))
	{
		if (LeftRight >= 30)
			hi_el = (40 - LeftRight);
		else
			if (LeftRight <= -2)
				hi_el = (LeftRight - -7);
	}
	else
    	if( (LeftRight <= 90) && (LeftRight >= -90) )
			hi_el = 0;
		else
			if(  ( (LeftRight > 90) && (LeftRight <= 150) ) || ( (LeftRight < -90) && (LeftRight >= -150) )  )
	  			hi_el = 0;
			else
				if( (LeftRight > 150) || (LeftRight < -150) )
					hi_el = -30;


	heading_to_use = fANGLE_TO_DEGREES(PlayerPlane->Heading);

//	check_azim = NormDegree(LeftRight + heading_to_use);

	ret_val = CheckAnglesWithinBounds(PlayerPlane->Attitude,RLAngle(LeftRight+heading_to_use),UpDown,360.0,hi_el,-360.0f,-90.0f,new_azim,new_elev);

	if (new_azim)
		*new_azim = RLAngle(*new_azim - heading_to_use);

	return ret_val;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void PointFlirToCurrentAGTarget()
{
  if(PlayerPlane->AGDesignate.X <= 0) return;

  if( (Av.Flir.AGDesignate.X != PlayerPlane->AGDesignate.X) ||
      ( (Av.Flir.AGDesignate.Y != PlayerPlane->AGDesignate.Y) && (fabs(PlayerPlane->AGDesignate.Y - Av.Flir.AGDesignate.Y) > 10.0)) ||
      (Av.Flir.AGDesignate.Z != PlayerPlane->AGDesignate.Z) || !Av.Flir.TargetObtained)
  {
      float Heading,Pitch;

      CalcNewFlirCamHeadingAndPitch(FlirPos, &Heading, &Pitch, PlayerPlane->AGDesignate,NULL);

      float LeftRight = Heading;
      float UpDown    = Pitch;

      if(!IsFlirWithinSlewBounds(LeftRight,UpDown,NULL,NULL))
	    {
        	Av.Flir.SlewLeftRight = LeftRight;
        	Av.Flir.SlewUpDown    = UpDown;

        	Av.Flir.AGDesignate.X = PlayerPlane->AGDesignate.X;
        	Av.Flir.AGDesignate.Y = PlayerPlane->AGDesignate.Y;
        	Av.Flir.AGDesignate.Z = PlayerPlane->AGDesignate.Z;

			Av.Flir.TargetObtained = TRUE;
			Av.Flir.TargetType = FLIR_CAM_LOC_TARGET;
			Av.Flir.TargetLoc  = PlayerPlane->AGDesignate;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void PointFlirToCurrentAATarget()
{
  if(PlayerPlane->AADesignate == NULL) return;

  if((Av.Flir.AADesignate != PlayerPlane->AADesignate) || (!Av.Flir.TargetObtained))
  {
      float Heading,Pitch;

      CalcNewFlirCamHeadingAndPitch(FlirPos, &Heading, &Pitch, PlayerPlane->AADesignate->WorldPosition,NULL);

      float LeftRight = Heading;
      float UpDown    = Pitch;

      if(!IsFlirWithinSlewBounds(LeftRight,UpDown,NULL,NULL))
	    {
        	Av.Flir.SlewLeftRight = LeftRight;
        	Av.Flir.SlewUpDown    = UpDown;

        	Av.Flir.AADesignate = PlayerPlane->AADesignate;

			Av.Flir.TargetObtained = TRUE;
			Av.Flir.TargetType = FLIR_CAM_PLANE_TARGET;
			Av.Flir.Target     = (PlaneParams *)PlayerPlane->AADesignate;
		}
	}
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int FlirTrackLocation(FPointDouble TrackLoc)
{
    float Heading,Pitch;

    CalcNewFlirCamHeadingAndPitch(FlirPos,&Heading,&Pitch,TrackLoc,NULL);

    float LeftRight = Heading;
    float UpDown    = Pitch;

    if(!IsFlirWithinSlewBounds(LeftRight,UpDown,NULL,NULL))
		{
      Av.Flir.SlewLeftRight = LeftRight;
      Av.Flir.SlewUpDown    = UpDown;
		}
		else
		{
			if(UFC.MasterMode == AA_MODE)
			  SetFlirAABoreSight();
			else
		    SetFlirBoreSight();

			return(FALSE);
		}

		return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetFlirTargetPosition(FPointDouble *Target)
{
	if(!Av.Flir.TargetObtained) return(FALSE);

	if(Av.Flir.TargetType == FLIR_CAM_GROUND_TARGET)
		*Target = ((BasicInstance *)Av.Flir.Target)->Position;
	else if(Av.Flir.TargetType == FLIR_CAM_VEH_TARGET)
		*Target = ((MovingVehicleParams *)Av.Flir.Target)->WorldPosition;
	else if(Av.Flir.TargetType == FLIR_CAM_PLANE_TARGET)
		*Target = ((PlaneParams *)Av.Flir.Target)->WorldPosition;
	else
		*Target = Av.Flir.TargetLoc;

	return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetFlirTarget(FPointDouble *FlirPos, int *TargetObtained,int *TargetType,FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane)
{
   FPointDouble ProjPoint;
   FPointDouble Intersect;
   FPointDouble TgtLoc;
   int PlaneIndex;

   CalcCameraViewVector(Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FlirPos,&ProjPoint,PlayerPlane->Heading);

   int Result = LOS(FlirPos,&ProjPoint,&Intersect, HI_RES_LOS, TRUE);

	 if(Result)
			Intersect.X = -1;  // nullify location if no LOS

	 *TargetObtained = FALSE;

   int PlaneFound,VehFound,ObjFound;
   float ObjDist,PlaneDist,VehDist;
   RegisteredObject *GrndTrgt;
   MovingVehicleParams *VehTrgt;

   ObjFound   = GetFlirWeapCamTarget(FlirPos,&Intersect,&GrndTrgt,&ObjDist,&TgtLoc);
   PlaneFound = GrabPlaneInFlirWeapCam(FlirPos,&Intersect,&PlaneIndex,&PlaneDist);
   VehFound   = GetFlirWeapCamVehicle(*FlirPos,Intersect,&VehTrgt,&VehDist);

   if(ObjFound || PlaneFound || VehFound)
   {
		if(ObjFound && (!PlaneFound || (ObjDist < PlaneDist)) && (!VehFound || (ObjDist < VehDist)) )
		{
       	*TargetObtained = TRUE;
		   *TargetType = FLIR_CAM_LOC_TARGET;
		   *TargetLoc = TgtLoc;
		}
		else if(PlaneFound && (!ObjFound || (PlaneDist < ObjDist)) && (!VehFound || (PlaneDist < VehDist)) )
		{
      *TargetObtained = TRUE;
			*TargetType = FLIR_CAM_PLANE_TARGET;
			*Plane     = &Planes[PlaneIndex];
		}
		else if(VehFound && (!ObjFound || (VehDist < ObjDist)) && (!PlaneFound || (VehDist < PlaneDist)) )
		{
      *TargetObtained = TRUE;
			*TargetType = FLIR_CAM_VEH_TARGET;
			*Veh        = VehTrgt;
		}
	 }
   else
	 {
		if(!Result)
		{
    	*TargetObtained = TRUE;
    	*TargetType = FLIR_CAM_LOC_TARGET;
    	*TargetLoc = Intersect;
		}
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ForceFlirUpDownInBounds(float &SlewLeftRight,float &SlewUpDown)
{
	if(SlewUpDown < -90.0)
       SlewUpDown    = -90.0;
	else
		if ((SlewLeftRight < 40) && (SlewLeftRight > -7))
		{
			if (SlewLeftRight > 30)
			{
				if (SlewUpDown > (40 -SlewLeftRight))
					SlewUpDown =  (40 -SlewLeftRight);
			}
			else
				if (SlewLeftRight < -2)
				{
					if (SlewUpDown > 2*(SlewLeftRight- -7))
						SlewUpDown =  2*(SlewLeftRight - -7);
				}
				else
					if (SlewUpDown > 10.0f)
						SlewUpDown = 10.0f;
		}
		else
 	   	if(SlewUpDown > 0.0)
		   	SlewUpDown = 0.0;
}

void ProcessFlirCamSlew(int MpdNum,WeapCamSlewType *CamSlew,int FOVToUse,int *DrewCamera,int RelativeSlew=0)
{
  *DrewCamera = FALSE;

	if( GetTDCMode() != TGT_IR_MODE ) return;

	int JoystickForSlew = GetVkStatus(CaptBarsVk);
	int JoyDir = -1;

	if(JoystickForSlew)
	{
	  JoyDir = GetJoystickMapDirection();
		if(JoyDir == -1)
			JoystickForSlew = FALSE;
	}

	int Left  = (JoyDir == LEFT);
	int Right = (JoyDir == RIGHT);
	int Up    = (JoyDir == TOP);
	int Down  = (JoyDir == BOTTOM);

  if(GetVkStatus(TdcUpVk) || (JoystickForSlew && Up) )
  {
    if( (!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_UP) )
	    SetupWeapCamSlew(SLEW_UP, MpdNum, FLIR_TYPE);
  }
  else if(GetVkStatus(TdcDownVk) || (JoystickForSlew && Down) )
  {
    if( (!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_DOWN) )
	    SetupWeapCamSlew(SLEW_DOWN, MpdNum, FLIR_TYPE);
  }
  else if(GetVkStatus(TdcLeftVk) || (JoystickForSlew && Left) )
  {
     if( (!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_LEFT)  )
	     SetupWeapCamSlew(SLEW_LEFT, MpdNum, FLIR_TYPE);
  }
  else if(GetVkStatus(TdcRightVk) || (JoystickForSlew && Right) )
  {
     if( (!CamSlew->Slewing) || (CamSlew->SlewDirection != SLEW_RIGHT) )
	     SetupWeapCamSlew(SLEW_RIGHT, MpdNum, FLIR_TYPE);
  }

  if(CamSlew->Slewing)
  {
  	if(CamSlew->MpdNum == MpdNum)
		{
	  	float SlewAmount;

	  	if(FOVToUse == WFOV)
		  	SlewAmount = 4.0;
	  	else if(FOVToUse == NFOV)
      	SlewAmount = 0.75;
	  	else if(FOVToUse == ENFOV)
      	SlewAmount = 0.15;

	float	elapsed = CamSlew->SlewTimer.TimeElapsed(GetTickCount());

	if (elapsed > 1.0f/15.0f)
		elapsed = 1.0f/15.0f;

	  	CamSlew->SlewOffset = elapsed*SlewAmount;
	  	//CamSlew->SlewOffset = SlewAmount;

		float SlewUpDown;
		float SlewLeftRight;

		if( (!Av.Flir.TargetObtained) || (!RelativeSlew) )
		{
	  		SlewUpDown    = Av.Flir.SlewUpDown;
	  		SlewLeftRight = Av.Flir.SlewLeftRight;
		}
		else
		{
			float Heading,Pitch;
			CalcNewFlirCamHeadingAndPitch(FlirPos,&Heading,&Pitch,Av.Flir.TrackPos,NULL);
			SlewUpDown    = Pitch;
	  		SlewLeftRight = Heading;
		}


			// the closer we are to an object, the finer grained we want the slewing
 	  	if(Av.Flir.TrackOn)
		{
			FPointDouble TargetPos;
		    int Result = GetFlirTargetPosition(&TargetPos);

			if(Result)
			{
				double Dist = (TargetPos - FlirPos)*WUTONM;
				if(Dist < 10.0)
				{
					float Frac = Dist/10.0;
					CamSlew->SlewOffset *= Frac;
				}
			}
		}

		switch(CamSlew->SlewDirection)
 	  	{
	    	case SLEW_UP :
							SlewUpDown    += CamSlew->SlewOffset;
												 //if(!GetVkStatus(TdcUpVk) || )
													//CamSlew->Slewing = FALSE;
  			               	 break;
      		case SLEW_DOWN : SlewUpDown    -= CamSlew->SlewOffset;
												 //if(!GetVkStatus(TdcDownVk))
												 //	CamSlew->Slewing = FALSE;
			                 	 break;
	   		case SLEW_LEFT : SlewLeftRight += CamSlew->SlewOffset;
			                 	 if(SlewLeftRight > 180.0)
							         	 SlewLeftRight = -180.0 + (SlewLeftRight - 180.0);
												// if(!GetVkStatus(TdcLeftVk))
												 //	CamSlew->Slewing = FALSE;
 			                 	 break;
		  	case SLEW_RIGHT: SlewLeftRight -= CamSlew->SlewOffset;
                       	 if(SlewLeftRight < -180.0)
				         	 SlewLeftRight = 180.0 + (SlewLeftRight + 180.0);
												// if(!GetVkStatus(TdcRightVk))
												//	CamSlew->Slewing = FALSE;
						           	 break;
	  	}

//		ForceFlirUpDownInBounds(SlewLeftRight,SlewUpDown);


	  	IsFlirWithinSlewBounds(SlewLeftRight,SlewUpDown,&Av.Flir.SlewLeftRight,&Av.Flir.SlewUpDown);

		if(RelativeSlew)
		{
	        DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);
          	GetFlirTarget(&FlirPos,&Av.Flir.TargetObtained,&Av.Flir.TargetType,&Av.Flir.TargetLoc,(BasicInstance **)&Av.Flir.Target,(MovingVehicleParams **)&Av.Flir.Target,(PlaneParams **)&Av.Flir.Target);
			*DrewCamera = TRUE;
		}

	  	int KeypressSlew = ( (GetVkStatus(TdcUpVk)) || (GetVkStatus(TdcDownVk)) ||
		                   	(GetVkStatus(TdcLeftVk)) || (GetVkStatus(TdcRightVk)) || (JoystickForSlew) );

	  	if( (GetMouseLB() == 0) && (!KeypressSlew) )
			{
	    	CamSlew->Slewing = FALSE;
			}
		}
	}
	else
	  	IsFlirWithinSlewBounds(Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,&Av.Flir.SlewLeftRight,&Av.Flir.SlewUpDown);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

PlaneParams *IsPlaneInFlirBeam()
{
	FPointDouble Target;
	FPointDouble ProjPoint;
	FMatrix Matrix;

	Matrix.SetRPH(0,Av.Flir.SlewUpDown*DEGREE,Av.Flir.SlewLeftRight*DEGREE);
	Matrix *= PlayerPlane->Attitude;

  ProjPoint.SetValues(-CAM_PROJ_DIST*Matrix.m_Data.RC.R0C2,
			                -CAM_PROJ_DIST*Matrix.m_Data.RC.R1C2,
							        -CAM_PROJ_DIST*Matrix.m_Data.RC.R2C2);

	ProjPoint += FlirPos;

	AvObjListEntryType *Plane = &Av.Planes.List[0];
	AvObjListEntryType *Closest = NULL;
	float MinDist = 1000000000;

	int Index = Av.Planes.NumPlanes;
  while(Index-- > 0)
  {
			Target = ((PlaneParams *)Plane->Obj)->WorldPosition;

 			float DistToRay   = CAM_PROJ_DIST;
 			float DistToEnemy = Plane->Dist;

			FPointDouble PlayerToScan,PlayerToEnemy;
			PlayerToScan.MakeVectorToFrom(ProjPoint,PlayerPlane->WorldPosition);
			PlayerToEnemy.MakeVectorToFrom(Target,PlayerPlane->WorldPosition);

  		float Dot = PlayerToScan * PlayerToEnemy;

  		float Angle = RadToDeg(acos(Dot/(DistToRay*DistToEnemy)));

			int InCone = ( (Angle > 0) && (Angle < 7.0) && (DistToEnemy <= DistToRay) );

			if( (InCone) && (DistToEnemy < MinDist) )
			{
				Closest = Plane;
				MinDist = DistToEnemy;
			}

			Plane++;
	}

	return( (Closest) ? ((PlaneParams *)Closest->Obj) : NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawFlirLOS()
{
  float NewX, NewZ;
  int NumPixels;

  AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

  NumPixels = 50.0*(1.0 - (fabs(Av.Flir.SlewUpDown)/90.0));

  ProjectPoint(CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,NumPixels,&NewX, &NewZ);

	GrDrawLine(GrBuffFor3D,NewX-3,NewZ-3,NewX-3,NewZ-2,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX+3,NewZ-3,NewX+3,NewZ-2,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX-3,NewZ-3,NewX-2,NewZ-3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX+3,NewZ-3,NewX+2,NewZ-3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX-3,NewZ+3,NewX-3,NewZ+2,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX+3,NewZ+3,NewX+3,NewZ+2,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX-3,NewZ+3,NewX-2,NewZ+3,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,NewX+3,NewZ+3,NewX+2,NewZ+3,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawFlirZoomCues()
{
	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	switch(Av.Flir.FOVToUse)
	{
    	case WFOV:
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-14,CAM_CENTER_Y-2,CAM_CENTER_X-14,CAM_CENTER_Y+2,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X+14,CAM_CENTER_Y-2,CAM_CENTER_X+14,CAM_CENTER_Y+2,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-2,CAM_CENTER_Y-14,CAM_CENTER_X+2,CAM_CENTER_Y-14,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-2,CAM_CENTER_Y+14,CAM_CENTER_X+2,CAM_CENTER_Y+14,Color.Red,Color.Green,Color.Blue,0);
			break;

    	case NFOV :
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-23,CAM_CENTER_Y-23,CAM_CENTER_X-23,CAM_CENTER_Y-15,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-23,CAM_CENTER_Y-23,CAM_CENTER_X-15,CAM_CENTER_Y-23,Color.Red,Color.Green,Color.Blue,0);

			GrDrawLine(GrBuffFor3D,CAM_CENTER_X+22,CAM_CENTER_Y-23,CAM_CENTER_X+22,CAM_CENTER_Y-15,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X+22,CAM_CENTER_Y-23,CAM_CENTER_X+14,CAM_CENTER_Y-23,Color.Red,Color.Green,Color.Blue,0);

			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-23,CAM_CENTER_Y+22,CAM_CENTER_X-23,CAM_CENTER_Y+14,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X-23,CAM_CENTER_Y+22,CAM_CENTER_X-15,CAM_CENTER_Y+22,Color.Red,Color.Green,Color.Blue,0);

			GrDrawLine(GrBuffFor3D,CAM_CENTER_X+22,CAM_CENTER_Y+22,CAM_CENTER_X+22,CAM_CENTER_Y+14,Color.Red,Color.Green,Color.Blue,0);
			GrDrawLine(GrBuffFor3D,CAM_CENTER_X+22,CAM_CENTER_Y+22,CAM_CENTER_X+14,CAM_CENTER_Y+22,Color.Red,Color.Green,Color.Blue,0);

			break;

    	case ENFOV:
			break;
	}
}

void DrawFlirTrackPointer()
{
	if(Av.Flir.ReticleOn)
	{
		AvRGB Color;
		GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

		GrDrawLine(GrBuffFor3D,CAM_CENTER_X-1,CAM_CENTER_Y,CAM_CENTER_X-4,CAM_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,CAM_CENTER_X+1,CAM_CENTER_Y,CAM_CENTER_X+4,CAM_CENTER_Y,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,CAM_CENTER_X,CAM_CENTER_Y-1,CAM_CENTER_X,CAM_CENTER_Y-4,Color.Red,Color.Green,Color.Blue,0);
		GrDrawLine(GrBuffFor3D,CAM_CENTER_X,CAM_CENTER_Y+1,CAM_CENTER_X,CAM_CENTER_Y+4,Color.Red,Color.Green,Color.Blue,0);
	}
// this code may have to be removed
//	else
//		if (Av.Flir.TargetObtained)
//		{
//		  	FPointDouble NullLoc;
//			SetCrossHairsSize(Av.Flir.TargetType, Av.Flir.Target,NullLoc);
//			DrawCrossHairs();
//		}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawFlirLockPointer()
{
	if(Av.Flir.ReticleOn)
	{
//  		AvRGB Color;
//  		GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

  		DrawDiamond(CAM_CENTER_X,CAM_CENTER_Y,2,2,AV_GREEN,1.0);

//		GrDrawLine(GrBuffFor3D,CAM_CENTER_X-7,CAM_CENTER_Y-2,CAM_CENTER_X-7,CAM_CENTER_Y+2,Color.Red,Color.Green,Color.Blue,0);
//		GrDrawLine(GrBuffFor3D,CAM_CENTER_X+7,CAM_CENTER_Y-2,CAM_CENTER_X+7,CAM_CENTER_Y+2,Color.Red,Color.Green,Color.Blue,0);
//		GrDrawLine(GrBuffFor3D,CAM_CENTER_X-2,CAM_CENTER_Y-7,CAM_CENTER_X+2,CAM_CENTER_Y-7,Color.Red,Color.Green,Color.Blue,0);
//		GrDrawLine(GrBuffFor3D,CAM_CENTER_X-2,CAM_CENTER_Y+7,CAM_CENTER_X+2,CAM_CENTER_Y+7,Color.Red,Color.Green,Color.Blue,0);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirSetTgtMode()
{
	if(Av.Flir.Mode == FLIR_TGT_MODE)
	{
		Av.Flir.Mode = FLIR_GS_MODE;
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_FLIR)
			PlayerPlane->AGDesignate.X = -1;
		return;
	}

	Av.Flir.Mode = FLIR_TGT_MODE;
	Av.Flir.TargetObtained = FALSE;
	GetFlirTarget(&FlirPos,&Av.Flir.TargetObtained,&Av.Flir.TargetType,&Av.Flir.TargetLoc,(BasicInstance **)&Av.Flir.Target,(MovingVehicleParams **)&Av.Flir.Target,(PlaneParams **)&Av.Flir.Target);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirSetVVMode()
{
	if(Av.Flir.Mode == FLIR_VV_MODE)
	{
		Av.Flir.TargetObtained = FALSE;
//		Av.Flir.SlewUpDown    = 0;
//		Av.Flir.SlewLeftRight = 0;

		Av.Flir.Mode = FLIR_GS_MODE;
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_FLIR)
			PlayerPlane->AGDesignate.X = -1;
		return;
	}

	Av.Flir.Mode = FLIR_VV_MODE;
	Av.Flir.TargetObtained = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirSetBSTMode()
{
	Av.Flir.Mode = FLIR_BST_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirSetLSMode()
{
	Av.Flir.Mode = FLIR_LS_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirToggleDeclutter()
{
  Av.Flir.DeclutterOn = !Av.Flir.DeclutterOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleLaserMode()
{
	Av.Flir.LaserOption++;
	if(Av.Flir.LaserOption > FLIR_MLAS)
	  Av.Flir.LaserOption = FLIR_ALAS;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleFlirLaser()
{
  if(!Av.Weapons.HasTargetIR) return;

  if(UFC.MasterArmState == ON)
  {
    if(Av.Flir.LaserState == FLIR_LSR_FIRING)
       Av.Flir.LaserState = FLIR_LSR_ARMED;
		else if(Av.Flir.LaserState == FLIR_LSR_ARMED)
			 Av.Flir.LaserState = FLIR_LSR_FIRING;
  }
  else
	Av.Flir.LaserState = FLIR_LSR_ARMED;


  if(Av.Flir.LaserState == FLIR_LSR_FIRING)
  {
    Av.Flir.MaskOn = FALSE;
  }

  if(Av.Flir.LaserState == FLIR_LSR_FIRING)
	lLaserStatusFlags |= WSO_LASER_IS_ON;
  else
    lLaserStatusFlags &= ~WSO_LASER_IS_ON;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirTGTMode(int MpdNum)
{
	FPointDouble TargetLoc;
	int DrewCamera;

  PointFlirToCurrentAGTarget();

  ProcessFlirCamSlew(MpdNum,&FlirSlew,Av.Flir.FOVToUse,&DrewCamera,TRUE);

	Av.Flir.TrackOn = GetFlirTargetPosition(&Av.Flir.TrackPos);
	if(Av.Flir.TrackOn)
	{
		FlirTrackLocation(Av.Flir.TrackPos);
	}

	if(!DrewCamera)
	  DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirGSMode(int MpdNum)
{
	FPointDouble TargetLoc;
	int DrewCamera;

  ProcessFlirCamSlew(MpdNum,&FlirSlew,Av.Flir.FOVToUse,&DrewCamera,TRUE);

	Av.Flir.TrackOn = GetFlirTargetPosition(&Av.Flir.TrackPos);
	if(Av.Flir.TrackOn)
	{
		FlirTrackLocation(Av.Flir.TrackPos);
	}

	if(!DrewCamera)
	  DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirVVMode(int MpdNum)
{

  int DrewCamera;

  ProcessFlirCamSlew(MpdNum,&FlirSlew,Av.Flir.FOVToUse,&DrewCamera);

	if(!Av.Flir.SnowplowOn)
	   Av.Flir.SnowplowOn = FlirSlew.Slewing;

	if(!Av.Flir.SnowplowOn)
	{
		if( (UFC.MasterMode == AG_MODE) && (AG_ImpactPoint.X != -1) )
		  FlirTrackLocation(AG_ImpactPoint);
	}

	DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirLockMode(int MpdNum)
{
	Av.Flir.TrackOn = GetFlirTargetPosition(&Av.Flir.TrackPos);

	int InSlewLimits = FlirTrackLocation(Av.Flir.TrackPos);

  DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);

	if(InSlewLimits)
	{
    PlayerPlane->AGDesignate  = Av.Flir.TrackPos;
    PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_FLIR;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirBstMode(int MpdNum)
{
	int DrewCamera;

	Av.Flir.SlewLeftRight = 0;
	Av.Flir.SlewUpDown    = 0;

  ProcessFlirCamSlew(MpdNum,&FlirSlew,Av.Flir.FOVToUse,&DrewCamera,FALSE);

	if(FlirSlew.Slewing)
		Av.Flir.Mode = FLIR_LOS_POINT_MODE;

  DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirLSMode(int MpdNum)
{
	FPointDouble TargetLoc;
	int DrewCamera = FALSE;

	if(PlayerPlane->AADesignate == NULL)
	{
		Av.Flir.SlewLeftRight = 0;
		Av.Flir.SlewUpDown    = 0;
	}
	else
	{
  		PointFlirToCurrentAATarget();

  		ProcessFlirCamSlew(MpdNum,&FlirSlew,Av.Flir.FOVToUse,&DrewCamera,FALSE);

			if(FlirSlew.Slewing)
			{
				Av.Flir.Mode = FLIR_LOS_POINT_MODE;
				Av.Flir.TrackOn = FALSE;
			}

			Av.Flir.TrackOn = GetFlirTargetPosition(&Av.Flir.TrackPos);
			if(Av.Flir.TrackOn)
			{
				FlirTrackLocation(Av.Flir.TrackPos);
			}
	}

	DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirLosPointMode(int MpdNum)
{
  int DrewCamera;

  ProcessFlirCamSlew(MpdNum,&FlirSlew,Av.Flir.FOVToUse,&DrewCamera,FALSE);

	DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirAALockMode(int MpdNum)
{
	int DrawBound = FALSE;
	FPointDouble TargetLoc;
    float MinX,MaxX,MinY,MaxY;

	Av.Flir.TrackOn = GetFlirTargetPosition(&Av.Flir.TrackPos);
	if(Av.Flir.TrackOn)
	{
		FlirTrackLocation(Av.Flir.TrackPos);

		PlaneParams *P = (PlaneParams *)Av.Flir.Target;
		int InCamera = GetPerspectBoundForObjects(FLIR_CAM_PLANE_TARGET,NULL,Av.Flir.TrackPos,P->Attitude,&MinX,&MaxX,&MinY,&MaxY,TRUE);

		if(InCamera)
		{
			DrawBound = TRUE;
		}
	}

	DrawCamera(MpdNum,CAM_CENTER_X,CAM_CENTER_Y,Av.Flir.SlewLeftRight,Av.Flir.SlewUpDown,FALSE,TRUE,(Av.Flir.Polarity == WHOT),FlirPos,FALSE,NULL,Av.Flir.Magnify,NULL);

	if(DrawBound)
	{
      AvRGB Color;
      GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	  DrawBox(GrBuffFor3D,MinX,MinY,MaxX - MinX,MaxY - MinY,Color.Red,Color.Green,Color.Blue);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirDesignate()
{
	if(UFC.MasterMode == AA_MODE)
	{
	  if(Av.Flir.Mode == FLIR_AA_LOCK_MODE) return;

		if(Av.Flir.Mode == FLIR_LS_MODE)
		{
			if(Av.Flir.TrackOn)
			{
				if(Av.Flir.TargetType == FLIR_CAM_PLANE_TARGET)
				{
					Av.Flir.Mode = FLIR_AA_LOCK_MODE;
				}
			}
		}
		else if( (Av.Flir.Mode == FLIR_LOS_POINT_MODE) || (Av.Flir.Mode == FLIR_BST_MODE) )
		{
			PlaneParams *Plane = IsPlaneInFlirBeam();
			if(Plane != NULL)
			{
				Av.Flir.TrackOn = TRUE;
				Av.Flir.TargetType = FLIR_CAM_PLANE_TARGET;
				Av.Flir.Target = (PlaneParams *)Plane;
				Av.Flir.Mode = FLIR_AA_LOCK_MODE;
			}
		}
	}
	else
	{
		//if(!Av.Flir.TrackOn) return;
		BreakAGLock();
        GetFlirTarget(&FlirPos,&Av.Flir.TargetObtained,&Av.Flir.TargetType,&Av.Flir.TargetLoc,(BasicInstance **)&Av.Flir.Target,(MovingVehicleParams **)&Av.Flir.Target,(PlaneParams **)&Av.Flir.Target);
		Av.Flir.Mode = FLIR_LOCK_MODE;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirUnDesignate()
{
	if(UFC.MasterMode == AA_MODE)
	{
		if(Av.Flir.Mode != FLIR_AA_LOCK_MODE) return;
		ResetFlir(AA_MODE);
	  Av.Flir.Mode = FLIR_LOS_POINT_MODE;
	}
	else
	{
		if(Av.Flir.Mode != FLIR_LOCK_MODE) return;
		Av.Flir.Mode = FLIR_GS_MODE;
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_FLIR)
			PlayerPlane->AGDesignate.X = -1;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirToggleFov()
{
  Av.Flir.FOVToUse++;
  if(Av.Flir.FOVToUse > ENFOV)
	Av.Flir.FOVToUse = WFOV;
  switch (Av.Flir.FOVToUse)
  {
			case WFOV:  Av.Flir.Magnify = (float)4.0*FlirMagMult*(float)DEGREE;   break;
			case NFOV:  Av.Flir.Magnify = (float)1.0*FlirMagMult*(float)DEGREE;   break;
			case ENFOV: Av.Flir.Magnify = (float)0.3*FlirMagMult*(float)DEGREE;   break;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirTogglePolarity()
{
 Av.Flir.Polarity++;
 if(Av.Flir.Polarity > BHOT)
	Av.Flir.Polarity = WHOT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirContrastUp()
{
 Av.Flir.Contrast++;
 if(Av.Flir.Contrast > 10)
	Av.Flir.Contrast = 10;

// FLIRContrast = ((float)Av.Flir.Contrast)*0.2;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirContrastDown()
{
 Av.Flir.Contrast--;
 if(Av.Flir.Contrast < 0)
	Av.Flir.Contrast = 0;

// FLIRContrast = ((float)Av.Flir.Contrast)*0.2;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirBrightnessUp()
{
 Av.Flir.Brightness++;
 if(Av.Flir.Brightness > 10)
	Av.Flir.Brightness = 10;

// FLIRBrightness = ((float)Av.Flir.Brightness)*0.2;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirBrightnessDown()
{
 Av.Flir.Brightness--;
 if(Av.Flir.Brightness < 0)
	Av.Flir.Brightness = 0;

// FLIRContrast = ((float)Av.Flir.Contrast)*0.2;
}

void NavFlirTogglePolarity()
{
 	Av.NavFlir.Polarity^=BHOT;
}


void NavFlirContrastUp()
{
 	Av.NavFlir.Contrast++;
 	if(Av.NavFlir.Contrast > 10)
		Av.NavFlir.Contrast = 10;
}

void NavFlirContrastDown()
{
 	Av.NavFlir.Contrast--;
 	if(Av.NavFlir.Contrast < 0)
		Av.NavFlir.Contrast = 0;
}

void NavFlirBrightnessUp()
{
 	Av.NavFlir.Brightness++;
 	if(Av.NavFlir.Brightness > 10)
		Av.NavFlir.Brightness = 10;
}

void NavFlirBrightnessDown()
{
 	Av.NavFlir.Brightness--;
 	if(Av.NavFlir.Brightness < 0)
		Av.NavFlir.Brightness = 0;
}

void NavFlirToggleDeclutter()
{
	Av.NavFlir.DeclutterOn ^= TRUE;
}



//FLIRContrast  defaults to 1.0 and takes numbers from 0.0 to 5.0, although the difference from 2.0 to 5.0 is pretty insignificant, we may want to limit it to 2.0.  Higher numbers increase the contrast, lower numbers decrease the contrast.  At 0.0, everything will be black.  10 steps from 0.0 to 2.0 is probably good, so 11 settings overall.

//FLIRBrightness defaults to 0.0 and takes numbers from -1.0 to 1.0.  Lower numbers make the screen darker, higher numbers


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirToggleRctl()
{
   Av.Flir.ReticleOn = !Av.Flir.ReticleOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirAGBoresight()
{
	if(Av.Flir.Mode == FLIR_LOCK_MODE)
	{
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_FLIR)
		  PlayerPlane->AGDesignate.X = -1;

		Av.Flir.Mode = FLIR_GS_MODE;
	}

	Av.Flir.TargetObtained = FALSE;
	Av.Flir.SlewUpDown    = 0;
	Av.Flir.SlewLeftRight = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirAAMode(int MpdNum)
{
 	switch(Av.Flir.Mode)
	{
		case FLIR_BST_MODE:
			FlirBstMode(MpdNum);
			DrawFlirTrackPointer();
			break;

		case FLIR_LS_MODE:
			FlirLSMode(MpdNum);
			DrawFlirTrackPointer();
			break;

		case FLIR_AA_LOCK_MODE:
			FlirAALockMode(MpdNum);
			DrawFlirLockPointer();
			break;

		case FLIR_LOS_POINT_MODE:
			FlirLosPointMode(MpdNum);
			DrawFlirTrackPointer();
			break;
	}

  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);


	if(MpdNum != MPD6)
	  DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,9,"DCLTR",(Av.Flir.DeclutterOn),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,6,(Av.Flir.Polarity == WHOT) ? "WHT" : "BLK",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,12,"BST",(Av.Flir.Mode == FLIR_BST_MODE),AV_GREEN,1.0);
  DrawVerticalWordAtButton(MpdNum,13,"L&S",(Av.Flir.Mode == FLIR_LS_MODE),AV_GREEN,1.0);

  DrawVertWordAtLocation(127,44,"SLAVE",FALSE,AV_GREEN,1.0);

	if(PlayerPlane->DamageFlags & DAMAGE_BIT_TARGET_IR)
	{
	  DrawTextAtLoc(4,8,"RDY",AV_GREEN,1.0);
	  GrDrawLine(GrBuffFor3D,2,11,17,11,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	else
	{
	  DrawTextAtLoc(4,8,"OPR",AV_GREEN,1.0);
	}

  DrawMdiArrow(2,21,0);
  DrawMdiArrow(2,51,1);

  DrawMdiArrow(2,68,0);
  DrawMdiArrow(2,98,1);

  	DrawVertWordAtLocation(5,28,"LVL",FALSE,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",Av.Flir.Contrast);
	DrawTextAtLoc(10,34,TmpStr,AV_GREEN,1.0);

  	DrawVertWordAtLocation(5,75,"BRT",FALSE,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",Av.Flir.Brightness);
	DrawTextAtLoc(10,81,TmpStr,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,16,"RTCL",(Av.Flir.ReticleOn),AV_GREEN,1.0);

	switch(Av.Flir.FOVToUse)
	{
    case WFOV:   sprintf(TmpStr,"WIDE"); break;
    case NFOV :  sprintf(TmpStr,"NARR"); break;
    case ENFOV:  sprintf(TmpStr,"ENNR"); break;
	}

	DrawWordAtMpdButton(MpdNum,19,TmpStr,FALSE,AV_GREEN,1.0);

	DrawFlirLOS();

  IntTo3DigitFont((int)Av.Flir.SlewLeftRight,TmpStr);
	strcat(TmpStr,(Av.Flir.SlewLeftRight >=0) ? "L" : "R");
  DrawTextAtLoc(FLIR_AZIM_ELEV_POS(0,0),TmpStr,AV_GREEN,1.0);

  float UpDown=GetRelPitch(PlayerPlane->Attitude,RLAngle(fANGLE_TO_DEGREES(PlayerPlane->Heading) + Av.Flir.SlewLeftRight),Av.Flir.SlewUpDown);

  IntTo3DigitFont((int)UpDown,TmpStr);
	strcat(TmpStr,(UpDown >= 0) ? "U" : "D");
  DrawTextAtLoc(FLIR_AZIM_ELEV_POS(0,6),TmpStr,AV_GREEN,1.0);

	if(!Av.Flir.DeclutterOn)
	{
  	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
 		DrawTextAtLoc(17,126,TmpStr,AV_GREEN,1.0);

  	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 		DrawTextAtLoc(11,132,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%d",(int)(PlayerPlane->WorldPosition.Y*WUTOFT));
		RightJustifyTextAtLocation(120,126,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	DrawFlirZoomCues();


  	DrawAttitudeIndicator(CAM_CENTER_X,CAM_CENTER_Y);
	}

	if(Av.Flir.Mode == FLIR_AA_LOCK_MODE)
    	DrawTextAtLoc(FLIR_TRACK_POS(0,0),"AUTOTRK",AV_GREEN,1.0);

  int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:  FlirContrastUp();
								break;
			case  1:  FlirContrastDown();
								break;

			case  2:  FlirBrightnessUp();
								break;
			case  3:  FlirBrightnessDown();
								break;

			case  6:  FlirTogglePolarity();
								break;
			case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							  break;
			case  9:  FlirToggleDeclutter();
								break;
			case 12:  FlirSetBSTMode();
			          break;
			case 13:  FlirSetLSMode();
			          break;
      case 16:  FlirToggleRctl();
								break;
      case 19:  FlirToggleFov();
			          break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetFlirLaserTarget()
{
	if(Av.Flir.LaserState != FLIR_LSR_FIRING)
	{
	  LaserLoc.X = LaserLoc.Y = LaserLoc.Z = -1;
	}
	else
	{
		FPointDouble TargetPos;
	  	int TargetAvailable = GetFlirTargetPosition(&TargetPos);

		if(TargetAvailable)
		{
			LaserLoc = TargetPos;
	    // JLM TEST CODE
			//PlayerPlane->AGDesignate = LaserLoc;
		}
		else
		{
			Av.Flir.LaserState = (UFC.MasterArmState == ON) ? FLIR_LSR_ARMED : FLIR_LSR_SAFE;
		}
	}
}

void make_time_str(int time)
{
	int Min,Sec;

	if (time < 0)
		time = 0;

	Min = time/60;
    Sec = time % 60;

	if(Min > 59)
	{
		Min = 59;
		Sec = 59;
	}

	sprintf(TmpStr,"%02d:%02d",Min,Sec);
}

BOOL LaserIsMasked()
{
	FPoint p1,p2,vec,normal;
	FPointDouble intersect;
	FMatrix mat,mat1;

	mat1 = PlayerPlane->Attitude;
	mat1.Transpose();

	mat.SetRadRPH(0,Av.Flir.SlewUpDown*DEGREES_TO_RADIANS,(fANGLE_TO_DEGREES(PlayerPlane->Heading) + Av.Flir.SlewLeftRight)*DEGREES_TO_RADIANS);
	mat *= mat1;

	vec.SetValues(0.0,0.0,-1.0);
	vec *= mat;

	p2 = p1 = TFlirOffset;
	p2.AddScaledVector(GetObjectRadius(PlayerPlane->Type->Model),vec);

	return LineCollidesWithModel(PlayerPlane->Type->Model,p1,p2,vec,intersect,normal,FALSE);
}

void FlirAGMode(int MpdNum)
{
	if(Av.Flir.Mode != FLIR_LOCK_MODE)
		Av.Flir.MtgtOn = FALSE;

	switch(Av.Flir.Mode)
	{
    	case FLIR_TGT_MODE:
			FlirTGTMode(MpdNum);
			DrawFlirTrackPointer();
			break;

    	case FLIR_GS_MODE:
			FlirGSMode(MpdNum);
			DrawFlirTrackPointer();
			break;

    	case FLIR_VV_MODE:
			FlirVVMode(MpdNum);
			DrawFlirTrackPointer();
			break;

    	case FLIR_LOCK_MODE:
			FlirLockMode(MpdNum);
			DrawFlirLockPointer();
			break;
	}

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(MpdNum != MPD6)
		DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,9,"DCLTR",(Av.Flir.DeclutterOn),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,6,(Av.Flir.Polarity == WHOT) ? "WHT" : "BLK",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,11,"RST",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,12,"VV",(Av.Flir.Mode == FLIR_VV_MODE),AV_GREEN,1.0);
 	DrawVerticalWordAtButton(MpdNum,13,"TGT",(Av.Flir.Mode == FLIR_TGT_MODE),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,18,(Av.Flir.LaserOption == FLIR_ALAS) ? "ALAS" : "MLAS",FALSE,AV_GREEN,1.0);
	//DrawWordAtMpdButton(MpdNum,15,"MTGT",(Av.Flir.MtgtOn),AV_GREEN,1.0);

	DrawVertWordAtLocation(127,44,"SLAVE",FALSE,AV_GREEN,1.0);

	if(PlayerPlane->DamageFlags & DAMAGE_BIT_TARGET_IR)
	{
		DrawTextAtLoc(4,8,"RDY",AV_GREEN,1.0);
		GrDrawLine(GrBuffFor3D,2,11,17,11,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	else
		DrawTextAtLoc(4,8,"OPR",AV_GREEN,1.0);

	DrawMdiArrow(2,21,0);
	DrawMdiArrow(2,51,1);

	DrawMdiArrow(2,68,0);
	DrawMdiArrow(2,98,1);

  	DrawVertWordAtLocation(5,28,"LVL",FALSE,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",Av.Flir.Contrast);
	DrawTextAtLoc(10,34,TmpStr,AV_GREEN,1.0);

  	DrawVertWordAtLocation(5,75,"BRT",FALSE,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",Av.Flir.Brightness);
	DrawTextAtLoc(10,81,TmpStr,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,16,"RTCL",(Av.Flir.ReticleOn),AV_GREEN,1.0);

	switch(Av.Flir.FOVToUse)
	{
    case WFOV:   sprintf(TmpStr,"WIDE"); break;
    case NFOV :  sprintf(TmpStr,"NARR"); break;
    case ENFOV:  sprintf(TmpStr,"ENNR"); break;
	}

	DrawWordAtMpdButton(MpdNum,19,TmpStr,FALSE,AV_GREEN,1.0);

	DrawFlirLOS();

	if(Av.Flir.Mode == FLIR_LOCK_MODE)
     DrawTextAtLoc(FLIR_TRACK_POS(0,0),"AUTOTRK",AV_GREEN,1.0);
	else if( (Av.Flir.Mode == FLIR_VV_MODE) && (Av.Flir.SnowplowOn) )
     DrawTextAtLoc(FLIR_TRACK_POS(0,0),"SNPLOW",AV_GREEN,1.0);
	else if(Av.Flir.TrackOn)
     DrawTextAtLoc(FLIR_TRACK_POS(0,0),"GND STAB",AV_GREEN,1.0);

	float Az = fabs(Av.Flir.SlewLeftRight);
	sprintf(TmpStr,"AZ%03d%s",(int)Az,(Av.Flir.SlewLeftRight >= 0) ? "L" : "R" );
	DrawTextAtLoc(16,16,TmpStr,AV_GREEN,1.0);


  	float El=GetRelPitch(PlayerPlane->Attitude,RLAngle(fANGLE_TO_DEGREES(PlayerPlane->Heading)+Av.Flir.SlewLeftRight),Av.Flir.SlewUpDown);

	sprintf(TmpStr,"EL%03d%s",(int)fabs(El),(El >= 0) ? "U" : "D" );
	DrawTextAtLoc(16,23,TmpStr,AV_GREEN,1.0);

	if(!Av.Flir.DeclutterOn)
	{
  	sprintf(TmpStr,"%d",(int)PlayerPlane->IndicatedAirSpeed);
 		DrawTextAtLoc(17,126,TmpStr,AV_GREEN,1.0);

  	sprintf(TmpStr,"M %1.2f",PlayerPlane->Mach);
 		DrawTextAtLoc(11,132,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%d",(int)(PlayerPlane->WorldPosition.Y*WUTOFT));
		RightJustifyTextAtLocation(120,126,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	DrawFlirZoomCues();

  	DrawAttitudeIndicator(CAM_CENTER_X,CAM_CENTER_Y);
	}

//	if(lBombTimer > 0)
//	{
//    	if(!Av.Flir.TimpactTimer.IsActive())
//	  	{
//		  	Av.Flir.TimeToImpact = lBombTimer;
//      	  	Av.Flir.TimpactTimer.Set(3.0,GameLoopInTicks);
//	  	}
//	}

	int TempTRel = UFC.TRelCountDown;
	int usettmr = 0;
	int tempval = 0;
	int ttm = 0;


	if(Av.Weapons.CurAGWeap)
	{
		if(pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE)
			usettmr = 1;
	}

	if(Av.Flir.TimpactTimer.IsActive())
	{
	  	int Dt = (Av.Flir.TimpactTimer.TimeElapsed(GameLoopInTicks))*1000.0;
	  	Av.Flir.TimpactTimer.Set(3.0,GameLoopInTicks);
    	Av.Flir.TimeToImpact -= Dt;
	  	if(Av.Flir.TimeToImpact <= -5000.0f)
		{
			if(lBombFlags & WSO_BOMB_IMPACT)
			{
				lBombFlags &= ~(WSO_BOMB_IMPACT|WSO_BOMB_TREL|WSO_BOMB_TPULL|WSO_STEERING_MSG|WSO_NAV_MSGS);
				lBombTimer = -1;
			}
		 	Av.Flir.TimpactTimer.Disable();
			Av.Flir.TimeToImpact = 0;

			if ((Av.Flir.LaserOption == FLIR_ALAS) && (Av.Flir.LaserState == FLIR_LSR_FIRING))
			{
				ToggleFlirLaser();
				SetFlirLaserTarget();
			}

			if(usettmr)
			{
				tempval = 0;
				ttm = 0;


				TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&tempval,&ttm);

				if(tempval != WEAP_STORES_NO_TARGET_ERROR)
				{
					make_time_str(ttm);
					DrawTextAtLoc(62,31,"TTMR",AV_GREEN,1.0);
					DrawTextAtLoc(61,37,TmpStr,AV_GREEN,1.0);
				}
			}
			else if(TRelSet)
			{
				TempTRel = UFC.TRelCountDown;
				if(TempTRel > 99)
					TempTRel = 99;
				if(TempTRel < 0)
					TempTRel = 0;

				make_time_str(TempTRel);
				DrawTextAtLoc(64,31,"REL",AV_GREEN,1.0);
				DrawTextAtLoc(61,37,TmpStr,AV_GREEN,1.0);
			}

		}
		else
		{
			int sx,sy;
			int tti = (int)((float)Av.Flir.TimeToImpact/1000.0);

			make_time_str(tti);
			DrawTextAtLoc(64,31,"TTI",AV_GREEN,1.0);
			DrawTextAtLoc(61,37,TmpStr,AV_GREEN,1.0);

			if((Av.Flir.LaserOption == FLIR_ALAS) && (Av.Flir.TargetObtained))
			{
				tti -= 20;
				make_time_str(tti);

				GetMPDButtonXY(MpdNum,18,TmpStr,sx,sy);
				sy += 6;
				DrawTextAtLoc(sx,sy,TmpStr,AV_GREEN,1.0);

				if ((tti <= 0) && (Av.Flir.LaserState != FLIR_LSR_FIRING))
				{
					ToggleFlirLaser();
					SetFlirLaserTarget();
				}
			}
		}
	}
	else
	{
		if ((Av.Flir.LaserOption == FLIR_ALAS) && (Av.Flir.LaserState == FLIR_LSR_FIRING))
		{
			ToggleFlirLaser();
			SetFlirLaserTarget();
		}

		if(usettmr)
		{
			tempval = 0;
			ttm = 0;


			TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&tempval,&ttm);

			if(tempval != WEAP_STORES_NO_TARGET_ERROR)
			{
				make_time_str(ttm);
				DrawTextAtLoc(62,31,"TTMR",AV_GREEN,1.0);
				DrawTextAtLoc(61,37,TmpStr,AV_GREEN,1.0);
			}
		}
		else if(TRelSet)
		{
			TempTRel = UFC.TRelCountDown;

			if(TempTRel > 99)
				TempTRel = 99;
			if(TempTRel < 0)
				TempTRel = 0;

			make_time_str(TempTRel);
			DrawTextAtLoc(64,31,"REL",AV_GREEN,1.0);
			DrawTextAtLoc(61,37,TmpStr,AV_GREEN,1.0);
		}
	}


	Av.Flir.MaskOn = FALSE;
	lLaserStatusFlags &= ~WSO_LASER_MASKED;

	if(Av.Weapons.HasTargetIR)
	{
		char *lasname;
		BOOL flash = FALSE;

  		if(UFC.MasterArmState != ON)
			lasname = "L SAFE";
		else
			if(Av.Flir.LaserState == FLIR_LSR_FIRING)
			{
				if (LaserIsMasked())
				{
					Av.Flir.MaskOn = TRUE;
					lLaserStatusFlags |= WSO_LASER_MASKED;
					lasname = "L MASK";
				}
				else
				{
					lasname = "LASE";
					float LasDist = LaserLoc / FlirPos;
					LasDist *= WUTOFT;
					sprintf(TmpStr,"LAS%d",(int)LasDist);
					DrawTextAtLoc(16,30,TmpStr,AV_GREEN,1.0);
				}
				flash = TRUE;
			}
			else
				lasname = "L ARM";

		if (!flash || SimPause || (((GetTickCount()+125) % 500) < 250))
			DrawTextAtLoc(59,22,lasname,AV_GREEN,1.0,TRUE);

	}



  	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:  FlirContrastUp();
								break;
			case  1:  FlirContrastDown();
								break;

			case  2:  FlirBrightnessUp();
								break;
			case  3:  FlirBrightnessDown();
								break;

			case  6:  FlirTogglePolarity();
								break;
			case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							  break;
			case  9:  FlirToggleDeclutter();
								break;
			case 11:  FlirAGBoresight();
								break;
			case 12:  FlirSetVVMode();
			          break;
			case 13:  FlirSetTgtMode();
			          break;
			case 16:  FlirToggleRctl();
			          break;
			case 18:  ToggleLaserMode();
								break;
      case 19:  FlirToggleFov();
			          break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FlirMode(int MpdNum)
{
	float old_bright;
	float old_contrast;

	 old_contrast = FLIRContrast;
	 old_bright = FLIRBrightness;

	FLIRContrast = ((float)Av.Flir.Contrast)*0.2;
	FLIRBrightness = ((float)Av.Flir.Brightness)*0.05;

	FlirPos = PlayerPlane->WorldPosition;

	FlirPos.AddScaledVector(TFlirOffset.X,PlayerPlane->Orientation.J);
	FlirPos.AddScaledVector(-TFlirOffset.Y,PlayerPlane->Orientation.K);
	FlirPos.AddScaledVector(-TFlirOffset.Z,PlayerPlane->Orientation.I);

	if(Av.Flir.LastMode != UFC.MasterMode)
	{
	  	ResetFlir(UFC.MasterMode);
    	Av.Flir.LastMode = UFC.MasterMode;
	}

	SetFlirLaserTarget();

	if(UFC.MasterMode == AA_MODE)
		FlirAAMode(MpdNum);
	else
		FlirAGMode(MpdNum);

	DrawTDCIndicator(MpdNum);

	FLIRContrast = old_contrast;
	FLIRBrightness = old_bright;

}

//*****************************************************************************************************************************************
// TEWS CODE  -- Header -- (search keyword to step through each section of file)
//**************************************************************************************************************************************
void SetAndSortTewsThreats()
{
	ThreatEntryType *T1 = &Threats.List[0];
	ThreatEntryType *T2;

	for(int i=0; i<Threats.NumThreats-1;i++)
	{
		T2 = &Threats.List[i+1];
		for(int j=i+1; j<Threats.NumThreats; j++)
		{
			if(T1->Prog > T2->Prog)
			{
				ThreatEntryType Temp;
				memcpy(&Temp,T1,sizeof(ThreatEntryType));
				memcpy(T1,T2,sizeof(ThreatEntryType));
				memcpy(T2,&Temp,sizeof(ThreatEntryType));
			}
			T2++;
		}
		T1++;
	}
}

	//*****************************************************************************************************************************************
void DrawAvDot(float X, float Y, int Color, float Shade)
{
	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);

	GrDrawLine(GrBuffFor3D,X,Y,X + 1,Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X,Y+1,X+1,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
void DrawTewsPlane(float X, float Y)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	GrDrawLine(GrBuffFor3D,X,Y+3,X,Y-2,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X-3,Y,X+3,Y,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X-1,Y+3,X+1,Y+3,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
void DrawTEWSEmitterSymbol(int ScrX, int ScrY,int Type,char *TEWSAbbrev)
{
	float WordX,WordY,size;

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	sprintf(TmpStr,TEWSAbbrev);
	size = strlen(TmpStr)*3;

	WordX = ScrX - size*0.5f;

	WordY = ScrY;

	switch(Type)
	{
		case TEWS_PLANE_THREAT:
			GrDrawLine(GrBuffFor3D,ScrX - 5,ScrY+1,ScrX, ScrY-3,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,ScrX, ScrY-3,ScrX + 6,ScrY+1,Triple.Red,Triple.Green,Triple.Blue,0);
			break;

		case TEWS_SHIP_THREAT:
			WordY -= 3;
			GrDrawLine(GrBuffFor3D,WordX - 2,WordY + 4,WordX + 0,WordY + 6,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,WordX + 0,WordY + 6,WordX + 6,WordY + 6,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,WordX + 6,WordY + 6,WordX + 8,WordY + 4,Triple.Red,Triple.Green,Triple.Blue,0);
			break;

		case TEWS_MISSLE_THREAT:
			WordY -= 2;
			DrawBox(GrBuffFor3D, WordX-2,WordY-2,size+4, 9, Triple.Red,Triple.Green,Triple.Blue);
			break;

		default:
			WordY -= 2;
	}

	DrawTextAtLoc(WordX,WordY,TmpStr,AV_GREEN,1.0);

}

//*****************************************************************************************************************************************
void DrawTewsEmitter(int CenterX, int CenterY, FPoint EnemyPos,BOOL  critical, int SymbolType, char *TewsAbbrev, int HasLock, int Draw)
{
	FPoint PlanePos,BoxPos,Origin;
	float DeltaAzim;
	float NewX, NewZ;
	int Range = (critical) ? 15 : 38;
	float x1,y1,x2,y2;

	if(Draw && ( (!Av.Tews.LimitOn) || (NumTewsDraw < 5) ))
	{

		PlanePos.X = PlayerPlane->WorldPosition.X;
		PlanePos.Z = PlayerPlane->WorldPosition.Z;
		PlanePos.Y = 0;

		DeltaAzim = NormDegree(ComputeHeadingToPoint(PlanePos,PlayerPlane->Heading,EnemyPos, 1));
		ProjectPoint(CenterX, CenterY, DeltaAzim,Range, &NewX, &NewZ);

		DrawTEWSEmitterSymbol(NewX,NewZ,SymbolType,TewsAbbrev);

		if (critical)
		{
			AvRGB Triple;
			GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

			ProjectPoint(CenterX, CenterY, DeltaAzim,30, &x1, &y1);
			ProjectPoint(CenterX, CenterY, DeltaAzim,50, &x2, &y2);
			GrDrawLine(GrBuffFor3D,x1,y1,x2,y2,Triple.Red,Triple.Green,Triple.Blue,0);
		}

		NumTewsDraw++;
	}
}


//*****************************************************************************************************************************************
int IsEwString(char *Str)
{
	if(strcmp(Str,"SD") == 0)  return(TRUE);
	else if(strcmp(Str,"50") == 0)  return(TRUE);
	else if(strcmp(Str,"EW") == 0)  return(TRUE);
	else if(strcmp(Str,"TS") == 0)  return(TRUE);
	else if(strcmp(Str,"NS") == 0)  return(TRUE);
	else if(strcmp(Str,"SU") == 0)  return(TRUE);

	return(FALSE);
}

//*****************************************************************************************************************************************
void DrawTewsEmitters(int MpdNum, int CenterX, int CenterY)
{
	int IsTarget;
	int Side;
	int UseEw;

	 // just to make the code easier to read
	int Norm     = (Av.Tews.Filter == TEWS_NORM);
	int Sea      = (Av.Tews.Filter == TEWS_SEA);
	int Land     = (Av.Tews.Filter == TEWS_LAND);
	int Air      = (Av.Tews.Filter == TEWS_AIR);
	int Ew       = (Av.Tews.Filter == TEWS_EW);
	int Friendly = (Av.Tews.Filter == TEWS_FRIENDLY);
	int passes_left = 1;
	BOOL under_limit = TRUE;
	BOOL draw_this_one;

	if (Av.Tews.LimitOn)
		passes_left = 2;

	NumTewsDraw = 0;

	while(passes_left-- && under_limit)
	{
		ThreatEntryType *T = &Threats.List[0];
		int Index = Threats.NumThreats;

		while((Index-- > 0) && under_limit)
		{
			IsTarget = FALSE;

			draw_this_one = FALSE;
			if (passes_left)
			{
				if (T->HasLock)
					draw_this_one = TRUE;
			}
			else
				if (!Av.Tews.LimitOn || !T->HasLock)
					draw_this_one = TRUE;

			if (draw_this_one)
			{
				BOOL Range = (BOOL)(T->HasLock);

				Side  = FALSE;
				UseEw = FALSE;

				switch(T->Type)
				{
					case TEWS_PLANE_THREAT:
						if(Friendly)
							Side = (T->P->AI.iSide == AI_FRIENDLY);

						if(Norm || Air || (Side && Friendly) || Ew )
						{
							if(Ew)
							{
								UseEw = IsEwString(T->TEWSAbbrev);

								if(UseEw)
   									DrawTewsEmitter(CenterX,CenterY, T->P->WorldPosition,Range,TEWS_PLANE_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
							}
							else
   								DrawTewsEmitter(CenterX,CenterY, T->P->WorldPosition,Range,TEWS_PLANE_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
						}
					break;

					case TEWS_MISSLE_THREAT:
						if(Norm || Air || (Side && Friendly))
							DrawTewsEmitter(CenterX,CenterY, T->W->Pos,Range,TEWS_MISSLE_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
						break;

					case TEWS_SAM_PROV_THREAT:
						if(Friendly)
							Side = (GDConvertGrndSide(&(T->SP->Basics)) == AI_FRIENDLY);

						if(Norm || Land || Ew || (Side && Friendly) )
						{
							if(Ew)
							{
								UseEw = IsEwString(T->TEWSAbbrev);
								if(UseEw)
									DrawTewsEmitter(CenterX,CenterY,T->SP->Basics.Position,Range,TEWS_SAM_PROV_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
							}
							else
								DrawTewsEmitter(CenterX,CenterY,T->SP->Basics.Position,Range,TEWS_SAM_PROV_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
						}
						break;

					case TEWS_SAM_WEAP_THREAT:
						if(Friendly)
							Side = (GDConvertGrndSide(&(T->SW->Basics)) == AI_FRIENDLY);

						if(Norm || Land || (Side && Friendly) || Ew )
						{
							if(Ew)
							{
								UseEw = IsEwString(T->TEWSAbbrev);
								if(UseEw)
									DrawTewsEmitter(CenterX,CenterY,T->SW->Basics.Position,Range,TEWS_SAM_WEAP_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
							}
							else
								DrawTewsEmitter(CenterX,CenterY,T->SW->Basics.Position,Range,TEWS_SAM_WEAP_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
						}
						break;

					case TEWS_VEH_THREAT:
						if(Friendly)
							Side = (T->V->iSide == AI_FRIENDLY);

						if(T->V->iShipType)
						{
							if(Norm || Sea || (Side && Friendly) || Ew )
							{
								if(Ew)
								{
									UseEw = IsEwString(T->TEWSAbbrev);
									if(UseEw)
										DrawTewsEmitter(CenterX,CenterY,T->V->WorldPosition,Range,TEWS_SHIP_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
								}
								else
									DrawTewsEmitter(CenterX,CenterY,T->V->WorldPosition,Range,TEWS_SHIP_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
							}
						}
						else
							if(Norm || Land || (Side && Friendly) )
								DrawTewsEmitter(CenterX,CenterY,T->V->WorldPosition,Range,TEWS_VEH_THREAT,T->TEWSAbbrev,T->HasLock,T->Draw);
						break;
				}
			}

			under_limit = (BOOL)((!Av.Tews.LimitOn) || (NumTewsDraw < 5));
			T++;
		}

	}
}

//*****************************************************************************************************************************************
void InitTews()
{
	Av.Tews.DeclutterOn = FALSE;
	Av.Tews.CurMode = TEWS_MANUAL_MODE;
	Av.Tews.Range = 40;
	Av.Tews.Filter = 0;
	Av.Tews.FriendlyOn = FALSE;
	Av.Tews.EarlyWarningOn = FALSE;
	Av.Tews.LimitOn = FALSE;
	Av.Tews.EditOn = FALSE;

	Av.Tews.CurProg = 0;

	Av.Tews.CM.ICSState  = CM_ICS_STBY;

	Av.Tews.CM.CurCMDDS = 0;
	Av.Tews.CM.CMDDSStat[0] = CM_CMDDS_STOW;
	Av.Tews.CM.CMDDSStat[1] = CM_CMDDS_STOW;
	Av.Tews.CM.CMDDSStat[2] = CM_CMDDS_STOW;
	Av.Tews.CM.CmddJamming  = FALSE;

	Av.Tews.CM.IDECMStat = CM_IDECM_MAN;

	Av.Tews.CM.ProgInitiated = FALSE;

	InitCM();

	DebugTewsTest = FALSE;
}

//***********************************************************************************************************************************
int GetIcsStat()
{
	if(UFC.EMISState)
		return(ICS_EMIS);
	else if(PlayerPlane->AI.iAIFlags1 & AIJAMMINGON)
		return(ICS_JAM);
	else
		return(ICS_STBY);
}

//*****************************************************************************************************************************************
void DrawTewsDisplay(int MpdNum)
{
	int TotalLength = 52;
	int End1X,End1Y,End2X,End2Y;
	float TotalAngle;
	int TempCenterX, TempCenterY;
	int Diameter = 86;              // JLM make a constant for other modes to use
	float s,c;
	float ang;

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	TempCenterX = TEWS_CENTER_X;
	TempCenterY = TEWS_CENTER_Y;

	DrawTewsEmitters(MpdNum,TempCenterX,TempCenterY);

	TotalAngle = 0.0;

	while(TotalAngle < 360.0)
   	{
		ang = DEGREES_TO_RADIANS * TotalAngle;
		s = sin(ang);
		c = cos(ang);

		End1X = TempCenterX + c * TotalLength;
		End1Y = TempCenterY - s * TotalLength;
		End2X = TempCenterX + c * (TotalLength + 5);
		End2Y = TempCenterY - s * (TotalLength + 5);

		GrDrawLine(GrBuffFor3D,End1X,End1Y,End2X,End2Y,Triple.Red,Triple.Green,Triple.Blue,0);

		TotalAngle += 30.0;
	}

	GrDrawLine(GrBuffFor3D,117,64,126,64,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,117,65,126,65,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,117,75,126,75,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,117,76,126,76,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,14,64,23,64,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,14,65,23,65,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,14,75,23,75,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,14,76,23,76,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawPolyCircle(GrBuffFor3D,TempCenterX,TempCenterY,28,32,Triple.Red,Triple.Green,Triple.Blue,0);

	DrawTewsPlane(TempCenterX,TempCenterY);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgPageUp()
{
	 if(Av.Tews.ReadyForProg == -1) return;

   TewsProgType *T = &TewsProgs[Av.Tews.CurProg];

	 if(Av.Tews.ReadyForProg == TEWS_CHAFF_SELECT)
	 {
		 T->NumChaff++;
		 if(T->NumChaff > 4)
				 T->NumChaff = 4;
   }
	 else if(Av.Tews.ReadyForProg == TEWS_FLARE_SELECT)
	 {
		 T->NumFlares++;
		 if(T->NumFlares > 4)
				 T->NumFlares = 4;
	 }
	 else if(Av.Tews.ReadyForProg == TEWS_REPEAT_SELECT)
	 {
		 T->Repeat++;
		 if(T->Repeat > 10)
				 T->Repeat = 10;
	 }
	 else if(Av.Tews.ReadyForProg == TEWS_INTERVAL_SELECT)
	 {
		 T->Interval += 0.25;
		 if(T->Interval > 9.75)
				 T->Interval = 9.75;
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgPageDown()
{
	 if(Av.Tews.ReadyForProg == -1) return;

   TewsProgType *T = &TewsProgs[Av.Tews.CurProg];

	 if(Av.Tews.ReadyForProg == TEWS_CHAFF_SELECT)
	 {
		 T->NumChaff--;
		 if(T->NumChaff < 0)
				 T->NumChaff = 0;
   }
	 else if(Av.Tews.ReadyForProg == TEWS_FLARE_SELECT)
	 {
		 T->NumFlares--;
		 if(T->NumFlares < 0)
				 T->NumFlares = 0;
	 }
	 else if(Av.Tews.ReadyForProg == TEWS_REPEAT_SELECT)
	 {
		 T->Repeat--;
		 if(T->Repeat < 0)
				 T->Repeat = 0;
	 }
	 else if(Av.Tews.ReadyForProg == TEWS_INTERVAL_SELECT)
	 {
		 T->Interval -= 0.25;
		 if(T->Interval < 0)
				 T->Interval = 0;
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgToggleChaff()
{
  if(Av.Tews.ReadyForProg == TEWS_CHAFF_SELECT)
		 Av.Tews.ReadyForProg = -1;
	else
		 Av.Tews.ReadyForProg = TEWS_CHAFF_SELECT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgToggleFlare()
{
  if(Av.Tews.ReadyForProg == TEWS_FLARE_SELECT)
		 Av.Tews.ReadyForProg = -1;
	else
		 Av.Tews.ReadyForProg = TEWS_FLARE_SELECT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgToggleRepeat()
{
  if(Av.Tews.ReadyForProg == TEWS_REPEAT_SELECT)
		 Av.Tews.ReadyForProg = -1;
	else
		 Av.Tews.ReadyForProg = TEWS_REPEAT_SELECT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgToggleInterval()
{
  if(Av.Tews.ReadyForProg == TEWS_INTERVAL_SELECT)
		 Av.Tews.ReadyForProg = -1;
	else
		 Av.Tews.ReadyForProg = TEWS_INTERVAL_SELECT;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsMode()
{
	Av.Tews.CurMode++;

	if(Av.Tews.CurMode > TEWS_AUTO_MODE)
		Av.Tews.CurMode = TEWS_MANUAL_MODE;

	ToggleIDECM();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsLimit()
{
	Av.Tews.LimitOn = !Av.Tews.LimitOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsProgramStep()
{
	if(Av.Tews.CM.IDECMStat != CM_IDECM_MAN) return;

	Av.Tews.CurProg++;
	if(Av.Tews.CurProg >= 4)
		Av.Tews.CurProg = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsProgramEdit()
{
	if(Av.Tews.CM.IDECMStat != CM_IDECM_MAN) return;

	Av.Tews.EditOn = TRUE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsProgramExit()
{
	Av.Tews.EditOn = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsProgramSave()
{
  memcpy(SavedTewsProgs,TewsProgs,sizeof(TewsProgType)*MAX_TEWS_PROGS);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsEw()
{
 Av.Tews.EarlyWarningOn = !Av.Tews.EarlyWarningOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsThreatPriority()
{
	Av.Tews.Filter++;
	if(Av.Tews.Filter > TEWS_FRIENDLY)
		 Av.Tews.Filter = TEWS_NORM;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleTewsFriendly()
{
	Av.Tews.FriendlyOn = !Av.Tews.FriendlyOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CMDDSTransmitOn()
{
	if(Av.Tews.CM.CurCMDDS == -1) return;

  if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_DEPLOY)
		Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_JAM;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CMDDSTransmitOff()
{
	if(Av.Tews.CM.CurCMDDS == -1) return;

	if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_JAM)
		Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_DEPLOY;
}

//*****************************************************************************************************************************************
void TewsEWMode(int MpdNum)
{
	if(MpdNum != MPD6)
		DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
		DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
		DrawHarmMode();
	}

	switch(Av.Tews.CurMode)
	{
		case TEWS_MANUAL_MODE:    sprintf(TmpStr,"MAN"); break;
		case TEWS_SEMI_AUTO_MODE: sprintf(TmpStr,"SEMI"); break;
		case TEWS_AUTO_MODE:      sprintf(TmpStr,"AUTO"); break;
	}
	DrawVerticalWordAtButton(MpdNum,0,TmpStr,FALSE,AV_GREEN,1.0);

	switch(Av.Tews.Filter)
	{
		case TEWS_NORM:     sprintf(TmpStr,"NORM"); break;
		case TEWS_SEA:      sprintf(TmpStr,"SEA");  break;
		case TEWS_AIR:      sprintf(TmpStr,"AIR");  break;
		case TEWS_LAND:     sprintf(TmpStr,"LAND"); break;
		case TEWS_EW:       sprintf(TmpStr,"EW"); break;
		case TEWS_FRIENDLY: sprintf(TmpStr,"FND"); break;
	}
	DrawVerticalWordAtButton(MpdNum,14,TmpStr,FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,12,"LIM",(Av.Tews.LimitOn),AV_GREEN,1.0);

	int Deploy = FALSE;
	if(Av.Tews.CM.CurCMDDS >= 0)
		 Deploy = (PlayerPlane->TowedDecoyPosition > 0.0);

	DrawVerticalWordAtButton(MpdNum,1,"DPY",Deploy,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,2,"CUT",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,3,"ON",IsDecoyJamming(),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,4,"OFF",!IsDecoyJamming(),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,"STEP",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,6,"EDIT",FALSE,AV_GREEN,1.0);

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);


	DrawTextAtLoc(15,18,"C",AV_GREEN,1.0);
	DrawTextAtLoc(121,18,"F",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",ChaffTotal);
	DrawTextAtLoc(13,26,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",FlareTotal);
	DrawTextAtLoc(119,26,TmpStr,AV_GREEN,1.0);

	DrawBox(GrBuffFor3D,11,24,10,8,Color.Red,Color.Green,Color.Blue);
	DrawBox(GrBuffFor3D,117,24,10,8,Color.Red,Color.Green,Color.Blue);

	DrawTextAtLoc(17,2,"ASPJ",AV_GREEN,1.0);

	if(UFC.EMISState)
		DrawTextAtLoc(17,9,"EMCON",AV_GREEN,1.0);
	else if( IsICSJamming() )
		DrawTextAtLoc(17,9,"JAM",AV_GREEN,1.0);
	else
	{
		if (Av.Tews.CurMode==TEWS_AUTO_MODE)
			DrawTextAtLoc(17,9,"REC",AV_GREEN,1.0);
		else
			DrawTextAtLoc(17,9,"STBY",AV_GREEN,1.0);
	}

	char Buff1[20];
	char Buff2[20];

	if(Av.Tews.CurProg < 5)
		sprintf(Buff1,"U%d",Av.Tews.CurProg+1);
	else
		sprintf(Buff1,"C%d",Av.Tews.CurProg+1);

	switch(Av.Tews.CurMode)
	{
		case TEWS_MANUAL_MODE:
			sprintf(Buff2,"MAN");
		break;
		case TEWS_SEMI_AUTO_MODE:
			sprintf(Buff2,"SEMI");
		break;
		case TEWS_AUTO_MODE:
			sprintf(Buff2,"AUTO");
		break;
	}
	sprintf(TmpStr,"%s %s",Buff1,Buff2);

	DrawTextAtLoc(37,2,"ALE-47",AV_GREEN,1.0);
	DrawTextAtLoc(37,9,TmpStr,AV_GREEN,1.0);

	if(UFC.EMISState)
	{
		sprintf(TmpStr,"EMCON");
		DrawTextAtLoc(83,2,"ALE-50",AV_GREEN,1.0);
		DrawTextAtLoc(83,9,TmpStr,AV_GREEN,1.0);
	} else {
		if(Av.Tews.CM.CurCMDDS != -1)
		{
			if (Av.Tews.CM.CurCMDDS==3)
			{
				sprintf(TmpStr,"NO DCY");
			} else {
				if(PlayerPlane->TowedDecoyPosition <= 0.0)
				{
					sprintf(TmpStr,"STOW %d", 3 - (Av.Tews.CM.CurCMDDS) );
				}
				else if( (PlayerPlane->TowedDecoyPosition > 0.0) && IsDecoyJamming() )
				{
					sprintf(TmpStr,"JAM %d", 3 - (Av.Tews.CM.CurCMDDS) );
				}
				else if( ((PlayerPlane->TowedDecoyPosition > 0.0) && (PlayerPlane->TowedDecoyPosition < 1.0)) && !IsDecoyJamming() )
				{
					sprintf(TmpStr,"STBY %d",3 - (Av.Tews.CM.CurCMDDS) );
				} else if ((PlayerPlane->TowedDecoyPosition == 1.0) && !IsDecoyJamming() )
				{
					sprintf(TmpStr,"DPLY %d",3 - (Av.Tews.CM.CurCMDDS) );
				}
			}

			DrawTextAtLoc(83,2,"ALE-50",AV_GREEN,1.0);
			DrawTextAtLoc(83,9,TmpStr,AV_GREEN,1.0);
		} else {
			sprintf(TmpStr,"NO DCY");
			DrawTextAtLoc(83,2,"ALE-50",AV_GREEN,1.0);
			DrawTextAtLoc(83,9,TmpStr,AV_GREEN,1.0);
		}
	}

	DrawVertWordAtLocation(10,91,"DECOY",FALSE,AV_GREEN,1.0);

	GrDrawLine(GrBuffFor3D,15,131,20,131,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,15,131,15,136,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,52,131,58,131,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,58,131,58,136,Color.Red,Color.Green,Color.Blue,0);

	DrawTextAtLoc(23,127,"PROGRAM",AV_GREEN,1.0);

	DrawTewsDisplay(MpdNum);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 0:
				ToggleTewsMode();
			break;
			case 1:
				ToggleCMDDSDecoyControl();
			break;
			case 2:
				DamageCMDDS();
			break;
			case 3:
				CMDDSTransmitOn();
			break;
			case 4:
				CMDDSTransmitOff();
			break;
			case 5:
				ToggleTewsProgramStep();
			break;
			case 6:
				ToggleTewsProgramEdit();
			break;
			case 9:
				ToggleHarmOverride();
			break;
			case 7:
				if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			break;
			case 12:
				ToggleTewsLimit();
			break;
			case 14:
				ToggleTewsThreatPriority();
			break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTewsProgramTable()
{
	TewsProgType *T = &TewsProgs[Av.Tews.CurProg];

	DrawTextAtLoc(WS_TABLE_ROW1_COL1(0,0),"CHAFF",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL2(0,0),"FLARE",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL3(0,0),"RPT",AV_GREEN,1.0);
	DrawTextAtLoc(WS_TABLE_ROW1_COL4(0,0),"INT",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",T->NumChaff);
	DrawTextAtLoc(WS_TABLE_ROW2_COL1(12,0),TmpStr,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",T->NumFlares);
	DrawTextAtLoc(WS_TABLE_ROW2_COL2(12,0),TmpStr,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",T->Repeat);
	DrawTextAtLoc(WS_TABLE_ROW2_COL3(12,0),TmpStr,AV_GREEN,1.0);
	sprintf(TmpStr,"%f",T->Interval);
	DrawTextAtLoc(WS_TABLE_ROW2_COL4(12,0),TmpStr,AV_GREEN,1.0);

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	sprintf(TmpStr,"CMDS PROGRAM %d",Av.Tews.CurProg + 1);
	DrawTextAtLoc(WS_TABLE_CENTER(-27,0),TmpStr,AV_GREEN,1.0);

  GrDrawLine(GrBuffFor3D,WS_TABLE_LINE_P1(0,0),WS_TABLE_LINE_P2(0,0),Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsProgramMode(int MpdNum)
{
	if(MpdNum != MPD6)
		DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	if( IsWeaponOnBoard(AGM88_ID) )
	{
		DrawWordAtMpdButton(MpdNum,9,"HRM OVRD",(WeapStores.HarmOverride),AV_GREEN,1.0);
		DrawHarmMode();
	}

	DrawVerticalWordAtButton(MpdNum,0,"CHF",(Av.Tews.ReadyForProg == TEWS_CHAFF_SELECT),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,1,"FLR",(Av.Tews.ReadyForProg == TEWS_FLARE_SELECT),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,2,"RPT",(Av.Tews.ReadyForProg == TEWS_REPEAT_SELECT),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,3,"INT",(Av.Tews.ReadyForProg == TEWS_INTERVAL_SELECT),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,4,"EW",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,12,"DWN",FALSE,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,13,"UP",FALSE,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,"STEP",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,6,"SAVE",FALSE,AV_GREEN,1.0);

	DrawTewsProgramTable();

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 0:  TewsProgToggleChaff();
				       break;
			case 1:  TewsProgToggleFlare();
				       break;
			case 2:  TewsProgToggleRepeat();
				       break;
			case 3:  TewsProgToggleInterval();
				       break;
			case 4:  ToggleTewsProgramExit();
				       break;
			case 5:  ToggleTewsProgramStep();
				       break;
			case 6:  ToggleTewsProgramSave();
				       break;
			case 7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							 break;
			case  9: ToggleHarmOverride();
							 break;
			case 12: TewsProgPageDown();
				       break;
			case 13: TewsProgPageUp();
				       break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void TewsMode(int MpdNum)
{
	if(Av.Tews.EditOn)
		TewsProgramMode(MpdNum);
	else
		TewsEWMode(MpdNum);

	DrawTDCIndicator(MpdNum);
}

//*****************************************************************************************************************************************
// UFC MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

UFCButtonType CNIButtons[] =
{
   {2,1,100,18,-1,-1,FALSE,UFC_NONE,"",UFC_NONE}, // CNI_TEXT

   {2,23,28,26,  15,33, TRUE,UFC_NONE,"1",  ACTION_CNI_1}, // CNI_1
   {38,23,28,26, 51,33, TRUE,UFC_NONE,"2",  ACTION_CNI_2}, // CNI_2
   {74,23,28,26, 87,33, TRUE,UFC_NONE,"3",  ACTION_CNI_3}, // CNI_3
   {2,53,28,26,  15,63, TRUE,UFC_NONE,"4",  ACTION_CNI_4}, // CNI_4
   {38,53,28,26, 51,63, TRUE,UFC_NONE,"5",  ACTION_CNI_5}, // CNI_5
   {74,53,28,26, 87,63, TRUE,UFC_NONE,"6",  ACTION_CNI_6}, // CNI_6
   {2,83,28,26,  15,93, TRUE,UFC_NONE,"7",  ACTION_CNI_7}, // CNI_7
   {38,83,28,26, 51,93, TRUE,UFC_NONE,"8",  ACTION_CNI_8}, // CNI_8
   {74,83,28,26, 87,93, TRUE,UFC_NONE,"9",  ACTION_CNI_9}, // CNI_9
   {38,113,28,26,51,123,TRUE,UFC_NONE,"0",  ACTION_CNI_0}, // CNI_0
   {2,113,28,26, 9,123, TRUE,UFC_NONE,"CLR",ACTION_CNI_CLR}, // CNI_CLR
   {74,113,28,26,82,123,TRUE,UFC_NONE,"ENT",ACTION_CNI_ENT}, // CNI_ENT

   {106,1,32,26,    116,11, TRUE,UFC_NONE,"ILS", ACTION_CNI_ILS}, // CNI_ILS
   {106,29,32,26,   116,39, TRUE,UFC_NONE,"A/P", ACTION_CNI_AP}, // CNI_AP
   {106,57,32,26,   116,63, TRUE,UFC_NONE,"TCN", ACTION_CNI_TCN}, // CNI_TCN
   {106,85,32,26,   116,95, TRUE,UFC_NONE,"IFF", ACTION_CNI_IFF}, // CNI_IFF
   {106,113,32,26,  116,123,TRUE,UFC_NONE,"MDI", ACTION_CNI_MDI}, // CNI_MDI
   {142,1,32,26,    -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK1}, // CNI_BLANK1
   {142,29,32,26,   150,35, TRUE,UFC_NONE,"RALT",ACTION_CNI_RALT}, // CNI_RALT
   {142,57,32,26,   154,67, TRUE,UFC_NONE,"EW",  ACTION_CNI_EW}, // CNI_EW
   {142,85,32,26,   150,95, TRUE,UFC_NONE,"FLIR",ACTION_CNI_FLIR}, // CNI_FLIR
   {142,113,32,26,  -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK2}, // CNI_BLANK2
};



UFCButtonType MDIButtons[] =
{
	{ 2,	2,	14,	22,	3,		10,	TRUE,	UFC_NONE,	"SMS",	ACTION_MDI_SMS},
	{ 2,	29,	14,	24,	3,		39,	TRUE,	UFC_NONE,	"HUD",	ACTION_MDI_HUD},
	{ 2,	58,	14,	24,	2,		65,	TRUE,	UFC_NONE,	"RALT",	ACTION_MDI_RALT},
	{ 2,	87,	14,	24,	8,		90,	FALSE,	MDI_UP,		"",		ACTION_MDI_UP},
	{ 2,	116,14,	24,	8,		137,FALSE,	MDI_DWN,	"",		ACTION_MDI_DWN},
	{ 164,	2,	12,	22, -1,		-1,	FALSE,	UFC_NONE,	"",		ACTION_MDI_BLANK1},
	{ 164,	29,	12,	24,	164,	39,	TRUE,	UFC_NONE,	"EW",	ACTION_MDI_EW},
	{ 164,	58,	12,	24,	164,	68,	TRUE,	UFC_NONE,	"FLR",	ACTION_MDI_FLR},
	{ 164,	87,	12,	24,	164,	97,	TRUE,	UFC_NONE,	"CNI",	ACTION_MDI_CNI},
	{ 164,	116,12,	24,	-1,		-1,	FALSE,	UFC_NONE,	"",		ACTION_MDI_BLANK2},
};


UFCButtonType RaltButtons[] =
{
   {2,1,100,18,-1,-1,FALSE,UFC_NONE,"",UFC_NONE}, // CNI_TEXT

   {2,23,28,26,  15,33, TRUE,UFC_NONE,"1",  ACTION_CNI_1},
   {38,23,28,26, 51,33, TRUE,UFC_NONE,"2",  ACTION_CNI_2},
   {74,23,28,26, 87,33, TRUE,UFC_NONE,"3",  ACTION_CNI_3},
   {2,53,28,26,  15,63, TRUE,UFC_NONE,"4",  ACTION_CNI_4},
   {38,53,28,26, 51,63, TRUE,UFC_NONE,"5",  ACTION_CNI_5},
   {74,53,28,26, 87,63, TRUE,UFC_NONE,"6",  ACTION_CNI_6},
   {2,83,28,26,  15,93, TRUE,UFC_NONE,"7",  ACTION_CNI_7},
   {38,83,28,26, 51,93, TRUE,UFC_NONE,"8",  ACTION_CNI_8},
   {74,83,28,26, 87,93, TRUE,UFC_NONE,"9",  ACTION_CNI_9},
   {38,113,28,26,51,123,TRUE,UFC_NONE,"0",  ACTION_CNI_0},
   {2,113,28,26, 9,123, TRUE,UFC_NONE,"CLR",ACTION_CNI_CLR},
   {74,113,28,26,82,123,TRUE,UFC_NONE,"ENT",ACTION_CNI_ENT},

   {106,1,32,26,    116,11, TRUE,UFC_NONE,"CNI", ACTION_CNI_ILS},
   {106,29,32,26,   116,39, FALSE,CNI_RALT_UP,"", ACTION_CNI_RALT_UP},
   {106,57,32,26,   116,63, TRUE,UFC_NONE,"TCN", ACTION_CNI_TCN},
   {106,85,32,26,   116,95, FALSE,CNI_RALT_DOWN,"", ACTION_CNI_RALT_DOWN},
   {106,113,32,26,  116,123,TRUE,UFC_NONE,"MDI", ACTION_CNI_MDI},
   {142,1,32,26,    -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK1},
   {142,29,32,26,   150,35, TRUE,UFC_NONE,"RALT",ACTION_CNI_RALT},
   {142,57,32,26,   154,67, TRUE,UFC_NONE,"EW",  ACTION_CNI_EW},
   {142,85,32,26,   150,95, TRUE,UFC_NONE,"FLIR",ACTION_CNI_FLIR},
   {142,113,32,26,  -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK2},
};

UFCButtonType APButtons[] =
{
   {2,1,100,18,-1,-1,FALSE,UFC_NONE,"",UFC_NONE}, // CNI_TEXT

   {2,23,28,26,  15,33, TRUE,UFC_NONE,"1",  ACTION_CNI_1},
   {38,23,28,26, 51,33, TRUE,UFC_NONE,"2",  ACTION_CNI_2},
   {74,23,28,26, 87,33, TRUE,UFC_NONE,"3",  ACTION_CNI_3},
   {2,53,28,26,  15,63, TRUE,UFC_NONE,"4",  ACTION_CNI_4},
   {38,53,28,26, 51,63, TRUE,UFC_NONE,"5",  ACTION_CNI_5},
   {74,53,28,26, 87,63, TRUE,UFC_NONE,"6",  ACTION_CNI_6},
   {2,83,28,26,  15,93, TRUE,UFC_NONE,"7",  ACTION_CNI_7},
   {38,83,28,26, 51,93, TRUE,UFC_NONE,"8",  ACTION_CNI_8},
   {74,83,28,26, 87,93, TRUE,UFC_NONE,"9",  ACTION_CNI_9},
   {38,113,28,26,51,123,TRUE,UFC_NONE,"0",  ACTION_CNI_0},
   {2,113,28,26, 9,123, TRUE,UFC_NONE,"CLR",ACTION_CNI_CLR},
   {74,113,28,26,82,123,TRUE,UFC_NONE,"ENT",ACTION_CNI_ENT},

   {106,1,32,26,    114,10, TRUE,UFC_NONE,"BALT",ACTION_AP_BALT},
   {106,29,32,26,   114,39, TRUE,UFC_NONE,"RALT",ACTION_AP_RALT},
   {106,57,32,26,   114,67, TRUE,UFC_NONE,"FPAH",ACTION_AP_FPAH},
   {106,85,32,26,   -1,-1 , TRUE,UFC_NONE,"",    ACTION_CNI_BLANK1},
   {106,113,32,26,  116,123,TRUE,UFC_NONE,"CNI", ACTION_AP_CNI},
   {142,1,32,26,    151,8,  TRUE,UFC_NONE,"CPL", ACTION_AP_CPL},
   {142,29,32,26,   150,39, TRUE,UFC_NONE,"ROLL",ACTION_AP_ROLL},
   {142,57,32,26,   152,67, TRUE,UFC_NONE,"HDG", ACTION_AP_HDG},
   {142,85,32,26,   -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK2},
   {142,113,32,26,  -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK2},
};

UFCButtonType TacanButtons[] =
{
   {2,1,100,18,-1,-1,FALSE,UFC_NONE,"",UFC_NONE}, // CNI_TEXT

   {106,1,32,26,    116,11, TRUE,UFC_NONE,"CNI", ACTION_CNI_ILS},
   {106,29,32,26,   116,39, FALSE,CNI_TACAN_UP,"", ACTION_TACAN_UP},
   {106,57,32,26,   116,63, TRUE,UFC_NONE,"TCN", ACTION_CNI_TCN},
   {106,85,32,26,   116,95, FALSE,CNI_TACAN_DOWN,"", ACTION_TACAN_DOWN},
   {106,113,32,26,  116,123,TRUE,UFC_NONE,"MDI", ACTION_CNI_MDI},
   {142,1,32,26,    -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK1},
   {142,29,32,26,   150,35, TRUE,UFC_NONE,"RALT",ACTION_CNI_RALT},
   {142,57,32,26,   154,67, TRUE,UFC_NONE,"EW",  ACTION_CNI_EW},
   {142,85,32,26,   150,95, TRUE,UFC_NONE,"FLIR",ACTION_CNI_FLIR},
   {142,113,32,26,  -1,-1,  TRUE,UFC_NONE,"",    ACTION_CNI_BLANK2},
};


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitUfc()
{
	Av.Ufc.Keypad[0] = '_';
	Av.Ufc.Keypad[1] = 0x00;
	Av.Ufc.Mode = UFC_CNI_MODE;
	Av.Ufc.RaltVal = 500;

	Av.Ufc.TacanStart = 0;
	Av.Ufc.TacanStop  = (g_iNumTACANObjects-1 <= 8) ? g_iNumTACANObjects-1 : 8;
	Av.Ufc.CurTacan   = 0;

	if(g_iNumTACANObjects > 0)
	    SetSpecificTacanStation( (pTACANTypeList + Av.Ufc.CurTacan) );
	else
		UFC.CurrentTacanPtr = NULL;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawSquarePoly(float X,float Y,float Width,float Height,int Color,float Shade)
{
	float Xs[4];
	float Ys[4];
	FPointData Colors[4];

	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple,TRUE);

	for(int i=0; i<4; i++)
	{
		Colors[i].X = Triple.Red;
		Colors[i].Y = Triple.Green;
		Colors[i].Z = Triple.Blue;
	}

	Xs[0] = X; Xs[1] = X + Width-1; Xs[2] = X + Width-1; Xs[3] = X;
	Ys[0] = Y; Ys[1]  = Y;          Ys[2] = Y+Height-1; Ys[3] = Y+Height-1;

	GrDrawRampedPolyBuff(GrBuffFor3D,4,Xs,Ys,Colors,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
int MDIRaltOn = 0;

void DrawMDIButtons(int MpdNum)
{

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	UFCButtonType *B = &MDIButtons[0];

	float X,Y;

	int CursorInside = CursorInMpd( MpdNum,&X,&Y);



	int Index = 10;
	while(Index-- > 0)
	{
		if( CursorInside && !((B->Action == ACTION_MDI_BLANK1) || (B->Action == ACTION_MDI_BLANK2)) )
		{
			if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
				DrawSquarePoly(B->ULX,B->ULY,B->Width,B->Height,AV_GREEN,0.6);
		}


		if(  MDIRaltOn  && (B->Action == ACTION_MDI_RALT))
			DrawSquarePoly(B->ULX,B->ULY,B->Width,B->Height,AV_GREEN,0.6);


		DrawBox(GrBuffFor3D, B->ULX,B->ULY,B->Width-1,B->Height-1,Color.Red,Color.Green,Color.Blue);

		if(B->IsString)
		{
		  DrawTextAtLoc(B->SULX,B->SULY,B->String,AV_GREEN,1.0);
			if(B->Action == ACTION_MDI_RALT)
			{
				itoa(Av.Ufc.RaltVal,TmpStr,10);
				DrawTextAtLoc(B->SULX,B->SULY+8,TmpStr,AV_GREEN,1.0);
			}
		}
		else
		{
  		AvRGB Triple;
 			GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

			if(B->Symbol == MDI_UP)
			{
				GrDrawLine(GrBuffFor3D,8,90,5,93,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,8,90,11,93,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,8,90,8,103,Triple.Red,Triple.Green,Triple.Blue,0);
			}
			else if(B->Symbol == MDI_DWN)
			{
				GrDrawLine(GrBuffFor3D,8,137,5,134,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,8,137,11,134,Triple.Red,Triple.Green,Triple.Blue,0);
				GrDrawLine(GrBuffFor3D,8,137,8,124,Triple.Red,Triple.Green,Triple.Blue,0);
			}
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDISms()
{
	Av.Ufc.MdiMode  = STORES_MODE;
	Mpds[MPD6].Mode = STORES_MODE;

	Mpds[MPD6].ProgModeInfo.ModeList[0] = STORES_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[1] = STORES_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[2] = STORES_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[3] = STORES_MODE;

	/*
	DefaultMpdProgrammingMatrix[MPD6][0] = STORES_MODE;
	DefaultMpdProgrammingMatrix[MPD6][1] = STORES_MODE;
	DefaultMpdProgrammingMatrix[MPD6][2] = STORES_MODE;
	DefaultMpdProgrammingMatrix[MPD6][3] = STORES_MODE;
	 */
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDIHud()
{
	Av.Ufc.MdiMode  = HUD_MODE;
	Mpds[MPD6].Mode = HUD_MODE;

  Mpds[MPD6].ProgModeInfo.ModeList[0] = HUD_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[1] = HUD_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[2] = HUD_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[3] = HUD_MODE;

	/*
	DefaultMpdProgrammingMatrix[MPD6][0] = HUD_MODE;
	DefaultMpdProgrammingMatrix[MPD6][1] = HUD_MODE;
	DefaultMpdProgrammingMatrix[MPD6][2] = HUD_MODE;
	DefaultMpdProgrammingMatrix[MPD6][3] = HUD_MODE;
	 * */
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDIRalt()
{
	Av.Ufc.Mode = UFC_RALT_MODE;

	//need to higlight button..thats it...nothing else


	//if(TDCMpdNum == MPD6)
	// TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDIUp()
{
  if(Av.Ufc.RaltVal + 50 < 10000)
	    Av.Ufc.RaltVal += 50;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDIDown()
{
  if(Av.Ufc.RaltVal - 50 >= 0)
	    Av.Ufc.RaltVal -= 50;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDIEw()
{
	Av.Ufc.MdiMode  = TEWS_MODE;
  Mpds[MPD6].Mode = TEWS_MODE;

  Mpds[MPD6].ProgModeInfo.ModeList[0] = TEWS_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[1] = TEWS_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[2] = TEWS_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[3] = TEWS_MODE;

	/*
  DefaultMpdProgrammingMatrix[MPD6][0] = TEWS_MODE;
  DefaultMpdProgrammingMatrix[MPD6][1] = TEWS_MODE;
  DefaultMpdProgrammingMatrix[MPD6][2] = TEWS_MODE;
  DefaultMpdProgrammingMatrix[MPD6][3] = TEWS_MODE;
	 * */
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDIFlr()
{
	if(!Av.Weapons.HasTargetIR) return;

	Av.Ufc.MdiMode  = TGT_IR_MODE;
  Mpds[MPD6].Mode = TGT_IR_MODE;

  Mpds[MPD6].ProgModeInfo.ModeList[0] = TGT_IR_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[1] = TGT_IR_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[2] = TGT_IR_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[3] = TGT_IR_MODE;

 //	DefaultMpdProgrammingMatrix[MPD6][0] = TGT_IR_MODE;
 // DefaultMpdProgrammingMatrix[MPD6][1] = TGT_IR_MODE;
 // DefaultMpdProgrammingMatrix[MPD6][2] = TGT_IR_MODE;
 // DefaultMpdProgrammingMatrix[MPD6][3] = TGT_IR_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoMDICni()
{
  Av.Ufc.Mode = UFC_CNI_MODE;
	if(TDCMpdNum == MPD6)
	  TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessMDIButtonClick(int X, int Y)
{
	UFCButtonType *B = &MDIButtons[0];

	int Index = 10;
	while(Index-- > 0)
	{
    if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
		{
				 switch(B->Action)
				 {
				 		case  ACTION_MDI_SMS:    MDIRaltOn = 0; DoMDISms(); break;
				 		case  ACTION_MDI_HUD:    MDIRaltOn = 0; DoMDIHud(); break;
				 		case  ACTION_MDI_RALT:   MDIRaltOn = !MDIRaltOn; break;//DoMDIRalt();   break;

				 		case  ACTION_MDI_UP:
							if( MDIRaltOn )
								DoRaltUp();
							break;

				 		case  ACTION_MDI_DWN:
							if( MDIRaltOn )
								 DoRaltDown();
							break;

				 		case  ACTION_MDI_EW:     MDIRaltOn = 0; DoMDIEw();   break;
				 		case  ACTION_MDI_FLR:    MDIRaltOn = 0; DoMDIFlr();  break;
				 		case  ACTION_MDI_CNI:    MDIRaltOn = 0; DoMDICni();    break;
				 }
				 return;
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawRaltButtons(int MpdNum)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	UFCButtonType *B = &RaltButtons[0];

	float X,Y;
	int CursorInside = CursorInMpd(MpdNum,&X,&Y);

	int Index = 23;
	while(Index-- > 0)
	{
		if(CursorInside && !((B->Action == ACTION_CNI_BLANK1) || (B->Action == ACTION_CNI_BLANK2) || (B->Action == UFC_NONE)) )
		{
			if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
			{
          DrawSquarePoly(B->ULX,B->ULY,B->Width,B->Height,AV_GREEN,0.6);
			}
		}

		DrawBox(GrBuffFor3D, B->ULX,B->ULY,B->Width-1,B->Height-1,Triple.Red,Triple.Green,Triple.Blue);

		if(B->Action == ACTION_CNI_RALT)
		{
		   DrawBox(GrBuffFor3D, B->ULX+1,B->ULY+1,B->Width-3,B->Height-3,Triple.Red,Triple.Green,Triple.Blue);
			 itoa(Av.Ufc.RaltVal,TmpStr,10);
			 DrawTextAtLoc(B->SULX,B->SULY+8,TmpStr,AV_GREEN,1.0);
		}

		if(B->Action == ACTION_CNI_TCN)
		{
			 if(UFC.CurrentTacanPtr)
			 {
				 sprintf(TmpStr,"%d",UFC.CurrentTacanPtr->lChannel);
				 DrawTextAtLoc(B->SULX+2,B->SULY+8,TmpStr,AV_GREEN,1.0,TRUE);

			 }
		}

		if(B->IsString)
		  DrawTextAtLoc(B->SULX,B->SULY,B->String,AV_GREEN,1.0,TRUE);
		else
		{
			if(B->Symbol == CNI_RALT_UP)
			{
	      GrDrawLine(GrBuffFor3D,122,35,126,39,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,35,118,39,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,35,122,49,Triple.Red,Triple.Green,Triple.Blue,0);
			}
			else if(B->Symbol == CNI_RALT_DOWN)
			{
	      GrDrawLine(GrBuffFor3D,122,105,118,101,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,105,126,101,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,105,122,91,Triple.Red,Triple.Green,Triple.Blue,0);
			}
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTacanButtons(int MpdNum)
{

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	UFCButtonType *B = &TacanButtons[0];

	float X,Y;
	int CursorInside = CursorInMpd(MpdNum,&X,&Y);

	int Index = 11;
	while(Index-- > 0)
	{
		if( CursorInside && !((B->Action == ACTION_CNI_BLANK1) || (B->Action == ACTION_CNI_BLANK2) || (B->Action == UFC_NONE)) )
		{
			if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
			{
          DrawSquarePoly(B->ULX,B->ULY,B->Width,B->Height,AV_GREEN,0.6);
			}
		}

		DrawBox(GrBuffFor3D, B->ULX,B->ULY,B->Width-1,B->Height-1,Triple.Red,Triple.Green,Triple.Blue);

		if(B->Action == ACTION_CNI_TCN)
		{
		   DrawBox(GrBuffFor3D, B->ULX+1,B->ULY+1,B->Width-3,B->Height-3,Triple.Red,Triple.Green,Triple.Blue);
			 if(UFC.CurrentTacanPtr)
			 {
				 sprintf(TmpStr,"%d",UFC.CurrentTacanPtr->lChannel);
				 DrawTextAtLoc(B->SULX+2,B->SULY+8,TmpStr,AV_GREEN,1.0,TRUE);
			 }
		}

		if(B->Action == ACTION_CNI_RALT)
		{
			 itoa(Av.Ufc.RaltVal,TmpStr,10);
		   DrawTextAtLoc(B->SULX,B->SULY+8,TmpStr,AV_GREEN,1.0,TRUE);
		}

		if(B->IsString)
		  DrawTextAtLoc(B->SULX,B->SULY,B->String,AV_GREEN,1.0,TRUE);
		else
		{
			if(B->Symbol == CNI_TACAN_UP)
			{
	      GrDrawLine(GrBuffFor3D,122,35,126,39,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,35,118,39,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,35,122,49,Triple.Red,Triple.Green,Triple.Blue,0);
			}
			else if(B->Symbol == CNI_TACAN_DOWN)
			{
	      GrDrawLine(GrBuffFor3D,122,105,118,101,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,105,126,101,Triple.Red,Triple.Green,Triple.Blue,0);
	      GrDrawLine(GrBuffFor3D,122,105,122,91,Triple.Red,Triple.Green,Triple.Blue,0);
			}
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawAPButtons(int MpdNum)
{
	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	UFCButtonType *B = &APButtons[0];

	float X,Y;
	int CursorInside = CursorInMpd(MpdNum,&X,&Y);

	int Index = 23;
	while(Index-- > 0)
	{
		if( CursorInside && !((B->Action == ACTION_CNI_BLANK1) || (B->Action == ACTION_CNI_BLANK2) || (B->Action == UFC_NONE)) )
		{
			if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
			{
          DrawSquarePoly(B->ULX,B->ULY,B->Width,B->Height,AV_GREEN,0.6);
			}
		}

		DrawBox(GrBuffFor3D, B->ULX,B->ULY,B->Width-1,B->Height-1,Color.Red,Color.Green,Color.Blue);

		int Highlight = FALSE;
		switch(B->Action)
		{
			case ACTION_AP_BALT:  if(PlayerPlane->AutoPilotMode & PL_AP_ALT_BARO) Highlight = TRUE; break;
			case ACTION_AP_RALT:  if(PlayerPlane->AutoPilotMode & PL_AP_ALT_RDR) Highlight  = TRUE; break;
			case ACTION_AP_FPAH:  if(PlayerPlane->AutoPilotMode & PL_AP_ATTITUDE) Highlight = TRUE; break;
			case ACTION_AP_CPL :  if(PlayerPlane->AutoPilotMode & PL_AP_CPL) Highlight = TRUE;      break;
			case ACTION_AP_ROLL:  if(PlayerPlane->AutoPilotMode & PL_AP_ROLL) Highlight = TRUE;     break;
			case ACTION_AP_HDG :  if(PlayerPlane->AutoPilotMode & PL_AP_HDG) Highlight = TRUE;      break;
	  }

		if(Highlight)
		  DrawBox(GrBuffFor3D, B->ULX+1,B->ULY+1,B->Width-3,B->Height-3,Color.Red,Color.Green,Color.Blue);

		if(B->Action == ACTION_AP_CPL)
		{
			switch(Av.Hsi.SteeringMode)
			{
				case HSI_TCN_MODE: sprintf(TmpStr,"TCN"); break;
				case HSI_TGT_MODE: sprintf(TmpStr,"TGT"); break;
				case HSI_ACL_MODE: sprintf(TmpStr,"ACL"); break;
				case HSI_WPT_MODE: sprintf(TmpStr,"WPT"); break;
				case HSI_GPS_MODE: sprintf(TmpStr,"GPS"); break;
			}
		  DrawTextAtLoc(B->SULX+1,B->SULY+8,TmpStr,AV_GREEN,1.0,TRUE);
		}

		if(B->IsString)
		  DrawTextAtLoc(B->SULX,B->SULY,B->String,AV_GREEN,1.0,TRUE);

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawCNIButtons(int MpdNum)
{
	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	UFCButtonType *B = &CNIButtons[0];

	float X,Y;
	int CursorInside = CursorInMpd(MpdNum,&X,&Y);

	int Index = 23;
	while(Index-- > 0)
	{
		if( CursorInside && !((B->Action == ACTION_CNI_BLANK1) || (B->Action == ACTION_CNI_BLANK2) || (B->Action == UFC_NONE)) )
		{
			if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
			{
          DrawSquarePoly(B->ULX,B->ULY,B->Width,B->Height,AV_GREEN,0.6);
			}
		}

		if(B->Action == ACTION_CNI_RALT)
		{
			itoa(Av.Ufc.RaltVal,TmpStr,10);
			DrawTextAtLoc(B->SULX,B->SULY+8,TmpStr,AV_GREEN,1.0);
		}

		if(B->Action == ACTION_CNI_TCN)
		{
			 if(UFC.CurrentTacanPtr)
			 {
				 sprintf(TmpStr,"%d",UFC.CurrentTacanPtr->lChannel);
				 DrawTextAtLoc(B->SULX+2,B->SULY+8,TmpStr,AV_GREEN,1.0,TRUE);
			 }
		}

		DrawBox(GrBuffFor3D, B->ULX,B->ULY,B->Width-1,B->Height-1,Color.Red,Color.Green,Color.Blue);

		if(B->IsString)
		  DrawTextAtLoc(B->SULX,B->SULY,B->String,AV_GREEN,1.0,TRUE);


		//TW: ils and A/P Fix Tw 10/11
		if( B->Action == ACTION_CNI_ILS  && UFC.ILSStatus)
	        DrawSquarePoly(B->ULX,B->ULY, B->Width*0.20f,B->Height*0.20f,AV_GREEN,0.6);
		if( B->Action == ACTION_CNI_AP && (UFC.APStatus) )//(PlayerPlane->Status & AL_AI_DRIVEN)  )
	        DrawSquarePoly(B->ULX,B->ULY, B->Width*0.20f,B->Height*0.20f,AV_GREEN,0.6);


		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNINumber(int Action)
{
	int Number;

	if(Action == ACTION_CNI_0)
		Number = 0;
	else
		Number = Action+1;

	int Length = strlen(Av.Ufc.Keypad);

	if(Av.Ufc.Keypad[Length-1] != '_') return;

	if(Length < 5)
	{
		itoa(Number,&Av.Ufc.Keypad[Length-1],10);
		Av.Ufc.Keypad[Length]     = '_';
		Av.Ufc.Keypad[Length+1] = 0x00;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIClear()
{
	int Length = strlen(Av.Ufc.Keypad);

	if(Av.Ufc.Keypad[Length - 1] != '_')
	{
		Av.Ufc.Keypad[0] = '_';
		Av.Ufc.Keypad[1] = 0x00;
		return;
	}

	if(Length == 1) return;

	Av.Ufc.Keypad[Length-1] = 0x00;
	Av.Ufc.Keypad[Length-2] = '_';
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIEnter()
{
	if(  Av.Ufc.Mode != UFC_RALT_MODE )
		return;						// only accept # if in RALT MODE

	int Length = strlen(Av.Ufc.Keypad);

	if(Length <= 1) return;

	if(Av.Ufc.Keypad[Length-1] == '_')
	{
		Av.Ufc.Keypad[Length-1] = 0x00;
		Av.Ufc.RaltVal = atoi(Av.Ufc.Keypad);
		Av.Ufc.Keypad[0] = '_';
		Av.Ufc.Keypad[1] = 0x00;	//clear after enter
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIILS()
{
  DoUFCMainIls();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIAP()
{
  Av.Ufc.Mode = UFC_AP_MODE;
	if(TDCMpdNum == MPD6)
	  TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNITCN()
{
  Av.Ufc.Mode = UFC_TACAN_MODE;
	if(TDCMpdNum == MPD6)
	  TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIIFF()
{
  InterrogateIffPlanes();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIMDI()
{
  Av.Ufc.Mode = UFC_MDI_MODE;
	if(TDCMpdNum == MPD6)
	  TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIRALT()
{
  Av.Ufc.Mode = UFC_RALT_MODE;
	//if(TDCMpdNum == MPD6)
	//  TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIEW()
{
  Av.Ufc.Mode = UFC_MDI_MODE;
	Av.Ufc.MdiMode = TEWS_MODE;

  Mpds[MPD6].Mode = TEWS_MODE;

	Mpds[MPD6].ProgModeInfo.ModeList[0] = TEWS_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[1] = TEWS_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[2] = TEWS_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[3] = TEWS_MODE;

	/*
  DefaultMpdProgrammingMatrix[MPD6][0] = TEWS_MODE;
  DefaultMpdProgrammingMatrix[MPD6][1] = TEWS_MODE;
  DefaultMpdProgrammingMatrix[MPD6][2] = TEWS_MODE;
  DefaultMpdProgrammingMatrix[MPD6][3] = TEWS_MODE;
	 */

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIFlir()
{
	if(!Av.Weapons.HasTargetIR) return;

  Av.Ufc.Mode = UFC_MDI_MODE;
	Av.Ufc.MdiMode = TGT_IR_MODE;

  Mpds[MPD6].Mode = TGT_IR_MODE;

	Mpds[MPD6].ProgModeInfo.ModeList[0] = TGT_IR_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[1] = TGT_IR_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[2] = TGT_IR_MODE;
  Mpds[MPD6].ProgModeInfo.ModeList[3] = TGT_IR_MODE;

	/*
  DefaultMpdProgrammingMatrix[MPD6][0] = TGT_IR_MODE;
  DefaultMpdProgrammingMatrix[MPD6][1] = TGT_IR_MODE;
  DefaultMpdProgrammingMatrix[MPD6][2] = TGT_IR_MODE;
  DefaultMpdProgrammingMatrix[MPD6][3] = TGT_IR_MODE;
	 */
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoRaltUp()
{

	 if(Av.Ufc.RaltVal + 50 < 10000)
	    Av.Ufc.RaltVal += 50;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoRaltDown()
{
  if(Av.Ufc.RaltVal - 50 >= 0)
	    Av.Ufc.RaltVal -= 50;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPCpl()
{
	if( !(PlayerPlane->AutoPilotMode & PL_AP_CPL) )
	{
		PlayerPlane->AutoPilotMode |= PL_AP_CPL;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ROLL;
		PlayerPlane->AutoPilotMode &= ~PL_AP_HDG;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPBalt()
{
	if( !(PlayerPlane->AutoPilotMode & PL_AP_ALT_BARO) )
	{
		PlayerPlane->AutoPilotMode |= PL_AP_ALT_BARO;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ALT_RDR;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ATTITUDE;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPRalt()
{
	if( !(PlayerPlane->AutoPilotMode & PL_AP_ALT_RDR) )
	{
		PlayerPlane->AutoPilotMode |= PL_AP_ALT_RDR;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ALT_BARO;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ATTITUDE;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPFpah()
{
	if( !(PlayerPlane->AutoPilotMode & PL_AP_ATTITUDE) )
	{
		PlayerPlane->AutoPilotMode |= PL_AP_ATTITUDE;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ALT_BARO;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ALT_RDR;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPCni()
{
  Av.Ufc.Mode = UFC_CNI_MODE;
	if(TDCMpdNum == MPD6)
	  TDCMpdNum = 1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPRoll()
{
	if( !(PlayerPlane->AutoPilotMode & PL_AP_ROLL) )
	{
		PlayerPlane->AutoPilotMode |= PL_AP_ROLL;
		PlayerPlane->AutoPilotMode &= ~PL_AP_CPL;
		PlayerPlane->AutoPilotMode &= ~PL_AP_HDG;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPHdg()
{
	if( !(PlayerPlane->AutoPilotMode & PL_AP_HDG) )
	{
		PlayerPlane->AutoPilotMode |= PL_AP_HDG;
		PlayerPlane->AutoPilotMode &= ~PL_AP_ROLL;
		PlayerPlane->AutoPilotMode &= ~PL_AP_CPL;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoTacanUp()
{
	if(g_iNumTACANObjects <= 0) return;

	if( (Av.Ufc.CurTacan-1 >= 0) )
		Av.Ufc.CurTacan--;

	if(Av.Ufc.CurTacan < Av.Ufc.TacanStart)
	{
		Av.Ufc.TacanStart--;
		Av.Ufc.TacanStop--;
	}

  SetSpecificTacanStation( (pTACANTypeList + Av.Ufc.CurTacan) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoTacanDown()
{
    if(g_iNumTACANObjects <= 0) return;

	if( (Av.Ufc.CurTacan+1 < g_iNumTACANObjects) )
		Av.Ufc.CurTacan++;

	if(Av.Ufc.CurTacan > Av.Ufc.TacanStop)
	{
		Av.Ufc.TacanStart++;
		Av.Ufc.TacanStop++;
	}

  SetSpecificTacanStation( (pTACANTypeList + Av.Ufc.CurTacan) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessTacanButtonClick(int X, int Y)
{
	UFCButtonType *B = &TacanButtons[0];

	int Index = 11;
	while(Index-- > 0)
	{
    if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
		{
				 switch(B->Action)
				 {
					case ACTION_CNI_ILS: DoMDICni();   break;
					case ACTION_TACAN_UP: DoTacanUp();    break;
					case ACTION_TACAN_DOWN: DoTacanDown(); break;
					case ACTION_CNI_MDI: DoCNIMDI();   break;
					case ACTION_CNI_RALT: DoCNIRALT();    break;
					case ACTION_CNI_EW: DoCNIEW();  break;
					case ACTION_CNI_FLIR: DoCNIFlir(); break;
				 }
	 			 return;
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessRaltButtonClick(int X, int Y)
{
	UFCButtonType *B = &RaltButtons[0];

	int Index = 23;
	while(Index-- > 0)
	{
    if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
		{
			if( (B->Action >= ACTION_CNI_1) && (B->Action <= ACTION_CNI_0) )
			{
				DoCNINumber(B->Action);
			}
			else
			{
				 switch(B->Action)
				 {
   				 case ACTION_CNI_CLR:       DoCNIClear(); break;
   				 case ACTION_CNI_ENT:       DoCNIEnter(); break;
   				 case ACTION_CNI_ILS:       DoMDICni();   break;
   				 case ACTION_CNI_RALT_UP :  DoRaltUp();    break;
   				 case ACTION_CNI_TCN:       DoCNITCN();   break;
   				 case ACTION_CNI_RALT_DOWN: DoRaltDown(); break;
   				 case ACTION_CNI_MDI:       DoCNIMDI();   break;
   				 case ACTION_CNI_EW:        DoCNIEW();    break;
   				 case ACTION_CNI_FLIR:      DoCNIFlir();  break;
				 }

				 return;
			}
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessAPButtonClick(int X, int Y)
{
	UFCButtonType *B = &APButtons[0];

	int Index = 23;
	while(Index-- > 0)
	{
    if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
		{
			if( (B->Action >= ACTION_CNI_1) && (B->Action <= ACTION_CNI_0) )
			{
				DoCNINumber(B->Action);
			}
			else
			{
				 switch(B->Action)
				 {
   				 case ACTION_CNI_CLR:  DoCNIClear(); break;
   				 case ACTION_CNI_ENT:  DoCNIEnter(); break;
   				 case ACTION_AP_CPL:   DoAPCpl();   break;
   				 case ACTION_AP_BALT:  DoAPBalt();    break;
   				 case ACTION_AP_RALT:  DoAPRalt();   break;
   				 case ACTION_AP_FPAH:  DoAPFpah();   break;
   				 case ACTION_AP_CNI:   DoAPCni();   break;
   				 case ACTION_AP_ROLL:  DoAPRoll();    break;
 				 case ACTION_AP_HDG:   DoAPHdg();  break;
				 }
				 return;
			}
		}

		B++;
	}
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessCNIButtonClick(int X, int Y)
{
	UFCButtonType *B = &CNIButtons[0];

	int Index = 23;
	while(Index-- > 0)
	{
    if( IsPointInside(X,Y,B->ULY,B->ULY + B->Height,B->ULX,B->ULX+B->Width) )
		{
			if( (B->Action >= ACTION_CNI_1) && (B->Action <= ACTION_CNI_0) )
			{
				DoCNINumber(B->Action);
			}
			else
			{
				 switch(B->Action)
				 {
   				 case ACTION_CNI_CLR:  DoCNIClear(); break;
   				 case ACTION_CNI_ENT:  DoCNIEnter(); break;
   				 case ACTION_CNI_ILS:  DoCNIILS();   break;
   				 case ACTION_CNI_AP :  DoCNIAP();    break;
   				 case ACTION_CNI_TCN:  DoCNITCN();   break;
   				 case ACTION_CNI_IFF:  DoCNIIFF();   break;
   				 case ACTION_CNI_MDI:  DoCNIMDI();   break;
   				 case ACTION_CNI_RALT: DoCNIRALT();  break;
   				 case ACTION_CNI_EW:   DoCNIEW();    break;
   				 case ACTION_CNI_FLIR: DoCNIFlir();  break;
				 }

				 return;
			}
		}

		B++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTacanText(int MpdNum)
{
 if(g_iNumTACANObjects <= 0) return;

 sprintf(TmpStr,"%d %s",(pTACANTypeList + Av.Ufc.CurTacan)->lChannel,(pTACANTypeList + Av.Ufc.CurTacan)->sLabel);
 DrawTextAtLoc(10,7,TmpStr ,AV_GREEN,1.0,TRUE);

 if(Av.Ufc.TacanStop > 8)
  DrawTextAtLoc(7,24,"MORE...",AV_GREEN,1.0,TRUE);

 if(Av.Ufc.TacanStop < g_iNumTACANObjects-1)
  DrawTextAtLoc(7,124,"MORE...",AV_GREEN,1.0,TRUE);

 TACANType *TWalk = pTACANTypeList + Av.Ufc.TacanStart;
 TACANType *TEnd  = pTACANTypeList + Av.Ufc.TacanStop;

 int XPos = 13;
 int YPos = 34;

 while(TWalk <= TEnd)
 {
	 if(TWalk - pTACANTypeList == Av.Ufc.CurTacan)
	 {
      DrawSquarePoly(2,YPos,100,10,AV_GREEN,0.9);
			sprintf(TmpStr,"%d %s",TWalk->lChannel,TWalk->sLabel);
      DrawTextAtLoc(XPos,YPos,TmpStr,AV_GREEN,0.4,TRUE);
	 }
	 else
	 {
			sprintf(TmpStr,"%d %s",TWalk->lChannel,TWalk->sLabel);
      DrawTextAtLoc(XPos,YPos,TmpStr,AV_GREEN,1.0,TRUE);
	 }

	 TWalk++;
	 YPos += 10;
 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoCNIMode(int MpdNum)
{
  DrawCNIButtons(MpdNum);

  DrawTextAtLoc(10,9,Av.Ufc.Keypad,AV_GREEN,1.0,TRUE);

	int X,Y;
  if( GetMpdClick(MpdNum,&X,&Y) )
	{
	  ProcessCNIButtonClick(X,Y);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


void DoMDIMode(int MpdNum)
{
	DrawMDIButtons(MpdNum);

	int X,Y;
	if( GetMpdClick(MpdNum,&X,&Y) )
	{
		ProcessMDIButtonClick(X,Y);
	}
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoTACANMode(int MpdNum)
{
	DrawTacanText(MpdNum);

	DrawTacanButtons(MpdNum);

	int X,Y;
  if( GetMpdClick(MpdNum,&X,&Y) )
	{
	  ProcessTacanButtonClick(X,Y);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoAPMode(int MpdNum)
{
	DrawTextAtLoc(10,9,Av.Ufc.Keypad,AV_GREEN,1.0,TRUE);

	DrawAPButtons(MpdNum);

	int X,Y;
	if( GetMpdClick(MpdNum,&X,&Y) )
		ProcessAPButtonClick(X,Y);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoRALTMode(int MpdNum)
{
	DrawTextAtLoc(10,9,Av.Ufc.Keypad,AV_GREEN,1.0,TRUE);
	DrawRaltButtons(MpdNum);

	int X,Y;
	if( GetMpdClick(MpdNum,&X,&Y) )
	  ProcessRaltButtonClick(X,Y);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void UFCMode(int MpdNum)
{
	switch(Av.Ufc.Mode)
	{
		case UFC_CNI_MODE  : DoCNIMode(MpdNum);   break;
		case UFC_MDI_MODE  : DoMDIMode(MpdNum);   break;
		case UFC_TACAN_MODE: DoTACANMode(MpdNum); break;
		case UFC_AP_MODE   : DoAPMode(MpdNum);    break;
		case UFC_RALT_MODE : DoRALTMode(MpdNum);  break;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

typedef struct
{
  int X;
  int Y;
} UFCClickType;

UFCClickType UFCClickButtons[] =
{
 {0,17},
 {0,40},
 {0,64},
 {0,86},
 {0,109},

 {17,130},
 {40,130},
 {63,130},
 {86,130},
 {109,130},

 {130,109},
 {130,86},
 {130,64},
 {130,40},
 {130,17},

 {109,0},
 {86, 0},
 {63, 0},
 {40, 0},
 {17, 0},

 {-1,-1}
};

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawUFCClickButtons()
{
	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	UFCClickType *Ptr = &UFCClickButtons[0];
	while(Ptr->X != -1)
	{
		if( (Ptr - &UFCClickButtons[0] < 5) || ( (Ptr - &UFCClickButtons[0] < 15) && (Ptr - &UFCClickButtons[0] >= 10))   )
	     DrawBox(GrBuffFor3D,Ptr->X,Ptr->Y,10,14,Color.Red,Color.Green,Color.Blue);
		else
	     DrawBox(GrBuffFor3D,Ptr->X,Ptr->Y,14,10,Color.Red,Color.Green,Color.Blue);

		Ptr++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetUFCClickButton(float X, float Y)
{
	int W,H;

	UFCClickType *Ptr = &UFCClickButtons[0];
	while(Ptr->X != -1)
	{
			if( (Ptr - &UFCClickButtons[0] < 5) || ( (Ptr - &UFCClickButtons[0] < 15) && (Ptr - &UFCClickButtons[0] >= 10))   )
			{
				W = 10;
				H = 14;
			}
			else
			{
				W = 10;
				H = 14;
			}

			int Inside = IsPointInside(X,Y,Ptr->Y,Ptr->Y + H,Ptr->X,Ptr->X + W);
			if(Inside)
			{
				return(Ptr - &UFCClickButtons[0]);
			}

		  Ptr++;
	}

	return(-1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CheckUFCMpdClick(int MpdNum, float X, float Y)
{
	 if(MpdNum != MPD6) return;
	 if(Av.Ufc.Mode != UFC_MDI_MODE) return;

   int Button = GetUFCClickButton(X,Y);

	 if(Button != -1)
	 {
		 MpdButtonPress(MPD6,Button);
	 }
}

//*****************************************************************************************************************************************
// HSI  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void InitGPSLabels()
{
	if(g_iNumMissionLabels <= 0)
	{
		AvNumGpsLabels = 0;
		return;
	}

	AvNumGpsLabels = 0;

	MissionLabelType *Walk = pMissionLabelList;
	int Index = g_iNumMissionLabels;
	while(Index-- > 0)
	{
		if(Walk->lOverviewMapFlag != LABEL_TYPE_CHECKLIST)
		{
			if(AvNumGpsLabels + 1 < AV_MAX_GPS_LABELS)
			{
			 	if(Walk->lGPSLabel)
			 	{
				 	AvNumGpsLabels++;
				 	AvGpsLabels[AvNumGpsLabels-1] = Walk;
			 	}
			}
		}
		Walk++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitHsi()
{
  InitGPSLabels();

	Av.Hsi.ShowMap = FALSE;
	Av.Hsi.SteeringMode = HSI_WPT_MODE;
	Av.Hsi.DisplayMode = HSI_TUP;
	Av.Hsi.SensorsOn = FALSE;
	Av.Hsi.TimeOn    = LOCAL_TIME;
	Av.Hsi.AutoOn    = TRUE;
	Av.Hsi.SequenceOn = FALSE;
	Av.Hsi.DataOn     = FALSE;
	Av.Hsi.CselOn     = FALSE;
	Av.Hsi.Range      = 20.0;
	Av.Hsi.DataTime   = LOCAL_TIME;
	Av.Hsi.HSelect    = 0;
	Av.Hsi.CSelect    = 0;
	Av.Hsi.CurGps     = (AvNumGpsLabels > 0) ? 0 : -1;

	Av.Hsi.Origin.X = 70;
	Av.Hsi.Origin.Z = 69;
	Av.Hsi.Origin.Y = 0;

	Av.Hsi.RadiusInPixels = 53.0;
	Av.Hsi.PixelsPerMile  = Av.Hsi.RadiusInPixels/Av.Hsi.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetAssociatedCarrier(FPointDouble *Loc)
{
	 FPointDouble Temp;

	 if(PlayerPlane->AI.iHomeBaseId >= 0)
	 {
	   MovingVehicleParams *Carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
		 GetLandingPoint(Carrier,Temp);
		 *Loc = Temp;
		 return(TRUE);
	 }

	 return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetCoupledModePosition(FPointDouble *Loc)
{
 int RetVal = TRUE;

 FPointDouble WaypointLoc;
 FPointDouble GpsLoc;
 FPointDouble ShipLoc;

 GetAssociatedCarrier(&ShipLoc);

 WaypointLoc.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
 WaypointLoc.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;
 WaypointLoc.Y = 0;

 if(AvNumGpsLabels > 0)
 {
		GpsLoc.X = AvGpsLabels[Av.Hsi.CurGps]->lX*FTTOWU;
		GpsLoc.Z = AvGpsLabels[Av.Hsi.CurGps]->lY*FTTOWU;
		GpsLoc.Y = 0;
 }

 switch(Av.Hsi.SteeringMode)
 {
	 case HSI_TCN_MODE: *Loc = UFC.TacanPos; break;
	 case HSI_TGT_MODE: *Loc = PlayerPlane->AGDesignate; break;
	 case	HSI_ACL_MODE: *Loc = ShipLoc;     break;
	 case	HSI_WPT_MODE: *Loc = WaypointLoc; break;
	 case	HSI_GPS_MODE: *Loc = GpsLoc;      break;
 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CycleSteeringMode(void)
{
  	FPointDouble ShipLoc;

	switch (Av.Hsi.SteeringMode)
	{
		case HSI_WPT_MODE:
			if(PlayerPlane->AGDesignate.X != -1)
			{
				Av.Hsi.SteeringMode = HSI_TGT_MODE;
				break;
			}

		case 	HSI_TGT_MODE:
			if(UFC.CurrentTacanPtr)
			{
				Av.Hsi.SteeringMode = HSI_TCN_MODE;
				break;
			}

		case HSI_TCN_MODE:
			if(AvNumGpsLabels > 0)
			{
				Av.Hsi.SteeringMode = HSI_GPS_MODE;
				break;
			}

		case HSI_GPS_MODE:
  			if (GetAssociatedCarrier(&ShipLoc))
			{
				Av.Hsi.SteeringMode = HSI_ACL_MODE;
				break;
			}

		case HSI_ACL_MODE:
			Av.Hsi.SteeringMode = HSI_WPT_MODE;
			break;
	}
}


void ToggleDataTime(int Set)
{
	Av.Hsi.DataTime = Set;
	UFC.DataCurrTimeMode = Set;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSetTcn()
{
	if(UFC.CurrentTacanPtr == NULL) return;

	Av.Hsi.SteeringMode = HSI_TCN_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSetGps()
{
	if(AvNumGpsLabels <= 0) return;

	Av.Hsi.SteeringMode = HSI_GPS_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSetTgt()
{
	if(PlayerPlane->AGDesignate.X == -1) return;

	Av.Hsi.SteeringMode = HSI_TGT_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSetAcl()
{
  FPointDouble ShipLoc;
  int ShipExists = GetAssociatedCarrier(&ShipLoc);
  if(!ShipExists) return;

	Av.Hsi.SteeringMode = HSI_ACL_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSetWpt()
{
	Av.Hsi.SteeringMode = HSI_WPT_MODE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSetTup()
{
	Av.Hsi.DisplayMode = HSI_TUP;
	Av.Hsi.Origin.X = 70;
	Av.Hsi.Origin.Z = 69;
	Av.Hsi.Origin.Y = 0;

	Av.Hsi.RadiusInPixels = 53.0;
	Av.Hsi.PixelsPerMile  = Av.Hsi.RadiusInPixels/Av.Hsi.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void HsiSetDctr()
{
	Av.Hsi.DisplayMode = HSI_DCTR;
	Av.Hsi.Origin.X = 70;
	Av.Hsi.Origin.Z = 122;
	Av.Hsi.Origin.Y = 0;

	Av.Hsi.RadiusInPixels = 105.0;
	Av.Hsi.PixelsPerMile  = Av.Hsi.RadiusInPixels/Av.Hsi.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleSensors()
{
	Av.Hsi.SensorsOn = !Av.Hsi.SensorsOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleTime()
{
	Av.Hsi.TimeOn = !Av.Hsi.TimeOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleAuto()
{
	Av.Hsi.AutoOn = !Av.Hsi.AutoOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleSeq()
{
	Av.Hsi.SequenceOn = !Av.Hsi.SequenceOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AVSetWayPosition()
{
	PlayerPlane->AI.WayPosition.X = ConvertWayLoc(PlayerPlane->AI.CurrWay->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
	PlayerPlane->AI.WayPosition.Z = ConvertWayLoc(PlayerPlane->AI.CurrWay->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(PlayerPlane->AI.CurrWay->iSpeed)
	{
		PlayerPlane->AI.lDesiredSpeed = PlayerPlane->AI.CurrWay->iSpeed;
	}

	if(PlayerPlane->AI.lDesiredSpeed <= 100)
	{
		if(!(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			PlayerPlane->AI.lDesiredSpeed = 350;
		}
		else
		{
			PlayerPlane->AI.lDesiredSpeed = 50;
		}
	}

	if(PlayerPlane->AI.CurrWay->lWPy > 0)
	{
		PlayerPlane->AI.WayPosition.Y = ConvertWayLoc(PlayerPlane->AI.CurrWay->lWPy);
	}
	else if(AIIsTakeOff(PlayerPlane->AI.CurrWay))
	{
		PlayerPlane->AI.WayPosition.Y = ConvertWayLoc(8000);
	}
	else
	{
		PlayerPlane->AI.WayPosition.Y = LandHeight(PlayerPlane->AI.WayPosition.X, PlayerPlane->AI.WayPosition.Z) + ConvertWayLoc(labs(PlayerPlane->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSteeringDown()
{
	if(Av.Hsi.SteeringMode == HSI_GPS_MODE)
	{
		if(Av.Hsi.CurGps == -1) return;
		if(Av.Hsi.CurGps - 1 >= 0)
			 Av.Hsi.CurGps--;
	}
  else if(Av.Hsi.SteeringMode == HSI_WPT_MODE)
	{
			if(PlayerPlane->AI.CurrWay > &AIWayPoints[PlayerPlane->AI.startwpts])
			{
				PlayerPlane->AI.CurrWay--;
				PlayerPlane->AI.numwaypts++;
				AVSetWayPosition();
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiSteeringUp()
{
	if(Av.Hsi.SteeringMode == HSI_GPS_MODE)
	{
		if(Av.Hsi.CurGps == -1) return;
		if(Av.Hsi.CurGps + 1 < AvNumGpsLabels)
			 Av.Hsi.CurGps++;
	}
  else if(Av.Hsi.SteeringMode == HSI_WPT_MODE)
	{
 			if(PlayerPlane->AI.numwaypts > 1)
			{
				PlayerPlane->AI.CurrWay++;
				PlayerPlane->AI.numwaypts--;
				AVSetWayPosition();
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleData()
{
	Av.Hsi.DataOn = !Av.Hsi.DataOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleCsel()
{
	Av.Hsi.CselOn = !Av.Hsi.CselOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiIncrementRange()
{
  Av.Hsi.Range  *= 2.0;
	if(Av.Hsi.Range > 160.0)
		Av.Hsi.Range = 5.0;

	Av.Hsi.PixelsPerMile  = Av.Hsi.RadiusInPixels/Av.Hsi.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiDecrementRange()
{
  Av.Hsi.Range  /= 2.0;
	if(Av.Hsi.Range < 5.0)
		Av.Hsi.Range = 160;

	Av.Hsi.PixelsPerMile  = Av.Hsi.RadiusInPixels/Av.Hsi.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiToggleMap()
{
	Av.Hsi.ShowMap = !Av.Hsi.ShowMap;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiNavInfo()
{
	int hour, min, sec;
	float Dist = UFC.DataCurrSPNavDist;

	if(Dist > 999.9) Dist = 999.9;

	hour = UFC.DataCurrSPEteHr;
	min  = UFC.DataCurrSPEteMin;
	sec  = UFC.DataCurrSPEteSec;

	if(hour > 9)
	{
		hour = 9;
		min  = 59;
		sec  = 59;
	}

	sprintf(TmpStr,"%d",(int)UFC.DataCurrSPMilBrg);
  DrawTextAtLoc(98,11,TmpStr,AV_GREEN,1.0);
  DrawTextAtLoc(114,11,"/",AV_GREEN,1.0);
	sprintf(TmpStr,"%3.1f",Dist);
  DrawTextAtLoc(119,11,TmpStr,AV_GREEN,1.0);
	sprintf(TmpStr,"%d:%d:%d",hour,min,sec);
  DrawTextAtLoc(108,18,TmpStr,AV_GREEN,1.0);

	hour = UFC.TacanEteHr;
	min  = UFC.TacanEteMin;
	sec  = UFC.TacanEteSec;

	if(hour > 9)
	{
		hour = 9;
		min  = 59;
		sec  = 59;
	}

	Dist = UFC.TacanNavDist;
	if(Dist > 999.9) Dist = 999.9;

	sprintf(TmpStr,"%d",(int)UFC.TacanMilBearing);
  DrawTextAtLoc(11,11,TmpStr,AV_GREEN,1.0);
  DrawTextAtLoc(27,11,"/",AV_GREEN,1.0);
	sprintf(TmpStr,"%3.1f",Dist);
  DrawTextAtLoc(31,11,TmpStr,AV_GREEN,1.0);
	sprintf(TmpStr,"%d:%d:%d",hour,min,sec);
  DrawTextAtLoc(11,17,TmpStr,AV_GREEN,1.0);
  DrawTextAtLoc(11,23,"TRM",AV_GREEN,1.0);
  DrawTextAtLoc(12,30,UFC.CurrentTacanStation,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawTriangleRot(float X,float Y,int Size,float Angle,int Color,float Shade,int Fill)
{
	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);

	FPoint Shape[3];
	FPoint RotShape[3];
	ZeroMemory(Shape,sizeof(FPoint)*3.0);
	ZeroMemory(RotShape,sizeof(FPoint)*3.0);

	Shape[0].X = 0; Shape[0].Z = 0;
	Shape[1].X = -Size; Shape[1].Z = Size;
	Shape[2].X = Size; Shape[2].Z = Size;

	FMatrix Mat;
	Mat.SetHPR(Angle*DEGREE,0,0);

	for(int j=0; j<3; j++)
	{
	  RotShape[j].RotateInto(Shape[j],Mat);

		RotShape[j].X += X;
	  RotShape[j].Z += Y;
	}

	if(Fill)
	{
			float Xs[3];
			float Ys[3];
			FPointData Colors[3];

			for(int i=0; i<3; i++)
			{
				Colors[i].X = Triple.Red/255.0;
				Colors[i].Y = Triple.Green/255.0;
				Colors[i].Z = Triple.Blue/255.0;
			}

			Xs[0] = RotShape[0].X; Xs[1] = RotShape[1].X; Xs[2] = RotShape[2].X;
			Ys[0] = RotShape[0].Z; Ys[1] = RotShape[1].Z; Ys[2] = RotShape[2].Z;

			GrDrawRampedPolyBuff(GrBuffFor3D,3,Xs,Ys,Colors,1.0);
	}
	else
	{
		int Index=0;

		while(Index < 3)
		{
		 int i = (Index+1) % 3;
	   GrDrawLine(GrBuffFor3D,RotShape[Index].X,RotShape[Index].Z,RotShape[i].X,RotShape[i].Z,Triple.Red,Triple.Green,Triple.Blue,0);
		 Index++;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHSelRot(float X,float Y,float Angle,int Color,float Shade)
{
	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);

	FPoint Shape[6];
	FPoint RotShape[6];
	ZeroMemory(Shape,sizeof(FPoint)*4.0);
	ZeroMemory(RotShape,sizeof(FPoint)*4.0);

	Shape[0].X = -2; Shape[0].Z = -1;
	Shape[1].X = 2;  Shape[1].Z = -1;
	Shape[2].X = 2;  Shape[2].Z = 1;
	Shape[3].X = -2; Shape[3].Z = 1;
	Shape[4].X = -2; Shape[3].Z = 1;
	Shape[5].X = -2; Shape[3].Z = 1;

	FMatrix Mat;
	Mat.SetHPR(Angle*DEGREE,0,0);

	for(int j=0; j<4; j++)
	{
	  RotShape[j].RotateInto(Shape[j],Mat);

		RotShape[j].X += X;
	  RotShape[j].Z += Y;
	}

	int Index=0;
	while(Index < 4)
	{
		int i = (Index+1) % 4;
	  GrDrawLine(GrBuffFor3D,RotShape[Index].X,RotShape[Index].Z,RotShape[i].X,RotShape[i].Z,Triple.Red,Triple.Green,Triple.Blue,0);
		Index++;
	}

	GrDrawLine(GrBuffFor3D,RotShape[4].X,RotShape[4].Z,RotShape[5].X,RotShape[5].Z,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawSquareRot(float X,float Y,int SizeX,int SizeY,float Angle,int Color,float Shade,int Fill)
{
	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);

	FPoint Shape[4];
	FPoint RotShape[4];
	ZeroMemory(Shape,sizeof(FPoint)*4.0);
	ZeroMemory(RotShape,sizeof(FPoint)*4.0);

	Shape[0].X = -SizeX; Shape[0].Z = -SizeY;
	Shape[1].X = SizeX;  Shape[1].Z = -SizeY;
	Shape[2].X = SizeX;  Shape[2].Z = SizeY;
	Shape[3].X = -SizeX; Shape[3].Z = SizeY;

	FMatrix Mat;
	Mat.SetHPR(Angle*DEGREE,0,0);

	for(int j=0; j<4; j++)
	{
	  RotShape[j].RotateInto(Shape[j],Mat);

		RotShape[j].X += X;
	  RotShape[j].Z += Y;
	}

	if(Fill)
	{
			float Xs[4];
			float Ys[4];
			FPointData Colors[4];

			for(int i=0; i<4; i++)
			{
				Colors[i].X = Triple.Red;
				Colors[i].Y = Triple.Green;
				Colors[i].Z = Triple.Blue;
			}

			Xs[0] = RotShape[0].X; Xs[1] = RotShape[1].X; Xs[2] = RotShape[2].X; Xs[3] = RotShape[3].X;
			Ys[0] = RotShape[0].Z; Ys[1] = RotShape[1].Z; Ys[2] = RotShape[2].Z; Ys[3] = RotShape[3].Z;

			GrDrawRampedPolyBuff(GrBuffFor3D,4,Xs,Ys,Colors,1.0);
	}
	else
	{
		int Index=0;

		while(Index < 4)
		{
		 int i = (Index+1) % 4;
	   GrDrawLine(GrBuffFor3D,RotShape[Index].X,RotShape[Index].Z,RotShape[i].X,RotShape[i].Z,Triple.Red,Triple.Green,Triple.Blue,0);
		 Index++;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHomeBase(float X, float Y, int Color, float Shade)
{
	AvRGB Triple;
	GetRGBFromAVPalette(Color,Shade,&Triple);

	GrDrawLine(GrBuffFor3D,X-4,Y-3,X+4,Y-3,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+4,Y-3,X+4,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X+4,Y+1,X,Y+5,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X,Y+5,X-4,Y+1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,X-4,Y+1,X-4,Y-3,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiCircle(float X,float Y,int ColorIndex,float Shade)
{
    AvRGB Color;
    GetRGBFromAVPalette(ColorIndex,Shade,&Color);

	GrDrawLine(GrBuffFor3D,X-2,Y,X+2,Y,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,X-2,Y-1,X+2,Y-1,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,X-2,Y+1,X+2,Y+1,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,X-1,Y-2,X+1,Y-2,Color.Red,Color.Green,Color.Blue,0);
	GrDrawLine(GrBuffFor3D,X-1,Y+2,X+1,Y+2,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CircleIntersect(FPointDouble P1,FPointDouble P2,FPointDouble Origin,float Radius,FPointDouble *Intersect)
{
	// P1 is point inside circle, P2 is point outside circle
	// see page 388 of graphics gems vol1

	float esquared,v,disc,rsquared,vsquared,d;

	FPoint E,V,R;

	E = Origin;
	E -= P2;
	V = P1;
	V -= P2;

	V.Normalize();

	v = E*V;
	vsquared = v*v;
	esquared = E * E;
	rsquared = Radius*Radius;
	disc     = rsquared - (esquared - vsquared);
	d        = sqrt(disc);

	V *= (v-d);

	*Intersect = P2;
	*Intersect += V;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawSteeringArrow(float X, float Y)
{
	FPointDouble ProjPoint,Reference,Intersect;

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	float Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53.0 : 105.0;
	Radius -= 10.0;

	Reference.X = X;
	Reference.Z = Y;
	Reference.Y = 0;

	ProjPoint.Y = 0;

	float CSel = NormDegree(360.0 - Av.Hsi.CSelect);

	float DeltaAzim    = NormDegree(CSel + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));
	float DeltaAzimOpp = NormDegree(DeltaAzim + 180.0);

  float ProjX;
	float ProjZ;

	ProjectPoint(X,Y,DeltaAzim,1000.0,&ProjX,&ProjZ);

	ProjPoint.X = ProjX; ProjPoint.Y = 0; ProjPoint.Z = ProjZ;

	CircleIntersect(Reference,ProjPoint,Av.Hsi.Origin,Radius,&Intersect);
	GrDrawLine(GrBuffFor3D,X,Y,Intersect.X,Intersect.Z,Color.Red,Color.Green,Color.Blue,0);
	DrawTriangleRot(Intersect.X,Intersect.Z,3,DeltaAzim,AV_GREEN,1.0,TRUE);

	ProjectPoint(X,Y,DeltaAzimOpp,1000.0,&ProjX,&ProjZ);

	ProjPoint.X = ProjX; ProjPoint.Y = 0; ProjPoint.Z = ProjZ;

	CircleIntersect(Reference,ProjPoint,Av.Hsi.Origin,Radius,&Intersect);
	GrDrawLine(GrBuffFor3D,X,Y,Intersect.X,Intersect.Z,Color.Red,Color.Green,Color.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiTacan()
{
	float NewX,NewZ;
	FPointDouble TacanPos = UFC.TacanPos;

	if ((Av.Hsi.SteeringMode ==  HSI_TCN_MODE) && (g_iNumTACANObjects > 0))
	{
		AvRGB Triple;
		GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

		float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,TacanPos,1);
		float Dist      = Dist2D(&PlayerPlane->WorldPosition,&TacanPos);
		Dist *= WUTONM * Av.Hsi.PixelsPerMile;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

		if(Radius >= Dist)
		{
			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);
			DrawAvRadarSymbol(NewX,NewZ,TACAN_SYMBOL,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

			if( Av.Hsi.CselOn)
    			DrawSteeringArrow(NewX,NewZ);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiGroundTarget()
{
	FPointDouble TgtLoc;
	float NewX,NewZ;

	if(((Av.Hsi.SteeringMode == HSI_TGT_MODE)) && (PlayerPlane->AGDesignate.X != -1))
	{
		TgtLoc.X = PlayerPlane->AGDesignate.X;
		TgtLoc.Z = PlayerPlane->AGDesignate.Z;
		TgtLoc.Y = 0;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

		float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,TgtLoc,1);

		ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Radius - 5,&NewX,&NewZ);
		DrawTriangleRot(NewX,NewZ,5,NormDegree(DeltaAzim),AV_GREEN,1.0,TRUE);

		ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree( NormDegree(DeltaAzim) + 180.0 ),Radius - 8,&NewX,&NewZ);
		DrawSquareRot(NewX,NewZ,2,4,NormDegree(DeltaAzim),AV_GREEN,1.0,FALSE);

		float Dist = Dist2D(&PlayerPlane->WorldPosition,&TgtLoc);
		Dist *= WUTONM*Av.Hsi.PixelsPerMile;

		if(Radius >= Dist)
		{
			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);

			DrawTriangleRot(NewX,NewZ,4,0,AV_GREEN,1.0,FALSE);

			if( Av.Hsi.CselOn)
				DrawSteeringArrow(NewX,NewZ);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiBullseye()
{
	FPointDouble BELoc;
	float NewX,NewZ;

	if( (BullsEye.x <= 0) && (BullsEye.z <= 0) ) return;

	AvRGB Color;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	BELoc.X = BullsEye.x*FTTOWU;
	BELoc.Z = BullsEye.z*FTTOWU;
	BELoc.Y = 0;

  float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,BELoc,1);
	float Dist      = Dist2D(&PlayerPlane->WorldPosition,&BELoc);
	Dist *= WUTONM*Av.Hsi.PixelsPerMile;

	int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;
	if(Radius >= Dist)
	{
  		ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);

		GrDrawPolyCircle(GrBuffFor3D,NewX,NewZ,4,20,Color.Red,Color.Green,Color.Blue,0);

		float TempX,TempZ;
		float AngleToNorth = 360.0 - (PlayerPlane->Heading/DEGREE);

		ProjectPoint(NewX,NewZ,NormDegree(AngleToNorth),11,&TempX,&TempZ);
		GrDrawLine(GrBuffFor3D,NewX,NewZ,TempX,TempZ,Color.Red,Color.Green,Color.Blue,0);
		DrawTriangleRot(TempX,TempZ,3,NormDegree(AngleToNorth),AV_GREEN,1.0,TRUE);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiAcl()
{
	FPointDouble ShipLoc;
  	int ShipExists;

  	if ((Av.Hsi.SteeringMode == HSI_ACL_MODE) && (ShipExists= GetAssociatedCarrier(&ShipLoc)))
  	{
  		float NewX,NewZ;

		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,ShipLoc,1);
		float Dist      = Dist2D(&PlayerPlane->WorldPosition,&ShipLoc);
		Dist *= WUTONM*Av.Hsi.PixelsPerMile;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;
		if(Radius >= Dist)
		{
			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);
			DrawHomeBase(NewX,NewZ,AV_GREEN,1.0);
		}
  	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
extern FPointDouble AA_PrimaryPosition;

void DrawHsiLs()
{
	FPointDouble TgtLoc;
	float NewX,NewZ;


	if(AA_PrimaryPosition.X != -1)
	{
		TgtLoc = AA_PrimaryPosition;
		TgtLoc.Y = 0;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,TgtLoc,1);

		float Dist = Dist2D(&PlayerPlane->WorldPosition,&TgtLoc);
		Dist *= WUTONM*Av.Hsi.PixelsPerMile;

		if(Radius >= Dist)
		{
			AvRGB Triple;
			GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);

			GrDrawLine(GrBuffFor3D,NewX-3,NewZ-1,NewX+3,NewZ-1,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,NewX+3,NewZ-1,NewX+3,NewZ+1,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,NewX+3,NewZ+1,NewX-3,NewZ+1,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,NewX-3,NewZ+1,NewX-3,NewZ-1,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,NewX,NewZ+1,NewX,NewZ-1,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,NewX,NewZ+1,NewX-2,NewZ+4,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLine(GrBuffFor3D,NewX,NewZ+1,NewX+2,NewZ+4,Triple.Red,Triple.Green,Triple.Blue,0);
		}
	}
}

void DrawHSIGroundTrack(void)
{
	int midx=Av.Hsi.Origin.X;
	int topy = Av.Hsi.Origin.Z+5.0;

	topy -= (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	GrDrawLine(GrBuffFor3D,midx,topy,midx-3,topy+3,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,midx-3,topy+3,midx,topy+6,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,midx,topy+6,midx+3,topy+3,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,midx+3,topy+3,midx,topy,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,midx,topy+6,midx,topy+9,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiFlir()
{
	float NewX,NewZ;

	if((Av.Hsi.SensorsOn) && (Av.Weapons.HasTargetIR) && (UFC.MasterMode == AG_MODE))
	{
		FPointDouble TargetPos;
		int TargetAvailable = GetFlirTargetPosition(&TargetPos);

		if(TargetAvailable)
		{
			float Dist = Dist2D(&PlayerPlane->WorldPosition,&TargetPos);
			int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

			Dist *= WUTONM*Av.Hsi.PixelsPerMile;

			if(Radius >= Dist)
			{
      			float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,TargetPos,1);
				ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist,&NewX,&NewZ);
				DrawTextAtLoc(NewX-2,NewZ-2,"F",AV_GREEN,1.0);
			}
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiRadar()
{

	if((Av.Hsi.SensorsOn) && (UFC.MasterMode == AG_MODE))
	{
		float NewX,NewZ;
		float Dist;
		float DeltaAzim;
		FPointDouble target;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? (53-5) : (105-5);

//  		AvRGB Triple;
//  		GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

		if (Av.AGRadar.CurMode == AG_FTT_MODE)
		{
			GetAGRadarTargetPos(target);

			Dist = (target/PlayerPlane->WorldPosition)*WUTONM*Av.Hsi.PixelsPerMile;
			DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,target,1);
		}
		else
			if (Av.AGRadar.CurMode == AG_MAP_MODE)
			{
				target = 0.0f;
				int i = 4;
				while(i--)
					target += ScanAreasWorld[Av.AGRadar.MapMode][i];

				target *= 0.25;

				Dist = (target/PlayerPlane->WorldPosition)*WUTONM*Av.Hsi.PixelsPerMile;
				DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,target,1);
			}
			else
			{
				Dist = (Av.AGRadar.Range/2.0)*Av.Hsi.PixelsPerMile;
				DeltaAzim = 0;
			}

		if(Radius >= Dist)
		{
  			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,DeltaAzim,Dist,&NewX,&NewZ);
			DrawTextAtLoc(NewX-2,NewZ-2,"R",AV_GREEN,1.0);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiWpt()
{
	FPointDouble WaypointLoc;
	float NewX,NewZ;

	if (Av.Hsi.SteeringMode == HSI_WPT_MODE)
	{

		WaypointLoc.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
		WaypointLoc.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;
		WaypointLoc.Y = 0;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,WaypointLoc,1);

		ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Radius - 8,&NewX,&NewZ);
		DrawTriangleRot(NewX,NewZ,5,NormDegree(DeltaAzim),AV_GREEN,1.0,TRUE);

		ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree( NormDegree(DeltaAzim) + 180.0 ),Radius - 12,&NewX,&NewZ);
		DrawSquareRot(NewX,NewZ,2,4,NormDegree(DeltaAzim),AV_GREEN,1.0,FALSE);

		float Dist = Dist2D(&PlayerPlane->WorldPosition,&WaypointLoc);
		Dist *= WUTONM;

		if(Radius >= Dist*Av.Hsi.PixelsPerMile)
		{
			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist*Av.Hsi.PixelsPerMile,&NewX,&NewZ);

			DrawHsiCircle(NewX,NewZ,AV_GREEN,1.0);

			if( Av.Hsi.CselOn)
    			DrawSteeringArrow(NewX,NewZ);

			sprintf(TmpStr,"%d",(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
			DrawTextAtLoc(NewX + 4,NewZ + 4,TmpStr,AV_GREEN,1.0);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiWaypointLine(FPoint &p1, FPoint &p2,BOOL P1In,BOOL P2In)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  /* -----------------10/25/99 3:53PM--------------------------------------------------------------------
  /* ooooooh this burns me up!!!!!  jp
  /* ----------------------------------------------------------------------------------------------------*/

  FPointDouble P1,P2;

  P1 = p1;
  P2 = p2;

	FPointDouble Intersect,Origin;

	Origin = Av.Hsi.Origin;

	int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? (53-5) : (105-5);

	if(P1In && P2In)
	{
	  GrDrawLine(GrBuffFor3D,P1.X,P1.Z,P2.X,P2.Z,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	else if(P1In)
	{
	  CircleIntersect(P1,P2,Origin,Radius,&Intersect);
	  GrDrawLine(GrBuffFor3D,P1.X,P1.Z,Intersect.X,Intersect.Z,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	else if(P2In)
	{
	  CircleIntersect(P2,P1,Origin,Radius,&Intersect);
	  GrDrawLine(GrBuffFor3D,P2.X,P2.Z,Intersect.X,Intersect.Z,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	else // both points are outside the circle
	{
		FPointDouble PointOnLine;

		int Result = PointOnLineClosestToPoint(P1,P2,Origin,NULL,&PointOnLine);

		if(!Result)
			PointOnLine = Origin;

		float DistToLine = PointOnLine / Origin;

		if(DistToLine <= Radius)
		{
			CircleIntersect(PointOnLine,P1,Origin,Radius,&Intersect);
			GrDrawLine(GrBuffFor3D,PointOnLine.X,PointOnLine.Z,Intersect.X,Intersect.Z,Triple.Red,Triple.Green,Triple.Blue,0);

			CircleIntersect(PointOnLine,P2,Origin,Radius,&Intersect);
			GrDrawLine(GrBuffFor3D,PointOnLine.X,PointOnLine.Z,Intersect.X,Intersect.Z,Triple.Red,Triple.Green,Triple.Blue,0);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

BOOL GetWaypointForHsiWPT(MBWayPoints &wp,int radius,float &sx,float &sy)
{
	float azim,dist;
	FPointDouble p;

	p.SetValues((double)(wp.lWPx)*FTTOWU,0.0,(double)(wp.lWPz)*FTTOWU);

	azim = NormDegree(ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,p,1));
	dist = Dist2D(&PlayerPlane->WorldPosition,&p) * WUTONM;
	ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,azim,dist*Av.Hsi.PixelsPerMile,&sx,&sy);

	return (BOOL)(dist*Av.Hsi.PixelsPerMile < radius);
}

void DrawHsiWaypoints()
{
  	MBWayPoints *Start  = &AIWayPoints[PlayerPlane->AI.startwpts];
  	MBWayPoints *Last   = PlayerPlane->AI.CurrWay + PlayerPlane->AI.numwaypts - 1;

	if ((Start+1) < Last)
	{
		AvRGB Triple;
		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? (53-5) : (105-5);
		BOOL done = FALSE;


		MBWayPoints *wFrom,*wTo;
		FPoint fp1st;
		FPoint fpFrom;
		FPoint fpTo;
		BOOL b1stin,bFromin,bToin;

		GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

		/* -----------------10/25/99 3:08PM--------------------------------------------------------------------
		/* first draw the from waypoint, in the loop, we'll only draw the to stuff
		/* ----------------------------------------------------------------------------------------------------*/

		wTo = Start;
		fp1st.Y = 0;
		fpFrom.Y = 0;
		fpTo.Y = 0;

		b1stin = GetWaypointForHsiWPT(*wTo,Radius,fp1st.X,fp1st.Z);

		fpTo.X = fp1st.X;
		fpTo.Z = fp1st.Z;
		bToin = b1stin;


		while(!done)
		{
			wFrom = wTo++;
			fpFrom.X = fpTo.X;
			fpFrom.Z = fpTo.Z;
			bFromin = bToin;

			if (wTo > Last)
			{
				wTo = Start;
				done = TRUE;
				fpTo.X = fp1st.X;
				fpTo.Z = fp1st.Z;
				bToin = b1stin;
			}
			else
				bToin = GetWaypointForHsiWPT(*wTo,Radius,fpTo.X,fpTo.Z);

			if (bFromin)
			{
				GrDrawPolyCircle(GrBuffFor3D,fpFrom.X,fpFrom.Z,4,21,Triple.Red,Triple.Green,Triple.Blue,0);
				sprintf(TmpStr,"%d",(wFrom-Start));
				DrawTextAtLoc(fpFrom.X + 4,fpFrom.Z + 4,TmpStr,AV_GREEN,1.0);
			}

			DrawHsiWaypointLine(fpFrom,fpTo,bFromin,bToin);
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHsiGps()
{
	FPointDouble GpsLoc;
	float NewX,NewZ;

	if((Av.Hsi.SteeringMode == HSI_GPS_MODE) && (AvNumGpsLabels > 0))
	{
		GpsLoc.X = AvGpsLabels[Av.Hsi.CurGps]->lX*FTTOWU;
		GpsLoc.Z = AvGpsLabels[Av.Hsi.CurGps]->lY*FTTOWU;
		GpsLoc.Y = 0;

		int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;

		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,GpsLoc,1);

		float Dist = Dist2D(&PlayerPlane->WorldPosition,&GpsLoc);
		Dist *= WUTONM;

		if(Radius >= Dist*Av.Hsi.PixelsPerMile)
		{
			ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,NormDegree(DeltaAzim),Dist*Av.Hsi.PixelsPerMile,&NewX,&NewZ);

			DrawTriangleRot(NewX,NewZ,4,0,AV_GREEN,1.0,FALSE);

			if( Av.Hsi.CselOn)
				DrawSteeringArrow(NewX,NewZ);

			sprintf(TmpStr,"%d",Av.Hsi.CurGps+1);
			DrawTextAtLoc(NewX + 4,NewZ + 4,TmpStr,AV_GREEN,1.0);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawHSel()
{
	float NewX,NewZ;

	float HSel = NormDegree(360.0 - Av.Hsi.HSelect);

	float DeltaAzim = NormDegree(HSel + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));

	int Radius = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;
	Radius += 6;

	ProjectPoint(Av.Hsi.Origin.X,Av.Hsi.Origin.Z,DeltaAzim,Radius,&NewX,&NewZ);
  	DrawHSelRot(NewX,NewZ,DeltaAzim,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HsiDataMode(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DrawVerticalWordAtButton(MpdNum,0,"TCN",(Av.Hsi.SteeringMode == HSI_TCN_MODE),AV_GREEN,1.0);
	if(UFC.CurrentTacanPtr == NULL)
	{
    //CrossOutVertWordAtLocation(10,10,"TGT",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,1,"GPS",(Av.Hsi.SteeringMode == HSI_GPS_MODE),AV_GREEN,1.0);
	if(AvNumGpsLabels <= 0)
	{
    //CrossOutVertWordAtLocation(10,10,"GPS",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,2,"ACL",(Av.Hsi.SteeringMode == HSI_ACL_MODE),AV_GREEN,1.0);
	if(FALSE /* Aircraft Carier assigned is null */)
	{
    //CrossOutVertWordAtLocation(10,10,"ACL",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,13,"TGT",(Av.Hsi.SteeringMode == HSI_TGT_MODE),AV_GREEN,1.0);
	if(PlayerPlane->AGDesignate.X == -1)
	{
    //CrossOutVertWordAtLocation(10,10,"TGT",AV_YELLOW,1.0);
	}

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);




	// Draw Time Above button 8,9
	int TextScrX;
	int TextScrY;

	GetMPDButtonXY(MpdNum,8,"A", TextScrX, TextScrY);
	DrawTextAtLoc(TextScrX+7,TextScrY-8, "TIME",AV_GREEN,1.0);



	DrawWordAtMpdButton(MpdNum,8,"ZULU",(Av.Hsi.DataTime == ZULU_TIME),AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,9,"LOC",(Av.Hsi.DataTime == LOCAL_TIME),AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,14,"WPT",(Av.Hsi.SteeringMode == HSI_WPT_MODE),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,15,"DATA",Av.Hsi.DataOn,AV_GREEN,1.0);

	if( ( (AvNumGpsLabels > 0) && (Av.Hsi.SteeringMode == HSI_GPS_MODE) ) || (Av.Hsi.SteeringMode == HSI_WPT_MODE) )
	{
			DrawTriangleRot(136,59,3,0,AV_GREEN,1.0,TRUE);
			GrDrawLine(GrBuffFor3D,136,59,136,72,Triple.Red,Triple.Green,Triple.Blue,0);

			sprintf(TmpStr,"%d",(Av.Hsi.SteeringMode == HSI_WPT_MODE) ? UFC.DataCurrSP : Av.Hsi.CurGps+1);
			DrawTextAtLoc(132,74,TmpStr,AV_GREEN,1.0);

			DrawTriangleRot(136,94,3,180.0,AV_GREEN,1.0,TRUE);
			GrDrawLine(GrBuffFor3D,136,81,136,94,Triple.Red,Triple.Green,Triple.Blue,0);

			if(Av.Hsi.SteeringMode == HSI_WPT_MODE)
			  sprintf(TmpStr,"WYPT %d",UFC.DataCurrSP);
			else
			  sprintf(TmpStr,"GPS %d",Av.Hsi.CurGps+1);
			DrawTextAtLoc(56,18,TmpStr,AV_GREEN,1.0,TRUE);
	}

	if(Av.Hsi.SteeringMode == HSI_TCN_MODE)
	   DrawTextAtLoc(56,18,"TACAN",AV_GREEN,1.0,TRUE);
	else if(Av.Hsi.SteeringMode == HSI_TGT_MODE)
	   DrawTextAtLoc(56,18,"TGT",AV_GREEN,1.0,TRUE);
	else if(Av.Hsi.SteeringMode == HSI_ACL_MODE)
	   DrawTextAtLoc(56,18,"ACL",AV_GREEN,1.0,TRUE);

	float Bearing,Range,Elev,Dist;
	int   Hour,Min,Sec;
	int   TotInSec,TotHour,TotMin,TotSec;

	switch(Av.Hsi.SteeringMode)
	{
		case HSI_TCN_MODE:
			{
				Dist = UFC.TacanNavDist;

				if(Dist > 999.9)
					Dist = 999.9;

				Bearing = UFC.TacanMilBearing;
				Range   = Dist;
				Elev    = UFC.TacanPos.Y*WUTOFT;

				Hour = UFC.TacanEteHr;
				Min  = UFC.TacanEteMin;
				Sec  = UFC.TacanEteSec;

				TotInSec = WorldParams.WorldTime + (UFC.TacanEteHr*3600 + UFC.TacanEteMin*60 +  UFC.TacanEteSec);


			}
			break;

		case HSI_TGT_MODE:
			{
				float Dx,Dy,Dz;
				float AbsBrg = AIComputeHeadingToPoint(PlayerPlane, PlayerPlane->AGDesignate, &Dist, &Dx ,&Dy, &Dz, 0);
				int TSeconds;

				if (AbsBrg < 0.0f)
					AbsBrg += 360.0f;

				AbsBrg = 360.0f - AbsBrg;

				if (AbsBrg == 360.0f)
					AbsBrg = 0.0f;

				Bearing = AbsBrg;
				Dist *= WUTONM;

				if(Dist > 999.9)
					Dist = 999.9;

				Range = Dist;
				Elev = PlayerPlane->AGDesignate.Y*WUTOFT;

				if (PlayerPlane->IfHorzVelocity > 10.0)
					TSeconds = (int)((float)(Dist*NMTOFT)/(float)PlayerPlane->IfHorzVelocity);
				else
					TSeconds = 0;

				Hour  =  TSeconds/3600;
				Min    = (TSeconds%3600)/60;
				Sec    =  TSeconds% 60;

				TotInSec = WorldParams.WorldTime + TSeconds;

			}
			break;

		case HSI_ACL_MODE:
			{
				float Dx,Dy,Dz;
				FPointDouble CarrierLoc;
				int TSeconds;

				MovingVehicleParams *Carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
				GetLandingPoint(Carrier,CarrierLoc);

				float AbsBrg = AIComputeHeadingToPoint(PlayerPlane, CarrierLoc, &Dist, &Dx ,&Dy, &Dz, 0);

				if (AbsBrg < 0.0f)
					AbsBrg += 360.0f;

				AbsBrg = 360.0f - AbsBrg;

				if (AbsBrg == 360.0f)
					AbsBrg = 0.0f;

				Bearing = AbsBrg;

				Dist *= WUTONM;
				if(Dist > 999.9)
					Dist = 999.9;

				Range = Dist;
				Elev = CarrierLoc.Y*WUTOFT;

				if (PlayerPlane->IfHorzVelocity > 10.0)
					TSeconds = (int)((float)(Dist*NMTOFT)/(float)PlayerPlane->IfHorzVelocity);
				else
					TSeconds = 0;

				Hour   =  TSeconds/3600;
				Min    = (TSeconds%3600)/60;
				Sec    =  TSeconds% 60;

				TotInSec = WorldParams.WorldTime + TSeconds;

			}
			break;

		case HSI_WPT_MODE:
			{
				FPointDouble WaypointLoc;
				float Dx,Dy,Dz;
				int TSeconds;

				WaypointLoc.X = (double)PlayerPlane->AI.CurrWay->lWPx;
				WaypointLoc.Y = (double)PlayerPlane->AI.CurrWay->lWPy;
				WaypointLoc.Z = (double)PlayerPlane->AI.CurrWay->lWPz;

				WaypointLoc *= FTTOWU;

				Bearing = 360.0f - AIComputeHeadingToPoint(PlayerPlane,WaypointLoc, &Range, &Dx ,&Dy, &Dz, 0);

				while(Bearing >= 360.0f)
					Bearing -= 360.0f;

				while(Bearing < 0.0f)
					Bearing += 360.0f;

				Range *= WUTONM;

				if(Range > 999.9)
					Range = 999.9;

				Elev    = PlayerPlane->AI.CurrWay->lWPy;

				Hour = UFC.DataCurrSPEteHr;
				Min  = UFC.DataCurrSPEteMin;
				Sec  = UFC.DataCurrSPEteSec;

				if (PlayerPlane->IfHorzVelocity > 10.0)
					TSeconds = (int)((float)(Range*NMTOFT)/(float)PlayerPlane->IfHorzVelocity);
				else
					TSeconds = 0;


				TotInSec = WorldParams.WorldTime + TSeconds;//(UFC.DataCurrSPEteHr*3600 + UFC.DataCurrSPEteMin*60 + UFC.DataCurrSPEteSec);

				if(Av.Hsi.SteeringMode == HSI_WPT_MODE)
				{
					sprintf(TmpStr,"%d",(int)PlayerPlane->AI.CurrWay->iSpeed);
					DrawTextAtLoc(61,84,TmpStr,AV_GREEN,1.0,TRUE);
					DrawTextAtLoc(30,84,"GSPD:",AV_GREEN,1.0,TRUE);
				}
			}
			break;

		case HSI_GPS_MODE:
			{
				FPointDouble GpsLoc;
				float Dx,Dy,Dz;
				int TSeconds;

				GpsLoc.X = AvGpsLabels[Av.Hsi.CurGps]->lX*FTTOWU;
				GpsLoc.Z = AvGpsLabels[Av.Hsi.CurGps]->lY*FTTOWU;
				GpsLoc.Y = LandHeight(GpsLoc.X,GpsLoc.Z);

				Bearing = 360.0f - AIComputeHeadingToPoint(PlayerPlane,GpsLoc, &Range, &Dx ,&Dy, &Dz, 0);

				while(Bearing >= 360.0f)
					Bearing -= 360.0f;

				while(Bearing < 0.0f)
					Bearing += 360.0f;

				Range *= WUTONM;

				if(Range > 999.9)
					Range = 999.9;

				Elev = GpsLoc.Y*WUTOFT;

				if (PlayerPlane->IfHorzVelocity > 10.0)
					TSeconds = (int)((float)(Range*NMTOFT)/(float)PlayerPlane->IfHorzVelocity);
				else
					TSeconds = 0;

				Hour  =  TSeconds/3600;
				Min    = (TSeconds%3600)/60;
				Sec    =  TSeconds% 60;

				TotInSec = WorldParams.WorldTime + TSeconds;

			}
			break;

	}

	if(Av.Hsi.DataTime == ZULU_TIME)
	{
		TotInSec -= 3600*3;
		if(TotInSec < 0)
			TotInSec = 3600*24 + TotInSec;
	}

	TotInSec %= (3600*24);
	TotHour  =  TotInSec/3600;
	TotMin    = (TotInSec%3600)/60;
	TotSec    =  TotInSec% 60;

	if(Hour > 9)
	{
		Hour = 9;
		Min  = 59;
		Sec  = 59;
	}

	DrawTextAtLoc(30,31,"BRG:",AV_GREEN,1.0,TRUE);
	DrawTextAtLoc(30,41,"RNG:",AV_GREEN,1.0,TRUE);
	DrawTextAtLoc(30,51,"ELEV:",AV_GREEN,1.0,TRUE);
	DrawTextAtLoc(30,61,"TTG:",AV_GREEN,1.0,TRUE);

	DrawTextAtLoc(97,41,"NM",AV_GREEN,1.0,TRUE);
	DrawTextAtLoc(97,51,"FT",AV_GREEN,1.0,TRUE);

	sprintf(TmpStr,"%d",(int)Bearing);
	DrawTextAtLoc(61,31,TmpStr,AV_GREEN,1.0,TRUE);

	sprintf(TmpStr,"%3.1f",Range);
	DrawTextAtLoc(61,41,TmpStr,AV_GREEN,1.0,TRUE);

	sprintf(TmpStr,"%d",(int)Elev);
	DrawTextAtLoc(61,51,TmpStr,AV_GREEN,1.0,TRUE);

	sprintf(TmpStr,"%02d:%02d:%02d",Hour,Min,Sec);
	DrawTextAtLoc(61,61,TmpStr,AV_GREEN,1.0,TRUE);

	GrDrawLine(GrBuffFor3D,24,70,100,70,Triple.Red,Triple.Green,Triple.Blue,0);

	sprintf(TmpStr,"%02d:%02d:%02d",TotHour,TotMin,TotSec);
	if (Av.Hsi.DataTime == ZULU_TIME)
		strcat(TmpStr,"Z");
	DrawTextAtLoc(61,74,TmpStr,AV_GREEN,1.0,TRUE);

	DrawTextAtLoc(30,74,"TOT:",AV_GREEN,1.0,TRUE);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 0:  HsiSetTcn();
				       break;
			case 1:  HsiSetGps();
				       break;
			case 2:  HsiSetAcl();
				       break;
			case 7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				       break;
			case 8:  ToggleDataTime(ZULU_TIME);
				       break;
			case 9:  ToggleDataTime(LOCAL_TIME);
				       break;
			case 11: HsiSteeringDown();
				       break;
			case 12: HsiSteeringUp();
				       break;
			case 13: HsiSetTgt();
				       break;
			case 14: HsiSetWpt();
				       break;
			case 15: HsiToggleData();
				       break;
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoHsi(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	if(Av.Hsi.ShowMap)
	{
			// moving map
			FPointDouble W[4];
			FPoint       S[4];
			FPointDouble P1,P2;

			P1.X = Av.Hsi.Origin.X; P1.Z = Av.Hsi.Origin.Z;  P1.Y = 0;

			S[0].X = 0;   S[0].Y = 0;   S[0].Z = 0;
			S[1].X = 140; S[1].Y = 0;   S[1].Z = 0;
			S[2].X = 140; S[2].Y = 140; S[2].Z = 0;
			S[3].X = 0;   S[3].Y = 140; S[3].Z = 0;

			for(int i=0; i<4; i++)
			{
				float NewX,NewZ;

				P2.X = S[i].X; P2.Z = S[i].Y; P2.Y = 0;
				float Dist = Dist2D(&P1,&P2);
				Dist /= Av.Hsi.PixelsPerMile;
				Dist *= NMTOWU;
  			float DeltaAzim = ComputeHeadingToPoint(Av.Hsi.Origin,0,P2,1);
  			ProjectPoint(PlayerPlane->WorldPosition.X,PlayerPlane->WorldPosition.Z,NormDegree((PlayerPlane->Heading/DEGREE) + DeltaAzim),Dist,&NewX,&NewZ);
				W[i].X = NewX; W[i].Z = NewZ; W[i].Y = 0;
			}

			DrawMovingMap(S[0],S[2],W[0],W[1],W[3],W[2],(MpdNum != 2));
	}

	DrawVerticalWordAtButton(MpdNum,0,"TCN",(Av.Hsi.SteeringMode == HSI_TCN_MODE),AV_GREEN,1.0);
	if(UFC.CurrentTacanPtr == NULL)
	{
    //CrossOutVertWordAtLocation(10,10,"TGT",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,1,"GPS",(Av.Hsi.SteeringMode == HSI_GPS_MODE),AV_GREEN,1.0);
	if(AvNumGpsLabels <= 0)
	{
    //CrossOutVertWordAtLocation(10,10,"GPS",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,2,"ACL",(Av.Hsi.SteeringMode == HSI_ACL_MODE),AV_GREEN,1.0);
	if(FALSE /* Aircraft Carier assigned is null */)
	{
    //CrossOutVertWordAtLocation(10,10,"ACL",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,13,"TGT",(Av.Hsi.SteeringMode == HSI_TGT_MODE),AV_GREEN,1.0);
	if(PlayerPlane->AGDesignate.X == -1)
	{
    //CrossOutVertWordAtLocation(10,10,"TGT",AV_YELLOW,1.0);
	}

	DrawVerticalWordAtButton(MpdNum,3,"TUP",(Av.Hsi.DisplayMode == HSI_TUP),AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,4,"DCTR",(Av.Hsi.DisplayMode == HSI_DCTR),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,5,"SENSOR",Av.Hsi.SensorsOn,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,8,"TIME",Av.Hsi.TimeOn,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,9,"AUTO",Av.Hsi.AutoOn,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,10,"SEQ",Av.Hsi.SequenceOn,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,14,"WPT",(Av.Hsi.SteeringMode == HSI_WPT_MODE),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,15,"DATA",Av.Hsi.DataOn,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,16,"CSEL",Av.Hsi.CselOn,AV_GREEN,1.0);
	sprintf(TmpStr,"SCL/%d ",(int)Av.Hsi.Range);
	DrawWordAtMpdButton(MpdNum,17,TmpStr,FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,19,"MAP",Av.Hsi.ShowMap,AV_GREEN,1.0);

	if( ( (AvNumGpsLabels > 0) && (Av.Hsi.SteeringMode == HSI_GPS_MODE) ) || (Av.Hsi.SteeringMode == HSI_WPT_MODE) )
	{
		DrawMdiArrow(137,59,0);
		DrawMdiArrow(137,94,1);

		if (Av.Hsi.SteeringMode == HSI_GPS_MODE)
			sprintf(TmpStr,"%d",Av.Hsi.CurGps+1);
		else
			sprintf(TmpStr,"%d",(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));

		DrawTextAtLoc(135,74,TmpStr,AV_GREEN,1.0);
	}

	GrDrawLine(GrBuffFor3D,Av.Hsi.Origin.X,Av.Hsi.Origin.Z-3,Av.Hsi.Origin.X,Av.Hsi.Origin.Z-2,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,Av.Hsi.Origin.X-6,Av.Hsi.Origin.Z,Av.Hsi.Origin.X-1,Av.Hsi.Origin.Z,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,Av.Hsi.Origin.X+6,Av.Hsi.Origin.Z,Av.Hsi.Origin.X+1,Av.Hsi.Origin.Z,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,Av.Hsi.Origin.X,Av.Hsi.Origin.Z+1,Av.Hsi.Origin.X,Av.Hsi.Origin.Z+8,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,Av.Hsi.Origin.X-1,Av.Hsi.Origin.Z+7,Av.Hsi.Origin.X+1,Av.Hsi.Origin.Z+7,Triple.Red,Triple.Green,Triple.Blue,0);

	if ((PlayerPlane->Status & PL_AI_DRIVEN) && (PlayerPlane->AutoPilotMode & (PL_AP_CPL | PL_AP_ROLL | PL_AP_HDG)))
	{
		char *small_text;

		if (PlayerPlane->AutoPilotMode & PL_AP_CPL)
		{
			switch (Av.Hsi.SteeringMode)
			{
				case HSI_TCN_MODE:
					small_text = "TCN";
					break;
				case HSI_TGT_MODE:
					small_text = "TGT";
					break;
				case HSI_WPT_MODE:
					small_text = "WPT";
					break;
				case HSI_GPS_MODE:
					small_text = "GPS";
					break;
				case HSI_ACL_MODE:
					small_text = "ACL";
					break;
			}
		}
		else
			if (PlayerPlane->AutoPilotMode & PL_AP_ROLL)
				small_text = "BNK";
			else
				small_text = "HDG";

		DrawTextAtLoc(Av.Hsi.Origin.X-12,Av.Hsi.Origin.Z-6,"CPL",AV_GREEN,1.0);
		DrawTextAtLoc(Av.Hsi.Origin.X+1,Av.Hsi.Origin.Z-6,small_text,AV_GREEN,1.0);

	}


	double speed = PlayerPlane->IfHorzVelocity*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	sprintf(TmpStr,"%dG",(int)speed);
	DrawTextAtLoc(Av.Hsi.Origin.X+3,Av.Hsi.Origin.Z+3,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%dT",(int)(PlayerPlane->BfLinVel.X*(FTSEC_TO_MLHR*MLHR_TO_KNOTS)));
	DrawTextAtLoc(Av.Hsi.Origin.X-17,Av.Hsi.Origin.Z+3,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(12,117,"HSEL",AV_GREEN,1.0);
  IntTo3DigitFont((int)Av.Hsi.HSelect,TmpStr);
	DrawTextAtLoc(13,123,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(114,117,"CSEL",AV_GREEN,1.0);
  IntTo3DigitFont((int)Av.Hsi.CSelect,TmpStr);
	DrawTextAtLoc(114,123,TmpStr,AV_GREEN,1.0);

  float Angle;
  int   TotalLength = (Av.Hsi.DisplayMode == HSI_TUP) ? 53 : 105;
  int   End1X,End1Y;
  float TotalAngle;
  float EAngle,NAngle,SAngle,WAngle;
  float Heading,PreciseHeading;
  int   TempCenterX, TempCenterY;

	Heading = PlayerPlane->Heading/DEGREE;

  PreciseHeading = PlayerPlane->Heading*100;
  PreciseHeading = PreciseHeading/DEGREE;
  PreciseHeading = PreciseHeading/100.0;

  NAngle = NormDegree(360.0 - Heading + 90.0);
  SAngle = NormDegree(NAngle + 180.0);
  EAngle = NormDegree(NAngle + 270.0);
  WAngle = NormDegree(NAngle + 90.0);

  TempCenterX = Av.Hsi.Origin.X;
  TempCenterY = Av.Hsi.Origin.Z;

	if( TRUE )
  {
    Angle = NAngle;
    TotalAngle = 0.0;

	  int Tick = 36;

    while(TotalAngle < 360.0)
    {
	    End1X = TempCenterX + ICosTimes((Angle*DEGREE), TotalLength);
      End1Y = TempCenterY - ISinTimes((Angle*DEGREE), TotalLength);

	  	if(Angle == NAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"N",AV_GREEN,1.0);
    	}
	  	else if(Angle == SAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"S",AV_GREEN,1.0);
 	  	}
    	else if(Angle == EAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"E",AV_GREEN,1.0);
			}
	  	else if(Angle == WAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"W",AV_GREEN,1.0);
 	  	}
			else if(fmod(TotalAngle,30) == 0)
			{
				sprintf(TmpStr,"%d",Tick);
				DrawTextAtLoc(End1X-2,End1Y-2,TmpStr,AV_GREEN,1.0);
			}
	    else
			{
	      DrawAvDot(End1X,End1Y,AV_GREEN,1.0);
			}

		  Tick--;
	  	TotalAngle += 10.0;
	  	Angle = NormDegree(Angle + 10.0);
		}
  } // declutter not on

	DrawHsiTacan();
	DrawHsiGroundTarget();
	DrawHsiWpt();
	DrawHsiAcl();
	DrawHsiGps();
	DrawHSIGroundTrack();

	DrawHsiBullseye();
	DrawHsiLs();
	DrawHsiFlir();
	DrawHsiRadar();

  	DrawHsiNavInfo();

	DrawHSel();

	if(Av.Hsi.SequenceOn)
    DrawHsiWaypoints();

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 0:  HsiSetTcn();
				       break;
			case 1:  HsiSetGps();
				       break;
			case 2:  HsiSetAcl();
				       break;
			case 3:  HsiSetTup();
				       break;
			case 4:  HsiSetDctr();
				       break;
			case 5:  HsiToggleSensors();
				       break;
			case 7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				       break;
			case 8:  HsiToggleTime();
				       break;
			case 9:  HsiToggleAuto();
				       break;
			case 10: HsiToggleSeq();
				       break;
			case 11: HsiSteeringDown();
				       break;
 			case 12: HsiSteeringUp();
				       break;
			case 13: HsiSetTgt();
				       break;
			case 14: HsiSetWpt();
				       break;
			case 15: HsiToggleData();
				       break;
			case 16: HsiToggleCsel();
				       break;
			case 17: HsiIncrementRange();
				       break;
			case 19: HsiToggleMap();
				       break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


void HSIMode(int MpdNum)
{
	if(Av.Hsi.SteeringMode == HSI_TGT_MODE)
	{
		 if(PlayerPlane->AGDesignate.X == -1)
			Av.Hsi.SteeringMode  = HSI_WPT_MODE;
	}

	if(Av.Hsi.DataOn)
	{
		HsiDataMode(MpdNum);
	}
	else
	{
		DoHsi(MpdNum);
	}
}

//*****************************************************************************************************************************************
// ADI MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

// polygons to cover the MDI field

float cp[] = {   -1,
	   			141,
	   			141,
	   			 -1,

				 -4,
				 -4,
				 18,
				 18,


				 -1,
				 25,
				 25,
				 -1,

				 17,
				 17,
				 141,
				 141,


				 115,
				 141,
				 141,
				 115,

				 17,
				 17,
				 141,
				 141,


				 25,
				 116,
				 116,
				 25,

				 110,
				 110,
				 141,
				 141};



void DrawADIBall(float X,float Y)
{
	float XOfs = X;
	float YOfs = Y;

	signed short TmpPitch = (signed short)((signed short)PlayerPlane->Pitch/(10));
	ANGLE TmpRoll =  PlayerPlane->Roll;
	extern inline float RadToDeg(float Rad);
	ANGLE TmpHeading = -RadToDeg(PlayerPlane->Beta)*DEGREE;

	if ((signed short)TmpPitch > (signed short)(3.5*DEGREE)) TmpPitch = (signed short)(3.5*DEGREE);
	else
	if ((signed short)TmpPitch < -(signed short)(3.5*DEGREE)) TmpPitch = -(signed short)(3.5*DEGREE);

	FMatrix	 RadarAttitude;
	RadarAttitude.SetHPR(0,-TmpPitch,-TmpRoll);

	for (int i=0; i<6; i++)
	{
		ADIAirToAirLadder.RadarPoints[i].Rotated.RotateAndPerspect((int *)&ADIAirToAirLadder.RadarPoints[i].iSX,
																(int *)&ADIAirToAirLadder.RadarPoints[i].iSY,
																	   &RadarAttitude,
	 																   &ADIAirToAirLadder.RawRadarPoint[i]);

		// Translate Rotated points to corrent MPD
		ADIAirToAirLadder.RadarPoints[i].iSX -= GrBuffFor3D->MidX-XOfs;//320
		ADIAirToAirLadder.RadarPoints[i].iSY -= GrBuffFor3D->MidY -YOfs;//116
	}

	AvRGB Triple;

	FPoint RotAtt[6];
	ZeroMemory(RotAtt,sizeof(FPoint)*6.0);

	for(i=0; i<6; i++)
	{
		RotAtt[i].X = ADIAirToAirLadder.RadarPoints[i].iSX;
		RotAtt[i].Z = ADIAirToAirLadder.RadarPoints[i].iSY;
	}

	float Xs[4];
	float Ys[4];
	FPointData Colors[4];

	GetRGBFromAVPalette(AV_YELLOW,1.0,&Triple,TRUE);

	PushClipRegion();

	if( GrBuffFor3D->pGrBuffPolygon)
	{
		GrBuffFor3D->ClipLeft = 0;
		GrBuffFor3D->ClipRight = 140;

		GrBuffFor3D->ClipTop = 0;
		GrBuffFor3D->ClipBottom = 140;

		GrBuffFor3D->MidX = 70;
		GrBuffFor3D->MidY = 60;

		// Turn on 2nd pass
	 	SetSecondPass( TRUE, NULL );

		Set3DScreenClip();

	}

	for(i=0; i<4; i++)
	{
		Colors[i].X = Triple.Red;
		Colors[i].Y = Triple.Green;
		Colors[i].Z = Triple.Blue;
	}

	FPointDouble Vec,P1,P2;

	Vec = RotAtt[4];
	Vec -= RotAtt[0];
	Vec.Normalize();

	Vec *= 120.0;

	P1 =  RotAtt[0];
	P1 += Vec;

	P2 =  RotAtt[3];
	P2 += Vec;

	Xs[0] = RotAtt[0].X; Xs[1] = RotAtt[3].X; Xs[2] = P2.X; Xs[3] = P1.X;
	Ys[0] = RotAtt[0].Z; Ys[1] = RotAtt[3].Z; Ys[2] = P2.Z;	Ys[3] = P1.Z;

	GrDrawColorPoly(GrBuffFor3D,4,Xs,Ys,Colors,1.0);

	GetRGBFromAVPalette(AV_RED,1.0,&Triple,TRUE);

	for(i=0; i<4; i++)
	{
		Colors[i].X = Triple.Red;
		Colors[i].Y = Triple.Green;
		Colors[i].Z = Triple.Blue;
	}

	Vec = RotAtt[4];
	Vec -= RotAtt[0];
	Vec.Normalize();

	Vec *= -120.0;

	P1 = RotAtt[0];
	P1 += Vec;

	P2 = RotAtt[3];
	P2 += Vec;

	Xs[0] =  RotAtt[0].X; Xs[1] = P1.X; Xs[2] = P2.X; Xs[3] = RotAtt[3].X;
	Ys[0] =  RotAtt[0].Z; Ys[1] = P1.Z; Ys[2] = P2.Z; Ys[3] = RotAtt[3].Z;

	GrDrawColorPoly(GrBuffFor3D,4,Xs,Ys,Colors,1.0);

	for(i=0; i<4; i++)
	{
		Colors[i].X = 0;
		Colors[i].Y = 14.0f*(1.0f/255.0f);
		Colors[i].Z = 0;
	}

	GrDrawColorPoly(GrBuffFor3D,4,&cp[0],&cp[4],Colors,1.0);
	GrDrawColorPoly(GrBuffFor3D,4,&cp[8],&cp[12],Colors,1.0);
	GrDrawColorPoly(GrBuffFor3D,4,&cp[16],&cp[20],Colors,1.0);
	GrDrawColorPoly(GrBuffFor3D,4,&cp[24],&cp[28],Colors,1.0);


	if( GrBuffFor3D->pGrBuffPolygon)
		SetSecondPass( FALSE, NULL);

	PopClipRegion();
	Set3DScreenClip();
	PushClipRegion();

//	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

//	GrDrawLine(GrBuffFor3D,RotAtt[0].X,RotAtt[0].Z,RotAtt[1].X,RotAtt[1].Z,Triple.Red,Triple.Green,Triple.Blue,0);
//	GrDrawLine(GrBuffFor3D,RotAtt[2].X,RotAtt[2].Z,RotAtt[3].X,RotAtt[3].Z,Triple.Red,Triple.Green,Triple.Blue,0);
//	GrDrawLine(GrBuffFor3D,RotAtt[0].X,RotAtt[0].Z,RotAtt[4].X,RotAtt[4].Z,Triple.Red,Triple.Green,Triple.Blue,0);
//	GrDrawLine(GrBuffFor3D,RotAtt[3].X,RotAtt[3].Z,RotAtt[5].X,RotAtt[5].Z,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawPolyCircleArcPolyBuff( GrBuffFor3D, 70, 60, 40, 75, 0, 360, 36, 0.0,14,0.0,0);//110

}



//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float AdiAngleLines[] = {(90.0f + 30.0f)*DEGREES_TO_RADIANS,
						 10.0f,
						 (90.0f + 60.0f)*DEGREES_TO_RADIANS,
						 10.0f,
						 (90.0f + 70.0f)*DEGREES_TO_RADIANS,
						 5.0f,
						 (90.0f + 80.0f)*DEGREES_TO_RADIANS,
						 5.0f,
						 (90.0f + 90.0f)*DEGREES_TO_RADIANS,
						 10.0f,
						 (90.0f + 100.0f)*DEGREES_TO_RADIANS,
						 5.0f,
						 (90.0f + 110.0f)*DEGREES_TO_RADIANS,
						 5.0f,
						 (90.0f + 120.0f)*DEGREES_TO_RADIANS,
						 10.0f,
						 (90.0f + 150.0f)*DEGREES_TO_RADIANS,
						 10.0f};

void AdiMode(int MpdNum)
{

	AvRGB Triple;

	// draw red and yellow parts
	DrawADIBall(70,60);

	// draw green circle to mask them out
	Triple.Red = 0;
	Triple.Green = 14;
	Triple.Blue = 0;


  	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	/* -----------------10/20/99 12:22PM-------------------------------------------------------------------
	/* draw the angle reference lines around the bottom of the ball
	/* ----------------------------------------------------------------------------------------------------*/
	float *fwalk;
	fwalk = &AdiAngleLines[16];
	while(fwalk >= AdiAngleLines)
	{
		float x1,y1,x2,y2;

		x1 = -sin(*fwalk);
		y1 = -cos(*fwalk);

		x2 = x1;
		y2 = y1;

		x1 *= 40.0f;
		y1 *= 40.0f;

		x1 += 70;
		y1 += 60;

		x2 *= 40.0f + fwalk[1];
		y2 *= 40.0f + fwalk[1];

		x2 += 70;
		y2 += 60;

		GrDrawLine(GrBuffFor3D,x1,y1,x2,y2,Triple.Red,Triple.Green,Triple.Blue,0);
		fwalk -= 2;
	}

	/* -----------------10/19/99 5:52PM--------------------------------------------------------------------
	/* this is the circle that goes around the edge of the ball
	/* ----------------------------------------------------------------------------------------------------*/

	GrDrawPolyCircleArcPolyBuff( GrBuffFor3D, 70, 60, 40, 41, 0, 360, 36, Triple.Red,Triple.Green,Triple.Blue,0);

	/* -----------------10/20/99 12:26PM-------------------------------------------------------------------
	/* the yawrate reference boxes
	/* ----------------------------------------------------------------------------------------------------*/
	DrawBox(GrBuffFor3D,42,116,14,5,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,63,116,14,5,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,84,116,14,5,Triple.Red,Triple.Green,Triple.Blue);

	/* -----------------10/20/99 12:23PM-------------------------------------------------------------------
	/* slide bottommost box to indicate yaw rate
	/* ----------------------------------------------------------------------------------------------------*/
	float yawbox = PlayerPlane->BfRotVel.Z * (RADIANS_TO_DEGREES*70.0f / 10.0f) + 63.0f;
	if (yawbox < 0.0)
		yawbox = 0.0;
	else
		if (yawbox > (139-14))
			yawbox = (139-14);

	DrawBox(GrBuffFor3D,yawbox,124,14,5,Triple.Red,Triple.Green,Triple.Blue);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// draw boxes for the airspeed and altitude
	DrawBox(GrBuffFor3D,12,9,22,11,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,103,9,26,11,Triple.Red,Triple.Green,Triple.Blue);

	// draw airspeed
	sprintf(TmpStr,"%3d",(int)PlayerPlane->IndicatedAirSpeed);
	DrawTextAtLoc(16,11,TmpStr,AV_GREEN,1.0,TRUE);
//	RightJustifyTextAtLocation(32,11,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	// draw altitude
	int Alt = (int)__max(0.0,PlayerPlane->Altitude);
	sprintf(TmpStr,"%2d",Alt/1000);
	DrawTextAtLoc(106,11,TmpStr,AV_GREEN,1.0,TRUE);

	sprintf(TmpStr,"%03d",abs(Alt % 1000));
	DrawTextAtLoc(116,13,TmpStr,AV_GREEN,1.0);

//	sprintf(TmpStr,"%d",(int)(PlayerPlane->IfVelocity.Y));
//	DrawTextAtLoc(107,3,TmpStr,AV_GREEN,1.0);

	/* -----------------10/19/99 4:50PM--------------------------------------------------------------------
	/* horizontal line across the middle
	/* ----------------------------------------------------------------------------------------------------*/
	GrDrawLine(GrBuffFor3D,20,60,64,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,76,60,120,60,Triple.Red,Triple.Green,Triple.Blue,0);

	/* -----------------10/19/99 4:50PM--------------------------------------------------------------------
	/* little W notch in the middle
	/* ----------------------------------------------------------------------------------------------------*/
	GrDrawLine(GrBuffFor3D,64,60,67,63,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,67,63,70,60,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,70,60,73,63,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,73,63,76,60,Triple.Red,Triple.Green,Triple.Blue,0);

//	float DegreePerSec = 0;

//	float Offset = DegreePerSec*7.3;   // 7.3 = 22 pixels per 3 degrees 22/3

//	float OriginX = 101;

	//DrawBox(GrBuffFor3D,OriginX + Offset,163,14,5,Triple.Red,Triple.Green,Triple.Blue);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				      break;
		}
	}

}

//*****************************************************************************************************************************************
// ENG MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern float EMD_lSpin;
extern float EMD_rSpin;

extern float EMD_lTemp;
extern float EMD_rTemp;

extern float EMD_Amad_lTemp;
extern float EMD_Amad_rTemp;

extern float EMD_lFlow;
extern float EMD_rFlow;

extern float EMD_lOpen;
extern float EMD_rOpen;

extern float EMD_lOlio;
extern float EMD_rOlio;

float EMD_lFanVib;
float EMD_lCpr;
float EMD_lEpr;
float EMD_lCdp;

float EMD_rFanVib;
float EMD_rCpr;
float EMD_rEpr;
float EMD_rCdp;

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void EngMode(int MpdNum)
{
    AvRGB Triple;
    GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	  DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

		// center column
		DrawTextAtLoc(52,12,"ENG STATUS",AV_GREEN,1.0);
		DrawTextAtLoc(53,19,"INLET TEMP",AV_GREEN,1.0);
		DrawTextAtLoc(60,26,"N1 RPM",AV_GREEN,1.0);
		DrawTextAtLoc(60,33,"N2 RPM",AV_GREEN,1.0);
		DrawTextAtLoc(65,40,"EGT",AV_GREEN,1.0);
		DrawTextAtLoc(67,47,"FF",AV_GREEN,1.0);
		DrawTextAtLoc(58,54,"NOZ POS",AV_GREEN,1.0);
		DrawTextAtLoc(55,61,"OIL PRESS",AV_GREEN,1.0);
		DrawTextAtLoc(59,68,"THRUST",AV_GREEN,1.0);
		DrawTextAtLoc(59,75,"FAN VIB",AV_GREEN,1.0);
		DrawTextAtLoc(65,82,"EPR",AV_GREEN,1.0);
		DrawTextAtLoc(65,89,"CDP",AV_GREEN,1.0);
		DrawTextAtLoc(65,96,"CPR",AV_GREEN,1.0);
		DrawTextAtLoc(65,103,"THA",AV_GREEN,1.0);
		DrawTextAtLoc(48,110,"AMAD OIL TEMP",AV_GREEN,1.0);
		DrawTextAtLoc(51,117,"ENG OIL TEMP",AV_GREEN,1.0);

		int LThrottle = 0;
		int RThrottle = 0;

		if(PlayerPlane->LeftThrottlePos == 0.0)
			LThrottle = 1;
		else if(PlayerPlane->LeftThrottlePos >= 80.0)
			LThrottle = 2;
		else if(PlayerPlane->LeftThrottlePos >= 70.0)
			LThrottle = 3;

   	if(PlayerPlane->RightThrottlePos == 0.0)
			RThrottle = 1;
		else if(PlayerPlane->RightThrottlePos >= 80.0)
			RThrottle = 2;
		else if(PlayerPlane->RightThrottlePos >= 70.0)
			RThrottle = 3;

		// Epr
		if(!LeftEngineOn)
			EMD_lEpr = PlayerPlane->LeftThrustPercent / 100.0f;
		else
			if(LThrottle == 1)
				EMD_lEpr = 1.0;
			else if(LThrottle == 2)
				EMD_lEpr = 1.5;
			else if(LThrottle == 3)
				EMD_lEpr = 1.8;
			else
				EMD_lEpr = 1.0 + (PlayerPlane->LeftThrottlePos/100.0)*0.8;

		if(!RightEngineOn)
			EMD_rEpr = PlayerPlane->RightThrustPercent / 100.0f;
		else
			if(RThrottle == 1)
				EMD_rEpr = 1.0;
			else if(RThrottle == 2)
				EMD_rEpr = 1.5;
			else if(RThrottle == 3)
				EMD_rEpr = 1.8;
			else
				EMD_rEpr = 1.0 + (PlayerPlane->RightThrottlePos/100.0)*0.8;

		if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_ENGINE)
		{
			EMD_lEpr -= 0.3;
			if (EMD_lEpr < 0.0f)
				EMD_lEpr = 0.0f;
		}


		if(PlayerPlane->DamageFlags & DAMAGE_BIT_R_ENGINE)
		{
			EMD_rEpr -= 0.3;
			if (EMD_rEpr < 0.0f)
				EMD_rEpr = 0.0f;
		}

		// fan vib
		if(!LeftEngineOn)
			EMD_lFanVib = 0;
		else
		{
			if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_ENGINE)
			{
				EMD_lFanVib = 1.6;
				EMD_lFanVib += (PlayerPlane->LeftThrottlePos/100.0)*1.4;
			}
			else
			{
				EMD_lFanVib = 0.7;
				EMD_lFanVib += (PlayerPlane->LeftThrottlePos/100.0)*0.6;
			}
		}

		if(!RightEngineOn)
			EMD_rFanVib = 0;
		else
		{
			if(PlayerPlane->DamageFlags & DAMAGE_BIT_R_ENGINE)
			{
				EMD_rFanVib = 1.6;
				EMD_rFanVib += (PlayerPlane->RightThrottlePos/100.0)*1.4;
			}
			else
			{
				EMD_rFanVib = 0.7;
				EMD_rFanVib += (PlayerPlane->RightThrottlePos/100.0)*0.6;
			}
		}

		// cdp
		if(!LeftEngineOn)
			EMD_lCdp = 0.0;
		else
		{
				if(LThrottle == 1)
					EMD_lCdp = 60.0;
				else if(LThrottle >= 2)
					EMD_lCdp = 120;
				else
					EMD_lCdp = 60.0 + (PlayerPlane->LeftThrottlePos/100.0)*60.0;
		}

		if(!RightEngineOn)
			EMD_rCdp = 0.0;
		else
		{
				if(RThrottle == 1)
					EMD_rCdp = 60.0;
				else if(RThrottle >= 2)
					EMD_rCdp = 120;
				else
					EMD_rCdp = 60.0 + (PlayerPlane->RightThrottlePos/100.0)*60.0;
		}

		if( (PlayerPlane->DamageFlags & DAMAGE_BIT_L_ENGINE) && LeftEngineOn)
			EMD_lCdp -= 15.0;

		if( (PlayerPlane->DamageFlags & DAMAGE_BIT_R_ENGINE) && RightEngineOn)
			EMD_rCdp -= 10.5;

		// cpr
		if(!LeftEngineOn)
			EMD_lCpr = PlayerPlane->LeftThrustPercent / 100.0f;
		else
			if(LThrottle == 1)
				EMD_lCpr = 1.0;
			else if(LThrottle >= 2)
				EMD_lCpr = 1.8;
			else
				EMD_lCpr = 1.0 + (PlayerPlane->LeftThrottlePos/100.0)*0.8;

		if(!RightEngineOn)
			EMD_rCpr = PlayerPlane->RightThrustPercent / 100.0f;
		else
			if(RThrottle == 1)
				EMD_rCpr = 1.0;
			else if(RThrottle >= 2)
				EMD_rCpr = 1.8;
			else
				EMD_rCpr = 1.0 + (PlayerPlane->RightThrottlePos/100.0)*0.8;

		if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_ENGINE)
		{
			EMD_lCpr -= 0.3;
			if (EMD_lCpr < 0.0f)
				EMD_lCpr = 0.0f;
		}

		if(PlayerPlane->DamageFlags & DAMAGE_BIT_R_ENGINE)
		{
			EMD_rCpr -= 0.3;
			if (EMD_rCpr < 0.0f)
				EMD_rCpr = 0.0f;
		}

		float N1LeftVar  = 32.0 + ((EMD_lSpin - 61.0)/39.0)*68.0;
		float N1RightVar = 32.0 + ((EMD_rSpin - 61.0)/39.0)*68.0;

		// LEFT COL
		RightJustifyTextAtLocation(31,5,"LEFT",Triple.Red,Triple.Green,Triple.Blue);

		if(!LeftEngineOn)
			sprintf(TmpStr,"SHUTDOWN");
		else if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_ENGINE)
			sprintf(TmpStr,"PERF 90");
		else if(PlayerPlane->DamageFlags & DAMAGE_BIT_L_BURNER)
			sprintf(TmpStr,"AB FAIL");
		else
			sprintf(TmpStr,"NORMAL");

		RightJustifyTextAtLocation(31,12,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%d", 10 /*InletVar*/);
		RightJustifyTextAtLocation(31,19,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		float wtemp = N1LeftVar;

		if (wtemp < 0.0f)
			wtemp = 0.0f;
		wtemp += 17*(100.0f-wtemp)/100.0f;
		sprintf(TmpStr,"%3.0f",wtemp  /*N1Var*/);
		RightJustifyTextAtLocation(31,26,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f", EMD_lSpin+9*(100.0f-EMD_lSpin)/100.0f /*N2Var*/);
		RightJustifyTextAtLocation(31,33,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f",EMD_lTemp /*EgtVar*/);
		RightJustifyTextAtLocation(31,40,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f", UFC.LeftFuelFlowRate*60 /*FFVar*/);
		RightJustifyTextAtLocation(31,47,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f", EMD_lOpen /*NozPos*/);
		RightJustifyTextAtLocation(31,54,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp = EMD_lOlio * (1.0f/80.0f);
		wtemp *= wtemp;
		wtemp *= 150.0f;
		wtemp+=20.0f+40*EMD_lOlio * (1.0f/80.0f);
		sprintf(TmpStr,"%3.0f", wtemp /*OilVar*/);
		RightJustifyTextAtLocation(31,61,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f", PlayerPlane->LeftThrustPercent /*ThrustVar*/);
		RightJustifyTextAtLocation(31,68,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f",EMD_lFanVib /*FanVar*/);
		RightJustifyTextAtLocation(31,75,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f", EMD_lEpr /*EprVar*/);
		RightJustifyTextAtLocation(31,82,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f",EMD_lCdp /*CdpVar*/);
		RightJustifyTextAtLocation(31,89,TmpStr,Triple.Red,Triple.Green,Triple.Blue);


		sprintf(TmpStr,"%3.1f", EMD_lCpr /*CprVar*/);
		RightJustifyTextAtLocation(31,96,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f",PlayerPlane->LeftThrottlePos /*ThaVar*/);
		RightJustifyTextAtLocation(31,103,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp =(EMD_Amad_lTemp-575.0f)*(10.0f/425.0f)+55.0f;

		if (wtemp < 55.0f)
			wtemp = 55.0f;

		sprintf(TmpStr,"%2.0f",wtemp/*AmadVar*/);
		RightJustifyTextAtLocation(31,110,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp =(EMD_Amad_lTemp-575.0f)*(70.0f/425.0f)+55.0f;
		sprintf(TmpStr,"%3.0f",wtemp /*EngVar*/);
		RightJustifyTextAtLocation(31,117,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		// RIGHT COL
		RightJustifyTextAtLocation(120,5,"RIGHT",Triple.Red,Triple.Green,Triple.Blue);

		if(!RightEngineOn)
			sprintf(TmpStr,"SHUTDOWN");
		else if(PlayerPlane->DamageFlags & DAMAGE_BIT_R_ENGINE)
			sprintf(TmpStr,"PERF 90");
		else if(PlayerPlane->DamageFlags & DAMAGE_BIT_R_BURNER)
			sprintf(TmpStr,"AB FAIL");
		else
			sprintf(TmpStr,"NORMAL");

		RightJustifyTextAtLocation(120,12,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%d", 10 /*R_InletVar*/);
		RightJustifyTextAtLocation(120,19,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp = N1RightVar;
		if (wtemp < 0.0f)
			wtemp = 0.0f;
		wtemp += 17*(100.0f-wtemp)/100.0f;
		sprintf(TmpStr,"%3.0f", wtemp /*R_N1Var*/);
		RightJustifyTextAtLocation(120,26,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f", EMD_rSpin +9*(100.0f-EMD_rSpin)/100.0f/*R_N2Var*/);
		RightJustifyTextAtLocation(120,33,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f",EMD_rTemp/*R_EgtVar*/);
		RightJustifyTextAtLocation(120,40,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f", UFC.RightFuelFlowRate*60 /*R_FFVar*/);
		RightJustifyTextAtLocation(120,47,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f", EMD_rOpen /*R_NozVar*/);
		RightJustifyTextAtLocation(120,54,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp = EMD_rOlio * (1.0f/80.0f);
		wtemp *= wtemp;
		wtemp *= 150.0f;
		wtemp+=20.0f+40*EMD_rOlio * (1.0f/80.0f);
		sprintf(TmpStr,"%3.0f", wtemp /*R_OilVar*/);

		RightJustifyTextAtLocation(120,61,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f", PlayerPlane->RightThrustPercent /*R_ThrustVar*/);
		RightJustifyTextAtLocation(120,68,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f",EMD_rFanVib /*R_FanVar*/);
		RightJustifyTextAtLocation(120,75,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f", EMD_rEpr /*R_EprVar*/);
		RightJustifyTextAtLocation(120,82,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f",EMD_rCdp /*R_CdpVar*/);
		RightJustifyTextAtLocation(120,89,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.1f", EMD_rCpr /*R_CprVar*/);
		RightJustifyTextAtLocation(120,96,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f",PlayerPlane->RightThrottlePos/*R_ThaVar*/);
		RightJustifyTextAtLocation(120,103,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp =(EMD_Amad_rTemp-575.0f)*(10.0f/425.0f)+55.0f;

		if (wtemp < 55.0f)
			wtemp = 55.0f;

		sprintf(TmpStr,"%2.0f",wtemp /*R_AmadVar*/);
		RightJustifyTextAtLocation(120,110,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp =(EMD_Amad_rTemp-575.0f)*(70.0f/425.0f)+55.0f;
		sprintf(TmpStr,"%4.0f",wtemp /*R_EngVar*/);
		RightJustifyTextAtLocation(120,117,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		int ButtonPress = GetButtonPress(MpdNum);
		if(ButtonPress != -1)
		{
			switch(ButtonPress)
			{
				case 7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				       	break;
			}
		}
}

//*****************************************************************************************************************************************
// BIT MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AV_FCS_CAT    0
#define AV_SENSOR_CAT 1
#define AV_STORES_CAT 2
#define AV_ENG_CAT    3
#define AV_AMAD_CAT   4
#define AV_DISP_CAT   5
#define AV_MISC_CAT   6
#define AV_EW_CAT     7

typedef struct
{
	char Label[20];
	int  Index;
	int  Category;
	int  MessageIndex;
} BitType;

char *BitMessages[] = { "NO GO", "EMPTY", "NOT RDY", "DEGR", "GO", "OVRD"  };


BitType BitList[] = { {"L BLEED AIR" ,AV_L_BLEED_AIR  ,AV_ENG_CAT,4},
											{"L FADEC"     ,AV_L_ENG_CONT	  ,AV_ENG_CAT,4},
											{"L OIL PRESS" ,AV_L_OIL_PRESS  ,AV_ENG_CAT,4},
											{"L BURNER"    ,AV_L_BURNER     ,AV_ENG_CAT,4},
											//{"LO ENGINE"   ,AV_LO_ENGINE	  ,AV_ENG_CAT,4},
											{"L ENGINE"    ,AV_L_ENGINE		  ,AV_ENG_CAT,4},
											{"R BLEED AIR" ,AV_R_BLEED_AIR  ,AV_ENG_CAT,4},
											{"R FADEC"     ,AV_R_ENG_CONT	  ,AV_ENG_CAT,4},
											{"R OIL PRESS" ,AV_R_OIL_PRESS  ,AV_ENG_CAT,4},
											{"R BURNER"    ,AV_R_BURNER		  ,AV_ENG_CAT,4},
											//{"RO ENGINE"   ,AV_RO_ENGINE	  ,AV_ENG_CAT,4},
											{"R ENGINE"    ,AV_R_ENGINE		  ,AV_ENG_CAT,4},

											{"L FUEL PUMP" ,AV_L_FUEL_PUMP  ,AV_AMAD_CAT,4},
											{"L GEN"       ,AV_L_GEN		    ,AV_AMAD_CAT,4},
											{"HYD 1A"      ,AV_L_MAIN_HYD	  ,AV_AMAD_CAT,4},
											{"HYD 1B"      ,AV_NO_HYD1B     ,AV_AMAD_CAT,4},
											{"R FUEL PUMP" ,AV_R_FUEL_PUMP  ,AV_AMAD_CAT,4},
											{"R GEN"       ,AV_R_GEN		    ,AV_AMAD_CAT,4},
											{"HYD 2A"      ,AV_R_MAIN_HYD	  ,AV_AMAD_CAT,4},
											{"HYD 2B"      ,AV_EMER_HYD		  ,AV_AMAD_CAT,4},
								//			{"FUEL TANKS"  ,AV_FUEL_TANKS   ,AV_AMAD_CAT,4},

											{"FCS"         ,AV_FLT_CONT		  ,AV_FCS_CAT,4},
											{"TARGET IR"   ,AV_TARGET_IR	  ,AV_SENSOR_CAT,4},
											{"SMS"         ,AV_PACS			    ,AV_STORES_CAT,4},
											{"MC1"         ,AV_CEN_COMP		  ,AV_FCS_CAT,4},
											{"RADAR"       ,AV_RADAR		    ,AV_SENSOR_CAT,4},
											{"ADC"         ,AV_ADC			    ,AV_MISC_CAT,4},
											{"OBOGS"       ,AV_OXYGEN		    ,AV_MISC_CAT,4},
											{"AUTOPILOT"   ,AV_AUTOPILOT	  ,AV_FCS_CAT,4},
											{"NAVFLIR"     ,AV_NAV_FLIR		  ,AV_SENSOR_CAT,4},
											{"GUN"         ,AV_GUN 			    ,AV_STORES_CAT,4},
											{"RWR"         ,AV_TEWS 		    ,AV_EW_CAT,4},
											{"HUD"         ,AV_HUD   		    ,AV_DISP_CAT,4},
											{"CHAFF"       ,AV_NO_CHAFF		  ,AV_EW_CAT,4},
											{"FLARES"      ,AV_NO_FLARE     ,AV_EW_CAT,4},
											{"DECOYS"      ,AV_NO_DECOY     ,AV_EW_CAT,4},
											{"MC2"         ,AV_MC2          ,AV_FCS_CAT,4},
											{"L MDI"       ,AV_NO_L_MDI     ,AV_DISP_CAT,4},
											{"R MDI"       ,AV_NO_R_MDI     ,AV_DISP_CAT,4},
										//	{"TFLIR"       ,AV_NO_TFLIR     ,AV_STORES_CAT,4},
											{"G LIMIT"     ,AV_G_LIMIT      ,AV_MISC_CAT,4},
											{"L AMAD"      ,AV_NO_L_AMAD    ,AV_AMAD_CAT,4},
											{"R AMAD"      ,AV_NO_R_AMAD    ,AV_AMAD_CAT,4}};


int NumDamagedBit;
BitType *BitDamageList[AV_MAX_DAMAGE];

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitBit()
{
	NumDamagedBit = 0;
	Av.Bit.CurBitPage = -1;
	Av.Bit.CurDisplayPage = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetBitMessage(int Damage,int MessageType)
{
	BitType *Ptr = &BitList[0];

	int i=0;
	while(i < AV_MAX_DAMAGE)
	{
		 if(Ptr->Index == Damage)
		 {
			 Ptr->MessageIndex = MessageType;
			 break;
		 }
		 i++;
		 Ptr++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddDamageToBit(int Damage,int MessageType)
{
	BitType *Ptr = &BitList[0];

	int i=0;
	while(i < AV_MAX_DAMAGE)
	{
		 if(Ptr->Index == Damage)
		 {
		 	  if(NumDamagedBit == 0)
				{
					 NumDamagedBit = 1;
					 BitDamageList[0] = Ptr;
					 Ptr->MessageIndex = MessageType;
				}
				else
				{
				  int j = NumDamagedBit-1;
					while(j >= 0)
					{
				    BitDamageList[j+1] = BitDamageList[j];
						j--;
					}
					BitDamageList[0] = Ptr;
		 		  Ptr->MessageIndex = MessageType;
					NumDamagedBit++;
				}
				break;
		 }
		 i++;
		 Ptr++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetBitColor(int Cat)
{
	 int Color = AV_GREEN;

	 switch(Cat)
	 {
		case AV_BIT_NO_GO    : Color = AV_RED; break;
		case AV_BIT_EMPTY    : Color = AV_RED; break;
	  case AV_BIT_NOT_READY: Color = AV_RED; break;
		case AV_BIT_DEGD     : Color = AV_YELLOW; break;
		case AV_BIT_GO       : Color = AV_GREEN; break;
    case AV_BIT_OVRD     : Color = AV_YELLOW; break;
	 }

	 return(Color);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetBitCategoryStatus(int Cat)
{
	 int Result = AV_BIT_GO;
	 int GoFound = FALSE;

 	 BitType *Ptr = &BitList[0];
	 int i=0;
	 while(i < AV_MAX_DAMAGE)
	 {
		 if(Ptr->Category == Cat)
		 {
				if(Ptr->MessageIndex == AV_BIT_GO)
					GoFound = TRUE;

				if( (Ptr->MessageIndex == AV_BIT_NO_GO) || (Ptr->MessageIndex == AV_BIT_EMPTY) || (Ptr->MessageIndex == AV_BIT_NOT_READY) )
					Result = AV_BIT_NO_GO;
		 }

		 i++;
		 Ptr++;
	 }

	 if( (Result == AV_BIT_NO_GO) && GoFound)
			Result = AV_BIT_DEGD;

	 return(Result);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void BitPage(int MpdNum)
{
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,17,"BIT",FALSE,AV_GREEN,1.0);

	 switch(Av.Bit.CurBitPage)
	 {
		 case AV_FCS_CAT    : sprintf(TmpStr,"FCS/MC"); break;
		 case AV_SENSOR_CAT : sprintf(TmpStr,"SENSOR"); break;
		 case AV_STORES_CAT : sprintf(TmpStr,"STORES"); break;
	   case AV_ENG_CAT    : sprintf(TmpStr,"ENGINE"); break;
		 case AV_AMAD_CAT   : sprintf(TmpStr,"AMAD"); break;
		 case AV_DISP_CAT   : sprintf(TmpStr,"DISPLAYS"); break;
     case AV_MISC_CAT   : sprintf(TmpStr,"MISC"); break;
		 case AV_EW_CAT     : sprintf(TmpStr,"EW"); break;
	 }

   DrawTextAtLoc(56,26,TmpStr,AV_GREEN,1.0,TRUE);

 	 BitType *Ptr = &BitList[0];
	 int YPos=37;
	 int i=0;
	 while(i < AV_MAX_DAMAGE)
	 {
		 if(Ptr->Category == Av.Bit.CurBitPage)
		 {
	   		DrawTextAtLoc(37,YPos,Ptr->Label,AV_GREEN,1.0);

     		AvRGB Triple;
     		GetRGBFromAVPalette(GetBitColor(Ptr->MessageIndex),1.0,&Triple);
		 		RightJustifyTextAtLocation(103,YPos,BitMessages[Ptr->MessageIndex],Triple.Red,Triple.Green,Triple.Blue);

				YPos += 7;
		 }

		 i++;
		 Ptr++;
	 }

	 int ButtonPress = GetButtonPress(MpdNum);
	 if(ButtonPress != -1)
	 {
			switch(ButtonPress)
			{
				case 17 : Av.Bit.CurBitPage = -1; break;

				case 7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				      	break;
			}
	 }

}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void BitMode(int MpdNum)
{
	if(Av.Bit.CurBitPage != -1)
	{
		 BitPage(MpdNum);
		 return;
	}

  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);
	DrawWordAtMpdButton(MpdNum,9,"PAGE",FALSE,AV_GREEN,1.0);

	GrDrawLine(GrBuffFor3D,1,24,30,24,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,1,47,30,47,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,1,70,30,70,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,1,93,30,93,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,1,116,30,116,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,110,24,139,24,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,110,47,139,47,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,110,70,139,70,Triple.Red,Triple.Green,Triple.Blue,0);

	int Stat;

	DrawTextAtLoc(2,18,"FCS/MC",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_FCS_CAT);
	DrawTextAtLoc(2,26,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(2,41,"SENSORS",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_SENSOR_CAT);
	DrawTextAtLoc(2,49,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(2,64,"STORES",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_STORES_CAT);
	DrawTextAtLoc(2,72,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(2,87,"ENGINE",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_ENG_CAT);
	DrawTextAtLoc(2,95,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(2,110,"AMAD",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_AMAD_CAT );
	DrawTextAtLoc(2,118,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(111,18,"DISPLAYS",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_DISP_CAT );
	DrawTextAtLoc(111,26,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(111,41,"MISC",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_MISC_CAT );
	DrawTextAtLoc(111,49,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(111,64,"EW",AV_GREEN,1.0);
	Stat = GetBitCategoryStatus(AV_EW_CAT);
	DrawTextAtLoc(111,73,BitMessages[Stat],GetBitColor(Stat),1.0);

	DrawTextAtLoc(43,8,"BIT FAILURES",AV_GREEN,1.0,TRUE);

 	BitType **Walk;
	int NumStep = NumDamagedBit;

	if( (Av.Bit.CurDisplayPage == 14) && (NumDamagedBit > 14) )
	{
 	    Walk = &BitDamageList[14];
		NumStep = NumDamagedBit - 14;
	}
	else
	{
 	  Walk = &BitDamageList[0];
      if(NumDamagedBit > 14)
		NumStep = 14;
	}

	int YPos=21;
	int i=0;
	while(i < NumStep)
	{
		BitType *Ptr = *Walk;

	  DrawTextAtLoc(37,YPos,Ptr->Label,AV_GREEN,1.0);

    AvRGB Triple;
    GetRGBFromAVPalette(GetBitColor(Ptr->MessageIndex),1.0,&Triple);
		RightJustifyTextAtLocation(103,YPos,BitMessages[Ptr->MessageIndex],Triple.Red,Triple.Green,Triple.Blue);

		YPos += 7;
		i++;
		Walk++;
	}

	if( (NumDamagedBit > 14) && (Av.Bit.CurDisplayPage == 0) )
	   DrawTextAtLoc(35,123,"MORE...",AV_GREEN,1.0);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 0  : Av.Bit.CurBitPage = AV_FCS_CAT;     break;
			case 1  : Av.Bit.CurBitPage = AV_SENSOR_CAT;  break;
			case 2  : Av.Bit.CurBitPage = AV_STORES_CAT;  break;
			case 3  : Av.Bit.CurBitPage = AV_ENG_CAT;     break;
			case 4  : Av.Bit.CurBitPage = AV_AMAD_CAT;    break;
			case 12 : Av.Bit.CurBitPage = AV_EW_CAT;      break;
			case 13 : Av.Bit.CurBitPage = AV_MISC_CAT;    break;
			case 14 : Av.Bit.CurBitPage = AV_DISP_CAT;    break;

			case 7: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				      break;
			case 9: if( (NumDamagedBit > 14) && (Av.Bit.CurDisplayPage == 0) )
				            Av.Bit.CurDisplayPage = 14;
							else if(Av.Bit.CurDisplayPage == 14)
				            Av.Bit.CurDisplayPage = 0;
						  break;
		}
	}
}

//*****************************************************************************************************************************************
// ACL MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void InitAcl()
{
	Av.Acl.CmdAirspeed = -1;
	Av.Acl.CmdAlt = -1;
	Av.Acl.CmdRod = -1;
	Av.Acl.HorzAdjust = 0;
	Av.Acl.VertAdjust = 0;
	Av.Acl.At10Sec = FALSE;
	Av.Acl.Range = 20.0;
  Av.Acl.PixelsPerMile = 55.0/20.0;
	Av.Acl.Message1[0] = 0x00;
	Av.Acl.TiltMessage[0] = 0x00;
	Av.Acl.TenSecMessage[0] = 0x00;
	Av.Acl.StatusMessage[0] = 0x00;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetACLDataForAvionics(float CmdAirspeed, float CmdAlt, float CmdRod, float HorzAdjust, float VertAdjust)
{
	Av.Acl.CmdAirspeed = CmdAirspeed;
	Av.Acl.CmdAlt = CmdAlt;
	Av.Acl.CmdRod = CmdRod;
	Av.Acl.HorzAdjust = HorzAdjust;
	Av.Acl.VertAdjust = VertAdjust;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimAcl()
{
		Av.Acl.Message1[0]      = 0x00;
		Av.Acl.TiltMessage[0]   = 0x00;
		Av.Acl.TenSecMessage[0] = 0x00;
		Av.Acl.StatusMessage[0] = 0x00;

		int AutoLandSet = ((((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14) || ((PlayerPlane->AI.lVar2 > 7) && (PlayerPlane->AI.lVar2 <= 10))))));

		FPointDouble CarrierLoc;
    int CarrierPresent = GetAssociatedCarrier(&CarrierLoc);
		if(!CarrierPresent) return;

		// get time to landpoint
		FPointDouble Vec;
		Vec = CarrierLoc;
		Vec -= PlayerPlane->WorldPosition;
		float Dist = Vec.Length();
		Dist *= WUTOFT;
		float Time = Dist/PlayerPlane->V;
		Av.Acl.At10Sec = (Time <= 10.0);

		if(Av.Acl.At10Sec)
			sprintf(Av.Acl.TenSecMessage,"10 SEC");

		float DistToCarrier = Dist2D(&PlayerPlane->WorldPosition,&CarrierLoc);
		DistToCarrier *= WUTONM;

		if(DistToCarrier > 10.0)
			sprintf(Av.Acl.TiltMessage,"TILT");

		MovingVehicleParams *carrier = NULL;
		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
		}

		if((carrier) && (carrier->lAIFlags1 & V_FLASH_WAVEOFF) && (PlayerPlane->AI.lVar2 == 11))
			sprintf(Av.Acl.Message1,"W/O");
		else if( !(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) || !(PlayerPlane->TailHookState) )
			sprintf(Av.Acl.Message1,"LND CHK");
		else if(!AutoLandSet)  //  (DistToCarrier > 10.0)
			sprintf(Av.Acl.Message1,"NOT CMD");
		else if(AutoLandSet && !UFC.APStatus)    // && autopilot off
			sprintf(Av.Acl.Message1,"ACL RDY");
		else if(AutoLandSet && UFC.APStatus) // && auto pilot on
			sprintf(Av.Acl.Message1,"CMD CONT");

		if( (Av.Hsi.SteeringMode == HSI_ACL_MODE) && AutoLandSet && UFC.APStatus)
			sprintf(Av.Acl.StatusMessage,"ACL 1");
		else if( (Av.Hsi.SteeringMode == HSI_ACL_MODE) && AutoLandSet)
			sprintf(Av.Acl.StatusMessage,"ACL 2");
		else
			sprintf(Av.Acl.StatusMessage,"ACL N/A");
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AclRangeUp()
{
  Av.Acl.Range  *= 2.0;
	if(Av.Acl.Range > 80.0)
		Av.Acl.Range = 5.0;

	Av.Acl.PixelsPerMile  = 55.0/Av.Acl.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AclRangeDown()
{
  Av.Acl.Range  /= 2.0;
	if(Av.Acl.Range < 5.0)
		Av.Acl.Range = 80.0;

	Av.Acl.PixelsPerMile  = 55.0/Av.Acl.Range;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawOuterRingArrow(float X,float Y,float Angle)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	float NewX,NewZ;
	float A1 = NormDegree(Angle + 30.0);
	float A2 = NormDegree(Angle - 30.0);

  ProjectPoint(X,Y,A1,5,&NewX,&NewZ);
	GrDrawLine(GrBuffFor3D,X,Y,NewX,NewZ,Triple.Red,Triple.Green,Triple.Blue,0);

  ProjectPoint(X,Y,A2,5,&NewX,&NewZ);
	GrDrawLine(GrBuffFor3D,X,Y,NewX,NewZ,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AclMode(int MpdNum)
{
    AvRGB Triple;
    GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	  DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

		DrawTextAtLoc(16,3,"CMD A/S",AV_GREEN,1.0);
		DrawTextAtLoc(9,10,"CMD ALT",AV_GREEN,1.0);
		DrawTextAtLoc(2,17,"CMD ROD",AV_GREEN,1.0);

//		int AutoLandSet = ((((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 > 7) && (PlayerPlane->AI.lVar2 <= 10)))));
		int AutoLandSet = ((((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 > 0) && (PlayerPlane->AI.lVar2 <= 10)))));

		if( (Av.Hsi.SteeringMode == HSI_ACL_MODE) && AutoLandSet)
		{
				sprintf(TmpStr,"%d", (int) Av.Acl.CmdAirspeed /*CmdAS*/);
				DrawTextAtLoc(44,3,TmpStr,AV_GREEN,1.0);

				sprintf(TmpStr,"%d", (int) Av.Acl.CmdAlt /*CmdAlt*/);
				DrawTextAtLoc(36,10,TmpStr,AV_GREEN,1.0);

    		sprintf(TmpStr,"%d", (int) Av.Acl.CmdRod /*CmdRod*/);
	  		DrawTextAtLoc(8,24,TmpStr,AV_GREEN,1.0);
		}

		sprintf(TmpStr,"%s",&Av.Acl.Message1[0] /*Group1Message*/);
		DrawTextAtLoc(88,3,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%s",&Av.Acl.TiltMessage[0] /*TiltMessage*/);
		DrawTextAtLoc(88,10,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%s",&Av.Acl.TenSecMessage[0] /*10Sec*/);
		DrawTextAtLoc(115,18,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%s",&Av.Acl.StatusMessage[0] /*AclSystemStatus*/);
		DrawTextAtLoc(115,25,TmpStr,AV_GREEN,1.0);

		DrawTextAtLoc(57,3,"SCL/",AV_GREEN,1.0);

		sprintf(TmpStr,"%d", (int)Av.Acl.Range /*Range*/);
		DrawTextAtLoc(73,3,TmpStr,AV_GREEN,1.0);


		GrDrawLine(GrBuffFor3D,1,16,16,1,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,16,1,55,1,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,55,1,55,16,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,55,16,31,16,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,31,16,31,30,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,31,30,1,30,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,1,30,1,16,Triple.Red,Triple.Green,Triple.Blue,0);

		GrDrawLine(GrBuffFor3D,85,1,124,1,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,124,1,139,16,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,139,16,139,31,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,139,31,113,31,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,113,31,113,16,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,113,16,85,16,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,85,16,85,1,Triple.Red,Triple.Green,Triple.Blue,0);

		// draw plane in center
		GrDrawLine(GrBuffFor3D,70,78,70,72,Triple.Red,Triple.Green,Triple.Blue,0);
    GrDrawLine(GrBuffFor3D,67,74,73,74,Triple.Red,Triple.Green,Triple.Blue,0);
    GrDrawLine(GrBuffFor3D,69,77,71,77,Triple.Red,Triple.Green,Triple.Blue,0);

	  GrDrawPolyCircle(GrBuffFor3D,70,74,27,34,Triple.Red,Triple.Green,Triple.Blue,0);

		float Angle;
		int   TotalLength = 55;
		int   End1X,End1Y;
		float TotalAngle;
		float EAngle,NAngle,SAngle,WAngle;
		float Heading,PreciseHeading;
		int   TempCenterX, TempCenterY;

		Heading = PlayerPlane->Heading/DEGREE;

		PreciseHeading = PlayerPlane->Heading*100;
		PreciseHeading = PreciseHeading/DEGREE;
		PreciseHeading = PreciseHeading/100.0;

		NAngle = NormDegree(360.0 - Heading + 90.0);
		SAngle = NormDegree(NAngle + 180.0);
		EAngle = NormDegree(NAngle + 270.0);
		WAngle = NormDegree(NAngle + 90.0);

		TempCenterX = 70;
		TempCenterY = 74;

  	Angle = NAngle;
  	TotalAngle = 0.0;

		int Tick = 36;

  	while(TotalAngle < 360.0)
  	{
	  	End1X = TempCenterX + ICosTimes((Angle*DEGREE), TotalLength);
    	End1Y = TempCenterY - ISinTimes((Angle*DEGREE), TotalLength);

	  	if(Angle == NAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"N",AV_GREEN,1.0);
    	}
	  	else if(Angle == SAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"S",AV_GREEN,1.0);
 	  	}
    	else if(Angle == EAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"E",AV_GREEN,1.0);
			}
	  	else if(Angle == WAngle)
	  	{
	    	DrawTextAtLoc(End1X-2,End1Y-2,"W",AV_GREEN,1.0);
 	  	}
			else if(fmod(TotalAngle,30) == 0)
			{
				sprintf(TmpStr,"%d",Tick);
				DrawTextAtLoc(End1X-2,End1Y-2,TmpStr,AV_GREEN,1.0);
			}
	  	else
			{
	    	DrawAvDot(End1X,End1Y,AV_GREEN,1.0);
			}

		  Tick--;
	  	TotalAngle += 10.0;
	  	Angle = NormDegree(Angle + 10.0);
		}

		FPointDouble CarrierLoc;
    int CarrierPresent = GetAssociatedCarrier(&CarrierLoc);

		if(CarrierPresent)
		{
				float NewX,NewZ;

				float DistToCarrier = Dist2D(&PlayerPlane->WorldPosition,&CarrierLoc);
				DistToCarrier *= WUTONM;

				float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,CarrierLoc,1);

				ProjectPoint(TempCenterX,TempCenterY,NormDegree(DeltaAzim),57,&NewX,&NewZ);
				DrawOuterRingArrow(NewX,NewZ,NormDegree(DeltaAzim));

				ProjectPoint(TempCenterX,TempCenterY,NormDegree(DeltaAzim),58,&NewX,&NewZ);
				DrawOuterRingArrow(NewX,NewZ,NormDegree(DeltaAzim));

				int Radius = 55;
				if(Radius > DistToCarrier*Av.Acl.PixelsPerMile)
				{
						ProjectPoint(TempCenterX,TempCenterY,NormDegree(DeltaAzim),DistToCarrier*Av.Acl.PixelsPerMile,&NewX,&NewZ);
    				DrawHomeBase(NewX,NewZ,AV_GREEN,1.0);
				}
		}

		int ButtonPress = GetButtonPress(MpdNum);
		if(ButtonPress != -1)
		{
			switch(ButtonPress)
			{
				case 7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				       	 break;

        case 17: AclRangeUp();
				         break;
			}
		}

}

//*****************************************************************************************************************************************
// FPAS MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void FpasMode(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	float BingoRange,BingoEnd,TwokRange,TwokEnd,ZeroRange,ZeroEnd;
	char Time[10];
	char BEnd[10];
	char TwoEnd[10];
	char ZEnd[10];
	int Waypoint;
	float FuelRemaining;
	float Lb_Nm;
	float TempFuel;
	float MaxTrap;


	float BasicWeight,FuelWeight,StoresWeight,TotalWeight;
	float TotalFuel;

	TotalFuel = PlayerPlane->InternalFuel + PlayerPlane->CenterDropFuel + PlayerPlane->WingDropFuel;

	// bingo
	TempFuel = TotalFuel - Av.Fuel.BingoVal;

	if(TempFuel <= 0)
	{
		 BingoRange = 0;
		 BingoEnd = 0;
	}
	else
	{
	    CalcFuelBurn(TempFuel,&BingoEnd,&BingoRange);
	}

	// 2000lb
	TempFuel = TotalFuel - 2000;

	if(TempFuel <= 0)
	{
		 TwokRange = 0;
		 TwokEnd = 0;
	}
	else
	{
	    CalcFuelBurn(TempFuel,&TwokEnd,&TwokRange);
	}

	// 0lb
	TempFuel = TotalFuel;

	if(TempFuel <= 0)
	{
		 ZeroRange = 0;
		 ZeroEnd = 0;
	}
	else
	{
		CalcFuelBurn(TempFuel,&ZeroEnd,&ZeroRange);
	}

	// adjust endurace to strings
	sprintf(BEnd,  "%02d:%02d",(int)(BingoEnd/60.0),((int)BingoEnd) % 60);
	sprintf(TwoEnd,"%02d:%02d",(int)(TwokEnd/60.0),((int)TwokEnd) % 60);
	sprintf(ZEnd,  "%02d:%02d",(int)(ZeroEnd/60.0),((int)ZeroEnd) % 60);

	// waypoint
	Waypoint = UFC.DataCurrSP;


	if(PlayerPlane->AI.fStoresWeight < 0)
	{
		PlayerPlane->AI.fStoresWeight = 0;
		PlayerPlane->AircraftDryWeight = 30564;
	}

	// weights
	BasicWeight  = PlayerPlane->AircraftDryWeight - PlayerPlane->AI.fStoresWeight;
	FuelWeight   = TotalFuel;
//	StoresWeight = BasicWeight - 30564;
	StoresWeight = PlayerPlane->AI.fStoresWeight;
	TotalWeight  = BasicWeight + FuelWeight + StoresWeight;

	MaxTrap = 46000;

	float FuelFlow = UFC.LeftFuelFlowRate + UFC.RightFuelFlowRate;

#if 0
	float Speed = PlayerPlane->IndicatedAirSpeed*FTTONM;

	float TimeToWay = (UFC.DataCurrSPNavDist/Speed)/60.0;
	float Frac = TimeToWay - ((int)TimeToWay);

  // time
	float Minutes = ( fmod(WorldParams.WorldTime,3600) )/60.0;
	if(Minutes >= 60.0) Minutes = 59;
	float Seconds = Frac*60.0;
	if(Seconds == 60.0)
			Seconds = 0;

	sprintf(Time,"%02d:%02d",(int)Minutes,(int)Seconds);

	Lb_Nm = (TimeToWay*FuelFlow)/UFC.DataCurrSPNavDist;

	FuelRemaining = FuelWeight - (TimeToWay*FuelFlow); // how much fuel when you get there
#else
	float Speed = PlayerPlane->IfHorzVelocity;

	if(Speed == 0)
		Speed = 1.0f;

	Speed = Speed*FTTONM*60.0f*60.0f;  //  FeetPerSec Ground Speed to NMPerHour

	float TimeToWayInMinutes = (UFC.DataCurrSPNavDist/Speed)*60.0f;
	float TimeToWayInSeconds = TimeToWayInMinutes * 60.0f;

  // time

	float fworktime = WorldParams.WorldTime + TimeToWayInSeconds;

	while(fworktime >= (24.0f HOURS))
		fworktime -= (24.0f HOURS);

	float arrive_hours = (int)fworktime / 3600;
	float arrive_minutes = ((int)fworktime%3600)/60;

	sprintf(Time,"%02d:%02d",(int)arrive_hours,(int)arrive_minutes);

	Lb_Nm = (TimeToWayInMinutes*FuelFlow)/UFC.DataCurrSPNavDist;

	FuelRemaining = FuelWeight - (TimeToWayInMinutes*FuelFlow); // how much fuel when you get there
#endif

	if(FuelRemaining < 0) FuelRemaining = 0;

	GrDrawLine(GrBuffFor3D,53,11,87,11,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,59,22,79,22,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,92,22,128,22,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,11,62,33,62,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,39,62,55,62,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,61,62,103,62,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,110,62,130,62,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,5,74,135,74,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,5,75,135,75,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,42,86,96,86,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,11,115,74,115,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,92,108,129,108,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,129,108,129,123,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,129,123,92,123,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,92,123,92,108,Triple.Red,Triple.Green,Triple.Blue,0);

	DrawTextAtLoc(12,27,"TO BINGO",AV_GREEN,1.0);
	DrawTextAtLoc(12,35,"TO 2000 LB",AV_GREEN,1.0);
	DrawTextAtLoc(12,43,"TO 0 LB",AV_GREEN,1.0);
	DrawTextAtLoc(12,56,"NAV TO",AV_GREEN,1.0);
	DrawTextAtLoc(41,56,"TIME",AV_GREEN,1.0);
	DrawTextAtLoc(62,56,"FUEL REMAIN",AV_GREEN,1.0);
	DrawTextAtLoc(111,56,"LB/NM",AV_GREEN,1.0);
	DrawTextAtLoc(60,16,"RANGE",AV_GREEN,1.0);
	DrawTextAtLoc(93,16,"ENDURANCE",AV_GREEN,1.0);
	DrawTextAtLoc(54,5,"FUEL FLOW",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",Waypoint /* WPT_VAR */);
	DrawTextAtLoc(27,65,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%s",Time /* TIME_VAR*/);
	DrawTextAtLoc(39,65,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)FuelRemaining /* FUEL_REMAIN_VAR*/);
	DrawTextAtLoc(77,65,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int) Lb_Nm /* LB_NM_VAR*/);
	DrawTextAtLoc(123,65,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(43,80,"AIRCRAFT WEIGHT",AV_GREEN,1.0);
	DrawTextAtLoc(12,92,"BASIC",AV_GREEN,1.0);
	DrawTextAtLoc(12,100,"FUEL",AV_GREEN,1.0);
	DrawTextAtLoc(12,108,"STORES",AV_GREEN,1.0);
	DrawTextAtLoc(12,119,"TOTAL",AV_GREEN,1.0);

	DrawTextAtLoc(94,110,"MAX TRAP",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)MaxTrap /* MAX_TRAP_VAR */);
	DrawTextAtLoc(94,117,TmpStr,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)BasicWeight /* basic_var */);
	RightJustifyTextAtLocation(73,91,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%d",(int)FuelWeight /* FUEL_VAR */);
	RightJustifyTextAtLocation(73,99,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%d",(int)StoresWeight /* STORES_VAR */);
	RightJustifyTextAtLocation(73,108,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%d",(int)TotalWeight /* TOTAL_VAR */);
	RightJustifyTextAtLocation(73,119,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%d",(int)BingoRange /* BINGO_RANGE_VAR */);
	RightJustifyTextAtLocation(75,27,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%d",(int)TwokRange /* 2000LB_VAR */);
	RightJustifyTextAtLocation(75,35,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%d",(int)ZeroRange /* 0LB_VAR */);
	RightJustifyTextAtLocation(75,43,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%s",BEnd /* BINGO_END_VAR */);
	RightJustifyTextAtLocation(126,27,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%s",TwoEnd /* 2000LB_END_VAR */);
	RightJustifyTextAtLocation(126,35,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	sprintf(TmpStr,"%s",ZEnd /* 0LB_END */);
	RightJustifyTextAtLocation(126,43,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				      break;
		}
	}
}

//*****************************************************************************************************************************************
// CHECKLIST MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
void CheckListMode(int MpdNum)
{
	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawTextAtLoc(23,11,"LAND",AV_GREEN,1.0);
	DrawTextAtLoc(28,24,"WHEELS",AV_GREEN,1.0);
	DrawTextAtLoc(28,32,"FLAPS",AV_GREEN,1.0);
	DrawTextAtLoc(28,40,"HOOK",AV_GREEN,1.0);
	DrawTextAtLoc(28,48,"ANTI SKID",AV_GREEN,1.0);
	DrawTextAtLoc(28,56,"HARNESS",AV_GREEN,1.0);
	DrawTextAtLoc(28,64,"DISPENSER",AV_GREEN,1.0);
	DrawTextAtLoc(87,24,"CONTROL",AV_GREEN,1.0);
	DrawTextAtLoc(87,32,"WING",AV_GREEN,1.0);
	DrawTextAtLoc(87,40,"TRIM",AV_GREEN,1.0);
	DrawTextAtLoc(87,48,"FLAPS",AV_GREEN,1.0);
	DrawTextAtLoc(87,56,"HOOK",AV_GREEN,1.0);
	DrawTextAtLoc(87,64,"HARNESS",AV_GREEN,1.0);
	DrawTextAtLoc(87,72,"WARN LITES",AV_GREEN,1.0);
	DrawTextAtLoc(87,80,"NWS LO",AV_GREEN,1.0);
	DrawTextAtLoc(87,88,"SEAT ARM",AV_GREEN,1.0);
	DrawTextAtLoc(87,106,"FUEL JP-8",AV_GREEN,1.0);
	DrawTextAtLoc(82,11,"T.O.",AV_GREEN,1.0);

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 7:
				Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			break;
		}
	}
}

//*****************************************************************************************************************************************
// FUEL MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

float FuelMaxs[]     = {2560, 2580, 2340, 3720, 1600, 1600, -1};
float FuelMaxsBurn[] = {1600, 1600, 3720, 2340, 2580, 2560, -1};    // reverse order from filling

void FillFuelTanks(int LI, float LO, float RI, float RO)
{
	float Internal = PlayerPlane->InternalFuel;

	Av.Fuel.BingoVal = 10000.0;  // this needs to be changed in sflight as well

	Av.Fuel.TotalCenter   = PlayerPlane->CenterDropFuel;
	Av.Fuel.TotalWing     = PlayerPlane->WingDropFuel;
	Av.Fuel.TotalInternal = WEIGHT_MAX_INTERNALFUEL;  //  PlayerPlane->InternalFuel;

	Av.Fuel.LastCenter   = Av.Fuel.TotalCenter;
	Av.Fuel.LastWing     = Av.Fuel.TotalWing;
	Av.Fuel.LastInternal = Av.Fuel.TotalInternal;

	Av.Fuel.DumpFuel     = FALSE;

	ZeroMemory(Av.Fuel.Tanks,sizeof(float)*11);

	Av.Fuel.Tanks[AV_FUEL_EX_LO] = LO;
	Av.Fuel.Tanks[AV_FUEL_EX_LI] = LI;
	Av.Fuel.Tanks[AV_FUEL_EX_RO] = RO;
	Av.Fuel.Tanks[AV_FUEL_EX_RI] = RI;
	Av.Fuel.Tanks[AV_FUEL_EX_C]  = PlayerPlane->CenterDropFuel;

#if 1
	Av.Fuel.TanksMax[AV_FUEL_EX_LI] = (Av.Fuel.Tanks[AV_FUEL_EX_LI] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_LI] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_LO] = (Av.Fuel.Tanks[AV_FUEL_EX_LO] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_LO] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_RI] = (Av.Fuel.Tanks[AV_FUEL_EX_RI] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_RI] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_RO] = (Av.Fuel.Tanks[AV_FUEL_EX_RO] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_RO] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_C]  = (Av.Fuel.Tanks[AV_FUEL_EX_C]  > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_C]  : WEIGHT_480_GALLONS_FUEL;
#else
	Av.Fuel.TanksMax[AV_FUEL_EX_LI] = (Av.Fuel.TanksMax[AV_FUEL_EX_LI] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_LI] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_LO] = (Av.Fuel.TanksMax[AV_FUEL_EX_LO] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_LO] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_RI] = (Av.Fuel.TanksMax[AV_FUEL_EX_RI] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_RI] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_RO] = (Av.Fuel.TanksMax[AV_FUEL_EX_RO] > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_RO] : WEIGHT_480_GALLONS_FUEL;
	Av.Fuel.TanksMax[AV_FUEL_EX_C]  = (Av.Fuel.TanksMax[AV_FUEL_EX_C]  > 0) ?  Av.Fuel.Tanks[AV_FUEL_EX_C]  : WEIGHT_480_GALLONS_FUEL;
#endif

	// fill internal tanks
	int i = 10;
	float *Ptr = &FuelMaxs[0];
	int Done = FALSE;
	while( (*Ptr != -1) )
	{
	  Av.Fuel.TanksMax[i] = *Ptr;

		if(!Done)
		{
				if(Internal <= *Ptr)
				{
		  		Av.Fuel.Tanks[i] = Internal;
					Done = TRUE;
				}
				else
				{
					Av.Fuel.Tanks[i] = *Ptr;
					Internal -= *Ptr;
				}
		}
		i--;
		Ptr++;
	}

	// make sure 2 internal wings are equal
	if(Av.Fuel.Tanks[AV_FUEL_IN_LEFT] != Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] )
	{
		float Fuel =  Av.Fuel.Tanks[AV_FUEL_IN_LEFT] + Av.Fuel.Tanks[AV_FUEL_IN_RIGHT];
		Fuel /= 2.0;

		Av.Fuel.Tanks[AV_FUEL_IN_LEFT]  = Fuel;
		Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] = Fuel;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleDumpFuel()
{
	if( !(PlayerPlane->OnGround) )
	  Av.Fuel.DumpFuel = !Av.Fuel.DumpFuel;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DropFuelTankForDisplay(int Station)
{
	if(Station == LEFT3_STATION)
	{
	  PlayerPlane->WingDropFuel -= Av.Fuel.Tanks[AV_FUEL_EX_LO];
		Av.Fuel.Tanks[AV_FUEL_EX_LO] = 0;
	  Av.Fuel.LastWing = PlayerPlane->WingDropFuel;
	}
	else if(Station == LEFT4_STATION)
	{
	  PlayerPlane->WingDropFuel -= Av.Fuel.Tanks[AV_FUEL_EX_LI];
		Av.Fuel.Tanks[AV_FUEL_EX_LI] = 0;
	  Av.Fuel.LastWing = PlayerPlane->WingDropFuel;
	}
	else if(Station == RIGHT8_STATION)
	{
	  PlayerPlane->WingDropFuel -= Av.Fuel.Tanks[AV_FUEL_EX_RI];
		Av.Fuel.Tanks[AV_FUEL_EX_RI] = 0;
  	Av.Fuel.LastWing = PlayerPlane->WingDropFuel;
	}
	else if(Station == RIGHT9_STATION)
	{
	  PlayerPlane->WingDropFuel -= Av.Fuel.Tanks[AV_FUEL_EX_RO];
		Av.Fuel.Tanks[AV_FUEL_EX_RO] = 0;
	  Av.Fuel.LastWing = PlayerPlane->WingDropFuel;
	}
	else if(Station == CENTER6_STATION)
	{
	  PlayerPlane->CenterDropFuel -= Av.Fuel.Tanks[AV_FUEL_EX_C];
		Av.Fuel.Tanks[AV_FUEL_EX_C] = 0;
	  Av.Fuel.LastCenter = PlayerPlane->CenterDropFuel;
	}

  if(PlayerPlane->WingDropFuel < 0)
	{
		PlayerPlane->WingDropFuel = 0;
		Av.Fuel.LastWing = 0;
	}

  if(PlayerPlane->CenterDropFuel < 0)
	{
		PlayerPlane->CenterDropFuel = 0;
		Av.Fuel.LastCenter = 0;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimFuelTanks()
{
	float Consumed;

	if(Av.Fuel.LastWing > 0)
	{
		Consumed = Av.Fuel.LastWing - PlayerPlane->WingDropFuel;
		Av.Fuel.LastWing = PlayerPlane->WingDropFuel;

		Consumed /= 2.0;

		if( (Av.Fuel.Tanks[AV_FUEL_EX_LO] > 0) || (Av.Fuel.Tanks[AV_FUEL_EX_RO] > 0) )
		{
			 Av.Fuel.Tanks[AV_FUEL_EX_LO] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_EX_LO] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_EX_RO] += Av.Fuel.Tanks[AV_FUEL_EX_LO];
				Av.Fuel.Tanks[AV_FUEL_EX_LO] = 0;
				if(Av.Fuel.Tanks[AV_FUEL_EX_RO] < 0)
					Av.Fuel.Tanks[AV_FUEL_EX_RO] = 0;
			 }

			 Av.Fuel.Tanks[AV_FUEL_EX_RO] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_EX_RO] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_EX_LO] += Av.Fuel.Tanks[AV_FUEL_EX_RO];
				Av.Fuel.Tanks[AV_FUEL_EX_RO] = 0;
				if(Av.Fuel.Tanks[AV_FUEL_EX_LO] < 0)
					Av.Fuel.Tanks[AV_FUEL_EX_LO] = 0;
			 }
		}
		else
		{
			 Av.Fuel.Tanks[AV_FUEL_EX_LI] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_EX_LI] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_EX_RI] += Av.Fuel.Tanks[AV_FUEL_EX_LI];
				Av.Fuel.Tanks[AV_FUEL_EX_LI] = 0;
				if(Av.Fuel.Tanks[AV_FUEL_EX_RI] < 0)
					Av.Fuel.Tanks[AV_FUEL_EX_RI] = 0;
			 }

			 Av.Fuel.Tanks[AV_FUEL_EX_RI] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_EX_RI] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_EX_LI] += Av.Fuel.Tanks[AV_FUEL_EX_RI];
				Av.Fuel.Tanks[AV_FUEL_EX_RI] = 0;
				if(Av.Fuel.Tanks[AV_FUEL_EX_LI] < 0)
					Av.Fuel.Tanks[AV_FUEL_EX_LI] = 0;
			 }
		}
	}
	else  if(Av.Fuel.LastCenter > 0)
	{
		Consumed = Av.Fuel.LastCenter - PlayerPlane->CenterDropFuel;
		Av.Fuel.LastCenter = PlayerPlane->CenterDropFuel;

		Av.Fuel.Tanks[AV_FUEL_EX_C] -= Consumed;

		if(Av.Fuel.Tanks[AV_FUEL_EX_C] < 0)
			 Av.Fuel.Tanks[AV_FUEL_EX_C] = 0;
	}
	else
	{
		Consumed = Av.Fuel.LastInternal - PlayerPlane->InternalFuel;
		Av.Fuel.LastInternal = PlayerPlane->InternalFuel;

		if( (Av.Fuel.Tanks[AV_FUEL_IN_LEFT] > 0) || (Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] > 0) )
		{
			 Consumed /= 2.0;

			 Av.Fuel.Tanks[AV_FUEL_IN_LEFT] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_IN_LEFT] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_IN_LEFT] = 0;
				Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] += Av.Fuel.Tanks[AV_FUEL_IN_LEFT];
				if(Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] < 0)
					Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] = 0;
			 }

			 Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] = 0;
				Av.Fuel.Tanks[AV_FUEL_IN_LEFT] += Av.Fuel.Tanks[AV_FUEL_IN_RIGHT];
				if(Av.Fuel.Tanks[AV_FUEL_IN_LEFT] < 0)
					Av.Fuel.Tanks[AV_FUEL_IN_LEFT] = 0;
			 }
		}
		else if(Av.Fuel.Tanks[AV_FUEL_IN_TANK4] > 0)
		{
			Av.Fuel.Tanks[AV_FUEL_IN_TANK4] -= Consumed;
			if(Av.Fuel.Tanks[AV_FUEL_IN_TANK4] < 0)
				Av.Fuel.Tanks[AV_FUEL_IN_TANK4] = 0;
		}
		else if(Av.Fuel.Tanks[AV_FUEL_IN_TANK1] > 0)
		{
			Av.Fuel.Tanks[AV_FUEL_IN_TANK1] -= Consumed;
			if(Av.Fuel.Tanks[AV_FUEL_IN_TANK1] < 0)
				Av.Fuel.Tanks[AV_FUEL_IN_TANK1] = 0;
		}
		else if( (Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] > 0) || (Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] > 0) )
		{
			 Consumed /= 2.0;

			 Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] = 0;
				Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] += Av.Fuel.Tanks[AV_FUEL_IN_L_FEED];
				if(Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] < 0)
					Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] = 0;
			 }

			 Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] -= Consumed;
			 if(Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] < 0)
			 {
				Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] = 0;
				Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] += Av.Fuel.Tanks[AV_FUEL_IN_R_FEED];
				if(Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] < 0)
					Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] = 0;
			 }
		}
	}

	// make sure there is no left over
	if(PlayerPlane->WingDropFuel <= 0)
	{
		Av.Fuel.Tanks[AV_FUEL_EX_RO] = 0;
		Av.Fuel.Tanks[AV_FUEL_EX_LO] = 0;
		Av.Fuel.Tanks[AV_FUEL_EX_RI] = 0;
		Av.Fuel.Tanks[AV_FUEL_EX_LI] = 0;
	}

	if(PlayerPlane->CenterDropFuel <= 0)
		Av.Fuel.Tanks[AV_FUEL_EX_C]  = 0;

	if(PlayerPlane->InternalFuel <= 0)
	{
		Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] = 0;
		Av.Fuel.Tanks[AV_FUEL_IN_LEFT]  = 0;
		Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] = 0;
		Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] = 0;
		Av.Fuel.Tanks[AV_FUEL_IN_TANK1] = 0;
		Av.Fuel.Tanks[AV_FUEL_IN_TANK4] = 0;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FuelMode(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

  DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	GrDrawLine(GrBuffFor3D,8,60,35,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,35,60,35,71,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,35,71,8,71,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,8,71,8,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,5,118,23,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,23,118,23,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,23,129,5,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,5,129,5,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,33,118,51,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,51,118,51,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,51,129,33,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,33,129,33,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,61,118,79,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,79,118,79,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,79,129,61,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,61,129,61,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,89,118,107,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,107,118,107,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,107,129,89,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,89,129,89,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,116,118,134,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,134,118,134,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,134,129,116,129,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,116,129,116,118,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,104,60,131,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,131,60,131,71,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,131,71,104,71,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,104,71,104,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,10,93,10,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,10,93,31,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,31,47,31,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,31,47,10,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,40,93,40,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,40,93,51,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,51,47,51,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,51,47,40,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,60,93,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,60,93,71,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,71,47,71,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,71,47,60,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,80,93,80,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,80,93,101,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,93,101,47,101,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,47,101,47,80,Triple.Red,Triple.Green,Triple.Blue,0);

	DrawTextAtLoc(8,17,"TOTAL",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)(PlayerPlane->CenterDropFuel + PlayerPlane->WingDropFuel + PlayerPlane->InternalFuel));
	DrawTextAtLoc(8,24,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(8,35,"INTERNAL",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)PlayerPlane->InternalFuel);
	DrawTextAtLoc(8,42,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(13,54,"L WING",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)Av.Fuel.Tanks[AV_FUEL_IN_LEFT]);
	DrawTextAtLoc(15,64,TmpStr,AV_GREEN,1.0);

	if(Av.Fuel.DumpFuel)
	{
	  DrawTextAtLoc(13,83,"DUMP",AV_GREEN,1.0,TRUE);
	  DrawTextAtLoc(13,92,"OPEN",AV_GREEN,1.0,TRUE);
	}

	DrawTextAtLoc(8,112,"L OT",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.Fuel.Tanks[AV_FUEL_EX_LO]);
	DrawTextAtLoc(8,122,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(38,112,"L IN",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)Av.Fuel.Tanks[AV_FUEL_EX_LI]);
	DrawTextAtLoc(35,122,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(67,112,"CL",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.Fuel.Tanks[AV_FUEL_EX_C]);
	DrawTextAtLoc(63,122,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(92,112,"R IN",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)Av.Fuel.Tanks[AV_FUEL_EX_RI]);
	DrawTextAtLoc(91,122,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(119,112,"R OT",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.Fuel.Tanks[AV_FUEL_EX_RO]);
	DrawTextAtLoc(119,122,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(56,105,"EXTERNAL",AV_GREEN,1.0);

	DrawTextAtLoc(61,4,"TANK 1",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.Fuel.Tanks[AV_FUEL_IN_TANK1]);
	DrawTextAtLoc(63,18,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(60,34,"L FEED",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)Av.Fuel.Tanks[AV_FUEL_IN_L_FEED]);
	DrawTextAtLoc(63,44,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(60,54,"R FEED",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)Av.Fuel.Tanks[AV_FUEL_IN_R_FEED]);
	DrawTextAtLoc(63,64,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(61,74,"TANK 4",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.Fuel.Tanks[AV_FUEL_IN_TANK4]);
	DrawTextAtLoc(63,88,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(104,36,"EXTERNAL",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)(PlayerPlane->CenterDropFuel + PlayerPlane->WingDropFuel));
	DrawTextAtLoc(117,43,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(108,54,"R WING",AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(int)Av.Fuel.Tanks[AV_FUEL_IN_RIGHT]);
	DrawTextAtLoc(111,64,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(118,17,"BINGO",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.Fuel.BingoVal);
	DrawTextAtLoc(120,24,TmpStr,AV_GREEN,1.0);

	// tick marks
 float Frac = Av.Fuel.Tanks[AV_FUEL_IN_TANK1]/Av.Fuel.TanksMax[AV_FUEL_IN_TANK1];
 float YPos =	LERP(31,10,Frac);
 DrawAvRadarSymbol(93,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_IN_L_FEED]/Av.Fuel.TanksMax[AV_FUEL_IN_L_FEED];
 YPos =	LERP(51,40,Frac);
 DrawAvRadarSymbol(93,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_IN_R_FEED]/Av.Fuel.TanksMax[AV_FUEL_IN_R_FEED];
 YPos =	LERP(71,60,Frac);
 DrawAvRadarSymbol(93,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_IN_TANK4]/Av.Fuel.TanksMax[AV_FUEL_IN_TANK4];
 YPos =	LERP(101,80,Frac);
 DrawAvRadarSymbol(93,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_IN_LEFT]/Av.Fuel.TanksMax[AV_FUEL_IN_LEFT];
 YPos =	LERP(71,60,Frac);
 DrawAvRadarSymbol(35,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_IN_RIGHT]/Av.Fuel.TanksMax[AV_FUEL_IN_RIGHT];
 YPos =	LERP(71,60,Frac);
 DrawAvRadarSymbol(131,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_EX_LO]/Av.Fuel.TanksMax[AV_FUEL_EX_LO];
 YPos =	LERP(129,118,Frac);
 DrawAvRadarSymbol(23,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_EX_LI]/Av.Fuel.TanksMax[AV_FUEL_EX_LI];
 YPos =	LERP(129,118,Frac);
 DrawAvRadarSymbol(51,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_EX_C]/Av.Fuel.TanksMax[AV_FUEL_EX_C];
 YPos =	LERP(129,118,Frac);
 DrawAvRadarSymbol(79,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_EX_RI]/Av.Fuel.TanksMax[AV_FUEL_EX_RI];
 YPos =	LERP(129,118,Frac);
 DrawAvRadarSymbol(107,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 Frac = Av.Fuel.Tanks[AV_FUEL_EX_RO]/Av.Fuel.TanksMax[AV_FUEL_EX_RO];
 YPos =	LERP(129,118,Frac);
 DrawAvRadarSymbol(134,YPos,RDR_ATN_ELV,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

 int ButtonPress = GetButtonPress(MpdNum);
 if(ButtonPress != -1)
 {
	switch(ButtonPress)
	{
		case 7:  Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				    break;
	}
 }

}

//*****************************************************************************************************************************************
// EASY CAM MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void EasyCamMode(int MpdNum)
{


}

//*****************************************************************************************************************************************
// EASY SENSOR MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void EasySensorMode(int MpdNum)
{
}

//*****************************************************************************************************************************************
// SA MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
void InitSA()
{
	Av.SA.DatalinkOn = TRUE;
	Av.SA.FlirOn = TRUE;
	Av.SA.RadarOn = TRUE;
	Av.SA.Range = 40.0;
	Av.SA.PixelsPerMile = 55.0/40.0;
}

//*****************************************************************************************************************************************
void SARangeUp()
{
	Av.SA.Range  *= 2.0;
	if(Av.SA.Range > 160.0)
		Av.SA.Range = 10.0;

	Av.SA.PixelsPerMile  = 55.0/Av.SA.Range;
}

//*****************************************************************************************************************************************
void SARangeDown()
{
	Av.SA.Range  /= 2.0;
	if(Av.SA.Range < 10.0)
		Av.SA.Range = 160.0;

	Av.SA.PixelsPerMile  = 55.0/Av.SA.Range;
}

void SADrawRadarFOV();

void DrawSAList(DetectedPlaneListType *PList,int draw_these_flags)
{
	int Radius = 55;
	int des;
	float NewX,NewZ;
	DetectedPlaneType *dp;

	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	dp = &PList->Planes[PList->NumPlanes];
	while(dp-- > PList->Planes)
	{
		des = dp->Designation & draw_these_flags;

		if(dp->ShouldDraw && ((des & AA_AWACS) || (!dp->AOT && (dp->Designation & (AA_RANKED | AA_PRIMARY | AA_SECONDARY)))))
		{
			float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,dp->Loc,1);
   			float Dist = Dist2D(&PlayerPlane->WorldPosition,&dp->Loc);
   			Dist *= WUTONM;
			PlaneParams *P = &Planes[ dp->PlaneId ];

			if(Radius > Dist*Av.SA.PixelsPerMile)
			{
				ProjectPoint(70,74,NormDegree(DeltaAzim),Av.SA.PixelsPerMile*Dist,&NewX, &NewZ);
				DrawHAFU(NewX,NewZ,dp->Designation,HAFUGETSETTER(des),dp->IDLevel,dp->Rank,dp->Heading,P,TRUE,1.0,0);

			}
		}
	}
}

//*****************************************************************************************************************************************
void SADrawPlaneInfo(int flags)
{
	if (flags & AA_AWACS)
	{
		AvObjListEntryType *Awacs;
		float NewX,NewZ;

		int Radius = 55;

		if (Primary->Id != -1)
		{
			if(Planes[Primary->Id].AI.iAICombatFlags2 & AI_AWACS_DETECT)
			{
				switch (Planes[Primary->Id].AI.iSide)
				{
					case AI_ENEMY:
						Primary->IDLevel = AA_HOSTILE;
						break;
					case AI_FRIENDLY:
						Primary->IDLevel = AA_FRIENDLY;
						break;
					case AI_NEUTRAL:
						Primary->IDLevel = AA_UNKNOWN;
						break;
				}
			}
		}

		Awacs = &Av.Planes.List[0];
		int Index = Av.Planes.NumPlanes;
		while(Index-- > 0)
		{
			int PlaneId = ( (PlaneParams *)Awacs->Obj - &Planes[0] );
			PlaneParams *AwacsPlane = (PlaneParams *)Awacs->Obj;

			int IDLevel;

			switch (AwacsPlane->AI.iSide)
			{
				case AI_ENEMY:
					IDLevel = AA_HOSTILE;
					break;
				case AI_FRIENDLY:
					IDLevel = AA_FRIENDLY;
					break;
				case AI_NEUTRAL:
					IDLevel = AA_UNKNOWN;
					break;
			}

			if(AwacsPlane->AI.iAICombatFlags2 & AI_AWACS_DETECT)
			{
				float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,AwacsPlane->WorldPosition,1);
				float Dist = Dist2D(&PlayerPlane->WorldPosition,&AwacsPlane->WorldPosition);
				Dist *= WUTONM;
				if(Radius > Dist*Av.SA.PixelsPerMile)
				{
					ProjectPoint(70,74,NormDegree(DeltaAzim),Av.SA.PixelsPerMile*Dist,&NewX, &NewZ);
					DrawHAFU(NewX,NewZ,AA_AWACS,FALSE,TRUE,IDLevel,1,(float)AwacsPlane->Heading/(float)DEGREE,AwacsPlane);
				}
			}
			Awacs++;
		}
		flags &= ~AA_AWACS;
	}

	if ((flags & AA_ON_RADAR) && (UFC.MasterMode != AG_MODE) && AreMpdsInGivenMode(RADAR_MODE))
	{
		SADrawRadarFOV();

		if (Av.AARadar.CurMode == AA_STT_MODE)
		{
			int Radius = 55;
			float NewX,NewZ;
		   	BOOL aot = PrimaryIsAOT(&CurFramePlanes);

			if( (Primary) && (Primary->Id != -1) && !aot && !Primary->Reaquire.BrokeLockReaquire)
			{
				float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,Planes[Primary->Id].WorldPosition,1);
				float Dist = Dist2D(&PlayerPlane->WorldPosition,&Planes[Primary->Id].WorldPosition);
				Dist *= WUTONM;

				if(Radius > Dist*Av.SA.PixelsPerMile)
				{
					AvRGB Triple;
					GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);
					ProjectPoint(70,74,NormDegree(DeltaAzim),Dist*Av.SA.PixelsPerMile,&NewX, &NewZ);
					DrawAvRadarSymbol(NewX,NewZ,RDR_OPEN_STAR,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);
					DrawPlaneVelocityVector(NewX,NewZ,( (Primary->Reaquire.BrokeLockReaquire) || (Primary->Jammer.JammerFakeOut) ) ? Primary->Reaquire.Heading : Planes[Primary->Id].Heading/DEGREE,AV_GREEN,1.0);
				}
			}
		}
		else
		{
			switch (Av.AARadar.CurMode)
			{
				case AA_TWS_MODE:
					DrawSAList(&PrevFramePlanes,flags);
					break;

				case AA_RWS_MODE:
				case AA_VS_MODE:
					DrawSAList(&CurFramePlanes,flags);
					break;
			}
		}
	}
}

//*****************************************************************************************************************************************
void SADrawAwacsInfo()
{
}


//*****************************************************************************************************************************************
void SADrawFlirInfo()
{
	float NewX,NewZ;

//	if( !AreMpdsInGivenMode(TGT_IR_MODE) ) return;

	FPointDouble TargetPos;
	int TargetAvailable = GetFlirTargetPosition(&TargetPos);

	float Radius = 55;

	float Dist = Dist2D(&PlayerPlane->WorldPosition,&TargetPos);
	Dist *= WUTONM;

	if ((UFC.MasterMode == AA_MODE) && (!TargetAvailable))
	{
		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,TargetPos,1);
		ProjectPoint(70,74,NormDegree(DeltaAzim),60,&NewX, &NewZ);
		DrawTextAtLoc(NewX,NewZ,"F",AV_GREEN,1.0);
	}

	if(TargetAvailable)
	{
		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,TargetPos,1);
		if(Radius > (Dist*Av.SA.PixelsPerMile))
		{
			ProjectPoint(70,74,NormDegree(DeltaAzim),(Dist*Av.SA.PixelsPerMile),&NewX, &NewZ);
			DrawTextAtLoc(NewX,NewZ,"F",AV_GREEN,1.0);
		}
	}
}

//*****************************************************************************************************************************************
void SADrawRadarFOV()
{
	if( (Av.AARadar.CurMode == AA_STT_MODE) || (Av.AARadar.CurMode == AA_ACQ_MODE) ) return;


	float NewX,NewZ;
	AvRGB Triple;

	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	int Dist = (Av.AARadar.Range*Av.SA.PixelsPerMile);
	Dist=55;

	float DeltaAzim = NormDegree(Av.AARadar.LeftAzimEdge);
	ProjectPoint(70,74,DeltaAzim,Dist,&NewX, &NewZ);
	DrawTextAtLoc(NewX,NewZ,"R",AV_GREEN,1.0);

	DeltaAzim = NormDegree(Av.AARadar.RightAzimEdge);
	ProjectPoint(70,74,DeltaAzim,Dist,&NewX, &NewZ);
	DrawTextAtLoc(NewX,NewZ,"R",AV_GREEN,1.0);
}


//*****************************************************************************************************************************************
void SADrawWaypoint()
{
	FPointDouble WaypointLoc;
	float NewX,NewZ;

	WaypointLoc.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
	WaypointLoc.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;
	WaypointLoc.Y = 0;

	int Radius = 55;

	float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,WaypointLoc,1);

	float Dist = Dist2D(&PlayerPlane->WorldPosition,&WaypointLoc);
	Dist *= WUTONM;

	if(Radius < Dist*Av.SA.PixelsPerMile) return;

	ProjectPoint(70,74,NormDegree(DeltaAzim),Dist*Av.SA.PixelsPerMile,&NewX,&NewZ);

	DrawHsiCircle(NewX,NewZ,AV_GREEN,1.0);

	sprintf(TmpStr,"%d",(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
	DrawTextAtLoc(NewX + 4,NewZ + 4,TmpStr,AV_GREEN,1.0);
}

//*****************************************************************************************************************************************
void SADrawBullseye()
{
	FPointDouble BELoc;
	float NewX,NewZ;

	AvRGB Color;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Color);

	BELoc.X = BullsEye.x*FTTOWU;
	BELoc.Z = BullsEye.z*FTTOWU;
	BELoc.Y = 0;

	float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,BELoc,1);
	float Dist      = Dist2D(&PlayerPlane->WorldPosition,&BELoc);
	Dist *= WUTONM;

	int Radius = 55;

	if(Radius < Dist*Av.SA.PixelsPerMile) return;

	ProjectPoint(70,74,NormDegree(DeltaAzim),Dist*Av.SA.PixelsPerMile,&NewX,&NewZ);

	GrDrawPolyCircle(GrBuffFor3D,NewX,NewZ,4,20,Color.Red,Color.Green,Color.Blue,0);

	float TempX,TempZ;
	float AngleToNorth = 360.0 - (PlayerPlane->Heading/DEGREE);

	ProjectPoint(NewX,NewZ,NormDegree(AngleToNorth),11,&TempX,&TempZ);
	GrDrawLine(GrBuffFor3D,NewX,NewZ,TempX,TempZ,Color.Red,Color.Green,Color.Blue,0);
	DrawTriangleRot(TempX,TempZ,3,NormDegree(AngleToNorth),AV_GREEN,1.0,TRUE);
}

//*****************************************************************************************************************************************
void SAToggleDatalink()
{
	Av.SA.DatalinkOn = !Av.SA.DatalinkOn;
}

//*****************************************************************************************************************************************
void SAToggleFlir()
{
	Av.SA.FlirOn = !Av.SA.FlirOn;
}

//*****************************************************************************************************************************************
void SAToggleRadar()
{
	Av.SA.RadarOn = !Av.SA.RadarOn;
}

//*****************************************************************************************************************************************
void SitAwareMode(int MpdNum)
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_GREEN,1.0,&Triple);

	DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	DrawVerticalWordAtButton(MpdNum,1,"D/L",Av.SA.DatalinkOn,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,2,"FLR",Av.SA.FlirOn,AV_GREEN,1.0);
	DrawVerticalWordAtButton(MpdNum,3,"RDR",Av.SA.RadarOn,AV_GREEN,1.0);

	DrawTextAtLoc(16,3,"CMD A/S",AV_GREEN,1.0);
	DrawTextAtLoc(9,10,"CMD ALT",AV_GREEN,1.0);
	// DrawTextAtLoc(2,17,"CMD ROD",AV_GREEN,1.0);

	//sprintf(TmpStr,"%d", (int) Av.Acl.CmdRod );
	//DrawTextAtLoc(8,24,TmpStr,AV_GREEN,1.0);

	DrawTextAtLoc(59,3,"SCL/",AV_GREEN,1.0);

	sprintf(TmpStr,"%d", (int)Av.SA.Range );
	DrawTextAtLoc(75,3,TmpStr,AV_GREEN,1.0);

	if(PlayerPlane->AI.CurrWay)
	{
		FPointDouble WPPos;

		WPPos.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
		WPPos.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;
		WPPos.Y = PlayerPlane->AI.CurrWay->lWPy*FTTOWU;

		float Dist = (PlayerPlane->WorldPosition / WPPos)*WUTONM;

		sprintf(TmpStr,"%5.1f W%d", Dist,UFC.DataCurrSP);
		DrawTextAtLoc(88,3,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%d", (int) PlayerPlane->AI.CurrWay->iSpeed);
		DrawTextAtLoc(44,3,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%d", (int) PlayerPlane->AI.CurrWay->lWPy);
		DrawTextAtLoc(36,10,TmpStr,AV_GREEN,1.0);

		float NewX,NewZ;
		float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,WPPos,1);

		ProjectPoint(70,74,NormDegree(DeltaAzim),57,&NewX,&NewZ);
		DrawOuterRingArrow(NewX,NewZ,NormDegree(DeltaAzim));

		ProjectPoint(70,74,NormDegree(DeltaAzim),58,&NewX,&NewZ);
		DrawOuterRingArrow(NewX,NewZ,NormDegree(DeltaAzim));
	}

	GrDrawLine(GrBuffFor3D,1,16,16,1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,16,1,55,1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,55,1,55,16,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,55,16,31,16,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,31,16,31,30,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,31,30,1,30,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,1,30,1,16,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawLine(GrBuffFor3D,85,1,124,1,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,124,1,139,16,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,139,16,139,31,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,139,31,113,31,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,113,31,113,16,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,113,16,85,16,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,85,16,85,1,Triple.Red,Triple.Green,Triple.Blue,0);

	// draw plane in center
	GrDrawLine(GrBuffFor3D,70,78,70,72,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,67,74,73,74,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLine(GrBuffFor3D,69,77,71,77,Triple.Red,Triple.Green,Triple.Blue,0);

	GrDrawPolyCircle(GrBuffFor3D,70,74,27,34,Triple.Red,Triple.Green,Triple.Blue,0);

	float Angle;
	int   TotalLength = 55;
	int   End1X,End1Y;
	float TotalAngle;
	float EAngle,NAngle,SAngle,WAngle;
	float Heading,PreciseHeading;
	int   TempCenterX, TempCenterY;

	Heading = PlayerPlane->Heading/DEGREE;

	PreciseHeading = PlayerPlane->Heading*100;
	PreciseHeading = PreciseHeading/DEGREE;
	PreciseHeading = PreciseHeading/100.0;

	NAngle = NormDegree(360.0 - Heading + 90.0);
	SAngle = NormDegree(NAngle + 180.0);
	EAngle = NormDegree(NAngle + 270.0);
	WAngle = NormDegree(NAngle + 90.0);

	TempCenterX = 70;
	TempCenterY = 74;

	Angle = NAngle;
	TotalAngle = 0.0;

	int Tick = 36;

  	while(TotalAngle < 360.0)
  	{
	  	End1X = TempCenterX + ICosTimes((Angle*DEGREE), TotalLength);
    	End1Y = TempCenterY - ISinTimes((Angle*DEGREE), TotalLength);

	  	if(Angle == NAngle)
	    	DrawTextAtLoc(End1X-2,End1Y-2,"N",AV_GREEN,1.0);
	  	else if(Angle == SAngle)
	    	DrawTextAtLoc(End1X-2,End1Y-2,"S",AV_GREEN,1.0);
    	else if(Angle == EAngle)
	    	DrawTextAtLoc(End1X-2,End1Y-2,"E",AV_GREEN,1.0);
	  	else if(Angle == WAngle)
	    	DrawTextAtLoc(End1X-2,End1Y-2,"W",AV_GREEN,1.0);
		else if(fmod(TotalAngle,30) == 0)
		{
			sprintf(TmpStr,"%d",Tick);
			DrawTextAtLoc(End1X-2,End1Y-2,TmpStr,AV_GREEN,1.0);
		}
	  	else
	    	DrawAvDot(End1X,End1Y,AV_GREEN,1.0);

		  Tick--;
	  	TotalAngle += 10.0;
	  	Angle = NormDegree(Angle + 10.0);
	}

	int sa_flag;

	if(Av.SA.RadarOn)
		sa_flag = AA_ON_RADAR;
	else
		sa_flag = 0;

	if(Av.SA.DatalinkOn)
		sa_flag |= AA_AWACS;

	if (sa_flag)
		SADrawPlaneInfo(sa_flag);

	if((Av.SA.FlirOn) && (Av.Weapons.HasTargetIR) && (UFC.MasterMode == AG_MODE))
		SADrawFlirInfo();

//		SADrawAwacsInfo();

	SADrawWaypoint();

	SADrawBullseye();

	int ButtonPress = GetButtonPress(MpdNum);
	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case 1:
				SAToggleDatalink();
			break;
			case 2:
				SAToggleFlir();
			break;
			case 3:
				SAToggleRadar();
			break;
			case 7:
				Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			break;
			case 17:
				SARangeUp();
			break;
		}
	}
}

//*****************************************************************************************************************************************
// EDF MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void EdfMode(int MpdNum)
{
    AvRGB Triple;
    GetRGBFromAVPalette(AV_GREEN,0.6,&Triple);

	float Frac,YPos;
	float FuelInT;

	DrawBox(GrBuffFor3D,20,19,14,11,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,37,19,14,11,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,54,19,14,11,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,71,19,14,11,Triple.Red,Triple.Green,Triple.Blue);
	DrawBox(GrBuffFor3D,88,19,14,11,Triple.Red,Triple.Green,Triple.Blue);

	Frac = Av.Fuel.Tanks[AV_FUEL_EX_LO]/Av.Fuel.TanksMax[AV_FUEL_EX_LO];
	YPos = LERP(30,20,Frac);
	if(YPos < 30)
	  DrawSquarePoly(21,YPos,12,30-YPos,AV_GREEN,1.0);

	Frac = Av.Fuel.Tanks[AV_FUEL_EX_LI]/Av.Fuel.TanksMax[AV_FUEL_EX_LI];
    YPos = LERP(30,20,Frac);
	if(YPos < 30)
		DrawSquarePoly(38,YPos,12,30-YPos,AV_GREEN,1.0);

	Frac = Av.Fuel.Tanks[AV_FUEL_EX_C]/Av.Fuel.TanksMax[AV_FUEL_EX_C];
	YPos = LERP(30,20,Frac);
	if(YPos < 30)
		DrawSquarePoly(55,YPos,12,30-YPos,AV_GREEN,1.0);

	Frac = Av.Fuel.Tanks[AV_FUEL_EX_RI]/Av.Fuel.TanksMax[AV_FUEL_EX_RI];
    YPos = LERP(30,20,Frac);
	if(YPos < 30)
	  DrawSquarePoly(72,YPos,12,30-YPos,AV_GREEN,1.0);

	Frac = Av.Fuel.Tanks[AV_FUEL_EX_RO]/Av.Fuel.TanksMax[AV_FUEL_EX_RO];
    YPos = LERP(30,20,Frac);
	if(YPos < 30)
 	   DrawSquarePoly(89,YPos,12,30-YPos,AV_GREEN,1.0);

	DrawBox(GrBuffFor3D,45,4,33,11,Triple.Red,Triple.Green,Triple.Blue);

	Frac = PlayerPlane->InternalFuel/Av.Fuel.TotalInternal;
	YPos = LERP(15,5,Frac);
	if(YPos < 15)
	  DrawSquarePoly(46,YPos,31,15-YPos,AV_GREEN,1.0);

		Frac = Av.Fuel.BingoVal/14075.0;
    YPos = LERP(15,4,Frac);
		DrawTriangleRot(45,YPos,3,270.0,AV_GREEN,0.6,TRUE);

		DrawTextAtLoc(2,4,"TOTAL",AV_GREEN,1.0);

		FuelInT = PlayerPlane->InternalFuel + PlayerPlane->WingDropFuel + PlayerPlane->CenterDropFuel;
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(23,2,TmpStr,AV_GREEN,1.0,TRUE);

		DrawTextAtLoc(3,13,"BINGO",AV_GREEN,1.0);

		FuelInT = Av.Fuel.BingoVal;
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(23,11,TmpStr,AV_GREEN,1.0,TRUE);

		DrawTextAtLoc(4,24,"EXT",AV_GREEN,1.0);

		FuelInT = PlayerPlane->WingDropFuel + PlayerPlane->CenterDropFuel;
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(4,30,TmpStr,AV_GREEN,1.0,TRUE);

		FuelInT = Av.Fuel.Tanks[AV_FUEL_EX_LO];
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(24,32,TmpStr,AV_GREEN,1.0);

		FuelInT = Av.Fuel.Tanks[AV_FUEL_EX_LI];
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(40,32,TmpStr,AV_GREEN,1.0);

		FuelInT = Av.Fuel.Tanks[AV_FUEL_EX_C];
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(57,32,TmpStr,AV_GREEN,1.0);

		FuelInT = Av.Fuel.Tanks[AV_FUEL_EX_RI];
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(74,32,TmpStr,AV_GREEN,1.0);

		FuelInT = Av.Fuel.Tanks[AV_FUEL_EX_RO];
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(92,32,TmpStr,AV_GREEN,1.0);

		DrawTextAtLoc(26,44,"RPM",AV_GREEN,1.0);
		DrawTextAtLoc(24,52,"TEMP",AV_GREEN,1.0);
		DrawTextAtLoc(28,60,"FF",AV_GREEN,1.0);
		DrawTextAtLoc(27,68,"OIL",AV_GREEN,1.0);
		DrawTextAtLoc(79,44,"NOZ",AV_GREEN,1.0);

		sprintf(TmpStr,"%d",(int)EMD_lOpen/* NozLeftVar*/);
		DrawTextAtLoc(68,68,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"%d",(int)EMD_rOpen/* NozRightVar*/);
		DrawTextAtLoc(94,68,TmpStr,AV_GREEN,1.0);

		DrawTextAtLoc(89,2,"INT",AV_GREEN,1.0);

		FuelInT = PlayerPlane->InternalFuel;
		FuelInT /= 1000.0;

		sprintf(TmpStr,"%2.1f",FuelInT);
		DrawTextAtLoc(89,8,TmpStr,AV_GREEN,1.0,TRUE);

		sprintf(TmpStr,"%3.0f", EMD_lSpin+9*(100.0f-EMD_lSpin)/100.0f /*N2Var*/);
		RightJustifyTextAtLocation(18,44,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f",EMD_lTemp /*EgtVar*/);
		RightJustifyTextAtLocation(18,52,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f",UFC.LeftFuelFlowRate*60/* FFLeftVar*/);
		RightJustifyTextAtLocation(18,60,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		float wtemp = EMD_lOlio * (1.0f/80.0f);
		wtemp *= wtemp;
		wtemp *= 150.0f;
		wtemp+=20.0f+40*EMD_lOlio * (1.0f/80.0f);
		sprintf(TmpStr,"%3.0f",wtemp /* OilLeftVar*/);
		RightJustifyTextAtLocation(18,68,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%3.0f", EMD_rSpin+9*(100.0f-EMD_rSpin)/100.0f /*N2Var*/);
		RightJustifyTextAtLocation(57,44,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f",EMD_rTemp /*EgtVar*/);
		RightJustifyTextAtLocation(57,52,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		sprintf(TmpStr,"%4.0f",UFC.RightFuelFlowRate*60/* FFRightVar*/);
		RightJustifyTextAtLocation(57,60,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		wtemp = EMD_rOlio * (1.0f/80.0f);
		wtemp *= wtemp;
		wtemp *= 150.0f;
		wtemp+=20.0f+40*EMD_rOlio * (1.0f/80.0f);
		sprintf(TmpStr,"%3.0f",wtemp /* OilRightVar*/);
		RightJustifyTextAtLocation(57,68,TmpStr,Triple.Red,Triple.Green,Triple.Blue);

		Frac = EMD_lOpen/100.0;
		GrDrawPolyCircleArcPolyBuff( GrBuffFor3D,73,58,9,9.0*Frac,0,360,30,Triple.Red,Triple.Green,Triple.Blue,0);

		//Frac = EMD_lOpen/100.0;
	 	//GrDrawPolyCircle(GrBuffFor3D,73,58,9,30,Triple.Red,Triple.Green,Triple.Blue,0);
		//GrDrawPolyCircle(GrBuffFor3D,73,58,9.0*Frac,30,Triple.Red,Triple.Green,Triple.Blue,0);

		 Frac = EMD_rOpen/100.0;
		GrDrawPolyCircleArcPolyBuff( GrBuffFor3D,95,58,9,9.0*Frac,0,360,30,Triple.Red,Triple.Green,Triple.Blue,0);

		//Frac = EMD_rOpen/100.0;
		// GrDrawPolyCircle(GrBuffFor3D,95,58,9,30,Triple.Red,Triple.Green,Triple.Blue,0);
		//GrDrawPolyCircle(GrBuffFor3D,95,58,9.0*Frac,30,Triple.Red,Triple.Green,Triple.Blue,0);

		GrDrawLine(GrBuffFor3D,0,39,106,39,Triple.Red,Triple.Green,Triple.Blue,0);
		GrDrawLine(GrBuffFor3D,61,39,61,76,Triple.Red,Triple.Green,Triple.Blue,0);
}

//*****************************************************************************************************************************************
// HUD  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawMdiHeadingBar(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,0.6,&Triple);

	ANGLE heading = -(int)PlayerPlane->Heading;	// true compass heading

	// Compute sprite offset based on heading  [0..7]
	float SprOfs = 15 * (float)( (float)( (heading % (5*DEGREE)) / (float)(5*DEGREE) ) );

	// Find first tick mark to left of center.
	float xPos = 70.0 - SprOfs;
	float xVal = (heading/DEGREE) - ((heading/DEGREE) % 5);	// round down to even degree

	// Adjust heading val to bar center to prevent waypoint jitter.
	heading = xVal*DEGREE + SprOfs*DEGREE/3;

	// Move right to last tick, ie. add 15 degrees
	xVal += 15;
	xPos +=	15*3;

	if(xVal >= 360.0)
		xVal -= 360.0;

	// Label every 5,10 degrees across heading bar.
	while (xPos >= 70-45)
	{

		if (xPos <= 70+45)
		{
			IntTo3DigitFont(xVal,TmpStr);

			if(fmod(xVal,10) == 0)
			{
			 	DrawTextAtLoc(xPos-6,14-8,TmpStr,AV_GREEN,1.0);
		   	//HUDtextSm( xPos-6, 14-8,TmpStr);
	     	GrDrawLineClipped(GrBuffFor3D,xPos,14,xPos,18,Triple.Red,Triple.Green,Triple.Blue,0);
		   	//HUDline(xPos,14,xPos,16);
			}
			else
			{
	     	GrDrawLineClipped(GrBuffFor3D,xPos,14,xPos,18,Triple.Red,Triple.Green,Triple.Blue,0);
 		   	//HUDline(xPos,14,xPos,18);
			}
		}

		xVal -= 5;

		if (xVal < 0.0)
		   xVal += 360.0;

		xPos -= 5*3;	// 3 pixels / degree
	}

	if (PlayerPlane->SystemInactive & DAMAGE_BIT_ADC)
	{
		return;
	}

	ANGLE wayHead;	// waypoint heading in DEGREEs

	if (UFC.SteeringMode == TGT_STR)
		wayHead = UFC.DataCurrTGTAbsBrg * DEGREE;
	else
		wayHead = UFC.DataCurrSPMilBrg  * DEGREE;

	// this cast forces sign extension for +/- 180°
	int wayOff = (signed short)(wayHead - heading);	// +/- DEGREEs from heading

	xPos = 70 + 3*wayOff/DEGREE;			// move waypoint to its spot

	if (xPos < 70-49 || xPos > 70+49)	// off scale, mark at end with heading
	{
		xPos = 70 + (wayOff<0 ? -49 : 49);
		sprintf(TmpStr,"%3d", wayHead/DEGREE);
	  DrawTextAtLoc(xPos-5,21,TmpStr,AV_GREEN,1.0);
		//HUDtextSm( xPos-5, 21, "%3d", wayHead/DEGREE );
	}

	GrDrawLineClipped(GrBuffFor3D,xPos  ,16,xPos,  20,Triple.Red,Triple.Green,Triple.Blue,0);
	GrDrawLineClipped(GrBuffFor3D,xPos+1,16,xPos+1,20,Triple.Red,Triple.Green,Triple.Blue,0);

	//HUDline( xPos  , 16, xPos  , 20);
  //HUDline( xPos+1, 16, xPos+1, 20);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

extern double HUDsin;
extern double HUDcos;

void DrawHudFlir(FMatrix &mat,BOOL white_hot);

void HudMode(int MpdNum)
{
  AvRGB Triple;
  GetRGBFromAVPalette(AV_GREEN,0.6,&Triple);

	int top,bottom,left,right;


	left = GrBuffFor3D->ClipLeft;
	right = GrBuffFor3D->ClipRight;
	top = GrBuffFor3D->ClipTop;
	bottom = GrBuffFor3D->ClipBottom;

	GrBuffFor3D->ClipLeft = 0;
	GrBuffFor3D->ClipRight = 140;
	GrBuffFor3D->ClipTop = 0;
	GrBuffFor3D->ClipBottom = 140;

	DrawMdiHeadingBar(MpdNum);

	if(!(PlayerPlane->SystemInactive & DAMAGE_BIT_ADC))
	{
		int i,k;
		int p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,p5x,p5y,p6x,p6y;
		int RotOfsX,RotOfsY;

		// pitch ladder has two modes fixed and yaw movement

		#define OFS	5	// ladders above/below center

		int PitchIndex = PlayerPlane->Pitch / (5*DEGREE);

		int Start = PitchIndex - OFS;
		int Stop  = PitchIndex + OFS;

		// Compute trig once using negated roll and 5 pixel length
		double dSin, dCos;

		dSin = 5 * HUDsin;
		dCos = 5 * HUDcos;

		float mulres = 640.0f/(float)ScreenSize.cx;

		for (i=Start; i<Stop; i++)
		{
			k = i;
			if (k< 0) k += 72;
			if (k>71) k -= 72;

			p1x = 70 + (UFC.HUDLadder[k].HUDPoints[0].iSX - 320)*mulres;
			p1y = 60 + (UFC.HUDLadder[k].HUDPoints[0].iSY - 116)*mulres;

			if ((k!=18) && (k!=54))	   // don't do pitch ladder line at +/- 90
			{
				p2x = 70 + (UFC.HUDLadder[k].HUDPoints[1].iSX - 320)*mulres;
				p2y = 60 + (UFC.HUDLadder[k].HUDPoints[1].iSY - 116)*mulres;
				p3x = 70 + (UFC.HUDLadder[k].HUDPoints[2].iSX - 320)*mulres;
				p3y = 60 + (UFC.HUDLadder[k].HUDPoints[2].iSY - 116)*mulres;
				p4x = 70 + (UFC.HUDLadder[k].HUDPoints[3].iSX - 320)*mulres;
				p4y = 60 + (UFC.HUDLadder[k].HUDPoints[3].iSY - 116)*mulres;
				p5x = 70 + (UFC.HUDLadder[k].HUDPoints[4].iSX - 320)*mulres;
				p5y = 60 + (UFC.HUDLadder[k].HUDPoints[4].iSY - 116)*mulres;
				p6x = 70 + (UFC.HUDLadder[k].HUDPoints[5].iSX - 320)*mulres;
				p6y = 60 + (UFC.HUDLadder[k].HUDPoints[5].iSY - 116)*mulres;

				int PitchNum = UFC.HUDLadder[k].PitchNum;

				if (PitchNum >= 0)
				{
			  GrDrawLineClipped(GrBuffFor3D,p1x,p1y,p2x,p2y,Triple.Red,Triple.Green,Triple.Blue,0);
			  GrDrawLineClipped(GrBuffFor3D,p3x,p3y,p4x,p4y,Triple.Red,Triple.Green,Triple.Blue,0);

					//HUDline( p1x, p1y, p2x, p2y );	// upper ladder
					//HUDline( p3x, p3y, p4x, p4y );
				}
				else
				{
			  GrDrawLineClipped(GrBuffFor3D,p1x,p1y,p2x,p2y,Triple.Red,Triple.Green,Triple.Blue,0);
			  GrDrawLineClipped(GrBuffFor3D,p3x,p3y,p4x,p4y,Triple.Red,Triple.Green,Triple.Blue,0);
			  //GrDrawDashedLineClipped(GrBuffFor3D,p1x,p1y,p2x,p2y,Triple.Red,Triple.Green,Triple.Blue,0);
			  //GrDrawDashedLineClipped(GrBuffFor3D,p3x,p3y,p4x,p4y,Triple.Red,Triple.Green,Triple.Blue,0);

					//HUDdash( p1x, p1y, p2x, p2y );	// lower ladder
					//HUDdash( p3x, p3y, p4x, p4y );
				}

			GrDrawLineClipped(GrBuffFor3D,p1x,p1y,p5x,p5y,Triple.Red,Triple.Green,Triple.Blue,0);
			GrDrawLineClipped(GrBuffFor3D,p4x,p4y,p6x,p6y,Triple.Red,Triple.Green,Triple.Blue,0);

				//HUDline( p1x, p1y, p5x, p5y );		// left  end tab
				//HUDline( p4x, p4y, p6x, p6y );		// right end tab

				sprintf( TmpStr, "%2d", abs(PitchNum) );

				if (PitchNum != 0)
				{
					float px, py;
				int Inside = IsPointInside(p1x,p1y,0,140,0,130);

					if (Inside)
					{
						px = (p1x + p5x) / 2.0;	// mid-point of end tab
						py = (p1y + p5y) / 2.0;

						RotOfsX = round( px - dCos - 3 );
						RotOfsY = round( py + dSin - 2 );

			   DrawTextAtLoc(RotOfsX, RotOfsY,TmpStr,AV_GREEN,1.0);


					//	HUDtextSm(  RotOfsX , RotOfsY, TmpStr );
					}

				Inside = IsPointInside(p4x, p4y,0,140,0,130);

					if (Inside)
					{
						px = (p4x + p6x) / 2.0;	// mid-point of end tab
						py = (p4y + p6y) / 2.0;

						RotOfsX = round( px + dCos - 3 );
						RotOfsY = round( py - dSin - 2 );

			  DrawTextAtLoc(RotOfsX, RotOfsY,TmpStr,AV_GREEN,1.0);

						//HUDtextSm( RotOfsX, RotOfsY, TmpStr );
					}
				}
			}
			else	// 90° pitch icon
			{
				//HUDicon( p1x, p1y, (k==18) ? SM_POS_90 : SM_NEG_90 );
			}
		}


		// hud alt
		int xPos,yPos;

		xPos = 111, yPos = 40;

		int nDig = sprintf( TmpStr, "%d", (int)PlayerPlane->Altitude );

		if (nDig > 3)	// print below 1000 w/ small font
		{
			char *Mp = TmpStr + (nDig-3);
		DrawTextAtLoc(xPos+10,yPos+2,Mp,AV_GREEN,1.0);
			//HUDtextSm( xPos+10, yPos+2, Mp );
			*Mp = 0;	// kill below 1000
			xPos += 3;	// adjust for small font
		}

		xPos += (5-nDig) * 5 - 3;	// skip to 1st digit

		DrawTextAtLoc(xPos,yPos,TmpStr,AV_GREEN,1.0,TRUE);
		//HUDtextLg( xPos, yPos, TmpStr );

		DrawBox(GrBuffFor3D,xPos-2,yPos-1,23,8,Triple.Red,Triple.Green,Triple.Blue);


		// airspeed
		xPos = 11;
		yPos = 40;

		DrawBox(GrBuffFor3D,xPos-2,yPos-2,20,8,Triple.Red,Triple.Green,Triple.Blue);
		//HUDbox( HUD_IAS_WIN(-2,-2+yPos), HUD_IAS_WIN(20,8+yPos) );

		sprintf(TmpStr,"%4d",(int)PlayerPlane->IndicatedAirSpeed);
		DrawTextAtLoc(xPos,yPos,TmpStr,AV_GREEN,1.0);
		//HUDtextLg( HUD_IAS_WIN(0,yPos), "%4d",(int)P->IndicatedAirSpeed );

		xPos = 11;
		yPos = 83;

		sprintf(TmpStr,"M  %1.3f", PlayerPlane->Mach);
		DrawTextAtLoc(xPos,yPos,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"G  %2.1f", PlayerPlane->GForce);
		DrawTextAtLoc(xPos,yPos+6,TmpStr,AV_GREEN,1.0);

		sprintf(TmpStr,"   %2.1f", PlayerPlane->AvailGForce);
		DrawTextAtLoc(xPos,yPos+12,TmpStr,AV_GREEN,1.0);

		//jlm need aoa sprite for mdi
		//DrawAvRadarSymbol(x,y,RDR_AOA,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

		// jlm need to make AOA not static. hud is checked out
		sprintf(TmpStr,"   %3.1f",0/*AOA_unit*/);
		DrawTextAtLoc(xPos,yPos+18,TmpStr,AV_GREEN,1.0);
	}

	// MENU
	if(MpdNum != MPD6) DrawWordAtMpdButton(MpdNum,7,"MENU",FALSE,AV_GREEN,1.0);

	// jlm need Vel Vector symbol here
	//DrawAvRadarSymbol(x,y,RDR_AOA,Triple.Red,Triple.Green,Triple.Blue,GrBuffFor3D);

	if ( Mpds[MpdNum].Mode != NAVFLIR_MODE)
	{
		int ButtonPress = GetButtonPress(MpdNum);
		if(ButtonPress != -1)
		{
			switch(ButtonPress)
			{
				case 7: if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				      	break;
			}
		}
	}

	GrBuffFor3D->ClipLeft   = left;
	GrBuffFor3D->ClipRight  = right;
	GrBuffFor3D->ClipTop    = top;
	GrBuffFor3D->ClipBottom = bottom;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void NavFlirMode(int MpdNum)
{
	FMatrix mat;

	mat = PlayerPlane->Attitude;
	mat	*= ViewMatrix;

	DrawHudFlir(mat,(BOOL)(Av.NavFlir.Polarity != WHOT));

	if (!Av.NavFlir.DeclutterOn)
		HudMode(MpdNum);

	DrawMdiArrow(2,21,0);
	DrawMdiArrow(2,51,1);

	DrawMdiArrow(2,68,0);
	DrawMdiArrow(2,98,1);

  	DrawVertWordAtLocation(5,23,"LVL",FALSE,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",Av.NavFlir.Contrast);
	DrawTextAtLoc(10,29,TmpStr,AV_GREEN,1.0);

  	DrawVertWordAtLocation(5,70,"BRT",FALSE,AV_GREEN,1.0);
	sprintf(TmpStr,"%d",Av.NavFlir.Brightness);
	DrawTextAtLoc(10,76,TmpStr,AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,8,"DCLTR",(Av.NavFlir.DeclutterOn),AV_GREEN,1.0);

	DrawWordAtMpdButton(MpdNum,6,(BOOL)(Av.NavFlir.Polarity == WHOT) ? "WHT" : "BLK",FALSE,AV_GREEN,1.0);

  	int ButtonPress = GetButtonPress(MpdNum);

	if(ButtonPress != -1)
	{
		switch(ButtonPress)
		{
			case  0:
				NavFlirContrastUp();
					break;

			case  1:
				NavFlirContrastDown();
					break;

			case  2:
				NavFlirBrightnessUp();
					break;

			case  3:
				NavFlirBrightnessDown();
					break;

			case  6:  NavFlirTogglePolarity();
								break;

			case  7:  if(MpdNum != MPD6) Mpds[MpdNum].Mode = MAIN_MENU_MODE;
							  break;

			case  8:
				NavFlirToggleDeclutter();
				break;
		}
	}

}

//*****************************************************************************************************************************************
// COUNTER MEASURE CODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define CM_CHAFF 0
#define CM_FLARE 1

typedef struct
{
	int Type;
	int Time;
} CMScheduleType;

CMScheduleType CMSchedule[120];
int NumCMScheduled;

extern int ChaffTotal;
extern int FlareTotal;

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitCM()
{
	NumCMScheduled = 0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetNumChaffFlareInSchedule(int Type)
{
	int Count = 0;

	CMScheduleType *S = &CMSchedule[0];
	int Index = NumCMScheduled;
	while(Index-- > 0)
	{
		if(S->Type == Type)
			Count++;
		S++;
	}

	return(Count);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DropScheduledCM(int NumChaff,int NumFlare)
{
  int j;
  if( (NumFlare > 5) || (NumChaff > 5) )
    j = 4;

  for(int i=0; i<NumChaff; i++)
  {
    if(ChaffTotal - GetNumChaffFlareInSchedule(CM_CHAFF) > 0)
	  {
				if(NumCMScheduled+1 < 120)
				{
      	  NumCMScheduled++;
	  		  CMSchedule[NumCMScheduled-1].Type = CM_CHAFF;
	  		  CMSchedule[NumCMScheduled-1].Time = GameLoopInTicks + i*300;
				}
    }
  }

  for(i=0; i<NumFlare; i++)
  {
    if(FlareTotal - GetNumChaffFlareInSchedule(CM_FLARE) > 0)
	  {
				if(NumCMScheduled+1 < 120)
				{
      	  NumCMScheduled++;
	  		  CMSchedule[NumCMScheduled-1].Type = CM_FLARE;
	  		  CMSchedule[NumCMScheduled-1].Time = GameLoopInTicks + i*300;
				}
    }
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimCMDrop()
{
	CMScheduleType *TimeWalk = &CMSchedule[0];
  CMScheduleType *LastTime = &CMSchedule[NumCMScheduled-1];
  int Count     = NumCMScheduled;
  int CurTime   = GameLoopInTicks;

  while(Count-- > 0)
  {
    if(CurTime > TimeWalk->Time)
		{
				int Type = (TimeWalk->Type == CM_CHAFF) ? 1 : 2;
      	TimeWalk->Type = LastTime->Type;
      	TimeWalk->Time = LastTime->Time;
      	NumCMScheduled--;
	  	  if(cPlayerLimitWeapons)
				{
					if(Type == 1)
					  ChaffTotal--;
					else
					 FlareTotal--;
				}
  	  	InstantiateCounterMeasure(PlayerPlane,CHAFF_FLARE_STATION,Type);
	  	  return;
		}
		TimeWalk++;
  }
}

void ToggleIDECM()
{
	Av.Tews.CM.IDECMStat++;

	if(Av.Tews.CM.IDECMStat > CM_IDECM_AUTO)
		 Av.Tews.CM.IDECMStat = CM_IDECM_MAN;

	if( (Av.Tews.CM.IDECMStat == CM_IDECM_MAN) && (Av.Tews.CurProg > 4) )
	   Av.Tews.CurProg = 0;

	Av.Tews.CM.AutoTimer.Disable();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleCMDDSDecoyControl()
{
	if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_STOW)
	{
		if(PlayerPlane->TowedDecoyPosition == 0.0)
		{
			 if( !(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) )
			 {
		     DeployTowedDecoy(PlayerPlane);
		     Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_DEPLOY;
			 }
		}
	}
	else if( (Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_DEPLOY) || (Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_JAM) )
	{
		if(PlayerPlane->TowedDecoyPosition == 1.0)
		{
		   RetractTowedDecoy(PlayerPlane);
		   Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_STOW;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsDecoyJamming()
{
	return( (Av.Tews.CM.CurCMDDS != -1) && (!UFC.EMISState) && (Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_JAM) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleCMDDSTransmit()
{
	if(Av.Tews.CM.CurCMDDS == -1) return;

	if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_JAM)
		Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_DEPLOY;
	else if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_DEPLOY)
		Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_JAM;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DamageCMDDS()
{
	if(Av.Tews.CM.CurCMDDS == -1) return;
	if(PlayerPlane->TowedDecoyPosition == 0.0) return;

	CutTowedDecoyCable(PlayerPlane);

  Av.Tews.CM.CurCMDDS++;
	if(Av.Tews.CM.CurCMDDS > 2)
		Av.Tews.CM.CurCMDDS = -1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleICS()
{
	if(Av.Tews.CM.ICSState == CM_ICS_STBY)
	{
		if(!UFC.EMISState)
		{
		  Av.Tews.CM.ICSState = CM_ICS_JAM;
	    PlayerPlane->AI.iAIFlags1 |= AIJAMMINGON;
		}
	}
	else if(Av.Tews.CM.ICSState == CM_ICS_JAM)
	{
		Av.Tews.CM.ICSState = CM_ICS_STBY;
		PlayerPlane->AI.iAIFlags1 &= ~AIJAMMINGON;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsICSJamming()
{
  return(Av.Tews.CM.ICSState == CM_ICS_JAM);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimCounterMeasures()
{
	SimCMDrop();

	if(Av.Tews.CM.IDECMStat == CM_IDECM_MAN)
	{
			if(Av.Tews.CM.ProgInitiated)
			{
					if(!Av.Tews.CM.AutoTimer.IsActive())
					{
						Av.Tews.CM.AutoTimer.Set(TewsProgs[Av.Tews.CurProg].Interval,GameLoopInTicks);
					  Av.Tews.CM.ProgRepeat = TewsProgs[Av.Tews.CurProg].Repeat;
					}
					else if( Av.Tews.CM.AutoTimer.TimeUp(GameLoopInTicks) )
					{
   				  DropScheduledCM(TewsProgs[Av.Tews.CurProg].NumChaff,TewsProgs[Av.Tews.CurProg].NumFlares);
		 				Av.Tews.CM.AutoTimer.Set(TewsProgs[Av.Tews.CurProg].Interval,GameLoopInTicks);
					  Av.Tews.CM.ProgRepeat--;
						if(Av.Tews.CM.ProgRepeat <= 0)
						{
							Av.Tews.CM.ProgInitiated = FALSE;
							Av.Tews.CM.AutoTimer.Disable();
						}
					}
			}

			return;
	}

	int MaxProg = -1;
	TewsProgType *Prog;

  ThreatEntryType *T = &Threats.List[0];

	int Index = Threats.NumThreats;
	while(Index-- > 0)
  {
		if(T->Firing || T->HasLock)
		{
			if(T->Prog + 4 > MaxProg)
				MaxProg = T->Prog + 4;

			if(MaxProg > 9) // just in case garbage comes in
			  MaxProg = 9;
		}
	  T++;
  }

	if(MaxProg != -1)
	{
		Prog = &TewsProgs[MaxProg];

		if(Av.Tews.CurProg != MaxProg)
		{
			Av.Tews.CM.AutoTimer.Disable();
		}

		Av.Tews.CurProg = MaxProg;
		if( !Av.Tews.CM.AutoTimer.IsActive() )
		{
			Av.Tews.CM.AutoTimer.Set(Prog->Interval,GameLoopInTicks);
			Av.Tews.CM.ProgRepeat = Prog->Repeat;
		}
	}
	else
	{
	 	Av.Tews.CM.AutoTimer.Disable();
	 	return;
	}

	if(Av.Tews.CM.IDECMStat == CM_IDECM_AUTO)
	{
			if( Av.Tews.CM.AutoTimer.TimeUp(GameLoopInTicks) )
			{
				DropScheduledCM(Prog->NumChaff,Prog->NumFlares);
				Av.Tews.CM.AutoTimer.Set(Prog->Interval,GameLoopInTicks);

				// auto deploy decoy
			  if( !(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) )
				{
					if(Av.Tews.CM.CurCMDDS != -1)
					{
						if(PlayerPlane->TowedDecoyPosition <= 0.0)
						{
						  DeployTowedDecoy(PlayerPlane);
		          Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_JAM;
						}
					}
				}

				// turn on jammer
				if(!UFC.EMISState)
				{
		  		Av.Tews.CM.ICSState = CM_ICS_JAM;
	    		PlayerPlane->AI.iAIFlags1 |= AIJAMMINGON;
				}
			}

	}
	else if(Av.Tews.CM.IDECMStat == CM_IDECM_SEMI)
	{
			if(Av.Tews.CM.ProgInitiated)
			{
					if( Av.Tews.CM.AutoTimer.TimeUp(GameLoopInTicks) )
					{
   				  DropScheduledCM(Prog->NumChaff,Prog->NumFlares);
		 				Av.Tews.CM.AutoTimer.Set(Prog->Interval,GameLoopInTicks);
					  Av.Tews.CM.ProgRepeat--;
						if(Av.Tews.CM.ProgRepeat <= 0)
						{
							Av.Tews.CM.ProgInitiated = FALSE;
							Av.Tews.CM.AutoTimer.Disable();
						}
					}
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CounterMeasuresProgramInitiate()
{
	if(Av.Tews.CM.IDECMStat == CM_IDECM_AUTO) return;

	Av.Tews.CM.ProgInitiated = TRUE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DropSingleChaff()
{
	DropScheduledCM(1,0);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DropSingleFlare()
{
  DropScheduledCM(0,1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AvionicsRangeUp()
{
	int Mode = GetTDCMode();
  if(Mode == -1) return;

	if(Mode == RADAR_MODE)
	{
		if(UFC.MasterMode != AG_MODE)
		{
		  if( (Av.AARadar.CurMode != AA_STT_MODE) && (Av.AARadar.CurMode != AA_ACQ_MODE) )
			  AARangeUp();
		}
		else
		{
		  if( (Av.AGRadar.CurMode != AG_FTT_MODE) && (Av.AGRadar.CurMode != AG_MAP_MODE) )
			  AGRangeUp();
		}
	}
	else if(Mode == HSI_MODE)
	{
		 if(!Av.Hsi.DataOn)
		   HsiIncrementRange();
	}
	else if(Mode == SA_MODE)
	{
		 SARangeUp();
	}
	else if(Mode == ACL_MODE)
	{
		 AclRangeUp();
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AvionicsRangeDown()
{
	int Mode = GetTDCMode();
	if(Mode == -1)
		return;

	if(Mode == RADAR_MODE)
	{
		if(UFC.MasterMode != AG_MODE)
		{
		  if( (Av.AARadar.CurMode != AA_STT_MODE) && (Av.AARadar.CurMode != AA_ACQ_MODE) )
			  AARangeDown();
		}
		else
		{
		  if( (Av.AGRadar.CurMode != AG_FTT_MODE) && (Av.AGRadar.CurMode != AG_MAP_MODE) )
			  AGRangeDown();
		}
	}
	else if(Mode == HSI_MODE)
	{
		 if(!Av.Hsi.DataOn)
		   HsiDecrementRange();
	}
	else if(Mode == SA_MODE)
		 SARangeDown();
	else if(Mode == ACL_MODE)
		 AclRangeDown();
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

// jlm debug
void DebugDrawTewsInfo()
{
	AvRGB Triple;
	GetRGBFromAVPalette(AV_RED,1.0,&Triple);

	int YPos = 10;

	ThreatEntryType *T = &Threats.List[0];

	int Index = Threats.NumThreats;
	while(Index-- > 0)
	{
		if(T->Type == TEWS_PLANE_THREAT)
		{
			sprintf(TmpStr,"Plane  %s  Prog = %d  %s  Category %s", T->TEWSAbbrev, T->Prog, (T->HasLock) ? "LOCK" : "NO LOCK",( (T->Category == TEWS_PLANE_THREAT) ? "PLANE" : ( (T->Category == TEWS_SAM_PROV_THREAT) ? "SAM" : "AAA") ) );
			DrawTextAtLocation(20,YPos,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);
		    YPos += 10;
		}
		else if(T->Type == TEWS_SAM_PROV_THREAT)
		{
			sprintf(TmpStr,"Sam  %s  Prog = %d  %s  Category %s", T->TEWSAbbrev, T->Prog, (T->HasLock) ? "LOCK" : "NO LOCK", ( (T->Category == TEWS_PLANE_THREAT) ? "PLANE" : ( (T->Category == TEWS_SAM_PROV_THREAT) ? "SAM" : "AAA") )  );
			DrawTextAtLocation(20,YPos,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);
			YPos += 10;
		}
		else if(T->Type == TEWS_SAM_WEAP_THREAT)
		{
			sprintf(TmpStr,"Ground  %s  Prog = %d  %s  Category %s", T->TEWSAbbrev, T->Prog, (T->HasLock) ? "LOCK" : "NO LOCK", ( (T->Category == TEWS_PLANE_THREAT) ? "PLANE" : ( (T->Category == TEWS_SAM_PROV_THREAT) ? "SAM" : "AAA") )  );
			DrawTextAtLocation(20,YPos,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);
			YPos += 10;
		}
		else if(T->Type == TEWS_VEH_THREAT)
		{
			sprintf(TmpStr,"Ground  %s  Prog = %d  %s  Category %s", T->TEWSAbbrev, T->Prog, (T->HasLock) ? "LOCK" : "NO LOCK", ( (T->Category == TEWS_PLANE_THREAT) ? "PLANE" : ( (T->Category == TEWS_SAM_PROV_THREAT) ? "SAM" : "AAA") )  );
			DrawTextAtLocation(20,YPos,TmpStr,Triple.Red,Triple.Green,Triple.Blue,0);
			YPos += 10;
		}

	  T++;
  }

}


//*****************************************************************************************************************************************
// AVIONICS TICK  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void MainAvionicsTick()
{
	// this func is for all avionics code that must be runn outside each individual mode
	int debug;
	if(NumCMScheduled > 50)
	  debug = 39;

	if(FlareTotal > 100)
	  debug = 39;

   if(ChaffTotal > 100)
	  debug = 39;

	if(PlayerPlane->AGDesignate.X == -1)
	{
		if(Av.Hsi.SteeringMode == HSI_TGT_MODE)
			Av.Hsi.SteeringMode = HSI_WPT_MODE;
	}

	SetCockpitVars();
	SimJett();
	SimFuelTanks();
	CheckDamageForMasterCaution();
	MissionHelpTick();
	CheckWeaponsForInZone();
	SimAcl();

	// update easy mode target if its a vehicle
	if(Av.AG.PrimeTarget)
	{
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_EASY_RADAR)
		{
			if(Av.AG.PrimeTarget->Type == MOVINGVEHICLE)
			{
			  FPointDouble Loc;
			  GetAGTargetLocation(&Loc);
				PlayerPlane->AGDesignate = Loc;
			}
		}
	}

	// jlm debug
	if(AARadarShowCrossSig)
	{
  		if( (GetCurrentPrimaryTarget() != -1) && ( GetCurrentAARadarMode() == AA_STT_MODE ) )
			{
				PrintCrossSectionInfo();
				DebugCrossSig = DebugPrf = DebugDeltaAlt = DebugRelVel = DebugTargetDist = DebugFinalCrossSig = DebugRangeAdj = DebugVelAdj = DebugAltAdj = DebugAvgAdj = DebugAdditive = -1;
			}
	}

	if(DebugTewsTest)
	    DebugDrawTewsInfo();

	// cut the decoy if player lands with it extended
	if( (PlayerPlane->TowedDecoyPosition > 0.0) && (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) )
		DamageCMDDS();

	// if afterburners are on, cut the decoy if it is out
	if( ((PlayerPlane->LeftThrustPercent > 90) || (PlayerPlane->RightThrustPercent > 90)) && (PlayerPlane->TowedDecoyPosition > 0.0) && (Av.Tews.CM.CurCMDDS != -1)  )
		DamageCMDDS();

}

//*******************************************************************************************************************
int AVFoundOtherLaser(PlaneParams *planepnt, FPointDouble *rposition)
{
	float foffhead = 60.0f;
	float offhead;
	int foundone = 0;
	void *pTarget;
	int iTargetType;
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	FPoint position;
	FPoint	foundpos;
	float dx, dy, dz;
	int cnt;
	FPoint ImpactPoint;
	int isbomb = 0;

	position.SetValues(-1.0f, -1.0f, -1.0f);
	*rposition = position;

	if(iFACHitFlags & FAC_LASER)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
		if(iTargetType == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)pTarget;
			position = vehiclepnt->WorldPosition;
		}
		else if(iTargetType == GROUNDOBJECT)
		{
			walker = (BasicInstance *)pTarget;
			position = walker->Position;
		}
		else
		{
			pTarget = NULL;
			position.X = -1.0f;
			position.Z = -1.0f;
		}

		if ((position.X > -1) && (position.Z > -1))
		{
			dx = position.X - planepnt->WorldPosition.X;
			dy = position.Y - planepnt->WorldPosition.Y;
			dz = position.Z - planepnt->WorldPosition.Z;

			offhead = atan2(-dx, -dz) * 57.2958;

			offhead = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(offhead) - planepnt->Heading);

			if(fabs(offhead) <= foffhead)
			{
				if(QuickDistance(dx, dz) < (BUDDY_LAS_RANGE_NM * NMTOWU))
				{
					foffhead = fabs(offhead);
					*rposition = position;
					foundone = -2;
				}
			}
		}
	}

	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if(cnt == MySlot)
				continue;

			if(iSlotToPlane[cnt] >= 0)
			{
				if(Planes[iSlotToPlane[cnt]].Status & PL_ACTIVE)
				{
					position = NetBuddyLasingData[cnt].laserloc;

					if ((position.X > -1) && (position.Z > -1))
					{
						dx = position.X - planepnt->WorldPosition.X;
						dy = position.Y - planepnt->WorldPosition.Y;
						dz = position.Z - planepnt->WorldPosition.Z;

						offhead = atan2(-dx, -dz) * 57.2958;

						offhead = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(offhead) - planepnt->Heading);

						if(fabs(offhead) <= foffhead)
						{
							if(QuickDistance(dx, dz) < (BUDDY_LAS_RANGE_NM * NMTOWU))
							{
								foffhead = fabs(offhead);
								*rposition = position;
								foundone = cnt + 1;
							}
						}
					}
				}
			}
		}
	}

	if(foundone == -2)
	{
		if(!(iFACHitFlags & FAC_LASER_ON))
		{
	 		AICAddSoundCall(AIC_GenericFACCall, -2, 3000 + ((rand() & 0x3) * 1000), 50, (1 + (rand() & 0x1)));
			iFACHitFlags |= FAC_LASER_ON;
		}
	}

	return(foundone);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float AV_CurrAGMissileToF()
{
	FPointDouble TargetPos;
	float distance;
	int tempval = 0;
	float avgalt = (PlayerPlane->WorldPosition.Y) * WUTOFT;
	float avgvel;
	float Time;
	float maxdist;

	int FlightProfile;
	int GroundTargetType;
	void *Trgt;
	DBWeaponType *pDBWeapon;

	pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];

	GetWeaponData(Av.Weapons.CurAGWeap->WeapPage,&FlightProfile,&Trgt,&GroundTargetType,&TargetPos);

	if(TargetPos.X >= 0)
	{
		distance = (TargetPos - PlayerPlane->WorldPosition) * WUTOFT;
	}
	else
	{
		distance = (float)pDBWeapon->iRange * CalcAltEffectOnMissile(PlayerPlane->WorldPosition.Y) * NMTOFT;
	}

	avgvel = MachAToFtSec((20000.0f), (float)pDBWeapon->fMaxSpeed);  //  WGetLaunchAvgSpeed(P->WeapLoad[Station].WeapIndex, distance, P->V, avgalt, &ftimeimpact);

	maxdist = (float)pDBWeapon->iRange * CalcAltEffectOnMissile(PlayerPlane->WorldPosition.Y) * NMTOFT;

	if(distance > maxdist)
		distance = maxdist;

	Time = distance / avgvel;

	Time += (float)pDBWeapon->iBurnTime * 0.5f;

	if((pDBWeapon->lWeaponID == 34) ||  (pDBWeapon->lWeaponID == 35) || (pDBWeapon->lWeaponID == 94) || (pDBWeapon->lWeaponID == 108) || (pDBWeapon->lWeaponID == 156) || (pDBWeapon->lWeaponID == 157) || (pDBWeapon->lWeaponID == 158) || (pDBWeapon->lWeaponID == 159))
	{
		Time += 1.0f;
	}

	return(Time);
}

BOOL restart_scan(int mode)
{
	BOOL ret_value;

	switch (mode)
	{
		case AA_RWS_MODE:
			ret_value = RWSCheckPrimaryAndSecondaryTargets();
			ClearIFF();
			break;

		case AA_TWS_MODE:
			PrevFramePlanes = CurFramePlanes;
			ret_value = TWSCheckPrimaryAndSecondaryTargets();
			break;

	}
	SetPrimarySecondaryForHud();
	AgeDetectedPlanes();
	ScanStart = FALSE;
	if(++BaseCount >= 2)
	{
		BaseCount = 0;
		Av.AARadar.AOTDamping = 4.0f;
	}
	return ret_value;
}

void SetAOTDamping()
{
	float DistToEnemy;
	int	  Cnt;
	int   PlaneIndex;
	AvObjListEntryType *Plane;
	PlaneParams *Target;
	BOOL InField;
	float yaw,pitch;
	float Base,start_base = 4.0f;
	BOOL IsStandoff;
	BOOL InternalJam;
	FPointDouble PlayerToScan,PlayerToEnemy;
	FPointDouble Intersection;
	FPointDouble STTRadarBeam;

	if(PlayerPlane->Altitude < 1000.0)
		start_base+=1.0f;

	Plane = &Av.Planes.List[0];
	Cnt = 0;

	if (Av.AARadar.CurMode == AA_STT_MODE)
	{
		Av.AARadar.AOTDamping = 4.0f;
		BaseCount = 0;
		if(Primary && (Primary->Id != -1))
		{
			STTRadarBeam.MakeVectorToFrom(Planes[Primary->Id].WorldPosition,PlayerPlane->WorldPosition);
			STTRadarBeam.Normalize();
		}
		else
			STTRadarBeam = PlayerPlane->Orientation.I;

	}

	while(Cnt < Av.Planes.NumPlanes)
	{
		PlaneIndex = Plane->Index;
		Target = (PlaneParams *)Plane->Obj;

		if (!(Target->FlightStatus & PL_STATUS_CRASHED))
		{
			IsStandoff  = (Target->AI.iAIFlags2 & AI_SO_JAMMING);
			InternalJam = AVIsNoiseJamming(Target);  //  ((Target->AI.iAIFlags1 & AIJAMMINGON) && (Target->AI.iAICombatFlags2 & AI_NOISE_JAMMER));

			// if we're in STT mode, and this is the plane we have locked up make sure he has his radar on before we use his jammer
			if ((Av.AARadar.CurMode == AA_STT_MODE) && InternalJam && Primary && (PlaneIndex == Primary->Id) && STTBreakLockTimer.IsActive() && !STTBreakLockTimer.TimeUp(GameLoopInTicks))
				InternalJam = FALSE;

			if (IsStandoff || InternalJam)
			{
				PlayerToEnemy.MakeVectorToFrom(Target->WorldPosition,PlayerPlane->WorldPosition);
				DistToEnemy = PlayerToEnemy.Normalize()*WUTONM;

				Base = start_base;

				if (IsStandoff)
				{
					if (DistToEnemy < 80.0f)
					{
						if (DistToEnemy <= 10.0f)
							Base += 5.0f;
						else
							if (DistToEnemy <= 15.0f)
								Base += 5.0f - (DistToEnemy-10.0f)*(1.0f/5.0f);
							else
								if (DistToEnemy <= 25.0f)
									Base += 4.0f - (DistToEnemy-15.0f)*(1.0f/10.0f);
								else
									if (DistToEnemy <= 40.0f)
										Base += 3.0f - (DistToEnemy-25.0f)*(1.0f/15.0f);
									else
										Base += 2.0f - (DistToEnemy-40.0f)*(2.0f/40.0f);
					}
					else
						Base = -1.0f;
				}
				else
				{
					if (DistToEnemy < 25.0f)
					{
						if (DistToEnemy <= 10.0f)
							Base += 2.0f;
						else
							if (DistToEnemy <= 15.0f)
								Base += 2.0f - (DistToEnemy-10.0f)*(1.0f/5.0f);
							else
								Base += 1.0f - (DistToEnemy-15.0f)*(1.0f/10.0f);
					}
					else
						Base = -1.0f;
				}

				if(Base > Av.AARadar.AOTDamping)
				{
					InField = FALSE;

  					if (Av.AARadar.CurMode == AA_ACQ_MODE)
  					{
						FPointDouble newp;
						FMatrix mat;

						newp = PlayerToEnemy;
						mat = PlayerPlane->Attitude;
						mat.Transpose();
						newp *= mat;

						yaw = atan2(-newp.X,-newp.Z)*RADIANS_TO_DEGREES;
						yaw = RLAngle(yaw - Av.AARadar.SlewLeftRight);
						if (fabs(yaw) <= Av.AARadar.AzimLimit)
						{
							pitch = atan2(newp.Y,sqrt(newp.X*newp.X + newp.Z*newp.Z))*RADIANS_TO_DEGREES;
							pitch = RLAngle(pitch - Av.AARadar.SlewUpDown);
							if (fabs(pitch) <= Av.AARadar.ElevLimit)
								InField = TRUE;
						}
  					}
  					else
  						if (Av.AARadar.CurMode == AA_STT_MODE)
						{
							double dot;

							dot = STTRadarBeam * PlayerToEnemy;

							if (dot > 0.99939) //4 degree cone
								InField = TRUE;
						}
						else
						{
							yaw = atan2(-PlayerToEnemy.X,-PlayerToEnemy.Z)*RADIANS_TO_DEGREES;
							yaw = RLAngle(yaw - (fANGLE_TO_DEGREES(PlayerPlane->Heading) + Av.AARadar.SlewLeftRight));
							if (fabs(yaw) <= Av.AARadar.AzimLimit)
							{
								pitch = atan2(PlayerToEnemy.Y,sqrt(PlayerToEnemy.X*PlayerToEnemy.X + PlayerToEnemy.Z*PlayerToEnemy.Z))*RADIANS_TO_DEGREES;
								pitch = RLAngle(pitch - Av.AARadar.SlewUpDown);
								if (fabs(pitch) <= Av.AARadar.ElevLimit)
									InField = TRUE;
							}
						}

					if (InField && LOS(&(PlayerPlane->WorldPosition),&(Target->WorldPosition), &Intersection, MED_RES_LOS, FALSE))
						Av.AARadar.AOTDamping = Base;
				}
			}
		}

		Cnt++;
		Plane++;
	}
}

void DoAARadarScan()
{
	if ((UFC.MasterMode == AA_MODE) || (UFC.MasterMode == NAV_MODE))
	{
		int NewPrimarySecondary = FALSE;
		int BrokeLock;
		FPointDouble ScanEnd;

		UpdateAATarget(PlayerPlane);

		int JoystickUsed = SimCaptBars();
		if(!JoystickUsed)
			SimCaptBarsForKeyboard();

		SimIff();

		if(Av.AARadar.CurMode != AA_STT_MODE)
			SimNCTR();

		SimBlinks();

		switch (Av.AARadar.CurMode)
		{
			case AA_RWS_MODE:
			case AA_VS_MODE:
			{
				FadeAgePlanes();

				UpdateRadarScan(&ScanEnd);

				if (NewPrimarySecondary = ScanStart)
					NewPrimarySecondary = restart_scan(AA_RWS_MODE);

				SetAOTDamping();

				FilterPlanesFronCone(ScanEnd,(Av.AARadar.CurMode == AA_VS_MODE),FALSE);
				FilterAwacsPlanes(FALSE);
				if (!SimPause)
					RefreshList(&CurFramePlanes,1);
//				SetIffPlanes();
				SetTargetPrimarySecondary();
				SetupPlanesForFiltering();
				FilterForRangeResolution();
				if(NewPrimarySecondary)
				{
					SetTargetPrimarySecondary();
					SetupPlanesForFiltering();
					FilterForRangeResolution();
				}

				break;
			}
			case AA_STT_MODE:
			{
				SetAOTDamping();

//				UpdateRadarScan(&ScanEnd);
				BOOL aot = UpdateSTTScan(&BrokeLock);

				if (BrokeLock)// || !IsTargetInCone(Primary->Id) || !LOS(&(PlayerPlane->WorldPosition),&(Planes[Primary->Id].WorldPosition), &ScanEnd, MED_RES_LOS, FALSE))
					Primary->Reaquire.BrokeLockReaquire = TRUE;

//				aot &= SimSTTJamming();
				SimSTTLockReaquire();

				SimSTTNCTR(aot);

				FPointDouble NullPos;

				NullPos.X = -1; NullPos.Y = -1; NullPos.Z = -1;

				if(Primary->Id != -1)
				{
				 	SetHudTDInfo(Planes[Primary->Id].WorldPosition,0); // 0 = primary, 1 = secondary
					PlayerPlane->AADesignate = (Primary->Id != -1) ? &Planes[Primary->Id] : NULL;
				}
				else
				{
					SetHudTDInfo(NullPos,0); // 0 = primary, 1 = secondary
					PlayerPlane->AADesignate = NULL;
				}

				SetHudTDInfo(NullPos,1); // 0 = primary, 1 = secondary

				break;
			}
			case AA_TWS_MODE:
			{
 				FadeAgePlanes();

				UpdateRadarScan(&ScanEnd);

				if (NewPrimarySecondary = ScanStart)
					NewPrimarySecondary = restart_scan(AA_TWS_MODE);

				SetAOTDamping();

				FilterPlanesFronCone(ScanEnd,FALSE,TRUE);

				if(!Av.AARadar.RaidOn)
				{
					if(Av.AARadar.AutoOn)
						TrackPrimeTargetTWS();
				}
				else
					TrackPrimeTargetRAID();

//				SetIffPlanes();
				FilterAwacsPlanes(FALSE);

				if (!SimPause)
					RefreshList(&CurFramePlanes,1);

				if(!Av.AARadar.AutoOn)
					TrackCaptBarsTWS();

				// FilterChaffAndTald();
				SetTargetPrimarySecondary();

				SetupPlanesForFiltering();

				if (!Av.AARadar.RaidOn)
					FilterForRangeResolution();

				SetPlaneRankings();

				break;
			}

			default:
			{
				UpdateRadarScan(&ScanEnd);

				FilterPlanesFronCone(ScanEnd,FALSE,FALSE);

				if( (CurFramePlanes.NumPlanes > 0) && (AcqTimer.TimeUp(GameLoopInTicks) ) )
				{
					ResetPrimary();
					Primary->Id = CurFramePlanes.Planes[0].PlaneId;
					Primary->IDLevel = AA_UNKNOWN;
					Av.AARadar.CurMode = AA_STT_MODE;
					ResetAARadar(AA_STT_MODE);
					PlayerPlane->AADesignate = (Primary->Id != -1) ? &Planes[Primary->Id] : NULL;

				}

				if (!SimPause)
					RefreshList(&CurFramePlanes,1);

				break;
			}
		}
	}
}

//**************************************************************************************
BOOL AVIsNoiseJamming(PlaneParams *planepnt)
{
	if((MultiPlayer) && ((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
	{
		if(planepnt->AI.iAICombatFlags1 & AI_MULTI_TOWED_DECOY)
			return(TRUE);
	}
	else
	{
		if((planepnt->AI.iAIFlags1 & AIJAMMINGON) && (planepnt->AI.iAICombatFlags2 & AI_NOISE_JAMMER))
			return(TRUE);
	}

	return(FALSE);
}