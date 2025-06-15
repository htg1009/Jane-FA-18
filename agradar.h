// azims
#define FULL     0
#define HALF     1
#define QUART  2

// modes
#define RBM_MODE	    0
#define HRM_MODE	    3
#define GMT_MODE	    2
#define GMTI_MODE       1

#define MAP_SELECT_MODE 0
#define WAITING_MODE    1
#define HI_RES_MODE     2


#define MAP_MODE    0
#define TARGET_MODE 1

#define NO_AG_ERROR    0
#define DW_RANGE_LIMIT 1
#define GIMBLE_LIMIT   2
#define BLIND_ZONE     3

typedef struct
{
  float CurX;
  float CurZ;
  float StepX;
  float StepZ;
  float Lx;
  float Rx;
  float Lz;
  float Rz;
  float LDx;
  float LDz;
  float RDx;
  float RDz;
  float LStepX;
  float LStepZ;
  float RStepX;
  float RStepZ;
  float Heading;
} MapScanInfoType;

void DoAirToGroundRadar(int XOfs, int YOfs, int MpdNum, int MpdDetectNum);

//void InitAGRadar(void);
//void CleanupAGRadar(void);


float PixelsPerMile(float Range);
void ToggleShowTerrain();
int  GetHiResMapLoc(FPoint *BoxLoc);
void CalcBoxInWorldCoords(FPoint Origin, float Size, FPoint *Box);
void GetHiResScanInfo(MapScanInfoType *HiresScanInfo);
void EasyModeAGChangeTarget(void);
void AGRadarIncRange(void);
void AGRadarDecRange(void);
void EasyModeAGResetTarget(void);