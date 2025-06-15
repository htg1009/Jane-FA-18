//*****************************************************************************************
//  CARRTANK.CPP
//*****************************************************************************************
#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"
#include "SkunkNet.h"
#include "flightfx.h"

extern AvionicsType Av;
extern WeaponParams *cplaceholder;
void setup_no_cockpit_art();
extern BOOL	g_bIAmHost;
extern void ShutBothEnginesDown(void);
extern void aic_request_takeoff(VKCODE vk);
void CTReDistributeFuelTanks();
float fBolterAdjust = 0;

//**************************************************************************************
void AIFormationFlyingLeadExact(PlaneParams *planepnt, PlaneParams *leadplane)
{
	double dx, dy, dz;
	double offangle, offangle1, toffpitch;
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
	float distmod;

	planepnt->AI.iAIFlags1 &= ~AINOFORMUPDATE;
	planepnt->AI.iAIFlags1 |= AIINFORMATION;

	leadroll = AIConvertAngleTo180Degree(leadplane->Roll);
	planeroll = AIConvertAngleTo180Degree(planepnt->Roll);
	leadheading = AIConvertAngleTo180Degree(leadplane->Heading);
	dhead = AIConvertAngleTo180Degree(planepnt->Heading - planepnt->AI.LastHeading);
	dleadhead = AIConvertAngleTo180Degree(leadplane->Heading - leadplane->AI.LastHeading);
	diffhead = AIConvertAngleTo180Degree(planepnt->Heading - leadplane->Heading);

	if(leadplane == PlayerPlane)
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

#if 0
	if(planepnt == PlayerPlane)
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
#else
	ymod = 0;
#endif

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
			distmod = 300.0f;  //  200.0f;

			if(fabs(leadroll) > 3)
			{
//				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.50))) - planepnt->WorldPosition.X;  // 100  //  200
//				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.50))) - planepnt->WorldPosition.Z;
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.750))) - planepnt->WorldPosition.X;  // 100  //  200
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.750))) - planepnt->WorldPosition.Z;
				offangle = atan2(-dx, -dz) * 57.2958;
				inturn = 1;
			}
			else if(fabs(planeroll) < 2)
			{
//				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.50))) - planepnt->WorldPosition.X;  //  100  //  200
//				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.50))) - planepnt->WorldPosition.Z;
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (distmod * FTTOWU))) - planepnt->WorldPosition.X;  //  100  //  200
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (distmod * FTTOWU))) - planepnt->WorldPosition.Z;
				offangle = atan2(-dx, -dz) * 57.2958;
			}
			else
			{
//				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (tempspeed * 0.625))) - planepnt->WorldPosition.X;  //  2000
//				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (tempspeed * 0.625))) - planepnt->WorldPosition.Z;
				dx = (planepnt->AI.WorldFormationPos.X - (leadplane->AI.WorldFormationPos.X * (distmod * FTTOWU))) - planepnt->WorldPosition.X;  //  2000
				dz = (planepnt->AI.WorldFormationPos.Z - (leadplane->AI.WorldFormationPos.Z * (distmod * FTTOWU))) - planepnt->WorldPosition.Z;
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
		if(tdistnm > 20.0)
		{
			speedmod = -200;
			dy -= (100.0f * FTTOWU);
		}
		else if(tdistnm > 5.0)
		{
			speedmod = -100;
			dy -= (100.0f * FTTOWU);
		}
		else if(tdistnm >= 2.0)
		{
			speedmod = -(20 * tdistnm);
			dy -= (100.0f * FTTOWU);
		}
		else if(tdistnm > 0.25)
		{
			speedmod = -40;
			dy -= (100.0f * FTTOWU);
		}
		else if(tdistnm > 0.05)
		{
			speedmod = -(80 * tdistnm);
		}
		else if(tdist > 10)
		{
			speedmod = -4;
		}
		else if(tdist > 2)
		{
			speedmod = -((tdist - 2) / 2);
		}
		else
		{
			speedmod = 0;
		}

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
		if(planepnt->SpeedBrakeCommandedPos != 0.0)
		{
			planepnt->SpeedBrakeCommandedPos = 0.0;
		}

		if(tdistnm > 0.05)  //  had been tdist but I don't know why
		{
			desiredspeed = leadspeed + 200;
		}
		else if(tdist > 10)
		{
			desiredspeed = leadspeed + 8;  //  4
		}
		else if(tdist > 0)  //  2
		{
			desiredspeed = leadspeed + ((tdist - 2) / 2) + 2;
		}
		else
		{
			desiredspeed = leadspeed;
		}

		if((tdistnm < 0.10) && (tdistnm > 0.05))
		{
			if(fabs(offangle1) > 5.0f)
			{
				desiredspeed = leadspeed + 8;  //  4
			}
		}

		if((tdist > 10) && (tdistnm < 0.75))
		{
			fpsspeed = AIConvertSpeedToFtPS(planepnt->Altitude, desiredspeed, AIDESIREDSPDCALC);
			ftempvar2 = sqrt((2*tdist)/planepnt->MaxSpeedAccel);
			ftempvar2 *= planepnt->MaxSpeedAccel;
			ftempvar = fpsspeed - leadplane->V;
			if(ftempvar > ftempvar2)
			{
				desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, (leadplane->V + ftempvar2), AIDESIREDSPDCALC);
			}
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
		if(fabs(offangle) < 30.0f)
		{
			if(tdistnm < 0.5f)
			{
				toffpitch = atan2(dy, (tdist + 100.0f) * FTTOWU) * 57.2958;

				if(tdistnm < 0.05f)
				{
					if(fabs(dy * WUTOFT) < 20.0f)
					{
						if(toffpitch > 2.0f)
							toffpitch = 2.0f;
						else if(toffpitch < -2.0f)
							toffpitch = -2.0f;
					}
					else
					{
						toffpitch = AIConvertAngleTo180Degree(AIGetDesiredPitchOffdy(dy, planepnt));
					}
				}

				if(tdist < 200.0f)
				{
					AIChangeDirPitch(planepnt, offangle * 3.0f, AIConvert180DegreeToAngle(toffpitch));
				}
				else
				{
					AIChangeDirPitch(planepnt, offangle, AIConvert180DegreeToAngle(toffpitch));
				}
			}
			else
			{
//				AIChangeDir(planepnt, offangle * 3.0f, (dy * WUTOFT));
				AIChangeDir(planepnt, offangle, (dy * WUTOFT));
			}
		}
		else
		{
			if(tdistnm < 0.5f)
			{
				toffpitch = atan2(dy, (tdist + 100.0f) * FTTOWU * 4.0f) * 57.2958;

				if(tdistnm < 0.05f)
				{
					if(toffpitch > 2.0f)
						toffpitch = 2.0f;
					else if(toffpitch < -2.0f)
						toffpitch = -2.0f;
				}

				AIChangeDirPitch(planepnt, offangle * 3.0f, AIConvert180DegreeToAngle(toffpitch));

			}
			else
			{
				AIChangeDir(planepnt, offangle, (dy * WUTOFT));
			}
		}
		if((planepnt->HeightAboveGround * WUTOFT) > 500)
		{
			planepnt->DesiredPitch += AIGetPlanesFlightPitch(leadplane);
		}
	}
	else
	{
		AIChangeDirInFormation(planepnt, leadplane, offangle, (dy * WUTOFT * 20.0f), dhead, dleadhead);
	}

	if(tdistnm < 0.015)
	{
		if(planepnt->DesiredRoll > 0x8000)
		{
			if(planepnt->DesiredRoll < 0xFB00)
			{
				planepnt->DesiredRoll = 0xFB00;
			}
		}
		else if(planepnt->DesiredRoll > 0x400)
		{
			planepnt->DesiredRoll = 0x400;
		}
	}
	else if(tdistnm < 0.03)
	{
		if(planepnt->DesiredRoll > 0x8000)
		{
			if(planepnt->DesiredRoll < 0xF800)
			{
				planepnt->DesiredRoll = 0xF800;
			}
		}
		else if(planepnt->DesiredRoll > 0x800)
		{
			planepnt->DesiredRoll = 0x800;
		}
	}


	if((planepnt->DesiredPitch > 0x4000) && (planepnt->DesiredPitch < 0xC000))
	{
		planepnt->DesiredPitch = (planepnt->DesiredPitch > 0x8000) ? 0xC000 : 0x4000;
	}
	return;
}

//**************************************************************************************
//**************************************************************************************
//   Tanker Stuff
//**************************************************************************************
//**************************************************************************************

//**************************************************************************************
void AICheckTankerCondition(PlaneParams *planepnt)
{
	TankerActionType *pTankerAction;
	int endtanking = 0;
	int actcnt, startact, endact;
	int tankact = -1;
	MBWayPoints *waypnt;
	PlaneParams *checkplane;

	waypnt = planepnt->AI.CurrWay;
	startact = waypnt->iStartAct;
	endact = startact + waypnt->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if(AIActions[actcnt].ActionID == ACTION_REFUEL_PATTERN)
		{
			tankact = actcnt;
			break;
		}
	}

	if(tankact == -1)
		return;

	pTankerAction = (TankerActionType *)AIActions[tankact].pAction;

	if(pTankerAction->lFlag == 0)
	{
		if(planepnt->AI.lTimer1 < 0)
		{
			endtanking = 1;
		}
	}
	else
	{
		endtanking = AICheckEventFlagStatus(pTankerAction->lFlag);
	}

	if(endtanking)
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
		}
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		AINextWayPoint(planepnt);
		CheckCurrentTacanStillValid(AIRCRAFT, planepnt);
		RetractFuelHose(planepnt,0);

		checkplane = planepnt;
		while(checkplane)
		{
			if(checkplane->AI.wingman >= 0)
			{
				RetractFuelHose(&Planes[checkplane->AI.wingman],0);
			}

			if(checkplane->AI.nextpair >= 0)
			{
				checkplane = &Planes[checkplane->AI.nextpair];
				RetractFuelHose(checkplane,0);
			}
			else
			{
				checkplane = NULL;
			}
		}
	}
}

//**************************************************************************************
void AIFlyToTankerPoint(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	int nowinglead = 0;
	int leadnum = -1;
	FPoint orbitpoint;
	float tdist, dx, dy, dz, offangle;

	DeployFuelHose(planepnt,0);

	leadplane = AIGetLeader(planepnt);

	if(leadplane == planepnt)
	{
		AIGetOrbitOffset(planepnt, &orbitpoint);
		tdist = FlyToPoint(planepnt, orbitpoint);
//		if(tdist < 1000)
		if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 25.0f))
		{
			planepnt->AI.Behaviorfunc = AIFlyTanker;
		}
		MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

		dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
		dy = planepnt->AI.WayPosition.Y - planepnt->WorldPosition.Y;
		dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

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

		AIUpdateFormation(planepnt);
	}
	else
	{
		AIFormationFlying(planepnt);
		if(leadplane->AI.Behaviorfunc == AIFlyTanker)
		{
			planepnt->AI.Behaviorfunc = AIFlyTanker;
		}
	}
}

//**************************************************************************************
void AIFlyTanker(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	int nowinglead = 0;
	float dx, dy, dz, tdist;
	int leadnum = -1;
	float ftoffset;

	DeployFuelHose(planepnt,0);

	leadplane = AIGetLeader(planepnt);
	ftoffset = planepnt->AI.lVar2;

	if(leadplane == planepnt)
	{
		AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 0);
		if((tdist * WUTOFT) >  (ftoffset * 2))
		{
			planepnt->AI.Behaviorfunc = AIFlyToTankerPoint;
			AIFlyToTankerPoint(planepnt);
			return;
		}
		if(planepnt->AI.iAICombatFlags2 & AI_CARRIER_REL)
		{
			if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
			{
				MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

				double dx2 = (carrier->WorldPosition.X) - planepnt->AI.WayPosition.X;
				double dz2 = (carrier->WorldPosition.Z) - planepnt->AI.WayPosition.Z;
				double tdist2 = QuickDistance(dx2, dz2) * WUTONM;

				tdist2 -= 10.0f;
				if(fabs(tdist2) > 3.0f)
				{
					planepnt->AI.WayPosition.X = carrier->WorldPosition.X;
					planepnt->AI.WayPosition.Z = carrier->WorldPosition.Z;
					AIOffsetFromPoint(&planepnt->AI.WayPosition, AIConvertAngleTo180Degree(carrier->Heading + 0x6000), 0, 0, -10.0f * NMTOWU);

					planepnt->AI.Behaviorfunc = AIFlyToTankerPoint;
					AIFlyToTankerPoint(planepnt);
					return;
				}
			}
		}

		planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
		MBAdjustAIThrust(planepnt, 300, 1);
		dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
		AICheckTankerCondition(planepnt);
		AIUpdateFormation(planepnt);
	}
	else
	{
		AIFormationFlying(planepnt);
//		AICheckTankerCondition(planepnt);
		planepnt->AI.Behaviorfunc = leadplane->AI.Behaviorfunc;
		planepnt->AI.OrgBehave = leadplane->AI.OrgBehave;
	}
}

//**************************************************************************************
void AITankingBreakaway(PlaneParams *planepnt)
{
	planepnt->DesiredPitch = 0x800;
	planepnt->DesiredRoll = 0;
	MBAdjustAIThrust(planepnt, 300, 1);
	AIChangeFXArray(planepnt, 0, 86, 5000);
	AIChangeFXArray(planepnt, 1, 0, 4000);

	if(planepnt->AI.lTimer2 < 0)
	{
		planepnt->AI.Behaviorfunc = AITankingFlight;
	}
}

//**************************************************************************************
void AITankingFlight(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist, tdistnm, rollval;
	double angzx, angzz;
	double heading;

	DeployFuelHose(planepnt,0);

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	tdistnm = (tdist * WUTONM);

	if(tdistnm > 75)
	{
		planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(offangle);
	}

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	if(fabs(planepnt->AI.LinkedPlane->AI.TargetPos.Z) < 2000)
	{
		if(planepnt->AI.TargetPos.Y > 0)
		{
			AICheckOffBoom(planepnt->AI.LinkedPlane, planepnt);
		}
		else
		{
			AICheckForOnRequest(planepnt);
		}
	}
	else
	{
		AICheckMilesOffBoom(planepnt);
	}

	if(planepnt->AI.LinkedPlane == PlayerPlane)
	{
		if(PlayerPlane->RefuelProbeCommandedPos > 10.0f)
		{
			if(planepnt->AI.LinkedPlane->AI.TargetPos.Z < 6076)
			{
				if(!(iAIHumanTankerFlags & AI_TANKER_PROBE_OK))
				{
					AIChangeGroupBehavior(planepnt->AI.LinkedPlane, AIFlyFormation, AIFlyTankerFormation);
				}

				iAIHumanTankerFlags |= AI_TANKER_PROBE_OK;
			}
		}
		else if((PlayerPlane->RefuelProbeCommandedPos < 1.0f) && (iAIHumanTankerFlags & AI_TANKER_PROBE_OK))
		{
			iAIHumanTankerFlags = AI_TANKER_P_DONE;
			
			CTGetNextToTank(planepnt->AI.LinkedPlane, planepnt);
		}
	}


	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 15)
	{
		rollval = (rollval < 0.0) ? -15 : 15;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.30;	//  max roll 90.0

}

//**************************************************************************************
void AITankingFlight2(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	AIChangeFXArray(planepnt, 0, 0xFF, 5000);
	AIChangeFXArray(planepnt, 1, 0xFF, 4000);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	if((fabs(planepnt->AI.LinkedPlane->AI.TargetPos.Z) < 10) && (fabs(planepnt->AI.LinkedPlane->AI.TargetPos.Y) < 10))
	{
		if(planepnt->AI.lTimer2 > 3000)
		{
			planepnt->AI.lTimer2 = 3000;
			if(SndStreamInit("clunk.wav"))
			{
				SndStreamPlay();
			}
		}
		else if(planepnt->AI.lTimer2 < 0)
		{
			AISetUpTankerFlyBy(planepnt);
		}
	}
	else
	{
		if(planepnt->AI.lTimer2 > 3000)
		{
			planepnt->AI.lTimer2 = 10000;
		}
	}
	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);
}

//**************************************************************************************
void AISetUpTankerFlyBy(PlaneParams *planepnt)
{
	planepnt->AI.lTimer2 = 6000;
	planepnt->AI.Behaviorfunc = AITankingFlight3;

#if 0
	float angzx, angzz;

	setup_no_cockpit_art();

	angzx = (double)planepnt->AI.WorldFormationPos.X;
	angzz = (double)planepnt->AI.WorldFormationPos.Z;

//	Camera1.CameraLocation.X = planepnt->WorldPosition.X + ConvertWayLoc((angzz * 250) + (angzx * -2600));
//	Camera1.CameraLocation.Y = planepnt->WorldPosition.Y + ConvertWayLoc(25);
//	Camera1.CameraLocation.Z = planepnt->WorldPosition.Z + ConvertWayLoc(-(angzx * 250) + (angzz * -2600));
//
	AssignCameraSubject(planepnt,CO_PLANE);

	ChangeViewModeOrSubject(CAMERA_RC_VIEW);
#else
	setup_no_cockpit_art();

	FMatrix offm;
	FPointDouble offset;

//	offm.SetRPH(0,0xF800 - (ANGLE)(rand() & 0x7FF),0xF000 - (ANGLE)(rand() & 0x7FF));
	offm.SetRPH(0,0x100, 0xF800);
	offset.SetValues( HIGH_FREQ * FTTOWU * 150.0, planepnt->IfVelocity ); //3 seconds
	offset *= offm;
	offset += planepnt->WorldPosition;
	camera_setup_initial_location(offset);

	offset.MakeVectorToFrom(PlayerPlane->WorldPosition,Camera1.CameraLocation);

	Camera1.Roll = 0;
	Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
	Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

	camera_setup_initial_relative_quats();
	Camera1.SubType = 0;
	Camera1.CameraMode = CAMERA_RC_VIEW;
#endif


	AICReceivingFuel(planepnt - Planes);
}

//**************************************************************************************
void AITankingFlight3(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	if(planepnt->AI.lTimer2 < 0)
	{
		planepnt->AI.Behaviorfunc = AITankingFlight4;
		AISetUpTankerReturnView(planepnt);
	}
	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);
}

//**************************************************************************************
void AISetUpTankerReturnView(PlaneParams *planepnt)
{
//	float offangle, dx, dy, dz, tdist;
	float tdist;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	setup_no_cockpit_art();

	planepnt->AI.lTimer2 = 4000;

	if(planepnt == PlayerPlane)  //  Playerized for demo SRE
	{
		AICEndTanking(planepnt, planepnt->AI.lTimer2);
	}

	PlaneParams *P = planepnt->AI.LinkedPlane;

	if (!P)
		AssignCameraSubject(PlayerPlane,CO_PLANE);
	else
		AssignCameraSubject(P,CO_PLANE);

	Camera1.CameraMode = CAMERA_FIXED_CHASE;
//	ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);

#if 0
	offangle = AIComputeHeadingToPoint(P, Camera1.CameraLocation, &tdist, &dx ,&dy, &dz, 0);
#endif

	tdist = P->WorldPosition - Camera1.CameraLocation;
	Camera1.DistanceFromObject = tdist;
	planepnt->AI.TargetPos.Z = tdist / planepnt->AI.lTimer2;
#if 0
	Camera1.Heading = AIConvert180DegreeToAngle(offangle) + 0x8000;
	Camera1.Pitch = 0;
	Camera1.Roll = 0;

	camera_setup_initial_relative_quats();
#endif
}

void virtual_free_look( VKCODE vk );
//**************************************************************************************
void AITankingFlight4(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;
	PlaneParams *tanking;
	float closedist;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	if(planepnt->AI.lTimer2 < 0)
	{
		planepnt->AI.Behaviorfunc = AITankingFlight5;

#if 0
		Camera1.CameraMode = CAMERA_COCKPIT;
		Camera1.SubType = COCKPIT_FRONT;
		LoadCockpit(FRONT_FORWARD_COCKPIT);
		SetupView( Camera1.SubType );
		CockpitSeat = FRONT_SEAT;
		OurShowCursor( TRUE );
#else
		VKCODE vk;
		vk.dwExtraCode = 0;
		vk.vkCode = 0;
		vk.wFlags = 0;
		virtual_free_look(vk);
#endif
		RefuelFuelPlane(planepnt->AI.LinkedPlane, planepnt->AI.cUseWeapon);

#if 0
		// Old
		Camera1.SubType = COCKPIT_FRONT;
		Camera1.CameraMode = CAMERA_COCKPIT;
		LoadCockpit(FRONT_FORWARD_COCKPIT);
		SetupView(COCKPIT_FRONT|COCKPIT_NOART);
		CockpitSeat = FRONT_SEAT;
#endif

		if(planepnt->AI.LinkedPlane == NULL)
			return;

		tanking = planepnt->AI.LinkedPlane;
		if(tanking != NULL)
		{
			tanking->AI.Behaviorfunc = AITankerDisconnect;
			if((MultiPlayer) && ((tanking->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (tanking == PlayerPlane)))
			{
				NetPutGenericMessage1(tanking, GM_TANKER_DISCO_HUMAN);
			}

			int iNumFlight=-1;

			if (tanking == PlayerPlane)
			{
				iNumFlight=1;
			}
			if (tanking->AI.iAIFlags1&AIPLAYERGROUP)
			{
				iNumFlight = (planepnt->AI.iAIFlags1&AIFLIGHTNUMS); // 0 - 7
				iNumFlight++;
			}

//			if (iNumFlight!=-1)
//			{
//				LogMissionFlag(FLIGHT_REFUELED,iNumFlight,1,2);
//				LogMissionFlag(FLIGHT1_REFUEL_TIME,iNumFlight,GameLoop,0);
//			}
		}
		planepnt->AI.lTimer2 = 15000;
	}
	else
	{
		tanking = (PlaneParams *)PlayerPlane;

		closedist = 2.0f * *(float *)((int)tanking->Type->Model + sizeof(ObjectHandler));
		Camera1.TargetDistanceFromObject -= planepnt->AI.TargetPos.Z * DeltaTicks;

		if(Camera1.TargetDistanceFromObject < closedist)
			Camera1.TargetDistanceFromObject = closedist;
	}

	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);
}

//**************************************************************************************
void AITankerDisconnect(PlaneParams *planepnt)
{
	MBAdjustAIThrust(planepnt, 280, 1);
	planepnt->DesiredRoll = 0;
	planepnt->DesiredPitch = 0;
}

//**************************************************************************************
void AITankingFlight5(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;
	PlaneParams *tanking;
	PlaneParams *checkplane;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	AIChangeFXArray(planepnt, 0, 86, 5000);
	AIChangeFXArray(planepnt, 1, 0, 4000);
	planepnt->AI.cFXarray[2] = 0;
	planepnt->AI.cFXarray[3] = 0;

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	if(planepnt->AI.lTimer2 < 0)
	{
		AIResetTanker(planepnt);
	}
	else
	{
		if(planepnt->AI.lTimer2 < 10000)
		{
			if(planepnt->AI.LinkedPlane == NULL)
				return;

			tanking = planepnt->AI.LinkedPlane;
			if(tanking != NULL)
			{
				if(tanking->AI.Behaviorfunc == AITankerDisconnect)
				{
					if((MultiPlayer) && (tanking->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
//						NetPutGenericMessage4(tanking, GM4_DISENGAGE, 2, 0, 0);
						NetPutGenericMessage1(tanking, GM_TANKER_DISCO_HUMAN2);
					}

					tanking->AI.Behaviorfunc = AIFlyFormation;
					AIChangeGroupBehavior(tanking, AIFlyTankerFormation, AIFlyFormation);
					tanking->AI.lVar2 = tanking->AI.fVarA[0];
					tanking->Status |= AL_DEVICE_DRIVEN;
					tanking->Status &= ~AL_AI_DRIVEN;
					tanking->DoControlSim = CalcF18ControlSurfaces;
					tanking->DoCASSim     = CalcF18CASUpdates;
					tanking->DoForces     = CalcAeroForces;
					tanking->DoPhysics    = CalcAeroDynamics;
					tanking->dt = HIGH_FREQ;
					tanking->UpdateRate = HIGH_AERO;
					tanking->AI.iAIFlags2 &= ~(AI_FAKE_AOA);

					if(tanking != PlayerPlane)
					{
						tanking->RefuelProbeCommandedPos = 0.0f;
					}

					//  **  WILL WANT TO REFUEL WINGMEN HERE
					checkplane = tanking;
					while(checkplane)
					{
						if(checkplane->AI.wingman >= 0)
						{
							RefuelFuelPlane(&Planes[checkplane->AI.wingman], planepnt->AI.cUseWeapon);
						}
						if(checkplane->AI.nextpair >= 0)
						{
							checkplane = &Planes[checkplane->AI.nextpair];
							RefuelFuelPlane(checkplane, planepnt->AI.cUseWeapon);
						}
						else
						{
							checkplane = NULL;
						}
					}
				}
			}
		}
	}

	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);
}

//**************************************************************************************
void AIResetTanker(PlaneParams *planepnt)
{
	float fworkvar, degspersec;

	if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutGenericMessage2(planepnt, GM2_DONE_TANKING, 255);
	}

	planepnt->AI.lVar2 = 2.5 * NMTOFT;
	planepnt->AI.lDesiredSpeed = 300;
	planepnt->AI.Behaviorfunc = AIFlyToTankerPoint;
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

//**************************************************************************************
void AICheckMilesOffBoom(PlaneParams *planepnt)
{
	int itempvar, itempvar2;
	char diststr[50];
	char tempstr[1024];

	if(planepnt->AI.LinkedPlane == NULL)
	{
		return;
	}

	AIChangeFXArray(planepnt, 0, 0xFF, 5000);
	AIChangeFXArray(planepnt, 1, 0xFF, 4000);

	itempvar = fabs(planepnt->AI.LinkedPlane->AI.TargetPos.Z) * FTTONM;
	itempvar2 = (float)planepnt->AI.lVar2 * FTTONM;

	if(itempvar > 4)
	{
		return;
	}

	if(itempvar < 2)
	{
		if(fabs(planepnt->AI.LinkedPlane->AI.TargetPos.Z) < 10000)
		{
			if((!UFC.EMISState) && (planepnt == PlayerPlane))  //  Playerized for demo SRE
			{
				UFCToggleEMIS();
				if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_EMIS_SELECTED, g_iLanguageId))
				{
					return;
				}
				AICAddAIRadioMsgs(tempstr, 40);
				AIRGenericSpeech(AICF_WSO_EMIS_SELECTED, (planepnt->AI.LinkedPlane - Planes));
			}
			AICheckClosure(planepnt->AI.LinkedPlane, planepnt, 50, 10000);
		}
	}
	else
	{
		AICheckClosure(planepnt->AI.LinkedPlane, planepnt, 150, 20000);
	}

	if((itempvar < itempvar2) || ((itempvar2 + 2) < itempvar))
	{
		if(planepnt == PlayerPlane)  //  Playerized for demo SRE
		{
			if(itempvar == 0)
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_MILE_TO_TARGET, GetUserDefaultLangID()))
				{
					return;
				}
			}
			else
			{
				if(!LANGGetTransMessage(diststr, 50, AIC_ZERO + (itempvar + 1), g_iLanguageId))
				{
					return;
				}
				if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_MILES_TO_TARGET, g_iLanguageId, diststr))
				{
					return;
				}
			}
			AIRGenericSpeech(AICF_WSO_MILES, (planepnt->AI.LinkedPlane - Planes), 0, 0, 0, 0, itempvar + 1);
			AICAddAIRadioMsgs(tempstr, 40);
		}
		planepnt->AI.lVar2 = ((float)itempvar * NMTOFT) + 200;  //  + 200 kludge for loss of precision.
	}
}

//**************************************************************************************
void AICheckOffBoom(PlaneParams *planepnt, PlaneParams *tanker)
{
	int xbox = 10;
	int ybox = 20;
	int dist = 20;
	int onlydist = 0;
	float workdist;
	char tempstr[1024];
	int itempvar, itempvar2;
	int randval;
	BYTE arrayval;
	int difflevel = 0;
	int autotank = 0;
	int probemate = 0;
	int canned = ((planepnt->Status & PL_AI_DRIVEN) || (!(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_DURATION))) ? 1 : 0;

	if((planepnt == NULL) || (tanker == NULL))
		return;

	if(tanker->FlightStatus & PL_OUT_OF_CONTROL)
	{
		return;
	}

	if(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_TOLERANCE)
	{
		difflevel = GP_AUTHENTIC;
	}
	else
	{
		difflevel = GP_ARCADE;
	}

	xbox <<= difflevel;
	ybox <<= difflevel;
	dist <<= difflevel;

	if((fabs(planepnt->AI.TargetPos.X) < 65) && ((planepnt->AI.TargetPos.Z < 10) && (planepnt->AI.TargetPos.Z > -140)) && ((planepnt->AI.TargetPos.Y < -10) && (planepnt->AI.TargetPos.Y > -70)))
	{
		tanker->AI.Behaviorfunc = AITankingBreakaway;
		if(planepnt == PlayerPlane)  //  Playerized for demo SRE
		{
			AICTankerBreakAway(planepnt - Planes, tanker - Planes);
		}
	}

	if((fabs(planepnt->AI.TargetPos.Z) > dist) || (tanker->AI.lTimer2 > 3000))
	{
		arrayval = tanker->AI.cFXarray[0];

		if((planepnt->AI.TargetPos.Y < -8) || (planepnt->AI.TargetPos.X > 5))
		{
			AIChangeFXArray(tanker, 0, 86, 5000);
		}
		else if((arrayval < 158) || (arrayval > 182))
		{
			AIChangeFXArray(tanker, 0, 170, 5000);
		}
		else if(arrayval & 1)
		{
			if(arrayval < 162)
			{
				randval = rand()&1;
				switch(randval)
				{
					case 0:
						tanker->AI.cFXarray[0] = tanker->AI.cFXarray[0] - 2;
						break;
					case 2:
						tanker->AI.cFXarray[0] = tanker->AI.cFXarray[0] + 1;
						break;
				}
				if(tanker->AI.cFXarray[0] < 158)
				{
					tanker->AI.cFXarray[0] = 158;
				}
			}
			else
			{
				tanker->AI.cFXarray[0] = tanker->AI.cFXarray[0] - 2;
			}
		}
		else
		{
			if(arrayval > 177)
			{
				randval = rand()&1;
				switch(randval)
				{
					case 0:
						tanker->AI.cFXarray[0] = tanker->AI.cFXarray[0] + 2;
						break;
					case 2:
						tanker->AI.cFXarray[0] = tanker->AI.cFXarray[0] - 1;
						break;
				}
				if(tanker->AI.cFXarray[0] > 182)
				{
					tanker->AI.cFXarray[0] = 181;
				}
			}
			else
			{
				tanker->AI.cFXarray[0] = tanker->AI.cFXarray[0] + 2;
			}
		}
		if((planepnt->AI.TargetPos.Y < -8) || (planepnt->AI.TargetPos.X > 5) || (fabs(planepnt->AI.TargetPos.Z) > 70.0f))
		{
			AIChangeFXArray(tanker, 1, 86, 4000);
		}
		else
		{
			AIChangeFXArray(tanker, 1, 0xFF, 4000);
		}
	}
	else
	{
		AIChangeFXArray(tanker, 0, 0xFF, 5000);
		AIChangeFXArray(tanker, 1, 0xFF, 4000);
	}

	if(!(planepnt->Status & AL_AI_DRIVEN))
	{
		if(0 <= planepnt->AI.lTimer2)
		{
			planepnt->AI.lTimer2 -= DeltaTicks;
		}
	}

#if 0
	if(planepnt->AI.TargetPos.Z < 0)
	{
		if(planepnt->AI.lTimer2 < 0)
		{
			planepnt->AI.lTimer2 = 5000 + ((rand() & 3) * 1000);
			if(planepnt == PlayerPlane)  //  Playerized for demo SRE
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_SLOWER, g_iLanguageId))
				{
					return;
				}
				AIRGenericSpeech(AICF_WSO_SLOWER, (planepnt - Planes));
				AICAddAIRadioMsgs(tempstr, 40);
			}
		}
	}
#endif

	if((!UFC.EMISState) && (planepnt->AI.lTimer2 < 0) && (!Av.AARadar.SilentOn))
	{
		planepnt->AI.lTimer2 = 5000 + ((rand() & 3) * 1000);
		if(planepnt == PlayerPlane)  //  Playerized for demo SRE
		{
			AICTankerCheckSwitches(tanker - Planes);
		}
	}

	AISetTankerLights(planepnt, tanker);

	if((planepnt->Status & AL_AI_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN))
	{
		onlydist = 1;
	}
	else if(fabs(planepnt->AI.TargetPos.X) > xbox)
	{
//		tanker->AI.lTimer2 = 10000 / difflevel;
		tanker->AI.lTimer2 = 5000;
		if(planepnt->AI.lTimer2 < 0)
		{
			planepnt->AI.lTimer2 = 5000;
			if(planepnt->AI.TargetPos.X < 0)
			{
				if(!(iAIHumanTankerFlags & AI_TANKER_LEFT))
				{
					iAIHumanTankerFlags &= ~(AI_TANKER_UP|AI_TANKER_DOWN|AI_TANKER_RIGHT|AI_TANKER_LEFT);
					if(lAdvisorFlags & ADV_CASUAL_BETTY)
					{
						AICGenericBettySpeech(BETTY_LEFT);
						if(!LANGGetTransMessage(tempstr, 1024, AIC_LEFT, g_iLanguageId))
						{
							return;
						}
	//					AIRGenericSpeech(AICF_LEFT, (planepnt - Planes));
						AICAddAIRadioMsgs(tempstr, 40);
					}
					iAIHumanTankerFlags |= AI_TANKER_LEFT;
				}
			}
			else
			{
				if(!(iAIHumanTankerFlags & AI_TANKER_RIGHT))
				{
					iAIHumanTankerFlags &= ~(AI_TANKER_UP|AI_TANKER_DOWN|AI_TANKER_RIGHT|AI_TANKER_LEFT);
					if(lAdvisorFlags & ADV_CASUAL_BETTY)
					{
						AICGenericBettySpeech(BETTY_RIGHT);
						if(!LANGGetTransMessage(tempstr, 1024, AIC_RIGHT, g_iLanguageId))
						{
							return;
						}
	//					AIRGenericSpeech(AICF_RIGHT, (planepnt - Planes));
						AICAddAIRadioMsgs(tempstr, 40);
					}
					iAIHumanTankerFlags |= AI_TANKER_RIGHT;
				}
			}
		}
	}
	else if(planepnt->AI.TargetPos.Y < 0)
	{
//		tanker->AI.lTimer2 = 10000 / difflevel;
		tanker->AI.lTimer2 = 5000;
		if(planepnt->AI.lTimer2 < 0)
		{
			planepnt->AI.lTimer2 = 5000;
			if(!(iAIHumanTankerFlags & AI_TANKER_DOWN))
			{
				iAIHumanTankerFlags &= ~(AI_TANKER_UP|AI_TANKER_DOWN|AI_TANKER_RIGHT|AI_TANKER_LEFT);
				if(lAdvisorFlags & ADV_CASUAL_BETTY)
				{
					AICGenericBettySpeech(BETTY_DOWN);
					if(!LANGGetTransMessage(tempstr, 1024, AIC_DOWN, g_iLanguageId))
					{
						return;
					}
	//				AIRGenericSpeech(AICF_DOWN, (planepnt - Planes));
					AICAddAIRadioMsgs(tempstr, 40);
				}
			 	iAIHumanTankerFlags |= AI_TANKER_DOWN;
			}
		}
	}
	else if(planepnt->AI.TargetPos.Y > ybox)
	{
//		tanker->AI.lTimer2 = 10000 / difflevel;
		tanker->AI.lTimer2 = 5000;
		if(planepnt->AI.lTimer2 < 0)
		{
			planepnt->AI.lTimer2 = 5000;
			if(!(iAIHumanTankerFlags & AI_TANKER_UP))
			{
				iAIHumanTankerFlags &= ~(AI_TANKER_UP|AI_TANKER_DOWN|AI_TANKER_RIGHT|AI_TANKER_LEFT);
				if(lAdvisorFlags & ADV_CASUAL_BETTY)
				{
					if(!LANGGetTransMessage(tempstr, 1024, AIC_UP, g_iLanguageId))
					{
						return;
					}
					AICGenericBettySpeech(BETTY_UP);
	//				AIRGenericSpeech(AICF_UP, (planepnt - Planes));
					AICAddAIRadioMsgs(tempstr, 40);
				}
			 	iAIHumanTankerFlags |= AI_TANKER_UP;
			}
		}
	}
	else
	{
		if(iAIHumanTankerFlags & (AI_TANKER_UP|AI_TANKER_DOWN|AI_TANKER_RIGHT|AI_TANKER_LEFT))
		{
			iAIHumanTankerFlags &= ~(AI_TANKER_UP|AI_TANKER_DOWN|AI_TANKER_RIGHT|AI_TANKER_LEFT);
			planepnt->AI.lTimer2 = 5000;
			AIC_WSO_Looking_Good_Msg(PlayerPlane - Planes);
		}
		onlydist = 1;
	}

	if(fabs(planepnt->AI.TargetPos.Z) > 150)  //  was .Z - dist)
		return;

	workdist = planepnt->AI.TargetPos.Z;  //   - dist;
	if(workdist >= 100)
	{
		planepnt->AI.lVar2 = 105;
	}
	else if(workdist > 7)
	{
		itempvar = workdist / 10;
		itempvar2 = planepnt->AI.lVar2 / 10;

		if((itempvar < itempvar2) || ((itempvar2 + 2) < itempvar))
		{
			if(planepnt == PlayerPlane)  //  Playerized for demo SRE
			{
#if 1
				if(itempvar < 6)
				{
					if(lAdvisorFlags & ADV_CASUAL_BETTY)
					{
						if(!LANGGetTransMessage(tempstr, 1024, AIC_TEN + (itempvar * 10), g_iLanguageId))
						{
							return;
						}
						AICGenericBettySpeech(BETTY_COUNT, 0, (itempvar + 1));
						AICAddAIRadioMsgs(tempstr, 40);
					}
				}
#else
				if(itempvar < 7)
				{
					if(!LANGGetTransMessage(tempstr, 1024, AIC_TEN + (itempvar * 10), g_iLanguageId))
					{
						return;
					}
				}
				else
				{
					if(!LANGGetTransMessage(tempstr, 1024, AIC_EIGHTY + ((itempvar - 7) * 2), g_iLanguageId))
					{
						return;
					}
				}
				AIRGenericSpeech(AICF_WSO_FT_DIST, (planepnt - Planes), 0, 0, 0, 0, (itempvar + 1) * 10);
				AICAddAIRadioMsgs(tempstr, 40);
#endif
			}
			planepnt->AI.lVar2 = itempvar * 10;
		}
		AICheckClosure(planepnt, tanker, 15, 5000);
	}

//	if((pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z*FOOT_MULTER) > planepnt->Type->Offsets[OFFSET_COCKPIT_FRONT].Z)
//	{
//		if(fabs(planepnt->AI.TargetPos.Z) < (10.0f * FTTOWU))
//		{
			probemate = AICheckProbeDroguePos(planepnt, tanker);
//		}

		if(!(planepnt->AI.lVar3 & 1))
		{
			if(probemate > 0)
			{
				autotank = 1;
			}
			else if(probemate < 0)
			{
				planepnt->AI.lVar3 |= 1;
			}
		}
		else if(!probemate)
		{
			planepnt->AI.lVar3 &= ~(0x01);
		}
//	}
//	else
//	{
//		if((fabs(planepnt->AI.TargetPos.Z) < dist) && (onlydist))
//		{
//			if(tanker->AI.lTimer2 < 0)
//			{
//				autotank = 1;
//			}
//	//		else if((long)(tanker->AI.lTimer2 + DeltaTicks) >= (long)(10000 / difflevel))
//			else if((long)(tanker->AI.lTimer2 + DeltaTicks) >= (long)(5000))
//			{
//				if(planepnt == PlayerPlane)  //  Playerized for demo SRE
//				{
//					if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_STABLIZE, g_iLanguageId))
//					{
//						return;
//					}
//					AIRGenericSpeech(AICF_TANKER_STABLIZE, (tanker - Planes));
//					AICAddAIRadioMsgs(tempstr, 40);
//				}
//			}
//		}
//		else
//		{
//	//		tanker->AI.lTimer2 = 10000 / difflevel;
//			tanker->AI.lTimer2 = 5000;
//		}
//	}

	if((autotank) && ((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane)))
	{
		if(planepnt == PlayerPlane)
		{
			LogMissionStatistic(LOG_FLIGHT_REFUELED,(planepnt-Planes),1,0);
			LogMissionStatistic(LOG_FLIGHT_REFUEL_TIME,(planepnt-Planes),GameLoop,0);

			if(canned)
			{
				if(MultiPlayer)
				{
					NetPutGenericMessage2(planepnt, GM2_AUTOTANK_1, (tanker - Planes));
				}

				if(planepnt->Status & AL_DEVICE_DRIVEN)
				{
					double aoadeg, cosroll, aoaoffset;

					aoadeg = RadToDeg(planepnt->Alpha);
					cosroll = cos(DegToRad(fabs(AIConvertAngleTo180Degree(planepnt->Roll))));
					aoaoffset = cosroll * aoadeg;
					planepnt->AI.AOADegPitch = AIConvert180DegreeToAngle(aoaoffset);
					planepnt->AI.iAIFlags2 |= (AI_FAKE_AOA);
				}
				planepnt->Status &= ~AL_DEVICE_DRIVEN;
				planepnt->Status |= AL_AI_DRIVEN;
				planepnt->DoControlSim = CalcF18ControlSurfacesSimple;
				planepnt->DoCASSim = CalcF18CASUpdatesSimple;
				planepnt->DoForces = CalcAeroForcesSimple;
				planepnt->DoPhysics = CalcAeroDynamicsSimple;
				planepnt->dt = 0;
				planepnt->UpdateRate = HIGH_AERO;


				planepnt->MaxPitchRate = 10.0;
				planepnt->MaxRollRate = 90.0;
				if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
				{
					planepnt->MaxRollRate /= 3.0f;
				}
				planepnt->YawRate = pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate;

				planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration;	//15
				planepnt->SpeedPercentage = 1.0;

				planepnt->BfLinVel.X = planepnt->V;
				planepnt->BfLinVel.Y = 0;
				planepnt->BfLinVel.Z = 0;
				tanker->AI.lTimer2 = 10000;
				tanker->AI.Behaviorfunc = AITankingFlight2;
			}
			else
			{
				tanker->AI.Behaviorfunc = AITankingFlight2HumanLong;
			}
		}
		else
		{
			tanker->AI.Behaviorfunc = AITankingFlight2AI;
			tanker->AI.lTimer2 = 60000;
		}

		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))
		{
			AttachPlanePointToRefuelingHose(tanker,(DWORD)planepnt,0);
			if(MultiPlayer)
			{
				NetPutGenericMessage2(tanker, GM2_ATTACH_HOSE, (planepnt - Planes));
			}
		}
	}
}

//**************************************************************************************
void AISetTankerLights(PlaneParams *planepnt, PlaneParams *tanker)
{
	int xbox = 10;
	int ybox = 20;
	int dist = 20;
	int difflevel = 0;  // 1 hardest, 3 easiest;
	int udok = 0;
	int faok = 0;
	int workval;

	if((planepnt == NULL) || (tanker == NULL))
		return;

	if(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_TOLERANCE)
	{
		difflevel = GP_AUTHENTIC;
	}
	else
	{
		difflevel = GP_ARCADE;
	}

	xbox <<= difflevel;
	ybox <<= difflevel;
	dist <<= difflevel;

	if(planepnt->AI.TargetPos.Y < 0)
	{
		workval = planepnt->AI.TargetPos.Y;
		if(workval > -5)
		{
			tanker->AI.cFXarray[2] = (TANKER_DOWN|TANKER_UD_OK|TANKER_U_UP|TANKER_D_DOWN);
		}
		else if(workval > -10)
		{
			tanker->AI.cFXarray[2] = (TANKER_DOWN|TANKER_U_UP|TANKER_D_DOWN);
		}
		else if(workval > -15)
		{
			tanker->AI.cFXarray[2] = (TANKER_WAY_DOWN|TANKER_DOWN|TANKER_U_UP|TANKER_D_DOWN);
		}
		else if(workval > -20)
		{
			tanker->AI.cFXarray[2] = (TANKER_WAY_DOWN|TANKER_U_UP|TANKER_D_DOWN);
		}
		else
		{
			tanker->AI.cFXarray[2] = (TANKER_U_UP|TANKER_D_DOWN);
		}
	}
	else if(planepnt->AI.TargetPos.Y > ybox)
	{
		workval = planepnt->AI.TargetPos.Y - ybox;
		if(workval < 5)
		{
			tanker->AI.cFXarray[2] = (TANKER_UP|TANKER_UD_OK|TANKER_U_UP|TANKER_D_DOWN);
		}
		else if(workval < 10)
		{
			tanker->AI.cFXarray[2] = (TANKER_UP|TANKER_U_UP|TANKER_D_DOWN);
		}
		else if(workval < 15)
		{
			tanker->AI.cFXarray[2] = (TANKER_WAY_UP|TANKER_UP|TANKER_U_UP|TANKER_D_DOWN);
		}
		else if(workval < 20)
		{
			tanker->AI.cFXarray[2] = (TANKER_WAY_UP|TANKER_U_UP|TANKER_D_DOWN);
		}
		else
		{
			tanker->AI.cFXarray[2] = (TANKER_U_UP|TANKER_D_DOWN);
		}
	}
	else
	{
		tanker->AI.cFXarray[2] = (TANKER_UD_OK|TANKER_U_UP|TANKER_D_DOWN);
	}

	if(fabs(planepnt->AI.TargetPos.Z) < dist)
	{
		tanker->AI.cFXarray[3] = (TANKER_FA_OK|TANKER_F_FORE|TANKER_A_AFT);
	}
	else if(planepnt->AI.TargetPos.Z > 0)
	{
		workval = planepnt->AI.TargetPos.Z - dist;
		if(workval < 5)
		{
			tanker->AI.cFXarray[3] = (TANKER_AFT|TANKER_FA_OK|TANKER_F_FORE|TANKER_A_AFT);
		}
		else if(workval < 10)
		{
			tanker->AI.cFXarray[3] = (TANKER_AFT|TANKER_F_FORE|TANKER_A_AFT);
		}
		else if(workval < 15)
		{
			tanker->AI.cFXarray[3] = (TANKER_WAY_AFT|TANKER_AFT|TANKER_F_FORE|TANKER_A_AFT);
		}
		else if(workval < 20)
		{
			tanker->AI.cFXarray[3] = (TANKER_WAY_AFT|TANKER_F_FORE|TANKER_A_AFT);
		}
		else
		{
			tanker->AI.cFXarray[3] = (TANKER_F_FORE|TANKER_A_AFT);
		}
	}
	else
	{
		workval = planepnt->AI.TargetPos.Z + dist;
		if(workval > -5)
		{
			tanker->AI.cFXarray[3] = (TANKER_FORE|TANKER_FA_OK|TANKER_F_FORE|TANKER_A_AFT);
		}
		else if(workval > -10)
		{
			tanker->AI.cFXarray[3] = (TANKER_FORE|TANKER_F_FORE|TANKER_A_AFT);
		}
		else if(workval > -15)
		{
			tanker->AI.cFXarray[3] = (TANKER_WAY_FORE|TANKER_FORE|TANKER_F_FORE|TANKER_A_AFT);
		}
		else if(workval > -20)
		{
			tanker->AI.cFXarray[3] = (TANKER_WAY_FORE|TANKER_F_FORE|TANKER_A_AFT);
		}
		else
		{
			tanker->AI.cFXarray[3] = (TANKER_F_FORE|TANKER_A_AFT);
		}
	}
}

//**************************************************************************************
int AICheckClosure(PlaneParams *planepnt, PlaneParams *tanker, int maxspeeddiff, long delayinc)
{
//	long planespeed, tankerspeed;
//	long speeddiff;
//	char tempstr[1024];

	if(planepnt == NULL)
		return(0);

	if(tanker == NULL)
		return(0);

	if(planepnt->Status & AL_AI_DRIVEN)
		return(1);

	return(1);

#if 0
	planespeed = AIConvertFtPSToSpeed(tanker->Altitude, planepnt->V, AIDESIREDSPDCALC);
	tankerspeed = AIConvertFtPSToSpeed(tanker->Altitude, tanker->V, AIDESIREDSPDCALC);

	speeddiff = planespeed - tankerspeed;

	if((speeddiff > maxspeeddiff) || (speeddiff < -5))
	{
		if(planepnt->AI.lTimer2 < 0)
		{
			planepnt->AI.lTimer2 = delayinc;
			if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_WATCH_CLOSURE, g_iLanguageId))
			{
				return(0);
			}
			AIRGenericSpeech(AICF_WSO_WATCH_CLOSURE, (planepnt - Planes));
			AICAddAIRadioMsgs(tempstr, 40);
		}
	}

	if(labs(speeddiff) < maxspeeddiff)
		return(1);

	return(0);
#endif
}


//**************************************************************************************
void AICheckForOnRequest(PlaneParams *planepnt)
{
	long delaycnt = 0;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		return;
	}

	if(AICheckClosure(planepnt->AI.LinkedPlane, planepnt, 25, 10000))
	{
		planepnt->AI.TargetPos.Y = 1.0;

		//  Moved from AICContactRefueler
		AIChangeGroupBehavior(planepnt->AI.LinkedPlane, AIFlyFormation, AIFlyTankerFormation);

//		if(planepnt == PlayerPlane)  //  Playerized for demo SRE
//		{
			AICAddSoundCall(AICTankerOn1, planepnt - Planes, delaycnt, 50);
			delaycnt = delaycnt + 5000;

			AICAddSoundCall(AICTankerOn2, planepnt - Planes, delaycnt, 50);
//		}
	}
}

//**************************************************************************************
void AIUpdateTankerBoomLoc(PlaneParams *planepnt)
{
	PlaneParams *tanking;

	if(planepnt->AI.LinkedPlane == NULL)
		return;

	tanking = planepnt->AI.LinkedPlane;
	if(tanking == NULL)
		return;

	if (planepnt->RefuelingHoses[0].ConnectedPlane == (DWORD)tanking)
		AttachPlanePointToRefuelingHose(planepnt,(DWORD)tanking,0);

#if 0
	if(GetAsyncKeyState('O') == 0xFFFF8001)
	{
		if(tanking->Status & AL_DEVICE_DRIVEN)
		{
			tanking->Status &= ~AL_DEVICE_DRIVEN;
			tanking->Status |= AL_AI_DRIVEN;
//			AICAddAIRadioMsgs("AI", 50);
			tanking->DoControlSim = CalcF18ControlSurfacesSimple;
			tanking->DoCASSim = CalcF18CASUpdatesSimple;
			tanking->DoForces = CalcAeroForcesSimple;
			tanking->DoPhysics = CalcAeroDynamicsSimple;
			tanking->dt = 0;
			tanking->UpdateRate = HIGH_AERO;

			tanking->MaxPitchRate = 10.0;
			tanking->MaxRollRate = 90.0;
			if(pDBAircraftList[tanking->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
			{
				tanking->MaxRollRate /= 3.0f;
			}
			tanking->YawRate = pDBAircraftList[tanking->AI.iPlaneIndex].iMaxYawRate;

			tanking->MaxSpeedAccel = pDBAircraftList[tanking->AI.iPlaneIndex].iAcceleration;	//15
			tanking->SpeedPercentage = 1.0;

			tanking->BfLinVel.X = tanking->V;
			tanking->BfLinVel.Y = 0;
			tanking->BfLinVel.Z = 0;
		}
		else
		{
			tanking->Status |= AL_DEVICE_DRIVEN;
			tanking->Status &= ~AL_AI_DRIVEN;
//			AICAddAIRadioMsgs("DEVICE", 50);
			tanking->DoControlSim = CalcF18ControlSurfaces;
			tanking->DoCASSim     = CalcF18CASUpdates;
			tanking->DoForces     = CalcAeroForces;
			tanking->DoPhysics    = CalcAeroDynamics;
			tanking->dt = HIGH_FREQ;
			tanking->UpdateRate = HIGH_AERO;
		}
	}
#endif
}

//**************************************************************************************
void AIUpdateTankerFormationLoc(PlaneParams *planepnt, FPoint leadposition, double angzx, double angzz)
{
	int placeingroup;

	if(planepnt->AI.Behaviorfunc == AIFlyTankerFormation)
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
		planepnt->AI.WorldFormationPos.X = leadposition.X + ConvertWayLoc((angzz * (-636 - (212 * placeingroup))) + (angzx * (636 + (212 * placeingroup))));
		planepnt->AI.WorldFormationPos.Y = leadposition.Y;
		planepnt->AI.WorldFormationPos.Z = leadposition.Z + ConvertWayLoc(-(angzx * (-636 - (212 * placeingroup))) + (angzz * (636 + (212 * placeingroup))));
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIUpdateTankerFormationLoc(&Planes[planepnt->AI.wingman], leadposition, angzx, angzz);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIUpdateTankerFormationLoc(&Planes[planepnt->AI.nextpair], leadposition, angzx, angzz);
	}
}

//**************************************************************************************
void AIFlyTankerFormation(PlaneParams *planepnt)
{
	PlaneParams *tanker;

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	tanker = AICheckTankerCloseBy(planepnt);
	if(tanker == NULL)
	{
		if(planepnt == PlayerPlane)
		{
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			AIChangeGroupBehavior(planepnt, AIFlyTankerFormation, AIFlyFormation);
			EndCasualAutoPilot();
		}
		else
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 2, 0, 0);
			}
			planepnt->AI.Behaviorfunc = AIFlyFormation;
		}
		return;
	}

	if(planepnt == PlayerPlane)
	{
		AIUpdateFormation(planepnt);
	}

	AISetTankerFormationPos(planepnt, tanker);

	if((planepnt == PlayerPlane) || (planepnt->AI.lVar3 & 0x4))
	{
		AIFormationFlyingLeadExact(planepnt, tanker);

		if(planepnt->AI.lVar3 & 1)
		{
			planepnt->DesiredSpeed = planepnt->DesiredSpeed - AIConvertSpeedToFtPS(planepnt->Altitude, 5.0f, 0);
		}
	}
	else
	{
		AIFormationFlyingLeadExact(planepnt, tanker);
	}

	planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
}

//**************************************************************************************
void AISetTankerFormationPos(PlaneParams *planepnt, PlaneParams *tanker)
{
	FPoint boompos;
//	float boomxinc, boomyinc, boomzinc;
	double angzx, angzz;
	int tempval = 1;
	int placeingroup;
	int extragroup = 0;
	PlaneParams *leadplane;

//	tempval = tanker->AI.lPlaneID;

//	switch(tempval)
//	{
//		case 16:  //  KC-135
//			boomxinc = 0;
//			boomyinc = -28;  //  -29;
//			boomzinc = 82;  //  83;
//			break;
//		case 64:
//			boomxinc = -13.0f;// * FOOT_MULTER;
//			boomyinc = 1.5f;// * FOOT_MULTER;
//			boomzinc = 34.80348f;// * FOOT_MULTER;
//			break;
//		case 999:  //  KC-10
//			boomxinc = 0;
//			boomyinc = -58;
//			boomzinc = 100;
//			break;
//		default:
//			boomxinc = 0;
//			boomyinc = -15;
//			boomzinc = 44;
//			break;
//	}

//#if 1
//	if((pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z*FOOT_MULTER) > planepnt->Type->Offsets[OFFSET_COCKPIT_FRONT].Z)
//	{
//		boomxinc += pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X;
//		boomyinc += pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y;
//		boomzinc += pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z;
//	}
//	else
//	{
//		boomxinc += 5;
//	}
//#endif

	angzx = (double)tanker->AI.WorldFormationPos.X;
	angzz = (double)tanker->AI.WorldFormationPos.Z;

//	boompos.X = tanker->WorldPosition.X + ConvertWayLoc((angzz * boomxinc) + (angzx * boomzinc));
//	boompos.Y = tanker->WorldPosition.Y + ConvertWayLoc(boomyinc);
//	boompos.Z = tanker->WorldPosition.Z + ConvertWayLoc(-(angzx * boomxinc) + (angzz * boomzinc));


	boompos = tanker->RefuelingHoses[0].ComputedLocation;
	boompos += tanker->WorldPosition;

	if((pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z*FOOT_MULTER) > planepnt->Type->Offsets[OFFSET_COCKPIT_FRONT].Z)
	{
		boompos.AddScaledVector(pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X FEET,tanker->Orientation.J);
		boompos.AddScaledVector(-pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y FEET,tanker->Orientation.K);
		boompos.AddScaledVector(-pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z FEET,tanker->Orientation.I);
	}
	else
	{
		boompos.AddScaledVector(5.0,tanker->Orientation.J);
	}

#if 0
	if(!cplaceholder)
	{
		cplaceholder = InstantiatePlaceHolder(boompos);
	}
	else
	{
		cplaceholder->Pos = boompos;
		cplaceholder->LifeTime = 2000;
	}
#endif

	leadplane = AIGetLeader(planepnt);
	if(((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || (planepnt == PlayerPlane)) && (!planepnt->AI.lVar3))
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	}
//	else if((AIGetLeader(planepnt) == PlayerPlane) && (!planepnt->AI.lVar3))
	else if((((leadplane->Status & PL_DEVICE_DRIVEN) || (leadplane == PlayerPlane) || (leadplane->Status & PL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))) && (!planepnt->AI.lVar3))
	{
		placeingroup = AIGetNumTankingWithPlayer(planepnt);
		extragroup = 1;
	}
	else
	{
		if(planepnt->AI.lVar3 & 0x8)
		{
			placeingroup = -((planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1);
		}
		else if(planepnt->AI.lVar3 & 0x4)
		{
			placeingroup = 1;
		}
		else
		{
			placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
		}
	}

	if(placeingroup > 1)
	{
		planepnt->AI.WorldFormationPos.X = boompos.X + ConvertWayLoc((angzz * (-636 - (212 * placeingroup))) + (angzx * (636 + (212 * placeingroup))));
		planepnt->AI.WorldFormationPos.Y = boompos.Y;
		if(extragroup) //  (!(planepnt->AI.iAIFlags1 & AIPLAYERGROUP))
		{
			planepnt->AI.WorldFormationPos.Y += 100.0f * FTTOWU;
		}
		planepnt->AI.WorldFormationPos.Z = boompos.Z + ConvertWayLoc(-(angzx * (-636 - (212 * placeingroup))) + (angzz * (636 + (212 * placeingroup))));
	}
	else if(placeingroup < 1)
	{
		planepnt->AI.WorldFormationPos.X = boompos.X + ConvertWayLoc((angzz * (636 - (212 * placeingroup))) + (angzx * (636 - (212 * placeingroup))));
		planepnt->AI.WorldFormationPos.Y = boompos.Y;
		if(extragroup)
		{
			planepnt->AI.WorldFormationPos.Y += 100.0f * FTTOWU;
		}
		planepnt->AI.WorldFormationPos.Z = boompos.Z + ConvertWayLoc(-(angzx * (636 - (212 * placeingroup))) + (angzz * (636 - (212 * placeingroup))));
	}
	else
	{
		planepnt->AI.WorldFormationPos = boompos;
		if((planepnt == PlayerPlane) && (planepnt->Status & PL_AI_DRIVEN))
		{
			if(planepnt->RefuelProbeCommandedPos < 1.0f)
			{
				planepnt->RefuelProbeCommandedPos = 45.0f;
			}
			if(!UFC.EMISState)
			{
				UFCToggleEMIS();
				SetCockpitItemState(199,UFC.EMISState);
			}
		}
	}

#if 0
	char tstr[256];
	sprintf(tstr, "DX %2.2f,DY %2.2f, DZ %2.2f", fabs(planepnt->WorldPosition.X - boompos.X) * WUTOFT, fabs(planepnt->WorldPosition.Y - boompos.Y) * WUTOFT, fabs(planepnt->WorldPosition.Z - boompos.Z) * WUTOFT);
	TXTWriteStringInBox(tstr, GrBuffFor3D, MessageFont, 1, 90, 638, 480, iMsgTextColor, 1, NULL, 0, 2);
#endif
}

//**************************************************************************************
void AIShowTankerFormationPos(PlaneParams *planepnt, PlaneParams *tanker)
{
	FPoint boompos;
//	float boomxinc, boomyinc, boomzinc;
//	double angzx, angzz;
	int tempval = 1;

//	tempval = tanker->AI.lPlaneID;

//	switch(tempval)
//	{
//		case 16:  //  KC-135
//			boomxinc = 0;
//			boomyinc = -28;  //  -29;
//			boomzinc = 82;  //  83;
//			break;
//		case 64:
//			boomxinc = -13.0f;// * FOOT_MULTER;
//			boomyinc = 1.5f;// * FOOT_MULTER;
//			boomzinc = 34.80348f;// * FOOT_MULTER;
//			break;
//		case 999:  //  KC-10
//			boomxinc = 0;
//			boomyinc = -58;
//			boomzinc = 100;
//			break;
//		default:
//			boomxinc = 0;
//			boomyinc = -15;
//			boomzinc = 44;
//			break;
//	}

//#if 1
//	if((pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z*FOOT_MULTER) > planepnt->Type->Offsets[OFFSET_COCKPIT_FRONT].Z)
//	{
//		boomxinc += (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X);
//		boomyinc += (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y);
//		boomzinc += (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z);
//	}
//	else
//	{
//		boomxinc += 5;
//	}
//#endif

//	angzx = (double)tanker->AI.WorldFormationPos.X;
//	angzz = (double)tanker->AI.WorldFormationPos.Z;

//	boompos.X = tanker->WorldPosition.X + ConvertWayLoc((angzz * boomxinc) + (angzx * boomzinc));
//	boompos.Y = tanker->WorldPosition.Y + ConvertWayLoc(boomyinc);
//	boompos.Z = tanker->WorldPosition.Z + ConvertWayLoc(-(angzx * boomxinc) + (angzz * boomzinc));

	boompos = tanker->RefuelingHoses[0].ComputedLocation;
	boompos += tanker->WorldPosition;

	if((pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z*FOOT_MULTER) > planepnt->Type->Offsets[OFFSET_COCKPIT_FRONT].Z)
	{
		boompos.AddScaledVector(pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X FEET,tanker->Orientation.J);
		boompos.AddScaledVector(-pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y FEET,tanker->Orientation.K);
		boompos.AddScaledVector(-pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z FEET,tanker->Orientation.I);
	}
	else
	{
		boompos.AddScaledVector(5.0,tanker->Orientation.J);
	}


#if 0
	if(!cplaceholder)
	{
		cplaceholder = InstantiatePlaceHolder(boompos);
	}
	else
	{
		cplaceholder->Pos = boompos;
		cplaceholder->LifeTime = 2000;
	}
#endif
}

//**************************************************************************************
void AIGetOffsetFromTanker(PlaneParams *planepnt, PlaneParams *tanker)
{
	double offangle;
	float tdist;
	float dx, dy, dz;
   	double angzx;  //, angzz;
	double heading;

	if((planepnt == NULL) || (tanker == NULL))
		return;

	AISetTankerFormationPos(planepnt, tanker);

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WorldFormationPos, &tdist, &dx ,&dy, &dz, 0) + 180;

	offangle -= AIConvertAngleTo180Degree(tanker->Heading);

	offangle = AICapAngle(offangle);

	heading = DegToRad(offangle);
	angzx = sin(heading);
//	angzz = cos(heading);

	planepnt->AI.TargetPos.X = (angzx * tdist) * WUTOFT;
	planepnt->AI.TargetPos.Y = dy * WUTOFT;
	if(fabs(offangle) < 90)
		planepnt->AI.TargetPos.Z = -tdist * WUTOFT;
	else
		planepnt->AI.TargetPos.Z = tdist * WUTOFT;

}

//**************************************************************************************
void AITankerTakeoff(PlaneParams *planepnt)
{
	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
	{
		AIChangeFXArray(planepnt, 0, 86, 5000);
	}
}

#if 1
//**************************************************************************************
void AIChangeFXArray(PlaneParams *planepnt, int arraynum, int desiredval, long ticksrange)
{
	BYTE itemp1, itemp2, itemp4, dval;
	float ftemp3;
	int neg = 0;

	itemp1 = (BYTE)planepnt->AI.cFXarray[arraynum];

	dval = (BYTE)desiredval;

	if(dval > itemp1)
	{
		itemp2 = dval - itemp1;
	}
	else
	{
		itemp2 = itemp1 - dval;
		neg = 1;
	}

	if(itemp2 == 0)
		return;

	ftemp3 = ((float)DeltaTicks / ticksrange) * 0xFF;
	itemp4 = (int)ftemp3;

	if(itemp2 > itemp4)
	{
		if(neg)
		{
			itemp1 = itemp1 - itemp4;
		}
		else
		{
			itemp1 = itemp1 + itemp4;
		}
	}
	else
	{
		itemp1 = dval;
	}

	planepnt->AI.cFXarray[arraynum] = (BYTE)itemp1;
}
#else
//**************************************************************************************
void AIChangeFXArray(PlaneParams *planepnt, int arraynum, int desiredval, long ticksrange)
{
	int itemp1, itemp2, itemp4;
	float ftemp3;

	itemp1 = (int)planepnt->AI.cFXarray[arraynum];
	itemp2 = desiredval - itemp1;

	if(itemp2 == 0)
		return;

	ftemp3 = ((float)DeltaTicks / ticksrange) * 0xFF;
	itemp4 = (int)ftemp3;

	if(abs(itemp2) > itemp4)
	{
		itemp2 = (itemp2 < 0) ? -itemp4 : itemp4;
	}

	itemp1 += itemp2;
	planepnt->AI.cFXarray[arraynum] = (BYTE)itemp1;
}
#endif

//**************************************************************************************
int AICheckProbeDroguePos(PlaneParams *planepnt, PlaneParams *tanker)
{
	FPointDouble boompos;
	FPointDouble probepos;
	FPoint delta;
//	double boomxinc, boomyinc, boomzinc;
	double droguex, droguey, droguez;
	double angzx, angzz;
	int tempval = 1;
	double heading;
	double tdist;
	float conemod;
	float distmod;

//	tempval = tanker->AI.lPlaneID;

//	switch(tempval)
//	{
//		case 16:  //  KC-135
//			boomxinc = 0;
//			boomyinc = -28;  //  -29;
//			boomzinc = 82;  //  83;
//			break;
//		case 64:
//			boomxinc = -13.0f;// * FOOT_MULTER;
//			boomyinc = 1.5f;// * FOOT_MULTER;
//			boomzinc = 34.80348f;// * FOOT_MULTER;
//			break;
//		case 999:  //  KC-10
//			boomxinc = 0;
//			boomyinc = -58;
//			boomzinc = 100;
//			break;
//		default:
//			boomxinc = 0;
//			boomyinc = -15;
//			boomzinc = 44;
//			break;
//	}

//	angzx = (double)tanker->AI.WorldFormationPos.X;
//	angzz = (double)tanker->AI.WorldFormationPos.Z;

//	boompos.X = tanker->WorldPosition.X + ConvertWayLoc((angzz * boomxinc) + (angzx * boomzinc));
//	boompos.Y = tanker->WorldPosition.Y + ConvertWayLoc(boomyinc);
//	boompos.Z = tanker->WorldPosition.Z + ConvertWayLoc(-(angzx * boomxinc) + (angzz * boomzinc));

	boompos = tanker->RefuelingHoses[0].ComputedLocation;
	boompos += tanker->WorldPosition;

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	droguex = -(pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X);
	droguey = -(pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y);
	droguez = -(pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z);

	angzx = (double)tanker->AI.WorldFormationPos.X;
	angzz = (double)tanker->AI.WorldFormationPos.Z;

	probepos.X = planepnt->WorldPosition.X + ConvertWayLoc((angzz * droguex) + (angzx * droguez));
	probepos.Y = planepnt->WorldPosition.Y + ConvertWayLoc(droguey);
	probepos.Z = planepnt->WorldPosition.Z + ConvertWayLoc(-(angzx * droguex) + (angzz * droguez));

	FPointDouble relative_position;
	FMatrix de_rotate;

	tdist = probepos - boompos;
	relative_position.MakeVectorToFrom(probepos, boompos);

	de_rotate = tanker->Attitude;
	de_rotate.Transpose();

	relative_position *= de_rotate;

//	char tstr[246];
//	sprintf(tstr, "X %2.2f, Y %2.2f, Z %2.2f", relative_position.X * WUTOFT, relative_position.Y * WUTOFT, relative_position.Z * WUTOFT);
//	AICAddAIRadioMsgs(tstr, 50);

	if ((tanker->RefuelingHoses[0].Status & RH_CONNECTED) && (tanker->RefuelingHoses[0].ConnectedPlane == (DWORD)planepnt))
	{
		if(planepnt->Status & AL_DEVICE_DRIVEN)
		{
			/* -----------------7/16/99 10:52AM--------------------------------------------------------------------
			/* adjust for dificulty level
			/* ----------------------------------------------------------------------------------------------------*/
			if(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL)
			{
				distmod = 20 FEET;
				conemod = 20 FEET;
			}
			else
			{
				distmod = 80 FEET;
				conemod = 80 FEET;
			}
		}
		else
		{
			distmod = 10 FEET;
			conemod = 10 FEET;
		}
		if((MultiPlayer) && (!((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))))
		{
			if((planepnt->AI.Behaviorfunc != AITankerDisconnect) && (planepnt->AI.Behaviorfunc != AITankerDisconnectAI))
			{
				distmod *= 2.0f;
				conemod *= 2.0f;
			}
		}

		if (fabs(relative_position.Z) >= distmod)
			return 0;
	}
	else
	{
		if(planepnt->Status & AL_DEVICE_DRIVEN)
		{
			/* -----------------7/16/99 10:52AM--------------------------------------------------------------------
			/* adjust for dificulty level
			/* ----------------------------------------------------------------------------------------------------*/
			if(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL)
			{
				distmod = 2.0f FEET;
				conemod = 2.0f FEET;
			}
			else
			{
				distmod = 40.0f FEET;
				conemod = 40.0f FEET;
			}
		}
		else
		{
			distmod = 2.0f FEET;  //  1.0f FEET;
			conemod = 2.0f FEET;  //  1.0f FEET;
		}

		if(relative_position.Z >= distmod)
			return 0;
	}

	if(planepnt->RefuelProbeCommandedPos < 1.0f)
		return(0);

	if(QuickDistance(relative_position.X, relative_position.Y) < conemod)
		return(1);
	else
		return(-1);
}


//**************************************************************************************
void AISetUpToTank(PlaneParams *planepnt)
{
	PlaneParams *tanker;
	tanker = AICheckTankerCloseBy(planepnt, -1);

	if(!tanker)
	{
		return;
	}
	if(!((tanker->Status & PL_ACTIVE) && (!(tanker->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))))
	{
		return;
	}

	planepnt->AI.lVar3 = 0x2;
//	Planes[escortplane].AI.LinkedPlane = planepnt; //  Don't think this is needed, and it screws up refueling with escorts for the refueler
	AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToTankerMeeting);
	AIUpdateGroupPlaneLink(planepnt - Planes, AIGetLeader(tanker));
}

//**************************************************************************************
void AIFlyToTankerMeeting(PlaneParams *planepnt)
{
	float tdist;
	float desiredspeed;

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
		tdist = FlyToPoint(planepnt, planepnt->AI.LinkedPlane->WorldPosition);
		desiredspeed = AIConvertFtPSToSpeed(planepnt->Altitude, planepnt->AI.LinkedPlane->V, AIDESIREDSPDCALC) + 100;
		MBAdjustAIThrust(planepnt, desiredspeed, 1);

		AIUpdateFormation(planepnt);

		if(((tdist * WUTONM) < 7.0f) && (planepnt->AI.lVar3 & 4))
		{
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyTankerFormation);
			AICContactRefueler(planepnt);
			planepnt->RefuelProbeCommandedPos = 45.0f;
		}
		planepnt->AI.lVar3 = 0x4;
	}
	else
	{
		planepnt->AI.lVar3 = 0x2;
		AIFormationFlying(planepnt);
	}
}

//**************************************************************************************
void AITankingFlight2AI(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;
	float fuelgiven;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	AIChangeFXArray(planepnt, 0, 0xFF, 5000);
	AIChangeFXArray(planepnt, 1, 0xFF, 4000);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	fuelgiven = planepnt->AI.fVarA[1];
	if(fuelgiven < 1000.0f)
		fuelgiven = 1500.0f;

	fuelgiven /= 60.0f;  //  lbs / min to lbs / sec;

	fuelgiven *= ((float)DeltaTicks / 1000.0f);

//	if(planepnt->AI.lTimer2 < 0)
	if(CTAddFuel(planepnt, planepnt->AI.LinkedPlane, fuelgiven))
	{
		planepnt->AI.lTimer2 = 15000;
		planepnt->AI.Behaviorfunc = AITankingFlight3AI;
		planepnt->AI.LinkedPlane->AI.Behaviorfunc = AITankerDisconnectAI;
		planepnt->AI.LinkedPlane->AI.lTimer2 = 10000;
		if((MultiPlayer) && ((planepnt->AI.LinkedPlane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt->AI.LinkedPlane == PlayerPlane)))
		{
			NetPutGenericMessage1(planepnt->AI.LinkedPlane, GM_TANKER_DISCO);
		}
	}

	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);
}

//**************************************************************************************
void AITankingFlight3AI(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	AIChangeFXArray(planepnt, 0, 0xFF, 5000);
	AIChangeFXArray(planepnt, 1, 0xFF, 4000);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	if(planepnt->AI.lTimer2 < 0)
	{
#if 0
		PlaneParams *tanking;
		PlaneParams *leadplane;

		tanking = planepnt->AI.LinkedPlane;
		if(tanking->AI.wingman >= 0)
		{
			if(Planes[tanking->AI.wingman].AI.Behaviorfunc == AIFlyTankerFormation)
			{
				planepnt->AI.LinkedPlane = &Planes[tanking->AI.wingman];
			}
		}
		if((tanking == planepnt->AI.LinkedPlane) && (tanking->AI.nextpair >= 0))
		{
			if(Planes[tanking->AI.nextpair].AI.Behaviorfunc == AIFlyTankerFormation)
			{
				planepnt->AI.LinkedPlane = &Planes[tanking->AI.nextpair];
			}
		}
		if((tanking == planepnt->AI.LinkedPlane) && (tanking->AI.winglead >= 0))
		{
			leadplane = &Planes[tanking->AI.winglead];
			if(leadplane->AI.nextpair >= 0)
			{
				if(Planes[leadplane->AI.nextpair].AI.Behaviorfunc == AIFlyTankerFormation)
				{
					planepnt->AI.LinkedPlane = &Planes[leadplane->AI.nextpair];
				}
			}
		}
		if(tanking == planepnt->AI.LinkedPlane)
		{
			if((MultiPlayer) && ((planepnt->AI.LinkedPlane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt->AI.LinkedPlane == PlayerPlane)))
			{
				NetPutGenericMessage2(planepnt, GM2_DONE_TANKING, (tanking - Planes));
			}

			tanking = AIGetLeader(tanking);
			AIChangeGroupBehavior(tanking, AIFlyTankerFormation, AIFlyFormation);
			AIUpdateGroupPlaneLink(tanking - Planes, NULL);
			AINextWayPointNoActions(tanking);
			AISetUpNewWayPoint(tanking);
			AIResetTanker(planepnt);
			AICTankerDisconnecting(planepnt->AI.LinkedPlane - Planes, planepnt - Planes);
			return;
		}
		else
		{
			planepnt->AI.LinkedPlane->AI.lVar3 = 0x4;
			planepnt->AI.LinkedPlane->RefuelProbeCommandedPos = 45.0f;
			planepnt->AI.Behaviorfunc = AITankingFlight;
			if((MultiPlayer) && ((planepnt->AI.LinkedPlane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt->AI.LinkedPlane == PlayerPlane)))
			{
		 		NetPutGenericMessage2(planepnt->AI.LinkedPlane, GM2_CONTACT_REFUELER_NO_B, (planepnt - Planes));
			}
		}
#else
		CTGetNextToTank(planepnt->AI.LinkedPlane, planepnt);
#endif
	}

	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);
}

//**************************************************************************************
void AITankerDisconnectAI(PlaneParams *planepnt)
{
	MBAdjustAIThrust(planepnt, 280, 1);
	planepnt->DesiredRoll = 0;
	planepnt->DesiredPitch = 0;

	if(planepnt->AI.lTimer2 < 0)
	{
		planepnt->AI.Behaviorfunc = AIFlyTankerFormation;
		planepnt->AI.lVar3 = 0x8;
		planepnt->RefuelProbeCommandedPos = 0.0f;
	}
}

extern void StartEngineSounds();

//************************************************************************************************
void RefuelFuelPlane(PlaneParams *planepnt, int fuelrequest)
{
	int cnt;
	int id;
	int bombtype;
	int weight;
	float fueladd;

	if((planepnt == PlayerPlane) && (planepnt->InternalFuel <= 0))
	{
		StartEngineSounds();
	}

	if((fuelrequest != 4) && (fuelrequest != 1))
	{
		planepnt->InternalFuel      = WEIGHT_MAX_INTERNALFUEL;
		planepnt->CenterDropFuel	= 0;
		planepnt->WingDropFuel      = 0;

		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
			id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

			if(bombtype == 12)
			{
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					if (id == 142)	// 330 gallon tank
						weight = planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
					else // 480 gallon tank
						weight = planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;

					if(cnt == CENTER6_STATION)
						planepnt->CenterDropFuel += weight;
					else
						planepnt->WingDropFuel += weight;
				}
			}
		}
	}
	else
	{
		if(fuelrequest == 4)
			fueladd = 4000.0f;
		else
		{
			fueladd = MAX_TRAP_WEIGHT - planepnt->TotalWeight;  // Determine max trap weight
			if(fueladd < 0)
			{
				fueladd = 0;
				return;
			}
		}
		planepnt->InternalFuel += fueladd;
		if(planepnt->InternalFuel > WEIGHT_MAX_INTERNALFUEL)
		{
			fueladd = planepnt->InternalFuel - WEIGHT_MAX_INTERNALFUEL;
			planepnt->InternalFuel      = WEIGHT_MAX_INTERNALFUEL;

			cnt = CENTER6_STATION;  //  8;

			bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
			id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

			if(bombtype == 12)
			{
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					if (id == 142)	// 330 gallon tank
						weight = planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
					else // 480 gallon tank
						weight = planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;

					planepnt->CenterDropFuel += fueladd;

					if(planepnt->CenterDropFuel > weight)
					{
						fueladd = planepnt->CenterDropFuel - weight;
						planepnt->CenterDropFuel = weight;
					}
					else
					{
						fueladd = 0;
					}
				}
			}

			weight = 0;
			if(fueladd)
			{
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					if(cnt == CENTER6_STATION)
						continue;

					bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
					id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

					if(bombtype == 12)
					{
						if(planepnt->WeapLoad[cnt].Count > 0)
						{
							if (id == 142)	// 330 gallon tank
								weight += planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
							else // 480 gallon tank
								weight += planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;
						}
					}
				}
				planepnt->WingDropFuel += fueladd;

				if(planepnt->WingDropFuel > weight)
				{
					fueladd = planepnt->WingDropFuel - weight;
					planepnt->WingDropFuel = weight;
				}
				else
				{
					fueladd = 0;
				}
			}
		}
	}


	planepnt->TotalWeight = 	planepnt->AircraftDryWeight +
					 	planepnt->InternalFuel +
					 	planepnt->CenterDropFuel +
					 	planepnt->WingDropFuel;

	planepnt->TotalMass = planepnt->TotalWeight/32.0;

	if(planepnt == PlayerPlane)
	{
		ClearMasterWarning(WARN_FUEL_LOW);

		CTReDistributeFuelTanks();
	}
}

//**************************************************************************************
void CTGetNextToTank(PlaneParams *planepnt, PlaneParams *tanker)
{
	PlaneParams *tanking = planepnt;
	PlaneParams *leadplane;

	tanking = planepnt;
	if(tanking->AI.wingman >= 0)
	{
		if(Planes[tanking->AI.wingman].AI.Behaviorfunc == AIFlyTankerFormation)
		{
			tanker->AI.LinkedPlane = &Planes[tanking->AI.wingman];
		}
	}
	if((tanking == tanker->AI.LinkedPlane) && (tanking->AI.nextpair >= 0))
	{
		if(Planes[tanking->AI.nextpair].AI.Behaviorfunc == AIFlyTankerFormation)
		{
			tanker->AI.LinkedPlane = &Planes[tanking->AI.nextpair];
		}
	}
	if((tanking == tanker->AI.LinkedPlane) && (tanking->AI.winglead >= 0))
	{
		leadplane = &Planes[tanking->AI.winglead];
		if(leadplane->AI.nextpair >= 0)
		{
			if(Planes[leadplane->AI.nextpair].AI.Behaviorfunc == AIFlyTankerFormation)
			{
				tanker->AI.LinkedPlane = &Planes[leadplane->AI.nextpair];
			}
		}
	}
	if(tanking == tanker->AI.LinkedPlane)
	{
		if((MultiPlayer) && ((tanker->AI.LinkedPlane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (tanker->AI.LinkedPlane == PlayerPlane)))
		{
			NetPutGenericMessage2(tanker, GM2_DONE_TANKING, (tanking - Planes));
		}

		tanking = AIGetLeader(tanking);
		AIChangeGroupBehavior(tanking, AIFlyTankerFormation, AIFlyFormation);
		AIUpdateGroupPlaneLink(tanking - Planes, NULL);
		AINextWayPointNoActions(tanking);
		AISetUpNewWayPoint(tanking);
		AIResetTanker(tanker);
		return;
	}
	else
	{
		long delaycnt = 0;

		AICAddSoundCall(AICTankerOn1, tanker - Planes, delaycnt, 50);
		delaycnt = delaycnt + 5000;

		AICAddSoundCall(AICTankerOn2, tanker - Planes, delaycnt, 50);
		delaycnt = delaycnt + 5000;

		AICAddSoundCall(AICTankerContact1, tanker->AI.LinkedPlane - Planes, delaycnt, 50, tanker->AI.cUseWeapon);

		tanker->AI.LinkedPlane->AI.lVar3 = 0x4;
		tanker->AI.LinkedPlane->RefuelProbeCommandedPos = 45.0f;
		tanker->AI.Behaviorfunc = AITankingFlight;
		tanker->AI.fVarA[0]	= CTGetTankingAmount(tanker->AI.LinkedPlane, tanker->AI.cUseWeapon);
		if((MultiPlayer) && ((tanker->AI.LinkedPlane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (tanker->AI.LinkedPlane == PlayerPlane) || (tanking == PlayerPlane)))
		{
		 	NetPutGenericMessage2(tanker->AI.LinkedPlane, GM2_CONTACT_REFUELER_NO_B, (tanker - Planes));
		}
	}
}

//**************************************************************************************
float CTGetTankingAmount(PlaneParams *planepnt, int refuelrequest)
{
	int cnt;
	int id;
	int bombtype;
	int weight;
	float fworkvar;
	PlaneParams *leadplane = AIGetLeader(planepnt);

	if(!((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (leadplane == PlayerPlane)))
	{
		return(2000.0f);
	}

	if(refuelrequest != 4)
	{
		weight = WEIGHT_MAX_INTERNALFUEL;

		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
			id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

			if(bombtype == 12)
			{
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					if (id == 142)	// 330 gallon tank
						weight += planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
					else // 480 gallon tank
						weight += planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;
				}
			}
		}
	}

	if(refuelrequest == 1)
	{
		if((planepnt->TotalWeight + weight) < MAX_TRAP_WEIGHT)
		{
			fworkvar = weight - (planepnt->InternalFuel + planepnt->CenterDropFuel + planepnt->WingDropFuel) + 500.0f;
		}
		else
		{
			fworkvar = (MAX_TRAP_WEIGHT - planepnt->TotalWeight) + 500.0f;
		}

		if(fworkvar < 0)
			fworkvar = 0;

		return(fworkvar);
	}
	else if(refuelrequest == 4)
	{
		return(4000.0f);
	}
	else
	{
		fworkvar = weight - (planepnt->InternalFuel + planepnt->CenterDropFuel + planepnt->WingDropFuel) + 500.0f;

		if(fworkvar < 0)
			fworkvar = 0;

		return(fworkvar);
	}

	return(0);
}

//**************************************************************************************
int CTAddFuel(PlaneParams *tanker, PlaneParams *planepnt, float fuelamount)
{
	int cnt;
	int id;
	int bombtype;
	int weight;
	float fueladd = fuelamount;

	if(tanker->AI.fVarA[0] <= 0)
		return(1);

	tanker->AI.fVarA[0] = tanker->AI.fVarA[0] - fuelamount;

	if((PlayerPlane == planepnt) && (planepnt->InternalFuel <= 0))
	{
		StartEngineSounds();
	}

	planepnt->InternalFuel += fueladd;
	if(planepnt->InternalFuel > WEIGHT_MAX_INTERNALFUEL)
	{
		fueladd = planepnt->InternalFuel - WEIGHT_MAX_INTERNALFUEL;
		planepnt->InternalFuel      = WEIGHT_MAX_INTERNALFUEL;

		cnt = CENTER6_STATION;

		bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

		if(bombtype == 12)
		{
			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				if (id == 142)	// 330 gallon tank
					weight = planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
				else // 480 gallon tank
					weight = planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;

				planepnt->CenterDropFuel += fueladd;

				if(planepnt->CenterDropFuel > weight)
				{
					fueladd = planepnt->CenterDropFuel - weight;
					planepnt->CenterDropFuel = weight;
				}
				else
				{
					fueladd = 0;
				}
			}
		}

		weight = 0;
		if(fueladd)
		{
			for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
			{
				if(cnt == CENTER6_STATION)
					continue;

				bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
				id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

				if(bombtype == 12)
				{
					if(planepnt->WeapLoad[cnt].Count > 0)
					{
						if (id == 142)	// 330 gallon tank
							weight += planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
						else // 480 gallon tank
							weight += planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;
					}
				}
			}

			planepnt->WingDropFuel += fueladd;

			if(planepnt->WingDropFuel > weight)
			{
				fueladd = planepnt->WingDropFuel - weight;
				planepnt->WingDropFuel = weight;
			}
			else
			{
				fueladd = 0;
			}
		}
	}
	else
	{
		fueladd = 0;
	}

	planepnt->TotalWeight = 	planepnt->AircraftDryWeight +
					 	planepnt->InternalFuel +
					 	planepnt->CenterDropFuel +
					 	planepnt->WingDropFuel;

	planepnt->TotalMass = planepnt->TotalWeight/32.0;

	if(PlayerPlane == planepnt)
	{
		CTReDistributeFuelTanks();
		ClearMasterWarning(WARN_FUEL_LOW);
	}

	if(fueladd > 0)
	{
		tanker->AI.fVarA[0] = -1.0f;
		return(1);
	}

	if(tanker->AI.fVarA[0] <= 0)
		return(1);

	return(0);
}

//**************************************************************************************
void AITankingFlight2HumanLong(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;
	float fuelgiven;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	AIChangeFXArray(planepnt, 0, 0xFF, 5000);
	AIChangeFXArray(planepnt, 1, 0xFF, 4000);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	fuelgiven = planepnt->AI.fVarA[1];
	if(fuelgiven < 1000.0f)
		fuelgiven = 1500.0f;

	fuelgiven /= 60.0f;  //  lbs / min to lbs / sec;

	fuelgiven *= ((float)DeltaTicks / 1000.0f);

//	if(planepnt->AI.lTimer2 < 0)
	if(CTAddFuel(planepnt, planepnt->AI.LinkedPlane, fuelgiven))
	{
		if(planepnt->AI.fVarA[0] > -9999.0f)
		{
			planepnt->AI.fVarA[0] = -10000.0f;
		}
		AICTankerDisconnecting(planepnt->AI.LinkedPlane - Planes, planepnt - Planes);
		planepnt->AI.Behaviorfunc = AITankingFlight3HumanLong;
	}

	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);

	if (!((planepnt->RefuelingHoses[0].Status & RH_CONNECTED) && (planepnt->RefuelingHoses[0].ConnectedPlane == (DWORD)planepnt->AI.LinkedPlane)))
	{
		planepnt->AI.Behaviorfunc = AITankingFlight;
	}
}

//**************************************************************************************
void AITankingFlight3HumanLong(PlaneParams *planepnt)
{
	float offangle, dx, dy, dz, tdist;
	double angzx, angzz;
	double heading;

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AIResetTanker(planepnt);
		return;
	}

	planepnt->AI.iAICombatFlags2 |= (AI_KEEP_HIGH_RATE);

	AIChangeFXArray(planepnt, 0, 0xFF, 5000);
	AIChangeFXArray(planepnt, 1, 0xFF, 4000);

	offangle = AIComputeWayPointHeading(planepnt, &tdist, &dx ,&dy, &dz, 0);

	offangle = AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading);
	AIChangeDir(planepnt, (double)AIConvertAngleTo180Degree(planepnt->AI.DesiredHeading - planepnt->Heading), ((double)dy * WUTOFT));

	heading = DegToRad((double)planepnt->Heading / DEGREE);
	angzx = sin(heading);
	angzz = cos(heading);

	planepnt->AI.WorldFormationPos.SetValues(angzx,(double)planepnt->WorldPosition.Y,angzz);

	AIGetOffsetFromTanker(planepnt->AI.LinkedPlane, planepnt);

	AIUpdateTankerBoomLoc(planepnt);
	MBAdjustAIThrust(planepnt, 300, 1);

	if (!((planepnt->RefuelingHoses[0].Status & RH_CONNECTED) && (planepnt->RefuelingHoses[0].ConnectedPlane == (DWORD)planepnt->AI.LinkedPlane)))
	{
		planepnt->AI.Behaviorfunc = AITankingFlight;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CTReDistributeFuelTanks()
{
	Av.Fuel.LastWing = PlayerPlane->WingDropFuel;
	Av.Fuel.LastCenter = PlayerPlane->CenterDropFuel;
	Av.Fuel.LastInternal = PlayerPlane->InternalFuel;

	float internalfuel = PlayerPlane->InternalFuel;
	if(internalfuel < (Av.Fuel.TanksMax[AV_FUEL_IN_L_FEED] * 2.0f))
	{
		Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] = Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] = internalfuel * 0.5f;
	}
	else
	{
		Av.Fuel.Tanks[AV_FUEL_IN_L_FEED] = Av.Fuel.TanksMax[AV_FUEL_IN_L_FEED];
		internalfuel -= Av.Fuel.TanksMax[AV_FUEL_IN_L_FEED];
		Av.Fuel.Tanks[AV_FUEL_IN_R_FEED] = Av.Fuel.TanksMax[AV_FUEL_IN_R_FEED];
		internalfuel -= Av.Fuel.TanksMax[AV_FUEL_IN_R_FEED];

		if(internalfuel > 0)
		{
			if(internalfuel < (Av.Fuel.TanksMax[AV_FUEL_IN_TANK1]))
			{
				Av.Fuel.Tanks[AV_FUEL_IN_TANK1] = internalfuel;
			}
			else
			{
				Av.Fuel.Tanks[AV_FUEL_IN_TANK1] = Av.Fuel.TanksMax[AV_FUEL_IN_TANK1];
				internalfuel -= Av.Fuel.TanksMax[AV_FUEL_IN_TANK1];
				if(internalfuel > 0)
				{
					if(internalfuel < (Av.Fuel.TanksMax[AV_FUEL_IN_TANK4]))
					{
						Av.Fuel.Tanks[AV_FUEL_IN_TANK4] = internalfuel;
					}
					else
					{
						Av.Fuel.Tanks[AV_FUEL_IN_TANK4] = Av.Fuel.TanksMax[AV_FUEL_IN_TANK4];
						internalfuel -= Av.Fuel.TanksMax[AV_FUEL_IN_TANK4];
						if(internalfuel > 0)
						{
							Av.Fuel.Tanks[AV_FUEL_IN_LEFT] = Av.Fuel.Tanks[AV_FUEL_IN_RIGHT] = internalfuel * 0.5f;
						}
					}
				}
			}
		}
	}

	int tank330gid = GetWeapId(GALLONTANK330_ID);
	int tank480gid = GetWeapId(GALLONTANK480_ID);

	Av.Fuel.Tanks[AV_FUEL_EX_C] = PlayerPlane->CenterDropFuel;

	float wingfuel = PlayerPlane->WingDropFuel;
	float max_EX_RI, max_EX_LI, max_EX_LO, max_EX_RO;

	if((PlayerPlane->WeapLoad[RIGHT8_STATION].Count > 0) && ((PlayerPlane->WeapLoad[RIGHT8_STATION].WeapId == tank330gid) || (PlayerPlane->WeapLoad[RIGHT8_STATION].WeapId == tank480gid)))
	{
		max_EX_RI = Av.Fuel.TanksMax[AV_FUEL_EX_RI];
	}
	else
	{
		max_EX_RI = 0;
	}

	if((PlayerPlane->WeapLoad[LEFT4_STATION].Count > 0) && ((PlayerPlane->WeapLoad[LEFT4_STATION].WeapId == tank330gid) || (PlayerPlane->WeapLoad[LEFT4_STATION].WeapId == tank480gid)))
	{
		max_EX_LI = Av.Fuel.TanksMax[AV_FUEL_EX_LI];
	}
	else
	{
		max_EX_LI = 0;
	}

	if((PlayerPlane->WeapLoad[RIGHT9_STATION].Count > 0) && ((PlayerPlane->WeapLoad[RIGHT9_STATION].WeapId == tank330gid) || (PlayerPlane->WeapLoad[RIGHT9_STATION].WeapId == tank480gid)))
	{
		max_EX_RO = Av.Fuel.TanksMax[AV_FUEL_EX_RO];
	}
	else
	{
		max_EX_RO = 0;
	}

	if((PlayerPlane->WeapLoad[LEFT3_STATION].Count > 0) && ((PlayerPlane->WeapLoad[LEFT3_STATION].WeapId == tank330gid) || (PlayerPlane->WeapLoad[LEFT3_STATION].WeapId == tank480gid)))
	{
		max_EX_LO = Av.Fuel.TanksMax[AV_FUEL_EX_LO];
	}
	else
	{
		max_EX_LO = 0;
	}

	if(wingfuel < (max_EX_LI + max_EX_RI))
	{
		if(max_EX_LI == max_EX_RI)
		{
			Av.Fuel.Tanks[AV_FUEL_EX_LI] = wingfuel * 0.5f;
			Av.Fuel.Tanks[AV_FUEL_EX_RI] = wingfuel * 0.5f;
		}
		else if(max_EX_LI > max_EX_RI)
		{
			if(max_EX_RI > (wingfuel * 0.5f))
			{
				Av.Fuel.Tanks[AV_FUEL_EX_RI] = Av.Fuel.Tanks[AV_FUEL_EX_LI] = wingfuel * 0.5f;
			}
			else
			{
				Av.Fuel.Tanks[AV_FUEL_EX_RI] = max_EX_RI;
				wingfuel -= Av.Fuel.Tanks[AV_FUEL_EX_RI];
				Av.Fuel.Tanks[AV_FUEL_EX_LI] = wingfuel;
			}
		}
		else
		{
			if(max_EX_LI > (wingfuel * 0.5f))
			{
				Av.Fuel.Tanks[AV_FUEL_EX_LI] = Av.Fuel.Tanks[AV_FUEL_EX_RI] = wingfuel * 0.5f;
			}
			else
			{
				Av.Fuel.Tanks[AV_FUEL_EX_LI] = max_EX_LI;
				wingfuel -= Av.Fuel.Tanks[AV_FUEL_EX_LI];
				Av.Fuel.Tanks[AV_FUEL_EX_RI] = wingfuel;
			}
		}
	}
	else
	{
		Av.Fuel.Tanks[AV_FUEL_EX_LI] = max_EX_LI;
		wingfuel -= max_EX_LI;
		Av.Fuel.Tanks[AV_FUEL_EX_RI] = max_EX_RI;
		wingfuel -= max_EX_RI;

		if(wingfuel > 0)
		{
			if(max_EX_LO == max_EX_RO)
			{
				Av.Fuel.Tanks[AV_FUEL_EX_RO] = wingfuel * 0.5f;
				Av.Fuel.Tanks[AV_FUEL_EX_LO] = wingfuel * 0.5f;
			}
			else if(max_EX_LO > max_EX_RO)
			{
				if(max_EX_RO > (wingfuel * 0.5f))
				{
					Av.Fuel.Tanks[AV_FUEL_EX_RO] = wingfuel * 0.5f;
					Av.Fuel.Tanks[AV_FUEL_EX_LO] = wingfuel * 0.5f;
				}
				else
				{
					Av.Fuel.Tanks[AV_FUEL_EX_RO] = max_EX_RO;
					wingfuel -= Av.Fuel.Tanks[AV_FUEL_EX_RO];
					Av.Fuel.Tanks[AV_FUEL_EX_LO] = wingfuel;
				}
			}
			else
			{
				if(max_EX_LO > (wingfuel * 0.5f))
				{
					Av.Fuel.Tanks[AV_FUEL_EX_LO] = wingfuel * 0.5f;
					Av.Fuel.Tanks[AV_FUEL_EX_RO] = wingfuel * 0.5f;
				}
				else
				{
					Av.Fuel.Tanks[AV_FUEL_EX_LO] = max_EX_LO;
					wingfuel -= Av.Fuel.Tanks[AV_FUEL_EX_LO];
					Av.Fuel.Tanks[AV_FUEL_EX_RO] = wingfuel;
				}
			}
		}
	}
}

#ifdef _DEBUG  //  For testing refueling
//**************************************************************************************
int CTAddFuelTest(PlaneParams *planepnt, float fuelamount)
{
	int cnt;
	int id;
	int bombtype;
	int weight;
	float fueladd = fuelamount;

	if((PlayerPlane == planepnt) && (planepnt->InternalFuel <= 0))
	{
		StartEngineSounds();
	}

	planepnt->InternalFuel += fueladd;
	if(planepnt->InternalFuel > WEIGHT_MAX_INTERNALFUEL)
	{
		fueladd = planepnt->InternalFuel - WEIGHT_MAX_INTERNALFUEL;
		planepnt->InternalFuel      = WEIGHT_MAX_INTERNALFUEL;

		cnt = CENTER6_STATION;

		bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

		if(bombtype == 12)
		{
			if(planepnt->WeapLoad[cnt].Count > 0)
			{
				if (id == 142)	// 330 gallon tank
					weight = planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
				else // 480 gallon tank
					weight = planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;

				planepnt->CenterDropFuel += fueladd;

				if(planepnt->CenterDropFuel > weight)
				{
					fueladd = planepnt->CenterDropFuel - weight;
					planepnt->CenterDropFuel = weight;
				}
				else
				{
					fueladd = 0;
				}
			}
		}

		weight = 0;
		if(fueladd)
		{
			for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
			{
				if(cnt == CENTER6_STATION)
					continue;

				bombtype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
				id = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;

				if(bombtype == 12)
				{
					if(planepnt->WeapLoad[cnt].Count > 0)
					{
						if (id == 142)	// 330 gallon tank
							weight += planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
						else // 480 gallon tank
							weight += planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;
					}
				}
			}

			planepnt->WingDropFuel += fueladd;

			if(planepnt->WingDropFuel > weight)
			{
				fueladd = planepnt->WingDropFuel - weight;
				planepnt->WingDropFuel = weight;
			}
			else
			{
				fueladd = 0;
			}
		}
	}
	else
	{
		fueladd = 0;
	}

	planepnt->TotalWeight = 	planepnt->AircraftDryWeight +
					 	planepnt->InternalFuel +
					 	planepnt->CenterDropFuel +
					 	planepnt->WingDropFuel;

	planepnt->TotalMass = planepnt->TotalWeight/32.0;

	if(PlayerPlane == planepnt)
	{
		ClearMasterWarning(WARN_FUEL_LOW);
		CTReDistributeFuelTanks();
	}

	if(fueladd > 0)
	{
		return(1);
	}

	return(0);
}
#endif

//**************************************************************************************
//**************************************************************************************
//   Carrier Stuff
//**************************************************************************************
//**************************************************************************************


//**************************************************************************************
void CTSendToDivertField(PlaneParams *planepnt)
{
	FPoint pointpos;
	float fworkspeed;
	int rtb = 0;
	TakeOffActionType ActionTakeOff;
	FPointDouble fieldpos, fptemp;
	ANGLE runwayheading;


	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		AIGetClosestAirField(planepnt, fptemp, &fieldpos, &runwayheading, &ActionTakeOff, -1.0f, 0);
		planepnt->AI.WayPosition = fieldpos;
		planepnt->AI.WayPosition.Y = 1000 * FTTOWU;
		AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToLandingPoint);
		planepnt->AI.lDesiredSpeed = pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
	}
	else
	{
		fworkspeed = 0.8f * pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
		planepnt->AI.lDesiredSpeed = fworkspeed;
		fptemp.SetValues(-1.0f,-1.0f,-1.0f);
		AIGetClosestAirField(planepnt, fptemp, &fieldpos, &runwayheading, &ActionTakeOff, -1.0f, 0);
		planepnt->AI.WayPosition = fieldpos;
		planepnt->AI.WayPosition.Y = 10000 * FTTOWU;
		AISetPlaneLandingFlyToField(planepnt, &ActionTakeOff, runwayheading);
	}
}

//**************************************************************************************
int AICheckPlaneLandingFlyToCarrier(PlaneParams *planepnt, TakeOffActionType *pActionTakeOff)
{
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;
	long xftoff = 0;
	long zftoff = 120000;

	runwaypos.SetValues(-1.0f,-1.0f,-1.0f);

	if(planepnt->AI.iAICombatFlags1 & AI_HOME_AIRPORT)
	{
		return(0);
	}
	else if(!(MovingVehicles[0].Status & VL_ACTIVE))
	{
		return(0);
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		if (OffDiskTerrainInfo)
		{
			OffDiskTerrainInfo->m_Location = planepnt->AI.WayPosition;
			OffDiskTerrainInfo->GetInfo(TI_WANT_FLAGS);

			if(!(OffDiskTerrainInfo->m_TileFlags & T_TILE_WATER))
			{
				if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
				{
					MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

					double dx = (carrier->pStartWP->lX * FTTOWU) - planepnt->AI.WayPosition.X;
					double dz = (carrier->pStartWP->lZ * FTTOWU) - planepnt->AI.WayPosition.Z;
					double tdist = QuickDistance(dx, dz) * WUTONM;
					if((tdist > 20.0f) && (planepnt->AI.Behaviorfunc != AISARPickUp))
					{
						planepnt->AI.Behaviorfunc = AIFlyToLandingPoint;
						return(2);
					}
				}
			}
		}
	}
	else if(planepnt != PlayerPlane)
	{
		if(AIGetClosestAirField(planepnt, planepnt->AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 5.0f * NMTOWU, 1))
		{
			if(!AIInPlayerGroup(planepnt))
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
				planepnt->AI.Behaviorfunc = AIPlaneLandingFlyToField;
				planepnt->AI.OrgBehave = NULL;

				planepnt->AI.DesiredHeading = runwayheading;
				planepnt->AI.TargetPos = runwaypos;

				AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
				planepnt->AI.LinkedPlane = NULL;
				planepnt->AI.WayPosition.Y = AIGetMarshallHeight(planepnt);
			}
			return(2);
		}
	}

	AISetPlaneLandingFlyToCarrier(planepnt);
	return(1);
}

//**************************************************************************************
void AISetPlaneLandingFlyToCarrier(PlaneParams *planepnt)
{
	PlaneParams *leadplane;
	MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	long delaycnt;

	leadplane = AIGetLeader(planepnt);

	if(carrier->lAIFlags1 & V_ON_FIRE)
	{
		if(leadplane == planepnt)
		{
			AICNotifyShipInbound((planepnt - Planes), 1);

			delaycnt = 10000;
			AICAddSoundCall(AIC_GenericMsgPlane, planepnt - Planes, delaycnt, 50, 34);

			delaycnt = delaycnt + 5000;
			AICAddSoundCall(AICAlphaCheck, planepnt - Planes, delaycnt, 50, 6);
		}

		if((leadplane->Status & PL_DEVICE_DRIVEN) || (leadplane == PlayerPlane) || (leadplane->Status & PL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (leadplane != planepnt))
		{
			CTSendToDivertField(planepnt);
		}
		else
		{
			AISendGroupHome(planepnt);
		}
		return;
	}

	
	planepnt->AI.DesiredPitch = 0;
	planepnt->AI.Behaviorfunc = AIPlaneLandingFlyToCarrier;
	planepnt->AI.OrgBehave = NULL;

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
//			AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
			planepnt->AI.WayPosition.Y = AIGetMarshallHeight(planepnt);
			if(planepnt->AI.startwpts != (planepnt->AI.CurrWay - AIWayPoints))
			{
				AICNotifyShipInbound((planepnt - Planes), 1);
				AICAddSoundCall(AICStrikeClear, (planepnt - Planes), 10000, 40, 1);
			}
		}
	}
	else
	{
//		AIGetXZOffset((float)planepnt->AI.DesiredHeading / DEGREE, planepnt->AI.TargetPos.X, planepnt->AI.TargetPos.Z, xftoff, zftoff, &planepnt->AI.WayPosition.X, &planepnt->AI.WayPosition.Z);
		if(((planepnt->AI.LinkedPlane == NULL) && (planepnt->AI.startwpts != (planepnt->AI.CurrWay - AIWayPoints))) && ((planepnt != PlayerPlane) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && (PlayerPlane->AI.lVar2 < 0)))))
		{
			AICNotifyShipInbound((planepnt - Planes), 1);
			AICAddSoundCall(AICStrikeClear, (planepnt - Planes), 10000, 40, 1);
		}
		if(planepnt == PlayerPlane)
		{
			if((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && (PlayerPlane->AI.lVar2 < 0))
			{
				AIContactCarrierMarshallPattern(PlayerPlane);
				PlayerPlane->AI.lTimer2 = PlayerPlane->AI.lTimer3;
				PlayerPlane->AI.lVar2 = 1;
			}
			else
			{
				planepnt->AI.iAICombatFlags1 |= AI_CARRIER_LANDING;
				planepnt->AI.lVar2 = 0;
			}
		}
		else
		{
			planepnt->AI.WayPosition.Y = AIGetMarshallHeight(planepnt);
		}
	}
}

//**************************************************************************************
void AIPlaneLandingFlyToCarrier(PlaneParams *planepnt)
{
	float tdist;
	double offangle;
	float dx, dy, dz;
	double rollval;
	PlaneParams *leadplane;
	FPoint	carrpos;
	MovingVehicleParams *carrier;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	carrpos = carrier->WorldPosition;
	carrpos.Y = planepnt->AI.WayPosition.Y;

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
	offangle = AIComputeHeadingToPoint(planepnt, carrpos, &tdist, &dx ,&dy, &dz, 1);

	AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));

	rollval = AIConvertAngleTo180Degree(planepnt->DesiredRoll);
	if(fabs(rollval) > 25)  //  was 60
	{
		rollval = (rollval < 0.0) ? -25 : 25;
		planepnt->DesiredRoll = AIConvert180DegreeToAngle(rollval);
	}
	planepnt->RollPercentage = 0.40;	//  max roll 90.0
	MBAdjustAIThrust(planepnt, planepnt->AI.lDesiredSpeed, 1);

	if(AIInNextRange(planepnt, tdist * WUTOFT,  50.0f * NMTOFT))
	{
		if((!MultiPlayer) || (!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
			AIContactCarrierMarshallPattern(planepnt);
	}
}

void do_debug_hit(VKCODE vk);

//**************************************************************************************
void AIContactCarrierMarshallPattern(PlaneParams *planepnt)
{
	float maxheight = 0;
	PlaneParams *checkplane;
	float runwaybuff;
	int foundone = 0;
	long pushtime = 0;
	double dworkvar, dworkangle, angels;
	int flags;
	double tdistft;
	long delaycnt = 13000;
	int numhelos = 0;
	long helopushtime = -1;
	long heloendpushtime = 0;
	float maxheloheight = 0;
	long orgpushtime;
	DBShipType *pshiptype;
	MovingVehicleParams *carrier;
	long lvar2org = planepnt->AI.lVar2;
	PlaneParams *leadplane;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(carrier->lAIFlags1 & V_ON_FIRE)
	{
		leadplane = AIGetLeader(planepnt);
		if(leadplane == planepnt)
		{
			AICNotifyShipInbound((planepnt - Planes), 1);

			delaycnt = 10000;
			AICAddSoundCall(AIC_GenericMsgPlane, planepnt - Planes, delaycnt, 50, 34);

			delaycnt = delaycnt + 5000;
			AICAddSoundCall(AICAlphaCheck, planepnt - Planes, delaycnt, 50, 6);
		}

		if((leadplane->Status & PL_DEVICE_DRIVEN) || (leadplane == PlayerPlane) || (leadplane->Status & PL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (leadplane != planepnt))
		{
			CTSendToDivertField(planepnt);
		}
		else
		{
			AISendGroupHome(planepnt);
		}
		return;
	}

	if((MultiPlayer) && ((!((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))) && (!g_bIAmHost)))
	{
		return;
	}
	else if(MultiPlayer && (!g_bIAmHost))
	{
		NetPutGenericMessage2(planepnt, GM2_GET_MARSHAL, planepnt->AI.iHomeBaseId);
		planepnt->AI.lTimer3 = 10000;
		return;
	}

	runwaybuff = 1000 * FTTOWU;

	checkplane = &Planes[0];

	pshiptype = &pDBShipList[carrier->iVDBIndex];

	if((pshiptype->lShipType & ~(SHIP_TYPE_CARRIER)) && (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
		planepnt->AI.lVar2 = 0;
		planepnt->AI.lTimer3 = -1;

		if(MultiPlayer)
		{
			delaycnt += 1000 + ((planepnt - Planes) * 50.0f);
		}
		else
		{
			delaycnt += frand() * 5000.0f;
		}
		maxheight = 500.0f * FTTOWU;
		pushtime = 3;

		if(MultiPlayer)
		{
			NetPutGenericMessage3Long(planepnt, GM3L_GIVE_MARSHAL, maxheight * WUTOFT, pushtime);
		}

		if(planepnt->AI.wingman >= 0)
		{
			AIMarshallRestOfGroup(&Planes[planepnt->AI.wingman], &maxheight, &pushtime, &delaycnt);
		}

		if(planepnt->AI.nextpair >= 0)
		{
			AIMarshallRestOfGroup(&Planes[planepnt->AI.nextpair], &maxheight, &pushtime, &delaycnt);
		}
		return;
	}

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			if((checkplane != planepnt) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (checkplane->AI.iHomeBaseId == planepnt->AI.iHomeBaseId))
			{
				if(checkplane->AI.Behaviorfunc == AIFlyCarrierMarshallPattern)
				{
					if(checkplane->AI.WayPosition.Y > maxheight)
					{
						maxheight = checkplane->AI.WayPosition.Y;
						pushtime = checkplane->AI.lTimer3;
						foundone = 1;
					}
				}
				else if(checkplane->AI.Behaviorfunc == AIFlyCarrierHeloMarshallPattern)
				{
					numhelos ++;
					if((helopushtime > checkplane->AI.lTimer3) || (helopushtime == -1))
					{
						helopushtime = checkplane->AI.lTimer3;
					}
					if(checkplane->AI.WayPosition.Y > maxheloheight)
					{
						maxheloheight = checkplane->AI.WayPosition.Y;
						heloendpushtime = checkplane->AI.lTimer3;
					}
				}
				else if(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
				{
					if((!(checkplane->AI.iAIFlags2 & AIINVISIBLE)) && (checkplane->AI.Behaviorfunc == AIMoveToParkingSpace) && (checkplane->AI.lVar3 == 6))
					{
						if(pshiptype->lShipType & ~(SHIP_TYPE_CARRIER))
						{
							checkplane->AI.lVar3 = 7;
							checkplane->AI.lTimer2 = 30000;
						}
						else
						{
							AIGetCarrierParkingSpace(checkplane);
							checkplane->AI.lTimer2 = 30000;
						}
					}
				}
			}
		}
		checkplane ++;
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		if(!foundone)
		{
			planepnt->AI.lTimer3 = 60000;
			planepnt->AI.TargetPos.Y = (6000 * FTTOWU);
			pushtime = 1;
		}
		else
		{
			if(maxheight < (6000.0f * FTTOWU))
			{
				maxheight = (6000.0f * FTTOWU);
			}
			planepnt->AI.TargetPos.Y = maxheight + (1000 * FTTOWU);

			pushtime /= 60000;

			pushtime += 2;
		}

		orgpushtime = pushtime;

		angels = (planepnt->AI.TargetPos.Y * WUTOFT) / 1000.0f;

		dworkangle = (planepnt->AI.TargetPos.Y / ((angels + 15.0f) * NMTOWU));

		if(fabs(dworkangle) > 1.0f)
		{
			dworkangle = 1.0f;
		}

		dworkangle = asin(dworkangle);
		dworkvar = planepnt->AI.TargetPos.Y / tan(dworkangle);
		planepnt->AI.TargetPos.Z = dworkvar;

		double deckangle = DECK_ANGLE;
		AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (5.0f * NMTOWU), 10, deckangle);

		maxheight = planepnt->AI.TargetPos.Y + (1000 * FTTOWU);

		planepnt->AI.Behaviorfunc = AIFlyCarrierMarshallPattern;

		tdistft = ((planepnt->WorldPosition - planepnt->AI.WayPosition) * WUTOFT) + (5.0f * NMTOWU);

		flags = AIDESIREDSPDCALC;
		planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->WorldPosition.Y * WUTOFT, 350, flags);

		dworkvar = (tdistft / planepnt->DesiredSpeed) + 180;

		if((dworkvar / 60) >= pushtime)
		{
			pushtime = (dworkvar / 60) + 1;
		}
	}
	else
	{
		if(!foundone)
		{
			planepnt->AI.lTimer3 = 60000;
			planepnt->AI.TargetPos.Y = (1000.0f * FTTOWU);
			pushtime = 1;
		}
		else
		{
			if(maxheight < (1000.0f * FTTOWU))
			{
				maxheight = (1000.0f * FTTOWU);
			}
			planepnt->AI.TargetPos.Y = maxheight + (500 * FTTOWU);

			pushtime /= 60000;

			pushtime += 2;
		}

		orgpushtime = pushtime;

		angels = (planepnt->AI.TargetPos.Y * WUTOFT) / 500.0f;

		dworkangle = (planepnt->AI.TargetPos.Y / ((angels + 5.0f) * NMTOWU));

		if(fabs(dworkangle) > 1.0f)
		{
			dworkangle = 1.0f;
		}

		dworkangle = asin(dworkangle);
		dworkvar = planepnt->AI.TargetPos.Y / tan(dworkangle);
		planepnt->AI.TargetPos.Z = dworkvar;

		double deckangle = DECK_ANGLE;
		AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (1.0f * NMTOWU), 10, deckangle);

		maxheight = planepnt->AI.TargetPos.Y + (500 * FTTOWU);

		planepnt->AI.Behaviorfunc = AIFlyCarrierHeloMarshallPattern;

		tdistft = ((planepnt->WorldPosition - planepnt->AI.WayPosition) * WUTOFT) + (1.0f * NMTOWU);

		flags = AIDESIREDSPDCALC;
		planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->WorldPosition.Y * WUTOFT, 100, flags);

		dworkvar = (tdistft / planepnt->DesiredSpeed) + 180;

		if((dworkvar / 60) >= pushtime)
		{
			pushtime = (dworkvar / 60) + 1;
		}

	}

	planepnt->AI.lTimer3 = pushtime * 60000;

	if(MultiPlayer)
	{
		NetPutGenericMessage3Long(planepnt, GM3L_GIVE_MARSHAL, planepnt->AI.TargetPos.Y * WUTOFT, pushtime);
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		pushtime ++;
	}
	else
	{
		pushtime += 3;
	}

//	char tstr[80];
//	sprintf(tstr, "Plane %d, push minutes %d", planepnt - Planes, pushtime);
//	AICAddAIRadioMsgs(tstr, 50);

	if(MultiPlayer)
	{
		delaycnt += 1000 + ((planepnt - Planes) * 50.0f);
	}
	else
	{
		delaycnt += frand() * 5000.0f;
	}

	if((planepnt != PlayerPlane) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && (lvar2org < 0))))
	{
		AICAddSoundCall(AICStrikeContactMarshal, (planepnt - Planes), delaycnt, 40, 1);
		delaycnt += 5000.0f;
	}

	AICAddSoundCall(AICNotifyShipInbound, (planepnt - Planes), delaycnt, 40, 2);
	delaycnt += 10000.0f;

	long minutes = ((((int)WorldParams.WorldTime%3600)/60) + (planepnt->AI.lTimer3 / 60000)) % 60;

	AICAddSoundCall(AICMarshalGivePushTime, (planepnt - Planes), delaycnt, 40, minutes);
	delaycnt += 30000.0f;

	if(planepnt->AI.wingman >= 0)
	{
		AIMarshallRestOfGroup(&Planes[planepnt->AI.wingman], &maxheight, &pushtime, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIMarshallRestOfGroup(&Planes[planepnt->AI.nextpair], &maxheight, &pushtime, &delaycnt);
	}

	//  This is in the wrong spot, try to remember where this was and if I fixed what this was for.
	pushtime -= (orgpushtime + 1);
	pushtime *= 60000;

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		checkplane = &Planes[0];
		while (checkplane <= LastPlane)
		{
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide))
			{
				if((checkplane != planepnt) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (checkplane->AI.iHomeBaseId == planepnt->AI.iHomeBaseId))
				{
					if(checkplane->AI.Behaviorfunc == AIFlyCarrierHeloMarshallPattern)
					{
						helopushtime = checkplane->AI.lTimer3 += pushtime;
					}
				}
			}
			checkplane ++;
		}
	}
}

//**************************************************************************************
void AIMarshallRestOfGroup(PlaneParams *planepnt, float *maxheight, long *pushtime, long *delaycnt)
{
	double dworkvar, dworkangle, angels;
	DBShipType *pshiptype;
	MovingVehicleParams *carrier;


	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	pshiptype = &pDBShipList[carrier->iVDBIndex];

	if(planepnt->AI.Behaviorfunc == AIFormationFlyingLinked)
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			planepnt->AI.lTimer3 = *pushtime * 60000;
			*pushtime = *pushtime + 1;
			planepnt->AI.TargetPos.Y = *maxheight;
			*maxheight = *maxheight + (1000 * FTTOWU);
			planepnt->AI.Behaviorfunc = AIFlyCarrierMarshallPattern;

			angels = (planepnt->AI.TargetPos.Y * WUTOFT) / 1000.0f;

			dworkangle = (planepnt->AI.TargetPos.Y / ((angels + 15.0f) * NMTOWU));

			if(fabs(dworkangle) > 1.0f)
			{
				dworkangle = 1.0f;
			}

			dworkangle = asin(dworkangle);
			dworkvar = planepnt->AI.TargetPos.Y / tan(dworkangle);
			planepnt->AI.TargetPos.Z = dworkvar;

			double deckangle = DECK_ANGLE;
			AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (5.0f * NMTOWU), 10, deckangle);

			AICAddSoundCall(AICNotifyShipInbound, (planepnt - Planes), *delaycnt, 40, 2);
			*delaycnt = *delaycnt + 10000.0f;

			long minutes = ((((int)WorldParams.WorldTime%3600)/60) + (planepnt->AI.lTimer3 / 60000)) % 60;
			AICAddSoundCall(AICMarshalGivePushTime, (planepnt - Planes), *delaycnt, 40, minutes);
			*delaycnt = *delaycnt + 30000.0f;
		}
		else if(pshiptype->lShipType & ~(SHIP_TYPE_CARRIER))
		{
			planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
			planepnt->AI.lVar2 = 0;
			planepnt->AI.lTimer3 = *pushtime * 60000;
			*pushtime = *pushtime + 3;
		}
		else
		{
			planepnt->AI.lTimer3 = *pushtime * 60000;
			*pushtime = *pushtime + 3;
			planepnt->AI.TargetPos.Y = *maxheight;
			*maxheight = *maxheight + (500 * FTTOWU);
			planepnt->AI.Behaviorfunc = AIFlyCarrierHeloMarshallPattern;

			angels = (planepnt->AI.TargetPos.Y * WUTOFT) / 500.0f;  //  Helos in by 500 ft

			dworkangle = (planepnt->AI.TargetPos.Y / ((angels + 5.0f) * NMTOWU));

			if(fabs(dworkangle) > 1.0f)
			{
				dworkangle = 1.0f;
			}

			dworkangle = asin(dworkangle);
			dworkvar = planepnt->AI.TargetPos.Y / tan(dworkangle);
			planepnt->AI.TargetPos.Z = dworkvar;

			double deckangle = DECK_ANGLE;
			AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (1.0f * NMTOWU), 10, deckangle);

			AICAddSoundCall(AICNotifyShipInbound, (planepnt - Planes), *delaycnt, 40, 2);
			*delaycnt = *delaycnt + 10000.0f;

			long minutes = ((((int)WorldParams.WorldTime%3600)/60) + (planepnt->AI.lTimer3 / 60000)) % 60;
			AICAddSoundCall(AICMarshalGivePushTime, (planepnt - Planes), *delaycnt, 40, minutes);
			*delaycnt = *delaycnt + 30000.0f;
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		AIMarshallRestOfGroup(&Planes[planepnt->AI.wingman], maxheight, pushtime, delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AIMarshallRestOfGroup(&Planes[planepnt->AI.nextpair], maxheight, pushtime, delaycnt);
	}
}

//**************************************************************************************
void AIFlyCarrierMarshallPattern(PlaneParams *planepnt)
{
	float tdist;
	double offangle;
	double dy;
	MovingVehicleParams *carrier;
	double fworkvar, degsec;
	char tstr[80];
	double deckangle = DECK_ANGLE;

	sprintf(tstr, "Somethings wrong");

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	if(carrier->lAIFlags1 & V_ON_FIRE)
	{
		CTSendToDivertField(planepnt);
		return;
	}

	MBAdjustAIThrust(planepnt, 350, 1);

	if(planepnt->AI.lTimer3 < 0)
	{
		// Start Push
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.Behaviorfunc = AIFlyToDME10;
			AICCommencingPush(planepnt - Planes);
			planepnt->AI.lVar2 = 0;
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
			}
//			sprintf(tstr, "Plane %d, Push", planepnt - Planes);
//			AICAddAIRadioMsgs(tstr, 50);
			return;
		}
	}

	if((planepnt->AI.lVar2 == 10) || (planepnt->AI.lVar2 < 3))
	{
		tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);

		if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 45.0f))
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage2Float(planepnt, GM2F_TARGET_Y, planepnt->AI.TargetPos.Y);
				NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
			}

			switch(planepnt->AI.lVar2)
			{
				case 1:
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 2, deckangle);
//					sprintf(tstr, "Plane %d, now Heading front corner", planepnt - Planes);
					break;
				case 2:
					if(planepnt->AI.lTimer3 < 10000)
					{
						//  Start Push
						if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							planepnt->AI.Behaviorfunc = AIFlyToDME10;
							AICCommencingPush(planepnt - Planes);
							planepnt->AI.lVar2 = 0;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
							}
							return;
						}
						else
						{
							planepnt->AI.lTimer3 = 50000;
						}
					}

					if(planepnt->AI.lTimer3 < 50000)
					{
						planepnt->AI.TargetPos.X = 360.0f / ((float)planepnt->AI.lTimer3 / 1000.0f);
						planepnt->AI.lTimer2 = planepnt->AI.lTimer3;
						planepnt->AI.lVar2 = 6;
//						sprintf(tstr, "Plane %d, now Turning towards back Sort Of", planepnt - Planes);
					}
					else
					{
						planepnt->AI.TargetPos.X = 180.0f / 20.0f;
						planepnt->AI.lTimer2 = 20000;
						planepnt->AI.lVar2 = 3;
//						sprintf(tstr, "Plane %d, now Turning towards back", planepnt - Planes);
					}
					break;
				case 10:
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
					tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
					AICEstablishedMarshal(planepnt - Planes);
//					sprintf(tstr, "Plane %d, now starting up FB side", planepnt - Planes);
					break;
			}
//			AICAddAIRadioMsgs(tstr, 50);
		}
		else if(planepnt->AI.lVar2 == 10)
		{
			AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (5.0f * NMTOWU), 10, deckangle);
		}
	}
	else
	{
		if(planepnt->AI.lVar2 == 4)
		{
			dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;

			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			offangle = AIConvertAngleTo180Degree(carrier->Heading + 0x8000) + deckangle - AIConvertAngleTo180Degree(planepnt->Heading);

			offangle = AICapAngle(offangle);

			AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
		}
		else
		{
			if(fabs(planepnt->AI.TargetPos.X) <= planepnt->YawRate)
			{
				degsec = planepnt->AI.TargetPos.X;
			}
			else
			{
				degsec = (planepnt->AI.TargetPos.X < 0) ? -planepnt->YawRate : planepnt->YawRate;
			}
			fworkvar = asin(degsec / planepnt->YawRate);
			fworkvar = RadToDeg(fworkvar);
			planepnt->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
			planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;

			dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
			planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
		}

		if(planepnt->AI.lTimer2 < 0)
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage2Float(planepnt, GM2F_TARGET_Y, planepnt->AI.TargetPos.Y);
				NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
			}

			switch(planepnt->AI.lVar2)
			{
				case 3:
					if(planepnt->AI.lTimer3 < 330000)
					{
						planepnt->AI.lTimer2 = ((planepnt->AI.lTimer3 - 20000) / 2);
						if(planepnt->AI.lTimer2 < 0)
						{
							planepnt->AI.lTimer2 = 1;
						}
//						sprintf(tstr, "Plane %d, now starting back leg SHORT", planepnt - Planes);
					}
					else
					{
						planepnt->AI.lTimer2 = 160000;
//						sprintf(tstr, "Plane %d, now starting back leg", planepnt - Planes);
					}
					planepnt->AI.lVar2 = 4;
					break;
				case 4:
					planepnt->AI.TargetPos.X = 180.0f / 20.0f;
					planepnt->AI.lTimer2 = 20000;
					planepnt->AI.lVar2 = 5;
//					sprintf(tstr, "Plane %d, now turning towards front", planepnt - Planes);
					break;
				case 5:
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
					tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
//					sprintf(tstr, "Plane %d, now starting FB correction", planepnt - Planes);
					break;
				case 6:
					//  Start Push
					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						planepnt->AI.Behaviorfunc = AIFlyToDME10;
						AICCommencingPush(planepnt - Planes);
						planepnt->AI.lVar2 = 0;
						if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
						}
//						sprintf(tstr, "Plane %d, Push", planepnt - Planes);
//						AICAddAIRadioMsgs(tstr, 50);
						return;
					}
					break;
			}
//			AICAddAIRadioMsgs(tstr, 50);
		}
	}
}

//**************************************************************************************
void AISetOffCarrierRelPoint(PlaneParams *planepnt, float xoff, float yoff, float zoff, int setvar, double addangle)
{
	FPoint workpos;
	MovingVehicleParams *carrier;
	double heading;
	double angxx, angxz, angzx, angzz;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	workpos = carrier->WorldPosition;

	heading = DegToRad(((double)carrier->Heading / DEGREE) + addangle);

	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	workpos.X = carrier->WorldPosition.X + (angxx * xoff) + (angzx * zoff);
//	planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y);
	workpos.Y = yoff;
	workpos.Z = carrier->WorldPosition.Z + (-(angxz * xoff) + (angzz * zoff));
	planepnt->AI.WayPosition = workpos;
	planepnt->AI.lVar2 = setvar;
}

//**************************************************************************************
void AIFlyToDME10(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float dx, dy, dz;
	double deckangle = DECK_ANGLE;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;

	MBAdjustAIThrust(planepnt, 250, 1);

#if 0
	AISetOffCarrierRelPoint(planepnt, LAND_POINT_X, LAND_POINT_Y, LAND_POINT_Z, planepnt->AI.lVar2);
#else
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

	tdist = (tdistorg) * 0.75f;
	AISetOffCarrierRelPoint(planepnt, 0, landy, tdist, planepnt->AI.lVar2, deckangle);
#endif

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

	if(planepnt->WorldPosition.Y > (5000.0f * FTTOWU))
	{
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 4000.0f);
	}
	else if(planepnt->WorldPosition.Y > (1200.0f * FTTOWU))
	{
		if((planepnt->AI.lVar2 == 0) && (planepnt != PlayerPlane))
		{
			AICPlatform(planepnt - Planes);
			planepnt->AI.lVar2 = 1;
		}
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 2000.0f);
	}
	else
	{
		dy = AICheckSafeAlt(planepnt, (1200.0f * FTTOWU)) - planepnt->WorldPosition.Y;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	}

	if((tdistorg < (10.0f * NMTOWU)) && (planepnt != PlayerPlane))
	{
		planepnt->AI.Behaviorfunc = AIFlyToDME6;
		AICLandingDistance(planepnt - Planes, 10);

//		char tstr[80];
//		sprintf(tstr, "Plane %d, DME 10", planepnt - Planes);
//		AICAddAIRadioMsgs(tstr, 50);
	}
}

//**************************************************************************************
void AIFlyToDME6(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float dx, dy, dz;
	double offFB, rdist, offFBP;
	double deckangle = DECK_ANGLE;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20.0f, 1);

#if 0
	AISetOffCarrierRelPoint(planepnt, LAND_POINT_X, LAND_POINT_Y, LAND_POINT_Z, planepnt->AI.lVar2);
#else
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

	tdist = (tdistorg) * 0.75f;
	AISetOffCarrierRelPoint(planepnt, 0, landy, tdist, planepnt->AI.lVar2, deckangle);
#endif
	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	offFB = AIDegreesOffGlideSlope(planepnt, &rdist, &offFBP);

#if 0
	offangle += offFB;
#endif

	offangle = AICapAngle(offangle);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

	if(planepnt->WorldPosition.Y > (5000.0f * FTTOWU))
	{
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 4000.0f);
	}
	else if(planepnt->WorldPosition.Y > (1220.0f * FTTOWU))
	{
		if((planepnt->AI.lVar2 == 0) && (planepnt != PlayerPlane))
		{
			AICPlatform(planepnt - Planes);
			planepnt->AI.lVar2 = 1;
		}
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 2000.0f);
	}
	else
	{
		dy = AICheckSafeAlt(planepnt, (1200.0f * FTTOWU)) - planepnt->WorldPosition.Y;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	}

	planepnt->AI.iAIFlags2 |= AILANDING;
	if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(GEAR, ON, 1);
		}
		RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
	}
	if(!planepnt->Flaps)
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(FLAPS, ON, 1);
		}
		planepnt->Flaps = 30.0f;
		planepnt->FlapsCommandedPos = 30.0;
	}

	if(!planepnt->TailHookState)
	{
		planepnt->TailHookState = 1;
//		if(planepnt == (PlaneParams *)PlayerPlane)
//		{
//			DisplayWarning(FLAPS, ON, 1);
//		}
//		planepnt->TailHook = 90.0f;
		planepnt->TailHookCommandedPos = 90.0;
	}

	float desiredpitch;
	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);

	if(desiredpitch >= (AIDESIREDAOA / 2.0f))
	{
		desiredpitch = (AIDESIREDAOA / 2.0f);
	}
	else
	{
		desiredpitch += 0.0005 * DeltaTicks;
	}

	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);

	if(tdistorg < (6.0f * NMTOWU))
	{
		planepnt->AI.Behaviorfunc = AIFlyToDME3;

//		char tstr[80];
//		sprintf(tstr, "Plane %d, DME 6", planepnt - Planes);
//		AICAddAIRadioMsgs(tstr, 50);
	}
}

//**************************************************************************************
void AIFlyToDME3(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float dx, dy, dz;
	double offFB, rdist, offFBP;
	double deckangle = DECK_ANGLE;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20, 1);

	AISetOffCarrierRelPoint(planepnt, landx, landy, landz, planepnt->AI.lVar2);

	offFB = AIDegreesOffGlideSlope(planepnt, &rdist, &offFBP);

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

	if((tdistorg < (5.0f * NMTOWU)) && (planepnt->AI.lVar2 < 2) && (planepnt != PlayerPlane))
	{
		RetractTowedDecoy(planepnt);

		if(carrier->lAIFlags1 & V_ON_FIRE)
		{
			CTSendToDivertField(planepnt);
			return;
		}

		AIC_ACLSContact(planepnt - Planes);
		planepnt->AI.lVar2 = 2;
	}

//	tdist = (tdistorg) * 0.75f;
//	AISetOffCarrierRelPoint(planepnt, 0, LAND_POINT_Y, tdist, 30, deckangle);
	AISetOffCarrierRelPoint(planepnt, 0, landy, 3.0f * NMTOWU, planepnt->AI.lVar2, deckangle);
//	AISetOffCarrierRelPoint(planepnt, 0, LAND_POINT_Y, 1.5f * NMTOWU, 30, deckangle);
//	AISetOffCarrierRelPoint(planepnt, 0, LAND_POINT_Y, 0.75f * NMTOWU, 30, deckangle);

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

#if 0
	offangle += offFB;
#endif

	offangle = AICapAngle(offangle);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

	//  I was doing stuff to 600 but other stuff says use 1200 instead
	if(planepnt->WorldPosition.Y > (5000.0f * FTTOWU))
	{
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 4000.0f);
	}
	else if(planepnt->WorldPosition.Y > (1220.0f * FTTOWU))
	{
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 2000.0f);
	}
	else
	{
		dy = AICheckSafeAlt(planepnt, (1200.0f * FTTOWU)) - planepnt->WorldPosition.Y;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
	}

	planepnt->AI.iAIFlags2 |= AILANDING;

//	if(tdistorg < (3.0f * NMTOWU))
	if(AIInNextRange(planepnt, tdist * WUTOFT,  1000))
	{
//		planepnt->AI.Behaviorfunc = AIFlyToDME3Q4;
		planepnt->AI.Behaviorfunc = AIFlyCarrierFinal;
		planepnt->Status &= ~PL_ONGROUND_NEXT;

		if(planepnt != PlayerPlane)
		{
			AICCheckLSOForPlane(planepnt, 1);
		}

//		char tstr[80];
//		sprintf(tstr, "Plane %d, DME 3", planepnt - Planes);
//		AICAddAIRadioMsgs(tstr, 50);

#if 0
		if((planepnt - Planes) == 1)
		{
			cplaceholder = InstantiatePlaceHolder(planepnt->AI.WayPosition);
		}
#endif
		planepnt->AI.iVar1 = 30;
	}

	float desiredpitch;
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

	if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(GEAR, ON, 1);
		}
		RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
	}
	if(!planepnt->Flaps)
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(FLAPS, ON, 1);
		}
		planepnt->Flaps = 30.0f;
		planepnt->FlapsCommandedPos = 30.0;
	}

	if(!planepnt->TailHookState)
	{
		planepnt->TailHookState = 1;
//		if(planepnt == (PlaneParams *)PlayerPlane)
//		{
//			DisplayWarning(FLAPS, ON, 1);
//		}
//		planepnt->TailHook = 90.0f;
		planepnt->TailHookCommandedPos = 90.0;
	}

}

//**************************************************************************************
void AIFlyToDME3Q4(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float dx, dy, dz;
	double offFB, rdist, offFBP;
	double deckangle = DECK_ANGLE;
	double tempdpitch;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20, 1);

	AISetOffCarrierRelPoint(planepnt, landx, landy, landz, planepnt->AI.lVar2);
	offFB = AIDegreesOffGlideSlope(planepnt, &rdist, &offFBP);

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

//	tdist = (tdistorg) * 0.75f;
//	AISetOffCarrierRelPoint(planepnt, 0, LAND_POINT_Y, tdist, 30, deckangle);
//	AISetOffCarrierRelPoint(planepnt, 0, LAND_POINT_Y, 3.0f * NMTOWU, 30, deckangle);
//	AISetOffCarrierRelPoint(planepnt, 0, LAND_POINT_Y, 1.5f * NMTOWU, 30, deckangle);
	AISetOffCarrierRelPoint(planepnt, 0, landy, 0.75f * NMTOWU, planepnt->AI.lVar2, deckangle);

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

#if 0
	offangle += offFB;
#endif

	offangle = AICapAngle(offangle);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

	tempdpitch = offFBP - dGlideSlope;

	if(fabs(tempdpitch) < 0.5f)
	{
		planepnt->DesiredPitch = AIConvert180DegreeToAngle(-offFBP);
	}
	else
	{
		planepnt->DesiredPitch = AIConvert180DegreeToAngle((-offFBP) - tempdpitch);
	}

	planepnt->AI.iAIFlags2 |= AILANDING;

//	if(tdistorg < (3.0f * NMTOWU))
	if(AIInNextRange(planepnt, tdist * WUTOFT,  500))
	{
		planepnt->AI.Behaviorfunc = AIFlyCarrierFinal;
		planepnt->Status &= ~PL_ONGROUND_NEXT;

//		char tstr[80];
//		sprintf(tstr, "Plane %d, 3 quarter mile", planepnt - Planes);
//		AICAddAIRadioMsgs(tstr, 50);

#if 0
		if((planepnt - Planes) == 1)
		{
			cplaceholder = InstantiatePlaceHolder(planepnt->AI.WayPosition);
		}
#endif
		planepnt->AI.iVar1 = 30;
	}

	float desiredpitch;
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
}

void CalculateRollPitchandHeight(MovingVehicleParams *vehiclepnt,FPointDouble &WorldPosition,DWORD game_loop,double &roll,double &pitch,double &depression);

//**************************************************************************************
void AIFlyCarrierFinal(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float actdistft;
	float dx, dy, dz;
	double offFB, rdist, offFBP;
	double tempdpitch;
	double tx, ty, tz;
	double movewu;
	double deckangle = DECK_ANGLE;
	double secs;
	double FBHeading;
//	char tstr[80];
	double adjangle, adjpitch;
	double moveft;
	double dworkvar;
	FPointDouble new_carrier_position;
	double new_roll;
	double new_pitch;
	double depression;
	float carrierfps;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	//ShowLandingGates(planepnt);

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20, 1);

	carrierfps = (carrier->Status & VL_MOVING) ? carrier->fSpeedFPS : 0;

	AISetOffCarrierRelPoint(planepnt, landx, landy, landz, planepnt->AI.lVar2);

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

	actdistft = (tdistorg) * WUTOFT;
	secs = actdistft / planepnt->IfHorzVelocity;

	if ((secs > 0.0) && (secs < 12.5))
	{
		tdistorg = sqrt(dx*dx+dz*dz);
		actdistft = (tdistorg) * WUTOFT;

		secs = actdistft / (planepnt->IfHorzVelocity+carrierfps);


//		dworkvar = (planepnt->WorldPosition.X-planepnt->LastWorldPosition.X);
//		new_pitch = (planepnt->WorldPosition.Z-planepnt->LastWorldPosition.Z);
//		dworkvar = sqrt(dworkvar*dworkvar + new_pitch*new_pitch)*50;

//		dworkvar *= WUTOFT;

//		secs = actdistft / (dworkvar+carrier->fSpeedFPS);
		dworkvar = (double)ANGLE_TO_RADIANS(carrier->Heading);

		new_carrier_position.SetValues(-sin(dworkvar),0.0,cos(dworkvar));
		new_carrier_position *= carrierfps*secs*FTTOWU;
		new_carrier_position += carrier->WorldPosition;
		new_carrier_position.Y = LandHeight(carrier->WorldPosition.X, carrier->WorldPosition.Z);

		CalculateRollPitchandHeight(carrier,new_carrier_position,GameLoop + (DWORD)(secs*50),new_roll,new_pitch,depression);

		depression -= sin(new_pitch)*landz;
		depression += sin(new_roll)*landx;

		AISetOffCarrierRelPoint(planepnt, landx, landy+depression, landz, planepnt->AI.lVar2);

		dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
		dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
		tdistorg = QuickDistance(dx,dz);

		actdistft = (tdistorg) * WUTOFT;
		secs = actdistft / (planepnt->IfHorzVelocity+carrierfps);

	}

#if 1  //  Removed for E3, but back afterwards
	if((tdistorg < (NMTOWU * 0.6f)) && (planepnt == (PlaneParams *)Camera1.AttachedObject) && (planepnt != PlayerPlane))
	{
		AICCheckLSOForPlane(planepnt);
	}

	if((tdistorg < (0.95f * NMTOWU)) && (planepnt->AI.lVar2 < 3) && (planepnt != PlayerPlane))
	{
		if(AICDeckClear(planepnt))
		{
			AICCallBall(planepnt - Planes);
			AICAddSoundCall(AICSeeBall, (planepnt - Planes), 4000, 40, -1);
			planepnt->AI.lVar2 = 3;
		}
	}
#endif

	tx = planepnt->AI.WayPosition.X - carrier->WorldPosition.X;
	ty = planepnt->AI.WayPosition.Y - carrier->WorldPosition.Y;
	tz = planepnt->AI.WayPosition.Z - carrier->WorldPosition.Z;

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;

#if 0
	if((cplaceholder) && ((planepnt - Planes) == 1))
	{
		cplaceholder->Pos = planepnt->AI.WayPosition;
		cplaceholder->LifeTime = 2000;
	}
#endif

	offFB = AIDegreesOffGlideSlope(planepnt, &rdist, &offFBP);

//	offangle = atan2(-dx, -dz) * 57.2958;
//	offangle -= AIConvertAngleTo180Degree(carrier->Heading);
//	offangle = fabs(offangle);
	offangle = fabs(deckangle);

	actdistft = (tdistorg) * WUTOFT;
	secs = actdistft / planepnt->IfHorzVelocity;

#if 0
	if(secs < 16)  // was 8
	{
		moveft = carrierfps * sin(DegToRad(offangle)) * 1.2f;
	}
	else
	{
//		moveft = carrierfps * sin(DegToRad(offangle)) * 1.8f;
		moveft = carrierfps * sin(DegToRad(offangle)) * 1.2f;
	}
#else
	moveft = carrierfps * sin(DegToRad(offangle));
#endif

	if(moveft)
	{
		adjangle = fabs(atan2(-moveft, -planepnt->IfHorzVelocity) * 57.2958f);
	}
	else
	{
		adjangle = 0;
	}

	if(adjangle > 90.0f)
	{
		adjangle = 180.0f - adjangle;
	}
	adjangle = -adjangle;

	if((offFBP > -1.0f) && (secs >= 8))
	{
		moveft = carrierfps * sin(DegToRad(90.0f - fabs(offangle)));

		dworkvar = (sin(DegToRad(dGlideSlope)) * planepnt->V) ;

		dworkvar = RadToDeg(asin(dworkvar / (planepnt->V + moveft)));

		adjpitch = dGlideSlope - dworkvar;
	}
	else
	{
		adjpitch = 0;
	}


	FBHeading = AIConvertAngleTo180Degree(carrier->Heading) + deckangle;
	AIOffsetFromPoint(&planepnt->AI.WayPosition, FBHeading, 0, 0, -500.0f * FTTOWU);

	if(secs < 8)
	{
		movewu = (-carrierfps * secs) * FTTOWU;

		AIOffsetFromPoint(&planepnt->AI.WayPosition, AIConvertAngleTo180Degree(carrier->Heading), 0, 0, movewu);
	}

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	if(secs >= 8)
	{
		offangle += adjangle;
	}

//	if(offFB < 0)
//	{
//		offangle -= 5.0f;
//	}

//	if(offFB < 0)
//	{
//		offangle -= 5.0f;
//	}

	if((offFB < 0) && (rdist > (300.0f * FTTOWU)))
	{
		double dworkvar;

		dworkvar = sin(DegToRad(offFB)) * rdist;
		if((offFB < 0) && (rdist > (1000.0f * FTTOWU)) && (dworkvar <= (rdist / 3.0f)))
		{
//			adjangle = fabs(RadToDeg(asin(dworkvar / (rdist / 2.0f))));
			adjangle = fabs(RadToDeg(asin(dworkvar / (rdist / 3.0f))));
		}
		else if(dworkvar <= (rdist / 2.0f))
		{
//			adjangle = fabs(RadToDeg(asin(dworkvar / rdist)));
			adjangle = fabs(RadToDeg(asin(dworkvar / (rdist / 2.0f))));
		}
		else
		{
			adjangle = 0;
			if(planepnt == PlayerPlane)
			{
				UFC.APStatus |= 0x3;
			}
		}
		offangle -= adjangle;
	}

#if 0
	if((cplaceholder) && ((planepnt - Planes) == 1))
	{
		cplaceholder->Pos = planepnt->AI.WayPosition;
		cplaceholder->LifeTime = 2000;
	}
#endif

	if(planepnt->WorldPosition.Y < planepnt->AI.WayPosition.Y)
	{
		planepnt->DesiredPitch = 0;
		planepnt->DesiredRoll = 0;
	}
	else
	{
#if 0
		if(tdistorg <= (3000.0f * FTTOWU))
		{
			offangle += offFB;
		}
#endif

		offangle = AICapAngle(offangle);

		planepnt->DesiredRoll = planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

		if(tdistorg <= (200.0f * FTTOWU))
		{
			planepnt->DesiredRoll = 0;
		}
		else if(tdistorg <= (500.0f * FTTOWU))
		{
			if(planepnt->DesiredRoll > 0x8000)
			{
				if(planepnt->DesiredRoll < 0xF000)
				{
					planepnt->DesiredRoll = 0xF000;
				}
			}
			else
			{
				if(planepnt->DesiredRoll > 0x1000)
				{
					planepnt->DesiredRoll = 0x1000;
				}
			}
		}
		else
		{
#if 0
			if(tdistorg > (0.75f * NMTOWU))
			{
				if(planepnt->WorldPosition.Y > (5000.0f * FTTOWU))
				{
					planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 4000.0f);
				}
				else if(planepnt->WorldPosition.Y > (600.0f * FTTOWU))
				{
					planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, 2000.0f);
				}
				else
				{
					dy = AICheckSafeAlt(planepnt, (600.0f * FTTOWU)) - planepnt->WorldPosition.Y;
					planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
				}
			}
			else
			{
#endif
				tempdpitch = offFBP - dGlideSlope;

				if(fabs(tempdpitch) < 0.15f)
				{
					planepnt->DesiredPitch = AIConvert180DegreeToAngle(-offFBP + adjpitch);
				}
				else
				{
					if((planepnt->WorldPosition.Y >= (590.0f * FTTOWU)) && (tempdpitch < 0))
					{
						dy = AICheckSafeAlt(planepnt, (600.0f * FTTOWU)) - planepnt->WorldPosition.Y;
						planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
					}
					else
					{
						planepnt->DesiredPitch = AIConvert180DegreeToAngle(((-offFBP) - tempdpitch) + adjpitch);
					}
				}
//			}
		}
	}

	planepnt->AI.iAIFlags2 |= AILANDING;

	//CalcHeightAboveDeck(planepnt, carrier);
	if ((planepnt->OnGround==2) || (planepnt->Status & PL_ONGROUND_NEXT))
	{
		planepnt->OnGround = 2;
		planepnt->Status &= ~PL_ONGROUND_NEXT;
		planepnt->AI.Behaviorfunc = AIWaitForTrapOrBolter;
		planepnt->AI.lVar3 = 0;
		carrier->lAIFlags1 |= PLANES_ON_DECK;

		planepnt->DesiredPitch = carrier->Pitch;
		planepnt->DesiredRoll = 0;
		planepnt->AI.DesiredRoll = 0;
		planepnt->MaxPitchRate = 70.0f;

		if (!planepnt->PlaneCopy)	/*on carrier*/
			planepnt->PlaneCopy = GetNewCarrierPlane(planepnt,carrier);
	}
	else
		if((fabs(offangle) > 90.0f) && (tdistorg < (1000.0f * FTTOWU)))
		{
			planepnt->AI.Behaviorfunc = AIFlyBolter;
			planepnt->AI.lTimer3 = 120000;
			fBolterAdjust += (3.0f * FTTOWU);
//			ClearLandingGates();
		}

	float desiredpitch;
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

	if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(GEAR, ON, 1);
		}
		RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
	}
	if(!planepnt->Flaps)
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(FLAPS, ON, 1);
		}
		planepnt->Flaps = 30.0f;
		planepnt->FlapsCommandedPos = 30.0;
	}

	if(!planepnt->TailHookState)
	{
		planepnt->TailHookState = 1;
//		if(planepnt == (PlaneParams *)PlayerPlane)
//		{
//			DisplayWarning(FLAPS, ON, 1);
//		}
//		planepnt->TailHook = 90.0f;
		planepnt->TailHookCommandedPos = 90.0;
	}
}

//**************************************************************************************
void AIFlyBolter(PlaneParams *planepnt)
{
	MovingVehicleParams *carrier;
	double FBHeading;
	double deckangle = DECK_ANGLE;
	double dy;
	double offangle;

	if((planepnt == PlayerPlane) || (planepnt->Status & (PL_COMM_DRIVEN|PL_DEVICE_DRIVEN)) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
	{
		AIC_ACLS_Switch(PlayerPlane, 0);
	}

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

//	CalcHeightAboveDeck(planepnt, carrier);
#if 0
	if (planepnt->OnGround || (planepnt->Status & PL_ONGROUND_NEXT))
	{
		planepnt->OnGround = 2;
		planepnt->Status &= ~PL_ONGROUND_NEXT;
		planepnt->AI.Behaviorfunc = AIWaitForTrapOrBolter;
		planepnt->AI.lVar3 = 0;
		carrier->lAIFlags1 |= PLANES_ON_DECK;
		if (!planepnt->PlaneCopy)	/*on carrier*/
			planepnt->PlaneCopy = GetNewCarrierPlane(planepnt,carrier);
		return;
	}
#else
	planepnt->OnGround = 0;
#endif

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20, 1);

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	FBHeading = AIConvertAngleTo180Degree(carrier->Heading) + deckangle;


	if(planepnt->WorldPosition.Y < (500.0f * WUTOFT))
	{
		planepnt->DesiredRoll = 0;
	}
	else
	{
		offangle = AIConvertAngleTo180Degree(FBHeading - planepnt->Heading);
		planepnt->DesiredRoll = planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
	}

	dy = AICheckSafeAlt(planepnt, (1200.0f * FTTOWU)) - planepnt->WorldPosition.Y;
	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);

	if(planepnt->AI.lTimer3 < 0)
	{
//		AIContactCarrierMarshallPattern(planepnt);
		planepnt->AI.DesiredPitch = 0;
		planepnt->AI.Behaviorfunc = AIQuickCarrierLanding;
		planepnt->AI.OrgBehave = NULL;
		AISetOffCarrierRelPoint(planepnt, (-2.0f * NMTOWU), planepnt->WorldPosition.Y, 0, 0, deckangle);
		if(planepnt == PlayerPlane)
		{
			planepnt->AI.lVar2 = 12;
		}
		else
		{
			planepnt->AI.lVar2 = 0;
		}
	}
}

//**************************************************************************************
ANGLE AIGetPitchANGLEForFPMDescent(PlaneParams *planepnt, double fpm)
{
	double vel = planepnt->V;
	double returnval;
	double fps = (fpm) / 60.0f;

	returnval = RadToDeg(asin((-fps) / vel));

	return(AIConvert180DegreeToAngle(returnval));
}

//**************************************************************************************
double AIDegreesOffGlideSlope(PlaneParams *planepnt, double *rdist, double *pitch, FPoint *deckpoint)
{
	double dx, dy, dz;
	double offangle;
	float tdist;
	MovingVehicleParams *carrier;
	double FBHeading;
	double deckangle = DECK_ANGLE;
	double offpitch;
	double gear_height;
	double add_height;

	gear_height = planepnt->Type->GearDownHeight * cos(DegToRad(dGlideSlope));

	add_height = planepnt->Type->ShadowLRXOff * fabs(sin(DegToRad(dGlideSlope)));

	if(add_height < gear_height)
		add_height = gear_height;

	if((add_height * 0.75f) > fBolterAdjust)
		add_height -= fBolterAdjust;
	else
		add_height *= 0.25f;

	if(deckpoint)
	{
		dx = planepnt->WorldPosition.X - deckpoint->X;
		dy = (planepnt->WorldPosition.Y - add_height) - deckpoint->Y;
		dz = planepnt->WorldPosition.Z - deckpoint->Z;
	}
	else
	{
		dx = planepnt->WorldPosition.X - planepnt->AI.WayPosition.X;
		dy = (planepnt->WorldPosition.Y - add_height) - planepnt->AI.WayPosition.Y;
		dz = planepnt->WorldPosition.Z - planepnt->AI.WayPosition.Z;
	}

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	FBHeading = AIConvertAngleTo180Degree(carrier->Heading) + deckangle + 180.0f;

	offangle -= FBHeading;

	offangle = AICapAngle(offangle);

	tdist = QuickDistance(dx,dz);

	if(pitch)
	{
		offpitch = (atan2(dy, tdist) * 57.2958);
		*pitch = offpitch;
	}

//	*rdx = dx;
//	*rdy = dy;
//	*rdz = dz;

	if(rdist)
	{
		*rdist = tdist;
	}

	return(offangle);

}

//**************************************************************************************
//  If you just want to offset a distance in a dir, just use zoff and set xoff and yoff to 0
//**************************************************************************************
void AIOffsetFromPoint(FPoint *location, double offangle, float xoff, float yoff, float zoff)
{
	double heading;
	heading = DegToRad(offangle);
	double angxx, angxz, angzx, angzz;

	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	location->X = location->X + (angxx * xoff) + (angzx * zoff);
//	planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y);
	location->Y = location->Y + yoff;
	location->Z = location->Z + (-(angxz * xoff) + (angzz * zoff));
}

//**************************************************************************************
//  If you just want to offset a distance in a dir, just use zoff and set xoff and yoff to 0
//**************************************************************************************
void AIOffsetFromPoint(FPointDouble *location, double offangle, float xoff, float yoff, float zoff)
{
	double heading;
	heading = DegToRad(offangle);
	double angxx, angxz, angzx, angzz;

	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	location->X = location->X + (angxx * xoff) + (angzx * zoff);
//	planepnt->AI.WorldFormationPos.Y = leadposition.Y + ConvertWayLoc(planepnt->AI.FormationPosition.Y);
	location->Y = location->Y + yoff;
	location->Z = location->Z + (-(angxz * xoff) + (angzz * zoff));
}

//**************************************************************************************
void ShowLandingGates(PlaneParams *planepnt, int showgates)
{
	return;
#if 0
	int cnt;
	MovingVehicleParams *carrier;
	FPoint gatepos, orgpos;
	double deckangle = DECK_ANGLE;
	double FBHeading;
	double workangle;
	double tdist;
	double landx = LAND_POINT_X;
	double carrierheight = LAND_POINT_Y;
	double landz = LAND_POINT_Z;
	double tanGlideSlope = tan(DegToRad(dGlideSlope));

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	FBHeading = AIConvertAngleTo180Degree(carrier->Heading);

	if(!pLandingGates[0])
	{
		for(cnt = 0; cnt < showgates; cnt ++)
		{
			pLandingGates[cnt] = InstantiatePlaceHolder(planepnt->AI.WayPosition);
		}
	}

	orgpos = carrier->WorldPosition;
	AIOffsetFromPoint(&orgpos, FBHeading, landx, carrierheight, landz);

	FBHeading += deckangle;
	FBHeading = AICapAngle(FBHeading);

	for(cnt = 0; cnt < showgates; cnt ++)
	{
		gatepos = orgpos;
		workangle = FBHeading;
		switch(cnt)
		{
			case 0:
				pLandingGates[cnt]->Heading = FBHeading;
				break;
			case 1:
//				workangle += 180.0f;
				workangle = AICapAngle(workangle);
				pLandingGates[cnt]->Heading = FBHeading;
				tdist = (0.25f * NMTOWU);
				AIOffsetFromPoint(&gatepos, workangle, 0, (tanGlideSlope * tdist), tdist);
				break;
			case 2:
//				workangle += 180.0f;
				workangle = AICapAngle(workangle);
				pLandingGates[cnt]->Heading = FBHeading;
				tdist = (0.5f * NMTOWU);
				AIOffsetFromPoint(&gatepos, workangle, 0, (tanGlideSlope * tdist), tdist);
				break;
			case 3:
//				workangle += 180.0f;
				workangle = AICapAngle(workangle);
				pLandingGates[cnt]->Heading = FBHeading;
				tdist = (0.75f * NMTOWU);
				AIOffsetFromPoint(&gatepos, workangle, 0, (tanGlideSlope * tdist), tdist);
				break;
			default:
				pLandingGates[cnt]->Heading = FBHeading;
				break;
		}
		pLandingGates[cnt]->Pos = gatepos;
		pLandingGates[cnt]->LifeTime = 1000;
		pLandingGates[cnt]->Attitude.SetHPR( pLandingGates[cnt]->Heading, pLandingGates[cnt]->Pitch, 0 );
	}
#endif
}

//**************************************************************************************
void ClearLandingGates()
{
	int cnt;

	for(cnt = 0; cnt < MAX_GATES; cnt ++)
	{
		if(pLandingGates[cnt])
		{
			DeletePlaceHolder(pLandingGates[cnt]);
		}
		pLandingGates[cnt] = NULL;
	}
}

void AIWaitForTrapOrBolter(PlaneParams *planepnt)
{
	float fworkval;
	float desiredpitch;

	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);
	planepnt->DesiredPitch = 0;
	planepnt->DesiredRoll = 0;
	planepnt->AI.DesiredRoll = 0;
	planepnt->MaxPitchRate = 90.0f;

	if(desiredpitch <= 0)
	{
		desiredpitch = 0;
	}
	else
	{
#if 0
		pitchpercent = 1 - (desiredpitch / AIDESIREDAOA);

		pitchchange = 0.02 * pitchpercent;

		if(pitchchange < 0.002)
		{
			pitchchange = 0.002;
		}
		else if(pitchchange > 0.02)
		{
			pitchchange = 0.02;
		}
		desiredpitch -= pitchchange * DeltaTicks;
#else
		fworkval = AIDESIREDAOA / 250.0f;
#endif
		desiredpitch -= fworkval * (float)DeltaTicks;

		if(desiredpitch < 0)
		{
			desiredpitch = 0;
		}
	}
	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);

}

void AIUnhookFromTrapWire(PlaneParams *planepnt)
{
	if ((planepnt->AI.lTimer2 < 1000) && !planepnt->AI.lVar3)
	{
		planepnt->AI.lVar3 = 1;
		if(planepnt->TailHookState)
		{
			planepnt->TailHookState = 0;
			planepnt->TailHookCommandedPos = 0.0;
		}
	}

	if (planepnt->AI.lTimer2 < 0)
	{
		planepnt->AI.lVar3 = 0;
		if(((planepnt != PlayerPlane) && (planepnt->AI.numwaypts > 1) && (!AIInPlayerGroup(planepnt))) || ((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) && (g_Settings.gp.dwCheats & GP_CHEATS_MULTI_REARM)))
		{
			CTCheckForRelaunch(planepnt);
			if((MultiPlayer) && (planepnt == PlayerPlane))
				NetPutGenericMessage1(planepnt, GM_SET_RELAUNCH);
		}
		else
			planepnt->AI.Behaviorfunc = AIMovePlaneToDeArmZone;
	}
}

//**************************************************************************************
void AICarrierTrap(PlaneParams *planepnt)
{
	float desiredpitch;
//	float pitchpercent, pitchchange;
	float fworkval;
	MovingVehicleParams *carrier;

	if(planepnt == PlayerPlane)
	{
		if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED|PL_PLANE_DITCHING))
		{
			AIC_ACLS_Switch(planepnt, 0);
		}
	}

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

//	CalcHeightAboveDeck(planepnt, carrier);

	desiredpitch = AIConvertAngleTo180Degree(planepnt->AI.DesiredPitch);
	planepnt->DesiredPitch = 0;
	planepnt->DesiredRoll = 0;
	planepnt->AI.DesiredRoll = 0;
	planepnt->MaxPitchRate = 90.0f;

	if(desiredpitch <= 0)
	{
		desiredpitch = 0;
	}
	else
	{
#if 0
		pitchpercent = 1 - (desiredpitch / AIDESIREDAOA);

		pitchchange = 0.02 * pitchpercent;

		if(pitchchange < 0.002)
		{
			pitchchange = 0.002;
		}
		else if(pitchchange > 0.02)
		{
			pitchchange = 0.02;
		}
		desiredpitch -= pitchchange * DeltaTicks;
#else
		fworkval = AIDESIREDAOA / 250.0f;
#endif
		desiredpitch -= fworkval * (float)DeltaTicks;

		if(desiredpitch < 0)
		{
			desiredpitch = 0;
		}
	}
	planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredpitch);

	if(planepnt->AI.lVar3 == 0)
	{
		if(planepnt->V > 0.1f)
		{
			planepnt->DesiredSpeed = 0;
			if ((planepnt == PlayerPlane) || ((planepnt->Status & PL_CARRIER_REL) && (PlayerPlane->PlaneCopy == planepnt)))
			{
				if (planepnt->Status & PL_SET_PLAYER_DECEL)
				{
					planepnt->MaxSpeedAccel = 0.25*(planepnt->V*planepnt->V)/400.0f ;  // this should stop us at around 400 feet .25 instead of .5 cuz scott x2 for dec
					planepnt->Status &= ~PL_SET_PLAYER_DECEL;
				}
			}
			else
			{
				planepnt->MaxSpeedAccel = 45;  //  was 40 but 4 wire guys were getting real close to the egdge;

				if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
				{
					if(planepnt->AI.fVarA[6] < (-160.0f * FTTOWU))	//  Getting too close to end.  Mainly for Client MP AI planes.
					{
						planepnt->BfLinVel.X = planepnt->V = 0;
					}
				}
				else if(planepnt->AI.fVarA[6] < (-192.0f * FTTOWU))	//  Getting too close to end.  Mainly for Client MP AI planes.
				{
					planepnt->BfLinVel.X = planepnt->V = 0;
				}
			}

			planepnt->AI.lTimer2 = 2000;
		}
		else
		{
			/* -----------------6/22/99 5:40PM---------------------------------------------------------------------
			 * add code here to wait for human to release the throttle
			 * ----------------------------------------------------------------------------------------------------*/
//			if(((planepnt == PlayerPlane) || (planepnt == (PlaneParams *)Camera1.AttachedObject)) && ((planepnt->AI.lTimer2 + DeltaTicks) == 2000))
//			{
//				AICJustMessage(AIC_CUT, AICF_CUT, SPCH_LSO);
//			}

			if((planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) && (planepnt->LeftThrustPercent >= 20.0f))
			{
				if((!MultiPlayer) || (planepnt == PlayerPlane))
				{
					planepnt->AI.lVar3 = 4;
					if(MultiPlayer)
						NetPutGenericMessage2Long(planepnt, GM2L_UPDATE_LVAR3, planepnt->AI.lVar3);
				}
			}
			else if(planepnt->AI.lTimer2 < 0)
			{
				if((!MultiPlayer) || (planepnt == PlayerPlane) || (!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				{
					planepnt->AI.lVar3 = 1;
					if((MultiPlayer) && (planepnt == PlayerPlane))
						NetPutGenericMessage2Long(planepnt, GM2L_UPDATE_LVAR3, planepnt->AI.lVar3);
				}
			}
			planepnt->DesiredSpeed = 0;
		}
	}
	else if(planepnt->AI.lVar3 == 4)
	{
		planepnt->DesiredSpeed = 0;
		if(planepnt->LeftThrustPercent < 20.0f)
		{
			planepnt->AI.lVar3 = 5;
		}
		else
		{
			planepnt->AI.lTimer2 += DeltaTicks;
		}
	}
	else if(planepnt->AI.lVar3 == 5)
	{
		planepnt->DesiredSpeed = 0;
		if(planepnt->AI.lTimer2 < 0)
		{
			if((!MultiPlayer) || (planepnt == PlayerPlane) || (!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
			{
				planepnt->AI.lVar3 = 1;
				if((MultiPlayer) && (planepnt == PlayerPlane))
					NetPutGenericMessage2Long(planepnt, GM2L_UPDATE_LVAR3, planepnt->AI.lVar3);
			}
		}
	}
	else
	{
		planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration;

		if(planepnt->AI.lVar3 == 1)
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, -10, 0);
			if(planepnt->V <= (planepnt->DesiredSpeed+0.01))
			{
				planepnt->AI.lVar3 = 2;
			}
		}
		else if(planepnt->AI.lVar3 == 2)
		{
			planepnt->DesiredSpeed = 0;
			if ((planepnt->V != 0) && (planepnt->V < (AIConvertSpeedToFtPS(planepnt->Altitude, -10, 0) / 2.0f)))
				planepnt->MaxSpeedAccel /= 4.0f;
//			else if((planepnt->V == 0) && (!(planepnt->TailHookFlags & TH_HOOKED)))
			else if(planepnt->V == 0)
			{
				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) && (g_Settings.gp.dwCheats & GP_CHEATS_MULTI_REARM))
				{
					CTCheckForRelaunch(planepnt);
				}
				else
				{
					planepnt->AI.Behaviorfunc = AIMovePlaneToDeArmZone;
				}
			}
		}
	}
}

//**************************************************************************************
void AIMovePlaneToDeArmZone(PlaneParams *planepnt)
{
	double offangle;
	float offx, offz;
	float tdist;
	float dx, dz;
	MovingVehicleParams *carrier;

	if(planepnt->TailHookState)
	{
		planepnt->TailHookState = 0;
		planepnt->TailHookCommandedPos = 0.0;
	}

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	planepnt->AI.iAIFlags2 |= AILANDING;

//	planepnt->YawRate = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate * airdensity;
	planepnt->YawRate = 45.0f;

	if(planepnt->AI.lVar3 == 0)
	{
		if(pDBShipList[carrier->iVDBIndex].lShipID != 6)
		{
			offx = -64 * FTTOWU;
			offz = -160 * FTTOWU;
		}
		else
		{
			offx = -80 * FTTOWU;
			offz = -60 * FTTOWU;
		}
	}
	else
	{
		if(pDBShipList[carrier->iVDBIndex].lShipID != 6)
		{
			offx = 96 * FTTOWU;
			offz = -160 * FTTOWU;
		}
		else
		{
			offx = 80 * FTTOWU;
			offz = -60 * FTTOWU;
		}
	}

	dx = offx - planepnt->AI.fVarA[4];
	dz = offz - planepnt->AI.fVarA[6];

	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	tdist = QuickDistance(dx,dz);

//	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);
	offangle -= (planepnt->AI.fVarA[7]);

	offangle = AICapAngle(offangle);

	planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
	planepnt->DesiredRoll = 0;
	planepnt->DesiredPitch = 0;

	if(planepnt->AI.lVar3 == 0)
	{
		if((tdist < (5 * FTTOWU)) || (dz > 0))
		{
			planepnt->AI.lVar3 = 1;
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
		if(fabs(offangle) < 30.0f)
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 10, 0);
		}
		else
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 5, 0);
		}
	}
	else if(planepnt->AI.lVar3 == 2)
	{
		planepnt->DesiredSpeed = 0;
		planepnt->AI.DesiredRoll = 0;
		if(planepnt->AI.lTimer2 < 0)
		{
			if(planepnt->WingUpCommandedPos < 1.0f)
			{
		//		if(planepnt == (PlaneParams *)PlayerPlane)
		//		{
		//			DisplayWarning(FLAPS, ON, 1);
		//		}
		//		planepnt->TailHook = 90.0f;
				planepnt->WingUpCommandedPos = 120.0f;
			}

			planepnt->AI.Behaviorfunc = AIMoveToParkingSpace;
			AIGetCarrierParkingSpace(planepnt);
		}
	}
	else if(tdist < (5 * FTTOWU))
	{
		planepnt->DesiredSpeed = 0;
		planepnt->AI.lTimer2 = 20000;
		planepnt->AI.lVar3 = 2;
	}
	else
	{
		if(fabs(offangle) < 30.0f)
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 10, 0);
		}
		else
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 5, 0);
		}
	}

}

//**************************************************************************************
void AIGetCarrierParkingSpace(PlaneParams *planepnt)
{
	int cnt;
	int slotnum = -1;
	float offx, offz, tempx, tempz;
	float parkangle;
	MovingVehicleParams *carrier;
	int iworkplanesondeck = iMaxPlanesOnDeck;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		AIGetSovCarrierParkingSpace(planepnt);
		return;
	}

	planepnt->AI.lVar3 = 1;

	if(!((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
	{
		if(MultiPlayer)
		{
			iworkplanesondeck -= MAX_PLAYERS;
		}
		else
		{
			iworkplanesondeck -= 1;
		}
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		for(cnt = 0; cnt < 32; cnt ++)
		{
			if((iworkplanesondeck <= 0) && (cnt < 25))
			{
				cnt = 25;
			}

			if(!((1<<cnt) & carrier->lVar1))
			{
				slotnum = cnt + 1;
				carrier->lVar1 |= (1<<cnt);
				break;
			}
			if(cnt > (iworkplanesondeck - 2))
			{
				if(cnt < 24)
					cnt = 24;
			}
		}
	}
	else
	{
		slotnum = 99;
	}

	switch(slotnum)
	{
		case 1:
			offx = -25;
			offz = -512;
			parkangle = -30.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 2:
			offx = -29;
			offz = -459;
			parkangle = -30.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 3:
			offx = -33;
			offz = -406;
			parkangle = -30.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 4:
			offx = -37;
			offz = -353;
			parkangle = -30.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 5:
			offx = -37;
			offz = -300;
			parkangle = -30.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 6:
			offx = -26;
			offz = -247;
			parkangle = -30.0f;
			tempx = offx;
			tempz = -215;
			planepnt->AI.lVar3 = 0;
			break;
		case 7:
			offx = 32;
			offz = -507;
			parkangle = 25.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 9:		//  8
			offx = 36;
			offz = -451;
			parkangle = 25.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 11:		//  9
			offx = 40;
			offz = -399;
			parkangle = 25.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 13:	//  10
			offx = 45;
			offz = -347;
			parkangle = 25.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 15:	//  11
			offx = 49;
			offz = -295;
			parkangle = 30.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 1;
			break;
		case 8:	//  12
			offx = 5;
			offz = -520;
			parkangle = 0.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 10:	//  13
			offx = 5;
			offz = -450;
			parkangle = 0.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 12:	//  14
			offx = 5;
			offz = -380;
			parkangle = 0.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 14:	//  15
			offx = 10;
			offz = -320;
			parkangle = 10.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 16:
			offx = 58;
			offz = -245;
			parkangle = 45.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 2;
			break;
		case 17:
			offx = 17;
			offz = 102;
			parkangle = 155.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 18:
			offx = 13;
			offz = 62;
			parkangle = 145.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 19:
			offx = 9;
			offz = 24;
			parkangle = 145.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 20:
			offx = 5;
			offz = -16;
			parkangle = 145.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 21:
			offx = 51;
			offz = 0;
			parkangle = 175.0f;
			tempx = 60;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 22:
			offx = 0;
			offz = -53;
			parkangle = 135.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 1;
			break;
		case 23:
			offx = 51;
			offz = -60;
			parkangle = 175.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 1;
			break;
		case 24:
			offx = -5;
			offz = -91;
			parkangle = 135.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 1;
			break;
		case 25:
			offx = 38;
			offz = -108;
			parkangle = 165.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 1;
			break;
		case 26:
			offx = 98;
			offz = -60;
			parkangle = -90.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 2;
			break;
		case 27:
			offx = 102;
			offz = -96;
			parkangle = 90.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 2;
			break;
		case 99:
			offx = -104;
			offz = 344;
			parkangle = -90.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 2;
			break;
		default:
			offx = 102;
			offz = -96;
			parkangle = 90.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 2;
			break;
	}
	planepnt->AI.fVarA[0] = offx * FTTOWU;
	planepnt->AI.fVarA[1] = offz * FTTOWU;
	planepnt->AI.fVarA[2] = parkangle;
	planepnt->AI.fVarA[3] = tempz * FTTOWU;
	planepnt->AI.fVarA[9] = tempx * FTTOWU;
	planepnt->AI.lVar2 = slotnum;
}

//**************************************************************************************
void AIGetSovCarrierParkingSpace(PlaneParams *planepnt)
{
	int slotnum = -1;
	float offx, offz, tempx, tempz;
	float parkangle;
	MovingVehicleParams *carrier;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	planepnt->AI.lVar3 = 1;

	offx = 80;
	offz = -60;
	parkangle = -90.0f;
	tempx = 0;
	tempz = 0;
	planepnt->AI.lVar3 = 2;

	planepnt->AI.fVarA[0] = offx * FTTOWU;
	planepnt->AI.fVarA[1] = offz * FTTOWU;
	planepnt->AI.fVarA[2] = parkangle;
	planepnt->AI.fVarA[3] = tempz * FTTOWU;
	planepnt->AI.fVarA[9] = tempx * FTTOWU;
	planepnt->AI.lVar2 = slotnum;
}

//**************************************************************************************
void AIMoveToParkingSpace(PlaneParams *planepnt)
{
	double offangle;
	float offx, offz;
	float tdist;
	float dx, dz;
	FPointDouble temppos;


	if(planepnt->AI.lVar3 == 0)
	{
//		planepnt->YawRate = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate;
		planepnt->YawRate = 45.0f;
		offx = planepnt->AI.fVarA[9];
		offz = planepnt->AI.fVarA[3];

		dx = offx - planepnt->AI.fVarA[4];
		dz = offz - planepnt->AI.fVarA[6];

		offangle = atan2(-dx, -dz) * 57.2958;

		offangle = AICapAngle(offangle);

		tdist = QuickDistance(dx,dz);

	//	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);
		offangle -= (planepnt->AI.fVarA[7]);

		offangle = AICapAngle(offangle);

		planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;

		if(tdist < (5.0f * FTTOWU))
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 5, 0);
			planepnt->AI.lVar3 = 1;
		}
		else if(fabs(offangle) < 30.0f)
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 10, 0);
		}
		else
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 3, 0);
		}
	}
	else if(planepnt->AI.lVar3 == 1)
	{
// 		planepnt->YawRate = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate;

		planepnt->YawRate = 45.0f;
		temppos.X = planepnt->AI.fVarA[0];
		temppos.Y = planepnt->WorldPosition.Y;
		temppos.Z = planepnt->AI.fVarA[1];
		AIOffsetFromPoint(&temppos, (double)planepnt->AI.fVarA[2], 0, 0, 20 * FTTOWU);

		offx = temppos.X;
		offz = temppos.Z;

		dx = offx - planepnt->AI.fVarA[4];
		dz = offz - planepnt->AI.fVarA[6];

		offangle = atan2(-dx, -dz) * 57.2958;

		offangle = AICapAngle(offangle);

		tdist = QuickDistance(dx,dz);

	//	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);
		offangle -= (planepnt->AI.fVarA[7]);

		offangle = AICapAngle(offangle);

		planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;

		if(tdist < (10.0f * FTTOWU))
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 3, 0);
			planepnt->AI.lVar3 = 2;
		}
		else if(fabs(offangle) < 10.0f)
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 10, 0);
		}
		else
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 3, 0);
		}
	}
	else if(planepnt->AI.lVar3 == 2)
	{
		if((!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
				|| (planepnt->AI.lTimer2 < 0))
		{
//			planepnt->YawRate = (float)pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate;
			planepnt->YawRate = 45.0f;
			offx = planepnt->AI.fVarA[0];
			offz = planepnt->AI.fVarA[1];

			dx = offx - planepnt->AI.fVarA[4];
			dz = offz - planepnt->AI.fVarA[6];

			offangle = atan2(-dx, -dz) * 57.2958;

			offangle = AICapAngle(offangle);

			tdist = QuickDistance(dx,dz);

		//	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);
			offangle -= (planepnt->AI.fVarA[7]);

			offangle = AICapAngle(offangle);

			planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
			planepnt->DesiredRoll = 0;
			planepnt->DesiredPitch = 0;

			if(tdist < (2.0f * FTTOWU))
			{
				planepnt->DesiredSpeed = 0;
				planepnt->AI.lVar3 = 3;
			}
			else
			{
				planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 3, 0);
			}
		}
	}
	else if(planepnt->AI.lVar3 == 3)
	{
		planepnt->DesiredSpeed = 0;

		offangle = planepnt->AI.fVarA[2] - (planepnt->AI.fVarA[7]);

		offangle = AICapAngle(offangle);

		planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;

		if((ANGLE)(planepnt->AI.DesiredRoll + 0x25) < 0x50)
		{
			planepnt->AI.lVar3 = 4;
			planepnt->AI.lTimer2 = 10000;
			if(planepnt == PlayerPlane)
			{
				ShutBothEnginesDown();
			}
		}
	}
	else
	{
		if((planepnt->AI.lVar3 == 4) && (planepnt->AI.lTimer2 < 0))
		{

			MovingVehicleParams *carrier;

			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			if((planepnt->AI.lVar2 >= 26) || (pDBShipList[carrier->iVDBIndex].lShipID == 6))
			{
				AIVanishPlane(planepnt);
				return;
			}

			planepnt->AI.lVar3 = 5;
			planepnt->AI.lTimer2 = 300000.0f;
			if(planepnt->CanopyCommandedPos < 1.0f)
			{
		//		if(planepnt == (PlaneParams *)PlayerPlane)
		//		{
		//			DisplayWarning(FLAPS, ON, 1);
		//		}
		//		planepnt->TailHook = 90.0f;
				planepnt->CanopyCommandedPos = 45.0f;
			}
		}
		else if((planepnt->AI.lVar3 == 5) && (planepnt->AI.lTimer2 < 0))
		{
			planepnt->AI.lVar3 = 6;
			if(planepnt->CanopyCommandedPos > 1.0f)
			{
		//		if(planepnt == (PlaneParams *)PlayerPlane)
		//		{
		//			DisplayWarning(FLAPS, ON, 1);
		//		}
		//		planepnt->TailHook = 90.0f;
				planepnt->CanopyCommandedPos = 0.0f;
			}
		}
		else if((planepnt->AI.lVar3 == 7) && (planepnt->AI.lTimer2 < 0))
		{
			planepnt->AI.lVar3 = 6;
			AIVanishPlane(planepnt);
		}
		else
		{
		}

		planepnt->AI.DesiredRoll = 0;
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
	}
}

//**************************************************************************************
int CTCheckForCarrierTakeoff(PlaneParams *planepnt, FPointDouble worldpos, FPointDouble *fieldpos, ANGLE *runwayheading, TakeOffActionType *ActionTakeOff, double minfar, int ignoreside, int allow_both_ends, int ignore_destroy)
{
	MovingVehicleParams *carrier;
	int waiting = 0;

	if(planepnt->AI.iAICombatFlags1 & AI_HOME_AIRPORT)
	{
		return(0);
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		if(AIGetClosestAirField(planepnt, worldpos, fieldpos, runwayheading, ActionTakeOff, (5.0f * NMTOWU), 1, 1, AIInPlayerGroup(planepnt)))
		{
			planepnt->WorldPosition.X = fieldpos->X;
			planepnt->WorldPosition.Y = fieldpos->Y;
			planepnt->WorldPosition.Z = fieldpos->Z;

			AISetOrientation(planepnt, AIConvertAngleTo180Degree(*runwayheading));
			return(1);
		}
	}

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(!(carrier->Status & VL_ACTIVE))
	{
		return(0);
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		float tdist = (carrier->WorldPosition - planepnt->WorldPosition) * WUTONM;
		if(tdist > 10.0f)
		{
			return(1);
		}
	}

	CTGetCarrierTakeOffSpace(planepnt);

	return(1);
}

//**************************************************************************************
void CTGetCarrierTakeOffSpace(PlaneParams *planepnt, int useslot)
{
	int cnt;
	int slotnum = useslot;
	float offx, offz, tempx, tempz;
	float offy = 64.0f;
	float parkangle;
	MovingVehicleParams *carrier;
	FPointDouble carrpos;
	double workangle;
	PlaneParams *leadplane;
	DBShipType *pshiptype;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	pshiptype = &pDBShipList[carrier->iVDBIndex];

	if(pshiptype->lShipType & ~(SHIP_TYPE_CARRIER))
	{
		slotnum = -99;
	}
	else if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		CTGetSovCarrierTakeOffSpace(planepnt, useslot);
		return;
	}

	if(slotnum == -1)
	{
		if((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN))
		{
			planepnt->AI.iAICombatFlags1 |= AI_HUMAN_CONTROLLED;
			StartCasualAutoPilotPlane(planepnt);
			slotnum = CTFindSlotForPlane(planepnt, 1);
		}
		else
		{
			leadplane = AIGetLeader(planepnt);
			if((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
			{
				slotnum = CTFindSlotForPlane(planepnt, 2);
			}
			else
			{
				slotnum = CTFindSlotForPlane(planepnt, 0);
			}
		}
		if(slotnum == -1)
		{
			return;
		}
	}

	if(planepnt->AI.OrgBehave == NULL)
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
	}

	planepnt->AI.Behaviorfunc = CTWaitingForLaunch;

	planepnt->AI.lVar3 = 1;

#if 0
	if(useslot == -1)
	{
		for(cnt = 0; cnt < 32; cnt ++)
		{
			if(!((1<<cnt) & carrier->lVar2))
			{
				slotnum = cnt + 1;
				carrier->lVar2 |= (1<<cnt);
				break;
			}
		}
	}
	else
	{
		carrier->lVar2 |= (1<<(slotnum - 1));
	}
#else
	if(slotnum < 32)
	{
		carrier->lVar2 |= (1<<(slotnum - 1));
	}
#endif

	switch(slotnum)
	{
		case -99:		//  Non-carrier with helo deck.
			offx = pshiptype->fHeloX;
			offy = pshiptype->fHeloY + fabs(pDBAircraftList[planepnt->AI.iPlaneIndex].fGearDownHeight);
			offz = pshiptype->fHeloZ;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 20000;
			break;
		case -2:		//  FirstHelo
			offx = -64.0f;
			offz = -80.0;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 1500;  //  20000;
			break;
		case 1:
			offx = 52.0f;
			offz = -201.5;
			parkangle = 4.712f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 10000;
			carrier->bFXArray[2] = 255;
			carrier->bFXDesiredArray[2] = 255;
			break;
		case 2:
			offx = -13.5;
			offz = -188;
			parkangle = 1.45f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 30000;
			carrier->bFXArray[3] = 255;
			carrier->bFXDesiredArray[3] = 255;
			break;
		case 3:
			offx = -65.89136;
			offz = 111.57729;
			parkangle = 4.45f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 50000;
			carrier->bFXArray[4] = 255;
			carrier->bFXDesiredArray[4] = 255;
			break;
		case 4:
			offx = -109;
			offz = 169;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 70000;
			carrier->bFXArray[5] = 255;
			carrier->bFXDesiredArray[5] = 255;
			break;
		case 5:
			offx = 56;
			offz = -102;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 1;
			break;
		case 6:
			offx = -8;
			offz = -70;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -215;
			planepnt->AI.lVar3 = 1;
			break;
		case 7:
			offx = -46;
			offz = 218;
			parkangle = 30.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 1;
			break;
		case 8:
			offx = -101;
			offz = 280;
			parkangle = 30.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 1;
			break;
		case 9:
			offx = 93;
			offz = -42;
			parkangle = 90.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 10:
			offx = 27;
			offz = -13;
			parkangle = 17.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 11:
			offx = 12;
			offz = 251;
			parkangle = 25.0f;
			tempx = 51;
			tempz = -230;
			planepnt->AI.lVar3 = 0;
			break;
		case 12:
			offx = -96;  //  -100;
			offz = 352;  //  331;
			parkangle = -40.0f;  //  -90.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 13:
			offx = 92;
			offz = 1;
			parkangle = 90.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 14:
			offx = 15;
			offz = 44;
			parkangle = 25.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 15:
			offx = -21;
			offz = 315;
			parkangle = 2.0f;
			tempx = 20;
			tempz = -240;
			planepnt->AI.lVar3 = 0;
			break;
		case 16:
			offx = -62;  //  -99;
			offz = 386;  //  377;
			parkangle = -25.0f;  //  -50.0f;
			tempx = 0;
			tempz = 0;
			planepnt->AI.lVar3 = 0;
			break;
		case 17:
			offx = 94;
			offz = 47;
			parkangle = 90.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 18:
			offx = 93;
			offz = 92;
			parkangle = 90.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 19:
			offx = 19;
			offz = 330;
			parkangle = 30.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		case 20:
			offx = -19;
			offz = 416;
			parkangle = -5.0f;
			tempx = 40;
			tempz = -130;
			planepnt->AI.lVar3 = 0;
			break;
		default:
			cnt = slotnum % 4;

			if((cnt == 1) || (cnt == 2))
			{
				offx = 98;
				offz = 77;
				parkangle = 90.0f;
			}
			else
			{
				offx = -107;
				offz = 357;  //  347;
				parkangle = -90.0f;
			}
			tempz = 0;
			planepnt->AI.iAIFlags2 |= AIINVISIBLE;
			planepnt->Status &= ~PL_ACTIVE;
			planepnt->AI.lVar3 = 0;
			break;
	}

	planepnt->OnGround = 2;
	planepnt->AI.fVarA[0] = offx * FTTOWU;
	planepnt->AI.fVarA[1] = offz * FTTOWU;
	planepnt->AI.fVarA[2] = parkangle;
	planepnt->AI.fVarA[3] = tempz * FTTOWU;
	planepnt->AI.fVarA[9] = tempx * FTTOWU;

	planepnt->AI.fVarA[4] = planepnt->AI.fVarA[0];
	planepnt->AI.fVarA[5] = offy * FTTOWU;
	planepnt->AI.fVarA[6] = planepnt->AI.fVarA[1];

	carrpos = carrier->WorldPosition;
	workangle = AIConvertAngleTo180Degree(carrier->Heading);
	AIOffsetFromPoint(&carrpos, (double)workangle, offx * FTTOWU, offy * FTTOWU, offz * FTTOWU);
	planepnt->Heading = carrier->Heading + AIConvert180DegreeToAngle(parkangle);
	workangle = AIConvertAngleTo180Degree(planepnt->Heading);
	if(slotnum < 5)
		AIOffsetFromPoint(&carrpos, (double)workangle, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].X * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z * FTTOWU);
	else
		carrpos.Y += pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU;

	planepnt->WorldPosition = carrpos;

	planepnt->Attitude.SetRPH(0,0,planepnt->Heading);

	planepnt->Orientation.I.X = -planepnt->Attitude.m_Data.RC.R0C2;
	planepnt->Orientation.I.Y = -planepnt->Attitude.m_Data.RC.R1C2;
	planepnt->Orientation.I.Z = -planepnt->Attitude.m_Data.RC.R2C2;

	planepnt->Orientation.J.X = planepnt->Attitude.m_Data.RC.R0C0;
	planepnt->Orientation.J.Y = planepnt->Attitude.m_Data.RC.R1C0;
	planepnt->Orientation.J.Z = planepnt->Attitude.m_Data.RC.R2C0;

	planepnt->Orientation.K.X = -planepnt->Attitude.m_Data.RC.R0C1;
	planepnt->Orientation.K.Y = -planepnt->Attitude.m_Data.RC.R1C1;
	planepnt->Orientation.K.Z = -planepnt->Attitude.m_Data.RC.R2C1;

	planepnt->AI.lVar2 = slotnum;

//**  REMOVED since it causes problems when I have to move planes around.  Moved planes would return to original location because of copy.
//	if (!planepnt->PlaneCopy)
//		planepnt->PlaneCopy = GetNewCarrierPlane(planepnt,carrier);
//**  END REMOVED

//	FPointDouble relative_position;
//	FMatrix de_rotate;

//	relative_position.MakeVectorToFrom(planepnt->WorldPosition, carrier->WorldPosition);

//	de_rotate = carrier->Attitude;
//	de_rotate.Transpose();

//	relative_position *= de_rotate;

//	planepnt->AI.fVarA[4] = relative_position.X;
//	planepnt->AI.fVarA[5] = relative_position.Y;
//	planepnt->AI.fVarA[6] = relative_position.Z;

//	planepnt->AI.fVarA[7] = AIConvertAngleTo180Degree(planepnt->Heading - carrier->Heading);
//	planepnt->AI.fVarA[8] = AIConvertAngleTo180Degree(planepnt->Pitch - carrier->Pitch);
////	planepnt->AI.fVarA[9] = AIConvertAngleTo180Degree(P->Roll - carrier->Roll);

	carrier->lAIFlags1 |= PLANES_ON_DECK;
}

//**************************************************************************************
void CTGetSovCarrierTakeOffSpace(PlaneParams *planepnt, int useslot)
{
	int cnt;
	int slotnum = useslot;
	float offx, offz, tempx, tempz;
	float offy = 64.0f;
	float parkangle;
	MovingVehicleParams *carrier;
	FPointDouble carrpos;
	double workangle;
	PlaneParams *leadplane;
	DBShipType *pshiptype;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	pshiptype = &pDBShipList[carrier->iVDBIndex];

	if(pshiptype->lShipType & ~(SHIP_TYPE_CARRIER))
	{
		slotnum = -99;
	}

	if(slotnum == -1)
	{
		if((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN))
		{
			planepnt->AI.iAICombatFlags1 |= AI_HUMAN_CONTROLLED;
			StartCasualAutoPilotPlane(planepnt);
			slotnum = CTFindSlotForSovPlane(planepnt, 1);
		}
		else
		{
			leadplane = AIGetLeader(planepnt);
			if((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
			{
				slotnum = CTFindSlotForSovPlane(planepnt, 2);
			}
			else
			{
				slotnum = CTFindSlotForSovPlane(planepnt, 0);
			}
		}
		if(slotnum == -1)
		{
			return;
		}
	}

	if(planepnt->AI.OrgBehave == NULL)
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
	}

	planepnt->AI.Behaviorfunc = CTWaitingForLaunch;

	planepnt->AI.lVar3 = 1;

#if 0
	if(useslot == -1)
	{
		for(cnt = 0; cnt < 32; cnt ++)
		{
			if(!((1<<cnt) & carrier->lVar2))
			{
				slotnum = cnt + 1;
				carrier->lVar2 |= (1<<cnt);
				break;
			}
		}
	}
	else
	{
		carrier->lVar2 |= (1<<(slotnum - 1));
	}
#else
	if(slotnum < 32)
	{
		carrier->lVar2 |= (1<<(slotnum - 1));
	}
#endif

	switch(slotnum)
	{
		case -99:		//  Non-carrier with helo deck.
			offx = pshiptype->fHeloX;
			offz = pshiptype->fHeloZ;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 20000;
			break;
		case -2:		//  FirstHelo
			offx = -60.0f;
			offz = 0.0;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 20000;
			break;
		case 1:
			offx = 45.572f;
			offz = -169.309f;
			parkangle = 8.4f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 10000;
			carrier->bFXArray[2] = 255;
			carrier->bFXDesiredArray[2] = 255;
			break;
		case 2:
			offx = -45.572f;
			offz = -169.309f;
			parkangle = -8.4f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 100;
			planepnt->AI.lTimer2 = 50000;  //  30000;
			carrier->bFXArray[3] = 255;
			carrier->bFXDesiredArray[3] = 255;
			break;
		case 5:
			offx = 35;
			offz = 0;
			parkangle = 0.0f;
			tempx = offx;
			tempz = -230;
			planepnt->AI.lVar3 = 1;
			break;
		case 6:
			offx = -20;
			offz = 0;
			parkangle = 10.0f;
			tempx = offx;
			tempz = -215;
			planepnt->AI.lVar3 = 1;
			break;
		default:
			cnt = slotnum % 4;

			offx = 70;
			offz = 230;
			parkangle = 90.0f;

			tempz = 0;
			planepnt->AI.iAIFlags2 |= AIINVISIBLE;
			planepnt->Status &= ~PL_ACTIVE;
			planepnt->AI.lVar3 = 0;
			break;
	}

	planepnt->OnGround = 2;
	planepnt->AI.fVarA[0] = offx * FTTOWU;
	planepnt->AI.fVarA[1] = offz * FTTOWU;
	planepnt->AI.fVarA[2] = parkangle;
	planepnt->AI.fVarA[3] = tempz * FTTOWU;
	planepnt->AI.fVarA[9] = tempx * FTTOWU;

	planepnt->AI.fVarA[4] = planepnt->AI.fVarA[0];
	planepnt->AI.fVarA[5] = offy * FTTOWU;
	planepnt->AI.fVarA[6] = planepnt->AI.fVarA[1];

	carrpos = carrier->WorldPosition;
	workangle = AIConvertAngleTo180Degree(carrier->Heading);
	AIOffsetFromPoint(&carrpos, (double)workangle, offx * FTTOWU, offy * FTTOWU, offz * FTTOWU);
	planepnt->Heading = carrier->Heading + AIConvert180DegreeToAngle(parkangle);
	workangle = AIConvertAngleTo180Degree(planepnt->Heading);
	if(slotnum < 5)
		AIOffsetFromPoint(&carrpos, (double)workangle, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].X * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z * FTTOWU);
	else
		carrpos.Y += pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU;

	planepnt->WorldPosition = carrpos;

	planepnt->Attitude.SetRPH(0,0,planepnt->Heading);

	planepnt->Orientation.I.X = -planepnt->Attitude.m_Data.RC.R0C2;
	planepnt->Orientation.I.Y = -planepnt->Attitude.m_Data.RC.R1C2;
	planepnt->Orientation.I.Z = -planepnt->Attitude.m_Data.RC.R2C2;

	planepnt->Orientation.J.X = planepnt->Attitude.m_Data.RC.R0C0;
	planepnt->Orientation.J.Y = planepnt->Attitude.m_Data.RC.R1C0;
	planepnt->Orientation.J.Z = planepnt->Attitude.m_Data.RC.R2C0;

	planepnt->Orientation.K.X = -planepnt->Attitude.m_Data.RC.R0C1;
	planepnt->Orientation.K.Y = -planepnt->Attitude.m_Data.RC.R1C1;
	planepnt->Orientation.K.Z = -planepnt->Attitude.m_Data.RC.R2C1;

	planepnt->AI.lVar2 = slotnum;

//**  REMOVED since it causes problems when I have to move planes around.  Moved planes would return to original location because of copy.
//	if (!planepnt->PlaneCopy)
//		planepnt->PlaneCopy = GetNewCarrierPlane(planepnt,carrier);
//**  END REMOVED

//	FPointDouble relative_position;
//	FMatrix de_rotate;

//	relative_position.MakeVectorToFrom(planepnt->WorldPosition, carrier->WorldPosition);

//	de_rotate = carrier->Attitude;
//	de_rotate.Transpose();

//	relative_position *= de_rotate;

//	planepnt->AI.fVarA[4] = relative_position.X;
//	planepnt->AI.fVarA[5] = relative_position.Y;
//	planepnt->AI.fVarA[6] = relative_position.Z;

//	planepnt->AI.fVarA[7] = AIConvertAngleTo180Degree(planepnt->Heading - carrier->Heading);
//	planepnt->AI.fVarA[8] = AIConvertAngleTo180Degree(planepnt->Pitch - carrier->Pitch);
////	planepnt->AI.fVarA[9] = AIConvertAngleTo180Degree(P->Roll - carrier->Roll);

	carrier->lAIFlags1 |= PLANES_ON_DECK;
}

extern void	LaunchAircraftSound(PlaneParams *planepnt);

//**************************************************************************************
void CTWaitingForLaunch(PlaneParams *planepnt)
{
	double orgoffangle, offangle;
	float offx, offz;
	float tdist;
	float dx, dz;
	FPointDouble temppos;
	long lworkvar, lworkvar2;
	MovingVehicleParams *carrier;
	int slotnum;

	planepnt->AI.DesiredRoll = 0;
	planepnt->DesiredPitch = 0;
	planepnt->DesiredRoll = 0;
	planepnt->DesiredSpeed = 0;

	planepnt->AI.iAIFlags2 |= AILANDING;

	SetLandingGearUpDown(planepnt,LOWER_LANDING_GEAR);

	if((planepnt->AI.lVar3 == 100) && (planepnt->AI.lTimer2 < 0) && ((!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)) || (planepnt->AI.iAICombatFlags2 & AI_READY_FOR_LAUNCH)))
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			LaunchAircraftSound(planepnt);
			LogMissionStatistic(LOG_FLIGHT_CARRIER_TAKEOFF,(planepnt-Planes),1,0);
		}
		else
		{
			if(AIWaitingForTakeOffFlag(planepnt))
			{
				return;
			}
		}

		if(MultiPlayer && (g_bIAmHost))
		{
			NetPutGenericMessage3Long(planepnt, GM3L_TAKEOFF_TIME, planepnt->AI.lTimer2, planepnt->AI.lTimer3);
		}

		carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
		CTCheckTakeOffQueue(planepnt);
		planepnt->AI.Behaviorfunc = CTDoCarrierLaunch;
		planepnt->AI.lVar3 = 0;
		if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
		{
			planepnt->AI.lTimer2 = 99000;
		}
		else
		{
			planepnt->AI.lTimer2 = 2000;
		}
		planepnt->AI.cFXarray[1] = 0;

		if(carrier->lAIFlags1 & CAT_4_BLOCKED)
		{
			slotnum = (planepnt->AI.lVar2 - 1) % 4;
			if(slotnum == 2)
			{
				carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
				carrier->lAIFlags1 &= ~CAT_4_BLOCKED;
			}
		}

		if(planepnt == PlayerPlane)
		{
			planepnt->AI.lTimer2 += 500;
		}
	}
	else if(planepnt->AI.lVar3 == 100)
	{
		if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
		{
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
			slotnum = (planepnt->AI.lVar2 - 1) % 4;

			if((planepnt->AI.lTimer2 < 0) && (slotnum == 2))
			{
				carrier->lAIFlags1 |= CAT_4_BLOCKED;
			}
			else if((slotnum == 3) && (carrier->lAIFlags1 & CAT_4_BLOCKED))
			{
				planepnt->AI.lTimer2 += DeltaTicks;
			}

			slotnum += 2;
			carrier->bFXDesiredArray[slotnum] = 255;
			if(carrier->bFXArray[slotnum] > 200)
			{
				if((pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 4) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 62))
				{
					planepnt->AI.cFXarray[1] = 0;
				}
			}
		}
	}
//	else if((planepnt->AI.lVar3) && (planepnt->AI.lVar3 != 100))
	else if(planepnt->AI.lVar3)
	{
		if((planepnt->AI.lTimer3 < 0) || ((planepnt->AI.lVar3 == 2) && (planepnt->AI.lTimer3 < 2500)) || ((planepnt->AI.lVar2 < 0) && (planepnt->AI.lTimer3 < 10000)))
		{
			planepnt->AI.iAIFlags2 &= ~AIINVISIBLE;
			if((planepnt->AI.lVar2 < 0) && (planepnt->AI.lTimer3 < 0))
			{
				planepnt->AI.lVar2 = labs(planepnt->AI.lVar2);
			}

			if(planepnt->AI.lVar3 == 2)
			{
				planepnt->YawRate = 20.0f;
			}
			else
			{
				planepnt->YawRate = 45.0f;
			}

			if(planepnt->AI.lVar3 == 99)
			{
				temppos.X = planepnt->AI.fVarA[0];
				temppos.Y = planepnt->WorldPosition.Y;
				temppos.Z = planepnt->AI.fVarA[1];

				AIOffsetFromPoint(&temppos, (double)planepnt->AI.fVarA[2], pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].X * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z * FTTOWU);

				offx = temppos.X;
				offz = temppos.Z;
			}
			else if(planepnt->AI.lVar3 & 1)
			{
				offx = planepnt->AI.fVarA[0];
				offz = planepnt->AI.fVarA[1];
			}
			else if((planepnt->AI.lVar3 == 98) || (planepnt->AI.lVar3 == 96))
			{
				temppos.X = planepnt->AI.fVarA[0];
				temppos.Y = planepnt->WorldPosition.Y;
				temppos.Z = planepnt->AI.fVarA[1];
				if(planepnt->AI.lVar3 == 96)
				{
					AIOffsetFromPoint(&temppos, (double)planepnt->AI.fVarA[2], pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].X * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU, (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z + 80) * FTTOWU);
				}
				else
				{
					AIOffsetFromPoint(&temppos, (double)planepnt->AI.fVarA[2], pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].X * FTTOWU, pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Y * FTTOWU, (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z + 20) * FTTOWU);
				}

				offx = temppos.X;
				offz = temppos.Z;
			}
			else
			{
				temppos.X = planepnt->AI.fVarA[0];
				temppos.Y = planepnt->WorldPosition.Y;
				temppos.Z = planepnt->AI.fVarA[1];
				AIOffsetFromPoint(&temppos, (double)planepnt->AI.fVarA[2], 0, 0, 20 * FTTOWU);

				offx = temppos.X;
				offz = temppos.Z;
			}

			dx = offx - planepnt->AI.fVarA[4];
			dz = offz - planepnt->AI.fVarA[6];

			offangle = atan2(-dx, -dz) * 57.2958;

			offangle = AICapAngle(offangle);

			tdist = QuickDistance(dx,dz);

		//	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);
			offangle -= (planepnt->AI.fVarA[7]);

			orgoffangle = offangle = AICapAngle(offangle);

			if(planepnt->AI.lTimer3 >= 0)
			{
				planepnt->AI.DesiredRoll = 0;
			}
			else if(((tdist > (2.0f * FTTOWU)) || (fabs(offangle) < 10.0f)) && (!((tdist < (0.25f * FTTOWU)) || (fabs(offangle) > 130.0f) || ((planepnt->AI.lVar3 != 99) && (tdist < (2.0f * FTTOWU))))))
			{
				planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
			}
			else
			{
				if(planepnt->AI.lVar3 & 1)
				{
					offangle = planepnt->AI.fVarA[2] - (planepnt->AI.fVarA[7]);

					offangle = AICapAngle(offangle);

					planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, offangle, 0);
				}
				else
				{
					planepnt->AI.DesiredRoll = 0;
				}
			}

			planepnt->DesiredRoll = 0;
			planepnt->DesiredPitch = 0;

			if((tdist < (0.25f * FTTOWU)) || (fabs(orgoffangle) > 130.0f) || ((planepnt->AI.lVar3 != 99) && (tdist < (2.0f * FTTOWU))))
			{
				planepnt->DesiredSpeed = 0;

				if((fabs(offangle) < 1.0f) || (!(planepnt->AI.lVar3 & 1)))
				{
					planepnt->AI.DesiredRoll = 0;
					if(planepnt->AI.lVar3 & 0x1)
					{
						if(planepnt->AI.lVar3 == 99)
						{
							planepnt->AI.fVarA[7] = planepnt->AI.fVarA[2];
							planepnt->Heading += AIConvert180DegreeToAngle(offangle);
							planepnt->Status |= PL_NEED_ATTITUDE;
							CalcAttitude(planepnt);
							planepnt->AI.lVar3 = 100;
							if(MultiPlayer && (g_bIAmHost) && (!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
							{
								NetPutGenericMessage3Long(planepnt, GM3L_TAKEOFF_TIME, planepnt->AI.lTimer2, planepnt->AI.lTimer3);
							}
							if(planepnt == PlayerPlane)
							{
								planepnt->Brakes = 1;
								DisplayWarning(BRAKE, ON, 1);
							}

						}
						else
						{
							planepnt->AI.lVar3 = 0;
						}
					}
					else
					{
						planepnt->AI.lVar3 ++;
						if(planepnt->AI.lVar3 == 97)
						{
							planepnt->AI.lVar3 = 99;
						}

						if(planepnt->AI.lVar2 & 0xFF00)
						{
							lworkvar = (planepnt->AI.lVar2 & 0xFF00)>>8;

							if(lworkvar <= (LastPlane - Planes))
							{
								if(Planes[lworkvar].AI.iAIFlags2 & AIINVISIBLE)
								{
									Planes[lworkvar].AI.iAIFlags2 &= ~AIINVISIBLE;
									Planes[lworkvar].Status |= PL_ACTIVE;
									Planes[lworkvar].AI.lTimer3 = -1;
									planepnt->AI.lVar2 = planepnt->AI.lVar2 & 0xFF;
									lworkvar2 = Planes[lworkvar].AI.lVar2 %4;
									if(lworkvar2 > 2)
									{
										Planes[lworkvar].AI.lTimer3 = 12500;
										Planes[lworkvar].AI.lVar2 *= -1;
									}
									else if(lworkvar2)
									{
										Planes[lworkvar].AI.lTimer3 = 5000;
									}

									MovingVehicleParams *carrier = &MovingVehicles[Planes[lworkvar].AI.iHomeBaseId];

									Planes[lworkvar].WorldPosition.SetValues(Planes[lworkvar].AI.fVarA[4],Planes[lworkvar].AI.fVarA[5],Planes[lworkvar].AI.fVarA[6]);
									Planes[lworkvar].WorldPosition *= carrier->Attitude;
									Planes[lworkvar].WorldPosition += carrier->WorldPosition;

									Planes[lworkvar].Attitude.SetRPH(0,0,Planes[lworkvar].Heading);
									Planes[lworkvar].Attitude *= carrier->Attitude;

									Planes[lworkvar].Orientation.I.X = -Planes[lworkvar].Attitude.m_Data.RC.R0C2;
									Planes[lworkvar].Orientation.I.Y = -Planes[lworkvar].Attitude.m_Data.RC.R1C2;
									Planes[lworkvar].Orientation.I.Z = -Planes[lworkvar].Attitude.m_Data.RC.R2C2;

									Planes[lworkvar].Orientation.J.X = Planes[lworkvar].Attitude.m_Data.RC.R0C0;
									Planes[lworkvar].Orientation.J.Y = Planes[lworkvar].Attitude.m_Data.RC.R1C0;
									Planes[lworkvar].Orientation.J.Z = Planes[lworkvar].Attitude.m_Data.RC.R2C0;

									Planes[lworkvar].Orientation.K.X = -Planes[lworkvar].Attitude.m_Data.RC.R0C1;
									Planes[lworkvar].Orientation.K.Y = -Planes[lworkvar].Attitude.m_Data.RC.R1C1;
									Planes[lworkvar].Orientation.K.Z = -Planes[lworkvar].Attitude.m_Data.RC.R2C1;

									Planes[lworkvar].Attitude.GetRPH(&Planes[lworkvar].Roll,&Planes[lworkvar].Pitch,&Planes[lworkvar].Heading);

									if(Planes[lworkvar].WorldPosition.Y < (70.0f * WUTOFT))
									{
										Planes[lworkvar].WorldPosition.Y = 70.0f * WUTOFT;
									}

									Planes[lworkvar].LastWorldPosition -= Planes[lworkvar].WorldPosition;

									if (!Planes[lworkvar].PlaneCopy)
										Planes[lworkvar].PlaneCopy = GetNewCarrierPlane(&Planes[lworkvar]);
									else
									{
										if(Planes[lworkvar].PlaneCopy->WorldPosition.Y < (70.0f * WUTOFT))
										{
											Planes[lworkvar].PlaneCopy->WorldPosition.Y = 70.0f * WUTOFT;
										}
									}
								}
							}
						}
					}
				}
			}
			else if((tdist < (3.0f * FTTOWU)) && (planepnt->AI.lVar3 & 1))
			{
				planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 1, 0);
			}
//			else if((tdist < (10.0f * FTTOWU)) || ((fabs(offangle) >= 10.0f) && (planepnt->AI.lVar3 != 2)))
			else if((tdist < (10.0f * FTTOWU)) || (fabs(offangle) >= 10.0f))
			{
				planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 3, 0);
			}
			else
			{
				planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, 5, 0);
			}
		}
	}

	planepnt->OnGround = 2;
}

//**************************************************************************************
void CTDoCarrierLaunch(PlaneParams *planepnt)
{
	MovingVehicleParams *carrier;
	double tdist;
	float degspersec, fworkvar;
	float carrang, planeang;
	double dx, dz;
	int isdone = 0;
	int slotnum;
	int orgwaynum;
	DBShipType *pshiptype;
	float desiredAoA;
	ANGLE AdesiredAoA;
	PlaneParams *leadplane;

	if(planepnt->AI.lVar3 == 0)
	{
		planepnt->AI.iAICombatFlags2 &= ~(AI_READY_FOR_LAUNCH);
		if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
		{
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
			planepnt->BfLinVel.X = carrier->fSpeedFPS;
			planepnt->V = carrier->fSpeedFPS;
			planepnt->DesiredPitch = 0x1000;
			planepnt->AI.lVar3 = 10;
			planepnt->AI.lTimer2 = 60000;
			planepnt->OnGround = 0;
			if(planepnt->AI.lVar2 >= 0)
			{
				slotnum = (planepnt->AI.lVar2 - 1) % 4;
				slotnum += 2;
				carrier->bFXDesiredArray[slotnum] = 0;
			}
			return;
		}

		if((pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 4) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 62))
		{
			planepnt->AI.cFXarray[1] = 0;
		}
		planepnt->AI.DesiredRoll = 0;
		planepnt->DesiredPitch = 0;
		planepnt->DesiredRoll = 0;

		carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
		if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
		{
			fworkvar = planepnt->AI.fVarA[6] - (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z * FTTOWU);
			if((planepnt->AI.fVarA[6] + (pDBAircraftList[planepnt->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z * FTTOWU)) < (-481.0f * FTTOWU))
			{
				planepnt->AI.lTimer2 = -1;
			}
			else if(fworkvar < (-438.5f * FTTOWU))
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(9.0f);
//				planepnt->DesiredPitch = AIConvert180DegreeToAngle(7.64f);
			}
			else if(fworkvar < (-375.0f * FTTOWU))
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(7.64f);
//				planepnt->DesiredPitch = AIConvert180DegreeToAngle(3.2f);
			}
			else if(fworkvar < (-300.0f * FTTOWU))
			{
				planepnt->DesiredPitch = AIConvert180DegreeToAngle(0.92f);
			}
		}

		if(planepnt->AI.lTimer2 < 0)
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, (300), 0);
			planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration;
			planepnt->AI.lVar3 = 1;
			planepnt->OnGround = 0;
			planepnt->AI.iAIFlags2 &= ~AILANDING;
			planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(5.0f);
			if(((planepnt->AI.winglead < 0) && (planepnt->AI.prevpair < 0)) || (planepnt == PlayerPlane))
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					AIC_GenericMsgPlane(planepnt - Planes, 37);
				}

				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage2(planepnt, GM2_GENERIC_RADIO, 37);
				}
			}

			if(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)
			{
//				EndCasualAutoPilotPlane(planepnt);
				planepnt->AI.lTimer2 = 3000;
			}
			else
			{
				planepnt->AI.lTimer2 = 60000;
			}

			slotnum = (planepnt->AI.lVar2 - 1) % 4;
			slotnum += 2;
			carrier->bFXDesiredArray[slotnum] = 0;
	   		planepnt->AI.lVar2 = 100;
		}
		else
		{
			planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, (pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 80), 0);
			if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
			{
				if(!((planepnt->Status & PL_DEVICE_DRIVEN) || (planepnt == PlayerPlane) || (planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
				{
					planepnt->MaxSpeedAccel = (pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration * 4.5f);
				}
				else
				{
					planepnt->MaxSpeedAccel = (pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration * 5.0f);
				}
//				planepnt->MaxSpeedAccel = planepnt->DesiredSpeed / 8;
			}
			else
			{
				planepnt->MaxSpeedAccel = planepnt->DesiredSpeed / 2;
			}
		}
	}
	else if(planepnt->AI.lVar3 == 1)
	{
		planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, (300), 0);
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = AIGetPitchANGLEForFPMDescent(planepnt, -1500.0f);

		desiredAoA = 5.0f;

		if((planepnt->Pitch < 0) || ((planepnt->HeightAboveGround * WUTOFT) < 80.0f))
		{
			if(planepnt->AI.AOADegPitch < AIConvert180DegreeToAngle(desiredAoA))
			{
				carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
				if(pDBShipList[carrier->iVDBIndex].lShipID != 6)
				{
					planepnt->AI.iAIFlags2 |= (AI_FAKE_AOA);
					planepnt->AI.iAIFlags2 |= AILANDING;
				}
			}
			if((planepnt->HeightAboveGround * WUTOFT) < 80.0f)
			{
				AdesiredAoA = AIConvert180DegreeToAngle(desiredAoA);
				planepnt->AI.DesiredPitch = AdesiredAoA;
			}
		}
		else
		{
			desiredAoA = 12.0f - AIConvertAngleTo180Degree(planepnt->DesiredPitch);

			AdesiredAoA = AIConvert180DegreeToAngle(desiredAoA);

			float maxdAoA = (7.0f * (float)DeltaTicks) / 1500.0f;
			ANGLE AmaxdAoA = AIConvert180DegreeToAngle(maxdAoA);

			ANGLE AdAoA = AdesiredAoA - planepnt->AI.DesiredPitch;

			if((AdAoA < 0x8000) && (AmaxdAoA < AdAoA))
			{
				AdesiredAoA = planepnt->AI.DesiredPitch + AmaxdAoA;
			}

			if(planepnt->AI.AOADegPitch < AdesiredAoA)
			{
				carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
				if(pDBShipList[carrier->iVDBIndex].lShipID != 6)
				{
					planepnt->AI.iAIFlags2 |= (AI_FAKE_AOA);
					planepnt->AI.iAIFlags2 |= AILANDING;
				}
			}

//			planepnt->AI.DesiredPitch = AIConvert180DegreeToAngle(desiredAoA);
			planepnt->AI.DesiredPitch = AdesiredAoA;

		}

		if((pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 4) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 62))
		{
			if(planepnt->AI.lTimer2 > 50000)
			{
				planepnt->AI.cFXarray[1] = 0;
			}
		}

		if((planepnt->AI.lTimer2 < 0) || (planepnt->WorldPosition.Y > (1500.0f * FTTOWU)))
		{
			planepnt->AI.iAIFlags2 &= ~AILANDING;
			planepnt->AI.iAIFlags2 &= ~(AI_FAKE_AOA);
			if(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)
			{
				double raddeg = planepnt->Alpha;  //  This works better than fixed value.  I had forgotten Alpha was getting set to 0 in EndCasual
				EndCasualAutoPilotPlane(planepnt);
				if(planepnt == PlayerPlane)
				{
					PlayerPlane->AI.iAICombatFlags1 |= AI_CARRIER_LANDING;
				}

//				double raddeg = DegToRad(5.0f);  //  12.0f;
				planepnt->Alpha = raddeg;  //  (double)atan2(planepnt->BfLinVel.Z, planepnt->BfLinVel.X);
				planepnt->LastAlpha = planepnt->Alpha;
				planepnt->BfLinVel.Z = planepnt->BfLinVel.X * sin(raddeg);
				planepnt->BfLinVel.X *= cos(raddeg);
				return;
			}
			planepnt->AI.lVar3 = 2;

			if(planepnt->AI.numwaypts == 1)
			{
				int actcnt, startact, endact;

				startact = AIWayPoints[planepnt->AI.startwpts].iStartAct;
				endact = startact + AIWayPoints[planepnt->AI.startwpts].iNumActs;
				for(actcnt = startact; actcnt < endact; actcnt ++)
				{
					if(AIActions[actcnt].ActionID == ACTION_LAND)
					{
						AINextWayPointNoActions(planepnt);
						if(planepnt->AI.startwpts == (planepnt->AI.CurrWay - AIWayPoints))
						{
							planepnt->AI.DesiredPitch = 0;
							planepnt->AI.Behaviorfunc = AIQuickCarrierLanding;
							planepnt->AI.OrgBehave = NULL;
							AISetOffCarrierRelPoint(planepnt, (-2.0f * NMTOWU), planepnt->WorldPosition.Y, 0, 0, 0);
							planepnt->AI.lVar2 = 0;
						}
					}
				}
			}
		}
		else
		{
			if(175 < (planepnt->HeightAboveGround*WUTOFT))
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

			if(350 < (planepnt->HeightAboveGround*WUTOFT))
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
	}
	else if((planepnt->AI.lVar3 == 2) || (planepnt->AI.lVar3 == 100))
	{
		planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, (300), 0);
		planepnt->DesiredRoll = 0;
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(((5000 * FTTOWU) - planepnt->WorldPosition.Y), planepnt);

		carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
		tdist = planepnt->WorldPosition - carrier->WorldPosition;
		if((tdist > (2.5 * NMTOWU)) && (planepnt->AI.lVar3 == 2))
		{
			if((planepnt->AI.winglead < 0) && (planepnt->AI.prevpair < 0))
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					AIC_GenericMsgPlane(planepnt - Planes, 38);
				}

				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage2(planepnt, GM2_GENERIC_RADIO, 38);
				}
			}
			planepnt->AI.lVar3 = 100;
		}
		else if(tdist > (7.0f * NMTOWU))
		{
			if((planepnt->AI.winglead < 0) && (planepnt->AI.prevpair < 0))
			{
				if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					AIC_GenericMsgPlane(planepnt - Planes, 39);
				}

				if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					NetPutGenericMessage2(planepnt, GM2_GENERIC_RADIO, 39);
				}
			}

			planepnt->AI.lVar3 = 3;
			degspersec = AIGetTurnRateForOrbit(2.0f * NMTOFT, 300, 5000.0f);
			if(degspersec > planepnt->YawRate)
			{
				degspersec = planepnt->YawRate;
			}
			if(planepnt->YawRate)
			{
				if((planepnt->AI.OrgBehave == AIAirInvestigate) && (planepnt->AI.AirTarget))
				{
					dx = planepnt->AI.AirTarget->WorldPosition.X - carrier->WorldPosition.X;
					dz = planepnt->AI.AirTarget->WorldPosition.Z - carrier->WorldPosition.Z;
				}
				else
				{
					dx = planepnt->AI.WayPosition.X - carrier->WorldPosition.X;
					dz = planepnt->AI.WayPosition.Z - carrier->WorldPosition.Z;
					while((planepnt->AI.numwaypts > 1) && (QuickDistance(dx,dz) < (10.0 * NMTOWU)))
					{
						orgwaynum = planepnt->AI.numwaypts;
						planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
						AINextWayPoint(planepnt);
						planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
						planepnt->AI.Behaviorfunc = CTDoCarrierLaunch;
						planepnt->AI.lVar3 = 3;
						dx = planepnt->AI.WayPosition.X - carrier->WorldPosition.X;
						dz = planepnt->AI.WayPosition.Z - carrier->WorldPosition.Z;
						if(orgwaynum == planepnt->AI.numwaypts)
							break;
					}
				}

				carrang = atan2(-dx, -dz) * 57.2958;

				carrang -= AIConvertAngleTo180Degree(carrier->Heading);

				carrang = AICapAngle(carrang);

				fworkvar = asin(degspersec / planepnt->YawRate);
				fworkvar = -RadToDeg(fworkvar);
				if(carrang > 0)
				{
					fworkvar = -fworkvar;
				}
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
	}
	else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(((1000 * FTTOWU) - planepnt->WorldPosition.Y), planepnt);
		planepnt->DesiredSpeed = AIConvertSpeedToFtPS(planepnt->Altitude, (300), 0);
		carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
		pshiptype = &pDBShipList[carrier->iVDBIndex];

//		if(planepnt->WorldPosition.Y < (150.0f * FTTOWU))
		if(planepnt->WorldPosition.Y < ((pshiptype->fHeloY + 50.0f) * FTTOWU))
		{
			planepnt->DesiredRoll = 0;
		}
		else
		{
			planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -AIConvertAngleTo180Degree((carrier->Heading - planepnt->Heading) + 0x2000), 0);
		}

		if(planepnt->AI.lTimer2 < 0)
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
		}
	}
	else
	{
		carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(((5000 * FTTOWU) - planepnt->WorldPosition.Y), planepnt);
		if(planepnt->AI.lVar3 == 3)
		{
			if((planepnt->AI.OrgBehave == AIAirInvestigate) && (planepnt->AI.AirTarget))
			{
				dx = planepnt->AI.AirTarget->WorldPosition.X - planepnt->WorldPosition.X;
				dz = planepnt->AI.AirTarget->WorldPosition.Z - planepnt->WorldPosition.Z;

				planeang = atan2(-dx, -dz) * 57.2958;
				planeang -= AIConvertAngleTo180Degree(planepnt->Heading);
				planeang = AICapAngle(planeang);

				if(fabs(planeang) < 5.0f)
				{
					isdone = 1;
				}

				dx = planepnt->AI.AirTarget->WorldPosition.X - carrier->WorldPosition.X;
				dz = planepnt->AI.AirTarget->WorldPosition.Z - carrier->WorldPosition.Z;
			}
			else
			{
				dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
				dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

				planeang = atan2(-dx, -dz) * 57.2958;
				planeang -= AIConvertAngleTo180Degree(planepnt->Heading);
				planeang = AICapAngle(planeang);

				if(fabs(planeang) < 5.0f)
				{
					isdone = 1;
				}

				dx = planepnt->AI.WayPosition.X - carrier->WorldPosition.X;
				dz = planepnt->AI.WayPosition.Z - carrier->WorldPosition.Z;
			}

			carrang = atan2(-dx, -dz) * 57.2958;

			dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
			dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;

			planeang = atan2(-dx, -dz) * 57.2958;

			if(fabs(AICapAngle(carrang - planeang)) < 5.0f)
			{
				isdone = 1;
			}
			else
			{

				fworkvar = AIConvertAngleTo180Degree(planepnt->Heading - carrier->Heading);
				if(fabs(fworkvar) > 85.0f)
				{
					planepnt->AI.lVar3 = 4;

					carrang -= AIConvertAngleTo180Degree(carrier->Heading);

					carrang = AICapAngle(carrang);

					degspersec = AIGetTurnRateForOrbit(10.0f * NMTOFT, 300, 5000.0f);
					if(degspersec > planepnt->YawRate)
					{
						degspersec = planepnt->YawRate;
					}
					if(planepnt->YawRate)
					{
						fworkvar = asin(degspersec / planepnt->YawRate);
						fworkvar = -RadToDeg(fworkvar);
						if(carrang > 0)
						{
							fworkvar = -fworkvar;
						}
						planepnt->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
					}
					else
					{
						planepnt->AI.DesiredRoll = 0;
					}
				}
			}
		}
		else if(planepnt->AI.lVar3 < 100)
		{
			if((planepnt->AI.OrgBehave == AIAirInvestigate) && (planepnt->AI.AirTarget))
			{
				dx = planepnt->AI.AirTarget->WorldPosition.X - planepnt->WorldPosition.X;
				dz = planepnt->AI.AirTarget->WorldPosition.Z - planepnt->WorldPosition.Z;

				planeang = atan2(-dx, -dz) * 57.2958;
				planeang -= AIConvertAngleTo180Degree(planepnt->Heading);
				planeang = AICapAngle(planeang);

				if(fabs(planeang) < 5.0f)
				{
					isdone = 1;
				}

//				dx = planepnt->AI.AirTarget->WorldPosition.X - planepnt->WorldPosition.X;
//				dz = planepnt->AI.AirTarget->WorldPosition.Z - planepnt->WorldPosition.Z;
				dx = planepnt->AI.AirTarget->WorldPosition.X - carrier->WorldPosition.X;
				dz = planepnt->AI.AirTarget->WorldPosition.Z - carrier->WorldPosition.Z;
			}
			else
			{
				dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
				dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;

				planeang = atan2(-dx, -dz) * 57.2958;
				planeang -= AIConvertAngleTo180Degree(planepnt->Heading);
				planeang = AICapAngle(planeang);

				if(fabs(planeang) < 5.0f)
				{
					isdone = 1;
				}

//				dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
//				dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
				dx = planepnt->AI.WayPosition.X - carrier->WorldPosition.X;
				dz = planepnt->AI.WayPosition.Z - carrier->WorldPosition.Z;
			}

			carrang = atan2(-dx, -dz) * 57.2958;

			dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
			dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;

			planeang = atan2(-dx, -dz) * 57.2958;

			if(fabs(AICapAngle(carrang - planeang)) < 5.0f)
			{
				isdone = 1;
			}
		}

		tdist = planepnt->WorldPosition - carrier->WorldPosition;
		if((tdist > (7.0f * NMTOWU)) && (planepnt->AI.lVar3 <= 100))
		{
			planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;
		}
		else
		{
			planepnt->DesiredRoll = 0;
		}

		leadplane = AIGetLeader(planepnt);

		if((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) || (leadplane == PlayerPlane))
		{
			tdist = (leadplane->WorldPosition - planepnt->WorldPosition) * WUTONM;
			if(tdist > 6.0f)
				isdone = 1;
		}

		if((isdone) || (planepnt->AI.lVar3 > 100))
		{
			if((planepnt->AI.winglead < 0) && (planepnt->AI.prevpair < 0))
			{
				if(planepnt->AI.lVar3 < 101)
				{
					planepnt->AI.lVar3 = 101;
					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						AIC_GenericMsgPlane(planepnt - Planes, 40);
					}

					if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						NetPutGenericMessage2(planepnt, GM2_GENERIC_RADIO, 40);
					}
					planepnt->AI.lTimer2 = 4000;
				}
				else if(planepnt->AI.lVar3 == 101)
				{
					planepnt->AI.lVar3 = 102;
					if(planepnt->AI.lTimer2 < 0)
					{
						if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							AIC_GenericMsgPlane(planepnt - Planes, 41);
						}

						if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							NetPutGenericMessage2(planepnt, GM2_GENERIC_RADIO, 41);
						}
						planepnt->AI.lTimer2 = 6000;
					}
				}
				else if(planepnt->AI.lTimer2 < 0)
				{
					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						AIC_GenericMsgPlane(planepnt - Planes, 36);
					}

					if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						NetPutGenericMessage2(planepnt, GM2_GENERIC_RADIO, 36);
					}

					int awacsnum = AIGetClosestAWACS(planepnt);

					AICAddSoundCall(AICStrikeSwitchControl, planepnt - Planes, 8000, 50, awacsnum);
					isdone = 2;
				}
			}
			else
			{
				isdone = 2;
			}

			if(isdone == 2)
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
			}
		}
	}

}

//**************************************************************************************
void CTCheckTakeOffQueue(PlaneParams *orgplanepnt)
{
	PlaneParams *planepnt = orgplanepnt;
	PlaneParams *checkplane;
	long modval = (orgplanepnt->AI.lVar2 & 0xFF) % 4;
	PlaneParams *foundplane = NULL;
	int cnt;
	long lworkvar;
	long timemod = 0;

	if(orgplanepnt->AI.iAICombatFlags2 & AI_READY_FOR_LAUNCH)
	{
		timemod = orgplanepnt->AI.lTimer3;
	}

	if(orgplanepnt->AI.lVar2 == -2)
	{
		return;
	}

	for(cnt = 0; cnt < 2; cnt ++)
	{
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if (!((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)))  continue;

			if(checkplane->AI.iAIFlags2 & AIINVISIBLE)
			{
				if((checkplane->AI.iAICombatFlags1 & AI_READY_5) || (checkplane->AI.iAICombatFlags2 & AI_CHECK_TAKEOFF))
				{
					continue;
				}
			}

			if (checkplane->FlightStatus & PL_STATUS_CRASHED)  continue;

			if((checkplane == planepnt) || (checkplane == orgplanepnt))  continue;

			if((checkplane->OnGround == 2) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId))
			{
				if(checkplane->AI.lVar2 == (planepnt->AI.lVar2 + 4))
				{
					foundplane = checkplane;
					break;
				}

				if((checkplane->AI.lVar2 % 4) == modval)
				{
					if(foundplane)
					{
						if(checkplane->AI.lVar2 < foundplane->AI.lVar2)
						{
							foundplane = checkplane;
						}
					}
					else
					{
						foundplane = checkplane;
					}
				}
			}
		}
		if(!foundplane)
		{
			if(cnt == 0)
			{
				return;
			}
			else
			{
				planepnt->AI.fVarA[0] = orgplanepnt->AI.fVarA[0];
				planepnt->AI.fVarA[1] = orgplanepnt->AI.fVarA[1];
				planepnt->AI.fVarA[2] = orgplanepnt->AI.fVarA[2];
				planepnt->AI.lTimer3 = 20000 + timemod;
				planepnt->AI.lTimer2 = 80000 + timemod;
				planepnt->AI.lVar3 = 98;
				if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
				{
					planepnt->AI.fVarA[0] = -64.0f * FTTOWU;
					planepnt->AI.fVarA[1] = -80.0f * FTTOWU;
					planepnt->AI.fVarA[2] = 0.0f;
				}
				return;
			}
		}
		else if(cnt == 0)
		{
			planepnt = foundplane;
			foundplane = NULL;
		}
	}

	foundplane->AI.fVarA[0] = planepnt->AI.fVarA[0];
	foundplane->AI.fVarA[1] = planepnt->AI.fVarA[1];
	foundplane->AI.fVarA[2] = planepnt->AI.fVarA[2];
	foundplane->AI.lTimer3 = 32500 + timemod;
	foundplane->AI.lVar3 = 2;
	if(foundplane->AI.iAIFlags2 & AIINVISIBLE)
	{
		lworkvar = foundplane - Planes;
		lworkvar <<= 8;
		planepnt->AI.lVar2 |= lworkvar;
	}

	planepnt->AI.fVarA[0] = orgplanepnt->AI.fVarA[0];
	planepnt->AI.fVarA[1] = orgplanepnt->AI.fVarA[1];
	planepnt->AI.fVarA[2] = orgplanepnt->AI.fVarA[2];
	planepnt->AI.lTimer3 = 20000 + timemod;
	planepnt->AI.lTimer2 = 80000 + timemod;
	planepnt->AI.lVar3 = 98;
	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		planepnt->AI.fVarA[0] = -64.0f * FTTOWU;
		planepnt->AI.fVarA[1] = -80.0f * FTTOWU;
		planepnt->AI.fVarA[2] = 0.0f;
	}
}

//**************************************************************************************
void CTReplacePlaneInSlot(PlaneParams *orgplanepnt, int slotnum)
{
	int done = 0;
	PlaneParams *planepnt = orgplanepnt;
	PlaneParams *checkplane;
	int replaceslot = slotnum;
	PlaneParams *leadplane;

	while(!done)
	{
		done = 1;
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if (!((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)))  continue;

			if (checkplane->FlightStatus & PL_STATUS_CRASHED)  continue;

			if((checkplane == planepnt) || (checkplane == orgplanepnt))  continue;

			if((checkplane->OnGround == 2) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId))
			{
				if(checkplane->AI.lVar2 == replaceslot)
				{
					done = 0;

					if(!((checkplane == PlayerPlane) || (checkplane->Status & AL_DEVICE_DRIVEN) || (checkplane->Status & AL_COMM_DRIVEN) || (checkplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
					{
						leadplane = AIGetLeader(checkplane);
						if(!((leadplane == PlayerPlane) || (leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
						{
							CTGetCarrierTakeOffSpace(planepnt, replaceslot);
							planepnt = checkplane;
						}
					}
					if((checkplane->AI.iAICombatFlags1 & AI_READY_5) || (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
					{
						replaceslot += 4;
					}
					else
					{
						replaceslot ++;
					}
				}
			}
		}
	}
	CTGetCarrierTakeOffSpace(planepnt, replaceslot);
}


//**************************************************************************************
int CTFindSlotForPlane(PlaneParams *planepnt, int searchtype)
{
	int done = 0;
	int replaceslot;
	int okcheck;
	int is5 = 0;
	PlaneParams *leadplane;
	PlaneParams *checkplane;
	int helocnt = 0;
	int ishelo = 0;

	if(planepnt->AI.iAICombatFlags1 & AI_READY_5)  //  Is ready 5 plane (lowest priority)
	{
		is5 = 1;
	}

	if(planepnt->AI.lTimer2 > 0)
	{
		is5 = 1;
		planepnt->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
	}
	else if(AIWaitingForTakeOffFlag(planepnt))
	{
		is5 = 1;
		planepnt->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
	}

	if(searchtype == 1)		//  Human
	{
		if(MultiPlayer)
		{
			replaceslot = 1;
		}
		else
		{
			replaceslot = 3;
		}
	}
	else if(searchtype == 2)	//  AI in Human Group
	{
		replaceslot = 1;
	}
	else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		replaceslot = 4;
		ishelo = 1;
	}
	else
	{
		replaceslot = 1;
	}

	if(is5)			//  If this plane won't launch right away, set him off the deck.
	{
		replaceslot = 21;
	}

	while(!done)
	{
		done = 1;
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if (!((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)))  continue;

			if (checkplane->FlightStatus & PL_STATUS_CRASHED)  continue;

			if(checkplane == planepnt)  continue;

			if((checkplane->OnGround == 2) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId))
			{
				okcheck = 1;
				if(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
				{
					helocnt++;
				}

				if(checkplane->AI.lVar2 == replaceslot)
				{
					if(searchtype == 1)
					{
						if((checkplane == PlayerPlane) || (checkplane->Status & AL_DEVICE_DRIVEN) || (checkplane->Status & AL_COMM_DRIVEN) || (checkplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
						{
							okcheck = 0;
						}
					}
					else if(searchtype == 2)
					{
						if((checkplane == PlayerPlane) || (checkplane->Status & AL_DEVICE_DRIVEN) || (checkplane->Status & AL_COMM_DRIVEN) || (checkplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
						{
							okcheck = 0;
						}
						else
						{
							leadplane = AIGetLeader(checkplane);
							if((leadplane == PlayerPlane) || (leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
							{
								okcheck = 0;
							}
						}
					}
					else if(is5)
					{
//						if((!(checkplane->AI.iAICombatFlags1 & AI_READY_5)) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)))
						if(!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
						{
							okcheck = 0;
						}
					}
					else if(ishelo)
					{
						okcheck = 0;
					}
					else
					{
						if((!(checkplane->AI.iAICombatFlags1 & AI_READY_5)) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))) && (!(checkplane->AI.iAICombatFlags2 & AI_CHECK_TAKEOFF)))
						{
							okcheck = 0;
						}
					}

					if(okcheck)
					{
						CTReplacePlaneInSlot(planepnt, replaceslot);
						return(-1);
					}
					else
					{
						done = 0;

						if((is5) || (ishelo))
						{
							replaceslot += 4;
						}
						else
						{
							replaceslot ++;
						}
					}
				}
			}
		}
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		if(helocnt == 0)
		{
			replaceslot = -2;
		}
	}

	return(replaceslot);
}

//**************************************************************************************
int CTFindSlotForSovPlane(PlaneParams *planepnt, int searchtype)
{
	int done = 0;
	int replaceslot;
	int okcheck;
	int is5 = 0;
	PlaneParams *leadplane;
	PlaneParams *checkplane;
	int helocnt = 0;
	int ishelo = 0;
	int modval;

	if(planepnt->AI.iAICombatFlags1 & AI_READY_5)  //  Is ready 5 plane (lowest priority)
	{
		is5 = 1;
	}

	if(planepnt->AI.lTimer2 > 0)
	{
		is5 = 1;
		planepnt->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
	}
	else if(AIWaitingForTakeOffFlag(planepnt))
	{
		is5 = 1;
		planepnt->AI.iAICombatFlags2 |= AI_CHECK_TAKEOFF;
	}

	if(searchtype == 1)		//  Human
	{
		replaceslot = 1;
	}
	else if(searchtype == 2)	//  AI in Human Group
	{
		replaceslot = 1;
	}
	else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		replaceslot = 4;
		ishelo = 1;
	}
	else
	{
		replaceslot = 1;
	}

	if(is5)			//  If this plane won't launch right away, set him off the deck.
	{
		replaceslot = 21;
	}

	while(!done)
	{
		done = 1;
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if (!((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)))  continue;

			if (checkplane->FlightStatus & PL_STATUS_CRASHED)  continue;

			if(checkplane == planepnt)  continue;

			if((checkplane->OnGround == 2) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId))
			{
				okcheck = 1;
				if(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
				{
					helocnt++;
				}

				if(checkplane->AI.lVar2 == replaceslot)
				{
					if(searchtype == 1)
					{
						if((checkplane->Status & AL_DEVICE_DRIVEN) || (checkplane->Status & AL_COMM_DRIVEN) || (checkplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
						{
							okcheck = 0;
						}
					}
					else if(searchtype == 2)
					{
						if((checkplane->Status & AL_DEVICE_DRIVEN) || (checkplane->Status & AL_COMM_DRIVEN) || (checkplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
						{
							okcheck = 0;
						}
						else
						{
							leadplane = AIGetLeader(checkplane);
							if((leadplane->Status & AL_DEVICE_DRIVEN) || (leadplane->Status & AL_COMM_DRIVEN) || (leadplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
							{
								okcheck = 0;
							}
						}
					}
					else if(is5)
					{
//						if((!(checkplane->AI.iAICombatFlags1 & AI_READY_5)) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)))
						if(!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
						{
							okcheck = 0;
						}
					}
					else if(ishelo)
					{
						okcheck = 0;
					}
					else
					{
						if((!(checkplane->AI.iAICombatFlags1 & AI_READY_5)) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))) && (!(checkplane->AI.iAICombatFlags2 & AI_CHECK_TAKEOFF)))
						{
							okcheck = 0;
						}
					}

					if(okcheck)
					{
						CTReplacePlaneInSlot(planepnt, replaceslot);
						return(-1);
					}
					else
					{
						done = 0;

						if((is5) || (ishelo))
						{
							replaceslot += 4;
						}
						else
						{
							modval = replaceslot % 4;
							if(modval == 1)  //  only want to use cats 1 and 2;
							{
								replaceslot ++;
							}
							else if(modval == 2)
							{
								replaceslot += 3;
							}
							else if(modval == 3)
							{
								replaceslot += 2;
							}
							else if(modval == 0)
							{
								replaceslot ++;
							}
						}
					}
				}
			}
		}
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))
	{
		if(helocnt == 0)
		{
			replaceslot = -2;
		}
	}

	return(replaceslot);
}

//**************************************************************************************
void AISetUpQuickCarrierLanding(PlaneParams *planepnt)
{
	MovingVehicleParams *carrier;
	PlaneParams *checkplane;
	int placeinqueue = 0;
	float ftoffset, fseparation;
	double deckangle = DECK_ANGLE;
	double FBHeading;
	float desiredpitch;

	fseparation = 30.0f;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if (!((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)))  continue;

		if (checkplane->FlightStatus & PL_STATUS_CRASHED)  continue;

		if(checkplane == planepnt)  continue;

		if((checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId))
		{
			if(checkplane->OnGround == 2)
			{
				if((checkplane->AI.Behaviorfunc == CTWaitingForLaunch) || (checkplane->AI.Behaviorfunc == CTDoCarrierLaunch))
				{
					if(((checkplane->AI.lVar2 % 4) == 3) || ((checkplane->AI.lVar2 % 4) == 0))
						return;
				}
			}
			else if((checkplane->AI.Behaviorfunc == AIFlyCarrierFinal) || (checkplane->AI.Behaviorfunc == AIFlyToDME3))
			{
				placeinqueue ++;
			}
		}
	}

#if 0
	if(placeinqueue)
	{
//		planepnt->WorldPosition.Y = AICheckSafeAlt(planepnt, carrier->WorldPosition.Y + (600.0f * FTTOWU));
		planepnt->WorldPosition.Y = carrier->WorldPosition.Y + (600.0f * FTTOWU);
	}
	else
	{
//		planepnt->WorldPosition.Y = AICheckSafeAlt(planepnt, carrier->WorldPosition.Y + (350.0f * FTTOWU));
		planepnt->WorldPosition.Y = carrier->WorldPosition.Y + (440.0f * FTTOWU);  //  (350.0f * FTTOWU);
	}
#endif

	planepnt->WorldPosition.Y = carrier->WorldPosition.Y + (600.0f * FTTOWU);

	MBAdjustAIThrust(planepnt, pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed + 20, 1);
	planepnt->V = planepnt->BfLinVel.X = planepnt->DesiredSpeed;
	ftoffset = (planepnt->V * (fseparation * (float)placeinqueue)) + (2.0f * NMTOFT);  //  (0.8f * NMTOFT);

	if((ftoffset * FTTONM) < 3.5f)
	{
		planepnt->AI.Behaviorfunc = AIFlyCarrierFinal;
		planepnt->Status &= ~PL_ONGROUND_NEXT;
	}
	else
	{
		planepnt->AI.Behaviorfunc = AIFlyToDME3;
	}

	FBHeading = AIConvertAngleTo180Degree(carrier->Heading) + deckangle;
	planepnt->Flaps = 30.0;
	planepnt->FlapsCommandedPos = 30.0;
	SetLandingGearUpDown(planepnt,LOWER_LANDING_GEAR);
	if(!planepnt->TailHookState)
	{
		planepnt->TailHookState = 1;
		planepnt->TailHookCommandedPos = 90.0;
	}

	desiredpitch = (AIDESIREDAOA);
	planepnt->AI.DesiredPitch = planepnt->Pitch = AIConvert180DegreeToAngle(desiredpitch);
	planepnt->Heading = AIConvert180DegreeToAngle(FBHeading);

	extern void RPH_to_Orientation( PlaneParams *P );
	RPH_to_Orientation(planepnt);

	planepnt->WorldPosition.X = carrier->WorldPosition.X;
	planepnt->WorldPosition.Z = carrier->WorldPosition.Z;
	AIOffsetFromPoint(&planepnt->WorldPosition, FBHeading, 0, 0, ftoffset * FTTOWU);
	planepnt->AI.cnt1 = (placeinqueue + 1) * -1;
	if(planepnt == PlayerPlane)
	{
		PlayerPlane->AI.iAICombatFlags1 |= AI_CARRIER_LANDING;
		if((ftoffset * FTTONM) < 3.0f)
		{
			planepnt->AI.lVar2 = 9;
		}
		else if((ftoffset * FTTONM) < 5.0f)
		{
			planepnt->AI.lVar2 = 8;
		}
		else
		{
			planepnt->AI.lVar2 = 7;
		}
	}
}

//**************************************************************************************
void CTCheckForRelaunch(PlaneParams *planepnt)
{
	int workval;
	float offx, offz, tempx, tempz;
	float offy = 64.0f;
	float parkangle;
	MovingVehicleParams *carrier;
	FPointDouble carrpos;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(planepnt == PlayerPlane)
	{
		lAdvisorFlags |= ADV_CARRIER_LAUNCH;
	}

	if((planepnt == PlayerPlane) && (MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED) && (g_Settings.gp.dwCheats & GP_CHEATS_MULTI_REARM))
	{
//		planepnt->Brakes = 1;
//		if (planepnt->Brakes)
//			DisplayWarning(BRAKE, ON, 1);

		F18LoadPlayerWeaponInfo();
		NetPutGenericMessage1(planepnt, GM_RELOAD_PLANE);
	}
	else if((PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch) && (PlayerPlane->AI.lVar3 == 100) && (!(PlayerPlane->AI.iAICombatFlags2 & AI_READY_FOR_LAUNCH)) && (g_Settings.gp.dwCheats & GP_CHEATS_MULTI_REARM))
	{
		PlayerPlane->Brakes = 0;
		DisplayWarning(BRAKE, OFF, 1);
		VKCODE vk;
		vk.dwExtraCode = 0;
		vk.vkCode = 0;
		vk.wFlags = 0;
		aic_request_takeoff(vk);
	}

	AINextWayPointNoActions(planepnt);

	planepnt->AI.Behaviorfunc = CTWaitingForLaunch;
	planepnt->AI.iAICombatFlags1 |= AI_CARRIER_LANDING;

//	workval = planepnt->AI.cnt1;
	workval = (planepnt - Planes) & 1;

	if(carrier->lVar1)
	{
		workval = 1;
	}

	if(!(workval & 1))
	{
		offx = -13.5;
		offz = -188;
		parkangle = 1.45f;
		tempx = offx;
		tempz = -230;
		planepnt->AI.lVar3 = 96;
		planepnt->AI.lVar2 = 2;
	}
	else
	{
		offx = 52.0f;
		offz = -201.5;
		parkangle = 4.712f;
		tempx = offx;
		tempz = -230;
		planepnt->AI.lVar3 = 96;
		planepnt->AI.lVar2 = 1;
	}

	if(planepnt->AI.cnt1 < 0)
	{
		planepnt->AI.lTimer2 = 20000;
		planepnt->AI.cnt1 *= -1;
	}
	else
	{
		planepnt->AI.lTimer2 = 30000;
	}

	//  I think I need this for multiplayer.
	planepnt->AI.lTimer3 = -1;

	planepnt->AI.fVarA[0] = offx * FTTOWU;
	planepnt->AI.fVarA[1] = offz * FTTOWU;
	planepnt->AI.fVarA[2] = parkangle;
}

//**************************************************************************************
void AIQuickCarrierLanding(PlaneParams *planepnt)
{
	float tdist;

	tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);

	if(AIInNextRange(planepnt, tdist * WUTOFT,  5743, 45.0f))
	{
		if(planepnt->AI.lVar2)
		{
			if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(GEAR, ON, 1);
				}
				RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
			}
			if(!planepnt->Flaps)
			{
				if(planepnt == (PlaneParams *)PlayerPlane)
				{
					DisplayWarning(FLAPS, ON, 1);
				}
				planepnt->Flaps = 30.0f;
				planepnt->FlapsCommandedPos = 30.0;
			}

			if(!planepnt->TailHookState)
			{
				planepnt->TailHookState = 1;
		//		if(planepnt == (PlaneParams *)PlayerPlane)
		//		{
		//			DisplayWarning(FLAPS, ON, 1);
		//		}
		//		planepnt->TailHook = 90.0f;
				planepnt->TailHookCommandedPos = 90.0;
			}

			planepnt->AI.Behaviorfunc = AIFlyToDME3;
		}
		else
		{
			AISetOffCarrierRelPoint(planepnt, (-1.0f * NMTOWU), planepnt->WorldPosition.Y, (4.0f * NMTOWU), 1, 0);
		}
	}
}

//**************************************************************************************
void UpdateCarrierViews()
{
	FPointDouble camerapos;
	FPointDouble offset;

	if(!(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER))
	{
		return;
	}

	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
	camerapos = carrier->WorldPosition;

	if(Camera1.CameraMode != CAMERA_RC_VIEW)
	{
		iCarrierWatch = -1;
		return;
	}

	if(iCarrierWatch & 0x1)  //  LSO view
	{
		offset.X = -64.0f * FTTOWU;
		offset.Y = 72.0f * FTTOWU;
		offset.Z = 416.0f * FTTOWU;
	}
	else					//  Tower view
	{
		offset.X = 66.0f * FTTOWU;
		offset.Y = 91.0f * FTTOWU;
		offset.Z = 135.0f * FTTOWU;
	}

	offset *= carrier->Attitude;

	camerapos.X = carrier->WorldPosition.X + offset.X;
	camerapos.Y = carrier->WorldPosition.Y + offset.Y;
	camerapos.Z = carrier->WorldPosition.Z + offset.Z;

	Camera1.CameraLocation.X = camerapos.X;
	Camera1.CameraLocation.Y = camerapos.Y;
	Camera1.CameraLocation.Z = camerapos.Z;

	Camera1.CameraTargetLocation = Camera1.CameraLocation;
}

//**************************************************************************************
void AIFlyCarrierHeloMarshallPattern(PlaneParams *planepnt)
{
	float tdist;
	double offangle;
	double dy;
	MovingVehicleParams *carrier;
	double fworkvar, degsec;
	char tstr[80];
	double deckangle = HELO_ANGLE;

	sprintf(tstr, "Somethings wrong");

	MBAdjustAIThrust(planepnt, 100, 1);

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	if(carrier->lAIFlags1 & V_ON_FIRE)
	{
		CTSendToDivertField(planepnt);
		return;
	}

	if(planepnt->AI.lTimer3 < 0)
	{
		// Start Push
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
			AICCommencingPush(planepnt - Planes);
			planepnt->AI.lVar2 = 0;
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
			}
	//		sprintf(tstr, "Plane %d, Push", planepnt - Planes);
	//		AICAddAIRadioMsgs(tstr, 50);
			return;
		}
	}

	if((planepnt->AI.lVar2 == 10) || (planepnt->AI.lVar2 < 3))
	{
		tdist = FlyToPoint(planepnt, planepnt->AI.WayPosition);

		if(AIInNextRange(planepnt, tdist * WUTOFT,  574, 45.0f))
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage2Float(planepnt, GM2F_TARGET_Y, planepnt->AI.TargetPos.Y);
				NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
			}

			switch(planepnt->AI.lVar2)
			{
				case 1:
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 2, deckangle);
//					sprintf(tstr, "Plane %d, now Heading front corner", planepnt - Planes);
					break;
				case 2:
					if(planepnt->AI.lTimer3 < 10000)
					{
						//  Start Push
						if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
							AICCommencingPush(planepnt - Planes);
							planepnt->AI.lVar2 = 0;
							planepnt->AI.lTimer3 = -1;
							if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
							}
	//						sprintf(tstr, "Plane %d, Push", planepnt - Planes);
	//						AICAddAIRadioMsgs(tstr, 50);
							return;
						}
						else
						{
							planepnt->AI.lTimer3 = 50000;
						}
					}

					if(planepnt->AI.lTimer3 < 50000)
					{
						planepnt->AI.TargetPos.X = 360.0f / ((float)planepnt->AI.lTimer3 / 1000.0f);
						planepnt->AI.lTimer2 = planepnt->AI.lTimer3;
						planepnt->AI.lVar2 = 6;
//						sprintf(tstr, "Plane %d, now Turning towards back Sort Of", planepnt - Planes);
					}
					else
					{
						planepnt->AI.TargetPos.X = 180.0f / 20.0f;
						planepnt->AI.lTimer2 = 20000;
						planepnt->AI.lVar2 = 3;
//						sprintf(tstr, "Plane %d, now Turning towards back", planepnt - Planes);
					}
					break;
				case 10:
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
					tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
					AICEstablishedMarshal(planepnt - Planes);
//					sprintf(tstr, "Plane %d, now starting up FB side", planepnt - Planes);
					break;
			}
//			AICAddAIRadioMsgs(tstr, 50);
		}
		else if(planepnt->AI.lVar2 == 10)
		{
			AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (2.0f * NMTOWU), 10, deckangle);
		}
	}
	else
	{
		if(planepnt->AI.lVar2 == 4)
		{
			dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;

			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			offangle = AIConvertAngleTo180Degree(carrier->Heading + 0x8000) + deckangle - AIConvertAngleTo180Degree(planepnt->Heading);

			offangle = AICapAngle(offangle);

			AIChangeDir(planepnt, (double)offangle, ((double)dy * WUTOFT));
		}
		else
		{
			if(fabs(planepnt->AI.TargetPos.X) <= planepnt->YawRate)
			{
				degsec = planepnt->AI.TargetPos.X;
			}
			else
			{
				degsec = (planepnt->AI.TargetPos.X < 0) ? -planepnt->YawRate : planepnt->YawRate;
			}
			fworkvar = asin(degsec / planepnt->YawRate);
			fworkvar = RadToDeg(fworkvar);
			planepnt->AI.DesiredRoll = -AIConvert180DegreeToAngle(fworkvar); //  Negative so racetrack the other way.
			planepnt->DesiredRoll = -planepnt->AI.DesiredRoll;

			dy = AICheckSafeAlt(planepnt, planepnt->AI.WayPosition.Y) - planepnt->WorldPosition.Y;
			planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);
		}

		if(planepnt->AI.lTimer2 < 0)
		{
			if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
			{
				NetPutGenericMessage2Float(planepnt, GM2F_TARGET_Y, planepnt->AI.TargetPos.Y);
				NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
			}

			switch(planepnt->AI.lVar2)
			{
				case 3:
					if(planepnt->AI.lTimer3 < 330000)
					{
						planepnt->AI.lTimer2 = ((planepnt->AI.lTimer3 - 20000) / 2);
						if(planepnt->AI.lTimer2 < 0)
						{
							planepnt->AI.lTimer2 = 1;
						}
//						sprintf(tstr, "Plane %d, now starting back leg SHORT", planepnt - Planes);
					}
					else
					{
						planepnt->AI.lTimer2 = 160000;
//						sprintf(tstr, "Plane %d, now starting back leg", planepnt - Planes);
					}
					planepnt->AI.lVar2 = 4;
					break;
				case 4:
					planepnt->AI.TargetPos.X = 180.0f / 20.0f;
					planepnt->AI.lTimer2 = 20000;
					planepnt->AI.lVar2 = 5;
//					sprintf(tstr, "Plane %d, now turning towards front", planepnt - Planes);
					break;
				case 5:
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
					tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
					AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
//					sprintf(tstr, "Plane %d, now starting FB correction", planepnt - Planes);
					break;
				case 6:
					//  Start Push
					if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
					{
						planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
						AICCommencingPush(planepnt - Planes);
						planepnt->AI.lVar2 = 0;
						planepnt->AI.lTimer3 = -1;
						if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							NetPutGenericMessage3Long(planepnt, GM3L_UPDATE_MARSHAL, planepnt->AI.lVar2, planepnt->AI.lTimer3);
						}
	//					sprintf(tstr, "Plane %d, Push", planepnt - Planes);
	//					AICAddAIRadioMsgs(tstr, 50);
						return;
					}
					break;
			}
//			AICAddAIRadioMsgs(tstr, 50);
		}
	}
}

//**************************************************************************************
void AIFlyToHeloDME3(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float dx, dy, dz;
	double deckangle = DECK_ANGLE;
	DBShipType *pshiptype;

	if(planepnt->AI.lTimer3 >= 0)
	{
		planepnt->DesiredSpeed = 0;
		planepnt->AI.DesiredPitch = 0;
		planepnt->AI.DesiredRoll = 0;
		return;
	}

	MBAdjustAIThrust(planepnt, 100, 1);

	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	pshiptype = &pDBShipList[carrier->iVDBIndex];

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

	AISetOffCarrierRelPoint(planepnt, 0, ((pshiptype->fHeloY + 500.0f) * FTTOWU), 3.0f * NMTOWU, planepnt->AI.lVar2, deckangle);

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	offangle = AICapAngle(offangle);

	planepnt->DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

//	dy = AICheckSafeAlt(planepnt, (600.0f * FTTOWU)) - planepnt->WorldPosition.Y;
	planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);

	planepnt->AI.iAIFlags2 |= AILANDING;

	if(AIInNextRange(planepnt, tdist * WUTOFT,  1000))
	{
		planepnt->AI.Behaviorfunc = AIFlyHeloCarrierFinal;
		planepnt->Status &= ~PL_ONGROUND_NEXT;
//		AICCheckLSOForPlane(planepnt, 1);

		planepnt->AI.iVar1 = 30;
	}
}

//**************************************************************************************
void AIFlyHeloCarrierFinal(PlaneParams *planepnt)
{
	double offangle;
	float tdist, tdistorg;
	float actdistft;
	float dx, dy, dz;
	double deckangle = DECK_ANGLE;
//	char tstr[80];
	FPointDouble new_carrier_position;
	DBShipType *pshiptype;
	MovingVehicleParams *carrier;
	float desiredspeed;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(carrier->lAIFlags1 & V_ON_FIRE)
	{
		CTSendToDivertField(planepnt);
		return;
	}

	pshiptype = &pDBShipList[carrier->iVDBIndex];

	AISetOffCarrierRelPoint(planepnt, ((pshiptype->fHeloX) * FTTOWU), ((pshiptype->fHeloY) * FTTOWU), ((pshiptype->fHeloZ) * FTTOWU), planepnt->AI.lVar2);

	dx = planepnt->AI.WayPosition.X - planepnt->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - planepnt->WorldPosition.Z;
	tdistorg = QuickDistance(dx,dz);

	actdistft = (tdistorg) * WUTOFT;

	offangle = AIComputeHeadingToPoint(planepnt, planepnt->AI.WayPosition, &tdist, &dx ,&dy, &dz, 1);

	if((actdistft < 10.0f) || ((fabs(offangle) > 90.0f) && (actdistft < 1000.0f)) || (planepnt->AI.lVar2))
	{
		planepnt->AI.lVar2 = 1;
		if(fabs(offangle) > 90.0f)
		{
			MBAdjustAIThrust(planepnt, -5, 1);
			planepnt->DesiredSpeed += carrier->fSpeedFPS;
		}
		else
		{
			planepnt->DesiredSpeed = carrier->fSpeedFPS;
		}

		offangle = AIConvertAngleTo180Degree(carrier->Heading - planepnt->Heading);

		if(planepnt->DesiredSpeed)
		{
			planepnt->DesiredPitch = 0xF000;
			if(planepnt->OnGround)
			{
				planepnt->DesiredPitch = 0;
			}
			else if(planepnt->HeightAboveGround < (20.0f * FTTOWU))
			{
				planepnt->DesiredPitch = 0xFDDD;
			}
		}
		else
		{
			tdist = fabs(dy);

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

			planepnt->DesiredPitch = 0xC000;

			if(planepnt->OnGround)
			{
				planepnt->DesiredPitch = 0;
			}
		}
		planepnt->DesiredRoll = planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);
	}
	else
	{
		planepnt->DesiredRoll = planepnt->AI.DesiredRoll = AIGetDesiredRollOffdyaw(planepnt, -offangle, 0);

		dy += (50.0f * FTTOWU);

		planepnt->DesiredPitch = AIGetDesiredPitchOffdy(dy, planepnt);


		if(actdistft < 500.0f)
		{
			MBAdjustAIThrust(planepnt, 10, 1);
			planepnt->DesiredSpeed += carrier->fSpeedFPS;
		}
		else if(actdistft < 1000.0f)
		{
			MBAdjustAIThrust(planepnt, 20, 1);
			planepnt->DesiredSpeed += carrier->fSpeedFPS;
		}
		else
		{
			MBAdjustAIThrust(planepnt, 50, 1);
			planepnt->DesiredSpeed += carrier->fSpeedFPS;
		}
	}

	planepnt->AI.iAIFlags2 |= AILANDING;

//	CalcHeightAboveDeck(planepnt, carrier);


	if((planepnt->OnGround == 2) || (planepnt->Status & PL_ONGROUND_NEXT))
	{
		planepnt->Status &= ~PL_ONGROUND_NEXT;
		planepnt->OnGround = 2;
		planepnt->AI.Behaviorfunc = AIMoveToParkingSpace;
		planepnt->AI.lVar3 = 6;  //  0;
		planepnt->DesiredSpeed = 0;
		planepnt->BfLinVel.X = 0;
		planepnt->V = 0;
		int getoffdeck = 0;

		carrier->lAIFlags1 |= PLANES_ON_DECK;
		if (!planepnt->PlaneCopy)	/*on carrier*/
			planepnt->PlaneCopy = GetNewCarrierPlane(planepnt,carrier);

		PlaneParams *checkplane = &Planes[0];
		while (checkplane <= LastPlane)
		{
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide == checkplane->AI.iSide) && (checkplane != planepnt))
			{
				if((checkplane != planepnt) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (checkplane->AI.iHomeBaseId == planepnt->AI.iHomeBaseId) && (!(checkplane->AI.iAIFlags2 & AIINVISIBLE)))
				{
					if((checkplane->AI.Behaviorfunc == AIFlyCarrierMarshallPattern) || (checkplane->AI.iAIFlags2 & AILANDING))
					{
						getoffdeck = 1;
					}
					else if((checkplane->AI.Behaviorfunc == AIFlyCarrierHeloMarshallPattern) || (checkplane->AI.iAIFlags2 & AILANDING))
					{
						getoffdeck = 1;
					}
				}
			}
			checkplane ++;
		}

		if(getoffdeck == 1)
		{
			if(pshiptype->lShipType & ~(SHIP_TYPE_CARRIER))
			{
				planepnt->AI.lVar3 = 7;
				planepnt->AI.lTimer2 = 30000;
			}
			else
			{
				AIGetCarrierParkingSpace(planepnt);
				planepnt->AI.lTimer2 = 30000;
			}
		}
	}

	if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(GEAR, ON, 1);
		}
		RaiseLowerLandingGear(planepnt,LOWER_LANDING_GEAR);
	}
	if(!planepnt->Flaps)
	{
		if(planepnt == (PlaneParams *)PlayerPlane)
		{
			DisplayWarning(FLAPS, ON, 1);
		}
		planepnt->Flaps = 30.0f;
		planepnt->FlapsCommandedPos = 30.0;
	}

	if(!planepnt->TailHookState)
	{
		planepnt->TailHookState = 1;
//		if(planepnt == (PlaneParams *)PlayerPlane)
//		{
//			DisplayWarning(FLAPS, ON, 1);
//		}
//		planepnt->TailHook = 90.0f;
		planepnt->TailHookCommandedPos = 90.0;
	}
}

//**************************************************************************************
void CTAddToLaunchQueue(PlaneParams *planepnt)
{
	int done = 0;
	int is5 = 0;
	PlaneParams *checkplane;
	int helocnt = 0;
	int ishelo = 0;
	MovingVehicleParams *carrier;
	int lastslot1 = 1;
	int cnt1 = 0;
	int modval;

	planepnt->AI.iAIFlags2 &= ~AIINVISIBLE;
	planepnt->Status |= PL_ACTIVE;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];


	planepnt->AI.iAICombatFlags2 &= ~(AI_CHECK_TAKEOFF);


	while(!done)
	{
		done = 1;
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if(!((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)))  continue;

			if((checkplane->AI.iAICombatFlags1 & AI_READY_5) || (checkplane->AI.iAICombatFlags2 & AI_CHECK_TAKEOFF))
			{
				continue;
			}

			if (checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_OUT_OF_CONTROL))  continue;

			if(checkplane == planepnt)  continue;

			if((checkplane->OnGround == 2) && (checkplane->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId) && (checkplane->AI.lVar2 >= 0) && (checkplane->AI.Behaviorfunc == CTWaitingForLaunch))
			{
				modval = checkplane->AI.lVar2 % 4;
				if(modval == 1)
				{
					cnt1 ++;
					if(checkplane->AI.lVar2 > lastslot1)
						lastslot1 = checkplane->AI.lVar2;
				}
			}
		}
	}
	if(cnt1 > 1)
	{
		planepnt->AI.lVar2 = lastslot1 + 4;
		planepnt->Status &= ~PL_ACTIVE;
		planepnt->AI.iAIFlags2 |= AIINVISIBLE;
	}
	else
	{
		planepnt->AI.lVar2 = lastslot1 + 4;
		CTSetUpAddToQueue(planepnt, cnt1);
	}
}

//**************************************************************************************
void CTSetUpAddToQueue(PlaneParams *planepnt, int waiting_area)
{

	planepnt->AI.iAICombatFlags1 &= ~(AI_READY_5);
	if(waiting_area)
	{
		planepnt->AI.fVarA[0] = 56.0f * FTTOWU;
		planepnt->AI.fVarA[1] = -102.0f * FTTOWU;
		planepnt->AI.fVarA[2] = 0.0f;
		planepnt->AI.lTimer3 = 25000;
		planepnt->AI.lVar3 = 2;
		planepnt->AI.iAIFlags2 |= AIINVISIBLE;
	}
	else
	{
		planepnt->AI.fVarA[0] = 52.0f * FTTOWU;
		planepnt->AI.fVarA[1] = -201.5f * FTTOWU;
		planepnt->AI.fVarA[2] = 4.712f;
		planepnt->AI.lTimer3 = 10000;
		planepnt->AI.lTimer2 = 80000;
		planepnt->AI.lVar3 = 98;
	}

	MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	planepnt->WorldPosition.SetValues(planepnt->AI.fVarA[4],planepnt->AI.fVarA[5],planepnt->AI.fVarA[6]);
	planepnt->WorldPosition *= carrier->Attitude;
	planepnt->WorldPosition += carrier->WorldPosition;

	planepnt->Attitude.SetRPH(0,0,planepnt->Heading);
	planepnt->Attitude *= carrier->Attitude;

	planepnt->Orientation.I.X = -planepnt->Attitude.m_Data.RC.R0C2;
	planepnt->Orientation.I.Y = -planepnt->Attitude.m_Data.RC.R1C2;
	planepnt->Orientation.I.Z = -planepnt->Attitude.m_Data.RC.R2C2;

	planepnt->Orientation.J.X = planepnt->Attitude.m_Data.RC.R0C0;
	planepnt->Orientation.J.Y = planepnt->Attitude.m_Data.RC.R1C0;
	planepnt->Orientation.J.Z = planepnt->Attitude.m_Data.RC.R2C0;

	planepnt->Orientation.K.X = -planepnt->Attitude.m_Data.RC.R0C1;
	planepnt->Orientation.K.Y = -planepnt->Attitude.m_Data.RC.R1C1;
	planepnt->Orientation.K.Z = -planepnt->Attitude.m_Data.RC.R2C1;

	planepnt->Attitude.GetRPH(&planepnt->Roll,&planepnt->Pitch,&planepnt->Heading);

	if(planepnt->WorldPosition.Y < (70.0f * WUTOFT))
	{
		planepnt->WorldPosition.Y = 70.0f * WUTOFT;
	}

	planepnt->LastWorldPosition -= planepnt->WorldPosition;

	if (!planepnt->PlaneCopy)
		planepnt->PlaneCopy = GetNewCarrierPlane(planepnt);
	else
	{
		if(planepnt->PlaneCopy->WorldPosition.Y < (70.0f * WUTOFT))
		{
			planepnt->PlaneCopy->WorldPosition.Y = 70.0f * WUTOFT;
		}
	}
}

//**************************************************************************************
void CTSetPlaneReadyToLaunch(PlaneParams *planepnt)
{
	long cattimes[3];
	PlaneParams *checkplane;
	int cnt;
	long worktime, ttime, slot3time;
	int slotnum;

	if(MultiPlayer && (!g_bIAmHost))
	{
		return;
	}

	if((planepnt->AI.Behaviorfunc != CTWaitingForLaunch) || (planepnt->AI.lVar3 != 100))
	{
		return;
	}

	if((planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED) && (planepnt->Brakes))
	{
		planepnt->Brakes = 0;
		if(planepnt == PlayerPlane)
		{
			DisplayWarning(BRAKE, OFF, 1);
		}
	}

	slot3time = -1;

	if(!(planepnt->Status & PL_AI_DRIVEN))
	{
		return;
	}

	if((planepnt->AI.Behaviorfunc != CTWaitingForLaunch) || (planepnt->AI.lVar3 != 100))
	{
		return;
	}

	if((planepnt->AI.lTimer2 < 25000) && (planepnt->AI.lTimer2 >= 0))
	{
		planepnt->AI.iAICombatFlags2 |= AI_READY_FOR_LAUNCH;
		planepnt->AI.lTimer3 = 0;
		if(planepnt->AI.lTimer2 < 2000)
		{
			planepnt->AI.lTimer2 = 2000;
		}

		if(MultiPlayer && (g_bIAmHost))
		{
			NetPutGenericMessage3Long(planepnt, GM3L_TAKEOFF_TIME, planepnt->AI.lTimer2, planepnt->AI.lTimer3);
		}
		return;
	}

	for(cnt = 0; cnt < 3; cnt ++)
	{
		cattimes[cnt] = -1;
	}

	for(checkplane = Planes; checkplane < LastPlane; checkplane ++)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == CTWaitingForLaunch) && (checkplane->AI.lVar3 == 100))
			{
				if((planepnt != checkplane) && (planepnt->AI.iHomeBaseId == checkplane->AI.iHomeBaseId))
				{
					if(!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
					{
						slotnum = (checkplane->AI.lVar2 - 1) % 4;
						if(slotnum == 2)
						{
							slot3time = planepnt->AI.lTimer2;
						}

						worktime = checkplane->AI.lTimer2;
						for(cnt = 0; cnt < 3; cnt ++)
						{
							if(cattimes[cnt] == -1)
							{
								cattimes[cnt] = worktime;
								break;
							}
							else if(worktime < cattimes[cnt])
							{
								ttime = cattimes[cnt];
								cattimes[cnt] = worktime;
								worktime = ttime;
							}
						}
					}
					else
					{
						if(checkplane->AI.lVar2 == -2)
						{
							if(cattimes[0] == -1)
							{
								cattimes[0] = 15000;
							}
						}
					}
				}
			}
		}
	}

//	slotnum = (checkplane->AI.lVar2 - 1) % 4;
	slotnum = (planepnt->AI.lVar2 - 1) % 4;

	if((slotnum == 3) && (slot3time >= 0))
	{
		planepnt->AI.iAICombatFlags2 |= AI_READY_FOR_LAUNCH;
		planepnt->AI.lTimer3 = 0;
		planepnt->AI.lTimer2 = slot3time + 20000;
		if(MultiPlayer && (g_bIAmHost))
		{
			NetPutGenericMessage3Long(planepnt, GM3L_TAKEOFF_TIME, planepnt->AI.lTimer2, planepnt->AI.lTimer3);
		}
		return;
	}

	if((cattimes[0] > 15000) || (cattimes[0] == -1))
	{
		worktime = 5000;
	}
	else if(((cattimes[1] - cattimes[0]) > 15000) || (cattimes[1] == -1))
	{
		worktime = cattimes[0] + 10000;
	}
	else if(((cattimes[2] - cattimes[1]) > 15000) || (cattimes[2] == -1))
	{
		worktime = cattimes[1] + 10000;
	}
	else
	{
		worktime = cattimes[2] + 10000;
	}

	if((worktime > planepnt->AI.lTimer2) && (planepnt->AI.lTimer2 >= 0))
	{
		planepnt->AI.iAICombatFlags2 |= AI_READY_FOR_LAUNCH;
		planepnt->AI.lTimer3 = 0;
		if(planepnt->AI.lTimer2 < 2000)
		{
			planepnt->AI.lTimer2 = 2000;
		}

		if(MultiPlayer && (g_bIAmHost))
		{
			NetPutGenericMessage3Long(planepnt, GM3L_TAKEOFF_TIME, planepnt->AI.lTimer2, planepnt->AI.lTimer3);
		}
		return;
	}

	planepnt->AI.iAICombatFlags2 |= AI_READY_FOR_LAUNCH;

	if(planepnt->AI.lTimer2 >= 0)
	{
		planepnt->AI.lTimer3 = planepnt->AI.lTimer2 - worktime;
	}
	else
	{
		if((cattimes[0] > 15000) || (cattimes[0] == -1))
		{
			planepnt->AI.lTimer3 = 0;
		}
		else if(((cattimes[1] - cattimes[0]) > 38000) || (cattimes[1] == -1))
		{
			planepnt->AI.lTimer3 = cattimes[0] + 20000;
		}
		else if(((cattimes[2] - cattimes[1]) > 38000) || (cattimes[2] == -1))
		{
			planepnt->AI.lTimer3 = cattimes[1] + 20000;
		}
		else
		{
			planepnt->AI.lTimer3 = cattimes[2] + 20000;
		}

		planepnt->AI.lTimer3 -= worktime;
	}

	planepnt->AI.lTimer2 = worktime;

	if(MultiPlayer && (g_bIAmHost))
	{
		NetPutGenericMessage3Long(planepnt, GM3L_TAKEOFF_TIME, planepnt->AI.lTimer2, planepnt->AI.lTimer3);
	}
}

//**************************************************************************************
int CT_ACLS_Ready()
{
	if((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 > 7) && (PlayerPlane->AI.lVar2 <= 10)))
		return(1);

	return(0);
}
