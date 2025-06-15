#include "F18.h"
#include "spchcat.h"
#include "gamesettings.h"
#include "keystuff.h"
#include "particle.h"

#include "autoplt.h"
#include "engine.h"
#include "flightfx.h"
#include "flight.h"

#include "snddefs.h"

#include "aero.h"

float GKValue = 1.0f;

int WeHaveRecovered = 1;
int CaptureNew = 1;
int OurPlaneTakingOffFromRunway = 0;
int CaptureNewTrim = 1;

VKCODE DisableGLimit;

extern AvionicsType Av;
extern void PlayPositionalNoGearHit(CameraInstance &camera,FPointDouble &SoundPosition);

#ifdef _DEBUG

/* Please reflect any changes made here in the Release versions of these functions */
/* Which are located in FLIGHT.H												   */

//************************************************************************************************
inline float DegToRad(float Degree)
{
	return (Degree*(PI/180.0f));
}
//************************************************************************************************
inline float RadToDeg(float Rad)
{
	return (Rad*(180.0f/PI));
}
//************************************************************************************************

#endif

//************************************************************************************************
//  Implementation for PlaneParams class
//************************************************************************************************
PlaneParams &PlaneParams::operator=(const InitPlaneParams &init_params)
{
	memset(this,0,sizeof(PlaneParams));

	Status				= init_params.Status;
	EngineStatus		= init_params.EngineStatus;

	UpdateRate			= init_params.UpdateRate;
	AeroComplexity		= init_params.AeroComplexity;
	dt					= init_params.dt;

	LastAlpha			= init_params.LastAlpha;

	Orientation.I		= init_params.Orientation.I;
	Orientation.J		= init_params.Orientation.J;
	Orientation.K		= init_params.Orientation.K;

	AvailGForce			= init_params.AvailGForce;

	AutoPilotMode		= init_params.AutoPilotMode;
	FlapsMode			= init_params.FlapsMode;

	CurrentTrimValue    = init_params.CurrentTrimValue;

	GunFireRate			= init_params.GunFireRate;

	Stats				= init_params.Stats;
	C					= init_params.C;
	CS					= init_params.CS;

	MaxSpeedAccel		= init_params.MaxSpeedAccel;
	SpeedPercentage		= init_params.SpeedPercentage;

	MaxRollRate			= init_params.MaxRollRate;
	RollPercentage		= init_params.RollPercentage;

	MaxPitchRate		= init_params.MaxPitchRate;
	PitchPercentage		= init_params.PitchPercentage;

	YawRate				= init_params.YawRate;

	RollYawCouple		= init_params.RollYawCouple;

	TerrainType			= TT_RUNWAY;	// Despite what it looks like, this doesn't mean it thinks it's on the runway
	TerrainType2 		= 0xFFFFFFFF;

	AADesignate			= NULL;
	AGDesignate.X		= -1.0;
	AGDesignate.Y		= -1.0;
	AGDesignate.Z		= -1.0;


	DoGetPlayerInputs	= init_params.DoGetPlayerInputs;
	DoControlSim		= init_params.DoControlSim;
	DoCASSim			= init_params.DoCASSim;
	DoForces			= init_params.DoForces;
	DoPhysics			= init_params.DoPhysics;
	DoAttitude			= init_params.DoAttitude;

	DoSimpleFlight		= init_params.DoSimpleFlight;
	DoSimpleAttitude    = init_params.DoSimpleAttitude;

	AutoPilotMode		= PL_AP_ALT_BARO | PL_AP_HDG;
	WeHaveRecovered		= 1;
	CaptureNew			= 1;
	OutOfControlFlight  = 0;

	return *this;
}
//************************************************************************************************
//  End of implementation for PlaneParams class
//************************************************************************************************
float MachToFtSec(PlaneParams *P, float Mach)
{
	float Vss;

	if (P->Altitude <= 0)
		Vss = sqrt((1.4*1716.3*(519.0*(1.0-0.00000703*1.0))));   // make this equation a constant
	else
		Vss = sqrt(1.4*1716.3*(519.0*(1.0-0.00000703*P->Altitude)));

	return(Vss*Mach);
}
//************************************************************************************************
inline float KnotsTASToIAS(float KnotsTAS, float Alt)
{
	if (Alt < 0.0f) Alt = 0.0f;
	if (Alt > 70000.0f) Alt = 70000.0f;

	return(KnotsTAS * sqrt(Pressure[(int)(Alt/500.0f)]/0.0023769f));
}
//************************************************************************************************
inline float FtSecTASToIAS(float FtSecTAS, float Alt)
{
	double TrueKnots = FtSecTAS*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);
	if (Alt < 0.0f) Alt = 0.0f;
	if (Alt > 70000.0f) Alt = 70000.0f;

	return(float)(TrueKnots * sqrt(Pressure[(int)(Alt/500.0f)]/0.0023769f));
}
//************************************************************************************************
inline float KnotsIASToTAS(float KnotsIAS, float Alt)
{
	if (Alt < 0.0f) Alt = 0.0f;
	if (Alt > 70000.0f) Alt = 70000.0f;

	return(KnotsIAS / sqrt(Pressure[(int)(Alt/500.0f)]/0.0023769f));
}
//************************************************************************************************
inline float KnotsTASToFtSec(float KnotsTAS)
{
	return((float)KnotsTAS/(FTSEC_TO_MLHR*MLHR_TO_KNOTS));
}
//************************************************************************************************
float Integrate(float InputPos, float InputPosMax, float CurrentDegree,
			    ControlRange Range, float Rate, float dt, float AINewDegree)
{
   	float NewDegree;

	// Calculate new degree	per request

    if (AINewDegree < 9999.0)
	{
		if (CurrentDegree == AINewDegree)
			return(CurrentDegree);
		NewDegree = AINewDegree;
	}
	else
		if (InputPosMax != 0.0)
			NewDegree = (InputPos*((Range.High-Range.Low)/(InputPosMax*2.0f)));

	float MaxUpdate = Rate*dt;

	if (fabs(NewDegree-CurrentDegree) > MaxUpdate)
	{
		if (NewDegree > CurrentDegree)
			NewDegree = CurrentDegree + MaxUpdate;
		else
			NewDegree = CurrentDegree - MaxUpdate;
	}

	// Limit request to maximum

	if (NewDegree > Range.High) NewDegree = Range.High;
	else if (NewDegree < Range.Low) NewDegree = Range.Low;

	return(NewDegree);
}
//************************************************************************************************
void DoSpeedBrakeCAS(PlaneParams *P)
{
	if (P->SpeedBrakeState)
	{
		float PosDeg;
		float TmpAOA = RadToDeg(P->Alpha);

		// In PA mode the speedbrake does not work

		if ((P->Mach < 1.05) &&
			//(P->FlapsMode == FLAPS_AUTO) &&
			(TmpAOA < 16.0) && (TmpAOA > -9.0))
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

		P->SpeedBrakeCommandedPos = PosDeg;

	}
	else
		P->SpeedBrakeCommandedPos = 0.0f;

	// We now have a PosDeg (the position of the speedbrake in degrees) based on speed
	// Use a proportion of that to move the ailerons and s visually - note
	// these surface will have no effect on the aerodynamics of the simulation.

	P->SBRudderOffset = Integrate(0.0f,0.0f, P->SBRudderOffset,
							           P->CS->RudderRange,
								       P->CS->SpeedBrakeRate,
							           P->dt, P->SpeedBrakeCommandedPos/2.0f);

	P->SBAileronOffset = Integrate(0.0f,0.0f, P->SBAileronOffset,
						 	           P->CS->AileronRange,
						 		       P->CS->SpeedBrakeRate,
						 	           P->dt, P->SpeedBrakeCommandedPos/8.0f);
}
//************************************************************************************************
void DoFlapsCAS(PlaneParams *P)
{
	// Note : This function is for purely visual reasons

  	float TmpAOA = RadToDeg(P->Alpha);

	if (P->IndicatedAirSpeed > 240)
		P->FlapsMode = FLAPS_AUTO;

	if (P->FlapsMode & FLAPS_AUTO)
	{
		// schedule LEFs - leading edge flaps based on AOA

		if (TmpAOA < 3.0)
			P->LEFlapsCommandedPos = 0.0;
		else
		{
			if (TmpAOA > 25.0) TmpAOA = 25.0;

			// 3deg AOA = 1deg LEF      25deg AOA = 34deg LEF
			P->LEFlapsCommandedPos = (1.5*TmpAOA) - 3.5;
		}

		// schedule TEFs - trailing edge flaps based on AOA

		TmpAOA = RadToDeg(P->Alpha);  // could have been modified

 		if (TmpAOA < 3.0)
			P->FlapsCommandedPos = 0.0;
		else
		{
			if ((TmpAOA >= 3.0) && (TmpAOA < 8.0))

				// 3deg AOA = 1deg TEF     8deg AOA = 12deg TEF
				P->FlapsCommandedPos = (TmpAOA*2.2)-5.6;				

			else
			{
				if (TmpAOA > 34.0) TmpAOA = 34.0;							

				// 8deg AOA = 12deg TEF     34deg AOA = 0deg TEF
				P->FlapsCommandedPos = (TmpAOA*-0.462)+15.692;
			}
		}
	}
	else
	{
		// in PA LEFS are scheduled as a function of AOA - I used UA mode schedule
		//		 TEFS are scheduled as a function of Airspeed with full deflection at approach speeds

		float TmpIAS = P->IndicatedAirSpeed;

		if (TmpIAS < 135.0) TmpIAS = 135.0;

		if (P->FlapsMode & FLAPS_HALF)

			// 135 IAS = 30deg      240 IAS =7.6 deg  (7.6 based on 6deg AOA @ 240)
			PlayerPlane->FlapsCommandedPos = (TmpIAS* -0.213)+58.8;
		else
			// 135 IAS = 40deg      240 IAS =7.6 deg  (7.6 based on 6deg AOA @ 240)
			PlayerPlane->FlapsCommandedPos = (TmpIAS* -0.309)+81.657;

		if (TmpAOA > 25.0) TmpAOA = 25.0;

		// 3deg AOA = 1deg LEF      25deg AOA = 34deg LEF
		P->LEFlapsCommandedPos = (1.5*TmpAOA) - 3.5;
	}


	P->FLPSAileronOffset = Integrate(0.0f,0.0f, P->FLPSAileronOffset,
						 	           P->CS->AileronRange,
						 		       20.0,
						 	           P->dt, P->FlapsCommandedPos);


	P->LastFlapsMode = P->FlapsMode;
	P->LastFlapsCommandedPos = P->FlapsCommandedPos;
}
//************************************************************************************************
void CalcF18ControlSurfaces(PlaneParams *P)
{

  	DoFlapsCAS(P);

	// Calculate Availiable G

	float TmpWeight = P->TotalWeight;

	if (TmpWeight > 66000)
		TmpWeight = 66000;
	else
		if (TmpWeight < 42000)
			TmpWeight = 42000;

	float GLoadLimit = (-0.0001125*TmpWeight) + 12.225;

	if (P->OnGround)
	{
		P->AvailGForce = 4.0;		// this give us some elevator authority for takeoff
	}
	else
	{
		if (P->IndicatedAirSpeed > 350)
		{
			P->AvailGForce = GLoadLimit;
			if (GetVkStatus(DisableGLimit))
				P->AvailGForce += (P->AvailGForce* 0.33);
		}
		else
		{
			P->AvailGForce = (0.00008489*(P->IndicatedAirSpeed * P->IndicatedAirSpeed)) +
							 (-0.012*P->IndicatedAirSpeed) + 1.416;

			if (P->AvailGForce > GLoadLimit)
				P->AvailGForce = GLoadLimit;

			if (P->AvailGForce > 7.5) P->AvailGForce = 7.5;
			if (P->AvailGForce < 1.0) P->AvailGForce = 1.0;
		}
	}

	// Transonic Bucket overrides everything
	if ((P->Mach > 0.96) && (P->Mach <= 0.98))
		P->AvailGForce = 6.8;

	if ((P->Mach > 0.98) && (P->Mach <= 1.02))
		P->AvailGForce = 5.8;

	if ((P->Mach > 1.02) && (P->Mach < 1.03))
		P->AvailGForce = 6.8;

	// Implement G Requestor system

	if (P->StickY == 0)
		P->RequestedG = 1.0;
	else
		if (P->StickY > 0)		// Positive G request
		{
			if (P->StickY > 114.0) P->StickY = 114.0;
//			P->RequestedG = (float)(P->StickY/(114.0/P->AvailGForce));
			P->RequestedG = (float)(P->StickY/(114.0/fabs(P->AvailGForce - 1.0f))) + 1.0f;

			if (P->RequestedG < 1.0) P->RequestedG = 1.0;
		}
		else					// Negative G request
		{
			if (P->StickY < -114.0) P->StickY = -114.0;

			float NegAvailG = P->AvailGForce;
			if (NegAvailG > 2.0) NegAvailG = 2.0;

//			P->RequestedG = (float)(P->StickY/(114.0/NegAvailG));
			P->RequestedG = (float)(P->StickY/(114.0/(NegAvailG + 1.0f))) + 1.0f;
		}

	// Implement AOA Requestor system

 	float TmpStickY = (float)P->StickY;
	if (TmpStickY >  114.0f) TmpStickY = 114.0f;
	if (TmpStickY < -114.0f) TmpStickY = -114.0f;

   	P->RequestedAOAPercentage = (float)((TmpStickY/2.85f)/40.0f);  // 40 degrees max AOA requested

	// Calculate Rudder effectivness based on speed

	if (P->Mach > 1.05)
		P->RudderRatio = 0.0;
	else
	{
		float ClipMach = P->Mach;
		if (ClipMach < 0.6)
			ClipMach = 0.6;

		P->RudderRatio = (ClipMach*-1.66667)+2.0;
		P->RudderRatio *= 2.5;		// Per Jim Campisi	
		
		if (P->RudderRatio < 0.0)
			P->RudderRatio = 0.0;
	}

	// Modify RollRatio based on MACH (attached shock wave)
	// As we go faster slow down roll rate requested


	if (P->Mach <= 0.5)
	{
		P->RollRatio = (2.0*P->Mach) + -0.25;
		if (P->RollRatio > 0.75) P->RollRatio = 0.75;
		if (P->RollRatio < 0.25) P->RollRatio = 0.25;
	}
	else
	{
		P->RollRatio = (-1.375*P->Mach) + 1.437;
		if (P->RollRatio > 0.75) P->RollRatio = 0.75;
		if (P->RollRatio < 0.20) P->RollRatio = 0.20;
	}

	if (TmpWeight > 66000)
		TmpWeight = 66000;
	else
		if (TmpWeight < 38000)
			TmpWeight = 38000;

	P->RollRatio *= (-0.00001143*TmpWeight) + 1.434;


	// Modify RollRatio based on G-loading

 	float CurrentG = P->GForce;
	if (CurrentG < 1.0) CurrentG = 1.0;
	else
		if (CurrentG > 7.5) CurrentG = 7.5;


	P->RollRatio *= (CurrentG*-0.14872)+1.14872;


	// Modify RollRatio as a function of AOA

	float CurrentAOA = fabs(RadToDeg(P->Alpha));
	if (CurrentAOA > 70.0) 
		CurrentAOA = 70.0;

	if (CurrentAOA >= 25.0)
	{

		//25AOA = 1.0    70AOA = .10
		P->RollRatio *= (CurrentAOA*-0.017) + 1.417;
	}

	// Determine Thrust

	P->LeftThrust =  GetCommandedThrust(P, P->LeftThrustPercent);
	P->RightThrust = GetCommandedThrust(P, P->RightThrustPercent);

	if (!LeftEngineOn  || (P->LeftThrust < 0.0))  P->LeftThrust = 0.0;
	if (!RightEngineOn || (P->RightThrust < 0.0)) P->RightThrust = 0.0;

	if (P->SpeedBrake != P->SpeedBrakeCommandedPos)
	{
		P->SpeedBrake = Integrate(0.0f,0.0f, P->SpeedBrake,
							           P->CS->SpeedBrakeRange,
								       P->CS->SpeedBrakeRate,
							           P->dt, P->SpeedBrakeCommandedPos);
	}

	if (P->Flaps != P->FlapsCommandedPos)
	{
		P->Flaps = Integrate(0.0f,0.0f, P->Flaps,
				             P->CS->FlapRange,
							 20.0,
							 P->dt, P->FlapsCommandedPos);
	}

	if (P->LEFlaps != P->LEFlapsCommandedPos)
	{
		P->LEFlaps = Integrate(0.0f,0.0f, P->LEFlaps,
				             P->CS->LEFlapRange,
							 20.0,
							 P->dt, P->LEFlapsCommandedPos);
	}

	if (P->Rudder != P->RudderPedalsCommandedPos)
	{
		P->Rudder = Integrate(-P->RudderPedalsCommandedPos * P->RudderRatio,
									128.0f,
									P->RudderPedals,
									P->CS->RudderRange,
									P->CS->RudderRate,
									P->dt,
									99999.0f);
	}
	if (P->Mach > 1.05)
		P->Rudder = 0.0f;

	MoveLandingGear(P);

	// Calculate Aileron Movement
	P->Aileron = Integrate(P->StickX*P->RollRatio, 128.0f,
		     			   P->Aileron,
						   P->CS->AileronRange,
						   P->CS->AileronRate,
						   P->dt, 99999.0f);

	// Differential command is limited by the availiable differential deflection left over from symetrical
	float MaxDiffRange = (P->CS->SymetricalElevatorRange.High - P->CS->SymetricalElevatorRange.Low)/2.0f;  // pre-compute later

	// Neccessary bias of Sym. Elevator to get max Diff. Elevator deflection
	float Bias = P->CS->SymetricalElevatorRange.Low + MaxDiffRange;	// pre-compute later

	// Low is always greater that High for Sym. Elevator
	float NewMaxRange = MaxDiffRange - abs(Bias-P->SymetricalElevator);
	P->CS->DifferentialElevatorRange.Low = -NewMaxRange;
	P->CS->DifferentialElevatorRange.High = NewMaxRange;

	P->DifferentialElevator =  Integrate(P->StickX*P->RollRatio, 128.0f,
	       						    	 P->DifferentialElevator,
										 P->CS->DifferentialElevatorRange,
								  		 P->CS->ElevatorRate,
			  					  		 P->dt, 99999.0f);

	CalcCarrierBasedVisuals(P);
	CalcOxygen(P);

}
//************************************************************************************************
void StallTrim(PlaneParams *P)
{

	P->SymetricalElevator = Integrate(-P->StickY*0.20, 128.0f,
		     							P->SymetricalElevator,
										P->CS->SymetricalElevatorRange,
										2,
										P->dt, 99999.0f);
}

//************************************************************************************************
void OnGroundTrim(PlaneParams *P)
{

	P->SymetricalElevator = Integrate(-P->StickY*0.10, 128.0f,
		     							P->SymetricalElevator,
										P->CS->SymetricalElevatorRange,
										/*P->CS->ElevatorRate*/ 2,
										P->dt, 99999.0f);
}
//************************************************************************************************
void Trim(PlaneParams *P)
{
	float TrimGain, NewValue;

	// flight computer damaged - screw with trim system
    if (P->SystemInactive & DAMAGE_BIT_FLT_CONT)
	    P->RequestedG -= 0.2;

	float TmpAlt = P->Altitude/1000.0;
	if (TmpAlt < 5.0)
		TmpAlt = 5.0;
	else
		if (TmpAlt > 50.0)
			TmpAlt = 50.0;

	TrimGain = (TmpAlt*-0.00028888889) + 0.0164444;

	// we are pulling g's here

	NewValue = (TrimGain*(P->GForce-P->RequestedG));

	P->SymetricalElevator += NewValue;

	if (P->SymetricalElevator > P->CS->SymetricalElevatorRange.High)
		P->SymetricalElevator = P->CS->SymetricalElevatorRange.High;
	else
		if (P->SymetricalElevator < P->CS->SymetricalElevatorRange.Low)
			P->SymetricalElevator = P->CS->SymetricalElevatorRange.Low;

	P->CurrentTrimValue = P->SymetricalElevator;
	CaptureNewTrim = 1;
}
//************************************************************************************************
void TrimTo(PlaneParams *P, float GForceTo)
{
	float TrimGain, NewValue;

	// flight computer damaged - screw with trim system
    if (P->SystemInactive & DAMAGE_BIT_FLT_CONT)
	    P->RequestedG -= 0.2;

	float TmpAlt = P->Altitude/1000.0;
	if (TmpAlt < 5.0)
		TmpAlt = 5.0;
	else
		if (TmpAlt > 50.0)
			TmpAlt = 50.0;

	TrimGain = (TmpAlt*-0.00028888889) + 0.0164444;

	// we are pulling g's here

	NewValue = (TrimGain*(P->GForce-GForceTo));

	P->SymetricalElevator += NewValue;

	if (P->SymetricalElevator > P->CS->SymetricalElevatorRange.High)
		P->SymetricalElevator = P->CS->SymetricalElevatorRange.High;
	else
		if (P->SymetricalElevator < P->CS->SymetricalElevatorRange.Low)
			P->SymetricalElevator = P->CS->SymetricalElevatorRange.Low;

	P->CurrentTrimValue = P->SymetricalElevator;
	CaptureNewTrim = 1;
}
//************************************************************************************************
void DoAOATrim(PlaneParams *P)
{
	float TrimGain = 0.0010;
	float NewValue;

	if (P->StickY == 0.0)		// trim to last aoa - if possible
	{
		if (CaptureNewTrim)
		{
			P->TrimToAOA = (RadToDeg(P->Alpha));		// were are we currently
			CaptureNewTrim ^= 1;
		}

		NewValue = (TrimGain*(RadToDeg(P->Alpha) - P->TrimToAOA));

		P->SymetricalElevator += NewValue;

		if (P->SymetricalElevator > P->CS->SymetricalElevatorRange.High)
			P->SymetricalElevator = P->CS->SymetricalElevatorRange.High;
		else
   	  		if (P->SymetricalElevator < P->CS->SymetricalElevatorRange.Low)
				P->SymetricalElevator = P->CS->SymetricalElevatorRange.Low;

		P->CurrentTrimValue = P->SymetricalElevator;
	}
   	else	  					// trim to commanded AOA
	{
		float DeltaRequest;

		if (P->RequestedAOAPercentage >= 0.0)
			DeltaRequest = (P->RequestedAOAPercentage * (50.0 - P->TrimToAOA));
		else
			DeltaRequest = (P->RequestedAOAPercentage * (100.0 + P->TrimToAOA));

		P->NewRequestedAOA = P->TrimToAOA + DeltaRequest;

		NewValue = (TrimGain*(RadToDeg(P->Alpha) - P->NewRequestedAOA));

		P->SymetricalElevator += NewValue;

		if (P->SymetricalElevator > P->CS->SymetricalElevatorRange.High)
			P->SymetricalElevator = P->CS->SymetricalElevatorRange.High;
		else
			if (P->SymetricalElevator < P->CS->SymetricalElevatorRange.Low)
				P->SymetricalElevator = P->CS->SymetricalElevatorRange.Low;

		P->CurrentTrimValue = P->SymetricalElevator;
		CaptureNewTrim = 1;
	}
}

//************************************************************************************************
void DoTrimmedFlight(PlaneParams *P, int LastInAPMode)
{
//	static int CaptureNew = 1;
//	static int WeHaveRecovered = 1;

	if (P->AGL > 5)
	{
		if ( ( (fabs(RadToDeg(P->Alpha)) < 60.0) && (P->IndicatedAirSpeed > 90)) && WeHaveRecovered)
		{
			// We neutralized the stick in AOA CAS so capture the current attitude

			if (((P->StickY == 0.0) && CaptureNew) || LastInAPMode)
			{
				 HoldAttitude = (float)((signed short)P->Pitch);
				 CaptureNew = 0;
			}

			if (P->StickY != 0.0)		
				CaptureNew = 1;

			if ((P->IndicatedAirSpeed >= 240) || (fabs(P->RequestedAOAPercentage) <  0.33))
				Trim(P);
  			else
			{
				DoAOATrim(P);
			}
			// If stick is neutralized then add an attitude trim system for stabilization

			if ((P->StickY == 0.0) && !P->OnGround)
			{
				if((P->Roll < 0x2000) || (P->Roll > 0xE000))
				{
					DoAttitudeHoldAutoPilot(P, HoldAttitude);
				}
				else
				{
					CaptureNew = 1;
				}
			}
		}
		else
		{
			WeHaveRecovered = ((P->IndicatedAirSpeed > 130) && (fabs(RadToDeg(P->Alpha)) < 30.0));
			CaptureNew = 1;
			StallTrim(P);
		}
	}
	else
		OnGroundTrim(P);
}
//************************************************************************************************
void DoTakeOffCAS(PlaneParams *P)
{
	Trim(P);
	
	float TargetAttitude = (float)(10*DEGREE);

	float CurrAttitude = (float)((signed short)P->Pitch);
	float SuggestedAttTrim = (0.000018f*(CurrAttitude-TargetAttitude));

	P->SymetricalElevator += Integrate(0.0f,0.0f,0.0f,
  		   							   P->CS->SymetricalElevatorRange,
									   P->CS->ElevatorRate,
	    							   P->dt, SuggestedAttTrim);
}
//************************************************************************************************
void CalcF18CASUpdates(PlaneParams *P)
{
	static int LastAutoPilotMode = 0;
	static int LastAutoPilotStatus = 0;
	static int LastInAPMode = 0;

	DoSpeedBrakeCAS(P);

	if (P != PlayerPlane) return;

	if (P->OnGround && /*(P->TerrainType == TT_AIRPORT) &&*/ (P->IndicatedAirSpeed <= 48.0))
		OurPlaneTakingOffFromRunway = 1;

	//********************************************
	//*** TAKING OFF FROM A RUNWAY - CAS
	//********************************************

	if (OurPlaneTakingOffFromRunway && (P->IndicatedAirSpeed <= 240))
	{
		if (P->StickY == 0.0)
		{
			float TmpWeight = P->TotalWeight;

			if (TmpWeight > 66000)
				TmpWeight = 66000;
			else
				if (TmpWeight < 38000)
					TmpWeight = 38000;

			float TakeOffSpeed = (TmpWeight*0.002) + 52.143;
			
			if (P->IndicatedAirSpeed > (TakeOffSpeed-10.0))
				DoTakeOffCAS(P);
			else
				P->SymetricalElevator = 0.0;	
		}
		else
		{
			OnGroundTrim(P);
		}
	}
	else

	//********************************************
	//*** NORMAL FLIGHT - CAS
	//********************************************

	{
		OurPlaneTakingOffFromRunway = 0;

		if (P->OnGround)
		{
			if (UFC.APStatus == ON) AutoPilotOff(P);
			Trim(P);
			return;
		}

		// Check if AP's within constraints
 		CheckAPDeselect(P);

		if (RealisticAPs && UFC.APStatus)
		{
			// If we have changed mode while AP is on or
			// We have just turned it on
			if ((LastAutoPilotMode != P->AutoPilotMode) || (LastAutoPilotStatus != UFC.APStatus))
			{
			   // Grab Data that is necessary for Realistic AP functionality

				if (P->AutoPilotMode & PL_AP_ALT_BARO)
					HoldAltitude = P->Altitude;
				else
					if (P->AutoPilotMode & PL_AP_ALT_RDR)
						HoldAltitudeDelta = P->HeightAboveGround*WUTOFT;
					else
						if (P->AutoPilotMode & PL_AP_ATTITUDE)
							HoldAttitude = (float)((signed short)P->Pitch);

				if (P->AutoPilotMode & PL_AP_ROLL)
				{
					HoldBankAngle = (float)(((unsigned short)(P->Roll/(unsigned short)DEGREE)));
					if (HoldBankAngle > 180.0f) HoldBankAngle -= 360.0f;
					HoldBankAngle = -HoldBankAngle;
				}
			}

			// Now implement the autopilot mode

	 		if (P->AutoPilotMode & PL_AP_CPL)
			{
				switch (Av.Hsi.SteeringMode)
				{
					case HSI_TCN_MODE:
					case HSI_TGT_MODE:
					case HSI_WPT_MODE:
					case HSI_GPS_MODE: DoNavSteeringAutoPilot(P);
									   break;

					case HSI_ACL_MODE: break;  // Scott picks this condition up in his AI code
				}
			}
			else
				if (P->AutoPilotMode & PL_AP_ROLL)
					DoBankAngleHoldAutoPilot(P, HoldBankAngle);
			else
				if (P->AutoPilotMode & PL_AP_HDG)
					DoBankAngleHoldAutoPilot(P, 0.0f);



			if (P->AutoPilotMode & PL_AP_ALT_BARO)
					DoAltitudeHoldAutoPilot(P);
			else
				if (P->AutoPilotMode & PL_AP_ALT_RDR)
				{
					float LandHeight = P->Altitude - (P->HeightAboveGround*WUTOFT);
					float NewRdrAlt = HoldAltitudeDelta+LandHeight;
					if (OldRdrAlt == -1.0)
						OldRdrAlt = NewRdrAlt;

					DoRdrAltitudeHoldAutoPilot(P, NewRdrAlt, OldRdrAlt);
					OldRdrAlt = NewRdrAlt;
				}
			else
				if (P->AutoPilotMode & PL_AP_ATTITUDE)
					DoAttitudeHoldAutoPilot(P, HoldAttitude);


			LastInAPMode = 1;
		}
		else
		{
			DoTrimmedFlight(P, LastInAPMode);
			LastInAPMode = 0;
		}
	}

	LastAutoPilotMode = P->AutoPilotMode;
	LastAutoPilotStatus = UFC.APStatus;
}
//************************************************************************************************
void LimitGForces(PlaneParams *P)
{
	if (-P->BfForce.Z > P->TotalWeight*P->AvailGForce)
		 P->BfForce.Z = -(P->TotalWeight*P->AvailGForce);
	else

	if (P->BfForce.Z > P->TotalWeight*3.0f)
		P->BfForce.Z = P->TotalWeight*3.0f;
}
//************************************************************************************************
float GetDensityFactor(PlaneParams *P)
{
	int ClipAlt = (int)P->Altitude;
	if (ClipAlt < 0) ClipAlt = 0;
	if (ClipAlt > 70000) ClipAlt = 70000;

	return ((float)((0.5*Pressure[(int)(ClipAlt/500.0f)])*(float)(P->V*P->V)));
}
//************************************************************************************************
void CalcAeroForces(PlaneParams *P)
{
	// Calculate velocity of speed of sound at altitude(ft/sec)
	double Vss;
	if (P->Altitude <= 0)
		Vss = sqrt((1.4*1716.3*(519.0*(1.0-0.00000703*1.0))));   // make this equation a constant
	else
		Vss = sqrt(1.4*1716.3*(519.0*(1.0-0.00000703*P->Altitude)));

	// Calculate velocity of aircraft (ft/sec)
	if (P->OnGround)
	{
		P->V = sqrt((P->IfVelocity.X*P->IfVelocity.X)+(P->IfVelocity.Z*P->IfVelocity.Z));
	}else
		P->V = sqrt((P->IfVelocity.X*P->IfVelocity.X)+(P->IfVelocity.Y*P->IfVelocity.Y)+(P->IfVelocity.Z*P->IfVelocity.Z));

	if (P->V < 0.0)
		P->V = 0.0;

	P->Knots = P->V*(FTSEC_TO_MLHR*MLHR_TO_KNOTS);

	if ((P->Knots > UFC.StatusMaxTAS) && (P->Knots < 2500))
		UFC.StatusMaxTAS = P->Knots;

	double Vt = sqrt((P->BfLinVel.X*P->BfLinVel.X)+
					 (P->BfLinVel.Y*P->BfLinVel.Y)+
					 (P->BfLinVel.Z*P->BfLinVel.Z));

	double V2 = (Vt*2.0);

	// Calculate dynamic pressure at altitude
	float Q = GetDensityFactor(P);

	// Calculate Mach at Altitude
	if (P->Mach < 0.0f || P->OnGround)
		P->Mach = 0.0f;
	else
		if (Vss != 0)
		 	P->Mach = (float)(P->V/Vss);



	if (P->V < 1.0)
		P->Alpha = DegToRad(0.0);
	else
	{
		if (P->OnGround && OurPlaneTakingOffFromRunway)
			P->BfLinVel.Z = 0.0;

		P->Alpha = (double)atan2(P->BfLinVel.Z, P->BfLinVel.X);
	}

	// special startup case for pitching derivative
	if (P->LastAlpha == -999.0)
		P->LastAlpha = P->Alpha;

	// Calculate Side Slip Angle
	if (P->OnGround || (Vt == 0.0))
		P->Beta = 0.0f;
	else
		P->Beta = (double)asin(P->BfLinVel.Y / Vt);

	// New for F-18
	double S = 500.0;	 // 608
	double c = 13.1;	 // 15.94
	double b = 41.833;	 // 42.83

	double m = P->TotalWeight/32.0;

	double RotVelX = P->BfRotVel.X;
	double RotVelY = P->BfRotVel.Y;
	double RotVelZ = P->BfRotVel.Z;

	double Alpha2 = P->Alpha*P->Alpha;
	double QS = Q*S;											// precompute in plane database
	double b_V2 = (V2 != 0.0) ? (b/V2) : 0.0;
	double QSc = QS*c;											// precompute in plane database
	double QSb = QS*b;											// precompute in plane database

	double CSa  = DegToRad(P->Aileron);
	double CSse = DegToRad(P->SymetricalElevator);				// stick back (climb) = -elevator
	double CSde = DegToRad(P->DifferentialElevator);			// stick right (roll) = +elevator
	double CSr  = DegToRad(P->Rudder);

	double CSf  = DegToRad(P->Flaps);
	double CSsb = DegToRad(P->SpeedBrake);
	double CSg  = ((float)P->LandingGear/255.0f);				// 0 - 1.0 (not rad. or deg.)

	P->BfForce.ZeroLength();	// default conditions for on ground
	P->BfMoment.ZeroLength();	// default conditions for on ground

	//*************************************************************************************************************
	// Compute Coefficient of Lift

	double CL = P->C->CL0 + (P->C->CLa*P->Alpha) + (P->C->CLdde*CSse) + (P->C->CLdf*CSf);

    if (V2 != 0.0) CL += ((P->C->CLq)*((RotVelY*c)/V2));

	float DegAlpha = RadToDeg(P->Alpha);

	if ((DegAlpha > 50.0) || (DegAlpha < -8.0))
		CL = 0.0;

  	if (CL < 0.0) CL = 0.0;

	if (P->IndicatedAirSpeed < 60.0)
		CL = 0.0;

	//*************************************************************************************************************
	// Calculate Drag Forces

	double FabsAlpha = fabs(P->Alpha);
	double TmpAlt = P->Altitude/1000.0;
	double X0Tmp;

	if (TmpAlt < 10.0) TmpAlt = 10.0;
	else
		if (TmpAlt > 50.0) TmpAlt = 50.0;

	// Calculate Parasitic Drag to produce proper topspeed thumbprint
	// This determines the outer maximum thumbprint (2-aim9's and 2-aim120's)

	double X0;
	if (TmpAlt <= 36.0)
		X0 = (0.0012814*TmpAlt) - 0.0835322;
	else
		X0 = -0.0436;

	// This makes it impossible to super cruise (above Mach 1.0 in MIL power 80%)
	// Drag bucket at MACH

	if ((P->Mach > 0.965) && (P->Mach < 1.03))
	{
	 	TmpAlt = P->Altitude/1000.0;
		if (TmpAlt < 5.0) TmpAlt = 5.0;
		else
			if (TmpAlt > 30.0) TmpAlt = 30.0;

		X0Tmp = (-0.00048*TmpAlt) - 0.0656;

		if ((X0Tmp < X0) && ((P->Altitude/1000.0) <= 36.5))
			X0 = X0Tmp;
	}

	// Calculate drag as a function of the Coefficient of Lift Curve

	double Xa = -0.10165*CL;

	// Calculate other contributors to Drag

	double Xb =    P->C->CDb;
	double Xde =  -P->C->CDdde;
	double Xdf =  -P->C->CDdf;

	if ((P->FlapsMode & FLAPS_AUTO))
		Xdf = 0.0;

	double Xdsb = -0.2160;
	double Xdg  = -0.0130 * 2.0;    	 // * 2.0 for effect

	// Calculate weapons drag

	double Xdw = (WeaponsDragWeight) ? P->WeaponDrag : 0.0;

	if (WeaponsDragWeight)
	{
		if (Xdw > 0.002)	// greater than minimal loadout - increase drag curves
		{
			if (P->Altitude < 35000)
				Xdw *= 3.09;
			else
			{
				if (P->Altitude > 40000)
					Xdw *= 6.09;
				else
					Xdw *= (0.0006 * P->Altitude) -17.91;
			}
		}
	}

//	if (Xdw > 0.035) Xdw = 0.035;
	if (Xdw < 0.0) Xdw = 0.0;

	// Buildup Forces

   	P->BfForce.X = QS*(X0
				   		   + (Xa*FabsAlpha)
			    		   - Xdw
			 			   + (Xb*fabs(P->Beta))
				  		   + (Xde*fabs(CSse))
						   + (Xdf*fabs(CSf))
						   + (Xdsb*CSsb)
						   + (Xdg*CSg)
	);

	// Increase drag for blownup plane
	if (P->FlightStatus & PL_OUT_OF_CONTROL)
		P->BfForce.X *= 3.0;

	// check here for multiplayer
	if (!LeftEngineOn)  P->BfForce.X *= 1.25;	// Increase Drag if Engine off
	if (!RightEngineOn) P->BfForce.X *= 1.25;

	//*************************************************************************************************************
	// Calculate Side Forces

	P->BfForce.Y = 0;	// Don't generate any side forces - this makes life WAY easier

	//*************************************************************************************************************
	// Calculate Lift Force

 	double Z0  = -0.02;			// gives use 1deg at 400 and .1 at > mach
 	double Za  = -4.87;

	if ((DegAlpha > 50.0) || (DegAlpha < -8.0) && !P->OnGround)
		Za = 0.0;


	double Zq  = (V2) ? -((c/(V2))*P->C->CLq) : 0.0;
	double Zde = 0.8479;		// this term is pretty insignificant in the grand scheme of things
	double Zdf = -0.93;			// 135 IAS @ 8.1 deg AOA @ FULL_FLAPS (42900 lbs)

	if ((P->FlapsMode & FLAPS_AUTO))
		Zdf = 0.0;

	// add effect of ground effect when altitude <= 1/4 wingspan

//	if (P->AGL < 15) Za *= 2.0;

    P->BfForce.Z = QS*(Z0 + 
		  			  (Za*P->Alpha) +
	                  (Zq*RotVelY) +
					  (Zde*CSse) +
					  (Zdf*CSf));

	LimitGForces(P);
	DoGRelatedStuff(P);

	//*************************************************************************************************************
	// Calculate Pitch Moment

	double Ma;
	double Mq;
	double Mde = -0.6778f+ -0.2;	// this stiffens things up a bit

	// Lower Pitching Moment at low airspeeds
	if ((P->IndicatedAirSpeed > 50.0) && (P->IndicatedAirSpeed < 125.0))
		Mde = (P->IndicatedAirSpeed*-0.011704) + 0.5852;
	else
		if (P->IndicatedAirSpeed <= 50.0)
			Mde = 0.0;

	Ma = -0.30;
    Mq = (V2) ? ((c/V2)*P->C->Cmq) : 0.0;

	double Madot = (V2) ? ((c/V2)*(-3.8*(2.0/3.0))) : 0.0;
	double Da_Dt = (P->Alpha-P->LastAlpha)/P->dt;

	// this is for landing, it pulls nose back down to the ground

	double M0;
	
	//  Added downward force as well when ditching or landing without gears (no ground collisions)
//	M0 = ((P->IndicatedAirSpeed <= 90.0) && P->OnGround && P->Brakes) ? -0.1200 : 0.0;  //was 0.01

	M0 = 0;

	if((P->IndicatedAirSpeed <= 90.0) && P->OnGround && P->Brakes)
		M0 = -0.1200;
	else if((P->FlightStatus & PL_PLANE_DITCHING) || ((P->OnGround) && (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED)) && (P->RightThrustPercent < 25) && (P->LeftThrustPercent < 25)))
		M0 = -0.2400;

	if ((signed short)P->Pitch <= 0)	// don't try to pull past ground, just down to it
		M0 = 0.0;
	
	double Mdf = -0.0271/2.0; 
	
//	double Mdsb = 0.005;
//	double Mlg = -0.0040;

	P->BfMoment.Y = QSc*
				 			(
							M0 
							+
							(Ma*P->Alpha) +
							(Mq*RotVelY) +
							(Mde*CSse) 
							+
				  			(Madot*Da_Dt)
							+
							(Mdf*CSf)
	//						+(Mdsb*CSsb)
	//						+(Mlg*CSg)
							);

	if ((P->IndicatedAirSpeed < 10) && P->OnGround)
		P->BfMoment.Y = 0.0;

	//*************************************************************************************************************
	// Calculate Yawing Moment

	double Nb = 0.204;

	double Nr = (b_V2*-1.4);
//	double Nr = (b_V2*GKValue);

//	double Np = (b_V2*0.010);
//original	double Ndr = -0.0653;			// interconnect - related
	double Ndr = -0.0653 * 2.5;
	double Nda = 0.00103;			// interconnect
//	double Nde = 0.0489;	  		// 


	double Ndthrst;
	#define	Ndt (0.0653/20.0)

	if ((P->Knots >= NOSE_STEERING_HANDOFF) || !P->OnGround)
	{
		if ((LeftEngineOn && !RightEngineOn))
			Ndthrst = Ndt;
		else
		if ((!LeftEngineOn && RightEngineOn))
			Ndthrst = -Ndt;
		else
			Ndthrst = 0.0;

	   	P->BfMoment.Z = QSb*(
					 	 (Nb*P->Beta) +			// directional stability
//						 (Np*RotVelX) +
		  				 (Nr*RotVelZ) +			// necessary - it opposes a continuous yaw moment
		 				 (Ndr*CSr) +			// command yaw moment from rudders
		 				 (Ndthrst)
						 );
	}

	if (P->FlightStatus & PL_OUT_OF_CONTROL)
	   P->BfMoment.Z = 0.0;

	DoYawRelatedStuff(P);

	//*************************************************************************************************************
	// Calculate Rolling Moment

//	double Lb = 0.010;

	double Lb = -0.0859;
	double Lp = (b_V2*-0.26);
	
	double Lr = (b_V2*0.010);
	double Ldr = -P->C->Cldr;

	// Lower Rolling Moment at low airspeeds for differential elevators

	double Lde = 0.1039;
	if ((P->IndicatedAirSpeed > 50.0) && (P->IndicatedAirSpeed < 125.0))
		Lde = (P->IndicatedAirSpeed*0.0013853) - 0.0692667;
	else
		if (P->IndicatedAirSpeed <= 50.0)
			Lde = 0.0;

	// Lower Rolling Moment at low airspeeds from Ailerons

	double Lda = 0.0837;
	if ((P->IndicatedAirSpeed > 50.0) && (P->IndicatedAirSpeed < 125.0))
		Lda = (P->IndicatedAirSpeed*0.001116) - 0.0558;
	else
		if (P->IndicatedAirSpeed <= 50.0)
			Lda = 0.0;

	if (!P->OnGround)
	   	P->BfMoment.X = QSb*(
//							 (Lb*P->Beta) + // beta gives us a rolling component
		                     (Lp*RotVelX) +	// slows down roll - opposes commanded roll
//							 (Lr*RotVelZ) + // tail waging effect
							 (Lda*CSa) 
				//new	 +	// command roll from aileron movement
//							 (Ldr*CSr) +	// command roll to a lesser extent from rudders
				//new			 (Lde*CSde)		// command roll from differential elevators
							 );

	//*************************************************************************************************************
	// Calculate Thrust

	if (P == PlayerPlane)
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
	else	// multiplayer plane
		if (!P->OnGround || !P->Brakes)
			P->BfForce.X += (P->LeftThrust + P->RightThrust);

	//*************************************************************************************************************
	// Calculate Gravity Effects

	if (!(P->FlightStatus & PL_STATUS_CRASHED))
	{
		P->BfForce.X += (P->TotalWeight * -P->Orientation.I.Y);
		P->BfForce.Y += (P->TotalWeight * -P->Orientation.J.Y);
		P->BfForce.Z += (P->TotalWeight * -P->Orientation.K.Y);
	}

	P->LastAlpha = P->Alpha;
	BrakesLastFrame = P->Brakes;
	UFC.OnGroundLastFrame = P->OnGround;
}
//************************************************************************************************
DoBrakes(PlaneParams *P)
{

	float StoppingDistance;
	float Decel = 1.0;

	// X-Force is already computed as a function of thrust and drag

	if (TouchDownOneShot)
	{
		TouchDownInitialPoint = P->WorldPosition;
		TouchDownOneShot = 0;												    // let's be super sure
	}

	// based on current weight compute stopping distance at 175 KCAS
	BrakeDistanceTraveled = (TouchDownInitialPoint/P->WorldPosition)*WUTOFT;	// accurate distance calculation in feet

	// now deccelerate properly with a brake model ala. Mr. Newton in ft/sec^2
	float Weight = (RealisticLandings) ? P->TotalWeight : 40000.0f;
	float Mass = (RealisticLandings) ? P->TotalMass : (40000.0f/32.0f);

	StoppingDistance = (0.187*Weight)-3033;							    // in feet (would be nice to do as a one-shot)
	if (StoppingDistance > 10000.0)
		StoppingDistance = 10000.0;

	if (StoppingDistance != 0.0)
		Decel = (-(293*293)/(2.0*StoppingDistance));  // 174 KCAS = 293 ft/sec.

	int	stillslowing = (P->BfLinVel.X > 0.0);

	if((P->FlightStatus & PL_PLANE_DITCHING) || ((P->OnGround) && (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED)) && (P->RightThrustPercent < 25) && (P->LeftThrustPercent < 25)))
	{
		Decel *= 4.0f;
	}
	else
	{
		stillslowing = 0;
	}

	// from decceleration compute stopping force in lbs.

	P->BfForce.X += (Mass*Decel);					  // add to current thrust
	P->BfLinAccel.X = (P->BfForce.X/Mass);

	if (P->BfLinVel.X <= 0.0)
	{
		P->BfLinAccel.X = 0.0;						  // We are stopped
		if(stillslowing)
		{
			RemoveFireHandleCaller(g_dwPlaneFireHandle);
		}
	}
	else if(stillslowing)
	{
		UpdateCanisterPos(g_dwPlaneFireHandle, P->WorldPosition);
	}
}

//************************************************************************************************
void CalcAeroDynamics(PlaneParams *P)	  
{

	float GroundCoefficient = 0.0f;

	//*********************************************************
	// Calculate Body Frame Linear and Rotational Accelerations
	//*********************************************************

	//*******************************
	// P->BfLinAccel is in (ft/sec^2)
	//*******************************

  	if ((P->OnGround) || (P->FlightStatus & PL_PLANE_DITCHING))
	{
		// stop any side motion
		P->BfLinAccel.Y = 0.0;		
		P->BfLinVel.Y = 0.0;

		// still allow lift to occur
		//P->BfLinAccel.Z = (P->BfForce.Z/P->TotalMass) + (P->BfRotVel.Y * P->BfLinVel.X) - (P->BfRotVel.X * P->BfLinVel.Y);
		  P->BfLinAccel.Z = (P->BfForce.Z/P->TotalMass);

		//P->BfLinAccel.Z = 0.0;

		if ((P->Brakes) || (P->FlightStatus & PL_PLANE_DITCHING) || ((P->OnGround) && (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED)) && (P->RightThrustPercent < 25) && (P->LeftThrustPercent < 25)))
			DoBrakes(P);
		else
			P->BfLinAccel.X = (P->BfForce.X/P->TotalMass) + (P->BfRotVel.Z * P->BfLinVel.Y) - (P->BfRotVel.Y * P->BfLinVel.Z);
	}
	else
	{
		//forward velocity									(heading * sideslip)			pitch * 
		P->BfLinAccel.X = (P->BfForce.X/P->TotalMass) + (P->BfRotVel.Z * P->BfLinVel.Y) - ((P->BfRotVel.Y * P->BfLinVel.Z)*0.50);

		// side force										roll * lift                                 heading * speed			
		P->BfLinAccel.Y = (P->BfForce.Y/P->TotalMass) + ((P->BfRotVel.X * P->BfLinVel.Z)*0.90) - ((P->BfRotVel.Z * P->BfLinVel.X)*2.0); 

			//2nd 		P->BfLinAccel.Y = ((P->BfForce.Y/P->TotalMass)/* + (P->BfRotVel.X * P->BfLinVel.Z)*/ - ((P->BfRotVel.Z * P->BfLinVel.X)*2.0)) * 0.10;  
//sucks		P->BfLinAccel.Y = ((P->BfForce.Y/P->TotalMass)/* + (P->BfRotVel.X * P->BfLinVel.Z)*/ - ((P->BfRotVel.Z * P->BfLinVel.X)*2.0)) * 0.20;  

		//lift force
		if (P->AGL >= 10)
			P->BfLinAccel.Z = (P->BfForce.Z/P->TotalMass) + (P->BfRotVel.Y * P->BfLinVel.X) - (P->BfRotVel.X * P->BfLinVel.Y);
		else  
			P->BfLinAccel.Z = (P->BfForce.Z/P->TotalMass);
	}

	#define MAX_LIN_RATE	2000

	if ((P->BfLinAccel.X < -MAX_LIN_RATE) || (P->BfLinAccel.Y < -MAX_LIN_RATE) ||
		(P->BfLinAccel.Z < -MAX_LIN_RATE) ||

		(P->BfLinAccel.X > MAX_LIN_RATE) || (P->BfLinAccel.Y > MAX_LIN_RATE) ||
		(P->BfLinAccel.Z > MAX_LIN_RATE))
		 
		P->OutOfControlFlight = 1;

	//*****************************************************
	// P->BfRotAccel is in (rad/sec^2) - Inertial Couplings
	//*****************************************************

	if (P->OnGround)
	{
		P->BfRotVel.X = 0.0;				// get rid of any rolling moment
		P->BfRotAccel.X = 0.0;

		if (P->Knots < NOSE_STEERING_HANDOFF)
		{
			P->BfRotVel.Z = 0.0;	   		// get rid of yaw moment if not ground rudder steering
			P->BfRotAccel.Z = 0.0;
		}
		else
		{
			if (P->RudderPedalsCommandedPos == 0.0)
				P->BfRotVel.Z = 0.0;	   	// get rid of yaw moment if not ground rudder steering
			//P->BfRotAccel.Z = (P->BfMoment.Z - P->BfRotVel.X * P->BfRotVel.Y * (P->Stats->Iy - P->Stats->Ix)) / P->Stats->Iz;
			P->BfRotAccel.Z = P->BfMoment.Z / P->Stats->Iz;
		}

		if(((signed short)P->Pitch < 0) || ((P->Knots < 1.0f) && (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))))
		{
			P->BfRotVel.Y = 0.0;
			P->BfRotAccel.Y = 0.0;

			// Orient Plane Flat if it noses dive in (easy mode flight, but also a great overall test)

			P->Orientation.I.Y = 0.0;
			P->Orientation.I.Normalize();

			P->Orientation.K.X =  0.0;
			P->Orientation.K.Y = 1.0;
			P->Orientation.K.Z =  0.0;

			P->Orientation.J = P->Orientation.I;
			P->Orientation.J %= P->Orientation.K;				// Cross product of K with J
			P->Orientation.J.Normalize();	

		}
		else
		{
			// allow pitch up only

		//	P->BfRotAccel.Y = (P->BfMoment.Y + P->BfRotVel.X * P->BfRotVel.Z * (P->Stats->Iz - P->Stats->Ix)) / (P->Stats->Iy/4.0);
			P->BfRotAccel.Y = (P->BfMoment.Y  / P->Stats->Iy);
		}
	}
	else
	{
		// Roll							   // pitch		   // heading
		P->BfRotAccel.X = (P->BfMoment.X + P->BfRotVel.Y * P->BfRotVel.Z * (P->Stats->Iy - P->Stats->Iz)) / (P->Stats->Ix);

		// Pitch						   // roll		   // heading
		P->BfRotAccel.Y = (P->BfMoment.Y + P->BfRotVel.X * P->BfRotVel.Z * (P->Stats->Iz - P->Stats->Ix)) / (P->Stats->Iy/4.0);

		// Heading						   // roll		   // pitch
		P->BfRotAccel.Z = (P->BfMoment.Z + P->BfRotVel.X * P->BfRotVel.Y * (P->Stats->Ix - P->Stats->Iy)) / (P->Stats->Iz);
	}

	// Limit out of bounds rotational accelerations

	#define MAX_ROT_RATE	2000
	float WARN_ROT_RATE	= 1.5f + fFCWarningAdjust;
	float WARN_ROT_RATE_X	= 30.0f + fFCWarningAdjustRoll;

	if ((P->BfRotAccel.X < -MAX_ROT_RATE) || (P->BfRotAccel.Y < -MAX_ROT_RATE) ||
		(P->BfRotAccel.Z < -MAX_ROT_RATE) ||

		(P->BfRotAccel.X > MAX_ROT_RATE) || (P->BfRotAccel.Y > MAX_ROT_RATE) ||
		(P->BfRotAccel.Z > MAX_ROT_RATE))
		 
		P->OutOfControlFlight = 2;
	else if ((P->BfRotAccel.X < -(WARN_ROT_RATE_X)) || (P->BfRotAccel.Y < -WARN_ROT_RATE) ||
		(P->BfRotAccel.Z < -WARN_ROT_RATE) ||

		(P->BfRotAccel.X > (WARN_ROT_RATE_X)) || (P->BfRotAccel.Y > WARN_ROT_RATE) ||
		(P->BfRotAccel.Z > WARN_ROT_RATE))
		 
		P->OutOfControlFlight = 1;
	else
		P->OutOfControlFlight = 0;

	if (P->OutOfControlFlight == 2)
	{
		
		P->BfLinAccel.X = P->BfLinAccel.Y = P->BfLinAccel.Z = 0.0;
		P->BfRotAccel.X = P->BfRotAccel.Y = P->BfRotAccel.Z = 0.0;

		P->BfLinVel.X = P->BfLinVel.Y = P->BfLinVel.Z = 0.0;
		P->BfRotVel.X = P->BfRotVel.Y = P->BfRotVel.Z = 0.0;
		
		CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT | PL_ENGINE_REAR_LEFT, NULL);
		P->OutOfControlFlight = 0;
	}

	// Update Body Frame Velocities

	P->BfLinVel.AddScaledVector(P->dt,P->BfLinAccel);				// LinVel in ft./sec.

	if (((P->BfLinVel.X*FTSEC_TO_KNOTS) > 900.0))					// Limit Top End Speed - Just in case
		P->BfLinVel.X = 900.0/FTSEC_TO_KNOTS;

	P->BfRotVel.AddScaledVector(P->dt,P->BfRotAccel);

	// Ground Rudder Steering

	if (P->OnGround)
	{
		if (P->Knots < 150)
		{
			if (P->Knots < NOSE_STEERING_HANDOFF)
			{
				float RudderPercentage = P->RudderPedalsCommandedPos;
				if (RudderPercentage > 128.0) RudderPercentage = 100.0;
				else if (RudderPercentage < -128.0) RudderPercentage = -100.0;
				RudderPercentage /= 100.0;

				if (RudderPercentage != 0.0)  
				{
					float Knots = P->Knots;
					if (Knots > 5.0) Knots = 30.0;
					P->BfRotVel.Z += ((RudderPercentage*(Knots))*P->dt);	  // rads/sec^2
				}
			}
		}
	}

	// Update the Orientation vector (inertial frame referenced)

	Vector3D Vel_i,Vel_j;

	Vel_i.SetValues(P->BfRotVel.Z,P->Orientation.J);			//Scaled vector initialization
	Vel_i.AddScaledVector(-P->BfRotVel.Y,P->Orientation.K);

	Vel_j.SetValues(P->BfRotVel.X,P->Orientation.K);			//Scaled vector initialization
	Vel_j.AddScaledVector(-P->BfRotVel.Z,P->Orientation.I);		// Vel_j in ft./sec.

	P->Orientation.I.AddScaledVector(P->dt,Vel_i);
	P->Orientation.I.Normalize();

	P->Orientation.J.AddScaledVector(P->dt,Vel_j);
	P->Orientation.J.Normalize();

	P->Orientation.K = P->Orientation.I;
	P->Orientation.K %= P->Orientation.J;						//Cross product of K with J
	P->Orientation.K.Normalize();

	// Transform Velocity in Body Frame to Inertial Frame Velocity
	P->IfVelocity.X = P->BfLinVel.X * P->Orientation.I.X +
 					  P->BfLinVel.Y * P->Orientation.J.X +
					  P->BfLinVel.Z * P->Orientation.K.X;

	// Up and Down velocity relative to the inertial reference frame
	P->IfVelocity.Y = P->BfLinVel.X * P->Orientation.I.Y +  	// forward nose velocity
					  P->BfLinVel.Y * P->Orientation.J.Y +  	// side force velocity
					  P->BfLinVel.Z * P->Orientation.K.Y;

	P->IfVelocity.Z = P->BfLinVel.X * P->Orientation.I.Z +
					  P->BfLinVel.Y * P->Orientation.J.Z +
  					  P->BfLinVel.Z * P->Orientation.K.Z;

	// Move Airplane in Inertial Frame of Reference based on updated Inertial Velocities

	P->LastWorldPosition = P->WorldPosition;
	P->WorldPosition.AddScaledVector((float)P->dt * (1.0/WUTOFT),P->IfVelocity);

	P->DistanceMoved = (float)(P->WorldPosition - P->LastWorldPosition);  /* quick distance calculation */

	P->HeightAboveGround = CalcHeightAboveGround(P);	// this could clip WPos.Y
	P->AGL = (unsigned int)((P->HeightAboveGround * WUTOFT) - 3.0);

	P->Altitude = (float)(P->WorldPosition.Y * WUTOFT);

	if (P->Altitude > UFC.StatusMaxAlt)
		UFC.StatusMaxAlt = P->Altitude;

	if (P->Altitude < UFC.StatusMinAlt)
		UFC.StatusMinAlt = P->Altitude;
}
//************************************************************************************************
void CalcAttitude(PlaneParams *P)
{
	#define RAD_TO_ANGLE (double)10430.3783505

	if (P->Status & PL_NEED_ATTITUDE)
	{
		if ((P->OnGround == 1) && !(P->Status & PL_CARRIER_REL))
		{
	  		P->Heading = (unsigned int)(-atan2(P->Orientation.I.X ,-P->Orientation.I.Z) * RAD_TO_ANGLE);
			P->Pitch   = (unsigned int)( asin(P->Orientation.I.Y) * RAD_TO_ANGLE);

			P->Roll = (P->Orientation.K.Y >= 0.0) ? 32768 : 0;
			P->Attitude.SetRPH(P->Roll, P->Pitch, P->Heading);
		}
		else
		{
			FPointDouble z,y,x;

			/* ------------------------------------------3/10/98 9:50AM--------------------------------------------
			 * we'll assume this vector is valid  (z vector)
			 * ----------------------------------------------------------------------------------------------------*/
			z.SetValues(-P->Orientation.I.X,-P->Orientation.I.Y,-P->Orientation.I.Z);
			z.Normalize();

			P->Attitude.m_Data.RC.R0C2 = (float)z.X;
			P->Attitude.m_Data.RC.R1C2 = (float)z.Y;
			P->Attitude.m_Data.RC.R2C2 = (float)z.Z;

			/* ------------------------------------------3/10/98 9:50AM--------------------------------------------
			 * now we'll calculate the x vector using the y and z vector
			 * ----------------------------------------------------------------------------------------------------*/
			x.SetValues(-P->Orientation.K.X,-P->Orientation.K.Y,-P->Orientation.K.Z);

			x %= z;
			x.Normalize();

			P->Attitude.m_Data.RC.R0C0 = (float)x.X;
			P->Attitude.m_Data.RC.R1C0 = (float)x.Y;
			P->Attitude.m_Data.RC.R2C0 = (float)x.Z;

			/* ------------------------------------------3/10/98 9:51AM--------------------------------------------
			 * now we'll get the y vector from the x and z vectors
			 * ----------------------------------------------------------------------------------------------------*/

			y = z;
			y %= x;

			P->Attitude.m_Data.RC.R0C1 = (float)y.X;
			P->Attitude.m_Data.RC.R1C1 = (float)y.Y;
			P->Attitude.m_Data.RC.R2C1 = (float)y.Z;


			P->Attitude.GetRPH(&P->Roll,&P->Pitch,&P->Heading);

		}
	}
	P->Status &= ~PL_NEED_ATTITUDE;

	// Compute Indicated Airspeed
	P->IndicatedAirSpeed = FtSecTASToIAS(P->V, P->Altitude);

	P->IfHorzVelocity = sqrt((P->IfVelocity.X*P->IfVelocity.X)+
							 (P->IfVelocity.Z*P->IfVelocity.Z));

}
//************************************************************************************************
void ControlPlanes(void)
{
	PlaneParams *P;

	//  Helo added SRE
	//  This stuff HAS to be outside of the main loop because wingmen and other planes need
	//  to reference the true pitch of a plane.
	for (P=Planes; P<=LastPlane; P++)
	{
		if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED))
		{
			if(((TimeExcel) || (DeltaTicks >= 200)) && (P->Status & PL_AI_DRIVEN))
			{
				P->DoAttitude(P);
			}

			if((P->AI.iAIFlags2 & AI_FAKE_AOA) || (P->AI.iAIFlags2 & AILANDING) || (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
					|| ((pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (P->Knots < 120.0f)))
			{
				P->Pitch -= P->AI.AOADegPitch;
			}

			if ((P->OnGround == 2) && (P->Status & PL_AI_DRIVEN))
			{
				if (!P->PlaneCopy)	/*on carrier*/
					P->PlaneCopy = GetNewCarrierPlane(P);
			}
			else
			if (P->PlaneCopy)
			{
				free(P->PlaneCopy);
				P->PlaneCopy = NULL;
			}
		}
		else
		{
			if (P->PlaneCopy)
			{
				free(P->PlaneCopy);
				P->PlaneCopy = NULL;
			}

			if ((P->AI.iAICombatFlags2 & AI_CHECK_TAKEOFF) && !(P->FlightStatus & PL_STATUS_CRASHED))
			{
				if(P->AI.lTimer2 >= 0)
				{
					P->AI.lTimer2 -= DeltaTicks;
					if(P->AI.lTimer2 < 0)
					{
						if(P->AI.Behaviorfunc == CTWaitingForLaunch)
						{
							CTAddToLaunchQueue(P);
						}
					}
				}
				else if(!AIWaitingForTakeOffFlag(P))
				{
					if(P->AI.Behaviorfunc == CTWaitingForLaunch)
					{
						CTAddToLaunchQueue(P);
					}
				}
			}
		}
	}

	for (P=Planes; P<=LastPlane; P++)
	{
		if (!(P->Status & PL_ACTIVE))  continue;

		if (P->FlightStatus & PL_STATUS_CRASHED)  continue;

		if ((P->Status & PL_DEVICE_DRIVEN) || (P->Status & PL_COMM_DRIVEN))
		{
			P->DoGetPlayerInputs(P);
		   	AIUpdateFormation(P);
		}
		else if (P->Status & PL_AI_DRIVEN)
		{
			if (P == PlayerPlane)
			{
				// If our plane is AI controlled thru casual mode autopilot and the AP is ON
				// then read stick to determine if an override is called for

				if((P->AI.Behaviorfunc == CTWaitingForLaunch) || (P->AI.Behaviorfunc == CTDoCarrierLaunch) || (P->AI.Behaviorfunc == AICarrierTrap))
				{
					if (JoyCap1.wCaps & JOYCAPS_HASZ)
						P->CommandedThrottlePos = (65535-GetJoyZ()) * (100.0/65535.0);	// 0.0 --> 100.0

					P->DoGetPlayerInputs(P);
				}

#ifdef __DEMO__
				if ((UFC.APStatus == ON) &&  ((!RealisticAPs) || (P->Status & PL_AI_DRIVEN)))
#else
				if ((UFC.APStatus == ON) &&  (!RealisticAPs))
#endif
				{
					int Offset = (32768>>TimeExcel);
					P->StickX = (((GetJoyPosX()>>TimeExcel)+AileronTrimOffset)-Offset) * (1.0/256.0);
					P->StickY = (((GetJoyPosY()>>TimeExcel)+NoseTrimOffset)-Offset) * (1.0/256.0);
		 			CheckAPDeselect(P);
//					CheckEasyFlightStuff(P);
				}


				AIResetPercents(P);
#ifdef __DEMO__
			   	P->AI.Behaviorfunc(P);
#else
				if(P->AI.Behaviorfunc == AIFlyTakeOff)
				{
					AIFlyTakeOff(P);
				}
				else if(P->AI.Behaviorfunc == AIFlyTankerFormation)
				{
					AIFlyTankerFormation(P);
				}
				else if(P->AI.Behaviorfunc == AITankerDisconnect)
				{
					AITankerDisconnect(P);
				}
				else if(P->AI.Behaviorfunc == CTWaitingForLaunch)
				{
					CTWaitingForLaunch(P);
				}
				else if(P->AI.Behaviorfunc == CTDoCarrierLaunch)
				{
					CTDoCarrierLaunch(P);
				}
				else if(P->AI.Behaviorfunc == AICarrierTrap)
				{
					AICarrierTrap(P);
				}
				else if(P->AI.Behaviorfunc == AIMovePlaneToDeArmZone)
				{
					AIMovePlaneToDeArmZone(P);
				}
				else if(P->AI.Behaviorfunc == AIWaitForTrapOrBolter)
				{
					AIWaitForTrapOrBolter(P);
				}
				else if(P->AI.Behaviorfunc == AIUnhookFromTrapWire)
				{
					AIUnhookFromTrapWire(P);
				}
				else if(P->AI.Behaviorfunc == AIMoveToParkingSpace)
				{
					AIMoveToParkingSpace(P);
				}
				else if((P->AI.Behaviorfunc == AIFlyToDME10) || (P->AI.Behaviorfunc == AIFlyToDME6) ||
						(P->AI.Behaviorfunc == AIFlyToDME3) || (P->AI.Behaviorfunc == AIMoveToParkingSpace) ||
						(P->AI.Behaviorfunc == AIFlyCarrierFinal) || (P->AI.Behaviorfunc == AIFlyBolter) ||
						(P->AI.Behaviorfunc == AIQuickCarrierLanding))
				{
				   	P->AI.Behaviorfunc(P);
				}
				else
				{
					AIFlyFormation(P);
				}
#endif
				AIControlSurfaces(P);
			}
			else
			{
				AIResetPercents(P);
#if 0
				if((!MultiPlayer) || (AIInPlayerGroup(P)))
				{
				   	P->AI.Behaviorfunc(P);
				}
#else
			   	P->AI.Behaviorfunc(P);
#endif
				AIControlSurfaces(P);
			}
		}

		/* I'm sure this shouldn't go here! */
		switch (P->Type->EngineConfig)
		{
			case TWO_REAR_ENGINES:
				if (P->SystemsStatus & PL_ENGINE_REAR_RIGHT)
				{
					P->RightThrustPercent = 0.0f;
					if ((P == PlayerPlane) && RightEngineOn)
						ShutRightEngineDown();
				}

				if (P->SystemsStatus & PL_ENGINE_REAR_LEFT)
				{
					P->LeftThrustPercent = 0.0f;
					if ((P == PlayerPlane) && LeftEngineOn)
						ShutLeftEngineDown();
				}
				break;

			case ONE_REAR_ENGINE:
				if (P->SystemsStatus & PL_ENGINE_REAR)
					P->RightThrustPercent = P->LeftThrustPercent = 0.0f;

				break;
		}
	}

	//  This stuff HAS to be outside of the main loop because wingmen and other planes need
	//  to reference the true last frame pitch of a plane.  Planes earlier in the list would
	//  see a different 'last' value then plane later in the list.
	//  Also need to reset the helicopter and landing plane pitches.

	for (P=Planes; P<=LastPlane; P++)
	{
		if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED))
		{
			P->AI.LastPitch = P->Pitch;
			//  Helo added SRE
			if((P->AI.iAIFlags2 & AI_FAKE_AOA) || (P->AI.iAIFlags2 & AILANDING) || (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
					|| ((pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (P->Knots < 120.0f)))
			{
				P->Pitch += P->AI.AOADegPitch;
			}

			P->AI.LastRoll =  P->Roll;
			P->AI.LastHeading = P->Heading;
			P->AI.LastSymetricalElevator = P->SymetricalElevator;
		}
	}
}
//************************************************************************************************
void MovePlanes(void)
{
	float airate;
	int sendnetstuff = 0;

#if 0
	if(!(GameLoop % 10))
	{
		sendnetstuff = 1;
	}
#else
	sendnetstuff = (GameLoop % 10);
#endif


	PlaneParams *P = &Planes[0];
	while (P <= LastPlane)
	{
		if (P->Status & PL_ACTIVE)

		{
			if (!(P->FlightStatus & PL_STATUS_CRASHED))
			{
				if ((P->Status & PL_DEVICE_DRIVEN) || (P->Status & PL_COMM_DRIVEN))
				{
				  	P->DoControlSim(P);
   					P->DoCASSim(P);
					Do50HzPlaneBasedWeaponStuff(P);
					CheckReportingStats(P);
//					if (EasyFlight)
//						CheckEasyFlightStuff(P);
				}
	   			
				if (P == PlayerPlane)
					CheckEasyFlightStuff(P);

				if ((P == PlayerPlane) && (P->Status & PL_AI_DRIVEN))
				{
					DetectWeaponFire(P);
					Do50HzPlaneBasedWeaponStuff(P);
				}

				if((!(P->FlightStatus & PL_OUT_OF_CONTROL)) && ((P->Status & PL_AI_DRIVEN) || (P->DoForces == CalcAeroForcesSimple)))
				{
					P->dt += HIGH_FREQ;

					switch(P->UpdateRate)
					{
						case LOW_AERO:
							airate = LOW_FREQ;
							break;
						case MED_AERO:
							airate = MED_FREQ;
							break;
						default:
							airate = HIGH_FREQ;
							break;
					}

					if((P->dt >= airate) || (TickCount < 40))
					{
						if (P->OnGround == 2)
						{
							if (P->PlaneCopy)
							{
								CopyWorldPlaneToCarrierPlane(P);
								P->DoForces(P->PlaneCopy);
								P->DoPhysics(P->PlaneCopy);
								CopyCarrierPlaneToWorldPlane(P);
							}
							else
							{
			  					P->DoForces(P);
    				 			P->DoPhysics(P);
							}
						}
						else
						{
			  				P->DoForces(P);
    				 		P->DoPhysics(P);
						}

						P->Status |= PL_NEED_ATTITUDE;
						DoSmoke(P);
						P->dt = 0;
					}

					if((!((sendnetstuff + (P - Planes)) % 10)) && (MultiPlayer))
					{
						NetPutAIPos(P);
					}
				}
				else
				{
					P->dt = HIGH_FREQ;

					if (P->OnGround == 2) /*on carrier*/
					{
						if (P->PlaneCopy)
						{
							CopyWorldPlaneToCarrierPlane(P);
							P->DoForces(P->PlaneCopy);
							P->DoPhysics(P->PlaneCopy);
							CopyCarrierPlaneToWorldPlane(P);
						}
						else
						{
			  				P->DoForces(P);
    				 		P->DoPhysics(P);
						}
					}
					else
					{
			  			P->DoForces(P);
    				 	P->DoPhysics(P);
					}

					P->Status |= PL_NEED_ATTITUDE;
					DoSmoke(P);
				}

				UpdatePlaneLightPositions(P);
			}
			else
			{
				FreePlaneLights(P);

				if (P->PlaneCopy)
				{
					free(P->PlaneCopy);
					P->PlaneCopy = NULL;
				}

				if (P->Smoke[SMOKE_BURNING])
	   				((SmokeTrail *)P->Smoke[SMOKE_BURNING])->Update();
			}
			P->DoAttitude(P);
		}
		P++;
	}

	if ((PlayerPlane->Status & PL_ACTIVE) && (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED)) )
		FuelDumpSmoke(NULL);

}
/**************************************************************************************/
void HUDtextLg( int X, int Y, PCSTR fmt, ... );
void GregAeroDebugInfo(PlaneParams *P)
{
	static float LinXMin, LinXMax = 0.0;
	static float LinYMin, LinYMax = 0.0;
	static float LinZMin, LinZMax = 0.0;

	static float RotXMin, RotXMax = 0.0;
	static float RotYMin, RotYMax = 0.0;
	static float RotZMin, RotZMax = 0.0;
	
	if (P->BfLinAccel.X < LinXMin) LinXMin = P->BfLinAccel.X;
	if (P->BfLinAccel.Y < LinYMin) LinYMin = P->BfLinAccel.Y;
	if (P->BfLinAccel.Z < LinZMin) LinZMin = P->BfLinAccel.Z;

	if (P->BfLinAccel.X > LinXMax) LinXMax = P->BfLinAccel.X;
	if (P->BfLinAccel.Y > LinYMax) LinYMax = P->BfLinAccel.Y;
	if (P->BfLinAccel.Z > LinZMax) LinZMax = P->BfLinAccel.Z;


	if (P->BfRotAccel.X < RotXMin) RotXMin = P->BfRotAccel.X;
	if (P->BfRotAccel.Y < RotYMin) RotYMin = P->BfRotAccel.Y;
	if (P->BfRotAccel.Z < RotZMin) RotZMin = P->BfRotAccel.Z;

	if (P->BfRotAccel.X > RotXMax) RotXMax = P->BfRotAccel.X;
	if (P->BfRotAccel.Y > RotYMax) RotYMax = P->BfRotAccel.Y;
	if (P->BfRotAccel.Z > RotZMax) RotZMax = P->BfRotAccel.Z;


	HUDtextLg(10,10, "LinAccelX %f	- %f, %f",P->BfLinAccel.X, LinXMin, LinXMax);
	HUDtextLg(10,20, "LinAccelY %f	- %f, %f",P->BfLinAccel.Y, LinYMin, LinYMax);
	HUDtextLg(10,30, "LinAccelZ %f	- %f, %f",P->BfLinAccel.Z, LinZMin, LinZMax);

	HUDtextLg(10,50, "RotAccelX %f	- %f, %f",P->BfRotAccel.X, RotXMin, RotXMax);
	HUDtextLg(10,60, "RotAccelY %f	- %f, %f",P->BfRotAccel.Y, RotYMin, RotYMax);
	HUDtextLg(10,70, "RotAccelZ %f	- %f, %f",P->BfRotAccel.Z, RotZMin, RotZMax);

//	HUDtextLg(10,10, "Thrust %f", P->LeftThrust);
//	HUDtextLg(10,20, "OnGround %d", P->OnGround);
//	HUDtextLg(10,30, "FPS %f", FrameRate);
//	HUDtextLg(10,40, "Rudder Command %f", P->RudderPedalsCommandedPos);

//	HUDtextLg(10,10, "Requested G %f", P->RequestedG);
//	HUDtextLg(10,20, "Taking Off %d", OurPlaneTakingOffFromRunway);
//	HUDtextLg(10,60, "SE %f", P->SymetricalElevator);
//	HUDtextLg(10,70, "Aileron %f", P->Aileron);

//	int TmpAileron1 = (P->Aileron - P->SBAileronOffset+P->FLPSAileronOffset);

//	HUDtextLg(10,50, "Aileron     %f",P->Aileron);
//	HUDtextLg(10,60, "SBAileron   %f",P->SBAileronOffset);
//	HUDtextLg(10,70, "FLPSAileron %f",P->FLPSAileronOffset);
//	HUDtextLg(10,80, "FLPSCommand %f",P->FlapsCommandedPos);

//	HUDtextLg(10,40, "AGL %d", P->AGL);

//	HUDtextLg(10,50, "Lift %f", P->BfForce.Z);
//	HUDtextLg(10,60, "Lin Accel Z %f", P->BfLinAccel.Z);

//	HUDtextLg(10,70, "Pitch Moment %f", P->BfMoment.Y);
//	HUDtextLg(10,80, "Rot Accel Y %f", P->BfRotAccel.Y);

//	if (P->OnGround && (P->TerrainType == TT_AIRPORT) && (P->IndicatedAirSpeed <= 48.0))
//	HUDtextLg(10,90, "Airport Terrain %d", (P->TerrainType == TT_AIRPORT));
//	HUDtextLg(10,100, "IAS %f", P->IndicatedAirSpeed);
//	HUDtextLg(10,110, "OnGround %d", P->OnGround);
//	HUDtextLg(10,120, "Elevator %f", P->SymetricalElevator);


//  P->StickX*P->RollRatio

//	HUDtextLg(10,80, "StickX %f", P->StickX);
//	HUDtextLg(10,90, "Roll Ratio %f", P->RollRatio);
//	HUDtextLg(10,130, "GKValue  %f", GKValue);




//	HUDtextLg(10,60, "Total Fuel %f", P->TotalFuel);
//	HUDtextLg(10,70, "Center Drop Fuel %f", P->CenterDropFuel);
//	HUDtextLg(10,80, "WingDropFuel Fuel %f", P->WingDropFuel);

//	P->TotalWeight = P->AircraftDryWeight + P->TotalFuel;

//	HUDtextLg(10,50, "ThrottlePos %f", P->LeftThrottlePos);
//	HUDtextLg(10,60, "TimeExcel %d", TimeExcel);			  

//	if (WeaponsDragWeight)
//		HUDtextLg(10,110, "WeaponsDragWeight-ACTIVE");
//	else
//		HUDtextLg(10,110, "WeaponsDragWeight-NOT ACTIVE");

//	HUDtextLg(10,80, "P->AGL %d", P->AGL);

//	HUDtextLg(10,70, "b_V2 %f", b_V2);
//	HUDtextLg(10,80, "Total W %f", P->TotalWeight);

//	HUDtextLg(10,90, "G %f", P->GForce);

//	HUDtextLg(10,110, "Lift %f", P->BfForce.Z);
//	HUDtextLg(10,120, "Pitch Moment %f", P->BfMoment.Y);

//	HUDtextLg(10,130, "MA %f", gk1);

 
//	HUDtextLg(10,170, "IAS %f", P->IndicatedAirSpeed);

/*

	HUDtextLg(10,180, "TAS %f", KnotsIASToTAS(P->IndicatedAirSpeed , P->Altitude));
	HUDtextLg(10,190, "Elevator %f", P->SymetricalElevator);
 	HUDtextLg(10,200, "Requested G %f", P->RequestedG);
	HUDtextLg(10,210, "AvailGForce %f", P->AvailGForce);
	HUDtextLg(10,220, "StickY %f", P->StickY);
	HUDtextLg(10,230, "GForce %f", P->GForce);

	HUDtextLg(10,240, "RequestedAOA %f", P->NewRequestedAOA);
*/
	switch (P->FlapsMode)
	{
		case FLAPS_FULL :
			HUDtextLg(10,250, "Flaps FULL");
			break;
		case FLAPS_HALF :
			HUDtextLg(10,250, "Flaps HALF");
			break;
		case FLAPS_AUTO :
			HUDtextLg(10,250, "Flaps AUTO");
			break;
	};

	/*
	HUDtextLg(10,260, "TEFlaps %f",P->Flaps);
	HUDtextLg(10,270, "LEFlaps %f",P->LEFlaps);

	if (P->AutoPilotMode & PL_AP_ALT_BARO)
		HUDtextLg(450,10, "AP_BARO");
	if (P->AutoPilotMode & PL_AP_ALT_RDR)
		HUDtextLg(450,20, "AP_ALT_RDR");
	if (P->AutoPilotMode & PL_AP_ATTITUDE)
		HUDtextLg(450,30, "AP_ATTITUDE");

	if (P->AutoPilotMode & PL_AP_CPL)
		HUDtextLg(450,40, "AP_CPL");
	if (P->AutoPilotMode & PL_AP_ROLL)
		HUDtextLg(450,50, "AP_ROLL");
	if (P->AutoPilotMode & PL_AP_HDG)
		HUDtextLg(450,60, "AP_HDG");

*/

/*
	HUDtextLg(450,20, "RequestedAOAPercentage %f", P->RequestedAOAPercentage);
	HUDtextLg(450,30, "NewRequestedAOA        %f", P->NewRequestedAOA);

	HUDtextLg(450,40, "SB commanded           %f", P->SpeedBrakeCommandedPos);
	HUDtextLg(450,50, "SBRudderOffset         %f", P->SBRudderOffset);
	HUDtextLg(450,60, "SBAileronOFfset        %f", P->SBAileronOffset);
	HUDtextLg(450,70, "Rudder                 %f", P->Rudder);
*/
	void WriteData(PlaneParams *P);
	WriteData(P);
}

//*********************************************************************
void WriteData(PlaneParams *P)
{
	static FILE *fp=NULL;
	static int FileOpened = 0;
	char szTxt[260];

	/*
	if (GetVkStatus(RDRLeft) && !fp)	// open file
	{
		fp = fopen("c:\\winmcad\\greg2.prn","w+t");
		FileOpened = 1;
	}

	if (FileOpened)
	{

		sprintf(szTxt,"%f %f %f %f %f %f %f %f %f %f %f\n",P->Altitude, gkvar1, RadToDeg(P->Alpha), gkvar2, gkvar3, 
			                                      P->SymetricalElevator, gkvar4, gkvar5, gkvar6, gkvar7, P->IndicatedAirSpeed);
		fwrite(szTxt,strlen(szTxt),1,fp);

	}

	if (GetVkStatus(RDRRight) && fp)	// close file
	{

		fclose(fp);
		fp = 0;
		FileOpened = 0;	
	}
  */

	if (GetVkStatus(RDRLeft))
		GKValue -= 0.1;
		
	if (GetVkStatus(RDRRight))
		GKValue += 0.1;
}












