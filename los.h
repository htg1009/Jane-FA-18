#define OCCLUDED      0
#define NOT_OCCLUDED  1
#define START         3

#define HI_RES_LOS    0
#define MED_RES_LOS    1

#define MAP_LOS_SIZE     32.0
#define MAP_LOS_SIZE_INT 32

typedef struct
{
  float Tval;
  float Height;
  int   Edge;
  float Remx;
  float Remy;
 } EdgeListEntryType;

typedef struct
{
  float T;
  char  InOut;
  float   Height;
  float Reflectance;
} InOutType;

typedef struct
{
  PlaneParams *P;
  float       Range;        // in miles
  float       ScanLimit;   	// scan will be between + and - this number
  int         Mode;         //3 modes -- RBM, GMT, GMTI
} AGScanInfoType;

typedef struct
{
  float		   RangeInWorld;
  float        RangeInMiles;
  float		   ScanDirection; // LTOR, RTOL
  float		   ScanLimit;
  float        ScanSlope;
  int		   Mode;
  float		   Azim;
  float        AzimStep;
  float        ScanRate;
  unsigned int LstTicks;
  unsigned int ThisTicks;
  int          FirstTime;
  unsigned int DiffCount;
  int          FreezeOn;
} AGBeamInfoType;


void  TerEdgeDetection(FPoint *P1, FPoint *P2, int *NumEdges, EdgeListEntryType *EdgeList);
void  HiResTerEdgeDetection(FPoint *P1, FPoint *P2, int *NumEdges, EdgeListEntryType *EdgeList, int *SwappedEndPoints, int *ClippedLine);
void  AGRadarShutDown(void);
float GetAGBeamAzim(void);
float GetAGBeamAzimLimit(void);
int   GetAGRange(void);
void  SetAGMode(int Mode);
void  DoScanBeam(void);
void  InitScanBeam(void);
void  CleanupScanBeam(void);
void StopScan();
int LOS(FPointDouble *FromPoint, FPointDouble *ToPoint, FPointDouble *Intersect, int ResToUse, int CalcIntersectPoint);

void ResetAGRadarScanInfo();

// JLM debug function
void CopyEdges(InOutType *Edges, int *Num);
void CopyLOSEdges(EdgeListEntryType *Edges, int *Num, float *T, float *Height, int *IntersectOccured);
void ToggleTerrainRes();

void  SetAGRadarFreeze(int Status);

void GetTValForLineIntersect(FPoint *A1, FPoint *A2, FPoint *B1, FPoint *B2, double *TVal, int *Parallel);

