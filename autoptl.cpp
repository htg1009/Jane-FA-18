#include "F18.h"
#include "spchcat.h"
#include "gamesettings.h"
#include "keystuff.h"
#include "particle.h"
#include "autoplt.h"

extern AvionicsType Av;
extern float GKValue;
extern float GKValue1;
extern float GKValue2;


//************************************************************************************************
//************************************************************************************************
void DoCplAltitudeHoldAutoPilot(PlaneParams *P)
{
	float TmpRoll = P->Roll/DEGREE;
	if (TmpRoll > 180.0f) TmpRoll -= 360.0f;

	float LoadFactor = 1.0/cos(DegToRad(fabs(TmpRoll)));
	if (LoadFactor > 2.0) LoadFactor = 2.0;

	TrimTo(P,LoadFactor);

	float CurrAttitude = (float)((signed short)P->Pitch);
	float Alpha = (float)((signed short)(RadToDeg(P->Alpha)*DEGREE));

	float SuggestedAltTrim = (0.000018f*(CurrAttitude-Alpha));

	P->SymetricalElevator += Integrate(0.0f,0.0f,0.0f,
	   								   P->CS->SymetricalElevatorRange,
									   P->CS->ElevatorRate,
    								   P->dt, SuggestedAltTrim);
}
//************************************************************************************************
void DoAltitudeHoldAutoPilot(PlaneParams *P)
{

	if (P->AutoPilotMode & PL_AP_CPL)
	{
		DoCplAltitudeHoldAutoPilot(P);	
	}
	else
	{
		Trim(P);

		float CurrAttitude = (float)((signed short)P->Pitch);
		float Alpha = (float)((signed short)(RadToDeg(P->Alpha)*DEGREE));

		float SuggestedAltTrim = (0.000018f*(CurrAttitude-Alpha));

		P->SymetricalElevator += Integrate(0.0f,0.0f,0.0f,
		   								   P->CS->SymetricalElevatorRange,
										   P->CS->ElevatorRate,
    									   P->dt, SuggestedAltTrim);
	}
}
//************************************************************************************************
void DoRdrAltitudeHoldAutoPilot(PlaneParams *P, float NewRdrAlt, float OldRdrAlt)
{
	Trim(P);

	float TargetAttitude = (float)((signed short)(RadToDeg(atan2((NewRdrAlt-OldRdrAlt),
		                                                   P->DistanceMoved*WUTOFT))*DEGREE));

	float Alpha = (float)((signed short)(RadToDeg(P->Alpha)*DEGREE));
	float CurrAttitude = (float)((signed short)P->Pitch);

	float SuggestedAttTrim = (0.000010f*(CurrAttitude-(TargetAttitude+Alpha)));

	P->SymetricalElevator += Integrate(0.0f,0.0f,0.0f,
  		   							   P->CS->SymetricalElevatorRange,
									   P->CS->ElevatorRate,
	    							   P->dt, SuggestedAttTrim);
}
//************************************************************************************************
void DoAttitudeHoldAutoPilot(PlaneParams *P, float TargetAttitude)
{
	Trim(P);

	float CurrAttitude = (float)((signed short)P->Pitch);
	float SuggestedAttTrim = (0.000018f*(CurrAttitude-TargetAttitude));

	P->SymetricalElevator += Integrate(0.0f,0.0f,0.0f,
  		   							   P->CS->SymetricalElevatorRange,
									   P->CS->ElevatorRate,
	    							   P->dt, SuggestedAttTrim);
}



//************************************************************************************************
void DoNavSteeringAutoPilot(PlaneParams *P)
{

	float TrackDelta = -ComputeHeadingToPoint(P->WorldPosition, P->Heading, HudSteerPoint, 1);

	BankAngleTarget = TrackDelta*2.5f;
	if (BankAngleTarget > 55.0) BankAngleTarget = 55.0;
	else
		if (BankAngleTarget < -55.0) BankAngleTarget = -55.0;

	float CurrentBankAngle = (float)(P->Roll/DEGREE);
	if (CurrentBankAngle > 180.0f) CurrentBankAngle -= 360.0f;
	CurrentBankAngle = -CurrentBankAngle;

  	float WorkAileron = (0.045f*(BankAngleTarget-CurrentBankAngle));
	P->Aileron = Integrate(0.0f,0.0f, P->Aileron, P->CS->AileronRange,
							P->CS->ElevatorRate,  P->dt, WorkAileron);
}
//************************************************************************************************
void DoBankAngleHoldAutoPilot(PlaneParams *P, float HoldBankAngle)
{
	float CurrentBankAngle = (float)(P->Roll/DEGREE);

	if (CurrentBankAngle > 180.0f) CurrentBankAngle -= 360.0f;
	CurrentBankAngle = -CurrentBankAngle;

	float WorkAileron = (0.085f*(HoldBankAngle - CurrentBankAngle));
	P->Aileron = Integrate(0.0f, 0.0f, P->Aileron, P->CS->AileronRange,
							  P->CS->ElevatorRate,  P->dt, WorkAileron);
}
//************************************************************************************************

void CheckAPDeselect(PlaneParams *P)
{

#ifdef __DEMO__
	VKCODE hack;

	hack.vkCode = 0;			   	// virtual key code (may be joy button)
	hack.wFlags = 0;			   	// vkCode Type
	hack.dwExtraCode = 0;		   	// any extra VK code, eg. SHIFT, CTRL, ALT
#endif

	if ((UFC.APStatus == OFF) && (UFC.TFStatus == OFF)) return;

	// AP System or Sub Systems damaged

	if (RealisticAPs)
	{
#ifdef __DEMO__
		if(P->Status & PL_AI_DRIVEN)
		{
			if((fabs(P->StickX) > 64.0) || (fabs(P->StickY) > 64.0))
			{
				if (UFC.APStatus == ON)
				{
					cam_front_view(hack);
					AutoPilotOnOff();
				}
			}
		}
		else
#endif

		/* temporary comment - redo below for F/A-18

		if (((P->AutoPilotMode & PL_AP_ALTITUDE) &&
			 (P->SystemInactive & DAMAGE_BIT_ADC)) ||
			 (P->SystemInactive & DAMAGE_BIT_AUTOPILOT))

		{
			goto APOff;
		}
		else		// Normal AP kickoff checks

		*/

		{
			/*
			if (UFC.TFStatus && ((P->IndicatedAirSpeed < 325) ||
  		       (P->SystemInactive & DAMAGE_BIT_ADC) ||
			   (P->SystemInactive & DAMAGE_BIT_AUTOPILOT) ||
			   (P->SystemInactive & DAMAGE_BIT_NAV_FLIR) ||
			   (P->GForce > (P->AvailGForce*0.75))))
			{
#ifdef __DEMO__
				cam_front_view(hack);
#endif
				TFAutoPilotOff(P);
				TimeExcel = 0;
				return;
			}
			  */
		  	int TmpRoll = P->Roll/DEGREE;
			int TmpPitch = P->Pitch/DEGREE;

			if ((UFC.APStatus == ON) &&

				(

				((float)RadToDeg(P->Alpha) >= 20.0) ||

//				(DegAOAToUnits( (float)RadToDeg(P->Alpha) ) >= 20.0) ||

				((TmpRoll > 75) && (TmpRoll < 285)) ||
 				((TmpPitch > 45) && (TmpPitch < 315))  ||
				(fabs(P->GForce) > 4.0) ||
				(P->Knots < 250)))
			{
				goto APOff;
			}


		}
	}
	else
	{
#ifdef __DEMO__
		if((fabs(P->StickX) > 64.0) || (fabs(P->StickY) > 64.0))
		{
			if (UFC.APStatus == ON)
			{
				cam_front_view(hack);
				AutoPilotOnOff();
			}
		}
#else
		if ((P->SystemInactive & DAMAGE_BIT_AUTOPILOT) ||
			(fabs(P->StickX) > 32.0) || (fabs(P->StickY) > 32.0))
		{
			if (UFC.APStatus == ON)
			{
//				AutoPilotOnOff();
				UFC.APStatus |= 2;
			}
		}
#endif
	}
	return;

APOff:
#ifdef __DEMO__
	cam_front_view(hack);
#endif
	AutoPilotOff(P);
	SndQueueSpeech(S250at, 0 ,g_iSoundLevelExternalSFX,3);
	SndServiceSound();
	TimeExcel = 0;
}
