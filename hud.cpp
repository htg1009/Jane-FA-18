/*****************************************************************************
*
*	hud.cpp  --  F18 HUD code
*
*
*-----------------------------------------------------------------------------
*	Copyright (c) 1997 by Origin Systems, Inc., All Rights Reserved.
*****************************************************************************/

#include "F18.h"
#include "3dfxF18.h"
#include "resources.h"
#include "spchcat.h"
#include "gamesettings.h"

#include "snddefs.h"

extern int WorldPrimed;
extern unsigned char GreenPal[];
extern unsigned char WGreenPal[];

extern int g_iBaseBettySoundLevel;
extern int g_iSoundLevelCaution;
extern int LeftDownSwitchState;

extern DWORD g_dwAOASndHandle;
extern DWORD g_dwAIMSndHandle;
extern DWORD g_dwAIMLockSndHandle;

extern void Load3bytePal3Dfx(unsigned char *pal,int start,int count);
extern MovingVehicleParams *MeatBallCarrier;

extern AvionicsType Av;
extern WeapStoresType WeapStores;

#define AV_MAX_GPS_LABELS 100
extern int AvNumGpsLabels;
extern MissionLabelType *AvGpsLabels[AV_MAX_GPS_LABELS];

#define MAX_BOMB_PROGS 5
extern BombProgType BombProgs[MAX_BOMB_PROGS];

class Timer HudFlashTimer;
BOOL DrawMeatball;

extern DetectedPlaneListType CurFramePlanes;
extern DetectedPlaneListType PrevFramePlanes;
extern TargetInfoType *Primary;

int CeilToPow2( int num );
void GrDrawSprite( TextureBuff *pTexture,int x, int y, float  r, float g, float b, int orientation, float u, float v, float u2, float v2);
void GrDrawPolyCircleArc( GrBuff *Buff, int OriginX, int OriginY, int inradius, int outradius, float startangle, float endangle, int Segments, float ColorRed, float Green, float Blue, int PAL8);
void ComputeVelocityVector( PlaneParams *P );


void DisplayHUDClock( void);
void DisplayHUDThrottle(  PlaneParams *P);

extern int TargetInZone(int Page, int *Condition,int *TimeToMaxInSec);
void HudShowOtherLaser(PlaneParams *planepnt);
extern int	MySlot;		// my player's slot index [0..N]
extern BOOL PrimaryIsAOT(DetectedPlaneListType *Plist);

float	fLast_Rmax = -1;		//   max range, really Rmax1 or Raero(nm)
extern long lShowLostTimer;
float HUDResMul = 1.0f;

///////////////////////
//					 //
//   Local Defines   //
//					 //
///////////////////////

#define HUD_DEGREE	  9	// pixels per degree in HUD

#define	SM_HEADING_BAR				0
#define SM_HEADING_PTR				1
#define SM_GUN_CROSS				2
#define SM_WATER_MARK				3
#define SM_VELOCITY_VECTOR			4
#define SM_GHOST_VELOCITY_VECTOR	5
#define SM_ALPHA					6
#define SM_POS_90					7
#define SM_NEG_90					8
#define SM_PIPPER					9
#define SM_DESIGNATE			   10
#define SM_PIPPER_TELLTALE		   11
#define SM_CIRCLE_4				   12
#define SM_CIRCLE_5				   13
#define SM_LAW_MARK				   14
#define SM_LAW_RULE				   15
#define SM_RADAR_MARK			   16
#define SM_RADAR_RULE			   17
#define SM_SHOOT_CUE			   18
#define SM_SHOOT_CUE_F			   19
#define SM_STAR6_CUE			   20
#define SM_STAR6_CUE_F			   21
#define SM_DOT_4				   22
#define SM_DOT_5				   23
#define SM_DOT_6				   24
#define SM_ILS_RULE				   25
#define SM_ILS_MARK				   26

#define SM_NEXT_ICON			   27

//----------------------------------------------------------------------
//
//	HUD symbol placements (from "HUD Formats - with VHSIC" change 4)
//
//		For text windows, this specifies the upper-left pixel of the box.
//

#define RESD_NUM_X(x) 			(((x-HUD_MIDX)*HUDResMul)+HUD_MIDX)
#define RESD_NUM_Y(y) 			(((y-HUD_MIDY)*HUDResMul)+HUD_MIDY)

#define RESD_NUM_XY(x,y)		(((x-HUD_MIDX)*HUDResMul)+HUD_MIDX),(((y-HUD_MIDY)*HUDResMul)+HUD_MIDY)

#define WINDOW_0(x,y)			(RESD_NUM_X(320)+x),(RESD_NUM_Y(116)+y)	// watermark (HUD_MIDX,HUD_MIDY)

#define WINDOW_1(x,y)			(RESD_NUM_X(210)+x),(RESD_NUM_Y(120)+y)	// calibrated airspeed
#define WINDOW_2(x,y)			(RESD_NUM_X(210)+x),(RESD_NUM_Y(126)+y)	// angle-of-attack
#define WINDOW_2A(x,y)			(RESD_NUM_X(206)+x),(RESD_NUM_Y(140)+y)	// true airspeedils_steer


#define WINDOW_4(x,y)			(RESD_NUM_X(200)+x),(RESD_NUM_Y(180)+y)	// HUD in command
#define WINDOW_5(x,y)			(RESD_NUM_X(195)+x),(RESD_NUM_Y(190)+y)	// AA missile type
#define WINDOW_6(x,y)			(RESD_NUM_X(213)+x),(RESD_NUM_Y(200)+y)	// Mach
#define WINDOW_7(x,y)			(RESD_NUM_X(195)+x),(RESD_NUM_Y(210)+y)	// current G

#define WINDOW_8(x,y)			(RESD_NUM_X(300)+x),(RESD_NUM_Y(130)+y)	// TF warnings

#define WINDOW_10(x,y)			(RESD_NUM_X(395)+x),(RESD_NUM_Y(105)+y)	// radar range (t/l pixel of ruler)

#define WINDOW_13(x,y)			(RESD_NUM_X(414)+x),(RESD_NUM_Y(117)+y)	// baro altitude
#define WINDOW_14(x,y)			(RESD_NUM_X(412)+x),(RESD_NUM_Y(128)+y)	// vertical velocity
#define WINDOW_14A(x,y)			(RESD_NUM_X(416)+x),(RESD_NUM_Y(137)+y)	// radar altitude

#define WINDOW_16(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(170)+y)	// AA aspect angle
#define WINDOW_17(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(180)+y)	// target/steering mode
#define WINDOW_18(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(190)+y)	// target range NM
#define WINDOW_19(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(200)+y)	// time-to-xxx
#define WINDOW_20(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(210)+y)	// mode text

#define WINDOW_21(x,y)			(RESD_NUM_X(240)+x),(RESD_NUM_Y(170)+y)	// ILS glideslope deviation
#define WINDOW_22(x,y)			(RESD_NUM_X(320)+x),(RESD_NUM_Y(150)+y)	// ILS localizer  deviation

//*******************************RESD_NUM_X()********RESD_NUM_Y()**************************************************************************************************
// HUD WINDOW DEFS FOR F18  -- HeRESD_NUM_X()ader -- RESD_NUM_Y()(search keyword to step through each section of file)
//*******************************RESD_NUM_X()********RESD_NUM_Y()**************************************************************************************************

#define HUD_CENTER(x,y)			(RESD_NUM_X(320)+x),(RESD_NUM_Y(116)+y)	// watermark (HUD_MIDX,HUD_MIDY)

#define HUD_WIN_1(x,y)			(RESD_NUM_X(370)+x),(RESD_NUM_Y(134)+y)  // jlm
#define HUD_WIN_2(x,y)			(RESD_NUM_X(370)+x),(RESD_NUM_Y(140)+y)  // jlm

#define HUD_WIN_5(x,y)			(RESD_NUM_X(373)+x),(RESD_NUM_Y(158)+y)  // jlm
#define HUD_WIN_6(x,y)			(RESD_NUM_X(373)+x),(RESD_NUM_Y(164)+y)  // jlm
#define HUD_WIN_7(x,y)			(RESD_NUM_X(373)+x),(RESD_NUM_Y(170)+y)  // jlm
#define HUD_WIN_8(x,y)			(RESD_NUM_X(373)+x),(RESD_NUM_Y(176)+y)  // jlm

#define HUD_WIN_NAV(x,y)		(RESD_NUM_X(373)+x),(RESD_NUM_Y(182)+y)  // jlm


#define HUD_VC_WIN(x,y)         (RESD_NUM_X(347)+x),(RESD_NUM_Y(143)+y)  // jlm
#define HUD_RANGE_WIN(x,y)      (RESD_NUM_X(347)+x),(RESD_NUM_Y(149)+y)  // jlm
#define HUD_TOF_WIN(x,y)	    (RESD_NUM_X(347)+x),(RESD_NUM_Y(155)+y)  // jlm

#define HUD_AOA_WIN(x,y)		(RESD_NUM_X(243)+x),(RESD_NUM_Y(152)+y)  // jlm
#define HUD_MACH_WIN(x,y)		(RESD_NUM_X(243)+x),(RESD_NUM_Y(158)+y)  // jlm
#define HUD_G_WIN(x,y)			(RESD_NUM_X(243)+x),(RESD_NUM_Y(164)+y)  // jlm
#define HUD_GMAX_WIN(x,y)		(RESD_NUM_X(243)+x),(RESD_NUM_Y(170)+y)  // jlm
#define HUD_AP_WIN(x,y)			(RESD_NUM_X(243)+x),(RESD_NUM_Y(176)+y)  // jlm

#define HUD_FLAPS_WIN(x,y)		(RESD_NUM_X(243)+x),(RESD_NUM_Y(182)+y)  // gk
#define HUD_SPDBRK_WIN(x,y)		(RESD_NUM_X(243)+x),(RESD_NUM_Y(188)+y)  // gk

#define HUD_ALT_WIN(x,y)		(RESD_NUM_X(364)+x),(RESD_NUM_Y(116)+y)	// baro altitude // jlm

#define HUD_IAS_WIN(x,y)		(RESD_NUM_X(254)+x),(RESD_NUM_Y(116)+y)	// AIRSPEED // jlm

#define HUD_WIN_9(x,y)		    (RESD_NUM_X(311)+x),(RESD_NUM_Y(188)+y)    // jlm
#define HUD_WIN_10(x,y)		    (RESD_NUM_X(311)+x),(RESD_NUM_Y(196)+y)    // jlm

#define HUD_BREAKX_WIN(x,y)     (RESD_NUM_X(320)+x),(RESD_NUM_Y(156)+y)    // jlm

#define HUD_WIN_16(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(170)+y)	// AA aspect angle
#define HUD_WIN_17(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(180)+y)	// target/steering mode
#define HUD_WIN_18(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(190)+y)	// target range NM
#define HUD_WIN_19(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(200)+y)	// time-to-xxx
#define HUD_WIN_20(x,y)			(RESD_NUM_X(405)+x),(RESD_NUM_Y(210)+y)	// mode text

#define HUD_WIN_21(x,y)			(RESD_NUM_X(240)+x),(RESD_NUM_Y(170)+y)	// ILS glideslope deviation
#define HUD_WIN_22(x,y)			(RESD_NUM_X(320)+x),(RESD_NUM_Y(150)+y)	// ILS localizer  deviation
#define HUD_GACQ(x,y)			(RESD_NUM_X(320)+x),(RESD_NUM_Y(148)+y) // gun acquire circle



//*****************************************************************************************************************************************
// HUD TYPES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
  class Timer MSITargetMemTimer;
} HudType;

//*****************************************************************************************************************************************
// HUD GLOBALS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

HudType Hud;

class Timer  RocketCCIPTimer;
FPoint LastVV;
FPoint LastRocketPipper;

int doHUD;			// global trick for SetHUDClipWindow()

int TranX;			// HUD offsets for virtual cockpit
int TranY;			// 0,0 for normal F1 HUD

int HudLeft;		// HUD clip window for virtual cockpit
int HudRight;
int HudTop;
int HudBottom;

double HUDsin;		// sin/cos of (-P->Roll) for HUD rotation
double HUDcos;

int VelVectorX;		// A/G pippers and ILS symbols are VV based
int VelVectorY;

int	g_iHUDWeapID = 0;
int g_iHUDCount  = 0;

AvRGB *HUDColorRGB;
AvRGB HUDColorList[11];

int LastFlightStatus;

extern int Aim9X_Id;
extern int Aim9L_Id;
extern int Aim9M_Id;
extern int Aim7F_Id;
extern int Aim7M_Id;
extern int Aim7MH_Id;
extern int Aim120_Id;

//*****************************************************************************
//=============================================================================
//		A I R - t o - A I R    F U N C T I O N S
//=============================================================================
//*****************************************************************************

#define AA_RANGER	(12000*FTTONM)	// range reticle max (nm)

//============================================================================
//		AA globals (these really should come from UFC/MPD)
//============================================================================

DBWeaponType *AA_weapDB; // weapon DB entry
PlaneParams  *AA_target; // target plane pointer
PlaneParams  *AA_lockon; // IRscan plane pointer

int		AA_weapID;		// WeapID of priority missile
int		AA_count;		//	 total count on-board
float	AA_Tpost;		//   missile in flight (sec)
int		AA_Tpre;		//	 prelaunch TOF/TTI (sec)
int		AA_Tact;		//	 prelaunch time-to-active (sec)
int		AA_Tmin;		//	 min TOF (sec)
int		AA_Tmax;		//	 postlaunch max TOF (sec)
float	AA_Rmin;		//   min range (nm)
float	AA_Rtr;			//   turn&run range (nm)
float	AA_Rmax;		//   max range, really Rmax1 or Raero(nm)
float	AA_Rmar;		//   missile active range (nm)
float AA_ROpt;     //   missile extented range (nm)
int   AA_AimPhase; // 0 = off,, 1 = time to activate
float AA_TFromActToImpact; // for aim 120, time from activate to impact
int   AA_MissileInFlight; // if missile has been fired and has not exploded yet
float AA_TMem;            // memory timer for L&S targets lost, lets pilot know that lock has been broken
FPointDouble AA_Mem_LastPosition; // for TD Box, the last known position for the designated target if it was lost
FPointDouble AA_PrimaryPosition;
FPointDouble AA_SecondaryPosition;
BOOL AA_AOT;			// target is jamming

float	AA_range;		// radar range to PDT (nm)
float	AA_aspect;		//   target aspect angle (+/- degrees)
float	AA_ASEsize;		//   ASE circle scaling factor (pixels)
int		AA_altitude;	//   altitude (ft)

int		AA_pickle;		// pickle state last frame (t/f)
int		AA_breakX;		// break X displayed (t/f)
int		AA_shootQ;		// range between Rmax and Rmin (icon#/0)
int		AA_flash;		// cue flashing state (t/f)
int		AA_scale;		// current radar full scale (nm)

int		AA_slave;		// seeker slaved to radar TD box (t/f)
int		AA_irFOV;		//   field of view (degrees)
int		AA_PosX;		//   azimuth (pixels)
int		AA_PosY;		//   altitude (pixels)

int		AA_DotX;		// LAE dot azimuth (pixels)
int		AA_DotY;		// LAE dot altitude (pixels)

int		AA_CAGE;		// seeker head caged (t/f)
int		AA_SCAN;		// SCAN button (nutation)
int		AA_THLD;		// THLD button (threshold)

int   AA_DrawASE;
float AA_ASESize;

int   AA_TDBoxX;
int   AA_TDBoxY;


int	  PrevAAWeapMode = -1;

FPointDouble HudSteerPoint;
float        HudSteerBearing;

int	  AboveLAW = 0;		// flag for above LAW last frame
float  AOA_unit = 0;		// AOA in units
int    AOA_last = 0;		// 0 if (AOA < 28.4); 1 for (AOA <30.0); etc.
int    AOA_mark = 0;		// timer for flashing AOA indicator
int    AGL_mark = 0;		// hysteresis marker: ON @ 1500', OFF @ 1800'
int    CUE_mark = 0;		// timer for flashing TD shoot cue
int    VxV_mark = 0;		// timer for flashing Velocity Vector
int    Targ_mark = 0;	// timer for flashing TargetDiamond

int	  ShowETE  = 0;		// Steering data flag


int TRelSet;

int HudDeclutter;
int HudAlt;

GrBuff HUDPushBuff;

//*****************************************************************************************************************************************
// EXTERNS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern WeapStoresType WeapStores;


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


//Cut out coordinates for Hudicon.pcx
GrIcon HUDsym[] =
{	//	   x1,y1   x2,y2  hx,hy	 w,h
	{ NULL,   4, 4, 140, 6, 64, 2, 0,0 },	//  0:SM_HEADING_BAR
	{ NULL,   4,14,  10,16,  3, 0, 0,0 },	//  1:SM_HEADING_PTR
	{ NULL,  16,12,  22,18,  3, 3, 0,0 },	//  2:SM_GUN_CROSS
	{ NULL,  28,14,  44,17,  8, 0, 0,0 },	//  3:SM_WATER_MARK
	{ NULL,  49,11,  61,20,  6, 6, 0,0 },	//  4:SM_VELOCITY_VECTOR
	{ NULL,  66,11,  78,17,  6, 6, 0,0 },	//  5:SM_GHOST_VELOCITY_VECTOR
	{ NULL,  85,14,  93,18,  0, 0, 0,0 },	//  6:SM_ALPHA
	{ NULL,  99, 9, 113,23,  7, 7, 0,0 },	//  7:SM_POS_90
	{ NULL, 118, 9, 132,23,  7, 7, 0,0 },	//  8:SM_NEG_90
	{ NULL,   4,24,  34,54, 15,15, 0,0 },	//  9:SM_PIPPER (12 radius)
	{ NULL,  40,30,  48,38,  4, 4, 0,0 },	// 10:SM_DESIGNATE (5 diag)
	{ NULL,  61,27,  83,49, 11,11, 0,0 },	// 11:SM_PIPPER_TELLTALE (11/10 radius)
	{ NULL, 140,10, 143,13,  1, 1, 0,0 },	// 12:SM_CIRCLE_4
	{ NULL, 140,20, 144,24,  2, 2, 0,0 },	// 13:SM_CIRCLE_5
	{ NULL, 140,30, 143,36,  0, 3, 0,0 },	// 14:SM_LAW_MARK
	{ NULL, 150, 0, 155,118, 0, 0, 0,0 },	// 15:SM_LAW_RULE
	{ NULL, 141,41, 143,45,  2, 2, 0,0 },	// 16:SM_RADAR_MARK	(5 high)
//	{ NULL, 140,40, 143,46,  3, 3, 0,0 },	// 16:SM_RADAR_MARK	(7 high)
	{ NULL, 160, 0, 162,80,  0, 0, 0,0 },	// 17:SM_RADAR_RULE	(3 wide)
//	{ NULL, 160, 0, 163,80,  0, 0, 0,0 },	// 17:SM_RADAR_RULE (4 wide)
	{ NULL, 120,30, 128,34,  4, 0, 0,0 },	// 18:SM_SHOOT_CUE
	{ NULL, 120,40, 128,44,  4, 4, 0,0 },	// 19:SM_SHOOT_CUE_F (flip)
	{ NULL, 120,50, 130,60,  5, 0, 0,0 },	// 20:SM_STAR6_CUE
	{ NULL, 120,50, 130,60,  5,10, 0,0 },	// 21:SM_STAR6_CUE_F (flip)
	{ NULL, 140,50, 144,53,  2, 1, 0,0 },	// 22:SM_DOT_4
	{ NULL, 140,60, 144,64,  2, 2, 0,0 },	// 23:SM_DOT_5
	{ NULL, 140,70, 145,75,  3, 3, 0,0 },	// 24:SM_DOT_6
	{ NULL, 170, 0, 174,60,  0,30, 0,0 },	// 25:SM_ILS_RULE
	{ NULL, 120,90, 128,98,  4, 8, 0,0 },	// 26:SM_ILS_MARK
};


/*-----------------------------------------------------------------------------
 *
 *	KillSound()
 *
 */
void KillSound( DWORD *SndHandle )
{
	if (*SndHandle)
	{
		SndEndSound( *SndHandle );
		*SndHandle = 0;
	}
}


//***************************
void DebugPoint(void)
{
	lpDD->FlipToGDISurface();
}

//*********************************************************************************


//*****************************************************************************
//=============================================================================
//		D E B U G    F U N C T I O N S
//=============================================================================
//*****************************************************************************

int  ShowID = 0;
static char marque[64];

void ShowUp(void) {	ShowID++; }
void ShowDn(void) {	ShowID--; }

/*-----------------------------------------------------------------------------
 *
 *	ShowMe()
 *
 */
void ShowMe(void)
{
#ifdef _DEBUG
//	float GetF18WeaponDrag( PlaneParams * );
//	HUDtextLg( HUD_MIDX, HUD_TOP, "weapon drag = %f", GetF18WeaponDrag(PlayerPlane) );

	switch (ShowID)
	{
		case -1:  ShowID=0;  return;
		case  0:			 return;

		case  1:
			extern int GetF18Version( char * );
			GetF18Version( marque );
			break;

		case  2:
			sprintf( marque, "raw joy: %6.1f %6.1f",
					((int)GetJoyX() - 32768) / 327.68,
					((int)GetJoyY() - 32768) / 327.68 );
			break;

		case  3:
			sprintf( marque, "dead joy: %6.1f %6.1f",
					((int)GetJoyDeadX() - 32768) / 327.68,
					((int)GetJoyDeadY() - 32768) / 327.68 );
			break;

		case  4:
			sprintf( marque, "pro joy: %6.1f %6.1f",
					((int)GetJoyPosX() - 32768) / 327.68,
					((int)GetJoyPosY() - 32768) / 327.68 );
			break;

		case  5:
			sprintf( marque, "XXX joy: %6.1f %6.1f %6.1f",
					((int)GetJoyX()     - 32768) / 327.68,
					((int)GetJoyDeadX() - 32768) / 327.68,
					((int)GetJoyPosX()  - 32768) / 327.68 );
			break;

		case  6:
			sprintf( marque, "YYY joy: %6.1f %6.1f %6.1f",
					((int)GetJoyY()     - 32768) / 327.68,
					((int)GetJoyDeadY() - 32768) / 327.68,
					((int)GetJoyPosY()  - 32768) / 327.68 );
			break;

		case  7:  ShowID=0;  return;

		default:  sprintf( marque, "invalid #: %d", ShowID );  break;
	}

	HUDtextLg( 300, 0, marque);//GrDrawStringClipped( GrBuffFor3D, LgHUDFont, 300, 0, marque, HUDColor );
#endif
}

//*****************************************************************************
//=============================================================================
//		C O M M O N    F U N C T I O N S  -  ( S M A L L )
//=============================================================================
//*****************************************************************************


/*-----------------------------------------------------------------------------
 *
 *	DisplayWaterMark()
 *
 */
void DisplayWaterMark(void)
{
	if( (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) || (AOA_unit < 0) )
	   HUDicon( WINDOW_0(0,0), SM_WATER_MARK );
}

//*****************************************************************************************************************************************
// DISPLAY MACH  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayMach( PlaneParams *P )
{
	if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)  return;
	if(UFC.MasterMode == NAV_MODE) return;

	HUDtextSm( HUD_MACH_WIN(0,0), "M  %1.3f", P->Mach );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayInCmd()
 *
 */
inline void DisplayInCmd(void)
{
	if (CurrentView != COCKPIT_FRONT)  return;

	ReadMouse();
	if (CheckInHUD( ppCurrMouse.x, ppCurrMouse.y ))
		HUDtextLg( WINDOW_4(0,0), "IN CMD" );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplaySniff()
 *
 */
inline void DisplaySniff(void)
{
	if (UFC.EMISState || SniffOn)
		HUDtextLg( WINDOW_14(1,0), "SNIFF" );
}


/*-----------------------------------------------------------------------------
 *
 *	DisplayGunCross()
 *
 */
inline void DisplayGunCross(void)
{
	if (UFC.MasterArmState == ON)
		HUDicon( WINDOW_0(0,-22), SM_GUN_CROSS );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayAPHUD()  --  AutoPilot
 *
 */
void DisplayAPHUD( PlaneParams *P )
{
	if (UFC.TFStatus == OFF)  return;

	switch (UFC.TFWarning)
	{
		case PULL_UP:  HUDtextLg( WINDOW_8(0,0), " FLY UP"  );  break;
		case OBSTACLE: HUDtextLg( WINDOW_8(0,0), "OBSTACLE" );  break;
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayVerticalVelocity()  --  (ft/min)
 *
 */
void DisplayVerticalVelocity( PlaneParams *P )
{
	if((UFC.MasterMode == NAV_MODE) && !P->OnGround)
	  HUDtextSm(HUD_ALT_WIN(0,-7),"%4d00",(int)(P->IfVelocity.Y * (60.0 / 100)) );

	/*
	int yPos = 0; // LEAVE IN SAME POSITION (P->FlightStatus & PL_GEAR_DOWN_LOCKED) ? 34 : 0;

	HUDtextSm( WINDOW_14(-4,yPos1), "VV%4d00", (int)(P->IfVelocity.Y * (60.0 / 100)) );
	*/
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleAim7Scan()
{
	WeapStores.Aim7LoftOn = !WeapStores.Aim7LoftOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ToggleAim120Scan()
{
	WeapStores.Aim120VisualOn = !WeapStores.Aim120VisualOn;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsWeapCaged()
{
	if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7M_Id) || (AA_weapID == Aim7MH_Id) )
	{
		return(WeapStores.Aim7LoftOn);
	}
	else if(AA_weapID == Aim120_Id)
	{
		return(WeapStores.Aim120VisualOn);
	}
	else if( (AA_weapID == Aim9X_Id) || (AA_weapID == Aim9M_Id) || (AA_weapID == Aim9L_Id) )
	{
	 return(AA_SCAN);
	}

	return(FALSE);
}

//*****************************************************************************************************************************************
// DISPLAY INDICATED AIRSPEED  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayIAS( PlaneParams *P )
{
	int yPos = 0; // leave in same pos (P->FlightStatus & PL_GEAR_DOWN_LOCKED) ? 34 : 0;



//	float DisplayIndicatedAirSpeed = FtSecTASToIAS(P->BfLinVel.X, P->Altitude);

//  inline float FtSecTASToIAS(float FtSecTAS, float Alt)
//	double TrueKnots = FtSecTAS*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
//	if (Alt < 0.0f) Alt = 0.0f;
//	if (Alt > 70000.0f) Alt = 70000.0f;
//	return(float)(TrueKnots * sqrt(Pressure[(int)(Alt/500.0f)]/0.0023769f));

	double TrueKnots = P->BfLinVel.X*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);

	float TmpAlt = P->Altitude;
	if (TmpAlt < 0.0f) TmpAlt = 0.0f;
	if (TmpAlt > 70000.0f) TmpAlt = 70000.0f;

	float DisplayIAS = (TrueKnots * sqrt(Pressure[(int)(TmpAlt/500.0f)]/0.0023769f));

	if (DisplayIAS < 48.0)
		DisplayIAS = 48.0;

	HUDtextLg( HUD_IAS_WIN(0,yPos), "%4d",(int)DisplayIAS);

	/*
	if (UFC.HudTasSelect && !(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		HUDtextLg( WINDOW_2A(0,yPos), "T%4d",(int)P->Knots );
	}
	 * */



	HUDbox( HUD_IAS_WIN(-2,-2+yPos), HUD_IAS_WIN(20,8+yPos) );

//	HUDtextLg( HUD_IAS_WIN(0,yPos), "%4d",(int)P->IndicatedAirSpeed );

	/*
	if (UFC.HudTasSelect && !(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		HUDtextLg( WINDOW_2A(0,yPos), "T%4d",(int)P->Knots );
	}
	 * */
}

//*****************************************************************************************************************************************
// DISPLAY G FORCE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayGForce( PlaneParams *P )
{
	if (!RealisticAPs && (UFC.APStatus == ON))
		HUDtextSm( HUD_AP_WIN(0,0), "AP CASUAL");		// Fake AP mode
	else
	{
		if((UFC.MasterMode != NAV_MODE) || (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED)))
		{
		  HUDtextSm( HUD_G_WIN(0,0), "G  %2.1f", P->GForce);
		  HUDtextSm( HUD_GMAX_WIN(0,0), "   %2.1f", P->AvailGForce);
		}

		if (RealisticAPs && (UFC.APStatus == ON))
			HUDtextSm( HUD_AP_WIN(0,0), "AP");
	}

	extern int  TimeExcel;
	if (TimeExcel)
		HUDtextSm( HUD_FLAPS_WIN(0,0), "%dX", 1<<TimeExcel );
	else
	{
		switch (P->FlapsMode)
		{
			case FLAPS_FULL :
				HUDtextSm( HUD_FLAPS_WIN(0,0), "FULLFLAPS");
				break;
			case FLAPS_HALF :
				HUDtextSm( HUD_FLAPS_WIN(0,0), "HALFFLAPS");
				break;
			case FLAPS_AUTO :
				HUDtextSm( HUD_FLAPS_WIN(0,0), "AUTOFLAPS");
				break;
		};
	}

	if (P->SpeedBrakeState)
		HUDtextSm( HUD_SPDBRK_WIN(0,6), "SBDBRK");
		//HUDtextSm( HUD_SPDBRK_WIN(0,0), "SBDBRK");

	// HUD_FLAPS_WIN(x,y)
	// HUD_SPDBRK_WIN(x,y)

/*
	if (!(JoyCap1.wCaps & JOYCAPS_HASZ) || UFC.HudThrottleSelect)
	{
		HUDtextLg( WINDOW_7(0,10), "THRUST %3d%%", round( P->ThrottlePos ) );
	}
	else if (DisplayFrameRate)
	{
		HUDtextLg( WINDOW_7(0,10), "%4.1f  fps", FrameRate );
//		HUDtextLg( WINDOW_7(0,10), "%4.1f  fps", P->CommandedThrust );
	}

*/
	if (DisplayFrameRate)
	{
		HUDtextLg( HUD_GMAX_WIN(0,18), "%4.1f  fps", FrameRate );
	}


}

/*-----------------------------------------------------------------------------
 *
 *	DoAOASoundWarnings()
 *
 *		This is called every frame regardless of camera view.
 *
 */
void DoAOASoundWarnings( PlaneParams *P )
{
	long warnticks;
	float fworkvar;
	int checkaoa = 1;
	float AOAabs;
	int vertvel;

	if((P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)) || (P->OnGround))
		return;

	if((SimPause) || (iEndGameState))
		return;

	if(!(P->Status & PL_DEVICE_DRIVEN))
	{
		return;
	}

	/*

	static int AOASND[] = { 0, SIMSND_900HZ4, SIMSND_900HZ10, SIMSND_900HZ };

	float AOA = DegAOAToUnits( RadToDeg(P->Alpha) );

	if (AOA > 45.5f)  AOA = 45.5f;

	int iAOA = (AOA >= 28.4) + (AOA >= 30.0) + (AOA >= 33.0);	// [0..3]

	if (iAOA != AOA_last)
	{
		KillSound( &g_dwAOASndHandle );

		if (iAOA)
		{
			int iLevel = g_iSoundLevelCaution - 20;
			if (iLevel > 0)
				g_dwAOASndHandle = SndQueueSound( AOASND[iAOA], 0, iLevel );
		}
	}
  */


//	AOA_unit =  AOA; for F-15
	AOA_unit =  RadToDeg(P->Alpha);

	AOAabs = fabs(AOA_unit);
	if(AOAabs > 14.0f)
	{
		if(PlayerPlane->FlapsMode == FLAPS_AUTO)
		{
			if(GetRegValueL("aoawarn_noua") == 1)
			{
				checkaoa = 0;
			}
		}

		if(checkaoa)
		{
			lAOAWarnTimer -= DeltaTicks;
			if(lAOAWarnTimer < 0)
			{
				int iLevel = g_iSoundLevelCaution - 20;
				SndQueueSound( SND_AOA_WARN, 1, iLevel );
			}
			if(AOAabs > 35.0f)
			{
				warnticks = 100;
			}
			else
			{
				fworkvar = (AOAabs - 14.0f) / 21.0f;

				if(fworkvar < 0)
				{
					warnticks = 1000;
				}
				else if(fworkvar > 1.0f)
				{
					warnticks = 100;
				}
				else
				{
					fworkvar = (900.0f * (1.0f - fworkvar)) + 100.0f;
					warnticks = fworkvar;
				}
			}
			if((lAOAWarnTimer < 0) || (lAOAWarnTimer > warnticks))
			{
				lAOAWarnTimer = warnticks;
			}
		}
	}

	if(lSREWarnTimer > 0)
	{
		lSREWarnTimer -= DeltaTicks;
		if(lSREWarnTimer < 0)
		{
			lSREWarnFlags &= ~(WARN_SINK_RATE|WARN_FLIGHT_CONTROLS);
		}
	}

	if(!(lSREWarnFlags & WARN_SINK_RATE))
	{
		if(!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			double TrueKnots = P->BfLinVel.X*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);

			float TmpAlt = P->Altitude;
			if (TmpAlt < 0.0f) TmpAlt = 0.0f;
			if (TmpAlt > 70000.0f) TmpAlt = 70000.0f;

			float DisplayIAS = (TrueKnots * sqrt(Pressure[(int)(TmpAlt/500.0f)]/0.0023769f));

			if(DisplayIAS < 120.0f)
			{
				vertvel = (int)(P->IfVelocity.Y * 60.0f);

				if(vertvel < -4000)
				{
					lSREWarnFlags |= WARN_SINK_RATE;
					lSREWarnTimer = 10000;
					AICGenericBettySpeech(BETTY_SINK_RATE);
				}
			}
		}
	}

	if(!(lSREWarnFlags & WARN_FLIGHT_CONTROLS))
	{
		if(P->OutOfControlFlight)
		{
			lSREWarnFlags |= WARN_FLIGHT_CONTROLS;
			lSREWarnTimer = 10000;
			AICGenericBettySpeech(BETTY_FLIGHT_CONTROLS);
		}
	}

	//AOA_last = iAOA;
}

//*****************************************************************************************************************************************
// DRAW ACQ  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawHudAcq()
{

 if(UFC.MasterMode != AA_MODE) return;
 if(Av.AARadar.CurMode != AA_ACQ_MODE) return;

 //inline void HUDdash( int X1, int Y1, int X2, int Y2, int color=HUDColor )
 //inline void HUDcircle( int X, int Y, int R, int color=HUDColor )
 //inline void HUDcircleDash( int X, int Y, int R, int color=HUDColor )

 if(Av.AARadar.AcqMode == AA_WIDE_ACQ_MODE)
 {
   HUDtextLg( HUD_VC_WIN(0,0), "WACQ" );
 }
 else if(Av.AARadar.AcqMode == AA_VERTICAL_ACQ_MODE )
 {
	 HUDdash(HUD_CENTER(-23.4,-40),HUD_CENTER(-23.4,100));
	 HUDdash(HUD_CENTER(23.4,-40),HUD_CENTER(23.4,100));
   HUDtextLg( HUD_VC_WIN(0,0), "VACQ" );
 }
 else if(Av.AARadar.AcqMode == AA_BORE_SIGHT_MODE)
 {
	 HUDcircleDash(HUD_CENTER(0,0),HUD_DEGREE*3);
   HUDtextLg( HUD_VC_WIN(0,0), "BST" );
 }
 else if(Av.AARadar.AcqMode == AA_GUN_ACQ_MODE)
 {
	 HUDcircleDash(HUD_GACQ(0,0),HUD_DEGREE*9);
   HUDtextLg( HUD_VC_WIN(0,0), "GACQ" );
 }

}

//*****************************************************************************************************************************************
// DISPLAY AOA  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayAOA( PlaneParams *P )
{
	if (AOA_last != 0 || AOA_unit < 0)
	{
		int cycle = GetTickCount() - AOA_mark;
										// flash @ 2Hz w/ 50% duty cycle
		if (cycle < 250)  return;			// OFF first half, then ON
		if (cycle > 500)  AOA_mark += cycle;	// set for next cycle
	}

	int PosY = 0; // leave in same pos (P->FlightStatus & PL_GEAR_DOWN_LOCKED) ? 34 : 0;

	HUDicon( HUD_AOA_WIN(0,0), SM_ALPHA );

	HUDtextSm( HUD_AOA_WIN(12,0), "%3.1f", AOA_unit );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************


/*-----------------------------------------------------------------------------
 *
 *	DisplayNAV()
 *
 */
void DisplayNAV()
{
	int hour, min, sec, EA;

	if (UFC.DataCurrSPTime == ETE)
	{
		hour = UFC.DataCurrSPEteHr;
		min  = UFC.DataCurrSPEteMin;
		sec  = UFC.DataCurrSPEteSec;
		EA   = 'E';
	}
	else
	{
		hour = UFC.DataCurrSPEtaHr;
		min  = UFC.DataCurrSPEtaMin;
		sec  = UFC.DataCurrSPEtaSec;
		EA   = 'A';
	}

	// Current NAV point
	HUDtextLg( WINDOW_17(0,0), "NAV  %dA", UFC.DataCurrSP );

	// Distance to NAV point
	HUDtextLg( WINDOW_18(0,0), "N%5.1f", UFC.DataCurrSPNavDist );

	//	ETA or ETE
	HUDtextLg( WINDOW_19(0,0), "%02d:%02d:%02d %c", hour, min, sec, EA );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayTCN()
 *
 */
void DisplayTCN()
{
	int hour, min, sec, EA;

	if (UFC.TacanEte == ETE)
	{
		hour = UFC.TacanEteHr;
		min  = UFC.TacanEteMin;
		sec  = UFC.TacanEteSec;
		EA   = 'E';
	}
	else
	{
		hour = UFC.TacanEtaHr;
		min  = UFC.TacanEtaMin;
		sec  = UFC.TacanEtaSec;
		EA   = 'A';
	}

	extern char UFCStr13[20];  // current tacan station

	// Current TACAN point
	HUDtextLg( WINDOW_17(0,0), UFCStr13 );	// "TCN XXX"

	// Distance to TACAN point
	HUDtextLg( WINDOW_18(0,0), "N%5.1f", UFC.TacanNavDist );

	//	ETA or ETE
	HUDtextLg( WINDOW_19(0,0), "%02d:%02d:%02d %c", hour, min, sec, EA );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayTGT()
 *
 */
void DisplayTGT()
{
	int hour, min, sec;

	hour = UFC.DataCurrTGTTimeHr;
	min  = UFC.DataCurrTGTTimeMin;
	sec  = UFC.DataCurrTGTTimeSec;

	HUDtextLg( WINDOW_17(0,0), "TGT" );
	HUDtextLg( WINDOW_18(0,0), "G%5.1f", UFC.DataCurrTGTNavDist );

	if (ShowETE)	// skip if A/G: LOFT/AUTO/GUIDED and weapon selected
 		HUDtextLg( WINDOW_19(0,0), "%02d:%02d:%02d E", hour, min, sec );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplaySteering()
 *
 */
void DisplaySteering()
{
	if(HudSteerPoint.X == -1) return;

	float Dist = (PlayerPlane->WorldPosition / HudSteerPoint)*WUTONM;

	switch(Av.Hsi.SteeringMode)
	{

 		case HSI_TCN_MODE:
			HUDtextSm( HUD_WIN_NAV(0,6), "%5.1f TCN", Dist);
			break;

		case HSI_TGT_MODE:
			HUDtextSm( HUD_WIN_NAV(0,6), "%5.1f TGT", Dist);
			break;

		case HSI_ACL_MODE:
			 HUDtextSm( HUD_WIN_NAV(0,-12),"%s", Av.Acl.TenSecMessage);
			 HUDtextSm( HUD_WIN_NAV(0,-6), "%s", Av.Acl.Message1);
			 HUDtextSm( HUD_WIN_NAV(0,0), "%s", Av.Acl.StatusMessage);
			 HUDtextSm( HUD_WIN_NAV(0,6), "%5.1f ACL", Dist);
			 break;

		case HSI_WPT_MODE:
			if( HudDeclutter != 2)  //Reject Symbols Mode 2
				HUDtextSm( HUD_WIN_NAV(0,6), "%5.1f W%d", Dist,UFC.DataCurrSP);
			break;

		case HSI_GPS_MODE:
			HUDtextSm( HUD_WIN_NAV(0,6), "%5.1f GPS", Dist);
			break;
	}
}

//*****************************************************************************
//=============================================================================
//		C O M M O N    F U N C T I O N S  -  ( L A R G E )
//=============================================================================
//*****************************************************************************

/*-----------------------------------------------------------------------------
 *
 *	DisplayPitchLadder()
 *
 *	  Numbering the ladder end tabs:
 *
 *		Assume a two digit label of a 3x5 font with kerning of 1.
 *		Project a line out 5 pixels from the center of the end tabs.
 *		This will establish the center of the text box (1 space +
 *		3 digit + 1 space = 5).  Then add (-3,-2) to get the upper-
 *		left corner of the text box.
 *
 *		...LLLLLLLL OXX XXX LLLLLLLL...	<-- "O" is origin of text box "X"
 *		          L XXX XXX L
 *		          L XXX+XXX L	<-- "+" is centroid we are trying to rotate
 *		          L XXX XXX L
 *		          L XXX XXX L	<-- "L" is pitch ladder
 *
 *		Use the negated roll angle since a positive roll (clockwise)
 *		will roll the ladder counter-clockwise in the HUD.
 */
void DisplayPitchLadder( PlaneParams *P )
{
	int i,k;
	int p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,p5x,p5y,p6x,p6y;
	int RotOfsX,RotOfsY;

	// pitch ladder has two modes fixed and yaw movement

	#define OFS	5	// ladders above/below center

	int PitchIndex = P->Pitch / (5*DEGREE);

	int Start = PitchIndex - OFS;
	int Stop  = PitchIndex + OFS;

	// Compute trig once using negated roll and 5 pixel length
	double dSin, dCos;

	dSin = 5 * HUDsin;
	dCos = 5 * HUDcos;

	for (i=Start; i<Stop; i++)
	{
		k = i;
		if (k< 0) k += 72;
		if (k>71) k -= 72;

		p1x = UFC.HUDLadder[k].HUDPoints[0].iSX;
		p1y = UFC.HUDLadder[k].HUDPoints[0].iSY;

		if ((k!=18) && (k!=54))	   // don't do pitch ladder line at +/- 90
		{
			p2x = UFC.HUDLadder[k].HUDPoints[1].iSX;
			p2y = UFC.HUDLadder[k].HUDPoints[1].iSY;
			p3x = UFC.HUDLadder[k].HUDPoints[2].iSX;
			p3y = UFC.HUDLadder[k].HUDPoints[2].iSY;
			p4x = UFC.HUDLadder[k].HUDPoints[3].iSX;
			p4y = UFC.HUDLadder[k].HUDPoints[3].iSY;
			p5x = UFC.HUDLadder[k].HUDPoints[4].iSX;
			p5y = UFC.HUDLadder[k].HUDPoints[4].iSY;
			p6x = UFC.HUDLadder[k].HUDPoints[5].iSX;
			p6y = UFC.HUDLadder[k].HUDPoints[5].iSY;

			int PitchNum = UFC.HUDLadder[k].PitchNum;

			if (PitchNum >= 0)
			{
				HUDline( p1x, p1y, p2x, p2y );	// upper ladder
				HUDline( p3x, p3y, p4x, p4y );
			}
			else
			{
				HUDdash( p1x, p1y, p2x, p2y );	// lower ladder
				HUDdash( p3x, p3y, p4x, p4y );
			}

			HUDline( p1x, p1y, p5x, p5y );		// left  end tab
			HUDline( p4x, p4y, p6x, p6y );		// right end tab

			sprintf( TmpStr, "%2d", abs(PitchNum) );

			if (PitchNum != 0)
			{
				float px, py;

				if (!HUDclip( p1x, p1y ))
				{
					px = (p1x + p5x) / 2.0;	// mid-point of end tab
					py = (p1y + p5y) / 2.0;

					RotOfsX = round( px - dCos - 3 );
					RotOfsY = round( py + dSin - 2 );

					HUDtextSm(  RotOfsX , RotOfsY, TmpStr );
				}

				if (!HUDclip( p4x, p4y ))
				{
					px = (p4x + p6x) / 2.0;	// mid-point of end tab
					py = (p4y + p6y) / 2.0;

					RotOfsX = round( px + dCos - 3 );
					RotOfsY = round( py - dSin - 2 );

					HUDtextSm( RotOfsX, RotOfsY, TmpStr );
				}
			}
		}
		else	// 90° pitch icon
		{
			HUDicon( p1x, p1y, (k==18) ? SM_POS_90 : SM_NEG_90 );
		}
	}
}

void DisplayHUDThrottle(  PlaneParams *P)
{
	int ThrottlePercent = (int)__max(P->LeftThrustPercent,P->RightThrustPercent);

	HUDtextSm( HUD_IAS_WIN(-1,-8), "%03d", ThrottlePercent  );
	if( ThrottlePercent > 80 )
		HUDtextSm( HUD_IAS_WIN(13,-8), "AB");

}


void DisplayHUDClock( void)
{
	float Hours   = WorldParams.WorldTime * (1.0f/3600.0f);
	float Minutes = ( fmod(WorldParams.WorldTime,3600.0f) )/60.0;
	float seconds = (int)WorldParams.WorldTime%60;

	if(Hours >= 24.0)
		Hours = 0.0;

	if(Minutes >= 60.0)
		Minutes = 0.0;

	if (UFC.DataCurrTimeMode == ZULU_TIME)
	{
		Hours -= 3;
		if (Hours < 0)
			Hours += 24.0f;
		HUDtextSm( HUD_FLAPS_WIN(0,6), "%02d:%02d:%02dZ", (int) Hours, (int) Minutes, (int) seconds );
	}
	else
		HUDtextSm( HUD_FLAPS_WIN(0,6), "%02d:%02d:%02d", (int) Hours, (int) Minutes, (int) seconds );
}



//*****************************************************************************************************************************************
// DISPLAY HEADING BAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************


void DisplayHeadingBar( PlaneParams *P )
{
	ANGLE heading = -(int)P->Heading;	// true compass heading

	// Compute sprite offset based on heading  [0..7]
	float SprOfs = 15 * (float)( (float)( (heading % (5*DEGREE)) / (float)(5*DEGREE) ) );

	// Find first tick mark to left of center.
	float xPos = 320.0 - SprOfs;
	float xVal = (heading/DEGREE) - ((heading/DEGREE) % 5);	// round down to even degree

	// Adjust heading val to bar center to prevent waypoint jitter.
	heading = xVal*DEGREE + SprOfs*DEGREE/3;

	// Move right to last tick, ie. add 15 degrees
	xVal += 15;
	xPos +=	15*3;

	if(xVal >= 360.0)
		xVal -= 360.0;

	// Label every 5,10 degrees across heading bar.
	while (xPos >= 320-45)
	{
		IntTo3DigitFont(xVal,TmpStr);

		if(fmod(xVal,10) == 0)
		{
		   HUDtextSm(RESD_NUM_XY( xPos-6, 84-8),TmpStr);
		   HUDline(RESD_NUM_XY(xPos,84),RESD_NUM_XY(xPos,88));
		}
		else
		{
 		   HUDline(RESD_NUM_XY(xPos,84),RESD_NUM_XY(xPos,86));
		}

		xVal -= 5;

		if (xVal < 0.0)
		   xVal += 360.0;

		xPos -= 5*3;	// 3 pixels / degree
	}

	// draw caraet
	HUDline(RESD_NUM_XY(320,89),RESD_NUM_XY(324,93));
	HUDline(RESD_NUM_XY(320,89),RESD_NUM_XY(316,93));

	// --------------------------
	// Display waypoint tick mark.

  //	if( (UFC.DataCurrSPDistFeet < 2000.0) && (!(P->Status & PL_AI_DRIVEN)) )
  //	{
	   //	AINextWayPoint(P);				// advance to next waypoint
	   //	UFCAdvanceWaypointVisual();
  //	}

	ANGLE wayHead;	// waypoint heading in DEGREEs

	if (P->SystemInactive & DAMAGE_BIT_ADC)
	{
		return;
	}

	if(HudSteerPoint.X == -1) return;

	wayHead = HudSteerBearing * DEGREE;

	// this cast forces sign extension for +/- 180°
	int wayOff = (signed short)(wayHead - heading);	// +/- DEGREEs from heading

	xPos = 320 + 3*wayOff/DEGREE;			// move waypoint to its spot

	if (xPos < 320-49 || xPos > 320+49)	// off scale, mark at end with heading
	{
		xPos = 320 + (wayOff<0 ? -49 : 49);
		HUDtextSm(RESD_NUM_XY( xPos-5, 95), "%3d", wayHead/DEGREE );
	}

	HUDline( RESD_NUM_XY(xPos  , 89), RESD_NUM_XY(xPos  , 93));
	HUDline( RESD_NUM_XY(xPos+1, 89), RESD_NUM_XY(xPos+1, 93));
	HUDline( RESD_NUM_XY(xPos-1, 89), RESD_NUM_XY(xPos-1, 93));
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayBankAngle()
 *
 */
void DisplayBankAngle( PlaneParams *P )
{
	double    dSin2,  dCos2;
	double    dSin3,  dCos3;

	sincosA( &dSin2, &dCos2, -P->Roll + 2*DEGREE );
	sincosA( &dSin3, &dCos3, -P->Roll - 2*DEGREE );

 	int X1 = round( 320 + 72 * HUDsin );
 	int Y1 = round( 151 + 72 * HUDcos );

 	int X2 = round( 320 + 67 * dSin2 );
 	int Y2 = round( 151 + 67 * dCos2 );

 	int X3 = round( 320 + 67 * dSin3 );
 	int Y3 = round( 151 + 67 * dCos3 );

	HUDline( RESD_NUM_XY(X1, Y1), RESD_NUM_XY(X2, Y2));		// display caret
	HUDline( RESD_NUM_XY(X1, Y1), RESD_NUM_XY(X3, Y3));

	HUDline( RESD_NUM_XY(320,    225   ),RESD_NUM_XY(320   , 225+ 5));	// display ruler
	HUDline( RESD_NUM_XY(320-13, 225- 1),RESD_NUM_XY(320-14, 225+ 4));
	HUDline( RESD_NUM_XY(320+13, 225- 1),RESD_NUM_XY(320+14, 225+ 4));
	HUDline( RESD_NUM_XY(320-25, 225- 4),RESD_NUM_XY(320-27, 225+ 1));
	HUDline( RESD_NUM_XY(320+25, 225- 4),RESD_NUM_XY(320+27, 225+ 1));
	HUDline( RESD_NUM_XY(320-37, 225-10),RESD_NUM_XY(320-41, 225- 3));
	HUDline( RESD_NUM_XY(320+37, 225-10),RESD_NUM_XY(320+41, 225- 3));
	HUDline( RESD_NUM_XY(320-52, 225-22),RESD_NUM_XY(320-55, 225-19));
	HUDline( RESD_NUM_XY(320+52, 225-22),RESD_NUM_XY(320+55, 225-19));
	HUDline( RESD_NUM_XY(320-64, 225-37),RESD_NUM_XY(320-71, 225-33));
	HUDline( RESD_NUM_XY(320+64, 225-37),RESD_NUM_XY(320+71, 225-33));
}

/*-----------------------------------------------------------------------------
 *
 *	DoAltitudeSoundWarning()
 *
 */
void DoAltitudeSoundWarning( PlaneParams *P, int Alt )
{
	if ((UFC.LawWarningStatus == OFF) ||
		(P->FlightStatus & PL_GEAR_DOWN_LOCKED)) return;

	if (AboveLAW)
	{
//		if (Alt < UFC.LowAltWarningLimit)
		if (Alt < Av.Ufc.RaltVal)
		{
#if 1
			AICGenericBettySpeech(BETTY_LOW_ALT);
#else
			if (g_iBaseBettySoundLevel)
			{
				SndQueueSound(SND_BETTY_LOW, 1, g_iBaseBettySoundLevel );
				SndQueueSound(SND_BETTY_ALTITUDE, 1, g_iBaseBettySoundLevel );
			}
#endif
			DisplayWarning( LOW_ALT, ON,0 );
			AboveLAW = 0;
		}
	}
	else
	{
		if (Alt >= Av.Ufc.RaltVal)
		{
			DisplayWarning( LOW_ALT, OFF, 0 );
			AboveLAW = 1;
		}
	}
}
/*-----------------------------------------------------------------------------
 *
 *	LAWtix()  --  return thermometer height for current altitude
 *
 */
int LAWtix( int alt )
{
	if (alt <  500)  return (        alt       * 90 / 500 );

	if (alt < 1000)  return (  90 + (alt- 500) * 18 / 500 );

					 return ( 108 + (alt-1000) * 10 / 500 );
}

/*-----------------------------------------------------------------------------
 *
 *	HUDaltitude()
 *
 *		Display altitude with mixed large/small font.
 *
 */
void HUDaltitude( int xPos, int yPos, int altitude )
{
	int nDig = sprintf( TmpStr, "%d", altitude );

	if (nDig > 3)	// print below 1000 w/ small font
	{
		char *Mp = TmpStr + (nDig-3);
		HUDtextSm( xPos+10, yPos+2, Mp );
		*Mp = 0;	// kill below 1000
		xPos += 3;	// adjust for small font
	}

	xPos += (5-nDig) * 5 - 3;	// skip to 1st digit

	HUDtextLg( xPos, yPos, TmpStr );



}

/*-----------------------------------------------------------------------------
 *
 *	DisplayLAWAltitude()
 *
 */

void DisplayLAWAltitude( PlaneParams *P )
{
	//	If LAW is ON, display thermometer if AGL<1500' or AGL<1800' for < 3 seconds.
	//

	if (UFC.LawWarningStatus && UFC.LawWithinConstraints &&
		( P->AGL<1500 || P->AGL<1800 && GetTickCount()-AGL_mark<3000 ))
	{
		if (P->AGL < 1500)  AGL_mark = GetTickCount();

		int yPos = 200 - LAWtix( UFC.LowAltWarningLimit );

		HUDicon( RESD_NUM_XY(451, 82),  SM_LAW_RULE );	// ruler 82 -> 200
		HUDicon( RESD_NUM_XY(458,yPos), SM_LAW_MARK );

		HUDtextSm( RESD_NUM_XY(444,  80), "15" );	// 1500' mark
		HUDtextSm( RESD_NUM_XY(444,  90), "10" );	// 1000' mark
		HUDtextSm( RESD_NUM_XY(446, 106),  "5" );	//  500' mark

		yPos = 200 - LAWtix( P->AGL );

		HUDline( RESD_NUM_XY(456,yPos), RESD_NUM_XY(456,200) );	// display thermometer
		HUDline( RESD_NUM_XY(457,yPos), RESD_NUM_XY(457,200) );
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayAltitude()
 *
 */
void DisplayAltitude( PlaneParams *P )
{
	 //	Display barometric/radar  altitude.
	float AltToUse;
	float yPos = 0; // leave it in same position for gear down (P->FlightStatus & PL_GEAR_DOWN_LOCKED) ? 34 : 0;
	char forceBaro =0;
	static float FlashTime=0;;

	FlashTime += seconds_per_frame;


	//AltToUse = (HudAlt == 0) ? (P->Altitude) : (P->Altitude - (LandHeight(P->WorldPosition.X,P->WorldPosition.Z)*WUTOFT));
	if( HudAlt) ///radar
	{
		AltToUse = (P->Altitude - (LandHeight(P->WorldPosition.X,P->WorldPosition.Z)*WUTOFT));
		if( AltToUse > 5000 || 	P->Orientation.K.Y > -0.707f) //> 5000ft force barometric + 45 deg angle
		{
			AltToUse = (P->Altitude); // barometric
			forceBaro =1;



		}




	}
	else
		AltToUse = (P->Altitude); // barometric

	HUDaltitude( HUD_ALT_WIN(0,yPos), (int)AltToUse );
	HUDbox( HUD_ALT_WIN(-2,-1+yPos), HUD_ALT_WIN(23,8+yPos) );

	if( forceBaro )
	{
		if( FlashTime> 1.0f)
			HUDtextLg(  HUD_ALT_WIN(-10,yPos), "B" );
	}
	else if(HudAlt)
		HUDtextLg(  HUD_ALT_WIN(-10,yPos), "R" );

	if( FlashTime> 1.0f)
		FlashTime = 0.0f;



	 /*
	 //	Display radar altitude.
	//
	if (UFC.LawWithinConstraints)
	{
		if (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			if (UFC.HudRAltSelect)
			{
				float AGL = (int)(P->HeightAboveGround * WUTOFT - 3.0);

				HUDtextSm(   WINDOW_14A(-4,1), "R" );
				HUDaltitude( WINDOW_14A( 0,0), AGL );
			}

	//		if (P->Status & PL_DEVICE_DRIVEN)  DoAltitudeSoundWarning(P, AGL);
		}
	}
		* */
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayBreakLock()
 *
 *		Radar break-lock occurs at 60° up/down or left/right, ie. radar gimbal limits.
 *		When the target is within 10° of breaking lock, display degrees before
 *		break-lock directly over the watermark.  (also in NAV mode)
 *
 */
void DisplayBreakLock( PlaneParams *P )
{
	PlaneParams *T = P->AADesignate;

	if (!T)  return;

	float angle, rdist, rdx, rdy, rdz;

	// Compute maximum radar angle for degrees before break-lock
	float DeltaAzim = AIComputeHeadingToPoint( P, T->WorldPosition, &rdist, &rdx, &rdy, &rdz, 1 );
	float DeltaElev = -ComputePitchToPoint( P, T->WorldPosition );

	DeltaAzim = fabs( DeltaAzim );
	DeltaElev = fabs( DeltaElev );

	if (DeltaAzim > DeltaElev)  angle = DeltaAzim;
	else						angle = DeltaElev;

	if (angle >= 50)  HUDtextLg( WINDOW_0(-6,-9), "%2d", round(60 - angle) );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayVelocityVector()
 *
 */
void DisplayVelocityVector( PlaneParams *P )
{
	int		PosX, PosY;
	FPoint	VelVector;
	static float FlashTime=0;;
	float flashrate = 0.7f;

	FlashTime += seconds_per_frame;


	if (P->Knots < 1)  return;

	VelVector.X =  P->BfLinVel.Y;
	VelVector.Y = -P->BfLinVel.Z;
    VelVector.Z = -P->BfLinVel.X;

	VelVector.Perspect( &PosX, &PosY );

	int NrmlX = PosX - GrBuffFor3D->MidX;
	int NrmlY = PosY - GrBuffFor3D->MidY - 20*HUDResMul;

	if (hypot( NrmlX, NrmlY ) > 85*HUDResMul)				// 70 screen pizels = 8.8° HUD
	{
		int cycle = GetTickCount() - VxV_mark;
												// flash @ 2Hz w/ 50% duty cycle
		if (cycle < 250)  return;				// OFF first half, then ON
		if (cycle > 500)  VxV_mark += cycle;	// set for next cycle
	}

	 // Translate virtual cockpit from physical screen center to HUD center
	//
	if (CurrentView & COCKPIT_VIRTUAL_SEAT)
	{
		PosX += HUD_MIDX + TranX - GrBuffFor3D->MidX;
		PosY += HUD_MIDY + TranY - GrBuffFor3D->MidY;
	}

 	VelVectorX = PosX;		// save position for VV related symbols
	VelVectorY = PosY;

	if ( CheckInHUD( PosX, PosY ) && !(P->OnGround) )
		CRTicon( PosX, PosY, SM_VELOCITY_VECTOR );
	else
	{	// Draw Flashing Icon
		if( FlashTime> flashrate)
		{
			if( PosX < GrBuffFor3D->ClipLeft)
				PosX = GrBuffFor3D->ClipLeft;
			else if( PosX >GrBuffFor3D->ClipRight)
				PosX = GrBuffFor3D->ClipRight;

			if (PosY < GrBuffFor3D->ClipTop )
				PosY = GrBuffFor3D->ClipTop;
			else if( PosY > GrBuffFor3D->ClipBottom)
				PosY = GrBuffFor3D->ClipBottom;
			CRTicon( PosX, PosY, SM_VELOCITY_VECTOR );
		}


	}

	if( FlashTime> flashrate)
		FlashTime = 0.0f;


}

/*-----------------------------------------------------------------------------
 *
 *	ComputeVelocityVector()
 *
 */
void ComputeVelocityVector( PlaneParams *P )
{
	int		PosX, PosY;
	FPoint	VelVector;
	static float FlashTime=0;;
	float flashrate = 0.7f;

	FlashTime += seconds_per_frame;


	if (P->Knots < 1)  return;

	VelVector.X =  P->BfLinVel.Y;
	VelVector.Y = -P->BfLinVel.Z;
    VelVector.Z = -P->BfLinVel.X;

	VelVector.Perspect( &PosX, &PosY );

	int NrmlX = PosX - GrBuffFor3D->MidX;
	int NrmlY = PosY - GrBuffFor3D->MidY - 20*HUDResMul;

	if (hypot( NrmlX, NrmlY ) > 85*HUDResMul)				// 70 screen pizels = 8.8° HUD
	{
		int cycle = GetTickCount() - VxV_mark;
												// flash @ 2Hz w/ 50% duty cycle
		if (cycle < 250)  return;				// OFF first half, then ON
		if (cycle > 500)  VxV_mark += cycle;	// set for next cycle
	}

	 // Translate virtual cockpit from physical screen center to HUD center
	//
	if (CurrentView & COCKPIT_VIRTUAL_SEAT)
	{
		PosX += HUD_MIDX + TranX - GrBuffFor3D->MidX;
		PosY += HUD_MIDY + TranY - GrBuffFor3D->MidY;
	}

 	VelVectorX = PosX;		// save position for VV related symbols
	VelVectorY = PosY;

	if( FlashTime> flashrate)
		FlashTime = 0.0f;
}

//*****************************************************************************************************************************************
// DRAW PIPPER  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawPipper(float X,float Y)
{
	float NewX1,NewY1,NewX2,NewY2;
	float DeltaDeg = 30.0;
	float CurDeg   = 0;

	for(int i=0; i<12; i++)
	{
	  ProjectPoint(X,Y,CurDeg,13*HUDResMul,&NewX1,&NewY1);
	  ProjectPoint(X,Y,CurDeg,((i % 3) ? 15 : 17)*HUDResMul,&NewX2,&NewY2);

		CRTline(NewX1,NewY1,NewX2,NewY2);

		CurDeg += DeltaDeg;
	}

  CRTline(X-1,Y-1,X+1,Y-1);
	CRTline(X-1,Y,X+1,Y);
	CRTline(X-1,Y+1,X+1,Y+1);
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW1  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayWindow1(PlaneParams *P)
{
	if(AA_TMem == -1) return;

	sprintf(TmpStr,"MEM %02d",(int) AA_TMem);

	HUDtextSm(HUD_WIN_1(0,0),TmpStr);
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW2  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayWindow2(PlaneParams *P)
{
	 // Get thew sensor that is giving the L&S target
	 int Contribution = 0;

	 switch(Contribution)
	 {
		 case 0: sprintf(TmpStr,"RADAR");  break;
		 case 1: sprintf(TmpStr,"FLIR");   break;
		 case 2: sprintf(TmpStr,"LINK 4"); break;
	 }

	 HUDtextSm(HUD_WIN_2(0,0),TmpStr);
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW5  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayWindow5(PlaneParams *P)
{
	sprintf(TmpStr,"ACL");

	HUDtextSm(HUD_WIN_5(0,0),TmpStr);
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW6  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayWindow6(PlaneParams *P)
{
	sprintf(TmpStr,"AUTO");

  HUDtextSm(HUD_WIN_6(0,0),TmpStr);
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW7  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayWindow7(PlaneParams *P)
{
	 // Get thew sensor that is giving the L&S target
	 int Cue = 1;

	 switch(Cue)
	 {
		 case 0: sprintf(TmpStr,"NWS");         break;
		 case 1: sprintf(TmpStr,"DISPENSE");    break;
		 case 2: sprintf(TmpStr,"DEPLOY");      break;
		 case 3: sprintf(TmpStr,"NO DEPLOY");   break;
	 }

	 HUDtextSm(HUD_WIN_7(0,0),TmpStr);
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW8  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayWindow8(PlaneParams *P)
{
	sprintf(TmpStr,"%4.1f  W%d",UFC.DataCurrSPNavDist,UFC.DataCurrSP);

	HUDtextSm( HUD_WIN_8(0,0),TmpStr);
}

//*****************************************************************************************************************************************
// DISPLAY RANGE WIN  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayRangeWin(PlaneParams *P)
{
	 if( Av.AARadar.CurMode == VS_MODE ) return;

	 AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

	 if(CurAAWeap && (CurAAWeap->WeapPage == AIM7_WEAP_PAGE) && WeapStores.FloodOn)
	 {
//		int hoj = 0;
//		int cnt;

//		DetectedPlaneListType *PList;
//		PList = &CurFramePlanes;
//		for(cnt=PList->NumPlanes-1; cnt>=0; cnt--)
//		{
//			if(PList->Planes[cnt].PlaneId == Primary->Id)
//			{
//				if(PList->Planes[cnt].AOT)
//				{
//					hoj = 1;
//					break;
//				}
//			}
//		}
//		if(cnt < 0)
//		{
//			PList = &PrevFramePlanes;
//			for(cnt=PList->NumPlanes-1; cnt>=0; cnt--)
//			{
//			   	if(PList->Planes[cnt].PlaneId == Primary->Id)
//				{
//					if(PList->Planes[cnt].AOT)
//					{
//						hoj = 1;
//						break;
//					}
//				}
//			}
//		}

		if(AA_AOT)
		{
			sprintf(TmpStr,"HOJ");
		}
		else
		{
			sprintf(TmpStr,"FLOOD");
		}
//		sprintf(TmpStr,"FLOOD");
		HUDtextSm(HUD_RANGE_WIN(0,0),TmpStr);
#if 0
		if(UFC.EMISState || Av.AARadar.SilentOn)
		{
		  HUDline( HUD__9(-5,-3), HUD_WIN_9(25,9) );
		  HUDline( HUD_WIN_9(-5,9),  HUD_WIN_9(25,-3) );
		}
#endif
	 }
	 else
	 {
		 // get range to L&S target
		if (!AA_AOT)
		{
		 	sprintf(TmpStr,"%2.1f RNG",AA_range);
	   		HUDtextSm(HUD_RANGE_WIN(0,0),TmpStr);
		}
		else if(CurAAWeap)
		{
			if(CurAAWeap->WeapPage == AIM120_WEAP_PAGE)
			{
				sprintf(TmpStr,"HOJ");
				HUDtextSm(HUD_RANGE_WIN(0,0),TmpStr);
			}
		}
	 }

}

//*****************************************************************************************************************************************
// DISPLAY VC WIN  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayVcWin(PlaneParams *P)
{
	 if((AA_target == NULL) || AA_AOT) return;
   HUDtextSm( HUD_VC_WIN(0,0), "%4d Vc", (int)AAClosureRate(P, AA_target) );
}

//*****************************************************************************************************************************************
// DISPLAY WINDOW8  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayTOFWin(PlaneParams *P)
{
	 if (AA_range > AA_Rmax)
	 {
		 if((AA_MissileInFlight) && (lShowLostTimer >= 0) && ((AA_range > fLast_Rmax) || (fLast_Rmax < 0)))
		 {
			lShowLostTimer -= DeltaTicks;

			if((lShowLostTimer % 1000) > 500)
			{
				HUDtextSm( HUD_TOF_WIN(0,0), "LOST");
			}
		 }

		 return;
	 }

	 if(!AA_MissileInFlight)
	 {
		 fLast_Rmax = -1.0f;
	 }
	 lShowLostTimer = 5000;

	 if( (UFC.MasterMode == AA_MODE) && (GetAARadarMode() == AA_ACQ_MODE) )
	 {
		 switch(Av.AARadar.CurMode)
	   {
			 case AA_WIDE_ACQ_MODE:     sprintf(TmpStr,"WACQ"); break;
       case AA_VERTICAL_ACQ_MODE: sprintf(TmpStr,"VACQ"); break;
       case AA_BORE_SIGHT_MODE:   sprintf(TmpStr,"BST");  break;
       case AA_GUN_ACQ_MODE:      sprintf(TmpStr,"AACQ"); break;
	   }
	   HUDtextSm(HUD_TOF_WIN(0,0),TmpStr);
	 }
	 else
	 {
		 if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7M_Id) || (AA_weapID == Aim7MH_Id) )
		 {
			 if(!AA_MissileInFlight)
			 {
				 HUDtextSm( HUD_TOF_WIN(0,0), "%2d", (int)AA_Tpre );
			 }
			 else
			 {
				 if(WeapStores.FloodOn)
				   HUDtextSm( HUD_TOF_WIN(0,0), "%2d SL", (int)AA_Tpost );
				 else
				   HUDtextSm( HUD_TOF_WIN(0,0), "%2d TTG", (int)AA_Tpost );
			 }
		 }
		 else if(AA_weapID == Aim120_Id)
		 {
			 if(!AA_MissileInFlight)
			 {
				 HUDtextSm( HUD_TOF_WIN(0,0), "%2d", (int)AA_Tpre );
			 }
			 else
			 {
				 if(AA_AimPhase)
				    HUDtextSm( HUD_TOF_WIN(0,0), "%2d ACT", (int)AA_Tpost );
				 else
				    HUDtextSm( HUD_TOF_WIN(0,0), "%2d TTG", (int)AA_Tpost );
			 }
		 }
 	 }
}

//*****************************************************************************
//=============================================================================
//		I L S   &   F L I R    F U N C T I O N S
//=============================================================================
//*****************************************************************************

float ILS_glide;	// current glideslope (degrees)
float ILS_steer;	// steering localizer (degrees)

/*-----------------------------------------------------------------------------
 *
 *	DisplayILS()
 *
 *		ILS steering displays: glideslope & localizer.
 *
 *		Glideslope is a 40 pixel horizontal line displaced above/below
 *		the velocity vector showing our relation to a 3° glideslope.
 *		The line is one pixel above VV for each 1/10° we are above 3°
 *
 *		Localizer is a 40 pixel vertical line displaced left/right of
 *		the velocity vector showing our relation to the runway vector.
 *		The line is one pixel left of VV for each 1/10° we are right
 *		of the runway vector, ie. we need to steer left.  It is not
 *		dependent on our heading, only our position.
 *
 */
void DisplayILS( PlaneParams *P )
{
	float rHead;	// compass heading of runway (degrees)
 	float vHead;	// compass heading to runway (degrees)

	FPointDouble runway;	// runway vector:   LandingPoint --> FarEndPoint
	FPointDouble vector;	// approach vector: PlayerPlane --> LandingPoint
	float DistToRunway  = 10000000;
	float DistToCarrier = 10000000;
	FPointDouble LandPoint;
	double Heading;
	int usecarrier = 0;
	double deckangle = DECK_ANGLE;

	if (P->OnGround)  return;

	RunwayInfo *R = (RunwayInfo *)UFC.ILSStatus;	// runway from UFC

	MovingVehicleParams *Carrier = NULL;
	if(PlayerPlane->AI.iHomeBaseId >= 0)
	{
		Carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
		DistToCarrier = Dist2D(&PlayerPlane->WorldPosition,&Carrier->WorldPosition);
	}

	if( (R == NULL) && (Carrier == NULL) ) return;

	DistToRunway = Dist2D(&PlayerPlane->WorldPosition,&(R->ILSLandingPoint));

	if((Carrier) && ((DistToCarrier <= DistToRunway) || ((FauxRunway *)R == &Carrier->Runwaydata)))
	{
		if(pDBShipList[Carrier->iVDBIndex].lShipID == 6)
		{
			deckangle = SOV_DECK_ANGLE;
		}

		 GetLandingPoint(Carrier,LandPoint);
		 Heading = NormDegree(360.0 - (Carrier->Heading/DEGREE) - deckangle);
		 usecarrier = 1;

		 //  Seems to work better.
		 LandPoint = R->ILSLandingPoint;
		 Heading = R->Heading;
	}
	else if( (DistToRunway < DistToCarrier) && (R) )
	{
		 LandPoint = R->ILSLandingPoint;
		 Heading = R->Heading;
	}
	else
	{
		 return;
	}

	vector  = LandPoint;
	vector -= P->WorldPosition;

#if 1
  float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,LandPoint,1);
  float DeltaElev = -ComputePitchFromPointToPoint(PlayerPlane->WorldPosition,LandPoint);
	float Dist      =  PlayerPlane->WorldPosition / LandPoint;
#else
  float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,R->ILSLandingPoint,1);
  float DeltaElev = -ComputePitchFromPointToPoint(PlayerPlane->WorldPosition,R->ILSLandingPoint);
	float Dist      =  PlayerPlane->WorldPosition / R->ILSLandingPoint;
#endif

	//HUDtextSm( 320, 116+10,"Azim: %d", (int)DeltaAzim );
	//HUDtextSm( 320, 116+17,"Pitch:%d ", (int)DeltaElev );
	//HUDtextSm( 320, 116+24,"Dist: %d ", (int)Dist );

  float ILS1,ILS2;

	vHead = atan2( vector.X, -vector.Z ) * RADIANS_TO_DEGREES;	// +/- 180°
	rHead = Heading;		// [0..359]

	ILS1 = vHead - rHead;	// +/- 180°
	while (ILS1 >  180)  ILS1 -= 360;
	while (ILS1 < -180)  ILS1 += 360;

	vHead = atan2( vector.X, -vector.Z ) * RADIANS_TO_DEGREES;	// +/- 180°
	rHead = NormDegree(Heading + 180.0);		// [0..359]

	ILS2 = vHead - rHead;	// +/- 180°
	while (ILS2 >  180)  ILS2 -= 360;
	while (ILS2 < -180)  ILS2 += 360;

	if( fabs(ILS1) < fabs(ILS2) )
		ILS_steer = ILS1;
	else
		ILS_steer = ILS2;

	// For small angles, sine and radians are nearly equal, so
	// we skip the asin() here and convert directly to degrees.
	ILS_glide  = ( -vector.Y / vector.Length() ) * RADIANS_TO_DEGREES;
	if(usecarrier)
	{
		ILS_glide -= dGlideSlope;	// convert to relative glideslope
	}
	else
	{
		ILS_glide -= 3.0;	// convert to relative glideslope
	}

	int PosX = VelVectorX + round( HUD_DEGREE * ILS_steer );
	int PosY = VelVectorY + round( HUD_DEGREE * ILS_glide );

	CRTline( PosX,VelVectorY-20, PosX,VelVectorY+20 );	// ILS localizer
	CRTline( VelVectorX-20,PosY, VelVectorX+20,PosY );	// ILS glideslope

	// Glideslope deviation indicator
	//HUDicon( WINDOW_21(0,0), SM_ILS_RULE );

	//if (fabs(ILS_glide) <= 1.0)				// 30 is half scale of ruler
		//HUDicon( WINDOW_21(-1,(int)(-ILS_glide*30)), SM_RADAR_MARK );

	// Localizer deviation indicator @ 1.25°/dot (mag 2x)
	//for (int i=-2; i<3; i++)
		//HUDicon( WINDOW_22(i*HUD_DEGREE*5/2,0), SM_CIRCLE_5 );

 //	if (fabs(ILS_steer) <= 2.5)
	//{
		//int xPos = round( HUD_DEGREE * ILS_steer *2 );

		//HUDicon( WINDOW_22(xPos,-16), SM_ILS_MARK );
		//HUDline( WINDOW_22(xPos,-15), WINDOW_22(xPos,20) );
	//}
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayHUDFlir()
 *
 */

#define HALF_NUM_FLIR_VERTEXES	15
#define STARTING_X		320
#define STARTING_Y      42

int NavFlirFrameXs[HALF_NUM_FLIR_VERTEXES]= {	 61,
						  						125,
						  						129,
						  						134,
						  						140,
						  						147,
						  						151,
						  						153,
						  						153,
						  						151,
						  						147,
						  						141,
						  						134,
						  						130,
						  						 95};

int NavFlirFrameYs[HALF_NUM_FLIR_VERTEXES]= {	 0,
						  						12,
						  						16,
						  						24,
						  						36,
						  						55,
						  						72,
						  						85,
						  						113,
						  						126,
						  						142,
						  						160,
						  						174,
						  						178,
						  						194};


RotPoint3D Flir3DPoints[30];
RotPoint3D *pFlir3DPoints[30]= {&Flir3DPoints[0],
						   		&Flir3DPoints[1],
						   		&Flir3DPoints[2],
						   		&Flir3DPoints[3],
						   		&Flir3DPoints[4],
						   		&Flir3DPoints[5],
						   		&Flir3DPoints[6],
						   		&Flir3DPoints[7],
						   		&Flir3DPoints[8],
						   		&Flir3DPoints[9],
						   		&Flir3DPoints[10],
						   		&Flir3DPoints[11],
						   		&Flir3DPoints[12],
						   		&Flir3DPoints[13],
						   		&Flir3DPoints[14],
						   		&Flir3DPoints[15],
						   		&Flir3DPoints[16],
						   		&Flir3DPoints[17],
						   		&Flir3DPoints[18],
						   		&Flir3DPoints[19],
						   		&Flir3DPoints[20],
						   		&Flir3DPoints[21],
						   		&Flir3DPoints[22],
						   		&Flir3DPoints[23],
						   		&Flir3DPoints[24],
						   		&Flir3DPoints[25],
						   		&Flir3DPoints[26],
						   		&Flir3DPoints[27],
						   		&Flir3DPoints[28],
						   		&Flir3DPoints[29]};\

void DisplayHUDFlir()
{
//	if ((PlayerPlane->SystemInactive & DAMAGE_BIT_NAV_FLIR) ||
//		((PlayerPlane->SystemInactive & DAMAGE_BIT_L_GEN) &&
//		 (PlayerPlane->SystemInactive & DAMAGE_BIT_R_GEN)))

//		return;

//	RotPoint3D *walker;
//	unsigned char *old_fading_table;
//	DWORD clip_flags;
//	int *xwalker;
//	int *ywalker;
//	int mid_x;
//	int mid_y;
//	int old_left,old_right,old_top,old_bottom,old_midx,old_midy;
//	float z_value,ooz_value,oow_value,light_value;

//	if ((CurrentCockpit == FRONT_FORWARD_COCKPIT) || (CurrentCockpit == VIRTUAL_MODEL))
//	{
//		old_left   = GrBuffFor3D->ClipLeft;
//		old_right  = GrBuffFor3D->ClipRight;
//		old_top    = GrBuffFor3D->ClipTop;
//		old_bottom = GrBuffFor3D->ClipBottom;

//		if (CurrentCockpit != VIRTUAL_MODEL)
//		{
//			GrBuffFor3D->ClipLeft	= 320-154;
//			GrBuffFor3D->ClipRight	= 320+154;
//			GrBuffFor3D->ClipTop	=  42+TranY;
//			GrBuffFor3D->ClipBottom =  42+TranY + 195;
//		}

//		old_midx = GrBuffFor3D->MidX;
//		GrBuffFor3D->MidX = HUDPushBuff.MidX;

//		old_midy = GrBuffFor3D->MidY;
//		GrBuffFor3D->MidY = HUDPushBuff.MidY;

//		mid_x = TranX + STARTING_X;
//		mid_y = TranY + STARTING_Y;

//		walker  = Flir3DPoints;
//		xwalker = NavFlirFrameXs;
//		ywalker = NavFlirFrameYs;

//		clip_flags = PT3_RXHIGH | PT3_RXLOW | PT3_RYHIGH | PT3_RYLOW;

//		_3dxlSetupNavFlirZValues(z_value,ooz_value,oow_value,light_value);

//		while(walker < &Flir3DPoints[HALF_NUM_FLIR_VERTEXES])
//		{
//			walker->Flags = PT3_ROTATED | PT3_PERSPECTED | PT3_OOW_SET;
//			walker->iSX = *xwalker++ + mid_x;

//			if (walker->iSX < GrBuffFor3D->ClipLeft)
//			{
//				walker->iSX = GrBuffFor3D->ClipLeft;
//				clip_flags &= ~PT3_RXHIGH;
//			}
//			else
//				if (walker->iSX > GrBuffFor3D->ClipRight)
//				{
//					walker->iSX = GrBuffFor3D->ClipRight;
//					clip_flags &= ~PT3_RXLOW;
//				}
//				else
//					clip_flags &= ~(PT3_RXHIGH | PT3_RXLOW);

//			walker->fSX = (float)walker->iSX+0.015f;

//			walker->iSY = *ywalker++ + mid_y;

//			if (walker->iSY < GrBuffFor3D->ClipTop)
//			{
//				walker->iSY = GrBuffFor3D->ClipTop;
//				clip_flags &= ~PT3_RYHIGH;
//			}
//			else
//				if (walker->iSY > GrBuffFor3D->ClipBottom)
//				{
//					walker->iSY = GrBuffFor3D->ClipBottom;
//					clip_flags &= ~PT3_RYLOW;
//				}
//				else
//					clip_flags &= ~(PT3_RYHIGH | PT3_RYLOW);

//			walker->fSY = (float)walker->iSY+0.015f;

//			walker->Rotated.Z = z_value;
//			walker->ooz = ooz_value;
//			walker->oow = oow_value;
//			walker++->Light = light_value;
//		}

//		mid_x--;

//		while(walker < &Flir3DPoints[HALF_NUM_FLIR_VERTEXES*2])
//		{
//			walker->Flags = PT3_ROTATED | PT3_PERSPECTED | PT3_OOW_SET;
//			xwalker--;
//			walker->iSX = mid_x- *xwalker;

//			if (walker->iSX < GrBuffFor3D->ClipLeft)
//			{
//				walker->iSX = GrBuffFor3D->ClipLeft;
//				clip_flags &= ~PT3_RXHIGH;
//			}
//			else
//				if (walker->iSX > GrBuffFor3D->ClipRight)
//				{
//					walker->iSX = GrBuffFor3D->ClipRight;
//					clip_flags &= ~PT3_RXLOW;
//				}
//				else
//					clip_flags &= ~(PT3_RXHIGH | PT3_RXLOW);

//			walker->fSX = (float)walker->iSX+0.015f;

//			ywalker--;
//			walker->iSY = mid_y + *ywalker;

//			if (walker->iSY < GrBuffFor3D->ClipTop)
//			{
//				walker->iSY = GrBuffFor3D->ClipTop;
//				clip_flags &= ~PT3_RYHIGH;
//			}
//			else
//				if (walker->iSY > GrBuffFor3D->ClipBottom)
//				{
//					walker->iSY = GrBuffFor3D->ClipBottom;
//					clip_flags &= ~PT3_RYLOW;
//				}
//				else
//					clip_flags &= ~(PT3_RYHIGH | PT3_RYLOW);

//			walker->fSY = (float)walker->iSY+0.015f;

//			walker->Rotated.Z = z_value;
//			walker->ooz = ooz_value;
//			walker->oow = oow_value;
//			walker++->Light = light_value;
//		}


//		if (!clip_flags)
//		{
////			HUDColor = 63;

//			if (_3dxl)
//			{
//				unsigned char tmppal[256*3];

//				if (UFC.NavFlirStatus == 1)
//					_3dxlPrepareForSpecialPalette(tmppal,MONO_GREEN_TERRAIN_TABLE,FX_GREENPAL);
//				else
//					_3dxlPrepareForSpecialPalette(tmppal,MONO_GREEN_TERRAIN_TABLE,FX_WGREENPAL);

//				_3dxlSetSpecialFoggingColor(0x9F);

//				StartRenderingFunctions();

//				_3dxlStartZBuffer();

//				DrawPolyNoClip(PRIMITIVE_MONO_SMOOTH | PRIMITIVE_NO_PRE_PROCESSING,HALF_NUM_FLIR_VERTEXES*2,&pFlir3DPoints[0],0x9F);

//				_3dxlTurnOffZBufferWrites();

//				GrBuffFor3D->ClipLeft	= HUDPushBuff.ClipLeft;
//				GrBuffFor3D->ClipRight	= HUDPushBuff.ClipRight;
//				GrBuffFor3D->ClipTop	= HUDPushBuff.ClipTop;
//				GrBuffFor3D->ClipBottom = HUDPushBuff.ClipBottom;

//				Set3DScreenClip();

//				WorldPrimed = 1;

//				DoingFlir = 0xDF;
//				RenderWorld(Camera1.CameraLocation);
//				DoingFlir = FALSE;

//				_3dxlStopZBuffer();

//				StopRenderingFunctions();

//				_3dxlFinishSpecialPalette(tmppal);

//			}
//			else
//			{
//				old_fading_table = FadingTableToUse;
//				SelectFadingTable(MONO_GREEN_TERRAIN_TABLE);

//  				if (!ThreeDOutputBuffer)
//  				{
//	 				GrGrabSurface(GrBuffFor3D);
//	 				ThreeDOutputBuffer = GrGetBuffPointer(GrBuffFor3D);
//  				}

//				ShortenFPU();

//				DrawPolyNoClip(PRIMITIVE_GLASS | PRIMITIVE_NO_PRE_PROCESSING,HALF_NUM_FLIR_VERTEXES*2,&pFlir3DPoints[0],(int)(16.0f * LIGHT_MULTER));

//  				RestoreFPU();

//  				if (ThreeDOutputBuffer)
//  				{
//					GrReleaseSurface(GrBuffFor3D);
//					ThreeDOutputBuffer = 0;
//  				}

//				FadingTableToUse = old_fading_table;

//			}

//		}

//		GrBuffFor3D->ClipLeft	= old_left;
//		GrBuffFor3D->ClipRight	= old_right;
//		GrBuffFor3D->ClipTop	= old_top;
//		GrBuffFor3D->ClipBottom	= old_bottom;
//		GrBuffFor3D->MidX		= old_midx;
//		GrBuffFor3D->MidY		= old_midy;

//		if (CurrentCockpit == VIRTUAL_MODEL)
//			Set3DScreenClip();
//	}
}



//*****************************************************************************************************************************************
// NAV MODE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void HudSteering(PlaneParams *P)
{


}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudLanding(PlaneParams *P)
{
	if (UFC.ILSStatus)
		DisplayILS(P);

	if( !(P->FlightStatus & PL_GEAR_DOWN_LOCKED) ) return;

	float TempAOA = AOA_unit;

	if(TempAOA > 9.3) TempAOA = 9.3;
	if(TempAOA < 6.9) TempAOA = 6.9;

	float Diff,Frac,Dist;

	if(TempAOA >= 8.1)
	{
		 Diff = TempAOA - 8.1;
		 Frac = Diff/1.2;
		 Dist = Frac * 7.0;  // 7 pixels on each side

		 float TempY = VelVectorY + Dist;

	   CRTline(VelVectorX-10,TempY-7,VelVectorX-10,TempY+7);
	   CRTline(VelVectorX-10,TempY-7,VelVectorX-7,TempY-7);
	   CRTline(VelVectorX-10,TempY,VelVectorX-7,TempY);
	   CRTline(VelVectorX-10,TempY+7,VelVectorX-7,TempY+7);
	}
	else
	{
		 Diff = 8.1 - TempAOA;
		 Frac = Diff/1.2;
		 Dist = Frac * 7.0;  // 7 pixels on each side

		 float TempY = VelVectorY - Dist;

	   CRTline(VelVectorX-10,TempY-7,VelVectorX-10,TempY+7);
	   CRTline(VelVectorX-10,TempY-7,VelVectorX-7,TempY-7);
	   CRTline(VelVectorX-10,TempY,VelVectorX-7,TempY);
	   CRTline(VelVectorX-10,TempY+7,VelVectorX-7,TempY+7);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void UpdateAATarget(PlaneParams *P);
void DisplayAAradar(PlaneParams *P);

void DisplayNavHUD( PlaneParams *P )
{
	HudSteering(P);
	HudLanding(P);
	if( !HudDeclutter)  //reject symbol button
		DisplayBankAngle(P);

	UpdateAATarget(P);
	DisplayAAradar(P);
	//DisplayRangeWin(P);
	//DisplayVcWin(P);
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayInstHUD()
 *
 */
void DisplayInstHUD( PlaneParams *P )
{
	/*
	DisplaySteering();
	DisplayVerticalVelocity(P);

	if (LeftDownSwitchState == SWITCH_HUD_DECLUTTER_OFF)	// declutter HUD
		DisplayBankAngle(P);

	if (UFC.ILSStatus)
		DisplayILS(P);
	*/
}

//*****************************************************************************************************************************************
// AA INIT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void AA_init()
{
	AA_Tmin = AA_Tmax = AA_Tpre = AA_Tpost = AA_Tact = AA_AimPhase = AA_MissileInFlight = 0;

  AA_TMem = -1;

	AA_weapID = 0;
	AA_weapDB = NULL;

	PrevAAWeapMode = -1;

	AA_PrimaryPosition.X = -1;
	AA_SecondaryPosition.X = -1;

	RocketCCIPTimer.Disable();
  LastVV.X = -1;
  LastRocketPipper.X = -1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AA_reset()
{
	AA_Tmin = AA_Tmax = AA_Tpre = AA_Tpost = AA_Tact = AA_AimPhase = AA_MissileInFlight = 0;

  AA_TMem = -1;

	AA_weapID = 0;
	AA_weapDB = NULL;

	PrevAAWeapMode = -1;
}



/*-----------------------------------------------------------------------------
 *
 *	DrawRmaxTick()
 *
 */
void DrawRmaxTick( float scale, float Rval )
{
	if (Rval==0 || Rval > scale)  return;

	int PosY = (scale-Rval) * 80 / scale;	// Rmax position on ruler

	HUDline( WINDOW_10(1,PosY+0), WINDOW_10(4,PosY+0) );
	HUDline( WINDOW_10(1,PosY-1), WINDOW_10(4,PosY-1) );
}

/*-----------------------------------------------------------------------------
 *
 *	DrawRtrTick()
 *
 */
void DrawRtrTick( float scale, float Rval )
{
	if( (Av.Weapons.CurrWeapMode != AIM7_STAT) && (Av.Weapons.CurrWeapMode != AIM120_STAT) )  return;

	if (Rval==0 || Rval > scale)  return;

	int PosY = (scale-Rval) * 80 / scale;	// Rmax position on ruler

	HUDline( WINDOW_10(1,PosY+0), WINDOW_10(4,PosY+0) );
	HUDline( WINDOW_10(1,PosY-1), WINDOW_10(4,PosY-1) );

	int yMin = (scale-AA_Rmin) * 80 / scale;	// Rmin position on ruler

	HUDline( WINDOW_10(4,PosY), WINDOW_10(4,yMin) );
}

/*-----------------------------------------------------------------------------
 *
 *	DrawMarDot()
 *
 */
void DrawMarDot( float scale, float Rval )
{
	if (Rval==0 || Rval > scale)  return;

	int PosY = (scale-Rval) * 80 / scale;	// Rmax position on ruler

	HUDicon( WINDOW_10(0,PosY), SM_DOT_5 );
}

/*-----------------------------------------------------------------------------
 *
 *	DrawHUDstick()
 *
 *		Aspect angle stick on missile FOV circle.
 *
 */
void DrawHUDstick( int X, int Y, int radius, int length, float angle )
{
	angle *= DEGREES_TO_RADIANS;

	radius *= HUDResMul;

	float fX = cos( angle );
	float fY = sin( angle );

	int x1 = round( fX * radius );
	int y1 = round( fY * radius );

	radius += length;

	int x2 = round( fX * radius );
	int y2 = round( fY * radius );

	HUDline( X-y1,Y-x1, X-y2,Y-x2 );
}

//*****************************************************************************************************************************************
// DRAW BREAK X  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawBreakX()
{
	if( HudFlashTimer.TimeUp(GameLoopInTicks) )
	{
		AA_flash = !AA_flash;
		HudFlashTimer.Set(0.25,GameLoopInTicks);
	}

	if (AA_flash)
	{
		HUDline( HUD_BREAKX_WIN(-27,-27), HUD_BREAKX_WIN( 27, 27) );
		HUDline( HUD_BREAKX_WIN(-27, 27), HUD_BREAKX_WIN( 27,-27) );
	}
}

//*****************************************************************************************************************************************
// DRAW TD BOX  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************
int PosX_TD;
int PosY_TD;


void DrawTDbox( PlaneParams *P, int shootQ,int *PosSet, int *X, int *Y )
{
	int		PosX, PosY;
	int		SecondaryPosX, SecondaryPosY;

	FPoint	tVector;

	*PosSet = FALSE;

	if(AA_TMem == -1)
	{
		if(AA_PrimaryPosition.X == -1)
			return;

		tVector = AA_PrimaryPosition;
	}
	else
		tVector  = AA_Mem_LastPosition;

	tVector -= P->WorldPosition;
	tVector *= ViewMatrix;
	tVector.Perspect( &PosX, &PosY );

	PosX_TD = PosX;
	PosY_TD = PosY;



	// Keep TD box at edge of HUD
	//ClipToHUD( &PosX, &PosY );
	int doflash = 0;
	if ((PosX-8) < GrBuffFor3D->ClipLeft  )
	{
		PosX = GrBuffFor3D->ClipLeft+8;
		doflash = 1;
	}
	else if ((PosX+8)> GrBuffFor3D->ClipRight )
	{
		PosX = GrBuffFor3D->ClipRight-8;
		doflash = 1;
	}


	if ((PosY-8) < GrBuffFor3D->ClipTop   )
	{
		PosY = GrBuffFor3D->ClipTop + 8;
		doflash = 1;
	}
	else if ((PosY+8) > GrBuffFor3D->ClipBottom)
	{
		PosY = GrBuffFor3D->ClipBottom - 8;
		doflash = 1;
	}


	static float FlashTime=0;
	float flashrate = 0.70f;

	FlashTime += seconds_per_frame;

	float subamt = 8*HUDResMul;
	float addamt = 9*HUDResMul;

	// if on edge flash...
	if( doflash )
	{
		if( FlashTime> flashrate )
			CRTbox( PosX-subamt, PosY-subamt, PosX+addamt, PosY+addamt );
	}
	else
		CRTbox( PosX-subamt, PosY-subamt, PosX+addamt, PosY+addamt );



	*PosSet = TRUE;
	*X = PosX;
	*Y = PosY;

	AA_TDBoxX  = PosX;
	AA_TDBoxY  = PosY;

	//if( GetAARadarMode() != AA_STT_MODE)

	// Break X the TD box if friendly
 	if( ( GetAARadarMode() == AA_STT_MODE) && (P->AI.iSide == AA_target->AI.iSide) )
	{
		if( doflash )
		{
			if( FlashTime> flashrate )
			{
			CRTline( PosX-subamt, PosY-subamt, PosX+addamt, PosY+addamt );
			CRTline( PosX-subamt, PosY+addamt, PosX+addamt, PosY-subamt );
			}
		}
		else
		{
			CRTline( PosX-subamt, PosY-subamt, PosX+addamt, PosY+addamt );
			CRTline( PosX-subamt, PosY+addamt, PosX+addamt, PosY-subamt );
		}
	}

	if( FlashTime> flashrate )
		FlashTime = 0.0f;


	if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7M_Id) || (AA_weapID == Aim7MH_Id) )
	{
		if( GetAARadarMode() != AA_STT_MODE)
				GrDrawStringClipped( GrBuffFor3D, LgHUDFont,PosX-12,PosY+addamt+1, "GO STT", HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue,0);
	}

	// now the secondary target
	if(AA_SecondaryPosition.X != -1)
	{
		tVector = AA_SecondaryPosition;
	  tVector -= P->WorldPosition;
	  tVector *= ViewMatrix;
	  tVector.Perspect( &SecondaryPosX, &SecondaryPosY );
	  float halfadd = addamt *0.5f;
	  float halfsub = subamt *0.5f;

		CRTline( SecondaryPosX-halfsub, SecondaryPosY-halfsub, SecondaryPosX+halfadd, SecondaryPosY+halfadd );
		CRTline( SecondaryPosX-halfsub, SecondaryPosY+halfadd, SecondaryPosX+halfadd, SecondaryPosY-halfsub );
	}

	if (shootQ)		// flash cue @ 2Hz w/ 50% duty cycle
	{
		if( HudFlashTimer.TimeUp(GameLoopInTicks) )
		{
			AA_flash = !AA_flash;
			HudFlashTimer.Set(0.25,GameLoopInTicks);
		}

		if ( ( (Av.Weapons.CurrWeapMode == AIM7_STAT) || (Av.Weapons.CurrWeapMode == AIM120_STAT) ) && (AA_range > AA_Rtr))
			AA_flash = TRUE;	// MRMs only flash within RTR
		else if ((Av.Weapons.CurrWeapMode == AIM9_STAT) && (AA_range > AA_Rtr))
			AA_flash = TRUE;	// SRMs only flash within RTR

		 int Draw = !( (AA_weapID == Aim120_Id) && (WeapStores.Aim120VisualOn) );
		 if(!Draw)
		   GrDrawStringClipped( GrBuffFor3D, LgHUDFont,PosX-14,PosY+addamt+1, "NO RDR", HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);

		 if(AA_flash)
		 {
			 // kludge..no shoot cue for aim120 visual mode
			 if(Draw && (UFC.MasterMode == AA_MODE))
			   GrDrawStringClipped( GrBuffFor3D, LgHUDFont,PosX-12,PosY-subamt-9, "SHOOT", HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);
		 }
	}

	//	Save TD position for IR seeker position next frame.
	//	This will give a nice slew between TD and IR symbols.
	//
	if (AA_slave && AA_SCAN)
	{
		AA_PosX = PosX - TranX;	// HUD normalize
		AA_PosY = PosY - TranY;
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayAASearchModeSymbology()
 *
 */
void DisplayAASearchModeSymbology( PlaneParams *P )
{
	switch (UFC.AASearchMode)
	{
		case SUPER_SEARCH_MODE:		HUDcircle( RESD_NUM_XY(320,153), 82*HUDResMul );			break;
		case BORE_SIGHT_SHORT_MODE:	HUDcircle( RESD_NUM_XY(320,117), 17*HUDResMul );			break;
		case BORE_SIGHT_LONG_MODE:	HUDcircle( RESD_NUM_XY(320,117), 17*HUDResMul );			break;

		case VERT_SCAN_MODE:		HUDline( RESD_NUM_XY(320, 117), RESD_NUM_XY(320,HUD_TOP) );	break;
		case GUN_RDR_MODE:	break;
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayGuns()
 *
 */
void DisplayGuns( PlaneParams *P, char *Str )
{
	HUDtextLg( WINDOW_17(30,0), Str );

	HUDtextLg( WINDOW_5(0,0), "%d", P->WeapLoad[GUNS_STATION].Count );
}

//*****************************************************************************************************************************************
// DRAW NIRD  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawNIRDTriangle(float X,float Y,float Radius,float Degree)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin;

	Radius *= HUDResMul;

	ProjectPoint(X,Y,NormDegree(Degree),Radius+1,&Origin.X,&Origin.Y);

	Degree = NormDegree(Degree - 15);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10*HUDResMul,&NewX1,&NewY1);

	Degree = NormDegree(Degree + 30);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10*HUDResMul,&NewX2,&NewY2);

	HUDline(Origin.X,Origin.Y,NewX1,NewY1);
	HUDline(Origin.X,Origin.Y,NewX2,NewY2);
	HUDline(NewX1,NewY1,NewX2,NewY2);
}

void DrawNIRDTriangleScreen(float X,float Y,float Radius,float Degree)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin;

	Radius *= HUDResMul;

	ProjectPoint(X,Y,NormDegree(Degree),Radius+1,&Origin.X,&Origin.Y);

	Degree = NormDegree(Degree - 15);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10*HUDResMul,&NewX1,&NewY1);

	Degree = NormDegree(Degree + 30);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),10*HUDResMul,&NewX2,&NewY2);

	GrDrawLineClipped( GrBuffFor3D, Origin.X,Origin.Y,NewX1,NewY1, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, Origin.X,Origin.Y,NewX2,NewY2, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, NewX1,NewY1,NewX2,NewY2,	   HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );

}
//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawNIRDDiamond(float X,float Y,float Radius,float Degree)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin,Origin2;
	float orgDegree = Degree;

	Radius *= HUDResMul;

	ProjectPoint(X,Y,NormDegree(Degree),Radius+1,&Origin.X,&Origin.Y);

	Degree = NormDegree(Degree - 15);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6*HUDResMul,&NewX1,&NewY1);  //  was 10

	Degree = NormDegree(Degree + 30);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6*HUDResMul,&NewX2,&NewY2);  //  was 10

	ProjectPoint(Origin.X,Origin.Y,NormDegree(orgDegree),10.4*HUDResMul,&Origin2.X,&Origin2.Y);  //  was 19.3   // 19.3 = 10*cos(30degrees)*2

	HUDline(Origin.X,Origin.Y,NewX1,NewY1);
	HUDline(Origin.X,Origin.Y,NewX2,NewY2);
	HUDline(NewX1,NewY1,Origin2.X,Origin2.Y);
	HUDline(NewX2,NewY2,Origin2.X,Origin2.Y);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawNIRDTick(float X,float Y,float Radius,float Degree,int Inside)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin;
	float OriginalDegree = Degree;

	Radius *= HUDResMul;

	ProjectPoint(X,Y,NormDegree(Degree),Radius,&Origin.X,&Origin.Y);


	if(Inside)
	  Degree = NormDegree(Degree+180);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6*HUDResMul,&NewX1,&NewY1);
	HUDline(Origin.X,Origin.Y,NewX1,NewY1);

	Degree = NormDegree(Degree+1);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6*HUDResMul,&NewX2,&NewY2);
	HUDline(Origin.X,Origin.Y,NewX2,NewY2);
}

void DrawNIRDTickScreen(float X,float Y,float Radius,float Degree,int Inside)
{
	float NewX1,NewY1,NewX2,NewY2;
	FPoint Origin;
	float OriginalDegree = Degree;

	Radius *= HUDResMul;

	ProjectPoint(X,Y,NormDegree(Degree),Radius,&Origin.X,&Origin.Y);

	if(Inside)
	  Degree = NormDegree(Degree+180);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6*HUDResMul,&NewX1,&NewY1);
	GrDrawLineClipped( GrBuffFor3D, Origin.X,Origin.Y,NewX1,NewY1, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );

	Degree = NormDegree(Degree+1);
	ProjectPoint(Origin.X,Origin.Y,NormDegree(Degree),6*HUDResMul,&NewX2,&NewY2);
	GrDrawLineClipped( GrBuffFor3D, Origin.X,Origin.Y,NewX1,NewY1, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );

}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float RTRTAngle = 190.0f;

#if 1
#define RAERO_ANGLE		150.0f
#define RMAX_ANGLE		180.0f
#define RNE_ANGLE		220.0f
#define RMIN_ANGLE		315.0f
void DrawNIRDTicks(int Radius)
{
	float DegreePerRange;
	float RangeTickDeg;
	float gunrange;

	// RMin,RMax,RAero
//	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,285.5);
	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,RMIN_ANGLE);
	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,RNE_ANGLE);
	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,RMAX_ANGLE);
	DrawNIRDDiamond(HUD_CENTER(0,0),Radius,RAERO_ANGLE);

	// Range
	if(AA_range < AA_Rmin)
	{
		DegreePerRange = (RMIN_ANGLE - RAERO_ANGLE)/(AA_Rmax-AA_Rmin);
		RangeTickDeg   = RMIN_ANGLE - (AA_range - AA_Rmin)*DegreePerRange;
	}
	else if(AA_range < AA_Rtr)
	{
		DegreePerRange = (RMIN_ANGLE - RNE_ANGLE)/(AA_Rtr-AA_Rmin);
		RangeTickDeg   = RMIN_ANGLE - (AA_range - AA_Rmin)*DegreePerRange;
	}
	else if(AA_range < AA_ROpt)
	{
		DegreePerRange = (RNE_ANGLE - RMAX_ANGLE)/(AA_ROpt-AA_Rtr);
		RangeTickDeg   = RNE_ANGLE - (AA_range - AA_Rtr)*DegreePerRange;
	}
	else if(AA_range < AA_Rmax)
	{
		DegreePerRange = (RMAX_ANGLE - RAERO_ANGLE)/(AA_Rmax-AA_ROpt);
		RangeTickDeg   = RMAX_ANGLE - (AA_range - AA_ROpt)*DegreePerRange;
	}
	else
	{
		DegreePerRange = (RMIN_ANGLE - RAERO_ANGLE)/(AA_Rmax-AA_Rmin);
		RangeTickDeg   = RMIN_ANGLE - (AA_range - AA_Rmin)*DegreePerRange;
	}


	if(RangeTickDeg < 10)
		RangeTickDeg = 10;
	else if(RangeTickDeg > 360.0)
		RangeTickDeg = 360.0;

	RangeTickDeg = NormDegree(RangeTickDeg);
	DrawNIRDTick(HUD_CENTER(0,0),Radius,RangeTickDeg,TRUE);

	// Gun Tick
	int AARadarMode = GetAARadarMode();
	if(AARadarMode != -1)
	{
		if(AARadarMode == AA_STT_MODE)
		{
			gunrange = (12000*FTTONM);
			if( AA_range <= gunrange )
			{
				if(gunrange < AA_Rmin)
				{
					DegreePerRange = (RMIN_ANGLE - RAERO_ANGLE)/(AA_Rmax-AA_Rmin);
					RangeTickDeg   = RMIN_ANGLE - (gunrange - AA_Rmin)*DegreePerRange;
				}
				else if(gunrange < AA_Rtr)
				{
					DegreePerRange = (RMIN_ANGLE - RNE_ANGLE)/(AA_Rtr-AA_Rmin);
					RangeTickDeg   = RMIN_ANGLE - (gunrange - AA_Rmin)*DegreePerRange;
				}
				else if(gunrange < AA_ROpt)
				{
					DegreePerRange = (RNE_ANGLE - RMAX_ANGLE)/(AA_ROpt-AA_Rtr);
					RangeTickDeg   = RNE_ANGLE - (gunrange - AA_Rtr)*DegreePerRange;
				}
				else if(gunrange < AA_Rmax)
				{
					DegreePerRange = (RMAX_ANGLE - RAERO_ANGLE)/(AA_Rmax-AA_ROpt);
					RangeTickDeg   = RMAX_ANGLE - (gunrange - AA_ROpt)*DegreePerRange;
				}
				else
				{
					DegreePerRange = (RMIN_ANGLE - RAERO_ANGLE)/(AA_Rmax-AA_Rmin);
					RangeTickDeg   = RMIN_ANGLE - (gunrange - AA_Rmin)*DegreePerRange;
				}

				 if(RangeTickDeg < 10)
						RangeTickDeg = 10;
				 else if(RangeTickDeg > 360.0)
						RangeTickDeg = 360.0;


			//TW bug 776 rmax circle que
			//	float angle = (AA_range/gunrange)  * 360.0f;
			//	float scale = 1.0f/255.0f;
			//	GrDrawPolyCircleArc( GrBuffFor3D, HUD_CENTER(0,0), Radius-1, Radius, 0.0f,  angle, 30, 1.0f,0.0f, 0.0f, 0 );//HUDColorRGB->Red* scale,HUDColorRGB->Green* scale,HUDColorRGB->Blue * scale,0);


				RangeTickDeg = NormDegree(RangeTickDeg);
				DrawNIRDTick(HUD_CENTER(0,0),Radius,RangeTickDeg,FALSE);
			}
		}
	}
}
#else
/*
void DrawNIRDTicks(int Radius)
{
	// RMin,RMax,RAero
//	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,285.5);
	DrawNIRDDiamond(HUD_CENTER(0,0),Radius,285.5);
	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,180.0);
	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,225.0);

	// Range
	float DegreePerRange = 105.0/(AA_Rmax-AA_Rmin);
	float RangeTickDeg   = 285.0 - (AA_range - AA_Rmin)*DegreePerRange;

	if(RangeTickDeg < 10)
		RangeTickDeg = 10;
	else if(RangeTickDeg > 360.0)
		RangeTickDeg = 360.0;

	RangeTickDeg = NormDegree(RangeTickDeg);
	DrawNIRDTick(HUD_CENTER(0,0),Radius,RangeTickDeg,TRUE);

	// ROpt
	RangeTickDeg   = 285.0 - (AA_ROpt - AA_Rmin)*DegreePerRange;
//	RangeTickDeg   = 285.0 - (AA_Rtr - AA_Rmin)*DegreePerRange;

	if(RangeTickDeg < 10)
		RangeTickDeg = 10;
	else if(RangeTickDeg > 360.0)
		RangeTickDeg = 360.0;

	RangeTickDeg = NormDegree(RangeTickDeg);
//	DrawNIRDTriangle(HUD_CENTER(0,0),Radius,RangeTickDeg);

	// Gun Tick
	int AARadarMode = GetAARadarMode();
	if(AARadarMode != -1)
	{
		if(AARadarMode == AA_STT_MODE)
		{
			if( AA_range <= (12000*FTTONM) )
			{
				 RangeTickDeg   = 285.0 - ((12000*FTTONM) - AA_Rmin)*DegreePerRange;

				 if(RangeTickDeg < 10)
						RangeTickDeg = 10;
				 else if(RangeTickDeg > 360.0)
						RangeTickDeg = 360.0;

					RangeTickDeg = NormDegree(RangeTickDeg);
					DrawNIRDTick(HUD_CENTER(0,0),Radius,RangeTickDeg,FALSE);
			}
		}
	}
}
*/
#endif

//*****************************************************************************************************************************************
// DRAW ASE CIRCLE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawASEcircle( int radius )
{
	AA_DrawASE = TRUE;
	AA_ASESize = radius;

	if(PlayerPlane->AADesignate == NULL) return;

	DrawNIRDTicks(radius);

	float ASEsize = 1.0;

	if ( (Av.Weapons.CurrWeapMode == AIM7_STAT) || (Av.Weapons.CurrWeapMode == AIM120_STAT) )
	{


		/*
		//	Radius is dependent upon range and aspect as follows:
		//		increases  until 0.75 Rmax1
		//		then fixed until 0.25 Rmax1
		//		then +/- depending on aspect angle
		//
		ASEsize *= 0.75 * AA_Rmax / AA_range;

		if (ASEsize > 1)  ASEsize = 1;

		if (AA_range < AA_Rmax * 0.25)	// [0.75 .. 1.25]
		{
			float mod = fabs( AA_aspect );
			if (mod > 90)  mod = 180 - mod;
			mod = (45 - mod) / 45;	// +1 @ H/T; 0 @ 45°; -1 * 90°
			ASEsize += mod / 4;
		}

  */
	}


	// Allowable Steering Error (ASE) circle.
	//
	AA_ASEsize = radius * ASEsize;

	HUDcircle( WINDOW_0(0,0), AA_ASEsize * HUDResMul);

	/*
	if (AA_weapID == Aim120_Id)
		HUDcircleDash( WINDOW_0(0,0), AA_ASEsize );
	else
		HUDcircle( WINDOW_0(0,0), AA_ASEsize );
	*/

	// Lead Angle Error (LAE) steering dot.
	// Fake to be 1/4 way from watermark to target.
	AA_DotX = (PosX_TD - (HUD_MIDX+TranX) ) / 4;
	AA_DotY = (PosY_TD - (HUD_MIDY+TranY) ) / 4;

	//AA_DotX = (AA_PosX - HUD_MIDX) / 4;
	//AA_DotY = (AA_PosY - HUD_MIDY) / 4;


	// flash steering dot if target is within 15 degrees of 70 azimuth
	int DrawDot = TRUE;
	int PrimaryId = GetCurrentPrimaryTarget();

	if ((!IsWeapCaged()) && ((AA_weapID == Aim9X_Id) || (AA_weapID == Aim9M_Id) || (AA_weapID == Aim9L_Id)))
	{
		if(!AA_lockon)
		{
			DrawDot = FALSE;
		}
		else
		{
			if(PrimaryId != (AA_lockon - Planes))
			{
				DrawDot = FALSE;
			}
		}
	}

	if((PrimaryId != -1) && (DrawDot))
	{
		float DeltaAzim = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Planes[PrimaryId].WorldPosition,TRUE);
		if( fabs(DeltaAzim) > 55.0 )
		{
			if( HudFlashTimer.TimeUp(GameLoopInTicks) )
			{
				AA_flash = !AA_flash;
				HudFlashTimer.Set(0.25,GameLoopInTicks);
			}
			DrawDot = (AA_flash);
		}
	}

	if(DrawDot)
	  HUDicon( WINDOW_0(AA_DotX,AA_DotY), SM_DOT_6 );
}

/*-----------------------------------------------------------------------------
 *
 *	AAClosureRate()
 *
 *		Return TRUE rate of closure between
 *		me and my AA target.  (+/- KTS)
 *
 */
float AAClosureRate( PlaneParams *P, PlaneParams *T )
{
	FPoint tVector;

	tVector  = T->WorldPosition;
	tVector -= P->WorldPosition;
	tVector.Normalize();		// unit vector ME -> target

	float V1 = P->IfVelocity * tVector;	// my speed toward target
	float V2 = T->IfVelocity * tVector;	// target's speed away from me

	return (V1 - V2) * FTSEC_TO_KNOTS;
}

/*-----------------------------------------------------------------------------
 *
 *	AGClosureRate()
 *
 *		Return TRUE rate of closure between
 *		me and my AG target.  (+/- KTS)
 *
 */
float AGClosureRate( PlaneParams *P, FPointDouble &T )
{
	FPoint tVector;

	tVector  = T;
	tVector -= P->WorldPosition;
	tVector.Normalize();		// unit vector ME -> target

	double V1 = P->IfVelocity * tVector;	// my speed toward target

	return V1 * FTSEC_TO_KNOTS;
}

//*****************************************************************************************************************************************
// TARGET MEM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SetBrokeLockHudTimer(float Time,FPointDouble LastKnownPos)
{
  AA_TMem = Time;
  AA_Mem_LastPosition = LastKnownPos;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ResetBrokeLockHudTimer()
{
 AA_TMem = -1;
}

//*****************************************************************************************************************************************
// DISPLAY AA RADAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayAAradar( PlaneParams *P )
{
	if (!AA_target || AA_AOT)  return;

 	int AAtens = abs( round(AA_aspect/10) );

	// Format PDT altitude
	sprintf( TmpStr, "%04d  ", AA_altitude/10 );
	TmpStr[3] = TmpStr[2];
	TmpStr[2] = '-';
	if (TmpStr[0] == '0')  TmpStr[0] = ' ';

	if (!IsWeapCaged())  strcat( TmpStr, "UNC" );
	else
	{
		char *pStr = TmpStr + 6;	// point to end of PDT altitude string

		if      (AAtens ==  0)  strcpy( pStr, " T" );
		else if (AAtens == 18)  strcpy( pStr, " H" );
		else	sprintf( pStr, "%2d%c", AAtens, AA_aspect>0 ? 'L' : 'R' );
	}

	//HUDtextLg( WINDOW_16(0,0), TmpStr );	// PDT altitude + aspect angle

	//HUDtextSm( HUD_RANGE_WIN(0,0), "R%5.1f", AA_range );
	//HUDtextSm( HUD_VC_WIN(0,0), "%4d", (int)AAClosureRate( P, AA_target ) );


	//	Radar range ruler
	int PosY = (AA_scale - AA_range) * 80 / AA_scale;	// rrange position on ruler

	//HUDicon( WINDOW_10( 0,0   ), SM_RADAR_RULE );
	//HUDicon( WINDOW_10(-1,PosY), SM_RADAR_MARK );

	//HUDtextSm( WINDOW_10(-19,PosY-2), "%4d", (int)AAClosureRate( P, AA_target ) );

	//HUDtextSm( WINDOW_10(-6,-6), "%3d", AA_scale   );
	//HUDtextSm( WINDOW_10( 4,38), "%d",  AA_scale/2 );	// left-justify the mid-scale

	//DrawRmaxTick( AA_scale, AA_Rmax );
	//DrawRmaxTick( AA_scale, AA_Rmin );
	//DrawRtrTick(  AA_scale, AA_Rtr  );

	//if (AA_weapID == Aim120_Id)  DrawMarDot( AA_scale, AA_Rmar );

	int PosSet,X,Y;
	DrawTDbox( P, AA_shootQ,&PosSet,&X,&Y);
}

/*-----------------------------------------------------------------------------
 *
 *	Draw_AIM9_Pipper() @ HUD center
 *
 */
void Draw_AIM9_Pipper()
{
	//	If tracking and within radar reticle range, show it.
	//
	float range = 360 * AA_range / AA_RANGER;	// convert to [0..360]
	float Rmin  = 360 * AA_Rmin  / AA_RANGER;

	HUDiconCmp( WINDOW_0(0,0), SM_PIPPER_TELLTALE, (int)range*80/360 );
	DrawHUDstick(  WINDOW_0(0,0), HUD_DEGREE, 4, -range );	// end tab
	DrawHUDstick(  WINDOW_0(0,0), HUD_DEGREE, 4, -Rmin  );	// Rmin tick
}

static int IR_PosX, IR_PosY;
/*-----------------------------------------------------------------------------
 *
 *	AIM9_IR_level()
 *
 *		Calculate the seeker's IR level for this target.
 *
 */
float AIM9_IR_level( PlaneParams *P, PlaneParams *T )
{
	float fRange;	// target range factor [0..1]
	float fAngle;	// FOV angle factor [0..1]
//	float fHeat;	// target heading factor [0..1]
//	float range;	// target distance (nm)
	float angle;	// FOV angle (degrees)
	int	  checkfov = 1;

	float Rmax = AA_Rmax * NMTOWU;	// distance threshold (WU)

	//	Create vector to target
	FPoint tVector;

	tVector  = T->WorldPosition;
	tVector -= P->WorldPosition;
	tVector *= ViewMatrix;

	if (!tVector.Perspect( &IR_PosX, &IR_PosY ))  return 0;	// behind me

	if(!AA_lockon)
	{
		// Past gimbal limits of IR seeker
		if (!CheckInHUD( IR_PosX, IR_PosY ))  return 0;
	}

	// Check for occlusions  (Moved into WCheckIRAspect
	// if (!AILOS( P->WorldPosition, T->WorldPosition ))  return 0;

	IR_PosX -= TranX;	// HUD normalize
	IR_PosY -= TranY;

	//	FOV angle factor [0..1], 1 inside FOV circle, 0 @ 2*radius

	if((AA_weapID == Aim9X_Id) && (!IsWeapCaged()))
	{
		if(((Camera1.SubType & (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) == (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) && (Camera1.TargetObjectType == CO_PLANE) && (Camera1.TargetObject))
		{
			if((PlaneParams *)Camera1.TargetObject)
			{
				checkfov = 0;
			}
		}
		else if(AA_lockon)
		{
			checkfov = 0;
		}
	}
	else if(!IsWeapCaged())
	{
		if(AA_lockon)
		{
			checkfov = 0;
		}
	}
	else if(P->AADesignate)
	{
		checkfov = 0;
	}

	if(checkfov)
	{
		 angle = hypot( IR_PosX - AA_PosX, IR_PosY - AA_PosY ) / HUD_DEGREE;
		fAngle = AA_irFOV - angle;
		if (fAngle >  1)  fAngle = 1;	// clip to 1.0
		if (fAngle <= 0)  return 0;
	}
	else
	{
		fAngle = 1;
	}


	//  Range factor [0..1], 1 inside Rmax, 0 @ 2*Rmax
//	 range = tVector.Normalize();
//	fRange = (1.5*Rmax - range) / Rmax;

	AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

	if(CurAAWeap)
	{
		fRange = WCheckIRAspect(&pDBWeaponList[CurAAWeap->W->WeapIndex], P, T);
	}
	else
	{
		fRange = 0;
	}
	if (fRange >  1)  fRange = 1;	// clip to 1.0
	if (fRange <= 0)  return 0;

#if 0	//  Moved this into WCheckIRAspect
	//	Heat factor [0..1]
	//	Afterburner doubles heat factor
	//	Speed doubles heat factor at [400..900] knots
	//  9P aspect angle attenuates factor to zero at 90°
	fHeat = 0.5;	// = T->Knots / 500;
	if (T->LeftThrustPercent > 80)  fHeat *= 1 + (T->LeftThrustPercent - 80) / 20;
	if (T->Knots > 400)				fHeat *= 1 + (T->Knots - 400) / 500;
//  *************
//  WRONG! WRONG! WRONG!  This is for the AIM-9P ONLY!  See old comment above.  Since we can't carry that anyway I'm just commenting this out.  SRE
//	if (AA_weapID == Aim9X_Id)		fHeat *= T->Orientation.I * tVector;
//  *************
	if (fHeat >  1)  fHeat = 1;		// clip to 1.0
	if (fHeat <= 0)  return  0;
#endif

//	HUDtextLg( WINDOW_20(-5,0), "%4.2f %4.2f %4.2f", fRange, fHeat, fAngle );
	// HUDtextLg( WINDOW_20(-5,0), "%4.2f %4.2f", fRange, fAngle );

//	return fAngle * fHeat * fRange;	// guaranteed [0..1]
	return fAngle * fRange;	// guaranteed [0..1]
}

/*-----------------------------------------------------------------------------
 *
 *	AIM9_IR_solar()
 *
 *		Calculate where the sun is relative to the plane.
 *
 *		Using David's code for this and modifying it for the sun.
 *
 */
float AIM9_IR_solar( PlaneParams *P )
{
	float sunoffyaw, sunoffpitch, sunval;
//	float sun_angle = atan2(WorldParams.SunY,-WorldParams.SunX);

	//	Create vector to target
	FPoint tVector;

//	tVector.SetValues(-1.0f,SunLight.VectorToLight);
	tVector.SetValues((100.0f * NMTOWU),SunLight.VectorToLight);
	tVector *= ViewMatrix;

	if (!tVector.Perspect( &IR_PosX, &IR_PosY ))  return 0;	// behind me

	// Past gimbal limits of IR seeker
	if (!CheckInHUD( IR_PosX, IR_PosY ))  return 0;

	IR_PosX -= TranX;	// HUD normalize
	IR_PosY -= TranY;

	sunval = HeatSeekerSunFactor( P->Heading + AIConvert180DegreeToAngle(fAim9SunOffYaw), P->Pitch + AIConvert180DegreeToAngle(fAim9SunOffPitch), P->WorldPosition, 5.0f, &sunoffyaw, &sunoffpitch);

	fAim9SunOffYaw = AICapAngle(fAim9SunOffYaw + sunoffyaw);
	fAim9SunOffPitch = AICapAngle(fAim9SunOffPitch + sunoffpitch);

	return(sunval);
}

/*-----------------------------------------------------------------------------
 *
 *	AIM9_IR_scan()
 *
 */
float AIM9_IR_scan( PlaneParams *P )
{
	float sunIR;
	float newIR = 0;
	float maxIR = 0;
	int sunlock = 0;

	if(AA_weapID == Aim9X_Id)
	{
		if(!IsWeapCaged())
		{
			if(((Camera1.SubType & (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) == (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) && (Camera1.TargetObjectType == CO_PLANE) && (Camera1.TargetObject))
			{
				AA_lockon = (PlaneParams *)Camera1.TargetObject;

				AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

				if(CurAAWeap)
				{
					if(WCheckIRAspect(&pDBWeaponList[CurAAWeap->W->WeapIndex], P, AA_lockon) <= 0)
						AA_lockon = NULL;
				}
				else
				{
					AA_lockon = NULL;
				}
			}
		}
	}

	if (!AA_lockon)		// find hottest IR source
	{
		for (PlaneParams *T=Planes; T<=LastPlane; T++)
		{
			if ((T->Status & PL_ACTIVE) == 0)  continue;

			if (T == P)  continue;	// skip me

			newIR = AIM9_IR_level( P, T );

			if (newIR > maxIR)
			{
				maxIR = newIR;
				AA_lockon = T;
			}
		}
	}

	if (AA_lockon)
	{
		newIR = AIM9_IR_level( P, AA_lockon );

		if (newIR > 0)
		{
			if (!IsWeapCaged())
			{
				AA_PosX = IR_PosX;	// save seeker position
				AA_PosY = IR_PosY;
			}
		}
		else
			AA_lockon = NULL;	// lost seeker lockon
	}

	sunIR = AIM9_IR_solar(P);

	if(AA_weapID == Aim9X_Id)
	{
		if(sunIR > 0.9f)
			sunIR = 0.9f;
	}

	if (sunIR > newIR)
	{
		newIR = sunIR;

		if((!IsWeapCaged()) && (AA_weapID != Aim9X_Id))
		{
			AA_PosX = IR_PosX;	// save seeker position
			AA_PosY = IR_PosY;
			sunlock = 1;
		}
		else
		{
			fAim9SunOffPitch = 0;
			fAim9SunOffPitch = 0;
		}
	}
	else
	{
		fAim9SunOffPitch = 0;
		fAim9SunOffPitch = 0;
	}

	if (IsWeapCaged() || (!AA_slave && !AA_lockon && !sunlock) )
	{
		AA_PosX = HUD_MIDX;		// return to missile boresight
		AA_PosY = HUD_MIDY+5;
	}

	return newIR;
}

/*-----------------------------------------------------------------------------
 *
 *	Snd_AIM9_Tone()
 *
 *		Sound AIM-9 audio tone (IR signal strength)
 *
 */
void Snd_AIM9_Tone( float IR_level )
{
	//int volume = IR_level * (g_iSoundLevelCaution-40);

	if(IR_level < 0.02f)
		IR_level = 0.02f;

	int volume = IR_level * (g_iSoundLevelCaution);

	if (volume > g_iSoundLevelCaution)
		volume = g_iSoundLevelCaution;

	if (volume > 0)
	{
		if (volume < 20)
			volume = 20;

		if (g_dwAIMSndHandle && !g_dwAIMLockSndHandle)
			SndSetSoundVolume( g_dwAIMSndHandle, volume );
		else
			g_dwAIMSndHandle = SndQueueSound( SND_AIM_9_GROWL, 0, volume );
//			g_dwAIMSndHandle = SndQueueSound( SideWinderGrowl, 0, volume );
	}
	else  KillSound( &g_dwAIMSndHandle );
}

/*-----------------------------------------------------------------------------
 *
 *	Snd_AIM9_Lock()
 *
 *		Sound AIM-9 audio lock
 *
 */
void Snd_AIM9_Lock(float tone_level)
{
	int locksound;
	//  I don't think we can use AA_shootQ since this looks at AA_target not AA_lockon
//	int local_AA_shootQ = 0;
//	double rdist;

//	if(AA_lockon)
//	{
//		rdist = (PlayerPlane->WorldPosition - AA_lockon->WorldPosition) * WUTONM;
//		local_AA_shootQ = AA_weapID && (rdist >= AA_Rmin) && (rdist <= AA_Rmax);
//	}

//	if (AA_lockon && local_AA_shootQ && !AA_CAGE)
	if (tone_level >= 1.0f)
	{
		KillSound( &g_dwAIMSndHandle );

		int volume = g_iSoundLevelCaution-10;

		if (volume > g_iSoundLevelCaution)
			volume = g_iSoundLevelCaution;

		if(AA_weapID==AIM_9L)
		{
			locksound = SND_AIM_9L_LOCK;
		}
		else if(AA_weapID==AIM_9M)
		{
			locksound = SND_AIM_9M_LOCK;
		}
		else
		{
			locksound = SND_AIM_9X_LOCK;
		}

		if (volume > 0 && !g_dwAIMLockSndHandle)
			g_dwAIMLockSndHandle = SndQueueSound( locksound, 0, volume );
//			g_dwAIMLockSndHandle = SndQueueSound( AA_weapID==AIM_9L ? Glock9L : Glock9M, 0, volume );
	}
	else  KillSound( &g_dwAIMLockSndHandle );
}

/*-----------------------------------------------------------------------------
 *
 *	DrawAIM_9X()
 *
 */
void DrawAIM_9X( PlaneParams *P )
{
	//  NOTE!!!!  THE AIM-9X is NOTHING, NADA, ZIPPO ... LIKE THE 9P
	//  Well, OK they are both heat seekers but if you put the 9X at one
	//  end of the effective heat seeker missile range (the good side)
	//  the 9P would be at the other end (the crap side) with the 9-M and
	//  L in the middle.

	//	AIM-9P has a 2° FOV circle depressed
	// 	46 mils below the gun cross and
	//	11 mils below FRL (Fuselage Reference Line)
	float tone_level;

	AA_irFOV = 6;		// 2° FOV  Changed by Scott
//	AA_slave = FALSE;	// 9Ps don't slave  Change by Scott
	AA_CAGE  = !AA_SCAN;

	if (AA_shootQ)  AA_shootQ = SM_SHOOT_CUE;	// triangle shoot cue

	HUDcircle(AA_PosX, AA_PosY, (HUD_DEGREE-1) * HUDResMul);  //  Copied from 9M Scott

	if (!AA_target)		// If no radar track...
	{					//  and seeker is caged...
		if (AA_CAGE)	//   draw 2° FOV circle.
			HUDcircle( WINDOW_0(0,5), HUD_DEGREE * HUDResMul );
	}
	else	// Radar track...
	{
		if (AA_breakX)  DrawBreakX();	// Warning: too close to fire.
		else
		{
			if (AA_range > AA_RANGER)	// If outside of range reticle...
			{							//  and seeker is caged...
				if (AA_CAGE)			//   draw the FOV circle w/aspect stick
				{
					HUDcircle( WINDOW_0(0,5), HUD_DEGREE *HUDResMul );
					//DrawHUDstick(  WINDOW_0(0,5), HUD_DEGREE, HUD_DEGREE, AA_aspect );
				}
			}
			//else						// Within range reticle
			 //	Draw_AIM9_Pipper();		//   so draw the pipper.
		}
	}

	tone_level = AIM9_IR_scan(P);
	Snd_AIM9_Tone( tone_level );
	Snd_AIM9_Lock( tone_level );		//  Moved here by Scott.
}

/*-----------------------------------------------------------------------------
 *
 *	DrawAIM_9M()
 *
 */
void DrawAIM_9M( PlaneParams *P )
{
	float tone_level;

	AA_irFOV = 4;		//	AIM-9L/M has a 4° FOV circle
	AA_CAGE  = !AA_SCAN;

	if (AA_shootQ)  AA_shootQ = SM_SHOOT_CUE;	// triangle shoot cue

	//	Always draw 2° seeker head position circle.
	//
	HUDcircle(AA_PosX, AA_PosY, (HUD_DEGREE-1) * HUDResMul);

	if (!AA_target || AA_AOT)		// If no radar track...
	{					//  and seeker is caged...
		if (IsWeapCaged())	//   draw 4° FOV circle .
			HUDcircle( WINDOW_0(0,5), 2*HUD_DEGREE  * HUDResMul);
	}
	else	// Radar track...
	{
		if (AA_breakX)  DrawBreakX();	// Warning: too close to fire.
		else
		{
			//if (AA_range < AA_RANGER)  Draw_AIM9_Pipper();

			//	Radar track so draw ASE circle
			//
			if (IsWeapCaged())		// 6° ASE circle for caged
			{
				DrawASEcircle( 3*HUD_DEGREE );
				//DrawHUDstick(  WINDOW_0(0,0), AA_ASEsize, HUD_DEGREE, AA_aspect );
			}
			else
			{
				DrawASEcircle( 3*HUD_DEGREE );  // used to be 6 degrees, now it is the same size.
			}

		}
	}

	tone_level = AIM9_IR_scan(P);
	Snd_AIM9_Tone( tone_level );
	Snd_AIM9_Lock( tone_level );
}

/*-----------------------------------------------------------------------------
 *
 *	DrawAIM_7()
 *
 */
void DrawAIM_7( PlaneParams *P )
{
	if (AA_shootQ)  AA_shootQ = SM_SHOOT_CUE;	// triangle shoot cue

	if (!AA_target)
	{
		//	If no radar track, draw the AIM-7 fixed 12° reference circle.
		//
		HUDcircleDash(WINDOW_0(0,0), 6*HUD_DEGREE );

		HUDtextLg(HUD_WIN_10(0,0),"FLOOD");

		if (UFC.EMISState || Av.AARadar.SilentOn)
		{
			HUDline( HUD_WIN_10(-5,-3), HUD_WIN_10(25,9) );
			HUDline( HUD_WIN_10(-5,9),  HUD_WIN_10(25,-3) );
			return;
		}

		if(WeapStores.FloodOn)
		{
			if( WeapStores.FloodTimer.IsActive() )
			{
				WeapStores.FloodTime -= WeapStores.FloodTimer.TimeElapsed(GameLoopInTicks);
				WeapStores.FloodTimer.Set(5.0,GameLoopInTicks);

				if(WeapStores.FloodTime <= 0)
					WeapStores.FloodTime = 0;

				int Min,Sec;
	  		Min = WeapStores.FloodTime/60.0;
    		Sec = ((int)WeapStores.FloodTime) % 60;

				if(Min > 59)
				{
					Min = 59;
					Sec = 59;
				}

				sprintf(TmpStr,"%02d:%02d SL",Min,Sec);
				HUDtextSm(HUD_TOF_WIN(0,0),TmpStr);

				if(WeapStores.FloodTime <= 0)
				  WeapStores.FloodTimer.Disable();
			}
		}
		else
		{
			WeapStores.FloodTimer.Disable();
		}

	}
	else	// Radar track...
	{
		if (AA_AOT)
		{

			if (UFC.EMISState || Av.AARadar.SilentOn)
			{
				HUDline( HUD_WIN_10(-5,-3), HUD_WIN_10(25,9) );
				HUDline( HUD_WIN_10(-5,9),  HUD_WIN_10(25,-3) );
				return;
			}
				WeapStores.FloodTimer.Disable();

			HUDcircleDash(WINDOW_0(0,0), 6*HUD_DEGREE );

	    	HUDtextLg(HUD_WIN_10(0,0),"HOJ");

			DisplayTOFWin(P);
			DisplayWindow1(P);

		}
		else
		{

			if (AA_breakX)
				DrawBreakX();	// Warning: too close to fire.
			else
			{

				if(AA_AOT)
				{
			    	HUDtextLg(HUD_WIN_10(0,0),"HOJ");
				}
				else if(WeapStores.Aim7LoftOn)
			    	HUDtextLg(HUD_WIN_10(0,0),"LOFT");

				//	Draw ASE circle and LAE dot.
				//
				DrawASEcircle( 3*HUD_DEGREE );

				if(WeapStores.FloodOn)
					HUDcircleDash( WINDOW_0(0,0), 6*HUD_DEGREE );

				//	Draw aspect stick if range > 12,000'
				//
				if (AA_range > AA_RANGER)
					DrawHUDstick(  WINDOW_0(0,0), AA_ASEsize, HUD_DEGREE, AA_aspect );

				DisplayTOFWin(P);
				DisplayWindow1(P);
			}


		}
	}


}

/*-----------------------------------------------------------------------------
 *
 *	DrawAIM_120()
 *
 */
void DrawAIM_120( PlaneParams *P )
{
	if (AA_shootQ)  AA_shootQ = SM_STAR6_CUE;	// 6-point star shoot cue

	if(WeapStores.Aim120VisualOn)
	  HUDtextLg(HUD_WIN_10(0,0),"VISUAL");

//	if (!AA_target || WeapStores.Aim120VisualOn || AA_AOT)
	if (!AA_target || WeapStores.Aim120VisualOn)
	{
		//	If no radar track, draw the AIM-120 fixed 12° reference circle.
		//
		HUDcircleDash( HUD_GACQ(0,-15), 7.5*HUD_DEGREE * HUDResMul);

		if(AA_target)
		{
			DisplayTOFWin(P);
			DisplayWindow1(P);
		}
	}
	else if (AA_AOT)
	{

		if (UFC.EMISState || Av.AARadar.SilentOn)
		{
			HUDline( HUD_WIN_10(-5,-3), HUD_WIN_10(25,9) );
			HUDline( HUD_WIN_10(-5,9),  HUD_WIN_10(25,-3) );
			return;
		}
			WeapStores.FloodTimer.Disable();

		HUDcircleDash( HUD_GACQ(0,-15), 7.5*HUD_DEGREE * HUDResMul);

	    HUDtextLg(HUD_WIN_10(0,0),"HOJ");

		DisplayTOFWin(P);
		DisplayWindow1(P);

	}
	else	// Radar track...
	{
		if (AA_breakX)  DrawBreakX();	// Warning: too close to fire.
		else
		{
			//	Draw ASE circle and LAE dot.
			//
			DrawASEcircle( 3*HUD_DEGREE );

			//	Draw aspect stick if range > 12,000'
			//
			if (AA_range > AA_RANGER)
				DrawHUDstick(  WINDOW_0(0,0), AA_ASEsize, HUD_DEGREE, AA_aspect );

			DisplayTOFWin(P);
			DisplayWindow1(P);
			//DisplayRangeWin(P);
			//DisplayVcWin(P);
		}
	}
}

/*-----------------------------------------------------------------------------
 *
 *	FixAIM9Rmin()
 *
 *		Weapons DB only has integer Rmin values.  Fix for Sidewinders.
 */
void FixAIM9Rmin()
{
	if(AA_weapID == Aim9X_Id)
		AA_Rmin = 0.2;
	else if( (AA_weapID == AIM_9L) || (AA_weapID == AIM_9M) )
		 AA_Rmin = 0.3;

		/*
	switch (AA_weapID)
	{
		case AIM_9P: AA_Rmin = 0.2;  break;
		case AIM_9L:
		case AIM_9M: AA_Rmin = 0.3;  break;
	}
		 */

}

//*****************************************************************************************************************************************
// GET AA WEAPON  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void GetAAWeapon( PlaneParams *P )
{
	// Don't update while Trigger or pickle
	if ( (P->Trigger2 || AA_MissileInFlight) && (Av.Weapons.UseAASeq) )  return;

	AA_weapID = AA_count = 0;

	int WeapIndex,WeapId;
	if(Av.Weapons.UseAASeq)
	{
			if(Av.Weapons.CurAASeqWeap == NULL)    return;
			if(!Av.Weapons.CurAASeqWeap->Selected) return;
			if(Av.Weapons.CurAASeqWeap->W->Count <= 0) return;

			WeapIndex = Av.Weapons.CurAASeqWeap->W->WeapIndex;
			WeapId    = Av.Weapons.CurAASeqWeap->W->WeapId;
	}
	else
	{
			if(Av.Weapons.CurAAWeap == NULL)    return;
			if(!Av.Weapons.CurAAWeap->Selected) return;
			if(Av.Weapons.CurAAWeap->W->Count <= 0) return;

			WeapIndex = Av.Weapons.CurAAWeap->W->WeapIndex;
			WeapId    = Av.Weapons.CurAAWeap->W->WeapId;
	}

	 // Get pertinent missile data
	//
	AA_weapID = WeapId;
	AA_weapDB = &pDBWeaponList[ WeapIndex ];
	AA_Rmin	  = AA_weapDB->fMinRange;
	AA_Rmax   = GetMissileRMaxNM( AA_weapDB, P->Altitude*FTTOWU );
//	AA_Rtr	  = AA_Rmax / 3;	// safety value
  	AA_Rtr	  = GetMissileRTR_NM( AA_target, AA_weapDB, P->Altitude*FTTOWU );
	AA_ROpt   = GetMissileROptNM( AA_weapDB, P->Altitude*FTTOWU );


	FixAIM9Rmin();	// weapons DB has integer Rmin !!!

	 //	Next, rescan for a full count of this missile type
	//
	for (int i=0; i<MAX_F18E_STATIONS; i++)
	{
		if (AA_weapID == P->WeapLoad[i].WeapId)
			AA_count  += P->WeapLoad[i].Count;
	}

	char *AA_type = AA_weapDB->sMPDAbbrev;
	char *AA_mode = (Av.Weapons.CurrWeapMode == AIM9_STAT) ? "SRM " : "MRM ";

	if (g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
		AA_mode = "AIM-";

	//	Finally, show missile type and count
	HUDtextLg( HUD_WIN_9(0,0), "%s %d",AA_type, AA_count );

	if(UFC.MasterArmState == OFF)
	{
	  HUDline( HUD_WIN_9(-5,-3), HUD_WIN_9(25,9) );
	  HUDline( HUD_WIN_9(-5,9),  HUD_WIN_9(25,-3) );
	}
}

/*-----------------------------------------------------------------------------
 *
 *	UpdateAAPickle()
 *
 *		Process new pickle and update TTI / TTA / TOF timers
 *
 */
void UpdateAAPickle( PlaneParams *P )
{
	if(PrimaryIsAOT(&CurFramePlanes))
	{
		return;
	}

	if (AA_weapID && AA_count && ( (Av.Weapons.CurrWeapMode == AIM7_STAT) || (Av.Weapons.CurrWeapMode == AIM120_STAT) ) && !AA_pickle && P->Trigger2)
	{
		 if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7M_Id) || (AA_weapID == Aim7MH_Id) )
		 {
				AA_Tpost = AA_Tpre;
		 }
		 else if(AA_weapID == Aim120_Id)
		 {
				AA_Tpost = AA_Tact;
				AA_TFromActToImpact = AA_Tpre - AA_Tact;
				AA_AimPhase = 1;
		 }

		 AA_MissileInFlight = TRUE;
	}
	else if (AA_MissileInFlight)		// missile in-flight
	{
		 if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7M_Id) || (AA_weapID == Aim7MH_Id) )
		 {
			 if( WeapStores.FloodOn && WeapStores.FloodTimer.IsActive() )
			 {
			   if(AA_Tpost < WeapStores.FloodTime)
						AA_Tpost = WeapStores.FloodTime;
			 }
		 }

		 if(AA_Tpost <= 0)
		 {
			  if(AA_weapID == Aim120_Id)
		 		{
		        if(!AA_AimPhase)
				      AA_MissileInFlight = FALSE;

			 		if(AA_AimPhase)
			 		{
						AA_Tpost = AA_TFromActToImpact;
						AA_AimPhase = 0;
			 		}
		 		}
				else
				{
					AA_MissileInFlight = FALSE;
					if(WeapStores.FloodOn)
						WeapStores.FloodOn = FALSE;
				}
		 }
	}

	AA_pickle = P->Trigger2;	// save state for next frame
}

/*-----------------------------------------------------------------------------
 *
 *	ResetAAGrowl()
 *
 *		Turn OFF Sidewinder tones.
 *
 */
void ResetAAGrowl()
{
	KillSound( &g_dwAIMSndHandle );
	KillSound( &g_dwAIMLockSndHandle );
}

/*-----------------------------------------------------------------------------
 *
 *	CheckAAGrowl()
 *
 *		Turn OFF Sidewinder tones when weapons change.
 *
 */
void CheckAAGrowl()
{
	if (AA_weapID != g_iHUDWeapID || AA_count != g_iHUDCount)
	{
		g_iHUDWeapID = AA_weapID;
		g_iHUDCount  = AA_count;

		AA_lockon = NULL;

		ResetAAGrowl();
	}
}

/*-----------------------------------------------------------------------------
 *
 *	UpdateAATarget()
 *
 *		Perform all A/A target calculations.
 *
 */

extern int CurrWeapMode;
BOOL PrimaryIsAOT(DetectedPlaneListType *Plist);

BOOL AATargetUpdated;

void UpdateAATarget( PlaneParams *P )
{
	if (!AATargetUpdated)
	{
		AATargetUpdated = TRUE;
		// jlm, temp until weapons.cpp can be checked out
		CurrWeapMode = Av.Weapons.CurrWeapMode;

		AA_SCAN = AAScanMode;
		AA_CAGE = TRUE;
		AA_THLD = TRUE;

		if (Av.AARadar.CurMode == AA_TWS_MODE)
			AA_AOT = PrimaryIsAOT(&PrevFramePlanes);
		else
			AA_AOT = PrimaryIsAOT(&CurFramePlanes);

		AA_target = P->AADesignate;
		AA_slave  = (AA_target != NULL);

		// for sparrows
  		if(WeapStores.FloodOn)
			AA_Tpre = 10.0;

		if (!AA_target)  return;

		float tActive, tImpact;

		AA_scale  = Av.AARadar.Range;

		float TargHeading = NormDegree(AA_target->Heading/DEGREE);
		AA_aspect = NormDegree( TargHeading + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));
		if(AA_aspect > 180.0)
			AA_aspect = -(360.0 - AA_aspect);
		AA_range  = PlayerPlane->WorldPosition / AA_target->WorldPosition;
    	AA_range *= WUTONM;


		tImpact = WGetTimeToAAImpact( P, AA_target, &tActive );
		AA_Tpre = round( tImpact );
		AA_Tact = round( tActive );
		AA_Rmar = AA_Tact ? AA_range * (tImpact - tActive) / tImpact : 0;

		AA_Rtr	  = GetMissileRTR_NM( AA_target, AA_weapDB, P->Altitude*FTTOWU );
		AA_shootQ = AA_weapID && (AA_range >= AA_Rmin) && (AA_range <= (AA_ROpt) );
		AA_breakX = AA_range < AA_Rmin;

		AA_altitude = (int)AA_target->Altitude;
	}
}

#define AG_RANGER	(12000*FTTOWU)	// range reticle max (WU)
/*-----------------------------------------------------------------------------
 *
 *	DisplayPipper()
 *
 *		Display the pipper and the reticle range bar.
 *
 */
void DisplayPipper( int PosX, int PosY, float range )
{
	CRTicon( PosX, PosY, SM_PIPPER );

	if (UFC.EMISState == ON)  return;	// no range if no radar

	int rads = round( 80 * range / AG_RANGER );	// convert to [0..80]

	if (rads > 80) rads = 80;

	CRTiconCmp( PosX, PosY, SM_PIPPER_TELLTALE, rads );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayGDS()
 *
 */
void DisplayGDS( PlaneParams *P )
{
	int		PosX, PosY;
	FPoint	tVector;
	float   Distance;

	if (!AA_target) return;

	Distance = AA_target->WorldPosition - P->WorldPosition;

	tVector = GetBallisticWUGivenDistance( P, Distance );

	tVector -= P->WorldPosition;
	tVector *= ViewMatrix;
	tVector.Perspect( &PosX, &PosY );

	DisplayPipper( PosX, PosY, Distance );
}

//*****************************************************************************************************************************************
// SET TD INFO  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SetHudTDInfo(FPointDouble Loc,int Type)
{
 // Type 0 = Primary Scan
 // Type 1 = Secondary Scan

 if(Type == 0)
	AA_PrimaryPosition = Loc;
 else if(Type == 1)
	AA_SecondaryPosition = Loc;
}

//*****************************************************************************************************************************************
// DISPLAY AA HUD  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayAAHUD( PlaneParams *P )
{
	 GetAAWeapon(P);

	 // reset symbology if different weap type
	 int CurWeapMode = GetModeForAAWeap(AA_weapID);
	 if(CurWeapMode != PrevAAWeapMode)
	 {
		 if(CurWeapMode != NO_WEAP_STAT)
		 {
		 AA_reset();
		 }
 		 PrevAAWeapMode = CurWeapMode;
	 }

	 UpdateAAPickle(P);
	  UpdateAATarget(P);

	 DisplayAAradar(P);

	 AA_DrawASE = FALSE;

	if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7MH_Id) || (AA_weapID == Aim7M_Id) )
		DrawAIM_7( P );
	else if(AA_weapID == Aim120_Id)
		DrawAIM_120( P );
	if( (AA_weapID == Aim9L_Id) || (AA_weapID == Aim9M_Id) )
		DrawAIM_9M( P );
	else if(AA_weapID == Aim9X_Id)
		DrawAIM_9M( P );
	else
	{
		if(WeapStores.FloodOn)
		{
			if( WeapStores.FloodTimer.IsActive() )
			{
				HUDtextLg(HUD_WIN_10(0,0),"FLOOD");

				WeapStores.FloodTime -= WeapStores.FloodTimer.TimeElapsed(GameLoopInTicks);
				WeapStores.FloodTimer.Set(5.0,GameLoopInTicks);

				if(WeapStores.FloodTime <= 0)
					WeapStores.FloodTime = 0;

				int Min,Sec;
				Min = WeapStores.FloodTime/60.0;
				Sec = ((int)WeapStores.FloodTime) % 60;

				if(Min > 59)
				{
					Min = 59;
					Sec = 59;
				}

				sprintf(TmpStr,"%02d:%02d SL",Min,Sec);
				HUDtextSm(HUD_TOF_WIN(0,0),TmpStr);

				if((WeapStores.FloodTime <= 0) || (AA_target))
				{
					WeapStores.FloodTimer.Disable();
					AA_Tpost = -1.0f;
				}
			}
		}
		else
		{
			WeapStores.FloodTimer.Disable();
			if((AA_MissileInFlight) && (AA_Tpost >= 0))
			{
				 if(AA_AimPhase)
				    HUDtextSm( HUD_TOF_WIN(0,0), "%2d ACT", (int)AA_Tpost );
				 else
				    HUDtextSm( HUD_TOF_WIN(0,0), "%2d TTG", (int)AA_Tpost );
			}
		}

	}


	 /*
	 else if( (AA_weapID == Aim7F_Id) || (AA_weapID == Aim7MH_Id) || (AA_weapID == Aim7M_Id) )
         DrawAIM_7( P );
	 else if(AA_weapID == Aim120_Id)
         DrawAIM_120( P );

	 */

	/*
	GetAAWeapon(P);		// get/show weapon type and count
	CheckAAGrowl();		// reset sidewinder tones at weapon change
	UpdateAAPickle(P);	// update TOF timers at pickle
	UpdateAATarget(P);	// perform CC target calculations

	if (CurrWeapMode == GUN_MODE)
	{
		switch (UFC.AASubMode)
		{
			case AA_GUN_FNL:

				HUDtextLg( WINDOW_17(30,0), "FNL" );
				HUDtextLg( WINDOW_5(0,0), "%d", P->WeapLoad[GUNS_STATION].Count );
				DisplayFunnel(P);
				break;

			case AA_GUN_GDS:

				HUDtextLg( WINDOW_17(30,0), "GDS");
				HUDtextLg( WINDOW_5(0,0), "%d", P->WeapLoad[GUNS_STATION].Count );
				DisplayGDS(P);
				break;

			case AA_GUN_FNL_GDS:

				HUDtextLg( WINDOW_17(30-20,0), "GDS FNL" );
				HUDtextLg( WINDOW_5(0,0), "%d", P->WeapLoad[GUNS_STATION].Count );
				DisplayFunnel(P);
				DisplayGDS(P);
				break;
		}
	}
	else	// SRM_MODE or MRM_MODE  (casual mode always = WEAP_OFF)
	{
		switch (AA_weapID)
		{
			case AIM_9L:
			case AIM_9M:	DrawAIM_9M( P );	break;
			case AIM_9P:	DrawAIM_9P( P );	break;
			case AIM_7F:
			case AIM_7M:	DrawAIM_7( P );		break;
			case AIM_120A:	DrawAIM_120( P );	break;
		}
	}

	// Draw the radar info here since AIM-XX() updates variables
	//
	DisplaySniff();
	DisplayAAradar(P);
	DisplayBreakLock(P);
	DisplayAASearchModeSymbology(P);
	 *
	 * */
}

//*****************************************************************************
//=============================================================================
//		A I R - t o - G R O U N D    F U N C T I O N S
//=============================================================================
//*****************************************************************************

DBWeaponType *AG_weapDB; // weapon DB entry

int   AG_weapOn;	// weapon selected ?
int   AG_weapIx;	// weapon selected ?
int   AG_guided;	// GUIDED mode (T/F) 0=none, 1=mav, 2=gbu
int   AG_pickle;	// pickle (weapon in flight)
int   AG_PosX;		// target CRT position
int   AG_PosY;
float AG_Trel;
float AG_timer;		// bomb release timer
float AG_tImpact;
FPointDouble AG_ImpactPoint;

/*-----------------------------------------------------------------------------
 *
 *	AG_init()
 *
 *		Initialize ALL A/G global variables.
 */
void AG_init()
{
	AG_PosX   = 0;
	AG_PosY   = 0;
	AG_timer  = 0;
	AG_guided = 0;
	AG_pickle = 0;
	AG_weapOn = 0;
	AG_tImpact = 0;
	AG_weapDB = NULL;
}

/*-----------------------------------------------------------------------------
 *
 *	AGGuided()
 *
 */
int AGGuided( PlaneParams *P )
{
	int WeapPage = -1;

	if(Av.Weapons.CurAGWeap)
	{
	 int WeapPage = GetWeapPageForWeapId(Av.Weapons.CurAGWeap->W->WeapId,Av.Weapons.CurAGWeap->Category);
	}

	return( (WeapPage == AGM_65_WEAP_PAGE) || (WeapPage == AGM_65F_WEAP_PAGE) || (WeapPage == AGM_65E_WEAP_PAGE) || (WeapPage == WALLEYE_WEAP_PAGE) );
}

/*-----------------------------------------------------------------------------
 *
 *	AGGetWeapon()
 *
 *		Rather cumbersome function since there is no CC.
 *
 *		Problem:  At pickle of last bomb at selected station,
 *				  the station is cleared causing
 *				  IsAGWeaponSelected() to return false.
 *				  Then we don't get to process the pickle.
 *
 *		Solution: Don't update our weapon info if Trigger2.
 *				  Continue to use values from last frame.
 *				  Also continue GUIDED while bomb in flight.
 *
 */
void AGGetWeapon( PlaneParams *P )
{
	// Don't update while Trigger or pickle
	if (P->Trigger2 || AG_pickle)  return;

	AG_weapOn = AG_guided = 0;

	AG_weapOn = GetSelectedAGWeaponId();
	if(AG_weapOn == -1) AG_weapOn = 0;
	else
	{
	  AG_weapIx = Av.Weapons.CurAGWeap->W->WeapIndex;
	  AG_weapDB = &pDBWeaponList[AG_weapIx];
	  AG_guided = AGGuided(P);
	}
}

/*----------------------------------------------------------------------------
 *
 *	GetAGImpactTime()
 *
 */
float GetAGImpactTime( PlaneParams *P, double orgVertVel, FPoint *ImpactPoint )
{
	double VertVel;
	float  gravity = 32.0f;
	double dheight = P->HeightAboveGround;
 	float  weapondrag = 1.0;
	double orgdheight;

	if (P->AGDesignate.X != -1)
		dheight = P->WorldPosition.Y - P->AGDesignate.Y;


	orgdheight = dheight;

 	if (AG_weapDB)
	{
		weapondrag = AG_weapDB->fDrag;

		if(P == PlayerPlane)
		{
			BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

			if(B->Drag != WEAP_STORES_RETARDED_MODE)
			{
				if(WIsRetardedWeapon(AG_weapDB->lWeaponID))
				{
					weapondrag = 0.97;
				}
			}
		}
		if(AG_weapDB->lWeaponID == 132)
		{

			if(dheight > (350.0f * FTTOWU))
			{
				dheight -= (350.0f * FTTOWU);
			}
		}
	}

	if (weapondrag > 1.0)
	{
		gravity /= weapondrag;
		weapondrag = 1.0;
	}

	VertVel = orgVertVel * weapondrag;

	float tImpact = (float)(1.0f/gravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*gravity)*(dheight * WUTOFT))));

	if (ImpactPoint)
	{
		(*ImpactPoint).SetValues( (float)(P->WorldPosition.X + (P->IfVelocity.X * weapondrag * tImpact)*FTTOWU),
								  (float)(P->WorldPosition.Y-orgdheight),
								  (float)(P->WorldPosition.Z + (P->IfVelocity.Z * weapondrag * tImpact)*FTTOWU));
	}

	return tImpact;
}

/*-----------------------------------------------------------------------------
 *
 *	SetUFCAGTrel()
 *
 */
void SetUFCAGTrel( PlaneParams *P, int loft )
{
	// If AUTO, use vertical component of velocity
	// If LOFT, use 30° pitch for vertical velocity
	//
	double upSpeed = loft ? P->V*0.5 : P->IfVelocity.Y;

	// Compute TREL - Time to Release
	FPoint	ImpactPoint;
	GetAGImpactTime( P, upSpeed, &ImpactPoint );

	UFC.TRelCountDown = ((ImpactPoint - P->AGDesignate) * WUTOFT) / P->IfHorzVelocity;

	float d1 = P->WorldPosition - ImpactPoint;
	float d2 = P->WorldPosition - P->AGDesignate;

	if (d1 < d2)  UFC.TRelCountDown++;	// keep Trel>0 till past target,
	else		  UFC.TRelCountDown=0;	// then keep at 0
}

/*-----------------------------------------------------------------------------
 *
 *	ShowCountDown()
 *
 *		Show TTGT / TPULL / TREL for AUTO modes if Target_Steering.
 *
 */
void ShowCountDown( char *type, int time )
{
	if (UFC.SteeringMode != TGT_STR)  return;

	if (time < 0)
		HUDtextLg( HUD_WIN_6(-6,0), " -:-- %s", type );
	else
	{
		if(time > 99)
		  HUDtextLg( HUD_WIN_6(-6,0), "99 %s", type );
		else
		  HUDtextLg( HUD_WIN_6(-6,0), "%d %s", time, type );
	}

	ShowETE = FALSE;	// never show default UFC ETE
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayFragCue()
 *
 */
void DisplayFragCue( PlaneParams *P, FPoint target )
{
	FPoint 	tVector;
	int		PosX, PosY;

	// Find 2D point 2000' above target.
	//
	tVector    = target;

	if(AG_weapDB)
	{
		if(AG_weapDB->iDamageRadius <= 1)
			return;

		if(AG_weapDB->iWeaponType == WEAPON_TYPE_CLUSTER_BOMB)
		{
			tVector.Y += (Av.Weapons.Bomb.CbuDetonateAltIndex*FTTOWU) + (200.0f * FTTOWU);
		}
		else if(AG_weapDB->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
		{
			tVector.Y += 1200.0f * FTTOWU;
		}
		else
		{
			tVector.Y += AG_weapDB->iDamageRadius * FTTOWU * 2.0f;
		}
	}
	else
	{
		tVector.Y += 2000 * FTTOWU;
	}

	tVector   -= P->WorldPosition;
	tVector   *= ViewMatrix;
	tVector.Perspect( &PosX, & PosY );

	int x1 = round(  5 * HUDcos );	// (x1,y1) is inside point
	int y1 = round(  5 * HUDsin );
	int x2 = round( 16 * HUDcos );	// (x2,y2) is outside point
	int y2 = round( 16 * HUDsin );
	int x3 = round(  4 * HUDcos );	// (x3,y3) is up-tip point
	int y3 = round(  4 * HUDsin );

	CRTline( PosX+x1, PosY-y1, PosX+x2, PosY-y2 );	// right base line
	CRTline( PosX-x1, PosY+y1, PosX-x2, PosY+y2 );	// left  base line

	CRTline( PosX+x2, PosY-y2, PosX+x2-y3, PosY-y2-x3 );	// right up-tip
	CRTline( PosX-x2, PosY+y2, PosX-x2-y3, PosY+y2-x3 );	// left  up-tip
}

/*-----------------------------------------------------------------------------
 *
 *	AGpipper()
 *
 */
void AGpipper( PlaneParams *P )
{
	// Pipper w/ Reticle range bar
	//
	DisplayPipper( VelVectorX, VelVectorY, UFC.DataCurrTGTNavDist );

	// FragCue - bomb fragment safe altitude bar
	//
	DisplayFragCue( P, P->AGDesignate );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayAGWeapons()
 *
 */
void DisplayAGWeapons( PlaneParams *P )
{
	if (AG_weapOn && (g_Settings.gp.nAGRADAR == GP_TYPE_CASUAL) )
		HUDtextLg( WINDOW_5(0,0), " %s", AG_weapDB->sMPDAbbrev );
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayAGDesignate()
 *
 *		Prepare for A/G mode and perform all common target functions.
 *
 */
void DisplayAGDesignate( PlaneParams *P, int DrawASL=1 )
{

	if (P->AGDesignate.X == -1)
		return;

	int Draw;
	int cycle = GetTickCount() - Targ_mark;
	Draw = (cycle < 250) ? FALSE : TRUE;
	if (cycle > 500)  Targ_mark += cycle;

	// All A/G modes w/target show TD diamond
	//
	FPoint  tVector;

	tVector  = P->AGDesignate;
	tVector -= P->WorldPosition;
	tVector *= ViewMatrix;

	if (!tVector.Perspect( &AG_PosX, &AG_PosY ))  return;	// behind me

	int InHud = CheckDesignateInHUD(AG_PosX,AG_PosY);

	if(InHud)
	  CRTicon( AG_PosX, AG_PosY, SM_DESIGNATE );	// TD diamond
	else
	{

		int x, y;
		if( AG_PosX<= HudLeft )
			x = HudLeft ;
		else if( AG_PosX>= HudRight)
			x = HudRight;
		else
			x = AG_PosX;

		if( AG_PosY<= HudTop )
			y= HudTop ;
		else if( AG_PosY>= HudBottom )
			y = HudBottom ;
		else
			y = AG_PosY;

		if(Draw)
			CRTicon( x, y, SM_DESIGNATE );	// TD diamond

			//CRTicon( AG_PosX, AG_PosY, SM_DESIGNATE );	// TD diamond
	}

//	if (AG_guided)  return;	// no ASL for guided bombs
	if (!DrawASL)  return;	// no ASL for guided bombs

	int x1 = AG_PosX - round(3.6 * HUDsin);	// edge of TD box
	int y1 = AG_PosY - round(3.6 * HUDcos);

	int x2 = AG_PosX - 999 * HUDsin;	// long line normal to ground
	int y2 = AG_PosY - 999 * HUDcos; // 72 = HUD_DEGREE * 8 DEGREES  (9*8)

	CRTline( x1,y1, x2,y2 );

	x1 = AG_PosX + round(3.6 * HUDsin);	// edge of TD box
	y1 = AG_PosY + round(3.6 * HUDcos);

	x2 = AG_PosX + 999 * HUDsin;	// long line normal to ground
	y2 = AG_PosY + 999 * HUDcos; // 72 = HUD_DEGREE * 8 DEGREES  (9*8)

	CRTline( x1,y1, x2,y2 );
}

/*-----------------------------------------------------------------------------
 *
 *	AGDesignateInCone()
 *
 */
int AGDesignateInCone( PlaneParams *P )
{
	#define HALF_CONE_ANGLE 20.0

	float Angle = UFC.DataCurrTGTRelBrg;

	return (fabs(Angle) < HALF_CONE_ANGLE);
}

/*-----------------------------------------------------------------------------
 *
 *	DesignateAGPoint()
 *
 */
void DesignateAGPoint(PlaneParams *P, int Sx, int Sy)
{
	FPointDouble ProjPoint;

	if (P->WorldPosition.ProjectRayToHeight( ProjPoint, Sx, Sy, P->HeightAboveGround )
	&& (P->WorldPosition - ProjPoint < 40*NMTOWU) )
	{
		FPointDouble Vec;

		Vec = ProjPoint;

		Vec -= PlayerPlane->WorldPosition;

		Vec.Normalize();
		Vec *= 40.0*NMTOWU;

		Vec += PlayerPlane->WorldPosition;

		ProjPoint = Vec;


		// we back projected a valid point, now do line-of-sight to get occlusions
		FPointDouble New;
		// switch res based on distance
		if (!LOS(&P->WorldPosition, &ProjPoint, &New, HI_RES_LOS, TRUE))
		{
			P->AGDesignate = New;
      P->AGDesignator = AG_DESIGNATE_FROM_HUD;
		}
	}
	else
	{
		// for some reason we could not get a valid point, so set it to something that
		// says we were unable to get a designation
		//P->AGDesignate.SetValues( -1.0, -1.0, -1.0 );
	}
}

//*****************************************************************************************************************************************
// DO AGCDIP  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DoAGCDIPManMode( PlaneParams *P )
{
	static int inPickle=0;		// were we in pickle last frame ?
	int nuPickle = P->Trigger2;	// current pickle state

	HUDtextLg( WINDOW_20(0,0), "CCIP" );

	if (!AG_weapOn && (!inPickle || !nuPickle))
	{
		DrawPipper(VelVectorX, VelVectorY);
		inPickle = 0;
		return;
	}

	FPoint 	ImpactPoint;
	float iTime = GetAGImpactTime( P, P->IfVelocity.Y, &ImpactPoint );

	AG_ImpactPoint = ImpactPoint;

	if (nuPickle)	// Force AUTO mode while pickle
	{
		if (!inPickle)  P->AGDesignate = ImpactPoint;	// new pickle

		//ShowCountDown( "TREL", 0 );

		//AGpipper(P);
	}
	else	// Draw reticle pipper @ impact point w/ Displayed Impact Line (DIL)
	{
		int		PosX, PosY;
		FPoint  tVector;

		tVector  = ImpactPoint;
		tVector -= P->WorldPosition;
		tVector *= ViewMatrix;

		tVector.Perspect( &PosX, &PosY );

		int x1 = VelVectorX + round( 3 * HUDsin );	// edge of VV
		int y1 = VelVectorY + round( 3 * HUDcos );

		int x2 = PosX - round( 12 * HUDsin );	// edge of pipper
		int y2 = PosY - round( 12 * HUDcos );

		int TempX1,TempY1,TempX2,TempY2;
		TempX1 = x1; TempX2 = x2; TempY1 = y1; TempY2 = y2;

		CRTline( x1,y1, x2,y2 );	// DIL

		if( GrClipLine(GrBuffFor3D,&TempX1,&TempY1,&TempX2,&TempY2) )
		{
		  CRTline( x1,y1, x2,y2 );	// DIL
		  //DisplayPipper( PosX, PosY, tVector.Length() );	// reticle pipper
	      DrawPipper(PosX, PosY);
		}

		DisplayFragCue( P, ImpactPoint );
	}

	inPickle = nuPickle;
}


/*-----------------------------------------------------------------------------
 *
 *	DoAGAutoMode()
 *
 */
void DoAGAutoMode( PlaneParams *P )
{
	HUDtextLg( WINDOW_20(0,-4), "AUTO" );

	if (!AG_weapOn || P->AGDesignate.X == -1)
	{
		CRTicon( VelVectorX, VelVectorY, SM_PIPPER );
		return;
	}

	// Must have designated target AND weapon selected !!!
	//
	AGpipper(P);

	if (AGDesignateInCone(P))
	{
		SetUFCAGTrel(P,0);
		ShowCountDown( "REL", UFC.TRelCountDown );
		AIC_WSO_Bomb_Speech( P, WSO_BOMB_TREL, UFC.TRelCountDown );
		TRelSet = TRUE;
	}
	else
	{
		// ShowCountDown( "TTGT", UFC.DataCurrTGTDistTime );
		lBombTimer = 161;
	}

	//	Draw release cue based on TREL time referenced to velocity vector
	//	At 10 seconds TREL, release cue is 5° above VV
	//
	if ((UFC.TRelCountDown <= 10) && P->Trigger2)  // don't countdown w/o consent
	{
		int  ReleaseCue = VelVectorY - UFC.TRelCountDown * HUD_DEGREE / 2;

		CRTline( VelVectorX-16, ReleaseCue, VelVectorX+16, ReleaseCue );
		CRTline( VelVectorX- 3, VelVectorY, VelVectorX+ 3, VelVectorY );
	}
}



int   TimeToReach30   = -1;
float PitchAtPullUp;
float dPitch;
int   PullUpTimer     = -1;
int   InTRelPhase     = FALSE;
FPointDouble AGAtPullUp;
//*-----------------------------------------------------------------------------

void DrawESLAtGivenPitch(PlaneParams *P,int Pitch)
{
  #define OFS	5	// ladders above/below center

  float TempPitch = Pitch;

  float StartPitch = NormDegree(P->Pitch/DEGREE - 25.0);
  float StopPitch  = NormDegree(P->Pitch/DEGREE + 25.0);

  float Adjust = 360.0 - StartPitch;
  StartPitch   = 0;
  StopPitch    = NormDegree(StopPitch + Adjust);
  Pitch        = NormDegree(Pitch + Adjust);

  if( (Pitch >= StartPitch) && (Pitch <= StopPitch) ) // 6 is the index for the 30 degree pitch mark
  {
	int   Index  = (int)floor(TempPitch/5.0);
	float Frac   = (TempPitch/5.0) - (floor(TempPitch/5.0));
	int   Index2 = (Index >= 71) ? 0 : Index+1;
	float Pos1   = UFC.HUDLadder[Index].HUDPoints[0].iSY;
	float Pos2   = UFC.HUDLadder[Index2].HUDPoints[0].iSY;

	int   yPos   = Pos1 + ((Pos2 - Pos1)*Frac);

	HUDline( WINDOW_0(-75,yPos-116), WINDOW_0(75,yPos-116) );
  }
}

/*-----------------------------------------------------------------------------
 *
 *	DoAGAutoLoftMode()
 *
 */
void DoAGAutoLoftMode( PlaneParams *P )
{
	HUDtextLg( WINDOW_20(0,0), "AUTO LOFT" );

	if (!AG_weapOn || P->AGDesignate.X == -1)
	{
		CRTicon( VelVectorX, VelVectorY, SM_PIPPER );
		return;
	}

	// Must have designated target AND weapon selected !!!
	//
	AGpipper(P);

	if (AGDesignateInCone(P))
	{
		SetUFCAGTrel(P,1);

		int tPull = UFC.TRelCountDown - 5;

		if( (tPull > 0) && (!InTRelPhase) )	// *** TPULL phase ***
		{
			ShowCountDown( "TPULL", tPull );
			AIC_WSO_Bomb_Speech( P, WSO_BOMB_TPULL, tPull );
            PullUpTimer = -1;
            AGAtPullUp = P->AGDesignate;
		}
		else			// *** TREL phase ***
		{
			SetUFCAGTrel(P,0);
			ShowCountDown( "REL", UFC.TRelCountDown );
			AIC_WSO_Bomb_Speech( P, WSO_BOMB_TREL, UFC.TRelCountDown );
			PullUpTimer = 5000;
			InTRelPhase = TRUE;

			if( (UFC.TRelCountDown <= 0) || ((P->AGDesignate.X != AGAtPullUp.X) || (P->AGDesignate.Z != AGAtPullUp.Z)) )
		    {
              TimeToReach30 = -1;
		      InTRelPhase   = FALSE;
		      return;
		    }
		}

		// Draw (ESL) elevation steering line
		//
		if( (tPull <= 5) || (PullUpTimer != -1) )
		{
			if( (TimeToReach30 == -1) && (PullUpTimer != -1) )
			{
			  TimeToReach30  = GameLoopInTicks + PullUpTimer;
			  PitchAtPullUp  = P->Pitch/DEGREE;         // in degrees 0..360
              if(PitchAtPullUp >= 180.0)
                 dPitch = (360.0 - PitchAtPullUp) + 30.0;
			  else
                 dPitch = 30 - (P->Pitch/DEGREE);
			}

			if(PullUpTimer == -1)
			{
               HUDline(RESD_NUM_X(HUD_MIDX - 75),VelVectorY,RESD_NUM_X(HUD_MIDX + 75),VelVectorY);
			}
            else
			{
              // ESL line marches up/down to 30 degree within 1 sec
			  float Frac;
			  if((int)GameLoopInTicks < TimeToReach30)
                 Frac = 1.0 - ((TimeToReach30 - GameLoopInTicks)/(float)PullUpTimer); //0.0 to 1.0
			  else
			     Frac = 1.0;
			  int   CurESLPitch = NormDegree(PitchAtPullUp + (dPitch*Frac));

			  DrawESLAtGivenPitch(P,CurESLPitch);
			}
		}
	}
	else
	{
//		ShowCountDown( "TTGT", UFC.DataCurrTGTDistTime );
		lBombTimer = 161;
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DoAGAutoLoftMode()
 *
 */

 /*
void DoAGAutoLoftMode( PlaneParams *P )
{
	HUDtextLg( WINDOW_20(0,0), "AUTO LOFT" );

	if (!AG_weapOn || P->AGDesignate.X == -1)
	{
		CRTicon( VelVectorX, VelVectorY, SM_PIPPER );
		return;
	}

	// Must have designated target AND weapon selected !!!
	//
	AGpipper(P);

	if (AGDesignateInCone(P))
	{
		SetUFCAGTrel(P,1);

		int tPull = UFC.TRelCountDown - 5;

		if (tPull > 0)	// *** TPULL phase ***
		{
			ShowCountDown( "TPULL", tPull );
			AIC_WSO_Bomb_Speech( P, WSO_BOMB_TPULL, tPull );
		}
		else			// *** TREL phase ***
		{
			SetUFCAGTrel(P,0);
			ShowCountDown( "TREL", UFC.TRelCountDown );
			AIC_WSO_Bomb_Speech( P, WSO_BOMB_TREL, UFC.TRelCountDown );
		}

		// Draw (ESL) elevation steering line
		//
		if (UFC.TRelCountDown <= 10)
		{
			int yPos = (UFC.TRelCountDown > 5) ? 0 : -40;

			HUDline( WINDOW_0(-75,yPos), WINDOW_0(75,yPos) );
		}
	}
	else
	{
		ShowCountDown( "TTGT", UFC.DataCurrTGTDistTime );
		lBombTimer = 161;
	}
}

  */

/*-----------------------------------------------------------------------------
 *
 *	DoAGGuideMode()
 *
 */
void DoAGGuideMode( PlaneParams *P )
{
	static float AG_range;	// save range @ pickle

	HUDtextLg( WINDOW_20(0,0), "GUIDED" );

	HUDicon( WINDOW_0(0,4), SM_PIPPER );

	FPointDouble target;
	GetMavGbuTargetPos( AG_guided-1, &target );

	int nuPickle = !AG_pickle && P->Trigger2;	// leading edge

	AG_pickle |= nuPickle;	// set the latch

	if (!AG_pickle)
	{
		if (target.X == -1)  return;

		AG_range = P->WorldPosition - target;

		float AG_Rmin;	// (wu)
		float AG_Rmax;	// (wu)
		float AG_Ropt;	// (wu)

		WGetGBUMavMinMax( P, AG_weapDB, &AG_Rmin, &AG_Rmax, &AG_Ropt );

		AG_Rmin *= FTTOWU;
		AG_Rmax *= FTTOWU;
		AG_Ropt *= FTTOWU;

	 	if (g_Settings.gp.nAGRADAR == GP_TYPE_CASUAL
		&&	UFC.SteeringMode == TGT_STR
		&&	AG_range < AG_Rmax
		&&	AG_range > AG_Rmin)
			HUDtextLg( WINDOW_17(0,0), "TGT IN RANGE" );

		if (AG_guided==1)	// Maverick
			AG_Ropt = GetMissileROptNM( AG_weapDB, P->Altitude*FTTOWU ) * NMTOWU;

		int tRel = (AG_range - AG_Ropt) * WUTOFT / P->IfHorzVelocity;
		if (tRel < 0)  tRel = 0;

		UFC.TRelCountDown = tRel;
		ShowCountDown( "REL", UFC.TRelCountDown );
	}
	else
	{
		if (nuPickle)	// leading edge
		{
			if (AG_guided==1)	// Maverick
				WGetAGLaunchAvgSpeed( AG_weapIx, AG_range*WUTOFT, P->V, P->Altitude, &AG_tImpact );
			else
				AG_tImpact  = GetAGImpactTime( P, P->IfVelocity.Y, NULL );
		}

		ShowCountDown( "TIMPCT", round( AG_tImpact ) );

		if (AG_tImpact < -3)	// Show -:-- for 3 seconds, then ...
			AG_pickle  =  0;	//	reset the latch
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DoAGGunsMode()
 *
 */
void DoAGGunsMode( PlaneParams *P )
{
	int		PosX, PosY;
	FPoint	Vel;
	FPoint	tVector;

	float BulletVelocity = 3380.0;	// this will ultimately be pulled from weapons structure

	#define SIN_GUN_SLOPE 0.0262	// 1.5°
	#define COS_GUN_SLOPE 0.9997

	HUDtextLg( WINDOW_20(0,0), "GUN" );
	HUDtextLg( WINDOW_5 (-3,0), "%03d", P->WeapLoad[GUNS_STATION].Count );

	Vel.X = (P->Orientation.I.X*COS_GUN_SLOPE - P->Orientation.K.X*SIN_GUN_SLOPE)*BulletVelocity;
	Vel.Y = (P->Orientation.I.Y*COS_GUN_SLOPE - P->Orientation.K.Y*SIN_GUN_SLOPE)*BulletVelocity;
	Vel.Z = (P->Orientation.I.Z*COS_GUN_SLOPE - P->Orientation.K.Z*SIN_GUN_SLOPE)*BulletVelocity;

	double VertVel = Vel.Y;
	float  Time = (float)(1.0f/32.0f)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*32.0f)*(P->HeightAboveGround * WUTOFT))));

	tVector.SetValues( Vel.X*Time*FTTOWU, -P->HeightAboveGround, Vel.Z*Time*FTTOWU );
	tVector *= ViewMatrix;
	tVector.Perspect( &PosX, &PosY );

	DisplayPipper( PosX, PosY, tVector.Length() );
}

//*****************************************************************************************************************************************
// AA GUNS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayAAGuns(PlaneParams *P)
{
   WeapStores.AGunCanFire = FALSE;

	 UpdateAATarget(P);

	 int AARadarMode = GetAARadarMode();

	 AA_target = P->AADesignate;


	 DisplayGunCross();

	 int Fx,Fy;
	 DisplayFunnel( ((AARadarMode != AA_STT_MODE) || (WeapStores.FunnelOn)), P, &Fx, &Fy);

	 if(AARadarMode == AA_STT_MODE)
	 {

		if( !P->AADesignate)
		{
			Av.AARadar.CurMode = AA_ACQ_MODE;/// This "isn't suppose to happen" wtf
			return ;
		 }

			float Range = (PlayerPlane->WorldPosition / PlayerPlane->AADesignate->WorldPosition)*WUTOFT;
			float Frac  = Range/23000.0;
			if(Frac > 1.0) Frac = 1.0;


	 		// calc shoot cue
	 		int PosSet,X,Y;
	 		DrawTDbox(P,FALSE,&PosSet,&X,&Y);

			// kludge...I need to call draw td box again because I need its X Y to figure out the shoot cue....
			float Dx = Fx - X;
			float Dy = Fy - Y;
			float Dist = sqrt(Dx*Dx + Dy*Dy);
			WeapStores.AGunCanFire  = ( (Dist < 8) && Range < (6000.0) );

	 		DrawTDbox(P,WeapStores.AGunCanFire,&PosSet,&X,&Y);

			//TW 10/10 Does this circle need to be drawn out of range?
			float scale = 1.0f/255.0f;
			float endangle = NormDegree(270.0 + (Frac*360));
			if( endangle == 270.0f)
				endangle = 269.8f;
			GrDrawPolyCircleArc( GrBuffFor3D, Fx-1, Fy-1, 12*HUDResMul, 13*HUDResMul, 270, endangle, 30,HUDColorRGB->Red* scale,HUDColorRGB->Green* scale,HUDColorRGB->Blue * scale,0);

	 		DrawPipper(Fx,Fy);

			//Frac  = 0.26; // 0.26 = 6000 ft / 23000 ft
			//DrawNIRDTick(Fx,Fy,13,266.0,FALSE);  // 266.0 = 360.0 - Frac*360.0
			DrawNIRDTickScreen( Fx, Fy, 13, 187.0, FALSE );  // 266.0 = 360.0 - Frac*360.0


			int NineXOnBoard = IsWeaponOnBoard(AIM9X_ID);
			int NineLOnBoard = IsWeaponOnBoard(AIM9L_ID);
			int NineMOnBoard = IsWeaponOnBoard(AIM9M_ID);

			if(NineXOnBoard || NineLOnBoard || NineMOnBoard)
			{
			   Frac  = (0.3*NMTOFT)/23000.0;
				 if(Frac > 1) Frac = 1.0;
					//DrawNIRDTriangle(Fx,Fy,13,NormDegree(360.0 - (Frac*360.0)));
					DrawNIRDTriangleScreen(Fx,Fy,13,NormDegree(360.0 - (Frac*360.0)));
			}

		//  draw sidewinder cue
	 }

	 HUDtextLg( HUD_WIN_9(0,0), "GUN");
	 HUDtextLg( HUD_WIN_9(0,9), "%03d", P->WeapLoad[GUNS_STATION].Count);
	if(UFC.MasterArmState == OFF)
	{
	  HUDline( HUD_WIN_9(-5,-3), HUD_WIN_9(25,9) );
	  HUDline( HUD_WIN_9(-5,9),  HUD_WIN_9(25,-3) );
	}
}

//*****************************************************************************************************************************************
// AG GUNS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayAGGuns(PlaneParams *P)
{
	int		PosX, PosY;
	FPoint	Vel;
	FPoint	tVector;

	WeapStores.AGunCanFire = TRUE;

	float BulletVelocity = 3380.0;	// this will ultimately be pulled from weapons structure

	#define SIN_GUN_SLOPE 0.0262	// 1.5°
	#define COS_GUN_SLOPE 0.9997

	HUDtextLg( HUD_WIN_6(0,0), "CCIP" );
	HUDtextLg( HUD_WIN_7(0,0), "GUN" );
	HUDtextLg( HUD_WIN_7(0,9), "%03d", P->WeapLoad[GUNS_STATION].Count );

	Vel.X = (P->Orientation.I.X*COS_GUN_SLOPE - P->Orientation.K.X*SIN_GUN_SLOPE)*BulletVelocity;
	Vel.Y = (P->Orientation.I.Y*COS_GUN_SLOPE - P->Orientation.K.Y*SIN_GUN_SLOPE)*BulletVelocity;
	Vel.Z = (P->Orientation.I.Z*COS_GUN_SLOPE - P->Orientation.K.Z*SIN_GUN_SLOPE)*BulletVelocity;

	double VertVel = Vel.Y;
	float  Time = (float)(1.0f/32.0f)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*32.0f)*(P->HeightAboveGround * WUTOFT))));

	tVector.SetValues( Vel.X*Time*FTTOWU, -P->HeightAboveGround, Vel.Z*Time*FTTOWU );
	tVector *= ViewMatrix;
	tVector.Perspect( &PosX, &PosY );

  // GrDrawPolyCircleArc( GrBuffFor3D, Fx, Fy, 12,13,270,NormDegree(270.0 + (Frac*360)),30,HUDColorRGB->Red,HUDColorRGB->Green,HUDColorRGB->Blue,0);

	DrawPipper(PosX,PosY);

	// DisplayPipper( PosX, PosY, tVector.Length() );
}

//*****************************************************************************************************************************************
// AG HUD MODES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void HudBombAutoMode(PlaneParams *P)
{
	HUDtextLg( HUD_WIN_5(-6,-2), "AUTO" );

	if (!AG_weapOn || P->AGDesignate.X == -1)
	{
 			int x1 = VelVectorX + round( 3 * HUDsin );	// edge of VV
			int y1 = VelVectorY + round( 3 * HUDcos );

			float Deg  = P->Pitch/DEGREE;
			float PosX,PosY;

			PosX = VelVectorX;

			if(Deg > 180)
			{
				Deg = 360.0 - Deg;
				if(Deg > 7.5) Deg = 7.5;
				PosY = VelVectorY + ( ((float)HUD_DEGREE*7.5)*(1.0 - (Deg/7.5)) );
			}
			else
			{
				PosY = VelVectorY + ((float)HUD_DEGREE*7.5);
			}

			int x2 = VelVectorX;
			int y2 = PosY - round( 12 * HUDcos );

			int TempX1,TempY1,TempX2,TempY2;
			TempX1 = x1; TempX2 = x2; TempY1 = y1; TempY2 = y2;

			CRTline( x1,y1, x2,y2 );	// DIL

			if( GrClipLine(GrBuffFor3D,&TempX1,&TempY1,&TempX2,&TempY2) )
			{
				  CRTline( x1,y1, x2,y2 );	// DIL
				//DisplayPipper( PosX, PosY, tVector.Length() );	// reticle pipper
	    		DrawPipper(PosX, PosY);
			}

			return;
 	}

		/*
	if (!AG_weapOn || P->AGDesignate.X == -1)
	{
		CRTicon( VelVectorX, VelVectorY, SM_PIPPER );
		return;
	}
		 * */

	// Must have designated target AND weapon selected !!!
	//
	// DrawPipper(VelVectorX,VelVectorY);

	//AGpipper(P);

	if (AGDesignateInCone(P))
	{
		SetUFCAGTrel(P,0);
		ShowCountDown( "REL", UFC.TRelCountDown );
		AIC_WSO_Bomb_Speech( P, WSO_BOMB_TREL, UFC.TRelCountDown );
		TRelSet = TRUE;
	}
	else
	{
//		ShowCountDown( "TTGT", UFC.DataCurrTGTDistTime );
		lBombTimer = 161;
	}

	//	Draw release cue based on TREL time referenced to velocity vector
	//	At 10 seconds TREL, release cue is 5° above VV
	//
	if ((UFC.TRelCountDown <= 10) && P->Trigger2)  // don't countdown w/o consent
	{
		int  ReleaseCue = VelVectorY - UFC.TRelCountDown * HUD_DEGREE / 2;

		CRTline( VelVectorX-16, ReleaseCue, VelVectorX+16, ReleaseCue );
		CRTline( VelVectorX- 3, VelVectorY, VelVectorX+ 3, VelVectorY );
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudBombCCIPMode(PlaneParams *P)
{
	static int inPickle=0;		// were we in pickle last frame ?
	int nuPickle = P->Trigger2;	// current pickle state

	HUDtextLg( HUD_WIN_5(0,0), "CCIP" );

	if (!AG_weapOn && (!inPickle || !nuPickle))
	{
		DrawPipper(VelVectorX,VelVectorY);
		inPickle = 0;
		return;
	}

	FPoint 	ImpactPoint;
	float   iTime = GetAGImpactTime( P, P->IfVelocity.Y, &ImpactPoint );

	AG_ImpactPoint = ImpactPoint;

	if (nuPickle)	// Force AUTO mode while pickle
	{
		if (!inPickle)  P->AGDesignate = ImpactPoint;	// new pickle

		 ShowCountDown( "REL", 0 );
		 DrawPipper(VelVectorX,VelVectorY);
		//AGpipper(P);
	}
	else	// Draw reticle pipper @ impact point w/ Displayed Impact Line (DIL)
	{
		int		PosX, PosY;
		FPoint  tVector;

		tVector  = ImpactPoint;
		tVector -= P->WorldPosition;
		tVector *= ViewMatrix;

		int PerpectOk = tVector.Perspect( &PosX, &PosY );

		int x1 = VelVectorX + round( 3 * HUDsin );	// edge of VV
		int y1 = VelVectorY + round( 3 * HUDcos );

		int x2 = PosX; // - round( 12 * HUDsin );	// edge of pipper
		int y2 = PosY; //- round( 12 * HUDcos );

		int TempX1,TempY1,TempX2,TempY2;
		TempX1 = x1; TempX2 = x2; TempY1 = y1; TempY2 = y2;

		//CRTline( x1,y1, x2,y2 );	// DIL

		if( GrClipLine(GrBuffFor3D,&TempX1,&TempY1,&TempX2,&TempY2) )
		{
			 if(PerpectOk)
			 {
		      CRTline( TempX1,TempY1, TempX2, TempY2 );	// DIL
		      //DisplayPipper( PosX, PosY, tVector.Length() );	// reticle pipper
	        //DrawPipper(PosX, PosY);
			    CRTline( PosX-9,PosY,PosX+9,PosY );
			    CRTline( PosX,PosY+9,PosX,PosY-9 );

			 		if(PosY - 9 > GrBuffFor3D->ClipBottom)
			 		{
							float Diff = (PosY-9) - GrBuffFor3D->ClipBottom;
							float PosY = GrBuffFor3D->ClipBottom - Diff;

							if(PosY < y1)
								PosY = y1;

							float Dx;
							Dx = (float)TempX2 - VelVectorX;
							float Frac = ((float)TempY2-PosY)/((float)TempY2-(float)VelVectorY);
							PosX = TempX2 - Dx*Frac;

			    		CRTline(PosX - 8,PosY,PosX+8,PosY);
			    		CRTline(PosX - 8,PosY+1,PosX+8,PosY+1);
			 		}
			 }
		}

		DisplayFragCue( P, ImpactPoint );
	}
	inPickle = nuPickle;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CCIPGuns(PlaneParams *P)
{
	static int inPickle=0;		// were we in pickle last frame ?
	int nuPickle = P->Trigger2;	// current pickle state

	if (!AG_weapOn && (!inPickle || !nuPickle))
	{
		DrawPipper(VelVectorX,VelVectorY);
		inPickle = 0;
		return;
	}

	FPoint 	ImpactPoint;
	float   iTime = GetAGImpactTime( P, P->IfVelocity.Y, &ImpactPoint );

	AG_ImpactPoint = ImpactPoint;

	if (nuPickle)	// Force AUTO mode while pickle
	{
		if (!inPickle)  P->AGDesignate = ImpactPoint;	// new pickle

		 ShowCountDown( "REL", 0 );
		 DrawPipper(VelVectorX,VelVectorY);
		//AGpipper(P);
	}
	else	// Draw reticle pipper @ impact point w/ Displayed Impact Line (DIL)
	{
		int		PosX, PosY;
		FPoint  tVector;

		tVector  = ImpactPoint;
		tVector -= P->WorldPosition;
		tVector *= ViewMatrix;

		tVector.Perspect( &PosX, &PosY );

		int x1 = VelVectorX + round( 3 * HUDsin );	// edge of VV
		int y1 = VelVectorY + round( 3 * HUDcos );

		int x2 = PosX; // - round( 12 * HUDsin );	// edge of pipper
		int y2 = PosY; //- round( 12 * HUDcos );

		int TempX1,TempY1,TempX2,TempY2;
		TempX1 = x1; TempX2 = x2; TempY1 = y1; TempY2 = y2;

		CRTline( x1,y1, x2,y2 );	// DIL

		if( GrClipLine(GrBuffFor3D,&TempX1,&TempY1,&TempX2,&TempY2) )
		{
		  CRTline( x1,y1, x2,y2 );	// DIL
		  //DisplayPipper( PosX, PosY, tVector.Length() );	// reticle pipper
	    DrawPipper(PosX,PosY);

			 if(PosY - 9 > GrBuffFor3D->ClipBottom)
			 {
					float Diff = (PosY-9) - GrBuffFor3D->ClipBottom;
					float PosY = GrBuffFor3D->ClipBottom - Diff;

					if(PosY < y1)
						PosY = y1;

					float Dx;
					Dx = (float)TempX2 - VelVectorX;
					float Frac = ((float)TempY2-PosY)/((float)TempY2-(float)VelVectorY);
					PosX = TempX2 - Dx*Frac;

			    CRTline(PosX - 8,PosY,PosX+8,PosY);
			    CRTline(PosX - 8,PosY+1,PosX+8,PosY+1);

			 }
		}

		//DisplayFragCue( P, ImpactPoint );
	}
	inPickle = nuPickle;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CCIPRocket(PlaneParams *P)
{
	FPoint Vec;

	if(!RocketCCIPTimer.IsActive())
		RocketCCIPTimer.Set(3.0,GameLoopInTicks);

	float TimeElapsed = RocketCCIPTimer.TimeElapsed(GameLoopInTicks);
	RocketCCIPTimer.Set(3.0,GameLoopInTicks);

	if(LastVV.X == -1)
	{
		LastVV.X = VelVectorX - TranX;
		LastVV.Z = VelVectorY - TranY;
		LastVV.Y = 0;
	}

	if(LastRocketPipper.X == -1)
	{
		LastRocketPipper.X = VelVectorX - TranX;
		LastRocketPipper.Z = VelVectorY - TranY;
		LastRocketPipper.Y = 0;
	}

	Vec.X = VelVectorX - TranX;
	Vec.Z = VelVectorY - TranY;
	Vec.Y = 0;

	Vec -= LastRocketPipper;

	float DistToPipper = Vec.Length();

	Vec.Normalize();

	float TravelDist = 30.0*TimeElapsed;

	if(TravelDist < DistToPipper)
   	Vec *= TravelDist;
	else
   	Vec *= DistToPipper;

	Vec += LastRocketPipper;

	float PipX = Vec.X;
	float PipY = Vec.Z;

//	DrawPipper(PipX,PipY);
	DrawPipper(TranX + RESD_NUM_X(PipX), TranY + RESD_NUM_Y(PipY));

	static int inPickle=0;		// were we in pickle last frame ?
	int nuPickle = P->Trigger2;	// current pickle state

	LastVV.X = VelVectorX - TranX;
	LastVV.Z = VelVectorY - TranY;
	LastVV.Y = 0;

	LastRocketPipper = Vec;
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudBombManMode(PlaneParams *P)
{

	int usettmr = 0;

 	if(AG_weapDB)
	{
		if(AG_weapDB->iWeaponType == WEAPON_TYPE_AG_MISSILE)
		{
			usettmr = 1;
			if((Av.Flir.TimpactTimer.IsActive()) && (Av.Flir.TimeToImpact > -5000.0f))
			{
				usettmr = 0;
			}
		}
	}


	if(usettmr)
	{
		int tempval = 0;
		int ttm = 0;

		TargetInZone(Av.Weapons.CurAGWeap->WeapPage,&tempval,&ttm);

		if(tempval != WEAP_STORES_NO_TARGET_ERROR)
		{
			if(tempval == WEAP_STORES_MIN_RANGE_ERROR)
			{
				HUDtextLg( HUD_WIN_6(-6, 0), "MIN RNG");
			}
			else if(ttm == 0)
			{
				HUDtextLg( HUD_WIN_6(-6,0), "IN RNG");
			}
			else
			{
				ShowCountDown( "TTMR", ttm );
			}
		}
	}

	 DrawPipper(TranX + RESD_NUM_X(320),TranY + RESD_NUM_Y(116));
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudRocketMode(PlaneParams *P)
{

	HUDtextLg( HUD_WIN_6(0,0),(WeapStores.RocketPod.Mode == WEAP_STORES_CCIP_MODE) ? "CCIP" : "MAN");

	int Count = WeapQuantity(GetWeapId(LAU10_ID)) + WeapQuantity(GetWeapId(LAU68_ID));

	HUDtextLg( HUD_WIN_7(0,2), "RMT %3d", Count );

	if(WeapStores.RocketPod.Mode == WEAP_STORES_CCIP_MODE)
		CCIPRocket(P);
	else
		DrawPipper(TranX+ RESD_NUM_X(320),TranY+ RESD_NUM_Y(116));
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudHarmPBMode(PlaneParams *P, FPointDouble TargetLocation,float MinRange,float MaxRange)
{
	HUDtextLg( HUD_WIN_5(0,0), "HARM" );

	float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&TargetLocation);

	float DeltaMinRange = (DistToTarget - MinRange)*WUTONM;
	float DeltaMaxRange = (DistToTarget - MaxRange)*WUTONM;

	if(DeltaMaxRange > 0)
	{
		if(DeltaMaxRange > 5)
			DeltaMaxRange = 5;

		float PosY = 116 - (DeltaMaxRange*HUD_DEGREE);
		CRTline(TranX + RESD_NUM_X(320 - 8), TranY + RESD_NUM_Y(PosY),TranX + RESD_NUM_X(320+8), TranY + RESD_NUM_Y(PosY));
		CRTline(TranX + RESD_NUM_X(320 - 8), TranY + RESD_NUM_Y(PosY + 1), TranX + RESD_NUM_X(320 + 8), TranY + RESD_NUM_Y(PosY + 1));
	}

	if((DeltaMinRange <= 5) && (DeltaMinRange > 0))
	{
		float PosY = (116 + (DeltaMinRange*HUD_DEGREE))*HUDResMul;
		CRTline(TranX + RESD_NUM_X(320 - 8), TranY + RESD_NUM_Y(PosY), TranX + RESD_NUM_X(320+8), TranY + RESD_NUM_Y(PosY));
		CRTline(TranX + RESD_NUM_X(320 - 8), TranY + RESD_NUM_Y(PosY + 1), TranX + RESD_NUM_X(320 + 8), TranY + RESD_NUM_Y(PosY + 1));
	}

	if( (DeltaMaxRange < 0) && (DeltaMinRange > 0) )
	{
		HUDtextSm( HUD_WIN_7(0,0), "IN RNG" );
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudHarmTOOMode(PlaneParams *P,FPointDouble TargetLocation)
{
	HUDtextSm( HUD_WIN_5(0,0), "HARM" );

	int	PosX, PosY;
	FPoint tVector;

	tVector = TargetLocation;

	tVector -= P->WorldPosition;
	tVector *= ViewMatrix;
	tVector.Perspect( &PosX, &PosY );

	int AtHudEdge = CheckDesignateInHUD(PosX,PosY);

	// Keep TD box at edge of HUD
	ClipToHUD( &PosX, &PosY );

	if(AtHudEdge)
	{
	  CRTbox( PosX-5, PosY-5, PosX+5, PosY+5 );
	}
	else
	{
			if( HudFlashTimer.TimeUp(GameLoopInTicks) )
			{
				AA_flash = !AA_flash;
				HudFlashTimer.Set(0.25,GameLoopInTicks);
			}

			if (AA_flash)
			{
	      CRTbox( PosX-5, PosY-5, PosX+5, PosY+5 );
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudHarmSPMode(PlaneParams *P)
{
//	int Page,Mode;
//	int Result = GetAGWeapPageAndModeForHud(&Page,&Mode);

	if( (Av.Harm.SelfProtectOn) && (WeapStores.HarmOverride) )
	{
		sprintf(TmpStr,"PLBK");
	  HUDtextLg( HUD_WIN_9(0,-20), TmpStr );
	}
	else if( (Av.Harm.SelfProtectOn) && (!WeapStores.HarmOverride) )
	{
		sprintf(TmpStr,"HARM");
	  HUDtextLg( HUD_WIN_9(0,-20), TmpStr );
	}

	// jlm something about Harm with an X through it needs to be checked here
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudMavMode(PlaneParams *P,int Page)
{
	int TargetExists = FALSE;
  FPointDouble Target;

	switch(Page)
	{
	  case AGM_65_WEAP_PAGE : sprintf(TmpStr,"MAVG"); break;
		case AGM_65F_WEAP_PAGE: sprintf(TmpStr,"MAVF"); break;
		case AGM_65E_WEAP_PAGE: sprintf(TmpStr,"MAVE"); break;
	}

  HUDtextSm( HUD_WIN_7(0,0), TmpStr );

	if( (Av.Mav.TrackOn) && (Av.Mav.TargetObtained) )
  {
		 if(Av.Mav.TrackPosObtained)
	      Target = Av.Mav.TrackPos;
     else
	      GetWeapCamTargetPosition(&Av.Mav,&Target);

		 TargetExists = TRUE;
	}
#if 0
	else if(Page == AGM_65E_WEAP_PAGE)
	{
		int FlightProfile;
		int GroundTargetType;
		void *Trgt;

		GetWeaponData(Av.Weapons.CurAGWeap->WeapPage,&FlightProfile,&Trgt,&GroundTargetType,&Target);

		if(Target.X >= 0)
		{
			TargetExists = TRUE;
		}
	}
#endif

	if(TargetExists)
	{
		if(Page == AGM_65E_WEAP_PAGE)
		{
			HUDtextSm( HUD_WIN_6(0,0), "MAV LKD");
		}

			float MinRange,MaxRange;
			DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
  		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

			MinRange *= FTTOWU;
  		MaxRange *= FTTOWU;

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&Target);

			float DeltaMinRange = (DistToTarget - MinRange)*WUTONM;
			float DeltaMaxRange = (DistToTarget - MaxRange)*WUTONM;

			if( (DeltaMaxRange < 0) && (DeltaMinRange > 0) )
			{
		  		HUDtextSm( HUD_WIN_5(0,0), "IN RNG" );
			}
			else if(DeltaMaxRange > 0)
			{
				int Time = (DeltaMaxRange * NMTOFT)/PlayerPlane->IfHorzVelocity;

				if(Time > 99)
					Time = 99;
				if(Time < 0)
					Time = 0;

				sprintf(TmpStr,"%d %s", Time, "TTMR" );
		  		HUDtextSm( HUD_WIN_5(0,0), TmpStr);
			}

			int PosX,PosY;

			FPoint tVector;

			tVector = Target;

			tVector -= P->WorldPosition;
			tVector *= ViewMatrix;
			if( !tVector.Perspect( &PosX, &PosY ) ) return;

	    int InHud = CheckDesignateInHUD(PosX,PosY);

			if(InHud)
			{
		    CRTline(PosX - 3,PosY - 3,PosX+3,PosY-3);
		    CRTline(PosX - 3,PosY - 3,PosX,PosY+5);
		    CRTline(PosX + 3,PosY - 3,PosX,PosY+5);
			}
	}
 }

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudWalleyeMode(PlaneParams *P)
{
	DrawPipper(TranX + RESD_NUM_X(320),TranY + RESD_NUM_Y(116 + (3 * HUD_DEGREE)));

	if(PlayerPlane->AGDesignate.X != -1)
	{
			float MinRange,MaxRange;
			DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
  		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

			MinRange *= FTTOWU;
  		MaxRange *= FTTOWU;

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);

			float DeltaMinRange = (DistToTarget - MinRange)*WUTONM;
			float DeltaMaxRange = (DistToTarget - MaxRange)*WUTONM;

			if( (DeltaMaxRange < 0) && (DeltaMinRange > 0) )
			{
	  		HUDtextSm( HUD_WIN_5(0,0), "IN RNG" );
			}
	}

	HUDtextSm( HUD_WIN_6(0,0),"WE" );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

#define MAX_BOMB_PROGS 5   // this is defined in avmodes....so make sure if avmodes changes, this changes.
extern HarpoonProgType HarpoonProgs[MAX_BOMB_PROGS];

void HudHarpoonMode(PlaneParams *P)
{
   HarpoonProgType *B = &HarpoonProgs[WeapStores.Harpoon.CurProg];

	if((PlayerPlane->Altitude < 200) && (HarpoonProgs[WeapStores.Harpoon.CurProg].Mode == HARPOON_BOL_MODE))
	{
		sprintf(TmpStr,"ALT");
	    HUDtextSm( HUD_WIN_5(0,0),TmpStr);
	}
	else if(WeapStores.Harpoon.InZone)
	  HUDtextSm( HUD_WIN_5(0,0),"IN ZONE" );
	else
	{
		if( (WeapStores.Harpoon.Condition == WEAP_STORES_MAX_RANGE_ERROR) && (B->Mode == HARPOON_RBL_MODE) )
		{
#if 0
			int Min,Sec;
			Min = WeapStores.Harpoon.TimeToMaxInSec;
			Min /= 60;
			if(Min > 59) Min = 59;
			Sec = WeapStores.Harpoon.TimeToMaxInSec % 60;
			sprintf(TmpStr,"%02d:%02d TTMR",Min,Sec);
#else
			int Sec;
			Sec = WeapStores.Harpoon.TimeToMaxInSec;
			if(Sec > 99)
				Sec = 99;
			sprintf(TmpStr,"%02d TTMR",Sec);
#endif
	    HUDtextSm( HUD_WIN_7(0,0),TmpStr);
		}
		else if(WeapStores.Harpoon.Condition == WEAP_STORES_MIN_RANGE_ERROR)
		{
			sprintf(TmpStr,"MIN RNG");
		    HUDtextSm( HUD_WIN_5(0,0),TmpStr);
		}
		else if(WeapStores.Harpoon.Condition == WEAP_STORES_ALT_ERROR)
		{
			sprintf(TmpStr,"ALT");
	    HUDtextSm( HUD_WIN_5(0,0),TmpStr);
		}
		else if(WeapStores.Harpoon.Condition == WEAP_STORES_OFF_AXIS_ERROR)
		{
			sprintf(TmpStr,"OFF AXIS");
	    HUDtextSm( HUD_WIN_5(0,0),TmpStr);
		}
		else if(WeapStores.Harpoon.Condition == WEAP_STORES_NO_TARGET_ERROR)
		{
			sprintf(TmpStr,"NO TARGET");
	    HUDtextSm( HUD_WIN_5(0,0),TmpStr);
		}
	}

	sprintf(TmpStr,"HP %s",(B->Mode == HARPOON_RBL_MODE) ? "R/BL" : "BOL");
	HUDtextSm( HUD_WIN_6(0,0),TmpStr );

	/*
	if(PlayerPlane->AGDesignate.X != -1)
	{
			float MinRange,MaxRange;
			DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
  		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

  		MaxRange *= FTTOWU;

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);

			float DeltaMaxRange = (DistToTarget - MaxRange);

			if(DeltaMaxRange > 0)
			{
        float VelToTarget = AGClosureRate(PlayerPlane,PlayerPlane->AGDesignate);
				VelToTarget *= NMTOFT;
				VelToTarget /= 3600;
    		VelToTarget *= FTTOWU;

 			int SecToMax     = (DeltaMaxRange)/VelToTarget;
				if(SecToMax < 0) SecToMax = 0;
				if(SecToMax > 99) SecToMax = 99;

				sprintf(TmpStr,"%d TTMR",SecToMax);
	      HUDtextSm( HUD_WIN_7(0,0),TmpStr);
			}
	}
	 */

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

extern SlamerProgType SlamerProgs[MAX_BOMB_PROGS];
extern JsowProgType   JsowProgs[MAX_BOMB_PROGS];
extern JsowProgType   JdamProgs[MAX_BOMB_PROGS];

void HudSlamerMode(PlaneParams *P)
{
  SlamerProgType *B = &SlamerProgs[WeapStores.Slamer.CurProg];

	HUDtextSm( HUD_WIN_6(0,0),"SLAM");

	if(WeapStores.Slamer.InZone)
	  HUDtextSm( HUD_WIN_5(0,0),"IN ZONE" );
	else
	{
		if(WeapStores.Slamer.Condition == WEAP_STORES_MAX_RANGE_ERROR)
		{
#if 0
			int Min,Sec;
			Min = WeapStores.Slamer.TimeToMaxInSec;
			Min /= 60;
			if(Min > 59) Min = 59;
			Sec = WeapStores.Slamer.TimeToMaxInSec % 60;
			sprintf(TmpStr,"%02d:%02d TTMR",Min,Sec);
#else
			int Sec;
			Sec = WeapStores.Slamer.TimeToMaxInSec;
			if(Sec > 99)
				Sec = 99;
			sprintf(TmpStr,"%02d TTMR",Sec);
#endif

		}
		else if(WeapStores.Slamer.Condition == WEAP_STORES_MIN_RANGE_ERROR)
			sprintf(TmpStr,"MIN RNG");
		else if(WeapStores.Slamer.Condition == WEAP_STORES_ALT_ERROR)
			sprintf(TmpStr,"ALT");
		else if(WeapStores.Slamer.Condition == WEAP_STORES_OFF_AXIS_ERROR)
			sprintf(TmpStr,"OFF AXIS");
		else if(WeapStores.Slamer.Condition == WEAP_STORES_NO_TARGET_ERROR)
			sprintf(TmpStr,"NO TARGET");

	  HUDtextSm( HUD_WIN_5(0,0),TmpStr);
	}

	if(PlayerPlane->AGDesignate.X != -1)
	{
			float MinRange,MaxRange,OptRange;
			DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
  		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

			MinRange *= FTTOWU;
  		MaxRange *= FTTOWU;

			OptRange = MinRange + ( (MaxRange - MinRange)*0.75 );

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);

			float DeltaOptRange = (DistToTarget - OptRange);

			if(DeltaOptRange > 0)
			{
				sprintf(TmpStr,"%3.1f LP",(float)(DeltaOptRange*WUTONM));
				HUDtextSm( HUD_WIN_8(0,0),TmpStr);

        float VelToTarget = AGClosureRate(PlayerPlane,PlayerPlane->AGDesignate);
				VelToTarget *= NMTOFT;
				VelToTarget /= 3600;
    		VelToTarget *= FTTOWU;

 			  int SecToOpt  = (DeltaOptRange)/VelToTarget;
				if(SecToOpt < 0) SecToOpt = 0;
				if(SecToOpt > 99) SecToOpt = 99;

				sprintf(TmpStr,"%d TTLP",SecToOpt);
	      HUDtextSm( HUD_WIN_7(0,0),TmpStr);

				float OptAlt;

				switch(B->Flt)
				{
					case WEAP_STORES_HIGH: OptAlt = 20000; break;
				  case WEAP_STORES_MED : OptAlt = 10000; break;
			    case WEAP_STORES_LOW : OptAlt = 1000; break;
				}

				float AltDiff = OptAlt - PlayerPlane->Altitude;
				float Frac = AltDiff/5000.0;
				if(Frac > 1.0) Frac = 1.0;
				if(Frac < -1.0) Frac = -1.0;
				float PosY = 116 - (Frac*(5.0*HUD_DEGREE));

//		    CRTline(320-10,PosY,320+10,PosY);
//		    CRTline(320-10,PosY+1,320+10,PosY+1);
		    CRTline(TranX + RESD_NUM_X(320-25), TranY + RESD_NUM_Y(PosY),TranX + RESD_NUM_X(320+25), TranY + RESD_NUM_Y(PosY));
		    CRTline(TranX + RESD_NUM_X(320-25), TranY + RESD_NUM_Y(PosY+1),TranX + RESD_NUM_X(320+25), TranY + RESD_NUM_Y(PosY+1));
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudJsowMode(PlaneParams *P)
{
  JsowProgType *B = &JsowProgs[WeapStores.Jsow.CurProg];

	if(WeapStores.Jsow.InZone)
	  HUDtextSm( HUD_WIN_5(0,0),"IN ZONE" );
	else
	{
		if(WeapStores.Jsow.Condition == WEAP_STORES_MAX_RANGE_ERROR)
		{
#if 0
			int Min,Sec;
			Min = WeapStores.Jsow.TimeToMaxInSec;
			Min /= 60;
			if(Min > 59) Min = 59;
			Sec = WeapStores.Jsow.TimeToMaxInSec % 60;
			sprintf(TmpStr,"%02d:%02d TTMR",Min,Sec);
#else
			int Sec;
			Sec = WeapStores.Jsow.TimeToMaxInSec;
			if(Sec > 99)
				Sec = 99;
			sprintf(TmpStr,"%02d TTMR",Sec);
#endif
		}
		else if(WeapStores.Jsow.Condition == WEAP_STORES_MIN_RANGE_ERROR)
			sprintf(TmpStr,"MIN RNG");
		else if(WeapStores.Jsow.Condition == WEAP_STORES_ALT_ERROR)
			sprintf(TmpStr,"ALT");
		else if(WeapStores.Jsow.Condition == WEAP_STORES_OFF_AXIS_ERROR)
			sprintf(TmpStr,"OFF AXIS");
		else if(WeapStores.Jsow.Condition == WEAP_STORES_NO_TARGET_ERROR)
			sprintf(TmpStr,"NO TARGET");

	  HUDtextSm( HUD_WIN_5(0,0),TmpStr);
	}

	HUDtextSm( HUD_WIN_6(0,0),"JSOW");

	if(PlayerPlane->AGDesignate.X != -1)
	{
			float MinRange,MaxRange,OptRange;
			DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
  		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

			MinRange *= FTTOWU;
  		MaxRange *= FTTOWU;

			OptRange = MinRange + ( (MaxRange - MinRange)*0.75 );

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);

			float DeltaOptRange = (DistToTarget - OptRange);

			if(DeltaOptRange > 0)
			{
				sprintf(TmpStr,"%3.1f LP",(float)(DeltaOptRange*WUTONM));
				HUDtextSm( HUD_WIN_8(0,0),TmpStr);

        float VelToTarget = AGClosureRate(PlayerPlane,PlayerPlane->AGDesignate);
				VelToTarget *= NMTOFT;
				VelToTarget /= 3600;
    		VelToTarget *= FTTOWU;

 			  int SecToOpt  = (DeltaOptRange)/VelToTarget;
				if(SecToOpt < 0) SecToOpt = 0;
				if(SecToOpt > 99) SecToOpt = 99;

				sprintf(TmpStr,"%d TTLP",SecToOpt);
	      HUDtextSm( HUD_WIN_7(0,0),TmpStr);
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void HudJdamMode(PlaneParams *P)
{
  JsowProgType *B = &JdamProgs[WeapStores.Jdam.CurProg];

	if(WeapStores.Jdam.InZone)
	  HUDtextSm( HUD_WIN_5(0,0),"IN ZONE" );
	else
	{
		if(WeapStores.Jdam.Condition == WEAP_STORES_MAX_RANGE_ERROR)
		{
#if 0
			int Min,Sec;
			Min = WeapStores.Jdam.TimeToMaxInSec;
			Min /= 60;
			if(Min > 59) Min = 59;
			Sec = WeapStores.Jdam.TimeToMaxInSec % 60;
			sprintf(TmpStr,"%02d:%02d TTMR",Min,Sec);
#else
			int Sec;
			Sec = WeapStores.Jdam.TimeToMaxInSec;
			if(Sec > 99)
				Sec = 99;
			sprintf(TmpStr,"%02d TTMR",Sec);
#endif
		}
		else if(WeapStores.Jdam.Condition == WEAP_STORES_MIN_RANGE_ERROR)
			sprintf(TmpStr,"MIN RNG");
		else if(WeapStores.Jdam.Condition == WEAP_STORES_ALT_ERROR)
			sprintf(TmpStr,"ALT");
		else if(WeapStores.Jdam.Condition == WEAP_STORES_OFF_AXIS_ERROR)
			sprintf(TmpStr,"OFF AXIS");
		else if(WeapStores.Jdam.Condition == WEAP_STORES_NO_TARGET_ERROR)
			sprintf(TmpStr,"NO TARGET");

	  HUDtextSm( HUD_WIN_5(0,0),TmpStr);
	}

	HUDtextSm( HUD_WIN_6(0,0),"JDAM");

	if(PlayerPlane->AGDesignate.X != -1)
	{
			float MinRange,MaxRange,OptRange;
			DBWeaponType *pDBWeapon = &pDBWeaponList[Av.Weapons.CurAGWeap->W->WeapIndex];
  		WGetGBUMavMinMax(PlayerPlane,pDBWeapon,&MinRange,&MaxRange);

			MinRange *= FTTOWU;
  		MaxRange *= FTTOWU;

			OptRange = MinRange + ( (MaxRange - MinRange)*0.75 );

			float DistToTarget = Dist2D(&PlayerPlane->WorldPosition,&PlayerPlane->AGDesignate);

			float DeltaOptRange = (DistToTarget - OptRange);

			if(DeltaOptRange > 0)
			{
				sprintf(TmpStr,"%3.1f LP",(float)(DeltaOptRange*WUTONM));
				HUDtextSm( HUD_WIN_8(0,0),TmpStr);

        float VelToTarget = AGClosureRate(PlayerPlane,PlayerPlane->AGDesignate);
				VelToTarget *= NMTOFT;
				VelToTarget /= 3600;
    		VelToTarget *= FTTOWU;

 			  int SecToOpt  = (DeltaOptRange)/VelToTarget;
				if(SecToOpt < 0) SecToOpt = 0;
				if(SecToOpt > 99) SecToOpt = 99;

				sprintf(TmpStr,"%d TTLP",SecToOpt);
	      HUDtextSm( HUD_WIN_7(0,0),TmpStr);
			}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DisplayAGHUD( PlaneParams *P )
{
	int DrawASL = FALSE;

  AGGetWeapon(P);

	AG_ImpactPoint.X = AG_ImpactPoint.Y = AG_ImpactPoint.Z = -1;

	int Page,Mode;
  int Result = GetAGWeapPageAndModeForHud(&Page,&Mode);

	if(!Result) return;

 	UFC.AGSubMode = MAN_AG;

	if( (Page == DUMB_WEAP_PAGE) || (Page == CLUSTER_WEAP_PAGE) || (Page == TALD_WEAP_PAGE) )
	{
			switch(Mode)
			{
				case WEAP_STORES_AUTO_MODE: UFC.AGSubMode = AUTO_AG;
																		DrawASL = TRUE;
					                          HudBombAutoMode(PlayerPlane);
																		break;
				case WEAP_STORES_CCIP_MODE: UFC.AGSubMode = CDIP_AG;
																		DrawASL = FALSE;
					                          HudBombCCIPMode(PlayerPlane);
																		break;
				case WEAP_STORES_MAN_MODE:  HudBombManMode(PlayerPlane);
																		DrawASL = FALSE;
																		break;
			}
	}
	else if(Page == ROCKET_POD_WEAP_PAGE)
	{
		HudRocketMode(PlayerPlane);
		DrawASL = FALSE;
	}
	else if(Page == HARM_WEAP_PAGE)
	{
			if(Mode == HARM_TOO_MODE)
			{
				 DrawASL = FALSE;
				 FPointDouble TargetLocation;
				 float MinRange,MaxRange;
         int TargetFound = GetHarmInfoForHud(&TargetLocation,&MinRange,&MaxRange);
				 if(TargetFound)
				   HudHarmTOOMode(PlayerPlane,TargetLocation);
			}
			else if(Mode == HARM_PB_MODE)
			{
				 DrawASL = TRUE;
  			 FPointDouble TargetLocation;
				 float MinRange,MaxRange;
			   UFC.AGSubMode = MAN_AG;
         int TargetFound = GetHarmInfoForHud(&TargetLocation,&MinRange,&MaxRange);
				 if(TargetFound)
				   HudHarmPBMode(PlayerPlane,TargetLocation,MinRange,MaxRange);
			}
			else if(Mode == HARM_SP_MODE)
			{
				 DrawASL = FALSE;
//				 HudHarmSPMode(PlayerPlane);
			}
	}
	else if( (Page == AGM_65_WEAP_PAGE) || (Page == AGM_65F_WEAP_PAGE) || (Page == AGM_65E_WEAP_PAGE) )
	{
		 DrawASL = FALSE;
		 HudMavMode(PlayerPlane,Page);
	}
	else if(Page == WALLEYE_WEAP_PAGE)
	{
		 DrawASL = FALSE;
		 HudWalleyeMode(PlayerPlane);
	}
	else if(Page == HARPOON_WEAP_PAGE)
	{
		 DrawASL = FALSE;
		 HudHarpoonMode(PlayerPlane);
	}
	else if(Page == SLAMER_WEAP_PAGE)
	{
		 DrawASL = FALSE;
		 HudSlamerMode(PlayerPlane);
	}
	else if(Page == JSOW_WEAP_PAGE)
	{
		 DrawASL = FALSE;
		 HudJsowMode(PlayerPlane);
	}
	else if(Page == JDAM_WEAP_PAGE)
	{
		 DrawASL = FALSE;
		 HudJdamMode(PlayerPlane);
	}




	//DisplayInCmd();
	//DisplaySniff();
  //DisplaySteering();
	//DisplayAGWeapons(P);

	DisplayAGDesignate(P,DrawASL);
 	if (AG_weapDB)
	{
		if(AG_weapDB->iSeekerType == 6)  //  if current weapon has IR seeker
		{
			HudShowOtherLaser(P);
		}
	}
}

//*****************************************************************************
//=============================================================================
//		I N I T   &   M A I N    F U N C T I O N S
//=============================================================================
//*****************************************************************************

//*****************************************************************************************************************************************
// INIT HEADING BAR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void InitHeadingBar()
{
 	#define XMinPos  (110.0f)
 	//#define XMaxPos  (450.0f)
 	//#define XMaxPos  (350.0f)
 	#define XMaxPos  (280.0f)

	#define ZPos     (5.0f*1024.0f)
	#define DownSideLine (40)

	int i,j;
	float Pitch;

	// Define pitch ladder points in 3D space (calculate positive values only)

	for (i=0; i<36; i++)
	{
		if (i<18) Pitch =     (float)i  * (5.0f*DEGREES_TO_RADIANS);
		else	  Pitch = (float)(36-i) * (5.0f*DEGREES_TO_RADIANS);

		double dSin, dCos;

		sincos( &dSin, &dCos, Pitch );

		// define non-zero pitch lines
		if (i != 18)
		{
			float yVal = dSin * ZPos;
			float zVal = dCos * ZPos;

			if (i<18)  zVal = -zVal;

			UFC.HUDLadder[i].RawHUDPoint[0].X = -XMaxPos;
			UFC.HUDLadder[i].RawHUDPoint[0].Y = yVal;
			UFC.HUDLadder[i].RawHUDPoint[0].Z = zVal;

			UFC.HUDLadder[i].RawHUDPoint[1].X = -XMinPos;
			UFC.HUDLadder[i].RawHUDPoint[1].Y = yVal;
			UFC.HUDLadder[i].RawHUDPoint[1].Z = zVal;

			UFC.HUDLadder[i].RawHUDPoint[2].X = XMinPos;
			UFC.HUDLadder[i].RawHUDPoint[2].Y = yVal;
			UFC.HUDLadder[i].RawHUDPoint[2].Z = zVal;

			UFC.HUDLadder[i].RawHUDPoint[3].X = XMaxPos;
			UFC.HUDLadder[i].RawHUDPoint[3].Y = yVal;
			UFC.HUDLadder[i].RawHUDPoint[3].Z = zVal;
		}
		else
		{
			UFC.HUDLadder[i].RawHUDPoint[0].X = 0;
			UFC.HUDLadder[i].RawHUDPoint[0].Y = ZPos;
			UFC.HUDLadder[i].RawHUDPoint[0].Z = 0;
		}

		// make zero pitch lines wider

		if (i==0)
		{
			UFC.HUDLadder[i].RawHUDPoint[0].X -= (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) ? 300 : 50;
			UFC.HUDLadder[i].RawHUDPoint[3].X += (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) ? 300 : 50;
		}

		UFC.HUDLadder[i].PitchNum = round(Pitch * RADIANS_TO_DEGREES);	// what's our actual pitch in degrees

		// Let's bend pitch ladders 1/2 the actual pitch representation

		if ((i!=0) && (i!=18))	// don't bend zero pitch or +90°
		{
			float StepDownPitch = (XMaxPos - XMinPos) * tan( Pitch * 0.5f );

			float yVal = dCos * StepDownPitch;
			float zVal = dSin * StepDownPitch;

			if (i>18)  yVal = -yVal;
			if (i>18)  zVal = -zVal;

			UFC.HUDLadder[i].RawHUDPoint[0].Y +=  yVal;
			UFC.HUDLadder[i].RawHUDPoint[0].Z +=  zVal;

			UFC.HUDLadder[i].RawHUDPoint[3].Y +=  yVal;
			UFC.HUDLadder[i].RawHUDPoint[3].Z +=  zVal;
		}

		// calculate ground facing side tick marks

		if (i!=18)	// don't bother with +90°
		{
			float yVal = dCos * DownSideLine;
			float zVal = dSin * DownSideLine;

			if (i>18)  yVal = -yVal;
			if (i>18)  zVal = -zVal;

			UFC.HUDLadder[i].RawHUDPoint[4].X = UFC.HUDLadder[i].RawHUDPoint[0].X;
			UFC.HUDLadder[i].RawHUDPoint[4].Y = UFC.HUDLadder[i].RawHUDPoint[0].Y - yVal;
			UFC.HUDLadder[i].RawHUDPoint[4].Z = UFC.HUDLadder[i].RawHUDPoint[0].Z - zVal;

			UFC.HUDLadder[i].RawHUDPoint[5].X = UFC.HUDLadder[i].RawHUDPoint[3].X;
			UFC.HUDLadder[i].RawHUDPoint[5].Y = UFC.HUDLadder[i].RawHUDPoint[3].Y - yVal;
			UFC.HUDLadder[i].RawHUDPoint[5].Z = UFC.HUDLadder[i].RawHUDPoint[3].Z - zVal;
		}
	}

	// Let's do negative side of pitch ladders base on positive pitch values already created

	for (i=1; i<36; i++)
	{
		j = 72-i;
		if (i != 18)	// don't do -90 (this is special case for F18)
		{
			UFC.HUDLadder[j].PitchNum = -UFC.HUDLadder[i].PitchNum;

			for (int k=0;k<6;k++)
			{
				UFC.HUDLadder[j].RawHUDPoint[k].X =  UFC.HUDLadder[i].RawHUDPoint[k].X;
				UFC.HUDLadder[j].RawHUDPoint[k].Y = -UFC.HUDLadder[i].RawHUDPoint[k].Y;
				UFC.HUDLadder[j].RawHUDPoint[k].Z =  UFC.HUDLadder[i].RawHUDPoint[k].Z;
			}
		}
		else
		{
			UFC.HUDLadder[j].RawHUDPoint[0].X = 0;
			UFC.HUDLadder[j].RawHUDPoint[0].Y = -ZPos;
			UFC.HUDLadder[j].RawHUDPoint[0].Z = 0;
		}
	}
}

int SetVirtualHudCenter( void )
{
	int x, y;
	float x_mult,y_mult;

	/* -----------------10/22/99 12:45PM-------------------------------------------------------------------
	/* first calculate the center for this res:
	/* ----------------------------------------------------------------------------------------------------*/
	x_mult = (float)ScreenSize.cx * (1.0f/640.0f);
	y_mult = (float)ScreenSize.cy * (1.0f/480.0f);

	//FPoint p1( 0.0 FEET, 0.160 FEET, -2.750 FEET );	// Points from VirF15b.3dg
	FPoint p1( 0.0 FEET, 0.0 FEET, -2.750 FEET );	// Points from VirF15b.3dg
	p1*= 8.0f;//

	p1.X += VC.xdelta;
	p1.Y += VC.ydelta;
	p1.Z += VC.zdelta;

	FMatrix mat = Camera1.CameraRelativeMatrix;
	mat.Transpose();

	if (p1.RotateAndPerspect( &x, &y, &mat ))
	{
		// Find the HUD center in screen coordinants.
		// Use this to find the HUD clip window.
		// If we use this point for the TranX/Y values,
		// the HUD markings will remain centered in the HUD,
		// but ground relative symbols will move weirdly
		// in relation to fixed HUD markings.
		//
		// Most notably the CDIP line will switch to left/
		// right of VV as the VC is slewed left and right.
		//

		TranX = x - HUD_MIDX;
		TranY = y - HUD_MIDY;

		HudTop    = RESD_NUM_Y(HUD_TOP)   + TranY;
		HudLeft   = RESD_NUM_X(HUD_LEFT)  + TranX;
		HudRight  = RESD_NUM_X(HUD_RIGHT) + TranX;
		HudBottom = RESD_NUM_Y(HUD_BOTTOM)+ TranY;

		//Test
		//DrawBox(GrBuffFor3D, HudLeft, HudTop, HudRight- HudLeft, HudBottom- HudTop, 100, 0, 0);

		// Check if Hud if outside the Clip Region.. Quick out for drawing the hud.
		if (HudTop >= GrBuffFor3D->ClipBottom)	return FALSE;
		if (HudLeft >= GrBuffFor3D->ClipRight)  return FALSE;
		if (HudRight <= GrBuffFor3D->ClipLeft)	return FALSE;
		if (HudBottom <= GrBuffFor3D->ClipTop)	return FALSE;


		// This code amplifies TranX/Y so that ground symbols
		// remain fairly fixed in relation to fixed symbols.
		Camera1.SubjectMatrix.GetZNormal(p1);

		p1 *= -100.0 FEET;

		p1.RotateAndPerspect( &x, &y );

		TranX = x - HUD_MIDX;
		TranY = y - HUD_MIDY;

		return TRUE;
	}
	else
		return FALSE;
}


/*-----------------------------------------------------------------------------
 *
 *	SetHUDClipWindow()
 *
 */
void SetHUDClipWindow()
{
	HUDPushBuff.ClipTop		= GrBuffFor3D->ClipTop;
	HUDPushBuff.ClipLeft	= GrBuffFor3D->ClipLeft;
	HUDPushBuff.ClipRight	= GrBuffFor3D->ClipRight;
	HUDPushBuff.ClipBottom	= GrBuffFor3D->ClipBottom;
	HUDPushBuff.MidX		= GrBuffFor3D->MidX;
	HUDPushBuff.MidY		= GrBuffFor3D->MidY;

	// Define HUD window for rotation and perspect functions

	GrBuffFor3D->ClipTop	=  HudTop;     //HUD_TOP    + TranY;
	GrBuffFor3D->ClipLeft	=  HudLeft;    //HUD_LEFT   + TranX;
	GrBuffFor3D->ClipRight	= HudRight;   //HUD_RIGHT  + TranX;
	GrBuffFor3D->ClipBottom	= HudBottom;   //HUD_BOTTOM + TranY;

	// For Virtual Cockpit, we must clip to the smaller
	// of the HUD artwork and the HUD drawing window.

	/*
	if( CurrentView & COCKPIT_VIRTUAL_SEAT )
	{
		if (GrBuffFor3D->ClipTop    < HudTop   )  GrBuffFor3D->ClipTop    = HudTop;
		if (GrBuffFor3D->ClipLeft   < HudLeft  )  GrBuffFor3D->ClipLeft   = HudLeft;
		if (GrBuffFor3D->ClipRight  > HudRight )  GrBuffFor3D->ClipRight  = HudRight;
		if (GrBuffFor3D->ClipBottom > HudBottom)  GrBuffFor3D->ClipBottom = HudBottom;
	}
	 */


	if (GrBuffFor3D->ClipTop    <  0 )  GrBuffFor3D->ClipTop    =  0;
	if (GrBuffFor3D->ClipLeft   <  0 )  GrBuffFor3D->ClipLeft   =  0;
	if (GrBuffFor3D->ClipRight  > (ScreenSize.cx-1))  GrBuffFor3D->ClipRight  = (ScreenSize.cx-1);
	if (GrBuffFor3D->ClipBottom > (ScreenSize.cy-1))  GrBuffFor3D->ClipBottom = (ScreenSize.cy-1);

	// In HUD.cpp point.Perspect() for virtual cockpit requires MidX/Y
	// to be physical screen center.  Other views require it to be set
	// to the HUD center.  Also, when UFC calcs the pitch ladder, all
	// views require the true HUD center.  So if we are in the HUD code
	// (ie. "doHUD") and virtual cockpit, then leave the MidX/Y alone
	// (ie. physical screen center).
	//

	//Since we are drawing the Hud in polybuffs
	Set3DScreenClip();
	if (doHUD && (CurrentView & COCKPIT_VIRTUAL_SEAT))
		return;

//	GrBuffFor3D->MidX = HUD_MIDX + TranX;
//	GrBuffFor3D->MidY = HUD_MIDY + TranY;




}

/*-----------------------------------------------------------------------------
 *
 *	UndoHUDClipWindow()
 *
 */
void UndoHUDClipWindow()
{
	// Put real screen back based on last PushBuff

	GrBuffFor3D->ClipTop    = HUDPushBuff.ClipTop;
	GrBuffFor3D->ClipLeft   = HUDPushBuff.ClipLeft;
	GrBuffFor3D->ClipBottom = HUDPushBuff.ClipBottom;
	GrBuffFor3D->ClipRight  = HUDPushBuff.ClipRight;
	GrBuffFor3D->MidX =       HUDPushBuff.MidX;
	GrBuffFor3D->MidY =       HUDPushBuff.MidY;

	//Since we are drawing the Hud in polybuffs
	Set3DScreenClip();

}

/*-----------------------------------------------------------------------------
 *
 *	InitHUDvars()
 *
 */
void InitHUDvars(void)
{
	// init HUD static variables
	g_iHUDWeapID = 0;
	g_iHUDCount  = 0;

	g_dwAIMSndHandle = 0;
	g_dwAOASndHandle = 0;

	AboveLAW = 0;
	AOA_last = 0;
	AOA_unit = 0;
	AOA_mark = VxV_mark = AGL_mark = CUE_mark = GetTickCount();

	AA_init();
	AG_init();

	HudSteerPoint.X = -1;

	ILS_glide = 0;
	ILS_steer = 0;

	HudFlashTimer.Set(0.25,0);
	AA_flash = FALSE;

	AA_DrawASE = FALSE;
  TRelSet = FALSE;

	LastFlightStatus = 0;

	HUDResMul = (float)ScreenSize.cx/640.0f;
}

/*-----------------------------------------------------------------------------
 *
 *	InitHUD()
 *
 */
void InitHUD(void)
{
	InitHUDvars();


	// Load HUD Fonts
	SmHUDFont = GrLoadFont(RegPath("Cockpits","nHUDsm.fnt"));
	LgHUDFont = GrLoadFont(RegPath("Cockpits","nHUDlg.fnt"));


	RECT SrcRect, dstRect;
	LPDIRECTDRAWSURFACE4	D3DSurface;
	TextureRef our_ref;
	DDCOLORKEY key;
	GrIcon *pHudsym;
	DDBLTFX		BltFX;


	BltFX.dwSize = sizeof (BltFX);
	BltFX.dwFillColor = 0;

	memset(&key,0,sizeof(DDCOLORKEY));

	ZeroMemory(&our_ref,sizeof(our_ref));
	our_ref.CellWidth = 1.0f;
	our_ref.CellHeight = 1.0f;

	dstRect.left =0;
	dstRect.top = 0;


	SetTextureFormat( FT_16BIT_DATA );

	//Load Up Cookie Sheet
	D3DSurface = LoadPCXDDSurface(RegPath("Cockpits","HUDicons.pcx"), CurrentLoadingFormat,TRUE,UT_RGB_ALPHA,FALSE,TRUE);
	/////lpDDSPrimary->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &BltFX);
	//lpDDSPrimary->Blt(&SrcRect,D3DSurface , NULL,DDBLT_WAIT|DDBLT_KEYSRC  ,NULL);

	pHudsym = &HUDsym[0];
	for (int i=0; i<SM_NEXT_ICON; i++)
	{
		int wide = pHudsym->endX - pHudsym->offX + 1;
		int high = pHudsym->endY - pHudsym->offY + 1;

		if ((UINT)wide * (UINT)high > 2000)  continue;			// safety???    TW: No idea on this one, just left it in

		SrcRect.left	= pHudsym->offX;
		SrcRect.top		= pHudsym->offY;
		SrcRect.right	= SrcRect.left + wide;
		SrcRect.bottom	= SrcRect.top  + high;

		dstRect.right = wide;
		dstRect.bottom = high;

		pHudsym->buff = NULL;									//GrAllocGrBuff( wide, high, GR_INDEXED | GR_8BIT );
		pHudsym->wide = wide;
		pHudsym->high = high;
		pHudsym->pTexture = CreateTexture( &our_ref,  CeilToPow2( wide),	CeilToPow2( high),	UT_RGB_ALPHA,0 );


	//	pHudsym->pTexture->D3DSurface->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &BltFX);	// Clear background

		pHudsym->pTexture->D3DSurface->Blt( &dstRect, D3DSurface, &SrcRect, DDBLT_WAIT , NULL);		//NewGrCopyRect8( pHudsym->buff, 0,0, CutBuff, pHudsym->offX, pHudsym->offY, wide, high );
	//	lpDDSPrimary->Blt(NULL,pHudsym->pTexture->D3DSurface , NULL,DDBLT_WAIT|DDBLT_KEYSRC  ,NULL);


		pHudsym->pTexture->WidthMulter = (float)wide/(float)CeilToPow2( wide);
		pHudsym->pTexture->HeightMulter = (float)high/(float)CeilToPow2( high);

		pHudsym++;
	}
	HUDsym[SM_HEADING_BAR].wide -= 8;	// allow room at end for skewing

	D3DSurface->Release();
	ReSetTextureFormat();

	InitHeadingBar();

//	HudFlirBuff = GrAllocGrBuff((HUD_RIGHT-HUD_LEFT)+1, (HUD_BOTTOM-HUD_TOP)+1, GR_INDEXED | GR_8BIT);
}

/*-----------------------------------------------------------------------------
 *
 *	CleanupHUDSprites()
 *
 */
void CleanupHUDSprites(void)
{
	GrIcon *pHudsym;

	pHudsym = &HUDsym[0];
	for (int i=0; i< SM_NEXT_ICON; i++)
	{
		if (pHudsym->buff)
			GrFreeGrBuff( pHudsym->buff );

		pHudsym->buff = NULL;


		RemoveTexture( pHudsym->pTexture  );
		EasyFree(pHudsym->pTexture  );
		pHudsym->pTexture = NULL;

		pHudsym++;
	}



	GrDestroyFont( SmHUDFont );
  	GrDestroyFont( LgHUDFont );

//	if (HudFlirBuff)
//	{
//		GrFreeGrBuff(HudFlirBuff);
//		HudFlirBuff = NULL;
//	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetSteerPoint()
{
	FPointDouble  Loc;

	switch(Av.Hsi.SteeringMode)
	{
		 case HSI_TCN_MODE:   if(UFC.CurrentTacanPtr != NULL)
		 	                      HudSteerPoint = UFC.TacanPos;
													else
		 	                      HudSteerPoint.X = -1;
													break;
	   case HSI_TGT_MODE:   if(UFC.MasterMode == AG_MODE)
		 	                      HudSteerPoint = PlayerPlane->AGDesignate;
													else
		 	                      HudSteerPoint.X = -1;
													break;
		 case HSI_ACL_MODE:   if(PlayerPlane->AI.iHomeBaseId >= 0)
													{
													 	MovingVehicleParams *Carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
														GetLandingPoint(Carrier,Loc);
														HudSteerPoint = Loc;
													}
													else
													{
														HudSteerPoint.X = -1;
													}
													break;
		 case HSI_WPT_MODE:  	HudSteerPoint.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
	                        HudSteerPoint.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;
													HudSteerPoint.Y = 0;
													break;
		 case HSI_GPS_MODE:  	FPointDouble GpsLoc;

													if(Av.Hsi.CurGps >= 0)
													{
													  GpsLoc.X = AvGpsLabels[Av.Hsi.CurGps]->lX*FTTOWU;
													  GpsLoc.Z = AvGpsLabels[Av.Hsi.CurGps]->lY*FTTOWU;
													  GpsLoc.Y = LandHeight(GpsLoc.X,GpsLoc.Z);

														HudSteerPoint = GpsLoc;
													}
													else
			                         HudSteerPoint.X = -1;
	                        break;
	}

	if(HudSteerPoint.X != -1)
	{
    float DeltaAzim = ComputeHeadingToPoint(PlayerPlane->WorldPosition,PlayerPlane->Heading,HudSteerPoint,1);
		HudSteerBearing = NormDegree( (PlayerPlane->Heading/DEGREE) + DeltaAzim);
		HudSteerBearing = NormDegree(360.0 - HudSteerBearing);
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DisplayHUD()
 *
 */

void DisplayHUD( PlaneParams *P )
{
	float tone_level;

	if(!P)
		return;

	static int SaveHUDColor = HUDColor;

	SetTextureFormat(FT_FIRE_PALETTE);

	ShowMe();

	if (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
		DoAOASoundWarnings(PlayerPlane);	// Always update audible warnings

	if (!SimPause)
	{
		AG_tImpact -= seconds_per_frame;
		AA_Tpost -= seconds_per_frame;

		if(AA_TMem != -1)
		{
		  AA_TMem -= seconds_per_frame;

		  if (AA_TMem < 0)
			  AA_TMem = -1;
		}
		if (AA_Tpost < 0)
			AA_Tpost = 0;
	}

    if (CurrentView & (COCKPIT_ZOOM_MPD))
	{
		if( (AA_weapID == Aim9X_Id) || (AA_weapID == Aim9M_Id) || (AA_weapID == Aim9L_Id) )
		{
			tone_level = AIM9_IR_scan(P);
			Snd_AIM9_Tone( tone_level );
			Snd_AIM9_Lock( tone_level );
		}
	}

	if (CockpitSeat == NO_SEAT)  return;
    if (CurrentView & (COCKPIT_45UP | COCKPIT_BACK_SEAT | COCKPIT_ZOOM_MPD))  return;
	if (CurrentView == (COCKPIT_ACM_MODE | COCKPIT_ACM_FRONT | COCKPIT_ACM_DOWN))  return;
	if (CurrentView & (COCKPIT_LEFT | COCKPIT_RIGHT))  return;

	// Don't remove
	//if ((CurrentView & COCKPIT_VIRTUAL_SEAT) && ((fabs(Camera1.CurrentHeading) > (float)0x1800) || (Camera1.CurrentPitch > (float)0x1680)))
	//	return;

	if (!(P->SystemInactive & DAMAGE_BIT_HUD) && (LeftEngineOn || RightEngineOn))
	{
	//	if (P->SystemInactive & DAMAGE_BIT_HUD) return;
	//	if (!LeftEngineOn && !RightEngineOn)  return;

		//if (CurrentView & COCKPIT_FRONT)
		//{
			// JLM commented, so that look forward no cockpit will match virtual mode
			//if (CurrentView & COCKPIT_NOART)   TranY =  123;
			//if (CurrentView & COCKPIT_45DOWN)  TranY = -200;
		//	doHUD = 1;
		//}

	//	if (CurrentView & COCKPIT_ACM_FRONT)
	//	{
	//		TranY = 228;
	//		doHUD = 1;
	//	}

		if( (CurrentView & COCKPIT_VIRTUAL_SEAT) || (CurrentView & COCKPIT_FRONT) )
		{
			doHUD = SetVirtualHudCenter();
		}

		if (doHUD)
		{
			SetHUDClipWindow();

			SetSteerPoint();

			if (P->FlightStatus & PL_STATUS_CRASHED)
				HUDtextLg( 300, 120, "CRASHED" );
			else
			{	// Per frame inits.
				ShowETE = TRUE;	// show default UFC ETE

				// Calc our rotation trig once.
				sincosA( &HUDsin, &HUDcos, -P->Roll );

				if (UFC.NavFlirStatus)
				{
					//SaveHUDColor = HUDColor;
					//DisplayHUDFlir();
				}

				if (P->SystemInactive & DAMAGE_BIT_ADC)
				{
					DisplayHeadingBar(P);
//					DisplayVelocityVector(P);
//					DisplayPitchLadder(P);
					//DisplayPitchLadder(P);
					//DisplayHeadingBar(P);
					//DisplayGunCross();
					//DisplayWaterMark();
				}
				else
				{
					if( HudDeclutter == 0)	// reject 1 and reject 2
					{
						DisplayVelocityVector(P);
						DisplayPitchLadder(P);
						DisplayMach(P);
						DisplayGForce(P);
					}
					else
					{
						ComputeVelocityVector(P);
					}

					if( HudDeclutter != 2) //reject 2 stuff
					{
						DisplayHeadingBar(P);
						if(Av.Hsi.TimeOn)
							DisplayHUDClock();
						//TODO missing selected way point
					}

					DisplayAltitude(P);

					//DisplayWindow1(P);
					//DisplayWindow2(P);

					//DisplayWindow5(P);
					//DisplayWindow6(P);
					//DisplayWindow7(P);
					//DisplayWindow8(P);

					//DisplayRangeWin(P);

					DisplayAOA(P);
					DisplayIAS(P);

					if(g_Settings.gp.dwCheats & GP_CHEATS_HUD_THROTTLE)
						DisplayHUDThrottle(P);

					DrawHudAcq();


					//DisplayLAWAltitude(P);
					//DisplayIAS(P);
					//DisplayGunCross();

					if( (UFC.MasterMode != AA_MODE) && (P->FlightStatus & PL_GEAR_DOWN_LOCKED) )
		  				DisplayWaterMark();

					//DisplayAPHUD(P);
					//if (RealisticAPs || (!RealisticAPs && (UFC.APStatus == OFF)))
						//DisplayAOA(P);

					DisplayVerticalVelocity(P);
					DisplaySteering();

					int CurTarget = GetCurrentPrimaryTarget();
					if(CurTarget != -1)
					{
						DisplayRangeWin(P);
						DisplayVcWin(P);
					}

					// KLUDGE...SET VARS FOR DISPLAY ON MDIS
					TRelSet = FALSE;

		 			switch (UFC.MasterMode)
					{
			 			case AA_MODE:
							if(WeapStores.GunsOn)
								DisplayAAGuns(P);
							else
					        	DisplayAAHUD(P);
							break;

	  		 			case AG_MODE:
							if(WeapStores.GunsOn)
								DisplayAGGuns(P);
							else
								DisplayAGHUD(P);
				            break;

			  			case NAV_MODE:
							DisplayNavHUD(P);
							break;
					}
				}

				if (UFC.NavFlirStatus)  HUDColor = SaveHUDColor;
			}


		if(Av.Harm.SelfProtectOn == TRUE)
			HudHarmSPMode((PlaneParams *)Camera1.AttachedObject);

	    if( (UFC.MasterMode != AA_MODE) || ( (AA_weapID != Aim9L_Id) && (AA_weapID != Aim9M_Id) && (AA_weapID != Aim9X_Id) )  )
				 ResetAAGrowl();

			UndoHUDClipWindow();
// 		 	void GregAeroDebugInfo(PlaneParams *P);
// 		 	GregAeroDebugInfo(P);
		}
		else
		{
			if((AA_weapID == Aim9X_Id) && (!IsWeapCaged()))
			{
				if(((Camera1.SubType & (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) == (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) && (Camera1.TargetObjectType == CO_PLANE) && (Camera1.TargetObject))
				{
					tone_level = AIM9_IR_scan(P);
					Snd_AIM9_Tone( tone_level );
					Snd_AIM9_Lock( tone_level );		//  Moved here by Scott.
				}
			}
		}
	}

//	if (((P->TailHookState) || (P->FlightStatus & PL_GEAR_DOWN_LOCKED)) && MeatBallCarrier)
	if (DrawMeatball &&  MeatBallCarrier)
		DrawMeatballForPlane();

	// if gear status changes, init heading bar so that the middle pitch ladder will be long
	if( !(LastFlightStatus & PL_GEAR_DOWN_LOCKED) && (P->FlightStatus & PL_GEAR_DOWN_LOCKED) )
	{
		InitHeadingBar();
	  LastFlightStatus = P->FlightStatus;
	}
	else if( (LastFlightStatus & PL_GEAR_DOWN_LOCKED) && !(P->FlightStatus & PL_GEAR_DOWN_LOCKED) )
	{
		InitHeadingBar();
	  LastFlightStatus = P->FlightStatus;
	}

	doHUD = 0;
	TranX = 0;
	TranY = 0;
}

//*******************************************************************************************************************
void HudShowOtherLaser(PlaneParams *planepnt)
{
	float foffhead = 60.0f;
	int foundone = 0;
	void *pTarget;
	int iTargetType;
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	FPoint position;
	FPoint	foundpos;
	int cnt;
	int PosX,PosY;
    int InHud;
	FPoint planepos = planepnt->WorldPosition;

	FPoint tVector;


	position.SetValues(-1.0f, -1.0f, -1.0f);

	if(iFACHitFlags & FAC_LASER)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
		if(iTargetType == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)pTarget;
			position = vehiclepnt->WorldPosition;
		}
		else if(iTargetType == GROUNDOBJECT)
		{
			walker = (BasicInstance *)pTarget;
			position = walker->Position;
		}
		else
		{
			pTarget = NULL;
			position.X = -1.0f;
			position.Z = -1.0f;
		}

		if ((position.X > -1) && (position.Z > -1))
		{
			if((planepos - position) < (BUDDY_LAS_RANGE_NM * NMTOWU))
			{
				tVector = position;

				tVector -= planepnt->WorldPosition;
				tVector *= ViewMatrix;
				if( !tVector.Perspect( &PosX, &PosY ) ) return;

				InHud = CheckDesignateInHUD(PosX,PosY);

				if(InHud)
				{
					CRTline(PosX - 2,PosY - 2,PosX + 2, PosY + 2);
					CRTline(PosX - 2,PosY + 2,PosX + 2, PosY - 2);
				}
			}
		}
	}

	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if(cnt == MySlot)
				continue;

			if(iSlotToPlane[cnt] >= 0)
			{
				if(Planes[iSlotToPlane[cnt]].Status & PL_ACTIVE)
				{
					position = NetBuddyLasingData[cnt].laserloc;

					if ((position.X > -1) && (position.Z > -1))
					{
						if((planepos - position) < (BUDDY_LAS_RANGE_NM * NMTOWU))
						{
							tVector = position;

							tVector -= planepnt->WorldPosition;
							tVector *= ViewMatrix;
							if( !tVector.Perspect( &PosX, &PosY ) ) return;

							InHud = CheckDesignateInHUD(PosX,PosY);

							if(InHud)
							{
								CRTline(PosX - 2,PosY - 2,PosX + 2, PosY + 2);
								CRTline(PosX - 2,PosY + 2,PosX + 2, PosY - 2);
							}
						}
					}
				}
			}
		}
	}
}