#ifndef __COCKPIT_H
#define __COCKPIT_H


///------ Cockpit Static Art Types --
#define FRONT_FORWARD_COCKPIT		0x00000000
#define FRONT_DOWN_COCKPIT			0x00000001
#define BACK_FORWARD_COCKPIT 		0x00000002

#define FRONT_LEFT_COCKPIT			0x00000003
#define FRONT_RIGHT_COCKPIT			0x00000004

#define BACK_LEFT_COCKPIT			0x00000005
#define BACK_RIGHT_COCKPIT			0x00000006

#define ACM_FRONT_HUD				0x00000007

#define FRONT_BACK_LEFT_COCKPIT		0x00000008
#define FRONT_BACK_RIGHT_COCKPIT	0x00000009
#define BACK_BACK_LEFT_COCKPIT		0x0000000A
#define BACK_BACK_RIGHT_COCKPIT		0x0000000B // see  MAX_DIFF_COCKPITS

#define FRONT_REAR_COCKPIT			0x0000000C
#define FRONT_UP_COCKPIT			0x0000000D
#define VIRTUAL_MODEL				0x0000000E
#define FRONT_NO_COCKPIT			0x0000000F



///------  COCKPIT VIEWS
#define COCKPIT_SEAT			0xf0000000
#define COCKPIT_FRONT_SEAT		0x00000000
#define COCKPIT_BACK_SEAT		0x10000000
#define COCKPIT_VIRTUAL_SEAT	0x20000000
#define COCKPIT_NOART			0x40000000

#define COCKPIT_VIEW_PITCH		0x0f000000
#define COCKPIT_NO45			0x00000000
#define COCKPIT_45UP			0x01000000
#define COCKPIT_45DOWN			0x02000000

#define COCKPIT_ZOOM_LEFT_MPD	0x00100000
#define COCKPIT_ZOOM_CENTER_MPD	0x00200000
#define COCKPIT_ZOOM_RIGHT_MPD	0x00400000
#define COCKPIT_ZOOM_DOWN_MPD	0x00800000
#define COCKPIT_ZOOM_MPD		0x00F00000

#define COCKPIT_VIEW_HEADING	0x000fffff
#define COCKPIT_FRONT			0x00000001
#define COCKPIT_FRONT_LEFT		0x00000002
#define COCKPIT_FRONT_RIGHT		0x00000004
#define COCKPIT_LEFT			0x00000008
#define COCKPIT_RIGHT			0x00000010
#define COCKPIT_BACK_LEFT		0x00000020
#define COCKPIT_BACK_RIGHT		0x00000040
#define COCKPIT_BACK			0x00000080
#define COCKPIT_FREE			0x00000100
#define COCKPIT_PADLOCK			0x00000200

#define COCKPIT_ACM_FRONT		0x00000400
#define COCKPIT_ACM_BACK		0x00000800

#define COCKPIT_ACM_LEFT		0x00001000
#define COCKPIT_ACM_RIGHT		0x00002000

#define COCKPIT_ACM_UP			0x00004000
#define COCKPIT_ACM_DOWN		0x00008000

#define COCKPIT_ACM_FRONT_LEFT	(COCKPIT_ACM_FRONT | COCKPIT_ACM_LEFT)
#define COCKPIT_ACM_BACK_LEFT	(COCKPIT_ACM_BACK | COCKPIT_ACM_LEFT)
#define COCKPIT_ACM_FRONT_RIGHT (COCKPIT_ACM_FRONT | COCKPIT_ACM_RIGHT)
#define COCKPIT_ACM_BACK_RIGHT	(COCKPIT_ACM_BACK | COCKPIT_ACM_RIGHT)

#define COCKPIT_ACM_MODE		0x00010000
#define COCKPIT_ACM_INDEX		0x0000FC00


// Virutal Cockpit Views
#define VIRT_FREE				11
#define VIRT_PAD_LOCK			12
#define VIRT_ACM_BACK			13
#define VIRT_ACM_FRONT			14

// for misc buttons
#define MAX_MISC_BUTTONS        2

// for misc sprites
#define MAX_MISC_SPRITES 		13
#define MAX_FLIR_SPRITES 		4

#define NUM_FIRE_EXT_SWITCH_SPRITES           3
#define NUM_JET_WEAP_SELECT_SWITCH_SPRITES    5
#define NUM_JETTISON_PUSH_SPRITES             2
#define NUM_FUEL_SWITCH_SPRITES               2
#define NUM_LEFT_DOWN_SWITCH_SPRITES          2
#define NUM_RIGHT_DOWN_SWITCH_SPRITES         2
#define NUM_DOUBLE_FIRE_SPRITES               16
#define NUM_SINGLE_FIRE_SPRITES               4
#define NUM_MASTER_ARM_SPRITES                2

#define SWITCH_MIDDLE 0
#define SWITCH_DOWN   1
#define SWITCH_UP     2

#define JETTISON_OFF      0
#define JETTISON_ALL      1
#define JETTISON_COMBAT   2
#define JETTISON_AG       3
#define JETTISON_AA       4

#define JETTISON_IN       0
#define JETTISON_OUT      1

#define FIRE_EXT_OFF       0
#define FIRE_EXT_TEST      1
#define FIRE_EXT_DISCHARGE 2

#define SWITCH_HUD_DECLUTTER_OFF 0
#define SWITCH_HUD_DECLUTTER_ON  1

#define SWITCH_FUEL_LEFT_RIGHT   0
#define SWITCH_FUEL_CENTER       1

#define SWITCH_DAY   0
#define SWITCH_NIGHT  1

#define AV_NUM_HELP_WINDOW_SPRITES 4

#define AV_HELP_WIN_SPIRAL      0
#define AV_HELP_WIN_LEFT_EDGE   1
#define AV_HELP_WIN_BOTTOM_EDGE 2
#define AV_HELP_WIN_RIGHT_EDGE  3

typedef struct{
	GrBuff *Buff;
	int XOfs,YOfs;
	int XOfs2,YOfs2;
	int Width,Height;
	int ScreenX, ScreenY;
	int FramesToDraw;
}LookDownSpriteType;

typedef struct{
	GrBuff *Buff;
	int XOfs,YOfs;
	int XOfs2,YOfs2;
	int Width,Height;
}AvGenericSpriteType;

typedef struct
{
  int Left;
  int Right;
  int Top;
  int Bottom;
} SpriteBoundBoxType;

typedef struct
{
  int PushState;
  int LightOn;
} FireExtStateType;

typedef struct{

	int		UpSprX, UpSprY, UpSprX1, UpSprY1;
	int		DnSprX, DnSprY, DnSprX1, DnSprY1;
	int		SprW, SprH;
	int		ScrX, ScrY;
	int		CurrentState;		// up/down
	int		Active;
	int     IsCockpitFront;
	GrBuff  *Buff[2];
	void	(*Action)(void);
}MISCPushButtonType;


//---- Virtual Cockpit Stuff
typedef struct _VirtualCockpit
{
	ObjectHandler *frontseat;
	ObjectHandler *airplane;
	ObjectHandler *ThrottleRight;
	ObjectHandler *ThrottleLeft ;
	ObjectHandler *JoyStick;
	ObjectHandler *refueling_probe;
	ObjectHandler *CanopyReflection;
	ObjectHandler *LiftLine;


	TextureBuff	*HudLeft;
	TextureBuff	*HudRight;
	TextureBuff	*HudCenter;
	TextureBuff	*UFC;

	FPoint	HeadPosition;
	float	HeadingOffset;
	float	PitchOffset;
	BOOL	HeadMovement;

	float xdelta;
	float ydelta;	//offset from model 0,0,0
	float zdelta;

}VirtualCockpit;
extern VirtualCockpit VC;




// used for blitting misc. sprites such as needle covers, ADD overlays ...
// these sprites are not buttons and do not have any state data associated with them
typedef struct{
	int		UpSprX, UpSprY, UpSprX1, UpSprY1;
	int		SprW, SprH;
	int		ScrX, ScrY;
	int		Active;
   	GrBuff  *Buff;
   } MISCSpriteType;


// Warning light (Sprite systems)

#define OFF 0
#define ON	1

typedef struct{

	GrBuff *Buff;
	int  XOfs, YOfs;
	int  XOfs2, YOfs2;
	int  Width, Height;
	int  Clr_SprX, SprY;
	char Status;
	char DisplayType;

}CockpitType;

#define MAX_DIFF_COCKPITS		12
#define MAX_PARTS_PER_COCKPIT	43

void LoadCockpit(int Type);
void DrawDynamicCockpit(void);
void DrawStaticCockpit(void);
void StampCockpit(void);
void CleanupCurrentCockpit(void);
void DisplayWarning(int Guage, int On_Off, int UpdateStatic);
void InitCockpit(PlaneParams *P);




#define MASTER_CAUTION_BUTTON 0

// prototypes for misc buttons
void InitMISCButtons(void);
void SearchForMISCCommands(void);
void SetMISCButtonFunction(int ButtonNum, void (*NewFunction)(void));
void MISCTestFunction(void);


// Warning Panel Types

#define MAX_WARNING_PANEL_LABELS  32
#define MAX_BACK_WARNING_PANEL_LABELS 14

// warning panel label designation of Master Warning Panel
#define WARN_L_BLEED_AIR    0	  // LEFT SIDE OF PANEL
#define WARN_L_ENG_CONT     1
#define WARN_L_OIL_PRESS	2
#define WARN_L_BURNER		3
#define WARN_L_FUEL_PUMP	4
#define WARN_L_GEN			5
#define WARN_MAIN_HYD		6
#define WARN_FLT_CONT		7
#define WARN_TARGET_IR		8
#define WARN_PACS			9
#define WARN_CEN_COMP		10
#define WARN_RADAR			11
#define WARN_ADC			12
#define WARN_OXYGEN		    13
#define WARN_CHAFF			14
#define WARN_SPARE1			15
#define WARN_R_BLEED_AIR	16	 // RIGHT SIDE OF PANEL
#define WARN_R_ENG_CONT		17
#define WARN_R_OIL_PRESS	18
#define WARN_R_BURNER		19
#define WARN_R_FUEL_PUMP	20
#define WARN_R_GEN			21
#define WARN_ENER_HYD		22
#define WARN_AUTOPILOT		23
#define WARN_NAV_FLIR		24
#define WARN_GUN 			25
#define WARN_TEWS 			26
#define WARN_HVD   			27
#define WARN_FUEL_LOW 		28
#define WARN_MINIMUM  		29
#define WARN_FLARE	  		30
#define WARN_SPARE2         31

// warning panel label designation of Back Seat Warning Panel
#define BCK_WARN_L_ENG      0
#define BCK_WARN_R_ENG      1
#define BCK_WARN_L_GEN	    2
#define BCK_WARN_R_GEN		3
#define BCK_WARN_FLT_CONT   4
#define BCK_WARN_C_C 		5
#define BCK_WARN_HYD_PC1	6
#define BCK_WARN_HYD_PC2  	7
#define BCK_WARN_A_P		8
#define BCK_WARN_FUEL_LOW  	9
#define BCK_WARN_MST_ARM	10
#define BCK_WARN_CHAFF 		11
#define BCK_WARN_MIN   		12
#define BCK_WARN_FLR		13


// misc sprites
#define	 AIRSPEED_SPRITE		 0
#define	 VVI_SPRITE				 1
#define	 AOA_SPRITE				 2
#define	 UTIL_SPRITE			 3
#define	 PC1_SPRITE			   	 4
#define	 PC2_SPRITE				 5
#define	 QUESTION_SPRITE		 6
#define	 ADI_1_SPRITE			 7
#define	 ADI_2_SPRITE     	     8
#define	 UTIL__FRONT_SPRITE      9
#define	 PC1_FRONT_SPRITE	     10
#define	 PC2_FRONT_SPRITE	     11
#define  ALT_SPRITE				 12


typedef struct{

	int		SprX, SprY, SprX1, SprY1;
	int		SprW, SprH;
	int		ScrX, ScrY;	  // location of panel on screen, upper left
	GrBuff *Buff;

}WarningPanelType;


typedef struct{

	int		SprX, SprY, SprX1, SprY1;
	int		SprW, SprH;
	int		OffsetX, OffsetY; // offsets of label from top left of warning panel
   	int     Active;
	GrBuff *Buff;

}WarningPanelLabelType;


typedef struct
{
  FMatrix	 CockpitAttitude;
  RotPoint3D  CockpitP1, CockpitP2;
  FPoint CockpitRawP1;
  FPoint CockpitRawP2;
  int  CentX,CentY;
} PlayerPitchRollInfoType;

typedef struct
{
  RotPoint3D  BoxVerts[7];
  RotPoint3D  PolyVerts[7];
  RotPoint3D *pPolyVerts[7];
  int RightIndex;
  int LeftIndex;
  int NumVerts;
  int DrawFullGround;
  int DrawFullSky;
} MasterADIType;


typedef struct
{
  int NumVerts;
  int DrawFullGround;
  int DrawFullSky;
  int GroundColor;
  int SkyColor;
  RotPoint3D  BoxVerts[7];
  RotPoint3D  PolyVerts[7];
  RotPoint3D *pPolyVerts[7];
} ADIEntryType;

#define NUM_ANIM_SPRITES 5

#define AV_ANIM_SPRITE_PRIME_TARGET_AIR     0
#define AV_ANIM_SPRITE_OPTION_CHECKBOX      1
#define AV_ANIM_SPRITE_OPTION_LEFT_BUTTON   2
#define AV_ANIM_SPRITE_OPTION_RIGHT_BUTTON  3
#define AV_ANIM_SPRITE_OPTION_CANCEL_BUTTON 4

typedef struct
{
  GrBuff *Buff;
  int XOfs,YOfs;
  int XOfs2,YOfs2;
  int Width,Height;
  int AnchX, AnchY;
  int NumFrames;
  int Repeat;
  int TotalTime;    // length of the anim in milliseconds
  int TimePerFrame; // in milliseconds
}AnimSpriteType;

typedef struct
{
  AnimSpriteType *SpriteClass;
  int CurFrame;
  int Complete;
  int LastTime;
}AnimSpriteInstType;

// prototypes
void LoadWarningPanel(void);
void CleanupWarningPanel(void);
void SetWarning(int Warning);
void ClearWarning(int Warning);
void SetBackSeatWarning(int Warning);
void ClearBackSeatWarning(int Warning);
void SimulateWarningPanel(void);
void ClearWarnings();
void DrawCircleNoClip(GrBuff *Buff, int X, int Y, int Radius, int ColorRed, int Green=0, int Blue=0, int PAL8=1);
void DrawCircleClip(GrBuff *Buff, int OriginX, int OriginY, int Radius, int ColorRed, int Green=0, int Blue=0, int PAL8=1);
void DrawCircleDashed(GrBuff *Buff, int OriginX, int OriginY, int Radius, int ColorRed, int Green=0, int Blue=0, int PAL8=1 ,int Clip=0);
void ToggleRadar();
void DrawAirSpeedNeedle();
void DrawGauge(int ScrX, int ScrY, int Length, double Val, double Max, int Wrap, float StartDegree);
void DrawHalfGauge(int ScrX, int ScrY, int Length, double Val, double Max);
void DrawAOAGauge(int ScrX, int ScrY, int Length, double Val, double Max);
void DrawFuelGauge(int ScrX, int ScrY, int Length, double Val, double Max);
void TestRectClip(int Left, int Right, int Top, int Bottom, int X0, int Y0, int X1, int Y1);
void CalculatePitchAndRollLine();

void Display6DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color);
void Display5DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color);
void Display4DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color);
void Display3DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color);
void Display2DigitFont(int ScrX, int ScrY, int FontType, int Val, int Color);

int  IsPointInside(int X,int Y, int Top, int Bottom, int Left, int Right);
void CalculateMasterADI();
void CalculateADI(ADIEntryType *ADIPtr);
void DrawADI(ADIEntryType *ADIPtr, int GroundColor, int SkyColor);
void PushClipRegion();
void PopClipRegion();
void SetClipRegion(int Left, int Right, int Top, int Bottom, int MidY, int MidX);

void LoadVirtualCockpit( BOOL force_load = FALSE);
void CleanupVirutalCockpit( void );
void DrawVirtualCockpit( void );
void UpdateVirtualMpd( void );
void CheckVirtualMouse( void );
void DisplayCurrentUFCTextVirtual( void );
void ClearUFCDisplaysVirtual( void );
int  SetVirtualHudCenter(void);





void SetMasterWarning(int Warning);
void ClearMasterWarning(int Warning);
void CleanupFLIRSprites(void);
void LoadFLIRSprites(void);

void InitLookDown(void);
void CleanUpLookDownSprites(void);

// KEY COMMANDS
void ToggleMasterArmSwitch();
void JettisonGivenLoad(int State);
void ActivateAmadFireExt(void);
void ActivateLeftEngFireExt(void);
void ActivateRightEngFireExt(void);

void LoadAnimSprites(void);
void CleanupAnimSprites(void);
void SimAnimSprite(AnimSpriteInstType *Sprite);
void DrawAnimSprite(int X,int Y,AnimSpriteInstType *Sprite);
void DrawAnimSpriteFrame(int X,int Y,int Type, int FrameNum);
void DrawGenericSprite(int X,int Y,AvGenericSpriteType *Sprite);

#define AV_MAX_DAMAGE   39

#define AV_L_BLEED_AIR	0
#define AV_L_ENG_CONT	  1
#define AV_L_OIL_PRESS  2
#define AV_L_BURNER		  3
#define AV_L_FUEL_PUMP	4
#define AV_L_GEN		    5
#define AV_L_MAIN_HYD	  6
#define AV_FLT_CONT		  7
#define AV_TARGET_IR	  8
#define AV_PACS			    9
#define AV_CEN_COMP		  10
#define AV_RADAR		    11
#define AV_ADC			    12
#define AV_OXYGEN		    13
//#define AV_LO_ENGINE	  14
#define AV_L_ENGINE		  14
#define AV_R_BLEED_AIR	15
#define AV_R_ENG_CONT	  16
#define AV_R_OIL_PRESS	17
#define AV_R_BURNER		  18
#define AV_R_FUEL_PUMP	19
#define AV_R_GEN		    20
#define AV_EMER_HYD		  21
#define AV_AUTOPILOT	  22
#define AV_NAV_FLIR		  23
#define AV_GUN 			    24
#define AV_TEWS 		    25
#define AV_HUD   		    26
//#define AV_FUEL_TANKS 	28
#define AV_R_MAIN_HYD	  27
// #define AV_RO_ENGINE	  29
#define AV_R_ENGINE		  28
#define AV_NO_CHAFF		  29
#define AV_NO_FLARE 	  30
#define AV_NO_DECOY 	  31
#define AV_NO_HYD1B 	  32
#define AV_MC2 	        33
#define AV_NO_L_MDI 	  34
#define AV_NO_R_MDI 	  35
// #define AV_NO_TFLIR 	  36
#define AV_G_LIMIT   	  36
#define AV_NO_L_AMAD	  37
#define AV_NO_R_AMAD	  38

#define AV_BIT_NO_GO     0
#define AV_BIT_EMPTY     1
#define AV_BIT_NOT_READY 2
#define AV_BIT_DEGD      3
#define AV_BIT_GO        4
#define AV_BIT_OVRD      5

typedef enum
{
	CTR_ITEM,
	LI_ITEM,
	RI_ITEM,
	LM_ITEM,
	RM_ITEM,
	LO_ITEM,
	RO_ITEM,
	JETTISON_ITEM,
	JETTISON_SWITCH_ITEM,
	ARM_SAFE_ITEM,
	AA_ITEM,
	AG_ITEM,
	LEFT_FIRE_ITEM,
	FIRE_LEFT_DISCHARGE_ITEM,
	RIGHT_FIRE_ITEM,
	FIRE_RIGHT_DISCHARGE_ITEM,
	LANDING_GEAR_ITEM,
	NAVFLIR_ITEM,
	HUD_COLOR_ITEM,
	HOOK_ITEM,
	BINGO_UP_ITEM,
	BINGO_DOWN_ITEM ,
	HSEL_DOWN_ITEM,
	HSEL_UP_ITEM,
	CSEL_DOWN_ITEM,
	CSEL_UP_ITEM,
	EMIS_ITEM,
	MASTER_CAUTION_ITEM,
	ALT_ITEM,
	SYM_REJECT_ITEM,
	COLL_LIGHT_ITEM,
	FORM_LIGHT_ITEM,
	REFUEL_ITEM,
	FLAPS_ITEM,
	APU_FIRE_ITEM
} Cpit_Item_EnumType;

typedef struct
{
	int Var;
	int State;
	int Type;
	void (*CpitClickFunc)(int *State);
} CpitIType;


typedef struct
{
	int JettSwitchState;
	int JettSwitchDir;
	int FireLeftPanelUp;
	int FireRightPanelUp;
	int NavFlirDir;
	int NavFlirSwitchState;
	int HudDeclutterDir;
} CpitItemType;

// new f18 code
typedef struct
{
	int MasterCautionOn;
	int Damage[AV_MAX_DAMAGE];
	Timer MasterCautionTimer;
	Timer MasterCautionShowBitTimer;
	int DecoyDeployVar;
	int DecoyOnVar;
	int LaserVar;
	int AIVar;
	int SAMVar;
	int AAAVar;
	int LBleedVar;
	int RBleedVar;
	int StbyVar;
	int RecVar;
	int XmitVar;
	int AutoVar;
	int ShootVar;
	int LockVar;
	int HSelClicked;
	int CSelClicked;
	int HSelVar;
	int CSelVar;
	int SimJettOn;
	Timer JettTimer;

	int FireRightVar;
	int FireLeftVar;
	int RightApuVar;
	int LeftApuVar;
	int RightExtinguishOn;
	int LeftExtinguishOn;
	int CanSimFireRight;
	int CanSimFireLeft;
	int LeftFireReadyVar;
	int RightFireReadyVar;
	int SpeedBrakeVar;
	int BrakeVar;
	int APVar;
	int LBar;
	int DispVar;

	int NoseVar;
	int LeftVar;
	int RightVar;
	int FlapsVar;
	int HalfVar;
	int FullVar;

	CpitItemType CpitItem;
} CPitType;


void InitF18Cockpit();
void CheckDamageForMasterCaution();
void SetCockpitVars();

// pushbuttons for jettison
#define AV_CTR_LIGHT 0
#define AV_LI_LIGHT  1
#define AV_RI_LIGHT  2
#define AV_LM_LIGHT  3
#define AV_RM_LIGHT  4
#define AV_LO_LIGHT  5
#define AV_RO_LIGHT  6

#define AV_JET_EMERGENCY  0
#define AV_JET_OFF        1
#define AV_JET_LEFT_FUSE  2
#define AV_JET_RIGHT_FUSE 3
#define AV_JET_STORES     4

void SimJett(void);

void CockpitItemClick(int Id);
int GetCockpitItemState(int Id);
void GetCockpitItemVars( char *pVars );

void SetMasterModeLights(int Mode);
void CheckDamageForMasterCaution();

void SetJettFromKeyboard(int Mode);

void SetCockpitItemState(int Id, int State);

void MasterCautionKeypress(void);





#endif