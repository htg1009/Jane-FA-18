#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"

VKCODE FlirSlewUpPress;
VKCODE FlirSlewDownPress;
VKCODE FlirSlewLeftPress;
VKCODE FlirSlewRightPress;

VKCODE CamSlewUpPress;
VKCODE CamSlewDownPress;
VKCODE CamSlewLeftPress;
VKCODE CamSlewRightPress;

FlirInfoType FlirInfo;

typedef struct
{
  int AquireMode;
  int FOVToUse;
  int TrackOn;
  int TargetSelected;
  FPointDouble Target;
  int LeftBayStat;
  int RightBayStat;
  float SlewLeftRight;
  float SlewUpDown;
  int TargetInKeyhole;
  int   GuideMethod;
  int   LaunchMode;
  int TargetObtained;
  int Magnify;
  int TargetType;
  int TrackPosObtained;
  int MarkerLight;
  int MarkersOn;
  int WithinSlewLimits;
  int DotNum;
  FPointDouble TrackPos;
  BasicInstance *GrndTrgt;
  MovingVehicleParams *VehTrgt;
  PlaneParams *PlaneTrgt;
  FPointDouble LocTrgt;
  int WeapFired;
  int LaunchTimer;
  int CameraShown;
  int AutoSet;
  int CamType;
  int LastStation;
  FPointDouble CamPos;
  FPointDouble AGDesignate;
  WeaponParams *Weap;
  FMatrix Matrix;
  FMatrix RelMatrix;
} CamInfoType;

CamInfoType MavInfo;
CamInfoType GbuInfo;

int TargetReAquireTimer;

typedef struct
{
  int   Slewing;
  int   SlewDirection;
  float SlewOffset;
  int   ButtonTime;
  int   MpdNum;
} CamSlewType;

CamSlewType MavCamSlew;
CamSlewType GbuCamSlew;
CamSlewType FlirCamSlew;

CamSlewType GbuSlew;

FPointDouble FlirOffsetF15;

int LastCamStation;
FPoint CamPosition;

GrBuff *MavOverlay;
GrBuff *FOVOverlay;
GrBuff *ENFOVOverlay;

FMatrix MavCamMatrixF15;

TargetButtonClickInfoType FlirEventInfo;

int FlirFlashStartTime;
int FlirDrawL;

int LaseDesignateStartTime;

int PrevFlirTrackStateF15;  // to know when flir track state changes for showing the "designate" text

// used for flashing the target select cross
typedef struct
{
  int CrossFlashStartTime;
  int DrawCross;
}CrossFlashType;

CrossFlashType MavCross;
CrossFlashType GbuCross;

FPointDouble LaserLocF15;

int GbuInIndirectMode;

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

#define FLIR_LSR_SAFE   0
#define FLIR_LSR_ARMED  1
#define FLIR_LSR_FIRING 2
#define FLIR_LSR_MASKED 3

#define FLIR_AG 0
#define FLIR_AA 1

#define FLIR_WHOT 0
#define FLIR_BHOT 1

//secondpas
void SetSecondPass( bool on, FMatrix mat );


void DrawMavOuterText(int XOfs, int YOfs, int MpdNum, int MpdDetectNum);
void DrawGbuOuterText(int XOfs, int YOfs, int MpdNum,int MpdDetectNum);
void DrawFlirOuterText(int XOfs, int YOfs,int MpdNum,int MpdDetectNum);
void DrawAltAndElev(int MpdNum, int XOfs,int YOfs,CamInfoType *Cam);
void DrawFlirAltAndElev(int MpdNum, int XOfs,int YOfs,int Color);
void DrawCamBuff(int MpdNum, GrBuff *CamBuff,int XOfs,int YOfs,float SlewLeftRight,float SlewUpDown,int UseHiDetail,int DrawAsFlir,int GreenHot,FPointDouble CamLoc,int UseGbu,WeaponParams *Weap,int Magnify,CamInfoType *Cam=NULL);
int  GrabFlirButtonClick(int MpdDetectNum);
int  GrabPlaneInFlirCamF15(FPointDouble *P1, FPointDouble *P2, int *PlaneIndex,float *Dist);
void CalcNewFlirCamHeadingAndPitchF15(float *Heading, float *Pitch, FPointDouble Target);
int  GetFlirCamTargetF15(FPointDouble *P1,FPointDouble *P2,BasicInstance **Target,float *Dist,FPointDouble *Intersect);
int  GetFlirCamVehicleF15(FPointDouble *P1,FPointDouble *P2,MovingVehicleParams **Target,float *Dist);
void ResetGbuF15(void);
void GetLaserTargetF15(FPointDouble *FlirPos, int *TargetObtained,int *TargetType,FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane);
void DrawFlirAttackTimer(void);
void GetCamTargetPosition(CamInfoType *CamInfo, FPointDouble *Target);
void DrawMavGbuRangeMarkers(int MpdNum,CamInfoType *Cam,int XOfs,int YOfs,float MinRange,float MaxRange);
int  WeapIsMav(void);
int UseGbuFlir(int Station);
int GetCloseVehicleOrObjectF15(FPointDouble *Loc,BasicInstance **GroundObject,MovingVehicleParams **Veh,int *Type);


SubObjectHandler *GetSubObjects(PlaneParams *P);

//***********************************************************************************************************************************
// *****

void GetMavGbuTargetPos(int Type, FPointDouble *Pos)
{
   if(Type == MAVERIC_TYPE)
   {
     if(!MavInfo.TargetObtained)
	 {
       Pos->X = -1;
	   Pos->Y = -1;
	   Pos->Z = -1;
	 }
	 else
	 {
       GetCamTargetPosition(&MavInfo,Pos);
	 }
   }
   else
   {
     if(!GbuInfo.TargetObtained)
	 {
       Pos->X = -1;
	   Pos->Y = -1;
	   Pos->Z = -1;
	 }
	 else
	 {
       GetCamTargetPosition(&GbuInfo,Pos);
	 }
   }
}

//***********************************************************************************************************************************
// *****

void FireMav(int *TargetType, FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane, int*InKeyhole)
{
   if(!MavInfo.TargetObtained)
	*TargetType = FLIR_CAM_NO_TARGET;
   else
   {
    if(MavInfo.TargetType == FLIR_CAM_GROUND_TARGET)
      *GroundTarget = MavInfo.GrndTrgt;
    else if(MavInfo.TargetType == FLIR_CAM_VEH_TARGET)
      *Veh = MavInfo.VehTrgt;
    else if(MavInfo.TargetType == FLIR_CAM_PLANE_TARGET)
      *Plane = MavInfo.PlaneTrgt;
    else if(MavInfo.TargetType == FLIR_CAM_LOC_TARGET )
      *TargetLoc = MavInfo.LocTrgt;

	*TargetType = MavInfo.TargetType;
   }


   *InKeyhole = MavInfo.TargetInKeyhole;
   MavInfo.WeapFired = TRUE;

}

//***********************************************************************************************************************************
// *****

void FireGbu(int *TargetType, FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane)
{
  if( (GbuInfo.GuideMethod == GBU_INDIRECT) && ((GbuInfo.LaunchMode == GBU_TRANS) || (GbuInfo.LaunchMode == GBU_NORM)) &&
	   (GbuInfo.TargetObtained) )
     *TargetType = FLIR_CAM_NO_TARGET;
  else if(!GbuInfo.TargetObtained)
	*TargetType = FLIR_CAM_NO_TARGET;
  else
  {
    if(GbuInfo.TargetType == FLIR_CAM_GROUND_TARGET)
  	  *GroundTarget = GbuInfo.GrndTrgt;
    else if(GbuInfo.TargetType == FLIR_CAM_VEH_TARGET)
      *Veh = GbuInfo.VehTrgt;
    else if(GbuInfo.TargetType == FLIR_CAM_PLANE_TARGET)
      *Plane = GbuInfo.PlaneTrgt;
	else if(GbuInfo.TargetType == FLIR_CAM_LOC_TARGET )
      *TargetLoc = GbuInfo.LocTrgt;

	 *TargetType = GbuInfo.TargetType;
  }


  if(*TargetType == FLIR_CAM_PLANE_TARGET)
  {
    *TargetType = FLIR_CAM_LOC_TARGET;
	*TargetLoc  = GbuInfo.PlaneTrgt->WorldPosition;
  }

  GbuInfo.WeapFired = TRUE;

  if(GbuInfo.GuideMethod == GBU_DIRECT)
	 ResetGbuF15();
  else // indirect mode
  {
    if(GbuInfo.LastStation != -1)
   	  GbuInfo.CamType = (UseGbuFlir(GbuInfo.LastStation)) ? 1 : 0;
	else
	  GbuInfo.CamType = 0;  // should never get here
  }

}

//***********************************************************************************************************************************
// *****

void CleanupMavGbu()
{
  GrFreeGrBuff(MavOverlay);
  GrFreeGrBuff(FOVOverlay);
  GrFreeGrBuff(ENFOVOverlay);
  GrFreeGrBuff(GbuMonoBuff);
  GrFreeGrBuff(MavMonoBuff);
}

//***********************************************************************************************************************************
// *****
void CleanupFlir()
{
  GrFreeGrBuff(FlirMonoBuff);
}

//***********************************************************************************************************************************
// *****

void SetupCamSlew(int Direction, int MpdNum, int Type)
{
  if(Type == MAVERIC_TYPE)
  {
    MavCamSlew.Slewing = TRUE;
    MavCamSlew.SlewDirection = Direction;
    MavCamSlew.SlewOffset = 0.0;
    MavCamSlew.ButtonTime = GetTickCount();
    MavCamSlew.MpdNum = MpdNum;
  }
  else if(Type == GBU15_TYPE)
  {
    GbuCamSlew.Slewing = TRUE;
    GbuCamSlew.SlewDirection = Direction;
    GbuCamSlew.SlewOffset = 0.0;
    GbuCamSlew.ButtonTime = GetTickCount();
    GbuCamSlew.MpdNum = MpdNum;
  }
  else // must be a flir type
  {
    FlirCamSlew.Slewing = TRUE;
    FlirCamSlew.SlewDirection = Direction;
    FlirCamSlew.SlewOffset = 0.0;
    FlirCamSlew.ButtonTime = GetTickCount();
    FlirCamSlew.MpdNum = MpdNum;
  }

}

//***********************************************************************************************************************************
// *****

void SetupGbuSlew(int Direction, int MpdNum)
{
    GbuSlew.Slewing = TRUE;
    GbuSlew.SlewDirection = Direction;
    GbuSlew.SlewOffset = 0.0;
    GbuSlew.ButtonTime = GetTickCount();
    GbuSlew.MpdNum = MpdNum;
}

//***********************************************************************************************************************************
// *****

void ResetMaverickF15()
{
  MavInfo.AquireMode = MSL_AUTO;
  MavInfo.TrackOn = FALSE;
  MavInfo.LeftBayStat = MSL_NONE;
  MavInfo.RightBayStat =  MSL_NONE;
  MavInfo.SlewLeftRight = 0.0;
  MavInfo.SlewUpDown = -12.0;
  MavInfo.TargetSelected = FALSE;
  MavInfo.TargetObtained = FALSE;
  MavInfo.WeapFired = FALSE;
  MavInfo.TargetInKeyhole = TRUE;
  MavInfo.TrackPosObtained = FALSE;
  MavInfo.AGDesignate.X = -1;
  MavInfo.AGDesignate.Y = -1;
  MavInfo.AGDesignate.Z = -1;
  MavInfo.MarkerLight = TRUE;
  MavInfo.WithinSlewLimits = FALSE;
  MavInfo.AutoSet = FALSE;
  MavInfo.MarkersOn = FALSE;
  MavInfo.CamType = -1;
  MavInfo.DotNum = 0;

  MavInfo.Magnify = (float)1.67*(float)DEGREE;
  MavInfo.FOVToUse = NFOV;

  MavCross.CrossFlashStartTime = -1;
  MavCross.DrawCross = TRUE;

}

//***********************************************************************************************************************************
// *****

void ResetGbuF15()
{
  GbuInfo.AquireMode = MSL_AUTO;
  GbuInfo.TrackOn = FALSE;
  GbuInfo.LeftBayStat = MSL_NONE;
  GbuInfo.RightBayStat =  MSL_NONE;
  GbuInfo.SlewLeftRight = 0.0;
  GbuInfo.SlewUpDown = -12.0;
  GbuInfo.TargetSelected = FALSE;
  GbuInfo.GuideMethod = GBU_DIRECT;
  GbuInfo.LaunchMode = GBU_NORM;
  GbuInfo.TargetSelected = FALSE;
  GbuInfo.TargetObtained = FALSE;
  GbuInfo.WeapFired = FALSE;
  GbuInfo.LaunchTimer = -1;
  GbuInfo.CameraShown = TRUE;
  GbuInfo.Weap = NULL;
  LaseDesignateStartTime = -1;
  GbuInfo.TrackPosObtained = FALSE;
  GbuInfo.AGDesignate.X = -1;
  GbuInfo.AGDesignate.Y = -1;
  GbuInfo.AGDesignate.Z = -1;
  GbuInfo.MarkerLight = TRUE;
  GbuInfo.WithinSlewLimits = FALSE;
  GbuInfo.Magnify =  (float)5.85*DEGREE;
  GbuInfo.FOVToUse = WFOV;
  GbuInfo.AutoSet = FALSE;
  GbuInfo.MarkersOn = FALSE;
  GbuInfo.CamType = -1;
  GbuInfo.LastStation = -1;
  GbuInIndirectMode = FALSE;
  GbuInfo.DotNum = 0;

  LastCamStation = -1;

  GbuCross.CrossFlashStartTime = -1;
  GbuCross.DrawCross = TRUE;

}

//***********************************************************************************************************************************
// *****

void InitFlirF15()
{
  FlirInfo.SlewLeftRight = 0.0;
  FlirInfo.SlewUpDown = 0.0;
  FlirInfo.ModeToSelect = FLIR_AG;
  FlirInfo.TrackOn = FALSE;
  FlirInfo.DeclutterOn = FALSE;
  FlirInfo.LaserState = FLIR_LSR_SAFE;
  FlirInfo.TargetSelected = FALSE;
  FlirInfo.ImageMode = FLIR_WHOT;
  FlirInfo.TargetObtained = FALSE;
  FlirInfo.MaskOn = FALSE;
  FlirInfo.CdesOn = FALSE;
  FlirInfo.ShortRangeOn = FALSE;
  FlirInfo.AquireMode = MSL_AUTO;

  FlirInfo.Magnify = (float)5.85*(float)DEGREE;
  FlirInfo.FOVToUse = WFOV;

  FlirOffsetF15.X = -4.5*FTTOWU;
  FlirOffsetF15.Y = -3.5*FTTOWU;
  FlirOffsetF15.Z = -7.0*FTTOWU;

  FlirFlashStartTime = -1;
  FlirDrawL = TRUE;

  FlirInfo.AGDesignate.X = -1;
  FlirInfo.AGDesignate.Y = -1;
  FlirInfo.AGDesignate.Z = -1;

  FlirInfo.TrackPos.X = 0;
  FlirInfo.TrackPos.Y = 0;
  FlirInfo.TrackPos.Z = 0;

  FlirInfo.TimpactTimer = -1;

  TargetReAquireTimer = -1;


  FlirInfo.TrackPosObtained = FALSE;

  PrevFlirTrackStateF15 = FALSE;

  FlirMonoBuff = GrAllocGrBuff(116,111, GR_INDEXED | GR_8BIT);

}

//***********************************************************************************************************************************
// *****

void InitMavGbu()
{

  GrBuff *CutBuff;

  ResetMaverickF15();
  ResetGbuF15();

  // turn off all slewing for all cameras
  MavCamSlew.Slewing  = FALSE;
  GbuCamSlew.Slewing  = FALSE;
  FlirCamSlew.Slewing = FALSE;
  GbuSlew.Slewing     = FALSE;

  PauseForDiskHit();

  MavMonoBuff = GrAllocGrBuff(116,111, GR_INDEXED | GR_8BIT);
  GbuMonoBuff = GrAllocGrBuff(116,111, GR_INDEXED | GR_8BIT);

  CutBuff = GrAllocGrBuff(640, 480, GR_INDEXED | GR_8BIT);
  LoadPCXGrBuff(CutBuff, RegPath("Cockpits", "mpdspr.pcx"), 640, 480, 0, NULL);

  MavOverlay = GrAllocGrBuff(362 - 228 + 1 + 2,185 - 50 + 1 + 2, GR_INDEXED | GR_8BIT);
  GrCopyRectNoClip(MavOverlay, 0,0, CutBuff,228,50,362 - 228 + 1 + 2,185 - 50 + 1 + 2);

  FOVOverlay = GrAllocGrBuff(480 - 411 + 1,156 - 76 + 1, GR_INDEXED | GR_8BIT);
  GrCopyRectNoClip(FOVOverlay, 0,0, CutBuff,411, 76,480 - 411 + 1,156 - 76 + 1);

  ENFOVOverlay = GrAllocGrBuff(543 - 506 + 1,136 - 96 + 1, GR_INDEXED | GR_8BIT);
  GrCopyRectNoClip(ENFOVOverlay, 0,0, CutBuff,506,96,543 - 506 + 1,136 - 96 + 1);

  GrFreeGrBuff(CutBuff);

  UnPauseFromDiskHit();

}

//***********************************************************************************************************************************
// *****

#define CAM_PROJ_DIST (40.0*NMTOWU)

// JLM remove
float JLMDegree = DEGREE;

void CalcCameraViewVectorF15(float LeftRight, float UpDown, FPointDouble *Origin, FPointDouble *ViewVec,float Heading)
{
  float Yaw,Pitch;
  FPointDouble Local;

  Local.X = 0;
  Local.Y = 0;
  Local.Z = -CAM_PROJ_DIST;

  Pitch = NormDegree(UpDown)*182.0444444f;
  MavCamMatrixF15.SetHPR(0,Pitch,0);
  Local.RotateInto(Local,MavCamMatrixF15);

  Yaw = NormDegree((Heading/(float)182.0444444f) + LeftRight)*(float)182.0444444f;
  MavCamMatrixF15.SetHPR(Yaw,0,0);
  Local.RotateInto(Local,MavCamMatrixF15);

  ViewVec->X =  Origin->X + Local.X;
  ViewVec->Z =  Origin->Z + Local.Z;
  ViewVec->Y =  Origin->Y + Local.Y;

}

//***********************************************************************************************************************************
// *****

void DrawPointer(int MpdNum, int XOfs, int YOfs, CamInfoType *Cam, int Type)
{
  int ScrX,ScrY;
  int AdjSlewUpDown;
  int PointerColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD]; // JLM this may change

  // compensate for centerline being -12 degrees
  AdjSlewUpDown = Cam->SlewUpDown + 12.0;

  ScrX = XOfs + -Cam->SlewLeftRight*(PIX_PER_SLEW_LR);
  ScrY = YOfs + -AdjSlewUpDown*(PIX_PER_SLEW_UD);

  if(Type == MAVERIC_TYPE)
  {
    GrDrawLine(GrBuffFor3D,ScrX,ScrY - 6,ScrX,ScrY+6,PointerColor);
    GrDrawLine(GrBuffFor3D,ScrX - 6,ScrY,ScrX+6,ScrY,PointerColor);
  }
  else
  {
    DrawRadarSymbol(ScrX,ScrY,RDR_CLOSE_BOX,PointerColor,GrBuffFor3D);
  }

}

//***********************************************************************************************************************************
// *****

#define FLIR_CAM_PIX_RADIUS 45

void DrawFlirPointer(int MpdNum, int XOfs, int YOfs,int Color)
{
  float NewX, NewZ;
  int NumPixels;

  NumPixels = FLIR_CAM_PIX_RADIUS*(1.0 - (fabs(FlirInfo.SlewUpDown)/90.0));

  ProjectPoint(XOfs, YOfs, FlirInfo.SlewLeftRight, NumPixels, &NewX, &NewZ);

  DrawRadarSymbol(NewX,NewZ,FLIR_DIR_POINTER,Color,GrBuffFor3D);

}

//***********************************************************************************************************************************
// *****

int IsTargetInKeyHoleF15(int XOfs, int YOfs, CamInfoType *Cam)
{
  float Dist;
  int ScrX,ScrY;

  ScrX = XOfs + -Cam->SlewLeftRight*(PIX_PER_SLEW_LR);  // JLM need to use target
  ScrY = YOfs + -Cam->SlewUpDown*(PIX_PER_SLEW_UD);

  if( (ScrX > XOfs-20) && (ScrX < XOfs+20) )
	if( (ScrY > YOfs+25) && (ScrY < YOfs+35) )
	  return(TRUE);

  Dist = sqrt((float)(XOfs-ScrX)*(XOfs-ScrX) + (float)(YOfs-ScrY)*(YOfs-ScrY));
  if(Dist < 35)
   return(TRUE);

  return(FALSE);

}

//***********************************************************************************************************************************
// *****

int IsCamWithinSlewBoundsF15(float LeftRight, float UpDown)
{
    if( (LeftRight <= 40.0) && (LeftRight >= -40.0) )
	   if( (UpDown <= 28.0) && (UpDown >= -52.0) )
	      return(TRUE);

	return(FALSE);

}

//***********************************************************************************************************************************
// *****

int IsFlirWithinSlewBoundsF15(float LeftRight, float UpDown)
{

    if( ((LeftRight <= 90) && (LeftRight >= 0)) || ((LeftRight >= -90) && (LeftRight <= 0)) )
	{
	  if( (UpDown >= -90) && (UpDown <= 0) )
	    return(TRUE);
	}
	else if(  ( (LeftRight > 90) && (LeftRight <= 150) ) || ( (LeftRight < -90) && (LeftRight >= -150) )  )
	{
      if( (UpDown >= -90) && (UpDown <= 0) )
	      return(TRUE);
	}
	else if( (LeftRight > 150) || (LeftRight < -150) )
	{
      if( (UpDown >= -90) && (UpDown <= -30) )
	      return(TRUE);
	}

	return(FALSE);
}


//***********************************************************************************************************************************
// *****

void ProcessFlirCamSlewF15(int MpdDetectNum, CamSlewType *CamSlew, int FOVToUse)
{
  if(GetVkStatus(FlirSlewUpPress))
  {
    if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) )
	  SetupCamSlew(SLEW_UP, MpdDetectNum, FLIR_TYPE);
  }

  if(GetVkStatus(FlirSlewDownPress))
  {
    if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) )
	  SetupCamSlew(SLEW_DOWN, MpdDetectNum, FLIR_TYPE);
  }

  if(GetVkStatus(FlirSlewLeftPress))
  {
     if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) )
	   SetupCamSlew(SLEW_LEFT, MpdDetectNum, FLIR_TYPE);
  }

  if(GetVkStatus(FlirSlewRightPress))
  {
     if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) )
	   SetupCamSlew(SLEW_RIGHT, MpdDetectNum, FLIR_TYPE);
  }

  if(CamSlew->Slewing)
  {
  	if(CamSlew->MpdNum == MpdDetectNum)
	{
	  float SlewAmount;

	  if(FOVToUse == WFOV)
		  SlewAmount = 4.0;
	  else if(FOVToUse == NFOV)
          SlewAmount = 0.75;
	  else if(FOVToUse == ENFOV)
        SlewAmount = 0.15;

	  CamSlew->SlewOffset = (((float)(GetTickCount() - CamSlew->ButtonTime))/1000.0)*SlewAmount;

	  if(CamSlew->SlewOffset > 20.0) CamSlew->SlewOffset = 20.0;

	  // JLM Easy Mode Flir should let you go 180 and -90 degrees-- need to implement here

	  float SlewUpDown    = FlirInfo.SlewUpDown;
	  float SlewLeftRight = FlirInfo.SlewLeftRight;

      switch(CamSlew->SlewDirection)
 	  {
	    case SLEW_UP   : SlewUpDown    += CamSlew->SlewOffset;
			             if(SlewUpDown > 0.0) SlewUpDown    = 0.0;
  			             break;
        case SLEW_DOWN : SlewUpDown    -= CamSlew->SlewOffset;
                         if(SlewUpDown < -90.0)       SlewUpDown    = -90.0;
			             break;
		case SLEW_LEFT : SlewLeftRight += CamSlew->SlewOffset;
			             if(SlewLeftRight > 180.0)
							 SlewLeftRight = -180.0 + (SlewLeftRight - 180.0);
 			             break;
		case SLEW_RIGHT: SlewLeftRight -= CamSlew->SlewOffset;
                         if(SlewLeftRight < -180.0)
							 SlewLeftRight = 180.0 + (SlewLeftRight + 180.0);
						 break;
	  }

	  if(IsFlirWithinSlewBoundsF15(SlewLeftRight,SlewUpDown))
	  {
        FlirInfo.SlewUpDown    = SlewUpDown;
        FlirInfo.SlewLeftRight = SlewLeftRight;
	  }

	  int KeypressSlew = ( (GetVkStatus(FlirSlewUpPress)) || (GetVkStatus(FlirSlewDownPress)) ||
		                   (GetVkStatus(FlirSlewLeftPress)) || (GetVkStatus(FlirSlewRightPress)) );

	  if( (GetMouseLB() == 0) && (!KeypressSlew) )
	    CamSlew->Slewing = FALSE;
	}

  }

}


int ShowVectorDebug = FALSE;

//***********************************************************************************************************************************
// *****

void ProcessCamSlew(int MpdDetectNum, CamSlewType *CamSlew, CamInfoType *Cam)
{
  int Type = WeapIsMav() ? MAVERIC_TYPE : GBU15_TYPE;

  if(GetVkStatus(CamSlewUpPress))
  {
    if( (!CamSlew->Slewing) && (!Cam->TrackOn) )
	  SetupCamSlew(SLEW_UP, MpdDetectNum, Type);
  }

  if(GetVkStatus(CamSlewDownPress))
  {
    if( (!CamSlew->Slewing) && (!Cam->TrackOn) )
	  SetupCamSlew(SLEW_DOWN, MpdDetectNum, Type);
  }

  if(GetVkStatus(CamSlewLeftPress))
  {
     if( (!CamSlew->Slewing) && (!Cam->TrackOn) )
	   SetupCamSlew(SLEW_LEFT, MpdDetectNum, Type);
  }

  if(GetVkStatus(CamSlewRightPress))
  {
     if( (!CamSlew->Slewing) && (!Cam->TrackOn) )
	   SetupCamSlew(SLEW_RIGHT, MpdDetectNum, Type);
  }

  if(CamSlew->Slewing)
  {

	if(CamSlew->MpdNum == MpdDetectNum)
	{
	  float SlewAmount;

	  if(Cam->FOVToUse == WFOV)
		  SlewAmount = 4.0;
	  else if(Cam->FOVToUse == NFOV)
          SlewAmount = 0.75;
	  else if(Cam->FOVToUse == ENFOV)
        SlewAmount = 0.15;

	  CamSlew->SlewOffset = (((float)(GetTickCount() - CamSlew->ButtonTime))/1000.0)*SlewAmount;

      switch(CamSlew->SlewDirection)
 	  {
	    case SLEW_UP   : Cam->SlewUpDown    += CamSlew->SlewOffset;
			             if(Cam->SlewUpDown > 28.0) Cam->SlewUpDown = 28.0;
			             break;
        case SLEW_DOWN : Cam->SlewUpDown    -= CamSlew->SlewOffset;
				         if(Cam->SlewUpDown < -52.0) Cam->SlewUpDown = -52.0;
			             break;
		case SLEW_LEFT : Cam->SlewLeftRight += CamSlew->SlewOffset;
					     if(Cam->SlewLeftRight > 40.0) Cam->SlewLeftRight = 40.0;
			             break;
		case SLEW_RIGHT: Cam->SlewLeftRight -= CamSlew->SlewOffset;
						 if(Cam->SlewLeftRight < -40.0) Cam->SlewLeftRight = -40.0;
			             break;
	  }

	  int KeypressSlew = ( (GetVkStatus(CamSlewUpPress)) || (GetVkStatus(CamSlewDownPress)) ||
		                   (GetVkStatus(CamSlewLeftPress)) || (GetVkStatus(CamSlewRightPress)) );

	  if( (GetMouseLB() == 0) && (!KeypressSlew) )
	    CamSlew->Slewing = FALSE;
	}

  }

}

//***********************************************************************************************************************************
// *****

void ProcessGbuSlew(int MpdNum, CamSlewType *CamSlew, CamInfoType *Cam)
{
  if(CamSlew->Slewing)
  {

	if(CamSlew->MpdNum == MpdNum)
	{
	  float SlewAmount;

	  if(Cam->FOVToUse == WFOV)
		  SlewAmount = 5.0;
	  else if(Cam->FOVToUse == NFOV)
          SlewAmount = 1.5;
	  else if(Cam->FOVToUse == ENFOV)
        SlewAmount = 1.5/2.0;

	  CamSlew->SlewOffset = (((float)(GetTickCount() - CamSlew->ButtonTime))/1000.0)*5.0;

      switch(CamSlew->SlewDirection)
 	  {
	    case SLEW_UP   : lBombFlags |= WSO_BOMB_LONGER;
			             break;
        case SLEW_DOWN : lBombFlags |= WSO_BOMB_SHORTER;
			             break;
		case SLEW_LEFT : lBombFlags |= WSO_TURN_BOMB_L;
			             break;
		case SLEW_RIGHT: lBombFlags |= WSO_TURN_BOMB_R;
			             break;
	  }

	  if(GetMouseLB() == 0)
	    CamSlew->Slewing = FALSE;
    }
 }

}


//***********************************************************************************************************************************
// *****

int UseGbuFlir(int Station)
{
  int RetVal = FALSE;

  if( (PlayerPlane->WeapLoad[Station].WeapId == 26) || (PlayerPlane->WeapLoad[Station].WeapId == 28) )
    return(TRUE);
  else
	return(FALSE);
 }

//***********************************************************************************************************************************
// *****

void ProcessMissleStatus(int *MissleOnBoard, int Type, CamInfoType *Cam,DBWeaponType **pDBWeapon, FPointDouble *CamPos,int *UseFlirType)
{
  int LeftSideHasMsl;
  int RightSideHasMsl;

  LeftSideHasMsl  = IsAGMissleAvailableOnGivenSide(Type,LEFT);
  RightSideHasMsl = IsAGMissleAvailableOnGivenSide(Type,RIGHT);

  // says whether or not there is at least one missle present and selected
  *MissleOnBoard = (LeftSideHasMsl || RightSideHasMsl);

  // no weapon fire, process the status
  if(LeftSideHasMsl && RightSideHasMsl)
  {
    Cam->LeftBayStat  = MSL_RDY;
    Cam->RightBayStat = MSL_STBY;
  }
  else if(LeftSideHasMsl && !RightSideHasMsl)
  {
    Cam->LeftBayStat  = MSL_RDY;
    Cam->RightBayStat = MSL_NONE;
  }
  else if(!LeftSideHasMsl && RightSideHasMsl)
  {
    Cam->LeftBayStat  = MSL_NONE;
    Cam->RightBayStat = MSL_RDY;
  }

  int Index = -1;
  int Station;

  if(LeftSideHasMsl && ( (IsWeaponGBUType(AG_LEFT)) || (IsWeaponMaverickType(AG_LEFT) ) ) )
  {
      Index = PlayerPlane->WeapLoad[AG_LEFT].WeapIndex;
	  Station = AG_LEFT;
  }
  else if(LeftSideHasMsl && ( (IsWeaponGBUType(FRONT_L)) || (IsWeaponMaverickType(FRONT_L)) ) )
  {
      Index = PlayerPlane->WeapLoad[FRONT_L].WeapIndex;
	  Station = FRONT_L;
  }
  else if(RightSideHasMsl && ( (IsWeaponGBUType(AG_RIGHT)) || (IsWeaponMaverickType(AG_RIGHT)) ) )
  {
      Index = PlayerPlane->WeapLoad[AG_RIGHT].WeapIndex;
      Station = AG_RIGHT;
  }
  else if(RightSideHasMsl && ((IsWeaponGBUType(FRONT_R)) || (IsWeaponMaverickType(FRONT_R)) ) )
  {
      Index = PlayerPlane->WeapLoad[FRONT_R].WeapIndex;
      Station = FRONT_R;
  }

  if(Index != -1)
  {
	 *pDBWeapon = &pDBWeaponList[Index];

	 // for showing proper camera, tv vs flir
     *UseFlirType = UseGbuFlir(Station);
	 GbuInfo.LastStation = Station;

	 int dot = GetHardPoint(PlayerPlane,Station,PlayerPlane->WeapLoad[Station].Count);
	 Cam->DotNum = dot;

	 if(LastCamStation != dot)
     {
	   FMatrix TempMat;

	   TempMat.Identity();

       CamPosition.SetValues(0.0,0.0,0.0);
       FindDot(PlayerPlane->Type->Model, dot,GetSubObjects(PlayerPlane),CamPosition,TempMat);
       CamPosition.Z -= 2.5 FEET;
	   LastCamStation = dot;
     }
   	 Cam->CamPos.RotateInto(CamPosition,PlayerPlane->Attitude);
	 Cam->CamPos += PlayerPlane->WorldPosition;
	 *CamPos = Cam->CamPos;
  }
  else
	*pDBWeapon = NULL;


}

//***********************************************************************************************************************************
// *****

int IsAGMissleSelected(int *Type,int *SideOnLeft)
{
  int LeftSideHasMav  = IsAGMissleAvailableOnGivenSide(MAVERIC_TYPE,LEFT);
  int RightSideHasMav = IsAGMissleAvailableOnGivenSide(MAVERIC_TYPE,RIGHT);

  int LeftSideHasGbu = IsAGMissleAvailableOnGivenSide(GBU15_TYPE,LEFT);
  int RightSideHasGbu = IsAGMissleAvailableOnGivenSide(GBU15_TYPE,RIGHT);

  if(LeftSideHasMav || RightSideHasMav)
  {
    *Type = MAVERIC_TYPE;
	*SideOnLeft = LeftSideHasMav;
	return(TRUE);
  }
  else if(LeftSideHasGbu || RightSideHasGbu)
  {
    *Type = GBU15_TYPE;
	*SideOnLeft = LeftSideHasGbu;
	return(TRUE);
  }

  return(FALSE);

}

//***********************************************************************************************************************************
// *****

void ProcessPointerF15(int MpdNum, int XOfs, int YOfs, CrossFlashType *CrossFlash, CamSlewType *CamSlew,
					CamInfoType *Cam, int Type)
{

	if(Type == MAVERIC_TYPE)
	  Cam->TargetInKeyhole = IsTargetInKeyHoleF15(XOfs,YOfs,Cam);
	else
	  Cam->TargetInKeyhole = TRUE; // no keyhole for gbu15

	if(!Cam->TargetInKeyhole)
	{
      if(CrossFlash->CrossFlashStartTime == -1)
	    CrossFlash->CrossFlashStartTime = GetTickCount();
	}

	if(CrossFlash->CrossFlashStartTime != -1)
	{
      if(GetTickCount() - CrossFlash->CrossFlashStartTime > 500)
	  {
        CrossFlash->CrossFlashStartTime = GetTickCount();
        CrossFlash->DrawCross = !CrossFlash->DrawCross;
	  }

	  if(Cam->TargetInKeyhole)
	  {
		 CrossFlash->CrossFlashStartTime = -1;
         CrossFlash->DrawCross = TRUE;
	  }
	}

	if(CrossFlash->DrawCross)
	  DrawPointer(MpdNum,XOfs,YOfs, Cam, Type);
}

//***********************************************************************************************************************************
// *****

void DrawElecCageIndicator(int XOfs, int YOfs)
{

 if( (fabs(GbuInfo.SlewLeftRight) < 3) && (fabs(GbuInfo.SlewUpDown) < 3) )
   GrFillRectNoClip(GrBuffFor3D,XOfs-65, YOfs-30, 13, 7, 61);
}

//***********************************************************************************************************************************
// *****

void CalcNewFlirCamHeadingAndPitchF15(FPointDouble Pos,float *Heading, float *Pitch, FPointDouble Target,WeaponParams *Weap)
{
  float DeltaAzim,DeltaElev,HeadingToUse;

  if(Weap != NULL)
    HeadingToUse = Weap->Heading;
  else
    HeadingToUse = PlayerPlane->Heading;

  DeltaAzim = ComputeHeadingToPoint(Pos,HeadingToUse,Target,1);
  DeltaElev = -ComputePitchFromPointToPoint(Pos,Target);

  *Heading = DeltaAzim;
  *Pitch   = DeltaElev;
}

//***********************************************************************************************************************************
// *****

void GetCamTargetPosition(CamInfoType *CamInfo, FPointDouble *Target)
{
  if(CamInfo->TargetType == FLIR_CAM_GROUND_TARGET)
  	*Target = CamInfo->GrndTrgt->Position;
  else if(CamInfo->TargetType == FLIR_CAM_VEH_TARGET)
    *Target = CamInfo->VehTrgt->WorldPosition;
  else if(CamInfo->TargetType == FLIR_CAM_PLANE_TARGET)
    *Target = CamInfo->PlaneTrgt->WorldPosition;
  else
    *Target = CamInfo->LocTrgt;
}

//***********************************************************************************************************************************
// *****

void GetFlirTargetPositionF15(int *TargetObtained,FPointDouble *Target)
{
  *TargetObtained = FlirInfo.TargetObtained;

  if(FlirInfo.TargetObtained)
  {
    if(FlirInfo.TargetType == FLIR_CAM_GROUND_TARGET)
  	  *Target = FlirInfo.LocTrgt; //FlirInfo.GrndTrgt->Position;
    else if(FlirInfo.TargetType == FLIR_CAM_VEH_TARGET)
      *Target = FlirInfo.VehTrgt->WorldPosition;
    else if(FlirInfo.TargetType == FLIR_CAM_PLANE_TARGET)
      *Target = FlirInfo.PlaneTrgt->WorldPosition;
    else
      *Target = FlirInfo.LocTrgt;
  }
}

//***********************************************************************************************************************************
// *****

void GetLaserTargetPositionF15(FPointDouble *Target,int TargetType,BasicInstance *GroundTarget,MovingVehicleParams *Veh,PlaneParams *Plane,FPointDouble TargetLoc)
{
  if(TargetType == FLIR_CAM_GROUND_TARGET)
  	*Target = GroundTarget->Position;
  else if(TargetType == FLIR_CAM_VEH_TARGET)
    *Target = Veh->WorldPosition;
  else if(TargetType == FLIR_CAM_PLANE_TARGET)
    *Target = Plane->WorldPosition;
  else
    *Target = TargetLoc;
}

//***********************************************************************************************************************************
// *****

void PointFlirToCurrentAGTargetF15()
{
  if(FlirInfo.AquireMode == MSL_MANUAL) return;
  if(PlayerPlane->AGDesignate.X <= 0) return;
  if(FlirInfo.LaserState == FLIR_LSR_FIRING) return;

  if( (FlirInfo.AGDesignate.X != PlayerPlane->AGDesignate.X) ||
      ( (FlirInfo.AGDesignate.Y != PlayerPlane->AGDesignate.Y) && (fabs(PlayerPlane->AGDesignate.Y- FlirInfo.AGDesignate.Y) > 1.0)) ||
      (FlirInfo.AGDesignate.Z != PlayerPlane->AGDesignate.Z) || FlirInfo.AutoSet)
  {
      float Heading,Pitch;
	  FPointDouble FlirPos;

      FlirPos = FlirOffsetF15;
      FlirPos.RotateInto(FlirPos,PlayerPlane->Attitude);
      FlirPos += PlayerPlane->WorldPosition;

	  FlirInfo.AutoSet = FALSE;

      CalcNewFlirCamHeadingAndPitchF15(FlirPos, &Heading, &Pitch, PlayerPlane->AGDesignate,NULL);

      float LeftRight = Heading;
      float UpDown    = Pitch;

      if(IsFlirWithinSlewBoundsF15(LeftRight,UpDown))
	  {
        FlirInfo.SlewLeftRight = LeftRight;
        FlirInfo.SlewUpDown    = UpDown;

        FlirInfo.AGDesignate.X = PlayerPlane->AGDesignate.X;
        FlirInfo.AGDesignate.Y = PlayerPlane->AGDesignate.Y;
        FlirInfo.AGDesignate.Z = PlayerPlane->AGDesignate.Z;

		FlirInfo.TrackOn = TRUE;
        FlirInfo.TrackPosObtained = TRUE;
        FlirInfo.TrackPos = PlayerPlane->AGDesignate;
		FlirInfo.TargetObtained = TRUE;
		FlirInfo.TargetType = FLIR_CAM_LOC_TARGET;
        FlirInfo.LocTrgt = PlayerPlane->AGDesignate;
		PrevFlirTrackStateF15 = !FlirInfo.TrackOn;
	  }
   }
}


//***********************************************************************************************************************************
// *****

void PointCamToCurrentAGTarget(CamInfoType *Cam)
{
  if(Cam->AquireMode == MSL_MANUAL) return;
  if(PlayerPlane->AGDesignate.X <= 0) return;

  int ShouldProcess = FALSE;

  if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
  {
	if(TargetReAquireTimer == -1)
	 TargetReAquireTimer = GameLoopInTicks;
	else
	{
      if(GameLoopInTicks - TargetReAquireTimer > 1000)
        ShouldProcess = TRUE;
	}
  }

  if( (Cam->AGDesignate.X != PlayerPlane->AGDesignate.X) ||
      (Cam->AGDesignate.Y != PlayerPlane->AGDesignate.Y) ||
      (Cam->AGDesignate.Z != PlayerPlane->AGDesignate.Z) || ShouldProcess || Cam->AutoSet)
  {
      float Heading,Pitch;

	  if(Cam->Weap != NULL)
	  {
        CalcNewFlirCamHeadingAndPitchF15(Cam->CamPos, &Heading, &Pitch, PlayerPlane->AGDesignate,Cam->Weap);
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

      if(IsCamWithinSlewBoundsF15(LeftRight,UpDown))
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

extern Debug2;

//***********************************************************************************************************************************
// *****

void DoMaverick(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
  int Pressed;
  int ButtonNum;
  int MissleOnBoard;
  DBWeaponType *pDBWeapon;
  FPointDouble MavPos;
  int OverlayColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD]; // JLM this may change
  int TextColor    = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT]; // JLM this may change

  PushClipRegion();
  SetClipRegion(0, 639,0,479, 240, 320);

  ClearMpd(MpdDetectNum);

  MavInfo.MarkersOn = FALSE;

  int UseFlir;
  ProcessMissleStatus(&MissleOnBoard,MAVERIC_TYPE, &MavInfo, &pDBWeapon,&MavPos,&UseFlir);

  if(MavInfo.WeapFired)
  {
    MavInfo.WeapFired = FALSE;
	ResetMaverickF15();
  }

  if(!MissleOnBoard)
  {
    DrawTextAtLocation(XOfs - 15, YOfs,"NO WEAPONS",TextColor);
  }
  else
  {

   	ProcessCamSlew(MpdDetectNum, &MavCamSlew, &MavInfo);

	if(MavInfo.AquireMode == MSL_AUTO)
	{
	  PointCamToCurrentAGTarget(&MavInfo);
	}

    if((MavInfo.TrackOn))
    {
	  if(MavInfo.TargetObtained)
	  {
	    float Heading,Pitch;
        FPointDouble Target;

		if(MavInfo.TrackPosObtained)
	      Target = MavInfo.TrackPos;
        else
	      GetCamTargetPosition(&MavInfo,&Target);

		FPointDouble EndPoint;
		EndPoint.MakeVectorToFrom(Target,MavPos);

		FMatrix TempMatrix(PlayerPlane->Attitude);

		TempMatrix.Transpose();

		EndPoint *= TempMatrix;

		Heading = atan2(-EndPoint.X,-EndPoint.Z);
		Pitch   = atan2(EndPoint.Y,sqrt(EndPoint.Z*EndPoint.Z + EndPoint.X*EndPoint.X));

		Heading = RadToDeg(Heading);
		Pitch   = RadToDeg(Pitch);

	    if(IsCamWithinSlewBoundsF15(Heading,Pitch))
	    {
	       MavInfo.SlewLeftRight = Heading;
		   MavInfo.SlewUpDown    = Pitch;
	    }
        else
	    {
          MavInfo.TargetObtained = FALSE;
          MavInfo.TrackOn        = FALSE;
	    }

		// JLM TEMP
		DrawCamBuff(MpdNum,MavMonoBuff,XOfs,YOfs,MavInfo.SlewLeftRight,MavInfo.SlewUpDown,FALSE,TRUE,TRUE,MavPos,FALSE,NULL,MavInfo.Magnify,&MavInfo);

	  }
      else
	  {
		FPointDouble ProjPoint;
        FPointDouble Intersect;
		FPointDouble TgtLoc;
	    FPointDouble Target;
        int PlaneIndex;
		int LosVal;

		// JLM TEMP
		DrawCamBuff(MpdNum,MavMonoBuff,XOfs,YOfs,MavInfo.SlewLeftRight,MavInfo.SlewUpDown,FALSE,TRUE,TRUE,MavPos,FALSE,NULL,MavInfo.Magnify,&MavInfo);

        ProjPoint.SetValues(-CAM_PROJ_DIST*MavInfo.Matrix.m_Data.RC.R0C2,
			                  -CAM_PROJ_DIST*MavInfo.Matrix.m_Data.RC.R1C2,
							  -CAM_PROJ_DIST*MavInfo.Matrix.m_Data.RC.R2C2);
		ProjPoint += MavPos;

        LosVal = LOS(&MavPos,&ProjPoint,&Intersect, HI_RES_LOS, TRUE);

		MavInfo.TrackPosObtained = !LosVal;
  		MavInfo.TrackPos = Intersect;

		int PlaneFound,VehFound,ObjFound;
	    FPointDouble ObjTarget,PlaneTarget,VehTarget;
		float ObjDist,PlaneDist,VehDist;
		BasicInstance *GrndTrgt;
        MovingVehicleParams *VehTrgt;

		ObjFound   = GetFlirCamTargetF15(&MavPos,&ProjPoint,&GrndTrgt,&ObjDist,&TgtLoc);
		PlaneFound = GrabPlaneInFlirCamF15(&MavPos,&ProjPoint,&PlaneIndex,&PlaneDist);
		VehFound   = GetFlirCamVehicleF15(&MavPos,&ProjPoint,&VehTrgt,&VehDist);

        if(!ObjFound && !PlaneFound && !VehFound && !MavInfo.TrackPosObtained)
		{
          MavInfo.TrackOn = FALSE;
		}
		else
		{
   	      if( (ObjDist < PlaneDist) && (ObjDist < VehDist) )
	      {
			MavInfo.TargetObtained = TRUE;
			MavInfo.TargetType = FLIR_CAM_LOC_TARGET;
			MavInfo.LocTrgt = TgtLoc;
	      }
		  else if( (PlaneDist < ObjDist) && (PlaneDist < VehDist) )
		  {
   	        MavInfo.TargetObtained = TRUE;
            MavInfo.TargetType = FLIR_CAM_PLANE_TARGET;
			MavInfo.PlaneTrgt = &Planes[PlaneIndex];
		  }
		  else if( (VehDist < ObjDist) && (VehDist < PlaneDist) )
		  {
            MavInfo.TargetObtained = TRUE;
            MavInfo.TargetType = FLIR_CAM_VEH_TARGET;
			MavInfo.VehTrgt = VehTrgt;
	      }
		  else if(!LosVal)
		  {
			int CloseTargetType;

			// since mavs are tracking the ground,get closest object within 400 ft and track it
			if( GetCloseVehicleOrObjectF15(&Intersect,&GrndTrgt,&VehTrgt,&CloseTargetType) )
			{
               MavInfo.TargetObtained = TRUE;
			   if(CloseTargetType == FLIR_CAM_GROUND_TARGET)
			   {
                 MavInfo.TargetType = FLIR_CAM_GROUND_TARGET;
                 MavInfo.GrndTrgt    = GrndTrgt;
			   }
			   else
			   {
                 MavInfo.TargetType = FLIR_CAM_VEH_TARGET;
                 MavInfo.VehTrgt    = VehTrgt;
			   }
			}
			else
			{
              MavInfo.TargetObtained = TRUE;
  			  MavInfo.TargetType = FLIR_CAM_LOC_TARGET;
			  MavInfo.LocTrgt = Intersect;
			}
		  }

		  if(MavInfo.TargetObtained && ( (MavInfo.TargetType == FLIR_CAM_PLANE_TARGET) || (MavInfo.TargetType == FLIR_CAM_VEH_TARGET)  || (MavInfo.TargetType == FLIR_CAM_GROUND_TARGET)) )
		  {
            MavInfo.TrackPosObtained = FALSE;
		  }
		}
	  }
     }

	 if(!MavInfo.TrackOn)
	 {
	   DrawCamBuff(MpdNum,MavMonoBuff,XOfs,YOfs,MavInfo.SlewLeftRight,MavInfo.SlewUpDown,FALSE,TRUE,TRUE,MavPos,FALSE,NULL,MavInfo.Magnify,&MavInfo);
	 }

     GrCopyRectMaskColor(GrBuffFor3D,XOfs-67,YOfs-67, MavOverlay, 0,0,362 - 228 + 1 + 2,185 - 50 + 1 + 2,OverlayColor);

	 if(MavInfo.FOVToUse == WFOV)
   	   GrCopyRectMaskColor(GrBuffFor3D,XOfs-34,YOfs-41, FOVOverlay, 0,0,480 - 411 + 1,156 - 76 + 1,OverlayColor);

	 DrawAltAndElev(MpdNum, XOfs,YOfs, &MavInfo);

	 if(pDBWeapon != NULL)
	 {
	  float MinRange,MaxRange;
	  WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange, &MaxRange);
	  DrawMavGbuRangeMarkers(MpdNum,&MavInfo,XOfs,YOfs,MinRange,MaxRange);
	 }

	 ProcessPointerF15(MpdNum,XOfs,YOfs, &MavCross, &MavCamSlew, &MavInfo, MAVERIC_TYPE);
  }

  DrawMavOuterText(XOfs,YOfs,MpdNum,MpdDetectNum);

  GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

   if (Pressed)
   {
		switch(ButtonNum)
		{
		  case 2:  if( (!MavCamSlew.Slewing) && (!MavInfo.TrackOn) )
				    SetupCamSlew(SLEW_LEFT, MpdDetectNum, MAVERIC_TYPE);
				   break;
		  case 5:  ToggleWpnMode();
				   break;
		  case 6:  ToggleWpnFov();
				   break;
		  case 7:  if( (!MavCamSlew.Slewing) && (!MavInfo.TrackOn) )
				    SetupCamSlew(SLEW_DOWN, MpdDetectNum, MAVERIC_TYPE);
				   break;
		  case 9:  ToggleWpnTrack();
			       break;
		  case 10: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			       break;
		  case 11: CenterWpn();
				   break;
		  case 12: if( (!MavCamSlew.Slewing) && (!MavInfo.TrackOn) )
				    SetupCamSlew(SLEW_RIGHT, MpdDetectNum, MAVERIC_TYPE);
				   break;
		  case 16: ShowVectorDebug = !ShowVectorDebug; break;
		  case 17: if( (!MavCamSlew.Slewing) && (!MavInfo.TrackOn) )
				    SetupCamSlew(SLEW_UP, MpdDetectNum, MAVERIC_TYPE);
				   break;
		  case 18: MavInfo.MarkerLight = !MavInfo.MarkerLight;
			       break;
 		}
   }

   PopClipRegion();

}

//***********************************************************************************************************************************
// *****

void DoGbu(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
  int Pressed;
  int ButtonNum;
  int MissleOnBoard;
  int OverlayColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD]; // JLM this may change
  int TextColor    = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT]; // JLM this may change
  int GbuOnMissle = FALSE;

  FPointDouble GbuPos;
  DBWeaponType *pDBWeapon;

  PushClipRegion();
  SetClipRegion(0, 639,0,479, 240, 320);

  ClearMpd(MpdDetectNum);

  GbuInfo.MarkersOn = FALSE;

  int UseFlir;
  ProcessMissleStatus(&MissleOnBoard,GBU15_TYPE, &GbuInfo,&pDBWeapon,&GbuPos,&UseFlir);

  if(GbuInfo.CamType != -1)
	UseFlir = (GbuInfo.CamType == 1);   // 0 is TV  type, 1 is FLIR type

  if((GbuInfo.WeapFired) && (GbuInfo.GuideMethod == GBU_DIRECT) )
  {
 	ResetGbuF15();
  }

  // so the main function will know there is a bomb in the air and show the right camera
  GbuInIndirectMode = (GbuInfo.GuideMethod == GBU_INDIRECT);

  if(GbuInfo.GuideMethod == GBU_INDIRECT)
  {
	if(GbuInfo.WeapFired)
	{
	  if(pCurrentGBU != NULL)
	  {
	     GbuInfo.Weap = pCurrentGBU; // must do this because it takes time to create weapon
	  }

      if(pCurrentGBU)
	  {
		FPoint RelPos;

		RelPos.SetValues(0,0,-2.5);

	    GbuPos = pCurrentGBU->Pos;
		RelPos.RotateInto(RelPos,pCurrentGBU->Attitude);
	    RelPos += pCurrentGBU->Pos;
		GbuOnMissle = TRUE;
	  }
 	}

	if(GbuInfo.Weap != pCurrentGBU)
	{
	  if(GbuInfo.Weap != NULL)
        ResetGbuF15();
   	}

    if( (GbuInfo.WeapFired) && (GbuInfo.LaunchTimer == -1) )
	{
      if(GbuInfo.LaunchMode == GBU_NORM)
	  {
        GbuInfo.LaunchTimer = GetTickCount();
   	  }
	}

    if(GbuInfo.LaunchTimer != -1)
	{
      if(GetTickCount() - GbuInfo.LaunchTimer > 1750)
	  {
	    GbuInfo.LaunchMode = GBU_TRANS;
        GbuInfo.LaunchTimer = -1;
	  }

	}

   	if(GbuInfo.LaunchMode == GBU_TERM)
	{
      if(GbuInfo.TrackOn)
	  {
		  int GroundTargetType;

   		  switch(GbuInfo.TargetType)
		  {
		    case FLIR_CAM_GROUND_TARGET: GroundTargetType = GROUNDOBJECT;
				                         break;
			case FLIR_CAM_PLANE_TARGET:	 GroundTargetType = AIRCRAFT;
										 break;
            case FLIR_CAM_VEH_TARGET:    GroundTargetType = MOVINGVEHICLE;
									     break;
		    case FLIR_CAM_LOC_TARGET:    GbuInfo.GrndTrgt = NULL;
                                         GroundTargetType = GROUNDOBJECT;
				                         break;
		  }

		  if(GroundTargetType == GROUNDOBJECT)
		  	SetGroundTarget(pCurrentGBU,GbuInfo.GrndTrgt,GbuInfo.LocTrgt,GroundTargetType);
		  else if(GroundTargetType == AIRCRAFT)
		    SetGroundTarget(pCurrentGBU,GbuInfo.PlaneTrgt,GbuInfo.LocTrgt,GroundTargetType);
		  else if(GroundTargetType == MOVINGVEHICLE)
		    SetGroundTarget(pCurrentGBU,GbuInfo.VehTrgt,GbuInfo.LocTrgt,GroundTargetType);
   	  }

	}
  }

  if( (!MissleOnBoard) && (GbuInfo.GuideMethod == GBU_DIRECT) )
  {
     DrawTextAtLocation(XOfs - 15, YOfs,"NO WEAPONS",TextColor);
  }
  else if( (MissleOnBoard) || (GbuInfo.GuideMethod != GBU_DIRECT) )
  {
	ProcessCamSlew(MpdDetectNum, &GbuCamSlew, &GbuInfo);
	ProcessGbuSlew(MpdDetectNum, &GbuSlew, &GbuInfo);

	if(GbuInfo.AquireMode == MSL_AUTO)
	{
	  PointCamToCurrentAGTarget(&GbuInfo);
	}

    if(GbuInfo.TrackOn)
    {
	  if(GbuInfo.TargetObtained)
	  {
	     float Heading,Pitch;
         FPointDouble Target;

		if(GbuInfo.TrackPosObtained)
	      Target = GbuInfo.TrackPos;
        else
		  GetCamTargetPosition(&GbuInfo,&Target);

		if(GbuInfo.Weap)
          CalcNewFlirCamHeadingAndPitchF15(GbuPos, &Heading,&Pitch,Target,GbuInfo.Weap);
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

	    if(IsCamWithinSlewBoundsF15(Heading,Pitch))
	    {
	       GbuInfo.SlewLeftRight = Heading;
		   GbuInfo.SlewUpDown    = Pitch;
	    }
        else
	    {
          GbuInfo.TargetObtained = FALSE;
          GbuInfo.TrackOn        = FALSE;
	    }

		// JLM TEMP
		if( (GbuInfo.GuideMethod != GBU_DIRECT) && (GbuInfo.WeapFired == TRUE) && (pCurrentGBU != NULL) )
          DrawCamBuff(MpdNum,GbuMonoBuff,XOfs,YOfs,GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,FALSE,UseFlir,TRUE,GbuPos,TRUE,GbuInfo.Weap,GbuInfo.Magnify,&GbuInfo);
        else
	      DrawCamBuff(MpdNum,GbuMonoBuff,XOfs,YOfs,GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,FALSE,UseFlir,TRUE,GbuPos,FALSE,NULL,GbuInfo.Magnify,&GbuInfo);
	  }
      else
	  {
		FPointDouble ProjPoint;
        FPointDouble Intersect;
        FPointDouble TgtLoc;
	    FPointDouble Target;
        int PlaneIndex;
        int LosVal;

		 // JLM TEMP
		 if( (GbuInfo.GuideMethod != GBU_DIRECT) && (GbuInfo.WeapFired == TRUE) && (pCurrentGBU != NULL) )
          DrawCamBuff(MpdNum,GbuMonoBuff,XOfs,YOfs,GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,FALSE,UseFlir,TRUE,GbuPos,TRUE,GbuInfo.Weap,GbuInfo.Magnify,&GbuInfo);
         else
	      DrawCamBuff(MpdNum,GbuMonoBuff,XOfs,YOfs,GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,FALSE,UseFlir,TRUE,GbuPos,FALSE,NULL,GbuInfo.Magnify,&GbuInfo);

		if(GbuOnMissle)
          CalcCameraViewVectorF15(GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,&GbuPos,&ProjPoint,GbuInfo.Weap->Heading);
		else
		{
          ProjPoint.SetValues(-CAM_PROJ_DIST*GbuInfo.Matrix.m_Data.RC.R0C2,
			                  -CAM_PROJ_DIST*GbuInfo.Matrix.m_Data.RC.R1C2,
							  -CAM_PROJ_DIST*GbuInfo.Matrix.m_Data.RC.R2C2);
		  ProjPoint += GbuPos;
		}

		LosVal = LOS(&GbuPos,&ProjPoint,&Intersect, HI_RES_LOS, TRUE);

		GbuInfo.TrackPosObtained = !LosVal;
  		GbuInfo.TrackPos = Intersect;

  		int PlaneFound,VehFound,ObjFound;
		float ObjDist,PlaneDist,VehDist;
		BasicInstance *GrndTrgt;
        MovingVehicleParams *VehTrgt;

		ObjFound   = GetFlirCamTargetF15(&GbuPos,&ProjPoint,&GrndTrgt,&ObjDist,&TgtLoc);
		PlaneFound = GrabPlaneInFlirCamF15(&GbuPos,&ProjPoint,&PlaneIndex,&PlaneDist);
		VehFound   = GetFlirCamVehicleF15(&GbuPos,&ProjPoint,&VehTrgt,&VehDist);

        if(!ObjFound && !PlaneFound && !VehFound && !GbuInfo.TrackPosObtained)
		{
              GbuInfo.TrackOn = FALSE;
		}
		else
		{
          if( (ObjDist < PlaneDist) && (ObjDist < VehDist) )
	      {
			GbuInfo.TargetObtained = TRUE;
			GbuInfo.TargetType = FLIR_CAM_LOC_TARGET;
			GbuInfo.LocTrgt = TgtLoc;
  	      }
		  else if( (PlaneDist < ObjDist) && (PlaneDist < VehDist) )
		  {
   	        GbuInfo.TargetObtained = TRUE;
            GbuInfo.TargetType = FLIR_CAM_PLANE_TARGET;
			GbuInfo.PlaneTrgt = &Planes[PlaneIndex];
		  }
		  else if( (VehDist < ObjDist) && (VehDist < PlaneDist) )
		  {
            GbuInfo.TargetObtained = TRUE;
            GbuInfo.TargetType = FLIR_CAM_VEH_TARGET;
			GbuInfo.VehTrgt = VehTrgt;
	      }
		  else if(!LosVal)
		  {
            GbuInfo.TargetObtained = TRUE;
			GbuInfo.TargetType = FLIR_CAM_LOC_TARGET;
			GbuInfo.LocTrgt = Intersect;
		  }

		  if(GbuInfo.TargetObtained && ( (GbuInfo.TargetType == FLIR_CAM_PLANE_TARGET) || (GbuInfo.TargetType == FLIR_CAM_VEH_TARGET)) )
		  {
            GbuInfo.TrackPosObtained = FALSE;
		  }

   		}

	  }
	}

	if(!GbuInfo.TrackOn)
	{
      if( (GbuInfo.GuideMethod != GBU_DIRECT) && (GbuInfo.WeapFired == TRUE) && (pCurrentGBU != NULL) )
        DrawCamBuff(MpdNum,GbuMonoBuff,XOfs,YOfs,GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,FALSE,UseFlir,TRUE,GbuPos,TRUE,GbuInfo.Weap,GbuInfo.Magnify,&GbuInfo);
      else
	    DrawCamBuff(MpdNum,GbuMonoBuff,XOfs,YOfs,GbuInfo.SlewLeftRight,GbuInfo.SlewUpDown,FALSE,UseFlir,TRUE,GbuPos,FALSE,NULL,GbuInfo.Magnify,&GbuInfo);
	}

   	GrCopyRectMaskColor(GrBuffFor3D,XOfs-67,YOfs-67, MavOverlay, 0,0,362 - 228 + 1 + 2,185 - 50 + 1 + 2,OverlayColor);

	ProcessPointerF15(MpdNum,XOfs,YOfs, &GbuCross, &GbuCamSlew, &GbuInfo, GBU15_TYPE);

	DrawAltAndElev(MpdNum, XOfs,YOfs, &GbuInfo);

	if( (pDBWeapon !=	NULL) && (!GbuInfo.WeapFired) )
	{
	  float MinRange,MaxRange;
	  WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange, &MaxRange);
	  DrawMavGbuRangeMarkers(MpdNum,&GbuInfo,XOfs,YOfs,MinRange,MaxRange);
	}

  }

  DrawGbuOuterText(XOfs,YOfs,MpdNum,MpdDetectNum);

  GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

   if (Pressed)
   {
		switch(ButtonNum)
		{
		  case 2:  if( (GbuInfo.LaunchMode == GBU_TERM) && (!GbuInfo.TrackOn) )
				   {
					  if(!GbuCamSlew.Slewing)
					    SetupGbuSlew(SLEW_LEFT, MpdDetectNum);
				   }
			       else
				   {
			        if(!GbuCamSlew.Slewing)
				      SetupCamSlew(SLEW_LEFT, MpdDetectNum, GBU15_TYPE);
				   }
				   break;
		  case 5:  ToggleWpnMode();
				   break;
		  case 6:  if(GbuInfo.GuideMethod == GBU_INDIRECT)
				   {
					 if(GbuInfo.LaunchMode == GBU_NORM)
					 {
                       if(GbuInfo.WeapFired)
	                     GbuInfo.LaunchMode = GBU_TRANS;
					 }
					 else if(GbuInfo.LaunchMode == GBU_TRANS)
					 {
                       if(pCurrentGBU != NULL)
                          GbuInfo.LaunchMode = GBU_TERM;
					 }
				   }
				   break;
		  case 7:  if( (GbuInfo.LaunchMode == GBU_TERM) && (!GbuInfo.TrackOn) )
				   {
					  if(!GbuCamSlew.Slewing)
					    SetupGbuSlew(SLEW_DOWN, MpdDetectNum);
				   }
			       else
				   {
			        if(!GbuCamSlew.Slewing)
				      SetupCamSlew(SLEW_DOWN, MpdDetectNum, GBU15_TYPE);
				   }
				   break;
		  case 8:  GbuInfo.GuideMethod++;
			       if(GbuInfo.GuideMethod > GBU_INDIRECT) GbuInfo.GuideMethod = GBU_DIRECT;
				   if( (GbuInfo.GuideMethod == GBU_INDIRECT) && (GbuInfo.AquireMode == MSL_AUTO))
                       GbuInfo.GuideMethod = GBU_DIRECT;
				   // go to indirect only if pod is present
				   if(GbuInfo.GuideMethod == GBU_INDIRECT)
				   {
                     if( (PlayerPlane->WeapLoad[AG_CENTER].Count <= 0) || (PlayerPlane->WeapLoad[AG_CENTER].WeapId != 40) )
                        GbuInfo.GuideMethod = GBU_DIRECT;
				   }
				   break;
		  case 9:  ToggleWpnTrack();
			       break;
		  case 10: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			       break;
		  case 11: CenterWpn();
				   break;
		  case 12: if( (GbuInfo.LaunchMode == GBU_TERM) && (!GbuInfo.TrackOn) )
				   {
					  if(!GbuCamSlew.Slewing)
					    SetupGbuSlew(SLEW_RIGHT, MpdDetectNum);
				   }
			       else
				   {
			        if(!GbuCamSlew.Slewing)
				      SetupCamSlew(SLEW_RIGHT, MpdDetectNum, GBU15_TYPE);
				   }
			  	   break;
		  case 15: if( (GbuInfo.LaunchMode == GBU_TRANS) )
					  if(!GbuCamSlew.Slewing)
					    SetupGbuSlew(SLEW_RIGHT, MpdDetectNum);
			       break;
		  case 16: if( (GbuInfo.LaunchMode == GBU_TRANS) )
					  if(!GbuCamSlew.Slewing)
					   SetupGbuSlew(SLEW_LEFT, MpdDetectNum);
			       break;
		  case 17: if( (GbuInfo.LaunchMode == GBU_TERM) && (!GbuInfo.TrackOn) )
				   {
					  if(!GbuCamSlew.Slewing)
					    SetupGbuSlew(SLEW_UP, MpdDetectNum);
				   }
			       else
				   {
			        if(!GbuCamSlew.Slewing)
				      SetupCamSlew(SLEW_UP, MpdDetectNum, GBU15_TYPE);
				   }
				   break;
		  case 18: GbuInfo.MarkerLight = !GbuInfo.MarkerLight;
			       break;
 		}
   }

   PopClipRegion();

}

//***********************************************************************************************************************************
// *****

void DoFlir(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
  int Pressed;
  int ButtonNum;
  int TextColor    = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
  FPointDouble FlirPos;
  int CamDrawn = FALSE;

  int Index;
  if(FlirInfo.ImageMode == FLIR_WHOT)
    Index = COLOR_NORM_BOLD;
  else
	Index = COLOR_BLACK_HOT;

  int OverlayColor = MpdColors[Mpds[MpdNum].IsMono][Index];

  PushClipRegion();
  //SetClipRegion(0, 639,0,479, 240, 320);

  ClearMpd(MpdDetectNum);

  FlirPos = FlirOffsetF15;
  FlirPos.RotateInto(FlirPos,PlayerPlane->Attitude);
  FlirPos += PlayerPlane->WorldPosition;

  if(MpdDamage[MPD_DAMAGE_TARGET_IR])
  {
   DrawTextAtLocation(MPD.MpdList[MpdDetectNum].CenterX - 35,MPD.MpdList[MpdDetectNum].CenterY,"SYSTEM MALFUNCTION",TextColor);
   DrawWordAtButton(MpdDetectNum,10, "M", FALSE, TextColor);
   GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);
   if( (Pressed) && (ButtonNum == 10) )
	 Mpds[MpdNum].Mode = MAIN_MENU_MODE;
   PopClipRegion();
   return;
  }

  if(FlirInfo.TrackOn != PrevFlirTrackStateF15)
  {
    PrevFlirTrackStateF15 = FlirInfo.TrackOn;
	if(FlirInfo.TrackOn && FlirInfo.CdesOn)
	{
       if(LaseDesignateStartTime == -1)
         LaseDesignateStartTime = GetTickCount();
	}
  }

int FlirAvail = 1;//( (PlayerPlane->WeapLoad[LANTIRN_L].Count > 0) && (PlayerPlane->WeapLoad[LANTIRN_L].WeapId == 46) );

if(!FlirAvail)
{
  DrawTextAtLocation(XOfs - 15, YOfs,"NO FLIR POD",TextColor);
  DrawWordAtButton(MpdDetectNum,10,"M",FALSE,TextColor);
}
else
{
  if(!FlirInfo.TrackOn)
  {
     ProcessFlirCamSlewF15(MpdDetectNum, &FlirCamSlew,FlirInfo.FOVToUse);
  }

  if(!FlirInfo.DeclutterOn)
	DrawFlirAttackTimer();

  if(FlirInfo.AquireMode == MSL_AUTO && (FlirInfo.ModeToSelect != FLIR_AA))
  {
	PointFlirToCurrentAGTargetF15();
  }

  // if new target has been designated through AA radar
  if(FlirInfo.ModeToSelect == FLIR_AA)
  {
	FlirInfo.TrackPosObtained = FALSE;
    if(FlirInfo.TargetObtained && (PlayerPlane->AADesignate) )
	{
 	  if((FlirInfo.PlaneTrgt != PlayerPlane->AADesignate))
	  {
        FlirInfo.PlaneTrgt = PlayerPlane->AADesignate;
	  }
	}
	else
	{
	  FlirInfo.TrackOn = TRUE;
	  FlirInfo.TargetObtained = FALSE;
      FlirInfo.SlewLeftRight = 0.0;
	  FlirInfo.SlewUpDown = 0.0;
	}

	if(PlayerPlane->AADesignate != NULL)
	{
	  float Heading,Pitch;
	  CalcNewFlirCamHeadingAndPitchF15(FlirPos,&Heading,&Pitch,(PlayerPlane->AADesignate->WorldPosition),NULL);
      if(!IsFlirWithinSlewBoundsF15(Heading,Pitch))
         FlirInfo.TrackOn = FALSE;
	}
	else
	{
      FlirInfo.TrackOn = FALSE;
    }
  }

  if(FlirInfo.TrackOn)
  {
	if(FlirInfo.TargetObtained)
	{
	  float Heading,Pitch;
      FPointDouble Target;
	  int TargetObtained;

      GetFlirTargetPositionF15(&TargetObtained,&Target);

	  if( ((FlirInfo.CdesOn) && (FlirInfo.AquireMode == MSL_MANUAL)) || ((FlirInfo.ModeToSelect == FLIR_AA) && (TargetObtained)) )
	  {
        PlayerPlane->AGDesignate = Target;
	  }

	  if(FlirInfo.TrackPosObtained)
	    Target = FlirInfo.TrackPos;

      CalcNewFlirCamHeadingAndPitchF15(FlirPos,&Heading,&Pitch,Target,NULL);

	  if(IsFlirWithinSlewBoundsF15(Heading,Pitch))
	  {
	     FlirInfo.SlewLeftRight = Heading;
		 FlirInfo.SlewUpDown    = Pitch;
	  }
      else
	  {
        FlirInfo.TargetObtained   = FALSE;
		if(FlirInfo.ModeToSelect != FLIR_AA)
           FlirInfo.TrackOn = FALSE;
        FlirInfo.TrackPosObtained = FALSE;
	  }

	  // JLM HERE TEMP
	  DrawCamBuff(MpdNum,FlirMonoBuff,XOfs,YOfs,FlirInfo.SlewLeftRight,FlirInfo.SlewUpDown,FlirInfo.ShortRangeOn,TRUE,(FlirInfo.ImageMode == FLIR_WHOT),FlirPos,FALSE,NULL,FlirInfo.Magnify);

	}
    else
	{
		FPointDouble ProjPoint;
        FPointDouble Intersect;
        FPointDouble TgtLoc;
        int PlaneIndex;
		int LosVal;

	    // JLM HERE TEMP
	    DrawCamBuff(MpdNum,FlirMonoBuff,XOfs,YOfs,FlirInfo.SlewLeftRight,FlirInfo.SlewUpDown,FlirInfo.ShortRangeOn,TRUE,(FlirInfo.ImageMode == FLIR_WHOT),FlirPos,FALSE,NULL,FlirInfo.Magnify);

        CalcCameraViewVectorF15(FlirInfo.SlewLeftRight,FlirInfo.SlewUpDown,&FlirPos,&ProjPoint,PlayerPlane->Heading);

        LosVal = LOS(&FlirPos,&ProjPoint,&Intersect,HI_RES_LOS,TRUE);

        FlirInfo.TrackPosObtained = !LosVal;

	    FlirInfo.TrackPos = Intersect;

        int PlaneFound,VehFound,ObjFound;
		float ObjDist,PlaneDist,VehDist;
		BasicInstance *GrndTrgt;
        MovingVehicleParams *VehTrgt;

        PlaneFound = ObjFound = VehFound = FALSE;
        ObjDist = PlaneDist = VehDist = 1000000;

		if(FlirInfo.ModeToSelect == FLIR_AA)
		{
		  PlaneFound = TRUE;
		  if( (PlayerPlane->AADesignate != NULL) )
		  {
			PlaneIndex = GetCurrentAATargetIndex();
     	    PlaneDist  = 0;
		  }
		  else
		  {
 			FlirInfo.TargetObtained = FALSE;
		  }
		}
	    else
		{
		  PlaneFound = FALSE;
		  ObjFound   = GetFlirCamTargetF15(&FlirPos,&ProjPoint,&GrndTrgt,&ObjDist,&TgtLoc);
		  VehFound   = GetFlirCamVehicleF15(&FlirPos,&ProjPoint,&VehTrgt,&VehDist);
		}

        if(!ObjFound && !PlaneFound && !VehFound && !FlirInfo.TrackPosObtained)
		{
           FlirInfo.TrackOn = FALSE;
		}
		else
		{
   		  if( (ObjDist < PlaneDist) && (ObjDist < VehDist) )
	      {
			FlirInfo.TargetObtained = TRUE;
			FlirInfo.TargetType = FLIR_CAM_GROUND_TARGET;
			FlirInfo.LocTrgt = TgtLoc;
	      }
		  else if( (PlaneDist < ObjDist) && (PlaneDist < VehDist) )
		  {
            FlirInfo.TargetObtained = TRUE;
			FlirInfo.TargetType = FLIR_CAM_PLANE_TARGET;
			FlirInfo.PlaneTrgt = &Planes[PlaneIndex];
		  }
		  else if( (VehDist < ObjDist) && (VehDist < PlaneDist) )
		  {
            FlirInfo.TargetObtained = TRUE;
			FlirInfo.TargetType = FLIR_CAM_VEH_TARGET;
			FlirInfo.VehTrgt = VehTrgt;
	      }
		  else if(!LosVal)
		  {
            FlirInfo.TargetObtained = TRUE;
			FlirInfo.TargetType = FLIR_CAM_LOC_TARGET;
			FlirInfo.LocTrgt = Intersect;
		  }

		  if(FlirInfo.TargetObtained && ( (FlirInfo.ModeToSelect == FLIR_AA) || (FlirInfo.TargetType == FLIR_CAM_VEH_TARGET)) )
		  {
            FlirInfo.TrackPosObtained = FALSE;
		  }
		}
  	}
  }

  if( (FlirInfo.LaserState == FLIR_LSR_SAFE) && (UFC.MasterArmState == ON) && (PlayerPlane->Altitude < 24000))
  {
    FlirInfo.LaserState = FLIR_LSR_ARMED;
  }
  else if( (FlirInfo.LaserState == FLIR_LSR_ARMED) && ((PlayerPlane->Altitude > 24000) || (UFC.MasterArmState == OFF)) )
    FlirInfo.LaserState = FLIR_LSR_SAFE;

  if(FlirInfo.LaserState == FLIR_LSR_FIRING)
  {
    FPointDouble LaserPos;
	int TargetObtained;

    GetFlirTargetPositionF15(&TargetObtained,&LaserPos);

	if(!TargetObtained)  // if flir did not already calculate a target
	{
      int TargetObtained;
	  int TargetType;
	  FPointDouble TargetLoc;
	  BasicInstance *GroundTarget;
	  MovingVehicleParams *Veh;
 	  PlaneParams *Plane;
      CamDrawn = TRUE;
	  DrawCamBuff(MpdNum,FlirMonoBuff,XOfs,YOfs,FlirInfo.SlewLeftRight,FlirInfo.SlewUpDown,FlirInfo.ShortRangeOn,TRUE,(FlirInfo.ImageMode == FLIR_WHOT),FlirPos,FALSE,NULL,FlirInfo.Magnify);
      GetLaserTargetF15(&FlirPos,&TargetObtained,&TargetType,&TargetLoc,&GroundTarget,&Veh,&Plane);
      GetLaserTargetPositionF15(&LaserPos,TargetType,GroundTarget,Veh,Plane,TargetLoc);
	}

	LaserLocF15 = LaserPos;

	float Heading,Pitch;
    GetRelativeHPFromLoc(PlayerPlane,FlirPos, LaserLocF15, &Heading, &Pitch);
	FlirInfo.MaskOn = (Pitch >= 10);

    if(FlirInfo.MaskOn)
      lLaserStatusFlags |= WSO_LASER_MASKED;
	else
	  lLaserStatusFlags &= ~WSO_LASER_MASKED;

	if(Pitch > 3 && Pitch < 10)
      lLaserStatusFlags |= WSO_LASER_MASK_WARN;
	else
      lLaserStatusFlags &= ~WSO_LASER_MASK_WARN;

	if(!FlirInfo.DeclutterOn)
	{
	  float LaseDist = hypot((LaserLocF15.X - FlirPos.X),(LaserLocF15.Y - FlirPos.Y),(LaserLocF15.Z - FlirPos.Z));
	  LaseDist *= WUTOFT;
	  IntTo5DigitFont(int(LaseDist),Str1);
      sprintf(TmpStr,"%s%s","LAS",Str1);
      DrawTextAtLocation(XOfs-54,YOfs-41,TmpStr,OverlayColor);
	}
  }

   if(!FlirInfo.TrackOn && !CamDrawn)
      DrawCamBuff(MpdNum,FlirMonoBuff,XOfs,YOfs,FlirInfo.SlewLeftRight,FlirInfo.SlewUpDown,FlirInfo.ShortRangeOn,TRUE,(FlirInfo.ImageMode == FLIR_WHOT),FlirPos,FALSE,NULL,FlirInfo.Magnify);
   if(!FlirInfo.DeclutterOn)
   {
    if(FlirInfo.FOVToUse == WFOV)
   	  GrCopyRectMaskColor(GrBuffFor3D,XOfs-17,YOfs-20, ENFOVOverlay, 0,0,543 - 506 + 1,136 - 96 + 1,OverlayColor);
    else if(FlirInfo.FOVToUse == NFOV)
   	  GrCopyRectMaskColor(GrBuffFor3D,XOfs-34,YOfs-41, FOVOverlay, 0,0,480 - 411 + 1,156 - 76 + 1,OverlayColor);
   }
   DrawRadarSymbol(XOfs+1,YOfs-3,FLIR_CENTER,OverlayColor,GrBuffFor3D);
   if(!FlirInfo.DeclutterOn)
   DrawFlirPointer(MpdNum,XOfs,YOfs,OverlayColor);
   DrawFlirOuterText(XOfs,YOfs,MpdNum,MpdDetectNum);
   if(!FlirInfo.DeclutterOn)
    DrawFlirAltAndElev(MpdNum,XOfs,YOfs,OverlayColor);

}
  GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

   if(Pressed)
   {
	    if(!FlirAvail && ButtonNum != 10) // disable all other keys
		{
		  PopClipRegion();
		  return;
		}

		switch(ButtonNum)
		{
		  case 0: ToggleTFlirCdes();
			      break;
		  case 2: if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) && (FlirInfo.AquireMode == MSL_MANUAL) )
				    SetupCamSlew(SLEW_LEFT, MpdDetectNum, FLIR_TYPE);
				  break;
          case 4: ToggleTFlirPolarity();
				  break;
		  case 5: FlirInfo.ModeToSelect++;
			      if(FlirInfo.ModeToSelect > FLIR_AA)
					  FlirInfo.ModeToSelect = FLIR_AG;
                  if(FlirInfo.ModeToSelect == FLIR_AA)
				  {
				     FlirInfo.AquireMode = MSL_AUTO;
                     FlirInfo.SlewLeftRight = 0.0;
					 FlirInfo.SlewUpDown = 0.0;
				  }
				  break;
		  case 6: ToggleTFlirFov();
				  break;
		  case 7: if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn)  && (FlirInfo.AquireMode == MSL_MANUAL) )
				    SetupCamSlew(SLEW_DOWN, MpdDetectNum, FLIR_TYPE);
				   break;
		  case 9:  ToggleTFlirTrack();
			       break;
		  case 10: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			       break;
		  case 11: CenterFlir();
				   break;
		  case 12: if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) && (FlirInfo.AquireMode == MSL_MANUAL) )
				    SetupCamSlew(SLEW_RIGHT, MpdDetectNum, FLIR_TYPE);
				   break;
		  case 8:  ToggleTFlirAquireMode();
				   break;
		  case 15: FlirInfo.DeclutterOn = !FlirInfo.DeclutterOn;
			       break;
		  case 16: FlirInfo.ShortRangeOn = !FlirInfo.ShortRangeOn;
			       break;
		  case 17: if( (!FlirCamSlew.Slewing) && (!FlirInfo.TrackOn) && (FlirInfo.AquireMode == MSL_MANUAL) )
				    SetupCamSlew(SLEW_UP, MpdDetectNum, FLIR_TYPE);
				   break;
	      case 18: ToggleLaser();
				   break;
 		}
   }

   PopClipRegion();

}

//***********************************************************************************************************************************

void ToggleTFlirPolarity()
{
  FlirInfo.ImageMode++;
  if(FlirInfo.ImageMode > FLIR_BHOT)
	FlirInfo.ImageMode = FLIR_WHOT;
}

//***********************************************************************************************************************************

void ToggleTFlirFov()
{
  FlirInfo.FOVToUse++;
  if(FlirInfo.FOVToUse > ENFOV)
	FlirInfo.FOVToUse = WFOV;
  switch (FlirInfo.FOVToUse)
  {
	case WFOV:  FlirInfo.Magnify = (float)5.85*(float)DEGREE;   break;
	case NFOV:  FlirInfo.Magnify = (float)1.67*(float)DEGREE;   break;
	case ENFOV: FlirInfo.Magnify = (float)0.83*(float)DEGREE;   break;
  }

}

//***********************************************************************************************************************************

void ToggleTFlirAquireMode()
{
  if(FlirInfo.ModeToSelect != FLIR_AA)
  {
    FlirInfo.AquireMode++;
    if(FlirInfo.AquireMode > MSL_AUTO)
 	  FlirInfo.AquireMode = MSL_MANUAL;
    if(FlirInfo.AquireMode == MSL_AUTO)
      FlirInfo.AutoSet = TRUE;
  }
}

//***********************************************************************************************************************************

void ToggleTFlirCdes()
{
  FlirInfo.CdesOn = !FlirInfo.CdesOn;
  if(FlirInfo.CdesOn)
	PrevFlirTrackStateF15 = !FlirInfo.TrackOn;
}

//***********************************************************************************************************************************

void ToggleTFlirTrack()
{
  FlirInfo.TrackOn = !FlirInfo.TrackOn;
  if(!FlirInfo.TrackOn)
  {
	FlirInfo.TargetObtained = FALSE;
	FlirInfo.TrackPosObtained = FALSE;
  }
}

void ToggleLaser()
{
  if(UFC.MasterArmState == ON)
  {
    FlirInfo.LaserState++;
    if(FlirInfo.LaserState > FLIR_LSR_FIRING)
       FlirInfo.LaserState = FLIR_LSR_ARMED;
  }

  if(FlirInfo.LaserState == FLIR_LSR_FIRING)
  {
    FlirInfo.MaskOn = FALSE;
    if(FlirFlashStartTime == -1)
      FlirFlashStartTime = GetTickCount();
  }

  if(FlirInfo.LaserState == FLIR_LSR_ARMED)
  {
    FlirFlashStartTime = -1;
	FlirDrawL = TRUE;
  }

  if(FlirInfo.LaserState == FLIR_LSR_FIRING)
	lLaserStatusFlags |= WSO_LASER_IS_ON;
  else
   lLaserStatusFlags &= ~WSO_LASER_IS_ON;
}

//***********************************************************************************************************************************

void CenterFlir()
{
  if(!FlirInfo.TrackOn)
  {
	FlirInfo.SlewLeftRight = 0.0;
    FlirInfo.SlewUpDown = 0.0;
  }
}

//***********************************************************************************************************************************

int WeapIsMav()
{
  int Side;
  int Type;
  if(IsAGMissleSelected(&Type,&Side))
  {
    if(Type == MAVERIC_TYPE)
	 return(TRUE);
	else
	 return(FALSE);
  }

  return(FALSE);

}

//***********************************************************************************************************************************

void CenterWpn()
{
  if(WeapIsMav())
  {
    if(!MavInfo.TrackOn)
	{
	  MavInfo.SlewLeftRight = 0.0;
      MavInfo.SlewUpDown = -12.0;
	}
  }
  else
  {
    if(!GbuInfo.TrackOn)
	{
	  GbuInfo.SlewLeftRight = 0.0;
      GbuInfo.SlewUpDown = -12.0;
	}
  }
}

//***********************************************************************************************************************************

void ToggleWpnMode()
{
  if(WeapIsMav())
  {
    MavInfo.AquireMode++;
	if(MavInfo.AquireMode > MSL_AUTO) MavInfo.AquireMode = MSL_MANUAL;
	if(MavInfo.AquireMode == MSL_AUTO)
     MavInfo.AutoSet = TRUE;
  }
  else
  {
    GbuInfo.AquireMode++;
	if(GbuInfo.AquireMode > MSL_AUTO)
      GbuInfo.AquireMode = MSL_MANUAL;
	if(GbuInfo.AquireMode == MSL_AUTO)
      GbuInfo.GuideMethod = GBU_DIRECT;
	if(GbuInfo.AquireMode == MSL_AUTO)
     GbuInfo.AutoSet = TRUE;
  }
}

//***********************************************************************************************************************************

void ToggleWpnFov()
{
  if(WeapIsMav())
  {
    MavInfo.FOVToUse++;
	if(MavInfo.FOVToUse > NFOV) MavInfo.FOVToUse = WFOV;
	if(MavInfo.FOVToUse == WFOV)
	  MavInfo.Magnify = (float)5.8*(float)DEGREE;
	else
	  MavInfo.Magnify = (float)1.67*(float)DEGREE;
  }
}

//***********************************************************************************************************************************

void ToggleWpnTrack()
{
  if(WeapIsMav())
  {
    if(MavInfo.AquireMode == MSL_MANUAL)
	{
	  MavInfo.TrackOn = !MavInfo.TrackOn;
	  if(!MavInfo.TrackOn)
	  {
		MavInfo.TargetObtained = FALSE;
		MavInfo.TrackPosObtained = FALSE;
	  }
	}
  }
  else
  {
    if(GbuInfo.AquireMode == MSL_MANUAL)
	{
	  GbuInfo.TrackOn = !GbuInfo.TrackOn;
	  if(!GbuInfo.TrackOn)
	  {
		GbuInfo.TargetObtained = FALSE;
		GbuInfo.TrackPosObtained = FALSE;
	  }
	}
  }
}

//***********************************************************************************************************************************
// *****

void GetLaserTargetF15(FPointDouble *FlirPos, int *TargetObtained,int *TargetType,FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane)
{
   FPointDouble ProjPoint;
   FPointDouble Intersect;
   FPointDouble TgtLoc;
   int PlaneIndex;

   CalcCameraViewVectorF15(FlirInfo.SlewLeftRight,FlirInfo.SlewUpDown,FlirPos,&ProjPoint,PlayerPlane->Heading);

   LOS(FlirPos,&ProjPoint,&Intersect, HI_RES_LOS, TRUE);

   int PlaneFound,VehFound,ObjFound;
   float ObjDist,PlaneDist,VehDist;
   BasicInstance *GrndTrgt;
   MovingVehicleParams *VehTrgt;

   ObjFound   = GetFlirCamTargetF15(FlirPos,&Intersect,&GrndTrgt,&ObjDist,&TgtLoc);
   PlaneFound = GrabPlaneInFlirCamF15(FlirPos,&Intersect,&PlaneIndex,&PlaneDist);
   VehFound   = GetFlirCamVehicleF15(FlirPos,&Intersect,&VehTrgt,&VehDist);

   if(ObjFound || PlaneFound || VehFound)
   {
		if( (ObjDist < PlaneDist) && (ObjDist < VehDist) )
	    {
           *TargetObtained = TRUE;
		   *TargetType = FLIR_CAM_GROUND_TARGET;
		   *GroundTarget = GrndTrgt;
	    }
		else if( (PlaneDist < ObjDist) && (PlaneDist < VehDist) )
		{
            *TargetObtained = TRUE;
			*TargetType = FLIR_CAM_PLANE_TARGET;
			*Plane     = &Planes[PlaneIndex];
		}
		else if( (VehDist < ObjDist) && (VehDist < PlaneDist) )
		{
            *TargetObtained = TRUE;
			*TargetType = FLIR_CAM_VEH_TARGET;
			*Veh        = VehTrgt;
	    }
	}
    else
    {
        *TargetObtained = TRUE;
        *TargetType = FLIR_CAM_LOC_TARGET;
        *TargetLoc = Intersect;
	}

}

//***********************************************************************************************************************************
// *****

void DrawCamBuff(int MpdNum,GrBuff *CamBuff,int XOfs,int YOfs,float SlewLeftRight,float SlewUpDown,int UseHiDetail,int DrawAsFlir,int GreenHot,FPointDouble CamLoc,int UseGbu,WeaponParams *Weap,int Magnify,CamInfoType *Cam)
{

  FPointDouble CamOffset;
  GrBuff *realbuff;
  int *TempCam;

   	PushClipRegion();

	if( GrBuffFor3D->pGrBuffPolygon)
	{
		//calc a mid pt for the displays
		GrBuffFor3D->ClipLeft = XOfs - (115 >> 1);
		GrBuffFor3D->ClipRight = GrBuffFor3D->ClipLeft + 115;

		GrBuffFor3D->ClipTop = YOfs - (110 >> 1);
		GrBuffFor3D->ClipBottom = GrBuffFor3D->ClipTop + 110;

		GrBuffFor3D->MidX = XOfs;
		GrBuffFor3D->MidY = YOfs;

		realbuff = GrBuffFor3D;
	}
	else
	{
  		GrBuffFor3D->ClipLeft = XOfs - (115 >> 1);
  		GrBuffFor3D->ClipRight = GrBuffFor3D->ClipLeft + 115;

  		GrBuffFor3D->ClipTop = YOfs - (110 >> 1);
  		GrBuffFor3D->ClipBottom = GrBuffFor3D->ClipTop + 110;

  		GrBuffFor3D->MidX=XOfs;
  		GrBuffFor3D->MidY=YOfs;

		realbuff = GrBuffFor3D;
	}

	TempCam = (int *)Camera1.AttachedObject;

  Camera1.AttachedObject = NULL;




	// Turn on 2nd pass
	if( GrBuffFor3D->pGrBuffPolygon)
		SetSecondPass( TRUE, NULL );

	 // JLM if Gbu, use its heading instead of the planes
	  if(UseGbu)
	  {
		DrawLimitedView(realbuff,CamLoc,NormDegree(fANGLE_TO_DEGREES(Weap->Heading) + SlewLeftRight),NormDegree(SlewUpDown),Magnify,Mpds[MpdNum].IsMono,DrawAsFlir,GreenHot,UseHiDetail,TRUE,0,Weap);
	  }
	  else
	  {
		if(DrawAsFlir && (Cam == NULL))
		  DrawLimitedView(realbuff,CamLoc,NormDegree(fANGLE_TO_DEGREES(PlayerPlane->Heading) + SlewLeftRight),NormDegree(SlewUpDown),Magnify,Mpds[MpdNum].IsMono,DrawAsFlir,GreenHot,UseHiDetail,TRUE,0,NULL);
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


  DrawBox(realbuff,realbuff->ClipLeft,realbuff->ClipTop,115,110,(Mpds[MpdNum].IsMono) ? 63 : 53);

  if (realbuff == GrBuffFor3D)
  {
  	PopClipRegion();
	Set3DScreenClip();
  	PushClipRegion();
  }

//  SetClipRegion(0, 639,0,479, 240, 320);

  Camera1.AttachedObject = TempCam;
}

//***********************************************************************************************************************************
// *****

void DrawAltAndElev(int MpdNum,int XOfs,int YOfs,CamInfoType *Cam)
{
  int AltElevColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD]; // JLM this may change

  IntTo3DigitFont(int(fabs(Cam->SlewLeftRight)),Str1);
  sprintf(TmpStr,"%s%s%s","AZ",Str1,(Cam->SlewLeftRight >= 0) ? "L" : "R");
  DrawTextAtLocation(XOfs-54,YOfs-53,TmpStr,AltElevColor);

  IntTo3DigitFont(int(fabs(Cam->SlewUpDown)),Str1);
  sprintf(TmpStr,"%s%s%s","EL",Str1,(Cam->SlewUpDown >= 0) ? "U" : "D");
  DrawTextAtLocation(XOfs-54,YOfs-47,TmpStr,AltElevColor);
}

//***********************************************************************************************************************************
// *****

void DrawFlirAltAndElev(int MpdNum, int XOfs,int YOfs,int Color)
{
  int AltElevColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD]; // JLM this may change

  IntTo3DigitFont(int(fabs(FlirInfo.SlewLeftRight)),Str1);
  sprintf(TmpStr,"%s%s%s","AZ",Str1,(FlirInfo.SlewLeftRight >= 0) ? "L" : "R");
  DrawTextAtLocation(XOfs-54,YOfs-53,TmpStr,Color);

  IntTo3DigitFont(int(fabs(FlirInfo.SlewUpDown)),Str1);
  sprintf(TmpStr,"%s%s%s","EL",Str1,(FlirInfo.SlewUpDown >= 0) ? "U" : "D");
  DrawTextAtLocation(XOfs-54,YOfs-47,TmpStr,Color);
}

//***********************************************************************************************************************************
// *****

void DrawFlirAttackTimer(void)
{




}

//***********************************************************************************************************************************
// *****

void DrawMavOuterText(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
	char *String;

	int TextColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

    DrawWordAtButton(MpdDetectNum,10,"M",FALSE,TextColor);
    DrawVerticalWordAtLocation(XOfs+64,YOfs+13,"BST",FALSE,TextColor);

   	switch (MavInfo.AquireMode)
	{
	  case MSL_MANUAL: String = "MAN";	break;
	  case MSL_AUTO:   String = "AUTO";	break;
	}

	DrawWordAtButton(MpdDetectNum,5,String,FALSE,TextColor);

	switch (MavInfo.FOVToUse)
	{
	  case WFOV: String = "WFOV"; break;
	  case NFOV: String = "NFOV"; break;
	}

	DrawWordAtButton(MpdDetectNum,6,String,FALSE,TextColor);

	switch (MavInfo.RightBayStat)
	{
	  case MSL_STBY: String = "STBY"; break;
	  case MSL_RDY : String = "RDY";  break;
	  default      : String = ""; break;
	}

	DrawWordAtButton(MpdDetectNum,15,String,(MavInfo.RightBayStat == MSL_RDY),TextColor);

	if(MavInfo.AquireMode == MSL_MANUAL)
	  DrawWordAtButton(MpdDetectNum,9,"TRK",(MavInfo.TrackOn == TRUE),TextColor);

	switch (MavInfo.LeftBayStat)
	{
	  case    MSL_STBY: String = "STBY"; break;
	  case    MSL_RDY : String = "RDY";  break;
	  default         : String = ""; break;
	}

    if(MavInfo.MarkersOn)
       DrawWordAtButton(MpdDetectNum,18,(MavInfo.MarkerLight) ? "LGHT" : "DRK",FALSE,TextColor);

	DrawWordAtButton(MpdDetectNum,19,String,(MavInfo.LeftBayStat == MSL_RDY),TextColor);

 }

//***********************************************************************************************************************************
// *****

void DrawGbuOuterText(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
	char *String;

    int TextColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
	int ArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];

    DrawWordAtButton(MpdDetectNum,10,"M",FALSE,TextColor);
    DrawVerticalWordAtLocation(XOfs+64,YOfs+13,"BST",FALSE,TextColor);

   	switch (GbuInfo.AquireMode)
	{
	  case MSL_MANUAL: String = "MAN";	break;
	  case MSL_AUTO:   String = "AUTO";	break;
	}

	DrawWordAtButton(MpdDetectNum,5,String,FALSE,TextColor);

	if(GbuInfo.GuideMethod == GBU_INDIRECT)
	{
      switch(GbuInfo.LaunchMode)
	  {
	    case GBU_NORM : String = "NORM";  break;
	    case GBU_TRANS: String = "TRANS"; break;
		case GBU_TERM : String = "TERM";  break;
	  }

	  DrawWordAtButton(MpdDetectNum,6,String,FALSE,TextColor);

	}

	switch (GbuInfo.RightBayStat)
	{
	  case MSL_STBY: String = "STBY"; break;
	  case MSL_RDY : String = "RDY";  break;
	  default      : String = ""; break;
	}

	if(GbuInfo.LaunchMode != GBU_TRANS)
	  DrawWordAtButton(MpdDetectNum,15,String,(GbuInfo.RightBayStat == MSL_RDY),TextColor);

	DrawWordAtButton(MpdDetectNum,9,"TRK",(GbuInfo.TrackOn == TRUE),TextColor);

	switch (GbuInfo.LeftBayStat)
	{
	  case    MSL_STBY: String = "STBY"; break;
	  case    MSL_RDY : String = "RDY";  break;
	  default         : String = ""; break;
	}

	DrawWordAtButton(MpdDetectNum,19,String,(GbuInfo.LeftBayStat == MSL_RDY),TextColor);

	if(GbuInfo.MarkersOn)
	  DrawWordAtButton(MpdDetectNum,18,(GbuInfo.MarkerLight) ? "LGHT" : "DRK",FALSE,TextColor);

	switch (GbuInfo.GuideMethod)
	{
	  case    GBU_DIRECT   : String = "DIR"; break;
	  case    GBU_INDIRECT : String = "IND";  break;
	  default              : String = ""; break;
	}

	DrawWordAtButton(MpdDetectNum,8,String,FALSE,TextColor);

	if(GbuInfo.LaunchMode == GBU_TRANS)
	{
	  DrawRadarSymbol(XOfs + 20,YOfs - 61,FLIR_LEFT_ARROW,ArrowColor,GrBuffFor3D);
	  DrawRadarSymbol(XOfs + 47,YOfs - 61,FLIR_RIGHT_ARROW,ArrowColor,GrBuffFor3D);
	}

 }

//***********************************************************************************************************************************
// *****

void DrawFlirOuterText(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
	char *String;

	int TextColor  = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
	int ArrowColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT]; // JLM may change
	int Index;

	if(FlirInfo.ImageMode == FLIR_WHOT)
    Index = COLOR_NORM_BOLD;
    else
	Index = COLOR_BLACK_HOT;

    int OverlayColor = MpdColors[Mpds[MpdNum].IsMono][Index];

    DrawRadarSymbol(XOfs+1,YOfs-66,FLIR_UP_ARROW,ArrowColor,GrBuffFor3D);
   	DrawRadarSymbol(XOfs+1,YOfs+68,FLIR_DOWN_ARROW,ArrowColor,GrBuffFor3D);
	DrawRadarSymbol(XOfs-68,YOfs-1,FLIR_LEFT_ARROW,ArrowColor,GrBuffFor3D);
	DrawRadarSymbol(XOfs+68,YOfs-1,FLIR_RIGHT_ARROW,ArrowColor,GrBuffFor3D);

    DrawWordAtButton(MpdDetectNum,10,"M",FALSE,TextColor);
    DrawVerticalWordAtLocation(XOfs+64,YOfs+13,"BST",FALSE,TextColor);

   	switch (FlirInfo.ModeToSelect)
	{
	  case FLIR_AA:   String = "A/A";	break;
	  case FLIR_AG:   String = "A/G";	break;
	}

	DrawWordAtButton(MpdDetectNum,5,String,FALSE,TextColor);

	switch (FlirInfo.FOVToUse)
	{
	  case WFOV:  String = "WFOV";  break;
	  case NFOV:  String = "NFOV";  break;
	  case ENFOV: String = "ENFOV"; break;
	}

	DrawWordAtButton(MpdDetectNum,6,String,FALSE,TextColor);

	if(FlirInfo.MaskOn)
	{
      DrawWordAtButton(MpdDetectNum,18,"MASK",FALSE,TextColor);
	}
	else
	{
	  switch (FlirInfo.LaserState)
	  {
	    case FLIR_LSR_SAFE:   String = "SAFE"; break;
	    case FLIR_LSR_ARMED:  String = "ARM";  break;
	    case FLIR_LSR_FIRING: String = "LASE"; break;
  	  }

	   DrawWordAtButton(MpdDetectNum,18,String,FALSE,TextColor);
	}

	DrawWordAtButton(MpdDetectNum,9,"TRK",(FlirInfo.TrackOn),TextColor);

	DrawWordAtButton(MpdDetectNum,8,(FlirInfo.AquireMode == MSL_AUTO) ? "AUTO":"MAN",FALSE,TextColor);

	DrawWordAtButton(MpdDetectNum,15,"DCL",(FlirInfo.DeclutterOn),TextColor);

	DrawWordAtButton(MpdDetectNum,16,(FlirInfo.ShortRangeOn) ? "SRF":"LRF",FALSE,TextColor);

    String = "CDES";
	DrawVerticalWordAtLocation(XOfs-65,YOfs-50, String, (FlirInfo.CdesOn),TextColor);

	String = "WHOT";
	DrawVerticalWordAtLocation(XOfs-65,YOfs+36, String, (FlirInfo.ImageMode == FLIR_WHOT),TextColor);

    if(FlirInfo.LaserState == FLIR_LSR_FIRING)
	{
		if(GetTickCount() - FlirFlashStartTime > 1000)
		{
		   FlirDrawL = !FlirDrawL;
           FlirFlashStartTime = GetTickCount();
		}

	    if(FlirDrawL)
		    DrawRadarSymbol(XOfs+15,YOfs+30,LASE_SYMBOL,OverlayColor,GrBuffFor3D);

		if(FlirInfo.MaskOn)
         	    DrawRadarSymbol(XOfs+23,YOfs+30,MASK_SYMBOL,OverlayColor,GrBuffFor3D);
   	}
	else if(FlirInfo.LaserState == FLIR_LSR_ARMED)
	{
       DrawRadarSymbol(XOfs+15,YOfs+30,LASE_SYMBOL,OverlayColor,GrBuffFor3D);
	}

	if(LaseDesignateStartTime != -1)
	{
      if(GetTickCount() - LaseDesignateStartTime < 3000)
		DrawTextAtLocation(XOfs - 15,YOfs + 33,"DESIGNATE",OverlayColor);
	  else
        LaseDesignateStartTime = -1;
	}

	if(FlirInfo.TrackOn && FlirInfo.TargetObtained && (FlirInfo.TargetType != FLIR_CAM_NO_TARGET))
	{
        int AreaTrack = (FlirInfo.TargetType == FLIR_CAM_LOC_TARGET);
	    DrawTextAtLocation(XOfs+34,YOfs + 49,(AreaTrack) ? "AR-TRK" : "PT-TRK",OverlayColor);
	}

	if(lBombTimer > 0)
	{
      if(FlirInfo.TimpactTimer == -1)
	  {
		FlirInfo.TimpactTimer = lBombTimer;
        FlirInfo.PreTimpactTimer = GameLoopInTicks;
	  }
	}

	//if(lBombFlags & WSO_BOMB_IMPACT)
	if(FlirInfo.TimpactTimer >= 0)
	{
	  int Dt = GameLoopInTicks - FlirInfo.PreTimpactTimer;
	  FlirInfo.PreTimpactTimer = GameLoopInTicks;
      FlirInfo.TimpactTimer -= Dt;
	  int TimeToImpact = (int)((float)FlirInfo.TimpactTimer/1000.0);
	  int Min,Sec;
	  Min = TimeToImpact/60;
      Sec = TimeToImpact % 60;
	  if(Min >= 10)
	  {
        if(Sec >= 10)
		  sprintf(TmpStr,"TIMPACT %d:%d",Min,Sec);
		else
			sprintf(TmpStr,"TIMPACT %d:0%d",Min,Sec);
	  }else
	  {
        if(Sec >= 10)
		  sprintf(TmpStr,"TIMPACT 0%d:%d",Min,Sec);
		else
			sprintf(TmpStr,"TIMPACT 0%d:0%d",Min,Sec);
	  }
      DrawTextAtLocation(XOfs - 19,YOfs - 30,TmpStr,OverlayColor);

	  if(FlirInfo.TimpactTimer <= 0)
		  FlirInfo.TimpactTimer = -1;
	}

 }

//***********************************************************************************************************************************
// *****

int GrabFlirButtonClick(int MpdDetectNum)
{
   if (FlirButtonClickInfo.TargetSelected)
   {
 	 if(MpdDetectNum == FlirButtonClickInfo.MpdDetectNum)
	 {
	   FlirButtonClickInfo.TargetSelected = FALSE;

	   FlirEventInfo.ScrX = FlirButtonClickInfo.ScrX;
	   FlirEventInfo.ScrY = FlirButtonClickInfo.ScrY;

	   return(TRUE);
   	 }
   }

   return(FALSE);
}


#define MAX_FILTERED_FLIR_PLANES_F15 50

int FilteredPlanesForFlirF15[MAX_FILTERED_FLIR_PLANES_F15];
int NumFlirFilteredPlanesF15;

//***********************************************************************************************************************************
// *****

void FilterPlanesForFlirCam()
{
	int Cnt;
	int *FilteredPlanes = &FilteredPlanesForFlirF15[0];
    PlaneParams *P = &Planes[0];
	int Left,Right,Top,Bottom;

	Cnt=0;

	Left   = PlayerPlane->WorldPosition.X + 20.0*NMTOWU;
	Right  = PlayerPlane->WorldPosition.X - 20.0*NMTOWU;
	Top    = PlayerPlane->WorldPosition.Z - 20.0*NMTOWU;
	Bottom = PlayerPlane->WorldPosition.Z + 20.0*NMTOWU;

	while (P <= LastPlane)
	{
	  if( (P->Status & PL_ACTIVE) && (P != PlayerPlane) )
	  {
        if(P->WorldPosition.X > Left)
          if(P->WorldPosition.X < Right)
            if(P->WorldPosition.X > Top)
              if(P->WorldPosition.X < Bottom)
		      {
			    if(Cnt < MAX_FILTERED_FLIR_PLANES_F15)
				{
			      *FilteredPlanes = Cnt;
				  FilteredPlanes++;
				  Cnt++;
		         }
		       }
   	  }

	  P++;
    }

	//NumFlirFilteredPlanes = Cnt;
}

//***********************************************************************************************************************************
// *****

int IsPlaneDetectedByFlirCamF15(FPointDouble *P1, FPointDouble *P2,int PlaneIndex, float *Dist)
{
	double WX,WY,WZ,EX,EY,EZ;
	double DxS,DyS,DzS,DxE,DyE,DzE;
	double DistToEnemy,DistToRay;
	double Dot,Angle;

	if(&Planes[PlaneIndex] == PlayerPlane) return FALSE;

	WX = P1->X;
    WY = P1->Y;
    WZ = P1->Z;

    EX = Planes[PlaneIndex].WorldPosition.X;
    EY = Planes[PlaneIndex].WorldPosition.Y;
    EZ = Planes[PlaneIndex].WorldPosition.Z;

    DxS = P2->X - WX;
    DyS = P2->Y - WY;
    DzS = P2->Z - WZ;

    DistToRay = sqrt(DxS*DxS + DyS*DyS + DzS*DzS);

    DxE = EX - WX;
    DyE = EY - WY;
    DzE = EZ - WZ;

    DistToEnemy = sqrt(DxE*DxE + DyE*DyE + DzE*DzE);

    Dot = DxE*DxS + DyE*DyS + DzE*DzS;

    Angle = RadToDeg(acos(Dot/(DistToRay*DistToEnemy)));

	*Dist = DistToEnemy;

	return( (Angle >= 0) && (Angle <= 2.0) );
}

//***********************************************************************************************************************************
// *****

int GrabPlaneInFlirCamF15(FPointDouble *P1, FPointDouble *P2, int *PlaneIndex, float *Dist)
{
   int *FilteredPlanes = NULL; //&FilteredPlanesForFlir[0];
   float Distance;
   float MinDist = 1000000.0;
   int Found = FALSE;

   FilterPlanesForFlirCam();

   *Dist = 1000000.0;

   int Index = 0; //NumFlirFilteredPlanes;

   while(Index-- > 0)
   {

	 if (IsPlaneDetectedByFlirCamF15(P1,P2,*FilteredPlanes, &Distance) )
	 {
       if(Distance < MinDist)
	   {
		 MinDist = Distance;
		 *Dist   = Distance;
  		 *PlaneIndex = *FilteredPlanes;
		 Found = TRUE;
	   }
	 }

     FilteredPlanes++;

   }

   return(Found);
}

extern MovingVehicleParams *FilteredVehiclesf15[100];
extern int NumFilteredVehicles;

//***********************************************************************************************************************************
// *****

int GetCloseVehicleOrObjectF15(FPointDouble *Loc,BasicInstance **GroundObject,MovingVehicleParams **Veh,int *Type)
{
  MovingVehicleParams *VehiclePtr = MovingVehicles;
  float Left,Right,Top,Bottom,Dist;
  float VehMinDist = 1000000;

  *Veh = NULL;

  // get vehicles in box
  Dist   = 5000.0*FTTOWU;
  Left   = Loc->X - Dist;
  Right  = Loc->X + Dist;
  Top    = Loc->Z - Dist;
  Bottom = Loc->Z + Dist;

  while(VehiclePtr <= LastMovingVehicle)
  {
    if(VehiclePtr->Status & VL_ACTIVE)
	{
      if(!(VehiclePtr->Status & VL_INVISIBLE))
	  {
         if(VehiclePtr->WorldPosition.X > Left)
          if(VehiclePtr->WorldPosition.X < Right)
           if(VehiclePtr->WorldPosition.Z < Bottom)
            if(VehiclePtr->WorldPosition.Z > Top)
		    {
			   Dist = *Loc / VehiclePtr->WorldPosition;
			   if(Dist < VehMinDist)
			   {
				  VehMinDist = Dist;
                  *Veh = VehiclePtr;
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
 			     *GroundObject = binst;
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

//***********************************************************************************************************************************
// *****

void FilterVehiclesForFlirCam()
{
  MovingVehicleParams *VehiclePtr = MovingVehicles;
  MovingVehicleParams **FilteredVehiclePtr = NULL; //&FilteredVehicles[0];
  float Left,Right,Top,Bottom,Dist;

// filter out unnecessary objects using a bounding box the size of the radar range
  // 1000 is to give so extra room
  NumFilteredVehicles = 0;
  Dist   = 20.0*NMTOWU + 1000.0;
  Left   = PlayerPlane->WorldPosition.X - Dist;
  Right  = PlayerPlane->WorldPosition.X + Dist;
  Top    = PlayerPlane->WorldPosition.Z - Dist;
  Bottom = PlayerPlane->WorldPosition.Z + Dist;

  while(VehiclePtr <= LastMovingVehicle)
  {
    if(VehiclePtr->Status & VL_ACTIVE)
	{
      if(!(VehiclePtr->Status & VL_INVISIBLE))
	  {
         if(VehiclePtr->WorldPosition.X > Left)
          if(VehiclePtr->WorldPosition.X < Right)
           if(VehiclePtr->WorldPosition.Z < Bottom)
            if(VehiclePtr->WorldPosition.Z > Top)
		    {
               *FilteredVehiclePtr = VehiclePtr;
			    NumFilteredVehicles++;
                FilteredVehiclePtr++;
		    }
	   }
	 }

	 VehiclePtr++;
   }

}

//***********************************************************************************************************************************
// *****

int GetFlirCamVehicleF15(FPointDouble *P1,FPointDouble *P2,MovingVehicleParams **Target,float *Dist)
{
   float MinVehDist = 1000000000.0;
   int ObjFound = FALSE;
   MovingVehicleParams *VehiclePtr = MovingVehicles;
   MovingVehicleParams **FilteredVehiclePtr = NULL; //&FilteredVehicles[0];
   int InsideBoundingBox;
   FPointDouble PointAlongBeam;

   FilterVehiclesForFlirCam();

   *Dist = 1000000.0;

   FilteredVehiclePtr = NULL; //&FilteredVehicles[0];

   float Dx = P2->X - P1->X;
   float Dy = P2->Y - P1->Y;
   float Dz = P2->Z - P1->Z;
   float BeamDist = hypot((Dx),(Dy),(Dz));

   int   Index = NumFilteredVehicles;
   float Left,Right,Top,Bottom,Height;

   while(Index-- > 0)
   {
     FPointDouble VehPos;

	 VehPos = (*FilteredVehiclePtr)->WorldPosition;

	 float DistToVeh = hypot((P1->X - VehPos.X),(P1->Y - VehPos.Y),(P1->Z - VehPos.Z));

	 float BeamFrac = DistToVeh/BeamDist;

	 PointAlongBeam.X = P1->X + Dx*BeamFrac;
	 PointAlongBeam.Y = P1->Y + Dy*BeamFrac;
     PointAlongBeam.Z = P1->Z + Dz*BeamFrac;

     InsideBoundingBox = FALSE;

	 Left    = VehPos.X - 25.0;
     Right   = VehPos.X + 25.0;
     Top     = VehPos.Z - 25.0;
     Bottom  = VehPos.Z + 25.0;
	 Height  = VehPos.Y + 25.0;

	 if(PointAlongBeam.X > Left)
	   if(PointAlongBeam.X < Right)
	     if(PointAlongBeam.Z > Top)
		   if(PointAlongBeam.Z < Bottom)
		     if(PointAlongBeam.Y < Height)
		       InsideBoundingBox = TRUE;

	 if(InsideBoundingBox)
	 {
        if(DistToVeh < MinVehDist)
		{
		  *Target = *FilteredVehiclePtr;
		  *Dist   = DistToVeh;
          MinVehDist = DistToVeh;
		  ObjFound = TRUE;
		}
	 }

	 FilteredVehiclePtr++;
   }

   return(ObjFound);
}

//******************************************************************************************
//****

int IsPointInsideObjBoundBoxF15(FPointDouble *P, RegisteredObject *Obj)
{
  if(P->X >= Obj->Instance->Position.X + Obj->ExtentLeft - 20)
    if(P->X <= Obj->Instance->Position.X + Obj->ExtentRight + 20)
	  if(P->Z >= Obj->Instance->Position.Z + Obj->ExtentTop - 20)
	    if(P->Z <= Obj->Instance->Position.Z + Obj->ExtentBottom + 20)
		  if(P->Y <= Obj->Instance->Position.Y + Obj->ExtentHeight + 20)
		      return(TRUE);

  return(FALSE);

}

//***********************************************************************************
BOOL DoesLineIntersectPlane(FPointDouble *P1,FPointDouble *P2, FPointDouble *PointOnPlane, FPointDouble *Norm, double *TVal, FPointDouble *Intersect)
{
	double t;
	double denom,numer;
	BOOL return_val;
	FPointDouble W;
	FPointDouble FromPlaneToOrigin;

	W.MakeVectorToFrom(*P2,*P1);

	denom = W*(*Norm);

	if (denom == 0.0)
	{
		if (TVal)
			*TVal =	1.7976931348623158E308;
		return_val = FALSE;
	}
	else
	{
		FromPlaneToOrigin.MakeVectorToFrom(*PointOnPlane,*P1);

		numer = FromPlaneToOrigin*(*Norm);

		t =  numer / denom;

		return_val = (BOOL) ((t >= 0.0) && (t <= 1.0));

		if (TVal)
			*TVal = t;

		if (Intersect)
		{
			*Intersect = *P1;
			Intersect->AddScaledVector(t,W);
		}
	}

	return return_val;

}

//******************************************************************************************
//****

int DoesBeamIntersectBoundingBoxF15(FPointDouble *Start, FPointDouble *End, RegisteredObject *Obj,FPointDouble *Intersect)
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
     if(IsPointInsideObjBoundBoxF15(&TempIntersect,Obj))
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
     if(IsPointInsideObjBoundBoxF15(&TempIntersect,Obj))
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
     if(IsPointInsideObjBoundBoxF15(&TempIntersect,Obj))
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
     if(IsPointInsideObjBoundBoxF15(&TempIntersect,Obj))
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
     if(IsPointInsideObjBoundBoxF15(&TempIntersect,Obj))
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

//******************************************************************************************
//****

int DoesBeamIntersectBoundingBoxRotatedF15(FPointDouble *Start, FPointDouble *End, RegisteredObject *Obj,FPointDouble *Intersect)
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
	     if(IntersectBoundingBox(Obj->Matrix,instance->Position,object_type->XYZExtentHigh,object_type->XYZExtentLow,*Start,*End,Intersect))
	     {
	       return(TRUE);
	     }
	   }
  }

  return(FALSE);
}

//******************************************************************************************
//****

int GetFlirCamTargetF15(FPointDouble *P1,FPointDouble *P2,BasicInstance **Target, float *Dist,FPointDouble *Intersect)
{
   RegisteredObject *Rwalker;
   float MinTargetDist = 1000000000.0;
   BasicInstance *binst;
   int ObjFound = FALSE;
   FPointDouble TempIntersect;

   FPointDouble PointAlongBeam;

   Rwalker = FirstInRangeObject;

   *Dist = 1000000.0;

   float Dx = P2->X - P1->X;
   float Dy = P2->Y - P1->Y;
   float Dz = P2->Z - P1->Z;

   float BeamDist = hypot(Dx,Dy,Dz);

   while(Rwalker)
   {
     if (Rwalker->Flags & RO_WAS_DRAWN)
     {
       if(Rwalker->DistanceFromCamera <= BeamDist)
			 {
		     if( DoesBeamIntersectBoundingBoxRotatedF15(P1,P2,Rwalker,&TempIntersect) )
				 {
            if(Rwalker->DistanceFromCamera < MinTargetDist)
						{
			   				binst = Rwalker->Instance;
			   				*Target = binst;
         				MinTargetDist = Rwalker->DistanceFromCamera;
			   				*Dist = Rwalker->DistanceFromCamera;
			   				ObjFound = TRUE;
			   				*Intersect = TempIntersect;
						}
				 }
			 }
   	 }

     Rwalker = Rwalker->NextObject;
   }

   return(ObjFound);
}

//******************************************************************************************
//****
#define FLIR_RANGE_BAR_LENGTH 104.0

void DrawMavGbuRangeMarkers(int MpdNum,CamInfoType *Cam,int XOfs,int YOfs,float MinRange,float MaxRange)
{
  int Color;

  if(Mpds[MpdNum].IsMono)
    Color = (Cam->MarkerLight) ? 63 : 56;
  else
	Color = (Cam->MarkerLight) ? 253 : 56;

  if(Cam->TargetObtained)
  {
	FPointDouble Target;
	float DistFromPlaneToTarget,VelToTarget,SecToMin,SecToMax,TimeToTarget;
	float Dx,Dy,Dz;

    GetCamTargetPosition(Cam,&Target);

	Dx = Target.X - PlayerPlane->WorldPosition.X;
    Dy = Target.Y - PlayerPlane->WorldPosition.Y;
	Dz = Target.Z - PlayerPlane->WorldPosition.Z;

	DistFromPlaneToTarget = hypot(Dx,Dz);

    VelToTarget = AGClosureRate(PlayerPlane,Target);
	VelToTarget *= NMTOFT;
	VelToTarget /= 3600;
    VelToTarget *= FTTOWU;

	SecToMin     = (DistFromPlaneToTarget - MinRange*FTTOWU)/VelToTarget;
	SecToMax     = (DistFromPlaneToTarget - MaxRange*FTTOWU)/VelToTarget;
	TimeToTarget = DistFromPlaneToTarget/VelToTarget;

	if(SecToMin < 0 ) SecToMin = 0;
    if(SecToMax < 0 ) SecToMax = 0;

	Cam->MarkersOn = TRUE;

    float BarStart = YOfs - 53;
    float MaxYPos,MinYPos;
    int XPos = XOfs + 54;

      MinYPos = BarStart + 56.0*FLIR_RANGE_BAR_LENGTH/60.0;
      GrDrawLine(GrBuffFor3D,XPos,MinYPos,XPos+3,MinYPos,Color);
      GrDrawLine(GrBuffFor3D,XPos,MinYPos+1,XPos+3,MinYPos+1,Color);
	  sprintf(TmpStr,"%d",(int)SecToMin);
	  GrDrawString(GrBuffFor3D,SmHUDFont,XPos-12,MinYPos,TmpStr,Color);

      MaxYPos = BarStart + 6.0*FLIR_RANGE_BAR_LENGTH/60.0;
      GrDrawLine(GrBuffFor3D,XPos,MaxYPos,XPos+3,MaxYPos,Color);
      GrDrawLine(GrBuffFor3D,XPos,MaxYPos+1,XPos+3,MaxYPos+1,Color);
      sprintf(TmpStr,"%d",(int)SecToMax);
	  GrDrawString(GrBuffFor3D,SmHUDFont,XPos-12,MaxYPos,TmpStr,Color);


	if( (SecToMax <= 0)  && (SecToMin > 0))
    {
	  DrawVerticalWordAtLocation(XPos,MaxYPos + 29,"IN RANGE",FALSE,Color);
    }

  }

}

//******************************************************************************************
//****

void SetFlirLaseToSafe()
{
   FlirInfo.LaserState = FLIR_LSR_SAFE;
}

//******************************************************************************************
//****

int IsGbuSteerable()
{
  return( (GbuInfo.LaunchMode == GBU_TERM) && (!GbuInfo.TrackOn) );
}

/* -----------------5/26/99 11:38AM--------------------------------------------------------------------
 * Draw FLIR in HUD
 * ----------------------------------------------------------------------------------------------------*/

void DrawLimitedViewMatrixAndPlane(GrBuff *buffer,FPointDouble &eye,FMatrix &matrix,ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot,BOOL high_detail_only,BOOL draw_textures,int dot,void *weap,void *plane);
extern GrBuffPolygon  MpdPolys[];
extern AvionicsType Av;
extern FPointDouble NavFlirOffset;

void DrawHudFlir(FMatrix &mat,BOOL white_hot)
{

	FPoint dummy;
	FPoint *dwalk;
	float left,right,top,bottom;
	float sx,sy;
	FPointDouble eye;

	float old_bright;
	float old_contrast;

	old_contrast = FLIRContrast;
	old_bright = FLIRBrightness;

	FLIRContrast = ((float)Av.NavFlir.Contrast)*0.2;
	FLIRBrightness = ((float)Av.NavFlir.Brightness)*0.05;

   	PushClipRegion();

	//calc a mid pt for the displays
	GrBuffFor3D->ClipLeft = 0;
	GrBuffFor3D->ClipRight = GrBuffFor3D->ClipLeft + (int)(MpdPolys[6].Width2D - 1.0f);

	GrBuffFor3D->ClipTop = 0;
	GrBuffFor3D->ClipBottom = GrBuffFor3D->ClipTop + (int)(MpdPolys[6].Height2D - 1.0f);

	GrBuffFor3D->MidX = (GrBuffFor3D->ClipLeft + GrBuffFor3D->ClipRight) / 2;
	GrBuffFor3D->MidY = GrBuffFor3D->ClipTop + (int)(MpdPolys[6].Points[0].Y*MpdPolys[6].Height2D/(MpdPolys[6].Points[0].Y - MpdPolys[6].Points[2].Y));

	SetSecondPass( TRUE, NULL );

	eye = Camera1.CameraLocation;//PlayerPlane->WorldPosition;

	//eye = PlayerPlane->WorldPosition;
	//eye.AddScaledVector(NavFlirOffset.X,PlayerPlane->Orientation.J);
	//eye.AddScaledVector(-NavFlirOffset.Y,PlayerPlane->Orientation.K);
	//eye.AddScaledVector(-NavFlirOffset.Z,PlayerPlane->Orientation.I);

	DrawLimitedViewMatrixAndPlane(GrBuffFor3D,eye,PlayerPlane->Attitude,0x614,TRUE,TRUE,!white_hot,TRUE,TRUE,0,NULL,PlayerPlane); //(BOOL)(UFC.NavFlirStatus != 1)

	SetSecondPass( FALSE, NULL);

	int i = 4;
	dwalk = &MpdPolys[6].Points[0];
	left = 800.0f;
	right = -1.0f;
	top = 600.0f;
	bottom = -1.0f;
	while(i--)
	{
		dummy = *dwalk++;
		if (dummy.RotateAndPerspect(&sx,&sy,&mat))
		{
			if (sx < 0.0f)
				sx = 0.0f;
			if (sx > (float)(ScreenSize.cx-1))
				sx = (float)(ScreenSize.cx-1);

			if (sy < 0.0f)
				sy = 0.0f;
			if (sy > (float)(ScreenSize.cy-1))
				sy = (float)(ScreenSize.cy-1);

			if (sx < left)
				left = sx;
			if (sx > right)
				right = sx;

			if (sy < top)
				top=sy;
			if (sy > bottom)
				bottom = sy;
		}
	}


	D3DRECT rect;

	rect.lX1 = left;
	rect.lY1 = top;

	rect.lX2 = right+1.0f;
	rect.lY2 = bottom+1.0f;


	lpD3DViewport->Clear2(1,&rect, D3DCLEAR_ZBUFFER ,0,1.0f,0);

	FLIRContrast = old_contrast;
	FLIRBrightness = old_bright;

  	PopClipRegion();
	Set3DScreenClip();
  	PushClipRegion();

	GrBuffPolygon *pOldpoly =		GrBuffFor3D->pGrBuffPolygon;
	GrBuffFor3D->pGrBuffPolygon = NULL;
	if( UFC.NavFlirStatus)
			SuperImposeSymbology();
	GrBuffFor3D->pGrBuffPolygon = pOldpoly;
}