#include "F18.h"
#include "spchcat.h"
#include "gamesettings.h"
#include "keystuff.h"
#include "particle.h"

#include "autoplt.h"
#include "engine.h"
#include "flightfx.h"
#include "aero.h"

#include "snddefs.h"
#include "seatstf.h"
void KillSound( DWORD *SndHandle );
extern void PlayPositionalNoGearHit(CameraInstance &camera,FPointDouble &SoundPosition);

//************************************************************************************************
void StartWingConTrails(PlaneParams *P)
{
	if (!P->Smoke[SMOKE_WING_LEFT])
		P->Smoke[SMOKE_WING_LEFT] = SmokeTrails->NewSmokeTrail(P,OFFSET_WING_LEFT,WHITE_WINGTIP_VAPOR);
	if (!P->Smoke[SMOKE_WING_RIGHT])
		P->Smoke[SMOKE_WING_RIGHT] = SmokeTrails->NewSmokeTrail(P,OFFSET_WING_RIGHT,WHITE_WINGTIP_VAPOR);
}
//************************************************************************************************
void StopWingConTrails(PlaneParams *P)
{
	if (P->Smoke[SMOKE_WING_LEFT])
	{
		((SmokeTrail *)P->Smoke[SMOKE_WING_LEFT])->MakeOrphan();
		P->Smoke[SMOKE_WING_LEFT] = NULL;
	}

	if (P->Smoke[SMOKE_WING_RIGHT])
	{
		((SmokeTrail *)P->Smoke[SMOKE_WING_RIGHT])->MakeOrphan();
		P->Smoke[SMOKE_WING_RIGHT] = NULL;
	}
}
//************************************************************************************************

// this is just the high number, not actually the max sustainable
#define G_MAX_SUSTAINABLE	12.0f

//use only positive values.  So -1.5 G would be 1.5
#define G_MIN_SUSTAINABLE   2.0f

//in ticks
#define G_TIME_TO_REGEN		(float)(3*50)

//in ticks
#define G_TIME_TO_DEPLETE	(float)(6*50)

//slerp rate bigger is faster
#define G_SLERP_RATE         (1.0f/100.0f)

#define G_POINT_TO_STOP_REGEN 4.0f


void CalcPilotGLoading(PlaneParams *P)
{

	float TmpGVal;
	float absTmpGVal;

	if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & (PL_STATUS_CRASHED | PL_PLANE_BLOWN_UP)))
		TmpGVal = P->GForce;
	else
		TmpGVal = 0.0;

	if (TmpGVal < 0.0)
		TmpGVal *= (G_MAX_SUSTAINABLE/G_MIN_SUSTAINABLE);

	absTmpGVal = fabs(TmpGVal);

	if (absTmpGVal < G_POINT_TO_STOP_REGEN)
	{
		P->PilotGEffortRemaining += G_MAX_SUSTAINABLE/G_TIME_TO_REGEN;
		if (P->PilotGEffortRemaining > G_MAX_SUSTAINABLE)
			P->PilotGEffortRemaining = G_MAX_SUSTAINABLE;
	}
	else
	{
	 	P->PilotGEffortRemaining -= absTmpGVal *  (1.0f/G_TIME_TO_DEPLETE);

		if (P->PilotGEffortRemaining < 0.0f)
			P->PilotGEffortRemaining = 0.0f;
	}

	P->PilotGThreshold = absTmpGVal - P->PilotGEffortRemaining;
	if (P->PilotGThreshold < 0.0)
		P->PilotGThreshold = 0.0;

	if (TmpGVal < 0.0)
		P->PilotGThreshold = -P->PilotGThreshold;

	P->PilotGLoad += (P->PilotGThreshold - P->PilotGLoad)*G_SLERP_RATE;

}
//************************************************************************************************
void DoYawRelatedStuff(PlaneParams *P)
{
#if 0
	if (P->Status & PL_DEVICE_DRIVEN)
	{
		if ((RadToDeg(fabs(P->BfRotVel.Z)) > 30.0) && (P->AGL >= 10))
		{
			if (!g_dwYawRate)
			{
				g_dwYawRate = SndQueueSound(SIMSND_YAWRATE, 2,g_iBaseBettySoundLevel);
				SndServiceSound();
			}
			else
				if (!SndIsSoundPlaying(g_dwYawRate))
					g_dwYawRate=0;
		}
		if (!SndIsSoundPlaying(g_dwYawRate) && g_dwYawRate)
			g_dwYawRate=0;
	}
#endif
}
//************************************************************************************************
void DoGRelatedStuff(PlaneParams *P)
{
	static DWORD s_dwToneWarning = 0;
	float new_gs;
	float vapor_amnt;

	// Calculate G Force
	if (P->TotalWeight == 0.0) P->TotalWeight = 40000.0;		// for dying AI planes
	new_gs = (P->OnGround) ? 1.0 : (float)(-P->BfForce.Z/P->TotalWeight);
	P->PilotGDelta = new_gs - P->GForce;
	P->GForce = new_gs;

	if (P->GForce > UFC.StatusMaxG)
		UFC.StatusMaxG = P->GForce;

	if (P->GForce < UFC.StatusMinG)
		UFC.StatusMinG = P->GForce;

	if ((P->Status & PL_DEVICE_DRIVEN))
	{
		if (P->AvailGForce == 0.0) P->AvailGForce = 9.0;		// for dying AI planes
		float PercentG = P->GForce/P->AvailGForce;

		if (RedBlackOut)
			CalcPilotGLoading(P);

		if (!P->OnGround)
		{
			vapor_amnt = (float)fabs(P->WorldPosition.Y - (WorldParams.CloudAlt+1000.0 FEET));

			vapor_amnt /=  4000.0f - (2000.0f*(float)(WorldParams.Weather & WR_VIS) FEET);

			vapor_amnt = 0.4 * vapor_amnt;

			if (PercentG > (0.60f + vapor_amnt))
			{
				P->VaporAmnt = (PercentG - vapor_amnt)*(1.0f/0.4f);
				if (P->VaporAmnt > 1.0f)
					P->VaporAmnt = 1.0f;
				StartWingConTrails(P);
			}
			else
			{
				P->VaporAmnt = 0;
				StopWingConTrails(P);
			}
		}
		else
		{
			StopWingConTrails(P);
			P->VaporAmnt = 0;
		}



//		if (!EasyFlight)
//		{
			if (P->GForce > P->AvailGForce) // Check for overG Warning
			{
				if ((!g_dwOverGBitch) && (!(P->FlightStatus & PL_OUT_OF_CONTROL)))
				{
//					g_dwOverGBitch = SndQueueSpeech(SIMSND_OVERG, CHANNEL_BETTY,g_iBaseBettySoundLevel,2);
//					SndServiceSound();
				}

				if (s_dwToneWarning)
				{
					SndEndSound (s_dwToneWarning);
					s_dwToneWarning = 0;
				}
			}
//			else if (PercentG >= 0.85)	 // f-15
			else if (PercentG >= 0.95)
			{
//				if ((!s_dwToneWarning) && (!(P->FlightStatus & PL_OUT_OF_CONTROL)))
//				{
//					int iLevel=(g_iSoundLevelCaution-20)/2;
//					if (iLevel<0) iLevel=0;
//					s_dwToneWarning = SndQueueSound(SIMSND_900HZ4, 0,iLevel);
//					SndServiceSound();
//				}
//				if (!SndIsSoundPlaying(g_dwOverGBitch))
//				{
//					g_dwOverGBitch=0;
//				}
			}
			else
			{
//				if (!SndIsSoundPlaying(g_dwOverGBitch))
//				{
//					g_dwOverGBitch=0;
//				}

				if (s_dwToneWarning)
				{
					SndEndSound (s_dwToneWarning);
					s_dwToneWarning = 0;
				}

			}
//		}
	}
}
//************************************************************************************************
int CheckForDeadlySinkRate(PlaneParams *P)
{

	int WeightClip = (int)P->TotalWeight;

	if (WeightClip > 90000) WeightClip = 90000;
	if (WeightClip < 0) WeightClip = 0;         // probably unnecessary but just to be safe

	int MinIndex,MaxIndex;
	float Frac;

	// calc min and max indexes for use in interpolation
	if (WeightClip == 90000) // end of table
	{
		MaxIndex = 18;
		MinIndex = 17;
		Frac = 1.0f;
	}
	else
	{
		float IndexInFloat = (float)WeightClip/5000.0f;

		MinIndex = (int)floor(IndexInFloat);
		MaxIndex = MinIndex+1;
		Frac = IndexInFloat - floor(IndexInFloat);
	}

	// now interpolate the table using the 2 indexes and Frac

	float Min = AllowableSinkRateTable[MinIndex];   // table is in ft/min, IfVel is in ft/sec
	float Max = AllowableSinkRateTable[MaxIndex];   // table is in ft/min, IfVel is in ft/sec


	float TableVal = -(Min + ((Max - Min)*Frac));
	float IfVel = (P->IfVelocity.Y * (60.0));

	if (IfVel < (TableVal*1.33)) // 1.33 is an adjustment factor table is 1.5, needs to be 2.0
		return (TRUE);
	else
		if (IfVel < TableVal)
		{
			if(!MultiPlayer)
			{
				SetUpDitch(P);

				// sound goes here
				return(FALSE);
			}
			return(TRUE);
		}
		else
			return(FALSE);
}
//************************************************************************************************
int CheckLandingAttitude(PlaneParams *P)
{
	int RetVal = 0;			// good no blow up state is default

	int TmpRoll = P->Roll/DEGREE;
	int TmpPitch = P->Pitch/DEGREE;

	if (RealisticLandings)
	{

		int PitchRollBad = (((TmpRoll > 10) && (TmpRoll < 350)) ||	// +- 10 degree roll
 			((TmpPitch > 15) && (TmpPitch < 358)));	                // between 0-12 deg. pitch up

		int BadSinkRate = CheckForDeadlySinkRate(P);

		if(PitchRollBad || BadSinkRate)
			RetVal = 1;		// blowup
	}
	else
	{
		if (((TmpRoll > 45) && (TmpRoll < 315)) ||		// +- 45 degree roll/pitch
 			((TmpPitch > 45) && (TmpPitch < 315)))

			RetVal = 1;		// blowup
	}
	return (RetVal);
}
//************************************************************************************************
float GetGroundSurfaceDragCoefficient(PlaneParams *P)
{
	float RetVal = 0.0f;

	switch (P->TerrainType)
	{
		case TT_LAND:
		case TT_DESTROYED_RUNWAY:
				RetVal = 0.5f;
			break;
		case TT_AIRPORT:
				RetVal = 0.9f;
			break;
	}
	return (RetVal);
}
//************************************************************************************************
void ToggleFlaps(void)
{

	if ((PlayerPlane->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_R_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_EMER_HYD))
		return;

	switch (PlayerPlane->FlapsMode)
	{
		case FLAPS_FULL :
			PlayerPlane->FlapsMode = FLAPS_HALF;
		  SetCockpitItemState(125,1);
			break;
		case FLAPS_HALF :
			PlayerPlane->FlapsMode = FLAPS_AUTO;
 		  SetCockpitItemState(125,0);
			break;
		case FLAPS_AUTO :
			PlayerPlane->FlapsMode = FLAPS_FULL;
		  SetCockpitItemState(125,2);
			break;
	}
}
//************************************************************************************************
void ToggleSpeedBrake(void)
{
	if ((PlayerPlane->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_R_MAIN_HYD))
		return;

//	if (PlayerPlane->OnGround)
//		PlayerPlane->SpeedBrakeState = 0;
//	else
//	{
		PlayerPlane->SpeedBrakeState ^= 1;

		if (PlayerPlane->SpeedBrakeState)
		{
			int iLevel=g_iSoundLevelEngineRum-40;
			if (iLevel<0) iLevel=0;
			if(iLevel)
			{
				if((dwSpeedBrakeSnd) && (SndIsSoundPlaying(dwSpeedBrakeSnd)))
				{
					KillSound(&dwSpeedBrakeSnd);
				}

				dwSpeedBrakeSnd = SndQueueSound(SND_FLAPS,1,iLevel);
			}

			DisplayWarning(SPDB, ON,  1);
		}
		else
		{
			DisplayWarning(SPDB, OFF, 1);
			int iLevel=g_iSoundLevelEngineRum-50;
			if (iLevel<0) iLevel=0;

			if(iLevel)
			{
				if((dwSpeedBrakeSnd) && (SndIsSoundPlaying(dwSpeedBrakeSnd)))
				{
					KillSound(&dwSpeedBrakeSnd);
				}

				dwSpeedBrakeSnd = SndQueueSound(SND_FLAPS,1,iLevel);
			}
		}
//	}
}
//************************************************************************************************
void CheckEasyFlightStuff(PlaneParams *P)
{
	if ((P->IndicatedAirSpeed > 240) && (P->AGL > 100))
	{
		// Put Gear up automatically
		if (!P->LandingGearStatus)
		{
			if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
			{
				RaiseLowerLandingGear(P,RAISE_LANDING_GEAR);
				DisplayWarning(GEAR, OFF, 1);

				int iLevel=g_iSoundLevelEngineRum-10;
				if (iLevel<0) iLevel=0;
				SndQueueSound(Gearupi,1,iLevel);
			}
		}

//			Put flaps up automatically
//			if (P->Flaps != 0.0)
//			{
//			P->FlapsCommandedPos = 0.0f;
//
//			if (P->Status & PL_AI_DRIVEN)
//				P->Flaps = 0.0;
//
//			int iLevel=g_iSoundLevelCaution-50;
//			if (iLevel<0) iLevel=0;
//			SndQueueSound(Flapmvi,1,iLevel);
//			DisplayWarning(FLAPS, OFF, 1);
//		}
	}
}


//************************************************************************************************
void ToggleGear(void)
{

	if ((PlayerPlane->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_R_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_EMER_HYD))
		return;

	if (PlayerPlane->OnGround) return;

	if (!PlayerPlane->LandingGearStatus)
	{
		if(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			if(!PlayerPlane->OnGround)
			{
				RaiseLowerLandingGear(PlayerPlane,RAISE_LANDING_GEAR);
				DisplayWarning(GEAR, OFF, 1);

				int iLevel=g_iSoundLevelEngineRum-10;
				if (iLevel<0) iLevel=0;
//				SndQueueSound(SND_GEARUP,1,iLevel);
				AICAddSoundCall(AICDelayedSoundEffect, SND_GEARUP, 1200, 50, iLevel);
			}
		}
		else
		{
			if (PlayerPlane->IndicatedAirSpeed <= 285)
			{
				RaiseLowerLandingGear(PlayerPlane,LOWER_LANDING_GEAR);
				DisplayWarning(GEAR, ON, 1);
				int iLevel=g_iSoundLevelEngineRum-10;
				if (iLevel<0) iLevel=0;
//				SndQueueSound(SND_GEARDOWN,1,iLevel);
				AICAddSoundCall(AICDelayedSoundEffect, SND_GEARDOWN, 1300, 50, iLevel);
				TimeExcel = 0;
			}
		}
	}
}
//************************************************************************************************
void MoveLandingGear(PlaneParams* P)
{
	int loops;

	if (P->LandingGearStatus)
	{
		loops = GameLoop - P->LandingGearLoop;

		switch (P->LandingGearStatus)
		{
			case PL_LG_REQUEST_OPEN:
				P->LandingGearLoop = GameLoop;
				P->LandingGear = 0x1;
				P->LandingGearStatus = PL_LG_STATE1;
				break;

			case PL_LG_REQUEST_CLOSED:
				P->LandingGearLoop = GameLoop;
				P->LandingGear = 0xFE;
				P->LandingGearStatus = PL_LG_STATE4;
				P->FlightStatus &= ~PL_GEAR_DOWN_LOCKED;
				break;

			case PL_LG_STATE3:	//Gear finishes opening
				if (loops > 50)
				{
					P->LandingGear = 0xFF;
					P->FlightStatus |= PL_GEAR_DOWN_LOCKED;
					P->LandingGearStatus = 0;
				}
				break;

			case PL_LG_STATE6:  //Gear finishes closing
				if (loops > 50)
				{
					P->LandingGear = 0;
					P->LandingGearStatus = 0;
				}
				break;

			case PL_LG_STATE1:  //Each of these waits for one second
			case PL_LG_STATE4:
				if (loops > 50)
				{
					P->LandingGearStatus++;
					P->LandingGearLoop = GameLoop - loops+50;
				}
				break;

			case PL_LG_STATE2:
				if (loops <= 100)
					P->LandingGear = (BYTE)((float)(loops)*((float)0xFF/100.0f));
				else
				{
					P->LandingGearStatus = PL_LG_STATE3;
					P->LandingGearLoop = GameLoop - loops+100;
				}
				break;

			case PL_LG_STATE5:
				if (loops <= 100)
					P->LandingGear = (BYTE)((float)(100-loops)*((float)0xFF/100.0f));
				else
				{
					P->LandingGearStatus = PL_LG_STATE6;
					P->LandingGearLoop = GameLoop - loops+100;
				}
				if (!P->LandingGear)
					P->LandingGear = 1;
				break;
		}
	}
}
//************************************************************************************************
void SetLandingGearUpDown(PlaneParams* P,int gear_down)
{
	P->LandingGearStatus = 0;
	if (gear_down)
	{
		P->FlightStatus |= PL_GEAR_DOWN_LOCKED;
		P->LandingGear = 0xFF;
	}
	else
	{
		P->FlightStatus &= ~PL_GEAR_DOWN_LOCKED;
		P->LandingGear = 0;
	}
}
//************************************************************************************************
void RaiseLowerLandingGear(PlaneParams* P,int gear_down)
{
	if (!P->LandingGearStatus)
	{
		if (gear_down)
		{
			if (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
			{
				P->LandingGearStatus = PL_LG_REQUEST_OPEN;
				SetCockpitItemState(253,1);
			}
		}
		else
		{
			if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
			{
				P->LandingGearStatus = PL_LG_REQUEST_CLOSED;
				SetCockpitItemState(253,0);
			}
		}
	}
}


//************************************************************************************************

void DeployTowedDecoy(PlaneParams *P)
{
	P->TowedDecoyState |= TOWED_WANT_DEPLOYED;
	P->TowedDecoyCommandedPosition = 1.0f;
}

void RetractTowedDecoy(PlaneParams *P)
{
	if ((P->TowedDecoyState & (TOWED_WANT_DEPLOYED | TOWED_DANGLING)) == TOWED_WANT_DEPLOYED)
	{
		P->TowedDecoyState &= ~TOWED_WANT_DEPLOYED;
		P->TowedDecoyCommandedPosition = 0.0f;
	}
}

void ClearTowedDecoySettings(PlaneParams *P)
{
	P->TowedDecoyState = 0;
	P->TowedDecoyCommandedPosition = 0.0f;
	P->TowedDecoyPosition = 0.0f;
}

void CutTowedDecoyCable(PlaneParams *P)
{
	if (P->TowedDecoyState & TOWED_WANT_DEPLOYED)
	{
		ClearTowedDecoySettings(P);

		/* -----------------7/12/99 5:52PM---------------------------------------------------------------------
		/* calls to graphics functions to drop the decoy like a bomb should go here
		/* ----------------------------------------------------------------------------------------------------*/

	}
}


//************************************************************************************************

void DeployFuelHose(PlaneParams *P,int number)
{
	if (number < MAX_REFUELING_HOSES)
	{
		P->RefuelingHoses[number].Status |= RH_WANT_EXTEND;
		P->RefuelingHoses[number].CommandedPosition = 1.0f;
	}
}

void RetractFuelHose(PlaneParams *P,int number)
{
	if (number < MAX_REFUELING_HOSES)
	{
		P->RefuelingHoses[number].Status &= ~(RH_WANT_EXTEND | RH_CONNECTED);
		P->RefuelingHoses[number].CommandedPosition = 0.0f;
		P->RefuelingHoses[number].ConnectedPlane = 0;
	}
}

void ClearFuelHoseSettings(PlaneParams *P,int number)
{
	if (number < MAX_REFUELING_HOSES)
	{
		P->RefuelingHoses[number].Status = 0;
		P->RefuelingHoses[number].CommandedPosition = 0.0f;
		P->RefuelingHoses[number].Position = 0.0f;
		P->RefuelingHoses[number].ConnectedPlane = 0;
	}
}

void RemovePlaneFromRefuelingHose(PlaneParams *P,int number)
{
 	if ((number < MAX_REFUELING_HOSES) && (P->RefuelingHoses[number].Status & RH_WANT_EXTEND))
	{
		P->RefuelingHoses[number].Status &= ~RH_CONNECTED;
		P->RefuelingHoses[number].ConnectedPlane = 0;
	}
}

void AttachPlanePointToRefuelingHose(PlaneParams *P,DWORD plane,int number)
{
	if ((number < MAX_REFUELING_HOSES) && (P->RefuelingHoses[number].Status & RH_WANT_EXTEND))
	{
		FPointDouble new_point;
		double max_dist;
		PlaneParams *new_plane = (PlaneParams *)plane;

		new_point = new_plane->WorldPosition;

		new_point.AddScaledVector(-pDBAircraftList[new_plane->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X FEET,new_plane->Orientation.J);
		new_point.AddScaledVector(pDBAircraftList[new_plane->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y FEET,new_plane->Orientation.K);
		new_point.AddScaledVector(pDBAircraftList[new_plane->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z FEET,new_plane->Orientation.I);

		new_point.MakeVectorToFrom(new_point,P->WorldPosition);
		new_point -= P->RefuelingHoses[number].ComputedLocation;

		if ((P->RefuelingHoses[number].Status & RH_CONNECTED) && (P->RefuelingHoses[number].ConnectedPlane == plane))
		{
			if (P->RefuelingHoses[number].ConnectedPlane == (DWORD)PlayerPlane)
			{
				/* -----------------7/16/99 10:55AM--------------------------------------------------------------------
				/* adjust for difficulty
				/* ----------------------------------------------------------------------------------------------------*/
				if(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL)
				{
					max_dist = 20 FEET;
				}
				else
				{
					max_dist = 80 FEET;
				}
			}
			else
				max_dist = 10 FEET;
		}
		else
		{
			if (new_plane == PlayerPlane)
			{
				/* -----------------7/16/99 10:55AM--------------------------------------------------------------------
				/* adjust for difficulty
				/* ----------------------------------------------------------------------------------------------------*/
				if(g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL)
				{
					max_dist = 20 FEET;
				}
				else
				{
					max_dist = 80 FEET;
				}
			}
			else
				max_dist = 10 FEET;
		}

		if((MultiPlayer) && (!((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane))))
		{
			if((P->AI.Behaviorfunc != AITankerDisconnect) && (P->AI.Behaviorfunc != AITankerDisconnectAI))
			{
//				max_dist *= 2.0f;
				max_dist = 200 FEET;
			}
		}

		if (new_point.PartialLength() > max_dist*max_dist) //15 ft squared cuz PartialLength = Length()^2
		{
			P->RefuelingHoses[number].Status &= ~RH_CONNECTED;
			P->RefuelingHoses[number].ConnectedPlane = NULL;

			if(pSeatData && ((new_plane == PlayerPlane) || (P == PlayerPlane)))
			{
				pSeatData->ProbeConnection = (PlayerPlane->RefuelProbeCommandedPos > 10.0f) ? 1 : 0;
			}

			if((MultiPlayer) && ((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane) || (new_plane == PlayerPlane)))
			{
				NetPutGenericMessage2(P, GM2_ATTACH_HOSE, 255);
			}
		}
		else
		{
			if(pSeatData && ((new_plane == PlayerPlane) || (P == PlayerPlane)))
			{
				pSeatData->ProbeConnection = 2;
			}

			P->RefuelingHoses[number].Status |= RH_CONNECTED;
			P->RefuelingHoses[number].ConnectedPosition = 1.0f;
			P->RefuelingHoses[number].ConnectedPlane = plane;
			P->RefuelingHoses[number].OffsetToConnectedPlane = new_point;
		}
	}
}

//************************************************************************************************
void DoSmoke(PlaneParams *P)
{
	float alphas[MAX_PLANE_SMOKE_TRAILS];
	PlaneEngineFire *walker;
	void **swalker,**last_smoke_trail;
	FPointDouble fire_location;
	FPoint *offset;
	int i;

	i = 5;
	while(i--)
	{
//		if (P->Smoke[i] && (*((SmokeTrail *)P->Smoke[i]) == ENGINE_FIRE))
//			alphas[i] = 0.25 + frand()*0.75f;
//		else
			alphas[i] = 1.0f;
	}

	if (pDBAircraftList[P->AI.iPlaneIndex].iFuelFlow) //this actually means if it's smoking or not
	{
		/* -----------------8/9/99 4:39PM----------------------------------------------------------------------
		/*
		/* ----------------------------------------------------------------------------------------------------*/

		float throttle[5];
		int   offset[5];
		int   smokepos[5];
		int num_engines;
		float *twalk;
		int *owalk;
		int *swalk;

		switch(P->Type->EngineConfig)
		{
			case TWO_REAR_ENGINES:
				num_engines = 2;
				throttle[0] = P->LeftThrustPercent;
				throttle[1] = P->RightThrustPercent;
				offset[0] = OFFSET_ENGINE_REAR_LEFT;
				offset[1] = OFFSET_ENGINE_REAR_RIGHT;
				smokepos[0] = SMOKE_ENGINE_REAR_LEFT;
				smokepos[1] = SMOKE_ENGINE_REAR_RIGHT;
				break;

			case ONE_REAR_ENGINE:
				num_engines = 1;
				throttle[0] = P->LeftThrustPercent;
				offset[0] = OFFSET_ENGINE_REAR;
				smokepos[0] = SMOKE_ENGINE_REAR;
				break;

			case TWO_WING_ENGINES:
				num_engines = 2;
				throttle[0] = P->LeftThrustPercent;
				throttle[1] = P->RightThrustPercent;
				offset[0] = OFFSET_ENGINE_WING_LEFT_INBOARD;
				offset[1] = OFFSET_ENGINE_WING_RIGHT_INBOARD;
				smokepos[0] = SMOKE_ENGINE_WING_LEFT_INBOARD;
				smokepos[1] = SMOKE_ENGINE_WING_RIGHT_INBOARD;
				break;

			case FOUR_WING_ENGINES:
				num_engines = 4;
				throttle[0] = P->LeftThrustPercent;
				throttle[1] = P->LeftThrustPercent;
				throttle[2] = P->RightThrustPercent;
				throttle[3] = P->RightThrustPercent;
				offset[0] = OFFSET_ENGINE_WING_LEFT_OUTBOARD;
				offset[1] = OFFSET_ENGINE_WING_LEFT_INBOARD;
				offset[2] = OFFSET_ENGINE_WING_RIGHT_OUTBOARD;
				offset[3] = OFFSET_ENGINE_WING_RIGHT_INBOARD;
				smokepos[0] = SMOKE_ENGINE_WING_LEFT_OUTBOARD;
				smokepos[1] = SMOKE_ENGINE_WING_LEFT_INBOARD;
				smokepos[2] = SMOKE_ENGINE_WING_RIGHT_OUTBOARD;
				smokepos[3] = SMOKE_ENGINE_WING_RIGHT_INBOARD;

				break;

		}


		twalk = throttle;
		owalk = offset;
		swalk = smokepos;

		if (pDBAircraftList[P->AI.iPlaneIndex].iAfterBurn)
		{
			while(num_engines--)
			{
				if (!P->Smoke[*swalk] || (((SmokeTrail *)P->Smoke[*swalk])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
					if ((*twalk > 80.0f) || (!*twalk))
					{
  						if (P->Smoke[*swalk])
						{
							((SmokeTrail *)P->Smoke[*swalk])->Update(0.0f);
							((SmokeTrail *)P->Smoke[*swalk])->MakeOrphan();
							P->Smoke[*swalk] = NULL;
						}
				 		alphas[*swalk] = 0.0;
					}
					else
					{
						if (!P->Smoke[*swalk])
							P->Smoke[*swalk] = SmokeTrails->NewSmokeTrail(P,*owalk,BROWN_ENGINE_SMOKE);
				 		alphas[*swalk] = *twalk * (0.7f/80.0f)+ 0.3f*frand();
					}

				twalk++;
				owalk++;
				swalk++;
			}
		}
		else
		{
			while(num_engines--)
			{
				if (!P->Smoke[*swalk] || (((SmokeTrail *)P->Smoke[*swalk])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
					if (!*twalk)
					{
						if (P->Smoke[*swalk])
						{
							((SmokeTrail *)P->Smoke[*swalk])->Update(0.0f);
							((SmokeTrail *)P->Smoke[*swalk])->MakeOrphan();
							P->Smoke[*swalk] = NULL;
						}
			  			alphas[*swalk] = 0.0;
					}
					else
					{
						if (!P->Smoke[*swalk])
							P->Smoke[*swalk] = SmokeTrails->NewSmokeTrail(P,*owalk,BROWN_ENGINE_SMOKE);
						alphas[*swalk] = *twalk * (0.7f/100.0f) + 0.3f*frand();
					}

				twalk++;
				owalk++;
				swalk++;
			}
		}
	}

	/* -----------------8/9/99 4:39PM----------------------------------------------------------------------
	/*
	/* ----------------------------------------------------------------------------------------------------*/

	swalker  = &P->Smoke[0];
	walker = &P->EngineFires[0];
	last_smoke_trail = swalker + MAX_PLANE_SMOKE_TRAILS;
	i = 0;

	while(swalker < last_smoke_trail)
	{
		if (*swalker)
		{
			if (i < SMOKE_WING_LEFT)
				((SmokeTrail *)*swalker)->Update(alphas[i]);
			else
				if ((i < SMOKE_LEFT_FUEL_DUMP) || (i > SMOKE_RIGHT_FUEL_DUMP))
					((SmokeTrail *)*swalker)->Update();
				else
					((SmokeTrail *)*swalker)->Update(0.5 + frand()*0.5);


			if (walker->TotalTimeLeft)
			{
				walker->TotalTimeLeft--;
				if (!(walker->LoopsTillNextSprite--))
				{
					walker->LoopsTillNextSprite = 4-(CurrentTick&3);
					offset = &P->Type->Offsets[((SmokeTrail *)*swalker)->Flags&ST_OFFSET];
					fire_location.X = P->WorldPosition.X + P->Orientation.I.X*(double)offset->Z + P->Orientation.J.X*(double)offset->X + P->Orientation.K.X*(double)offset->Y;
					fire_location.Y = P->WorldPosition.Y + P->Orientation.I.Y*(double)offset->Z + P->Orientation.J.Y*(double)offset->X + P->Orientation.K.Y*(double)offset->Y;
					fire_location.Z = P->WorldPosition.Z + P->Orientation.I.Z*(double)offset->Z + P->Orientation.J.Z*(double)offset->X + P->Orientation.K.Z*(double)offset->Y;

					NewSprite(&SpriteTypes[PLANE_BURNS],fire_location,GameLoop);
				}
			}
			else
				if ((*(SmokeTrail **)swalker)->Light)
				{
					RemoveLight((LightSource **)(*(SmokeTrail **)swalker)->Light);
					(*(SmokeTrail **)swalker)->Light = NULL;
				}

		}
		i++;
		swalker++;
		walker++;
	}
}
//************************************************************************************************
void CheckReportingStats(PlaneParams *P)
{
	if (!UFC.TakeOffTime && ((P->HeightAboveGround*WUTOFT) > 50))
	{
		UFC.TakeOffTime = CurrentTick;
		LogMissionStatistic(LOG_FLIGHT_TAKEOFF_TIME,(P-Planes),GameLoop,0);
	}

	if ((UFC.TakeOffTime) && (P->OnGround) && (P->Knots < 3) &&
	    !(P->FlightStatus & PL_STATUS_CRASHED))
	{
		LogMissionStatistic(LOG_FLIGHT_LANDING_TIME,(P-Planes),GameLoop,0);

		AIChangeEventFlag(MISSION_FLAG_START + 11);

		int tempfence = AICCheckAllFences((P->WorldPosition.X * WUTOFT), (P->WorldPosition.Z * WUTOFT));

		if (tempfence)
		{
			LogMissionStatistic(LOG_FLIGHT_LANDED_ENEMY,(P-Planes),1,2);
		} else {
			LogMissionStatistic(LOG_FLIGHT_LANDED_FRIENDLY,(P-Planes),1,2);
		}
	}
}
//************************************************************************************************
void UpdateSpeedBrakePosition(PlaneParams *P)
{

	if (P->SpeedBrakeState)
	{
		float PosDeg;

//		if (EasyFlight)
//		{
//			PosDeg = 45.0f;
//		}
//		else
//		{
			if (P->Mach < 1.0)
			{
				if (P->IndicatedAirSpeed < 350)
					PosDeg = 45.0f;
				else
					if (P->IndicatedAirSpeed >= 350 && P->IndicatedAirSpeed <= 600)
						PosDeg = (P->IndicatedAirSpeed*-0.14) + 94.0;
				else
					PosDeg = 1.0f;
			}
			else
				PosDeg = 0.0f;
//		}

		P->SpeedBrakeCommandedPos = PosDeg;
	}
	else
	{
		P->SpeedBrakeCommandedPos = 0.0f;
	}
}
//************************************************************************************************
void KeyThrottleOff(void)
{
//	PlayerPlane->LastKeyboardThrottle = PlayerPlane->ThrottlePos = 0;
//	PlayerPlane->KeyboardAB = 0;
}
//************************************************************************************************
void KeyThrottleIdle(void)
{
	  PlayerPlane->LastKeyboardThrottle = PlayerPlane->CommandedThrottlePos = 0.0;
	  PlayerPlane->KeyboardAB = 0;

	  PlayerPlane->LeftCommandedThrottlePos  = PlayerPlane->CommandedThrottlePos;
	  PlayerPlane->RightCommandedThrottlePos = PlayerPlane->CommandedThrottlePos;
}
//************************************************************************************************
void KeyThrottleMil(void)
{
	PlayerPlane->LastKeyboardThrottle = PlayerPlane->CommandedThrottlePos = 80.0;
	PlayerPlane->KeyboardAB = 0;

	PlayerPlane->LeftCommandedThrottlePos  = PlayerPlane->CommandedThrottlePos;
	PlayerPlane->RightCommandedThrottlePos = PlayerPlane->CommandedThrottlePos;
}
//************************************************************************************************
void KeyThrottleAB(void)
{
	PlayerPlane->KeyboardAB ^= 1;

	if (PlayerPlane->KeyboardAB)
		PlayerPlane->CommandedThrottlePos = 100.0;
	else
		PlayerPlane->CommandedThrottlePos = 80.0;

    PlayerPlane->LeftCommandedThrottlePos  = PlayerPlane->CommandedThrottlePos;
	PlayerPlane->RightCommandedThrottlePos = PlayerPlane->CommandedThrottlePos;
}
//************************************************************************************************
void KeyThrottleUp(void)
{
    PlayerPlane->CommandedThrottlePos += 5;

	int MaxVal = (PlayerPlane->KeyboardAB) ? 100 : 80;
	if (PlayerPlane->CommandedThrottlePos > MaxVal)
		PlayerPlane->CommandedThrottlePos = MaxVal;

	PlayerPlane->LeftCommandedThrottlePos  = PlayerPlane->CommandedThrottlePos;
	PlayerPlane->RightCommandedThrottlePos = PlayerPlane->CommandedThrottlePos;
}
//************************************************************************************************
void KeyThrottleDown(void)
{
    PlayerPlane->CommandedThrottlePos -= 5;

	if (PlayerPlane->CommandedThrottlePos <= 80)
		PlayerPlane->KeyboardAB = 0;

	if (PlayerPlane->CommandedThrottlePos < 0)
		PlayerPlane->CommandedThrottlePos = 0;

	PlayerPlane->LeftCommandedThrottlePos  = PlayerPlane->CommandedThrottlePos;
	PlayerPlane->RightCommandedThrottlePos = PlayerPlane->CommandedThrottlePos;
}
//************************************************************************************************
void ForceKeyboardThrust(float Value)
{
   	PlayerPlane->LastKeyboardThrottle = PlayerPlane->CommandedThrottlePos = Value;
	PlayerPlane->LeftCommandedThrottlePos  = PlayerPlane->CommandedThrottlePos;
    PlayerPlane->RightCommandedThrottlePos = PlayerPlane->CommandedThrottlePos;
}
//************************************************************************************************
void DefaultTrim(void)
{
	NoseTrimOffset = 0;
	AileronTrimOffset = 0;
	RudderTrimOffset = 0;
}
//************************************************************************************************
void AileronTrimLeft(void)
{
	AileronTrimOffset -= AILERON_OFFSET;
	if (AileronTrimOffset < -8192)
		AileronTrimOffset = -8192;
}
//************************************************************************************************
void AileronTrimRight(void)
{
	AileronTrimOffset += AILERON_OFFSET;
	if (AileronTrimOffset > 8192)
		AileronTrimOffset = 8192;
}
//************************************************************************************************
void RudderTrimLeft(void)
{
	RudderTrimOffset -= RUDDER_OFFSET;
	if (RudderTrimOffset < -8192)
		RudderTrimOffset = -8192;
}
//************************************************************************************************
void RudderTrimRight(void)
{
	RudderTrimOffset += RUDDER_OFFSET;
	if (RudderTrimOffset > 8192)
		RudderTrimOffset = 8192;
}
//************************************************************************************************
void NoseTrimUp(void)
{
	NoseTrimOffset += NOSE_OFFSET;
	if (NoseTrimOffset > 8192)
		NoseTrimOffset = 8192;
}
//************************************************************************************************
void NoseTrimDown(void)
{
	NoseTrimOffset -= NOSE_OFFSET;
	if (NoseTrimOffset < -8192)
		NoseTrimOffset = -8192;
}
//************************************************************************************************
float DoKeyBoardRudderMotion(PlaneParams *P)
{
	#define RDR_NONE 0
	#define RDR_LEFT 1
	#define RDR_RIGHT 2

	static float KeyboardCommandedPos = 0.0f;

	if (!(GetVkStatus(RDRLeft) ^ GetVkStatus(RDRRight)))
		KeyboardCommandedPos = 0.0f;
	else
	{
		float GainValue = (P->AGL < 10) ? 40.0f : 80.0f;

		if (GetVkStatus(RDRLeft))
		{
			if (KeyboardCommandedPos <= 0.0f)
				KeyboardCommandedPos -= GainValue*(float)(DeltaTicks/1000.0f);
			else
				KeyboardCommandedPos = -GainValue*(float)(DeltaTicks/1000.0f);

			if (KeyboardCommandedPos < -114.0)
				KeyboardCommandedPos = -114.0;
		}

		if (GetVkStatus(RDRRight))
		{
			if (KeyboardCommandedPos >= 0.0f)
				KeyboardCommandedPos += GainValue*(float)(DeltaTicks/1000.0f);
			else
				KeyboardCommandedPos = GainValue*(float)(DeltaTicks/1000.0f);

			if (KeyboardCommandedPos > 114.0)
				KeyboardCommandedPos = 114.0;
		}
	}
	return (KeyboardCommandedPos);
}
//************************************************************************************************
void GetF18StickPos(PlaneParams *P)
{
	if (P->Status & PL_DEVICE_DRIVEN)
	{
		int HydrolicGain;

		if ((P->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) &&
			(P->SystemInactive & DAMAGE_BIT_R_MAIN_HYD))
		{
			HydrolicGain = (P->SystemInactive & DAMAGE_BIT_EMER_HYD) ? 2 : 1;
		}
		else
		{
			HydrolicGain = TimeExcel;

			// Time accelration greater than 4x causes our "stick" to go away
			if (HydrolicGain > 2)
				HydrolicGain = 2;
		}

		int Offset = (32768>>HydrolicGain);

		// check here for has stick and if not
		P->StickY = (((GetJoyPosY()>>HydrolicGain)+NoseTrimOffset)-Offset) * (1.0/256.0);

		// don't allow stick to drive us into the ground
		if (P->OnGround && (P->V != 0.0) && (P->StickY < 0))
			P->StickY = 0;

		P->StickX = (((GetJoyPosX()>>HydrolicGain)+AileronTrimOffset)-Offset) * (1.0/256.0);

		if (JoyCap1.wCaps & JOYCAPS_HASZ)
			P->CommandedThrottlePos = (65535-GetJoyZ()) * (100.0/65535.0);	// 0.0 --> 100.0

		if (JoyCap1.wCaps & JOYCAPS_HASR)
			P->RudderPedalsCommandedPos = (((GetJoyPosR()>>HydrolicGain)+RudderTrimOffset)-Offset) * (1.0/256.0);
		else
			P->RudderPedalsCommandedPos = DoKeyBoardRudderMotion(P);

		DetectWeaponFire(P);
	}
	// This needs to be here for Multiplayer
	P->LeftThrustPercent = P->RightThrustPercent = 0.0;

	P->LeftCommandedThrottlePos = P->CommandedThrottlePos;
	P->RightCommandedThrottlePos = P->CommandedThrottlePos;

	if (UFC.LeftSpoolState != SPOOL_OFF)
		P->LeftThrottlePos  = (EMD_lSpin > 62.0) ? ( ((EMD_lSpin - 62.0)/30.0)*100.0) : 0.0;
	if (UFC.RightSpoolState != SPOOL_OFF)
		P->RightThrottlePos = (EMD_rSpin > 62.0) ? ( ((EMD_rSpin - 62.0)/30.0)*100.0) : 0.0;

	P->LeftThrustPercent  = P->LeftThrottlePos;
	P->RightThrustPercent = P->RightThrottlePos;

	P->ThrottlePos = (P->LeftThrottlePos + P->RightThrottlePos)/2.0;

	if (P->SystemInactive & DAMAGE_BIT_L_ENG_CONT)
		if (P->LeftThrustPercent > 65.0)
			P->LeftThrustPercent = 65.0;

	if (P->SystemInactive & DAMAGE_BIT_L_BURNER)
		if (P->LeftThrustPercent > 78.0)
			P->LeftThrustPercent = 78.0;

	if (P->SystemInactive & DAMAGE_BIT_R_ENG_CONT)
		if (P->RightThrustPercent > 65.0)
			P->RightThrustPercent = 65.0;

	if (P->SystemInactive & DAMAGE_BIT_R_BURNER)
		if (P->RightThrustPercent > 78.0)
			P->RightThrustPercent = 78.0;
}
//************************************************************************************************
float CalcHeightAboveGround( PlaneParams *P, MovingVehicleParams *carrier )
{
	// Check for contact with ground, update "OnGround" and return new "HeightAboveGround"
	// clip world position here and do ground collision

	double  clip_height = 0.0;
	double  land_height;
	double  wheel_height;
	float ret_val;

//	if (InHighResTerrainRegion(P->WorldPosition))
//		land_height = LandHeight(P->WorldPosition.X, P->WorldPosition.Z);
//	else
//		land_height = P->LastWorldPosition.Y - P->HeightAboveGround;



	land_height = GetPlaneLandHeightWithCarriers(P,&carrier,&clip_height);

	//gk added below
	 if (!RealisticCrashes)
		 clip_height = __max(P->Type->GearDownHeight,clip_height);

	P->HeightForGear = (float)(clip_height + P->Type->GearDownHeight*0.5f);

	clip_height += land_height;		// clip_height is now ABSOLUTE

	wheel_height = P->WorldPosition.Y - clip_height;


	P->OnGround = (wheel_height <= 0.01);

	if (P->OnGround && carrier)
	{
		P->ShipWeAreAbove = carrier;
		P->OnGround = 2;
		P->TerrainType = TT_CARRIER_DECK;
	}
	else
		if (wheel_height > (5 FEET))
			P->Status &= ~PL_SPEED_BOLTER;  // keeps more lines from snagging plane


	if (P->OnGround)
		P->WorldPosition.Y = clip_height;

	ret_val = (float)(P->WorldPosition.Y - land_height);

	if (ret_val < 50 FEET)
	{
		if (P->TailHook != 0.0f)
			UpdateTailHookAndCheckForWireCatch(P,carrier);

		/* -----------------7/20/99 4:45PM---------------------------------------------------------------------
		/* check for tail drag here.
		/* ----------------------------------------------------------------------------------------------------*/
		double tail_y = P->WorldPosition.Y;
		FPoint *offset = &P->Type->Offsets[OFFSET_ENGINE_REAR_LEFT];
		tail_y +=  P->Orientation.I.Y*((double)offset->Z-(1 FEET)) + P->Orientation.J.Y*(double)offset->X + P->Orientation.K.Y*(double)offset->Y;
		if((tail_y <= land_height) && ((!MultiPlayer) || (P == PlayerPlane) || (PlayerPlane->PlaneCopy == P)))
		{
			tail_y -= land_height;
			tail_y /= (double)offset->Z*0.75;
			if (tail_y > 1.0)
				tail_y = 1.0;
			else
				if (tail_y < 0.0)
					tail_y = 0.0;

			ScrapePlayerTail((float)tail_y,DAMAGE_L_ENGINE);
		}

		tail_y = P->WorldPosition.Y;
		offset = &P->Type->Offsets[OFFSET_ENGINE_REAR_RIGHT];
		tail_y +=  P->Orientation.I.Y*((double)offset->Z-(1 FEET)) + P->Orientation.J.Y*(double)offset->X + P->Orientation.K.Y*(double)offset->Y;
		if((tail_y <= land_height) && ((!MultiPlayer) || (P == PlayerPlane) || (PlayerPlane->PlaneCopy == P)))
		{
			tail_y -= land_height;
			tail_y /= (double)offset->Z*0.75;
			if (tail_y > 1.0)
				tail_y = 1.0;
			else
				if (tail_y < 0.0)
					tail_y = 0.0;

			ScrapePlayerTail((float)tail_y,DAMAGE_R_ENGINE);
		}


	}

	return ret_val;	// new HeightAboveGround
}
//************************************************************************************************
void SetUpDitch(PlaneParams *P)
{
	if (!RealisticCrashes)
		return;

	int outofcontrol = 0;

	SetLandingGearUpDown(PlayerPlane,RAISE_LANDING_GEAR);
	P->FlightStatus |= PL_PLANE_DITCHING;

	outofcontrol = P->FlightStatus & PL_OUT_OF_CONTROL;

	DestroyPlaneItem(P, DAMAGE_LO_ENGINE, 1);
	DestroyPlaneItem(P, DAMAGE_L_ENGINE, 1);
	DestroyPlaneItem(P, DAMAGE_R_ENGINE, 1);
	DestroyPlaneItem(P, DAMAGE_RO_ENGINE, 1);

#if 1
	PlayPositionalNoGearHit(Camera1,P->WorldPosition);
#else
	char wavestr[_MAX_PATH];
	char *tempstr;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath("crash.wav", drive, dir, fname, ext);
	tempstr = GetRegValue("sound");

	sprintf(wavestr, "%s\\%s%s", tempstr, fname, ext);

	if(SndStreamInit(wavestr))
	{
		SndStreamPlay();
	}
#endif

	OrphanAllPlaneSmoke(P);

	if(!outofcontrol)
	{
		P->FlightStatus &= ~(PL_OUT_OF_CONTROL);
	}

	GetFireHandle(P->WorldPosition,&g_dwPlaneFireHandle,CT_BLACK_SMOKE,0.5f,(2.0f*(1.5f - frand()) * 60.0f));
}

//************************************************************************************************
//*  Some new stuff for F/A-18
//************************************************************************************************

//************************************************************************************************
void ToggleHook(void)
{

	if ((PlayerPlane->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_R_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_EMER_HYD))
		return;

	if(PlayerPlane->TailHookState == 0)
	{
		PlayerPlane->TailHookState = 1;
		PlayerPlane->TailHookCommandedPos = 90.0f;
#if 0
		int iLevel=g_iSoundLevelCaution-10;
		if (iLevel<0) iLevel=0;
		SndQueueSound(Flapmvi,1,iLevel);
		DisplayWarning(FLAPS, ON, 1);
#endif
	}
	else
	{
		PlayerPlane->TailHookState = 0;
		PlayerPlane->TailHookCommandedPos = 0.0f;
#if 0
		int iLevel=g_iSoundLevelCaution-50;
		if (iLevel<0) iLevel=0;
		SndQueueSound(Flapmvi,1,iLevel);
		DisplayWarning(FLAPS, OFF, 1);
#endif
	}
}

//************************************************************************************************
void ToggleRefuelingProbe(void)
{

	if ((PlayerPlane->SystemInactive & DAMAGE_BIT_L_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_R_MAIN_HYD) &&
		(PlayerPlane->SystemInactive & DAMAGE_BIT_EMER_HYD))
		return;

	if(PlayerPlane->RefuelProbeCommandedPos == 0.0)
	{
		PlayerPlane->RefuelProbeCommandedPos = 45.0f;
		SetCockpitItemState(126,1);

		if (!PlayerPlane->FuelProbeLight)
		{
			if (PlayerPlane->FuelProbeLight = (PointLightSource **)GetNewLight())
			{
				PointLightSource *new_light = new PointLightSource;
				new_light->WorldPosition = PlayerPlane->WorldPosition;

				new_light->WorldPosition.AddScaledVector(-pDBAircraftList[PlayerPlane->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X FEET,PlayerPlane->Orientation.J);
				new_light->WorldPosition.AddScaledVector(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y FEET,PlayerPlane->Orientation.K);
				new_light->WorldPosition.AddScaledVector(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z FEET,PlayerPlane->Orientation.I);
				new_light->Direction = PlayerPlane->Orientation.I;
				new_light->Color.SetValues(1.0,1.0,1.0);
				new_light->Flags |= LIGHT_TINY_SPOT | LIGHT_DIRECTED;
				new_light->Radius1 = 500.0 FEET;
				new_light->ooRadius1 = 1.0f/(500.0 FEET);
				new_light->Radius2 = 100.0 FEET;
				new_light->ooRadius2 = 1.0f/(100.0 FEET);
				*(PlayerPlane->FuelProbeLight) = new_light;
			}
		}

//#if 0
//		int iLevel=g_iSoundLevelCaution-10;
//		if (iLevel<0) iLevel=0;
//		SndQueueSound(Flapmvi,1,iLevel);
//		DisplayWarning(FLAPS, ON, 1);
//#endif

	}
	else
	{
		PlayerPlane->RefuelProbeCommandedPos = 0.0f;
		SetCockpitItemState(126,0);

		if (PlayerPlane->FuelProbeLight)
		{
			RemoveLight((LightSource **)PlayerPlane->FuelProbeLight);
			PlayerPlane->FuelProbeLight = NULL;
		}


//#if 0
//		int iLevel=g_iSoundLevelCaution-50;
//		if (iLevel<0) iLevel=0;
//		SndQueueSound(Flapmvi,1,iLevel);
//		DisplayWarning(FLAPS, OFF, 1);
//#endif


	}


}

void CalcOxygen(PlaneParams *P)
{
	if ((P == PlayerPlane) && (P->DamageFlags & DAMAGE_BIT_OXYGEN))
	{
		if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & (PL_STATUS_CRASHED | PL_PLANE_BLOWN_UP)) && (P->WorldPosition.Y > 10000 FEET))
		{
			P->TicksWithoutOxygen += 0.1f * ((P->WorldPosition.Y - 10000 FEET)/1000 FEET);
			if (P->TicksWithoutOxygen > 300.0f)
				P->TicksWithoutOxygen = 300.0f;
		}
		else
		{
			P->TicksWithoutOxygen -= 1.0f;
			if (P->TicksWithoutOxygen < 0.0f)
				P->TicksWithoutOxygen = 0.0f;

		}
	}
}

ControlRange zero2one = {0.0f,1.0f};
ControlRange zero2ninety = {0.0f,90.0f};
ControlRange winguprange = {0, 120.0f};
ControlRange proberange = {0, 45.0f};
ControlRange leadingedgerange = {0, 30.0f};

void CalcCarrierBasedVisuals(PlaneParams *P)
{
	float dt;


	if(P->Status & (PL_DEVICE_DRIVEN | PL_COMM_DRIVEN))
	{
		dt = P->dt;
	}
	else
	{
		dt = (float)DeltaTicks/1000.0f;
	}

	//***********************************************************************************
	//*  New stuff for F/A-18
	//***********************************************************************************

	P->WingUp =  Integrate(	0, 0,
   							P->WingUp,
							winguprange,
							20.0f,
	  						dt, P->WingUpCommandedPos);

	P->TailHook =  Integrate(0, 0,
   						    	 P->TailHook,
								 zero2ninety,
						  		 60.0f,
	  					  		 dt, P->TailHookCommandedPos);

#if 0  //  Right now probe is a switch

	P->RefuelProbe =  Integrate(0, 0,
   						    	 P->RefuelProbe,
								 proberange,
						  		 45.0f,
	  					  		 dt, P->RefuelProbeCommandedPos);
#endif

	P->Canopy =  Integrate(0, 0,
   						    	 P->Canopy,
								 zero2ninety,
						  		 10.0f,
	  					  		 dt, P->CanopyCommandedPos);


	P->LeadingEdge =  Integrate(0, 0,
   						    	 P->LeadingEdge,
								 leadingedgerange,
						  		 30.0f,
	  					  		 dt, P->LeadingEdgeCommandedPos);

	if (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
	{
#if 0
		if (P->AI.cFXarray[0])
			DeployFuelHose(P,0);
		else
			RetractFuelHose(P,0);
#else
		if(P->AI.iAIFlags2 & AILANDING)
			RetractFuelHose(P,0);
#endif
	}

	RefuelHoseData *rhdwalk = &P->RefuelingHoses[MAX_REFUELING_HOSES];
	int i = MAX_REFUELING_HOSES;

	while(rhdwalk-- != P->RefuelingHoses)
	{
		i--;
		if (rhdwalk->CommandedPosition != rhdwalk->Position)
		{
			rhdwalk->Position = Integrate(	0.0f,0.0f,rhdwalk->Position ,
							           		zero2one,
								       		0.1f,
							           		dt, rhdwalk->CommandedPosition);


			CalculateRefuelHosePosition(P,i);
		}
		else
			if (rhdwalk->Position)
				CalculateRefuelHosePosition(P,i);


		if (rhdwalk->Status & RH_CONNECTED)
			rhdwalk->ConnectedPosition = 1.0f;
		else
			if (rhdwalk->ConnectedPosition)
				rhdwalk->ConnectedPosition = Integrate(	0.0f,0.0f,rhdwalk->ConnectedPosition ,
							           					zero2one,
								       					0.5f,
							           					dt, 0.0f);
	}

	if (P->TowedDecoyCommandedPosition != P->TowedDecoyPosition)
	{
		P->TowedDecoyPosition = Integrate(	0.0f,0.0f,P->TowedDecoyPosition ,
							           		zero2one,
								       		0.25f,
							           		dt, P->TowedDecoyCommandedPosition);
	}

}

void FreePlaneLights(PlaneParams *P)
{
	if (P->MachineGunLight)
	{
		RemoveLight((LightSource **)P->MachineGunLight);
		P->MachineGunLight = NULL;
	}

	if (P->ABLight)
	{
		RemoveLight((LightSource **)P->ABLight);
		P->ABLight = NULL;
	}

	if (P->FuelProbeLight)
	{
		RemoveLight((LightSource **)P->FuelProbeLight);
		P->FuelProbeLight = NULL;
	}
}

void UpdatePlaneLightPositions(PlaneParams *P)
{
	FPointDouble *location;

	if (P->MachineGunLight)
	{
		FPoint gun_off;

		gun_off = P->Type->MachineGun;
		gun_off.Normalize();
		gun_off *= 1.0 FEET;
		gun_off.Y += 0.25 FEET;
		gun_off += P->Type->MachineGun;

		location = &(*(P->MachineGunLight))->WorldPosition;
		*location = P->WorldPosition;
		location->AddScaledVector(gun_off.X,P->Orientation.J); //rgt
		location->AddScaledVector(-gun_off.Y,P->Orientation.K); //down
		location->AddScaledVector(-gun_off.Z,P->Orientation.I); //fwd
//	 	(*(P->MachineGunLight))->Intensity = fmod(P->WorldPosition.X + P->WorldPosition.Y,1.0);
	}

	if (P->FuelProbeLight)
	{
		FPointDouble *new_point;

		new_point = &(*(P->FuelProbeLight))->WorldPosition;

		*new_point = P->WorldPosition;
		new_point->AddScaledVector(-pDBAircraftList[P->AI.iPlaneIndex].OffSet[OFFSET_PROBE].X FEET,P->Orientation.J);
		new_point->AddScaledVector(pDBAircraftList[P->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Y FEET,P->Orientation.K);
		new_point->AddScaledVector(pDBAircraftList[P->AI.iPlaneIndex].OffSet[OFFSET_PROBE].Z FEET,P->Orientation.I);
		(*(P->FuelProbeLight))->Direction = P->Orientation.I;

	}


	if ((P->LeftThrustPercent > 80.0f) || (P->RightThrustPercent > 80.0f))
	{
		if (!P->ABLight)
		{
			if (P->ABLight = (PointLightSource **)GetNewLight())
			{
				PointLightSource *new_light = new PointLightSource;
				new_light->WorldPosition = P->WorldPosition;
		//		new_light->Color = WhiteishFireColor;
				new_light->Color.SetValues(0.65,0.65,0.95);
				new_light->Flags |= LIGHT_TINY_SPOT;
				new_light->Radius1 = 100.0 FEET;
				new_light->ooRadius1 = 1.0f/(100.0 FEET);
				new_light->Radius2 = 10.0 FEET;
				new_light->ooRadius2 = 1.0f/(10.0 FEET);
				*(P->ABLight) = new_light;
			}
		}

		if (P->ABLight)
		{
			FPoint *offset;
			float modifier;
			location = &(*(P->ABLight))->WorldPosition;

			*location = P->WorldPosition;

			offset = &P->Type->Offsets[OFFSET_ENGINE_REAR_LEFT];
	 		(*(P->ABLight))->Intensity = (__max(P->LeftThrustPercent,P->RightThrustPercent)-80.0f)/20.0f;

			location->AddScaledVector(offset->Y,P->Orientation.K); //down
			location->AddScaledVector(offset->Z-(5.0 FEET)*(*(P->ABLight))->Intensity,P->Orientation.I); //fwd

			if (P->LeftThrustPercent > 80.0f)
				if (P->RightThrustPercent > 80.0f)
				{
					modifier = P->RightThrustPercent - P->LeftThrustPercent;
					modifier /= 40.0f;
					modifier += 0.5;
					modifier *= modifier;
				}
				else
					modifier = 0.0;
			else
				modifier = 1.0f;

			location->AddScaledVector(offset->X*(1.0f-modifier),P->Orientation.J); //left engine
			location->AddScaledVector(offset[1].X*modifier,P->Orientation.J); //right engine

		}
	}
	else
		if (P->ABLight)
		{
			RemoveLight((LightSource **)P->ABLight);
			P->ABLight = NULL;
		}

}


extern AvionicsType Av;

void FuelDumpSmoke(PlaneParams *P)
{
	if (Av.Fuel.DumpFuel)
	{
		if (!PlayerPlane->Smoke[SMOKE_LEFT_FUEL_DUMP])
			PlayerPlane->Smoke[SMOKE_LEFT_FUEL_DUMP] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_LEFT_FUEL_DUMP,FUEL_DUMP);
		if (!PlayerPlane->Smoke[SMOKE_RIGHT_FUEL_DUMP])
			PlayerPlane->Smoke[SMOKE_RIGHT_FUEL_DUMP] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_RIGHT_FUEL_DUMP,FUEL_DUMP);

		if(PlayerPlane->OnGround)
		{
			Av.Fuel.DumpFuel = 0;
		}
	}
	else
	{
		if (PlayerPlane->Smoke[SMOKE_LEFT_FUEL_DUMP])
		{
			((SmokeTrail *)PlayerPlane->Smoke[SMOKE_LEFT_FUEL_DUMP])->MakeOrphan();
			PlayerPlane->Smoke[SMOKE_LEFT_FUEL_DUMP] = NULL;
		}

		if (PlayerPlane->Smoke[SMOKE_RIGHT_FUEL_DUMP])
		{
			((SmokeTrail *)PlayerPlane->Smoke[SMOKE_RIGHT_FUEL_DUMP])->MakeOrphan();
			PlayerPlane->Smoke[SMOKE_RIGHT_FUEL_DUMP] = NULL;
		}
	}
}