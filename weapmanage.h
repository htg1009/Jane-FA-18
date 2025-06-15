 //IMPORTANT!! IF WEAP IDS CHANGE IN THE DATABASE, MUST CHANGE THE GLOBAL LIST (below,in Xvars.bld) AND THE DEFINES
 /*
 int F18WeapIds[] = {34,35,108,36,37,109,38,110,111,4,112,
                    114,87,116,117,118,119,24,120,30,121,122,
                    123,124,32,125,126,127,128,33,129,130,131,15,132,
                    133,134,135,136,137,138,139,140,141,142,143,1,2,7,44,-1};

 */

#define MAX_F18_WEAP_IDS           49

#define AIM7F_ID                   0
#define AIM7M_ID                   1
#define AIM7MH_ID                  2
#define AIM9L_ID                   3
#define AIM9M_ID                   4
#define AIM9X_ID                   5
#define AIM120_ID                  6
#define AGM65E_ID                  7
#define AGM65F_ID                  8
#define AGM65G_ID                  9
#define AGM84A_ID                  10
#define AGM84H_ID         		   11
#define AGM88_ID                   12
#define AGM123_ID                  13
#define AGM62_ID                   14
#define AGM154A_ID                 15
#define AGM154C_ID                 16
#define GBU12D_ID                  17
#define GBU16B_ID                  18
#define GBU24A_ID                  19
#define GBU24B_ID                  20
#define GBU29_ID                   21
#define GBU30_ID                   22
#define GBU31_ID                   23
#define MK82_ID                    24
#define MK82R_ID                   25
#define MK82SE_ID                  26
#define MK83_ID                    27
#define MK83R_ID                   28
#define MK84_ID                    29
#define MK84R_ID                   30
#define MK20_ID                    31
#define MK77_ID                    32
#define CBU59B_ID                  33
#define CBU72_ID                   34
#define LAU10_ID                   35
#define LAU68_ID                   36
#define MK60_ID                    37
#define MK65_ID                    38
#define AAR50_ID                   39
#define AAS38A_ID                  40
#define AWW13_ID                   41
#define ADM141_ID                  42
#define GALLONTANK330_ID           43
#define GALLONTANK480_ID           44
#define	CHAFF_FLARE6060_ID		   45
#define CHAFF_FLARE9030_ID		   46
#define CHAFF_FLARE3090_ID		   47
#define CANNON_20MM_ID			   48

// weap store type, used to define the weapon page to use for this weapon
#define DUMB_WEAP_PAGE       0
#define CLUSTER_WEAP_PAGE    1
#define ROCKET_POD_WEAP_PAGE 2
#define AGM_65_WEAP_PAGE     3
#define AGM_65F_WEAP_PAGE    4
#define AGM_65E_WEAP_PAGE    5
#define HARM_WEAP_PAGE       6
#define HARPOON_WEAP_PAGE    7
#define SLAMER_WEAP_PAGE     8
#define WALLEYE_WEAP_PAGE    9
#define JSOW_WEAP_PAGE       10
#define JDAM_WEAP_PAGE       11
#define TALD_WEAP_PAGE       12
#define AIM9_WEAP_PAGE       13
#define AIM7_WEAP_PAGE       14
#define AIM120_WEAP_PAGE     15

// F18 STATION DEFINES
#define LEFT1_STATION       0
#define LEFT2_STATION       1
#define LEFT3_STATION       2
#define LEFT4_STATION       3
#define LEFT5_STATION       4
#define CENTER6_STATION     5
#define RIGHT7_STATION      6
#define RIGHT8_STATION      7
#define RIGHT9_STATION      8
#define RIGHT10_STATION     9
#define RIGHT11_STATION     10
#define CHAFF_FLARE_STATION 11
#define GUNS_STATION         12
#define ALE50_STATION       13

#define AIM7_STAT       0
#define AIM9_STAT       1
#define AIM120_STAT     2
#define AIR_GROUND_STAT 3
#define GUNS_STAT       4
#define NO_WEAP_STAT    5

#define AIM7_MODE   0
#define AIM9_MODE   1
#define AIM120_MODE 2
#define GUNS_MODE   3
#define AA_WEAP_OFF 4

// FOR NOW WE WILL LEAVE THE OLD F-15 DEFINES BUT SET THEM TO 11 STATIONS
#define WING_L		 0
#define WING_CL		 1
#define WING_R		 3
#define WING_CR		 4
#define FRONT_L		 6
#define FRONT_R		 7
#define BACK_L		10
#define BACK_R		10
#define LANTIRN_L	 9
#define LANTIRN_R	10
#define CHAFF_FLARE	11	// was 10 jjd
#define GUN_STATION	10
#define AG_LEFT		 2
#define AG_RIGHT	 5
#define AG_CENTER	 8



void InitWeaponsLoadout(void);
void SelectNextWeapon(void);
void SelectNextAAWeapon(void);
void SelectNextAGWeapon(void);
int  HasAGWeapSelected(void);
int  HasAAWeapSelected(void);
void ManualSelectAGWeapon(int Station);
void SelectUnSelectAGWeap(int Station);
void SelectMediumOrShortRangeAAWeapon(int Type);
void MissleReject(void);
void Button1Press(void);
void Button2Press(void);
void DoBombReleaseSim(int CalcInterval);
void F18LoadPlayerWeaponVars();
void InitF18LoadoutInfo(void);
void F18LoadPlayerWeaponInfo(void);
int  WeapQuantity(int WeapId);
int GetNumStationsUnselected(int WeapId);


void DoRocketReleaseSim(int FirstTime);


int GetWeapId(int Type);
int GetWeapIndexFromId (int nId);

int GetWeapPageForWeapId(int WeapId,int Type);


int GetModeForAAWeap(int WeapId);


int GetSelectedAAWeaponId();
int GetSelectedAGWeaponId();

int IsWeaponOnBoard(int Id);

void SetAGWeapon(int WeapId,int NumStations);

void SetBombingAttributes(int Qty, int Mult,int Interval,int DetonateHeight);

int AGStationReject();

int IsWeapPod(int Id);

void FixupAALoadout();
void FixupAGLoadout();


void SelectStationsForCount(int WeapId,int Count);