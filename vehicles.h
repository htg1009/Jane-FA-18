#ifndef __VEHICLE__
#define __VEHICLE__
// vehicles.h

#define AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G3 0.0333     // 3 was 1 degree on each side = 3/90
#define AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G2 0.0667     // 6 was 3 degree on each side = 6/90
#define AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G1 0.1        // 9 was 6 degree on each side = 9/90

#define AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G3 0.044     // 4 was 2 degrees on each side = 4/90
#define AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G2 0.0778    // 7 was 5 degrees on each side = 7/90
#define AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G1 0.1111    // 10 degrees on each side = 10/90



#define SUBMERGE_DEPTH		100.0f

//  #define V_USE_INST_HEIGHT 1

#define VL_ACTIVE			0x00000001
#define VL_INVISIBLE		0x00000002
#define VL_MOVING			0x00000004
#define VL_WAITING			0x00000008
#define VL_DESTROYED		0x00000010
#define VL_REVERSE			0x00000020
#define VL_DONE_MOVING		0x00000040
#define VL_SKIP_NEXT_WAIT	0x00000080
#define VL_FIRE_WEAPONS		0x00000100
#define VL_CHECK_FLAGS		0x00000200
#define VL_WAIT_DESTRUCTION 0x00000400
#define VL_AVOID_JAM		0x00000800
#define VL_SURFACE_ATTACK	0x00001000
#define VL_ON_BRIDGE_RAMP_1 0x00002000
#define VL_ON_BRIDGE_RAMP_2 0x00004000
#define VL_ON_BRIDGE_SPAN 	0x00008000
#define VL_ON_BRIDGE		0x0000E000
#define VL_BRIDGE_TARE		0x00010000
#define VL_BRIDGE_INFO		0x0001E000
#define VL_IN_VISUAL_RANGE	0x00020000
#define VL_FALLING_LEFT		0x00040000
#define VL_FALLING_RIGHT	0x00080000
#define VL_FALLING			0x000C0000
#define VL_SINKING_POS		0x00100000
#define VL_SINKING_NEG		0x00200000
#define VL_SINKING			0x00300000
#define VL_SWEPT_UP			0x00400000
#define VL_GOAL_CHECKED		0x00800000
#define VL_CHECK_LEAD		0x01000000
#define VL_TURNING			0x02000000
#define VL_CLOSE_ENOUGH		0x04000000
#define VL_DO_NEXT_WAY		0x08000000

#define VEHICLE_SPACING		200.0f  /*  75.0f  */
#define ROAD_WIDTH			25.0f
#define LANE_WIDTH			12.5f
#define LANE_MIDDLE			6.25f

#define VT_COLLIDEABLE		1

#define MAX_VEHICLE_FX		10

#define MAX_CARRIER_PLANES	100
extern PlaneParams CarrierPlanes[];

#define MAX_VEHICLE_SMOKE_SLOTS 5


//  lAIFlags1 bit flags
#define PLANES_ON_DECK		0x00000001
#define	CAT_4_BLOCKED		0x00000002
#define SAR_CHECKED			0x00000004
#define V_JUST_TO_WAY		0x00000008
#define V_TEMP_STOP			0x00000010
#define V_NO_SHADOW			0x00000020
#define V_FLASH_WAVEOFF		0x00000040
#define V_FLASH_E_WAVEOFF	0x00000080
#define V_ON_FIRE			0x00000100

#define VS_DEFENSE_WEAPON	0
#define VS_DEFENSE_RADAR	1

typedef struct _WakeBoundPoint
{
	float NextZ;
	float CurrentX;
	float XSlope;
} WakeBoundPoint;

typedef struct _WakeBounds
{
	int 			NumPoints;
	float 			FirstZ;
	float			WidestX;
	WakeBoundPoint	Points[20];
} WakeBounds;

#define MAX_WAKE_POINTS 32
#define MAX_WAKE_POINTS_M1 (MAX_WAKE_POINTS-1)
#define MAX_WAKE_EXPANSION_TIME 3.0f

#define WP_IS_VALID		  1
#define WP_PAST_EXPANSION 2
#define WP_PAST_BOAT	  4

typedef struct _WakePoint
{
	DWORD   		Flags;
	DWORD  			FirstTick;
	FPointDouble 	CenterPoint;
	float  			OuterLeft;
	float  			InnerLeft;
	float  			InnerRight;
	float  			OuterRight;
	float  			ShipVelocity;
	float			ShipVelocityRight;
	FPoint 			RightVector;
} WakePoint;

typedef struct _Wake
{
	int		  FirstPoint;
	int		  FirstTick;
	WakePoint WakePoints[MAX_WAKE_POINTS];
	float	  Vs[MAX_WAKE_POINTS];
} Wake;

typedef struct _CarrierToPlaneData
{
	FMatrix CarrierToPlane;
	FMatrix PlaneToCarrier;
	FPoint	CarrierDeckNormalRelToPlane;
	FPoint  CarrierDeckPointRelToPlane;
} CarrierToPlaneData;

#define CW_WIRE_0		0
#define CW_WIRE_1		1
#define CW_WIRE_2		2
#define CW_WIRE_3		3
#define CW_WIRE			3

#define	CW_PLANE_CAUGHT	4
#define CW_RETURNING	8

typedef struct _CatchWireDef
{
	FPoint*		Wires[4];
	FPoint		Values[4];
	FPoint      WireNormal;
	float		WireDs[4];
	FPoint		LeftNormal;
	float		LeftNegD;
	FPoint		RightNormal;
	float		RightNegD;
} CatchWireDef;

typedef struct _WireData
{
	DWORD		Flags;
	DWORD		Tick;
	FPoint		LastPlace;
	PlaneParams *CaughtPlane;
} WireData;


#define MVT_CHECKED			0x80000000
#define MVT_NIMITZ_MODEL 	0x40000000

typedef struct _MovingVehicleType
{
	int				TypeNumber;
	int				iShipType;
	int				iSizeType;
	DWORD			Flags;
	FPoint			HighLR;
	FPoint			LowUL;
	WakeBounds  	WakeGen;
	CatchWireDef	CatchWires;
	ObjectHandler 	*Model;
	//ObjectHandler *SubParts[MAX_CONNECTED_PARTS];
} MovingVehicleType;

typedef struct _FauxRunway
{
	DWORD Flags;
	float Heading;

	FPointDouble ILSLandingPoint;
	FPointDouble ILSFarEndPoint;
} FauxRunway;

typedef struct _MovingVehicleParams
{
	ObjectSortHeader *SortHeader;
	MovingVehicleType *Type;
	int iType;			// Database ID - This is the id number in the database of the object (this
						// is not the index into the database structure!!!)
						// If (!iVehicleType) then use pShipDB when looking for database stuff
						// If (!iShipType) then use pVehicleDB

	int iVehicleType;	// Type of vehicle (0 if new ship type-use iShipType) - See dataform.h for list of types
	int iShipType;		// Type of ship (0 if vehicle,use iVehicleType) - See dataform.h for list of types

						// Yea, the above could have been done with one variable, but its easier this
						// way for old code.  No buts, its this way, deal with it.

						// Why are you so ANGRY?  Can't we all just get along?

	//	Special little note: to support the old moving vehicle types, a ship possibly could
	//  exist and the above condition would not be true - If this is the case, then
	//  the !type->iVehicleType will be VEHICLE_TYPE_SHIP.  This is only here to support
	//  old F-15 missions.  This does not have to be supported in F-18.

	FPointDouble	WorldPosition;		// X,Y,Z

	FMatrix		 Attitude;				// Rotation matrix
	ANGLE		 Heading;
	ANGLE		 Pitch;
	FPoint		 OrdDistanceFromCamera;
	float		 DistanceFromCamera;
	float		 SinkingMultiplier;
	DWORD		 FirstSinkingTick;

	struct _MovingVehicleParams *NextVehicleOnBridge;
	struct _MovingVehicleParams *NextVehicleToDraw;
	StructureInstance *Bridge;
	int			 iVListID;		//  ID into g_pMovingVehicleList array;
	int			 iVDBIndex;		//  Index into vehicle database;
	int			 iSide;			//  Tells which side this vehicle belongs;
	long		 Status;
	float		 fSpeedFPS;		//  How fast this vehicle is moving in feet per second.
	float		 fWorkingFPS;	//  This is going to be for some acceleration stuff.;
	RoutePoint	 *pStartWP;		//  First Waypoint for Vehicle
	int			 numwaypts;		//  Number of Waypoints remaining
	RoutePoint	 *pCurrWP;		//  CurrentWayPoint
	FPoint		 WayPosition;	//  This holds the World Position that the AI is moving towards.
	float		 fSinPitch;		//  Holds the Sin of the pitch from the next waypoint to the current.  This is for calculating altitude when there is no terrain loaded.
	long		 lAITimer1;		//  Generic timer used for waiting and other such things.
	long		 lLandCheckTimer; //  Timer so I don't keep checking for landheights when there is no land.
	int			 iCheckFlag;	//  User Flag to check;
	void		 *pLeadVehicle; //  The Vehicle this is following.
	void		 *pTrailVehicle; //  The Vehicle which is following this vehicle.
	int			 iPlaceInGroup;
	float		 fVehicleSpacing;  //  This is the spacing to maintain between this vehicle and its leader;
	long		 lAIFlags1;		//  Used for bit flags.
	float		 fSustainableDamage;
	float		 fTotalDamage;
	DWORD		 LastDamagedTick;		// Also used when vehicles are falling
	DWORD		 Flags;
	DWORD		 Smoke[MAX_VEHICLE_SMOKE_SLOTS];
	DWORD		 TicksSinceFireStarted;
	MovingVehicleType *DestroyedType;	//  Holds the pointer to the destroyed type.
	int			 iAnimationValue;		//  Holds value for animations (camel walks, skud launcher.
	int			 iAnimationWork;		//  Used for keeping max timing values, for example camels will "walk" at different rates.
	GDRadarData  RadarWeaponData[NUM_DEFENSE_LIST_ITEMS];		//  Holds the radar and weapon data for vehiles. Same as ground defense stuff.
	int			 lVar1;
	int			 lVar2;
	float		 fSubmergeDepth;
	FPoint		 FormationOffset;
	Wake		 *WakeData;
	WireData	 CatchWire;
	BYTE		 bFXArray[MAX_VEHICLE_FX];
	BYTE		 bFXDesiredArray[MAX_VEHICLE_FX];
	BYTE		 bDamageLevel;
	float		 listroll;
	long		 lNetTimer;
	ShadowMapEntry *Shadow;
	ShadowInstance *PlaneShadows;
	FauxRunway   Runwaydata;
} MovingVehicleParams;


#endif

BOOL PlaceVehiclesOnBridges(CameraInstance *camera);
void DisplayVehicles(CameraInstance *camera);
float VDetermineNextWayPoint(RoutePoint	 *currway, int numwayrem, int reverse, RoutePoint *startway, FPoint *newway, int firstwaypoint = 0, MovingVehicleParams *vehiclepnt = NULL, int ignoreform = 1);
void VNextWayPoint(MovingVehicleParams *vehiclepnt, float remaingdist);
void VMoveVehicles();
void VCheckWayPointAction(MovingVehicleParams *vehiclepnt);
void VCheckWaiting(MovingVehicleParams *vehiclepnt);
void VCheckLeader(MovingVehicleParams *vehiclepnt, MovingVehicleParams *leader);
void VCheckForTrafficJam(MovingVehicleParams *vehiclepnt);
BOOL VDamageVehicle(MovingVehicleParams *vehiclepnt, DWORD damage_type, DWORD game_loop, float damage, int *num_secondaries, float *delay_time, MultipleExplosionType *secondaries);
float VGetSinPitchFromWayPoint(FPointDouble WayPosition, FPointDouble WorldPosition);
void AISimpleVehicleRadar(MovingVehicleParams *radarsite, GDRadarData *radardat);
void VClearRadardat(MovingVehicleParams *vehiclepnt);
void VGDCheckDefenses(MovingVehicleParams *vehiclepnt);
int VGDSetUpWeaponTargeting(MovingVehicleParams *foundweapon, PlaneParams *planepnt, float currdist, GDRadarData *radardat, GDRadarData *rradardat);
void VAIGroundWeaponAction(MovingVehicleParams *currweapon, GDRadarData *radardat);
void *VCheckForSurfaceAttack(MovingVehicleParams *vehiclepnt, int *returntype, GDRadarData *radardat);
int AISeeIfAlreadyTarget(int targettype, void *targobj);
int DrawAVehicle(MovingVehicleParams *W, FPoint &relative_position,BYTE *carrierlights = NULL);
int DrawSubVehicles(void *object_info);
void DestroyVehicle(MovingVehicleParams *vehiclepnt,BOOL play_sound);
float SetMeatballVars(FPointDouble &location,MovingVehicleParams *carrier,BYTE *varslist);
void DrawMeatballForPlane();

void InstantiateAAABullet( MovingVehicleParams *P,ANGLE pitch, ANGLE heading,long id, int radardatnum = 0);  //  Prototype moved from weapons.h
float GetPlaneVehicleRadarVisibility( MovingVehicleParams *site, PlaneParams *P, BOOL plane_to_radar, int radarid);
int AISeeIfStillVehicleMissiled(MovingVehicleParams *radarsite, PlaneParams *targplane, int maintain, GDRadarData *radardat, int missilesonly = 0);

BOOL UpdateTailHookAndCheckForWireCatch(PlaneParams *P,MovingVehicleParams *V);
BOOL AngleForTailHook(PlaneType *ptype,FPoint &cooef,float D,float &rangle,BYTE &angle);
double GetPlaneLandHeightWithCarriers(PlaneParams *P,MovingVehicleParams **v=NULL,double *addnl_height=NULL);
void PlaneReleaseTailhook(PlaneParams *P,MovingVehicleParams *V = NULL);


//  New for F/A-18
void VUpdatePlanesOnDeck(MovingVehicleParams *vehiclepnt);
float CalcHeightAboveDeck( PlaneParams *P, MovingVehicleParams *vehiclepnt);
void AISimpleShipRadar(MovingVehicleParams *radarsite, GDRadarData *radardat);
int VGDAssignShipWeapon(MovingVehicleParams *vehiclepnt, int targettype, void *target, int targdistnm, GDRadarData *rradardat);
int VGDRadarOn(MovingVehicleParams *vehiclepnt);
DBWeaponType *VGDGetShipWeaponForRDNum(MovingVehicleParams *vehiclepnt, int radardatnum);
GDRadarData *VGDGetRadarDatAtPlane(MovingVehicleParams *vehiclepnt, PlaneParams *planepnt, int *found = NULL);
int VGDCheckVWeapCanFire(MovingVehicleParams *vehiclepnt, GDRadarData *radardat, GDRadarData *rradardat, int targettype, void *target);
void VGDGetWeaponPosition(void *ocurrweapon, int sitetype, int radardatnum, FPointDouble *weaponoffset);
PlaneParams *GetNewCarrierPlane(PlaneParams *P);
PlaneParams *GetNewCarrierPlane(PlaneParams *P,MovingVehicleParams *carrier);
void RestoreValuesToPlane(PlaneParams *dest);
void CopyCarrierPlaneToWorldPlane(PlaneParams *dest);
void CopyWorldPlaneToCarrierPlane(PlaneParams *source);
void ApplyCarrierMovementToPlane(PlaneParams *dest);
void VSetUpInitialFormations();
void VAIGetFormationOffset(MovingVehicleParams *vehiclepnt, int placeingroup, FPoint *formation_offset, MovingVehicleParams *leadpnt, int formationid);
void VAIGetShipFormationOffset(MovingVehicleParams *vehiclepnt, int placeingroup, FPoint *formation_offset, MovingVehicleParams *leadpnt, int formationid);
int VConvertVGListNumToVNum(int vglistnum);
void VSetShipDamageLevel(int vnum, int level, int initialize_damage);
void VGetShipGroupDamageState(int resourceid, int *iship_damage_array);
void VSetShipGroupDamageState(int resourceid, int *iship_damage_array);
void VDamageVehicleRadars(MovingVehicleParams *vehiclepoint, FPointDouble *position, float fMultiplier, float fBlastRadius);
int AICheckRadarCrossSig(MovingVehicleParams *vehiclepnt, PlaneParams *checkplane, float daltft, float distnm, DBRadarType *radar, int inSTT);
float AGClosureRate(MovingVehicleParams *vehiclepnt, PlaneParams *T );
int VGetMaxShipRadarRange(MovingVehicleParams *vehiclepnt);
DBRadarType *VGetMaxShipRadarPtr(MovingVehicleParams *vehiclepnt);
void GetLandingPoint(MovingVehicleParams *V,FPointDouble &location);
void VehicleStartHeatFire(MovingVehicleParams *V,FPointDouble &position,float fDistance,float fBlastRadius,void *plane = NULL);
void VehicleStartNapalmFire(MovingVehicleParams *V,float t,void *plane = NULL);
void PositionFireOnVehicle(MovingVehicleParams *V,FPointDouble &world_position,BOOL random);
int AIPutVehicleRadarInTEWSCenter(MovingVehicleParams *vehiclepnt, GDRadarData *radardat);
int AICheckForGateStealerV(MovingVehicleParams *radarsite, GDRadarData *radardat, PlaneParams *target);
void VGetShipGroupRadarDamageState(int resourceid, int *iship_radar_damage_array);
void VSetShipGroupRadarDamageState(int resourceid, int *iship_radar_damage_array);

//  In behave.cpp with other SAR stuff
int AICheckForVehicleSAR(MovingVehicleParams *vehiclepnt);
