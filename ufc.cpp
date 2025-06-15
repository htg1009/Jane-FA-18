#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"

char UFCtext[32];  // public message to line 6
char UFCStr1[20];  // used for low altitude warning value
char UFCStr2[20];  // used for law warning limit
char UFCStr3[20];  // used for str waypoint
char UFCStr4[20];  // used for hud tas
char UFCStr5[20];  // used for hud rdr alt
char UFCStr6[20];  // data SP
char UFCStr7[20];  // data curr SP brg_RNG
char UFCStr8[20];  // data curr SP brg_RNG
char UFCStr9[20];  // data curr time (local or zulu)
char UFCStr10[20];  // data curr time (local or zulu)
char UFCStr11[20];  // data curr time (local or zulu)
char UFCStr12[20];  // data curr time (local or zulu)
char UFCStr13[20];  // current tacan station
char UFCStr14[20];  // current tacan time
char UFCStr15[20];  // current tacan range/bearing
char UFCStr16[20];  // throttle percentage
char UFCStr17[20];  // lookahead fuel left
char UFCStr18[20];  // lookahead fuel left
char UFCStr19[20];  // sp alt
char UFCStr20[20];  // sp speed

int  TGTLatch = 0;
extern int NoseTrimOffset;
extern int AileronTrimOffset;
extern int RudderTrimOffset;
extern CPitType Cpit;
extern void DoCNIEW();

extern AvionicsType Av;

//***********************************************************************************************************************************
void InitUFC(void)
{
	UFC.TRelCountDown =						0;
	UFC.HudInCmd =				  			0;
	UFC.AGSubMode =							CDIP_AG;
	UFC.AASubMode =							AA_GUN_FNL_GDS;
	UFC.AASearchMode =						SEARCH_MODE;

	UFC.MasterMode =						NAV_MODE;
	UFC.UFCClear =							0;
	UFC.LowAltWarningLimit = 				500;
	UFC.LawWarningStatus = 					ON;
	UFC.LawWithinConstraints =				YES;
	UFC.ILSStatus = 						OFF;
	UFC.TFStatus = 							OFF;
//	UFC.IFFStatus = 						NORMAL;
	UFC.NavFlirStatus = 					OFF;
	UFC.InMainMenu = 						1;
	UFC.InDataMenu = 						1;
	UFC.APStatus = 							OFF;
	UFC.APStrModeActive = 					NO;
	UFC.APAltHoldActive = 					NO;
	UFC.APStrMode = 						NAV;
	UFC.APAltHoldMode = 					BARO;
	UFC.HudRAltSelect = 					YES;
	UFC.HudTasSelect = 						YES;
	UFC.HudThrottleSelect =					NO;

	UFC.DataCurrSPDistance = 				BRG_RNG;
	UFC.DataCurrSPTime = 					ETE;
	UFC.DataLkAheadSPDistance = 			BRG_RNG;
	UFC.DataLkAheadSPTime = 				TOA;
	UFC.DataCurrSPNavDist = 				0.0;
	UFC.DataLkAheadSPNavDist = 				0.0;
	UFC.DataLkAheadDistFeet = 				0.0;
	UFC.DataCurrSPEteHr = 					0;
	UFC.DataCurrSPEteMin = 					0;
	UFC.DataCurrSPEteSec = 					0;
	UFC.DataCurrSPEtaHr = 					0;
	UFC.DataCurrSPEtaMin = 					0;
	UFC.DataCurrSPEtaSec = 					0;
	UFC.DataLkAheadSPToaHr = 				0;
	UFC.DataLkAheadSPToaMin = 				0;
	UFC.DataLkAheadSPToaSec = 				0;
	UFC.DataLkAheadSPTotHr = 				0;
	UFC.DataLkAheadSPTotMin = 				0;
	UFC.DataLkAheadSPTotSec = 				0;
	UFC.DataCurrSPBrg = 					0;
	UFC.DataLkAheadSPBrg = 					0;
	UFC.DataCurrTimeMode = 					0;
	UFC.DataCurrTimeLocalHr = 				0;
	UFC.DataCurrTimeLocalMin = 				0;
	UFC.DataCurrTimeLocalSec = 				0;
	UFC.DataCurrTimeZuluHr = 				0;
	UFC.DataCurrTimeZuluMin = 				0;
	UFC.DataCurrTimeZuluSec = 				0;
	UFC.EMISState = 						0;
	UFC.iCurrLKAhead = 						-1;
	UFC.pWPLKAhead = 						NULL;
	UFC.pWPPlane = 							NULL;
	UFC.ActiveMenuExecuteFunction =			NULL;
	UFC.DataCurrSP =						0;
	UFC.DataCurrSPDistFeet =				0.0;

	UFC.DataCurrTGTAbsBrg =					0.0;
	UFC.DataCurrTGTRelBrg = 				0.0;
	UFC.DataCurrTGTDistFeet = 				0.0;
	UFC.DataCurrTGTNavDist = 				0.0;
	UFC.DataCurrTGTTimeHr =  				0;
	UFC.DataCurrTGTTimeMin =  				0;
	UFC.DataCurrTGTTimeSec =  				0;
    UFC.MasterArmState     =                ON;
	UFC.DataCurrTGTDistTime =  				0;
	UFC.CurrentTacanPtr =					NULL;
	UFC.TCNSubMenu =						0;
	UFC.TacanEte =							ETE;

	UFC.TacanBearing =						0.0;
	UFC.TacanDistFeet =						0.0;
	UFC.TacanNavDist = 						0.0;

	UFC.TacanEteHr =						0;
	UFC.TacanEteMin = 						0;
	UFC.TacanEteSec = 						0;

	UFC.TacanEtaHr =  						0;
	UFC.TacanEtaMin = 						0;
	UFC.TacanEtaSec = 						0;

	UFC.SteeringMode =						NAV_STR;

	UFC.StatusMaxG =						0.0;
	UFC.StatusMinG = 						1.0;
	UFC.StatusMaxTAS = 						0.0;
	UFC.StatusMaxAlt =						0.0;
	UFC.StatusMinAlt = 						100000.0;
	UFC.StatusTotalFuelUsed =				0.0;

	UFC.StatusSuccessfulLanding =			0;
	UFC.TakeOffTime =						0;

	UFC.TireSqueelOneShot =					0;
	UFC.TireOneShotEnabled =				0;
	UFC.OnGroundLastFrame =					0;

	UFC.DegRoll =							0;
	UFC.DegPitch = 							0;

	UFC.DataCurrAltitude =					0;
	UFC.DataCurrSpeed =						0;

	UFC.LastTimeFuelFlowCalc   =            0;
	UFC.LastTimeRightFuelFlowCalc   =       0;

	UFC.GoalFuelFlow           =            -1;

	UFC.GoalLeftFuelFlow;
	UFC.GoalRightFuelFlow;
	UFC.CurLeftFuelFlow;
	UFC.CurRightFuelFlow;

	UFC.LeftSpoolState = SPOOL_OFF;
	UFC.RightSpoolState = SPOOL_OFF;
    UFC.LeftSpoolStartTime = -1;
    UFC.RightSpoolStartTime = -1;

	UFC.Psi                    =            15 + (rand()%50);
	UFC.PsiAdjust              =            0;

	UFC.OffTemp                =            10  + (rand()%10);
	UFC.MilMaxTemp             =            925 + (rand()%9);

	UFC.RightHeatUpWarningGiven = FALSE;
	UFC.LeftHeatUpWarningGiven  = FALSE;

	UFC.RightAmadHeatUpWarningGiven = FALSE;
	UFC.LeftAmadHeatUpWarningGiven  = FALSE;

	CurrMasterModeIndex = 0;

	if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
	{
		UFC.IFFStatus = AUTO;
	    SetIff(NORMAL);
	}
	else
	{
		UFC.IFFStatus = NORMAL;
	    SetIff(AUTO);
	}
	NoseTrimOffset = 0;
	AileronTrimOffset = 0;
	RudderTrimOffset = 0;
}
//***********************************************************************************************************************************
void ClearUFCDisplays(void)
{
	GrFillRectNoClip(GrBuffFor3D, 253+CockXOfs, 279+CockYOfs,130,   8, 1);
	GrFillRectNoClip(GrBuffFor3D, 253+CockXOfs, 295+CockYOfs,130,   8, 1);
	GrFillRectNoClip(GrBuffFor3D, 254+CockXOfs, 309+CockYOfs,130-1, 8, 1);
	GrFillRectNoClip(GrBuffFor3D, 255+CockXOfs, 323+CockYOfs,130-3, 8, 1);
	GrFillRectNoClip(GrBuffFor3D, 255+CockXOfs, 336+CockYOfs,130-3, 8, 1);
	GrFillRectNoClip(GrBuffFor3D, 256+CockXOfs, 349+CockYOfs,130-5, 8, 1);
}
//***********************************************************************************************************************************
void DoCasualAutoPilot(void)
{
	AutoPilotOnOff();
}
//***********************************************************************************************************************************
void DisplayCurrentUFCText(void)
{
	if (CurrentView & COCKPIT_VIRTUAL_SEAT)
	{
		ClearUFCDisplaysVirtual();
		DisplayCurrentUFCTextVirtual();
		return;
	}

	if ((CurrentCockpit == FRONT_FORWARD_COCKPIT) ||
		(CurrentCockpit == FRONT_DOWN_COCKPIT)  )
	{
		LedPtr = &UFCLed[0];
		LastLedPtr = &UFCLed[9];

		if (UFC.UFCClear)
		{
			ClearUFCDisplays();
			UFC.UFCClear--;
		}

		if (UFC.ActiveMenuExecuteFunction != NULL)
			UFC.ActiveMenuExecuteFunction();

		while (LedPtr <= LastLedPtr)
		{
			int Len = GrStrLen(LedPtr->Text, LgLEDFont);

			if (LedPtr->JustifyType == CENTERED)
				GrDrawString(GrBuffFor3D, LgLEDFont, LedPtr->XOfs[LedPtr->JustifyType]-(Len/2)+CockXOfs,
													 //LedPtr->YOfs-LgLEDFont->iMaxFontHeight+4+CockYOfs,
													 LedPtr->YOfs-LgLEDFont->iMaxFontHeight+9+CockYOfs,
													 LedPtr->Text, 3);

			else if (LedPtr->JustifyType == LEFT_JUSTIFY)
				GrDrawString(GrBuffFor3D, LgLEDFont, LedPtr->XOfs[LedPtr->JustifyType]+CockXOfs,
				                                     //LedPtr->YOfs-LgLEDFont->iMaxFontHeight+4+CockYOfs,
				                                     LedPtr->YOfs-LgLEDFont->iMaxFontHeight+9+CockYOfs,
													 LedPtr->Text, 3);

			else if (LedPtr->JustifyType == RIGHT_JUSTIFY)
				GrDrawString(GrBuffFor3D, LgLEDFont, LedPtr->XOfs[LedPtr->JustifyType]-Len+CockXOfs,
													 //LedPtr->YOfs-LgLEDFont->iMaxFontHeight+4+CockYOfs,
													 LedPtr->YOfs-LgLEDFont->iMaxFontHeight+9+CockYOfs,
													 LedPtr->Text, 3);
			LedPtr++;
		}
//		GrDrawString( GrBuffFor3D, LgLEDFont, 258+CockXOfs, 350+CockYOfs, UFCtext, 3);
		GrDrawString( GrBuffFor3D, LgLEDFont, 295+CockXOfs-14, 350+CockYOfs-2, UFCtext, 3);
	}
}
//***********************************************************************************************************************************
void UFCToggleEMIS(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;
	UFC.EMISState ^= 1;

	if (UFC.EMISState)
	{
		DisplayWarning(EMIS, ON, 0);
		// turn off jammer
		Av.Tews.CM.ICSState = CM_ICS_STBY;
		PlayerPlane->AI.iAIFlags1 &= ~AIJAMMINGON;
	}
	else
		DisplayWarning(EMIS, OFF, 0);
}
//***********************************************************************************************************************************
void ToggleHudRAlt(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;
	UFC.HudRAltSelect ^=1;
	unsigned int RAlt = (unsigned int)((PlayerPlane->HeightAboveGround * WUTOFT) - 3.0);
	sprintf(UFCStr5, (UFC.HudRAltSelect)?"*RALT %d":"RALT %d", RAlt);
	SetUFCButtonFunction(4, ToggleHudRAlt, UFCStr5, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void ToggleHudTas(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;
	UFC.HudTasSelect ^=1;
	sprintf(UFCStr4, (UFC.HudTasSelect)?"*TAS %4d":"TAS %4d", (int)(PlayerPlane->Knots));
	SetUFCButtonFunction(3, ToggleHudTas, UFCStr4, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void ToggleHudThrottle(void)
{
	if (JoyCap1.wCaps & JOYCAPS_HASZ)
		UFC.HudThrottleSelect ^= 1;
	else
		UFC.HudThrottleSelect = 1;

	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;
	sprintf(UFCStr16, (UFC.HudThrottleSelect)?"*THRUST %3d%%":"THRUST %3d%%", round(PlayerPlane->ThrottlePos));
	SetUFCButtonFunction(5, ToggleHudThrottle, UFCStr16, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void DoUFCMainHudData(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;
	UFC.ActiveMenuExecuteFunction = DoUFCMainHudData;

	SetUFCButtonFunction(1,   NULL, "HUD OPTIONS", CENTERED);

	SetUFCButtonFunction(2,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(8,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(6,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(7,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(9,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(10,  NULL, "", RIGHT_JUSTIFY);

	sprintf(UFCStr4, (UFC.HudTasSelect)?"*TAS %4d":"TAS %4d", (int)(PlayerPlane->Knots));
	SetUFCButtonFunction(3, ToggleHudTas, UFCStr4, LEFT_JUSTIFY);

	unsigned int RAlt = (unsigned int)((PlayerPlane->HeightAboveGround * WUTOFT) - 3.0);
	sprintf(UFCStr5, (UFC.HudRAltSelect)?"*RALT %d":"RALT %d", RAlt);
	SetUFCButtonFunction(4, ToggleHudRAlt, UFCStr5, LEFT_JUSTIFY);

	sprintf(UFCStr16, (UFC.HudThrottleSelect)?"*THRUST %3d%%":"THRUST %3d%%", round(PlayerPlane->ThrottlePos));
	SetUFCButtonFunction(5, ToggleHudThrottle, UFCStr16, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
//*
//*	Data Menu Stuff
//*
//***********************************************************************************************************************************
void DoUFCDataSPVisual(void)
{
	sprintf(UFCStr6,"NAV  %dA",(int)(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
	SetUFCButtonFunction(1, DoUFCDataSP,  UFCStr6, LEFT_JUSTIFY);
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void DoUFCDataSP(void)
{
	AINextWayPoint(PlayerPlane);
	sprintf(UFCStr6,"NAV  %dA",(int)(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
	SetUFCButtonFunction(1, DoUFCDataSP,  UFCStr6, LEFT_JUSTIFY);
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void DisplayCurrSPDistance(void)
{
	if (UFC.DataCurrSPDistance == BRG_RNG)
	{
//		sprintf(UFCStr7,"%d/%3.1f",(int)UFC.DataCurrSPBrg, UFC.DataCurrSPNavDist);
		sprintf(UFCStr7,"%d/%3.1f",(int)UFC.DataCurrSPMilBrg, UFC.DataCurrSPNavDist);
		SetUFCButtonFunction(10, ToggleDataCurrSPDistance, UFCStr7, RIGHT_JUSTIFY);
	}
	else
	{
		// Calculate the amount of fuel you will have left at this steering point

		unsigned int Seconds = (int)(UFC.DataCurrSPDistFeet/(float)PlayerPlane->V);
		unsigned int FuelLbsLeft = round(PlayerPlane->TotalFuel - ((Seconds/60.0) * (UFC.LeftFuelFlowRate+UFC.RightFuelFlowRate)));

		sprintf(UFCStr17, "%d LBS",FuelLbsLeft);
		SetUFCButtonFunction(10, ToggleDataCurrSPDistance, UFCStr17, RIGHT_JUSTIFY);

//		SetUFCButtonFunction(10, ToggleDataCurrSPDistance, "15000 LBS", RIGHT_JUSTIFY);
	}
}
//***********************************************************************************************************************************
void DisplayCurrSPTime(void)
{
	if (UFC.DataCurrSPTime == ETE)
	{
		sprintf( UFCStr8,"ETE %02d:%02d:%02d", UFC.DataCurrSPEteHr,
											   UFC.DataCurrSPEteMin,
											   UFC.DataCurrSPEteSec);

		SetUFCButtonFunction(9,  ToggleDataCurrSPTime, UFCStr8, RIGHT_JUSTIFY);
	}
	else
	{

		sprintf (UFCStr8,"ETA %02d:%02d:%02d", UFC.DataCurrSPEtaHr,
											   UFC.DataCurrSPEtaMin,
											   UFC.DataCurrSPEtaSec);

		SetUFCButtonFunction(9,  ToggleDataCurrSPTime, UFCStr8, RIGHT_JUSTIFY);
	}
}
//***********************************************************************************************************************************
//*	LookAhead Functions
//***********************************************************************************************************************************
void DoUFCDataLkAheadSPVisual(void)
{
	if (!UFC.pWPLKAhead)
	{
		SetUFCButtonFunction(3, NULL,"", LEFT_JUSTIFY);
		return;
	}

	UFC.InMainMenu = 0;
	UFC.InDataMenu = 1;
	sprintf(UFCStr10,"NAV  %dA", (UFC.pWPLKAhead - &AIWayPoints[PlayerPlane->AI.startwpts]) );
	SetUFCButtonFunction(3, DoUFCDataLkAheadSP,  UFCStr10, LEFT_JUSTIFY);
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void DoUFCDataLkAheadSP(void)
{
	if (!UFC.pWPLKAhead) return;

	if((UFC.iCurrLKAhead == -1) || (PlayerPlane != UFC.pWPPlane))
	{
		UFC.iCurrLKAhead = PlayerPlane->AI.numwaypts;
		UFC.pWPLKAhead = PlayerPlane->AI.CurrWay;
		UFC.pWPPlane = PlayerPlane;
	}

	if(UFC.iCurrLKAhead > 1)
	{
		UFC.pWPLKAhead++;
		UFC.iCurrLKAhead -= 1;
	}
	else
	{
		UFC.iCurrLKAhead =  (UFC.pWPLKAhead - &AIWayPoints[PlayerPlane->AI.startwpts]) + 1;
		UFC.pWPLKAhead = &AIWayPoints[PlayerPlane->AI.startwpts];
	}
	DoUFCDataLkAheadSPVisual();
}
//***********************************************************************************************************************************
void ComputeLkAheadNumbers(void)
{
	if (!UFC.pWPLKAhead) return;

	float Dx = (float)(PlayerPlane->WorldPosition.X - ConvertWayLoc(UFC.pWPLKAhead->lWPx));
	float Dz = (float)(PlayerPlane->WorldPosition.Z - ConvertWayLoc(UFC.pWPLKAhead->lWPz));
	UFC.DataLkAheadDistFeet = (sqrt((Dx*Dx) + (Dz*Dz))*WUTOFT); /// accuracy is important here
}
//***********************************************************************************************************************************
void DisplayLkAheadSPDistance(void)
{
	if (!UFC.pWPLKAhead)
	{
		SetUFCButtonFunction(8, NULL,"", RIGHT_JUSTIFY);
		return;
	}

	if (UFC.DataLkAheadSPDistance == BRG_RNG)
	{
		UFC.DataLkAheadSPNavDist = UFC.DataLkAheadDistFeet*FTTONM;

		float TotalDist, DeltaX, DeltaZ, DeltaY;
		FPoint Dist;

		Dist.X = ConvertWayLoc(UFC.pWPLKAhead->lWPx);
		Dist.Y = ConvertWayLoc(UFC.pWPLKAhead->lWPy);
		Dist.Z = ConvertWayLoc(UFC.pWPLKAhead->lWPz);

		float LkAheadWayPoint = AIComputeHeadingToPoint(PlayerPlane, Dist, &TotalDist, &DeltaX, &DeltaY, &DeltaZ, 0);

		if (LkAheadWayPoint < 0.0f)
			LkAheadWayPoint += 360.0f;

		LkAheadWayPoint = 360.0f - LkAheadWayPoint;

		if (LkAheadWayPoint == 360.0f)
			LkAheadWayPoint = 0.0f;

		UFC.DataLkAheadSPBrg = LkAheadWayPoint;

		sprintf(UFCStr11,"%d/%3.1f",UFC.DataLkAheadSPBrg, UFC.DataLkAheadSPNavDist);
		SetUFCButtonFunction(8, ToggleDataLkAheadSPDistance, UFCStr11, RIGHT_JUSTIFY);
	}
	else
	{
		// Calculate the amount of fuel you will have left at this steering point

		unsigned int Seconds = (int)(UFC.DataLkAheadDistFeet/(float)PlayerPlane->V);
		unsigned int FuelLbsLeft = round(PlayerPlane->TotalFuel - ((Seconds/60.0) * (UFC.LeftFuelFlowRate+UFC.RightFuelFlowRate)));

		sprintf(UFCStr18, "%d LBS",FuelLbsLeft);
		SetUFCButtonFunction(8, ToggleDataLkAheadSPDistance, UFCStr18, RIGHT_JUSTIFY);
	}
}
//***********************************************************************************************************************************
void DisplayLkAheadSPTime(void)
{
	if (!UFC.pWPLKAhead)
	{
		SetUFCButtonFunction(7,  NULL, "", RIGHT_JUSTIFY);
		return;
	}

	int TSeconds = (int)(UFC.DataLkAheadDistFeet/(float)PlayerPlane->V);

	if (UFC.DataLkAheadSPTime == TOA)
	{
		UFC.DataLkAheadSPToaHr =  TSeconds/3600;
		UFC.DataLkAheadSPToaMin = (TSeconds%3600)/60;
		UFC.DataLkAheadSPToaSec = TSeconds % 60;

		sprintf( UFCStr12,"TOA %02d:%02d:%02d", UFC.DataLkAheadSPToaHr,
											    UFC.DataLkAheadSPToaMin,
											    UFC.DataLkAheadSPToaSec);

		SetUFCButtonFunction(7,  ToggleDataLkAheadSPTime, UFCStr12, RIGHT_JUSTIFY);
	}
	else
	{

		int WrkSeconds = (int)WorldParams.WorldTime + TSeconds;
		if (WrkSeconds >= 86400)
			WrkSeconds -= 86400;

		if (UFC.DataCurrTimeMode == ZULU)
		{
			WrkSeconds -= 10800;
			if (WrkSeconds < 0)
				WrkSeconds += 86400;
		}

		UFC.DataLkAheadSPTotHr =  WrkSeconds/3600;
		UFC.DataLkAheadSPTotMin = (WrkSeconds%3600)/60;
		UFC.DataLkAheadSPTotSec = WrkSeconds % 60;

		sprintf( UFCStr12,"TOT %02d:%02d:%02d", UFC.DataLkAheadSPTotHr,
											    UFC.DataLkAheadSPTotMin,
											    UFC.DataLkAheadSPTotSec);

		SetUFCButtonFunction(7,  ToggleDataLkAheadSPTime, UFCStr12, RIGHT_JUSTIFY);
	}
}
//***********************************************************************************************************************************
void ToggleDataLkAheadSPDistance(void)
{
	if (!UFC.pWPLKAhead) return;

	UFC.UFCClear = 2;
	UFC.DataLkAheadSPDistance ^= 1;
	DisplayLkAheadSPDistance();
}
//***********************************************************************************************************************************
void ToggleDataLkAheadSPTime(void)
{
	if (!UFC.pWPLKAhead) return;

	UFC.UFCClear = 2;
	UFC.DataLkAheadSPTime ^= 1;
	DisplayLkAheadSPTime();
}
//***********************************************************************************************************************************
//*	End LookAhead Functions
//***********************************************************************************************************************************
//***********************************************************************************************************************************
//*	Current Waypoint Functions
//***********************************************************************************************************************************
void ToggleDataCurrSPDistance(void)
{
	UFC.UFCClear = 2;
	UFC.DataCurrSPDistance ^= 1;
	DisplayCurrSPDistance();
}
//***********************************************************************************************************************************
void ToggleDataCurrSPTime(void)
{
	UFC.UFCClear = 2;
	UFC.DataCurrSPTime ^= 1;
	DisplayCurrSPTime();
}
//***********************************************************************************************************************************
void DisplayCurrSPAltitude(void)
{
	UFC.DataCurrAltitude = PlayerPlane->AI.CurrWay->lWPy;
	sprintf(UFCStr19,"ALT %d",	UFC.DataCurrAltitude);
	SetUFCButtonFunction(8,  NULL, UFCStr19, RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************
void DisplayCurrSPSpeed(void)
{
	UFC.DataCurrSpeed =    PlayerPlane->AI.CurrWay->iSpeed;
	sprintf(UFCStr20,"SPD %d",	UFC.DataCurrSpeed);
	SetUFCButtonFunction(3,  NULL, UFCStr20, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void ToggleDataCurrTimeMode(void)
{
	UFC.UFCClear = 2;
	UFC.DataCurrTimeMode ^= 1;
	SetUFCButtonFunction(6,  NULL, (UFC.DataCurrTimeMode == LOCAL) ? "LOCAL" : "ZULU", RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************
void DisplayDataCurrTime(void)
{
	if (UFC.DataCurrTimeMode == LOCAL)
	{
		UFC.DataCurrTimeLocalHr =   (int)WorldParams.WorldTime/3600;
		UFC.DataCurrTimeLocalMin = ((int)WorldParams.WorldTime%3600)/60;
		UFC.DataCurrTimeLocalSec =  (int)WorldParams.WorldTime%60;

		sprintf(UFCStr9,"TIME %02d:%02d:%02d",	UFC.DataCurrTimeLocalHr,
												UFC.DataCurrTimeLocalMin,
												UFC.DataCurrTimeLocalSec);
	}
	else   // zulu time
	{
		float WorkTime = WorldParams.WorldTime;
		WorkTime -= 10800.0;
		if (WorkTime < 0.0)
			WorkTime += 86400.0;

		UFC.DataCurrTimeZuluHr =   (int)WorkTime/3600;
		UFC.DataCurrTimeZuluMin = ((int)WorkTime%3600)/60;
		UFC.DataCurrTimeZuluSec =  (int)WorkTime%60;

		sprintf(UFCStr9,"TIME %02d:%02d:%02d",	UFC.DataCurrTimeZuluHr,
												UFC.DataCurrTimeZuluMin,
												UFC.DataCurrTimeZuluSec);
	}

	SetUFCButtonFunction(5,  NULL, UFCStr9, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void InitUFCDataMenu(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 1;
	UFC.ActiveMenuExecuteFunction = InitUFCDataMenu;

	DoUFCDataSPVisual();
	DisplayCurrSPDistance();
	DisplayCurrSPTime();
	DisplayCurrSPAltitude();
	DisplayCurrSPSpeed();

	SetUFCButtonFunction(2,  NULL, "", LEFT_JUSTIFY);
	SetUFCButtonFunction(4,  NULL, "", LEFT_JUSTIFY);
	SetUFCButtonFunction(7,  NULL, "", RIGHT_JUSTIFY);

	DisplayDataCurrTime();

	SetUFCButtonFunction(6,  ToggleDataCurrTimeMode, (UFC.DataCurrTimeMode == LOCAL) ? "LOCAL" : "ZULU", RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************
void ChangeStrMode(void)
{
	TGTLatch = 0;

	if (UFC.SteeringMode == NAV_STR)
	{
		if ((g_iNumTACANObjects) && (UFC.CurrentTacanPtr))
			UFC.SteeringMode = TCN_STR;
		else
			if ((PlayerPlane->AGDesignate.X != -1) && (PlayerPlane->AGDesignate.Z != -1))
				UFC.SteeringMode = TGT_STR;
	}
	else
		if (UFC.SteeringMode == TCN_STR)
		{
			if ((PlayerPlane->AGDesignate.X != -1) && (PlayerPlane->AGDesignate.Z != -1))
				UFC.SteeringMode = TGT_STR;
			else
				UFC.SteeringMode = NAV_STR;
		}
		else
		{
			UFC.SteeringMode = NAV_STR;
			TGTLatch = 1;
		}

	DisplayStrMode();
}
//***********************************************************************************************************************************
void DisplayStrMode(void)
{
	switch(UFC.SteeringMode)
	{
		case NAV_STR:	SetUFCButtonFunction(6, ChangeStrMode, "NAV STR", RIGHT_JUSTIFY);
						break;
		case TCN_STR:	SetUFCButtonFunction(6, ChangeStrMode, "TCN STR", RIGHT_JUSTIFY);
						break;
		case TGT_STR:	SetUFCButtonFunction(6, ChangeStrMode, "TGT STR", RIGHT_JUSTIFY);
						break;
	}
	UFC.UFCClear = 2;
}

//***********************************************************************************************************************************

void DisplayNavFlirInfo()
{
	if (UFC.NavFlirStatus == 0)
	{
		SetUFCButtonFunction(7,  DoUFCMainNavFlir, "N-F OFF", RIGHT_JUSTIFY);
	}
	else
	{
		if (_3dxlDoesBlackHot())
		{
			if (UFC.NavFlirStatus == 1)
				SetUFCButtonFunction(7,  DoUFCMainNavFlir, "N-F NORM-WH", RIGHT_JUSTIFY);
			else
				SetUFCButtonFunction(7,  DoUFCMainNavFlir, "N-F NORM-BH", RIGHT_JUSTIFY);
		}
		else
			SetUFCButtonFunction(7,  DoUFCMainNavFlir, "N-F NORM", RIGHT_JUSTIFY);
	}
}

//***********************************************************************************************************************************

void InitUFCMainMenu(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 1;
	UFC.InDataMenu = 0;
	UFC.ActiveMenuExecuteFunction = NULL;
	UFC.TCNSubMenu = 0;

	if (UFC.LawWarningStatus)
	{
		sprintf(UFCStr2, "LAW  %d",UFC.LowAltWarningLimit);
		SetUFCButtonFunction(1,  DoUFCMainLaw, UFCStr2, LEFT_JUSTIFY);
	}else
		SetUFCButtonFunction(1,  DoUFCMainLaw, "LAW OFF", LEFT_JUSTIFY);

	if (!UFC.CurrentTacanPtr)
		SetDefaultTacanStation();

	if ((g_iNumTACANObjects) && (UFC.CurrentTacanPtr))
		SetUFCButtonFunction(2,  DoUFCMainTcn, UFCStr13, LEFT_JUSTIFY);
	else
		SetUFCButtonFunction(2,  NULL, UFCStr13, LEFT_JUSTIFY);

	DisplayStrMode();

	if (UFC.IFFStatus == NORMAL)
		SetUFCButtonFunction(3,  DoUFCMainIff, "IFF NORM", LEFT_JUSTIFY);
	else if (UFC.IFFStatus == AUTO)
		SetUFCButtonFunction(3,  DoUFCMainIff, "IFF AUTO", LEFT_JUSTIFY);

	SetUFCButtonFunction(4,  DoUFCMainTf, (UFC.TFStatus) ? "TF NORM" : "TF OFF",  LEFT_JUSTIFY);
	SetUFCButtonFunction(5,  LawOff, (UFC.LawWarningStatus) ? "LAW OFF" : "LAW ON", LEFT_JUSTIFY);
	SetUFCButtonFunction(5,  DoUFCMainIls, (UFC.ILSStatus) ? "ILS ON" : "ILS OFF",  LEFT_JUSTIFY);

	DisplayStrMode();
	DisplayNavFlirInfo();

	SetUFCButtonFunction(8,  DoUFCMainHudData,   "HUD DATA", RIGHT_JUSTIFY);

	if (UFC.APStatus == OFF)
	{
		if (RealisticAPs)
			SetUFCButtonFunction(9, DoUFCMainAutoPilot, "A/P OFF",  RIGHT_JUSTIFY);
		else
			SetUFCButtonFunction(9, DoCasualAutoPilot, "A/P OFF",  RIGHT_JUSTIFY);
	}
	else
	{
		if (RealisticAPs)
			UFCDisplayAutoMode(PlayerPlane,9,RIGHT_JUSTIFY);
		else
			SetUFCButtonFunction(9, DoCasualAutoPilot, "A/P ON",  RIGHT_JUSTIFY);
	}

	sprintf(UFCStr3,"STR  %dA",(int)(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
	SetUFCButtonFunction(10, DoUFCMainStrMode,  UFCStr3,   RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************
void DoUFCMainLaw(void)
{
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;

	SetUFCButtonFunction(1,  NULL, "LAW STATUS", CENTERED);

	sprintf(UFCStr1, "ALT  %d",UFC.LowAltWarningLimit);
	SetUFCButtonFunction(2,  NULL, UFCStr1, LEFT_JUSTIFY);

	SetUFCButtonFunction(3,  DecreaseLaw, "DECREASE", LEFT_JUSTIFY);
	SetUFCButtonFunction(4,  NULL,  "", LEFT_JUSTIFY);

	SetUFCButtonFunction(5,  LawOff, (UFC.LawWarningStatus) ? "LAW ON" : "LAW OFF", LEFT_JUSTIFY);

	SetUFCButtonFunction(6,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(7,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(8,   IncreaseLaw, "INCREASE", RIGHT_JUSTIFY);
	SetUFCButtonFunction(9,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(10,  NULL, "", RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************
void IncreaseLaw(void)
{
	UFC.LowAltWarningLimit+=100;
	if (UFC.LowAltWarningLimit > 1500) UFC.LowAltWarningLimit = 1500;
	UFC.UFCClear = 2;
	sprintf(UFCStr1, "ALT  %d",UFC.LowAltWarningLimit);
	SetUFCButtonFunction(2,  NULL, UFCStr1, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void DecreaseLaw(void)
{
	UFC.LowAltWarningLimit-=100;
	if (UFC.LowAltWarningLimit < 100) UFC.LowAltWarningLimit = 100;
	UFC.UFCClear = 2;
	sprintf(UFCStr1, "ALT  %d",UFC.LowAltWarningLimit);
	SetUFCButtonFunction(2,  NULL, UFCStr1, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void ForceLaw(void)
{
	UFC.UFCClear = 2;
	if (UFC.LawWarningStatus)
	{
		sprintf(UFCStr2, "LAW  %d",UFC.LowAltWarningLimit);
		SetUFCButtonFunction(1,  DoUFCMainLaw, UFCStr2, LEFT_JUSTIFY);
	}else
		SetUFCButtonFunction(1,  DoUFCMainLaw, "LAW OFF", LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void LawOff(void)
{
	UFC.LawWarningStatus^=1;
	SetUFCButtonFunction(5,  LawOff, (UFC.LawWarningStatus) ? "LAW ON" : "LAW OFF", LEFT_JUSTIFY);
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void DisplayTacanRngBrg(void)
{
	sprintf(UFCStr15, "%d/%3.1f",(int)UFC.TacanMilBearing, UFC.TacanNavDist);
	SetUFCButtonFunction(2,  NULL, UFCStr15, LEFT_JUSTIFY);
}
//***********************************************************************************************************************************
void DisplayTacanTime(void)
{
	if (UFC.TacanEte == ETE)
	{
		sprintf(UFCStr14,"%02d:%02d:%02d E", UFC.TacanEteHr,
											 UFC.TacanEteMin,
											 UFC.TacanEteSec);
	}
	else
	{
		sprintf(UFCStr14,"%02d:%02d:%02d A", UFC.TacanEtaHr,
											 UFC.TacanEtaMin,
											 UFC.TacanEtaSec);
	}
	SetUFCButtonFunction(9, ToggleTacanTime, UFCStr14, RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************
void ToggleTacanTime(void)
{
	UFC.UFCClear = 2;
	UFC.TacanEte ^= 1;
	DisplayTacanTime();
}
//***********************************************************************************************************************************
void SetCurrentTacanPos(void)
{
	if(!UFC.CurrentTacanPtr)
	{
		//InitUFCMainMenu();
		return;
	}

	if (UFC.CurrentTacanPtr->iType == AIRCRAFT)
	{
		int i = UFC.CurrentTacanPtr->dwSerialNumber;
		UFC.TacanPos.X = Planes[i].WorldPosition.X;
		UFC.TacanPos.Y = Planes[i].WorldPosition.Y;
		UFC.TacanPos.Z = Planes[i].WorldPosition.Z;
	}
	else if (UFC.CurrentTacanPtr->iType == GROUNDOBJECT)
	{
		UFC.TacanPos.X = UFC.CurrentTacanPtr->lX * FTTOWU;
		UFC.TacanPos.Z = UFC.CurrentTacanPtr->lZ * FTTOWU;
		UFC.TacanPos.Y = LandHeight(UFC.TacanPos.X,UFC.TacanPos.Z);
	}
	else if(UFC.CurrentTacanPtr->iType == MOVINGVEHICLE)
	{
		int i = UFC.CurrentTacanPtr->dwSerialNumber;
		UFC.TacanPos.X = MovingVehicles[i].WorldPosition.X;
		UFC.TacanPos.Y = MovingVehicles[i].WorldPosition.Y;
		UFC.TacanPos.Z = MovingVehicles[i].WorldPosition.Z;
	}
}
//***********************************************************************************************************************************
void SetDefaultTacanStation(void)
{
	if (g_iNumTACANObjects)
	{
		UFC.CurrentTacanPtr = pTACANTypeList;
		_ltoa(UFC.CurrentTacanPtr->lChannel,UFC.CurrentTacanStation,10);

		strcpy(UFC.CurrentTacanString,UFC.CurrentTacanPtr->sLabel);

		UFC.CurrentTacanString[19] = 0;
		SetCurrentTacanPos();
		strcpy(UFCStr13,"TCN ");
		strcat(UFCStr13, UFC.CurrentTacanStation);
		strcat(UFCStr13, ((UFC.CurrentTacanPtr->iType == AIRCRAFT) ? "X" : "Y"));

		PlaneParams *planepnt;
		TACANType *NewPtr = UFC.CurrentTacanPtr;
		if(NewPtr->iType == AIRCRAFT)
		{
			planepnt = &Planes[UFC.CurrentTacanPtr->dwSerialNumber];
			if((planepnt->Status & PL_ACTIVE) && (!(planepnt->FlightStatus & PL_STATUS_CRASHED)))
			{
				if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
				{
					if(!((planepnt->AI.Behaviorfunc == AIFlyTanker) || (planepnt->AI.Behaviorfunc == AIFlyToTankerPoint) || (planepnt->AI.Behaviorfunc == AITankingFlight) || (planepnt->AI.Behaviorfunc == AITankingFlight2) || (planepnt->AI.Behaviorfunc == AITankingFlight3) || (planepnt->AI.Behaviorfunc == AITankingFlight4) || (planepnt->AI.Behaviorfunc == AITankingFlight5)))
					{
						TacanNextWClear();
					}
				}
			}
		}
	}
	else
	{
		UFC.CurrentTacanPtr = NULL;
		strcpy(UFC.CurrentTacanStation,"XXX");
		UFC.CurrentTacanString[0] = NULL;
		strcpy(UFCStr13,"TCN ");
		strcat(UFCStr13, UFC.CurrentTacanStation);
	}
}
//***********************************************************************************************************************************
void SetSpecificTacanStation(TACANType *NewPtr)
{
	UFC.CurrentTacanPtr = NewPtr;
	_ltoa(UFC.CurrentTacanPtr->lChannel,UFC.CurrentTacanStation,10);

	strcpy(UFC.CurrentTacanString,UFC.CurrentTacanPtr->sLabel);

	UFC.CurrentTacanString[19] = 0;
	SetCurrentTacanPos();

	strcpy(UFCStr13,"TCN ");
	strcat(UFCStr13, UFC.CurrentTacanStation);
	strcat(UFCStr13, ((UFC.CurrentTacanPtr->iType == AIRCRAFT) ? "X" : "Y"));
}
//***********************************************************************************************************************************
void TacanNext(void)
{
	int done = 0;
	PlaneParams *planepnt;

	if (!g_iNumTACANObjects) return;

	TACANType *NewPtr = UFC.CurrentTacanPtr;
	TACANType *LastPtr = &pTACANTypeList[g_iNumTACANObjects-1];

	while(!done)
	{
		if ((++NewPtr) > LastPtr)
			NewPtr = pTACANTypeList;

		if(UFC.CurrentTacanPtr == NewPtr)
		{
			return;
		}
		else
		{
			if(NewPtr->iType != AIRCRAFT)
			{
				done = 1;
			}
			else
			{
				planepnt = &Planes[NewPtr->dwSerialNumber];
				if((planepnt->Status & PL_ACTIVE) && (!(planepnt->FlightStatus & PL_STATUS_CRASHED)))
				{
					if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER))
					{
						done = 1;
					}
					else
					{
						if((planepnt->AI.Behaviorfunc == AIFlyTanker) || (planepnt->AI.Behaviorfunc == AIFlyToTankerPoint) || (planepnt->AI.Behaviorfunc == AITankingFlight) || (planepnt->AI.Behaviorfunc == AITankingFlight2) || (planepnt->AI.Behaviorfunc == AITankingFlight3) || (planepnt->AI.Behaviorfunc == AITankingFlight4) || (planepnt->AI.Behaviorfunc == AITankingFlight5))
						{
							done = 1;
						}
					}
				}
			}
		}
	}

	SetSpecificTacanStation(NewPtr);
	DoUFCMainTcn();
}
//***********************************************************************************************************************************
void TacanNextWClear(void)
{
	int done = 0;
	PlaneParams *planepnt;

	if (!g_iNumTACANObjects) return;

	TACANType *NewPtr = UFC.CurrentTacanPtr;
	TACANType *LastPtr = &pTACANTypeList[g_iNumTACANObjects-1];

	while(!done)
	{
		if ((++NewPtr) > LastPtr)
			NewPtr = pTACANTypeList;

		if(UFC.CurrentTacanPtr == NewPtr)
		{
			ClearCurrentTacan();
			return;
		}
		else
		{
			if(NewPtr->iType != AIRCRAFT)
			{
				done = 1;
			}
			else
			{
				planepnt = &Planes[NewPtr->dwSerialNumber];
				if((planepnt->Status & PL_ACTIVE) && (!(planepnt->FlightStatus & PL_STATUS_CRASHED)))
				{
					if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER))
					{
						done = 1;
					}
					else
					{
						if((planepnt->AI.Behaviorfunc == AIFlyTanker) || (planepnt->AI.Behaviorfunc == AIFlyToTankerPoint) || (planepnt->AI.Behaviorfunc == AITankingFlight) || (planepnt->AI.Behaviorfunc == AITankingFlight2) || (planepnt->AI.Behaviorfunc == AITankingFlight3) || (planepnt->AI.Behaviorfunc == AITankingFlight4) || (planepnt->AI.Behaviorfunc == AITankingFlight5))
						{
							done = 1;
						}
					}
				}
			}
		}
	}

	SetSpecificTacanStation(NewPtr);
//	DoUFCMainTcn();
}
//***********************************************************************************************************************************
void TacanPrevious(void)
{
	int done = 0;
	PlaneParams *planepnt;

	if (!g_iNumTACANObjects) return;

	TACANType *NewPtr = UFC.CurrentTacanPtr;

	while(!done)
	{
		if ((--NewPtr) < pTACANTypeList)
			NewPtr = &pTACANTypeList[g_iNumTACANObjects-1];

		if(UFC.CurrentTacanPtr == NewPtr)
		{
			return;
		}
		else
		{
			if(NewPtr->iType != AIRCRAFT)
			{
				done = 1;
			}
			else
			{
				planepnt = &Planes[NewPtr->dwSerialNumber];
				if((planepnt->Status & PL_ACTIVE) && (!(planepnt->FlightStatus & PL_STATUS_CRASHED)))
				{
					if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER))
					{
						done = 1;
					}
					else
					{
						if((planepnt->AI.Behaviorfunc == AIFlyTanker) || (planepnt->AI.Behaviorfunc == AIFlyToTankerPoint) || (planepnt->AI.Behaviorfunc == AITankingFlight) || (planepnt->AI.Behaviorfunc == AITankingFlight2) || (planepnt->AI.Behaviorfunc == AITankingFlight3) || (planepnt->AI.Behaviorfunc == AITankingFlight4) || (planepnt->AI.Behaviorfunc == AITankingFlight5))
						{
							done = 1;
						}
					}
				}
			}
		}
	}

	SetSpecificTacanStation(NewPtr);
	DoUFCMainTcn();
}

//***********************************************************************************************************************************
void CheckCurrentTacanStillValid(int type, void *ptr)
{
	PlaneParams *planepnt;
	BasicInstance *walker;

	if(!UFC.CurrentTacanPtr)
	{
		return;
	}

	if(UFC.CurrentTacanPtr->iType != type)
	{
		return;
	}

	if(UFC.CurrentTacanPtr->iType == AIRCRAFT)
	{
		planepnt = (PlaneParams *)ptr;
		if(&Planes[UFC.CurrentTacanPtr->dwSerialNumber] == planepnt)
		{
			TacanNextWClear();
			InitUFCMainMenu();
		}
	}
	else if (UFC.CurrentTacanPtr->iType == GROUNDOBJECT)
	{
		walker = (BasicInstance *)ptr;
		if(walker->SerialNumber == UFC.CurrentTacanPtr->dwSerialNumber)
		{
			TacanNextWClear();
			InitUFCMainMenu();
		}
	}
	return;
}

//***********************************************************************************************************************************
void ClearCurrentTacan(void)
{
	UFC.CurrentTacanPtr = NULL;
	strcpy(UFC.CurrentTacanStation,"XXX");
	UFC.CurrentTacanString[0] = NULL;
	strcpy(UFCStr13,"TCN ");
	strcat(UFCStr13, UFC.CurrentTacanStation);
	if (UFC.SteeringMode == TCN_STR)
	{
		UFC.SteeringMode = NAV_STR;
	}
}

//***********************************************************************************************************************************
void DoUFCMainTcn(void)
{
	UFC.TCNSubMenu = 1;
	UFC.UFCClear = 2;
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;
	UFC.ActiveMenuExecuteFunction = DoUFCMainTcn;

	SetUFCButtonFunction(1,  NULL, UFCStr13, CENTERED);
	DisplayTacanRngBrg();	// function 2
	SetUFCButtonFunction(3,   NULL, UFC.CurrentTacanString, CENTERED);
	SetUFCButtonFunction(4,   TacanPrevious,  "PREVIOUS", LEFT_JUSTIFY);
	SetUFCButtonFunction(5,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(6,   NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(7,   TacanNext,  "NEXT", RIGHT_JUSTIFY);
	SetUFCButtonFunction(8,   NULL, "", RIGHT_JUSTIFY);
	DisplayTacanTime();		// function 9
	SetUFCButtonFunction(10,  NULL, "", RIGHT_JUSTIFY);
}
//***********************************************************************************************************************************

//***********************************************************************************************************************************
void DoUFCMainIff(void)
{
	UFC.IFFStatus++;
	if (UFC.IFFStatus > 2)
		UFC.IFFStatus = 1;

	//if (UFC.IFFStatus == OFF)
	//	SetUFCButtonFunction(3,  DoUFCMainIff, "IFF OFF", LEFT_JUSTIFY);
	//else

	if (UFC.IFFStatus == NORMAL)
	{
		SetUFCButtonFunction(3,  DoUFCMainIff, "IFF NORM", LEFT_JUSTIFY);
//	    SetIff(NORMAL);
	    SetIff(AUTO);
	}
	else if (UFC.IFFStatus == AUTO)
	{
		SetUFCButtonFunction(3,  DoUFCMainIff, "IFF AUTO", LEFT_JUSTIFY);
//	    SetIff(AUTO);
	    SetIff(NORMAL);
	}

	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
int HasNavFlir(void)
{
	return((PlayerPlane->WeapLoad[LANTIRN_R].Count > 0) && (PlayerPlane->WeapLoad[LANTIRN_R].WeapId == 45));
}
//***********************************************************************************************************************************
void DoUFCMainTf(void)
{
	if (!HasNavFlir() || MultiPlayer) return;

	int AGL = (int)(PlayerPlane->HeightAboveGround*WUTOFT);
	int A = AGL%100;

	if (A >= 50) A = 100-A;
	else A = -A;

	AGL += A;

	if (((AGL >= 200) && (AGL <= 3000) &&
		 (PlayerPlane->IndicatedAirSpeed >= 325) && (UFC.TFStatus == OFF) &&
  		 (((UFC.DegRoll <= 60) || (UFC.DegRoll >= 300)) &&
		  ((UFC.DegPitch <= 45) || (UFC.DegPitch >= 315))))
		 || (UFC.TFStatus == ON))
	{
		UFC.TFStatus ^= 1;

		if (UFC.TFStatus)
		{
			UFC.TFHoldRdrAlt = AGL;

			// Needs to be an even 100 multiple
			A = (AGL*0.5f);
			int B = A - (A%100);
			UFC.LowAltWarningLimit = B;
			ForceLaw();
			AutoPilotOff(PlayerPlane);

			DisplayWarning(AUTO_ALERT,ON,0);
		}
		else
			DisplayWarning(AUTO_ALERT,OFF,0);

		SetUFCButtonFunction(4,  DoUFCMainTf, (UFC.TFStatus) ? "TF NORM" : "TF OFF",  LEFT_JUSTIFY);
		UFC.UFCClear = 2;
	}
}
//***********************************************************************************************************************************
/*-----------------------------------------------------------------------------
 *
 *	GetRunway()
 *
 *		Find the nearest operational runway within 10nm of player.
 *		Return pRunway cast as an int to stuff into "ILSStatus"
 */
int GetRunway()
{
	double  minfar = 400.0*(NMTOWU * NMTOWU); //20 NM squared
	RunwayInfo *runway = NULL;
	double newfar;

	for (RunwayInfo *R=AllRunways; R!=NULL; R=R->NextRunway)
	{
		if (!(R->Flags & RI_DESTROYED))
		{
			newfar = R->ILSLandingPoint ^ PlayerPlane->WorldPosition;

			if (newfar < minfar)
			{
				minfar = newfar;
				runway = R;
			}
		}
	}

	/* -----------------10/18/99 4:50PM--------------------------------------------------------------------
	/* Vehicles can also be runways, so we have to look for them
	/* ----------------------------------------------------------------------------------------------------*/
	MovingVehicleParams *W;

	for (W = MovingVehicles;W <= LastMovingVehicle;W++)
	{
		if(!W->Type)
			W->Status &= ~(VL_ACTIVE);

		if((W->Status & VL_ACTIVE) && !(W->Status & (VL_SINKING | VL_DESTROYED | VL_INVISIBLE)))
		{
			if (W->Type->iShipType && (!W->Type->TypeNumber) && ((W->Type->Model->Name[0] | 0x20) == 'n') && ((W->Type->Model->Name[1] | 0x20) == 'i') && ((W->Type->Model->Name[2] | 0x20) == 't'))
			{
				newfar = W->Runwaydata.ILSLandingPoint ^ PlayerPlane->WorldPosition;

				if (newfar < minfar)
				{
					minfar = newfar;
					runway = (RunwayInfo *)&W->Runwaydata;
				}
			}
		}
	}

	return (int)runway;
}


//***********************************************************************************************************************************
void DoUFCMainIls(void)
{
	UFC.ILSStatus = UFC.ILSStatus ? OFF : GetRunway();
}

//***********************************************************************************************************************************
void DoUFCMainNavFlir(void)
{
	static int LastHudColor;

	if (!Av.Weapons.HasNavFlir) return;

	UFC.NavFlirStatus++;

	if (UFC.NavFlirStatus >= 3)
		UFC.NavFlirStatus = 0;

	if (UFC.NavFlirStatus==0)
	{
		CurrentHudColorIndex = LastHudColor;
		HUDColor = HUDColorChoices[CurrentHudColorIndex];
	}
	else
		if (UFC.NavFlirStatus==1)
		{
			LastHudColor = CurrentHudColorIndex;
			CurrentHudColorIndex = 5+2;
			HUDColor = HUDColorChoices[CurrentHudColorIndex];
		}
		else
		{
			CurrentHudColorIndex = 5+2;
			HUDColor = HUDColorChoices[CurrentHudColorIndex];
		}
}
//***********************************************************************************************************************************
void DoUFCMainData(void)
{


}
//***********************************************************************************************************************************
//*
//*	AutoPilot UFC Stuff
//*
//***********************************************************************************************************************************

/**

void APOverrideAutoMode(void)
{
	if ((PlayerPlane->AutoPilotMode & PL_AP_ATTITUDE) ||
		(PlayerPlane->AutoPilotMode & PL_AP_HDG))
	{
		PlayerPlane->AutoPilotMode &= ~PL_AP_ATTITUDE;
		PlayerPlane->AutoPilotMode &= ~PL_AP_HDG;
	}
}
//***********************************************************************************************************************************
void APToggleStrMode(void)
{
	if (((UFC.APStrMode == NAV) && (g_iNumTACANObjects) && (UFC.CurrentTacanPtr))	||
		 (UFC.APStrMode == TCN) )
	{
		UFC.APStrMode ^= 1;
		SetUFCButtonFunction(3,  APToggleStrMode, (UFC.APStrMode == NAV) ? "NAV" : "TCN" , LEFT_JUSTIFY);
 		if (UFC.APStatus == ON)
		{
			APOverrideAutoMode();
			SetAutoPilotMode();
			UFCDisplayAutoMode(PlayerPlane,1,CENTERED);
		}
		UFC.UFCClear = 2;
	}
}
//***********************************************************************************************************************************
void APToggleAltHoldMode(void)
{
	UFC.APAltHoldMode ^= 1;
	SetUFCButtonFunction(5,  APToggleAltHoldMode,   (UFC.APAltHoldMode == BARO) ? "BARO" : "RDR", LEFT_JUSTIFY);
 	if (UFC.APStatus == ON)
	{
		APOverrideAutoMode();
		SetAutoPilotMode();
		UFCDisplayAutoMode(PlayerPlane,1,CENTERED);
	}
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void SetAutoPilotMode(void)
{
	// att, alt, alt/nav, alt/tcn, hdg, nav, tcn

	if (PlayerPlane->AutoPilotMode == PL_AP_OFF)
		return;

	PlayerPlane->AutoPilotMode = 0;

	if (UFC.APStrModeActive || UFC.APAltHoldActive)
	{
			if (UFC.APStrModeActive)
			{
				if (UFC.APStrMode == NAV)
				{
					PlayerPlane->AutoPilotMode |= PL_AP_NAV;
					if (UFC.APAltHoldActive)
					{
						if (UFC.APAltHoldMode == RDR)
							PlayerPlane->AutoPilotMode |= PL_AP_ALT_RDR;
					}
				}
				else
					PlayerPlane->AutoPilotMode |= PL_AP_TACAN;
			}
			else
			{
				PlayerPlane->AutoPilotMode |= PL_AP_ALTITUDE;
				if (UFC.APAltHoldMode == BARO)
					PlayerPlane->AutoPilotMode |= PL_AP_ALT_BARO;
				else
					PlayerPlane->AutoPilotMode |= PL_AP_ALT_RDR;
			}
	}
	else		// nothing is active, determine HDG or ATT hold
	{
		if ((UFC.DegRoll <= 7) || (UFC.DegRoll >= 352))
		{
			PlayerPlane->AutoPilotMode |= PL_AP_HDG;
		}else
			PlayerPlane->AutoPilotMode |= PL_AP_ATTITUDE;
	}
}
//**********************************************************************************************************************************
void APToggleStrModeActive(void)
{
	UFC.APStrModeActive ^= 1;
	SetUFCButtonFunction(2,  APToggleStrModeActive, (UFC.APStrModeActive) ? "*STR MODE" : "STR MODE", LEFT_JUSTIFY);

	if (UFC.APStatus == ON)
	{
		APOverrideAutoMode();
		SetAutoPilotMode();
		UFCDisplayAutoMode(PlayerPlane,1,CENTERED);
	}
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void APToggleAltHoldActive(void)
{
	UFC.APAltHoldActive ^= 1;
	SetUFCButtonFunction(4,  APToggleAltHoldActive, (UFC.APAltHoldActive) ? "*ALT HOLD" : "ALT HOLD", LEFT_JUSTIFY);

	if (UFC.APStatus == ON)
	{
		APOverrideAutoMode();
		SetAutoPilotMode();
		UFCDisplayAutoMode(PlayerPlane,1,CENTERED);
	}
	UFC.UFCClear = 2;
}
*/
//***********************************************************************************************************************************
void UFCDisplayAutoMode(PlaneParams *P, int Pos, int Justify)
{
/*
	// att, alt/rdr, alt/baro, alt/nav, alt/tcn, hdg, nav, tcn

	if (P->AutoPilotMode & PL_AP_OFF)
		SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P OFF",  Justify);
	else
	if (P->AutoPilotMode & PL_AP_ATTITUDE)
		SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P ATT",  Justify);
	else
	if (P->AutoPilotMode & PL_AP_HDG)
		SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P HDG",  Justify);
	else
	if (P->AutoPilotMode & PL_AP_ALTITUDE)
	{
		if (UFC.InMainMenu)
			SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, (UFC.APAltHoldMode == BARO) ? "ALT/BARO" : "ALT/RDR",  Justify);
		else
			SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, (UFC.APAltHoldMode == BARO) ? "AP ALT/BARO" : "AP ALT/RDR",  Justify);
	}
	else
	if (P->AutoPilotMode & PL_AP_NAV)
	{
		if ((UFC.APAltHoldActive) && (UFC.APAltHoldMode == BARO))
			SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P NAV/BARO",  Justify);
		else
		if ((UFC.APAltHoldActive) && (UFC.APAltHoldMode == RDR))
			SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P NAV/RDR",  Justify);
		else
			SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P NAV",  Justify);
	}
	else
	if (P->AutoPilotMode & PL_AP_TACAN)
		SetUFCButtonFunction(Pos,  DoUFCMainAutoPilot, "A/P TCN",  Justify);
		*/
}

//***********************************************************************************************************************************
void DoUFCMainAutoPilot(void)
{
	/*
	UFC.InMainMenu = 0;
	UFC.InDataMenu = 0;

	UFCDisplayAutoMode(PlayerPlane,1,CENTERED);

	SetUFCButtonFunction(2,  APToggleStrModeActive, (UFC.APStrModeActive) ? "*STR MODE" : "STR MODE", LEFT_JUSTIFY);
	SetUFCButtonFunction(3,  APToggleStrMode, (UFC.APStrMode == NAV) ? "NAV" : "TCN"   , LEFT_JUSTIFY);

	SetUFCButtonFunction(4,  APToggleAltHoldActive, (UFC.APAltHoldActive) ? "*ALT HOLD" : "ALT HOLD", LEFT_JUSTIFY);
	SetUFCButtonFunction(5,  APToggleAltHoldMode,   (UFC.APAltHoldMode == BARO) ? "BARO" : "RDR", LEFT_JUSTIFY);

	SetUFCButtonFunction(6,  NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(7,  NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(8,  NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(9,  NULL, "", RIGHT_JUSTIFY);
	SetUFCButtonFunction(10, NULL, "", RIGHT_JUSTIFY);
	UFC.UFCClear = 2;
	*/
}

/*
//***********************************************************************************************************************************
void TFAutoPilotOff(PlaneParams *P)
{
	UFC.TFStatus = OFF;
	SetUFCButtonFunction(4, DoUFCMainTf, (UFC.TFStatus) ? "TF NORM" : "TF OFF", LEFT_JUSTIFY);
	DisplayWarning(AUTO_ALERT,OFF,0);
	UFC.UFCClear = 2;
}
*/
//***********************************************************************************************************************************
void AutoPilotOff(PlaneParams *P)
{
	if(PlayerPlane->OnGround == 2)  //  Don't turn autopilot off on carrier.
		return;

	UFC.APStatus = 0;
	P->Aileron = 0.0f;

//	P->AutoPilotMode = PL_AP_OFF;
	if (RealisticAPs)
	{
		if (UFC.InMainMenu)
			UFCDisplayAutoMode(P,9,RIGHT_JUSTIFY);
		else
			DoUFCMainAutoPilot();
	}
	else
	{
		if (UFC.InMainMenu)
			SetUFCButtonFunction(9, NULL, "A/P OFF",  RIGHT_JUSTIFY);
	}
	DisplayWarning(AUTO_ALERT,OFF,0);
	UFC.UFCClear = 2;

}
//***********************************************************************************************************************************

void AutoPilotOnOff(void)
{
	if(PlayerPlane->OnGround == 2)  //  Don't turn autopilot off on carrier.
		return;

	if((PlayerPlane->Status & PL_AI_DRIVEN) && ((PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch) || (PlayerPlane->AI.Behaviorfunc == CTDoCarrierLaunch)))
		return;

	if(!UFC.APStatus)
//	if ((!UFC.APStatus) && (((PlayerPlane->Flaps != 0.0) || (UFC.TFStatus)
//		||
//		 (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED))))
	{
		if(Cpit.Damage[AV_AUTOPILOT])
			return;

		if((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14) || ((PlayerPlane->AI.lVar2 > 7) && (PlayerPlane->AI.lVar2 <= 10))))
		{
			UFC.APStatus ^= 1;
			AIC_ACLS_Switch(PlayerPlane, 1);
			return;
		}
	}

//	if ((MultiPlayer) && (!(UFC.APStatus))) return;   //  Going to try and allow ACLS in multiplayer

	UFC.APStatus ^= 1;

	if (UFC.APStatus & 0x1)	// turn ON
	{
		if (RealisticAPs)
		{
			if (((UFC.DegRoll  <= 60) || (UFC.DegRoll  >= 300)) &&
				((UFC.DegPitch <= 45) || (UFC.DegPitch >= 315)))
			{
//				PlayerPlane->AutoPilotMode &= ~PL_AP_OFF;
				//SetAutoPilotMode();
				DisplayWarning(AUTO_ALERT,ON,0);
			}
			else
			{
				UFC.APStatus ^= 1;
//				PlayerPlane->AutoPilotMode = PL_AP_OFF;
				DisplayWarning(AUTO_ALERT,OFF,0);
			}
		}
		else
		{
			// Make sure realistic AP code is rendered usless
//			PlayerPlane->AutoPilotMode = PL_AP_OFF;

			StartCasualAutoPilot();
			DisplayWarning(AUTO_ALERT,ON,0);

			if(MultiPlayer)
			{
				NetPutGenericMessage2(PlayerPlane, GM2_ACLS_SET, 2);
			}
		}
	}
	else			// turn OFF
	{
		if((RealisticAPs) && (!(PlayerPlane->Status & PL_AI_DRIVEN)))
		{
#ifdef __DEMO__
			if(PlayerPlane->Status & PL_AI_DRIVEN)
			{
				EndCasualAutoPilot();
			}
#endif
			if(GetRegValueL("fore3") == 1)
			{
				if(PlayerPlane->Status & PL_AI_DRIVEN)
				{
					EndCasualAutoPilot();
				}
			}

	//		PlayerPlane->AutoPilotMode = PL_AP_OFF;
		}
		else
		{
			EndCasualAutoPilot();
			if(MultiPlayer)
			{
				NetPutGenericMessage2(PlayerPlane, GM2_ACLS_SET, 0);
			}
		}

		DisplayWarning(AUTO_ALERT,OFF,0);
	}

/*
	if (RealisticAPs)
	{
	 	if (UFC.InMainMenu)
			UFCDisplayAutoMode(PlayerPlane,9,RIGHT_JUSTIFY);
		else
			DoUFCMainA    utoPilot();
	}
	else
	{
		if (UFC.InMainMenu)
			SetUFCButtonFunction(9, DoCasualAutoPilot, UFC.APStatus ? "A/P ON" : "A/P OFF",  RIGHT_JUSTIFY);
	}

	UFC.UFCClear = 2;
*/
}

//***********************************************************************************************************************************
void UFCAdvanceWaypointVisual(void)
{
	if (UFC.InMainMenu)
		DoUFCMainStrModeVisual();
	else
		if (UFC.InDataMenu)
			DoUFCDataSPVisual();
}
//***********************************************************************************************************************************
void DoUFCMainStrModeVisual(void)
{
	sprintf(UFCStr3,"STR  %dA",(int)(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
	SetUFCButtonFunction(10, DoUFCMainStrMode,  UFCStr3,   RIGHT_JUSTIFY);
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void DoUFCMainStrMode(void)
{
	AINextWayPoint(PlayerPlane);
	sprintf(UFCStr3,"STR  %dA",(int)(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]));
	SetUFCButtonFunction(10, DoUFCMainStrMode,  UFCStr3,   RIGHT_JUSTIFY);
	UFC.UFCClear = 2;
}
//***********************************************************************************************************************************
void UFCSetAAMode(void)
{
	DisplayWarning(AG_DISPLAY,OFF,1);
	DisplayWarning(NAV_DISPLAY,OFF,1);
	DisplayWarning(INST_DISPLAY,OFF,1);
	DisplayWarning(AA_DISPLAY,ON,1);

	AGRadarShutDown(); // shut off AG radar since you are not using AG Radar

	if(Av.Ufc.Mode == UFC_CNI_MODE)
		DoCNIEW();

	UFC.HudInCmd = 0;
	UFC.MasterMode = AA_MODE;
	SetMasterModeLights(UFC.MasterMode);
}
//***********************************************************************************************************************************
void UFCSetAGMode(void)
{
	DisplayWarning(AG_DISPLAY,ON,1);
	DisplayWarning(NAV_DISPLAY,OFF,1);
	DisplayWarning(INST_DISPLAY,OFF,1);
	DisplayWarning(AA_DISPLAY,OFF,1);

	UFC.MasterMode = AG_MODE;
	SetMasterModeLights(UFC.MasterMode);
}
//***********************************************************************************************************************************
void UFCSetNAVMode(void)
{
	DisplayWarning(AG_DISPLAY,OFF,1);
	DisplayWarning(NAV_DISPLAY,ON,1);
	DisplayWarning(INST_DISPLAY,OFF,1);
	DisplayWarning(AA_DISPLAY,OFF,1);

	AGRadarShutDown(); // shut off AG radar since you are not using AG Radar

	UFC.HudInCmd = 0;
	UFC.MasterMode = NAV_MODE;
	SetMasterModeLights(UFC.MasterMode);
}
//***********************************************************************************************************************************
void UFCSetINSTMode(void)
{
   DisplayWarning(AG_DISPLAY,OFF,1);
   DisplayWarning(NAV_DISPLAY,OFF,1);
   DisplayWarning(INST_DISPLAY,ON,1);
   DisplayWarning(AA_DISPLAY,OFF,1);

   AGRadarShutDown(); // shut off AG radar since you are not using AG Radar

   UFC.HudInCmd = 0;
   UFC.MasterMode = INST_MODE;
}
//***********************************************************************************************************************************
void StepThroughMasterModes()
{

  // find current master mode
  CurrMasterModeIndex = 0;
  while(MasterModeOrder[CurrMasterModeIndex] != UFC.MasterMode)
  {
    CurrMasterModeIndex++;
    if (CurrMasterModeIndex > (MAX_MASTER_MODES-1))
		CurrMasterModeIndex = 0;
  }

  CurrMasterModeIndex++;
  if (CurrMasterModeIndex > (MAX_MASTER_MODES-1))
		CurrMasterModeIndex = 0;

  UFC.MasterMode = MasterModeOrder[CurrMasterModeIndex];

	// make  sure the lights match up
	SetMasterModeLights(UFC.MasterMode);

  switch(UFC.MasterMode)
  {
    case NAV_MODE:  UFCSetNAVMode();
	                break;
    case AG_MODE:   UFCSetAGMode();
	                break;
    case AA_MODE:   UFCSetAAMode();
	                break;
  }
}
//***********************************************************************************************************************************
void SetUFCButtonFunction(int ButtonNum, void (*NewFunction)(void), char *NewText, int Justify)
{
//	UFCPtr = &UFCControl[ButtonNum-1+25];
//	UFCPtr->Action = NewFunction;
//	UFCLed[ButtonNum-1].Text = NewText;
//	UFCLed[ButtonNum-1].JustifyType = Justify;
}
//***********************************************************************************************************************************
void LoadUFCSprites(void)
{
	LgLEDFont = GrLoadFont(RegPath("Cockpits","nLCD.fnt"));

	UnPauseFromDiskHit();
	InitUFCMainMenu();
}
//***********************************************************************************************************************************
void CleanupUFCSprites(void)
{
  	GrDestroyFont(LgLEDFont);
}
//***********************************************************************************************************************************
void UFCCalcCentralComputerData(void)
{
	//*************************************************************
	// HUD Ladder calculations based on orientation of PlayerPlane
	//*************************************************************

    int i,j,k,TSeconds,WrkSeconds;
	float Dx,Dy,Dz,Dist;

	UFC.DegRoll = PlayerPlane->Roll/DEGREE;
	UFC.DegPitch = PlayerPlane->Pitch/DEGREE;


	// Limit Beta for uncaged pitch ladders
//	float LimitBeta = -RadToDeg(PlayerPlane->Beta);

//	if (LimitBeta > 10.0f) LimitBeta = 10.0f;
//	else if (LimitBeta < -10.0f) LimitBeta = -10.0f;

	if (CockpitSeat != NO_SEAT)
	{
		int old_midx;
		int old_midy;

		SetVirtualHudCenter();

		SetHUDClipWindow();

		HUDAttitude.SetHPR(0,-PlayerPlane->Pitch,-PlayerPlane->Roll);

		old_midx = GrBuffFor3D->MidX;
		old_midy = GrBuffFor3D->MidY;

		GrBuffFor3D->MidX = 0;
		GrBuffFor3D->MidY = 0;

		// define FOV (20 degrees total) clip limits based upon actual pitch

		int PitchIndex = ((unsigned short)(PlayerPlane->Pitch/(unsigned short)DEGREE)/5);

		#define OFS 5

		int Start = PitchIndex-OFS;
		int Stop =  PitchIndex+OFS;

		for (i=Start; i<Stop; i++)
		{
			k = i;
			if (k< 0) k += 72;
			if (k>71) k -= 72;

			for (j=0; j<6; j++)
			{
				UFC.HUDLadder[k].HUDPoints[j].Rotated.RotateAndPerspect((int *)&UFC.HUDLadder[k].HUDPoints[j].iSX,
				                                                    	(int *)&UFC.HUDLadder[k].HUDPoints[j].iSY,
																    	&HUDAttitude,
																		&UFC.HUDLadder[k].RawHUDPoint[j]);

//				UFC.HUDLadder[k].HUDPoints[j].iSX *= (float)ScreenSize.cx/640.0f;
				UFC.HUDLadder[k].HUDPoints[j].iSX += HUD_MIDX;

//				UFC.HUDLadder[k].HUDPoints[j].iSY *= (float)ScreenSize.cx/640.0f;
				UFC.HUDLadder[k].HUDPoints[j].iSY += HUD_MIDY;
			}
		}

		GrBuffFor3D->MidX = old_midx;
		GrBuffFor3D->MidY = old_midy;

		UndoHUDClipWindow();
	}

	//*************************************************************
	// NAV WAYPOINT/TGT POINT/TIME Calculations
	//*************************************************************

	// Current TGT point - if we have one

	if( (PlayerPlane->AGDesignate.X != -1) )
	{
		UFC.DataCurrTGTRelBrg = AIComputeHeadingToPoint(PlayerPlane, PlayerPlane->AGDesignate, &Dist, &Dx ,&Dy, &Dz, 1);
		UFC.DataCurrTGTAbsBrg = AIComputeHeadingToPoint(PlayerPlane, PlayerPlane->AGDesignate, &Dist, &Dx ,&Dy, &Dz, 0);
		if (UFC.DataCurrTGTAbsBrg < 0.0f)
			UFC.DataCurrTGTAbsBrg += 360.0f;

		UFC.DataCurrTGTAbsBrg = 360.0f - UFC.DataCurrTGTAbsBrg;

		UFC.DataCurrTGTDistFeet = Dist*WUTOFT;
		UFC.DataCurrTGTNavDist  = Dist*WUTONM;

		if (PlayerPlane->IfHorzVelocity > 10.0)
			UFC.DataCurrTGTDistTime = (int)((float)UFC.DataCurrTGTDistFeet/(float)PlayerPlane->IfHorzVelocity);
		else
			UFC.DataCurrTGTDistTime = 0;

		UFC.DataCurrTGTTimeHr  =  UFC.DataCurrTGTDistTime/3600;
		UFC.DataCurrTGTTimeMin = (UFC.DataCurrTGTDistTime%3600)/60;
		UFC.DataCurrTGTTimeSec =  UFC.DataCurrTGTDistTime% 60;
	}

	// Current WayPoint
  FPointDouble WayPointLoc;

	WayPointLoc.SetValues(PlayerPlane->AI.CurrWay->lWPx*FTTOWU,0,PlayerPlane->AI.CurrWay->lWPz*FTTOWU);

	UFC.DataCurrSP = (int)(PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]);
	UFC.DataCurrSPBrg =  AIComputeWayPointHeading(PlayerPlane, &Dist, &Dx, &Dy, &Dz, 0);

	if (UFC.DataCurrSPBrg < 0.0f)
		UFC.DataCurrSPBrg += 360.0f;

	UFC.DataCurrSPMilBrg = 360.0 - UFC.DataCurrSPBrg;

	if(UFC.DataCurrSPMilBrg == 360.0)
		UFC.DataCurrSPMilBrg = 0.0;

	// Distance to Navigation Point
	Dist = Dist2D(&PlayerPlane->WorldPosition,&WayPointLoc);
	UFC.DataCurrSPDistFeet = Dist*WUTOFT;
	UFC.DataCurrSPNavDist  = Dist*WUTONM;

	//Dx = (float)(PlayerPlane->WorldPosition.X - PlayerPlane->AI.WayPosition.X);
    //Dz = (float)(PlayerPlane->WorldPosition.Z - PlayerPlane->AI.WayPosition.Z);

	// Seconds away from Waypoint
	if (PlayerPlane->IfHorzVelocity > 10.0)
		TSeconds = (int)((float)UFC.DataCurrSPDistFeet/(float)PlayerPlane->IfHorzVelocity);
	else
		TSeconds = 0;

	UFC.DataCurrSPEteHr  =  TSeconds/3600;
	UFC.DataCurrSPEteMin = (TSeconds%3600)/60;
	UFC.DataCurrSPEteSec =  TSeconds% 60;

	WrkSeconds = (int)WorldParams.WorldTime + TSeconds;
	if (WrkSeconds >= 86400)
		WrkSeconds -= 86400;

	if (UFC.DataCurrTimeMode == ZULU)
	{
		WrkSeconds -= 10800;
		if (WrkSeconds < 0)
			WrkSeconds += 86400;
	}

	UFC.DataCurrSPEtaHr  =  WrkSeconds/3600;
	UFC.DataCurrSPEtaMin = (WrkSeconds%3600)/60;
	UFC.DataCurrSPEtaSec =  WrkSeconds% 60;

	//*************************************************************
	// Do TACAN calculations
	//*************************************************************

	if ((g_iNumTACANObjects) && (UFC.CurrentTacanPtr))
	{
		SetCurrentTacanPos();

		Dx = (PlayerPlane->WorldPosition.X - UFC.TacanPos.X);
		Dy = (PlayerPlane->WorldPosition.Y - UFC.TacanPos.Y);
		Dz = (PlayerPlane->WorldPosition.Z - UFC.TacanPos.Z);

		if (UFC.CurrentTacanPtr->iType == GROUNDOBJECT)
			Dy = 0.0;

		float AbsBrg = AIComputeHeadingToPoint(PlayerPlane, UFC.TacanPos, &Dist, &Dx ,&Dy, &Dz, 0);
		UFC.TacanBearing = AbsBrg;

		if (AbsBrg < 0.0f) AbsBrg += 360.0f;
		AbsBrg = 360.0f - AbsBrg;
		if (AbsBrg == 360.0f) AbsBrg = 0.0f;
		UFC.TacanMilBearing = AbsBrg;

		UFC.TacanRelBearing = -AIComputeHeadingToPoint(PlayerPlane, UFC.TacanPos, &Dist, &Dx ,&Dy, &Dz, 1);
		UFC.TacanDistFeet = Dist*WUTOFT;
		UFC.TacanNavDist  = Dist*WUTONM;

		// Seconds away from Waypoint
		if (PlayerPlane->IfHorzVelocity > 10.0)
			TSeconds = (int)((float)UFC.TacanDistFeet/(float)PlayerPlane->IfHorzVelocity);
		else
			TSeconds = 0;

		UFC.TacanEteHr  =  TSeconds/3600;
		UFC.TacanEteMin = (TSeconds%3600)/60;
		UFC.TacanEteSec =  TSeconds% 60;

		WrkSeconds = (int)WorldParams.WorldTime + TSeconds;
		if (WrkSeconds >= 86400)
			WrkSeconds -= 86400;

		if (UFC.DataCurrTimeMode == ZULU)
		{
			WrkSeconds -= 10800;
			if (WrkSeconds < 0)
				WrkSeconds += 86400;
		}
		UFC.TacanEtaHr  =  WrkSeconds/3600;
		UFC.TacanEtaMin = (WrkSeconds%3600)/60;
		UFC.TacanEtaSec =  WrkSeconds% 60;
	}

	UFC.LawWithinConstraints = !(((UFC.DegRoll > 60) && (UFC.DegRoll < 300)) ||
							  	 ((UFC.DegPitch > 20) && (UFC.DegPitch < 340)));

	void DoAltitudeSoundWarning(PlaneParams *P, int Alt);
	if ((PlayerPlane->Status & PL_DEVICE_DRIVEN) && UFC.LawWithinConstraints)
		DoAltitudeSoundWarning(PlayerPlane, PlayerPlane->AGL);
	else
		DisplayWarning(LOW_ALT, OFF, 0);

	#define MILE  (5280.0)

	if (Av.Hsi.AutoOn)
	{
		float StrTripDist = (UFC.APStatus == OFF) ? (MILE*0.5) : MILE;

		if ((UFC.DataCurrSPDistFeet < StrTripDist) && (!(PlayerPlane->Status & PL_AI_DRIVEN)))
		{
			AINextWayPoint(PlayerPlane);				// advance to next waypoint
			UFCAdvanceWaypointVisual();
		}
	}

	if (UFC.TFStatus)
		DisplayWarning(AUTO_ALERT,ON,0);

	if ((UFC.SteeringMode != TGT_STR) && (UFC.MasterMode == AG_MODE) &&
		((PlayerPlane->AGDesignate.X != -1) && (PlayerPlane->AGDesignate.Z != -1)))
	{
		if (!TGTLatch)
		{
			UFC.SteeringMode = TGT_STR;
			DisplayStrMode();
		}
	}
	else

	if ((UFC.SteeringMode == TGT_STR) && (UFC.MasterMode == AG_MODE) &&
		((PlayerPlane->AGDesignate.X == -1) || (PlayerPlane->AGDesignate.Z == -1)))
	{
			UFC.SteeringMode = NAV_STR;
 			TGTLatch = 0;
			DisplayStrMode();
	}
	else

	if ((UFC.SteeringMode == TGT_STR) && (UFC.MasterMode != AG_MODE))
	{
			UFC.SteeringMode = NAV_STR;
			DisplayStrMode();
	}
}
//***********************************************************************************************************************************
void UFCTestFunction(void)
{
	// dummy function
}
//***********************************************************************************************************************************
void ToggleMasterArm()
{
  UFC.MasterArmState = !UFC.MasterArmState;
  Av.Flir.LaserState = FLIR_LSR_ARMED;
  lLaserStatusFlags &= ~WSO_LASER_IS_ON;
}