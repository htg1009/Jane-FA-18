// airdam.h

// lDamage1 Flags
#define DAMAGE_BIT_L_BLEED_AIR	0x00000001		// shut engine down in 10 minutes or fire
#define DAMAGE_BIT_L_ENG_CONT	0x00000002		// no AB and 70-80% Mil is max.
#define DAMAGE_BIT_L_OIL_PRESS 	0x00000004		// shut engine down within 5 minutes or fire
#define DAMAGE_BIT_L_BURNER		0x00000008		// no AB
#define DAMAGE_BIT_L_FUEL_PUMP	0x00000010		// lose one ok, lose l and r go <30k stay at mil or flameout
#define DAMAGE_BIT_L_GEN		0x00000020		// if both l/r no SB and only front color MPD as ADI, if l engine not shut dwn AMAD after 60 minutes
#define DAMAGE_BIT_L_MAIN_HYD	0x00000040		// if both l/r no radar, SB, cut gain on joystick, if EMER_HYD too then 1/4 joystick and no flaps, gear
#define DAMAGE_BIT_FLT_CONT		0x00000080		// no autotrim
#define DAMAGE_BIT_TARGET_IR	0x00000100
#define DAMAGE_BIT_PACS			0x00000200
#define DAMAGE_BIT_CEN_COMP		0x00000400		// loose all displays
#define DAMAGE_BIT_RADAR		0x00000800
#define DAMAGE_BIT_ADC			0x00001000		// loose pitch ratio system, alt hold, hud pitch and heading scale only
#define DAMAGE_BIT_OXYGEN		0x00002000		// get below 10000 or blackout in 5-10 minutes
#define DAMAGE_BIT_LO_ENGINE	0x00004000		// ai plane left outer engine gone
#define DAMAGE_BIT_L_ENGINE		0x00008000		// gone - no thrust single engine
#define DAMAGE_BIT_R_BLEED_AIR	0x00010000
#define DAMAGE_BIT_R_ENG_CONT	0x00020000
#define DAMAGE_BIT_R_OIL_PRESS	0x00040000
#define DAMAGE_BIT_R_BURNER		0x00080000
#define DAMAGE_BIT_R_FUEL_PUMP	0x00100000
#define DAMAGE_BIT_R_GEN		0x00200000
#define DAMAGE_BIT_EMER_HYD		0x00400000		// see regular hyd.
#define DAMAGE_BIT_AUTOPILOT	0x00800000		// gone
#define DAMAGE_BIT_NAV_FLIR		0x01000000
#define DAMAGE_BIT_GUN 			0x02000000		// gone
#define DAMAGE_BIT_TEWS 		0x04000000
#define DAMAGE_BIT_HUD   		0x08000000		// gone
#define DAMAGE_BIT_FUEL_TANKS 	0x10000000		// fuel leak
#define DAMAGE_BIT_R_MAIN_HYD	0x20000000
#define DAMAGE_BIT_RO_ENGINE	0x40000000
#define DAMAGE_BIT_R_ENGINE		0x80000000

// Damage1 ID numbers (used during damage determination, but not used for storage).  Similar to WARNING defines for MOST things
#define DAMAGE_L_BLEED_AIR	0		// shut engine down in 10 minutes or fire
#define DAMAGE_L_ENG_CONT	1		// no AB and 70-80% Mil is max.
#define DAMAGE_L_OIL_PRESS 	2		// shut engine down within 5 minutes or fire
#define DAMAGE_L_BURNER		3		// no AB
#define DAMAGE_L_FUEL_PUMP	4		// lose one ok, lose l and r go <30k stay at mil or flameout
#define DAMAGE_L_GEN		5		// if both l/r no SB and only front color MPD as ADI, if l engine not shut dwn AMAD after 60 minutes
#define DAMAGE_L_MAIN_HYD	6		// if both l/r no radar, SB, cut gain on joystick, if EMER_HYD too then 1/4 joystick and no flaps, gear
#define DAMAGE_FLT_CONT		7		// no autotrim
#define DAMAGE_TARGET_IR	8
#define DAMAGE_PACS			9
#define DAMAGE_CEN_COMP		10		// loose all displays
#define DAMAGE_RADAR		11
#define DAMAGE_ADC			12		// loose pitch ratio system, alt hold, hud pitch and heading scale only
#define DAMAGE_OXYGEN		13		// get below 10000 or blackout in 5-10 minutes
#define DAMAGE_LO_ENGINE	14		// ai plane left outer engine gone
#define DAMAGE_L_ENGINE		15		// gone - no thrust single engine
#define DAMAGE_R_BLEED_AIR	16
#define DAMAGE_R_ENG_CONT	17
#define DAMAGE_R_OIL_PRESS	18
#define DAMAGE_R_BURNER		19
#define DAMAGE_R_FUEL_PUMP	20
#define DAMAGE_R_GEN		21
#define DAMAGE_EMER_HYD		22		// see regular hyd.
#define DAMAGE_AUTOPILOT	23		// gone
#define DAMAGE_NAV_FLIR		24
#define DAMAGE_GUN 			25		// gone
#define DAMAGE_TEWS 		26
#define DAMAGE_HUD   		27		// gone
#define DAMAGE_FUEL_TANKS 	28		// fuel leak
#define DAMAGE_R_MAIN_HYD	29
#define DAMAGE_RO_ENGINE	30
#define DAMAGE_R_ENGINE		31

//  Flags For lFireFlags
#define HEAT_UP_LEFT_ENGINE		0x00000001
#define FIRE_LEFT_ENGINE		0x00000002
#define HEAT_UP_RIGHT_ENGINE	0x00000010
#define FIRE_RIGHT_ENGINE		0x00000020
#define HEAT_UP_AMAD_LEFT		0x00000100
#define FIRE_AMAD_LEFT			0x00000200
#define HEAT_UP_AMAD_RIGHT		0x00001000
#define FIRE_AMAD_RIGHT			0x00002000

//  New Plane Damage functions
void DoExplosionInAir(WeaponParams *pweapon,BOOL do_big_bang = 0,FPointDouble *position = NULL,DBWeaponType *pweapon_type=NULL, PlaneParams *skipplane = NULL);
void DamagePlane(PlaneParams *planepnt, DWORD damage_type, DWORD game_loop, float damage, FPointDouble relative_position, float damage_radius = 1, WeaponParams *W = NULL, DBWeaponType *pweapon_type = NULL);
void DistributeDamage(PlaneParams *planepnt, float damage, DamageProportions *ratio, float pitch, float heading, float damage_radius, long weaponflags = 0);
void DistributePlayerDamage(PlaneParams *planepnt, float damage, DamageProportions *ratio, float pitch, float heading, float damage_radius, long weaponflags);
float GetPlaneItemArmor(PlaneParams *planepnt, long itemnum);
float GetPlaneItemMaxDamage(PlaneParams *planepnt, long itemnum);
float DamageAIPlaneItem(PlaneParams *planepnt, int hititem, float startdamage, float add_to_total, float our_max_total, int big_boom);
float DamagePlayerPlaneItem(PlaneParams *planepnt, int hititem, float startdamage, float add_to_total, float our_max_total, int big_boom);
void DestroyPlaneItem(PlaneParams *planepnt, long itemid, int critical);
void DelayDestroyPlaneItem(PlaneParams *planepnt, long itemid, float destroyedperc);
void CheckDelayedDestruction();
long ConvertAIDamageToFlag(int hititem);
float ADCheckSpecificHit(PlaneParams *planepnt, WeaponParams *W, DBWeaponType *pweapon_type, float *pitch, FPointDouble relative_position);
void ADGetRelativeHPPoint(PlaneParams *planepnt, FPointDouble engineposition, FPointDouble worldposition, float *heading, float *pitch);
void ADSetUpPlayerHeatUp(int leftside, long timetillfire, int amad = 0);
void ADEngineFireOut(int leftside, int amad = 0);
void ADEngineFireTempOut(int leftside, int amad = 0);
void ADQuickFireSim();
void ScrapePlayerTail(float severity,int item);
void OrphanAllPlaneSmoke(PlaneParams *P);
void ADDamageTowedDecoy(PlaneParams *planepnt);
