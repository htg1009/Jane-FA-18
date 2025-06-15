// weapons.h

#ifndef __WEAPONS_H
#define __WEAPONS_H

#define FUEL_TANK_WEIGHT   3900

#define AA_GUN_FNL	0
#define AA_GUN_GDS	1
#define AA_GUN_FNL_GDS	2

#define SRM_MODE	 0
#define MRM_MODE	 1
#define GUN_MODE	 2
#define RJCT_MODE	 3
#define	WEAP_OFF	 4

#define SRM_STAT	 0
#define MRM_STAT	 1
#define STBY_STAT	 2
#define RDY_STAT	 3
#define HUNG_STAT	 4
#define NONE_STAT	 5
#define AG_STAT		 6

#define DUMB_AG		 0
#define GUIDED_AG	 1
#define SHORT_RANGE	 2
#define MED_RANGE	 3
#define AG_MISSILE	 5
#define FLARE		 9
#define CHAFF		10
#define CLUSTER		11
#define FUEL_TANK	12
#define DATA_LINK	16

// for camera views
#define MAVERIC_TYPE 0
#define GBU15_TYPE   1
#define FLIR_TYPE    2

#define LOW  0
#define HIGH 1

// bomb method defines
#define SNGL_BMB     0
#define SNGL_RPL_BMB 1
#define MLT_RPL_BMB  2

#define CDIP_AG      0
#define AUTO_AG      1
#define AUTO_LOFT_AG 2
#define GUNS_AG		   3
#define MAN_AG		   4

//  Surface weapon attack profiles
#define LOW_LOW		0
#define LOW_POP		1
#define MED_ATTACK	2
#define HIGH_ATTACK	3

//******************************************************************************************
// These are for the WeaponParams.Flags field

#define WEAPON_INUSE	0x0000000F
#define BULLET_INUSE	0x00000001
#define BOMB_INUSE		0x00000002
#define MISSILE_INUSE	0x00000004
#define CHECK_FOR_BURST 0x00000010
#define ALREADY_BURST 	0x00000020
#define RED_TRACER 		0x00000040
#define FUEL_AIR_STAGE2 0x00000040
#define DURANDEL_STAGE2 0x00000080

#define CAMERA_ACTIVE	0x00000100
#define COLLISION_CHECK 0x00000200

#define WEP_SUN_TRACK	0x00000400
#define WEP_JETTISONED  0x00000800

#define WEP_SPREAD_DAMAGE 0x00001000
#define WEP_FIRST_CHECK 0x00002000
#define WEP_TURN_LEFT	0x00004000	//  For rogue AIM-120s
#define WEP_TURN_RIGHT	0x00008000	//  For rogue AIM-120s

#define BOOST_PHASE		0x00010000
#define GLIDE_PHASE		0x00020000
#define FINAL_PHASE		0x00040000
#define GUIDED_WEAPON	0x00080000
#define LOSING_LOCK		0x00100000
#define LOST_LOCK		0x00200000
#define ACTIVE_SEARCH	0x00400000
#define ACTIVE_SEEKER	0x00800000
#define STT_MISSILE		0x01000000
#define MISSILE_SPOTTED	0x02000000
#define WEP_PADLOCKED   0x04000000
#define WEP_NO_CHECK	0x08000000
	//  Missiles
#define WEP_TRACK_LI	0x10000000
#define WEP_TRACK_RI	0x20000000
#define WEP_TRACK_LO	0x40000000
#define WEP_TRACK_RO	0x80000000
	// Guided Bombs
#define WEP_PITCH_UP	0x10000000
#define WEP_DIRECT_LAUNCH 0x20000000
	//  EJECTION SEAT
#define WEP_DAMAGED_GUY_IN_CHAIR	0x20000000
#define WEP_GUY_IN_CHAIR	0x80000000
	//  PARACHUTE
#define WEP_DAMAGED_CHUTE	0x20000000
#define WEP_CHUTE_HALF_OUT	0x40000000
#define WEP_CHUTE_OUT	0x80000000
	//  BULLETS
#define WEP_BULLET_GROUND_CHECK	0x80000000

//******************************************************************************************
// These are for the WeaponParams.Flags2 field
#define ANTI_RAD_SHIP_MASK	0x1F
#define MISSILE_DROP_PHASE	0x20
#define MISSILE_VERTICAL_PHASE	0x40
#define MISSILE_LOFT_AIM_7	0x80
#define BOMB_FORCE_FREEFALL	0x100
#define MISSILE_WARNED		0x200
#define MISSILE_HOME_ON_JAM	0x400
#define MISSILE_GPS_ONLY	0x800

//******************************************************************************************

#define TARGET_PLANE  1
#define TARGET_GROUND 2
#define TARGET_DECOY  3
#define TARGET_VEHICLE 4

#define NONE		 0
#define BULLET		 1
#define BOMB		 2
#define MISSILE		 4
#define EJECTION_SEAT	 8

#define ACTIVE		 0
#define KILLED		 1

typedef struct _WeaponParams{

    // general weapon stuff
	DWORD 		 SortType;
//	ObjectSortHeader *SortHeader;

	PlaneParams  *P;					// pointer to the plane that weapon is associated with
	WeaponType	 *Type;

	int			 iNetID;
	void		 *Launcher;
	int			 LauncherType;
	int			 LaunchStation;			// For vehicles/ships which have more that one weapon.
	int			 Kind;					// BULLET / BOMB / MISSILE
	int			 Flags;					// state change flags specific to Type
	int			 Flags2;
	int			 LifeTime;				// sometimes life in ticks, sometimes total WU traveled
	double		 Altitude;
	double       GroundHeight;			//  Holds the altitude where the bomb should hit the "ground" in World Units;
	DWORD		 CollisionCheckTimer;	//  time between consecutive ground collsion detections

	double		 InitialVelocity;       // initial velocity when released from plane
	FPointDouble Pos;					// current position in the world
	FPointDouble LastPos;				// previous position in the world
	FPointDouble Vel;					// velocity vector
	FPointDouble AccelOrientation;		// Normalized vector showing direction for ejection acceleration.

	FMatrix		 Attitude;				// Rotation matrix
	ANGLE		 Heading;
	ANGLE		 Pitch;
	ANGLE		 Roll;

	// pointers to functions
  	void (*WeaponMove)(_WeaponParams *W);

	int			 iTargetType;			//  Plane, Ground, Decoy
	void		 *pTarget;				//  Pointer to either PlaneParams (Air) or BasicInstance (Ground)
//	int			 iTargetIndex;			//  Index into appropriate list (using iTargetType)
	FPoint		 fpTempWay;				//  Fly to point for Active Radar Missiles whose planes have lost lock with target.
	long		 lBurnTimer;			//  Timer for how long to have Flame on missile and missile acceleration.
	long		 lTimer;				//  Generic Timer for things like loosing lock and checking for decoys.
	long		 lTargetDistFt;			//  Distance to Target in feet;
	void		 *pDecoyed;				//  Pointer to Counter Measure we are fooled by.
	float		 fGravity;				//  This will usually be 32.0f ft/(sec*sec) but can change for gravity bombs.
	float		 fMinGravity;			//  Max gravity value a guide bomb can have.

	void		 *Smoke;

}WeaponParams;


typedef struct{

	float	Interval;
	float	TotalReleaseNumber;
	int		Activate;
	int		StartIndex;
	int		OneTimeDropNum;

}WeaponReleaseSimType;

#define BUDDY_LAS_RANGE_NM	15.0f

#define MAX_CHASING_MISSLES 10

typedef struct
{
  int TimeFired;
  int EstimatedTime;
  PlaneParams *Target;
  int ActivateTime;
  int WeaponPage;
} ChasingMisslesType;

// This defines how fuzzy the edges of the radar visibility are
#define RADAR_SLOP_VALUE		0.05
#define RADAR_MAX_OPTICAL_RANGE	20 KLICKS

/* -----------------10/6/98 5:42PM---------------------------------------------------------------------
/* lights for various flame type doohickies
/* ----------------------------------------------------------------------------------------------------*/

extern FPoint MissileFlameLightColor;
extern FPoint CounterFlareLightColor;
extern FPoint BurningPlaneLightColor;

#define MISSILE_FLAME_LIGHT		TRUE,&MissileFlameLightColor, 75  ,    15 FEET
#define COUNTER_FLARE_LIGHT		TRUE,&CounterFlareLightColor, 75  ,    20 FEET
#define EJECTION_FLARE_LIGHT	TRUE,&MissileFlameLightColor,100 FEET ,10 FEET
#define PLANE_BURNING			TRUE,&BurningPlaneLightColor,500 FEET ,50 FEET


// Funnel functions
void InstantiateFunnel(PlaneParams *P);
void InitFunnel( void );
void MoveFunnel( void );
void DisplayFunnel(PlaneParams *P);

// Bullets functions
void InstantiateBullet(PlaneParams *P,BOOL tracer);
void InstantiateTurretBullet( PlaneParams *P, BOOL tracer, WeaponType *wtype, ANGLE heading, ANGLE pitch, FPoint *offset = NULL, int numbershot = 1);
void MoveBullets(WeaponParams *W);
void DeleteBullet(WeaponParams *W);
void InstantiateAAABullet( BasicInstance *P,ANGLE pitch, ANGLE heading,long id);
//  void InstantiateAAABullet( MovingVehicleParams *P,ANGLE pitch, ANGLE heading,long id);  //  Prototype moved to vehicles.h

// Bomb functions
void InstantiateBomb(PlaneParams *P, int Station,double burst_height = (1000 * (1.0f/5.7435)), void *GroundTarget = NULL, int groundtype = 3, double GX = -1.0f, double GY = -1.0f, double GZ = -1.0f, int netid = -1);  //One thousand Feet
void MoveBombs(WeaponParams *W);
void DeleteBomb(WeaponParams *W);
void MoveGuidedBombs(WeaponParams *W);
void GuideBomb(WeaponParams *W, FPointDouble AGDesignate);
void WSOControlWeapon(WeaponParams *W, DBWeaponType *pDBWeapon);
double GetDesiredGravity(float Time, double VertVel, double weaponheight);
void SetGroundTarget(WeaponParams *W, void *GroundTarget, FPoint targetloc, int groundtype = 3);

// Missile functions
void InstantiateMissile(PlaneParams *P, int Station, void *GroundLauncher = NULL, int sitetype = 3, PlaneParams *target = NULL, int netid = -1, int radardatnum = 0);
void LockHeaterOnEngine(PlaneParams *planepnt, WeaponParams *W);
void MoveMissile(WeaponParams *W);
int  WCheckDesignateTarget(WeaponParams *W);
void WGetActivationPoint(WeaponParams *W);
void WGetLaunchInterceptPoint(PlaneParams *launcher, int weaponindex, PlaneParams *planepnt, double avgvelfps, FPointDouble *InterceptPoint);
float WGetLaunchAvgSpeed(int weaponindex, float distft, float startfps, float avgaltft, float *time);
float WGetTimeToAAImpact(PlaneParams *P, PlaneParams *target, float *timeactive = NULL);
void WSeekTarget(WeaponParams *W);
void DeleteMissile(WeaponParams *W, int ignorenet = 0);
int GetEngineArrayOffset(PlaneParams *planepnt, long engineid, int idtype = 0);

// Counter Measure functions
void InstantiateCounterMeasure(PlaneParams *P, int Station, int cmtype = -1, int netid = -1);
void DeleteCounterMeasure(WeaponParams *W);
void MoveCounterMeasure(WeaponParams *W);
void CMDropChaff(PlaneParams *planepnt);
void CMDropFlare(PlaneParams *planepnt);
void CMCheckIsStillDecoyed(WeaponParams *W);
void CMDecoyCheck(WeaponParams *decoy);
void CMDecoyClear(WeaponParams *decoy);

// Weapons functions
void InitWeapons(void);
void MoveWeapons(void);
void DetectWeaponFire(PlaneParams *P);
void Do50HzPlaneBasedWeaponStuff(PlaneParams *P);
void DisplayWeapons(CameraInstance *camera);
WeaponParams *GetWeaponSlot(void);

//  Air to Ground Missile functions
void InstantiateAGMissile(PlaneParams *P, int Station, void *GroundTarget, FPoint targetloc, int groundtype = 3, int netid = -1, int flightprofile = 0);
void MoveAGMissile(WeaponParams *W);
float WGetAGLaunchAvgSpeed(int weaponindex, float distft, float startfps, float avgaltft, float *time);
void InstantiateSSMissile(void *launcher, int launchertype, void *GroundTarget, int targettype, FPoint targetloc, int netid = -1, int radardatnum = 0, int flightprofile = 0);
void LaunchSSMissile(WeaponParams *W);

// Collision detection functions
BOOL WeaponIntersectsGround( WeaponParams *W );
BOOL CheckIntersection(FPointDouble &Line1P1,FPointDouble &Line1P2,FPointDouble &Line2P1,FPointDouble &Line2P2,float radius);
PlaneParams *WeaponHitsPlane( WeaponParams *W , int *bullethit = NULL);
void AAAHitsPlane( AAAStreamVertex *W );
void CrashPlane(PlaneParams *P,int flight_status,int failed_system, WeaponParams *W = NULL);

// Ground based radar functions
float GetPlaneGroundRadarVisibility(BasicInstance *site,PlaneParams *plane,BOOL plane_to_radar);
//  float GetPlaneVehicleRadarVisibility( MovingVehicleParams *site, PlaneParams *P, BOOL plane_to_radar );

// HardPoint functions
int GetHardPoint( PlaneParams *P, int station, int count );
FPointDouble GetHardPointPos( PlaneParams *P, int station );

//  Missile Range functions
float CalcAltEffectOnMissile(double weaponaltwu);
float GetMissileRMaxNM(DBWeaponType *pweapon_type, double weaponaltwu);
float GetMissileROptNM(DBWeaponType *pweapon_type, double weaponaltwu);
float GetMissileRTR_NM(PlaneParams *targetplane, DBWeaponType *pweapon_type, double weaponaltwu);

//  Bomb impact functions
float GetImpactTime(PlaneParams *P, double orgVertVel, FPoint *ImpactPoint);
float GetImpactTime(WeaponParams *W, double orgVertVel, FPoint *ImpactPoint);

void AddMissleToChasingMissleList(PlaneParams *Target);
void RemoveMissleFromChasingMissleList(int Index);

void WGetGBUMavMinMax(PlaneParams *planepnt, DBWeaponType *pDBWeapon, float *minwu, float *maxwu, float *optwu = NULL);

//  Ejection Functions
void InstantiateEjectionSeat(PlaneParams *P, int numejects = 1, int sar_on_way = 0, WeaponParams *WSO = NULL, int netid = -1);
void InstantiateBomberEjections(PlaneParams *P, int numejects = 1, int sar_on_way = 0, WeaponParams *WSO = NULL, int netid = -1, WeaponType	 *Type = NULL);
void InstantiateChute(WeaponParams *Seat);
BOOL DeleteEjectionSeat( WeaponParams *W);
void DeleteChute( WeaponParams *W);
void MoveEjectionSeat( WeaponParams *W);
void MoveJumpers( WeaponParams *W );
void MoveChute( WeaponParams *W);
void CheckNotInPlayerPlane();
void ChuteStayOnGround(WeaponParams *W);
void BeginEjectionSequence(int planenum, int targetnum = -1);
void AIUpdateChuteSAR(WeaponParams *W);
void ChuteMoveToHelo(WeaponParams *W);

int PlaneHasAAWeapons(PlaneParams *P);
int PlaneHasAGWeapons(PlaneParams *P);

//  Network functions
void UpdateNetWeaponTarget(int weaponid, int targetid);
void SendNetWeaponTargetChange(int weaponid, int targetid, WeaponParams *W = NULL);

float HeatSeekerSunFactor(ANGLE heading, ANGLE pitch, FPointDouble seekerpos, float maxangle, float *sunoffyaw = NULL, float *sunoffpitch = NULL);
void SteerWeaponToSun(WeaponParams *W);
FPoint GetBallisticWUGivenDistance(PlaneParams *P, double WUDist);
void KillGSAM(WeaponParams *W, int dodamage = 1);
int WCheckWeaponNowPlane(void *launcher, int launchertype, void *GroundTarget, int targettype, FPoint targetloc, int netid, int radardatnum, int flightprofile = 0);
int WGetWeaponToPlaneID(DBWeaponType *weapon);
void WInitializeWeaponPlane(int planenum, int planeid);
int WIsWeaponPlane(PlaneParams *planepnt);
void InstantiateCannon(void *launcher, int launchertype, void *GroundTarget, int targettype, FPoint targetloc, int netid, int radardatnum);
void MoveCannonShell(WeaponParams *W);
void WCheckAntiRadarLock(WeaponParams *W);
void WAntiRadarSearch(WeaponParams *W);
void WKillRadar(WeaponParams *W);
void MoveTorpedo(WeaponParams *W);
int WFoundOtherLaser(WeaponParams *W, int seeker, FPointDouble *rposition);
float WCheckIRAspect(DBWeaponType *pDBWeapon, WeaponParams *W, PlaneParams *target);
float WCheckIRAspect(DBWeaponType *pDBWeapon, PlaneParams *planepnt, PlaneParams *target);
void WGetRelativeHP(WeaponParams *W, FPointDouble worldposition, float *heading, float *pitch);
int WCheckForHARMNotice(BasicInstance *target, int infoprovider, WeaponParams *W);
void WJettisonRocketPod(PlaneParams *planepnt, int Station);
void DisplayFunnel(int ShowFunnel,PlaneParams *P , int *FunnelX, int *FunnelY);
int WIsRetardedWeapon(int weapid);
void WInstantiatePhantomWeapon(PlaneParams *P, void *launcher, int launchertype, int Station, WeaponType *Type, void *GroundTarget, int targettype);
void WMovePhantomWeapon(WeaponParams *W);

#endif