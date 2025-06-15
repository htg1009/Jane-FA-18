// ssound.cpp

#include "F18.h"
#include "gamesettings.h"
#include "spchcat.h"
#include "3dfxF18.h"
#include "SkunkFF.h"
#include "snddefs.h"
#include "seatstf.h"

#define NGScrape	458
#define S1exex02	418
#define S1exin02	421
#define SND_HEART_BEAT	319

DWORD g_dwDitchScrape;
extern void KillSound( DWORD *SndHandle );
void PlayPositionalScraping(CameraInstance &camera,FPointDouble &SoundPosition);


struct stAircraftSound
{
	PlaneParams *Plane;
	DWORD dwSoundHandle;
	DWORD dwBurnerSoundHandle;
};
typedef struct stAircraftSound AircraftSound;

struct stVehicleSound
{
	MovingVehicleParams *Vehicle;
	DWORD dwSoundHandle;
};
typedef struct stVehicleSound VehicleSound;


#define MAX_AIRCRAFT_SND_TRACK		5
#define MAX_VEHICLE_SND_TRACK		5

AircraftSound TrackAircraftSnd[MAX_AIRCRAFT_SND_TRACK];
VehicleSound TrackVehicleSnd[MAX_AIRCRAFT_SND_TRACK];

int g_iEngineType=SND_F18_ENGINE_INTERNAL_NORM;
int g_iBurnerType=Abrrin;
int g_iAircraftSoundLevels=0;
int g_iExplosionSoundLevels=0;

void UpdateOtherAircraftSounds();
void AircraftSounds();
void VehicleSounds();


//*************************************************************************************************
void InitSoundEffectsVariables()
{
	g_dwInternalEngineRumble = 0;
	g_dwExternalEngineGeneratorWine=0;
	g_dwExternalEngineRumble = 0;
	g_dwEngineGeneratorWine = 0;
	g_dwBurnerRumble = 0;
	g_dwWndNoise=0;
	g_dwAOASndHandle=0;
	g_dwAIMSndHandle=0;
	g_dwGunSndHandle=0;
    g_dwMissileLaunchSound=0;
	g_dwExplosionSnd1=0;
	g_dwExplosionSnd2=0;
	g_dwExplosionSnd3=0;
	g_dwAIMLockSndHandle=0;
	g_iAircraftSoundLevels=0;
	g_iExplosionSoundLevels=0;


	g_iBurnerType=Abrrin;
	g_iEngineType = SND_F18_ENGINE_INTERNAL_NORM;

#if 0
	if (g_Settings.snd.byFlags & GP_BURNER_BASE)
	{
		g_iBurnerType=Abrrin;
		g_iEngineType = SND_F18_ENGINE_INTERNAL_NORM;
	}
	else
	{
		g_iBurnerType=822;
		g_iEngineType = 310;
	}
#endif

	g_iPrimeSounds=0;

	iWndMode=0;
	g_iPlayerSoundMode=1;	// 1=Internal,0=External
	g_iEngineSoundThrust=0;
	g_iOurPlanePan=0;
	g_iOurPlaneVolume=0;

	memset(&TrackAircraftSnd,0,(sizeof(AircraftSound)*MAX_AIRCRAFT_SND_TRACK));
	memset(&TrackVehicleSnd,0,(sizeof(VehicleSound)*MAX_VEHICLE_SND_TRACK));

	g_iProximityHit=0;
	g_iFlameOutTimer=0;

	dwFlameOut1=0;
	dwFlameOut2=0;

	g_bBingoFuel=FALSE;

	if (g_Settings.snd.byEngine)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byEngine/100);
		g_iSoundLevelEngineRum = fTmpPercent*90.0f;
	} else {
		g_iSoundLevelEngineRum = 0;
	}
	if (g_iSoundLevelEngineRum)
	{
		g_iSoundLevelEngineGen = g_iSoundLevelEngineRum/6;
		if (g_iSoundLevelEngineGen<5)
		{
			g_iSoundLevelEngineGen=5;
		}
	}

	if (g_Settings.snd.byExternalSFX)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byExternalSFX/100);
		g_iSoundLevelExternalSFX = fTmpPercent*127.0f;
	} else {
		g_iSoundLevelExternalSFX=0;
	}

	if (g_Settings.snd.byExplosions)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byExplosions/100);
		g_iExplosionSoundLevels = fTmpPercent*127.0f;
	} else {
		g_iExplosionSoundLevels=0;
	}


	if (g_Settings.snd.byBetty)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byBetty/100);
		g_iBaseBettySoundLevel = fTmpPercent*90.0f;
	} else {
		g_iBaseBettySoundLevel=0;
	}

	if (g_Settings.snd.byRadioSpeech)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byRadioSpeech/100);
		g_iBaseSpeechSoundLevel = fTmpPercent*90.0f;
	} else {
		g_iBaseSpeechSoundLevel=0;
	}

	if (g_Settings.snd.byCautionSeekTones)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byCautionSeekTones/100);
		g_iSoundLevelCaution = fTmpPercent*100.0f;
	} else {
		g_iSoundLevelCaution=0;
	}

	if (g_Settings.snd.byExternalAircraftLevel)
	{
		float fTmpPercent = (float)((float)g_Settings.snd.byExternalAircraftLevel/100);
		g_iAircraftSoundLevels = fTmpPercent*127.0f;
	} else {
		g_iAircraftSoundLevels=0;
	}
}


//*************************************************************************************************
void StartEngineSounds()
{
	g_iPlayerSoundMode=1;
	g_iEngineSoundThrust=0;
	g_iFlameOutTimer=0;

	if (g_iSoundLevelEngineRum)
	{
		g_dwInternalEngineRumble = SndQueueSound(g_iEngineType, 0, g_iSoundLevelEngineRum, CHANNEL_INTAKE_L);
		g_dwEngineGeneratorWine = SndQueueSound(S4001, 0, g_iSoundLevelEngineGen, CHANNEL_INTAKE_R);
	}
}

//*************************************************************************************************
void StopEngineSounds()
{
	if (g_iPlayerSoundMode)
	{
		if (g_dwInternalEngineRumble)
			SndEndSound(g_dwInternalEngineRumble);
		if (g_dwEngineGeneratorWine)
			SndEndSound(g_dwEngineGeneratorWine);
		g_dwInternalEngineRumble=0;
		g_dwEngineGeneratorWine=0;
	} else {
		if (g_dwExternalEngineRumble)
			SndEndSound(g_dwExternalEngineRumble);
		g_dwExternalEngineRumble=0;
	}
	if (g_dwBurnerRumble)
	{
		SndEndSound(g_dwBurnerRumble);
		g_dwBurnerRumble=0;
	}
	if (g_dwWndNoise)
	{
		SndEndSound(g_dwWndNoise);
		g_dwWndNoise=0;
	}
	if (g_dwExternalEngineRumble)
	{
		SndEndSound(g_dwExternalEngineRumble);
		g_dwExternalEngineRumble=0;
	}
	if (g_dwExternalEngineGeneratorWine)
	{
		SndEndSound(g_dwExternalEngineGeneratorWine);
		g_dwExternalEngineGeneratorWine=0;
	}
}

//*************************************************************************************************
void CheckFlameOut()
{
	if (!g_iSoundLevelEngineRum) return;

	int iShutdownStage = ((CurrentTick - g_iFlameOutTimer) > 1000) + ((CurrentTick - g_iFlameOutTimer) > 3000) + ((CurrentTick - g_iFlameOutTimer) > 6000) + ((CurrentTick - g_iFlameOutTimer) > 8000);

	if ((g_iFlameOutTimer) && (iShutdownStage<4))
	{
		switch(iShutdownStage)
		{
			case 1:
			break;
			case 2:
				if (!dwFlameOut1)
				{
					SndQueueSound(Eflamei,1,(g_iSoundLevelEngineRum+30));
					if (g_dwBurnerRumble)
					{
						SndEndSound(g_dwBurnerRumble);
						g_dwBurnerRumble=0;
					}

					if (g_iPlayerSoundMode)
					{
						if (g_dwInternalEngineRumble)
							SndEndSound(g_dwInternalEngineRumble);
						g_dwInternalEngineRumble=0;
					} else {
						if (g_dwExternalEngineRumble)
							SndEndSound(g_dwExternalEngineRumble);
						g_dwExternalEngineRumble=0;
					}
					dwFlameOut1=1;
				}
			break;
			case 3:
				if (!dwFlameOut2)
				{
					SndQueueSound(Eflamei,1,g_iSoundLevelEngineRum);
					if (g_iPlayerSoundMode)
					{
						if (g_dwEngineGeneratorWine)
							SndEndSound(g_dwEngineGeneratorWine);
						g_dwEngineGeneratorWine=0;
					}
					dwFlameOut2=1;
				}
			break;
		}
	}
}

//*************************************************************************************************
void FlameOutStopEngineSounds()
{
	g_iFlameOutTimer=CurrentTick;
}

//*************************************************************************************************
void UpdateWindNoise()
{
	if ((PlayerPlane->LandingGear) && (PlayerPlane->Knots>150))
	{
		float fTmpPercent = (float)((float)PlayerPlane->Knots/600.0f);
		int iLevel = g_iSoundLevelEngineRum-20;
		if (iLevel<0) iLevel=1;
		int iVolVariance = fTmpPercent*(float)(iLevel);
		if (iVolVariance>iLevel) iVolVariance=iLevel;
		if (iVolVariance<0) iVolVariance=0;

		if (!g_dwWndNoise)
		{
			g_dwWndNoise = SndQueueSound(Wndgear,0,iVolVariance,CHANNEL_WNDNOISE);
			iWndMode=1;
		}
	} else {
		if ((g_dwWndNoise) && (iWndMode==1))
		{
			SndEndSound(g_dwWndNoise);
			g_dwWndNoise=NULL;
		}
	}

/* gk commented this out

	if ((PlayerPlane->Flaps) && (PlayerPlane->Knots>150))
	{
		float fTmpPercent = (float)((float)PlayerPlane->Knots/600.0f);
		int iLevel = g_iSoundLevelEngineRum-20;
		int iVolVariance = fTmpPercent*(float)iLevel;
		if (iVolVariance>iLevel) iVolVariance=iLevel;
		if (iVolVariance<0) iVolVariance=0;

		if (!g_dwWndNoise)
		{
			g_dwWndNoise = SndQueueSound(Wndflap,0,iVolVariance,CHANNEL_WNDNOISE);
			iWndMode=2;
		} else {
			if (iWndMode<2)
			{
				SndEndSound(g_dwWndNoise);
				g_dwWndNoise = SndQueueSound(Wndflap,0,iVolVariance,CHANNEL_WNDNOISE);
				iWndMode=2;
			}
		}
	} else {
		if ((g_dwWndNoise) && (iWndMode==2))
		{
			SndEndSound(g_dwWndNoise);
			g_dwWndNoise=NULL;
		}
	}
	*/
//		if ((PlayerPlane->SpeedBrakeState) && (PlayerPlane->Knots>150))

	if((PlayerPlane->SpeedBrake > 10.0) && ((g_iSoundLevelEngineRum-40) > 0))
	{
		float fTmpPercent = (float)((float)PlayerPlane->Knots/800.0f);
		int iVolVariance = fTmpPercent*(float)(g_iSoundLevelEngineRum+25);
		if (iVolVariance>(g_iSoundLevelEngineRum+25)) iVolVariance=(g_iSoundLevelEngineRum+25);
		if (iVolVariance<0) iVolVariance=0;

		if (!g_dwWndNoise)
		{
			g_dwWndNoise = SndQueueSound(Wndbrak,0,iVolVariance,CHANNEL_WNDNOISE);
			iWndMode=3;
		} else {
			if (iWndMode<3)
			{
				SndEndSound(g_dwWndNoise);
				g_dwWndNoise = SndQueueSound(Wndbrak,0,iVolVariance,CHANNEL_WNDNOISE);
				iWndMode=3;
			}
		}
	} else {
		if ((g_dwWndNoise) && (iWndMode==3))
		{
			SndEndSound(g_dwWndNoise);
			g_dwWndNoise=NULL;
		}
	}

	// Figure out wind noise volume

	if (g_dwWndNoise)
	{
		float fTmpPercent=0;
		int iVolVariance=0;
		int iLevel=0;

		switch(iWndMode)
		{
			case 1:
				fTmpPercent = (float)((float)PlayerPlane->Knots/600.0f);
				iLevel=(g_iSoundLevelEngineRum-20);
				if (iLevel<0) iLevel=1;
				iVolVariance = fTmpPercent*(float)(iLevel);
				if (iVolVariance>iLevel) iVolVariance=(iLevel);
				if (iVolVariance<0) iVolVariance=0;
			break;
			case 2:
				fTmpPercent = (float)((float)PlayerPlane->Knots/600.0f);
				iLevel=(g_iSoundLevelEngineRum-20);
				if (iLevel<0) iLevel=1;
				iVolVariance = fTmpPercent*(float)(iLevel);
				if (iVolVariance>iLevel) iVolVariance=iLevel;
				if (iVolVariance<0) iVolVariance=0;
			break;
			case 3:
				fTmpPercent = (float)((float)PlayerPlane->Knots/800.0f);
				iVolVariance = fTmpPercent*(float)(g_iSoundLevelEngineRum+25);
				if (iVolVariance>(g_iSoundLevelEngineRum+25)) iVolVariance=(g_iSoundLevelEngineRum+25);
				if (iVolVariance<0) iVolVariance=0;
			break;
		}
		SndSetSoundVolume(g_dwWndNoise,iVolVariance);
	}
}

//*************************************************************************************************
void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition)
{
	// Find Distance
	float dx,dy,dz,fDist;
	ANGLE offangle;
	int iExplosionType=Gexlgex1;
	int iExplosionVolume=60;

//	if (!g_iSoundLevelExternalSFX) return;
	if(!g_iExplosionSoundLevels) return;
	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;



	dx = SoundPosition.X - camera.CameraLocation.X;
	dy = SoundPosition.Y - camera.CameraLocation.Y;
	dz = SoundPosition.Z - camera.CameraLocation.Z;

	fDist = QuickDistance3D(dx,dy,dz);
	fDist = fDist * WUTOFT;

	if (fDist<(5*NMTOFT))
	{
		float fVolPercent = (float)(fDist/(5*NMTOFT));
		iExplosionVolume = ((1.0f - fVolPercent)*g_iExplosionSoundLevels);  //  g_iSoundLevelExternalSFX);
		if (iExplosionVolume<0) iExplosionVolume=0;
//		if (iExplosionVolume>g_iSoundLevelExternalSFX) iExplosionVolume=g_iSoundLevelExternalSFX;
		if (iExplosionVolume>g_iExplosionSoundLevels) iExplosionVolume=g_iExplosionSoundLevels;

		offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera.Heading;

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

		// JLM FF we want up close explosions to be BIG, others to fade with distance
        int Magnitude;
		if(fDist < 200)
		  Magnitude = 100;
		else
		  Magnitude = 60 - (( (fDist)/(5*NMTOFT) )*60);

		if(pSeatData)
		{
			pSeatData->DamageType = 100;
			pSeatData->DamageDone = Magnitude;
			pSeatData->Heading = AIConvertAngleTo180Degree(offangle);
			pSeatData->Pitch = 0;
		}

		PlayFFEffect(FF_PROXIMITY_EXPLOSION,FF_SET_MAGNITUDE,&Magnitude);

		switch(iWarheadType)
		{
			case WARHEAD_STANDARD_MEDIUM:
				iExplosionType=Gexsmex2;
			break;
			case WARHEAD_STANDARD_LARGE:
				iExplosionType=Gexlgex2;
			break;
			case WARHEAD_STANDARD_HUGE:
				iExplosionType=Gexlgex3;
			break;
			case WARHEAD_NUKE:
			break;
			case WARHEAD_AAA_SMALL:
			case WARHEAD_AAA_MEDIUM:
			case WARHEAD_AAA_LARGE:
			case WARHEAD_AAA_TINY:
				iExplosionType=Gexlgex1;
				if (iExplosionVolume>60) iExplosionVolume=60;
			break;
			case WARHEAD_SAM_LARGE:
				iExplosionType=Sammisi;
			break;
			case WARHEAD_CRATERING:
				iExplosionType=Gexlgex1;
			break;

			default:
				iExplosionType=Gexlgex1;
				break;
		}
#if 0
		char sTxt[80];
		sprintf(sTxt,"Submitted Warhead: %d Type: %d Volume: %d Pan: %d\n",iWarheadType,iExplosionType,iExplosionVolume,iDirection);
		OutputDebugString(sTxt);
#endif
		if (!SndIsSoundPlaying(g_dwExplosionSnd1))
		{
#if 0
#ifndef	sTxt
			char sTxt[80];
#endif	sTxt

			sprintf(sTxt,"Warhead: %d Type: %d Volume: %d Pan: %d\n",iWarheadType,iExplosionType,iExplosionVolume,iDirection);
			OutputDebugString(sTxt);
#endif
			PauseForDiskHit();
			g_dwExplosionSnd1=SndQueueSound(iExplosionType,1,iExplosionVolume,-1,-1,iDirection);
			UnPauseFromDiskHit();
		}
		else {
			if (!SndIsSoundPlaying(g_dwExplosionSnd2))
			{
#if 0
#ifndef	sTxt
				char sTxt[80];
#endif	sTxt
				sprintf(sTxt,"Warhead: %d Type: %d Volume: %d Pan: %d\n",iWarheadType,iExplosionType,iExplosionVolume,iDirection);
				OutputDebugString(sTxt);
#endif
				PauseForDiskHit();
				g_dwExplosionSnd2=SndQueueSound(iExplosionType,1,iExplosionVolume,-1,-1,iDirection);
				UnPauseFromDiskHit();
			} else {
				if (!SndIsSoundPlaying(g_dwExplosionSnd3))
				{
#if 0
#ifndef	sTxt
					char sTxt[80];
#endif	sTxt
					sprintf(sTxt,"Warhead: %d Type: %d Volume: %d Pan: %d\n",iWarheadType,iExplosionType,iExplosionVolume,iDirection);
					OutputDebugString(sTxt);
#endif
					PauseForDiskHit();
					g_dwExplosionSnd3=SndQueueSound(iExplosionType,1,iExplosionVolume,-1,-1,iDirection);
					UnPauseFromDiskHit();
				}
			}
		}
	}
}


#if 0
AIRCRAFT_TYPE_FIGHTER
AIRCRAFT_TYPE_BOMBER
AIRCRAFT_TYPE_TRANSPORT
AIRCRAFT_TYPE_TANKER
AIRCRAFT_TYPE_C3
AIRCRAFT_TYPE_HELICOPTER
AIRCRAFT_TYPE_CIVILIAN
#endif



//*************************************************************************************************
void ProximitySoundHit(DBWeaponType *pweapon_type)
{
	int iExplosionType=0;

	if (!g_iSoundLevelExternalSFX) return;
	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;

	if (!pweapon_type) return;

	switch(pweapon_type->iWarHead)
	{
		case WARHEAD_STANDARD_MEDIUM:
		case WARHEAD_STANDARD_LARGE:
		case WARHEAD_STANDARD_HUGE:
		case WARHEAD_NUKE:
		case WARHEAD_CRATERING:
			iExplosionType=Gexlgex1;
		break;
		case WARHEAD_AAA_LARGE:
			iExplosionType=Gexlgex1;
		break;
		case WARHEAD_SAM_LARGE:
			iExplosionType=Sammisi;
			break;
		default:
			iExplosionType=Gexlgex1;
			break;
	}
	g_iProximityHit=16;
}

//*************************************************************************************************
void PlayScheduledProximityHits()
{
	DWORD dwTmpSound=0;
	int iLevel=0;

	if (!g_iSoundLevelExternalSFX) return;
	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;

	if (!g_iProximityHit) return;

	switch(g_iProximityHit)
	{
		case 10:
			PauseForDiskHit();
			SndQueueSound(Impsmex2,1,g_iSoundLevelExternalSFX,-1,-1,127);
			UnPauseFromDiskHit();
		break;
		case 12:
			iLevel = g_iSoundLevelExternalSFX-40;
			if (iLevel<0) iLevel=0;
			PauseForDiskHit();
			SndQueueSound(Radthum,1,iLevel,-1,-1,0);
			UnPauseFromDiskHit();
		break;
  		case 15:
			iLevel = g_iSoundLevelExternalSFX-30;
			if (iLevel<0) iLevel=0;
			PauseForDiskHit();
			SndQueueSound(Impsmex1,1,g_iSoundLevelExternalSFX,-1,-1,127);
			UnPauseFromDiskHit();
		break;
	}
	if (g_iProximityHit>0)
		g_iProximityHit--;
}


//*************************************************************************************************
void CheckPlaneCrash()
{
	if (PlayerPlane->FlightStatus & PL_STATUS_CRASHED)
	{
		StopEngineSounds();

		if (g_dwAOASndHandle)
		{
			SndEndSound(g_dwAOASndHandle);
			g_dwAOASndHandle=0;
		}
		if (g_dwAIMSndHandle)
		{
			SndEndSound(g_dwAIMSndHandle);
			g_dwAIMSndHandle=0;
		}
		if (g_dwAIMLockSndHandle)
		{
			SndEndSound(g_dwAIMLockSndHandle);
			g_dwAIMLockSndHandle=0;
		}
		if (g_dwGunSndHandle)
		{
			SndEndSound(g_dwGunSndHandle);
			g_dwGunSndHandle=0;
		}
		if (g_dwMissileLaunchSound)
		{
			SndEndSound(g_dwMissileLaunchSound);
			g_dwMissileLaunchSound=0;
		}
		SndFlushSpeechQueue(CHANNEL_BETTY);
	}
}

//*************************************************************************************************
void PrimeSounds()
{
	// This really, really sucks that I have to do this.
	if (!g_iPrimeSounds)
	{
		SndQueueSound(Gexlgex1,1,0,-1,-1,64);
		g_iPrimeSounds=1;
	}
}

//*************************************************************************************************
void UpdateSounds()
{
	PrimeSounds();
	AircraftSounds();
	UpdateWindNoise();
	VehicleSounds();
	PlayScheduledProximityHits();
	CheckFlameOut();
	CheckPlaneCrash();
}

//*************************************************************************************************
void LandingBurnerSound(PlaneParams *P)
{
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (1320 * FTTOWU);
	int iPlaneSoundVolume=0;
	ANGLE camera_heading, camera_pitch, camera_roll;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;


	int iExternalAircraftSoundLevel=g_iAircraftSoundLevels;
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		// Dampen sound - we are inside the cockpit
		iExternalAircraftSoundLevel=(iExternalAircraftSoundLevel*0.10);
	}

	if (iExternalAircraftSoundLevel<=0) return;

	PlaneParams *pPlane=NULL;

	for(PlaneParams *planepnt = Planes; planepnt <= LastPlane; planepnt ++)
	{
		if (planepnt->PlaneCopy==P)
		{
			pPlane = planepnt;
			break;
		}
	}
	if (!pPlane) return;


	dx = pPlane->WorldPosition.X - Camera1.CameraLocation.X;
	dy = pPlane->WorldPosition.Y - Camera1.CameraLocation.Y;
	dz = pPlane->WorldPosition.Z - Camera1.CameraLocation.Z;
	fDist = QuickDistance3D(dx,dy,dz);

	if ((fDist<fCanHearYou) && (fDist>0))
	{
		offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera_heading;

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

		float fVolPercent = (float)(fDist/fCanHearYou);
		iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalAircraftSoundLevel-(iBehind*20)));
		iPlaneSoundVolume-=30;
		if (iPlaneSoundVolume<0) iPlaneSoundVolume=0;
		if (iPlaneSoundVolume>iExternalAircraftSoundLevel) iPlaneSoundVolume=iExternalAircraftSoundLevel;


		int iTrapSound=SND_F18_F14TRAP;

		if (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID==61)
		{
			iTrapSound=SND_F18_LAND;
		}

		if (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID==63)
		{
			iTrapSound=0;
		}

		if (iTrapSound)
		{
			DWORD dwSoundHandle=SndQueueSound(iTrapSound,1,iPlaneSoundVolume);
			SndSetSoundPanPosition(dwSoundHandle,iDirection);
		}
	}
}


//*********************************************************************************************************************
int CurrentlyTracking(PlaneParams *P)
{
	for (int i=0; i<MAX_AIRCRAFT_SND_TRACK; i++)
	{
		if (TrackAircraftSnd[i].Plane==P)
			return(i);
	}
	return(-1);
}

//*********************************************************************************************************************
int GetTrackingIndex()
{
	for (int i=0; i<MAX_AIRCRAFT_SND_TRACK; i++)
	{
		if (!TrackAircraftSnd[i].Plane)
			return(i);
	}
	return(-1);
}


//*********************************************************************************************************************
int CurrentlyTrackingVehicle(MovingVehicleParams *V)
{
	for (int i=0; i<MAX_VEHICLE_SND_TRACK; i++)
	{
		if (TrackVehicleSnd[i].Vehicle==V)
			return(i);
	}
	return(-1);
}

//*********************************************************************************************************************
int GetTrackingIndexVehicle()
{
	for (int i=0; i<MAX_VEHICLE_SND_TRACK; i++)
	{
		if (!TrackVehicleSnd[i].Vehicle)
			return(i);
	}
	return(-1);
}




//*********************************************************************************************************************
void LaunchAircraftSound(PlaneParams *pPlane)
{
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (1320 * FTTOWU);
	int iPlaneSoundVolume=0;
	ANGLE camera_heading, camera_pitch, camera_roll;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;

	int iExternalAircraftSoundLevel=g_iAircraftSoundLevels;
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		// Dampen sound - we are inside the cockpit
		iExternalAircraftSoundLevel=(iExternalAircraftSoundLevel*0.10);
	}

	if (iExternalAircraftSoundLevel<=0) return;
	if (!pPlane) return;


	dx = pPlane->WorldPosition.X - Camera1.CameraLocation.X;
	dy = pPlane->WorldPosition.Y - Camera1.CameraLocation.Y;
	dz = pPlane->WorldPosition.Z - Camera1.CameraLocation.Z;
	fDist = QuickDistance3D(dx,dy,dz);

	if ((fDist<fCanHearYou) && (fDist>0))
	{
		offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera_heading;

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

		float fVolPercent = (float)(fDist/fCanHearYou);
		iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalAircraftSoundLevel-(iBehind*20)));
		iPlaneSoundVolume-=10;
		if (iPlaneSoundVolume<0) iPlaneSoundVolume=0;
		if (iPlaneSoundVolume>iExternalAircraftSoundLevel) iPlaneSoundVolume=iExternalAircraftSoundLevel;

		int iLaunchSound=SND_F18_JETLAUNCH;

		if (pDBAircraftList[pPlane->AI.iPlaneIndex].lPlaneID==63)
		{
			iLaunchSound=0;
		}

		if (iLaunchSound)
		{
			DWORD dwSoundHandle=SndQueueSound(iLaunchSound,1,iPlaneSoundVolume);
			SndSetSoundPanPosition(dwSoundHandle,iDirection);
		}
	}
}


void DisplaySoundData()
{
	HUDtextLg(10,10, "FPS: %f", FrameRate);

	if (TrackAircraftSnd[0].Plane)
		HUDtextLg(10,20, "Aircraft #1 Thrust: %f",TrackAircraftSnd[0].Plane->LeftThrustPercent); //,(22050+(TrackAircraftSnd[0].Plane->LeftThrustPercent*120)));
	if (TrackAircraftSnd[1].Plane)				 											 
		HUDtextLg(10,30, "Aircraft #2 Thrust: %f",TrackAircraftSnd[1].Plane->LeftThrustPercent); //,(22050+(TrackAircraftSnd[1].Plane->LeftThrustPercent*120)));
	if (TrackAircraftSnd[2].Plane)
		HUDtextLg(10,40, "Aircraft #3 Thrust: %f",TrackAircraftSnd[2].Plane->LeftThrustPercent); //,(22050+(TrackAircraftSnd[2].Plane->LeftThrustPercent*120)));
	if (TrackAircraftSnd[3].Plane)				 											 
		HUDtextLg(10,50, "Aircraft #4 Thrust: %f",TrackAircraftSnd[3].Plane->LeftThrustPercent); //,(22050+(TrackAircraftSnd[3].Plane->LeftThrustPercent*120)));
}

//*************************************************************************************************
void CheckOurEngineSounds()
{
	if ((PlayerPlane->Status & PL_ACTIVE) && (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED)))
	{
		if (g_iSoundLevelEngineRum)
		{
			if (!g_dwInternalEngineRumble)
				g_dwInternalEngineRumble = SndQueueSound(g_iEngineType, 0, g_iSoundLevelEngineRum, CHANNEL_INTAKE_L);
			if (!g_dwEngineGeneratorWine)
				g_dwEngineGeneratorWine = SndQueueSound(S4001, 0, g_iSoundLevelEngineGen, CHANNEL_INTAKE_R);
		}
	}
}

//*********************************************************************************************************************
struct stAircraftSoundType
{
	int iIdleID;			// Idle 
	int iMoveID;			// Taxing
	int iFlightID;			// In Flight 
	int iBurnerID;			// Afterburner/Max Power
	int iStartUpID;			// Engine Start Up
	int iShutdownID;		// Engine Shutdown
	int iLaunchSoundID;		// Carrier Launch Sound - Sound at Cat launch time
	int iLandSoundID;		// Carrier Wire Catch and movement away from wire
	int iVolumeLevel;		// Average volume level
	int iMaxVolumeLevel;	// Maximum volue level
	int iPitchLevel;		// Average pitch level
	int iMinPitchLevel;		// Minimum pitch level
	int iMaxPitchLevel;		// Maximum pitch level
	int iFlags;
};
typedef struct stAircraftSoundType AircraftSoundType;

#define MAX_NUM_AIRCRAFT_SOUND_TYPES		9

//*********************************************************************************************************************
AircraftSoundType AircraftSoundList[MAX_NUM_AIRCRAFT_SOUND_TYPES] =
{
	{  SND_F18_ENGINE_IDLE,SND_F18_ENGINE_IDLE,SND_F18_ENGINE_FLIGHT,0,0,0,0,0,0,0,0,0,0,0 },		// F/A-18E
	{  SND_F14_ENGINE_IDLE,SND_F14_ENGINE_IDLE,SND_F14_ENGINE_IDLE,  0,0,0,0,0,0,0,0,0,0,0 },		// F-14
	{  SND_E2C_ENGINE_IDLE,SND_E2C_ENGINE_IDLE,SND_E2C_ENGINE_IDLE,  0,0,0,0,0,0,0,0,0,0,0 },		// E-2C
	{  SND_S3_ENGINE_IDLE,SND_S3_ENGINE_IDLE,SND_S3_ENGINE_IDLE,		0,0,0,0,0,0,0,0,0,0,0 },		// S-3
	{  SND_A6_ENGINE_IDLE,SND_A6_ENGINE_IDLE,SND_A6_ENGINE_IDLE,		0,0,0,0,0,0,0,0,0,0,0 },		// A-6
	{  SND_F18_ENGINE_IDLE,SND_F18_ENGINE_IDLE,SND_OTHERJET_ENGINE_FLIGHT,0,0,0,0,0,0,0,0,0,0,0 },		// Other
	{  SND_JET3_ENGINE_FLIGHT,SND_JET3_ENGINE_FLIGHT,SND_JET3_ENGINE_FLIGHT,0,0,0,0,0,0,0,0,0,0,0 },		// Other
	{  SND_SH60_ENGINE_IDLE,SND_SH60_ENGINE_IDLE,SND_SH60_ENGINE_IDLE,0,0,0,0,0,0,0,0,0,0,0 },		// Other
	{  SND_OTHERHELO_ENGINE_FLIGHT,SND_OTHERHELO_ENGINE_FLIGHT,SND_OTHERHELO_ENGINE_FLIGHT,0,0,0,0,0,0,0,0,0,0,0 },		// Other
};

#define SND_TYPE_F18		0
#define	SND_TYPE_F14		1
#define	SND_TYPE_E2C		2
#define SND_TYPE_S3			3
#define SND_TYPE_A6			4
#define SND_TYPE_OTHERJET	5
#define SND_TYPE_TRANSPORT	6
#define SND_TYPE_SH60		7
#define SND_TYPE_OTHERHELO	8

//*********************************************************************************************************************
void AircraftSounds()
{
  	PlaneParams *P = &Planes[0];
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (1320 * FTTOWU);	// 1/4 Mile
	int iPlaneSoundVolume=0;
	int iExternalAircraftSoundLevel=0;
	int isparked;
	int outofgas;
	ANGLE camera_heading, camera_pitch, camera_roll;
	float behind_perc = 0;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);

	PlayPositionalScraping(Camera1,PlayerPlane->WorldPosition);

	isparked = 0;

	if(PlayerPlane->AI.Behaviorfunc == AIParked)
		isparked = 1;
	else if((PlayerPlane->AI.Behaviorfunc == AIMoveToParkingSpace) && (PlayerPlane->AI.lVar3 >= 4))
	{
		isparked = 1;
	}

	if((PlayerPlane->TotalFuel <= 0) || (!(PlayerPlane->EngineStatus & (PL_RIGHT_ENGINE_ON|PL_LEFT_ENGINE_ON))))
		outofgas = 1;
	else
		outofgas = 0;

	if((Camera1.CameraMode == CAMERA_COCKPIT) && (!isparked) && (!outofgas))
	{
		// Check to see if internal sounds should be on

		CheckOurEngineSounds();

		// We are inside a cockpit
		g_iEngineSoundThrust=0;

		if (LeftEngineOn)
			g_iEngineSoundThrust=(PlayerPlane->LeftThrustPercent);
		else
			if (RightEngineOn)
				g_iEngineSoundThrust=(PlayerPlane->RightThrustPercent);

		float fTmpPercent = g_iEngineSoundThrust;

		int iVolVariance = fTmpPercent*30;
		if (iVolVariance>30) iVolVariance=30;
		if (iVolVariance<0) iVolVariance=0;

		int iVolVariance2 = fTmpPercent*0.60f;
		if (iVolVariance2>60) iVolVariance2=60;
		if (iVolVariance2<0) iVolVariance2=0;

		if (!LeftEngineOn && !RightEngineOn)
		{
			iVolVariance = 0;
			iVolVariance2 = 0;
		}
		else
			if ((LeftEngineOn && !RightEngineOn) || (!LeftEngineOn && RightEngineOn))
			{
				iVolVariance >>= 1;
				iVolVariance2 >>= 1;
			}

		iVolVariance2 -= 30;

		if((g_iSoundLevelEngineRum+iVolVariance2) < 0)
		{
			iVolVariance2 = -g_iSoundLevelEngineRum;
		}

		if((g_iSoundLevelEngineRum) && (!(!LeftEngineOn && !RightEngineOn)))
		{
			SndSetSoundVolume(g_dwInternalEngineRumble,(g_iSoundLevelEngineRum+iVolVariance2));
			SndSetSoundVolume(g_dwEngineGeneratorWine,(g_iSoundLevelEngineGen-(iVolVariance/5)));

			if (g_iEngineSoundThrust>80) 
			{
				// We are in burner - lets turn the burner sound on
				float fTmpPercent=(float)((float)(g_iEngineSoundThrust-80)/40);

				int iBurnerPercent=fTmpPercent*30;
				if (iBurnerPercent>30) iBurnerPercent=30;
				if (iBurnerPercent<0) iBurnerPercent=0;

				if ((!LeftEngineOn && RightEngineOn)||(LeftEngineOn && !RightEngineOn))
					iBurnerPercent >>= 1;

				if ((!g_dwBurnerRumble) && (!g_iFlameOutTimer))
				{
					g_dwBurnerRumble = SndQueueSound(g_iBurnerType,0,(g_iSoundLevelEngineRum+iBurnerPercent),CHANNEL_ENGBASS);
				} else {
					SndSetSoundVolume(g_dwBurnerRumble,(g_iSoundLevelEngineRum+iBurnerPercent));
				}
			} else {
				// We are not in burner
				if (g_iEngineSoundThrust<76)
				{
					if (g_dwBurnerRumble)
					{
						// If we were in burner and are now not, turn the burner sound off
						SndEndSound(g_dwBurnerRumble);
						g_dwBurnerRumble=NULL;
					}
				}
			}
		}
	} else {
		// Shutdown all internal engine sounds - we are now outside the aircraft
		if (g_dwInternalEngineRumble)
			SndEndSound(g_dwInternalEngineRumble);
		if (g_dwEngineGeneratorWine)
			SndEndSound(g_dwEngineGeneratorWine);
		if (g_dwBurnerRumble)
			SndEndSound(g_dwBurnerRumble);
		g_dwBurnerRumble=0;
		g_dwInternalEngineRumble=0;
		g_dwEngineGeneratorWine=0;
	}

	// Let's play the engine sounds for everyone

	int iExteriorSounds=1;

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) iExteriorSounds=0;

	iExternalAircraftSoundLevel=g_iAircraftSoundLevels;
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		// Dampen sound - we are inside the cockpit
		iExternalAircraftSoundLevel=(iExternalAircraftSoundLevel*0.20);
	}

	if (iExternalAircraftSoundLevel<=0) return;


	//if((PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED) || ((!RightEngineOn) && (!LeftEngineOn)))

	while (P <= LastPlane)
	{
		isparked = 0;

		if(P->AI.Behaviorfunc == AIParked)
			isparked = 1;
		else if((P->AI.Behaviorfunc == AIMoveToParkingSpace) && (P->AI.lVar3 >= 4))
		{
			isparked = 1;
		}

		if((P->TotalFuel <= 0) || ((P == PlayerPlane) && (!(P->EngineStatus & (PL_RIGHT_ENGINE_ON|PL_LEFT_ENGINE_ON)))))
		{
			outofgas = 1;
		}
		else
		{
			outofgas = 0;
		}

		if(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 85)
		{
			outofgas = 1;
		}

		if ((P->Status & PL_ACTIVE) && (!(P->FlightStatus & PL_STATUS_CRASHED)) && (!isparked) && (!outofgas))
		{
			if (!((P==PlayerPlane) && (Camera1.CameraMode == CAMERA_COCKPIT)))
			{
				dx = P->WorldPosition.X - Camera1.CameraLocation.X;
				dy = P->WorldPosition.Y - Camera1.CameraLocation.Y;
				dz = P->WorldPosition.Z - Camera1.CameraLocation.Z;
				fDist = QuickDistance3D(dx,dy,dz);

				if ((fDist<fCanHearYou) && (fDist>0) && iExteriorSounds)
				{
					offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera_heading;

					int iSide=0;	// 0 = Left, 1 = Right
					int iBehind=0;
					behind_perc = 0;

					if ((offangle>0x4000) && (offangle<0x8000))
					{
						iBehind=1;
					}
					if ((offangle>0x8000) && (offangle<0xC000))
					{
						iBehind=1;
					}

					if(iBehind)
					{
						behind_perc = ((fabs(AIConvertAngleTo180Degree(offangle)) - 90.0f) / 90.0f);
					}

					if (offangle>0x8000)
					{
						offangle = ((0xFFFF - offangle)+1);
						iSide=1;
					}
					if (offangle > 0x4000)
					{
						offangle = 0x8000-offangle;
					}

					float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
					int iDirection=0;

					if (iSide)
					{
						iDirection = 64+fOffDirect;
					} else {
						iDirection = 64-fOffDirect;
					}
					if (iDirection>127) iDirection=127;
					if (iDirection<0) iDirection=0;

					float fVolPercent = (float)(fDist/fCanHearYou);
//					iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalAircraftSoundLevel*(1.0f - (iBehind*0.20f))));
					iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalAircraftSoundLevel*(1.0f - (behind_perc * 0.80f))));
					if(WIsWeaponPlane(P))
					{
						iPlaneSoundVolume = (float)iPlaneSoundVolume * 0.25f;
					}
					if (iPlaneSoundVolume<0) iPlaneSoundVolume=0;
					if (iPlaneSoundVolume>iExternalAircraftSoundLevel) iPlaneSoundVolume=iExternalAircraftSoundLevel;

					// This aircraft should be tracked, lets see if we have an open slot and am not already
					// tracking this aircraft.

					int iTrackIndex=CurrentlyTracking(P);

					if(iTrackIndex != -1)
					{
						if(!SndIsSoundPlaying(TrackAircraftSnd[iTrackIndex].dwSoundHandle))
						{
							// This aircraft can no longer be heard.
							SndEndSound(TrackAircraftSnd[iTrackIndex].dwSoundHandle);
							SndEndSound(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle);
							TrackAircraftSnd[iTrackIndex].dwSoundHandle=NULL;
							TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle=NULL;
							TrackAircraftSnd[iTrackIndex].Plane = NULL;
							iTrackIndex = -1;
						}
					}

					if (iTrackIndex==-1)
					{
						int iNewTrackIndex = GetTrackingIndex();
						if (iNewTrackIndex!=-1)
						{
							int iSoundType = SND_TYPE_OTHERJET;

							if ((pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER) ||
								(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_BOMBER))
							{
								iSoundType = SND_TYPE_OTHERJET;
							}
							if ((pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER) ||
								(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TRANSPORT) ||
								(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3) ||
								(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN))
							{
								iSoundType = SND_TYPE_TRANSPORT;
							}
							if (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
							{
								iSoundType = SND_TYPE_OTHERHELO;
							}

							int iBurnerSoundType=0;

							switch(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID)
							{
								case 61:	// F/A-18E
									iSoundType=SND_TYPE_F18;
									if (P->LeftThrustPercent>70)
										iBurnerSoundType=451;
								break;
								case 62:	// F-14B
									iSoundType=SND_TYPE_F14;
									if (P->LeftThrustPercent>70)
										iBurnerSoundType=451;
								break;
								case 63:	// E-2C
									iSoundType=SND_TYPE_E2C;
								break;
								case 64:	// S-3B
									iSoundType=SND_TYPE_S3;
								break;
								case 65:	// SH60
									iSoundType=SND_TYPE_SH60;
								break;
								case 18:	// EA6B
									iSoundType=SND_TYPE_A6;
									if (P->LeftThrustPercent>70)
										iBurnerSoundType=451;
								break;
								case 79:	//  Bear
									iSoundType = SND_TYPE_TRANSPORT;
									break;
							}

							TrackAircraftSnd[iNewTrackIndex].dwSoundHandle = SndQueueSound(AircraftSoundList[iSoundType].iIdleID,0,iPlaneSoundVolume);
							SndSetSoundPanPosition(TrackAircraftSnd[iNewTrackIndex].dwSoundHandle,iDirection);
							TrackAircraftSnd[iNewTrackIndex].Plane=P;
							if (iBurnerSoundType)
							{
								TrackAircraftSnd[iNewTrackIndex].dwBurnerSoundHandle = SndQueueSound(iBurnerSoundType,0,iPlaneSoundVolume);
								SndSetSoundPanPosition(TrackAircraftSnd[iNewTrackIndex].dwBurnerSoundHandle,iDirection);
							}
						}
					} else {
//						if (!((TrackAircraftSnd[iTrackIndex].Plane==PlayerPlane) && !PlayerPlane->LandingGear))
//						{
							int iPlayBackRate=22050+(TrackAircraftSnd[iTrackIndex].Plane->LeftThrustPercent*130);
							SndSetSoundPlaybackRate(TrackAircraftSnd[iTrackIndex].dwSoundHandle,iPlayBackRate);
//						}
						if (TrackAircraftSnd[iTrackIndex].Plane->LeftThrustPercent>80)
						{
							if (pDBAircraftList[TrackAircraftSnd[iTrackIndex].Plane->AI.iPlaneIndex].lPlaneID==61)
							{
								if (!TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle)
								{
									TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle = SndQueueSound(451,0,iPlaneSoundVolume);
									SndSetSoundPanPosition(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle,iDirection);
								} else {
									SndSetSoundPanPosition(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle,iDirection);
									SndSetSoundVolume(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle,iPlaneSoundVolume);
								}
							}
						} else {
							if (TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle)
							{
								SndEndSound(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle);
								TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle=NULL;
							}
						}
						SndSetSoundPanPosition(TrackAircraftSnd[iTrackIndex].dwSoundHandle,iDirection);
						SndSetSoundVolume(TrackAircraftSnd[iTrackIndex].dwSoundHandle,iPlaneSoundVolume);
					}
				} else {
					int iTrackIndex=CurrentlyTracking(P);
					if (iTrackIndex!=-1)
					{
						// This aircraft can no longer be heard.
						SndEndSound(TrackAircraftSnd[iTrackIndex].dwSoundHandle);
						SndEndSound(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle);
						TrackAircraftSnd[iTrackIndex].dwSoundHandle=NULL;
						TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle=NULL;
						TrackAircraftSnd[iTrackIndex].Plane=NULL;
					}
				}
			} else {
				// If we are in the cockpit, make sure that we are not being tracked
				int iTrackIndex=CurrentlyTracking(P);
				if (iTrackIndex!=-1)
				{
					// This aircraft can no longer be heard.
					SndEndSound(TrackAircraftSnd[iTrackIndex].dwSoundHandle);
					SndEndSound(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle);
					TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle=NULL;
					TrackAircraftSnd[iTrackIndex].dwSoundHandle=NULL;
					TrackAircraftSnd[iTrackIndex].Plane=NULL;
				}
			}
		} else {
			if ((!(P->Status & PL_ACTIVE)) || (P->FlightStatus & PL_STATUS_CRASHED) || (isparked) || (outofgas))
			{
				int iTrackIndex=CurrentlyTracking(P);
				if (iTrackIndex!=-1)
				{
					// This aircraft can no longer be heard.
					SndEndSound(TrackAircraftSnd[iTrackIndex].dwSoundHandle);
					SndEndSound(TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle);
					TrackAircraftSnd[iTrackIndex].dwBurnerSoundHandle=NULL;
					TrackAircraftSnd[iTrackIndex].dwSoundHandle=NULL;
					TrackAircraftSnd[iTrackIndex].Plane=NULL;
				}
			}
		}
		P++;
	}
}


void GunSounds(DBWeaponType *weapon,FPointDouble position)
{
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (2000 * FTTOWU);
	int iPlaneSoundVolume=0;
	ANGLE camera_heading, camera_pitch, camera_roll;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);

	if (!weapon) return;

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;

	int iExternalAircraftSoundLevel=g_iSoundLevelExternalSFX;
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		// Dampen sound - we are inside the cockpit
		iExternalAircraftSoundLevel=(iExternalAircraftSoundLevel*0.10);
	}

	if (iExternalAircraftSoundLevel<=0) return;

	dx = position.X - Camera1.CameraLocation.X;
	dy = position.Y - Camera1.CameraLocation.Y;
	dz = position.Z - Camera1.CameraLocation.Z;
	fDist = QuickDistance3D(dx,dy,dz);

	if ((fDist<fCanHearYou) && (fDist>0))
	{
		offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera_heading;

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

		float fVolPercent = (float)(fDist/fCanHearYou);
		iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalAircraftSoundLevel-(iBehind*20)));
		if (iPlaneSoundVolume<0) iPlaneSoundVolume=0;
		if (iPlaneSoundVolume>iExternalAircraftSoundLevel) iPlaneSoundVolume=iExternalAircraftSoundLevel;


		// Determine Gun Type
		int iGunSoundType=0;
		
		switch(weapon->lWeaponID)
		{
			case 62:	// ZSU
			case 63:
				iGunSoundType=SND_GUN_3A37MM_EXTERNAL;
			break;
			case 64:	// 23mm
				iGunSoundType=SND_GUN_3A23MM_EXTERNAL;
			break;
			case 65:	// 37mm
				iGunSoundType=SND_GUN_3A37MM_EXTERNAL;
			break;
			case 66:	// 85mm
				iGunSoundType=SND_GUN_3A85MM_EXTERNAL;
			break;
			case 67:	// 100mm
				iGunSoundType=SND_GUN_3A100MM_EXTERNAL;
			break;
			case 160:	// 30mm
				iGunSoundType=SND_GUN_3A30MM_EXTERNAL;
			break;
			case 185:	// 130mm
				iGunSoundType=SND_GUN_3A120MM_EXTERNAL;
			break;
			case 199:	// 120mm
				iGunSoundType=SND_GUN_3A120MM_EXTERNAL;
			break;
			case 200:	// 25mm
				iGunSoundType=SND_GUN_3A120MM_EXTERNAL;
			break;
			default:
			break;
		}
		if (iGunSoundType)
		{
			DWORD dwSoundHandle=SndQueueSound(iGunSoundType,1,iPlaneSoundVolume);
			SndSetSoundPanPosition(dwSoundHandle,iDirection);
		}
	}
}


void GroundLaunchMissileSound(DBWeaponType *weapon,FPointDouble position)
{
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (1320 * FTTOWU);
	int iPlaneSoundVolume=0;
	ANGLE camera_heading, camera_pitch, camera_roll;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);

	if (!weapon) return;

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;

	int iExternalAircraftSoundLevel=g_iSoundLevelExternalSFX;
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		// Dampen sound - we are inside the cockpit
		iExternalAircraftSoundLevel=(iExternalAircraftSoundLevel*0.10);
	}

	if (iExternalAircraftSoundLevel<=0) return;

	dx = position.X - Camera1.CameraLocation.X;
	dy = position.Y - Camera1.CameraLocation.Y;
	dz = position.Z - Camera1.CameraLocation.Z;
	fDist = QuickDistance3D(dx,dy,dz);

	if ((fDist<fCanHearYou) && (fDist>0))
	{
		offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera_heading;

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

		float fVolPercent = (float)(fDist/fCanHearYou);
		iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalAircraftSoundLevel-(iBehind*20)));
		if (iPlaneSoundVolume<0) iPlaneSoundVolume=0;
		if (iPlaneSoundVolume>iExternalAircraftSoundLevel) iPlaneSoundVolume=iExternalAircraftSoundLevel;


		// Determine Missile Type
		int iMissileSoundType=SND_MISSILE_LAUNCH_EXTERNAL;
#if 0
		switch(weapon->lWeaponID)
		{
			case 62:	// ZSU
			case 63:
				iMissileSoundType=SND_Missile_3A37MM_EXTERNAL;
			break;
			default:
			break;
		}
#endif
		if (iMissileSoundType)
		{
			DWORD dwSoundHandle=SndQueueSound(iMissileSoundType,1,iPlaneSoundVolume);
			SndSetSoundPanPosition(dwSoundHandle,iDirection);
		}
	}
}



//*********************************************************************************************************************
void VehicleSounds()
{
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (1320 * FTTOWU);	// 1/4 Mile
	int iPlaneSoundVolume=0;
	int iExternalVehicleSoundLevel=0;
	int forceoff;
	ANGLE camera_heading, camera_pitch, camera_roll;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);

	// Let's play the engine sounds for everyone

	int iExteriorSounds=1;

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) iExteriorSounds=0;

	iExternalVehicleSoundLevel=g_iAircraftSoundLevels;

#if 0
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		// Dampen sound - we are inside the cockpit
		iExternalVehicleSoundLevel=(iExternalVehicleSoundLevel*0.10);
	}
#endif
	
	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		for (int i=0; i<MAX_VEHICLE_SND_TRACK; i++)
		{
			if (TrackVehicleSnd[i].dwSoundHandle)
			{
				SndEndSound(TrackVehicleSnd[i].dwSoundHandle);
				TrackVehicleSnd[i].dwSoundHandle=NULL;
				TrackVehicleSnd[i].Vehicle=NULL;
			}
		}
		return;
	}


	if (iExternalVehicleSoundLevel<=0) return;

	for (MovingVehicleParams *W=MovingVehicles; W<=LastMovingVehicle; W++)
	{
		if(!W->Type)
			W->Status &= ~(VL_ACTIVE);

		forceoff = 0;

		if(((W->Status & VL_WAITING) && (!(W->Status & (VL_SURFACE_ATTACK|VL_FIRE_WEAPONS)))) || (W->Status & (VL_DESTROYED|VL_DONE_MOVING)))
		{
			forceoff = 1;
		}

		if(!W->iShipType)
		{
			if(pDBVehicleList[W->iVDBIndex].lVehicleType & VEHICLE_TYPE_OTHER)
			{
				forceoff = 1;
			}
		}

		if((W->Status & VL_ACTIVE) && (!(W->Status & (VL_INVISIBLE))) && (!(W->Status & VL_DESTROYED)) && (!(forceoff)))
		{
			dx = W->WorldPosition.X - Camera1.CameraLocation.X;
			dy = W->WorldPosition.Y - Camera1.CameraLocation.Y;
			dz = W->WorldPosition.Z - Camera1.CameraLocation.Z;
			fDist = QuickDistance3D(dx,dy,dz);

			if ((fDist<fCanHearYou) && (fDist>0))
			{
				offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera_heading;

				int iSide=0;	// 0 = Left, 1 = Right
				int iBehind=0;

				if ((offangle>0x4000) && (offangle<0x8000))
				{
					iBehind=1;
				}
				if ((offangle>0x8000) && (offangle<0xC000))
				{
					iBehind=1;
				}

				if (offangle>0x8000)
				{
					offangle = ((0xFFFF - offangle)+1);
					iSide=1;
				}
				if (offangle > 0x4000)
				{
					offangle = 0x8000-offangle;
				}

				float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
				int iDirection=0;

				if (iSide)
				{
					iDirection = 64+fOffDirect;
				} else {
					iDirection = 64-fOffDirect;
				}
				if (iDirection>127) iDirection=127;
				if (iDirection<0) iDirection=0;

				float fVolPercent = (float)(fDist/fCanHearYou);
				iPlaneSoundVolume = ((1.0f - fVolPercent)*(iExternalVehicleSoundLevel-(iBehind*20)));
				if (iPlaneSoundVolume<0) iPlaneSoundVolume=0;
				if (iPlaneSoundVolume>iExternalVehicleSoundLevel) iPlaneSoundVolume=iExternalVehicleSoundLevel;

				// This Vehicle should be tracked, lets see if we have an open slot and am not already
				// tracking this Vehicle.

				int iVehicleSoundType=0;

				int iTrackIndex=CurrentlyTrackingVehicle(W);
				if (iTrackIndex==-1)
				{
					int iNewTrackIndex = GetTrackingIndexVehicle();
					if (iNewTrackIndex!=-1)
					{
						DBVehicleType *pvehicletype;

						if (!W->iShipType)
						{
							pvehicletype = &pDBVehicleList[W->iVDBIndex];
							iVehicleSoundType=SND_TRUCKIDLE_EXTERNAL;
						} else {
							iVehicleSoundType=SND_SHIPWAKE_EXTERNAL;
						}

						TrackVehicleSnd[iNewTrackIndex].dwSoundHandle = SndQueueSound(iVehicleSoundType,0,iPlaneSoundVolume);
						SndSetSoundPanPosition(TrackVehicleSnd[iNewTrackIndex].dwSoundHandle,iDirection);
						TrackVehicleSnd[iNewTrackIndex].Vehicle=W;
					}
				} else {
					SndSetSoundPanPosition(TrackVehicleSnd[iTrackIndex].dwSoundHandle,iDirection);
					SndSetSoundVolume(TrackVehicleSnd[iTrackIndex].dwSoundHandle,iPlaneSoundVolume);
				}
			} else {
				int iTrackIndex=CurrentlyTrackingVehicle(W);
				if (iTrackIndex!=-1)
				{
					// This Vehicle can no longer be heard.
					SndEndSound(TrackVehicleSnd[iTrackIndex].dwSoundHandle);
					TrackVehicleSnd[iTrackIndex].dwSoundHandle=NULL;
					TrackVehicleSnd[iTrackIndex].Vehicle=NULL;
				}
			}
		} else {
			if((!(W->Status & VL_ACTIVE)) || (W->Status & VL_DESTROYED) || (forceoff))
			{
				int iTrackIndex=CurrentlyTrackingVehicle(W);
				if (iTrackIndex!=-1)
				{
					// This Vehicle can no longer be heard.
					SndEndSound(TrackVehicleSnd[iTrackIndex].dwSoundHandle);
					TrackVehicleSnd[iTrackIndex].dwSoundHandle=NULL;
					TrackVehicleSnd[iTrackIndex].Vehicle=NULL;
				}
			}
		}
	}
}


//*********************************************************************************************************************
void SoundThunder(int iVolume, int iPan, int iMode)
{
	int iSoundID=SND_THUNDER;
	float fThunderVolume=iVolume;

	if (!g_iSoundLevelExternalSFX) return;

	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
		(Camera1.CameraMode == CAMERA_COCKPIT)) return;

	if (iMode)
	{
		iSoundID=SND_THUNDER_CLAP;
	}

	if (Camera1.CameraMode == CAMERA_COCKPIT)
	{
		fThunderVolume = fThunderVolume * 0.60;
	}

	if (fThunderVolume<0) fThunderVolume=0;
	if (fThunderVolume>g_iSoundLevelExternalSFX) fThunderVolume=g_iSoundLevelExternalSFX;

	DWORD dwSoundHandle=SndQueueSound(iSoundID,1,((int)fThunderVolume));
	if ((iPan>-1) && (iPan<128))
	{
		SndSetSoundPanPosition(dwSoundHandle,iPan);
	}
}
	
//*************************************************************************************************
void PlayPositionalNoGearHit(CameraInstance &camera,FPointDouble &SoundPosition)
{
	// Find Distance
	float dx,dy,dz,fDist;
	ANGLE offangle;
	int iCrashType=S1exin02;
	int iCrashVolume=60;

//	if (!g_iSoundLevelExternalSFX) return;
	if(!g_iExplosionSoundLevels) return;
//	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
//		(Camera1.CameraMode == CAMERA_COCKPIT)) return;



	if(Camera1.CameraMode == CAMERA_COCKPIT)
	{
		fDist = 0;
		iCrashType=S1exin02;
	}
	else
	{
		iCrashType=S1exex02;
		dx = SoundPosition.X - camera.CameraLocation.X;
		dy = SoundPosition.Y - camera.CameraLocation.Y;
		dz = SoundPosition.Z - camera.CameraLocation.Z;

		fDist = QuickDistance3D(dx,dy,dz);
		fDist = fDist * WUTOFT;
	}

	if (fDist<(5*NMTOFT))
	{
		float fVolPercent = (float)(fDist/(5*NMTOFT));
		iCrashVolume = ((1.0f - fVolPercent)*g_iExplosionSoundLevels);  //  g_iSoundLevelExternalSFX);
		if (iCrashVolume<0) iCrashVolume=0;
//		if (iCrashVolume>g_iSoundLevelExternalSFX) iCrashVolume=g_iSoundLevelExternalSFX;
		if (iCrashVolume>g_iExplosionSoundLevels) iCrashVolume=g_iExplosionSoundLevels;

		offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera.Heading;

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

		// JLM FF we want up close explosions to be BIG, others to fade with distance
        int Magnitude;
		if(fDist < 200)
		  Magnitude = 100;
		else
		  Magnitude = 60 - (( (fDist)/(5*NMTOFT) )*60);

		if(Camera1.CameraMode == CAMERA_COCKPIT)
		{
			if(pSeatData)
			{
				pSeatData->DamageType = 100;
				pSeatData->DamageDone = Magnitude;
				pSeatData->Heading = AIConvertAngleTo180Degree(offangle);
				pSeatData->Pitch = 0;
			}

			PlayFFEffect(FF_PROXIMITY_EXPLOSION,FF_SET_MAGNITUDE,&Magnitude);
		}

#if 0
		if (!SndIsSoundPlaying(g_dwExplosionSnd1))
		{
			PauseForDiskHit();
			g_dwExplosionSnd1=SndQueueSound(iCrashType,1,iCrashVolume,-1,-1,iDirection);
			UnPauseFromDiskHit();
		}
		else {
			if (!SndIsSoundPlaying(g_dwExplosionSnd2))
			{
				PauseForDiskHit();
				g_dwExplosionSnd2=SndQueueSound(iCrashType,1,iCrashVolume,-1,-1,iDirection);
				UnPauseFromDiskHit();
			} else {
				if (!SndIsSoundPlaying(g_dwExplosionSnd3))
				{
					PauseForDiskHit();
					g_dwExplosionSnd3=SndQueueSound(iCrashType,1,iCrashVolume,-1,-1,iDirection);
					UnPauseFromDiskHit();
				}
			}
		}
#endif
	}
}

//*************************************************************************************************
void PlayPositionalScraping(CameraInstance &camera,FPointDouble &SoundPosition)
{
	// Find Distance
	float dx,dy,dz,fDist;
	ANGLE offangle;
	int iCrashType=NGScrape;
	int iCrashVolume=60;

	if((PlayerPlane->TerrainType != TT_AIRPORT)|| (PlayerPlane->FlightStatus & PL_STATUS_CRASHED) || (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) || ((!(PlayerPlane->OnGround)) && ((!g_dwDitchScrape) || (PlayerPlane->HeightAboveGround > (12.0f * FTTOWU)))) || (PlayerPlane->V < 1.0f))
	{
		if(g_dwDitchScrape)
		{
			if (SndIsSoundPlaying(g_dwDitchScrape))
			{
				KillSound(&g_dwDitchScrape);
			}
		}
		return;
	}

//	if (!g_iSoundLevelExternalSFX) return;
	if(!g_iExplosionSoundLevels) return;
//	if ((g_Settings.snd.byFlags & GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT) &&
//		(Camera1.CameraMode == CAMERA_COCKPIT)) return;



	if(Camera1.CameraMode == CAMERA_COCKPIT)
	{
		fDist = 0;
		dx = 0;
		dy = 0;
		dz = -1;
	}
	else
	{
		dx = SoundPosition.X - camera.CameraLocation.X;
		dy = SoundPosition.Y - camera.CameraLocation.Y;
		dz = SoundPosition.Z - camera.CameraLocation.Z;

		fDist = QuickDistance3D(dx,dy,dz);
		fDist = fDist * WUTOFT;
	}

	if (fDist<(5*NMTOFT))
	{
		float fVolPercent = (float)(fDist/(5*NMTOFT));
		iCrashVolume = ((1.0f - fVolPercent)*g_iExplosionSoundLevels);  //  g_iSoundLevelExternalSFX);
		if(PlayerPlane->Knots < 100)
		{
			iCrashVolume = (float)iCrashVolume * (PlayerPlane->Knots / 100.0f);
		}

		if (iCrashVolume<0) iCrashVolume=0;
//		if (iCrashVolume>g_iSoundLevelExternalSFX) iCrashVolume=g_iSoundLevelExternalSFX;
		if (iCrashVolume>g_iExplosionSoundLevels) iCrashVolume=g_iExplosionSoundLevels;

		if(Camera1.CameraMode == CAMERA_COCKPIT)
		{
			offangle = AIConvert180DegreeToAngle(atan2(-dx, -dz) * 57.2958) - camera.Heading;
		}
		else
		{
			offangle = 0;
		}

		int iSide=0;	// 0 = Left, 1 = Right
		int iBehind=0;

		if ((offangle>0x4000) && (offangle<0x8000))
		{
			iBehind=1;
		}
		if ((offangle>0x8000) && (offangle<0xC000))
		{
			iBehind=1;
		}

		if (offangle>0x8000)
		{
			offangle = ((0xFFFF - offangle)+1);
			iSide=1;
		}
		if (offangle > 0x4000)
		{
			offangle = 0x8000-offangle;
		}

		float fOffDirect = (float)((float)offangle/(float)0x4000)*64.0f;
		int iDirection=0;

		if (iSide)
		{
			iDirection = 64+fOffDirect;
		} else {
			iDirection = 64-fOffDirect;
		}
		if (iDirection>127) iDirection=127;
		if (iDirection<0) iDirection=0;

#if 0
		// JLM FF we want up close explosions to be BIG, others to fade with distance
        int Magnitude;
		if(fDist < 200)
		  Magnitude = 100;
		else
		  Magnitude = 60 - (( (fDist)/(5*NMTOFT) )*60);

		if(Camera1.CameraMode == CAMERA_COCKPIT)
		{
			if(pSeatData)
			{
				pSeatData->DamageType = 100;
				pSeatData->DamageDone = Magnitude;
				pSeatData->Heading = AIConvertAngleTo180Degree(offangle);
				pSeatData->Pitch = 0;
			}

			PlayFFEffect(FF_PROXIMITY_EXPLOSION,FF_SET_MAGNITUDE,&Magnitude);
		}
#endif

		if(g_dwDitchScrape)
		{
			if (!SndIsSoundPlaying(g_dwDitchScrape))
			{
				KillSound(&g_dwDitchScrape);
			}
		}

		if(g_dwDitchScrape)
		{
			SndSetSoundPanPosition(g_dwDitchScrape,iDirection);
			SndSetSoundVolume(g_dwDitchScrape,iCrashVolume);
		}
		else
		{
			g_dwDitchScrape = SndQueueSound(iCrashType,0,iCrashVolume);
			SndSetSoundPanPosition(g_dwDitchScrape,iDirection);
		}
	}
}

/*-----------------------------------------------------------------------------
 *
 *	DoGHeartBeat()
 *
 *		This is called every frame regardless of camera view.
 *
 */
void DoGHeartBeat( PlaneParams *P )
{
	long warnticks;
	float fworkvar;
	float pilotgload = fabs(P->PilotGLoad);
	float maxrate = 1000.0f / 3.0f;
	float minrate = 1000.0f;
	float ming = 2.0f;
	float maxg = 9.0f;
	float gperc;

	if(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	if((SimPause) || (iEndGameState))
		return;

	if(pilotgload > ming)
	{
		lWSOgTimer -= DeltaTicks;
		if(lWSOgTimer < 0)
		{
			int iLevel = g_iSoundLevelCaution;
			gperc = (((pilotgload - ming) / (maxg - ming)) * 0.8f) + 0.2f;
			if(gperc < 0)
				gperc = 0;
			else if(gperc > 1.0f)
				gperc = 1.0f;

			iLevel = (float)iLevel * gperc;
			SndQueueSound( SND_HEART_BEAT, 1, iLevel );
		}
		if(pilotgload > 9.0f)
		{
			warnticks = maxrate;
		}
		else
		{
			fworkvar = (pilotgload - ming) / (maxg - ming);

			if(fworkvar < 0)
			{
				warnticks = minrate;
			}
			else if(fworkvar > 1.0f)
			{
				warnticks = maxrate;
			}
			else
			{
				fworkvar = ((minrate - maxrate) * (1.0f - fworkvar)) + maxrate;
				warnticks = fworkvar;
			}
		}
		if((lWSOgTimer < 0) || (lWSOgTimer > warnticks))
		{
			lWSOgTimer = warnticks;
		}
	}
}


