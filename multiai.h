//*****************************************************************************************
//  MULTIAI.H
//*****************************************************************************************
#ifndef __MULTI_AI_HEADER__
#define __MULTI_AI_HEADER__
#include "MultiMsg.h"

//*****************************************************************************************
//*  Bit flags for lFlags1 in the DPMSG_AI_POS structure
//*****************************************************************************************
#define NET_SBRAKE 0x1
#define NET_GEAR 0x2
#define NET_FLAPS 0x4
#define NET_AI_GUN 0x8
#define NET_TRIGGER2 0x10
#define NET_GUN_RATE 0x20
#define NET_BRAKES	0x40
#define NET_ON_GROUND 0x80
#define NET_JAMMER	0x100
#define NET_TOWED_JAMMING	0x200
#define NET_OUT_OF_CONTROL	0x400
#define	NET_RUNNING_LIGHTS	0x800
#define NET_FORMATION_LIGHTS	0x3000
#define NET_HOOK			0x4000

/*  Right now I'm putting a copy of cMultiCheckFlags into 0xFF00  */
#define MC_SHIFT	8
#define MC_lF1_MASK	0xFF00

//*****************************************************************************************
//*  Bit flags for bMultiCheckFlags
//*****************************************************************************************
#define MC_SUPPORT	0x1
#define MC_DEFENSIVE 0x2
#define MC_MISSILE_AVOID	0x4
#define MC_DOGFIGHT	0x8
#define MC_COMBAT_BEHAVE	0x10
#define MC_WINCHESTER_AA	0x20
#define MC_WINCHESTER_AG	0x40

#define PLAYERS			MAX_PLAYERS

//*****************************************************************************************
//*  Bit flags for iHotZoneCheckFlags
//*****************************************************************************************
#define ZONES_GET_MOVERS	0x1
#define ZONES_GET_STATIC	0x2
#define ZONES_SEND_FIRST	0x4

//*****************************************************************************************
//*  IDs for generic message with slot ID and 1 BYTE argument
//*****************************************************************************************
#define GM_EGRESS		1
#define GM_CAP_END		2
#define GM_GROUP_EVADE	3
#define GM_KILL_PLANE	4
#define GM_TANKER_DISCO	5
#define GM_TANKER_DISCO_HUMAN	6
#define GM_TANKER_DISCO_HUMAN2	7
#define GM_END_CASUAL_AUTO	8
#define GM_BOLTER		9
#define GM_CLEAN		10
#define GM_DROP_CHUTES	11
#define GM_ESCORT_ENGAGE	12
#define GM_ESCORT_SEAD	13
#define GM_ESCORT_CAS	14
#define GM_ESCORT_REJOIN	15
#define GM_ALLOW_REGEN	16
#define GM_REQUEST_WEATHER	17
#define GM_HOST_EXITS	18
#define GM_PLAYER_EXITS	19
#define GM_RELOAD_PLANE	20
#define GM_ESTABLISHED	21
#define GM_COMMENCE_NOW 22
#define GM_RETURN_MARSHAL	23
#define GM_PLATFORM	24
#define GM_ACLS_CONTACT	25
#define GM_BOLTER_RADIO	26
#define GM_BOLTER_RADIO_2	27
#define GM_SET_RELAUNCH	28
#define GM_RELEASE_HOOK	29

//*****************************************************************************************
//*  IDs for generic message with slot ID and 2 BYTE arguments
//*****************************************************************************************
#define GM2_VECTOR		1
#define GM2_ENGAGE_SET	2
#define GM2_NEW_THREAT	3
#define GM2_NEW_TARGET	4
#define GM2_NEW_TARGET_RAD	5
#define GM2_NEW_TARGET_PING 6
#define GM2_NEW_TARGET_PING_RAD 7
#define GM2_MULTI_FLAGS	8
#define GM2_UPDATE_BEHAVE	9
#define GM2_UPDATE_ORGBEHAVE	10
#define GM2_LINK_PLANE	11
#define GM2_ATTACH_HOSE	12
#define GM2_TRANSFER_CONTROL	13
#define GM2_DONE_TANKING		14
#define GM2_CONTACT_REFUELER_NO_B	15
#define GM2_AUTOTANK_1		16
#define GM2_AUTOTANK_2		17
#define GM2_GET_MARSHAL		18
#define GM2_AWACS_DECLARE	19
#define GM2_GENERIC_RADIO	20
#define GM2_SAR_PLANE		21
#define GM2_SAR_VEHICLE		22
#define GM2_REPLACE_CAP		23
#define GM2_SAR_CAP			24
#define GM2_WING_EJECT		25
#define GM2_ACLS_SET		26
#define GM2_LANDING_DIST	27
#define GM2_UPDATE_VOICE	28

//*****************************************************************************************
//*  IDs for generic message with slot ID and 3 BYTE arguments
//*****************************************************************************************
#define GM3_CAP_1		1
#define GM3_CAP_2		2
#define GM3_CAP_3		3
#define GM3_CAP_4		4
#define GM3_WING_HELP	5
#define GM3_CAP_ATTACK  6
#define GM3_UPDATE_BEHAVES	7
#define GM3_DESIRED_BOMBS	8
#define GM3_WING_UPD	9
#define GM3_PAIR_UPD	10
#define GM3_LOG_V_DEST  11
#define GM3_CONTACT_REFUELER	12
#define GM3_WAVEOFF		13
#define GM3_SORT_MSG	14
#define GM3_NUM_IN_PIC	15
#define GM3_SINK_SHIP	16

//*****************************************************************************************
//*  IDs for generic message with slot ID and 4 BYTE arguments
//*****************************************************************************************
#define GM4_START_ATTACK		1
#define GM4_DISENGAGE			2
#define GM4_SWITCH_ATTACK		3
#define GM4_SUPPORT				4
#define GM4_WEAP_LOAD			5

//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (ACTION) and 1 long argument
//*****************************************************************************************
#define GM2L_COMBAT_FLAGS		1
#define GM2L_V_RADARS_DEAD		2
#define GM2L_CHK_R_CROSS_SIG	3
#define GM2L_UPDATE_LVAR2		4
#define GM2L_UPDATE_LVAR3		5

//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (ACTION) and 2 long arguments
//*****************************************************************************************
#define GM3L_TIMERS				1
#define GM3L_VARS				2
#define GM3L_AI_FLAGS			3
#define GM3L_TAKEOFF_TIME		4
#define GM3L_HOOK_WIRE			5
#define GM3L_GIVE_MARSHAL		6
#define GM3L_UPDATE_MARSHAL		7
#define GM3L_BANDIT_CALL		8
#define GM3L_ESCORT_RELEASE		9
#define GM3L_SAR_RESPONSE		10

//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (ACTION) and 3 long arguments
//*****************************************************************************************
#define GM4L_LOG_PLANE_DAMAGE	1
#define GM4L_LOG_PLANE_DESTROY	2
#define GM4L_LOG_VEH_DAMAGE		3
#define GM4L_LOG_VEH_DESTROY	4

//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (Action) an FPoint argument
//*****************************************************************************************
#define GM2FP_WAYPOS	1
#define GM2FP_FORM_POS	2
#define GM2FP_STORES	3
#define GM2FP_VEH_POS	4
#define GM2FP_VEH_S_EXP 5
#define GM2FP_PUT_ON_DECK	6
#define GM2FP_BUDDY_LASER	7
#define	GM2FP_WEATHER_INFO	8

//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (Action) and a Float argument
//*****************************************************************************************
#define GM2F_VEH_DAMAGE 1
#define GM2F_TARGET_Y	2
#define GM2F_GRND_TARG_AT	3
#define GM2F_SHIP_AT	4
#define GM2F_STROBE_AT	5
#define GM2F_CHK_SHADOW	6

//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (Action) and 2 Float arguments
//*****************************************************************************************


//*****************************************************************************************
//*  IDs for generic message with slot ID, 1 BYTE (Action), 1 BYTE (type) and 1 DWORD 
//*			either Ground Object Serial Number or an index number.
//*****************************************************************************************
#define GMOP_GROUND_TARG	1
#define GMOP_PAINTED_BY		2
#define GMOP_FAC_TARG		3
#define GMOP_GROUND_DEAD	4
#define GMOP_GROUND_NO_RADAR	5
#define GMOP_GROUND_NO_WEAPON	6

//*****************************************************************************************
//*  Bit flags for weapons explosion stuff
//*****************************************************************************************
#define GX_DO_BIG_BANG			0x1
#define GX_SKIP_GRAPHIC			0x2
#define GX_FAKE_ACCURACY		0x4
#define GX_FAKE_DAMAGE			0x8


//*****************************************************************************************
//*  Bit flags for MultiSides
//*****************************************************************************************
#define MS_AGGRESIVE_NEUTRALS	0x1
#define MS_IGNORE_FENCES	0x2

//******************************************************************************
//  STUFF MOVED FROM NETWORK.CPP
//******************************************************************************

// DPMSG_GENERIC moved to MultiMsg.h

typedef struct _DPMSG_PAUSED
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#

} DPMSG_PAUSED;

typedef struct _DPMSG_MISSILE
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		target;	// target plane slot#
	int		weapon;	// weapon station
	int		weapIX;	// weapon GUID

} DPMSG_MISSILE;

typedef struct _DPMSG_TARGET
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		target;	// weapon target ID.  If decoy will be decoy GUID
	int		weapIX;	// weapon GUID

} DPMSG_TARGET;

typedef struct _DPMSG_STRIKE
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		target;	// target plane slot#

} DPMSG_STRIKE;

typedef struct _DPMSG_DAMAGE
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		target;	// target plane slot#
	int		system;	// failed_system or itemid
	int		status;	// flight_status or critical

} DPMSG_DAMAGE;

typedef struct _DPMSG_CRASH
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		system;	// failed_system
	int		status;	// flight_status

} DPMSG_CRASH;

typedef struct _DPMSG_DECOY
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		cmtype;	// chaff/flare
	int		weapIX;	// decoy GUID

} DPMSG_DECOY;

typedef struct _DPMSG_EJECT
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		weapIX;	// EJECT GUID

} DPMSG_EJECT;

typedef struct _DPMSG_PLANEDATA		// network plane data packet
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot# [0..N]

	char	Status;			// int		[0..31]		
 	char	Flaps;			// float	[0..30] degrees
	char	Brakes;			// int		[0..1]
	BYTE	LandingGear;	// BYTE		[0..255]

	char	StickX;			// int		[-128..127]
	char	StickY;			// int		[-128..127]
	char	Trigger1;		// int		[0..1]
	char	Trigger2;		// int		[0..1]

	char	GunFireRate;	// int		[1..2]
	char	RudderPedals;	// float	
	char	SpeedBrakeState;// int		[0..1]
	char	Radar;			// 			[-1..8]

	float	ThrottlePos;	// float	[0..127]

	float	InternalFuel;
	float	WingDropFuel;
	float	CenterDropFuel;
	float	AircraftDryWeight;

	float	TotalFuel;
	float	TotalMass;
	float	TotalWeight;

	float	BfLinVelX;
	float	HeightAboveGround;	// remote "LandHeight()"

//	FPoint	WorldPosition;		// X,Y,Z     
	FPointDouble WorldPosition;	// X,Y,Z     

	RefFrame  Orientation;	// aircraft orientation vectors in inertial frame - normalized

//	ANGLE	Roll;
//	ANGLE	Pitch;
//	ANGLE	Heading;

	int		netDelta;
							// *** 160 / 164 as is ***
} DPMSG_PLANEDATA;			// *** 50 bytes (w/o Orientation) ***

typedef struct _DPMSG_CHATTER
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	char	chat[256];

} DPMSG_CHATTER;

void NetGetPause( DPMSG_PAUSED *Net );
void NetGetMissile( DPMSG_MISSILE *Net );
void NetGetTarget( DPMSG_TARGET *Net );
void NetGetStrike( DPMSG_STRIKE *Net );
void NetGetDamage( DPMSG_DAMAGE *Net );
void NetGetCrash( DPMSG_CRASH *Net );
void NetGetDecoy( DPMSG_DECOY *Net );
void NetGetEject( DPMSG_EJECT *Net );
void NetGetPlane( DPMSG_PLANEDATA *Net );
void NetGetChat( DPMSG_CHATTER *Net );
void NetSetKill( int src, int tgt, int noMsg = 0);
void NetLogKill( int tgt );		 // tgt: index of crash'd plane
void NetSetPlane( int ix );
void NetSetDeath( PlaneParams *P );
char NetPutTEWS();
void NetGetTEWS( PlaneParams *P, int radar );

//******************************************************************************
//  END STUFF FROM NETWORK.CPP
//******************************************************************************


//*****************************************************************************************
//*  Bit flags for the lAINetFlags1 variable
//*****************************************************************************************
#define NGAI_ACTIVE 0x1

#define MAX_HUMANS 8

typedef struct _DPMSG_BIG_PACKET_HEAD
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#	CHANGE TO BYTE
	BYTE	num_packets;	//  number of packets in this big packet.
} DPMSG_BIG_PACKET_HEAD;


typedef struct _DPMSG_BIG_PACKET
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#	CHANGE TO BYTE
	BYTE	num_packets;	//  number of packets in this big packet.
	char	smaller_packets[1000];	//  holds the smaller packets that have been grouped together.
} DPMSG_BIG_PACKET;


typedef struct _DPMSG_AI_POS
{
	BYTE	type;		// message ID
	int		time;		// system time stamp (ms)
	BYTE	slot;		// player plane slot#
	int		aislot;		// ai plane slot #
	FPointDouble fpdWorldPosition;  //  ai's position in the world
	ANGLE	Heading;	//  ai's new heading
	ANGLE	Pitch;		//  ai's new pitch
	ANGLE	Roll;		//  ai's new roll
	double	V;			//  ai's velocity in ft/sec
	ANGLE	DesiredPitch;		//  ai's desired pitch
	ANGLE	DesiredRoll;		//  ai's desired roll
	float	DesiredSpeed;		//  ai's desired speed in ft/sec
	long	lFlags1;	//  Flags for things like flags, gear, speedbrake, radar "ping", etc;
	int		iAirTarget; //  holds the index number for who the plane is currently targeting.
	int		iAirThreat; //  holds the index number for the plane which is the biggest threat.
	int		iwaypoint;	//  holds current waypoint.

} DPMSG_AI_POS;

typedef struct _DPMSG_MISSILE_GENERAL
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		target;	// target plane slot#
	int		weapon;	// weapon station
	int		weapIX;	// weapon GUID
	DWORD	dwGrndSerialNum;  //  serial number of ground launcher
	int		sitetype;  //  identifies what type of thing launcher is.

} DPMSG_MISSILE_GENERAL;

typedef struct _DPMSG_GROUND_LOCK
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	DWORD	dwWeaponSerialNum;  //  serial number of ground launcher
	DWORD	dwRadarSerialNum;  //  serial number of ground launcher
	int		target;	// target plane slot#
} DPMSG_GROUND_LOCK;

typedef struct _DPMSG_BOMB_GENERAL
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		weapon;	// weapon station
	int		weapIX;	// weapon GUID
	double	burst_height;  //  burst height for clust weapons.
	DWORD	dwGrndSerialNum;  //  serial number of ground target
	int		groundtype;  //  type of ground object (vehicle, object)
	double	GX;		//  impact x location;
	double	GY;		//  impact y location;
	double	GZ;		//  impact z location;

} DPMSG_BOMB_GENERAL;

typedef struct _DPMSG_AG_MISSILE_GENERAL
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	int		weapon;	// weapon station
	int		weapIX;	// weapon GUID
	DWORD	dwGrndSerialNum;  //  serial number of ground target
	int		groundtype;  //  type of ground object (vehicle, object)
	FPoint	targetloc;
	BYTE	profile;
} DPMSG_AG_MISSILE_GENERAL;

typedef struct _NET_CONTROL_LOC
{
	__int64	active_areas;
} NET_CONTROL_LOC;	//  8 bytes

typedef struct _DPMSG_CONTROL_ZONES
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	NET_CONTROL_LOC		zones;
} DPMSG_CONTROL_ZONES;	//  14 bytes

typedef struct _DPMSG_AI_COMMS
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	BYTE	commid;	// Number identifying what comm function to use
	int keyflag;	// Key flags for comm keys
	WPARAM wParam;	// What key was hit  32 bits
} DPMSG_AI_COMMS;	//  15 bytes

typedef struct _DPMSG_DESIRED_HEADING
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	ANGLE	new_heading;  //  new heading in ANGLE
} DPMSG_DESIRED_HEADING;	//  8 bytes.

typedef struct _DPMSG_CAS_DATA
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	BYTE	attacktype;  //  type of attack 1)popup 2)flyaway 3)SEAD Winchester
	DWORD	dwGrndSerialNum;  //  serial number of ground target
	BYTE	groundtype;  //  type of ground object (vehicle, object)
	BYTE	weapon;	//  Holds weapon station number
	BYTE	numrel;  //  Holds number of weapons to release.
} DPMSG_CAS_DATA;

typedef struct _DPMSG_GENERIC_1
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
} DPMSG_GENERIC_1;	//  7 bytes.

typedef struct _DPMSG_GENERIC_2
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	arg2;  //  generic argument.
} DPMSG_GENERIC_2;	//  8 bytes.

typedef struct _DPMSG_GENERIC_3
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	arg2;  //  generic argument.
	BYTE	arg3;  //  generic argument.
} DPMSG_GENERIC_3;	//  9 bytes.

typedef struct _DPMSG_GENERIC_4
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	arg2;  //  generic argument.
	BYTE	arg3;  //  generic argument.
	BYTE	arg4;  //  generic argument.
} DPMSG_GENERIC_4;	//  10 bytes.

typedef struct _DPMSG_DOGFIGHT_UPDATE
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	behavior;	//  ID of combat behavior
	long	lTimer;		//  how long behavior should go.
} DPMSG_DOGFIGHT_UPDATE;	//  11 bytes.

typedef struct _DPMSG_DOGFIGHT_UPDATE_CPOS
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	behavior;	//  ID of combat behavior
	long	lTimer;		//  how long behavior should go.
	FPoint	CombatPos;	//  Combat Position
} DPMSG_DOGFIGHT_UPDATE_CPOS;	//  23 bytes.

typedef struct _DPMSG_MISSILE_BREAK
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	BYTE	breakid;  //  Added info about the break;
	int		weapIX;	// weapon GUID
	ANGLE	new_heading;  //  new heading in ANGLE
} DPMSG_MISSILE_BREAK;	//  13 bytes.

typedef struct _DPMSG_WEAPON_G_EXPLODE
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	BYTE	launcher;  // who shot it;
	BYTE	wtype;	//  what is the weapon
	FPoint  fpWorldPosition;  //  weapon's position in the world
	BYTE	bFlags;	//  Right now for weapons level.
	BYTE	randseed;  //  Used main for matching cluster weapon damage.
	short timer;  //  Used when determining radius
} DPMSG_WEAPON_G_EXPLODE;	//  21 bytes.

typedef struct _DPMSG_GENERIC_2_LONG
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	long	arg2;  //  generic argument.
} DPMSG_GENERIC_2_LONG;	//  11 bytes.

typedef struct _DPMSG_GENERIC_3_LONG
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	long	arg2;  //  generic argument.
	long	arg3;  //  generic argument.
} DPMSG_GENERIC_3_LONG;	//  15 bytes.

typedef struct _DPMSG_GENERIC_4_LONG
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	long	arg2;  //  generic argument.
	long	arg3;  //  generic argument.
	long	arg4;  //  generic argument.
} DPMSG_GENERIC_4_LONG;	//  19 bytes.

typedef struct _DPMSG_GENERIC_2_FPOINT
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	FPoint	arg2;  //  generic argument.
} DPMSG_GENERIC_2_FPOINT;	//  19 bytes.

typedef struct _DPMSG_GENERIC_OBJ_POINT
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	objtype;  //  type of ground object (vehicle, object)
	DWORD	dwObjSerialNum;  //  serial number of ground target or index into a list
} DPMSG_GENERIC_OBJ_POINT;	//  12 bytes.

typedef struct _DPMSG_VGROUND_LOCK
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// player plane slot#
	BYTE	VNum;  //  Vehicle Number doing lock
	BYTE	WRDNum;  //  RadarWeaponData index num (only really needed for ships).
	BYTE	RDNum;  //  RadarWeaponData index num (only really needed for ships).
	int		target;	// target plane slot#
} DPMSG_VGROUND_LOCK;

typedef struct _DPMSG_GENERIC_2_FLOAT
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	float	arg2;  //  generic argument.
} DPMSG_GENERIC_2_FLOAT;	//  11 bytes.

typedef struct _DPMSG_GENERIC_3_FLOAT
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	float	arg2;  //  generic argument.
	float	arg3;  //  generic argument.
} DPMSG_GENERIC_3_FLOAT;	//  15 bytes.

typedef struct _DPMSG_WEAPON_HACK_CHECK
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
	BYTE	slot;	// slot/planeid
	BYTE	weapid;	// id num of weapon (index into weapondb
	BYTE	weapflags;	//  0xF seeker type, 0x30 rocket motor
	BYTE	ecmresist;	//  ECM resistance flags
	int		damageval;	//  damagevalue
	int		damagerad;	//  damage radius
	float	maxspeed;	//  max speed
	int		burntime;	//  burn time
	int		maxalt;		//  max altitude
	int		maxrange;	//  max range
	BYTE	maxyawrate;	//  max yaw rate
	int		acceleration;	//  acceleration
	BYTE	seekerfov;	//  seeker fov
	float	MinRange;	//  Min Range
} DPMSG_WEAPON_HACK_CHECK;	//  43 bytes.

//**********************************************************************************
//  Condensed headers for big packets.
//**********************************************************************************
typedef struct _DPMSG_MISSILE_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	target;	// target plane slot#
	char	weapon;	// weapon station
	int		weapIX;	// weapon GUID

} DPMSG_MISSILE_SM;		//  8 bytes

typedef struct _DPMSG_TARGET_SM
{
	BYTE	type;	// message ID
	int		target;	// weapon target ID, if decoy will be decoy GUID
	int 	weapIX;	// weapon GUID

} DPMSG_TARGET_SM;	//  9 bytes

typedef struct _DPMSG_STRIKE_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	target;	// target plane slot#

} DPMSG_STRIKE_SM;	//  3 bytes.

typedef struct _DPMSG_DAMAGE_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	target;	// target plane slot#
	char	system;	// failed_system or itemid
	char	status;	// flight_status or critical

} DPMSG_DAMAGE_SM;	//  5 bytes

typedef struct _DPMSG_CRASH_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	int		system;	// failed_system
	int		status;	// flight_status

} DPMSG_CRASH_SM;	//  10 bytes

typedef struct _DPMSG_DECOY_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	char	cmtype;	// chaff/flare
	int		weapIX;	// decoy GUID

} DPMSG_DECOY_SM;	//  7 bytes.

typedef struct _DPMSG_EJECT_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	int		weapIX;	// EJECT GUID

} DPMSG_EJECT_SM;	//  6 bytes

typedef struct _DPMSG_PLANEDATA_SM		// network plane data packet
{
	BYTE	type;	// message ID
	int		time;	// system time stamp (ms)
//	BYTE	slot;	// player plane slot# [0..N]

	char	Status;			// int		[0..31]		
 	char	Flaps;			// float	[0..30] degrees
	ANGLE	BitFlags;		// Brakes[0,1], Trigger1[0,1], Trigger2[0,1], GunFireRate[1,2],
							// SpeedBrakeState[0,1], OnGround[0,1], Jammer[0,1]

	BYTE	LandingGear;	// BYTE		[0..255]
	char	StickX;			// int		[-128..127]
	char	StickY;			// int		[-128..127]

	char	RudderPedals;	// float	
	char	Radar;			// 			[-1..8]

	float	ThrottlePos;	// float	[0..127]

	float	TotalWeight;

	float	BfLinVelX;
//	float	HeightAboveGround;	// remote "LandHeight()"  //  Think only needed for OnGround

	FPoint	WorldPosition;		// X,Y,Z     

	ANGLE	Roll;
	ANGLE	Pitch;
	ANGLE	Heading;
	float	SymetricalElevator;

	int		netDelta;
							// *** 160 / 164 as is ***
} DPMSG_PLANEDATA_SM;			// *** 48 bytes (w/o Orientation) ***

typedef struct _DPMSG_CHATTER_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	chatlen;  //  number of characters sent.
	char	chat[256];

} DPMSG_CHATTER_SM;  //  259 bytes (variable)

typedef struct _DPMSG_AI_POS_SM
{
	BYTE	type;		// message ID
	int		time;		// system time stamp (ms)
	BYTE	aislot;		// ai plane slot #
	FPoint  fpWorldPosition;  //  ai's position in the world
	ANGLE	Heading;	//  ai's new heading
	ANGLE	Pitch;		//  ai's new pitch
	ANGLE	Roll;		//  ai's new roll
	double	V;			//  ai's velocity in ft/sec
	ANGLE	DesiredPitch;		//  ai's desired pitch
	ANGLE	DesiredRoll;		//  ai's desired roll
	float	DesiredSpeed;		//  ai's desired speed in ft/sec
	long	lFlags1;	//  Flags for things like flags, gear, speedbrake, radar "ping", etc;
	BYTE	iAirTarget; //  holds the index number for who the plane is currently targeting.
	BYTE	iAirThreat; //  holds the index number for the plane which is the biggest threat.
	BYTE	iwaypoint;	//  holds current waypoint.

} DPMSG_AI_POS_SM;		//  47 bytes.

typedef struct _DPMSG_AI_POS_LITE_SM
{
	BYTE	type;		// message ID
	int		time;		// system time stamp (ms)
	BYTE	aislot;		// ai plane slot #
	FPoint  fpWorldPosition;  //  ai's position in the world
	long	lFlags1;	//  Flags for things like flags, gear, speedbrake, radar "ping", etc;
	BYTE	iAirTarget; //  holds the index number for who the plane is currently targeting.
	BYTE	iAirThreat; //  holds the index number for who the plane is currently targeting.
	BYTE	iwaypoint;	//  holds current waypoint.

} DPMSG_AI_POS_LITE_SM;	//  25 bytes.

typedef struct _DPMSG_MISSILE_GENERAL_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	target;	// target plane slot#
	char	weapon;	// weapon station
	int		weapIX;	// weapon GUID
	DWORD	dwGrndSerialNum;  //  serial number of ground launcher
	BYTE	sitetype;  //  identifies what type of thing launcher is.

} DPMSG_MISSILE_GENERAL_SM;	//  13 bytes

typedef struct _DPMSG_GROUND_LOCK_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	DWORD	dwWeaponSerialNum;  //  serial number of ground launcher
	DWORD	dwRadarSerialNum;  //  serial number of ground launcher
	BYTE	target;	// target plane slot#
} DPMSG_GROUND_LOCK_SM;	//  11 bytes

typedef struct _DPMSG_BOMB_GENERAL_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	char	weapon;	// weapon station
	int		weapIX;	// weapon GUID
	short	burst_height;  //  burst height for clust weapons.
	DWORD	dwGrndSerialNum;  //  serial number of ground target
	char	groundtype;  //  type of ground object (vehicle, object)
	float	GX;		//  impact x location;
	float	GY;		//  impact y location;
	float	GZ;		//  impact z location;

} DPMSG_BOMB_GENERAL_SM;	//  26 bytes.

typedef struct _DPMSG_AG_MISSILE_GENERAL_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	char	weapon;	// weapon station
	int		weapIX;	// weapon GUID
	DWORD	dwGrndSerialNum;  //  serial number of ground target
	char	groundtype;  //  type of ground object (vehicle, object)
	FPoint	targetloc;
	BYTE	profile;
} DPMSG_AG_MISSILE_GENERAL_SM;	//  24 bytes.

typedef struct _DPMSG_CONTROL_ZONES_SM
{
	BYTE	type;	// message ID
	NET_CONTROL_LOC		zones;
} DPMSG_CONTROL_ZONES_SM;	//  9 bytes.

typedef struct _DPMSG_AI_COMMS_SM
{
	BYTE	type;	// message ID
	BYTE	commid;	// Number identifying what comm function to use
	int keyflag;	// Key flags for comm keys
	WPARAM wParam;	// What key was hit  32 bits
} DPMSG_AI_COMMS_SM;	//  10 bytes

typedef struct _DPMSG_DESIRED_HEADING_SM
{
	BYTE	type;	// message ID
	BYTE	slot;  //  place in plane array
	ANGLE	new_heading;	//  new heading in ANGLE
} DPMSG_DESIRED_HEADING_SM;	//  3 bytes.

typedef struct _DPMSG_CAS_DATA_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	attacktype;  //  type of attack 1)popup 2)flyaway 3)SEAD Winchester
	DWORD	dwGrndSerialNum;  //  serial number of ground target
	BYTE	groundtype;  //  type of ground object (vehicle, object)
	BYTE	weapon;	//  Holds weapon station number
	BYTE	numrel;  //  Holds number of weapons to release.
} DPMSG_CAS_DATA_SM;

typedef struct _DPMSG_GENERIC_1_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
} DPMSG_GENERIC_1_SM;	//  3 bytes.

typedef struct _DPMSG_GENERIC_2_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	arg2;  //  generic argument.
} DPMSG_GENERIC_2_SM;	//  4 bytes.

typedef struct _DPMSG_GENERIC_3_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	arg2;  //  generic argument.
	BYTE	arg3;  //  generic argument.
} DPMSG_GENERIC_3_SM;	//  5 bytes.

typedef struct _DPMSG_GENERIC_4_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	arg2;  //  generic argument.
	BYTE	arg3;  //  generic argument.
	BYTE	arg4;  //  generic argument.
} DPMSG_GENERIC_4_SM;	//  6 bytes.

typedef struct _DPMSG_DOGFIGHT_UPDATE_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	behavior;	//  ID of combat behavior
	long	lTimer;		//  how long behavior should go.
} DPMSG_DOGFIGHT_UPDATE_SM;	//  7 bytes.

typedef struct _DPMSG_DOGFIGHT_UPDATE_CPOS_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	behavior;	//  ID of combat behavior
	long	lTimer;		//  how long behavior should go.
	FPoint	CombatPos;	//  Combat Position
} DPMSG_DOGFIGHT_UPDATE_CPOS_SM;	//  19 bytes.

typedef struct _DPMSG_MISSILE_BREAK_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	breakid;  //  Added info about the break;
	int		weapIX;	// weapon GUID
	ANGLE	new_heading;  //  new heading in ANGLE
} DPMSG_MISSILE_BREAK_SM;	//  9 bytes.

typedef struct _DPMSG_WEAPON_G_EXPLODE_SM
{
	BYTE	type;	// message ID
	BYTE	launcher;  // who shot it;
	BYTE	wtype;	//  what is the weapon
	FPoint  fpWorldPosition;  //  weapon's position in the world
	BYTE	bFlags;	//  Right now for weapons level.
	BYTE	randseed;  //  Used main for matching cluster weapon damage.
	short timer;  //  Used when determining radius
} DPMSG_WEAPON_G_EXPLODE_SM;	//  16 bytes.

typedef struct _DPMSG_GENERIC_2_LONG_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	long	arg2;  //  generic argument.
} DPMSG_GENERIC_2_LONG_SM;	//  7 bytes.

typedef struct _DPMSG_GENERIC_3_LONG_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	long	arg2;  //  generic argument.
	long	arg3;  //  generic argument.
} DPMSG_GENERIC_3_LONG_SM;	//  11 bytes.

typedef struct _DPMSG_GENERIC_4_LONG_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	long	arg2;  //  generic argument.
	long	arg3;  //  generic argument.
	long	arg4;  //  generic argument.
} DPMSG_GENERIC_4_LONG_SM;	//  15 bytes.

typedef struct _DPMSG_GENERIC_2_FPOINT_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	FPoint	arg2;  //  generic argument.
} DPMSG_GENERIC_2_FPOINT_SM;	//  15 bytes.

typedef struct _DPMSG_GENERIC_OBJ_POINT_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	BYTE	objtype;  //  type of ground object (vehicle, object)
	DWORD	dwObjSerialNum;  //  serial number of ground target or index into a list
} DPMSG_GENERIC_OBJ_POINT_SM;	//  8 bytes.

typedef struct _DPMSG_VGROUND_LOCK_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// player plane slot#
	BYTE	VNum;  //  Vehicle Number doing lock
	BYTE	WRDNum;  //  RadarWeaponData index num (only really needed for ships).
	BYTE	RDNum;  //  RadarWeaponData index num (only really needed for ships).
	BYTE	target;	// target plane slot#
} DPMSG_VGROUND_LOCK_SM;	//  6 bytes

typedef struct _DPMSG_GENERIC_2_FLOAT_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	float	arg2;  //  generic argument.
} DPMSG_GENERIC_2_FLOAT_SM;	//  7 bytes.

typedef struct _DPMSG_GENERIC_3_FLOAT_SM
{
	BYTE	type;	// message ID
	BYTE	slot;	// slot/planeid
	BYTE	arg1;  //  generic argument.
	float	arg2;  //  generic argument.
	float	arg3;  //  generic argument.
} DPMSG_GENERIC_3_FLOAT_SM;	//  15 bytes.

typedef struct _DPMSG_WEAPON_HACK_CHECK_SM
{
	BYTE	type;	// message ID
	BYTE	weapid;	// id num of weapon (index into weapondb
	BYTE	weapflags;	//  0xF seeker type, 0x30 rocket motor
	BYTE	ecmresist;	//  ECM resistance flags
	int		damageval;	//  damagevalue
	int		damagerad;	//  damage radius
	float	maxspeed;	//  max speed
	int		burntime;	//  burn time
	int		maxalt;		//  max altitude
	int		maxrange;	//  max range
	BYTE	maxyawrate;	//  max yaw rate
	int		acceleration;	//  acceleration
	BYTE	seekerfov;	//  seeker fov
	float	MinRange;	//  Min Range
} DPMSG_WEAPON_HACK_CHECK_SM;	//  38 bytes.

//**********************************************************************************
//  Other typedefs
//**********************************************************************************
typedef struct _BUDDY_LASING_DATA
{
	FPoint	laserloc;	// Where laser is pointing in world
	long	lasertimer;	// Generic timer.  Mainly used by MySlot
} BUDDY_LASING_DATA;

//**********************************************************************************
//  MultiPlayerGlobals
//**********************************************************************************
#ifdef MULTI_AI_SOURCE
DPMSG_AI_POS  NetAIPos;	// static packet for sending
DPMSG_MISSILE_GENERAL  NetMissileGeneral;	// static packet for sending
DPMSG_GROUND_LOCK  NetGroundLock;	// static packet for sending
DPMSG_BOMB_GENERAL  NetBombGeneral;	// static packet for sending
DPMSG_AG_MISSILE_GENERAL  NetAGMissileGeneral;	// static packet for sending
long	lLastMsgTime[MAX_HUMANS];
long	lAINetFlags1;
int		iMultiCast = 0;
DPMSG_BIG_PACKET	BigPackets[MAX_HUMANS];
DWORD	dwLastBPSent[MAX_HUMANS];
int		iStartNextPacket[MAX_HUMANS];
int		iBigPacketSize[MAX_HUMANS];
int		iMaxPacketSize = 500;
HWND hWrap = NULL;	// post this window for all DP_SYS messages  MOVED FROM NETWORK.CPP
DPMSG_BIG_PACKET	NetBigPacket;
int netCall;	// anti-recursion semaphore  Dave's
int cSend;		// Network messages sent  Dave's
DWORD	dwMaxPacketDelay = 200;
NET_CONTROL_LOC	tHotZones[MAX_HUMANS];
int	iHotZoneMaxX = 7;
int iHotZoneMaxY = 7;
double dHotZoneDivideX = 2000;  // BS here
double dHotZoneDivideY = 2000;
NET_CONTROL_LOC	tGroundDefHotZones;
NET_CONTROL_LOC	tLastHotZonesSent;
int iHotZoneCheckFlags = -1;
int iPacketFrom;
long	lLastZonesSent = 0;
int iAICommFrom = -1;
long lCurrBytes[4];
long lByteTimer[4];
long lMaxBytes;
long lTotalBytes;
int iSlotToPlane[MAX_HUMANS];
int iWeaponFiredBy = -1;
unsigned int	iClusterRandSeed;
short sClusterTimer;
int iMultiSides;
int hNetDebugFile = -1;
long lTotalSecs = 0;
DPMSG_PLANEDATA_SM *pLastBPPos[MAX_HUMANS];  //  pointer to plane pos packet in big packet.  NULL if not in current packet.
int hNetPacketFile = -1;
long lLastBytes[4];
int	iMAISendTo = -1;
long lArenaUpdateTimers[MAX_HUMANS];
PlaneParams *pArenaUpdatePlane[MAX_HUMANS];
int iMissionSlotToPlane[MAX_HUMANS];			//  iShotToPlane should be the same as this but I'm going to keep this as a backup.
BUDDY_LASING_DATA	NetBuddyLasingData[MAX_HUMANS];
PlaneParams			NetRegenPlanes[MAX_HUMANS];
PlaneParams		*pPlaneLoadUpdate[MAX_HUMANS];
int				iLoadUpdateStation[MAX_HUMANS];
int				iLoadUpdateStatus[MAX_HUMANS];
long			lLoadUpdateTimer[MAX_HUMANS];
int	iLeadPlace = 0;
int	iFirstPlace = 0;
int iLastPlace = 7;
int iAllowRegen = 0;
double	dLastSlotV[MAX_HUMANS];
int	iNetHasWeather = 0;
long	lHostLeftTimer = -1;
BYTE bMultiLabels = 0;
#else
extern DPMSG_AI_POS  NetAIPos;	// static packet for sending
extern DPMSG_MISSILE_GENERAL  NetMissileGeneral;	// static packet for sending
extern DPMSG_GROUND_LOCK  NetGroundLock;	// static packet for sending
extern DPMSG_BOMB_GENERAL  NetBombGeneral;	// static packet for sending
extern DPMSG_AG_MISSILE_GENERAL  NetAGMissileGeneral;	// static packet for sending
extern	long	lLastMsgTime[MAX_HUMANS];
extern	long	lAINetFlags1;
extern  int		iMultiCast;
extern DPMSG_BIG_PACKET	BigPackets[MAX_HUMANS];
extern DWORD	dwLastBPSent[MAX_HUMANS];
extern int		iStartNextPacket[MAX_HUMANS];
extern int		iBigPacketSize[MAX_HUMANS];
extern int		iMaxPacketSize;
extern DPMSG_BIG_PACKET	NetBigPacket;
extern HWND hWrap;	// post this window for all DP_SYS messages  MOVED FROM NETWORK.CPP
extern int netCall;	// anti-recursion semaphore
extern int cSend;		// Network messages sent
extern DWORD	dwMaxPacketDelay;
extern NET_CONTROL_LOC	tHotZones[MAX_HUMANS];
extern int iHotZoneMaxX;
extern int iHotZoneMaxY;
extern double dHotZoneDivideX;
extern double dHotZoneDivideY;
extern NET_CONTROL_LOC	tGroundDefHotZones;
extern int iHotZoneCheckFlags;
extern NET_CONTROL_LOC	tLastHotZonesSent;
extern int iPacketFrom;
extern long	lLastZonesSent;
extern int iAICommFrom;
extern long lCurrBytes[4];
extern long lByteTimer[4];
extern long lMaxBytes;
extern long lTotalBytes;
extern int iSlotToPlane[MAX_HUMANS];
extern int iWeaponFiredBy;
extern unsigned int	iClusterRandSeed;
extern short sClusterTimer;
extern int iMultiSides;
extern DPMSG_PLANEDATA_SM *pLastBPPos[MAX_HUMANS];
extern int hNetDebugFile;
extern int hNetPacketFile;
extern long lTotalSecs;
extern long lLastBytes[4];
extern int	iMAISendTo;
extern long lArenaUpdateTimers[MAX_HUMANS];
extern PlaneParams *pArenaUpdatePlane[MAX_HUMANS];
extern int iMissionSlotToPlane[MAX_HUMANS];
extern BUDDY_LASING_DATA	NetBuddyLasingData[MAX_HUMANS];
extern PlaneParams 		NetRegenPlanes[MAX_HUMANS];
extern PlaneParams		*pPlaneLoadUpdate[MAX_HUMANS];
extern int				iLoadUpdateStation[MAX_HUMANS];
extern int				iLoadUpdateStatus[MAX_HUMANS];
extern long				lLoadUpdateTimer[MAX_HUMANS];
extern int	iLeadPlace;
extern int	iFirstPlace;
extern int iLastPlace;
extern double	dLastSlotV[MAX_HUMANS];
extern int iAllowRegen;
extern int iNetHasWeather;
extern BYTE bMultiLabels;
extern long	lHostLeftTimer;
#endif

void NetPutAIPos(PlaneParams *P);
void NetGetAIPos( DPMSG_AI_POS *Net );
void NetSetAIPlanes(int ix);
void NetReconnectAIPlanes(int ix);
void MAIFireStraightGun(PlaneParams *P);
int NetPutMissileGeneral( PlaneParams *P, int Station, void *GroundLauncher, int sitetype, PlaneParams *pTarget);
void NetGetMissileGeneral( DPMSG_MISSILE_GENERAL *Net );
int MAIIsActiveWeapon(WeaponParams *W, int checkplanes = 1, int checkground = 1);
void NetPutGroundLock(BasicInstance *foundweapon, BasicInstance *radarsite, PlaneParams *planepnt);
void NetGetGroundLock( DPMSG_GROUND_LOCK *Net );
int NetPutBombGeneral( PlaneParams *P, int Station , double burst_height, void *GroundTarget, int groundtype, double GX, double GY, double GZ);
void NetGetBombGeneral( DPMSG_BOMB_GENERAL *Net );
int NetPutAGMissileGeneral(PlaneParams *P, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile = 0);
void NetGetAGMissileGeneral( DPMSG_AG_MISSILE_GENERAL *Net );
void NetAddToBigPacket(int packet_num, void *lpMsg, int size);
void NetGetBigPacket(DPMSG_BIG_PACKET	*Net);

int NetPutMissileSmall( PlaneParams *P, int station, PlaneParams *pTarget );
void NetGetMissileSmall( DPMSG_MISSILE_SM *Net );
void NetPutTargetSmall( int weapIX, int target  );
void NetGetTargetSmall( DPMSG_TARGET_SM *Net );
void NetPutStrikeSmall( PlaneParams *P );
void NetGetStrikeSmall( DPMSG_STRIKE_SM *Net );
void NetPutDamageSmall( PlaneParams *P, long itemid, int critical );
void NetGetDamageSmall( DPMSG_DAMAGE_SM *Net );
int NetPutCrashSmall( PlaneParams *P, int flight_status, int failed_system );
void NetGetCrashSmall( DPMSG_CRASH_SM *Net );
int NetPutDecoySmall( PlaneParams *P, int cmtype );
void NetGetDecoySmall( DPMSG_DECOY_SM *Net );
int NetPutEjectSmall( PlaneParams *P );
void NetGetEjectSmall( DPMSG_EJECT_SM *Net );
void NetPutPlaneSmall();
void NetGetPlaneSmall(int slotnum, DPMSG_PLANEDATA_SM *Net );
void NetPutChatSmall( int dpid, char *chat );
void NetGetChatSmall( DPMSG_CHATTER_SM *Net );
void NetPutAIPosSmall(PlaneParams *P);
void NetGetAIPosSmall(int fromslot, DPMSG_AI_POS_SM *Net);
int NetPutMissileGeneralSmall( PlaneParams *P, int Station, void *GroundLauncher, int sitetype, PlaneParams *pTarget);
void NetGetMissileGeneralSmall( DPMSG_MISSILE_GENERAL_SM *Net );
void NetPutGroundLockSmall(BasicInstance *foundweapon, BasicInstance *radarsite, PlaneParams *planepnt);
void NetGetGroundLockSmall( DPMSG_GROUND_LOCK_SM *Net );
int NetPutBombGeneralSmall( PlaneParams *P, int Station , double burst_height, void *GroundTarget, int groundtype, double GX, double GY, double GZ);
void NetGetBombGeneralSmall( DPMSG_BOMB_GENERAL_SM *Net );
int NetPutAGMissileGeneralSmall(PlaneParams *P, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile = 0);
void NetGetAGMissileGeneralSmall( DPMSG_AG_MISSILE_GENERAL_SM *Net );
void NoMultiCastBigPackets(void *lpMsg, int size);
void MAIGetHotZoneIndex(float X, float Y, int *OutX, int *OutY);
int MAICheckHotZones(int X, int Y);
int MAICheckAZone(int X, int Y, int slot);
int MAILimitAIPosSend(DPMSG_AI_POS_SM *Net);
int MAILimitMissileSend(DPMSG_MISSILE_SM *Net);
int MAILimitTargetSend(DPMSG_TARGET_SM *Net);
int MAILimitDecoySend(DPMSG_DECOY_SM *Net);
int MAILimitEjectSend(DPMSG_EJECT_SM *Net);
int MAILimitMissileGeneralSend(DPMSG_MISSILE_GENERAL_SM *Net);
int MAILimitGroundLockSend(DPMSG_GROUND_LOCK_SM *Net);
int MAILimitBombGeneralSend(DPMSG_BOMB_GENERAL_SM *Net);
int MAILimitAGMissileGeneralSend(DPMSG_AG_MISSILE_GENERAL_SM *Net);
WeaponParams *MAIGetWeaponPointer(int netid);
void MAISetAHotZone(float X, float Y, int grndstatic = 0, float radius = 75.0f);
void NetPutZones(int slotnumber = -1);
void NetGetZones( DPMSG_CONTROL_ZONES *Net );
void NetPutZonesSmall(int slotnumber = -1);
void NetGetZonesSmall(int sendfrom, DPMSG_CONTROL_ZONES_SM *Net );
void NetCheckZones();
void NetPutAIComm(int keyflag, WPARAM wParam);
void NetGetAIComms( DPMSG_AI_COMMS *Net );
void NetPutAICommSmall(int keyflag, WPARAM wParam);
void NetGetAICommsSmall(int fromslot, DPMSG_AI_COMMS_SM *Net );
void MAISetToNewWaypoint(PlaneParams *planepnt, int waypoint);
void NetPutDesiredHeading(int planenum, ANGLE newheading);
void NetGetDesiredHeading( DPMSG_DESIRED_HEADING *Net );
void NetPutDesiredHeadingSmall(int planenum, ANGLE newheading);
void NetGetDesiredHeadingSmall( DPMSG_DESIRED_HEADING_SM *Net );
void NetPutCASData(PlaneParams *planepnt, int attacktype);
void NetGetCASData( DPMSG_CAS_DATA *Net);
void NetPutCASDataSmall(PlaneParams *planepnt, int attacktype);
void NetGetCASDataSmall( DPMSG_CAS_DATA_SM *Net);
void NetPutGenericMessage1(PlaneParams *planepnt, BYTE arg1);
void NetGetGenericMessage1( DPMSG_GENERIC_1 *Net);
void NetPutGenericMessage1Small(PlaneParams *planepnt, BYTE arg1);
void NetGetGenericMessage1Small( DPMSG_GENERIC_1_SM *Net );
void NetPutGenericMessage2(PlaneParams *planepnt, BYTE arg1, BYTE arg2);
void NetGetGenericMessage2( DPMSG_GENERIC_2 *Net);
void NetPutGenericMessage2Small(PlaneParams *planepnt, BYTE arg1, BYTE arg2);
void NetGetGenericMessage2Small( DPMSG_GENERIC_2_SM *Net );
void NetPutGenericMessage3(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3);
void NetGetGenericMessage3( DPMSG_GENERIC_3 *Net);
void NetPutGenericMessage3Small(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3);
void NetGetGenericMessage3Small( DPMSG_GENERIC_3_SM *Net );
void NetPutGenericMessage4(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3, BYTE arg4);
void NetGetGenericMessage4( DPMSG_GENERIC_4 *Net);
void NetPutGenericMessage4Small(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3, BYTE arg4);
void NetGetGenericMessage4Small( DPMSG_GENERIC_4_SM *Net );
void NetVectorIntercept(BYTE planenum, BYTE targetnum);
void NetStartAttack(PlaneParams *planepnt, int attacktype, PlaneParams *target, PlaneParams *threat);
void NetAdjustCAPLegs(PlaneParams *planepnt, BYTE capleg);
void NetPutDogfightUpdate(PlaneParams *planepnt, BYTE behaveid, long ltimer);
void NetGetDogfightUpdate( DPMSG_DOGFIGHT_UPDATE *Net);
void NetPutDogfightUpdateSmall(PlaneParams *planepnt, BYTE behaveid, long ltimer);
void NetGetDogfightUpdateSmall( DPMSG_DOGFIGHT_UPDATE_SM *Net );
void NetPutDogfightCPosUpdate(PlaneParams *planepnt, BYTE behaveid, long ltimer, FPoint cpos);
void NetGetDogfightCPosUpdate( DPMSG_DOGFIGHT_UPDATE_CPOS *Net);
void NetPutDogfightCPosUpdateSmall(PlaneParams *planepnt, BYTE behaveid, long ltimer, FPoint cpos);
void NetGetDogfightCPosUpdateSmall( DPMSG_DOGFIGHT_UPDATE_CPOS_SM *Net );
void NetUpdateCombatBehavior(PlaneParams *planepnt, BYTE behaviorid);
void NetUpdateSupport(PlaneParams *planepnt, int support, PlaneParams *wingman = NULL);
void NetPutMissileBreak(PlaneParams *planepnt, BYTE breakid, WeaponParams *weaponpnt);
void NetGetMissileBreak( DPMSG_MISSILE_BREAK *Net);
void NetPutMissileBreakSmall(PlaneParams *planepnt, BYTE breakid, WeaponParams *weaponpnt);
void NetGetMissileBreakSmall( DPMSG_MISSILE_BREAK_SM *Net);
void NetUpdateTarget(DPMSG_GENERIC_2_SM *Net);
PlaneParams *GeneralSetNewAirTarget(PlaneParams *planepnt, PlaneParams *target);
PlaneParams *GeneralSetNewAirThreat(PlaneParams *planepnt, PlaneParams *threat);
void MAICheckFlags(PlaneParams *planepnt);
void GeneralClearCombatBehavior(PlaneParams *planepnt);
int NetGetSlotFromPlaneIndex(int planenum, int showerr = 0);
int NetGetPlaneIndexFromSlot(int slotnum, int showerr = 0);
void NetPutWeaponGExplode(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type,BOOL skip_graphic);
void NetGetWeaponGExplode( DPMSG_WEAPON_G_EXPLODE *Net);
void NetPutWeaponGExplodeSmall(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type,BOOL skip_graphic);
void NetGetWeaponGExplodeSmall( DPMSG_WEAPON_G_EXPLODE_SM *Net );
void MAINetDebugMessage(DPMSG_F18GENERIC *lpMsg, int dpID, int size);
void MAINetDebugBigPacket(DPMSG_BIG_PACKET *Net );
int MAIGetIDForBehaviorfunc(void *behaviorfunc);
void MAIUpdateBehavior(int behaviornum, PlaneParams *planepnt, int updatebehavior = 0);
void NetPutFullAIUpdate(PlaneParams *planepnt);
void NetPutGenericMessage2Long(PlaneParams *planepnt, BYTE arg1, long arg2, BYTE slotnum = 0);
void NetGetGenericMessage2Long( DPMSG_GENERIC_2_LONG *Net);
void NetPutGenericMessage2LongSmall(PlaneParams *planepnt, BYTE arg1, long arg2, BYTE slotnum = 0);
void NetGetGenericMessage2LongSmall( DPMSG_GENERIC_2_LONG_SM *Net );
void NetPutGenericMessage3Long(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, BYTE slotnum = 0);
void NetGetGenericMessage3Long( DPMSG_GENERIC_3_LONG *Net);
void NetPutGenericMessage3LongSmall(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, BYTE slotnum = 0);
void NetGetGenericMessage3LongSmall( DPMSG_GENERIC_3_LONG_SM *Net );
void NetPutGenericMessage4Long(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, long arg4, BYTE slotnum = 0);
void NetGetGenericMessage4Long( DPMSG_GENERIC_4_LONG *Net);
void NetPutGenericMessage4LongSmall(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, long arg4, BYTE slotnum = 0);
void NetGetGenericMessage4LongSmall( DPMSG_GENERIC_4_LONG_SM *Net );
void NetSetUpMarshal(PlaneParams *planepnt, long lmaxheight, long lpushtime, long ldelaycnt);
void NetUpdateMarshal(PlaneParams *planepnt);
void NetPutGenericMessage2FPoint(PlaneParams *planepnt, BYTE arg1, FPoint arg2, BYTE slotnum = 0);
void NetGetGenericMessage2FPoint( DPMSG_GENERIC_2_FPOINT *Net);
void NetPutGenericMessage2FPointSmall(PlaneParams *planepnt, BYTE arg1, FPoint arg2, BYTE slotnum = 0);
void NetGetGenericMessage2FPointSmall( DPMSG_GENERIC_2_FPOINT_SM *Net );
void NetPutGenericMessageObjectPoint(PlaneParams *planepnt, BYTE arg1, BYTE objtype, DWORD objectid);
void NetGetGenericMessageObjectPoint( DPMSG_GENERIC_OBJ_POINT *Net);
void NetPutGenericMessageObjectPointSmall(PlaneParams *planepnt, BYTE arg1, BYTE objtype, DWORD objectid);
void NetGetGenericMessageObjectPointSmall( DPMSG_GENERIC_OBJ_POINT_SM *Net);
void *NetGetObjectPointer(int objtype, DWORD serialnumber);
int NetGetMissionHumanAssignments();
void NetPutVehicleGroundLock(int vnum, int windex, int radardatindex, PlaneParams *planepnt);
void NetGetVehicleGroundLock( DPMSG_VGROUND_LOCK *Net );
void NetPutVehicleGroundLockSmall(int vnum, int windex, int radardatindex, PlaneParams *planepnt);
void NetGetVehicleGroundLockSmall( DPMSG_VGROUND_LOCK_SM *Net );
int NetPutSSMissileGeneral(MovingVehicleParams *vehiclepnt, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile);
int NetPutSSMissileGeneralSmall(MovingVehicleParams *vehiclepnt, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile);
int NetPutWeaponPlaneGeneral(void *launcher, int launchertype, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile, int planenum);
int NetPutWeaponPlaneGeneralSmall(void *launcher, int launchertype, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile, int planenum);
int MAILimitVGroundLockSend(DPMSG_VGROUND_LOCK_SM *Net);
void NetPutGenericMessage2Float(PlaneParams *planepnt, BYTE arg1, float arg2, BYTE slotnum = 0);
void NetGetGenericMessage2Float( DPMSG_GENERIC_2_FLOAT *Net);
void NetPutGenericMessage2FloatSmall(PlaneParams *planepnt, BYTE arg1, float arg2, BYTE slotnum = 0);
void NetGetGenericMessage2FloatSmall( DPMSG_GENERIC_2_FLOAT_SM *Net );
void NetPutGenericMessage3Float(PlaneParams *planepnt, BYTE arg1, float arg2, float arg3, BYTE slotnum = 0);
void NetGetGenericMessage3Float( DPMSG_GENERIC_3_FLOAT *Net);
void NetPutGenericMessage3FloatSmall(PlaneParams *planepnt, BYTE arg1, float arg2, float arg3, BYTE slotnum = 0);
void NetGetGenericMessage3FloatSmall( DPMSG_GENERIC_3_FLOAT_SM *Net );
void NetVehicleDamage(MovingVehicleParams *vehiclepnt, float newdamage);
void NetConvertFirstAndLastValid(int firstvalid, int lastvalid, int *retfirst, int *retlast);
void NetCheckDatabase();
void NetPutDBInfo(int slotnum, int planenum, int stationnum);
void NetGetDBInfo(DPMSG_WEAPON_HACK_CHECK *Net);
void NetPutDBInfoSmall(int slotnum, int planenum, int stationnum);
void NetGetDBInfoSmall(int fromslot, DPMSG_WEAPON_HACK_CHECK_SM *Net);
void NetGetMultiPlayerName(int planenum, char *callsign);
void NetCheckStationUpdated(PlaneParams *P, int station);
void NetSendPlayerExit(PlaneParams *planepnt);

#endif
