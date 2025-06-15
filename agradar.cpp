
#include "F18.h"
#include "gamesettings.h"

//***********************************************************************************************************************************
// *****    DEFINES, GLOBALS, TYPES -- WILL BE MOVED TO .H

typedef struct
{
  FPoint BoxLoc;
  FPoint TgtLoc;
  int BoxOn;
  int BoxSizeIndex;
  int FreezeOn;
  FPoint FreezePlaneLoc;
  float FreezePlaneAzim;
  float CursAzim;
  float CursRange;
  int Timer;
  int Duration;
  FPointDouble AGDesignate;
} AGGeneralInfoType;

typedef struct
{
  int    ScanRate;
  FPoint Box[4];
  FPoint WCBox[4];
  float  Range;
  int    TargetExists;
  int    TargetXOff;
  int    TargetYOff;
} AGRadarScanInfoType;

typedef struct
{
  FPoint Box[4];
  FPoint WCBox[4];
  float  Range;
  int    StartTime;
  int    CreatingMap;
  int    SubMode;
  int    MapSizeTableIndex;
  int    PrevMapSizeTableIndex;
  int    TargetExists;
  int    IsMissionTarget;
  int    TargetXOff;
  int    TargetYOff;
} AGRadarMapInfoType;

AGGeneralInfoType  AGInfo;
AGRadarMapInfoType AGMapInfo;
AGRadarScanInfoType AGScanInfo;

TargetButtonClickInfoType AGEventInfo;
int PreviousMapSizeIndex;

//GrBuff *AGRadarBackground;

int    Debug2;

int    AGRadarOverlays[3][2];

int    TempBool;

float  MapSizeTable[8];

AGScanInfoType AGRadarInfo;

MapScanInfoType HiResScan;

// JLM remove
FPointDouble JLMTemp;
float JLMDistance;
float JLMWtoM = WUTONM;


void   DrawOuterBoundry(int XOfs, int YOfs, int MpdNum, int MpdDetectNum);
void   LoadAGRadarOverlayBuff(int Type);
int    GrabAGButtonClick(int MpdDetectNum);
void   DrawErrorMessage(int MpdNum, int XOfs, int YOfs,int ErrorCode);
void   DrawTimer(int MpdNum, int XOfs, int YOfs);
void   DrawWayPointList(int MpdNum, int XOfs, int YOfs);
void   DrawBeamLine(int XOfs, int YOfs);
void   DrawAGBullsEye(int MpdNum, int XOfs, int YOfs);


void DrawScanBeamEdgeTable( void );


// jlm DEBUG
float TempAzim;
int NumDebugInOut = 0;
InOutType DebugInOutList[800];

int ShowTerrain=FALSE;

//***********************************************************************************************************************************
// *****

float PixelsPerMile(float Range)
{
  return(108.0/Range);
}

// JLM remove
int BoxDebug = FALSE;

//***********************************************************************************************************************************
// *****

void DrawBox(int MpdNum,FPoint *P1,FPoint *P2,FPoint *P3,FPoint *P4)
{
  int Color = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
  FPoint TempP1,TempP2,TempP3,TempP4;

  if(BoxDebug)
	 Debug2 = TRUE;

  TempP1 = *P1; TempP2 = *P2;
  GrDrawDashedLineClipped(GrBuffFor3D,TempP1.X, TempP1.Y, TempP2.X, TempP2.Y,Color);
  TempP2 = *P2; TempP3 = *P3;
  GrDrawDashedLineClipped(GrBuffFor3D,TempP2.X, TempP2.Y, TempP3.X, TempP3.Y,Color);
  TempP3 = *P3; TempP4 = *P4;
  GrDrawDashedLineClipped(GrBuffFor3D,TempP3.X, TempP3.Y, TempP4.X, TempP4.Y,Color);
  TempP1 = *P1; TempP4 = *P4;
  GrDrawDashedLineClipped(GrBuffFor3D,TempP4.X, TempP4.Y, TempP1.X, TempP1.Y,Color);
}

//***********************************************************************************************************************************
// *****

void CalcScreenBox(int ScrX, int ScrY, float Size, float PixelsPerMile, FPoint *Box)
{
   Box[0].X = ScrX - 0.5*(Size*PixelsPerMile);  Box[0].Y = ScrY - 0.5*(Size*PixelsPerMile);
   Box[1].X = ScrX + 0.5*(Size*PixelsPerMile);  Box[1].Y = ScrY - 0.5*(Size*PixelsPerMile);
   Box[2].X = ScrX + 0.5*(Size*PixelsPerMile);  Box[2].Y = ScrY + 0.5*(Size*PixelsPerMile);
   Box[3].X = ScrX - 0.5*(Size*PixelsPerMile);  Box[3].Y = ScrY + 0.5*(Size*PixelsPerMile);
}

//***********************************************************************************************************************************
// *****

void CalcScreenBoxInWorldUnits(int WorldX, int WorldY, float Size, FPoint *Box)
{
   Box[0].X = WorldX - 0.5*(Size);  Box[0].Z = WorldY  - 0.5*(Size);   Box[0].Y = 0;
   Box[1].X = WorldX + 0.5*(Size);  Box[1].Z = WorldY  - 0.5*(Size);   Box[1].Y = 0;
   Box[2].X = WorldX + 0.5*(Size);  Box[2].Z = WorldY + 0.5*(Size);    Box[2].Y = 0;
   Box[3].X = WorldX - 0.5*(Size);  Box[3].Z = WorldY  + 0.5*(Size);   Box[3].Y = 0;
}

//***********************************************************************************************************************************
// *****

void CalcBoxInWorldCoords(FPoint Origin, float Size, FPoint *Box)
{
    FPoint *WCBoxPtr = &AGMapInfo.WCBox[0];
	for(int i=0; i<4; i++)
	{
      Box->X = WCBoxPtr->X*NMTOWU;
      Box->Y = WCBoxPtr->Y*NMTOWU;
      Box->Z = WCBoxPtr->Z*NMTOWU;

	  Box++;
	  WCBoxPtr++;
    }
 }

//***********************************************************************************************************************************
// *****

int MpdToUse(int ThisMpd)
{

  return(0);
}

//***********************************************************************************************************************************
// *****

void MapBoxSizeToRanges(float Size, float *Min, float *Max)
{
  switch(int(Size))
  {
    case 0 : *Min = 3.0;  *Max = 20.0;  break;
    case 1 : *Min = 3.4;  *Max = 39.3;  break;
    case 3 : *Min = 4.4;  *Max = 48.7;  break;
    case 4 : *Min = 5.2;  *Max = 77.7;  break;
    case 10: *Min = 11.0; *Max = 155.0; break;
    case 20: *Min = 22.0; *Max = 150.0; break;
    case 40: *Min = 44.0; *Max = 140.0; break;
    case 80: *Min = 88.0; *Max = 120.0; break;
  }
}

//***********************************************************************************************************************************
// *****

float CalcRelAngle(float X, float Y, float OX, float OY)
{
  float Dx,Dz;
  float Angle;

  Dx = X - OX;
  Dz = -(Y - OY);
  Angle = fabs(atan(Dz/Dx));
  Angle = RadToDeg(Angle);
  if(Dx < 0)
    Angle = 90.0 - Angle;
  else
    Angle = Angle + -90.0;

  return(Angle);

}

//***********************************************************************************************************************************
// *****

float CalcCursRange(float X, float Y, float OX, float OY)
{
  float Dx,Dz;

  Dx = X - OX;
  Dz = -(Y - OY);

  return(sqrt(Dx*Dx + Dz*Dz));

}

//***********************************************************************************************************************************
// *****

float CalcCursAzim(POINT CursPos, FPoint Origin)
{
  float Azim;

  Azim = CalcRelAngle(CursPos.x,CursPos.y,Origin.X,Origin.Y);
  Azim = NormDegree(360.0 - NormDegree(PlayerPlane->Heading/DEGREE + Azim));

  return(Azim);

}

//***********************************************************************************************************************************
// *****

int GetTimeBasedOnAzim(POINT CursPos, FPoint Origin)
{
  int Time;
  float Azim;

  Azim = fabs(CalcRelAngle(CursPos.x,CursPos.y,Origin.X,Origin.Y));

  if( (Azim <= 50) && (Azim >= 30) )
	Time = 3000;
  else if( (Azim < 30) && (Azim > 8) )
	Time = 3000 + 3000*((30 - Azim)/22.0);
  else
	Time = 6000;

  return(Time);
}

//***********************************************************************************************************************************
// *****

void AGErrorProcessing(int MpdNum, POINT CursPos, FPoint Origin, FPoint *Box, int *BoxInCone,int *ErrorCode)
{

  float Dx,Dz;
  float Angle;
  float Dist;
  float Min,Max;
  float Limit;

  *ErrorCode = NO_AG_ERROR;
  *BoxInCone = TRUE;

  Dx = CursPos.x - Origin.X;
  Dz = -(CursPos.y - Origin.Y);
  Dist = sqrt(Dx*Dx + Dz*Dz);

  Angle = CalcRelAngle(CursPos.x,CursPos.y,Origin.X,Origin.Y);

  if(AGScanInfo.ScanRate == FULL)
	Limit = 50.0;
  else if(AGScanInfo.ScanRate == HALF)
	Limit = 25.0;
  else if(AGScanInfo.ScanRate == QUART)
	Limit = 12.5;

  if( (Angle > Limit) || (Angle < -Limit) )
  {
	 *ErrorCode	= GIMBLE_LIMIT;
	 *BoxInCone = FALSE;
  }

  if( (Angle > -8.0) && (Angle < 8.0) )
  {
     *ErrorCode	= BLIND_ZONE;
	 *BoxInCone = FALSE;
  }

  MapBoxSizeToRanges(MapSizeTable[AGMapInfo.MapSizeTableIndex], &Min, &Max);

  if((Dist*(1/PixelsPerMile(AGScanInfo.Range)) < Min) ||
	       (Dist*(1/PixelsPerMile(AGScanInfo.Range)) > Max))
  {
	*ErrorCode	= DW_RANGE_LIMIT;
	*BoxInCone = FALSE;
  }

  // see if box is inside the cone
  for(int i=0; i<4; i++)
  {
	Angle = CalcRelAngle(Box[i].X,Box[i].Y,Origin.X,Origin.Y);
	if( (Angle > Limit) || (Angle < -Limit) )
	{
	   *ErrorCode	= GIMBLE_LIMIT;
	   *BoxInCone   = FALSE;
	   break;
	}
  }

}

//***********************************************************************************************************************************
// *****

void DrawHeights()
{
  InOutType *Walker = &DebugInOutList[1];
  int XPos1,YPos1, XPos2, YPos2;
  int Color;

  if(NumDebugInOut == 0)
	return;

  GrFillRectNoClip(GrBuffFor3D, 0, 0, 639, 479,	56);

  GrDrawLine(GrBuffFor3D, 0, 400, 639, 400, 60);

  while(Walker <= &DebugInOutList[NumDebugInOut-2])
  {
	  if((Walker-1)->InOut == NOT_OCCLUDED)
		Color = 60;
	  else
		Color = 5;

	  XPos1 = 600*(Walker-1)->T;
	  YPos1 = 400 - ( ((Walker-1)->Height/5000)*300 );

	  if( (YPos1 <= 400) && (YPos1 >= 0) )
	    GrDrawLine(GrBuffFor3D, XPos1, 400, XPos1,YPos1, Color);

	  XPos2 = 600*(Walker)->T;
	  YPos2 = 400 - ( ( (Walker)->Height/5000)*300 );


	   if( (YPos1 <= 400) && (YPos2 <= 400) && (YPos1 >= 0) && (YPos2 >= 0) )
	   {
	     GrDrawLine(GrBuffFor3D, XPos1, YPos1, XPos2,YPos2, Color);
	   }

	Walker++;

  }

  XPos1 = 600*(Walker)->T;
  YPos1 = 400 - ( ((Walker)->Height/5000)*300 );


 /* if( (YPos1 <= 400) && (YPos1 >= 0) )
    GrDrawLine(GrBuffFor3D, XPos1, 400, XPos1,YPos1, 60);
  else
	Debug2 = TRUE;

  if( (YPos1 <= 400) && (YPos2 <= 400) && (YPos1 >= 0) && (YPos2 >= 0) )
    GrDrawLine(GrBuffFor3D, XPos1, YPos1, XPos2,YPos2, 60);
  else
	Debug2 = TRUE;
  */

  GrDrawDot(GrBuffFor3D,0,400 - (PlayerPlane->WorldPosition.Y/5000.0)*300,3);

}

//***********************************************************************************************************************************
// *****

void SetModeOnAGMpds(int Mode)
{
 
}

//***********************************************************************************************************************************
// *****

int GetModeForAGMpds(int MyMpd)
{
  return(0);

}

//***********************************************************************************************************************************
// *****

int MapWorldCoordToScreenCoord(FPoint ToLoc, FPoint Origin, FPoint *ScrLoc)
{
  float Dist,Dx,Dy,Dz;
  float NewX,NewZ;

  float DeltaAzim = AIComputeHeadingToPoint(PlayerPlane,ToLoc, &Dist, &Dx, &Dy, &Dz,1);

  //if( (DeltaAzim > 90) || (DeltaAzim < -90) )
//	  return(FALSE);

  Dist = sqrt(Dx*Dx + Dz*Dz);
  Dist *= WUTONM;

  ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);

  int NumPixX = PixelsPerMile(Dx*WUTONM);
  int NumPixY = PixelsPerMile(Dz*WUTONM);

  ScrLoc->X = NewX; //Origin.X + NumPixX;
  ScrLoc->Y = NewZ; //Origin.Y - NumPixY;
  ScrLoc->Z  = 0;

  return(TRUE);
}

//***********************************************************************************************************************************
// *****

void GetBoxScreenCoords(FPoint Origin,FPoint *Box,FPoint *WCBox)
{
  FPoint *BoxPtr    = WCBox;
  FPoint *ScrBoxPtr = Box;
  FPoint TempLoc;

  for(int i=0; i<4; i++)
  {
    TempLoc.X = BoxPtr->X;
    TempLoc.Y = BoxPtr->Y;
    TempLoc.Z = BoxPtr->Z;

	TempLoc *= NMTOWU;

    MapWorldCoordToScreenCoord(TempLoc,Origin,ScrBoxPtr);

	ScrBoxPtr++;
    BoxPtr++;

  }
}

float DebugDeg = DEGREE;   // JLM remove

//***********************************************************************************************************************************
// *****

void MapScreenToWorld(FPoint Origin, int ScrX, int ScrY, FPoint *WorldLoc)
{
  float Dx,Dz;
  float Angle;
  float NewX, NewZ, Dist;

  Dx = ScrX - Origin.X;
  Dz = -(ScrY - Origin.Y);
  Angle = fabs(atan(Dz/Dx));
  Angle = RadToDeg(Angle);
  if(Dx < 0)
	Angle = 90.0 - Angle;
  else
    Angle = Angle + -90.0;

  Dist = sqrt(Dx*Dx + Dz*Dz);

  ProjectPoint(PlayerPlane->WorldPosition.X*WUTONM, PlayerPlane->WorldPosition.Z*WUTONM, NormDegree(PlayerPlane->Heading/DEGREE + Angle),Dist*(1.0/PixelsPerMile(AGScanInfo.Range)), &NewX, &NewZ);

  WorldLoc->X = NewX;
  WorldLoc->Y = 0.0;
  WorldLoc->Z = NewZ;

}

//***********************************************************************************************************************************
// *****

void GetBoxWorldCoords(FPoint Origin,FPoint *WCBox,FPoint *Box)
{
  FPoint *WCBoxPtr    = WCBox;
  FPoint *ScrBoxPtr   = Box;

  for(int i=0; i<4; i++)
  {
 	MapScreenToWorld(Origin, ScrBoxPtr->X, ScrBoxPtr->Y, WCBoxPtr);

	ScrBoxPtr++;
    WCBoxPtr++;
  }
}

//***********************************************************************************************************************************
// *****

void GetHiResScanInfo(MapScanInfoType *HiResScanInfo)
{
  HiResScanInfo->Lx      = HiResScan.Lx;
  HiResScanInfo->Lz      = HiResScan.Lz;
  HiResScanInfo->Rx      = HiResScan.Rx;
  HiResScanInfo->Rz      = HiResScan.Rz;
  HiResScanInfo->CurX    = HiResScan.CurX;
  HiResScanInfo->CurZ    = HiResScan.CurZ;

  HiResScanInfo->LDx     = HiResScan.LDx;
  HiResScanInfo->LDz     = HiResScan.LDz;
  HiResScanInfo->RDx     = HiResScan.RDx;
  HiResScanInfo->RDz     = HiResScan.RDz;
  HiResScanInfo->LStepX  = HiResScan.LStepX;
  HiResScanInfo->RStepX  = HiResScan.RStepX;
  HiResScanInfo->LStepZ  = HiResScan.LStepZ;
  HiResScanInfo->RStepZ  = HiResScan.RStepZ;
  HiResScanInfo->Heading = HiResScan.Heading;

}

//***********************************************************************************************************************************
// *****

void GetPosInHiResMap(FPoint Origin, int ScrX, int ScrY, FPoint *NewPos)
{

  FPoint Center;

  //Origin.Y -= 55;

  Center.X = AGInfo.BoxLoc.X*NMTOWU;
  Center.Y = 0;
  Center.Z = AGInfo.BoxLoc.Z*NMTOWU;

  float PercentZ = (float)(ScrY - Origin.Y)/111.0;
  float PercentX = (float)(ScrX - Origin.X)/116.0;

  float Dx = (AGMapInfo.WCBox[1].X*NMTOWU - AGMapInfo.WCBox[0].X*NMTOWU);
  float Dz = (AGMapInfo.WCBox[1].Z*NMTOWU - AGMapInfo.WCBox[0].Z*NMTOWU);

  float LDx = (AGMapInfo.WCBox[3].X - AGMapInfo.WCBox[0].X)*NMTOWU;
  float LDz = (AGMapInfo.WCBox[3].Z - AGMapInfo.WCBox[0].Z)*NMTOWU;

  float MoveX = (Dx)*PercentX;
  float MoveZ = (Dz)*PercentX;

  Center.X += MoveX;
  Center.Y = 0;
  Center.Z += MoveZ;

  MoveX = (LDx)*PercentZ;
  MoveZ = (LDz)*PercentZ;

  Center.X += MoveX;
  Center.Y = 0;
  Center.Z += MoveZ;

  NewPos->X = Center.X;
  NewPos->Z = Center.Z;
  NewPos->Y = LandHeight(NewPos->X,NewPos->Z);
}

//***********************************************************************************************************************************
// *****

void GetPosInHiResMap(int Left, int Top, FPoint Origin, int ScrX, int ScrY, FPointDouble *NewPos)
{

  FPoint LeftTop;

  LeftTop = AGMapInfo.WCBox[0];
  LeftTop *= NMTOWU;

  float PercentZ = (float)(ScrY - Top)/111.0;
  float PercentX = (float)(ScrX - Left)/116.0;

  float Dx = (AGMapInfo.WCBox[1].X*NMTOWU - AGMapInfo.WCBox[0].X*NMTOWU);
  float Dz = (AGMapInfo.WCBox[1].Z*NMTOWU - AGMapInfo.WCBox[0].Z*NMTOWU);

  float LDx = (AGMapInfo.WCBox[3].X - AGMapInfo.WCBox[0].X)*NMTOWU;
  float LDz = (AGMapInfo.WCBox[3].Z - AGMapInfo.WCBox[0].Z)*NMTOWU;

  float MoveX = (Dx)*PercentX;
  float MoveZ = (Dz)*PercentX;

  LeftTop.X += MoveX;
  LeftTop.Y = 0;
  LeftTop.Z += MoveZ;

  MoveX = (LDx)*PercentZ;
  MoveZ = (LDz)*PercentZ;

  LeftTop.X += MoveX;
  LeftTop.Y = 0;
  LeftTop.Z += MoveZ;

  NewPos->X = LeftTop.X;
  NewPos->Z = LeftTop.Z;
  NewPos->Y = LandHeight(NewPos->X,NewPos->Z);

}

//***********************************************************************************************************************************
// *****

void DrawTargetSymbol(int MpdNum, int Left, int Top,FPointDouble TargetLoc,int Symb_Id)
{
  int Sx,Sw;
  int Sy,Sh;
  int SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
  FPoint Box[4];

  if( (g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) && (AGMapInfo.IsMissionTarget) )
	SymbColor = 5;

  Box[0] = AGMapInfo.WCBox[0];
  Box[0] *= NMTOWU;
  Box[1] = AGMapInfo.WCBox[1];
  Box[1] *= NMTOWU;
  Box[2] = AGMapInfo.WCBox[2];
  Box[2] *= NMTOWU;
  Box[3] = AGMapInfo.WCBox[3];
  Box[3] *= NMTOWU;

  double Wx,Ww;
  double Wz,Wh;

  Sx = 0;
  Sw = 116;

  Sy = 0;
  Sh = 111;

  Wx = Box[0].X;
  Ww = sqrt ( (Box[2].X - Box[3].X)*(Box[2].X - Box[3].X) +
	          (Box[2].Z - Box[3].Z)*(Box[2].Z - Box[3].Z) );

  Wz = Box[0].Z;
  Wh = sqrt ( (Box[3].X - Box[0].X)*(Box[3].X - Box[0].X) +
	          (Box[3].Z - Box[0].Z)*(Box[3].Z - Box[0].Z) );

  float Width  = (float)(Sw)/(float)(Ww);
  float Height = (float)(Sh)/(float)(Wh);

  int ScrX,ScrY;

  FPoint DownEdge;
  FPoint RightEdge;
  FPoint TopLeftToLoc;
  FPoint BotLeftToLoc;

  DownEdge.MakeVectorToFrom(Box[3],Box[0]);  DownEdge.Y  = 0;
  RightEdge.MakeVectorToFrom(Box[2],Box[3]); RightEdge.Y = 0;

  DownEdge.Normalize();
  RightEdge.Normalize();

  TopLeftToLoc.MakeVectorToFrom(TargetLoc,Box[0]);
  BotLeftToLoc.MakeVectorToFrom(TargetLoc,Box[3]);

  float DotDownEdge  = DownEdge  * TopLeftToLoc;
  float DotRightEdge = RightEdge * BotLeftToLoc;

  ScrX = Left + (int) floor( (DotRightEdge*Width) + Sx + 0.5f);
  ScrY = Top  + (int) floor( (DotDownEdge*Height) + Sy + 0.5f);

  DrawRadarSymbol(ScrX,ScrY,Symb_Id,SymbColor,GrBuffFor3D);

}

//***********************************************************************************************************************************
// *****

void CalcNewHiResMap(FPoint Origin, int ScrX, int ScrY)
{
  //Origin.Y -= 55; // adjust by putting origin in the middle instead of the bottom

  float PercentZ = (float)(ScrY - Origin.Y)/111.0;
  float PercentX = (float)(ScrX - Origin.X)/116.0;

  float Dx = (AGMapInfo.WCBox[1].X*NMTOWU - AGMapInfo.WCBox[0].X*NMTOWU);
  float Dz = (AGMapInfo.WCBox[1].Z*NMTOWU - AGMapInfo.WCBox[0].Z*NMTOWU);

  float LDx = (AGMapInfo.WCBox[3].X - AGMapInfo.WCBox[0].X)*NMTOWU;
  float LDz = (AGMapInfo.WCBox[3].Z - AGMapInfo.WCBox[0].Z)*NMTOWU;

  float MoveX = (Dx)*PercentX;
  float MoveZ = (Dz)*PercentX;

  for(int i=0; i<4; i++)
  {
    AGMapInfo.WCBox[i].X += MoveX*WUTONM;
    AGMapInfo.WCBox[i].Z += MoveZ*WUTONM;
  }

  MoveX = (LDx)*PercentZ;
  MoveZ = (LDz)*PercentZ;

  for(i=0; i<4; i++)
  {
    AGMapInfo.WCBox[i].X += MoveX*WUTONM;
    AGMapInfo.WCBox[i].Z += MoveZ*WUTONM;
  }

}

//***********************************************************************************************************************************
// *****

void ScaleHiResMap(float PrevRange)
{
  float Factor = (MapSizeTable[AGMapInfo.MapSizeTableIndex]/PrevRange);

  if(Factor < 1.0)
	Factor = (1.0 - Factor)/2.0;
  else
	Factor = -(Factor - 1.0)/2.0;

  float MoveX;
  float MoveZ;

  float Dx = (AGMapInfo.WCBox[1].X*NMTOWU - AGMapInfo.WCBox[0].X*NMTOWU);
  float Dz = (AGMapInfo.WCBox[1].Z*NMTOWU - AGMapInfo.WCBox[0].Z*NMTOWU);

  float LDx = (AGMapInfo.WCBox[3].X - AGMapInfo.WCBox[0].X)*NMTOWU;
  float LDz = (AGMapInfo.WCBox[3].Z - AGMapInfo.WCBox[0].Z)*NMTOWU;

  float RDx = (AGMapInfo.WCBox[2].X - AGMapInfo.WCBox[1].X)*NMTOWU;
  float RDz = (AGMapInfo.WCBox[2].Z - AGMapInfo.WCBox[1].Z)*NMTOWU;

  MoveX = Dx*Factor;
  MoveZ = Dz*Factor;

  AGMapInfo.WCBox[0].X += MoveX*WUTONM;
  AGMapInfo.WCBox[0].Z += MoveZ*WUTONM;
   MoveX = (LDx)*Factor;
  MoveZ = (LDz)*Factor;
  AGMapInfo.WCBox[0].X += MoveX*WUTONM;
  AGMapInfo.WCBox[0].Z += MoveZ*WUTONM;

  MoveX = Dx*-Factor;
  MoveZ = Dz*-Factor;
  AGMapInfo.WCBox[1].X += MoveX*WUTONM;
  AGMapInfo.WCBox[1].Z += MoveZ*WUTONM;
  MoveX = (RDx)*Factor;
  MoveZ = (RDz)*Factor;
  AGMapInfo.WCBox[1].X += MoveX*WUTONM;
  AGMapInfo.WCBox[1].Z += MoveZ*WUTONM;

  MoveX = Dx*-Factor;
  MoveZ = Dz*-Factor;
  AGMapInfo.WCBox[2].X += MoveX*WUTONM;
  AGMapInfo.WCBox[2].Z += MoveZ*WUTONM;
  MoveX = (RDx)*-Factor;
  MoveZ = (RDz)*-Factor;
  AGMapInfo.WCBox[2].X += MoveX*WUTONM;
  AGMapInfo.WCBox[2].Z += MoveZ*WUTONM;

  MoveX = Dx*Factor;
  MoveZ = Dz*Factor;
  AGMapInfo.WCBox[3].X += MoveX*WUTONM;
  AGMapInfo.WCBox[3].Z += MoveZ*WUTONM;
  MoveX = (LDx)*-Factor;
  MoveZ = (LDz)*-Factor;
  AGMapInfo.WCBox[3].X += MoveX*WUTONM;
  AGMapInfo.WCBox[3].Z += MoveZ*WUTONM;

}

//***********************************************************************************************************************************
// *****

void SetHiResMapScanInfo()
{
  HiResScan.Lx = AGMapInfo.WCBox[0].X*NMTOWU;
  HiResScan.Lz = AGMapInfo.WCBox[0].Z*NMTOWU;
  HiResScan.Rx = AGMapInfo.WCBox[1].X*NMTOWU;
  HiResScan.Rz = AGMapInfo.WCBox[1].Z*NMTOWU;

  HiResScan.CurX = HiResScan.Lx;
  HiResScan.CurZ = HiResScan.Lz;

  HiResScan.LDx = (AGMapInfo.WCBox[3].X - AGMapInfo.WCBox[0].X)*NMTOWU;
  HiResScan.LDz = (AGMapInfo.WCBox[3].Z - AGMapInfo.WCBox[0].Z)*NMTOWU;

  HiResScan.RDx = (AGMapInfo.WCBox[2].X - AGMapInfo.WCBox[1].X)*NMTOWU;
  HiResScan.RDz = (AGMapInfo.WCBox[2].Z - AGMapInfo.WCBox[1].Z)*NMTOWU;

  HiResScan.LStepX = HiResScan.LDx/MAP_LOS_SIZE;
  HiResScan.RStepX = HiResScan.RDx/MAP_LOS_SIZE;

  HiResScan.LStepZ = HiResScan.LDz/MAP_LOS_SIZE;
  HiResScan.RStepZ = HiResScan.RDz/MAP_LOS_SIZE;

}

//***********************************************************************************************************************************
// *****

void ProcessTargetChange()
{
 if( (AGInfo.AGDesignate.X != PlayerPlane->AGDesignate.X) ||
     (AGInfo.AGDesignate.Y != PlayerPlane->AGDesignate.Y) ||
     (AGInfo.AGDesignate.Z != PlayerPlane->AGDesignate.Z)    )
 {
   AGInfo.TgtLoc      = PlayerPlane->AGDesignate;
   AGInfo.AGDesignate = PlayerPlane->AGDesignate;
   AGMapInfo.TargetExists = TRUE;
  }

}

//***********************************************************************************************************************************
// ***** EASY MODE AG

FPointDouble EasyModePrevTargLoc;
FPointDouble TargLoc;
int EasyModeSelectTarget = FALSE;
int EasyModeResetTarget  = FALSE;

//***********************************************************************************************************************************

void EasyModeAGChangeTarget(void)
{
  EasyModeSelectTarget = TRUE;
}

//***********************************************************************************************************************************

void EasyModeAGResetTarget(void)
{
  EasyModeResetTarget = TRUE;
}

//***********************************************************************************************************************************

void EasyModeSetAGTarget(int MpdNum, FPoint Origin)
{

}

//***********************************************************************************************************************************
// *****

 void EasyModeRBMMode(int XOfs,int YOfs,int MpdNum,int MpdDetectNum)
 {
   int ButtonNum,Pressed;
   POINT CursPos;
   FPoint Origin;
   FPoint PlanePos;
   FPoint CursBoxWorldLoc;

   float Dx,Dz;
   FPoint MapPos;
   float Angle;
   float NewX, NewZ, Dist;

   Origin.X = XOfs;//-(116/2) + 57;
   Origin.Y = YOfs;//-(111/2) + 109;

   PlanePos.X = (PlayerPlane->WorldPosition.X*WUTONM);
   PlanePos.Z = (PlayerPlane->WorldPosition.Z*WUTONM);

   if(AGMapInfo.TargetExists)
   {
     PlayerPlane->AGDesignate.X = AGInfo.TgtLoc.X;
     PlayerPlane->AGDesignate.Y = AGInfo.TgtLoc.Y;
     PlayerPlane->AGDesignate.Z = AGInfo.TgtLoc.Z;
   }

 //  PushClipRegion();
//   SetClipRegion(XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2,YOfs-(111/2)+1,	YOfs-(111/2) + 111 - 2, YOfs, XOfs);

   if(UFC.EMISState || SniffOn)
   {
	   if(Mpds[MpdNum].AGInfo.CurMode != GMT_MODE)
	      GrFillRectNoClip(AGScanBuffer, 0, 0, 116, 111, 56);
	   else
	      GrFillRectNoClip(GMTAGScanBuffer, 0, 0, 116, 111, 56);
   }


   if(Mpds[MpdNum].AGInfo.CurMode != GMT_MODE)
      GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGScanBuffer, 0, 0, 116,111);
   else
      GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), GMTAGScanBuffer, 0, 0, 116,111);

   	//Render All valid edges in the scanbeamedge table
   DrawScanBeamEdgeTable();

   DrawBeamLine(XOfs,YOfs);

 //  GrCopyRectMaskNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGRadarBackground, 0, 0, 116,111);
	GrDrawLine(GrBuffFor3D, 1,  111/2-1      ,   4, 111/2-1      ,   60);
	GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 1*3,   2, 111/2-1 + 1*3,   60);
    GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 2*3,   2, 111/2-1 + 2*3,   60);
    GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 5*3,   2, 111/2-1 + 5*3,   60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + 10*3,   4, 111/2-1 + 10*3,  60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -1*3,   2, 111/2-1 + -1*3,  60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -2*3,   2, 111/2-1 + -2*3,  60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -5*3,   2, 111/2-1 + -5*3,  60);
    GrDrawLine(GrBuffFor3D, 1,111/2-1 + -10*3,   4, 111/2-1 + -10*3, 60);

    GrDrawLine(GrBuffFor3D,   116/2-1 - 46, 110, 116/2-1 - 46, 105, 60);
	GrDrawLine(GrBuffFor3D,   116/2-1 + 46, 110, 116/2-1 + 46, 105, 60);

   int MapModeOn = (Mpds[MpdNum].AGInfo.SelectMode == MAP_MODE);

   if(AGInfo.BoxOn)
   {
	  float DeltaAzim,Dist,NewX, NewZ;

	  float dist,delx,dely,delz;
	  FPoint Point;
	  Point.X = (AGInfo.BoxLoc.X*NMTOWU);
	  Point.Y = (AGInfo.BoxLoc.Y*NMTOWU);
      Point.Z = (AGInfo.BoxLoc.Z*NMTOWU);

	  if(!AGInfo.FreezeOn)
      {
	    Dist = sqrt( (PlanePos.X - AGInfo.BoxLoc.X)*(PlanePos.X - AGInfo.BoxLoc.X) +
			       (PlanePos.Z - AGInfo.BoxLoc.Z)*(PlanePos.Z - AGInfo.BoxLoc.Z)   );
	    DeltaAzim = AIComputeHeadingToPoint(PlayerPlane, Point, &dist, &delx, &dely, &delz, 1);
	    ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);
      }
	  else
	  {
	    Dist = sqrt( (AGInfo.FreezePlaneLoc.X*WUTONM - AGInfo.BoxLoc.X)*(AGInfo.FreezePlaneLoc.X*WUTONM - AGInfo.BoxLoc.X) +
			         (AGInfo.FreezePlaneLoc.Z*WUTONM - AGInfo.BoxLoc.Z)*(AGInfo.FreezePlaneLoc.Z*WUTONM - AGInfo.BoxLoc.Z)   );
	    DeltaAzim = ComputeHeadingToPoint(AGInfo.FreezePlaneLoc, AGInfo.FreezePlaneAzim*DEGREE, Point, 1);
	    ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);
	  }

      GetBoxScreenCoords(Origin,&AGScanInfo.Box[0],&AGMapInfo.WCBox[0]);
	  DrawBox(MpdNum, &AGScanInfo.Box[0],&AGScanInfo.Box[1],&AGScanInfo.Box[2],&AGScanInfo.Box[3]);
   }

   if(EasyModeSelectTarget || EasyModeResetTarget)
   {
     int CanShow = TRUE;

        CanShow = FALSE;

	 if(CanShow)
	 {
       EasyModeSetAGTarget(MpdNum,Origin);
       EasyModeSelectTarget = FALSE;
	 }

   }

   if(!MapModeOn)
   {
	 GetCursorPos(&CursPos);

     if( (IsPointInside(CursPos.x, CursPos.y, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) ) )
     {

	   AGInfo.CursAzim  = CalcCursAzim(CursPos,Origin);
	   AGInfo.CursRange = CalcCursRange(CursPos.x,CursPos.y,Origin.X,Origin.Y)*
		                  (1/PixelsPerMile(AGScanInfo.Range));

       if(GrabAGButtonClick(MpdDetectNum))
       {
         if (IsPointInside(AGEventInfo.ScrX, AGEventInfo.ScrY, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
         {
           Dx = AGEventInfo.ScrX - Origin.X;
		   Dz = -(AGEventInfo.ScrY - Origin.Y);
		   Angle = fabs(atan(Dz/Dx));
		   Angle = RadToDeg(Angle);
		   if(Dx < 0)
		    Angle = 90.0 - Angle;
		   else
		    Angle = Angle + -90.0;

		   Dist = sqrt(Dx*Dx + Dz*Dz);

		   if(!AGInfo.FreezeOn)
   		     ProjectPoint(PlanePos.X, PlanePos.Z, NormDegree(PlayerPlane->Heading/DEGREE + Angle),Dist*(1/PixelsPerMile(AGScanInfo.Range)), &NewX, &NewZ);
           else
             ProjectPoint(AGInfo.FreezePlaneLoc.X, AGInfo.FreezePlaneLoc.Z, NormDegree(AGInfo.FreezePlaneAzim + Angle),Dist*(1/PixelsPerMile(AGScanInfo.Range)), &NewX, &NewZ);

		   PlayerPlane->AGDesignate.X = (NewX*NMTOFT)*FTTOWU;
           PlayerPlane->AGDesignate.Z = (NewZ*NMTOFT)*FTTOWU;

           if(!AGInfo.FreezeOn)
		     PlayerPlane->AGDesignate.Y = LandHeight(PlayerPlane->AGDesignate.X,PlayerPlane->AGDesignate.Z);
           else
             PlayerPlane->AGDesignate.Y = -1;

		   AGInfo.TgtLoc.X = PlayerPlane->AGDesignate.X;
		   AGInfo.TgtLoc.Y = PlayerPlane->AGDesignate.Y;
		   AGInfo.TgtLoc.Z = PlayerPlane->AGDesignate.Z;

		   AGMapInfo.TargetExists = TRUE;

		 }

	   }

	 }

   }   // tgt mode

   if(AGMapInfo.TargetExists)
   {
	  FPoint ScrLoc;
      int SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

	  if(AGMapInfo.IsMissionTarget)
		SymbColor = 5;

      MapWorldCoordToScreenCoord(AGInfo.TgtLoc,Origin,&ScrLoc);
      DrawRadarSymbol(ScrLoc.X,ScrLoc.Y,HI_RES_MAP_TARGET,SymbColor,GrBuffFor3D);
   }

  // PopClipRegion();

   GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

   if (Pressed)
   {
		switch(ButtonNum)
		{
		  case 10: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
				   Mpds[MpdNum].AGInfo.FreezeErrorStartTime = -1;
			       break;
		  case 13: AGRadarIncRange();
				   break;
	   	  case 17: SniffOn = !SniffOn;
			        break;
		  case 12: AGRadarDecRange();
				   break;
		  case 15: AGInfo.FreezeOn = !AGInfo.FreezeOn;
			       SetAGRadarFreeze(AGInfo.FreezeOn);
				   if(AGInfo.FreezeOn)
                   {
					 AGInfo.FreezePlaneLoc  = PlayerPlane->WorldPosition;
                     AGInfo.FreezePlaneAzim = PlayerPlane->Heading/DEGREE;
                   }
				   BoxDebug = TRUE; // JLM remove
			  	   break;
 		}
   }

 }

// ***** EASY MODE AG END
//***********************************************************************************************************************************



//***********************************************************************************************************************************
// *****

 void RBMMode(int XOfs,int YOfs,int MpdNum,int MpdDetectNum)
 {
   int ButtonNum,Pressed;
   POINT CursPos;
   FPoint Origin;
   FPoint PlanePos;
   FPoint CursBoxWorldLoc;

   float Dx,Dz;
   FPoint MapPos;
   float Angle;
   float NewX, NewZ, Dist;

   if(ShowTerrain)
   {
     PushClipRegion();
     SetClipRegion(0, 639,0,479, 240, 320);
     CopyEdges(&DebugInOutList[0], &NumDebugInOut);
     DrawHeights();
     PopClipRegion();
   }

   Origin.X = XOfs-(116/2) + 57;
   Origin.Y = YOfs-(111/2) + 109;

   PlanePos.X = (PlayerPlane->WorldPosition.X*WUTONM);
   PlanePos.Z = (PlayerPlane->WorldPosition.Z*WUTONM);

   PushClipRegion();
   SetClipRegion(XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2,YOfs-(111/2)+1,	YOfs-(111/2) + 111 - 2, YOfs, XOfs);

   if(UFC.EMISState || SniffOn)
   {
	 if(Mpds[MpdNum].AGInfo.CurMode != GMT_MODE)
	   GrFillRectNoClip(AGScanBuffer, 0, 0, 116, 111, 56);
	 else
	   GrFillRectNoClip(GMTAGScanBuffer, 0, 0, 116, 111, 56);
   }

   if(Mpds[MpdNum].AGInfo.CurMode != GMT_MODE)
     GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGScanBuffer, 0, 0, 116,111);
   else
     GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), GMTAGScanBuffer, 0, 0, 116,111);


	//Render All valid edges in the scanbeamedge table
   DrawScanBeamEdgeTable();

   DrawBeamLine(XOfs,YOfs);
   //GrCopyRectMaskNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGRadarBackground, 0, 0, 116,111);
   	GrDrawLine(GrBuffFor3D, 1,  111/2-1      ,   4, 111/2-1      ,   60);
	GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 1*3,   2, 111/2-1 + 1*3,   60);
    GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 2*3,   2, 111/2-1 + 2*3,   60);
    GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 5*3,   2, 111/2-1 + 5*3,   60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + 10*3,   4, 111/2-1 + 10*3,  60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -1*3,   2, 111/2-1 + -1*3,  60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -2*3,   2, 111/2-1 + -2*3,  60);
    GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -5*3,   2, 111/2-1 + -5*3,  60);
    GrDrawLine(GrBuffFor3D, 1,111/2-1 + -10*3,   4, 111/2-1 + -10*3, 60);

    GrDrawLine(GrBuffFor3D,   116/2-1 - 46, 110, 116/2-1 - 46, 105, 60);
	GrDrawLine(GrBuffFor3D,   116/2-1 + 46, 110, 116/2-1 + 46, 105, 60);


   int MapModeOn = (Mpds[MpdNum].AGInfo.SelectMode == MAP_MODE);

   if(AGInfo.BoxOn)
   {
	  float DeltaAzim,Dist,NewX, NewZ;

	  float dist,delx,dely,delz;
	  FPoint Point;
	  Point.X = (AGInfo.BoxLoc.X*NMTOWU);
	  Point.Y = (AGInfo.BoxLoc.Y*NMTOWU);
      Point.Z = (AGInfo.BoxLoc.Z*NMTOWU);

	  if(!AGInfo.FreezeOn)
      {
	    Dist = sqrt( (PlanePos.X - AGInfo.BoxLoc.X)*(PlanePos.X - AGInfo.BoxLoc.X) +
			       (PlanePos.Z - AGInfo.BoxLoc.Z)*(PlanePos.Z - AGInfo.BoxLoc.Z)   );
	    DeltaAzim = AIComputeHeadingToPoint(PlayerPlane, Point, &dist, &delx, &dely, &delz, 1);
	    ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);
      }
	  else
	  {
	    Dist = sqrt( (AGInfo.FreezePlaneLoc.X*WUTONM - AGInfo.BoxLoc.X)*(AGInfo.FreezePlaneLoc.X*WUTONM - AGInfo.BoxLoc.X) +
			         (AGInfo.FreezePlaneLoc.Z*WUTONM - AGInfo.BoxLoc.Z)*(AGInfo.FreezePlaneLoc.Z*WUTONM - AGInfo.BoxLoc.Z)   );
	    DeltaAzim = ComputeHeadingToPoint(AGInfo.FreezePlaneLoc, AGInfo.FreezePlaneAzim*DEGREE, Point, 1);
	    ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);
	  }

      GetBoxScreenCoords(Origin,&AGScanInfo.Box[0],&AGMapInfo.WCBox[0]);
	  DrawBox(MpdNum, &AGScanInfo.Box[0],&AGScanInfo.Box[1],&AGScanInfo.Box[2],&AGScanInfo.Box[3]);
   }

   GetCursorPos(&CursPos);

   if( !(IsPointInside(CursPos.x, CursPos.y, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) ) )
   {
		Mpds[MpdNum].AGInfo.ErrorCode = NO_AG_ERROR;
   }
   else if(MapModeOn)
   {
	   if (IsPointInside(CursPos.x, CursPos.y, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
	   {
	     AGInfo.CursAzim  = CalcCursAzim(CursPos,Origin);
	     AGInfo.CursRange = CalcCursRange(CursPos.x,CursPos.y,Origin.X,Origin.Y)*
		                  (1/PixelsPerMile(AGScanInfo.Range));
	     AGInfo.Timer     = GetTimeBasedOnAzim(CursPos,Origin);
	   }

	   CalcScreenBox(CursPos.x,CursPos.y, MapSizeTable[AGMapInfo.MapSizeTableIndex],
		      PixelsPerMile(AGScanInfo.Range), &AGMapInfo.Box[0]);

	   int BoxInCone;
	   AGErrorProcessing(MpdNum,CursPos,Origin,&AGMapInfo.Box[0],&BoxInCone, &Mpds[MpdNum].AGInfo.ErrorCode);

	   if(BoxInCone)
	      DrawBox(MpdNum,&AGMapInfo.Box[0],&AGMapInfo.Box[1],&AGMapInfo.Box[2],&AGMapInfo.Box[3]);

	   if(GrabAGButtonClick(MpdDetectNum) && (BoxInCone) )
	   {
		  if (IsPointInside(AGEventInfo.ScrX, AGEventInfo.ScrY, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
		  {

			 if( (AGInfo.FreezeOn) || (UFC.EMISState) || (SniffOn) )
             {
               if(AGInfo.FreezeOn)
                  Mpds[MpdNum].AGInfo.FreezeErrorStartTime = GetTickCount();
			   else if(UFC.EMISState || SniffOn)
                  Mpds[MpdNum].AGInfo.EmisErrorStartTime = GetTickCount();

			   PopClipRegion();
			   return;
			 }

  			 AGMapInfo.StartTime = GetTickCount();
			 AGMapInfo.CreatingMap = TRUE;

  			 Dx = AGEventInfo.ScrX - Origin.X;
			 Dz = -(AGEventInfo.ScrY - Origin.Y);
			 Angle = fabs(atan(Dz/Dx));
			 Angle = RadToDeg(Angle);
			 if(Dx < 0)
			   Angle = 90.0 - Angle;
			 else
			   Angle = Angle + -90.0;

			 Dist = sqrt(Dx*Dx + Dz*Dz);

   			 ProjectPoint(PlanePos.X, PlanePos.Z, NormDegree(PlayerPlane->Heading/DEGREE + Angle),Dist*(1.0/PixelsPerMile(AGScanInfo.Range)), &NewX, &NewZ);

			 AGInfo.BoxLoc.X = NewX;
			 AGInfo.BoxLoc.Y = 0.0;
		     AGInfo.BoxLoc.Z = NewZ;

			 GetBoxWorldCoords(Origin,&AGMapInfo.WCBox[0],&AGMapInfo.Box[0]);
			 SetHiResMapScanInfo();
			 HiResScan.Heading = NormDegree(PlayerPlane->Heading/DEGREE);

			 AGInfo.BoxOn    = TRUE;

			 AGInfo.Duration = AGInfo.Timer;

			 AGInfo.BoxSizeIndex = AGMapInfo.MapSizeTableIndex;

			 AGMapInfo.Range = MapSizeTable[AGMapInfo.MapSizeTableIndex];

			// DrawHiResMap(AGInfo.BoxLoc,MapSizeTable[AGMapInfo.MapSizeTableIndex],AGInfo.Timer);
//
			 int Mpd = MpdToUse(MpdNum);
			 if(Mpd == MpdNum)
			   Mpds[Mpd].AGInfo.MapSubMode = WAITING_MODE;
			 else
			   Mpds[Mpd].AGInfo.MapSubMode = HI_RES_MODE;

			 AGMapInfo.PrevMapSizeTableIndex = AGMapInfo.MapSizeTableIndex;

             if(!MpdDamage[MPD_DAMAGE_CEN_COMP])      // central computer knocks out HRM
			    Mpds[Mpd].AGInfo.CurMode = HRM_MODE;
		  }

	   }

   }

   if(!MapModeOn)
   {
	 GetCursorPos(&CursPos);

     if( (IsPointInside(CursPos.x, CursPos.y, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) ) )
     {

	   AGInfo.CursAzim  = CalcCursAzim(CursPos,Origin);
	   AGInfo.CursRange = CalcCursRange(CursPos.x,CursPos.y,Origin.X,Origin.Y)*
		                  (1/PixelsPerMile(AGScanInfo.Range));

       if(GrabAGButtonClick(MpdDetectNum))
       {
         if (IsPointInside(AGEventInfo.ScrX, AGEventInfo.ScrY, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
         {
           Dx = AGEventInfo.ScrX - Origin.X;
		   Dz = -(AGEventInfo.ScrY - Origin.Y);
		   Angle = fabs(atan(Dz/Dx));
		   Angle = RadToDeg(Angle);
		   if(Dx < 0)
		    Angle = 90.0 - Angle;
		   else
		    Angle = Angle + -90.0;

		   Dist = sqrt(Dx*Dx + Dz*Dz);

		   if(!AGInfo.FreezeOn)
   		     ProjectPoint(PlanePos.X, PlanePos.Z, NormDegree(PlayerPlane->Heading/DEGREE + Angle),Dist*(1/PixelsPerMile(AGScanInfo.Range)), &NewX, &NewZ);
           else
             ProjectPoint(AGInfo.FreezePlaneLoc.X, AGInfo.FreezePlaneLoc.Z, NormDegree(AGInfo.FreezePlaneAzim + Angle),Dist*(1/PixelsPerMile(AGScanInfo.Range)), &NewX, &NewZ);

		   PlayerPlane->AGDesignate.X = (NewX*NMTOFT)*FTTOWU;
           PlayerPlane->AGDesignate.Z = (NewZ*NMTOFT)*FTTOWU;

           if(!AGInfo.FreezeOn)
		     PlayerPlane->AGDesignate.Y = LandHeight(PlayerPlane->AGDesignate.X,PlayerPlane->AGDesignate.Z);
           else
             PlayerPlane->AGDesignate.Y = -1;

		   AGInfo.TgtLoc.X = PlayerPlane->AGDesignate.X;
		   AGInfo.TgtLoc.Y = PlayerPlane->AGDesignate.Y;
		   AGInfo.TgtLoc.Z = PlayerPlane->AGDesignate.Z;

		   AGMapInfo.TargetExists = TRUE;

		 }

	   }

	 }

   }   // tgt mode

   if(AGMapInfo.TargetExists)
   {
	  FPoint ScrLoc;
      int SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

      MapWorldCoordToScreenCoord(AGInfo.TgtLoc,Origin,&ScrLoc);
      DrawRadarSymbol(ScrLoc.X,ScrLoc.Y,HI_RES_MAP_TARGET,SymbColor,GrBuffFor3D);
   }

   if(MapModeOn)
   {
     DrawErrorMessage(MpdNum, XOfs,YOfs,Mpds[MpdNum].AGInfo.ErrorCode);
     DrawTimer(MpdNum, XOfs,YOfs);
   }

   if(!AGInfo.FreezeOn)
      DrawWayPointList(MpdNum,XOfs,YOfs);

   DrawAGBullsEye(MpdNum,XOfs,YOfs);

   if(Mpds[MpdNum].AGInfo.FreezeErrorStartTime != -1)
   {
     sprintf(TmpStr,"RADAR FREEZE");
     GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-20, YOfs-3, TmpStr, 249);
	 if(GetTickCount() -  Mpds[MpdNum].AGInfo.FreezeErrorStartTime > 2000)
		Mpds[MpdNum].AGInfo.FreezeErrorStartTime = -1;
   }
   else if(Mpds[MpdNum].AGInfo.EmisErrorStartTime != -1)
   {
     sprintf(TmpStr,"RADAR STBY");
     GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-20, YOfs+7, TmpStr, 249);
	 if(GetTickCount() -  Mpds[MpdNum].AGInfo.EmisErrorStartTime > 2000)
		Mpds[MpdNum].AGInfo.EmisErrorStartTime = -1;
   }


   PopClipRegion();

   GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

   if (Pressed)
   {
		switch(ButtonNum)
		{
		  case 5:  Mpds[MpdNum].AGInfo.CurMode++;
			       if( (Mpds[MpdNum].AGInfo.CurMode == HRM_MODE) && (!AGInfo.BoxOn) )
					 Mpds[MpdNum].AGInfo.CurMode++;
				   else if(Mpds[MpdNum].AGInfo.CurMode == HRM_MODE)
					  Mpds[MpdNum].AGInfo.MapSubMode = HI_RES_MODE;

			       if(Mpds[MpdNum].AGInfo.CurMode > 3)
					  Mpds[MpdNum].AGInfo.CurMode = 0;

				   if(Mpds[MpdNum].AGInfo.CurMode != HRM_MODE)
                   {
				     SetAGMode(Mpds[MpdNum].AGInfo.CurMode);
					 SetModeOnAGMpds(Mpds[MpdNum].AGInfo.CurMode);
				   }
				   else if(MpdDamage[MPD_DAMAGE_CEN_COMP])
				   {
                     Mpds[MpdNum].AGInfo.CurMode = RBM_MODE;
		             SetAGMode(Mpds[MpdNum].AGInfo.CurMode);
					 SetModeOnAGMpds(Mpds[MpdNum].AGInfo.CurMode);
				   }
			       if(Mpds[MpdNum].AGInfo.CurMode == RBM_MODE)
				     GrFillRectNoClip(AGScanBuffer, 0, 0, 116, 111,	56);
	 			   break;
		  case 6:  Mpds[MpdNum].AGInfo.SelectMode++;
			       if(Mpds[MpdNum].AGInfo.SelectMode > 1)
					  Mpds[MpdNum].AGInfo.SelectMode = 0;
				   break;
		  case 8:  AGScanInfo.ScanRate++;
			       if(AGScanInfo.ScanRate > 2)
					  AGScanInfo.ScanRate = 0;
				   LoadAGRadarOverlayBuff(AGScanInfo.ScanRate);
				   if(AGScanInfo.ScanRate == FULL)
		             AGRadarInfo.ScanLimit = 50;
	               else if(AGScanInfo.ScanRate == HALF)
	         	     AGRadarInfo.ScanLimit = 25;
	               else
		            AGRadarInfo.ScanLimit = 12.5;

				   if(Mpds[MpdNum].AGInfo.CurMode != GMT_MODE)
				     GrFillRectNoClip(AGScanBuffer, 0, 0, 116, 111,	56);
				   else
				     GrFillRectNoClip(GMTAGScanBuffer, 0, 0, 116, 111,	56);
				   //AGRadarReset(&AGRadarInfo);
				   break;
	      case 7:  AGMapInfo.MapSizeTableIndex--;
			       if(AGMapInfo.MapSizeTableIndex < 0)
					  AGMapInfo.MapSizeTableIndex = 7;
				   while(MapSizeTable[AGMapInfo.MapSizeTableIndex] >= AGScanInfo.Range)
                       AGMapInfo.MapSizeTableIndex--;
				   break;
		  case 10: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			      
				   Mpds[MpdNum].AGInfo.FreezeErrorStartTime = -1;
			       break;
		  case 13: AGScanInfo.Range *= 2;
			       if(AGScanInfo.Range > 80)
					  AGScanInfo.Range = 80;
				   AGRadarInfo.Range = AGScanInfo.Range;
	              // AGRadarReset(&AGRadarInfo);
				   break;
	   	  case 17: SniffOn = !SniffOn;
			        break;
		  case 12: AGScanInfo.Range /= 2;
			       if(AGScanInfo.Range < 5)
					  AGScanInfo.Range = 5;
				   AGRadarInfo.Range = AGScanInfo.Range;
	              // AGRadarReset(&AGRadarInfo);
				   while(MapSizeTable[AGMapInfo.MapSizeTableIndex] >= AGScanInfo.Range)
                     AGMapInfo.MapSizeTableIndex--;
				   break;
		  case 15: AGInfo.FreezeOn = !AGInfo.FreezeOn;
			       SetAGRadarFreeze(AGInfo.FreezeOn);
				   if(AGInfo.FreezeOn)
                   {
					 AGInfo.FreezePlaneLoc  = PlayerPlane->WorldPosition;
                     AGInfo.FreezePlaneAzim = PlayerPlane->Heading/DEGREE;
                   }
				   BoxDebug = TRUE; // JLM remove
			  	   break;
 		}
   }

 }

//***********************************************************************************************************************************
// *****

void SetWaitingMpdsToHiRes()
{
  for(int i=0; i<7; i++)
  {
	 if(Mpds[i].AGInfo.MapSubMode == MAP_SELECT_MODE)
	   Mpds[i].AGInfo.MapSubMode = HI_RES_MODE;
  }
}

//***********************************************************************************************************************************
// *****

 void HRMMode(int XOfs,int YOfs,int MpdNum,int MpdDetectNum)
 {
   int ButtonNum,Pressed;
   POINT CursPos;
   FPoint Origin;
   FPoint PlanePos;
   int SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

   if(MpdDamage[MPD_DAMAGE_CEN_COMP]) // central computer damage knocks out the HRM
   {
	Mpds[MpdNum].AGInfo.CurMode = RBM_MODE;
	return;
   }

   Origin.X = XOfs; //-(116/2) + 57;
   Origin.Y = YOfs; //-(111/2) + 109;

   PlanePos.X = (PlayerPlane->WorldPosition.X*WUTOFT)*FTTONM;
   PlanePos.Z = (PlayerPlane->WorldPosition.Z*WUTOFT)*FTTONM;

   PushClipRegion();
   SetClipRegion(XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2,YOfs-(111/2)+1,
		YOfs-(111/2) + 111 - 2, YOfs, XOfs);

   if(Mpds[MpdNum].AGInfo.MapSubMode == WAITING_MODE)
   {
	  if(Mpds[MpdNum].AGInfo.CurMode != GMT_MODE)
         GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGScanBuffer, 0, 0, 116,111);
	  else
         GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), GMTAGScanBuffer, 0, 0, 116,111);

	  //GrCopyRectMaskNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGRadarBackground, 0, 0, 116,111);
  		GrDrawLine(GrBuffFor3D, 1,  111/2-1      ,   4, 111/2-1      ,   60);
		GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 1*3,   2, 111/2-1 + 1*3,   60);
		GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 2*3,   2, 111/2-1 + 2*3,   60);
		GrDrawLine(GrBuffFor3D, 1,  111/2-1 + 5*3,   2, 111/2-1 + 5*3,   60);
		GrDrawLine(GrBuffFor3D, 1, 111/2-1 + 10*3,   4, 111/2-1 + 10*3,  60);
		GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -1*3,   2, 111/2-1 + -1*3,  60);
		GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -2*3,   2, 111/2-1 + -2*3,  60);
		GrDrawLine(GrBuffFor3D, 1, 111/2-1 + -5*3,   2, 111/2-1 + -5*3,  60);
		GrDrawLine(GrBuffFor3D, 1,111/2-1 + -10*3,   4, 111/2-1 + -10*3, 60);

		GrDrawLine(GrBuffFor3D,   116/2-1 - 46, 110, 116/2-1 - 46, 105, 60);
		GrDrawLine(GrBuffFor3D,   116/2-1 + 46, 110, 116/2-1 + 46, 105, 60);

	  DrawBeamLine(XOfs,YOfs);
   }
   else
   {
     if(Mpds[MpdNum].IsMono)
	    GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGMonoMapBuffer, 0, 0, 116,111);
	 else
      	GrCopyRectNoClip(GrBuffFor3D, XOfs-(116/2), YOfs-(111/2), AGMapBuffer, 0, 0, 116,111);
   }

   if( (AGMapInfo.CreatingMap) && ( (int)(GetTickCount() - AGMapInfo.StartTime) < AGInfo.Duration) )
   {
	 int TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
	 GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-15, YOfs, "WAITING", TextColor);
	 AGInfo.Timer = AGInfo.Duration - (GetTickCount() - AGMapInfo.StartTime);
   }
   else	if( (AGMapInfo.CreatingMap) && ((int)(GetTickCount() - AGMapInfo.StartTime) > AGInfo.Duration) )
   {
     AGMapInfo.CreatingMap = FALSE;
	 Mpds[MpdNum].AGInfo.MapSubMode = HI_RES_MODE;
	 AGInfo.Timer =  AGInfo.Duration;
     GrCopyRectNoClip(AGMapBuffer,0,0, AGMapBackBuffer, 0, 0, 116,111);
	 if(Mpds[MpdNum].IsMono)
	   CalcMonoCamera(AGMapBuffer,AGMonoMapBuffer);
   }

   GetCursorPos(&CursPos);

   if( (IsPointInside(CursPos.x, CursPos.y, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) ) && (!AGMapInfo.CreatingMap))
   {

	 if(Mpds[MpdNum].AGInfo.SelectMode == MAP_MODE)
     {
	    CalcScreenBox(CursPos.x,CursPos.y, MapSizeTable[AGMapInfo.MapSizeTableIndex],
		      PixelsPerMile(AGMapInfo.Range), &AGMapInfo.Box[0]);

        DrawBox(MpdNum,&AGMapInfo.Box[0],&AGMapInfo.Box[1],&AGMapInfo.Box[2],&AGMapInfo.Box[3]);

	    if(GrabAGButtonClick(MpdDetectNum))
	    {
		  if (IsPointInside(AGEventInfo.ScrX, AGEventInfo.ScrY, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
		  {
			 FPoint MapPos;

			 if( (AGInfo.FreezeOn) || (UFC.EMISState) || (SniffOn) )
             {
               if(AGInfo.FreezeOn)
                  Mpds[MpdNum].AGInfo.FreezeErrorStartTime = GetTickCount();
			   else if(UFC.EMISState || SniffOn)
                  Mpds[MpdNum].AGInfo.EmisErrorStartTime = GetTickCount();
			   PopClipRegion();
			   return;
			 }

             POINT ProjCursPos;
			 FPoint RbmOrigin;
			 int BoxInCone;
			 FPoint DesignatedPos;
			 float Dist,Dx,Dy,Dz,NewX,NewZ;

		     GetPosInHiResMap(Origin,AGEventInfo.ScrX,AGEventInfo.ScrY,&(DesignatedPos));

			 RbmOrigin.X = XOfs-(116/2) + 57;
   			 RbmOrigin.Y = YOfs-(111/2) + 109;

			 float DeltaAzim = AIComputeHeadingToPoint(PlayerPlane,DesignatedPos, &Dist, &Dx, &Dy, &Dz,1);
           	 Dist = hypot((float)(PlayerPlane->WorldPosition.X - DesignatedPos.X),(float)(PlayerPlane->WorldPosition.Z - DesignatedPos.Z));
             Dist *= WUTONM;

			 ProjectPoint(RbmOrigin.X,RbmOrigin.Y,NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);

			 ProjCursPos.x = (int)NewX;
             ProjCursPos.y = (int)NewZ;

             GetBoxScreenCoords(RbmOrigin,&AGScanInfo.Box[0],&AGMapInfo.WCBox[0]);

			 AGErrorProcessing(MpdNum,ProjCursPos,RbmOrigin,&AGScanInfo.Box[0],&BoxInCone, &Mpds[MpdNum].AGInfo.HrmErrorCode);

			 if(!BoxInCone)
			 {
               Mpds[MpdNum].AGInfo.HrmErrorStartTime = GetTickCount();
			 }
			 else
			 {
			   AGMapInfo.StartTime = GetTickCount();
			   AGMapInfo.CreatingMap = TRUE;

               AGInfo.BoxLoc = DesignatedPos;
               AGInfo.BoxLoc *= WUTONM;

			   CalcNewHiResMap(Origin, AGEventInfo.ScrX, AGEventInfo.ScrY);

			   if(AGMapInfo.MapSizeTableIndex != AGInfo.BoxSizeIndex)
			   {
			     ScaleHiResMap(MapSizeTable[AGInfo.BoxSizeIndex]);
  			   }

			   AGInfo.BoxOn    = TRUE;
			   SetHiResMapScanInfo();
			   AGInfo.BoxSizeIndex = AGMapInfo.MapSizeTableIndex;
			   AGMapInfo.Range = MapSizeTable[AGMapInfo.MapSizeTableIndex];
			  // DrawHiResMap(AGInfo.BoxLoc,MapSizeTable[AGMapInfo.MapSizeTableIndex],AGInfo.Timer);
			 }


		  }

	   }

     }   // map mode
	 else
	 {
		 // only calc azim and range if inside mpd
		/*
		 if (IsPointInside(AGEventInfo.ScrX, AGEventInfo.ScrY, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
		 {
	 	   AGInfo.CursAzim  = CalcCursAzim(CursPos,Origin);
	       AGInfo.CursRange = CalcCursRange(CursPos.x,CursPos.y,Origin.X,Origin.Y)*
		                  (1/PixelsPerMile(AGMapInfo.Range));
		 }
		 */

         if(GrabAGButtonClick(MpdDetectNum))
         {
           if (IsPointInside(AGEventInfo.ScrX, AGEventInfo.ScrY, YOfs-(111/2)+1, YOfs-(111/2) + 111 - 2,
   	                 XOfs-(116/2)+1, XOfs-(116/2) + 116 - 2) )
           {

			 GetPosInHiResMap(XOfs-(116/2),YOfs-(111/2),Origin,AGEventInfo.ScrX,AGEventInfo.ScrY,&(PlayerPlane->AGDesignate));

			 AGMapInfo.TargetExists = TRUE;

			 AGInfo.TgtLoc.X = PlayerPlane->AGDesignate.X;
		     AGInfo.TgtLoc.Y = PlayerPlane->AGDesignate.Y;
		     AGInfo.TgtLoc.Z = PlayerPlane->AGDesignate.Z;

		   }
		 }

	 } // tgt mode

   }

   DrawTimer(MpdNum, XOfs,YOfs);
   DrawAGBullsEye(MpdNum,XOfs,YOfs);

   if(AGMapInfo.TargetExists)
   {
	 FPointDouble TargetLoc = AGInfo.TgtLoc;
     TargetLoc.Y = 0.0;
	 DrawTargetSymbol(MpdNum,XOfs-(116/2),YOfs-(111/2),TargetLoc,HI_RES_MAP_TARGET);
   }

   if(Mpds[MpdNum].AGInfo.FreezeErrorStartTime != -1)
   {
     sprintf(TmpStr,"RADAR FREEZE");
     GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-20, YOfs-3, TmpStr, 249);
	 if(GetTickCount() -  Mpds[MpdNum].AGInfo.FreezeErrorStartTime > 2000)
		Mpds[MpdNum].AGInfo.FreezeErrorStartTime = -1;
   }
   else if(Mpds[MpdNum].AGInfo.EmisErrorStartTime != -1)
   {
     sprintf(TmpStr,"RADAR STBY");
     GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-20, YOfs+7, TmpStr, 249);
	 if(GetTickCount() -  Mpds[MpdNum].AGInfo.EmisErrorStartTime > 2000)
		Mpds[MpdNum].AGInfo.EmisErrorStartTime = -1;
   }

   if(Mpds[MpdNum].AGInfo.HrmErrorStartTime != -1)
   {
	 if(GetTickCount() - Mpds[MpdNum].AGInfo.HrmErrorStartTime < 3000)
        DrawErrorMessage(MpdNum, XOfs,YOfs,Mpds[MpdNum].AGInfo.HrmErrorCode);
     else
       Mpds[MpdNum].AGInfo.HrmErrorStartTime = -1;
   }

   PopClipRegion();


   GrabButtonPress(MpdDetectNum,&ButtonNum,&Pressed);

   if (Pressed)
   {
		switch(ButtonNum)
		{

		  case 5:  Mpds[MpdNum].AGInfo.MapSubMode = MAP_SELECT_MODE;
			       Mpds[MpdNum].AGInfo.CurMode = RBM_MODE;

   				  // Mpds[MpdNum].AGInfo.CurMode = GetModeForAGMpds(MpdNum);
				   SetAGMode(Mpds[MpdNum].AGInfo.CurMode);
                   SetModeOnAGMpds(Mpds[MpdNum].AGInfo.CurMode);

				//   AGMapInfo.Range = AGScanInfo.Range;
				   break;
		  case 6:  Mpds[MpdNum].AGInfo.SelectMode++;
			       if(Mpds[MpdNum].AGInfo.SelectMode > 1)
					  Mpds[MpdNum].AGInfo.SelectMode = 0;
				   break;
		  case 10: Mpds[MpdNum].Mode = MAIN_MENU_MODE;
			       break;
		  case 13: AGMapInfo.PrevMapSizeTableIndex = AGMapInfo.MapSizeTableIndex;
			       AGMapInfo.MapSizeTableIndex++;
			       if(AGMapInfo.MapSizeTableIndex > 7)
					  AGMapInfo.MapSizeTableIndex = 0;
				   break;
		  case 12: AGMapInfo.PrevMapSizeTableIndex = AGMapInfo.MapSizeTableIndex;
			       AGMapInfo.MapSizeTableIndex--;
			       if(AGMapInfo.MapSizeTableIndex < 0)
					  AGMapInfo.MapSizeTableIndex = 7;
				   break;
          case 17: SniffOn = !SniffOn;
			       break;
		  case 15: AGInfo.FreezeOn = !AGInfo.FreezeOn;
			       SetAGRadarFreeze(AGInfo.FreezeOn);
				   if(AGInfo.FreezeOn)
                   {
					 AGInfo.FreezePlaneLoc  = PlayerPlane->WorldPosition;
                     AGInfo.FreezePlaneAzim = PlayerPlane->Heading/DEGREE;
                   }
				   break;
 		}
   }

 }

//***********************************************************************************************************************************
// *****

extern NumEasyModeObjects;
extern EMCurObjPos;
extern NumEasyModeVehs;
extern EMCurVehPos;
extern SelectVehicles;

void DoAirToGroundRadar(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
  //PushClipRegion();
  //SetClipRegion(GrBuffFor3D->ClipLeft, GrBuffFor3D->ClipRight,GrBuffFor3D->ClipTop,
//		479, 480>>1, GrBuffFor3D->MidX);

  DrawOuterBoundry(XOfs,YOfs,MpdNum,MpdDetectNum);

  //PopClipRegion();

  ProcessTargetChange();

  /*
  DrawCircleDebug2(20,20,NumEasyModeObjects);
  DrawCircleDebug2(20,30,EMCurObjPos);
  DrawCircleDebug2(20,40,NumEasyModeVehs);
  DrawCircleDebug2(20,50,EMCurVehPos);
  DrawCircleDebug2(20,60,SelectVehicles);
  */

  if(Mpds[MpdNum].AGInfo.CurMode == HRM_MODE)
	HRMMode(XOfs,YOfs,MpdNum,MpdDetectNum);
  else
  {
	if(!ScanBeamOn)
	{
	  AGRadarInfo.Range = AGScanInfo.Range;
	  if(AGScanInfo.ScanRate == FULL)
		AGRadarInfo.ScanLimit = 50;
	  else if(AGScanInfo.ScanRate == HALF)
		AGRadarInfo.ScanLimit = 25;
	  else
		AGRadarInfo.ScanLimit = 12.5;

	  AGRadarInfo.Mode = Mpds[MpdNum].AGInfo.CurMode;

	 // AGRadarReset(&AGRadarInfo);

	  Mpds[MpdNum].AGInfo.ScanOn = TRUE;
	}

	if(g_Settings.gp.nAARADAR != GP_TYPE_CASUAL)
      RBMMode(XOfs,YOfs,MpdNum,MpdDetectNum);
	else
	  EasyModeRBMMode(XOfs,YOfs,MpdNum,MpdDetectNum);
  }

}

//***********************************************************************************************************************************
// *****

void DrawBeamLine(int XOfs, int YOfs)
{
  FPoint Origin;
  float Azim = GetAGBeamAzim();
  float NewX,NewZ;

  Origin.X = XOfs-(116/2) + 57;
  Origin.Y = YOfs-(111/2) + 109;

  ProjectPoint(Origin.X, Origin.Y, NormDegree(Azim),40*PixelsPerMile(40.0), &NewX, &NewZ);

  int X = Origin.X;
  int Y	= Origin.Y;
  int NX = NewX;
  int NZ = NewZ;

  if( GrClipLine(GrBuffFor3D, &X, &Y, &NX, &NZ))
	  GrDrawLine(GrBuffFor3D, X, Y, NX, NZ, 60);

}


//***********************************************************************************************************************************
// *****

 void DrawAGBullsEye(int MpdNum, int XOfs, int YOfs)
 {
   FPoint WPPos,PlanePos,BoxPos,Origin;
   float DeltaAzim;
   float NewX, NewZ,Dist;
   int   BullsEyeColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
   FPointDouble BullsEyeLoc;

   if( (BullsEye.x <= 0) && (BullsEye.z <= 0) ) return;

   BullsEyeLoc.X = BullsEye.x;
   BullsEyeLoc.Z = BullsEye.z;
   BullsEyeLoc.Y = 0.0;
   BullsEyeLoc *= FTTOWU;

   PlanePos.X = PlayerPlane->WorldPosition.X;
   PlanePos.Z = PlayerPlane->WorldPosition.Z;

   if( (Mpds[MpdNum].AGInfo.CurMode == RBM_MODE) || (Mpds[MpdNum].AGInfo.CurMode == GMT_MODE) || (Mpds[MpdNum].AGInfo.CurMode == GMTI_MODE) )
   {
	 Origin.X = XOfs-(116/2) + 57;
     Origin.Y = YOfs-(111/2) + 109;

	 Dist = sqrt( (PlanePos.X - BullsEyeLoc.X)*(PlanePos.X - BullsEyeLoc.X) +
			      (PlanePos.Z - BullsEyeLoc.Z)*(PlanePos.Z - BullsEyeLoc.Z)   );

	 Dist *= WUTONM;

	 DeltaAzim = ComputeHeadingToPoint(PlanePos,PlayerPlane->Heading,BullsEyeLoc,1);
	 ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);

	 DrawRadarSymbol(NewX, NewZ, BULLSEYE_SYMBOL,BullsEyeColor,GrBuffFor3D);

   }
   else
   {
     DrawTargetSymbol(MpdNum,XOfs-(116/2),YOfs-(111/2),BullsEyeLoc,BULLSEYE_SYMBOL);
   }

 }


//***********************************************************************************************************************************
// *****

 void DrawWayPoint(int MpdNum, int XOfs, int YOfs, MBWayPoints *WayPnt)
 {
   FPoint WPPos,PlanePos,BoxPos,Origin;
   float DeltaAzim;
   float NewX, NewZ,Dist;
   int WayPntColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

   WPPos.X = WayPnt->lWPx*FTTOWU;
   WPPos.Z = WayPnt->lWPz*FTTOWU;

   PlanePos.X = PlayerPlane->WorldPosition.X;
   PlanePos.Z = PlayerPlane->WorldPosition.Z;

   if( (Mpds[MpdNum].AGInfo.CurMode == RBM_MODE) || (Mpds[MpdNum].AGInfo.CurMode == GMT_MODE) || (Mpds[MpdNum].AGInfo.CurMode == GMTI_MODE) )
   {
	 Origin.X = XOfs-(116/2) + 57;
     Origin.Y = YOfs-(111/2) + 109;

	 Dist = sqrt( (PlanePos.X - WPPos.X)*(PlanePos.X - WPPos.X) +
			      (PlanePos.Z - WPPos.Z)*(PlanePos.Z - WPPos.Z)   );

	 Dist = (Dist*WUTOFT)*FTTONM;

	 DeltaAzim = ComputeHeadingToPoint(PlanePos,PlayerPlane->Heading,WPPos, 1);
	 ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),Dist*PixelsPerMile(AGScanInfo.Range), &NewX, &NewZ);

	 DrawRadarSymbol(NewX, NewZ, SMALL_WYPT,WayPntColor,GrBuffFor3D);

   }
   else
   {

	/*
	 Origin.X = XOfs;
     Origin.Y = YOfs;

     BoxPos.X  = AGInfo.BoxLoc.X*NMTOWU;
	 BoxPos.Z  = AGInfo.BoxLoc.Z*NMTOWU;

	 Dist = sqrt( (BoxPos.X - WPPos.X)*(BoxPos.X - WPPos.X) +
			      (BoxPos.Z - WPPos.Z)*(BoxPos.Z - WPPos.Z)   );

	 Dist *= WUTONM;

	 DeltaAzim = ComputeHeadingToPoint(BoxPos,PlayerPlane->Heading,WPPos, 1);
	 ProjectPoint(Origin.X, Origin.Y, NormDegree(DeltaAzim),
		          Dist*PixelsPerMile(MapSizeTable[AGMapInfo.MapSizeTableIndex]),
				  &NewX, &NewZ);

  	 DrawCircleDebug2(100,110,NewX);
	 DrawCircleDebug2(100,120,NewZ);

	 DrawRadarSymbol(NewX, NewZ, RDR_UP_ARROW,61);	  */

   }

 }

//***********************************************************************************************************************************
// *****

void DrawWayPointList(int MpdNum, int XOfs, int YOfs)
{
  MBWayPoints *Start,*Last, *Cur;

  Start = &AIWayPoints[PlayerPlane->AI.startwpts];
  Last  = PlayerPlane->AI.CurrWay + PlayerPlane->AI.numwaypts - 1;
  Cur   = PlayerPlane->AI.CurrWay;

  while(Cur != Last)
  {
 	 DrawWayPoint(MpdNum,XOfs,YOfs,Cur);
	 Cur++;
  }

  DrawWayPoint(MpdNum,XOfs,YOfs,Cur);  // draw last wpt

  Cur = Start;

  while(Cur != PlayerPlane->AI.CurrWay)
  {
	DrawWayPoint(MpdNum,XOfs,YOfs,Cur);
    Cur++;
  }

}

//***********************************************************************************************************************************
// *****

void DrawErrorMessage(int MpdNum, int XOfs, int YOfs, int ErrorCode)
{
	int ErrorTextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
    char *Str;

	switch(ErrorCode)
	{
	  case DW_RANGE_LIMIT: Str = "DW RANGE LIMIT"; break;
      case GIMBLE_LIMIT  : Str = "GIMBLE LIMIT";   break;
      case BLIND_ZONE    : Str = "BLIND ZONE";     break;
      default            : Str = "";               break;
	}

    sprintf(TmpStr, Str);
	GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-20, YOfs+38, TmpStr, ErrorTextColor);
}

//***********************************************************************************************************************************
// *****

void DrawTimer(int MpdNum, int XOfs, int YOfs)
{
  int TimerColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];
  sprintf(TmpStr, "%d",AGInfo.Timer/1000 + 1);
  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs+30, YOfs+44, TmpStr, TimerColor);
}

//***********************************************************************************************************************************
// *****

void DrawOuterBoundry(int XOfs, int YOfs, int MpdNum, int MpdDetectNum)
{
    char *String;
    int TextColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_SOFT];
    int SymbColor = MpdColors[Mpds[MpdNum].IsMono][COLOR_NORM_BOLD];

 	ClearMpd(MpdDetectNum);

	switch (Mpds[MpdNum].AGInfo.CurMode)
	{
	  case RBM_MODE:   String = "RBM";	break;
	  case HRM_MODE:   String = "HRM";	break;
	  case GMT_MODE:   String = "GMT";	break;
	  case GMTI_MODE:  String = "IGMT";	break;
	}

	DrawWordAtButton(MpdDetectNum,5,String,FALSE,TextColor);

    if(g_Settings.gp.nAARADAR != GP_TYPE_CASUAL)
	{
	  switch (Mpds[MpdNum].AGInfo.SelectMode)
	  {
	    case MAP_MODE:       String = "MAP";	break;
	    case TARGET_MODE:    String = "TRGT";	break;
	  }

	  DrawWordAtButton(MpdDetectNum,6,String,FALSE,TextColor);
	}

    if(g_Settings.gp.nAARADAR != GP_TYPE_CASUAL)
	{
	  if(Mpds[MpdNum].AGInfo.CurMode == HRM_MODE)
	  {
   	   if(MapSizeTable[AGInfo.BoxSizeIndex] >= 10.0)
	      sprintf(TmpStr, "%2d", (int)MapSizeTable[AGInfo.BoxSizeIndex]);
	    else
	      sprintf(TmpStr, "%1.2f", MapSizeTable[AGInfo.BoxSizeIndex]);
	  }
      else
	  {
	    if(MapSizeTable[AGMapInfo.MapSizeTableIndex] >= 10.0)
		  sprintf(TmpStr, "%2d", (int)MapSizeTable[AGMapInfo.MapSizeTableIndex]);
	    else
	      sprintf(TmpStr, "%1.2f", MapSizeTable[AGMapInfo.MapSizeTableIndex]);
	  }

	  DrawWordAtButton(MpdDetectNum,7,TmpStr,FALSE,TextColor);

	}

	switch (AGScanInfo.ScanRate)
	{
	  case FULL:    String = "FULL";	break;
	  case HALF:    String = "HALF";	break;
	  case QUART:   String = "QUART";	break;
	}

	DrawWordAtButton(MpdDetectNum,8,String,FALSE,TextColor);

	DrawWordAtButton(MpdDetectNum,10, "M", FALSE,TextColor);

	int Offset = 58;
	if(Mpds[MpdNum].AGInfo.CurMode == HRM_MODE)
	{
      if(g_Settings.gp.nAARADAR != GP_TYPE_CASUAL)
	  {
	    if(MapSizeTable[AGMapInfo.MapSizeTableIndex] >= 10)
	    {
	      sprintf(TmpStr, "%2d", (int)MapSizeTable[AGMapInfo.MapSizeTableIndex]);
		  Offset = 62;
   	    }
	    else
	    {
		  sprintf(TmpStr, "%1.1f",MapSizeTable[AGMapInfo.MapSizeTableIndex]);
		  Offset = 60;
	    }

		GrDrawString(GrBuffFor3D, SmHUDFont, XOfs+Offset, YOfs-15, TmpStr, TextColor);

	  }
	}
	else
	{
	  sprintf(TmpStr, "%3d", (int)AGScanInfo.Range);
	  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs+Offset, YOfs-15, TmpStr, TextColor);
	}

	DrawWordAtButton(MpdDetectNum,15, "FRZ", (AGInfo.FreezeOn), 61);

	DrawWordAtButton(MpdDetectNum,17, "SNIFF", (SniffOn || UFC.EMISState), 61);

	if( (Mpds[MpdNum].AGInfo.CurMode != HRM_MODE) && (g_Settings.gp.nAARADAR != GP_TYPE_CASUAL) )
	{
	  sprintf(TmpStr, "%3d", (int)AGInfo.CursAzim);
	  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-58, YOfs-61, TmpStr, TextColor);
	  sprintf(TmpStr, "%3d", (int)AGInfo.CursRange);
	  GrDrawString(GrBuffFor3D, SmHUDFont, XOfs-40, YOfs-61, TmpStr, TextColor);
	}

	if( !((g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) && (Mpds[MpdNum].AGInfo.CurMode == HRM_MODE)) )
	{
	  DrawRadarSymbol(XOfs+67, YOfs-22, RDR_UP_ARROW, SymbColor,GrBuffFor3D);
      DrawRadarSymbol(XOfs+67, YOfs-2,   RDR_DOWN_ARROW, SymbColor,GrBuffFor3D);
	}
}

//***********************************************************************************************************************************
// *****

int GrabAGButtonClick(int MpdDetectNum)
{
   if (AGButtonClickInfo.TargetSelected)
   {
 	 if(MpdDetectNum == AGButtonClickInfo.MpdDetectNum)
	 {
	   AGButtonClickInfo.TargetSelected = FALSE;

	   AGEventInfo.ScrX = AGButtonClickInfo.ScrX;
	   AGEventInfo.ScrY = AGButtonClickInfo.ScrY;

	   return(TRUE);
   	 }
   }

   return(FALSE);
}

//***********************************************************************************************************************************
// *****

//void InitAGRadar(void)
//{

	//AGRadarBackground = GrAllocGrBuff(116,111, GR_INDEXED | GR_8BIT);
	//GrFillRectNoClip(AGRadarBackground, 0, 0, 116, 111,	56);

/*
    AGRadarOverlays[QUART][0] = 14;  AGRadarOverlays[QUART][1]     = 44;
	AGRadarOverlays[HALF][0]  = 157; AGRadarOverlays[HALF][1]      = 44;
	AGRadarOverlays[FULL][0]  = 305; AGRadarOverlays[FULL][1]      = 45;

 	LoadAGRadarOverlayBuff(FULL);

 	AGScanInfo.Range       = 20.0;
	AGMapInfo.Range        = 20.0;
	AGMapInfo.CreatingMap  = FALSE;
	AGMapInfo.SubMode	   = MAP_SELECT_MODE;
	AGMapInfo.TargetExists = FALSE;

	AGScanInfo.ScanRate = FULL;
    MapSizeTable[0] = 0.67; MapSizeTable[1] = 1.3;
	MapSizeTable[2] = 3.3;  MapSizeTable[3] = 4.7;
    MapSizeTable[4] = 10;   MapSizeTable[5] = 20;
	MapSizeTable[6] = 40;   MapSizeTable[7] = 80;

 	AGInfo.BoxOn = FALSE;
	AGInfo.BoxSizeIndex = 0;

	if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
      AGMapInfo.MapSizeTableIndex = 2;
	else
      AGMapInfo.MapSizeTableIndex = 4;

    AGMapInfo.PrevMapSizeTableIndex = 0;
	AGInfo.FreezeOn = FALSE;
	AGInfo.CursAzim = 0.0;
	AGInfo.CursRange = 0.0;
	AGInfo.Timer = 3000;
	AGInfo.AGDesignate.X = -1;
	AGInfo.AGDesignate.Y = -1;
	AGInfo.AGDesignate.Z = -1;

	InitScanBeam();
	ScanBeamOn = FALSE;
	SniffOn = FALSE;

	EasyModePrevTargLoc.X = -1; EasyModePrevTargLoc.Y = -1; EasyModePrevTargLoc.Z = -1;
    TargLoc.X = -1;             TargLoc.Y = -1;             TargLoc.Z = -1;

	EasyModeSelectTarget = FALSE;
	EasyModeResetTarget = FALSE;

*/

//}


//***********************************************************************************************************************************
// *****

//void CleanupAGRadar()
//{
  //GrFreeGrBuff(AGRadarBackground);
  //CleanupScanBeam();
//}

//***********************************************************************************************************************************
// *****

void LoadAGRadarOverlayBuff(int Type)
{
}

//***********************************************************************************************************************************
// *****

int GetHiResMapLoc(FPoint *BoxLoc)
{

  if(AGInfo.BoxOn)
  {
	  BoxLoc->X = AGInfo.BoxLoc.X*NMTOWU;
      BoxLoc->Y = AGInfo.BoxLoc.Y*NMTOWU;
	  BoxLoc->Z = AGInfo.BoxLoc.Z*NMTOWU;
      return(TRUE);
  }

  return(FALSE);

}

//***********************************************************************************************************************************
// *****

void AGRadarIncRange()
{
  AGScanInfo.Range *= 2;
  if(AGScanInfo.Range > 80)
	AGScanInfo.Range = 80;
  AGRadarInfo.Range = AGScanInfo.Range;
 // AGRadarReset(&AGRadarInfo);
}

//***********************************************************************************************************************************
// *****

void AGRadarDecRange()
{
  AGScanInfo.Range /= 2;
  if(AGScanInfo.Range < 5)
	AGScanInfo.Range = 5;
  AGRadarInfo.Range = AGScanInfo.Range;
 // AGRadarReset(&AGRadarInfo);
}

//***********************************************************************************************************************************
// *****


void ToggleShowTerrain()
{
  ShowTerrain = !ShowTerrain;
}