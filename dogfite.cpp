//*****************************************************************************************
//  DOGFITE.CPP
//*****************************************************************************************

#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"

extern int lvcolor;
extern char		g_szExePath[260];
extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)

//#define BEHAVETEMP 1
//			lpDD->FlipToGDISurface();
//#define SREXYZZY 1

#define BOMBTRAILDIST 180960  //  180960 /*30nm*/ //  60320 /*10nm*/  //  90480  /*15nm*/
#define ATTACKALTSPACING 50

//#define SREDEBUGFILE
#ifdef SREDEBUGFILE
int DebugFileSRE;
int SREFILEOpen = 0;
#endif

extern int		TimeExcel;
extern int CockpitView;// = COCKPIT_FRONT_SEAT;
extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
extern DBRadarType *GetRadarPtr(long id);
extern DetectedPlaneListType CurFramePlanes;
extern DBWeaponType *get_weapon_ptr(long id);
// extern RadarInfoType RadarInfo;
// extern CurSelectedTargetInfoType CurTargetInfo;
extern DetectedPlaneListType PrevFramePlanes;
extern float GetCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);
extern int GetCalculatedCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT);


extern BOOL DoingSimSetup;
extern BYTE WarHeadTypeConversion[];
void StartWingConTrails(PlaneParams *P);
void StopWingConTrails(PlaneParams *P);


//**************************************************************************************
void AISimpleRadar(PlaneParams *planepnt)
{
  	PlaneParams *checkplane = &Planes[0];
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	PlaneParams *foundplane = NULL;
	float nlfoundrange;
	PlaneParams *nlfoundplane = NULL;
	float offangle, toffangle, toffpitch, offpitch;
	float targbearing, nltargbearing;
	float targpitch, nltargpitch;
	int widthok, heightok, visualok;
	PlaneParams *leadplane;
	PlaneParams *orgtarget;
	int ifoundplane;
	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
	int stationnum;
	int noupdates = 1;
	PlaneParams *checkorgairthreat = NULL;
	PlaneParams *tempairthreat = NULL;
	int radarid;
	DBRadarType *radar;
	int inradar = 0;
	int nlinradar = 0;
	int tempval, tempfence;
	int side;
	float jammingmod;
	float skilltime;
	float ftempval;
	int isnight = 0;
	int hours;
	int inburner;
	int nightburner;
	int justkeepcheck = 0;
	FPointDouble targetpos;
	int isawacs;
	int crosssig, los;

	if(planepnt == PlayerPlane)
	{
		return;
	}

	if((planepnt->OnGround) || (planepnt->AI.Behaviorfunc == AIFlyTakeOff) || (planepnt->AI.iAIFlags2 & AILANDING) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		planepnt->AI.iAIFlags2 &= ~AI_RADAR_PING;	// *** init: reset the PING bit ***
		return;
	}

	if(planepnt->AI.iAICombatFlags2 & AI_DRONE)
		return;


	if((planepnt->AI.iAIFlags2 & (AI_CAS_ATTACK|AI_SEAD_ATTACK|AI_SEAD_COVER)) || (planepnt->AI.iAICombatFlags2 & AI_ESCORT_CAS))  //  (AI_CAS_ATTACK|AI_SEAD_ATTACK|AI_SEAD_COVER)
	{
		AISimpleGroundRadar(planepnt);
		return;
	}

	if((planepnt->AI.iAIFlags2 & AILOCKEDUP) || (planepnt->AI.iAICombatFlags2 & (AI_EARLY_LOCK)))
	{
		AICheckForGateStealerP(planepnt, planepnt->AI.AirTarget);
	}
	planepnt->AI.lRadarDelay -= DeltaTicks;
	if(planepnt->AI.lRadarDelay > 0)
	{
		return;
	}

	leadplane = AIGetLeader(planepnt);

	orgtarget = planepnt->AI.AirTarget;

	if(orgtarget == NULL)
	{
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	}

	isawacs = pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3;

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		isawacs = 0;
	}

	radarid = pDBAircraftList[planepnt->AI.iPlaneIndex].lRadarID;
	radar = GetRadarPtr(radarid);

	if(planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1)
	{
		radar = NULL;
	}
	if(radar)
	{
		if((radar->lRadarID == 38) || (radar->lRadarID == 39))
		{
			radar = NULL;
		}
	}

	hours = WorldParams.WorldTime/3600;

	if((hours < 6) || (hours > 18))
	{
		isnight = 1;
	}

	visualrangenm = AI_VISUAL_RANGE_NM;
	if(isnight)
	{
		visualrangenm /= 4;
	}

	visualrange = (visualrangenm * NMTOWU);
	visualrange *= 1.0f - (planepnt->AI.iSkill * 0.1);

	if(radar)
	{
		ftempval = radar->iSearchAzimuth;
		ftempval *= 1.0f - (planepnt->AI.iSkill * 0.1f);
		widthangle = ftempval;

		ftempval = radar->iSearchElevation;
		ftempval *= 1.0f - (planepnt->AI.iSkill * 0.1f);
		heightangle = ftempval;
	}
	else
	{
		ftempval = 20.0f * (1.0f - (planepnt->AI.iSkill * 0.1f));
		widthangle = ftempval;
		ftempval = 20.0f * (1.0f - (planepnt->AI.iSkill * 0.1f));
		heightangle = ftempval;
	}

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

	jammingmod = AIGetPlaneJammingMod(planepnt, radaranglepitch, radarangleyaw, widthangle, heightangle);

	if(jammingmod < 1.0f)
	{
		dx = 1;
		dy = 1;
	}

	if(radar)
	{
		rangenm = radar->iMaxRange * jammingmod;
		if(rangenm < visualrangenm)
		{
			rangenm = visualrangenm;
		}
		radarrange = (rangenm * NMTOWU);
		radarrange *= 1.0f - (planepnt->AI.iSkill * 0.1);
	}
	else
	{
		rangenm = visualrangenm;
		radarrange = visualrange;
	}

	foundrange = radarrange;
	foundplane = NULL;
	nlfoundrange = radarrange;
	nlfoundplane = NULL;
	PlaneParams *endplane;

	if(isnight)
	{
		if(foundrange <= visualrange)
		{
			nlfoundrange = foundrange = visualrange * 8;
		}
	}

	if(!(planepnt->AI.iAIFlags1 & AIRADARON))
	{
		radarrange = visualrange;

		if(isnight)
		{
			nlfoundrange = foundrange = visualrange;
		}
		else
		{
			nlfoundrange = foundrange = visualrange * 8;
		}
	}

	planepnt->AI.iAIFlags2 &= ~AI_RADAR_PING;

	endplane = LastPlane;
	if(planepnt->AI.iAIFlags2 & AILOCKEDUP)
	{
		if(planepnt->AI.AirTarget == NULL)
		{
			planepnt->AI.iAIFlags2 &= ~AILOCKEDUP;
		}
		else
		{
			if(!AISeeIfStillMissiled(planepnt, planepnt->AI.AirTarget, 1))
			{
				planepnt->AI.iAIFlags2 &= ~AILOCKEDUP;
			}
			else
			{
				if(AICheckForGateStealerP(planepnt, planepnt->AI.AirTarget))
				{
					return;
				}
				checkplane = planepnt->AI.AirTarget;
				endplane = checkplane;
			}
		}
	}
	else if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		justkeepcheck = 1;
	}

	if(!(planepnt->AI.iAIFlags2 & AILOCKEDUP))
		planepnt->AI.lGateTimer = -99999;

	if((planepnt->AI.iAIFlags2 & AIKEEPTARGET) && (planepnt->AI.winglead == (PlayerPlane - Planes)))
	{
		justkeepcheck = 1;
	}

	while (checkplane <= LastPlane)
	{
		if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (checkplane->OnGround == 0)
					&& ((planepnt->AI.iAIFlags1 & (AIENGAGED|AICANENGAGE)) || (!((checkplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2)))) && ((!(planepnt->AI.iAIFlags1 & (AIENGAGED))) || (checkplane->AI.iSide != AI_NEUTRAL) || (iAI_ROE[AI_NEUTRAL] >= 2)))
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;

			nightburner = inburner = 0;

			//  At night visibility is reduce to 1/4 unless plane in burner, then visibility is 2 time normal day (or 8 times night)

			if(isnight)
			{
				if((checkplane->RightThrustPercent > 0.75f) || (checkplane->LeftThrustPercent > 0.75f))
				{
					inburner = 1;
				}
				if(inburner && ((!radar) || (!(planepnt->AI.iAIFlags1 & AIRADARON))))
				{
					if((fabs(dx) < (radarrange * 8)) && (fabs(dz) < (radarrange * 8)))
					{
						nightburner = 1;
					}
				}
			}

			if(checkplane->Status & PL_DEVICE_DRIVEN)
			{
				AICheckHumanThreats(planepnt, checkplane, dx, dy, dz, (visualrange * (1 + (inburner * 7))));
			}

//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			if(((fabs(dx) < radarrange) && (fabs(dz) < radarrange)) || (nightburner))
			{
				tdist = QuickDistance(dx, dz);
//				if(tdist < nlfoundrange)
				if((tdist < radarrange) && (!((checkplane->AI.lPlaneID == 85) && (tdist < (visualrange * 0.5f)))))
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
					if(fabs(toffpitch) < heightangle)  //  Plane in radar area
					{
						heightok = 1;
					}

					if((tdist < visualrange) || ((inburner) && (tdist < (visualrange * 8))))
					{
						//  May want to do pitch and heading check for this
						visualok = 1;
					}

					if((widthok && heightok) || (visualok) || (isawacs))
					{
						tempairthreat = checkplane->AI.AirThreat;

						if(radar)
						{
							AIUpdateRadarThreat(planepnt, checkplane, ((widthok && heightok) || isawacs));
						}

						targetpos = checkplane->WorldPosition;

						//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
						//  If was previous target, keep lock regardless of chance.

						if(tdist < (2.0f * NMTOWU))
						{
							targetpos.Y += 500 * FTTOWU;
						}

						los = AILOS(planepnt->WorldPosition, targetpos);
						crosssig = AICheckRadarCrossSig(planepnt, checkplane, dy * WUTOFT, tdist * WUTONM, radar, (planepnt->AI.iAIFlags2 & AILOCKEDUP));

						if((los) && (crosssig))
						{
							if((planepnt->AI.iAIFlags2 & AI_CHECK_MISSILES) && (los) && (crosssig))
							{
								if(AISeeIfStillMissiled(planepnt, checkplane, 1))
								{
									noupdates = 0;
								}
							}
						}
						else
						{
							planepnt->AI.iAIFlags2 &= ~AILOCKEDUP;
						}

						if(((visualok) || (crosssig)) && (los))
						{
							if(isawacs)
							{
								checkplane->AI.iAICombatFlags2 |= AI_AWACS_DETECT;
								checkplane->AI.lTimerAWACS = (AI_AWACS_SWEEP_DELAY * 1000) + 5000;
							}
						}

						if((!lockedorg) && (tdist < nlfoundrange) && (los))
						{
							if((visualok) || (crosssig))
							{
								if(isawacs)
								{
									checkplane->AI.iAICombatFlags2 |= AI_AWACS_DETECT;
									checkplane->AI.lTimerAWACS = (AI_AWACS_SWEEP_DELAY * 1000) + 5000;
								}

								if(!justkeepcheck)
								{
									if(!AICheckIfAlreadyTarget(planepnt, AIGetLeader(planepnt), checkplane))
									{
										foundplane = checkplane;
										foundrange = tdist;
										nlfoundplane = checkplane;
										nlfoundrange = tdist;
										targbearing = nltargbearing = offangle;
										targpitch = nltargpitch = offpitch;
										checkorgairthreat = tempairthreat;
										if(widthok && heightok)
										{
											inradar = 1;
											nlinradar = 1;
										}
									}
									else
									{
										foundplane = checkplane;
										foundrange = tdist;
										targbearing = offangle;
										targpitch = offpitch;
										if(widthok && heightok)
										{
											inradar = 1;
										}
									}
								}
								else if(checkplane == planepnt->AI.AirTarget)
								{
									foundplane = checkplane;
									targbearing = offangle;
									targpitch = offpitch;
									if(widthok && heightok)
									{
										inradar = 1;
									}
								}

								if((orgtarget == checkplane) && (planepnt->AI.iAIFlags2 & AIKEEPTARGET))
								{
									foundplane = checkplane;
									foundrange = tdist;
									targbearing = offangle;
									targpitch = offpitch;
									lockedorg = 1;
									if(widthok && heightok)
									{
										inradar = 1;
									}
								}
							}
						}
					}
				}
			}
		}
		checkplane ++;
	}

	if(noupdates)
	{
		planepnt->AI.iAIFlags2 &= ~AI_CHECK_MISSILES;
	}

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(foundplane == NULL)
		{
			GeneralSetNewAirTarget(planepnt, NULL);

			planepnt->AI.iAIFlags2 &= ~AILOCKEDUP;
			planepnt->AI.iAICombatFlags2 &= ~(AI_EARLY_LOCK);
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				GeneralClearCombatBehavior(planepnt);
			}

			if(planepnt->AI.iAIFlags1 & AIENGAGED)
			{
				planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
				skilltime = frand() * (planepnt->AI.iSkill * 1000);
				planepnt->AI.lRadarDelay += skilltime;
			}
			else
			{
				planepnt->AI.lRadarDelay = 10000;  //  10 seconds until next sweep
				skilltime = frand() * (planepnt->AI.iSkill * 3000);
				planepnt->AI.lRadarDelay += skilltime;
			}
			planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
			planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
			return;
		}

		if(nlfoundplane != NULL)
		{
			inradar = nlinradar;
			if((checkorgairthreat == NULL) && (radar) && (inradar))
			{
				GDCheckForPingReport(NULL, nlfoundplane, planepnt);

				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirTarget != nlfoundplane))
				{
					NetPutGenericMessage2(planepnt, GM2_NEW_TARGET_PING, (BYTE)(nlfoundplane - Planes));
				}
			}
			else
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirTarget != nlfoundplane))
				{
					NetPutGenericMessage2(planepnt, GM2_NEW_TARGET, (BYTE)(nlfoundplane - Planes));
				}
			}

			if((planepnt->AI.AirTarget != nlfoundplane) && (!((foundplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2)))
					&& ((planepnt->AI.iAIFlags1 & (AIENGAGED)) || (planepnt->AI.iSide != AI_NEUTRAL)))
			{
				planepnt->AI.lLockTimer = 3000 + ((rand() & 1) * 1000);
				skilltime = frand() * (planepnt->AI.iSkill * 2000);
				planepnt->AI.lLockTimer += skilltime;
				planepnt->AI.iAICombatFlags2 &= ~(AI_EARLY_LOCK);
				AICanFireMissile(planepnt, nlfoundplane, nlfoundrange);
			}
			if((planepnt->AI.AirTarget != nlfoundplane) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				GeneralClearCombatBehavior(planepnt);
			}

			planepnt->AI.AirTarget = nlfoundplane;
			targbearing = nltargbearing;
			targpitch = nltargpitch;
			foundrange = nlfoundrange;
			foundplane = nlfoundplane;
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirTarget != foundplane))
			{
				NetPutGenericMessage2(planepnt, GM2_NEW_TARGET, (BYTE)(foundplane - Planes));
			}
			checkorgairthreat = planepnt;
			if(planepnt->AI.AirTarget != foundplane)
			{
				planepnt->AI.lLockTimer = 3000 + ((rand() & 1) * 1000);
				skilltime = frand() * (planepnt->AI.iSkill * 2000);
				planepnt->AI.lLockTimer += skilltime;
				planepnt->AI.iAICombatFlags2 &= ~(AI_EARLY_LOCK);
				AICanFireMissile(planepnt, foundplane, foundrange);
			}
			if((planepnt->AI.AirTarget != foundplane) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				GeneralClearCombatBehavior(planepnt);
			}

			planepnt->AI.AirTarget = foundplane;
		}

		if((!(planepnt->AI.iAIFlags2 & AILOCKEDUP)) && (planepnt->AI.lLockTimer < 0) && (planepnt->AI.iAIFlags1 & (AIENGAGED|AICANENGAGE)) && (!((foundplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2))) && (!((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[planepnt->AI.iSide])))
					&& ((planepnt->AI.iAIFlags1 & (AIENGAGED)) || (planepnt->AI.iSide != AI_NEUTRAL)))
		{
			tempval = 0;
			side = planepnt->AI.iSide;

			if(foundplane->AI.iSide == side)
			{
				tempval = 1;
			}
			else if((!(iMultiSides & (MS_AGGRESIVE_NEUTRALS))) && (((foundplane->AI.iSide == AI_ENEMY) || (foundplane->AI.iSide == AI_NEUTRAL)) && ((side == AI_ENEMY) || (side == AI_NEUTRAL))))
			{
				tempval = 1;
			}
			else if((iAI_ROE[side] < 2) && (!(iMultiSides & (MS_IGNORE_FENCES))))
			{
				tempfence = AICCheckAllFences((foundplane->WorldPosition.X * WUTOFT), (foundplane->WorldPosition.Z * WUTOFT));

				if((!tempfence) && ((foundplane->AI.iSide == AI_FRIENDLY) || (foundplane->AI.iSide == AI_NEUTRAL)) && (side == AI_ENEMY))
				{
					tempval = 1;
				}
				else if((tempfence) && (foundplane->AI.iSide == AI_ENEMY))
				{
					tempval = 1;
				}

			}

			if((!tempval) && (inradar))
			{
				stationnum = AICanFireMissile(planepnt, planepnt->AI.AirTarget, foundrange);
				if(stationnum != -1)
				{
					AIFireMissile(planepnt, planepnt->AI.AirTarget, stationnum);
				}
				else
				{
					planepnt->AI.lLockTimer = 3000 + ((rand() & 1) * 1000);
					skilltime = frand() * (planepnt->AI.iSkill * 2000);
					planepnt->AI.lLockTimer += skilltime;
				}
			}
			else
			{
		 		planepnt->AI.lLockTimer = 15000 + ((rand() & 1) * 1000);
				skilltime = frand() * (planepnt->AI.iSkill * 2000);
				planepnt->AI.lLockTimer += skilltime;
			}
		}
	}
	else if(foundplane == NULL)
	{
		if(foundplane == NULL)
		{
			planepnt->AI.iAIFlags2 &= ~AILOCKEDUP;
			planepnt->AI.AirTarget = NULL;
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				GeneralClearCombatBehavior(planepnt);
			}

			if(planepnt->AI.iAIFlags1 & AIENGAGED)
			{
				planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep
				skilltime = frand() * (planepnt->AI.iSkill * 1000);
				planepnt->AI.lRadarDelay += skilltime;
			}
			else
			{
				planepnt->AI.lRadarDelay = 10000;  //  10 seconds until next sweep
				skilltime = frand() * (planepnt->AI.iSkill * 3000);
				planepnt->AI.lRadarDelay += skilltime;
			}
			planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
			planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
			if(isawacs)
			{
				planepnt->AI.lRadarDelay = (AI_AWACS_SWEEP_DELAY * 1000);
			}
			return;
		}
	}

	if(inradar)
	{
		planepnt->AI.fRadarAngleYaw = targbearing;
		planepnt->AI.fRadarAnglePitch = targpitch;
	}
	else
	{
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	}

	if(planepnt->AI.iAICombatFlags2 & AI_ESCORT_STAY)
	{
//		if((!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA)) && (iAI_ROE[planepnt->AI.iSide] == 2) && ((((foundrange < ((AI_VISUAL_RANGE_NM * NMTOWU) * 4.0f)) && (g_Settings.gp.nType == GP_TYPE_CASUAL)) || (foundrange < ((AI_VISUAL_RANGE_NM * NMTOWU) * 4.0f)))) && (planepnt->AI.Behaviorfunc == AIFlyEscort))
		if((!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA)) && (iAI_ROE[planepnt->AI.iSide] == 2) && (((foundrange < ((AI_VISUAL_RANGE_NM * NMTOWU) * 4.0f)))) && (planepnt->AI.Behaviorfunc == AIFlyEscort) && ((foundplane->AI.iSide != AI_NEUTRAL) || (iAI_ROE[AI_NEUTRAL] >= 2)))
		{
//			if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			if((!planepnt->AI.LinkedPlane) || (planepnt->AI.LinkedPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			{
				planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				leadplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				AIUpdateGroupFlags(leadplane - Planes, AI_ASK_PLAYER_ENGAGE, 0, 0);
			}
			else if(!(planepnt->AI.iAIFlags1 & AI_ASK_PLAYER_ENGAGE))
			{
				AICWingReadyToEngageMsg(planepnt - Planes);
				AIUpdateGroupFlags(leadplane - Planes, AI_ASK_PLAYER_ENGAGE, 0, 0);
#if 0
				if(g_Settings.gp.nType == GP_TYPE_CASUAL)
				{
					iEasyAAVar = 1;
					lEasyAATimer = 15000;
				}
#endif
			}
			else if(iEasyAAVar == 2)
			{
				planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				leadplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
				AIUpdateGroupFlags(leadplane - Planes, AI_ASK_PLAYER_ENGAGE, 0, 0);
			}
		}
	}

	if((orgtarget == NULL) && ((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)) && (!((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[planepnt->AI.iSide]))))
	{
		ifoundplane = -1;

		if(!AICheckIfAnyTargets(AIGetLeader(planepnt), AIGetLeader(planepnt->AI.AirTarget), planepnt, &ifoundplane))
		{
			leadplane = AIGetLeader(planepnt);
			if((PlayerPlane == leadplane) && AIInPlayerGroup(planepnt))
			{
				if((planepnt->AI.AirTarget->RightThrustPercent > 0.75f) || (planepnt->AI.AirTarget->LeftThrustPercent > 0.75f))
				{
					inburner = 1;
				}
				if((foundrange > visualrange) && (!((isnight) && (inburner) && (foundrange <= (visualrange * 8)))))
				{
					AICRadarBanditCall(planepnt, planepnt->AI.AirTarget, targbearing, foundrange);
				}
				else
				{
					if(lVisualOverloadTimer < 0)
					{
						AICVisualBanditCall(planepnt, planepnt->AI.AirTarget, targbearing, foundrange);
						lVisualOverloadTimer = 30000;
					}
				}
			}
			else if(PlayerPlane->AI.iSide == planepnt->AI.iSide)
			{
				AICCheckForBanditCalls(planepnt, planepnt->AI.AirTarget, targbearing, foundrange);
			}
		}

		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			AIVectorIntercepts(planepnt->AI.AirTarget, planepnt->AI.iSide);
		}
		//  See if anyone else in group has lock.
		//  If not then
		//  If in Player's group, notify Player and then perhaps general AI.
		//  else notify general AI.
	}

	if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
//		if((!((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[planepnt->AI.iSide]))) && (!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA)) && (iAI_ROE[planepnt->AI.iSide] == 2) && ((((foundrange < ((AI_VISUAL_RANGE_NM * NMTOWU) * 2.0f)) && (g_Settings.gp.nType == GP_TYPE_CASUAL)) || (foundrange < ((AI_VISUAL_RANGE_NM * NMTOWU) * 2.0f))) && (planepnt->AI.Behaviorfunc == AIFlyFormation)))
		if((!((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[planepnt->AI.iSide]))) && (!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA)) && (iAI_ROE[planepnt->AI.iSide] == 2) && (((foundrange < ((AI_VISUAL_RANGE_NM * NMTOWU) * 2.0f))) && (planepnt->AI.Behaviorfunc == AIFlyFormation)) && ((foundplane->AI.iSide != AI_NEUTRAL) || (iAI_ROE[AI_NEUTRAL] >= 2)))
		{
			if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				AICFEngageBandits();
				AIUpdateGroupFlags(PlayerPlane - Planes, AI_ASK_PLAYER_ENGAGE, 0, 0);
			}
			else if(!(planepnt->AI.iAIFlags1 & AI_ASK_PLAYER_ENGAGE))
			{
				AICWingReadyToEngageMsg(planepnt - Planes);
				AIUpdateGroupFlags(PlayerPlane - Planes, AI_ASK_PLAYER_ENGAGE, 0, 0);
#if 0
				if(g_Settings.gp.nType == GP_TYPE_CASUAL)
				{
					iEasyAAVar = 1;
					lEasyAATimer = 15000;
				}
#endif
			}
			else if(iEasyAAVar == 2)
			{
				AICFEngageBandits();
				AIUpdateGroupFlags(PlayerPlane - Planes, AI_ASK_PLAYER_ENGAGE, 0, 0);
			}
		}
	}

	if((radar) && (inradar))
	{
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
	}

	planepnt->AI.lRadarDelay = 3000;  //  3 seconds until next sweep

	if(isawacs)
	{
		planepnt->AI.lRadarDelay = (AI_AWACS_SWEEP_DELAY * 1000);
	}
}

//**************************************************************************************
void AIVectorIntercepts(PlaneParams *target, int side, float interceptnm)
{
	PlaneParams *checkplane;
	float dx, dy, dz;
	float foundrange;
	PlaneParams *foundplane = NULL;
	float interceptrange;
	float tdist;
	int tempfence = 0;
	int SideOK[3];
	int needcheck = 0;
	int cnt;
	int playergroupcover;
	int playergroupcap;
	PlaneParams *leadplane;
	int ai_can_engage = 1;

	if(0 <= target->AI.lInterceptTimer)
	{
		return;
	}

	if((target->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[side]))
	{
		ai_can_engage = 0;
	}

	tempfence = AICCheckAllFences((target->WorldPosition.X * WUTOFT), (target->WorldPosition.Z * WUTOFT));

	cnt = side;

	if(cnt == target->AI.iSide)
	{
		SideOK[cnt] = 0;
	}
	else if((!(iMultiSides & (MS_AGGRESIVE_NEUTRALS))) && (((cnt == AI_ENEMY) || (cnt == AI_NEUTRAL)) && ((target->AI.iSide == AI_ENEMY) || (target->AI.iSide == AI_NEUTRAL))))
	{
		SideOK[cnt] = 0;
	}
	else if((!(iMultiSides & (MS_AGGRESIVE_NEUTRALS))) && ((target->AI.iSide == AI_NEUTRAL) && (iAI_ROE[target->AI.iSide] < 2)))
	{
		SideOK[cnt] = 0;
	}
	else if((iAI_ROE[cnt] < 2) && (!(iMultiSides & (MS_IGNORE_FENCES))))
	{
		if((tempfence) && ((target->AI.iSide == AI_FRIENDLY) || (target->AI.iSide == AI_NEUTRAL)) && (side == AI_ENEMY))
		{
			SideOK[cnt] = 1;
			needcheck = 1;
		}
		else if((!tempfence) && (target->AI.iSide == AI_ENEMY))
		{
			SideOK[cnt] = 1;
			needcheck = 1;
		}

	}
	else
	{
		SideOK[cnt] = 1;
		needcheck = 1;
	}

	if(!needcheck)
	{
		return;
	}

	interceptrange = (interceptnm * NMTOWU);
	foundrange = interceptrange;

	checkplane = &Planes[0];
	while(checkplane <= LastPlane)
	{
		playergroupcover = 0;
		playergroupcap = 0;
		if(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.winglead < 0))
			{
				if(checkplane->AI.prevpair >= 0)
				{
					if(Planes[checkplane->AI.prevpair].AI.Behaviorfunc != AIFlyEscort)
					{
						playergroupcover = 1;
					}
				}
			}
			if((checkplane->AI.winglead < 0) || (checkplane->AI.winglead == (PlayerPlane - Planes)))
			{
				if(AIIsCAPing(checkplane))
				{
					playergroupcap = 1;
				}
			}
		}

		if ((!(checkplane->AI.iAICombatFlags2 & AI_ESCORT_STAY)) && ((checkplane->Status & PL_ACTIVE) || ((checkplane->AI.iAIFlags2 & AIINVISIBLE) && (checkplane->AI.iAICombatFlags1 & AI_READY_5))) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (side == checkplane->AI.iSide) && (SideOK[checkplane->AI.iSide])
				&& (((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1)) || (playergroupcover) || (playergroupcap)) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER) && (checkplane->AI.Behaviorfunc != AIAirInvestigate) && (!(checkplane->AI.iAIFlags2 & AI_IN_DOGFIGHT)) && (!AIIsCurrentlyBombing(checkplane)) && (checkplane->AI.Behaviorfunc != AIParked) && (checkplane->AI.lAIVoice != SPCH_FAC_AIR) && (!(AIIsBomberVoice(checkplane) || AIIsSEADVoice(checkplane))))
		{
			dx = checkplane->WorldPosition.X - target->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - target->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - target->WorldPosition.Z;
			if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
			{
				tdist = QuickDistance(dx, dz);

				//  May want to modify this by checkplane's position to target.
				//  IE planes infront of target will be in a better position to
				//  intercept than planes behind (ie. would have to catch up).
				if(tdist < foundrange)
				{
					if((AIIsCAPing(checkplane)) && (((tdist * WUTONM)) > checkplane->AI.lVar2))
					{
						checkplane ++;
						continue;
					}
					else if(AIIsBeingEscorted(checkplane))
					{
						checkplane ++;
						continue;
					}
					else if(checkplane->AI.iAIFlags2 & AIINVISIBLE)
					{
						if(AIInAlertRange(checkplane, tdist))
						{
							foundplane = checkplane;
							foundrange = tdist;
						}
					}
					else if((checkplane->AI.iAICombatFlags1 & AI_WINCHESTER_AA) && (g_nMissionType != SIM_QUICK) && (g_nMissionType != SIM_TRAIN))
					{
						if(((tdist * WUTONM)) > 60.0f)
						{
							checkplane ++;
							continue;
						}
						else
						{
							for(int i=0; i<MAX_F18E_STATIONS; i++)
							{
								  if(checkplane->WeapLoad[i].Count > 0)
								  {
										int Type = pDBWeaponList[checkplane->WeapLoad[i].WeapIndex].iWeaponType;

										if(Type == WEAPON_TYPE_GUN)
										{
											foundplane = checkplane;
											foundrange = tdist;
											break;
										}
								  }
							}
						}
					}
					else
					{
						//  Check to make sure AI isn't on important mission (ie fighter/bomber on attack mission).
						foundplane = checkplane;
						foundrange = tdist;
					}
				}
			}
		}
		checkplane ++;
	}

	if(foundplane != NULL)
	{
		leadplane = AIGetLeader(foundplane);

		if((ai_can_engage) || ((foundplane->AI.iAIFlags1 & AIPLAYERGROUP) || (leadplane->Status & PL_COMM_DRIVEN)))
		{
			if(((foundplane->AI.iAIFlags1 & AIPLAYERGROUP) || (leadplane->Status & PL_COMM_DRIVEN)) && (AIIsCAPing(foundplane)))
			{
				AISetUpPlayerCAPIntercept(foundplane, target);
			}
			if(foundplane->AI.Behaviorfunc == AIFlyEscort)
			{
				AISetUpEscortInterceptGroup(foundplane, target);
			}
			else
			{
				AIUpdateInterceptGroup(foundplane, target, 1);
			}
			AISetInterceptTimer(target, 15);

			if((MultiPlayer) && (foundplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage2(foundplane, GM2_VECTOR, (BYTE)(target - Planes));
			}
		}
		else
		{
			AISetInterceptTimer(target, 3);
		}
	}
	else
	{
		AISetInterceptTimer(target, 5);
	}
}

//**************************************************************************************
int AIInAlertRange(PlaneParams *planepnt, float tdist)
{
	float tdistnm;
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	TakeOffActionType *pActionTakeOff;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_ALERT_INTERCEPT))
		{
			pActionTakeOff = (TakeOffActionType *)AIActions[actcnt].pAction;
			tdistnm = (tdist * WUTONM);
			if(tdistnm < pActionTakeOff->lRange)
			{
				return(1);
			}
			else
			{
				return(0);
			}
		}
	}
	return(1);
}

//**************************************************************************************
int AIUpdateInterceptGroup(PlaneParams *planepnt, PlaneParams *target, int firstplane)
{
	int wingman, nextsec;
	float tdist, dx, dy, dz;
	int ifoundplane;
	int givesoundcall = 1;
	int returnval = firstplane;
	PlaneParams *leadplane;

	leadplane = AIGetLeader(planepnt);

	if((!((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || (leadplane->Status & PL_COMM_DRIVEN))) || (planepnt->AI.Behaviorfunc == AIFlyEscort))
	{
		returnval = 0;
		if(planepnt->AI.iAIFlags2 & AIINVISIBLE)
		{
			planepnt->AI.iAIFlags2 &= ~AIINVISIBLE;
			planepnt->Status |= PL_ACTIVE;
			if(planepnt->AI.Behaviorfunc == CTWaitingForLaunch)
			{
				CTAddToLaunchQueue(planepnt);
			}
		}

		if(!(planepnt->Status & PL_AI_DRIVEN))
		{
			ifoundplane = -1;

			if(planepnt->AI.AirTarget != NULL)
			{
				if(AIGetLeader(planepnt->AI.AirTarget) == AIGetLeader(target))
				{
					givesoundcall = 0;
				}
			}
			else if((PlayerPlane->AGL < 20) && (GameLoop < 3000))
			{
				givesoundcall = 0;
			}

			if((givesoundcall) && (firstplane))
			{
				if(!AICheckIfAnyTargets(AIGetLeader(planepnt), AIGetLeader(target), planepnt, &ifoundplane))
				{
					AICAddSoundCall(AIC_AWACS_Threat_Msg, planepnt - Planes, 0, 50, target - Planes);
				}
				else
				{
					AICAddSoundCall(AIC_AWACS_Threat_Msg, planepnt - Planes, 15000, 50, target - Planes);
				}
			}
			if((planepnt->AI.AirTarget != target) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				GeneralClearCombatBehavior(planepnt);
			}

			planepnt->AI.AirTarget = target;
			return(returnval);
		}


		planepnt->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);

		if(firstplane)
		{
			AICCheckForEngageMsg(planepnt - Planes, target - Planes);
		}

		if((planepnt->AI.AirTarget != target) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
		{
			GeneralClearCombatBehavior(planepnt);
		}

		planepnt->AI.AirTarget = target;
		if(planepnt->OnGround)
		{
			planepnt->AI.OrgBehave = AIAirInvestigate;
			if(planepnt->OnGround != 2)
			{
				planepnt->AI.Behaviorfunc = AIFlyTakeOff;
			}
		}
		else if((planepnt->AI.OrgBehave == NULL) && (planepnt->AI.OrgBehave != AIAirInvestigate))
		{
			planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
			planepnt->AI.Behaviorfunc = AIAirInvestigate;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIAirInvestigate;
		}

		planepnt->AI.fRadarAngleYaw = AIComputeHeadingToPoint(planepnt, target->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

		if(fabs(planepnt->AI.fRadarAngleYaw) < 60)
		{
			planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(planepnt->AI.fRadarAngleYaw) + planepnt->Heading);
		}
		else
		{
			planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		}
	}

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;

	if(wingman >= 0)
	{
		if(!AIUpdateInterceptGroup(&Planes[wingman], target, returnval))
		{
			returnval = 0;
		}
	}

	if(nextsec >= 0)
	{
		if(!AIUpdateInterceptGroup(&Planes[nextsec], target, returnval))
		{
			returnval = 0;
		}
	}

	return(returnval);
}

//**************************************************************************************
void AISetInterceptTimer(PlaneParams *foundplane, int minutes)
{
	PlaneParams *checkplane;
	float dx, dy, dz;
	float includerange = 10 * NMTOWU;
	long newtimer = minutes * 60000;
	PlaneParams *leadplane = AIGetLeader(foundplane);

	checkplane = &Planes[0];
	while(checkplane <= LastPlane)
	{
		// second line added for patch so that multiple groups close together won't get "passed over"
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (foundplane->AI.iSide == checkplane->AI.iSide) &&
				((leadplane == AIGetLeader(checkplane)) || (checkplane->AI.Behaviorfunc == AIFlyEscort)))
		{
			dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
			if((fabs(dx) < includerange) && (fabs(dz) < includerange))
			{
				if(checkplane->AI.lInterceptTimer < newtimer)
				{
					checkplane->AI.lInterceptTimer = newtimer;
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
void AIUpdateGroundRadarThreat(void *radarsite, PlaneParams *checkplane, float tdist, int sitetype, GDRadarData *orgradardat)
{
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;
	GDRadarData *radardat;
	BasicInstance *walker;
	float tdistnm = tdist * WUTOFT;
	MovingVehicleParams *vehiclepnt;

	if(sitetype == GROUNDOBJECT)
	{
		walker = (BasicInstance *)radarsite;
		if(walker->Family == FAMILY_AAWEAPON)
		{
			tweapon = (AAWeaponInstance *)walker;
			if(orgradardat)
			{
				radardat = orgradardat;
			}
			else
			{
				radardat = (GDRadarData *)&tweapon->AIDataBuf;
			}
		}
		else
		{
			tprovider = (InfoProviderInstance *)walker;
			if(orgradardat)
			{
				radardat = orgradardat;
			}
			else
			{
				radardat = (GDRadarData *)&tprovider->AIDataBuf;
			}
		}
	}
	else
	{
		vehiclepnt = (MovingVehicleParams *)radarsite;
			if(orgradardat)
			{
				radardat = orgradardat;
			}
			else
			{
				radardat = vehiclepnt->RadarWeaponData;  //  DONE XXX  SCOTT CHECK THIS
			}
	}

	if(!(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED))
	{
		if(GDCheckForPingReport(radarsite, checkplane, NULL, sitetype))
		{
			radardat->lRFlags1 |= GD_HAS_BEEN_REPORTED;
		}
	}

	if(!(checkplane->Status & PL_AI_DRIVEN))
	{
		if(checkplane == PlayerPlane)
		{
			radardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
		}
		//  Do RWR stuff here for none AI planes.
	}

//	if((checkplane->AI.pPaintedBy == NULL) || (checkplane->AI.pPaintedBy == radarsite))
	if(checkplane->AI.pPaintedBy == radarsite)
	{
		checkplane->AI.pPaintedBy = radarsite;
		checkplane->AI.iPaintedByType = sitetype;
		checkplane->AI.iPaintDist = tdistnm;
		return;
	}
#if 0 //  I think this needs to be done in SimpleGroundRadar
	else if(tdistnm < checkplane->AI.iPaintDist)
	{
		checkplane->AI.pPaintedBy = radarsite;
		checkplane->AI.iPaintedByType = sitetype;
		checkplane->AI.iPaintDist = tdistnm;
		return;
	}
#endif
	return;
}

//**************************************************************************************
void AIUpdateRadarThreat(PlaneParams *planepnt, PlaneParams *checkplane, int radarzone)
{
	float dx, dz, dxorg, dzorg;
	float tdist, tdistorg;
	PlaneParams *orgthreat;

	if(!(checkplane->Status & PL_AI_DRIVEN))
	{
		if((checkplane == PlayerPlane) && (radarzone))
		{
			planepnt->AI.iAIFlags2 |= AI_RADAR_PING;
		}
		//  Do RWR stuff here for none AI planes.
	}

	if((checkplane->AI.AirThreat == NULL) || (checkplane->AI.AirThreat == planepnt))
	{
//		checkplane->AI.AirThreat = planepnt;
		GeneralSetNewAirThreat(checkplane, planepnt);
		return;
	}
	else
	{
		orgthreat = checkplane->AI.AirThreat;
		dxorg = checkplane->WorldPosition.X - orgthreat->WorldPosition.X;
		dzorg = checkplane->WorldPosition.Z - orgthreat->WorldPosition.Z;
		tdistorg = QuickDistance(dxorg, dzorg);

		dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
		dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
		tdist = QuickDistance(dx, dz);

		if((tdist < tdistorg) && ((radarzone) || (planepnt->AI.AirTarget == checkplane)))
		{
			GeneralSetNewAirThreat(checkplane, planepnt);
//			checkplane->AI.AirThreat = planepnt;
			return;
		}
	}
	return;
}

//**************************************************************************************
int AICheckIfAlreadyTarget(PlaneParams *searchplane, PlaneParams *planepnt, PlaneParams *checkplane)
{
	if(AIPlaneAlreadyTarget(AIRCRAFT, searchplane, checkplane))
	{
		return(1);
	}

	if(searchplane != planepnt)
	{
		if(planepnt == PlayerPlane)
		{
			if(IsPlaneDetected(checkplane - Planes))
			{
				if(!(planepnt->AI.iAIFlags2 & AIWINGMANTARGET))
				{
					return(1);
				}
				else if(planepnt->AI.wingman < 0)
				{
					return(1);
				}
				else if(planepnt->AI.wingman != (searchplane - Planes))
				{
					return(1);
				}
			}
		}
		else if(planepnt->AI.AirTarget == checkplane)
		{
			if(!(planepnt->AI.iAIFlags2 & AIWINGMANTARGET))
			{
				return(1);
			}
			else if(planepnt->AI.wingman < 0)
			{
				return(1);
			}
			else if(planepnt->AI.wingman != (searchplane - Planes))
			{
				return(1);
			}
		}
	}
	if(planepnt->AI.wingman >= 0)
	{
		if((planepnt != searchplane) || (!(planepnt->AI.iAIFlags2 & AIWINGMANTARGET)))
		{
			if(AICheckIfAlreadyTarget(searchplane, &Planes[planepnt->AI.wingman], checkplane))
			{
				return(1);
			}
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AICheckIfAlreadyTarget(searchplane, &Planes[planepnt->AI.nextpair], checkplane));
	}
	return(0);
}

//**************************************************************************************
int AICheckIfAnyTargets(PlaneParams *planepnt, PlaneParams *checklead, PlaneParams *searchplane, int *foundplane)
{
	PlaneParams *targlead = NULL;
	PlaneParams *target = NULL;
	float mergerange, mergerangenm;
	float dx, dz, tdist;

	if(planepnt == PlayerPlane)
	{
#if 0
		if( GetCurrentAARadarMode() == AA_STT_MODE )
		{
			if( GetCurrentPrimaryTarget() != -1 )
			{
				targlead = AIGetLeader(&Planes[ GetCurrentPrimaryTarget() ]);
				target = &Planes[ GetCurrentPrimaryTarget() ];
			}
		}
		else if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(int i=0; i<PrevFramePlanes.NumPlanes; i++)
			{
				if(PrevFramePlanes.Planes[i].PlaneId != -1)
				{
					targlead = AIGetLeader(&Planes[PrevFramePlanes.Planes[i].PlaneId]);
					target = &Planes[PrevFramePlanes.Planes[i].PlaneId];
					if(targlead == checklead)
					{
						break;
					}
				}
			}
		}
#else
		if( ( GetCurrentAARadarMode() == AA_STT_MODE) || ( GetCurrentAARadarMode() == AA_TWS_MODE ) )
		{
			if( GetCurrentPrimaryTarget() != -1)
			{
				targlead = AIGetLeader(&Planes[GetCurrentPrimaryTarget()]);
				target = &Planes[GetCurrentPrimaryTarget()];
			}
		}
#endif
	}
	else
	{
		target = planepnt->AI.AirTarget;
	}

	if((target != NULL) && (planepnt != searchplane))
	{
		if(!targlead)
		{
			targlead = AIGetLeader(target);
		}
		if(checklead != targlead)
		{
			mergerangenm = 5;
			mergerange = (mergerangenm * NMTOWU);
			dx = checklead->WorldPosition.X - target->WorldPosition.X;
			dz = checklead->WorldPosition.Z - target->WorldPosition.Z;
			tdist = QuickDistance(dx, dz);
			if(tdist < mergerange)
			{
				*foundplane = planepnt - Planes;
				return(1);
			}
		}
		else
		{
			*foundplane = planepnt - Planes;
			return(1);
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(AICheckIfAnyTargets(&Planes[planepnt->AI.wingman], checklead, searchplane, foundplane))
		{
			return(1);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AICheckIfAnyTargets(&Planes[planepnt->AI.nextpair], checklead, searchplane, foundplane));
	}
	return(0);
}

//**************************************************************************************
int AIAAStandardChecks(PlaneParams *planepnt)
{
	PlaneParams *target;
	PlaneParams *wingplane = NULL;
	double dground;
	PlaneParams *leadplane = AIGetLeader(planepnt);
	int numenemy;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return(1);

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		target = planepnt->AI.AirTarget;

		if(target != NULL)
		{
			if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
//				target = planepnt->AI.AirTarget = NULL;
				planepnt->AI.lRadarDelay = -1;
				AISimpleRadar(planepnt);

				target = planepnt->AI.AirTarget;
			}
		}
//		if(target == NULL)
		else
		{
			planepnt->AI.lRadarDelay = -1;
			AISimpleRadar(planepnt);

			target = planepnt->AI.AirTarget;
		}

		if(target == NULL)
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

			if(wingplane != NULL)
			{
				if(wingplane->AI.AirTarget != NULL)
				{
					wingplane->AI.lRadarDelay = -1;
					AISimpleRadar(wingplane);

					if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirTarget != wingplane->AI.AirTarget))
					{
						NetPutGenericMessage2(planepnt, GM2_NEW_TARGET, (BYTE)(wingplane->AI.AirTarget - Planes));
					}
					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						target = planepnt->AI.AirTarget = wingplane->AI.AirTarget;
					}
					else
					{
						target = wingplane->AI.AirTarget;
					}
				}
				else if(!(leadplane->Status & PL_AI_DRIVEN))
				{
					leadplane->AI.lRadarDelay = -1;
					AISimpleRadar(leadplane);

					if((planepnt->AI.AirTarget != leadplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirTarget != leadplane->AI.AirTarget))
						{
							NetPutGenericMessage2(planepnt, GM2_NEW_TARGET, (BYTE)((leadplane->AI.AirTarget) ? (leadplane->AI.AirTarget - Planes) : 255));
						}
						target = planepnt->AI.AirTarget = leadplane->AI.AirTarget;
					}
					else
					{
						target = leadplane->AI.AirTarget;
					}

				}
			}
		}

//		if((target == NULL) || ((0 > planepnt->AI.lTimer1) && (leadplane->Status & PL_AI_DRIVEN)))
		if((0 > planepnt->AI.lTimer1) && (leadplane->Status & PL_AI_DRIVEN))
		{
			if(planepnt->AI.Behaviorfunc != AIDisengage)
			{
				AISetFighterDisengage(planepnt);
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					ANGLE atemp1, atemp2;
					atemp1 = (planepnt->AI.DesiredHeading & 0xFF00)>>8;
					atemp2 = (planepnt->AI.DesiredHeading & 0xFF);
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 1, (BYTE)atemp1, (BYTE)atemp2);
				}
				return(1);
			}
			else if(planepnt->AI.OrgBehave == NULL)
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
				planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			}
			planepnt->AI.DesiredRoll = 0;
			planepnt->AI.OrgBehave = NULL;
			planepnt->AI.lTimer1 = -1;
			return(1);
		}
//		else if((!(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)) && (planepnt->AI.AirTarget == NULL) && (planepnt->AI.AirThreat == NULL))
		else if((planepnt->AI.AirTarget == NULL) && (planepnt->AI.AirThreat == NULL))
		{
			if(planepnt->AI.iSide == AI_FRIENDLY)
			{
				numenemy = AINumberOfSideInArea(planepnt->WorldPosition, AI_ENEMY, 40.0f * NMTOWU, 0);
				numenemy += AINumberOfSideInArea(planepnt->WorldPosition, AI_NEUTRAL, 40.0f * NMTOWU, 0);
			}
			else
			{
				numenemy = AINumberOfSideInArea(planepnt->WorldPosition, AI_FRIENDLY, 40.0f * NMTOWU, 0);
			}

			if((AICheckIfGroupAAClear(leadplane)) && (numenemy == 0))
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
					planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				}
				if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) & (planepnt->AI.iAIFlags1 & AIENGAGED))
				{
					AICAddSoundCall(AICBasicAck, planepnt - Planes, (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) * 2000, 50);
				}
				planepnt->AI.DesiredRoll = 0;
				planepnt->AI.OrgBehave = NULL;
				planepnt->AI.lTimer1 = -1;
			}
		}

#if 0  //  Not sure if I want to do this anymore.  SRE
		else if(target != NULL)
		{
			if(!(target->Status & PL_ACTIVE))
			{
				if(planepnt->AI.OrgBehave == NULL)
				{
					planepnt->AI.Behaviorfunc = AIFlyFormation;
				}
				else
				{
					planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				}
				planepnt->AI.DesiredRoll = 0;
				planepnt->AI.OrgBehave = NULL;
				planepnt->AI.lTimer1 = -1;
				return(1);
			}
		}
#endif
	}

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if(dground < 500)
	{
		planepnt->AI.Behaviorfunc = AIGainAlt;
		planepnt->AI.DesiredRoll = 0;
		AIGainAlt(planepnt);
		return(1);
	}

	if(!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
		planepnt->AI.cnt1 = 3;

	return(0);
}

//**************************************************************************************
void AIBracketTargetRight(PlaneParams *planepnt)
{
	if((AIBracketTargetHeadingOff(planepnt, 30) * WUTOFT) < (60760))
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);
	}
}

//**************************************************************************************
void AIBracketTargetLeft(PlaneParams *planepnt)
{
	if((AIBracketTargetHeadingOff(planepnt, -30) * WUTOFT) < (60760))
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);
	}
}

//**************************************************************************************
float AIBracketTargetHeadingOff(PlaneParams *planepnt, float headingoff)
{
	PlaneParams *target;
	double dx, dy, dz;
	double offangle;
	double tdist;
	double theading;
	double toffpitch;
	double dground;
	FPoint pointpos;
	int talds = 0;
	int cnt;
	DBWeaponType *pweapon_type;

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
			}
			if(talds)
			{
				planepnt->AI.lTimer3 = 3000 + ((rand() & 0x3) * 1000);
			}
			else
			{
				planepnt->AI.iAICombatFlags2 |= AI_TALDS_DONE;
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						AIC_GenericMsgPlane(planepnt - Planes, GM_DUCKS_AWAY);
						break;
					}
				}
			}
		}
		planepnt->DesiredPitch = 0;
		planepnt->DesiredRoll = 0;
		talds |= 1;
	}

	target = planepnt->AI.AirTarget;

	if(target != NULL)
	{
		dx = target->WorldPosition.X - planepnt->WorldPosition.X;
		dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, target->WorldPosition.Y) - planepnt->WorldPosition.Y;
		planepnt->AI.TargetPos = target->WorldPosition;
	}
	else
	{
		dx = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
		dz = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, planepnt->AI.TargetPos.Y) - planepnt->WorldPosition.Y;
	}

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if((dy < 0.0) && (dground < 5000))
		dy = 0.0;

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	theading = AIConvertAngleTo180Degree(planepnt->Heading);

	offangle -= (theading + headingoff);

	offangle = AICapAngle(offangle);

	MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now

	tdist = QuickDistance(dx,dz);

	if(talds)
		return(tdist);

	toffpitch = atan2(dy, tdist) * 57.2958;

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));
	return(tdist);
}

//**************************************************************************************
void AIDragTargetRight(PlaneParams *planepnt)
{
	if((AIDragTargetHeadingOff(planepnt, 90) * WUTOFT) < (60760))
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);
	}
}

//**************************************************************************************
void AIDragTargetLeft(PlaneParams *planepnt)
{
	if((AIDragTargetHeadingOff(planepnt, -90) * WUTOFT) < (60760))
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);
	}
}

//**************************************************************************************
float AIDragTargetHeadingOff(PlaneParams *planepnt, float headingoff)
{
	PlaneParams *target;
	double dx, dy, dz;
	double offangle;
	double tdist;
	double theading;
	double toffpitch;
	double dground;
	FPoint pointpos;
	int talds = 0;
	int cnt;
	DBWeaponType *pweapon_type;

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
			}
			if(talds)
			{
				planepnt->AI.lTimer3 = 3000 + ((rand() & 0x3) * 1000);
			}
			else
			{
				planepnt->AI.iAICombatFlags2 |= AI_TALDS_DONE;
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						AIC_GenericMsgPlane(planepnt - Planes, GM_DUCKS_AWAY);
						break;
					}
				}
			}
		}
		planepnt->DesiredPitch = 0;
		planepnt->DesiredRoll = 0;
		talds |= 1;
	}

	target = planepnt->AI.AirThreat;

	if(target != NULL)
	{
		dx = target->WorldPosition.X - planepnt->WorldPosition.X;
		dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, target->WorldPosition.Y) - planepnt->WorldPosition.Y;
		planepnt->AI.TargetPos = target->WorldPosition;
	}
	else
	{
		target = planepnt->AI.AirTarget;
		if(target != NULL)
		{
			dx = target->WorldPosition.X - planepnt->WorldPosition.X;
			dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;
			dy = AICheckSafeAlt(planepnt, target->WorldPosition.Y) - planepnt->WorldPosition.Y;
			planepnt->AI.TargetPos = target->WorldPosition;
		}
		else
		{
			dx = planepnt->AI.TargetPos.X - planepnt->WorldPosition.X;
			dz = planepnt->AI.TargetPos.Z - planepnt->WorldPosition.Z;
			dy = AICheckSafeAlt(planepnt, planepnt->AI.TargetPos.Y) - planepnt->WorldPosition.Y;
		}
	}

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if((dy < 0.0) && (dground < 5000))
		dy = 0.0;

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	theading = AIConvertAngleTo180Degree(planepnt->Heading);

	offangle -= (theading + headingoff);

	offangle = AICapAngle(offangle);

	MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now

	tdist = QuickDistance(dx,dz);

	if(talds)
		return(tdist);

	toffpitch = atan2(dy, tdist) * 57.2958;

	AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));
	return(tdist);
}

//**************************************************************************************
void AISplitHighTarget(PlaneParams *planepnt)
{
	if((AISplitTargetOffset(planepnt, 10000) * WUTOFT) < (60760))
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);
	}
}

//**************************************************************************************
void AISplitLowTarget(PlaneParams *planepnt)
{
	if((AISplitTargetOffset(planepnt, -10000) * WUTOFT) < (60760))
	{
//		planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
		AISwitchToAttack(planepnt, 0, 0);
	}
}

//**************************************************************************************
float AISplitTargetOffset(PlaneParams *planepnt, int offsetval)
{
	PlaneParams *target;
	FPoint pointpos;
	float ftempval;
	float lowlimit = (500 * FTTOWU);
	float tdist;
	float dy;
	int talds = 0;
	int cnt;
	DBWeaponType *pweapon_type;

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
			}
			if(talds)
			{
				planepnt->AI.lTimer3 = 3000 + ((rand() & 0x3) * 1000);
			}
			else
			{
				planepnt->AI.iAICombatFlags2 |= AI_TALDS_DONE;
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						AIC_GenericMsgPlane(planepnt - Planes, GM_DUCKS_AWAY);
						break;
					}
				}
			}
		}
	}

	target = planepnt->AI.AirTarget;

	if(target != NULL)
	{
		pointpos = target->WorldPosition;
		planepnt->AI.TargetPos = target->WorldPosition;
	}
	else
	{
		pointpos = planepnt->AI.TargetPos;
	}

	ftempval = offsetval * FTTOWU;

	ftempval = pointpos.Y + ftempval;
	pointpos.Y = ftempval;

	dy = planepnt->WorldPosition.Y - pointpos.Y;

	if(dy > (planepnt->HeightAboveGround - lowlimit))
	{
		pointpos.Y = planepnt->WorldPosition.Y - (planepnt->HeightAboveGround - lowlimit);
	}

	tdist = FlyToPoint(planepnt, pointpos) * WUTOFT;

	MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now
	return(tdist);
}

//**************************************************************************************
void AIPostHole(PlaneParams *planepnt, float rollval, float pitchval, long minalt)
{
	int talds = 0;
	int cnt;
	DBWeaponType *pweapon_type;

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
			}
			if(talds)
			{
				planepnt->AI.lTimer3 = 3000 + ((rand() & 0x3) * 1000);
			}
			else
			{
				planepnt->AI.iAICombatFlags2 |= AI_TALDS_DONE;
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						AIC_GenericMsgPlane(planepnt - Planes, GM_DUCKS_AWAY);
						break;
					}
				}
			}
		}
	}

	if((planepnt->HeightAboveGround * WUTOFT) > minalt)
	{
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(pitchval);
	}
	else
	{
		planepnt->DesiredPitch = 0;
	}

	planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);

}

//**************************************************************************************
PlaneParams *AIGetAirTargetPosition(PlaneParams *planepnt, FPoint *targetpos, int allowthreat, int allowtarget, int allowWFP)
{
	PlaneParams *target;

	targetpos->X = 0.0f;
	targetpos->Y = 0.0f;
	targetpos->Z = 0.0f;

	target = planepnt->AI.AirThreat;

	if(target)
	{
		if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			planepnt->AI.AirThreat = NULL;
			target = NULL;
		}
	}

	if((target != NULL) && (allowthreat))
	{
		targetpos->X = target->WorldPosition.X;
		targetpos->Z = target->WorldPosition.Z;
		targetpos->Y = AICheckSafeAlt(planepnt, target->WorldPosition.Y);
		planepnt->AI.TargetPos = target->WorldPosition;
	}
	else
	{
		target = planepnt->AI.AirTarget;

		if(target)
		{
			if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				planepnt->AI.AirTarget = NULL;
				target = NULL;
			}
		}

		if((target != NULL) && (allowtarget))
		{
			targetpos->X = target->WorldPosition.X;
			targetpos->Z = target->WorldPosition.Z;
			targetpos->Y = AICheckSafeAlt(planepnt, target->WorldPosition.Y);
			planepnt->AI.TargetPos = target->WorldPosition;
		}
		else if(allowWFP)
		{
			target = NULL;
			targetpos->X = planepnt->AI.TargetPos.X;
			targetpos->Z = planepnt->AI.TargetPos.Z;
			targetpos->Y = AICheckSafeAlt(planepnt, planepnt->AI.TargetPos.Y);
		}
		else
		{
			target = NULL;
		}
	}

	return(target);
}

//**************************************************************************************
void AIAirInvestigate(PlaneParams *planepnt)
{
	PlaneParams *target;
	PlaneParams *leadplane;
	FPoint targetpos;
	float tdist;

	leadplane = AIGetLeader(planepnt);

	if(planepnt == leadplane)
	{
		target = AIGetAirTargetPosition(leadplane, &targetpos);

		tdist = FlyToPoint(planepnt, targetpos) * WUTOFT;
		if(tdist <  (60 * NMTOFT))  //  182280)  //  30 nm in feet
		{
			planepnt->AI.Behaviorfunc = AIAirEngage;
			AIAirEngageSetPlane(planepnt);
		}
//		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
		MBAdjustAIThrust(planepnt, 600, 1);

		if((planepnt->AI.OrgBehave == AIFlyEscort) && (planepnt->AI.LinkedPlane))
		{
			if(planepnt->DesiredSpeed < (planepnt->AI.LinkedPlane->V + 100))
			{
				planepnt->DesiredSpeed = planepnt->AI.LinkedPlane->V + 100;
			}
		}

		AIUpdateFormation(planepnt);
	}
	else
	{
		AIFormationFlying(planepnt);

		if((leadplane->AI.Behaviorfunc == AIAirEngage) || (leadplane->AI.OrgBehave == AIAirEngage))
		{
			planepnt->AI.Behaviorfunc = AIAirEngage;
			AIAirEngageSetPlane(planepnt);
			planepnt->AI.TargetPos = leadplane->AI.TargetPos;
			planepnt->AI.iAIFlags1 &= ~AIINFORMATION;
		}
	}
}

//**************************************************************************************
float AIHeadTowardTargetPosition(PlaneParams *planepnt)
{
	PlaneParams *target;
	FPoint targetpos;
	float tdist;

	target = AIGetAirTargetPosition(planepnt, &targetpos);

	tdist = FlyToPoint(planepnt, targetpos);
	MBAdjustAIThrust(planepnt, 600, 1);

	return(tdist);
}

//**************************************************************************************
void AIAirEngage(PlaneParams *planepnt)
{
	int dodefensive = 0;
	PlaneParams *wingplane;
	int startattack = 0;
	float tdist, dx, dy, dz;
	float maxbracket = 25.0f;
	int numenemy, numfriendly;
	int enemyside;
	PlaneParams *target;
	int checkwingman = 0;
	BYTE attacktype = 0;
	BYTE btarget, bthreat;

	planepnt->AI.lTimer1 = 600000 - (planepnt->AI.iSkill * 100000);

	wingplane = NULL;
	if(planepnt->AI.wingman >= 0)
	{
		wingplane = &Planes[planepnt->AI.wingman];
		checkwingman = 1;
	}
	else if(planepnt->AI.winglead >= 0)
	{
		wingplane = &Planes[planepnt->AI.winglead];
	}

	if(wingplane)
	{
		if(planepnt->AI.AirThreat == PlayerPlane)
		{
			if(PlayerPlane->AADesignate == planepnt)
			{
				dodefensive = 1;
			}
		}
		else if(planepnt->AI.AirThreat)
		{
			if(planepnt->AI.AirThreat->AI.AirTarget == planepnt)
			{
				dodefensive = 1;
			}
		}

		if(checkwingman && dodefensive)
		{
			if(wingplane->AI.AirThreat == PlayerPlane)
			{
				if(PlayerPlane->AADesignate == wingplane)
				{
					dodefensive = 0;
				}
			}
			else if(wingplane->AI.AirThreat)
			{
				if(wingplane->AI.AirThreat->AI.AirTarget == wingplane)
				{
					dodefensive = 0;
				}
			}
		}
	}

	if((planepnt->AI.AirTarget == NULL) && (wingplane != NULL))
	{
		if(wingplane->AI.AirTarget != NULL)
		{
			if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				GeneralClearCombatBehavior(planepnt);
			}

			GeneralSetNewAirTarget(planepnt, wingplane->AI.AirTarget);
		}
		else if(wingplane->AI.AirThreat != NULL)
		{
			if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				GeneralClearCombatBehavior(planepnt);
			}
			GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
		}
		if(planepnt->AI.AirTarget != NULL)
		{
			planepnt->AI.fRadarAngleYaw = AIComputeHeadingToPoint(planepnt, planepnt->AI.AirTarget->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

			if(fabs(planepnt->AI.fRadarAngleYaw) < 60)
			{
				planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(planepnt->AI.fRadarAngleYaw) + planepnt->Heading);
			}
			else
			{
				planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
			}
		}
	}

	if(planepnt->AI.iAIFlags2 & (AI_CHECK_MISSILES|AILOCKEDUP))
	{
		dodefensive = 0;
	}
	
	switch(planepnt->AI.lVar2)
	{
		case 1:
			if(dodefensive)
			{
				if((AIDragTargetHeadingOff(planepnt, 90) * WUTOFT) < (60760))
				{
					startattack = 1;
				}
			}
			else
			{
				if((AIBracketTargetHeadingOff(planepnt, maxbracket) * WUTOFT) < (60760))
				{
					startattack = 1;
				}
			}
			break;
		case 2:
			if(dodefensive)
			{
				if((AIDragTargetHeadingOff(planepnt, -90) * WUTOFT) < (60760))
				{
					startattack = 1;
				}
			}
			else
			{
				if((AIBracketTargetHeadingOff(planepnt, -maxbracket) * WUTOFT) < (60760))
				{
					startattack = 1;
				}
			}
			break;
		case 3:
			if(dodefensive)
			{
				AIPostHole(planepnt, 45.0, -5.0, 2000);
			}
			else
			{
				if((AIBracketTargetHeadingOff(planepnt, maxbracket) * WUTOFT) < (60760))
				{
					startattack = 1;
				}
			}
			break;
		case 4:
			if(dodefensive)
			{
				AIPostHole(planepnt, -45.0, -5.0, 2000);
			}
			else
			{
				if((AIBracketTargetHeadingOff(planepnt, -maxbracket) * WUTOFT) < (60760))
				{
					startattack = 1;
				}
			}
			break;
		case 5:
			if(dodefensive)
			{
				AIPostHole(planepnt, 45.0, -5.0, 2000);
			}
			else
			{
				if((AIHeadTowardTargetPosition(planepnt) * WUTOFT) <  60760)  //  10 nm in feet
				{
					startattack = 1;
				}
			}
			break;
		default:
			break;
	}

	if(MultiPlayer && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		startattack = 0;
	}

	if(startattack)
	{
		planepnt->AI.lTimer1 = 600000 - (planepnt->AI.iSkill * 100000);

		if(planepnt->AI.AirTarget)
		{
			target = planepnt->AI.AirTarget;
			enemyside = target->AI.iSide;
			if(planepnt->AI.AirTarget)
			{
				numenemy = AINumberOfSideInArea(planepnt->AI.AirTarget->WorldPosition, enemyside, 40.0f * NMTOWU, 1);
			}
			else
			{
				numenemy = AINumberOfSideInArea(planepnt->WorldPosition, enemyside, 40.0f * NMTOWU, 1);
			}

			numfriendly = AIGetTotalNumInGroup(AIGetLeader(planepnt));

			if(((target->AI.iSkill > 2)  && (frand() < 0.5f)) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER))
			{
				planepnt->AI.Behaviorfunc = AIAllOutAttack;
				attacktype = 1;
			}
			else if((enemyside == AI_ENEMY) && ((target->AI.iSkill > 1) || (frand() < (0.25f + (target->AI.iSkill * 0.25f)))))
			{
				if(((numenemy < numfriendly) && (frand() < 0.1f)) || ((numenemy * 2) <= numfriendly))
				{
					planepnt->AI.Behaviorfunc = AIFluidFour;
					attacktype = 2;
				}
				else
				{
					planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
					attacktype = 3;
				}
			}
			else
			{
				if(((numenemy > numfriendly) && (frand() < 0.5f)) || ((numenemy) >= (numfriendly * 2)))
				{
					planepnt->AI.Behaviorfunc = AIGaggle;
					attacktype = 4;
				}
				else if((numfriendly <= 2) && ((numenemy > 2) || (frand() < 0.5f)))
				{
					planepnt->AI.Behaviorfunc = AIDoubleAttack;
					attacktype = 5;
				}
				else
				{
					planepnt->AI.Behaviorfunc = AILooseDeuce;
					attacktype = 6;
				}
			}
		}
		else if(planepnt->AI.iSide == AI_ENEMY)
		{
			planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
			attacktype = 7;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
		 	attacktype = 8;
		}

		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 &= ~AISUPPORT;
		}

		btarget = (planepnt->AI.AirTarget) ? (planepnt->AI.AirTarget - Planes) : 255;
		bthreat = (planepnt->AI.AirTarget) ? (planepnt->AI.AirTarget - Planes) : 255;
		NetPutGenericMessage4(planepnt, GM4_START_ATTACK, attacktype, btarget, bthreat);

		wingplane = NULL;
		if(planepnt->AI.wingman >= 0)
		{
			wingplane = &Planes[planepnt->AI.wingman];
		}
		else if(planepnt->AI.winglead >= 0)
		{
			wingplane = &Planes[planepnt->AI.winglead];
		}

		if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
		{
			planepnt->AI.iAICombatFlags1 &= ~(AI_HELO_BULLET_MASK);
			planepnt->AI.iAICombatFlags1 |= (rand() & 0xF) << 24;
		}


		if(wingplane != NULL)
		{
			if(pDBAircraftList[wingplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
			{
				wingplane->AI.iAICombatFlags1 &= ~(AI_HELO_BULLET_MASK);
				wingplane->AI.iAICombatFlags1 |= (rand() & 0xF) << 24;
			}

			if(wingplane->AI.Behaviorfunc == AIAirEngage)
			{
				wingplane->AI.Behaviorfunc = planepnt->AI.Behaviorfunc;
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					wingplane->AI.iAIFlags2 |= AISUPPORT;
				}
				wingplane->AI.lTimer1 = planepnt->AI.lTimer1;
			}

			if(wingplane->AI.AirTarget == NULL)
			{
				if(planepnt->AI.AirTarget != NULL)
				{
					if((wingplane->AI.AirTarget != planepnt->AI.AirTarget) && (!(wingplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(wingplane);
					}

					GeneralSetNewAirTarget(wingplane, planepnt->AI.AirTarget);
				}
				else if(planepnt->AI.AirThreat != NULL)
				{
					if((wingplane->AI.AirTarget != planepnt->AI.AirThreat) && (!(wingplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(wingplane);
					}

					GeneralSetNewAirTarget(wingplane, planepnt->AI.AirThreat);
				}
				if(wingplane->AI.AirTarget != NULL)
				{
					wingplane->AI.fRadarAngleYaw = AIComputeHeadingToPoint(wingplane, wingplane->AI.AirTarget->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

					if(fabs(wingplane->AI.fRadarAngleYaw) < 60)
					{
						wingplane->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(wingplane->AI.fRadarAngleYaw) + wingplane->Heading);
					}
					else
					{
						wingplane->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(wingplane->Heading);
					}
				}
			}
		}
	}
}

//**************************************************************************************
void AIAirEngageSetPlane(PlaneParams *planepnt)
{
	// 1 Bracket/Drag Right
	// 2 Bracket/Drag Left
	// 3 Bracket/Posthole Right
	// 4 Bracket/Posthole Left
	// 5 Straight on/Posthole.
	PlaneParams *leadplane;
	FPoint minform;
	FPoint maxform;
	int wingman, nextpair;
	int numgroup = 0;
	float ftempx, ftempy, ftempz, ftempmid, ftempval;

	leadplane = AIGetLeader(planepnt);

	minform.SetValues(0.0f, 0.0f, 0.0f);
	maxform.SetValues(0.0f, 0.0f, 0.0f);

	wingman = leadplane->AI.wingman;
	if(wingman >= 0)
	{
		AIGetFormationArea(&Planes[wingman], &minform, &maxform, &numgroup);
	}
	nextpair = leadplane->AI.nextpair;
	if(nextpair >= 0)
	{
		AIGetFormationArea(&Planes[nextpair], &minform, &maxform, &numgroup);
	}

	ftempz = planepnt->AI.FormationPosition.Z - minform.Z;
	ftempy = planepnt->AI.FormationPosition.Y - minform.Y;

	if(ftempz < 12152.0)  //  2nm
	{
		ftempmid = (maxform.X + minform.X) / 2;
		if(ftempmid < planepnt->AI.FormationPosition.X)  //  Right Side
		{
			ftempx = maxform.X - planepnt->AI.FormationPosition.X;
			ftempval = maxform.X - minform.X;
			ftempval = ftempx / ftempval;
			if(ftempval < 0.1)
			{
				planepnt->AI.lVar2 = 1;  //  Bracket/Drag Right
			}
			else
			{
				planepnt->AI.lVar2 = 3;  //  Bracket/Posthole Right
			}
		}
		else									//  Left Side
		{
			ftempx = planepnt->AI.FormationPosition.X - minform.X;
			ftempval = maxform.X - minform.X;
			ftempval = ftempx / ftempval;
			if(ftempval < 0.1)
			{
				planepnt->AI.lVar2 = 2;  //  Bracket/Drag Left
			}
			else
			{
				planepnt->AI.lVar2 = 4;  //  Bracket/Posthole Left
			}
		}
	}
	else
	{
		planepnt->AI.lVar2 = 5;
	}

	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutGenericMessage2(planepnt, GM2_ENGAGE_SET, (BYTE)planepnt->AI.lVar2);
	}
}

//**************************************************************************************
void AIGetFormationArea(PlaneParams *planepnt, FPoint *minform, FPoint *maxform, int *numgroup)
{
	int wingman, nextpair;

	minform->X = (planepnt->AI.FormationPosition.X < minform->X) ? planepnt->AI.FormationPosition.X : minform->X;
	minform->Y = (planepnt->AI.FormationPosition.Y < minform->Y) ? planepnt->AI.FormationPosition.Y : minform->Y;
	minform->Z = (planepnt->AI.FormationPosition.Z < minform->Z) ? planepnt->AI.FormationPosition.Z : minform->Z;
	maxform->X = (planepnt->AI.FormationPosition.X > maxform->X) ? planepnt->AI.FormationPosition.X : maxform->X;
	maxform->Y = (planepnt->AI.FormationPosition.Y > maxform->Y) ? planepnt->AI.FormationPosition.Y : maxform->Y;
	maxform->Z = (planepnt->AI.FormationPosition.Z > maxform->Z) ? planepnt->AI.FormationPosition.Z : maxform->Z;
	*numgroup = *numgroup + 1;

	wingman = planepnt->AI.wingman;
	if(wingman >= 0)
	{
		AIGetFormationArea(&Planes[wingman], minform, maxform, numgroup);
	}
	nextpair = planepnt->AI.nextpair;
	if(nextpair >= 0)
	{
		AIGetFormationArea(&Planes[nextpair], minform, maxform, numgroup);
	}
}

//**************************************************************************************
int AIAATargetChecks(PlaneParams *planepnt)
{
	PlaneParams *target;
	PlaneParams *wingplane = NULL;
	PlaneParams *leadplane = AIGetLeader(planepnt);
	double dground;
	int numenemy;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return(1);

	target = planepnt->AI.AirTarget;

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(target != NULL)
		{
			if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				target = planepnt->AI.AirTarget = NULL;
				if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
				{
					GeneralClearCombatBehavior(planepnt);
				}
			}
		}

		if(target == NULL)
		{
			planepnt->AI.lRadarDelay = -1;
			AISimpleRadar(planepnt);

			target = planepnt->AI.AirTarget;
		}

		if(target == NULL)
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

			if(wingplane != NULL)
			{
				if(wingplane->AI.AirTarget != NULL)
				{
					wingplane->AI.lRadarDelay = -1;
					AISimpleRadar(wingplane);

					if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					target = planepnt->AI.AirTarget = wingplane->AI.AirTarget;
				}
				else if(!(leadplane->Status & PL_AI_DRIVEN))
				{
					leadplane->AI.lRadarDelay = -1;
					AISimpleRadar(leadplane);

					if((planepnt->AI.AirTarget != leadplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					target = planepnt->AI.AirTarget = leadplane->AI.AirTarget;

				}
			}
		}

		if(target != NULL)
		{
			if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				target = NULL;
			}
		}

		if(target == NULL)
		{
			target = planepnt->AI.AirThreat;
			if(target != NULL)
			{
				if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
				{
					target = NULL;
				}
			}

			if(target == NULL)
			{
				planepnt->AI.AirTarget = NULL;
				planepnt->AI.AirThreat = NULL;
				if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
				{
					GeneralClearCombatBehavior(planepnt);
				}
			}
		}

//		if((target == NULL) || ((0 > planepnt->AI.lTimer1) && (leadplane->Status & PL_AI_DRIVEN)))
		if((0 > planepnt->AI.lTimer1) && (leadplane->Status & PL_AI_DRIVEN))
		{
			if(planepnt->AI.Behaviorfunc != AIDisengage)
			{
				if((!(rand() & 3)) && (PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
				{
//					PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
//					lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
					iLastWSOWarnType = TARGET_PLANE;
					pLastWSOWarnObj = planepnt;

//					AICAddSoundCall(AICDoWSOEnemyDisengaging, PlayerPlane - Planes, 10000, 50, planepnt - Planes);
				}

				AISetFighterDisengage(planepnt);

				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					ANGLE atemp1, atemp2;
					atemp1 = (planepnt->AI.DesiredHeading & 0xFF00)>>8;
					atemp2 = (planepnt->AI.DesiredHeading & 0xFF);
					NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 1, (BYTE)atemp1, (BYTE)atemp2);
				}
				return(1);
			}
			else if(planepnt->AI.OrgBehave == NULL)
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
				planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			}
			planepnt->AI.DesiredRoll = 0;
			planepnt->AI.OrgBehave = NULL;
			planepnt->AI.lTimer1 = -1;
			return(1);
		}
//		else if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) && (planepnt->AI.AirTarget == NULL) && (planepnt->AI.AirThreat == NULL))
		else if((planepnt->AI.AirTarget == NULL) && (planepnt->AI.AirThreat == NULL))
		{
			if(planepnt->AI.iSide == AI_FRIENDLY)
			{
				numenemy = AINumberOfSideInArea(planepnt->WorldPosition, AI_ENEMY, 40.0f * NMTOWU, 0);
				numenemy += AINumberOfSideInArea(planepnt->WorldPosition, AI_NEUTRAL, 40.0f * NMTOWU, 0);
			}
			else
			{
				numenemy = AINumberOfSideInArea(planepnt->WorldPosition, AI_FRIENDLY, 40.0f * NMTOWU, 0);
			}

			if((AICheckIfGroupAAClear(leadplane)) && (numenemy == 0))
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
					planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				}
				if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) & (planepnt->AI.iAIFlags1 & AIENGAGED))
				{
					AICAddSoundCall(AICBasicAck, planepnt - Planes, (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) * 2000, 50);
				}
				planepnt->AI.DesiredRoll = 0;
				planepnt->AI.OrgBehave = NULL;
				planepnt->AI.lTimer1 = -1;
			}
		}
#if 0  //  Not sure if I want to do this anymore
		else if(target != NULL)
		{
			if(!(target->Status & PL_ACTIVE))
			{
				if(planepnt->AI.OrgBehave == NULL)
				{
					planepnt->AI.Behaviorfunc = AIFlyFormation;
				}
				else
				{
					planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				}
				planepnt->AI.DesiredRoll = 0;
				planepnt->AI.OrgBehave = NULL;
				planepnt->AI.lTimer1 = -1;
				return(1);
			}
		}
#endif
	}

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if(dground < 500)
	{
		planepnt->AI.iAIFlags2 |= AIGROUNDAVOID;
	}

	if(!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
		planepnt->AI.cnt1 = 3;

	return(0);
}

//**************************************************************************************
void AIGainAltitude(PlaneParams *planepnt)
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

	if(dground > 2000)
	{
		planepnt->AI.iAIFlags2 &= ~AIGROUNDAVOID;
		planepnt->AI.DesiredRoll = 0;
	}
	else
	{
		if(degpitch > 0)
		{
			MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now
		}
		else
		{
			MBAdjustAIThrust(planepnt, 250, 1);  //  temp for now
		}

		degroll = AIConvertAngleTo180Degree(planepnt->Roll);

		if(fabs(degroll) < 90)
		{
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(15);
		}

		planepnt->DesiredRoll = 0;
	}
}

//**************************************************************************************
void AIMissileAvoid(PlaneParams *planepnt, float *roffangle, float *rtdist)
{
	AIMissileBreakToHeading(planepnt);
	return;
}

//**************************************************************************************
void AIDefensiveACM(PlaneParams *planepnt, float *roffangle, float *rtdist)
{
	PlaneParams *threat;
	PlaneParams *wingplane;
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
	float deltayaw, tdeltayaw;
	float yawtime;
	float circlec, radft;
	float defensiverange = 12000.0f;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	planepnt->AI.cnt1 = 2;


	if(planepnt->AI.winglead == (PlayerPlane - Planes))
	{
		if(planepnt->AI.iAIFlags2 & AIKEEPTARGET)
		{
			planepnt->AI.iAIFlags2 &= ~(AIKEEPTARGET);
		}

		WingNumDTWSTargets = 0;
	}

	if(AIAATargetChecks(planepnt))
	{
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 &= ~AIDEFENSIVE;
		}
		return;
	}

	threat = AIGetAirTargetPosition(planepnt, &threatpos);

	if(threat == NULL)
	{
		FlyToPoint(planepnt, threatpos);
		MBAdjustAIThrust(planepnt, 600, 1);
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 &= ~AIDEFENSIVE;
		}
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
			if(!(planepnt->AI.iAIFlags2 & AISUPPORT))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
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

		if(planepnt->AI.iAICombatFlags1 & (AI_FUEL_TANKS))
		{
			if(tdist < (10.0f * NMTOWU))
			{
				AIDropTanks(planepnt);
			}
		}

		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			defensiverange = 24000.0f;
		}

//		if((fabs(toffangle) > 30) || (fabs(offangle) < 90) || ((tdist * WUTOFT) > 12000))
		if((fabs(toffangle) > 45) || (fabs(offangle) < 90) || ((tdist * WUTOFT) > defensiverange))
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				planepnt->AI.iAIFlags2 &= ~AIDEFENSIVE;
			}
			return;
		}

		degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

		toffpitch = atan2(-dy, -tdist) * 57.2958;

		if(planepnt->AI.lCombatTimer < 0)
		{
			AICheckForNewDefensiveTactic(planepnt, threat, offangle, toffpitch, tdist, toffangle);
		}

		if(planepnt->AI.CombatBehavior)
		{
 			planepnt->AI.CombatBehavior(planepnt, threat, offangle, toffpitch, tdist, toffangle);
 			if((planepnt->AI.CombatBehavior == NULL) || (planepnt->AI.lCombatTimer < 0))
 			{
 				planepnt->AI.CombatPos.X = offangle;
 				planepnt->AI.CombatPos.Z = toffangle;
 			}
			return;
		}

		deltayaw = fabs(AIConvertAngleTo180Degree(planepnt->Heading - planepnt->AI.LastHeading));
		tdeltayaw = fabs(AIConvertAngleTo180Degree(threat->Heading - threat->AI.LastHeading));

		degroll = AIConvertAngleTo180Degree(planepnt->Roll);

		if((fabs(offangle) < 90) || ((deltayaw > (tdeltayaw)) && (fabs(toffangle) > 10.0f) && (fabs(degroll) > 45)))
		{
			if(planepnt->Roll < 0x8000)
			{
				desiredroll = 90.0;
			}
			else
			{
				desiredroll = -90.0;
			}
		}
		else
		{
			circlec = (360.0f / planepnt->YawRate) * planepnt->V;
			radft = circlec / (2 * PI);

			yawtime = (fabs(offangle) - 90) / planepnt->YawRate;
			yawtime += (fabs(90 - fabs(degroll))) / planepnt->MaxRollRate;

//			if(yawtime < ((tdist - radft) / threat->V))
			if(yawtime < (tdist / threat->V))
			{
				if(offangle > 0)
				{
					desiredroll = 90.0;
				}
				else
				{
					desiredroll = -90.0;
				}
			}
			else if((planepnt->Roll < 0x8000) && (threat->Roll > 0x8000))
			{
				desiredroll = 90.0f;
			}
			else if((planepnt->Roll > 0x8000) && (threat->Roll < 0x8000))
			{
				desiredroll = -90.0f;
			}
			else if(toffangle < 0)
			{
				desiredroll = -90.0;
			}
			else
			{
				desiredroll = 90.0;
			}
		}

	//	dground = ((planepnt->WorldPosition.Y - LandHeight(planepnt->WorldPosition.X, planepnt->WorldPosition.Z)) * WUTOFT) ;
		dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 3000))* WUTOFT;

		if(((toffpitch > 0) || (dground < 3000)) && (degpitch < 20) && (dground < 6000))  //  was dground < 8000)
		{
			if(desiredroll < 0)
				desiredroll += 30;
			else
				desiredroll -= 30;
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
		droll = -AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll);

		if(fabs(droll) < 5.0)
		{
#if 0  //  Non lift vector
			if(fabs(degroll) < 90)
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(15.0);
			}
			else
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(340.0);
			}
#else
			if(fabs(degroll) < 90)
			{
				planepnt->DesiredPitch = 0x1000;
			}
			else
			{
				planepnt->DesiredPitch = 0xE000;
			}

			if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredPitch - planepnt->Pitch)) > 3.0f)
			{
				planepnt->RollYawCouple = -1;
				planepnt->MaxPitchRate = planepnt->YawRate;
			}
#endif
		}
		else if((dground < 3000) && (degpitch < 0) && (fabs(degroll) < 85))
		{
#if 0  //  Non lift vector
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(15);
#else
			planepnt->DesiredPitch = 0x1000;
			planepnt->RollYawCouple = -1;
			planepnt->MaxPitchRate = planepnt->YawRate;
#endif
		}

		MBAdjustAIThrust(planepnt, 600, 1);  // temp until we have a max speed
	}
	return;
}

//**************************************************************************************
void AISupportACM(PlaneParams *planepnt, float *roffangle, float *rtdist)
{
	PlaneParams *target;
	float dx, dy, dz;
	float offangle;
	float tdist, tdistnm;
	PlaneParams *wingplane = NULL;
	FPoint targetpos;
	float desiredoa;
	float dyaw, dalt, dzft;
//	float tdist;
//	int cnt;
	float modifier = 1.0f;

	if(AIAATargetChecks(planepnt))
	{
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
		return;
	}

	target = AIGetAirTargetPosition(planepnt, &targetpos, 0);

	offangle = AIComputeHeadingToPoint(planepnt, targetpos, &tdist, &dx ,&dy, &dz, 1);
	tdistnm = (tdist * WUTONM);  //  Convert to nm

	if(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA)
	{
		modifier = 0.5f;
	}

	if(tdistnm > (4.5 * modifier))
	{
		if(fabs(offangle) > 30)
		{
			desiredoa = (offangle < 0) ? -30 : 30;
		}
		else
		{
			desiredoa = offangle;
		}
	}
	else if(tdistnm < (2.0 * modifier))
	{
		if(fabs(offangle) < 120)
		{
			desiredoa = (offangle < 0) ? -120 : 120;
		}
		else
		{
			desiredoa = offangle;
		}
	}
	else
	{
		if(fabs(offangle) < 30)
		{
			desiredoa = (offangle < 0) ? -30 : 30;
		}
		else if(fabs(offangle) > 120)
		{
			desiredoa = (offangle < 0) ? -120 : 120;
		}
		else
		{
			desiredoa = offangle;
		}
	}
	dyaw = offangle - desiredoa;

	dzft = dz * WUTOFT;
	dalt = 0;

	if(dzft > 0)
	{
		if((planepnt->HeightAboveGround * WUTOFT) > 2000)
		{
			if(dzft < 2000)
			{
				dalt = 2000 - dzft;
			}
			else if(dzft > 4000)
			{
				dalt = 4000 - dzft;
			}
		}
		else
		{
			if(dzft > -2000)
			{
				dalt = -2000 - dzft;
			}
			else if(dzft < -4000)
			{
				dalt = -4000 - dzft;
			}
		}
	}
	else
	{
		if(dzft > -2000)
		{
			dalt = -2000 - dzft;
		}
		else if(dzft < -4000)
		{
			dalt = -4000 - dzft;
		}
	}

	AIChangeDir(planepnt, (double)dyaw, (double)dalt);

	MBAdjustAIThrust(planepnt, 475, 1);  // temp until we have a max speed

	AICheckIfDefensive(planepnt, roffangle, rtdist);

	*rtdist = tdist;
	*roffangle = offangle;
}

//**************************************************************************************
void AIOffensiveACM(PlaneParams *planepnt, float *roffangle, float *rtdist)
{
	PlaneParams *target;
	double dx, dy, dz;
	double offangle, toffangle;
	double dground;
	double tdist, tdistnm;
	double heading, theading;
	double toffpitch;
	PlaneParams *wingplane = NULL;
	FPoint targetpos;
	float defensiverange = 12000.0f;
	int cnt, checkguns;
//	double tdist;
//	int cnt;


	if(AIAATargetChecks(planepnt))
	{
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
		if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
		{
			GeneralClearCombatBehavior(planepnt);
		}
		return;
	}

	target = AIGetAirTargetPosition(planepnt, &targetpos, 0);

	if(target == NULL)
	{
		target = AIGetAirTargetPosition(planepnt, &targetpos);
	}

	planepnt->AADesignate = target;

	if(target == NULL)
	{
		if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
		{
			GeneralClearCombatBehavior(planepnt);
		}
		FlyToPoint(planepnt, targetpos);
		MBAdjustAIThrust(planepnt, 600, 1);

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
			if(!(planepnt->AI.iAIFlags2 & AISUPPORT))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
	}
	else
	{
		dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

		dx = target->WorldPosition.X - planepnt->WorldPosition.X;
		dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, target->WorldPosition.Y) - planepnt->WorldPosition.Y;

//		if((dy < 0.0) && (dground < 5000))
//			dy = 0.0;

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

		*roffangle = offangle;
		*rtdist = tdist;

		if(planepnt->AI.iAICombatFlags1 & (AI_FUEL_TANKS))
		{
			if(tdist < (10.0f * NMTOWU))
			{
				AIDropTanks(planepnt);
			}
		}

#if 0
		if((fabs(toffangle) < 30) && (fabs(offangle) > 90) && ((tdist * WUTOFT) < 12000))
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
			}
			if(planepnt->AI.lCombatTimer < 0)
			{
				planepnt->AI.CombatBehavior = NULL;
			}
			return;
		}
#endif
		if(!(pDBAircraftList[target->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			defensiverange = 24000.0f;
		}

		if((fabs(offangle) < 30) && (fabs(toffangle) > 90) && ((tdist * WUTOFT) < defensiverange))
		{
			if((lDefensiveTimer >= 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
			{
				lDefensiveTimer += DeltaTicks;
				if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDefensiveTimer > GOOD_ENEMY_TIME))
				{
//					PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
//					lDefensiveTimer = -10000;
					iLastWSOWarnType = TARGET_PLANE;
					pLastWSOWarnObj = planepnt;
//					AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_GOOD, 1000, 50, AICF_ENEMY_GOOD);
				}
			}

			if(target != NULL)
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					target->AI.iAIFlags2 |= AIDEFENSIVE;
				}
				GeneralSetNewAirThreat(target, planepnt);
//				target->AI.AirThreat = planepnt;
			}
		}
		else if((lDefensiveTimer > 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			lDefensiveTimer = 0;
			PlayerPlane->AI.iAIFlags2 &= ~AIDEFENSIVE;
		}



	//	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);
	//		planepnt->LeftThrustPercent = planepnt->RightThrustPercent = 60.0;

	// 		planepnt->CommandedThrust = GetCommandedThrust(planepnt);

		toffpitch = atan2(dy, tdist) * 57.2958;
		tdistnm = tdist * WUTONM;

		if(!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
		{
			if(planepnt->AI.lCombatTimer < 0)
			{
				AICheckForNewOffensiveTactic(planepnt, target, offangle, toffpitch, tdist, toffangle);
			}

			if(planepnt->AI.CombatBehavior)
			{
 				planepnt->AI.CombatBehavior(planepnt, target, offangle, toffpitch, tdist, toffangle);
				if((planepnt->AI.CombatBehavior == NULL) || (planepnt->AI.lCombatTimer < 0))
				{
					planepnt->AI.CombatPos.X = offangle;
					planepnt->AI.CombatPos.Z = toffangle;
				}
			}
			else if((tdistnm < 1.5f))  //   && (fabs(toffangle) > 90))
			{
				planepnt->AI.CombatPos.X = offangle;
				planepnt->AI.CombatPos.Z = toffangle;
				if((iAI_ROE[planepnt->AI.iSide] > 1) && (!(planepnt->DamageFlags & (DAMAGE_BIT_GUN))) && (planepnt->AI.cInteriorDamage[AI_REAR_GUN] == -1))
				{
					checkguns = 1;

					if(fabs(offangle) > 60.0f)
					{
						checkguns = 0;
					}

					if((tdistnm < 0.3f) && (checkguns))
					{
						for(cnt = 0; cnt <MAX_F18E_STATIONS ; cnt ++)
						{
							if(planepnt->WeapLoad[cnt].Count > 0)
							{
	  							if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE)
								{
	  								if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].fMinRange <= tdistnm)
									{
	  									if((pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iSeekerType != 3) || (fabs(toffangle) > 115.0f))
										{
											checkguns = 0;
											break;
										}
									}
								}
							}
						}
					}
					if(checkguns)
					{
						if(!AICheckForGuns(planepnt, target, offangle, toffpitch, tdist))
						{
							AI_ACM(planepnt, target, offangle, toffpitch, tdist, toffangle);
						}
					}
					else
					{
						AI_ACM(planepnt, target, offangle, toffpitch, tdist, toffangle);
					}
				}
				else
				{
					AI_ACM(planepnt, target, offangle, toffpitch, tdist, toffangle);
				}
			}
			else
			{
				planepnt->AI.CombatPos.X = offangle;
				planepnt->AI.CombatPos.Z = toffangle;
				AI_ACM(planepnt, target, offangle, toffpitch, tdist, toffangle);
			}

			planepnt->AI.cnt1 = 3;
		}

	}
	if(planepnt->AI.lCombatTimer < 0)
	{
		GeneralClearCombatBehavior(planepnt);
		planepnt->AI.CombatBehavior = NULL;
	}
	return;
}

//**************************************************************************************
void AICoreAttack(PlaneParams *planepnt, float *roffangle, float *rtdist)
{
	*rtdist = -1.0f;
	*roffangle = 0.0f;
	int talds = 0;
	int cnt;
	DBWeaponType *pweapon_type;

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
			}
			if(talds)
			{
				planepnt->AI.lTimer3 = 3000 + ((rand() & 0x3) * 1000);
			}
			else
			{
				planepnt->AI.iAICombatFlags2 |= AI_TALDS_DONE;
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					pweapon_type = &pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex];
					if(pweapon_type->lWeaponID == 140)  //  TALD
					{
						AIC_GenericMsgPlane(planepnt - Planes, GM_DUCKS_AWAY);
						break;
					}
				}
			}
		}
	}

	if(!planepnt->AI.CombatBehavior)
	{
		planepnt->AI.iAICombatFlags1 &= ~(AI_FINISH_BEHAVIOR);
	}

	planepnt->AI.iAIFlags2 |= (AI_IN_DOGFIGHT);

	if(planepnt->AI.iAIFlags2 & AIGROUNDAVOID)
	{
		if((lDefensiveTimer > 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			lDefensiveTimer = 0;
		}

		GeneralClearCombatBehavior(planepnt);
		AIGainAltitude(planepnt);
		if(planepnt->Pitch < 0x8000)
		{
			AICheckIfDefensive(planepnt, roffangle, rtdist);
		}
	}
	else if(planepnt->AI.iAIFlags2 & AIMISSILEDODGE)
	{
		if((lDefensiveTimer > 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			lDefensiveTimer = 0;
		}

		if(planepnt->AI.iAICombatFlags1 & AI_OFFENSIVE)
		{
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				GeneralClearCombatBehavior(planepnt);
			}
			planepnt->AI.iAICombatFlags1 &= ~AI_OFFENSIVE;
		}
		AIMissileAvoid(planepnt, roffangle, rtdist);
	}
	else if(planepnt->AI.iAIFlags2 & AIDEFENSIVE)
	{
		if((lDefensiveTimer > 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			lDefensiveTimer = 0;
		}

		if(planepnt->AI.iAICombatFlags1 & AI_OFFENSIVE)
		{
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				GeneralClearCombatBehavior(planepnt);
			}
			planepnt->AI.iAICombatFlags1 &= ~AI_OFFENSIVE;
		}
		AIDefensiveACM(planepnt, roffangle, rtdist);
	}
	else if(planepnt->AI.iAIFlags2 & AISUPPORT)
	{
		if((lDefensiveTimer > 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			lDefensiveTimer = 0;
		}

		if(!(planepnt->AI.iAICombatFlags1 & AI_OFFENSIVE))
		{
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				GeneralClearCombatBehavior(planepnt);
			}
			planepnt->AI.iAICombatFlags1 |= AI_OFFENSIVE;
		}
		if(planepnt->AI.CombatBehavior)
		{
			AIOffensiveACM(planepnt, roffangle, rtdist);
		}
		else
		{
			AISupportACM(planepnt, roffangle, rtdist);
		}
	}
	else
	{
		if(!(planepnt->AI.iAICombatFlags1 & AI_OFFENSIVE))
		{
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				GeneralClearCombatBehavior(planepnt);
			}
			planepnt->AI.iAICombatFlags1 |= AI_OFFENSIVE;
		}
		AIOffensiveACM(planepnt, roffangle, rtdist);
	}

	if(planepnt->AI.lCombatTimer < 0)
	{
		GeneralClearCombatBehavior(planepnt);
		planepnt->AI.CombatBehavior = NULL;
	}
}

//**************************************************************************************
void AIAllOutAttack(PlaneParams *planepnt)
{
	float rtdist, roffangle;

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		planepnt->AI.iAIFlags2 &= ~AISUPPORT;
	}
	AICoreAttack(planepnt, &roffangle, &rtdist);
}

//**************************************************************************************
void AIFightingWing(PlaneParams *planepnt)
{
	float rtdist, roffangle;
	PlaneParams *wingplane;
	float tdist, dx, dy, dz;
	double angzx, angzz;
	double heading;
	float followdist = 3000 * FTTOWU;

	planepnt->AI.iAIFlags2 |= AIWINGMANTARGET;
	planepnt->AI.iAIFlags2 |= AIKEEPTARGET;

	wingplane = NULL;
	if(planepnt->AI.wingman >= 0)
	{
		wingplane = &Planes[planepnt->AI.wingman];
	}
	else if(planepnt->AI.winglead >= 0)
	{
		wingplane = &Planes[planepnt->AI.winglead];
	}

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(wingplane != NULL)
		{
			if((!(planepnt->AI.iAIFlags2 & AISUPPORT)) && (wingplane->AI.iAIFlags2 & AIDEFENSIVE))
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					planepnt->AI.iAIFlags2 &= ~(AIDEFENSIVE);
				}
				if(wingplane->AI.AirThreat != NULL)
				{
					if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
				}
			}
			else if(planepnt->AI.iAIFlags2 & AISUPPORT)
			{
				if(wingplane->AI.AirTarget != NULL)
				{
					if(planepnt->AI.AirTarget != wingplane->AI.AirTarget)
					{
						if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
						{
							GeneralClearCombatBehavior(planepnt);
						}

						GeneralSetNewAirTarget(planepnt, wingplane->AI.AirTarget);

						planepnt->AI.fRadarAngleYaw = AIComputeHeadingToPoint(planepnt, planepnt->AI.AirTarget->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

						if(fabs(planepnt->AI.fRadarAngleYaw) < 60)
						{
							planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(planepnt->AI.fRadarAngleYaw) + planepnt->Heading);
						}
						else
						{
							planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
						}
					}
				}
			}
		}
	}

	if((wingplane == NULL) || ((!(planepnt->AI.iAIFlags2 & AISUPPORT)) || (planepnt->AI.iAIFlags2 & (AIMISSILEDODGE))))
	{
		AICoreAttack(planepnt, &roffangle, &rtdist);
	}
	else
	{
		if(wingplane->AI.iAIFlags2 & AIDEFENSIVE)
		{
			AIOffensiveACM(planepnt, &roffangle, &rtdist);
		}
		else
		{
			heading = DegToRad((double)planepnt->Heading / DEGREE);
			angzx = sin(heading);
			angzz = cos(heading);

			wingplane->AI.WorldFormationPos.SetValues(angzx,(double)wingplane->WorldPosition.Y,angzz);

			planepnt->AI.WorldFormationPos.X = wingplane->WorldPosition.X + ConvertWayLoc(angzx * followdist);
			planepnt->AI.WorldFormationPos.Y = wingplane->WorldPosition.Y;
			planepnt->AI.WorldFormationPos.Z = wingplane->WorldPosition.Z + ConvertWayLoc(angzz * followdist);

			AIFormationFlyingLead(planepnt, wingplane);
			planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
		}
	}

	if(wingplane == NULL)
	{
		NetUpdateSupport(planepnt, 0);
	}
	else if(!(planepnt->AI.iAIFlags2 & AISUPPORT))
	{
		if(!(wingplane->AI.iAIFlags2 & AISUPPORT))
		{
			NetUpdateSupport(wingplane, 1, planepnt);
		}
	}
}

//**************************************************************************************
void AIDoubleAttack(PlaneParams *planepnt)
{
	float rtdist, roffangle;
	PlaneParams *wingplane;
	float tdist, dx, dy, dz;

	planepnt->AI.iAIFlags2 |= AIWINGMANTARGET;
	planepnt->AI.iAIFlags2 |= AIKEEPTARGET;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		AICoreAttack(planepnt, &roffangle, &rtdist);
		return;
	}

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
		if((!(planepnt->AI.iAIFlags2 & AISUPPORT)) && (wingplane->AI.iAIFlags2 & AIDEFENSIVE))
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				planepnt->AI.iAIFlags2 &= ~(AIDEFENSIVE);
			}
			if(wingplane->AI.AirThreat != NULL)
			{
				if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					GeneralClearCombatBehavior(planepnt);
				}

				GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
			}
		}
		else if(planepnt->AI.iAIFlags2 & AISUPPORT)
		{
			if(wingplane->AI.AirTarget != NULL)
			{
				if(planepnt->AI.AirTarget != wingplane->AI.AirTarget)
				{
					if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					GeneralSetNewAirTarget(planepnt, wingplane->AI.AirTarget);
					planepnt->AI.fRadarAngleYaw = AIComputeHeadingToPoint(planepnt, planepnt->AI.AirTarget->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

					if(fabs(planepnt->AI.fRadarAngleYaw) < 60)
					{
						planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(planepnt->AI.fRadarAngleYaw) + planepnt->Heading);
					}
					else
					{
						planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
					}
				}
			}
		}
	}

	AICoreAttack(planepnt, &roffangle, &rtdist);

	if(wingplane == NULL)
	{
		NetUpdateSupport(planepnt, 0);
	}
	else if(!(planepnt->AI.iAIFlags2 & AISUPPORT))
	{
		if(!(wingplane->AI.iAIFlags2 & AISUPPORT))
		{
			NetUpdateSupport(wingplane, 1, planepnt);
		}
		if(planepnt->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE))
		{
			if(wingplane != NULL)
			{
				if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
				{
					NetUpdateSupport(planepnt, 1, wingplane);
				}
			}
		}
		else if((fabs(roffangle) > 90) && ((rtdist * WUTOFT) < 12000))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
	}
}

//**************************************************************************************
int AIGoodTargetEngage(PlaneParams *planepnt, float roffangle, float rtdist)
{
	FPoint targetpos;
	PlaneParams *target;
	float tdist, dx, dy, dz, offangle, rollval;

	target = AIGetAirTargetPosition(planepnt, &targetpos, 0);
	if(target == NULL)
	{
		target = AIGetAirTargetPosition(planepnt, &targetpos);
	}

	if(target == NULL)
	{
		return(0);
	}

	if(fabs(roffangle) > 90)
	{
		return(0);
	}

	if(fabs(rtdist * WUTONM) < 2.5)
	{
		return(0);
	}

	offangle = AIComputeHeadingToPoint(target, planepnt->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

	rollval = AIConvertAngleTo180Degree(planepnt->Roll);

	if((fabs(offangle) < 30) || ((fabs(offangle) > 150) && (fabs(rollval) > 10)))
	{
		return(0);
	}

	if((fabs(rollval) > 25) && (fabs(rollval) < 155))
	{
		if((rollval > 0) && (offangle < 0))
		{
			return(1);
		}
		if((rollval < 0) && (offangle > 0))
		{
			return(1);
		}
	}
	else if((fabs(rollval) < 10) && (fabs(offangle) > 150))
	{
	 	return(1);
	}

	return(0);
}

//**************************************************************************************
void AILooseDeuce(PlaneParams *planepnt)
{
	float rtdist, roffangle;
	PlaneParams *wingplane;
	float tdist, dx, dy, dz;

	planepnt->AI.iAIFlags2 |= AIWINGMANTARGET;
	planepnt->AI.iAIFlags2 |= AIKEEPTARGET;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		AICoreAttack(planepnt, &roffangle, &rtdist);
		return;
	}

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
		if((!(planepnt->AI.iAIFlags2 & AISUPPORT)) && (wingplane->AI.iAIFlags2 & AIDEFENSIVE))
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				planepnt->AI.iAIFlags2 &= ~(AIDEFENSIVE);
			}
			if(wingplane->AI.AirThreat != NULL)
			{
				if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					GeneralClearCombatBehavior(planepnt);
				}

				GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
			}
		}
		else if(planepnt->AI.iAIFlags2 & AISUPPORT)
		{
			if(wingplane->AI.AirTarget != NULL)
			{
				if(planepnt->AI.AirTarget != wingplane->AI.AirTarget)
				{
					if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						GeneralClearCombatBehavior(planepnt);
					}

					GeneralSetNewAirTarget(planepnt, wingplane->AI.AirTarget);
					planepnt->AI.fRadarAngleYaw = AIComputeHeadingToPoint(planepnt, planepnt->AI.AirTarget->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

					if(fabs(planepnt->AI.fRadarAngleYaw) < 60)
					{
						planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(planepnt->AI.fRadarAngleYaw) + planepnt->Heading);
					}
					else
					{
						planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
					}
				}
			}
		}
	}

	AICoreAttack(planepnt, &roffangle, &rtdist);

	if(wingplane == NULL)
	{
		NetUpdateSupport(planepnt, 0);
	}
	else if(planepnt->AI.iAIFlags2 & AISUPPORT)
	{
		if(AIGoodTargetEngage(planepnt, roffangle, rtdist))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 0, wingplane);
			}
		}
	}
	else
	{
		if(!(wingplane->AI.iAIFlags2 & AISUPPORT))
		{
			NetUpdateSupport(wingplane, 1, planepnt);
		}
		if(planepnt->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
		else if((fabs(roffangle) > 90) && ((rtdist * WUTOFT) < 12000))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
		if((!(planepnt->AI.iAIFlags2 & AISUPPORT)) && (wingplane->AI.iAIFlags2 & AIDEFENSIVE))
		{
			if(wingplane->AI.AirThreat != NULL)
			{
				if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					GeneralClearCombatBehavior(planepnt);
				}

				GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
			}
		}
	}
}

//**************************************************************************************
void AIFluidFour(PlaneParams *planepnt)
{
	float rtdist, roffangle;

	AICoreAttack(planepnt, &roffangle, &rtdist);
}

//**************************************************************************************
void AIGaggle(PlaneParams *planepnt)
{
	float rtdist, roffangle;
	PlaneParams *wingplane;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		AICoreAttack(planepnt, &roffangle, &rtdist);
		return;
	}

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
		if(planepnt->AI.AirTarget != wingplane->AI.AirTarget)
		{
			NetUpdateSupport(planepnt, 0);
		}

		if((!(planepnt->AI.iAIFlags2 & AISUPPORT)) && (wingplane->AI.iAIFlags2 & AIDEFENSIVE))
		{
			if(wingplane->AI.AirThreat != NULL)
			{
				if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					GeneralClearCombatBehavior(planepnt);
				}

				GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
			}
		}
	}

	AICoreAttack(planepnt, &roffangle, &rtdist);

	if(wingplane == NULL)
	{
		NetUpdateSupport(planepnt, 0);
	}
	else if(planepnt->AI.iAIFlags2 & AISUPPORT)
	{
		if(AIGoodTargetEngage(planepnt, roffangle, rtdist))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 0, wingplane);
			}
		}
	}
	else
	{
		if(planepnt->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
		else if((fabs(roffangle) > 90) && ((rtdist * WUTOFT) < 12000))
		{
			if(!(wingplane->AI.iAIFlags2 & (AIDEFENSIVE|AIMISSILEDODGE)))
			{
				NetUpdateSupport(planepnt, 1, wingplane);
			}
		}
		if((!(planepnt->AI.iAIFlags2 & AISUPPORT)) && (wingplane->AI.iAIFlags2 & AIDEFENSIVE))
		{
			if(wingplane->AI.AirThreat != NULL)
			{
				if((planepnt->AI.AirTarget != wingplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					GeneralClearCombatBehavior(planepnt);
				}

				GeneralSetNewAirTarget(planepnt, wingplane->AI.AirThreat);
			}
		}
	}
}

//**************************************************************************************
void AICheckRearGuns(PlaneParams *planepnt)
{
	double distance, pdist;
	float tempheading, temppitch;
	int cnt;
	int reargun;
	int inrange = 0;
	FPointDouble gunpos;
	FPoint aimpoint;
	FPoint offset;
	FPOffSet toffset;
	float heading, pitch;
	float usetime = -1.0f;
	DBWeaponType *weapon;
	float bulletvel;
	float radardist = NMTOWU;
	float gundist = NMTOWU;
	float randeffect;
	float coneval = 0.3f;  //  0.5f;
	int radarid;
	DBRadarType *radar;
	int playernew = 0;
	float fworkval;

	if(((!planepnt->AI.AirThreat) || (planepnt->AI.AirThreat != PlayerPlane)) && (planepnt->AI.iSide != PlayerPlane->AI.iSide))
	{
		if(planepnt->AI.AirThreat)
		{
			distance = (planepnt->AI.AirThreat->WorldPosition - planepnt->WorldPosition);
		}
		else
		{
			distance = -1;
		}
		pdist = (PlayerPlane->WorldPosition - planepnt->WorldPosition);

		if((pdist < distance) && (distance >= 0))
		{
			distance = pdist;
			GeneralSetNewAirThreat(planepnt, PlayerPlane);
//			planepnt->AI.AirThreat = PlayerPlane;
			playernew = 1;
		}
		else if((distance <= radardist) && (distance < 0))
		{
			distance = pdist;
			GeneralSetNewAirThreat(planepnt, PlayerPlane);
//			planepnt->AI.AirThreat = PlayerPlane;
			playernew = 1;
		}
		else
		{
			planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
			return;
		}
	}
	else if(planepnt->AI.AirThreat)
	{
		distance = (planepnt->AI.AirThreat->WorldPosition - planepnt->WorldPosition);
	}
	else
	{
		planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
		return;
	}

	radarid = pDBAircraftList[planepnt->AI.iPlaneIndex].lRadarID;
	radar = GetRadarPtr(radarid);
	if(radar)
	{
		radardist = radar->iMaxRange;
		radardist *= NMTOWU;
	}

	if(distance > radardist)
	{
		planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
		planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
		planepnt->AI.lGunTimer = 3000;
		return;
	}

	if(!planepnt->AI.AirThreat)
	{
		planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
		return;
	}

	PLGetRelativeHP(planepnt, planepnt->AI.AirThreat->WorldPosition, &tempheading, &temppitch);
	if(planepnt->AI.lPlaneID == 49)
	{
		if((fabs(temppitch) < 30) && (tempheading > 60) && (tempheading < 120))
		{
			inrange = 1;
		}
	}
	else if((fabs(temppitch) < 30) && (fabs(tempheading) > 150))
	{
		inrange = 1;
	}

	if(inrange)
	{
	//  Do radar Stuff here
		if((planepnt->AI.AirThreat == PlayerPlane) && (radar))
		{
			planepnt->AI.iAIFlags2 |= (AI_RADAR_PING|AILOCKEDUP);
			if(playernew)
			{
				GDCheckForPingReport(NULL, PlayerPlane, planepnt);
			}
		}
		else
		{
			planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
		}

		reargun = -1;
		for(cnt = 0; cnt <MAX_F18E_STATIONS ; cnt ++)
		{
			if((planepnt->WeapLoad[cnt].Count > 0) && ((planepnt->WeapLoad[cnt].WeapId == 75) || (planepnt->WeapLoad[cnt].WeapId == 76) || (planepnt->WeapLoad[cnt].WeapId == 95)))  //   || (Planes[i].WeapLoad[k].WeapId == 96) || (Planes[i].WeapLoad[k].WeapId == 97))
			{
				reargun = cnt;
				break;
			}
		}

		if(reargun != -1)
		{
			weapon = &pDBWeaponList[planepnt->WeapLoad[reargun].WeapIndex];
			gundist = weapon->iRange;
			if(gundist)
			{
				gundist *= NMTOWU;
			}
			else
			{
				gundist = NMTOWU;
			}
			if(distance > gundist)
			{
				inrange = 0;
			}
		}
		else
		{
			inrange = 0;
		}
	}
	else
	{
		planepnt->AI.iAIFlags2 &= ~(AI_RADAR_PING|AILOCKEDUP);
	}


	if(inrange)
	{
		bulletvel = weapon->fMaxSpeed;
		if(bulletvel == 0)
		{
			bulletvel = 3380;
		}

		toffset = pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[10];
		offset.X = toffset.X * FOOT_MULTER;
		offset.Y = toffset.Y * FOOT_MULTER;
		offset.Z = toffset.Z * FOOT_MULTER;

		gunpos.X = planepnt->WorldPosition.X + planepnt->Orientation.I.X*(double)offset.Z + planepnt->Orientation.J.X*(double)offset.X + planepnt->Orientation.K.X*(double)offset.Y;
		gunpos.Y = planepnt->WorldPosition.Y + planepnt->Orientation.I.Y*(double)offset.Z + planepnt->Orientation.J.Y*(double)offset.X + planepnt->Orientation.K.Y*(double)offset.Y;
		gunpos.Z = planepnt->WorldPosition.Z + planepnt->Orientation.I.Z*(double)offset.Z + planepnt->Orientation.J.Z*(double)offset.X + planepnt->Orientation.K.Z*(double)offset.Y;

		GDComputeGunHP(planepnt->AI.AirThreat, gunpos, &aimpoint, bulletvel, &heading, &pitch, &usetime, 0, 1);

		randeffect = (frand() * (coneval * 2)) - coneval;
		pitch += randeffect;
		randeffect = (frand() * (coneval * 2)) - coneval;
		heading += randeffect;

		InstantiateTurretBullet(planepnt, TRUE, planepnt->WeapLoad[reargun].Type, AIConvert180DegreeToAngle(heading), AIConvert180DegreeToAngle(pitch), &offset);

		fworkval = (float)weapon->iRateOfFire / 60.0f;  //  Seconds per shot
		fworkval = 1000.0f / fworkval;		//  Ticks per shot

		if(fworkval <= DeltaTicks)
		{
			planepnt->WeapLoad[reargun].Count = planepnt->WeapLoad[reargun].Count - (DeltaTicks / fworkval);
//			planepnt->AI.fStoresWeight -= pDBWeaponList[planepnt->WeapLoad[reargun].WeapIndex].iWeight * (DeltaTicks / fworkval);
			planepnt->AI.fStoresWeight -= (DeltaTicks / fworkval);

			planepnt->AI.lGunTimer -= DeltaTicks;
			if(planepnt->AI.lGunTimer < 0)
			{
				planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
				planepnt->AI.lGunTimer = 3000;
			}
		}
		else
		{
			planepnt->WeapLoad[reargun].Count = planepnt->WeapLoad[reargun].Count - 1;
//			planepnt->AI.fStoresWeight -= pDBWeaponList[planepnt->WeapLoad[reargun].WeapIndex].iWeight;
			planepnt->AI.fStoresWeight -= 1.0f;

			planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
			planepnt->AI.lGunTimer = DeltaTicks - fworkval;
		}
	}
	else
	{
		planepnt->AI.lGunTimer = 1000;
	}
}

//**************************************************************************************
void AISwitchToAttack(PlaneParams *planepnt, int defensive, int missile)
{
	PlaneParams *target, *leadthreat;
	int enemyside, numenemy, numfriendly;
	PlaneParams *checkplane;
	int useairport;
	FPointDouble airportpos;
	FPointDouble fptemp;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;
	PlaneParams *leadplane;
	BYTE attacktype = 0;
	BYTE sendflag = 0;
	BYTE targetnum;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	if((planepnt->OnGround) || (planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		return;
	}

	if(planepnt->AI.iAIFlags2 & (AI_IN_DOGFIGHT))
	{
		return;
	}
	if((planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1) || (planepnt->AI.iAICombatFlags2 & AI_ESCORTABLE))
	{
		if(missile)
		{
			planepnt->AI.Behaviorfunc = AIMissileBreakToHeading;
			attacktype = 1;
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_SWITCH_ATTACK, attacktype, 0, 0);
			}
		}
		return;
	}


	planepnt->AI.iAIFlags2 |= (AI_IN_DOGFIGHT);
	if(planepnt->AI.OrgBehave == NULL)
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
	}

#if 0
	if(planepnt->AI.iSide == AI_ENEMY)
		planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
	else
		planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
#else

	target = planepnt->AI.AirTarget;
	if(!target)
	{
		target = planepnt->AI.AirThreat;
		sendflag |= 4;
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
	{
		planepnt->AI.lTimer1 = 300000 - (planepnt->AI.iSkill * 100000);
		useairport = 1;
		if((AIGetClosestAWACS(PlayerPlane) >= 0) && (target))
		{
			leadthreat = AIGetLeader(target);
			for(checkplane = Planes; useairport && (checkplane <= LastPlane); checkplane ++)
			{
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide)
						&& (checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER) && ((checkplane->AI.Behaviorfunc == AIAirInvestigate) || (checkplane->AI.iAIFlags2 & AI_IN_DOGFIGHT)) && (checkplane->AI.AirTarget))
				{
					if(AIGetLeader(checkplane->AI.AirTarget) == leadthreat)
					{
						useairport = 0;
						planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(atan2(-(checkplane->WorldPosition.X - planepnt->WorldPosition.X), -(checkplane->WorldPosition.Z - planepnt->WorldPosition.Z)) * 57.2958);
					}
				}
			}
		}
		if(useairport)
		{
			fptemp.SetValues(-1.0f,-1.0f,-1.0f);
			if(AIGetClosestAirField(planepnt, fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
			{
				planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(atan2(-(runwaypos.X - planepnt->WorldPosition.X), -(runwaypos.Z - planepnt->WorldPosition.Z)) * 57.2958);
			}
			else
			{
				planepnt->AI.DesiredHeading = planepnt->Heading;
			}
		}
		planepnt->AI.Behaviorfunc = AIDisengage;
		leadplane = AIGetLeader(planepnt);
		AISetGroupDisengage(leadplane, leadplane);
		planepnt->AI.LinkedPlane = NULL;
		attacktype = 2;
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			ANGLE atemp1, atemp2;
			atemp1 = (planepnt->AI.DesiredHeading & 0xFF00)>>8;
			atemp2 = (planepnt->AI.DesiredHeading & 0xFF);
			NetPutGenericMessage4(planepnt, GM4_SWITCH_ATTACK, attacktype, (BYTE)atemp1, (BYTE)atemp2);
		}
	}
	else if(target)
	{
		numfriendly = AIGetTotalNumInGroup(AIGetLeader(planepnt));
		enemyside = target->AI.iSide;
		if(planepnt->AI.AirTarget)
		{
			numenemy = AINumberOfSideInArea(planepnt->AI.AirTarget->WorldPosition, enemyside, 40.0f * NMTOWU, 1);
		}
		else
		{
			numenemy = AINumberOfSideInArea(planepnt->WorldPosition, enemyside, 40.0f * NMTOWU, 1);
		}

		if(((target->AI.iSkill > 2)  && (frand() < 0.5f)) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER))
		{
			planepnt->AI.Behaviorfunc = AIAllOutAttack;
			attacktype = 3;
		}
		else if((enemyside == AI_ENEMY) && ((target->AI.iSkill > 1) || (frand() < (0.25f + (target->AI.iSkill * 0.25f)))))
		{
			if(((numenemy < numfriendly) && (frand() < 0.1f)) || ((numenemy * 2) <= numfriendly))
			{
				planepnt->AI.Behaviorfunc = AIFluidFour;
				attacktype = 4;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
				attacktype = 5;
			}
		}
		else
		{
			if(((numenemy > numfriendly) && (frand() < 0.5f)) || ((numenemy) >= (numfriendly * 2)))
			{
				planepnt->AI.Behaviorfunc = AIGaggle;
				attacktype = 6;
			}
			else if(numfriendly <= 2)
			{
				planepnt->AI.Behaviorfunc = AIDoubleAttack;
				attacktype = 7;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AILooseDeuce;
				attacktype = 8;
			}
		}
	}
	else if(planepnt->AI.iSide == AI_ENEMY)
	{
		planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
	 	attacktype = 5;
	}
	else
	{
		planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
	 	attacktype = 8;
	}
#endif

	if(defensive)
	{
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
		}
		sendflag |= 1;
	}

	if(missile)
	{
		planepnt->AI.iAIFlags2 |= AIMISSILEDODGE;
		sendflag |= 2;
	}

	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(target)
		{
			targetnum = target - Planes;
		}
		else
		{
			targetnum = 255;
		}

		NetPutGenericMessage4(planepnt, GM4_SWITCH_ATTACK, attacktype, sendflag, targetnum);
	}
}

//**************************************************************************************
int AICheckForGuns(PlaneParams *planepnt, PlaneParams *target, float offangle, float offpitch, float planedist)
{
	int gunslot = -1;
	int cnt;
	int inrange = 0;
	FPointDouble gunpos;
	FPoint aimpoint;
	FPoint offset;
	FPOffSet toffset;
	float heading, pitch;
	float usetime = -1.0f;
	DBWeaponType *weapon;
	float bulletvel;
	float gundist = NMTOWU;
	float randeffect;
	float coneval = 0.3f;  //  0.25f;  //  0.5f;
	float fworkval;
	float dheading, dpitch, addsizeangle;
	int shoot = 0;
	float dx, dy, dz, dground, tdist;
	double toffpitch, toffangle;
	float ftempvar, ftempvar2;
	int ishelo = 0;
	int tempval;
	float ftempval;
	float fnumshots;
	int numshots = 1;
	int straightshot = 0;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		ishelo;
	}

	if(planepnt->AI.iAIFlags2 & AI_GUN_PAUSE)
	{
		if(planepnt->AI.lGunTimer >= 0)
		{
			planepnt->AI.lGunTimer -= DeltaTicks;
			if(planepnt->AI.lGunTimer > 10000)
			{
				return(0);
			}
		}

		if(planepnt->AI.lGunTimer < 0)
		{
			planepnt->AI.iAIFlags2 &= ~(AI_GUN_PAUSE);
			planepnt->AI.lGunTimer = 500 + (planepnt->AI.iSkill * 300);
			if(ishelo)
			{
				planepnt->AI.iAICombatFlags1 &= ~(AI_HELO_BULLET_MASK);
				planepnt->AI.iAICombatFlags1 |= (rand() & 0xF) << 24;
			}
		}
	}

	for(cnt = 0; cnt <MAX_F18E_STATIONS ; cnt ++)
	{
		if((planepnt->WeapLoad[cnt].Count > 0) && (planepnt->WeapLoad[cnt].WeapId != 75) && (planepnt->WeapLoad[cnt].WeapId != 76) && (planepnt->WeapLoad[cnt].WeapId != 95) && (planepnt->WeapLoad[cnt].WeapId != 96) && (planepnt->WeapLoad[cnt].WeapId != 97))
		{
	  		if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType == WEAPON_TYPE_GUN)
			{
				gunslot = cnt;
				break;
			}
		}
	}

	if(gunslot == -1)
	{
		planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
		planepnt->AI.lGunTimer = 360000;
		return(0);
	}

	weapon = &pDBWeaponList[planepnt->WeapLoad[gunslot].WeapIndex];
	gundist = weapon->iRange;
	if(gundist)
	{
		gundist *= NMTOWU;
	}
	else
	{
		gundist = NMTOWU;
	}

	bulletvel = weapon->fMaxSpeed;
	if(bulletvel == 0)
	{
		bulletvel = 3380;
	}

	toffset = pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[10];
	offset.X = toffset.X * FOOT_MULTER;
	offset.Y = toffset.Y * FOOT_MULTER;
	offset.Z = toffset.Z * FOOT_MULTER;

	gunpos.X = planepnt->WorldPosition.X + planepnt->Orientation.I.X*(double)offset.Z + planepnt->Orientation.J.X*(double)offset.X + planepnt->Orientation.K.X*(double)offset.Y;
	gunpos.Y = planepnt->WorldPosition.Y + planepnt->Orientation.I.Y*(double)offset.Z + planepnt->Orientation.J.Y*(double)offset.X + planepnt->Orientation.K.Y*(double)offset.Y;
	gunpos.Z = planepnt->WorldPosition.Z + planepnt->Orientation.I.Z*(double)offset.Z + planepnt->Orientation.J.Z*(double)offset.X + planepnt->Orientation.K.Z*(double)offset.Y;

	GDComputeGunHP(target, gunpos, &aimpoint, bulletvel, &heading, &pitch, &usetime, 0, 1);

	dheading = heading - AIConvertAngleTo180Degree(planepnt->Heading);

	dheading = AICapAngle(dheading);

	dpitch = pitch - AIConvertAngleTo180Degree(planepnt->Pitch);

	dpitch = AICapAngle(dpitch);

	addsizeangle = fabs(atan2(-target->Type->ShadowLRZOff, planedist) * 57.2958) + 1.0f;

	if((fabs(dpitch) < addsizeangle) && (fabs(dheading) < addsizeangle) && (gundist > planedist))
	{
		shoot = 1;

		if((planepnt->AI.lPlaneID == 54) || (planepnt->AI.lPlaneID == 55) || (planepnt->AI.lPlaneID == 59))
		{
			tempval = (planepnt->AI.iAICombatFlags1 & AI_HELO_BULLET_MASK)>>24;
			ftempval = 8 - tempval;
			if(planepnt->AI.iSkill > 1)
			{
				tempval *= 2;
			}
			heading = AICapAngle(heading + ftempval);
		}
		else
		{
			pitch = AIConvertAngleTo180Degree(planepnt->Pitch);
			heading = AIConvertAngleTo180Degree(planepnt->Heading);
			straightshot = 1;
		}
	}
	else
	{
		if((planepnt->AI.lPlaneID == 54) || (planepnt->AI.lPlaneID == 55) || (planepnt->AI.lPlaneID == 59))
		{
			if((fabs(dpitch) < 110.0f) && (fabs(dheading) < 30.0f) && (gundist > planedist))
			{
				shoot = 1;

				tempval = (planepnt->AI.iAICombatFlags1 & AI_HELO_BULLET_MASK)>>24;
				ftempval = 8 - tempval;
				if(planepnt->AI.iSkill > 1)
				{
					tempval *= 2;
				}
				heading = AICapAngle(heading + ftempval);
			}
			else
			{
				planepnt->AI.iAICombatFlags1 &= ~(AI_HELO_BULLET_MASK);
				planepnt->AI.iAICombatFlags1 |= (rand() & 0xF) << 24;
			}
		}
		else if((fabs(dpitch) < (addsizeangle + (planepnt->AI.iSkill * 0.3f))) && (fabs(dheading) < (addsizeangle + (planepnt->AI.iSkill * 0.3f))) && (gundist > planedist))
		{
			shoot = 1;
			pitch = AIConvertAngleTo180Degree(planepnt->Pitch);
			heading = AIConvertAngleTo180Degree(planepnt->Heading);
			straightshot = 1;
		}
		else
		{
			if(ishelo)
			{
				planepnt->AI.iAICombatFlags1 &= ~(AI_HELO_BULLET_MASK);
				planepnt->AI.iAICombatFlags1 |= (rand() & 0xF) << 24;
			}
		}
	}

	if((shoot) && (!(planepnt->AI.iAIFlags2 & AI_GUN_PAUSE)))
	{
		randeffect = (frand() * (coneval * 2)) - coneval;
		pitch += randeffect;
		randeffect = (frand() * (coneval * 2)) - coneval;
		heading += randeffect;

		fworkval = (float)weapon->iRateOfFire / 60.0f;  //  Seconds per shot
		fworkval = 1000.0f / fworkval;		//  Ticks per shot

		if(fworkval <= DeltaTicks)
		{
			fnumshots = DeltaTicks;
			fnumshots = (fnumshots / fworkval);

			numshots = fnumshots;
			if(numshots > planepnt->WeapLoad[gunslot].Count)
			{
				numshots = planepnt->WeapLoad[gunslot].Count;
			}

			planepnt->WeapLoad[gunslot].Count = planepnt->WeapLoad[gunslot].Count - (DeltaTicks / fworkval);
//			planepnt->AI.fStoresWeight -= pDBWeaponList[planepnt->WeapLoad[gunslot].WeapIndex].iWeight * (DeltaTicks / fworkval);
			planepnt->AI.fStoresWeight -= (DeltaTicks / fworkval);

			planepnt->AI.lGunTimer -= DeltaTicks;
			if(planepnt->AI.lGunTimer < 0)
			{
				planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
				planepnt->AI.lGunTimer = 3000;
			}
		}
		else
		{
			numshots = 1;
			planepnt->WeapLoad[gunslot].Count = planepnt->WeapLoad[gunslot].Count - 1;
//			planepnt->AI.fStoresWeight -= pDBWeaponList[planepnt->WeapLoad[gunslot].WeapIndex].iWeight;
			planepnt->AI.fStoresWeight -= 1.0f;
			if(ishelo)
			{
				if(!(planepnt->WeapLoad[gunslot].Count & 0x63))
				{
					planepnt->AI.iAICombatFlags1 &= ~(AI_HELO_BULLET_MASK);
					planepnt->AI.iAICombatFlags1 |= (rand() & 0xF) << 24;
				}
			}
			planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
			planepnt->AI.lGunTimer = DeltaTicks - fworkval;
		}

		InstantiateTurretBullet(planepnt, TRUE, planepnt->WeapLoad[gunslot].Type, AIConvert180DegreeToAngle(heading), AIConvert180DegreeToAngle(pitch), &offset, numshots);

		if((straightshot) && (MultiPlayer))
		{
			planepnt->AI.iAICombatFlags1 |= AI_MULTI_FIRE_GUN;
		}
	}



	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	dx = aimpoint.X - planepnt->WorldPosition.X;
	dz = aimpoint.Z - planepnt->WorldPosition.Z;
	dy = AICheckSafeAlt(planepnt, aimpoint.Y) - planepnt->WorldPosition.Y;

//	if((dy < 0.0) && (dground < 5000))
//		dy = 0.0;

	toffangle = atan2(-dx, -dz) * 57.2958;

	toffangle = AICapAngle(toffangle);

	toffangle -= AIConvertAngleTo180Degree(planepnt->Heading);

	toffangle = AICapAngle(toffangle);

	tdist = sqrt((dx * dx) + (dz * dz));

	toffpitch = atan2(dy, tdist) * 57.2958;

	AIChangeDirPitch(planepnt, toffangle, AIConvert180DegreeToAngle(toffpitch));
//	MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now
	if(tdist > (NMTOWU * 0.25f))
	{
		fworkval = 1.0f + (tdist * WUTONM * 0.10f);
		if(fworkval > 1.25f)
		{
			fworkval = 1.25f;
		}
		planepnt->DesiredSpeed = target->V * fworkval;
	}
	else
	{
		planepnt->DesiredSpeed = target->V;
	}

	planepnt->DesiredSpeed += fabs(sin(DegToRad(toffangle)) * planepnt->DesiredSpeed * 0.25f);

	if((fabs(AIConvertAngleTo180Degree(target->Heading - planepnt->Heading)) < 90.0f) && (fabs(toffangle) < 45.0f))
	{
		ftempvar2 = sqrt((2*tdist*WUTOFT)/planepnt->MaxSpeedAccel);
		if(ftempvar2 > 1.0f)
		{
			ftempvar2 -= 1.0f;
		}

		ftempvar2 *= planepnt->MaxSpeedAccel;
		ftempvar = planepnt->V - target->V;
		if((ftempvar > ftempvar2) && (planepnt->DesiredSpeed > target->V))
		{
			planepnt->DesiredSpeed = target->V;
		}
	}

	AICounterTargetMovement(planepnt, target, offangle, offpitch, aimpoint, toffangle);

	return(1);
}

//**************************************************************************************
void AICounterTargetMovement(PlaneParams *planepnt, PlaneParams *target, float offangle, float offpitch, FPoint aimpoint, double desiredyaw)
{
	float dx, dy, dz, tdist;
	float toffangle;
	float fworkvar;
	ANGLE workpitch;
	FPoint leadpoint;
	double yawval;
	float disttravel;
	float toffpitch;
	ANGLE workheading;
	FPointDouble fpdleadpoint;
	workpitch = AIGetPlanesFlightPitch(target, 1);

	fworkvar = (sin(DegToRad((double)workpitch / DEGREE)) * target->V);
	leadpoint.Y = (target->WorldPosition.Y + (fworkvar / WUTOFT));
	aimpoint.Y += (fworkvar / WUTOFT);

	disttravel = (cos(DegToRad((double)workpitch / DEGREE)) * target->V);

	workheading = target->Heading;
	workheading += (target->Heading - target->AI.LastHeading);

	fworkvar = (sin(DegToRad((double)workheading / DEGREE)) * disttravel);
	leadpoint.X = (target->WorldPosition.X - (fworkvar / WUTOFT));
	aimpoint.X -= (fworkvar / WUTOFT);

	fworkvar = (cos(DegToRad((double)workheading / DEGREE)) * disttravel);
	leadpoint.Z = (target->WorldPosition.Z - (fworkvar / WUTOFT));
	aimpoint.Z -= (fworkvar / WUTOFT);

	float ttoff;
	ttoff = toffangle = AIComputeHeadingToPoint(planepnt, leadpoint, &tdist, &dx ,&dy, &dz, 1);

	toffangle -= offangle;

	toffangle = AICapAngle(toffangle);

#if 0  //  this had a problem with capping yawrate
	yawval = (planepnt->RollYawCouple) ? (sin(DegToRad(planepnt->DesiredRoll*(1.0f/(float)DEGREE)))*(planepnt->YawRate)) : 0.0f;

	yawval -= toffangle; //  minus since flight expects clockwise to be positive, and coordinate system produces clockwise neg.

	if(fabs(yawval) > planepnt->YawRate)
	{
		yawval = (yawval >= 0) ? planepnt->YawRate : -planepnt->YawRate;
	}
	planepnt->DesiredRoll = ((planepnt->RollYawCouple) && (planepnt->YawRate)) ? AIConvert180DegreeToAngle(RadToDeg(asin(yawval / planepnt->YawRate))) : 0.0f;
#else
	desiredyaw += toffangle;

	desiredyaw = AICapAngle(desiredyaw);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -desiredyaw, 0);

	yawval = (planepnt->RollYawCouple) ? (sin(DegToRad(planepnt->DesiredRoll*(1.0f/(float)DEGREE)))*(planepnt->YawRate)) : 0.0f;
#endif

	toffpitch = atan2(dy, tdist) * 57.2958;

	toffpitch = AICapAngle(toffpitch);

	toffpitch -= AIConvertAngleTo180Degree(planepnt->Pitch);

	toffpitch = AICapAngle(toffpitch);

	fworkvar = DeltaTicks;
	fworkvar /= 1000.0f;

	toffpitch *= fworkvar;

	planepnt->DesiredPitch += AIConvert180DegreeToAngle(toffpitch);

	fpdleadpoint = aimpoint;

	AICheckForLiftVectorUsage(planepnt, target, fpdleadpoint, desiredyaw, AIConvertAngleTo180Degree(planepnt->DesiredPitch), yawval);
}

#if 1
//**************************************************************************************
void AI_ACM(PlaneParams *planepnt, PlaneParams *target, double offangle, double toffpitch, double dist, double toffangle)
{
	float ftempvar, ftempvar2;

	planepnt->DesiredPitch = AIConvert180DegreeToAngle(toffpitch);

	AICheckForLiftVectorUsage(planepnt, target, target->WorldPosition, offangle, toffpitch);

	MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now

	if((fabs(offangle) < 90) && (fabs(toffangle) > 90) && (dist < (NMTOWU * 0.25f)))
	{
		planepnt->DesiredSpeed = target->V;
	}


	if((fabs(toffangle) > 135.0f) && (fabs(offangle) < 45.0f))
	{
		ftempvar2 = sqrt((2*dist*WUTOFT)/planepnt->MaxSpeedAccel);
		ftempvar2 *= planepnt->MaxSpeedAccel;
		ftempvar = planepnt->V - target->V;
		if((ftempvar > ftempvar2) && (planepnt->DesiredSpeed > target->V))
		{
			planepnt->DesiredSpeed = target->V;
		}
	}
}
#else
//**************************************************************************************
void AI_ACM(PlaneParams *planepnt, PlaneParams *target, double offangle, double toffpitch, double dist, double toffangle)
{
	float ftempvar, ftempvar2;
	float deltayaw, tdeltayaw;
	float yawtime;
	float circlec, radft;

	planepnt->DesiredPitch = AIConvert180DegreeToAngle(toffpitch);

	AICheckForLiftVectorUsage(planepnt, target, target->WorldPosition, offangle, toffpitch);

	if(planepnt->RollYawCouple == -1)
	{
		circlec = (360.0f / planepnt->YawRate) * planepnt->V;
		radft = circlec / (2 * PI);

		yawtime = (fabs(offangle) - 90) / planepnt->YawRate;
		yawtime += (fabs(90 - fabs(degroll))) / planepnt->MaxRollRate;

//			if(yawtime < ((tdist - radft) / threat->V))
		if(yawtime < (tdist / threat->V))
	}

	MBAdjustAIThrust(planepnt, 600, 1);  //  temp for now

	if((fabs(offangle) < 90) && (fabs(toffangle) > 90) && (dist < (NMTOWU * 0.25f)))
	{
		planepnt->DesiredSpeed = target->V;
	}


	if((fabs(toffangle) > 135.0f) && (fabs(offangle) < 45.0f))
	{
		ftempvar2 = sqrt((2*dist*WUTOFT)/planepnt->MaxSpeedAccel);
		ftempvar2 *= planepnt->MaxSpeedAccel;
		ftempvar = planepnt->V - target->V;
		if((ftempvar > ftempvar2) && (planepnt->DesiredSpeed > target->V))
		{
			planepnt->DesiredSpeed = target->V;
		}
	}
}
#endif

//**************************************************************************************
void AICheckHumanThreats(PlaneParams *planepnt, PlaneParams *checkplane, float dx, float dy, float dz, float visualrange)
{
	int playerpinged = 0;
	float fdist, tdist;
	float offangle;
	float tempheading, temppitch;

	if(planepnt->AI.AirThreat == checkplane)
	{
		return;
	}

	if((fabs(dx) > visualrange) || (fabs(dz) > visualrange))
	{
		if(!IsPlaneDetected(planepnt - Planes))
		{
			return;
		}
		playerpinged = 1;
	}

	if(planepnt->AI.AirThreat == NULL)
	{
		tdist = -1.0f;
	}
	else
	{
		tdist = planepnt->WorldPosition - planepnt->AI.AirThreat->WorldPosition;
	}

	fdist = planepnt->WorldPosition - checkplane->WorldPosition;
	if((fdist > tdist) && (tdist >= 0))
	{
		return;
	}

	if(!playerpinged)
	{
		offangle = atan2(dx, dz) * 57.2958;  //  Normally -dx, -dz, but since they are from planepnt to checkplane we need neg for checkplane to planepnt;
		if(fabs(offangle) > 90)
		{
			return;
		}
		PLGetRelativeHP(planepnt, checkplane->WorldPosition, &tempheading, &temppitch);
		if(PLClipViewHP(&tempheading, &temppitch, 0))
		{
			return;
		}
	}

	GeneralSetNewAirThreat(planepnt, checkplane);
//	planepnt->AI.AirThreat = checkplane;
}

//**************************************************************************************
void AICheckForNewOffensiveTactic(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double tdist, double toffangle)
{
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	DBAircraftType *target_data = &pDBAircraftList[target->AI.iPlaneIndex];
	float yawrate, tyawrate, degroll;
	float chance;
	long workspeed;
	float cornerspeed = -1.0f;
	float dvel;
	float relvel;
	float fworkval;
	float ftempvar, ftempvar2;
	float circlec, radft;
	float circletime;
	int randspch;
	PlaneParams *wingman;
	BYTE btempvar, targetnum;

	planepnt->AI.iAICombatFlags1 &= ~(AI_FINISH_BEHAVIOR);

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(planepnt->AI.CombatBehavior == AIGetSeperation)
		{
			GeneralClearCombatBehavior(planepnt);
			AIAfterSeperation(planepnt, target, offangle, offpitch, tdist, toffangle);
			if(planepnt->AI.CombatBehavior)
			{
				return;
			}
		}
	}

	if((planepnt->Pitch > 0x2000) && (planepnt->Pitch < 0xE000))
	{
		if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			if(((planepnt->Pitch > 0xA000) && (planepnt->Pitch < 0xE000)) && ((planepnt->WorldPosition.Y - PlayerPlane->WorldPosition.Y) < (-500.0f * FTTOWU)))
			{
				if((PlayerPlane->Pitch > 0xE000) || (PlayerPlane->Pitch < 0x8000))
				{
					PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
					lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
					iLastWSOWarnType = TARGET_PLANE;
					pLastWSOWarnObj = planepnt;
					if(lAdvisorFlags & ADV_CASUAL_BETTY)
					{
						AICGenericBettySpeech(BETTY_ENEMY_U_D, 0);
					}
//					AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_WEEDS, 1000, 50, AICF_ENEMY_WEEDS);
				}
			}
			if(((planepnt->Pitch > 0x2000) && (planepnt->Pitch < 0x6000)) && ((planepnt->WorldPosition.Y - PlayerPlane->WorldPosition.Y) > (500.0f * FTTOWU)))
			{
				if((PlayerPlane->Pitch > 0x8000) || (PlayerPlane->Pitch < 0x2000))
				{
					PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
					lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
					iLastWSOWarnType = TARGET_PLANE;
					pLastWSOWarnObj = planepnt;
					if(lAdvisorFlags & ADV_CASUAL_BETTY)
					{
						AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
					}
//					AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 1000, 50, AICF_ENEMY_VERTICLE);
				}
			}
		}

		if((lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (fabs(toffangle) > 60))
			{
				PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
				lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
				iLastWSOWarnType = TARGET_PLANE;
				pLastWSOWarnObj = planepnt;

				AICAddSoundCall(AIC_WSO_BanditCall, PlayerPlane - Planes, 1000, 50, planepnt - Planes);
			}
		}

		return;
	}

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if(fabs(offangle) < 45)
		{
			if((PlayerPlane->AI.wingman == (planepnt - Planes)) && (fabs(toffangle) < 90))
			{
				if((PlayerPlane->AI.iAIFlags2 & AIDEFENSIVE) && (frand() < 0.25f) && (target->AI.AirTarget == PlayerPlane))
				{
					if(AICheckPlayerBreak(planepnt, target, offangle, offpitch, tdist, toffangle))
					{
						if(!MultiPlayer)
						{
							planepnt->AI.lCombatTimer = 15000;
						}
						return;
					}
				}
			}
			if(fabs(toffangle) < 45)
			{
				AIDetermineMerge(planepnt, target, offangle, offpitch, tdist, toffangle);
				return;
			}
			else if(fabs(toffangle) < 90)
			{
			}
			else
			{
				if(fabs(toffangle) > 135.0f)
				{
					if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane) && (fabs(offpitch) < 60) && (tdist < (2.0f * NMTOWU)))
					{
						iLastWSOWarnType = TARGET_PLANE;
						pLastWSOWarnObj = planepnt;

						if(AICNoShotsAtPlayer(NULL))
						{
							PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
							lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
							randspch = rand() & 1;
//							AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_CONVERT_SIX + randspch, 1000, 50, AICF_ENEMY_CONVERT_SIX + randspch);
							if(lAdvisorFlags & ADV_CASUAL_BETTY)
							{
								AICGenericBettySpeech(BETTY_ENEMY_POSITION, 6, 0, 1);
							}
						}
#if 0  //  Not for F/A-18
						else
						{
							PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
							lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
							randspch = rand() % 3;
							switch(randspch)
							{
								case 0:
									AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_GET_ENEMY_OFF_US, 1000, 50, AICF_GET_ENEMY_OFF_US);
									break;
								case 1:
									AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_SHAKE_THIS_GUY, 1000, 50, AICF_SHAKE_THIS_GUY);
									break;
								default:
									AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_GET_GUY_OFF_ASS, 1000, 50, AICF_GET_GUY_OFF_ASS);
									break;
							}
						}
#endif

						//  Stuff to have wingman go and help Player (though might want to do this more)

						if(PlayerPlane->AI.wingman >= 0)
						{
							if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								wingman = &Planes[PlayerPlane->AI.wingman];
								wingman->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
								wingman->AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);

								btempvar = 0;
								if((wingman->AI.AirTarget != PlayerPlane->AI.AirThreat) && (!(wingman->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
								{
									GeneralClearCombatBehavior(wingman);
									btempvar = 1;
								}

								GeneralSetNewAirTarget(wingman, PlayerPlane->AI.AirThreat);
								wingman->AI.iAIFlags2 |= AIKEEPTARGET;
								if((MultiPlayer) && (wingman->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
								{
									if(wingman->AI.AirTarget)
									{
										targetnum = wingman->AI.AirTarget - Planes;
									}
									else
									{
										targetnum = 255;
									}

									NetPutGenericMessage3(wingman, GM3_WING_HELP, btempvar, targetnum);
								}
							}
						}
					}

					dvel = planepnt->IfHorzVelocity - target->IfHorzVelocity;

					//  Get seconds left to decelerate until reaching target
					ftempvar2 = sqrt((2*tdist*WUTOFT)/planepnt->MaxSpeedAccel);
					//  Add how more time it will take to get to where the target has moved during this time (approx)
	//				ftempvar2 += ((target->IfHorzVelocity * ftempvar2) / planepnt->IfHorzVelocity);

					//  Determine how much deceleration can take place.
					ftempvar2 *= planepnt->MaxSpeedAccel;

					ftempvar = planepnt->V - target->V;
					if((ftempvar > ftempvar2) && (tdist < ((dvel) * 5.0f)) && (((tdist / dvel) * 2.0f) < (dvel / (plane_data->iAcceleration * 2.0f))))
					{
						chance = 0.7f - (planepnt->AI.iSkill * 0.2);
						if(frand() < chance)
						{
							planepnt->AI.CombatPos.X = 0.0f;
							planepnt->AI.CombatPos.Y = 0.0f;

							planepnt->AI.CombatPos.Z = 90.0f;
							if(target->Roll > 0x8000)
							{
								planepnt->AI.CombatPos.Z *= -1.0f;
							}
							planepnt->AI.lCombatTimer = 25000;
							planepnt->AI.CombatBehavior = AIBarrelAvoid;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 1, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							return;
						}
					}
				}

				if(((planepnt->Pitch > 0xF800) || (planepnt->Pitch < 0x8000)) && (planepnt->IfHorzVelocity < target->IfHorzVelocity))
				{
					chance = 0.8f - (planepnt->AI.iSkill * 0.15);
					if(frand() < chance)
					{
						degroll = AIConvertAngleTo180Degree(planepnt->Roll);
						if((fabs(90.0f - fabs(degroll)) < 10.0f) && (fabs(AIConvertAngleTo180Degree(planepnt->Roll - target->Roll)) < 20.0f) && (planepnt->HeightAboveGround > (4000.0f * FTTOWU)))
						{
							yawrate = fabs(AIConvertAngleTo180Degree(planepnt->AI.LastHeading - planepnt->Heading));
							yawrate *= 1000.0f / (float)DeltaTicks;
							tyawrate = fabs(AIConvertAngleTo180Degree(target->AI.LastHeading - target->Heading));
							tyawrate *= 1000.0f / (float)DeltaTicks;
							if(yawrate < tyawrate)
							{
								workspeed = (plane_data->iStallSpeed * 2);
								cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 0.95f;
								if(planepnt->V < cornerspeed)
								{
									planepnt->AI.CombatBehavior = AILowYoYo;
									planepnt->AI.lCombatTimer = 5000;
									if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
									{
										NetPutDogfightUpdate(planepnt, 2, planepnt->AI.lCombatTimer);
									}
									return;
								}
							}
						}
					}
				}

				relvel = (cos(DegToRad(offangle)) * planepnt->IfHorzVelocity);
				fworkval = (cos(DegToRad(toffangle)) * target->IfHorzVelocity);
				if((fabs(toffangle) > 45) && (fabs(toffangle) < 135))
				{
					fworkval = -(target->IfHorzVelocity * 0.75f);
				}

				relvel += fworkval;

				chance = 0.8f - (planepnt->AI.iSkill * 0.15);
				if((tdist < (relvel * 3.0f)) && (fabs(toffangle) < 160) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity * 1.25f)))
				{
					chance = 0.8f - (planepnt->AI.iSkill * 0.15);
					if(frand() < chance)
					{
						workspeed = (plane_data->iStallSpeed * 2);
						cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25f;

						if((planepnt->V > cornerspeed) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity)) && ((tdist / relvel) < ((planepnt->IfHorzVelocity - target->IfHorzVelocity) / (plane_data->iAcceleration * 2.0f))) && ((tdist / relvel) < ((planepnt->V - cornerspeed) / (plane_data->iAcceleration * 2.0f))))
						{
							planepnt->AI.CombatBehavior = AIHighYoYo;
							planepnt->AI.lCombatTimer = 10000;
							planepnt->AI.CombatPos.X = 0;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 3, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							return;
						}
					}
				}
				if((tdist < (relvel * 7.0f)) && (tdist > (relvel * 2.0f)) && (fabs(toffangle) < 135) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity * 1.25f)))
				{
					chance = 0.8f - (planepnt->AI.iSkill * 0.15);
					if(frand() < chance)
					{
						workspeed = (plane_data->iStallSpeed * 2);
						cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25f;

						if((planepnt->V > cornerspeed) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity)))
						{
							AISetUpLagPursuitRoll(planepnt, target, offangle, offpitch, tdist, toffangle);
							return;
						}
					}
				}
			}
		}
		else if(fabs(offangle) < 90)
		{
			if(fabs(toffangle) < 45)
			{
			}
			else if(fabs(toffangle) < 90)
			{
			}
			else
			{
				if(((planepnt->Pitch > 0xF800) || (planepnt->Pitch < 0x8000)) && (planepnt->IfHorzVelocity < target->IfHorzVelocity))
				{
					chance = 0.8f - (planepnt->AI.iSkill * 0.15);
					if(frand() < chance)
					{
						degroll = AIConvertAngleTo180Degree(planepnt->Roll);
						if((fabs(90.0f - fabs(degroll)) < 10.0f) && (fabs(AIConvertAngleTo180Degree(planepnt->Roll - target->Roll)) < 20.0f) && (planepnt->HeightAboveGround > (4000.0f * FTTOWU)))
						{
							yawrate = fabs(AIConvertAngleTo180Degree(planepnt->AI.LastHeading - planepnt->Heading));
							yawrate *= 1000.0f / (float)DeltaTicks;
							tyawrate = fabs(AIConvertAngleTo180Degree(target->AI.LastHeading - target->Heading));
							tyawrate *= 1000.0f / (float)DeltaTicks;
							if(yawrate < tyawrate)
							{
								workspeed = (plane_data->iStallSpeed * 2);
								cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 0.95f;
								if(planepnt->V < cornerspeed)
								{
									planepnt->AI.CombatBehavior = AILowYoYo;
									planepnt->AI.lCombatTimer = 5000;
									if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
									{
										NetPutDogfightUpdate(planepnt, 2, planepnt->AI.lCombatTimer);
									}
									return;
								}
							}
						}
					}
				}
				relvel = (cos(DegToRad(offangle)) * planepnt->IfHorzVelocity);
				fworkval = (cos(DegToRad(toffangle)) * target->IfHorzVelocity);
				if((fabs(toffangle) > 45) && (fabs(toffangle) < 135))
				{
					fworkval = -(target->IfHorzVelocity * 0.75f);
				}

				relvel += fworkval;

				chance = 0.8f - (planepnt->AI.iSkill * 0.15);
				if((tdist < (relvel * 3.0f)) && (fabs(toffangle) < 160) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity * 1.25f)))
				{
					chance = 0.8f - (planepnt->AI.iSkill * 0.15);
					if(frand() < chance)
					{
						workspeed = (plane_data->iStallSpeed * 2);
						cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25f;

	//					if((planepnt->V > cornerspeed) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity)))
						if((planepnt->V > cornerspeed) && (planepnt->IfHorzVelocity > (target->IfHorzVelocity)) && ((tdist / relvel) < ((planepnt->IfHorzVelocity - target->IfHorzVelocity) / (plane_data->iAcceleration * 2.0f))) && ((tdist / relvel) < ((planepnt->V - cornerspeed) / (plane_data->iAcceleration * 2.0f))))
						{
							planepnt->AI.CombatBehavior = AIHighYoYo;
							planepnt->AI.lCombatTimer = 10000;
							planepnt->AI.CombatPos.X = 0;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 3, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							return;
						}
					}
				}
			}
		}
		else
		{
			if(fabs(toffangle) < 45)
			{
			}
			else
	//		else if(fabs(toffangle) < 90)
			{
				if(frand() < (0.40f - (planepnt->AI.iSkill * 0.05f)))
				{
					circletime = (360.0f / planepnt->YawRate);
					circlec = circletime * planepnt->V;
					radft = circlec / (2 * PI);

					if(radft > (tdist * WUTOFT * 0.5))
					{
						if(((target->Roll > 0x8000) && (toffangle > 0)) || ((target->Roll < 0x8000) && (toffangle < 0)))
						{
							planepnt->AI.CombatBehavior = AITurnAway;
							planepnt->AI.lCombatTimer = circletime * 1000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightUpdate(planepnt, 4, planepnt->AI.lCombatTimer);
							}
									if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
									{
										NetPutDogfightCPosUpdate(planepnt, 4, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
									}
							return;
						}
					}
				}
				if(fabs(toffangle) < 120)
				{
					if(frand() < (0.35f - (planepnt->AI.iSkill * 0.05f)))
					{
						workspeed = (plane_data->iStallSpeed * 2);
						cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
						if(planepnt->V < (cornerspeed * 0.70f))
						{
							planepnt->AI.CombatBehavior = AIExtend;
							planepnt->AI.lCombatTimer = 10000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightUpdate(planepnt, 5, planepnt->AI.lCombatTimer);
							}
							return;
						}
					}
				}
	//		}
	// PATCH		else
	//		{
				if(!((plane_data->lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))))
				{
	//	PATCH			if((fabs(offangle) > 160) && (fabs(toffangle) > 90) && ((planepnt->Pitch < 0x1800) || (planepnt->Pitch > 0xE800)))
					if((fabs(offangle) > 150) && (fabs(toffangle) > 45) && ((planepnt->Pitch < 0x1800) || (planepnt->Pitch > 0xE800)))
					{
		//				if(frand() < 1.0f)
						if(frand() < (0.25f - (planepnt->AI.iSkill * 0.05f)))
						{
							workspeed = (plane_data->iStallSpeed);
							cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
							if((cornerspeed * 3) < planepnt->V)
							{
		//						Camera1.AttachedObject = (int *)planepnt;
								planepnt->AI.CombatPos.X = 1;
								planepnt->AI.CombatPos.Y = 90.0f;
								planepnt->AI.CombatPos.Z = 1;
								planepnt->AI.CombatBehavior = AIImmelmann;
								planepnt->AI.lCombatTimer = 10000;
								if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
								{
									NetPutDogfightCPosUpdate(planepnt, 6, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
								}
								if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
								{
									PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
									lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
									iLastWSOWarnType = TARGET_PLANE;
									pLastWSOWarnObj = planepnt;
									if(lAdvisorFlags & ADV_CASUAL_BETTY)
									{
										AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
									}
//									AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 3000, 50, AICF_ENEMY_VERTICLE);
								}
								return;
							}
							else if((cornerspeed * 2) > planepnt->V)
							{
								if((planepnt->V * (180.0f / planepnt->MaxPitchRate)) < (planepnt->HeightAboveGround * WUTOFT))
								{
		//							Camera1.AttachedObject = (int *)planepnt;
									planepnt->AI.CombatPos.X = 1;
									planepnt->AI.CombatPos.Y = -85.0f;
									planepnt->AI.CombatPos.Z = 1;
									planepnt->AI.CombatBehavior = AISplitS;
									planepnt->AI.lCombatTimer = 10000;
									if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
									{
										NetPutDogfightCPosUpdate(planepnt, 7, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
									}
									if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
									{
										PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
										lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
										iLastWSOWarnType = TARGET_PLANE;
										pLastWSOWarnObj = planepnt;
										if(lAdvisorFlags & ADV_CASUAL_BETTY)
										{
											AICGenericBettySpeech(BETTY_ENEMY_U_D, 0);
										}
//										AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_WEEDS, 3000, 50, AICF_ENEMY_WEEDS);
									}
									return;
								}
								else if((planepnt->V * (180.0f / planepnt->MaxPitchRate) * 0.708f) < (planepnt->HeightAboveGround * WUTOFT))
								{
									if(offangle < 0)
									{
										planepnt->AI.CombatPos.X = 1;
									}
									else
									{
										planepnt->AI.CombatPos.X = -1;
									}
		//							Camera1.AttachedObject = (int *)planepnt;
									planepnt->AI.CombatPos.Y = -45.0f;
									planepnt->AI.CombatPos.Z = 1;
									planepnt->AI.CombatBehavior = AISplitS;
									planepnt->AI.lCombatTimer = 10000;
									if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
									{
										NetPutDogfightCPosUpdate(planepnt, 7, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
									}
									if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
									{
										PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
										lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
										iLastWSOWarnType = TARGET_PLANE;
										pLastWSOWarnObj = planepnt;
										if(lAdvisorFlags & ADV_CASUAL_BETTY)
										{
											AICGenericBettySpeech(BETTY_ENEMY_U_D, 0);
										}
//										AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_WEEDS, 3000, 50, AICF_ENEMY_WEEDS);
									}

									return;
								}
							}
							else
							{
								if((planepnt->V * (180.0f / planepnt->MaxPitchRate) * 0.6667f) < (planepnt->HeightAboveGround * WUTOFT))
								{
		//							Camera1.AttachedObject = (int *)planepnt;
									planepnt->AI.CombatPos.X = 1;
									planepnt->AI.CombatPos.Y = -85.0f;
									planepnt->AI.CombatPos.Z = 1;
									planepnt->AI.CombatBehavior = AICuban8;
									planepnt->AI.lCombatTimer = 10000;
									if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
									{
										NetPutDogfightCPosUpdate(planepnt, 8, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
									}
									if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
									{
										PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
										lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
										iLastWSOWarnType = TARGET_PLANE;
										pLastWSOWarnObj = planepnt;
										if(lAdvisorFlags & ADV_CASUAL_BETTY)
										{
											AICGenericBettySpeech(BETTY_ENEMY_U_D, 0);
										}
//										AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 1000, 50, AICF_ENEMY_VERTICLE);
									}
									return;
								}
							}
						}
					}
				}

				if(frand() < (0.40f - (planepnt->AI.iSkill * 0.05f)))
				{
					workspeed = (plane_data->iStallSpeed * 2);
					cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
					if(planepnt->V < (cornerspeed * 0.75f))
					{
						planepnt->AI.CombatBehavior = AIExtend;
						planepnt->AI.lCombatTimer = 15000;
						if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							NetPutDogfightUpdate(planepnt, 9, planepnt->AI.lCombatTimer);
						}
						if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
						{
//							PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
//							lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
							iLastWSOWarnType = TARGET_PLANE;
							pLastWSOWarnObj = planepnt;
//							AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_EXTENDING, 3000, 50, AICF_ENEMY_EXTENDING);
						}
						return;
					}
				}

				if((lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
				{
					if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (planepnt->Pitch > 0xF000) || (planepnt->Pitch < 0x1000))
					{
						PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
						lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
						iLastWSOWarnType = TARGET_PLANE;
						pLastWSOWarnObj = planepnt;

						if(offangle > 0)
						{
							AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_BREAKING_LEFT, 2000, 50, AICF_ENEMY_BREAKING_LEFT);
						}
						else
						{
							AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_BREAKING_RIGHT, 2000, 50, AICF_ENEMY_BREAKING_RIGHT);
						}
						if(!MultiPlayer)
						{
							planepnt->AI.CombatBehavior = NULL;
							planepnt->AI.lCombatTimer = 7000 + (planepnt->AI.iSkill * 1000);
						}
					}
				}

				if(PlayerPlane->AI.wingman == (planepnt - Planes))
				{
					if((PlayerPlane->AI.iAIFlags2 & AIDEFENSIVE) && (frand() < 0.25f))
					{
						wingman = &Planes[PlayerPlane->AI.wingman];
						wingman->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
						wingman->AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);

						btempvar = 0;
						if((wingman->AI.AirTarget != PlayerPlane->AI.AirThreat) && (!(wingman->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
						{
							GeneralClearCombatBehavior(wingman);
							btempvar = 1;
						}

						GeneralSetNewAirTarget(wingman, PlayerPlane->AI.AirThreat);
						wingman->AI.iAIFlags2 |= AIKEEPTARGET;
						if((MultiPlayer) && (wingman->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							if(wingman->AI.AirTarget)
							{
								targetnum = wingman->AI.AirTarget - Planes;
							}
							else
							{
								targetnum = 255;
							}

							NetPutGenericMessage3(wingman, GM3_WING_HELP, btempvar, targetnum);
						}
					}
				}

			}
		}
	}

	if((lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
	{
		if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (fabs(toffangle) > 90))
		{
			PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
			iLastWSOWarnType = TARGET_PLANE;
			pLastWSOWarnObj = planepnt;

			AICAddSoundCall(AIC_WSO_BanditCall, PlayerPlane - Planes, 1000, 50, planepnt - Planes);
		}
	}

	planepnt->AI.lCombatTimer = 2000 + (planepnt->AI.iSkill * 1000);
	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.CombatBehavior))
	{
		NetPutDogfightUpdate(planepnt, 0, planepnt->AI.lCombatTimer);
	}
	planepnt->AI.CombatBehavior = NULL;
}

//**************************************************************************************
void AIDetermineMerge(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float intercepttime;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	DBAircraftType *target_data = &pDBAircraftList[target->AI.iPlaneIndex];
	long workspeed;
	float cornerspeed;
	float circlec, radft;
	float targetmyaw, planemyaw;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		if(planepnt->AI.lCombatTimer < 0)
		{
			planepnt->AI.lCombatTimer = 2000 + (planepnt->AI.iSkill * 1000);
		}
		return;
	}

	workspeed = (plane_data->iStallSpeed * 2);
	cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
	intercepttime = (dist * WUTOFT) / (planepnt->IfHorzVelocity + target->IfHorzVelocity);  //  This isn't exact since this would assume planes heading at each other, but hey, should be close enough.
	targetmyaw = target_data->iMaxYawRate;
	planemyaw = plane_data->iMaxYawRate;

//	if((intercepttime < 5.0f))
//	if(!((plane_data->lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN)) || ((planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA))))
	if(!(plane_data->lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN)))
	{
		if(intercepttime <= 20.0f)
		{
			if((intercepttime > 15.0f) && ((planepnt->AI.iSkill <= GP_EXPERT) && (frand() < 0.20f)))
			{
				if(planemyaw > (targetmyaw * 0.75f))
				{
					circlec = (360.0f / planemyaw) * cornerspeed;
					radft = circlec / (2 * PI);
					planepnt->AI.CombatPos.X = radft * FTTOWU * 2;
				}
				else
				{
					circlec = (360.0f / targetmyaw) * target->V;
					radft = circlec / (2 * PI);
					planepnt->AI.CombatPos.X = radft * FTTOWU * 0.75f;
				}
				planepnt->AI.CombatPos.Z = 0.0f;

				if(planepnt->V > (cornerspeed * 1.5f))
				{
					planepnt->AI.CombatPos.Y = 20.0f;
				}
				else if(planepnt->V < (cornerspeed * 0.75))
				{
					planepnt->AI.CombatPos.Y = -20.0f;
				}
				else
				{
					planepnt->AI.CombatPos.Y = 0.0f;
				}

				planepnt->AI.CombatBehavior = AIPrepareLeadTurn;
				intercepttime *= 2000;
				planepnt->AI.lCombatTimer = intercepttime;
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutDogfightCPosUpdate(planepnt, 13, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
				}
				return;
			}
#if 0  //  This needs to move to more rear quarter tactics
			else if((intercepttime <= 15.0f) && (intercepttime > 10.0f) && ((planepnt->AI.iSkill <= GP_EXPERT) /* && (frand() < 0.25f)  */   ) && ((cornerspeed * 3) < planepnt->V) && (fabs(toffangle) >= 30.0f))
			{
				AISetUpLagRoll(planepnt, target, offangle, offpitch, dist, toffangle);
				return;
			}
#endif
			else if(intercepttime <= 10.0f)
			{
				if((planepnt->AI.iSkill == GP_NOVICE) || ((planepnt->AI.iSkill <= GP_EXPERT) && ((AIAAGunsOnly(planepnt) && (frand() < 0.25f)) || (frand() < 0.15f))))
				{
					planepnt->AI.CombatBehavior = AIJoust;
					intercepttime *= 2000;
					planepnt->AI.lCombatTimer = intercepttime;
					if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						NetPutDogfightUpdate(planepnt, 14, planepnt->AI.lCombatTimer);
					}
					return;
				}
			}
		}
	}
	if((intercepttime <= 5.0f) && (fabs(toffangle) < 3.0f))
	{
		if(dist < NMTOWU)
		{
			if((dist < (NMTOWU * 0.75f)) || (frand() < 0.5f))
			{
				planepnt->AI.CombatBehavior = AIJoust;
				intercepttime *= 2000;
				planepnt->AI.lCombatTimer = intercepttime;
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutDogfightUpdate(planepnt, 14, planepnt->AI.lCombatTimer);
				}
				return;
			}
		}

		if(planemyaw > (targetmyaw * 0.75f))
		{
			circlec = (360.0f / planemyaw) * cornerspeed;
			radft = circlec / (2 * PI);
			planepnt->AI.CombatPos.X = radft * FTTOWU * 2;
		}
		else
		{
			circlec = (360.0f / targetmyaw) * target->V;
			radft = circlec / (2 * PI);
			planepnt->AI.CombatPos.X = radft * FTTOWU * 0.75f;
		}
		planepnt->AI.CombatPos.Z = 0.0f;


		if(plane_data->lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
		{
			planepnt->AI.iAICombatFlags1 |= AI_BLOW_THROUGH;
		}
		else if((planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA) || (intercepttime > 5.0f))
		{
			planepnt->AI.iAICombatFlags1 |= AI_BLOW_THROUGH;
		}
		else
		{
			planepnt->AI.iAICombatFlags1 &= ~AI_BLOW_THROUGH;
		}

		if((planepnt->AI.iAICombatFlags1 & AI_BLOW_THROUGH) && (intercepttime <= 5.0f))
		{
			planepnt->AI.CombatPos.Y = (sin(DegToRad(-30.0f)) * dist);
		}
		else
		{
			planepnt->AI.CombatPos.Y = 0.0f;
		}

		planepnt->AI.CombatBehavior = AIGetSeperation;
		intercepttime *= 1000;
		planepnt->AI.lCombatTimer = intercepttime;
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			if(planepnt->AI.iAICombatFlags1 & AI_BLOW_THROUGH)
			{
				NetPutDogfightCPosUpdate(planepnt, 15, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
			}
			else
			{
				NetPutDogfightCPosUpdate(planepnt, 16, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
			}
		}
		return;
	}
	if(planepnt->AI.lCombatTimer < 0)
	{
		planepnt->AI.lCombatTimer = 2000 + (planepnt->AI.iSkill * 1000);
	}
}

//**************************************************************************************
void AITurnAway(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	planepnt->DesiredPitch = 0;

	if(!planepnt->Pitch)
	{
		planepnt->DesiredRoll = (toffangle < 0) ? 0x4000 : 0xC000;
	}
	else
	{
		planepnt->DesiredRoll = (toffangle < 0) ? 0x3000 : 0xD000;
	}

	if((fabs(toffangle) < 45.0f) || (fabs(offangle) < 90.0f))
	{
		GeneralClearCombatBehavior(planepnt);
	}
}

//**************************************************************************************
void AIGetSeperation(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	FPoint flypoint;
	ANGLE workheading;
	double heading, sinh, cosh;
	float sideval;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	long workspeed;

	if(planepnt->AI.AirTarget == NULL)
	{
		GeneralClearCombatBehavior(planepnt);
		return;
	}

//	if((fabs(offangle) > 60.0f) && (fabs(toffangle) < 30.0f))
	if(fabs(offangle) > 60.0f)
	{
		GeneralClearCombatBehavior(planepnt);
		AIAfterSeperation(planepnt, target, offangle, offpitch, dist, toffangle);
		return;
	}

	flypoint = target->WorldPosition;
	if(planepnt->AI.CombatPos.X)
	{
		workheading = target->Heading;
		if(toffangle < 0x8000)
		{
			sideval = 1.0f;
		}
		else
		{
			sideval = -1.0f;
		}

		heading = DegToRad((double)workheading / DEGREE);
		sinh = sin(heading);
		cosh = cos(heading);

		flypoint.X += (cosh * planepnt->AI.CombatPos.X * sideval) + (sinh * planepnt->AI.CombatPos.Z);
		flypoint.Z += -(sinh * planepnt->AI.CombatPos.X * sideval) + (cosh * planepnt->AI.CombatPos.Z);
	}
	flypoint.Y += planepnt->AI.CombatPos.Y;
	FlyToPoint(planepnt, flypoint);

	if(planepnt->AI.CombatPos.Y < 0)
	{
		if(planepnt->DesiredPitch < 0x8000)
		{
			planepnt->DesiredPitch = 0;
		}
		else if(planepnt->DesiredPitch < 0xE000)
		{
			planepnt->DesiredPitch = 0xE000;
		}
	}
	else if(planepnt->AI.CombatPos.Y > 0)
	{
		if(planepnt->DesiredPitch > 0x8000)
		{
			planepnt->DesiredPitch = 0;
		}
		else if(planepnt->DesiredPitch > 0x2000)
		{
			planepnt->DesiredPitch = 0x2000;
		}
	}

	if(planepnt->AI.iAICombatFlags1 & AI_BLOW_THROUGH)
	{
		MBAdjustAIThrust(planepnt, 600, 1);
	}
	else
	{
		workspeed = (plane_data->iStallSpeed * 2) + (plane_data->iStallSpeed>>2);
		MBAdjustAIThrust(planepnt, workspeed, 1);
	}
}

//**************************************************************************************
void AIAfterSeperation(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	long workspeed;
	float cornerspeed = -1.0f;
	float pitch;

	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	if(planepnt->AI.AirTarget == NULL)
	{
		GeneralClearCombatBehavior(planepnt);
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightUpdate(planepnt, 0, planepnt->AI.lCombatTimer);
		}
		return;
	}

	if(fabs(offangle - 10.0f) < fabs(toffangle))
	{
		return;
	}

	if(((toffangle < 0) && (target->Roll < 0x8000)) || ((toffangle > 0) && (target->Roll > 0x8000)))
	{
		return;
	}

	pitch = AIConvertAngleTo180Degree(planepnt->Pitch);

	if(fabs(pitch) > 10.0f)
	{
		return;
	}

	workspeed = (plane_data->iStallSpeed);
	cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);

	if(((cornerspeed * 3) < planepnt->V) && (frand() < (0.5f - (planepnt->AI.iSkill * 0.05f))))
	{
//		Camera1.AttachedObject = (int *)planepnt;
		planepnt->AI.CombatPos.X = 1;
		planepnt->AI.CombatPos.Y = 90.0f;
		planepnt->AI.CombatPos.Z = 1;
		planepnt->AI.CombatBehavior = AIImmelmann;
		planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;
		planepnt->AI.lCombatTimer = 10000;
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightCPosUpdate(planepnt, 11, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
		}
		if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
		{
			PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
			iLastWSOWarnType = TARGET_PLANE;
			pLastWSOWarnObj = planepnt;
			if(lAdvisorFlags & ADV_CASUAL_BETTY)
			{
				AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
			}
//			AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 3000, 50, AICF_ENEMY_VERTICLE);
		}
		return;
	}

	planepnt->AI.CombatPos.Y = pitch;

	if(offangle < 0)
	{
		planepnt->AI.CombatPos.X = 90.0f;
	}
	else
	{
		planepnt->AI.CombatPos.X = -90.0f;
	}

	if(fabs(toffangle) < 90.0f)
	{
		planepnt->AI.CombatPos.X *= -1;
	}

	planepnt->AI.lCombatTimer = (120000.0f / (float)planepnt->YawRate);
	planepnt->AI.CombatBehavior = AIPitchAndTurn;
	planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;
	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutDogfightCPosUpdate(planepnt, 12, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
	}
	return;
}

//**************************************************************************************
int AIAAGunsOnly(PlaneParams *planepnt)
{
	int cnt;
	int indexnum;
	int weapontype;
	int gunexists = 0;

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		indexnum = planepnt->WeapLoad[cnt].WeapIndex;
		weapontype = pDBWeaponList[indexnum].iWeaponType;
		if((weapontype == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AA_LONG_RANGE_MISSILE))
		{
			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				return(0);
			}
		}
		if(weapontype == WEAPON_TYPE_GUN)
		{
			gunexists = 1;
		}
	}
	if(!gunexists)
	{
		planepnt->AI.iAICombatFlags1 |= AI_WINCHESTER_AA;
		return(0);
	}
	return(1);
}

//**************************************************************************************
void AIJoust(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float intercepttime;

	if(!AICheckForGuns(planepnt, target, offangle, offpitch, dist))
	{
		AI_ACM(planepnt, target, offangle, offpitch, dist, toffangle);
	}

	MBAdjustAIThrust(planepnt, 600, 1);

	if(fabs(offangle) > 45.0f)
	{
		GeneralClearCombatBehavior(planepnt);
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightUpdate(planepnt, 0, planepnt->AI.lCombatTimer);
		}
		return;
	}

	if(fabs(toffangle) > 45.0f)  //  Don't duplicate effort.  Set things up so that AI can access tactics logic next frame.
	{
		GeneralClearCombatBehavior(planepnt);
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightUpdate(planepnt, 0, planepnt->AI.lCombatTimer);
		}
		return;
	}

	intercepttime = (dist * WUTOFT) / (planepnt->IfHorzVelocity + target->IfHorzVelocity);  //  This isn't exact since this would assume planes heading at each other, but hey, should be close enough.

	if(intercepttime < 1.2f)
	{
		planepnt->AI.CombatPos.Y = 0.0f;
		planepnt->AI.CombatBehavior = AIGetSeperation;
		intercepttime *= 1000;
		planepnt->AI.lCombatTimer = intercepttime;

		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			if(planepnt->AI.iAICombatFlags1 & AI_BLOW_THROUGH)
			{
				NetPutDogfightCPosUpdate(planepnt, 15, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
			}
			else
			{
				NetPutDogfightCPosUpdate(planepnt, 16, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
			}
		}
	}
	else if((intercepttime < 3.0f) && (dist < (0.75f * NMTOWU)))
	{
		long workspeed;
		float cornerspeed = -1.0f;
		DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];

		workspeed = (plane_data->iStallSpeed * 2);
		cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25;

		if(planepnt->V > cornerspeed)
		{
			if(planepnt->AI.iSkill == GP_NOVICE)
			{
				planepnt->AI.CombatPos.X = frand() * 90.0f;
				if(toffangle > 0.0f)
				{
					planepnt->AI.CombatPos.X *= -1.0f;
					planepnt->AI.CombatPos.Y = cos(DegToRad(planepnt->AI.CombatPos.X)) * 30.0f;
					planepnt->AI.lCombatTimer = 4000;
					planepnt->AI.CombatBehavior = AIPitchAndTurn;
					if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						NetPutDogfightCPosUpdate(planepnt, 17, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
					}
				}
			}
			else
			{
				if(frand() < 0.50f)
				{
					planepnt->AI.CombatPos.X = (frand() * 30.0f) + 60.0f;
					if(toffangle > 0.0f)
					{
						planepnt->AI.CombatPos.X *= -1.0f;
					}
					planepnt->AI.CombatPos.Y = cos(DegToRad(planepnt->AI.CombatPos.X)) * 30.0f;
				}
				else
				{
					planepnt->AI.CombatPos.X = 0.0f;
					planepnt->AI.CombatPos.Y = 0.0f;
				}

				planepnt->AI.CombatPos.Z = 90.0f;
				if(toffangle > 0.0f)
				{
					planepnt->AI.CombatPos.Z *= -1.0f;
				}
				planepnt->AI.lCombatTimer = 25000;
				planepnt->AI.CombatBehavior = AIBarrelAvoid;
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutDogfightCPosUpdate(planepnt, 1, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
				}
			}
		}
	}
}

//**************************************************************************************
void AIBarrelAvoid(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	ANGLE adiffangle, tempangle;
	float diffangle;
	float degroll;
	long workspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];

	if((fabs(offangle) < 90.0f) && (fabs(toffangle) > 90.0f))
	{
		planepnt->DesiredSpeed = target->V;
	}
	else if((fabs(offangle) > 90.0f) && (fabs(toffangle) < 90.0f))
	{
		workspeed = plane_data->iStallSpeed + (plane_data->iStallSpeed>>1);
		MBAdjustAIThrust(planepnt, workspeed, 1);
	}
	else
	{
		workspeed = (plane_data->iStallSpeed * 2) + (plane_data->iStallSpeed>>2);
		MBAdjustAIThrust(planepnt, workspeed, 1);
	}

	if(planepnt->AI.CombatPos.X)
	{
		planepnt->DesiredRoll = tempangle = AIConvert180DegreeToAngle(-planepnt->AI.CombatPos.X);
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.Y);

		adiffangle = tempangle + planepnt->Roll;
		diffangle = AIConvertAngleTo180Degree(adiffangle);
		if(fabs(diffangle) < 1.0f)
		{
			if(planepnt->AI.CombatPos.X > 0)
			{
				planepnt->AI.CombatPos.Z = 90.0f;
			}
			else
			{
				planepnt->AI.CombatPos.Z = -90.0f;
			}

			planepnt->AI.CombatPos.X = 0.0f;
			planepnt->AI.CombatPos.Y = 0.0f;
		}
		return;
	}

	planepnt->DesiredRoll = tempangle = AIConvert180DegreeToAngle(-planepnt->AI.CombatPos.Z);

	adiffangle = tempangle + planepnt->Roll;
	diffangle = AIConvertAngleTo180Degree(adiffangle);
	if(fabs(diffangle) < 1.0f)
	{
		if(fabs(planepnt->AI.CombatPos.Z) == 90.0f)
		{
			planepnt->AI.CombatPos.Z = (planepnt->AI.CombatPos.Z < 0) ? 120.0f : -120.0f;
		}
		else if(planepnt->AI.iAIFlags2 & AIDEFENSIVE)
		{
			GeneralClearCombatBehavior(planepnt);
		}
		else if((fabs(offangle) < 90.0f) && (fabs(toffangle) > 90.0f))
		{
			GeneralClearCombatBehavior(planepnt);
			if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
			{
//				PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
//				lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
				iLastWSOWarnType = TARGET_PLANE;
				pLastWSOWarnObj = planepnt;
//				AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_TURNING_BACK_TO_US, 1000, 50, AICF_ENEMY_TURNING_BACK_TO_US);
			}

		}
		else if(planepnt->AI.lCombatTimer > 3000)
		{
			planepnt->AI.lCombatTimer = 3000;
		}
	}

	if(fabs(planepnt->AI.CombatPos.Z) <= 90.0f)
	{
		planepnt->DesiredPitch = 0x2000;
		degroll = AIConvertAngleTo180Degree(planepnt->Roll);
		if(planepnt->AI.CombatPos.Z < 0)
		{
			if((degroll > -45.0f) && (degroll < 0))
			{
				planepnt->RollPercentage *= 0.25;
			}
		}
		else
		{
			if((degroll < 45.0f) && (degroll > 0))
			{
				planepnt->RollPercentage *= 0.25;
			}
		}

	}
	else
	{
		if(planepnt->HeightAboveGround > (5000.0f * FTTOWU))
		{
			planepnt->DesiredPitch = 0xE000;
		}
		else if(planepnt->HeightAboveGround > (3000.0f * FTTOWU))
		{
			planepnt->DesiredPitch = 0xF000;
		}
		else
		{
			planepnt->DesiredPitch = 0;
		}
	}
}

//**************************************************************************************
void AIPitchAndTurn(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	planepnt->DesiredRoll = AIConvert180DegreeToAngle(-planepnt->AI.CombatPos.X);
	planepnt->DesiredPitch = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.Y);
}

//**************************************************************************************
void AIPrepareLeadTurn(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float intercepttime;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	DBAircraftType *target_data = &pDBAircraftList[target->AI.iPlaneIndex];
	long workspeed;
	long desiredspeed;
	float cornerspeed;
	float circlec, radft;
	float targetmyaw, planemyaw;
	float extraturndist;
	ANGLE workangle;

	if(fabs(offangle) > 45.0f)
	{
		GeneralClearCombatBehavior(planepnt);
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightUpdate(planepnt, 0, planepnt->AI.lCombatTimer);
		}
		return;
	}

	if(fabs(toffangle) > 45.0f)  //  Don't duplicate effort.  Set things up so that AI can access tactics logic next frame.
	{
		GeneralClearCombatBehavior(planepnt);
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightUpdate(planepnt, 0, planepnt->AI.lCombatTimer);
		}
		return;
	}

	AIGetSeperation(planepnt, target, offangle, offpitch, dist, toffangle);

	workspeed = (plane_data->iStallSpeed * 2);
	desiredspeed = workspeed + plane_data->iStallSpeed;
	MBAdjustAIThrust(planepnt, desiredspeed, 1);
	cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
	intercepttime = (dist * WUTOFT) / (planepnt->IfHorzVelocity + target->IfHorzVelocity);  //  This isn't exact since this would assume planes heading at each other, but hey, should be close enough.
	targetmyaw = target_data->iMaxYawRate;
	planemyaw = plane_data->iMaxYawRate;

	if(planemyaw > (targetmyaw * 0.75f))
	{
		circlec = (360.0f / planemyaw) * planepnt->V;
		radft = circlec / (2 * PI);
	}
	else
	{
		circlec = (360.0f / targetmyaw) * target->V;
		radft = circlec / (2 * PI);
	}

	if(offangle < 0)
	{
		workangle = 0xC000 - planepnt->Roll;
	}
	else
	{
		workangle = 0x4000 - planepnt->Roll;
	}

	extraturndist = fabs(AIConvertAngleTo180Degree(workangle)) / planepnt->MaxRollRate;
	extraturndist += fabs(offangle) / planepnt->YawRate;
	extraturndist *= (planepnt->IfHorzVelocity + target->IfHorzVelocity) * 0.4f;

//	if((dist - extraturndist) < (radft * (2.0f * FTTOWU)))
	if(((dist - extraturndist) < (radft * (FTTOWU))) && ((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		planepnt->AI.lCombatTimer = 20000;
		planepnt->AI.CombatBehavior = AIDoLeadTurn;
		if(offangle < 0)
		{
//  I have no idea what the heck this was about			planepnt->AI.CombatPos.X = AIConvertAngleTo180Degree(planepnt->Heading - 0x6000);
//  New stuff below
			planepnt->AI.CombatPos.X = - 70.0f - (frand() * 40.0f);
			if((planepnt->HeightAboveGround < (2000.0f * FTTOWU)) && (planepnt->AI.CombatPos.X < -90.0f))
			{
				planepnt->AI.CombatPos.X = -180.0f - planepnt->AI.CombatPos.X;
			}
		}
		else
		{
//  I have no idea what the heck this was about			planepnt->AI.CombatPos.X = AIConvertAngleTo180Degree(planepnt->Heading + 0x6000);
//  New stuff below
			planepnt->AI.CombatPos.X = 70.0f + (frand() * 40.0f);
			if((planepnt->HeightAboveGround < (2000.0f * FTTOWU)) && (planepnt->AI.CombatPos.X < -90.0f))
			{
				planepnt->AI.CombatPos.X = 180.0f - planepnt->AI.CombatPos.X;
			}
		}
		planepnt->AI.CombatPos.Z = -1.0f;
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutDogfightCPosUpdate(planepnt, 18, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
		}
	}
	else if((dist - extraturndist) < (radft * (3.0f * FTTOWU)))
	{
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.Y);
	}
	else
	{
		MBAdjustAIThrust(planepnt, 600, 1);
	}
}

//**************************************************************************************
void AIDoLeadTurn(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	ANGLE workheading, workroll;
	float fworkheading;  //  , dy;
	float degroll, temproll;

	if(planepnt->AI.CombatPos.Z < 0)
	{
		workheading = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.X) - planepnt->Heading;
		planepnt->DesiredPitch = 0;
	}
	else
	{
		AIHighYoYo2(planepnt, target, offangle, offpitch, dist, toffangle);
		return;
//		workheading = planepnt->AI.AirTarget->Heading - planepnt->Heading;
//		dy = AICheckSafeAlt(planepnt, planepnt->AI.AirTarget->WorldPosition.Y) - planepnt->WorldPosition.Y;
//		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	}

	fworkheading = AIConvertAngleTo180Degree(workheading);

	if(fabs(fworkheading) < 2.0f)
	{
		if(planepnt->AI.CombatPos.Z)
		{
			planepnt->AI.CombatPos.Z = 0;
		}
		else
		{
			GeneralClearCombatBehavior(planepnt);
			return;
		}
	}
	else if((fabs(offangle) < 2.0f) && (planepnt->AI.CombatPos.Z >= 0))
	{
		GeneralClearCombatBehavior(planepnt);
		return;
	}
	else if((fabs(offangle) < 45.0f) && (fabs(toffangle) > 120.0f))
	{
		GeneralClearCombatBehavior(planepnt);
		return;
	}

	if(planepnt->Pitch == 0)
	{
		planepnt->DesiredRoll = (fworkheading < 0) ? 0x4000 : 0xC000;
	}
	else if(planepnt->AI.CombatPos.Z)
	{
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(-planepnt->AI.CombatPos.X);
	}
	else if(planepnt->Pitch > 0x8000)
	{
		planepnt->DesiredRoll = (fworkheading < 0) ? 0x4800 : 0xB800;
	}
	else
	{
		planepnt->DesiredRoll = (fworkheading < 0) ? 0x3800 : 0xC800;
	}

	degroll = AIConvertAngleTo180Degree(planepnt->Roll);
	workroll = planepnt->DesiredRoll + planepnt->Roll;
	temproll = AIConvertAngleTo180Degree(workroll);
//	if((fabs(temproll) < 10.0f) && ((fabs(90.0f - fabs(degroll)) > 10.0f) || (fabs(degpitch) > 45.0f)))
	if(fabs(temproll) < 10.0f)
	{
		if(fabs(degroll) <= 90)
		{
			planepnt->DesiredPitch += 0x4000;
		}
		else
		{
			planepnt->DesiredPitch -= 0x4000;
		}

		planepnt->RollYawCouple = -1;
		planepnt->MaxPitchRate = planepnt->YawRate;
		if((planepnt == (PlaneParams *)Camera1.AttachedObject) || (PlayerPlane == (PlaneParams *)Camera1.AttachedObject))
		{
			lvcolor = 63;
		}
	}
	else
	{
		planepnt->RollYawCouple = 2;
	}
}

//**************************************************************************************
void AISetUpLagPursuitRoll(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	AIComputeInterceptPoint(target, planepnt->WorldPosition, &planepnt->AI.CombatPos, planepnt->V);

	planepnt->AI.CombatBehavior = AIStartLagPursuitRoll;
	planepnt->AI.lCombatTimer = 20000;
	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutDogfightCPosUpdate(planepnt, 10, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
	}
	return;
}

//**************************************************************************************
void AIStartLagPursuitRoll(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	double dyaw;
	float tdist;
	float dx, dy, dz;
	long workspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];

	workspeed = (plane_data->iStallSpeed * 2) + (plane_data->iStallSpeed>>2);
	MBAdjustAIThrust(planepnt, workspeed, 1);

	dyaw = AIComputeHeadingToPoint(planepnt, planepnt->AI.CombatPos, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDirPitch(planepnt, (double)dyaw, 0x1555);

	if(((fabs(dyaw) < 2.0f) && (fabs(AIConvertAngleTo180Degree(planepnt->DesiredPitch - planepnt->Pitch)) < 2.0f)) || (fabs(dyaw) > 90.0f))
	{
//		planepnt->AI.CombatBehavior = AIFinishLagPursuitRoll;
		planepnt->AI.CombatBehavior = AIHighYoYo2;
		planepnt->AI.lCombatTimer = 15000;
	}

	return;
}

//**************************************************************************************
void AIFinishLagPursuitRoll(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float temproll, degroll;
	long workspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];

	workspeed = (plane_data->iStallSpeed * 2) + (plane_data->iStallSpeed>>2);
	MBAdjustAIThrust(planepnt, workspeed, 1);

	degroll = AIConvertAngleTo180Degree(planepnt->Roll);

	PLGetRelativeHPR(planepnt, target->WorldPosition, NULL, NULL, &temproll);

	degroll += temproll;

	planepnt->DesiredRoll = AIConvert180DegreeToAngle(-degroll);

	planepnt->RollYawCouple = 0;

	if((fabs(offangle) > 90.0f) || (fabs(toffangle) > 90.0f))
	{
		GeneralClearCombatBehavior(planepnt);
	}

	return;
}

//************************************************************************************************
void AIComputeInterceptPoint(PlaneParams *planepnt, FPointDouble attackerposition, FPoint *aimpoint, float attackervel, float timemod)
{
	float disttravel;
	float fworkvar;
	float time;
	ANGLE workpitch;

	time = GDGetTimeBulletIntercept(planepnt, attackerposition, attackervel);

	time += timemod;

	disttravel = planepnt->V * time;

	workpitch = AIGetPlanesFlightPitch(planepnt, 1);

	fworkvar = (sin(DegToRad((double)workpitch / DEGREE)) * disttravel);
	aimpoint->Y = planepnt->WorldPosition.Y + (fworkvar / WUTOFT);

	disttravel = (cos(DegToRad((double)workpitch / DEGREE)) * disttravel);

	fworkvar = (sin(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	aimpoint->X = planepnt->WorldPosition.X - (fworkvar / WUTOFT);

	fworkvar = (cos(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	aimpoint->Z = planepnt->WorldPosition.Z - (fworkvar / WUTOFT);
}

//**************************************************************************************
void AILowYoYo(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	long workspeed;
	float cornerspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	float temproll;

	workspeed = (plane_data->iStallSpeed * 2);
	planepnt->DesiredSpeed = cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25f;

	if(((fabs(offangle) < 15.0f) && (fabs(toffangle) > 135.0f)) || (planepnt->HeightAboveGround < (2000.0f * FTTOWU)) || ((planepnt->Pitch < 0xE000) && (planepnt->Pitch > 0x8000)))
	{
		GeneralClearCombatBehavior(planepnt);
		AI_ACM(planepnt, target, offangle, offpitch, dist, toffangle);
		return;
	}

	planepnt->DesiredRoll = (offangle > 0) ? 0xB000 : 0x5000;
	planepnt->DesiredPitch = 0xD000;

	temproll = -AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll);

	if(fabs(temproll) < 10.0f)
	{
		planepnt->RollYawCouple = -1;
		planepnt->MaxPitchRate = planepnt->YawRate;
	}
}

//**************************************************************************************
void AIHighYoYo(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	double dyaw;
	float tdist;
	float dx, dy, dz;
	long workspeed;
	float cornerspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	float degroll;

	workspeed = (plane_data->iStallSpeed * 2);
	planepnt->DesiredSpeed = cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);

	dyaw = AIComputeHeadingToPoint(planepnt, target->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDirPitch(planepnt, (double)dyaw, 0x2000);

	if(((planepnt->Pitch > 0x1E00) && (planepnt->Pitch < 0x8000)) || ((cornerspeed >= planepnt->V) && (planepnt->V < target->V)))
	{
		planepnt->AI.CombatBehavior = AIHighYoYo2;
		planepnt->AI.lCombatTimer = 180000 / planepnt->MaxRollRate;
		return;
	}

	degroll = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	planepnt->DesiredRoll = AIConvert180DegreeToAngle(degroll / 2);

}

//**************************************************************************************
void AIHighYoYo2(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	long workspeed;
	float cornerspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	float temproll, degroll, temppitch, degpitch;
	float orgdegroll;

	workspeed = (plane_data->iStallSpeed * 2);
	planepnt->DesiredSpeed = cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25f;

	PLGetRelativeHPR(planepnt, target->WorldPosition, NULL, &temppitch, &temproll);

	orgdegroll = degroll = AIConvertAngleTo180Degree(planepnt->Roll);
	if((planepnt->Pitch > 0x8000) || (offpitch >= 0) || ((fabs(temproll) < 5.0f) && (fabs(degroll) < 90.0f)))
	{
		GeneralClearCombatBehavior(planepnt);
		AI_ACM(planepnt, target, offangle, offpitch, dist, toffangle);
		return;
	}

#if 0
	degroll = AIConvertAngleTo180Degree(planepnt->Roll);

	degroll += temproll;

	planepnt->DesiredRoll = AIConvert180DegreeToAngle(-degroll);

	planepnt->DesiredPitch = 0xE000;
#else
	orgdegroll = degroll = AIConvertAngleTo180Degree(planepnt->Roll);
	degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

	degroll += temproll;

	planepnt->DesiredRoll = AIConvert180DegreeToAngle(-degroll);

	if(fabs(temproll) < 10.0f)
	{
		if(fabs(orgdegroll) <= 90)
		{
			degpitch += temppitch;
		}
		else
		{
			degpitch -= temppitch;
		}

		planepnt->RollYawCouple = -1;
		planepnt->MaxPitchRate = planepnt->YawRate;
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(degpitch);
	}
	else
	{
		degpitch += cos(DegToRad(orgdegroll)) * temppitch;  //  Use this if not using pure lift vector (ie. MaxPitchRate = YawRate)
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(degpitch);
	}
#endif
}

//**************************************************************************************
void AICheckForLiftVectorUsage(PlaneParams *planepnt, PlaneParams *target, FPointDouble targetposition, double offangle, double toffpitch, double yawval)
{
	float dpitch, degroll, degpitch, orgdegroll;
	float temproll;
	float temppitch;
	float yawperc, rollyaw;
	float tdist;

	if((yawval > -9999.0f) && (fabs(yawval) < (planepnt->YawRate / 2)))
	{
		dpitch = AIConvertAngleTo180Degree(planepnt->DesiredPitch - planepnt->Pitch);

		if(dpitch < 0)
		{
			degroll = AIConvertAngleTo180Degree(planepnt->Roll);
			if((-dpitch > ((planepnt->MaxPitchRate * 2.0f) / 3)) || ((fabs(degroll) > 90.0f) && (toffpitch < (-planepnt->MaxPitchRate/4))))
			{
				planepnt->DesiredRoll = 0x8000 - planepnt->DesiredRoll;
			}
		}
		if((planepnt == (PlaneParams *)Camera1.AttachedObject) || (PlayerPlane == (PlaneParams *)Camera1.AttachedObject))
		{
			lvcolor = 3;
		}
		return;
	}

	tdist = QuickDistance((targetposition.X - planepnt->WorldPosition.X) * WUTONM, (targetposition.Z - planepnt->WorldPosition.Z) * WUTONM);
//	if((fabs(offangle) < (planepnt->YawRate / 2)) && (yawval <= -9999.0f) && (((planepnt->Roll < 0x4000) || (planepnt->Roll > 0xC000)) || (fabs(toffpitch) < 5.0)))
	if(((fabs(offangle) < (planepnt->YawRate)) || (tdist > 5.0f)) && (yawval <= -9999.0f) && (((planepnt->Roll < 0x4000) || (planepnt->Roll > 0xC000)) || (fabs(toffpitch) < 5.0)) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
				|| ((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (planepnt->Knots < 150.0f)))
	{
	// 	AIChangeDir(planepnt, offangle, (dy * WUTOFT));
		AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));

		dpitch = AIConvertAngleTo180Degree(planepnt->DesiredPitch - planepnt->Pitch);

		if((dpitch < 0) && (!((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
				|| ((pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (planepnt->Knots < 150.0f)))))
		{
			degroll = AIConvertAngleTo180Degree(planepnt->Roll);
			if((-dpitch > ((planepnt->MaxPitchRate * 2.0f) / 3)) || (fabs(degroll) > 90.0f))
			{
				planepnt->DesiredRoll = 0x8000 - planepnt->DesiredRoll;
			}
		}
		if((planepnt == (PlaneParams *)Camera1.AttachedObject) || (PlayerPlane == (PlaneParams *)Camera1.AttachedObject))
		{
			lvcolor = 6;
		}
	}
	else
	{
		orgdegroll = degroll = AIConvertAngleTo180Degree(planepnt->Roll);
		degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

		PLGetRelativeHPR(planepnt, targetposition, NULL, &temppitch, &temproll);

		degroll += temproll;

		planepnt->DesiredRoll = AIConvert180DegreeToAngle(-degroll);

		//  If our lift vector is almost in the direction of target then use lift vector setup
		//  UNLESS we are near a 90 degree roll. I'm having a problem with having to switch
		//  the DesiredPitch sign at 90.

		//	Might go back and modify sflight.cpp so that if RollYawCouple is == -1 then DeltaPitch is
		//  Always a positive value.

//		if((fabs(temproll) < 10.0f) && (fabs(90.0f - fabs(degroll)) > 5.0f))
//		if(fabs(temproll) < 10.0f)
		if((fabs(temproll) < 10.0f) && ((fabs(90.0f - fabs(degroll)) > 10.0f) || (fabs(degpitch) > 45.0f)))
		{
			if(fabs(orgdegroll) <= 90)
			{
				degpitch += temppitch;
			}
			else
			{
				degpitch -= temppitch;
			}

			planepnt->RollYawCouple = -1;
			planepnt->MaxPitchRate = planepnt->YawRate;
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(degpitch);
			if((planepnt == (PlaneParams *)Camera1.AttachedObject) || (PlayerPlane == (PlaneParams *)Camera1.AttachedObject))
			{
				lvcolor = 63;
			}
		}
		else
		{
			degpitch += cos(DegToRad(orgdegroll)) * temppitch;  //  Use this if not using pure lift vector (ie. MaxPitchRate = YawRate)
			planepnt->DesiredPitch = AIConvert180DegreeToAngle(degpitch);
			planepnt->RollYawCouple = 2;
			if((planepnt == (PlaneParams *)Camera1.AttachedObject) || (PlayerPlane == (PlaneParams *)Camera1.AttachedObject))
			{
				lvcolor = 207;
			}
		}

		if(yawval > -9999.0f)
		{
			rollyaw = (planepnt->RollYawCouple > 0) ? (sin(DegToRad(planepnt->DesiredRoll*(1.0f/(float)DEGREE)))*(planepnt->YawRate)) : 0.0f;
			yawperc = fabs(yawval) / fabs(rollyaw);
			if(yawperc < 1.0f)
			{
				planepnt->YawRate *= yawperc;
			}
		}
	}
}

//**************************************************************************************
void AIImmelmann(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float pitchpercent;
	float degpitch;
	float desiredroll;
	int workspeed;
	float stallspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	double dground;

	MBAdjustAIThrust(planepnt, 600, 1);

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if(dground < 2000)
	{
		planepnt->AI.iAIFlags2 |= AIGROUNDAVOID;
	}

	planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;

	planepnt->MaxPitchRate = planepnt->YawRate;

	workspeed = (plane_data->iStallSpeed);
	stallspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);

	if(planepnt->AI.CombatPos.Z == 1)
	{

		if((stallspeed * 1.25f) > planepnt->V)
		{
			planepnt->DesiredRoll = 0x8000;
			planepnt->DesiredPitch = 0xE000;
			GeneralClearCombatBehavior(planepnt);
			planepnt->AI.CombatPos.Y = 0;
			return;
		}

		degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

		//  Determine how much pitching is left to go.
		pitchpercent = (planepnt->AI.CombatPos.Y - degpitch) / planepnt->AI.CombatPos.Y;

		//  desired roll set so that it is 90 degress at max roll, and (90.0f - max desired pitch) at 0 pitch (ie 80 degree roll is max pitch desired is 10 degrees).
		desiredroll = (90.0f - (planepnt->AI.CombatPos.Y * pitchpercent)) ;

		if(desiredroll < 0.0f) //  Negative pitch land
		{
			desiredroll = 0.0f;
		}
		else if(desiredroll >= 90.0f)  // Over max desired pitch, start back down.
		{
			planepnt->AI.CombatPos.Y = degpitch;
			desiredroll = 90.0f;
		}

		if(planepnt->AI.CombatPos.X < 0)
		{
			desiredroll = -desiredroll;
		}
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(-desiredroll);

		if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll)) < 1)
		{
			planepnt->AI.CombatPos.Z = 2.0f;
		}
		planepnt->AI.lCombatTimer = 10000;
	}
	else
	{
		planepnt->RollYawCouple = -2;
		if(planepnt->AI.CombatPos.Z < 0)  //  On our way down.
		{
			planepnt->DesiredPitch = 0xE000;
			if(planepnt->Pitch > 0x8000)  //  we are done
			{
				planepnt->DesiredRoll = 0;
				GeneralClearCombatBehavior(planepnt);
				planepnt->AI.CombatPos.Y = 0;
			}
			else
			{
				planepnt->AI.lCombatTimer = 10000;
			}
		}
		else
		{
			planepnt->AI.lCombatTimer = 10000;
			if((planepnt->Roll > 0x4000) && (planepnt->Roll < 0xC000))
			{
				planepnt->DesiredPitch = 0xE000;
				planepnt->AI.CombatPos.Z = -1;
			}
			else
			{
//				planepnt->DesiredPitch = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.Y);
//				planepnt->DesiredPitch = 0x6000;
				planepnt->DesiredPitch = planepnt->Pitch + 0x800;
			}
		}
	}
}

//**************************************************************************************
void AISplitS(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float pitchpercent;
	float degpitch;
	float desiredroll;
	long workspeed;
	float cornerspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	double dground;

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if(dground < 2000)
	{
		planepnt->AI.iAIFlags2 |= AIGROUNDAVOID;
	}

	planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;

	planepnt->MaxPitchRate = planepnt->YawRate;

	workspeed = (plane_data->iStallSpeed * 2);
	planepnt->DesiredSpeed = cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);

	if(planepnt->AI.CombatPos.Z == 1)
	{
		degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

		//  Determine how much pitching is left to go.
		pitchpercent = (planepnt->AI.CombatPos.Y - degpitch) / planepnt->AI.CombatPos.Y;

		//  desired roll set so that it is 90 degress at max roll, and (90.0f - max desired pitch) at 0 pitch (ie 80 degree roll is max pitch desired is 10 degrees).
		desiredroll = (90.0f - (planepnt->AI.CombatPos.Y * pitchpercent)) ;

		if(desiredroll > 180.0f) //  Negative pitch land
		{
			desiredroll = 180.0f;
		}
		else if(desiredroll <= 90.0f)  // Over max desired pitch, start back down.
		{
			planepnt->AI.CombatPos.Y = degpitch;
			desiredroll = 90.0f;
		}

		if(planepnt->AI.CombatPos.X < 0)
		{
			desiredroll = -desiredroll;
		}
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(-desiredroll);

		if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll)) < 1)
		{
			planepnt->AI.CombatPos.Z = 2.0f;
		}
		planepnt->AI.lCombatTimer = 10000;
	}
	else
	{
		planepnt->RollYawCouple = -2;
		if(planepnt->AI.CombatPos.Z < 0)  //  On our way down.
		{
			planepnt->DesiredPitch = 0x2000;
			if(planepnt->Pitch < 0x8000)  //  we are done
			{
				planepnt->DesiredRoll = 0;
				planepnt->AI.CombatPos.Y = 0;
				GeneralClearCombatBehavior(planepnt);
			}
			else
			{
				planepnt->AI.lCombatTimer = 10000;
			}
		}
		else
		{
			planepnt->AI.lCombatTimer = 10000;
			if((planepnt->Roll < 0x4000) || (planepnt->Roll > 0xC000))
			{
				planepnt->DesiredPitch = 0x2000;
				planepnt->AI.CombatPos.Z = -1;
			}
			else
			{
//				planepnt->DesiredPitch = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.Y);
//				planepnt->DesiredPitch = 0x6000;
				planepnt->DesiredPitch = planepnt->Pitch - 0x800;
			}
		}
	}
}

//**************************************************************************************
//  Name might be wrong for maneuver... OH WELL!!!!
void AICuban8(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float pitchpercent;
	float degpitch;
	float desiredroll;
	long workspeed;
	float cornerspeed;
	float stallspeed;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	double dground;

	dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

	if(dground < 2000)
	{
		planepnt->AI.iAIFlags2 |= AIGROUNDAVOID;
	}

	planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;

	planepnt->MaxPitchRate = planepnt->YawRate;

	workspeed = (plane_data->iStallSpeed);
	stallspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
	planepnt->DesiredSpeed = cornerspeed = stallspeed * 2;

	if(planepnt->AI.CombatPos.Z == 1)
	{
		if(((planepnt->Pitch >= 0x2000) && (planepnt->Pitch <= 0x4000)) || (planepnt->V < (stallspeed * 1.25f)))
		{
			planepnt->RollYawCouple = 0;
			degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

			//  Determine how much pitching is left to go.
			pitchpercent = (planepnt->AI.CombatPos.Y - degpitch) / planepnt->AI.CombatPos.Y;

			//  desired roll set so that it is 90 degress at max roll, and (90.0f - max desired pitch) at 0 pitch (ie 80 degree roll is max pitch desired is 10 degrees).
			desiredroll = (90.0f - (planepnt->AI.CombatPos.Y * pitchpercent)) ;

			if(desiredroll > 180.0f) //  Negative pitch land
			{
				desiredroll = 180.0f;
			}
			else if(desiredroll <= 90.0f)  // Over max desired pitch, start back down.
			{
				planepnt->AI.CombatPos.Y = degpitch;
				desiredroll = 90.0f;
			}

			if(planepnt->AI.CombatPos.X < 0)
			{
				desiredroll = -desiredroll;
			}
			planepnt->DesiredRoll = AIConvert180DegreeToAngle(-desiredroll);

			if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll)) < 1)
			{
				planepnt->AI.CombatPos.Z = 2.0f;
			}
			planepnt->AI.lCombatTimer = 10000;

			planepnt->AI.DesiredPitch = planepnt->Pitch;
		}
		else
		{
			planepnt->DesiredRoll = 0;
			planepnt->DesiredPitch = 0x2800;
		}
	}
	else if(planepnt->AI.CombatPos.Z == 2)
	{
		degpitch = AIConvertAngleTo180Degree(planepnt->Pitch);

		//  Determine how much pitching is left to go.
		pitchpercent = (planepnt->AI.CombatPos.Y - degpitch) / planepnt->AI.CombatPos.Y;

		//  desired roll set so that it is 90 degress at max roll, and (90.0f - max desired pitch) at 0 pitch (ie 80 degree roll is max pitch desired is 10 degrees).
		desiredroll = (90.0f - (planepnt->AI.CombatPos.Y * pitchpercent)) ;

		if(desiredroll > 180.0f) //  Negative pitch land
		{
			desiredroll = 180.0f;
		}
		else if(desiredroll <= 90.0f)  // Over max desired pitch, start back down.
		{
			planepnt->AI.CombatPos.Y = degpitch;
			desiredroll = 90.0f;
		}

		if(planepnt->AI.CombatPos.X < 0)
		{
			desiredroll = -desiredroll;
		}
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(-desiredroll);

		if(fabs(AIConvertAngleTo180Degree(planepnt->DesiredRoll + planepnt->Roll)) < 1)
		{
			planepnt->AI.CombatPos.Z = 3.0f;
		}
		planepnt->AI.lCombatTimer = 10000;
	}
	else
	{
		planepnt->RollYawCouple = -2;
		if(planepnt->AI.CombatPos.Z < 0)  //  On our way down.
		{
			planepnt->DesiredPitch = 0x2000;
			if(planepnt->Pitch < 0x8000)  //  we are done
			{
				planepnt->DesiredRoll = 0;
				planepnt->AI.CombatPos.Y = 0;
				GeneralClearCombatBehavior(planepnt);
			}
			else
			{
				planepnt->AI.lCombatTimer = 10000;
			}
		}
		else
		{
			planepnt->AI.lCombatTimer = 10000;
			if((planepnt->Roll < 0x4000) || (planepnt->Roll > 0xC000))
			{
				planepnt->DesiredPitch = 0x2000;
				planepnt->AI.CombatPos.Z = -1;
			}
			else
			{
//				planepnt->DesiredPitch = AIConvert180DegreeToAngle(planepnt->AI.CombatPos.Y);
//				planepnt->DesiredPitch = 0x6000;
				planepnt->DesiredPitch = planepnt->Pitch - 0x800;
			}
		}
	}
}

//**************************************************************************************
void AICheckForNewDefensiveTactic(PlaneParams *planepnt, PlaneParams *threat, double offangle, double offpitch, double tdist, double toffangle)
{
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	DBAircraftType *threat_data = &pDBAircraftList[threat->AI.iPlaneIndex];
	int workspeed;
	float cornerspeed;
	float degroll;
	float stallspeed;
	float chance;
	float yawrate, tyawrate;

	planepnt->AI.iAICombatFlags1 &= ~(AI_FINISH_BEHAVIOR);


	if((MultiPlayer) && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		planepnt->AI.CombatBehavior = NULL;
		planepnt->AI.lCombatTimer = 2000 + (planepnt->AI.iSkill * 500);
	}

	if((planepnt->Pitch > 0x2000) && (planepnt->Pitch < 0xE000))
	{
		return;
	}

	if(fabs(offangle) < 45)
	{
		if(fabs(toffangle) < 45)
		{
		}
		else if(fabs(toffangle) < 90)
		{
		}
		else
		{
		}
	}
	else if(fabs(offangle) < 90)
	{
		if(fabs(toffangle) < 45)
		{
		}
		else if(fabs(toffangle) < 90)
		{
		}
		else
		{
		}
	}
	else
	{
		workspeed = (plane_data->iStallSpeed);
		stallspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC);
		cornerspeed = stallspeed * 2;
		degroll = AIConvertAngleTo180Degree(planepnt->Roll);

		if(!((plane_data->lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))))
		{
			if((fabs(offangle) > 160) && ((planepnt->Pitch < 0x1800) || (planepnt->Pitch > 0xE800)))
			{
	//			if(frand() < (1.0f))
				if(frand() < (0.25f - (planepnt->AI.iSkill * 0.05f)))
				{
					if(((stallspeed * 2) < planepnt->V) && ((fabs(toffangle) > 70.0f) || (tdist < (0.5f * NMTOWU))))
					{
						if(fabs(toffangle) < 15)
						{
							if(fabs(degroll) <= 45)
							{
	//							Camera1.AttachedObject = (int *)planepnt;
								planepnt->AI.CombatPos.X = (degroll < 0) ? -1 : 1;
								planepnt->AI.CombatPos.Y = 90.0f - fabs(degroll);
								planepnt->AI.CombatPos.Z = 1;
								planepnt->AI.CombatBehavior = AIImmelmann;
								planepnt->AI.lCombatTimer = 10000;
								if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
								{
									NetPutDogfightCPosUpdate(planepnt, 6, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
								}
								if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
								{
									PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
									lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
									iLastWSOWarnType = TARGET_PLANE;
									pLastWSOWarnObj = planepnt;
									if(lAdvisorFlags & ADV_CASUAL_BETTY)
									{
										AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
									}
//									AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 3000, 50, AICF_ENEMY_VERTICLE);
								}
								return;
							}
						}
						else if((fabs(toffangle) < 30) || (frand() < 0.5f))
						{
	//						Camera1.AttachedObject = (int *)planepnt;
							planepnt->AI.CombatPos.X = 1;
							planepnt->AI.CombatPos.Y = 90.0f;
							planepnt->AI.CombatPos.Z = 1;
							planepnt->AI.CombatBehavior = AIImmelmann;
							planepnt->AI.lCombatTimer = 10000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 6, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
							{
								PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
								lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
								iLastWSOWarnType = TARGET_PLANE;
								pLastWSOWarnObj = planepnt;
								if(lAdvisorFlags & ADV_CASUAL_BETTY)
								{
									AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
								}
//								AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 3000, 50, AICF_ENEMY_VERTICLE);
							}
							return;
						}
						else
						{
	//						Camera1.AttachedObject = (int *)planepnt;
							planepnt->AI.CombatPos.X = (toffangle > 0) ? -1 : 1;
							planepnt->AI.CombatPos.Y = 45.0f;
							planepnt->AI.CombatPos.Z = 1;
							planepnt->AI.CombatBehavior = AIImmelmann;
							planepnt->AI.lCombatTimer = 10000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 6, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
							{
								PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
								lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
								iLastWSOWarnType = TARGET_PLANE;
								pLastWSOWarnObj = planepnt;
								if(lAdvisorFlags & ADV_CASUAL_BETTY)
								{
									AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
								}
//								AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 3000, 50, AICF_ENEMY_VERTICLE);
							}
							return;
						}
					}
					else if(((stallspeed * 1.25) > planepnt->V) || (((stallspeed * 2) > planepnt->V) && (planepnt->V < threat->V)))
					{
						if((planepnt->V * (180.0f / planepnt->MaxPitchRate)) < (planepnt->HeightAboveGround * WUTOFT))
						{
	//						Camera1.AttachedObject = (int *)planepnt;
							planepnt->AI.CombatPos.X = 1;
							if(fabs(toffangle) < 30.0f)
							{
								planepnt->AI.CombatPos.Y = -85 + (frand() * 40.0f);
								if(rand() & 1)
								{
									planepnt->AI.CombatPos.X = -1;
								}
							}
							else
							{
								planepnt->AI.CombatPos.Y = -45.0f;
								planepnt->AI.CombatPos.X = (toffangle > 0) ? -1 : 1;
							}
							planepnt->AI.CombatPos.Z = 1;
							planepnt->AI.CombatBehavior = AISplitS;
							planepnt->AI.lCombatTimer = 10000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 7, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
							{
								PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
								lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
								iLastWSOWarnType = TARGET_PLANE;
								pLastWSOWarnObj = planepnt;
								if(lAdvisorFlags & ADV_CASUAL_BETTY)
								{
									AICGenericBettySpeech(BETTY_ENEMY_U_D, 0);
								}
//								AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_WEEDS, 1000, 50, AICF_ENEMY_WEEDS);
							}
							return;
						}
						else if((planepnt->V * (180.0f / planepnt->MaxPitchRate) * 0.708f) < (planepnt->HeightAboveGround * WUTOFT))
						{
							if(offangle < 0)
							{
								planepnt->AI.CombatPos.X = 1;
							}
							else
							{
								planepnt->AI.CombatPos.X = -1;
							}
	//						Camera1.AttachedObject = (int *)planepnt;
							planepnt->AI.CombatPos.Y = -45.0f;
							planepnt->AI.CombatPos.Z = 1;
							planepnt->AI.CombatBehavior = AISplitS;
							planepnt->AI.lCombatTimer = 10000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 7, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
							{
								PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
								lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
								iLastWSOWarnType = TARGET_PLANE;
								pLastWSOWarnObj = planepnt;
								if(lAdvisorFlags & ADV_CASUAL_BETTY)
								{
									AICGenericBettySpeech(BETTY_ENEMY_U_D, 0);
								}
//								AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_WEEDS, 1000, 50, AICF_ENEMY_WEEDS);
							}
							return;
						}
					}
					else
					{
						if((planepnt->V * (180.0f / planepnt->MaxPitchRate) * 0.6667f) < (planepnt->HeightAboveGround * WUTOFT))
						{
	//						Camera1.AttachedObject = (int *)planepnt;
							planepnt->AI.CombatPos.X = 1;
							planepnt->AI.CombatPos.Y = -85.0f;
							planepnt->AI.CombatPos.Z = 1;
							planepnt->AI.CombatBehavior = AICuban8;
							planepnt->AI.lCombatTimer = 10000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightCPosUpdate(planepnt, 8, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
							}
							if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
							{
								PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
								lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
								iLastWSOWarnType = TARGET_PLANE;
								pLastWSOWarnObj = planepnt;
								if(lAdvisorFlags & ADV_CASUAL_BETTY)
								{
									AICGenericBettySpeech(BETTY_ENEMY_U_D, 1);
								}
//								AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 1000, 50, AICF_ENEMY_VERTICLE);
							}
							return;
						}
					}
				}
			}

			if((tdist < (NMTOWU * 0.5f)) && (planepnt->Pitch < 0x8000) && ((planepnt->Pitch - threat->Pitch) < 0x8000))
			{
	//			if(frand() < (1.25f))
				if(frand() < (0.25f - (planepnt->AI.iSkill * 0.05f)))
				{
					if(((stallspeed * 1.5) < planepnt->V) && (planepnt->V < (threat->V + 50.0f)))
					{
						if(frand() < 0.50f)
						{
							planepnt->AI.CombatPos.X = (frand() * 30.0f) + 60.0f;
							if(toffangle < 0.0f)
							{
								planepnt->AI.CombatPos.X *= -1.0f;
							}
							planepnt->AI.CombatPos.Y = cos(DegToRad(planepnt->AI.CombatPos.X)) * 30.0f;
						}
						else
						{
							planepnt->AI.CombatPos.X = 0.0f;
							planepnt->AI.CombatPos.Y = 0.0f;
						}

						planepnt->AI.CombatPos.Z = 90.0f;
						if(toffangle < 0.0f)
						{
							planepnt->AI.CombatPos.Z *= -1.0f;
						}
						planepnt->AI.lCombatTimer = 25000;
						planepnt->AI.CombatBehavior = AIBarrelAvoid;
						if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							NetPutDogfightCPosUpdate(planepnt, 1, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
						}
						return;
					}
				}
			}
		}

		if(fabs(toffangle) < 45)
		{
			if(((planepnt->Pitch > 0xF800) || (planepnt->Pitch < 0x8000)) && (planepnt->V < (cornerspeed * 0.95f)))
			{
				chance = 0.8f - (planepnt->AI.iSkill * 0.15);
				if(frand() < chance)
				{
					degroll = AIConvertAngleTo180Degree(planepnt->Roll);
					if((fabs(90.0f - fabs(degroll)) < 10.0f) && (fabs(AIConvertAngleTo180Degree(planepnt->Roll - threat->Roll)) < 20.0f) && (planepnt->HeightAboveGround > (4000.0f * FTTOWU)))
					{
						yawrate = fabs(AIConvertAngleTo180Degree(planepnt->AI.LastHeading - planepnt->Heading));
						yawrate *= 1000.0f / (float)DeltaTicks;
						tyawrate = fabs(AIConvertAngleTo180Degree(threat->AI.LastHeading - threat->Heading));
						tyawrate *= 1000.0f / (float)DeltaTicks;
						if(yawrate < tyawrate)
						{
							planepnt->AI.CombatBehavior = AILowYoYo;
							planepnt->AI.lCombatTimer = 5000;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutDogfightUpdate(planepnt, 2, planepnt->AI.lCombatTimer);
							}
							return;
						}
					}
				}
			}

		}
		else if(fabs(toffangle) < 90)
		{
		}
		else
		{
		}
	}
	planepnt->AI.CombatBehavior = NULL;
	planepnt->AI.lCombatTimer = 2000 + (planepnt->AI.iSkill * 500);
}

//**************************************************************************************
int AICanFireMissile(PlaneParams *planepnt, PlaneParams *targplane, float range)
{
	int cnt;
	int weapontype;
	int maxrange, rangenm;
	float minrange;
	int firenum = -1;
	int indexnum;
	float temprange;
	int seeker;
	float tempheading, temppitch;
	int gotaspect = 0;
	float skillangle = 60.0f - (planepnt->AI.iSkill * 5.0f);
	float obskillangle = 90.0f - (planepnt->AI.iSkill * 5.0f);
	int off_bore_only = 0;
	int seekerFOV;
	int locknum = -1;
	int maxlockrange;

	if(targplane == NULL)
	{
		return(firenum);
	}

	if((targplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2))
	{
		return(firenum);
	}

	if(AIPlaneAlreadyTarget(AIRCRAFT, planepnt, targplane))
	{
		return(firenum);
	}

	PLGetRelativeHP(planepnt, targplane->WorldPosition, &tempheading, &temppitch);
	if((fabs(tempheading) > skillangle) || (fabs(temppitch) > skillangle))
	{
		if((fabs(tempheading) > obskillangle) || ((temppitch > obskillangle) || (temppitch < 0)))
		{
			return(firenum);
		}
		off_bore_only = 1;
	}

	if(range < (NMTOWU * 1.0f))
	{
		//  I think I was looking for rear only shots at close range so I didn't waste a shot
		if(!gotaspect)
		{
			PLGetRelativeHP(targplane, planepnt->WorldPosition, &tempheading, &temppitch);
			gotaspect = 1;
		}
		if((fabs(tempheading) < (180.0f - skillangle)) || (fabs(temppitch) > skillangle) || (range < (NMTOWU * 0.3f)))  // was 0.5f
		{
			return(firenum);
		}
	}

	maxrange = -1;
	maxlockrange = -1;
	rangenm = (range * WUTONM);

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		indexnum = planepnt->WeapLoad[cnt].WeapIndex;
		weapontype = pDBWeaponList[indexnum].iWeaponType;
		if((weapontype == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AA_LONG_RANGE_MISSILE))
		{
			seeker = pDBWeaponList[indexnum].iSeekerType;
			if(!(planepnt->AI.iAIFlags1 & AIRADARON))
			{
				if((seeker == 1) || (seeker == 7))
				{
					continue;
				}
			}

			if(seeker == 3)
			{
				if(!gotaspect)
				{
					PLGetRelativeHP(targplane, planepnt->WorldPosition, &tempheading, &temppitch);
					gotaspect = 1;
				}
				if((fabs(tempheading) < (180.0f - skillangle)) || (fabs(temppitch) > skillangle))
				{
					continue;
				}
			}

			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				if((off_bore_only) && ((seeker != 2) && (seeker != 3)))
				{
					continue;
				}

				seekerFOV = pDBWeaponList[indexnum].iSeekerFOV;
				if((fabs(tempheading) > seekerFOV) || (fabs(temppitch) > seekerFOV))
				{
					continue;
				}

				temprange = pDBWeaponList[indexnum].iRange;
				temprange *= CalcAltEffectOnMissile((planepnt->WorldPosition.Y + planepnt->WorldPosition.Y + targplane->WorldPosition.Y) / 3);
				minrange = pDBWeaponList[indexnum].fMinRange;
				if((minrange <= rangenm) && (temprange > rangenm) && ((maxrange == -1) || (temprange < maxrange)))
				{
					firenum = cnt;
					maxrange = temprange;
					locknum = cnt;
					maxlockrange = temprange;
				}
				else if((minrange <= rangenm) && (((float)temprange * 1.25f) > (float)rangenm) && ((maxlockrange == -1) || (temprange < maxlockrange)))
				{
					locknum = cnt;
					maxlockrange = temprange;
				}
			}
		}
	}
	if((firenum == -1) && (locknum != -1))
	{
		if(pDBWeaponList[planepnt->WeapLoad[locknum].WeapIndex].iSeekerType == 7)
		{
			planepnt->AI.iAICombatFlags2 |= AI_EARLY_LOCK;
		}
	}
	else if(firenum != -1)
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_EARLY_LOCK);
	}

	return(firenum);
}

#if 0
//**************************************************************************************
int AIPlaneAlreadyTarget(PlaneParams *planepnt, PlaneParams *targplane)
{
	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->iTargetType == TARGET_PLANE) && (W->P->AI.iSide == planepnt->AI.iSide))
			{
				if(targplane == (PlaneParams *)W->pTarget)
				{
					return(1);
				}
			}
		}
		W++;
	}
	return(0);
}

#else
//**************************************************************************************
int AIPlaneAlreadyTarget(int launchertype, void *launchpnt, PlaneParams *targplane)
{
	WeaponParams *W = &Weapons[0];
	PlaneParams *planepnt;
	BasicInstance *tbasic;
	BasicInstance *tradar;
	AAWeaponInstance *tweapon;
	GDRadarData *radardat;


	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(launchertype == -1)
			{
				if(W->iTargetType == TARGET_PLANE)
				{
					if(targplane == (PlaneParams *)W->pTarget)
					{
						return(1);
					}
				}
			}
			else if(W->P && (launchertype == AIRCRAFT))
			{
				planepnt = (PlaneParams *)launchpnt;
				if(planepnt)
				{
					if((W->iTargetType == TARGET_PLANE) && (W->P->AI.iSide == planepnt->AI.iSide))
					{
						if(targplane == (PlaneParams *)W->pTarget)
						{
							return(1);
						}
					}
				}
				else
				{
					if(W->iTargetType == TARGET_PLANE)
					{
						if(targplane == (PlaneParams *)W->pTarget)
						{
							return(1);
						}
					}
				}
			}
			else
			{
				if(W->iTargetType == TARGET_PLANE)
				{
					if(targplane == (PlaneParams *)W->pTarget)
					{
						if((launchertype == AIRCRAFT) || (launchertype == MOVINGVEHICLE))
						{
							return(1);
						}
						else
						{
							tbasic = (BasicInstance *)W->Launcher;
							if(tbasic->Family == FAMILY_AAWEAPON)
							{
								tweapon = (AAWeaponInstance *)tbasic;
								radardat = (GDRadarData *)tweapon->AIDataBuf;
								tradar = (BasicInstance *)radardat->pWRadarSite;
							}
							else
							{
								tradar = NULL;
							}

							if(tradar != launchpnt)
							{
								return(1);
							}
						}
					}
				}
			}
		}
		W++;
	}
	return(0);
}
#endif

//**************************************************************************************
int AINumMissilesAtPlane(PlaneParams *targplane)
{
	WeaponParams *W = &Weapons[0];
	int numinair = 0;


	while(W <= LastWeapon)
	{
		if ((W->Flags & WEAPON_INUSE) && (W->Kind == MISSILE))
		{
			if(W->iTargetType == TARGET_PLANE)
			{
				if(targplane == (PlaneParams *)W->pTarget)
				{
					numinair ++;
				}
			}
		}
		W ++;
	}
	return(numinair);
}

//**************************************************************************************
void AIFireMissile(PlaneParams *planepnt, PlaneParams *targplane, int stationnum)
{
	int weaponindex, seeker;
	int targetnum = targplane - Planes;
	int skilltime;

	if(MultiPlayer && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	if(planepnt->WeapLoad[stationnum].Count <= 0)
	{
		return;
	}

	planepnt->AADesignate = targplane;

	weaponindex = planepnt->WeapLoad[stationnum].WeapIndex;
	seeker = pDBWeaponList[weaponindex].iSeekerType;

	if((WingNumDTWSTargets) && (seeker != 7))
	{
		if(planepnt->AI.winglead == (PlayerPlane - Planes))
		{
			AIRemoveTopWingDTWS();
		}
	}

	if(seeker == 7) // || (no TWS radar only STT))
	{
		planepnt->AI.iAIFlags2 |= AILOCKEDUP;
	}
	else if(seeker == 1)
	{
		planepnt->AI.iAIFlags2 |= AI_CHECK_MISSILES;
	}

	planepnt->WeapLoad[stationnum].Count--;
	InstantiateMissile(planepnt, stationnum);

	planepnt->AI.lLockTimer = 3000 + ((rand() & 1) * 1000);
	skilltime = frand() * ((GP_NOVICE - planepnt->AI.iSkill) * 1000);
	planepnt->AI.lLockTimer += skilltime;
}

//  NOTE:  Bullets are counted here too, but since this is used to maintain STT locks
//  I'm going to keep it the same way.
//**************************************************************************************
int AISeeIfStillMissiled(PlaneParams *planepnt, PlaneParams *targplane, int maintain, int missilesonly)
{
	int returnval = 0;
	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->iTargetType == TARGET_PLANE) && (W->P == planepnt) && ((W->Kind == MISSILE) || (!missilesonly)))
			{
				if(targplane == (PlaneParams *)W->pTarget)
				{
					if(maintain)
					{
					 	W->Flags &= ~(LOSING_LOCK|LOST_LOCK);
					}
					returnval = 1;
				}
			}
		}
		W++;
	}
	return(returnval);
}

//**************************************************************************************
int AISeeIfStillGroundMissiled(BasicInstance *radarsite, PlaneParams *targplane, int maintain, int missilesonly)
{
	int returnval = 0;
	WeaponParams *W = &Weapons[0];
	BasicInstance *tbasic;
	BasicInstance *tradar;
	AAWeaponInstance *tweapon;
	GDRadarData *radardat = NULL;

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->iTargetType == TARGET_PLANE))  //   && (W->Launcher == radarsite))
			{
				if((W->LauncherType == GROUNDOBJECT) && ((W->Kind == MISSILE) || (!missilesonly)))
				{
					tbasic = (BasicInstance *)W->Launcher;
					if(tbasic->Family == FAMILY_AAWEAPON)
					{
						tweapon = (AAWeaponInstance *)tbasic;
						radardat = (GDRadarData *)tweapon->AIDataBuf;
						tradar = (BasicInstance *)radardat->pWRadarSite;
					}
					else
					{
						tradar = NULL;
					}

					if(tradar == radarsite)
					{
						if(targplane == (PlaneParams *)W->pTarget)
						{
							if(maintain)
							{
					 			W->Flags &= ~(LOSING_LOCK|LOST_LOCK);
							}
							else
							{
								return(1);
							}
							returnval = 1;
						}
					}
				}
			}
		}
		W++;
	}
	return(returnval);
}

//**************************************************************************************
int AISeeIfStillVehicleMissiled(MovingVehicleParams *radarsite, PlaneParams *targplane, int maintain, GDRadarData *radardat, int missilesonly)
{
	int returnval = 0;
	int radardatnum = radardat - radarsite->RadarWeaponData;
	WeaponParams *W = &Weapons[0];
	GDRadarData *rradardat;
	int slotnum;
	DBWeaponType *weapon=NULL;

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->iTargetType == TARGET_PLANE) && (W->Launcher == radarsite))
			{
				if((W->LauncherType == MOVINGVEHICLE) && ((W->Kind == MISSILE) || (!missilesonly)))
				{
					if(targplane == (PlaneParams *)W->pTarget)
					{
						if(radardatnum == W->LaunchStation)
						{
							if(maintain)
							{
				 				W->Flags &= ~(LOSING_LOCK|LOST_LOCK);
							}
							else
							{
								return(1);
							}
							returnval = 1;
						}
						else if(W->LaunchStation >= 0)
						{
							rradardat = (GDRadarData *)radarsite->RadarWeaponData[W->LaunchStation].pWRadarSite;
							if(rradardat == radardat)
							{
								if(maintain)
								{
				 					W->Flags &= ~(LOSING_LOCK|LOST_LOCK);
								}
								else
								{
									return(1);
								}
								returnval = 1;
							}
						}
					}
				}
			}
		}
		W++;
	}

	if(missilesonly) return(returnval);

	if(!returnval)
	{
		if(radarsite->iShipType)
		{
			for(slotnum = 0; slotnum < pDBShipList[radarsite->iVDBIndex].iNumDefenseItems; slotnum ++)
			{
				if(pDBShipList[radarsite->iVDBIndex].DefenseList[slotnum].lDefenseType != VS_DEFENSE_WEAPON)
				{
					continue;
				}
				if(radarsite->RadarWeaponData[slotnum].Target == (void *)targplane)
				{
					if(radarsite->RadarWeaponData[slotnum].lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
					{
						if(!missilesonly)
						{
							returnval = 1;
						}
						else
						{
							weapon = VGDGetShipWeaponForRDNum(radarsite, slotnum);
							if(weapon)
							{
								if(weapon->iWeaponType == WEAPON_TYPE_SAM)
								{
									returnval = 1;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			for(slotnum = 0; slotnum < 2; slotnum ++)
			{
				if(radarsite->RadarWeaponData[slotnum].Target == (void *)targplane)
				{
					if(radarsite->RadarWeaponData[slotnum].lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
					{
						if(!missilesonly)
						{
							returnval = 1;
						}
						else
						{
							if(slotnum == 0)
								weapon = get_weapon_ptr(pDBVehicleList[radarsite->iVDBIndex].lWeaponType);
							else
								weapon = get_weapon_ptr(pDBVehicleList[radarsite->iVDBIndex].lWeaponType2);

							if(weapon)
							{
								if(weapon->iWeaponType == WEAPON_TYPE_SAM)
								{
									returnval = 1;
								}
							}
						}
					}
				}
			}
		}
	}

	return(returnval);
}


//**************************************************************************************
void AICheckTargetReaction(WeaponParams *weaponpnt, int justfired)
{
	PlaneParams *planepnt;
	int currthreat;
	float impacttime;
	int allownotice = 1;

	planepnt = (PlaneParams *)weaponpnt->pTarget;

	if((weaponpnt->Flags2 & MISSILE_HOME_ON_JAM) && (planepnt))
	{
		if(!(weaponpnt->Smoke))
		{
			if((weaponpnt - Weapons) != planepnt->AI.iMissileThreat)
			{
				allownotice = 0;
			}
		}
	}

	if((weaponpnt->iTargetType == TARGET_PLANE) && (weaponpnt->lTargetDistFt >= 0) && (planepnt) && (allownotice))
	{
		if(weaponpnt->Flags & MISSILE_SPOTTED)
		{
			if(planepnt->AI.iMissileThreat < 0)
			{
				planepnt->AI.lCMTimer = 1000;
				planepnt->AI.iMissileThreat = weaponpnt - Weapons;
				if(AIPlaneAlreadyTarget(-1, NULL, planepnt))
				{
					planepnt->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN);
				}
				else
				{
					planepnt->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN|AI_WSO_ENGAGE_DEFENSIVE);
				}
			}
			else
			{
				currthreat = planepnt->AI.iMissileThreat;
				if((weaponpnt->lTargetDistFt + 1000) < Weapons[currthreat].lTargetDistFt)
				{
					planepnt->AI.iMissileThreat = weaponpnt - Weapons;
					planepnt->AI.iAIFlags1 &= ~AI_WSO_MISSILE_WARN;
				}
			}
			currthreat = planepnt->AI.iMissileThreat;
			if(currthreat == (weaponpnt - Weapons))
			{
				impacttime = weaponpnt->lTargetDistFt / (weaponpnt->InitialVelocity * (WUTOFT*50.0));
				if(impacttime < 10)  //  5
				{
					if(!(planepnt->AI.iAIFlags1 & AI_WSO_MISSILE_WARN))
					{
						planepnt->AI.iAIFlags1 |= AI_WSO_MISSILE_WARN;
						AICDoMissileBreak(planepnt, weaponpnt);
					}
				}

				if(impacttime < 15)  //  10
				{
					if(planepnt->AI.lCMTimer < 0)
					{
						AIDoCMDrop(planepnt, weaponpnt);
						if(impacttime < 5)
						{
							if(planepnt->AI.iSkill < GP_NOVICE)
							{
								if(planepnt->AI.lCMTimer > (1000 + (300 * planepnt->AI.iSkill)))
								{
									planepnt->AI.lCMTimer = 1000 + (300 * planepnt->AI.iSkill);
								}
							}
						}
					}
				}

				if(impacttime < 20)  //  15
				{
					if(!(planepnt->AI.iAIFlags1 & AI_WSO_ENGAGE_DEFENSIVE))
					{
						planepnt->AI.iAIFlags1 |= AI_WSO_ENGAGE_DEFENSIVE;
						if(planepnt == PlayerPlane)
						{
							iLastWSOWarnType = TARGET_WEAPON;
							pLastWSOWarnObj = weaponpnt;

//							AICAddSoundCall(AICWSOEngageDefensive, planepnt - Planes, 0, 20, weaponpnt - Weapons);
						}
					}
				}
			}
		}
		else
		{
			if(AIMissileNoticed(weaponpnt))
			{
				if(planepnt == PlayerPlane)
				{
					iLastWSOWarnType = TARGET_WEAPON;
					pLastWSOWarnObj = weaponpnt;

					AICAddSoundCall(AIC_WSO_MissileInbound, planepnt - Planes, 500 + (rand() & 1023), 10, weaponpnt - Weapons);
				}
				weaponpnt->Flags |= MISSILE_SPOTTED;
			}
		}
	}
	return;
}

//**************************************************************************************
int AIMissileNoticed(WeaponParams *weaponpnt)
{
	int weaponindex, seeker;
	int visualrange;
	PlaneParams *planepnt;
	float heading, pitch;
	int hours;
	int isnight = 0;

	weaponindex = AIGetWeaponIndex(weaponpnt->Type->TypeNumber);
	seeker = pDBWeaponList[weaponindex].iSeekerType;

	if(((seeker == 7) && (!(weaponpnt->Flags2 & MISSILE_HOME_ON_JAM))) || ((seeker == 1) && (weaponpnt->Flags & ACTIVE_SEEKER)))
	{
		return(1);
	}

	if((weaponpnt->Flags2 & MISSILE_HOME_ON_JAM) && (!(weaponpnt->Smoke)))
	{
		return(0);
	}

	hours = WorldParams.WorldTime/3600;

	if((hours < 6) || (hours > 18))
	{
		isnight = 1;
	}

	if(isnight)
	{
		visualrange = 5 * NMTOFT;
		if(pDBWeaponList[weaponindex].iRocketMotor == 1)
		{
			visualrange *= 2;
		}
		else
		{
			visualrange /= 4;
		}
	}
	else if((pDBWeaponList[weaponindex].iRocketMotor == 1) && (weaponpnt->Flags & BOOST_PHASE))
	{
		visualrange = 12 * NMTOFT;
	}
	else
	{
		visualrange = 5 * NMTOFT;
	}

	if(weaponpnt->lTargetDistFt > 0)
	{
		if(visualrange > weaponpnt->lTargetDistFt)
		{
			//  Will want to do a check for plane orientation to missile.
			if(weaponpnt->iTargetType == TARGET_PLANE)
			{
				planepnt = (PlaneParams *)weaponpnt->pTarget;

				if((planepnt->AI.iMissileThreat < 0) && (planepnt->AI.lCMTimer >= 0))
				{
					return(0);
				}

				PLGetRelativeHP(planepnt, weaponpnt->Pos, &heading, &pitch);

				if(pitch < 0)
				{
					pitch += 30;	//  Added since Clip will leave center of view point well above rail.
				}

				if(!PLClipViewHP(&heading, &pitch))
				{
					return(1);
				}
			}
			else
			{
				return(1);
			}
		}
	}

	return(0);
}

//**************************************************************************************
void AIDoCMDrop(PlaneParams *planepnt, WeaponParams *weaponpnt)
{
	int weaponindex, seeker;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}
	else if(weaponpnt->Flags2 & MISSILE_HOME_ON_JAM)
	{
		return;
	}

	weaponindex = AIGetWeaponIndex(weaponpnt->Type->TypeNumber);
	seeker = pDBWeaponList[weaponindex].iSeekerType;

	if(planepnt == PlayerPlane)
	{
#ifdef __DEMO__
		if(planepnt->Status & PL_AI_DRIVEN)
		{
			AIDropCounterMeasure(planepnt, weaponpnt);
			planepnt->AI.lCMTimer = 2000;
		}
		else if((weaponpnt - Weapons) == planepnt->AI.iMissileThreat)
		{
			iLastWSOWarnType = TARGET_WEAPON;
			pLastWSOWarnObj = weaponpnt;

			AIC_WSO_DoCMMsg(planepnt - Planes, seeker);
			planepnt->AI.lCMTimer = 3000;
		}
#else
		if((weaponpnt - Weapons) == planepnt->AI.iMissileThreat)
		{
			iLastWSOWarnType = TARGET_WEAPON;
			pLastWSOWarnObj = weaponpnt;

			AIC_WSO_DoCMMsg(planepnt - Planes, seeker);
			planepnt->AI.lCMTimer = 3000;
		}
#endif
	}
	else if(planepnt->Status & PL_AI_DRIVEN)
	{
		AIDropCounterMeasure(planepnt, weaponpnt);
		planepnt->AI.lCMTimer = 2000;
	}
	else
	{
		planepnt->AI.lCMTimer = 2000;
	}
}

//**************************************************************************************
void AIDropCounterMeasure(PlaneParams *planepnt, WeaponParams *weaponpnt)
{
	int weaponindex, seeker;

	if(MultiPlayer && (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	weaponindex = AIGetWeaponIndex(weaponpnt->Type->TypeNumber);
	seeker = pDBWeaponList[weaponindex].iSeekerType;

	if((seeker == 1) || (seeker == 7))
	{
		CMDropChaff(planepnt);
	}
	else if((seeker == 2) || (seeker == 3))
	{
		CMDropFlare(planepnt);
	}

	return;
}

//**************************************************************************************
void AICDoMissileBreak(PlaneParams *planepnt, WeaponParams *weaponpnt)
{
	float offangle, tdist, dx, dy, dz;
	int breakright;

	if((planepnt->AI.lPlaneID == 30) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		return;
	}

	offangle = AIComputeHeadingToPoint(planepnt, weaponpnt->Pos, &tdist, &dx ,&dy, &dz, 1);

	if(planepnt->Status & PL_AI_DRIVEN)
	{
		if(!(weaponpnt->Flags2 & MISSILE_WARNED))
		{
			weaponpnt->Flags2 |= MISSILE_WARNED;
			if((AIInPlayerGroup(planepnt)) && (PlayerPlane != planepnt))
			{
				AIC_Wingman_Defensive_Msg(planepnt - Planes, weaponpnt - Weapons);
			}
			else if(planepnt->AI.iSide == PlayerPlane->AI.iSide)
			{
				if((planepnt->AI.lAIVoice == SPCH_CAP1) || (planepnt->AI.lAIVoice == SPCH_CAP2) || (planepnt->AI.lAIVoice == SPCH_CAP3))
				{
					AIC_CAPMissileWarn(planepnt - Planes, weaponpnt - Weapons);
				}

			}
		}
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			AISetBreakBehavior(planepnt, offangle, weaponpnt);
		}
		return;
	}

	if(fabs(offangle) < 120)
	{
		return;
	}

	breakright = (offangle < 0) ? 1 : 0;

	if(!(weaponpnt->Flags2 & MISSILE_WARNED))
	{
		weaponpnt->Flags2 |= MISSILE_WARNED;
		AIC_WSO_DoBreakMsg(planepnt - Planes, breakright);
	}

	planepnt->AI.lCMTimer = 2000;

	if(planepnt == PlayerPlane)
	{
		iLastWSOWarnType = TARGET_WEAPON;
		pLastWSOWarnObj = weaponpnt;
	}
}

//**************************************************************************************
int AICheckIfGroupAAClear(PlaneParams *planepnt)
{
	if(!(planepnt->Status & PL_AI_DRIVEN))
	{
		if(planepnt->AI.AirThreat)
		{
			return(0);
		}
	}
	else
	{
		if(planepnt->AI.AirTarget)
		{
			return(0);
		}
		else if(planepnt->AI.AirThreat)
		{
			return(0);
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(!AICheckIfGroupAAClear(&Planes[planepnt->AI.wingman]))
		{
			return(0);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		if(!AICheckIfGroupAAClear(&Planes[planepnt->AI.nextpair]))
		{
			return(0);
		}
	}

	return(1);
}

//**************************************************************************************
void AICheckIfDefensive(PlaneParams *planepnt, float *roffangle, float *rtdist)
{
	PlaneParams *target;
	double dx, dy, dz;
	double offangle, toffangle;
	double dground;
	double tdist;
	double heading, theading;
	PlaneParams *wingplane = NULL;
	FPoint targetpos;
	float defensiverange = 12000.0f;
//	double tdist;
//	int cnt;


	if((planepnt->AI.Behaviorfunc == AIDisengage) || (!(planepnt->AI.iAIFlags2 & AISUPPORT)))
	{
		if(AIAATargetChecks(planepnt))
		{
			planepnt->DesiredRoll = 0;
			planepnt->DesiredPitch = 0;
			if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
			{
				planepnt->AI.CombatBehavior = NULL;
			}
			return;
		}
	}

	target = AIGetAirTargetPosition(planepnt, &targetpos, 0);

	if(target == NULL)
	{
		target = AIGetAirTargetPosition(planepnt, &targetpos);
	}

	planepnt->AADesignate = target;

	if(target == NULL)
	{
		return;
	}
	else
	{
		dground = (planepnt->WorldPosition.Y - AICheckHeightInPath(planepnt, 2000)) * WUTOFT;

		dx = target->WorldPosition.X - planepnt->WorldPosition.X;
		dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;
		dy = AICheckSafeAlt(planepnt, target->WorldPosition.Y) - planepnt->WorldPosition.Y;

//		if((dy < 0.0) && (dground < 5000))
//			dy = 0.0;

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

		*roffangle = offangle;
		*rtdist = tdist;

#if 1
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
		{
			defensiverange = 24000.0f;
		}

		if((fabs(toffangle) < 30) && (fabs(offangle) > 90) && ((tdist * WUTOFT) < defensiverange))
		{
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
			}
			return;
		}
#else
		if((fabs(offangle) < 30) && (fabs(toffangle) > 90) && ((tdist * WUTOFT) < 12000))
		{
			if(target != NULL)
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					target->AI.iAIFlags2 |= AIDEFENSIVE;
				}
				GeneralSetNewAirThreat(target, planepnt);
//				target->AI.AirThreat = planepnt;
			}
		}
#endif
	}
	return;
}

//**************************************************************************************
int AINumberOfSideInArea(FPointDouble position, int side, float maxrange, int fightersonly)
{
	int banditcnt = 0;
	PlaneParams *checkplane = Planes;
	float dx, dy, dz;

	while (checkplane <= LastPlane)
	{
		if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (side == checkplane->AI.iSide) && (checkplane->OnGround == 0))
		{
			dx = checkplane->WorldPosition.X - position.X;
			dy = checkplane->WorldPosition.Y - position.Y;
			dz = checkplane->WorldPosition.Z - position.Z;

			if((fabs(dx) < maxrange) && (fabs(dz) < maxrange))
			{
				if(fightersonly)
				{
					if(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER)
					{
						banditcnt ++;
					}
				}
				else
				{
					banditcnt ++;
				}
			}
		}
		checkplane ++;
	}

	return(banditcnt);
}

//**************************************************************************************
int AIIsBeingEscorted(PlaneParams *planepnt)
{
	PlaneParams *checkplane = Planes;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide == checkplane->AI.iSide) && (checkplane->OnGround == 0))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (planepnt == checkplane->AI.LinkedPlane))
			{
				return(1);
			}
		}

		checkplane ++;
	}
	return(0);
}

//**************************************************************************************
void AISetUpEscortInterceptGroup(PlaneParams *planepnt, PlaneParams *target)
{
	int numenemy, numfriendly;

	numenemy = AINumberOfSideInArea(target->WorldPosition, target->AI.iSide, 40.0f * NMTOWU, 1);

	if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		numfriendly = AIGetTotalNumPlayerGroupCovering(AIGetLeader(planepnt));
	}
	else
	{
		numfriendly = AIGetTotalNumInGroup(AIGetLeader(planepnt));
	}

	if(numenemy < 2)
	{
		numenemy = 2;
	}

	if(numenemy & 1)
	{
		numenemy ++;
	}

	if(numenemy >= numfriendly)
	{
		AIUpdateInterceptGroup(planepnt, target, 1);
		return;
	}

	if(AISendEscortsStartingWith(numfriendly - numenemy, planepnt, target, 0) != -1)
	{
		AIUpdateInterceptGroup(planepnt, target, 1);
	}
	return;
}

//**************************************************************************************
int AISendEscortsStartingWith(int numingroup, PlaneParams *currplane, PlaneParams *target, int currcnt)
{
	if(currplane->AI.Behaviorfunc == AIFlyEscort)
	{
		if(numingroup == currcnt)
		{
			currplane->AI.iAICombatFlags1 |= AI_ESCORT_PART_LEAD;
			AIUpdateInterceptGroup(currplane, target, 1);
			return(-1);
		}

		currcnt ++;
	}

	if(currplane->AI.wingman >= 0)
	{
		if(Planes[currplane->AI.wingman].AI.Behaviorfunc == AIFlyEscort)
		{
			if(numingroup == currcnt)
			{
				if(currplane == PlayerPlane)
				{
					if(currplane->AI.nextpair >= 0)
					{
						if(Planes[currplane->AI.nextpair].AI.iAIFlags1 & AIPLAYERGROUP)
						{
							Planes[currplane->AI.nextpair].AI.iAIFlags1 |= AI_ESCORT_PART_LEAD;
						}
					}
				}
				currplane->AI.iAICombatFlags1 |= AI_ESCORT_PART_LEAD;
				AIUpdateInterceptGroup(currplane, target, 1);
				return(-1);
			}
			currcnt ++;
		}
	}

	if(currplane->AI.nextpair >= 0)
	{
		return(AISendEscortsStartingWith(numingroup, &Planes[currplane->AI.nextpair], target, currcnt));
	}

	return(0);
}

//**************************************************************************************
int AIGetTotalNumPlayerGroupCovering(PlaneParams *currplane)
{
	long wingman, nextsec;
	int currcnt;

	if(currplane->AI.Behaviorfunc == AIFlyEscort)
	{
		currcnt = 1;
	}
	else
	{
		currcnt = 0;
	}

	wingman = currplane->AI.wingman;
	nextsec = currplane->AI.nextpair;

	if(wingman >= 0)
	{
		currcnt += AIGetTotalNumPlayerGroupCovering(&Planes[wingman]);
	}

	if(nextsec >= 0)
	{
		currcnt += AIGetTotalNumPlayerGroupCovering(&Planes[nextsec]);
	}
	return(currcnt);
}

//**************************************************************************************
PlaneParams *AIGetEscortLead(PlaneParams *planepnt)
{
	PlaneParams *checkplane;
	PlaneParams *wcheckplane;

	if(!(planepnt->AI.iAIFlags1 & AIPLAYERGROUP))
	{
		return(AIGetLeader(planepnt));
	}

	checkplane = AIGetLeader(planepnt);

	while(checkplane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if(checkplane->AI.Behaviorfunc == AIFlyEscort)
			{
				return(checkplane);
			}

			if(checkplane->AI.wingman >= 0)
			{
				wcheckplane = &Planes[checkplane->AI.wingman];

				if ((wcheckplane->Status & PL_ACTIVE) && (!(wcheckplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
				{
					if(wcheckplane->AI.Behaviorfunc == AIFlyEscort)
					{
						return(wcheckplane);
					}
				}
			}
		}
		if(checkplane->AI.nextpair >= 0)
		{
			checkplane = &Planes[checkplane->AI.nextpair];
		}
		else
		{
			checkplane = NULL;
		}
	}
	return(planepnt);
}

//**************************************************************************************
void AISetUpPlayerCAPIntercept(PlaneParams *planepnt, PlaneParams *target)
{
	long delaycnt = 1000;

	while(planepnt)
	{
		planepnt->AI.AirTarget = target;

		planepnt->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
		planepnt->AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);
		planepnt->AI.iAIFlags2 &= ~(AIKEEPTARGET);

		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			AICAddSoundCall(AICEngageMsg, planepnt - Planes, delaycnt, 50);
			delaycnt = delaycnt + 3000;
		}

		planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;

		planepnt->AI.OrgBehave = AISetUpCAPStart;

		planepnt->AI.lTimer1 = 660000 - (planepnt->AI.iSkill * 100000);

		if(planepnt->AI.wingman < 0)
		{
			planepnt = NULL;
		}
		else
		{
			planepnt = &Planes[planepnt->AI.wingman];
		}
	}
}

//**************************************************************************************
void AIRemoveTopWingDTWS()
{
	int cnt;

	if(!WingNumDTWSTargets)
		return;

	for(cnt = 0; cnt < (WingNumDTWSTargets - 1); cnt ++)
	{
		WingDTWSTargets[cnt] = WingDTWSTargets[cnt + 1];
	}

	WingDTWSTargets[WingNumDTWSTargets - 1] = -1;

	WingNumDTWSTargets--;

	if(!WingNumDTWSTargets)
	{
		if(PlayerPlane->AI.wingman >= 0)
		{
			Planes[PlayerPlane->AI.wingman].AI.iAIFlags2 &= ~AIKEEPTARGET;
		}
	}
	else
	{
		if(PlayerPlane->AI.wingman >= 0)
		{
			GeneralSetNewAirTarget(&Planes[PlayerPlane->AI.wingman], &Planes[WingDTWSTargets[0]]);
//			Planes[PlayerPlane->AI.wingman].AI.AirTarget = &Planes[WingDTWSTargets[0]];
		}
	}
}

//**************************************************************************************
int AILOS(FPointDouble planepos, FPointDouble targetpos)
{
	FPointDouble junkpos;
	int planepntin = 1;

#if 0  //  Supposed to be OK now

	//  ******  NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
	//  ******  SRE SRE SRE SRE SRE SRE SRE
	//  ******  THIS IS TEMPOARAY UNTIL MED TERRAIN HEIGHT
	//  ******  IS FIXED!!!!!!!!
	//  ******  THIS REALLY, REALLY HAS TO BE CHANGED BACK
	//  ******  ASAP SO WE DON'T FORGET ABOUT IT!!!!!!!
	return(1);//  TEMORARY LINE
#endif

	if(!InHighResTerrainRegion(planepos))
	{
		planepntin = 0;
		return(1);
	}

	if(!InHighResTerrainRegion(targetpos))
	{
		if(!planepntin)
		{
			return(1);
		}
		return(1);
	}

	return(LOS(&planepos, &targetpos, &junkpos, MED_RES_LOS, FALSE));
#if 0
	if(planepntin)
	{
		return(LOS(&planepnt->WorldPosition, &checkplane->WorldPosition, &junkpos, MED_RES_LOS, FALSE));
	}
	else
	{
		return(LOS(&checkplane->WorldPosition, &planepnt->WorldPosition, &junkpos, MED_RES_LOS, FALSE));
	}
	return(0);
#endif
}

//**************************************************************************************
int AIAllGroupAGWinchester(PlaneParams *planepnt)
{
	if(!(planepnt->AI.iAIFlags2 & AI_WINCHESTER_AG))
	{
		if(PlaneHasAGWeapons(planepnt))
		{
			return(0);
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(!AIAllGroupAGWinchester(&Planes[planepnt->AI.wingman]))
		{
			return(0);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AIAllGroupAGWinchester(&Planes[planepnt->AI.nextpair]));
	}

	return(1);
}

//**************************************************************************************
int AIAllGroupAAWinchester(PlaneParams *planepnt)
{
	if(!(planepnt->AI.iAIFlags2 & AI_WINCHESTER_AA))
	{
		if(PlaneHasAAWeapons(planepnt))
		{
			return(0);
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(!AIAllGroupAAWinchester(&Planes[planepnt->AI.wingman]))
		{
			return(0);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AIAllGroupAAWinchester(&Planes[planepnt->AI.nextpair]));
	}

	return(1);
}

//**************************************************************************************
void AIDisengage(PlaneParams *planepnt)
{
	float rtdist = -1.0f;
	float roffangle = 0;
	float offangle;
	PlaneParams *linkedplane;

	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		planepnt->AI.iAIFlags2 &= ~AISUPPORT;
	}
	planepnt->AI.iAIFlags2 |= (AI_IN_DOGFIGHT);
	float dy;

	linkedplane = planepnt->AI.LinkedPlane;
	if(linkedplane)
	{
		if(linkedplane->AI.Behaviorfunc != AIDisengage)
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
				planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
			}
			planepnt->DesiredRoll = 0;
			planepnt->DesiredPitch = 0;
			if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				planepnt->AI.iAIFlags2 &= ~AIDEFENSIVE;
			}
			return;
		}
	}

	if((planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1) && (!(planepnt->AI.iAIFlags2 & AIMISSILEDODGE)))
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
			planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
		}
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 &= ~AIDEFENSIVE;
		}
		return;
	}

	if(!planepnt->AI.CombatBehavior)
	{
		planepnt->AI.iAICombatFlags1 &= ~(AI_FINISH_BEHAVIOR);
	}

	if((planepnt->AI.iAIFlags2 & AIGROUNDAVOID) ||(planepnt->AI.iAIFlags2 & AIMISSILEDODGE) || (planepnt->AI.iAIFlags2 & AIDEFENSIVE) || (planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
	{
		if(planepnt->AI.iAIFlags2 & AIDEFENSIVE)
		{
			planepnt->AI.lTimer1 = 600000 - (planepnt->AI.iSkill * 100000);
		}

		if((linkedplane == NULL) && (planepnt->AI.iAIFlags2 & (AIMISSILEDODGE|AIDEFENSIVE)))
		{
			AITransferCASLead(planepnt);
		}
		AICoreAttack(planepnt, &roffangle, &rtdist);
	}
	else if(linkedplane == NULL)
	{
		offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);

		planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

		MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed, 1);
		if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER)
		{
			planepnt->DesiredSpeed *= 0.9;

			dy = (AICheckSafeAlt(planepnt, 5) + (1007.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
		}
		else
		{
			planepnt->DesiredSpeed *= 0.8;

			if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TRANSPORT)
			{
				dy = (AICheckSafeAlt(planepnt, 5) + (2007.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
			}
			else
			{
				dy = (AICheckSafeAlt(planepnt, 5) + (507.0 * FOOT_MULTER)) - planepnt->WorldPosition.Y;
			}
		}

		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
		if(planepnt->AI.AirThreat)
		{
			if(planepnt->AI.AirThreat != PlayerPlane)
			{
				if(planepnt->AI.AirThreat->AI.AirTarget)
				{
					if(AIGetLeader(planepnt->AI.AirThreat->AI.AirTarget) != planepnt)
					{
						if(planepnt->AI.lTimer1 > 60000)
						{
							planepnt->AI.lTimer1 = 60000;
						}
					}
				}
				else
				{
					if(planepnt->AI.lTimer1 > 60000)
					{
						planepnt->AI.lTimer1 = 60000;
					}
				}
			}
		}
		else
		{
			if(planepnt->AI.lTimer1 > 60000)
			{
				planepnt->AI.lTimer1 = 60000;
			}
		}

		planepnt->AI.DesiredPitch *= 2;

		AIUpdateFormation(planepnt);
		AICheckIfDefensive(planepnt, &roffangle, &rtdist);
	}
	else
	{
		AIFormationFlyingLinkedOffset(planepnt);
		AICheckIfDefensive(planepnt, &roffangle, &rtdist);
	}
}

//**************************************************************************************
void AISetGroupDisengage(PlaneParams *planepnt, PlaneParams *leadplane)
{
	long wingman, nextsec;

	if((planepnt != leadplane) && (planepnt->AI.OrgBehave == NULL) && (planepnt->AI.iAIFlags1 & AIINFORMATION))
	{
		planepnt->AI.LinkedPlane = leadplane;
		planepnt->AI.DesiredHeading = leadplane->AI.DesiredHeading;
		planepnt->AI.Behaviorfunc = AIDisengage;
		planepnt->AI.iAIFlags2 |= (AI_IN_DOGFIGHT);
		planepnt->AI.lTimer1 = 300000 - (planepnt->AI.iSkill * 100000);
	}

	wingman = planepnt->AI.wingman;
	nextsec = planepnt->AI.nextpair;
	if(wingman >= 0)
	{
		AISetGroupDisengage(&Planes[wingman], leadplane);
	}

	if(nextsec >= 0)
	{
		AISetGroupDisengage(&Planes[nextsec], leadplane);
	}
}

//**************************************************************************************
void AIExtend(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle)
{
	float feetAGL;
	DBAircraftType *plane_data = &pDBAircraftList[planepnt->AI.iPlaneIndex];
	float workspeed, cornerspeed;

	if(fabs(toffangle) < 45.0f)
	{
		GeneralClearCombatBehavior(planepnt);
		return;
	}

	planepnt->AI.DesiredRoll = 0;

	feetAGL = planepnt->HeightAboveGround * WUTOFT;

	if(feetAGL < 5000.0f)
	{
		planepnt->AI.DesiredPitch = 0x800;
	}
	else if(feetAGL > 10000.0f)
	{
		planepnt->AI.DesiredPitch = 0xF800;
	}
	else
	{
		planepnt->AI.DesiredPitch = 0;
	}

 	MBAdjustAIThrust(planepnt, 600, 1);

 	workspeed = (plane_data->iStallSpeed * 2);
 	cornerspeed = AIConvertSpeedToFtPS(planepnt->Altitude, workspeed, AIDESIREDSPDCALC) * 1.25f;

	if(planepnt->V > (cornerspeed * 1.25f))
	{
		GeneralClearCombatBehavior(planepnt);
	}
}

//**************************************************************************************
int AICheckRadarCrossSig(PlaneParams *planepnt, PlaneParams *checkplane, float daltft, float distnm, DBRadarType *radar, int inSTT)
{
	int crossval = 0;
	int workval;
	int crosssig = pDBAircraftList[checkplane->AI.iPlaneIndex].iRadarCrossSignature;
	int checkedone = 0;
	int checkaspect = 0;
	int useHPRF = 1;
	int useLPRF = 0;
	float Threshold;

	if(!radar)
	{
		return(0);
	}

	if(radar->iRadarType & RADAR_TYPE_MPRF)
	{
//	   	crossval = GetCalculatedCrossSection(crosssig, MPRF, daltft, AAClosureRate(planepnt, checkplane), distnm, inSTT);
	   	crossval = GetCrossSection(crosssig, MPRF, daltft, AAClosureRate(planepnt, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		checkedone = 1;
		useHPRF = 0;
	}

	if(radar->iRadarType & RADAR_TYPE_LPRF)
	{
//	   	workval = GetCalculatedCrossSection(crosssig, HPRF, daltft, AAClosureRate(planepnt, checkplane), distnm, inSTT);
	   	workval = GetCrossSection(crosssig, LPRF, daltft, AAClosureRate(planepnt, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		if(workval > crossval)
		{
			crossval = workval;
		}
		checkedone = 1;
		useHPRF = 0;
		useLPRF = 1;
	}

	if((radar->iRadarType & RADAR_TYPE_HPRF) || (!checkedone))
	{
//	   	workval = GetCalculatedCrossSection(crosssig, HPRF, daltft, AAClosureRate(planepnt, checkplane), distnm, inSTT);
	   	workval = GetCrossSection(crosssig, HPRF, daltft, AAClosureRate(planepnt, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		if(workval > crossval)
		{
			crossval = workval;
		}
		useHPRF = 1;
	}


	if(crossval >= 3)
	{
		checkaspect = 1;;
	}
	if((crossval >= 3) && (inSTT))
	{
		checkaspect = 1;
	}

	if(!checkaspect)
	{
		return(FALSE);
	}
	if(useLPRF)
	{
		return(TRUE);
	}

	if(checkplane->WorldPosition.Y >= planepnt->WorldPosition.Y) return(TRUE);

	FPointDouble FromPlaneToTarget;

	FromPlaneToTarget.MakeVectorToFrom(checkplane->WorldPosition,planepnt->WorldPosition);

	FromPlaneToTarget.Normalize();  //  I think we need to do this

	FPointDouble ProjPoint;
//	ProjPoint.SetValues(-500.0*checkplane->Attitude.m_Data.RC.R0C2,-500.0*checkplane->Attitude.m_Data.RC.R1C2,
//							                     -500.0*checkplane->Attitude.m_Data.RC.R2C2);
	ProjPoint.SetValues(checkplane->Attitude.m_Data.RC.R0C2,checkplane->Attitude.m_Data.RC.R1C2, 
								checkplane->Attitude.m_Data.RC.R2C2);


	float Dot = FromPlaneToTarget * ProjPoint;

	if(radar->iGeneration == 1)
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G1 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G1;
	}
	else if(radar->iGeneration == 2)
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G2 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G2;
	}
	else
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G3 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G3;
	}

	return( fabs(Dot) > Threshold );
}

//**************************************************************************************
int AICheckPlayerBreak(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double tdist, double toffangle)
{
	float dx, dy, dz, tdist2, poffangle, tpoffangle;

	if(tdist < NMTOWU)
	{
		return(0);
	}

	poffangle = AIComputeHeadingToPoint(PlayerPlane, planepnt->WorldPosition, &tdist2, &dx ,&dy, &dz, 1);

	if(fabs(poffangle) < 90.0f)
	{
		tpoffangle = AIComputeHeadingToPoint(target, planepnt->WorldPosition, &tdist2, &dx ,&dy, &dz, 1);
		if(fabs(tpoffangle) < 45.0f)
		{
			if(poffangle < 0)  // Break Left
			{
				AIC_Wingman_BreakLRMsg(planepnt - Planes, 0);
			}
			else  //  Break Right
			{
				AIC_Wingman_BreakLRMsg(planepnt - Planes, 1);
			}
		}

		return(1);
	}

	return(0);
}

//**************************************************************************************
void AICheckForAC_130Guns(PlaneParams *planepnt, float offangle, float offpitch, float targetdist)
{
	int gunslot = -1;
	int cnt;
	int inrange = 0;
	FPointDouble gunpos;
	FPoint aimpoint;
	FPoint offset;
	FPOffSet toffset;
	float heading, pitch;
	float usetime = -1.0f;
	DBWeaponType *weapon;
	float bulletvel;
	float gundist = NMTOWU;
	float randeffect;
	float coneval = 0.3f;  //  0.25f;  //  0.5f;
	float fworkval;
	float dheading, dpitch;
	int shoot = 0;

	if(planepnt->AI.iAIFlags2 & AI_GUN_PAUSE)
	{
		if(planepnt->AI.lGunTimer >= 0)
		{
			planepnt->AI.lGunTimer -= DeltaTicks;
			if(planepnt->AI.lGunTimer > 10000)
			{
				return;
			}
		}

		if(planepnt->AI.lGunTimer < 0)
		{
			planepnt->AI.iAIFlags2 &= ~(AI_GUN_PAUSE);
			planepnt->AI.lGunTimer = 500 + (planepnt->AI.iSkill * 300);
		}
	}

	for(cnt = 0; cnt <MAX_F18E_STATIONS ; cnt ++)
	{
		if((planepnt->WeapLoad[cnt].Count > 0) && (planepnt->WeapLoad[cnt].WeapId != 75) && (planepnt->WeapLoad[cnt].WeapId != 76) && (planepnt->WeapLoad[cnt].WeapId != 95) && (planepnt->WeapLoad[cnt].WeapId != 96) && (planepnt->WeapLoad[cnt].WeapId != 97))
		{
	  		if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType == WEAPON_TYPE_GUN)
			{
				gunslot = cnt;
				break;
			}
		}
	}

	if(gunslot == -1)
	{
		planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
		planepnt->AI.lGunTimer = 360000;
		return;
	}

	weapon = &pDBWeaponList[planepnt->WeapLoad[gunslot].WeapIndex];
	gundist = weapon->iRange;
	if(gundist)
	{
		gundist *= NMTOWU;
	}
	else
	{
		gundist = NMTOWU;
	}

	bulletvel = weapon->fMaxSpeed;
	if(bulletvel == 0)
	{
		bulletvel = 3380;
	}

	toffset = pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[10];
	offset.X = toffset.X * FOOT_MULTER;
	offset.Y = toffset.Y * FOOT_MULTER;
	offset.Z = toffset.Z * FOOT_MULTER;

	gunpos.X = planepnt->WorldPosition.X + planepnt->Orientation.I.X*(double)offset.Z + planepnt->Orientation.J.X*(double)offset.X + planepnt->Orientation.K.X*(double)offset.Y;
	gunpos.Y = planepnt->WorldPosition.Y + planepnt->Orientation.I.Y*(double)offset.Z + planepnt->Orientation.J.Y*(double)offset.X + planepnt->Orientation.K.Y*(double)offset.Y;
	gunpos.Z = planepnt->WorldPosition.Z + planepnt->Orientation.I.Z*(double)offset.Z + planepnt->Orientation.J.Z*(double)offset.X + planepnt->Orientation.K.Z*(double)offset.Y;

	GDComputeGunHPGround(planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag, gunpos, &aimpoint, bulletvel, &heading, &pitch, &usetime, 0, 1);

	dheading = heading - AIConvertAngleTo180Degree(planepnt->Heading);

	dheading = AICapAngle(dheading);

	dpitch = pitch - AIConvertAngleTo180Degree(planepnt->Pitch);

	dpitch = AICapAngle(dpitch);

	if((fabs(dpitch) < 1.0f) && (fabs(dheading) < 1.0f) && (gundist > targetdist))
	{
		shoot = 1;
	}

	if((shoot) && (!(planepnt->AI.iAIFlags2 & AI_GUN_PAUSE)))
	{
		randeffect = (frand() * (coneval * 2)) - coneval;
		pitch += randeffect;
		randeffect = (frand() * (coneval * 2)) - coneval;
		heading += randeffect;

		InstantiateTurretBullet(planepnt, TRUE, planepnt->WeapLoad[gunslot].Type, AIConvert180DegreeToAngle(heading), AIConvert180DegreeToAngle(pitch), &offset);

		fworkval = (float)weapon->iRateOfFire / 60.0f;  //  Seconds per shot
		fworkval = 1000.0f / fworkval;		//  Ticks per shot

		if(fworkval <= DeltaTicks)
		{
			planepnt->WeapLoad[gunslot].Count = planepnt->WeapLoad[gunslot].Count - (DeltaTicks / fworkval);
//			planepnt->AI.fStoresWeight -= pDBWeaponList[planepnt->WeapLoad[gunslot].WeapIndex].iWeight * (DeltaTicks / fworkval);
			planepnt->AI.fStoresWeight -= (DeltaTicks / fworkval);

			planepnt->AI.lGunTimer -= DeltaTicks;
			if(planepnt->AI.lGunTimer < 0)
			{
				planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
				planepnt->AI.lGunTimer = 3000;
			}
		}
		else
		{
			planepnt->WeapLoad[gunslot].Count = planepnt->WeapLoad[gunslot].Count - 1;
//			planepnt->AI.fStoresWeight -= pDBWeaponList[planepnt->WeapLoad[gunslot].WeapIndex].iWeight;
			planepnt->AI.fStoresWeight -= 1;

			planepnt->AI.iAIFlags2 |= (AI_GUN_PAUSE);
			planepnt->AI.lGunTimer = DeltaTicks - fworkval;
		}
	}
}

//**************************************************************************************
void AIDropTanks(PlaneParams *planepnt)
{
	int cnt;

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(planepnt->WeapLoad[cnt].Count)
		{
			if(planepnt->WeapLoad[cnt].WeapId == 39)
			{
				InstantiateBomb(planepnt, cnt, (1000 * (1.0f/5.7435)), NULL, 0, -1.0f, -1.0f, -1.0f);
				planepnt->WeapLoad[cnt].Count = 0;
			}
		}
	}
}

//**************************************************************************************
void AISetFighterDisengage(PlaneParams *planepnt)
{
	FPointDouble airportpos;
	FPointDouble fptemp;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;
	PlaneParams *wingman; //, *leadplane;

	if(AIInPlayerGroup(planepnt))
	{
		return;
	}

	planepnt->AI.lTimer1 = 600000 - (planepnt->AI.iSkill * 100000);
	fptemp.SetValues(-1.0f,-1.0f,-1.0f);
	if(AIGetClosestAirField(planepnt, fptemp, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
	{
		planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(atan2(-(runwaypos.X - planepnt->WorldPosition.X), -(runwaypos.Z - planepnt->WorldPosition.Z)) * 57.2958);
	}
	else
	{
		planepnt->AI.DesiredHeading = planepnt->Heading;
	}

	planepnt->AI.Behaviorfunc = AIDisengage;

	if(planepnt->AI.wingman >= 0)
	{
		wingman = &Planes[planepnt->AI.wingman];
	}
	else if(planepnt->AI.winglead >= 0)
	{
		wingman = &Planes[planepnt->AI.winglead];
	}
	else
	{
		wingman = NULL;
	}

	if(wingman)
	{
		if(wingman->AI.Behaviorfunc == AIDisengage)
		{
			planepnt->AI.LinkedPlane = wingman;
			planepnt->AI.DesiredHeading = wingman->AI.DesiredHeading;
			planepnt->AI.Behaviorfunc = AIDisengage;
			planepnt->AI.iAIFlags2 |= (AI_IN_DOGFIGHT);
			planepnt->AI.lTimer1 = 600000 - (planepnt->AI.iSkill * 100000);
		}
		else
		{
			if(wingman->AI.iAIFlags2 & AI_IN_DOGFIGHT)
			{
				if(wingman->AI.OrgBehave)
				{
					wingman->AI.Behaviorfunc = AIAllOutAttack;
				}
			}

			planepnt->AI.LinkedPlane = NULL;
		}
	}
	else
	{
		planepnt->AI.LinkedPlane = NULL;
	}
}

//**************************************************************************************
int AIPutPlaneInTEWSCenter(PlaneParams *planepnt)
{
	if(((planepnt->AI.iAIFlags2 & AILOCKEDUP) || (planepnt->AI.iAICombatFlags2 & AI_EARLY_LOCK)) && (planepnt->AI.AirTarget == PlayerPlane))
	{
		return(1);
	}

	if(!(planepnt->AI.iAIFlags2 & (AI_CHECK_MISSILES|AILOCKEDUP)))
		return(0);

	WeaponParams *W = &Weapons[0];
	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->iTargetType == TARGET_PLANE) && (W->P == planepnt))
			{
				if(PlayerPlane == (PlaneParams *)W->pTarget)
				{
					return(1);
				}
			}
		}
		W++;
	}
	return(0);
}

//**************************************************************************************
int AICheckForGateStealerP(PlaneParams *planepnt, PlaneParams *target)
{
	float fworkvar;
	int iworkvar;
	DBRadarType *radar;
	int breaklock;

	if(MultiPlayer)
	{
		if(!((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane)))
			return(0);
	}

	if(!target)
		return(0);

	if(!(target->AI.iAICombatFlags2 & AI_GATE_STEALER))
	{
		return(0);
	}

	if(planepnt == PlayerPlane)
	{
		if( GetCurrentAARadarMode() != AA_STT_MODE )
			return(0);
	}
	else if(!((planepnt->AI.iAIFlags2 & AILOCKEDUP) || (planepnt->AI.iAICombatFlags2 & (AI_EARLY_LOCK))))
		return(0);

	if(target->Status & PL_AI_DRIVEN)
	{
		target->AI.iAICombatFlags2 |= AI_STT_THREAT;
	}

	// MAY HAVE TO DO SOMETHING HUMAN SPECIFIC
	if(target == PlayerPlane)
	{
		if(!IsICSJamming())
			return(0);
	}
	else if(!(target->AI.iAIFlags1 & AIJAMMINGON))
	{
		return(0);
	}

#if 0		//  WRONG ONE,  THIS IS NOISE.  WORK INTO MOVE MISSILE
		if(!(target->TowedDecoyState & TOWED_WANT_DEPLOYED))
		{
	if(Av.Tews.CM.CurCMDDS == -1) return;

	if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_JAM)
		Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_DEPLOY;
	else if(Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] == CM_CMDDS_DEPLOY)
		Av.Tews.CM.CMDDSStat[Av.Tews.CM.CurCMDDS] = CM_CMDDS_JAM;
			return(0);
		}
#endif

	if(planepnt->AI.lGateTimer > -99999)
	{
		if(planepnt->AI.lGateTimer >= 0)
		{
			planepnt->AI.lGateTimer -= DeltaTicks;
			if(planepnt->AI.lGateTimer < 0)
			{
				radar = GetRadarPtr(pDBAircraftList[planepnt->AI.iPlaneIndex].lRadarID);

				fworkvar = frand() * 100.0f;

				breaklock = 0;
				switch(radar->iGeneration)
				{
					case 1:
						if(fworkvar < 40.0f)
							breaklock = 1;
						break;
					case 2:
						if(fworkvar < 30.0f)
							breaklock = 1;
						break;
					default:
						if(fworkvar < 20.0f)
							breaklock = 1;
						break;
				}
				if(breaklock)
				{
					if(planepnt != PlayerPlane)
					{
						planepnt->AI.iAIFlags2 &= ~(AILOCKEDUP|AI_CHECK_MISSILES);
						GeneralSetNewAirTarget(planepnt, NULL);
						planepnt->AI.iAICombatFlags2 &= ~(AI_EARLY_LOCK);
						if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
						{
							GeneralClearCombatBehavior(planepnt);
						}
						if(planepnt->AI.lRadarDelay < 15000)
						{
							planepnt->AI.lRadarDelay = 15000;  //  15 seconds until next sweep
						}

						planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
						planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);

						if(MultiPlayer)
						{
						}
					}
					else
					{
						//  Ask Jay how to break STT lock
						PlayerSttTargetJamming();
					}
					return(1);
				}
			}
		}
	}
	else
	{
		fworkvar = frand() * 20000;
		iworkvar = target->AI.iSkill * 6000;
		planepnt->AI.lGateTimer	= 10000 + fworkvar + iworkvar;
	}

	return(0);
}