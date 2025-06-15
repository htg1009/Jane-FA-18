//*****************************************************************************************************************************************
// AA RADAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AA_NORMAL               0x00
#define AA_ON_RADAR				0x01
#define AA_PRIMARY              0x02
#define AA_SECONDARY            0x04
#define AA_RANKED               0x08
#define AA_AWACS                0x10
#define AA_NO_LANDS				0x20

// RWS,TWS,and VS must stasy in order
#define AA_RWS_MODE				      0
#define AA_TWS_MODE				      1
#define AA_VS_MODE					    2
#define AA_STT_MODE			        3
#define AA_RAID_MODE            4
#define AA_ACQ_MODE             5

// ACQ MODES
#define AA_WIDE_ACQ_MODE        0
#define AA_VERTICAL_ACQ_MODE    1
#define AA_BORE_SIGHT_MODE      2
#define AA_GUN_ACQ_MODE		      3

#define MPRF	0
#define HPRF	1
#define LPRF	3

#define SEARCH_H_MODE	 0
#define SEARCH_M_MODE	 1
#define SEARCH_I_MODE	 2

// note : THESE DEFINES CAN'T CHANGE!!! these defines are used in a sort routine to dort friendly from other categories
#define AA_HOSTILE   0
#define AA_AMBIGUOUS 1
#define AA_UNKNOWN   2
#define AA_FRIENDLY  3

#define AA_IFF_OFF         0
#define AA_IFF_INTERROGATE 1
#define AA_IFF_AUTO        2

#define AA_MAX_DET_PLANES 32

#define AA_MAX_AWACS_TARGETS 50

#define AA_MAX_RANKED 50

typedef struct
{
  int   PlaneId;
  float OfsX,OfsY;
  int   ShouldDraw;
  int   Designation;
  int   TimeAdded;
  float Heading;
	float RelAngle;
	float RelPitch;
	int   Rank;
	int   IDLevel;
//	int   AwacsReported;
	int   AOT;
	int   Color;
	float Shade;
	FPointDouble Loc;
	DWORD RadarFrame;
} DetectedPlaneType;

typedef struct
{
  int NumPlanes;
	Timer FadeTimer;
	float ColorFadePercent;
  DetectedPlaneType Planes[AA_MAX_DET_PLANES];
} DetectedPlaneListType;

typedef struct
{
	int   IDLevel;
	float TTG;
	int   AOT;
	DetectedPlaneType *P;
} AARankingType;

typedef struct
{
	int NumPlanes;
	AARankingType Planes[AA_MAX_RANKED];
} AARankingListType;

typedef struct
{
	int PlaneId;
	int IDLevel;
	FPointDouble WorldPosition;
	float RelAngle;
	float RelVel;
} AwacsTargetType;

typedef struct
{
	int NumTargets;
	AwacsTargetType Targets[AA_MAX_AWACS_TARGETS];
} AwacsTargetListType;

// JLM remove, this is so external code will compile
typedef struct
{
  int   CurTargetId;
  int   PrevTargetId;
  int   TargetReselected;
  int   CompSelectedTargetId;
  int   ShouldDraw;
  float OfsX, OfsY;
} CurSelectedTargetInfoType;

typedef struct
{
	class Timer  JammerTimer;
	int TargetJamming;
	int   JammerFakeOut;
	int   JammerDirection;
	int   JammerYPos;
	int   JammerXPos;
	float Heading;
} AAJammerType;

typedef struct
{
	 Timer BreakLockTimer;
	 int   BrokeLockReaquire;
	 FPointDouble LastPos;
	 FPointDouble CurPos;
	 float Velocity;  // world units per sec
	 int   ReaquireYPos;
	 int   ReaquireXPos;
	 float Heading;
} AAReaquireType;

typedef struct
{
  	int   Id;
	int   IDLevel;
	float NCTRPercentPerSec;
	float NCTRPercent;
	class Timer NCTRTimer;
	int   ShouldDraw;
	float XOfs,YOfs;
	float RelAngle;
	AAJammerType Jammer;
	AAReaquireType Reaquire;
	Timer CriticalAspectTimer;
} TargetInfoType;

typedef struct
{
	FPointDouble Pos;
	int   Active;
	int   KeyActive;
	class Timer Timer;
} CaptBarsType;

int GetCurrentPrimaryTarget();
int GetCurrentAARadarMode();

int GetCurrentAATargetIndex();

void DoAirToAirRadar(int MpdNum);
void InitAvionicsModes(void);

void AARadarDesignateTarget(void);
void AARadarUnDesignateTarget(void);

void InterrogateIffPlanes(void);
void ManualIffPlanes(void);

int GetAARadarMode();
int GetCurrentAARadarMode();
int SetAARadarToSTTMode();
void AARadarAutoAcqTarget();
void AABreakLock();

void ToggleSilent();

PlaneParams *GetTargetForAim120();


//*****************************************************************************************************************************************
// AG RADAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AG_RBM_MODE  0
#define AG_GMT_MODE  1
#define AG_SEA_MODE  2
#define AG_TA_MODE   3
#define AG_MAP_MODE  4
#define AG_GMTT_MODE 5
#define AG_FTT_MODE  6

#define AG_EXP1 0
#define AG_EXP2 1
#define AG_EXP3 2

#define AG_MAX_DET_GROUND_OBJS 300
#define AG_MAX_DET_VEHICLES    200

#define AG_REGISTERED_OBJECT 0
#define AG_VEHICLE           1
#define AG_SHIP              2
#define AG_LOCATION          3

#define AG_DESIGNATE_FROM_FLIR       0
#define AG_DESIGNATE_FROM_RADAR      1
#define AG_DESIGNATE_FROM_EASY_RADAR 2
#define AG_DESIGNATE_FROM_HUD        3
#define AG_DESIGNATE_FROM_GPS        4


typedef struct
{
  int   Id;
  float OfsX,OfsY;
  int   ShouldDraw;
	FPointDouble Loc;
	BasicInstance *Obj;
  float Heading;
	float RelAngle;
	int   Color;
	float Shade;
	DWORD RadarFrame;
} DetectedGroundObjType;

typedef struct
{
  int NumGroundObj;
	Timer FadeTimer;
	float ColorFadePercent;
  DetectedGroundObjType GroundObjs[AG_MAX_DET_GROUND_OBJS];
} DetectedGroundObjListType;

typedef struct
{
  int   Id;
  float OfsX,OfsY;
	FPointDouble Loc;
	MovingVehicleParams *Veh;
	int   IsMoving;
  int   ShouldDraw;
  float Heading;
	float RelAngle;
	int   Color;
	float Shade;
	DWORD RadarFrame;
} DetectedVehicleType;

typedef struct
{
  int NumVehicles;
	Timer FadeTimer;
	float ColorFadePercent;
  DetectedVehicleType Vehicles[AG_MAX_DET_VEHICLES];
} DetectedVehicleListType;

void DoAGRadar(int MpdNum);
void InitAGRadar(void);
void CleanupAGRadar(void);
void AGRadarDesignateTarget(void);
void UpdateAGRadarScan(void);
void BreakAGLock(void);

void AGToggleSilent();



//*****************************************************************************************************************************************
// WEAP STORES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define WEAP_STORES_CCIP_MODE 0
#define WEAP_STORES_AUTO_MODE 1
#define WEAP_STORES_MAN_MODE  2

#define WEAP_STORES_FREE_FALL_MODE 0
#define WEAP_STORES_RETARDED_MODE  1

#define WEAP_STORES_SINGLE_METHOD        0
#define WEAP_STORES_SALVO_METHOD         1
#define WEAP_STORES_RIPPLE_SINGLE_METHOD 2
#define WEAP_STORES_RIPPLE_MULT_METHOD   3

#define WEAP_STORES_MODE_READY 	 0
#define WEAP_STORES_DRAG_READY 	 1
#define WEAP_STORES_QTY_READY  	 2
#define WEAP_STORES_MULT_READY 	 3
#define WEAP_STORES_INV_READY  	 4
#define WEAP_STORES_HOB_READY  	 5
#define WEAP_STORES_FLT_READY  	 6
#define WEAP_STORES_TERM_READY   7
#define WEAP_STORES_SEEK_READY   8
#define WEAP_STORES_SEARCH_READY 9
#define WEAP_STORES_TGT_READY    10

#define HARM_SP_MODE   0
#define HARM_TOO_MODE  1
#define HARM_PB_MODE   2

#define ROCKET_POD_SINGLE 0
#define ROCKET_POD_SALVO  1

#define HARPOON_RBL_MODE 0
#define HARPOON_BOL_MODE 1

#define WEAP_STORES_SKIM 0
#define WEAP_STORES_POP  1

#define WEAP_STORES_LARGE 0
#define WEAP_STORES_MED   1
#define WEAP_STORES_SMALL 2

#define WEAP_STORES_HIGH 0
#define WEAP_STORES_MED  1
#define WEAP_STORES_LOW  2

#define WEAP_STORES_PB_MODE  0
#define WEAP_STORES_TOO_MODE 1

#define WEAP_STORES_MAX_RANGE_ERROR 0
#define WEAP_STORES_ALT_ERROR       1
#define WEAP_STORES_OFF_AXIS_ERROR  2
#define WEAP_STORES_NO_TARGET_ERROR 3
#define WEAP_STORES_MIN_RANGE_ERROR 4

typedef struct
{
	int Mode;
	int Qty;
	int Mult;
	int Inv;
	int Drag;
	int Hob;
} BombProgType;

typedef struct
{
	int Mode;
	int Flt;
	int Term;
	int Seek;
	int Search;
} HarpoonProgType;

typedef struct
{
	int Mode;
	int Flt;
	int Term;
	int Tgt;
} SlamerProgType;

typedef struct
{
	int Mode;
	int Tgt;
} JsowProgType;

typedef struct
{
	int Tgt;
} HarmProgType;

typedef struct
{
	int ReadyForProg;
	int CurProg;
	int InZone;
	int Condition;
	int TimeToMaxInSec;
} DumbBombType;

typedef struct
{
	int Mode;
	int FireMethod;
	int NumStations;
} RocketPodType;

typedef struct
{
	int Aim7LoftOn;
	int Aim120VisualOn;
	int AGunOn;
  int GunRate;
	int DSTBOn;
	int GunsDataOn;
	int HarmOverride;
	int FloodOn;
	Timer FloodTimer;
	float FloodTime;
	int GunsOn;
	int AGunCanFire;
	int FunnelOn;
	int LastCamStation;
	FPoint CamPosition;
	DumbBombType DumbBomb;
	DumbBombType ClusterBomb;
	DumbBombType Harpoon;
	DumbBombType Slamer;
	DumbBombType Jsow;
	DumbBombType Jdam;
	DumbBombType Harm;
	RocketPodType RocketPod;
} WeapStoresType;

void FireMaveric(int *TargetType, FPointDouble *TargetLoc,void **Target,int*InKeyhole);
void FireWalleye(int *TargetType, FPointDouble *TargetLoc,void **Target);
void GetWeapCamTargetPosition(WeapCamType *CamInfo, FPointDouble *Target);
void GetHarpoonData(float *RangeInMiles, int *Seek);

//*****************************************************************************************************************************************
// FLIR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define FLIR_LSR_SAFE   0
#define FLIR_LSR_ARMED  1
#define FLIR_LSR_FIRING 2
#define FLIR_LSR_MASKED 3

void FlirMode(int MpdNum);

//*****************************************************************************************************************************************
// TEWS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define TEWS_MISSLE_THREAT   0
#define TEWS_PLANE_THREAT    1
#define TEWS_SAM_PROV_THREAT 2
#define TEWS_SAM_WEAP_THREAT 3
#define TEWS_VEH_THREAT      4
#define TEWS_SHIP_THREAT     5

typedef struct
{
	int NumChaff;
	int NumFlares;
	int Repeat;
	float Interval;
} TewsProgType;

void TewsMode(int MpdNum);

//*****************************************************************************************************************************************
// COLORS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AV_PALETTE_MAX_COLORS 4
#define AV_PALETTE_NUM_SHADES 50

#define AV_GREEN  0
#define AV_YELLOW 1
#define AV_RED    2
#define AV_BLUE   3

typedef struct
{
	float Red;
	float Green;
	float Blue;
} AvRGB;

void CreateAvColorRamp(void);
void GetRGBFromAVPalette(int Color,float Shade,AvRGB *Triple,int Normalize=0);

void AddTargetToAwacs(int PlaneIndex, FPointDouble WorldPosition, float Heading, float RelVelocity, int IDLevel);
void ClearAwacsTargetList(void);

//*****************************************************************************************************************************************
// MPD INTERACTIONS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int Pressed;
	int MpdNum;
	int ButtonNum;
} MpdButtonPressType;

void MpdButtonPress(int MpdNum, int ButtonNum);
void InitMpdInteractions(void);
int GetButtonPress(int MpdNum);

//*****************************************************************************************************************************************
// MISC MPD FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawTextAtLoc(int ScrX, int ScrY,  char *pMpdString,int Color, float Shade, int UseLarge=0);
void DrawVertWordAtLocation(int ScrX, int ScrY,  char *pMpdString, int BoxIt,int Color, float Shade);
void DrawVerticalWordAtButton(int MpdDetectNum,int ButtonNum, char *Word, int BoxIt,int Color, float Shade);

void SetAcqMode(int Mode);
void SlewAAAntenna(int Dir);
int  SetTDCMpd(int MpdNum,int X, int Y);
void ToggleTDCMpd();
int  GetTDCMode();


void ResetMaveric(void);
void ResetWalleye(void);


void InitWeapStores(void);
void DoWeaponStoresMode(int MpdNum);

void CageUnCageWeapon(int Type);

void MavDesignate(void);
void MavUnDesignate(void);

void GbuDesignate(void);
void GbuUnDesignate(void);

void FlirDesignate(void);
void FlirUnDesignate(void);
void FlirBoreSightKeypress(void);
void FlirToggleFov(void);
void ToggleFlirLaser(void);

void HarmUnDesignate(void);
void HarmClick(int XOff,int YOff);
void SimHarmSP(void);

void ResetBombProgram(AvWeapEntryType *Weap);

void MainMenuMode(int MpdNum);

int  IsDecoyJamming(void);
int  IsICSJamming(void);

void SimCMDDDecoy(void);

void SimCounterMeasures(void);

void ToggleTewsProgramStep(void);
void CounterMeasuresProgramInitiate(void);
void ToggleCMDDSTransmit(void);
void ToggleICS(void);
void ToggleCMDDSDecoyControl(void);
void ToggleIDECM(void);

void DropSingleChaff(void);
void DropSingleFlare(void);

int GetAGWeapPageAndModeForHud(int *Page, int *Mode);
int GetHarmInfoForHud(FPointDouble *TargetLocation,float *MinRange, float *MaxRange);

void GetCoupledModePosition(FPointDouble *Loc);

int GetFlirTargetPosition(FPointDouble *Target);


void MainAvionicsTick(void);

void FillFuelTanks(int LI, float LO, float RI, float RO);

void AddDamageToBit(int Damage, int MessageType);
void SetBitMessage(int Damage,int MessageType);
void DamageCMDDS();

void ToggleDumpFuel(void);

void CheckUFCMpdClick(int MpdNum, float X, float Y);

int GetWeaponData(int Page,int *FlightProfile,void **Target, int *TargetType, FPointDouble *Loc);
int TargetInZone(int Page);
int GetGPSLocation(int Page, FPointDouble *Loc);

void SetACLDataForAvionics(float CmdAirspeed, float CmdAlt, float CmdRod, float HorzAdjust, float VertAdjust);

void SetAndSortTewsThreats(void);

void PlayerSttTargetJamming();

void AvionicsSaveData(HKEY hKey);
void AvionicsLoadData(HKEY hKey);


void UFCMode(int ModNum);
void HSIMode(int MpdNum);
void AdiMode(int MpdNum);
void HudMode(int MpdNum);
void EngMode(int MpdNum);
void BitMode(int MpdNum);
void AclMode(int MpdNum);
void FpasMode(int MpdNum);
void CheckListMode(int MpdNum);
void FuelMode(int MpdNum);
void EasyCamMode(int MpdNum);
void EasySensorMode(int MpdNum);
void SitAwareMode(int MpdNum);
void EdfMode(int MpdNum);
void NavFlirMode(int MpdNum);
int GetClosestNimitz(FPointDouble *Loc);
void AAToggleGuns();
void DropFuelTankForDisplay(int Station);

void AvionicsRangeUp();
void AvionicsRangeDown();

void AGChangeProgram();


void ToggleHarmOverride();
void InitHarm();