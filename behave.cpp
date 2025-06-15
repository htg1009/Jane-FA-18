//*****************************************************************************************
//  BEHAVE.CPP
//*****************************************************************************************

#define	MBBEHAVE	1
#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"
#include "SkunkNet.h"
#include "flightfx.h"
#include "particle.h"
#include "snddefs.h"

extern "C" __declspec( dllimport ) int NetCheckForMultiCast(int flags);

//  copied from avmodes.cpp
typedef struct
{
	char Label[20];
	int  Index;
	int  Category;
	int  MessageIndex;
} BitType;

extern char		g_szExePath[260];

#define BEHAVETEMP 1
//			lpDD->FlipToGDISurface();
#define SREXYZZY 1

#define BOMBTRAILDIST 180960  //  180960 /*30nm*/ //  60320 /*10nm*/  //  90480  /*15nm*/
#define ATTACKALTSPACING 50

//#define SREDEBUGFILE
#ifdef SREDEBUGFILE
int DebugFileSRE;
int SREFILEOpen = 0;
#endif

int accmultval = 4;
extern int		TimeExcel;
extern int netsendanyway;

extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
extern DBRadarType *GetRadarPtr(long id);
extern int GetAirframeCount (RESOURCEFILE *);
extern int GetAircraftIndex (RESOURCEFILE *pRes, int nRequestedIndex);
extern BOOL DoingSimSetup;
extern BYTE WarHeadTypeConversion[];
void StartWingConTrails(PlaneParams *P);
void StopWingConTrails(PlaneParams *P);
void setup_no_cockpit_art();
extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
extern int BombStations[];
extern CameraInstance	 OldCamera;
extern int OldCockpitSeat, OldCurrentCockpit;
int AICheckForVehicleSAR(MovingVehicleParams *vehiclepnt);
extern BOOL	g_bIAmHost;
extern int clearpackets;
extern int netsendanyway;
extern DBWeaponType *get_weapon_ptr(long id);
extern long lFFBaseGMag;
extern void InitBit();
extern BitType *BitDamageList[AV_MAX_DAMAGE];
extern BitType BitList[];
extern CPitType Cpit;
extern class Timer HudFlashTimer;
extern class Timer  RocketCCIPTimer;
extern WeapStoresType WeapStores;

extern long lShowLostTimer;
extern float	fLast_Rmax;
extern long lHARMToF;
extern long lHARMLaunched;
extern AvionicsType Av;
extern float fBolterAdjust;
extern BYTE	bBallCalled;
extern int iNetBulletHit;

WeaponParams *cplaceholder = NULL;

extern int OurPlaneTakingOffFromRunway;
void	WriteDumpLog (char *pszString);
//***********************************************************************************************************************************
void LogError(char *sErrorDesc, char *sFile, int iLine, int iTerminate)
{
	char err[512];

    sprintf(err,"%s %s %d",sErrorDesc,sFile,iLine);

	if (iTerminate)
	{
	    MessageBox(hwnd, err, "Error - Exiting Game", MB_OK);
		sprintf (err, "LogError exiting program! Error = %s, File = %s, Line = %d\n", sErrorDesc, sFile, iLine);
		WriteDumpLog (err);
		exit(-1);
	} else {
	    MessageBox(hwnd, err, "Warning", MB_OK);
	}
}


//**************************************************************************************
void AIInitGlobals(int firsttime)
{
	int cnt, cnt2;
	FPoint clear;

	//  At least some of these will eventually be gotten from a file.
	iNumRadioDisplay = 3;
	iNumRadioHistDisplay = 10;
	if(GetRegValueL("fore3") == 1)
		iRadioDisplayFlags = 6;
	else
		iRadioDisplayFlags = 2;

	if(g_Settings.gp.dwCheats & GP_CHEATS_RADIO)
		iRadioDisplayFlags &= ~4;
	else
		iRadioDisplayFlags |= 4;


	lRadioDisplayDur = 5000;
//	iMsgBoxColor = 199;
	iMsgBoxColor = 53;
	iMsgTextColor = 207;
//	iNoSelectTextColor = 201;
	iNoSelectTextColor = 54;
	iAIShowRadarRange = 0;
	lEventMinuteTimer = 0;
	lEventMinutesPassed = -1;
	lAreaEventTimer = -1000;
	lAreaEventOccuranceFlags = 0;
	l2AreaEventOccuranceFlags = 0;
	iAIHumanLanding = 0;
	lAIHumanLandingTimer = -1;
	lAIHumanAWACSTimer = -1;

	iFACState = 0;
	iFACTargetPos = -1;
	lFACTimer = -1;
	fpFACPosition.SetValues(-1.0f, -1.0f, -1.0f);
	lBombTimer = 0;
	lBombFlags = 0;
	lWSOSpeakTimer = -1;
	iUseBullseye = 1;
	lAWACSPictTimer = -1;
	iAIHumanTankerFlags = 0;
	iCurrEyePoint = -1;
	lJSTARSFlags = 0;
	lJSTARSDead = 0;
	iJSTARSTarget = 0;
	pChaffType = NULL;
	pFlareType = NULL;
	pEjectioSeatType = NULL;
	pFriendlyChuteType = NULL;
	pEnemyChuteType = NULL;
	pGuyOnGroundType = NULL;
	pLandingGateType = NULL;
	lPlayerSARTimer = -1;
	lPlayerDownedTimer = -1;
	lFenceCheckTimer = 17000;
	iFenceCnt = 0;
	iFenceIn = 0;
	pPadLockTarget = NULL;
	fpdPadLockLocation.SetValues(-1.0f, -1.0f, -1.0f);
	iPadLockType = 0;
	pLastWSOWarnObj = NULL;
	iLastWSOWarnType = 0;
	lPadLockTimer = -1;
	lPadLockFlags = 0;
	iInJump = 0;
	lJumpTimer = 0;
	pJumpStr = NULL;
	lNoticeTimer = -1;
	lPlayerGroupTimer = -1;
	cAAAFiring = 0;
	cCurrentLook = 0;

	for(cnt = 0; cnt < MAX_DELAYED_DAMAGE; cnt ++)
	{
		gDamageEvents[cnt].iPlaneNum = -1;
		gDamageEvents[cnt].lDamageTimer = -1;
		gDamageEvents[cnt].lDamagedSystem = 0;
	}
	for(cnt2 = 0; cnt2 < MAX_PLAYERS; cnt2 ++)
	{
		for(cnt = 0; cnt < 40; cnt ++)
		{
			cPlayerArmor[cnt2][cnt] = (char)125;  //  25 total armor points per item * 5;
			cPlayerDamage[cnt2][cnt] = 0;
		}
	}
	lNoBulletHitMsgs = -1;
	lNoOtherBulletHitMsgs = -1;
	lNoCheckPlanelMsgs = -1;
	lLaserStatusFlags = 0;
	lLastAverageMovePlane = -1;
	iAISortNumber = 0;
	pCurrentGBU = NULL;
	iDoAllAAA = 0;
	dLastPlayerV = 0;
	iPlayerRunwayID = 0;
	lPlayerRunwayVoice = SPCH_TOWER1;
	lFenceFlags = 0;
	pPlayerChute = NULL;
	lEasyBomberTimer = -1;
	iEasyBombVar = 0;
	lEasyAATimer = -1;
	iEasyAAVar = 0;
	iEndGameState = 0;
	iMaxEnemyGrndSkill = 0;
	lLeftOrgTimeToFire = -1;
	lLeftTimeToFire = -1;
	lRightOrgTimeToFire = -1;
	lRightTimeToFire = -1;
	lLOrgAMADTimeToFire = -1;
	lLAMADTimeToFire = -1;
	lROrgAMADTimeToFire = -1;
	lRAMADTimeToFire = -1;
	lLeftTimeToExplode = 90000;
	lRightTimeToExplode = 90000;
	lLAMADTimeToExplode = 180000;
	lRAMADTimeToExplode = 180000;
	lFireFlags = 0;
	WingNumDTWSTargets = 0;
	for(cnt = 0; cnt < 8; cnt ++)
	{
		WingDTWSTargets[cnt] = -1;
	}
	lWSOgTimer = -1;
	lDogSpeechTimer = -1;
	lDefensiveTimer = 0;

	clear.SetValues(0.0f, 0.0f, 0.0f);
	for(cnt = 0; cnt < 8; cnt ++)
	{
		fpPlayerGroupFormation[cnt] = clear;
	}
	lNetSpeechTimer = -1;
	lPlayerBulletCount = 0;
	cPlayerVulnerable = 1;
	cPlayerLimitWeapons = 1;

	if((((g_Settings.gp.dwCheats & GP_CHEATS_INVULNERABLE)) && (g_nMissionType != SIM_CAMPAIGN)) || (g_nMissionType == SIM_TRAIN))
	{
		cPlayerVulnerable = 0;
	}

	if((((g_Settings.gp.dwCheats & GP_CHEATS_UNLIMITED_AMMO )) && (g_nMissionType != SIM_CAMPAIGN)) || (g_nMissionType == SIM_TRAIN))
	{
		cPlayerLimitWeapons = 0;
	}

	char *pszCheck = GetRegValue ("campaigncheat");
	if (pszCheck)
	{
		if (g_Settings.gp.dwCheats & GP_CHEATS_INVULNERABLE)
		{
			cPlayerVulnerable = 1;
		}

		if (g_Settings.gp.dwCheats & GP_CHEATS_UNLIMITED_AMMO)
		{
			cPlayerLimitWeapons = 1;
		}
	}


	iAAAFireAlreadyChecked = 0;
	lAfterJump = 0;
	lVisualOverloadTimer = -1;
	lViewTimer = -1;
	lForceEndTimer = -1;
	lMaxTimeToFire = 0;
	lAINetFlags1 = 0;

	lAdvisorFlags = 0;
//	lAdvisorFlags = ADV_TAKEOFF|ADV_MISSILE|ADV_WAYPT|ADV_TARGET;
//	if(g_Settings.gp.dwCheats & GP_CHEATS_TAKEOFF_SPEECH)
//	{
//		lAdvisorFlags |= ADV_TAKEOFF;
//	}

	if(g_Settings.gp.dwCheats & GP_CHEATS_NAV_SPEECH)
	{
		lAdvisorFlags |= ADV_WAYPT;
		lAdvisorFlags |= ADV_TAKEOFF;
	}

	if(g_Settings.gp.dwCheats & GP_CHEATS_TARG_SPEECH)
	{
		lAdvisorFlags |= ADV_MISSILE|ADV_TARGET;
	}

	if(g_Settings.gp.dwCheats & GP_CHEATS_VOICE_WARNINGS)
	{
		lAdvisorFlags |= ADV_CASUAL_BETTY;
	}

	lAdvisorTimer = 15000;
	AdvisorLastPitch = 0;
	fAdvisorLastThrottle = 0;

	lFFBaseGMag = GetRegValueL("ffbaseg");

	if(lFFBaseGMag <= 0)
	{
		lFFBaseGMag = 50;
	}
	else if(lFFBaseGMag > 90)
	{
		lFFBaseGMag = 90;
	}

	HudFlashTimer.Disable();
	RocketCCIPTimer.Disable();
	WeapStores.FloodTimer.Disable();
  	Av.Flir.TimpactTimer.Set(0,0);
  	Av.Flir.TimpactTimer.Disable();
   	Av.Flir.TimeToImpact = -5001;

	//  restore bit mpd page
	InitBit();
	BitDamageList[0] = NULL;
	Cpit.MasterCautionOn = 0;
	Cpit.MasterCautionTimer.Disable();
    DisplayWarning(MASTER_CAUTION, OFF, 0);
	InitF18Cockpit();

	BitType *Ptr = &BitList[0];

	cnt=0;
	while(cnt < AV_MAX_DAMAGE)
	{
		Cpit.Damage[cnt] = 0;
		Ptr->MessageIndex = AV_BIT_GO;
		cnt++;
		Ptr++;
	}

	lShowLostTimer = 5000;
	fLast_Rmax = -1.0f;
	AAScanMode = 1;
	lHARMToF = -1;
	lHARMLaunched = -1;
	iNetBulletHit = 0;

	for(cnt = 0; cnt < MAX_PLANES; cnt ++)
	{
		lNoRepeatTimer[cnt] = -1; 
		lNoRepeatFlags[cnt] = 0;
	}
	lCarrierHitMsgTimer = -1;
	lCarrierBirdsAfirmTimer = -1;
	lCarrierVampireTimer = -1;
	lCarrierMsgFlags = 0;
	bBallCalled = 0;
	iReturnACMView = 0;

	if(firsttime)
	{
		pRocket5 = NULL;
		pRocket275 = NULL;
		clearpackets = 1;
		lHostLeftTimer = -1;
		fBolterAdjust = 0;
		double edge_of_world = (double)NumTilesInWorld * TILE_SIZE;
		dHotZoneDivideX = edge_of_world / (double)(iHotZoneMaxX + 1);
		dHotZoneDivideY = edge_of_world / (double)(iHotZoneMaxY + 1);

		if(!MultiPlayer)
		{
			iMultiCast = 0;
		}
		else
		{
			iMultiCast = NetCheckForMultiCast(DPGETCAPS_GUARANTEED);
		}

		for(cnt = 0; cnt < 4; cnt ++)
		{
			lCurrBytes[cnt] = 0;
			lByteTimer[cnt] = 250 + (250 * cnt);
		}
		lMaxBytes = 0;
		lTotalBytes = 0;
		netsendanyway = 5;

		iMultiSides = (MultiPlayer) ? (MS_AGGRESIVE_NEUTRALS|MS_IGNORE_FENCES) : 0;

		for(cnt = 0; cnt < MAX_GATES; cnt ++)
		{
			pLandingGates[cnt] = NULL;
		}
		dGlideSlope = 3.5f;
		iCarrierWatch = -1;
		if(GetRegValueL("fore3") == 1)
			iSpeechMute = 1;
		else
			iSpeechMute = 0;

		dFACRange = 0;
		pFACTarget = NULL;
		iFACTargetType = -1;
		fFACRange = -1.0f;
		iFACHitFlags = 0;
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			NetBuddyLasingData[cnt].laserloc.X = -1.0f;
			NetBuddyLasingData[cnt].laserloc.Y = -1.0f;
			NetBuddyLasingData[cnt].laserloc.Z = -1.0f;
			NetBuddyLasingData[cnt].lasertimer = -1;
			dLastSlotV[cnt] = 0;
			pPlaneLoadUpdate[cnt] = NULL;
			iLoadUpdateStation[cnt] = 0;
			iLoadUpdateStatus[cnt] = 0;
			lLoadUpdateTimer[cnt] = -1;
		}

		for(cnt = 0; cnt < MAX_LSO_WATCH; cnt ++)
		{
			lLSOWatchHist[cnt] = 0;
		}
		lLSORating = 0;
		iLSOWatchHistIndex = 0;
		iLeadPlace = 0;
		iFirstPlace = 0;
		iLastPlace = 7;
		iFirstTimeInFrame = 1;
		iMaxPlanesOnDeck = 27;

#if 1
		if(g_Settings.gr.nNumGroundObjs == GP_REQUIRED_ONLY)
		{
			iMaxPlanesOnDeck = 0;
		}
		else if(g_Settings.gr.nNumGroundObjs == GP_SOME)
		{
			iMaxPlanesOnDeck = 10;
		}
		else
		{
			iMaxPlanesOnDeck = 20;
		}

		if(!MultiPlayer)
		{
			if(iMaxPlanesOnDeck < 1)
			{
				iMaxPlanesOnDeck = 1;
			}
		}
		else if(iMaxPlanesOnDeck < MAX_PLAYERS)
		{
			iMaxPlanesOnDeck = MAX_PLAYERS;
		}
#endif

		long maxdeck = GetRegValueL("maxdeck");
		if(maxdeck > 0)
		{
			iMaxPlanesOnDeck = maxdeck;
		}
		else if(maxdeck < 0)
		{
			iMaxPlanesOnDeck = 0;
		}

		if(g_bIAmHost && MultiPlayer && (g_nMissionType != SIM_CAMPAIGN))
		{
			//  Need to base this off an option and not for campaigns
			if(g_Settings.gp.dwCheats & GP_CHEATS_MULTI_REGEN)
			{
				iAllowRegen = 1;
				NetPutGenericMessage1(PlayerPlane, GM_ALLOW_REGEN);
			}
			else
			{
				iAllowRegen = 0;
			}
		}
		else
		{
			iAllowRegen = 0;
		}
		fAim9SunOffPitch = 0;
		fAim9SunOffYaw = 0;
		lAOAWarnTimer = -1;
		lSREWarnFlags = 0;
		lSREWarnTimer = -1;
		dwSpeedBrakeSnd = 0;
		netsendanyway = 5;
		iNetHasWeather = 0;
		iWingmanRadio = 1;
		iATCRadio = 1;
		iOtherRadio = 1;
		AICInitExternalVars();
	}

	if(GetRegValueL("Can Cause Crash") == 1)
	{
		iAllowMultiPause = 1;
	}
}

//**************************************************************************************
void AIFlyFormation(PlaneParams *planepnt)
{
	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		FlyToWayPoint(planepnt);
		AIUpdateFormation(planepnt);
	}
	else
	{
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AIUpdateFormation(PlaneParams *planepnt)
{
	double angxx, angxz, angzx, angzz;
	double heading, lastheading, lastdesired;
	double planeroll;
	double tangle, cosroll, aoadeg, aoaoffset;

//	heading = DegToRad(AIConvertAngleTo180Degree(planepnt->Heading));
	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	if (planepnt->Status & PL_ACTIVE)
	{
		if (planepnt->Status & PL_DEVICE_DRIVEN)
//		if (planepnt->Status)
		{
			planeroll = AIConvertAngleTo180Degree(planepnt->Roll);
			if(fabs(planeroll) > 10)
			{
				heading = AIConvertAngleTo180Degree(planepnt->Heading);
				lastheading = AIConvertAngleTo180Degree(planepnt->AI.LastHeading);
//				lastdesired = ((double)planepnt->AI.DesiredHeading / DEGREE) - 180;
				lastdesired = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading);
				heading = heading - lastheading;

				heading = AICapAngle(heading);

				heading = heading * FrameRate;
				if(fabs(heading) < 23)  //  20)
				{
					heading = heading * 3.5;  //  4;
				}
				else
				{
					if(heading < 0)
						heading = -80;
					else
						heading = 80;
				}

				heading = AICapAngle(heading);

				if((0 > planepnt->AI.lTimer1) || ((heading < 0) && (lastdesired > 0)) || ((heading > 0) && (lastdesired < 0)))
				{
					planepnt->AI.lTimer1 = 5000;
					planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(heading);
				}
				else if(((heading > 0) && (lastdesired < 0)) || ((heading < 0) && (lastdesired > 0)))
				{
					planepnt->AI.DesiredHeading = 0;
					planepnt->AI.lTimer1 = -1;
				}
				else if(fabs(heading) > fabs(lastdesired))
				{
					planepnt->AI.lTimer1 = 5000;
					planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(heading);
				}
				else if(fabs(heading) < (fabs(lastdesired) / 2))
				{
					planepnt->AI.DesiredHeading = 0;
					planepnt->AI.lTimer1 = -1;
				}
			}
			else
			{
				planepnt->AI.DesiredHeading = 0;
				planepnt->AI.lTimer1 = -1;
			}

			tangle = AIConvertAngleTo180Degree(planepnt->Pitch);
			aoadeg = RadToDeg(planepnt->Alpha);
			cosroll = cos(DegToRad(fabs(planeroll)));

			aoaoffset = cosroll * (double)aoadeg;
			tangle -= aoaoffset;

			planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(tangle);
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIUpdateFormationLoc(&Planes[planepnt->AI.wingman], planepnt->WorldPosition, angxx, angxz, angzx, angzz, planepnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIUpdateFormationLoc(&Planes[planepnt->AI.nextpair], planepnt->WorldPosition, angxx, angxz, angzx, angzz, planepnt);
	}
}

//**************************************************************************************
void AIUpdateFormationLoc(PlaneParams *planepnt, FPointDouble &leadposition, double angxx, double angxz, double angzx, double angzz, PlaneParams *leadplane)
{
	int isOK = 1;
	int placeingroup, leadplace;

	placeingroup = (leadplane->AI.iAIFlags1 & AIFLIGHTNUMS);
	leadplace = (planepnt->AI.iAICombatFlags2 & AI_LEADER_MASK)>>28;

	if(placeingroup != leadplace)
	{
		if(!leadplace)
		{
			if(!((leadplane->AI.winglead < 0) && (leadplane->AI.prevpair < 0)))
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) && (leadplane != PlayerPlane) && ((leadplane->AI.winglead >= 0) || (leadplane->AI.prevpair >= 0)))
	{
		if(planepnt->AI.Behaviorfunc != leadplane->AI.Behaviorfunc)
		{
			isOK = 0;
		}
	}

	if((!(planepnt->AI.iAIFlags1 & AINOFORMUPDATE)) && (planepnt->AI.iAIFlags1 & AIINFORMATION) && (isOK))
	{
		planepnt->AI.WorldFormationPos.X = leadposition.X + ConvertWayLoc((angxx * planepnt->AI.FormationPosition.X) + (angzx * planepnt->AI.FormationPosition.Z));
		planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y);
	//	planepnt->AI.WayPosition.Z = leadposition.Z + ConvertWayLoc((angxz * planepnt->AI.FormationPosition.X) + (angzz * planepnt->AI.FormationPosition.Z));
		planepnt->AI.WorldFormationPos.Z = leadposition.Z + ConvertWayLoc(-(angxz * planepnt->AI.FormationPosition.X) + (angzz * planepnt->AI.FormationPosition.Z));
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIUpdateFormationLoc(&Planes[planepnt->AI.wingman], leadposition, angxx, angxz, angzx, angzz, leadplane);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIUpdateFormationLoc(&Planes[planepnt->AI.nextpair], leadposition, angxx, angxz, angzx, angzz, leadplane);
	}
}

//void FlyToWayPoint(Plane *planepnt)
//**************************************************************************************
void FlyToWayPoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	double rollval;

	if(planepnt->FlightStatus & PL_OUT_OF_CONTROL)
	{
		AIDeathSpiral(planepnt);
		return;
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

//	if(tdist < 1000.0)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		AINextWayPoint(planepnt);
	}
//	AIChangeDir(planepnt, offangle, (dy * 60000 / 6979));
	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
//	planepnt->RollPercentage = 0.75;	//  when max roll was 45.0
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	//	double workval;
	//  workval = 0.008 * offangle;
	//  if(fabs(workval) > 90)
	//		workval = (workval < 0) ? -90 : 90;
	//	planepnt->DesiredRoll = AIConvert180DegreeToAngle(workval);



	return;
}

//**************************************************************************************
float AIComputeWayPointHeading(PlaneParams *planepnt, float *rtdist, float *rdx, float *rdy, float *rdz, int isrelative)
{
	double dx, dy, dz, desiredht;
	MBWayPoints *waypnt;
	double offangle;
	float tdist;

	waypnt = planepnt->AI.CurrWay;

	if(planepnt->AI.WayPosition.X == -1)
	{
		planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(waypnt->lWPy > 0)
		{
			planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
		}
		else
		{
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}
	}

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
	if(waypnt->lWPy > 0)
	{
		dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
#if 0
		if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
		{
			planepnt->AI.FormationPosition.Y = planepnt->AI.WayPosition.Y;
		}
#endif
	}
	else if(AIIsTakeOff(waypnt))
	{
		desiredht = ConvertWayLoc(8000);
		dy = AICheckSafeAlt(planepnt, desiredht) - planepnt->WorldPosition.Y;
#if 0
		if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
		{
			planepnt->AI.FormationPosition.Y = desiredht;
		}
#endif
	}
	else
	{
//		desiredht = (LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER));
//		desiredht = (AICheckHeightInPath(planepnt, 3000) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER));
		desiredht = AICheckHeightInPath(planepnt, 3000);

		dy = (AICheckSafeAlt(planepnt, desiredht) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
#if 0
		if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
		{
			planepnt->AI.FormationPosition.Y = desiredht;
		}
#endif
	}

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	if(isrelative)
	{
		offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

		offangle = AICapAngle(offangle);
	}

	tdist = QuickDistance(dx,dz);
	*rdx = dx;
	*rdy = dy;
	*rdz = dz;
	*rtdist = tdist;
	return((float) offangle);
}

//**************************************************************************************
void AINextWayPoint(PlaneParams *planepnt)
{
	MBWayPoints *waypnt, *orgwaypnt;
	int numwaypts;
	PlaneParams *leadplane;

//	orgwaypnt = waypnt = planepnt->AI.CurrWay;  //  Moved these lower, was having problems with ending fighter sweeps.
//	numwaypts = planepnt->AI.numwaypts;

	if(planepnt == NULL)
		return;

	if(planepnt->AI.CurrWay == NULL)
	{
		if(WIsWeaponPlane(planepnt))
		{
			planepnt->AI.Behaviorfunc = WeaponAsPlane;
		}
		return;
	}

	leadplane = AIGetLeader(planepnt);

//	if((leadplane->Status & PL_AI_DRIVEN) && (leadplane == planepnt))
#if 0
#ifdef __DEMO__
	if((planepnt->Status & PL_AI_DRIVEN) && (leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
#else
	if((leadplane != PlayerPlane) && (leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
#endif
#else
	if((leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
#endif
	{
		if(AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay))
		{
			return;
		}
	}

	if(iInJump)
	{
		if(planepnt == PlayerPlane)
		{
			if((planepnt->AI.CurrWay->bJumpPoint) || (planepnt->AI.numwaypts <= 1))
			{
//				RestoreFromJump();
				iInJump |= 4;
			}
			else
			{
				int startact, endact, actcnt;

				startact = planepnt->AI.CurrWay->iStartAct;
				endact = startact + planepnt->AI.CurrWay->iNumActs;
				for(actcnt = startact; actcnt < endact; actcnt ++)
				{
					if(AIActions[actcnt].ActionID == ACTION_REFUEL)
					{
						RefuelFuelPlane(PlayerPlane);
					}
				}
			}
		}
	}

	orgwaypnt = waypnt = planepnt->AI.CurrWay;
	numwaypts = planepnt->AI.numwaypts;

	if(waypnt->iNumActs == 0)
	{
		if(planepnt->AI.numwaypts > 1)
		{
			planepnt->AI.CurrWay ++;
			planepnt->AI.numwaypts -= 1;
		}
		else
		{
			planepnt->AI.numwaypts = planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts];
			planepnt->AI.numwaypts += 1;
			planepnt->AI.CurrWay = &AIWayPoints[planepnt->AI.startwpts];
		}
	}
	else
	{
		if(planepnt->AI.numwaypts > 1)
		{
			planepnt->AI.CurrWay ++;
			planepnt->AI.numwaypts -= 1;
		}
		else
		{
			planepnt->AI.numwaypts = planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts];
			planepnt->AI.numwaypts += 1;
			planepnt->AI.CurrWay = &AIWayPoints[planepnt->AI.startwpts];
		}
	}
	waypnt = planepnt->AI.CurrWay;

	planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
	planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(waypnt->iSpeed)
	{
		planepnt->AI.lDesiredSpeed = waypnt->iSpeed;
	}

	if(planepnt->AI.lDesiredSpeed <= 100)
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			planepnt->AI.lDesiredSpeed = 350;
		}
		else
		{
			planepnt->AI.lDesiredSpeed = 50;
		}
	}

	if(waypnt->lWPy > 0)
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
	}
	else if(AIIsTakeOff(waypnt))
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(8000);
	}
	else
	{
		if (!DoingSimSetup)
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		else
		{
			OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
			OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
			planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}

	}

	if(planepnt->AI.lPlaneID == 30)
	{
	 	planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(500);
		return;
	}

#ifdef __DEMO__
	if((leadplane == planepnt) && (leadplane->Status & AL_AI_DRIVEN))
#else
	if((planepnt == PlayerPlane) && ((planepnt->AI.CurrWay - AIWayPoints) == (planepnt->AI.startwpts + 1)))
	{
		if(!(planepnt->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
		{
			float tdistnm = (planepnt->WorldPosition - planepnt->AI.WayPosition) * WUTONM;

			if(tdistnm < 10.0f)
			{
				int actcnt, startact, endact;

				startact = planepnt->AI.CurrWay->iStartAct;
				endact = startact + planepnt->AI.CurrWay->iNumActs;
				for(actcnt = startact; actcnt < endact; actcnt ++)
				{
					 if(AIActions[actcnt].ActionID == ACTION_LAND)
					 {
						planepnt->AI.iAICombatFlags1 |= AI_CARRIER_LANDING;
						if(tdistnm < 3.0f)
						{
							planepnt->AI.lVar2 = 9;
						}
						else if(tdistnm < 5.0f)
						{
							planepnt->AI.lVar2 = 8;
						}
						else
						{
							planepnt->AI.lVar2 = 7;
						}
					 }
				}
			}
		}
	}
	else if((leadplane == planepnt) && (leadplane != PlayerPlane) && (!((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))))
#endif
	{
		AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);

		if(planepnt->AI.wingman >= 0)
		{
			AIUpdateWaypointBehavior(planepnt->AI.wingman, orgwaypnt, numwaypts);
		}
		if(planepnt->AI.nextpair >= 0)
		{
			AIUpdateWaypointBehavior(planepnt->AI.nextpair, orgwaypnt, numwaypts);
		}
	}
#ifdef __DEMO__
	else if(leadplane->Status & PL_AI_DRIVEN)
#else
	else if(!((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))  //(leadplane != PlayerPlane)
#endif
	{
		AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);
	}

	if((planepnt == PlayerPlane) && (lBombFlags & WSO_NAV_MSGS) && (!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)))
	{
		lBombTimer = 130;
	}

	if(planepnt == PlayerPlane)
	{
		if(PlayerPlane->Status & PL_AI_DRIVEN)
		{
			UFCAdvanceWaypointVisual();
		}
	}
}

//**************************************************************************************
void AINextWayPointNoActions(PlaneParams *planepnt)
{
	MBWayPoints *waypnt, *orgwaypnt;
	int numwaypts;
	PlaneParams *leadplane;

	if(planepnt == NULL)
		return;

	if(planepnt->AI.CurrWay == NULL)
	{
		if(WIsWeaponPlane(planepnt))
		{
			planepnt->AI.Behaviorfunc = WeaponAsPlane;
		}
		return;
	}

	orgwaypnt = waypnt = planepnt->AI.CurrWay;
	numwaypts = planepnt->AI.numwaypts;

	leadplane = AIGetLeader(planepnt);

	if(waypnt->iNumActs == 0)
	{
		if(planepnt->AI.numwaypts > 1)
		{
			planepnt->AI.CurrWay ++;
			planepnt->AI.numwaypts -= 1;
		}
		else
		{
			planepnt->AI.numwaypts = planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts];
			planepnt->AI.numwaypts += 1;
			planepnt->AI.CurrWay = &AIWayPoints[planepnt->AI.startwpts];
			if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
			{
				iEasyBombVar = 5;
			}
		}
	}
	else
	{
		if(planepnt->AI.numwaypts > 1)
		{
			planepnt->AI.CurrWay ++;
			planepnt->AI.numwaypts -= 1;
		}
		else
		{
			planepnt->AI.numwaypts = planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts];
			planepnt->AI.numwaypts += 1;
			planepnt->AI.CurrWay = &AIWayPoints[planepnt->AI.startwpts];
			if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
			{
				iEasyBombVar = 5;
			}
		}
	}
	waypnt = planepnt->AI.CurrWay;

	planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
	planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(waypnt->lWPy > 0)
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
	}
	else if(AIIsTakeOff(waypnt))
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(8000);
	}
	else
	{
		if (!DoingSimSetup)
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		else
		{
			OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
			OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
			planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}
	}

	if(waypnt->iSpeed)
	{
		planepnt->AI.lDesiredSpeed = waypnt->iSpeed;
	}

	if(planepnt->AI.lDesiredSpeed <= 100)
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			planepnt->AI.lDesiredSpeed = 350;
		}
		else if(planepnt->AI.lDesiredSpeed == 0)
		{
			planepnt->AI.lDesiredSpeed = 50;
		}
	}
}

//**************************************************************************************
void AIChangeDir(PlaneParams *planepnt, double offangle, double dy)
{
	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
}

//**************************************************************************************
ANGLE AIGetDesiredPitchOffdy(double dy, PlaneParams *planepnt)
{
	double goalpitch = 0;
	ANGLE returnpitch;
	double altdiff, altpitch, degpitch;
	float distance = 0;
	int addpitch = 0;
	double instantheight;
	int heightbuffer = 50;

	if(fabs(dy) >  2000)  //  5000)
	{
		goalpitch = (dy < 0) ? -10 : 10;
	}
	else if(fabs(dy) > 165)  //  500)  //  was 500
	{
		goalpitch = (dy < 0) ? -5 : 5;
	}
	else
	{
		goalpitch = dy / 32;  //   / 2;   //  4;  //   100;

		if((fabs(goalpitch) < 0.16) && (fabs(dy) > 2))
		{
			goalpitch = (goalpitch < 0) ? -0.16 : 0.16;
		}
	}

	altdiff = (float)(AIGetMinSafeAlt(planepnt, &distance) + heightbuffer) - (planepnt->WorldPosition.Y * WUTOFT);

	if(((dy > altdiff) && (altdiff > 0)) || ((dy > 0) && (planepnt->AI.CurrWay) && (planepnt->AI.CurrWay->lWPy < 0)))
	{
		altpitch = (atan2(altdiff, (double)fabs(distance)) * 57.2958) + 5;
		if(goalpitch < altpitch)
		{
			planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;
			goalpitch = altpitch;
		}
	}

	if(dy > 100)
	{
		instantheight = (planepnt->WorldPosition.Y - LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z)) * WUTOFT;
		if((planepnt->AI.CurrWay) && (planepnt->AI.CurrWay->lWPy < 0))
		{
			if(instantheight < labs(planepnt->AI.CurrWay->lWPy))
			{
				addpitch = 1;
			}
		}
		else if(instantheight < 100)
		{
			addpitch = 1;
		}
	}

	if(addpitch)
	{
		planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;
		degpitch = (AIConvertAngleTo180Degree(planepnt->Pitch) + 1);

		if(goalpitch < degpitch)
		{
			goalpitch = degpitch;
		}
	}

	if(goalpitch > 85)
	{
		goalpitch = 85;
	}

	returnpitch = AIConvert180DegreeToAngle(goalpitch);

	return(returnpitch);
}

//**************************************************************************************
ANGLE AIGetDesiredRollOffdyaw(PlaneParams *planepnt, double dyaw, int ignoresecofyaw)
{
	double goalroll = 0;
	ANGLE returnroll;
	double maxyaw;
//	double rollperc;
	double secofyaw, secofroll, desiredroll;

//	maxyaw = planepnt->YawRate * OneOverFrameRate;
	maxyaw = planepnt->YawRate;
	if(maxyaw < fabs(dyaw))
	{
		goalroll = (dyaw < 0.0) ? -90 : 90;
		if(!ignoresecofyaw)
		{
			if(planepnt->YawRate)
			{
				secofyaw = fabs(dyaw / planepnt->YawRate);
			}
			else
			{
				secofyaw = 1;
#ifdef _DEBUG
				char errmsg[256];
				sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
			    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
			}
			if(planepnt->MaxRollRate && planepnt->RollPercentage)
			{
				secofroll = fabs(AIConvertAngleTo180Degree(planepnt->Roll) / (planepnt->MaxRollRate * planepnt->RollPercentage));
			}
			else
			{
				secofroll = 1;
#ifdef _DEBUG
				if(!planepnt->MaxRollRate)
				{
					char errmsg[256];
					sprintf(errmsg, "Bad Max Roll Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
				    MessageBox(hwnd, errmsg, "Error", MB_OK);
				}
				if(!planepnt->RollPercentage)
				{
					char errmsg[256];
					sprintf(errmsg, "Bad Roll percentage for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
				    MessageBox(hwnd, errmsg, "Error", MB_OK);
				}
#endif
			}
			if(secofyaw < secofroll)
			{
				goalroll *= (secofyaw / secofroll);
			}
			if(secofyaw < 1.0)
			{
				if(planepnt->YawRate)
				{
					desiredroll = asin(dyaw / planepnt->YawRate);
				}
				else
				{
					desiredroll = 0;
#ifdef _DEBUG
					char errmsg[256];
					sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
				    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
				}
				desiredroll = RadToDeg(desiredroll);
			}
		}
	}
	else
	{
#if 0
		rollperc = (dyaw / maxyaw) / 8;
		goalroll = asin(rollperc);  //  divide by 8 is so we don't overshoot mark
		goalroll = RadToDeg(goalroll);
#else
//		goalroll = asin((dyaw * OneOverFrameRate) / planepnt->YawRate);
		if(planepnt)
		{
			if(planepnt->YawRate)
			{
				goalroll = asin(dyaw / planepnt->YawRate);
			}
			else
			{
				goalroll = 0;
#ifdef _DEBUG
				char errmsg[256];
				sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
			    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
			}
		}
		else
		{
			goalroll = 0;
#ifdef _DEBUG
			char errmsg[256];
			sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
		    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
		}
		goalroll = RadToDeg(goalroll);
#endif
	}
	returnroll = AIConvert180DegreeToAngle(goalroll);

	return(returnroll);
}

//**************************************************************************************
float ConvertWayLoc(long wayloc)
{
#if 0
	double workvar;

	workvar = (double)wayloc;

	workvar /= (double)WUTOFT;
#endif

	return((float)wayloc * FTTOWU);
}

//**************************************************************************************
double AIConvertAngleTo180Degree(ANGLE angleval)
{
	double dworkvar;

	dworkvar = (double)angleval/DEGREE;

	dworkvar = AICapAngle(dworkvar);

	return(dworkvar);
}

//**************************************************************************************
ANGLE AIConvert180DegreeToAngle(double degreeval)
{
	double dworkvar;

	dworkvar = AICapAngle(degreeval);

	dworkvar *= DEGREE;

	return((ANGLE)dworkvar);
}

//**************************************************************************************
void AIInitFirstWayPt()
{
	int objcnt, waycnt, actcnt, startact, endact;
	int formnum;
	double headsin, headcos;
	MBWayPoints *waypnt;
	float degspersec, fworkvar;
	FPoint Eye;
	FPointDouble fptemp;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	float Alt;
	int settakeoff;
	int orgnumwpts;
	PlaneParams *leadplane;

	FormationActionType *pActionFormation;
	TakeOffActionType *pActionTakeOff;
	TakeOffActionType TempActionTakeOff;
	BombTarget *pActionBombTarget;
	AWACSPatternActionType *pAWACSAction;
	CAPActionType *pCAPAction;
	SOJActionType *pSOJAction;
	OrbitActionType *pOrbitAction;
	TankerActionType *pTankerAction;
	HoverActionType *pHoverAction;
	FighterSweepActionType *pSweepAction;
	CASActionType *pCASAction;
	SEADActionType *pSEADAction;
//	SARActionType *pSARAction;
	int cnt;
	float airdensity, stalleffect;

	OurPlaneTakingOffFromRunway = 0;

	for(objcnt = 0; objcnt < iNumAIObjs; objcnt ++)
	{
		if(Planes[objcnt].Status)
		{
			waycnt = Planes[objcnt].AI.startwpts;
			waypnt = &AIWayPoints[waycnt];
			Alt = Planes[objcnt].WorldPosition.Y * WUTOFT;
			if(Alt > 70000)
			{
				Alt = 70000;
			}
			airdensity = sqrt(Pressure[(int)((Alt)/500.0f)]/0.0023769f);

			if((airdensity <= 0) || (airdensity > 1.0f))
			{
				airdensity = 1.0f;
			}

			Planes[objcnt].DesiredSpeed = Planes[objcnt].IndicatedAirSpeed = waypnt->iSpeed;
			Planes[objcnt].BfLinVel.X = (double) (waypnt->iSpeed / (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) / airdensity;
			if(&Planes[objcnt] == PlayerPlane)
			{
				dLastPlayerV = Planes[objcnt].BfLinVel.X;
			}

			if(MultiPlayer)
			{
				for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
				{
					if(iSlotToPlane[cnt] == objcnt)
					{
						dLastSlotV[cnt] = Planes[objcnt].BfLinVel.X;
					}
				}
			}


			Planes[objcnt].AI.lDesiredSpeed = waypnt->iSpeed;

			if(!((&Planes[objcnt] == PlayerPlane) || (Planes[objcnt].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
			{
				if((pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].iStallSpeed) && (Planes[objcnt].IndicatedAirSpeed))
				{
					stalleffect = (Planes[objcnt].IndicatedAirSpeed / (float)pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].iStallSpeed);
					if(stalleffect < 2.0f)
					{
						Planes[objcnt].AI.iAIFlags2 |= (AI_FAKE_AOA);
					}

					stalleffect -= 0.7f;

					if(stalleffect > 1.0f)
					{
						stalleffect = 1.0f;
					}
				}
				else
				{
					stalleffect = 1.0f;
				}

				if(stalleffect < 0.30f)
				{
					stalleffect = 0.30f;
				}
				else if(stalleffect > 1.0f)
				{
					stalleffect = 1.0f;
				}
				airdensity *= stalleffect;

				if(airdensity < 0.2f)
				{
					airdensity = 0.2f;
				}
				Planes[objcnt].YawRate = (float)pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].iMaxYawRate * airdensity;
				Planes[objcnt].MaxPitchRate = 10.0f * airdensity;
				if(Planes[objcnt].DesiredPitch > 0x8000)
				{
					Planes[objcnt].MaxPitchRate *= 3.0f;
					if(Planes[objcnt].MaxPitchRate > 10.0f)
					{
						Planes[objcnt].MaxPitchRate = 10.0f;
					}
				}
				Planes[objcnt].MaxRollRate = 90.0f * airdensity;
				if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
				{
					Planes[objcnt].MaxRollRate /= 3.0f;
				}
			}
			else
			{
				Planes[objcnt].YawRate = (float)pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].iMaxYawRate;
				Planes[objcnt].MaxPitchRate = 10.0f;
				Planes[objcnt].MaxRollRate = 90.0f;
				if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
				{
					Planes[objcnt].MaxRollRate /= 3.0f;
				}
			}

			startact = AIWayPoints[waycnt].iStartAct;
			endact = startact + AIWayPoints[waycnt].iNumActs;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				switch (AIActions[actcnt].ActionID)
				{
					case ACTION_AWACS_PATTERN:
						pAWACSAction = (AWACSPatternActionType *)AIActions[actcnt].pAction;
						Planes[objcnt].AI.iVar1 = 0;
						Planes[objcnt].AI.lVar2 = 0;
						Planes[objcnt].AI.lTimer1 = pAWACSAction->lTimer * 60000;
						AISetUpCAPStart(&Planes[objcnt]);
					break;
					case ACTION_BOMB_TARGET:
						Planes[objcnt].AI.iAIFlags1 &= ~AICANENGAGE;
						pActionBombTarget=(BombTarget *)AIActions[actcnt].pAction;
						switch(pActionBombTarget->iAttackProfile)
						{
							case 1:
								Planes[objcnt].AI.Behaviorfunc = AIGroupLevelBomb;
								break;
							case 2:		//  Will be Pop-up
								Planes[objcnt].AI.Behaviorfunc = AIGroupFlyToPopPoint;
								break;
							case 3:
								Planes[objcnt].AI.Behaviorfunc = AIGroupFlyToLoftPoint;
								break;
							case 4:
								Planes[objcnt].AI.Behaviorfunc = AIGroupFlyToDivePoint;
								break;
							default:
								Planes[objcnt].AI.Behaviorfunc = AIGroupLevelBomb;
								break;
						}
						Planes[objcnt].AI.iVar1 = 0;
	//					AISelectGroupAttackWeapons(&Planes[objcnt], pActionBombTarget);
						AIGetAttackWeapon(&Planes[objcnt], pActionBombTarget);
						AIGetPlaceInAttack(&Planes[objcnt]);
					break;
					case ACTION_CAP:
						pCAPAction = (CAPActionType *)AIActions[actcnt].pAction;
						Planes[objcnt].AI.iVar1 = AIConvert8ToHeading(pCAPAction->iHeading);
						Planes[objcnt].AI.lVar2 = pCAPAction->lRange;
						Planes[objcnt].AI.lTimer1 = pCAPAction->lTimer * 60000;
						AISetUpCAPStart(&Planes[objcnt]);
					break;
					case ACTION_ESCORT:
					break;
					case ACTION_DAMAGE_THIS_OBJECT:
					break;
					case ACTION_FORMON:
					break;
					case ACTION_HOVER:
						pHoverAction = (HoverActionType *)AIActions[actcnt].pAction;;
						Planes[objcnt].AI.Behaviorfunc = AIFlyToHoverPoint;
						Planes[objcnt].BfLinVel.X = 0;
					break;
#if 0
					case ACTION_SAR:
						pSARAction = (SARActionType *)AIActions[actcnt].pAction;;
						Planes[objcnt].AI.Behaviorfunc = AISARWait;
						Planes[objcnt].AI.lTimer1 = pSARAction->lTimer * 60000;
						Planes[objcnt].AI.iVar1 = pSARAction->lFlag;
						Planes[objcnt].AI.lVar2 = pSARAction->lRange;
					break;
#endif
					case ACTION_LAND:
						pActionTakeOff = (TakeOffActionType *)AIActions[actcnt].pAction;

						if(!AICheckPlaneLandingFlyToCarrier(&Planes[objcnt], pActionTakeOff))
						{
							if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
							{
								Planes[objcnt].AI.Behaviorfunc = AIFlyToLandingPoint;
							}
#if 0
							else if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL)
							{
								if(AIGetClosestAirField(&Planes[objcnt], Planes[objcnt].AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 5.0f * NMTOWU, 0))
								{
									pActionTakeOff->dwSerialNumber = TempActionTakeOff.dwSerialNumber;
									pActionTakeOff->lRunwayStartX = TempActionTakeOff.lRunwayStartX;
									pActionTakeOff->lRunwayStartY = TempActionTakeOff.lRunwayStartY;
									pActionTakeOff->lRunwayStartZ = TempActionTakeOff.lRunwayStartZ;
									pActionTakeOff->lRunwayEndX = TempActionTakeOff.lRunwayEndX;
									pActionTakeOff->lRunwayEndY = TempActionTakeOff.lRunwayEndY;
									pActionTakeOff->lRunwayEndZ = TempActionTakeOff.lRunwayEndZ;

									AISetPlaneLandingFlyToField(&Planes[objcnt], pActionTakeOff, runwayheading);
								}
								else
								{
									Planes[objcnt].AI.Behaviorfunc = AIFlyToLandingPoint;
								}
							}
#endif
							else
							{
								fptemp.SetValues(-1.0f,-1.0f,-1.0f);
								if(AIGetClosestAirField(&Planes[objcnt], Planes[objcnt].AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 5.0f * NMTOWU, 1))
								{
									pActionTakeOff->dwSerialNumber = TempActionTakeOff.dwSerialNumber;
									pActionTakeOff->lRunwayStartX = TempActionTakeOff.lRunwayStartX;
									pActionTakeOff->lRunwayStartY = TempActionTakeOff.lRunwayStartY;
									pActionTakeOff->lRunwayStartZ = TempActionTakeOff.lRunwayStartZ;
									pActionTakeOff->lRunwayEndX = TempActionTakeOff.lRunwayEndX;
									pActionTakeOff->lRunwayEndY = TempActionTakeOff.lRunwayEndY;
									pActionTakeOff->lRunwayEndZ = TempActionTakeOff.lRunwayEndZ;
								}
								else if(AIGetClosestAirField(&Planes[objcnt], Planes[objcnt].AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
								{
									pActionTakeOff->dwSerialNumber = TempActionTakeOff.dwSerialNumber;
									pActionTakeOff->lRunwayStartX = TempActionTakeOff.lRunwayStartX;
									pActionTakeOff->lRunwayStartY = TempActionTakeOff.lRunwayStartY;
									pActionTakeOff->lRunwayStartZ = TempActionTakeOff.lRunwayStartZ;
									pActionTakeOff->lRunwayEndX = TempActionTakeOff.lRunwayEndX;
									pActionTakeOff->lRunwayEndY = TempActionTakeOff.lRunwayEndY;
									pActionTakeOff->lRunwayEndZ = TempActionTakeOff.lRunwayEndZ;
								}

								AISetPlaneLandingFlyToField(&Planes[objcnt], pActionTakeOff, runwayheading);
							}
						}
					break;
					case ACTION_MESSAGE:
					break;
					case ACTION_CHANGE_FORMATION:
						pActionFormation = (FormationActionType *)AIActions[actcnt].pAction;

						if((Planes[objcnt].AI.winglead == -1) && (Planes[objcnt].AI.prevpair == -1))
						{
							Planes[objcnt].AI.Behaviorfunc = AIFlyFormation;
							AIInitFormation(&Planes[objcnt], pActionFormation->iFormation, actcnt);
							AIInitFormationStart(&Planes[objcnt]);
						}
					break;
					case ACTION_ALTERNATE_PATH:
					break;
					case ACTION_REFUEL_PATTERN:
						pTankerAction = (TankerActionType *)AIActions[actcnt].pAction;
						Planes[objcnt].AI.lVar2 = 2.5 * NMTOFT;
						Planes[objcnt].AI.lTimer1 = pTankerAction->lTimer * 60000;
						Planes[objcnt].AI.lDesiredSpeed = 300;
						Planes[objcnt].AI.Behaviorfunc = AIFlyToTankerPoint;

						Planes[objcnt].AI.iAICombatFlags2 &= ~(AI_CARRIER_REL);

						if(Planes[objcnt].AI.iAICombatFlags1 & AI_HOME_CARRIER)
						{
							MovingVehicleParams *carrier = &MovingVehicles[Planes[objcnt].AI.iHomeBaseId];

							double dx = (carrier->pStartWP->lX * FTTOWU) - Planes[objcnt].AI.WayPosition.X;
							double dz = (carrier->pStartWP->lZ * FTTOWU) - Planes[objcnt].AI.WayPosition.Z;
							double tdist = QuickDistance(dx, dz) * WUTONM;
							if(tdist < 20.0f)
							{
								Planes[objcnt].AI.iAICombatFlags2 |= (AI_CARRIER_REL);
								Planes[objcnt].AI.WayPosition.X = carrier->WorldPosition.X;
								Planes[objcnt].AI.WayPosition.Z = carrier->WorldPosition.Z;
								AIOffsetFromPoint(&Planes[objcnt].AI.WayPosition, AIConvertAngleTo180Degree(carrier->Heading + 0x6000), 0, 0, -10.0f * NMTOWU);
							}
						}

						if((g_iNumTACANObjects) && (!UFC.CurrentTacanPtr))
						{
							SetDefaultTacanStation();
							if(UFC.CurrentTacanPtr)
							{
								InitUFCMainMenu();
							}
						}
						degspersec = AIGetTurnRateForOrbit(Planes[objcnt].AI.lVar2, Planes[objcnt].AI.lDesiredSpeed, Planes[objcnt].AI.WayPosition.Y * WUTOFT);
						if(Planes[objcnt].YawRate)
						{
							if(degspersec > Planes[objcnt].YawRate)
							{
								degspersec = Planes[objcnt].YawRate;
							}
							fworkvar = asin(degspersec / Planes[objcnt].YawRate);
							fworkvar = RadToDeg(fworkvar);
							Planes[objcnt].AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
						}
						else
						{
							Planes[objcnt].AI.DesiredRoll = 0;
#ifdef _DEBUG
							char errmsg[256];
							sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", objcnt, Planes[objcnt].AI.lPlaneID);
							MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
						}
					break;
					case ACTION_RELEASE_ESCORT:
					break;
					case ACTION_RELEASE_FORMON:
					break;
					case ACTION_ALERT_INTERCEPT:
					case ACTION_TAKEOFF:
						if(AIActions[actcnt].ActionID == ACTION_ALERT_INTERCEPT)  //  Was at bottom but needed higher to set up Ready 5 correctly
						{
							Planes[objcnt].AI.iAIFlags2 |= AIINVISIBLE;
							Planes[objcnt].AI.iAICombatFlags1 |= AI_READY_5;
						}

						pActionTakeOff = (TakeOffActionType *)AIActions[actcnt].pAction;

						Planes[objcnt].LeftThrustPercent = Planes[objcnt].RightThrustPercent = 0.0;
						Planes[objcnt].DesiredSpeed = Planes[objcnt].BfLinVel.X = 0;
						if(&Planes[objcnt] == PlayerPlane)
						{
							dLastPlayerV = Planes[objcnt].BfLinVel.X;
						}
						if(MultiPlayer)
						{
							for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
							{
								if(iSlotToPlane[cnt] == objcnt)
								{
									dLastSlotV[cnt] = Planes[objcnt].BfLinVel.X;
								}
							}
						}

						Planes[objcnt].OnGround = 1;
						Planes[objcnt].Flaps = 30.0;
						Planes[objcnt].FlapsCommandedPos = 30.0;
						Planes[objcnt].Brakes = 1;
						SetLandingGearUpDown(&Planes[objcnt],LOWER_LANDING_GEAR);
						Planes[objcnt].AI.cFXarray[0] = 0;
						Planes[objcnt].AI.cFXarray[1] = 0;
						Planes[objcnt].AI.cFXarray[2] = 0;
						Planes[objcnt].AI.cFXarray[3] = 0;
						if(pActionTakeOff->lFlag == 0)
						{
							Planes[objcnt].AI.lTimer2 = pActionTakeOff->lTimer * 60000;
						}
						else
						{
							Planes[objcnt].AI.lTimer2 = -1;
						}
						//  can set orientation here.
						fptemp.SetValues(-1.0f,-1.0f,-1.0f);
//						if(!(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER))
//						{
//							if(!MultiPlayer)
//							{
								if(!CTCheckForCarrierTakeoff(&Planes[objcnt], fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 1, 1, AIInPlayerGroup(&Planes[objcnt])))
								{
//									if(AIGetClosestAirField(&Planes[objcnt], fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, 20.0f * NMTOWU, 0))
//									if(AIGetClosestAirField(&Planes[objcnt], fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0, 1))
									if(!(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
									{
										if(AIGetClosestAirField(&Planes[objcnt], fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 1, 1, AIInPlayerGroup(&Planes[objcnt])))
										{
											Planes[objcnt].WorldPosition = runwaypos;

											AISetOrientation(&Planes[objcnt], AIConvertAngleTo180Degree(runwayheading));
										}
//										else if(AIGetClosestAirField(&Planes[objcnt], fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, 20.0f * NMTOWU, 1))
										else if(AIGetClosestAirField(&Planes[objcnt], fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 1, 1, AIInPlayerGroup(&Planes[objcnt])))
										{
											Planes[objcnt].WorldPosition = runwaypos;

											AISetOrientation(&Planes[objcnt], AIConvertAngleTo180Degree(runwayheading));
										}
										else
										{
											if(GetRegValueL("rrtraining") != 1)
											{
												AISetOrientation(&Planes[objcnt], 0);
											}
										}
									}
								}
//							}
//						}
						if(AIActions[actcnt].ActionID == ACTION_ALERT_INTERCEPT)
						{
							Planes[objcnt].Status &= ~PL_ACTIVE;
						}
					break;
					case ACTION_SEAD:
						pSEADAction = (SEADActionType *)AIActions[actcnt].pAction;
						Planes[objcnt].AI.Behaviorfunc = AIFlyToCASArea;
						Planes[objcnt].AI.iVar1 = pSEADAction->lRange;
						Planes[objcnt].AI.lVar2 = pSEADAction->lFlag;
						Planes[objcnt].AI.lTimer2 = pSEADAction->lTimer;
						Planes[objcnt].AI.iAIFlags2 |= (AI_CAS_ATTACK|AI_SEAD_ATTACK);
						Planes[objcnt].AI.iAIFlags2 &= ~(AI_SEAD_COVER);
						Planes[objcnt].AI.pGroundTarget = NULL;
					break;
					case ACTION_CAS:
					case ACTION_ANTISHIP:
					case ACTION_ASW:
					case ACTION_FAC:
						Planes[objcnt].AI.iAICombatFlags2 &= ~(AI_ANTI_SHIP|AI_ANTI_SUB|AI_FAC);
						pCASAction = (CASActionType *)AIActions[actcnt].pAction;
						Planes[objcnt].AI.Behaviorfunc = AIFlyToCASArea;
						Planes[objcnt].AI.iVar1 = pCASAction->lRange;
						Planes[objcnt].AI.lVar2 = pCASAction->lFlag;
						Planes[objcnt].AI.lTimer2 = pCASAction->lTimer;
						Planes[objcnt].AI.iAIFlags2 |= AI_CAS_ATTACK;
						Planes[objcnt].AI.iAIFlags2 &= ~(AI_SEAD_ATTACK|AI_SEAD_COVER);
						Planes[objcnt].AI.pGroundTarget = NULL;
						Planes[objcnt].AI.iAICombatFlags2 &= ~(AI_ANTI_SHIP|AI_ANTI_SUB);
						if(AIActions[actcnt].ActionID == ACTION_ANTISHIP)
						{
							Planes[objcnt].AI.iAICombatFlags2 |= (AI_ANTI_SHIP);
						}
						else if(AIActions[actcnt].ActionID == ACTION_ANTISHIP)
						{
							Planes[objcnt].AI.iAICombatFlags2 |= (AI_ANTI_SUB);
						}
						else if(AIActions[actcnt].ActionID == ACTION_FAC)
						{
							Planes[objcnt].AI.iAICombatFlags2 |= (AI_FAC|AI_FAC_JUST_LOOK);
							if((iFACState & 0xFFFF) == 0)
							{
								iFACState |= 0x10000;
							}
						}
					break;
					case ACTION_FIGHTER_SWEEP:
						if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER)
						{
							pSweepAction = (FighterSweepActionType *)AIActions[actcnt].pAction;
							AISetUpSweepAction(&Planes[objcnt], pSweepAction);
						}
						else
						{
							Planes[objcnt].AI.iAICombatFlags2 |= AI_ESCORTABLE;
							Planes[objcnt].AI.Behaviorfunc = FlyDrone;
							Planes[objcnt].AI.OrgBehave = NULL;
						}
					break;
					case ACTION_ORBIT:
						pOrbitAction = (OrbitActionType *)AIActions[actcnt].pAction;
						if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3)
						{
							pAWACSAction = (AWACSPatternActionType *)AIActions[actcnt].pAction;
							Planes[objcnt].AI.iVar1 = pOrbitAction->iUntil;
							Planes[objcnt].AI.lVar2 = 0;
							Planes[objcnt].AI.lTimer1 = pOrbitAction->lTimer * 60000;
							AISetUpCAPStart(&Planes[objcnt]);
						}
						else
						{
							Planes[objcnt].AI.iVar1 = pOrbitAction->iUntil;
							Planes[objcnt].AI.lVar2 = 2.5 * NMTOFT;
							Planes[objcnt].AI.lTimer1 = pOrbitAction->lTimer * 60000;
							if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
							{
								Planes[objcnt].AI.lDesiredSpeed = 80;
							}
							else
							{
								Planes[objcnt].AI.lDesiredSpeed = 350;
							}
							Planes[objcnt].AI.Behaviorfunc = AIFlyToOrbitPoint;
							degspersec = AIGetTurnRateForOrbit(Planes[objcnt].AI.lVar2, Planes[objcnt].AI.lDesiredSpeed, Planes[objcnt].AI.WayPosition.Y * WUTOFT);
							if(degspersec > Planes[objcnt].YawRate)
							{
								degspersec = Planes[objcnt].YawRate;
							}
							if(Planes[objcnt].YawRate)
							{
								fworkvar = asin(degspersec / Planes[objcnt].YawRate);
								fworkvar = RadToDeg(fworkvar);
								Planes[objcnt].AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
							}
							else
							{
								Planes[objcnt].AI.DesiredRoll = 0;
#ifdef _DEBUG
								char errmsg[256];
								sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", objcnt, Planes[objcnt].AI.lPlaneID);
								MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
							}
						}
					break;
					case ACTION_SOJ:
						pSOJAction = (SOJActionType *)AIActions[actcnt].pAction;
						Planes[objcnt].AI.iVar1 = AIConvert8ToHeading(pSOJAction->iHeading);
						Planes[objcnt].AI.lVar2 = 0;
						Planes[objcnt].AI.lTimer1 = pSOJAction->lTimer * 60000;
						Planes[objcnt].AI.iAIFlags2 |= AI_SO_JAMMING;
						for(cnt = 0; cnt < 15; cnt ++)
						{
							if(pDBWeaponList[Planes[objcnt].WeapLoad[cnt].WeapIndex].iWeaponType == 14)
							{
								Planes[objcnt].AI.iAIFlags2 |= (AI_SOJ_ON_BOARD);
								break;
							}
						}
						AISetUpCAPStart(&Planes[objcnt]);
						break;
					case ACTION_VIEW:
						break;
					case ACTION_DRONE:
						Planes[objcnt].AI.Behaviorfunc = FlyDrone;
						Planes[objcnt].AI.OrgBehave = NULL;
						Planes[objcnt].AI.iAICombatFlags2 |= AI_DRONE;
						break;
					case ACTION_ATTACKTARGET:
						AISetUpAttackTarget(&Planes[objcnt], (AttackTargetActionType *)AIActions[actcnt].pAction);
						break;
					case ACTION_SETTRIGGER:
						break;
					case ACTION_RELEASE_CHUTES:
						break;
				}
			}

			if(Planes[objcnt].AI.lPlaneID == 30)
			{
				Planes[objcnt].AI.lAIVoice = -1;
				Planes[objcnt].AI.Behaviorfunc = FlyCruiseMissile;
				if((Planes[objcnt].AI.winglead == -1) && (Planes[objcnt].AI.prevpair == -1))
				{
					AIInitFormation(&Planes[objcnt], 7, -1);
					AIInitFormationStart(&Planes[objcnt]);
				}
			}


			//  Sometime will have to correct for heading;
			if(Planes[objcnt].OnGround)
			{
				if(&Planes[objcnt] == PlayerPlane)
				{
					UFC.TakeOffTime = 0;
				}

				formnum = 0;
				AIGetNumInGroup(&Planes[objcnt], AIGetLeader(&Planes[objcnt]), &formnum);
				if(Planes[objcnt].OnGround == 1)
				{
					headsin = sin(DegToRad((double)Planes[objcnt].Heading / DEGREE));
					headcos = cos(DegToRad((double)Planes[objcnt].Heading / DEGREE));

#if 0
					Planes[objcnt].WorldPosition.X += ConvertWayLoc(headsin * 200.0 * formnum);
					Planes[objcnt].WorldPosition.Z += ConvertWayLoc(headcos * 200.0 * formnum);
#else
#if 0
					if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3))
					{
						Planes[objcnt].WorldPosition.X -= ConvertWayLoc(headsin * (1500.0 - (200.0f * formnum)));
						Planes[objcnt].WorldPosition.Z -= ConvertWayLoc(headcos * (1500.0 - (200.0f * formnum)));
					}
					else if(PlayerPlane == &Planes[objcnt])
					{
						Planes[objcnt].WorldPosition.X -= ConvertWayLoc(headsin * (1400.0 - (100.0f * formnum)));
						Planes[objcnt].WorldPosition.Z -= ConvertWayLoc(headcos * (1400.0 - (100.0f * formnum)));
					}
					else
					{
						Planes[objcnt].WorldPosition.X -= ConvertWayLoc(headsin * (1400.0 - (100.0f * formnum)));
						Planes[objcnt].WorldPosition.Z -= ConvertWayLoc(headcos * (1400.0 - (100.0f * formnum)));
						if(formnum & 1)
						{
							Planes[objcnt].WorldPosition.X += ConvertWayLoc(headcos * (50.0f));
							Planes[objcnt].WorldPosition.Z -= ConvertWayLoc(headsin * (50.0f));
						}
						else
						{
							Planes[objcnt].WorldPosition.X -= ConvertWayLoc(headcos * (50.0f));
							Planes[objcnt].WorldPosition.Z += ConvertWayLoc(headsin * (50.0f));
						}
					}
#else
					if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3))
					{
						Planes[objcnt].WorldPosition.X -= ConvertWayLoc(headsin * (1500.0 - (200.0f * formnum)));
						Planes[objcnt].WorldPosition.Z -= ConvertWayLoc(headcos * (1500.0 - (200.0f * formnum)));
					}
																																			//g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_FLIGHT
					else if(((Planes[objcnt].AI.wingman < 0) && (Planes[objcnt].AI.winglead < 0)) || ((PlayerPlane == &Planes[objcnt]) && (!((1) && (g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_LANDINGS))) && (PlayerPlane->AI.wingman != -1)))
					{
						Planes[objcnt].WorldPosition.X += ConvertWayLoc(headsin * 100.0f * (formnum - 1));
						Planes[objcnt].WorldPosition.Z += ConvertWayLoc(headcos * 100.0f * (formnum - 1));
					}
					else
					{
						Planes[objcnt].WorldPosition.X += ConvertWayLoc(headsin * 100.0f * (formnum - 1));
						Planes[objcnt].WorldPosition.Z += ConvertWayLoc(headcos * 100.0f * (formnum - 1));
						if(formnum & 1)
						{
							Planes[objcnt].WorldPosition.X += ConvertWayLoc(headcos * (50.0f));
							Planes[objcnt].WorldPosition.Z -= ConvertWayLoc(headsin * (50.0f));
						}
						else
						{
							Planes[objcnt].WorldPosition.X -= ConvertWayLoc(headcos * (50.0f));
							Planes[objcnt].WorldPosition.Z += ConvertWayLoc(headsin * (50.0f));
						}
					}
#endif
#endif

		//			if(!(Planes[objcnt].Status & PL_AI_DRIVEN))
		//			{
#if 0
						Eye.X = Planes[objcnt].WorldPosition.X;
						Eye.Y = Planes[objcnt].WorldPosition.Y;
						Eye.Z = Planes[objcnt].WorldPosition.Z;
						LoadTerrain(Eye);
#endif

					GetHeightWithInstances(Planes[objcnt].WorldPosition);
					Planes[objcnt].WorldPosition.Y += Planes[objcnt].Type->GearDownHeight;

		//			}
	//				Planes[objcnt].WorldPosition.Y = LandHeight(Planes[objcnt].WorldPosition.X,Planes[objcnt].WorldPosition.Z);
	//				Planes[objcnt].WorldPosition.Y += Planes[objcnt].Type->GearDownHeight;

		//			OffDiskTerrainInfo->m_Location = Planes[objcnt].WorldPosition;
		//			OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
		//			Planes[objcnt].WorldPosition.Y = OffDiskTerrainInfo->m_Location.Y + Planes[objcnt].Type->GearDownHeight;

	//				Planes[objcnt].OnGround = 1;
				}

				Planes[objcnt].V = 0;
				Planes[objcnt].DesiredSpeed = Planes[objcnt].BfLinVel.X = 0;
				if(&Planes[objcnt] == PlayerPlane)
				{
					dLastPlayerV = Planes[objcnt].BfLinVel.X;
				}

				if(MultiPlayer)
				{
					for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
					{
						if(iSlotToPlane[cnt] == objcnt)
						{
							dLastSlotV[cnt] = Planes[objcnt].BfLinVel.X;
						}
					}
				}


				if(&Planes[objcnt] == PlayerPlane)
				{
					if(!(JoyCap1.wCaps & JOYCAPS_HASZ))
					{
						ForceKeyboardThrust(0.0f);
					}
				}
				Planes[objcnt].Flaps = 30.0;
				Planes[objcnt].FlapsCommandedPos = 30.0;
				Planes[objcnt].Brakes = 1;
				SetLandingGearUpDown(&Planes[objcnt],LOWER_LANDING_GEAR);
				Planes[objcnt].AI.cFXarray[0] = 0;
				Planes[objcnt].AI.cFXarray[1] = 0;
				Planes[objcnt].AI.cFXarray[2] = 0;
				Planes[objcnt].AI.cFXarray[3] = 0;
			}
			else
			{
				headsin = sin(DegToRad((double)Planes[objcnt].Heading / DEGREE));
				headcos = cos(DegToRad((double)Planes[objcnt].Heading / DEGREE));

				Planes[objcnt].WorldPosition.X += ConvertWayLoc((headcos * Planes[objcnt].AI.FormationPosition.X) + (headsin * Planes[objcnt].AI.FormationPosition.Z));
				Planes[objcnt].WorldPosition.Y += ConvertWayLoc(Planes[objcnt].AI.FormationPosition.Y);
				Planes[objcnt].WorldPosition.Z += ConvertWayLoc(-(headsin * Planes[objcnt].AI.FormationPosition.X) + (headcos * Planes[objcnt].AI.FormationPosition.Z));

	//			Planes[objcnt].WorldPosition.X += ConvertWayLoc(Planes[objcnt].AI.FormationPosition.X);
	//			Planes[objcnt].WorldPosition.Y += ConvertWayLoc(Planes[objcnt].AI.FormationPosition.Y);
	//			Planes[objcnt].WorldPosition.Z += ConvertWayLoc(Planes[objcnt].AI.FormationPosition.Z);
			}
		}
	}

	for(objcnt = 0; objcnt < iNumAIObjs; objcnt ++)
	{
		if(Planes[objcnt].OnGround)
		{
			settakeoff = 1;
			if(&Planes[objcnt] == PlayerPlane)
			{
				PlayerPlane->AI.iAICombatFlags1 |= AI_CARRIER_LANDING;
				if(PlayerPlane->Status & PL_DEVICE_DRIVEN)
				{
					PlayerPlane->AI.lVar2 = 100;
				}
			}

			if(pDBAircraftList[Planes[objcnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
			{
				startact = Planes[objcnt].AI.CurrWay->iStartAct;
				endact = startact + Planes[objcnt].AI.CurrWay->iNumActs;
				for(actcnt = startact; actcnt < endact; actcnt ++)
				{
					if(AIActions[actcnt].ActionID == ACTION_SAR)
					{
						leadplane = AIGetLeader(&Planes[objcnt]);
						if(leadplane->AI.Behaviorfunc != AIFlyTakeOff)
						{
							settakeoff = 0;
						}
					}
				}
			}

			if(settakeoff)
			{
				if(Planes[objcnt].Status & PL_DEVICE_DRIVEN)
				{
					OurPlaneTakingOffFromRunway = 1;

					AIC_Set_Takeoff_Msgs(objcnt);
				}

//				orgnumwpts = Planes[objcnt].AI.numwaypts;
				orgnumwpts = Planes[objcnt].AI.numwaypts + (Planes[objcnt].AI.CurrWay - &AIWayPoints[Planes[objcnt].AI.startwpts]);

				if((Planes[objcnt].AI.winglead == -1) && (Planes[objcnt].AI.prevpair == -1))
				{
//					AINextWayPoint(&Planes[objcnt]);  //  So we don't try to fly back to take off point.
					AINextWayPointNoActions(&Planes[objcnt]);
				}

				if(Planes[objcnt].Status & PL_AI_DRIVEN)
				{
					if((orgnumwpts > 1) || (Planes[objcnt].AI.CurrWay->iNumActs))
					{
						if((orgnumwpts == 2) && (!Planes[objcnt].AI.CurrWay->iSpeed) && (!Planes[objcnt].AI.CurrWay->lWPy))
						{
							PlaneParams *templead = AIGetLeader(&Planes[objcnt]);

							if(!((templead->Status & AL_DEVICE_DRIVEN) || (templead->Status & AL_COMM_DRIVEN) || (templead->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (templead == PlayerPlane)))
							{
								if((AIWayPoints[Planes[objcnt].AI.startwpts].iNumActs == 0) && (AIWayPoints[Planes[objcnt].AI.startwpts + 1].iNumActs == 0))
								{
									Planes[objcnt].AI.Behaviorfunc = FlyDrone;
									Planes[objcnt].AI.OrgBehave = NULL;
									Planes[objcnt].AI.iAICombatFlags2 |= AI_DRONE;
									Planes[objcnt].AI.lDesiredSpeed = 0;
								}
							}
						}
						else if(Planes[objcnt].AI.OrgBehave == NULL)
						{
							Planes[objcnt].AI.OrgBehave = Planes[objcnt].AI.Behaviorfunc;
						}
						if(Planes[objcnt].OnGround == 2)
						{
							if(Planes[objcnt].AI.Behaviorfunc != CTWaitingForLaunch)
							{
								Planes[objcnt].AI.OrgBehave = Planes[objcnt].AI.Behaviorfunc;
							}

							Planes[objcnt].AI.Behaviorfunc = CTWaitingForLaunch;
						}
						else
						{
							if(Planes[objcnt].AI.Behaviorfunc != AIFlyTakeOff)
							{
								Planes[objcnt].AI.OrgBehave = Planes[objcnt].AI.Behaviorfunc;
								Planes[objcnt].AI.Behaviorfunc = AIFlyTakeOff;
							}
						}
					}
					else
					{
//						Planes[objcnt].AI.Behaviorfunc = AIParked;
						Planes[objcnt].AI.Behaviorfunc = FlyDrone;
						Planes[objcnt].AI.OrgBehave = NULL;
						Planes[objcnt].AI.iAICombatFlags2 |= AI_DRONE;
						Planes[objcnt].AI.lDesiredSpeed = 0;
					}
				}
			}
		}
		else if(Planes[objcnt].AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			leadplane = AIGetLeader(&Planes[objcnt]);
			if((leadplane->AI.Behaviorfunc == AIFlyCarrierFinal) || (leadplane->AI.Behaviorfunc == AIPlaneLandingFlyToCarrier) || (leadplane->AI.Behaviorfunc == AIFlyToDME3))
			{
				AISetUpQuickCarrierLanding(&Planes[objcnt]);
			}
		}
	}

	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			objcnt = iSlotToPlane[cnt];
			if(objcnt >= 0)
			{
				NetRegenPlanes[cnt].WorldPosition = Planes[objcnt].WorldPosition;
				NetRegenPlanes[cnt].OnGround = Planes[objcnt].OnGround;
			}
		}
	}
}

//**************************************************************************************
void AIUpdateGroupSpeed(int planenum, double newspeed)
{
	int wingman, nextsec;

	wingman = Planes[planenum].AI.wingman;
	nextsec = Planes[planenum].AI.nextpair;
	if(wingman >= 0)
	{
		Planes[wingman].DesiredSpeed = Planes[wingman].BfLinVel.X = newspeed;
		AIUpdateGroupSpeed(wingman, newspeed);
	}

	if(nextsec >= 0)
	{
		Planes[nextsec].DesiredSpeed = Planes[nextsec].BfLinVel.X = newspeed;
		AIUpdateGroupSpeed(nextsec, newspeed);
	}
}

//**************************************************************************************
void MBAdjustAIThrust(PlaneParams *planepnt, double desiredspeed, int doindicated)
{
	int flags = 0;

	if(doindicated)
	{
		flags = AIDESIREDSPDCALC;
	}
	planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, desiredspeed, flags);
	return;
}

//**************************************************************************************
void MBAISBrakeOn(PlaneParams *planepnt, float percentopen)
{
	float ClipAlt = planepnt->Altitude;
	double IAmod;
	double tspeed;
	double dworkvar;

	if (ClipAlt < 0.0) ClipAlt = 0.0;
	if (ClipAlt > 70000.0) ClipAlt = 70000.0;

	IAmod = sqrt(Pressure[(int)(ClipAlt/500.0)]/0.0023769);

	tspeed = (double)planepnt->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS) * IAmod;


	dworkvar = (double)45.0;

	if(tspeed > 500)
	{
		dworkvar = 10;  //  5.0;
	}
	else if(tspeed > 200)
	{
		dworkvar = ((500.0 - tspeed) * 0.11667) + 10.0;  //  ((500.0 - tspeed) * 0.13333) + 5.0;
		if(dworkvar > 45.0)
			dworkvar = 45.0;
	}
	dworkvar *= (double)percentopen;

	planepnt->SpeedBrakeCommandedPos = (float)dworkvar;
}

//**************************************************************************************
void AISetUpWayPointActions(PlaneParams *planepnt, MBWayPoints *wayptr)
{
	int actcnt, startact, endact;
	float degspersec, fworkvar;
	FormationActionType *pActionFormation;
	TakeOffActionType *pActionTakeOff;
	TakeOffActionType TempActionTakeOff;
	AWACSPatternActionType *pAWACSAction;
	BombTarget *pActionBombTarget;
	CAPActionType *pCAPAction;
	SOJActionType *pSOJAction;
	OrbitActionType *pOrbitAction;
	TankerActionType *pTankerAction;
	HoverActionType *pHoverAction;
	FighterSweepActionType *pSweepAction;
	CASActionType *pCASAction;
	SEADActionType *pSEADAction;
//	SARActionType *pSARAction;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	int cnt;


	planepnt->AI.iAICombatFlags2 &= ~(AI_DRONE|AI_ESCORTABLE);

	startact = wayptr->iStartAct;
	endact = startact + wayptr->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		switch (AIActions[actcnt].ActionID)
		{
			case ACTION_AWACS_PATTERN:
				pAWACSAction = (AWACSPatternActionType *)AIActions[actcnt].pAction;
				planepnt->AI.iVar1 = 0;
				planepnt->AI.lVar2 = 0;
				planepnt->AI.lTimer1 = pAWACSAction->lTimer * 60000;
				AISetUpCAPStart(planepnt);
			break;
			case ACTION_BOMB_TARGET:
				planepnt->AI.iAIFlags1 &= ~AICANENGAGE;
				pActionBombTarget=(BombTarget *)AIActions[actcnt].pAction;
				switch(pActionBombTarget->iAttackProfile)
				{
					case 1:
						planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
						break;
					case 2:		//  Will be Pop-up
						planepnt->AI.Behaviorfunc = AIGroupFlyToPopPoint;
						break;
					case 3:
						planepnt->AI.Behaviorfunc = AIGroupFlyToLoftPoint;
						break;
					case 4:
						planepnt->AI.Behaviorfunc = AIGroupFlyToDivePoint;
						break;
					default:
						planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
						break;
				}
				planepnt->AI.iVar1 = 0;
//				AISelectGroupAttackWeapons(planepnt, pActionBombTarget);
				AIGetAttackWeapon(planepnt, pActionBombTarget);
				AIGetPlaceInAttack(planepnt);
			break;
			case ACTION_CAP:
				pCAPAction = (CAPActionType *)AIActions[actcnt].pAction;
				planepnt->AI.iVar1 = AIConvert8ToHeading(pCAPAction->iHeading);
				planepnt->AI.lVar2 = pCAPAction->lRange;
				planepnt->AI.lTimer1 = pCAPAction->lTimer * 60000;
				AISetUpCAPStart(planepnt);
			break;
			case ACTION_ESCORT:
			break;
			case ACTION_DAMAGE_THIS_OBJECT:
			break;
			case ACTION_FORMON:
			break;
			case ACTION_HOVER:
				pHoverAction = (HoverActionType *)AIActions[actcnt].pAction;;
				planepnt->AI.Behaviorfunc = AIFlyToHoverPoint;
			break;
#if 0
			case ACTION_SAR:
				pSARAction = (SARActionType *)AIActions[actcnt].pAction;;
				planepnt->AI.Behaviorfunc = AISARWait;
				planepnt->AI.lTimer1 = pSARAction->lTimer * 60000;
				planepnt->AI.iVar1 = pSARAction->lFlag;
				planepnt->AI.lVar2 = pSARAction->lRange;
			break;
#endif
			case ACTION_LAND:
				if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
				{
					if((planepnt->AI.iAIFlags1 & (AI_ESCORTED_PLAYER)) && (planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (!(planepnt->AI.iAIFlags1 & AI_RELEASED_PLAYER)))
					{
						AIC_Release_Human_Escort_Msg(planepnt - Planes, PlayerPlane - Planes);
					}
				}
				pActionTakeOff = (TakeOffActionType *)AIActions[actcnt].pAction;
				if(!AICheckPlaneLandingFlyToCarrier(planepnt, pActionTakeOff))
				{
					if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
					{
						planepnt->AI.Behaviorfunc = AIFlyToLandingPoint;
					}
#if 0
					else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL)
					{
						if(AIGetClosestAirField(planepnt, planepnt->AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 5.0f * NMTOWU, 0))
						{
							pActionTakeOff->dwSerialNumber = TempActionTakeOff.dwSerialNumber;
							pActionTakeOff->lRunwayStartX = TempActionTakeOff.lRunwayStartX;
							pActionTakeOff->lRunwayStartY = TempActionTakeOff.lRunwayStartY;
							pActionTakeOff->lRunwayStartZ = TempActionTakeOff.lRunwayStartZ;
							pActionTakeOff->lRunwayEndX = TempActionTakeOff.lRunwayEndX;
							pActionTakeOff->lRunwayEndY = TempActionTakeOff.lRunwayEndY;
							pActionTakeOff->lRunwayEndZ = TempActionTakeOff.lRunwayEndZ;
							AISetPlaneLandingFlyToField(planepnt, pActionTakeOff, runwayheading);
						}
						else
						{
							planepnt->AI.Behaviorfunc = AIFlyToLandingPoint;
						}
					}
#endif
					else
					{
						if(AIGetClosestAirField(planepnt, planepnt->AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 5.0f * NMTOWU, 1))
						{
							pActionTakeOff->dwSerialNumber = TempActionTakeOff.dwSerialNumber;
							pActionTakeOff->lRunwayStartX = TempActionTakeOff.lRunwayStartX;
							pActionTakeOff->lRunwayStartY = TempActionTakeOff.lRunwayStartY;
							pActionTakeOff->lRunwayStartZ = TempActionTakeOff.lRunwayStartZ;
							pActionTakeOff->lRunwayEndX = TempActionTakeOff.lRunwayEndX;
							pActionTakeOff->lRunwayEndY = TempActionTakeOff.lRunwayEndY;
							pActionTakeOff->lRunwayEndZ = TempActionTakeOff.lRunwayEndZ;
						}
						else if(AIGetClosestAirField(planepnt, planepnt->AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 20.0f * NMTOWU, 0))
						{
							pActionTakeOff->dwSerialNumber = TempActionTakeOff.dwSerialNumber;
							pActionTakeOff->lRunwayStartX = TempActionTakeOff.lRunwayStartX;
							pActionTakeOff->lRunwayStartY = TempActionTakeOff.lRunwayStartY;
							pActionTakeOff->lRunwayStartZ = TempActionTakeOff.lRunwayStartZ;
							pActionTakeOff->lRunwayEndX = TempActionTakeOff.lRunwayEndX;
							pActionTakeOff->lRunwayEndY = TempActionTakeOff.lRunwayEndY;
							pActionTakeOff->lRunwayEndZ = TempActionTakeOff.lRunwayEndZ;
						}

						AISetPlaneLandingFlyToField(planepnt, pActionTakeOff, runwayheading);
					}
				}
			break;
			case ACTION_MESSAGE:
			break;
			case ACTION_CHANGE_FORMATION:
				pActionFormation = (FormationActionType *)AIActions[actcnt].pAction;

				if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
				{
					planepnt->AI.Behaviorfunc = AIFlyFormation;
					planepnt->AI.DesiredRoll = 0;
					AIInitFormation(planepnt, pActionFormation->iFormation, actcnt);
				}
			break;
			case ACTION_ALTERNATE_PATH:
			break;
			case ACTION_REFUEL_PATTERN:
				pTankerAction = (TankerActionType *)AIActions[actcnt].pAction;
				planepnt->AI.lVar2 = 2.5 * NMTOFT;
				planepnt->AI.lTimer1 = pTankerAction->lTimer * 60000;
				planepnt->AI.lDesiredSpeed = 300;
				planepnt->AI.Behaviorfunc = AIFlyToTankerPoint;
				planepnt->AI.iAICombatFlags2 &= ~(AI_CARRIER_REL);

				if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
				{
					MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

					double dx = (carrier->pStartWP->lX * FTTOWU) - planepnt->AI.WayPosition.X;
					double dz = (carrier->pStartWP->lZ * FTTOWU) - planepnt->AI.WayPosition.Z;
					double tdist = QuickDistance(dx, dz) * WUTONM;
					if(tdist < 20.0f)
					{
						planepnt->AI.iAICombatFlags2 |= (AI_CARRIER_REL);
						planepnt->AI.WayPosition.X = carrier->WorldPosition.X;
						planepnt->AI.WayPosition.Z = carrier->WorldPosition.Z;
						AIOffsetFromPoint(&planepnt->AI.WayPosition, AIConvertAngleTo180Degree(carrier->Heading + 0x6000), 0, 0, -10.0f * NMTOWU);
					}
				}
				if((g_iNumTACANObjects) && (!UFC.CurrentTacanPtr))
				{
					SetDefaultTacanStation();
					if(UFC.CurrentTacanPtr)
					{
						InitUFCMainMenu();
					}
				}
				degspersec = AIGetTurnRateForOrbit(planepnt->AI.lVar2, planepnt->AI.lDesiredSpeed, planepnt->AI.WayPosition.Y * WUTOFT);
				if(planepnt->YawRate)
				{
					if(degspersec > planepnt->YawRate)
					{
						degspersec = planepnt->YawRate;
					}
					fworkvar = asin(degspersec / planepnt->YawRate);
					fworkvar = RadToDeg(fworkvar);
					planepnt->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
				}
				else
				{
					planepnt->AI.DesiredRoll = 0;
#ifdef _DEBUG
					char errmsg[256];
					sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
				    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
				}
			break;
			case ACTION_RELEASE_ESCORT:
			break;
			case ACTION_RELEASE_FORMON:
			break;
			case ACTION_ALERT_INTERCEPT:
			case ACTION_TAKEOFF:
				pActionTakeOff = (TakeOffActionType *)AIActions[actcnt].pAction;

				planepnt->AI.WayPosition.Y = ConvertWayLoc(8000);
			break;
			case ACTION_SEAD:
					pSEADAction = (SEADActionType *)AIActions[actcnt].pAction;
					planepnt->AI.Behaviorfunc = AIFlyToCASArea;
					planepnt->AI.iVar1 = pSEADAction->lRange;
					planepnt->AI.lVar2 = pSEADAction->lFlag;
					planepnt->AI.lTimer2 = pSEADAction->lTimer;
					planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK);
					planepnt->AI.iAIFlags2 &= ~(AI_SEAD_COVER);
					planepnt->AI.pGroundTarget = NULL;
			break;
			case ACTION_CAS:
			case ACTION_ANTISHIP:
			case ACTION_ASW:
			case ACTION_FAC:
					planepnt->AI.iAICombatFlags2 &= ~(AI_ANTI_SHIP|AI_ANTI_SUB|AI_FAC);
					pCASAction = (CASActionType *)AIActions[actcnt].pAction;
					planepnt->AI.Behaviorfunc = AIFlyToCASArea;
					planepnt->AI.iVar1 = pCASAction->lRange;
					planepnt->AI.lVar2 = pCASAction->lFlag;
					planepnt->AI.lTimer2 = pCASAction->lTimer;
					planepnt->AI.iAIFlags2 |= AI_CAS_ATTACK;
					planepnt->AI.iAIFlags2 &= ~(AI_SEAD_ATTACK|AI_SEAD_COVER);
					planepnt->AI.pGroundTarget = NULL;
					planepnt->AI.iAICombatFlags2 &= ~(AI_ANTI_SHIP|AI_ANTI_SUB);
					if(AIActions[actcnt].ActionID == ACTION_ANTISHIP)
					{
						planepnt->AI.iAICombatFlags2 |= (AI_ANTI_SHIP);
					}
					else if(AIActions[actcnt].ActionID == ACTION_ANTISHIP)
					{
						planepnt->AI.iAICombatFlags2 |= (AI_ANTI_SUB);
					}
					else if(AIActions[actcnt].ActionID == ACTION_FAC)
					{
						planepnt->AI.iAICombatFlags2 |= (AI_FAC|AI_FAC_JUST_LOOK);
						if((iFACState & 0xFFFF) == 0)
						{
							iFACState |= 0x10000;
						}
					}
			break;
			case ACTION_FIGHTER_SWEEP:
				if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER)
				{
					pSweepAction = (FighterSweepActionType *)AIActions[actcnt].pAction;
					AISetUpSweepAction(planepnt, pSweepAction);
				}
				else
				{
					planepnt->AI.iAICombatFlags2 |= AI_ESCORTABLE;
					planepnt->AI.Behaviorfunc = FlyDrone;
					planepnt->AI.OrgBehave = NULL;
				}
			break;
			case ACTION_ORBIT:
				pOrbitAction = (OrbitActionType *)AIActions[actcnt].pAction;
				if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3)
				{
					pAWACSAction = (AWACSPatternActionType *)AIActions[actcnt].pAction;
					planepnt->AI.iVar1 = pOrbitAction->iUntil;
					planepnt->AI.lVar2 = 0;
					planepnt->AI.lTimer1 = pOrbitAction->lTimer * 60000;
					AISetUpCAPStart(planepnt);
				}
				else
				{
					planepnt->AI.iVar1 = pOrbitAction->iUntil;
					planepnt->AI.lVar2 = 2.5 * NMTOFT;
					planepnt->AI.lTimer1 = pOrbitAction->lTimer * 60000;
					if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
					{
						planepnt->AI.lDesiredSpeed = 80;
					}
					else
					{
						planepnt->AI.lDesiredSpeed = 350;
					}
					planepnt->AI.Behaviorfunc = AIFlyToOrbitPoint;
					degspersec = AIGetTurnRateForOrbit(planepnt->AI.lVar2, planepnt->AI.lDesiredSpeed, planepnt->AI.WayPosition.Y * WUTOFT);
					if(planepnt->YawRate)
					{
						if(degspersec > planepnt->YawRate)
						{
							degspersec = planepnt->YawRate;
						}
						fworkvar = asin(degspersec / planepnt->YawRate);
						fworkvar = RadToDeg(fworkvar);
						planepnt->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
					}
					else
					{
						planepnt->AI.DesiredRoll = 0;
#ifdef _DEBUG
						char errmsg[256];
						sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
						MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
					}
				}
			break;
			case ACTION_SOJ:
				pSOJAction = (SOJActionType *)AIActions[actcnt].pAction;
				planepnt->AI.iVar1 = AIConvert8ToHeading(pSOJAction->iHeading);
				planepnt->AI.lVar2 = 0;
				planepnt->AI.lTimer1 = pSOJAction->lTimer * 60000;
				planepnt->AI.iAIFlags2 |= AI_SO_JAMMING;
				for(cnt = 0; cnt < 15; cnt ++)
				{
					if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType == 14)
					{
						planepnt->AI.iAIFlags2 |= (AI_SOJ_ON_BOARD);
						break;
					}
				}
				AISetUpCAPStart(planepnt);
			break;
			case ACTION_VIEW:
				break;
			case ACTION_DRONE:
				planepnt->AI.Behaviorfunc = FlyDrone;
				planepnt->AI.OrgBehave = NULL;
				planepnt->AI.iAICombatFlags2 |= AI_DRONE;
				break;
			case ACTION_ATTACKTARGET:
				AISetUpAttackTarget(planepnt, (AttackTargetActionType *)AIActions[actcnt].pAction);
				break;
			case ACTION_SETTRIGGER:
				break;
			case ACTION_RELEASE_CHUTES:
				break;
		}
	}
}

//**************************************************************************************
int AISetUpWayPointEndActions(PlaneParams *planepnt, MBWayPoints *wayptr)
{
	int actcnt, startact, endact;
	int returnval = 0;
//	float degspersec, fworkvar;
	MessageActionType *pMessageAction;
	DamageActionType *pDamageAction;
	FormOn	*pFormOnAction;
	Escort	*pEscortAction;
	SARActionType *pSARAction;
	ViewActionType *pViewAction;
	SetTriggerActionType *pSetTriggerAction;
	int wingman, nextsec;

	startact = wayptr->iStartAct;
	endact = startact + wayptr->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		switch (AIActions[actcnt].ActionID)
		{
			case ACTION_AWACS_PATTERN:
			break;
			case ACTION_BOMB_TARGET:
			break;
			case ACTION_CAP:
			break;
			case ACTION_ESCORT:
				if(planepnt != PlayerPlane)
				{
					planepnt->AI.iAIFlags1 |= AICANENGAGE;
					pEscortAction = (Escort *)AIActions[actcnt].pAction;

					AISetUpEscort(planepnt, pEscortAction->iPlaneNum);
					returnval = 1;
				}
			break;
			case ACTION_REFUEL:
				if(planepnt != PlayerPlane)
				{
					if(AICheckTankerCloseBy(planepnt, -1))
					{
						AISetUpToTank(planepnt);
						returnval = 1;
					}
				}
			break;
			case ACTION_DAMAGE_THIS_OBJECT:
				if(planepnt != PlayerPlane)
				{
					pDamageAction = (DamageActionType *)AIActions[actcnt].pAction;
					AIDamageObjectGroup(planepnt - Planes, pDamageAction->lDamageAmount);
				}
			break;
			case ACTION_FORMON:
				if(planepnt != PlayerPlane)
				{
#if 1
					pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
					AISetUpFormOnMeeting(planepnt, pFormOnAction->iPlaneNum);
					returnval = 1;
#else
					pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
					AIAddPlaneGroupToFormation(planepnt, pFormOnAction->iPlaneNum);
					returnval = 1;
#endif
				}
			break;
			case ACTION_HOVER:
			break;
			case ACTION_SAR:
				if(planepnt != PlayerPlane)
				{
					planepnt->AI.iAICombatFlags2 &= ~(AI_CARRIER_REL);

					if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
					{
						MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

						double dx = (carrier->pStartWP->lX * FTTOWU) - planepnt->AI.WayPosition.X;
						double dz = (carrier->pStartWP->lZ * FTTOWU) - planepnt->AI.WayPosition.Z;
						double tdist = QuickDistance(dx, dz) * WUTONM;
						if(tdist < 20.0f)
						{
							if((planepnt->AI.CurrWay->lWPy == 0) && ((planepnt->AI.CurrWay - AIWayPoints) == planepnt->AI.startwpts))
							{
								CTGetCarrierTakeOffSpace(planepnt, -2);
							}
							else
							{
								planepnt->AI.iAICombatFlags2 |= (AI_CARRIER_REL);
							}
						}
					}

					pSARAction = (SARActionType *)AIActions[actcnt].pAction;;
					planepnt->AI.Behaviorfunc = AISARWait;
					planepnt->AI.lTimer1 = pSARAction->lTimer * 60000;
					planepnt->AI.iVar1 = pSARAction->lFlag;
					planepnt->AI.lVar2 = pSARAction->lRange;

					returnval = 1;
				}
			break;
			case ACTION_LAND:
			break;
			case ACTION_MESSAGE:
				pMessageAction = (MessageActionType *)AIActions[actcnt].pAction;
				AIDoMessageAction(pMessageAction);
			break;
			case ACTION_CHANGE_FORMATION:
			break;
			case ACTION_ALTERNATE_PATH:
			break;
			case ACTION_REFUEL_PATTERN:
			break;
			case ACTION_RELEASE_ESCORT:
				if(planepnt != PlayerPlane)
				{
					pEscortAction = (Escort *)AIActions[actcnt].pAction;
					if((pEscortAction->iPlaneNum >= 0) && (pEscortAction->iPlaneNum <= (LastPlane - Planes)))
					{
						if(pEscortAction->iPlaneNum == (PlayerPlane - Planes))
						{
							AIC_Release_Human_Escort_Msg(planepnt - Planes, PlayerPlane - Planes);
						}
						else
						{
							AIReleaseEscort(planepnt, pEscortAction->iPlaneNum, pEscortAction->iWayPointNum);
						}
					}
				}
			break;
			case ACTION_RELEASE_FORMON:
				if(planepnt != PlayerPlane)
				{
					pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
					if((pFormOnAction->iPlaneNum >= 0) && (pFormOnAction->iPlaneNum <= (LastPlane - Planes)))
					{
						AIReleasePlaneGroupFromFormation(planepnt, pFormOnAction->iPlaneNum, pFormOnAction->iWayPointNum);
					}
				}
			break;
			case ACTION_TAKEOFF:
			break;
			case ACTION_SEAD:
			break;
			case ACTION_ALERT_INTERCEPT:
			break;
			case ACTION_CAS:
			break;
			case ACTION_FIGHTER_SWEEP:
				if((planepnt != PlayerPlane) && (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
				{
					AICheckSweepConditions(planepnt);
				}
			break;
			case ACTION_ORBIT:
			break;
			case ACTION_SOJ:
				break;
			case ACTION_VIEW:
				if(planepnt->Status & PL_AI_DRIVEN)
				{
					pViewAction = (ViewActionType *)AIActions[actcnt].pAction;
					AIDoViewAction(pViewAction);
				}
				break;
			case ACTION_DRONE:
				break;
			case ACTION_ASW:
				break;
			case ACTION_ANTISHIP:
				break;
			case ACTION_ATTACKTARGET:
				break;
			case ACTION_SETTRIGGER:
				pSetTriggerAction = (SetTriggerActionType *)AIActions[actcnt].pAction;
				if (pSetTriggerAction->lUserFlag<USER_FLAG_START)
					AIChangeEventFlag(pSetTriggerAction->lUserFlag+USER_FLAG_START);
				else
					AIChangeEventFlag(pSetTriggerAction->lUserFlag);
				break;
			case ACTION_RELEASE_CHUTES:
				if(planepnt != PlayerPlane)
				{
					if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						NetPutGenericMessage1(planepnt, GM_DROP_CHUTES);
					}

					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIDropChuteMines);
	//					planepnt->AI.Behaviorfunc = AIDropChuteMines;
						planepnt->AI.lTimer2 = -1;
						wingman = planepnt->AI.wingman;
						nextsec = planepnt->AI.nextpair;
						while((wingman >= 0) || (nextsec >= 0))
						{
							if(wingman >= 0)
							{
								Planes[wingman].AI.lTimer2 = rand() & 0x3FF;
							}
							if(nextsec >= 0)
							{
								Planes[nextsec].AI.lTimer2 = rand() & 0x3FF;
								wingman = Planes[nextsec].AI.wingman;
								nextsec = Planes[nextsec].AI.nextpair;
							}
							else
							{
								wingman = -1;
								nextsec = -1;
							}
						}
						planepnt->AI.cNumWeaponRelease = 0;
						planepnt->AI.cActiveWeaponStation = -1;
					}
				}
				break;
		}
	}

	return(returnval);
}

//**************************************************************************************
void AIInitFormation(PlaneParams *planepnt, long formationid, int actionnum)
{
	int planecnt = 0;
//	PlaneParams *leadplane;

//	if((AIGetLeader(planepnt) == PlayerPlane) && (!AIInPlayerGroup(planepnt)))
	if(PlayerPlane)
	{
		if(((AIGetLeader(planepnt, 1) == AIGetLeader(PlayerPlane, 1)) && (!AIInPlayerGroup(planepnt))) && (iAICommFrom < 0))
		{
			return;
		}
	}

	switch(formationid)
	{
		case 1:
			AIUpdateParadeFormation(planepnt, &planecnt);
			break;
		case 2:
			AIUpdateCruiseFormation(planepnt, &planecnt);
			break;
		case 3:
		case 9:  // Line abreast
			AIUpdateCombatSpreadFormation(planepnt, &planecnt);
			break;
		case 4:
			AIUpdateWallFormation(planepnt, &planecnt);
			break;
		case 5:
			AIUpdateBoxFormation(planepnt, &planecnt);
			break;
		case 6:
			AIUpdateLadderFormation(planepnt, &planecnt);
			break;
		case 7:
			AIUpdateTrailFormation(planepnt, &planecnt);
			break;
		case 10:
			AIUpdateEchelonFormation(planepnt, &planecnt);
			break;
		case 8:
			AIUpdateWedgeFormation(planepnt, &planecnt);
			break;
	}
	return;
}

//**************************************************************************************
void AIUpdateParadeFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int iworkvar;
	int tval;
	float spacing = 62.0f;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if((planepnt->Type->ShadowLRZOff * 1.5f * WUTOFT) > spacing)
		{
			spacing = planepnt->Type->ShadowLRZOff * 1.5f * WUTOFT;
		}

		if((planepnt->Type->ShadowLRXOff * 1.5f * WUTOFT) > spacing)
		{
			spacing = planepnt->Type->ShadowLRXOff * 1.5f * WUTOFT;
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		switch(tval)
		{
			case 0:
				planepnt->AI.FormationPosition.ZeroLength();
				break;
			case 1:
				planepnt->AI.FormationPosition.SetValues(spacing,0.0f,spacing);
				break;
			case 2:
				planepnt->AI.FormationPosition.SetValues(-spacing,0.0f,spacing);
				break;
			case 3:
				planepnt->AI.FormationPosition.SetValues(-spacing * 2.0f,0.0f,spacing * 2.0f);
				break;
			default:
				iworkvar = tval - 3;
				if(iworkvar & 1)
				{
					planepnt->AI.FormationPosition.SetValues(-(spacing * 3.0f) - ((float)(iworkvar>>1) * spacing),0.0f,(spacing * 3.0f) + ((float)(iworkvar>>1) * spacing));
				}
				else
				{
					planepnt->AI.FormationPosition.SetValues(spacing + ((float)(iworkvar>>1) * spacing),0.0f,spacing + ((float)(iworkvar>>1) * spacing));
				}
				break;
		}

		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}


	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateParadeFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateParadeFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateCruiseFormation(PlaneParams *planepnt, int *planecnt, int oneplane)
{
	int wingman, nextsec, winglead, prevsec;
	int iworkvar;
	int tval;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		switch(tval)
		{
			case 0:
				planepnt->AI.FormationPosition.ZeroLength();
				break;
			case 1:
	//			planepnt->AI.FormationPosition.SetValues(100.0f,0.0f,100.0f);
				planepnt->AI.FormationPosition.SetValues(212.0f,0.0f,212.0f);
				break;
			case 2:
	//			planepnt->AI.FormationPosition.SetValues(-100.0f,0.0f,100.0f);
				planepnt->AI.FormationPosition.SetValues(-212.0f,0.0f,212.0f);
				break;
			case 3:
	//			planepnt->AI.FormationPosition.SetValues(-200.0f,0.0f,200.0f);
				planepnt->AI.FormationPosition.SetValues(-424.0f,0.0f,424.0f);
				break;
			default:
				iworkvar = tval - 3;
				if(iworkvar & 1)
				{
	//				planepnt->AI.FormationPosition.SetValues(-300.0f - ((float)(iworkvar>>1) * 100.0f),0.0f,300.0f + ((float)(iworkvar>>1) * 100.0f));
					planepnt->AI.FormationPosition.SetValues(-636.0f - ((float)(iworkvar>>1) * 212.0f),0.0f,636.0f + ((float)(iworkvar>>1) * 212.0f));
				}
				else
				{
	//				planepnt->AI.FormationPosition.SetValues(100.0f + ((float)(iworkvar>>1) * 100.0f),0.0f,100.0f + ((float)(iworkvar>>1) * 100.0f));
					planepnt->AI.FormationPosition.SetValues(212.0f + ((float)(iworkvar>>1) * 212.0f),0.0f,212.0f + ((float)(iworkvar>>1) * 212.0f));
				}
				break;
		}
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}


	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;

	if(!oneplane)
	{
		if(wingman >= 0)
		{
			AIUpdateCruiseFormation(&Planes[wingman], planecnt);
		}

		if(nextsec >= 0)
		{
			AIUpdateCruiseFormation(&Planes[nextsec], planecnt);
		}
	}
}

//**************************************************************************************
void AIUpdateWedgeFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int iworkvar;
	int tval;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		switch(tval)
		{
			case 0:
				planepnt->AI.FormationPosition.ZeroLength();
				break;
			case 1:
	//			planepnt->AI.FormationPosition.SetValues(100.0f,0.0f,100.0f);
				planepnt->AI.FormationPosition.SetValues(212.0f,0.0f,212.0f);
				break;
			case 2:
	//			planepnt->AI.FormationPosition.SetValues(-100.0f,0.0f,100.0f);
				planepnt->AI.FormationPosition.SetValues(-212.0f,0.0f,0.0f);
				break;
			case 3:
	//			planepnt->AI.FormationPosition.SetValues(-200.0f,0.0f,200.0f);
				planepnt->AI.FormationPosition.SetValues(-424.0f,0.0f,212.0f);
				break;
			default:
				iworkvar = tval - 3;
				if(iworkvar & 1)
				{
	//				planepnt->AI.FormationPosition.SetValues(-300.0f - ((float)(iworkvar>>1) * 100.0f),0.0f,300.0f + ((float)(iworkvar>>1) * 100.0f));
					planepnt->AI.FormationPosition.SetValues(-636.0f - ((float)(iworkvar>>1) * 212.0f),0.0f,636.0f + ((float)((iworkvar>>1) - 1) * 212.0f));
				}
				else
				{
	//				planepnt->AI.FormationPosition.SetValues(100.0f + ((float)(iworkvar>>1) * 100.0f),0.0f,100.0f + ((float)(iworkvar>>1) * 100.0f));
					planepnt->AI.FormationPosition.SetValues(212.0f + ((float)(iworkvar>>1) * 212.0f),0.0f,212.0f + ((float)(iworkvar>>1) * 212.0f));
				}
				break;
		}
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}


	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateWedgeFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateWedgeFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateCombatSpreadFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int iworkvar;
	int tval;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		switch(tval)
		{
			case 0:
				planepnt->AI.FormationPosition.ZeroLength();
				break;
			case 1:
				planepnt->AI.FormationPosition.SetValues(350.0f,0.0f,0.0f);
				break;
			case 2:
				planepnt->AI.FormationPosition.SetValues(-350.0f,0.0f,0.0f);
				break;
			case 3:
				planepnt->AI.FormationPosition.SetValues(-700.0f,0.0f,0.0f);
				break;
			default:
				iworkvar = tval - 3;
				if(iworkvar & 1)
				{
					planepnt->AI.FormationPosition.SetValues(-1050.0f - ((float)(iworkvar>>1) * 350.0f),0.0f,0.0f);
				}
				else
				{
					planepnt->AI.FormationPosition.SetValues(350.0f + ((float)(iworkvar>>1) * 350.0f),0.0f,0.0f);
				}
				break;
		}
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}

	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateCombatSpreadFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateCombatSpreadFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateWallFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int iworkvar;
	int tval;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		switch(tval)
		{
			case 0:
				planepnt->AI.FormationPosition.ZeroLength();
				break;
			case 1:
				planepnt->AI.FormationPosition.SetValues(10000.0f,6000.0f,0.0f);
				break;
			case 2:
				planepnt->AI.FormationPosition.SetValues(-10000.0f,3000.0f,0.0f);
				break;
			case 3:
				planepnt->AI.FormationPosition.SetValues(-20000.0f,-3000.0f,0.0f);
				break;
			case 4:
				planepnt->AI.FormationPosition.SetValues(20000.0f,0.0f,0.0f);
				break;
			case 5:
				planepnt->AI.FormationPosition.SetValues(30000.0f,3000.0f,0.0f);
				break;
			case 6:
				planepnt->AI.FormationPosition.SetValues(-30000.0f,6000.0f,0.0f);
				break;
			case 7:
				planepnt->AI.FormationPosition.SetValues(-40000.0f,3000.0f,0.0f);
				break;
			default:
				iworkvar = tval - 7;
				planepnt->AI.FormationPosition.SetValues(3000 + ((float)iworkvar * 1000.0f),(iworkvar & 7) * 1000.0f,0.0f);
				break;
		}
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}

	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateWallFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateWallFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateBoxFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int tval;
	int itempval;
	float zplus;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		itempval = tval>>1;
		zplus = ((float)itempval / 2.0) * 4.0 * NMTOFT;
		switch(tval & 3)
		{
			case 0:
				planepnt->AI.FormationPosition.SetValues(0.0f,0.0f,zplus);
				break;
			case 1:
				planepnt->AI.FormationPosition.SetValues((float)(3.0 * NMTOFT),0.0f,zplus);
				break;
			case 2:
				planepnt->AI.FormationPosition.SetValues(0.0f,-10000.0f,zplus);
				break;
			case 3:
				planepnt->AI.FormationPosition.SetValues((float)(3.0 * NMTOFT),-10000.0f,zplus);
				break;
		}
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}

	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateBoxFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateBoxFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateLadderFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int tval;
	float fplus;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		fplus = tval * 500.0f;

		planepnt->AI.FormationPosition.SetValues(0.0f,-fplus, fplus);

		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}

	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateLadderFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateLadderFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateTrailFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int tval;
	float fplus;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		fplus = tval * NMTOFT;

		planepnt->AI.FormationPosition.SetValues(0.0f,0.0f, fplus);

		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}


	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateTrailFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateTrailFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
void AIUpdateEchelonFormation(PlaneParams *planepnt, int *planecnt)
{
	int wingman, nextsec, winglead, prevsec;
	int tval;
	float fplus;

	if((!MultiPlayer) || (!iLeadPlace) || (((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				planepnt->AI.iAICombatFlags2 |= (iLeadPlace);
		}

		if(planepnt->AI.OrgBehave == NULL)   //   AIFlyTakeOff
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			planepnt->AI.OrgBehave = AIFlyFormation;
		}
		tval = *planecnt;
		fplus = tval * 212.0f;

		planepnt->AI.FormationPosition.SetValues(fplus,0.0f, fplus);

		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			int placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;
			if(placeingroup < 0)
			{
				placeingroup = 0;
			}
			else if(placeingroup > 7)
			{
				placeingroup = 7;
			}

			fpPlayerGroupFormation[placeingroup] = planepnt->AI.FormationPosition;
		}

		*planecnt = tval + 1;
	}

	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateEchelonFormation(&Planes[wingman], planecnt);
	}

	if(nextsec >= 0)
	{
		AIUpdateEchelonFormation(&Planes[nextsec], planecnt);
	}
}

//**************************************************************************************
PlaneParams *AIGetLeader(PlaneParams *planepnt, int gettop)
{
	long leadplace;

	if((MultiPlayer) && (!gettop))
	{
		leadplace = (planepnt->AI.iAICombatFlags2 & AI_LEADER_MASK)>>28;
	}
	else
	{
		leadplace = 0;
	}

	if(planepnt->AI.OrgBehave == AIFlyEscort)
	{
		return(AIGetLeaderEscort(planepnt));
	}

	return(AIGetLeaderNoEscort(planepnt, leadplace));
}

//**************************************************************************************
PlaneParams *AIGetLeaderNoEscort(PlaneParams *planepnt, int leadplace)
{
	int placeingroup;
	PlaneParams *checkplane;

	if(planepnt->AI.prevpair >= 0)
	{
		checkplane = &Planes[planepnt->AI.prevpair];
		placeingroup = (checkplane->AI.iAIFlags1 & AIFLIGHTNUMS);
		if((placeingroup == leadplace) && (leadplace))
		{
			return(checkplane);
		}

		return(AIGetLeaderNoEscort(checkplane, leadplace));
	}

	if(planepnt->AI.winglead >= 0)
	{
		checkplane = &Planes[planepnt->AI.winglead];
		placeingroup = (checkplane->AI.iAIFlags1 & AIFLIGHTNUMS);
		if((placeingroup == leadplace) && (leadplace))
		{
			return(checkplane);
		}

		return(AIGetLeaderNoEscort(checkplane, leadplace));
	}

	return(planepnt);
}

//**************************************************************************************
PlaneParams *AIGetLeaderEscort(PlaneParams *planepnt)
{
	if(planepnt->AI.prevpair >= 0)
	{
		if(planepnt->AI.OrgBehave == AIFlyEscort)
		{
			if(planepnt->AI.iAICombatFlags1 & AI_ESCORT_PART_LEAD)
			{
				return(planepnt);
			}
			if(Planes[planepnt->AI.prevpair].AI.OrgBehave != AIFlyEscort)
			{
				return(planepnt);
			}
		}

		return(AIGetLeaderEscort(&Planes[planepnt->AI.prevpair]));
	}

	if(planepnt->AI.winglead >= 0)
	{
		if(planepnt->AI.OrgBehave == AIFlyEscort)
		{
			if(planepnt->AI.iAICombatFlags1 & AI_ESCORT_PART_LEAD)
			{
				return(planepnt);
			}
			if(Planes[planepnt->AI.winglead].AI.OrgBehave != AIFlyEscort)
			{
				return(planepnt);
			}
		}
		return(AIGetLeaderEscort(&Planes[planepnt->AI.winglead]));
	}

	return(planepnt);
}

//**************************************************************************************
void AIFormationFlying(PlaneParams *planepnt)
{
	BasicInstance *checkobject;
	MovingVehicleParams *vehiclepnt;
	float dx, dy, dz, tdist;
	float offpitch;
	PlaneParams *leader;
	int nocheck;
	void *target;
	int targettype = 0;

	leader = AIGetLeader(planepnt);

//	AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
	AIFormationFlyingLead(planepnt, leader);

	if(((planepnt->AI.Behaviorfunc == AIFlyFormation) || (planepnt->AI.Behaviorfunc == AIFormationFlying)) && (planepnt->AI.lRadarDelay < 1000) && (((planepnt->AI.pPaintedBy) && (planepnt->AI.iPaintedByType >= 0) && (planepnt->AI.iPaintDist >= 0)) || ((leader->AI.pPaintedBy) && (leader->AI.iPaintedByType >= 0) && (leader->AI.iPaintDist >= 0))))
	{
		if(planepnt->AI.iAICombatFlags1 & AI_FORMON_SEAD_SEARCH)
		{
			nocheck = 0;
			if((planepnt->AI.pPaintedBy) && (planepnt->AI.iPaintedByType >= 0) && (planepnt->AI.iPaintDist >= 0))
			{
				if(planepnt->AI.iPaintedByType == GROUNDOBJECT)
				{
					target = planepnt->AI.pPaintedBy;
					targettype = planepnt->AI.iPaintedByType;
					checkobject = (BasicInstance *)planepnt->AI.pPaintedBy;
					if(InstanceIsBombable(checkobject))
					{
						if(!(AICheckIfAlreadyGroundTarget(planepnt, leader, target, planepnt->AI.iPaintedByType) || AIGroundAlreadyTarget(target)))
						{
							planepnt->AI.WayPosition.X = checkobject->Position.X;
							planepnt->AI.WayPosition.Y = checkobject->Position.Y + (500 * FTTOWU);
							planepnt->AI.WayPosition.Z = checkobject->Position.Z;
							planepnt->AI.TargetPos = checkobject->Position;
							nocheck = 1;
						}
					}
					else
					{
						planepnt->AI.pPaintedBy = NULL;
						planepnt->AI.iPaintedByType = -1;
						planepnt->AI.iPaintDist = -1;
					}
				}
				else if(planepnt->AI.iPaintedByType == MOVINGVEHICLE)
				{
					target = planepnt->AI.pPaintedBy;
					targettype = planepnt->AI.iPaintedByType;
					vehiclepnt = (MovingVehicleParams *)planepnt->AI.pPaintedBy;
					if(!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE)))
					{
						if(!(AICheckIfAlreadyGroundTarget(planepnt, leader, target, planepnt->AI.iPaintedByType) || AIGroundAlreadyTarget(target)))
						{
							planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
							planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y + (500 * FTTOWU);
							planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
							planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
							nocheck = 1;
						}
					}
					else
					{
						planepnt->AI.pPaintedBy = NULL;
						planepnt->AI.iPaintedByType = -1;
						planepnt->AI.iPaintDist = -1;
					}
				}
			}
			if(!nocheck)
			{
				if((leader->AI.pPaintedBy) && (leader->AI.iPaintedByType >= 0) && (leader->AI.iPaintDist >= 0))
				{
					if(leader->AI.iPaintedByType == GROUNDOBJECT)
					{
						target = leader->AI.pPaintedBy;
						targettype = leader->AI.iPaintedByType;
						checkobject = (BasicInstance *)leader->AI.pPaintedBy;
						if(InstanceIsBombable(checkobject))
						{
							if(!(AICheckIfAlreadyGroundTarget(planepnt, leader, target, targettype) || AIGroundAlreadyTarget(target)))
							{
								planepnt->AI.WayPosition.X = checkobject->Position.X;
								planepnt->AI.WayPosition.Y = checkobject->Position.Y + (500 * FTTOWU);
								planepnt->AI.WayPosition.Z = checkobject->Position.Z;
								planepnt->AI.TargetPos = checkobject->Position;
								nocheck = 1;
							}
						}
						else
						{
							planepnt->AI.pPaintedBy = NULL;
							planepnt->AI.iPaintedByType = -1;
							planepnt->AI.iPaintDist = -1;
						}
					}
					else if(leader->AI.iPaintedByType == MOVINGVEHICLE)
					{
						target = leader->AI.pPaintedBy;
						targettype = leader->AI.iPaintedByType;
						vehiclepnt = (MovingVehicleParams *)leader->AI.pPaintedBy;
						if(!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE)))
						{
							if(!(AICheckIfAlreadyGroundTarget(planepnt, leader, target, targettype) || AIGroundAlreadyTarget(target)))
							{
								planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
								planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y + (500 * FTTOWU);
								planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
								planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
								nocheck = 1;
							}
						}
						else
						{
							planepnt->AI.pPaintedBy = NULL;
							planepnt->AI.iPaintedByType = -1;
							planepnt->AI.iPaintDist = -1;
						}
					}
				}
			}

			if(!nocheck)
			{
				return;
			}

			planepnt->AI.iAICombatFlags1 |= AI_FORMON_SEAD;

			dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
			dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
			dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

			tdist = QuickDistance(dx, dz);
			offpitch = (atan2(dy, tdist) * 57.2958);

			if(offpitch > -20)
			{
				planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIAwayFromTarget;
			}

			planepnt->AI.pGroundTarget = target;
			planepnt->AI.lGroundTargetFlag = targettype;

			AIDetermineBestBomb(planepnt, planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag);

			if(planepnt->AI.Behaviorfunc == AIGroupLevelBomb)
			{
				planepnt->AI.Behaviorfunc = AILevelBomb;
			}

			planepnt->AI.lDesiredSpeed = planepnt->IndicatedAirSpeed + 100;
		}
	}
}

//**************************************************************************************
void AIFormationFlyingLead(PlaneParams *planepnt, PlaneParams *leadplane)
{
	double dx, dy, dz;
	double offangle, offangle1;
	double tdist, tdistnm;
	float ClipAlt = planepnt->Altitude;
	double IAmod;
	int		doindicated = 1;
	double leadspeed, leadaccel;
	double leadroll, planeroll;
	double leadheading, planehead;
	double dhead, dleadhead, diffhead;
	int inturn = 0;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	long speedmod;
	double circledist, leadradius, workradius;
	float fpsspeed, ftempvar, ftempvar2;
	float tempspeed;
	float ymod;


	planepnt->AI.iAIFlags1 &= ~AINOFORMUPDATE;
	planepnt->AI.iAIFlags1 |= AIINFORMATION;

#if 0  //  Allows planes to "freak out" as if they were shot at.  Need player plane in mission
	if((GetAsyncKeyState('J')) && (planepnt == (PlaneParams *)Camera1.AttachedObject))
	{
	  	PlaneParams *shooter = &Planes[0];
 		while (shooter <= LastPlane)
		{
			if (shooter->Status & PL_DEVICE_DRIVEN)
			{
				break;
			}
			shooter ++;
		}

		AINearMiss(planepnt, shooter);
	}
#endif


	leadroll = AIConvertAngleTo180Degree(leadplane->Roll);
	planeroll = AIConvertAngleTo180Degree(planepnt->Roll);
	leadheading = AIConvertAngleTo180Degree(leadplane->Heading);
	dhead = AIConvertAngleTo180Degree(planepnt->Heading - planepnt->AI.LastHeading);
	dleadhead = AIConvertAngleTo180Degree(leadplane->Heading - leadplane->AI.LastHeading);
	diffhead = AIConvertAngleTo180Degree(planepnt->Heading - leadplane->Heading);

//	if(leadplane == PlayerPlane)
	if((leadplane->Status & PL_DEVICE_DRIVEN) || (leadplane == PlayerPlane) || (leadplane->Status & PL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
	{
		if(!MultiPlayer)
		{
			leadaccel = leadplane->V - dLastPlayerV;
		}
		else
		{
			int slotnum = NetGetSlotFromPlaneIndex(leadplane - Planes);
			if((slotnum < MAX_HUMANS) && (iSlotToPlane[slotnum] >= 0))
			{
				leadaccel = leadplane->V - dLastSlotV[slotnum];
			}
		}
	}
	else
	{
		leadaccel = 0;
	}

	if(doindicated)
	{
		if (ClipAlt < 0.0) ClipAlt = 0.0;
		if (ClipAlt > 70000.0) ClipAlt = 70000.0;

		IAmod = sqrt(Pressure[(int)(ClipAlt/500.0)]/0.0023769);


		leadspeed = (double)(leadplane->V + leadaccel)*(FTSEC_TO_MLHR*MLHR_TO_KNOTS) * IAmod;
	}
	else
	{
		leadspeed = (double)(leadplane->V + leadaccel)*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}

	dx = planepnt->AI.WorldFormationPos.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WorldFormationPos.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx,dz) * WUTOFT;
	tdistnm = tdist * FTTONM;

//	if(planepnt == PlayerPlane)
	if((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
	{
		ymod = 0;
	}
	else if((fabs(diffhead) > 90.0f) || (tdist > 2000))
	{
		ymod = (250.0f + ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) * 50.0f)) * FTTOWU;
	}
	else if(tdist > 540)
	{
		ymod = 200.0f * FTTOWU;
	}
	else if(tdist > 40)
	{
		ymod = (tdist - 40.0f) * 0.02;
	}
	else
	{
		ymod = 0;
	}

	dy = AICheckSafeAlt(planepnt, planepnt->AI.WorldFormationPos.Y - ymod) - planepnt->WorldPosition.Y;

	offangle1 = atan2(-dx, -dz) * 57.2958;
	offangle1= AICapAngle(offangle1);

	planehead= AIConvertAngleTo180Degree(planepnt->Heading);
	offangle1 -= planehead;

	offangle1 = AICapAngle(offangle1);

	tempspeed = planepnt->V;
	if(tempspeed < 40)
	{
		tempspeed = 40;
	}
	if(tdistnm < 2.0)
	{
		if((fabs(offangle1) > 90) && (!(fabs(leadroll) > 3)))
		{
			if(fabs(leadroll) > 3)
			{
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.25))) - planepnt->WorldPosition.X;
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.25))) - planepnt->WorldPosition.Z;
				inturn = 1;
			}
			if(tdistnm < 0.3)
			{
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.75))) - planepnt->WorldPosition.X;
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.75))) - planepnt->WorldPosition.Z;
			}
			else if(tdistnm < 1.0)
			{
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 1.75))) - planepnt->WorldPosition.X;
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 1.75))) - planepnt->WorldPosition.Z;
			}
			else
			{
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 4.375))) - planepnt->WorldPosition.X;
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 4.375))) - planepnt->WorldPosition.Z;
			}
			offangle = atan2(-dx, -dz) * 57.2958;

			if(fabs(leadroll) > 3)
			{
				inturn = 1;
			}
		}
		else
		{
			if(fabs(leadroll) > 3)
			{
#if 0
				if(!((leadplane->Status & PL_DEVICE_DRIVEN) || ((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))))
				{
					if((leadplane->AI.CurrWay->lWPy > 0) && (!(planepnt->AI.iAIFlags2 & AIIGNOREWPY)))
					{
						dy = AICheckSafeAlt(planepnt, (leadplane->AI.WayPosition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y))) - planepnt->WorldPosition.Y;
					}
					else
					{
						dy = AICheckSafeAlt(planepnt, (leadplane->AI.WorldFormationPos.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y))) - planepnt->WorldPosition.Y;
					}
				}
#endif

				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.50))) - planepnt->WorldPosition.X;  // 100  //  200
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.50))) - planepnt->WorldPosition.Z;
				offangle = atan2(-dx, -dz) * 57.2958;
				inturn = 1;
			}
			else if(fabs(planeroll) < 2)
			{
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.50))) - planepnt->WorldPosition.X;  //  100  //  200
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.50))) - planepnt->WorldPosition.Z;
				offangle = atan2(-dx, -dz) * 57.2958;
			}
			else
			{
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.625))) - planepnt->WorldPosition.X;  //  2000
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.625))) - planepnt->WorldPosition.Z;
				offangle = atan2(-dx, -dz) * 57.2958;
			}
		}

		offangle = AICapAngle(offangle);

		offangle -= planehead;

		offangle = AICapAngle(offangle);
	}
	else
	{
		offangle = offangle1;
	}


	if(fabs(offangle1) > 90)
	{
#if 0  //  May not be getting speed from lDesiredSpeed
		if((leadspeed > leadplane->AI.lDesiredSpeed) && (leadplane->Status & PL_AI_DRIVEN))
		{
			leadspeed = leadplane->AI.lDesiredSpeed;
		}
#endif

		if(tdistnm > 20.0)
		{
//			desiredspeed = leadspeed - 200;
			speedmod = -200;
		}
		else if(tdistnm > 5.0)
		{
//			desiredspeed = leadspeed - 100;
			speedmod = -100;
		}
		else if(tdistnm >= 2.0)
		{
//			desiredspeed = leadspeed - (20 * tdistnm);
			speedmod = -(20 * tdistnm);
		}
		else if(tdistnm > 0.25)
		{
//			desiredspeed = leadspeed - 40;
			speedmod = -40;
		}
		else if(tdistnm > 0.05)
		{
//			desiredspeed = leadspeed - (80 * tdistnm);
			speedmod = -(80 * tdistnm);
		}
		else if(tdist > 10)
		{
//			desiredspeed = leadspeed - 4;
			speedmod = -4;
		}
		else if(tdist > 2)
		{
//			desiredspeed = leadspeed - ((tdist - 2) / 2);
			speedmod = -((tdist - 2) / 2);
		}
		else
		{
//			desiredspeed = leadspeed;
			speedmod = 0;
		}

//		if(tdist > 30)
//		{
//			MBAISBrakeOn(planepnt, 1.0);
//		}

		if((desiredspeed < 200) && (!(planepnt->AI.iAIFlags2 & AILANDING)) && (!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))))
		{
			if(leadspeed < 200)
				desiredspeed = leadspeed;
			else
				desiredspeed = 200;
		}
		else if(labs(speedmod) > leadspeed)
		{
			desiredspeed = leadspeed - (leadspeed / 4);
			fpsspeed = AIConvertSpeedToFtPS(planepnt->Altitude, desiredspeed, AIDESIREDSPDCALC);
			ftempvar2 = sqrt((2*tdist)/planepnt->MaxSpeedAccel);
			ftempvar2 *= planepnt->MaxSpeedAccel;
			ftempvar = fabs(fpsspeed - leadplane->V);
			if(ftempvar > ftempvar2)
			{
				desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, (leadplane->V + ftempvar2), AIDESIREDSPDCALC);
			}
		}
		else
		{
			desiredspeed = leadspeed + speedmod;
		}
	}
	else
	{
#if 0  //  May not be getting speed from lDesiredSpeed
		if((leadspeed < leadplane->AI.lDesiredSpeed) && (leadplane->Status & PL_AI_DRIVEN))
		{
			leadspeed = leadplane->AI.lDesiredSpeed;
		}
#endif

		if(planepnt->SpeedBrakeCommandedPos != 0.0)
		{
			planepnt->SpeedBrakeCommandedPos = 0.0;
		}

		if(tdistnm > 0.05)  //  had been tdist but I don't know why
		{
			desiredspeed = leadspeed + 200;
		}
#if 0
		if(tdistnm > 10.0)
		{
			desiredspeed = leadspeed + 200;
		}
		else if(tdistnm > 2.5)
		{
			desiredspeed = leadspeed + 100;
		}
		else if(tdistnm >= 1.0)
		{
			desiredspeed = leadspeed + (40 * tdistnm);
		}
		else if(tdistnm > 0.25)
		{
			desiredspeed = leadspeed + 40;
		}
		else if(tdistnm > 0.05)
		{
			desiredspeed = leadspeed + (80 * tdistnm);
		}
#endif
		else if(tdist > 10)
		{
			desiredspeed = leadspeed + 4;
		}
		else if(tdist > 2)
		{
			desiredspeed = leadspeed + ((tdist - 2) / 2);
		}
		else
		{
			desiredspeed = leadspeed;
		}

		if((tdist > 10) && (tdistnm < 0.75))
		{
#if 0
			fpsspeed = AIConvertSpeedToFtPS(planepnt->Altitude, desiredspeed, AIDESIREDSPDCALC);
			ftempvar = fpsspeed - leadplane->V;
			ftempvar2 = tdist / ftempvar;
			ftempvar /= planepnt->MaxSpeedAccel;
			if(ftempvar > ftempvar2)
			{
				ftempvar = leadplane->V + (planepnt->MaxSpeedAccel * ftempvar2);
				desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, ftempvar, AIDESIREDSPDCALC);
			}
#else
			fpsspeed = AIConvertSpeedToFtPS(planepnt->Altitude, desiredspeed, AIDESIREDSPDCALC);
			ftempvar2 = sqrt((2*tdist)/planepnt->MaxSpeedAccel);
			ftempvar2 *= planepnt->MaxSpeedAccel;
			ftempvar = fpsspeed - leadplane->V;
			if(ftempvar > ftempvar2)
			{
				desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, (leadplane->V + ftempvar2), AIDESIREDSPDCALC);
			}
#endif
		}
	}

	if((inturn) && (dleadhead))
	{
		circledist = (leadplane->V * (360 / (fabs(dleadhead) * FrameRate)));
		leadradius = workradius = circledist / 6.28318530718;

		if(leadroll > 0)
		{
			workradius += planepnt->AI.FormationPosition.X;
			if(leadplane->Status & PL_AI_DRIVEN)
			{
				workradius -= leadplane->AI.FormationPosition.X;
			}
		}
		else
		{
			workradius -= planepnt->AI.FormationPosition.X;
			if(leadplane->Status & PL_AI_DRIVEN)
			{
				workradius += leadplane->AI.FormationPosition.X;
			}
		}

		desiredspeed = desiredspeed * (workradius / leadradius);
	}

	MBAdjustAIThrust(planepnt, desiredspeed, 1);

	if(!inturn)
	{
		AIChangeDir(planepnt, offangle, (dy * WUTOFT));
		if((planepnt->HeightAboveGround * WUTOFT) > 500)
		{
			planepnt->DesiredPitch += AIGetPlanesFlightPitch(leadplane);
		}
	}
	else
	{
		AIChangeDirInFormation(planepnt, leadplane, offangle, (dy * WUTOFT), dhead, dleadhead);
	}

	if((planepnt->DesiredPitch > 0x4000) && (planepnt->DesiredPitch < 0xC000))
	{
		planepnt->DesiredPitch = (planepnt->DesiredPitch > 0x8000) ? 0xC000 : 0x4000;
	}
	return;
}

//**************************************************************************************
void FlyToWayNoDist(PlaneParams *planepnt)
{
	double dx, dy, dz;
	double offangle;

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
	dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);


	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
// 	planepnt->CommandedThrust = GetCommandedThrust(planepnt);

	AIChangeDir(planepnt, offangle, (dy * WUTOFT));
	return;
}

//**************************************************************************************
void AIInitFormationStart(PlaneParams *planepnt)
{
	int wingman, nextsec, winglead, prevsec;

	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;
	if(winglead >= 0)
	{
		planepnt->WorldPosition.X = Planes[winglead].WorldPosition.X;
		planepnt->WorldPosition.Y = Planes[winglead].WorldPosition.Y;
		planepnt->WorldPosition.Z = Planes[winglead].WorldPosition.Z;
	}
	else if(prevsec >= 0)
	{
		planepnt->WorldPosition.X = Planes[prevsec].WorldPosition.X;
		planepnt->WorldPosition.Y = Planes[prevsec].WorldPosition.Y;
		planepnt->WorldPosition.Z = Planes[prevsec].WorldPosition.Z;
	}


	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIInitFormationStart(&Planes[wingman]);
	}

	if(nextsec >= 0)
	{
		AIInitFormationStart(&Planes[nextsec]);
	}
}

#if 1
//**************************************************************************************
void AISetOrientation(PlaneParams *planepnt, float heading)
{
	float radheading;

	//  Heading in planepnt is opposite from what we see on HUD.
	planepnt->Heading = AIConvert180DegreeToAngle(heading);
	radheading = DegToRad(-heading);

	planepnt->Orientation.I.SetValues((double)sin(radheading),(double)0.0f,(double)-cos(radheading));
	planepnt->Orientation.J.SetValues((double)cos(radheading),(double)0.0f,(double)sin(radheading));
	planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.0f);

#if 0  //  heading
	planepnt->Orientation.I.SetValues(sin(radheading),0.0f,-cos(radheading));
	planepnt->Orientation.J.SetValues(cos(radheading),0.0f,sin(radheading));
	planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.0f);
	// roll
	planepnt->Orientation.I.SetValues(0.0f,0.0f,-1.0f);
	planepnt->Orientation.J.SetValues(cos(radheading),sin(radheading),0.0f);
	planepnt->Orientation.K.SetValues(sin(radheading),-cos(radheading),0.0f);
	// pitch
	planepnt->Orientation.I.SetValues(0.0f,sin(radheading),-cos(radheading));
	planepnt->Orientation.J.SetValues(1.0f,0.0f,0.0f);
	planepnt->Orientation.K.SetValues(0.0f,-cos(radheading),sin(radheading));
#endif
}
#else
//**************************************************************************************
void AISetOrientation(PlaneParams *planepnt, long heading)
{
	planepnt->Heading = (360 - heading) * DEGREE;
	if(heading < 45)
	{
//		planepnt->Orientation = UNITPLANENORTH;
		planepnt->Orientation.I.SetValues(0.0f,0.0f,-1.0f);
		planepnt->Orientation.J.SetValues(1.0f,0.0f,0.0f);
		planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.0f);
	}
	else if(heading < 135)
	{
//		planepnt->Orientation = UNITPLANEEAST;
		planepnt->Orientation.I.SetValues(1.0f,0.0f,0.0f);
		planepnt->Orientation.J.SetValues(0.0f,0.0f,1.f);
		planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.f);
	}
	else if(heading < 225)
	{
//		planepnt->Orientation = UNITPLANESOUTH;
		planepnt->Orientation.I.SetValues(0.0f,0.0f,1.0f);
		planepnt->Orientation.J.SetValues(-1.0f,0.0f,0.0f);
		planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.0f);
	}
	else if(heading < 315)
	{
//		planepnt->Orientation = UNITPLANEWEST;
		planepnt->Orientation.I.SetValues(-1.0f,0.0f,0.0f);
		planepnt->Orientation.J.SetValues(0.0f,0.0f,-1.0f);
		planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.0f);
	}
	else
	{
//		planepnt->Orientation = UNITPLANENORTH;
		planepnt->Orientation.I.SetValues(0.0f,0.0f,-1.0f);
		planepnt->Orientation.J.SetValues(1.0f,0.0f,0.0f);
		planepnt->Orientation.K.SetValues(0.0f,-1.0f,0.0f);
	}
}
#endif

//**************************************************************************************
int AIGetNumInGroup(PlaneParams *lookplane, PlaneParams *currplane, int *cntnum)
{
	long wingman, nextsec;

	if(lookplane == currplane)
		return(1);

	*cntnum = *cntnum + 1;
	wingman = currplane->AI.wingman;
	nextsec = currplane->AI.nextpair;
	if(wingman >= 0)
	{
		if(AIGetNumInGroup(lookplane, &Planes[wingman], cntnum))
			return(1);
	}

	if(nextsec >= 0)
	{
		if(AIGetNumInGroup(lookplane, &Planes[nextsec], cntnum))
			return(1);
	}
	return(0);
}

//**************************************************************************************
int AIGetTotalNumInGroup(PlaneParams *currplane)
{
	long wingman, nextsec;
	int currcnt = 1;

	wingman = currplane->AI.wingman;
	nextsec = currplane->AI.nextpair;

	if(wingman >= 0)
	{
		currcnt += AIGetTotalNumInGroup(&Planes[wingman]);
	}

	if(nextsec >= 0)
	{
		currcnt += AIGetTotalNumInGroup(&Planes[nextsec]);
	}
	return(currcnt);
}

//**************************************************************************************
int AIIsTakeOff(MBWayPoints *wayptr)
{
	int actcnt, startact, endact;

	startact = wayptr->iStartAct;
	endact = startact + wayptr->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if(AIActions[actcnt].ActionID == 26)
			return(1);
	}
	return(0);
}

//**************************************************************************************
void AINearMiss(PlaneParams *planepnt, PlaneParams *shooter)
{
	PlaneParams *leadplanepnt;
	long wingman, nextsec;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	if(shooter)
	{
		if(planepnt->AI.iSide == shooter->AI.iSide)
		{
			return;
		}
	}
	else
	{
		return;
	}

	if(planepnt)
	{
		if(iAI_ROE[planepnt->AI.iSide] < 2)
		{
			iAI_ROE[planepnt->AI.iSide] = 2;
		}

		if(planepnt->OnGround)
		{
			return;
		}
	}
	else
	{
		return;
	}

	//  Will have to come up with something different for this. SRE
	if((planepnt->AI.OrgBehave == NULL) && (planepnt->AI.iAIFlags1 & AIINFORMATION))
	{
		leadplanepnt = AIGetLeader(planepnt);

		if((shooter) && ((!MultiPlayer) || (leadplanepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
		{
			if((leadplanepnt->AI.AirTarget != shooter) && (!(leadplanepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				GeneralClearCombatBehavior(leadplanepnt);
			}

			GeneralSetNewAirTarget(leadplanepnt, shooter);

			GeneralSetNewAirThreat(leadplanepnt, shooter);

			GeneralSetNewAirThreat(planepnt, shooter);
		}
		leadplanepnt->AI.lTimer1 = 180000;


		leadplanepnt->AI.OrgBehave = leadplanepnt->AI.Behaviorfunc;
//		leadplanepnt->AI.Behaviorfunc = AIDoJink;
		leadplanepnt->AI.DesiredRoll = 0;
//		leadplanepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(leadplanepnt, 1);

		wingman = leadplanepnt->AI.wingman;
		nextsec = leadplanepnt->AI.nextpair;
		if(wingman >= 0)
		{
			AISetGroupEvade(&Planes[wingman], leadplanepnt);
		}

		if(nextsec >= 0)
		{
			AISetGroupEvade(&Planes[nextsec], leadplanepnt);
		}
	}
	else
	{
		if(planepnt->AI.OrgBehave == NULL)
		{
			planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		}

		if(shooter)
		{
			GeneralSetNewAirThreat(planepnt, shooter);
		}

		AISwitchToAttack(planepnt, 1, 0);
	}


	return;
}

//**************************************************************************************
void AISetGroupEvade(PlaneParams *planepnt, PlaneParams *leadplane)
{
	long wingman, nextsec;

	if(planepnt->AI.iAIFlags1 & AIINFORMATION)
	{
//		planepnt->AI.AirThreat = leadplane->AI.AirThreat;
		GeneralSetNewAirThreat(planepnt, leadplane->AI.AirThreat);
		if((planepnt->AI.AirTarget != leadplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
		{
			GeneralClearCombatBehavior(planepnt);
		}

		GeneralSetNewAirTarget(planepnt, leadplane->AI.AirTarget);

		if(planepnt->AI.OrgBehave == NULL)
		{
			planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		}
		planepnt->AI.Behaviorfunc = leadplane->AI.Behaviorfunc;
		planepnt->AI.DesiredRoll = 0;
		planepnt->AI.lTimer1 = leadplane->AI.lTimer1;

		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
		}
	}

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AISetGroupEvade(&Planes[wingman], leadplane);
	}

	if(nextsec >= 0)
	{
		AISetGroupEvade(&Planes[nextsec], leadplane);
	}
}

//**************************************************************************************
void AIRemoveFromFormation(PlaneParams *planepnt)
{
	int replacenum = -1;
	PlaneParams *replaceplane;
	PlaneParams *leadplane;
	int placeingroup, replaceplace;
	PlaneParams *checkplane;
	int slotnum;
	MovingVehicleParams *carrier;

	if(planepnt == PlayerPlane)
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
	}

	if((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (planepnt == PlayerPlane))
	{
		if(planepnt->AI.Behaviorfunc == CTWaitingForLaunch)
		{
			CTCheckTakeOffQueue(planepnt);

			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			if(carrier->lAIFlags1 & CAT_4_BLOCKED)
			{
				slotnum = (planepnt->AI.lVar2 - 1) % 4;
				if(slotnum == 2)
				{
					carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
					carrier->lAIFlags1 &= ~CAT_4_BLOCKED;
				}
			}
			slotnum = (planepnt->AI.lVar2 - 1) % 4;
			slotnum += 2;
			carrier->bFXDesiredArray[slotnum] = 0;
		}
	}

#if 0  //  for watching crashing planes
	if(!(planepnt->AI.iAICombatFlags1 & 0x4000))
	{
		planepnt->AI.iAICombatFlags1 |= 0x4000;
		Camera1.AttachedObject = (int *)planepnt;
		SimPause ^= 1;
	}
#endif

#if 0
	if(planepnt->FlightStatus & PL_STATUS_CRASHED)
	{
//		AICheckForSAR(planepnt);
		AIUpdateSAR(planepnt);
	}
#endif

	if(planepnt->AI.iVar1 > 0)
	{
		AIMovePlaneToEndOfAttack(planepnt);
		planepnt->AI.iVar1 = 0;
	}

	CheckCurrentTacanStillValid(AIRCRAFT, planepnt);

	if(planepnt->AI.wingman >= 0)
	{
		replacenum = planepnt->AI.wingman;
	}
	else
	{
		replacenum = planepnt->AI.nextpair;
	}

	AIUpdateGoalStates(planepnt - Planes, replacenum, AIGetLeader(planepnt) - Planes, AIRCRAFT);
	AICheckEvents(EVENT_OBJECT, planepnt - Planes);
	AICheckEvents(EVENT_MISSION_END, planepnt - Planes);
	AIUpdatePlanesInActions(planepnt - Planes, replacenum, AIGetLeader(planepnt) - Planes);
	if(replacenum == -1)
	{
		AIUpdatePlaneLinks(planepnt , NULL);
	}
	else
	{
		AIUpdatePlaneLinks(planepnt , &Planes[replacenum]);
	}

	leadplane = AIGetLeader(planepnt);

	if((leadplane == planepnt) && (replacenum >= 0))
	{
		AIUpdatePathObjLinks(planepnt - Planes, replacenum);
	}

	if(leadplane == PlayerPlane)
	{
		if(planepnt == leadplane)
		{
			AIChangeEventFlag(MISSION_FLAG_START + 2);
		}
		else
		{
			placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);
			if(placeingroup < 8)
			{
				AIChangeEventFlag(MISSION_FLAG_START + 2 + placeingroup);
			}
		}
	}

	if(replacenum >= 0)
	{

		replaceplane = &Planes[replacenum];
		if(MultiPlayer)
		{
			placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS)<<28;
			if((!((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))) && (placeingroup))
			{
				if((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
				{
					replaceplace = (replaceplane->AI.iAIFlags1 & AIFLIGHTNUMS)<<28;
					checkplane = planepnt;
					while(checkplane)
					{
						if(checkplane->AI.wingman >= 0)
						{
							if((checkplane->AI.iAICombatFlags2 & AI_LEADER_MASK) == placeingroup)
							{
								checkplane->AI.iAICombatFlags2 &= ~(placeingroup);
								checkplane->AI.iAICombatFlags2 |= (replaceplace);
							}
						}
						if(checkplane->AI.nextpair >= 0)
						{
							checkplane = &Planes[checkplane->AI.nextpair];
							if((checkplane->AI.iAICombatFlags2 & AI_LEADER_MASK) == placeingroup)
							{
								checkplane->AI.iAICombatFlags2 &= ~(placeingroup);
								checkplane->AI.iAICombatFlags2 |= (replaceplace);
							}
						}
						else
						{
							checkplane = NULL;
						}
					}
				}
			}
		}

		if(planepnt->AI.LinkedPlane != NULL)
		{
			if(planepnt->AI.LinkedPlane->AI.LinkedPlane == planepnt)
			{
				planepnt->AI.LinkedPlane->AI.LinkedPlane = &Planes[replacenum];
			}
			Planes[replacenum].AI.LinkedPlane = planepnt->AI.LinkedPlane;
		}

		replaceplane = &Planes[replacenum];
		replaceplane->AI.WayPosition = planepnt->AI.WayPosition;
		replaceplane->AI.winglead = planepnt->AI.winglead;
#if 0  //  We shouldn't need this since if replacenum != wingman then wingman == -1 and could overwrite replacenum's wingman
		if(replacenum != planepnt->AI.wingman)
		{
			replaceplane->AI.wingman = planepnt->AI.wingman;
			if(replaceplane->AI.wingman >= 0)
			{
				Planes[replaceplane->AI.wingman].AI.winglead = replacenum;
			}
		}
#endif
		replaceplane->AI.prevpair = planepnt->AI.prevpair;
		if(replacenum != planepnt->AI.nextpair)
		{
			replaceplane->AI.nextpair = planepnt->AI.nextpair;
			if(replaceplane->AI.nextpair >= 0)
			{
				Planes[replaceplane->AI.nextpair].AI.prevpair = replacenum;
			}
		}
		replaceplane->AI.lDesiredSpeed = planepnt->AI.lDesiredSpeed;
		replaceplane->AI.FormationPosition = planepnt->AI.FormationPosition;
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);
		if(placeingroup == 0)
		{
			if(!AICCheckVoiceNumInGroup(replaceplane - Planes, replaceplane->AI.lAIVoice, (replaceplane->AI.iAIFlags1 & AIFLIGHTNUMS)))
			{
				planepnt->AI.iAIFlags1 &= ~AIFLIGHTNUMS;
				planepnt->AI.iAIFlags1 |= replaceplane->AI.iAIFlags1 & AIFLIGHTNUMS;
				replaceplane->AI.iAIFlags1 &= ~AIFLIGHTNUMS;
			}
		}

		if(planepnt->Status & PL_AI_DRIVEN)
		{
			if(planepnt->AI.OrgBehave == AIFlyEscort)
			{
				replaceplane->AI.iAICombatFlags1 |= (planepnt->AI.iAICombatFlags1 & AI_ESCORT_PART_LEAD);
			}

			if((planepnt->AI.Behaviorfunc == CTWaitingForLaunch) || (planepnt->AI.Behaviorfunc == CTDoCarrierLaunch)) 
			{
				if(replaceplane->AI.OrgBehave)
				{
					replaceplane->AI.OrgBehave = planepnt->AI.OrgBehave;
				}
				else
				{
					replaceplane->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				}
			}
			else if((replaceplane->AI.Behaviorfunc == CTWaitingForLaunch) || (planepnt->AI.Behaviorfunc == CTDoCarrierLaunch))
			{
				if(planepnt->AI.OrgBehave)
				{
					replaceplane->AI.OrgBehave = planepnt->AI.OrgBehave;
				}
				else
				{
					replaceplane->AI.OrgBehave = planepnt->AI.Behaviorfunc;
				}
			}
			else
			{
				replaceplane->AI.Behaviorfunc = planepnt->AI.Behaviorfunc;
				replaceplane->AI.OrgBehave = planepnt->AI.OrgBehave;
			}
		}
		else
		{
			replaceplane->AI.numwaypts = planepnt->AI.numwaypts;	//  I think I wasn't doing this in general
			replaceplane->AI.startwpts = planepnt->AI.startwpts;	//  so I didn't screw up planes that were in
			replaceplane->AI.CurrWay = planepnt->AI.CurrWay;		//  the middle of bombing.  SRE

			if(!((replaceplane->AI.Behaviorfunc == CTWaitingForLaunch) || (replaceplane->AI.Behaviorfunc == CTDoCarrierLaunch) || (replaceplane->OnGround)))
			{
				if(!((replaceplane->Status & AL_DEVICE_DRIVEN) || (replaceplane->Status & AL_COMM_DRIVEN) || (replaceplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				{
					AISetUpWayPointActions(replaceplane, replaceplane->AI.CurrWay);

					if(replaceplane->AI.wingman >= 0)
					{
						AIRestoreWaypointBehavior(replaceplane->AI.wingman, replaceplane->AI.CurrWay, replaceplane->AI.numwaypts);
					}
					if(replaceplane->AI.nextpair >= 0)
					{
						AIRestoreWaypointBehavior(replaceplane->AI.nextpair, replaceplane->AI.CurrWay, replaceplane->AI.numwaypts);
					}
				}
			}
		}
	}
	else if(planepnt->AI.LinkedPlane != NULL)
	{
		if(planepnt->AI.LinkedPlane->AI.LinkedPlane == planepnt)
		{
			planepnt->AI.LinkedPlane->AI.LinkedPlane = NULL;
		}
	}


	if(planepnt->AI.winglead >= 0)
	{
		Planes[planepnt->AI.winglead].AI.wingman = replacenum;
	}
	if(planepnt->AI.prevpair >= 0)
	{
		Planes[planepnt->AI.prevpair].AI.nextpair = replacenum;
	}
	planepnt->AI.winglead = -1;
	planepnt->AI.wingman = -1;
	planepnt->AI.prevpair = -1;
	planepnt->AI.nextpair = -1;

}

//**************************************************************************************
void AIDoJink(PlaneParams *planepnt)
{
	PlaneParams *threat;
	double dx, dy, dz;
	double offangle;
	double toffangle;
	double desiredroll;
	double tdist;
	double toffpitch;
	double degroll, droll, degpitch;
	double dground;
	double dfinalelev = 0;
	double daileron = 0;
	double dtempvar;
	FPoint threatpos;
//	int cnt;

	planepnt->AI.cnt1 = 2;

	if(AIAAStandardChecks(planepnt))
	{
		return;
	}

	threat = AIGetAirTargetPosition(planepnt, &threatpos);

	if(threat == NULL)
	{
		FlyToPoint(planepnt, threatpos);
		MBAdjustAIThrust(planepnt, 600, 1);
	}
	else
	{
		dx = threat->WorldPosition.X - planepnt->WorldPosition.X;
		dz = threat->WorldPosition.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, threat->WorldPosition.Y) - planepnt->WorldPosition.Y;

		offangle = atan2(-dx, -dz) * 57.2958;

		offangle = AICapAngle(offangle);

		toffangle = offangle + 180;

		toffangle = AICapAngle(toffangle);

		offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

		offangle = AICapAngle(offangle);

		toffangle -= AIConvertAngleTo180Degree(threat->Heading);

		toffangle = AICapAngle(toffangle);

		tdist = QuickDistance(dx,dz);

		if((fabs(toffangle) > 30) || (fabs(offangle) < 90) || ((tdist * WUTOFT) > 12000))
		{
#if 0
			planepnt->AI.WayPosition.X = fabs(toffangle);
			planepnt->AI.WayPosition.Y = fabs(offangle);
			planepnt->AI.WayPosition.Z = tdist * WUTOFT;
#endif

//			planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
			AISwitchToAttack(planepnt, 0, 0);

			planepnt->AI.DesiredRoll = 0;
			return;
		}

		if(toffangle < 0)
		{
			desiredroll = -90.0;
		}
		else
		{
			desiredroll = 90.0;
		}

		degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

		toffpitch = atan2(-dy, -tdist) * 57.2958;

	//	dground = ((planepnt->WorldPosition.Y - LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z)) * WUTOFT) ;
		dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 3000))* WUTOFT;

		if(((toffpitch > 0) || (dground < 3000)) && (degpitch < 20) && (dground < 6000))  //  was dground < 8000)
		{
			if(desiredroll < 0)
				desiredroll += 30;
			else
				desiredroll -= 30;
			if(dground < 2000)
			{
				planepnt->AI.Behaviorfunc = AIGainAlt;
				planepnt->AI.DesiredRoll = 0;
			}
		}
		else if((((toffpitch > 0) && (dground > 5000)) || (dground > 6000)) && (degpitch > -5))  //  was dground > 8000)
		{
			if(desiredroll < 0)
				desiredroll -= 10;
			else
				desiredroll += 10;
		}
		else if((dground < 5000) && (degpitch < -5))
		{
			if(desiredroll > 85)
			{
				dtempvar = (fabs(degpitch) - 5) * 3;
				if(dtempvar > 85)
				{
					desiredroll = 0;
				}
				else
				{
					desiredroll = 85 - dtempvar;
				}
			}
			else if (desiredroll < -85)
			{
				dtempvar = (fabs(degpitch) - 5) * 3;
				if(dtempvar > 85)
				{
					desiredroll = 0;
				}
				else
				{
					desiredroll = -85 + dtempvar;
				}
			}
		}

		planepnt->DesiredRoll = AIConvert180DegreeToAngle(-desiredroll);
		degroll = AIConvertAngleTo180Degree(planepnt->Roll);
		droll = desiredroll - degroll;

		if(fabs(droll) < 5.0)
		{
			if(fabs(degroll) < 90)
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(10.0);
			}
			else
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(340.0);
			}
		}
		else if((dground < 3000) && (degpitch < 0) && (fabs(degroll) < 85))
		{
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(10);
		}

		MBAdjustAIThrust(planepnt, 600, 1);  // temp until we have a max speed
	//	planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 60.0;
	}
	return;
}

//**************************************************************************************
void AIHeadAtAirTarget(PlaneParams *planepnt)
{
	PlaneParams *target;
	double dx, dy, dz;
	double offangle, toffangle;
	double dground;
	double tdist;
	double heading, theading;
	double toffpitch;
	PlaneParams *wingplane = NULL;
	FPoint targetpos;
//	double tdist;
//	int cnt;

	if(AIAAStandardChecks(planepnt))
	{
		return;
	}

	target = AIGetAirTargetPosition(planepnt, &targetpos);

	if(target == NULL)
	{
		FlyToPoint(planepnt, targetpos);
		MBAdjustAIThrust(planepnt, 600, 1);
	}
	else
	{
		dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

		dx = target->WorldPosition.X - planepnt->WorldPosition.X;
		dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, target->WorldPosition.Y) - planepnt->WorldPosition.Y;

		if((dy < 0.0) && (dground < 5000))
			dy = 0.0;

		offangle = atan2(-dx, -dz) * 57.2958;

		offangle = AICapAngle(offangle);

		toffangle = offangle + 180;

		toffangle = AICapAngle(toffangle);

	//		toffangle -= AIConvertAngleTo180Degree(target->Heading);
		theading = AIConvertAngleTo180Degree(target->Heading);

		toffangle -= theading;

		toffangle = AICapAngle(toffangle);

	//		offangle -= AIConvertAngleTo180Degree(planepnt->Heading);
  		heading = AIConvertAngleTo180Degree(planepnt->Heading);

		offangle -= heading;

		offangle = AICapAngle(offangle);

		tdist = QuickDistance(dx,dz);
		if((fabs(toffangle) < 30) && (fabs(offangle) > 90) && ((tdist * WUTOFT) < 12000))
		{
//			planepnt->AI.Behaviorfunc = AIDoJink;
			AISwitchToAttack(planepnt, 1);
			planepnt->AI.DesiredRoll = 0;
			return;
		}

	//	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
	//		planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 60.0;
		MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now


	// 		planepnt->CommandedThrust = GetCommandedThrust(planepnt);

		toffpitch = atan2(dy, tdist) * 57.2958;

	//		AIChangeDir(planepnt, offangle, (dy * WUTOFT));
		AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

		if(!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			planepnt->AI.cnt1 = 3;

	}
	return;
}

//**************************************************************************************
void AIGainAlt(PlaneParams *planepnt)
{
	double dground;
	double degroll;
	double degpitch;
	double dfinalelev = 0;
	double daileron = 0;


	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	planepnt->AI.cnt1 = 4;

	dground = planepnt->HeightAboveGround * WUTOFT;// ((planepnt->WorldPosition.Y - LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z)) * WUTOFT) ;

	degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

	if(dground > 4000)
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);

		planepnt->AI.DesiredRoll = 0;
//		AIHeadAtAirTarget(planepnt);  // I got into a loop doing this.
	}
	else
	{
#if 0
		if(degpitch > 0)
		{
			planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 100.0;
		}
		else
		{
			planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 40.0;
		}
#else
		if(degpitch > 0)
		{
			MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now
		}
		else
		{
			MBAdjustAIThrust(planepnt, 250, 1);  //  temp for now
		}
#endif

		degroll = AIConvertAngleTo180Degree(planepnt->Roll);

		if(fabs(degroll) < 90)
		{
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(10);
		}

		planepnt->DesiredRoll = 0;
	}
}

//************************************************************************************************
float MachAToFtSec(float Altitude, float Mach)
{
	double Vss;

	if (Altitude <= 0)
		Vss = sqrt((1.4*1716.3*(519.0*(1.0-0.00000703*1.0))));   // make this equation a constant
	else
		Vss = sqrt(1.4*1716.3*(519.0*(1.0-0.00000703 * Altitude)));

	return(Vss*Mach);
}

//**************************************************************************************
float AICalculateMach(float Altitude, float vel)
{
	double Vss;
	float machval;

	if (Altitude <= 0)
		Vss = sqrt((1.4*1716.3*(519.0*(1.0-0.00000703 * 1.0))));   // make this equation a constant
	else
		Vss = sqrt(1.4*1716.3*(519.0*(1.0-0.00000703 * Altitude)));

 	machval = (float)(vel/Vss);

	return(machval);
}

//**************************************************************************************
void AIControlSurfaces(PlaneParams *planepnt)
{
	double degdroll, degdpitch, avgval;
	float rollperc, pitchperc, workdegree;
	float EffectiveAcceleration, dspeed;
	float machval, daccelperc, orgthrust;
	float degpitch, degroll;
	float timetilgrnd, altdiff;
	float distance = 0;
	float stalleffect;
	double accinc;
	float ftempval;
	PlaneParams *leadplane;
	float orgrthrust = planepnt->RightThrustPercent;
	float orglthrust = planepnt->LeftThrustPercent;
	float desiredthrottle;

	planepnt->AGL = (unsigned int)((planepnt->HeightAboveGround * WUTOFT) - 3.0);

	if(((planepnt->DesiredRoll > 0x3000) && (planepnt->DesiredRoll < 0xD000)) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		if((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
				|| ((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (planepnt->Knots < 150.0f)))
		{
			if((planepnt->DesiredRoll > 0x4000) && (planepnt->DesiredRoll < 0xC000))
			{
				planepnt->DesiredRoll = 0x8000 - planepnt->DesiredRoll;
			}

			if((planepnt->HeightAboveGround * WUTOFT) < 50.0f)
			{
				if((planepnt->DesiredRoll > 0x1000) && (planepnt->DesiredRoll <= 0x8000))
				{
					planepnt->DesiredRoll = 0x1000;
				}
				else if((planepnt->DesiredRoll < 0xF000) && (planepnt->DesiredRoll > 0x8000))
				{
					planepnt->DesiredRoll = 0xF000;
				}
			}
			else
			{
				if((planepnt->DesiredRoll > 0x3000) && (planepnt->DesiredRoll <= 0x8000))
				{
					planepnt->DesiredRoll = 0x3000;
				}
				else if((planepnt->DesiredRoll < 0xD000) && (planepnt->DesiredRoll > 0x8000))
				{
					planepnt->DesiredRoll = 0xD000;
				}
			}
		}
		else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
		{
			if((planepnt->DesiredRoll > 0x4000) && (planepnt->DesiredRoll < 0xC000))
			{
				planepnt->DesiredRoll = 0x8000 - planepnt->DesiredRoll;
			}

			if((planepnt->DesiredRoll > 0x3000) && (planepnt->DesiredRoll <= 0x8000))
			{
				planepnt->DesiredRoll = 0x3000;
			}
			else if((planepnt->DesiredRoll < 0xD000) && (planepnt->DesiredRoll > 0x8000))
			{
				planepnt->DesiredRoll = 0xD000;
			}
		}
	}

	//  Avoid Ground Stuff

	AICheckMaxGs(planepnt);

	if((!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && ((!((planepnt->AI.iAIFlags2 & AILANDING) || (planepnt->OnGround))) || (planepnt->AI.iAIFlags2 & AI_CAN_CRASH)))
	{
		if((!((planepnt->AI.iAIFlags2 & AIGROUNDAVOID) || (planepnt->AI.Behaviorfunc == AIFlyTakeOff))) || (planepnt->AI.iAIFlags2 & AI_CAN_CRASH))
		{
			if((pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed) && (planepnt->IndicatedAirSpeed))
			{
				stalleffect = (planepnt->IndicatedAirSpeed / (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed);
			}
			else if(planepnt->AI.iAIFlags2 & AI_CAN_CRASH)
			{
				stalleffect = 0.33f;
			}
			else
			{
				stalleffect = 1.0f;
			}

			if(stalleffect < 1.0f)
			{
				degpitch = -90.0f * (1.0f - stalleffect);
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(degpitch);
				planepnt->DesiredRoll = 0;
				planepnt->AI.iAIFlags2 |= AI_STALLING;
			}
			else if(planepnt->AI.iAIFlags2 & AI_STALLING)
			{
				if(stalleffect < 1.2f)
				{
					planepnt->DesiredPitch = 0;
					planepnt->DesiredRoll = 0;
				}
				else
				{
					planepnt->AI.iAIFlags2 &= ~(AI_STALLING);
				}
			}
		}
		altdiff = (float)(AIGetMinSafeAlt(planepnt, &distance) + 50) - (planepnt->WorldPosition.Y * WUTOFT);
		if((planepnt->Pitch > 0x8000) && (planepnt->IfVelocity.Y < 0) && (!(planepnt->AI.iAIFlags2 & AI_CAN_CRASH)))
		{
			timetilgrnd = fabs(altdiff / planepnt->IfVelocity.Y) * 0.7f;

			if(altdiff > -500)
			{
				timetilgrnd *= 1.5f;
			}

			degpitch = -AIConvertAngleTo180Degree(planepnt->Pitch);
			if(planepnt->RollYawCouple == -2)
			{
				degroll = 0;
			}
			else
			{
				degroll = fabs(AIConvertAngleTo180Degree(planepnt->Roll));
			}

			if((timetilgrnd < ((degpitch / planepnt->MaxPitchRate) + (degroll / planepnt->MaxRollRate))) || (altdiff > -20.0f))
			{
				if(planepnt->DesiredPitch > 0x8000)
				{
//					planepnt->RollYawCouple = 1;
					planepnt->DesiredPitch = 0;
					planepnt->DesiredRoll = 0;
					planepnt->AI.iAIFlags2 |= AIGROUNDAVOID;
					planepnt->PitchPercentage = 1.0f;
					planepnt->RollPercentage = 1.0f;
				}
				else if((altdiff < -20.0f) && ((planepnt->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)) && (planepnt->Pitch < 0x8000)))
				{
					planepnt->AI.iAIFlags2 &= ~(AIGROUNDAVOID);
				}
			}
			else if((altdiff < -1950.0f) || ((planepnt->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)) && (planepnt->Pitch < 0x8000)))
			{
				planepnt->AI.iAIFlags2 &= ~(AIGROUNDAVOID);
			}
		}
		else
		{
			if((altdiff < -1950.0f) || ((planepnt->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)) && (planepnt->Pitch < 0x8000)))
			{
				planepnt->AI.iAIFlags2 &= ~(AIGROUNDAVOID);
			}
		}
	}

	if((iAI_ROE[planepnt->AI.iSide] > 1) && (!(planepnt->DamageFlags & (DAMAGE_BIT_GUN))) && (planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1) &&(!(planepnt->AI.iAIFlags2 & AI_GUN_PAUSE)) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		AICheckRearGuns(planepnt);
	}
	else if(planepnt->AI.iAIFlags2 & AI_GUN_PAUSE)
	{
		if(planepnt->AI.lGunTimer >= 0)
		{
			planepnt->AI.lGunTimer -= DeltaTicks;
		}

		if(planepnt->AI.lGunTimer < 0)
		{
			planepnt->AI.iAIFlags2 &= ~(AI_GUN_PAUSE);
			planepnt->AI.lGunTimer = 1000;
		}
	}

	//  Accumulator Stuff

	ftempval = 180 * accmultval;
#if 0
	planepnt->AI.dRollAccumulator += AIConvertAngleTo180Degree(planepnt->DesiredRoll) - (planepnt->AI.dRollAccumulator / accmultval);
#else
	accinc = AIConvertAngleTo180Degree(planepnt->DesiredRoll) - (planepnt->AI.dRollAccumulator / accmultval);
	accinc = AICapAngle(accinc);

	planepnt->AI.dRollAccumulator += accinc;

	if(planepnt->AI.dRollAccumulator > ftempval)
	{
		planepnt->AI.dRollAccumulator -= ftempval * 2;
	}
	else if(planepnt->AI.dRollAccumulator <= -ftempval)
	{
		planepnt->AI.dRollAccumulator += ftempval * 2;
	}
#endif
	avgval = planepnt->AI.dRollAccumulator / accmultval;
	if(TimeExcel == 0)
	{
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(avgval);
	}

#if 0
	if((planepnt->DesiredPitch > 0x4000) && (planepnt->DesiredPitch < 0xC000))
	{
		degpitch = AIConvertAngleTo180Degree(planepnt->DesiredPitch);
		if(degpitch < 0)
		{
			planepnt->DesiredPitch = (degpitch < -90) ? 0xC000 : 0x4000;
		}
	}
#endif

#if 0
	planepnt->AI.dPitchAccumulator += AIConvertAngleTo180Degree(planepnt->DesiredPitch) - (planepnt->AI.dPitchAccumulator / accmultval);
#else
	accinc = AIConvertAngleTo180Degree(planepnt->DesiredPitch) - (planepnt->AI.dPitchAccumulator / accmultval);

	accinc = AICapAngle(accinc);

	planepnt->AI.dPitchAccumulator += accinc;

	if(planepnt->AI.dPitchAccumulator > ftempval)
	{
		planepnt->AI.dPitchAccumulator -= ftempval * 2;
	}
	else if(planepnt->AI.dPitchAccumulator <= -ftempval)
	{
		planepnt->AI.dPitchAccumulator += ftempval * 2;
	}
#endif

	avgval = planepnt->AI.dPitchAccumulator / accmultval;

	if(TimeExcel == 0)
	{
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(avgval);
	}
	//  End accumulator stuff

	degdpitch = AIConvertAngleTo180Degree(planepnt->DesiredPitch);

	if((!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && ((fabs(degdpitch) > 1.0) && (!(planepnt->AI.iAIFlags2 & AI_IN_DOGFIGHT))))
	{
		degdroll = AIConvertAngleTo180Degree(planepnt->DesiredRoll);

		if(fabs(degdroll) <= 90)
		{
			if(fabs(degdroll) > 60)
			{
				degdroll = (degdroll < 0) ? -60 : 60;
				planepnt->DesiredRoll = AIConvert180DegreeToAngle(degdroll);
			}
		}
		else
		{
			if(fabs(degdroll) < 120)
			{
				degdroll = (degdroll < 0) ? -120 : 120;
				planepnt->DesiredRoll = AIConvert180DegreeToAngle(degdroll);
			}
		}
	}

	if(planepnt->AI.iAIFlags2 & AI_IN_DOGFIGHT)
	{
		planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	}

#if 0
	if(!(planepnt->AI.iAIFlags1 & AIFASTPITCHROLL))
	{
//		if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll - planepnt->Roll)) < 5)
		if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll)) < 5)
		{
			if(planepnt->RollPercentage > 0.10)
			{
				planepnt->RollPercentage = 0.10;	//  max roll 90.0
			}
		}

		if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredPitch - planepnt->Pitch)) < 6)
		{
			if(planepnt->PitchPercentage > 0.10)
			{
				planepnt->PitchPercentage = 0.10;	//  max roll 90.0
			}
		}
	}


#endif
	rollperc = (AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll) / planepnt->MaxPitchRate) * planepnt->RollPercentage * 0.5;
	if(rollperc >= 0.5)
	{
		rollperc = 0.5;
	}
	else if(rollperc <= -0.5)
	{
		rollperc = -0.5;
	}

	//  For Greg
	rollperc *= 0.5f;

	if(rollperc < 0)
	{
		workdegree = planepnt->CS->AileronRange.Low * fabs(rollperc);
	}
	else
	{
		workdegree = planepnt->CS->AileronRange.High * rollperc;
	}

	planepnt->Aileron = Integrate(0, 0,
			     		   planepnt->Aileron,
		                   planepnt->CS->AileronRange,
						   planepnt->CS->AileronRate,
						   (float)DeltaTicks/1000.0f, workdegree);

	pitchperc = -(AIConvertAngleTo180Degree(planepnt->DesiredPitch - planepnt->Roll) / planepnt->MaxPitchRate) * planepnt->PitchPercentage * 0.5;
	if(pitchperc >= 0.5)
	{
		pitchperc = 0.5;
	}
	else if(pitchperc <= -0.5)
	{
		pitchperc = -0.5;
	}

	//  For Greg
	pitchperc *= 0.5f;

	if(pitchperc < 0)
	{
		workdegree = planepnt->CS->SymetricalElevatorRange.Low * fabs(pitchperc);
	}
	else
	{
		workdegree = planepnt->CS->SymetricalElevatorRange.High * pitchperc;
	}

	planepnt->SymetricalElevator = Integrate(0, 0,
				     	   planepnt->SymetricalElevator,
			               planepnt->CS->SymetricalElevatorRange,
						   planepnt->CS->ElevatorRate,
						   (float)DeltaTicks/1000.0f, workdegree);

	workdegree = -planepnt->Aileron*0.158;

	planepnt->Rudder = Integrate(0, 0,
			     		   planepnt->Rudder,
		                   planepnt->CS->RudderRange,
						   planepnt->CS->RudderRate,
						   (float)DeltaTicks/1000.0f, workdegree);

	EffectiveAcceleration = (planepnt->MaxSpeedAccel * planepnt->SpeedPercentage);
	dspeed = planepnt->DesiredSpeed - (float)planepnt->V;


	if((!(planepnt->AI.iAIFlags2 & AILANDING)) && ((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		if((dspeed / EffectiveAcceleration) < -20)
		{
	//		MBAISBrakeOn(planepnt, 1.0);
			if(planepnt->AI.lEffectTimer < 0)
			{
				if(!planepnt->SpeedBrakeState)
				{
					planepnt->AI.lEffectTimer = 5000;
				}
				planepnt->SpeedBrakeState = 1;
			}
	 		daccelperc = 0;
		}
		else
		{
	//		planepnt->SpeedBrakeCommandedPos = 0.0;
			if(planepnt->AI.lEffectTimer < 0)
			{
				if(planepnt->SpeedBrakeState)
				{
					planepnt->AI.lEffectTimer = 5000;
				}
				planepnt->SpeedBrakeState = 0;
			}
			if(dspeed > 0)
			{
				daccelperc = ((dspeed / EffectiveAcceleration) / 20) * 50.0;
			}
			else
			{
				daccelperc = 0;
			}
		}
	}
	else
	{
	 	daccelperc = 0;
	}
//	UpdateSpeedBrakePosition(planepnt);  //  Moved to CalcF18CASUpdatesSimple

	if(MultiPlayer)
	{
		if(!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			if(planepnt->AI.iAICombatFlags1 & AI_MULTI_FIRE_GUN)
			{
				MAIFireStraightGun(planepnt);
			}
		}
	}

 	planepnt->CommandedThrust = GetCommandedThrust(planepnt,planepnt->ThrottlePos);

	machval = AICalculateMach(planepnt->Altitude, (float)planepnt->DesiredSpeed);

	if((planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) && ((planepnt->AI.Behaviorfunc == CTWaitingForLaunch) || (planepnt->AI.Behaviorfunc == CTDoCarrierLaunch) || (planepnt->AI.Behaviorfunc == AICarrierTrap)))
	{
		planepnt->LeftThrust =  GetCommandedThrust(planepnt, planepnt->LeftThrustPercent);
		planepnt->RightThrust = GetCommandedThrust(planepnt, planepnt->RightThrustPercent);
	}
	else
	{
		orgthrust = planepnt->RightThrustPercent;
		planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 50.0;

		if(machval >= 1.0)
		{
			if(machval > 2.0)
			{
				planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 100.0;
			}
			else
			{
				planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 75.0 + (((machval - 1.0) / 1.0) * 25.0);
			}
		}
		else
		{
			planepnt->LeftThrustPercent = planepnt->RightThrustPercent = ((machval / 1.0) * 75.0);
		}

		if(GetRegValueL("nohollywood") != 1)
		{
			if((planepnt->AI.Behaviorfunc == (CTWaitingForLaunch)) && (planepnt->AI.lVar3 == 100) && (planepnt->AI.lTimer2 < 16000))
			{
				if (planepnt->AI.lTimer2 < 8000)
				{
					desiredthrottle = 100;
					planepnt->LeftThrustPercent = orglthrust;
					planepnt->RightThrustPercent = orgrthrust;

					if(planepnt->LeftThrustPercent < 80)
					{
						orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 80;
					}
					else
					{
//						planepnt->LeftThrustPercent += (20.0f * (float) DeltaTicks) / 1500.0f;
						planepnt->LeftThrustPercent = 80 + (20 - ((20.0f * (float)(planepnt->AI.lTimer2 - 5000)) / 3000.0f));
						orglthrust = orgrthrust = planepnt->RightThrustPercent = planepnt->LeftThrustPercent;
						if(planepnt->LeftThrustPercent > 100)
						{
							orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 100;
						}
					}
				}
				else
				{
					planepnt->LeftThrustPercent = orglthrust;
					planepnt->RightThrustPercent = orgrthrust;

					//  8000 here is 16000 - AB spool up time (in this case also 8000)
					orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = (80 - ((80.0f * (float)(planepnt->AI.lTimer2 - 8000)) / 8000.0f));
				}
			}
			else if((planepnt->AI.Behaviorfunc == CTDoCarrierLaunch) && ((planepnt->AI.lVar3 == 0) || ((planepnt->AI.lVar3 == 1) && (planepnt->AI.lTimer2 > 40000))))
			{
				planepnt->LeftThrustPercent = orglthrust;
				planepnt->RightThrustPercent = orgrthrust;

				if(((planepnt->AI.lVar3 == 1) && (planepnt->AI.lTimer2 > 50000)) || (planepnt->AI.lVar3 == 0))
				{
					orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 100;
				}
				else
				{
					desiredthrottle = ((machval / 1.0) * 75.0);

					planepnt->LeftThrustPercent -= (20.0f * (float) DeltaTicks) / 1500.0f;
					orglthrust = orgrthrust = planepnt->RightThrustPercent = planepnt->LeftThrustPercent;
					if(planepnt->LeftThrustPercent < desiredthrottle)
					{
						orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = desiredthrottle;
					}
				}
			}
		}
		else
		{
			if((planepnt->AI.Behaviorfunc == (CTWaitingForLaunch)) && (planepnt->AI.lVar3 == 100) && (planepnt->AI.lTimer2 < 16000))
			{
				planepnt->LeftThrustPercent = orglthrust;
				planepnt->RightThrustPercent = orgrthrust;

				//  8000 here is 16000 - AB spool up time (in this case also 8000)
				orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = (80 - ((80.0f * (float)(planepnt->AI.lTimer2 - 8000)) / 8000.0f));
				if(planepnt->LeftThrustPercent > 75)
				{
					orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 75;
				}
			}
			else if((planepnt->AI.Behaviorfunc == CTDoCarrierLaunch) && ((planepnt->AI.lVar3 == 0) || ((planepnt->AI.lVar3 == 1) && (planepnt->AI.lTimer2 > 40000))))
			{
				planepnt->LeftThrustPercent = orglthrust;
				planepnt->RightThrustPercent = orgrthrust;

				if(((planepnt->AI.lVar3 == 1) && (planepnt->AI.lTimer2 > 50000)) || (planepnt->AI.lVar3 == 0))
				{
					orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 75.0f;
				}
				else
				{
					desiredthrottle = ((machval / 1.0) * 75.0);

					planepnt->LeftThrustPercent -= (20.0f * (float) DeltaTicks) / 1500.0f;
					orglthrust = orgrthrust = planepnt->RightThrustPercent = planepnt->LeftThrustPercent;
					if(planepnt->LeftThrustPercent < desiredthrottle)
					{
						orglthrust = orgrthrust = planepnt->LeftThrustPercent = planepnt->RightThrustPercent = desiredthrottle;
					}
				}
			}
		}

		planepnt->RightThrustPercent += daccelperc;
		if(planepnt->RightThrustPercent > 100.0)
		{
			planepnt->RightThrustPercent = 100.0;
		}
		else if(planepnt->RightThrustPercent > 74.0)
		{
			if(planepnt->RightThrustPercent < 80.0)
			{
				if(orgthrust > 75.0)
				{
					planepnt->RightThrustPercent = 80.0;
				}
				else
				{
					planepnt->RightThrustPercent = 74.0;
				}
			}
		}

		if(planepnt->AI.Behaviorfunc == AIFlyTakeOff)
		{
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				if(planepnt->RightThrustPercent < leadplane->RightThrustPercent)
				{
					planepnt->RightThrustPercent = leadplane->RightThrustPercent;
				}
			}
		}
		else if((!(planepnt->AI.iAIFlags1 & AINOFORMUPDATE)) && (planepnt->AI.iAIFlags1 & AIINFORMATION))
		{
			leadplane = AIGetLeader(planepnt);
			if((leadplane == PlayerPlane) && (leadplane->ThrottlePos >= 80.0f))
			{
				if(planepnt->RightThrustPercent < leadplane->ThrottlePos)
				{
					planepnt->RightThrustPercent = leadplane->ThrottlePos;
				}
			}
		}

		planepnt->ThrottlePos = planepnt->LeftThrustPercent = planepnt->RightThrustPercent;

		if(planepnt->AI.lEffectTimer >= 0)
		{
			if((orglthrust < 0.75) && (planepnt->LeftThrustPercent >= 0.75))
			{
				planepnt->LeftThrustPercent = orglthrust;
			}
			else if((orglthrust >= 0.75) && (planepnt->LeftThrustPercent < 0.75))
			{
				planepnt->LeftThrustPercent = orglthrust;
			}
			if((orgrthrust < 0.75) && (planepnt->RightThrustPercent >= 0.75))
			{
				planepnt->RightThrustPercent = orgrthrust;
			}
			else if((orgrthrust >= 0.75) && (planepnt->RightThrustPercent < 0.75))
			{
				planepnt->RightThrustPercent = orgrthrust;
			}
		}
		else
		{
			if(((orglthrust < 0.75) && (planepnt->LeftThrustPercent >= 0.75)) || ((orglthrust >= 0.75) && (planepnt->LeftThrustPercent < 0.75)))
			{
				planepnt->AI.lEffectTimer = 5000;
			}
			if(((orgrthrust < 0.75) && (planepnt->RightThrustPercent >= 0.75)) || ((orgrthrust >= 0.75) && (planepnt->RightThrustPercent < 0.75)))
			{
				planepnt->AI.lEffectTimer = 5000;
			}
		}
	}

	if(planepnt->DamageFlags & DAMAGE_BIT_L_ENGINE)
	{
		planepnt->LeftThrustPercent = 0.0f;
	}
	if(planepnt->DamageFlags & DAMAGE_BIT_R_ENGINE)
	{
		planepnt->RightThrustPercent = 0.0f;
	}

	GetAISimpleFuelUsage(planepnt);

	if (planepnt->SpeedBrake != planepnt->SpeedBrakeCommandedPos)
	{
		planepnt->SpeedBrake = Integrate(0,0, planepnt->SpeedBrake,
							      planepnt->CS->SpeedBrakeRange,
								  planepnt->CS->SpeedBrakeRate,
								  (float)DeltaTicks/1000.0f, planepnt->SpeedBrakeCommandedPos);
	}

	MoveLandingGear(planepnt);
	if((planepnt->DesiredSpeed < 0) && (planepnt->OnGround != 2))
	{
		planepnt->DesiredSpeed = 0;
	}

	AIDetermineAIHz(planepnt);

	if((planepnt->AI.AOADegPitch) && (planepnt->Mach < 1.15f))
	{
		workdegree = AIConvertAngleTo180Degree(planepnt->AI.AOADegPitch);

		workdegree *= 0.75;

		if((planepnt->Mach > 1.10f) && (workdegree > 9.0f))
		{
			workdegree = 9.0f;
		}
		else if((planepnt->Mach > 1.05f) && (workdegree > 14.0f))
		{
			workdegree = 14.0f;
		}
		else if((planepnt->Mach > 1.0f) && (workdegree > 22.0f))
		{
			workdegree = 22.0f;
		}
		else if((planepnt->Mach > 0.95f) && (workdegree > 26.0f))
		{
			workdegree = 26.0f;
		}
		else if(workdegree > 28.0f)
		{
			workdegree = 28.0f;
		}
		planepnt->LeadingEdgeCommandedPos = workdegree;
	}
	else
	{
		planepnt->LeadingEdgeCommandedPos = 0.0f;
	}

	CalcCarrierBasedVisuals(planepnt);

	if(planepnt->AI.lTimerAWACS >= 0)
	{
		planepnt->AI.lTimerAWACS -= DeltaTicks;
		if(planepnt->AI.lTimerAWACS < 0)
		{
			planepnt->AI.iAICombatFlags2 &= ~AI_AWACS_DETECT;
		}
	}

	if((pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 4) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 62))
	{
		AIChangeFXArray(planepnt, 0, planepnt->AI.cFXarray[1], 5000);
	}


	if((planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		if(planepnt->OnGround == 2)
		{
			MovingVehicleParams *carrier;
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			if((carrier->Status & VL_DESTROYED) || (!(carrier->Status & VL_ACTIVE)))
			{
				AIDamagePlane(planepnt - Planes, 100);
			}
		}
	}

	if((planepnt == PlayerPlane) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
	{
		if(planepnt->IndicatedAirSpeed > 240)
		{
			if(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)
			{
				RaiseLowerLandingGear(planepnt,RAISE_LANDING_GEAR);
				if(planepnt == PlayerPlane)
				{
					DisplayWarning(GEAR, OFF, 1);
					int iLevel=g_iSoundLevelEngineRum-10;
					if (iLevel<0) iLevel=0;
					AICAddSoundCall(AICDelayedSoundEffect, SND_GEARUP, 1200, 50, iLevel);
				}
			}
			if(planepnt == PlayerPlane)
			{
				PlayerPlane->FlapsMode = FLAPS_AUTO;
				SetCockpitItemState(125,0);
			}
			planepnt->Flaps = 0;
			planepnt->FlapsCommandedPos = 0.0;
		}
	}
	else if(!(planepnt->AI.iAIFlags2 & AILANDING))
	{
		if(planepnt->IndicatedAirSpeed > 240)
		{
			if(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)
			{
				RaiseLowerLandingGear(planepnt,RAISE_LANDING_GEAR);
			}
			planepnt->Flaps = 0;
			planepnt->FlapsCommandedPos = 0.0;
		}
	}

	lDebugPlaneSRE = -1;
	return;
}

//**************************************************************************************
void AIDetermineAIHz(PlaneParams *planepnt)
{
	float froll, droll;

	if(planepnt->AI.iAICombatFlags2 & AI_KEEP_HIGH_RATE)
	{
		planepnt->UpdateRate = HIGH_AERO;
		return;
	}

	if(iInJump)
	{
  		planepnt->UpdateRate = LOW_AERO;   //  10 Hz
		return;
	}
	else if((planepnt->V == 0) && (planepnt->DesiredSpeed == 0))
	{
  		planepnt->UpdateRate = LOW_AERO;   //  10 Hz
		return;
	}
	else if(FrameRate > 23.0f)
	{
		planepnt->UpdateRate = HIGH_AERO; //  50 Hz
		return;
	}
	else if(FrameRate > 9.0f)
	{
		planepnt->UpdateRate = MED_AERO;   //  25 Hz
		return;
	}

	froll = fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll));

	if(froll < 10.0f)  //  if Roll is less than 5 degrees
	{
		droll = fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll - planepnt->Roll));
		if(droll < 5.0f)  //  and we aren't changing our roll by more than 5 degrees
		{
	  		planepnt->UpdateRate = LOW_AERO;   //  10 Hz
			return;
		}
	}

	planepnt->UpdateRate = MED_AERO;  //  25 Hz
	return;
}


//**************************************************************************************
void AIChangeDirInFormation(PlaneParams *planepnt, PlaneParams *leadplane, double offangle, double dy, double dhead, double dleadhead)
{
	double dworkvar, desiredroll;
	double leadyaw;

	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
//	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -(offangle + (dleadhead * FrameRate)), 0);

	if(leadplane->Status & PL_AI_DRIVEN)
	{
		leadyaw = sin(DegToRad(AIConvertAngleTo180Degree(-leadplane->DesiredRoll))) * leadplane->YawRate;
		dworkvar = offangle + leadyaw;
	}
	else
	{
		dworkvar = offangle + (dleadhead * FrameRate);
	}

	if(!planepnt->YawRate)
	{
		planepnt->DesiredRoll = 0;
#ifdef _DEBUG
		char errmsg[256];
		sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
	    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
	}
	else if(fabs(dworkvar) < planepnt->YawRate)
	{
		desiredroll = asin(dworkvar / planepnt->YawRate);
		desiredroll = RadToDeg(desiredroll);
		planepnt->DesiredRoll = -desiredroll * DEGREE;
	}
	else
	{
		planepnt->DesiredRoll = (dworkvar < 0) ? 0x4000 : 0xC000;
	}

#if 0
	if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll)) < 10)
	{
		planepnt->RollPercentage = 15.0 / planepnt->MaxRollRate;
	}
#endif

#if 0  //  #ifdef SREDEBUGFILE
	char tempstr[180];
	double temptime;

//	if(planepnt == (PlaneParams *)Camera1.AttachedObject)
	if(planepnt->AI.winglead == 0)
	{
		temptime = (double)GetTickCount();
#if 0
		sprintf(tempstr, "%02.02f : ANG %02.02f, DH %02.02f, DLH %02.02f, LR %02.02f, DR %02.02f, R %02.02f, RP %02.02f \n", temptime/1000, offangle, dhead * FrameRate, dleadhead * FrameRate, AIConvertAngleTo180Degree(leadplane->Roll), -AIConvertAngleTo180Degree(planepnt->DesiredRoll), AIConvertAngleTo180Degree(planepnt->Roll), planepnt->RollPercentage);
		_write(DebugFileSRE,tempstr,strlen(tempstr));
		sprintf(tempstr, "    FR %02.02f,  CT %u, LT %u, DT %u \n", FrameRate, CurrentTick, LastTick, DeltaTicks);
		_write(DebugFileSRE,tempstr,strlen(tempstr));
#else
		sprintf(tempstr, "    FR %02.02f,  SY %02.02f, LSR %02.02f, LR %02.02f\n", FrameRate, (dleadhead * FrameRate), (dleadhead * LastFrameRate), AIConvertAngleTo180Degree(leadplane->Roll));
		_write(DebugFileSRE,tempstr,strlen(tempstr));
#endif
	}
#endif

#if 0
	double degpitch, leaddegpitch, newpitch;

	degpitch = AIConvertAngleTo180Degree(planepnt->DesiredPitch);
	leaddegpitch = AIConvertAngleTo180Degree(AIGetPlanesFlightPitch(leadplane));

	planepnt->DesiredPitch += AIGetPlanesFlightPitch(leadplane);

	newpitch = AIConvertAngleTo180Degree(planepnt->DesiredPitch);
#endif
	if((planepnt->HeightAboveGround * WUTOFT) > 500)
	{
		planepnt->DesiredPitch += AIGetPlanesFlightPitch(leadplane);
	}

//	planepnt->DesiredRoll += leadplane->DesiredRoll;
}


//**************************************************************************************
int MBDoDebugBreak()
{
	int ispaused = 0;
//	PlaneParams *planepnt;

#ifdef _DEBUG
//	planepnt = (PlaneParams *)Camera1.AttachedObject;
//	if(planepnt->Status & AL_AI_DRIVEN)
//	{
//		lpDD->FlipToGDISurface();
//		PauseForDiskHit();
//		ispaused = 1;
		ispaused = 0;
//	}
#endif

	return(ispaused);
}


//**************************************************************************************
int AICheckFBG(PlaneParams *planepnt, PlaneParams *leadplane)
{
	float leadspeed;
	float planespeed;
	float pitchup;
	float rotatespeed;
	float leadheight = leadplane->HeightAboveGround * WUTOFT;

	planepnt->AI.iAIFlags2 |= AI_STAY_ON_GROUND;
	planepnt->AI.iAIFlags2 &= ~(AI_FAKE_AOA);

	if(planepnt->AI.lTimer2 >= 0)
	{
		return(1);
	}

	leadspeed = (float)leadplane->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
//	planespeed = (float)planepnt->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	planespeed = planepnt->IndicatedAirSpeed;
	rotatespeed = pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed * 1.15f;

	if((leadplane == planepnt) && (planepnt->V == 0))
	{
		if(AIWaitingForTakeOffFlag(planepnt))
		{
			return(1);
		}
	}
	else if((leadplane != planepnt) && (leadplane->V == 0))
	{
		return(1);
	}

	if(planepnt->Brakes)
	{
		if((planepnt == leadplane) || ((!leadplane->Brakes) && (leadplane->RightThrustPercent > 20.0) && (!(leadplane->Status & PL_AI_DRIVEN)))
			 || ((!leadplane->Brakes) && (leadspeed > 40) && (leadplane->Status & PL_AI_DRIVEN)))
		{
			planepnt->Brakes = 0;
			if(planepnt == (PlaneParams *)PlayerPlane)
			{
			 	DisplayWarning(BRAKE, OFF, 1);
			}
		}
		else
		{
			planepnt->DesiredPitch = 0;
			planepnt->DesiredRoll = 0;
			planepnt->DesiredSpeed = 0;
			planepnt->V = 0;
			return(1);
		}
	}

	if(planepnt->OnGround)
	{
		if(!planepnt->Brakes)
		{
//			if((planepnt != leadplane) && ((leadplane->Brakes) || (leadplane->RightThrustPercent < 20.0)))
			if((planepnt != leadplane) && ((((leadplane->Brakes) || (leadplane->RightThrustPercent < 20.0)) && (!(leadplane->Status & PL_AI_DRIVEN)))
				 || (((leadplane->Brakes) || (leadspeed < 40)) && (leadplane->Status & PL_AI_DRIVEN))))
			{
				planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 0.0;
				planepnt->Brakes = 1;
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
				 	DisplayWarning(BRAKE, ON, 1);
				}
				planepnt->DesiredPitch = 0;
				planepnt->DesiredRoll = 0;
				planepnt->DesiredSpeed = 0;
				planepnt->V = 0;
				planepnt->AI.iAIFlags2 |= AI_STAY_ON_GROUND;
				return(1);
			}
		}
	}


	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		planepnt->AI.iAIFlags2 &= ~AI_STAY_ON_GROUND;
		if((planespeed < 50) || (300 > (planepnt->HeightAboveGround*WUTOFT)))
		{
			pitchup = 10;
			planepnt->DesiredPitch = pitchup * DEGREE;
			planepnt->DesiredRoll = 0;
			MBAdjustAIThrust(planepnt, 60, 1);
			if(25 < (planepnt->HeightAboveGround*WUTOFT))
			{
				if(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)
				{
					if(planepnt == (PlaneParams *)PlayerPlane)
					{
						DisplayWarning(GEAR, OFF, 1);
					}
					RaiseLowerLandingGear(planepnt,RAISE_LANDING_GEAR);
				}
			}

			if(50 < (planepnt->HeightAboveGround*WUTOFT))
			{
				if(planepnt->Flaps)
				{
					if(planepnt == (PlaneParams *)PlayerPlane)
					{
						DisplayWarning(FLAPS, OFF, 1);
					}
					planepnt->Flaps = 0;
					planepnt->FlapsCommandedPos = 0.0;
				}
			}
		}
		else
		{
			if(planepnt->AI.OrgBehave == NULL)
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 6, 0, 0);
				}
				planepnt->AI.Behaviorfunc = AIFlyFormation;
				AISetUpNewWayPoint(planepnt);
			}
			else
			{
				planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				planepnt->AI.OrgBehave = NULL;
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					if(!((planepnt->AI.Behaviorfunc == AIAirInvestigate) && (planepnt->AI.AirTarget)))
						NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 7, 0, 0);
					else
						NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
				}
				if(!((planepnt->AI.Behaviorfunc == AIAirInvestigate) && (planepnt->AI.AirTarget)))
					AISetUpNewWayPoint(planepnt);
			}
  			planepnt->AI.Behaviorfunc(planepnt);
			if(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(GEAR, OFF, 1);
				}
				RaiseLowerLandingGear(planepnt,RAISE_LANDING_GEAR);
			}
			if(planepnt->Flaps)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(FLAPS, OFF, 1);
				}
				planepnt->Flaps = 0;
				planepnt->FlapsCommandedPos = 0.0;
			}
			if(leadplane == planepnt)
			{
				AICCheckForNewThreat(planepnt);
			}
		}
	}
	else if((planespeed < rotatespeed) || ((leadheight < 10.0f) && (leadplane != planepnt)))
	{
		if(planepnt->V == 0)
		{
			if(!AIIsClosestAirFieldOpen(planepnt, planepnt->WorldPosition, NMTOWU))
			{
				planepnt->AI.lTimer2 = 3600000;
				return(1);
			}
		}

		planepnt->DesiredPitch = 0;
		planepnt->DesiredRoll = 0;
		if((planepnt->V > leadplane->V) && (leadplane != planepnt))
		{
			planepnt->DesiredSpeed = leadplane->V;
		}
		else
		{
			MBAdjustAIThrust(planepnt, 600, 1);
		}
	}
	else if((planespeed < 200) || (500 > (planepnt->HeightAboveGround*WUTOFT)))
	{
		planepnt->AI.iAIFlags2 &= ~AI_STAY_ON_GROUND;
		pitchup = ((planespeed - rotatespeed) / 10);
		if(pitchup > 5)
		{
			pitchup = 5;
		}
		planepnt->DesiredPitch = pitchup * DEGREE;
		planepnt->DesiredRoll = 0;
		if((planepnt->V > leadplane->V) && (leadplane != planepnt))
		{
			planepnt->DesiredSpeed = leadplane->V;
		}
		else
		{
			MBAdjustAIThrust(planepnt, 600, 1);
		}
		if(75 < (planepnt->HeightAboveGround*WUTOFT))
		{
			if(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(GEAR, OFF, 1);
				}
				RaiseLowerLandingGear(planepnt,RAISE_LANDING_GEAR);
			}
			AITankerTakeoff(planepnt);
		}

		if(250 < (planepnt->HeightAboveGround*WUTOFT))
		{
			if(planepnt->Flaps)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(FLAPS, OFF, 1);
				}
				planepnt->Flaps = 0;
				planepnt->FlapsCommandedPos = 0.0;
			}
		}
	}
	else
	{
		planepnt->AI.iAIFlags2 &= ~AI_STAY_ON_GROUND;
		if(planepnt->AI.OrgBehave == NULL)
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 6, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			AISetUpNewWayPoint(planepnt);
		}
		else
		{
			planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			planepnt->AI.OrgBehave = NULL;
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				if(!((planepnt->AI.Behaviorfunc == AIAirInvestigate) && (planepnt->AI.AirTarget)))
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 7, 0, 0);
				else
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
			}
			if(!((planepnt->AI.Behaviorfunc == AIAirInvestigate) && (planepnt->AI.AirTarget)))
				AISetUpNewWayPoint(planepnt);
		}
  		planepnt->AI.Behaviorfunc(planepnt);
		if(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			if(planepnt == (PlaneParams *)PlayerPlane)
			{
				DisplayWarning(GEAR, OFF, 1);
			}
			RaiseLowerLandingGear(planepnt,RAISE_LANDING_GEAR);
		}
		if(planepnt->Flaps)
		{
			if(planepnt == (PlaneParams *)PlayerPlane)
			{
				DisplayWarning(FLAPS, OFF, 1);
			}
			planepnt->Flaps = 0;
			planepnt->FlapsCommandedPos = 0.0;
		}
		if(leadplane == planepnt)
		{
			AICCheckForNewThreat(planepnt);
		}
	}

	if((planepnt->DesiredSpeed > leadplane->V) && (leadplane->OnGround) && (planepnt != leadplane))
	{
		planepnt->DesiredSpeed = leadplane->V;
	}
	return(0);
}

//**************************************************************************************
double AICheckHeightInPath(PlaneParams *planepnt, int minheight)
{
	double heading;
	double workingheight;
	int cnt;
	double workx, workz, worky;
	double angzx, angzz;

//	heading = DegToRad(AIConvertAngleTo180Degree(planepnt->Heading));
	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	workx = planepnt->WorldPosition.X;
	workz = planepnt->WorldPosition.Z;
	workingheight = LandHeight(workx, workz);

	for(cnt = 2000; cnt <= 10000; cnt += 2000)
	{
		workx = planepnt->WorldPosition.X - ConvertWayLoc(angzx * cnt);  // was + but I think should be -
		workz = planepnt->WorldPosition.Z - ConvertWayLoc(angzz * cnt);  // was + but I think should be -
		worky = LandHeight(workx, workz);
		if(worky > workingheight)
		{
			workingheight = worky;
		}
	}

	return(workingheight);
}

//**************************************************************************************
//  Does not take into account AOA
//**************************************************************************************
double AIGetDesiredPitch(PlaneParams *planepnt, double dy)
{
	double goalpitch = 0;

	if(fabs(dy) >  165000)  //  5000)
	{
		if(dy < 0)
		{
			goalpitch = - 10;  //  -45;
		}
		else
		{
			goalpitch = 10;  //  45;
		}
	}
	else if(fabs(dy) > 165)  //  500)  //  was 500
	{
		if(dy < 0)
		{
			goalpitch = -5;  //  -1 - (0.001 * dy);  //  -15;
		}
		else
		{
			goalpitch = 5;  //  1 + (0.001 * dy);  //  15;
		}
	}
	else
	{
#if 0
		if(dy > 20)
		{
			goalpitch = (dy - 20) / 32;  //   / 2;   //  4;  //   100;
		}
		else if (dy < -5)
		{
			goalpitch = (dy + 5) / 32;  //   / 2;   //  4;  //   100;
		}
#else
	  	goalpitch = dy / 32;  //   / 2;   //  4;  //   100;
#endif
	}

	return(goalpitch);
}

//**************************************************************************************
double AIGetPitchForBomb(double bx, double by, double bz, double tx, double ty, double tz, float vel, double planepitch)
{
#if 0
	double tdist, feetdist;
	double dx, dy, dz;
	double pitchtotarg;
//	double pitchtotarg2, pitchdeg, pitchdeg2;
	double vx, mod, tof;
	double retpitch;
//	double tempval;

	dx = tx - bx;
	dy = LandHeight(tx, tz) - by;
	dz = tz - bz;
	tdist = QuickDistance(dx,dz);
	feetdist = tdist * WUTOFT;

//	tempval = (dy / tdist);
	pitchtotarg = atan2(dy, tdist);
//	pitchdeg2 = RadToDeg(pitchtotarg2);
//	pitchtotarg = atan(tempval);
//	pitchdeg = RadToDeg(pitchtotarg);

	if(RadToDeg(pitchtotarg) < -75)		//  KLUDGE to handle high altitude mod inaccuracies
		return(RadToDeg(pitchtotarg));

	tof = feetdist / vel;

	vx = cos(pitchtotarg) * vel;
	mod = (feetdist / vx) * (32.1740 / vx);

	retpitch = atan((dy / tdist) + mod) * 57.2958;
//	retpitch = atan(-dy / -tdist) * 57.2958;

	return(retpitch);
#else
	double distance, tot, horzvel, alt, disttraveled;
	double dx, dy, dz, tdist;
	double dworkvar;
	double retpitch;

	dx = tx - bx;
	dy = LandHeight(tx, tz) - by;
	dz = tz - bz;
	tdist = QuickDistance(dx,dz);
	distance = tdist * WUTOFT;

	horzvel = cos(DegToRad(planepitch)) * vel;
	tot = distance / horzvel;
	alt = -dy * WUTOFT;

	disttraveled = tot * vel;
	if(!disttraveled)
	{
		return(92);
	}

	dworkvar = -2 * alt + tot * tot * 32;
	dworkvar = 0.5 * (dworkvar / disttraveled);

	if(fabs(dworkvar) > 1.0)
	{
		return(92);
	}

	retpitch = RadToDeg(asin(dworkvar));

	return(retpitch);
#endif
}

//**************************************************************************************
float AITimeToRelease(PlaneParams *planepnt)
{
	double VertVel;
	float Time;
	float weapondrag = 1.0f;
	float gravity = 32.0f;
	//  This function may not be needed since I have things working for pitch using a variation of
	//  the same formula.  Just to be sure, I'm going to test it, though if it does seem redundant
	//  I won't call it since it would just slow things down.

//	dy = LandHeight(tx, tz) - by;  //  may want to use actual height above target since this is an AI.


	VertVel = (double)planepnt->IfVelocity.Y;
 	weapondrag = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].fDrag;
	if(weapondrag > 1.0f)
	{
		gravity /= weapondrag;
		weapondrag = 1.0f;
	}
	else
	{
		VertVel *= weapondrag;
	}

	Time = (float)(1.0f/gravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*gravity)*(planepnt->HeightAboveGround * WUTOFT))));

	return(Time);
}

//**************************************************************************************
float AITimeToGround(PlaneParams *planepnt, double VertVel, float dy)
{
	float Time;
	float weapondrag = 1.0f;
	float gravity = 32.0f;
	//  This function may not be needed since I have things working for pitch using a variation of
	//  the same formula.  Just to be sure, I'm going to test it, though if it does seem redundant
	//  I won't call it since it would just slow things down.

//	dy = LandHeight(tx, tz) - by;  //  may want to use actual height above target since this is an AI.

 	weapondrag = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].fDrag;

// 	if(pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType == WEAPON_TYPE_CLUSTER_BOMB)
//	{
//		dy += 1000.0f * FTTOWU;
//	}

	if(weapondrag > 1.0f)
	{
		gravity /= weapondrag;
		weapondrag = 1.0f;
	}
	else
	{
		VertVel *= weapondrag;
	}

	Time = (float)(1.0f/gravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*gravity)*(fabs(dy) * WUTOFT))));

	return(Time);
}

//**************************************************************************************
float AICheckForAGMissileRelease(PlaneParams *planepnt, double bx, double by, double bz, double tx, double ty, double tz)
{
	double distance, weaponrange, checkdistance;
	double dx, dy, dz, tdist;
	float offangle;
	FPointDouble targetpos;
	FPointDouble junkpos;

	dx = tx - bx;
	dy = planepnt->AI.TargetPos.Y - by;
	dz = tz - bz;

	targetpos.X = tx;
	targetpos.Y = planepnt->AI.TargetPos.Y + (50.0f * FTTOWU);  //  20.0f
	targetpos.Z = tz;

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

	offangle = AICapAngle(offangle);

	if(fabs(offangle) > 30)
	{
		return(99);
	}

	tdist = sqrt((dx*dx) + (dz*dz));

	distance = tdist * WUTONM;

	weaponrange = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iRange;

	if(weaponrange > 2.0f)
	{
		checkdistance = distance * 1.5f;
	}
	else
	{
		checkdistance = distance * 1.1f;
	}

	if((checkdistance) < weaponrange)
	{
		if(planepnt->AI.pGroundTarget)
		{
			if(AILOS(planepnt->WorldPosition, targetpos))
			{
				return(0);
			}
		}
		else
		{
		 	return(0);
		}
	}

	return(99);
}

//**************************************************************************************
float AIVarTimeToRelease(PlaneParams *planepnt, double bx, double by, double bz, double tx, double ty, double tz, float predict_pitch, int ignorehighdrag)
{
	double distance, tot, time_bomb_in_air, dtime;
	double dx, dy, dz, tdist;
	float weapondrag = 1.0f;
	int cnt;
	DBWeaponType *pweapon_type = NULL;

	planepnt->AI.iAIFlags2 &= ~(AI_FAKE_AOA);

	dx = tx - bx;
//	dy = LandHeight(tx, tz) - by;
	dy = planepnt->AI.TargetPos.Y - by;
	if(ignorehighdrag)
	{
		if(dy > (-3000 * FTTOWU))
		{
			dy = -3000;
		}
	}
	dz = tz - bz;
	tdist = sqrt((dx*dx) +
//			  (DeltaPoint.Y*DeltaPoint.Y) +
			  (dz*dz));

	distance = tdist * WUTOFT;

	if(planepnt == PlayerPlane)
	{
		for (cnt = 0; cnt < MAX_F18E_STATIONS; cnt++)
		{
			if(BombStations[cnt]==TRUE)
			{
				pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
				weapondrag = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].fDrag;
				break;
			}
		}
	}
	else
	{
		pweapon_type = &pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex];
	 	weapondrag = pweapon_type->fDrag;
	}

	if(weapondrag > 1.0f)
	{
		weapondrag = 1.0f;
	}
	else if((ignorehighdrag) && (weapondrag < 1.0f))  //  Otherwise popup bombing can happen too late.
	{
		weapondrag = 1.0f;
	}

	if(pweapon_type->lWeaponID == 132)  
	{
		dy += (350.0f * FTTOWU);  //  I think dy is negative here.
	}

	tot = distance / (planepnt->IfHorzVelocity * weapondrag);

	if(predict_pitch > -900)
	{
		time_bomb_in_air = AITimeToGround(planepnt, FSinTimes(predict_pitch * DEGREE, planepnt->V), dy);
	}
	else
	{
		time_bomb_in_air = AITimeToGround(planepnt, planepnt->IfVelocity.Y, dy);
	}

	dtime = tot - time_bomb_in_air;
	if((pweapon_type->iSeekerType == 13) && (pweapon_type->iWeaponType == WEAPON_TYPE_DUMB_BOMB) && (dtime > 0))
	{
		//  Do some stuff for Torpedos.
		//  Make sure they are flying low and Level
		if(planepnt->AI.WayPosition.Y > (500.0f * FTTOWU))
		{
			planepnt->AI.WayPosition.Y = (500.0f * FTTOWU);
		}

		if((planepnt->AI.Behaviorfunc != AIAwayFromTarget) && (planepnt->AI.Behaviorfunc != AILevelBomb))
		{
			planepnt->AI.Behaviorfunc = AILevelBomb;
		}

		//  Have them dropped a 1/2 mile or less from normal release point
		distance -= (NMTOFT * 0.5f);
		if(distance < 0)
		{
			distance = 0;
		}
		tot = distance / (planepnt->IfHorzVelocity * weapondrag);

		if((tot - time_bomb_in_air) < 0)
		{
			dtime = 0;
		}
		else
		{
			dtime = tot - time_bomb_in_air;
		}
	}

	return(dtime);
}

//**************************************************************************************
void AIGroupLevelBomb(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float tdistnm;
	float dx, dy, dz;
	PlaneParams *leadplane;
	float activatedist;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	tdistnm = (tdist * WUTONM);

	if(tdistnm < 70)
	{
		AICheckEscortRelease(planepnt);
	}

	planepnt->AI.iAICombatFlags2 &= ~(AI_FAR_TARGET_BOMB);

	activatedist = BOMBTRAILDIST;
	int iweapontype = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType;

	if((iweapontype == WEAPON_TYPE_AG_MISSILE) || (iweapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
	{
		float weaponrange = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iRange;
		weaponrange *= NMTOFT;
		if(weaponrange > activatedist)
		{
			activatedist = weaponrange;
		}
	}

	if((tdist * WUTOFT) > activatedist)
	{
		AIFlyFormation(planepnt);
		return;
	}

	leadplane = AIGetLeader(planepnt);

	if(leadplane == planepnt)
	{
		if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
		{
			if((planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (planepnt->AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)))
			{
				AICBomberAttackMsg(planepnt);
			}
		}
	}
	AIChangeGroupBehavior(leadplane, planepnt->AI.Behaviorfunc, AILevelBomb);
	AILevelBomb(planepnt);

}

//**************************************************************************************
void AILevelBomb(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	float tdistnm;
	PlaneParams *leadplane;
	float releasetime;
	float multimod;
	int endbomb;
	int ismissile = 0;
	float weaponspacing = AI_RIPPLE_SPREAD;
	int iweapontype;
	int ismine = 0;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);

	iweapontype = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType;
	if((iweapontype == WEAPON_TYPE_NAVAL_MINE) || (pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iUsageIndex == 22))
	{
		ismine = 1;
	}
	else if((iweapontype == WEAPON_TYPE_AGROCKET) || (iweapontype == WEAPON_TYPE_GUN))
	{
		ismissile = 0;
		if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
		{
			AIUpdateAttackOnVehicle(planepnt);
		}

		releasetime = AICheckForStraightAttack(planepnt, planepnt->WorldPosition, planepnt->AI.TargetPos);
	}
	else if((iweapontype == WEAPON_TYPE_AG_MISSILE) || (iweapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
	{
		ismissile = 1;
		if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
		{
			AIUpdateAttackOnVehicle(planepnt);
		}

		releasetime = AICheckForAGMissileRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z);
	}
	else
	{
		if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
		{
			AIUpdateAttackOnVehicle(planepnt);
		}

	//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, planepnt->V, planepitch);
		releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z);

		weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
		multimod = (planepnt->AI.cNumWeaponRelease / 2) * (weaponspacing / planepnt->IfHorzVelocity);
		releasetime -= multimod;
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

//	if(planepitch > bombpitch)

	if(ismine)
	{
		if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
		{
			planepnt->AI.Behaviorfunc = AIDropChuteMines;
		}
	}
//	if(((releasetime < -1.0f) || ((releasetime < 10.0f) && (fabs(offangle) > 90))) && (!(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)))
	else if(((releasetime < -1.0f) || ((AIInNextRange(planepnt, tdist * WUTOFT,  releasetime * planepnt->IfHorzVelocity)) && (fabs(offangle) > 90))) && (!(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)))
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		planepnt->AI.Behaviorfunc = AIAwayFromTarget;
	}
	else if(((fabs(offangle) < 2.0f) || (planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)) && (planepnt->AI.lTimer1 < 0) && ((releasetime * 2000) < DeltaTicks)) //  if releasetime < 1/2 DeltaTicks in seconds
	{
		//  Drop Bomb
		if(planepnt->AI.cNumWeaponRelease)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				if(ismissile)
				{
					InstantiateAGMissile(planepnt, planepnt->AI.cActiveWeaponStation, planepnt->AI.pGroundTarget, planepnt->AI.TargetPos, planepnt->AI.lGroundTargetFlag);
				}
				else
				{
					InstantiateBomb(planepnt, planepnt->AI.cActiveWeaponStation, (1000 * (1.0f/5.7435)), planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z);
				}
			}
			planepnt->AI.iAIFlags2 |= AI_RIPPLE_BOMB;
			planepnt->AI.cNumWeaponRelease = planepnt->AI.cNumWeaponRelease - 1;
			planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count = planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count - 1;
		}

		if(planepnt->AI.cNumWeaponRelease)
		{
			weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
			planepnt->AI.lTimer1 = (weaponspacing / planepnt->IfHorzVelocity) * 1000;
			endbomb = 0;
			if(planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count <= 0)
			{
				if(!AISelectNextWeaponStation(planepnt))
				{
					endbomb = 1;
				}
			}
		}
		else
		{
			endbomb = 1;
		}

		if(endbomb)
		{
			planepnt->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;
			planepnt->AI.iVar1 = 0;
			leadplane = AIGetLeader(planepnt);
			if(!ismissile)
			{
#if F15_SPEECH
				if(leadplane == PlayerPlane)
				{
					AICBombDropMsg(planepnt);
	//				AICMaverickLaunchMsg(planepnt);
				}
				else if(leadplane == planepnt)
				{
					if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
					{
						if((planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (planepnt->AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)))
						{
							AICBomberBombDropMsg(planepnt);
						}
					}
				}
#else
				if(AIRIsAttack(planepnt - Planes))
				{
					AICBombDropMsg(planepnt);
				}
#endif
			}
			AISetUpEgress(planepnt);
			return;
		}
	}

	if(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)
	{
		planepnt->DesiredRoll = 0;
		return;
	}

	tdistnm = (tdist * WUTONM);

	if(tdistnm >= (BOMBTRAILDIST * 0.75 * FTTONM))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_FAR_TARGET_BOMB);
	}
	else if(!(planepnt->AI.iAICombatFlags2 & AI_FAR_TARGET_BOMB))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			desiredspeed -= ((planepnt->AI.iVar1 - 1) * 10.0f);
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
	}

	if((planepnt->AI.iVar1 > 1) && (tdistnm > 7.0))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
		desiredspeed = AIAdjustAttackSpeed(planepnt, tdist);
	}

	if((tdistnm < 7.0) && (!ismine))
	{
		if(!(planepnt->AI.iAIFlags1 & AIBOMBHOTMSG))
		{
			planepnt->AI.iAIFlags1 |= AIBOMBHOTMSG;
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICInHotMsg(planepnt);
			}
		}
	}

	MBAdjustAIThrust(planepnt, desiredspeed, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	return;
}

//**************************************************************************************
void AIGroupFlyToDivePoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistnm;
	float dx, dy, dz;
	PlaneParams *leadplane;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	tdistnm = (tdist * WUTONM);

	if(tdistnm < 70)
	{
		AICheckEscortRelease(planepnt);
	}

	planepnt->AI.iAICombatFlags2 &= ~(AI_FAR_TARGET_BOMB);

	if((tdist * WUTOFT) > BOMBTRAILDIST)
	{
		AIFlyFormation(planepnt);
		return;
	}

	leadplane = AIGetLeader(planepnt);

	if(leadplane == planepnt)
	{
		if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
		{
			if((planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (planepnt->AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)))
			{
				AICBomberAttackMsg(planepnt);
			}
		}
	}

	AIChangeGroupBehavior(leadplane, planepnt->AI.Behaviorfunc, AIFlyToDivePoint);
	AIFlyToDivePoint(planepnt);

}

//**************************************************************************************
void AIFlyToDivePoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz, dheight;
	double tempval;
	double targpitch, actpitch, divepitch;
	double pitchinc;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	float tdistnm;
	PlaneParams *leadplane;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	tdistnm = (tdist * WUTONM);

	if((planepnt->AI.iVar1 > 1) && (tdistnm > 5.0))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
		desiredspeed = AIAdjustAttackSpeed(planepnt, tdist);
	}

	if(tdistnm >= (BOMBTRAILDIST * 0.75 * FTTONM))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_FAR_TARGET_BOMB);
	}
	else if(!(planepnt->AI.iAICombatFlags2 & AI_FAR_TARGET_BOMB))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			desiredspeed -= ((planepnt->AI.iVar1 - 1) * 10.0f);
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
	}

	MBAdjustAIThrust(planepnt, desiredspeed, 1);

	dheight = (LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) - planepnt->WorldPosition.Y);
	tempval = -dheight / -tdist;
	dheight *= WUTOFT;
	actpitch = atan(tempval) * 57.2958;

	targpitch = divepitch = dheight * 0.0015;
	if(targpitch < -30)
	{
		targpitch = divepitch = -30;
	}
	else if(targpitch > -10)
	{
		targpitch = divepitch = -10;
	}

	pitchinc = targpitch * 0.10;

	if(pitchinc > -2.5)
	{
		pitchinc = -2.5;
	}
	targpitch += pitchinc;

//	AIChangeDir(planepnt, offangle, (dy * 60000 / 6979));
	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	if(actpitch < targpitch)
	{
		planepnt->AI.Behaviorfunc = AIStartDiveBomb;
		planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(divepitch);
		if(!(planepnt->AI.iAIFlags1 & AIBOMBHOTMSG))
		{
			planepnt->AI.iAIFlags1 |= AIBOMBHOTMSG;
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICInHotMsg(planepnt);
			}
		}
	}

	return;
}

//**************************************************************************************
int AIInvertedDive(PlaneParams *planepnt)
{
	double degroll, workroll, desiredroll;
	double offangle;
	float tdist;
	float dx, dy, dz;
	double dpitch, droll;
	ANGLE orgroll;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	float releasetime;
	double lastdisroll;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;  //  Don't do major slowdown near end of roll/pitch.

	degroll = AIConvertAngleTo180Degree(planepnt->Roll);
	lastdisroll = AIConvertAngleTo180Degree(planepnt->DesiredRoll);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	if((planepnt->WorldPosition.Y - planepnt->AI.TargetPos.Y) < (2000.0f * FTTOWU))
	{
		planepnt->AI.Behaviorfunc = AILevelBomb;
		planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000.0f * FTTOWU);
		AILevelBomb(planepnt);
		return(0);
	}

	orgroll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
	workroll = AIConvertAngleTo180Degree(orgroll);

	if(fabs(offangle) > 90.0f)
	{
		planepnt->DesiredRoll = 0;
		return(1);
	}
//	else if(fabs(offangle) < 1.0f)
//	{
//		planepnt->DesiredRoll = 0x8000;
//		planepnt->RollYawCouple = 0;
//	}
	else if(fabs(workroll) > 40)
	{
		workroll = (workroll < 0.0) ? -40 : 40;
		planepnt->DesiredRoll = 0x8000 - AIConvert180DegreeToAngle(workroll);
		planepnt->RollYawCouple = 1;
	}
	else
	{
		planepnt->DesiredRoll = 0x8000 - orgroll;
		planepnt->RollYawCouple = 1;
	}
	desiredroll = AIConvertAngleTo180Degree(planepnt->DesiredRoll);

	dpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch - planepnt->Pitch);

	droll = AIConvertAngleTo180Degree((0xFFFF - planepnt->DesiredRoll) - planepnt->Roll);

//	if((fabs(degroll) <= 90) && (dpitch < -2.0))
	if((!((fabs(droll) < 5) || ((fabs(desiredroll) > 90) && (fabs(degroll) > fabs(desiredroll))) || ((fabs(desiredroll) < 90) && (fabs(degroll) < fabs(desiredroll)))))
		&& ((dpitch < -2.0) && ((dpitch < -3.0) || (fabs(lastdisroll) > 90))))
	{
		planepnt->DesiredPitch = planepnt->AI.DesiredPitch;
		planepnt->RollYawCouple = 0;
		return(0);
	}
	else if((dpitch >= -2.0) || ((dpitch >= -3.0) && (fabs(lastdisroll) <= 90)))
	{
		releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z);
		if((releasetime > ((180.0f / planepnt->MaxRollRate) + 3.0f)) || (fabs(lastdisroll) <= 90))
		{
			planepnt->DesiredPitch = planepnt->AI.DesiredPitch;
			planepnt->DesiredRoll = orgroll;
			planepnt->RollYawCouple = 0;
			droll = AIConvertAngleTo180Degree((0xFFFF - orgroll) - planepnt->Roll);
			if((fabs(droll) < 5) || (fabs(degroll) < fabs(workroll)))
			{
				return(1);
			}
		}
		else
		{
			planepnt->AI.DesiredPitch -= AIConvert180DegreeToAngle(5.0f);
			planepnt->DesiredPitch = planepnt->AI.DesiredPitch;
		}
		return(0);
	}

	planepnt->DesiredPitch = planepnt->AI.DesiredPitch;

	return(0);
}

//**************************************************************************************
void AIStartDiveBomb(PlaneParams *planepnt)
{
	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.iAIFlags2 & AIGROUNDAVOID)
	{
		planepnt->AI.Behaviorfunc = AIDiveBomb;
		AIDiveBomb(planepnt);
	}
	else if(AIInvertedDive(planepnt))
	{
		planepnt->AI.Behaviorfunc = AIDiveBomb;
	}
}

//**************************************************************************************
void AIDiveBomb(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	PlaneParams *leadplane;
	float releasetime;
	float multimod;
	int endbomb;
	float weaponspacing = AI_RIPPLE_SPREAD;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;

//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, planepnt->V, planepitch);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z);

	weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
	multimod = (planepnt->AI.cNumWeaponRelease / 2) * (weaponspacing / planepnt->IfHorzVelocity);
	releasetime -= multimod;

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	if((planepnt->WorldPosition.Y - planepnt->AI.TargetPos.Y) < (2000.0f * FTTOWU))
	{
		planepnt->AI.Behaviorfunc = AILevelBomb;
		planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000.0f * FTTOWU);
		AILevelBomb(planepnt);
		return;
	}

	if(((AIInNextRange(planepnt, tdist * WUTOFT,  releasetime * planepnt->IfHorzVelocity)) && (fabs(offangle) > 90)) && (!(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)))
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		planepnt->AI.Behaviorfunc = AIAwayFromTarget;
	}
	else if(((fabs(offangle) < 2.0f) || (planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)) && (planepnt->AI.lTimer1 < 0) && (((releasetime * 2000) < DeltaTicks) && ((releasetime > -1.0f) || (planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)))) //  if releasetime < 1/2 DeltaTicks in seconds
	{

		//  Drop Bomb
//		planepnt->AI.Behaviorfunc = AIFlyFormation;

//		planepnt->AI.Behaviorfunc = AIAfterDiveBomb;
//		AIChangeDirPitch(planepnt, (double)0, AIConvert180DegreeToAngle(30));

		if(planepnt->AI.cNumWeaponRelease)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				InstantiateBomb(planepnt, planepnt->AI.cActiveWeaponStation, (1000 * (1.0f/5.7435)), planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z);
			}
			planepnt->AI.iAIFlags2 |= AI_RIPPLE_BOMB;
			planepnt->AI.cNumWeaponRelease = planepnt->AI.cNumWeaponRelease - 1;
			planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count = planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count - 1;
		}

		if(planepnt->AI.cNumWeaponRelease)
		{
			weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
			planepnt->AI.lTimer1 = (weaponspacing / planepnt->IfHorzVelocity) * 1000;
			endbomb = 0;
			if(planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count <= 0)
			{
				if(!AISelectNextWeaponStation(planepnt))
				{
					endbomb = 1;
				}
			}
		}
		else
		{
			endbomb = 1;
		}

		if(endbomb)
		{
			planepnt->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;
			if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
			{
				planepnt->AI.iVar1 = 0;
			}
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICBombDropMsg(planepnt);
//				AICMaverickLaunchMsg(planepnt);
			}
			AISetUpEgress(planepnt);
		}
		return;
	}

	if(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)
	{
		planepnt->DesiredRoll = 0;
		return;
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	planepnt->Brakes = 0;

//	AIChangeDir(planepnt, offangle, (dy * 60000 / 6979));
	AIChangeDirPitch(planepnt, (double)offangle, planepnt->AI.DesiredPitch);
	return;
}

//**************************************************************************************
void AIAfterDiveBomb(PlaneParams *planepnt)
{

	double dground;
	double degroll;
	double degpitch;
	double dfinalelev = 0;
	double daileron = 0;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	dground = planepnt->HeightAboveGround * WUTOFT;// ((planepnt->WorldPosition.Y - LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z)) * WUTOFT) ;

	degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

	if(degpitch > 0)
	{
//		planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 100.0;
		if(planepnt->AI.lDesiredSpeed > 600)
		{
			MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);  //  temp for now
		}
		else
		{
			MBAdjustAIThrust(planepnt, 600, 1);
		}

		if(planepnt->Brakes)
			planepnt->Brakes = 0;

		planepnt->AI.iVar1 = 0;
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		AINextWayPoint(planepnt);
	}
	else
	{
//		planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 0.0;
		MBAdjustAIThrust(planepnt, 250, 1);  //  temp for now
	}


	degroll = AIConvertAngleTo180Degree(planepnt->Roll);
	planepnt->DesiredRoll = 0;

	if((abs(degroll) < 90.0) && (degpitch < 30.0))
	{
		planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(20);
	}
}


//**************************************************************************************
void AIGroupFlyToLoftPoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistnm;
	float dx, dy, dz;
	PlaneParams *leadplane;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	tdistnm = (tdist * WUTONM);

	if(tdistnm < 70)
	{
		AICheckEscortRelease(planepnt);
	}

	planepnt->AI.iAICombatFlags2 &= ~(AI_FAR_TARGET_BOMB);

	if((tdist * WUTOFT) > BOMBTRAILDIST)
	{
		AIFlyFormation(planepnt);
		return;
	}

	leadplane = AIGetLeader(planepnt);

	if(leadplane == planepnt)
	{
		if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
		{
			if((planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (planepnt->AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)))
			{
				AICBomberAttackMsg(planepnt);
			}
		}
	}

	AIChangeGroupBehavior(leadplane, planepnt->AI.Behaviorfunc, AIFlyToLoftPoint);
	AIFlyToLoftPoint(planepnt);

}

//**************************************************************************************
void AIFlyToLoftPoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistnm;
	float dx, dy, dz;
//	double bombpitch;
	float releasetime;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	PlaneParams *leadplane;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	if((tdist * WUTOFT) > BOMBTRAILDIST)
	{
		AIFlyFormation(planepnt);
		return;
	}

	tdistnm = (tdist * WUTONM);

	if((planepnt->AI.iVar1 > 1) && (tdistnm > 5.0))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
		desiredspeed = AIAdjustAttackSpeed(planepnt, tdist);
	}

	if(tdistnm >= (BOMBTRAILDIST * 0.75 * FTTONM))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_FAR_TARGET_BOMB);
	}
	else if(!(planepnt->AI.iAICombatFlags2 & AI_FAR_TARGET_BOMB))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			desiredspeed -= ((planepnt->AI.iVar1 - 1) * 10.0f);
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
	}

	MBAdjustAIThrust(planepnt, desiredspeed, 1);

//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, planepnt->V, AIConvertAngleTo180Degree(planepnt->Pitch));
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, 35.0f);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

//	if(bombpitch < 35)
	if((releasetime * 1000) < DeltaTicks)
	{
		planepnt->AI.Behaviorfunc = AILoftBomb;
		planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(20);
		if(!(planepnt->AI.iAIFlags1 & AIBOMBHOTMSG))
		{
			planepnt->AI.iAIFlags1 |= AIBOMBHOTMSG;
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICInHotMsg(planepnt);
			}
		}
	}

	return;
}

//**************************************************************************************
void AILoftBomb(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	PlaneParams *leadplane;
	float releasetime;
	float multimod;
	int endbomb;
	float weaponspacing = AI_RIPPLE_SPREAD;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;

//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, planepnt->V, planepitch);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z);

	weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
	multimod = (planepnt->AI.cNumWeaponRelease / 2) * (weaponspacing / planepnt->IfHorzVelocity);
	releasetime -= multimod;

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

//	if(planepitch > bombpitch)
	if(((releasetime < -1.0f) || ((AIInNextRange(planepnt, tdist * WUTOFT,  releasetime * planepnt->IfHorzVelocity)) && (fabs(offangle) > 90))) && (!(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)))
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		planepnt->AI.Behaviorfunc = AIAwayFromTarget;
	}
	else if(((fabs(offangle) < 2.0f) || (planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)) && (planepnt->AI.lTimer1 < 0) && ((releasetime * 2000) < DeltaTicks)) //  if releasetime < 1/2 DeltaTicks in seconds
	{
		//  Drop Bomb
		if(planepnt->AI.cNumWeaponRelease)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				InstantiateBomb(planepnt, planepnt->AI.cActiveWeaponStation, (1000 * (1.0f/5.7435)), planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z);
			}
			planepnt->AI.iAIFlags2 |= AI_RIPPLE_BOMB;
			planepnt->AI.cNumWeaponRelease = planepnt->AI.cNumWeaponRelease - 1;
			planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count = planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count - 1;
		}

		if(planepnt->AI.cNumWeaponRelease)
		{
			weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
			planepnt->AI.lTimer1 = (weaponspacing / planepnt->IfHorzVelocity) * 1000;
			endbomb = 0;
			if(planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count <= 0)
			{
				if(!AISelectNextWeaponStation(planepnt))
				{
					endbomb = 1;
				}
			}
		}
		else
		{
			endbomb = 1;
		}

		if(endbomb)
		{
			planepnt->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;
			planepnt->AI.iVar1 = 0;
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICBombDropMsg(planepnt);
//				AICMaverickLaunchMsg(planepnt);
			}
			AISetUpEgress(planepnt);
			return;
		}
	}

	if(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)
	{
		planepnt->DesiredRoll = 0;
		return;
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	planepnt->Brakes = 0;

//	AIChangeDir(planepnt, offangle, (dy * 60000 / 6979));
	AIChangeDirPitch(planepnt, (double)offangle, planepnt->AI.DesiredPitch);
	return;
}

//**************************************************************************************
void AIGroupFlyToPopPoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistnm;
	float dx, dy, dz;
	PlaneParams *leadplane;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	tdistnm = (tdist * WUTONM);

	if(tdistnm < 70)
	{
		AICheckEscortRelease(planepnt);
	}

	planepnt->AI.iAICombatFlags2 &= ~(AI_FAR_TARGET_BOMB);

	if((tdist * WUTOFT) > BOMBTRAILDIST)
	{
		AIFlyFormation(planepnt);
		return;
	}

	leadplane = AIGetLeader(planepnt);

	if(leadplane == planepnt)
	{
		if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
		{
			if((planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (planepnt->AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)))
			{
				AICBomberAttackMsg(planepnt);
			}
		}
	}

	AIChangeGroupBehavior(leadplane, planepnt->AI.Behaviorfunc, AIFlyToPopPoint);
	AIFlyToPopPoint(planepnt);

}

//**************************************************************************************
void AIFlyToPopPoint(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistnm, tdistft;
	float dx, dy, dz;
//	double bombpitch;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	int planepitch;
	float usespeed;
	int iworkangle;
	float releasetime;
	float weapondrag;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	tdistft = tdist * WUTOFT;

	tdistnm = tdistft * FTTONM;

	if((planepnt->AI.iVar1 > 1) && (tdistnm > 5.0) && (!(planepnt->AI.iAIFlags2 & AI_CAS_ATTACK)))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
		desiredspeed = AIAdjustAttackSpeed(planepnt, tdist);
	}

	if(tdistnm >= (BOMBTRAILDIST * 0.75 * FTTONM))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_FAR_TARGET_BOMB);
	}
	else if(!(planepnt->AI.iAICombatFlags2 & AI_FAR_TARGET_BOMB))
	{
		if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
		{
			desiredspeed -= ((planepnt->AI.iVar1 - 1) * 10.0f);
			dy += ((planepnt->AI.iVar1 - 1) * ATTACKALTSPACING) * FTTOWU;
		}
	}

	MBAdjustAIThrust(planepnt, desiredspeed, 1);

	planepitch = AIConvertAngleTo180Degree(planepnt->Pitch);

	usespeed = AIConvertSpeedToFtPS(planepnt->Altitude, planepnt->AI.lDesiredSpeed, planepnt->AI.iAIFlags1);

	if(usespeed < planepnt->V)
	{
		usespeed = planepnt->V;
	}

//	iworkangle = (1012 - usespeed) / 68;
	iworkangle = (1012 - planepnt->V) / 68;
	if(abs(iworkangle) > 5)
	{
		iworkangle = (iworkangle < 0) ? -5: 5;
	}

//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, usespeed, planepitch);
//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) - (100 * FTTOWU), planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, usespeed, planepitch);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) - (100 * FTTOWU), planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, (45.0f + (iworkangle<<1)), 1);
//	releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) - (100 * FTTOWU), planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, (45.0f + (iworkangle<<1))) - 5.0f;

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

//	if((bombpitch < 45) || ((tdistft / planepnt->V) < 50))  //  35
//	if(bombpitch < (45 + (iworkangle<<1)))
	if((releasetime * 1000) < DeltaTicks)
	{
		planepnt->AI.Behaviorfunc = AIPopUpBomb;

	 	weapondrag = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].fDrag;

		if(weapondrag < 1.0f)
		{
			planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(20 + iworkangle);
		}
		else
		{
			planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(30 + iworkangle);
		}

//		planepnt->AI.lTimer1 = 10000;  //  5000
		if(planepnt->AI.iVar1 & 1)
		{
			planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(offangle + 45) + planepnt->Heading;
		}
		else
		{
			planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(offangle - 45) + planepnt->Heading;
		}
	}

	return;
}

//**************************************************************************************
void AIPopUpBomb(PlaneParams *planepnt)
{
	double offangle, workroll;
	float tdist;
	float dheight;
	double tempval;
	double actpitch;
	double pitchinc, orgpitch;
	PlaneParams *leadplane;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;

	dheight = (LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) - planepnt->WorldPosition.Y);
	tdist = QuickDistance(planepnt->AI.WayPosition.X - planepnt->WorldPosition.X, planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z);
	tempval = -dheight / -tdist;
	dheight *= WUTOFT;
	orgpitch = actpitch = atan(tempval) * 57.2958;

	pitchinc = actpitch * 0.10;

	if(pitchinc > -2.5)
	{
		pitchinc = -2.5;
	}
	actpitch -= pitchinc;

//	if(0 > planepnt->AI.lTimer1)
	if(actpitch < -10)
	{
		planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(actpitch);
		planepnt->AI.Behaviorfunc = AIStartDiveBomb;
		AIStartDiveBomb(planepnt);
//		planepnt->AI.Behaviorfunc = AIPopUpPullIn;
//		AIPopUpPullIn(planepnt);
		if(!(planepnt->AI.iAIFlags1 & AIBOMBHOTMSG))
		{
			planepnt->AI.iAIFlags1 |= AIBOMBHOTMSG;
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICInHotMsg(planepnt);
			}
		}
		return;
	}

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	planepnt->Brakes = 0;

//	AIChangeDir(planepnt, offangle, (dy * 60000 / 6979));
	AIChangeDirPitch(planepnt, (double)offangle, planepnt->AI.DesiredPitch);

	workroll = AIConvertAngleTo180Degree(planepnt->DesiredRoll);

	if(fabs(workroll) > 60)
	{
		workroll = (workroll < 0.0) ? -60 : 60;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(workroll);
	}

	return;
}

//**************************************************************************************
void AIPopUpPullIn(PlaneParams *planepnt)
{
	double degroll, workroll;
	double offangle;
	float tdist;
	float dx, dy, dz;
	double dpitch, droll;
	ANGLE orgroll;
	long desiredspeed = planepnt->AI.lDesiredSpeed;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;

	degroll = AIConvertAngleTo180Degree(planepnt->Roll);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);


	orgroll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
	workroll = AIConvertAngleTo180Degree(orgroll);


	if(fabs(workroll) > 60)
	{
		workroll = (workroll < 0.0) ? -60 : 60;
		planepnt->DesiredRoll = 0x8000 - AIConvert180DegreeToAngle(workroll);
	}
	else
	{
		planepnt->DesiredRoll = 0x8000 - orgroll;
	}

	dpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch - planepnt->Pitch);

	if((fabs(degroll) <= 90) && (dpitch < -2.0))
	{
		planepnt->RollYawCouple = 0;
		return;
	}
	else if(dpitch >= -2.0)
	{
		planepnt->DesiredPitch = planepnt->AI.DesiredPitch;
		planepnt->DesiredRoll = orgroll;
		planepnt->RollYawCouple = 0;
		droll = AIConvertAngleTo180Degree((0xFFFF - orgroll) - planepnt->Roll);
		if((fabs(droll) < 5) || (fabs(degroll) < fabs(workroll)))
		{
			planepnt->AI.Behaviorfunc = AIDiveBomb;
		}
		return;
	}

	planepnt->DesiredPitch = planepnt->AI.DesiredPitch;

}

//**************************************************************************************
void AIChangeDirPitch(PlaneParams *planepnt, double offangle, ANGLE desiredpitch)
{
	planepnt->DesiredPitch = desiredpitch;
	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
}

//**************************************************************************************
void AIUpdateWaypointBehavior(int planenum, MBWayPoints *waypnt, int numwaypts)
{
	int wingman, nextsec;
	void (*holdbehavior)(PlaneParams *planepnt);

	if(Planes[planenum].AI.Behaviorfunc == AIFlyFormation)
	{
		Planes[planenum].AI.CurrWay = waypnt;
		Planes[planenum].AI.numwaypts = numwaypts;
		AINextWayPoint(&Planes[planenum]);
	}
	else if(Planes[planenum].AI.OrgBehave == AIFlyFormation)
	{
		Planes[planenum].AI.CurrWay = waypnt;
		Planes[planenum].AI.numwaypts = numwaypts;
		holdbehavior = Planes[planenum].AI.Behaviorfunc;
		Planes[planenum].AI.Behaviorfunc = Planes[planenum].AI.OrgBehave;
		AINextWayPoint(&Planes[planenum]);
		Planes[planenum].AI.OrgBehave = Planes[planenum].AI.Behaviorfunc;
		Planes[planenum].AI.Behaviorfunc = holdbehavior;
	}
	else if(Planes[planenum].AI.iAIFlags1 & AIINFORMATION)
	{
		Planes[planenum].AI.CurrWay = waypnt;
		Planes[planenum].AI.numwaypts = numwaypts;
		AINextWayPoint(&Planes[planenum]);
	}

	wingman = Planes[planenum].AI.wingman;
	nextsec = Planes[planenum].AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateWaypointBehavior(wingman, waypnt, numwaypts);
	}

	if(nextsec >= 0)
	{
		AIUpdateWaypointBehavior(nextsec, waypnt, numwaypts);
	}
}

//**************************************************************************************
void AIChangeGroupBehavior(PlaneParams *planepnt, void (*orgbehavior)(PlaneParams *planepnt), void (*newbehavior)(PlaneParams *planepnt), int checkbehave, int clearorg)
{
	int wingman, nextsec;

	if(checkbehave)
	{
		if(planepnt->AI.Behaviorfunc == orgbehavior)
		{
			planepnt->AI.Behaviorfunc = newbehavior;
		}
		if(planepnt->AI.OrgBehave == orgbehavior)
		{
			planepnt->AI.OrgBehave = newbehavior;
		}
	}
	else
	{
		if(planepnt->AI.Behaviorfunc == CTWaitingForLaunch)
		{
	 		planepnt->AI.OrgBehave = newbehavior;
		}
		else if(planepnt->AI.Behaviorfunc == AIFlyTakeOff)
		{
	 		planepnt->AI.OrgBehave = newbehavior;
		}
		else
		{
	 		planepnt->AI.Behaviorfunc = newbehavior;
			if(clearorg)
			{
				planepnt->AI.OrgBehave = NULL;
			}
		}
	}

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;

	if(wingman >= 0)
	{
		AIChangeGroupBehavior(&Planes[wingman], orgbehavior, newbehavior, checkbehave);
	}

	if(nextsec >= 0)
	{
		AIChangeGroupBehavior(&Planes[nextsec], orgbehavior, newbehavior, checkbehave);
	}
}

//**************************************************************************************
void AIGetPlaceInAttack(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	int attacknum = 0;

	leadplane = AIGetLeader(planepnt);
	AIGetAttackersInGroup(leadplane, planepnt, &attacknum);
	planepnt->AI.iVar1 = attacknum + 1;
}

//**************************************************************************************
void AIGetAttackersInGroup(PlaneParams *planepnt, PlaneParams *skipplane, int *attacknum)
{
	if(planepnt != skipplane)
	{
		if(planepnt->AI.Behaviorfunc == skipplane->AI.Behaviorfunc)
		{
			if(planepnt->AI.CurrWay == skipplane->AI.CurrWay)
			{
				if(*attacknum < planepnt->AI.iVar1)
				{
					*attacknum = planepnt->AI.iVar1;
				}
			}
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIGetAttackersInGroup(&Planes[planepnt->AI.wingman], skipplane, attacknum);
	}
	if(planepnt->AI.nextpair >= 0)
	{
		AIGetAttackersInGroup(&Planes[planepnt->AI.nextpair], skipplane, attacknum);
	}
}

//**************************************************************************************
long AIAdjustAttackSpeed(PlaneParams *planepnt, float tdist)
{
	PlaneParams *leadplane, *attacklead;
	double aoffangle;
	float atdist;
	float adx, ady, adz;
	float diffdist, desireddiff;
	float diffdistnm;
	double leadspeed;
	float ClipAlt = planepnt->Altitude;
	double IAmod;
	int		doindicated = 1;
	int speedchng;
	int numattack = 0;
	long desiredspeed = planepnt->AI.lDesiredSpeed;
	int ishelo = 0;
	int minspeed = 300;
	float secspace = 30.0f;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		ishelo = 1;
		minspeed = 50;
		secspace = 10.0f;
	}

	if(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))
	{
		return(planepnt->AI.lDesiredSpeed);
	}
	else if(planepnt->AI.iAICombatFlags1 & AI_FORMON_SEAD)
	{
		return(planepnt->AI.lDesiredSpeed);
	}

	leadplane = AIGetLeader(planepnt);

	if(ishelo)
	{
		speedchng = (5 * (planepnt->AI.iVar1 - 1));
	}
	else
	{
		speedchng = (25 * (planepnt->AI.iVar1 - 1));
	}
	attacklead = AIGetAttackLeader(leadplane, planepnt);
	if(attacklead == NULL)
	{
		attacklead = leadplane;
	}

	aoffangle = AIComputeWayPointHeading(attacklead, &atdist, &adx ,&ady, &adz, 1);

	desireddiff = attacklead->V * (secspace * (planepnt->AI.iVar1 - 1));  //  add distance of 30 seconds
	diffdist = (tdist - atdist) * WUTOFT;
	diffdist = desireddiff - diffdist;
	diffdistnm = diffdist * FTTONM;

	if(doindicated)
	{
		if (ClipAlt < 0.0) ClipAlt = 0.0;
		if (ClipAlt > 70000.0) ClipAlt = 70000.0;

		IAmod = sqrt(Pressure[(int)(ClipAlt/500.0)]/0.0023769);


		leadspeed = (double)attacklead->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS) * IAmod;
	}
	else
	{
		leadspeed = (double)attacklead->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}

	AIGetAttackersInGroup(leadplane, planepnt, &numattack);

	if(numattack > 1)
	{
		speedchng = (leadspeed - minspeed) / (numattack - 1);
		speedchng *= (planepnt->AI.iVar1 - 1);
	}

	if(diffdist > 0)
	{
//		if((leadspeed > leadplane->AI.lDesiredSpeed) && (diffdistnm > 0.05))
		if(leadspeed > leadplane->AI.lDesiredSpeed)
		{
			leadspeed = leadplane->AI.lDesiredSpeed;
		}

		if(diffdistnm > 0.5)
		{
			desiredspeed = leadspeed - speedchng;
		}
		else
		{
			desiredspeed = leadspeed - (speedchng * 2 * diffdistnm);
		}

//*		if(diffdist > 30)
//*		{
//*			MBAISBrakeOn(planepnt, 1.0);
//*		}

		if(desiredspeed < minspeed)
		{
			if(leadspeed < minspeed)
				desiredspeed = leadspeed;
			else
				desiredspeed = minspeed;
		}
	}
	else
	{
//		if((leadspeed < leadplane->AI.lDesiredSpeed) && (diffdistnm > 0.05))
		if(leadspeed < leadplane->AI.lDesiredSpeed)
		{
			leadspeed = leadplane->AI.lDesiredSpeed;
		}

		if(planepnt->SpeedBrakeCommandedPos != 0.0)
		{
			planepnt->SpeedBrakeCommandedPos = 0.0;
		}

#if 0
		if((leadplane->V > planepnt->V) && (leadplane->RightThrustPercent > planepnt->RightThrustPercent))
		{
			planepnt->LeftThrustPercent = planepnt->RightThrustPercent = leadplane->RightThrustPercent;
		}
#endif

		if(diffdistnm < -0.5)
		{
			desiredspeed = leadspeed + speedchng;
		}
		else
		{
			desiredspeed = leadspeed + (speedchng * 2 * diffdistnm);
		}
	}

	return(desiredspeed);
}

//**************************************************************************************
PlaneParams *AIGetAttackLeader(PlaneParams *planepnt, PlaneParams *baseplane)
{
	PlaneParams *workplane;

	if(planepnt->AI.iVar1 == 1)
	{
		if(planepnt->AI.Behaviorfunc == baseplane->AI.Behaviorfunc)
		{
			return(planepnt);
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		workplane = AIGetAttackLeader(&Planes[planepnt->AI.wingman], baseplane);
		if(workplane != NULL)
		{
			return(workplane);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		workplane = AIGetAttackLeader(&Planes[planepnt->AI.nextpair], baseplane);
		if(workplane != NULL)
		{
			return(workplane);
		}
	}

	return(NULL);
}

//**************************************************************************************
void AIMovePlaneToEndOfAttack(PlaneParams *planepnt)
{
	int orgplace;
	PlaneParams *leadplane;
	int attacknum = 0;

	orgplace = planepnt->AI.iVar1;

	leadplane = AIGetLeader(planepnt);
	AIGetAttackersInGroup(leadplane, planepnt, &attacknum);

	planepnt->AI.iVar1 = attacknum + 1;

	if(planepnt->AI.OrgBehave == NULL)
	{
		AIRedistributeAttackers(leadplane, planepnt->AI.Behaviorfunc, orgplace);
	}
	else
	{
		AIRedistributeAttackers(leadplane, planepnt->AI.OrgBehave, orgplace);
	}
}

//**************************************************************************************
void AIRedistributeAttackers(PlaneParams *planepnt, void (*attackbehavior)(PlaneParams *planepnt), int vacantnum)
{
	if(planepnt->AI.Behaviorfunc == attackbehavior)
	{
		if(planepnt->AI.iVar1 > vacantnum)
		{
			planepnt->AI.iVar1 -= 1;
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIRedistributeAttackers(&Planes[planepnt->AI.wingman], attackbehavior, vacantnum);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIRedistributeAttackers(&Planes[planepnt->AI.nextpair], attackbehavior, vacantnum);
	}
}

//**************************************************************************************
void AIResetPercents(PlaneParams *planepnt)
{
	long tempxx, tempzz, wayxx, wayzz;
	float airdensity, stalleffect;
	float fworkval;
	int humanescortOK;
	int hojmissile;

	lDebugPlaneSRE = planepnt - Planes;

	if((iHotZoneCheckFlags & ZONES_GET_MOVERS) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		MAISetAHotZone((float)planepnt->WorldPosition.X, (float)planepnt->WorldPosition.Z);
	}

#if 0
	if(TimeExcel)
	{
		planepnt->DoAttitude(planepnt);
	}
#endif

	//  New flight stuff.  Vary yaw/pitch/roll by difference in air density.  By messing with
	//  these variables in this way, it should be transparent.

	if(planepnt == PlayerPlane)
	{
		if((planepnt->AI.Behaviorfunc != AIFlyTankerFormation) && (planepnt->AI.Behaviorfunc != AITankerDisconnect))
		{
			planepnt->AI.iAIFlags2 &= ~(AI_FAKE_AOA);
		}
	}
	else
	{
		planepnt->AI.iAIFlags2 &= ~(AI_FAKE_AOA);
	}

	if(!((planepnt == PlayerPlane) || (planepnt->AI.iAIFlags2 & AILANDING) || (planepnt->OnGround) || (planepnt->AI.iAIFlags2 & AIGROUNDAVOID) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		if((planepnt->Knots > 5) && (planepnt->IndicatedAirSpeed > 5))
		{
			if(planepnt->IndicatedAirSpeed < planepnt->Knots)
			{
				airdensity = planepnt->IndicatedAirSpeed / planepnt->Knots;
			}
			else
			{
				airdensity = 1.0f;
			}
		}
		else
		{
			airdensity = 1.0f;
		}

		if((pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed) && (planepnt->IndicatedAirSpeed))
		{
			stalleffect = (planepnt->IndicatedAirSpeed / (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed);
			if(stalleffect < 2.0f)
			{
				planepnt->AI.iAIFlags2 |= (AI_FAKE_AOA);
			}

			stalleffect -= 0.7f;

			if(stalleffect > 1.0f)
			{
				stalleffect = 1.0f;
			}
		}
		else
		{
			if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_VERTICAL))
			{
				if((planepnt->Knots >= 90.0f) && (planepnt->Knots < 120.0f))
				{
					planepnt->AI.iAIFlags2 |= (AI_FAKE_AOA);
				}
			}

			stalleffect = 1.0f;
		}

		if(stalleffect < 0.50f)  //  was 0.30f
		{
			stalleffect = 0.50f;
		}
		else if(stalleffect > 1.0f)
		{
			stalleffect = 1.0f;
		}
		airdensity *= stalleffect;

		if(airdensity < 0.2f)
		{
			airdensity = 0.2f;
		}
		planepnt->YawRate = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate * airdensity;
		planepnt->MaxPitchRate = 10.0f * airdensity;
		if(planepnt->DesiredPitch > 0x8000)
		{
			planepnt->MaxPitchRate *= 3.0f;
			if(planepnt->MaxPitchRate > 10.0f)
			{
				planepnt->MaxPitchRate = 10.0f;
			}
		}
		planepnt->MaxRollRate = 90.0f * airdensity;
		if((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
				|| ((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (planepnt->Knots < 150.0f)))
		{
			planepnt->MaxRollRate /= 3.0f;
		}
		else if(planepnt->AI.iAIFlags2 & AI_IN_DOGFIGHT)
		{
			if(planepnt->AI.iSkill == GP_VETERAN)
			{
				planepnt->MaxRollRate *= 1.5f;
				planepnt->YawRate *= 1.5f;
			}
			else if(planepnt->AI.iSkill <= GP_EXPERT)
			{
				planepnt->MaxRollRate *= 2.0f;
				planepnt->YawRate *= 2.0f;
			}
		}
	}
	else
	{
		planepnt->YawRate = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate;
		planepnt->MaxPitchRate = 10.0f;
		planepnt->MaxRollRate = 90.0f;
		if((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
				|| ((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (planepnt->Knots < 150.0f)))
		{
			planepnt->MaxRollRate /= 3.0f;
		}
		else if(planepnt->AI.iAIFlags2 & AI_IN_DOGFIGHT)
		{
			if(planepnt->AI.iSkill == GP_VETERAN)
			{
				planepnt->MaxRollRate *= 1.5f;
				planepnt->YawRate *= 1.5f;
			}
			else if(planepnt->AI.iSkill <= GP_EXPERT)
			{
				planepnt->MaxRollRate *= 2.0f;
				planepnt->YawRate *= 2.0f;
			}
		}
	}

	if((pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 4) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 62))
	{
		fworkval = (planepnt->IndicatedAirSpeed / (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed);

		if(planepnt->AI.cFXarray[1] == 0)
		{
			if(fworkval > 1.7)
			{
				planepnt->AI.cFXarray[1] = 255;
			}
			else
			{
				planepnt->AI.cFXarray[1] = 0;
			}
		}
		else
		{
			if(fworkval > 1.5)
			{
				planepnt->AI.cFXarray[1] = 255;
			}
			else
			{
				planepnt->AI.cFXarray[1] = 0;
			}
		}

		if(planepnt->OnGround)
		{
			if((planepnt->AI.Behaviorfunc != AIWaitForTrapOrBolter) && (planepnt->AI.Behaviorfunc != AICarrierTrap) && (planepnt->AI.Behaviorfunc != AILandingTouchdown) && (planepnt->AI.Behaviorfunc != AILandingRollout))
			{
				planepnt->AI.cFXarray[1] = 255;
			}
		}
	}

	//  Also modify it for if we are less than twice our stall speed.

//	if(pPadLockTarget == NULL)
	if((!((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))) || (Camera1.TargetObjectType != CO_PLANE))
	{
		planepnt->AI.iAIFlags2 &= ~AI_PADLOCKED;
	}

	if(!((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (planepnt == PlayerPlane)))
	{
		hojmissile = 0;
		if(planepnt->AI.iMissileThreat >= 0)
		{
			if(Weapons[planepnt->AI.iMissileThreat].Flags2 & MISSILE_HOME_ON_JAM)
			{
				hojmissile = 1;
			}
		}

		if((hojmissile) || ((planepnt->AI.iMissileThreat < 0) && (!(planepnt->AI.iAIFlags1 & AI_KEEP_JAMMER_ON)) && (!(planepnt->AI.iAICombatFlags2 & AI_STT_THREAT)) && (iFirstTimeInFrame)) || ((planepnt->AI.iAIFlags2 & AILANDING) && (planepnt->AI.iAIFlags1 & (AIJAMMINGON))))
		{
			if((planepnt->AI.lPlaneID == 61) && (planepnt->AI.iAICombatFlags2 & AI_NOISE_JAMMER))
			{
				if(planepnt->AI.iAIFlags1 & (AIJAMMINGON))
				{
					RetractTowedDecoy(planepnt);
				}
			}
			planepnt->AI.iAIFlags1 &= ~(AIJAMMINGON);
		}
		else if(((planepnt->AI.iMissileThreat >= 0) && (planepnt->AI.iAIFlags1 & AI_INTERNAL_JAMMER)) || ((planepnt->AI.iAICombatFlags2 & AI_STT_THREAT) && (planepnt->AI.iAICombatFlags2 & AI_GATE_STEALER)))
		{
			if((planepnt->AI.lPlaneID == 61) && (planepnt->AI.iAICombatFlags2 & AI_NOISE_JAMMER))
			{
				if(!(planepnt->AI.iAIFlags1 & (AIJAMMINGON)))
				{
					DeployTowedDecoy(planepnt);
				}
			}
			planepnt->AI.iAIFlags1 |= (AIJAMMINGON);
			planepnt->AI.iAICombatFlags2 &= ~AI_STT_THREAT;
		}
	}

	if(iInJump)
	{
		if(planepnt->AI.AirTarget != NULL)
		{
			if((planepnt->AI.AirTarget == PlayerPlane) || (planepnt->AI.AirTarget->AI.iAIFlags1 & AIPLAYERGROUP))
			{
				planepnt->AI.AirTarget = NULL;
				if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
				{
					planepnt->AI.lCombatTimer = -1;
					planepnt->AI.CombatBehavior = NULL;
				}
			}
		}
		if(planepnt->AI.AirThreat != NULL)
		{
			if((planepnt->AI.AirThreat == PlayerPlane) || (planepnt->AI.AirThreat->AI.iAIFlags1 & AIPLAYERGROUP))
			{
				planepnt->AI.AirThreat = NULL;
			}
		}
	}

	tempxx = planepnt->WorldPosition.X;
	tempzz = planepnt->WorldPosition.Z;
	wayxx = planepnt->AI.WayPosition.X;
	wayzz = planepnt->AI.WayPosition.Z;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		AIRemoveFromFormation(planepnt);
		planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
		if((planepnt->OnGround) && (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
		{
//			planepnt->FlightStatus |= PL_STATUS_CRASHED;
#if 0  //  Redudant, I think Damage plane calls RemoveFromFormation which does this as well.
			if(!(planepnt->FlightStatus & PL_STATUS_CRASHED))
			{
//				AICheckForSAR(planepnt);
				AIUpdateSAR(planepnt);
			}
#endif
			if(!(planepnt->FlightStatus & (PL_STATUS_CRASHED)))
			{
				CrashPlane(planepnt,PL_STATUS_CRASHED,0,NULL);
			}
		}
	}

	planepnt->SpeedPercentage = (1.0 + (planepnt->SpeedPercentage)) / 2;
	planepnt->RollPercentage = (1.0 + (planepnt->RollPercentage)) / 2;
	planepnt->PitchPercentage = (1.0 + (planepnt->PitchPercentage)) / 2;
//	planepnt->SpeedPercentage = (1.0 + planepnt->SpeedPercentage) / 2;
	planepnt->RollYawCouple = 1;
	planepnt->AI.iAIFlags1 &= ~(AIFASTPITCHROLL|AIINFORMATION);

	if(planepnt != PlayerPlane)
	{
		planepnt->AADesignate = NULL;
	}

	if(planepnt->FlightStatus & PL_OUT_OF_CONTROL)
	{
		planepnt->AI.Behaviorfunc = AIDeathSpiral;
#if 0
		planepnt->DoControlSim = CalcF18ControlSurfaces;
		planepnt->DoCASSim     = CalcF18CASUpdates;
		planepnt->DoForces     = CalcAeroForces;
		planepnt->DoPhysics    = CalcAeroDynamics;
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		planepnt->dt = HIGH_FREQ;
		planepnt->UpdateRate = HIGH_AERO;
#endif
	}

	AIAltCheck(planepnt);
	AISimpleRadar(planepnt);

	if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		if(planepnt->AI.winglead == (PlayerPlane - Planes))
		{
			planepnt->AI.iAIFlags2 &= ~(AIWINGMANTARGET|AIIGNOREWPY|AI_STAY_ON_GROUND);
			if(planepnt->AI.AirTarget == NULL)
			{
				if(WingNumDTWSTargets)
				{
					AIRemoveTopWingDTWS();
				}
				else
				{
					planepnt->AI.iAIFlags2 &= ~(AIKEEPTARGET);
				}
			}
			else if(planepnt->AI.AirTarget->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				if(WingNumDTWSTargets)
				{
					AIRemoveTopWingDTWS();
				}
				else
				{
					planepnt->AI.iAIFlags2 &= ~(AIKEEPTARGET);
				}
			}
		}
		else
		{
			planepnt->AI.iAIFlags2 &= ~(AIWINGMANTARGET|AIKEEPTARGET|AIIGNOREWPY|AI_STAY_ON_GROUND);
		}
	}
	else
	{
		planepnt->AI.iAIFlags2 &= ~(AIWINGMANTARGET|AIKEEPTARGET|AIIGNOREWPY|AI_STAY_ON_GROUND);
	}
	planepnt->AI.iAIFlags2 &= ~(AI_IN_DOGFIGHT);


	if(0 <= planepnt->AI.lTimer1)
	{
		planepnt->AI.lTimer1 -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lTimer2)
	{
		planepnt->AI.lTimer2 -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lTimer3)
	{
		planepnt->AI.lTimer3 -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lCMTimer)
	{
		planepnt->AI.lCMTimer -= DeltaTicks;
	}
	else if(planepnt->AI.iMissileThreat < 0)
	{
		planepnt->AI.lCMTimer = 2000 + (planepnt->AI.iSkill * 2500);
	}

	if(0 <= planepnt->AI.lLockTimer)
	{
		planepnt->AI.lLockTimer -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lSpeechOverloadTimer)
	{
		planepnt->AI.lSpeechOverloadTimer -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lInterceptTimer)
	{
		planepnt->AI.lInterceptTimer -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lCombatTimer)
	{
		planepnt->AI.lCombatTimer -= DeltaTicks;
	}
	if(0 <= planepnt->AI.lEffectTimer)
	{
		planepnt->AI.lEffectTimer -= DeltaTicks;
	}
	if(0 <= lNoRepeatTimer[planepnt - Planes])
	{
		lNoRepeatTimer[planepnt - Planes] = lNoRepeatTimer[planepnt - Planes] - DeltaTicks;
		if(0 > lNoRepeatTimer[planepnt - Planes])
		{
			lNoRepeatFlags[planepnt - Planes] = 0;
		}
	}

	if(lNoticeTimer < 0)
	{
		planepnt->AI.iAIFlags1 &= ~AIWARNINGGIVEN;
	}

	if(planepnt != PlayerPlane)
	{
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			if(planepnt->AI.lHumanTimer < 0)
			{
				planepnt->AI.lHumanTimer += 60000;
				AICheckNearBombWaypt(planepnt);
			}
			else
			{
				planepnt->AI.lHumanTimer = planepnt->AI.lHumanTimer - DeltaTicks;
			}
		}
		else if(planepnt->AI.iAIFlags1 & (AI_ESCORTED_PLAYER|AI_FORMED_PLAYER))
		{
			if(planepnt->AI.lHumanTimer < 0)
			{
				planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
				planepnt->Status |= PlayerPlane->Status & (PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);

				planepnt->AI.lHumanTimer += 60000;
				humanescortOK = 1;

				if((planepnt->OnGround) || (PlayerPlane->OnGround))
				{
					humanescortOK = 0;
				}
				else
				{
					if((planepnt->AI.Behaviorfunc == CTWaitingForLaunch) || (planepnt->AI.Behaviorfunc == CTDoCarrierLaunch))
					{
						humanescortOK = 0;
					}
					else
					{
						if(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)
						{
							if((PlayerPlane->AI.lVar2 >= 100) || ((PlayerPlane->Status & PL_AI_DRIVEN) && ((PlayerPlane->AI.Behaviorfunc == CTDoCarrierLaunch) || (PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch))))
							{
								humanescortOK = 0;
							}
						}
					}
				}


				if((planepnt->AI.iAIFlags1 & AI_ESCORTED_PLAYER) && (planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1) && (humanescortOK))
				{
					if(!(planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER))
					{
						AILookPlayerEscort(planepnt);
					}
				}
				else if(planepnt->AI.iAIFlags1 & AI_FORMED_PLAYER)
				{
					AICheckAutoPlayerRelease(planepnt);
				}
			}
			else
			{
				planepnt->AI.lHumanTimer = planepnt->AI.lHumanTimer - DeltaTicks;
			}
		}
		else
		{
			if((planepnt->AI.AirThreat) || (planepnt->AI.pPaintedBy) || (planepnt->AI.iMissileThreat >= 0) || (iFenceIn))
			{
				planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
			}
			else
			{
				planepnt->Status |= (PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
			}
		}
	}

	if((WIsWeaponPlane(planepnt)) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		planepnt->AI.Behaviorfunc = WeaponAsPlane;
	}
	else if((planepnt->AI.lPlaneID == 30) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		planepnt->AI.Behaviorfunc = FlyCruiseMissile;
	}
	else if((planepnt->AI.iAICombatFlags2 & AI_DRONE) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		planepnt->AI.Behaviorfunc = FlyDrone;
		planepnt->AI.OrgBehave = NULL;
	}
	else if((planepnt->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		if(planepnt->AI.Behaviorfunc != AIMissileBreakToHeading)
		{
			planepnt->AI.Behaviorfunc = FlyDrone;
			planepnt->AI.OrgBehave = NULL;
		}
	}


	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(planepnt->AI.AirThreat != planepnt->AI.LastAirThreat)
		{
			NetPutGenericMessage2(planepnt, GM2_NEW_THREAT, (BYTE)((planepnt->AI.AirThreat) ? (planepnt->AI.AirThreat - Planes) : 255));
			planepnt->AI.LastAirThreat = planepnt->AI.AirThreat;
		}
		MAICheckFlags(planepnt);
	}

	planepnt->AI.iAICombatFlags2 &= ~(AI_KEEP_HIGH_RATE);
}

//**************************************************************************************
void AIAltCheck(PlaneParams *planepnt)
{
	int newmax = 0;
	double hsin, hcos;
	int cnt;
	float fcnt;
	double workx, worky, workz, worky2;
	double heading, checkdist, checkdistwu, degroll;
	double disttraveled, radpitch, cospitch, percsec;
	float secblock = 2.5;
	ANGLE workheading;

	//  Determine Ground Distance traveled
	radpitch = DegToRad((double)planepnt->Pitch / DEGREE);
	cospitch = cos(radpitch);
	percsec = DeltaTicks * 0.001;
	disttraveled = (cospitch * planepnt->V * percsec);
	for(cnt = 0; cnt < 4; cnt ++)
	{
		planepnt->AI.fDistToHeight[cnt] -= disttraveled;
	}

	//  Update countdown to next check (DeltaTicks changes with time acceleration)

	if(0 < planepnt->AI.lAltCheckDelay)
	{
		planepnt->AI.lAltCheckDelay -= DeltaTicks;
		return;
	}


	degroll = AIConvertAngleTo180Degree(planepnt->Roll);
	workheading = planepnt->Heading;
	if(degroll > 20)
	{
		workheading += AIConvert180DegreeToAngle(planepnt->YawRate);
	}
	else if(degroll < -20)
	{
		workheading -= AIConvert180DegreeToAngle(planepnt->YawRate);
	}
	heading = DegToRad((double)workheading / DEGREE);
	hsin = sin(heading);
	hcos = cos(heading);

	for(cnt = 0; cnt < 4; cnt ++)
	{
		if(planepnt->AI.fDistToHeight[cnt] < 0)
		{
//			planepnt->AI.iAIFlags1 &= ~AIDETAILALTCHECK;

			if(cnt > 0)
			{
				planepnt->AI.lMinAltFt[cnt - 1] = planepnt->AI.lMinAltFt[cnt];
				planepnt->AI.fDistToHeight[cnt - 1] = planepnt->AI.fDistToHeight[cnt];
			}
			if(cnt < 3)
			{
				planepnt->AI.lMinAltFt[cnt] = planepnt->AI.lMinAltFt[cnt + 1];
				planepnt->AI.fDistToHeight[cnt] = planepnt->AI.fDistToHeight[cnt + 1];
			}
			else
			{
				checkdist = (planepnt->V * secblock * 4);
				checkdistwu = checkdist * FTTOWU;
				workx = (planepnt->WorldPosition.X - (hsin * checkdistwu));
				workz = (planepnt->WorldPosition.Z - (hcos * checkdistwu));
				planepnt->AI.lMinAltFt[3] = LandHeight(workx, workz) * WUTOFT;

				planepnt->AI.fDistToHeight[3] = checkdist * 1.2;
				newmax = 2;
			}
		}
	}

	if(planepnt->AI.iAIFlags1 & AIDETAILALTCHECK)
	{
		for(cnt = 0; cnt < 4; cnt ++)
		{
			checkdist = (planepnt->V * ((cnt + 1) * secblock));
			checkdistwu = checkdist * FTTOWU;
			workx = (planepnt->WorldPosition.X - (hsin * checkdistwu));
			workz = (planepnt->WorldPosition.Z - (hcos * checkdistwu));
			worky = LandHeight(workx, workz) * WUTOFT;

			if(cnt == 0)
			{
				worky2 = LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z) * WUTOFT;
				if(worky2 < worky)
				{
					worky = worky2;
				}
			}

			if(worky > planepnt->AI.lMinAltFt[cnt])
			{
				newmax = 1;
				planepnt->AI.lMinAltFt[cnt] = worky;
				planepnt->AI.fDistToHeight[cnt] = checkdist * 1.2;
			}
		}

		newmax = 1;
	}
	else
	{
		checkdist = (planepnt->V * secblock * 4);
		checkdistwu = checkdist * FTTOWU;
		workx = (planepnt->WorldPosition.X - (hsin * checkdistwu));
		workz = (planepnt->WorldPosition.Z - (hcos * checkdistwu));
		worky = LandHeight(workx, workz) * WUTOFT;

		if(worky > planepnt->AI.lMinAltFt[3])
		{
			newmax = 1;
			planepnt->AI.lMinAltFt[3] = worky;
			planepnt->AI.fDistToHeight[3] = checkdist * 1.2;
		}
	}

	for(cnt = 1; cnt < 4; cnt ++)
	{
		if(planepnt->AI.lMinAltFt[cnt] > planepnt->AI.lMinAltFt[0])
			break;
	}

	if(cnt == 4)
	{
		for(fcnt = 0.5; fcnt < secblock; fcnt += 0.5)
		{
			checkdist = (planepnt->V * fcnt);
			checkdistwu = checkdist * FTTOWU;
			workx = (planepnt->WorldPosition.X - (hsin * checkdistwu));
			workz = (planepnt->WorldPosition.Z - (hcos * checkdistwu));
			worky = LandHeight(workx, workz) * WUTOFT;

			if(worky > planepnt->AI.lMinAltFt[0])
			{
				newmax = 1;
				planepnt->AI.lMinAltFt[0] = worky;
				checkdist *= 1.2;
				if(checkdist > planepnt->AI.fDistToHeight[0])
				{
					planepnt->AI.fDistToHeight[0] = checkdist * 1.2;
				}
			}
		}

		newmax = 1;
	}

	if(newmax == 1)
	{
		planepnt->AI.lAltCheckDelay = 250;
	}
	else if(newmax == 2)
	{
		planepnt->AI.lAltCheckDelay = 500;
	}
	else
	{
		planepnt->AI.lAltCheckDelay = 1000;
	}
}

//**************************************************************************************
double AICheckSafeAlt(PlaneParams *planepnt, double altval)
{
	double returnval;
	float distance = 0;

	if(planepnt == PlayerPlane)
	{
		returnval = (double)(AIGetMinSafeAlt(planepnt, &distance) + 1000) * FTTOWU;  //  was 20 feet buffer
	}
	else if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER))
	{
		returnval = (double)(AIGetMinSafeAlt(planepnt, &distance) + 200.0f + (planepnt->AI.iSkill * 100.0f)) * FTTOWU;  //  was 20 feet buffer
	}
	else
	{
		returnval = (double)(AIGetMinSafeAlt(planepnt, &distance) + 50) * FTTOWU;  //  was 20 feet buffer
	}

	if(altval > returnval)
	{
		return(altval);
	}

	return(returnval);
}

//**************************************************************************************
long AIGetMinSafeAlt(PlaneParams *planepnt, float *distance)
{
	int cnt;
	long returnval = 0;

	for(cnt = 0; cnt < 3; cnt ++)  //  was 4, but 4 changes enough that we get jerky motion if we check it.
	{
		if(planepnt->AI.lMinAltFt[cnt] > returnval)
		{
			returnval = planepnt->AI.lMinAltFt[cnt];
			if(distance != NULL)
			{
				*distance = planepnt->AI.fDistToHeight[cnt];
			}
		}
	}

	return(returnval);
}

//**************************************************************************************
ANGLE AIGetPlanesFlightPitch(PlaneParams *planepnt, int noerrorcheck)
{
	double aoadeg, cosroll, aoaoffset;
	ANGLE returnval;

	if(planepnt->Status & (AL_DEVICE_DRIVEN|AL_COMM_DRIVEN))
	{
		aoadeg = RadToDeg(planepnt->Alpha);
		cosroll = cos(DegToRad(fabs(AIConvertAngleTo180Degree(planepnt->Roll))));

		aoaoffset = cosroll * aoadeg;

		if((aoaoffset > 15) && (!noerrorcheck))
		{
			return(0);
		}

		returnval = AIConvert180DegreeToAngle(aoaoffset);

		returnval = planepnt->Pitch - returnval;

		return(returnval);
	}

	return(planepnt->Pitch);
}

//**************************************************************************************
void AISwitchLeader(PlaneParams *planepnt)
{
	int switchnum = -1;
	PlaneParams *switchplane;
	Behavior	tempbehavior;
	int iswingman = 0;


	if(planepnt->AI.iVar1 > 0)
	{
		AIMovePlaneToEndOfAttack(planepnt);
	}

	if(planepnt->AI.wingman >= 0)
	{
		switchnum = planepnt->AI.wingman;
		iswingman = 1;
	}
	else if (planepnt->AI.nextpair >= 0)
	{
		switchnum = planepnt->AI.nextpair;
	}
	else
	{
		return;
	}

	if(switchnum >= 0)
	{
		switchplane = &Planes[switchnum];
		tempbehavior = switchplane->AI;

		if(iswingman)
		{
			switchplane->AI.wingman = switchplane->AI.winglead;
			switchplane->AI.winglead = planepnt->AI.winglead;
			planepnt->AI.winglead = switchnum;
			planepnt->AI.wingman = tempbehavior.wingman;
			switchplane->AI.nextpair = planepnt->AI.nextpair;
			switchplane->AI.prevpair = planepnt->AI.prevpair;
			planepnt->AI.nextpair = tempbehavior.nextpair;
			planepnt->AI.prevpair = tempbehavior.prevpair;
		}
		else
		{
			switchplane->AI.nextpair = switchplane->AI.prevpair;
			switchplane->AI.prevpair = planepnt->AI.prevpair;
			planepnt->AI.prevpair = switchnum;
			planepnt->AI.nextpair = tempbehavior.nextpair;
		}
		switchplane->AI.FormationPosition = planepnt->AI.FormationPosition;
		planepnt->AI.FormationPosition = tempbehavior.FormationPosition;
	}
}

//**************************************************************************************
void AISetUpEgress(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	PlaneParams *leadplane;
	double heading;
	ANGLE tempangle;

	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutGenericMessage1(planepnt, GM_EGRESS);
	}

	if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) || (planepnt->AI.iAICombatFlags2 & AI_ESCORT_CAS))
	{
		if(!(planepnt->AI.iAICombatFlags2 & AI_FAC))
		{
			planepnt->AI.pGroundTarget = NULL;
		}

		if(planepnt->AI.iAIFlags2 & AI_RETURN_SEAD_COVER)
		{
			planepnt->AI.iAIFlags2 &= ~(AI_CAS_ATTACK|AI_SEAD_ATTACK|AI_RETURN_SEAD_COVER);
			if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
			{
				planepnt->AI.Behaviorfunc = AIFlyToEscortMeeting;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIFlyEscort;
			}
			return;
		}
		AIRestoreCASorSEAD(planepnt);
		return;
	}
	else if(planepnt->AI.iAICombatFlags1 & (AI_FORMON_SEAD))
	{
		planepnt->AI.iAICombatFlags1 &= ~AI_FORMON_SEAD;
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.Behaviorfunc = AIFormationFlying;
		planepnt->AI.lRadarDelay = 90000;
		return;
	}

	leadplane = AIGetLeader(planepnt);
	if((leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
	{
		AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay);
	}

	AINextWayPointNoActions(planepnt);

	if(leadplane == PlayerPlane)
	{
		planepnt->AI.iAIFlags1 &= ~(AIBOMBHOTMSG|AI_ASK_PLAYER_ATTACK);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	if(planepnt->AI.Behaviorfunc == AILoftBomb)
	{
		if(fabs(offangle) > 90)
		{
			planepnt->AI.Behaviorfunc = AIEgressToWayPt;
			tempangle = AIConvert180DegreeToAngle(offangle) + planepnt->Heading;
			heading = AIConvertAngleTo180Degree(tempangle);
			if(leadplane == PlayerPlane)
			{
//				AICEndGrndAttackMsg(planepnt, heading);
				AICAddSoundCall(AICEndGrndAttackMsg, planepnt - Planes, 5000 + ((rand() & 5) * 1000), 50, heading);
			}
		}
		else
		{
			if(offangle > 0)
			{
				planepnt->AI.DesiredHeading = planepnt->Heading + (120 * DEGREE);
			}
			else
			{
				planepnt->AI.DesiredHeading = planepnt->Heading - (120 * DEGREE);
			}
			planepnt->AI.Behaviorfunc = AIEgressToHeading;
			heading = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading);
			if(leadplane == PlayerPlane)
			{
//				AICEndGrndAttackMsg(planepnt, heading);
				AICAddSoundCall(AICEndGrndAttackMsg, planepnt - Planes, 5000 + ((rand() & 5) * 1000), 50, heading);
			}
		}
	}
	else
	{
		planepnt->AI.Behaviorfunc = AIEgressToWayPt;
		tempangle = AIConvert180DegreeToAngle(offangle) + planepnt->Heading;
		heading = AIConvertAngleTo180Degree(tempangle);
		if(leadplane == PlayerPlane)
		{
//			AICEndGrndAttackMsg(planepnt, heading);
			AICAddSoundCall(AICEndGrndAttackMsg, planepnt - Planes, 5000 + ((rand() & 5) * 1000), 50, heading);
		}
	}

	planepnt->AI.lTimer1 = 30000;
}

//**************************************************************************************
void AIEgressToWayPt(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	double rollval;
	PlaneParams *leadplane;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

//	planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 80.0;
	if(planepnt->AI.lDesiredSpeed > 600)
	{
		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);  // temp for now
	}
	else
	{
		MBAdjustAIThrust(planepnt, 700, 1);
	}

//	if((tdist < 6000.0) || (planepnt->AI.lTimer1 < 0))
	if((AIInNextRange(planepnt, tdist * WUTOFT,   34461, 60.0f)) || (planepnt->AI.lTimer1 < 0))
	{
		AICheckEscortRestore(planepnt);

		planepnt->AI.Behaviorfunc = AIFlyFormation;

		leadplane = AIGetLeader(planepnt);

		if(leadplane->Status & PL_AI_DRIVEN)
		{
			AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);
		}
	}

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 60)  //  was 60
	{
		rollval = (rollval < 0.0) ? -60 : 60;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
//	planepnt->RollPercentage = 0.75;	//  when max roll was 45.0
//	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	return;
}

//**************************************************************************************
void AIEgressToHeading(PlaneParams *planepnt)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	double rollval;
	PlaneParams *leadplane;


	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);

	if(planepnt->AI.lDesiredSpeed > 600)
	{
		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);  // temp for now
	}
	else
	{
		MBAdjustAIThrust(planepnt, 700, 1);
	}
//	planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 80.0;

	if(planepnt->AI.lTimer1 < 0)
	{
		AICheckEscortRestore(planepnt);

		planepnt->AI.Behaviorfunc = AIFlyFormation;

		leadplane = AIGetLeader(planepnt);

		if(leadplane->Status & PL_AI_DRIVEN)
		{
			AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);
		}
	}

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 60)  //  was 60
	{
		rollval = (rollval < 0.0) ? -60 : 60;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
//	planepnt->RollPercentage = 0.75;	//  when max roll was 45.0
//	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	return;
}

//**************************************************************************************
float AIConvertSpeedToFtPS(float altitude, long desiredspeed, int flags)
{
	double tspeed;
	float ClipAlt = altitude;
	double IAmod;
	int doindicated = 0;
	int domach = 0;
	float returnval = 0;

	if(flags & AIDESIREDSPDCALC)
	{
		doindicated = 1;
	}
	else if(flags & AIDESIREDSPDMACH)
	{
		domach = 1;
	}

	if(doindicated)
	{
		if (ClipAlt < 0.0) ClipAlt = 0.0;
		if (ClipAlt > 70000.0) ClipAlt = 70000.0;

		IAmod = sqrt(Pressure[(int)(ClipAlt/500.0)]/0.0023769);

		tspeed = (double)desiredspeed / ((FTSEC_TO_MLHR*MLHR_TO_KNOTS) * IAmod);
	}
	else
	{
		tspeed = (double)desiredspeed / (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}

	returnval = (float)tspeed;

	return(returnval);
}

//**************************************************************************************
long AIConvertFtPSToSpeed(float altitude, float desiredspeed, int flags)
{
	double tspeed;
	float ClipAlt = altitude;
	double IAmod;
	int doindicated = 0;
	int domach = 0;
	long returnval = 0;

	if(flags & AIDESIREDSPDCALC)
	{
		doindicated = 1;
	}
	else if(flags & AIDESIREDSPDMACH)
	{
		domach = 1;
	}

	if(doindicated)
	{
		if (ClipAlt < 0.0) ClipAlt = 0.0;
		if (ClipAlt > 70000.0) ClipAlt = 70000.0;

		IAmod = sqrt(Pressure[(int)(ClipAlt/500.0)]/0.0023769);

		tspeed = (double)desiredspeed * ((FTSEC_TO_MLHR*MLHR_TO_KNOTS) * IAmod);
	}
	else
	{
		tspeed = (double)desiredspeed * (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}

	returnval = (long)tspeed;

	return(returnval);
}

//**************************************************************************************
void AIBreakRight()
{
	AISetUpBreak(1);
}

//**************************************************************************************
void AIBreakLeft()
{
	AISetUpBreak(2);
}

//**************************************************************************************
void AIBreakHigh()
{
	AISetUpBreak(3);
}

//**************************************************************************************
void AIBreakLow()
{
	AISetUpBreak(4);
}

//**************************************************************************************
void AISetUpBreak(int breaktype)
{
	long delaycnt = 1000;
	int wingman;
	PlaneParams *planepnt;

	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}

	wingman = planepnt->AI.wingman;

	if(wingman >= 0)
	{
		if(!(((!(Planes[wingman].AI.iAIFlags2 & AILANDING)) || (Planes[wingman].AI.lTimer2 > 0)) && (!Planes[wingman].OnGround)))
		{
			return;
		}

		switch(breaktype)
		{
			case 1:
				if(Planes[wingman].AI.iVar1 > 0)
					AIMovePlaneToEndOfAttack(&Planes[wingman]);
				Planes[wingman].AI.DesiredHeading = Planes[wingman].Heading + 0x4000;
				if(Planes[wingman].AI.OrgBehave == NULL)
				{
					Planes[wingman].AI.OrgBehave = Planes[wingman].AI.Behaviorfunc;
					Planes[wingman].AI.Behaviorfunc = AIBreakToHeading;
				}
				else
				{
					Planes[wingman].AI.Behaviorfunc = AIBreakToHeading;
				}
				break;
			case 2:
				if(Planes[wingman].AI.iVar1 > 0)
					AIMovePlaneToEndOfAttack(&Planes[wingman]);
				Planes[wingman].AI.DesiredHeading = Planes[wingman].Heading - 0x4000;
				if(Planes[wingman].AI.OrgBehave == NULL)
				{
					Planes[wingman].AI.OrgBehave = Planes[wingman].AI.Behaviorfunc;
					Planes[wingman].AI.Behaviorfunc = AIBreakToHeading;
				}
				else
				{
					Planes[wingman].AI.Behaviorfunc = AIBreakToHeading;
				}
				break;
			case 3:
				if(Planes[wingman].AI.iVar1 > 0)
					AIMovePlaneToEndOfAttack(&Planes[wingman]);
				Planes[wingman].AI.DesiredPitch = 0x3555;
				if(Planes[wingman].AI.OrgBehave == NULL)
				{
					Planes[wingman].AI.OrgBehave = Planes[wingman].AI.Behaviorfunc;
					Planes[wingman].AI.Behaviorfunc = AIBreakToPitch;
				}
				else
				{
					Planes[wingman].AI.Behaviorfunc = AIBreakToPitch;
				}
				break;
			case 4:
				if(Planes[wingman].AI.iVar1 > 0)
					AIMovePlaneToEndOfAttack(&Planes[wingman]);
				Planes[wingman].AI.DesiredPitch = 0xCAAB;
				if(Planes[wingman].AI.OrgBehave == NULL)
				{
					Planes[wingman].AI.OrgBehave = Planes[wingman].AI.Behaviorfunc;
					Planes[wingman].AI.Behaviorfunc = AIBreakToPitch;
				}
				else
				{
					Planes[wingman].AI.Behaviorfunc = AIBreakToPitch;
				}
				break;
			default:
				return;
				break;
		}
		AICAddSoundCall(AICBasicAck, wingman, delaycnt, 40);
		Planes[wingman].AI.lTimer1 = 10000;
	}
}

//**************************************************************************************
void AIBreakToHeading(PlaneParams *planepnt)
{
	if(planepnt->AI.lTimer1 > 0)
	{
		planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), 0);
	}
	else
	{
		if(planepnt->AI.iAIFlags1 & AIENGAGED)
		{
			//  Need to change this to an offensive action once A2A stuff is done.  SRE
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			planepnt->AI.OrgBehave = NULL;
		}
		else
		{
			if(planepnt->AI.OrgBehave == NULL)
			{
				planepnt->AI.Behaviorfunc = AIFlyFormation;
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
				}
			}
			else
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
				}
				planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			}
			planepnt->AI.OrgBehave = NULL;
		}
	}
}

//**************************************************************************************
void AIMissileBreakToHeading(PlaneParams *planepnt)
{
	float impacttime;
	WeaponParams *weaponpnt = NULL;
	ANGLE workroll;
	double offangle;
	float tdist;
	float dx, dy, dz;
	float planeheading, desiredhead;

	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(0, planepnt);

	if((planepnt->AI.lTimer2 < 0) || (planepnt->AI.iMissileThreat < 0))
	{
		workroll = AIGetDesiredRollOffdyaw(planepnt, -AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), 0);

		if(planepnt->AI.iAIFlags2 & AIMISSILEDODGE)
		{
			planepnt->AI.iAIFlags2 &= ~(AIMISSILEDODGE);
		}
		else if(planepnt->AI.iAIFlags2 & AI_CONTINUE_ORG_BEHAVE)
		{
	  		planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			planepnt->AI.iAIFlags2 &= ~(AI_CONTINUE_ORG_BEHAVE);
			planepnt->AI.OrgBehave = NULL;
		}
		else
		{
//			planepnt->AI.Behaviorfunc = AIDoJink;
			planepnt->AI.lTimer1 = 300000 - (planepnt->AI.iSkill * 100000);
			AISwitchToAttack(planepnt, 1, 0);
		}
	}
	else
	{
		weaponpnt = &Weapons[planepnt->AI.iMissileThreat];
		if(weaponpnt->pTarget == NULL)
		{
			planepnt->AI.iMissileThreat = -1;
		}

		impacttime = weaponpnt->lTargetDistFt / (weaponpnt->InitialVelocity * (WUTOFT*50.0));
		planeheading = AIConvertAngleTo180Degree(planepnt->Heading);

		if((impacttime < 5.0f) && (planepnt->AI.iSkill < GP_VETERAN))  //  Better guys do this
		{
			offangle = AIComputeHeadingToPoint(planepnt, weaponpnt->Pos, &tdist, &dx ,&dy, &dz, 1);

			desiredhead = (offangle < 0) ? planeheading + (offangle + 90) : planeheading + (offangle - 90);

			workroll = AIGetDesiredRollOffdyaw(planepnt, -AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(desiredhead) - planepnt->Heading), 0);

			if(impacttime <= 2.0f)
			{
				planepnt->DesiredPitch += 0x2000;
			}
		}
		else
		{
			workroll = AIGetDesiredRollOffdyaw(planepnt, -AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), 0);
		}
	}

	if((workroll < 0x4000) || (workroll > 0xC000))
	{
		workroll *= 2;
		if((workroll > 0x4000) && (workroll < 0x8000))
		{
			workroll = 0x4000;
		}
		else if((workroll > 0x8000) && (workroll < 0xC000))
		{
			workroll = 0xC000;
		}
	}
	planepnt->DesiredRoll = workroll;
}

//**************************************************************************************
void AIBreakToPitch(PlaneParams *planepnt)
{
	if(planepnt->AI.DesiredPitch > 0x8000)
	{
		if(planepnt->AI.iAIFlags2 & AIGROUNDAVOID)
		{
			planepnt->AI.lTimer1 = -1;
		}
	}

	if(planepnt->AI.lTimer1 > 0)
	{
		planepnt->DesiredPitch = planepnt->AI.DesiredPitch;
	}
	else
	{
		if(planepnt->AI.iAIFlags1 & AIENGAGED)
		{
			//  Need to change this to an offensive action once A2A stuff is done.  SRE
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			planepnt->AI.OrgBehave = NULL;
		}
		else
		{
			if(planepnt->AI.OrgBehave == NULL)
			{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
				}
				planepnt->AI.Behaviorfunc = AIFlyFormation;
			}
			else
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
				}
				planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			}
			planepnt->AI.OrgBehave = NULL;
		}
	}
}

//**************************************************************************************
void AIWSanitizeRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
#ifdef SREDEBUGFILE
	if(SREFILEOpen)
	{
		SREFILEOpen = 0;
		_close(DebugFileSRE);
	}
#endif
	AISetUpSanitize(firstvalid, lastvalid, 1);
}

//**************************************************************************************
void AIWSanitizeLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AISetUpSanitize(firstvalid, lastvalid, 2);
}

//**************************************************************************************
void AIESanitizeRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AISetUpSanitize(firstvalid, lastvalid, 1);
}

//**************************************************************************************
void AIESanitizeLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AISetUpSanitize(firstvalid, lastvalid, 2);
}

//**************************************************************************************
void AIDSanitizeRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AISetUpSanitize(firstvalid, lastvalid, 1);
}

//**************************************************************************************
void AIDSanitizeLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AISetUpSanitize(firstvalid, lastvalid, 2);
}

//**************************************************************************************
void AISetUpSanitize(int firstvalid, int lastvalid, int sanitizetype)
{
	long delaycnt = 1000;
	PlaneParams *planepnt;

	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}

	if(planepnt->AI.wingman >= 0)
	{
		AISetUpSanitizePlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid, sanitizetype);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AISetUpSanitizePlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid, sanitizetype);
	}
	return;
}

//**************************************************************************************
void AISetUpSanitizePlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int sanitizetype)
{
	int placeingroup;
//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid))
	{
		switch(sanitizetype)
		{
			case 1:
				if(Planes[planenum].AI.iVar1 > 0)
					AIMovePlaneToEndOfAttack(&Planes[planenum]);
				Planes[planenum].AI.DesiredHeading = PlayerPlane->Heading + 0x4000;
				if(Planes[planenum].AI.OrgBehave == NULL)
				{
					Planes[planenum].AI.OrgBehave = Planes[planenum].AI.Behaviorfunc;
					Planes[planenum].AI.Behaviorfunc = AISanitizing;
				}
				else
				{
					Planes[planenum].AI.OrgBehave = AISanitizing;
				}
				break;
			case 2:
				if(Planes[planenum].AI.iVar1 > 0)
					AIMovePlaneToEndOfAttack(&Planes[planenum]);
				Planes[planenum].AI.DesiredHeading = PlayerPlane->Heading - 0x4000;
				if(Planes[planenum].AI.OrgBehave == NULL)
				{
					Planes[planenum].AI.OrgBehave = Planes[planenum].AI.Behaviorfunc;
					Planes[planenum].AI.Behaviorfunc = AISanitizing;
				}
				else
				{
					Planes[planenum].AI.OrgBehave = AISanitizing;
				}
				break;
			default:
				return;
				break;
		}
		AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
		*delaycnt = *delaycnt + 2000;
		Planes[planenum].AI.lTimer1 = 45000;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AISetUpSanitizePlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid, sanitizetype);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AISetUpSanitizePlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid, sanitizetype);
	}
	return;
}

//**************************************************************************************
void AISanitizing(PlaneParams *planepnt)
{
	float rollval;

	AIBreakToHeading(planepnt);

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;
}

//**************************************************************************************
void AIFlyTakeOff(PlaneParams *planepnt)
{
	PlaneParams *prevplane;

	if(planepnt->AI.winglead >= 0)
	{
		prevplane = &Planes[planepnt->AI.winglead];
	}
	else if(planepnt->AI.prevpair >= 0)
	{
		if(Planes[planepnt->AI.prevpair].AI.wingman >= 0)
		{
			prevplane = &Planes[Planes[planepnt->AI.prevpair].AI.wingman];
		}
		else
		{
			prevplane = &Planes[planepnt->AI.prevpair];
		}
	}
	else
	{
		prevplane = planepnt;
	}

	AICheckFBG(planepnt, prevplane);
}

//**************************************************************************************
void AISetUpCAPStart(PlaneParams *orgplanepnt)
{
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	PlaneParams *planepnt;
	PlaneParams *wingplane;
	int done = 0;
	int paircnt = 0;
	MBWayPoints *waypnt;
	int cnt;
	int placeingroup;

	planepnt = orgplanepnt;

	if(orgplanepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		placeingroup = (orgplanepnt->AI.iAIFlags1  & AIFLIGHTNUMS);
		if(placeingroup == 1)
		{
			AISetUpCAPPlayer(PlayerPlane, 1, 1);
		}
		else if(placeingroup & 1)
		{
			AISetUpCAPPlayer(PlayerPlane, placeingroup - 1, placeingroup);
		}
		else
		{
			AISetUpCAPPlayer(PlayerPlane, placeingroup, placeingroup + 1);
		}
		return;
	}

	planepnt->AI.iAIFlags1 |= AICANENGAGE;
	if((planepnt->AI.winglead != -1) || (planepnt->AI.prevpair != -1))
	{
		return;
	}

	if(planepnt->AI.WayPosition.X == -1)
	{
		waypnt = planepnt->AI.CurrWay;
		planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(waypnt->lWPy > 0)
		{
			planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
		}
		else
		{
			if (!DoingSimSetup)
				planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
			else
			{
				OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
				OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
				planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
			}
		}
	}

	heading = DegToRad((double)planepnt->AI.iVar1);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	while(!done)
	{
		planepnt->AI.Behaviorfunc = AISetUpCAP1;
		planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;

		planepnt->AI.iVar1 = orgplanepnt->AI.iVar1;
		planepnt->AI.lVar2 = orgplanepnt->AI.lVar2;
		planepnt->AI.lTimer1 = orgplanepnt->AI.lTimer1;

		if(planepnt->AI.wingman >= 0)
		{
			wingplane = &Planes[planepnt->AI.wingman];
			wingplane->AI.iAIFlags1 |= AINOFORMUPDATE;
			wingplane->AI.iAIFlags2 |= (planepnt->AI.iAIFlags2 & AI_SO_JAMMING);
			wingplane->AI.WayPosition = planepnt->AI.WayPosition;
			if(wingplane->AI.OrgBehave == NULL)   //   AIFlyTakeOff
			{
				wingplane->AI.Behaviorfunc = AISetUpCAP1;
			}
			else
			{
				wingplane->AI.OrgBehave = AISetUpCAP1;
			}
//			wingplane->AI.Behaviorfunc = AISetUpCAP1;

			wingplane->AI.iVar1 = orgplanepnt->AI.iVar1;
			wingplane->AI.lVar2 = orgplanepnt->AI.lVar2;
			wingplane->AI.lTimer1 = orgplanepnt->AI.lTimer1;
			for(cnt = 0; cnt < 15; cnt ++)
			{
				if(pDBWeaponList[wingplane->WeapLoad[cnt].WeapIndex].iWeaponType == 14)
				{
					wingplane->AI.iAIFlags2 |= (AI_SO_JAMMING|AI_SOJ_ON_BOARD);
					break;
				}
			}
		}
		if(planepnt->AI.nextpair >= 0)
		{
			planepnt = &Planes[planepnt->AI.nextpair];
			xoff = ((paircnt / 2) * 30) + 30;
			if(paircnt & 1)
			{
				xoff *= -1;
			}
			zoff = 0;
			planepnt->AI.WayPosition.X = orgplanepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
			planepnt->AI.WayPosition.Y = orgplanepnt->AI.WayPosition.Y;
			planepnt->AI.WayPosition.Z = orgplanepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));
			planepnt->AI.iAIFlags2 |= (orgplanepnt->AI.iAIFlags2 & AI_SO_JAMMING);
			for(cnt = 0; cnt < 15; cnt ++)
			{
				if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType == 14)
				{
					planepnt->AI.iAIFlags2 |= (AI_SO_JAMMING|AI_SOJ_ON_BOARD);
					break;
				}
			}
			paircnt ++;
		}
		else
		{
			done = 1;
		}
	}
	return;
}

//**************************************************************************************
void AISetUpCAPPlayer(PlaneParams *orgplanepnt, int firstvalid, int lastvalid)
{
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	PlaneParams *planepnt;
	PlaneParams *wingplane;
	int done = 0;
	int paircnt = 0;
	MBWayPoints *waypnt;
	int placeingroup;
	long delaycnt = 1000;
	int startact, endact, actcnt;
	CAPActionType *pCAPAction;
	int numwpts;

 	if(!(PlayerPlane->Status & AL_DEVICE_DRIVEN))
	{
		return;
	}

	planepnt = orgplanepnt;

	waypnt = planepnt->AI.CurrWay;

	startact = waypnt->iStartAct;
	endact = startact + waypnt->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_CAP))
		{
			pCAPAction = (CAPActionType *)AIActions[actcnt].pAction;
			planepnt->AI.iVar1 = AIConvert8ToHeading(pCAPAction->iHeading);
			planepnt->AI.lVar2 = pCAPAction->lRange;
			planepnt->AI.lTimer1 = pCAPAction->lTimer * 60000;
			break;
		}
	}

	if(actcnt >= endact)
	{
		return;
	}

	if(planepnt->AI.WayPosition.X == -1)
	{
		planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(waypnt->lWPy > 0)
		{
			planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
		}
		else
		{
			if (!DoingSimSetup)
				planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
			else
			{
				OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
				OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
				planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
			}
		}
	}

	heading = DegToRad((double)planepnt->AI.iVar1);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	while(!done)
	{
		placeingroup = (planepnt->AI.iAIFlags1  & AIFLIGHTNUMS);

		if(((placeingroup >= firstvalid) && (placeingroup <= lastvalid))
			|| ((planepnt == PlayerPlane) && (firstvalid <= 1)))
		{

			if(orgplanepnt == PlayerPlane)
			{
				numwpts = PlayerPlane->AI.numwaypts + (PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]);
				planepnt->AI.CurrWay = PlayerPlane->AI.CurrWay;
				planepnt->AI.numwaypts = numwpts - (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
			}

			if(planepnt->AI.OrgBehave == NULL)
			{
				planepnt->AI.Behaviorfunc = AISetUpCAP1;
			}
			else
			{
				planepnt->AI.OrgBehave = AISetUpCAP1;
			}

			planepnt->AI.iVar1 = orgplanepnt->AI.iVar1;
			planepnt->AI.lVar2 = orgplanepnt->AI.lVar2;
			planepnt->AI.lTimer1 = orgplanepnt->AI.lTimer1;
			if(placeingroup > 1)
			{
				AICAddSoundCall(AICBasicAck, (int)(planepnt - Planes), delaycnt, 50);
				delaycnt = delaycnt + 2000;
				planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
			}
			if(planepnt->AI.wingman >= 0)
			{
				wingplane = &Planes[planepnt->AI.wingman];

				wingplane->AI.CurrWay = planepnt->AI.CurrWay;
				wingplane->AI.numwaypts = planepnt->AI.numwaypts;

				wingplane->AI.iVar1 = orgplanepnt->AI.iVar1;
				wingplane->AI.lVar2 = orgplanepnt->AI.lVar2;
				wingplane->AI.lTimer1 = orgplanepnt->AI.lTimer1;
				wingplane->AI.iAIFlags1 |= AINOFORMUPDATE;

				wingplane->AI.WayPosition = planepnt->AI.WayPosition;
				xoff = -1.5 * NMTOFT;
				zoff = 8 * NMTOFT;

				wingplane->AI.WayPosition = planepnt->AI.WayPosition;
				wingplane->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
				wingplane->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
				wingplane->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));
				wingplane->AI.lDesiredSpeed = 400;

				if(planepnt != PlayerPlane)
				{
					if(wingplane->AI.OrgBehave == NULL)
					{
						wingplane->AI.Behaviorfunc = AISetUpCAP1;
					}
					else
					{
						wingplane->AI.OrgBehave = AISetUpCAP1;
					}

					AICAddSoundCall(AICBasicAck, (int)(wingplane - Planes), delaycnt, 50);
					delaycnt = delaycnt + 2000;
				}
				else
				{
					if(wingplane->AI.OrgBehave == NULL)
					{
						wingplane->AI.Behaviorfunc = AIFlyCAPSide3;
					}
					else
					{
						wingplane->AI.OrgBehave = AIFlyCAPSide3;
					}

					AICAddSoundCall(AICGoingColdMsgPN, (int)(wingplane - Planes), delaycnt, 50);
					delaycnt = delaycnt + 5000;
				}
			}
		}
		if(planepnt->AI.nextpair >= 0)
		{
			planepnt = &Planes[planepnt->AI.nextpair];
			xoff = (((paircnt / 2) * 30) + 30) * NMTOFT;
			if(paircnt & 1)
			{
				xoff *= -1;
			}
			zoff = 0;
			planepnt->AI.WayPosition.X = orgplanepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
			planepnt->AI.WayPosition.Y = orgplanepnt->AI.WayPosition.Y;
			planepnt->AI.WayPosition.Z = orgplanepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));
			paircnt ++;
		}
		else
		{
			done = 1;
		}
	}
	return;
}

//**************************************************************************************
void AISetUpCAPStartSinglePair(PlaneParams *orgplanepnt)
{
	double angxx, angxz, angzx, angzz;
	double heading;
	PlaneParams *planepnt;
	PlaneParams *wingplane;
	int done = 0;
	int paircnt = 0;
	MBWayPoints *waypnt;

	planepnt = orgplanepnt;

	planepnt->AI.iAIFlags1 |= AICANENGAGE;
	if(planepnt->AI.winglead != -1)
	{
		planepnt->AI.Behaviorfunc = Planes[planepnt->AI.winglead].AI.Behaviorfunc;
		planepnt->AI.OrgBehave = Planes[planepnt->AI.winglead].AI.OrgBehave;
		planepnt->AI.lTimer1 = Planes[planepnt->AI.winglead].AI.lTimer1;
		return;
	}

	planepnt->AI.lDesiredSpeed = 500;

	if(planepnt->AI.WayPosition.X == -1)
	{
		waypnt = planepnt->AI.CurrWay;
		planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(waypnt->lWPy > 0)
		{
			planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
		}
		else
		{
			if (!DoingSimSetup)
				planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
			else
			{
				OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
				OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
				planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
			}
		}
	}

	heading = DegToRad((double)planepnt->AI.iVar1);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	planepnt->AI.Behaviorfunc = AISetUpCAP1;
	if(planepnt->AI.wingman >= 0)
	{
		wingplane = &Planes[planepnt->AI.wingman];
		wingplane->AI.WayPosition = planepnt->AI.WayPosition;
		wingplane->AI.Behaviorfunc = AISetUpCAP1;
	}
	return;
}

//**************************************************************************************
void AISetUpCAP1(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;
	PlaneParams *wingplane;

	if(planepnt->AI.winglead >= 0)
	{
		AIFormationFlyingLead(planepnt, &Planes[planepnt->AI.winglead]);
		return;
	}

	if(!(planepnt->AI.iAIFlags1 & AIPLAYERGROUP))
	{
		if(!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA))
		{
			PlaneHasAAWeapons(planepnt);
		}

		if((planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA) && (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			if(AIAllGroupAAWinchester(AIGetLeader(planepnt)))
			{
				planepnt->AI.Behaviorfunc = AIFlyFormation;
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
				}
				AINextWayPoint(planepnt);
//				if((planepnt->AI.lAIVoice >= SPCH_CAP1) && (planepnt->AI.lAIVoice <= SPCH_CAP3))
				if(AIRIsAttack(planepnt - Planes))
				{
					AIC_CAP_WinchesterMsg(planepnt - Planes, -1);
				}
				return;
			}
		}
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
	{
		float maxroll, turnradius;

		maxroll = 25.0f;
		turnradius = AIGetTurnRadFt(planepnt) * 2;

		if((maxroll < 90.0f) && (maxroll > 10.0f))
		{
			turnradius /= sin(DegToRad(maxroll));
		}

		turnradius *= FTTONM;
		if(turnradius > 1.5)
		{
			xoff = turnradius * NMTOFT;
			zoff = 0;
		}
		else
		{
			xoff = 1.5 * NMTOFT;
			zoff = 0;
		}

	}
	else
	{
		xoff = 1.5 * NMTOFT;
		zoff = 0;
	}

	heading = DegToRad((double)planepnt->AI.iVar1);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;


	pointpos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
	pointpos.Y = planepnt->AI.WayPosition.Y;
	pointpos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

	tdist = FlyToPoint(planepnt, pointpos);

//	if(tdist < 1000.0)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		xoff = 1.5 * NMTOFT;
		zoff = -8 * NMTOFT;

		if((!AIInPlayerGroup(planepnt)) && (AIGetLeader(planepnt) == planepnt))
		{
			AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay);
		}
		AIC_On_Station_Msg(planepnt - Planes);

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		planepnt->AI.Behaviorfunc = AIFlyCAPSide1;
		planepnt->AI.lDesiredSpeed = 300;

		if(planepnt->AI.wingman >= 0)
		{
			if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
			{
				float maxroll, turnradius;

				maxroll = 25.0f;
				turnradius = AIGetTurnRadFt(planepnt) * 2;

				if((maxroll < 90.0f) && (maxroll > 10.0f))
				{
					turnradius /= sin(DegToRad(maxroll));
				}

				turnradius *= FTTONM;
				if(turnradius > 1.5)
				{
					xoff = -turnradius * NMTOFT;
					zoff = 16 * NMTOFT;
				}
				else
				{
					xoff = -1.5 * NMTOFT;
					zoff = 8 * NMTOFT;
				}

			}
			else
			{
				xoff = -1.5 * NMTOFT;
				zoff = 8 * NMTOFT;
			}

			wingplane = &Planes[planepnt->AI.wingman];

			wingplane->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
			wingplane->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
			wingplane->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));
			wingplane->AI.lDesiredSpeed = 400;

//			wingplane->AI.Behaviorfunc = AIFlyCAPSide3;
			if(wingplane->AI.OrgBehave == NULL)   //   AIFlyTakeOff
			{
				wingplane->AI.Behaviorfunc = AIFlyCAPSide3;
			}
			else
			{
				wingplane->AI.OrgBehave = AIFlyCAPSide3;
			}
		}
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
	return;
}

//**************************************************************************************
void AISetUpCAP2(PlaneParams *planepnt)
{
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
	return;
}

//**************************************************************************************
void AIFlyCAPSide1(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;
	PlaneParams *wingplane;

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos);

//	if(tdist < 1000.0)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			int headang;
			long range;

			headang = 8 - (planepnt->AI.iVar1 / 45);
			if((headang < 0) || (headang > 7))
			{
				headang = 0;
			}
			range = (planepnt->AI.lVar2 < 256) ? planepnt->AI.lVar2 : 255;
			NetPutGenericMessage3(planepnt, GM3_CAP_2, (BYTE) headang, (BYTE)range);
		}

		heading = DegToRad((double)planepnt->AI.iVar1);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;


		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			float maxroll, turnradius;

			maxroll = 25.0f;
			turnradius = AIGetTurnRadFt(planepnt);

			if((maxroll < 90.0f) && (maxroll > 10.0f))
			{
				turnradius /= sin(DegToRad(maxroll));
			}

			turnradius *= FTTONM;
			if(turnradius > 1.5)
			{
				xoff = -turnradius * NMTOFT;
				zoff = -12 * NMTOFT;
			}
			else
			{
				xoff = -1.5 * NMTOFT;
				zoff = -6 * NMTOFT;
			}

		}
		else
		{
			xoff = -1.5 * NMTOFT;
			zoff = -6 * NMTOFT;
		}

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		planepnt->AI.Behaviorfunc = AIFlyCAPSide2;
		planepnt->AI.lDesiredSpeed = 400;

		wingplane = NULL;
		if(planepnt->AI.wingman >= 0)
		{
			wingplane = &Planes[planepnt->AI.wingman];
		}
		else if(planepnt->AI.winglead >= 0)
		{
			wingplane = &Planes[planepnt->AI.winglead];
		}

		if(wingplane != NULL)
		{
			//  Going COLD
			if(wingplane->Status & PL_DEVICE_DRIVEN)
			{
				AICGoingColdMsg(planepnt);
			}

			if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
			{
				float maxroll, turnradius;

				maxroll = 25.0f;
				turnradius = AIGetTurnRadFt(planepnt);

				if((maxroll < 90.0f) && (maxroll > 10.0f))
				{
					turnradius /= sin(DegToRad(maxroll));
				}

				turnradius *= FTTONM;
				if(turnradius > 1.5)
				{
					xoff = turnradius * NMTOFT;
					zoff = -16 * NMTOFT;
				}
				else
				{
					xoff = 1.5 * NMTOFT;
					zoff = -8 * NMTOFT;
				}

			}
			else
			{
				xoff = 1.5 * NMTOFT;
				zoff = -8 * NMTOFT;
			}

			wingplane->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
			wingplane->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
			wingplane->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));
			wingplane->AI.lDesiredSpeed = 300;

			wingplane->AI.Behaviorfunc = AIFlyCAPSide1;
		}
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);
	return;
}

//**************************************************************************************
void AIFlyCAPSide2(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos);

//	if(tdist < 1000.0)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			int headang;
			long range;

			headang = 8 - (planepnt->AI.iVar1 / 45);
			if((headang < 0) || (headang > 7))
			{
				headang = 0;
			}
			range = (planepnt->AI.lVar2 < 256) ? planepnt->AI.lVar2 : 255;
			NetPutGenericMessage3(planepnt, GM3_CAP_3, (BYTE) headang, (BYTE)range);
		}

		heading = DegToRad((double)planepnt->AI.iVar1);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;


		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			float maxroll, turnradius;

			maxroll = 25.0f;
			turnradius = AIGetTurnRadFt(planepnt);

			if((maxroll < 90.0f) && (maxroll > 10.0f))
			{
				turnradius /= sin(DegToRad(maxroll));
			}

			turnradius *= FTTONM;
			if(turnradius > 1.5)
			{
				xoff = -turnradius * NMTOFT;
				zoff = 16 * NMTOFT;
			}
			else
			{
				xoff = -1.5 * NMTOFT;
				zoff = 8 * NMTOFT;
			}

		}
		else
		{
			xoff = -1.5 * NMTOFT;
			zoff = 8 * NMTOFT;
		}

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		planepnt->AI.Behaviorfunc = AIFlyCAPSide3;
		planepnt->AI.lDesiredSpeed = 400;
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);

	return;
}

//**************************************************************************************
void AIFlyCAPSide3(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos);

//	if(tdist < 1000.0)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			int headang;
			long range;

			headang = 8 - (planepnt->AI.iVar1 / 45);
			if((headang < 0) || (headang > 7))
			{
				headang = 0;
			}
			range = (planepnt->AI.lVar2 < 256) ? planepnt->AI.lVar2 : 255;
			NetPutGenericMessage3(planepnt, GM3_CAP_4, (BYTE) headang, (BYTE)range);
		}

		heading = DegToRad((double)planepnt->AI.iVar1);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;


		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			float maxroll, turnradius;

			maxroll = 25.0f;
			turnradius = AIGetTurnRadFt(planepnt);

			if((maxroll < 90.0f) && (maxroll > 10.0f))
			{
				turnradius /= sin(DegToRad(maxroll));
			}

			turnradius *= FTTONM;
			if(turnradius > 1.5)
			{
				xoff = turnradius * NMTOFT;
				zoff = 12 * NMTOFT;
			}
			else
			{
				xoff = 1.5 * NMTOFT;
				zoff = 6 * NMTOFT;
			}

		}
		else
		{
			xoff = 1.5 * NMTOFT;
			zoff = 6 * NMTOFT;
		}

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		planepnt->AI.Behaviorfunc = AIFlyCAPSide4;
		planepnt->AI.lDesiredSpeed = 400;
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);

	return;
}

//**************************************************************************************
void AIFlyCAPSide4(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;
	PlaneParams *wingplane;

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos);

//	if(tdist < 1000.0)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			int headang;
			long range;

			headang = 8 - (planepnt->AI.iVar1 / 45);
			if((headang < 0) || (headang > 7))
			{
				headang = 0;
			}
			range = (planepnt->AI.lVar2 < 256) ? planepnt->AI.lVar2 : 255;
			NetPutGenericMessage3(planepnt, GM3_CAP_1, (BYTE) headang, (BYTE)range);
		}

		heading = DegToRad((double)planepnt->AI.iVar1);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;


		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			float maxroll, turnradius;

			maxroll = 25.0f;
			turnradius = AIGetTurnRadFt(planepnt);

			if((maxroll < 90.0f) && (maxroll > 10.0f))
			{
				turnradius /= sin(DegToRad(maxroll));
			}

			turnradius *= FTTONM;
			if(turnradius > 1.5)
			{
				xoff = turnradius * NMTOFT;
				zoff = -16 * NMTOFT;
			}
			else
			{
				xoff = 1.5 * NMTOFT;
				zoff = -8 * NMTOFT;
			}

		}
		else
		{
			xoff = 1.5 * NMTOFT;
			zoff = -8 * NMTOFT;
		}

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		planepnt->AI.Behaviorfunc = AIFlyCAPSide1;
		planepnt->AI.lDesiredSpeed = 400;

		wingplane = NULL;
		if(planepnt->AI.wingman >= 0)
		{
			wingplane = &Planes[planepnt->AI.wingman];
		}
		else if(planepnt->AI.winglead >= 0)
		{
			wingplane = &Planes[planepnt->AI.winglead];
		}
		if(wingplane != NULL)
		{
			// Going HOT
			if(wingplane->Status & PL_DEVICE_DRIVEN)
			{
				AICGoingHotMsg(planepnt);
			}

			if(wingplane->AI.Behaviorfunc == planepnt->AI.Behaviorfunc)
			{
				wingplane->AI.lDesiredSpeed = 450;
			}
		}
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);

	return;
}

//**************************************************************************************
void AIStandardCAPStuff(PlaneParams *planepnt)
{
	float dx, dy, dz;
	PlaneParams *targplane;
	PlaneParams *wingplane;
	PlaneParams *targlead;
	float interceptdist;
	float tdist;
	int tempfence;
	int side;
	int doret = 0;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER)
	{
		targplane = planepnt->AI.AirTarget;
		if(targplane == NULL)
		{
			planepnt->AI.iAIFlags1 &= ~AIWARNINGGIVEN;
		}
		else if(iAI_ROE[planepnt->AI.iSide] > 1)
		{
		//	dx = targplane->WorldPosition.X - planepnt->WorldPosition.X;
		//	dy = targplane->WorldPosition.Y - planepnt->WorldPosition.Y;
		//	dz = targplane->WorldPosition.Z - planepnt->WorldPosition.Z;
			if(iAI_ROE[planepnt->AI.iSide] < 2)
			{
				tempfence = 0;

				side = planepnt->AI.iSide;

				if(targplane->AI.iSide == side)
				{
					doret = 1;
				}
				else if(((targplane->AI.iSide == AI_ENEMY) || (targplane->AI.iSide == AI_NEUTRAL)) && ((side == AI_ENEMY) || (side == AI_NEUTRAL)))
				{
					doret = 1;
				}
				else if(iAI_ROE[side] < 2)
				{
					tempfence = AICCheckAllFences((targplane->WorldPosition.X * WUTOFT), (targplane->WorldPosition.Z * WUTOFT));

					if((!tempfence) && ((targplane->AI.iSide == AI_FRIENDLY) || (targplane->AI.iSide == AI_NEUTRAL)) && (side == AI_ENEMY))
					{
						doret = 1;
					}
					else if((tempfence) && (targplane->AI.iSide == AI_ENEMY))
					{
						doret = 1;
					}
				}

				if(doret)
				{
					if(0 > targplane->AI.lInterceptTimer)
					{
						targplane->AI.lInterceptTimer = 60000;
					}
					return;
				}
			}

			//  Distance from center of CAP area.
			dx = targplane->WorldPosition.X - planepnt->AI.WayPosition.X;
			dy = targplane->WorldPosition.Y - planepnt->AI.WayPosition.Y;
			dz = targplane->WorldPosition.Z - planepnt->AI.WayPosition.Z;

			tdist = QuickDistance(dx,dz);
			interceptdist = (planepnt->AI.lVar2 * NMTOWU);

			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				if(((tdist < interceptdist) && (0 > targplane->AI.lInterceptTimer)) || (tdist < (interceptdist / 2)))
				{
					wingplane = NULL;
					if(planepnt->AI.wingman >= 0)
					{
						wingplane = &Planes[planepnt->AI.wingman];
					}
					else if(planepnt->AI.winglead >= 0)
					{
						wingplane = &Planes[planepnt->AI.winglead];
					}

					if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
					{
						if((planepnt->AI.winglead < 0) || (wingplane == PlayerPlane))
						{
							AISetUpPlayerCAPIntercept(planepnt, targplane);
							if(MultiPlayer)
							{
								NetPutGenericMessage3(planepnt, GM3_CAP_ATTACK, (BYTE)1, (BYTE)((targplane) ? targplane - Planes : 255));
							}
						}
						else
						{
							AISetUpPlayerCAPIntercept(wingplane, targplane);
							if(MultiPlayer)
							{
								NetPutGenericMessage3(wingplane, GM3_CAP_ATTACK, (BYTE)2, (BYTE)((targplane) ? targplane - Planes : 255));
							}
						}
					}
					else if(wingplane != NULL)
					{
	//					planepnt->AI.Behaviorfunc = wingplane->AI.Behaviorfunc = AIHeadAtAirTarget;
						AISwitchToAttack(planepnt, 0, 0);

						planepnt->AI.OrgBehave = wingplane->AI.OrgBehave = AISetUpCAPStartSinglePair;
						if((wingplane->AI.AirTarget != planepnt->AI.AirTarget) && (!(wingplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
						{
							GeneralClearCombatBehavior(wingplane);
						}

						if(MultiPlayer)
						{
							NetPutGenericMessage3(planepnt, GM3_CAP_ATTACK, (BYTE)3, (BYTE)((wingplane->AI.AirTarget) ? wingplane->AI.AirTarget - Planes : 255));
						}
						GeneralSetNewAirTarget(planepnt, wingplane->AI.AirTarget);
						wingplane->AI.lTimer1 = 360000;
					}
					else
					{
	//					planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
						AISwitchToAttack(planepnt, 0, 0);

						planepnt->AI.OrgBehave = AISetUpCAPStartSinglePair;
						if(MultiPlayer)
						{
							NetPutGenericMessage3(planepnt, GM3_CAP_ATTACK, (BYTE)4, (BYTE)((planepnt->AI.AirTarget) ? planepnt->AI.AirTarget - Planes : 255));
						}
					}
					planepnt->AI.lTimer1 = 360000;
				}
	//			else if((tdist < (interceptdist * 1.5)) && (!(planepnt->AI.iAIFlags1 & AIWARNINGGIVEN)))
				else if((tdist < (interceptdist * 1.5)))
				{
	//				planepnt->AI.iAIFlags1 |= AIWARNINGGIVEN;
					targlead = AIGetLeader(targplane);
					wingplane = NULL;
					if(planepnt->AI.wingman >= 0)
					{
						wingplane = &Planes[planepnt->AI.wingman];
	//					wingplane->AI.iAIFlags1 |= AIWARNINGGIVEN;
					}
					else if(planepnt->AI.winglead >= 0)
					{
						wingplane = &Planes[planepnt->AI.winglead];
	//					wingplane->AI.iAIFlags1 |= AIWARNINGGIVEN;
					}
	//				if(targlead->Status & PL_DEVICE_DRIVEN)
	//				{
	//					AICTurnBackMsg(planepnt);
	//				}

				}
			}
		}
	}

	AICheckCAPCondition(planepnt);
}

//**************************************************************************************
void AICheckCAPCondition(PlaneParams *planepnt)
{
	CAPActionType *pCAPAction;
	int endcap = 0;
	int actcnt, startact, endact;
	int capact = -1;
	MBWayPoints *waypnt;

	waypnt = planepnt->AI.CurrWay;
	startact = waypnt->iStartAct;
	endact = startact + waypnt->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if(AIActions[actcnt].ActionID == ACTION_CAP)
		{
			capact = actcnt;
			break;
		}
		else if(AIActions[actcnt].ActionID == ACTION_AWACS_PATTERN)
		{
			AICheckAWACSCondition(planepnt, actcnt);
			return;
		}
		else if(AIActions[actcnt].ActionID == ACTION_SOJ)
		{
			AICheckSOJCondition(planepnt, actcnt);
			return;
		}
		else if(AIActions[actcnt].ActionID == ACTION_ORBIT)
		{
			AICheckOrbitCondition(planepnt, 1);
			return;
		}
	}

	if(capact == -1)
		return;

	pCAPAction = (CAPActionType *)AIActions[capact].pAction;

	if(pCAPAction->lFlag == 0)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				endcap = 1;
			}
		}
	}
	else
	{
		endcap = AICheckEventFlagStatus(pCAPAction->lFlag);
	}

	if(endcap)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage1(planepnt, GM_CAP_END);
		}

		AICCAPDone(planepnt - Planes);
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		AINextWayPoint(planepnt);
	}
}

//**************************************************************************************
void AICheckAWACSCondition(PlaneParams *planepnt, int awacsact)
{
	AWACSPatternActionType *pAWACSAction;
	int endawacs = 0;

	if(awacsact == -1)
		return;

	pAWACSAction = (AWACSPatternActionType *)AIActions[awacsact].pAction;

	if(pAWACSAction->lFlag == 0)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			endawacs = 1;
		}
	}
	else
	{
		endawacs = AICheckEventFlagStatus(pAWACSAction->lFlag);
	}

	if(endawacs)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		AINextWayPoint(planepnt);
	}
}

//**************************************************************************************
void AICheckSOJCondition(PlaneParams *planepnt, int sojact)
{
	SOJActionType *pSOJAction;
	int endsoj = 0;

	if(sojact == -1)
		return;

	pSOJAction = (SOJActionType *)AIActions[sojact].pAction;

	if(pSOJAction->lFlag == 0)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			endsoj = 1;
		}
	}
	else
	{
		endsoj = AICheckEventFlagStatus(pSOJAction->lFlag);
	}

	if(endsoj)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		planepnt->AI.iAIFlags2 &= ~(AI_SO_JAMMING|AI_SOJ_ON_BOARD);
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		AINextWayPoint(planepnt);
	}
}

//**************************************************************************************
float FlyToPoint(PlaneParams *planepnt, FPoint pointpos)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
	double rollval;


	offangle = AIComputeHeadingToPoint(planepnt, pointpos, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);

	if(!(planepnt->AI.iAIFlags2 & AI_IN_DOGFIGHT))
	{
		if(fabs(rollval) > 25)  //  was 60
		{
			rollval = (rollval < 0.0) ? -25 : 25;
			planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
		}

		planepnt->RollPercentage = 0.40;	//  max roll 90.0
	}

	return(tdist);
}

//**************************************************************************************
float AIComputeHeadingToPoint(PlaneParams *planepnt, FPoint pointpos, float *rtdist, float *rdx, float *rdy, float *rdz, int isrelative)
{
	double dx, dy, dz;
	double offangle;
	float tdist;

	dx = pointpos.X - planepnt->WorldPosition.X;
	if(pointpos.Y >= 0)
	{
		dy = AICheckSafeAlt(planepnt, pointpos.Y) - planepnt->WorldPosition.Y;
	}
	else
	{
		dy = (AICheckSafeAlt(planepnt, 500) + ConvertWayLoc(labs(pointpos.Y)) + (7.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
	}
	dz = pointpos.Z - planepnt->WorldPosition.Z;

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	if(isrelative)
	{
		offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

		offangle = AICapAngle(offangle);
	}

	tdist = QuickDistance(dx,dz);
	*rdx = dx;
	*rdy = dy;
	*rdz = dz;
	*rtdist = tdist;
	return((float) offangle);
}

//**************************************************************************************
int AIConvert8ToHeading(int headingval)
{
	int returnval;
	switch(headingval)
	{
		case 8:
			return(0);
			break;
		case 7:
			return(45);
			break;
		case 6:
			return(90);
			break;
		case 5:
			return(135);
			break;
		case 4:
			return(180);
			break;
		case 3:
			return(225);
			break;
		case 2:
			return(270);
			break;
		case 1:
			return(315);
			break;
		case 0:
			return(0);
			break;
		default:
			returnval = 360 - headingval;
			if(returnval = 360)
			{
				returnval = 0;
			}
			return(returnval);
			break;
	}
}

//**************************************************************************************
void AIUpdateBehaviorFormation(PlaneParams *planepnt)
{
	double angxx, angxz, angzx, angzz;
	double heading;
	int placeingroup;

//	heading = DegToRad(AIConvertAngleTo180Degree(planepnt->Heading));
	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	if(planepnt->AI.wingman >= 0)
	{
		AIUpdateBehaviorFormationLoc(&Planes[planepnt->AI.wingman], planepnt, planepnt->WorldPosition, angxx, angxz, angzx, angzz);
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);

	if((planepnt->AI.nextpair >= 0) && (placeingroup > 3))
	{
		AIUpdateBehaviorFormationLoc(&Planes[planepnt->AI.nextpair], planepnt, planepnt->WorldPosition, angxx, angxz, angzx, angzz);
	}
}

//**************************************************************************************
void AIUpdateBehaviorFormationLoc(PlaneParams *planepnt, PlaneParams *leadplane, FPointDouble &leadposition, double angxx, double angxz, double angzx, double angzz)
{
	int placeingroup;

	if(planepnt->AI.iAIFlags1 & AIINFORMATION)
	{
		planepnt->AI.WorldFormationPos.X = leadposition.X + ConvertWayLoc((angxx * planepnt->AI.FormationPosition.X) + (angzx * planepnt->AI.FormationPosition.Z)) - ConvertWayLoc((angxx * leadplane->AI.FormationPosition.X) + (angzx * leadplane->AI.FormationPosition.Z));
		planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y) - ConvertWayLoc(leadplane->AI.FormationPosition.Y);
	//	planepnt->AI.WayPosition.Z = leadposition.Z + ConvertWayLoc((angxz * planepnt->AI.FormationPosition.X) + (angzz * planepnt->AI.FormationPosition.Z));
		planepnt->AI.WorldFormationPos.Z = leadposition.Z + ConvertWayLoc(-(angxz * planepnt->AI.FormationPosition.X) + (angzz * planepnt->AI.FormationPosition.Z)) - ConvertWayLoc(-(angxz * leadplane->AI.FormationPosition.X) + (angzz * leadplane->AI.FormationPosition.Z));
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIUpdateBehaviorFormationLoc(&Planes[planepnt->AI.wingman], leadplane, leadposition, angxx, angxz, angzx, angzz);
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);

	if((planepnt->AI.nextpair >= 0) && (placeingroup > 3))
	{
		AIUpdateBehaviorFormationLoc(&Planes[planepnt->AI.nextpair], leadplane, leadposition, angxx, angxz, angzx, angzz);
	}
}

//**************************************************************************************
void AIGetOrbitOffset(PlaneParams *planepnt, FPoint *orbitpoint, int orbittype)
{
	double angxx, angxz, angzx, angzz;
	double heading;
	float ftoffset;

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	ftoffset = planepnt->AI.lVar2;
	//  This will give a point 90 off of the plane's heading.

	if(orbittype == 2)
	{
		orbitpoint->X = planepnt->AI.TargetPos.X - ConvertWayLoc((angzz * ftoffset));
		orbitpoint->Y = planepnt->AI.TargetPos.Y;
		orbitpoint->Z = planepnt->AI.TargetPos.Z - ConvertWayLoc(-(angzx * ftoffset));
	}
	else if(orbittype == 1)
	{
		orbitpoint->X = planepnt->AI.TargetPos.X + ConvertWayLoc((angzz * ftoffset));
		orbitpoint->Y = planepnt->AI.TargetPos.Y;
		orbitpoint->Z = planepnt->AI.TargetPos.Z + ConvertWayLoc(-(angzx * ftoffset));
	}
	else
	{
		orbitpoint->X = planepnt->AI.WayPosition.X + ConvertWayLoc((angzz * ftoffset));
		orbitpoint->Y = planepnt->AI.WayPosition.Y;
		orbitpoint->Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angzx * ftoffset));
	}
}

//**************************************************************************************
void AICheckOrbitCondition(PlaneParams *planepnt, int islead)
{
	int nocheck = 0;
	PlaneParams *checkplane = NULL;
	float dx, dy, dz, tdist;
	OrbitActionType *pOrbitAction;
	int endorbit = 0;
	int actcnt, startact, endact;
	int orbitact = -1;
	MBWayPoints *waypnt;
	PlaneParams *leadplane;


	switch(planepnt->AI.iVar1)
	{
		case 0:
			nocheck = 0;
			break;
		case 1:
			checkplane = PlayerPlane;
			break;
		case 2:
			if(AICheckForEscortsArriving(planepnt))
			{
				leadplane = AIGetLeader(planepnt);
				if(leadplane->AI.Behaviorfunc != planepnt->AI.Behaviorfunc)
				{
					leadplane = planepnt;
				}

				if((MultiPlayer) && (leadplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(leadplane, GM4_DISENGAGE, 2, 0, 0);
				}
				leadplane->AI.Behaviorfunc = AIFlyFormation;
				AINextWayPoint(leadplane);
			}
			nocheck = 1;
//			checkplane = Escort Plane;
			break;
		case 3:
			if(AICheckForEscortsArriving(planepnt))
			{
				leadplane = AIGetLeader(planepnt);
				if(leadplane->AI.Behaviorfunc != planepnt->AI.Behaviorfunc)
				{
					leadplane = planepnt;
				}

				if((MultiPlayer) && (leadplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(leadplane, GM4_DISENGAGE, 2, 0, 0);
				}
				leadplane->AI.Behaviorfunc = AIFlyFormation;
				AINextWayPoint(leadplane);
			}
			nocheck = 1;
//			checkplane = Cover Plane;
			break;
		default:
			nocheck = 1;
			break;
	}

	if(nocheck)
	{
		return;
	}

	if(checkplane)
	{
		dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
		dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
		dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx,dz) * WUTOFT;
	//	if(tdist < (NMTOFT * 5))
		if(AIInNextRange(planepnt, tdist,  (5 * NMTOFT)))
		{
			leadplane = AIGetLeader(planepnt);
			if(leadplane->Status & PL_AI_DRIVEN)
			{
				AIChangeGroupBehavior(leadplane, leadplane->AI.Behaviorfunc, AIFlyFormation);
				AINextWayPoint(leadplane);
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(leadplane, GM4_DISENGAGE, 10, 0, 0);
				}
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIFlyFormation;
				AINextWayPoint(planepnt);
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
				}
			}
	//		if(islead)
	//		{
	//			AICGiveContactMessage(planepnt, checkplane);
	//		}
			return;
		}
	}

	waypnt = planepnt->AI.CurrWay;
	startact = waypnt->iStartAct;
	endact = startact + waypnt->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if(AIActions[actcnt].ActionID == ACTION_ORBIT)
		{
			orbitact = actcnt;
			break;
		}
	}

	if(orbitact == -1)
		return;

	pOrbitAction = (OrbitActionType *)AIActions[orbitact].pAction;

	if(pOrbitAction->lFlag == 0)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			endorbit = 1;
		}
	}
	else
	{
		endorbit = AICheckEventFlagStatus(pOrbitAction->lFlag);
	}

	if(endorbit)
	{
		leadplane = AIGetLeader(planepnt);
		if(leadplane->Status & PL_AI_DRIVEN)
		{
			AIChangeGroupBehavior(leadplane, leadplane->AI.Behaviorfunc, AIFlyFormation);
			AINextWayPoint(leadplane);
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(leadplane, GM4_DISENGAGE, 10, 0, 0);
			}
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			AINextWayPoint(planepnt);
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
		}
	}
}

//**************************************************************************************
void AIFlyToOrbitPoint(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	int placeingroup;
	int nowinglead = 0;
	int leadnum = -1;
	int tempnum;
	FPoint orbitpoint;
	float tdist, dx, dy, dz, offangle;

	leadplane = AIGetLeader(planepnt);

	if(leadplane->Status & PL_AI_DRIVEN)
	{
		if(leadplane == planepnt)
		{
			AIGetOrbitOffset(planepnt, &orbitpoint);
			tdist = FlyToPoint(planepnt, orbitpoint);
//			if(tdist < 1000)
			if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
			{
				if(!AIInPlayerGroup(planepnt))
				{
					AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay);
				}
				planepnt->AI.Behaviorfunc = AIFlyOrbit;
			}
			MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

			dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
			dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
			dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

			tdist = QuickDistance(dx,dz) * WUTOFT;
//			if(tdist < planepnt->AI.lVar2)
			if(AIInNextRange(planepnt, tdist,  planepnt->AI.lVar2, 25.0f))
			{
				offangle = AIComputeHeadingToPoint(planepnt, orbitpoint, &tdist, &dx ,&dy, &dz, 1);
				if(fabs(offangle) > 60)
				{
					planepnt->DesiredRoll = 0;
				}
//				planepnt->AI.Behaviorfunc = AIFlyOrbit;
			}

			AIUpdateFormation(planepnt);
			AICheckOrbitCondition(planepnt, 1);
		}
		else
		{
			AIFormationFlying(planepnt);
			AICheckOrbitCondition(planepnt, 0);
			if(leadplane->AI.Behaviorfunc == AIFlyOrbit)
			{
				planepnt->AI.Behaviorfunc = AIFlyOrbit;
			}
		}
		return;
	}
	else
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);


		if(planepnt->AI.winglead >= 0)
		{
			if(Planes[planepnt->AI.winglead].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				nowinglead = 1;
			}
			else
			{
				leadnum = planepnt->AI.winglead;
				if((Planes[leadnum].AI.iAIFlags1 & AIFLIGHTNUMS) > 5)
				{
					tempnum = Planes[leadnum].AI.prevpair;
					if(tempnum >= 0)
					{
						if((Planes[tempnum].AI.iAIFlags1 & AIFLIGHTNUMS) > 3)
						{
							leadnum = tempnum;
						}
					}
				}
			}
		}
		else if(placeingroup > 5)
		{
			tempnum = planepnt->AI.prevpair;
			if(tempnum >= 0)
			{
				if((Planes[tempnum].AI.iAIFlags1 & AIFLIGHTNUMS) > 3)
				{
					leadnum = tempnum;
				}
				else
				{
					nowinglead = 1;
				}
			}
			else
			{
				nowinglead = 1;
			}
		}
		else
		{
			nowinglead = 1;
		}

		if((placeingroup == 1) || (placeingroup == 2) || (placeingroup == 4) || (nowinglead))
		{
			AICheckOrbitCondition(planepnt, 1);
			AIGetOrbitOffset(planepnt, &orbitpoint);
			tdist = FlyToPoint(planepnt, orbitpoint);
//			if(tdist < 1000)
			if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
			{
				planepnt->AI.Behaviorfunc = AIFlyOrbit;
			}
			MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

			dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
			dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
			dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

			tdist = QuickDistance(dx,dz) * WUTOFT;
//			if(tdist < planepnt->AI.lVar2)
			if(AIInNextRange(planepnt, tdist,  planepnt->AI.lVar2, 25.0f))
			{
				offangle = AIComputeHeadingToPoint(planepnt, orbitpoint, &tdist, &dx ,&dy, &dz, 1);
				if(fabs(offangle) > 60)
				{
					planepnt->DesiredRoll = 0;
				}
//				planepnt->AI.Behaviorfunc = AIFlyOrbit;
			}
			AIUpdateBehaviorFormation(planepnt);
		}
		else
		{
			AIFormationFlyingLead(planepnt, &Planes[leadnum]);
			AICheckOrbitCondition(planepnt, 0);
			planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
			if(Planes[leadnum].AI.Behaviorfunc == AIFlyOrbit)
			{
				planepnt->AI.Behaviorfunc = AIFlyOrbit;
			}
		}
	}
}

//**************************************************************************************
void AIFlyOrbit(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	int placeingroup;
	int nowinglead = 0;
	float dx, dy, dz, tdist;
	int leadnum = -1;
	int tempnum;
	float ftoffset;

	leadplane = AIGetLeader(planepnt);
	ftoffset = planepnt->AI.lVar2;

	if(leadplane->Status & PL_AI_DRIVEN)
	{
		if(leadplane == planepnt)
		{
			AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 0);
			if((tdist * WUTOFT) >  (ftoffset * 2))
			{
				planepnt->AI.Behaviorfunc = AIFlyToOrbitPoint;
				AIFlyToOrbitPoint(planepnt);
				return;
			}

			planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
			MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
			dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
			planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
			AICheckOrbitCondition(planepnt, 1);
			AIUpdateFormation(planepnt);
		}
		else
		{
			AIFormationFlying(planepnt);
			AICheckOrbitCondition(planepnt, 0);
			planepnt->AI.Behaviorfunc = leadplane->AI.Behaviorfunc;
			planepnt->AI.OrgBehave = leadplane->AI.OrgBehave;
		}
		return;
	}
	else
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);


		if(planepnt->AI.winglead >= 0)
		{
			if(Planes[planepnt->AI.winglead].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				nowinglead = 1;
			}
			else
			{
				leadnum = planepnt->AI.winglead;
				if((Planes[leadnum].AI.iAIFlags1 & AIFLIGHTNUMS) > 5)
				{
					tempnum = Planes[leadnum].AI.prevpair;
					if(tempnum >= 0)
					{
						if((Planes[tempnum].AI.iAIFlags1 & AIFLIGHTNUMS) > 3)
						{
							leadnum = tempnum;
						}
					}
				}
			}
		}
		else if(placeingroup > 5)
		{
			tempnum = planepnt->AI.prevpair;
			if(tempnum >= 0)
			{
				if((Planes[tempnum].AI.iAIFlags1 & AIFLIGHTNUMS) > 3)
				{
					leadnum = tempnum;
				}
				else
				{
					nowinglead = 1;
				}
			}
			else
			{
				nowinglead = 1;
			}
		}
		else
		{
			nowinglead = 1;
		}

		if((placeingroup == 1) || (placeingroup == 2) || (placeingroup == 4) || (nowinglead))
		{
			AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 0);
			if((tdist * WUTOFT) >  (ftoffset * 2))
			{
				planepnt->AI.Behaviorfunc = AIFlyToOrbitPoint;
				AIFlyToOrbitPoint(planepnt);
				return;
			}

			AICheckOrbitCondition(planepnt, 1);
			planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
			MBAdjustAIThrust(planepnt, 350, 1);
			dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
			planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
			AIUpdateBehaviorFormation(planepnt);
		}
		else
		{
			AIFormationFlyingLead(planepnt, &Planes[leadnum]);
			planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
			AICheckOrbitCondition(planepnt, 0);
		}
	}
}

//**************************************************************************************
float AIGetTurnRateForOrbit(float radft, long desiredspeed, float altitude)
{
	float vfps, circlec, secstofly, degpersec;

	vfps = AIConvertSpeedToFtPS(altitude, desiredspeed, AIDESIREDSPDCALC);
	circlec = 2 * PI * radft;
	secstofly = circlec / vfps;  //  ft / ft/sec = sec;
	degpersec = 360 / secstofly;  //  deg / sec = deg/sec;

	return(degpersec);
}

int lvcolor = 0;
int iWatchPlane = -1;
//**************************************************************************************
void AIDrawSimpleRadar()
{
	PlaneParams *planepnt = (PlaneParams *)Camera1.AttachedObject;
  	PlaneParams *checkplane = &Planes[0];
	MovingVehicleParams *checkvehicle = MovingVehicles;
	float radarrange, rangenm;
	float dx, dy, dz, tdist;
	float offangle, toffangle;
	float targbearing;
	float ftempx, ftempz, fpixsize, fdrawx, fdrawz;
	float flinex, flinez;
	float fboxsize;
	float angzx, angzz, heading;
	int redcolor = 3;
	int yellowcolor = 6;
	int greencolor = 63;
	int blackcolor = 207;
	int graycolor = 10;
//	int usecolor;
	int cnt;
	float radarangle;
	int tred, tgreen, tblue;
	int usepallet = 0;
	int bred, bgreen, bblue;

	if(iWatchPlane >= 0)
	{
		planepnt = &Planes[iWatchPlane];
	}

#ifdef _DEBUG
//	SetWindowPos(hwnd, HWND_BOTTOM, 0,0, 640, 480, 0);
#endif

	rangenm = iAIShowRadarRange;

	fdrawx = 320;
	fdrawz = 240;
	fboxsize = 1;


	if(usepallet)
	{
		bred = tred = blackcolor;
		bgreen = tgreen = 0;
		bblue = tblue = 0;
	}
	else
	{
		bred = tred = 1;
		bgreen = tgreen = 1;
		bblue = tblue = 1;
	}

	GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
	GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
	GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
	GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);

	if(rangenm > 5)
	{
		ftempz = 240 / rangenm;
		ftempx = ftempz * 5;

		if(usepallet)
		{
			tred = graycolor;
			tgreen = 0;
			tblue = 0;
		}
		else
		{
			tred = 127;
			tgreen = 127;
			tblue = 127;
		}

		GrDrawLineClipped(GrBuffFor3D, 310, 240 - ftempx, 330, 240 - ftempx, tred, tgreen, tblue, usepallet);
		GrDrawLineClipped(GrBuffFor3D, 320 - ftempx, 230, 320 - ftempx, 250, tred, tgreen, tblue, usepallet);
		GrDrawLineClipped(GrBuffFor3D, 310, 240 + ftempx, 330, 240 + ftempx, tred, tgreen, tblue, usepallet);
		GrDrawLineClipped(GrBuffFor3D, 320 + ftempx, 230, 320 + ftempx, 250, tred, tgreen, tblue, usepallet);

		for(cnt = 10; cnt < rangenm; cnt += 10)
		{
			ftempx = ftempz * cnt;

			GrDrawLineClipped(GrBuffFor3D, 310, 240 - ftempx, 330, 240 - ftempx, bred, bgreen, bblue, usepallet);
			GrDrawLineClipped(GrBuffFor3D, 320 - ftempx, 230, 320 - ftempx, 250, bred, bgreen, bblue, usepallet);
			GrDrawLineClipped(GrBuffFor3D, 310, 240 + ftempx, 330, 240 + ftempx, bred, bgreen, bblue, usepallet);
			GrDrawLineClipped(GrBuffFor3D, 320 + ftempx, 230, 320 + ftempx, 250, bred, bgreen, bblue, usepallet);
		}
	}

	radarrange = (rangenm * NMTOWU);
	fpixsize = (rangenm * NMTOFT) / 240;
	fboxsize = 3;

	if(planepnt->Status & PL_AI_DRIVEN)
	{
		radarangle = planepnt->AI.fRadarAngleYaw - AIConvertAngleTo180Degree(planepnt->Heading);

		toffangle = radarangle - 20;
		toffangle = AICapAngle(toffangle);

		heading = DegToRad((double)toffangle);
		angzx = sin(heading);
		angzz = cos(heading);
		ftempx = (angzx * radarrange) * WUTOFT;
		ftempz = (angzz * radarrange) * WUTOFT;
		fdrawx = 320 - (ftempx / fpixsize);
		fdrawz = 240 - (ftempz / fpixsize);

		if(usepallet)
		{
			tred = graycolor;
			tgreen = 0;
			tblue = 0;
		}
		else
		{
			tred = 127;
			tgreen = 127;
			tblue = 127;
		}

		GrDrawLineClipped(GrBuffFor3D, 320, 240, fdrawx, fdrawz, tred, tgreen, tblue, usepallet);

		toffangle = radarangle + 20;
		toffangle = AICapAngle(toffangle);

		heading = DegToRad((double)toffangle);
		angzx = sin(heading);
		angzz = cos(heading);
		ftempx = (angzx * radarrange) * WUTOFT;
		ftempz = (angzz * radarrange) * WUTOFT;
		fdrawx = 320 - (ftempx / fpixsize);
		fdrawz = 240 - (ftempz / fpixsize);

		GrDrawLineClipped(GrBuffFor3D, 320, 240, fdrawx, fdrawz, tred, tgreen, tblue, usepallet);
	}

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt != checkplane))
		{
			dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
			if((fabs(dx) < radarrange) && (fabs(dz) < radarrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < radarrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					offangle = AICapAngle(offangle);

					toffangle = AICapAngle(toffangle);

					heading = DegToRad((double)toffangle);
					angzx = sin(heading);
					angzz = cos(heading);

//					ftempx = ((angzz * dx) + (angzx * dz)) * WUTOFT;
//					ftempz = ((angzx * dx) + (angzz * dz)) * WUTOFT;
					ftempx = (angzx * tdist) * WUTOFT;
					ftempz = (angzz * tdist) * WUTOFT;
					fdrawx = 320 - (ftempx / fpixsize);
					fdrawz = 240 - (ftempz / fpixsize);
					tgreen = 0;
					tblue = 0;
					if(checkplane->FlightStatus & PL_OUT_OF_CONTROL)
					{
						if(usepallet)
						{
							tred = blackcolor;
						}
						else
						{
							tred = 1;
							tgreen = 1;
							tblue = 1;
						}
					}
    				else
					{
						switch(checkplane->AI.iSide)
						{
							case 0:
								if(usepallet)
								{
									tred = redcolor;
								}
								else
								{
									tred = 255;
								}
								break;
							case 1:
								if(usepallet)
								{
									tred = greencolor;
								}
								else
								{
									tred = 0;
									tgreen = 255;
								}
								break;
							case 2:
								if(usepallet)
								{
									tred = yellowcolor;
								}
								else
								{
									tred = 255;
									tgreen = 255;
								}
								break;
							default:
								if(usepallet)
								{
									tred = redcolor;
								}
								else
								{
									tred = 255;
								}
								break;
						}
					}
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, tred, tgreen, tblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
					if(planepnt->AI.AirTarget == checkplane)
					{
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
					}

					if(planepnt->AI.AirThreat == checkplane)
					{
						GrDrawLineClipped(GrBuffFor3D, fdrawx, fdrawz - fboxsize, fdrawx, fdrawz + fboxsize, blackcolor, 0, 0, 1);
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz, fdrawx + fboxsize, fdrawz, blackcolor, 0, 0, 1);
					}
					fboxsize ++;

					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);

					if(checkplane->AI.iAIFlags2 & AISUPPORT)
					{
						fboxsize ++;
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, tred, tgreen, tblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
						fboxsize --;
					}

					fboxsize --;

					if(checkplane->AI.iAIFlags2 & AISUPPORT)
					{
						fboxsize --;
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
						GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
						fboxsize ++;
					}

					targbearing = AIConvertAngleTo180Degree(checkplane->Heading - planepnt->Heading);
					heading = DegToRad((double)targbearing);
					angzx = sin(heading);
					angzz = cos(heading);

					ftempx = (angzx * fboxsize * 3);
					ftempz = (angzz * fboxsize * 3);
					flinex = fdrawx - ftempx;
					flinez = fdrawz - ftempz;
					GrDrawLineClipped(GrBuffFor3D, fdrawx - 1, fdrawz, flinex - 1, flinez, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx, fdrawz, flinex, flinez, tred, tgreen, tblue, usepallet);
				}
			}
		}
		checkplane ++;
	}

	while (checkvehicle <= LastMovingVehicle)
	{
		if((checkvehicle->Status & VL_ACTIVE) && (!(checkvehicle->Status & (VL_INVISIBLE))))
		{
			dx = checkvehicle->WorldPosition.X - planepnt->WorldPosition.X;
			dy = checkvehicle->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = checkvehicle->WorldPosition.Z - planepnt->WorldPosition.Z;
			if((fabs(dx) < radarrange) && (fabs(dz) < radarrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < radarrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					offangle = AICapAngle(offangle);

					toffangle = AICapAngle(toffangle);

					heading = DegToRad((double)toffangle);
					angzx = sin(heading);
					angzz = cos(heading);

//					ftempx = ((angzz * dx) + (angzx * dz)) * WUTOFT;
//					ftempz = ((angzx * dx) + (angzz * dz)) * WUTOFT;
					ftempx = (angzx * tdist) * WUTOFT;
					ftempz = (angzz * tdist) * WUTOFT;
					fdrawx = 320 - (ftempx / fpixsize);
					fdrawz = 240 - (ftempz / fpixsize);
					tgreen = 0;
					tblue = 0;
					switch(checkvehicle->iSide)
					{
						case 0:
							if(usepallet)
							{
								tred = redcolor;
							}
							else
							{
								tred = 255;
							}
							break;
						case 1:
							if(usepallet)
							{
								tred = greencolor;
							}
							else
							{
								tred = 0;
								tgreen = 255;
							}
							break;
						case 2:
							if(usepallet)
							{
								tred = yellowcolor;
							}
							else
							{
								tred = 255;
								tgreen = 255;
							}
							break;
						default:
							if(usepallet)
							{
								tred = redcolor;
							}
							else
							{
								tred = 255;
							}
							break;
					}
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, tred, tgreen, tblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, tred, tgreen, tblue, usepallet);

					fboxsize ++;

					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					fboxsize ++;

					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);

					fboxsize ++;

					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);

					fboxsize ++;

					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, bred, bgreen, bblue, usepallet);


					fboxsize --;
					fboxsize --;
					fboxsize --;
					fboxsize --;

					targbearing = AIConvertAngleTo180Degree(checkvehicle->Heading - planepnt->Heading);
					heading = DegToRad((double)targbearing);
					angzx = sin(heading);
					angzz = cos(heading);

					ftempx = (angzx * fboxsize * 3);
					ftempz = (angzz * fboxsize * 3);
					flinex = fdrawx - ftempx;
					flinez = fdrawz - ftempz;
					GrDrawLineClipped(GrBuffFor3D, fdrawx - 1, fdrawz, flinex - 1, flinez, bred, bgreen, bblue, usepallet);
					GrDrawLineClipped(GrBuffFor3D, fdrawx, fdrawz, flinex, flinez, tred, tgreen, tblue, usepallet);
				}
			}
		}
		checkvehicle ++;
	}
}

//**************************************************************************************
void AIDoMessageAction(MessageActionType *pMessageAction)
{
	char wavestr[_MAX_PATH];
	char *tempstr;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	AICAddAIRadioMsgs(pMessageAction->sMessage, 69);

	if(strlen(pMessageAction->sSoundFile) < 2)
	{
		return;
	}

	OutputDebugString(pMessageAction->sMessage);
	OutputDebugString("\n");
	OutputDebugString(pMessageAction->sSoundFile);
	OutputDebugString("\n");

	if(SndStreamInit(pMessageAction->sSoundFile))
	{
		SndStreamPlay();
	}
	else
	{
		_splitpath(pMessageAction->sSoundFile, drive, dir, fname, ext);
		tempstr = GetRegValue ("resource");

		sprintf(wavestr, "%s\\Temp\\%s%s", tempstr, fname, ext);

		if(SndStreamInit(wavestr))
		{
			SndStreamPlay();
		}
		else
		{
			sprintf(wavestr, "%s\\%s%s", g_szExePath, fname, ext);

			if(SndStreamInit(wavestr))
			{
				SndStreamPlay();
			}
			else
			{
				tempstr = GetRegValue("mission");
				sprintf(wavestr, "%s\\%s%s", tempstr, fname, ext);

				if(SndStreamInit(wavestr))
				{
					SndStreamPlay();
				} else {
					sprintf(wavestr, "%s%s", fname, ext);
					if(SndStreamInit(wavestr))
					{
						SndStreamPlay();
					}
				}
			}
		}
	}

}

//**************************************************************************************
void AIDoMessageEvent(EventActionMessageType *pMessageAction)
{
	char wavestr[_MAX_PATH];
	char *tempstr;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];


	AICAddAIRadioMsgs(pMessageAction->sMessage, 69);

	if(strlen(pMessageAction->sSoundFile) < 2)
	{
		return;
	}

	if(g_iBaseSpeechSoundLevel <= 0)
		return;

	if(SndStreamInit(pMessageAction->sSoundFile))
	{
		SndStreamSetVolume(g_iBaseSpeechSoundLevel);
		SndStreamPlay();
	}
	else
	{
		_splitpath(pMessageAction->sSoundFile, drive, dir, fname, ext);
		tempstr = GetRegValue ("resource");
		sprintf(wavestr, "%s\\Temp\\%s%s", tempstr, fname, ext);

		if(SndStreamInit(wavestr))
		{
			SndStreamSetVolume(g_iBaseSpeechSoundLevel);
			SndStreamPlay();
		}
		else
		{
			sprintf(wavestr, "%s\\%s%s", g_szExePath, fname, ext);

			if(SndStreamInit(wavestr))
			{
				SndStreamSetVolume(g_iBaseSpeechSoundLevel);
				SndStreamPlay();
			}
			else
			{
				tempstr = GetRegValue("mission");
				sprintf(wavestr, "%s\\%s%s", tempstr, fname, ext);

				if(SndStreamInit(wavestr))
				{
					SndStreamSetVolume(g_iBaseSpeechSoundLevel);
					SndStreamPlay();
				} else {
					sprintf(wavestr, "%s%s", fname, ext);
					if(SndStreamInit(wavestr))
					{
						SndStreamSetVolume(g_iBaseSpeechSoundLevel);
						SndStreamPlay();
					}
				}
			}
		}
	}

}

//**************************************************************************************
void AIUpdateGoalStates(int planenum, int replacenum, int leadnum, int objecttype)
{
	int cnt;
	BasicInstance *walker;
	RunwayInfo *runway;
	RunwayPieceInstance *rwalker;
	int rcnt;

	for(cnt = 0; cnt < iNumGoals; cnt ++)
	{
		if(cAIGoalSame[cnt])
		{
			if((GoalList[cnt].lObjectType == AIRCRAFT) && (objecttype == AIRCRAFT))
			{
				if(GoalList[cnt].dwSerialNumber == (DWORD)leadnum)
				{
					if(planenum == leadnum)
					{
						GoalList[cnt].dwSerialNumber = replacenum;
					}
					cAIGoalSame[cnt] = cAIGoalSame[cnt] - 1;
					if(cAIGoalSame[cnt] <= 0)
					{
						AICheckEvents(EVENT_GOAL, cnt);
						AICheckEvents(EVENT_MISSION_END, cnt, 1);
					}
				}
			}
			else if((GoalList[cnt].lObjectType == MOVINGVEHICLE) && (objecttype == MOVINGVEHICLE))
			{
				if(GoalList[cnt].dwSerialNumber == (DWORD)leadnum)
				{
					cAIGoalSame[cnt] = cAIGoalSame[cnt] - 1;
					if(cAIGoalSame[cnt] <= 0)
					{
						AICheckEvents(EVENT_GOAL, cnt);
						AICheckEvents(EVENT_MISSION_END, cnt, 1);
					}
				}
			}
			else if((GoalList[cnt].lObjectType == GROUNDOBJECT) && (objecttype == GROUNDOBJECT))
			{
				if(GoalList[cnt].dwSerialNumber == (DWORD)planenum)
				{
					cAIGoalSame[cnt] = cAIGoalSame[cnt] - 1;
					if(cAIGoalSame[cnt] <= 0)
					{
						AICheckEvents(EVENT_GOAL, cnt);
						AICheckEvents(EVENT_MISSION_END, cnt, 1);
					}
				}
				else
				{
					walker = FindInstance(AllInstances, (DWORD)planenum);
					if(walker->Family == FAMILY_RUNWAYPIECE)
					{
						rwalker = (RunwayPieceInstance *)walker;
						if(rwalker->Runway)
						{
							runway = (RunwayInfo *)rwalker->Runway;
							for(rcnt = 0; rcnt < 16; rcnt ++)
							{
								if(runway->Pieces[rcnt])
								{
									if(runway->Pieces[rcnt]->Basics.SerialNumber == GoalList[cnt].dwSerialNumber)
									{
										cAIGoalSame[cnt] = cAIGoalSame[cnt] - 1;
										if(cAIGoalSame[cnt] <= 0)
										{
											AICheckEvents(EVENT_GOAL, cnt);
											AICheckEvents(EVENT_MISSION_END, cnt, 1);
										}
										rcnt = 17;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

//**************************************************************************************
void AICheckEvents(int changeid, long changeval, long extraflag)
{
	int cnt;
	int eventoccurs;
	TimeEventType *TimeEvent;
	AreaEventType *AreaEvent;
	MovingAreaEventType *MovingAreaEvent;
	GoalEventType *GoalEvent;
	ObjectEventType *ObjectEvent;
	MissionEndEventType *EndEvent;

	for(cnt = 0; cnt < g_iNumEvents; cnt ++)
	{
		if(changeid == EventList[cnt].lEventID)
		{
			eventoccurs = 0;
			switch(changeid)
			{
			case EVENT_TIME:
				TimeEvent = (TimeEventType *)EventList[cnt].pEvent;
				if((TimeEvent->iStart < 0) && (extraflag == 0))
				{
					if(TimeEvent->iTime == changeval)
					{
						eventoccurs = 1;
					}
				}
				if((TimeEvent->iStart >= 0) && (TimeEvent->iStart == changeval))
				{
					eventoccurs = 1;
				}
				if(eventoccurs)
				{
					AICheckTimeEvent(TimeEvent, cnt);
				}
				break;
			case EVENT_AREA:
				AreaEvent = (AreaEventType *)EventList[cnt].pEvent;
				if(AIScanEventArea(AreaEvent, cnt))
				{
					AICheckAreaEvent(AreaEvent, cnt);
				}
				break;
			case EVENT_MOVING_AREA:
				MovingAreaEvent = (MovingAreaEventType *)EventList[cnt].pEvent;
				if(AIScanMovingEventArea(MovingAreaEvent, cnt))
				{
					AICheckMovingAreaEvent(MovingAreaEvent, cnt);
				}
				break;
			case EVENT_GOAL:
				GoalEvent = (GoalEventType *)EventList[cnt].pEvent;
				if(GoalEvent->iGoalNum == changeval)
				{
					AICheckGoalEvent(GoalEvent, cnt);
					AICheckEvents(EVENT_MISSION_END, cnt, 1);
				}
				break;
			case EVENT_OBJECT:
				ObjectEvent = (ObjectEventType *)EventList[cnt].pEvent;
				if(ObjectEvent->iObjectType == 0)
				{
					if(ObjectEvent->iObjectNum == changeval)
					{
						AICheckObjectEvent(ObjectEvent, cnt);
						AICheckEvents(EVENT_MISSION_END, cnt, 1);
					}
				}
				break;
			case EVENT_MISSION_END:
				EndEvent = (MissionEndEventType *)EventList[cnt].pEvent;
				AICheckEndEvent(EndEvent, cnt);
				break;
			}
		}
	}
}

//**************************************************************************************
void AIDoDelayedEvents(int eventnum)
{
	TimeEventType *TimeEvent;
	AreaEventType *AreaEvent;
	GoalEventType *GoalEvent;
	ObjectEventType *ObjectEvent;
	MissionEndEventType *EndEvent;

	switch(EventList[eventnum].lEventID)
	{
		case EVENT_TIME:
			TimeEvent = (TimeEventType *)EventList[eventnum].pEvent;
			AIDoTimeEvent(TimeEvent);
			break;
		case EVENT_AREA:
			AreaEvent = (AreaEventType *)EventList[eventnum].pEvent;
			AIDoAreaEvent(AreaEvent);
			break;
		case EVENT_GOAL:
			GoalEvent = (GoalEventType *)EventList[eventnum].pEvent;
			AIDoGoalEvent(GoalEvent);
			break;
		case EVENT_OBJECT:
			ObjectEvent = (ObjectEventType *)EventList[eventnum].pEvent;
			AIDoObjectEvent(ObjectEvent);
			break;
		case EVENT_MISSION_END:
			EndEvent = (MissionEndEventType *)EventList[eventnum].pEvent;
			AIDoEndEvent(EndEvent);
			break;
	}
}

//**************************************************************************************
void AICheckTimeEvent(TimeEventType *TimeEvent, int eventnum)
{
	if(!AICheckGeneralEvent(TimeEvent->iNumEventConditions, TimeEvent->EventCondition))
	{
		return;
	}

	if(TimeEvent->iDuration <= 0)
	{
		AIDoTimeEvent(TimeEvent);
	}
	else
	{
		if(iAIEventDelay[eventnum] < 0)
		{
			iAIEventDelay[eventnum] = 60000 * TimeEvent->iDuration;
		}
	}
}

//**************************************************************************************
void AICheckAreaEvent(AreaEventType *AreaEvent, int eventnum)
{
	long event_bit_flag;
	long event_bit_flag2;

	if(!AICheckGeneralEvent(AreaEvent->iNumEventConditions, AreaEvent->EventCondition))
	{
		return;
	}

	if(eventnum < 32)
	{
		event_bit_flag = 1 << eventnum;
		event_bit_flag2 = 0;
	}
	else
	{
		event_bit_flag = 0;
		event_bit_flag2 = 1 << (eventnum - 32);
	}
	lAreaEventOccuranceFlags |= event_bit_flag;
	l2AreaEventOccuranceFlags |= event_bit_flag2;

	AIDoAreaEvent(AreaEvent);
#if 0
	if(AreaEvent->iDuration <= 0)
	{
		AIDoAreaEvent(AreaEvent);
	}
	else
	{
		if(iAIEventDelay[eventnum] < 0)
		{
			iAIEventDelay[eventnum] = 60000 * AreaEvent->iDuration;
		}
	}
#endif
}

//**************************************************************************************
void AICheckMovingAreaEvent(MovingAreaEventType *AreaEvent, int eventnum)
{
	long event_bit_flag;
	long event_bit_flag2;

	if(!AICheckGeneralEvent(AreaEvent->iNumEventConditions, AreaEvent->EventCondition))
	{
		return;
	}

	if(eventnum < 32)
	{
		event_bit_flag = 1 << eventnum;
		event_bit_flag2 = 0;
	}
	else
	{
		event_bit_flag = 0;
		event_bit_flag2 = 1 << (eventnum - 32);
	}
	lAreaEventOccuranceFlags |= event_bit_flag;
	l2AreaEventOccuranceFlags |= event_bit_flag2;

	AIDoMovingAreaEvent(AreaEvent);
#if 0
	if(AreaEvent->iDuration <= 0)
	{
		AIDoAreaEvent(AreaEvent);
	}
	else
	{
		if(iAIEventDelay[eventnum] < 0)
		{
			iAIEventDelay[eventnum] = 60000 * AreaEvent->iDuration;
		}
	}
#endif
}

//**************************************************************************************
void AICheckGoalEvent(GoalEventType *GoalEvent, int eventnum)
{
	if(!AICheckGeneralEvent(GoalEvent->iNumEventConditions, GoalEvent->EventCondition))
	{
		return;
	}

	AIDoGoalEvent(GoalEvent);

#if 0
	if(GoalEvent->iDuration <= 0)
	{
		AIDoGoalEvent(GoalEvent);
	}
	else
	{
		if(iAIEventDelay[eventnum] < 0)
		{
			iAIEventDelay[eventnum] = 60000 * GoalEvent->iDuration;
		}
	}
#endif
}

//**************************************************************************************
void AICheckObjectEvent(ObjectEventType *ObjectEvent, int eventnum)
{
	if(!AICheckGeneralEvent(ObjectEvent->iNumEventConditions, ObjectEvent->EventCondition))
	{
		return;
	}

	AIDoObjectEvent(ObjectEvent);

#if 0
	if(ObjectEvent->iDuration <= 0)
	{
		AIDoObjectEvent(ObjectEvent);
	}
	else
	{
		if(iAIEventDelay[eventnum] < 0)
		{
			iAIEventDelay[eventnum] = 60000 * ObjectEvent->iDuration;
		}
	}
#endif
}

//**************************************************************************************
void AICheckEndEvent(MissionEndEventType *EndEvent, int eventnum)
{
	int cnt;
	int chutedelay = 0;

	if(!AICheckGeneralEvent(EndEvent->iNumEventConditions, EndEvent->EventCondition))
	{
		return;
	}

	for(cnt = 0; cnt < EndEvent->iNumEventConditions; cnt ++)
	{
		if(!EndEvent->EventCondition[cnt].iType)
		{
			if(EndEvent->EventCondition[cnt].iIndex == (MISSION_FLAG_START + 2))
			{
				chutedelay = 1;
			}
		}
	}

	EndEvent->iNumEventConditions = -1;

	if(chutedelay)
	{
		iAIEventDelay[eventnum] = 15000;
	}
	else if(EndEvent->iDelay <= 0)
	{
		AIDoEndEvent(EndEvent);
	}
	else
	{
		if(iAIEventDelay[eventnum] < 0)
		{
			iAIEventDelay[eventnum] = 60000 * EndEvent->iDelay;
		}
	}
}

//**************************************************************************************
int AICheckGeneralEvent(int inumconditions, MBCondition *EventCondition)
{
	int cnt;
	int tempval;
	int finaltrue;

	if(inumconditions < 0)
	{
		return(0);
	}

	for(cnt = 0; cnt < inumconditions; cnt ++)
	{
		if(EventCondition[cnt].iType)
		{
			if(cAIGoalSame[EventCondition[cnt].iIndex] == 0)
			{
				tempval = 1;
			}
			else
			{
				tempval = 0;
			}
		}
		else
		{
			tempval = AICheckEventFlagStatus(EventCondition[cnt].iIndex);
		}
		finaltrue = EventCondition[cnt].wValue;
		if(EventCondition[cnt].wOperator == 0)
		{
			finaltrue = (finaltrue == 0) ? 1 : 0;
		}
		if(!(((tempval == 0) && (finaltrue == 0)) || ((tempval) && (finaltrue))))
		{
			return(0);
		}
	}

	return(1);
}

//**************************************************************************************
void AIDoTimeEvent(TimeEventType *TimeEvent)
{
	AIDoGeneralEvent(TimeEvent->iNumEventActions, TimeEvent->EventAction);
}

//**************************************************************************************
void AIDoAreaEvent(AreaEventType *AreaEvent)
{
	AIDoGeneralEvent(AreaEvent->iNumEventActions, AreaEvent->EventAction);
}

//**************************************************************************************
void AIDoMovingAreaEvent(MovingAreaEventType *AreaEvent)
{
	AIDoGeneralEvent(AreaEvent->iNumEventActions, AreaEvent->EventAction);
}

//**************************************************************************************
void AIDoGoalEvent(GoalEventType *GoalEvent)
{
	AIDoGeneralEvent(GoalEvent->iNumEventActions, GoalEvent->EventAction);
}

//**************************************************************************************
void AIDoObjectEvent(ObjectEventType *ObjectEvent)
{
	AIDoGeneralEvent(ObjectEvent->iNumEventActions, ObjectEvent->EventAction);
}

//**************************************************************************************
void AIDoEndEvent(MissionEndEventType *EndEvent)
{
	OldCockpitSeat    = CockpitSeat;
	OldCurrentCockpit = CurrentCockpit;
	memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));

//	if(lpDD)
//	  lpDD->FlipToGDISurface();


	if(iEndBoxOpt != 0)
	{
		iEndGameState = EndEvent->iFlag + 1;
	}

//	GetAsyncKeyState(VK_RETURN);
//	if(PRIMARYLANGID(g_iLanguageId) == LANG_ENGLISH)
//	{
//		GetAsyncKeyState('Y');
//		GetAsyncKeyState('N');
//	}
//	else if(PRIMARYLANGID(g_iLanguageId) == LANG_GERMAN)
//	{
//		GetAsyncKeyState('J');
//		GetAsyncKeyState('N');
//	}
//	else if(PRIMARYLANGID(g_iLanguageId) == LANG_FRENCH)
//	{
//		GetAsyncKeyState('O');
//		GetAsyncKeyState('N');
//	}
//	AIDoGeneralEvent(ObjectEvent->iNumEventActions, ObjectEvent->EventAction);
}

//**************************************************************************************
void AIDoGeneralEvent(int numactions, MBEvents *EventAction)
{
	int cnt;
	EventActionMessageType *pMessageAction;
	EventActionDamageType *pDamageAction;
	EventActionAltPathType *pAltPathAction;
	EventActionSetFlagType *pSetFlagAction;
	EventActionFACType		*pFACAction;
	BasicInstance *walker;

	for (cnt=0; cnt<numactions; cnt++)
	{
		if (EventAction[cnt].pEvent)
		{
			switch(EventAction[cnt].lEventID)
			{
				case EVENT_ACTION_NONE:
				break;
				case EVENT_ACTION_MESSAGE:
					pMessageAction = (EventActionMessageType *)EventAction[cnt].pEvent;
					AIDoMessageEvent(pMessageAction);
				break;
				case EVENT_ACTION_DAMAGE:
					pDamageAction = (EventActionDamageType *)EventAction[cnt].pEvent;
					AIDamagePlane(pDamageAction->iObjectID, pDamageAction->iDamageAmount);
				break;
				case EVENT_ACTION_SETFLAG:
					pSetFlagAction = (EventActionSetFlagType *)EventAction[cnt].pEvent;
					AIChangeEventFlag(pSetFlagAction->iFlag);
				break;
				case EVENT_ACTION_FAC:
					pFACAction = (EventActionFACType *)EventAction[cnt].pEvent;
					if(((iFACState & 0xFFFF) == 0) && (FACObject.iType != -1))
					{
						walker = FindInstance(AllInstances, FACObject.dwSerialNumber);
						fpFACPosition.X = ConvertWayLoc(FACObject.lX);
						fpFACPosition.Z = ConvertWayLoc(FACObject.lZ);
						if(walker)
						{
							fpFACPosition.Y = walker->Position.Y;
						}
						iFACState |= 1;
					}
				break;
			}
		}
	}

	for (cnt=0; cnt<numactions; cnt++)
	{
		if (EventAction[cnt].pEvent)
		{
			switch(EventAction[cnt].lEventID)
			{
				case EVENT_ACTION_ALTPATH:
					pAltPathAction = (EventActionAltPathType *)EventAction[cnt].pEvent;
					AIAlterPlanePath(pAltPathAction);
				break;
			}
		}
	}
}

//**************************************************************************************
void AIDamagePlane(long planenum, int damageamount)
{
	while(!(Planes[planenum].FlightStatus & PL_OUT_OF_CONTROL))
	{
		CrashPlane(&Planes[planenum],PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT, NULL);
	}
}

//**************************************************************************************
void AIChangeEventFlag(int flagval)
{
	int cnt;

	for(cnt = 0; cnt < 30; cnt ++)
	{
		if(iAIChangedEventFlags[cnt] == flagval)
		{
			return;
		}
		else if(iAIChangedEventFlags[cnt] == 0)
		{
			iAIChangedEventFlags[cnt] = flagval;
			AICheckEvents(EVENT_TIME, flagval, 1);
			AICheckEvents(EVENT_MISSION_END, flagval, 1);
			return;
		}
	}
}

//**************************************************************************************
int AICheckEventFlagStatus(int flagval)
{
	int cnt;

	if(flagval == 0)
	{
		return(1);
	}
	else if(flagval == 2100)
	{
		return(1);
	}

	for(cnt = 0; cnt < 30; cnt ++)
	{
		if(iAIChangedEventFlags[cnt] == flagval)
		{
			return(1);
		}
		else if(iAIChangedEventFlags[cnt] == 0)
		{
			return(0);
		}
	}
	return(0);
}

#if 0
#if 0
//**************************************************************************************
int AIScanEventArea(AreaEventType *AreaEvent, int eventnum)
{
	float minx, minz, maxx, maxz;
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	long event_bit_flag;
	long event_bit_flag2;
	int docheck;
	int returnval = 0;
	int checkval;
	int eventtrigger;

	if(eventnum < 32)
	{
		event_bit_flag = 1 << eventnum;
		event_bit_flag2 = 0;
	}
	else
	{
		event_bit_flag = 0;
		event_bit_flag2 = 1 << (eventnum - 32);
	}
	if(((lAreaEventOccuranceFlags & event_bit_flag) || (l2AreaEventOccuranceFlags & event_bit_flag2)) && (AreaEvent->iDuration == 1))
	{
		return(0);
	}

	minx = AreaEvent->x0 * FTTOWU;
	minz = AreaEvent->z0 * FTTOWU;
	maxx = AreaEvent->x1 * FTTOWU;
	maxz = AreaEvent->z1 * FTTOWU;
	eventtrigger = (AreaEvent->iWhen == 0) ? 1 : 0;

  	planepnt = &Planes[0];
	while (planepnt <= LastPlane)
	{
		docheck = 0;
		if((planepnt->WorldPosition.X >= minx) && (planepnt->WorldPosition.Z >= minz)
				&& (planepnt->WorldPosition.X <= maxx) && (planepnt->WorldPosition.Z <= maxz))
		{
			if(!((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2)))
			{
				docheck = 1;
				checkval = 1;
			}
		}
		else
		{
			if((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2))
			{
				docheck = 2;
				checkval = 0;
			}
		}

		if((docheck) && ((eventtrigger == checkval) || (AreaEvent->iWhen == 2)))
		{
			leadplane = AIGetLeader(planepnt);
			if(AICheckGroupForAreaEvent(leadplane, checkval, event_bit_flag, event_bit_flag2))
			{
				returnval = 1;
			}
		}

		if(docheck)
		{
			if(docheck == 1)
			{
				planepnt->AI.lAreaEventFlags |= event_bit_flag;
				planepnt->AI.l2AreaEventFlags |= event_bit_flag2;
			}
			else
			{
				planepnt->AI.lAreaEventFlags &= ~(event_bit_flag);
				planepnt->AI.l2AreaEventFlags &= ~(event_bit_flag2);
			}
		}

		planepnt ++;
	}
	return(returnval);
}
#else
//**************************************************************************************
int AIScanEventArea(AreaEventType *AreaEvent, int eventnum)
{
	float minx, minz, maxx, maxz;
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	long event_bit_flag;
	long event_bit_flag2;
	int docheck;
	int returnval = 0;
	int checkval;
	int eventtrigger;

	if(eventnum < 32)
	{
		event_bit_flag = 1 << eventnum;
		event_bit_flag2 = 0;
	}
	else
	{
		event_bit_flag = 0;
		event_bit_flag2 = 1 << (eventnum - 32);
	}
	if(((lAreaEventOccuranceFlags & event_bit_flag) || (l2AreaEventOccuranceFlags & event_bit_flag2)) && (AreaEvent->iDuration == 1))
	{
		return(0);
	}

	minx = AreaEvent->x0 * FTTOWU;
	minz = AreaEvent->z0 * FTTOWU;
	maxx = AreaEvent->x1 * FTTOWU;
	maxz = AreaEvent->z1 * FTTOWU;
	eventtrigger = (AreaEvent->iWhen == 0) ? 1 : 0;

  	planepnt = PlayerPlane;

	docheck = 0;
	if((planepnt->WorldPosition.X >= minx) && (planepnt->WorldPosition.Z >= minz)
			&& (planepnt->WorldPosition.X <= maxx) && (planepnt->WorldPosition.Z <= maxz))
	{
		if(!((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2)))
		{
			docheck = 1;
			checkval = 1;
		}
	}
	else
	{
		if((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2))
		{
			docheck = 2;
			checkval = 0;
		}
	}

	if((docheck) && ((eventtrigger == checkval) || (AreaEvent->iWhen == 2)))
	{
		leadplane = AIGetLeader(planepnt);
		if(AICheckGroupForAreaEvent(leadplane, checkval, event_bit_flag, event_bit_flag2))
		{
			returnval = 1;
		}
	}

	if(docheck)
	{
		if(docheck == 1)
		{
			planepnt->AI.lAreaEventFlags |= event_bit_flag;
			planepnt->AI.l2AreaEventFlags |= event_bit_flag2;
		}
		else
		{
			planepnt->AI.lAreaEventFlags &= ~(event_bit_flag);
			planepnt->AI.l2AreaEventFlags &= ~(event_bit_flag2);
		}
	}

	return(returnval);
}
#endif
#else
//**************************************************************************************
int AIScanEventArea(AreaEventType *AreaEvent, int eventnum)
{
	float minx, minz, maxx, maxz;
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	long event_bit_flag;
	long event_bit_flag2;
	int docheck;
	int returnval = 0;
	int checkval;
	int eventtrigger;
	int checkside;
	PlaneParams *lastplane;
	int modtrigger;
	int nothuman = 1;

	if(eventnum < 32)
	{
		event_bit_flag = 1 << eventnum;
		event_bit_flag2 = 0;
	}
	else
	{
		event_bit_flag = 0;
		event_bit_flag2 = 1 << (eventnum - 32);
	}
	if(((lAreaEventOccuranceFlags & event_bit_flag) || (l2AreaEventOccuranceFlags & event_bit_flag2)) && (AreaEvent->iDuration == 1))
	{
		return(0);
	}

	minx = AreaEvent->x0 * FTTOWU;
	minz = AreaEvent->z0 * FTTOWU;
	maxx = AreaEvent->x1 * FTTOWU;
	maxz = AreaEvent->z1 * FTTOWU;
//	eventtrigger = (AreaEvent->iWhen == 0) ? 1 : 0;
	modtrigger = AreaEvent->iWhen % 3;
	eventtrigger = (modtrigger == 0) ? 1 : 0;

	if(AreaEvent->iWhen < 3)
	{
		nothuman = 0;
		if(MultiPlayer)
		{
		  	planepnt = &Planes[0];
			lastplane = LastPlane;
			checkside = -1;
		}
		else
		{
			planepnt = PlayerPlane;
			lastplane = PlayerPlane;
			checkside = PlayerPlane->AI.iSide;
		}
	}
	else
	{
	  	planepnt = &Planes[0];
		lastplane = LastPlane;
		if(AreaEvent->iWhen < 6)
		{
			checkside = AI_FRIENDLY;
		}
		else if(AreaEvent->iWhen < 9)
		{
			checkside = AI_ENEMY;
		}
		else
		{
			checkside = -1;
		}
	}

	while (planepnt <= lastplane)
	{
		docheck = 0;
		if(((planepnt->AI.iSide == checkside) || (checkside == -1)) && ((nothuman) || ((planepnt == PlayerPlane) || (planepnt->Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN)) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))))
		{
			if((planepnt->WorldPosition.X >= minx) && (planepnt->WorldPosition.Z >= minz)
					&& (planepnt->WorldPosition.X <= maxx) && (planepnt->WorldPosition.Z <= maxz))
			{
				if(!((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2)))
				{
					docheck = 1;
					checkval = 1;
				}
			}
			else
			{
				if((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2))
				{
					docheck = 2;
					checkval = 0;
				}
			}
		}

		if((docheck) && ((eventtrigger == checkval) || (modtrigger == 2)))
		{
			leadplane = AIGetLeader(planepnt);
			if(AICheckGroupForAreaEvent(leadplane, checkval, event_bit_flag, event_bit_flag2))
			{
				returnval = 1;
			}
		}

		if(docheck)
		{
			if(docheck == 1)
			{
				planepnt->AI.lAreaEventFlags |= event_bit_flag;
				planepnt->AI.l2AreaEventFlags |= event_bit_flag2;
			}
			else
			{
				planepnt->AI.lAreaEventFlags &= ~(event_bit_flag);
				planepnt->AI.l2AreaEventFlags &= ~(event_bit_flag2);
			}
		}

		planepnt ++;
	}
	return(returnval);
}
#endif

//**************************************************************************************
int AIScanMovingEventArea(MovingAreaEventType *AreaEvent, int eventnum)
{
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	long event_bit_flag;
	long event_bit_flag2;
	int docheck;
	int returnval = 0;
	int checkval;
	int eventtrigger;
	int checkside;
	PlaneParams *lastplane;
	int modtrigger;
	FPointDouble position;
	float dx, dz;
	float tdist;
	float wurad = AreaEvent->lRange * NMTOWU;
	int inarea;
	int onlyplayer = 0;
	int tempval;
	int nothuman = 1;

	if(eventnum < 32)
	{
		event_bit_flag = 1 << eventnum;
		event_bit_flag2 = 0;
	}
	else
	{
		event_bit_flag = 0;
		event_bit_flag2 = 1 << (eventnum - 32);
	}
	if(((lAreaEventOccuranceFlags & event_bit_flag) || (l2AreaEventOccuranceFlags & event_bit_flag2)) && (AreaEvent->lDuration == 1))
	{
		return(0);
	}

	if(AreaEvent->lObjectType)
	{
//		position = MovingVehicles[AreaEvent->lObjectID].WorldPosition;
		tempval = VConvertVGListNumToVNum(AreaEvent->lObjectID);
		if(tempval < 0)
		{
			position = MovingVehicles[AreaEvent->lObjectID].WorldPosition;
		}
		else
		{
			position = MovingVehicles[tempval].WorldPosition;
		}
	}
	else
	{
		position = Planes[AreaEvent->lObjectID].WorldPosition;
	}

//	eventtrigger = (AreaEvent->iWhen == 0) ? 1 : 0;
	modtrigger = AreaEvent->lWhenCondition % 3;
	eventtrigger = (modtrigger == 0) ? 1 : 0;

	if(AreaEvent->lWhenCondition < 3)
	{
		nothuman = 0;
		if(MultiPlayer)
		{
		  	planepnt = &Planes[0];
			lastplane = LastPlane;
			checkside = -1;
		}
		else
		{
			planepnt = PlayerPlane;
			lastplane = PlayerPlane;
			checkside = PlayerPlane->AI.iSide;
		}
		onlyplayer = 1;
	}
	else
	{
	  	planepnt = &Planes[0];
		lastplane = LastPlane;
		if(AreaEvent->lWhenCondition < 6)
		{
			checkside = AI_FRIENDLY;
		}
		else if(AreaEvent->lWhenCondition < 9)
		{
			checkside = AI_ENEMY;
		}
		else
		{
			checkside = -1;
		}
	}

	while (planepnt <= lastplane)
	{
		docheck = 0;
		if(((planepnt->AI.iSide == checkside) || (checkside == -1)) && ((nothuman) || ((planepnt == PlayerPlane) || (planepnt->Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN)) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))))
		{
			inarea = 1;
			dx = fabs(position.X - planepnt->WorldPosition.X);
			dz = fabs(position.Z - planepnt->WorldPosition.Z);

			if((wurad < dx) && (wurad < dz))
			{
				inarea = 0;
			}
			else
			{
				tdist = sqrt((dx*dx)+(dz*dz));
				if(tdist > wurad)
				{
					inarea = 0;
				}
			}

			if(inarea)
			{
				if(!((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2)))
				{
					docheck = 1;
					checkval = 1;
				}
			}
			else
			{
				if((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2))
				{
					docheck = 2;
					checkval = 0;
				}
			}
		}

		if((docheck) && ((eventtrigger == checkval) || (modtrigger == 2)))
		{
			if(onlyplayer)
			{
				int bitcheck;

				bitcheck = ((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2)) ? 1 : 0;

				if(checkval != bitcheck)
				{
					returnval = 1;
				}
			}
			else
			{
				leadplane = AIGetLeader(planepnt);
				if(AICheckGroupForAreaEvent(leadplane, checkval, event_bit_flag, event_bit_flag2))
				{
					returnval = 1;
				}
			}
		}

		if(docheck)
		{
			if(docheck == 1)
			{
				planepnt->AI.lAreaEventFlags |= event_bit_flag;
				planepnt->AI.l2AreaEventFlags |= event_bit_flag2;
			}
			else
			{
				planepnt->AI.lAreaEventFlags &= ~(event_bit_flag);
				planepnt->AI.l2AreaEventFlags &= ~(event_bit_flag2);
			}
		}

		planepnt ++;
	}
	return(returnval);
}

//**************************************************************************************
void AIAlterPlanePath(EventActionAltPathType *pAltPathAction)
{
	int pathnum;
	int planenum;
	PlaneParams *planepnt;

	pathnum = pAltPathAction->iPath;
	planenum = AIWPPaths[pathnum].lObjLink;
	if((planenum < 0) || (planenum >= iNumAIObjs))
		return;

	planepnt = &Planes[planenum];

	if(planepnt == NULL)
		return;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	if(AIGetClosestAWACS(&Planes[planenum]) < 0)
	{
		return;
	}

	if(!(planepnt->Status & PL_AI_DRIVEN))
	{
		AICDoAltPathMsgs(planenum);
	}

	if(iInJump)
	{
		iInJump |= 4;
	}

	if((pathnum >= 0) && (pathnum < iNumWPPaths))
	{
		planepnt->AI.CurrWay = &AIWayPoints[AIWPPaths[pathnum].iStartWP];
		planepnt->AI.numwaypts = AIWPPaths[pathnum].iNumWP;
		planepnt->AI.startwpts = AIWPPaths[pathnum].iStartWP;
	}
}

//**************************************************************************************
int AICheckGroupForAreaEvent(PlaneParams *planepnt, int checkval, long event_bit_flag, long event_bit_flag2)
{
	int bitcheck;

	bitcheck = ((planepnt->AI.lAreaEventFlags & event_bit_flag) || (planepnt->AI.l2AreaEventFlags & event_bit_flag2)) ? 1 : 0;

	if(checkval == bitcheck)
	{
		return(0);
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(!AICheckGroupForAreaEvent(&Planes[planepnt->AI.wingman], checkval, event_bit_flag, event_bit_flag2))
		{
			return(0);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AICheckGroupForAreaEvent(&Planes[planepnt->AI.nextpair], checkval, event_bit_flag, event_bit_flag2));
	}

	return(1);
}

//**************************************************************************************
void AIUpdatePathObjLinks(long planenum, long replacenum)
{
	int cnt;

	for(cnt = 0; cnt < iNumWPPaths; cnt ++)
	{
		if(AIWPPaths[cnt].lObjLink == planenum)
		{
			AIWPPaths[cnt].lObjLink = replacenum;
		}
	}
}

//**************************************************************************************
void AIDamageObjectGroup(long planenum, long damageamount)
{
	if(Planes[planenum].AI.nextpair >= 0)
	{
		AIDamageObjectGroup(Planes[planenum].AI.nextpair, damageamount);
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AIDamageObjectGroup(Planes[planenum].AI.wingman, damageamount);
	}

	AIDamagePlane(planenum, damageamount);
}

//**************************************************************************************
void AIAddPlaneGroupToFormation(PlaneParams *planepnt, int planenum)
{
	int worknum;
	long maxz;

	worknum = planenum;

	while(Planes[worknum].AI.nextpair >= 0)
	{
		worknum = Planes[worknum].AI.nextpair;
	}

	maxz = AIGetFormationMaxZ(planenum, 0);

	planepnt->AI.FormationPosition.SetValues(0.0f,0.0f,0.0f);

	AIAddToFormPos(planepnt, maxz + 2000);

	Planes[worknum].AI.nextpair = planepnt - Planes;
	planepnt->AI.prevpair = worknum;

	planepnt->AI.CurrWay = Planes[planenum].AI.CurrWay;
	planepnt->AI.startwpts = Planes[planenum].AI.startwpts;
	planepnt->AI.numwaypts = Planes[planenum].AI.numwaypts;

	planepnt->AI.Behaviorfunc = AIFlyFormation;
}

//**************************************************************************************
long AIGetFormationMaxZ(int planenum, long currmax)
{
	long workingmax;

	if(Planes[planenum].AI.FormationPosition.Z > currmax)
	{
		workingmax = Planes[planenum].AI.FormationPosition.Z;
	}
	else
	{
		workingmax = currmax;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		workingmax = AIGetFormationMaxZ(Planes[planenum].AI.wingman, workingmax);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		workingmax = AIGetFormationMaxZ(Planes[planenum].AI.nextpair, workingmax);
	}

	return(workingmax);
}

//**************************************************************************************
long AIGetFormationMinZ(int planenum, long currmin)
{
	long workingmin;

	if(Planes[planenum].AI.FormationPosition.Z < currmin)
	{
		workingmin = Planes[planenum].AI.FormationPosition.Z;
	}
	else
	{
		workingmin = currmin;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		workingmin = AIGetFormationMaxZ(Planes[planenum].AI.wingman, workingmin);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		workingmin = AIGetFormationMaxZ(Planes[planenum].AI.nextpair, workingmin);
	}

	return(workingmin);
}

//**************************************************************************************
void AIAddToFormPos(PlaneParams *planepnt, long addamount)
{
	planepnt->AI.FormationPosition.Z = planepnt->AI.FormationPosition.Z + addamount;

	if(planepnt->AI.wingman >= 0)
	{
		AIAddToFormPos(&Planes[planepnt->AI.wingman], addamount);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIAddToFormPos(&Planes[planepnt->AI.nextpair], addamount);
	}
}

//**************************************************************************************
void AIReleasePlaneGroupFromFormation(PlaneParams *planepnt, int planenum, int newwaynum)
{
	int worknum;
	int cnt;
	int waypointset = 0;
	MBWayPoints *lookway;
	int waycnt;
	int workwaynum = -1;
	int startact, endact, actcnt;
	int numwpts;
	FormOn	*pFormOnAction;
	Escort	*pEscortAction;
	int pathnum = -1;


	if((planepnt < Planes) || (planepnt > LastPlane))
		return;

	if((planenum < 0) || (planenum > (LastPlane - Planes)))
		return;

	worknum = planepnt - Planes;

	while(Planes[worknum].AI.nextpair >= 0)
	{
		if(Planes[worknum].AI.nextpair == planenum)
		{
			break;
		}
		else
		{
			worknum = Planes[worknum].AI.nextpair;
		}
	}

	if(Planes[worknum].AI.nextpair != planenum)
		return;

	AIAddToFormPos(&Planes[planenum], -Planes[planenum].AI.FormationPosition.Z);

	Planes[worknum].AI.nextpair = -1;
	Planes[planenum].AI.prevpair = -1;

	AIUpdateGroupFlags(planenum, 0, 0, 1, AI_FORMON_SEAD_SEARCH);

	if(Planes[planenum].AI.OrgBehave == NULL)
	{
		Planes[planenum].AI.Behaviorfunc = AIFlyFormation;
	}
	else
	{
		Planes[planenum].AI.OrgBehave = AIFlyFormation;
	}

	for(cnt = 0; cnt < iNumWPPaths; cnt ++)
	{
		if((AIWPPaths[cnt].lObjLink == planenum) && (AIWPPaths[cnt].iStartWP <= newwaynum) && ((AIWPPaths[cnt].iStartWP + AIWPPaths[cnt].iNumWP) >= newwaynum))
		{
//			if((AIWPPaths[cnt].iStartWP + AIWPPaths[cnt].iNumWP) >= newwaynum)
//			{
//				newwaynum = AIWPPaths[cnt].iStartWP;
//			}
			Planes[planenum].AI.CurrWay = &AIWayPoints[newwaynum];
			Planes[planenum].AI.startwpts = AIWPPaths[cnt].iStartWP;
			Planes[planenum].AI.numwaypts = AIWPPaths[cnt].iNumWP - (newwaynum - Planes[planenum].AI.startwpts);
			waypointset = 1;
		}
		else if(AIWPPaths[cnt].lObjLink == planenum)
		{
			pathnum = cnt;
		}
	}

	if(!waypointset)
	{
		if(pathnum == -1)
		{
			lookway = &AIWayPoints[Planes[planenum].AI.startwpts];
			numwpts = Planes[planenum].AI.numwaypts + (Planes[planenum].AI.CurrWay - &AIWayPoints[Planes[planenum].AI.startwpts]);
		}
		else
		{
			lookway = &AIWayPoints[AIWPPaths[pathnum].iStartWP];
			numwpts = AIWPPaths[pathnum].iNumWP;
		}

		for(waycnt = 0; waycnt < numwpts; waycnt ++)
		{
			startact = lookway->iStartAct;
			endact = startact + lookway->iNumActs;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				if((AIActions[actcnt].ActionID == ACTION_ESCORT))
				{
					pEscortAction = (Escort *)AIActions[actcnt].pAction;
					if(pEscortAction->iPlaneNum == (planepnt - Planes))
					{
						if(pEscortAction->iWayPointNum < newwaynum)
						{
							workwaynum = lookway - AIWayPoints;
						}
					}
				}
				if((AIActions[actcnt].ActionID == ACTION_FORMON))
				{
					pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
					if(pFormOnAction->iPlaneNum == (planepnt - Planes))
					{
						if(pFormOnAction->iWayPointNum < newwaynum)
						{
							workwaynum = lookway - AIWayPoints;
						}
					}
				}
			}
			lookway ++;
		}
		if(workwaynum < 0)
		{
			AINextWayPointNoActions(&Planes[planenum]);
		}
		else
		{
			Planes[planenum].AI.CurrWay = &AIWayPoints[workwaynum];
			Planes[planenum].AI.startwpts = AIWPPaths[pathnum].iStartWP;
			Planes[planenum].AI.numwaypts = numwpts - (workwaynum - Planes[planenum].AI.startwpts);
			AINextWayPointNoActions(&Planes[planenum]);
			waypointset = 1;
		}
	}

	AISetUpNewWayPoint(&Planes[planenum]);
}

//**************************************************************************************
void AIReleaseEscort(PlaneParams *planepnt, int planenum, int newwaynum)
{
	int worknum;
	int cnt;
	int planecnt = 0;
	int waypointset = 0;
	MBWayPoints *lookway;
	int waycnt;
	int workwaynum = -1;
	int startact, endact, actcnt;
	int numwpts;
	FormOn	*pFormOnAction;
	Escort	*pEscortAction;

	if((planepnt < Planes) || (planepnt > LastPlane))
		return;

	if((planenum < 0) || (planenum > (LastPlane - Planes)))
		return;

	worknum = planepnt - Planes;
	planepnt->AI.iAIFlags2 &= ~(AI_SEAD_COVER);

	while(Planes[worknum].AI.nextpair >= 0)
	{
		if(Planes[worknum].AI.nextpair == planenum)
		{
			break;
		}
		else
		{
			worknum = Planes[worknum].AI.nextpair;
		}
	}

	if(Planes[worknum].AI.nextpair == planenum)
	{
		AIAddToFormPos(&Planes[planenum], -Planes[planenum].AI.FormationPosition.Z);

		Planes[worknum].AI.nextpair = -1;
		Planes[planenum].AI.prevpair = -1;
	}

	Planes[planenum].AI.Behaviorfunc = AIFlyFormation;

	for(cnt = 0; cnt < iNumWPPaths; cnt ++)
	{
		if((AIWPPaths[cnt].lObjLink == planenum) && (AIWPPaths[cnt].iStartWP <= newwaynum) && ((AIWPPaths[cnt].iStartWP + AIWPPaths[cnt].iNumWP) >= newwaynum))
		{
//			if((AIWPPaths[cnt].iStartWP + AIWPPaths[cnt].iNumWP) >= newwaynum)
//			{
//				newwaynum = AIWPPaths[cnt].iStartWP;
//			}
			Planes[planenum].AI.CurrWay = &AIWayPoints[newwaynum];
			Planes[planenum].AI.startwpts = AIWPPaths[cnt].iStartWP;
			Planes[planenum].AI.numwaypts = AIWPPaths[cnt].iNumWP - (newwaynum - Planes[planenum].AI.startwpts);
			waypointset = 1;
		}
	}

	if(!waypointset)
	{
		lookway = &AIWayPoints[Planes[planenum].AI.startwpts];

		numwpts = Planes[planenum].AI.numwaypts + (Planes[planenum].AI.CurrWay - &AIWayPoints[Planes[planenum].AI.startwpts]);
		for(waycnt = 0; waycnt < numwpts; waycnt ++)
		{
			startact = lookway->iStartAct;
			endact = startact + lookway->iNumActs;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				if((AIActions[actcnt].ActionID == ACTION_ESCORT))
				{
					pEscortAction = (Escort *)AIActions[actcnt].pAction;
					if(pEscortAction->iPlaneNum == (planepnt - Planes))
					{
						if(pEscortAction->iWayPointNum < newwaynum)
						{
							workwaynum = lookway - AIWayPoints;
						}
					}
				}
				if((AIActions[actcnt].ActionID == ACTION_FORMON))
				{
					pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
					if(pFormOnAction->iPlaneNum == (planepnt - Planes))
					{
						if(pFormOnAction->iWayPointNum < newwaynum)
						{
							workwaynum = lookway - AIWayPoints;
						}
					}
				}
			}
			lookway ++;
		}
		if(workwaynum < 0)
		{
			AINextWayPointNoActions(&Planes[planenum]);
		}
		else
		{
			Planes[planenum].AI.CurrWay = &AIWayPoints[workwaynum];
			Planes[planenum].AI.numwaypts = numwpts - (workwaynum - Planes[planenum].AI.startwpts);
			AINextWayPointNoActions(&Planes[planenum]);
			waypointset = 1;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AIReleaseEscort2(Planes[planenum].AI.wingman, planenum);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AIReleaseEscort2(Planes[planenum].AI.nextpair, planenum);
	}

	AIUpdateCruiseFormation(AIGetLeader(&Planes[planenum]), &planecnt);

	AISetUpNewWayPoint(&Planes[planenum]);
}

//**************************************************************************************
void AIReleaseEscort2(int planenum, int leadnum)
{
	Planes[planenum].AI.CurrWay = Planes[leadnum].AI.CurrWay;
	Planes[planenum].AI.startwpts = Planes[leadnum].AI.startwpts;
	Planes[planenum].AI.numwaypts = Planes[leadnum].AI.numwaypts;

	if(Planes[planenum].AI.Behaviorfunc == AIFlyEscort)
	{
		Planes[planenum].AI.Behaviorfunc = AIFlyFormation;
	}
	else if(Planes[planenum].AI.OrgBehave == AIFlyEscort)
	{
		Planes[planenum].AI.OrgBehave = AIFlyFormation;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AIReleaseEscort2(Planes[planenum].AI.wingman, leadnum);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AIReleaseEscort2(Planes[planenum].AI.nextpair, leadnum);
	}
}

//**************************************************************************************
void AISetUpNewWayPoint(PlaneParams *planepnt)
{
	MBWayPoints *waypnt, *orgwaypnt;
	int numwaypts;
	PlaneParams *leadplane;

	leadplane = AIGetLeader(planepnt);

	orgwaypnt = waypnt = planepnt->AI.CurrWay;
	numwaypts = planepnt->AI.numwaypts;

	planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
	planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(waypnt->iSpeed)
	{
		planepnt->AI.lDesiredSpeed = waypnt->iSpeed;
	}

	if(planepnt->AI.lDesiredSpeed <= 100)
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			planepnt->AI.lDesiredSpeed = 350;
		}
	}

	if(waypnt->lWPy > 0)
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
	}
	else if(AIIsTakeOff(waypnt))
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(8000);
	}
	else
	{
		if (!DoingSimSetup)
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		else
		{
			OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
			OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
			planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}
	}

	if((leadplane == planepnt) && (leadplane->Status & AL_AI_DRIVEN))
	{
		AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);

		if(planepnt->AI.wingman >= 0)
		{
			AIUpdateWaypointBehavior(planepnt->AI.wingman, orgwaypnt, numwaypts);
		}
		if(planepnt->AI.nextpair >= 0)
		{
			AIUpdateWaypointBehavior(planepnt->AI.nextpair, orgwaypnt, numwaypts);
		}
	}
	else if(leadplane->Status & PL_AI_DRIVEN)
	{
		AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);
	}
}

//**************************************************************************************
void AIUpdatePlanesInActions(int planenum, int replacenum, int leadnum)
{
	int actcnt;
	FormOn *pFormOnAction;
	Escort *pEscortAction;

	for(actcnt = 0; actcnt < iNumActions; actcnt ++)
	{
		switch (AIActions[actcnt].ActionID)
		{
			case ACTION_AWACS_PATTERN:
			break;
			case ACTION_BOMB_TARGET:
			break;
			case ACTION_CAP:
			break;
			case ACTION_ESCORT:
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				if(pEscortAction->iPlaneNum == planenum)
				{
					pEscortAction->iPlaneNum = replacenum;
				}
			break;
			case ACTION_DAMAGE_THIS_OBJECT:
			break;
			case ACTION_FORMON:
				pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
				if(pFormOnAction->iPlaneNum == planenum)
				{
					pFormOnAction->iPlaneNum = replacenum;
				}
			break;
			case ACTION_HOVER:
			break;
			case ACTION_LAND:
			break;
			case ACTION_MESSAGE:
			break;
			case ACTION_CHANGE_FORMATION:
			break;
			case ACTION_ALTERNATE_PATH:
			break;
			case ACTION_REFUEL_PATTERN:
			break;
			case ACTION_RELEASE_ESCORT:
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				if(pEscortAction->iPlaneNum == planenum)
				{
					pEscortAction->iPlaneNum = replacenum;
				}
			break;
			case ACTION_RELEASE_FORMON:
				pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
				if(pFormOnAction->iPlaneNum == planenum)
				{
					pFormOnAction->iPlaneNum = replacenum;
				}
			break;
			case ACTION_TAKEOFF:
			break;
			case ACTION_SEAD:
			break;
			case ACTION_ALERT_INTERCEPT:
			break;
			case ACTION_CAS:
			break;
			case ACTION_FIGHTER_SWEEP:
			case ACTION_ORBIT:
			break;
		}
	}
}

//**************************************************************************************
void AIFlyEscort(PlaneParams *planepnt)
{
	int numescorting, escortnum;
	float minx, miny, minz, maxx, maxy, maxz;
	PlaneParams *leadprotect;
	FPoint escortpos;
	float zdiff, zinc;
	PlaneParams *leadplane;
	int numsead;
	float tdist;
	PlaneParams *target;
	PlaneParams *escortlead;
	float engagenm = (planepnt->AI.iAICombatFlags2 & AI_ESCORT_STAY) ? 20.0f : 30.0f;

	planepnt->AI.iAICombatFlags1 &= ~(AI_ESCORT_PART_LEAD);
	planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_BUSY);

	leadprotect = planepnt->AI.LinkedPlane;

	if(!leadprotect)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		return;
	}

	escortlead = AIGetEscortLead(planepnt);

	if(escortlead == planepnt)
	{
		if(planepnt->AI.AirTarget)
		{
			if(!(planepnt->AI.AirTarget->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			{
				target = planepnt->AI.AirTarget;
				tdist = QuickDistance((target->WorldPosition.X - planepnt->WorldPosition.X), (target->WorldPosition.Z - planepnt->WorldPosition.Z));
				if(tdist < (engagenm * NMTOWU))
				{
					AIUpdateInterceptGroup(planepnt, target, 1);
					return;
				}
			}
		}
		if((planepnt->AI.AirThreat) && (planepnt->AI.AirThreat != planepnt->AI.AirTarget))
		{
			if(!(planepnt->AI.AirThreat->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			{
				target = planepnt->AI.AirThreat;
				tdist = QuickDistance((target->WorldPosition.X - planepnt->WorldPosition.X), (target->WorldPosition.Z - planepnt->WorldPosition.Z));
				if(tdist < (engagenm * NMTOWU))
				{
					AIUpdateInterceptGroup(planepnt, target, 1);
					return;
				}
			}
		}
	}

	numescorting = AIGetNumEscorting(planepnt, leadprotect, &escortnum, &numsead);

	minx = miny = minz = maxx = maxy = maxz = 0;

	leadplane = AIGetLeader(planepnt);

	if(leadplane != leadprotect)
	{
		AIGetFormationMinsAndMaxs(leadprotect, &minx, &miny, &minz, &maxx, &maxy, &maxz);
		if((leadprotect == PlayerPlane) && (leadplane == planepnt))
		{
			if(planepnt->AI.lHumanTimer < 0)
			{
				planepnt->AI.lHumanTimer = 60000;
				if(AICheckAutoPlayerRelease(planepnt))
				{
					if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
					{
						AICPlayerSEADCoverDone(planepnt - Planes, PlayerPlane - Planes);
					}
					else
					{
						AICPlayerCoverDone(planepnt - Planes, PlayerPlane - Planes);
					}
				}
			}
			else
			{
				planepnt->AI.lHumanTimer = planepnt->AI.lHumanTimer - DeltaTicks;
			}
		}
	}

	if(leadplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_ESCORT_STAY);
	}
	else
	{
		if(planepnt->AI.iAICombatFlags2 & (AI_ESCORT_STAY))
		{
			planepnt->AI.pGroundTarget = NULL;
		}
		planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
	}

	if(leadplane->AI.iAICombatFlags2 & (AI_ESCORT_CAS))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_ESCORT_CAS);
	}
	else
	{
		if(planepnt->AI.iAICombatFlags2 & (AI_ESCORT_CAS))
		{
			planepnt->AI.pGroundTarget = NULL;
		}
		planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
	}

	if(leadplane->AI.iAIFlags2 & AI_SEAD_COVER)
	{
		planepnt->AI.iAIFlags2 |= AI_SEAD_COVER;
	}
	else
	{
		if(planepnt->AI.iAIFlags2 & (AI_SEAD_COVER))
		{
			planepnt->AI.pGroundTarget = NULL;
		}
		planepnt->AI.iAIFlags2 &= ~(AI_SEAD_COVER);
	}

	if(leadprotect->AI.iAIFlags2 & AILANDING)
	{
		if(!((leadprotect->AI.Behaviorfunc == CTWaitingForLaunch) || (leadprotect->AI.Behaviorfunc == CTDoCarrierLaunch)))
		{
			AICoveredPlaneLanding(planepnt, leadprotect);
			return;
		}
	}

	if((planepnt->AI.iAIFlags2 & AI_SEAD_COVER) || (planepnt->AI.iAICombatFlags2 & AI_ESCORT_CAS))
	{
		if(escortnum & 1)
		{
			escortpos.X = maxx + 12152;
		}
		else
		{
			escortpos.X = minx - 12152;
		}

		escortpos.Y = (100 * escortnum);

		if(numsead < 3)
		{
			escortpos.Z = minz - 10152;
		}
		else
		{
			zdiff = maxx - minx;
			zdiff += 20304;

			zinc = zdiff / ((numsead - 1) / 2);
			if(zinc < 500)
			{
				zinc = 500;
			}
			escortpos.Z = maxz + 10152;
			escortpos.Z -= (escortnum / 2) * zinc;
		}
	}
	else
	{
		if(escortnum & 1)
		{
			escortpos.X = maxx + NMTOFT;
		}
		else
		{
			escortpos.X = minx - NMTOFT;
		}

		escortpos.Y = 2000 + (100 * escortnum);

		if(numescorting < 3)
		{
			escortpos.Z = minz - 12152;
		}
		else
		{
			zdiff = maxx - minx;
			zdiff += 24304;

			zinc = zdiff / ((numescorting - 1) / 2);
			if(zinc < 500)
			{
				zinc = 500;
			}
			escortpos.Z = maxz + 12152;
			escortpos.Z -= (escortnum / 2) * zinc;
		}
	}

	AISetEscortPos(planepnt, leadprotect, escortpos);

	AIFormationFlyingLead(planepnt, leadprotect);
	planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;

	if((escortnum == 0) && (!(leadprotect->Status & PL_AI_DRIVEN)))
	{
		AICheckHumanBombWaypoint(leadprotect, planepnt);
	}
}

//**************************************************************************************
int AIGetNumEscorting(PlaneParams *planepnt, PlaneParams *leadprotect, int *escortpos, int *numsead, int *numcas, int *seadstay)
{
	int numescorting = 0;
	int seadescorting = 0;
	int casescorting = 0;
	PlaneParams *checkplane;

	checkplane = &Planes[0];
	*escortpos = 0;
	*numsead = 0;
	if(numcas)
	{
		*numcas = 0;
	}
	if(seadstay)
	{
		*seadstay = 0;
	}

	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == leadprotect))
			{
				if((checkplane->AI.iAIFlags2 & AI_SEAD_COVER))
				{
					if((checkplane->AI.iAICombatFlags2 & AI_ESCORT_STAY) && (seadstay))
					{
						*seadstay = *seadstay + 1;
					}

					if(checkplane == planepnt)
					{
						*escortpos = seadescorting + casescorting;
					}
					seadescorting ++;
				}
				else if((checkplane->AI.iAICombatFlags2 & AI_ESCORT_CAS))
				{
					if(checkplane == planepnt)
					{
						*escortpos = seadescorting + casescorting;
					}
					if(numcas)
					{
						casescorting ++;
					}
					else
					{
						seadescorting ++;
					}
				}
				else
				{
					if(checkplane == planepnt)
					{
						*escortpos = numescorting;
					}
					numescorting ++;
				}
			}
		}
		checkplane ++;
	}
	*numsead = seadescorting;
	if(numcas)
	{
		*numcas = casescorting;
	}
	return(numescorting);
}

//**************************************************************************************
void AIGetFormationMinsAndMaxs(PlaneParams *planepnt, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz)
{
	if(planepnt->AI.FormationPosition.X < *minx)
	{
		*minx = planepnt->AI.FormationPosition.X;
	}
	if(planepnt->AI.FormationPosition.Y < *miny)
	{
		*miny = planepnt->AI.FormationPosition.Y;
	}
	if(planepnt->AI.FormationPosition.Z < *minz)
	{
		*minz = planepnt->AI.FormationPosition.Z;
	}
	if(planepnt->AI.FormationPosition.X > *maxx)
	{
		*maxx = planepnt->AI.FormationPosition.X;
	}
	if(planepnt->AI.FormationPosition.Y > *maxy)
	{
		*maxy = planepnt->AI.FormationPosition.Y;
	}
	if(planepnt->AI.FormationPosition.Z > *maxz)
	{
		*maxz = planepnt->AI.FormationPosition.Z;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIGetFormationMinsAndMaxs(&Planes[planepnt->AI.wingman], minx, miny, minz, maxx, maxy, maxz);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIGetFormationMinsAndMaxs(&Planes[planepnt->AI.nextpair], minx, miny, minz, maxx, maxy, maxz);
	}
}

//**************************************************************************************
void AIUpdatePlaneLinks(PlaneParams *planepnt, PlaneParams *replaceplane)
{
	PlaneParams *checkplane;

	checkplane = &Planes[0];

	while (checkplane <= LastPlane)
	{
		if(checkplane->AI.LinkedPlane == planepnt)
		{
			checkplane->AI.LinkedPlane = replaceplane;

			if(checkplane == replaceplane)
			{
				checkplane->AI.LinkedPlane = NULL;
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
void AISetEscortPos(PlaneParams *planepnt, PlaneParams *escorted, FPoint escortpos)
{
	double angzx, angzz, heading;

	heading = DegToRad((double)escorted->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);
	escorted->AI.WorldFormationPos.SetValues(angzx,(double)escorted->WorldPosition.Y,angzz);

	planepnt->AI.FormationPosition = escortpos;
	planepnt->AI.WorldFormationPos.X = escorted->WorldPosition.X + ConvertWayLoc((angzz * escortpos.X) + (angzx * escortpos.Z));
	planepnt->AI.WorldFormationPos.Y = escorted->WorldPosition.Y + ConvertWayLoc(escortpos.Y);
	planepnt->AI.WorldFormationPos.Z = escorted->WorldPosition.Z + ConvertWayLoc(-(angzx * escortpos.X) + (angzz * escortpos.Z));
}

//**************************************************************************************
void AISetUpEscort(PlaneParams *planepnt, int escortplane)
{
	if((escortplane < 0))
	{
		return;
	}
	if(!((Planes[escortplane].Status & PL_ACTIVE) && (!(Planes[escortplane].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))))
	{
		return;
	}

//	Planes[escortplane].AI.LinkedPlane = planepnt; //  Don't think this is needed, and it screws up refueling with escorts for the refueler
	AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToEscortMeeting);
	AIUpdateGroupPlaneLink(planepnt - Planes, AIGetLeader(&Planes[escortplane]));

	if(Planes[escortplane].Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN))
	{
		planepnt->AI.iAICombatFlags2 |= (AI_ESCORT_STAY);
	}
}

//**************************************************************************************
void AIUpdateGroupPlaneLink(int planenum, PlaneParams *linkedplane)
{
	Planes[planenum].AI.LinkedPlane = linkedplane;

	if(Planes[planenum].AI.wingman >= 0)
	{
		AIUpdateGroupPlaneLink(Planes[planenum].AI.wingman, linkedplane);
	}
	if(Planes[planenum].AI.nextpair >= 0)
	{
		AIUpdateGroupPlaneLink(Planes[planenum].AI.nextpair, linkedplane);
	}
}

//**************************************************************************************
void AIFlyToEscortMeeting(PlaneParams *planepnt)
{
	float tdist;
	float desiredspeed;
	int escortOK = 1;
	PlaneParams *linkedplane;
	FPointDouble linkedpos;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		return;
	}

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		linkedplane = planepnt->AI.LinkedPlane;

		linkedpos = planepnt->AI.LinkedPlane->WorldPosition;

		if(linkedpos.Y < (9000.0f * FTTOWU))
		{
			linkedpos.Y = 10000.0f * FTTOWU;
		}
		else
		{
			linkedpos.Y -= 1000.0f * FTTOWU;
		}

		tdist = FlyToPoint(planepnt, linkedpos);
		desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, planepnt->AI.LinkedPlane->V, AIDESIREDSPDCALC) + 100;

		if(desiredspeed < 200)
			desiredspeed = 200;

		MBAdjustAIThrust(planepnt, desiredspeed, 1);

		AIUpdateFormation(planepnt);

		if(linkedplane->OnGround)
		{
			escortOK = 0;
		}
		else
		{
			if(linkedplane->Status & PL_AI_DRIVEN)
			{
				if((linkedplane->AI.Behaviorfunc == CTWaitingForLaunch) || (linkedplane->AI.Behaviorfunc == CTDoCarrierLaunch))
				{
					escortOK = 0;
				}
			}
			else
			{
				if(linkedplane == PlayerPlane)
				{
					if(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)
					{
						if((PlayerPlane->AI.lVar2 >= 100) || ((PlayerPlane->Status & PL_AI_DRIVEN) && ((PlayerPlane->AI.Behaviorfunc == CTDoCarrierLaunch) || (PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch))))
						{
							escortOK = 0;
						}
					}
				}
			}
		}

		if(((tdist * WUTOFT) < 12152) && (escortOK))
		{
			if(!(planepnt->AI.LinkedPlane->Status & PL_AI_DRIVEN))
			{
				AIC_Beginning_Human_Escort_Msg(planepnt - Planes, planepnt->AI.LinkedPlane - Planes);
			}
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyEscort);

			if(AIIsSEADPlane(planepnt))
			{
				planepnt->AI.iAIFlags2 |= AI_SEAD_COVER;
			}
		}
	}
	else
	{
		if(AIIsSEADPlane(planepnt))
		{
			planepnt->AI.iAIFlags2 |= AI_SEAD_COVER;
		}
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AICheckEscortRelease(PlaneParams *planepnt)
{
	PlaneParams *checkplane;
	int numescorting, escortnum;
	PlaneParams *escortlead;
	long orbitradius;
	long desiredspeed;
	float degspersec, fworkvar;
	ANGLE desiredroll;
	float orbitalt = 20000;
	float seadalt = 20000;
	int numsead;

	if((planepnt < Planes) || (planepnt > LastPlane))
		return;

	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		return;
	}

	checkplane = &Planes[0];
	escortlead = NULL;
	numescorting = -1;

	while(checkplane <= LastPlane)
	{
		if((checkplane->AI.LinkedPlane == planepnt) && (checkplane->AI.Behaviorfunc == AIFlyEscort))
		{
			checkplane->AI.iAICombatFlags2 |= AI_ESCORT_BUSY;
			if(AIIsSEADPlane(checkplane))
			{
				checkplane->AI.iAIFlags2 |= AI_SEAD_COVER;
			}
			if(checkplane->AI.iAIFlags2 & AI_SEAD_COVER)
			{
				checkplane->AI.Behaviorfunc = AIFlyToCASArea;
				checkplane->AI.iVar1 = 20;
				checkplane->AI.iAIFlags2 |= (AI_CAS_ATTACK|AI_SEAD_ATTACK);
				checkplane->AI.pGroundTarget = NULL;
				checkplane->AI.WayPosition = planepnt->AI.WayPosition;
				checkplane->AI.WayPosition.Y = seadalt * FTTOWU;
				seadalt += 1000;
				desiredspeed = AIConvertFtPSToSpeed(orbitalt, planepnt->V, AIDESIREDSPDCALC) + 200;
				if(desiredspeed < 600)
				{
					desiredspeed = 600;
				}
				checkplane->AI.lDesiredSpeed = desiredspeed;
				checkplane ++;
				continue;
			}
			else if(numescorting == -1)
			{
				numescorting = AIGetNumEscorting(checkplane, planepnt, &escortnum, &numsead);
				escortlead = checkplane;
//				Planes[objcnt].AI.iVar1 = pOrbitAction->iUntil;
				orbitradius = 15 * NMTOFT;
				desiredspeed = AIConvertFtPSToSpeed(orbitalt, planepnt->V, AIDESIREDSPDCALC) + 200;
				if(desiredspeed < 600)
				{
					desiredspeed = 600;
				}
				degspersec = AIGetTurnRateForOrbit(orbitradius, desiredspeed, orbitalt);
				if(degspersec > checkplane->YawRate)
				{
					degspersec = checkplane->YawRate;
				}
				fworkvar = asin(degspersec / checkplane->YawRate);
				fworkvar = RadToDeg(fworkvar);
				desiredroll = AIConvert180DegreeToAngle(fworkvar);
				if((planepnt == PlayerPlane) && ((planepnt->AI.lAIVoice >= SPCH_CAP1) && (planepnt->AI.lAIVoice <= SPCH_CAP3)))
				{
					AICCoverSweepMsg(checkplane - Planes, planepnt - Planes);
				}
			}
			else
			{
				escortnum ++;
			}
			checkplane->AI.TargetPos = planepnt->AI.WayPosition;
			checkplane->AI.lVar2 = orbitradius;
			checkplane->AI.lDesiredSpeed = desiredspeed;
			if(numescorting <= 3)
			{
				checkplane->AI.Behaviorfunc = AIEscortOrbitCCSet;
				checkplane->AI.DesiredRoll = desiredroll;
				checkplane->AI.lTimer1 = escortnum * 15000;
				checkplane->AI.TargetPos.Y = orbitalt * FTTOWU;
			}
			else
			{
				if(escortnum < (numescorting>>1))
				{
					checkplane->AI.Behaviorfunc = AIEscortOrbitCCSet;
					checkplane->AI.DesiredRoll = desiredroll;
					checkplane->AI.lTimer1 = escortnum * 15000;
					checkplane->AI.TargetPos.Y = orbitalt * FTTOWU;
				}
				else
				{
					if(escortnum == (numescorting>>1))
					{
						escortlead = checkplane;
						desiredspeed = AIConvertFtPSToSpeed(orbitalt + 1000, planepnt->V, AIDESIREDSPDCALC) + 200;
						if(desiredspeed < 600)
						{
							desiredspeed = 600;
						}
						degspersec = AIGetTurnRateForOrbit(orbitradius, desiredspeed, orbitalt + 1000);
						if(degspersec > checkplane->YawRate)
						{
							degspersec = checkplane->YawRate;
						}
						fworkvar = asin(degspersec / checkplane->YawRate);
						fworkvar = RadToDeg(fworkvar);
						desiredroll = AIConvert180DegreeToAngle(fworkvar);
					}
					checkplane->AI.TargetPos.Y = (orbitalt + 1000) * FTTOWU;
					checkplane->AI.lTimer1 = (escortnum - (numescorting>>1)) * 15000;
					checkplane->AI.Behaviorfunc = AIEscortOrbitCSet;
					checkplane->AI.DesiredRoll = -desiredroll;
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
void AICheckEscortRestore(PlaneParams *planepnt)
{
	PlaneParams *checkplane;
	PlaneParams *leadplane;

	leadplane = AIGetLeader(planepnt);
	checkplane = &Planes[0];

	while(checkplane <= LastPlane)
	{
		if((checkplane->AI.LinkedPlane == leadplane) && ((checkplane->AI.Behaviorfunc == AIEscortOrbitCCSet) || (checkplane->AI.Behaviorfunc == AIEscortOrbitCSet) || (checkplane->AI.Behaviorfunc == AIFlyEscortOrbit) || (checkplane->AI.OrgBehave == AIEscortOrbitCCSet) || (checkplane->AI.OrgBehave == AIEscortOrbitCSet) || (checkplane->AI.OrgBehave == AIFlyEscortOrbit)))
		{
			if((checkplane->AI.Behaviorfunc == AIEscortOrbitCCSet) || (checkplane->AI.Behaviorfunc == AIEscortOrbitCSet) || (checkplane->AI.Behaviorfunc == AIFlyEscortOrbit))
			{
				checkplane->AI.Behaviorfunc = AIResumeEscort;
			}
			else
			{
				checkplane->AI.OrgBehave = AIResumeEscort;
			}
		}
		else if ((checkplane->AI.LinkedPlane == leadplane) && (checkplane->AI.iAIFlags2 & AI_SEAD_COVER))
		{
			if((checkplane->AI.Behaviorfunc != AIFlyToCASArea) && (checkplane->AI.Behaviorfunc != AIFlyCAS) && (checkplane->AI.OrgBehave == AIFlyToCASArea) && (checkplane->AI.OrgBehave == AIFlyCAS))
			{
				checkplane->AI.iAIFlags2 |= AI_RETURN_SEAD_COVER;
			}
			else
			{
				checkplane->AI.iAIFlags2 &= ~(AI_CAS_ATTACK|AI_SEAD_ATTACK);
				checkplane->AI.pGroundTarget = NULL;
				if(checkplane->AI.OrgBehave == NULL)
				{
					if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
					{
						checkplane->AI.Behaviorfunc = AIFlyToEscortMeeting;
					}
					else
					{
						checkplane->AI.Behaviorfunc = AIFlyEscort;
					}
				}
				else
				{
					if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
					{
						checkplane->AI.Behaviorfunc = AIFlyToEscortMeeting;
					}
					else
					{
						checkplane->AI.OrgBehave = AIFlyEscort;
					}
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
void AIEscortOrbitCCSet(PlaneParams *planepnt)
{
	planepnt->AI.iAIFlags2 |= AIIGNOREWPY;
	AIFlyToEscortOrbitPoint(planepnt, 1);
}

//**************************************************************************************
void AIEscortOrbitCSet(PlaneParams *planepnt)
{
	planepnt->AI.iAIFlags2 |= AIIGNOREWPY;
	AIFlyToEscortOrbitPoint(planepnt, 2);
}

//**************************************************************************************
void AIFlyToEscortOrbitPoint(PlaneParams *planepnt, int orbittype)
{
	PlaneParams *leadplane;
	int nowinglead = 0;
	int leadnum = -1;
	FPoint orbitpoint;
	float tdist, dx, dy, dz, offangle;

	if(planepnt->AI.lTimer1 > 0)
	{
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
		return;
	}

	leadplane = AIGetEscortLeader(planepnt);

	if(leadplane == planepnt)
	{
		AIGetOrbitOffset(planepnt, &orbitpoint, orbittype);
		orbitpoint.Y = planepnt->AI.TargetPos.Y;
		tdist = FlyToPoint(planepnt, orbitpoint);
//		if(tdist < 1000)
		if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
		{
			planepnt->AI.Behaviorfunc = AIFlyEscortOrbit;
		}
		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

		dx = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
		dy = planepnt->AI.TargetPos.Y - planepnt->WorldPosition.Y;
		dz = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx,dz) * WUTOFT;
//		if(tdist < planepnt->AI.lVar2)
		if(AIInNextRange(planepnt, tdist,  planepnt->AI.lVar2, 25.0f))
		{
			offangle = AIComputeHeadingToPoint(planepnt, orbitpoint, &tdist, &dx ,&dy, &dz, 1);
			if(fabs(offangle) > 60)
			{
				planepnt->DesiredRoll = 0;
			}
//				planepnt->AI.Behaviorfunc = AIFlyOrbit;
		}
		AIUpdateEscortFormation(planepnt);
	}
	else
	{
		AIFormationFlyingLead(planepnt, leadplane);
		planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
		if(leadplane->AI.Behaviorfunc == AIFlyEscortOrbit)
		{
			planepnt->AI.Behaviorfunc = AIFlyEscortOrbit;
		}
	}
}

//**************************************************************************************
void AIFlyEscortOrbit(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	int nowinglead = 0;
	float dx, dy, dz, tdist;
	float ftoffset;

	leadplane = AIGetEscortLeader(planepnt);
	ftoffset = planepnt->AI.lVar2;

	if(leadplane == planepnt)
	{
		AIComputeHeadingToPoint(planepnt, planepnt->AI.TargetPos, &tdist, &dx ,&dy, &dz, 0);
		if(planepnt->AI.Behaviorfunc == AIResumeEscort)
		{
			if((tdist * WUTOFT) > (ftoffset * 1.5))
			{
				planepnt->AI.Behaviorfunc = AIFlyEscort;
				planepnt->AI.Behaviorfunc(planepnt);
				return;
			}
		}
		else if((tdist * WUTOFT) >  (ftoffset * 2))
		{
			if(planepnt->AI.DesiredRoll > 0x8000)
			{
				planepnt->AI.Behaviorfunc = AIEscortOrbitCSet;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIEscortOrbitCCSet;
			}
			planepnt->AI.Behaviorfunc(planepnt);
			return;
		}

		planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
		dy = AICheckSafeAlt(planepnt, planepnt->AI.TargetPos.Y) - planepnt->WorldPosition.Y;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
		AIUpdateEscortFormation(planepnt);
	}
	else
	{
		planepnt->AI.iAIFlags2 |= AIIGNOREWPY;
		AIFormationFlyingLead(planepnt, leadplane);
		planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
	}
}

//**************************************************************************************
void AIUpdateEscortFormation(PlaneParams *planepnt)
{
	PlaneParams *checkplane;
	double angzx, angzz;
	double heading;
	float xoffmult, zoffmult;
	int escortplace = 1;

	xoffmult = 1000;
	zoffmult = 0;
	if(planepnt->AI.DesiredRoll > 0x8000)
	{
		xoffmult *= -1;
	}

//	heading = DegToRad(AIConvertAngleTo180Degree(planepnt->Heading));
	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	if(!(planepnt->AI.iAIFlags1 & AIPLAYERGROUP))
	{
		planepnt->AI.FormationPosition.SetValues(0.0f,0.0f,0.0f);
	}

	checkplane = planepnt + 1;

	while(checkplane <= LastPlane)
	{
		if((checkplane->AI.Behaviorfunc == planepnt->AI.Behaviorfunc) && (checkplane->AI.LinkedPlane == planepnt->AI.LinkedPlane) && (checkplane->AI.DesiredRoll == planepnt->AI.DesiredRoll))
		{
			if(checkplane->AI.iAIFlags1 & AIINFORMATION)
			{
				checkplane->AI.FormationPosition.X = xoffmult * escortplace;
				checkplane->AI.FormationPosition.Y = 0;
				checkplane->AI.FormationPosition.Z = zoffmult * escortplace;
				checkplane->AI.WorldFormationPos.X = planepnt->WorldPosition.X + ConvertWayLoc((angzz * xoffmult * escortplace) + (angzx * zoffmult * escortplace));
				checkplane->AI.WorldFormationPos.Y = planepnt->WorldPosition.Y;
				checkplane->AI.WorldFormationPos.Z = planepnt->WorldPosition.Z + ConvertWayLoc(-(angzx * xoffmult * escortplace) + (angzz * zoffmult * escortplace));
				escortplace ++;
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
PlaneParams *AIGetEscortLeader(PlaneParams *planepnt)
{
	PlaneParams *checkplane;

	checkplane = &Planes[0];

	while(checkplane <= LastPlane)
	{
		if((checkplane->AI.Behaviorfunc == planepnt->AI.Behaviorfunc) && (checkplane->AI.LinkedPlane == planepnt->AI.LinkedPlane) && (checkplane->AI.DesiredRoll == planepnt->AI.DesiredRoll))
		{
			return(checkplane);
		}
		checkplane ++;
	}
	return(planepnt);
}

//**************************************************************************************
void AIResumeEscort(PlaneParams *planepnt)
{
	float dx, dy, dz, tdist;

	AIFlyEscortOrbit(planepnt);

	if(planepnt->AI.LinkedPlane != NULL)
	{
		if(30 > fabs(AIComputeHeadingToPoint(planepnt, planepnt->AI.LinkedPlane->WorldPosition, &tdist, &dx ,&dy, &dz, 1)))
		{
			if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
			{
				planepnt->AI.Behaviorfunc = AIFlyToEscortMeeting;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIFlyEscort;
			}
		}
	}
}

//**************************************************************************************
void AICheckHumanBombWaypoint(PlaneParams *humanplane, PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int bombfound = 0;
	int startact, endact, actcnt;
	double offangle;
	float tdist;
	float tdistnm;
	float dx, dy, dz;

	if(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AG)
	{
		return;
	}

	lookway = humanplane->AI.CurrWay;

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
		{
			bombfound ++;
			break;
		}
	}

	if(bombfound)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			if(planepnt->AI.lTimer1 < -10000)
			{
				planepnt->AI.lTimer1 = -1;
				offangle = AIComputeHeadingToPoint(humanplane, humanplane->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

				tdistnm = (tdist * WUTONM);

				if(tdistnm < 70)
				{
					AICheckEscortRelease(humanplane);
				}
			}
			else
			{
				planepnt->AI.lTimer1 -= DeltaTicks;
			}
		}
	}
	else
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			planepnt->AI.lTimer1 = -1;
		}
	}

}

//**************************************************************************************
void AICheckHumanNonBombWaypoint(PlaneParams *humanplane, PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int bombfound = 0;
	int startact, endact, actcnt;

	lookway = humanplane->AI.CurrWay;

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
		{
			bombfound ++;
			break;
		}
	}

	if(!bombfound)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			if(planepnt->AI.lTimer1 < -10000)
			{
				planepnt->AI.lTimer1 = -1;
				AICheckEscortRestore(humanplane);
			}
			else
			{
				planepnt->AI.lTimer1 -= DeltaTicks;
			}
		}
	}
	else
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			planepnt->AI.lTimer1 = -1;
		}
	}

}

//**************************************************************************************
int AIWaitingForTakeOffFlag(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	TakeOffActionType *pActionTakeOff;

	if(planepnt->AI.lTimer2 == -99999)
		return(0);

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_TAKEOFF))
		{
			pActionTakeOff = (TakeOffActionType *)AIActions[actcnt].pAction;
			if((pActionTakeOff->lFlag == 0) || (planepnt->AI.lTimer2 == -99999))
			{
				return(0);
			}
			else if(!AICheckEventFlagStatus(pActionTakeOff->lFlag))
			{
				return(1);
			}
			break;
		}
	}
	return(0);
}

//**************************************************************************************
void AIFlyToHoverPoint(PlaneParams *planepnt)
{
	float tdist;
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	HoverActionType *pHoverAction;
	float offangle;
	float dx, dy, dz;


	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);
		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
//		if(tdist < 250)
		if(AIInNextRange(planepnt, tdist * WUTOFT,   1435, 25.0f))
		{
			lookway = planepnt->AI.CurrWay;

			startact = lookway->iStartAct;
			endact = startact + lookway->iNumActs;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				if((AIActions[actcnt].ActionID == ACTION_HOVER))
				{
					pHoverAction = (HoverActionType *)AIActions[actcnt].pAction;;
					if(pHoverAction->lFlag == 0)
					{
						planepnt->AI.lTimer1 = pHoverAction->lTimer * 60000;
					}
					else if(!AICheckEventFlagStatus(pHoverAction->lFlag))
					{
						planepnt->AI.lVar2 = pHoverAction->lFlag;
						planepnt->AI.lTimer1 = -1;
					}
					break;
				}
			}
			offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 0);
			planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(offangle);
			planepnt->AI.Behaviorfunc = AIFlyHover;
			if(GameLoop < 50)
			{
				planepnt->DesiredSpeed = 0;
			}
		}
		AIUpdateFormation(planepnt);
	}
	else
	{
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AIFlyHover(PlaneParams *planepnt)
{
	float tdist, desiredspeed;
	int donextway;
	PlaneParams *leadplane;
	double offangle;
	float dx, dy, dz;
	double rollval;
	float desiredpitch;
	float heading, angzx, angzz, offangle2, tdist2, dy2;
	FPoint landingpos;


	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		landingpos = planepnt->AI.WayPosition;
		offangle = AIComputeHeadingToPoint(planepnt, landingpos, &tdist, &dx ,&dy, &dz, 1);
		tdist *= WUTOFT;
		dy *= WUTOFT;

		heading = DegToRad((double)planepnt->AI.DesiredHeading / DEGREE);
		angzx = sin(heading);
		angzz = cos(heading);

		landingpos.X = landingpos.X + ConvertWayLoc(angzx * -100);
		landingpos.Z = landingpos.Z + ConvertWayLoc(angzz * -100);

		offangle2 = AIComputeHeadingToPoint(planepnt, landingpos, &tdist2, &dx ,&dy2, &dz, 1);

		AIChangeDir(planepnt, (double)offangle2, dy);

		rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
		if(fabs(rollval) > 25)  //  was 60
		{
			rollval = (rollval < 0.0) ? -25 : 25;
			planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
		}
		planepnt->RollPercentage = 0.40;	//  max roll 90.0

		if((tdist < 10) || (offangle2 > 90))
		{
			if(fabs(dy) > 5)
			{
				desiredpitch = (dy < 0) ? -90 : 90;
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);
				tdist = fabs(dy);
			}
			else
			{
				desiredpitch = 0;
				tdist = 0;
			}
			planepnt->DesiredRoll = 0;
		}
		else if(fabs(dy) > 5)
		{
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(atan2(dy, tdist) * 57.2958);
			if((planepnt->DesiredPitch > 0x4000) && (planepnt->DesiredPitch < 0xC000))
			{
				desiredpitch = (dy < 0) ? -90 : 90;
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);
			}
			if(fabs(dy) > tdist)
				tdist = fabs(dy);
		}

		desiredspeed = tdist / 5;
		if((desiredspeed * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) > 40)
		{
			desiredspeed = 40 / (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
		}
		planepnt->DesiredSpeed = desiredspeed;
		AIUpdateFormation(planepnt);

		donextway = 0;
		if(planepnt->AI.lVar2 == 0)
		{
			if(planepnt->AI.lTimer1 < 0)
			{
				donextway = 1;
			}
		}
		else
		{
			donextway = AICheckEventFlagStatus(planepnt->AI.lVar2);
		}

		if(donextway)
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			AINextWayPoint(planepnt);
		}
	}
	else
	{
		leadplane = AIGetLeader(planepnt);
		planepnt->AI.lTimer1 = leadplane->AI.lTimer1;
		planepnt->AI.lVar2 = leadplane->AI.lVar2;
		AIFormationFlyingLead(planepnt, leadplane);
	}
}

//**************************************************************************************
void AIFlyToLandingPoint(PlaneParams *planepnt)
{
	float tdist;
	double offangle, offangleabs;
	float dx, dy, dz;
	double rollval;
	FPoint landingpos;
	PlaneParams *leadplane;
	float heading, angzz, angzx;

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		landingpos = planepnt->AI.WayPosition;
		offangle = offangleabs = AIComputeHeadingToPoint(planepnt, landingpos, &tdist, &dx ,&dy, &dz, 0);

		offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

		offangle = AICapAngle(offangle);

//		if(tdist < 1000)
		if(AIInNextRange(planepnt, tdist * WUTOFT,  5743))
		{
			landingpos.Y = LandHeight(landingpos.X, landingpos.Z) + planepnt->Type->GearDownHeight + (100 * FTTOWU);
			dy = AICheckSafeAlt(planepnt, landingpos.Y) - planepnt->WorldPosition.Y;
		}
		dy *= WUTOFT;

		AIChangeDir(planepnt, (double)offangle, dy);

		rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
		if(fabs(rollval) > 25)  //  was 60
		{
			rollval = (rollval < 0.0) ? -25 : 25;
			planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
		}
		planepnt->RollPercentage = 0.40;	//  max roll 90.0


		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
//		if(tdist < 250)
		if(AIInNextRange(planepnt, tdist * WUTOFT,  1435))
		{
			planepnt->AI.iAIFlags2 |= AILANDING;
			planepnt->AI.Behaviorfunc = AIFlyHeloLanding;
			planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(offangleabs);

			if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(GEAR, ON, 1);
				}
				RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
			}
			if(!(planepnt->Flaps))
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(FLAPS, ON, 1);
				}
				planepnt->Flaps = 30.0f;
				planepnt->FlapsCommandedPos = 30.0;
			}
		}
		AIUpdateFormation(planepnt);
	}
	else
	{
		leadplane = AIGetLeader(planepnt);
//		if(((leadplane->V < 1.0f) && (leadplane->AI.Behaviorfunc == AIFlyHeloLanding)) || (leadplane->AI.Behaviorfunc == AIHeloSinkToGround))
		if((leadplane->AI.Behaviorfunc == AIFlyHeloLanding) || (leadplane->AI.Behaviorfunc == AIHeloSinkToGround))
		{
			planepnt->AI.Behaviorfunc = AIJustHeloLand;
			planepnt->AI.iAIFlags2 |= AILANDING;

			heading = DegToRad((double)leadplane->AI.DesiredHeading / DEGREE);
			angzx = sin(heading);
			angzz = cos(heading);
			planepnt->AI.WayPosition.X = leadplane->AI.WayPosition.X + ConvertWayLoc((angzz * planepnt->AI.FormationPosition.X) + (angzx * planepnt->AI.FormationPosition.Z));
			planepnt->AI.WayPosition.Y = leadplane->AI.WayPosition.Y;
			planepnt->AI.WayPosition.Z = leadplane->AI.WayPosition.Z + ConvertWayLoc(-(angzx * planepnt->AI.FormationPosition.X) + (angzz * planepnt->AI.FormationPosition.Z));
			planepnt->AI.DesiredHeading = leadplane->AI.DesiredHeading;

			planepnt->AI.Behaviorfunc(planepnt);
			return;
		}
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AIFlyHeloLanding(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	float heading, angzz, angzx;

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		AIJustHeloLand(planepnt);
		AIUpdateFormation(planepnt);
	}
	else
	{
		leadplane = AIGetLeader(planepnt);
		if(leadplane->V < 1.0f)
		{
			planepnt->AI.Behaviorfunc = AIJustHeloLand;

			heading = DegToRad((double)leadplane->AI.DesiredHeading / DEGREE);
			angzx = sin(heading);
			angzz = cos(heading);
			planepnt->AI.WayPosition.X = leadplane->AI.WayPosition.X + ConvertWayLoc((angzz * planepnt->AI.FormationPosition.X) + (angzx * planepnt->AI.FormationPosition.Z));
			planepnt->AI.WayPosition.Y = leadplane->AI.WayPosition.Y;
			planepnt->AI.WayPosition.Z = leadplane->AI.WayPosition.Z + ConvertWayLoc(-(angzx * planepnt->AI.FormationPosition.X) + (angzz * planepnt->AI.FormationPosition.Z));
			planepnt->AI.DesiredHeading = leadplane->AI.DesiredHeading;

			planepnt->AI.Behaviorfunc(planepnt);
			return;
		}
		AIFormationFlyingLead(planepnt, leadplane);
	}
}

//**************************************************************************************
void AIJustHeloLand(PlaneParams *planepnt)
{
	float tdist, desiredspeed, tdist2;
	double offangle, offangle2;
	float dx, dy, dz, dy2;
	double rollval;
	FPoint landingpos;
	float desiredpitch;
	float heading, angzz, angzx;

	landingpos = planepnt->AI.WayPosition;
//	landingpos.Y = LandHeight(landingpos.X, landingpos.Z) + planepnt->Type->GearDownHeight;
	offangle = AIComputeHeadingToPoint(planepnt, landingpos, &tdist, &dx ,&dy, &dz, 1);
	dy =  planepnt->Type->GearDownHeight - planepnt->HeightAboveGround;
	tdist *= WUTOFT;
	dy *= WUTOFT;

	heading = DegToRad((double)planepnt->AI.DesiredHeading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	landingpos.X = landingpos.X + ConvertWayLoc(angzx * -100);
	landingpos.Z = landingpos.Z + ConvertWayLoc(angzz * -100);

	offangle2 = AIComputeHeadingToPoint(planepnt, landingpos, &tdist2, &dx ,&dy2, &dz, 1);

	if(tdist > 30)
	{
		dy += 20;
	}

	AIChangeDir(planepnt, (double)offangle2, dy);

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	if((tdist < 5) || (fabs(offangle) > 90))
	{
		desiredpitch = (dy < 0) ? -90 : 90;
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);
		tdist = fabs(dy);
		planepnt->AI.Behaviorfunc = AIHeloSinkToGround;
	}
	else if(fabs(dy) > tdist)
	{
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(atan2(dy, tdist) * 57.2958);
		tdist = fabs(dy);
	}

	desiredspeed = tdist / 5;
	if((desiredspeed * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) > 40)
	{
		desiredspeed = 40 / (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}
	else if(desiredspeed < 2.0f)
	{
		desiredspeed = 2.0f;
	}
	planepnt->DesiredSpeed = desiredspeed;
}

//**************************************************************************************
void AIHeloSinkToGround(PlaneParams *planepnt)
{
	float tdist, desiredspeed;
	float dy;

	planepnt->DesiredRoll = 0;
	dy =  planepnt->Type->GearDownHeight - planepnt->HeightAboveGround;
	dy *= WUTOFT;
	planepnt->DesiredPitch = (dy < 0) ? 0xC000 : 0x4000;
	tdist = fabs(dy);

//	if((tdist <= 0.5f) || (dy > 0))
	if((tdist <= 0.5f) || (dy > 0) || (planepnt->OnGround))
	{
		planepnt->AI.iAIFlags2 &= ~AILANDING;
		planepnt->OnGround = 1;
		planepnt->AI.Behaviorfunc = AIParked;
		planepnt->WorldPosition.Y = 0;
		planepnt->AI.Behaviorfunc(planepnt);

		PlaneParams *leadplane = AIGetLeader(planepnt);
		if((leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
		{
			AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay);
		}
		return;
	}

	desiredspeed = tdist / 5;
	if((desiredspeed * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) > 40)
	{
		desiredspeed = 40 / (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}
	else if(desiredspeed < 2.0f)
	{
		desiredspeed = 2.0f;
	}
	planepnt->DesiredSpeed = desiredspeed;

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		AIUpdateFormation(planepnt);
	}
}

//**************************************************************************************
void AIParked(PlaneParams *planepnt)
{
	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);

	planepnt->AI.DesiredRoll = 0;
	planepnt->DesiredRoll = 0;
	planepnt->DesiredPitch = 0;
	planepnt->DesiredSpeed = 0;
	planepnt->V = 0;
	planepnt->BfLinVel.X = 0;
	planepnt->BfLinVel.Z = 0;
	planepnt->AI.AOADegPitch = 0;
	planepnt->Alpha = 0;
	if(!planepnt->OnGround)
	{
		planepnt->WorldPosition.Y = 0;
	}
	planepnt->OnGround = 1;

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		AIUpdateFormation(planepnt);
	}

	if(planepnt->AI.lTimer1 < 0)
	{
		if((AIInPlayerGroup(planepnt) || (planepnt->AI.numwaypts < 1) || (planepnt->AI.CurrWay->iNumActs)) && (!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)))
		{
			AIVanishPlane(planepnt);
		}
	}
}

//**************************************************************************************
void AISetUpSweepAction(PlaneParams *planepnt, FighterSweepActionType *pSweepAction)
{
	if(planepnt->AI.Behaviorfunc == AIFlySweep)
	{
		if(planepnt->AI.lVar2)
		{
			planepnt->AI.lVar2 = pSweepAction->lFlag;;
			if(pSweepAction->lFlag == 0)
			{
				planepnt->AI.lTimer2 = pSweepAction->lTimer;
			}
			else
			{
				planepnt->AI.lTimer2 = -1;
			}
		}
	}
	else
	{
		planepnt->AI.lVar2 = pSweepAction->lFlag;;
		if(pSweepAction->lFlag == 0)
		{
			planepnt->AI.lTimer2 = pSweepAction->lTimer;
		}
		else
		{
			planepnt->AI.lTimer2 = -1;
		}
		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySweep);
	}
}

//**************************************************************************************
void AIFlySweep(PlaneParams *planepnt)
{
	AIFlyFormation(planepnt);
}

//**************************************************************************************
void AICheckSweepConditions(PlaneParams *planepnt)
{
	int gotonext = 1;

	if(planepnt->AI.lVar2 == 0)
	{
		if(planepnt->AI.lTimer2 < 0)
		{
			gotonext = 0;
		}
	}
	else if(AICheckSweepFlags(planepnt))
	{
		gotonext = 0;
	}

	if(gotonext)
	{
		AIGetNextSweepWayPt(planepnt);
	}
	else
	{
		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyFormation);
		AIAfterSweepWayPt(planepnt);
	}
}

//**************************************************************************************
int AICheckSweepFlags(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	FighterSweepActionType *pSweepAction;

	lookway = planepnt->AI.CurrWay;

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_FIGHTER_SWEEP))
		{
			pSweepAction = (FighterSweepActionType *)AIActions[actcnt].pAction;
			return(AICheckEventFlagStatus(pSweepAction->lFlag));
		}
	}
	return(1);
}

//**************************************************************************************
void AIGetNextSweepWayPt(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int numwaypts;
	int startact, endact, actcnt;
	int gotoend = 0;
	int done = 0;
	int foundsweep;

	if(planepnt->AI.numwaypts > 1)
	{
		lookway = planepnt->AI.CurrWay + 1;
	}
	else
	{
		lookway = &AIWayPoints[planepnt->AI.startwpts];
	}

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_FIGHTER_SWEEP))
		{
			return;
		}
	}

	lookway = planepnt->AI.CurrWay - 1;
	numwaypts = planepnt->AI.numwaypts + 1;

	while(!done)
	{
		if(lookway < &AIWayPoints[planepnt->AI.startwpts])
		{
			done = 1;
			gotoend = 1;
		}
		else
		{
			startact = lookway->iStartAct;
			endact = startact + lookway->iNumActs;
			foundsweep = 0;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				if((AIActions[actcnt].ActionID == ACTION_FIGHTER_SWEEP))
				{
					foundsweep = 1;
				}
			}
			if(!foundsweep)
			{
				planepnt->AI.CurrWay = lookway;
				planepnt->AI.numwaypts = numwaypts;
				return;
			}
		}
		lookway --;
		numwaypts ++;
	}

	if(gotoend)
	{
		while(planepnt->AI.numwaypts > 1)
		{
			planepnt->AI.CurrWay ++;
			planepnt->AI.numwaypts -= 1;
		}
	}
}

//**************************************************************************************
void AIAfterSweepWayPt(PlaneParams *planepnt)
{
	MBWayPoints *lookway, *lastlook;
	int numwaypts, lastnumwaypts;
	int startact, endact, actcnt;
	int gotoend = 0;
	int done = 0;
	int foundsweep;

	if(planepnt->AI.numwaypts > 1)
	{
		lookway = planepnt->AI.CurrWay + 1;
		numwaypts = planepnt->AI.numwaypts - 1;
	}
	else
	{
		lookway = &AIWayPoints[planepnt->AI.startwpts];
		numwaypts = (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]) + 1;
	}
	lastlook = planepnt->AI.CurrWay;
	lastnumwaypts = planepnt->AI.numwaypts;

	while(lookway != planepnt->AI.CurrWay)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		foundsweep = 0;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_FIGHTER_SWEEP))
			{
				foundsweep = 1;
			}
		}
		if(!foundsweep)
		{
			planepnt->AI.CurrWay = lastlook;
			planepnt->AI.numwaypts = lastnumwaypts;
			return;
		}
		lastlook = lookway;
		lastnumwaypts = numwaypts;
		if(numwaypts > 1)
		{
			lookway ++;
			numwaypts --;
		}
		else
		{
			numwaypts = (lookway - &AIWayPoints[planepnt->AI.startwpts]) + 1;
			lookway = &AIWayPoints[planepnt->AI.startwpts];
		}
	}
}

//**************************************************************************************
void AIGetXZOffset(float offangle, float orgx, float orgz, long xftoff, long zftoff, float *newx, float *newz, PlaneParams *updateplane)
{
	float heading, angzz, angzx;

	heading = DegToRad(offangle);

	angzx = sin(heading);
	angzz = cos(heading);

	if(updateplane != NULL)
	{
		updateplane->AI.WorldFormationPos.X = angzx;
		updateplane->AI.WorldFormationPos.Y = updateplane->WorldPosition.Y;
		updateplane->AI.WorldFormationPos.Z = angzz;
	}

	*newx = orgx + ConvertWayLoc((angzz * xftoff) + (angzx * zftoff));
	*newz = orgz + ConvertWayLoc(-(angzx * xftoff) + (angzz * zftoff));
}

//**************************************************************************************
void AISetPlaneLandingFlyToField(PlaneParams *planepnt, TakeOffActionType *pActionTakeOff, ANGLE runwayheading)
{
	long xftoff = 0;
	long zftoff = 120000;
	PlaneParams *leadplane;
	TakeOffActionType TempActionTakeOff;
	FPointDouble fptemp;
	FPointDouble runwaypos;
	ANGLE runwayheading2;

	planepnt->AI.DesiredPitch = 0;
	planepnt->AI.Behaviorfunc = AIPlaneLandingFlyToField;
	planepnt->AI.OrgBehave = NULL;

	//  Eventually get this off the aActionTakeOff information.
	if(pActionTakeOff == NULL)
	{
		fptemp.SetValues(-1.0f,-1.0f,-1.0f);
		if(AIGetClosestAirField(planepnt, fptemp, &runwaypos, &runwayheading2, &TempActionTakeOff, -1.0f, 0, 0, 0))
		{
			planepnt->AI.DesiredHeading = runwayheading2;  //  in ANGLES
			planepnt->AI.TargetPos.X = TempActionTakeOff.lRunwayStartX * FTTOWU;
			planepnt->AI.TargetPos.Y = TempActionTakeOff.lRunwayStartY * FTTOWU;
			planepnt->AI.TargetPos.Z = TempActionTakeOff.lRunwayStartZ * FTTOWU;
		}
		else
		{
			planepnt->AI.TargetPos.X = ConvertWayLoc(6318441);
			planepnt->AI.TargetPos.Z = ConvertWayLoc(4518604);
		}
	}
	else
	{
		planepnt->AI.DesiredHeading = runwayheading;  //  in ANGLES
		planepnt->AI.TargetPos.X = pActionTakeOff->lRunwayStartX * FTTOWU;
		planepnt->AI.TargetPos.Y = pActionTakeOff->lRunwayStartY * FTTOWU;
		planepnt->AI.TargetPos.Z = pActionTakeOff->lRunwayStartZ * FTTOWU;
	}
	//  planepnt->AI.TargetPos.Y = Landing point from takeoff info.
	//  Planes[planenum].AI.TargetPos.Y = Landing point from takeoff info.

	leadplane = AIGetLeader(planepnt);

	if((leadplane->Status & PL_AI_DRIVEN) && (!AIInPlayerGroup(planepnt)))
	{
		if(leadplane != planepnt)
		{
			planepnt->AI.Behaviorfunc = AIFormationFlyingLinked;
			planepnt->AI.OrgBehave = NULL;
			planepnt->AI.LinkedPlane = leadplane;
			planepnt->AI.Behaviorfunc(planepnt);
			planepnt->AI.WayPosition = leadplane->AI.WayPosition;
			return;
		}
		else
		{
			planepnt->AI.LinkedPlane = NULL;
			AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
			planepnt->AI.WayPosition.Y = AIGetMarshallHeight(planepnt);
		}
	}
	else
	{
		AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
		planepnt->AI.WayPosition.Y = AIGetMarshallHeight(planepnt);
	}
}

//**************************************************************************************
void AIPlaneLandingFlyToField(PlaneParams *planepnt)
{
	float tdist;
	double offangle;
	float dx, dy, dz;
	double rollval;
	PlaneParams *leadplane;
	float degspersec, fworkvar;

	planepnt->AI.DesiredPitch = 0;
	leadplane = AIGetLeader(planepnt);
	if((leadplane->Status & PL_AI_DRIVEN) && (!AIInPlayerGroup(planepnt)))
	{
		if(leadplane != planepnt)
		{
			planepnt->AI.Behaviorfunc = AIFormationFlyingLinked;
			planepnt->AI.LinkedPlane = leadplane;
			planepnt->AI.Behaviorfunc(planepnt);
			return;
		}
	}

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

#if 0
	dy2 = AICheckSafeAlt(planepnt, (planepnt->AI.TargetPos.Y + (2000 * FTTOWU) + (tdist * 2))) - planepnt->WorldPosition.Y;
	if(dy2 < dy)
	{
		dy = dy2;
	}
#endif

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

//	if(tdist < 1000)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743))
	{
		if(AICheckMarshallPattern(planepnt))
		{
			degspersec = AIGetTurnRateForOrbit((2.5f * NMTOFT), 350, planepnt->AI.WayPosition.Y * WUTOFT);
			if(planepnt->YawRate)
			{
				if(degspersec > planepnt->YawRate)
				{
					degspersec = planepnt->YawRate;
				}
				fworkvar = asin(degspersec / planepnt->YawRate);
				fworkvar = RadToDeg(fworkvar);
				planepnt->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
			}
			else
			{
				planepnt->AI.DesiredRoll = 0;
#ifdef _DEBUG
				char errmsg[256];
				sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.lPlaneID);
			    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
			}
			planepnt->AI.Behaviorfunc = AIFlyMarshallPattern;
			planepnt->AI.lTimer2 = 60000;
		}
		else
		{
			AISetEnterLandingPattern(planepnt);
		}
	}
}

//**************************************************************************************
void AISetEnterLandingPattern(PlaneParams *planepnt)
{
	long xftoff = 2000;
	long zftoff = 6000;
	int planecnt = 0;


	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
	{
		AISetUpSlowLanderPattern(planepnt);
		return;
	}

//	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIUpdateLandingFormation(planepnt, &planecnt, planepnt);
		planepnt->AI.Behaviorfunc = AIEnterLandingPattern;
	}
	else
	{
		planepnt->AI.Behaviorfunc = AIFormationFlyingLinked;
	}

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000 * FTTOWU);
}

//**************************************************************************************
void AIUpdateLandingFormation(PlaneParams *planepnt, int *planecnt, PlaneParams *leadplane)
{
	int wingman, nextsec, winglead, prevsec;
	int tval;
	float fplus;
	PlaneParams *purelead;
	int nextpair;

	purelead = AIGetLeader(planepnt);
	if(planepnt != leadplane)
	{
		if((!(purelead->Status & PL_AI_DRIVEN)) || (AIInPlayerGroup(planepnt)))
		{
			if(planepnt->AI.LinkedPlane != leadplane)
			{
				return;
			}
		}
		planepnt->AI.LinkedPlane = leadplane;
	}
	else
	{
		planepnt->AI.LinkedPlane = NULL;
	}

	planepnt->AI.Behaviorfunc = AIFormationFlyingLinked;

	tval = *planecnt;
	fplus = tval * 100.0f;

	planepnt->AI.FormationPosition.SetValues(fplus,0.0f, fplus);

	*planecnt = tval + 1;


	winglead = planepnt->AI.winglead;
	prevsec = planepnt->AI.prevpair;

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AIUpdateLandingFormation(&Planes[wingman], planecnt, leadplane);
	}

	if(nextsec >= 0)
	{
		AIUpdateLandingFormation(&Planes[nextsec], planecnt, leadplane);
	}

	if(planepnt == leadplane)
	{
		if((!(purelead->Status & PL_AI_DRIVEN)) || (AIInPlayerGroup(planepnt)))
		{
			winglead = planepnt->AI.winglead;
			if(winglead >= 0)
			{
				nextpair = Planes[winglead].AI.nextpair;
				if(nextpair >= 0)
				{
					AIUpdateLandingFormation(&Planes[nextpair], planecnt, leadplane);
				}
			}
		}
	}
}

//**************************************************************************************
void AIEnterLandingPattern(PlaneParams *planepnt)
{
	float tdist;
	double offangle;
	float dx, dy, dz;
	double rollval;
	PlaneParams *leadplane;

	planepnt->AI.DesiredPitch = 0;
	leadplane = AIGetLeader(planepnt);
	if((leadplane->Status & PL_AI_DRIVEN) && (!AIInPlayerGroup(planepnt)))
	{
		if(leadplane != planepnt)
		{
			planepnt->AI.Behaviorfunc = AIFormationFlyingLinked;
			planepnt->AI.LinkedPlane = leadplane;
			planepnt->AI.Behaviorfunc(planepnt);
			return;
		}
	}

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, 300, 1);

//	if(tdist < 1000)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743))
	{
		AISetLandingApproachBreak(planepnt);
//		AIAllowNextLander(planepnt);
	}
}

//**************************************************************************************
void AISetLandingApproachBreak(PlaneParams *planepnt)
{
	long xftoff = 2000;
	long zftoff = -12000;

	planepnt->AI.Behaviorfunc = AILandingApproachBreak;
	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000 * FTTOWU);
	if(planepnt == (PlaneParams *)PlayerPlane)
	{
		DisplayWarning(FLAPS, ON, 1);
	}
	planepnt->AI.lVar2 = xftoff;	//  New
	planepnt->Flaps = 30.0f;
	planepnt->FlapsCommandedPos = 30.0;
}

//**************************************************************************************
void AILandingApproachBreak(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;

	planepnt->AI.DesiredPitch = 0;
	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, 300, 1);

	tdistft = tdist * WUTOFT;
	if((tdistft < 20) || ((fabs(offangle) > 30) && (AIInNextRange(planepnt, tdistft,  3000))))
	{
		AISetLandingLevelBreak(planepnt);
	}
}

//**************************************************************************************
void AISetLandingLevelBreak(PlaneParams *planepnt)
{
	long xftoff = -4000;
	long zftoff = -3000;
	float turnradft;

	turnradft = AIGetTurnRadFt(planepnt) * 2;

	if(xftoff > -turnradft)
	{
		xftoff = -turnradft;
	}
	if(zftoff > -turnradft)
	{
		zftoff = -turnradft;
	}

//	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	if(planepnt->AI.LinkedPlane == NULL)
	{
		AISetUpLandingTiming(planepnt);
	}

	xftoff -= planepnt->AI.FormationPosition.X;
	planepnt->AI.lVar2 += xftoff;  //  New
	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingLevelBreak;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000 * FTTOWU);
}

//**************************************************************************************
void AISetUpLandingTiming(PlaneParams *planepnt)
{
	int planecnt = 1;
	int dowingpairs = 1;
	PlaneParams *leadplane;
	int winglead, nextpair;
	int dospacing = 0;

	long xftoff = 2000;
	long zftoff = -12000;

	if(planepnt->AI.wingman >= 0)
	{
		AISetUpLandingTimingPlane(&Planes[planepnt->AI.wingman], &planecnt, planepnt, dowingpairs);
		dospacing = 1;
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AISetUpLandingTimingPlane(&Planes[planepnt->AI.nextpair], &planecnt, planepnt, dowingpairs);
	}

	leadplane = AIGetLeader(planepnt);
	if((!(leadplane->Status & PL_AI_DRIVEN)) || (AIInPlayerGroup(planepnt)))
	{
		winglead = planepnt->AI.winglead;
		if(winglead >= 0)
		{
			nextpair = Planes[winglead].AI.nextpair;
			if(nextpair >= 0)
			{
				AISetUpLandingTimingPlane(&Planes[nextpair], &planecnt, planepnt, dowingpairs);
			}
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		if((dowingpairs) && (dospacing) && (planepnt->AI.iAIFlags2 & AILANDING))
		{
			xftoff = -50;
			zftoff = 0;
			AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.TargetPos.X, &planepnt->AI.TargetPos.Z);
			planepnt->AI.FormationPosition.SetValues(0.0f,0.0f,0.0f);
		}
	}
}

//**************************************************************************************
void AISetUpLandingTimingPlane(PlaneParams *planepnt, int *planecnt, PlaneParams *leadplane, int dowingpairs)
{
	long xftoff = 2000;
	long zftoff = -12000;
	int isvalid = 0;
	int dospacing = 0;

	if(planepnt->AI.iAIFlags2 & AILANDING)
	{
		if(dowingpairs && (planepnt->AI.winglead >= 0))
		{
			if(planepnt->AI.Behaviorfunc == AIFormationFlyingLinked)
			{
				isvalid = 1;
			}
			else if(planepnt->AI.OrgBehave == AIFormationFlyingLinked)
			{
				isvalid = 1;
			}

			if(isvalid)
			{
				planepnt->AI.LinkedPlane = &Planes[planepnt->AI.winglead];
				planepnt->AI.DesiredHeading = leadplane->AI.DesiredHeading;
				planepnt->AI.TargetPos = leadplane->AI.TargetPos;
				xftoff = 50;
				zftoff = 0;
				AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, leadplane->AI.TargetPos.X, leadplane->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.TargetPos.X, &planepnt->AI.TargetPos.Z);
	//			planepnt->AI.FormationPosition.SetValues(212.0f,0.0f, 212.0f);
				planepnt->AI.FormationPosition.SetValues(100.0f,0.0f, 100.0f);
			}
		}
		else
		{
			if(planepnt->AI.Behaviorfunc == AIFormationFlyingLinked)
			{
				planepnt->AI.Behaviorfunc = AILandingApproachBreak;
				isvalid = 1;
			}
			else if(planepnt->AI.OrgBehave == AIFormationFlyingLinked)
			{
				planepnt->AI.OrgBehave = AILandingApproachBreak;
				isvalid = 1;
			}

			if(isvalid)
			{
				planepnt->AI.DesiredHeading = leadplane->AI.DesiredHeading;
				planepnt->AI.TargetPos = leadplane->AI.TargetPos;

				xftoff = 2000 + planepnt->AI.FormationPosition.X;
				zftoff = -12000 - (planepnt->V * (10 * *planecnt));

				planepnt->AI.lVar2 = xftoff;  //  New;

				AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
				FPointDouble temppos = planepnt->AI.TargetPos;
				if(InHighResTerrainRegion(temppos))
				{
					GetHeightWithInstances(temppos,AllFoundations);
					planepnt->AI.TargetPos.Y = temppos.Y;
				}
				else
				{
					planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
				}
				planepnt->AI.WayPosition.Y = leadplane->AI.WayPosition.Y;
				*planecnt = *planecnt + 1;
			}
		}
	}


	if(planepnt->AI.wingman >= 0)
	{
		AISetUpLandingTimingPlane(&Planes[planepnt->AI.wingman], planecnt, leadplane, dowingpairs);
		dospacing = 1;
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AISetUpLandingTimingPlane(&Planes[planepnt->AI.nextpair], planecnt, leadplane, dowingpairs);
	}

	if(planepnt->AI.wingman >= 0)
	{
		if((dowingpairs) && (dospacing) && (planepnt->AI.iAIFlags2 & AILANDING))
		{
			xftoff = -50;
			zftoff = 0;
			AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, leadplane->AI.TargetPos.X, leadplane->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.TargetPos.X, &planepnt->AI.TargetPos.Z);
			planepnt->AI.FormationPosition.SetValues(0.0f,0.0f,0.0f);
		}
	}
}

//**************************************************************************************
void AIFormationFlyingLinked(PlaneParams *planepnt)
{
	FPoint leadposition;
	PlaneParams *leadplane;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		FlyToWayPoint(planepnt);
		return;
	}

	leadplane = planepnt->AI.LinkedPlane;

	leadposition = leadplane->WorldPosition;

	AIGetXZOffset((float)leadplane->Heading / DEGREE, leadposition.X, leadposition.Z, (long)planepnt->AI.FormationPosition.X, (long)planepnt->AI.FormationPosition.Z, &planepnt->AI.WorldFormationPos.X, &planepnt->AI.WorldFormationPos.Z, leadplane);
	planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y);

	AIFormationFlyingLead(planepnt, leadplane);

	if(leadplane->AI.iAIFlags2 & AILANDING)
	{
		planepnt->AI.iAIFlags2 |= AILANDING;
		planepnt->AI.DesiredPitch = leadplane->AI.DesiredPitch;
		if(leadplane->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(GEAR, ON, 1);
				}
				RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
			}
		}
		if(leadplane->Flaps)
		{
			if(!planepnt->Flaps)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(FLAPS, ON, 1);
				}
				planepnt->Flaps = leadplane->Flaps;
				planepnt->FlapsCommandedPos = leadplane->FlapsCommandedPos;
			}
		}
	}
}

//**************************************************************************************
void AIFormationFlyingLinkedOffset(PlaneParams *planepnt)
{
	FPoint leadposition;
	PlaneParams *leadplane;
	FPoint orgFormationPosition;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		FlyToWayPoint(planepnt);
		return;
	}

	leadplane = planepnt->AI.LinkedPlane;

	leadposition = leadplane->WorldPosition;
	orgFormationPosition = planepnt->AI.FormationPosition;

	planepnt->AI.FormationPosition.X -= leadplane->AI.FormationPosition.X;
	planepnt->AI.FormationPosition.Y -= leadplane->AI.FormationPosition.Y;
	planepnt->AI.FormationPosition.Z -= leadplane->AI.FormationPosition.Z;

	AIGetXZOffset((float)leadplane->Heading / DEGREE, leadposition.X, leadposition.Z, (long)planepnt->AI.FormationPosition.X, (long)planepnt->AI.FormationPosition.Z, &planepnt->AI.WorldFormationPos.X, &planepnt->AI.WorldFormationPos.Z, leadplane);
	planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y);

	AIFormationFlyingLead(planepnt, leadplane);

	planepnt->AI.FormationPosition = orgFormationPosition;

	if(leadplane->AI.iAIFlags2 & AILANDING)
	{
		planepnt->AI.iAIFlags2 |= AILANDING;
		planepnt->AI.DesiredPitch = leadplane->AI.DesiredPitch;
		if(leadplane->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(GEAR, ON, 1);
				}
				RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
			}
		}
		if(leadplane->Flaps)
		{
			if(!planepnt->Flaps)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(FLAPS, ON, 1);
				}
				planepnt->Flaps = leadplane->Flaps;
				planepnt->FlapsCommandedPos = leadplane->FlapsCommandedPos;
			}
		}
	}
}

//**************************************************************************************
void AILandingLevelBreak(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;

	planepnt->AI.DesiredPitch = 0;
	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, 300, 1);

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || (fabs(offangle) > 90))
	{
		if(fabs(offangle) > 90)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingLevelBreak2(planepnt);
	}
}

//**************************************************************************************
void AISetLandingLevelBreak2(PlaneParams *planepnt)
{
#if 1
	long xftoff = -4000;
	long zftoff = 3000;
	float turnradft;

	turnradft = AIGetTurnRadFt(planepnt) * 2;

	if(xftoff > -turnradft)
	{
		xftoff = -turnradft;
	}
	if(zftoff > turnradft)
	{
		zftoff = turnradft;
	}

	planepnt->AI.lVar2 += xftoff;  //  New
	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingLevelBreak2;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000 * FTTOWU);
#else
	long xftoff = -6000;
	long zftoff = -8000;

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingLevelBreak2;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000 * FTTOWU);
#endif
}

//**************************************************************************************
void AILandingLevelBreak2(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;

	planepnt->AI.DesiredPitch = 0;
	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0
	MBAdjustAIThrust(planepnt, 300, 1);

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 90) && (AIInNextRange(planepnt, tdistft,  3000))))
	{
		if(fabs(offangle) > 90)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingDownwind(planepnt);
	}
}

//**************************************************************************************
void AISetLandingDownwind(PlaneParams *planepnt)
{
	long xftoff = -6000;
	long zftoff = 18000;
	float orgx = planepnt->AI.WayPosition.X;

	xftoff = planepnt->AI.lVar2;  //  New, fix for E/W runways;
	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
//	planepnt->AI.WayPosition.X = orgx;  Doesn't work for E/W runways.
	planepnt->AI.Behaviorfunc = AILandingDownwind;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000 * FTTOWU);
	if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(GEAR, ON, 1);
		}
		RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
	}
	planepnt->AI.DesiredPitch = 0;
}

//**************************************************************************************
void AILandingDownwind(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;
	float desiredpitch;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, 250, 1);

	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);

	if(desiredpitch >= AIDESIREDAOA)
	{
		desiredpitch = AIDESIREDAOA;
	}
	else
	{
		desiredpitch += 0.0005 * DeltaTicks;
	}

	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 90) && (AIInNextRange(planepnt, tdistft, 3000))))
	{
		if(fabs(offangle) > 90)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingBaseLeg(planepnt);
	}
}

//**************************************************************************************
void AISetLandingBaseLeg(PlaneParams *planepnt)
{
	long xftoff = -3000;
	long zftoff = 21000;
	float orgx = planepnt->AI.WayPosition.X;

	xftoff = (planepnt->AI.lVar2 / 2); //  New for E/W runways
	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
//	planepnt->AI.WayPosition.X = (planepnt->AI.TargetPos.X + orgx) / 2;  //  Doesn't work for E/W runways.
	planepnt->AI.Behaviorfunc = AILandingBaseLeg;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (1500 * FTTOWU);
}

//**************************************************************************************
void AILandingBaseLeg(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 30)  //  was 60
	{
		rollval = (rollval < 0.0) ? -30 : 30;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, 220, 1);

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 90) && (AIInNextRange(planepnt, tdistft,  1500))))
	{
		if(fabs(offangle) > 90)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingBaseLeg2(planepnt);
	}
}

//**************************************************************************************
void AISetLandingBaseLeg2(PlaneParams *planepnt)
{
	long xftoff = 0;
	long zftoff = 15000;

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingBaseLeg2;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (1000 * FTTOWU);
}

//**************************************************************************************
void AILandingBaseLeg2(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 30)  //  was 60
	{
		rollval = (rollval < 0.0) ? -30 : 30;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 50, 1);

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 120) && (AIInNextRange(planepnt, tdistft, 1500))))
	{
		if(fabs(offangle) > 120)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingFinal(planepnt);
		if(planepnt->AI.LinkedPlane == NULL)
		{
			AIAllowNextLander(planepnt);
		}
	}
}

//**************************************************************************************
void AISetLandingFinal(PlaneParams *planepnt)
{
	long xftoff = 0;
	long zftoff = 12000;
	int iswingpair = 0;
	PlaneParams *wingplane;

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingFinal;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;

	if(planepnt->AI.wingman >= 0)
	{
		wingplane = &Planes[planepnt->AI.wingman];
		if(wingplane->AI.Behaviorfunc == AIFormationFlyingLinked)
		{
			AISetLandingFinal(wingplane);
		}
		else if(wingplane->AI.OrgBehave == AIFormationFlyingLinked)
		{
			AISetLandingFinal(wingplane);
		}
	}
}

//**************************************************************************************
void AILandingFinal(PlaneParams *planepnt)
{
	float tdist, tdistft, tdist2;
	double offangle;
	float dx, dy, dz, dx2, dz2;
	double rollval;
	float toffpitch;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	dx2 = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
	dz2 = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;

//	tdist2 = QuickDistance(dx2,dz2);
	tdist2 = sqrt((dx2 * dx2) + (dz2 * dz2));
	toffpitch = atan2(dy, tdist2) * 57.2958;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
	{
		if(toffpitch > -1.5)
		{
			toffpitch += 0.5;
		}
		else if(toffpitch < -2.0)
		{
			toffpitch -= 0.5;
		}

	}
	else
	{
		if(toffpitch > -2.5)
		{
			toffpitch += 0.5;
		}
		else if(toffpitch < -3.0)
		{
			toffpitch -= 0.5;
		}
	}

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 30)  //  was 60
	{
		rollval = (rollval < 0.0) ? -30 : 30;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
#if 0
	if(!planepnt->OnGround)
	{
		planepnt->DesiredPitch -= 0x222;
	}
#endif
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 40, 1);


	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 30) && (AIInNextRange(planepnt, tdistft, 1500))))
	{
		if(fabs(offangle) > 30)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingTouchdown(planepnt);
	}
}

//**************************************************************************************
void AISetLandingTouchdown(PlaneParams *planepnt)
{
	planepnt->AI.WayPosition = planepnt->AI.TargetPos;  //  May want to move this up some.
	planepnt->AI.Behaviorfunc = AILandingTouchdown;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
}

//**************************************************************************************
void AILandingTouchdown(PlaneParams *planepnt)
{
	float tdist, tdistft;
	float toffpitch;
	double offangle;
	float dx, dy, dz;
	double rollval;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	toffpitch = atan2(dy, tdist) * 57.2958;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
	{
		if(toffpitch > -1.5)
		{
			toffpitch -= 1.0;
		}
		else if(toffpitch < -2.0)
		{
			toffpitch += 1.0;
		}

	}
	else
	{
		if(toffpitch > -2.5)
		{
			toffpitch -= 1.0;
		}
		else if(toffpitch < -3.0)
		{
			toffpitch += 1.0;
		}
	}

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	if(planepnt->OnGround)
	{
		planepnt->AI.iAIFlags2 |= AI_STAY_ON_GROUND;
		planepnt->DesiredPitch = 0;
//		planepnt->AI.DesiredRoll = -planepnt->DesiredRoll;
		planepnt->DesiredRoll = 0;
	}

	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20, 1);
	planepnt->AI.DesiredRoll = 0;		//  to keep from yawing on ground when not meaning to.

	tdistft = tdist * WUTOFT;
	if((tdistft < 80) || ((fabs(offangle) > 30) && (AIInNextRange(planepnt, tdistft, 1000))))  //  was (tdistft < 40)
	{
		if(fabs(offangle) > 30)
		{
			planepnt->DesiredRoll = planepnt->Roll;
			planepnt->DesiredPitch = planepnt->Pitch;
		}
		AISetLandingRollout(planepnt);
	}
}

//**************************************************************************************
void AISetLandingRollout(PlaneParams *planepnt)
{
	long xftoff = 0;
	long zftoff = -6000;

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingRollout;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
	planepnt->SpeedBrakeState = 1;
	if(planepnt == (PlaneParams *)PlayerPlane)
	{
		DisplayWarning(SPDB, ON,  1);
	}
}

//**************************************************************************************
void AILandingRollout(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;
	float desiredpitch, pitchpercent, pitchchange;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}

	if(!planepnt->OnGround)
	{
		planepnt->DesiredPitch = -0x222;
////		planepnt->DesiredPitch = planepnt->Pitch;
	}
	else
	{
		planepnt->AI.iAIFlags2 |= AI_STAY_ON_GROUND;
		planepnt->DesiredPitch = 0;
		planepnt->AI.DesiredRoll = -planepnt->DesiredRoll;
		planepnt->DesiredRoll = 0;
	}

	planepnt->RollPercentage = 0.40;	//  max roll 90.0
	planepnt->SpeedPercentage = 0.3f;  0.5f;  //  0.25;  //  this is a test SRE

	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);

	if((planepnt->IndicatedAirSpeed < 90) || ((planepnt->IndicatedAirSpeed < 155) && (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))))
	{
		if(desiredpitch <= 0)
		{
			desiredpitch = 0;
		}
		else
		{
			pitchpercent = 1 - (desiredpitch / 13.0f);

			if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
			{
				pitchchange = 0.005 * pitchpercent;

				if(pitchchange < 0.0005)
				{
					pitchchange = 0.0005;
				}
				else if(pitchchange > 0.005)
				{
					pitchchange = 0.005;
				}
			}
			else
			{
				pitchchange = 0.02 * pitchpercent;

				if(pitchchange < 0.002)
				{
					pitchchange = 0.002;
				}
				else if(pitchchange > 0.02)
				{
					pitchchange = 0.02;
				}
			}

			desiredpitch -= pitchchange * DeltaTicks;
			if(desiredpitch < 0)
			{
				desiredpitch = 0;
			}
		}
	}
	else
	{
		if(desiredpitch > 13.0f)
		{
			desiredpitch = 13.0f;
		}
		else
		{
			desiredpitch += 0.0005 * DeltaTicks;
		}
	}

	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);

	if(planepnt->OnGround)
	{
		MBAdjustAIThrust(planepnt, 75, 1);
	}
	else
	{
		MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed, 1);
	}

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 30) && (AIInNextRange(planepnt, tdistft, 1000))))
	{
		AISetLandingTaxi(planepnt);
	}
}

//**************************************************************************************
void AISetLandingTaxi(PlaneParams *planepnt)
{
	long xftoff = 0;
	long zftoff = -8000;

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingTaxi;
	planepnt->SpeedBrakeState = 0;
	if(planepnt == (PlaneParams *)PlayerPlane)
	{
		DisplayWarning(SPDB, OFF,  1);
	}

	LogMissionStatistic(LOG_FLIGHT_LANDED_FRIENDLY,(planepnt-Planes),1,0);
}

//**************************************************************************************
void AILandingTaxi(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;
	float desiredpitch;
	PlaneParams *parkedplane;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
	if(planepnt->DesiredPitch < 0x8000)
	{
		planepnt->DesiredPitch = 0;
	}

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 15)  //  was 60
	{
		rollval = (rollval < 0.0) ? -15 : 15;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->AI.DesiredRoll = -planepnt->DesiredRoll;
	planepnt->DesiredRoll = 0;
	planepnt->RollPercentage = 0.40;	//  max roll 90.0
	MBAdjustAIThrust(planepnt, 30, 1);
	planepnt->AI.iAIFlags2 |= AI_STAY_ON_GROUND;

	if(tdist < (200 * FTTOWU))
	{
		MBAdjustAIThrust(planepnt, (tdist * WUTOFT) / 4, 1);
	}
	else
	{
		MBAdjustAIThrust(planepnt, 75, 1);
	}

	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);

	if(desiredpitch < 0)
	{
		desiredpitch = 0;
	}
	else
	{
		desiredpitch -= 0.0005 * DeltaTicks;
		if(desiredpitch < 0)
		{
			desiredpitch = 0;
		}
	}

	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);


	tdistft = tdist * WUTOFT;
	if((tdistft < 20) || ((fabs(offangle) > 30) && (AIInNextRange(planepnt, tdistft,  1000))))
	{
		planepnt->AI.DesiredPitch = 0;
		planepnt->AI.Behaviorfunc = AIParked;
		planepnt->AI.lTimer1 = 30000;

		PlaneParams *leadplane = AIGetLeader(planepnt);
		if((leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
		{
			AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay);
		}
	}
	parkedplane = AIAvoidLandingCrash(planepnt);
	if(parkedplane)
	{
		planepnt->SpeedPercentage = 1.0;
		if(parkedplane->AI.Behaviorfunc == AIParked)
		{
			planepnt->AI.DesiredPitch = 0;
			planepnt->AI.Behaviorfunc = AIParked;
			planepnt->AI.lTimer1 = 30000;

			PlaneParams *leadplane = AIGetLeader(planepnt);
			if((leadplane == planepnt) && (leadplane->AI.lPlaneID != 30))
			{
				AISetUpWayPointEndActions(planepnt, planepnt->AI.CurrWay);
			}
		}
		else
		{
			planepnt->DesiredSpeed = parkedplane->V * 0.9;
		}
	}
}

//**************************************************************************************
float AIGetMarshallHeight(PlaneParams *planepnt)
{
	float maxheight = 0;
	PlaneParams *checkplane;
	PlaneParams *leadplane;
	float minx, maxx, minz, maxz;
	float checkx, checkz;
	float runwaybuff;
	int foundone = 0;

	runwaybuff = 1000 * FTTOWU;
	minx = planepnt->AI.TargetPos.X - runwaybuff;
	maxx = planepnt->AI.TargetPos.X + runwaybuff;
	minz = planepnt->AI.TargetPos.Z - runwaybuff;
	maxz = planepnt->AI.TargetPos.Z + runwaybuff;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			leadplane = AIGetLeader(checkplane);
			if((checkplane != planepnt) && ((leadplane == checkplane) || (checkplane->AI.LinkedPlane == NULL)))
			{
//				if((checkplane->AI.Behaviorfunc == AIPlaneLandingFlyToField) || (checkplane->AI.Behaviorfunc == AIEnterLandingPattern))  //   || (checkplane->AI.Behaviorfunc == ) ||
				if((checkplane->AI.Behaviorfunc == AIPlaneLandingFlyToField) || (checkplane->AI.Behaviorfunc == AIFlyMarshallPattern) || (checkplane->AI.Behaviorfunc == AIFlySlowMarshallPattern))
				{
					if(checkplane->AI.WayPosition.Y > maxheight)
					{
						checkx = checkplane->AI.TargetPos.X;
						checkz = checkplane->AI.TargetPos.Z;
						if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
						{
							maxheight = checkplane->AI.WayPosition.Y;
							foundone = 1;
						}
					}
				}
				else if((checkplane->AI.iAIFlags2 & AILANDING) && (!(checkplane->Status & PL_AI_DRIVEN)))
				{
					if(checkplane->AI.TargetPos.Y > maxheight)
					{
						checkx = checkplane->AI.TargetPos.X;
						checkz = checkplane->AI.TargetPos.Z;
						if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
						{
							maxheight = checkplane->AI.TargetPos.Y;
							foundone = 1;
						}
					}
				}
			}
		}
		checkplane ++;
	}

	if(!foundone)
	{
		return((10000.0f * FTTOWU));
	}

	if(maxheight < (10000.0f * FTTOWU))
	{
		maxheight = (10000.0f * FTTOWU);
	}

	maxheight += (1000.0f * FTTOWU);

	return(maxheight);
}

//**************************************************************************************
int AICheckMarshallPattern(PlaneParams *planepnt)
{
	float maxheight = 0;
	PlaneParams *checkplane;
	PlaneParams *leadplane;
	float minx, maxx, minz, maxz;
	float checkx, checkz;
	float runwaybuff;

	runwaybuff = 1000 * FTTOWU;
	minx = planepnt->AI.TargetPos.X - runwaybuff;
	maxx = planepnt->AI.TargetPos.X + runwaybuff;
	minz = planepnt->AI.TargetPos.Z - runwaybuff;
	maxz = planepnt->AI.TargetPos.Z + runwaybuff;


	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			leadplane = AIGetLeader(checkplane);
			if((checkplane != planepnt) && ((leadplane == checkplane) || (checkplane->AI.LinkedPlane == NULL)))
			{
//				if((checkplane->Status & PL_AI_DRIVEN) && ((checkplane->AI.Behaviorfunc == AIEnterLandingPattern) || (checkplane->AI.Behaviorfunc == AIFlyMarshallPattern)))  //   || (checkplane->AI.Behaviorfunc == ) ||
				if((checkplane->AI.Behaviorfunc == AIEnterLandingPattern) || (checkplane->AI.Behaviorfunc == AIFlyMarshallPattern) || (checkplane->AI.Behaviorfunc == AIFlySlowMarshallPattern) || ((checkplane->AI.Behaviorfunc == AILandingSlowBaseLeg2) && (checkplane->AI.wingman == -1) && (checkplane->AI.nextpair == -1)))  //   || (checkplane->AI.Behaviorfunc == ) ||
				{
					checkx = checkplane->AI.TargetPos.X;
					checkz = checkplane->AI.TargetPos.Z;
					if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
					{
						return(1);
					}
				}
				else if((checkplane->AI.iAIFlags2 & AILANDING) && (!(checkplane->Status & PL_AI_DRIVEN)))
				{
					checkx = checkplane->AI.TargetPos.X;
					checkz = checkplane->AI.TargetPos.Z;
					if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
					{
						return(1);
					}
				}
			}
		}
		checkplane ++;
	}
	return(0);
}

//**************************************************************************************
void AIFlyMarshallPattern(PlaneParams *planepnt)
{
	float dy;

	planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
	MBAdjustAIThrust(planepnt, 350, 1);
	dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	if(planepnt->AI.lTimer2 < 0)
	{
		if(!AICheckMarshallPattern(planepnt))
		{
			AISetEnterLandingPattern(planepnt);
		}
		else
		{
			planepnt->AI.lTimer2 = 60000;
		}
	}
}

//**************************************************************************************
void AIFlySlowMarshallPattern(PlaneParams *planepnt)
{
	float dy;

	planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
	MBAdjustAIThrust(planepnt, 350, 1);
	dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	if(planepnt->AI.lTimer2 < 0)
	{
		AIMoreInSlowLanding(planepnt);
#if 0
		if(!AIMoreInSlowLanding(planepnt))
		{
			AIAllowNextLander(planepnt);
		}
#endif
	}
}

//**************************************************************************************
void AISetUpSlowLanderPattern(PlaneParams *planepnt)
{
	AIMoreInSlowLanding(planepnt);
#if 0
	if(!AIMoreInSlowLanding(planepnt))
	{
		AIAllowNextLander(planepnt);
	}
#endif
}

//**************************************************************************************
int AIMoreInSlowLanding(PlaneParams *planepnt)
{
	int newlead = -1;
	int worknum;
	float degspersec, fworkvar;

	if(planepnt->AI.wingman >= 0)
	{
		newlead = planepnt->AI.wingman;
		AIUpdateGroupPlaneLink(planepnt - Planes, &Planes[newlead]);
	}
	if(planepnt->AI.nextpair >= 0)
	{
		if(newlead == -1)
		{
			newlead = planepnt->AI.nextpair;
			AIUpdateGroupPlaneLink(newlead, &Planes[newlead]);
		}
		AIOffsetFormationPositions(planepnt->AI.nextpair, Planes[newlead].AI.FormationPosition);
	}

	if(newlead == -1)
	{
		worknum = planepnt->AI.winglead;
		if(worknum >= 0)
		{
			worknum = Planes[worknum].AI.nextpair;
			if(worknum >= 0)
			{
				newlead = worknum;
				AIUpdateGroupPlaneLink(worknum, &Planes[worknum]);
				AIOffsetFormationPositions(newlead, Planes[newlead].AI.FormationPosition);
			}
		}
	}

	if(newlead != -1)
	{
		Planes[newlead].AI.LinkedPlane = NULL;
		Planes[newlead].AI.Behaviorfunc = AIFlySlowMarshallPattern;
		Planes[newlead].AI.lTimer2 = 75000;
		Planes[newlead].AI.FormationPosition.ZeroLength();
		Planes[newlead].AI.TargetPos = planepnt->AI.TargetPos;
		Planes[newlead].AI.WayPosition = planepnt->AI.WayPosition;
		degspersec = AIGetTurnRateForOrbit((2.5f * NMTOFT), 350, Planes[newlead].AI.WayPosition.Y * WUTOFT);
		if(degspersec > Planes[newlead].YawRate)
		{
			degspersec = Planes[newlead].YawRate;
		}
		fworkvar = asin(degspersec / Planes[newlead].YawRate);
		fworkvar = RadToDeg(fworkvar);
		Planes[newlead].AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
	}

	planepnt->AI.LinkedPlane = NULL;

	AISetLandingSlowBaseLeg2(planepnt);

	if(newlead == -1)
	{
		return(0);
	}
	return(1);
}

//**************************************************************************************
void AISetLandingSlowBaseLeg2(PlaneParams *planepnt)
{
	long xftoff = 0;
	long zftoff = 30000;

	AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
	planepnt->AI.Behaviorfunc = AILandingSlowBaseLeg2;
	FPointDouble temppos = planepnt->AI.TargetPos;
	if(InHighResTerrainRegion(temppos))
	{
		GetHeightWithInstances(temppos,AllFoundations);
		planepnt->AI.TargetPos.Y = temppos.Y;
	}
	else
	{
		planepnt->AI.TargetPos.Y = LandHeight(planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z);
	}
	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (1000 * FTTOWU);
//	planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
}

//**************************************************************************************
void AILandingSlowBaseLeg2(PlaneParams *planepnt)
{
	float tdist, tdistft;
	double offangle;
	float dx, dy, dz;
	double rollval;
	float toffpitch;
	float desiredpitch;

	planepnt->AI.iAIFlags2 |= AILANDING;
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

#if 0
	float dx2, dz2, tdist2;

	dx2 = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
	dz2 = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;

	tdist2 = QuickDistance(dx2,dz2);
	toffpitch = atan2(dy, tdist2) * 57.2958;
#endif
	toffpitch = atan2(dy, tdist) * 57.2958;

	if(toffpitch > -1.5)
	{
		toffpitch += 1.0;
	}
	else if(toffpitch < -2.0)
	{
		toffpitch -= 7.0;
	}

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 30)  //  was 60
	{
		rollval = (rollval < 0.0) ? -30 : 30;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	MBAdjustAIThrust(planepnt, 350, 1);

	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);

	if(desiredpitch >= AISLOWDESIREDAOA)
	{
		desiredpitch = AISLOWDESIREDAOA;
	}
	else
	{
		desiredpitch += 0.0005 * DeltaTicks;
	}

	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TANKER))
	{
		AIChangeFXArray(planepnt, 0, 0, 5000);
	}

	tdistft = tdist * WUTOFT;
	if((tdistft < 40) || ((fabs(offangle) > 120) && (AIInNextRange(planepnt, tdistft, 1500))))
	{
		if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			if(planepnt == (PlaneParams *)PlayerPlane)
			{
				DisplayWarning(GEAR, ON, 1);
			}
			RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
		}

		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(FLAPS, ON, 1);
		}
		planepnt->Flaps = 30.0f;
		planepnt->FlapsCommandedPos = 30.0;
		if((planepnt->AI.wingman == -1) && (planepnt->AI.nextpair == -1))
		{
			if(planepnt->AI.winglead == -1)
			{
				AIAllowNextLander(planepnt);
			}
			else if(Planes[planepnt->AI.winglead].AI.nextpair == -1)
			{
				AIAllowNextLander(planepnt);
			}
		}
		AISetLandingFinal(planepnt);
	}
}

//**************************************************************************************
void AIOffsetFormationPositions(int planenum, FPoint offsetval)
{
	Planes[planenum].AI.FormationPosition.X -= offsetval.X;
	Planes[planenum].AI.FormationPosition.Y -= offsetval.Y;
	Planes[planenum].AI.FormationPosition.Z -= offsetval.Z;

	if(Planes[planenum].AI.wingman >= 0)
	{
		AIOffsetFormationPositions(Planes[planenum].AI.wingman, offsetval);
	}
	if(Planes[planenum].AI.nextpair >= 0)
	{
		AIOffsetFormationPositions(Planes[planenum].AI.nextpair, offsetval);
	}
}

//**************************************************************************************
void AIAllowNextLander(PlaneParams *planepnt)
{
	float minheight = -1;
	PlaneParams *checkplane;
	PlaneParams *leadplane;
	PlaneParams *foundplane = NULL;
	float minx, maxx, minz, maxz;
	float checkx, checkz;
	float runwaybuff, wu1000, wu500;
	int ianglesht;

	runwaybuff = 1000 * FTTOWU;
	wu500 = 500 * FTTOWU;
	wu1000 = 1000 * FTTOWU;
	minx = planepnt->AI.TargetPos.X - runwaybuff;
	maxx = planepnt->AI.TargetPos.X + runwaybuff;
	minz = planepnt->AI.TargetPos.Z - runwaybuff;
	maxz = planepnt->AI.TargetPos.Z + runwaybuff;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			leadplane = AIGetLeader(planepnt);
			if((leadplane == checkplane) || (checkplane->AI.LinkedPlane == NULL))
			{
//				if((checkplane->AI.Behaviorfunc == AIPlaneLandingFlyToField) || (checkplane->AI.Behaviorfunc == AIEnterLandingPattern))  //   || (checkplane->AI.Behaviorfunc == ) ||
				if((checkplane->Status & PL_AI_DRIVEN) && ((checkplane->AI.Behaviorfunc == AIEnterLandingPattern) || (checkplane->AI.Behaviorfunc == AIFlyMarshallPattern)))  //   || (checkplane->AI.Behaviorfunc == ) ||
				{
					if((checkplane->AI.WayPosition.Y < minheight) || (minheight == -1))
					{
						checkx = checkplane->AI.TargetPos.X;
						checkz = checkplane->AI.TargetPos.Z;
						if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
						{
							minheight = checkplane->AI.WayPosition.Y;
							foundplane = checkplane;
						}
					}
				}
				else if((checkplane->AI.iAIFlags2 & AILANDING) && (!(checkplane->Status & PL_AI_DRIVEN)))
				{
					if((checkplane->AI.TargetPos.Y < minheight) || (minheight == -1))
					{
						checkx = checkplane->AI.TargetPos.X;
						checkz = checkplane->AI.TargetPos.Z;
						if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
						{
							minheight = checkplane->AI.TargetPos.Y;
							foundplane = checkplane;
						}
					}
				}
			}
		}
		checkplane ++;
	}

	if(foundplane != NULL)
	{
		if(foundplane->Status & PL_AI_DRIVEN)
		{
			AISetEnterLandingPattern(foundplane);
		}
		else
		{
			AICTowerLandingReplyMsg(foundplane - Planes, 1);
			foundplane->AI.iAIFlags2 &= ~AILANDING;
		}
	}
	else
	{
		return;
	}

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			leadplane = AIGetLeader(checkplane);
			if((leadplane == checkplane) || (checkplane->AI.LinkedPlane == NULL))
			{
//				if((checkplane->AI.Behaviorfunc == AIPlaneLandingFlyToField) || (checkplane->AI.Behaviorfunc == AIEnterLandingPattern))  //   || (checkplane->AI.Behaviorfunc == ) ||
				if((checkplane->AI.Behaviorfunc == AIPlaneLandingFlyToField) || (checkplane->AI.Behaviorfunc == AIFlyMarshallPattern))
				{
					if(checkplane->AI.WayPosition.Y > (minheight + wu500))
					{
						checkx = checkplane->AI.TargetPos.X;
						checkz = checkplane->AI.TargetPos.Z;
						if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
						{
							checkplane->AI.WayPosition.Y -= wu1000;
#if 0  //  Shouldn't Happen
							if(checkplane->Status & PL_DEVICE_DRIVEN)
							{
								ianglesht = ((checkplane->AI.WayPosition.Y * WUTOFT) + 500) / 1000;
								AICTowerAnglesHoldMsg(checkplane - Planes, ianglesht);
							}
#endif
						}
					}
				}
				else if((checkplane->AI.iAIFlags2 & AILANDING) && (!(checkplane->Status & PL_AI_DRIVEN)))
				{
					if(checkplane->AI.TargetPos.Y > (minheight + wu500))
					{
						checkx = checkplane->AI.TargetPos.X;
						checkz = checkplane->AI.TargetPos.Z;
						if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
						{
							checkplane->AI.TargetPos.Y -= wu1000;
							if(checkplane->Status & PL_DEVICE_DRIVEN)
							{
								ianglesht = ((checkplane->AI.TargetPos.Y * WUTOFT) + 500) / 1000;
								AICTowerAnglesHoldMsg(checkplane - Planes, ianglesht);
							}
						}
					}
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AICheckHumanMarshallPattern()
{
	float maxheight = 0;
	PlaneParams *checkplane;
	PlaneParams *leadplane;
	float minx, maxx, minz, maxz;
	float checkx, checkz;
	float runwaybuff;
	PlaneParams *planepnt;
	int returnval = 0;
	int workval;

	planepnt = &Planes[0];

	while(planepnt <= LastPlane)
	{
		if((planepnt->Status & PL_DEVICE_DRIVEN) && (planepnt->AI.iAIFlags2 & AILANDING))
		{
			if(planepnt->OnGround)
			{
				planepnt->AI.iAIFlags2 &= ~AILANDING;
				AIAllowNextLander(planepnt);
			}
		}

		if((planepnt->Status & PL_DEVICE_DRIVEN) && (planepnt->AI.iAIFlags2 & AILANDING))
		{
			returnval = 1;

			runwaybuff = 1000 * FTTOWU;
			minx = planepnt->AI.TargetPos.X - runwaybuff;
			maxx = planepnt->AI.TargetPos.X + runwaybuff;
			minz = planepnt->AI.TargetPos.Z - runwaybuff;
			maxz = planepnt->AI.TargetPos.Z + runwaybuff;
			workval = 0;

			checkplane = &Planes[0];
			while (checkplane <= LastPlane)
			{
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
				{
					leadplane = AIGetLeader(checkplane);
					if((checkplane != planepnt) && ((leadplane == checkplane) || (checkplane->AI.LinkedPlane == NULL)))
					{
						if((checkplane->AI.Behaviorfunc == AIEnterLandingPattern) || (checkplane->AI.Behaviorfunc == AIFlyMarshallPattern) || (checkplane->AI.Behaviorfunc == AIFlySlowMarshallPattern))  //   || (checkplane->AI.Behaviorfunc == ) ||
						{
							checkx = checkplane->AI.TargetPos.X;
							checkz = checkplane->AI.TargetPos.Z;
							if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
							{
								workval = 1;
							}
						}
						else if((checkplane->AI.TargetPos.Y == 0) && (checkplane->AI.iAIFlags2 & AILANDING) && (!(checkplane->Status & PL_AI_DRIVEN)))
						{
							checkx = checkplane->AI.TargetPos.X;
							checkz = checkplane->AI.TargetPos.Z;
							if((checkx > minx) && (checkx < maxx) && (checkz > minz) && (checkz < maxz))
							{
								workval = 1;
							}
						}
					}
				}
				checkplane ++;
			}
			if(workval == 0)
			{
				AICTowerLandingReplyMsg(planepnt - Planes, 1);
				planepnt->AI.TargetPos.Y = 0.0f;
			}
		}
		planepnt ++;
	}

	return(returnval);
}

//**************************************************************************************
void AIFlyToCASArea(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	float tdist, tdistnm;

	leadplane = AIGetLeader(planepnt);
	if((leadplane == planepnt) || (planepnt->AI.iAIFlags2 & AI_SEAD_COVER))
	{
		tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);
		tdistnm = (tdist * WUTONM);
		if(tdistnm < planepnt->AI.iVar1)
		{
			if(planepnt->AI.iAIFlags2 & (AI_SEAD_ATTACK|AI_SEAD_COVER))
			{
				AIC_On_Station_Msg(planepnt - Planes);
			}

			planepnt->AI.DesiredHeading = planepnt->Heading;
			if(!(planepnt->AI.iAIFlags2 & AI_SEAD_COVER))
			{
				planepnt->AI.lTimer2 = AIGetCASorSEADTime(planepnt);
				planepnt->AI.LinkedPlane = NULL;
			}
			else
			{
				planepnt->AI.lDesiredSpeed = 350;
			}
			planepnt->AI.lTimer1 = 30000 + (((rand() % 20) - 10) * 1000);
			planepnt->AI.Behaviorfunc = AIFlyCAS;
		}
	}
	else
	{
		planepnt->AI.LinkedPlane = leadplane;
		AIFormationFlyingLinkedOffset(planepnt);
		if(leadplane->AI.Behaviorfunc == AIFlyCAS)
		{
			planepnt->AI.Behaviorfunc = AIFlyCAS;
			planepnt->AI.lTimer2 = leadplane->AI.lTimer2;
		}
		return;
	}
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
}

//**************************************************************************************
void AIFlyCAS(PlaneParams *planepnt)
{
	float offangle;
	float dx, dy, dz, tdist;
	float desiredht;
	float fworkval;
	FPoint fptemp;
	float rollval;
	int cnt = 0;
	PlaneParams *linkedplane;
	DBWeaponType *pweapon_type;
	int talds = 0;

	if(!(planepnt->AI.iAICombatFlags2 & AI_TALDS_DONE))
	{
		if(((planepnt->AI.lTimer3 < 0) || (planepnt->AI.lTimer3 > 15000)) && ((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
		{
			for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
			{
				pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						talds |= 2;
						InstantiateBomb(planepnt, cnt, (1000 * (1.0f/5.7435)), NULL, -1, -1.0f, -1.0f, -1.0f);
						planepnt->WeapLoad[cnt].Count = planepnt->WeapLoad[cnt].Count - 1;
						break;
					}
				}
				else
				{
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						talds |= 1;
					}
				}
			}
			if(talds)
			{
				if(talds == 1)
				{
				 	planepnt->AI.iAICombatFlags2 |= AI_TALDS_DONE;
					AIC_GenericMsgPlane(planepnt - Planes, GM_DUCKS_AWAY);
				}

				planepnt->AI.lTimer3 = 4000 + ((rand() & 0x3) * 1000);
				planepnt->AI.lTimer1 += planepnt->AI.lTimer3;
				if(!((planepnt->AI.LinkedPlane != NULL) && (!(planepnt->AI.iAIFlags2 & AI_SEAD_COVER))))
				{
					if(talds & 2)
					{
						fworkval = (rand() % 10) - 5;
						if(fworkval < 0)
						{
							fworkval -= 5;
						}
						else
						{
							fworkval += 5;
						}

						planepnt->AI.DesiredHeading += AIConvert180DegreeToAngle(fworkval);
					}
					else
					{
						fworkval = (rand() % 40) - 20;
						if(fworkval < 0)
						{
							fworkval -= 80;
						}
						else
						{
							fworkval += 80;
						}

						planepnt->AI.DesiredHeading += AIConvert180DegreeToAngle(fworkval);
					}
					if(MultiPlayer)
					{
						NetPutDesiredHeading(planepnt - Planes, planepnt->AI.DesiredHeading);
					}
				}
			}
		}
	}

	cnt = 0;
	//  AISimpleGroundRadar;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS);
	if(!planepnt->AI.iVar1)
	{
		planepnt->AI.iVar1 = 20;
	}

	linkedplane = planepnt->AI.LinkedPlane;
	if((linkedplane != NULL) && (!(planepnt->AI.iAIFlags2 & AI_SEAD_COVER)))
	{
		AIFormationFlyingLinkedOffset(planepnt);

		if((linkedplane->Status & PL_AI_DRIVEN) && (linkedplane->AI.Behaviorfunc != AIFlyCAS))
		{
			if(planepnt->AI.CurrWay->lWPy > 0)
			{
				dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
			}
			else
			{
//				desiredht = (LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z) + ConvertWayLoc(labs(planepnt->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER));
//				desiredht = (AICheckHeightInPath(planepnt, 3000) + ConvertWayLoc(labs(planepnt->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER));
				desiredht = AICheckHeightInPath(planepnt, 3000);

				dy = (AICheckSafeAlt(planepnt, desiredht) + ConvertWayLoc(labs(planepnt->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
			}
			planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
		}

		while((planepnt->AI.LinkedPlane->AI.LinkedPlane != NULL) && (planepnt->AI.LinkedPlane->Status & PL_AI_DRIVEN))
		{
			linkedplane = planepnt->AI.LinkedPlane->AI.LinkedPlane;
			if((linkedplane == planepnt) || (cnt > 10))
			{
				break;
			}
			planepnt->AI.LinkedPlane = linkedplane;
			cnt ++;
		}

		if((planepnt->AI.LinkedPlane->AI.Behaviorfunc != AIFlyCAS) && (planepnt->AI.LinkedPlane->Status & PL_AI_DRIVEN))
		{
			AICheckCASorSEADCondition(planepnt);
		}
		return;
	}

	if(!(planepnt->AI.iAIFlags2 & AI_SEAD_COVER))
	{
		AICheckCASorSEADCondition(planepnt);
	}

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
	if((planepnt->AI.CurrWay->lWPy > 0) || (planepnt->AI.iAIFlags2 & AI_SEAD_COVER))
	{
		dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
	}
	else
	{
//		desiredht = (LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z) + ConvertWayLoc(labs(planepnt->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER));
//		desiredht = (AICheckHeightInPath(planepnt, 3000) + ConvertWayLoc(labs(planepnt->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER));
		desiredht = AICheckHeightInPath(planepnt, 3000);

		dy = (AICheckSafeAlt(planepnt, desiredht) + ConvertWayLoc(labs(planepnt->AI.CurrWay->lWPy)) + (7.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
	}

	if((planepnt->AI.lTimer1 < 0) && ((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		tdist = (fabs(dx) * WUTONM);
		if(tdist < planepnt->AI.iVar1)
		{
			tdist = (fabs(dz) * WUTONM);
		}
		if(tdist < planepnt->AI.iVar1)
		{
			fworkval = (rand() % 20) - 10;
			if(fworkval < 0)
			{
				fworkval -= 15;
			}
			else
			{
				fworkval += 15;
			}

			planepnt->AI.DesiredHeading += AIConvert180DegreeToAngle(fworkval);

			planepnt->AI.lTimer1 = 30000 + (((rand() % 20) - 10) * 1000);
		}
		else
		{
			fptemp = planepnt->AI.WayPosition;
			fptemp.X += ((rand() %  planepnt->AI.iVar1) - (planepnt->AI.iVar1 / 2)) * NMTOFT;
			fptemp.Z += ((rand() %  planepnt->AI.iVar1) - (planepnt->AI.iVar1 / 2)) * NMTOFT;
			planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(AIComputeHeadingToPoint(planepnt, fptemp, &tdist, &dx ,&dy, &dz, 0));
			planepnt->AI.lTimer1 = 40000 + (((rand() % 20) - 10) * 1000);
		}

		if(MultiPlayer)
		{
			NetPutDesiredHeading(planepnt - Planes, planepnt->AI.DesiredHeading);
		}
	}

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

}

//**************************************************************************************
long AIGetCASorSEADTime(PlaneParams *planepnt)
{
	CASActionType *pCASAction;
	SEADActionType *pSEADAction;
	int actcnt, startact, endact;
	MBWayPoints *waypnt;

	waypnt = planepnt->AI.CurrWay;
	startact = waypnt->iStartAct;
	endact = startact + waypnt->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if(AIActions[actcnt].ActionID == ACTION_SEAD)
		{
			pSEADAction = (SEADActionType *)AIActions[actcnt].pAction;
			return(pSEADAction->lTimer);
		}
		if(AIActions[actcnt].ActionID == ACTION_CAS)
		{
			pCASAction = (CASActionType *)AIActions[actcnt].pAction;
			return(pCASAction->lTimer);
		}
	}
	return(0);
}

//**************************************************************************************
void AICheckCASorSEADCondition(PlaneParams *planepnt)
{
#if 0
	CASActionType *pCASAction;
	SEADActionType *pSEADAction;
	int actcnt, startact, endact;
	MBWayPoints *waypnt;
	long lflagvar = 0;
	long ltimer = -1;
	int nextway = 0;

	waypnt = planepnt->AI.CurrWay;
	startact = waypnt->iStartAct;
	endact = startact + waypnt->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if(AIActions[actcnt].ActionID == ACTION_SEAD)
		{
			pSEADAction = (SEADActionType *)AIActions[actcnt].pAction;
			lflagvar = pSEADAction->lFlag;
			ltimer = planepnt->lTimer2;
			break;
		}
		if(AIActions[actcnt].ActionID == ACTION_CAS)
		{
			pCASAction = (CASActionType *)AIActions[actcnt].pAction;
			lflagvar = pCASAction->lFlag;
			ltimer = planepnt->lTimer2;
			break;
		}
	}
#else
	long lflagvar = 0;
	long ltimer = -1;
	int nextway = 0;

	lflagvar = planepnt->AI.lVar2;
	ltimer = planepnt->AI.lTimer2;
#endif

	if(lflagvar == 0)
	{
		if(ltimer < 0)
		{
			nextway = 1;
		}
	}
	else
	{
		nextway = AICheckEventFlagStatus(lflagvar);
	}

	if(nextway)
	{
		if(planepnt->AI.iSide == PlayerPlane->AI.iSide)
		{
			int awacsnum;
			awacsnum = AIGetClosestAWACS(planepnt);
			if(awacsnum != -1)
			{
				AICPlayerSEADCoverDone(planepnt - Planes, awacsnum);
			}
		}

		planepnt->AI.iAIFlags2 &= ~(AI_CAS_ATTACK|AI_SEAD_ATTACK);
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		AINextWayPoint(planepnt);
	}
}

//**************************************************************************************
void AIRestoreCASorSEAD(PlaneParams *planepnt)
{
	FPoint fptemp;
	float dx, dy, dz, tdist;
	MBWayPoints *waypnt;
	int rangenm;

	planepnt->AI.lRadarDelay = 45000;     //   180000;
//	planepnt->AI.lGroundTargetID = -1;
	if(!(planepnt->AI.iAICombatFlags2 & AI_FAC))
	{
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.lGroundTargetFlag = 0;
	}

	if(planepnt->AI.LinkedPlane == NULL)
	{
		rangenm = planepnt->AI.iVar1;
		if(rangenm <= 0)
		{
			rangenm = 1;
		}

		fptemp = planepnt->AI.WayPosition;
		fptemp.X += ((rand() %  rangenm) - (rangenm / 2)) * NMTOFT;
		fptemp.Z += ((rand() %  rangenm) - (rangenm / 2)) * NMTOFT;
		planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(AIComputeHeadingToPoint(planepnt, fptemp, &tdist, &dx ,&dy, &dz, 0));
		if(MultiPlayer)
		{
			NetPutDesiredHeading(planepnt - Planes, planepnt->AI.DesiredHeading);
		}
		planepnt->AI.lTimer1 = 40000 + (((rand() % 20) - 10) * 1000);

		waypnt = planepnt->AI.CurrWay;
		planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(waypnt->lWPy > 0)
		{
			planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
		}
		else
		{
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}
	}
	else if((planepnt->AI.iAIFlags2 & AI_SEAD_COVER) || (planepnt->AI.iAICombatFlags2 & AI_ESCORT_CAS))
	{
		rangenm = 20;

		fptemp = planepnt->AI.LinkedPlane->AI.WayPosition;
		fptemp.X += ((rand() %  rangenm) - (rangenm / 2)) * NMTOFT;
		fptemp.Z += ((rand() %  rangenm) - (rangenm / 2)) * NMTOFT;
		planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(AIComputeHeadingToPoint(planepnt, fptemp, &tdist, &dx ,&dy, &dz, 0));
		if(MultiPlayer)
		{
			NetPutDesiredHeading(planepnt - Planes, planepnt->AI.DesiredHeading);
		}
		planepnt->AI.lTimer1 = 40000 + (((rand() % 20) - 10) * 1000);

		planepnt->AI.WayPosition = planepnt->AI.LinkedPlane->AI.WayPosition;
		planepnt->AI.WayPosition.Y = 20000 * FTTOWU;
	}
	else
	{
		waypnt = planepnt->AI.CurrWay;
		planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(waypnt->lWPy > 0)
		{
			planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
		}
		else
		{
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}
	}

	if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		planepnt->AI.iAIFlags1 |= AI_GIVE_CAS_RESULT;
//	 	AICWingFinishCASAttackMsg(planepnt - Planes);
	}

	planepnt->AI.Behaviorfunc = AIFlyCAS;
}

#if 0
//**************************************************************************************
void AISimpleGroundRadar(PlaneParams *planepnt)
{
  	GroundObject *checkobject;
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
  	GroundObject *foundobject = NULL;
	float nlfoundrange;
  	GroundObject *nlfoundobject = NULL;
	float offangle, toffangle, toffpitch, offpitch;
	float targbearing, nltargbearing;
	float targpitch, nltargpitch;
	int widthok, heightok, visualok;
  	long orgtarg_id;
  	long orgtarg_flag;
	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
	long cnt, cnt_obj_flag;
	float rangeCAS;
	int inCASarea;
	PlaneParams *leadplane = AIGetLeader(planepnt);


	planepnt->AI.lRadarDelay -= DeltaTicks;
	if(planepnt->AI.lRadarDelay > 0)
	{
		return;
	}

	orgtarg_id = planepnt->AI.lGroundTargetID;
	orgtarg_flag = planepnt->AI.lGroundTargetFlag;

	if(orgtarg_id == -1)
	{
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	}

	visualrangenm = AI_VISUAL_RANGE_NM / 2;
	visualrange = (visualrangenm * NMTOWU);
	rangenm = 40;

	if(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))
	{
		rangeCAS = (planepnt->AI.iVar1 * NMTOWU);
		if(!(leadplane->Status & PL_AI_DRIVEN))
		{
			rangenm = planepnt->AI.iVar1;
		}
	}

	radarrange = (rangenm * NMTOWU);
	foundrange = radarrange;
	foundobject = NULL;
	nlfoundrange = radarrange;
	nlfoundobject = NULL;
	widthangle = 20;
	heightangle = 20;

	toffangle = planepnt->AI.fRadarAngleYaw - AIConvertAngleTo180Degree(planepnt->Heading);
	if(fabs(toffangle) > 60)
	{
		radarangleyaw = (toffangle < 0)? AIConvertAngleTo180Degree(planepnt->Heading) - 60 : AIConvertAngleTo180Degree(planepnt->Heading) + 60;
		radarangleyaw = AICapAngle(radarangleyaw);
	}
	else
	{
		radarangleyaw = planepnt->AI.fRadarAngleYaw;
	}

	toffpitch = planepnt->AI.fRadarAnglePitch - AIConvertAngleTo180Degree(planepnt->Pitch);
	if(fabs(toffpitch) > 60)
	{
		radaranglepitch = (toffpitch < 0)? AIConvertAngleTo180Degree(planepnt->Pitch) - 60 : AIConvertAngleTo180Degree(planepnt->Pitch) + 60;
		radaranglepitch = AICapAngle(radaranglepitch);
	}
	else
	{
		radaranglepitch = planepnt->AI.fRadarAnglePitch;
	}

	if(!(planepnt->AI.iAIFlags1 & AIRADARON))
	{
		foundrange = visualrange;
		nlfoundrange = visualrange;
	}

  	checkobject = &GroundObjectList[0];
	for(cnt = 0; cnt < iNumGroundObjects; cnt ++)
	{
//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
		//  if(object is alive and object is a valid target)
		if (cnt >= 0)
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = (checkobject->X * FTTOWU) - planepnt->WorldPosition.X;
			dy = (checkobject->Y * FTTOWU) - planepnt->WorldPosition.Y;
			dz = (checkobject->Z * FTTOWU) - planepnt->WorldPosition.Z;
			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < nlfoundrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					toffangle = offangle - radarangleyaw;
					offangle = AICapAngle(offangle);

					toffangle = AICapAngle(toffangle);
					if(fabs(toffangle) < widthangle)
					{
						widthok = 1;
					}

#if 0  //  Don't think we need to check pitch for ground radar
					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					if(fabs(toffpitch) < heightangle)  //  Plane in radar area
					{
						heightok = 1;
					}
#else
					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					heightok = 1;
#endif

					if(tdist < visualrange)
					{
						//  May want to do pitch and heading check for this
						visualok = 1;
					}

					if((widthok && heightok) || (visualok))
					{
//						AIUpdateRadarThreat(planepnt, checkplane, (widthok && heightok));
						//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
						//  If was previous target, keep lock regardless of chance.
						if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) && (leadplane->Status & PL_AI_DRIVEN))
						{
							dx = (checkobject->X * FTTOWU) - planepnt->AI.WayPosition.X;
							dz = (checkobject->Z * FTTOWU) - planepnt->AI.WayPosition.Z;
							if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS))
							{
								inCASarea = 1;
							}
							else
							{
								inCASarea = 0;
							}
						}
						else
						{
							inCASarea = 1;
						}


						if((!lockedorg) && (inCASarea))
						{
							cnt_obj_flag = 0;
							if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), cnt, cnt_obj_flag))
							{
								foundobject = checkobject;
								foundrange = tdist;
								nlfoundobject = checkobject;
								nlfoundrange = tdist;
								targbearing = nltargbearing = offangle;
								targpitch = nltargpitch = offpitch;
							}
							else
							{
								foundobject = checkobject;
								foundrange = tdist;
								targbearing = offangle;
								targpitch = offpitch;
							}

							if((orgtarg_id == cnt) && (orgtarg_flag == 0) && (planepnt->AI.iAIFlags2 & AIKEEPTARGET))
							{
								foundobject = checkobject;
								foundrange = tdist;
								targbearing = offangle;
								targpitch = offpitch;
								lockedorg = 1;
							}
						}
					}
				}
			}
		}
		checkobject ++;
	}

	if(foundobject == NULL)
	{
		planepnt->AI.lGroundTargetID = -1;
		planepnt->AI.lGroundTargetFlag = 0;
		if(planepnt->AI.iAIFlags1 & AIENGAGED)
		{
			planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
		}
		else
		{
			planepnt->AI.lRadarDelay = 10000;  //  10 seconds until next sweep
		}
		return;
	}

	if(nlfoundobject != NULL)
	{
		planepnt->AI.lGroundTargetID = nlfoundobject - GroundObjectList;
		planepnt->AI.lGroundTargetFlag = 0;
		targbearing = nltargbearing;
		targpitch = nltargpitch;
		foundrange = nlfoundrange;
	}
	else
	{
		planepnt->AI.lGroundTargetID = -1;
		planepnt->AI.lGroundTargetFlag = 0;
	}

	if((planepnt->AI.lGroundTargetID != -1) && (orgtarg_id == -1))
	{
		if(planepnt->AI.iAIFlags2 & AI_SEAD_COVER)
		{
			if(planepnt->AI.Behaviorfunc == AIFlyEscort)
			{
				planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK|AI_RETURN_SEAD_COVER);
			}
			else
			{
				planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK);
			}
		}

		if(planepnt->AI.LinkedPlane == NULL)
		{
			AITransferCASLead(planepnt);
		}

		AIDelayCASRadar(AIGetLeader(planepnt));

		planepnt->AI.WayPosition.X = (GroundObjectList[planepnt->AI.lGroundTargetID].X * FTTOWU);
		planepnt->AI.WayPosition.Y = (GroundObjectList[planepnt->AI.lGroundTargetID].Y * FTTOWU);
		planepnt->AI.WayPosition.Z = (GroundObjectList[planepnt->AI.lGroundTargetID].Z * FTTOWU);
		planepnt->AI.TargetPos = planepnt->AI.WayPosition;

		dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
		dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx, dz);
		offpitch = (atan2(dy, tdist) * 57.2958);

		if(offpitch > -20)
		{
			planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIAwayFromTarget;
		}
		AIDetermineBestBomb(planepnt, planepnt->AI.lGroundTargetID);

		if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
		{
			AICSEADAttackMsg(planepnt - Planes, 1);
		}
		else if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
		 	AICWingCASAttackMsg(planepnt - Planes);
		}
	}

	planepnt->AI.fRadarAngleYaw = targbearing;
	planepnt->AI.fRadarAnglePitch = targpitch;

	toffangle = targbearing - AIConvertAngleTo180Degree(planepnt->Heading);
	if(fabs(toffangle) > 60)
	{
		toffangle = (toffangle < 0)? AIConvertAngleTo180Degree(planepnt->Heading) - 60 : AIConvertAngleTo180Degree(planepnt->Heading) + 60;
		toffangle = AICapAngle(toffangle);
		planepnt->AI.fRadarAngleYaw = toffangle;
	}
	else
	{
		planepnt->AI.fRadarAngleYaw = targbearing;
	}

	toffpitch = targpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
	if(fabs(toffpitch) > 60)
	{
		toffpitch = (toffpitch < 0)? AIConvertAngleTo180Degree(planepnt->Pitch) - 60 : AIConvertAngleTo180Degree(planepnt->Pitch) + 60;
		toffpitch = AICapAngle(toffpitch);
		planepnt->AI.fRadarAnglePitch = toffpitch;
	}
	else
	{
		planepnt->AI.fRadarAnglePitch = targpitch;
	}

	planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
}
#else
//**************************************************************************************
void AISimpleGroundRadar(PlaneParams *planepnt)
{
//  	BasicInstance *checkobject;
  	BasicInstance *checkobject;
	MovingVehicleParams *vehiclepnt;
  	void *foundobject = NULL;
  	void *nlfoundobject = NULL;
  	void *orgtarg_id;
	long objectflag;
	long nlobjectflag;
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	float nlfoundrange;
	float offangle, toffangle, toffpitch, offpitch;
	float targbearing, nltargbearing;
	float targpitch, nltargpitch;
	int widthok, heightok, visualok;
  	long orgtarg_flag;
	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
	long cnt_obj_flag;
	float rangeCAS;
	int inCASarea;
	PlaneParams *leadplane = AIGetLeader(planepnt);
	int pass;
	int attacktype = 0;
	DBShipType *pshiptype;
	int isship;
	int usefactarget = 0;
	int nearfac;

	if(MultiPlayer && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	if(planepnt->AI.Behaviorfunc == AIFlyToCASArea)
	{
		return;
	}

	if(planepnt->AI.iAIFlags2 & (AI_SEAD_ATTACK|AI_SEAD_COVER))
	{
		AISEADSearch(planepnt);
		return;
	}

	planepnt->AI.lRadarDelay -= DeltaTicks;
	if(planepnt->AI.lRadarDelay > 0)
	{
		return;
	}

	offangle = toffangle = toffpitch = offpitch = targpitch = nltargpitch = 0;
	targbearing = nltargbearing = 0;

	if((planepnt->AI.pGroundTarget) && (!(planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET)))
	{
		if(planepnt->AI.iAICombatFlags2 & AI_FAC)
		{
			AICheckFACTarget(planepnt);
		}

		return;
	}

	orgtarg_id = planepnt->AI.pGroundTarget;
	orgtarg_flag = planepnt->AI.lGroundTargetFlag;

	if(orgtarg_id == NULL)
	{
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	}

	visualrangenm = AI_VISUAL_RANGE_NM / 2;
	visualrange = (visualrangenm * NMTOWU);
	rangenm = 40;

	if(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))
	{
		rangeCAS = (planepnt->AI.iVar1 * NMTOWU);
		if(!(leadplane->Status & PL_AI_DRIVEN))
		{
			rangenm = planepnt->AI.iVar1;
		}
	}

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	foundobject = NULL;
	nlfoundrange = radarrange;
	nlfoundobject = NULL;
	objectflag = 0;
	nlobjectflag = 0;
	widthangle = 20;
	heightangle = 20;

	toffangle = planepnt->AI.fRadarAngleYaw - AIConvertAngleTo180Degree(planepnt->Heading);
	toffangle = AICapAngle(toffangle);
	if(fabs(toffangle) > 60)
	{
		radarangleyaw = (toffangle < 0)? AIConvertAngleTo180Degree(planepnt->Heading) - 60 : AIConvertAngleTo180Degree(planepnt->Heading) + 60;
		radarangleyaw = AICapAngle(radarangleyaw);
	}
	else
	{
		radarangleyaw = planepnt->AI.fRadarAngleYaw;
	}

	toffpitch = planepnt->AI.fRadarAnglePitch - AIConvertAngleTo180Degree(planepnt->Pitch);
	toffpitch = AICapAngle(toffpitch);
	if(fabs(toffpitch) > 60)
	{
		radaranglepitch = (toffpitch < 0)? AIConvertAngleTo180Degree(planepnt->Pitch) - 60 : AIConvertAngleTo180Degree(planepnt->Pitch) + 60;
		radaranglepitch = AICapAngle(radaranglepitch);
	}
	else
	{
		radaranglepitch = planepnt->AI.fRadarAnglePitch;
	}

	if(!(planepnt->AI.iAIFlags1 & AIRADARON))
	{
		foundrange = visualrange;
		nlfoundrange = visualrange;
	}

	BasicInstance ***checklist = &AllTargetTypes[0];
	pass = NumTargetTypes;

	if(planepnt->AI.iAICombatFlags2 & (AI_ANTI_SHIP|AI_ANTI_SUB))
	{
		pass = 0;
	}

	if((planepnt->AI.iAICombatFlags2 & AI_FAC) && (planepnt->AI.pGroundTarget) && (planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET))
	{
		pass = 0;
		foundobject = planepnt->AI.pGroundTarget;
		objectflag = planepnt->AI.lGroundTargetFlag;
		nlfoundobject = planepnt->AI.pGroundTarget;
		nlobjectflag = planepnt->AI.lGroundTargetFlag;
	}

	if(!(planepnt->AI.iAICombatFlags2 & AI_FAC))
	{
		if(AICCheckFACClose(planepnt))
		{
			nearfac = AICGetClosestFAC(planepnt);
			if(nearfac == -2)
			{
				if(pFACTarget)
				{
					if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), pFACTarget, iFACTargetType))
					{
						usefactarget = 1;
						pass = 0;
						foundobject = pFACTarget;
						objectflag = iFACTargetType;
						nlfoundobject = pFACTarget;
						nlobjectflag = iFACTargetType;
						nlfoundrange = 1;
						nltargbearing = 0;
						nltargpitch = 0;
					}
				}
			}
			else
			{
				if(Planes[nearfac].AI.pGroundTarget)
				{
					if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), Planes[nearfac].AI.pGroundTarget, Planes[nearfac].AI.lGroundTargetFlag))
					{
						usefactarget = 1;
						pass = 0;
						foundobject = Planes[nearfac].AI.pGroundTarget;
						objectflag = Planes[nearfac].AI.lGroundTargetFlag;
						nlfoundobject = Planes[nearfac].AI.pGroundTarget;
						nlobjectflag = Planes[nearfac].AI.lGroundTargetFlag;
						nlfoundrange = 1;
						nltargbearing = 0;
						nltargpitch = 0;
					}
				}
			}
		}
	}

	while(pass--)
	{
		checkobject = **checklist++;

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject) && (planepnt->AI.iSide != GDConvertGrndSide(checkobject)))
			{
				widthok = 0;
				heightok = 0;
				visualok = 0;
				dx = checkobject->Position.X - planepnt->WorldPosition.X;
				dy = checkobject->Position.Y - planepnt->WorldPosition.Y;
				dz = checkobject->Position.Z - planepnt->WorldPosition.Z;
				if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
				{
					tdist = QuickDistance(dx, dz);
					if(tdist < nlfoundrange)
					{
						offangle = atan2(-dx, -dz) * 57.2958;
	//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
						toffangle = offangle - radarangleyaw;
						offangle = AICapAngle(offangle);

						toffangle = AICapAngle(toffangle);

						if(fabs(toffangle) < widthangle)
						{
							widthok = 1;
						}

	#if 0  //  Don't think we need to check pitch for ground radar
						offpitch = (atan2(dy, tdist) * 57.2958);
	//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
						toffpitch = offpitch - radaranglepitch;
						toffpitch = AICapAngle(toffpitch);

						if(fabs(toffpitch) < heightangle)  //  Plane in radar area
						{
							heightok = 1;
						}
	#else
						offpitch = (atan2(dy, tdist) * 57.2958);
	//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
						toffpitch = offpitch - radaranglepitch;
						toffpitch = AICapAngle(toffpitch);
						heightok = 1;
	#endif

						if(tdist < visualrange)
						{
							//  May want to do pitch and heading check for this
							visualok = 1;
						}

						if((widthok && heightok) || (visualok))
						{
	//						AIUpdateRadarThreat(planepnt, checkplane, (widthok && heightok));
							//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
							//  If was previous target, keep lock regardless of chance.
							if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) && (leadplane->Status & PL_AI_DRIVEN))
							{
								dx = checkobject->Position.X - planepnt->AI.WayPosition.X;
								dz = checkobject->Position.Z - planepnt->AI.WayPosition.Z;
								if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS))
								{
									inCASarea = 1;
								}
								else
								{
									inCASarea = 0;
								}
							}
							else
							{
								inCASarea = 1;
							}


							if((!lockedorg) && (inCASarea))
							{
								cnt_obj_flag = GROUNDOBJECT;
								if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), checkobject, cnt_obj_flag))
								{
									foundobject = checkobject;
									foundrange = tdist;
									objectflag = GROUNDOBJECT;
									nlfoundobject = checkobject;
									nlfoundrange = tdist;
									nlobjectflag = GROUNDOBJECT;
									targbearing = nltargbearing = offangle;
									targpitch = nltargpitch = offpitch;
								}
								else
								{
									foundobject = checkobject;
									foundrange = tdist;
									objectflag = GROUNDOBJECT;
									targbearing = offangle;
									targpitch = offpitch;
								}

								if((orgtarg_id == checkobject) && (orgtarg_flag == GROUNDOBJECT) && (planepnt->AI.iAIFlags2 & AIKEEPTARGET))
								{
									foundobject = checkobject;
									foundrange = tdist;
									objectflag = GROUNDOBJECT;
									targbearing = offangle;
									targpitch = offpitch;
									lockedorg = 1;
								}
							}
						}
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	for(vehiclepnt = MovingVehicles; ((vehiclepnt <= LastMovingVehicle) && (!((planepnt->AI.iAICombatFlags2 & AI_FAC) && (planepnt->AI.pGroundTarget))) && (!usefactarget)); vehiclepnt ++)
	{
		if(vehiclepnt->iShipType)
		{
			pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
			isship = 1;
			if(pshiptype->lShipType & SHIP_TYPE_SUBMARINE)
			{
				isship = 3;
			}
		}
		else
		{
			isship = 0;
		}

		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))) && ((!(vehiclepnt->Status & (VL_INVISIBLE))) || ((isship & 2) && (planepnt->AI.iAICombatFlags2 & (AI_ANTI_SHIP|AI_ANTI_SUB)))) && (planepnt->AI.iSide != vehiclepnt->iSide) && ((!(planepnt->AI.iAICombatFlags2 & (AI_ANTI_SHIP|AI_ANTI_SUB))) || ((planepnt->AI.iAICombatFlags2 & (AI_ANTI_SHIP)) && isship) || ((planepnt->AI.iAICombatFlags2 & (AI_ANTI_SUB)) && isship == 3)))
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = vehiclepnt->WorldPosition.X - planepnt->WorldPosition.X;
			dy = vehiclepnt->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = vehiclepnt->WorldPosition.Z - planepnt->WorldPosition.Z;
			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < nlfoundrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					toffangle = offangle - radarangleyaw;
					offangle = AICapAngle(offangle);

					toffangle = AICapAngle(toffangle);
					if(fabs(toffangle) < widthangle)
					{
						widthok = 1;
					}

					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					heightok = 1;

					if(tdist < visualrange)
					{
						//  May want to do pitch and heading check for this
						visualok = 1;
					}

					if((widthok && heightok) || (visualok))
					{
//						AIUpdateRadarThreat(planepnt, checkplane, (widthok && heightok));
						//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
						//  If was previous target, keep lock regardless of chance.
						if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) && (leadplane->Status & PL_AI_DRIVEN))
						{
							dx = vehiclepnt->WorldPosition.X - planepnt->AI.WayPosition.X;
							dz = vehiclepnt->WorldPosition.Z - planepnt->AI.WayPosition.Z;
							if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS))
							{
								inCASarea = 1;
							}
							else
							{
								inCASarea = 0;
							}
						}
						else
						{
							inCASarea = 1;
						}


						if((!lockedorg) && (inCASarea))
						{
							cnt_obj_flag = MOVINGVEHICLE;
							if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), vehiclepnt, cnt_obj_flag))
							{
								foundobject = vehiclepnt;
								foundrange = tdist;
								objectflag = MOVINGVEHICLE;
								nlfoundobject = vehiclepnt;
								nlfoundrange = tdist;
								nlobjectflag = MOVINGVEHICLE;
								targbearing = nltargbearing = offangle;
								targpitch = nltargpitch = offpitch;
							}
							else
							{
								foundobject = vehiclepnt;
								foundrange = tdist;
								objectflag = MOVINGVEHICLE;
								targbearing = offangle;
								targpitch = offpitch;
							}

							if((orgtarg_id == vehiclepnt) && (orgtarg_flag == MOVINGVEHICLE) && (planepnt->AI.iAIFlags2 & AIKEEPTARGET))
							{
								foundobject = vehiclepnt;
								foundrange = tdist;
								objectflag = MOVINGVEHICLE;
								targbearing = offangle;
								targpitch = offpitch;
								lockedorg = 1;
							}
						}
					}
				}
			}
		}
	}

	if(foundobject == NULL)
	{
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.lGroundTargetFlag = 0;
		if(planepnt->AI.iAIFlags1 & AIENGAGED)
		{
			planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
		}
		else
		{
			planepnt->AI.lRadarDelay = 10000;  //  10 seconds until next sweep
		}
		return;
	}

	if(nlfoundobject != NULL)
	{
		planepnt->AI.pGroundTarget = nlfoundobject;
		planepnt->AI.lGroundTargetFlag = nlobjectflag;
		targbearing = nltargbearing;
		targpitch = nltargpitch;
		foundrange = nlfoundrange;
		objectflag = nlobjectflag;
	}
	else
	{
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.lGroundTargetFlag = 0;
	}

	if((planepnt->AI.iAICombatFlags2 & AI_FAC) && (planepnt->AI.pGroundTarget) && ((orgtarg_id == NULL) || (planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET)) && (!(planepnt->AI.iAICombatFlags2 & AI_FAC_JUST_LOOK)))
	{
		AICReportFACTarget(planepnt - Planes, PlayerPlane - Planes, (planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET), (planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET));
	}

	if((planepnt->AI.pGroundTarget) && ((orgtarg_id == NULL) || (planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET)) && (AICheckFACSmoke(planepnt)) && (!(planepnt->AI.iAICombatFlags2 & AI_FAC_JUST_LOOK)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_FAC_REDO_TARGET);

		if((planepnt->AI.iAIFlags2 & AI_SEAD_COVER) || (planepnt->AI.iAICombatFlags2 & AI_ESCORT_CAS))
		{
			if(planepnt->AI.Behaviorfunc == AIFlyEscort)
			{
				planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK|AI_RETURN_SEAD_COVER);
			}
			else
			{
				planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK);
			}
		}

		if(planepnt->AI.LinkedPlane == NULL)
		{
			AITransferCASLead(planepnt);
		}

		AIDelayCASRadar(AIGetLeader(planepnt));

		if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
		{
			checkobject = (BasicInstance *)planepnt->AI.pGroundTarget;
			planepnt->AI.WayPosition.X = checkobject->Position.X;
			planepnt->AI.WayPosition.Y = checkobject->Position.Y + (500 * FTTOWU);
			planepnt->AI.WayPosition.Z = checkobject->Position.Z;
//			planepnt->AI.TargetPos = planepnt->AI.WayPosition;
			planepnt->AI.TargetPos = checkobject->Position;
		}
		else if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
			planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
			planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y + (500 * FTTOWU);
			planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
//			planepnt->AI.TargetPos = planepnt->AI.WayPosition;
			planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
		}
		else
		{
			return;
		}

		dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
		dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx, dz);
		offpitch = (atan2(dy, tdist) * 57.2958);

		if(offpitch > -20)
		{
			if(planepnt->AI.iAICombatFlags2 & AI_FAC)
			{
				planepnt->AI.Behaviorfunc = AIStraightRocketAttack;
				attacktype = 4;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
				attacktype = 1;
			}
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIAwayFromTarget;
			attacktype = 2;
		}

		if(!(planepnt->AI.iAICombatFlags2 & AI_FAC))
		{
			AIDetermineBestBomb(planepnt, planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag);
		}

		if((planepnt->AI.cNumWeaponRelease > 0) && (!(planepnt->AI.iAICombatFlags2 & AI_FAC)))
		{
	 		AICWingCASAttackMsg(planepnt - Planes);
		}
		NetPutCASData(planepnt, attacktype);
	}
	else if((planepnt->AI.pGroundTarget) && ((orgtarg_id == NULL) || (planepnt->AI.iAICombatFlags2 & AI_FAC_REDO_TARGET)))
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_FAC_REDO_TARGET);

		DWORD	objectid;
		switch(planepnt->AI.lGroundTargetFlag)
		{
			case AIRCRAFT:
				objectid = (PlaneParams *)planepnt->AI.pGroundTarget - Planes;
				break;
			case MOVINGVEHICLE:
			case SHIP:
				objectid = (MovingVehicleParams *)planepnt->AI.pGroundTarget - MovingVehicles;
				break;
			case GROUNDOBJECT:
				checkobject = (BasicInstance *)planepnt->AI.pGroundTarget;
				objectid = checkobject->SerialNumber;
				break;
		}
		NetPutGenericMessageObjectPoint(planepnt, GMOP_GROUND_TARG, planepnt->AI.lGroundTargetFlag, objectid);
	}

	planepnt->AI.fRadarAngleYaw = targbearing;
	planepnt->AI.fRadarAnglePitch = targpitch;

	toffangle = targbearing - AIConvertAngleTo180Degree(planepnt->Heading);
	toffangle = AICapAngle(toffangle);
	if(fabs(toffangle) > 60)
	{
		toffangle = (toffangle < 0)? AIConvertAngleTo180Degree(planepnt->Heading) - 60 : AIConvertAngleTo180Degree(planepnt->Heading) + 60;
		toffangle = AICapAngle(toffangle);
		planepnt->AI.fRadarAngleYaw = toffangle;
	}
	else
	{
		planepnt->AI.fRadarAngleYaw = targbearing;
	}

	toffpitch = targpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
	toffpitch = AICapAngle(toffpitch);
	if(fabs(toffpitch) > 60)
	{
		toffpitch = (toffpitch < 0)? AIConvertAngleTo180Degree(planepnt->Pitch) - 60 : AIConvertAngleTo180Degree(planepnt->Pitch) + 60;
		toffpitch = AICapAngle(toffpitch);
		planepnt->AI.fRadarAnglePitch = toffpitch;
	}
	else
	{
		planepnt->AI.fRadarAnglePitch = targpitch;
	}

	planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
}
#endif

//**************************************************************************************
void AISEADSearch(PlaneParams *planepnt)
{
  	BasicInstance *checkobject;
	MovingVehicleParams *vehiclepnt;
  	void *foundobject = NULL;
  	void *radarfoundobject = NULL;
  	void *nlfoundobject = NULL;
  	void *nlradarfoundobject = NULL;
	long robjectflag;
	long objectflag;
	long nlobjectflag;
	long rnlobjectflag;
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	float radarfoundrange;
	float nlfoundrange;
	float nlradarfoundrange;
	float offangle, toffangle, toffpitch, offpitch;
	float targbearing, nltargbearing, rtargbearing, nlrtargbearing;
	float targpitch, nltargpitch, rtargpitch, nlrtargpitch;
	int widthok, heightok, visualok;
  	void *orgtarg_id;
  	long orgtarg_flag;
	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
	long cnt_obj_flag;
	float rangeCAS;
	int inCASarea;
	PlaneParams *leadplane = AIGetLeader(planepnt);
	int pass, startpass;
	GDRadarData *radardat;
	int hasbombs = -1;
	int nlfrmult, nlrfrmult, frmult, rfrmult;
	int attacktype = 0;
	int cnt;
	int attackspch = 0;

	nlfrmult = nlrfrmult = frmult = rfrmult = 0;

	if(planepnt->AI.pGroundTarget)
	{
		return;
	}

	planepnt->AI.lRadarDelay -= DeltaTicks;
	if(planepnt->AI.lRadarDelay > 0)
	{
		return;
	}

	orgtarg_id = planepnt->AI.pGroundTarget;
	orgtarg_flag = planepnt->AI.lGroundTargetFlag;

	if(orgtarg_id == NULL)
	{
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	}

	visualrangenm = AI_VISUAL_RANGE_NM / 2;
	visualrange = visualrangenm * NMTOWU;
	rangenm = 40;  //  Get this from database SRE

	rangeCAS = planepnt->AI.iVar1 * NMTOWU;
	if(!(leadplane->Status & PL_AI_DRIVEN))
	{
		rangenm = planepnt->AI.iVar1;
	}

	if(planepnt->AI.iAIFlags2 & (AI_SEAD_COVER))
	{
		if(rangeCAS < (20.0f * NMTOWU))
		{
			rangeCAS = 20.0f * NMTOWU;
		}
		if(rangenm < 20)
		{
			rangenm = 20;
		}
	}

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	radarfoundrange = radarrange;
	foundobject = NULL;
	radarfoundobject = NULL;
	nlfoundrange = radarrange;
	nlradarfoundrange = radarrange;
	nlfoundobject = NULL;
	nlradarfoundobject = NULL;
	widthangle = 20;	//  might get from database
	heightangle = 20;	//  might get from database

	toffangle = planepnt->AI.fRadarAngleYaw - AIConvertAngleTo180Degree(planepnt->Heading);
	toffangle = AICapAngle(toffangle);
	if(fabs(toffangle) > 60)
	{
		radarangleyaw = (toffangle < 0)? AIConvertAngleTo180Degree(planepnt->Heading) - 60 : AIConvertAngleTo180Degree(planepnt->Heading) + 60;
		radarangleyaw = AICapAngle(radarangleyaw);
	}
	else
	{
		radarangleyaw = planepnt->AI.fRadarAngleYaw;
	}

	toffpitch = planepnt->AI.fRadarAnglePitch - AIConvertAngleTo180Degree(planepnt->Pitch);
	toffpitch = AICapAngle(toffpitch);
	if(fabs(toffpitch) > 60)
	{
		radaranglepitch = (toffpitch < 0)? AIConvertAngleTo180Degree(planepnt->Pitch) - 60 : AIConvertAngleTo180Degree(planepnt->Pitch) + 60;
		radaranglepitch = AICapAngle(radaranglepitch);
	}
	else
	{
		radaranglepitch = planepnt->AI.fRadarAnglePitch;
	}

	if(!(planepnt->AI.iAIFlags1 & AIRADARON))
	{
		foundrange = visualrange;
		nlfoundrange = visualrange;
	}


	startpass = 0;
	if(planepnt->AI.pPaintedBy)
	{
		if(planepnt->AI.iPaintedByType == GROUNDOBJECT)
		{
			checkobject = (BasicInstance *)planepnt->AI.pPaintedBy;

			dx = checkobject->Position.X - planepnt->AI.WayPosition.X;
			dz = checkobject->Position.Z - planepnt->AI.WayPosition.Z;
			if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS) && (InstanceIsBombable(checkobject)))
			{
				startpass = 999;

				dx = checkobject->Position.X - planepnt->WorldPosition.X;
				dy = checkobject->Position.Y - planepnt->WorldPosition.Y;
				dz = checkobject->Position.Z - planepnt->WorldPosition.Z;

				tdist = QuickDistance(dx, dz);
				offangle = atan2(-dx, -dz) * 57.2958;
				offangle = AICapAngle(offangle);

				offpitch = (atan2(dy, tdist) * 57.2958);

				foundobject = checkobject;
				objectflag = GROUNDOBJECT;
				radarfoundobject = checkobject;
				robjectflag = GROUNDOBJECT;
				foundrange = tdist;
				radarfoundrange = tdist;
				nlfoundobject = checkobject;
				nlobjectflag = GROUNDOBJECT;
				nlradarfoundobject = checkobject;
				rnlobjectflag = GROUNDOBJECT;
				nlfoundrange = tdist;
				nlradarfoundrange = tdist;
				targbearing = nltargbearing = rtargbearing = nlrtargbearing = offangle;
				targpitch = nltargpitch = rtargpitch = nlrtargpitch = offpitch;
			}
		}
		else if(planepnt->AI.iPaintedByType == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)planepnt->AI.pPaintedBy;

			dx = vehiclepnt->WorldPosition.X - planepnt->AI.WayPosition.X;
			dz = vehiclepnt->WorldPosition.Z - planepnt->AI.WayPosition.Z;
			if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
			{
				startpass = 999;

				dx = vehiclepnt->WorldPosition.X - planepnt->WorldPosition.X;
				dy = vehiclepnt->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = vehiclepnt->WorldPosition.Z - planepnt->WorldPosition.Z;

				tdist = QuickDistance(dx, dz);
				offangle = atan2(-dx, -dz) * 57.2958;
				offangle = AICapAngle(offangle);

				offpitch = (atan2(dy, tdist) * 57.2958);

				foundobject = vehiclepnt;
				radarfoundobject = vehiclepnt;
				foundrange = tdist;
				radarfoundrange = tdist;
				nlfoundobject = vehiclepnt;
				nlradarfoundobject = vehiclepnt;
				nlfoundrange = tdist;
				nlradarfoundrange = tdist;
				targbearing = nltargbearing = rtargbearing = nlrtargbearing = offangle;
				targpitch = nltargpitch = rtargpitch = nlrtargpitch = offpitch;
				robjectflag = MOVINGVEHICLE;
				objectflag = MOVINGVEHICLE;
				nlobjectflag = MOVINGVEHICLE;
				rnlobjectflag = MOVINGVEHICLE;
			}
		}
	}

	for(pass = startpass; pass < 2; pass ++)
	{
		if(pass == 0)
		{
	  		checkobject = (BasicInstance *)AllInfoProviders;
		}
		else
		{
	  		checkobject = (BasicInstance *)AllAAWeapons;
			nlfrmult = nlrfrmult = frmult = rfrmult = 1;
		}

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject) && (GDConvertGrndSide(checkobject) != planepnt->AI.iSide))
			{
				widthok = 0;
				heightok = 0;
				visualok = 0;
				dx = checkobject->Position.X - planepnt->WorldPosition.X;
				dy = checkobject->Position.Y - planepnt->WorldPosition.Y;
				dz = checkobject->Position.Z - planepnt->WorldPosition.Z;
				if(((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange)) || ((fabs(dx) < nlradarfoundrange) && (fabs(dz) < nlradarfoundrange)))
				{
					tdist = QuickDistance(dx, dz);
					if((tdist < nlfoundrange) || (tdist < nlradarfoundrange))
					{
						offangle = atan2(-dx, -dz) * 57.2958;
	//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
						toffangle = offangle - radarangleyaw;
						offangle = AICapAngle(offangle);

						toffangle = AICapAngle(toffangle);
						if(fabs(toffangle) < widthangle)
						{
							widthok = 1;
						}

	#if 0  //  Don't think we need to check pitch for ground radar
						offpitch = (atan2(dy, tdist) * 57.2958);
	//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
						toffpitch = offpitch - radaranglepitch;
						toffpitch = AICapAngle(toffpitch);
						if(fabs(toffpitch) < heightangle)  //  Plane in radar area
						{
							heightok = 1;
						}
	#else
						offpitch = (atan2(dy, tdist) * 57.2958);
	//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
						toffpitch = offpitch - radaranglepitch;
						toffpitch = AICapAngle(toffpitch);
						heightok = 1;
	#endif

						if(tdist < visualrange)
						{
							//  May want to do pitch and heading check for this
							visualok = 1;
						}

						if((checkobject->Family == FAMILY_INFOPROVIDER) || (checkobject->Family == FAMILY_AAWEAPON))
						{
							radardat = GDGetRadarData(checkobject);
							if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
							{
								visualok = 1;
							}
							else
							{
								if(hasbombs == -1)
								{
									hasbombs = 0;
									for(int i=0; ((i<MAX_F18E_STATIONS) && (!hasbombs)); i++)
   									{
										if(planepnt->WeapLoad[i].Count > 0)
										{
											int Type = pDBWeaponList[planepnt->WeapLoad[i].WeapIndex].iWeaponType;
											switch(Type)
											{
												case DUMB_AG:
												case GUIDED_AG:
												case AG_MISSILE:
												case WEAPON_TYPE_ANTI_SHIP_MISSILE:
												case CLUSTER:
													if(pDBWeaponList[planepnt->WeapLoad[i].WeapIndex].iSeekerType != 11)
													{
														hasbombs = 1;
													}
												break;
											}
										}
									}
								}
								if(!hasbombs)
								{
									widthok = heightok = visualok = 0;
								}
							}
						}


						if((widthok && heightok) || (visualok))
						{
	//						AIUpdateRadarThreat(planepnt, checkplane, (widthok && heightok));
							//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
							//  If was previous target, keep lock regardless of chance.
							if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) && (leadplane->Status & PL_AI_DRIVEN))
							{
								if(!(planepnt->AI.iAIFlags2 & (AI_SEAD_COVER)))
								{
									dx = checkobject->Position.X - planepnt->AI.WayPosition.X;
									dz = checkobject->Position.Z - planepnt->AI.WayPosition.Z;
								}

								if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS))
								{
									inCASarea = 1;
								}
								else
								{
									inCASarea = 0;
								}
							}
							else
							{
								inCASarea = 1;
							}

							if((!lockedorg) && (inCASarea))
							{
								cnt_obj_flag = 0;
								if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), checkobject, cnt_obj_flag))
								{
									if(AIRadarOnCheck(checkobject))
									{
										if((tdist * (nlrfrmult + 1)) < nlradarfoundrange)
										{
											foundobject = checkobject;
											objectflag = GROUNDOBJECT;
											radarfoundobject = checkobject;
											robjectflag = GROUNDOBJECT;
											foundrange = tdist;
											radarfoundrange = tdist;
											nlfoundobject = checkobject;
											nlobjectflag = GROUNDOBJECT;
											nlradarfoundobject = checkobject;
											rnlobjectflag = GROUNDOBJECT;
											nlfoundrange = tdist;
											nlradarfoundrange = tdist;
											targbearing = nltargbearing = rtargbearing = nlrtargbearing = offangle;
											targpitch = nltargpitch = rtargpitch = nlrtargpitch = offpitch;
											nlfrmult = nlrfrmult = frmult = rfrmult = 0;
										}
									}
									else
									{
										if((tdist * (nlfrmult + 1)) < nlfoundrange)
										{
											foundobject = checkobject;
											objectflag = GROUNDOBJECT;
											foundrange = tdist;
											nlfoundobject = checkobject;
											nlobjectflag = GROUNDOBJECT;
											nlfoundrange = tdist;
											targbearing = nltargbearing = offangle;
											targpitch = nltargpitch = offpitch;
											nlfrmult = frmult = 0;
										}
									}
								}
								else
								{
									if(AIRadarOnCheck(checkobject))
									{
										if((tdist * (rfrmult + 1)) < radarfoundrange)
										{
											foundobject = checkobject;
											objectflag = GROUNDOBJECT;
											radarfoundobject = checkobject;
											robjectflag = GROUNDOBJECT;
											foundrange = tdist;
											radarfoundrange = tdist;
											targbearing = rtargbearing = offangle;
											targpitch = rtargpitch = offpitch;
											frmult = rfrmult = 0;
										}
									}
									else
									{
										if((tdist * (frmult + 1)) < foundrange)
										{
											foundobject = checkobject;
											objectflag = GROUNDOBJECT;
											foundrange = tdist;
											targbearing = offangle;
											targpitch = offpitch;
											frmult = 0;
										}
									}
								}

								if((orgtarg_id == checkobject) && (orgtarg_flag == 0) && (planepnt->AI.iAIFlags2 & AIKEEPTARGET))
								{
									foundobject = checkobject;
									objectflag = GROUNDOBJECT;
									radarfoundobject = checkobject;
									robjectflag = GROUNDOBJECT;
									foundrange = tdist;
									radarfoundrange = tdist;
									targbearing = rtargbearing = offangle;
									targpitch = rtargpitch = offpitch;
									lockedorg = 1;
									frmult = rfrmult = 0;
								}
							}
						}
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	for(vehiclepnt = MovingVehicles; ((vehiclepnt <= LastMovingVehicle) && (startpass < 100)); vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))) && (planepnt->AI.iSide != vehiclepnt->iSide))
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = vehiclepnt->WorldPosition.X - planepnt->WorldPosition.X;
			dy = vehiclepnt->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = vehiclepnt->WorldPosition.Z - planepnt->WorldPosition.Z;
			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < nlfoundrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					toffangle = offangle - radarangleyaw;
					offangle = AICapAngle(offangle);

				    toffangle = AICapAngle(toffangle);
					if(fabs(toffangle) < widthangle)
					{
						widthok = 1;
					}

					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					heightok = 1;

					if(tdist < visualrange)
					{
						//  May want to do pitch and heading check for this
						visualok = 1;
					}

//					radardat = &vehiclepnt->RadarWeaponData;
//					if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
					if(VGDRadarOn(vehiclepnt))
					{
						visualok = 1;
					}
					else
					{
						if(hasbombs == -1)
						{
							hasbombs = 0;
							for(int i=0; ((i<MAX_F18E_STATIONS) && (!hasbombs)); i++)
   							{
								if(planepnt->WeapLoad[i].Count > 0)
								{
									int Type = pDBWeaponList[planepnt->WeapLoad[i].WeapIndex].iWeaponType;
									switch(Type)
									{
										case DUMB_AG:
										case GUIDED_AG:
										case AG_MISSILE:
										case WEAPON_TYPE_ANTI_SHIP_MISSILE:
										case CLUSTER:
											if(pDBWeaponList[planepnt->WeapLoad[i].WeapIndex].iSeekerType != 11)
											{
												hasbombs = 1;
											}
										break;
									}
								}
							}
						}
						if(!hasbombs)
						{
							widthok = heightok = visualok = 0;
						}
					}

					if((widthok && heightok) || (visualok))
					{
//						AIUpdateRadarThreat(planepnt, checkplane, (widthok && heightok));
						//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
						//  If was previous target, keep lock regardless of chance.
						if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) && (leadplane->Status & PL_AI_DRIVEN))
						{
							if(!(planepnt->AI.iAIFlags2 & (AI_SEAD_COVER)))
							{
								dx = vehiclepnt->WorldPosition.X - planepnt->AI.WayPosition.X;
								dz = vehiclepnt->WorldPosition.Z - planepnt->AI.WayPosition.Z;
							}
							if((fabs(dx) < rangeCAS) && (fabs(dz) < rangeCAS))
							{
								inCASarea = 1;
							}
							else
							{
								inCASarea = 0;
							}
						}
						else
						{
							inCASarea = 1;
						}


						if((!lockedorg) && (inCASarea))
						{
							cnt_obj_flag = MOVINGVEHICLE;
							if(!AICheckIfAlreadyGroundTarget(planepnt, AIGetLeader(planepnt), vehiclepnt, cnt_obj_flag))
							{
//								if(vehiclepnt->RadarWeaponData.lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
								if(VGDRadarOn(vehiclepnt))
								{
									foundobject = vehiclepnt;
									objectflag = MOVINGVEHICLE;
									radarfoundobject = vehiclepnt;
									robjectflag = MOVINGVEHICLE;
									foundrange = tdist;
									radarfoundrange = tdist;
									nlfoundobject = vehiclepnt;
									nlobjectflag = MOVINGVEHICLE;
									nlradarfoundobject = vehiclepnt;
									rnlobjectflag = MOVINGVEHICLE;
									nlfoundrange = tdist;
									nlradarfoundrange = tdist;
									targbearing = nltargbearing = rtargbearing = nlrtargbearing = offangle;
									targpitch = nltargpitch = rtargpitch = nlrtargpitch = offpitch;
								}
								else
								{
									foundobject = vehiclepnt;
									objectflag = MOVINGVEHICLE;
									foundrange = tdist;
									nlfoundobject = vehiclepnt;
									nlobjectflag = MOVINGVEHICLE;
									nlfoundrange = tdist;
									targbearing = nltargbearing = offangle;
									targpitch = nltargpitch = offpitch;
								}
							}
							else
							{
//								if(vehiclepnt->RadarWeaponData.lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
								if(VGDRadarOn(vehiclepnt))
								{
									foundobject = vehiclepnt;
									objectflag = MOVINGVEHICLE;
									radarfoundobject = vehiclepnt;
									robjectflag = MOVINGVEHICLE;
									foundrange = tdist;
									radarfoundrange = tdist;
									targbearing = rtargbearing = offangle;
									targpitch = rtargpitch = offpitch;
								}
								else
								{
									foundobject = vehiclepnt;
									objectflag = MOVINGVEHICLE;
									foundrange = tdist;
									targbearing = offangle;
									targpitch = offpitch;
								}
							}

							if((orgtarg_id == vehiclepnt) && (orgtarg_flag == MOVINGVEHICLE) && (planepnt->AI.iAIFlags2 & AIKEEPTARGET))
							{
								foundobject = vehiclepnt;
								objectflag = MOVINGVEHICLE;
								radarfoundobject = vehiclepnt;
								robjectflag = MOVINGVEHICLE;
								foundrange = tdist;
								radarfoundrange = tdist;
								targbearing = rtargbearing = offangle;
								targpitch = rtargpitch = offpitch;
								lockedorg = 1;
							}
						}
					}
				}
			}
		}
	}


	if(foundobject == NULL)
	{
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.lGroundTargetFlag = 0;
		if(planepnt->AI.iAIFlags1 & AIENGAGED)
		{
			planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
		}
		else
		{
			planepnt->AI.lRadarDelay = 10000;  //  10 seconds until next sweep
		}
		return;
	}

	if((!(planepnt->AI.iAICombatFlags1 & AI_NAKED)) && (nlradarfoundobject == NULL) && (radarfoundobject == NULL))
	{
		AIC_SEAD_SpikeCall(planepnt, NULL, NULL);
	}

	if(nlradarfoundobject)
	{
		planepnt->AI.pGroundTarget = nlradarfoundobject;
		planepnt->AI.lGroundTargetFlag = rnlobjectflag;
		targbearing = nlrtargbearing;
		targpitch = nlrtargpitch;
		foundrange = nlradarfoundrange;
	}
	else if(nlfoundobject != NULL)
	{
		planepnt->AI.pGroundTarget = nlfoundobject;
		planepnt->AI.lGroundTargetFlag = nlobjectflag;
		targbearing = nltargbearing;
		targpitch = nltargpitch;
		foundrange = nlfoundrange;
	}
	else
	{
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.lGroundTargetFlag = 0;
	}

	if((planepnt->AI.iAICombatFlags2 & AI_ESCORT_STAY) && (planepnt->AI.pGroundTarget))
	{
		if((!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AG)) && (iAI_ROE[planepnt->AI.iSide] == 2) && (planepnt->AI.Behaviorfunc == AIFlyEscort))
		{
			if((!planepnt->AI.LinkedPlane) || (planepnt->AI.LinkedPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			{
				planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				leadplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				AIUpdateGroupFlags(leadplane - Planes, AI_ASK_PLAYER_ATTACK, 0, 0);
			}
			else if(!(planepnt->AI.iAIFlags1 & AI_ASK_PLAYER_ATTACK))
			{
				AICWingReadyToAttackMsg(planepnt - Planes);
				AIUpdateGroupFlags(leadplane - Planes, AI_ASK_PLAYER_ATTACK, 0, 0);
#if 0
				if(g_Settings.gp.nType == GP_TYPE_CASUAL)
				{
					iEasyBombVar = 1;
					lEasyBomberTimer = 60000;
				}
#endif
			}
			else if(iEasyAAVar == 2)
			{
				planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				leadplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				AIUpdateGroupFlags(leadplane - Planes, AI_ASK_PLAYER_ATTACK, 0, 0);
			}
		}
	}

	if((planepnt->AI.pGroundTarget) && (orgtarg_id == NULL) && (!(planepnt->AI.iAICombatFlags2 & AI_ESCORT_STAY)))
	{
		if(planepnt->AI.Behaviorfunc == AIFlyEscort)
		{
			planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK|AI_RETURN_SEAD_COVER);
		}
		else
		{
			planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK);
		}

		if(planepnt->AI.LinkedPlane == NULL)
		{
			AITransferCASLead(planepnt);
		}

		AIDelayCASRadar(AIGetLeader(planepnt));

		DBWeaponType *weapon;

		if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
		{
			checkobject = (BasicInstance *)planepnt->AI.pGroundTarget;
			planepnt->AI.WayPosition.X = checkobject->Position.X;
			planepnt->AI.WayPosition.Y = checkobject->Position.Y + (500 * FTTOWU);
			planepnt->AI.WayPosition.Z = checkobject->Position.Z;
			planepnt->AI.TargetPos = checkobject->Position;
			if(checkobject->Family == FAMILY_AAWEAPON)
			{
				weapon = GetWeaponPtr(checkobject);
				if(weapon)
				{
					if(weapon->iWeaponType == WEAPON_TYPE_GUN)
					{
						if(weapon->iSeekerType == 10)
						{
							attackspch = 1;
						}
						else
						{
							attackspch = 3;
						}
					}
					else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
					{
						attackspch = 2;
					}
				}
			}
		}
		else if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
			planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
			planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y + (500 * FTTOWU);
			planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
			planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
			if(vehiclepnt->iShipType)
			{
				if(pDBShipList[vehiclepnt->iVDBIndex].lSizeType & (SHIP_SIZE_VSMALL|SHIP_SIZE_SMALL))
				{
					attackspch = 8;
				}
				else
				{
					attackspch = 5;
				}
			}
			else
			{
				for(cnt = 0; cnt < 2; cnt ++)
				{
					if(!cnt)
					{
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
					}
					else
					{
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
					}
					if(weapon)
					{
						if((weapon->iWeaponType == WEAPON_TYPE_GUN) && (attackspch < 3))
						{
							attackspch = 3;
						}
						else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
						{
							attackspch = 4;
						}
					}
				}
			}
		}
		else
		{
			return;
		}

		dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
		dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx, dz);
		offpitch = (atan2(dy, tdist) * 57.2958);

		if(offpitch > -20)
		{
			planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
			attacktype = 1;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIAwayFromTarget;
			attacktype = 2;
		}
		AIDetermineBestBomb(planepnt, planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag);

		if(planepnt->AI.cNumWeaponRelease > 0)
		{
#if 0
			if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
			{
		 		AICWingCASAttackMsg(planepnt - Planes);
			}
			else if(planepnt->AI.LinkedPlane)
			{
				if(planepnt->AI.LinkedPlane->Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN))
				{
			 		AICWingCASAttackMsg(planepnt - Planes);
				}
				else
				{
					AICSEADAttackMsg(planepnt - Planes, attackspch);
				}
			}
			else
			{
				AICSEADAttackMsg(planepnt - Planes, attackspch);
			}
#else
			AICSEADAttackMsg(planepnt - Planes, attackspch);
#endif
		}
		else if((leadplane == planepnt) || (!(leadplane->AI.iAIFlags2 & (AI_SEAD_ATTACK))))
		{
			if(AIAllGroupAGWinchester(leadplane))
			{
				attacktype = 3;
				planepnt->AI.iAIFlags2 &= ~(AI_CAS_ATTACK|AI_SEAD_ATTACK);
				planepnt->AI.pGroundTarget = NULL;
				planepnt->AI.Behaviorfunc = AIFlyFormation;
				AINextWayPoint(planepnt);
				AICSEADWinchesterMsg(planepnt - Planes, -1);
			}
		}
		NetPutCASData(planepnt, attacktype);
	}

	planepnt->AI.fRadarAngleYaw = targbearing;
	planepnt->AI.fRadarAnglePitch = targpitch;

	toffangle = targbearing - AIConvertAngleTo180Degree(planepnt->Heading);
	toffangle = AICapAngle(toffangle);
	if(fabs(toffangle) > 60)
	{
		toffangle = (toffangle < 0)? AIConvertAngleTo180Degree(planepnt->Heading) - 60 : AIConvertAngleTo180Degree(planepnt->Heading) + 60;
		toffangle = AICapAngle(toffangle);
		planepnt->AI.fRadarAngleYaw = toffangle;
	}
	else
	{
		planepnt->AI.fRadarAngleYaw = targbearing;
	}

	toffpitch = targpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
	toffpitch = AICapAngle(toffpitch);
	if(fabs(toffpitch) > 60)
	{
		toffpitch = (toffpitch < 0)? AIConvertAngleTo180Degree(planepnt->Pitch) - 60 : AIConvertAngleTo180Degree(planepnt->Pitch) + 60;
		toffpitch = AICapAngle(toffpitch);
		planepnt->AI.fRadarAnglePitch = toffpitch;
	}
	else
	{
		planepnt->AI.fRadarAnglePitch = targpitch;
	}

	planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
}

//**************************************************************************************
void AITransferCASLead(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	PlaneParams *newlead;

	leadplane = AIGetLeader(planepnt);
	newlead = NULL;

	if(leadplane->AI.wingman >= 0)
	{
		AIUpdateCASLead(&Planes[leadplane->AI.wingman], planepnt, &newlead);
	}

	if(leadplane->AI.nextpair >= 0)
	{
		AIUpdateCASLead(&Planes[leadplane->AI.nextpair], planepnt, &newlead);
	}
	planepnt->AI.LinkedPlane = newlead;
}

//**************************************************************************************
void AIUpdateCASLead(PlaneParams *planepnt, PlaneParams *oldlead, PlaneParams **newlead)
{
	int linkeddead = 0;

	if(planepnt->AI.LinkedPlane)
	{
		if(planepnt->AI.LinkedPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			linkeddead = 1;
		}
	}

	if((planepnt->AI.LinkedPlane == oldlead) && (planepnt->AI.Behaviorfunc == oldlead->AI.Behaviorfunc) && ((linkeddead) || (!(planepnt->AI.iAIFlags2 & (AIMISSILEDODGE|AIDEFENSIVE)))))
	{
		if(*newlead == NULL)
		{
			planepnt->AI.LinkedPlane = NULL;
			*newlead = planepnt;
			AIUpdateLinkedLead(AIGetLeader(planepnt), oldlead, newlead);
			return;
		}
		else
		{
			planepnt->AI.LinkedPlane = *newlead;
		}
	}

	if((planepnt->AI.wingman >= 0) && (*newlead == NULL))
	{
		AIUpdateCASLead(&Planes[planepnt->AI.wingman], oldlead, newlead);
	}

	if((planepnt->AI.nextpair >= 0) && (*newlead == NULL))
	{
		AIUpdateCASLead(&Planes[planepnt->AI.nextpair], oldlead, newlead);
	}

}

//**************************************************************************************
void AIUpdateLinkedLead(PlaneParams *planepnt, PlaneParams *oldlead, PlaneParams **newlead)
{
	if(planepnt->AI.LinkedPlane == oldlead)
	{
		planepnt->AI.LinkedPlane = *newlead;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIUpdateLinkedLead(&Planes[planepnt->AI.wingman], oldlead, newlead);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIUpdateLinkedLead(&Planes[planepnt->AI.nextpair], oldlead, newlead);
	}

}

//**************************************************************************************
int AICheckIfAlreadyGroundTarget(PlaneParams *searchplane, PlaneParams *planepnt, void *object_id, long object_flag, int check_stays)
{
	BasicInstance *searchtarg, *planetarg;
	float tempdist = (800 * FTTOWU);

	if((searchplane != planepnt) && ((planepnt->AI.iAICombatFlags2 & AI_ESCORT_STAY) || (check_stays)))
	{
//		if((planepnt->AI.lGroundTargetID == object_id) && (planepnt->AI.lGroundTargetFlag == object_flag))
		if((planepnt->AI.pGroundTarget == object_id) && (planepnt->AI.lGroundTargetFlag == object_flag))
		{
			return(1);
		}
		if(planepnt->AI.pGroundTarget)
		{
			searchtarg = (BasicInstance *)object_id;
			planetarg = (BasicInstance *)planepnt->AI.pGroundTarget;
			if(fabs(searchtarg->Position.X - planetarg->Position.X) < tempdist)
			{
				if(fabs(searchtarg->Position.Z - planetarg->Position.Z) < tempdist)
				{
					return(1);
				}
			}
		}
	}
	if(planepnt->AI.wingman >= 0)
	{
		if(AICheckIfAlreadyGroundTarget(searchplane, &Planes[planepnt->AI.wingman], object_id, object_flag))
		{
			return(1);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AICheckIfAlreadyGroundTarget(searchplane, &Planes[planepnt->AI.nextpair], object_id, object_flag));
	}
	return(0);
}

//**************************************************************************************
void AIDelayCASRadar(PlaneParams *planepnt)
{
	if(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))
	{
		planepnt->AI.lRadarDelay += 10000;  //  10 seconds until next sweep
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIDelayCASRadar(&Planes[planepnt->AI.wingman]);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIDelayCASRadar(&Planes[planepnt->AI.nextpair]);
	}

}


//**************************************************************************************
void AIAwayFromTarget(PlaneParams *planepnt)
{
	double offangle, offpitch;
	float tdist;
	float dx, dy, dz;
	double rollval;
	int aiswitch = 0;
	float releasetime;
	float multimod;
	float turnrate;
	float weaponspacing = AI_RIPPLE_SPREAD;
	int iweapontype;

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 0) - 180;

	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

	offangle = AICapAngle(offangle);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	offpitch = (atan2(dy, tdist) * 57.2958);

	if(planepnt->AI.OrgBehave != AIFlyToPopPoint)
	{
		if((pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE)
				|| (pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE))
		{
			if((tdist * WUTONM) > 10)
			{
				aiswitch = 1;
			}
		}
		else if(pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType == WEAPON_TYPE_AGROCKET)
		{
			if((tdist * WUTONM) > 5)
			{
				aiswitch = 1;
			}
		}
		else
		{
			if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
			{
				AIUpdateAttackOnVehicle(planepnt);
			}

			if(planepnt->YawRate)
			{
				turnrate = 360.0f / planepnt->YawRate;
			}
			else
			{
				turnrate = 60.0f;
			}

			releasetime = AIVarTimeToRelease(planepnt, planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, -999, 1);

			weaponspacing = (float)pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iDamageRadius * 1.5f;
			multimod = (planepnt->AI.cNumWeaponRelease / 2) * (weaponspacing / planepnt->IfHorzVelocity);
			releasetime -= multimod;
			if(releasetime > 90.0f)
			{
				aiswitch = 1;
			}
		}
	}
	else if(offpitch > -15)
	{
		aiswitch = 1;
	}

	if(aiswitch)
	{
		iweapontype = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].iWeaponType;
		if(planepnt->AI.cActiveWeaponStation < 0)
		{
			AISetUpEgress(planepnt);
		}
		else if(planepnt->AI.OrgBehave)
		{
			planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			planepnt->AI.OrgBehave = NULL;
		}
		else if((iweapontype == WEAPON_TYPE_AG_MISSILE) || (iweapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE) || (iweapontype == WEAPON_TYPE_AGROCKET) || (iweapontype == WEAPON_TYPE_GUN))
		{
			planepnt->AI.Behaviorfunc = AILevelBomb;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
		}
	}
}

//**************************************************************************************
void AIGetMaxDamage(BasicInstance *instance, float *sustainable, float *total)
{
	StructureInstance *struct_instance;
	InfoProviderInstance *info_instance;
	AAWeaponInstance *weapon_instance;
	StructureFamily *struct_family;
	InfoProviderFamily *info_family;
	AAWeaponFamily *weapon_family;

	if(!instance)
	{
		*sustainable = 20.0f;
		*total = 150.0f;
		return;
	}

	switch(instance->Family)
	{
		case FAMILY_STRUCTURE :
			struct_instance = (StructureInstance *)instance;
			struct_family = struct_instance->Type;

			*sustainable = AbsorbableDamages[struct_family->Physicals.Damage.DamageTypes & DTE_SUS_DAMAGE];
			*total = TotalDamages[(struct_family->Physicals.Damage.DamageTypes & DTE_PERM_DAMAGE)>>8];
			break;

		case FAMILY_INFOPROVIDER :
			info_instance = (InfoProviderInstance *)instance;
			info_family = info_instance->Type;

			*sustainable = AbsorbableDamages[info_family->Physicals.Damage.DamageTypes & DTE_SUS_DAMAGE];
			*total = TotalDamages[(info_family->Physicals.Damage.DamageTypes & DTE_PERM_DAMAGE)>>8];
			break;

		case FAMILY_AAWEAPON :
			weapon_instance = (AAWeaponInstance *)instance;
			weapon_family = weapon_instance->Type;

			*sustainable = AbsorbableDamages[weapon_family->Physicals.Damage.DamageTypes & DTE_SUS_DAMAGE];
			*total = TotalDamages[(weapon_family->Physicals.Damage.DamageTypes & DTE_PERM_DAMAGE)>>8];
			break;
		default:
			break;
	}
	return;
}

//**************************************************************************************
DamageProportions *AIGetDamageRatio(BasicInstance *instance,DWORD damage_type)
{
	StructureInstance *struct_instance;
	InfoProviderInstance *info_instance;
	AAWeaponInstance *weapon_instance;
	DamageProportions *ratio;
	StructureFamily *struct_family;
	InfoProviderFamily *info_family;
	AAWeaponFamily *weapon_family;
	int target_type;

	if(!instance)
	{
		ratio = DamageMultipliers;
		return(ratio);
	}

	switch(instance->Family)
	{
		case FAMILY_STRUCTURE :
			struct_instance = (StructureInstance *)instance;
			struct_family = struct_instance->Type;

			target_type = struct_family->Physicals.Flags & DTE_TARGET_TYPE;
			if ((struct_instance->Basics.Flags & BI_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
				target_type = DTE_SOFT_TARGET;
			target_type |= (damage_type & DTE_TARGET_TYPE) << 2;

			ratio = &DamageMultipliers[target_type << 1];
			break;

		case FAMILY_INFOPROVIDER :
			info_instance = (InfoProviderInstance *)instance;
			info_family = info_instance->Type;

			target_type = info_family->Physicals.Flags & DTE_TARGET_TYPE;
			if ((info_instance->Basics.Flags & BI_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
				target_type = DTE_SOFT_TARGET;
			target_type |= (damage_type & DTE_TARGET_TYPE) << 2;

			ratio = &DamageMultipliers[target_type << 1];
			break;

		case FAMILY_AAWEAPON :
			weapon_instance = (AAWeaponInstance *)instance;
			weapon_family = weapon_instance->Type;

			target_type = weapon_family->Physicals.Flags & DTE_TARGET_TYPE;
			if ((weapon_instance->Basics.Flags & BI_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
				target_type = DTE_SOFT_TARGET;
			target_type |= (damage_type & DTE_TARGET_TYPE) << 2;

			ratio = &DamageMultipliers[target_type << 1];
			break;
		default:
			ratio = DamageMultipliers;
			break;
	}
	return(ratio);
}

#if 0
//**************************************************************************************
void AIDetermineBestBomb(PlaneParams *planepnt, void *pGroundTarget)
{
	int cnt;
	int bombtype;
	int onboard;
	int bestcnt = -1;
	int bestnumrel = 0;
	int bestrating = 999;
	DBWeaponType *pweapon_type;
	DamageProportions *ratio, *directratio;
	DWORD hit_value;
	float damagepercent = 0;
	float workpercent = 0;
	float sustainable = 0.0f;
	float sdam, tdam;
	float total = 200.0f;  //  default value if they don't find anything.  Value damage from a Mark 82 ish
	int tempval;
	int foundbombtype = 0;

	AIGetMaxDamage((BasicInstance *)pGroundTarget, &sustainable, &total);

	if(!((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP)))
	{
		if(!(planepnt->AI.iAIFlags2 & AI_BOMBED_PRIME))
		{
			planepnt->AI.iAIFlags2 |= AI_BOMBED_PRIME;
		}
		else
		{
			planepnt->AI.iAIFlags2 |= AI_BOMBED_SECOND;
		}
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(planepnt->WeapLoad[cnt].Count > 0)
		{
			pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
			bombtype = pweapon_type->iWeaponType;

			while(pweapon_type->lSubWeaponID)
			{
				tempval = AIGetWeaponIndex(pweapon_type->lSubWeaponID);
				if(tempval >= 0)
				{
					pweapon_type = &pDBWeaponList[tempval];
				}
				else
				{
					break;
				}
			}

			hit_value = (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];
			ratio = AIGetDamageRatio((BasicInstance *)pGroundTarget, hit_value);
			directratio = ratio + 1;

			if(bombtype == WEAPON_TYPE_DUMB_BOMB)
			{
				sdam = pweapon_type->iDamageValue * ratio->SustainablePercentage;
				tdam = sdam - sustainable;

				if (tdam < 0.0f)
					tdam = 0.0f;

				// then figure out how many points will go to total
				tdam += pweapon_type->iDamageValue * ratio->TotalPercentage;

				workpercent = tdam / total;

				if((bestrating > 100) && ((workpercent > 1.5f) || (workpercent > damagepercent)))
				{
					if((workpercent < 3.0f) || (damagepercent < 1.5f) || (damagepercent > workpercent))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 100;
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if(((workpercent > 1.5f) && (workpercent < 3.0f)) && ((damagepercent < 1.5f) || (damagepercent > workpercent)))
				{
					bestcnt = cnt;
					bestnumrel = 1;
					bestrating = 100;
					damagepercent = workpercent;
					foundbombtype = bombtype;
				}
			}
			else if(bombtype == WEAPON_TYPE_CLUSTER_BOMB)
			{
				sdam = pweapon_type->iDamageValue * ratio->SustainablePercentage;
				tdam = (sdam * 10) - sustainable;

				if (tdam < 0.0f)
					tdam = 0.0f;

				// then figure out how many points will go to total
				tdam += pweapon_type->iDamageValue * ratio->TotalPercentage;

				workpercent = (tdam * 10) / total;

				if((bestrating > 50) && ((workpercent > 1.5f) || (workpercent > damagepercent)))
				{
					if((workpercent < 3.0f) || (damagepercent < 1.5f) || (damagepercent > workpercent))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 50;
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if(((workpercent > 1.5f) && (workpercent < 3.0f)) && ((damagepercent < 1.5f) || (damagepercent > workpercent)))
				{
					bestcnt = cnt;
					bestnumrel = 1;
					bestrating = 50;
					damagepercent = workpercent;
					foundbombtype = bombtype;
				}
			}
			else if(bombtype == WEAPON_TYPE_GUIDED_BOMB)
			{
				sdam = pweapon_type->iDamageValue * directratio->SustainablePercentage;
				tdam = sdam - sustainable;

				if (tdam < 0.0f)
					tdam = 0.0f;

				// then figure out how many points will go to total
				tdam += pweapon_type->iDamageValue * directratio->TotalPercentage;

				workpercent = tdam / total;

				if((bestrating > 50) && ((workpercent > 1.5f) || (workpercent > damagepercent)))
				{
					if((workpercent < 3.0f) || (damagepercent < 1.5f) || (damagepercent > workpercent))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 50;
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if(((workpercent > 1.5f) && (workpercent < 3.0f)) && ((damagepercent < 1.5f) || (damagepercent > workpercent)))
				{
					bestcnt = cnt;
					bestnumrel = 1;
					bestrating = 50;
					damagepercent = workpercent;
					foundbombtype = bombtype;
				}
			}
			else if(bombtype == WEAPON_TYPE_AG_MISSILE)
			{
				sdam = pweapon_type->iDamageValue * directratio->SustainablePercentage;
				tdam = sdam - sustainable;

				if (tdam < 0.0f)
					tdam = 0.0f;

				// then figure out how many points will go to total
				tdam += pweapon_type->iDamageValue * directratio->TotalPercentage;

				workpercent = tdam / total;

				if((bestrating > 10) && ((workpercent > 1.5f) || (workpercent > damagepercent)))
				{
					if((workpercent < 3.0f) || (damagepercent < 1.5f) || (damagepercent > workpercent))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 10;
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if(((workpercent > 1.5f) && (workpercent < 3.0f)) && ((damagepercent < 1.5f) || (damagepercent > workpercent)))
				{
					bestcnt = cnt;
					bestnumrel = 1;
					bestrating = 10;
					damagepercent = workpercent;
					foundbombtype = bombtype;
				}
			}
		}
	}

	if(bestcnt != -1)
	{
//		bombtype = pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].iWeaponType;
		bombtype = foundbombtype;
		planepnt->AI.cActiveWeaponStation = bestcnt;
		onboard = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID);
		if(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))
		{
			if((bombtype == WEAPON_TYPE_GUIDED_BOMB) || (bombtype == WEAPON_TYPE_AG_MISSILE))
			{
				planepnt->AI.cNumWeaponRelease = 1;
			}
			else
			{
				if(onboard > 2)
				{
					planepnt->AI.cNumWeaponRelease = 2;
				}
				else
				{
					planepnt->AI.cNumWeaponRelease = 1;
				}
			}
		}
		else
		{
			if(bombtype == WEAPON_TYPE_AG_MISSILE)
			{
				planepnt->AI.cNumWeaponRelease = 1;
			}
			else if(bombtype == WEAPON_TYPE_GUIDED_BOMB)
			{
				if(((planepnt->AI.iAIFlags2 & (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) == (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) || (onboard > 3))
				{
					planepnt->AI.cNumWeaponRelease = 2;
				}
				else
				{
					planepnt->AI.cNumWeaponRelease = 1;
				}
			}
			else
			{
				if((planepnt->AI.iAIFlags2 & (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) == (AI_BOMBED_PRIME|AI_BOMBED_SECOND))
				{
					planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID);
				}
				else
				{
					planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID) / 2;
				}
			}
		}

		if(onboard < planepnt->AI.cNumWeaponRelease)
		{
			planepnt->AI.cNumWeaponRelease = onboard;
		}

		if(bombtype == WEAPON_TYPE_AG_MISSILE)
		{
			if(planepnt->AI.Behaviorfunc != AIAwayFromTarget)
			{
				if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
				{
					planepnt->AI.Behaviorfunc = AILevelBomb;
				}
				else
				{
					planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
				}
				planepnt->AI.lTimer1 = 60000 + ((rand() & 15) * 1000);
			}
		}
		return;
	}

	planepnt->AI.cActiveWeaponStation = -1;
	planepnt->AI.cNumWeaponRelease = 0;
	AISetUpEgress(planepnt);
	planepnt->AI.lRadarDelay += 30000;  //  10 seconds until next sweep
	return;
}
#else
//**************************************************************************************
void AIDetermineBestBomb(PlaneParams *planepnt, void *pGroundTarget, long groundtype)
{
	int cnt;
	int bombtype;
	int onboard;
	int bestcnt = -1;
	int bestnumrel = 0;
	int bestrating = 999;
	int temprating;
	DBWeaponType *pweapon_type;
	DamageProportions *ratio, *directratio;
	DWORD hit_value;
	float damagepercent = 0;
	float workpercent = 0;
	float sustainable = 0.0f;
	float sdam, tdam;
	float total = 0.0f;
	int tempval;
	int foundbombtype = 0;
	int longbest = AIIsGrndWeaponNear(planepnt);
	DBVehicleType *pvehicletype;
	DBShipType *pshiptype;
	MovingVehicleParams *vehiclepnt;
	int target_type;
	int issearcher = (planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK));    //  (AI_CAS_ATTACK|AI_SEAD_ATTACK|AI_SEAD_COVER)
	int allowguided, allowunguided, allowmissile, ignore_use_weapon;
	int droptype;
	int isship = 0;
	int issub = 0;
	int iscas = 0;
	int gradar = 0;
   	BasicInstance *checkobject;
	GDRadarData *radardat;

	if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) || (planepnt->AI.cUseWeapon == 0))
	{
		allowguided = allowunguided = allowmissile = 1;
		droptype = 0;
		ignore_use_weapon = 1;
	}
	else
	{
		ignore_use_weapon = 0;
		tempval = (planepnt->AI.cUseWeapon - 1) / 3;
		switch(tempval)
		{
			case 1:
				allowguided = 1;
				break;
			case 2:
				allowmissile = 1;
				break;
			default:
				allowunguided = 1;
				break;
		}

		droptype = (planepnt->AI.cUseWeapon - 1) % 3;
	}


	if(!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AG))
	{
		if(!pGroundTarget)
		{
			gradar = 0;
			sustainable = 20.0f;
			total = 150.0f;
		}
		else if(groundtype == GROUNDOBJECT)
		{
			AIGetMaxDamage((BasicInstance *)pGroundTarget, &sustainable, &total);

		  	checkobject = (BasicInstance *)pGroundTarget;
			if((checkobject->Family == FAMILY_INFOPROVIDER) || (checkobject->Family == FAMILY_AAWEAPON))
			{
				radardat = GDGetRadarData(checkobject);
				if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
				{
					gradar = 1;
				}
			}
		}
		else if(groundtype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
			if(VGDRadarOn(vehiclepnt))
			{
				gradar = 1;
			}

			// SCOTT FIX THIS
			if (!vehiclepnt->iShipType)
			{
				pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];
				sustainable = pvehicletype->iSustainedDamage;
				total = pvehicletype->iPermanentDamage;
			}
			else
			{
				pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
				sustainable = pshiptype->iSustainedDamage;
				total = pshiptype->iPermanentDamage;
				isship = 1;
				if(pshiptype->lShipType & SHIP_TYPE_SUBMARINE)
				{
					issub = 1;
					if(vehiclepnt->Status & VL_INVISIBLE)
					{
						issub |= 2;
					}
				}
			}
		}
		else
		{
#if 0
			planepnt->AI.cActiveWeaponStation = -1;
			planepnt->AI.cNumWeaponRelease = 0;
			AISetUpEgress(planepnt);
			planepnt->AI.lRadarDelay += 30000;  //  10 seconds until next sweep
			return;
#else
			gradar = 0;
			sustainable = 20.0f;
			total = 150.0f;
#endif
		}

		if(!((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP)))
		{
			if(!(planepnt->AI.iAIFlags2 & AI_BOMBED_PRIME))
			{
				planepnt->AI.iAIFlags2 |= AI_BOMBED_PRIME;
			}
			else
			{
				planepnt->AI.iAIFlags2 |= AI_BOMBED_SECOND;
			}
		}
		if(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))
		{
			iscas = 1;
			if(planepnt->AI.iAIFlags2 & (AI_SEAD_ATTACK))
			{
				iscas |= 2;
			}
		}

		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
				bombtype = pweapon_type->iWeaponType;

				while(pweapon_type->lSubWeaponID)
				{
					tempval = AIGetWeaponIndex(pweapon_type->lSubWeaponID);
					if(tempval >= 0)
					{
						pweapon_type = &pDBWeaponList[tempval];
					}
					else
					{
						break;
					}
				}

				hit_value = (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];
				if(groundtype == GROUNDOBJECT)
				{
					ratio = AIGetDamageRatio((BasicInstance *)pGroundTarget, hit_value);
				}
				else if(groundtype == MOVINGVEHICLE)
				{
					if(isship)
					{
						target_type = pshiptype->iArmorType;
					}
					else
					{
						target_type = pvehicletype->iArmorType;
					}
					if ((vehiclepnt->Status & VL_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
						target_type = DTE_SOFT_TARGET;
					target_type |= (hit_value & DTE_TARGET_TYPE) << 2;

					ratio = &DamageMultipliers[target_type << 1];
				}

				directratio = ratio + 1;

				if(((bombtype == WEAPON_TYPE_DUMB_BOMB) || (bombtype == WEAPON_TYPE_AGROCKET)) && (allowunguided) && (((issub != 3) && (pweapon_type->iSeekerType != 13)) || ((isship) && (pweapon_type->iSeekerType == 13))))
				{
					sdam = pweapon_type->iDamageValue * ratio->SustainablePercentage;
					tdam = sdam - sustainable;

					if (tdam < 0.0f)
						tdam = 0.0f;

					// then figure out how many points will go to total
					tdam += pweapon_type->iDamageValue * ratio->TotalPercentage;

					workpercent = tdam / total;

					if((bestrating >= 100) && ((workpercent >= 3.0f) || (workpercent > damagepercent)))
					{
						if(((workpercent < damagepercent) && (damagepercent > 3.0f)) || (damagepercent < 3.0f))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = 100;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
					else if(!longbest)
					{
						if(((workpercent < damagepercent) && (workpercent > 3.0f) && (damagepercent > 3.0f)) || ((damagepercent < 3.0f) && (workpercent > damagepercent)))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = 100;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
					if((!issearcher) && ((bestcnt == -1) || ((workpercent >= damagepercent) && (damagepercent < 1.0f))))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 100;
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if((bombtype == WEAPON_TYPE_CLUSTER_BOMB) && (allowunguided) && (issub != 3))
				{
					sdam = pweapon_type->iDamageValue * ratio->SustainablePercentage;
					tdam = (sdam * 10) - sustainable;

					if (tdam < 0.0f)
						tdam = 0.0f;

					// then figure out how many points will go to total
					tdam += pweapon_type->iDamageValue * ratio->TotalPercentage;

					workpercent = (tdam * 10) / total;

					if((bestrating >= 50) && ((workpercent >= 3.0f) || (workpercent > damagepercent)))
					{
						if(((workpercent < damagepercent) && (damagepercent > 3.0f)) || (damagepercent < 3.0f))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = 50;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
					else if(!longbest)
					{
						if(((workpercent < damagepercent) && (workpercent > 3.0f) && (damagepercent > 3.0f)) || ((damagepercent < 3.0f) && (workpercent > damagepercent)))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = 50;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
					if(((!issearcher) || (groundtype == MOVINGVEHICLE)) && ((bestcnt == -1) || ((workpercent >= damagepercent) && (damagepercent < 1.0f))))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 50;
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if((bombtype == WEAPON_TYPE_GUIDED_BOMB) && (allowguided) && (issub != 3))
				{
					sdam = pweapon_type->iDamageValue * directratio->SustainablePercentage;
					tdam = sdam - sustainable;

					if (tdam < 0.0f)
						tdam = 0.0f;

					// then figure out how many points will go to total
					tdam += pweapon_type->iDamageValue * directratio->TotalPercentage;

					workpercent = tdam / total;

					temprating = 50;

					if(pweapon_type->iSeekerType == 12)  //  GSP seeker
					{
						if(iscas)
						{
							temprating = 101;
						}
						else
						{
							temprating = 5;
						}
					}

					if((bestrating >= temprating) && ((workpercent >= 3.0f) || (workpercent > damagepercent)))
					{
						if(((workpercent < damagepercent) && (damagepercent > 3.0f)) || (damagepercent < 3.0f))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = temprating;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
					else if(!longbest)
					{
						if(((workpercent < damagepercent) && (workpercent > 3.0f) && (damagepercent > 3.0f)) || ((damagepercent < 3.0f) && (workpercent > damagepercent)))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = temprating;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
					if((!issearcher) && ((bestcnt == -1) || ((workpercent >= damagepercent) && (damagepercent < 1.0f))))
					{
						bestcnt = cnt;
						bestnumrel = 1;
						bestrating = 100;
						if(temprating > bestrating)
						{
							bestrating = temprating;
						}
						damagepercent = workpercent;
						foundbombtype = bombtype;
					}
				}
				else if(((bombtype == WEAPON_TYPE_AG_MISSILE) || ((bombtype == WEAPON_TYPE_ANTI_SHIP_MISSILE) && (isship))) && (allowmissile) && (issub != 3))
				{
					sdam = pweapon_type->iDamageValue * directratio->SustainablePercentage;
					tdam = sdam - sustainable;

					if (tdam < 0.0f)
						tdam = 0.0f;

					// then figure out how many points will go to total
					tdam += pweapon_type->iDamageValue * directratio->TotalPercentage;

					workpercent = tdam / total;

					temprating = 10;

					if(pweapon_type->iSeekerType == 12)  //  GSP seeker
					{
						if(iscas)
						{
							temprating = 101;
						}
						else
						{
							temprating = 5;
						}
					}
					else if(pweapon_type->iSeekerType == 11)  //  Anti Radar
					{
						if(gradar)
						{
							if(iscas & 2)
							{
								temprating = 5;
							}
							else
							{
								temprating = 60;
							}
						}
						else
						{
							temprating = 10000;
						}
					}

					if(temprating < 999)
					{
						if((bestrating >= temprating) && ((workpercent >= 3.0f) || (workpercent > damagepercent) || ((pweapon_type->iSeekerType == 11) && (iscas & 2))))
						{
							if(((workpercent < damagepercent) && (damagepercent > 3.0f)) || (damagepercent < 3.0f))
							{
								bestcnt = cnt;
								bestnumrel = 1;
								bestrating = temprating;
								damagepercent = workpercent;
								foundbombtype = bombtype;
							}
						}
						else if(!longbest)
						{
							if(((workpercent < damagepercent) && (workpercent > 3.0f) && (damagepercent > 3.0f)) || ((damagepercent < 3.0f) && (workpercent > damagepercent)))
							{
								bestcnt = cnt;
								bestnumrel = 1;
								bestrating = temprating;
								damagepercent = workpercent;
								foundbombtype = bombtype;
							}
						}
						if((!issearcher) && ((bestcnt == -1) || ((workpercent >= damagepercent) && (damagepercent < 1.0f))))
						{
							bestcnt = cnt;
							bestnumrel = 1;
							bestrating = temprating;
							damagepercent = workpercent;
							foundbombtype = bombtype;
						}
					}
				}
			}
		}

		if(bestcnt != -1)
		{
	//		bombtype = pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].iWeaponType;
			bombtype = foundbombtype;
			planepnt->AI.cActiveWeaponStation = bestcnt;
			onboard = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID);
			if(!ignore_use_weapon)
			{
				switch(droptype)
				{
					case 1:
						planepnt->AI.cNumWeaponRelease = onboard / 2;
						break;
					case 2:
						planepnt->AI.cNumWeaponRelease = 1;
						break;
					default:
						planepnt->AI.cNumWeaponRelease = onboard;
						break;
				}
			}
			else if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP))
			{
				if((bombtype == WEAPON_TYPE_AG_MISSILE) || (bombtype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
				{
					planepnt->AI.cNumWeaponRelease = 1;
				}
				else if(bombtype == WEAPON_TYPE_GUIDED_BOMB)
				{
					if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) && (!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))))
					{
						if(onboard > 2)
						{
							planepnt->AI.cNumWeaponRelease = 2;
						}
						else
						{
							planepnt->AI.cNumWeaponRelease = 1;
						}
					}
					else
					{
						planepnt->AI.cNumWeaponRelease = 1;
					}
				}
				else
				{
					if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) && (!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK))))
					{
						if((planepnt->AI.iAIFlags2 & (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) == (AI_BOMBED_PRIME|AI_BOMBED_SECOND))
						{
							planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID);
						}
						else if(AICGetBombWaypoint(planepnt - Planes, 2) == NULL)
						{
							planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID) / 2;
						}
						else
						{
							planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID) / 3;
						}
					}
					else if(onboard > 2)
					{
						planepnt->AI.cNumWeaponRelease = 2;
					}
					else
					{
						planepnt->AI.cNumWeaponRelease = 1;
					}
				}
			}
			else
			{
				if((bombtype == WEAPON_TYPE_AG_MISSILE) || (bombtype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
				{
					planepnt->AI.cNumWeaponRelease = 1;
				}
				else if(bombtype == WEAPON_TYPE_GUIDED_BOMB)
				{
					if(((planepnt->AI.iAIFlags2 & (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) == (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) || (onboard > 3))
					{
						planepnt->AI.cNumWeaponRelease = 2;
					}
					else
					{
						planepnt->AI.cNumWeaponRelease = 1;
					}
				}
				else
				{
					if((planepnt->AI.iAIFlags2 & (AI_BOMBED_PRIME|AI_BOMBED_SECOND)) == (AI_BOMBED_PRIME|AI_BOMBED_SECOND))
					{
						planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID);
					}
					else if((!(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)) && (AICGetBombWaypoint(planepnt - Planes, 2) == NULL))
					{
						planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID);
					}
					else
					{
						planepnt->AI.cNumWeaponRelease = AIGetNumWeaponIDOnBoard(planepnt, pDBWeaponList[planepnt->WeapLoad[bestcnt].WeapIndex].lWeaponID) / 2;
					}
				}
			}

			if(onboard < planepnt->AI.cNumWeaponRelease)
			{
				planepnt->AI.cNumWeaponRelease = onboard;
			}

			if((bombtype == WEAPON_TYPE_AG_MISSILE) || (bombtype == WEAPON_TYPE_ANTI_SHIP_MISSILE) || (bombtype == WEAPON_TYPE_AGROCKET) || (bombtype == WEAPON_TYPE_GUN))
			{
				if(planepnt->AI.Behaviorfunc != AIAwayFromTarget)
				{
					if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || (planepnt->AI.iAIFlags2 & (AI_SEAD_ATTACK|AI_CAS_ATTACK)))
					{
						planepnt->AI.Behaviorfunc = AILevelBomb;
					}
					else
					{
						planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
					}
					planepnt->AI.lTimer1 = 60000 + ((rand() & 15) * 1000);
				}
			}
			else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
			{
				if(planepnt->AI.Behaviorfunc != AIAwayFromTarget)
				{
					if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
					{
						planepnt->AI.Behaviorfunc = AILevelBomb;
					}
					else
					{
						planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
					}
				}
			}
			return;
		}
		else if(!ignore_use_weapon)
		{
			planepnt->AI.cUseWeapon = 0;
			AIDetermineBestBomb(planepnt, pGroundTarget, groundtype);
			return;
		}
		PlaneHasAGWeapons(planepnt);
	}

	planepnt->AI.cActiveWeaponStation = -1;
	planepnt->AI.cNumWeaponRelease = 0;
	AISetUpEgress(planepnt);
	planepnt->AI.lRadarDelay += 30000;  //  10 seconds until next sweep

	return;
}
#endif

//**************************************************************************************
int AIIsGrndWeaponNear(PlaneParams *planepnt)
{
  	BasicInstance *checkobject;
	FPointDouble Pos;
	float dx, dy, dz;
	float dangerrange = 20.0f * NMTOWU;

	if(planepnt->AI.pPaintedBy)
	{
		return(1);
	}

  	checkobject = (BasicInstance *)AllAAWeapons;

	if(planepnt->AI.TargetPos.X >= 0)
	{
		Pos = planepnt->AI.TargetPos;
	}
	else
	{
		Pos = planepnt->WorldPosition;
	}

	while(checkobject)
	{
//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
		//  if(object is alive and object is a valid target)
		if(InstanceIsBombable(checkobject) && (GDConvertGrndSide(checkobject) != planepnt->AI.iSide))
		{
			dx = checkobject->Position.X - Pos.X;
			dy = checkobject->Position.Y - Pos.Y;
			dz = checkobject->Position.Z - Pos.Z;
			if((fabs(dx) < dangerrange) && (fabs(dz) < dangerrange))
			{
				return(1);
			}
		}
		checkobject = checkobject->NextRelatedInstance;
	}
	return(0);
}

//**************************************************************************************
void AISelectGroupAttackWeapons(PlaneParams *planepnt, BombTarget *pActionBombTarget)
{
	PlaneParams *leadplane;
	int attacknum = 0;

	leadplane = AIGetLeader(planepnt);
	AIGetAttackWeapon(planepnt, pActionBombTarget);
}

//**************************************************************************************
void AIGetAttackWeapon(PlaneParams *planepnt, BombTarget *pActionBombTarget)
{
	int placeingroup;
	int bombtarget;
	int cnt;
	BasicInstance *walker;

	placeingroup = planepnt->AI.iAIFlags1 & AIFLIGHTNUMS;

	bombtarget = placeingroup;

	if(pActionBombTarget->FlightAttackList[bombtarget].dwSerialNumber == 0)
	{
		cnt = 0;
		while((pActionBombTarget->FlightAttackList[cnt].dwSerialNumber) && (cnt < 8))
		{
			cnt ++;
		}

		if(cnt)
		{
			bombtarget = placeingroup % cnt;
		}
		else
		{
			bombtarget = 0;
		}
	}

	walker = FindInstance(AllInstances, pActionBombTarget->FlightAttackList[bombtarget].dwSerialNumber);
	planepnt->AI.pGroundTarget = walker;
	planepnt->AI.lGroundTargetFlag = GROUNDOBJECT;
//	AIDetermineBestBomb(planepnt, pActionBombTarget->FlightAttackList[bombtarget].lType);
	AIDetermineBestBomb(planepnt, planepnt->AI.pGroundTarget);

	if(walker)
	{
		if((walker->Position - planepnt->AI.WayPosition) > (50.0f * NMTOWU))
		{
			walker = NULL;
		}
	}

	if(walker)
	{
		planepnt->AI.WayPosition.X = walker->Position.X;
	//	planepnt->AI.WayPosition.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
		planepnt->AI.WayPosition.Z = walker->Position.Z;
		planepnt->AI.TargetPos = walker->Position;
		planepnt->AI.lGroundTargetFlag = GROUNDOBJECT;
	}
	else
	{
		FPoint temppos;
		temppos.X = pActionBombTarget->FlightAttackList[bombtarget].X;
		temppos.Y = pActionBombTarget->FlightAttackList[bombtarget].Y;
		temppos.Z = pActionBombTarget->FlightAttackList[bombtarget].Z;

		if(((pActionBombTarget->FlightAttackList[bombtarget].X > 0) || (pActionBombTarget->FlightAttackList[bombtarget].Y > 0) || (pActionBombTarget->FlightAttackList[bombtarget].Z > 0)) && ((temppos - planepnt->AI.WayPosition) < (50.0f * NMTOWU)))
		{
			planepnt->AI.WayPosition.X = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].X);
		//	planepnt->AI.WayPosition.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
			planepnt->AI.WayPosition.Z = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Z);
			planepnt->AI.TargetPos.X = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].X);
			planepnt->AI.TargetPos.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
			planepnt->AI.TargetPos.Z = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Z);
			planepnt->AI.lGroundTargetFlag = 0;
		}
		else
		{
			planepnt->AI.WayPosition.X = ConvertWayLoc(planepnt->AI.CurrWay->lWPx);
		//	planepnt->AI.WayPosition.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
			planepnt->AI.WayPosition.Z = ConvertWayLoc(planepnt->AI.CurrWay->lWPz);
			planepnt->AI.TargetPos.X = ConvertWayLoc(planepnt->AI.CurrWay->lWPx);
			planepnt->AI.TargetPos.Y = ConvertWayLoc(planepnt->AI.CurrWay->lWPy);
			planepnt->AI.TargetPos.Z = ConvertWayLoc(planepnt->AI.CurrWay->lWPz);
			planepnt->AI.lGroundTargetFlag = 0;
		}
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER))
	{
		if(((planepnt->AI.WayPosition.Y - planepnt->AI.TargetPos.Y) < (2000.0f * FTTOWU)) && ((planepnt->AI.Behaviorfunc != AIFlyToPopPoint) && (planepnt->AI.Behaviorfunc != AIFlyToLoftPoint) && (planepnt->AI.Behaviorfunc != AIGroupFlyToPopPoint) && (planepnt->AI.Behaviorfunc != AIGroupFlyToLoftPoint)))
		{
			planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000.0f * FTTOWU);
		}
	}
	else
	{
		if(((planepnt->AI.WayPosition.Y - planepnt->AI.TargetPos.Y) < (50.0f * FTTOWU)) && ((planepnt->AI.Behaviorfunc != AIFlyToPopPoint) && (planepnt->AI.Behaviorfunc != AIFlyToLoftPoint) && (planepnt->AI.Behaviorfunc != AIGroupFlyToPopPoint) && (planepnt->AI.Behaviorfunc != AIGroupFlyToLoftPoint)))
		{
			planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (50.0f * FTTOWU);
		}
	}

#if 0
	if(planepnt->AI.wingman >= 0)
	{
		AIGetAttackWeapon(&Planes[planepnt->AI.wingman], pActionBombTarget);
	}
	if(planepnt->AI.nextpair >= 0)
	{
		AIGetAttackWeapon(&Planes[planepnt->AI.nextpair], pActionBombTarget);
	}
#endif
}

//**************************************************************************************
int AISelectNextWeaponStation(PlaneParams *planepnt)
{
	long weaponid;
	int cnt;

	weaponid = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].lWeaponID;
	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(weaponid == pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID)
		{
			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				planepnt->AI.cActiveWeaponStation = cnt;
				return(1);
			}
		}
	}
	return(0);
}

//**************************************************************************************
int AIGetNumWeaponIDOnBoard(PlaneParams *planepnt, long weaponid)
{
	int cnt;
	int returnval = 0;

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(weaponid == pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID)
		{
			returnval += planepnt->WeapLoad[cnt].Count;
		}
	}
	return(returnval);
}

//**************************************************************************************
int AIIsSEADPlane(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;
	int cnt;
	DBWeaponType *pweapon_type;
	int weapontype;
	int orbit = 0;
	int bomb = 0;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if(AIActions[actcnt].ActionID == ACTION_SEAD)
			{
				return(1);
			}
			if(AIActions[actcnt].ActionID == ACTION_ORBIT)
			{
				orbit = 1;
			}
			if(AIActions[actcnt].ActionID == ACTION_BOMB_TARGET)
			{
				bomb = 1;
			}
			if(AIActions[actcnt].ActionID == ACTION_ESCORT)
			{
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					if(planepnt->WeapLoad[cnt].Count > 0)
					{
						pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
						weapontype = pweapon_type->iWeaponType;
						if((weapontype == WEAPON_TYPE_DUMB_BOMB) || (weapontype == WEAPON_TYPE_CLUSTER_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE) || (weapontype == WEAPON_TYPE_AG_MISSILE))
						{
							return(1);
						}
					}
				}
				return(0);
			}
		}
		lookway ++;
	}

	if(orbit && !bomb)
	{
		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
				weapontype = pweapon_type->iWeaponType;
				if((weapontype == WEAPON_TYPE_DUMB_BOMB) || (weapontype == WEAPON_TYPE_CLUSTER_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE) || (weapontype == WEAPON_TYPE_AG_MISSILE))
				{
					return(1);
				}
			}
		}
	}

//	if((pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 17) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 18) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 27))
//	{
//		return(1);
//	}
	//  else if(check loadout for Anti-radation or bombs)
	//  {
	//		return(1);
	//  }
	return(0);
}

//**************************************************************************************
void AIReplaceCAPorSweep(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	void (*behaviorid)(PlaneParams *planetype);

	foundrange = (80 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->AI.iAIFlags2 & AIINVISIBLE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)))
		{
			if((checkplane->OnGround == 2) && (!(checkplane->AI.iAICombatFlags1 & AI_READY_5)))
			{
				checkplane ++;
				continue;
			}

			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < foundrange)
				{
					foundrange = tdist;
					foundplane = checkplane - Planes;
				}
			}
		}
		checkplane ++;
	}

	if(foundplane == -1)
	{
		return;
	}
	else
	{
		if(MultiPlayer)
		{
			NetPutGenericMessage2(checkplane, GM2_REPLACE_CAP, (BYTE)(planepnt - Planes));
		}

		checkplane = &Planes[foundplane];

		if(checkplane->AI.iAIFlags2 & AIINVISIBLE)
		{
			behaviorid = checkplane->AI.Behaviorfunc;
			checkplane->AI.iAIFlags2 &= ~AIINVISIBLE;
			checkplane->Status |= PL_ACTIVE;
			checkplane->AI.CurrWay = planepnt->AI.CurrWay;
			checkplane->AI.startwpts = planepnt->AI.startwpts;
			checkplane->AI.numwaypts = planepnt->AI.numwaypts;
			checkplane->AI.Behaviorfunc = AIFlyFormation;
			AISetUpNewWayPoint(checkplane);
			if((checkplane->OnGround == 2) || (behaviorid == CTWaitingForLaunch))
			{
				checkplane->AI.iAIFlags2 |= AIINVISIBLE;
				checkplane->Status &= ~(PL_ACTIVE);
				checkplane->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
				if(checkplane->AI.Behaviorfunc != CTWaitingForLaunch)
				{
					checkplane->AI.OrgBehave = checkplane->AI.Behaviorfunc;
					checkplane->AI.Behaviorfunc = CTWaitingForLaunch;
				}
				checkplane->AI.lTimer2 = -99999;
//				CTAddToLaunchQueue(checkplane);
				PlaneParams *wingwalk;
				wingwalk = checkplane;
				while(wingwalk)
				{
					if(wingwalk->AI.wingman >= 0)
					{
						Planes[wingwalk->AI.wingman].AI.lTimer2 = -99999;
						Planes[wingwalk->AI.wingman].AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
					}

					if(wingwalk->AI.nextpair >= 0)
					{
						wingwalk = &Planes[wingwalk->AI.nextpair];
						wingwalk->AI.lTimer2 = -99999;
						wingwalk->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
					}
					else
					{
						wingwalk = NULL;
					}
				}
			}
			else if(checkplane->OnGround)
			{
				if(planepnt->AI.Behaviorfunc == AIFlyTakeOff)
				{
					checkplane->AI.OrgBehave = checkplane->AI.Behaviorfunc;
					checkplane->AI.Behaviorfunc = AIFlyTakeOff;
				}
			}
		}
	}
}

//**************************************************************************************
void AICoveredPlaneLanding(PlaneParams *planepnt, PlaneParams *linkedplane)
{

	//  Eventually check to see if planepnt is supposed to do more after escorting.  SRE
	if(!AICheckForWayPointsAfterEscort(planepnt, linkedplane - Planes))
	{
		//  Eventually get information for closest field.  SRE.
		AISetPlaneLandingFlyToField(planepnt, NULL);
	}
}

//**************************************************************************************
MBWayPoints	*AIGetWaypointAfterLastEscort(PlaneParams *planepnt, int escortplanenum, int *numleft)
{
	MBWayPoints *lookway;
	int waycnt;
	int startact, endact, actcnt;
	int numwpts;
	MBWayPoints *returnway;
	Escort	*pEscortAction;
	int returnnum;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	returnway = NULL;
	returnnum = numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				if(pEscortAction->iPlaneNum == escortplanenum)
				{
					if(waycnt == (numwpts - 1))
					{
						return(NULL);
					}
					returnway = lookway + 1;
					*numleft = returnnum - 1;
				}
			}
		}
		lookway ++;
		returnnum --;
	}

	return(returnway);
}

//**************************************************************************************
int AICheckForWayPointsAfterEscort(PlaneParams *planepnt, int escortplanenum)
{
	MBWayPoints *lookway;
	int numwaypts;
	int planecnt = 0;

	lookway = AIGetWaypointAfterLastEscort(planepnt, escortplanenum, &numwaypts);
	if(lookway == NULL)
	{
		return(0);
	}

	planepnt->AI.numwaypts = numwaypts;
	planepnt->AI.CurrWay = lookway;
	planepnt->AI.Behaviorfunc = AIFlyFormation;
	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		AIUpdateCruiseFormation(planepnt, &planecnt);
	}
	AISetUpNewWayPoint(planepnt);

	return(1);
}

//**************************************************************************************
int AICheckForEscortsArriving(PlaneParams *planepnt)
{
	int searchtype = planepnt->AI.iVar1;
	PlaneParams *checkplane = &Planes[0];
	float dx, dy, dz, tdist;
	int donesearchtype = 2;

	if(planepnt->AI.lTimer2 >= 0)
	{
		return(0);
	}

	planepnt->AI.lTimer2 = 60000;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && ((checkplane->Status & PL_AI_DRIVEN) || (searchtype == 3)))
		{
			if(!(checkplane->Status & PL_AI_DRIVEN))
			{
				if(!AISeeIfPossibleEscort(checkplane, planepnt))
				{
					checkplane ++;
					continue;
				}
				donesearchtype = 3;
			}
			else if(!(((checkplane->AI.Behaviorfunc == AIFlyToEscortMeeting) || (checkplane->AI.Behaviorfunc == AIFlyEscort)) && (checkplane->AI.LinkedPlane == planepnt)))
			{
				checkplane ++;
				continue;
			}

			if((!(checkplane->Status & PL_AI_DRIVEN)) || ((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1)))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz) * WUTOFT;

				if((!(checkplane->Status & PL_AI_DRIVEN)) && (planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER))
				{
					if(tdist < (NMTOFT * 15))
					{
						return(1);
					}
				}
				else if(tdist < (NMTOFT * 5))
				{
					return(1);
				}
			}
		}
		checkplane ++;
	}

	planepnt->AI.iVar1 = donesearchtype;

	return(0);
}

//**************************************************************************************
int AISeeIfPossibleEscort(PlaneParams *planepnt, PlaneParams *linkedplane)
{
	MBWayPoints *lookway;
	int waycnt;
	int startact, endact, actcnt;
	int numwpts;
	Escort	*pEscortAction;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				if(pEscortAction->iPlaneNum == (linkedplane - Planes))
				{
					return(1);
				}
			}
		}
		lookway ++;
	}

	return(0);
}

//**************************************************************************************
void AISARWait(PlaneParams *planepnt)
{
	int isflying = 0;
	float tdist;

#if 0
	if(AICheckSARConditions(planepnt))
	{
		planepnt->AI.Behaviorfunc = AIFlyFormation;
//		AINextWayPoint(planepnt);
		AINextWayPointNoActions(planepnt);
	}
#endif
	AICheckSARCondition(planepnt);

	if(planepnt->AI.iAICombatFlags2 & AI_CARRIER_REL)
	{
		if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			isflying = 1;
			MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			if(planepnt->AI.iAICombatFlags2 & AI_UPDATE_POSITION)
			{
				planepnt->AI.WayPosition = carrier->WorldPosition;
				AIOffsetFromPoint(&planepnt->AI.WayPosition, AIConvertAngleTo180Degree(carrier->Heading + 0x4000), 0, 100.0F * FTTOWU, -1.5f * NMTOWU);
				tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);

				if(AIInNextRange(planepnt, tdist * WUTOFT, 0.25f * NMTOFT, 25.0f))
				{
					planepnt->AI.iAICombatFlags2 &= ~AI_UPDATE_POSITION;
				}
				else if(tdist > NMTOWU)
				{
					MBAdjustAIThrust(planepnt, 100.0f, 1);
				}
				else
				{
					MBAdjustAIThrust(planepnt, 50.0f, 1);
				}
			}
			else
			{
				double dx2 = (carrier->WorldPosition.X) - planepnt->WorldPosition.X;
				double dz2 = (carrier->WorldPosition.Z) - planepnt->WorldPosition.Z;
				double tdist2 = QuickDistance(dx2, dz2) * WUTONM;
				float offangle;

				tdist2 -= 1.5f;
				if(fabs(tdist2) > 0.5f)
				{
					planepnt->AI.WayPosition = carrier->WorldPosition;
					AIOffsetFromPoint(&planepnt->AI.WayPosition, AIConvertAngleTo180Degree(carrier->Heading + 0x4000), 0, 100.0f * FTTOWU, -1.5f * NMTOWU);
					tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);
					planepnt->AI.iAICombatFlags2 |= AI_UPDATE_POSITION;
				}
				else
				{
					planepnt->DesiredSpeed = carrier->fSpeedFPS;
					offangle = (AIConvertAngleTo180Degree(carrier->Heading - planepnt->Heading));
					planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
				}
			}
		}
	}

	if(!isflying)
	{
		planepnt->DesiredSpeed = 0;
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
	}

	AIUpdateFormation(planepnt);
}

//**************************************************************************************
void AICheckSARCondition(PlaneParams *planepnt)
{
	int endsar = 0;

	if(planepnt->AI.iVar1 == 0)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			endsar = 1;
		}
	}
	else
	{
		endsar = AICheckEventFlagStatus(planepnt->AI.iVar1);
	}

	if(endsar)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		AINextWayPointNoActions(planepnt);
		AISetUpWayPointActions(planepnt, planepnt->AI.CurrWay);
	}
}

//**************************************************************************************
void AIFlyToSARPoint(PlaneParams *planepnt)
{
	float tdist;
	float offangle;
	float dx, dy, dz;
	PlaneParams *leadplane;

	leadplane = AIGetLeader(planepnt);

	if(leadplane->AI.lTimer2 >= 0)
	{
		AISARWait(planepnt);
		return;
	}

	if(planepnt->OnGround)
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		if(planepnt->OnGround != 2)
		{
			planepnt->AI.Behaviorfunc = AIFlyTakeOff;
		}
		else
		{
			planepnt->AI.Behaviorfunc = CTWaitingForLaunch;
		}
	}

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
		{
			MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
			planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
			planepnt->AI.TargetPos.Y += (1000 * FTTOWU);
		}

		tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos) * WUTOFT;

		if(tdist < 3600)
		{

			if(planepnt->AI.lDesiredSpeed != 20)
			{
				AIC_SAR_Approach_Msg(planepnt - Planes);
				AI_SAR_Mark(planepnt);
			}
			planepnt->AI.lDesiredSpeed = 20;
			MBAdjustAIThrust(planepnt, 20, 1);
		}
		else
		{
			planepnt->AI.lDesiredSpeed = pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
			MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed, 1);
		}

		if(tdist < 400)
		{
			planepnt->AI.iAIFlags2 |= AILANDING;
			offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.TargetPos, &tdist, &dx ,&dy, &dz, 0);
			planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(offangle);
			planepnt->AI.lTimer2 = 150000;
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AISARPickUp);
			if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
			{
				AICheckForVehicleSARPickup(planepnt);
			}
			else
			{
				AICheckForChutePickup(planepnt);
			}
			SetLandingGearUpDown(planepnt,LOWER_LANDING_GEAR);
		}
		AIUpdateFormation(planepnt);
	}
	else
	{
		planepnt->AI.lVar2 = leadplane->AI.lVar2;
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AISARPickUp(PlaneParams *planepnt)
{
	float tdist, desiredspeed;
	float dy;
	FPointDouble fieldpos;
	TakeOffActionType ActionTakeOff;
	FPointDouble fptemp;
	ANGLE runwayheading;


	planepnt->DesiredRoll = 0;
//	dy =  (planepnt->WorldPosition.Y - planepnt->Type->GearDownHeight) - planepnt->HeightAboveGround;
	dy =  planepnt->HeightAboveGround - planepnt->Type->GearDownHeight;
	dy *= WUTOFT;
	planepnt->DesiredPitch = (dy > 0) ? 0xC000 : 0x4000;
	if(planepnt->OnGround)
	{
		planepnt->DesiredPitch = 0;
	}
	tdist = fabs(dy);

	if(((tdist <= 0.5f) || (dy < 0)) && (planepnt->OnGround == 0))
	{
		planepnt->OnGround = 1;
		planepnt->WorldPosition.Y = 0;
	}

	if(planepnt->OnGround)
	{
		if(planepnt->AI.lDesiredSpeed == 20)
		{
			AIC_SAR_Land_Msg(planepnt - Planes);
			planepnt->AI.lDesiredSpeed = pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
		}
		planepnt->DesiredPitch = 0;
		planepnt->DesiredSpeed = 0;
		planepnt->DesiredRoll = 0;
		planepnt->AI.AOADegPitch = 0;
		planepnt->Alpha = 0;
	}
	else
	{
		desiredspeed = tdist / 5;
		if((desiredspeed * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) > 40)
		{
			desiredspeed = 40 / (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
		}
		else if(desiredspeed < 3.0f)
		{
			desiredspeed = 3.0f;
		}
		planepnt->DesiredSpeed = desiredspeed;
	}

	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		if(planepnt->AI.lTimer2 < 0)
		{
			AIReleaseSARCover(planepnt);

			fptemp.SetValues(-1.0f,-1.0f,-1.0f);

			if(!AICheckPlaneLandingFlyToCarrier(planepnt, &ActionTakeOff))
			{
				AIGetClosestAirField(planepnt, fptemp, &fieldpos, &runwayheading, &ActionTakeOff, -1.0f, 0);
				planepnt->AI.WayPosition = fieldpos;
				planepnt->AI.WayPosition.Y = 1000 * FTTOWU;
				AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToLandingPoint);
				planepnt->AI.lDesiredSpeed = pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
			}

			if(planepnt->AI.Behaviorfunc != AIFlyTakeOff)
			{
				planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
				planepnt->AI.Behaviorfunc = AIFlyTakeOff;
			}

			if(planepnt->AI.lGroundTargetFlag == -1)
			{
				AIC_SAR_Success_Msg(planepnt - Planes);
			}
			else
			{
				AIC_SAR_Fail_Msg(planepnt - Planes);
			}
			SetLandingGearUpDown(planepnt,RAISE_LANDING_GEAR);
			planepnt->AI.iAIFlags2 &= ~AILANDING;
		}
	}
}

//**************************************************************************************
void *AIGetClosestAirField(PlaneParams *planepnt, FPointDouble worldpos, FPointDouble *fieldpos, ANGLE *runwayheading, TakeOffActionType *ActionTakeOff, double minfar, int ignoreside, int allow_both_ends, int ignore_destroy)
{
	RunwayInfo *runway = NULL;
	double newfar;
	double heading;
	FPointDouble searchpos;
	FPointDouble endpoint;
	int switchends = 0;

	if(worldpos.X < 0)
	{
		searchpos = planepnt->WorldPosition;
	}
	else
	{
		searchpos = worldpos;
	}

	for (RunwayInfo *R=AllRunways; R!=NULL; R=R->NextRunway)
	{
		if((!ignore_destroy) && (R->Flags & RI_DESTROYED))  continue;

		newfar = R->ILSLandingPoint - searchpos;

		if((GDConvertRunwaySide(R) == planepnt->AI.iSide) || (ignoreside))
		{
			if((newfar < minfar) || (minfar < 0.0f))
			{
				minfar = newfar;
				runway = R;
				switchends = 0;
			}
		}
		if(allow_both_ends)
		{
			newfar = R->ILSFarEndPoint - searchpos;

			if((GDConvertRunwaySide(R) == planepnt->AI.iSide) || (ignoreside))
			{
				if((newfar < minfar) || (minfar < 0.0f))
				{
					minfar = newfar;
					runway = R;
					switchends = 1;
				}
			}
		}
	}

	if(runway)
	{
		if(!switchends)
		{
			if(planepnt == PlayerPlane)
			{
				AICSetUpTowerVoice(runway->ILSLandingPoint);
			}
			heading = -runway->Heading;
			*runwayheading = AIConvert180DegreeToAngle(heading);
			*fieldpos = runway->ILSLandingPoint;
			endpoint = runway->ILSFarEndPoint;
			ActionTakeOff->dwSerialNumber = runway->Pieces[0]->Basics.SerialNumber;
		}
		else
		{
			if(planepnt == PlayerPlane)
			{
				AICSetUpTowerVoice(runway->ILSFarEndPoint);
			}
			heading = -runway->Heading + 180.0f;
			*runwayheading = AIConvert180DegreeToAngle(heading);
			*fieldpos = runway->ILSFarEndPoint;
			endpoint = runway->ILSLandingPoint;
			ActionTakeOff->dwSerialNumber = runway->Pieces[0]->Basics.SerialNumber;
		}
	}
	else
	{
		heading = 0;
		*runwayheading = 0;
		*fieldpos = searchpos;
		endpoint = searchpos;
		ActionTakeOff->dwSerialNumber = 0;
	}


	if (!DoingSimSetup)
	{
		fieldpos->Y = LandHeight(fieldpos->X, fieldpos->Z);
	}
	else
	{
		OffDiskTerrainInfo->m_Location = *fieldpos;
		OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
		fieldpos->Y = OffDiskTerrainInfo->m_Location.Y;
	}

	ActionTakeOff->lRunwayStartX = fieldpos->X * WUTOFT;
	ActionTakeOff->lRunwayStartY = fieldpos->Y * WUTOFT;
	ActionTakeOff->lRunwayStartZ = fieldpos->Z * WUTOFT;
	ActionTakeOff->lRunwayEndX = endpoint.X * WUTOFT;
	ActionTakeOff->lRunwayEndY = fieldpos->Y * WUTOFT;
	ActionTakeOff->lRunwayEndZ = endpoint.X * WUTOFT;
	ActionTakeOff->lTimer = 0;
	ActionTakeOff->lRange = 0;
	ActionTakeOff->lFlag = 0;

	return(runway);
}

//**************************************************************************************
int AIIsClosestAirFieldOpen(PlaneParams *planepnt, FPointDouble worldpos, double minfar)
{
	RunwayInfo *runway = NULL;
	double newfar;
	FPointDouble searchpos;
	FPointDouble endpoint;

	if(worldpos.X < 0)
	{
		searchpos = planepnt->WorldPosition;
	}
	else
	{
		searchpos = worldpos;
	}

	for (RunwayInfo *R=AllRunways; R!=NULL; R=R->NextRunway)
	{
		newfar = R->ILSLandingPoint - searchpos;

		if((newfar < minfar) || (minfar < 0.0f))
		{
			minfar = newfar;
			runway = R;
		}
	}

	if(runway)
	{
		if (runway->Flags & RI_DESTROYED)
			return(0);
	}
	return(1);
}

//**************************************************************************************
int AICheckForSAR(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	float tdistnm;
	WeaponParams *W;

	foundrange = (400 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if(checkplane->AI.Behaviorfunc == AISARWait)
				{
					dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					tdistnm = (tdist * WUTONM);
					if((tdist < foundrange) && (tdistnm < checkplane->AI.lVar2))
					{
						foundrange = tdist;
						foundplane = checkplane - Planes;
					}
				}
			}
		}
		checkplane ++;
	}

	if(foundplane == -1)
	{
		return(0);
	}
	else
	{
		checkplane = &Planes[foundplane];
		AIChangeGroupBehavior(checkplane, checkplane->AI.Behaviorfunc, AIFlyToSARPoint);
		checkplane->AI.TargetPos = planepnt->WorldPosition;
		checkplane->AI.TargetPos.Y += 1000 * FTTOWU;
		checkplane->AI.lVar2 = planepnt - Planes;
		checkplane->AI.lTimer2 = 30000;
		AIGetSARCAP(checkplane);
		AICAddSoundCall(AIC_SAR_On_Way_Msg, foundplane, 30000, 50);
		checkplane->AI.lGroundTargetFlag = AIRCRAFT;
		checkplane->AI.pGroundTarget = planepnt;
		if(MultiPlayer)
		{
			NetPutGenericMessage2(checkplane, GM2_SAR_PLANE, (BYTE)(planepnt - Planes));
		}

		if(AIInPlayerGroup(planepnt))
		{
			for(W=Weapons; W<=LastWeapon; W++)
			{
				if((W->P == planepnt) && (W->Type == pGuyOnGroundType) && (W->Flags & WEAPON_INUSE) && (W->fpTempWay.X == 1))
				{
					FPoint updatedpos;
					updatedpos = W->Pos;

					updatedpos.X += ((frand() * 1000.0f) - 500.0f) * FTTOWU;
					updatedpos.Y += 1000 * FTTOWU;
					updatedpos.Z += (frand() * 1000.0f) - 500.0f * FTTOWU;

					checkplane->AI.TargetPos = updatedpos;

					if(rand() & 1)
					{
						AICAddSoundCall(AIC_Help_On_Way, planepnt - Planes, 10000, 50, PlayerPlane - Planes);
					}
					else
					{
						AICAddSoundCall(AIC_SAR_Inbound, planepnt - Planes, 10000, 50, PlayerPlane - Planes);
					}

					break;
				}
			}
		}
	}
	return(1);
}

//**************************************************************************************
int AICheckForVehicleSAR(MovingVehicleParams *vehiclepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	float tdistnm;

	foundrange = (400 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (vehiclepnt->iSide == checkplane->AI.iSide) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if(checkplane->AI.Behaviorfunc == AISARWait)
				{
					dx = checkplane->WorldPosition.X - vehiclepnt->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - vehiclepnt->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					tdistnm = (tdist * WUTONM);
					if((tdist < foundrange) && (tdistnm < checkplane->AI.lVar2))
					{
						foundrange = tdist;
						foundplane = checkplane - Planes;
					}
				}
			}
		}
		checkplane ++;
	}

	if(foundplane == -1)
	{
		return(0);
	}
	else
	{
		checkplane = &Planes[foundplane];
		AIChangeGroupBehavior(checkplane, checkplane->AI.Behaviorfunc, AIFlyToSARPoint);
		checkplane->AI.TargetPos = vehiclepnt->WorldPosition;
		checkplane->AI.TargetPos.Y += 1000 * FTTOWU;
		checkplane->AI.lVar2 = vehiclepnt - MovingVehicles;
		checkplane->AI.lTimer2 = 30000;
		AIGetSARCAP(checkplane);
		AICAddSoundCall(AIC_SAR_On_Way_Msg, foundplane, 30000, 50);
		checkplane->AI.lGroundTargetFlag = MOVINGVEHICLE;
		checkplane->AI.pGroundTarget = vehiclepnt;
		if(MultiPlayer)
		{
			NetPutGenericMessage2(checkplane, GM2_SAR_VEHICLE, (BYTE)(vehiclepnt - MovingVehicles));
		}

	}
	return(1);
}

//**************************************************************************************
void AIUpdateSAR(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	FPoint updatedpos;
	int foundplane = 0;

	updatedpos = planepnt->WorldPosition;
	updatedpos.Y += 1000 * FTTOWU;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if((checkplane->AI.Behaviorfunc == AIFlyToSARPoint) || (checkplane->AI.Behaviorfunc == AISARPickUp))
				{
					if(checkplane->AI.lVar2 == (planepnt - Planes))
					{
						checkplane->AI.TargetPos = updatedpos;
						foundplane = 1;
					}
				}
			}
		}
		checkplane ++;
	}


	if((!(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (PlayerPlane->Status & PL_ACTIVE))
	{
		if((planepnt->AI.iSide == PlayerPlane->AI.iSide) && ((planepnt->AI.lAIVoice >= SPCH_WNG1) && (planepnt->AI.lAIVoice <= SPCH_WNG8)) || ((planepnt->AI.lAIVoice >= SPCH_CAP1) && (planepnt->AI.lAIVoice <= SPCH_CAP3)) || ((planepnt->AI.lAIVoice >= SPCH_SEAD1) && (planepnt->AI.lAIVoice <= SPCH_UK_BOMB1)))
		{
			if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) == 0))
			{
				if(lPlayerDownedTimer < 0)
				{
					lPlayerDownedTimer = 600000;
					if(foundplane)
					{
						AICDoSAROnWayMsgs(planepnt);
					}
					else
					{
						AICDoNoSAROnWayMsgs(planepnt);
					}
				}
				else
				{
					AICShortSARMsgs(planepnt);
				}
			}
		}
	}
}

//**************************************************************************************
void AIUpdateChuteSAR(WeaponParams *W)
{
	PlaneParams *checkplane = &Planes[0];
	FPoint updatedpos;
	int foundplane = 0;
	PlaneParams *planepnt = W->P;
	float workspeed;
	int guyhurt = 0;

	updatedpos = W->Pos;

	updatedpos.X += ((frand() * 1000.0f) - 500.0f) * FTTOWU;
	updatedpos.Y += 1000 * FTTOWU;
	updatedpos.Z += (frand() * 1000.0f) - 500.0f * FTTOWU;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if((checkplane->AI.Behaviorfunc == AIFlyToSARPoint) || (checkplane->AI.Behaviorfunc == AISARPickUp))
				{
					if(checkplane->AI.lVar2 == (planepnt - Planes))
					{
						checkplane->AI.TargetPos = updatedpos;
						foundplane = 1;
					}
				}
			}
		}
		checkplane ++;
	}


	workspeed = sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Y * W->Vel.Y) + (W->Vel.Z * W->Vel.Z)) * FTSEC_TO_MLHR*MLHR_TO_KNOTS;

	if(workspeed > 22.0f)
	{
		guyhurt = 1;
	}
	else if(W->P)
	{
		if ((0.98f - (W->P->AI.iSkill * 0.5f)) < frand())
		{
			guyhurt = 1;
		}
	}

	if((!(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (PlayerPlane->Status & PL_ACTIVE))
	{
		if((planepnt->AI.iSide == PlayerPlane->AI.iSide) && ((planepnt->AI.lAIVoice >= SPCH_WNG1) && (planepnt->AI.lAIVoice <= SPCH_WNG8)) || ((planepnt->AI.lAIVoice >= SPCH_CAP1) && (planepnt->AI.lAIVoice <= SPCH_CAP3)) || ((planepnt->AI.lAIVoice >= SPCH_SEAD1) && (planepnt->AI.lAIVoice <= SPCH_UK_BOMB1)))
		{
			if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || ((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) == 0))
			{
				if(lPlayerDownedTimer < 0)
				{
					lPlayerDownedTimer = 600000;
					if(foundplane)
					{
						AICDoSAROnWayMsgs(planepnt, guyhurt);
					}
					else
					{
						AICDoNoSAROnWayMsgs(planepnt, guyhurt);
					}
				}
				else
				{
					AICShortSARMsgs(planepnt, guyhurt);
				}
			}
		}
	}
}

//**************************************************************************************
void AIGetSARCAP(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dz, tdist;
	int foundplane = -1;
	float fworkspeed;

	foundrange = (120 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if (((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_C3))))
		{
			if((checkplane->OnGround == 2) && (!(checkplane->AI.iAICombatFlags1 & AI_READY_5)))
			{
				checkplane ++;
				continue;
			}

			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if((AIIsCAPing(checkplane)) || (checkplane->AI.Behaviorfunc == AIFlySweep) || (checkplane->AI.iAIFlags2 & AIINVISIBLE))
				{
					dx = checkplane->WorldPosition.X - planepnt->AI.TargetPos.X;
					dz = checkplane->WorldPosition.Z - planepnt->AI.TargetPos.Z;
					tdist = QuickDistance(dx, dz);
					if(tdist < foundrange)
					{
						foundrange = tdist;
						foundplane = checkplane - Planes;
					}
				}
			}
		}
		checkplane ++;
	}
	if(foundplane != -1)
	{
		if(MultiPlayer)
		{
			NetPutGenericMessage2(checkplane, GM2_SAR_CAP, (BYTE)(planepnt - Planes));
		}

		checkplane = &Planes[foundplane];

		if(!(planepnt->AI.iAIFlags2 & AIINVISIBLE))
		{
			AIReplaceCAPorSweep(checkplane);
		}

		fworkspeed = 0.8f * pDBAircraftList[checkplane->AI.iPlaneIndex].iMaxSpeed;
		checkplane->AI.lDesiredSpeed = fworkspeed;
		AIChangeGroupBehavior(checkplane, checkplane->AI.Behaviorfunc, AISetUpSARCAP, 0);
		checkplane->AI.WayPosition = planepnt->AI.TargetPos;
		checkplane->AI.WayPosition.Y = 10000 * FTTOWU;
		AIUpdateGroupPlaneLink(checkplane - Planes, planepnt);

		if(checkplane->AI.iAIFlags2 & AIINVISIBLE)
		{
			if(checkplane->OnGround == 2)
			{
//				checkplane->AI.iAIFlags2 |= AIINVISIBLE;
//				checkplane->Status &= ~PL_ACTIVE;
				if(checkplane->AI.Behaviorfunc != CTWaitingForLaunch)
				{
					checkplane->AI.OrgBehave = checkplane->AI.Behaviorfunc;
					checkplane->AI.Behaviorfunc = CTWaitingForLaunch;
				}
				checkplane->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
				checkplane->AI.lTimer2 = -99999;
//				CTAddToLaunchQueue(checkplane);

				PlaneParams *wingwalk;
				wingwalk = checkplane;
				while(wingwalk)
				{
					if(wingwalk->AI.wingman >= 0)
					{
						Planes[wingwalk->AI.wingman].AI.lTimer2 = -99999;
						Planes[wingwalk->AI.wingman].AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
					}

					if(wingwalk->AI.nextpair >= 0)
					{
						wingwalk = &Planes[wingwalk->AI.nextpair];
						wingwalk->AI.lTimer2 = -99999;
						wingwalk->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
					}
					else
					{
						wingwalk = NULL;
					}
				}
			}
			else if(checkplane->OnGround)
			{
				checkplane->AI.iAIFlags2 &= ~AIINVISIBLE;
				checkplane->Status |= PL_ACTIVE;
				if(checkplane->AI.Behaviorfunc != AIFlyTakeOff)
				{
					checkplane->AI.OrgBehave = checkplane->AI.Behaviorfunc;
					checkplane->AI.Behaviorfunc = AIFlyTakeOff;
				}
			}
		}
	}
}

//**************************************************************************************
void AISetUpSARCAP(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;

	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
		return;
	}

	tdist = (FlyToPoint(planepnt, planepnt->AI.WayPosition) * WUTOFT);

	if(tdist < (15.0f * NMTOFT))
	{
		xoff = 0;
		zoff = 15.0f * NMTOFT;

		planepnt->AI.iVar1 = AIConvertAngleTo180Degree(planepnt->Heading);
		if(planepnt->AI.iVar1 < 0)
		{
			planepnt->AI.iVar1 = planepnt->AI.iVar1 + 360;
		}
		planepnt->AI.lVar2 = 60;
		heading = DegToRad((double)planepnt->AI.iVar1);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;

		AIC_On_Station_Msg(planepnt - Planes);

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySARCAPSide1);
		planepnt->AI.lDesiredSpeed = 400;
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
	return;
}

//**************************************************************************************
void AIFlySARCAPSide1(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;

	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
		return;
	}

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos) * WUTOFT;

//	if(tdist < 5000)
	if(AIInNextRange(planepnt, tdist,  5000))
	{
		heading = (double)planepnt->AI.iVar1 - 120;
		while(heading < 0)
		{
			heading += 360;
		}
		heading = DegToRad(heading);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;

		xoff = 0;
		zoff = 15.0f * NMTOFT;

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySARCAPSide2);
		planepnt->AI.lDesiredSpeed = 400;
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);
	return;
}

//**************************************************************************************
void AIFlySARCAPSide2(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;

	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
		return;
	}

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos) * WUTOFT;

//	if(tdist < 5000)
	if(AIInNextRange(planepnt, tdist,  5000))
	{
		heading = (double)planepnt->AI.iVar1 + 120;
		while(heading > 0)
		{
			heading -= 360;
		}
		heading = DegToRad(heading);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;

		xoff = 0;
		zoff = 15.0f * NMTOFT;

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySARCAPSide3);
		planepnt->AI.lDesiredSpeed = 400;
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);
	return;
}

//**************************************************************************************
void AIFlySARCAPSide3(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;

	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
		return;
	}

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos) * WUTOFT;

//	if(tdist < 5000)
	if(AIInNextRange(planepnt, tdist,  5000))
	{
		heading = (double)planepnt->AI.iVar1;
		heading = DegToRad(heading);
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;

		xoff = 0;
		zoff = 15.0f * NMTOFT;

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySARCAPSide1);
		planepnt->AI.lDesiredSpeed = 400;
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);
	return;
}

//**************************************************************************************
void AIFlySARCoverSide1(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;
	PlaneParams *linkedplane;
	float fworkspeed;
	int rtb = 0;
	TakeOffActionType ActionTakeOff;
	FPointDouble fieldpos, fptemp;
	ANGLE runwayheading;


	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
		return;
	}

	linkedplane = planepnt->AI.LinkedPlane;
	if(linkedplane == NULL)
	{
		rtb = 1;
	}
	else if(linkedplane->AI.Behaviorfunc != AIFlyToLandingPoint)
	{
		rtb = 1;
	}

	if(rtb)
	{
		fworkspeed = 0.8f * pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
		planepnt->AI.lDesiredSpeed = fworkspeed;
		fptemp.SetValues(-1.0f,-1.0f,-1.0f);
		AIGetClosestAirField(planepnt, fptemp, &fieldpos, &runwayheading, &ActionTakeOff, -1.0f, 0);
		planepnt->AI.WayPosition = fieldpos;
		planepnt->AI.WayPosition.Y = 10000 * FTTOWU;
		AISetPlaneLandingFlyToField(planepnt, &ActionTakeOff, runwayheading);
		return;
	}

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos) * WUTOFT;

//	if(tdist < 5000)
	if(AIInNextRange(planepnt, tdist,  5000))
	{
		heading = DegToRad(AIConvertAngleTo180Degree(linkedplane->Heading));
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;

		xoff = 0;
		zoff = -15.0f * NMTOFT;

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySARCoverSide2);
		planepnt->AI.lDesiredSpeed = 400;
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);
	return;
}

//**************************************************************************************
void AIFlySARCoverSide2(PlaneParams *planepnt)
{
	FPoint pointpos;
	double angxx, angxz, angzx, angzz;
	double heading;
	float xoff, zoff;
	float tdist;
	PlaneParams *linkedplane;
	int rtb = 0;
	TakeOffActionType ActionTakeOff;
	FPointDouble fieldpos;
	ANGLE runwayheading;
	float fworkspeed;
	FPointDouble fptemp;

	if((planepnt->AI.winglead >= 0) || (planepnt->AI.prevpair >= 0))
	{
		AIFormationFlyingLead(planepnt, AIGetLeader(planepnt));
		return;
	}

	linkedplane = planepnt->AI.LinkedPlane;
	if(linkedplane == NULL)
	{
		rtb = 1;
	}
	else if(linkedplane->AI.Behaviorfunc != AIFlyToLandingPoint)
	{
		rtb = 1;
	}

	if(rtb)
	{
		fworkspeed = 0.8f * pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
		planepnt->AI.lDesiredSpeed = fworkspeed;
		fptemp.SetValues(-1.0f,-1.0f,-1.0f);
		AIGetClosestAirField(planepnt, fptemp, &fieldpos, &runwayheading, &ActionTakeOff, -1.0f, 0);
		planepnt->AI.WayPosition = fieldpos;
		planepnt->AI.WayPosition.Y = 10000 * FTTOWU;
		AISetPlaneLandingFlyToField(planepnt, &ActionTakeOff, runwayheading);
		return;
	}

	tdist = FlyToPoint(planepnt, planepnt->AI.TargetPos) * WUTOFT;

//	if(tdist < 5000)
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5000))
	{
		heading = DegToRad(AIConvertAngleTo180Degree(linkedplane->Heading));
		angzx = sin(heading);
		angzz = cos(heading);
		angxx = angzz;
		angxz = angzx;

		xoff = 0;
		zoff = 15.0f * NMTOFT;

		planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlySARCoverSide1);
		planepnt->AI.lDesiredSpeed = 400;
	}
	else
	{
		AIUpdateBehaviorFormation(planepnt);
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	AIStandardCAPStuff(planepnt);
	return;
}

//**************************************************************************************
void AIReleaseSARCover(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];

	while (checkplane <= LastPlane)
	{
		if (((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_C3))))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (checkplane->AI.LinkedPlane == planepnt))
			{
				if(AIIsSARCAPing(checkplane))
				{
					checkplane->AI.lDesiredSpeed = 350;
					AIChangeGroupBehavior(checkplane, checkplane->AI.Behaviorfunc, AIFlySARCoverSide1);
					checkplane->AI.TargetPos = planepnt->WorldPosition;
					checkplane->AI.TargetPos.Y = 10000 * FTTOWU;
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AIGetWeaponIndex(int itypenum)
{
	for (int cnt=0; cnt<iNumWeaponList; cnt++)
	{
		if (pDBWeaponList[cnt].lWeaponID == itypenum)
		{
			return(cnt);
		}
	}
	return(-1);
}

//**************************************************************************************
int AIInNextRange(PlaneParams *planepnt, float ftdist, int okdist, float maxroll)
{
	float turnradius;

	if(ftdist < okdist)
	{
		return(1);
	}

	else if((planepnt->Roll > 0x1000) && (planepnt->Roll < 0xF000))
	{
		turnradius = AIGetTurnRadFt(planepnt) * 2;

		if((maxroll < 90.0f) && (maxroll > 10.0f))
		{
			turnradius /= sin(DegToRad(maxroll));
		}

		if(ftdist < turnradius)
		{
			return(1);
		}
	}

	return(0);
}

//**************************************************************************************
float AIGetTurnRadFt(PlaneParams *planepnt)
{
	float fworkvar, turnradius;

	if(planepnt->YawRate)
	{
		fworkvar = planepnt->IfHorzVelocity * (360 / planepnt->YawRate);
	}
	else
	{
		fworkvar = planepnt->IfHorzVelocity * 360;
#ifdef _DEBUG
		char errmsg[256];
		sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planepnt - Planes, planepnt->AI.iPlaneIndex);
	    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
	}
	turnradius = fworkvar / (2 * PI);

	return(turnradius);
}

//**************************************************************************************
float AIComputeHeadingFromBullseye(FPoint pointpos, float *rtdist, float *rdx, float *rdz)
{
	double dx, dz;
	double offangle;
	float tdist;

	dx = pointpos.X - ConvertWayLoc(BullsEye.x);
	dz = pointpos.Z - ConvertWayLoc(BullsEye.z);

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	tdist = QuickDistance(dx,dz);
	*rdx = dx;
	*rdz = dz;
	*rtdist = tdist;
	return((float) offangle);
}

//**************************************************************************************
void AISetUpFormOnMeeting(PlaneParams *planepnt, int formonplane)
{
	if((formonplane < 0) || (formonplane > (LastPlane - Planes)))
	{
		return;
	}
	if(!((Planes[formonplane].Status & PL_ACTIVE) && (!(Planes[formonplane].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))))
	{
		return;
	}

//	Planes[escortplane].AI.LinkedPlane = planepnt; //  Don't think this is needed, and it screws up refueling with escorts for the refueler
	AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToFormOnMeeting);
	AIUpdateGroupPlaneLink(planepnt - Planes, &Planes[formonplane]);
}

//**************************************************************************************
void AIFlyToFormOnMeeting(PlaneParams *planepnt)
{
	float tdist;
	float desiredspeed;
	PlaneParams *linkedplane;
	int escortOK = 1;
	FPointDouble linkedpos;
	
	if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
	{
		if(!planepnt->AI.LinkedPlane)
		{
			if(planepnt->AI.OrgBehave == NULL)
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
				}
				planepnt->AI.Behaviorfunc = AIFlyFormation;
			}
			else
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 3, 0, 0);
				}
				planepnt->AI.OrgBehave = AIFlyFormation;
			}

			AINextWayPointNoActions(planepnt);

			AISetUpNewWayPoint(planepnt);

			return;
		}

		linkedplane = planepnt->AI.LinkedPlane;

		linkedpos = planepnt->AI.LinkedPlane->WorldPosition;

		if(linkedpos.Y < (9000.0f * FTTOWU))
		{
			linkedpos.Y = 10000.0f * FTTOWU;
		}
		else
		{
			linkedpos.Y -= 1000.0f * FTTOWU;
		}

		tdist = FlyToPoint(planepnt, planepnt->AI.LinkedPlane->WorldPosition);
		desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, planepnt->AI.LinkedPlane->V, AIDESIREDSPDCALC) + 100;

		if(desiredspeed < 200)
			desiredspeed = 200;

		MBAdjustAIThrust(planepnt, desiredspeed, 1);

		AIUpdateFormation(planepnt);

		if(linkedplane->OnGround)
		{
			escortOK = 0;
		}
		else
		{
			if(linkedplane->Status & PL_AI_DRIVEN)
			{
				if((linkedplane->AI.Behaviorfunc == CTWaitingForLaunch) || (linkedplane->AI.Behaviorfunc == CTDoCarrierLaunch))
				{
					escortOK = 0;
				}
			}
			else
			{
				if(linkedplane == PlayerPlane)
				{
					if(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)
					{
						if((PlayerPlane->AI.lVar2 >= 100) || ((PlayerPlane->Status & PL_AI_DRIVEN) && ((PlayerPlane->AI.Behaviorfunc == CTDoCarrierLaunch) || (PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch))))
						{
							escortOK = 0;
						}
					}
				}
			}
		}

		if(((tdist * WUTOFT) < (AI_VISUAL_RANGE_NM * NMTOFT)) && (escortOK))
		{
			if(planepnt->AI.LinkedPlane == PlayerPlane)
			{
#if 1
				if((AIRIsAttack(planepnt - Planes)) || (AIRIsFighter(planepnt - Planes)))
				{
					AIC_Beginning_Human_Escort_Msg(planepnt - Planes, planepnt->AI.LinkedPlane - Planes);
				}
#else
				if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
				{
					AIC_FormOn_Human_Msg(planepnt - Planes, planepnt->AI.LinkedPlane - Planes);
				}
				else if((planepnt->AI.lAIVoice == SPCH_CAP1) || (planepnt->AI.lAIVoice == SPCH_CAP2) || (planepnt->AI.lAIVoice == SPCH_CAP3) || (planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
				{
					AIC_Beginning_Human_Escort_Msg(planepnt - Planes, planepnt->AI.LinkedPlane - Planes);
				}
#endif
				AIUpdateGroupFlags(planepnt - Planes, (AI_CHECK_PLAYER_RELEASE|AI_FORMED_PLAYER), 0);
			}

			if(AIIsSEADPlane(planepnt))
			{
				AIUpdateGroupFlags(planepnt - Planes, 0, 0, 0, AI_FORMON_SEAD_SEARCH);
			}

			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFormationFlying);
			linkedplane = planepnt->AI.LinkedPlane;
			AIUpdateGroupPlaneLink(planepnt - Planes, NULL);
//			AIAddPlaneGroupToFormation(planepnt, PlayerPlane - Planes);
			AIAddPlaneGroupToFormation(planepnt, linkedplane - Planes);
		}
	}
	else
	{
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AIUpdateGroupFlags(int planenum, int flag1, int flag2, int doinv, int combatflags1)
{
	if(doinv)
	{
		Planes[planenum].AI.iAIFlags1 &= ~flag1;
		Planes[planenum].AI.iAIFlags2 &= ~flag2;
		Planes[planenum].AI.iAICombatFlags1 &= ~combatflags1;
	}
	else
	{
		Planes[planenum].AI.iAIFlags1 |= flag1;
		Planes[planenum].AI.iAIFlags2 |= flag2;
		Planes[planenum].AI.iAICombatFlags1 |= combatflags1;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AIUpdateGroupFlags(Planes[planenum].AI.wingman, flag1, flag2, doinv, combatflags1);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AIUpdateGroupFlags(Planes[planenum].AI.nextpair, flag1, flag2, doinv, combatflags1);
	}
}

//**************************************************************************************
int AICheckAutoPlayerRelease(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	Escort	*pEscortAction;
	FormOn	*pFormOnAction;
	float dx, dz, tdist;
	int nobombleft;

// 	if(!(PlayerPlane->Status & AL_DEVICE_DRIVEN))
//		return(NULL);

	if((planepnt < Planes) || (planepnt > LastPlane))
		return(0);

	lookway = PlayerPlane->AI.CurrWay;

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_RELEASE_ESCORT))
		{
			pEscortAction = (Escort *)AIActions[actcnt].pAction;
			if(pEscortAction->iPlaneNum == (planepnt - Planes))
			{
				dx = PlayerPlane->WorldPosition.X - ConvertWayLoc(lookway->lWPx);
				dz = PlayerPlane->WorldPosition.Z - ConvertWayLoc(lookway->lWPz);

				tdist = QuickDistance(dx,dz);

				if((tdist * WUTOFT) < (PlayerPlane->V * 70))
				{
					AIReleaseEscort(PlayerPlane, pEscortAction->iPlaneNum, pEscortAction->iWayPointNum);
					AIUpdateGroupFlags(planepnt - Planes, (AI_CHECK_PLAYER_RELEASE|AI_FORMED_PLAYER), 0, 1);
					return(1);
				}
				else
				{
					return(0);
				}
			}
		}
		else if((AIActions[actcnt].ActionID == ACTION_RELEASE_FORMON))
		{
			pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
			if(pFormOnAction->iPlaneNum == (planepnt - Planes))
			{
				dx = PlayerPlane->WorldPosition.X - ConvertWayLoc(lookway->lWPx);
				dz = PlayerPlane->WorldPosition.Z - ConvertWayLoc(lookway->lWPz);

				tdist = QuickDistance(dx,dz);

				if((tdist * WUTOFT) < (PlayerPlane->V * 70))
				{
					AIReleasePlaneGroupFromFormation(PlayerPlane, planepnt - Planes, pFormOnAction->iWayPointNum);
					if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
					{
						if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
						{
							if((planepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (planepnt->AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)))
							{
								nobombleft = AINoBombingLeft(planepnt - Planes);
								if(nobombleft == 0)
								{
									AICBomberAttackMsg(planepnt);
								}
								else if(nobombleft != -1)
								{
									AIC_Bombers_RTB_Msg(planepnt - Planes, PlayerPlane - Planes);
								}
							}
						}
					}
					AIUpdateGroupFlags(planepnt - Planes, (AI_CHECK_PLAYER_RELEASE|AI_FORMED_PLAYER), 0, 1);
					return(1);
				}
				else
				{
					return(0);
				}
			}
		}
	}
	return(0);
}

//**************************************************************************************
void AICheckIfPlayerDoingEscort(int planenum)
{
	MBWayPoints *lookway;
	Escort	*pEscortAction;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;

	lookway = &AIWayPoints[Planes[planenum].AI.startwpts];

	numwpts = Planes[planenum].AI.numwaypts + (Planes[planenum].AI.CurrWay - &AIWayPoints[Planes[planenum].AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				AIUpdateGroupFlags(pEscortAction->iPlaneNum, AI_ESCORTED_PLAYER, 0);
			}
		}
		lookway ++;
	}
}

//**************************************************************************************
int AINoBombingLeft(int planenum)
{
	FormOn	*pFormOnAction;
	Escort	*pEscortAction;
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;
	int returnval = 1;

//	lookway = &AIWayPoints[Planes[planenum].AI.startwpts];
//	numwpts = Planes[planenum].AI.numwaypts + (Planes[planenum].AI.CurrWay - &AIWayPoints[Planes[planenum].AI.startwpts]);
	numwpts = Planes[planenum].AI.numwaypts;
	lookway = Planes[planenum].AI.CurrWay;

	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
			{
				return(0);
			}
			if((AIActions[actcnt].ActionID == ACTION_CAS))
			{
				return(0);
			}
			if((AIActions[actcnt].ActionID == ACTION_SEAD))
			{
				return(0);
			}
			if((AIActions[actcnt].ActionID == ACTION_FORMON))
			{
				pFormOnAction = (FormOn *)AIActions[actcnt].pAction;
				if(pFormOnAction->iPlaneNum == PlayerPlane - Planes)
				{
					returnval = -1;
				}
			}
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				if(pEscortAction->iPlaneNum == PlayerPlane - Planes)
				{
					returnval = -1;
				}
			}
		}
		lookway ++;
	}
	return(returnval);
}

//**************************************************************************************
void EVNextEyePoint()
{
	int slen;

	if(g_iNumEyePoints <= 0)
	{
		return;
	}

	iCurrEyePoint ++;
	if((iCurrEyePoint >= g_iNumEyePoints) || (iCurrEyePoint < 0))
	{
		iCurrEyePoint = 0;
	}

	slen = strlen(EyePointList[iCurrEyePoint].sLabel);

	if(slen > 0)
	{
		AICAddAIRadioMsgs(EyePointList[iCurrEyePoint].sLabel, 40);
	}

	EVActivateEyePoint(iCurrEyePoint);
}

//*******************
// EyePointList type
//*******************
/*
	long X;
	long Y;
	long Z;
	int H;
	int P;
	int R;
	int iZoom;
	long lTimer;
	long lZoomTimer;
	long lFlags;	// For things like zoom over time
	long lRange;
	long lObject;
	char sLabel[NAMESIZE];
	long lReserved1;
	long lReserved2;
	long lReserved3;
	long lReserved4;
	long lReserved5;
*/
//***********************

//**************************************************************************************
void EVActivateEyePoint(int eyenum)
{
	setup_no_cockpit_art();

	if(EyePointList[eyenum].lObject == -1)
	{
		ChangeViewModeOrSubject(CAMERA_FREE);

		Camera1.Heading = AIConvert180DegreeToAngle(-EyePointList[eyenum].H);
		Camera1.Pitch = AIConvert180DegreeToAngle(EyePointList[eyenum].P);
		Camera1.Roll = AIConvert180DegreeToAngle(EyePointList[eyenum].R);

		camera_setup_initial_relative_quats();
	}
	else
	{
		AssignCameraSubject(&Planes[EyePointList[eyenum].lObject],CO_PLANE);
		setup_no_cockpit_art();
		ChangeViewModeOrSubject(CAMERA_RC_VIEW);
	}
	Camera1.CameraLocation.X = ConvertWayLoc(EyePointList[eyenum].X);
	Camera1.CameraLocation.Y = ConvertWayLoc(EyePointList[eyenum].Y);
	Camera1.CameraLocation.Z = ConvertWayLoc(EyePointList[eyenum].Z);

	camera_setup_initial_location(Camera1.CameraLocation);
}

//**************************************************************************************
void EVSetNewEyePoint()
{
	int newpoint;

	if(g_iNumEyePoints < (MAX_NUM_EYEPOINTS - 1))
	{
		g_iNumEyePoints ++;
	}
	newpoint = g_iNumEyePoints - 1;

	EyePointList[newpoint].X = Camera1.CameraLocation.X * WUTOFT;
	EyePointList[newpoint].Y = Camera1.CameraLocation.Y * WUTOFT;
	EyePointList[newpoint].Z = Camera1.CameraLocation.Z * WUTOFT;
	EyePointList[newpoint].lObject = -1;
	EyePointList[newpoint].H = -AIConvertAngleTo180Degree(Camera1.Heading);
	EyePointList[newpoint].P = AIConvertAngleTo180Degree(Camera1.Pitch);
	EyePointList[newpoint].R = AIConvertAngleTo180Degree(Camera1.Roll);
	sprintf(EyePointList[newpoint].sLabel, "Camera %d", newpoint + 1);
}


//**************************************************************************************
int AICheckNearBombWaypt(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	float dx, dz, tdist;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	planepnt->Status |= PlayerPlane->Status & (PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);

// 	if(!(PlayerPlane->Status & AL_DEVICE_DRIVEN))
//		return(0);

	if(planepnt->AI.Behaviorfunc != AIFlyFormation)
	{
		return(0);
	}

	lookway = PlayerPlane->AI.CurrWay;

	if((lookway <= planepnt->AI.CurrWay) || (((planepnt->AI.CurrWay - AIWayPoints) == planepnt->AI.startwpts) && (iEasyBombVar == 5)))
	{
		return(0);
	}

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
		{
			dx = PlayerPlane->WorldPosition.X - ConvertWayLoc(lookway->lWPx);
			dz = PlayerPlane->WorldPosition.Z - ConvertWayLoc(lookway->lWPz);

			tdist = QuickDistance(dx,dz);

			if((!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AG)) && ((tdist * WUTOFT) < BOMBTRAILDIST) && ((!(planepnt->AI.iAIFlags1 & AI_ASK_PLAYER_ATTACK)) || (iEasyBombVar == 2)))
			{
				if((PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)) || (iEasyBombVar == 2))
				{
					iEasyBombVar = 0;
					if(AICGetBombWaypoint(planepnt - Planes, 1) == lookway)
					{
						AICFGrndAttackPrime();
					}
					else
					{
						AICFGrndAttackSecond();
					}
					AIUpdateGroupFlags(PlayerPlane - Planes, AI_ASK_PLAYER_ATTACK, 0, 0);
				}
				else
				{
					AICWingReadyToAttackMsg(planepnt - Planes);
#if 0
					if(g_Settings.gp.nType == GP_TYPE_CASUAL)
					{
						iEasyBombVar = 1;
						lEasyBomberTimer = 60000;
					}
#endif
					AIUpdateGroupFlags(PlayerPlane - Planes, AI_ASK_PLAYER_ATTACK, 0, 0);
				}
				return(1);
			}
			else
			{
				return(0);
			}
		}
	}
	return(0);
}

//**************************************************************************************
void AIRestoreWaypointBehavior(int planenum, MBWayPoints *waypnt, int numwaypts)
{
	int wingman, nextsec;
	void (*holdbehavior)(PlaneParams *planepnt);

	if(Planes[planenum].AI.Behaviorfunc == AIFlyFormation)
	{
		Planes[planenum].AI.CurrWay = waypnt;
		Planes[planenum].AI.numwaypts = numwaypts;
		AISetUpWayPointActions(&Planes[planenum], Planes[planenum].AI.CurrWay);
	}
	else if(Planes[planenum].AI.OrgBehave == AIFlyFormation)
	{
		Planes[planenum].AI.CurrWay = waypnt;
		Planes[planenum].AI.numwaypts = numwaypts;
		holdbehavior = Planes[planenum].AI.Behaviorfunc;
		Planes[planenum].AI.Behaviorfunc = Planes[planenum].AI.OrgBehave;
		AISetUpWayPointActions(&Planes[planenum], Planes[planenum].AI.CurrWay);
		Planes[planenum].AI.OrgBehave = Planes[planenum].AI.Behaviorfunc;
		Planes[planenum].AI.Behaviorfunc = holdbehavior;
	}
	else if(Planes[planenum].AI.iAIFlags1 & AIINFORMATION)
	{
		Planes[planenum].AI.CurrWay = waypnt;
		Planes[planenum].AI.numwaypts = numwaypts;
		AISetUpWayPointActions(&Planes[planenum], Planes[planenum].AI.CurrWay);
	}

	wingman = Planes[planenum].AI.wingman;
	nextsec = Planes[planenum].AI.nextpair;
	if(wingman >= 0)
	{
		AIRestoreWaypointBehavior(wingman, waypnt, numwaypts);
	}

	if(nextsec >= 0)
	{
		AIRestoreWaypointBehavior(nextsec, waypnt, numwaypts);
	}
}

//**************************************************************************************
void AICheckROE(PlaneParams *planepnt, int action)
{
	int curr_roe = iAI_ROE[planepnt->AI.iSide];

	if(action > curr_roe)
	{
		//  check for setting flag
	}
}

//**************************************************************************************
void AIUpdateAttackOnVehicle(PlaneParams *planepnt)
{
	MovingVehicleParams *vehiclepnt;
	double dy;
	float time_bomb_in_air;
	FPointDouble workpos;
	double radhead;

	if((planepnt->AI.lGroundTargetFlag != MOVINGVEHICLE) || (!planepnt->AI.pGroundTarget))
	{
		return;
	}

	vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;

	if((!(vehiclepnt->Status & VL_MOVING)) || (vehiclepnt->Status & VL_WAITING) || (planepnt->AI.Behaviorfunc == AIFlyCASOrbit))
	{
		planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
		planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
		planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
		return;
	}

	if(((planepnt->AI.WayPosition.Y - planepnt->AI.TargetPos.Y) < (2000.0f * FTTOWU)) && ((planepnt->AI.Behaviorfunc != AIFlyToPopPoint) && (planepnt->AI.Behaviorfunc != AIFlyToLoftPoint) && (planepnt->AI.Behaviorfunc != AIGroupFlyToPopPoint) && (planepnt->AI.Behaviorfunc != AIGroupFlyToLoftPoint)))
	{
		planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y + (2000.0f * FTTOWU);
	}

	dy = vehiclepnt->WorldPosition.Y - planepnt->WorldPosition.Y;

	time_bomb_in_air = AITimeToGround(planepnt, planepnt->IfVelocity.Y, dy);

	radhead = DegToRad(AIConvertAngleTo180Degree(vehiclepnt->Heading));

	workpos = vehiclepnt->WorldPosition;

	workpos.X -= (sin(radhead) * vehiclepnt->fSpeedFPS * FTTOWU * time_bomb_in_air);
	workpos.Z -= (cos(radhead) * vehiclepnt->fSpeedFPS * FTTOWU * time_bomb_in_air);

	if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
	{
#ifdef V_USE_INST_HEIGHT
		GetHeightWithInstances(workpos,AllFoundations);
#else
		workpos.Y = LandHeight(workpos.X, workpos.Z);
#endif
	}
	else
	{
		workpos.Y = vehiclepnt->WorldPosition.Y + (vehiclepnt->fSpeedFPS * FTTOWU * time_bomb_in_air * (-vehiclepnt->fSinPitch));
	}

	planepnt->AI.WayPosition.X = workpos.X;
	if(planepnt->AI.WayPosition.Y < (workpos.Y + (500.0f * FTTOWU)))
	{
		planepnt->AI.WayPosition.Y = (workpos.Y + (500.0f * FTTOWU));
	}
	planepnt->AI.WayPosition.Z = workpos.Z;
	planepnt->AI.TargetPos = workpos;
}

//**************************************************************************************
int AIInPlayerGroup(PlaneParams *planepnt)
{
	if(planepnt == PlayerPlane)
	{
		return(1);
	}

	if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		return(1);
	}
	return(0);
}

//**************************************************************************************
void AICheckMaxGs(PlaneParams *planepnt)
{
	double yawval, pitchval, yawg, pitchg;
	float EffectivePitchRate;
	int inversepitch = 0;
	float rollpitcheffect;
	ANGLE DeltaPitch = (ANGLE)(planepnt->DesiredPitch - planepnt->Pitch);
	float maxg, fwork1, fwork2;
	float tvar;
	float stvar;
	float pitchwork;

#if 0    //  Not sure what this was for SRE
	if(((!planepnt->RollYawCouple) || ((planepnt->DesiredRoll > 0xE000) && (planepnt->DesiredRoll < 0x1000))) && ((DeltaPitch > 0xE000) && (DeltaPitch < 0x1000)))
	{
		return;
	}
#endif

	EffectivePitchRate = (planepnt->MaxPitchRate*planepnt->PitchPercentage);

	yawval = DegToRad(((planepnt->RollYawCouple > 0) ? (sin(DegToRad(planepnt->DesiredRoll*(1.0f/(float)DEGREE)))*(planepnt->YawRate)) : 0.0f));

	if((planepnt->DesiredRoll > 0x4000) && (planepnt->DesiredRoll < 0xC000))  //  Inverted roll makes opposite pitch required
	{
		DeltaPitch = (0xFFFF - DeltaPitch) + 1;
		inversepitch = 1;
	}

	if(DeltaPitch > 0x8000)  //  If trying to push negative Gs lower pitch rate to 1/3.
	{
		EffectivePitchRate /= 3;
	}

	if(planepnt->RollYawCouple < 0)
	{
		rollpitcheffect = 1.0f;
	}
	else
	{
		rollpitcheffect = fabs(cos(DegToRad(planepnt->DesiredRoll/(float)DEGREE)));  //SRE  so plane doesn't pitch alot while rolled
	}
	if(inversepitch)
	{
		rollpitcheffect *= 2;
		if(fabs(rollpitcheffect) > 1.0)
		{
			rollpitcheffect = (rollpitcheffect < 0) ? -1.0f : 1.0f;
		}
	}
	EffectivePitchRate *= rollpitcheffect;

	if (DeltaPitch)
	{
		tvar = ((float)DeltaPitch/DEGREE);
		tvar = (tvar > 360) ? 360 : tvar;
		stvar = (tvar > 180) ? (tvar - 360) : tvar;

		pitchwork = fabs(stvar / EffectivePitchRate);
		if (fabs(stvar) < (EffectivePitchRate * planepnt->dt))
			pitchval = DegToRad(stvar)/planepnt->dt;
		else
			pitchval = (float)((DeltaPitch < 0x8000) ? DegToRad(EffectivePitchRate) : -DegToRad(EffectivePitchRate));
	}
	else
	{
		pitchval = 0.0f;
		pitchwork = 0;
	}

	pitchg = fabs(sin(pitchval) * planepnt->V) / 32.0f;

	if(pitchwork < 1.0f)
	{
		pitchg *= pitchwork;
	}

	yawg = fabs(sin(yawval) *  planepnt->V) / 32.0f;
	yawval = RadToDeg(yawval);
	pitchval = RadToDeg(pitchval);

	if(DeltaPitch > 0x8000)
	{
		if(!inversepitch)
		{
			pitchg = -pitchg;
			yawg = -yawg;
		}
	}
	else
	{
		if(inversepitch)
		{
			pitchg = -pitchg;
			yawg = -yawg;
		}
	}

	fwork1 = pitchg + yawg + 1.0f;

	maxg = pDBAircraftList[planepnt->AI.iPlaneIndex].iYawDeceleration;
	if((fwork1 > 6.0f) && (maxg > 6.0f))
	{
		StartWingConTrails(planepnt);
	}
	else
	{
		StopWingConTrails(planepnt);
	}

	if(pitchg >= 0)
	{
		if(fwork1 > maxg)
		{
			fwork2 = maxg / fwork1;
			planepnt->PitchPercentage *= fwork2;
			if(planepnt->RollYawCouple > 0)
			{
				yawval *= fwork2;
				planepnt->DesiredRoll = ((planepnt->RollYawCouple) && (planepnt->YawRate)) ? AIConvert180DegreeToAngle(RadToDeg(asin(yawval / planepnt->YawRate))) : 0.0f;
			}
			else
			{
				planepnt->YawRate *= fwork2;
			}
		}
	}
	else
	{
		maxg = -3.0f;
		if(fwork1 < maxg)
		{
			fwork2 = maxg / fwork1;
			planepnt->PitchPercentage *= fwork2;
			if(planepnt->RollYawCouple == 1)  //  was > 0 but this was causing some dogfight problems
			{
				yawval *= fwork2;
				planepnt->DesiredRoll = ((planepnt->RollYawCouple) && (planepnt->YawRate)) ? AIConvert180DegreeToAngle(RadToDeg(asin(yawval / planepnt->YawRate))) : 0.0f;
			}
			else
			{
				planepnt->YawRate *= fwork2;
			}
		}
	}
}

//**************************************************************************************
double AICapAngle(double angle, double minval, double maxval)
{
	int cnt;
	double dworkvar = angle;

	cnt = 0;
	while((dworkvar > maxval) && (cnt < 5))
	{
		dworkvar -= 360.0;
		cnt ++;
	}

#ifdef _DEBUG
	if(cnt == 5)
	{
		PauseForDiskHit();
		if( lpDD )			//lpDD NULL in 3Dfx
			lpDD->FlipToGDISurface();
		DebugBreak();
		UnPauseFromDiskHit();
		dworkvar = 0;
	}
#endif

	cnt = 0;
	while((dworkvar < minval) && (cnt < 5))
	{
		dworkvar += 360.0;
		cnt ++;
	}

#ifdef _DEBUG
	if(cnt == 5)
	{
		PauseForDiskHit();
		if( lpDD)
			lpDD->FlipToGDISurface();
		DebugBreak();
		UnPauseFromDiskHit();
		dworkvar = 0;
	}
#endif

	return(dworkvar);
}

//**************************************************************************************
void AICheckForChutePickup(PlaneParams *planepnt)
{
	WeaponParams *W;
	int Index;

	planepnt->AI.lGroundTargetFlag = -2;

	for (W=Weapons; W<=LastWeapon; W++)
	{
		if(W->Flags & WEAPON_INUSE)
		{
			if(W->WeaponMove == ChuteStayOnGround)
			{
				if((W->P) && (!(W->Flags & WEP_DAMAGED_CHUTE)))
				{
					Index = AIGetWeaponIndex(W->Type->TypeNumber);
					if(pDBWeaponList[Index].lWeaponID != 104)
					{
						if((W->P - Planes) == planepnt->AI.lVar2)
						{
							W->WeaponMove = ChuteMoveToHelo;
							W->pTarget = (void *)planepnt;
							planepnt->AI.lGroundTargetFlag = -1;
						}
					}
				}
			}
		}
	}
}

//**************************************************************************************
void AICheckForVehicleSARPickup(PlaneParams *planepnt)
{
	MovingVehicleParams *vehiclepnt;

	planepnt->AI.lGroundTargetFlag = -2;

	vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
	while(vehiclepnt)
	{
		if(!(vehiclepnt->Status & VL_DESTROYED))
			planepnt->AI.lGroundTargetFlag = -1;
		vehiclepnt->WayPosition = planepnt->WorldPosition;
		vehiclepnt->RadarWeaponData[3].Target = planepnt;
		vehiclepnt->lAIFlags1 |= (V_JUST_TO_WAY|SAR_CHECKED);
		vehiclepnt->Status &= ~(VL_INVISIBLE|VL_DONE_MOVING);
		vehiclepnt->Status |= (VL_MOVING);
		vehiclepnt = (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
	}
}

//**************************************************************************************
void AI_SAR_Mark(PlaneParams *planepnt)
{
	MovingVehicleParams *vehiclepnt;
	FPoint vel(0.0f);
	DWORD hCanister;
	FPointDouble position;
	WeaponParams *W;
	int Index;

	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
		if(!vehiclepnt)
			return;

		if(vehiclepnt->Status & VL_DESTROYED)
		{
			return;
		}

		position = vehiclepnt->WorldPosition;
	}
	else
	{
		for (W=Weapons; W<=LastWeapon; W++)
		{
			if(W->Flags & WEAPON_INUSE)
			{
				if(W->WeaponMove == ChuteStayOnGround)
				{
					if((W->P) && (!(W->Flags & WEP_DAMAGED_CHUTE)))
					{
						Index = AIGetWeaponIndex(W->Type->TypeNumber);
						if(pDBWeaponList[Index].lWeaponID != 104)
						{
							if((W->P - Planes) == planepnt->AI.lVar2)
							{
								position = W->Pos;
								break;
							}
						}
					}
				}
			}
		}

		if(W > LastWeapon)
			return;
	}

	position.X += (10.0f * FTTOWU);

	hCanister = NewCanister( CT_STEAM_SMOKE, position, vel, 0.0f, 300);
}


//**************************************************************************************
void FlyCruiseMissile(PlaneParams *planepnt)
{
	double offangle, offpitch;
	float tdist;
	float dx, dy, dz;
	double rollval;
	int orgnumwpts;


	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	MBAdjustAIThrust(planepnt, 600, 1);

//	if(tdist < 1000.0)
	orgnumwpts = planepnt->AI.numwaypts;
	if(planepnt->AI.numwaypts > 1)
	{
		if(AIInNextRange(planepnt, tdist * WUTOFT,  5743))
		{
			AINextWayPoint(planepnt);
			offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);
		}
	}
//	AIChangeDir(planepnt, offangle, (dy * 60000 / 6979));

	if(planepnt->FlightStatus & PL_STATUS_CRASHED)
	{
		if(planepnt->WeapLoad[0].Count)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), NULL, -1, planepnt->AI.WayPosition.X, -1.0f, planepnt->AI.WayPosition.Z);
			}
			planepnt->WeapLoad[0].Count = 0;
		}
	}
	else if((orgnumwpts > 1) || ((tdist * WUTOFT) > 6000.0f))
	{
		AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
	}
	else
	{
		tdist = sqrt((dx*dx) + (dz*dz));

		offpitch = atan2(-planepnt->HeightAboveGround, tdist) * 57.2958;

		AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(offpitch));

		planepnt->AI.iAIFlags2 |= AI_CAN_CRASH;

		if((tdist < (50.0f * FTTOWU)) || (fabs(offangle) > 45))
		{
			planepnt->WorldPosition.Y = LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z) - 0.5f;
			if(planepnt->WeapLoad[0].Count)
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), NULL, -1, planepnt->AI.WayPosition.X, -1.0f, planepnt->AI.WayPosition.Z);
				}
				planepnt->WeapLoad[0].Count = 0;
			}
			CrashPlane(planepnt,PL_STATUS_CRASHED,0,NULL);
		}
	}

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
//	planepnt->RollPercentage = 0.75;	//  when max roll was 45.0
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	//	double workval;
	//  workval = 0.008 * offangle;
	//  if(fabs(workval) > 90)
	//		workval = (workval < 0) ? -90 : 90;
	//	planepnt->DesiredRoll = AIConvert180DegreeToAngle(workval);



	return;
}

extern void DropWeaponCamera();
//**************************************************************************************
void AICheckWeaponAsPlaneCameraSwap(PlaneParams *planepnt)
{
	WeaponParams *W;

	W = &Weapons[(int)planepnt->AI.fVarA[1]];
	if(W->WeaponMove != WMovePhantomWeapon)
	{
		W = NULL;
	}

	if((Camera1.AttachedObjectType == CO_WEAPON) && (W))
	{
		if(W == (WeaponParams *)Camera1.AttachedObject)
		{
			if((Camera1.CameraMode == CAMERA_FIXED_CHASE) || (Camera1.CameraMode == CAMERA_CHASE))
				DropWeaponCamera();
		}
	}
	else if((Camera1.AttachedObjectType == CO_PLANE) && WIsWeaponPlane((PlaneParams *)Camera1.AttachedObject))
	{
		if((Camera1.CameraMode == CAMERA_FIXED_CHASE) || (Camera1.CameraMode == CAMERA_CHASE))
		{
//			ChangeViewModeOrSubject(CAMERA_FLYBY);
			ChangeViewModeOrSubject(CAMERA_RC_VIEW);

			double speed = (double)Camera1.SubjectVelocity.Length() * (5.5 + frand()*5.5);

			if (rand()&1)
				speed = -speed;

			Camera1.CameraTargetLocation.SetValues(12.0f+frand()*12.0f,Camera1.SubjectVelocity);

			Camera1.CameraTargetLocation.X += (double)Camera1.SubjectMatrix.m_Data.RC.R0C0 * speed;
			Camera1.CameraTargetLocation.Y += (double)Camera1.SubjectMatrix.m_Data.RC.R1C0 * speed;
			Camera1.CameraTargetLocation.Z += (double)Camera1.SubjectMatrix.m_Data.RC.R2C0 * speed;

			Camera1.CameraTargetLocation += Camera1.SubjectLocation;

			Camera1.RELATIVE_T_DIST = 0.8;
			Camera1.Flags &= ~CF_TS_SET;
		}
	}
}


//**************************************************************************************
void WeaponAsPlane(PlaneParams *planepnt)
{
	double offangle, offpitch;
	float tdist;
	float dx, dy, dz;
	double rollval;
	MovingVehicleParams *vtarget;
	PlaneParams *ptarget;
	BasicInstance *target;
	FPoint position;
	float tempdy;
	float range;
	float tdistmoved = 0;

	if(planepnt->AI.lPlaneID == 85)
	{
		TALDAsPlane(planepnt);
		return;
	}

	planepnt->AI.iAIFlags2 &= ~AI_CAN_CRASH;
	if(planepnt->AI.lPlaneID == 74)
	{
		planepnt->AI.iAIFlags2 |= AI_CAN_CRASH;
	}

	if((planepnt->AI.CurrWay) && (planepnt->AI.CurrWay == &AIWayPoints[planepnt->AI.startwpts]))
	{
		planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration;
		planepnt->SpeedPercentage = 1.0;
		planepnt->AI.lDesiredSpeed = 400;
//		planepnt->DesiredSpeed = planepnt->BfLinVel.X = 700.0f;
		planepnt->DesiredSpeed = planepnt->BfLinVel.X = AIConvertSpeedToFtPS(planepnt->Altitude, (pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed), 0);
	}

	if (planepnt->AI.pGroundTarget)
	{
		if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
		{
			target = (BasicInstance *)planepnt->AI.pGroundTarget;
			position.X = target->Position.X;
			position.Y = target->Position.Y;
			position.Z = target->Position.Z;
		}
		else if(planepnt->AI.lGroundTargetFlag == AIRCRAFT)
		{
			ptarget = (PlaneParams *)planepnt->AI.pGroundTarget;
			position = ptarget->WorldPosition;
		}
		else
		{
			vtarget = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
			position = vtarget->WorldPosition;
		}
	}
	else
	{
		position = planepnt->AI.WayPosition;
		position.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z);
	}

	planepnt->AI.lTimer2 -= DeltaTicks;
	if(planepnt->AI.lTimer2 < 0)
	{
		planepnt->AI.lTimer2 = 3000;
		DBWeaponType *pDBWeapon = &pDBWeaponList[ planepnt->WeapLoad[0].WeapIndex ];

		if(planepnt->AI.CurrWay)
		{
			tdist = QuickDistance(((AIWayPoints[planepnt->AI.startwpts].lWPx * FTTOWU) - planepnt->WorldPosition.X), ((AIWayPoints[planepnt->AI.startwpts].lWPz * FTTOWU) - planepnt->WorldPosition.Z)) * WUTONM;
		}
		else
		{
			tdistmoved = tdist = QuickDistance((planepnt->AI.WorldFormationPos.X - planepnt->WorldPosition.X), (planepnt->AI.WorldFormationPos.Z - planepnt->WorldPosition.Z)) * WUTONM;
		}

		if((pDBWeapon->iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) && (!planepnt->AI.pGroundTarget) && (planepnt->AI.fVarA[0] < tdist))
		{
			AIWeaponPlaneShipSearch(planepnt);
		}

		if(planepnt->AI.CurrWay)
		{
			tempdy = planepnt->AI.WayPosition.Y;
		}
		else
		{
			switch(planepnt->AI.lVar3)
			{
				case MED_ATTACK:
					tempdy = (10000.0f * FTTOWU);
					break;
				case HIGH_ATTACK:
					tempdy = (20000.0f * FTTOWU);
					break;
				case LOW_POP:
				case LOW_LOW:
				default:
					tempdy = 0;
					break;
			}
		}

		if(planepnt->AI.CurrWay)
		{
			range = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iRange;
		}
		else
		{
			range = (float)pDBWeapon->iRange;
		}

		if(tdist > (range * CalcAltEffectOnMissile(tempdy)))
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag, position.X, position.Y, position.Z);
			}
			planepnt->WeapLoad[0].Count = 0;
			planepnt->Status = 0;
			planepnt->AI.iAIFlags2 = 0;

			OrphanAllPlaneSmoke(planepnt);
		}
	}
	else if(!planepnt->AI.CurrWay)
	{
		tdistmoved = QuickDistance((planepnt->AI.WorldFormationPos.X - planepnt->WorldPosition.X), (planepnt->AI.WorldFormationPos.Z - planepnt->WorldPosition.Z)) * WUTONM;
	}


	if(position.Y < 0)
	{
		position.Y = 0;
	}

	if(position.X >= 0)
	{
		offangle = AIComputeHeadingToPoint(planepnt, position, &tdist, &dx ,&dy, &dz, 1);
	}
	else
	{
		offangle = 0;

		if(planepnt->AI.CurrWay)
		{
			dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		}
		else
		{
			switch(planepnt->AI.lVar3)
			{
				case MED_ATTACK:
					dy = (10000.0f * FTTOWU) - planepnt->WorldPosition.Y;
					break;
				case HIGH_ATTACK:
					dy = (20000.0f * FTTOWU) - planepnt->WorldPosition.Y;
					break;
				case LOW_POP:
				case LOW_LOW:
				default:
//					dy = (200.0f * FTTOWU) - planepnt->WorldPosition.Y;
					dy = (200.0f * FTTOWU) - planepnt->HeightAboveGround;
					break;
			}
		}
	}

//	MBAdjustAIThrust(planepnt, 600, 1);
	planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed, 0);

	if(planepnt->FlightStatus & PL_STATUS_CRASHED)
	{
		if(planepnt->WeapLoad[0].Count)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
//				InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), NULL, -1, position.X, -1.0f, position.Z);
				InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), NULL, -1, position.X, position.Y, position.Z);
			}
			planepnt->WeapLoad[0].Count = 0;
			planepnt->Status = 0;
			planepnt->AI.iAIFlags2 = 0;
			OrphanAllPlaneSmoke(planepnt);

			return;
		}
	}
	else if((!planepnt->AI.CurrWay) && (tdistmoved < 0.25f) && (planepnt->HeightAboveGround > (500.0f * FTTOWU)))
	{
		planepnt->DesiredPitch = planepnt->Pitch;
		if(AIConvertAngleTo180Degree(planepnt->DesiredPitch > -5.0f))
		{
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(-5.0f);
		}

		planepnt->DesiredRoll = 0;
	}
	else if(position.X < 0)
	{
		AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
	}
	else if((planepnt->AI.CurrWay) && (planepnt->AI.numwaypts > 1))
	{
		if(planepnt->AI.CurrWay)
		{
			tempdy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		}
		else
		{
			switch(planepnt->AI.lVar3)
			{
				case MED_ATTACK:
					tempdy = (10000.0f * FTTOWU) - planepnt->WorldPosition.Y;
					break;
				case HIGH_ATTACK:
					tempdy = (20000.0f * FTTOWU) - planepnt->WorldPosition.Y;
					break;
				case LOW_POP:
				case LOW_LOW:
				default:
					tempdy = dy;
					break;
			}
		}

		if(dy < tempdy)
			dy = tempdy;

		AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

		if((tdist < (1000.0f * FTTOWU)) && (fabs(offangle) > 45.0f))
		{
			AINextWayPoint(planepnt);
		}
	}
	else if((planepnt->AI.lVar3 == LOW_POP) && ((tdist * WUTOFT) < 12000.0f)  && ((tdist * WUTOFT) > 9000.0f) && (tdist > fabs(dy * 3.0f)))
	{
		planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
		planepnt->DesiredPitch = 0x1800;
	}
	else if(((tdist * WUTOFT) > 6000.0f) && (tdist > fabs(dy * 3.0f)))
	{
		if(planepnt->AI.CurrWay)
		{
			tempdy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		}
		else
		{
			switch(planepnt->AI.lVar3)
			{
				case MED_ATTACK:
					tempdy = (10000.0f * FTTOWU) - planepnt->WorldPosition.Y;
					break;
				case HIGH_ATTACK:
					tempdy = (20000.0f * FTTOWU) - planepnt->WorldPosition.Y;
					break;
				case LOW_POP:
				case LOW_LOW:
				default:
					tempdy = dy;
					break;
			}
		}

		if(dy < tempdy)
			dy = tempdy;

		AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
	}
	else
	{
		tdist = sqrt((dx*dx) + (dz*dz));

		offpitch = atan2(-planepnt->HeightAboveGround, tdist) * 57.2958;

		AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(offpitch));

		planepnt->AI.iAIFlags2 |= AI_CAN_CRASH;

//		if((tdist < (50.0f * FTTOWU)) || (fabs(offangle) > 45))
		if((tdist < (250.0f * FTTOWU)) || (fabs(offangle) > 45))
		{
			AICheckWeaponAsPlaneCameraSwap(planepnt);
			if(planepnt->WeapLoad[0].Count)
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
//					InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), NULL, -1, position.X, -1.0f, position.Z);
					InstantiateBomb(planepnt, 0, (1000 * (1.0f/5.7435)), NULL, -1, position.X, position.Y, position.Z);
				}
				planepnt->WeapLoad[0].Count = 0;
			}
			planepnt->AI.iAIFlags2 &= ~AI_CAN_CRASH;
//			planepnt->WorldPosition.Y = LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z) - 0.5f;
		  	CrashPlane(planepnt,PL_STATUS_CRASHED,0,NULL);
			planepnt->Status = 0;
			planepnt->AI.iAIFlags2 = 0;

			OrphanAllPlaneSmoke(planepnt);
			return;
		}
		else if((tdist * WUTOFT) < (planepnt->V * 4.0f))
		{
			AICheckWeaponAsPlaneCameraSwap(planepnt);
		}
	}

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0

	if((AIConvertAngleTo180Degree(planepnt->Pitch) > 70.0f) && (planepnt->HeightAboveGround < (2000.0f * FTTOWU)))
	{
		planepnt->DesiredPitch = planepnt->Pitch;
	}
	else if(planepnt->Smoke[SMOKE_ENGINE_REAR] && (!(planepnt->DamageFlags)) && (AIConvertAngleTo180Degree(planepnt->Pitch) < 35.0f))
	{
		OrphanAllPlaneSmoke(planepnt);
		planepnt->Smoke[SMOKE_ENGINE_REAR] = NULL;
		planepnt->EngineFires[SMOKE_ENGINE_REAR].TotalTimeLeft = 0;
		planepnt->EngineFires[SMOKE_ENGINE_REAR].LoopsTillNextGenerator = 0;
		planepnt->EngineFires[SMOKE_ENGINE_REAR].LoopsTillNextSprite = 0;
	}

	return;
}

//**************************************************************************************
void TALDAsPlane(PlaneParams *planepnt)
{
	float dpitch;
	float fworkvar, fworkvar2;


#ifdef _DEBUG
	planepnt->AI.lVar2 += DeltaTicks;
	fworkvar = (float)planepnt->AI.lVar2 / 1000.0f;
	fworkvar2 = fworkvar / 60.0f;
	dpitch = QuickDistance((planepnt->AI.WorldFormationPos.X - planepnt->WorldPosition.X), (planepnt->AI.WorldFormationPos.Z - planepnt->WorldPosition.Z)) * WUTONM;
#endif

	planepnt->AI.iAIFlags2 |= AI_CAN_CRASH;
	if(((float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed) < ((float)planepnt->IndicatedAirSpeed * 0.75f))
	{
		planepnt->DesiredPitch = 0;
	}
	else
	{
		if(((float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed) < ((float)planepnt->IndicatedAirSpeed))
		{
			dpitch = -10.0f;
		}
		else
		{
			fworkvar = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed - ((float)planepnt->IndicatedAirSpeed * 0.75f);
			fworkvar2 = (float)planepnt->IndicatedAirSpeed - (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed;
			if(!fworkvar2)
			{
				fworkvar2 = 1.0f;
			}

			dpitch = -10.0f * (fworkvar / fworkvar2);

			if(dpitch > 0)
			{
				dpitch = -10.0f;
			}
		}

		planepnt->DesiredPitch = AIConvert180DegreeToAngle(dpitch);
	}
	planepnt->DesiredRoll = 0;

	if((((float)pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed) < ((float)planepnt->IndicatedAirSpeed * 2.0f)) && (AIConvertAngleTo180Degree(planepnt->Pitch) > -35.0f))
	{
		// 50 hz version	planepnt->DesiredSpeed = planepnt->V - ((32.0f / 7.0f)*FTTOWU/2500.0f);  //  32 / 7 is GBU-15 max Glide
		// 	planepnt->DesiredSpeed = planepnt->V - ((32.0f / 7.0f)) * (DeltaTicks / 1000.0f);  //  32 / 7 is GBU-15 max Glide
		//  planepnt->DesiredSpeed = planepnt->V - ((32.0f / 7.0f)) * ((DeltaTicks / 1000.0f) * (DeltaTicks / 1000.0f));  //  32 / 7 is GBU-15 max Glide
		//	planepnt->DesiredSpeed = planepnt->V - ((32.0f)) * ((DeltaTicks / 1000.0f) * (DeltaTicks / 1000.0f));  //  32 / 7 is GBU-15 max Glide
		planepnt->DesiredSpeed = planepnt->V - ((32.0f / 11.0f)) * (DeltaTicks / 1000.0f);  //  32 / 7 is GBU-15 max Glide
	}

	fworkvar = -FSinTimes(planepnt->Pitch, GRAVITY * (DeltaTicks / 1000.0f));
	planepnt->DesiredSpeed += fworkvar;

	if((planepnt->FlightStatus & PL_STATUS_CRASHED) || ((planepnt->HeightAboveGround * WUTOFT) < 50.0f))
	{
		planepnt->Status = 0;
		planepnt->AI.iAIFlags2 = 0;

		OrphanAllPlaneSmoke(planepnt);
	}
}

//**************************************************************************************
int AIGetNumTankingWithPlayer(PlaneParams *planepnt)
{
	int cnt = 10;
	PlaneParams *checkplane, *leadplane;

	//  used to be PlayerPlane but had to update it for multiplayer
	leadplane = AIGetLeader(planepnt);

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if(checkplane == planepnt)
		{
			return(cnt);
		}

		if(checkplane->AI.Behaviorfunc == AIFlyTankerFormation)
		{
			if ((!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide == checkplane->AI.iSide))
			{
//				if(AIGetLeader(checkplane) == PlayerPlane)
				if(AIGetLeader(checkplane) == leadplane)
				{
					cnt ++;
				}
			}
		}
	}

	return(cnt);
}

//**************************************************************************************
PlaneParams *AIAvoidLandingCrash(PlaneParams *planepnt)
{
	PlaneParams *checkplane;
	int headingcheck;
	float pwidth, plength, cwidth, clength;
	float dwidth, dlength, gwidth, glength;

	pwidth = planepnt->Type->ShadowLRXOff;
	plength = planepnt->Type->ShadowLRZOff;

	if((planepnt->Heading > 0xE000) || (planepnt->Heading <= 0x2000))
	{
		headingcheck = 0;
	}
	else if((planepnt->Heading > 0x2000) && (planepnt->Heading <= 0x6000))
	{
		headingcheck = 1;
	}
	else if((planepnt->Heading > 0x6000) && (planepnt->Heading <= 0xA000))
	{
		headingcheck = 2;
	}
	else
	{
		headingcheck = 3;
	}



	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if((checkplane->AI.Behaviorfunc == AILandingTaxi) || (checkplane->AI.Behaviorfunc == AIParked))
		{
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
			{
				cwidth = checkplane->Type->ShadowLRXOff;
				clength = checkplane->Type->ShadowLRZOff;
				gwidth = cwidth + pwidth + (20.0f * FTTOWU);
				glength = clength + plength + (75.0f * FTTOWU);

				switch(headingcheck)
				{
					case 0:
						dwidth = fabs(checkplane->WorldPosition.X - planepnt->WorldPosition.X);
						dlength = planepnt->WorldPosition.Z - checkplane->WorldPosition.Z;
						if((dwidth < gwidth) && (dlength > 0) && (dlength < glength))
						{
							return(checkplane);
						}
						break;
					case 1:
						dwidth = fabs(checkplane->WorldPosition.Z - planepnt->WorldPosition.Z);
						dlength = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
						if((dwidth < gwidth) && (dlength > 0) &&  (dlength < glength))
						{
							return(checkplane);
						}
						break;
					case 2:
						dwidth = fabs(checkplane->WorldPosition.X - planepnt->WorldPosition.X);
						dlength = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
						if((dwidth < gwidth) && (dlength > 0) && (dlength < glength))
						{
							return(checkplane);
						}
						break;
					default:
						dwidth = fabs(checkplane->WorldPosition.Z - planepnt->WorldPosition.Z);
						dlength = planepnt->WorldPosition.X - checkplane->WorldPosition.X;
						if((dwidth < gwidth) && (dlength > 0) && (dlength < glength))
						{
							return(checkplane);
						}
						break;
				}
			}
		}
	}

	return(NULL);
}

//**************************************************************************************
void AIDeathSpiral(PlaneParams *planepnt)
{
	float fworkval;
	ANGLE tempangle;

#if 0
	fworkval = 0x4000;
	fworkval *= ((float)DeltaTicks / 1000) * 4.0f;
	tempangle = fworkval;

	planepnt->DesiredRoll = -(planepnt->Roll + tempangle);
#endif

	if(planepnt->OnGround)
		return;

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		tempangle = -(planepnt->Roll + 0x4000);
		planepnt->DesiredRoll = tempangle;
		if((planepnt->DesiredPitch > 0xD000) || (planepnt->DesiredPitch < 0x8000))
		{
			fworkval = 0x800;
			fworkval *= ((float)DeltaTicks / 1000);
			tempangle = fworkval;

			planepnt->DesiredPitch = planepnt->Pitch - tempangle;
		}
		planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration;	//15
	}
	else
	{
		if(planepnt->Roll < 0x8000)
		{
			planepnt->DesiredRoll = 0xE800;
		}
		else
		{
			planepnt->DesiredRoll = 0x1800;
		}
		planepnt->DesiredPitch = 0xD800;
		planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration * 4.0f;	//15
	}

	planepnt->DesiredSpeed = planepnt->V;

	if(planepnt->DesiredSpeed < (350.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS)))
	{
		planepnt->DesiredSpeed = (350.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS));
	}
}

//**************************************************************************************
void AIVanishPlane(PlaneParams *planepnt)
{
	int replacenum = -1;
	PlaneParams *replaceplane;
	PlaneParams *leadplane;
	int placeingroup;

	if(planepnt->AI.iVar1 > 0)
	{
		AIMovePlaneToEndOfAttack(planepnt);
		planepnt->AI.iVar1 = 0;
	}

	CheckCurrentTacanStillValid(AIRCRAFT, planepnt);

	if(planepnt->AI.wingman >= 0)
	{
		replacenum = planepnt->AI.wingman;
	}
	else
	{
		replacenum = planepnt->AI.nextpair;
	}

	AIUpdatePlanesInActions(planepnt - Planes, replacenum, AIGetLeader(planepnt) - Planes);
	if(replacenum == -1)
	{
		AIUpdatePlaneLinks(planepnt , NULL);
	}
	else
	{
		AIUpdatePlaneLinks(planepnt , &Planes[replacenum]);
	}

	leadplane = AIGetLeader(planepnt);

	if((leadplane == planepnt) && (replacenum >= 0))
	{
		AIUpdatePathObjLinks(planepnt - Planes, replacenum);
	}

	if(replacenum >= 0)
	{
		if(planepnt->AI.LinkedPlane != NULL)
		{
			if(planepnt->AI.LinkedPlane->AI.LinkedPlane == planepnt)
			{
				planepnt->AI.LinkedPlane->AI.LinkedPlane = &Planes[replacenum];
			}
			Planes[replacenum].AI.LinkedPlane = planepnt->AI.LinkedPlane;
		}

		replaceplane = &Planes[replacenum];
		replaceplane->AI.winglead = planepnt->AI.winglead;
		replaceplane->AI.prevpair = planepnt->AI.prevpair;
		if(replacenum != planepnt->AI.nextpair)
		{
			replaceplane->AI.nextpair = planepnt->AI.nextpair;
			if(replaceplane->AI.nextpair >= 0)
			{
				Planes[replaceplane->AI.nextpair].AI.prevpair = replacenum;
			}
		}

		if(planepnt->AI.Behaviorfunc != AIMoveToParkingSpace)
		{
			replaceplane->AI.WayPosition = planepnt->AI.WayPosition;
#if 0  //  We shouldn't need this since if replacenum != wingman then wingman == -1 and could overwrite replacenum's wingman
			if(replacenum != planepnt->AI.wingman)
			{
				replaceplane->AI.wingman = planepnt->AI.wingman;
				if(replaceplane->AI.wingman >= 0)
				{
					Planes[replaceplane->AI.wingman].AI.winglead = replacenum;
				}
			}
#endif
			replaceplane->AI.lDesiredSpeed = planepnt->AI.lDesiredSpeed;
			replaceplane->AI.FormationPosition = planepnt->AI.FormationPosition;
			placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);
			if(placeingroup == 0)
			{
				if(!AICCheckVoiceNumInGroup(replaceplane - Planes, replaceplane->AI.lAIVoice, (replaceplane->AI.iAIFlags1 & AIFLIGHTNUMS)))
				{
					planepnt->AI.iAIFlags1 &= ~AIFLIGHTNUMS;
					planepnt->AI.iAIFlags1 |= replaceplane->AI.iAIFlags1 & AIFLIGHTNUMS;
					replaceplane->AI.iAIFlags1 &= ~AIFLIGHTNUMS;
				}
			}

			if(planepnt->Status & PL_AI_DRIVEN)
			{
				if(planepnt->AI.OrgBehave == AIFlyEscort)
				{
					replaceplane->AI.iAICombatFlags1 |= (planepnt->AI.iAICombatFlags1 & AI_ESCORT_PART_LEAD);
				}

				replaceplane->AI.Behaviorfunc = planepnt->AI.Behaviorfunc;
				replaceplane->AI.OrgBehave = planepnt->AI.OrgBehave;
			}
			else
			{
				replaceplane->AI.numwaypts = planepnt->AI.numwaypts;	//  I think I wasn't doing this in general
				replaceplane->AI.startwpts = planepnt->AI.startwpts;	//  so I didn't screw up planes that were in
				replaceplane->AI.CurrWay = planepnt->AI.CurrWay;		//  the middle of bombing.  SRE

				AISetUpWayPointActions(replaceplane, replaceplane->AI.CurrWay);

				if(replaceplane->AI.wingman >= 0)
				{
					AIRestoreWaypointBehavior(replaceplane->AI.wingman, replaceplane->AI.CurrWay, replaceplane->AI.numwaypts);
				}
				if(replaceplane->AI.nextpair >= 0)
				{
					AIRestoreWaypointBehavior(replaceplane->AI.nextpair, replaceplane->AI.CurrWay, replaceplane->AI.numwaypts);
				}
			}
		}
	}
	else if(planepnt->AI.LinkedPlane != NULL)
	{
		if(planepnt->AI.LinkedPlane->AI.LinkedPlane == planepnt)
		{
			planepnt->AI.LinkedPlane->AI.LinkedPlane = NULL;
		}
	}


	if(planepnt->AI.winglead >= 0)
	{
		Planes[planepnt->AI.winglead].AI.wingman = replacenum;
	}
	if(planepnt->AI.prevpair >= 0)
	{
		Planes[planepnt->AI.prevpair].AI.nextpair = replacenum;
	}
	planepnt->AI.winglead = -1;
	planepnt->AI.wingman = -1;
	planepnt->AI.prevpair = -1;
	planepnt->AI.nextpair = -1;

	planepnt->Status = 0;
}


//**************************************************************************************
void AIFlyToCASOrbitPoint(PlaneParams *planepnt)
{
	FPoint orbitpoint;
	float tdist, dx, dy, dz, offangle;

	AIUpdateAttackOnVehicle(planepnt);

	AIGetOrbitOffset(planepnt, &orbitpoint);
	tdist = FlyToPoint(planepnt, orbitpoint);
	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
	{
		planepnt->AI.Behaviorfunc = AIFlyCASOrbit;
	}
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx,dz) * WUTOFT;
	if(AIInNextRange(planepnt, tdist,  planepnt->AI.lVar2, 25.0f))
	{
		offangle = AIComputeHeadingToPoint(planepnt, orbitpoint, &tdist, &dx ,&dy, &dz, 1);
		if(fabs(offangle) > 60)
		{
			planepnt->DesiredRoll = 0;
		}
//				planepnt->AI.Behaviorfunc = AIFlyOrbit;
	}
}

//**************************************************************************************
void AIFlyCASOrbit(PlaneParams *planepnt)
{
	float dx, dy, dz, tdist;
	float ftoffset;

	AIUpdateAttackOnVehicle(planepnt);

	ftoffset = planepnt->AI.lVar2;

	AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 0);
	if((tdist * WUTOFT) >  (ftoffset * 2))
	{
		planepnt->AI.Behaviorfunc = AIFlyToOrbitPoint;
		AIFlyToCASOrbitPoint(planepnt);
		return;
	}

	planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
	dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
}

//************************************************************************************************
float AIGetTimeBulletInterceptGround(FPointDouble targetpos, float speedfps, FPointDouble gunposition, float bulletvel)
{
	FPointDouble Delta;
	double ddist;
	float vel1, vel2, time;
	float offangle;

	Delta.X = (targetpos.X - gunposition.X);
	Delta.Y = (targetpos.Y - gunposition.Y);
	Delta.Z = (targetpos.Z - gunposition.Z);

	ddist = sqrt((Delta.X*Delta.X) + (Delta.Y*Delta.Y) + (Delta.Z*Delta.Z)) * WUTOFT;

	if(speedfps)
	{
		offangle = atan2(Delta.X, Delta.Z) * 57.2958;

		vel1 = cos(DegToRad(offangle)) * speedfps;
	}
	else
	{
		vel1 = 0;
	}

	vel2 = bulletvel;

	time = (ddist) / (vel1 + vel2);

  	return(time);
}

//************************************************************************************************
void GDComputeGunHPGround(void *ptarget, long targettype, FPointDouble gunposition, FPoint *aimpoint, float bulletvel, float *heading, float *pitch, float *usetime, float timemod, int truepitch)
{
	float disttravel;
	float fworkvar;
	float time, timesqr;
	float dx, dy, dz, offangle, offpitch;
	double ddist;
	FPointDouble targetpos;
	float speedfps = 0;
	MovingVehicleParams *vehicle = NULL;
	BasicInstance *walker = NULL;

	if(targettype == MOVINGVEHICLE)
	{
		vehicle = (MovingVehicleParams *)ptarget;
	}
	else
	{
		walker = (BasicInstance *)ptarget;
	}


	if(*usetime < 0)
	{
		time = AIGetTimeBulletInterceptGround(targetpos, speedfps, gunposition, bulletvel);
		*usetime = time;
	}
	else
	{
		time = *usetime;
	}
	time += timemod;

	if(vehicle)
	{
		speedfps = vehicle->fSpeedFPS;
		disttravel = speedfps * time;

		fworkvar = (sin(DegToRad((double)vehicle->Heading / DEGREE)) * disttravel);
		aimpoint->X = vehicle->WorldPosition.X - (fworkvar / WUTOFT);

		aimpoint->Y = vehicle->WorldPosition.Y;

		fworkvar = (cos(DegToRad((double)vehicle->Heading / DEGREE)) * disttravel);
		aimpoint->Z = vehicle->WorldPosition.Z - (fworkvar / WUTOFT);
	}
	else
	{
		aimpoint->X = walker->Position.X;
		aimpoint->Y = walker->Position.Y;
		aimpoint->Z = walker->Position.Z;
	}

	dx = aimpoint->X - gunposition.X;
	dy = aimpoint->Y - gunposition.Y;
	dz = aimpoint->Z - gunposition.Z;

	offangle = atan2(-dx, -dz) * 57.2958;

	*heading = offangle;

#if 1
	ddist = sqrt((dx*dx) + (dy*dy) + (dz*dz)) * WUTOFT;


	timesqr = (ddist / bulletvel);
	timesqr = timesqr * timesqr;

	dy *= WUTOFT;
	dy += 0.5 * GRAVITY * timesqr;
#else
	ddist = sqrt((dx*dx) + (dy*dy) + (dz*dz));

	timesqr = (ddist / (bulletvel * FTTOWU));
	timesqr = timesqr * timesqr;

	float dy1, dy2, dyft, ddistft;
	ddistft = ddist * WUTOFT;

	dy1 = 0.5 * GRAVITY * timesqr * WUTOFT;
	dy += 0.5 * GRAVITY * timesqr * FTTOWU;

	dyft = dy * WUTOFT;
	offpitch = atan2(dy, (float)ddist) * 57.2958;

	ddist = sqrt((dx*dx) + (dy*dy) + (dz*dz)) * WUTOFT;

	timesqr = (ddist / bulletvel);
	timesqr = timesqr * timesqr;

	dy *= WUTOFT;
	dy2 = 0.5 * GRAVITY * timesqr;
	dy += 0.5 * GRAVITY * timesqr;
#endif

	offpitch = atan2(dy, (float)ddist) * 57.2958;

	if((offpitch < 5.0f) && (!truepitch))
	{
		offpitch = 5.0f;
	}

	*pitch = offpitch;
}

//**************************************************************************************
void AISendGroupHome(PlaneParams *planepnt)
{
	TakeOffActionType TempActionTakeOff;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	PlaneParams *tempplane, *tempwing;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		planepnt->AI.Behaviorfunc = AIFlyToLandingPoint;
		if(AIGetClosestAirField(planepnt, planepnt->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
		{
			planepnt->AI.WayPosition.X = TempActionTakeOff.lRunwayStartX * FTTOWU;
			planepnt->AI.WayPosition.Y = TempActionTakeOff.lRunwayStartY * FTTOWU;
			planepnt->AI.WayPosition.Z = TempActionTakeOff.lRunwayStartZ * FTTOWU;
		}
		else if(AIGetClosestAirField(planepnt, planepnt->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 1))
		{
			planepnt->AI.WayPosition.X = TempActionTakeOff.lRunwayStartX * FTTOWU;
			planepnt->AI.WayPosition.Y = TempActionTakeOff.lRunwayStartY * FTTOWU;
			planepnt->AI.WayPosition.Z = TempActionTakeOff.lRunwayStartZ * FTTOWU;
		}
	}
	else
	{
		if(AIGetClosestAirField(planepnt, planepnt->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
		{
			AISetPlaneLandingFlyToField(planepnt, &TempActionTakeOff, runwayheading);
		}
		else if(AIGetClosestAirField(planepnt, planepnt->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 1))
		{
			AISetPlaneLandingFlyToField(planepnt, &TempActionTakeOff, runwayheading);
		}
	}

	tempplane = planepnt;

	while(tempplane)
	{
		if(tempplane->AI.wingman >= 0)
		{
			tempwing = &Planes[tempplane->AI.wingman];
			if(pDBAircraftList[tempwing->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
			{
				tempwing->AI.Behaviorfunc = AIFlyToLandingPoint;
				tempwing->AI.WayPosition = planepnt->AI.WayPosition;
			}
			else
			{
				AISetPlaneLandingFlyToField(tempwing, &TempActionTakeOff, runwayheading);
			}
		}
		if(tempplane->AI.nextpair >= 0)
		{
			tempplane = &Planes[tempplane->AI.nextpair];
			if(pDBAircraftList[tempplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
			{
				tempplane->AI.Behaviorfunc = AIFlyToLandingPoint;
				tempplane->AI.WayPosition = planepnt->AI.WayPosition;
			}
			else
			{
				AISetPlaneLandingFlyToField(tempplane, &TempActionTakeOff, runwayheading);
			}
		}
		else
		{
			tempplane = NULL;
		}
	}
}

extern void lso_view(VKCODE vk);
extern void carrier_tower_view(VKCODE vk);

//**************************************************************************************
void AIDoViewAction(ViewActionType *pViewAction)
{
	KEYBOARDMAP *pKy;
	CameraObjectType targettype;
	void *ptr = NULL;
	long objid;
	BasicInstance *walker = NULL;
	VKCODE hack;
	int vnum;

	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

	if(pViewAction->lTime > 0)
	{
		lViewTimer = pViewAction->lTime * 1000;
	}
	else
	{
		lViewTimer = -1;
	}

	if(pViewAction->iViewTypeFlag == 0)
	{
		if((pViewAction->lEyePointIndex >= 0) && (pViewAction->lEyePointIndex < g_iNumEyePoints))
		{
			EVActivateEyePoint(pViewAction->lEyePointIndex);
		}
	}
	else
	{
		objid = pViewAction->lObjectID;
		if(pViewAction->lTrackingFlag == 1)
		{
			targettype = CO_PLANE;
			if(objid < 0)
			{
				return;
			}
			if(objid > (LastPlane - Planes))
			{
				return;
			}

			if(Planes[objid].FlightStatus & (PL_STATUS_CRASHED))
			{
				return;
			}

			if(!(Planes[objid].Status & PL_ACTIVE))
			{
				return;
			}

			ptr = &Planes[objid];
		}
		else if(pViewAction->lTrackingFlag == 2)
		{
			targettype = CO_VEHICLE;
			vnum = 0;
			for(int cnt3 = 0; cnt3 < objid; cnt3 ++)
			{
				vnum += g_pMovingVehicleList[cnt3].iNumGroup;
			}

			objid = vnum;

			if(objid < 0)
			{
				return;
			}
			if(objid > (LastMovingVehicle - MovingVehicles))
			{
				return;
			}

			if(!(MovingVehicles[objid].Status & VL_ACTIVE))
			{
				return;
			}

			ptr = &MovingVehicles[objid];
		}
		else if(pViewAction->lTrackingFlag == 4)
		{
			targettype = CO_GROUND_OBJECT;

			walker = FindInstance(AllInstances, objid);

			if(!walker)
			{
				return;
			}

			ptr = walker;
		}

		pKy = KeyFriendlyToFunction(pViewAction->sFunction);
		if(pKy)
		{
			if(ptr)
			{
				AssignCameraSubject(ptr,targettype);
//				AssignCameraTarget(ptr, targettype);
			}

			pKy->pfnKeyFunction(hack);
		}
		else
		{
			return;
		}

		if(ptr)
		{
			AssignCameraSubject(ptr,targettype);
//			AssignCameraTarget(ptr, targettype);
			if(pKy)
			{
				if((pKy->pfnKeyFunction != lso_view) && (pKy->pfnKeyFunction != carrier_tower_view))
				{
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
				}
				else
				{
					pKy->pfnKeyFunction(hack);
				}
			}
			else
			{
 				ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
	}
	if(pViewAction->iDefaultView)
	{
		PreViewActionCamera = Camera1;
		PreViewActionCockpitSeat = CockpitSeat;
		PreViewActionCurrentCockpit = CurrentCockpit;
	}
}

//**************************************************************************************
void AIRestoreViewActionCamera()
{
	OldCamera = PreViewActionCamera;
	OldCockpitSeat    = PreViewActionCockpitSeat;
	OldCurrentCockpit = PreViewActionCurrentCockpit;
//	memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));
	RestorePostExitView();
}

//**************************************************************************************
void AILetFormedOnLand(PlaneParams *planepnt)
{
	int inplayergroup = 0;

	if(!AIInPlayerGroup(planepnt))
	{
		AISetPlaneLandingFlyToField(planepnt, NULL, 0);
	}
	else
	{
		inplayergroup = 1;
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(inplayergroup)
		{
			if(!AIInPlayerGroup(&Planes[planepnt->AI.wingman]))
			{
				Planes[planepnt->AI.wingman].AI.winglead = -1;
			}
		}

		AILetFormedOnLand(&Planes[planepnt->AI.wingman]);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		if(inplayergroup)
		{
			if(!AIInPlayerGroup(&Planes[planepnt->AI.nextpair]))
			{
				Planes[planepnt->AI.nextpair].AI.prevpair = -1;
			}
		}

		AILetFormedOnLand(&Planes[planepnt->AI.nextpair]);
	}
}

//**************************************************************************************
int AIIsCurrentlyBombing(PlaneParams *planepnt)
{
	if(planepnt->AI.Behaviorfunc == AIGroupLevelBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AILevelBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIGroupFlyToDivePoint)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyToDivePoint)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIStartDiveBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIDiveBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIAfterDiveBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIGroupFlyToLoftPoint)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyToLoftPoint)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AILoftBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIGroupFlyToPopPoint)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyToPopPoint)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIPopUpBomb)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIPopUpPullIn)
	{
		return(1);
	}
	return(0);
}


//**************************************************************************************
int AIGroundAlreadyTarget(void *target)
{
	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(target == W->pTarget)
			{
				return(1);
			}
		}
		W++;
	}
	return(0);
}

//**************************************************************************************
void FlyDrone(PlaneParams *planepnt)
{
	if((!planepnt->AI.CurrWay->iSpeed) && (!planepnt->AI.CurrWay->lWPy))
	{
		planepnt->AI.lTimer1 = 60000;  //  Just so we don't vanish the Plane
		AIParked(planepnt);
	}
	else
	{
		AIFlyFormation(planepnt);
	}

	if(planepnt->AI.iAICombatFlags2 & AI_ESCORTABLE)
	{
		AICheckIsEscorted(planepnt);
	}

	return;
}

//**************************************************************************************
void AISetUpAttackTarget(PlaneParams *planepnt, AttackTargetActionType *attack_action)
{
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	float dx, dy, dz, tdist;
	float offpitch;
	int attacktype = 0;

	if(attack_action->lTargetType == AIRCRAFT)
	{
		AIUpdateInterceptGroup(planepnt, &Planes[attack_action->lTargetID], 1);
		return;
	}
	else if(attack_action->lTargetType == GROUNDOBJECT)
	{
		planepnt->AI.lGroundTargetFlag = GROUNDOBJECT;
		walker = FindInstance(AllInstances, attack_action->lTargetID);
		if(!walker)
		  	return;
		planepnt->AI.pGroundTarget = walker;
		planepnt->AI.WayPosition.X = walker->Position.X;
//		planepnt->AI.WayPosition.Y = walker->Position.Y + (500 * FTTOWU);
		planepnt->AI.WayPosition.Z = walker->Position.Z;
		planepnt->AI.TargetPos = walker->Position;
	}
	else if(attack_action->lTargetType == MOVINGVEHICLE)
	{
		planepnt->AI.lGroundTargetFlag = MOVINGVEHICLE;
		vehiclepnt = &MovingVehicles[AIConvertVGroupToVNum(attack_action->lTargetID)];
		planepnt->AI.pGroundTarget = vehiclepnt;
		planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
//		planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y + (500 * FTTOWU);
		planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
		planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
	}
	else
	{
		return;
	}

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx, dz);
	offpitch = (atan2(dy, tdist) * 57.2958);

	if(offpitch > -20)
	{
//		planepnt->AI.Behaviorfunc = AIGroupFlyToPopPoint;
//		attacktype = 1;
		planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
		attacktype = 8;
	}
	else
	{
		planepnt->AI.Behaviorfunc = AIAwayFromTarget;
		attacktype = 2;
	}

	AIDetermineBestBomb(planepnt, planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag);

	if(planepnt->AI.cNumWeaponRelease > 0)
	{
//		if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
//		{
//			AICSEADAttackMsg(planepnt - Planes, 1);
//		}
//		else if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
//		{
//		 	AICWingCASAttackMsg(planepnt - Planes);
//		}
	 	AICWingCASAttackMsg(planepnt - Planes);
	}
	planepnt->AI.iVar1 = 0;
	AIGetPlaceInAttack(planepnt);

	if((!planepnt->OnGround) && (MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutCASData(planepnt, attacktype);
	}
}

//**************************************************************************************
void AISetUpPlayerGroupChuteDrop(PlaneParams *planepnt, MBWayPoints *bombway)
{
	int attacktype = 0;
	int cnt;
	DBWeaponType *pweapon_type;
	int bombtype;

	planepnt->AI.cActiveWeaponStation = -1;
	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(planepnt->WeapLoad[cnt].Count > 0)
		{
			pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
			bombtype = pweapon_type->iWeaponType;
			if((bombtype == WEAPON_TYPE_NAVAL_MINE) || (pweapon_type->iUsageIndex == 22))
			{
				planepnt->AI.cNumWeaponRelease = planepnt->WeapLoad[cnt].Count;
				planepnt->AI.cActiveWeaponStation = cnt;
				break;
			}
		}
	}

	if(planepnt->AI.cActiveWeaponStation == -1)
		return;

	planepnt->AI.WayPosition.X = ConvertWayLoc(bombway->lWPx);
	planepnt->AI.WayPosition.Y = ConvertWayLoc(bombway->lWPy);
	planepnt->AI.WayPosition.Z = ConvertWayLoc(bombway->lWPz);
	planepnt->AI.TargetPos.X = ConvertWayLoc(bombway->lWPx);
	planepnt->AI.TargetPos.Y = ConvertWayLoc(bombway->lWPy);
	planepnt->AI.TargetPos.Z = ConvertWayLoc(bombway->lWPz);

	planepnt->AI.lGroundTargetFlag = 0;
	planepnt->AI.Behaviorfunc = AILevelBomb;

	planepnt->AI.iVar1 = 0;
	AIGetPlaceInAttack(planepnt);
}

//**************************************************************************************
void AIDropChuteMines(PlaneParams *planepnt)
{
	float dy;
	int cnt;
	DBWeaponType *pweapon_type;
	int bombtype;
	PlaneParams *leadplane = AIGetLeader(planepnt);

	if(((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1)) || (leadplane->Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN)))
	{
		planepnt->DesiredRoll = 0;
		dy = AICheckSafeAlt(planepnt, planepnt->WorldPosition.Y) - planepnt->WorldPosition.Y;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);

		if((planepnt->AI.winglead == -1) && (planepnt->AI.prevpair == -1))
		{
			AIUpdateFormation(planepnt);
		}
	}
	else
	{
		AIFormationFlying(planepnt);
	}

	planepnt->AI.lTimer2 -= DeltaTicks;
	if(planepnt->AI.lTimer2 < 0)
	{
		if((planepnt->AI.cNumWeaponRelease <= 0) || (planepnt->AI.cActiveWeaponStation == -1))
		{
			planepnt->AI.cNumWeaponRelease = 0;
			planepnt->AI.cActiveWeaponStation = -1;
			for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
			{
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
					bombtype = pweapon_type->iWeaponType;
					if((bombtype == WEAPON_TYPE_NAVAL_MINE) || (pweapon_type->iUsageIndex == 22))
					{
						planepnt->AI.cNumWeaponRelease = planepnt->WeapLoad[cnt].Count;
						planepnt->AI.cActiveWeaponStation = cnt;
						break;
					}
				}
			}

			if((planepnt->AI.cNumWeaponRelease <= 0) || (planepnt->AI.cActiveWeaponStation == -1))
			{
				if(leadplane->Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN))
				{
					if(AIRIsAttack(planepnt - Planes))
					{
						AICBombDropMsg(planepnt);
					}
					AISetUpEgress(planepnt);
				}
				else
				{
					AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyFormation);
					AINextWayPointNoActions(planepnt);
					AISetUpNewWayPoint(planepnt);
				}
				return;
			}
		}

		//  Drop Bomb/Chutes/Mines here
		InstantiateBomberEjections(planepnt, planepnt->AI.cNumWeaponRelease, planepnt->AI.cActiveWeaponStation, NULL, -1, planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Type);

//		planepnt->AI.cNumWeaponRelease --;
//		planepnt->AI.lTimer2 = 5000;
		planepnt->AI.lTimer2 = 1500 * (planepnt->AI.cNumWeaponRelease + 1);
		planepnt->AI.cNumWeaponRelease = 0;
		planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count = 0;
	}
}

//**************************************************************************************
float AICheckForStraightAttack(PlaneParams *planepnt, FPointDouble worldpos, FPoint targetpos)
{
	FPointDouble position;
	double distance;
	DBWeaponType *pweapon_type;
	double dx, dy, dz, tdist, toffpitch;

	if(((planepnt->AI.WayPosition.Y - targetpos.Y) * WUTOFT) < 2000.0f)
	{
		planepnt->AI.WayPosition.Y = targetpos.Y + (2000.0f * FTTOWU);
	}

	pweapon_type = &pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex];

	position = targetpos;

	dx = position.X - planepnt->WorldPosition.X;
	dy = position.Y - planepnt->WorldPosition.Y;
	dz = position.Z - planepnt->WorldPosition.Z;

//	distance = planepnt->WorldPosition - position;
	distance = tdist = sqrt((dx * dx) + (dz * dz));
	toffpitch = atan2(dy, tdist) * 57.2958;

	if(pweapon_type->iWeaponType == WEAPON_TYPE_GUN)
	{
		if(distance < NMTOWU)
		{
			planepnt->AI.Behaviorfunc = AIStraightGunAttack;
			planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
			planepnt->AI.lTimer1 = -1;
		}
	}
	else
	{
		if(distance < ((float)pweapon_type->iRange * NMTOWU))
		{
			planepnt->AI.Behaviorfunc = AIStraightRocketAttack;
			planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
			planepnt->AI.lTimer1 = -1;
		}
	}

	if(toffpitch < -20.0f)
	{
		if(pweapon_type->iWeaponType == WEAPON_TYPE_GUN)
		{
			planepnt->AI.Behaviorfunc = AIStraightGunAttack;
			planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
			planepnt->AI.lTimer1 = -1;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIStraightRocketAttack;
			planepnt->AI.WayPosition.Y = planepnt->AI.TargetPos.Y;
			planepnt->AI.lTimer1 = -1;
		}
	}

	return(6000.0f);
}

//**************************************************************************************
void AIStraightRocketAttack(PlaneParams *planepnt)
{
	double offangle, toffpitch;
	float tdist;
	float dx, dy, dz;
	double dx2, dz2, tdist2;
	PlaneParams *leadplane;
	int endbomb;
	float weaponspacing = AI_RIPPLE_SPREAD;
	DBWeaponType *pweapon_type;
	pweapon_type = &pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex];

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;

//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, planepnt->V, planepitch);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}
	else if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
	{
		BasicInstance *walker;

		walker = (BasicInstance *)planepnt->AI.pGroundTarget;
		dx2 = (walker->Position.X - planepnt->AI.WayPosition.X) * WUTOFT;
		dy = (walker->Position.Y - planepnt->AI.WayPosition.Y) * WUTOFT;
		dz2 = (walker->Position.Z - planepnt->AI.WayPosition.Z) * WUTOFT;
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	dx2 = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
	dz2 = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;

//	tdist2 = QuickDistance(dx2,dz2);
	tdist2 = sqrt((dx2 * dx2) + (dz2 * dz2));
	toffpitch = atan2(dy, tdist2) * 57.2958;

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

	toffpitch = AIConvertAngleTo180Degree(planepnt->Pitch - planepnt->DesiredPitch);

	if((((fabs(offangle) < 1.0f) && (fabs(toffpitch) < 1.0f)) || (planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)) && (planepnt->AI.lTimer1 < 0) && (tdist < ((float)pweapon_type->iRange * NMTOWU * 0.75f))) //  if releasetime < 1/2 DeltaTicks in seconds
	{

		//  Drop Bomb
//		planepnt->AI.Behaviorfunc = AIFlyFormation;

//		planepnt->AI.Behaviorfunc = AIAfterDiveBomb;
//		AIChangeDirPitch(planepnt, (double)0, AIConvert180DegreeToAngle(30));

		if(planepnt->AI.cNumWeaponRelease)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				InstantiateAGMissile(planepnt, planepnt->AI.cActiveWeaponStation, planepnt->AI.pGroundTarget, planepnt->AI.TargetPos, planepnt->AI.lGroundTargetFlag);
				FPointDouble position = planepnt->AI.WayPosition;
			}
			planepnt->AI.iAIFlags2 |= AI_RIPPLE_BOMB;
			planepnt->AI.cNumWeaponRelease = planepnt->AI.cNumWeaponRelease - 1;
			planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count = planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count - 1;
		}

		if(planepnt->AI.cNumWeaponRelease)
		{
			planepnt->AI.lTimer1 = 500;
			endbomb = 0;
			if(planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count <= 0)
			{
				if(!AISelectNextWeaponStation(planepnt))
				{
					endbomb = 1;
				}
			}
		}
		else
		{
			endbomb = 1;
		}

		if(endbomb)
		{
			planepnt->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;
			if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
			{
				planepnt->AI.iVar1 = 0;
			}
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICBombDropMsg(planepnt);
//				AICMaverickLaunchMsg(planepnt);
			}
			AISetUpEgress(planepnt);
		}
		return;
	}
	else if((fabs(offangle) > 45.0f) && (tdist < NMTOWU * 0.75f)) //  if releasetime < 1/2 DeltaTicks in seconds
	{
		planepnt->AI.Behaviorfunc = AIAwayFromTarget;
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	planepnt->Brakes = 0;

	return;
}

//**************************************************************************************
void AIStraightGunAttack(PlaneParams *planepnt)
{
	double offangle, toffpitch;
	float tdist;
	float dx, dy, dz;
	double dx2, dz2, tdist2;
	PlaneParams *leadplane;
	int endbomb;
	float weaponspacing = AI_RIPPLE_SPREAD;

	planepnt->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	planepnt->AI.iAIFlags1 |= AIFASTPITCHROLL;

//	bombpitch = AIGetPitchForBomb(planepnt->WorldPosition.X, planepnt->WorldPosition.Y, planepnt->WorldPosition.Z, planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Y, planepnt->AI.WayPosition.Z, planepnt->V, planepitch);
	if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		AIUpdateAttackOnVehicle(planepnt);
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 1);

	dx2 = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
	dz2 = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;

//	tdist2 = QuickDistance(dx2,dz2);
	tdist2 = sqrt((dx2 * dx2) + (dz2 * dz2));
	toffpitch = atan2(dy, tdist2) * 57.2958;

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

	toffpitch = AIConvertAngleTo180Degree(planepnt->Pitch - planepnt->DesiredPitch);

	if((((fabs(offangle) < 1.0f) && (fabs(toffpitch) < 1.0f)) || (planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB)) && (planepnt->AI.lTimer1 < 0)) //  if releasetime < 1/2 DeltaTicks in seconds
	{

		//  Drop Bomb
//		planepnt->AI.Behaviorfunc = AIFlyFormation;

//		planepnt->AI.Behaviorfunc = AIAfterDiveBomb;
//		AIChangeDirPitch(planepnt, (double)0, AIConvert180DegreeToAngle(30));

		if(planepnt->AI.cNumWeaponRelease)
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				InstantiateAGMissile(planepnt, planepnt->AI.cActiveWeaponStation, planepnt->AI.pGroundTarget, planepnt->AI.TargetPos, planepnt->AI.lGroundTargetFlag);
			}
			planepnt->AI.iAIFlags2 |= AI_RIPPLE_BOMB;
			planepnt->AI.cNumWeaponRelease = planepnt->AI.cNumWeaponRelease - 1;
			planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count = planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count - 1;
		}

		if(planepnt->AI.cNumWeaponRelease)
		{
			planepnt->AI.lTimer1 = 500;
			endbomb = 0;
			if(planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].Count <= 0)
			{
				if(!AISelectNextWeaponStation(planepnt))
				{
					endbomb = 1;
				}
			}
		}
		else
		{
			endbomb = 1;
		}

		if(endbomb)
		{
			planepnt->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;
			if(!(planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK)))
			{
				planepnt->AI.iVar1 = 0;
			}
			leadplane = AIGetLeader(planepnt);
			if(leadplane == PlayerPlane)
			{
				AICBombDropMsg(planepnt);
//				AICMaverickLaunchMsg(planepnt);
			}
			AISetUpEgress(planepnt);
		}
		return;
	}

	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	planepnt->Brakes = 0;

	return;
}

//**************************************************************************************
void AIUpdateStraightAttackOnVehicle(PlaneParams *planepnt)
{
	MovingVehicleParams *vehiclepnt;
	double dy;
	float time_bomb_in_air;
	FPointDouble workpos;
	double radhead;
	DBWeaponType *pweapon_type;
	double distance;

	if((planepnt->AI.lGroundTargetFlag != MOVINGVEHICLE) || (!planepnt->AI.pGroundTarget))
	{
		return;
	}

	vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;

	if((!(vehiclepnt->Status & VL_MOVING)) || (vehiclepnt->Status & VL_WAITING) || (planepnt->AI.Behaviorfunc == AIFlyCASOrbit))
	{
		planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
		planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y;
		planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
		planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
		return;
	}

	dy = vehiclepnt->WorldPosition.Y - planepnt->WorldPosition.Y;

	pweapon_type = &pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex];

	distance = planepnt->WorldPosition - vehiclepnt->WorldPosition;
	distance *= WUTOFT;

	if(pweapon_type->iWeaponType == WEAPON_TYPE_AGROCKET)
	{
		time_bomb_in_air = distance = MachAToFtSec((20000.0f), (float)pweapon_type->fMaxSpeed);
	}
	else if(pweapon_type->iWeaponType == WEAPON_TYPE_GUN)
	{
		time_bomb_in_air = distance / GDGetWeaponVelocity(pweapon_type);
	}
	else
	{
		planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
		planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y;
		planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
		planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
		return;
	}

	radhead = DegToRad(AIConvertAngleTo180Degree(vehiclepnt->Heading));

	workpos = vehiclepnt->WorldPosition;

	workpos.X -= (sin(radhead) * vehiclepnt->fSpeedFPS * FTTOWU * time_bomb_in_air);
	workpos.Z -= (cos(radhead) * vehiclepnt->fSpeedFPS * FTTOWU * time_bomb_in_air);

	if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
	{
#ifdef V_USE_INST_HEIGHT
		GetHeightWithInstances(workpos,AllFoundations);
#else
		workpos.Y = LandHeight(workpos.X, workpos.Z);
#endif
	}
	else
	{
		workpos.Y = vehiclepnt->WorldPosition.Y + (vehiclepnt->fSpeedFPS * FTTOWU * time_bomb_in_air * (-vehiclepnt->fSinPitch));
	}

	planepnt->AI.WayPosition.X = workpos.X;
	planepnt->AI.WayPosition.Y = workpos.Y;
	planepnt->AI.WayPosition.Z = workpos.Z;
	planepnt->AI.TargetPos = workpos;
}

//**************************************************************************************
void AIWeaponPlaneShipSearch(PlaneParams *planepnt)
{
	MovingVehicleParams *vehiclepnt;
  	void *foundobject = NULL;
	long objectflag;
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	float offangle, toffangle, toffpitch, offpitch;
	int widthok, heightok, visualok;
	float radarangleyaw, radaranglepitch;
	int nlfrmult, nlrfrmult, frmult, rfrmult;
	int attacktype = 0;
	float searchtypemod;
	DBShipType *pshiptype;

	nlfrmult = nlrfrmult = frmult = rfrmult = 0;

	visualrangenm = AI_VISUAL_RANGE_NM;
	visualrange = visualrangenm * NMTOWU;
	rangenm = visualrangenm;  //  Get this from database SRE

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	foundobject = NULL;
	widthangle = 20;	//  might get from database
	heightangle = 20;	//  might get from database

	radarangleyaw = AIConvertAngleTo180Degree(planepnt->Heading);

	radaranglepitch = AIConvertAngleTo180Degree(planepnt->Pitch);

	for(vehiclepnt = MovingVehicles; (vehiclepnt <= LastMovingVehicle); vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))) && (vehiclepnt->iShipType))
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = vehiclepnt->WorldPosition.X - planepnt->WorldPosition.X;
			dy = vehiclepnt->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = vehiclepnt->WorldPosition.Z - planepnt->WorldPosition.Z;
			if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
			{
				pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
				if(planepnt->AI.lVar2 == WEAP_STORES_SMALL)
				{
					if(pshiptype->lSizeType & SHIP_SIZE_HUGE)
					{
						searchtypemod = visualrange;
					}
					else if(pshiptype->lSizeType & SHIP_SIZE_LARGE)
					{
						searchtypemod = (visualrange * 0.66f);
					}
					else if(pshiptype->lSizeType & SHIP_SIZE_MEDIUM)
					{
						searchtypemod = (visualrange * 0.5f);
					}
					else
					{
						searchtypemod = (visualrange * 0.33f);
					}
				}
				else if(planepnt->AI.lVar2 == WEAP_STORES_MED)
				{
					if(pshiptype->lSizeType & SHIP_SIZE_HUGE)
					{
						searchtypemod = visualrange * 0.9f;
					}
					else if(pshiptype->lSizeType & SHIP_SIZE_LARGE)
					{
						searchtypemod = (visualrange * 0.82f) * 0.9f;
					}
					else if(pshiptype->lSizeType & SHIP_SIZE_MEDIUM)
					{
						searchtypemod = (visualrange * 0.66f) * 0.9f;
					}
					else
					{
						searchtypemod = (visualrange * 0.5f) * 0.9f;
					}
				}
				else
				{
					searchtypemod = visualrange * 0.75f;
				}

				tdist = QuickDistance(dx, dz);
				if((tdist < foundrange) && (tdist < searchtypemod))
				{
					offangle = atan2(-dx, -dz) * 57.2958;
//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					toffangle = offangle - radarangleyaw;
					offangle = AICapAngle(offangle);

				    toffangle = AICapAngle(toffangle);
					if(fabs(toffangle) < widthangle)
					{
						widthok = 1;
					}

					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					heightok = 1;

					if(widthok && heightok)
					{
						foundobject = vehiclepnt;
						objectflag = MOVINGVEHICLE;
						foundrange = tdist;
					}
				}
			}
		}
	}

	if(foundobject)
	{
		planepnt->AI.pGroundTarget = foundobject;
		planepnt->AI.lGroundTargetFlag = objectflag;
	}
}

//**************************************************************************************
void AICheckFACTarget(PlaneParams *planepnt)
{
	BasicInstance *checkobject;
	MovingVehicleParams *vehiclepnt;

	if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
	{
		checkobject = (BasicInstance *)planepnt->AI.pGroundTarget;
		if(!InstanceIsBombable(checkobject))
		{
			planepnt->AI.pGroundTarget = NULL;
			planepnt->AI.lGroundTargetFlag = 0;
		}
	}
	else if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
		if(!((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))) && (!(vehiclepnt->Status & (VL_INVISIBLE)))))
		{
			planepnt->AI.pGroundTarget = NULL;
			planepnt->AI.lGroundTargetFlag = 0;
		}
	}
}

//**************************************************************************************
int AICheckFACSmoke(PlaneParams *planepnt)
{
	DBWeaponType *pweapon_type;
	int cnt;

	if(!(planepnt->AI.iAICombatFlags2 & AI_FAC))
	{
		return(1);
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(planepnt->WeapLoad[cnt].Count > 0)
		{
			pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
			if((pweapon_type->iWeaponType == WEAPON_TYPE_AGROCKET) && (pweapon_type->iWarHead == WARHEAD_SMOKE))
			{
				planepnt->AI.cActiveWeaponStation = cnt;
				planepnt->AI.cNumWeaponRelease = 1;
				//  eventually offset waypoint here.
				return(1);
			}
		}
	}
	return(0);
}

//**************************************************************************************
void AICheckIsEscorted(PlaneParams *planepnt)
{
	int patrol_end = 0;
	double distance;

	if(planepnt->AI.Behaviorfunc == AIMissileBreakToHeading)
	{
		patrol_end = 1;
	}

	if(planepnt->AI.AirTarget)
	{
		distance = (planepnt->AI.AirTarget->WorldPosition - planepnt->WorldPosition) * WUTONM;
		if(distance < (AI_ESCORT_RANGE_NM))
		{
			patrol_end = 1;
		}
	}

	if(planepnt->AI.AirThreat)
	{
		distance = (planepnt->AI.AirThreat->WorldPosition - planepnt->WorldPosition) * WUTONM;
		if(distance < (AI_ESCORT_RANGE_NM))
		{
			patrol_end = 1;
		}
	}

	if(patrol_end)
	{
		AIAfterEscortableWayPts(planepnt);
	}
}

//**************************************************************************************
void AIAfterEscortableWayPts(PlaneParams *planepnt)
{
	MBWayPoints *lookway, *lastlook;
	int numwaypts, lastnumwaypts;
	int startact, endact, actcnt;
	int gotoend = 0;
	int done = 0;
	int foundsweep;

	if(planepnt->AI.numwaypts > 1)
	{
		lookway = planepnt->AI.CurrWay;
		numwaypts = planepnt->AI.numwaypts;
	}
	else
	{
		return;
	}
	lastlook = planepnt->AI.CurrWay;
	lastnumwaypts = planepnt->AI.numwaypts;
	foundsweep = 1;

	while(foundsweep)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		foundsweep = 0;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_FIGHTER_SWEEP))
			{
				foundsweep = 1;
			}
		}
		if(!foundsweep)
		{
			planepnt->AI.CurrWay = lookway;
			planepnt->AI.numwaypts = numwaypts;
			break;
		}
		lastlook = lookway;
		lastnumwaypts = numwaypts;
		if(numwaypts > 1)
		{
			lookway ++;
			numwaypts --;
		}
		else
		{
			planepnt->AI.CurrWay = lookway;
			planepnt->AI.numwaypts = numwaypts;
			break;
		}
	}

	MBWayPoints	*waypnt = planepnt->AI.CurrWay;

	planepnt->AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
	planepnt->AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(waypnt->iSpeed)
	{
		planepnt->AI.lDesiredSpeed = waypnt->iSpeed;
	}

	if(planepnt->AI.lDesiredSpeed <= 100)
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_HELICOPTER)))
		{
			planepnt->AI.lDesiredSpeed = 350;
		}
		else
		{
			planepnt->AI.lDesiredSpeed = 50;
		}
	}

	if(waypnt->lWPy > 0)
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
	}
	else if(AIIsTakeOff(waypnt))
	{
		planepnt->AI.WayPosition.Y = ConvertWayLoc(8000);
	}
	else
	{
		if (!DoingSimSetup)
			planepnt->AI.WayPosition.Y = LandHeight(planepnt->AI.WayPosition.X, planepnt->AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		else
		{
			OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
			OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
			planepnt->AI.WayPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
		}

	}
}

//**************************************************************************************
int AIConvertVGroupToVNum(int vgroup)
{
	int cnt;
	int vnum = 0;

	if(vgroup >= g_iNumMovingVehicle)
		return(0);

	for(cnt = 0; cnt < vgroup; cnt ++)
	{
		vnum += g_pMovingVehicleList[cnt].iNumGroup;
	}

	if(vnum > (LastMovingVehicle - MovingVehicles))
		return(0);

	return(vnum);
}

//**************************************************************************************
#if 0  //  STUFF FOR HUD
//**************************************************************************************
void AIFlyNumbNuts(PlaneParams *planepnt)
{
	planepnt->DesiredRoll = 0;
	if(planepnt->OnGround)
	{
		planepnt->DesiredPitch = 0x2000;
	}
	else
	{
		planepnt->DesiredPitch = 0;
	}
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
}

	else
	{
		long tempangle;

		tempangle = (long)P->AI.DesiredHeading - 180;
		sprintf(TmpStr,"T %02.02f, SB %02.02f, DH %ld",P->RightThrustPercent, P->SpeedBrake, tempangle);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}

	else
	{
		sprintf(TmpStr,"ET %02.02f, PT %02.02f",P->ElevatorTrim, P->AI.PitchTrim);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}

	else
	{
		double degpitch, degdpitch;

		degpitch = AIConvertAngleTo180Degree(P->Pitch);


		degdpitch = AIConvertAngleTo180Degree(P->AI.DesiredPitch);

		sprintf(TmpStr,"ET %02.02f, PT %02.02f, DP %02.02f, OP %02.02f",P->ElevatorTrim, P->AI.PitchTrim, (degpitch - lastpitch), (degdpitch - degpitch));
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}
	else
	{
		sprintf(TmpStr,"ET %02.02f, PT %02.02f, MX %02.02f, MN %02.02f",P->ElevatorTrim, P->AI.PitchTrim, P->AI.fGLimitMaxElev, P->AI.fGLimitMinElev);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}
	else
	{
		sprintf(TmpStr,"ET %02.02f, PT %02.02f, MX %02.02f, MN %02.02f, EV %02.02f",P->ElevatorTrim, P->AI.PitchTrim, P->AI.fGLimitMaxElev, P->AI.fGLimitMinElev, P->SymetricalElevator);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}
	else
	{
		sprintf(TmpStr,"T %02.02f, SB %02.02f",P->RightThrustPercent, P->SpeedBrake);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}

	else
	{
		long tempangle;

		tempangle = (long)P->AI.DesiredHeading - 180;
		if(P->AI.Behaviorfunc == AIHeadAtAirTarget)
			sprintf(TmpStr,"XXX, T %02.02f, SB %02.02f, DH %ld, DR %02.02f",P->RightThrustPercent, P->SpeedBrake, tempangle, P->AI.WayPosition.X);
		else if(P->AI.Behaviorfunc == AIDoJink)
			sprintf(TmpStr,"OOO, T %02.02f, SB %02.02f, DH %ld, DR %02.02f",P->RightThrustPercent, P->SpeedBrake, tempangle, P->AI.WayPosition.X);
		else
			sprintf(TmpStr,"T %02.02f, SB %02.02f, DH %ld, DR %02.02",P->RightThrustPercent, P->SpeedBrake, tempangle, P->AI.WayPosition.X);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}
	else
	{
		double tempangle;

		tempangle = AIConvertAngleTo180Degree(P->AI.DesiredHeading);
		if(P->AI.cnt1 == 1)
			sprintf(TmpStr,"DH %02.02f, PT %02.02f, XXXXXXX", tempangle, P->AI.PitchTrim);
		else if(P->AI.cnt1 == 2)
			sprintf(TmpStr,"DH %02.02f, PT %02.02f, OOOO", tempangle, P->AI.PitchTrim);
		else if(P->AI.cnt1 == 3)
			sprintf(TmpStr,"DH %02.02f, PT %02.02f, IIIIII", tempangle, P->AI.PitchTrim);
		else if(P->AI.cnt1 == 4)
			sprintf(TmpStr,"DH %02.02f, PT %02.02f, ++++", tempangle, P->AI.PitchTrim);
		else
			sprintf(TmpStr,"DH %02.02f, PT %02.02f", tempangle, P->AI.PitchTrim);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}


		int cnt;

		for(cnt = 219; cnt <= 226; cnt ++)
		{
			GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
			GrDrawLine(GrBuffFor3D, 205, cnt, 436, cnt, 0);
		}

	int cnt;

	for(cnt = 117; cnt <= 127; cnt ++)
	{
		GrDrawLine(GrBuffFor3D, 412, cnt, 436, cnt, 0);
	}

	else
	{
		double tempangle;
		char tstr[180];
		int cnt;

		for(cnt = 218; cnt <= 228; cnt ++)
		{
			GrDrawLine(GrBuffFor3D, 205, cnt, 436, cnt, 0);
		}

		tempangle = AIConvertAngleTo180Degree(P->AI.DesiredHeading);

		sprintf(TmpStr,"DH %02.02f, DY %d, PT %02.02f", tempangle, P->AI.cnt1, P->AI.PitchTrim);


		if(P->AI.cnt1 == 1)
			strcat(TmpStr, ", XXXXXXX");
		else if(P->AI.cnt1 == 2)
			strcat(TmpStr, ",   00000");
		else if(P->AI.cnt1 == 3)
			strcat(TmpStr, ", IIIIIII");
		else if(P->AI.cnt1 == 4)
			strcat(TmpStr, ", +++++++");
		else if(P->AI.cnt1 == 5)
			strcat(TmpStr, ", XXXXxxx");
		else if(P->AI.cnt1 == 6)
			strcat(TmpStr, ",   OOOooo");
		else if(P->AI.cnt1 == 7)
			strcat(TmpStr, ", IIIiii");
		else if(P->AI.cnt1 == 8)
			strcat(TmpStr, ", ++++XOIX");
		else if(P->AI.cnt1 == 9)
			strcat(TmpStr, ", RRRRRR");
		else if(P->AI.cnt1 == 10)
			strcat(TmpStr, ", SSSSSS");
		else if(P->AI.cnt1 == 11)
			strcat(TmpStr, ", TTTTTTT");
		else if(P->AI.cnt1 == 12)
			strcat(TmpStr, ", UUUUUUUU");
		else if(P->AI.cnt1 == 13)
			strcat(TmpStr, ", WWWWWWWW");
		else if(P->AI.cnt1 == 14)
			strcat(TmpStr, ", ZZZzzzZZZ");
		else if(P->AI.cnt1 == 20)
			strcat(TmpStr, ", -A-B-C-S");
		else if(P->AI.cnt1 == 21)
			strcat(TmpStr, ", -----------");
		else if(P->AI.cnt1 == 25)
			strcat(TmpStr, ",                       ");
		else if(P->AI.cnt1 == 26)
			strcat(TmpStr, ",           26         ");

//		sprintf(tstr, ", DR %02.02f", AIConvertAngleTo180Degree(P->AI.DesiredRoll));
		sprintf(tstr, ", DR %02.02f, DP %02.02f, ET %02.02f, SE %02.02f", AIConvertAngleTo180Degree(P->AI.DesiredRoll), AIConvertAngleTo180Degree(P->AI.DesiredPitch), P->ElevatorTrim, P->SymetricalElevator);
		strcat(TmpStr, tstr);
		GrDrawString(GrBuffFor3D, MessageFont, 210,220, TmpStr, HUDColor);
	}


			int ispaused = 0;
			if((GetAsyncKeyState('Z')) && (P == (PlaneParams *)Camera1.AttachedObject))
			{
				ispaused = MBDoDebugBreak();
			}
			if((GetAsyncKeyState('K')) && (P == (PlaneParams *)Camera1.AttachedObject))
			{
//					ispaused = MBDoDebugBreak();
				AISwitchLeader(P);
			}

			if(ispaused)
			{
				UnPauseFromDiskHit();
			}

//  Draw a circle around a waypoint
	float ccentx, ccentz;
	float lx, lz, cx, cz;

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx, dz);

	offangle = atan2(-dx, -dz) * 57.2958;
	toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
	offangle = AICapAngle(offangle);

	toffangle = AICapAngle(toffangle);

	heading = DegToRad((double)toffangle);
	angzx = sin(heading);
	angzz = cos(heading);

//					ftempx = ((angzz * dx) + (angzx * dz)) * WUTOFT;
//					ftempz = ((angzx * dx) + (angzz * dz)) * WUTOFT;
	ftempx = (angzx * tdist) * WUTOFT;
	ftempz = (angzz * tdist) * WUTOFT;
	ccentx = 320 - (ftempx / fpixsize);
	ccentz = 240 - (ftempz / fpixsize);

	tdist = planepnt->AI.lVar2 * FTTOWU;

	heading = 0;
	angzx = sin(heading);
	angzz = cos(heading);

//					ftempx = ((angzz * dx) + (angzx * dz)) * WUTOFT;
//					ftempz = ((angzx * dx) + (angzz * dz)) * WUTOFT;
	ftempx = (angzx * tdist) * WUTOFT;
	ftempz = (angzz * tdist) * WUTOFT;
	lx = ccentx - (ftempx / fpixsize);
	lz = ccentz - (ftempz / fpixsize);

//	for(heading = 1; heading < 361; heading += 1.0)
	for(heading = 1; heading < 361; heading += 1.0)
	{
		angzx = sin(DegToRad(heading));
		angzz = cos(DegToRad(heading));

	//					ftempx = ((angzz * dx) + (angzx * dz)) * WUTOFT;
	//					ftempz = ((angzx * dx) + (angzz * dz)) * WUTOFT;
		ftempx = (angzx * tdist) * WUTOFT;
		ftempz = (angzz * tdist) * WUTOFT;
		cx = ccentx - (ftempx / fpixsize);
		cz = ccentz - (ftempz / fpixsize);
		GrDrawLineClipped(GrBuffFor3D, cx, cz, lx, lz, graycolor, 0, 0);

		lx = cx;
		lz = cz;
	}
//  End Draw Circle

//  Draw steering point for orbit
	dx = TempPoint.X - planepnt->WorldPosition.X;
	dy = TempPoint.Y - planepnt->WorldPosition.Y;
	dz = TempPoint.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx, dz);

	offangle = atan2(-dx, -dz) * 57.2958;
	toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
	offangle = AICapAngle(offangle);

	toffangle = AICapAngle(toffangle);

	heading = DegToRad((double)toffangle);
	angzx = sin(heading);
	angzz = cos(heading);

//					ftempx = ((angzz * dx) + (angzx * dz)) * WUTOFT;
//					ftempz = ((angzx * dx) + (angzz * dz)) * WUTOFT;
	ftempx = (angzx * tdist) * WUTOFT;
	ftempz = (angzz * tdist) * WUTOFT;
	fdrawx = 320 - (ftempx / fpixsize);
	fdrawz = 240 - (ftempz / fpixsize);
	usecolor = yellowcolor;
	GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz - fboxsize, usecolor, 0, 0);
	GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz + fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, usecolor, 0, 0);
	GrDrawLineClipped(GrBuffFor3D, fdrawx - fboxsize, fdrawz - fboxsize, fdrawx - fboxsize, fdrawz + fboxsize, usecolor, 0, 0);
	GrDrawLineClipped(GrBuffFor3D, fdrawx + fboxsize, fdrawz - fboxsize, fdrawx + fboxsize, fdrawz + fboxsize, usecolor, 0, 0);
//  End of draw steering point


	int ispaused = 0;

	if(GetAsyncKeyState('H'))
	{
		PauseForDiskHit();
		if(lpDD)
		  lpDD->FlipToGDISurface();
		DebugBreak();
		ispaused = 1;
	}

	if(ispaused)
	{
		UnPauseFromDiskHit();
	}


	if(lvcolor)
	{
		for(cnt = 0; cnt < 10; cnt ++)
		{
			GrDrawLineClipped(GrBuffFor3D, 315, cnt, 325, cnt, lvcolor, 0, 0);
		}
		lvcolor = 0;
	}

#endif