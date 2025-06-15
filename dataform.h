// dataform.h - Data Format

#ifndef __DATAFORM_H__

#define __DATAFORM_H__


#define AIRCRAFT_DAM_F18_HULK		1
#define AIRCRAFT_DAM_ALLIED_HULK	2
#define AIRCRAFT_DAM_ENEMY_HULK		3
#define AIRCRAFT_DAM_HELO_HULK		4

#define AIRCRAFT_DAM_BUE2			5
#define AIRCRAFT_DAM_BUEA6			6
#define AIRCRAFT_DAM_BUF14			7
#define AIRCRAFT_DAM_BUMIG25		8
#define AIRCRAFT_DAM_BUMIG29		9
#define AIRCRAFT_DAM_BUS3			10
#define AIRCRAFT_DAM_BUSU24			11
#define AIRCRAFT_DAM_BUSU25			12
#define AIRCRAFT_DAM_BUSU27			13
#define AIRCRAFT_DAM_BUSU35			14
#define AIRCRAFT_DAM_BUV22			15



#define AIRCRAFT_TYPE_FIGHTER		1
#define AIRCRAFT_TYPE_BOMBER		2
#define AIRCRAFT_TYPE_TRANSPORT		4
#define AIRCRAFT_TYPE_TANKER		8
#define AIRCRAFT_TYPE_C3			16
#define AIRCRAFT_TYPE_HELICOPTER	32
#define AIRCRAFT_TYPE_CIVILIAN		64
#define AIRCRAFT_TYPE_VERTICAL		128
#define AIRCRAFT_TYPE_CARRIER_CAP	256

#define SHIP_TYPE_CARRIER			1
#define SHIP_TYPE_CRUISER			2
#define SHIP_TYPE_CARGO				4
#define SHIP_TYPE_DESTROYER			8
#define SHIP_TYPE_FASTATTACK		16
#define SHIP_TYPE_FRIGATE			32
#define SHIP_TYPE_LANDING			64
#define SHIP_TYPE_LUXARY			128
#define SHIP_TYPE_SUBMARINE			256

#define	SHIP_SIZE_VSMALL			1
#define SHIP_SIZE_SMALL				2
#define SHIP_SIZE_MEDIUM			4
#define SHIP_SIZE_LARGE				8
#define SHIP_SIZE_HUGE				16

#define ECM_CHAFF		1
#define ECM_FLARE		2
#define ECM_JAMMING		4
#define ECM_DECOY		8

#define RADAR_TYPE_STT		1
#define RADAR_TYPE_TWS		2
#define RADAR_TYPE_LPRF		4
#define	RADAR_TYPE_MPRF		8
#define RADAR_TYPE_HPRF		16
#define	RADAR_TYPE_CW		32
#define RADAR_TYPE_OPTICAL	64
#define RADAR_TYPE_IRST		128
#define RADAR_TYPE_NCTR		256

#define VEHICLE_TYPE_CAR		1
#define	VEHICLE_TYPE_TRUCK		2
#define VEHICLE_TYPE_TANK		4
#define VEHICLE_TYPE_SHIP		8
#define VEHICLE_TYPE_AIRCRAFT	16
#define	VEHICLE_TYPE_OTHER		32

#define	ARMOR_TYPE_SOFT			1
#define	ARMOR_TYPE_NORMAL		2
#define	ARMOR_TYPE_ARMORED		4

#define VEHICLE_SPEED_VSLOW		1
#define VEHICLE_SPEED_SLOW		2
#define VEHICLE_SPEED_NORMAL	4
#define VEHICLE_SPEED_FAST		8
#define VEHICLE_SPEED_VFAST		16

#define	FIRE_TYPE_TRUCK			1
#define FIRE_TYPE_HOUSE			2
#define FIRE_TYPE_FUELBLADDER	4
#define FIRE_TYPE_GASTANKER		8

#define	VEHICLE_SPEED_VSLOW		1
//#define VEHICLE_SPEED_

#define	NUM_DEFENSE_LIST_ITEMS	18


#define RADAR_TYPE_PULSE_RATE (RADAR_TYPE_LPRF | RADAR_TYPE_MPRF | RADAR_TYPE_HPRF | RADAR_TYPE_CW)

#define MAX_NUM_AIRCRAFT	150
#define MAX_NUM_RADAR		150
#define MAX_NUM_LOADOUT		250
#define MAX_NUM_WEAPON		250
#define MAX_NUM_VEHICLE		150
#define MAX_NUM_SHIP		100

#define	MAX_NUMLOADOUT_STATIONS		20
#define	MAX_NUM_LOADOUTS_ENTRIES	30

#define WEAPON_TYPE_DUMB_BOMB					0
#define WEAPON_TYPE_GUIDED_BOMB                 1
#define WEAPON_TYPE_AA_SHORT_RANGE_MISSILE		2
#define WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE		3
#define WEAPON_TYPE_AA_LONG_RANGE_MISSILE		4
#define WEAPON_TYPE_AG_MISSILE					5
#define WEAPON_TYPE_GUN                         6
#define WEAPON_TYPE_SCUD                        7
#define WEAPON_TYPE_SAM                         8
#define WEAPON_TYPE_FLARE                       9
#define WEAPON_TYPE_CHAFF                       10
#define WEAPON_TYPE_CLUSTER_BOMB                11
#define WEAPON_TYPE_FUEL_TANK                   12
#define WEAPON_TYPE_CHAFF_FLARE_DISP			13
#define WEAPON_TYPE_STANDOFF_JAMMER				14
#define WEAPON_TYPE_IJAM_NOISE					15
#define WEAPON_TYPE_DATALINK					16
#define WEAPON_TYPE_ANTI_SHIP_MISSILE			17
#define WEAPON_TYPE_FUEL_AIR_EXPLOSIVE			18
#define WEAPON_TYPE_NAPALM						19
#define WEAPON_TYPE_AA_ANTIRADAR_MISSILE		20
#define WEAPON_TYPE_DECOY						21
#define WEAPON_TYPE_NAVAL_MINE					22
#define WEAPON_TYPE_AGROCKET					23
#define WEAPIN_TYPE_IJAM_DECEPT					24


#define WARHEAD_STANDARD_TINY		0
#define WARHEAD_STANDARD_SMALL		1
#define WARHEAD_STANDARD_MEDIUM		2
#define WARHEAD_STANDARD_LARGE		3
#define WARHEAD_STANDARD_HUGE		4
#define WARHEAD_ANTIARMOR_TINY		5
#define WARHEAD_ANTIARMOR_SMALL		6
#define WARHEAD_ANTI_PERSONNEL		7
#define WARHEAD_ICENDIARY			8
#define WARHEAD_PENETRATION			9
#define WARHEAD_CRATERING			10
#define WARHEAD_CHEMICAL			11
#define WARHEAD_NUKE				12
#define WARHEAD_SAM_LARGE			13
#define WARHEAD_SAM_MEDIUM			14
#define WARHEAD_SAM_SMALL			15
#define WARHEAD_SAM_TINY			16
#define WARHEAD_AAA_LARGE			17
#define WARHEAD_AAA_MEDIUM			18
#define WARHEAD_AAA_SMALL			19
#define WARHEAD_AAA_TINY			20
#define WARHEAD_FUEL_AIR_EXPLOSIVE	21
#define WARHEAD_NAPALM				22
#define WARHEAD_SMOKE				23

// Planes

struct stOffSet
{
	float X;
	float Y;
	float Z;
};

typedef struct stOffSet FPOffSet;

struct stDataBaseIdTag
{
	char sTag[4];
	long lDataBaseFormatIdTag;
	long lVersion;
	long lSubVersion;
};

typedef struct stDataBaseIdTag DataBaseIdTagType;

//*******************************************************************
//	stDBAircraft
//
//  OffSet values:
//		0	Wing Left
//		1	Wing Right
//		2	Cockpit Front
//		3	Cockpit Rear
//		4	Engine Rear Left / Engine Rear
//		5	Engine Rear Right / Wing Left Outboard
//		6	Engine Wing Left Inboard
//		7	Engine Wing Right Outboard
//		8	Engine Wing Right Inboard
//		9	Burning
//		10	Gun
//		11  Gun (for AC-130 only)
//		12  Gun (for AC-130 only)

struct stDBAircraftOld
{
	long lPlaneID;
	char sName[64];
	long lType;
	int iSide;	// Default
	long lRadarID;
	int iRadarCrossSignature;			// Front RCS
	int iMaxSpeed;
	int iStallSpeed;
	int iAcceleration;
	int iMaxYawRate;
	int iYawDeceleration;
	int iWeight;
	int iFuel;
	int iRange;
	int iAfterBurn;
	int iCeiling;
	int iDamageDefense;
	int iVisibility;
	long lLoadOutId;
	long lLoadOutIndex;
	char sFileName[32];
	char sObjectName[32];
	char sShadowName[32];
	int iEngineConfig;
	FPOffSet OffSet[20];  //  see comments above for what item is what
	float fShadowULXOff;
	float fShadowULZOff;
	float fShadowLRXOff;
	float fShadowLRZOff;
	float fGearUpHeight;
	float fGearDownHeight;
	float fTailHeight;
	int iDamageValue;
	int iDamageRadius;
	float fReserved1;
	float fReserved2;
	long lDestroyModel;
	long lReserved2;
};

typedef struct stDBAircraftOld DBAircraftTypeOld;

struct stDBAircraft
{
	long lPlaneID;
	char sName[64];
	long lType;
	int iSide;	// Default
	long lRadarID;
	int iRadarCrossSignature;			// Front RCS
	int iMaxSpeed;
	int iStallSpeed;
	int iAcceleration;
	int iMaxYawRate;
	int iYawDeceleration;
	int iWeight;
	int iFuel;
	int iRange;
	int iAfterBurn;
	int iCeiling;
	int iDamageDefense;
	int iVisibility;
	long lLoadOutId;
	long lLoadOutIndex;
	char sFileName[32];
	char sObjectName[32];
	char sShadowName[32];
	int iEngineConfig;
	FPOffSet OffSet[20];  //  see comments above for what item is what
	float fShadowULXOff;
	float fShadowULZOff;
	float fShadowLRXOff;
	float fShadowLRZOff;
	float fGearUpHeight;
	float fGearDownHeight;
	float fTailHeight;
	int iDamageValue;
	int iDamageRadius;
	float fReserved1;
	float fReserved2;
	long lDestroyModel;
	long lReserved2;
	int iMaxAlpha;
	int iMaxSust;
	int iMaxInst;
	int iFuelFlow;
	int iSideRCS;
	int iMBFlags;
	int iReserved[5];
	char sNCTR[20];
};

typedef struct stDBAircraft DBAircraftType;



struct stDBVehicleOld
{
	long lVehicleID;
	char sName[64];
	long lVehicleType;
	long lRadarID;
	int iRadarCrossSignature;
	int iSpeed;
	int iMovement;
	char sFileName[32];
	char sBlownObjectFileName[32];
	int iSustainedDamage;
	int iPermanentDamage;
	int iArmorType;
	long lWeaponType;
	int iWeaponCount;
	long lExpWeaponType;
	int iNumExplosions;
	int iRadius;
	int iTicksBetweenBlasts;
	int iVarianceBetweenBlasts;
	int iFireType;
	int iFireDuration;
	long lLoadOutId;
	long lLoadOutIndex;
	int iMBFlags;
	float fReserved[6];
	long lReserved[6];
};

typedef struct stDBVehicleOld DBVehicleTypeOld;

struct stDBVehicle
{
	long lVehicleID;
	char sName[64];
	long lVehicleType;
	long lRadarID;
	int iRadarCrossSignature;
	int iSpeed;
	int iMovement;
	char sFileName[32];
	char sBlownObjectFileName[32];
	int iSustainedDamage;
	int iPermanentDamage;
	int iArmorType;
	long lWeaponType;
	int iWeaponCount;
	long lExpWeaponType;
	int iNumExplosions;
	int iRadius;
	int iTicksBetweenBlasts;
	int iVarianceBetweenBlasts;
	int iFireType;
	int iFireDuration;
	long lLoadOutId;
	long lLoadOutIndex;
	int iMBFlags;
	float fReserved[6];
	long lWeaponType2;
	long lWeaponCount2;
	long lReserved[4];
};

typedef struct stDBVehicle DBVehicleType;


struct stDBRadarOld
{
	long lRadarID;
	char sName[64];
	int iTEWSType;
	int iRadarType;
	int iMaxRange;
	int iSearchAzimuth;
	int iSearchElevation;
	int iSearchRate;
	int iHorizontalSlew;
	int iVerticalSlew;
	int iECMResistance;
	int iGeneration;
	int iNumberOfTracks;
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
};

typedef struct stDBRadarOld DBRadarTypeOld;

struct stDBRadar
{
	long lRadarID;
	char sName[64];
	int iTEWSType;
	int iRadarType;
	int iMaxRange;
	int iSearchAzimuth;
	int iSearchElevation;
	int iSearchRate;
	int iHorizontalSlew;
	int iVerticalSlew;
	int iECMResistance;
	int iGeneration;
	int iNumberOfTracks;
	long lMinElevation;
	long lReserved2;
	long lReserved3;
	long lReserved4;
	char sNTEWS[10];
};

typedef struct stDBRadar DBRadarType;


struct stDBWeaponOld
{
	long lWeaponID;
	char sName[64];
	char sModel[32];
	int iUsageIndex;
	char sMPDAbbrev[16];
	char sObjectFileName[32];
	int iWeaponType;
	int iWarHead;
	int iSeekerType;
	int iRocketMotor;
	int iTEWSType;
	int iDamageValue;
	int iDamageRadius;
	int iWeight;
	float fDrag;
	float fAircraftDrag;
	float fMaxSpeed;
	int iBurnTime;
	int iMaxAlt;
	int iRange;
	int iMaxYawRate;
	int iYawDeceleration;
	int iAcceleration;
	int iRateOfFire;
	int iBurstRate;
	int iECMResistance;

	float fFlameXOffSet;
	float fFlameYOffSet;
	float fFlameZOffSet;
	float fMinRange;
	float fReserved2;
	long lNumberOfSubWeapon;
	long lSubWeaponID;
};

typedef struct stDBWeaponOld DBWeaponTypeOld;

struct stDBWeapon
{
	long lWeaponID;
	char sName[64];
	char sModel[32];
	int iUsageIndex;
	char sMPDAbbrev[16];
	char sObjectFileName[32];
	int iWeaponType;
	int iWarHead;
	int iSeekerType;
	int iRocketMotor;
	int iTEWSType;
	int iDamageValue;
	int iDamageRadius;
	int iWeight;
	float fDrag;
	float fAircraftDrag;
	float fMaxSpeed;
	int iBurnTime;
	int iMaxAlt;
	int iRange;
	int iMaxYawRate;
	int iYawDeceleration;
	int iAcceleration;
	int iRateOfFire;
	int iBurstRate;
	int iECMResistance;

	float fFlameXOffSet;
	float fFlameYOffSet;
	float fFlameZOffSet;
	float fMinRange;
	float fReserved2;
	long lNumberOfSubWeapon;
	long lSubWeaponID;
	int iSeekerFOV;
	int iVerticalLaunch;
	int iMBFlags;
	int iF18Resource;
	int iReserved[3];
	char sNTEWS[10];
};

typedef struct stDBWeapon DBWeaponType;

struct stDefenseType
{
	long lDefenseType;
	long lTypeID;	// Weapon ID or Radar ID
	long lCount;	// If Weapon, amount of ammo else 0
	float fLocationX;
	float fLocationY;
	float fLocationZ;
	long lReserved[4];
};
typedef struct stDefenseType DefenseType;
	

struct stDBShip
{
	long lShipID;
	char sName[64];
	long lShipType;
	long lSizeType;
	int iArmorType;
	int iRadarCrossSignature;
	int iMaxSpeed;
	int iMaxTurnRate;
	int iMaxAccel;
	char sObjectFileName[32];
	char sBlownObjectFileName[32];
	int iSustainedDamage;
	int iPermanentDamage;
	int iDamageLevel1;
	int iDamageLevel2;
	int iDamageLevel3;
	long lExpWeaponType;
	int iNumExplosions;
	int iRadius;
	int iTicksBetweenBlasts;
	int iVarianceBetweenBlasts;
	int iFireType;
	int iFireDuration;
	int iMBFlags;
	float fSmokeStack1X;
	float fSmokeStack1Y;
	float fSmokeStack1Z;
	float fSmokeStack2X;
	float fSmokeStack2Y;
	float fSmokeStack2Z;
	float fHeloX;
	float fHeloY;
	float fHeloZ;
	int iNumDefenseItems;
	DefenseType DefenseList[NUM_DEFENSE_LIST_ITEMS];
};

typedef struct stDBShip DBShipType;



struct stDBLoadoutEntry
{
	long lType;
	long lWeaponId;
	int iWeaponCount;
	int iStation;
	int iReserved2;
	int iReserved3;
	int iReserved4;
};

typedef struct stDBLoadoutEntry DBLoadoutEntry;


struct stDBLoadOutTemplateOld			// 156
{
	char sName[32];					// 32
	int iNumLoadOut;				// 4
	int iFlags;
	int iQuickActionId;
	int iReserved1;
	int iReserved2;
	int iReserved3;
	int iReserved4;
	DBLoadoutEntry Loadouts[10];	// 12*10 = 120
};

typedef struct stDBLoadOutTemplateOld DBLoadOutTemplateOld;


struct stDBLoadOutOld							// 1600
{
	long lLoadOutID;						// 4
	char sName[32];							// 32
	int iNumTemplate;						// 4
	DBLoadOutTemplateOld LoadOutTemplate[10];	// 156 * 10 = 1560
};

typedef struct stDBLoadOutOld DBLoadOutTypeOld;


struct stDBLoadOutTemplate			// 156
{
	char sName[32];					// 32
	int iNumLoadOut;				// 4
	int iFlags;
	int iQuickActionId;
	int iReserved1;
	int iReserved2;
	int iReserved3;
	int iReserved4;
	DBLoadoutEntry Loadouts[MAX_NUMLOADOUT_STATIONS];	// 12*10 = 120
};

typedef struct stDBLoadOutTemplate DBLoadOutTemplate;


struct stDBLoadOut							// 1600
{
	long lLoadOutID;						// 4
	char sName[32];							// 32
	int iNumTemplate;						// 4
	DBLoadOutTemplate LoadOutTemplate[MAX_NUM_LOADOUTS_ENTRIES];	// 156 * 10 = 1560
};

typedef struct stDBLoadOut DBLoadOutType;




struct stWeaponListType
{
	int iID;
	char sName[50];
};

typedef struct stWeaponListType WeaponListType;

struct stWarheadListType
{
	int iID;
	char sName[50];
};

typedef struct stWarheadListType WarheadListType;

struct stSeekerListType
{
	int iID;
	char sName[50];
};

typedef struct stSeekerListType SeekerListType;

struct stMotorListType
{
	int iID;
	char sName[50];
};

typedef struct stMotorListType MotorListType;

struct stWeaponUsageType
{
	int iID;
	char sName[50];
};

typedef struct stWeaponUsageType WeaponUsageType;

struct stEngineType
{
	int iID;
	char sName[50];
};

typedef struct stEngineType EngineTypeList;

struct stExplosionModel
{
	int iID;
	char sName[50];
};

typedef struct stExplosionModel ExplosionModel;

#ifdef __THEBASETOOL__
DBAircraftType AircraftDB[MAX_NUM_AIRCRAFT];
DBRadarType RadarDB[MAX_NUM_RADAR];
DBLoadOutType LoadOutDB[MAX_NUM_LOADOUT];
DBWeaponType WeaponDB[MAX_NUM_WEAPON];
DBVehicleType VehicleDB[MAX_NUM_VEHICLE];
DBShipType ShipDB[MAX_NUM_SHIP];

#endif

#define NUM_WEAPON_TYPE_LIST			25
#define NUM_WARHEAD_TYPE_LIST			24
#define NUM_SEEKER_TYPE_LIST			14
#define NUM_MOTOR_TYPE_LIST				3
#define NUM_WEAPONUSAGE_TYPE_LIST		29
#define NUM_EXPLOSION_MODEL_TYPE_LIST	16
#define NUM_ENGINE_TYPE_LIST			4


#ifdef __THEBASE__


ExplosionModel ExplosionModelList[NUM_EXPLOSION_MODEL_TYPE_LIST]=
{
	{ 0, "NONE" },
	{ 1, "F18 HULK" },
	{ 2, "METALIC(a) HULK" },
	{ 3, "CAMMO HULK" },
	{ 4, "HELO" },
	{ 5, "BUE2" },
	{ 6, "BUEA6" },
	{ 7, "BUF14" },
	{ 8, "BUMIG25" },
	{ 9, "BUMIG29" },
	{ 10, "BUS3" },
	{ 11, "BUSU24" },
	{ 12, "BUSU25" },
	{ 13, "BUSU27" },
	{ 14, "BUSU35" },
	{ 15, "BUV22" },
};


EngineTypeList EngineList[NUM_ENGINE_TYPE_LIST] =
{
	{ 1, "TWO_REAR_ENGINES" },
	{ 2, "ONE_REAR_ENGINE" },
	{ 4, "TWO_WING_ENGINES" },
	{ 8, "FOUR_WING_ENGINES" }
};

#define DB_WEAPON_TYPE_SAM		8

WeaponListType WeaponTypeList[NUM_WEAPON_TYPE_LIST]=
{
	{ 0, "Dumb Bomb" },
	{ 1, "Guided Bomb" },
	{ 2, "A/A Short Range Missile" },
	{ 3, "A/A Medium Range Missile" },
	{ 4, "A/A Long Range Missile" },
	{ 5, "A/G Missile" },
	{ 6, "Gun" },
	{ 7, "SCUD" },
	{ 8, "SAM" },
	{ 9, "Flare" },
	{ 10, "Chaff" },
	{ 11, "Cluster Bomb" },
	{ 12, "Fuel Tank" },
	{ 13, "AN/ALE-40 Chaff And Flare Dispensor" },
	{ 14, "Standoff Jammer" },
	{ 15, "IJAM - Noise" },
	{ 16, "AN/AXQ-14 Data Link Pod" },
	{ 17, "Anti-Ship Missile" },
	{ 18, "Fuel Air Explosive" },
	{ 19, "Napalm" },
	{ 20, "A/A Anti-Radar Missile" },
	{ 21, "Decoy " },
	{ 22, "Naval Mine" },
	{ 23, "A/G Rocket" },
	{ 24, "IJAM - Decept" }
};

WarheadListType WarheadList[NUM_WARHEAD_TYPE_LIST]=
{
	{ 0, "Standard-Tiny" },
	{ 1, "Standard-Small" },
	{ 2, "Standard-Medium" },
	{ 3, "Standard-Large" },
	{ 4, "Standard-Huge" },
	{ 5, "AntiArmor-Tiny" },
	{ 6, "AntiArmor-Small" },
	{ 7, "Anti-Personnel" },
	{ 8, "Icendiary" },
	{ 9, "Penetration" },
	{ 10, "Cratering" },
	{ 11, "Chemical" },
	{ 12, "Nuke" },
	{ 13, "SAM - Large" },
	{ 14, "SAM - Medium" },
	{ 15, "SAM - Small" },
	{ 16, "SAM - Tiny" },
	{ 17, "AAA - Large" },
	{ 18, "AAA - Medium" },
	{ 19, "AAA - Small" },
	{ 20, "AAA - Tiny" },
	{ 21, "Fuel Air Explosive" },
	{ 22, "Napalm" },
	{ 23, "Smoke" }
};

SeekerListType SeekerList[NUM_SEEKER_TYPE_LIST]=
{
	{ 0, "None" },
	{ 1, "Active Radar" },
	{ 2, "All Aspect IR" },
	{ 3, "Rear Aspect IR" },
	{ 4, "Video" },
	{ 5, "Imaging IR" },
	{ 6, "Laser" },
	{ 7, "Semi-Active Radar"},
	{ 8, "AAA - Stream" },
	{ 9, "AAA - Burst" },
	{ 10, "AAA - Single" },
	{ 11, "Anti-Radar" },
	{ 12, "GPS" },
	{ 13, "Naval Mine" },
};

MotorListType MotorList[NUM_MOTOR_TYPE_LIST]=
{
	{ 0, "No Plume" },
	{ 1, "Plume" },
	{ 2, "Low Smoke " }
};

WeaponUsageType WeaponUsageList[NUM_WEAPONUSAGE_TYPE_LIST] =
{
	{ 0, "Stand Off Precision Attack" },
	{ 1, "Stand Off Precision Attack (Hardened Target)" },
	{ 2, "Runway/Taxiway Cratering" },
	{ 3, "General Purpose Low Altitude Attack" },
	{ 4, "Anti-Armor" },
	{ 5, "Anti-Personnel/Material" },
	{ 6, "Anti-Personnel/Material (Incendiary)" },
	{ 7, "Anti-Ship Mines" },
	{ 8, "Precision Attack" },
	{ 9, "Precision Attack (Hardened Target)" },
	{ 10, "Precision Attack (Underground Target)" },
	{ 11, "General Purpose Ground Attack" },
	{ 12, "Close Range Air/Ground Attack" },
	{ 13, "Short Range Anti-Air" },
	{ 14, "Medium Range Anti-Air" },
	{ 15, "Advanced Targeting FLIR Pod" },
	{ 16, "Navigation Flir Pod" },
	{ 17, "Data Link Pod" },
	{ 18, "External Fuel Tank" },
	{ 19, "Expendable Radar Decoy" },
	{ 20, "Expendable Infared Decoy" },
	{ 21, "Expendable Radar/IR Decoys" },
	{ 22, "Physics Package" },
	{ 23, "Expendable Radar and IR Decoys" },
	{ 24, "A/G Rocket" },
	{ 25, "Tactical Air Launched Decoy" },
	{ 26, "Anti-Ship Cruise Missile" },
	{ 27, "Anti-Radar Missile" },
	{ 28, "Stand Off Area Attack" }
};




int g_iNumAircraft=0;
int g_iNumRadar=0;
int g_iNumLoadOut=0;
int g_iNumWeapon=0;
int g_iNumVehicle=0;

int g_iNumShip;


BOOL g_bIdEdit=FALSE;

DBAircraftType *pAircraftDB=NULL;
DBRadarType *pRadarDB=NULL;
DBLoadOutType *pLoadOutDB=NULL;
DBWeaponType *pWeaponDB=NULL;
DBVehicleType *pVehicleDB=NULL;
DBShipType *pShipDB=NULL;

DBAircraftType *pDBAircraftList=NULL;
int iNumAircraftList=0;
DBRadarType *pDBRadarList=NULL;
int iNumRadarList=0;
DBWeaponType *pDBWeaponList=NULL;
int iNumWeaponList=0;
DBVehicleType *pDBVehicleList=NULL;
int iNumVehicleList=0;
DBShipType *pDBShipList=NULL;
int iNumShipList=0;





#else

extern int g_iNumAircraft;
extern int g_iNumRadar;
extern int g_iNumLoadOut;
extern int g_iNumWeapon;
extern int g_iNumVehicle;
extern int g_iNumShip;

extern BOOL g_bIdEdit;

extern DBAircraftType AircraftDB[MAX_NUM_AIRCRAFT];
extern DBRadarType RadarDB[MAX_NUM_RADAR];
extern DBLoadOutType LoadOutDB[MAX_NUM_LOADOUT];
extern DBWeaponType WeaponDB[MAX_NUM_WEAPON];
extern DBVehicleType VehicleDB[MAX_NUM_VEHICLE];
extern DBShipType ShipDB[MAX_NUM_SHIP];

extern DBAircraftType *pAircraftDB;
extern DBRadarType *pRadarDB;
extern DBLoadOutType *pLoadOutDB;
extern DBWeaponType *pWeaponDB;
extern DBVehicleType *pVehicleDB;
extern DBShipType *pShipDB;

extern ExplosionModel ExplosionModelList[NUM_EXPLOSION_MODEL_TYPE_LIST];
extern EngineTypeList EngineList[NUM_ENGINE_TYPE_LIST];
extern WeaponListType WeaponTypeList[NUM_WEAPON_TYPE_LIST];
extern WarheadListType WarheadList[NUM_WARHEAD_TYPE_LIST];
extern SeekerListType SeekerList[NUM_SEEKER_TYPE_LIST];
extern MotorListType MotorList[NUM_MOTOR_TYPE_LIST];
extern WeaponUsageType WeaponUsageList[NUM_WEAPONUSAGE_TYPE_LIST];

extern DBAircraftType *pDBAircraftList;
extern int iNumAircraftList;
extern DBRadarType *pDBRadarList;
extern int iNumRadarList;
extern DBWeaponType *pDBWeaponList;
extern int iNumWeaponList;
extern DBVehicleType *pDBVehicleList;
extern int iNumVehicleList;
extern DBShipType *pDBShipList;
extern int iNumShipList;


#endif


#endif __DATAFORM_H__