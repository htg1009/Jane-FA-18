//*****************************************************************************************
// Mission.h
//*****************************************************************************************

#include "fmath.h"
#include "polydraw.h"
#include "family.h"
#include "terrain.h"
#include "flaglist.h"


#define SAVE_BUILDER		2
#define SAVE_BRIEF			1
#define SAVE_MULTI			0

//*****************************************************************************************
#define MAXAIOBJS					120		//  Maximum number of moving objects (planes, vehicles(convoys)).  Also maximum number of paths (Should be less paths then objects since a group of planes can use one path, and alternate paths are the only way a group can have more than one path.
#define MAXWPTS						400		//  Maximum number of waypoints.
#define MAXACTS						400		//  Maximum number of actions.
#define NUMGOALS					100		//  Maximum number of goals (array size).
#define NUMEVENTS					100		//  Maximum number of object events (array size).

#define	MAXMOVINGVEHICLE			100

#define	MAX_GROUPS					20
#define MAX_GROUP_PACKAGE			10

#define MAX_NUM_EYEPOINTS			30
#define MAX_NUM_JSTAR_OBJECTS		20
#define MAX_NUM_TACAN_OBJECTS		30

#define MAX_NUM_DEBRIEF_GROUPS		64
#define MAX_NUM_DEBRIEF_STATEMENTS	32

#define MAX_NUM_CALLSIGN			58
#define	MAX_NUM_INTEL_LABELS		40

#define	APPLY_COM_DESTROYED		0x00000001
#define APPLY_COM_SMOKE			0x00000002
#define APPLY_COM_FIRE_SMOKE	0x00000004
#define APPLY_COM_EXPLODE		0x00000008

#define	WAIT_FLAG_TIME			0x00000001
#define	WAIT_FLAG_FLAG			0x00000002
#define WAIT_FLAG_DISSAPPEAR	0x00000004
#define WAIT_FLAG_FIREWEAPON	0x00000008
#define WAIT_FLAG_SUBMERGE		0x00000010
#define WAIT_FLAG_SURFACE		0x00000020


#define STOP_FLAG_STOP			0x00000001
#define STOP_FLAG_STOPDIS		0x00000002
#define STOP_FLAG_EXPLODE		0x00000004
#define STOP_FLAG_SETFLAG		0x00000008
#define	STOP_FLAG_LOOP			0x00000010
#define STOP_FLAG_TURNAROUND	0x00000020
#define STOP_FLAG_SUBMERGE		0x00000040
#define STOP_FLAG_SURFACE		0x00000080


//*****************************************************************************************
//  DEFINES ****************************************************************************
#define MBISMOVE			0x20000000		//  If &ed with a unique ID identifies that this is a moving object. 
#define MBISSCENARIO		0x40000000		//  If &ed with a unique ID identifies that this is a plug-in object.
#define MBISWORLD			0x80000000		//  If &ed with a unique ID identifies that this is a world object.

#define	AIRCRAFT				0
#define WAYPOINT				1
#define MOVINGVEHICLE			2
#define	GROUNDOBJECT			3
#define WORLDLABEL				4
#define EYEPOINT				5
#define	MOVINGVEHICLEWAYPOINT	6
#define SHIP					7
#define NO_TARGET				8

#define LABEL_TYPE_MISSION		0
#define LABEL_TYPE_GPS			1
#define LABEL_TYPE_CHECKLIST	2

#define MBF18ETYPE			61
//#define MBF18ETYPE			0		//  Number representing plane type is F-15, used to keep humans from being something other then F-15.

#define NAMESIZE			30
#define LONGERNAMESIZE		45






//  iflag in some structures can use these bit flags (like AIObjects).
#define AL_ACTIVE			1		//  Identifies that an object is active/being used.
#define AL_AI_DRIVEN		2		//  Identifies that an object is an AI.
#define AL_DEVICE_DRIVEN	4		//  Identifies that an object is device driven.
#define AL_COMM_DRIVEN		8		//  Identifies that an object is a network player.
#define AL_ISVEHICLE		16		//  Identifies that an object is a ground vehicle.

#define	NUM_ACTION_TYPE		31

#define	ACTION_AWACS_PATTERN			0
#define	ACTION_BOMB_TARGET				1
#define	ACTION_CAP						2
#define ACTION_ESCORT					4
#define ACTION_DAMAGE_THIS_OBJECT		6
#define ACTION_FORMON					10
#define ACTION_HOVER					13
#define ACTION_LAND						14
#define ACTION_MESSAGE					16
#define ACTION_CHANGE_FORMATION			18
#define ACTION_ALTERNATE_PATH			19
#define ACTION_REFUEL					21
#define ACTION_REFUEL_PATTERN			22
#define ACTION_RELEASE_ESCORT			31
#define ACTION_TAKEOFF					26
#define ACTION_SEAD						33
#define ACTION_ALERT_INTERCEPT			34
#define ACTION_CAS						35
#define ACTION_FIGHTER_SWEEP			36
#define ACTION_ORBIT					37
#define ACTION_RELEASE_FORMON			38
#define ACTION_SAR						39
#define	ACTION_SOJ						40
#define ACTION_VIEW						41
#define ACTION_ASW						42
#define ACTION_RELEASE_CHUTES			43
#define ACTION_FAC						44
#define ACTION_ANTISHIP					45
#define ACTION_DRONE					46
#define ACTION_ATTACKTARGET				47
#define ACTION_SETTRIGGER				48



#define EVENT_NONE				0
#define EVENT_TIME				1
#define EVENT_AREA				2
#define EVENT_GOAL				3
#define EVENT_OBJECT			4
#define EVENT_MISSION_END		5
#define EVENT_MOVING_AREA		6

#define EVENT_ACTION_NONE		0
#define EVENT_ACTION_MESSAGE	1
#define	EVENT_ACTION_DAMAGE		2
#define EVENT_ACTION_ALTPATH	3
#define EVENT_ACTION_SETFLAG	4
#define EVENT_ACTION_FAC		5
#define EVENT_ACTION_VIEW		6

#define	USER_FLAG_START			0x00010000
#define	MISSION_FLAG_START		0x00020000
#define	GOAL_START_START		0x00040000

#define	CALLSIGN_AIRCRAFT_F18E				0
#define	CALLSIGN_AIRCRAFT_SAR				1
#define CALLSIGN_AIRCRAFT_STRATEGIC			2
#define CALLSIGN_AIRCRAFT_OTHER				3
#define CALLSIGN_GROUNDCONTROL_TOWER		4
#define CALLSIGN_GROUNDUNIT_USFAC			5
#define CALLSIGN_GROUNDUNIT_UKFAC			6

//**************************************************************************************

#define		GOAL_TYPE_NONE				0
#define		GOAL_TYPE_MUSTDESTROY		1
#define		GOAL_TYPE_MUSTSURVIVE		2

//**************************************************************************************

#define VEHICLE_ACTION_ATTACKALONGPATH					0x00000001
#define VEHICLE_ACTION_ATTACKSPECIFIC					0x00000002
#define VEHICLE_ACTION_ATTACKAROUNDSPECIFIC				0x00000004
#define VEHICLE_ACTION_NOTUSED							0x00000008
#define VEHICLE_ACTION_FORMATION1						0x00000010
#define VEHICLE_ACTION_FORMATION2						0x00000020
#define VEHICLE_ACTION_FORMATION3						0x00000040
#define VEHICLE_ACTION_FORMATION4						0x00000080
#define VEHICLE_ACTION_FORMATION5						0x00000100
#define VEHICLE_ACTION_GROUNDOBJECT						0x00000200
#define VEHICLE_ACTION_MOVINGVEHICLE					0x00000400


#define DEBRIEF_FLIGHT_FLAG_PLAYER						0x00000001
#define DEBRIEF_FLIGHT_FLAG_GROUP_A						0x00000002
#define DEBRIEF_FLIGHT_FLAG_GROUP_B						0x00000004
#define DEBRIEF_FLIGHT_FLAG_GROUP_C						0x00000008
#define DEBRIEF_FLIGHT_FLAG_GROUP_D						0x00000010
#define DEBRIEF_FLIGHT_FLAG_GROUP_E						0x00000020
#define DEBRIEF_FLIGHT_FLAG_GROUP_F						0x00000040
#define DEBRIEF_FLIGHT_FLAG_GROUP_G						0x00000080
#define DEBRIEF_FLIGHT_FLAG_GROUP_H						0x00000100


#define FLIGHT_FLAG_GROUP1			0x00000001
#define FLIGHT_FLAG_GROUP2			0x00000002
#define FLIGHT_FLAG_GROUP3			0x00000004
#define FLIGHT_FLAG_GROUP4			0x00000008
#define FLIGHT_FLAG_GROUP5			0x00000010
#define FLIGHT_FLAG_GROUP6			0x00000020
#define FLIGHT_FLAG_GROUP7			0x00000040
#define FLIGHT_FLAG_GROUP8			0x00000080
#define FLIGHT_FLAG_HOME_AIRPORT	0x00000100
#define FLIGHT_FLAG_HOME_CARRIER	0x00000200

#define	FENCE_IRAQ		0x01
#define FENCE_KUWAIT	0x02
#define FENCE_IRAN		0x04

#define FENCE1			0x01
#define FENCE2			0x02
#define FENCE3			0x04
#define	FENCE4			0x08

#define	CR_RESETSTATUS		0x0001
#define CR_DONOTTRACK		0x0002

#define VEHICLE_NORMAL_TYPE			0x00000001
#define VEHICLE_SHIP_TYPE			0x00000002


#define SPLASH_LOCATION_BRIEF		0x00000000
#define SPLASH_LOCATION_DEBRIEF		0x00000001

#define MISSION_FLOW_BRIEFING		0x00000001
#define MISSION_FLOW_ARMING			0x00000002
#define MISSION_FLOW_FLIGHT			0x00000004
#define MISSION_FLOW_MP				0x00000008
#define MISSION_FLOW_DEBRIEF		0x00000010

#define MISSION_FLOW_DEMOMODE		0x00000001
#define MISSION_FLOW_PLAYERCONTROL	0x00000002

#define	MISSION_VERSION				1
#define MISSION_VERSION_OLD			0

#define NUM_SPLASH_SCREEN			20

#define MAX_NUM_CHECKLIST_ITEMS		10

struct stEventDisplayOptions
{
	char bPlayer;
	char bFlightGroupA;
	char bFlightGroupB;
	char bFlightGroupC;
	char bFlightGroupD;
	char bFlightGroupE;
	char bFlightGroupF;
	char bFlightGroupG;
	char bFlightGroupH;
	char bFriendlyAircraft;
	char bFriendlyCarrier;
	char bFriendlyShips;
	char bFriendlyVehicles;
	char bFriendlyCounter;
	char bFriendlyGuns;
	char bFriendlyWeapons;
	char bEnemyAircraft;
	char bEnemyShips;
	char bEnemyVehicles;
	char bEnemyCounter;
	char bEnemyGuns;
	char bEnemyWeapons;
	char bNeutralAircraft;
	char bNeutralShips;
	char bNeutralVehicles;
	char bNeutralCounter;
	char bNeutralWeapons;
	char bNeutralGuns;
	char bLabels;
	char bTrailType;			// 0 - Off, 1 - short, 2 - long
	char bDamagedObjects;		// 0 - No Damaged Objects Reports,Damaged objects reports
	char bRadioEvents;
	char bEngagedObjects;
	char b3DObjects;
	char bTimeLogEvents;
	int iPlayBackRate;
};
typedef struct stEventDisplayOptions EventDisplayOptionsType;


struct stSplashScreenType
{
	long lLocation;
	long lImageOptions;
	char sImageFileName[256];
	long lSoundOptions;
	char sWAVFileName[256];
	long lDurationOption;
	long lDurationTime;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};
typedef struct stSplashScreenType SplashScreenType;

struct stMissionFlowControl
{
	long bfMissionFlow;
	long lBriefingDurationOption;
	long lDebriefDurationOption;
	long lBriefingDurationTime;
	long lDebriefDurationTime;
	long lDemoMode;
	int iNumSplash;
	SplashScreenType SplashScreen[NUM_SPLASH_SCREEN];
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};
typedef struct stMissionFlowControl MissionFlowControl;

struct stBriefingSummaryType
{
	char sObjectiveImageFileName[256];
	char sObjectiveSpeechFileName[256];
	char sFlightSpeechFileName[256];
	char sMapSpeechFileName[256];
	char sBriefingSpeechFileName[256];
	char sMapImageFileName[256];
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
	long lSummaryBriefingSize;
	char *pszBuffer;
};
typedef struct stBriefingSummaryType BriefingSummaryType;

//***********************************************************************************************************************************
// CHUNK FILE TYPES
//
// 0x00000010	Mission File

#define CHUNK_MISSION_FILE		0x00000010


//***********************************************************************************************************************************
// CHUNK TYPES
//
// 0x00000010	Mission File V201
// 0x00000050	Mission Summary Data
// 0x00000100	Load Mission Image
// 0x00000150	Sound File

#define	MISSION_FILE_DATA				0x00000010
#define MISSION_BRIEFINGSUMMARY_DATA	0x00000050
#define MISSION_FLOW_CONTROL			0x00000060
#define MISSION_MISSIONINFO_DATA		0x00000070
#define MISSION_LOADMISSION_IMAGE		0x00000100
#define MISSION_SPASH_IMAGE				0x00000101
#define MISSION_OBJECTIVE_IMAGE			0x00000102
#define MISSION_OBJECTIVE_WAV			0x00000104
#define MISSION_FLIGHT_WAV				0x00000106
#define MISSION_MAP_WAV					0x00000108
#define MISSION_BRIEFING_WAV			0x00000110
#define MISSION_DETAILEDBRIEFING_WAV	0x00000112
#define MISSION_SPLASH_IMAGE			0x00000114
#define MISSION_SPLASH_WAV				0x00000116
#define MISSION_LOADOUT					0x00000118
#define MISSION_MAP_IMAGE				0x00000120

#define	CHUNK_CAMPAIGN_FILE				0x00000040
#define CAMPAIGN_IMAGE					0x00000122
#define CAMPAIGN_SAVE_IMAGE				0x00000124
#define CAMPAIGN_BUILDER_DATA			0x00000126

#define MISSION_USERWAV					0x00000150
#define MISSION_USERWAV_HEADER 			0x00000152

//**************************************************************************************

struct stMissionFileSummary
{
	char sLongFileName[256];		// Loading Screens - So we can be more descriptive in file name
	char sImageName[256];			// Loading Screens - Image
	long lMissionTitleSize;			// Loading Screens - Title on Loading Screen
	char *psTitle;					// " "	
	long lMissionSummarySize;		// Loading Screens - Mission Description
	char *psMissionSummary;
	long lCommentsSize;
	char *psComments;
	long lAuthorSize;				// Who Made Me
	char *psAuthor;					// " "
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};
typedef struct stMissionFileSummary MissionFileSummary;

//**************************************************************************************


struct stMissionIdTag
{
	char sTag[4];
	long lMissionFormatIdTag;
	long lVersion;
	long lSubVersion;
};

typedef struct stMissionIdTag MissionIdTagType;

struct stEditorInfo
{
	long lMapX0;		//  Holds the displayed left edge (in feet) from the edge of world.
	long lMapZ0;		//  Holds the displayed top edge (in feet) from the edge of world.
	long lMapX1;		//  Holds the displayed right edge (in feet) from the edge of world.
	long lMapZ1;		//  Holds the displayed bottom edge cooridinate (in feet) from the edge of world.
	long lMapMaxX;		//  Holds the width of the world (X), in feet, for the loaded world.
	long lMapMaxZ;		//  Holds the width of the world (Z), in feet, for the loaded world.
	long lMapScreenX;
	long lMapScreenZ;
	long lMissionTitleSize;
	char *psTitle;
	long lAuthorSize;
	char *psAuthor;
	long lMissionSummarySize;
	char *psMissionSummary;
	long lCommentsSize;
	char *psComments;
	int iAnimationType;
	long lNotUsed[10];
};

typedef struct stEditorInfo EditorInfoType;

struct stMissionInfo
{
	long lWorldId;
	long lBriefingSize;
	char *psBriefing;
	long lMissionTime;
	long lMissionWeather;
	long lFence;
	long lLanguageId;
	long lFriendlyROE;
	long lEnemyROE;
	long lReserved3;
	long lOverviewMapX0;
	long lOverviewMapX1;
	long lOverviewMapZ0;
	long lOverviewMapZ1;
	long lWeaponGoalAreaX0;
	long lWeaponGoalAreaX1;
	long lWeaponGoalAreaZ0;
	long lWeaponGoalAreaZ1;
	long lWeaponType;
	long lObjectiveType;
	long lObjectiveID;
	long lObjectStatus;
	long lNotUsed[2];
};

typedef struct stMissionInfo MissionInfoType;

// 0x01	- Iraq
// 0x02	- Kuwait
// 0x04 - Iran
// 0x08 - UAE

struct stColor
{
	int iID;
	char sName[NAMESIZE];
	COLORREF crColor;
};

typedef struct stColor ColorType;

struct stControlType
{
	int iID;
	char sName[LONGERNAMESIZE];
};

typedef struct stControlType ControlType;

struct stDuration
{
	int iID;
	char sName[LONGERNAMESIZE];
};

typedef struct stDuration DurationType;

struct stWhen
{
	int iID;
	char sName[LONGERNAMESIZE];
};

typedef struct stWhen WhenType;



struct stDirection
{
	int iID;
	char sName[LONGERNAMESIZE];
	int iHeadingDegrees;
};

typedef struct stDirection DirectionType;


struct stCallSign
{
	int iPlaneType;
	int iID;
	char sName[50];
	long lSpeechID;
	long lIntSpeechID;
};

typedef struct stCallSign CallSignType;

struct stFormation
{
	int iID;
	char sName[LONGERNAMESIZE];
};

typedef struct stFormation Formation;

struct stOrbitUntil
{
	int iID;
	char sName[LONGERNAMESIZE];
};

typedef struct stOrbitUntil OrbitUntil;

struct stSides
{
	int iID;
	char sName[LONGERNAMESIZE];
};

typedef struct stSides SidesType;

struct stEyePoint
{
	long X;
	long Y;
	long Z;
	int H;
	int P;
	int R;
	int iZoom;
	long lTimer;
	long lZoomTimer;
	long lFlags;	// For things like zoom over time
	long lRange;
	long lObject;
	char sLabel[NAMESIZE];
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
	long lReserved5;
};

typedef struct stEyePoint EyePointActionType;

struct stViewAction
{
	int iViewTypeFlag;		// 0 = Eyepoint,1=Camera
	long lEyePointIndex;	// Index into Eyepoint List
	long lCameraView;		// Camera View Define
	char sFunction[100];	// Function Name
	char sLabel[35];
	long lTime;				// Seconds
	long lTrackingFlag;		// 0 = Default
							// 1 = Aircraft
							// 2 = Moving Vehicle
							// 4 = Ground Object (PlugIn)
	long lObjectID;			// For the above
	int iDefaultView;		// Is this the new default view
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
	long lReserved5;
};
typedef struct stViewAction ViewActionType;

struct stCAP
{
	int iHeading;
	long lRange;
	long lTimer;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stCAP CAPActionType;

struct stSOJ
{
	int iHeading;
	long lRange;
	long lTimer;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stSOJ SOJActionType;

struct stCAS
{
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stCAS CASActionType;

struct stASW
{
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stASW ASWActionType;

struct stReleaseChutes
{
	long lNotUsed[5];
};

typedef struct stReleaseChutes ReleaseChutesActionType;


struct stFAC
{
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stFAC FACActionType;

struct stAntiShip
{
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stAntiShip AntiShipActionType;

struct stDrone
{
	long lNotUsed[5];
};

typedef struct stDrone DroneActionType;

struct stRefuel
{
	long lNotUsed[5];
};

typedef struct stRefuel RefuelActionType;


struct stAttackTarget
{
	long lTargetType;
	long lTargetID;
	long lNotUsed[10];
};

typedef struct stAttackTarget AttackTargetActionType;

struct stSetTrigger
{
	long lUserFlag;
	long lNotUsed[5];
};

typedef struct stSetTrigger SetTriggerActionType;

struct stSAR
{
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[5];
};

typedef struct stSAR SARActionType;

struct stDamage
{
	long lDamageAmount;
	long lNotUsed[5];
};
typedef struct stDamage DamageActionType;

struct stFighterSweep
{
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[5];
};
typedef struct stFighterSweep FighterSweepActionType;

struct stFormationType
{
	int iFormation;
	long lNotUsed[5];
};
typedef struct stFormationType FormationActionType;

struct stOrbit
{
	long lTimer;
	int iUntil;
	long lFlag;
	long lNotUsed[5];
};
typedef struct stOrbit OrbitActionType;

struct stSEAD
{
	long lRange;
	long lTimer;
	long lFlag;
	long lNotUsed[5];
};
typedef struct stSEAD SEADActionType;

struct stMessage
{
	char sMessage[320];
	char sSoundFile[260];
	long lNotUsed[5];
};
typedef struct stMessage MessageActionType;

struct stTankerAction
{
	long lTimer;
	long lFlag;
	long lNotUsed[5];
};
typedef struct stTankerAction TankerActionType;

struct stAWACSPattern
{
	long lTimer;
	long lFlag;
	long lNotUsed[5];
};
typedef struct stAWACSPattern AWACSPatternActionType;

struct stHover
{
	long lTimer;
	long lFlag;
	long lNotUsed[5];
};
typedef struct stHover HoverActionType;


struct stRunway
{
	long dwSerialNumber;
	long lX;
	long lY;
	long lZ;
	long lRunwayStartX;
	long lRunwayStartY;
	long lRunwayStartZ;
	long lRunwayEndX;
	long lRunwayEndY;
	long lRunwayEndZ;
	long lTimer;
	long lRange;
	long lFlag;
	long lNotUsed[10];
};

typedef struct stRunway Landing;
typedef struct stRunway TakeOffActionType;
typedef struct stRunway AlertIntercept;


struct stFormOn
{
	int iPlaneNum;
	int iWayPointNum;
	long lNotUsed[5];
};
typedef struct stFormOn FormOn;

struct stEscort
{
	int iPlaneNum;
	int iWayPointNum;
	long lNotUsed[5];
};
typedef struct stEscort Escort;

struct stAttackProfile
{
	int iID;
	char sName[LONGERNAMESIZE];
};
typedef struct stAttackProfile AttackProfile;

struct stFlightAttack
{
	DWORD dwSerialNumber;
	long lType;
	long X;
	long Y;
	long Z;
	int iFlags;
	long lNotUsed[10];
};
typedef struct stFlightAttack FlightAttack;

struct stIntelLabel
{
	char sLabel[NAMESIZE];
	long lX;
	long lY;
	long lZ;
	int iWidth;
	int iHeight;
	int iFlags;
	int iForeColor;
	int iBackColor;
};
typedef struct stIntelLabel IntelLabelType;

struct stIntelligence
{
	long lX;
	long lY;
	long lZ;
	long lULX;
	long lULZ;
	long lLRX;
	long lLRZ;
	long lFlags;
	long lZoom;
	char sLabel[LONGERNAMESIZE];
	int iNumLabels;
	IntelLabelType IntelLabels[MAX_NUM_INTEL_LABELS];
	long lNotUsed[10];
};
typedef struct stIntelligence IntelligenceType;

struct stBombTarget
{
	int iAttackProfile;
	FlightAttack FlightAttackList[8];
	IntelligenceType IntelligenceArea;
	long lNotUsed[10];
};
typedef struct stBombTarget BombTarget;


struct stActionType
{
	int iId;
	char sDescription[LONGERNAMESIZE];
};
typedef struct stActionType ActionType;

struct stGoalType
{
	int iId;
	char sName[LONGERNAMESIZE];
};
typedef struct stGoalType GoalType;

struct stMPoint
{
	long x;
	long z;
};
typedef struct stMPoint MPoint;

struct stAreaGoal
{
	long x0;
	long z0;
	long x1;
	long z1;
};
typedef struct stAreaGoal AreaGoalType;

struct stPath
{
	int iNumPoints;
	MPoint point[20];
};
typedef struct stPath PathType;

struct stFenceV0
{
	int iNumPoints;
	MPoint point[30];
};
typedef struct stFenceV0 FenceTypeV0;

struct stFenceV1
{
	int iNumPoints;
	MPoint point[100];
};
typedef struct stFenceV1 FenceTypeV1;

struct stAIObject 
{				
	long X;						//  Holds the starting X position of the object, in feet, from the upper left corner of the map.
	long Y;						//  Holds the starting Y position of the object in feet.
	long Z;						//  Holds the starting X position of the object, in feet, from the upper left corner of the map.
	long lHeading;				//  Starting Heading
	int lSpeed;					//  Starting Speed
	int iType;					//  Tells what type of object this is.
	long lDifficulty;
	int iFlags;					//  Used for bit flags, like control type.  Not yet full.
	int iSide;					//  Holds the value for which side this object is on (could be switched to country)
	int iNumInFlight;			//  Tells how many objects are in this group/flight.
	int	iWPPathNum;				//  Tells which element in the AIWPPaths array that this object is going to follow.
	int iFlightFlags;			//	New Flags for flight info (that's descriptive)
	int iRandomChance;			//  Tells what the change is this object is going to appear.
	int iRandomGroup;			//  Tells which random group this object belongs.  Random groups can be used to set up such things as a random CAP, where each object of this group number is flying a CAP mission but in a different area.  Only one element/object of a group is picked for a mission.
	int iRandomElement;			//  Tells which random element this object belongs.  Random elements are used to "attach" objects together within a random group so that if one appears they all appear.  For example, if a attack group is picked, you would want to have its cover appear as well.
	char sName[NAMESIZE];		//  Holds a text string that can be used to identify an object with in the mission builder.  This is just in the builder and has no effect "in game".
	long lCallSign;				// 	Index into callsign list for this type of object
	long lLoadOut;				//  Id of Generic Loadout
	char bBriefing;				//  Display on briefing Map (TRUE/FALSE)
	char bThreat;				//  Display on threat list (TRUE/FALSE)
	char cRouteColor;			//  Index into a color array
	char cLabelColor;			//  Index into a color array
	long iHomeBaseId;			//	Dependent on iFlightFlags
	long iReserved2;	  
	int iBriefingGroup;
	long lSubCallSignIndex;
	long lNotUsed[9];
};
typedef struct stAIObject MBObject;

struct stGroundObject 
{			   
	DWORD dwSerialNumber;
	int iSide;						//  Holds the value for which side this object is on (could be switched to country)
	char sFileName[14];
	char bBriefingMap;
	char bBriefingThreat;
	long lReserved1;					// Callsign (used if Airport)
	long lCampaignResourceFlags;
	long lNotUsed[9];
};
typedef struct stGroundObject GroundObject;

struct stWPPaths	
{
	long lObjLink;  //  This lets me know for which object group this is a path for.  Right now it give the array number into AIObjects, though could be leader's ID (Advantages to each)
	int	iStartWP;	//  Tells which element in AIWayPoints is the first waypoint for this object to follow.
	int iNumWP;		//  Tells how many consecutive waypoints this object is supposed to follow.
};
typedef struct stWPPaths MBWPPaths;

struct stWayPoints		//  Holds information for where waypoints are located, and what actions are associated with them.
{
	long	lWPx;		//  Holds the X position of the waypoint in feet from the top left corner of the map.
	long	lWPy;		//  Holds the Y position of the waypoint in feet.
	long	lWPz;		//  Holds the Z position of the waypoint in feet from the top left corner of the map.
	int		iSpeed;
	char	sName[NAMESIZE];
	char	bJumpPoint;
	int		iStartAct;	//  Tells which element in AIActions is the first action for this waypoint (-1 if no action).
	int		iNumActs;	//  Tells how many consecutive action slots are used by this waypoint (an action can use more than 1 action slot.  (0 if no actions)
	int		iModeWayPoint;
	long	lFlags;
	long lNotUsed[3];
};
typedef struct stWayPoints MBWayPoints;

struct stMBActions			//  Holds information about actions an data about the actions.  Some actions can use more than one of these (Such as bombing actions).
{
	long	ActionID;		//  Identifies what action is supposed to take place (unless continuation of a previous action).
	unsigned long pAction;	//  ptr to the data that helps define the action, such as identifying what new formation is supposed to be flown if the ActionID represents a new formation.
};
typedef struct stMBActions MBActions;

struct stMBEvents
{
	long lEventID;
	unsigned long pEvent;
};
typedef struct stMBEvents MBEvents;

struct stMBCondition
{
	int iType;
	int iIndex;
	short wOperator;
	short wValue;
};
typedef struct stMBCondition MBCondition;

struct stAreaEvent
{
	long x0;
	long z0;
	long x1;
	long z1;
	char sLabel[LONGERNAMESIZE];
	int iWhen;
	int iDuration;
	int iNumEventActions;
	int iNumEventConditions;
	MBEvents EventAction[15];
	MBCondition EventCondition[20];
	long lNotUsed[10];
};
typedef struct stAreaEvent AreaEventType;


struct stTimeStart
{
	int iID;
	char sLabel[LONGERNAMESIZE];
};
typedef struct stTimeStart TimeStartType;

struct stMissionFlag
{
	int iID;
	char sLabel[LONGERNAMESIZE];
	int iType;
};
typedef struct stMissionFlag MissionFlag;

struct stMissionEventFlag
{
	short iID;
	short iFlight;
	short iValue;
};
typedef struct stMissionEventFlag MissionEventFlag;

struct stTimeEvent
{
	int iStart;
	int iTime;
	int iDuration;
	char sLabel[LONGERNAMESIZE];
	int iNumEventActions;
	int iNumEventConditions;
	MBEvents EventAction[15];
	MBCondition EventCondition[20];
	long lNotUsed[10];
};
typedef struct stTimeEvent TimeEventType;

struct stMissionEndEvent
{
	int iFlag;
	int iDelay;
	int iReserved1;
	char sLabel[LONGERNAMESIZE];
	int iNumEventConditions;
	MBCondition EventCondition[20];
	long lNotUsed[10];
};
typedef struct stMissionEndEvent MissionEndEventType;

struct stGoalEvent
{
	int iGoalNum;
	char sLabel[LONGERNAMESIZE];
	int iNumEventActions;
	int iNumEventConditions;
	MBEvents EventAction[15];
	MBCondition EventCondition[20];
	long lNotUsed[10];
};
typedef struct stGoalEvent GoalEventType;

struct stMovingAreaEvent
{
	long lObjectType;	// SHIP,MOVINGVEHICLE,AIRCRAFT
	long lObjectID;
	long lRange;
	char sLabel[LONGERNAMESIZE];
	int iNumEventActions;
	int iNumEventConditions;
	MBEvents EventAction[15];
	MBCondition EventCondition[20];
	long lWhenCondition;
	long lDuration;
	long lNotUsed[8];
};
typedef struct stMovingAreaEvent MovingAreaEventType;

struct stObjectEvent
{
	int iObjectType;
	int iObjectNum;
	int iSubObjectId;
	char sLabel[LONGERNAMESIZE];
	int iNumEventActions;
	int iNumEventConditions;
	MBEvents EventAction[15];
	MBCondition EventCondition[20];
	long lNotUsed[10];
};
typedef struct stObjectEvent ObjectEventType;

struct stEventActionMessage
{
	char sMessage[320];
	char sSoundFile[256];
};
typedef struct stEventActionMessage EventActionMessageType;


struct stEventActionSetFlag
{
	int iFlag;
	int iOperator;
	int iValue;
};
typedef struct stEventActionSetFlag EventActionSetFlagType;

struct stEventActionAltPath
{
	int iPath;
};
typedef struct stEventActionAltPath EventActionAltPathType;


struct stEventActionDamage
{
	long iObjectID;
	int iDamageAmount;
};
typedef struct stEventActionDamage EventActionDamageType;


struct stEventActionFAC
{
	long lReserved1;
	long lReserved2;
	long lReserved3;
};
typedef struct stEventActionFAC EventActionFACType;


typedef struct stEyePoint EventActionEyePointType;



struct stEventCondOperator
{
	int iType;
	char sLabel[10];
};
typedef struct stEventCondOperator EventCondOperator;

struct stEventCondValue
{
	int iType;
	char sLabel[20];
};

typedef struct stEventCondValue EventCondValue;

struct stDebriefItemOperator
{
	int iType;
	char sLabel[10];
};
typedef struct stDebriefItemOperator DebriefItemOperator;

struct stDebriefCondValue
{
	int iType;
	long lLanguageID;
};
typedef struct stDebriefCondValue DebriefCondValue;


typedef struct stMBGoalInfo			//  Is the structure which holds the info for one goal.
{
	long lGoalRating;				//  !!! Only Used For Area Goals = 0xFF !!!
	long lGoalType;					//  Identifies type of goal (Must Protext, Must Destroy, ...)
	long lObjectType;				//  Aircraft, Moving Vehicle, Plugin, Fixed Object
	DWORD dwSerialNumber;			//  Used to be lObjectIndex
	char sLabel[LONGERNAMESIZE];
	long lGoalRatingType;			// 0 = Normal, 1 = Primary, 2 = Secondary
	long lNotUsed[9];
} MBGoalInfo;


struct stPackageType
{
	char sName[NAMESIZE];
	int iChance;
};
typedef struct stPackageType PackageType;

struct stGroupType
{
	int iID;
	char sName[NAMESIZE];
	int iNumPackage;
	PackageType Package[MAX_GROUP_PACKAGE];
};
typedef struct stGroupType GroupType;

struct stDebriefStatementItem
{
	int iType;
	int iID;
	int iValue;
	int iOperator;
	int iFlag;
	long lDebriefFlightFlag;
	long lNotUsed[9];
};
typedef struct stDebriefStatementItem DebriefStatementItem;

struct stDebriefStatement
{
	int iID;
	char sName[NAMESIZE];
	int iNumItems;
	char *pszMessage;
	int iMessageSize;
	DebriefStatementItem *pDebriefStatementItem[MAX_NUM_DEBRIEF_STATEMENTS];
	char szFileName[260];
	long lNotUsed[10];
};
typedef struct stDebriefStatement DebriefStatement;

struct stDebriefGroups
{
	int iID;
	char sName[NAMESIZE];
	int iStopProcessing;
	int iNumStatements;
	long lCampaignGoalNumber;
	long lDebriefSummary;
	DebriefStatement *pDebriefStatement[MAX_NUM_DEBRIEF_STATEMENTS];
	long lNotUsed[10];
};
typedef struct stDebriefGroups DebriefGroups;


struct stDebriefType
{
	int iNumDebriefGroups;
	DebriefGroups *pDebriefGroup[MAX_NUM_DEBRIEF_GROUPS];
	long lNotUsed[10];
};
typedef struct stDebriefType DebriefType;

struct stMissionFlagType
{
	int iMissionFlagID;
	long lLanguageID;
};
typedef struct stMissionFlagType MissionFlagType;

struct stJSTARObject
{
	int iObjectType;
	DWORD dwSerialNumber;
	long lX;
	long lY;
	long lZ;
	long lType;
	long lReserved1;
	long lReserved2;
};
typedef struct stJSTARObject JSTARObject;

struct stJSTARType
{
	int iMode;
	JSTARObject JSTARObjectList[MAX_NUM_JSTAR_OBJECTS];
	long iNumJSTARObjects;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};
typedef struct stJSTARType JSTARType;

struct stTACANType
{
	int iType;
	long lChannel;
	DWORD dwSerialNumber;
	long lX;
	long lY;
	long lZ;
	char sLabel[NAMESIZE];
	char sAbbrev[4];
//	long lReserved1;
	long lReserved2;
	long lReserved3;
};
typedef struct stTACANType TACANType;

struct stFACType
{
	int iType;
	DWORD dwSerialNumber;
	long lX;
	long lY;
	long lZ;
	long lFlags;
	long lCallSign;
	long lSide;
	long lReserved2;
	long lReserved3;
};
typedef struct stFACType FACType;

struct stMissionLabel
{
	char sLabel[LONGERNAMESIZE];
	char sGPSLabel[260];
	long lX;
	long lY;
	int iWidth;
	int iHeight;
	int iFlags;
	int iForeColor;
	int iBackColor;
	int iFontSize;
	int iFontFlags;
	char bBold;
	char bItalic;
	char bUnderLine;
	long lMaxRange;
	long lMinRange;
	long lOverviewMapFlag;
	long lZ;
	long lGPSLabel;	  // 0 = Regular Label, else GPS Label
	long lCheckListNum;
	long lReserved[9];
};
typedef struct stMissionLabel MissionLabelType;

struct stApplyCommandsV0
{
	DWORD dwSerialNumber;
	long lFlags;
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};
typedef struct stApplyCommandsV0 ApplyCommandsTypeV0;

struct stApplyCommandsV1
{
	long lVersion;
	DWORD dwSerialNumber;
	long lFlags;
	FPointDouble TempLocation;
	ANGLE		 Heading;
	ANGLE		 Pitch;
	ANGLE		 Roll;
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};
typedef struct stApplyCommandsV1 ApplyCommandsTypeV1;



struct stCampaignStatusFileSizes
{
	long lSizeOfData;
	long lSizeOfGoals;
	long lSizeOfUsageList;
	long lSizeOfDamageList;
};
typedef struct stCampaignStatusFileSizes CampaignStatusFileSizes;

struct stCampaignDamageType
{
	DWORD dwSerialNumber;
	int iStatus;
	int MissionsTillRepaired;
	int OriginalMissionsTillRepaired;
	DWORD dwReplacementSerial;
	int MissionsTillNoFire;
	int OriginalMissionsTillNoFire;
	int iBurnType;
	int OriginalBurnTime;
	ExtraDamageInfo DamagePlacement;
};
typedef struct stCampaignDamageType CampaignDamageType;

struct stRoutePoint
{
	int iRouteActionFlags;
	int iWaitFlags;
	int iWaitTime;
	int iWaitFlag;
	int iEndOfPathOptions;
	int iUserFlag;
	int iObjectID;		// Object ID - Vehicle Index or Ground Object Serial Number
	long lX;
	long lY;
	long lZ;
	long lNotUsed[10];
};
typedef struct stRoutePoint RoutePoint;

struct stMoving 
{
	int iID;
	int iSide;					//  Holds the value for which side this object is on (could be switched to country)
	int iType;					//  Vehicle Type
	int iNumGroup;				//  Tells how many objects are in this group/flight.
	int iRandomChance;			//  Tells what the change is this object is going to appear.
	int iRandomGroup;			//  Tells which random group this object belongs.  Random groups can be used to set up such things as a random CAP, where each object of this group number is flying a CAP mission but in a different area.  Only one element/object of a group is picked for a mission.
	int iRandomElement;			//  Tells which random element this object belongs.  Random elements are used to "attach" objects together within a random group so that if one appears they all appear.  For example, if a attack group is picked, you would want to have its cover appear as well.
	int iFlags;
	char sBriefingLabel[LONGERNAMESIZE];	//  Holds a text string that can be used to identify an object with in the mission builder.  This is just in the builder and has no effect "in game".
	char bBriefingMap;			// Display on Briefing Map
	char bBriefingThreat;		// Display on Threat Map
	int iStartingCondition;		// Starting Condition 0=Time,1=Flag
	int iStartingTime;			// Starting Time
	int iStartingFlag;			// Starting User Flag
	char cRouteColor;			//  Index into a color array
	char cLabelColor;			//  Index into a color array
	int iOtherStartFlags;
	int iLeader;
	int iReserved1;
	long lResourceNumber;
	long lRequiredObject;
	long lNotUsed[8];
	int iNumPoints;					// Number of waypoints
	RoutePoint *pRoutePointList;	// Waypoint list
};
typedef struct stMoving MovingVehicle;


typedef struct
{
	int		nStatus;
	int		nBombsReleased;
	int		nBombsHit;
	int		nMissilesReleased;
	int		nMissilesHit;
	int		nMissileKills;
	int		nReserved1;
	int		nReserved2;
	int		nReserved3;
	int		nReserved4;
	int		nReserved5;
	int		nReserved6;
} FLYRESULTS;

// nStatus
// 0 - Flight Ready
// 1 - Successfully returned to base
// 2 - Crashed and was killed
// 3 - Shotdown by AAA, ejected and was killed
// 4 - Shotdown by AAA, ejected and survived
// 5 - Shotdown by AAA, ejected and is missing
// 6 - Shotdown by SAM, ejected and was killed
// 7 - Shotdown by SAM, ejected and survived
// 8 - Shotdown by SAM, ejected and is missing
// 9 - Shotdown by enemy aircraft, ejected and was killed
// 10 - Shotdown by enemy aircraft, ejected and survived
// 11 - Shotdown by enemy aircraft, ejected and is missing
// 12 - Shotdown by friendly aircraft, ejected and was killed
// 13 - Shotdown by friendly aircraft, ejected and survived
// 14 - Shotdown by friendly aircraft, ejected and is missing
// 15 - Obtained heavy damage, but ready for flight
// 16 - Crashed and survived!
// 17 - Ejected and survived!
// 18 - Ejected and was killed
// 19 - Ejected and is missing

typedef struct
{
	FLYRESULTS		FlightResults[8];
} FLIGHTRESULTS;

struct stAreaZoom
{
	long x0;
	long z0;
	long x1;
	long z1;
};
typedef struct stAreaZoom AreaZoomType;

struct stTokenList
{
	long lLanguageID;
	long lTokenID;
	long lVal1;
	long lVal2;
	long lVal3;
	long lVal4;
	long lReserved1;
	long lTextSize;
	char *pText;
};

typedef struct stTokenList TokenList;

struct stLocationData
{
	long H;
	long P;
	long R;
	double X;
	double Y;
	double Z;
	char szName[35];
	float fWorldTime;
	long lWeather;
	long lReserved1;
	long lReserved2;
	long lReserved3;
};
typedef struct stLocationData LocationData;


//**************************************************************************************
#ifdef MBBEHAVE

MissionFlowControl BuilderFlowControl;
BriefingSummaryType BuilderBriefingSummary;
MissionFileSummary BuilderMissionSummary;


MissionFlowControl g_PreviousBuilderFlowControl;
BriefingSummaryType g_PreviousBuilderBriefingSummary;
MissionFileSummary g_PreviousBuilderMissionSummary;


MBObject			AIObjects[MAXAIOBJS];					//  Holds the data about the objects in the mission.
MBWPPaths			AIWPPaths[MAXAIOBJS];					//  Holds data about the paths in the mission.
MBWayPoints			AIWayPoints[MAXWPTS];					//  Holds data about the waypoints in the mission.
MBActions			AIActions[MAXACTS];						//  Holds data about the actions in the mission.
MBGoalInfo			GoalList[NUMGOALS];						//  This holds the information about the mission goals.
MBEvents			EventList[NUMEVENTS];
GroupType			GroupList[MAX_GROUPS];

LocationData *g_pLocationList=NULL;
int g_iNumLocations=0;

AreaZoomType AreaZoom;

GroundObject *pGroundObjectList=NULL;

MovingVehicle	*g_pMovingVehicleList=NULL;
int g_iNumMovingVehicle=0;

ApplyCommandsTypeV0 *pApplyCommandsListV0=NULL;
ApplyCommandsTypeV1 *pApplyCommandsList=NULL;
int g_iNumApplyCommands=0;

int g_iNumMissionLabels=0;
MissionLabelType *pMissionLabelList=NULL;

int g_iNumJSTARObjects=0;
int g_iNumTACANObjects=0;
FACType FACObject;
JSTARType JSTARList;
TACANType *pTACANTypeList=NULL;
DebriefType DebriefData;

int g_iNumFenceArea=0;
FenceTypeV1 FenceArea[5];

ColorType ColorList[14] = 
{
	{ 0,"Black",RGB(0,0,0) },
	{ 1,"Red",RGB(255,0,0) },
	{ 2,"Green",RGB(0,255,0) },
	{ 3,"Blue",RGB(0,0,255) },
	{ 4,"Yellow",RGB(255,255,0) },
	{ 5,"White",RGB(255,255,255) },
	{ 6,"Grey",RGB(127,127,127) },
	{ 7,"Pink",RGB(255,150,160) },
	{ 8,"Purple",RGB(204,0,204) },
	{ 9,"Light Blue",RGB(4,253,255) },
	{ 10, "Brown",RGB(160,82,4) },
	{ 11, "Orange",RGB(245,134,55) },
	{ 12, "Lime",RGB(125,255,4) },
	{ 13, "Light Gray",RGB(200,200,200) }
};

ColorType ColorBackList[8] = 
{
	{ 0,"None",RGB(0,0,0) },
	{ 1,"Black",RGB(0,0,0) },
	{ 2,"Red",RGB(255,0,0) },
	{ 3,"Green",RGB(0,255,0) },
	{ 4,"Blue",RGB(0,0,255) },
	{ 5,"Yellow",RGB(255,255,0) },
	{ 6,"White",RGB(255,255,255) },
	{ 7,"Grey",RGB(128,128,128) }
};


AttackProfile AttackProfileList[4] = {
	{ 1, "Level" },
	{ 2, "Pop-Up" },
	{ 3, "Loft" },
	{ 4, "Dive" }
};

// Events
MissionFlag MissionFlagList[42] = 
{
	{ USER_FLAG_START, "USER FLAG 1",1 },
	{ USER_FLAG_START+1, "USER FLAG 2",1 },
	{ USER_FLAG_START+2, "USER FLAG 3",1 },
	{ USER_FLAG_START+3, "USER FLAG 4",1 },
	{ USER_FLAG_START+4, "USER FLAG 5",1 },
	{ USER_FLAG_START+5, "USER FLAG 6",1 },
	{ USER_FLAG_START+6, "USER FLAG 7",1 },
	{ USER_FLAG_START+7, "USER FLAG 8",1 },
	{ USER_FLAG_START+8, "USER FLAG 9",1 },
	{ USER_FLAG_START+9, "USER FLAG 10",1 },
	{ USER_FLAG_START+10, "USER FLAG 11",1 },
	{ USER_FLAG_START+11, "USER FLAG 12",1 },
	{ USER_FLAG_START+12, "USER FLAG 13",1 },
	{ USER_FLAG_START+13, "USER FLAG 14",1 },
	{ USER_FLAG_START+14, "USER FLAG 15",1 },
	{ USER_FLAG_START+15, "USER FLAG 16",1 },
	{ USER_FLAG_START+16, "USER FLAG 17",1 },
	{ USER_FLAG_START+17, "USER FLAG 18",1 },
	{ USER_FLAG_START+18, "USER FLAG 19",1 },
	{ USER_FLAG_START+19, "USER FLAG 20",1 },
	{ USER_FLAG_START+20, "USER FLAG 21",1 },
	{ USER_FLAG_START+21, "USER FLAG 22",1 },
	{ USER_FLAG_START+22, "USER FLAG 23",1 },
	{ USER_FLAG_START+23, "USER FLAG 24",1 },
	{ USER_FLAG_START+24, "USER FLAG 25",1 },
	{ USER_FLAG_START+25, "USER FLAG 26",1 },
	{ USER_FLAG_START+26, "USER FLAG 27",1 },
	{ USER_FLAG_START+27, "USER FLAG 28",1 },
	{ USER_FLAG_START+28, "USER FLAG 29",1 },
	{ USER_FLAG_START+29, "USER FLAG 30",1 },
	{ MISSION_FLAG_START, "OUT OF A/G", 0 },
	{ MISSION_FLAG_START+1, "OUT OF A/A", 0 },
	{ MISSION_FLAG_START+2, "FLIGHT 1 (PLAYER) DESTROYED", 0 },
	{ MISSION_FLAG_START+3, "FLIGHT 2 (WINGMAN) DESTROYED", 0, },
	{ MISSION_FLAG_START+4, "FLIGHT 3 DESTROYED", 0, },
	{ MISSION_FLAG_START+5, "FLIGHT 4 DESTROYED", 0, },
	{ MISSION_FLAG_START+6, "FLIGHT 5 DESTROYED", 0, },
	{ MISSION_FLAG_START+7, "FLIGHT 6 DESTROYED", 0, },
	{ MISSION_FLAG_START+8, "FLIGHT 7 DESTROYED", 0, },
	{ MISSION_FLAG_START+9, "FLIGHT 8 DESTROYED", 0, },
	{ MISSION_FLAG_START+10, "CROSSED FENCE", 0, },
	{ MISSION_FLAG_START+11, "FLIGHT 1 (PLAYER) LANDED", 0, },
};


EventCondOperator EventCondOperatorList[2] =
{
	{ 1, "=" },
	{ 0, "!=" }
};

EventCondValue EventCondValueList[2] = 
{
	{ 1, "TRUE" },
	{ 0, "FALSE" }
};

DebriefItemOperator DebriefItemOperatorList[6] =
{
	{ 0,"!=" },
	{ 1,"=" },
	{ 2,"<" },
	{ 3,">" },
	{ 4,"<=" },
	{ 5,">=" },
};


AreaGoalType AreaGoal;
AreaGoalType TempAreaEvent;
MPoint BullsEye = { 0,0 };

int g_iNumGroups=0;


int g_iNumEvents=0;

int g_iAreaGoal=0;
int g_iTempAreaEvent=0;

int g_iCurrentObject=-1;
int g_iObjectType=-1;
int g_iState=-1;
int	g_iCurrentWayPoint=-1;

int g_iEscortMode=0;
int g_iFormOnMode=0;
int g_iBombMode=0;

int g_iInitActions=-1;

int g_iLanguageId=1033;

int iNumMovingVehicles=0;
int iNumGroundObjects=0;

int iNumSpecialGroundObjects=0;

int iNumGoals=0;
int iNumEvents=0;

int g_iJSTARDesignation=0;


int	iNumAIObjs = 0;					//  Identifies total number of objects currently being used by mission.
int iNumStaticObjs = 0;				//  Identifies total number of plug-ins currently being used by mission.
int iNumWPPaths = 0;				//  Identifies total number of paths currently being used by mission.
int iNumWayPts = 0;					//  Identifies total number of waypoints currently being used by mission.
int iNumActions = 0;				//  Identifies total number of actions currently being used by mission.
long lLastMoveID = 0;				//  Used to hold a starting point for creating a new ID for a plane.

int  iMapScreenMaxX;	// was 640	//  Holds the displayed map area width.
int  iMapScreenMaxZ;	// was 480	//  Holds the displayed map area height.

long lMapBoxX0 = 0;					//  When creating a dragged zoom box, this holds the screen X position where the mouse was first clicked to create the box.
long lMapBoxZ0 = 0;					//  When creating a dragged zoom box, this holds the screen Z position where the mouse was first clicked to create the box.
long lMBWorldID = 1;				//  This holds the world ID of the last mission loaded, so we know if we have to reload/draw the terrain on screen when loading/creating a different mission.

char sWorldStr[260];					//  This string holds the name of the world file which is to be loaded (from regit).
char sWorldPath[260];				//  This string holds the path of where the world file is loaded (from regit).
long lMBRDX0;						//  This holds the left edge coordinate of the displayed map when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
long lMBRDZ0;						//  This holds the top edge coordinate of the displayed map	when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
long lMBRDX1;						//  This holds the right edge coordinate of the displayed map when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
long lMBRDZ1;						//  This holds the bottom edge coordinate of the displayed map when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
int  iRDMapScreenMaxX;  // 640		//  This holds the displayed map area width, in screen coordinates, when moving/zooming the map.  This is so we know if the scroll bars were up before modifying the map so original trasitional size can be represented.
int  iRDMapScreenMaxZ;  // 480		//  This holds the displayed map area width, in screen coordinates, when moving/zooming the map.  This is so we know if the scroll bars were up before modifying the map so original trasitional size can be represented.


CallSignType CallSignList[MAX_NUM_CALLSIGN] =
{
	{ 0,   0,	"Cobra",		872, 942},
	{ 0,   0,	"Python",		873, 943},
	{ 0,   0,	"Boa",			874, 944},
	{ 0,   0,	"Rattler",		875, 945},
	{ 0,   0,	"Mercury",		876, 946},
	{ 0,   0,	"Venus",		877, 947},
	{ 0,   0,	"Mars",			878, 948},
	{ 0,   0,	"Jupiter",		879, 949},
	{ 0,   0,	"Saturn",		880, 950},
	{ 0,   0,	"Neptune",		881, 951},
	{ 0,   0,	"Pluto",		882, 952},
	{ 0,   0,	"Shark",		883, 953},
	{ 0,   0,	"Ray",			884, 954},
	{ 0,   0,	"Skate",		885, 955},
	{ 0,   0,	"Manta",		886, 956},
	{ 0,   0,	"Tuna",			887, 957},
	{ 0,   0,	"Flounder",		888, 958},
	{ 0,   0,	"Mackerel",		889, 959},
	{ 0,   0,	"Striper",		890, 960},
	{ 0,   0,	"Jay",			891, 961},
	{ 0,   0,	"Sparrow",		892, 962},
	{ 0,   0,	"Starling",		893, 963},
	{ 0,   0,	"Raven",		894, 964},
	{ 0,   0,	"Swift",		895, 965},
	{ 0,   0,	"Robin",		896, 966},
	{ 0,   0,	"Panhead",		897, 967},
	{ 0,   0,	"Knucklehead",	898, 968},
	{ 0,   0,	"Shovelhead",	899, 969},
	{ 0,   0,	"Alpha",		901, 971},
	{ 0,   0,	"Bravo",		902, 972},
	{ 0,   0,	"Charlie",		903, 973},
	{ 0,   0,	"Delta",		904, 974},
	{ 0,   0,	"Echo",			905, 975},
	{ 0,   0,	"Foxtrot",		906, 976},
	{ 0,   0,	"Golf",			907, 977},
	{ 0,   0,	"Hotel",		908, 978},
	{ 0,   0,	"India",		909, 979},
	{ 0,   0,	"Juliet",		910, 980},
	{ 0,   0,	"Kilo",			911, 981},
	{ 0,   0,	"Lima",			912, 982},
	{ 0,   0,	"Mike",			913, 983},
	{ 0,   0,	"November",		914, 984},
	{ 0,   0,	"Oscar",		915, 985},
	{ 0,   0,	"Papa",			916, 986},
	{ 0,   0,	"Quebec",		917, 987},
	{ 0,   0,	"Romeo",		918, 988},
	{ 0,   0,	"Sierra",		919, 989},
	{ 0,   0,	"Tango",		920, 990},
	{ 0,   0,	"Uniform",		921, 991},
	{ 0,   0,	"Violet",		922, 992},
	{ 0,   0,	"Winchester",	923, 993},
	{ 0,   0,	"X-ray",		924, 994},
	{ 0,   0,	"Zulu",			925, 995},
	{ 0,   0,	"Blade",		926, 996},
	{ 0,   0,	"Slasher",		927, 997},
	{ 0,   0,	"Hacksaw",		928, 998},
	{ 0,   0,	"Hachet",		929, 999},
	{ 1,   0,	"Sandy",		900, 970},
};									  


#if 0
CallSignType CallSignList[MAX_NUM_CALLSIGN] =
{
	{ CALLSIGN_AIRCRAFT_F18E,0,"Chevy", 800, 968},
	{ CALLSIGN_AIRCRAFT_F18E,1,"Dodge", 801, 969},
	{ CALLSIGN_AIRCRAFT_F18E,2,"T-Bird", 802, 970},
	{ CALLSIGN_AIRCRAFT_F18E,3,"Packard", 803, 971},
	{ CALLSIGN_AIRCRAFT_F18E,4,"Edsel", 804, 972},
	{ CALLSIGN_AIRCRAFT_F18E,5,"Firebird",805, 973},
	{ CALLSIGN_AIRCRAFT_F18E,6,"Buick", 806, 974},
	{ CALLSIGN_AIRCRAFT_F18E,7,"Stingray", 807, 975},
	{ CALLSIGN_AIRCRAFT_F18E,8,"Charger", 808, 976},
	{ CALLSIGN_AIRCRAFT_F18E,9,"Outlaw", 809, 977},
	{ CALLSIGN_AIRCRAFT_F18E,10,"Deputy", 810, 978},
	{ CALLSIGN_AIRCRAFT_F18E,11,"Sheriff", 811, 979},
	{ CALLSIGN_AIRCRAFT_SAR,0,"Sandy", 919, 619},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,0,"Rhino", 932, 980},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,1,"Buffalo", 933, 981},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,2,"Boar", 934, 982},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,3,"Exxon", 935, 983},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,4,"Crystal Palace", 920, 620},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,5,"Ivory Tower", 921, 621},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,6,"Merlin", 908, 608},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,7,"Warlock", 909, 609},
	{ CALLSIGN_AIRCRAFT_STRATEGIC,8,"Bandsaw", 910, 610},
	{ CALLSIGN_AIRCRAFT_OTHER,0,"Citgo", 911, 611},
	{ CALLSIGN_AIRCRAFT_OTHER,1,"Pennzoil", 912, 612},
	{ CALLSIGN_AIRCRAFT_OTHER,2,"Zerex", 913, 613},
	{ CALLSIGN_AIRCRAFT_OTHER,3,"Gulf", 914, 614},
	{ CALLSIGN_AIRCRAFT_OTHER,4,"Shell", 915, 615},
	{ CALLSIGN_AIRCRAFT_OTHER,5,"Talon", 916, 616},
	{ CALLSIGN_AIRCRAFT_OTHER,6,"Starfighter", 917, 617},
	{ CALLSIGN_AIRCRAFT_OTHER,7,"Comet", 918, 618},
	{ CALLSIGN_AIRCRAFT_OTHER,8,"Cortland", 922, 622},
	{ CALLSIGN_AIRCRAFT_OTHER,9,"Remington", 923, 623},
	{ CALLSIGN_AIRCRAFT_OTHER,10,"Medusa", 924, 624},
	{ CALLSIGN_AIRCRAFT_OTHER,11,"Zeus", 925, 625},
	{ CALLSIGN_AIRCRAFT_OTHER,12,"Lion", 951, 651},
	{ CALLSIGN_AIRCRAFT_OTHER,13,"Tiger", 952, 652},
	{ CALLSIGN_AIRCRAFT_OTHER,14,"Bear", 953, 653},
	{ CALLSIGN_AIRCRAFT_OTHER,15,"Miller", 954, 654},
	{ CALLSIGN_AIRCRAFT_OTHER,16,"Budweiser", 955, 655},
	{ CALLSIGN_AIRCRAFT_OTHER,17,"Michelob", 956, 656},
	{ CALLSIGN_AIRCRAFT_OTHER,18,"Oiler", 957, 657},
	{ CALLSIGN_AIRCRAFT_OTHER,19,"Giant", 958, 658},
	{ CALLSIGN_AIRCRAFT_OTHER,20,"Raven", 959, 658},
	{ CALLSIGN_AIRCRAFT_OTHER,21,"Nova", 960, 660},
	{ CALLSIGN_AIRCRAFT_OTHER,22,"Scorpian", 961, 661},
	{ CALLSIGN_AIRCRAFT_OTHER,23,"Tusk", 962, 662},
	{ CALLSIGN_AIRCRAFT_OTHER,24,"Snout", 963, 663},
	{ CALLSIGN_AIRCRAFT_OTHER,25,"Sword", 964, 664},
	{ CALLSIGN_AIRCRAFT_OTHER,26,"Rapier", 965, 665},
	{ CALLSIGN_AIRCRAFT_OTHER,27,"Dagger", 966, 666},
	{ CALLSIGN_AIRCRAFT_OTHER,28,"Crossbow", 967, 667},
	{ CALLSIGN_GROUNDCONTROL_TOWER,0,"Tower", 926, 926},
	{ CALLSIGN_GROUNDCONTROL_TOWER,1,"Al Kharj Tower", 927, 927},
	{ CALLSIGN_GROUNDCONTROL_TOWER,2,"King Fahd Tower", 928, 928},
	{ CALLSIGN_GROUNDCONTROL_TOWER,3,"Tabuk Tower", 929, 929},
	{ CALLSIGN_GROUNDCONTROL_TOWER,4,"King Khalid Tower", 930, 930},
	{ CALLSIGN_GROUNDCONTROL_TOWER,5,"Bahrain International Tower", 931, 931},
	{ CALLSIGN_GROUNDUNIT_USFAC,0,"Mike four seven", 936, 936},
	{ CALLSIGN_GROUNDUNIT_USFAC,1,"Zulu eight three", 937, 937},
	{ CALLSIGN_GROUNDUNIT_USFAC,2,"Victor six one", 938, 938},
	{ CALLSIGN_GROUNDUNIT_USFAC,3,"Alpha five seven", 939, 939},
	{ CALLSIGN_GROUNDUNIT_USFAC,4,"Sierra four three", 940, 940},
	{ CALLSIGN_GROUNDUNIT_USFAC,5,"Tango nine five", 941, 941},
	{ CALLSIGN_GROUNDUNIT_USFAC,6,"Juliet eight seven", 942, 942},
	{ CALLSIGN_GROUNDUNIT_USFAC,7,"India five three", 943, 943},
	{ CALLSIGN_GROUNDUNIT_USFAC,8,"Oscar zero zero", 944, 944},
	{ CALLSIGN_GROUNDUNIT_UKFAC,0,"Romeo one two", 945, 945},
	{ CALLSIGN_GROUNDUNIT_UKFAC,1,"Echo one one", 946, 946},
	{ CALLSIGN_GROUNDUNIT_UKFAC,2,"Whiskey three four", 947, 947},
	{ CALLSIGN_GROUNDUNIT_UKFAC,3,"Yankee three three", 948, 948},
	{ CALLSIGN_GROUNDUNIT_UKFAC,4,"Victor nine three", 949, 949},
	{ CALLSIGN_GROUNDUNIT_UKFAC,5,"Delta one four", 950, 950}
};
#endif



int g_iNumEyePoints=0;
EyePointActionType EyePointList[MAX_NUM_EYEPOINTS];

#else


//**************************************************************************************

extern MissionFlowControl BuilderFlowControl;
extern BriefingSummaryType BuilderBriefingSummary;
extern MissionFileSummary BuilderMissionSummary;

extern MissionFlowControl g_PreviousBuilderFlowControl;
extern BriefingSummaryType g_PreviousBuilderBriefingSummary;
extern MissionFileSummary g_PreviousBuilderMissionSummary;

extern LocationData *g_pLocationList;
extern int g_iNumLocations;



extern AreaZoomType AreaZoom;
extern int g_iNumMissionLabels;
extern MissionLabelType *pMissionLabelList;


extern ApplyCommandsTypeV0 *pApplyCommandsListV0;
extern ApplyCommandsTypeV1 *pApplyCommandsList;
extern int g_iNumApplyCommands;


extern GroundObject *pGroundObjectList;

extern FACType FACObject;


extern int g_iNumEyePoints;
extern EyePointActionType EyePointList[MAX_NUM_EYEPOINTS];


extern int g_iNumJSTARObjects;
extern int g_iNumTACANObjects;

extern TACANType *pTACANTypeList;
extern JSTARType JSTARList;

extern int g_iNumFenceArea;
extern FenceTypeV1 FenceArea[5];

extern MissionFlagType MissionFlagTypeList[61];

extern DebriefType DebriefData;

extern DebriefCondValue DebriefCondValueList[2];
extern DebriefItemOperator DebriefItemOperatorList[6];

extern GroupType GroupList[MAX_GROUPS];
extern MissionFlag MissionFlagList[42];


extern EventCondOperator EventCondOperatorList[2];
extern EventCondValue EventCondValueList[2];

extern MissionIdTagType MissionIdTag;
extern EditorInfoType EditorInfo;
extern MissionInfoType MissionInfo;

extern AreaGoalType AreaGoal;
extern AreaGoalType TempAreaEvent;
extern MPoint BullsEye;


extern int g_iNumGroups;
extern int g_iNumEvents;
extern int g_iAreaGoal;
extern int g_iTempAreaEvent;

extern int g_iInitActions;

extern int g_iEscortMode;
extern int g_iFormOnMode;
extern int g_iBombMode;

extern int g_iCurrentObject;
extern int g_iObjectType;
extern int g_iState;
extern int g_iCurrentWayPoint;

extern int g_iLanguageId;

extern int g_iJSTARDesignation;

extern int iNumMovingVehicles;
extern int iNumGroundObjects;

extern int iNumSpecialGroundObjects;

extern int iNumGoals;

extern MBEvents	EventList[NUMEVENTS];

extern MovingVehicle	*g_pMovingVehicleList;
extern int g_iNumMovingVehicle;


extern ActionType ActionTypeList[NUM_ACTION_TYPE];
extern SidesType SidesList[3];
extern CallSignType CallSignList[MAX_NUM_CALLSIGN];
extern DirectionType DirectionList[8];
extern ColorType ColorList[14];
extern ColorType ColorBackList[8];
extern DurationType DurationList[2];
extern WhenType WhenList[1];
extern Formation FormationList[8];
extern OrbitUntil OrbitUntilList[4];
extern ControlType ControlList[2];
extern AttackProfile AttackProfileList[4];

extern GoalType GoalTypeList[3];
extern GoalType MissionGoalTypeList[3];

extern int iNumAIObjs;
extern int iNumStaticObjs;
extern int iNumWPPaths;
extern int iNumWayPts;
extern int iNumActions;
extern long lLastMoveID;
extern MBObject	AIObjects[MAXAIOBJS];
extern MBWPPaths AIWPPaths[MAXAIOBJS];
extern MBWayPoints AIWayPoints[MAXWPTS];
extern MBActions	AIActions[MAXACTS];
extern long lMBX0;
extern long lMBZ0;
extern long lMBX1;
extern long lMBZ1;
extern long lMBMapStartX0;
extern long lMBMapStartZ0;
extern long lMBMapStartX1;
extern long lMBMapStartZ1;
extern int  iMapScreenOrgX;
extern int  iMapScreenOrgZ;
extern int  iMapScreenMaxX;
extern int  iMapScreenMaxZ;
extern long lMapMaxX;
extern long lMapMaxZ;
extern long lMapBoxX0;
extern long lMapBoxZ0;
extern MBGoalInfo GoalList[NUMGOALS];
extern long lMBWorldID;
extern char sWorldStr[260];
extern char sWorldPath[260];
extern long lMBRDX0;
extern long lMBRDZ0;
extern long lMBRDX1;
extern long lMBRDZ1;
extern int  iRDMapScreenMaxX;  // 640
extern int  iRDMapScreenMaxZ;  // 480

#endif

//**************************************************************************************

int LoadChunkMissionFile(char *sFileName);
int SaveChunkMissionFile(char *sFileName,int iLocation=0);

void SaveMissionFileSummary(char *sFileName,MissionFileSummary *MissionSummaryData);
int LoadMissionFileSummary(char *sFileName,MissionFileSummary *LoadMissionSummaryData);
void FreeMissionFileSummary(MissionFileSummary *MissionSummaryData);


int SaveMissionFile(char *sFileName,int iLocation=SAVE_BRIEF);
int LoadMissionFile(char *sFileName);
void CheckAddGroupPlaneEffects(int leadplane, int numadd);
long MBGetNextMovingID();
void CheckRemoveGroupPlaneEffects(int leadplane, int numremove);
void AddWayPoint(long lWPx, long lWPz, int iNewWpt);
void DeleteWayPoint(int iOldWpt);
void DeleteWaypoints(int startwp, int numwp);
void MBScreenToWorldConvert(int startx, int startz, long *endx, long *endz);
void MBWorldToScreenConvert(long startx, long startz, int *endx, int *endz);
void MBZoomMap(void);
void MBUnZoomMap(void);
void MBMapLeft(void);
void MBMapRight(void);
void MBMapUp(void);
void MBMapDown(void);
void RemoveGroupPlanes(int leadplane, int numremove);
void AIRemovePlanePaths(int planenum);
void AIRemovePath(int pathnum);
void MBSetUpMissionWorld(long worldid, int lastworldid);
void AddNewObjectGroup(int startwp, int numwps);

void MBOldWorldToScreenConvert(long startx, long startz, int *endx, int *endz);

int AddAction(int iNewAct, int numacts, int actid, int nowpactions);

int DeleteAction(int iWayPoint,int iAction);
int AddAction(int iWayPoint,MBActions TmpAction);
int GetCurrentAIObject();
int GetWhoAmIFromAction(int iAction);
void UpdateActions(int iNewWayPoint,int iMode);
int GetHeadingSelection(int iHeading);
int GetTimerSelection(int iDuration);
int GetControlSelection(int iControl);
BOOL LANGGetTransMessage(char *pszBuffer, DWORD maxbuffsize, DWORD msgid, DWORD languageid, ... );
void LoadWorldObjects();
void CleanUpMissionFileData();
int LoadTitleInfo(char *sFileName,char **sMissionTitle,char **sMissionSummary);
void LogError(char *sErrorDesc, char *sFile, int iLine, int iTerminate);
int GetSelectType(int iValue);
int LoadMissionVersion100(char *sFileName);
int LoadMissionVersion101(char *sFileName);
int LoadMissionVersion200(char *sFileName);
int LoadMissionVersion201(char *sFileName);
extern void InitDB();
extern void FreeDB();
void ModifyNumberInFlight(int iPlaneNum, int iNumInFlight);
int GetPlayerIndex();
void ScreenToWorldConvert(int startx, int startz, long *endx, long *endz);
int GetWayPointOwner(int iWayPoint);
int GetOperator(int iId);
int GetDebriefValue(int iId);
int GetGoalSelection(int iType, int iID);
int VerifyVersion(char *sFileName);
void CleanUpMissionData();
void EvaluateMission();
BOOL Targeting(int);
void LogMissionStatistic(int iMissionFlagId, int iFlight,int iValue, int iOperator);
int ApplyCommandsFromFile(char *sFileName);
int ApplyCommandsToFile(char *sFileName);
void ApplyCommandsToMission();
void MBCleanUpMissionData();

int MBPlayWAVFile(char *psFileName);






