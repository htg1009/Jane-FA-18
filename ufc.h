#ifndef __UFC_H
#define __UFC_H

#define MAX_UFC_CONTROLS	35
#define MAX_UPC_CONTROL_STATES	5
// for misc buttons
#define MAX_MISC_CONTROLS   1

#define PUSH	1
#define RELEASE	0

#define LEFT_JUSTIFY	0
#define CENTERED		1
#define RIGHT_JUSTIFY	2

typedef struct{

	int		UpSprX, UpSprY, UpSprX1, UpSprY1;
	int		DnSprX, DnSprY, DnSprX1, DnSprY1;
	int		SprW, SprH;
	int		ScrX, ScrY;
	int		CurrentState;		// up/down
	int		Active;
	GrBuff  *Buff[2];
	void	(*Action)(void);

}UFCPushButtonType;

typedef struct{

	int		XOfs[3];
	int		YOfs;
	char	*Text;
	int		JustifyType;

}UFCLedType;

#define NAV_STR		0
#define TCN_STR		1
#define TGT_STR		2

#define NORMAL 1

#define AUTO 2
#define ATT	 1
#define HDG  2

#define NO  0
#define YES 1

#define NAV 0
#define TCN 1

#define BARO 0
#define RDR	 1

#define FUEL	0
#define BRG_RNG	1

#define ETA		0
#define	ETE		1

#define LOCAL	0
#define ZULU	1

#define TOA		0
#define	TOT		1

#define NONE		0
#define PULL_UP		1
#define OBSTACLE	2

#define SPOOL_OFF  0
#define SPOOL_DOWN 1
#define SPOOL_UP   2

typedef struct{

	int		MasterMode;
	int		AGSubMode;
	int		AASubMode;
	int		AASearchMode;
	int		HudInCmd;
	int		TRelCountDown;

	int		UFCClear;

	int		LowAltWarningLimit;
	int		LawWarningStatus;
	int		LawWithinConstraints;
	int		ILSStatus;
	int		TFStatus;
	int		TFWarning;
	int		TFHoldRdrAlt;

	int		IFFStatus;
	int		NavFlirStatus;
	int		InMainMenu;
	int		InDataMenu;

	int		APStatus;					// AutoPilot modes - flags
	int		APStrModeActive;
	int		APAltHoldActive;
	int		APStrMode;
	int		APAltHoldMode;

	int		HudRAltSelect;				// Flags for HUD display data
	int		HudTasSelect;
	int		HudThrottleSelect;

	int		DataCurrSPDistance;			// Flags stuff
	int		DataCurrSPTime;

	int		DataLkAheadSPDistance;		// Flags stuff
	int		DataLkAheadSPTime;

	int		DataCurrSP;					// Current Waypoint ie. 2
	float	DataCurrSPBrg;				// Current Waypoint bearing relative to us
	float	DataCurrSPMilBrg;			// CUrrent Waypoint bearing in Military compass (clockwise)


	float	DataCurrSPNavDist;			// Current Waypoint Distance in Knots
	float	DataCurrSPDistFeet;			// Current Waypoint Distance in Feet
	int		DataLkAheadSPBrg;
	float	DataLkAheadSPNavDist;
	float	DataLkAheadDistFeet;

	int		DataCurrSPEteHr;			// Below - Different times for various modes
	int		DataCurrSPEteMin;			//         relative to steering points
	int		DataCurrSPEteSec;

	int		DataCurrSPEtaHr;
	int		DataCurrSPEtaMin;
	int		DataCurrSPEtaSec;

	int		DataLkAheadSPToaHr;
	int		DataLkAheadSPToaMin;
	int		DataLkAheadSPToaSec;

	int		DataLkAheadSPTotHr;
	int		DataLkAheadSPTotMin;
	int		DataLkAheadSPTotSec;

	int		DataCurrTimeMode;			// Realtime time mode and data

	int		DataCurrTimeLocalHr;
	int		DataCurrTimeLocalMin;
	int		DataCurrTimeLocalSec;

	int		DataCurrTimeZuluHr;
	int		DataCurrTimeZuluMin;
	int		DataCurrTimeZuluSec;

	float	DataCurrTGTRelBrg;
	float	DataCurrTGTAbsBrg;
	float   DataCurrTGTDistFeet;
	float	DataCurrTGTNavDist;

	int		DataCurrTGTDistTime;
	int		DataCurrTGTTimeHr;
	int		DataCurrTGTTimeMin;
	int 	DataCurrTGTTimeSec;

	int		EMISState;

	int     MasterArmState;

	int iCurrLKAhead;
	MBWayPoints *pWPLKAhead;
	PlaneParams *pWPPlane;

	void (*ActiveMenuExecuteFunction)(void);

	HUDLadderType	HUDLadder[NUM_PITCH_LADDERS];

	TACANType		*CurrentTacanPtr;
	char			CurrentTacanStation[5];
	char			CurrentTacanString[20];
 	FPoint		 	TacanPos;
 	FPoint		 	TacanTrackPos;
	int				TCNSubMenu;

	float			TacanBearing;
	float			TacanRelBearing;
	float			TacanMilBearing;
	float			TacanNavDist;
	float			TacanDistFeet;
	float			TacanTrackDistFeet;

	int				TacanEte;

	int				TacanEtaHr;
	int				TacanEtaMin;
	int				TacanEtaSec;

	int				TacanEteHr ;
	int				TacanEteMin;
	int				TacanEteSec;
	int				SteeringMode;

	float			StatusMaxG;
	float			StatusMinG;
	float			StatusMaxTAS;
	float			StatusMaxAlt;
	float			StatusMinAlt;
	float			StatusTotalFuelUsed;
	int				StatusSuccessfulLanding;
	int				TakeOffTime;

	float			LeftFuelFlowRate;		// lb./min.
	float			RightFuelFlowRate;		// lb./min.
	int             LastTimeFuelFlowCalc;
	int             LastTimeRightFuelFlowCalc;
	float           GoalFuelFlow;
	float           CurFuelFlow;

	float 	        GoalLeftFuelFlow;
	float 	        GoalRightFuelFlow;
	float           CurLeftFuelFlow;
	float           CurRightFuelFlow;

	float           Psi;
	float           PsiAdjust;

	float           OffTemp;
	float           MilMaxTemp;

	int             LeftSpoolState;
	int             RightSpoolState;
	int             LeftSpoolStartTime;
	int             RightSpoolStartTime;

	int             RightHeatUpWarningGiven;
	int             LeftHeatUpWarningGiven;

	int             RightAmadHeatUpWarningGiven;
	int             LeftAmadHeatUpWarningGiven;

	int				TireSqueelOneShot;
	int				TireOneShotEnabled;
	int				OnGroundLastFrame;

	int				DegRoll;
	int				DegPitch;

	int				DataCurrAltitude;
	int				DataCurrSpeed;

}UFCType;

void UFCTestFunction(void);
void CleanupUFCSprites(void);
void LoadUFCSprites(void);
void DetectCockpitInteractions(void);
void InitUFCMainMenu(void);
void InitUFCDataMenu(void);
void DoUFCMainLaw(void);
void DoUFCMainTcn(void);
void DoUFCMainIff(void);
void DoUFCMainTf(void);
void DoUFCMainIls(void);
void DoUFCMainNavFlir(void);
void DoUFCMainHudData(void);
void DoUFCMainAutoPilot(void);
void IncreaseLaw(void);
void DecreaseLaw(void);
void LawOff(void);
void AutoPilotOnOff(void);
void APToggleStrMode(void);
void APToggleAltHoldMode(void);
void APToggleStrModeActive(void);
void APToggleAltHoldActive(void);
void DoUFCMainData(PlaneParams *P);
void DisplayCurrentUFCText(void);
void UFCDisplayAutoMode(PlaneParams *P, int Pos, int Justify);
void SetAutoPilotMode(void);
void AutoPilotOff(PlaneParams *P);
void ToggleHudRAlt(void);
void ToggleHudTas(void);
void SetUFCButtonFunction(int ButtonNum, void (*NewFunction)(void), char *NewText, int Justify);
void UFCAdvanceWaypointVisual(void);

void DoUFCDataSP(void);
void DoUFCMainStrMode(void);
void DoUFCDataSPVisual(void);
void DoUFCMainStrModeVisual(void);

void ToggleDataCurrSPDistance(void);
void ToggleDataCurrSPTime(void);
void ToggleDataCurrTimeMode(void);

void ToggleDataLkAheadSPDistance(void);
void ToggleDataLkAheadSPTime(void);
void ToggleDataLkAheadTimeMode(void);

void DoUFCDataLkAheadSP(void);
void DoUFCDataLkAheadSPVisual(void);
void DisplayLkAheadSPDistance(void);
void DisplayLkAheadSPTime(void);

void ComputeLkAheadNumbers(void);
void UFCToggleEMIS(void);

void UFCSetAAMode(void);
void UFCSetAGMode(void);
void UFCSetNAVMode(void);
void UFCSetINSTMode(void);

void UFCCalcCentralComputerData(void);

void InitUFC(void);

void StepThroughMasterModes(void);
void TFAutoPilotOff(PlaneParams *P);
void SetDefaultTacanStation(void);
void ToggleTacanTime(void);
void ToggleMasterArm(void);
void TacanNext(void);
void TacanNextWClear(void);
void TacanPrevious(void);
void CheckCurrentTacanStillValid(int type, void *ptr);
void ClearCurrentTacan(void);

void DisplayStrMode(void);

void SetSpecificTacanStation(TACANType *NewPtr);



#endif