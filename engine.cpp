#include "F18.h"
#include "spchcat.h"
#include "gamesettings.h"
#include "keystuff.h"
#include "particle.h"

#include "snddefs.h"

#include "engine.h"


extern AvionicsType Av;

//************************************************************************************************
float GetCommandedThrust(PlaneParams *P, float ThrustPercent)
{
	// Thrust Model for One PW-220 engine

	float H,Dh,Dm,Rm,Cdh,S,T,TMil,TIdle,TMax,Thrust;
	int I,M;
	float ClipAltitude;

	ClipAltitude = P->Altitude;
	if (ClipAltitude > 50000.0f) ClipAltitude = 50000.0f;
	if (ClipAltitude < 0.0f) ClipAltitude = 0.0f;

	H = (ClipAltitude * 0.0001f);
	I = (int)H;
	if (I >= 5) I = 4;
	Dh = H-(float)I;

	float TempMach = P->Mach;

//	if (TempMach > 1.0f) TempMach = 1.0f;
	if (TempMach > 0.8f) TempMach = 0.8f;
	if (TempMach < 0.0f) TempMach = 0.0f;

	Rm = (TempMach*(float)5.0);
	M = (int)Rm;
	if (M >= 5) M = 4;
	Dm = Rm-(float)M;

	Cdh = 1.0f-Dh;

	S = (ATD[1][M][I]*Cdh) + (ATD[1][M][I+1]*Dh);
	T = (ATD[1][M+1][I]*Cdh) + (ATD[1][M+1][I+1]*Dh);
	TMil = S + ((T-S)*Dm);

	if (ThrustPercent < 50.0)
	{
		S = (ATD[0][M][I]*Cdh) + (ATD[0][M][I+1]*Dh);
		T = (ATD[0][M+1][I]*Cdh) + (ATD[0][M+1][I+1]*Dh);
		TIdle = S + ((T-S)*Dm);
		Thrust = TIdle + ((TMil-TIdle)*ThrustPercent*0.02f);

	}else{

		S = (ATD[2][M][I]*Cdh) + (ATD[2][M][I+1]*Dh);
		T = (ATD[2][M+1][I]*Cdh) + (ATD[2][M+1][I+1]*Dh);
		TMax = S + ((T-S)*Dm);
		Thrust = TMil + ((TMax-TMil)*(ThrustPercent-50.0f)*0.02f);
	}

	if (Thrust < 0.0f) Thrust = 0.0f;
	if (P->TotalFuel == 0.0f) Thrust = 0.0f;

//	if (EasyFlight && !P->OnGround)
//	{
//		Thrust += (Thrust*0.10);
//		if (P->Altitude > 60000)
//			Thrust = 0.0;
//	}
//	else
//	{
		if (P->Altitude > 65000)
			Thrust = 0.0;
//	}

	if (P->OnGround)
	{
		if (P->ThrottlePos < 20.0)
			Thrust = 0.0;
	}

	if (Thrust > 21890.0)
		Thrust = 21890.0;

	return(Thrust * 1.10f);		//for GE F/A-1 Engine 22,000lb max static in A/B at sea level
 //	return(Thrust);				//for PW F-15  Engine 20,000lb max static in A/B at sea level
}
//************************************************************************************************
void SetEngineWarnings(long TurnOffFlags, long TurnOnFlags, long WarningFlags)
{
	PlayerPlane->SystemInactive &= ~TurnOnFlags;

	for(int cnt = 0; cnt < 32; cnt ++)
	{
		if(WarningFlags & (1<<cnt))
		{
			if (TurnOffFlags)
			{
				if(!(PlayerPlane->SystemInactive & (1<<cnt)))
				{
					DisplayWarning(MASTER_CAUTION, ON, 0);
				}
				SetMasterWarning(cnt);
			}
			else
				if ((TurnOnFlags) && (!(PlayerPlane->DamageFlags & (1<<cnt))))
					ClearMasterWarning(cnt);
		}
	}
	PlayerPlane->SystemInactive |= TurnOffFlags | PlayerPlane->DamageFlags;
}
//************************************************************************************************
void LeftEngineOutEffects(int On)
{
	long warnflags = 0;
	long turnoffflags = 0;

	turnoffflags |= (DAMAGE_BIT_LO_ENGINE)|(DAMAGE_BIT_L_ENG_CONT)|(DAMAGE_BIT_L_OIL_PRESS)|(DAMAGE_BIT_L_BURNER)|(DAMAGE_BIT_L_FUEL_PUMP)|(DAMAGE_BIT_L_GEN)|(DAMAGE_BIT_L_MAIN_HYD);
	warnflags |= (1<<WARN_L_ENG_CONT)|(1<<WARN_L_OIL_PRESS)|(1<<WARN_L_BURNER)|(1<<WARN_L_FUEL_PUMP)/* temp for now|(1<<WARN_L_GEN)*/;

	SetEngineWarnings((!On)?turnoffflags:0,(!On)?0:turnoffflags, warnflags);
}
//************************************************************************************************
void RightEngineOutEffects(int On)
{
	long warnflags = 0;
	long turnoffflags = 0;

	turnoffflags |= (DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_R_ENG_CONT)|(DAMAGE_BIT_R_OIL_PRESS)|(DAMAGE_BIT_R_BURNER)|(DAMAGE_BIT_R_FUEL_PUMP)|(DAMAGE_BIT_R_GEN)|(DAMAGE_BIT_R_MAIN_HYD)|(DAMAGE_BIT_EMER_HYD);
	warnflags |= (1<<WARN_R_ENG_CONT)|(1<<WARN_R_OIL_PRESS)|(1<<WARN_R_BURNER)|(1<<WARN_R_FUEL_PUMP)/*|(1<<WARN_R_GEN)*/;

	SetEngineWarnings((!On)?turnoffflags:0,(!On)?0:turnoffflags, warnflags);
}
//************************************************************************************************
void ShutLeftEngineDown(void)
{
	PlayerPlane->EngineStatus &= ~PL_LEFT_ENGINE_ON;
	LeftEngineOutEffects(OFF);
	if (!RightEngineOn)
		StopEngineSounds();
}
//************************************************************************************************
void ShutRightEngineDown(void)
{
	PlayerPlane->EngineStatus &= ~PL_RIGHT_ENGINE_ON;
	RightEngineOutEffects(OFF);
	if (!LeftEngineOn)
		StopEngineSounds();
}
//************************************************************************************************
void ShutBothEnginesDown(void)
{
	PlayerPlane->EngineStatus = 0;
	FlameOutStopEngineSounds();
	LeftEngineOutEffects(OFF);
	RightEngineOutEffects(OFF);
}
//************************************************************************************************
void ToggleLeftEngineStatus(void)
{
	if (LeftEngineOn)
		ShutLeftEngineDown();
	else
	{
		if (!PlayerPlane->EngineStatus)
			StartEngineSounds();
		PlayerPlane->EngineStatus |= PL_LEFT_ENGINE_ON;
		LeftEngineOutEffects(ON);
	}
}
//************************************************************************************************
void ToggleRightEngineStatus(void)
{
	if (RightEngineOn)
		ShutRightEngineDown();
	else
	{
		if (!PlayerPlane->EngineStatus)
			StartEngineSounds();
		PlayerPlane->EngineStatus |= PL_RIGHT_ENGINE_ON;
		RightEngineOutEffects(ON);
	}
}
//************************************************************************************************
float GetFuelFlow(PlaneParams *P, float ThrottlePercent)
{
	float FuelFlow;		// Lb./Min.

	// Compute Max. Potential Fuel flow at Alttitude and Mach
	int AltIndx = round(P->Altitude/5000);
	if (AltIndx > 10) AltIndx = 10;

	float ClipMach = P->Mach;
	if (ClipMach > 2.0) ClipMach = 2.0;

	int MachIndx = (int)(((ClipMach-0.8f)*10.0f));
	if (MachIndx < 0) MachIndx = 0;
	else
		if (MachIndx > 12) MachIndx = 12;

	if (ClipMach > 0.8)
	{
		// Integrate between Mach
		float Prop = fmod(ClipMach, 0.1f);

		float Sample1 = FF[AltIndx][MachIndx];
		float Sample2 = FF[AltIndx][MachIndx+1];

		float Delta	= (Sample2-Sample1)*Prop;
		FuelFlow = Sample1 + Delta;
	}
	else
	{
		FuelFlow = FF[AltIndx][MachIndx] * ClipMach;
	}

	// Compute Throttle Percentage that Gives Max Potential Fuel Flow Calculated above
	if (ThrottlePercent <= 80.0)
	{
		float CalcPercent = ((ClipMach*0.442f)+0.036f)*100.0;

		float ClipCommandThrust = ThrottlePercent;
		if (ClipCommandThrust < 2.0)
			ClipCommandThrust = 2.0;

		if (ClipCommandThrust < CalcPercent)
			FuelFlow *= (ClipCommandThrust/CalcPercent);
	}
	else
	{
		// Afterburner Effect
//		if (!EasyFlight)
			FuelFlow *= (ThrottlePercent-60.0)/20.0;			// 100% 2*FuelFlow
	}
	if (FuelFlow < 20.0) FuelFlow = 20.0;

	FuelFlow *= 0.80;											// F/A-18 20% more fuel effiecient than F-15

	return (FuelFlow);
}
//************************************************************************************************
float CalcFuelFlow(PlaneParams *P,float ThrottlePercent,float Altitude,int UseMinMach)
{
	int ThrustIndex,AltIndex;
	float Range,MinPercent;
	float FuelFlow,Min,Max;

	// being extra cautious by forcing indexes to be in range
	AltIndex = round(P->Altitude/5000);
	if (AltIndex > 14)
	    AltIndex = 14;
	if (AltIndex < 0)
	    AltIndex = 0;

	// being extra cautious by forcing percent to be in range
	ThrottlePercent /= 100.0;
	if (ThrottlePercent > 1.0)
	    ThrottlePercent = 1.0;
	if (ThrottlePercent < 0.0)
	    ThrottlePercent = 0.0;

	if (ThrottlePercent <= 0.8)
	{
		  ThrustIndex = 1;
	    Range  = 0.8;
		  MinPercent = 0.0;
	}
	else if((ThrottlePercent > 0.8) && (ThrottlePercent <= 0.9))
	{
		ThrustIndex = 2;
	  Range  = 0.1;
		MinPercent = 0.8;
	}
	else
	{
		ThrustIndex = 3;
		Range  = 0.1;
		MinPercent = 0.9;
	}

	if(UseMinMach)
	{
		Min = FFLowMach[AltIndex][ThrustIndex-1];
	  Max = FFLowMach[AltIndex][ThrustIndex];
	}
	else
	{
		Min = FFHighMach[AltIndex][ThrustIndex-1];
	  Max = FFHighMach[AltIndex][ThrustIndex];
	}

	// interpolate and return val
	float Frac = (ThrottlePercent - MinPercent)/Range;
	FuelFlow = Min + (Max - Min)*Frac;

	return(FuelFlow);
}
//************************************************************************************************
float GetSingleEngineFuelFlowAndThrottlePos(PlaneParams *P, int EngineSpooling,float ThrottlePercent,float *ThrottlePos,float CommandedThrottlePos,float *CurFuelFlow,float *GoalFuelFlow,float *NumStepsToThrottleGoal,float Altitude,int *LastTimeFuelFlowCalc)
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

	if (*LastTimeFuelFlowCalc > 0)
	{
		*GoalFuelFlow = FuelFlow;

		int   Dt   = GameLoopInTicks - *LastTimeFuelFlowCalc;
		float Dist = fabs(*GoalFuelFlow - *CurFuelFlow);
		float Frac;

		if (Range != 0.0)      // div by zero check
			Frac = Dist/Range;
		else
			Frac = 0.0;

		// float Time = (4.0 + (ClipAlt/70000)*(12.25))*Frac; For F-15
		float Time = (2.0 + (ClipAlt/70000)*(6.15))*Frac; 	// faster spools for F/A-18

		float NumSteps  = Time/((float)Dt/1000.0);
		*NumStepsToThrottleGoal = NumSteps; // for use by other functions that are based on throttle spool up
		if (NumSteps < 1.0)
	    {
     		*CurFuelFlow = *GoalFuelFlow;
			if (!EngineSpooling)
				*ThrottlePos = CommandedThrottlePos;
		}
		else
		{
			*CurFuelFlow = *CurFuelFlow + (*GoalFuelFlow - *CurFuelFlow)/NumSteps;
			if(!EngineSpooling)
				*ThrottlePos    = *ThrottlePos  + (CommandedThrottlePos-*ThrottlePos)/NumSteps;
		}
	}
	else
	{
		*LastTimeFuelFlowCalc = GameLoopInTicks;  // first time through
		*GoalFuelFlow = FuelFlow;
		*CurFuelFlow = FuelFlow;
		*ThrottlePos = CommandedThrottlePos;
		*NumStepsToThrottleGoal = 1.0f;  // for use by other functions that are based on throttle spool up
	}

	return (*CurFuelFlow);
}

//************************************************************************************************

int DebugFuel = -1;
int LastDebugFuel;

void CalcTemp(PlaneParams *P,float NumStepsToThrottleGoal)
{
	int   Dt;
	float Dist,Frac,Time,NumSteps;
	float goto_temp;

    // engine is damaged or turned off
	float dt = (GameLoopInTicks - UFC.LastTimeFuelFlowCalc);
	float d;

	if (!LeftEngineOn)
		goto_temp = UFC.OffTemp;
	else
	{
		if (P->LeftThrustPercent > 75)
			goto_temp = 350.0 + ((910 - 350.0f)*P->LeftThrustPercent)/100.0;
		else
			goto_temp = 350.0 + ((820 - 350.0f)*P->LeftThrustPercent)/100.0;
	}
	d = ((goto_temp - EMD_lTemp)*dt)/(2.0f*1000.0f);
	EMD_lTemp += d;



	if (!RightEngineOn)
		goto_temp = UFC.OffTemp;
	else
	{
		if (P->RightThrustPercent > 75)
			goto_temp = 350.0 + ((910 - 350.0f)*P->RightThrustPercent)/100.0;
		else
			goto_temp = 350.0 + ((820 - 350.0f)*P->RightThrustPercent)/100.0;
	}
	d = ((goto_temp - EMD_rTemp)*dt)/(2.0f*1000.0f);
	EMD_rTemp += d;



    if (lFireFlags & (HEAT_UP_LEFT_ENGINE | HEAT_UP_AMAD_LEFT))
    {
  		float TimeToFire = (lFireFlags & HEAT_UP_LEFT_ENGINE) ? lLeftTimeToFire : lLAMADTimeToFire;
		float OrgTimeToFire = (lFireFlags & HEAT_UP_LEFT_ENGINE) ? lLeftOrgTimeToFire : lLOrgAMADTimeToFire;
		EMD_lTemp = UFC.MilMaxTemp + (1500.0 - UFC.MilMaxTemp)*(1.0 - ((float)TimeToFire/OrgTimeToFire));
		if( (!UFC.LeftHeatUpWarningGiven) && (EMD_lTemp >= 1000.0) )
		{
			UFC.LeftHeatUpWarningGiven = TRUE;
		}
		else
			if (EMD_lTemp < 1000.0)
				UFC.LeftHeatUpWarningGiven = FALSE;
	}

	if (lFireFlags & (HEAT_UP_RIGHT_ENGINE | HEAT_UP_AMAD_RIGHT))
	{
   		float TimeToFire = (lFireFlags & HEAT_UP_RIGHT_ENGINE) ? lRightTimeToFire : lRAMADTimeToFire;
  		float OrgTimeToFire = (lFireFlags & HEAT_UP_LEFT_ENGINE) ? lRightOrgTimeToFire : lROrgAMADTimeToFire;
		float Frac;
		if (OrgTimeToFire != 0)
			Frac = (float)TimeToFire/OrgTimeToFire;
		else
			Frac = 0.0;
		EMD_rTemp = UFC.MilMaxTemp + (1500.0 - UFC.MilMaxTemp)*(1.0 - ((float)TimeToFire/OrgTimeToFire));
		if ((!UFC.RightHeatUpWarningGiven) && (EMD_rTemp >= 1000.0f) )
		{
			UFC.RightHeatUpWarningGiven = TRUE;
		}
		else
			 if (EMD_rTemp < 1000.0)
				 UFC.RightHeatUpWarningGiven = FALSE;
	}

	// Quick Hack limit fix
	if (EMD_lTemp > 1500.0f)
		EMD_lTemp = 1500.0f;

	if (EMD_rTemp > 1500.0f)
		EMD_rTemp = 1500.0f;


}


//************************************************************************************************

void CalcAmadTemp(PlaneParams *P,float NumStepsToThrottleGoal)
{
	int   Dt;
	float Dist,Frac,Time,NumSteps;

    	// engine is damaged or turned off
	if (!LeftEngineOn)
	{
			Dt   = GameLoopInTicks - UFC.LastTimeFuelFlowCalc;
			Dist = fabs(EMD_Amad_lTemp -  UFC.OffTemp);
			Frac = Dist/(575.0 - UFC.OffTemp);
			Time = 300.0*Frac;  // 300 sec = 5 min
			NumSteps  = Time/((float)Dt/1000.0);

			EMD_Amad_lTemp = EMD_Amad_lTemp + ((UFC.OffTemp - EMD_Amad_lTemp)/NumSteps);
	}
	else
   		EMD_Amad_lTemp = 575.0 + ((UFC.MilMaxTemp - 575.0)*(P->LeftThrottlePos/100.0));

	if (EMD_Amad_lTemp < 400.0f)
		EMD_Amad_lTemp = 400.0f;

    if (!RightEngineOn) // engine is damaged or turned off
    {
				Dt   = GameLoopInTicks - UFC.LastTimeFuelFlowCalc;
				Dist = fabs(EMD_Amad_rTemp - UFC.OffTemp);
				if((575.0 - UFC.OffTemp) != 0.0)
					Frac = Dist/(575.0 - UFC.OffTemp);
				else
					Frac = 0.0;
				Time = 300.0*Frac;	//  why in the heck was this 15000.0f????
				NumSteps  = Time/((float)Dt/1000.0);

				if (NumSteps < 1.0)
					NumSteps = 1.0;
				EMD_Amad_rTemp = EMD_Amad_rTemp + ((UFC.OffTemp - EMD_Amad_rTemp)/NumSteps);
    }
	else
   		EMD_Amad_rTemp = 575.0 + ((UFC.MilMaxTemp - 575.0)*(P->RightThrottlePos/100.0));

	if (EMD_Amad_rTemp < 400.0f)
		EMD_Amad_rTemp = 400.0f;

    if (lFireFlags & (HEAT_UP_AMAD_LEFT))
    {
  		float TimeToFire = lLAMADTimeToFire;
		  float OrgTimeToFire = lLOrgAMADTimeToFire;
		  EMD_Amad_lTemp = UFC.MilMaxTemp + (1500.0 - UFC.MilMaxTemp)*(1.0 - ((float)TimeToFire/OrgTimeToFire));
			if( (!UFC.LeftAmadHeatUpWarningGiven) && (EMD_Amad_lTemp >= 1000.0) )
			{
				UFC.LeftAmadHeatUpWarningGiven = TRUE;
			}
			else
				if (EMD_Amad_lTemp < 1000.0)
					UFC.LeftAmadHeatUpWarningGiven = FALSE;
	}

	if (lFireFlags & (HEAT_UP_AMAD_RIGHT))
	{
   		float TimeToFire = lRAMADTimeToFire;
  		float OrgTimeToFire = lROrgAMADTimeToFire;
		float Frac;
		if (OrgTimeToFire != 0)
			Frac = (float)TimeToFire/OrgTimeToFire;
		else
			Frac = 0.0;
		EMD_Amad_rTemp = UFC.MilMaxTemp + (1500.0 - UFC.MilMaxTemp)*(1.0 - ((float)TimeToFire/OrgTimeToFire));
		if ((!UFC.RightAmadHeatUpWarningGiven) && (EMD_Amad_rTemp >= 1000.0f) )
		{
			UFC.RightAmadHeatUpWarningGiven = TRUE;
		}
		else
			 if (EMD_Amad_rTemp < 1000.0)
				 UFC.RightAmadHeatUpWarningGiven = FALSE;
	}
}

//************************************************************************************************

void CalcPsi(PlaneParams *P,float NumStepsToThrottleGoal)
{
	float PsiToUse;

	UFC.PsiAdjust = 0;
	float ThrottlePos;

	if ((LeftEngineOn) || (RightEngineOn) )
		PsiToUse = UFC.Psi;
	else
		PsiToUse = 0;

	if (LeftEngineOn)
	{
		if (RightEngineOn)
			ThrottlePos = (P->LeftThrottlePos >= P->RightThrottlePos) ? P->LeftThrottlePos : P->RightThrottlePos;
		else
			ThrottlePos = P->LeftThrottlePos;
	}

	if (RightEngineOn)
	{
		if (LeftEngineOn)
			ThrottlePos = (P->LeftThrottlePos >= P->RightThrottlePos) ? P->LeftThrottlePos : P->RightThrottlePos;
	    else
			ThrottlePos = P->RightThrottlePos;
	}

	if ((ThrottlePos >= 80.0) && (UFC.Psi < 30) )
	{
		if (NumStepsToThrottleGoal < 1.0)
			NumStepsToThrottleGoal = 1.0;
		UFC.Psi = UFC.Psi + ((30 - UFC.Psi)/NumStepsToThrottleGoal);
	}

	if (P->Altitude < 35000)
		UFC.PsiAdjust = (ThrottlePos/100.0)*15.0;
	else if (P->Altitude > 50000)
		UFC.PsiAdjust = (ThrottlePos)*5.0;

	// if g force <= 0 psi or damaged, should go down to zero within 5 seconds


	float   Dt = (float)(GameLoopInTicks - UFC.LastTimeFuelFlowCalc);
	float Dist;
	float NumSteps;

	if (Dt)
	{
		Dt = Dt/1000.0;

		// first the left oil pressure
		if ((P->GForce < 0.0) || (PlayerPlane->SystemInactive & DAMAGE_BIT_L_OIL_PRESS) || !(LeftEngineOn))
			Dist = 0;
		else
		{
		 	Dist = (PsiToUse + UFC.PsiAdjust);

			if ((EMD_lOlio >= UFC.Psi + UFC.PsiAdjust) || (PsiToUse <= 0) )
				Dt = 6.0f;
		}

		Dist -= EMD_lOlio;

		NumSteps = Dist*Dt*(1.0f/6.0);

		if (NumSteps)
		{
			EMD_lOlio += NumSteps;

			if(EMD_lOlio < 0)
				EMD_lOlio = 0;
		}

		// now the right oil pressure

		if ((P->GForce < 0.0) || (PlayerPlane->SystemInactive & DAMAGE_BIT_R_OIL_PRESS) || !(RightEngineOn))
			Dist = 0;
		else
		{
		 	Dist = (PsiToUse + UFC.PsiAdjust);

			if ((EMD_rOlio >= UFC.Psi + UFC.PsiAdjust) || (PsiToUse <= 0) )
				Dt = 6.0f;
		}

		Dist -= EMD_rOlio;

		NumSteps = Dist*Dt*(1.0/6.0f);

		if (NumSteps)
		{
			EMD_rOlio += NumSteps;

			if(EMD_rOlio < 0)
				EMD_rOlio = 0;
		}
	}

//	if ((P->GForce >= 0.0) && (PlayerPlane->SystemInactive & DAMAGE_BIT_L_OIL_PRESS))
//	{
//		if (((EMD_lOlio < UFC.Psi + UFC.PsiAdjust) || (EMD_rOlio < UFC.Psi + UFC.PsiAdjust)) && (PsiToUse > 0) )
//			{

//    			if ((EMD_lOlio < UFC.Psi + UFC.PsiAdjust))
//	  			{
//	      			int   Dt   = GameLoopInTicks - UFC.LastTimeFuelFlowCalc;
//	      			float Dist = (PsiToUse + UFC.PsiAdjust) - EMD_lOlio;
//        			float Frac = Dist/30.0;
//        			float Time = 5.0*Frac;  // 5 sec
//        			float NumSteps  = Time/((float)Dt/1000.0);

//	      			if(NumSteps != 0.0)
//	        			EMD_lOlio += Dist/NumSteps;
//   				}

//	  			if ((EMD_rOlio < UFC.Psi + UFC.PsiAdjust))
//	  			{
//	      			int   Dt   = GameLoopInTicks - UFC.LastTimeFuelFlowCalc;
//	      			float Dist = (PsiToUse + UFC.PsiAdjust) - EMD_rOlio;
//        			float Frac = Dist/30.0;
//        			float Time = 5.0*Frac;  // 5 sec
//        			float NumSteps  = Time/((float)Dt/1000.0);

//	      			if(NumSteps != 0.0)
//           			EMD_rOlio += Dist/NumSteps;
//	   			}
//		}
//		else
//		{
//     		EMD_lOlio = EMD_rOlio = PsiToUse + UFC.PsiAdjust;
//		}
//	}
//	else
//	{
//		int   Dt   = GameLoopInTicks - UFC.LastTimeFuelFlowCalc;
//		float Dist = fabs((LeftEngineOn ? EMD_lOlio : EMD_rOlio));
//		float Frac = Dist/30.0;
//		float Time = 5.0*Frac;  // 5 sec
//		float NumSteps  = Time/((float)Dt/1000.0);

//		EMD_lOlio -= EMD_lOlio/NumSteps;
//		EMD_rOlio -= EMD_rOlio/NumSteps;

//		if(EMD_lOlio < 0) EMD_lOlio = 0;
//		if(EMD_rOlio < 0) EMD_rOlio = 0;
//	}

	// note, after this function is called, the EMD_lOlio and EMD_rOlio are set to 0.0 if
	// the respective engines are destroyed.
}
//************************************************************************************************

void CalcRPMs(PlaneParams *P,float NumStepsToLeftThrottleGoal,float NumStepsToRightThrottleGoal)
{

	float LeftFrac,RightFrac;
	float Dt = GameLoopInTicks - UFC.LastTimeFuelFlowCalc;


	if ((UFC.LeftSpoolState == SPOOL_DOWN) || (UFC.LeftSpoolState == SPOOL_UP))
	{
		EMD_lSpin += 3.1*( (UFC.LeftSpoolState == SPOOL_DOWN) ? -1.0 : 1.0)*(Dt/1000.0);
		if(EMD_lSpin < 0.0) EMD_lSpin = 0.0;
	}
	else
	{

  		if (P->LeftCommandedThrottlePos < 80.0)
  			LeftFrac = P->LeftCommandedThrottlePos/80.0;
	  	else
			  LeftFrac = 1.0;

		float LeftGoalRpm = LeftFrac*(39.0) + 61.0;
  		float LeftRpmToUse;

		LeftRpmToUse  = EMD_lSpin;

		if (NumStepsToLeftThrottleGoal < 1.0)
  			LeftRpmToUse = LeftRpmToUse + (LeftGoalRpm - LeftRpmToUse);
  		else
  			LeftRpmToUse = LeftRpmToUse + ((LeftGoalRpm - LeftRpmToUse)/NumStepsToLeftThrottleGoal);

  		EMD_lSpin = LeftRpmToUse;
	}

	if ((UFC.RightSpoolState == SPOOL_DOWN) || (UFC.RightSpoolState == SPOOL_UP) )
	{
		EMD_rSpin += 3.1*( (UFC.RightSpoolState == SPOOL_DOWN) ? -1.0 : 1.0)*(Dt/1000.0);
		if(EMD_rSpin < 0.0) EMD_rSpin = 0.0;
	}
	else
	{
   		if (P->RightCommandedThrottlePos < 80.0)
  			RightFrac = P->RightCommandedThrottlePos/80.0;
  		else
			RightFrac = 1.0;

  		float RightGoalRpm = RightFrac*(39.0) + 61.0;
  		float RightRpmToUse;

  		RightRpmToUse = EMD_rSpin;

  		if (NumStepsToRightThrottleGoal < 1.0)
  			RightRpmToUse = RightRpmToUse + (RightGoalRpm - RightRpmToUse);
  		else
  			RightRpmToUse = RightRpmToUse + ((RightGoalRpm - RightRpmToUse)/NumStepsToRightThrottleGoal);

  		EMD_rSpin = RightRpmToUse;
	}

	// note, after this function is called, the EMD_lSpin and EMP_rSpin are set to 0.0 if
	// the respective engines are destroyed.
}

float JLMFuelFlow;

//************************************************************************************************
void ConsumeFuel(PlaneParams *P, unsigned int DeltaTicks)
{
	static int OneShot = 0;
	static DWORD s_dwToneWarning = 0;
	float LeftThrottlePercent,RightThrottlePercent;
	float NumStepsToLeftThrottleGoal; // used by Fuel flow,throttle,RPM, and temperature calculations
	float NumStepsToRightThrottleGoal; // used by Fuel flow,throttle,RPM, and temperature calculations
	int cnt;
	float workLeftFuelFlow, workRightFuelFlow;

	if(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	LeftThrottlePercent  = P->LeftThrottlePos;
	RightThrottlePercent = P->RightThrottlePos;

	float LeftFuelFlowLow,LeftFuelFlowHigh,LeftFuelFlow;
	float RightFuelFlowLow,RightFuelFlowHigh,RightFuelFlow;
	int TempAlt = P->Altitude;

	if (TempAlt > 70000.0) TempAlt = 70000.0;

	TempAlt = TempAlt - (TempAlt % 5000);

	LeftFuelFlowLow  = GetSingleEngineFuelFlowAndThrottlePos(P,(UFC.LeftSpoolState != SPOOL_OFF),P->LeftCommandedThrottlePos,&(P->LeftThrottlePos),P->LeftCommandedThrottlePos,&UFC.CurLeftFuelFlow,&UFC.GoalLeftFuelFlow,&NumStepsToLeftThrottleGoal,(float)TempAlt,&UFC.LastTimeFuelFlowCalc);
	LeftFuelFlowHigh = GetSingleEngineFuelFlowAndThrottlePos(P,(UFC.LeftSpoolState != SPOOL_OFF),P->LeftCommandedThrottlePos,&(P->LeftThrottlePos),P->LeftCommandedThrottlePos,&UFC.CurLeftFuelFlow,&UFC.GoalLeftFuelFlow,&NumStepsToLeftThrottleGoal,(float)TempAlt + 5000.0,&UFC.LastTimeFuelFlowCalc);
	LeftFuelFlow     = LeftFuelFlowLow + ( ((P->Altitude - TempAlt)/5000.0)*(LeftFuelFlowHigh - LeftFuelFlowLow));

	if (LeftThrottlePercent == RightThrottlePercent)
	{
		RightFuelFlow = LeftFuelFlow;
		NumStepsToRightThrottleGoal = NumStepsToLeftThrottleGoal;
	}
	else
	{
		RightFuelFlowLow  = GetSingleEngineFuelFlowAndThrottlePos(P,(UFC.RightSpoolState != SPOOL_OFF),P->RightCommandedThrottlePos,&(P->RightThrottlePos),P->RightCommandedThrottlePos,&UFC.CurRightFuelFlow,&UFC.GoalRightFuelFlow,&NumStepsToRightThrottleGoal,(float)TempAlt,&UFC.LastTimeRightFuelFlowCalc);
		RightFuelFlowHigh = GetSingleEngineFuelFlowAndThrottlePos(P,(UFC.RightSpoolState != SPOOL_OFF),P->RightCommandedThrottlePos,&(P->RightThrottlePos),P->RightCommandedThrottlePos,&UFC.CurRightFuelFlow,&UFC.GoalRightFuelFlow,&NumStepsToRightThrottleGoal,(float)TempAlt+5000.0,&UFC.LastTimeRightFuelFlowCalc);
  	RightFuelFlow     = RightFuelFlowLow + ( ((P->Altitude - TempAlt)/5000.0)*(RightFuelFlowHigh - RightFuelFlowLow));
	}

	CalcRPMs(P,NumStepsToLeftThrottleGoal,NumStepsToRightThrottleGoal);
	CalcPsi (P,NumStepsToLeftThrottleGoal);
	CalcTemp(P,NumStepsToLeftThrottleGoal);
	CalcAmadTemp(P,NumStepsToLeftThrottleGoal);

	UFC.LastTimeFuelFlowCalc = GameLoopInTicks;      // this var is used for time between ticks
	UFC.LastTimeRightFuelFlowCalc = GameLoopInTicks; // this is set for the first time in GetSingleFuelFlowAndThrottlePos

	UFC.LeftFuelFlowRate  = LeftFuelFlow*(0.74);
	UFC.RightFuelFlowRate = RightFuelFlow*(0.74);

	if((UnlimitedFuel) || (P->Status & PL_AI_DRIVEN))
	{
		workLeftFuelFlow = 0;
		workRightFuelFlow = 0;
	}
	else
	{
		workLeftFuelFlow = UFC.LeftFuelFlowRate;
		workRightFuelFlow = UFC.RightFuelFlowRate;
	}

	// add 1300 ppm if fuel dump is on
	if(Av.Fuel.DumpFuel)
	{
		if( (PlayerPlane->WingDropFuel + PlayerPlane->CenterDropFuel + PlayerPlane->InternalFuel) <= Av.Fuel.BingoVal)
		{
			Av.Fuel.DumpFuel = FALSE;
		}
		else if((PlayerPlane->WingDropFuel + PlayerPlane->CenterDropFuel + PlayerPlane->InternalFuel) <= 0)
		{
			Av.Fuel.DumpFuel = FALSE;
		}
		else if(UnlimitedFuel && (P->TotalFuel < 5000))
		{
			Av.Fuel.DumpFuel = FALSE;
		}
		else
		{
	  		UFC.LeftFuelFlowRate  += 1300;
			UFC.RightFuelFlowRate += 1300;
			workLeftFuelFlow += 1300;
			workRightFuelFlow += 1300;
		}
	}

	if(!LeftEngineOn)
		workLeftFuelFlow = UFC.LeftFuelFlowRate = 0;

	if(!RightEngineOn)
		workRightFuelFlow = UFC.RightFuelFlowRate = 0;

	if ((!LeftEngineOn) && (UFC.LeftSpoolState != SPOOL_DOWN))
	{
		UFC.LeftSpoolState = SPOOL_DOWN;
	}
	else
	{
		if (UFC.LeftSpoolState == SPOOL_DOWN)
		{
			if ((EMD_lSpin <= 0.0) && (LeftEngineOn))
			{
			  UFC.LeftSpoolStartTime = GameLoopInTicks;
			  UFC.LeftSpoolState = SPOOL_UP;
			}
		}
		else if (UFC.LeftSpoolState == SPOOL_UP)
		{
			if (GameLoopInTicks - UFC.LeftSpoolStartTime > 20000)
			{
				UFC.LeftSpoolStartTime = -1;
				UFC.LeftSpoolState = SPOOL_OFF;
			}
		}
	}

	if ((!RightEngineOn) && (UFC.RightSpoolState != SPOOL_DOWN))
	{
		UFC.RightSpoolState = SPOOL_DOWN;
	}
	else
	{
		if (UFC.RightSpoolState == SPOOL_DOWN)
		{
			if ((EMD_rSpin <= 0.0) && (RightEngineOn))
			{
				UFC.RightSpoolStartTime = GameLoopInTicks;
				UFC.RightSpoolState = SPOOL_UP;
			}
		}
		else if (UFC.RightSpoolState == SPOOL_UP)
		{
			if (GameLoopInTicks - UFC.RightSpoolStartTime > 20000)
			{
				UFC.RightSpoolStartTime = -1;
				UFC.RightSpoolState = SPOOL_OFF;
			}
		}
	}

 	if (!LeftEngineOn)
	{
		EMD_lOlio = 0.0f;
	}

	if (!RightEngineOn)
	{
		EMD_rOlio = 0.0f;
	}



//	if (!UnlimitedFuel && (LeftEngineOn || RightEngineOn))
	if ((LeftEngineOn || RightEngineOn))
	{
		float LeftFF;
		float RightFF;

		if(!((UnlimitedFuel) || (P->Status & PL_AI_DRIVEN)))
		{
			LeftFF  = (LeftEngineOn)  ? UFC.LeftFuelFlowRate  : 0;
			RightFF = (RightEngineOn) ? UFC.RightFuelFlowRate : 0;
		}
		else
		{
			LeftFF  = (LeftEngineOn)  ? workLeftFuelFlow  : 0;
			RightFF = (RightEngineOn) ? workRightFuelFlow : 0;
		}

		float LbsConsumed = ((LeftFF + RightFF)/60.0f)*(float)(DeltaTicks/1000.0f);

		if (P->SystemInactive & DAMAGE_BIT_FUEL_TANKS)	 // 2% fuel leak
			LbsConsumed *= 1.02;

		UFC.StatusTotalFuelUsed += LbsConsumed;

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

	  	if ((P->TotalFuel <= 0.0) ||
			(((P->SystemInactive & (DAMAGE_BIT_L_FUEL_PUMP | DAMAGE_BIT_R_FUEL_PUMP)) ==
								  (DAMAGE_BIT_L_FUEL_PUMP | DAMAGE_BIT_R_FUEL_PUMP)	)
			 && ((P->Altitude > 30000) || (P->ThrottlePos > 80.0))))
		{
			ShutBothEnginesDown();
		}

		if (P->TotalFuel < Av.Fuel.BingoVal)

		{
			if (!g_bBingoFuel)
			{
				SndQueueSpeech(SND_BETTY_BINGO, CHANNEL_BETTY,g_iBaseBettySoundLevel,2);
				SndServiceSound();
				g_bBingoFuel=TRUE;
			}
		}
		else
			if (P->TotalFuel >= Av.Fuel.BingoVal)
			{
				g_bBingoFuel = FALSE;
			}

		if (P->TotalFuel < 2000.0f)
		{
			if (!OneShot)
			{
				DisplayWarning(MASTER_CAUTION, ON, 0);
				SetMasterWarning(WARN_FUEL_LOW);

				for(cnt = 0; cnt < 2; cnt ++)
				{
					SndQueueSpeech (SND_BETTY_FUEL, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
					SndQueueSpeech (SND_BETTY_LOW, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
				}
				SndServiceSound();
				OneShot = 1;
			}
		}
		else
			OneShot = 0;		// for multiple passes (from mission to mission)
	}
	else
		P->TotalFuel = (P->InternalFuel + P->CenterDropFuel + P->WingDropFuel);

	if((!MultiPlayer) || (PlayerPlane == P))
	{
//		if (EasyFlight)
//			P->TotalWeight = 40000.0;
//		else
//		{
//			if (!WeaponsDragWeight)
//				P->TotalWeight = 37800.0 + P->TotalFuel;
//			else
				P->TotalWeight = P->AircraftDryWeight + P->TotalFuel;

			if (P->TotalWeight < 30564.0)
				P->TotalWeight = 30564.0;

//			P->TotalWeight = 42900.0;

			//*********
			//* This is a weight test for the F18 model
			//* dryweight/no weapons = 30,564
			//* max internal fuel = 14,400

			//* below set using 3/4 fuel =

//			P->TotalWeight = 42900.0;
//		}
	}

	P->TotalMass = P->TotalWeight/32.0;

	// Let's now compute new inertia values based on updated weight

//temp	float Weight = P->TotalWeight;
//temp	if (Weight < 36000.0) Weight = 36000.0;
//temp	else
//temp		if (Weight > 80000.0) Weight = 80000.0;
//temp
//temp	P->Stats->Ix = (0.243*Weight) + 16870.0f;
//temp	P->Stats->Iy = (1.176*Weight) + 125000.0f;
//temp	P->Stats->Iz = (1.355*Weight) + 139000.0f;

	//**********
	// F/A-18 dry weight values

//	P->Stats->Ix = 22337;
	P->Stats->Ix = 28337;
	P->Stats->Iy = 120293;
	P->Stats->Iz = 138945;

	// nozzle calcs
	float lOpen;
	float rOpen;

//	if(P->Status & PL_AI_DRIVEN)
//	{
		if (LeftEngineOn)
		{
			if (P->LeftThrustPercent < 80)
		 		lOpen = 30.0f+(80.0f-P->LeftThrustPercent)*(50.0f/80.0f);	// 80% --> 16%
			else lOpen = 30.0f + (P->LeftThrustPercent-80)*(67.0/20.0);	// 16% --> 75%
		}
		else
		{
			if (P->IndicatedAirSpeed < 60.0f)
				lOpen = frand()*P->IndicatedAirSpeed*(7.0f/60.0f);
			else
				lOpen = rand()&7;

			lOpen += 92.0f;
		}

		if (RightEngineOn)
		{
			if (P->RightThrustPercent < 80)
		 		rOpen = 30.0f + (80.0f-P->RightThrustPercent)*(50.0f/80.0f);	// 80% --> 16%
			else rOpen = 30.0f + (P->RightThrustPercent-80)*(67.0/20.0);	// 16% --> 75%
		}
		else
		{
			if (P->IndicatedAirSpeed < 60.0f)
				rOpen = frand()*P->IndicatedAirSpeed*(7.0f/60.0f);
			else
				rOpen = rand()&7;

			rOpen += 92.0f;
		}
//	}
//	else
//	{
//		if (P->LeftThrustPercent < 80)
//			 lOpen = 0.8*(100-P->LeftThrottlePos);	// 80% --> 16%
//		else lOpen = 3.0*(P->LeftThrottlePos-75 );	// 16% --> 75%

//		if (P->RightThrustPercent < 80)
//			 rOpen = 0.8*(100-P->RightThrottlePos);	// 80% --> 16%
//		else rOpen = 3.0*(P->RightThrottlePos-75 );	// 16% --> 75%
//	}

	EMD_lOpen += ((float)lOpen - EMD_lOpen)*0.035f*sqrtDeltaTicks;
	EMD_rOpen += ((float)rOpen - EMD_rOpen)*0.035f*sqrtDeltaTicks;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float GetSingleEngineFuelFlowForFPas(PlaneParams *P,float ThrottlePercent)
{
	float FuelFlow;		// Lb./Min.
	float MaxFuelFlow,MinFuelFlow;

	float ClipMach = P->Mach;
	if (ClipMach > 2.0) ClipMach = 2.0;

	float ClipAlt = P->Altitude;
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
	}
	else if(ClipMach >= 1.1)
	{
		FuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,FALSE);
	}
	else  // interpolate between the two tables
	{
		MinFuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,TRUE);
		MaxFuelFlow = CalcFuelFlow(P,ThrottlePercent,ClipAlt,FALSE);
		FuelFlow    = MinFuelFlow + (((ClipMach - 0.8)/0.3)*(MaxFuelFlow - MinFuelFlow));
	}

	return(FuelFlow);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CalcFuelBurn(float FuelAmount, float *Endurance, float *DistInMiles)
{
  float LeftFF  = GetSingleEngineFuelFlowForFPas(PlayerPlane,PlayerPlane->LeftCommandedThrottlePos);
  float RightFF = GetSingleEngineFuelFlowForFPas(PlayerPlane,PlayerPlane->RightCommandedThrottlePos);
  float TotalFF = LeftFF + RightFF;

	float MinutesLeft = FuelAmount/TotalFF;

	float Range = ( MinutesLeft * (PlayerPlane->IndicatedAirSpeed*60) )*FTTONM;

	*Endurance   = MinutesLeft;
	*DistInMiles = Range;
}