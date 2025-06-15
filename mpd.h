#ifndef __MPD_H
#define __MPD_H
#endif


#define MAX_MPD_BUTTONS	20
#define MAX_MPDS        9
#define MAX_UNIQUE_MPDS 7
#define MAX_MODES       16


#define	MPD_BUTTON_1 0
#define	MPD_BUTTON_2 1
#define	MPD_BUTTON_3 2
#define	MPD_BUTTON_4 3
#define	MPD_BUTTON_5 4
#define	MPD_BUTTON_6 5
#define	MPD_BUTTON_7 6
#define	MPD_BUTTON_8 7


#define MPD0 0
#define MPD1 1
#define MPD2 2
#define MPD3 3
#define MPD4 4
#define MPD5 5
#define MPD6 6


#define FRONT_RIGHT_MPD       0
#define FRONT_LEFT_MPD        1
#define FRONT_DOWN_RIGHT_MPD  2
#define FRONT_DOWN_LEFT_MPD   3
#define FRONT_DOWN_CENTER_MPD 8
#define BACK_RIGHT_MPD        4
#define	BACK_CENTER_RIGHT_MPD 5
#define	BACK_CENTER_LEFT_MPD  6
#define BACK_LEFT_MPD         7

#define MAX_MPD_DAMAGE        10

#define MPD_DAMAGE_L_GEN      0
#define MPD_DAMAGE_TARGET_IR  1
#define MPD_DAMAGE_PACS       2
#define MPD_DAMAGE_CEN_COMP   3
#define MPD_DAMAGE_RADAR      4
#define MPD_DAMAGE_ADC        5
#define MPD_DAMAGE_NAV_FLIR   6
#define MPD_DAMAGE_TEWS       7
#define MPD_DAMAGE_HUD        8
#define MPD_DAMAGE_R_GEN      9


#define ADI_MODE	       0
#define HSI_MODE	       1
#define UFC_MODE	       2
#define MAIN_MENU_MODE   3
#define TGT_IR_MODE	     4
#define TEWS_MODE	       5
#define HUD_MODE	       6
#define ENG_MODE	       7
#define NONE_MODE        8
#define RADAR_MODE       9
#define STORES_MODE      10
#define BIT_MODE         11
#define ACL_MODE         12
#define FPAS_MODE        13
#define CHECKLIST_MODE   14
#define FUEL_MODE        15
#define EASY_CAM_MODE    16
#define EASY_SENSOR_MODE 17
#define SA_MODE          18
#define EDF_MODE         19
#define NAVFLIR_MODE     20


// COLOR CATEGORIES FOR MPDS

#define MAX_MPD_COLOR_TYPES 10

#define COLOR_NORM_SOFT                   0
#define COLOR_NORM_BOLD                   1
#define COLOR_B_3                         2
#define COLOR_TEWS_ARROW_NO_ALERT         3
#define COLOR_TEWS_ARROW_ALERT            4
#define COLOR_BLACK_HOT                   5
#define COLOR_B_6                         6
#define COLOR_B_7                         7
#define COLOR_B_8                         8
#define COLOR_B_9                         9


#define LEFT   0
#define TOP	   1
#define RIGHT  2
#define BOTTOM 3

#define FT_PER_MILE  6076.115


typedef struct
{
  int ScrX;
  int ScrY;
} TargetButtonClickInfoType;

typedef struct
{
  int TargetSelected;
  int TargetId;
  int ScrX;
  int ScrY;
  int MpdDetectNum;
} AATargetSelectInfoType;

typedef struct
{
  int TopX;
  int TopY;
  int TopWidth;
  int TopHeight;
  int MidX;
  int MidY;
  int MidWidth;
  int MidHeight;
  int BotX;
  int BotY;
  int BotWidth;
  int BotHeight;
} MpdClearType;

typedef struct
{
	int		UpSprX, UpSprY, UpSprX1, UpSprY1;
	int		DwnSprX, DwnSprY, DwnSprX1, DwnSprY1;
	int		LUpSprX,  LUpSprY;
	int		LDwnSprX, LDwnSprY;
	int		SprW, SprH;
   	GrBuff  *Buff[2][2];
} MpdButtonSpriteType;

typedef struct
{
  int ScrX;
  int ScrY;
  int Width;
  int Height;
  int State;   // pressed or released?
  int Valid; // is this button a valid button, not all buttons are defined for all MPDs
  int FramesToDraw;
  int ButtonDirection; // up down left right
} MpdButtonType;


typedef struct
{
  int Active;
  int CurButton;
  int ModeList[4];      // can only scroll through 4 Modes when programming
} MpdProgramModeType;

typedef struct
{
  int Cockpit;
 // int Mode;
  int ScrX;	  // where is the box (for mouse detects) on the screen
  int ScrY;
  int Height;
  int Width;
  int CenterX;
  int CenterY;
  int TransX; // for translating due to virtual modes.
  int TransY; // for translating due to virtual modes.
  MpdButtonType MpdButtons[MAX_MPD_BUTTONS];
} MpdBoxType;



typedef struct
{
 // up and down sprites for top bottom left right
 //	"ButtonDirection" will be used as an index into this array"
 MpdButtonSpriteType ButtonSprites[4];
 MpdBoxType MpdList[MAX_MPDS];
 int Active;
 int CurrentMpd;		// current Mpd mouse is pointing at
 int CurrentMpdButton;	// current Mpd button being pressed
} MpdType;


typedef struct
{
  int TextColor;
  int ClearColor;
  int AdiGroundColor;
  int AdiSkyColor;
  int LineColor;
} ColorInfoType;

typedef struct
{
  int Range;
  int Mode;
} HsiInfoType;

typedef struct
{
  int ClickNum;
  int StartTime;
} MMRadarErrorType;

typedef struct
{
  int TextChangeCountDown;
  MMRadarErrorType MMRadarError;
} AARadarInfoType;

typedef struct
{
  int StartTime;
  int ModeToUse;
} WeapInfoType;

typedef struct
{
  int StartTime;
} TgtIrInfoType;


typedef struct
{
  int TextChangeCountDown;
  int CurMode;
  int SelectMode;
  int MapSubMode;
  int ErrorCode;
  int ScanOn;
  int MapOn;
  int FreezeErrorStartTime;
  int EmisErrorStartTime;
  int HrmErrorStartTime;
  int HrmErrorCode;
  MMRadarErrorType MMRadarError;
} AGRadarInfoType;

typedef struct
{
  int Mode;
  AARadarInfoType AAInfo;
  AGRadarInfoType AGInfo;
  MpdProgramModeType ProgModeInfo;
  ColorInfoType ColorInfo;
  HsiInfoType HsiInfo;
  WeapInfoType WeapInfo;
  TgtIrInfoType FlirInfo;
  int IsMono;
} MpdModeInfoType;


typedef struct
{
  int XOff;
  int YOff;
} ButtonOffsetType;


typedef struct
{
  int Left;
  int Right;
  int Top;
  int Bottom;
  int Width;
  int Height;
} MpdViewBoxType;

typedef struct
{
  int ButtonPressed;
  int MpdNum;
  int ButtonNum;
} ButtonClickInfoType;






#define HST_SPHERE		0x01
#define HST_RECTANGLE	0x02
#define HST_POLY		HST_RECTANGLE

typedef struct _hspot
{
	char	Type;

	FPoint	Center;
	float	Radius;

	FPoint UL;
	FPoint LR;
	FPoint Normal;

	float	tu;			// mouse cordinates in  MPD
	float	tv;

	int		States;		//num states
	int		Var;
	int		MpdNum;

	GrBuffPolygon *Poly;
}hspot;














// prototypes
void SetMpdButtonSprites(MpdButtonSpriteType *ButtonSprites);
void SetMpdInfo();
void CalcMonoPalette();
void InitMpdViewBoxInfo(void);
void ClearMpd(int MpdDetectNum);
int ShouldCalculateHudCamera();
int MapMpdDetectToMpdMode(int MpdDetectNum);
void InitMpd();
void ProcessMpdButtonPress(int MpdNum, int MpdButtonNum);
void GrabButtonPress(int MpdDetectNum,int *ButtonNum, int *Pressed);
int CheckMpd(int MouseX, int MouseY, int CurrentCockpit);
void DrawTextAtLocation(int ScrX, int ScrY,  char *pMpdString, int ColorRed, int Green=0, int Blue=0, int Pal8=1);
void RightJustifyTextAtLocation(int ScrX, int ScrY,  char *pMpdString, int ColorRed, int Green=0, int Blue=0);
void DrawTextAtCenter(int ScrX, int ScrY, int Width, int Height, char *pMpdString, int ColorRed, int Green=0, int Blue=0);
void DrawWordAtButton(int MpdDetectNum,int ButtonNum, char *Word, int DrawBox, int ColorRed, int Green=0, int Blue=0);
void DrawVerticalWordAtLocation(int ScrX, int ScrY,  char *pMpdString, int BoxIt, int ColorRed, int Green=0, int Blue=0);
void DrawBox(GrBuff *Buff, int X1,int Y1,int Width, int Height, int ColorRed, int Green=0, int Blue=0);
void DrawBoxAroundWord(char *Word, int ScrX, int ScrY, int ColorRed, int Green=0, int Blue=0);
void DrawMenuSelections(int MpdDetectNum, int ColorRed, int Green=0, int Blue=0);
void CalculateHUDMpd(int Left, int Right, int Top, int Bottom);
void DrawTews(int XOfs,int  YOfs,int  Width,int Height);
void DrawHudCamera(GrBuff *src, GrBuff *dest);
void CalcMonoCamera(GrBuff *src, GrBuff *dest);
void DrawProgramChoices(int MpdNum, int ScrX, int ScrY, int ColorRed, int Green=0, int Blue=0);
void ProgramMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void DrawAaArm(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void DrawAgArm(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void AaArmMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void AgArmMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
float NormDegree(float Angle);
void AngleDiff(float Direction, float GoalDir, float *AngleDiff, int *Dir);
void DrawTacan(int ScrX, int ScrY, int ColorRed, int Green=0, int Blue=0);
void DrawWaypoint(int ScrX, int ScrY, int ColorRed, int Green=0, int Blue=0);
void DrawDot(int ScrX, int ScrY, int LineColorRed, int Green=0, int Blue=0);
void DrawErrorDots(int CenterX, int CenterY, float Direction, int LineColorRed, int Green=0, int Blue=0);
void DrawBearingNeedle(int CenterX, int CenterY, float Direction, float GoalDirection, int LineColorRed, int Green=0, int Blue=0);
void DrawPlaneSymbol(int CenterX, int CenterY, int LineColorRed, int Green=0, int Blue=0);
void DrawHsi(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void HsiModeF18(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void TsdMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void TgtIrMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void TewsMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void AgRadarMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void AaRadarMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void DrawEng(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void EngMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void WpnMode(int MpdNum, int MpdDetectNum, int Left, int Top, int Width, int Height);
void DrawMpd(int MpdNum, int MpdDetectNum);
void CheckHUDMode();
void DrawMpds(int CurrentCockpit);
void LoadMpdButtonSprites(void);
void CleanupMpdButtonSprites(void);
void ReleaseMpdButton();
void ClearMpd(int MpdDetectNum);
int AreMpdsInGivenMode(int Mode);
int IsRadarModeOn(int Mode);
void CleanupMpdCornerSprites(void);


void DrawCircleDebug2(int XOfs, int YOfs, int Test);
void FDrawCircleDebug2(int XOfs, int YOfs, double Test);


void LoadPlayerWeaponInfo(void);
void Button1CPACSUpdate(void);
void Button2CPACSUpdate(void);
void SrmSelect(void);
void MrmSelect(void);
void MissleReject(void);
void GunSelect(void);

int IsAGWeaponSelected(void);
void DoReleaseSim(int CalcInterval);

void InitTewsInfo(void);
void InitTsd();

void ProcessEnemyRadarActivity(void);

void PlayerDropChaff(void);

void InitAGArm(void);

void ActivateVirtualMpds(void);
void DeActivateVirtualMpds(void);

void ToggleVirtualMpds(void);

int IsAGMissleAvailableOnGivenSide(int Type, int Side);
int IsWeaponGBUType(int Station);
int IsWeaponMaverickType(int Station);

void ToggleAim9Scan(void);

void CalcMonoCamera(GrBuff *src, GrBuff *dest);

void CheckMpdDamage(void);

void EasyModeChangeWeapon(void);
void CleanupMiscMpdSprites(void);

void InitJettisonInfo(void);

void TewsIncRange(void);
void TewsDecRange(void);
void HsiIncRange(void);
void HsiDecRange(void);
void TsdDecRange(void);
void TsdIncRange(void);

//void InitF18LoadoutInfo(void);

void AGArmSelectStation(int Station);
void StepThroughBombMode(void);
void ToggleGunRate(void);
void HsiSelectNAV(void);
void StepThroughTewsCmdMode(void);
void HsiIncRange();
void HsiDecRange();
void HsiSelectTacan();
void HsiSelectNAV();

void CleanupMpdInfo();

void DrawBullsEye(int ScrX, int ScrY, int ColorRed, int Green=0, int Blue=0);

void SimTewsBlink();

//int GetCurrentMouseBufferLocation(int *MpdNum,float *X,float *Y);

void InitVCInteractions( void );