#ifndef __SKUNKFF_HEADER__
#define __SKUNKFF_HEADER__

//***********************************************************************************
// * Initialization & shutdown routines - must be called first & last, respectively
// **********************************************************************************
__declspec(dllexport) int  ForceFeedbackInit(HINSTANCE MainInst,HWND hWnd);
__declspec(dllexport) void ForceFeedbackFree (void);
__declspec(dllexport) void StopFFEffect(int Index);
__declspec(dllexport) void PlayFFEffect(int Index,int Modify = -1,void *Params = NULL);


#define MAX_NUM_FF_EFFECTS     26

#define FF_DAMAGE_BUFFET_INCREASE      0
#define FF_DAMAGE_BUFFET_DECREASE      1
#define FF_DAMAGE_BUFFET_SUSTAIN       2
#define FF_WEAPON_RELEASE              3
#define FF_ROCKET_FIRING               4
#define FF_MISSILE_FIRING              5 
#define FF_HIT_BUFFET                  6
#define FF_STALL_BUFFET_INCREASE       7
#define FF_STALL_BUFFET_DECREASE       8
#define FF_STALL_BUFFET_SUSTAIN        9
#define FF_GUN_RECOIL_HI               10
#define FF_GUN_RECOIL_LO               11
#define FF_TRIM_FORCES                 12
#define FF_HIGH_AOA_BUFFET_INCREASE    13
#define FF_HIGH_AOA_BUFFET_DECREASE    14
#define FF_HIGH_AOA_BUFFET_SUSTAIN     15
#define FF_CARRIER_TAKEOFF_NOSEDOWN    16
#define FF_CARRIER_TAKEOFF_NOSEUP      17
#define FF_CARRIER_LANIDNG_THUMP       18
#define FF_CARRIER_LANIDNG_SHAKE       19
#define FF_GROUND_RUNWAY_BUMP          20
#define FF_CRASH                       21
#define FF_DITCH_SCRAPE                22
#define FF_DITCH_SCRAPE_DECREASE       23
#define FF_DITCH_WATER                 24
#define FF_PROXIMITY_EXPLOSION         25



#define FF_SET_MAGNITUDE               0
#define FF_SET_DIRECTION               1
#define FF_SET_OFFSET                  2
#define FF_SET_SPRING                  3

typedef struct
{
  int Button1Press;
  int DitchScrapeOn;
  int DitchScrapeDecOn;
  int CrashOn;
  int OnGround;
  double LastVel;
  int    LastTick;
} ForceFeedType;

void SimFF();
void InitFFVars();

#endif
