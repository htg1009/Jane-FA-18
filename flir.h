#define FLIR_CAM_GROUND_TARGET 0
#define FLIR_CAM_PLANE_TARGET  1
#define FLIR_CAM_VEH_TARGET    2
#define FLIR_CAM_LOC_TARGET    3
#define FLIR_CAM_NO_TARGET     4

typedef struct
{
  float  SlewLeftRight;
  float  SlewUpDown;
  int    AquireMode;
  int    ModeToSelect;
  int    FOVToUse;
  int    ImageMode;
  int    TrackOn;
  int    DeclutterOn;
  int    ShortRangeOn;
  int    LaserState;
  int    TargetSelected;
  int    TrackPosObtained;
  FPointDouble TrackPos;
  FPointDouble Target;
  int TargetObtained;
  int Magnify;
  int TargetType;
  BasicInstance *GrndTrgt;
  MovingVehicleParams *VehTrgt;
  PlaneParams *PlaneTrgt;
  FPointDouble LocTrgt;
  int MaskOn;
  int CdesOn;
  int AutoSet;
  FPointDouble AGDesignate;
  FMatrix Matrix;
  int TimpactTimer;
  int PreTimpactTimer;
} FlirInfoType;


void InitMavGbu(void);
void InitFlir(void);
void InitMaverick(void);
void CleanupMavGbu(void);
void CleanupFlir(void);
void DoMaverick(int XOfs, int YOfs, int MpdNum, int MpdDetectNum);
void DoGbu(int XOfs, int YOfs, int MpdNum, int MpdDetectNum);
void DoFlir(int XOfs, int YOfs, int MpdNum, int MpdDetectNum);

void FireMav(int *TargetType, FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane, int *InKeyhole);
void FireGbu(int *TargetType, FPointDouble *TargetLoc, BasicInstance **GroundTarget,MovingVehicleParams **Veh, PlaneParams **Plane);
int IsAGMissleSelected(int *Type,int *SideOnLeft);
void GetFlirTargetPositionF15(int *TargetObtained,FPointDouble *Target);
void SetFlirLaseToSafe();
void GetMavGbuTargetPos(int Type, FPointDouble *Pos);

int IsGbuSteerable(void);

// key commands
void ToggleTFlirPolarity(void);
void ToggleTFlirFov(void);
void ToggleTFlirAquireMode(void);
void ToggleTFlirCdes(void);
void ToggleTFlirTrack(void);
void ToggleLaser(void);
void CenterFlir(void);

void ToggleWpnTrack(void);
void CenterWpn(void);
void ToggleWpnMode(void);
void ToggleWpnFov(void);

BOOL DoesLineIntersectPlane(FPointDouble *P1,FPointDouble *P2, FPointDouble *PointOnPlane, FPointDouble *Norm, double *TVal, FPointDouble *Intersect);