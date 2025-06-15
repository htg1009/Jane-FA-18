//************************************************************************************************
//  SFLIGHT.CPP
//************************************************************************************************
#include "F18.h"
#include "flightfx.h"
#include "particle.h"
#include "gamesettings.h"
#include "snddefs.h"

//#define G_TIX	(GRAVITY*FTTOWU/2500.0)	// Gravity in WU/tick squared
#define G_TIX	(GRAVITY/2500.0f)	// Gravity in WU/tick (Planes use F/S not WU stuff) even though I'm modifying speed instead of
									//  Position I'm still using 50hz squared since it seems to work and accleration for planes isn't
									//  Set for real gravity.  (ie F-15C should be greater than 32 and is only 20.  I only use this in
									//  One place anyway.

extern float CalcFuelFlow(PlaneParams *P,float ThrottlePercent,float Altitude,int UseMinMach);

extern AvionicsType Av;
extern void DoSpeedBrakeCAS(PlaneParams *P);
void SFCheckOnGroundStuff(PlaneParams *P);
extern void PlayPositionalNoGearHit(CameraInstance &camera,FPointDouble &SoundPosition);
extern float fBolterAdjust;

//************************************************************************************************
void CalcSimpleFlight(PlaneParams *P)
{


}
//************************************************************************************************
void CalcSimpleAttitude(PlaneParams *P)
{


}
//************************************************************************************************
//*
//*
//*
//*		Simple AI Flight - Scott Elson
//*
//*
//*
//************************************************************************************************
void CalcF18ControlSurfacesSimple(PlaneParams *P)
{
	// do nothing much for simple flight
	P->CommandedThrust = GetCommandedThrust(P, P->ThrottlePos);
}
//************************************************************************************************
void CalcF18CASUpdatesSimple(PlaneParams *P)
{
	// only one thing for simple flight
}
//************************************************************************************************
void CalcAeroForcesSimple(PlaneParams *P)
{
	// Calculate velocity of speed of sound at altitude(ft/sec)
	double Vss;
	if (P->Altitude <= 0)
		Vss = sqrt((1.4*1716.3*(519.0*(1.0-0.00000703*1.0))));   // make this equation a constant
	else
		Vss = sqrt(1.4*1716.3*(519.0*(1.0-0.00000703*P->Altitude)));

	// Calculate velocity of aircraft (ft/sec)
	if((P->V < 0) & (P->OnGround == 2))
	{
		P->V = (P->BfLinVel.X);
	  	P->BfLinVel.Z = 0.0;
	  	P->BfLinVel.Y = 0.0;
	}
	else if ((P->OnGround) && !((P->AI.iAIFlags2 & AILANDING) && (P->AI.AOADegPitch)))
	{
//		P->V = sqrt((P->BfLinVel.X*P->BfLinVel.X));
		P->V = fabs(P->BfLinVel.X);
	  	P->BfLinVel.Z = 0.0;
	  	P->BfLinVel.Y = 0.0;

	}else
		P->V = P->BfLinVel.Length();  /* gets an accurate length using sqrt*/

	P->Knots = P->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);

	// Calculate dynamic pressure at altitude
	float Q = GetDensityFactor(P);

	// Calculate Mach at Altitude
	if (P->Mach < 0.0f || P->OnGround) P->Mach = 0.0f;
	else
	 	P->Mach = (float)(P->V/Vss);

	//*************************************************************************************************************
	// Calculate Drag Forces - Create a force which realistically opposes our commanded thrust

	// this will determine acceleration
	// this will determine max altitude	we are able to attain
	// give use acceleration based on altitude

	double X0 =   -0.0184;
   	P->BfForce.X = Q*P->Stats->S*X0;	 // lbs.

	//*************************************************************************************************************
	// Calculate Lift Forces

   	P->BfForce.Z = 0.0f;

	//*************************************************************************************************************
	// Calculate Side Forces

 	P->BfForce.Y = 0.0f;

	//*************************************************************************************************************
	// Calculate Yawing Moment

 	P->BfMoment.Z = 0.0f;				//  ft./lbs of torque

	//*************************************************************************************************************
	// Calculate Rolling Moment

 	P->BfMoment.X = 0.0f;

	//*************************************************************************************************************
	// Calculate Pitch Moment

	P->BfMoment.Y = 0.0f;

	//*************************************************************************************************************
	// Calculate Thrust/Brake Effects

 	P->BfForce.X += (P->CommandedThrust*2.0f);

	DoSpeedBrakeCAS(P);
}

//************************************************************************************************
void CalcAeroDynamicsSimple(PlaneParams *P)
{
	float dspeed;
	float EffectiveAcceleration, orgaccel;
	float MaxSpeed, MaxAlt;
	float alteffect;
	double degroll, degpitch;
	float tropopause, dtalt;
	float stallatmaxalt, stallatthisalt;
#ifdef SREOLDSPEED
	double dworkvar;
	float gravityeffect;
#endif
	float rollpitcheffect;
	//  Helo added SRE
	long jobtype = pDBAircraftList[P->AI.iPlaneIndex].lType;
	int ishelo = 0;
	float speedangle;
	float workspeed;
	float aoadeg, cosroll, aoaoffset;
	int maxangle;
	float raddeg;
	float altpitchpercent;
	float fworkval, fworkval2;
	int ceiling = pDBAircraftList[P->AI.iPlaneIndex].iCeiling;
	long desiredspeed;
	double dtempval;
	int num50hz;
	ANGLE tempdangle;
	float yawmod = PYAW_G_MOD;  //  0.5f;
	float engineperc;
	double orgpitch, orgroll;
	ANGLE LastHeading = P->Heading;
	ANGLE LastRoll = P->Roll;
	float aoaperc;
	float stalleffect;
	float workdeccel;
	float fstallspd;
	FPointDouble orgpos = P->WorldPosition;
	float fuellimitsspeed;
	float dz, dy;
	PlaneParams *checkplane;
	double tempheight;


	if(ceiling == 0)
	{
		ceiling = 50000;
	}

	if(P->AI.fStoresWeight > 0)
	{
		fworkval = 1.0f - ((P->AI.fStoresWeight / P->AircraftDryWeight) * 0.3f);

		if(fworkval < 0.5f)
		{
			fworkval = 0.5f;
		}

		fworkval2 = ceiling;
		fworkval2 *= fworkval;

		ceiling = fworkval2;
	}

	orgaccel = EffectiveAcceleration = (P->MaxSpeedAccel * P->SpeedPercentage);

	MaxSpeed = pDBAircraftList[P->AI.iPlaneIndex].iMaxSpeed;

	MaxAlt = pDBAircraftList[P->AI.iPlaneIndex].iCeiling;

	if(P->Knots && P->IndicatedAirSpeed)
	{
		alteffect = P->IndicatedAirSpeed / P->Knots;
	}
	else
	{
		alteffect = AIConvertFtPSToSpeed(P->Altitude,  168.780972, AIDESIREDSPDCALC);  //  get 100 Kts
		alteffect /= 100.0f;
	}

	alteffect *= 1.10f;  //  Jets (which most of our planes are) have slightly better reaction to altitude changes.
	if((alteffect > 1.0f) || (alteffect <= 0))
	{
		alteffect = 1.0f;
	}

	if((MaxAlt > 36000.0f) && (pDBAircraftList[P->AI.iPlaneIndex].iAfterBurn))
	{
		if(P->Altitude > 36000.0f)
		{
			dtalt = (P->Altitude - 36000.0f) / (MaxAlt - 36000.0f);
			if((dtalt < 1.0f) && (dtalt > 0))
			{
				dtalt = 1.0f - dtalt;
				if(dtalt < 0.1f)
				{
					dtalt = 0.1f;
				}

			}
			stallatmaxalt = AIConvertSpeedToFtPS(MaxAlt, pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed, AIDESIREDSPDCALC) * (FTSEC_TO_MLHR*MLHR_TO_KNOTS) * 1.1f;
			tropopause = MaxSpeed - stallatmaxalt;
			MaxSpeed = stallatmaxalt + (tropopause * dtalt);
		}
	}
	else
	{
		MaxSpeed *= alteffect;
	}

	if((P->AI.fStoresWeight > 0) && (P->AircraftDryWeight) && (P->AI.Behaviorfunc != WeaponAsPlane))
	{
		fworkval = (P->AI.fStoresWeight / P->AircraftDryWeight);
		if(fworkval >= 0)
		{
			if(fworkval < 0.25f)
			{
				MaxSpeed *= 1.0f - (0.8f * fworkval);
			}
			else if(fworkval < 0.5f)
			{
				fworkval -= 0.25f;
				MaxSpeed *= 0.8f - (0.4f * fworkval);
			}
			else
			{
				fworkval -= 0.5;
				fworkval *= 0.2f;
				if(fworkval > 0.3)
					fworkval = 0.3;
				MaxSpeed *= 0.7f - fworkval;
			}
		}
	}

	fstallspd = pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed;
	stallatthisalt = AIConvertSpeedToFtPS(P->Altitude, pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed, AIDESIREDSPDCALC) * (FTSEC_TO_MLHR*MLHR_TO_KNOTS) * 1.1f;
	if(MaxSpeed < stallatthisalt)
	{
		MaxSpeed = stallatthisalt;
	}

	fuellimitsspeed = (10000.0);   //  jlm changed to default for player

	if (P->TotalFuel < fuellimitsspeed)
	{
		if(P->TotalFuel < (fuellimitsspeed * 0.5))
		{
			MaxSpeed = (MaxSpeed + stallatthisalt) * 0.5;
		}
		else
		{
			MaxSpeed = (MaxSpeed + stallatthisalt) * 0.75;
		}
	}

	if(P->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		if(MaxSpeed < PlayerPlane->Knots)
		{
			if(P->Altitude < (PlayerPlane->Altitude + 500))
			{
				if(P->AI.fStoresWeight <= (PlayerPlane->AI.fStoresWeight + 100.0f))
				{
					MaxSpeed = PlayerPlane->Knots;
				}
			}
		}
	}

	EffectiveAcceleration *= alteffect;

	workdeccel = EffectiveAcceleration;
	if(workdeccel < (orgaccel * 0.1f))
	{
		workdeccel = orgaccel * 0.1f;
	}

	//  Some code to help make accelerations not so drastic for AIs.  SRE
	if(P->Knots > fstallspd)
	{
		fworkval = 1.0f - ((P->Knots - fstallspd) / (MaxSpeed - fstallspd));
		if(fworkval <= 1.0f)
		{
			if(fworkval < 0.0f)
			{
				EffectiveAcceleration = 0.0f;
			}
			else
			{
				if(fworkval < 0.5f)
				{
					EffectiveAcceleration *= fworkval * 2.0f;
				}
			}
		}
	}

	if(EffectiveAcceleration < (orgaccel * 0.1f))
	{
		EffectiveAcceleration = orgaccel * 0.1f;
	}

//	workdeccel = EffectiveAcceleration;


	engineperc = 1.0f;
	if(!(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		if(P->DamageFlags & DAMAGE_BIT_LO_ENGINE)
		{
			engineperc -= 0.25f;
		}
		if(P->DamageFlags & DAMAGE_BIT_L_ENGINE)
		{
			engineperc -= 0.25f;
		}
		if(P->DamageFlags & DAMAGE_BIT_RO_ENGINE)
		{
			engineperc -= 0.25f;
		}
		if(P->DamageFlags & DAMAGE_BIT_R_ENGINE)
		{
			engineperc -= 0.25f;
		}
	}

	EffectiveAcceleration *= engineperc;

	if(EffectiveAcceleration == 0)
	{
		EffectiveAcceleration = -(GRAVITY * FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	}

	//  See if we are above ceiling and then rough check to see if we might be in top 20% of service ceiling.
	if((int)P->Altitude > ceiling)
	{
		if(P->DesiredPitch < 0x8000)
		{
			P->DesiredPitch = 0xF000;
			if(P->MaxPitchRate < 5.0f)
				P->MaxPitchRate = 5.0f;
		}
	}
	else if(((int)P->Altitude + 20000) > ceiling)
	{
		fworkval = P->Altitude / ceiling;
		//  If we are in the top 20% then determine percentage of pitch up ability.
		if(fworkval > 1.0f)
		{
			altpitchpercent = 0.0f;
		}
		else if(fworkval > 0.8f)
		{
			fworkval -= 0.8f;
			altpitchpercent = 1.0f - (fworkval / 0.2f);
		}
		else
		{
			altpitchpercent = 1.0f;
		}

		//  If restricting pitch up ability, also restrict maximum pitch.
		if(altpitchpercent < 1.0f)
		{
//			fworkval = 90 * altpitchpercent;
			fworkval = 20 * altpitchpercent;
			degpitch = AIConvertAngleTo180Degree(P->DesiredPitch);
			if(degpitch > fworkval)
			{
				P->DesiredPitch = AIConvert180DegreeToAngle(fworkval);
			}
		}
	}
	else
	{
		altpitchpercent = 1.0f;
	}

	//  Check to see if plane wants to go slower than its stall speed.
	if((P->HeightAboveGround > 1.0f) && (!P->OnGround))
	{
		desiredspeed = AIConvertFtPSToSpeed(P->Altitude, P->DesiredSpeed, AIDESIREDSPDCALC);
		if((desiredspeed < pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed) && (P->AI.lPlaneID != 85))
		{
			desiredspeed = pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed;
			P->DesiredSpeed = AIConvertSpeedToFtPS(P->Altitude, desiredspeed, AIDESIREDSPDCALC);
		}
#if 0
		else if(desiredspeed > (MaxSpeed * engineperc))
		{
			desiredspeed = MaxSpeed * engineperc;
			P->DesiredSpeed = AIConvertSpeedToFtPS(P->Altitude, desiredspeed, AIDESIREDSPDCALC);
		}
#else
		else
		{
			desiredspeed = (P->DesiredSpeed * FTSEC_TO_MLHR*MLHR_TO_KNOTS);

			//  Make sure planes that are refueling can reach 300 knots.
			if(P->RefuelingHoses[0].Status & RH_WANT_EXTEND)
			{
				float fworkvar;

				fworkvar = AIConvertSpeedToFtPS(P->Altitude, 325, AIDESIREDSPDCALC);

				if(MaxSpeed < (fworkvar))
				{
					MaxSpeed = fworkvar;
				}
			}

			if(desiredspeed > (MaxSpeed * engineperc))
			{
				desiredspeed = MaxSpeed * engineperc;
				P->DesiredSpeed = desiredspeed / (FTSEC_TO_MLHR*MLHR_TO_KNOTS);
			}
		}
#endif

	}

	if(P->AI.Behaviorfunc != WeaponAsPlane)
	{
		if(pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed > (pDBAircraftList[P->AI.iPlaneIndex].iMaxSpeed * engineperc))
		{
			P->AI.iAIFlags2 |= AI_CAN_CRASH;
		}
		else
		{
			P->AI.iAIFlags2 &= ~AI_CAN_CRASH;
		}
	}

	//  Restrict rolls for large aircraft.  Might switch this to getting a max roll from database.
	if(!(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		if(pDBAircraftList[P->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3))
		{
			degroll = AIConvertAngleTo180Degree(P->DesiredRoll);
			if(fabs(degroll) > 60)  //  was 60
			{
				degroll = (degroll < 0.0) ? -60 : 60;
				P->DesiredRoll = AIConvert180DegreeToAngle(degroll);
			}
		}
	}

	//  Helo added SRE
	if((jobtype & AIRCRAFT_TYPE_HELICOPTER)
		|| ((jobtype & AIRCRAFT_TYPE_VERTICAL) && (P->Knots < 120.0f)))
	{
		ishelo = 1;
	}


	//  Helo added SRE
	if((ishelo) || (P->AI.iAIFlags2 & AILANDING) || (P->AI.iAIFlags2 & AI_FAKE_AOA))
	{
		P->Pitch -= P->AI.AOADegPitch;
	}

	orgpitch = AIConvertAngleTo180Degree(P->Pitch);
	orgroll = AIConvertAngleTo180Degree(P->Roll);

	dtempval = P->dt / HIGH_FREQ;
	num50hz = dtempval;
#ifdef SREOLDSPEED
	//*************************************************************************
	//  Code for acceleration/deceleration for yaw changes and pitch here.
	//
	//  May want to add code for dpitch as well.
	//
	//  Decceleration due to roll may need to be plane type specific (Corner velocity)
	//  right now is hard coded 40.
	//
	//  Gravity effect may need to be plane type specific.
	//  represented by grevityeffect.
	//*************************************************************************
	if((P->RollYawCouple) && (!(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		degroll = AIConvertAngleTo180Degree(P->Roll);
		dworkvar = (float)(fabs(sin(DegToRad((float)degroll))) * P->V * (double)P->dt) * 1.0f / 40.0f;
		P->BfLinVel.X -= dworkvar;
		P->V -= dworkvar;
	}

	gravityeffect = 11.0f;  //  This may be plane type specific.

	degpitch = AIConvertAngleTo180Degree(P->Pitch);
	dworkvar = sin(DegToRad((float)degpitch)) * (double)gravityeffect * (double)P->dt;
	if((!(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (P->Pitch < 0x8000))
	{
		P->BfLinVel.X -= dworkvar;
		P->V -= dworkvar;
	}
#else
	/*
	**********************************************************************************
	This is what I did for unpowered missiles.  Since I am assuming the acceleration
	value of the plane counteracts gravity, then for a plane Gravity will be based off of
	flying level as 0 gravity acceleration.   SRE
	**********************************************************************************

	if(P->Pitch > 0x8000)
	{
		W->InitialVelocity -= FCosTimes(P->Pitch, G_TIX);
	}
	else
	{
		W->InitialVelocity -= G_TIX + FSinTimes(P->Pitch, G_TIX);
	}   */

	if((!P->OnGround) && (!ishelo) && ((!(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) || (P->Pitch < 0x8000)))
	{
//		dtempval = FCosTimes(P->Pitch, G_TIX * num50hz); j //  I think this should be -Sin
		dtempval = -FSinTimes(P->Pitch, G_TIX * num50hz);
		P->BfLinVel.X += dtempval;
		P->V += dtempval;
	}
#endif

	// Calculate Body Frame Linear and Rotational Accelerations

	// P->BfLinAccel is in (ft/sec^2)

	dspeed = P->DesiredSpeed - (float)P->V;

	if (dspeed > 0)
	{
		if (dspeed < (EffectiveAcceleration * P->dt))
			P->BfLinAccel.X = dspeed;
		else
		{
			P->BfLinAccel.X = EffectiveAcceleration * P->dt;
		}

		P->BfLinVel.X += P->BfLinAccel.X;
		P->V += P->BfLinAccel.X;
	}
	else if (dspeed < 0)  //  lack of engines won't change slowing down, use original accel.
	{
		workdeccel *= -2;

		if (dspeed > (workdeccel * P->dt))
			P->BfLinAccel.X = dspeed;
		else
			P->BfLinAccel.X = workdeccel * P->dt;

		P->BfLinVel.X += P->BfLinAccel.X;
		P->V += P->BfLinAccel.X;
	}


#if 0  //  Previous Acceleration
	P->BfLinAccel.X = (P->BfForce.X/P->TotalMass);
//	P->BfLinAccel.X = 10.0*(P->LeftThrustPercent/100.0);
	P->BfLinAccel.Y = 0.0;
	P->BfLinAccel.Z = 0.0;

	// P->BfRotAccel is in (rad/sec^2)

//	P->BfRotAccel.X = (P->BfMoment.X - P->BfRotVel.Y * P->BfRotVel.Z * (P->Stats->Iz - P->Stats->Iy)) / P->Stats->Ix;
//	P->BfRotAccel.Y = (P->BfMoment.Y - P->BfRotVel.X * P->BfRotVel.Z * (P->Stats->Ix - P->Stats->Iz)) / P->Stats->Iy;
//	P->BfRotAccel.Z = (P->BfMoment.Z - P->BfRotVel.X * P->BfRotVel.Y * (P->Stats->Iy - P->Stats->Ix)) / P->Stats->Iz;

	// Update Body Frame Velocities
	P->BfLinVel.AddScaledVector(P->dt,P->BfLinAccel);	  // LinVel in ft./sec.

#endif

//	P->BfRotVel.X += (P->BfRotAccel.X * P->dt);		  // RotVel in rad./sec.
//	P->BfRotVel.Y += (P->BfRotAccel.Y * P->dt);
//	P->BfRotVel.Z += (P->BfRotAccel.Z * P->dt);

//	ANGLE StickXAngle = (ANGLE)(fabs(P->StickX)) << 7;					 works good for stick flight
//	float RateX = (P->StickX > 0) ? 90.0 : -90.0;
//	P->BfRotVel.X = (float)FSinTimes(StickXAngle, DegToRad(RateX));

	ANGLE DeltaRoll = (ANGLE)(P->DesiredRoll+P->Roll);
	float EffectiveRollRate = (P->MaxRollRate*P->RollPercentage);

	if((P->RollYawCouple == -2)/* || (P->OnGround == 2)*/) //Don't fix roll if we're on the carrier deck
	{
		DeltaRoll = 0;
	}
	else if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 86) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 87))
	{
		DeltaRoll = 0;
	}

	if (DeltaRoll)
	{
//SRE		float tvar = (float)(DeltaRoll/DEGREE);
		float tvar = ((float)DeltaRoll/DEGREE);
		float stvar;

		tvar = (tvar > 360) ? 360 : tvar;
		stvar = (tvar > 180) ? (tvar - 360) : tvar;

//SRE		if (fabs(tvar) < (EffectiveRollRate * P->dt))
		if (fabs(stvar) < (EffectiveRollRate * P->dt))
			P->BfRotVel.X = DegToRad(stvar)/P->dt;
		else
			P->BfRotVel.X = (float)(((signed short)DeltaRoll) > 0) ? DegToRad(EffectiveRollRate) : -DegToRad(EffectiveRollRate);

		double temptemp = ((RadToDeg((float)P->BfRotVel.X)) * P->dt);
		P->Roll -= (ANGLE)((RadToDeg((float)P->BfRotVel.X)*DEGREE) * P->dt);
	}else
		P->BfRotVel.X = 0.0;


// now add stick pull into equation
//	ANGLE StickYAngle = (ANGLE)(fabs(P->StickY)) << 7;
//	float RateY = (P->StickY > 0) ? 90.0 : -90.0;
//	P->BfRotVel.Y = (float)FSinTimes(StickYAngle, DegToRad(RateY));

	ANGLE DeltaPitch = (ANGLE)(P->DesiredPitch-P->Pitch);

	float EffectivePitchRate = (P->MaxPitchRate*P->PitchPercentage);

	int inversepitch = 0;
	if(fabs(AIConvertAngleTo180Degree(P->Roll)) > 90)  //  Inverted roll makes opposite pitch required
	if((P->Roll > 0x4000) && (P->Roll < 0xC000))  //  Inverted roll makes opposite pitch required
	{
		DeltaPitch = (0xFFFF - DeltaPitch) + 1;
		inversepitch = 1;
	}

	if(DeltaPitch > 0x8000)  //  If trying to push negative Gs lower pitch rate to 1/3.
	{
		if((P->AI.Behaviorfunc != FlyCruiseMissile) && (P->AI.Behaviorfunc != WeaponAsPlane))
		{
			EffectivePitchRate /= 3;
		}
	}

#if 1  //  Attempt at more general lift vector stuff.
	if(P->RollYawCouple < 0)
	{
		if(DeltaPitch > 0x8000)
		{
			inversepitch = (inversepitch) ? 0 : 1;
			DeltaPitch = (0xFFFF - DeltaPitch) + 1;
		}
	}
#endif

	if(((DeltaPitch < 0x8000) && (!inversepitch)) || ((DeltaPitch > 0x8000) && (inversepitch)))
	{
		EffectivePitchRate *= altpitchpercent;
	}

	rollpitcheffect = (P->RollYawCouple >= 0) ? fabs(cos(DegToRad(P->Roll/(float)DEGREE))) : 1.0f;  //SRE  so plane doesn't pitch alot while rolled
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
//SRE		float tvar = (float)(DeltaPitch/DEGREE);
		float tvar = ((float)DeltaPitch/DEGREE);
		float stvar;
		tvar = (tvar > 360) ? 360 : tvar;
		stvar = (tvar > 180) ? (tvar - 360) : tvar;


//SRE		if (fabs(tvar) < (EffectivePitchRate * P->dt))
		if (fabs(stvar) < (EffectivePitchRate * P->dt))
			P->BfRotVel.Y = DegToRad(stvar)/P->dt;
		else
			P->BfRotVel.Y = (float)((DeltaPitch < 0x8000) ? DegToRad(EffectivePitchRate) : -DegToRad(EffectivePitchRate));

		if(inversepitch)
		{
			tempdangle = (ANGLE)((RadToDeg((float)P->BfRotVel.Y)*(double)DEGREE) * P->dt);
			P->Pitch -= tempdangle;
			tempdangle = (0xFFFF - tempdangle) + 1;
		}
		else
		{
			tempdangle = (ANGLE)((RadToDeg((float)P->BfRotVel.Y)*(double)DEGREE) * P->dt);
			P->Pitch += tempdangle;
		}

#ifndef SREOLDSPEED
		dtempval = fabs(FSinTimes(tempdangle,  P->V)) * yawmod;
		P->BfLinVel.X -= dtempval;
		P->V -= dtempval;
#endif
	}else
		P->BfRotVel.Y = 0.0;

	P->BfRotVel.Z = 0.0;	// there is no bf heading change - ever

	//  Helo added SRE
	if((ishelo) || (P->AI.iAIFlags2 & AILANDING) || (P->AI.iAIFlags2 & AI_FAKE_AOA))
	{
		P->Pitch += P->AI.AOADegPitch;
	}

	//  Landing added SRE
	float YawCoupleRate;
	if((P->OnGround) && (P->AI.iAIFlags2 & AILANDING))
	{
		YawCoupleRate = sin(sin(DegToRad(P->AI.DesiredRoll*(1.0f/(float)DEGREE)))*(DegToRad(P->YawRate)));
	}
	else if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 86) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 87))
	{
		YawCoupleRate = sin(sin(-DegToRad(P->DesiredRoll*(1.0f/(float)DEGREE)))*(DegToRad(P->YawRate)));
	}
	else
	{
		YawCoupleRate = (P->RollYawCouple > 0) ? sin(sin(DegToRad(P->Roll*(1.0f/(float)DEGREE)))*(DegToRad(P->YawRate))) : 0.0f;
#ifndef SREOLDSPEED
		tempdangle = (ANGLE)((RadToDeg((float)YawCoupleRate) * P->dt) * (double)DEGREE);
		dtempval = fabs(FSinTimes(tempdangle,  P->V)) * yawmod;
		P->BfLinVel.X -= dtempval;
		P->V -= dtempval;
#endif
	}

	//  Helo added SRE
	if((P->BfLinVel.X < 0) && (P->OnGround != 2))
	{
		P->BfLinVel.X = 0;
	}
	if((P->V < 0) && (P->OnGround != 2))
	{
		P->V = 0;
	}

	if((jobtype & AIRCRAFT_TYPE_VERTICAL) && (P->Knots >= 90.0f))
	{
		ishelo = 0;
		if(P->Knots < 120.0f)
		{
			P->AI.iAIFlags2 |= (AI_FAKE_AOA);
		}
	}

	P->BfLinVel.Z = 0;
	if(ishelo)
	{

		workspeed = P->Knots;
		maxangle = -25;
		if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 86) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 87))
		{
			speedangle = 0;
		}
		else if(jobtype & AIRCRAFT_TYPE_VERTICAL)
		{
			speedangle = (workspeed / 90.0f) * maxangle;
		}
		else
		{
			speedangle = (workspeed / MaxSpeed) * maxangle;
		}
		if(speedangle < maxangle)
		{
			speedangle = maxangle;
		}
		else if(speedangle > 0)
		{
			speedangle = 0;
		}
		raddeg = DegToRad(speedangle - AIConvertAngleTo180Degree(P->DesiredPitch));
		P->BfLinVel.X = P->V * cos(raddeg);
		P->BfLinVel.Z = P->V * sin(raddeg);
	}
	else if((P->AI.iAIFlags2 & AILANDING) && (P->V > 0))
	{
		raddeg = DegToRad(AIConvertAngleTo180Degree(P->AI.DesiredPitch));
		P->BfLinVel.X = P->V * cos(raddeg);
		P->BfLinVel.Z = P->V * sin(raddeg);
	}
	else if(P->AI.iAIFlags2 & AI_FAKE_AOA)
	{
		if(jobtype & AIRCRAFT_TYPE_VERTICAL)
		{
			workspeed = P->Knots;
			maxangle = -25;
			speedangle = ((-(P->Knots - 120.0f)) / 30.0f) * maxangle;

			if(speedangle < maxangle)
			{
				speedangle = maxangle;
			}
			else if(speedangle > 0)
			{
				speedangle = 0;
			}
			raddeg = DegToRad(speedangle);
			P->BfLinVel.X = P->V * cos(raddeg);
			P->BfLinVel.Z = P->V * sin(raddeg);
		}
		else if((P != PlayerPlane) || ((P->AI.Behaviorfunc != AIFlyTankerFormation) && (P->AI.Behaviorfunc != AITankerDisconnect)))
		{
			stalleffect = (P->IndicatedAirSpeed / (float)pDBAircraftList[P->AI.iPlaneIndex].iStallSpeed) - 1.0f;

			if(stalleffect <= 1.0f)
			{
				if(stalleffect < 0)
				{
					aoaperc = 1.0f;
				}
				else
				{
					aoaperc = 1.0f - stalleffect;
				}

				if(pDBAircraftList[P->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN))
				{
					raddeg = DegToRad(aoaperc * AISLOWDESIREDAOA);
				}
				else
				{
					raddeg = DegToRad(aoaperc * AIDESIREDAOA);
				}
				P->BfLinVel.X = P->V * cos(raddeg);
				P->BfLinVel.Z = P->V * sin(raddeg);
			}
			else
			{
				P->AI.iAIFlags2 &= ~(AI_FAKE_AOA);
			}
		}
		else
		{
			P->BfLinVel.X = P->V * cos(P->Alpha);
			P->BfLinVel.Z = P->V * sin(P->Alpha);
		}
	}

	// Update the Orientation vector (which is inertial frame referenced)

	DoubleVector3D Vel_i,Vel_j;

	// Vel_i in ft./sec.
	Vel_i.SetValues(P->BfRotVel.Z,P->Orientation.J);		  //scaled vector initialization
	Vel_i.X += YawCoupleRate * P->Orientation.I.Z;
	Vel_i.Z -= YawCoupleRate * P->Orientation.I.X;
	Vel_i.AddScaledVector(-P->BfRotVel.Y,P->Orientation.K);

	// Vel_j in ft./sec.
	Vel_j.SetValues(P->BfRotVel.X,P->Orientation.K);		  //scaled vector initialization
	Vel_j.X += YawCoupleRate * P->Orientation.J.Z;
	Vel_j.Z -= YawCoupleRate * P->Orientation.J.X;
	Vel_j.AddScaledVector(-P->BfRotVel.Z,P->Orientation.I);

	P->Orientation.I.AddScaledVector(P->dt,Vel_i);
	P->Orientation.I.Normalize();

	P->Orientation.J.AddScaledVector(P->dt,Vel_j);
	P->Orientation.J.Normalize();

	P->Orientation.K = P->Orientation.I;
	P->Orientation.K %= P->Orientation.J;  //cross product

	// Transform Velocity in Body Frame to Inertial Frame Velocity

	P->IfVelocity.X = P->BfLinVel.X * P->Orientation.I.X +
					  P->BfLinVel.Y * P->Orientation.J.X +
					  P->BfLinVel.Z * P->Orientation.K.X;

	// Up and Down velocity relative to the inertial reference frame

	P->IfVelocity.Y = P->BfLinVel.X * P->Orientation.I.Y +  		// forward nose velocity
					  P->BfLinVel.Y * P->Orientation.J.Y +  		// side force velocity
					  P->BfLinVel.Z * P->Orientation.K.Y;

	P->IfVelocity.Z = P->BfLinVel.X * P->Orientation.I.Z +
					  P->BfLinVel.Y * P->Orientation.J.Z +
					  P->BfLinVel.Z * P->Orientation.K.Z;

	// Move Airplane in Inertial Frame of Reference based on updated Inertial Velocities
	// IfVelocity is in Ft./sec.*dt

	P->LastWorldPosition = P->WorldPosition;
	P->WorldPosition.AddScaledVector((float)P->dt * (1.0f/WUTOFT),P->IfVelocity);

	P->DistanceMoved = (float)(P->WorldPosition - P->LastWorldPosition);  // Quick distance

	if(P->RollYawCouple < 0)
	{
		double temproll, temppitch;
		ANGLE atemproll;

		temproll = RadToDeg(atan2(P->Orientation.J.Y,-P->Orientation.K.Y));
		atemproll = AIConvert180DegreeToAngle(temproll);
		temppitch = RadToDeg(asin(P->Orientation.I.Y));

		if(((fabs(orgroll) >= 90) && (fabs(temproll) < 90)) || ((fabs(orgroll) <= 90) && (fabs(temproll) > 90)))
//		if((((LastRoll >= 0x4000) && (LastRoll <= 0xC000)) && ((atemproll < 0x4000) || (atemproll > 0xC000))) || (((LastRoll <= 0x4000) || (LastRoll >= 0xC000)) && ((atemproll > 0x4000) && (atemproll < 0xC000))))
		{
#if 0
			if((P->DesiredPitch > 0x4000) && (P->DesiredPitch < 0xC000))
			{
				P->DesiredPitch = 0;
			}
#else   //  Attempt at more general lift vector stuff.
			P->DesiredPitch = (0xFFFF - P->DesiredPitch) + 1;
#endif
		}
		P->Roll = atemproll;
		P->Pitch = AIConvert180DegreeToAngle(temppitch);
	}

	// clip world position here and do ground collision
	double land_height;
	MovingVehicleParams *carrier = NULL;
	double gear_height = 0.0;
	double add_height = 0.0;
	BOOL need_add_height = TRUE;

	if (P->OnGround == 2)
	{
		carrier =  &MovingVehicles[P->AI.iHomeBaseId];

		if (P->Status & PL_CARRIER_REL)
		{
			FPoint rel;
			float deck_height;
			int forcedeck = 0;
			float offsetpos;

			if(P->AI.Behaviorfunc == AISARWait)
			{
				tempheight = P->WorldPosition.Y;
				P->WorldPosition.Y = tempheight + (100.0f * FTTOWU);
			}

			if (P->AI.Behaviorfunc == CTDoCarrierLaunch)
			{
				if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
				{
					forcedeck = 1;
					dy = SOV_LAND_POINT_Y;
					for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
					{
						if(checkplane->PlaneCopy == P)
						{
							break;
						}
					}
					if(checkplane > LastPlane)
					{
						checkplane = P;
					}

//					offsetpos = checkplane->AI.fVarA[6] + (pDBAircraftList[checkplane->AI.iPlaneIndex].OffSet[OFFSET_FRONT_WHEEL].Z * FTTOWU);
					offsetpos = checkplane->AI.fVarA[6];
					if(offsetpos < (-438.5f * FTTOWU))
					{
						dz = -offsetpos + (-438.5f * FTTOWU);
						dy += (4.72f * FTTOWU) + (dz * (5.65f / 42.5f));
					}
					else if(offsetpos < (-375.0f * FTTOWU))
					{
						dz = -offsetpos + (-375.0f * FTTOWU);
						dy += (1.20f * FTTOWU) + (dz * (3.55f / 63.5));
					}
					else if(offsetpos < (-300.0f * FTTOWU))
					{
						dz = offsetpos - (-300.0f * FTTOWU);
						dy += (dz * (1.2f / 75.0f));
					}
					rel.SetValues(0.0f,dy,0.0f);
				}
				else
				{
					rel.SetValues(0.0f,(float)(70.0 FEET),0.0f);
				}
			}
			else
			 	rel = P->WorldPosition;

			if(forcedeck)
			{
				land_height = (double)dy;
			}
			else if (OverRunway(carrier->Type->Model,rel,&deck_height))
				land_height = (double)deck_height;
			else
			{
				land_height = 0;
				carrier = NULL;
			}

			if(P->AI.Behaviorfunc == AISARWait)
			{
				P->WorldPosition.Y = tempheight;
			}
		}
		else
		{
			land_height = GetPlaneLandHeightWithCarriers(P,&carrier,&add_height);
			need_add_height = FALSE;
		}
	}
	else
	{
		P->Status &= ~PL_SPEED_BOLTER;  // keeps more lines from snagging plane
		/* -----------------6/24/99 7:04PM---------------------------------------------------------------------
		 * are we landing?
		 * ----------------------------------------------------------------------------------------------------*/
		if ((P->AI.iAIFlags2 & AILANDING) && (P->AI.iAICombatFlags1 & AI_HOME_CARRIER))
		{
			/* -----------------6/24/99 7:04PM---------------------------------------------------------------------
			 * are we landing on a carrier
			 * ----------------------------------------------------------------------------------------------------*/
			if ((P->AI.Behaviorfunc == AIFlyHeloCarrierFinal) || (P->AI.Behaviorfunc == AIFlyCarrierFinal) || (P->AI.Behaviorfunc == AIFlyBolter) || (P->AI.Behaviorfunc == AICarrierTrap))
				carrier =  &MovingVehicles[P->AI.iHomeBaseId];
		}

		if (carrier)
		{
			if(P->AI.Behaviorfunc == AIFlyHeloCarrierFinal)
			{
				tempheight = P->WorldPosition.Y;
				P->WorldPosition.Y = tempheight + (100.0f * FTTOWU);
			}

			land_height = GetPlaneLandHeightWithCarriers(P,&carrier,&add_height);

			if (P->AI.Behaviorfunc == AIFlyHeloCarrierFinal)
			{
				P->WorldPosition.Y = tempheight;
			}
			need_add_height = FALSE;
		}
		else
	 		land_height = LandHeight(P->WorldPosition.X, P->WorldPosition.Z);
	}


	P->HeightAboveGround = (float)(P->WorldPosition.Y - land_height);

	if (need_add_height)
	{
		if (P->Orientation.K.Y < 0)
		{
			if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
				gear_height = P->Type->GearDownHeight;
			else
				if (P->LandingGear)
					gear_height = P->Type->GearUpHeight+(float)FSinTimes((P->LandingGear << 6),(P->Type->GearDownHeight-P->Type->GearUpHeight));
				else
					gear_height = P->Type->GearUpHeight;

//			if (P->Orientation.I.Y < 0)
				gear_height += 1.5f*P->Type->GearDownHeight*fabs(P->Orientation.I.Y); //front gear
//			else
//				gear_height += 0.8f*P->Type->GearDownHeight*P->Orientation.I.Y; //back gear

		}
		else
			gear_height = P->Type->TailHeight;

		gear_height *= (float)fabs(P->Orientation.K.Y * (1.0f - (P->Orientation.J.Y*P->Orientation.J.Y)));
		add_height = (float)fabs(P->Orientation.J.Y*P->Type->ShadowLRXOff);

		if (add_height < gear_height)
			add_height = gear_height;

	}

	P->HeightForGear = (float)(add_height + P->Type->GearDownHeight*0.5f);

	land_height += add_height;

	if (P->WorldPosition.Y <= land_height)
	{
		P->WorldPosition.Y = land_height;
		if (!P->OnGround)
		{
			if(P->AI.iAIFlags2 & AI_CAN_CRASH)
			{
				if(P->AI.lPlaneID == 74)
				{
					if(P->AI.Behaviorfunc == WeaponAsPlane)
					{
						P->AI.Behaviorfunc = AIDeathSpiral;
					}
				}

				CrashPlane(P,PL_STATUS_CRASHED,0);
				if (carrier)
					P->OnGround = 1;
				else
					P->OnGround = 2;
			}
			else
			{
				if (!carrier)
					P->OnGround = 1;
				else
					P->Status |= PL_ONGROUND_NEXT;
			}
		}
	}
	else
		if(P->OnGround)
		{
			if ((P->OnGround == 2) || (P->Status & PL_ONGROUND_NEXT))
			{
				if(((carrier == NULL) || ((P->AI.Behaviorfunc == AIWaitForTrapOrBolter) && (((P->TailHookFlags & TH_BOLTER_FLAG) == TH_BOLTER_FLAG) || ((P->TailHookFlags & 4) && ((P->TailHookFlags & 0x670000) != 0x630000)) || (P->WorldPosition.Y > (land_height * 10.0f))))) && (!(pDBAircraftList[P->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL))))
				{
					if(P->Status & PL_CARRIER_REL)
					{
						for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
						{
							if(checkplane->PlaneCopy == P)
							{
								break;
							}
						}
						if(checkplane > LastPlane)
						{
							checkplane = P;
						}
					}
					else
						checkplane = P;


					if(P->AI.iAICombatFlags1 & AI_HOME_CARRIER)
					{
						carrier =  &MovingVehicles[P->AI.iHomeBaseId];
						PlaneReleaseTailhook(P,carrier);
					}
					checkplane->AI.Behaviorfunc = AIFlyBolter;
					fBolterAdjust += (3.0f * FTTOWU);
					checkplane->AI.lTimer3 = 120000;
					checkplane->OnGround = 0;
					P->AI.Behaviorfunc = AIFlyBolter;
					P->AI.lTimer3 = 120000;
					P->OnGround = 0;
					if(AICAllowThisRadio(checkplane - Planes, 1))
					{
						AICJustMessage(AIC_BOLTER, AICF_BOLTER, SPCH_LSO);
						AICAddSoundCall(AIC_BolterStart, checkplane - Planes, 4000, 50);
					}
				}
				else
				{
					P->WorldPosition.Y = land_height;
				}
//				if (!carrier && !ishelo)
//					P->OnGround = 0;
			}
			else
				if((P->AI.iAIFlags2 & AI_STAY_ON_GROUND) || ((P->WorldPosition.Y <= (land_height + AIHEIGHTBUFFER)) && (P->AI.Behaviorfunc != AIFlyTakeOff)))
				{
					P->WorldPosition.Y = land_height;
				}
				else
				{
					P->OnGround = 0;
				}
		}
		else
			if (P->HeightAboveGround > (50 FEET))
				P->Status &= ~PL_ONGROUND_NEXT;

	if((P == PlayerPlane) || (P == PlayerPlane->PlaneCopy))
		SFCheckOnGroundStuff(P);

	if (carrier && (pDBShipList[carrier->iVDBIndex].lShipID != 6) && (!(GameLoop & 3)) && (P->AI.Behaviorfunc == CTDoCarrierLaunch) && (P->AI.lVar3 == 0))
		MakeCatSteamCanister(P->WorldPosition,carrier);

	if ((P->TailHook != 0.0f) && (P->HeightAboveGround < 100 FEET))
	{
		if (P->OnGround == 2)
			UpdateTailHookAndCheckForWireCatch(P,carrier);
		else
		{
			if (P->AI.Behaviorfunc == AIFlyCarrierFinal)
				UpdateTailHookAndCheckForWireCatch(P,carrier);
			else
				UpdateTailHookAndCheckForWireCatch(P,NULL);
		}
	}


	P->Altitude = (float)(P->WorldPosition.Y * WUTOFT);

	//  Helo added SRE
	if((ishelo) || (P->AI.iAIFlags2 & AILANDING) || (P->AI.iAIFlags2 & AI_FAKE_AOA))
	{
		if(P->V > 0)
		{
			P->LastAlpha = P->Alpha;
			if((P != PlayerPlane) || ((P->AI.Behaviorfunc != AIFlyTankerFormation) && (P->AI.Behaviorfunc != AITankerDisconnect)))
			{
				P->Alpha = (double)atan2(P->BfLinVel.Z, P->BfLinVel.X);
				aoadeg = RadToDeg(P->Alpha);
			}
			else
			{
				aoadeg = RadToDeg(P->Alpha);
			}

			cosroll = cos(DegToRad(fabs(AIConvertAngleTo180Degree(P->Roll))));
			aoaoffset = cosroll * aoadeg;
			P->AI.AOADegPitch = AIConvert180DegreeToAngle(aoaoffset);
		}
	}
}

//**************************************************************************************
void StartCasualAutoPilot()
{
	PlayerPlane->Status &= ~AL_DEVICE_DRIVEN;
	PlayerPlane->Status |= AL_AI_DRIVEN;
	PlayerPlane->AI.iAICombatFlags1 |= AI_HUMAN_CONTROLLED;

	PlayerPlane->DoControlSim = CalcF18ControlSurfacesSimple;
	PlayerPlane->DoCASSim = CalcF18CASUpdatesSimple;
	PlayerPlane->DoForces = CalcAeroForcesSimple;
	PlayerPlane->DoPhysics = CalcAeroDynamicsSimple;
	PlayerPlane->dt = 0;
	PlayerPlane->UpdateRate = HIGH_AERO;

	PlayerPlane->MaxPitchRate = 10.0;
	PlayerPlane->MaxRollRate = 90.0;
	PlayerPlane->YawRate = pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iMaxYawRate;

	PlayerPlane->MaxSpeedAccel = pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iAcceleration;	//15
	PlayerPlane->SpeedPercentage = 1.0;

	PlayerPlane->BfLinVel.X = PlayerPlane->V;
	PlayerPlane->BfLinVel.Y = 0;
	PlayerPlane->BfLinVel.Z = 0;

	PlayerPlane->Alpha = 0;
	PlayerPlane->LastAlpha = -999.0;

	if((PlayerPlane->OnGround) || (((PlayerPlane->HeightAboveGround * WUTOFT) < 10.0f) && (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)))
	{
		PlayerPlane->AI.Behaviorfunc = AIFlyTakeOff;
		PlayerPlane->AI.OrgBehave = AIFlyFormation;
	}
	else
	{
		PlayerPlane->AI.Behaviorfunc = AIFlyFormation;
	}
}

//**************************************************************************************
void EndCasualAutoPilot()
{
	DoubleVector3D clearvector;

	clearvector.X = 0;
	clearvector.Y = 0;
	clearvector.Z = 0;

	PlayerPlane->Status |= AL_DEVICE_DRIVEN;
	PlayerPlane->Status &= ~AL_AI_DRIVEN;
//	AICAddAIRadioMsgs("DEVICE", 50);
	PlayerPlane->DoControlSim = CalcF18ControlSurfaces;
	PlayerPlane->DoCASSim     = CalcF18CASUpdates;
	PlayerPlane->DoForces     = CalcAeroForces;
	PlayerPlane->DoPhysics    = CalcAeroDynamics;
	PlayerPlane->dt = HIGH_FREQ;
	PlayerPlane->UpdateRate = HIGH_AERO;

	PlayerPlane->Alpha = 0;
	PlayerPlane->LastAlpha = -999.0;

	PlayerPlane->BfLinVel.Y = 0;
	PlayerPlane->BfLinVel.Z = 0;
	PlayerPlane->BfForce = clearvector;
	PlayerPlane->BfMoment = clearvector;
	PlayerPlane->BfGhostLinVel = clearvector;
	PlayerPlane->BfRotVel = clearvector;
	PlayerPlane->BfLinAccel = clearvector;
	PlayerPlane->BfRotAccel = clearvector;
	PlayerPlane->BfVelocity = clearvector;
	PlayerPlane->IfVelocity = clearvector;
	PlayerPlane->ElevatorTrim = 0;
	PlayerPlane->AltitudeHoldTrim = 0;
	PlayerPlane->AttitudeHoldTrim = 0;

	PlayerPlane->Aileron = 0;				// current position
	PlayerPlane->Rudder = 0;
	PlayerPlane->TotalElevator = 0;			// get rid of
	PlayerPlane->SymetricalElevator = 0;
	PlayerPlane->DifferentialElevator = 0;
}

//**************************************************************************************
void StartCasualAutoPilotPlane(PlaneParams *planepnt)
{
	planepnt->Status &= ~(AL_DEVICE_DRIVEN|AL_COMM_DRIVEN);
	planepnt->Status |= AL_AI_DRIVEN;
	planepnt->AI.iAICombatFlags1 |= AI_HUMAN_CONTROLLED;

	planepnt->DoControlSim = CalcF18ControlSurfacesSimple;
	planepnt->DoCASSim = CalcF18CASUpdatesSimple;
	planepnt->DoForces = CalcAeroForcesSimple;
	planepnt->DoPhysics = CalcAeroDynamicsSimple;
	planepnt->dt = 0;
	planepnt->UpdateRate = HIGH_AERO;

	planepnt->MaxPitchRate = 10.0;
	planepnt->MaxRollRate = 90.0;
	planepnt->YawRate = pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxYawRate;

	planepnt->MaxSpeedAccel = pDBAircraftList[planepnt->AI.iPlaneIndex].iAcceleration;	//15
	planepnt->SpeedPercentage = 1.0;

	planepnt->BfLinVel.X = planepnt->V;
	planepnt->BfLinVel.Y = 0;
	planepnt->BfLinVel.Z = 0;

	if((planepnt->OnGround) || (((planepnt->HeightAboveGround * WUTOFT) < 10.0f) && (planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED)))
	{
		planepnt->AI.Behaviorfunc = AIFlyTakeOff;
		planepnt->AI.OrgBehave = AIFlyFormation;
	}
	else
	{
		planepnt->AI.Behaviorfunc = AIFlyFormation;
	}
}

//**************************************************************************************
void EndCasualAutoPilotPlane(PlaneParams *planepnt)
{
	DoubleVector3D clearvector;

	clearvector.X = 0;
	clearvector.Y = 0;
	clearvector.Z = 0;

	if(planepnt == PlayerPlane)
	{
		planepnt->Status |= AL_DEVICE_DRIVEN;
	}
	else
	{
		planepnt->Status |= AL_COMM_DRIVEN;
	}
	planepnt->Status &= ~AL_AI_DRIVEN;
//	AICAddAIRadioMsgs("DEVICE", 50);
	planepnt->DoControlSim = CalcF18ControlSurfaces;
	planepnt->DoCASSim     = CalcF18CASUpdates;
	planepnt->DoForces     = CalcAeroForces;
	planepnt->DoPhysics    = CalcAeroDynamics;
	planepnt->dt = HIGH_FREQ;
	planepnt->UpdateRate = HIGH_AERO;

	planepnt->Alpha = 0;
	planepnt->LastAlpha = -999.0;

	planepnt->BfLinVel.Y = 0;
	planepnt->BfLinVel.Z = 0;
	planepnt->BfForce = clearvector;
	planepnt->BfMoment = clearvector;
	planepnt->BfGhostLinVel = clearvector;
	planepnt->BfRotVel = clearvector;
	planepnt->BfLinAccel = clearvector;
	planepnt->BfRotAccel = clearvector;
	planepnt->BfVelocity = clearvector;
	planepnt->IfVelocity = clearvector;
	planepnt->ElevatorTrim = 0;
	planepnt->AltitudeHoldTrim = 0;
	planepnt->AttitudeHoldTrim = 0;

	planepnt->Aileron = 0;				// current position
	planepnt->Rudder = 0;
	planepnt->TotalElevator = 0;			// get rid of
	planepnt->SymetricalElevator = 0;
	planepnt->DifferentialElevator = 0;
}

//************************************************************************************************
float GetAISingleEngineFuelFlow(PlaneParams *P, float ThrottlePercent, float Altitude)
{
	float FuelFlow;		// Lb./Min.
	float MaxFuelFlow,MinFuelFlow;
	float Low,High,Range;

	float ClipMach = P->Mach;
	if (ClipMach > 2.0) ClipMach = 2.0;

	float ClipAlt = Altitude;
	if (ClipAlt > 70000)
		ClipAlt = 70000;

	 // being extra cautious by forcing indexes to be in range
    int AltIndex = round(P->Altitude/5000);
    if (AltIndex > 14)
		AltIndex = 14;
    if (AltIndex < 0)
	    AltIndex = 0;

	if (ClipMach <= 0.8)
	{
		FuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,TRUE);
		Range = FFLowMach[AltIndex][3] - FFLowMach[AltIndex][0];
	}
	else if(ClipMach >= 1.1)
	{
		FuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,FALSE);
		Range = FFHighMach[AltIndex][3] - FFHighMach[AltIndex][0];
	}
	else  // interpolate between the two tables
	{
		MinFuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,TRUE);
		MaxFuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,FALSE);
		FuelFlow    = MinFuelFlow + (((ClipMach - 0.8)/0.3)*(MaxFuelFlow - MinFuelFlow));

		Low  = FFLowMach[AltIndex][0] + (((ClipMach - 0.8)/0.3)*(FFHighMach[AltIndex][0] - FFLowMach[AltIndex][0]));
  		High = FFLowMach[AltIndex][3] + (((ClipMach - 0.8)/0.3)*(FFHighMach[AltIndex][3] - FFLowMach[AltIndex][3]));
		Range = High - Low;
	}

	return (FuelFlow);
}

//************************************************************************************************
void GetAISimpleFuelUsage(PlaneParams *P)
{
	float LeftFuelFlowLow,LeftFuelFlowHigh,LeftFuelFlow;
	int TempAlt = P->Altitude;
	float engineperc;

	if(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	if((UnlimitedFuel) && (P == PlayerPlane))
	{
		return;
	}

	if (TempAlt > 70000.0) TempAlt = 70000.0;

	TempAlt = TempAlt - (TempAlt % 5000);


	LeftFuelFlowLow  = GetAISingleEngineFuelFlow(P, P->LeftThrustPercent, (float)TempAlt);
	LeftFuelFlowHigh = GetAISingleEngineFuelFlow(P, P->LeftThrustPercent, (float)TempAlt + 5000.0);
	LeftFuelFlow     = LeftFuelFlowLow + ( ((P->Altitude - TempAlt)/5000.0)*(LeftFuelFlowHigh - LeftFuelFlowLow));

	float LeftFF;
	float RightFF;

	if(P->Type->EngineConfig == ONE_REAR_ENGINE)
	{
		LeftFF  = RightFF = LeftFuelFlow;
		RightFF = 0;
	}
	else
	{
		LeftFF  = RightFF = LeftFuelFlow;

		engineperc = 1.0f;
		if(P->DamageFlags & DAMAGE_BIT_LO_ENGINE)
		{
			engineperc -= 0.5f;
		}
		if(P->DamageFlags & DAMAGE_BIT_L_ENGINE)
		{
			engineperc -= 0.5f;
		}
		LeftFF *= engineperc;

		engineperc = 1.0f;
		if(P->DamageFlags & DAMAGE_BIT_RO_ENGINE)
		{
			engineperc -= 0.5f;
		}
		if(P->DamageFlags & DAMAGE_BIT_R_ENGINE)
		{
			engineperc -= 0.5f;
		}
		RightFF *= engineperc;
	}

	float LbsConsumed = ((LeftFF + RightFF)/60.0f)*(float)(DeltaTicks/1000.0f);

	LbsConsumed *= 0.50f;  //  AI's seemed to be consuming fuel a little too fast.

	if((P != PlayerPlane) || (P->OnGround != 2))
	{
		if (P->TotalFuel < (273.0f + (P - Planes)))
		{
			LbsConsumed = 0;  //  I really don't want AI's to run out of gas
		}
		else if (P->TotalFuel < 2000.0f)
		{
			LbsConsumed *= 0.01f;  //  I really don't want AI's to run out of gas
		}
		else if (P->TotalFuel < Av.Fuel.BingoVal)
		{
			LbsConsumed *= 0.5f;  //  Start slowing down
		}
	}

	if (P->SystemInactive & DAMAGE_BIT_FUEL_TANKS)	 // 2% fuel leak
		LbsConsumed *= 1.02;

	float orgLbsConsumed = LbsConsumed;

	// Suck fuel from the Wing Drop Tanks first
	if (P->WingDropFuel != 0.0f)
	{
		if (LbsConsumed < P->WingDropFuel)
		{
			P->WingDropFuel -= LbsConsumed;
			LbsConsumed = 0.0f;
		}
		else
		{
			P->WingDropFuel = 0.0f;
			LbsConsumed -= P->WingDropFuel;		// let's suck somewhere else now
		}
	}

	// then suck the Center Drop Tank
	if ((P->CenterDropFuel != 0.0f) && (LbsConsumed != 0.0f))
	{
		if (LbsConsumed < P->CenterDropFuel)
		{
			P->CenterDropFuel -= LbsConsumed;
			LbsConsumed = 0.0f;
		}
		else
		{
			P->CenterDropFuel = 0.0f;
			LbsConsumed -= P->CenterDropFuel;
		}
	}

	// then use inboard fuel including conformal tanks
	if ((P->InternalFuel != 0.0) && (LbsConsumed != 0.0))
	{
		P->InternalFuel -= LbsConsumed;
		if (P->InternalFuel <= 0.0f)
			P->InternalFuel = 0.0f;
	}

	P->TotalFuel = (P->InternalFuel + P->CenterDropFuel + P->WingDropFuel);

	if((AIInPlayerGroup(P)) && (P != PlayerPlane))
	{
		if (P->TotalFuel < Av.Fuel.BingoVal)
		{
			if ((P->TotalFuel + orgLbsConsumed) >= (Av.Fuel.BingoVal))
			{
				AIC_GenericMsgPlane(P - Planes, 2);
			}
		}
	}
}

#if 1
//************************************************************************************************
void SFCheckOnGroundStuff(PlaneParams *P)
{
	if (((PlayerPlane->HeightAboveGround * WUTOFT) > 20) && (!P->OnGround))
		UFC.TireOneShotEnabled = 1;

//	if ((!UFC.OnGroundLastFrame && PlayerPlane->OnGround))
	if(!UFC.OnGroundLastFrame)
		UFC.TireSqueelOneShot = 1;

	if (P->OnGround)
	{
		BOOL blow_up = FALSE;
		if(P->OnGround > 1)
		{
			if (!((PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) || (PlayerPlane->FlightStatus & PL_PLANE_DITCHING)))
				blow_up = TRUE;

			if (!SndIsSoundPlaying(g_dwTireSqueel))
				g_dwTireSqueel=0;

			if (UFC.TireSqueelOneShot && UFC.TireOneShotEnabled)	// when we first touchdown on ground
			{
				blow_up = CheckLandingAttitude(P);

				if (!blow_up)
				{
					if(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)
					{
						if (!g_dwTireSqueel)
						{
    						g_dwTireSqueel = SndQueueSpeech(SND_TIRE_SQUEEL, 0 ,g_iBaseBettySoundLevel,2);
    						SndServiceSound();
						}
					}
					else
					{
						PlayPositionalNoGearHit(Camera1,PlayerPlane->WorldPosition);
					}
					UFC.ILSStatus = OFF;
				}

				UFC.TireSqueelOneShot = UFC.TireOneShotEnabled = 0;
			}

		}
#if 0
		else
		{
			PlayerPlane->TerrainType = GetGroundType(PlayerPlane->WorldPosition,&PlayerPlane->TerrainType1,&PlayerPlane->TerrainType2);

			if(PlayerPlane->FlightStatus & PL_PLANE_DITCHING)
			{
				blow_up = FALSE;
			}
			else if (PlayerPlane->TerrainType == TT_RUNWAY)
			{
				if (!((PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) || (PlayerPlane->FlightStatus & PL_PLANE_DITCHING)))
					blow_up = TRUE;

				if (!SndIsSoundPlaying(g_dwTireSqueel))
					g_dwTireSqueel=0;

				if (UFC.TireSqueelOneShot && UFC.TireOneShotEnabled)	// when we first touchdown on ground
				{
					blow_up = CheckLandingAttitude(P);

					if (!blow_up)
					{
						if(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)
						{
							if (!g_dwTireSqueel)
							{
    							g_dwTireSqueel = SndQueueSpeech(SND_TIRE_SQUEEL, 0 ,g_iBaseBettySoundLevel,2);
    							SndServiceSound();
							}
						}
						else
						{
							PlayPositionalNoGearHit(Camera1,PlayerPlane->WorldPosition);
						}
						UFC.ILSStatus = OFF;
					}

					UFC.TireSqueelOneShot = UFC.TireOneShotEnabled = 0;
				}
			}
			else if (!(UFC.TireOneShotEnabled))
			{
				if(!(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED))
				{
					blow_up = TRUE;
				}
				if (PlayerPlane->TerrainType == TT_AIRPORT)
				{
					if ((PlayerPlane->Knots > 300) || (!(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)))
						blow_up = TRUE;
				}
				else
					if ((PlayerPlane->TerrainType == TT_DESTROYED_RUNWAY) || (PlayerPlane->TerrainType == TT_LAND))
					{
						if (PlayerPlane->Knots > 48)
						{
							if((PlayerPlane->Knots > 120) || (MultiPlayer))
							{
								blow_up = TRUE;
							}
							else
							{
								SetUpDitch(P);
							}
						}
					}
					if (PlayerPlane->TerrainType == TT_WATER)
						blow_up = TRUE;
			}
			else
			{
				if ((PlayerPlane->TerrainType == TT_AIRPORT) && (!MultiPlayer))
				{
					blow_up = CheckLandingAttitude(P);
				}
				else
				{
					blow_up = TRUE;
				}

				if ((!blow_up) && (!MultiPlayer))
				{
					SetUpDitch(P);
					if (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)
					{
						if (!g_dwTireSqueel)
						{
    						g_dwTireSqueel = SndQueueSpeech(SND_TIRE_SQUEEL, 0 ,g_iBaseBettySoundLevel,2);
    						SndServiceSound();
						}
					}
					else
					{
						PlayPositionalNoGearHit(Camera1,PlayerPlane->WorldPosition);
					}
					UFC.ILSStatus = OFF;
				}

				UFC.TireSqueelOneShot = UFC.TireOneShotEnabled = 0;
			}
		}
#endif

		if(!blow_up)
		{
			if (!PlayerPlane->Brakes)
			{
				PlayerPlane->BfForce.X += (PlayerPlane->LeftThrust + PlayerPlane->RightThrust);
				TouchDownOneShot = 0;
			}
			else
				TouchDownOneShot = ((PlayerPlane->Brakes != BrakesLastFrame) && PlayerPlane->Brakes) ? 1 : 0;
		}
		else
		{
			if (RealisticCrashes)
				CrashPlane(P,PL_STATUS_CRASHED,0);
			else
				PlayerPlane->BfForce.X += (PlayerPlane->LeftThrust + PlayerPlane->RightThrust);
		}
	}
	else
	{
		PlayerPlane->BfForce.X += (PlayerPlane->LeftThrust + PlayerPlane->RightThrust);
		TouchDownOneShot = 0;
	}
}
#else
void SFCheckOnGroundStuff(PlaneParams *P)
{
	if ((P->AGL > 20))
		UFC.TireOneShotEnabled = 1;

	if ((!UFC.OnGroundLastFrame && P->OnGround))
		UFC.TireSqueelOneShot = 1;

	if (P->OnGround)
	{
		BOOL blow_up = FALSE;
		if ((P->OnGround > 1) && (P->TerrainType >= TT_CARRIER_DECK))
		{
			if (!((P->FlightStatus & PL_GEAR_DOWN_LOCKED) || (P->FlightStatus & PL_PLANE_DITCHING)))
				blow_up = TRUE;

			if (!SndIsSoundPlaying(g_dwTireSqueel))
				g_dwTireSqueel=0;

			if (UFC.TireSqueelOneShot && UFC.TireOneShotEnabled)	// when we first touchdown on ground
			{
				blow_up = CheckLandingAttitude(P);

				if (!blow_up)
				{
					if(P->FlightStatus & PL_GEAR_DOWN_LOCKED)
					{
						if (!g_dwTireSqueel)
						{
    						g_dwTireSqueel = SndQueueSpeech(SND_TIRE_SQUEEL, 0 ,g_iBaseBettySoundLevel,2);
    						SndServiceSound();
						}
					}
					else
					{
						PlayPositionalNoGearHit(Camera1,P->WorldPosition);
					}
					UFC.ILSStatus = OFF;
				}

				UFC.TireSqueelOneShot = UFC.TireOneShotEnabled = 0;
			}

		}
		else
		{
			P->TerrainType = GetGroundType(P->WorldPosition,&P->TerrainType1,&P->TerrainType2);

			if(P->FlightStatus & PL_PLANE_DITCHING)
			{
				blow_up = FALSE;
			}
			else if (P->TerrainType == TT_RUNWAY)
			{
				if (!((P->FlightStatus & PL_GEAR_DOWN_LOCKED) || (P->FlightStatus & PL_PLANE_DITCHING)))
					blow_up = TRUE;

				if (!SndIsSoundPlaying(g_dwTireSqueel))
					g_dwTireSqueel=0;

				if (UFC.TireSqueelOneShot && UFC.TireOneShotEnabled)	// when we first touchdown on ground
				{
					blow_up = CheckLandingAttitude(P);

					if (!blow_up)
					{
						if(P->FlightStatus & PL_GEAR_DOWN_LOCKED)
						{
							if (!g_dwTireSqueel)
							{
    							g_dwTireSqueel = SndQueueSpeech(SND_TIRE_SQUEEL, 0 ,g_iBaseBettySoundLevel,2);
    							SndServiceSound();
							}
						}
						else
						{
							PlayPositionalNoGearHit(Camera1,P->WorldPosition);
						}
						UFC.ILSStatus = OFF;
					}

					UFC.TireSqueelOneShot = UFC.TireOneShotEnabled = 0;
				}
			}
			else if (!(UFC.TireOneShotEnabled))
			{
				if(!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
				{
					blow_up = TRUE;
				}
				if (P->TerrainType == TT_AIRPORT)
				{
					if ((P->Knots > 300) || (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED)))
						blow_up = TRUE;
				}
				else
					if ((P->TerrainType == TT_DESTROYED_RUNWAY) || (P->TerrainType == TT_LAND))
					{
						if (P->Knots > 48)
						{
							if((P->Knots > 120) || (MultiPlayer))
							{
								blow_up = TRUE;
							}
							else
							{
								SetUpDitch(P);
							}
						}
					}
					if (P->TerrainType == TT_WATER)
						blow_up = TRUE;
			}
			else
			{
				if ((P->TerrainType == TT_AIRPORT) && (!MultiPlayer))
				{
					blow_up = CheckLandingAttitude(P);
				}
				else
				{
					blow_up = TRUE;
				}

				if ((!blow_up) && (!MultiPlayer))
				{
					SetUpDitch(P);
					if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
					{
						if (!g_dwTireSqueel)
						{
    						g_dwTireSqueel = SndQueueSpeech(SND_TIRE_SQUEEL, 0 ,g_iBaseBettySoundLevel,2);
    						SndServiceSound();
						}
					}
					else
					{
						PlayPositionalNoGearHit(Camera1,P->WorldPosition);
					}
					UFC.ILSStatus = OFF;
				}

				UFC.TireSqueelOneShot = UFC.TireOneShotEnabled = 0;
			}
		}

		if(!blow_up)
		{
			if (!P->Brakes)
			{
				P->BfForce.X += (P->LeftThrust + P->RightThrust);
				TouchDownOneShot = 0;
			}
			else
				TouchDownOneShot = ((P->Brakes != BrakesLastFrame) && P->Brakes) ? 1 : 0;
		}
		else
		{
			if (RealisticCrashes)
				CrashPlane(P,PL_STATUS_CRASHED,0);
			else
				P->BfForce.X += (P->LeftThrust + P->RightThrust);
		}
	}
	else
	{
		P->BfForce.X += (P->LeftThrust + P->RightThrust);
		TouchDownOneShot = 0;
	}
}
#endif