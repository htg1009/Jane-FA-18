#ifndef __F18_WEAPON_HEADER__
#define __F18_WEAPON_HEADER__

#define AG_WEAP		0
#define AA_WEAP		1
#define GUN_WEAP	2
#define TARGET_WEAP	3
#define FLARE_WEAP	4
#define NAV_WEAP	5
#define FUEL_WEAP	6

#define	ID_330GALLON_TANK		142
#define ID_480GALLON_TANK		143

#define MAX_WEAPONS				256
#define MAX_F18E_STATIONS		15
#define WEAPCOUNT_UNLIMITED		(-1)
#define WEIGHT_330_GALLONS_FUEL	2210
#define WEIGHT_480_GALLONS_FUEL	3120
#define WEIGHT_MAX_INTERNALFUEL	14400	
#define WEIGHT_MAX_EXTERNALS	20533		// was 17750, jjd 10/26/99
#define	WEIGHT_EMPTY_CRAFT		30564
#define	MIN_FUEL_ALLOWED		800
#define FUEL_INCREMENT			800
#define WEIGHT_MAX_TOTAL		(WEIGHT_EMPTY_CRAFT + WEIGHT_MAX_INTERNALFUEL + WEIGHT_MAX_EXTERNALS)

#define	MAX_LDT_DESCRIPTION		512

typedef struct
{
	int iWeaponId;
	int iWeaponType;
	char acWeaponModel[26];
	char acWeaponMPDAbbrev[12];
	char sWeaponType[24];
	char sWeaponUsage[80];
	char sWeaponWarhead[24];
	int	nWeight;		// lbs; used to be a float
	int iSpritePage;
	int iX;
	int iY;
	int iWidth;
	int iHeight;
} WeaponInfo;

typedef struct
{
	int iWeaponId;
	int iWeaponCnt;
} StationInfo;

typedef struct
{
	int			nLoadoutType;
	int			nInternalFuelWeight;
	DWORD		dwReserved;
	StationInfo Station[MAX_F18E_STATIONS];
} F18Loadout;

typedef struct
{
	F18Loadout	Loadout;
	char		szLoadoutName[260];
	char		szDescription[MAX_LDT_DESCRIPTION];
	int			nReserved1;
	int			nReserved2;
	int			nReserved3;
	int			nReserved4;
} F18LoadoutFile;

typedef struct
{
	StationInfo LeftWing1[4];
	StationInfo LeftWing2[4];
	StationInfo LeftWing3[21];
	StationInfo RightWing1[4];
	StationInfo RightWing2[4];
	StationInfo RightWing3[21];
	StationInfo LeftCent[27];
	StationInfo RightCent[27];
	StationInfo Center[13];
	StationInfo	TargetFlir[1];
	StationInfo ChaffFlare[3];
	StationInfo Gun[2];
	StationInfo NavFlir[1];
} F18WeaponLayout;

// Get weight of aircraft with provided loadout.
int GetWeight (F18Loadout *pLoadout);
int GetWeightNoFuel (F18Loadout *pLoadout);


#endif