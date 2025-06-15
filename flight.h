#ifndef __FLIGHT_H__

#define __FLIGHT_H__

#define WUTOFT      (5.7435f)
#define NMTOFT      (6076.115f)
#define WUTONM      (WUTOFT / NMTOFT)

#define FTTOWU		(1.0f/WUTOFT)
#define FTTONM		(1.0f/NMTOFT)
#define NMTOWU		(NMTOFT / WUTOFT)

#define HIGH_FREQ	(1.0f/50.0f)
#define MED_FREQ	(1.0f/25.0f)
#define LOW_FREQ	(1.0f/10.0f)

#define HIGH_AERO	0
#define MED_AERO	1
#define LOW_AERO	2

#define GRAVITY		32.0

#define NULLVECTOR	      {0.0f,0.0f,0.0f}
#define DOUBLENULLVECTOR  {(double)0.0,(double)0.0,(double)0.0}

#define UNITPLANENORTH    {{0.0f,0.0f,-1.0f}, {1.0f,0.0f,0.0f},  {0.0f,-1.0f,0.0f}}
#define UNITPLANESOUTH    {{0.0f,0.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,-1.0f,0.0f}}
#define UNITPLANEEAST     {{1.0f,0.0f,0.0f},  {0.0f,0.0f,1.0f},  {0.0f,-1.0f,0.0f}}
#define UNITPLANEWEST     {{-1.0f,0.0f,0.0f}, {0.0f,0.0f,-1.0f}, {0.0f,-1.0f,0.0f}}

#define UNITPLANEDOWN     {{0.0f,-1.0f,0.0f}, {1.0f,0.0f,0.0f},  {0.0f,0.0f,1.0f}}
#define UNITPLANEUP       {{0.0f,1.0f,0.0f},  {1.0f,0.0f,0.0f},  {0.0f,0.0f,1.0f}}

#define NULLFMATRIX       {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}

//***********************************************************************************************************************************
//Defines for the raise landing gear functions

#define RAISE_LANDING_GEAR		0
#define LOWER_LANDING_GEAR		1

//***********************************************************************************************************************************

/* Flags for PlaneParams.Status*/

#define PL_ACTIVE	      	0x00000001
#define PL_AI_DRIVEN      	0x00000002
#define PL_DEVICE_DRIVEN  	0x00000004
#define PL_COMM_DRIVEN    	0x00000008
#define PL_NEED_ATTITUDE  	0x00000010
#define PL_CARRIER_REL		0x00000020
#define PL_ONGROUND_NEXT	0x00000040
#define PL_SET_PLAYER_DECEL 0x00000080
#define PL_SPEED_BOLTER		0x00000100
#define PL_RUNNING_LIGHTS  	0x00000800
#define PL_FORMATION_LIGHTS	0x00003000


/* Flags for PlaneParams.FlightStatus*/

#define PL_STATUS_TAKEOFF	  0x00000001
#define PL_STATUS_CRASHED	  0x00000002
#define PL_OUT_OF_CONTROL	  0x00000004
#define PL_GEAR_DOWN_LOCKED   0x00000008
#define PL_PLANE_BLOWN_UP	  0x00000010
#define PL_PLANE_DITCHING	  0x00000020

/* Flags for PlaneParams.SystemsStatus*/

#define PL_ENGINES						0x0000000F
#define PL_ENGINE_REAR					0x00000001
#define PL_ENGINE_REAR_RIGHT			0x00000001
#define PL_ENGINE_REAR_LEFT				0x00000002

#define PL_ENGINE_WING_LEFT_OUTBOARD	0x00000001
#define PL_ENGINE_WING_LEFT_INBOARD		0x00000002
#define PL_ENGINE_WING_RIGHT_OUTBOARD	0x00000004
#define PL_ENGINE_WING_RIGHT_INBOARD	0x00000008

/* Flags for PlaneParams.LandingGearStatus*/

#define PL_LG_REQUEST_OPEN	  0x0001
#define PL_LG_REQUEST_CLOSED  0x0002
#define PL_LG_STATE1		  0x0003
#define PL_LG_STATE2		  0x0004
#define PL_LG_STATE3		  0x0005
#define PL_LG_STATE4		  0x0006
#define PL_LG_STATE5		  0x0007
#define PL_LG_STATE6		  0x0008

#define PL_LG_WORKING		  0x00000003

#define GEAR_OPEN_SPEED_RATIO ((float)0xFFFF/100.0f)

/* Flags for Flaps Mode */

#define FLAPS_AUTO	0x0001
#define FLAPS_FULL	0x0002
#define FLAPS_HALF	0x0004

/* Flags for various AutoPilot modes */


/*
// these were for F-15's autopilot
#define	PL_AP_OFF						0x0001
#define	PL_AP_TRIMMED					0x0001
#define	PL_AP_ATTITUDE					0x0002
#define	PL_AP_ALTITUDE					0x0004
#define	PL_AP_NAV_ALTITUDE				0x0008
#define	PL_AP_TACAN_ALTITUDE  			0x0010
#define	PL_AP_HDG						0x0020
#define	PL_AP_NAV						0x0040
#define	PL_AP_TACAN						0x0080
#define PL_AP_ALT_BARO					0x0100
#define PL_AP_ALT_RDR					0x0200
*/

// New for F/A-18

#define	PL_AP_OFF						0x0001
#define	PL_AP_TRIMMED					0x0001

// Pitch Axis
#define PL_AP_ALT_BARO					0x0002
#define PL_AP_ALT_RDR					0x0004
#define	PL_AP_ATTITUDE					0x0008

//Roll Axis
#define PL_AP_CPL						0x0020
#define PL_AP_ROLL						0x0040
#define	PL_AP_HDG						0x0080

// Engine Status Variables

#define PL_LEFT_ENGINE_ON	0x0001
#define PL_RIGHT_ENGINE_ON	0x0002

#define EasyFlight        0//(!(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_FLIGHT))
#define RealisticLandings (g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_LANDINGS)
#define RealisticCrashes  (g_Settings.gp.dwFlight & GP_FLIGHT_CRASHES)
#define WeaponsDragWeight (g_Settings.gp.dwFlight & GP_FLIGHT_WEAPON_WEIGHT_DRAG)
#define UnlimitedFuel     (!(g_Settings.gp.dwFlight & GP_FLIGHT_LIMITED_FUEL))
#define RedBlackOut		  (g_Settings.gp.dwFlight & GP_FLIGHT_READOUT_BLACKOUT)
#define RealisticAPs	  (g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_AUTOPILOTS)

#define LeftEngineOn	  (PlayerPlane->EngineStatus & PL_LEFT_ENGINE_ON)
#define RightEngineOn	  (PlayerPlane->EngineStatus & PL_RIGHT_ENGINE_ON)


// Misc Defines
#define NOSE_STEERING_HANDOFF	130

#define KEY_RIGHT_RUDDER	0x0001
#define KEY_LEFT_RUDDER		0x0002

#define AILERON_OFFSET	(384/2)
#define RUDDER_OFFSET	(2*256)
#define NOSE_OFFSET		(384/2)


//***********************************************************************************************************************************
typedef FPoint Vector3D;

typedef FPointDouble DoubleVector3D;

typedef struct {
	FPointDoubleData	I;
	FPointDoubleData	J;
	FPointDoubleData	K;
} InitRefFrame;

typedef struct {
	DoubleVector3D	I;
	DoubleVector3D	J;
	DoubleVector3D	K;
}RefFrame;

typedef struct {
	float		Low;
	float		High;
}ControlRange;

typedef struct {

	float	CD0;		// zero alpha drag
	float	CDa;		// drag from alpha
	float	CDb;		// drag from beta
	float	CDdde;		// drag from elevator deflection
	float	CDdf;		// drag from flap deflection
	float	CDdsb;		// drag from speed brake deflection

	float	CL0;		// zero alpha lift
	float	CLa;		// lift from alpha
	float	CLq;		// lift from pitching velocity
	float	CLdde;		// lift from elevator
	float	CLdf;		// lift from flaps

	float	CYb;		// sideforce from sideslip
	float	CYdde;		// sideforce from elevator
	float	CYdr;		// sideforce from rudder
	float	CYda;		// sideforce from aileron

	float	Clb;		// roll moment from sideslip
	float	Clp;		// roll moment from roll rate
	float	Clr;		// roll moment from yaw rate
	float	Cldr;		// roll moment from rudder
	float	Clda;		// roll moment from aileron
	float	Cldde;		// roll moment from elevator

	float	Cm0;		// zero alpha pitch moment
	float	Cmu;		// pitch moment from speed
	float	Cma;		// pitch moment from alpha
	float	Cmq;		// pitch moment from pitch rate
	float	Cmdde;		// pitch moment from tail
	float	Cmdf;		// pitch moment from flap
	float	Cmdsb;		// pitch moment from speed brake
	float	Cmdlg;		// pitch moment from gear

	float	Cnb;		// yaw moment from sideslip
	float	Cnp;		// yaw moment from roll rate
	float	Cnr;		// yaw moment from yaw rate
	float	Cndr;		// yaw moment from rudder
	float	Cnda;		// yaw moment from aileron
	float	Cndde;		// yaw moment from elevator

}StabilityDerivatives;

typedef struct {

 	float AileronRate;				// deg./sec.
	float RudderRate;
	float ElevatorRate;
	float FlapRate;
	float LEFlapRate;
	float SpeedBrakeRate;

	ControlRange AileronRange;		// -deg to +deg
	ControlRange RudderRange;
	ControlRange SymetricalElevatorRange;
	ControlRange DifferentialElevatorRange;
	ControlRange FlapRange;
	ControlRange LEFlapRange;
	ControlRange SpeedBrakeRange;

}ControlSurfaces;

typedef struct {

	float	Weight;		// Default to clean plane, then add armament
	float	Mass;		// Default to clean plane, then add armament
	float	Ix;			// Inertia (slug*Ft^2)
	float	Iy;
	float	Iz;
	float	S;			// Wing area (ft^2)
	float	b;			// Wing span (ft)
	float	c;			// Wing chord (ft)

}PlaneStatistics;

#define MAX_OFFSETS							12

#define MAX_CONNECTED_PARTS					8
#define MAX_SUBPARTS						50

#define OFFSET_WING_LEFT 					0
#define OFFSET_WING_RIGHT					1

#define OFFSET_COCKPIT_FRONT				2
#define OFFSET_COCKPIT_REAR					3

#define OFFSET_ENGINE_REAR_LEFT				4
#define OFFSET_ENGINE_REAR_RIGHT			5

#define OFFSET_ENGINE_REAR					4

#define OFFSET_ENGINE_WING_LEFT				5
#define OFFSET_ENGINE_WING_RIGHT			7

#define OFFSET_ENGINE_WING_LEFT_OUTBOARD	5
#define OFFSET_ENGINE_WING_LEFT_INBOARD		6
#define OFFSET_ENGINE_WING_RIGHT_OUTBOARD	7
#define OFFSET_ENGINE_WING_RIGHT_INBOARD	8

#define OFFSET_BURNING						9

#define OFFSET_LEFT_FUEL_DUMP				10
#define OFFSET_RIGHT_FUEL_DUMP				11

/* engine configs */

#define TWO_REAR_ENGINES					1
#define ONE_REAR_ENGINE						2
#define TWO_WING_ENGINES					4
#define FOUR_WING_ENGINES					8

/*Plane Types */


#define PT_F18e		0
#define PT_F16		1
#define PT_F111		2
#define PT_FA18		3
#define PT_F14		4
#define PT_F117a    5
#define PT_F15c		11
#define PT_MIG21	6
#define PT_MIG23	7
#define PT_MIG25    8
#define PT_MIG29	9
#define PT_SU27		10
#define PT_AWACS	12
#define PT_MIRAGE   13
#define PT_C130		14
#define PT_C17		15


typedef struct _PlaneType
{
	int			  TypeNumber;
	ObjectHandler *Model;
	ObjectHandler *SubParts[MAX_CONNECTED_PARTS];
	TextureBuff   *Shadow;
	int			  EngineConfig;
	FPoint		  Offsets[MAX_OFFSETS];
	float		  ShadowULXOff;
	float		  ShadowULZOff;
	float		  ShadowLRXOff;
	float		  ShadowLRZOff;
	float		  GearUpHeight;
	float		  GearDownHeight;
	float		  TailHeight;
	FPoint		  TailHookPoint;
	FPoint		  TailHookPivot;
	float		  TailHookLength;
	float		  TailHookRestAngle;
	FPoint		  TowedDecoyPosition;
	FPoint		  RefuelingHosePositions[5];
	FPoint		  MachineGun;
	FPoint		  FrontWheelContact;
	FPoint		  RightRearWheelContact;
	FPoint		  LeftRearWheelContact;
	ObjectHandler *DestroyedModel;
} PlaneType;

typedef struct _PlaneEngineFire
{
	unsigned short TotalTimeLeft;
	unsigned char  LoopsTillNextSprite;
	unsigned char  LoopsTillNextGenerator;
} PlaneEngineFire;


#define MAX_REFUELING_HOSES 5

#define RH_WANT_RETRACT		0
#define RH_WANT_EXTEND		1
#define RH_CONNECTED		2

typedef struct _RefuelHoseData
{
	DWORD Status;
	float CommandedPosition;
	float Position;
	float ConnectedPosition;
	DWORD ConnectedPlane;
	FPoint ComputedLocation;
	FPoint OffsetToConnectedPlane;
} RefuelHoseData;

#define MAX_PLANE_SMOKE_TRAILS				14

#define SMOKE_ENGINE_REAR					0

#define SMOKE_ENGINE_WING_LEFT_OUTBOARD		1
#define SMOKE_ENGINE_WING_LEFT_INBOARD		2
#define SMOKE_ENGINE_WING_RIGHT_OUTBOARD	3
#define SMOKE_ENGINE_WING_RIGHT_INBOARD		4

#define SMOKE_ENGINE_REAR_LEFT				0
#define SMOKE_ENGINE_REAR_RIGHT				1

#define SMOKE_WING_LEFT						5
#define SMOKE_WING_RIGHT					6

#define SMOKE_BURNING						7

#define SMOKE_LEFT_FUEL_DUMP				8
#define SMOKE_RIGHT_FUEL_DUMP				9

#define SMOKE_LEFT_AB_SMOKE					10
#define SMOKE_RIGHT_AB_SMOKE				11

#define SMOKE_LEFT_ENGINE_FIRE				12
#define SMOKE_RIGHT_ENGINE_FIRE				13


#define TH_WIRE_FLAGS			0x07
#define TH_HOOKED				0x10
#define TH_TOUCHED_DOWN			0x20
#define TH_IN_CHANNEL			0x40
#define TH_BACK_UP				0x80
#define TH_BOLTER_FLAG			0x40004

#define TOWED_WANT_RETRACT		0x00000000
#define TOWED_WANT_DEPLOYED		0x00000001
#define TOWED_DANGLING			0x00000002

typedef struct _WeaponType
{
	int			  TypeNumber;
	ObjectHandler *Model;
	ObjectHandler *SubParts[MAX_CONNECTED_PARTS];
} WeaponType;

// defines the weapon loadout that will be in the PlaneParams structure
typedef struct
{
  int WeapId;
  int Count;
  int Status;
  int WeapIndex;
  WeaponType *Type;
  BYTE bNumPods;
} WeaponLoadoutInfoType;


//Forward declared so that initplaneparams can define pointers to it
class PlaneParams;

//***********************************************************************************************************************************
// Used to hold initialization data for the PlaneParams class

typedef struct _InitPlaneParams
{
	// General Header/Flag Stuff
	int		Status;
	int		EngineStatus;

	// Timing loop variables
	int		UpdateRate;
	int		AeroComplexity;
	float	dt;

	// Body Frame Data
	float	LastAlpha;		// previous angle of attack for time delta derivatives

	// Inertial Frame Data
	InitRefFrame	Orientation;	// aircraft orientation vectors in inertial frame - normalized
	float	AvailGForce;			// currently computed max g-force that will over g the plane

	// Airframe Systems
	int	 	AutoPilotMode;
	int 	FlapsMode;
	int 	LastFlapsMode;
	float	CurrentTrimValue;

	// Misc weapons system stuff
	int	 	GunFireRate;			// rate loop runs at 50hz. so init to 5 for a 1/10 second rate

	// Pointers to Data for Real Flight Simulation Model
	PlaneStatistics			*Stats;
	StabilityDerivatives	*C;
	ControlSurfaces			*CS;

	// Data for Simple AI Flight Model
	float			MaxSpeedAccel;				// ft/second^2
	float			SpeedPercentage;			// (0.0 - 1.0) -> 0.5 is 50% of MaxSpeed Acceleration

	float			MaxRollRate;				// degrees/second
	float			RollPercentage;				// (0.0 - 1.0) -> 0.5 is 50% of MaxRollRate

	float			MaxPitchRate;				// degrees/second
	float			PitchPercentage;			// (0.0 - 1.0) -> 0.5 is 50% of MaxPitchRate

	float			YawRate;					// degrees/second

	int				RollYawCouple;				// 1 for YES, 0 for NO

	// Pointers to Functions for specific Simulation Model

	void (*DoGetPlayerInputs)(PlaneParams *P);
	void (*DoControlSim)(PlaneParams *P);
	void (*DoCASSim)(PlaneParams *P);
	void (*DoForces)(PlaneParams *P);
	void (*DoPhysics)(PlaneParams *P);
	void (*DoAttitude)(PlaneParams *P);

	void (*DoSimpleFlight)(PlaneParams *P);
	void (*DoSimpleAttitude)(PlaneParams *P);

} InitPlaneParams;

class PlaneParams

{

	public:

	PlaneParams &operator=(const InitPlaneParams &init_params);
	PlaneParams &operator=(const PlaneParams &plane) {memcpy(this,&plane,sizeof(PlaneParams)); return *this;};

	// General Header/Flag Stuff
	PlaneParams			*PlaneCopy;  //Used for planes on carriers
	PlaneParams			*NextVisiblePlane;  // used during rendering to reduce trips through the plane list
	ShadowMapEntry		*Shadow;		    // Contains data about this plane's shadow

	Behavior			AI;
	PlaneType			*Type;
	int					Status;
	int					FlightStatus;
	int					SystemsStatus;
	long				DamageFlags;
	long				SystemInactive;
	void				*Smoke[MAX_PLANE_SMOKE_TRAILS];
	PlaneEngineFire		EngineFires[MAX_PLANE_SMOKE_TRAILS];

	// Timing loop variables

	int				UpdateRate;		// High,Med,Low
	int 			AeroComplexity;	// High,Med,Low
	float			dt;

	// Body Frame Data

	double			V;				// BodyFrame Velocity
	double			Knots;			// Knots
	float			Alpha;			// angle of attack
	float			LastAlpha;		// previous angle of attack for time delta derivatives
	float			Beta;			// side slip angle

 	DoubleVector3D 	BfForce;		// Forces in body frame
	DoubleVector3D 	BfMoment;		// Moments in body frame

	DoubleVector3D 	BfLinVel;		// linear velocity in body frame
	DoubleVector3D 	BfGhostLinVel;	// Ghost linear velocity in body frame
	DoubleVector3D 	BfRotVel;		// rotational velocity in body frame

	DoubleVector3D 	BfLinAccel;
	DoubleVector3D 	BfRotAccel;

	// Inertial Frame Data

	DoubleVector3D  BfVelocity;		// velocity in body frame (double) - true airspeed
	DoubleVector3D	IfVelocity;		// velocity in inertial frame (double) - true airspeed
	float			IfHorzVelocity;
	float			IndicatedAirSpeed;

	RefFrame		Orientation;	// aircraft orientation vectors in inertial frame - normalized
	float			Mach;			// based on altitude
	float			Altitude;   	// based on WorldPosition.Y*WU_TO_FEET
	unsigned int 	AGL;
	float			GForce;
	float			AvailGForce;	// currently computed max g-force that will over g the plane
	float			RequestedG;
	float			PilotGDelta;
	float			PilotGEffortRemaining;
	float			PilotGThreshold;
	float			PilotGLoad;     // Where the pilot is as far as red out / black out.  In the range of -2.0f to 3.0f

	ANGLE			Roll;
	ANGLE			Pitch;
	ANGLE			Heading;

	float			DistanceMoved;
	FPointDouble	LastWorldPosition;  // X,Y,Z
	FPointDouble	WorldPosition;		// X,Y,Z
	float			HeightAboveGround;
	FMatrix			Attitude;			// Rotation matrix

	// Airframe Systems

	float			TotalWeaponDragIndex;
	float			AircraftDryWeight;  // including weapon loadout
	float			InternalFuel;		// including conformals
	float			CenterDropFuel;
	float			WingDropFuel;
	float			TotalFuel;			// lot's of stuff uses this so it's here
	float			TotalWeight;		// lot's of stuff uses this so it's here
	float			TotalMass;			// lot's of stuff uses this so it's here
	float			WeaponDrag;

	int				EngineStatus;
	float			CommandedThrust;		// might need right and left
	float			LeftThrust;
	float			RightThrust;
	float			LeftThrustPercent;
	float			RightThrustPercent;

	float			ElevatorTrim;
	float			AltitudeHoldTrim;
	float			AttitudeHoldTrim;

	float			Aileron;				// current position
	float			Rudder;
	float			TotalElevator;			// get rid of
	float			SymetricalElevator;
	float			DifferentialElevator;

	float			Flaps;
	float			FlapsCommandedPos;
	float			LEFlaps;
	float			LEFlapsCommandedPos;
	float			LastFlapsCommandedPos;
	float			SpeedBrake;
	float			SpeedBrakeCommandedPos;
	int			    SpeedBrakeState;		// have we commanded speed brake?

	float			SBRudderOffset;			// Speedbrake rudder offset - toe in
	float			SBAileronOffset;		// Speedbrake aileron offset - droop down
	float			FLPSAileronOffset;		// Allow aileron to appear to function as Flaps do

	float			WingUp;
	float			WingUpCommandedPos;
	float			TailHook;
	float			TailHookCommandedPos;
	int			    TailHookState;
	PointLightSource **ABLight;
	PointLightSource **MachineGunLight;
	PointLightSource **FuelProbeLight;
	float			TicksWithoutOxygen;
	void 			*ShipWeAreAbove;

	// Data for keeping track of the tailhook position when it's down.
	DWORD			TailHookFlags;
	FPointDouble	LastTailHookPosition;
	FPointDouble	TailHookPosition;

	float			VaporAmnt;

	// data for the towed decoy stuff
	int				TowedDecoyState;
	float			TowedDecoyCommandedPosition;
	float			TowedDecoyPosition;

	RefuelHoseData  RefuelingHoses[MAX_REFUELING_HOSES];

	float			RefuelProbe;
	float			RefuelProbeCommandedPos;
	float			Canopy;
	float			CanopyCommandedPos;
	float			LeadingEdge;
	float			LeadingEdgeCommandedPos;

  	float			StickX;					// side to side
	float			StickY;					// forward to back
	int				Trigger1;
	int				Trigger2;
	float			RudderPedals;
	float			RudderPedalsCommandedPos;
	float			ThrottlePos;
	float			CommandedThrottlePos;
	float			LeftThrottlePos;
	float			RightThrottlePos;
	float			LeftCommandedThrottlePos;
	float			RightCommandedThrottlePos;

	int				KeyboardAB;
	float			LastKeyboardThrottle;

	short			LandingGearStatus;
	BYTE			LandingGear;
	int				LandingGearLoop;
	float			HeightForGear;

	int				OnGround;
	int				Brakes;
	float			RollRatio;
	float			PitchRatio;
	float			RudderRatio;
	int				AutoPilotMode;
	int				FlapsMode;
	int				LastFlapsMode;

	float			tmp2;
	float			NewRequestedAOA;
	float			TrimToAOA;
	float			RequestedAOAPercentage;
	float			tmp1;
	int				OutOfControlFlight;

	int 			APAltCountDown;
	int 			APAttCountDown;
	float			CurrentTrimValue;

	// Misc data for simple weapons systems (this will eventually be a pointer to a weapons structure)

	int				GunFireRate;				// rate loop runs at 50hz. so init to 5 for a 1/10 second rate
	int				ElapsedFireRate;

	// Pointers to Data for Real Flight Simulation Model

	PlaneStatistics			*Stats;
	StabilityDerivatives	*C;
	ControlSurfaces			*CS;

	// Data for Simple AI Flight Model

	float			DesiredSpeed;				// ft/second
	float			MaxSpeedAccel;				// ft/second^2
	float			SpeedPercentage;			// (0.0 - 1.0) -> 0.5 is 50% of MaxSpeed Acceleration

	float			DesiredRoll;				// degrees (0.0-359.0)
	float			MaxRollRate;				// degrees/second
	float			RollPercentage;				// (0.0 - 1.0) -> 0.5 is 50% of MaxRollRate

	float			DesiredPitch;				// degrees (0.0-359.0)
	float			MaxPitchRate;				// degrees/second
	float			PitchPercentage;			// (0.0 - 1.0) -> 0.5 is 50% of MaxPitchRate

	float			YawRate;					// degrees/second
	int				RollYawCouple;				// 1 for YES, 0 for NO

	//	These hold data for determining what type of ground the plane is on
	int			TerrainType;				// The type of ground beneath the plane
	int			TerrainType1;
	int			TerrainType2;

	// Weapons and Loadout data
	WeaponLoadoutInfoType WeapLoad[15];

	// Target Designator place holders
	PlaneParams   *AADesignate;
	FPointDouble   AGDesignate;				// X,Y,Z
	int            AGDesignator;

	// Tailfin designations
	int				TailYear;
	int				TailSerial;
	int				TailSquadron;
	int				TailBase;

	// Pointers to Functions for specific Simulation Model

	void (*DoGetPlayerInputs)(PlaneParams *P);
	void (*DoControlSim)(PlaneParams *P);
	void (*DoCASSim)(PlaneParams *P);
	void (*DoForces)(PlaneParams *P);
	void (*DoPhysics)(PlaneParams *P);
	void (*DoAttitude)(PlaneParams *P);

	void (*DoSimpleFlight)(PlaneParams *P);
	void (*DoSimpleAttitude)(PlaneParams *P);
};

//***********************************************************************************************************************************

float GetCommandedThrust(PlaneParams *P, float ThrustPercent);
float GetDensityFactor(PlaneParams *P);
void  GetF18StickPos(PlaneParams *P);

float Integrate(float InputPos, float InputPosMax, float CurrentDegree,
					   ControlRange Range, float Rate, float dt, float AINewDegree);

void CalcF18ControlSurfaces(PlaneParams *P);
void CalcAeroForces(PlaneParams *P);
void CalcAeroDynamics(PlaneParams *P);
void CalcF18CASUpdates(PlaneParams *P);

void CalcF18ControlSurfacesSimple(PlaneParams *P);
void CalcAeroForcesSimple(PlaneParams *P);
void CalcAeroDynamicsSimple(PlaneParams *P);
void CalcF18CASUpdatesSimple(PlaneParams *P);

void CalcSimpleFlight(PlaneParams *P);
void CalcSimpleAttitude(PlaneParams *P);

void CalcAttitude(PlaneParams *P);
void ControlPlanes(void);
void MovePlanes(void);
void FlightRecordData(PlaneParams *P);
void DisplayFlightData(PlaneParams *P);
void InitOurPlane(void);

void MoveLandingGear(PlaneParams* P);
void SetLandingGearUpDown(PlaneParams* P,int gear_down);
void RaiseLowerLandingGear(PlaneParams* P,int gear_down);

float DegToRad(float Degree);
float RadToDeg(float Rad);
//float DegAOAToUnits(float Deg);

inline float KnotsTASToIAS(float KnotsTAS, float Alt);
inline float KnotsIASToTAS(float KnotsIAS, float Alt);
inline float FtSecTASToIAS(float FtSecTAS, float Alt);
inline float FtSecIASToTAS(float FtSecIAS, float Alt);
float MachToFtSec(PlaneParams *P, float Mach);
float GetFuelFlow(PlaneParams *P, float ThrustPercent);
void ConsumeFuel(PlaneParams *P, unsigned int DeltaTicks);

void UpdateSpeedBrakePosition(PlaneParams *P);
void DoAileronRudderInterconnect(PlaneParams *P);

void ToggleFlaps(void);
void ToggleSpeedBrake(void);
void ToggleGear(void);

void KeyThrottleOff(void);
void KeyThrottleIdle(void);
void KeyThrottleMil(void);
void KeyThrottleAB(void);
void KeyThrottleUp(void);
void KeyThrottleDown(void);
void ForceKeyboardThrust(float Value);

float GetOneGSlabPreset(PlaneParams *P);
void ToggleLeftEngineStatus(void);
void ToggleRightEngineStatus(void);

void DefaultTrim(void);
void AileronTrimLeft(void);
void AileronTrimRight(void);
void RudderTrimLeft(void);
void RudderTrimRight(void);
void NoseTrimUp(void);
void NoseTrimDown(void);

void TrimTo(PlaneParams *P, float GForceTo);

extern void FlameOutStopEngineSounds();
void SetUpDitch(PlaneParams *P);

extern int g_iSoundLevelExternalSFX;
extern int TimeExcel;
extern int g_iSoundLevelCaution;

#ifdef _DEBUG

inline float DegToRad(float Degree);
inline float RadToDeg(float Rad);
//inline float DegAOAToUnits(float Deg);

#else

/* Please reflect any changes made here in the debug versions of these functions */
/* Which are located in FLIGHT.CPP											     */

//************************************************************************************************
inline float DegToRad(float Degree)
{
	return (Degree*(PI/180.0));
}
//************************************************************************************************
inline float RadToDeg(float Rad)
{
	return (Rad*(180.0/PI));
}
//************************************************************************************************
//inline float DegAOAToUnits(float Deg)
//{
//	return((float)(Deg*(115.714f/180.0f))+9.343f);   // from video tape
//	return((float)(Deg*(115.714/180.0))+12.343);	// from McAir for F-15
//	return((float)Deg));							// F-18
//}

#endif

#endif