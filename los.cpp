#include "F18.h"
#include "gamesettings.h"

#define X_LINE    0
#define Y_LINE    1
#define DIAG_LINE 2

#define LTR_EDGE   0
#define RTL_EDGE   1

FPoint Origin;

// JLM debug
FPointDouble TempIntersect;
int UseIntersection = FALSE;
int ResToUse;

int ScanStop = FALSE;

FPoint BeamP1, BeamP2;

float DegPerTick = 1.8;

int NumEdges = 0;
EdgeListEntryType EdgeList[800];

int NumInOut;
InOutType *InOutList = (InOutType *)&EdgeList[0];

int SwappedEndPoints;

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
	int StartY;
	PixelListType Edge[SCAN_LINE_LENGTH];
}ScanEdge;


int ScanBeamEdge =0;
ScanEdge ScanBeamEdges[NUM_SCAN_LINES];

void DrawScanBeamEdgeTable( void );

//*****************************************************************************************************************************************
// EXTERNS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern int TimeExcel;
extern AvionicsType Av;



FMatrix HeadingAdjustMatrix;

void ReverseEdges();
void RemoveNegativeHeights();
void FilterObjectsForScanBeam();
void DrawObjectsForScanBeam();
void DrawGroundObjectsForScanBeam();
void FilterVehiclesForScanBeam();

//***********************************************************************************************************************************
// *****

int CheckEdgeListIntegrity()
{
  EdgeListEntryType *Edges;

  Edges = &EdgeList[0];

  int Index = NumEdges;

  while(Index-- > 0)
  {
    if(Edges->Tval > 1.0)
      return(FALSE);
	if(Edges->Height < 0.0)
	  return(FALSE);
	Edges++;
  }

  return(TRUE);
}

//***********************************************************************************************************************************
// *****

void CalcDiagonalTvals(FPoint *A1, FPoint *A2, FPoint *B1, FPoint *B2, double *AT1, int *Parallel)
{
  FPoint NormVec;
  float Numer, Denom;
  float DDx,DDz;

  DDx = B2->X - B1->X;
  DDz = B2->Z - B1->Z;

  NormVec.X =  DDz;
  NormVec.Z = -DDx;

  Numer = NormVec.X*(A1->X - B2->X) + NormVec.Z*(A1->Z - B2->Z);
  Denom = -( NormVec.X*(A2->X - A1->X) + NormVec.Z*(A2->Z - A1->Z) );

  if( (Denom > 0.0001) || (Denom < -0.0001) )
    *AT1 = Numer/Denom;
  else
  {
    *AT1 = 0.0;
	*Parallel = TRUE;
  }

}

//***********************************************************************************************************************************
// *****

void HiResTerEdgeDetection(FPoint *P1, FPoint *P2, int *NumEdges, EdgeListEntryType *EdgeList, int *SwappedEndPoints, int *ClippedLine)
{
  double DxDz,DzDx;
  double NewX, NewZ, CurX, CurZ, NextX, NextZ, PrevX, PrevZ,BackX,BackZ;
  double width  =  TILE_SIZE;
  double height =  float(-width);
  double OneOverPixelSize = 1.0/width;
  double StartLength,LineLength;
  double CurT,Tx,Tz,Tdx,Tdz;
  FPoint Diag1,Diag2,NormVec;
  double T1,T2, LTRT1, LTRT2;
  int    ParallelToDiag=FALSE;
  int    LTRParallelToDiag=FALSE;
  int    InsertDiagT=FALSE;
  int    LTRInsertDiagT=FALSE;
  double PDx,PDz;
  FPoint Temp;
  double RemTx, RemTz, RemTDiagx, RemTDiagz, RemTDiagdx,RemTDiagdz,RemTdx, RemTdz;
  double LTRRemTDiagdx,LTRRemTDiagdz,LTRRemTDiagx,LTRRemTDiagz;
  float  LineDistance;

  int ToTheRight;
  int LineCrossed;
  SimpleLandEntry *Addr;
  int U,V;
  int ZRemTCalculated =   TRUE;
  int XRemTCalculated =   TRUE;
  int DiagRemTCalculated = FALSE;
  int LTRDiagRemTCalculated = FALSE;
  int DeltaAddrX;
  int DeltaAddrZ;
  double ULX,ULY;
  float Height1, Height2;
  double XMod,YMod;
  BOOL split_URLL;

  float Dx,Dy,Dz;
  Dx = P2->X - P1->X;
  Dy = P2->Y - P1->Y;
  Dz = P2->Z - P1->Z;

  LineDistance = sqrt(Dx*Dx + Dy*Dy + Dz*Dz);
  LineDistance *= WUTONM;

  if(LineDistance > 40.0)
  {
	*ClippedLine = TRUE;
	return;
  }

  *NumEdges = 0;

  *SwappedEndPoints = FALSE;

  // sort endpoints
  if(P2->Z > P1->Z)
  {
	Temp.X  = P2->X;   Temp.Z = P2->Z;
	P2->X   = P1->X;   P2->Z   = P1->Z;
	P1->X   = Temp.X; P1->Z   = Temp.Z;
	*SwappedEndPoints = TRUE;
  }

  U = floor((P1->X - TerrainLoadedULLocation.X)*OneOverPixelSize);
  V	= floor((P1->Z - TerrainLoadedULLocation.Z)*OneOverPixelSize);

  *ClippedLine = FALSE;

  // clip to buffer edge
  if(floor((P2->Z - TerrainLoadedULLocation.Z)*OneOverPixelSize) < 0)
  {
	FPoint UEdgeLeft, UEdgeRight;
	double Tval;
	int Parallel;

	UEdgeLeft.X = TerrainLoadedULLocation.X;
	UEdgeLeft.Y = 0;
	UEdgeLeft.Z = floor(TerrainLoadedULLocation.Z);

	UEdgeRight.X = TerrainLoadedULLocation.X + SL_CACHE_WIDTH;
	UEdgeRight.Y = 0;
	UEdgeRight.Z = floor(TerrainLoadedULLocation.Z);

    CalcDiagonalTvals(P1,P2,&UEdgeLeft,&UEdgeRight,&Tval,&Parallel);

	// for now, don't clip. We will use med res if clip occurs. This may change JLM
	//P2->X = P1->X + (P2->X - P1->X)*Tval;
	//P2->Z = P1->Z + (P2->Z - P1->Z)*Tval;

	*ClippedLine = TRUE;
	return;
  }

  CurX = P1->X;
  CurZ = P1->Z;

  PDx = P2->X - P1->X;
  PDz = P2->Z - P1->Z;

  DzDx = -fabs( (P2->Z - P1->Z) / (P2->X - P1->X) );
  DxDz = (P2->X - P1->X)  / -(P2->Z - P1->Z);

  LineLength  = sqrt((P1->X-P2->X)*(P1->X-P2->X) + (P1->Z-P2->Z)*(P1->Z-P2->Z));

  Tdx = sqrt((DzDx*width)*(DzDx*width) + width*width)/LineLength;
  Tdz = sqrt((DxDz*height)*(DxDz*height) + height*height)/LineLength;

  RemTdx = fabs((Tdx*PDz)/width);
  RemTdz = fabs((Tdz*PDx)/width);

  // set up address walk
  ToTheRight = (P2->X - P1->X > 0);

  if(ToTheRight)
	DeltaAddrX =  1;
  else
	DeltaAddrX = -1;

  DeltaAddrZ = -SL_CACHE_SIZE;

  ULX = TerrainLoadedULLocation.X;
  ULY = TerrainLoadedULLocation.Z;

  Addr = &TerrainBuffer[U + V*SL_CACHE_SIZE];

  split_URLL = (U ^ V)&1;

  XMod = fmod(double(CurX - ULX),width);
  YMod = fmod(double(CurZ - ULY),fabs(height));

  // Store first element T = 0.0
  (*NumEdges)++;
  EdgeList[(*NumEdges)-1].Tval = 0.0;
  Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*(XMod/width);
  Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*(XMod/width);
  EdgeList[(*NumEdges)-1].Height = Height2 + (Height1 - Height2)*(YMod/width);

  if(P2->X >= P1->X)
	 if(XMod > 0)
	 {
      NextX = CurX + width - XMod;
	  PrevX = CurX - XMod;
	 }
	 else
	   NextX = PrevX = CurX;
  else
	 if(XMod > 0)
	 {
      NextX = CurX - XMod;
	  PrevX = CurX + width - XMod;
	 }
	 else NextX = PrevX = CurX;

  if(YMod > 0)
  {
     NextZ = CurZ - YMod;
	 PrevZ = CurZ + fabs(height) - YMod;
  }
  else NextZ = PrevZ = CurZ;

  NewX = NextX;
  NewZ = CurZ - fabs((NextX - CurX)*DzDx);
  LineCrossed = X_LINE;

  if(NewZ < NextZ)
  {
    NewZ = NextZ;
	NewX = CurX + (CurZ - NextZ)*DxDz;
	LineCrossed = Y_LINE;
  }

  Diag2.X = NextX;
  Diag2.Z = NextZ;
  Diag1.X = PrevX;
  Diag1.Z = PrevZ;

  if(P2->X < P1->X)
  {
 	Temp.X  = Diag2.X;
	Diag2.X = Diag1.X;
	Diag1.X = Temp.X;
  }

  if(Diag2.Z == PrevZ)
	Diag1.Z += width;

  if(Diag1.Z == NextZ)
	 Diag2.Z -= width;

  CalcDiagonalTvals(P1,P2,&Diag1,&Diag2,&T1,&ParallelToDiag);

  Temp.X = Diag2.X;
  Diag2.X = Diag1.X;
  Diag1.X = Temp.X;

  CalcDiagonalTvals(P1,P2,&Diag1,&Diag2,&LTRT1,&LTRParallelToDiag);

  T2 = fabs(width/-(PDx + PDz));

  LTRT2 = fabs(width/(PDx - PDz));

  RemTDiagdx = fabs((T2*PDz)/width);
  RemTDiagdz = fabs((T2*PDx)/width);

  LTRRemTDiagdx = fabs((LTRT2*PDz)/width);
  LTRRemTDiagdz = fabs((LTRT2*PDx)/width);

  StartLength = sqrt((CurX-NewX)*(CurX-NewX) + (CurZ-NewZ)*(CurZ-NewZ));

  CurT = StartLength/LineLength;

  if( /*(Addr->Flags & T_TILE_SPLIT_URLL)*/ split_URLL && (CurT > T1) && (T1 > 0.0) && (T1 < 1.0) )
  {
	DiagRemTCalculated = TRUE;
    if(ToTheRight)
	{
	  RemTDiagx = fabs( ( ( (ULY + (V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	  RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX + U*width) ) / width );
	}
	else
	{
	  RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	  RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	}

	(*NumEdges)++;
	EdgeList[(*NumEdges)-1].Tval = T1;

	double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	if(ToTheRight)
    {
	  Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
	  Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	}
	else
    {
      Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
	  Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
    }

	EdgeList[(*NumEdges)-1].Edge = 555;
	EdgeList[(*NumEdges)-1].Remx = RemTDiagx;
	EdgeList[(*NumEdges)-1].Remy = RemTDiagz;


	if(LineCrossed == X_LINE)
	{
	  RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
	  ZRemTCalculated = FALSE;
	  Addr += DeltaAddrX;
	  split_URLL ^= 1;
	  (*NumEdges)++;
      EdgeList[(*NumEdges)-1].Tval = CurT;
	  if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + (Addr->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTx - floor(RemTx));
	  else
		 EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+1)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTx - floor(RemTx));
	  EdgeList[(*NumEdges)-1].Remx = RemTx;
	  EdgeList[(*NumEdges)-1].Edge = 111;

    }
	else
	{
	  XRemTCalculated = FALSE;
	  Addr += DeltaAddrZ;
	  split_URLL ^= 1;
	  (*NumEdges)++;
      EdgeList[(*NumEdges)-1].Tval = CurT;

	  if(ToTheRight)
	  {
	    RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
		EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + ((Addr+SL_CACHE_SIZE+1)->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTz - floor(RemTz));
	  }
	  else
	  {
		RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );
		EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+SL_CACHE_SIZE)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTz - floor(RemTz));
	  }

	  EdgeList[(*NumEdges)-1].Remy = RemTz;
	  EdgeList[(*NumEdges)-1].Edge = 222;

	}
  }
  else if( /*!(Addr->Flags & T_TILE_SPLIT_URLL)*/ !split_URLL && (CurT > LTRT1) && (LTRT1 > 0.0) && (LTRT1 < 1.0) )
  {

	LTRDiagRemTCalculated = TRUE;
    if(ToTheRight)
	{
	  LTRRemTDiagx = fabs( ( ( (ULY + (V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
	  LTRRemTDiagz = fabs( ( (P1->X + LTRT1*PDx) - (ULX + U*width) ) / width );
	}
	else
	{
	  LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
 	  LTRRemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + LTRT1*PDx) ) / width ) );
	}

	(*NumEdges)++;
	EdgeList[(*NumEdges)-1].Tval = LTRT1;

	double TempRemTDiagx = LTRRemTDiagx - floor(LTRRemTDiagx);
	double TempRemTDiagz = LTRRemTDiagz - floor(LTRRemTDiagz);
	if(ToTheRight)
    {
	  Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
	  Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	}
	else
    {
      Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
	  Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
    }

	EdgeList[(*NumEdges)-1].Edge = 777;
	EdgeList[(*NumEdges)-1].Remx = LTRRemTDiagx;
	EdgeList[(*NumEdges)-1].Remy = LTRRemTDiagz;


	if(LineCrossed == X_LINE)
	{
	  RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
	  ZRemTCalculated = FALSE;
	  Addr += DeltaAddrX;
	  split_URLL ^= 1;
	  (*NumEdges)++;
      EdgeList[(*NumEdges)-1].Tval = CurT;
	  if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + (Addr->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTx - floor(RemTx));
	  else
		 EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+1)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTx - floor(RemTx));
	  EdgeList[(*NumEdges)-1].Remx = RemTx;
	  EdgeList[(*NumEdges)-1].Edge = 111;

    }
	else
	{
	  XRemTCalculated = FALSE;
	  Addr += DeltaAddrZ;
	  split_URLL ^= 1;
	  (*NumEdges)++;
      EdgeList[(*NumEdges)-1].Tval = CurT;

	  if(ToTheRight)
	  {
	    RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
		EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + ((Addr+SL_CACHE_SIZE+1)->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTz - floor(RemTz));
	  }
	  else
	  {
		RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );
		EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+SL_CACHE_SIZE)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTz - floor(RemTz));
	  }

	  EdgeList[(*NumEdges)-1].Remy = RemTz;
	  EdgeList[(*NumEdges)-1].Edge = 222;

	}

  }
  else
  {
	if( (T1 > 0) && (T1 < 1.0) )
	 InsertDiagT = TRUE;

	if( (LTRT1 > 0) && (LTRT1 < 1.0) )
	 LTRInsertDiagT = TRUE;

	if( (CurT > 0) && (CurT < 1.0) )
	{

	  if(LineCrossed == X_LINE)
	  {
	    RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
	    ZRemTCalculated = FALSE;
		Addr += DeltaAddrX;
	  	split_URLL ^= 1;
	    (*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;
		if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + (Addr->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTx - floor(RemTx));
	    else
		 EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+1)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTx - floor(RemTx));
		 EdgeList[(*NumEdges)-1].Remx = RemTx;
	     EdgeList[(*NumEdges)-1].Edge = 111;

      }
	  else
      {

		XRemTCalculated = FALSE;
	    Addr += DeltaAddrZ;
	  	split_URLL ^= 1;
	    (*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;

	    if(ToTheRight)
	    {
	      RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
	      EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + ((Addr+SL_CACHE_SIZE+1)->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTz - floor(RemTz));
	    }
	    else
	    {
	      RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );
	   	  EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+SL_CACHE_SIZE)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTz - floor(RemTz));

	    }

	   EdgeList[(*NumEdges)-1].Remy = RemTz;
	   EdgeList[(*NumEdges)-1].Edge = 222;

      }

	}

  }

  if(NewX == NextX)
  {
	if(CurZ != PrevZ)
	  BackX = CurX - (PrevZ - CurZ)*DxDz;
	else
	  BackX = CurX;

	BackZ = PrevZ;
 	Tz = -(sqrt((P1->X-BackX)*(P1->X-BackX) + (P1->Z-BackZ)*(P1->Z-BackZ)))/LineLength;
	Tx = CurT;
  }
  else
  {
    if(PrevX != CurX)
	  BackZ = CurZ + fabs( (PrevX - CurX)*DzDx);
	else
	  BackZ = CurZ;

	BackX = PrevX;
	Tx = -(sqrt((P1->X-BackX)*(P1->X-BackX) + (P1->Z-BackZ)*(P1->Z-BackZ)))/LineLength;
	Tz = CurT;
  }

  if(P2->X - P1->X == 0)
  {
	Tx  = 0.0f;
	Tdx = 1000.0f;
  }

  if(P2->Z - P1->Z == 0)
  {
	Tz  = 0.0f;
	Tdz = 1000.0f;
  }

  if(ParallelToDiag)
  {
	T1 = 0.0f;
	T2 = 1000.0f;
  }

  if(LTRParallelToDiag)
  {
    LTRT1 = 0.0f;
	LTRT2 = 1000;
  }

  while( (Tx + Tdx < 1.0) || (Tz + Tdz < 1.0) || (T1 + T2 < 1.0) || (LTRT1 + LTRT2 < 1.0) )
  {
      if( (Tz + Tdz < Tx + Tdx) && (Tz + Tdz < T1 + T2) && (Tz + Tdz < LTRT1 + LTRT2) )
	  {
		CurT = Tz = Tz + Tdz;
		Addr += DeltaAddrZ;
	  	split_URLL ^= 1;
		if(!ZRemTCalculated)
		{
		  if(ToTheRight)
	        RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
	      else
	        RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );

		  ZRemTCalculated = TRUE;
		}
		else
		{
		  RemTz += RemTdz;
		}

		(*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;
		if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + ((Addr+SL_CACHE_SIZE+1)->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTz - floor(RemTz));
	    else
		 EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+SL_CACHE_SIZE)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTz - floor(RemTz));

		EdgeList[(*NumEdges)-1].Remy = RemTz;
	    EdgeList[(*NumEdges)-1].Edge = 222;

	  }
	  else if ( (Tx + Tdx < T1 + T2)  && (Tx + Tdx < LTRT1 + LTRT2) )
	  {
		CurT = Tx = Tx + Tdx;
		Addr += DeltaAddrX;
	  	split_URLL ^= 1;
 		if(!XRemTCalculated)
		{
		  RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
		  XRemTCalculated = TRUE;
		}
		else
		{
		  RemTx += RemTdx;
		}

		(*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;
		if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE)->Height + (Addr->Height - (Addr+SL_CACHE_SIZE)->Height)*(RemTx - floor(RemTx));
	    else
		 EdgeList[(*NumEdges)-1].Height = (Addr+SL_CACHE_SIZE+1)->Height + ((Addr+1)->Height - (Addr+SL_CACHE_SIZE+1)->Height)*(RemTx - floor(RemTx));
		 EdgeList[(*NumEdges)-1].Remx = RemTx;
	     EdgeList[(*NumEdges)-1].Edge = 111;

  	  }
	  else
	  {

		if(T1 + T2 < LTRT1 + LTRT2)
		{

		  CurT = T1 = T1 + T2;

		  if(!DiagRemTCalculated)
		  {
		      if(ToTheRight)
	          {
	            RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	            RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX+U*width) ) / width );
	          }
       	      else
	          {
	            RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	            RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	          }

		      DiagRemTCalculated = TRUE;

		    }
		    else
		    {
		      RemTDiagx += RemTDiagdx;
		      RemTDiagz	+= RemTDiagdz;
		    }	// diag calc

  			if(split_URLL)//Addr->Flags & T_TILE_SPLIT_URLL)
			{

			  (*NumEdges)++;
              EdgeList[(*NumEdges)-1].Tval = CurT;

   	          double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	          double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	          if(ToTheRight)
		      {
		        Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
		        Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
		        EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 		      }
	          else
	          {
     	        Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
		        Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
		        EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
		      }

		      EdgeList[(*NumEdges)-1].Edge = 555;
		      EdgeList[(*NumEdges)-1].Remx = RemTDiagx;
     	      EdgeList[(*NumEdges)-1].Remy = RemTDiagz;

		  }	// use right edge

		}
		else
		{

	 	  CurT = LTRT1 = LTRT1 + LTRT2;

		   if(!LTRDiagRemTCalculated)
		   {
		      if(ToTheRight)
	          {
	            LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
	            LTRRemTDiagz = fabs( ( (P1->X + LTRT1*PDx) - (ULX+U*width) ) / width );
	          }
       	      else
	          {
	            LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
 	            LTRRemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + LTRT1*PDx) ) / width ) );
	          }

		      LTRDiagRemTCalculated = TRUE;

		    }
		    else
		    {
		       LTRRemTDiagx += LTRRemTDiagdx;
		       LTRRemTDiagz	+= LTRRemTDiagdz;
		    }	// diag calc

			if(!split_URLL)//!(Addr->Flags & T_TILE_SPLIT_URLL))
			{

		      (*NumEdges)++;
              EdgeList[(*NumEdges)-1].Tval = CurT;

   	          double TempRemTDiagx = LTRRemTDiagx - floor(LTRRemTDiagx);
	          double TempRemTDiagz = LTRRemTDiagz - floor(LTRRemTDiagz);
	          if(ToTheRight)
		      {
		        Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
		        Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
		        EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 		       }
	           else
	           {
     	        Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
		        Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
		        EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
		       }

		       EdgeList[(*NumEdges)-1].Edge = 777;
		       EdgeList[(*NumEdges)-1].Remx = LTRRemTDiagx;
     	       EdgeList[(*NumEdges)-1].Remy = LTRRemTDiagz;

		     }	// use edge

		  }	   // USE LTR EDGE

	  }		  // CHECK CONDITIONS


	  if(InsertDiagT)
	  {
		if(T1 <= CurT)
		{
		   if(ToTheRight)
	       {
	          RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	          RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX+U*width) ) / width );
	       }
       	   else
	       {
	          RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	          RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	       }

		   // move the current entry up one
		   (*NumEdges)++;
		   EdgeList[(*NumEdges)-1].Tval   = EdgeList[(*NumEdges)-2].Tval;
		   EdgeList[(*NumEdges)-1].Height = EdgeList[(*NumEdges)-2].Height;
		   EdgeList[(*NumEdges)-1].Edge = EdgeList[(*NumEdges)-2].Edge;
		   EdgeList[(*NumEdges)-1].Remx = EdgeList[(*NumEdges)-2].Remx;
	       EdgeList[(*NumEdges)-1].Remy = EdgeList[(*NumEdges)-2].Remy;

           EdgeList[(*NumEdges)-2].Tval = T1;

		   double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	       double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	       if(ToTheRight)
		   {
		     Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
		     Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
		     EdgeList[(*NumEdges)-2].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	       }
	       else
		   {
     	     Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
		     Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
		     EdgeList[(*NumEdges)-2].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
		   }


		   EdgeList[(*NumEdges)-2].Edge = 555;
		   EdgeList[(*NumEdges)-2].Remx = RemTDiagx;
	       EdgeList[(*NumEdges)-2].Remy = RemTDiagz;


		   InsertDiagT = FALSE;
		   DiagRemTCalculated = TRUE;

		}

	  }

	  if(LTRInsertDiagT)
	  {

		if(LTRT1 <= CurT)
		{
		   if(ToTheRight)
	       {
	          LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
	          LTRRemTDiagz = fabs( ( (P1->X + LTRT1*PDx) - (ULX+U*width) ) / width );
	       }
       	   else
	       {
	          LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
 	          LTRRemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + LTRT1*PDx) ) / width ) );
	       }

		   // move the current entry up one
		   (*NumEdges)++;
		   EdgeList[(*NumEdges)-1].Tval   = EdgeList[(*NumEdges)-2].Tval;
		   EdgeList[(*NumEdges)-1].Height = EdgeList[(*NumEdges)-2].Height;
		   EdgeList[(*NumEdges)-1].Edge = EdgeList[(*NumEdges)-2].Edge;
		   EdgeList[(*NumEdges)-1].Remx = EdgeList[(*NumEdges)-2].Remx;
	       EdgeList[(*NumEdges)-1].Remy = EdgeList[(*NumEdges)-2].Remy;

           EdgeList[(*NumEdges)-2].Tval = LTRT1;

		   double TempRemTDiagx = LTRRemTDiagx - floor(LTRRemTDiagx);
	       double TempRemTDiagz = LTRRemTDiagz - floor(LTRRemTDiagz);
	       if(ToTheRight)
		   {
		     Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
		     Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
		     EdgeList[(*NumEdges)-2].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	       }
	       else
		   {
     	     Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
		     Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
		     EdgeList[(*NumEdges)-2].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
		   }


		   EdgeList[(*NumEdges)-2].Edge = 777;
		   EdgeList[(*NumEdges)-2].Remx = LTRRemTDiagx;
	       EdgeList[(*NumEdges)-2].Remy = LTRRemTDiagz;


		   LTRInsertDiagT = FALSE;
		   LTRDiagRemTCalculated = TRUE;

		}	// IF TVAL < CURT

	  }	 // INSERT

  } // while

  if(InsertDiagT)
  {
	if(ToTheRight)
	{
      RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	  RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX+U*width) ) / width );
	}
    else
	{
	  RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	  RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	}

	(*NumEdges)++;
    EdgeList[(*NumEdges)-1].Tval = T1;

	double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	if(ToTheRight)
    {
	  Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
	  Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	}
	else
    {
      Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
	  Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
    }

	EdgeList[(*NumEdges)-1].Edge = 555;
    EdgeList[(*NumEdges)-1].Remx = RemTDiagx;
	EdgeList[(*NumEdges)-1].Remy = RemTDiagz;

  }	// if insert

  if(LTRInsertDiagT)
  {
	if(ToTheRight)
	{
      LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
	  LTRRemTDiagz = fabs( ( (P1->X + LTRT1*PDx) - (ULX+U*width) ) / width );
	}
    else
	{
	  LTRRemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + LTRT1*PDz) ) / width ) );
 	  LTRRemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + LTRT1*PDx) ) / width ) );
	}

	(*NumEdges)++;
    EdgeList[(*NumEdges)-1].Tval = LTRT1;

	double TempRemTDiagx = LTRRemTDiagx - floor(LTRRemTDiagx);
	double TempRemTDiagz = LTRRemTDiagz - floor(LTRRemTDiagz);
	if(ToTheRight)
    {
	  Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*TempRemTDiagz;
	  Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	}
	else
    {
      Height1 = (Addr + SL_CACHE_SIZE + 1)->Height + ((Addr + SL_CACHE_SIZE)->Height - (Addr + SL_CACHE_SIZE + 1)->Height)*TempRemTDiagz;
	  Height2 = (Addr + 1)->Height + ((Addr)->Height - (Addr + 1)->Height)*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
    }

	EdgeList[(*NumEdges)-1].Edge = 777;
    EdgeList[(*NumEdges)-1].Remx = LTRRemTDiagx;
	EdgeList[(*NumEdges)-1].Remy = LTRRemTDiagz;

  }	// if insert


  // Store last element T = 1.0
  U = floor((P2->X - floor(TerrainLoadedULLocation.X))*OneOverPixelSize);
  V	= floor((P2->Z - floor(TerrainLoadedULLocation.Z))*OneOverPixelSize);

  Addr = &TerrainBuffer[U + V*SL_CACHE_SIZE];

  XMod = fmod(double(P2->X - ULX),width);
  YMod = fmod(double(P2->Z - ULY),fabs(height));

  (*NumEdges)++;
  EdgeList[(*NumEdges)-1].Tval = 1.0;
  Height1 = (Addr + SL_CACHE_SIZE)->Height + ((Addr + SL_CACHE_SIZE + 1)->Height - (Addr + SL_CACHE_SIZE)->Height)*(XMod/width);
  Height2 = (Addr)->Height + ((Addr + 1)->Height - (Addr)->Height)*(XMod/width);
  EdgeList[(*NumEdges)-1].Height = Height2 + (Height1 - Height2)*(YMod/width);

  RemoveNegativeHeights();

} // done

//***********************************************************************************************************************************
// *****

void GetTValForLineIntersect(FPoint *A1, FPoint *A2, FPoint *B1, FPoint *B2, double *TVal, int *Parallel)
{
  FPointDouble NormVec;
  float Numer, Denom;
  float DDx,DDz;

  DDx = B2->X - B1->X;
  DDz = B2->Z - B1->Z;

  NormVec.X =  DDz;
  NormVec.Z = -DDx;

  Numer = NormVec.X*(A1->X - B2->X) + NormVec.Z*(A1->Z - B2->Z);
  Denom = -( NormVec.X*(A2->X - A1->X) + NormVec.Z*(A2->Z - A1->Z) );

  if( (Denom > 0.0001) || (Denom < -0.0001) )
    *TVal = Numer/Denom;
  else
	*Parallel = TRUE;

}

//***********************************************************************************************************************************
// *****

void ClipToTerrainBuffer(FPoint *P1, FPoint *P2)
{
  FPoint UEdgeLeft,UEdgeRight,LEdgeLeft,LEdgeRight,Temp;
  double TVal;
  int Parallel;

  UEdgeLeft.X = TerrainLoadedULLocation.X;
  UEdgeLeft.Y = 0;
  UEdgeLeft.Z = floor(TerrainLoadedULLocation.Z);

  UEdgeRight.X = TerrainLoadedULLocation.X + SL_CACHE_WIDTH;
  UEdgeRight.Y = 0;
  UEdgeRight.Z = floor(TerrainLoadedULLocation.Z);

  LEdgeLeft.X = TerrainLoadedULLocation.X;
  LEdgeLeft.Y = 0;
  LEdgeLeft.Z = floor(TerrainLoadedULLocation.Z + SL_CACHE_WIDTH);

  LEdgeRight.X = TerrainLoadedULLocation.X + SL_CACHE_WIDTH;
  LEdgeRight.Y = 0;
  LEdgeRight.Z = floor(TerrainLoadedULLocation.Z + SL_CACHE_WIDTH);

  GetTValForLineIntersect(P1,P2,&UEdgeLeft,&UEdgeRight,&TVal,&Parallel);

  if(!Parallel)
  {
      if((TVal >= 0.0) && (TVal <= 1.0))
	  {
        P2->X = P1->X + (P2->X - P1->X)*TVal;
	    P2->Z = P1->Z + (P2->Z - P1->Z)*TVal;
        return;
	  }
  }

  GetTValForLineIntersect(P1,P2,&UEdgeRight,&LEdgeRight,&TVal,&Parallel);

  if(!Parallel)
  {
      if((TVal >= 0.0) && (TVal <= 1.0))
	  {
        P2->X = P1->X + (P2->X - P1->X)*TVal;
	    P2->Z = P1->Z + (P2->Z - P1->Z)*TVal;
        return;
	  }
  }

  GetTValForLineIntersect(P1,P2,&LEdgeLeft,&LEdgeRight,&TVal,&Parallel);

  if(!Parallel)
  {
      if((TVal >= 0.0) && (TVal <= 1.0))
	  {
        P2->X = P1->X + (P2->X - P1->X)*TVal;
	    P2->Z = P1->Z + (P2->Z - P1->Z)*TVal;
        return;
	  }
  }

  GetTValForLineIntersect(P1,P2,&UEdgeLeft,&LEdgeLeft,&TVal,&Parallel);

  if(!Parallel)
  {
      if((TVal >= 0.0) && (TVal <= 1.0))
	  {
        P2->X = P1->X + (P2->X - P1->X)*TVal;
	    P2->Z = P1->Z + (P2->Z - P1->Z)*TVal;
        return;
	  }
  }
}

//***********************************************************************************************************************************
// *****

 void TerEdgeDetection(FPoint *P1, FPoint *P2, int *NumEdges, EdgeListEntryType *EdgeList, int *SwappedEndPoints, int UsedForLOS=FALSE)
{
  double DxDz,DzDx;
  double NewX, NewZ, CurX, CurZ, NextX, NextZ, PrevX, PrevZ,BackX,BackZ;
  double width  =  float(MediumHeightCache->PixelSize);
  double height =  float(-MediumHeightCache->PixelSize);
  double StartLength,LineLength;
  double CurT,Tx,Tz,Tdx,Tdz;
  FPointDouble Diag1,Diag2,NormVec;
  double Numer,Denom;
  double T1,T2;
  int   ParallelToDiag=FALSE;
  int   InsertDiagT=FALSE;
  double PDx,PDz,DDx,DDz;
  FPointDouble Temp;
  double RemTx, RemTz, RemTDiagx, RemTDiagz, RemTDiagdx,RemTDiagdz,RemTdx, RemTdz;

  int ToTheRight;
  int LineCrossed;
  short *Addr,*PrevAddr;
  int U,V;
  int ZRemTCalculated =   TRUE;
  int XRemTCalculated =   TRUE;
  int DiagRemTCalculated = FALSE;
  int DeltaAddrX;
  int DeltaAddrZ;
  double ULX,ULY;
  double Height1, Height2;
  double XMod,YMod;

  *NumEdges = 0;

  *SwappedEndPoints = FALSE;

  // sort endpoints
  if(P2->Z > P1->Z)
  {
	Temp.X  = P2->X;   Temp.Z = P2->Z;
	P2->X   = P1->X;   P2->Z   = P1->Z;
	P1->X   = Temp.X; P1->Z   = Temp.Z;
	*SwappedEndPoints = TRUE;
  }

  if(UsedForLOS)
  {
    ClipToTerrainBuffer(P1,P2);
  }

  CurX = P1->X;
  CurZ = P1->Z;

  PDx = P2->X - P1->X;
  PDz = P2->Z - P1->Z;

  DzDx = -fabs( (P2->Z - P1->Z) / (P2->X - P1->X) );
  DxDz = (P2->X - P1->X)  / -(P2->Z - P1->Z);

  LineLength  = sqrt((P1->X-P2->X)*(P1->X-P2->X) + (P1->Z-P2->Z)*(P1->Z-P2->Z));

  Tdx = sqrt((DzDx*width)*(DzDx*width) + width*width)/LineLength;
  Tdz = sqrt((DxDz*height)*(DxDz*height) + height*height)/LineLength;

  RemTdx = fabs((Tdx*PDz)/width);
  RemTdz = fabs((Tdz*PDx)/width);

  // set up address walk
  ToTheRight = (P2->X - P1->X > 0);

  if(ToTheRight)
	DeltaAddrX =  1;
  else
	DeltaAddrX = -1;

  DeltaAddrZ = -256;

  U = floor((P1->X - MediumHeightCache->BLoadedULX)*MediumHeightCache->OneOverPixelSize);
  V	= floor((P1->Z - MediumHeightCache->BLoadedULY)*MediumHeightCache->OneOverPixelSize);

  ULX = MediumHeightCache->BLoadedULX;
  ULY = MediumHeightCache->BLoadedULY;

  Addr = (short *) (&MediumHeightCache->Buffer[0] + (V*256+U)*2);

  XMod = fmod(double(CurX - ULX),width);
  YMod = fmod(double(CurZ - ULY),fabs(height));

  // Store first element T = 0.0
  (*NumEdges)++;
  EdgeList[(*NumEdges)-1].Tval = 0.0;
   Height1 = *(Addr + 256) + (*(Addr + 256 + 1) - *(Addr + 256))*(XMod/width);
  Height2 = *(Addr) + (*(Addr + 1) - *(Addr))*(XMod/width);
  EdgeList[(*NumEdges)-1].Height = Height2 + (Height1 - Height2)*(YMod/width);

  if(P2->X >= P1->X)
	 if(XMod > 0)
	 {
      NextX = CurX + width - XMod;
	  PrevX = CurX - XMod;
	 }
	 else
	   NextX = PrevX = CurX;
  else
	 if(XMod > 0)
	 {
      NextX = CurX - XMod;
	  PrevX = CurX + width - XMod;
	 }
	 else NextX = PrevX = CurX;

  if(YMod > 0)
  {
     NextZ = CurZ - YMod;
	 PrevZ = CurZ + fabs(height) - YMod;
  }
  else NextZ = PrevZ = CurZ;

  NewX = NextX;
  NewZ = CurZ - fabs((NextX - CurX)*DzDx);
  LineCrossed = X_LINE;

  if(NewZ < NextZ)
  {
    NewZ = NextZ;
	NewX = CurX + (CurZ - NextZ)*DxDz;
	LineCrossed = Y_LINE;
  }

  Diag2.X = NextX;
  Diag2.Z = NextZ;
  Diag1.X = PrevX;
  Diag1.Z = PrevZ;

  if(P2->X < P1->X)
  {
 	Temp.X  = Diag2.X;
	Diag2.X = Diag1.X;
	Diag1.X = Temp.X;
  }

  if(Diag2.Z == PrevZ)
	Diag1.Z += width;

  if(Diag1.Z == NextZ)
	 Diag2.Z -= width;

  DDx = Diag2.X - Diag1.X;
  DDz = Diag2.Z - Diag1.Z;

  NormVec.X =  DDz;
  NormVec.Z = -DDx;

  Numer = NormVec.X*(P1->X - Diag2.X) + NormVec.Z*(P1->Z - Diag2.Z);
  Denom = -( NormVec.X*(P2->X - P1->X) + NormVec.Z*(P2->Z - P1->Z) );

  if( (Denom > 0.0001) || (Denom < -0.0001) )
    T1 = Numer/Denom;
  else
  {
    T1 = 0.0;
	ParallelToDiag = TRUE;
  }

  T2 = fabs(width/-(PDx + PDz));

  RemTDiagdx = fabs((T2*PDz)/width);
  RemTDiagdz = fabs((T2*PDx)/width);

  StartLength = sqrt((CurX-NewX)*(CurX-NewX) + (CurZ-NewZ)*(CurZ-NewZ));

  CurT = StartLength/LineLength;

  if( (CurT > T1) && (T1 > 0.0) && (T1 < 1.0) )
  {
	DiagRemTCalculated = TRUE;
    if(ToTheRight)
	{
	  RemTDiagx = fabs( ( ( (ULY + (V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	  RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX + U*width) ) / width );
	}
	else
	{
	  RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	  RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	}

	(*NumEdges)++;
	EdgeList[(*NumEdges)-1].Tval = T1;

	double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	if(ToTheRight)
    {
	  Height1 = *(Addr + 256) + (*(Addr + 256 + 1) - *(Addr + 256))*TempRemTDiagz;
	  Height2 = *(Addr) + (*(Addr + 1) - *(Addr))*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	}
	else
    {
      Height1 = *(Addr + 256 + 1) + (*(Addr + 256) - *(Addr + 256 + 1))*TempRemTDiagz;
	  Height2 = *(Addr + 1) + (*(Addr) - *(Addr + 1))*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
    }

	EdgeList[(*NumEdges)-1].Edge = 555;
	EdgeList[(*NumEdges)-1].Remx = RemTDiagx;
	EdgeList[(*NumEdges)-1].Remy = RemTDiagz;


	if(LineCrossed == X_LINE)
	{
	  RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
	  ZRemTCalculated = FALSE;
	  Addr += DeltaAddrX;
	  (*NumEdges)++;
      EdgeList[(*NumEdges)-1].Tval = CurT;
	  if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = *(Addr+256) + (*Addr - *(Addr+256))*(RemTx - floor(RemTx));
	  else
		 EdgeList[(*NumEdges)-1].Height = *(Addr+256+1) + (*(Addr+1) - *(Addr+256+1))*(RemTx - floor(RemTx));
	  EdgeList[(*NumEdges)-1].Remx = RemTx;
	  EdgeList[(*NumEdges)-1].Edge = 111;

    }
	else
	{
	  XRemTCalculated = FALSE;
	  Addr += DeltaAddrZ;
	  (*NumEdges)++;
      EdgeList[(*NumEdges)-1].Tval = CurT;

	  if(ToTheRight)
	  {
	    RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
		EdgeList[(*NumEdges)-1].Height = *(Addr+256) + (*(Addr+256+1) - *(Addr+256))*(RemTz - floor(RemTz));
	  }
	  else
	  {
		RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );
		EdgeList[(*NumEdges)-1].Height = *(Addr+256+1) + (*(Addr+256) - *(Addr+256+1))*(RemTz - floor(RemTz));
	  }

	  EdgeList[(*NumEdges)-1].Remy = RemTz;
	  EdgeList[(*NumEdges)-1].Edge = 222;

	}
  }
  else
  {
	if( (T1 > 0) && (T1 < 1.0) )
	 InsertDiagT = TRUE;

	if( (CurT > 0) && (CurT < 1.0) )
	{

	  if(LineCrossed == X_LINE)
	  {
	    RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
	    ZRemTCalculated = FALSE;
		Addr += DeltaAddrX;
	    (*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;
		if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = *(Addr+256) + (*Addr - *(Addr+256))*(RemTx - floor(RemTx));
	    else
		 EdgeList[(*NumEdges)-1].Height = *(Addr+256+1) + (*(Addr+1) - *(Addr+256+1))*(RemTx - floor(RemTx));
		 EdgeList[(*NumEdges)-1].Remx = RemTx;
	     EdgeList[(*NumEdges)-1].Edge = 111;

      }
	  else
      {

		XRemTCalculated = FALSE;
	    Addr += DeltaAddrZ;
	    (*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;

	    if(ToTheRight)
	    {
	      RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
	      EdgeList[(*NumEdges)-1].Height = *(Addr+256) + (*(Addr+256+1) - *(Addr+256))*(RemTz - floor(RemTz));
	    }
	    else
	    {
	      RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );
	   	  EdgeList[(*NumEdges)-1].Height = *(Addr+256+1) + (*(Addr+256) - *(Addr+256+1))*(RemTz - floor(RemTz));

	    }

	   EdgeList[(*NumEdges)-1].Remy = RemTz;
	   EdgeList[(*NumEdges)-1].Edge = 222;

      }

	}

  }

  if(NewX == NextX)
  {
	if(CurZ != PrevZ)
	  BackX = CurX - (PrevZ - CurZ)*DxDz;
	else
	  BackX = CurX;

	BackZ = PrevZ;
 	Tz = -(sqrt((P1->X-BackX)*(P1->X-BackX) + (P1->Z-BackZ)*(P1->Z-BackZ)))/LineLength;
	Tx = CurT;
  }
  else
  {
    if(PrevX != CurX)
	  BackZ = CurZ + fabs( (PrevX - CurX)*DzDx);
	else
	  BackZ = CurZ;

	BackX = PrevX;
	Tx = -(sqrt((P1->X-BackX)*(P1->X-BackX) + (P1->Z-BackZ)*(P1->Z-BackZ)))/LineLength;
	Tz = CurT;
  }

  if(P2->X - P1->X == 0)
  {
	Tx  = 0.0f;
	Tdx = 1000.0f;
  }

  if(P2->Z - P1->Z == 0)
  {
	Tz  = 0.0f;
	Tdz = 1000.0f;
  }

  if(ParallelToDiag)
  {
	T1 = 0.0f;
	T2 = 1000.0f;
  }

  while( (Tx + Tdx < 1.0) || (Tz + Tdz < 1.0) || (T1 + T2 < 1.0) )
  {
	  PrevAddr = Addr;
      if( (Tz + Tdz < Tx + Tdx) && (Tz + Tdz < T1 + T2) )
	  {
		CurT = Tz = Tz + Tdz;
		Addr += DeltaAddrZ;
		if(!ZRemTCalculated)
		{
		  if(ToTheRight)
	        RemTz = fabs( ( (P1->X + CurT*PDx) - (ULX+U*width) ) / width );
	      else
	        RemTz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + CurT*PDx) ) / width ) );

		  ZRemTCalculated = TRUE;
		}
		else
		{
		  RemTz += RemTdz;
		}

		(*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;

		if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = *(Addr+256) + (*(Addr+256+1) - *(Addr+256))*(RemTz - floor(RemTz));
	    else
		 EdgeList[(*NumEdges)-1].Height = *(Addr+256+1) + (*(Addr+256) - *(Addr+256+1))*(RemTz - floor(RemTz));

		EdgeList[(*NumEdges)-1].Remy = RemTz;
	    EdgeList[(*NumEdges)-1].Edge = 222;

	  }
	  else if (Tx + Tdx < T1 + T2)
	  {
		CurT = Tx = Tx + Tdx;
		Addr += DeltaAddrX;
		if(!XRemTCalculated)
		{
		  RemTx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + CurT*PDz) ) / width ) );
		  XRemTCalculated = TRUE;
		}
		else
		{
		  RemTx += RemTdx;
		}

		(*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;

		if(ToTheRight)
	     EdgeList[(*NumEdges)-1].Height = *(Addr+256) + (*Addr - *(Addr+256))*(RemTx - floor(RemTx));
	    else
		 EdgeList[(*NumEdges)-1].Height = *(Addr+256+1) + (*(Addr+1) - *(Addr+256+1))*(RemTx - floor(RemTx));
		 EdgeList[(*NumEdges)-1].Remx = RemTx;
	     EdgeList[(*NumEdges)-1].Edge = 111;

  	  }
	  else
	  {
		CurT = T1 = T1 + T2;
		if(!DiagRemTCalculated)
		{
		   if(ToTheRight)
	       {
	          RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	          RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX+U*width) ) / width );
	       }
       	   else
	       {
	          RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	          RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	       }

		   DiagRemTCalculated = TRUE;

		}
		else
		{
		  RemTDiagx += RemTDiagdx;
		  RemTDiagz	+= RemTDiagdz;
		}

		(*NumEdges)++;
        EdgeList[(*NumEdges)-1].Tval = CurT;

   	    double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	    double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	    if(ToTheRight)
		{
		 Height1 = *(Addr + 256) + (*(Addr + 256 + 1) - *(Addr + 256))*TempRemTDiagz;
		 Height2 = *(Addr) + (*(Addr + 1) - *(Addr))*TempRemTDiagz;
		 EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 		}
	    else
		{
     	 Height1 = *(Addr + 256 + 1) + (*(Addr + 256) - *(Addr + 256 + 1))*TempRemTDiagz;
		 Height2 = *(Addr + 1) + (*(Addr) - *(Addr + 1))*TempRemTDiagz;
		 EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
		}

		EdgeList[(*NumEdges)-1].Edge = 555;
		EdgeList[(*NumEdges)-1].Remx = RemTDiagx;
     	EdgeList[(*NumEdges)-1].Remy = RemTDiagz;

	  }

	  if(InsertDiagT)
	  {
		if(T1 <= CurT)
		{
		   if(ToTheRight)
	       {
	          RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	          RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX+U*width) ) / width );
	       }
       	   else
	       {
	          RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	          RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	       }

		   // move the current entry up one
		   (*NumEdges)++;
		   EdgeList[(*NumEdges)-1].Tval   = EdgeList[(*NumEdges)-2].Tval;
		   EdgeList[(*NumEdges)-1].Height = EdgeList[(*NumEdges)-2].Height;
		   EdgeList[(*NumEdges)-1].Edge = EdgeList[(*NumEdges)-2].Edge;
		   EdgeList[(*NumEdges)-1].Remx = EdgeList[(*NumEdges)-2].Remx;
	       EdgeList[(*NumEdges)-1].Remy = EdgeList[(*NumEdges)-2].Remy;

           EdgeList[(*NumEdges)-2].Tval = T1;

		   double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	       double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	       if(ToTheRight)
		   {
		     Height1 = *(Addr + 256) + (*(Addr + 256 + 1) - *(Addr + 256))*TempRemTDiagz;
		     Height2 = *(Addr) + (*(Addr + 1) - *(Addr))*TempRemTDiagz;
		     EdgeList[(*NumEdges)-2].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	       }
	       else
		   {
     	     Height1 = *(Addr + 256 + 1) + (*(Addr + 256) - *(Addr + 256 + 1))*TempRemTDiagz;
		     Height2 = *(Addr + 1) + (*(Addr) - *(Addr + 1))*TempRemTDiagz;
		     EdgeList[(*NumEdges)-2].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
		   }

		   EdgeList[(*NumEdges)-2].Edge = 555;
		   EdgeList[(*NumEdges)-2].Remx = RemTDiagx;
	       EdgeList[(*NumEdges)-2].Remy = RemTDiagz;

		   InsertDiagT = FALSE;
		   DiagRemTCalculated = TRUE;
		}
	  }
  }

  if(InsertDiagT)
  {
	if(ToTheRight)
	{
      RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
	  RemTDiagz = fabs( ( (P1->X + T1*PDx) - (ULX+U*width) ) / width );
	}
    else
	{
	  RemTDiagx = fabs( ( ( (ULY+(V + 1)*width) - (P1->Z + T1*PDz) ) / width ) );
 	  RemTDiagz = fabs( ( ( (ULX+(U+1)*width) - (P1->X + T1*PDx) ) / width ) );
	}

	(*NumEdges)++;
    EdgeList[(*NumEdges)-1].Tval = T1;

	double TempRemTDiagx = RemTDiagx - floor(RemTDiagx);
	double TempRemTDiagz = RemTDiagz - floor(RemTDiagz);
	if(ToTheRight)
    {
	  Height1 = *(Addr + 256) + (*(Addr + 256 + 1) - *(Addr + 256))*TempRemTDiagz;
	  Height2 = *(Addr) + (*(Addr + 1) - *(Addr))*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
 	}
	else
    {
      Height1 = *(Addr + 256 + 1) + (*(Addr + 256) - *(Addr + 256 + 1))*TempRemTDiagz;
	  Height2 = *(Addr + 1) + (*(Addr) - *(Addr + 1))*TempRemTDiagz;
	  EdgeList[(*NumEdges)-1].Height = Height1 + (Height2 - Height1)*TempRemTDiagx;
    }

	EdgeList[(*NumEdges)-1].Edge = 555;
    EdgeList[(*NumEdges)-1].Remx = RemTDiagx;
	EdgeList[(*NumEdges)-1].Remy = RemTDiagz;
  }

  // Store last element T = 1.0
  U = floor((P2->X - MediumHeightCache->BLoadedULX)*MediumHeightCache->OneOverPixelSize);
  V	= floor((P2->Z - MediumHeightCache->BLoadedULY)*MediumHeightCache->OneOverPixelSize);

  Addr = (short *) (&MediumHeightCache->Buffer[0] + (V*256+U)*2);

  XMod = fmod(double(P2->X - ULX),width);
  YMod = fmod(double(P2->Z - ULY),fabs(height));

  (*NumEdges)++;
  EdgeList[(*NumEdges)-1].Tval = 1.0;
  Height1 = *(Addr + 256) + (*(Addr + 256 + 1) - *(Addr + 256))*(XMod/width);
  Height2 = *(Addr) + (*(Addr + 1) - *(Addr))*(XMod/width);
  EdgeList[(*NumEdges)-1].Height = Height2 + (Height1 - Height2)*(YMod/width);

  RemoveNegativeHeights();

 }

//***********************************************************************************************************************************
// *****

 float GetPercentReflected(FPoint P1, FPoint P2)
 {
	P1.Z = 0;
	P2.Z = 0;

    P1.Normalize();
    P2.Normalize();

    return( -(P1.X*P2.X + P1.Y*P2.Y) );
 }

//***********************************************************************************************************************************
// *****

void CalcInOut(FPoint *P1, FPoint *P2, int SwappedEndPoints)
{
  EdgeListEntryType *EdgeWalk;
  InOutType         *InOutWalk;
  FPoint V1,V2,V3,V4,Norm,EndPoint;
  float Length;
  float TEndPoint;
  float Numer,Denom;
  float T1,TIntersect;
  int   State = START;
  int   EndOfList,HeightTooLow, TTooLow;
  int   Done = FALSE;

	Norm.X = 0;
	Norm.Y = 1;
	Norm.Z = 0;

  NumInOut = 0;

  if(SwappedEndPoints)
	ReverseEdges();

  EdgeWalk  = EdgeList;
  InOutWalk = (InOutType *)EdgeList;

  Length  = sqrt((P1->X-P2->X)*(P1->X-P2->X) + (P1->Z-P2->Z)*(P1->Z-P2->Z));

  while( (EdgeWalk < &EdgeList[NumEdges-1]) && (!Done) )  // don't do the last one
  {
			V1.X = EdgeWalk->Tval*Length;
			V1.Y = EdgeWalk->Height - PlayerPlane->WorldPosition.Y;
			V2.X = (EdgeWalk + 1)->Tval*Length - EdgeWalk->Tval*Length;
			V2.Y = (EdgeWalk + 1)->Height - EdgeWalk->Height;

			Norm.X = -V2.Y;
			Norm.Y = V2.X;

			if(V1.Y > 0)
	  		Done = TRUE;

			if( (((V1.X*Norm.X) + (V1.Y*Norm.Y)) > 0) || (Done) )
			{
	  		V1.X = 0;
	  		V1.Y = PlayerPlane->WorldPosition.Y;

	  		V2.X = EdgeWalk->Tval*Length;
	  		V2.Y = EdgeWalk->Height;

 	  		Norm.X = 0;
	  		Norm.Y = 1;

	  		Numer = Norm.X*(V1.X - Length) + Norm.Y*(V1.Y - 0);
      	Denom = -( Norm.X*(V2.X - V1.X) + Norm.Y*(V2.Y - V1.Y) );

	  		if( (Denom > 0.0001) || (Denom < -0.0001) )
        		T1 = Numer/Denom;
      	else
      	{
         	 NumInOut = 0;   // JLM should never happen except at altitude 0
	     		 return;
      	}

      	EndPoint.X = (V2.X - V1.X)*T1;
	  		EndPoint.Y = 0;
	  		TEndPoint = EndPoint.X/Length;

	  		NumInOut++;
	  		InOutWalk->T = EdgeWalk->Tval;

	  		InOutWalk->Height = (int)EdgeWalk->Height;
	  		InOutWalk->Reflectance = 0.0;
	  		InOutWalk->InOut = OCCLUDED;
	  		InOutWalk++;

	  		EdgeWalk++;
	  		EndOfList = (EdgeWalk == &EdgeList[NumEdges-1]);

	  		if(EndOfList)
				{
 					NumInOut++;
 					InOutWalk->T = 1.0;
 					InOutWalk->Height = 0;
 					InOutWalk->Reflectance = 0;
 					InOutWalk->InOut = OCCLUDED;
					return;
				}

	  		TTooLow = (EdgeWalk->Tval > TEndPoint);

	  		if( (!TTooLow) && (!Done) )
	  		{
	     			HeightTooLow = EdgeWalk->Height <
		                 			((TEndPoint - EdgeWalk->Tval)/TEndPoint)*PlayerPlane->WorldPosition.Y;

		 				while(HeightTooLow && !EndOfList)
	     			{
		   				EdgeWalk++;

		   				if(EdgeWalk->Tval > TEndPoint)
							  HeightTooLow = FALSE;
		   				else
		      			HeightTooLow = EdgeWalk->Height <
		                 				((TEndPoint - EdgeWalk->Tval)/TEndPoint)*PlayerPlane->WorldPosition.Y;

	       				EndOfList    = (EdgeWalk >= &EdgeList[NumEdges-1]);
	     			}
	  		}

	  		if(EndOfList)
	  		{
 					NumInOut++;
 					InOutWalk->T = 1.0;
 					InOutWalk->Height = 0;
 					InOutWalk->Reflectance = 0;
 					InOutWalk->InOut = OCCLUDED;
	   		  return;  // we are done
	  		}
	  		else if( (TTooLow) || (Done) )
	  		{
 				  EdgeWalk--; // backup one. end of loop will increment
	  		}
	  		else
	  		{
		 				V1.X = 0;
		 				V1.Y = PlayerPlane->WorldPosition.Y;

		 				V2.X = EndPoint.X;
		 				V2.Y = 0;

		 				V3.X = (EdgeWalk-1)->Tval*Length;
		 				V3.Y = (EdgeWalk-1)->Height;

		 				V4.X = (EdgeWalk)->Tval*Length;
		 				V4.Y = (EdgeWalk)->Height;

  		 				Norm.X = -(V4.Y - V3.Y);
		 				Norm.Y =  (V4.X - V3.X);

		 				Numer = Norm.X*(V1.X - V4.X) + Norm.Y*(V1.Y - V4.Y);
         				Denom = -( Norm.X*(V2.X - V1.X) + Norm.Y*(V2.Y - V1.Y) );

		 				if( (Denom > 0.0001) || (Denom < -0.0001) )
           		T1 = Numer/Denom;
         		else
		 				{
           		T1 = 0.0;  // debug JLM this should not happen
         	    NumInOut = 0;
           		return;
		 				}

         		TIntersect = (EndPoint.X*T1)/Length;

		 				NumInOut++;
	     		  InOutWalk->T = TIntersect;

		 				InOutWalk->Height = (int)EdgeWalk->Height;

		 				FPoint TempV1;

		 				TempV1.X = V2.X;
		 				TempV1.Y = -V1.Y;

		 				InOutWalk->Reflectance = GetPercentReflected(TempV1,Norm);
	     			InOutWalk->InOut = NOT_OCCLUDED;
	     			InOutWalk++;
	  		}

			}
			else
			{
	  			NumInOut++;
      		InOutWalk->T = EdgeWalk->Tval;

      		InOutWalk->Height = EdgeWalk->Height;
	  			InOutWalk->Reflectance = GetPercentReflected(V1, Norm);
      		InOutWalk->InOut = NOT_OCCLUDED;
	  			InOutWalk++;
			}

	    EdgeWalk++;
  }

 	NumInOut++;
 	InOutWalk->T = 1.0;
 	InOutWalk->Height = EdgeList[NumEdges-1].Height;
 	InOutWalk->Reflectance = (InOutWalk-1)->Reflectance;
 	InOutWalk->InOut = (InOutWalk-1)->InOut;
}

//***********************************************************************************************************************************
//******

void RemoveNegativeHeights()
{
  EdgeListEntryType *EdgeWalk;
  EdgeListEntryType *EdgeWalkLast;

  EdgeWalk     = EdgeList;
  EdgeWalkLast = &EdgeList[NumEdges-1];

  while(EdgeWalk <= EdgeWalkLast)
  {
    if(EdgeWalk->Height < 0)
		EdgeWalk->Height = 0;
    EdgeWalk++;
  }
}
//***********************************************************************************************************************************
//******

void ReverseEdges()
{
  EdgeListEntryType *EdgeWalkFront;
  EdgeListEntryType *EdgeWalkBack;
  float TempT,TempHeight;

  EdgeWalkFront = EdgeList;
  EdgeWalkBack = &EdgeList[NumEdges-1];

  while(EdgeWalkFront < EdgeWalkBack)
  {
	TempT      = EdgeWalkFront->Tval;
	TempHeight = EdgeWalkFront->Height;

	EdgeWalkFront->Tval   = 1.0 - EdgeWalkBack->Tval;
	EdgeWalkFront->Height = EdgeWalkBack->Height;

	EdgeWalkBack->Tval   = 1.0 - TempT;
	EdgeWalkBack->Height = TempHeight;

	EdgeWalkFront++;
	EdgeWalkBack--;
  }

}

//***********************************************************************************************************************************
// *****

void ComputeAGBeamEndPointsAndAngles(float Radius, float BeamAzim, FPoint *P1, FPoint *P2, float *EndAngle)
{
  float WorldUnitsPerPixel = Radius/108.0;
  FPoint PlanePos;
  float Azim;

  Azim = NormDegree(PlayerPlane->Heading/DEGREE + BeamAzim);

  PlanePos.X = PlayerPlane->WorldPosition.X;
  PlanePos.Z = PlayerPlane->WorldPosition.Z;

  P1->X  = PlanePos.X;
  P1->Z  = PlanePos.Z;

  ProjectPoint(PlanePos.X, PlanePos.Z, Azim, Radius,&(P2->X), &(P2->Z));

  *EndAngle   = RadToDeg(atan((Radius)/PlayerPlane->WorldPosition.Y));

}

//grprims.cpp
void GrDrawRampedLinePolyBuff( GrBuff *dest, float x, float y,float x2, float y2, FPoint Color1, FPoint Color2, float alpha=1.0f);
void GrDrawRampedPolyBuff( GrBuff *dest, int numpts, float *pXs, float *pYs, FPointData *pColor, float alpha);


//***********************************************************************************************************************************
void InterpolateSpan(int Y, int X1, int X2, int Color1, int Color2, float alpha)
{
	/*
	FPoint C1, C2;
	float scale = 1.0f/255.0f;


	Color1 = 0;
	C1.X = (float)GET_CURRENT_RED( Color1) * scale;
	C1.Y = (float)GET_CURRENT_GREEN( Color1) * scale;
	C1.Z = (float)GET_CURRENT_BLUE( Color1)* scale;

  Color2 = 60;
	C2.X = (float)GET_CURRENT_RED( Color2) * scale;
	C2.Y = (float)GET_CURRENT_GREEN( Color2) * scale;
	C2.Z = (float)GET_CURRENT_BLUE( Color2)* scale;


	GrDrawRampedLinePolyBuff( GrBuffFor3D, X1, Y, X2, Y, C1, C2, alpha);
	*/
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetAGTerrainAvoidance()
{
	FPointDouble TopVecEnd,BottomVecStart,BottomVecEnd,IntersectionTop,IntersectionBottom;
  FMatrix Mat;

	Mat.SetRPH(0,PlayerPlane->Pitch,PlayerPlane->Heading);

	float Dist = -(Av.AGRadar.Range*NMTOWU);

	TopVecEnd.SetValues(Dist*Mat.m_Data.RC.R0C2,Dist*Mat.m_Data.RC.R1C2,Dist*Mat.m_Data.RC.R2C2);

	TopVecEnd += PlayerPlane->WorldPosition;

	float TempPitch = NormDegree( (PlayerPlane->Pitch/DEGREE) - 45.0 )*DEGREE;

	Mat.SetRPH(0,TempPitch,PlayerPlane->Heading);

	Dist = -(707.0*FTTOWU);  // 707 = 500 ft / cos(45)
	BottomVecStart.SetValues(Dist*Mat.m_Data.RC.R0C2,Dist*Mat.m_Data.RC.R1C2,Dist*Mat.m_Data.RC.R2C2);
	BottomVecStart += PlayerPlane->WorldPosition;

	Mat.SetRPH(0,PlayerPlane->Pitch,PlayerPlane->Heading);

  float Dist1 = (Av.AGRadar.Range*NMTOWU);
	float Dist2 = ((500.0*FTTOWU)/(Av.AGRadar.Range*NMTOWU))*Dist1;

	Dist = -(Dist1-Dist2);

	BottomVecEnd.SetValues(Dist*Mat.m_Data.RC.R0C2,Dist*Mat.m_Data.RC.R1C2,Dist*Mat.m_Data.RC.R2C2);

	BottomVecEnd += BottomVecStart;

  InOutType *InOutWalk;
  InOutWalk = (InOutType *)EdgeList;

  NumInOut = 0;

	int TopLos    = LOS(&(PlayerPlane->WorldPosition),&TopVecEnd, &IntersectionTop, MED_RES_LOS, TRUE);
	int BottomLos = LOS(&BottomVecStart,&BottomVecEnd, &IntersectionBottom, MED_RES_LOS, TRUE);

	if(TopLos && BottomLos)
	{
	  NumInOut = 2;

	  InOutWalk->T = 0.0;
		InOutWalk->Height = IntersectionBottom.Y;
   	InOutWalk->Reflectance = 0.0;
	  InOutWalk->InOut = OCCLUDED;

		InOutWalk++;

	  InOutWalk->T = 1.0;
		InOutWalk->Height = IntersectionBottom.Y;
   	InOutWalk->Reflectance = 0.0;
	  InOutWalk->InOut = OCCLUDED;
	}
	else if(!TopLos && !BottomLos)
	{
	  NumInOut = 2;

	  InOutWalk->T = 0.0;
		InOutWalk->Height = IntersectionBottom.Y;
   	InOutWalk->Reflectance = 1.0;
	  InOutWalk->InOut = NOT_OCCLUDED;

		InOutWalk++;

	  InOutWalk->T = 1.0;
		InOutWalk->Height = IntersectionBottom.Y;
   	InOutWalk->Reflectance = 1.0;
	  InOutWalk->InOut = NOT_OCCLUDED;
	}
	else if(TopLos && !BottomLos)
	{
		 FPointDouble Vec;
		 float LengthToIntersection,TopLength;

		 Vec = IntersectionBottom;
		 Vec -= BottomVecStart;

		 LengthToIntersection = Vec.Length();

		 Vec =  TopVecEnd;
		 Vec -= PlayerPlane->WorldPosition;

		 TopLength = Vec.Length();

		 float TToUse = (LengthToIntersection + 500.0*FTTOWU)/(TopLength);

/*
		 NumInOut = 2;

	   InOutWalk->T = TToUse;
		 InOutWalk->Height = IntersectionBottom.Y;
   	 InOutWalk->Reflectance = 0.5;
	   InOutWalk->InOut = NOT_OCCLUDED;

		 InOutWalk++;

	   InOutWalk->T = 1.0;
		 InOutWalk->Height = IntersectionBottom.Y;
   	 InOutWalk->Reflectance = 0.0;
	   InOutWalk->InOut = OCCLUDED;
 *
 */

		 NumInOut = 4;

		 InOutWalk->T = 0.0;
		 InOutWalk->Height = IntersectionBottom.Y;
   	 InOutWalk->Reflectance = 0.0;
	   InOutWalk->InOut = OCCLUDED;

		 InOutWalk++;

		 InOutWalk->T = TToUse;
		 InOutWalk->Height = IntersectionBottom.Y;
   	 InOutWalk->Reflectance = 0.0;
	   InOutWalk->InOut = OCCLUDED;

		 InOutWalk++;

		 InOutWalk->T = TToUse+0.1;
		 InOutWalk->Height = IntersectionBottom.Y;
   	 InOutWalk->Reflectance = 0.5;
	   InOutWalk->InOut = NOT_OCCLUDED;

		 InOutWalk++;

	   InOutWalk->T = 1.0;
		 InOutWalk->Height = IntersectionBottom.Y;
   	 InOutWalk->Reflectance = 0.5;
	   InOutWalk->InOut = NOT_OCCLUDED;
	}

}


//***********************************************************************************************************************************
// *****

int CheckInOutIntegrity()
{
  InOutType *InOutWalk;

  InOutWalk = InOutList;

  int Index = NumInOut;

  while(Index-- > 0)
  {
    if(InOutWalk->T > 1.0)
      return(FALSE);
	InOutWalk++;
  }

  return(TRUE);
}


#define RBM_BLACK	(14.0f/255.0f)
#define RBM_WHITE   (0.9 - RBM_BLACK)
#define RBM_COLOR(x)  (x*RBM_WHITE + RBM_BLACK)



//***********************************************************************************************************************************
void UpdateScanBeamEdgeTable( void )
{
	float NewX,NewZ;
	float Dx,Dz;
	float XStep,CurX,CurZ;
	InOutType *InOutWalk;
	int    SwappedEndPoints;
	PixelListType *pEdge;
	ScanEdge *pScanBeam;
	int *pStartY;
	FPointDouble ScanWorldPosition;
	FPointDouble BeamVector;
	FMatrix back_mat;

	float our_height = PlayerPlane->WorldPosition.Y - 50.0f FEET;
	float our_lowest_height = PlayerPlane->WorldPosition.Y - 500.0f FEET;


	if(ScanBeamEdge <0 || ScanBeamEdge >NUM_SCAN_LINES)
		return;

	pScanBeam = &ScanBeamEdges[ScanBeamEdge];
	//ZeroMemory( pScanBeam->Edge, sizeof( pScanBeam->Edge) );
	pEdge = pScanBeam->Edge;
	pStartY = &pScanBeam->StartY;
	*pStartY = (SCAN_LINE_LENGTH - 1);

	NumEdges = 0;
	NumInOut = 0;

	BeamP1   = PlayerPlane->WorldPosition;
	BeamP1.Y = 0;

	BeamP2 = Av.AGRadar.BeamEndPointWorld;

	NumEdges = 0;

	int ClippedLine;
	HiResTerEdgeDetection(&BeamP1,&BeamP2, &NumEdges, &EdgeList[0], &SwappedEndPoints, &ClippedLine);
	if(ClippedLine)
		TerEdgeDetection(&BeamP1,&BeamP2, &NumEdges, &EdgeList[0], &SwappedEndPoints);

	CalcInOut(&BeamP1,&BeamP2, SwappedEndPoints);

	ProjectPoint(Origin.X, Origin.Y, NormDegree(Av.AGRadar.AntAzimuth),Av.AGRadar.Range*PixelsPerMile(Av.AGRadar.Range), &NewX, &NewZ);

	Dx = NewX - Origin.X;
	Dz = NewZ - Origin.Y;

	XStep = Dx/-Dz;  // Dz is always negative
	CurX = 0;
	CurZ = 0;

	int   PrevZ;
	float ReflectStep, CurReflect;
	float HeightStep, CurHeight;

	InOutWalk = InOutList;

	int LastState = InOutWalk->InOut;
	int j=0;
	float reflect_mult;

	if (Av.AGRadar.Range > 70.0)
        reflect_mult = 2.0f;
	else
		if (Av.AGRadar.Range > 30.0)
            reflect_mult = 1.6f;
		else
            reflect_mult = 1.2f;

	if(Av.AGRadar.CurMode == AG_TA_MODE)
	{

		FPoint x,y,z;

	DoubleVector3D	IfVelocity;		// velocity in inertial frame (double) - true airspeed


		z.SetValues(-PlayerPlane->IfVelocity.X,-PlayerPlane->IfVelocity.Y,-PlayerPlane->IfVelocity.Z);
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

		back_mat.m_Data.RC.R0C0 = x.X;
		back_mat.m_Data.RC.R1C0 = x.Y;
		back_mat.m_Data.RC.R2C0 = x.Z;

		back_mat.m_Data.RC.R0C1 = y.X;
		back_mat.m_Data.RC.R1C1 = y.Y;
		back_mat.m_Data.RC.R2C1 = y.Z;

		back_mat.m_Data.RC.R0C2 = z.X;
		back_mat.m_Data.RC.R1C2 = z.Y;
		back_mat.m_Data.RC.R2C2 = z.Z;

		back_mat.Transpose();

	}

	j = NumInOut;


//	while(!InOutWalk->InOut)
//		j--;

	BeamVector.MakeVectorToFrom(Av.AGRadar.BeamEndPointWorld,PlayerPlane->WorldPosition);

	while(j--)
	{
		if(!LastState)
		{
			PrevZ = CurZ;
			CurZ = Dz*InOutWalk->T;

			for(int i=(Origin.Y + PrevZ-1); i>=(Origin.Y + CurZ); i--)
			{
				if( i<0 || i> 130)
					return;

				//Save Edge Values
				(pEdge+i)->X	= CurX + Origin.X;
				(pEdge+i)->ColorPercent = RBM_BLACK;
				(pEdge+i)->Valid= TRUE;
				if( i< *pStartY)
					*pStartY = i;

				CurX += XStep;
			}
		 }
		 else
		 {
			if(InOutWalk != InOutList)
			{
			 ReflectStep = ((InOutWalk)->Reflectance - (InOutWalk-1)->Reflectance)/fabs(Dz*InOutWalk->T - CurZ);
			 CurReflect =  (InOutWalk-1)->Reflectance;

			 HeightStep = ((InOutWalk)->Height - (InOutWalk-1)->Height)/fabs(Dz*InOutWalk->T - CurZ);
			 CurHeight =  (InOutWalk-1)->Height;

			}
			else
			{
			  ReflectStep = 0;
			  CurReflect =  (InOutWalk+1)->Reflectance;

			  HeightStep = 0;
			  CurHeight =  (InOutWalk+1)->Height;

			}

			while(CurZ >= Dz*InOutWalk->T)
			{
				float Color;

				ScanWorldPosition.SetValues(CurZ/Dz,BeamVector);

				if(Av.AGRadar.CurMode == AG_TA_MODE)
				{
//					ScanWorldPosition.Y = LandHeight(ScanWorldPosition.X,ScanWorldPosition.Z);
//					ScanWorldPosition -= PlayerPlane->WorldPosition;
					// This would be faster but less accurate
					ScanWorldPosition.Y = CurHeight-PlayerPlane->WorldPosition.Y;
					ScanWorldPosition *= back_mat;

					if (ScanWorldPosition.Y < -500.0 FEET)
						Color = RBM_BLACK;
					else
						if (ScanWorldPosition.Y < -50.0 FEET)
							Color = RBM_COLOR(0.3f);
						else
							Color = RBM_COLOR(1.0f);
				}
				else
				{
					ScanWorldPosition += PlayerPlane->WorldPosition;

					if( (Av.AGRadar.CurMode == AG_SEA_MODE) && (!Av.AGRadar.InterleaveOn) )
					{
					 	if( InWater (ScanWorldPosition.X,ScanWorldPosition.Z) )
						 	Color = RBM_BLACK; // we can experiment with this color
					 	else
                			Color = RBM_COLOR(0.4);
					}
					else
					{
					 	if( InWater (ScanWorldPosition.X,ScanWorldPosition.Z) )
						 	Color = RBM_BLACK; // we can experiment with this color
					 	else
                			Color = RBM_COLOR(CurReflect*reflect_mult);
					}
				}

					//Save Edge Values
				if(Color > RBM_COLOR(1.0f))
					Color = RBM_COLOR(1.0f);
				if( Color <RBM_COLOR(0.0f))
					Color = RBM_COLOR(0.0f);


				int scanline = (int)(Origin.Y+CurZ);
				if( scanline<0 || scanline>130)
					return;

				(pEdge+scanline)->X				= CurX + Origin.X;
				(pEdge+scanline)->ColorPercent	= Color;
				(pEdge+scanline)->Valid			= TRUE;
				if( scanline < *pStartY)
					*pStartY = scanline;

				CurHeight += HeightStep;
				CurReflect += ReflectStep;
				CurZ--;
				CurX += XStep;
		   }
		}
		LastState = InOutWalk->InOut;
		InOutWalk++;
	}

	if( pScanBeam->StartY)
	{
		pEdge = &pScanBeam->Edge[*pStartY-1];
		while( pEdge >= pScanBeam->Edge)
			(pEdge--)->Valid = FALSE;
	}
}


#define AG_RADAR_TOP 		 20.0
#define AG_RADAR_LEFT        14.0
#define AG_RADAR_RIGHT       128.0
#define AG_RADAR_BOTTOM      127.0
#define AG_RADAR_CENTER_X    70.0
#define AG_RADAR_CENTER_Y    73.0


float CalcAvgColor( PixelListType *pOneLess,PixelListType *pEdge,PixelListType *pOneMore)
{
	float color;

	if ((Av.AGRadar.CurMode == AG_TA_MODE) || ((Av.AGRadar.CurMode == AG_SEA_MODE) && (!Av.AGRadar.InterleaveOn)))
	{
		float color1,color2,color3;

		color1 = __max(pOneLess->ColorPercent,__max((pOneLess-1)->ColorPercent,(pOneLess+1)->ColorPercent));
		color2 = __max(pEdge->ColorPercent,__max((pEdge-1)->ColorPercent,(pEdge+1)->ColorPercent));
		color3 = __max(pOneMore->ColorPercent,__max((pOneMore-1)->ColorPercent,(pOneMore+1)->ColorPercent));

		color = __max(color1,__max(color2,color3));
	}
	else
	{
		// calc the avg of a 3x3 grid no weight yets
		color = pOneLess->ColorPercent + (pOneLess-1)->ColorPercent + (pOneLess+1)->ColorPercent;
		color += pEdge->ColorPercent + (pEdge-1)->ColorPercent + (pEdge+1)->ColorPercent;
		color += pOneMore->ColorPercent + (pOneMore-1)->ColorPercent + (pOneMore+1)->ColorPercent;
		color *= 1.0f/9.0f;
	}

	return color;
}

typedef struct
{
	BOOL on;
	//FPoint Top, Left, Right, Bottom;
	FPoint p1, p2, p3, p4;
	FPoint p1p2, p2p3, p3p4, p4p1;

	float maxx, maxy;
	float minx, miny;


	FPoint Top,Left,Right,Bottom;

}RBZoomCoordinates;

RBZoomCoordinates RBZoom;
//***********************************************************************************************************************************
void ZoomRBMap( FPoint pt1, FPoint pt2, FPoint pt3, FPoint pt4)
{
	int bits = 0;
	BOOL got_one = TRUE;

	RBZoom.maxx = __max(pt1.X,__max(pt2.X,__max(pt3.X,pt4.X)));
	RBZoom.minx = __min(pt1.X,__min(pt2.X,__min(pt3.X,pt4.X)));
	RBZoom.maxy = __max(pt1.Y,__max(pt2.Y,__max(pt3.Y,pt4.Y)));
	RBZoom.miny = __min(pt1.Y,__min(pt2.Y,__min(pt3.Y,pt4.Y)));

	if (pt1.Y < pt2.Y)
		if (pt1.Y < pt3.Y)
			if (pt1.Y < pt4.Y)
			{
				bits = 0x1;
				RBZoom.Top = pt1;
			}
			else
			{
				bits = 0x8;
				RBZoom.Top = pt4;
			}
		else
			if (pt3.Y < pt4.Y)
			{
				bits = 0x4;
				RBZoom.Top = pt3;
			}
			else
			{
				bits = 0x8;
				RBZoom.Top = pt4;
			}
	else
		if (pt2.Y < pt3.Y)
			if (pt2.Y < pt4.Y)
			{
				bits = 0x2;
				RBZoom.Top = pt2;
			}
			else
			{
				bits = 0x8;
				RBZoom.Top = pt4;
			}
		else
			if (pt3.Y < pt4.Y)
			{
				bits = 0x4;
				RBZoom.Top = pt3;
			}
			else
			{
				bits = 0x8;
				RBZoom.Top = pt4;
			}


	got_one = FALSE;

	if (bits & 1)
	{
		if (pt2.X > pt3.X)
			if (pt2.X > pt4.X)
			{
				bits |= 2;
				RBZoom.Right = pt2;
			}
			else
			{
				bits |= 8;
				RBZoom.Right = pt4;
			}
		else
			if (pt3.X > pt4.X)
			{
				bits |= 4;
				RBZoom.Right = pt3;
			}
			else
			{
				bits |= 8;
				RBZoom.Right = pt4;
			}
	}
	else
		if (bits & 2)
		{
			if (pt1.X > pt3.X)
				if (pt1.X > pt4.X)
				{
					bits |= 1;
					RBZoom.Right = pt1;
				}
				else
				{
					bits |= 8;
					RBZoom.Right = pt4;
				}
			else
				if (pt3.X > pt4.X)
				{
					bits |= 4;
					RBZoom.Right = pt3;
				}
				else
				{
					bits |= 8;
					RBZoom.Right = pt4;
				}
		}
		else
			if (bits & 4)
			{
				if (pt1.X > pt2.X)
					if (pt1.X > pt4.X)
					{
						bits |= 1;
						RBZoom.Right = pt1;
					}
					else
					{
						bits |= 8;
						RBZoom.Right = pt4;
					}
				else
					if (pt2.X > pt4.X)
					{
						bits |= 2;
						RBZoom.Right = pt2;
					}
					else
					{
						bits |= 8;
						RBZoom.Right = pt4;
					}
			}
			else
				if (pt1.X > pt2.X)
					if (pt1.X > pt3.X)
					{
						bits |= 1;
						RBZoom.Right = pt1;
					}
					else
					{
						bits |= 4;
						RBZoom.Right = pt3;
					}
				else
					if (pt2.X > pt3.X)
					{
						bits |= 2;
						RBZoom.Right = pt2;
					}
					else
					{
						bits |= 4;
						RBZoom.Right = pt3;
					}



	got_one = FALSE;

	if (!(bits&1))
	{
		got_one = TRUE;
		RBZoom.Bottom = pt1;
	}

	if (!(bits&2))
	{
		if (!got_one)
		{
			got_one = TRUE;
			RBZoom.Bottom = pt2;
		}
		else
			if (pt2.X > RBZoom.Bottom.X)
			{
				RBZoom.Left = RBZoom.Bottom;
				RBZoom.Bottom = pt2;
			}
			else
				RBZoom.Left = pt2;
	}

	if (!(bits&4))
	{
		if (!got_one)
		{
			got_one = TRUE;
			RBZoom.Bottom = pt3;
		}
		else
			if (pt3.X > RBZoom.Bottom.X)
			{
				RBZoom.Left = RBZoom.Bottom;
				RBZoom.Bottom = pt3;
			}
			else
				RBZoom.Left = pt3;
	}

	if (!(bits&8))
	{
		if (!got_one)
		{
			got_one = TRUE;
			RBZoom.Bottom = pt4;
		}
		else
			if (pt4.X > RBZoom.Bottom.X)
			{
				RBZoom.Left = RBZoom.Bottom;
				RBZoom.Bottom = pt4;
			}
			else
				RBZoom.Left = pt4;
	}



//	RBZoom.maxx = pt1.X;
//	if( pt2.X> RBZoom.maxx)
//		RBZoom.maxx = pt2.X;
//	if( pt3.X> RBZoom.maxx)
//		RBZoom.maxx = pt3.X;
//	if( pt4.X> RBZoom.maxx)
//		RBZoom.maxx = pt4.X;

//	RBZoom.maxx+=1;

//	RBZoom.maxy = pt1.Y;
//	if( pt2.Y> RBZoom.maxy)
//		RBZoom.maxy = pt2.Y;
//	if( pt3.Y> RBZoom.maxy)
//		RBZoom.maxy = pt3.Y;

//	if( pt4.Y> RBZoom.maxy)
//		RBZoom.maxy = pt4.Y;

//	RBZoom.maxy+=1;

//	RBZoom.minx = pt1.X;
//	if( pt2.X< RBZoom.minx)
//		RBZoom.minx = pt2.X;

//	if( pt3.X< RBZoom.minx)
//		RBZoom.minx = pt3.X;

//	if( pt4.X< RBZoom.minx)
//		RBZoom.minx = pt4.X;


//	RBZoom.miny = pt1.Y;
//	if( pt2.Y< RBZoom.miny)
//		RBZoom.miny = pt2.Y;

//	if( pt3.Y< RBZoom.miny)
//		RBZoom.miny = pt3.Y;

//	if( pt4.Y< RBZoom.miny)
//		RBZoom.miny = pt4.Y;


//	RBZoom.p1 = pt1;
//	RBZoom.p2 = pt4;
//	RBZoom.p3 = pt3;
//	RBZoom.p4 = pt2;


////	RBZoom.p1.X += 1.0f;
////	RBZoom.p2.X += 1.0f;
////	RBZoom.p3.X += 1.0f;
////	RBZoom.p4.X += 1.0f;
////	RBZoom.p1.Y += 1.0f;
////	RBZoom.p2.Y += 1.0f;
////	RBZoom.p3.Y += 1.0f;
////	RBZoom.p4.Y += 1.0f;


//	if( RBZoom.p1.Y >RBZoom.p2.Y)
//		RBZoom.p1p2.X = RBZoom.minx;
//	else
//		RBZoom.p1p2.X = RBZoom.maxx;
//	RBZoom.p1p2.Y = RBZoom.miny;

//	if( RBZoom.p3.Y >RBZoom.p4.Y)
//		RBZoom.p3p4.X = RBZoom.minx;
//	else
//		RBZoom.p3p4.X = RBZoom.maxx;
//	RBZoom.p3p4.Y = RBZoom.maxy;


//	if( RBZoom.p2.X >RBZoom.p3.X)
//		RBZoom.p2p3.Y = RBZoom.maxy;
//	else
//		RBZoom.p2p3.Y = RBZoom.miny;
//	RBZoom.p2p3.X = RBZoom.maxx;


//	if( RBZoom.p4.X >RBZoom.p1.X)
//		RBZoom.p4p1.Y = RBZoom.maxy;
//	else
//		RBZoom.p4p1.Y = RBZoom.miny;
//	RBZoom.p4p1.X = RBZoom.minx;


	RBZoom.on = 1;

}

void ZoomRBMapOff( void)
{
	RBZoom.on = 0;
}

void GrDrawColorPoly( GrBuff *dest, int numpts, float *pXs, float *pYs, FPointData *Color,  float alpha);
void GrDrawLinePoly( GrBuff *dest, int numpts, float *pXs, float *pYs, FPointData *Color,  float alpha);

void make_rb_offsets(float &xoffset,float &yoffset,float &xscale, float &yscale,float &midx, float &midy)
{
	int width  =  AG_RADAR_RIGHT  - AG_RADAR_LEFT ;//GrBuffFor3D->pGrBuffPolygon->Width2D;//
	int height =  AG_RADAR_BOTTOM - AG_RADAR_TOP ;//GrBuffFor3D->pGrBuffPolygon->Height2D;/

	xoffset = GrBuffFor3D->pGrBuffPolygon->Width2D * 0.5f;
	yoffset = GrBuffFor3D->pGrBuffPolygon->Height2D * 0.5f;

	midx = ( RBZoom.maxx - RBZoom.minx )* 0.5f + RBZoom.minx ;//RBZoom.maxx - RBZoom.minxRBZoom.minx ;//+ AG_RADAR_LEFT;
	midy = ( RBZoom.maxy - RBZoom.miny )* 0.5f + RBZoom.miny ;//RBZoom.miny ;//+ AG_RADAR_TOP;
	xscale = width /( RBZoom.maxx - RBZoom.minx );//141=MAP_WIDTH
	yscale = height/( RBZoom.maxy - RBZoom.miny );
}

//***********************************************************************************************************************************
void DrawScanBeamEdgeTable( void )
{

	int StepAmount;  //Lod
	ScanEdge *pScanBeam, *pNextBeam, *pLastBeam;
	ScanEdge *pSOneBeamLess, *pSOneBeamMore;
	ScanEdge *pNOneBeamLess, *pNOneBeamMore;
	PixelListType *pEdge, *pNext;
	PixelListType *pEOneLess, *pEOneMore;
	PixelListType *pNOneLess, *pNOneMore;
	float alpha,alphaless;
	int numpts;
	float Xs[4];
	float Ys[4];
	FPointData Color[4];

	float xoffset, yoffset, xscale, yscale, midx, midy;

	if( RBZoom.on)
		make_rb_offsets(xoffset, yoffset, xscale, yscale, midx, midy);
	else
	{
		xoffset = 0;
		yoffset =0;
		xscale = yscale = 1.0f;
		midx = 0;
		midy = 0;
	}


	StepAmount = 3;


	pLastBeam = &ScanBeamEdges[NUM_SCAN_LINES-2];

	pScanBeam = &ScanBeamEdges[1];
	pSOneBeamLess = &ScanBeamEdges[0];
	pSOneBeamMore = &ScanBeamEdges[2];

	pNextBeam = pScanBeam+StepAmount;
	pNOneBeamLess = pSOneBeamLess+StepAmount;
	pNOneBeamMore = pSOneBeamMore+StepAmount;

	//Draw all rect from Scan beam edge table using iterated colors and alpha :)
	while( pNextBeam< pLastBeam)
	{

		 pEdge = pScanBeam->Edge;
		 pEOneLess = pSOneBeamLess->Edge;
		 pEOneMore = pSOneBeamMore->Edge;

		 pNext = pNextBeam->Edge;
		 pNOneLess = pNOneBeamLess->Edge;
		 pNOneMore = pNOneBeamMore->Edge;

		 //pEdge +=pScanBeam->StartY;
 		 //pNext +=pScanBeam->StartY;

		 alpha = pScanBeam->t/(float)(NUM_SCAN_LINES*2.0f);
		 alphaless = pNextBeam->t/(float)(NUM_SCAN_LINES*2.0f);

		 if( alpha>1.0f)
			 alpha = 1.0f;
		else
			if (alpha < 0.0f)
				alpha = 0.0f;

		if (alphaless > 1.0f)
			alphaless = 1.0f;
		else
			if (alphaless < 0.0f)
				alphaless = 0.0f;

		// walk top to bottom//pScanBeam->StartY
		 for( int y=0; y<(SCAN_LINE_LENGTH-StepAmount-1); y+=StepAmount)
		 {
			if( 1)// (pEdge->Valid && (pEdge+StepAmount)->Valid) && (pNext->Valid && (pNext+StepAmount)->Valid) )  //BOX case
			{

				numpts = 0;

				//Convert colors here
				if( pEdge->Valid)
				{
					Color[numpts].X = 0.0f;
					Color[numpts].Y =  (CalcAvgColor(pEOneLess,pEdge,pEOneMore ) - RBM_BLACK)*alpha + RBM_BLACK;//pEdge->ColorPercent;
					Color[numpts].Z = 0.0f;
					Xs[numpts] = (pEdge->X - midx) * xscale + xoffset;
					Ys[numpts++] = (y-midy) * yscale + yoffset;
				}

				if( pNext->Valid)
				{
					Color[numpts].X = 0.0f;
					Color[numpts].Y = (CalcAvgColor(pNOneLess,pNext,pNOneMore) - RBM_BLACK)*alphaless + RBM_BLACK;//pNext->ColorPercent;
					Color[numpts].Z = 0.0f;
					Xs[numpts] = (pNext->X- midx) * xscale + xoffset;
					Ys[numpts++] = (y- midy) * yscale + yoffset;
				}


				if( (pNext+StepAmount)->Valid)
				{
					Color[numpts].X = 0.0f;
					Color[numpts].Y = (CalcAvgColor(pNOneLess+StepAmount,(pNext+StepAmount),pNOneMore+StepAmount) - RBM_BLACK)*alphaless + RBM_BLACK;//(pNext+StepAmount)->ColorPercent;
					Color[numpts].Z = 0.0f;
					Xs[numpts] = ((pNext+StepAmount)->X- midx) * xscale + xoffset;
					Ys[numpts++] = ((y+StepAmount)-midy) * yscale + yoffset;

				}

				if( (pEdge+StepAmount)->Valid)
				{

					Color[numpts].X = 0.0f;
					Color[numpts].Y = (CalcAvgColor(pEOneLess+StepAmount,(pEdge+StepAmount),pEOneMore+StepAmount) - RBM_BLACK)*alpha + RBM_BLACK;//(pEdge+StepAmount)->ColorPercent;
					Color[numpts].Z = 0.0f;
					Xs[numpts] = ((pEdge+StepAmount)->X- midx) * xscale + xoffset;
					Ys[numpts++] = ((y+StepAmount)-midy) * yscale + yoffset;
				}

				if( numpts>2)
					GrDrawColorPoly( GrBuffFor3D, numpts, Xs, Ys, Color, 1.0f);

			}
			else
			if( pEdge->Valid && pNext->Valid )
			{
				// InterpolateSpan(y, pEdge->X, pNext->X, pEdge->Color, pNext->Color, alpha);
				//DrawColoredBox(numpts, color0, color1, color2, color3);
			}
			pEdge += StepAmount;
			pNext += StepAmount;
		 	pEOneLess += StepAmount;
		 	pEOneMore += StepAmount;
		 	pNOneLess += StepAmount;
		 	pNOneMore += StepAmount;
		 }


			//Draw Top Edge
			int valid=1;
			float oostep = 1.0f/ StepAmount;

/*
			if( pScanBeamEdge->StartY > (pScanBeamEdge+StepAmount)->StartY )
			{
				int y =pScanBeamEdge->StartY  * oostep +1;
				y*= StepAmount;
				pEdge = &((pScanBeamEdge)->Edge[y]);
				valid &= pEdge->Valid;


				Color[0].X = 0.0f;
				Color[0].Y = (pEdge)->ColorPercent;
				Color[0].Z = 0.0f;
				Xs[0] = pEdge->X;
				Ys[0] = y;

				y =(pScanBeamEdge+StepAmount)->StartY * oostep +1;
				y*=  StepAmount;

				pEdge = &((pScanBeamEdge+StepAmount)->Edge[y]);
				valid &= pEdge->Valid;
				Color[1].X = 0.0f;
				Color[1].Y = (pEdge)->ColorPercent;
				Color[1].Z = 0.0f;
				Xs[1] = pEdge->X;
				Ys[1] = y;

				y = pScanBeamEdge->StartY * oostep +1;
				y *= StepAmount;
				pEdge = &((pScanBeamEdge+StepAmount)->Edge[y]);
				valid &= pEdge->Valid;
				Color[2].X = 0.0f;
				Color[2].Y = (pEdge)->ColorPercent;
				Color[2].Z = 0.0f;
				Xs[2] = pEdge->X;
				Ys[2]d = y;

				if( valid )
					GrDrawRampedPolyBuff( GrBuffFor3D, 3, Xs, Ys, Color, alpha);
			}
			else if( pScanBeamEdge->StartY < (pScanBeamEdge+StepAmount)->StartY )
			{

				y = pScanBeamEdge->StartY * oostep +1;
				y *= StepAmount;

				pEdge = &((pScanBeamEdge)->Edge[y]);
				valid &= pEdge->Valid;
				Color[0].X = 0.0f;
				Color[0].Y = (pEdge)->ColorPercent;
				Color[0].Z = 0.0f;
				Xs[0] = pEdge->X;
				Ys[0] = y;

				y = (pScanBeamEdge+StepAmount)->StartY * oostep +1;
				y *= StepAmount;

				pEdge = &((pScanBeamEdge+StepAmount)->Edge[y]);
				valid &= pEdge->Valid;
				Color[1].X = 0.0f;
				Color[1].Y = (pEdge)->ColorPercent;
				Color[1].Z = 0.0f;
				Xs[1] = pEdge->X;
				Ys[1] = y;

			//	y = (pScanBeamEdge+StepAmount)->StartY * oostep +1;
			//	y *= StepAmount;

				pEdge = &((pScanBeamEdge)->Edge[y]);
				valid &= pEdge->Valid;
				Color[2].X = 0.0f;
				Color[2].Y = (pEdge)->ColorPercent;
				Color[2].Z = 0.0f;
				Xs[2] = pEdge->X;
				Ys[2] = y;

				if( valid )
					GrDrawRampedPolyBuff( GrBuffFor3D, 3, Xs, Ys, Color, alpha);
			}

  */
		pScanBeam = pNextBeam;
		pSOneBeamLess = pNOneBeamLess;
		pSOneBeamMore = pNOneBeamMore;

		pNextBeam+= StepAmount;
		pNOneBeamLess += StepAmount;
		pNOneBeamMore += StepAmount;
	 }


}

#define RBX(x) ((x-midx)*xscale+xoffset)
#define RBY(y) ((y-midy)*yscale+yoffset)

#define MAX_X  (AG_RADAR_RIGHT+1)
#define MAX_Y  (AG_RADAR_BOTTOM+1)
#define MIN_X  (AG_RADAR_LEFT+1)
#define MIN_Y  (AG_RADAR_TOP+1)


void DrawMaskPolys()
{

	float Xs[4];
	float Ys[4];
	FPointData Color[4];

	if( RBZoom.on)
	{

		float xoffset, yoffset, xscale, yscale, midx, midy;

		make_rb_offsets(xoffset, yoffset, xscale, yscale, midx, midy);

		for( int i=0; i< 3; i++)
		{
			Color[i].X = 0.0f;
			Color[i].Y = 14.0f/255.0f;
			Color[i].Z = 0.0f;
		}

		Xs[0] = MIN_X;
		Ys[0] = MIN_Y;
		Xs[1] = MAX_X;
		Ys[1] = MIN_Y;
		Xs[2] = RBX(RBZoom.Top.X);
		Ys[2] = RBY(RBZoom.Top.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MAX_X;
		Ys[0] = MIN_Y;
		Xs[1] = RBX(RBZoom.Right.X);
		Ys[1] = RBY(RBZoom.Right.Y);
		Xs[2] = RBX(RBZoom.Top.X);
		Ys[2] = RBY(RBZoom.Top.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MAX_X;
		Ys[0] = MIN_Y;
		Xs[1] = MAX_X;
		Ys[1] = MAX_Y;
		Xs[2] = RBX(RBZoom.Right.X);
		Ys[2] = RBY(RBZoom.Right.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MAX_X;
		Ys[0] = MAX_Y;
		Xs[1] = RBX(RBZoom.Bottom.X);
		Ys[1] = RBY(RBZoom.Bottom.Y);
		Xs[2] = RBX(RBZoom.Right.X);
		Ys[2] = RBY(RBZoom.Right.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MAX_X;
		Ys[0] = MAX_Y;
		Xs[1] = MIN_X;
		Ys[1] = MAX_Y;
		Xs[2] = RBX(RBZoom.Bottom.X);
		Ys[2] = RBY(RBZoom.Bottom.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MIN_X;
		Ys[0] = MAX_Y;
		Xs[1] = RBX(RBZoom.Left.X);
		Ys[1] = RBY(RBZoom.Left.Y);
		Xs[2] = RBX(RBZoom.Bottom.X);
		Ys[2] = RBY(RBZoom.Bottom.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MIN_X;
		Ys[0] = MAX_Y;
		Xs[1] = MIN_X;
		Ys[1] = MIN_Y;
		Xs[2] = RBX(RBZoom.Left.X);
		Ys[2] = RBY(RBZoom.Left.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);

		Xs[0] = MIN_X;
		Ys[0] = MIN_Y;
		Xs[1] = RBX(RBZoom.Top.X);
		Ys[1] = RBY(RBZoom.Top.Y);
		Xs[2] = RBX(RBZoom.Left.X);
		Ys[2] = RBY(RBZoom.Left.Y);
		GrDrawColorPoly( GrBuffFor3D, 3, Xs, Ys, Color, 1.0f);


		Xs[0] = RBX(RBZoom.Top.X);
		Ys[0] = RBY(RBZoom.Top.Y);
		Xs[1] = RBX(RBZoom.Right.X);
		Ys[1] = RBY(RBZoom.Right.Y);
		Xs[2] = RBX(RBZoom.Bottom.X);
		Ys[2] = RBY(RBZoom.Bottom.Y);
		Xs[3] = RBX(RBZoom.Left.X);
		Ys[3] = RBY(RBZoom.Left.Y);

		for( i=0; i< 4; i++)
		{
			Color[i].X = 0.0;
			Color[i].Y = 1.0;
			Color[i].Z = 0.0;
		}


    	GrDrawLinePoly(GrBuffFor3D,4,Xs,Ys,Color,1.0f);

		Xs[0] = MIN_X;
		Ys[0] = MIN_Y;
		Xs[1] = MAX_X;
		Ys[1] = MIN_Y;
		Xs[2] = MAX_X;
		Ys[2] = MAX_Y;
		Xs[3] = MIN_X;
		Ys[3] = MAX_Y;

    	GrDrawLinePoly(GrBuffFor3D,4,Xs,Ys,Color,1.0f);
	}

}




 void ClearPixelList()
 {
 	ZeroMemory( ScanBeamEdges, sizeof( ScanBeamEdges) );
	ScanBeamEdge = 0;
 }

#define MAX_FILTERED_VEHICLES 100
MovingVehicleParams *FilteredVehicles[MAX_FILTERED_VEHICLES];
int NumFilteredVehicles;

//***********************************************************************************************************************************
void UpdateScanBeam()
{
	UpdateAGRadarScan();
}

//***********************************************************************************************************************************
// *****

void DoScanBeam()
{
  if(TimeExcel != 0) return;
  if(Av.AGRadar.FreezeOn) return;

  UpdateScanBeam();
  UpdateScanBeamEdgeTable();
}

//***********************************************************************************************************************************
// *****

void ResetAGRadarScanInfo()
{
	ZeroMemory( ScanBeamEdges, sizeof( ScanBeamEdges) );
	//Fix Rescaling of mode...ScanBeamEdge = 0;

	Origin.X = 70;
	Origin.Y = 127;
}

//***********************************************************************************************************************************
// *****

void  AGRadarShutDown(void)
{
  ScanBeamOn = FALSE;
  MediumHeightCache->SetViewDist(CACHE_HGT_DIST_NORM);
}

//***********************************************************************************************************************************
// *****

void  SetAGRadarFreeze(int Status)
{
// this function no longer used
}

//***********************************************************************************************************************************
// *****

float GetAGBeamAzim(void)
{
  // this function no longer used
	return(12);
}

//***********************************************************************************************************************************
// *****

float GetAGBeamAzimLimit(void)
{
// this function no longer used
	return(60.0);
}

//***********************************************************************************************************************************
// *****
int GetAGRange(void)
{
// this function no longer used
 return(20);
}

//***********************************************************************************************************************************
// *****

void  SetAGMode(int Mode)
{
// this function no longer used
}

//***********************************************************************************************************************************
// *****

void InitScanBeam()
{
	// this function no longer used
}

//***********************************************************************************************************************************
// *****

void CleanupScanBeam()
{
	// this function no longer used
}

//***********************************************************************************************************************************
// *****

void StopScan()
{
  ScanStop = TRUE;
}

//***********************************************************************************************************************************

float TIntersect;
float IntersectHeight;
int   IntersectYes;

//***********************************************************************************************************************************
// *****

void CalcIntersection(double ScanLength, FPointDouble *StartPoint, FPointDouble *EndPoint,
					  double Tval1, double Height1, double Tval2, double Height2, FPointDouble *Intersection)
{
   IntersectYes = TRUE;

   FPointDouble V1,V2,V3,V4,Norm;
   double Numer,Denom,T1;

   V1.X = 0;
   V1.Y = StartPoint->Y;

   V2.X = ScanLength;
   V2.Y = EndPoint->Y;

   V3.X = Tval1*ScanLength;
   V3.Y = Height1;

   V4.X = Tval2*ScanLength;
   V4.Y = Height2;

   Norm.X = -(V4.Y - V3.Y);
   Norm.Y =  (V4.X - V3.X);

   Numer = Norm.X*(V1.X - V4.X) + Norm.Y*(V1.Y - V4.Y);
   Denom = -( Norm.X*(V2.X - V1.X) + Norm.Y*(V2.Y - V1.Y) );

   if( (Denom > 0.0001) || (Denom < -0.0001) )
         T1 = Numer/Denom;
   else T1 = 1.0;  // should NEVER happen

   Intersection->X = StartPoint->X + T1*(EndPoint->X - StartPoint->X);
   Intersection->Y = StartPoint->Y + T1*(EndPoint->Y - StartPoint->Y);
   Intersection->Z = StartPoint->Z + T1*(EndPoint->Z - StartPoint->Z);

   // debug
   TIntersect      = T1;
   IntersectHeight = Intersection->Y;

}

//***********************************************************************************************************************************
// *****

int LOS(FPointDouble *FromPoint, FPointDouble *ToPoint, FPointDouble *Intersect, int ResToUse, int CalcIntersectPoint)
{
  double PlaneMinHeight, TerMaxHeight;
  double Dz;
  int SwappedEndPoints;
  FPointDouble StartPoint, EndPoint, Temp;
  EdgeListEntryType *EdgeWalk, *LastEdge, *FirstEdge;
  int EdgeStep;

  IntersectYes = FALSE;

  StartPoint = *FromPoint;
  EndPoint = *ToPoint;

  BeamP1.X = StartPoint.X;
  BeamP1.Y = 0;
  BeamP1.Z = StartPoint.Z;

  BeamP2.X = EndPoint.X;
  BeamP2.Y = 0;
  BeamP2.Z = EndPoint.Z;

//  if(ResToUse == HI_RES_LOS)
//  {
	int ClippedLine;
    HiResTerEdgeDetection(&BeamP1,&BeamP2, &NumEdges, &EdgeList[0], &SwappedEndPoints, &ClippedLine);
	if(ClippedLine)
	  TerEdgeDetection(&BeamP1,&BeamP2, &NumEdges, &EdgeList[0], &SwappedEndPoints, TRUE);
//  }
//  else
//    TerEdgeDetection(&BeamP1,&BeamP2, &NumEdges, &EdgeList[0], &SwappedEndPoints, TRUE);

  if(SwappedEndPoints)
  {
	LastEdge  = &EdgeList[0];
	FirstEdge = &EdgeList[NumEdges-1];
	EdgeStep  = -1;
  }
  else
  {
	LastEdge  = &EdgeList[NumEdges-1];
	FirstEdge = &EdgeList[0];
	EdgeStep = 1;
  }

  TerMaxHeight = -1000000;
  EdgeWalk = &EdgeList[0];
  while(EdgeWalk != &EdgeList[NumEdges-1])
  {
    if(EdgeWalk->Height > TerMaxHeight)
	{
	  TerMaxHeight = EdgeWalk->Height;
	}

	EdgeWalk++;
  }

  if(StartPoint.Y < EndPoint.Y)
	PlaneMinHeight = StartPoint.Y;
  else
	PlaneMinHeight = EndPoint.Y;

  if(PlaneMinHeight > TerMaxHeight)
	  return(TRUE);

  Dz = EndPoint.Y - StartPoint.Y;

  EdgeWalk = FirstEdge;

  while(EdgeWalk != LastEdge)
  {
	if(!SwappedEndPoints)
	{
      if(EdgeWalk->Height >= StartPoint.Y + Dz*EdgeWalk->Tval)
	  {
	     if(CalcIntersectPoint)
	     {
	       float LengthScan;

	       LengthScan = sqrt((BeamP1.X-BeamP2.X)*(BeamP1.X-BeamP2.X) +
		              (BeamP1.Z-BeamP2.Z)*(BeamP1.Z-BeamP2.Z));

		   CalcIntersection(LengthScan, &StartPoint, &EndPoint, (EdgeWalk-1)->Tval,(EdgeWalk-1)->Height,
			              (EdgeWalk)->Tval,(EdgeWalk)->Height, Intersect);

	     }

 		 return(FALSE);
	  }
	}
	else
	{
      if(EdgeWalk->Height >= StartPoint.Y + Dz*(1.0 - EdgeWalk->Tval) )
	  {
	     if(CalcIntersectPoint)
	     {
	       float LengthScan;

	       LengthScan = sqrt((BeamP1.X-BeamP2.X)*(BeamP1.X-BeamP2.X) +
		              (BeamP1.Z-BeamP2.Z)*(BeamP1.Z-BeamP2.Z));

		   CalcIntersection(LengthScan, &StartPoint, &EndPoint, 1.0 - (EdgeWalk+1)->Tval,(EdgeWalk+1)->Height,
			              1.0 - (EdgeWalk)->Tval,(EdgeWalk)->Height, Intersect);

	     }

		 return(FALSE);
	  }
	}

	EdgeWalk += EdgeStep;
  }

  return(TRUE);

}

//***********************************************************************************************************************************
// *****

void CopyEdges(InOutType *Edges, int *Num)
{
  // EdgeListEntryType *Walker;
  InOutType *Walker;

  Walker = &InOutList[0];

  *Num = NumInOut;

  if(NumInOut == 0)
	return;

  while(Walker <= &InOutList[NumInOut-1])
  {
 	Edges->Height = Walker->Height;
	Edges->T   = Walker->T;
	Edges->InOut = Walker->InOut;

	Edges++;
	Walker++;
  }

}

void CopyLOSEdges(EdgeListEntryType *Edges, int *Num, float *T, float *Height, int *IntersectOccured)
{
  EdgeListEntryType *Walker;

  Walker = &EdgeList[0];

  *Num = NumEdges;

  if(NumEdges == 0)
	return;

  while(Walker <= &EdgeList[NumEdges-1])
  {
 	Edges->Height = Walker->Height;
	Edges->Tval   = Walker->Tval;

	Edges++;
	Walker++;
  }

  *T = TIntersect;
  *Height = IntersectHeight;
  *IntersectOccured = IntersectYes;

}

void ToggleTerrainRes()
{
  ResToUse = !ResToUse;
}