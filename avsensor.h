#define LERP(from,to,frac)  (from + ( frac * (to - from) ) )

#define UP   0
#define DOWN 1

#define LTOR	0
#define RTOL	1


//*****************************************************************************************************************************************
// TIMER CLASS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

class Timer
{
    private:
	  int IsValid;
	  int StartTime;
	  int Duration;
	public:
	  Timer() {IsValid = FALSE;};
		Timer &operator = (const Timer &Source) { memcpy((void *)this,(void *)&Source,sizeof(Timer)); return *this;	};
		void  Set(float d,int s) {StartTime = s; Duration = (int)(d*1000); IsValid = TRUE;};
	  int   TimeUp(int CurTime)  const {return ( (IsValid) ? (CurTime - StartTime > Duration) : FALSE);};
	  void  Disable(void) {IsValid  = FALSE;};
	  int   IsActive(void) {return(IsValid);};
	  float TimeElapsed(int CurTime) { return( float(CurTime - StartTime)/1000.0f );};
};

//*****************************************************************************************************************************************
// SITUATIONAL AWARENESS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AV_MAX_RUNWAYS     50
#define AV_MAX_PLANES      200
#define AV_MAX_AG_TARGETS  500
#define AV_MAX_SAMS        200
#define AV_MAX_RADAR_SITES 200
#define AV_MAX_MISSILES    100
#define AV_MAX_VEHICLES    200
#define AV_MAX_DETECT_MISSILE_LAUNCH 100

// all objects that can be designated as ground targets
#define AV_REGISTERED_OBJECT 0
#define AV_PLANE             1
#define AV_SAM               2
#define AV_RADAR_SITE        3
#define AV_VEHICLE           4
#define AV_MISSILE           5
#define AV_RUNWAY            6

#define AV_PRIMARY           0
#define AV_SECONDARY         1
#define AV_NORMAL            2

// for object camera
#define OBJ_PROJ_DIST 100.0

// mission help ag target select filter
#define AV_USE_VEHICLES         1
#define AV_USE_SHIPS            2
#define AV_USE_PLANES           4
#define AV_USE_PRIME_GROUND     8
#define AV_USE_SECONDARY_GROUND 16
#define AV_USE_GROUND_TARGS     32

// arrows
#define AV_TARGET_DIR_ARROW   0
#define AV_WAYPOINT_DIR_ARROW 1

typedef struct
{
  void  *Obj;
	int   Index;
	int   Type;
	int   Category;
  float Dist;
  float RelAngle;  // +-180
  int   HasLock;
  int   Ping;
	int   ScrX;
	int   ScrY;
	int   InView;
} AvObjListEntryType;

typedef struct
{
  int NumAGTargets;
  AvObjListEntryType List[AV_MAX_AG_TARGETS];
} AvAGTargetsListType;

typedef struct
{
  int NumAATargets;
  AvObjListEntryType List[AV_MAX_AG_TARGETS];
} AvAATargetsListType;

typedef struct
{
  int NumSams;
  AvObjListEntryType List[AV_MAX_SAMS];
} AvSamsListType;

typedef struct
{
  int NumRadarSites;
  AvObjListEntryType List[AV_MAX_RADAR_SITES];
} AvRadarSitesListType;

typedef struct
{
  int NumVehicles;
  AvObjListEntryType List[AV_MAX_VEHICLES];
} AvVehiclesListType;

typedef struct
{
  int NumMissiles;
  AvObjListEntryType List[AV_MAX_MISSILES];
} AvMissilesListType;

typedef struct
{
  int NumPlanes;
  AvObjListEntryType List[AV_MAX_PLANES];
} AvPlaneListType;

typedef struct
{
  int NumRunways;
  AvObjListEntryType List[AV_MAX_RUNWAYS];
} AvRunwayListType;

//*****************************************************************************************************************************************
// AIR TO AIR DEFS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int UseAARadarForTargets;
	AvObjListEntryType PrimeTarget;
} AAType;

//*****************************************************************************************************************************************
// AIR TO GROUND DEFS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int  UseAGRadarForTargets;
	AvObjListEntryType *PrimeTarget;
} AGType;

//*****************************************************************************************************************************************
// WEAPONS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define AV_MAX_WEAPONS 15

typedef struct
{
	int Category;
	int Selected;
	int Station;
	int WeapPage;
	ObjectHandler *Model;
	WeaponLoadoutInfoType  *W;
} AvWeapEntryType;

typedef struct
{
	int BombMethod;
	int BombInterval;
	int NumRplBmbs;
	int CbuDetonateAltIndex;
	WeaponReleaseSimType Drop;
	WeaponReleaseSimType Rocket;
} BombingType;

typedef struct
{
	AvWeapEntryType *Left;
	AvWeapEntryType *FrontLeft;
	AvWeapEntryType *Center;
	AvWeapEntryType *FrontRight;
	AvWeapEntryType *Right;
} BombStationsType;


#define AV_FUEL_CENTER_HAS_TANK 1
#define AV_FUEL_LI_HAS_TANK     2
#define AV_FUEL_LO_HAS_TANK     4
#define AV_FUEL_RI_HAS_TANK     8
#define AV_FUEL_RO_HAS_TANK     16

#define AV_FUEL_EX_C      0
#define AV_FUEL_EX_LO     1
#define AV_FUEL_EX_RO     2
#define AV_FUEL_EX_LI     3
#define AV_FUEL_EX_RI     4
#define AV_FUEL_IN_LEFT   5
#define AV_FUEL_IN_RIGHT  6
#define AV_FUEL_IN_TANK4  7
#define AV_FUEL_IN_TANK1  8
#define AV_FUEL_IN_L_FEED 9
#define AV_FUEL_IN_R_FEED 10

typedef struct
{
	int Flags;
	float TanksMax[11];
	float Tanks[11];
	float TotalWing;
	float TotalCenter;
	float TotalInternal;
	float LastInternal;
	float LastWing;
	float LastCenter;
	float BingoVal;
	int   DumpFuel;
} AvFuelType;

typedef struct
{
	float CmdAirspeed;
	float CmdAlt;
	float CmdRod;
	float HorzAdjust;
	float VertAdjust;
	int   At10Sec;
	float Range;
	char  Message1[20];
	char  TiltMessage[20];
	char  TenSecMessage[20];
	char  StatusMessage[20];
	float PixelsPerMile;
} AclType;

typedef struct
{
	int NumAAWeapons;
	int NumAASeqWeapons;
	int NumAGWeapons;
	int UseAASeq;
	int CurrWeapMode;
  int PrevWeapMode;
	int WeaponsLoaded;
	int HasNavFlir;
	int HasTargetIR;
	int HasDataLink;
	AvWeapEntryType AAList[AV_MAX_WEAPONS];
	AvWeapEntryType AASeqList[AV_MAX_WEAPONS];
	AvWeapEntryType AGList[AV_MAX_WEAPONS];
	AvWeapEntryType *CurAAWeap;
	AvWeapEntryType *CurAASeqWeap;
	AvWeapEntryType *CurAGWeap;
	BombingType     Bomb;
	BombStationsType BombStations;
} AvWeaponsListType;

typedef struct
{
	WeaponParams *W;
	int Reported;
} AvMissileLaunchType;

typedef struct
{
	int NumMissileLaunch;
	AvMissileLaunchType Missiles[AV_MAX_DETECT_MISSILE_LAUNCH];
} AvMissileLaunchDetectType;

typedef struct
{
	int Draw;
	int ScrX;
	int ScrY;
	int ClipSide;
} HelpArrowType;

typedef struct
{
	int CurAAWeapId;
	int PrevAAWeapId;
	int CurAGWeapId;
	int PrevAGWeapId;
	AvMissileLaunchDetectType MissileLaunch;
	int AGTargSelectFilter;
	MBWayPoints *PrevWaypoint;
	FPointDouble StartPos;
	class Timer  WaypointOOBTimer;
	HelpArrowType TargetArrow;
	HelpArrowType WaypointArrow;
} MissionHelpType;

typedef struct
{
	float Range;
} SensorType;

typedef struct
{
	float Range;
} AvCameraType;

typedef struct
{
	WeaponParams *CurWeap;
	int          PrevView;
  int          Status;
} AvDestructoType;

//*****************************************************************************************************************************************
// AA_RADAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
  float AzimLimit;
  float ElevLimit;
  float CosAzimLimit;
  float CosElevLimit;
  float ElevStep;
  float AOTDamping;
  int   Bars;
  int   BarCount;
  int   Range;
  int   SlewUpDown;
  int   SlewLeftRight;
  float LeftAzimEdge;
	float RightAzimEdge;
  float UpElevEdge;
	float DownElevEdge;
  float ScanRate;
  unsigned int LstTicks;
  unsigned int ThisTicks;
  int   FirstTime;
  unsigned int DiffCount;
  float AntAzimuth;
  float AntElevation;
  int   ScanDirection;
  int   Prf;
  int   PrfFrameStart;
  int   TextChangeCountDown;
  int   CurMode;
  int   PrevMode;
  int   SearchPrf;
  int   FrameStoreCount;
  Timer AcqTimer;
	int   IffMode;
	Timer IffTimer;
	int   RaidOn;
	Timer RsetTimer;
	float AgeFrequency; // in sec
	int   SilentOn;
	int   NCTROn;
	int   AcqMode;
	int   Declutter;
	int   MSI;
	int   AutoSlewOn;
	float VsLimit;
	int   AutoOn;
	int   SttJamming;
} RadarInfoType;

//*****************************************************************************************************************************************
// AG RADAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
  unsigned int LstTicks;
  unsigned int ThisTicks;
  int   FirstTime;
  unsigned int DiffCount;
  float ScanRate;
  int   ScanDirection;
  float AntAzimuth;
	FPointDouble BeamEndPointWorld;
	FPoint       BeamEndPointPixel;

	int  TargetType;
	void *Target;
	int   TargetMoving;

	int CaptBarsOn;
	int InterleaveOn;
	int CurMode;
	int PrevMode;
	int DeclutterOn;
	int RSETOn;
	int FreezeOn;
	int SilenceOn;
	int MapMode;
	int ZoomMode;
	float Range;
	float Azim;
	float BoxRot;
	float BoxTranslate;
	FPointDouble WorldBoxCenter;
	Timer RsetTimer;
	int   VideoGain;
} AGRadarType;

//*****************************************************************************************************************************************
// WEAP CAM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
  int AquireMode;
  int FOVToUse;
  int TrackOn;
  int TargetSelected;
	int TargetObtained;
	int TrackPosObtained;
  float SlewLeftRight;
  float SlewUpDown;
  int TargetInKeyhole;
	int DrawCross;
	int AutoSet;
	Timer CrossTimer;
	FPointDouble AGDesignate;
  int Magnify;
  int TargetType;
  int MarkerLight;
  int MarkersOn;
  int WithinSlewLimits;
  int DotNum;
  FPointDouble TrackPos;
	FPointDouble TargetLoc;
  void *Target;
  int WeapFired;
  int LaunchTimer;
  FPointDouble CamPos;
  WeaponParams *Weap;
  FMatrix Matrix;
  FMatrix RelMatrix;

	int ForceCorrelateOn;
	int Polarity;
	Timer CHTimer; // CH = CrossHairs
	float CHRadius; // radius in pixels from center of MPD, represents the destination
	float CurCHPos;

	// do not change the ordering of these 8 floats!! (see avmodes.cpp)
	float CHRadiusLeft;
	float CHRadiusRight;
	float CHRadiusUp;
	float CHRadiusDown;
	float CurCHPosLeft;
	float CurCHPosRight;
	float CurCHPosUp;
	float CurCHPosDown;

	RegisteredObject *CHMavTarget;
	Timer CHReAdjustTimer;

	// FOR HARM
	int Mode;
	int SurfaceFilter;
	int LimitOn;
	void *CurHarmObj;
	int  CurHarmObjType;
	int  SelfProtectOn;

} WeapCamType;

//*****************************************************************************************************************************************
// FLIR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
 int Mode;
 int TrackOn;
 int FOVToUse;
 int Magnify;
 float SlewLeftRight;
 float SlewUpDown;
 int Brightness;
 int Contrast;
 int Polarity;
 int TrackLS;
 int ReticleOn;
 int DeclutterOn;
 int LaserOption;
 int LaserState;
 int MaskOn;
 int SnowplowOn;
 int MtgtOn;
 FPointDouble AGDesignate;
 FPointDouble TrackPos;
 int TargetType;
 void *Target;
 FPointDouble TargetLoc;
 int TargetObtained;
 int LastMode;
 PlaneParams *AADesignate;
 Timer TimpactTimer;
 int   TimeToImpact;
} FlirType;

typedef struct
{
 int Brightness;
 int Contrast;
 int DeclutterOn;
 int Polarity;
} NavFlirType;

//*****************************************************************************************************************************************
// TEWS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define CM_CMDDS_STOW     0
#define CM_CMDDS_STBY     1
#define CM_CMDDS_DEPLOY   2
#define CM_CMDDS_JAM      3
#define CM_CMDDS_EMCON    4
#define CM_CMDDS_NO_DECOY 5

#define CM_ICS_STBY     0
#define CM_ICS_JAM      1
#define CM_ICS_EMCON    2

#define CM_IDECM_MAN    0
#define CM_IDECM_SEMI   1
#define CM_IDECM_AUTO   2

typedef struct
{
	int CurCMDDS;
	int CMDDSStat[3];
	Timer CMDDSTimer;
	int ICSState;
	int IDECMStat;
	int CmddJamming;

	Timer AutoTimer;
	int   ProgRepeat;
	int   ProgInitiated;
} TewsCMType;

typedef struct
{
	int Filter;
	int FriendlyOn;
	int EarlyWarningOn;
	int LimitOn;
  int DeclutterOn;
	int EditOn;
  int CurMode;
  int Range;
  int IcsStatus;

	int ReadyForProg;
	int CurProg;
	TewsCMType CM;
} TewsType;

//*****************************************************************************************************************************************
// HARM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int Mode;
	int SurfaceFilter;
	int LimitOn;
	void *CurHarmObj;
	int  CurHarmObjType;
	int  SelfProtectOn;
} HarmType;

//*****************************************************************************************************************************************
// UFC  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define UFC_CNI_MODE   0
#define UFC_RALT_MODE  1
#define UFC_AP_MODE    3
#define UFC_TACAN_MODE 4
#define UFC_MDI_MODE   5

typedef struct
{
	char Keypad[50];
	int  Mode;
	int  MdiMode;
	int  RaltVal;

	int TacanStart;
	int TacanStop;
	int CurTacan;
} UfcType;

//*****************************************************************************************************************************************
// HSI  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define HSI_TCN_MODE 0
#define HSI_TGT_MODE 1
#define HSI_ACL_MODE 2
#define HSI_WPT_MODE 3
#define HSI_GPS_MODE 4

#define HSI_TUP  0
#define HSI_DCTR 1

#define LOCAL_TIME 0
#define ZULU_TIME  1

#define HSI_TACAN      0
#define HSI_GND_TARGET 1
#define HSI_BULLSEYE   2
#define HSI_LS         3
#define HSI_FLIR       4
#define HSI_RADAR      5
#define HSI_WPT        6


typedef struct
{
	int ShowMap;
	int SteeringMode;
	int DisplayMode;
	int SensorsOn;
	int TimeOn;
	int AutoOn;
	int SequenceOn;
	int DataOn;
	int CselOn;
  float Range;
	float HSelect;
	float CSelect;
	int   CurGps;

	int DataTime;

	FPointDouble Origin;
	float  RadiusInPixels;
	float  PixelsPerMile;
} HsiType;

//*****************************************************************************************************************************************
// SIT AWARE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int DatalinkOn;
	int FlirOn;
	int RadarOn;
	float Range;
	float PixelsPerMile;
} SAType;

//*****************************************************************************************************************************************
// BIT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	int CurBitPage;
	int CurDisplayPage;
} BITType;

//*****************************************************************************************************************************************
// GLOBAL AVIONICS STRUCT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
	AvAGTargetsListType  MissionAGTargets;
	AvAATargetsListType  MissionAATargets;
	AvAGTargetsListType  AGTargets;
  AvSamsListType       Sams;
  AvRadarSitesListType RadarSites;
  AvMissilesListType   Missiles;
  AvPlaneListType      Planes;
  AvVehiclesListType   Vehicles;
  AvRunwayListType     Runways;
  AvWeaponsListType    Weapons;
	AvFuelType           Fuel;
  AAType               AA;
  AGType               AG;
	MissionHelpType      MH;
	SensorType           Sensor;
	AvCameraType         Camera;
	AvDestructoType      Destructo;

	RadarInfoType        AARadar;
	AGRadarType          AGRadar;
	WeapCamType          Mav;
	WeapCamType          Gbu;
	FlirType             Flir;
	NavFlirType			 NavFlir;
	TewsType             Tews;
	WeapCamType          Harm;
	UfcType              Ufc;
	HsiType              Hsi;
	SAType               SA;
	BITType              Bit;
	AclType              Acl;
} AvionicsType;

//*****************************************************************************************************************************************
// PROTOTYPES  -- *Header -- (search "*Header" to step through each section of file)
//*****************************************************************************************************************************************

double ComputeHeadingToPoint(FPoint WorldPosition, double Heading, FPoint pointpos, int isrelative);
double ComputeHeadingToPointDouble(FPointDouble WorldPosition, double Heading, FPointDouble pointpos, int isrelative);
float  ComputePitchToPoint(PlaneParams *planepnt, FPoint pointpos);
float  ComputePitchFromPointToPoint(FPointDouble FromPos, FPoint pointpos);
double ComputePitchFromPointToPointDouble(FPointDouble FromPos, FPointDouble pointpos);
void   GetRelativeHPFromLoc(PlaneParams *planepnt,FPointDouble Loc, FPointDouble worldposition, float *heading, float *pitch);
void   ProjectPoint(float X, float Z, float Degrees, float Magnitude, float *NewX, float *NewZ);
float  Dist2D(FPointDouble *P1, FPointDouble *P2);
void   IntTo3DigitFont(int Val, char *Str);
void   IntTo5DigitFont(int Val, char *Str);
void   TruncatePlaneName(char *StrTo, char *StrFrom);
void   InitAvionicsSensor(void);
void   CleanupAvionicsSensor(void);
void   SetSensorAwareness(void);
void   DrawDoAllSensor(int CenterX, int CenterY);
void   DrawObjectCamera(int CenterX, int CenterY);
void   SelectNextTarget(void);
void   AutoGrabAATarget(void);
void   AutoGrabAGTarget(void);
int    IsObjInSamList(BasicInstance *Obj);
int    IsObjInRadarSiteList(BasicInstance *Obj);
int    IsObjInMissionObjectList(int Type,void *Obj,AvObjListEntryType **Entry);
void   AvSensorClick(int XOff, int YOff);
int GetObjectLocation(AvObjListEntryType *Obj,FPointDouble *Location);

void SelectFirstAGTarget();
void SelectPrevAGTarget();
void SelectNextAGTarget();
int GetAGTargetLocation(FPointDouble *Location);
int IsPlanePrimaryOrSecondary(PlaneParams *P);