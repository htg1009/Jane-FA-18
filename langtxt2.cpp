//*******************************************************************************
//*  Langtext.cpp
//*
//*  This file contains functions dealing with language and text actions.
//*******************************************************************************
#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"
#include "3dfxF18.h"
#include "particle.h"
#include "snddefs.h"
#include "seatstf.h"
#include "engine.h"

#define DONOSOUNDS 1

extern AvRGB *HUDColorRGB;
extern AvionicsType Av;
extern int		TimeExcel;
extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
extern DBRadarType *GetRadarPtr(long id);
extern DetectedPlaneListType CurFramePlanes;
extern DBWeaponType *get_weapon_ptr(long id);
// extern RadarInfoType RadarInfo;
extern CurSelectedTargetInfoType CurTargetInfo;
extern DetectedPlaneListType PrevFramePlanes;
// extern DetectedPlaneListType Age1Planes;

#define AA_MAX_AGE_PLANES 10
extern DetectedPlaneListType AgePlanes[AA_MAX_AGE_PLANES];

extern int   NumDTWSTargets;
extern int   DTWSTargets[];
extern void FixBridges();
extern void StartEngineSounds();
extern DWORD g_dwAIMSndHandle;
extern DWORD g_dwAOASndHandle;
extern DWORD g_dwAIMLockSndHandle;
void KillSound( DWORD *SndHandle );
void setup_no_cockpit_art();
void CTSendToDivertField(PlaneParams *planepnt);

extern int iWatchMAISends;


extern int g_iBaseSpeechSoundLevel;
extern void do_sim_done(VKCODE vk);
extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
extern TargetInfoType *Primary;
extern WeapStoresType WeapStores;
BYTE	bBallCalled = 0;

extern AvRGB *HUDColorRGB;
extern AvRGB HUDColorList[11];


//**************************************************************************************
int AICAllowThisRadio(int planenum, int priority)
{
//	if(!MultiPlayer)
//		return(100);
	float tdist;
	PlaneParams *planepnt = &Planes[planenum];
	FPointDouble position = Camera1.CameraLocation;  //  PlayerPlane->WorldPosition;
	PlaneParams *playerlead;  //  , *leadplane;

	if(Planes[planenum].AI.iSide != PlayerPlane->AI.iSide)
	{
		return(0);
	}

	if((priority >= 76) && (priority < 79) && (planepnt != PlayerPlane))
	{
		if(iATCRadio == 0)
		{
			return(0);
		}

		if((priority == 77) || (priority == 78))
		{
			tdist = Planes[planenum].WorldPosition - position;
			tdist *= WUTONM;
			if(tdist > 70.0f)
			{
				return(0);
			}
			
			if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
			{
				return(0);
			}

			if((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 > 7) && (PlayerPlane->AI.lVar2 <= 10)))
			{
				return(0);
			}
		}
		else if(priority == 76)
		{
			tdist = Planes[planenum].WorldPosition - position;
			tdist *= WUTONM;
			if(tdist > 150.0f)
			{
				return(0);
			}
			if((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 > 7) && (PlayerPlane->AI.lVar2 <= 10)))
			{
				return(0);
			}
		}
	}
	else if((priority >= 76) && (priority < 79) && (planepnt == PlayerPlane) && (iATCRadio == 0))
	{
		return(0);
	}

	if(planepnt != PlayerPlane)
	{
		if(AIInPlayerGroup(planepnt))
		{
			if((planepnt->Status & PL_AI_DRIVEN) && (!(planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)) && (iWingmanRadio == 0))
				return(0);
		}
		else if(iOtherRadio == 0)
		{
			if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_C3)))
			{
				playerlead = AIGetLeader(PlayerPlane);
				if(planepnt->AI.LinkedPlane != playerlead)
				{
					tdist = Planes[planenum].WorldPosition - position;
					tdist *= WUTONM;

					if(tdist > iOtherRadioRangeNM)
					{
						return(0);
					}
				}
			}
		}
	}

	return(100);
}


//**************************************************************************************
void AICNotifyShipInbound(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	char shipsign[256];
	char radial[256];
	char range[256];
	char altitude3[256];
	char fuelstate[256];
	float tbearing;
	int bearing360;
	double dx, dz, offangle;
	MovingVehicleParams *carrier;
	double tdist;
	PlaneParams *planepnt = &Planes[planenum];
	long lworkvar;
	float fworkvar;

	carrier = &MovingVehicles[Planes[planenum].AI.iHomeBaseId];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	if(targetnum == 1)
	{
		if(!AICAllowThisRadio(planenum, 76))
		{
			return;
		}

		if(!LANGGetTransMessage(shipsign, 256, AIC_STRIKE, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!AICAllowThisRadio(planenum, 77))
		{
			return;
		}

		if(!LANGGetTransMessage(shipsign, 256, AIC_MARSHAL, g_iLanguageId))
		{
			return;
		}
	}

	dx = planepnt->WorldPosition.X - carrier->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - carrier->WorldPosition.Z;
	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	tbearing = -offangle;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(radial, "%03d", bearing360);

	tdist = (planepnt->WorldPosition - carrier->WorldPosition) * WUTONM;
//	AICGetNumberText(range, tdist);
	AICGetRangeText(range, tdist);

	fworkvar = ((planepnt->WorldPosition.Y * WUTOFT) + 500.0f) / 1000.0f;
	lworkvar = fworkvar;
	sprintf(tempstr, "%d", lworkvar);
	if(!LANGGetTransMessage(altitude3, 256, AIC_ANGELS, g_iLanguageId, tempstr))
	{
		return;
	}

	fworkvar = planepnt->TotalFuel / 1000.0f;
	sprintf(tempstr2, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr2))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	AICGetCallSign(planenum, callsign);
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

//	strcat(callsign, " ");
//	strcat(callsign, tempstr2);

	AIC_Get_Callsign_With_Number(planenum, callsign);

//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, AIC_TELL_STRIKE_INBOUND, g_iLanguageId, shipsign, callsign, radial, range, altitude3, fuelstate))
	{
		return;
	}

	AIRGenericSpeech(AICF_TELL_STRIKE_INBOUND, planenum, targetnum, offangle, 0, 0, tdist, lworkvar * 1000, 0, 0, 0, 0);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICStrikeClear(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 76))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	AIC_Get_Callsign_With_Number(planenum, callsign);

//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_CLEARED_IN, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_STRIKE_CLEARED_IN, 1, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_STRIKE);
	return;
}

//**************************************************************************************
void AICStrikeContactMarshal(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 76))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeContactMarshal(planenum, placeingroup, tempnum);

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, AIC_SWITCH_MARSHAL, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_SWITCH_MARSHAL, 1, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_STRIKE);
	return;
}

//**************************************************************************************
void AICGetNumberText(char *numstr, float numval)
{
	long lnumval;
	long worknum;
//	char tstr[128];

	lnumval = (long)numval;

	if(lnumval <= 80)
	{
		if(lnumval > 60)
		{
			lnumval /= 5;
			lnumval *= 5;
		}

		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numval, g_iLanguageId))
		{
			return;
		}
	}
	else if(lnumval <= 200)
	{
		if(!LANGGetTransMessage(numstr, 256, AIC_EIGHTY + (((lnumval / 5) - 16)), g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		worknum = lnumval / 10;
		worknum *= 10;
		sprintf(numstr, "%ld", worknum);
	}
}

//**************************************************************************************
void AICMarshalGivePushTime(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[1024];
	char callsign[256];
	int placeingroup;
	char radial[256];
	char range[256];
	char altitude3[256];
	float tbearing;
	int bearing360;
	double dx, dz, offangle;
	MovingVehicleParams *carrier;
	PlaneParams *planepnt = &Planes[planenum];
	long lworkvar;
	char ceilingstr[256];
	char visstr[256];
	float angels;
	int minutes, minutes1;
	char cminstr[80];
	char pminstr[80];
	long visrange;
	int addcallsign = 0;
	int numids = 0;
	int sndids[20];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	carrier = &MovingVehicles[Planes[planenum].AI.iHomeBaseId];
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	AICGetCarrierCeiling(Planes[planenum].AI.iHomeBaseId, ceilingstr);
	visrange = AICGetCarrierVisibility(Planes[planenum].AI.iHomeBaseId, visstr);


	if(((Planes[planenum].AI.winglead < 0) && (Planes[planenum].AI.prevpair < 0)) || (planenum == (PlayerPlane - Planes)))
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_MARSHAL_CHECK, g_iLanguageId, callsign, ceilingstr, visstr))
		{
			return;
		}
		AICAddAIRadioMsgs(tempstr, 40);
		AICMarshalCheckSnd(planenum, visrange);
	}
	else
	{
		addcallsign = 1;
	}

	dx = planepnt->AI.WayPosition.X - carrier->WorldPosition.X;
	dz = planepnt->AI.WayPosition.Z - carrier->WorldPosition.Z;
	offangle = atan2(-dx, -dz) * 57.2958;

	offangle = AICapAngle(offangle);

	tbearing = -offangle;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(radial, "%03d", bearing360);

	angels = ((planepnt->AI.TargetPos.Y * WUTOFT) + 500.0f) / 1000.0f;
	lworkvar = angels;
	sprintf(tempstr, "%d", lworkvar);
#if 0
	if(!LANGGetTransMessage(altitude3, 256, AIC_ANGELS, g_iLanguageId, tempstr))
	{
		return;
	}
#else
	strcpy(altitude3, tempstr);
#endif

	AICGetRangeText(range, angels + 15);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_CASE_III, g_iLanguageId, radial, range, altitude3))
	{
		return;
	}

	if(addcallsign)
	{
		sprintf(tempstr2, "%s, %s", callsign, tempstr);
		AICAddAIRadioMsgs(tempstr2, 40);

		sndids[numids] = AIRGetCallSignID(planenum) + SPCH_MARSHAL;
		numids ++;

		sndids[numids] = SPCH_ZERO + Planes[planenum].AI.cCallSignNum + SPCH_MARSHAL;
		numids ++;

		sndids[numids] = AIRWingNumSndID(planenum, placeingroup) + SPCH_MARSHAL;
		numids ++;

		sndids[numids] = DEAD_SPACE;
		numids ++;

		sndids[numids] = DEAD_SPACE;
		numids ++;

		AIRSendSentence(planenum, numids, sndids, SPCH_MARSHAL);
	}
	else
	{
		AICAddAIRadioMsgs(tempstr, 40);
	}

	minutes1 = ((int)WorldParams.WorldTime%3600)/60;
	sprintf(cminstr, "%d", minutes1);

	minutes = targetnum;
	sprintf(pminstr, "%d", minutes);

//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, AIC_EXPECTED_APPROACH, g_iLanguageId, pminstr, cminstr))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AICMarshalGivePushTimeSnd(planenum, offangle, angels + 15, angels * 1000, minutes, minutes1);
	return;
}

//**************************************************************************************
void AICMarshalCheckSnd(int planenum, long visrange)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;
	int tempval;
	int ceiling;
	float fceiling;

	numids = 0;

	channel =  Planes[planenum].AI.lAIVoice;  //  AIRIsFighter(planenum);

	if(!LANGGetTransMessage(tempstr, 1024, AICF_MARSHAL_CHECK, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, 2, planenum, 0, 0, 0, (float)visrange, 0, 0, 0, SPCH_MARSHAL);
		}
		else if(spchvar == 2)
		{
			if(numval == 3)
			{
				if(!(WorldParams.Weather & WR_CLOUD1))
				{
					ceiling = 100;
				}
				else
				{
					fceiling = WorldParams.CloudAlt * WUTOFT;
					fceiling /= 1000;
					ceiling = fceiling;
				}


				if(ceiling > 36)
				{
					sndids[numids] = (300) + SPCH_MARSHAL;
					numids ++;
				}
				else
				{
					if(ceiling < 1)
					{
						ceiling = 1;
					}

					if((WorldParams.Weather & WR_CLOUD1) == WR_CLOUD1_SCATTERED)
					{
						sndids[numids] = (301) + SPCH_MARSHAL;
						numids ++;
					}
					else if((WorldParams.Weather & WR_CLOUD1) == WR_CLOUD1_BROKEN)
					{
						sndids[numids] = (302) + SPCH_MARSHAL;
						numids ++;
					}
					else
					{
						sndids[numids] = (303) + SPCH_MARSHAL;
						numids ++;
					}

					tempval = ceiling / 10;

					sndids[numids] = (SPCH_ZERO + tempval) + SPCH_MARSHAL;
					numids ++;

					tempval = ceiling % 10;

					sndids[numids] = (SPCH_ZERO + tempval) + SPCH_MARSHAL;
					numids ++;

					sndids[numids] = (305) + SPCH_MARSHAL;
					numids ++;
				}

			}
		}
		else
		{
			sndids[numids] = numval + SPCH_MARSHAL;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids, SPCH_MARSHAL);
}

//**************************************************************************************
void AICMarshalGivePushTimeSnd(int planenum, float bearing, long range, float angles, int minutes, int minutes_now)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;
	int tempval;

	numids = 0;

	channel =  Planes[planenum].AI.lAIVoice;  //  AIRIsFighter(planenum);

	if(!LANGGetTransMessage(tempstr, 1024, AICF_CASE_III, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, 2, planenum, bearing, 0, 0, (float)range, (long)angles, 0, 0, SPCH_MARSHAL);
		}
		else if(spchvar == 2)
		{
			if(numval == 1)
			{
				tempval = minutes / 10;
				sndids[numids] = (SPCH_ZERO + tempval) + SPCH_MARSHAL;
				numids ++;
				tempval = minutes % 10;
				sndids[numids] = (SPCH_ZERO + tempval) + SPCH_MARSHAL;
				numids ++;
			}
			else if(numval == 2)
			{
				tempval = minutes_now / 10;
				sndids[numids] = (SPCH_ZERO + tempval) + SPCH_MARSHAL;
				numids ++;
				tempval = minutes_now % 10;
				sndids[numids] = (SPCH_ZERO + tempval) + SPCH_MARSHAL;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + SPCH_MARSHAL;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids, SPCH_MARSHAL);
}

//**************************************************************************************
long AICGetCarrierWindDirSpeed(int planenum, char *winddirstr, long *windspeednum)
{
	float fwindbearing;
	int windbearing;
	char bearstr[6];
	char windspeedstr[256];

	fwindbearing = 360.0f * frand();
	windbearing = fwindbearing;
	windbearing /= 10;
	if(windbearing >= 36)
	{
		windbearing = 35;
	}

	sprintf(bearstr, "%03d", windbearing * 10);

	*windspeednum = AICGetWindSpeed(planenum, windspeedstr);

	if(*windspeednum == 0)
	{
		bearstr[0] = 0;
		bearstr[1] = 0;

		if(!LANGGetTransMessage(winddirstr, 256, AIC_TOWER_WINDS_ARE_CALM, g_iLanguageId))
		{
			return(windbearing);
		}
	}
	else if(!LANGGetTransMessage(winddirstr, 256, AIC_TOWER_WINDS_ARE, g_iLanguageId, bearstr, windspeedstr))
	{
		return(windbearing);
	}

	return(windbearing);
}

//**************************************************************************************
long AICGetCarrierWindSpeed(int planenum, char *windspeedstr)
{
	float fwindvel;
	int windvel;
	char tempstr2[256];

	if((WorldParams.Weather & (WR_VIS_LOW|WR_CLOUD1_OVERCAST)) == (WR_VIS_LOW | WR_CLOUD1_OVERCAST))
	{
		fwindvel = frand() * 34.0f;
	}
	else
	{
		fwindvel = frand() * 19.0f;
	}

	windvel = fwindvel;
	windvel /= 5;
	if(windvel > 5)
	{
		windvel = 5;
	}

	if(windvel)
	{

#if 1
		sprintf(tempstr2, "%d", windvel * 5);
#else
		if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + (windvel * 5), g_iLanguageId))
		{
			return(windvel);
		}
#endif

		if(!LANGGetTransMessage(windspeedstr, 256, AIC_TOWER_WIND_SPEED, g_iLanguageId, tempstr2))
		{
			return(windvel);
		}
	}
	else
	{
		if(!LANGGetTransMessage(windspeedstr, 256, AIC_TOWER_WINDS_CALM, g_iLanguageId))
		{
			return(windvel);
		}
	}

	return(windvel);
}

//**************************************************************************************
long AICGetCarrierCeiling(int planenum, char *ceilingstr)
{
	int ceiling;
	char tempstr2[256];
	float fceiling;

	if(!(WorldParams.Weather & WR_CLOUD1))
	{
		ceiling = 100;
	}
	else
	{
		fceiling = WorldParams.CloudAlt * WUTOFT;
		fceiling /= 1000;
		ceiling = fceiling;
	}


	if(ceiling < 1)
	{
		ceiling = 1;
	}
	else if(ceiling > 36)
	{
		ceiling = 36;

		if(!LANGGetTransMessage(ceilingstr, 256, AIC_UNLIMITED, g_iLanguageId, tempstr2))
		{
			return(ceiling);
		}
		return(ceiling);
	}

	sprintf(tempstr2, "%d000", ceiling);

	if((WorldParams.Weather & WR_CLOUD1) == WR_CLOUD1_SCATTERED)
	{
		if(!LANGGetTransMessage(ceilingstr, 256, AIC_SCATTERED_CD, g_iLanguageId, tempstr2))
		{
			return(ceiling);
		}
	}
	else if((WorldParams.Weather & WR_CLOUD1) == WR_CLOUD1_BROKEN)
	{
		if(!LANGGetTransMessage(ceilingstr, 256, AIC_BROKEN_CD, g_iLanguageId, tempstr2))
		{
			return(ceiling);
		}
	}
	else
	{
		if(!LANGGetTransMessage(ceilingstr, 256, AIC_OVERCAST_CD, g_iLanguageId, tempstr2))
		{
			return(ceiling);
		}
	}
	return(ceiling);
}

//**************************************************************************************
long AICGetCarrierVisibility(int planenum, char *visiblestr)
{
	float fvis;
	int visibility;
//	char tempstr2[256];
	char rangestr[256];

//	visibility = 100;

	fvis = CurrentMaxDistance * WUTONM;
	visibility = fvis;
	if(visibility > 25)
	{
		visibility = 25;
	}

	AICGetRangeText(rangestr, visibility);

//	sprintf(tempstr2, "%d", visibility);

	if(!LANGGetTransMessage(visiblestr, 256, AIC_VISIBILITY, g_iLanguageId, rangestr))
	{
		return(visibility);
	}

	return(visibility);
}

//**************************************************************************************
void AICEstablishedMarshal(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	char fuelstate[256];
	char altitude3[256];
	PlaneParams *planepnt = &Planes[planenum];
	long lworkvar;
	float fworkvar;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	fworkvar = planepnt->TotalFuel / 1000.0f;
	sprintf(tempstr2, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr2))
	{
		return;
	}

	fworkvar = ((planepnt->WorldPosition.Y * WUTOFT) + 500.0f) / 1000.0f;
	lworkvar = fworkvar;
	sprintf(tempstr, "%d", lworkvar);
	if(!LANGGetTransMessage(altitude3, 256, AIC_ANGELS, g_iLanguageId, tempstr))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_ESTABLISHED_MARSHAL, g_iLanguageId, callsign, altitude3, fuelstate))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_ESTABLISHED_MARSHAL, planenum, 0, 0, 0, 0, 0, Planes[planenum].WorldPosition.Y * WUTOFT);

	return;
}

//**************************************************************************************
void AICCommencingPush(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	char fuelstate[256];
	PlaneParams *planepnt = &Planes[planenum];
	float fworkvar;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	fworkvar = planepnt->TotalFuel / 1000.0f;
	sprintf(tempstr2, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr2))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_COMMENCING_PUSH, g_iLanguageId, callsign, fuelstate))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_COMMENCING_PUSH, planenum);

	return;
}

//**************************************************************************************
void AICPlatform(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	char fuelstate[256];
	PlaneParams *planepnt = &Planes[planenum];
	float fworkvar;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	fworkvar = planepnt->TotalFuel / 1000.0f;
	sprintf(tempstr2, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr2))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_PLATFORM, g_iLanguageId, callsign, fuelstate))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_PLATFORM, planenum);
	return;
}

//**************************************************************************************
void AICLandingDistance(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	char rangestr[256];
	char fuelstate[256];
	PlaneParams *planepnt = &Planes[planenum];
	float fworkvar;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	AICGetRangeText(rangestr, (float)targetnum);

	fworkvar = planepnt->TotalFuel / 1000.0f;
	sprintf(tempstr2, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr2))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_LANDING_DISTANCE, g_iLanguageId, callsign, rangestr, fuelstate))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_LANDING_DISTANCE, planenum, 0, 0, 0, 0, (float)targetnum);
	return;
}

//**************************************************************************************
void AIC_ACLSContact(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;
	PlaneParams *planepnt = &Planes[planenum];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 78))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_FINAL_ACLS_CONTACT, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_FINAL_ACLS_CONTACT, 3, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_FINAL);

	AICAddSoundCall(AIC_ACLSDisplay, planenum, 6000, 50);

	return;
}

//**************************************************************************************
void AIC_ACLSDisplay(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	PlaneParams *planepnt = &Planes[planenum];
	int msgnum, msgsnd;
	double offFB, rdist, offFBP;
	MovingVehicleParams *carrier;
	double offangle, offpitch;
	double deckangle = DECK_ANGLE;
	double tdist;
	int voicenum = 0;
	float onslope = 0.5;
	float onslopewarn = 0.25f;
	float offslopeslight = 1.25f;
	float offslope = 4.25f;
	float online = 0.5;
	float onlinewarn = 0.25f;
	float offlineslight = 1.25f;
	float offline = 4.25f;
	FPoint deckpoint;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 78))
	{
		return;
	}

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	deckpoint = carrier->WorldPosition;
	AIOffsetFromPoint(&deckpoint, AIConvertAngleTo180Degree(carrier->Heading), landx, landy, landz);

	offFB = AIDegreesOffGlideSlope(planepnt, &rdist, &offFBP, &deckpoint);
	offFBP = offFBP - dGlideSlope;

	offangle = AIConvertAngleTo180Degree(carrier->Heading) + deckangle - AIConvertAngleTo180Degree(planepnt->Heading);
	offangle = AICapAngle(offangle);

	offpitch = AICapAngle(3.50f + AIConvertAngleTo180Degree(AIGetPlanesFlightPitch(planepnt)));
	offpitch = AICapAngle(offpitch);

	tdist = deckpoint - planepnt->WorldPosition;

#ifdef _DEBUG
	sprintf(tempstr, "Plane %d offFB %2.2f, FBP %2.2f, offA %2.2f, offP %2.2f, dist %2.1f", planepnt - Planes, offFB, offFBP, offangle, offpitch, tdist * WUTOFT);
	AICAddAIRadioMsgs(tempstr, 40);
#endif

	if(fabs(offFBP) > onslope)  // little high/low
	{
//		if((offFBP < 0) && (offpitch >= 0))
		if((offFBP < 0))
		{
			msgnum = AIC_ACLS_FLY_UP;
			msgsnd = AICF_ACLS_FLY_UP;
		}
//		else if((offFBP > 0) && (offpitch <= 0))
		else if((offFBP >= 0))
		{
			msgnum = AIC_ACLS_FLY_DOWN;
			msgsnd = AICF_ACLS_FLY_DOWN;
		}
	}
	else
	{
		msgnum = AIC_ACLS_ON_GLIDESLOPE;
		msgsnd = AICF_ACLS_ON_GLIDESLOPE;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr2, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}
	AIRGenericSpeech(msgsnd, planenum);

	if(fabs(offFB) > online)		//  little left/right
	{
		if(offFB > 0)
		{
			msgnum = AIC_ACLS_FLY_LEFT;
			msgsnd = AICF_ACLS_FLY_LEFT;
		}
		else
		{
			msgnum = AIC_ACLS_FLY_RIGHT;
			msgsnd = AICF_ACLS_FLY_RIGHT;
		}
	}
	else							//  on glideslope
	{
		msgnum = AIC_ACLS_ON_CENTERLINE;
		msgsnd = AICF_ACLS_ON_CENTERLINE;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(msgsnd, planenum);

	AICAddSoundCall(AIC_ACLSConcur, planenum, 4000, 50);
	return;
}

//**************************************************************************************
void AIC_ACLSConcur(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;
	PlaneParams *planepnt = &Planes[planenum];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 78))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_CONCUR, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_CONCUR	, 3, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_FINAL);

	return;
}

//**************************************************************************************
void AICCallBall(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;
	PlaneParams *planepnt = &Planes[planenum];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 78))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	bBallCalled = 0;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_FINAL_CALL_BALL, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_FINAL_CALL_BALL , 3, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_FINAL);
	return;
}

//**************************************************************************************
void AICSeeBall(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
//	char rangestr[256];
	char fuelstate[256];
	PlaneParams *planepnt = &Planes[planenum];
	float fworkvar;
	int msgnum = AIC_S_HORNET_BALL;
	int planeid = pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID;
	int msgsnd = AICF_S_HORNET_BALL;

	if((iAICommFrom < 0) && ((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14))))))
		return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 78))
	{
		return;
	}

	switch(planeid)
	{
		case 3:
		case 60:
			msgnum = AIC_HORNET_BALL;
			msgsnd = AICF_HORNET_BALL;
			break;
		case 18:
			msgnum = AIC_PROWLER_BALL;
			msgsnd = AICF_PROWLER_BALL;
			break;
		case 4:
		case 62:
			msgnum = AIC_TOMCAT_BALL;
			msgsnd = AICF_TOMCAT_BALL;
			break;
		case 64:
			msgnum = AIC_VIKING_BALL;
			msgsnd = AICF_VIKING_BALL;
			break;
		case 63:
			msgnum = AIC_HAWKEYE_BALL;
			msgsnd = AICF_HAWKEYE_BALL;
			break;
		case 61:
		default:
			msgnum = AIC_S_HORNET_BALL;
			msgsnd = AICF_S_HORNET_BALL;
			break;
	}

	if(targetnum == 1)
	{
		msgnum = AIC_CLARA;
		msgsnd = AICF_CLARA;

		AICAddSoundCall(AICRogerBall, planenum, 5000, 40, 2);
	}
	else if(targetnum == 2)
	{
		msgnum = AIC_ABEAM;
		msgsnd = AICF_ABEAM;
	}
	else
	{
		AICAddSoundCall(AICRogerBall, planenum, 5000, 40, -1);
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

//	AICGetNumberText(rangestr, (float)targetnum);

	fworkvar = planepnt->TotalFuel / 1000.0f;
	sprintf(tempstr2, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr2))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, fuelstate))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICRogerBall(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;
	PlaneParams *planepnt = &Planes[planenum];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 78))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	if(targetnum == 2)
	{
		AICJustMessage(AIC_KEEP_COMING, AICF_KEEP_COMING, SPCH_LSO);
		if(planenum == (PlayerPlane - Planes))
		{
			PlayerPlane->AI.lVar2 = 14;
		}
		return;
	}

	bBallCalled = 1;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, AIC_ROGER_BALL, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_ROGER_BALL , 4, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_LSO);
	return;
}

//**************************************************************************************
void AICCheckPlayerCarrierLanding()
{
	double tdist;
	MovingVehicleParams *carrier;
	double deckangle = DECK_ANGLE;
	FPoint deckpoint;
	double offFB, rdist, offFBP;
	float offangle;
	ANGLE dhead;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;
	FPoint	fptemp;

//	ShowLandingGates(PlayerPlane);

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	if((PlayerPlane->AI.iAICombatFlags1 & AI_HOME_AIRPORT) || (PlayerPlane->AI.iHomeBaseId < 0))
	{
		return;
	}

	if((PlayerPlane->AI.Behaviorfunc == AIFlyTankerFormation) || (PlayerPlane->AI.Behaviorfunc == AITankerDisconnect))
	{
		return;
	}

	if((PlayerPlane->AI.lVar2 >= 100) || ((PlayerPlane->Status & PL_AI_DRIVEN) && ((PlayerPlane->AI.Behaviorfunc == CTDoCarrierLaunch) || (PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch))))
	{
		AICCheckPlayerTakeOff();
		return;
	}

	carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	if(PlayerPlane->AI.lVar2 >= 3)
	{
		deckpoint = carrier->WorldPosition;
		AIOffsetFromPoint(&deckpoint, AIConvertAngleTo180Degree(carrier->Heading), landx, landy, landz);

		offFB = AIDegreesOffGlideSlope(PlayerPlane, &rdist, &offFBP, &deckpoint);
		offFBP = offFBP - dGlideSlope;
	}

	if(carrier->lAIFlags1 & V_FLASH_WAVEOFF)
	{
		if(PlayerPlane->AI.lVar2 != 11)
		{
			carrier->lAIFlags1 &= ~(V_FLASH_WAVEOFF|V_FLASH_E_WAVEOFF);
		}
		else
		{
			tdist = (carrier->WorldPosition - PlayerPlane->WorldPosition);
			if(tdist > NMTOWU)
			{
				carrier->lAIFlags1 &= ~(V_FLASH_WAVEOFF|V_FLASH_E_WAVEOFF);
			}
			else
			{
				dhead = (carrier->Heading - PlayerPlane->Heading);
				if((dhead < 0xC000) && (dhead > 0x4000))
				{
					carrier->lAIFlags1 &= ~(V_FLASH_WAVEOFF|V_FLASH_E_WAVEOFF);
				}
			}
		}
	}

	if(PlayerPlane->AI.lVar2 < 11)
	{
		if(UFC.APStatus)
		{
			if(PlayerPlane->Status & PL_DEVICE_DRIVEN)
			{
				AIC_ACLS_Switch(PlayerPlane, 1);
			}
			else if(!((PlayerPlane->AI.Behaviorfunc == AIFlyToDME3) || (PlayerPlane->AI.Behaviorfunc == AIMoveToParkingSpace) ||
					(PlayerPlane->AI.Behaviorfunc == AIFlyCarrierFinal) || (PlayerPlane->AI.Behaviorfunc == AIFlyBolter) || (PlayerPlane->AI.Behaviorfunc == AICarrierTrap) || (PlayerPlane->AI.Behaviorfunc == AIWaitForTrapOrBolter)))
			{
				AIC_ACLS_Switch(PlayerPlane, 1);
			}
		}
		else
		{
			if(!(PlayerPlane->Status & PL_DEVICE_DRIVEN))
			{
				if(!((PlayerPlane->AI.Behaviorfunc == AIFlyToDME3) || (PlayerPlane->AI.Behaviorfunc == AIMoveToParkingSpace) ||
						(PlayerPlane->AI.Behaviorfunc == AIFlyCarrierFinal) || (PlayerPlane->AI.Behaviorfunc == AIFlyBolter) || (PlayerPlane->AI.Behaviorfunc == AICarrierTrap) || (PlayerPlane->AI.Behaviorfunc == AIWaitForTrapOrBolter)
						|| (PlayerPlane->AI.Behaviorfunc == AIFlyTankerFormation) || (PlayerPlane->AI.Behaviorfunc == AITankerDisconnect)))
				{
					AIC_ACLS_Switch(PlayerPlane, 0);
				}
			}
		}
	}

	if(PlayerPlane->AI.Behaviorfunc == AICarrierTrap)
	{
		if((PlayerPlane->OnGround == 2) && (PlayerPlane->AI.lVar3))
		{
			PlayerPlane->AI.iAICombatFlags1 &= ~(AI_CARRIER_LANDING);
		}
	}

	if(!(PlayerPlane->Status & AL_AI_DRIVEN))
	{
		if(0 <= PlayerPlane->AI.lTimer2)
		{
			PlayerPlane->AI.lTimer2 -= DeltaTicks;
		}
	}

	PlayerPlane->AI.WayPosition = carrier->WorldPosition;
	switch(PlayerPlane->AI.lVar2)
	{
		case -2:
		case -1:
		case 0:
			if(PlayerPlane->AI.lVar2 != -2)
			{
				tdist = PlayerPlane->WorldPosition - carrier->WorldPosition;
				if(tdist < (50.0f * NMTOWU))
				{
					if(PlayerPlane->AI.lVar2 == -1)
					{
						AICAddSoundCall(AICStrikeContactMarshal, (PlayerPlane - Planes), 12000, 40, 1);
						PlayerPlane->AI.lVar2 = -2;
					}
					else
					{
						AIContactCarrierMarshallPattern(PlayerPlane);
						PlayerPlane->AI.lTimer2 = PlayerPlane->AI.lTimer3;
						PlayerPlane->AI.lVar2 = 1;
					}
				}
			}
			break;
		case 1:
			SetACLDataForAvionics(300, PlayerPlane->AI.TargetPos.Y, 4000, offFB, offFBP);
			AISetOffCarrierRelPoint(PlayerPlane, 0, PlayerPlane->AI.TargetPos.Y, PlayerPlane->AI.TargetPos.Z + (5.0f * NMTOWU), PlayerPlane->AI.lVar2, deckangle);
			if(fabs(PlayerPlane->WorldPosition.Y - PlayerPlane->AI.TargetPos.Y) < (500.0f * FTTOWU))
			{
				AICEstablishedMarshal(PlayerPlane - Planes);
				PlayerPlane->AI.lVar2 = 2;
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_ESTABLISHED);
			}
			if(PlayerPlane->AI.lTimer2 < 0)
			{
				AICCommenceNow(PlayerPlane - Planes);
				AICAddSoundCall(AICCommencingPush, (PlayerPlane - Planes), 5000, 40, -1);
				PlayerPlane->AI.lVar2 = 4;
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_COMMENCE_NOW);
			}
			break;
		case 2:
			SetACLDataForAvionics(300, PlayerPlane->AI.TargetPos.Y, 4000, offFB, offFBP);
			AISetOffCarrierRelPoint(PlayerPlane, 0, PlayerPlane->AI.TargetPos.Y, PlayerPlane->AI.TargetPos.Z + (5.0f * NMTOWU), PlayerPlane->AI.lVar2, deckangle);
			if(fabs(PlayerPlane->WorldPosition.Y - PlayerPlane->AI.TargetPos.Y) > (600.0f * FTTOWU))
			{
				AICReturnToMarshall(PlayerPlane - Planes);
				PlayerPlane->AI.lVar2 = 3;
				NetPutGenericMessage1(PlayerPlane, GM_RETURN_MARSHAL);
			}
			if(PlayerPlane->AI.lTimer2 < 0)
			{
				AICCommenceNow(PlayerPlane - Planes);
				AICAddSoundCall(AICCommencingPush, (PlayerPlane - Planes), 5000, 40, -1);
				PlayerPlane->AI.lVar2 = 4;
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_COMMENCE_NOW);
			}
			break;
		case 3:
			SetACLDataForAvionics(300, PlayerPlane->AI.TargetPos.Y, 4000, offFB, offFBP);
			AISetOffCarrierRelPoint(PlayerPlane, 0, PlayerPlane->AI.TargetPos.Y, PlayerPlane->AI.TargetPos.Z + (5.0f * NMTOWU), PlayerPlane->AI.lVar2, deckangle);
			if(fabs(PlayerPlane->WorldPosition.Y - PlayerPlane->AI.TargetPos.Y) < (500.0f * FTTOWU))
			{
				PlayerPlane->AI.lVar2 = 2;
			}
			if(PlayerPlane->AI.lTimer2 < 0)
			{
				AICCommenceNow(PlayerPlane - Planes);
				AICAddSoundCall(AICCommencingPush, (PlayerPlane - Planes), 5000, 40, -1);
				PlayerPlane->AI.lVar2 = 4;
				SetACLDataForAvionics(250, 1200, 4000, offFB, offFBP);
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_COMMENCE_NOW);
			}
			break;
		case 4:
			SetACLDataForAvionics(250, 1200, 4000, offFB, offFBP);
			if(PlayerPlane->WorldPosition.Y < (5000.0f * FTTOWU))
			{
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_PLATFORM);
				AICPlatform(PlayerPlane - Planes);
				PlayerPlane->AI.lVar2 = 5;
				SetACLDataForAvionics(250, 1200, 2000, offFB, offFBP);
			}
		case 5:
			tdist = PlayerPlane->WorldPosition - carrier->WorldPosition;
			SetACLDataForAvionics(250, 1200, 2000, offFB, offFBP);
			if(tdist < (10.0f * NMTOWU))
			{
				NetPutGenericMessage2(PlayerPlane, GM2_LANDING_DIST, 10);
				AICLandingDistance(PlayerPlane - Planes, 10);
				PlayerPlane->AI.lVar2 += 2;
			}
			break;
		case 6:
			SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20.0f, 1200, 4000, offFB, offFBP);
			if(PlayerPlane->WorldPosition.Y < (5000.0f * FTTOWU))
			{
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_PLATFORM);
				AICPlatform(PlayerPlane - Planes);
				PlayerPlane->AI.lVar2 = 7;
				SetACLDataForAvionics(250, 1200, 2000, offFB, offFBP);
			}
		case 7:
//			tdist = PlayerPlane->WorldPosition - carrier->WorldPosition;
			SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20.0f, 1200, 2000, offFB, offFBP);
			if(rdist < (5.0f * NMTOWU))
			{
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
				NetPutGenericMessage1(PlayerPlane, GM_ACLS_CONTACT);
				AIC_ACLSContact(PlayerPlane - Planes);
				PlayerPlane->AI.lVar2 = 8;
			}
			break;
		case 8:
//			tdist = PlayerPlane->WorldPosition - carrier->WorldPosition;
			SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20.0f, 1200, 2000, offFB, offFBP);
			if(rdist < (3.0f * NMTOWU))
			{
				PlayerPlane->AI.lVar2 = 9;
				SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20, 600, 2000, offFB, offFBP);
			}
			break;
		case 9:
			SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20, 600, 2000, 0, offFBP);
//			tdist = PlayerPlane->WorldPosition - carrier->WorldPosition;
			if(rdist < (0.95f * NMTOWU))
			{
				if(AICDeckClear(PlayerPlane))
				{
					AICCallBall(PlayerPlane - Planes);
					PlayerPlane->AI.lVar2 = 13;

					//  Might want to keep this in simple mode?
//					AICAddSoundCall(AICSeeBall, (PlayerPlane - Planes), 5000, 40, -1);
//					PlayerPlane->AI.lVar2 = 10;
				}
			}
			break;
		case 11:
			SetACLDataForAvionics(250, 1200, -2000, offFB, offFBP);
			if(PlayerPlane->HeightAboveGround > (200 * FTTOWU))
			{
				PlayerPlane->AI.iAICombatFlags2 &= ~(AI_BOLTERED);
			}

			if((PlayerPlane->AI.lTimer2 < 0) || (rdist > (4.0f * NMTOWU)))
			{
				PlayerPlane->AI.iAICombatFlags2 &= ~(AI_BOLTERED);
				if(!((PlayerPlane->OnGround == 2) || (PlayerPlane->PlaneCopy)))
				{
					NetPutGenericMessage1(PlayerPlane, GM_BOLTER_RADIO);
					AIC_GenericMsgPlaneBearing(PlayerPlane - Planes, 4, AIConvertAngleTo180Degree(carrier->Heading + 0x8000 + AIConvert180DegreeToAngle(deckangle)));
				}
				PlayerPlane->AI.lVar2 = 12;
				PlayerPlane->AI.lTimer2 = -1;
			}
			break;
		case 12:
//			tdist = PlayerPlane->WorldPosition - carrier->WorldPosition;
			SetACLDataForAvionics(250, 1200, 2000, offFB, offFBP);
			if((rdist > (5.0f * NMTOWU)) && (fabs(offFB) < 60.0f))
			{
				offangle = AIConvertAngleTo180Degree(carrier->Heading) + deckangle;
				offangle = AICapAngle(offangle);

				if(!((PlayerPlane->OnGround == 2) || (PlayerPlane->PlaneCopy)))
				{
					NetPutGenericMessage1(PlayerPlane, GM_BOLTER_RADIO_2);
					AIC_GenericMsgPlaneBearing(PlayerPlane - Planes, 4, offangle);
				}
				PlayerPlane->AI.lVar2 = 7;
			}
			break;
		case 13:	//  Waiting for Ball
		case 14:	//  Got Clara, waiting for LSO paddles
			SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20, 70, 2000, offFB, offFBP);
			AISetOffCarrierRelPoint(PlayerPlane, landx, landy, landz, PlayerPlane->AI.lVar2);
			if((rdist < (NMTOWU * 0.5f)) || ((rdist < CurrentMaxDistance) && (PlayerPlane->AI.lVar2 == 14)))
			{
				AICJustMessage(AIC_PADDLES_CONTACT, AICF_PADDLES_CONTACT, SPCH_LSO);
				PlayerPlane->AI.lVar2 = 10;
				bBallCalled = 1;
			}
			break;
		case 10:
		default:
			SetACLDataForAvionics(pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iStallSpeed + 20, 70, 2000, offFB, offFBP);
			AISetOffCarrierRelPoint(PlayerPlane, landx, landy, landz, PlayerPlane->AI.lVar2);
			if(bBallCalled)
				AICCheckLSOForPlane(PlayerPlane);
			break;
	}
}

//**************************************************************************************
void AICReturnToMarshall(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	PlaneParams *planepnt = &Planes[planenum];
	int msgnum;
	int msgsnd;
	int tempval;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	if((targetnum == -1) || (targetnum > 2))
	{
		if(MultiPlayer)
		{
			tempval = planenum % 3;
		}
		else
		{
			tempval = rand() % 3;
		}
		msgnum = AIC_WHAT_DOING + tempval;
		msgsnd = AICF_WHAT_DOING + tempval;
	}
	else
	{
		msgnum = AIC_WHAT_DOING + targetnum;
		msgsnd = AICF_WHAT_DOING + targetnum;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(msgsnd , 2, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_MARSHAL);
	return;
}

//**************************************************************************************
void AICCommenceNow(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int placeingroup;
	PlaneParams *planepnt = &Planes[planenum];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 77))
	{
		return;
	}

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, AIC_COMMENCE_NOW, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_COMMENCE_NOW, 2, planenum, 0, 0, 0, 0, 0, 0, 0, SPCH_MARSHAL);
	return;
}

//**************************************************************************************
void AICJustMessage(int msgnum, int msgsnd, int voicenum)
{
	char tempstr[1024];
	int voiceval = 1;

	if(voicenum == SPCH_MARSHAL)
	{
		voiceval = 2;
		if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
		{
			return;
		}
	}
	else if(voicenum == SPCH_FINAL)
	{
		voiceval = 3;
		if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
		{
			return;
		}
	}
	else if(voicenum == SPCH_LSO)
	{
		voiceval = 4;
		if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
		{
			return;
		}
	}

#if 0
	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}
#endif

	//  Make sound call
//	AIRStrikeClear(planenum, placeingroup, tempnum);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(msgsnd, voiceval, 0, 0, 0, 0, 0, 0, 0, 0, voicenum);
	return;
}

//**************************************************************************************
void AICCheckLSOForPlane(PlaneParams *planepnt, int firsttime)
{
	double offFB, rdist, offFBP;
	MovingVehicleParams *carrier;
	double offangle, offpitch;
	double deckangle = DECK_ANGLE;
	double tdist;
	int voicenum = SPCH_LSO;
	float onslope = 0.5;
	float onslopewarn = 0.25f;
	float offslopeslight = 1.0f;  //  1.25f;
	float offslope = 1.5f;		 //   4.25f;  This can't be right, it would put me below the deck.
	float online = 0.5;
	float onlinewarn = 0.25f;
	float offlineslight = 1.25f;
	float offline = 4.25f;
	FPoint deckpoint;
	float aoadeg;
	float desiredaoa = AIDESIREDAOA;
	float warndaoa = 2.0f;
	float wavedist = 1500.0f * FTTOWU;
	float intensedist = 2500.0f * FTTOWU;
	long ltimeradd = 0;
	long laddinc = 2000;
	int waveoff = 0;
	int speeddone = 0;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;
	float stallspeed = pDBAircraftList[planepnt->AI.iPlaneIndex].iStallSpeed;

	if(planepnt == PlayerPlane)
	{
		if(planepnt->AI.lTimer2 >= 0)
		{
			return;
		}
		planepnt->AI.lTimer2 = 1000;
	}
	else
	{
		if(planepnt->AI.lTimer3 >= 0)
		{
			return;
		}
		planepnt->AI.lTimer3 = 1000;
	}

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	deckpoint = carrier->WorldPosition;
	AIOffsetFromPoint(&deckpoint, AIConvertAngleTo180Degree(carrier->Heading), landx, landy, landz);

	offFB = AIDegreesOffGlideSlope(planepnt, &rdist, &offFBP, &deckpoint);
	offFBP = offFBP - dGlideSlope;

	offangle = AIConvertAngleTo180Degree(carrier->Heading) + deckangle - AIConvertAngleTo180Degree(planepnt->Heading);
	offangle = AICapAngle(offangle);

	offpitch = AICapAngle(3.50f + AIConvertAngleTo180Degree(AIGetPlanesFlightPitch(planepnt)));
	offpitch = AICapAngle(offpitch);

	if(firsttime)
	{
		planepnt->AI.TargetPos.X = offFB;
		planepnt->AI.TargetPos.Y = offFBP;
		planepnt->AI.TargetPos.Z = planepnt->Knots;
		planepnt->AI.CombatPos = planepnt->AI.TargetPos;
		planepnt->AI.fVarA[0] = offangle;
		planepnt->AI.fVarA[1] = offpitch;
		planepnt->AI.lVar3 = 0;
		return;
	}
	tdist = deckpoint - planepnt->WorldPosition;

#if 0
	if(planepnt != PlayerPlane)
	{
		char tstr[256];
		sprintf(tstr, "Plane %d offFB %2.2f, FBP %2.2f, offA %2.2f, offP %2.2f, dist %2.1f", planepnt - Planes, offFB, offFBP, offangle, offpitch, tdist * WUTOFT);
		AICAddAIRadioMsgs(tstr, 40);
	}
#endif

	if((tdist < (600.0f * FTTOWU)) || (fabs(offFB) > 90))
	{
		return;
	}

	if(tdist < wavedist)
	{
		if(planepnt->Knots > (stallspeed + 70))
		{
			waveoff = 2;
		}
		else if(planepnt->Knots > (stallspeed + 40))
		{
			waveoff = 1;
		}
		else if(offFB < -(fabs(dGlideSlope) - 0.5f))
		{
			waveoff = 3;
		}
		else if(fabs(offFBP) > offslope)  //  way high/low
		{
			waveoff = 1;
		}
		else if(fabs(offFB) > offline)	// way left/right
		{
			waveoff = 1;
		}

		if(waveoff)
		{
			if(waveoff == 3)
			{
				AICJustMessage(AIC_WAVEOFF_3, AICF_WAVEOFF_3, voicenum);
				ltimeradd =+ laddinc;
			}
			else if(waveoff == 2)
			{
				AICJustMessage(AIC_WAVEOFF_INTENSE, AICF_WAVEOFF_INTENSE, voicenum);
				ltimeradd =+ laddinc;
			}
			else
			{
				AICJustMessage(AIC_WAVEOFF, AICF_WAVEOFF, voicenum);
				ltimeradd =+ laddinc;
			}

			if(planepnt == PlayerPlane)
			{
				carrier->lAIFlags1 |= V_FLASH_WAVEOFF;
				if(waveoff > 1)
				{
					carrier->lAIFlags1 |= V_FLASH_E_WAVEOFF;
				}

				AICAddSoundCall(AIC_BolterStart, PlayerPlane - Planes, 9000, 50);
				PlayerPlane->AI.iAICombatFlags2 |= AI_BOLTERED;
				PlayerPlane->AI.lTimer2 = 120000;
				PlayerPlane->AI.lVar2 = 11;

				if(!(PlayerPlane->Status & PL_DEVICE_DRIVEN))
				{
					AIC_ACLS_Switch(PlayerPlane, 0);
				}
				return;
			}
			else if(!((planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
			{
				AICAddSoundCall(AIC_BolterStart, planepnt - Planes, 9000, 50);
				planepnt->AI.Behaviorfunc = AIFlyBolter;
				planepnt->AI.lTimer3 = 120000;
				return;
			}
//			PlayerPlane->AI.lTimer2 = 120000;
//			PlayerPlane->AI.lVar2 = 11;
		}
	}
	else if(tdist < intensedist)
	{
		if(fabs(offFB) > 45.0f)
		{
			if(planepnt == PlayerPlane)
			{
				AIC_BolterStart(PlayerPlane - Planes);
				PlayerPlane->AI.iAICombatFlags2 |= AI_BOLTERED;
				PlayerPlane->AI.lTimer2 = 120000;
				PlayerPlane->AI.lVar2 = 11;
			}
			else if(!((planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
			{
				AIC_BolterStart(planepnt - Planes);
				planepnt->AI.Behaviorfunc = AIFlyBolter;
				planepnt->AI.lTimer3 = 120000;
			}

		}
		return;
	}

	if((planepnt->Knots < 250.0f) && (((carrier->Heading - planepnt->Heading) > 0xE000) || ((carrier->Heading - planepnt->Heading) < 0x2000)))
	{
		if(!(planepnt->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			if(tdist < intensedist)
			{
				AICJustMessage(AIC_DROP_GEAR_INTENSE, AICF_DROP_GEAR_INTENSE, voicenum);
				ltimeradd =+ laddinc;
			}
			else
			{
				AICJustMessage(AIC_DROP_GEAR, AICF_DROP_GEAR, voicenum);
				ltimeradd =+ laddinc;
			}
		}

		if(!(planepnt->Flaps))
		{
			if(tdist < intensedist)
			{
				AICJustMessage(AIC_DROP_FLAPS_INTENSE, AICF_DROP_FLAPS_INTENSE, voicenum);
				ltimeradd =+ laddinc;
			}
			else
			{
				AICJustMessage(AIC_DROP_FLAPS, AICF_DROP_FLAPS, voicenum);
				ltimeradd =+ laddinc;
			}
		}
		if(!planepnt->TailHookState)
		{
			if(tdist < intensedist)
			{
				AICJustMessage(AIC_DROP_HOOK_INTENSE, AICF_DROP_HOOK_INTENSE, voicenum);
				ltimeradd =+ laddinc;
			}
			else
			{
				AICJustMessage(AIC_DROP_HOOK, AICF_DROP_HOOK, voicenum);
				ltimeradd =+ laddinc;
			}
		}
	}

	if((fabs(offFB) > online) || (fabs(offFBP) > onslope))
	{
		planepnt->AI.lVar3 &= ~0x1;
	}
	else if((fabs(offFB) < onlinewarn) && (fabs(offFBP) < onslopewarn))
	{
		planepnt->AI.lVar3 &= ~0x2;
	}
	else if((fabs(offFB) < onlinewarn) && (fabs(offFBP) < onslopewarn))
	{
		planepnt->AI.lVar3 &= ~0x2;
	}

	if(planepnt->Knots > (stallspeed + 30))
	{
		AICJustMessage(AIC_U_FAST, AICF_U_FAST, voicenum);
		ltimeradd =+ laddinc;
		speeddone = 1;
	}

	if(fabs(offFBP) > offslope)  //  way high/low
	{
		if(offFBP < 0)
		{
			aoadeg = RadToDeg(planepnt->Alpha);

			if(speeddone)
			{
				aoadeg = desiredaoa;
			}

			if(aoadeg > (desiredaoa + (warndaoa * 3)))
			{
  				AICJustMessage(AIC_BURNER, AICF_BURNER, voicenum);
				ltimeradd =+ laddinc;
			}
			else if(aoadeg > (desiredaoa + (warndaoa * 2)))
			{
  				AICJustMessage(AIC_POWER_INTENSE, AICF_POWER_INTENSE, voicenum);
				ltimeradd =+ laddinc;
			}
			else if(aoadeg > (desiredaoa + (warndaoa * 1.5)))
			{
				if(tdist < intensedist)
				{
					if(offFB < -dGlideSlope)
					{
		  				AICJustMessage(AIC_BURNER, AICF_BURNER, voicenum);
					}
					else
					{
		  				AICJustMessage(AIC_POWER_INTENSE, AICF_POWER_INTENSE, voicenum);
					}
					ltimeradd =+ laddinc;
				}
				else
				{
					AICJustMessage(AIC_U_UNDERPOWERED, AICF_U_UNDERPOWERED, voicenum);
					ltimeradd =+ laddinc;
				}
			}
			else if(aoadeg > (desiredaoa + warndaoa))
			{
				if(tdist < intensedist)
				{
					if(offFB < -dGlideSlope)
					{
		  				AICJustMessage(AIC_BURNER, AICF_BURNER, voicenum);
					}
					else
					{
		  				AICJustMessage(AIC_POWER_INTENSE, AICF_POWER_INTENSE, voicenum);
					}
					ltimeradd =+ laddinc;
				}
				else
				{
					if(rand() & 0x1)
					{
						AICJustMessage(AIC_U_UNDERPOWERED, AICF_U_UNDERPOWERED, voicenum);
						ltimeradd =+ laddinc;
					}
					else
					{
						AICJustMessage(AIC_LITTLE_POWER, AICF_LITTLE_POWER, voicenum);
						ltimeradd =+ laddinc;
					}
				}
			}
//			else if(((offpitch > planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y > planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			else if((((offpitch < planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y < planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2))) && (offpitch <= 0))
			{
				AICJustMessage(AIC_U_LOW, AICF_U_LOW, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else if(offFBP > 0)
		{
			aoadeg = RadToDeg(planepnt->Alpha);

			if(speeddone)
			{
				aoadeg = desiredaoa;
			}

			if(aoadeg < (desiredaoa - warndaoa))
			{
				AICJustMessage(AIC_U_OVERPOWERED, AICF_U_OVERPOWERED, voicenum);
				ltimeradd =+ laddinc;
			}
//			if(((offpitch < planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y < planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			if((((offFBP > planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y > planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2))) && (offpitch >= 0))
			{
				AICJustMessage(AIC_U_HIGH, AICF_U_HIGH, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if(fabs(offFBP) > offslopeslight)  // high/low
	{
		if((offFBP < 0) && (offpitch < 0))
		{
//			if(((offpitch > planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y > planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			if(((offFBP < planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y < planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_U_LOW, AICF_U_LOW, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else if((offFBP > 0) && (offpitch > 0))
		{
//			if(((offpitch < planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y < planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			if(((offFBP > planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y > planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_U_HIGH, AICF_U_HIGH, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if(fabs(offFBP) > onslope)  // little high/low
	{
		if((offFBP < 0) && (offpitch < 0))
		{
			if(((offFBP < planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y < planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_LITTLE_LOW, AICF_LITTLE_LOW, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else if((offFBP > 0) && (offpitch > 0))
		{
			if(((offFBP > planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y > planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_LITTLE_HIGH, AICF_LITTLE_HIGH, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if((fabs(offFBP) > onslopewarn) && (planepnt->AI.lVar3 & 0x1))  //  check for drifting if near edge of safe area
	{
		if((offFBP < 0) && (offpitch < 0))
		{
			if(((offFBP < planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y < planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_GOING_LOW, AICF_GOING_LOW, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else if((offFBP > 0) && (offpitch > 0))
		{
			if(((offFBP > planepnt->AI.TargetPos.Y) && (planepnt->AI.TargetPos.Y > planepnt->AI.CombatPos.Y)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_GOING_HIGH, AICF_GOING_HIGH, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if(fabs(offFB) > offline)	// way left/right
	{
		if(offFB > 0)
		{
//			if((offangle < planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			if((offFB > planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_COME_LEFT, AICF_COME_LEFT, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else
		{
//			if((offangle > planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			if((offFB < planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_RIGHT_LINEUP, AICF_RIGHT_LINEUP, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if(fabs(offFB) > offlineslight)	//  left/right
	{
		if(offFB > 0)
		{
			if((offFB > planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_COME_LEFT, AICF_COME_LEFT, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else
		{
			if((offFB < planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_RIGHT_LINEUP, AICF_RIGHT_LINEUP, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if(fabs(offFB) > online)		//  little left/right
	{
		if(offFB > 0)
		{
			if((offFB > planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_COME_LITTLE_LEFT, AICF_COME_LITTLE_LEFT, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else
		{
			if((offFB < planepnt->AI.TargetPos.X) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_LITTLE_R_LINEUP, AICF_LITTLE_R_LINEUP, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else if((fabs(offFB) > onlinewarn) && (planepnt->AI.lVar3 & 0x1))	//  check for drifting if near edge of safe area
	{
		if((offFB > 0) && (offangle >= 0))
		{
			if(((offFB > planepnt->AI.TargetPos.X) && (planepnt->AI.TargetPos.X > planepnt->AI.CombatPos.X)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_U_DRIFTING_RIGHT, AICF_U_DRIFTING_RIGHT, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
		else if((offFB < 0) && (offangle <= 0))
		{
			if(((offFB < planepnt->AI.TargetPos.X) && (planepnt->AI.TargetPos.X < planepnt->AI.CombatPos.X)) || (!(planepnt->AI.lVar3 & 0x2)))
			{
				AICJustMessage(AIC_U_DRIFTING_LEFT, AICF_U_DRIFTING_LEFT, voicenum);
				planepnt->AI.lVar3 |= 0x2;
				ltimeradd =+ laddinc;
			}
		}
	}
	else							//  on glideslope
	{
		if(!(planepnt->AI.lVar3 & 1))
		{
			if(fabs(planepnt->AI.TargetPos.X) > 0.5f)
			{
				AICJustMessage(AIC_U_ON_CENTERLINE, AICF_U_ON_CENTERLINE, voicenum);
				ltimeradd =+ laddinc;
			}
			else
			{
				AICJustMessage(AIC_U_ON_GLIDESLOPE, AICF_U_ON_GLIDESLOPE, voicenum);
				ltimeradd =+ laddinc;
			}
			planepnt->AI.lVar3 |= 1;
		}
	}

	if(planepnt == PlayerPlane)
	{
		planepnt->AI.lTimer2 += ltimeradd;
	}
	else
	{
		planepnt->AI.lTimer3 += ltimeradd;
	}

	planepnt->AI.CombatPos = planepnt->AI.TargetPos;
	planepnt->AI.TargetPos.X = offFB;
	planepnt->AI.TargetPos.Y = offFBP;
	planepnt->AI.TargetPos.Z = planepnt->Knots;
	planepnt->AI.fVarA[0] = offangle;
	planepnt->AI.fVarA[1] = offpitch;
}

void AICGetRGBColors(int bcolor, int *redval, int *greenval, int *blueval)
{
	if((bcolor < 0) && (bcolor != -10) && (bcolor != -20) && (bcolor != -30))
	{
		*redval = HUDColorRGB->Red;
		*greenval = HUDColorRGB->Green;
		*blueval = HUDColorRGB->Blue;
		return;
	}

	switch(bcolor)
	{
		case -30:
			*redval = iInactiveCommR;
			*greenval = iInactiveCommG;
			*blueval = iInactiveCommB;
			break;
		case -20:
			*redval = iCommBoxR;
			*greenval = iCommBoxG;
			*blueval = iCommBoxB;
			break;
		case -10:
			*redval = iCommR;
			*greenval = iCommG;
			*blueval = iCommB;
			break;
		case 53:
			*redval = 128;
			*greenval = 128;
			*blueval = 128;
			break;
		case 54:
			*redval = 64;
			*greenval = 64;
			*blueval = 64;
			break;
		case 57:
			*redval = 0;
			*greenval = 64;
			*blueval = 0;
			break;
		case 58:
			*redval = 0;
			*greenval = 96;
			*blueval = 0;
			break;
		case 59:
			*redval = 0;
			*greenval = 128;
			*blueval = 0;
			break;
		case 60:
			*redval = 0;
			*greenval = 160;
			*blueval = 0;
			break;
		case 61:
			*redval = 0;
			*greenval = 192;
			*blueval = 0;
			break;
		case 62:
			*redval = 0;
			*greenval = 224;
			*blueval = 0;
			break;
		case 63:
			*redval = 0;
			*greenval = 255;
			*blueval = 0;
			break;
		case 199:
			*redval = 64;
			*greenval = 64;
			*blueval = 64;
			break;
		case 207:
			*redval = 1;	// 0
			*greenval = 1;	//  128
			*blueval = 1;	//  0
			break;
		case 248:
			*redval = 255;
			*greenval = 0;
			*blueval = 0;
			break;
		case 249:
			*redval = 0;
			*greenval = 168;
			*blueval = 0;
			break;
		case 251:
			*redval = 0;
			*greenval = 188;
			*blueval = 0;
			break;
		case 252:
			*redval = 241;
			*greenval = 167;
			*blueval = 0;
			break;
		case 253:
			*redval = 255;
			*greenval = 255;
			*blueval = 255;
			break;
		case 255:
			*redval = 1;
			*greenval = 1;
			*blueval = 1;
			break;
		default:
			*redval = 128;
			*greenval = 128;
			*blueval = 128;
			break;
	}
}

//**************************************************************************************
void AICWWeaponOptionUnguidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICEWeaponOptionUnguidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICDWeaponOptionUnguidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICFWeaponOptionUnguidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICWWeaponOptionUnguidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICEWeaponOptionUnguidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICDWeaponOptionUnguidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICFWeaponOptionUnguidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICWWeaponOptionUnguidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICEWeaponOptionUnguidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(2, 3, 3);
	return;
}

//**************************************************************************************
void AICDWeaponOptionUnguidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICFWeaponOptionUnguidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICWWeaponOptionGuidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICEWeaponOptionGuidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICDWeaponOptionGuidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICFWeaponOptionGuidedAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICWWeaponOptionGuidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICEWeaponOptionGuidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICDWeaponOptionGuidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICFWeaponOptionGuidedHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICWWeaponOptionGuidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICEWeaponOptionGuidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICDWeaponOptionGuidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICFWeaponOptionGuidedSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICWWeaponOptionMissileAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 7);
	return;
}

//**************************************************************************************
void AICEWeaponOptionMissileAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 7);
	return;
}

//**************************************************************************************
void AICDWeaponOptionMissileAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 7);
	return;
}

//**************************************************************************************
void AICFWeaponOptionMissileAll()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 7);
	return;
}

//**************************************************************************************
void AICWWeaponOptionMissileHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 8);
	return;
}

//**************************************************************************************
void AICEWeaponOptionMissileHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 8);
	return;
}

//**************************************************************************************
void AICDWeaponOptionMissileHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 8);
	return;
}

//**************************************************************************************
void AICFWeaponOptionMissileHalf()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 8);
	return;
}

//**************************************************************************************
void AICWWeaponOptionMissileSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 9);
	return;
}

//**************************************************************************************
void AICEWeaponOptionMissileSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 9);
	return;
}

//**************************************************************************************
void AICDWeaponOptionMissileSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 9);
	return;
}

//**************************************************************************************
void AICFWeaponOptionMissileSingle()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 9);
	return;
}

//**************************************************************************************
void AICWWeaponOptionDefault()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 0);
	return;
}

//**************************************************************************************
void AICEWeaponOptionDefault()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 0);
	return;
}

//**************************************************************************************
void AICDWeaponOptionDefault()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 0);
	return;
}

//**************************************************************************************
void AICFWeaponOptionDefault()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);

	AICWeaponOptionSet(firstvalid, lastvalid, 0);
	return;
}

//**************************************************************************************
void AICWeaponOptionSet(int firstvalid, int lastvalid, int weaponoption)
{
	long delaycnt = 1000;
	PlaneParams *planepnt;

	if((firstvalid == 1) && (lastvalid == 7))
	{
		delaycnt += 2000;
	}

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom))
		return;
#endif

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICWeaponOptionSetPlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid, weaponoption);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICWeaponOptionSetPlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid, weaponoption);
	}
	return;
}

//**************************************************************************************
void AICWeaponOptionSetPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int weaponoption)
{
	int placeingroup;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid) && ((!(Planes[planenum].AI.iAIFlags2 & AILANDING)) || (Planes[planenum].AI.lTimer2 > 0)) && (!Planes[planenum].OnGround))
	{
		Planes[planenum].AI.cUseWeapon = weaponoption;
		if((Planes[planenum].AI.iAICombatFlags1 & AI_WINCHESTER_AG) || (AICPlaneHasWeaponOption(planenum) == 0))
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AIC_Basic_Neg, planenum, *delaycnt, 50, 1);
				*delaycnt = *delaycnt + 3000;
			}
		}
		else
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 2000;
			}
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICWeaponOptionSetPlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid, weaponoption);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICWeaponOptionSetPlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid, weaponoption);
	}
	return;
}

//**************************************************************************************
int AICPlaneHasWeaponOption(int planenum)
{
	int weaponoption;
	int cnt;
	DBWeaponType *pweapon_type;
	int bombtype;

	if(Planes[planenum].AI.cUseWeapon)
	{
		weaponoption = (Planes[planenum].AI.cUseWeapon - 1) / 3;
	}
	else
	{
		return(1);
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		if(Planes[planenum].WeapLoad[cnt].Count > 0)
		{
			pweapon_type = &pDBWeaponList[Planes[planenum].WeapLoad[cnt].WeapIndex];
			bombtype = pweapon_type->iWeaponType;
			if((bombtype == WEAPON_TYPE_DUMB_BOMB) && (weaponoption == 0))
			{
				return(1);
			}
			else if((bombtype == WEAPON_TYPE_CLUSTER_BOMB) && (weaponoption == 0))
			{
				return(1);
			}
			else if((bombtype == WEAPON_TYPE_GUIDED_BOMB) && (weaponoption == 1))
			{
				return(1);
			}
			else if(((bombtype == WEAPON_TYPE_AG_MISSILE) || (bombtype == WEAPON_TYPE_ANTI_SHIP_MISSILE)) && (weaponoption == 2))
			{
				return(1);
			}
		}
	}

	Planes[planenum].AI.cUseWeapon = 0;
	return(0);
}

//**************************************************************************************
void AICShowFlightReportCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_CONTACT_REPORTS, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_THREAT_CALLS, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_WINCHESTER, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ENGAGEMENT, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_DAMAGED, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BINGO, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RETURNING_TO_BASE, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICFlightReportCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


	switch(wParam)
	{
		case '1':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowContactReportCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenContactReportCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICContactReportCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '2':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowThreatCallCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenThreatCallCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICThreatCallCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '3':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 1);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Winchester
			break;
		case '4':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowEngagementCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenEngagementCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEngagementCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '5':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 4);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  I've been damaged
			break;
		case '6':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 2);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Bingo
			break;
		case '7':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 3);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Returning To Base
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenFlightReportCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_CONTACT_REPORTS, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_THREAT_CALLS, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_WINCHESTER, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ENGAGEMENT, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_DAMAGED, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_BINGO, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_RETURNING_TO_BASE, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowContactReportCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REPORT_SPIKE, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(PlayerPlane->AADesignate)
	{
		usecolor = textcolor;
	}
	else
	{
		usecolor = invalidcolor;
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REPORT_SORT, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REPORT_CONTACTS_RADAR, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(Av.AGRadar.Target)
	{
		usecolor = textcolor;
	}
	else
	{
		usecolor = invalidcolor;
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REPORT_GROUND_TARGET, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REPORT_STROBE, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICContactReportCommKeyOps(int keyflag, WPARAM wParam)
{
//	int menuedge;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


	switch(wParam)
	{
		case '1':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 27);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Report Spike
			break;
		case '2':
			if((PlayerPlane->AADesignate) && (iAICommFrom < 0))
			{
				AICDoCommSort(PlayerPlane);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Report Sort
			break;
		case '3':
			if(iAICommFrom < 0)
			{
				AIC_WSO_Report_Contacts(PlayerPlane - Planes, 1);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Report Contacts (Radar)
			break;
		case '4':
			if((Av.AGRadar.Target) && (iAICommFrom < 0))
			{
				AICDoCommTargetSpot();
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Report Ground/Sea Targets
			break;
		case '5':
			if(iAICommFrom < 0)
			{
				AICDoCommStrobeSpot();
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Report Strobe
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenContactReportCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_REPORT_SPIKE, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_REPORT_SORT, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_REPORT_CONTACTS_RADAR, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_REPORT_GROUND_TARGET, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_REPORT_STROBE, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowThreatCallCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BANDIT_BANDIT, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SAM_LAUNCH, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MISSILE_LAUNCH, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AAA_SPOTTED, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICThreatCallCommKeyOps(int keyflag, WPARAM wParam)
{
//	int menuedge;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


	switch(wParam)
	{
		case '1':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 23);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Bandit Bandit
			break;
		case '2':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 24);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  SAM Launch
			break;
		case '3':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 25);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Missile Launch
			break;
		case '4':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 26);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  AAA Fired
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenThreatCallCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_BANDIT_BANDIT, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_SAM_LAUNCH, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_MISSILE_LAUNCH, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_AAA_SPOTTED, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	return(messagelen);
}

//**************************************************************************************
void AICShowEngagementCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ENGAGED_OFFENSIVE, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ENGAGED_DEFENSIVE, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TARGET_DESTROYED, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ATTACKING_GROUND, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICEngagementCommKeyOps(int keyflag, WPARAM wParam)
{
//	int menuedge;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


	switch(wParam)
	{
		case '1':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 19);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Engaged Offensive
			break;
		case '2':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 20);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Engaged Defensive
			break;
		case '3':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 21);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Target Destroyed
			break;
		case '4':
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 22);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Attacking Ground Target
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenEngagementCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_ENGAGED_OFFENSIVE, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_ENGAGED_DEFENSIVE, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_TARGET_DESTROYED, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ATTACKING_GROUND, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowAlphaCheckCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_BULLSEYE, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_IP, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_TARGET, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_PACKAGE, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_HOMEPLATE, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_TANKER, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ALPHA_TO_DIVERT_FIELD, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICAlphaCheckCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge = 0;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
	int planenum;

	if(iAICommFrom == -1)
	{
		planenum = PlayerPlane - Planes;
	}
	else
	{
		planenum = iAICommFrom;
	}

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAlphaCheck(planenum, 0);
			//  Alpha Check Bullseye
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAlphaCheck(planenum, 1);
			//  Alpha Check to IP
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAlphaCheck(planenum, 2);
			//  Alpha Check to Target
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAlphaCheck(planenum, 3);
			//  Alpha Check To Package
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAlphaCheck(planenum, 4);
			//  Alpha Check To Homeplate
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
//			AICRequestClosestTanker();
			AICAlphaCheck(planenum, 5);
			//  Alpha Check To Tanker
			break;
		case '7':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAlphaCheck(planenum, 6);
			//  Alpha Check To Divert Field
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenAlphaCheckCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_BULLSEYE, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_IP, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_TARGET, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_PACKAGE, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_HOMEPLATE, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_TANKER, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ALPHA_TO_DIVERT_FIELD, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowJSTARSCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int jstars = 0;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	if(AIGetClosestJSTARS(PlayerPlane) >= 0)
	{
		jstars = 1;
	}

	if((jstars) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;


	if(lBombFlags & WSO_JSTARS_CHECK)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_CHECK_OUT, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_CHECK_IN, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_TARGET_REQUEST, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_NEW_TARGET, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICJSTARSCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge = 0;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((lBombFlags & WSO_JSTARS_CHECK) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
			{
				AICPlayerJSTARSCheckOut();
			}
			else
			{
				AICPlayerJSTARSCheckIn();
			}
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				AIC_JSTARS_Give_Target_Loc();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				AIC_JSTARS_Give_Next_Target_Loc();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenJSTARSCommOps()
{
	int messagelen = 0;
	int worklen;



	if(lBombFlags & WSO_JSTARS_CHECK)
	{
		messagelen = AICGetMaxMenuPixelLen(AIC_JSTARS_CHECK_OUT, 1, MessageFont);
	}
	else
	{
		messagelen = AICGetMaxMenuPixelLen(AIC_JSTARS_CHECK_IN, 1, MessageFont);
	}

	worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_TARGET_REQUEST, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_NEW_TARGET, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowTankerCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int tanker = 0;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	if(AICheckTankerCloseBy(PlayerPlane) != NULL)
	{
		tanker = 1;
	}

	if((tanker) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MAX_FUEL, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((tanker) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MAX_TRAP_FUEL, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((tanker) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_4K_FUEL, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICTankerCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge = 0;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((iAICommFrom == -1) || (!MultiPlayer))
			{
				if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
				{
					AICPlayerContactRefueler();
				}
			}
			else
			{
				if((AICheckTankerCloseBy(&Planes[iAICommFrom]) != NULL) && (!(Planes[iAICommFrom].AI.iAIFlags1 & AI_HAS_EJECTED)))
				{
					AICPlayerContactRefueler();
				}
			}
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((iAICommFrom == -1) || (!MultiPlayer))
			{
				if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
				{
					AICPlayerContactRefuelerMaxTrap();
				}
			}
			else
			{
				if((AICheckTankerCloseBy(&Planes[iAICommFrom]) != NULL) && (!(Planes[iAICommFrom].AI.iAIFlags1 & AI_HAS_EJECTED)))
				{
					AICPlayerContactRefuelerMaxTrap();
				}
			}
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((iAICommFrom == -1) || (!MultiPlayer))
			{
				if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
				{
					AICPlayerContactRefueler4K();
				}
			}
			else
			{
				if((AICheckTankerCloseBy(&Planes[iAICommFrom]) != NULL) && (!(Planes[iAICommFrom].AI.iAIFlags1 & AI_HAS_EJECTED)))
				{
					AICPlayerContactRefueler4K();
				}
			}
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenTankerCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_MAX_FUEL, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_MAX_TRAP_FUEL, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_4K_FUEL, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowTowerCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	RunwayInfo *tower;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;


	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	tower = (RunwayInfo *)AIGetClosestAirField(PlayerPlane, PlayerPlane->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, (20.0f * NMTOWU), 0);

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	if(!PlayerPlane->OnGround)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TOWER_LANDING_REQUEST, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

//	if(tower)
	if(PlayerPlane->OnGround)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TOWER_REQUEST_TAKEOFF, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(tower)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TOWER_REQUEST_TAXI, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(!PlayerPlane->OnGround)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TOWER_INBOUND, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((!PlayerPlane->OnGround) && ((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && (((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14)))))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SUPER_HORNET_BALL, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((!PlayerPlane->OnGround) && ((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && (((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14)))))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TOWER_CLARA, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(!PlayerPlane->OnGround)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REQUEST_EMERGENCY, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICTowerCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge = 0;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;

			if((iAICommFrom < 0) && (PlayerPlane->OnGround))
				return;

			if(!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
				AICPlayerRequestLanding();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED)) && (planepnt->OnGround))
				AICPlayerRequestTakeOff();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;

			if((iAICommFrom < 0) && (PlayerPlane->OnGround))
				return;

			//  Request Taxi
			break;
		case '4':
			if((iAICommFrom < 0) && (PlayerPlane->OnGround))
				return;

			if(iAICommFrom >= 0)
			{
				AICTowerInbound(iAICommFrom);
			}
			else
			{
				AICTowerInbound(PlayerPlane - Planes);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Inbound
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
//			if((iAICommFrom < 0) && (PlayerPlane->OnGround))
			if((iAICommFrom < 0) && ((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14))))))
				return;

			if(iAICommFrom < 0)
			{
				AICPlayerSeesBall(PlayerPlane - Planes);
			}
			else
			{
				AICPlayerSeesBall(iAICommFrom);
			}
			//  Super Hornet Ball
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((iAICommFrom < 0) && ((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14))))))
				return;

			if(iAICommFrom < 0)
			{
				AICPlayerClara(PlayerPlane - Planes);
			}
			else
			{
				AICPlayerClara(iAICommFrom);
			}
			//  Clara
			break;
		case '7':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;

//			if((iAICommFrom < 0) && ((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14))))))
//				return;

			if(iAICommFrom < 0)
			{
				AICRequestEmergencyApproach(PlayerPlane - Planes);
			}
			else
			{
				AICRequestEmergencyApproach(iAICommFrom);
			}
			//  Request Emergency Approach
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenTowerCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_TOWER_LANDING_REQUEST, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_TOWER_REQUEST_TAKEOFF, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_TOWER_REQUEST_TAXI, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_TOWER_INBOUND, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_SUPER_HORNET_BALL, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_TOWER_CLARA, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_REQUEST_EMERGENCY, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowFACCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int facclose;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	usecolor = textcolor;
	if(lBombFlags & WSO_FAC_CHECK)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FAC_CHECK_OUT, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	else
	{
		facclose = AICCheckFACClose(PlayerPlane);
		if(!facclose)
		{
			usecolor = invalidcolor;
		}
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FAC_CHECK_IN, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}

	if(lBombFlags & WSO_FAC_CHECK)
	{
		usecolor = textcolor;
	}
	else
	{
		usecolor = invalidcolor;
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FAC_TARGET, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(lBombFlags & WSO_FAC_CHECK)
	{
		usecolor = textcolor;
	}
	else
	{
		usecolor = invalidcolor;
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FAC_NEW_TARGET, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(lBombFlags & WSO_FAC_CHECK)
	{
		usecolor = textcolor;
	}
	else
	{
		usecolor = invalidcolor;
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FAC_BLIND, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICFACCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge = 0;
	int facclose;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
	int planenum;
	PlaneParams *planepnt;

	if(iAICommFrom == -1)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}
	planenum = planepnt - Planes;


	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;

			//  Temp for now, will need function with audio added.
			if(lBombFlags & WSO_FAC_CHECK)
			{
				lBombFlags &= ~(WSO_FAC_CHECK);
				AICAddSoundCall(AIC_FACAck, planenum, 2000, 50, 15);
			}
			else
			{
				facclose = AICCheckFACClose(PlayerPlane);
				if(facclose)
				{
					lBombFlags |= (WSO_FAC_CHECK);
					AICCheckInWithFAC(planenum);
				}
			}
			break;
		case '2':
			AIC_FAC_RequestTarget(planenum);

			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  FAC Target Request
			break;
		case '3':
			AIC_FAC_RequestNewTarget(planenum);
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  FAC New Target Request
			break;
		case '4':
			AIC_FAC_Blind(planenum);
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  FAC Blind
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenFACCommOps()
{
	int messagelen = 0;
	int worklen;

	if(lBombFlags & WSO_FAC_CHECK)
	{
		messagelen = AICGetMaxMenuPixelLen(AIC_FAC_CHECK_OUT, 1, MessageFont);
	}
	else
	{
		messagelen = AICGetMaxMenuPixelLen(AIC_FAC_CHECK_IN, 1, MessageFont);
	}

	worklen = AICGetMaxMenuPixelLen(AIC_FAC_TARGET, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_FAC_NEW_TARGET, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_FAC_BLIND, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICShowEscortCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int escorts = 0;
	int temp1, numsead;
	int pescort;
	PlaneParams *eplanepnt;
	int numcas;
	int seadstay;

	escorts = AIGetNumEscorting(PlayerPlane, PlayerPlane, &temp1, &numsead, &numcas, &seadstay);

//	escorts += numsead;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	if(((escorts) || (numsead)) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RELEASE_ESCORTS, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;


	pescort = AIGetHumanEscortPlane(PlayerPlane - Planes);

	if(pescort != -1)
	{
		eplanepnt = &Planes[pescort];
	}
	else
	{
		eplanepnt = NULL;
	}

	if(!eplanepnt)
	{
		usecolor = invalidcolor;
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_CHECK_IN, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	else
	{
		usecolor = textcolor;
		if((eplanepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (!(eplanepnt->AI.iAIFlags1 & AI_RELEASED_PLAYER)))
		{
			theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_CHECK_OUT, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
		}
		else
		{
			theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_CHECK_IN, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
		}
	}

	if(((escorts) || (numsead)) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ESCORT_ENGAGE_BANDITS, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((numcas + seadstay) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ESCORT_ENGAGE_DEF, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((numsead) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ESCORT_ENGAGE_GROUND, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if((AIRejoinableEscorts(PlayerPlane, AIGetLeader(PlayerPlane))) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ESCORT_REJOIN, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICEscortCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge = 0;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
	int pescort;
	PlaneParams *eplanepnt;
	PlaneParams *planepnt;


	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = AICShowEscortReleaseCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortReleaseCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEscortReleaseCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '2':

			if(iAICommFrom < 0)
			{
				planepnt = PlayerPlane;
			}
			else
			{
				planepnt = &Planes[iAICommFrom];
			}

			pescort = AIGetHumanEscortPlane(planepnt - Planes);

			if(pescort != -1)
			{
				eplanepnt = &Planes[pescort];
			}
			else
			{
				eplanepnt = NULL;
			}

			if((!(eplanepnt->AI.iAIFlags1 & AI_SEEN_PLAYER)) || (eplanepnt->AI.iAIFlags1 & AI_RELEASED_PLAYER))
			{
				AIUpdateGroupFlags(eplanepnt - Planes, AI_RELEASED_PLAYER, 0, 1);
				AIC_Human_Beginning_Escort_Msg(planepnt - Planes, eplanepnt - Planes);
				AICAddSoundCall(AIC_Spot_Human_Escort_Msg, eplanepnt - Planes, 5000, 50, planepnt - Planes);
			}
			else if(!(eplanepnt->AI.iAIFlags1 & AI_RELEASED_PLAYER))
			{
				AIUpdateGroupFlags(eplanepnt - Planes, AI_RELEASED_PLAYER, 0);
				AICCAPDone(planepnt - Planes, eplanepnt - Planes);
			}

			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			//  Check in/out
			break;
		case '3':
			gAICommMenu.AICommMenufunc = AICShowEscortEngageCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortEngageCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEscortEngageCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			//  Engage Bandits
			break;
		case '4':
			gAICommMenu.AICommMenufunc = AICShowEscortSEADCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortSEADCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEscortSEADCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			//  Engage Air Defenses
			break;
		case '5':
			gAICommMenu.AICommMenufunc = AICShowEscortCASCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortCASCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEscortCASCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			//  Engage Ground Targets
			break;
		case '6':
			gAICommMenu.AICommMenufunc = AICShowEscortRejoinCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortRejoinCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEscortRejoinCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			//  Engage Ground Targets
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICPixelLenEscortCommOps()
{
	int messagelen = 0;
	int worklen;
	int pescort;
	PlaneParams *eplanepnt;

	messagelen = AICGetMaxMenuPixelLen(AIC_RELEASE_ESCORTS, 1, MessageFont);
	pescort = AIGetHumanEscortPlane(PlayerPlane - Planes);

	if(pescort != -1)
	{
		eplanepnt = &Planes[pescort];
	}
	else
	{
		eplanepnt = NULL;
	}

	if(!eplanepnt)
	{
		worklen = AICGetMaxMenuPixelLen(AIC_CHECK_IN, 2, MessageFont);
	}
	else
	{
		if((eplanepnt->AI.iAIFlags1 & AI_SEEN_PLAYER) && (!(eplanepnt->AI.iAIFlags1 & AI_RELEASED_PLAYER)))
		{
			worklen = AICGetMaxMenuPixelLen(AIC_CHECK_OUT, 2, MessageFont);
		}
		else
		{
			worklen = AICGetMaxMenuPixelLen(AIC_CHECK_IN, 2, MessageFont);
		}
	}

	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ESCORT_ENGAGE_BANDITS, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ESCORT_ENGAGE_DEF, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ESCORT_ENGAGE_GROUND, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ESCORT_REJOIN, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	return(messagelen);
}

//**************************************************************************************
int AIGetHumanEscortPlane(int planenum)
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
				return(pEscortAction->iPlaneNum);
			}
		}
		lookway ++;
	}
	return(-1);
}

//**************************************************************************************
int AIRIsFighter(int planenum)
{
	long voice;

	voice = Planes[planenum].AI.lAIVoice;

	switch(voice)
	{
		case SPCH_BETTY:
		case SPCH_LSO:
		case SPCH_AWACS1:
		case SPCH_AWACS2:
		case SPCH_JSTARS1:
		case SPCH_SAR1:
		case SPCH_ATK_USAF:
		case SPCH_ATK_RAF:
		case SPCH_ATK_RUS:
		case SPCH_TWR_NATO:
		case SPCH_TWR_RUS:
		case SPCH_STRIKE:
		case SPCH_MARSHAL:
		case SPCH_FINAL:
		case SPCH_FAC_AIR:
		case SPCH_FAC_GRND:
		case SPCH_TANK_USN:
		case SPCH_TANK_USMC:
			return(0);
			break;
		case SPCH_WNG1:
		case SPCH_WNG2:
		case SPCH_WNG3:
		case SPCH_WNG4:
		case SPCH_WNG5:
		case SPCH_WNG6:
		case SPCH_WNG7:
		case SPCH_WNG8:
		case SPCH_NAVY1:
		case SPCH_NAVY2:
		case SPCH_NAVY3:
		case SPCH_NAVY4:
		case SPCH_NAVY5:
		case SPCH_FTR_USAF:
		case SPCH_FTR_NATO:
		case SPCH_FTR_RAF:
		case SPCH_FTR_RUS:
			return(1);
			break;
		default:
			return(0);
			break;
	}
	return(0);
}

//**************************************************************************************
int AIRIsAttack(int planenum)
{
	long voice;

	voice = Planes[planenum].AI.lAIVoice;

	switch(voice)
	{
		case SPCH_BETTY:
		case SPCH_LSO:
		case SPCH_AWACS1:
		case SPCH_AWACS2:
		case SPCH_JSTARS1:
		case SPCH_SAR1:
		case SPCH_TWR_NATO:
		case SPCH_TWR_RUS:
		case SPCH_STRIKE:
		case SPCH_MARSHAL:
		case SPCH_FINAL:
		case SPCH_FAC_AIR:
		case SPCH_FAC_GRND:
		case SPCH_TANK_USN:
		case SPCH_TANK_USMC:
		case SPCH_FTR_USAF:
		case SPCH_FTR_NATO:
		case SPCH_FTR_RAF:
		case SPCH_FTR_RUS:
			return(0);
			break;
		case SPCH_WNG1:
		case SPCH_WNG2:
		case SPCH_WNG3:
		case SPCH_WNG4:
		case SPCH_WNG5:
		case SPCH_WNG6:
		case SPCH_WNG7:
		case SPCH_WNG8:
		case SPCH_NAVY1:
		case SPCH_NAVY2:
		case SPCH_NAVY3:
		case SPCH_NAVY4:
		case SPCH_NAVY5:
		case SPCH_ATK_USAF:
		case SPCH_ATK_RAF:
		case SPCH_ATK_RUS:
			return(1);
			break;
		default:
			return(0);
			break;
	}
	return(0);
}

//**************************************************************************************
int AICGetEmitterType(int radarid, int isgun, int isinfo, char *emitterstr)
{
	if(isgun)
	{
		if(emitterstr)
		{
			if(!LANGGetTransMessage(emitterstr, 128, AIC_TRIPLE_A, g_iLanguageId))
			{
				return(460);
			}
		}
		return(460);
	}
	if(isinfo)
	{
		if(emitterstr)
		{
			if(!LANGGetTransMessage(emitterstr, 128, AIC_EW, g_iLanguageId))
			{
				return(461);
			}
		}
		return(461);
	}

	switch(radarid)
	{
		case 7:
			if(emitterstr)
			{
				if(!LANGGetTransMessage(emitterstr, 128, AIC_GECKO, g_iLanguageId))
				{
					return(456);
				}
			}
			return(456);
			break;
		case 8:
		case 47:
		case 49:
			if(emitterstr)
			{
				if(!LANGGetTransMessage(emitterstr, 128, AIC_GRUMBLE, g_iLanguageId))
				{
					return(457);
				}
			}
			return(457);
			break;
		case 9:
			if(emitterstr)
			{
				if(!LANGGetTransMessage(emitterstr, 128, AIC_GADFLY, g_iLanguageId))
				{
					return(458);
				}
			}
			return(458);
			break;
		case 11:
			if(emitterstr)
			{
				if(!LANGGetTransMessage(emitterstr, 128, AIC_GAUNTLET, g_iLanguageId))
				{
					return(459);
				}
			}
			return(459);
			break;
		case 50:
		case 51:
			if(emitterstr)
			{
				if(!LANGGetTransMessage(emitterstr, 128, AIC_EW, g_iLanguageId))
				{
					return(461);
				}
			}
			return(461);
			break;
	}

	if(emitterstr)
	{
		*emitterstr = 0;
	}
	return(-1);
}

//**************************************************************************************
int AICGetBanditID(int banditidnum, char *idstr, int ischick)
{

	if(ischick)
	{
		if(idstr)
		{
			if(!LANGGetTransMessage(idstr, 128, AIC_CHICK, g_iLanguageId))
			{
				return(455);
			}
		}
		return(455);
	}

	switch(banditidnum)
	{
		case 50:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_CANDID, g_iLanguageId))
				{
					return(436);
				}
			}
			return(436);
			break;
		case 21:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_MAINSTAY, g_iLanguageId))
				{
					return(437);
				}
			}
			return(437);
			break;
//		case :
//			if(idstr)
//			{
//				if(!LANGGetTransMessage(idstr, 128, AIC_HELIX, g_iLanguageId))
//				{
//					return(438);
//				}
//			}
//			return(438);
//			break;
//		case :
//			if(idstr)
//			{
//				if(!LANGGetTransMessage(idstr, 128, AIC_HOKUM, g_iLanguageId))
//				{
//					return(439);
//				}
//			}
//			return(439);
//			break;
		case 6:
		case 40:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FISHBED, g_iLanguageId))
				{
					return(440);
				}
			}
			return(440);
			break;
		case 7:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FLOGGER, g_iLanguageId))
				{
					return(441);
				}
			}
			return(441);
			break;
		case 8:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FOXBAT, g_iLanguageId))
				{
					return(442);
				}
			}
			return(442);
			break;
		case 9:
		case 41:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FULCRUM, g_iLanguageId))
				{
					return(443);
				}
			}
			return(443);
			break;
		case 66:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FOXHOUND, g_iLanguageId))
				{
					return(444);
				}
			}
			return(444);
			break;
		case 28:
		case 56:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_HIP, g_iLanguageId))
				{
					return(445);
				}
			}
			return(445);
			break;
		case 54:
		case 55:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_HIND, g_iLanguageId))
				{
					return(446);
				}
			}
			return(446);
			break;
//		case :
//			if(idstr)
//			{
//				if(!LANGGetTransMessage(idstr, 128, AIC_HAVOC, g_iLanguageId))
//				{
//					return(447);
//				}
//			}
//			return(447);
//			break;
		case 42:
		case 43:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FITTER, g_iLanguageId))
				{
					return(448);
				}
			}
			return(448);
			break;
		case 45:
		case 81:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FENCER, g_iLanguageId))
				{
					return(449);
				}
			}
			return(449);
			break;
		case 23:
		case 44:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FROGFOOT, g_iLanguageId))
				{
					return(450);
				}
			}
			return(450);
			break;
		case 10:
		case 52:
		case 67:
		case 80:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_FLANKER, g_iLanguageId))
				{
					return(451);
				}
			}
			return(451);
			break;
		case 79:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_BEAR, g_iLanguageId))
				{
					return(452);
				}
			}
			return(452);
			break;
		case 24:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_BLINDER, g_iLanguageId))
				{
					return(453);
				}
			}
			return(453);
			break;
		case 68:
			if(idstr)
			{
				if(!LANGGetTransMessage(idstr, 128, AIC_BACKFIRE, g_iLanguageId))
				{
					return(454);
				}
			}
			return(454);
			break;
	}

	if(idstr)
	{
		*idstr = 0;
	}
	return(-1);
}

//**************************************************************************************
void AIC_TellNumInPicture(int planenum, int numgroups, int hasgroups)
{
	char tempstr[1024];
	char callsign[256];
	char tempstr2[128];
	int msgnum;

	if(planenum < 0)
	{
		sprintf(tempstr2, "%d", numgroups);

		if(hasgroups)
		{
			msgnum = AIC_STRIKE_GROUPS;
		}
		else
		{
			msgnum = AIC_STRIKE_SINGLES;
		}
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
		{
			return;
		}
	}
	else
	{
		AIC_Get_Callsign_With_Number(planenum, callsign);

		sprintf(tempstr2, "%d", numgroups);

		if(hasgroups)
		{
			msgnum = AIC_GROUPS;
		}
		else
		{
			msgnum = AIC_SINGLES;
		}
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, tempstr2))
		{
			return;
		}
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIC_TellNumInPictureSnd(planenum, numgroups, hasgroups);
}

//**************************************************************************************
void AIC_TellNumInPictureSnd(int planenum, int numgroups, int hasgroups)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;
	int voice;
	int placeingroup = 0;

	numids = 0;

	if(planenum < 0)
	{
		voice = SPCH_STRIKE;
	}
	else
	{
		voice = channel =  Planes[planenum].AI.lAIVoice;  //  AIRIsFighter(planenum);
		placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	}

	if(numgroups && hasgroups)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_GROUPS, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_SINGLES, g_iLanguageId))
		{
			return;
		}
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			if(planenum < 0)
			{
				numids += AIRProcessSpeechVars(&sndids[numids], numval, 1, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
			}
			else
			{
				numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
			}
		}
		else if(spchvar == 2)
		{
			if(numval == 1)
			{
				if(planenum < 0)
				{
					sndids[numids] = SPCH_STRIKE_CS + voice;
					numids ++;
				}
				else
				{
					sndids[numids] = AIRGetCallSignID(planenum) + voice;
					numids ++;

					sndids[numids] = SPCH_ZERO + Planes[planenum].AI.cCallSignNum + voice;
					numids ++;

					sndids[numids] = AIRWingNumSndID(planenum, placeingroup) + voice;
					numids ++;
				}
			}
			else if(numval == 2)
			{
				sndids[numids] = AIRGetNumSpeech(numgroups) + voice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + voice;
			numids ++;
		}
	}

	if(planenum < 0)
	{
		AIRSendSentence(planenum, numids, sndids, voice);
	}
	else
	{
		AIRSendSentence(planenum, numids, sndids, 0);
	}
}

//**************************************************************************************
void AICBroadcastBanditCall(int planenumplus, int foundplane)
{
	int placeingroup;
	char callsign[128];
	char tempstr[1024];
	char tempstr2[1024];
	char placestr[128];
	char groupstr[128];
	char altstr[128];
	char taltstr[128];
	char aspectstr[128];
	char sidestr[128];
	char eheadingstr[128];
	char bearstr[128];
	char rangestr[128];
	int numingroup;
	int bearing360;
	float rangenm;
	float tbearing;
	int headingval;
	int altval;
	int msgnum, msgsnd;
	int headingval2;
	char bcardinal[128];
	int aspectval;
	float trange, dx, dz;
	int sourcenum = (planenumplus & 0x8000) ? -1 : (planenumplus & 0x3FFF);
	int planenum = sourcenum;
	PlaneParams *planepnt = (sourcenum == -1) ? NULL : &Planes[sourcenum];
	PlaneParams *targplane = &Planes[foundplane & 0x1FFF];
	int voice = 0;
	float bearing;
	float range;
	int numval = planenumplus >> 17;
	int awacsnum;
	int digitheading2;
	int popup = (foundplane & 0x4000) ? -1 : 0;
	int talknum = sourcenum;
	int msgnumb, msgsndb;

	if(!planepnt)
		return;

	if(foundplane & 0x8000)
	{
		awacsnum = -2;
		voice = SPCH_STRIKE;
		talknum = 1;
	}
	else if(foundplane & 0x2000)
	{
		awacsnum = -1;
	}
	else
	{
		talknum = awacsnum = (foundplane & ~(0x40000000)) >> 17;
	}

	if(foundplane & 0x40000000)
	{
		popup = 2;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(placestr, 128, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
//	numingroup = AICGetIfPlaneGroupText(groupstr, targplane, 1);

	numingroup = AICGetIfPlaneGroupRadarText(groupstr, targplane, &msgnum, &msgsnd, -1, popup);

	AIRGenericSpeech(msgsnd, talknum, targplane - Planes, 0, 0, 0, 0, 0, 0, numval, voice);

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		digitheading2 = tbearing = AIComputeHeadingFromBullseye(targplane->WorldPosition, &trange, &dx, &dz);
		headingval2 = AICGetTextHeadingGeneral(bcardinal, tbearing);
	}

	dx = targplane->WorldPosition.X - planepnt->WorldPosition.X;
	dz = targplane->WorldPosition.Z - planepnt->WorldPosition.Z;

	bearing = atan2(-dx, -dz) * 57.2958;

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	aspectval = AICGetAspectText(aspectstr, bearing, targplane);

	altval = (targplane->WorldPosition.Y * WUTOFT) / 1000.0f;

	sprintf(altstr, "%d", altval);

	if(!LANGGetTransMessage(taltstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
	{
		return;
	}

	if((targplane->WorldPosition.Y * WUTOFT) > 40000)
	{
		altval = 0;
		if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->WorldPosition.Y * WUTOFT) > 30000)
	{
		altval = 1;
		if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->HeightAboveGround * WUTOFT) < 10000)
	{
		altval = 3;
		if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		altval = 2;
		if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
		{
			return;
		}
	}

	headingval = AICGetTextHeadingGeneral(eheadingstr, AIConvertAngleTo180Degree(targplane->Heading));

	if(targplane->AI.iSide == AI_ENEMY)
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_BANDIT_BANDIT, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_UNKNOWN, g_iLanguageId))
		{
			return;
		}
	}

	range = QuickDistance(dx, dz);
	rangenm = (range * WUTONM);
	if((planepnt) && (planepnt->AI.lAIVoice < 14000))
	{
		if(rangenm > 100)
			rangenm = 100;
	}
	AICGetRangeText(rangestr, rangenm);

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)) && (rangenm > 15))
	{
		rangenm = (trange * WUTONM);
		if((planepnt) && (planepnt->AI.lAIVoice < 14000))
		{
			if(rangenm > 100)
				rangenm = 100;
		}
		AICGetRangeText(rangestr, rangenm);

		if(iUseBullseye == 2)
		{
			msgnumb = AIC_DO_DIGIT_BULLSEYE;
			msgsndb = AICF_DO_DIGIT_BULLSEYE;

			tbearing = -digitheading2;
			headingval2 = digitheading2;
			if(tbearing < 0)
			{
				bearing360 = tbearing + 360;
			}
			else
			{
				bearing360 = tbearing;
			}
			sprintf(bearstr, "%03d", bearing360);

			if(!LANGGetTransMessage(tempstr, 1024, msgnumb, g_iLanguageId, rangestr, bearstr, taltstr, eheadingstr))
			{
				return;
			}
			AIRGenericSpeech(msgsndb, talknum, targplane - Planes, bearing, headingval, headingval2, rangenm, targplane->WorldPosition.Y * WUTOFT, 0, numingroup, voice);
		}
		else
		{
			msgnumb = AIC_DO_BULLSEYE;
			msgsndb = AICF_DO_BULLSEYE;

			if(!LANGGetTransMessage(tempstr, 1024, msgnumb, g_iLanguageId, rangestr, bcardinal, altstr, eheadingstr))
			{
				return;
			}
			AIRGenericSpeech(msgsndb, talknum, targplane - Planes, bearing, headingval, headingval2, rangenm, altval, 0, numingroup, voice);
		}

	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_DO_BRA, g_iLanguageId, bearstr, rangestr, taltstr, aspectstr))
		{
			return;
		}

		AIRGenericSpeech(AICF_DO_BRA, talknum, targplane - Planes, bearing, headingval, headingval2, rangenm, targplane->WorldPosition.Y * WUTOFT, 0, numingroup, voice);
	}

	if((msgsnd == AICF_GROUP_INFO) || (msgsnd == AICF_SINGLE_INFO))
	{
		if(numval < 0)
		{
			msgnumb = SPCH_ZERO;
		}
		else if(numval > 20)
		{
			msgnumb = AIC_ADDITIONAL;
		}
		else
		{
			msgnumb = AIC_FIRST + numval - 1;
		}

		if(!LANGGetTransMessage(callsign, 1024, msgnumb, g_iLanguageId))
		{
			return;
		}
	}

	if(!LANGGetTransMessage(tempstr2, 1024, msgnum, g_iLanguageId, callsign, tempstr))
	{
		return;
	}

	if(popup == 2)
	{
		strcpy(tempstr, tempstr2);

		if(!LANGGetTransMessage(tempstr2, 1024, AIC_DECLARE2, g_iLanguageId, tempstr))
		{
			return;
		}

		AIRGenericSpeech(AICF_DECLARE2, talknum, targplane - Planes, 0, 0, 0, 0, 0, 0, numval, voice);
	}

//	AIRRadarBanditCallSnd(planepnt, targplane, bearing360, rangenm, headingval, numingroup);
	AICAddAIRadioMsgs(tempstr2, 40);
}

//**************************************************************************************
void AIC_AWACSGoTactical(int var1, int var2)
{
	char callsign[256];
	char tempstr[256];
	PlaneParams *planepnt;
	int planenum;
	int awacsnum;
	int voice = 0;

	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(!((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		AIC_Basic_Neg(awacsnum, 1);
	}

	if(awacsnum < 0)
	{
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}

	lBombFlags |= (WSO_AWACS_TACTICAL);
	iUseBullseye = 0;

	planenum = planepnt - Planes;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_TACTICAL, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_WING_GO_TACTICAL, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AIC_AWACSGoBullseye(int var1, int var2)
{
	char callsign[256];
	char tempstr[256];
	PlaneParams *planepnt;
	int planenum;
	int awacsnum;
	int voice = 0;


	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(!((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		AIC_Basic_Neg(awacsnum, 1);
	}

	if(awacsnum < 0)
	{
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}

	planenum = planepnt - Planes;

	lBombFlags &= ~(WSO_AWACS_TACTICAL);
	iUseBullseye = 1;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_BROADCAST, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_WING_GO_BROADCAST, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AIC_AWACSGoDigitalBullseye(int var1, int var2)
{
	char callsign[256];
	char tempstr[256];
	PlaneParams *planepnt;
	int planenum;
	int awacsnum;
	int voice = 0;

	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(!((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		AIC_Basic_Neg(awacsnum, 1);
	}

	if(awacsnum < 0)
	{
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}

	planenum = planepnt - Planes;

	lBombFlags &= ~(WSO_AWACS_TACTICAL);
	iUseBullseye = 2;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_BROADCAST, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_WING_GO_BROADCAST, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AIC_WSO_Report_Contacts(int orgplanenum, int targetnum)
{
	PlaneParams *planepnt = &Planes[orgplanenum];
	PlaneParams *checkplane;
	float foundrange = -1;
	PlaneParams *foundplane = NULL;
	int foundplanenum;
	float dx, dy, dz, tdist;
	float nearrange = 10.0f * NMTOWU;
	int cnt, currid;
	int done = 0;
	int firstpass = 1;
	int currentcnt = 0;
	long nextdelay = 7000;
	int numhide;
	int hasgroups = 0;
	int planenum;
	BYTE nettime;

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		checkplane->AI.iAIFlags2 &= ~(AI_REPORTED_AS_CONTACT);
		checkplane->AI.iAICombatFlags2 &= ~(AI_TEMP_IGNORE_CHECK);
	}

	while(!done)
	{
		planenum = orgplanenum;
		foundplane = NULL;

		if( GetCurrentAARadarMode() == AA_STT_MODE )
		{
			if((CurTargetInfo.CurTargetId >= 0) && firstpass)
			{
				foundplane = &Planes[CurTargetInfo.CurTargetId];
			}
		}
		else
		{
			for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
			{
				currid = CurFramePlanes.Planes[cnt].PlaneId;
				checkplane = &Planes[currid];
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
				{
					dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if((tdist < foundrange) || (foundrange < 0))
					{
						foundrange = tdist;
						foundplane = checkplane;
					}
				}
			}
			if( GetCurrentAARadarMode() == AA_TWS_MODE )
			{
				for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
				{
					currid = AgePlanes[0].Planes[cnt].PlaneId;
					checkplane = &Planes[currid];
					if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
					{
						dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
						dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
						dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
						tdist = QuickDistance(dx, dz);
						if((tdist < foundrange) || (foundrange < 0))
						{
							foundrange = tdist;
							foundplane = checkplane;
						}
					}
				}
			}
		}

		if(foundplane == NULL)
		{
			if(firstpass)
			{
				if(MultiPlayer && (targetnum == 1))
				{
					NetPutGenericMessage1(planepnt, GM_CLEAN);
				}
//				else
//				{
					AICCleanCall(planenum);
//				}
			}
			else
			{
				if(MultiPlayer && (targetnum == 1))
				{
					NetPutGenericMessage3(planepnt, GM3_NUM_IN_PIC, currentcnt, hasgroups);
				}
//				else
//				{
					AIC_TellNumInPicture(planenum, currentcnt, hasgroups);
//				}
			}
//			AICWingReportContacts();
			done = 1;
			break;
		}

		numhide = 0;
		if( GetCurrentAARadarMode() == AA_STT_MODE )
		{
	 		foundplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
			foundplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
			for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
			{
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
				{
					dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if(tdist < nearrange)
					{
						checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
						checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
						numhide ++;
					}
				}
			}
//			AICAddSoundCall(AIC_Wing_Report_Contacts_Delay, (int)(planepnt - Planes), 4000, 50);
		}
		else
		{
	 		foundplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
			for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
			{
				currid = CurFramePlanes.Planes[cnt].PlaneId;
				checkplane = &Planes[currid];
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
				{
					dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if(tdist < nearrange)
					{
						checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
						checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
						numhide ++;
					}
				}
			}
			if( GetCurrentAARadarMode() == AA_TWS_MODE )
			{
				for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
				{
					currid = AgePlanes[0].Planes[cnt].PlaneId;
					checkplane = &Planes[currid];
					if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
					{
						dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
						dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
						dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
						tdist = QuickDistance(dx, dz);
						if(tdist < nearrange)
						{
							checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
							checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
							numhide ++;
						}
					}
				}
			}
//			AICAddSoundCall(AIC_WSO_Report_Contacts_Delay, (int)(planepnt - Planes), 4000, 50);
		}

		if(numhide)
		{
			planenum |= 0x4000;
			hasgroups = 1;
		}
		currentcnt ++;
		planenum |= currentcnt << 17;

		foundplanenum = foundplane - Planes;

		foundplanenum |= 0x2000;

		if(MultiPlayer && (targetnum == 1))
		{
			nettime = nextdelay / 1000.0f;
			NetPutGenericMessage3Long(NULL, GM3L_BANDIT_CALL, planenum, foundplanenum, nettime);
		}
//		else
//		{
		  	AICAddSoundCall(AICBroadcastBanditCall, planenum, nextdelay, 50, foundplanenum);
//		}
		nextdelay = nextdelay + 5000 + ((rand() & 3) * 1000);

		firstpass = 0;
	}

	if(!(MultiPlayer && (targetnum == 1)))
	{
		if(planepnt->AI.wingman >= 0)
		{
			AICAddSoundCall(AICContinueWingPicture, planepnt->AI.wingman, 5000, 50, nextdelay);
		}
		else if(planepnt->AI.nextpair >= 0)
		{
			AICAddSoundCall(AICContinueWingPicture, planepnt->AI.nextpair, 5000, 50, nextdelay);
		}
	}
}

//**************************************************************************************
void AICContinueWingPicture(int planenum, int nextdelay)
{
	PlaneParams *planepnt = &Planes[planenum];
	PlaneParams *checkplane;

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		checkplane->AI.iAICombatFlags2 &= ~(AI_TEMP_IGNORE_CHECK);
	}

	AICContinuePictureNearPlane(&nextdelay, -1, planepnt, -1, 0, 0, 1);
}

//**************************************************************************************
void AICFlightFuelCheck(int planenum, int targetnum)
{

	long delaycnt = 3000;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

//	AIC_WSO_Check_Msgs(planepnt - Planes, 1);

	if(planepnt->AI.wingman >= 0)
	{
		AICFlightFuelCheckPlane(planepnt->AI.wingman, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICFlightFuelCheckPlane(planepnt->AI.nextpair, &delaycnt);
	}
	return;
}

//**************************************************************************************
void AICFlightFuelCheckPlane(int planenum, long *delaycnt)
{
	int planeok = 1;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);
	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(AICAllowThisRadio(planenum, 1))
		{
			AICAddSoundCall(AICFuelCheck, planenum, *delaycnt, 50);
			*delaycnt = *delaycnt + 4000;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICFlightFuelCheckPlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICFlightFuelCheckPlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICFuelCheck(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char tempstr3[256];
	char fuelstate[256];
	float fworkvar;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

//	AIC_Get_Callsign_With_Number(planenum, callsign);
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	fworkvar = Planes[planenum].TotalFuel / 1000.0f;
	sprintf(tempstr3, "%2.1f", fworkvar);
	if(!LANGGetTransMessage(fuelstate, 256, AIC_FUEL_STATE, g_iLanguageId, tempstr3))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 256, AIC_WING_FUEL_STATE, g_iLanguageId, tempstr2, fuelstate))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WING_FUEL_STATE, planenum);
	return;
}

//**************************************************************************************
void AICAlphaCheck(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum;
	int awacsnum;
	int voice = 0;
	float bearing, range, altitude;
	int bearing360;
	float tbearing;
	char callsign[128];
	char bearstr[128];
	char rangestr[128];
	char altstr[128];
	char taltstr[128];
	float rangenm;
	int altval;
	int targetsnd = targetnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum < 0)
	{
		voice = SPCH_STRIKE;
	}

	taltstr[0] = 0;

	switch(targetnum)
	{
		case 0:
			msgnum = AIC_BULLSEYE_BEARING;
			bearing = AICGetBullseyeBearing(planenum, &range);
			break;
		case 1:
			msgnum = AIC_IP_BEARING;
			bearing = AICGetIPBearing(planenum, &range);
			break;
		case 2:
			msgnum = AIC_TARGET_BEARING;
			bearing = AICGetTargetBearing(planenum, &range);
			break;
		case 3:
			msgnum = AIC_PACKAGE_BEARING;
			bearing = AICGetPackageBearing(planenum, &range, &altitude);

			altval = (altitude * WUTOFT) / 1000.0f;
			sprintf(altstr, "%d", altval);

			if(!LANGGetTransMessage(taltstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
			{
				return;
			}
			targetsnd = 6;
			break;
		case 4:
			msgnum = AIC_HOME_BEARING;
			bearing = AICGetHomeBearing(planenum, &range);
			targetsnd = 3;
			break;
		case 5:
			msgnum = AIC_TANKER_BEARING;
			bearing = AICGetTankerBearing(planenum, &range, &altitude);

			altval = (altitude * WUTOFT) / 1000.0f;
			sprintf(altstr, "%d", altval);

			if(!LANGGetTransMessage(taltstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
			{
				return;
			}
			targetsnd = 4;
			break;
		case 6:
			msgnum = AIC_DIVERT_BEARING;
			bearing = AICGetDivertBearing(planenum, &range);
			targetsnd = 5;
			break;
	}

	if(bearing > 360)
	{
		if(awacsnum >= 0)
		{
			AICAddSoundCall(AIC_Basic_Neg, awacsnum, 1000, 50, 1);
		}
		return;
	}

	AICGetCallSign(planenum, callsign);

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	rangenm = (range * WUTONM);
	AICGetRangeText(rangestr, rangenm);

	if(!LANGGetTransMessage(tempstr, 256, msgnum, g_iLanguageId, callsign, bearstr, rangestr, taltstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AICAlphaCheckSnd(planenum, targetsnd, bearing, range, altitude, awacsnum, voice);
	return;
}

//**************************************************************************************
void AICAlphaCheckSnd(int planenum, int targetnum, float bearing, float range, float altitude, int awacsnum, int voice)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int talknum = (awacsnum < 0) ? 1 : awacsnum;
	int usevoice = (awacsnum < 0) ? voice : Planes[awacsnum].AI.lAIVoice;
	int msgsnd;
	long altft = altitude * WUTOFT;

	numids = 0;

	msgsnd = AICF_ITEM_BEARING;

	if((targetnum == 4) || (targetnum == 6))
	{
		msgsnd = AICF_ITEM_BEARING_ALT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgsnd, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, talknum, planenum, bearing, 0, 0, range * WUTONM, altft, 0, 0, voice);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (368 + targetnum) + usevoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + usevoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
float AICGetBullseyeBearing(int planenum, float *range)
{
	FPoint	position;
	float dx, dy, dz, offangle, tdist;

	if(!(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0))))
	{
		return(1000);
	}
	position.X = ConvertWayLoc(BullsEye.x);
	position.Y = 0;
	position.Z = ConvertWayLoc(BullsEye.z);

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	return(offangle);
}

//**************************************************************************************
float AICGetIPBearing(int planenum, float *range)
{
	FPoint	position;
	float dx, dy, dz, offangle, tdist;
	MBWayPoints	*bombway;	//  This is a pointer to the bomb waypoint.

	bombway = AICGetBombWaypoint(planenum, 1);
	if(bombway == NULL)
	{
		return(1000);
	}

	if((bombway - AIWayPoints) > Planes[planenum].AI.startwpts)
	{
		bombway --;
	}

	position.X = ConvertWayLoc(bombway->lWPx);
	position.Y = 0;
	position.Z = ConvertWayLoc(bombway->lWPz);

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	return(offangle);
}

//**************************************************************************************
float AICGetTargetBearing(int planenum, float *range)
{
	FPoint	position;
	float dx, dy, dz, offangle, tdist;
	MBWayPoints	*bombway;	//  This is a pointer to the bomb waypoint.
	MBWayPoints	*lookway;	//  This is a pointer to the bomb waypoint.
	BombTarget *pActionBombTarget;
	AttackTargetActionType *pActionAttackTarget;
	int startact, endact, actcnt;
	BasicInstance *walker;
	int done = 0;
	PlaneParams *planepnt = &Planes[planenum];
	int waycnt;
	int numwpts;

	bombway = AICGetBombWaypoint(planenum, 1);
	if(bombway == NULL)
	{
		lookway = &AIWayPoints[planepnt->AI.startwpts];

		numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
		for(waycnt = 0; (waycnt < numwpts) && (!done); waycnt ++)
		{
			startact = lookway->iStartAct;
			endact = startact + lookway->iNumActs;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				if((AIActions[actcnt].ActionID == ACTION_ATTACKTARGET))
				{
					pActionAttackTarget = (AttackTargetActionType *)AIActions[actcnt].pAction;
					if(pActionAttackTarget->lTargetType == AIRCRAFT)
					{
						position = Planes[pActionAttackTarget->lTargetID].WorldPosition;
					}
					else if(pActionAttackTarget->lTargetType == GROUNDOBJECT)
					{
						walker = FindInstance(AllInstances, pActionAttackTarget->lTargetID);
						if(!walker)
							return(1000.0f);
						position = walker->Position;
					}
					else if(pActionAttackTarget->lTargetType == MOVINGVEHICLE)
					{
						position = MovingVehicles[AIConvertVGroupToVNum(pActionAttackTarget->lTargetID)].WorldPosition;
					}
					else
					{
						return(1000.0f);
					}
					done = 1;
					break;
				}
			}
			lookway ++;
		}

		return(1000);
	}
	else
	{
		pActionBombTarget = NULL;

		startact = bombway->iStartAct;
		endact = startact + bombway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
			{
				pActionBombTarget=(BombTarget *)AIActions[actcnt].pAction;
				break;
			}
		}

		if(pActionBombTarget == NULL)
		{
			return(1000);
		}

		walker = FindInstance(AllInstances, pActionBombTarget->FlightAttackList[0].dwSerialNumber);

		if(walker == NULL)
		{
			position.X = ConvertWayLoc(bombway->lWPx);
			position.Y = 0;
			position.Z = ConvertWayLoc(bombway->lWPz);
		}
		else
		{
			position = walker->Position;
		}
	}

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	return(offangle);
}

//**************************************************************************************
float AICGetHomeBearing(int planenum, float *range)
{
	FPoint	position;
	float dx, dy, dz, offangle, tdist;
	MovingVehicleParams *carrier = NULL;
	MBWayPoints *lookway;
	FPointDouble lastwaypos;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;

	lookway = Planes[planenum].AI.CurrWay + (Planes[planenum].AI.numwaypts - 1);

	lastwaypos.X = ConvertWayLoc(lookway->lWPx);
	lastwaypos.Y = 0;
	lastwaypos.Z = ConvertWayLoc(lookway->lWPz);

	if(AIGetClosestAirField(&Planes[planenum], lastwaypos, &runwaypos, &runwayheading, &TempActionTakeOff, 5.0f * NMTOWU, 0))
	{
		position = runwaypos;
	}
	else
	{
		if(Planes[planenum].AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
			position = carrier->WorldPosition;
		}

		if(carrier == NULL)
		{
			if(AIGetClosestAirField(&Planes[planenum], lastwaypos, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
			{
				position = runwaypos;
			}
			else
			{
				return(1000.0f);
			}
		}
	}

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	return(offangle);
}

//**************************************************************************************
float AICGetTankerBearing(int planenum, float *range, float *altitude)
{
	PlaneParams *tanker;
	FPoint	position;
	float dx, dy, dz, offangle, tdist;

	*altitude = 0;

	tanker = AICheckTankerCloseBy(PlayerPlane, -1);

	if(!tanker)
	{
		return(1000.0f);
	}

	position = tanker->WorldPosition;

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	*altitude = tanker->WorldPosition.Y;
	return(offangle);
}

//**************************************************************************************
float AICGetDivertBearing(int planenum, float *range)
{
	FPoint	position;
	float dx, dy, dz, offangle, tdist;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;

	if(!AIGetClosestAirField(&Planes[planenum], Planes[planenum].WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0))
	{
		return(1000);
	}
	position = runwaypos;

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	return(offangle);
}

//**************************************************************************************
float AICGetPackageBearing(int planenum, float *range, float *altitude)
{
	FPoint	position;
	float dx, dy, dz, offangle, tdist;
	int waycnt;
	int numwpts;
	PlaneParams *package = NULL;
	Escort	*pEscortAction;
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	PlaneParams *planepnt = &Planes[planenum];

	*altitude = 0;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; (waycnt < numwpts) && (!package); waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				if((pEscortAction->iPlaneNum >= 0) && (pEscortAction->iPlaneNum <= (LastPlane - Planes)))
				{
					package = &Planes[pEscortAction->iPlaneNum];
					break;
				}
			}
		}
		lookway ++;
	}

	if(package == NULL)
	{
		return(1000);
	}

	position = package->WorldPosition;

	offangle = AIComputeHeadingToPoint(&Planes[planenum], position, &tdist, &dx ,&dy, &dz, 0);
	*range = tdist;
	*altitude = package->WorldPosition.Y;
	return(offangle);
}


//**************************************************************************************
void AICPlayerRequestTakeOff()
{
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->OnGround == 2)
	{
		CTSetPlaneReadyToLaunch(planepnt);

		return;
	}

//	AIC_Request_Takeoff_Msg(planepnt - Planes, 0);
}

//**************************************************************************************
void AICFlightPositionCheck(int planenum, int targetnum)
{

	long delaycnt = 3000;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}
	planenum = planepnt - Planes;

//	AIC_WSO_Check_Msgs(planepnt - Planes, 1);

	if(planepnt->AI.wingman >= 0)
	{
		AICFlightPositionCheckPlane(planepnt->AI.wingman, &delaycnt, planenum);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICFlightPositionCheckPlane(planepnt->AI.nextpair, &delaycnt, planenum);
	}
	return;
}

//**************************************************************************************
void AICFlightPositionCheckPlane(int planenum, long *delaycnt, int leadplane)
{
	int planeok = 1;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);
	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(AICAllowThisRadio(planenum, 1))
		{
			AICAddSoundCall(AICPositionCheck, planenum, *delaycnt, 50, leadplane);
			*delaycnt = *delaycnt + 6000;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICFlightPositionCheckPlane(Planes[planenum].AI.wingman, delaycnt, leadplane);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICFlightPositionCheckPlane(Planes[planenum].AI.nextpair, delaycnt, leadplane);
	}
	return;
}

//**************************************************************************************
void AICPositionCheck(int planenum, int leadplanenum)
{
	int placeingroup;
	char callsign[128];
	char tempstr[1024];
	char tempstr2[1024];
	char placestr[128];
	char altstr[128];
	char taltstr[128];
	char aspectstr[128];
	char eheadingstr[128];
	char bearstr[128];
	char rangestr[128];
	int bearing360;
	float rangenm;
	float tbearing;
	int headingval;
	int altval;
	int headingval2;
	char bcardinal[128];
	int aspectval;
	float trange, dx, dz;
	PlaneParams *planepnt = &Planes[planenum];
	PlaneParams *leadplane = &Planes[leadplanenum];
	float bearing;
	float range;
	int digitheading2;
	int msgnumb, msgsndb;


	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIRGenericSpeech(AICF_WING_POS_START, planenum, leadplanenum);

	AIC_Get_Callsign_With_Number(planenum, callsign);
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(placestr, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		digitheading2 = tbearing = AIComputeHeadingFromBullseye(planepnt->WorldPosition, &trange, &dx, &dz);
		headingval2 = AICGetTextHeadingGeneral(bcardinal, tbearing);
	}

	dx = planepnt->WorldPosition.X - leadplane->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - leadplane->WorldPosition.Z;

	bearing = atan2(-dx, -dz) * 57.2958;

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	aspectval = AICGetAspectText(aspectstr, bearing, planepnt);

	altval = (planepnt->WorldPosition.Y * WUTOFT) / 1000.0f;

	sprintf(altstr, "%d", altval);

	if(!LANGGetTransMessage(taltstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
	{
		return;
	}

	headingval = AICGetTextHeadingGeneral(eheadingstr, AIConvertAngleTo180Degree(planepnt->Heading));

	range = QuickDistance(dx, dz);
	rangenm = (range * WUTONM);
	if(rangenm < 1)
		rangenm = 1;
	else if(rangenm > 100)
		rangenm = 100;
	AICGetRangeText(rangestr, rangenm);

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)) && (rangenm > 15) && ((trange * WUTONM) < 100))
	{
		rangenm = (trange * WUTONM);
		AICGetRangeText(rangestr, rangenm);

		if(iUseBullseye == 2)
		{
			msgnumb = AIC_DO_DIGIT_BULLSEYE;
			msgsndb = AICF_DO_DIGIT_BULLSEYE;

			tbearing = -digitheading2;
			headingval2 = digitheading2;
			if(tbearing < 0)
			{
				bearing360 = tbearing + 360;
			}
			else
			{
				bearing360 = tbearing;
			}
			sprintf(bearstr, "%03d", bearing360);

			if(!LANGGetTransMessage(tempstr, 1024, msgnumb, g_iLanguageId, rangestr, bearstr, taltstr, eheadingstr))
			{
				return;
			}
			AIRGenericSpeech(msgsndb, planenum, planenum, bearing, headingval, headingval2, rangenm, planepnt->WorldPosition.Y * WUTOFT, 0);
		}
		else
		{
			msgnumb = AIC_DO_BULLSEYE;
			msgsndb = AICF_DO_POS_BULLSEYE;

			if(!LANGGetTransMessage(tempstr, 1024, msgnumb, g_iLanguageId, rangestr, bcardinal, taltstr, eheadingstr))
			{
				return;
			}
			AIRGenericSpeech(msgsndb, planenum, planenum, bearing, headingval, headingval2, rangenm, planepnt->WorldPosition.Y * WUTOFT, 0);
		}

	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_DO_BRA, g_iLanguageId, bearstr, rangestr, taltstr, aspectstr))
		{
			return;
		}

		AIRGenericSpeech(AICF_DO_BRA, planenum, planenum, bearing, headingval, headingval2, rangenm, planepnt->WorldPosition.Y * WUTOFT, 0);
	}

	if(!LANGGetTransMessage(tempstr2, 1024, AIC_FLIGHT_POSITION_INFO, g_iLanguageId, placestr, tempstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr2, 50);
	return;
}

//**************************************************************************************
void AICShowEscortEngageCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int planenum = PlayerPlane - Planes;
	PlaneParams *checkplane;
	int optionnum = 1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
				{
					if(optionnum == 10)
					{
						optionnum = 0;
					}
					theight = AICDisplayEscorts(GrBuffFor3D, AIC_RELEASE_ESCORT_OPTIONS, optionnum, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width, checkplane - Planes);
					if(optionnum == 0)
					{
						return;
					}
					optionnum ++;
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AICPixelLenEscortEngageCommOps()
{
	int messagelen = 0;
	int worklen;
	char tempstr[1024];
	char callsign[256];
	char escortstr[256];
	PlaneParams *checkplane;
	int optionnum = 1;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
				{
					AIC_Get_Callsign_With_Number(checkplane - Planes, callsign);
					if(AIIsSEADVoice(checkplane))
					{
						if(!LANGGetTransMessage(escortstr, 256, AIC_SEAD, g_iLanguageId))
						{
							return(640);
						}
					}
					else
					{
						if(!LANGGetTransMessage(escortstr, 256, AIC_COVER, g_iLanguageId))
						{
							return(640);
						}
					}
					if(!LANGGetTransMessage(tempstr, 1024, AIC_RELEASE_ESCORT_OPTIONS, g_iLanguageId, optionnum, callsign, escortstr))
					{
						return(640);
					}
					worklen = TXTGetStringPixelLength(tempstr, MessageFont);
					if(worklen > messagelen)
					{
						messagelen = worklen;
					}

					optionnum ++;
				}
			}
		}
		checkplane ++;
	}

	return(messagelen + 5);
}

//**************************************************************************************
void AICEscortEngageCommKeyOps(int keyflag, WPARAM wParam)
{
	int planenum;
	int optionnum = 1;
	int lookforopt;
	PlaneParams *checkplane;
	PlaneParams *target;

	if(iAICommFrom == -1)
	{
		planenum = PlayerPlane - Planes;
	}
	else
	{
		planenum = iAICommFrom;
	}

	lookforopt = wParam - '0';
	if((lookforopt < 0) || (lookforopt > 9))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}
	else if(lookforopt == 0)
	{
		lookforopt = 10;
	}

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
				{
					if(optionnum == lookforopt)
					{
						target = NULL;
						checkplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
						checkplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
						checkplane->AI.iAIFlags2 &= ~(AI_SEAD_COVER);

						if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
						{
							NetPutGenericMessage1(checkplane, GM_ESCORT_ENGAGE);
						}

						if(Planes[planenum].AI.AirTarget)
						{
							target = Planes[planenum].AI.AirTarget;
						}
						else if(Planes[planenum].AI.AirThreat)
						{
							target = Planes[planenum].AI.AirThreat;
						}
						else if(checkplane->AI.AirTarget)
						{
							target = checkplane->AI.AirTarget;
						}
						else
						{
							target = checkplane->AI.AirThreat;
						}

						if(target)
						{
							if((!MultiPlayer) || (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								if(checkplane->AI.Behaviorfunc == AIFlyEscort)
								{
									AISetUpEscortInterceptGroup(checkplane, target);
								}
								else
								{
									AIUpdateInterceptGroup(checkplane, target, 1);
								}
								AISetInterceptTimer(target, 15);

								if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
								{
									NetPutGenericMessage2(checkplane, GM2_VECTOR, (BYTE)(target - Planes));
								}
							}
//							else
//							{
//								AICCAPEngagingMsg(checkplane - Planes, target - Planes);
//							}
						}
						else
						{
							AIC_Basic_Ack(checkplane - Planes);
						}
						gAICommMenu.AICommMenufunc = NULL;
						gAICommMenu.AICommKeyfunc = NULL;
						gAICommMenu.lTimer = -1;
						return;
					}
					optionnum ++;
				}
			}
		}
		checkplane ++;
	}

	gAICommMenu.AICommMenufunc = NULL;
	gAICommMenu.AICommKeyfunc = NULL;
	gAICommMenu.lTimer = -1;
}

//**************************************************************************************
void AICShowEscortSEADCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int planenum = PlayerPlane - Planes;
	PlaneParams *checkplane;
	int optionnum = 1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(AIIsSEADVoice(checkplane) && ((!(checkplane->AI.iAIFlags2 & AI_SEAD_COVER)) || (checkplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))))
					{
						if(optionnum == 10)
						{
							optionnum = 0;
						}
						theight = AICDisplayEscorts(GrBuffFor3D, AIC_RELEASE_ESCORT_OPTIONS, optionnum, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width, checkplane - Planes);
						if(optionnum == 0)
						{
							return;
						}
						optionnum ++;
					}
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AICPixelLenEscortSEADCommOps()
{
	int messagelen = 0;
	int worklen;
	char tempstr[1024];
	char callsign[256];
	char escortstr[256];
	PlaneParams *checkplane;
	int optionnum = 1;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(AIIsSEADVoice(checkplane) && ((!(checkplane->AI.iAIFlags2 & AI_SEAD_COVER)) || (checkplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))))
					{
						AIC_Get_Callsign_With_Number(checkplane - Planes, callsign);
						if(AIIsSEADVoice(checkplane))
						{
							if(!LANGGetTransMessage(escortstr, 256, AIC_SEAD, g_iLanguageId))
							{
								return(640);
							}
						}
						else
						{
							if(!LANGGetTransMessage(escortstr, 256, AIC_COVER, g_iLanguageId))
							{
								return(640);
							}
						}
						if(!LANGGetTransMessage(tempstr, 1024, AIC_RELEASE_ESCORT_OPTIONS, g_iLanguageId, optionnum, callsign, escortstr))
						{
							return(640);
						}
						worklen = TXTGetStringPixelLength(tempstr, MessageFont);
						if(worklen > messagelen)
						{
							messagelen = worklen;
						}

						optionnum ++;
					}
				}
			}
		}
		checkplane ++;
	}

	return(messagelen + 5);
}

//**************************************************************************************
void AICEscortSEADCommKeyOps(int keyflag, WPARAM wParam)
{
	int planenum;
	int optionnum = 1;
	int lookforopt;
	PlaneParams *checkplane;
	void *target;
	int targettype = -1;

	if(iAICommFrom == -1)
	{
		planenum = PlayerPlane - Planes;
	}
	else
	{
		planenum = iAICommFrom;
	}

	lookforopt = wParam - '0';
	if((lookforopt < 0) || (lookforopt > 9))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}
	else if(lookforopt == 0)
	{
		lookforopt = 10;
	}

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(AIIsSEADVoice(checkplane) && ((!(checkplane->AI.iAIFlags2 & AI_SEAD_COVER)) || (checkplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))))
					{
						if(optionnum == lookforopt)
						{
							checkplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
							checkplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
							checkplane->AI.iAIFlags2 |= AI_SEAD_COVER;

							if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutGenericMessage1(checkplane, GM_ESCORT_SEAD);
							}

							target = NULL;

							if(Planes[planenum].AI.pPaintedBy)
							{
								target = Planes[planenum].AI.pPaintedBy;
								targettype = Planes[planenum].AI.iPaintedByType;
							}
							else
							{
								target = checkplane->AI.pPaintedBy;
								targettype = checkplane->AI.iPaintedByType;
							}

							checkplane->AI.pPaintedBy = target;
							checkplane->AI.iPaintedByType = targettype;
							checkplane->AI.pGroundTarget = NULL;

							if((!MultiPlayer) || (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								checkplane->AI.lRadarDelay = -1;
								AISEADSearch(checkplane);
								if(!checkplane->AI.pGroundTarget)
								{
									AIC_Basic_Ack(checkplane - Planes);
								}
							}
							else
							{
								AIC_Basic_Ack(checkplane - Planes);
							}
							gAICommMenu.AICommMenufunc = NULL;
							gAICommMenu.AICommKeyfunc = NULL;
							gAICommMenu.lTimer = -1;
							return;
						}
						optionnum ++;
					}
				}
			}
		}
		checkplane ++;
	}

	gAICommMenu.AICommMenufunc = NULL;
	gAICommMenu.AICommKeyfunc = NULL;
	gAICommMenu.lTimer = -1;
}

//**************************************************************************************
void AICShowEscortCASCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int planenum = PlayerPlane - Planes;
	PlaneParams *checkplane;
	int optionnum = 1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if((AIIsBomberVoice(checkplane) || AIIsSEADVoice(checkplane)) && ((!(checkplane->AI.iAICombatFlags2 & (AI_ESCORT_CAS))) || (checkplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))))
					{
						if(optionnum == 10)
						{
							optionnum = 0;
						}
						theight = AICDisplayEscorts(GrBuffFor3D, AIC_RELEASE_ESCORT_OPTIONS, optionnum, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width, checkplane - Planes);
						if(optionnum == 0)
						{
							return;
						}
						optionnum ++;
					}
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AICPixelLenEscortCASCommOps()
{
	int messagelen = 0;
	int worklen;
	char tempstr[1024];
	char callsign[256];
	char escortstr[256];
	PlaneParams *checkplane;
	int optionnum = 1;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if((AIIsBomberVoice(checkplane) || AIIsSEADVoice(checkplane)) && ((!(checkplane->AI.iAICombatFlags2 & (AI_ESCORT_CAS))) || (checkplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))))
					{
						AIC_Get_Callsign_With_Number(checkplane - Planes, callsign);
						if(AIIsSEADVoice(checkplane))
						{
							if(!LANGGetTransMessage(escortstr, 256, AIC_SEAD, g_iLanguageId))
							{
								return(640);
							}
						}
						else
						{
							if(!LANGGetTransMessage(escortstr, 256, AIC_COVER, g_iLanguageId))
							{
								return(640);
							}
						}
						if(!LANGGetTransMessage(tempstr, 1024, AIC_RELEASE_ESCORT_OPTIONS, g_iLanguageId, optionnum, callsign, escortstr))
						{
							return(640);
						}
						worklen = TXTGetStringPixelLength(tempstr, MessageFont);
						if(worklen > messagelen)
						{
							messagelen = worklen;
						}

						optionnum ++;
					}
				}
			}
		}
		checkplane ++;
	}

	return(messagelen + 5);
}

//**************************************************************************************
void AICEscortCASCommKeyOps(int keyflag, WPARAM wParam)
{
	int planenum;
	int optionnum = 1;
	int lookforopt;
	PlaneParams *checkplane;

	if(iAICommFrom == -1)
	{
		planenum = PlayerPlane - Planes;
	}
	else
	{
		planenum = iAICommFrom;
	}

	lookforopt = wParam - '0';
	if((lookforopt < 0) || (lookforopt > 9))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}
	else if(lookforopt == 0)
	{
		lookforopt = 10;
	}

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if((AIIsBomberVoice(checkplane) || AIIsSEADVoice(checkplane)) && ((!(checkplane->AI.iAICombatFlags2 & (AI_ESCORT_CAS))) || (checkplane->AI.iAICombatFlags2 & (AI_ESCORT_STAY))))
					{
						if(optionnum == lookforopt)
						{
							checkplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
							checkplane->AI.iAICombatFlags2 |= AI_ESCORT_CAS;
							checkplane->AI.iAIFlags2 &= ~AI_SEAD_COVER;
							checkplane->AI.pGroundTarget = NULL;

							if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutGenericMessage1(checkplane, GM_ESCORT_CAS);
							}

							if((!MultiPlayer) || (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								checkplane->AI.lRadarDelay = -1;
								AISimpleGroundRadar(checkplane);
								if(!checkplane->AI.pGroundTarget)
								{
									AIC_Basic_Ack(checkplane - Planes);
								}
							}
							else
							{
								AIC_Basic_Ack(checkplane - Planes);
							}

							gAICommMenu.AICommMenufunc = NULL;
							gAICommMenu.AICommKeyfunc = NULL;
							gAICommMenu.lTimer = -1;
							return;
						}
						optionnum ++;
					}
				}
			}
		}
		checkplane ++;
	}

	gAICommMenu.AICommMenufunc = NULL;
	gAICommMenu.AICommKeyfunc = NULL;
	gAICommMenu.lTimer = -1;
}

//**************************************************************************************
void AICShowEscortRejoinCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int planenum = PlayerPlane - Planes;
	PlaneParams *checkplane;
	int optionnum = 1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc != AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))  // Should this be AIGetLeader(PlayerPoint
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(!(checkplane->AI.iAICombatFlags2 & AI_ESCORT_BUSY))
					{
						if((checkplane->AI.OrgBehave == AIFlyEscort) || (checkplane->AI.iAIFlags2 & AI_RETURN_SEAD_COVER) || (checkplane->AI.iAIFlags2 & AI_SEAD_COVER) || (checkplane->AI.iAICombatFlags2 & AI_ESCORT_CAS))
						{
							if(optionnum == 10)
							{
								optionnum = 0;
							}
							theight = AICDisplayEscorts(GrBuffFor3D, AIC_RELEASE_ESCORT_OPTIONS, optionnum, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width, checkplane - Planes);
							if(optionnum == 0)
							{
								return;
							}
							optionnum ++;
						}
					}
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AICPixelLenEscortRejoinCommOps()
{
	int messagelen = 0;
	int worklen;
	char tempstr[1024];
	char callsign[256];
	char escortstr[256];
	PlaneParams *checkplane;
	int optionnum = 1;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc != AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))  // Should this be AIGetLeader(PlayerPoint
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(!(checkplane->AI.iAICombatFlags2 & AI_ESCORT_BUSY))
					{
						if((checkplane->AI.OrgBehave == AIFlyEscort) || (checkplane->AI.iAIFlags2 & AI_RETURN_SEAD_COVER) || (checkplane->AI.iAIFlags2 & AI_SEAD_COVER) || (checkplane->AI.iAICombatFlags2 & AI_ESCORT_CAS))
						{
							AIC_Get_Callsign_With_Number(checkplane - Planes, callsign);
							if(AIIsSEADVoice(checkplane))
							{
								if(!LANGGetTransMessage(escortstr, 256, AIC_SEAD, g_iLanguageId))
								{
									return(640);
								}
							}
							else
							{
								if(!LANGGetTransMessage(escortstr, 256, AIC_COVER, g_iLanguageId))
								{
									return(640);
								}
							}
							if(!LANGGetTransMessage(tempstr, 1024, AIC_RELEASE_ESCORT_OPTIONS, g_iLanguageId, optionnum, callsign, escortstr))
							{
								return(640);
							}
							worklen = TXTGetStringPixelLength(tempstr, MessageFont);
							if(worklen > messagelen)
							{
								messagelen = worklen;
							}

							optionnum ++;
						}
					}
				}
			}
		}
		checkplane ++;
	}

	return(messagelen + 5);
}

//**************************************************************************************
void AICEscortRejoinCommKeyOps(int keyflag, WPARAM wParam)
{
	int planenum;
	int optionnum = 1;
	int lookforopt;
	PlaneParams *checkplane;

	if(iAICommFrom == -1)
	{
		planenum = PlayerPlane - Planes;
	}
	else
	{
		planenum = iAICommFrom;
	}

	lookforopt = wParam - '0';
	if((lookforopt < 0) || (lookforopt > 9))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}
	else if(lookforopt == 0)
	{
		lookforopt = 10;
	}

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc != AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))  // Should this be AIGetLeader(PlayerPoint
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(!(checkplane->AI.iAICombatFlags2 & AI_ESCORT_BUSY))
					{
						if((checkplane->AI.OrgBehave == AIFlyEscort) || (checkplane->AI.iAIFlags2 & AI_RETURN_SEAD_COVER) || (checkplane->AI.iAIFlags2 & AI_SEAD_COVER) || (checkplane->AI.iAICombatFlags2 & AI_ESCORT_CAS))
						{
							if(optionnum == lookforopt)
							{
								AIChangeGroupBehavior(checkplane, checkplane->AI.Behaviorfunc, AIFlyEscort, 0, 1);
								checkplane->AI.iAICombatFlags2 |= (AI_ESCORT_STAY);
								checkplane->AI.iAIFlags2 &= ~AI_SEAD_COVER;
								checkplane->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
								if(AIIsSEADPlane(checkplane))
								{
									checkplane->AI.iAIFlags2 |= AI_SEAD_COVER;
								}
								AIC_Basic_Ack(checkplane - Planes);

								if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
								{
									NetPutGenericMessage1(checkplane, GM_ESCORT_REJOIN);
								}

								gAICommMenu.AICommMenufunc = NULL;
								gAICommMenu.AICommKeyfunc = NULL;
								gAICommMenu.lTimer = -1;
								return;
							}
							optionnum ++;
						}
					}
				}
			}
		}
		checkplane ++;
	}

	gAICommMenu.AICommMenufunc = NULL;
	gAICommMenu.AICommKeyfunc = NULL;
	gAICommMenu.lTimer = -1;
}

//**************************************************************************************
int AIRejoinableEscorts(PlaneParams *planepnt, PlaneParams *leadprotect)
{
	int numescorting = 0;
	int seadescorting = 0;
	PlaneParams *checkplane;

	checkplane = &Planes[0];

	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.LinkedPlane == leadprotect) && (checkplane->AI.Behaviorfunc != AIFlyEscort))
			{
				if(!(checkplane->AI.iAICombatFlags2 & AI_ESCORT_BUSY))
				{
					if(checkplane->AI.OrgBehave == AIFlyEscort)
						return(1);
					else if(checkplane->AI.iAIFlags2 & AI_RETURN_SEAD_COVER)
						return(1);
					else if(checkplane->AI.iAIFlags2 & AI_SEAD_COVER)
						return(1);
				}
			}
		}
		checkplane ++;
	}
	return(0);
}

//**************************************************************************************
int AICCheckFACClose(PlaneParams *planepnt)
{
	int planeclose = 0;
	int groundclose = 0;
	float dx, dz, tdist, tdistnm;
	PlaneParams *checkplane;

	if(!iFACState)
		return(0);

	if(iFACState & 0xFFFF)
	{
		dx = fpFACPosition.X - planepnt->WorldPosition.X;
		dz = fpFACPosition.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx, dz);
		tdistnm = (tdist * WUTONM);

		if(tdistnm < 20)
		{
			groundclose = 1;
		}
	}

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt != checkplane) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			if(checkplane->AI.iAICombatFlags2 & (AI_FAC))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;

				tdist = QuickDistance(dx, dz);
				tdistnm = (tdist * WUTONM);

				if(tdistnm < 20)
				{
					return(0x10000 | groundclose);
				}
			}
		}
	}
	return(groundclose);
}

//**************************************************************************************
void AICCheckInWithFAC(int planenum, int targetnum)
{
	int numagm, numguided, numdumb, numcluster, numpod;
	char numstr[128];
	int placeingroup;
	char heaterstr[128];
	char tempstr[1024];
	char tempstr3[256];
	int fac, faccallsign;
	long facvoice;
	int facnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	fac = AICGetClosestFAC(&Planes[planenum]);

	if(fac == -1)
		return;
	else if(fac == -2)
	{
		faccallsign = AIRGetCallSignSndID(FACObject.lCallSign, 0);
		facnum = AIGetFACCallsign(planenum, &facvoice);
		strcpy(heaterstr, CallSignList[facnum].sName);
		iFACState &= ~0x1;
		iFACState |= 0x8;
		if(!(iFACHitFlags & (FAC_LASER|FAC_NO_LASER)))
		{
			if(rand() & 1)
			{
				iFACHitFlags |= FAC_LASER;
			}
			else
			{
				iFACHitFlags |= FAC_NO_LASER;
			}
		}
	}
	else
	{
		AIC_Get_Callsign_With_Number(fac, heaterstr);
		faccallsign = fac;
		iFACState &= ~0x10000;
		iFACState |= 0x80000;
	}

	AIC_Get_Callsign_With_Number(planenum, tempstr3);

	strcat(heaterstr, " ");
	strcat(heaterstr, tempstr3);


	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	numagm = AIGetNumWeaponType(planenum, 5);
	numguided = AIGetNumWeaponType(planenum, 6);
	numdumb = AIGetNumWeaponType(planenum, 7);
	numcluster = AIGetNumWeaponType(planenum, 8);
	numpod = AIGetNumWeaponType(planenum, 9);

	if(numagm)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numagm, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_A2M_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numguided)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numguided, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_GUIDED_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numcluster)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numcluster, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_CLUSTER_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numdumb)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numdumb, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_DUMB_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numpod)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numpod, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_POD_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_ON_STATION, g_iLanguageId, heaterstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AICCheckInWithFACSnd(planenum, faccallsign, numagm, numguided, numdumb, numcluster, numpod);

	AICAddSoundCall(AIC_FACAck, planenum, 6000, 50, 0);

	return;
}

//**************************************************************************************
void AICCheckInWithFACSnd(int planenum, int faccallsign, int numagm, int numguided, int numdumb, int numcluster, int numpod)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_FAC_CHECK_IN, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, faccallsign);
		}
		else if(spchvar == 2)
		{
			if((numval == 5) && (numagm))
			{
				if(numagm < 10)
				{
					sndids[numids] = (SPCH_ZERO + numagm) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numagm - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (718) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 6) && (numguided))
			{
				if(numguided < 10)
				{
					sndids[numids] = (SPCH_ZERO + numguided) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numguided - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (719) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 7) && (numdumb))
			{
				if(numdumb < 10)
				{
					sndids[numids] = (SPCH_ZERO + numdumb) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numdumb - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (720) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 8) && (numcluster))
			{
				if(numcluster < 10)
				{
					sndids[numids] = (SPCH_ZERO + numcluster) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numcluster - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (721) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 9) && (numpod))
			{
				if(numpod < 10)
				{
					sndids[numids] = (SPCH_ZERO + numpod) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numpod - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (722) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
int AICGetClosestFAC(PlaneParams *planepnt)
{
	int closest = -1;
	float dx, dz, tdist, tdistnm;
	PlaneParams *checkplane;
	float closestnm = 9999.0f;

	if(iFACState & 0xFFFF)
	{
		dx = fpFACPosition.X - planepnt->WorldPosition.X;
		dz = fpFACPosition.Z - planepnt->WorldPosition.Z;

		tdist = QuickDistance(dx, dz);
		tdistnm = (tdist * WUTONM);

		if(tdistnm < 20.0f)
		{
			closest = -2;
			closestnm = tdistnm;
		}
	}

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt != checkplane) && (planepnt->AI.iSide == checkplane->AI.iSide))
		{
			if(checkplane->AI.iAICombatFlags2 & (AI_FAC))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;

				tdist = QuickDistance(dx, dz);
				tdistnm = (tdist * WUTONM);

				if(tdistnm < closestnm)
				{
					closest = checkplane - Planes;
					closestnm = tdistnm;
				}
			}
		}
	}
	return(closest);
}

//**************************************************************************************
void AIC_FACAck(int planenum, int targetnum)
{
	char tempstr[1024];
	int voice = 0;
	int msgnum, msgsnd;
	char callsign[256];
	int fac;

	fac = AICGetClosestFAC(&Planes[planenum]);

	if(fac == -1)
		return;
	else if(fac == -2)
	{
		voice = SPCH_FAC_GRND;
	}

	switch(targetnum)
	{
		case 1:
			msgnum = AIC_AFFIRMATIVE;
			msgsnd = AICF_FAC_AFFIRM;
			break;
		case 2:
			msgnum = AIC_NEGATIVE;
			msgsnd = AICF_FAC_NEGATIVE;
			break;
		case 3:
			msgnum = AIC_STANDBY;
			msgsnd = AICF_FAC_STANDBY;
			break;
		case 4:
			msgnum = AIC_STANDBY_FOR_WORDS;
			msgsnd = AICF_STANDBY_FOR_WORDS;
			break;
		case 5:
			msgnum = AIC_STANDBY_FOR_NINE;
			msgsnd = AICF_STANDBY_FOR_NINE;
			break;
		case 6:
			msgnum = AIC_STANDBY_FOR_FOUR;
			msgsnd = AICF_STANDBY_FOR_FOUR;
			break;
		case 7:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_4;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_4;
			break;
		case 8:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_2;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_2;
			break;
		case 9:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_5;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_5;
			break;
		case 10:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_6;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_6;
			break;
		case 11:
			msgnum = AIC_GROUND_GOOD_US_4;
			msgsnd = AICF_GROUND_GOOD_US_4;
			break;
		case 12:
			msgnum = AIC_GROUND_GOOD_US_5;
			msgsnd = AICF_GROUND_GOOD_US_5;
			break;
		case 13:
			msgnum = AIC_GROUND_GOOD_US_3;
			msgsnd = AICF_GROUND_GOOD_US_3;
			break;
		case 14:
			msgnum = AIC_FAC_TARGET_CLEAR;
			msgsnd = AICF_FAC_TARGET_CLEAR;
			break;
		case 15:
			msgnum = AIC_COPY_RTB;
			msgsnd = AICF_COPY_RTB;
			break;
		case 16:
			msgnum = AIC_HIT;
			msgsnd = AICF_HIT;
			break;
		case 17:
			msgnum = AIC_TARGET_DESTROYED;
			msgsnd = AICF_TARGET_DESTROYED;
			break;
		case 18:
			msgnum = AIC_TARGET_DAMAGED;
			msgsnd = AICF_TARGET_DAMAGED;
			break;
		default:
			msgnum = AIC_AWACS_COPY;
			msgsnd = AICF_FAC_COPY;
			break;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, fac, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AIC_Established(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];

	msgnum = AIC_ESTABLISHED;
	msgsnd = AICF_ESTABLISHED;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, planenum);
}

//**************************************************************************************
void AIC_FAC_RequestTarget(int planenum, int targetnum)
{
	int fac = AICGetClosestFAC(&Planes[planenum]);

	if(fac == -2)
	{
		if((iFACState & 0xFFFF) == 8)
		{
			AIC_Established(planenum, 0);
//			AIReCheckingForGroundEnemies(fpFACPosition, 10, NULL, AI_FRIENDLY, iFACTargetPos);
			AICheckForGroundEnemies(fpFACPosition, 10, NULL, Planes[planenum].AI.iSide, -2, &fFACRange);
			AICReportFACTarget(fac, planenum, 1, 1);
			iFACState -= 8;
			iFACState |= 3;
			AICPutSmokeOnFACTarget(fac);
		}
		else
		{
			AICReportFACTarget(fac, planenum, 0, 1);
		}
	}
	else
	{
		if((iFACState & 0xFFFF0000) == 0x80000)
		{
			AIC_Established(planenum, 0);
			if(Planes[fac].AI.pGroundTarget)
				Planes[fac].AI.iAICombatFlags2 |= AI_FAC_REDO_TARGET;
			Planes[fac].AI.iAICombatFlags2 &= ~(AI_FAC_JUST_LOOK);
			Planes[fac].AI.lRadarDelay = -1;
			AISimpleGroundRadar(&Planes[fac]);
			iFACState &= ~0x80000;
			iFACState |= 0x30000;
		}
		else
		{
			AICReportFACTarget(fac, planenum, 0, 1);
		}
	}
}

//**************************************************************************************
void AIC_FAC_RequestNewTarget(int planenum, int targetnum)
{
	int fac = AICGetClosestFAC(&Planes[planenum]);

	if(fac == -2)
	{
		AICheckForGroundEnemies(fpFACPosition, 10, NULL, Planes[planenum].AI.iSide, -2, &fFACRange);
		AICReportFACTarget(fac, planenum, 0, 1);
		AICPutSmokeOnFACTarget(fac);
	}
	else
	{
		AICAddSoundCall(AIC_FACAck, planenum, 4000, 50, 4);
		Planes[fac].AI.pGroundTarget = NULL;
	}
}

//**************************************************************************************
void AIC_FAC_Blind(int planenum, int targetnum)
{
	int fac = AICGetClosestFAC(&Planes[planenum]);

	if(fac == -2)
	{
		AICPutSmokeOnFACTarget(fac);
		AICReportFACTarget(fac, planenum, 1, 1);
	}
	else
	{
		if(!(AICheckFACSmoke(&Planes[fac])))
		{
			AICReportFACTarget(fac, planenum, 1, 1);
		}
		else
		{
			Planes[fac].AI.iAICombatFlags2 |= AI_FAC_REDO_TARGET;
			Planes[fac].AI.lRadarDelay = -1;
			AISimpleGroundRadar(&Planes[fac]);
		}
	}
}

//**************************************************************************************
void AIC_FAC_9Line(int planenum, int facnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[64];
	char bearstr[64];
	char rangestr[64];
	char tempstr3[64];
	char tempstr4[64];
	int closestWP;
	void *pTarget;
	int iTargetType;
	FPointDouble position;
	MovingVehicleParams *vehiclepnt = NULL;
	BasicInstance *walker;
	float dx, dz;  //  , tdist;
	FPointDouble refpos;
	float targdist;
	float tbearing;
	int bearing360;
	int targetelev;
	int targetsnd;
	int smoke360 = 0;
	float smokerange = 0;
	int smokeid;
	int friendly360;
	int friendrangenm;
	int threatsnd;
	int threat360;
	float threatrange;
	void *pThreat;
	int iThreatType;
	FPointDouble threatpos;
	float targbearing = 0;
	float smokebearing = 0;
	float friendlybearing = 0;
	float threatbearing = 0;
	float offangle;

	if(facnum == -2)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
	}
	else
	{
		pTarget = Planes[facnum].AI.pGroundTarget;
		iTargetType = Planes[facnum].AI.lGroundTargetFlag;
	}

	if(!pTarget)
		return;

	if(iTargetType == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)pTarget;
		position = vehiclepnt->WorldPosition;
	}
	else if(iTargetType == GROUNDOBJECT)
	{
		walker = (BasicInstance *)pTarget;
		position = walker->Position;
	}
	else
	{
		return;
	}

	closestWP = AICGetClosestWP(position, planenum);

	if(GetRegValueL("noip") == 1)
	{
		refpos = Planes[planenum].WorldPosition;
	}
	else
	{
		refpos.X = AIWayPoints[Planes[planenum].AI.startwpts + closestWP].lWPx * FTTOWU;
		refpos.Z = AIWayPoints[Planes[planenum].AI.startwpts + closestWP].lWPz * FTTOWU;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	sprintf(tempstr2, "%d", closestWP + 1);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_NINE_LINE_1, g_iLanguageId, callsign, tempstr2))
	{
		return;
	}

	dx = position.X - refpos.X;
	dz = position.Z - refpos.Z;

	offangle = targbearing = atan2(-dx, -dz) * 57.2958;

	tbearing = -offangle;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_2, g_iLanguageId, bearstr))
	{
		return;
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	targdist = QuickDistance(dx, dz);
	AICGetRangeText(rangestr, targdist * WUTONM);

	if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_3, g_iLanguageId, rangestr))
	{
		return;
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	targetelev = position.Y * WUTOFT;
	sprintf(tempstr3, "%d", targetelev);
	if(!LANGGetTransMessage(tempstr4, 64, AIC_DIST_FEET, g_iLanguageId, tempstr3))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_4, g_iLanguageId, tempstr4))
	{
		return;
	}

	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	targetsnd = AICGetTargetType(pTarget, iTargetType, tempstr3);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_5_6, g_iLanguageId, tempstr3))
	{
		return;
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	if(facnum == -2)
	{
		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_7C, g_iLanguageId))
		{
			return;
		}
		smokeid = 1;
	}
	else if(Planes[facnum].AI.iAICombatFlags2 & AI_FAC_REDO_TARGET)
	{
		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_7C, g_iLanguageId))
		{
			return;
		}
		smokeid = 1;
	}
	else
	{
		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_7A, g_iLanguageId))
		{
			return;
		}
		smokeid = 0;
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	friendlybearing = offangle = AICheckForGroundEnemies(position, 5, &friendrangenm, Planes[planenum].AI.iSide, facnum, NULL, 1);

	if(offangle < -600)
	{
		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_8A, g_iLanguageId))
		{
			return;
		}
		friendly360 = -999;
		friendrangenm = -1;
	}
	else
	{
		tbearing = -offangle;
		if(tbearing < 0)
		{
			friendly360 = tbearing + 360;
		}
		else
		{
			friendly360 = tbearing;
		}
		sprintf(tempstr3, "%03d", friendly360);

		if(friendrangenm < 1)
			friendrangenm = 1;

		AICGetRangeText(rangestr, friendrangenm);

		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_8B, g_iLanguageId, tempstr3, rangestr))
		{
			return;
		}
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	AICGetClosestThreat(position, Planes[planenum].AI.iSide, 3, &pThreat, &iThreatType);

	if(!pThreat)
	{
		threatrange = -1;
		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_9A, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(iThreatType == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)pTarget;
			threatpos = vehiclepnt->WorldPosition;
		}
		else if(iTargetType == GROUNDOBJECT)
		{
			walker = (BasicInstance *)pTarget;
			threatpos = walker->Position;
		}
		else
		{
			pThreat = NULL;
		}

		if(pThreat)
		{
			threatsnd = AICGetThreatType(pThreat, iThreatType, tempstr3);
			dx = threatpos.Z - position.X;
			dz = threatpos.Z - position.Z;

			threatbearing = offangle = atan2(-dx, -dz) * 57.2958;

			tbearing = -offangle;
			if(tbearing < 0)
			{
				threat360 = tbearing + 360;
			}
			else
			{
				threat360 = tbearing;
			}
			sprintf(bearstr, "%03d", threat360);

			threatrange = QuickDistance(dx, dz);
			AICGetRangeText(rangestr, threatrange * WUTONM);

			if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_9B, g_iLanguageId, tempstr3, threat360, rangestr))
			{
				return;
			}
		}
		else
		{
			threatrange = -1;
			if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_9A, g_iLanguageId))
			{
				return;
			}
		}
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	AICAddAIRadioMsgs(tempstr, 50);

	AIC_FAC_9LineSnd(planenum, facnum, closestWP, targbearing, targdist, targetelev, targetsnd, smokeid, smokebearing, smokerange, friendlybearing, friendrangenm, threatsnd, threatbearing, threatrange);
}

//**************************************************************************************
void AIC_FAC_9LineSnd(int planenum, int facnum, int closestWP, float targbearing, float targdist, int targetelev, int targetsnd, int smokeid, float smokebearing, float smokerange, float friendlybearing, int friendrangenm, int threatsnd, float threatbearing, float threatrange)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int cnt;
	int msgnum;
	int rangenm;
	int bearing;
	int voice = (facnum == -2) ? SPCH_FAC_GRND : 0;
	int usevoice = (facnum == -2) ? voice : Planes[facnum].AI.lAIVoice;

//	if(lpDD)
//	  lpDD->FlipToGDISurface();

//	SimPause = 1;

	numids = 0;

	for(cnt = 0; cnt < 4; cnt ++)
	{
		switch(cnt)
		{
			case 0:
				msgnum = AICF_START_9_LINE;
				rangenm = targdist * WUTONM;
				bearing = targbearing;
				break;
			case 1:
				if(smokeid == 1)
				{
					msgnum = AICF_MARKER_ON_TARGET;
				}
				else if (smokeid == 2)
				{
					msgnum = AIC_SMOKE_BEARING;
				}
				else
				{
					msgnum = AICF_NO_MARKER;
				}
				rangenm = smokerange * WUTONM;
				bearing = smokebearing;
				break;
			case 2:
				if(friendrangenm < 0)
				{
					msgnum = AICF_NO_FRIENDS;
				}
				else
				{
					msgnum = AICF_FRIENDS_BEARING;
				}
				rangenm = friendrangenm;
				bearing = friendlybearing;
				break;
			case 3:
				if(threatrange < 0)
				{
					msgnum = AICF_NO_THREATS;
				}
				else
				{
					msgnum = AICF_THREATS_BEARING;
				}
				rangenm = threatrange * WUTONM;
				bearing = threatbearing;
				break;
		}

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		tsptr = tempstr;
		done = 0;
		while(!done)
		{
			numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
			tsptr += strcnt;

			if(spchvar == 1)
			{
				numids += AIRProcessSpeechVars(&sndids[numids], numval, facnum, planenum, bearing, 0, 0, rangenm, 0, 0, 0, voice);
			}
			else if(spchvar == 2)
			{
				if(numval == 1)
				{
					sndids[numids] = SPCH_COUNT_ONE + closestWP + usevoice;
					numids ++;
				}
				if(numval == 3)
				{
					int workelev = targetelev;
					if(targetelev >= 10000)
					{
						sndids[numids] = SPCH_ZERO + (workelev / 10000) + usevoice;
						numids ++;
						workelev %= 10000;
					}

					if(targetelev >= 1000)
					{
						sndids[numids] = SPCH_ZERO + (workelev / 1000) + usevoice;
						numids ++;
						workelev %= 1000;
					}

					if(targetelev >= 100)
					{
						sndids[numids] = SPCH_ZERO + (workelev / 100) + usevoice;
						numids ++;
						workelev %= 100;
					}

					if(targetelev >= 10)
					{
						sndids[numids] = SPCH_ZERO + (workelev / 10) + usevoice;
						numids ++;
						workelev %= 10;
					}

					sndids[numids] = SPCH_ZERO + workelev + usevoice;
					numids ++;

					sndids[numids] = 342 + usevoice;
					numids ++;
				}
				if(numval == 4)
				{
					sndids[numids] = targetsnd + usevoice;
					numids ++;
				}
				if(numval == 8)
				{
					sndids[numids] = threatsnd + usevoice;
					numids ++;
				}
			}
			else
			{
				sndids[numids] = numval + usevoice;
				numids ++;
			}

			if(numids > 15)
			{
				AIRSendSentence(planenum, numids, sndids);
				numids = 0;
			}
		}
	}

	if(numids)
	{
		AIRSendSentence(planenum, numids, sndids);
	}
}

//**************************************************************************************
void AIC_FAC_4Line(int planenum, int facnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[64];
	char bearstr[64];
	char rangestr[64];
	char tempstr3[64];
	void *pTarget;
	int iTargetType;
	FPointDouble position;
	MovingVehicleParams *vehiclepnt = NULL;
	BasicInstance *walker;
	float dx, dz;  // , tdist;
	FPointDouble refpos;
	float targdist;
	float tbearing;
	int bearing360;
	int targetsnd;
	int smoke360 = 0;
	float smokerange = 0;
	int threatsnd;
	int threat360;
	float threatrange;
	void *pThreat;
	int iThreatType;
	FPointDouble threatpos;
	float targbearing = 0;
	float smokebearing = 0;
	float friendlybearing = 0;
	float threatbearing = 0;
	float offangle;

	if(facnum == -2)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
	}
	else
	{
		pTarget = Planes[facnum].AI.pGroundTarget;
		iTargetType = Planes[facnum].AI.lGroundTargetFlag;
	}

	if(!pTarget)
		return;

	if(iTargetType == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)pTarget;
		position = vehiclepnt->WorldPosition;
	}
	else if(iTargetType == GROUNDOBJECT)
	{
		walker = (BasicInstance *)pTarget;
		position = walker->Position;
	}
	else
	{
		return;
	}

	refpos = Planes[planenum].WorldPosition;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr2, 1024, AIC_FOUR_LINE_FOLLOWS, g_iLanguageId))
	{
		return;
	}

	sprintf(tempstr, "%s %s", callsign, tempstr2);

	AICGetClosestThreat(position, Planes[planenum].AI.iSide, 3, &pThreat, &iThreatType);

	if(!pThreat)
	{
		threatrange = -1;
		if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_9A, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(iThreatType == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)pTarget;
			threatpos = vehiclepnt->WorldPosition;
		}
		else if(iTargetType == GROUNDOBJECT)
		{
			walker = (BasicInstance *)pTarget;
			threatpos = walker->Position;
		}
		else
		{
			pThreat = NULL;
		}

		if(pThreat)
		{
			threatsnd = AICGetThreatType(pThreat, iThreatType, tempstr3);
			dx = threatpos.Z - position.X;
			dz = threatpos.Z - position.Z;

			threatbearing = offangle = atan2(-dx, -dz) * 57.2958;

			tbearing = -offangle;
			if(tbearing < 0)
			{
				threat360 = tbearing + 360;
			}
			else
			{
				threat360 = tbearing;
			}
			sprintf(bearstr, "%03d", threat360);

			threatrange = QuickDistance(dx, dz);
			AICGetRangeText(rangestr, threatrange * WUTONM);

			if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_9B, g_iLanguageId, tempstr3, bearstr, rangestr))
			{
				return;
			}
		}
		else
		{
			threatrange = -1;
			if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_9A, g_iLanguageId))
			{
				return;
			}
		}
	}

	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	dx = position.X - refpos.X;
	dz = position.Z - refpos.Z;

	offangle = targbearing = atan2(-dx, -dz) * 57.2958;

	tbearing = -offangle;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_2, g_iLanguageId, bearstr))
	{
		return;
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	targdist = QuickDistance(dx, dz);
	AICGetRangeText(rangestr, targdist * WUTONM);

	if(!LANGGetTransMessage(tempstr2, 256, AIC_NINE_LINE_3, g_iLanguageId, rangestr))
	{
		return;
	}
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	targetsnd = AICGetTargetType(pTarget, iTargetType, tempstr2);
	strcat(tempstr, " ");
	strcat(tempstr, tempstr2);

	AICAddAIRadioMsgs(tempstr, 50);

	AIC_FAC_4LineSnd(planenum, facnum, targbearing, targdist, targetsnd, threatsnd, threatbearing, threatrange);
}

//**************************************************************************************
void AIC_FAC_4LineSnd(int planenum, int facnum, float targbearing, float targdist, int targetsnd, int threatsnd, float threatbearing, float threatrange)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int cnt;
	int msgnum;
	int rangenm;
	int bearing;
	int voice = (facnum == -2) ? SPCH_FAC_GRND : 0;
	int usevoice = (facnum == -2) ? voice : Planes[facnum].AI.lAIVoice;

//	if(lpDD)
//	  lpDD->FlipToGDISurface();

//	SimPause = 1;

	numids = 0;

	for(cnt = 0; cnt < 3; cnt ++)
	{
		switch(cnt)
		{
			case 0:
				msgnum = AICF_START_4_LINE;
				rangenm = targdist * WUTONM;
				bearing = targbearing;
				break;
			case 1:
				if(threatrange < 0)
				{
					msgnum = AICF_NO_THREATS;
				}
				else
				{
					msgnum = AICF_THREATS_BEARING;
				}
				rangenm = threatrange * WUTONM;
				bearing = threatbearing;
				break;
			case 2:
				msgnum = AICF_TARGET_4_LINE;
				rangenm = targdist * WUTONM;
				bearing = targbearing;
				break;
		}

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		tsptr = tempstr;
		done = 0;
		while(!done)
		{
			numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
			tsptr += strcnt;

			if(spchvar == 1)
			{
				numids += AIRProcessSpeechVars(&sndids[numids], numval, facnum, planenum, bearing, 0, 0, rangenm, 0, 0, 0, voice);
			}
			else if(spchvar == 2)
			{
				if(numval == 4)
				{
					sndids[numids] = targetsnd + usevoice;
					numids ++;
				}
				if(numval == 8)
				{
					sndids[numids] = threatsnd + usevoice;
					numids ++;
				}
			}
			else
			{
				sndids[numids] = numval + usevoice;
				numids ++;
			}

			if(numids > 15)
			{
				AIRSendSentence(planenum, numids, sndids);
				numids = 0;
			}
		}
	}

	if(numids)
	{
		AIRSendSentence(planenum, numids, sndids);
	}
}

//**************************************************************************************
void AIC_GenericFACCall(int planenum, int targetnum)
{
	char tempstr[1024];
	int voice = 0;
	int msgnum, msgsnd;
	char callsign[256];
	int fac;
//	int callsignsnd;
	int faccallsign;
	int facnum;
	long facvoice;

//	fac = AICGetClosestFAC(&Planes[planenum]);
	fac = planenum;

	if(fac == -1)
		return;
	else if(fac == -2)
	{
		faccallsign = AIRGetCallSignSndID(FACObject.lCallSign, 0);
		facnum = AIGetFACCallsign(planenum, &facvoice);
		strcpy(callsign, CallSignList[facnum].sName);
		voice = facvoice;
	}
	else
	{
		AIC_Get_Callsign_With_Number(fac, callsign);
		faccallsign = fac;
	}

	switch(targetnum)
	{
		case 1:
			msgnum = AIC_FAC_LASER_ON;
			msgsnd = AICF_FAC_LASER_ON;
			break;
		case 2:
			msgnum = AIC_FAC_ILLUMINATED;
			msgsnd = AICF_FAC_ILLUMINATED;
			break;
		case 3:
			msgnum = AIC_FAC_SMOKE_PLACED;
			msgsnd = AICF_FAC_SMOKE_PLACED;
			break;
		default:
			msgnum = AIC_FAC_RELEASE_SMOKE;
			msgsnd = AICF_FAC_RELEASE_SMOKE;
			break;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, faccallsign, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AICReportFACTarget(int facnum, int planenum, int reporttype, int usecopy)
{
	void *pTarget;
	int iTargetType;
	int delay1 = 4000;
	int delay2 = 10000;

	if(facnum == -2)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
	}
	else
	{
		pTarget = Planes[facnum].AI.pGroundTarget;
		iTargetType = Planes[facnum].AI.lGroundTargetFlag;
	}

	if(pTarget)
	{
		iFACHitFlags |= FAC_HAS_TARGET;
	}

	if(usecopy)
	{
		if(!pTarget)
		{
			AICAddSoundCall(AIC_FACAck, planenum, delay1, 50, 4);
		}
		else if(reporttype)  //  9 line brief
		{
			AICAddSoundCall(AIC_FACAck, planenum, delay1, 50, 5);
		}
		else	//  4 line brief
		{
			AICAddSoundCall(AIC_FACAck, planenum, delay1, 50, 6);
		}
	}

	if(!pTarget)
	{
		if(facnum == -2)
		{
			if((iFACHitFlags & FAC_HAS_TARGET) && (facnum == -2))
			{
				AICAddSoundCall(AIC_FACAck, planenum, delay2, 50, 11 + (rand() % 3));
			}
			else
			{
				AICAddSoundCall(AIC_FACAck, planenum, delay2, 50, 15);
			}
		}
		else
		{
			return;
		}
	}
	else if(reporttype)
	{
		AICAddSoundCall(AIC_FAC_9Line, planenum, delay2, 50, facnum);

		if(facnum == -2)
		{
			if(iFACHitFlags & FAC_LASER)
			{
		 		AICAddSoundCall(AIC_GenericFACCall, -2, delay2 + 20000 + ((rand() & 0x3) * 1000), 50, (1 + (rand() & 0x1)));
			}
		}
	}
	else
	{
		AICAddSoundCall(AIC_FAC_4Line, planenum, delay2, 50, facnum);

		if(facnum == -2)
		{
			if(iFACHitFlags & FAC_LASER)
			{
		 		AICAddSoundCall(AIC_GenericFACCall, -2, delay2 + 10000 + ((rand() & 0x3) * 1000), 50, (1 + (rand() & 0x1)));
			}
		}
	}
}

//**************************************************************************************
void AICPutSmokeOnFACTarget(int facnum)
{
	FPoint vel(0.0f);
	DWORD hCanister;
	FPointDouble position;
	void *pTarget;
	int iTargetType;
	MovingVehicleParams *vehiclepnt = NULL;
	BasicInstance *walker;

	if(facnum == -2)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
	}
	else
	{
		pTarget = Planes[facnum].AI.pGroundTarget;
		iTargetType = Planes[facnum].AI.lGroundTargetFlag;
	}

	if(!pTarget)
		return;

	if(iTargetType == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)pTarget;
		position = vehiclepnt->WorldPosition;
	}
	else if(iTargetType == GROUNDOBJECT)
	{
		walker = (BasicInstance *)pTarget;
		position = walker->Position;
	}
	else
	{
		return;
	}

//	hCanister = NewCanister( CT_STEAM_SMOKE, position, vel, 0.0f, 300);
	int smokeid = (LastPlane - Planes) % 5;

	switch(smokeid)
	{
		case 1:
			hCanister = NewCanister( CT_GREEN_MARKER_SMOKE, position, vel, 0.0f, 300);
			break;
		case 2:
			hCanister = NewCanister( CT_RED_MARKER_SMOKE, position, vel, 0.0f, 300);
			break;
		case 3:
			hCanister = NewCanister( CT_BLUE_MARKER_SMOKE, position, vel, 0.0f, 300);
			break;
		case 4:
			hCanister = NewCanister( CT_YELLOW_MARKER_SMOKE, position, vel, 0.0f, 300);
			break;
		default:
			hCanister = NewCanister( CT_ORANGE_MARKER_SMOKE, position, vel, 0.0f, 300);
			break;
	}
}

//**************************************************************************************
void AIC_FAC_Miss(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[64];
	char tempstr3[64];
	int voice = 0;
	int msgnum, msgsnd;
	char callsign[256];
	int fac;
	int distft;

	if(dFACRange >= 10000.0f)
	{
		iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT|FAC_HIT_DAMAGE|FAC_HIT_DESTROYED|FAC_HIT_FRIEND|FAC_LASER_ON);
		dFACRange = 0;
		return;
	}

	fac = AICGetClosestFAC(&Planes[planenum]);

	if(fac == -1)
		return;
	else if(fac == -2)
	{
		voice = SPCH_FAC_GRND;
	}

	if(iFACHitFlags & FAC_HIT_SHORT)
	{
		msgnum = AIC_MISS_SHORT;
		msgsnd = AICF_MISS_SHORT;
	}
	else if(iFACHitFlags & FAC_HIT_LONG)
	{
		msgnum = AIC_MISS_LONG;
		msgsnd = AICF_MISS_LONG;
	}
	else if(iFACHitFlags & FAC_HIT_LEFT)
	{
		msgnum = AIC_MISS_LEFT;
		msgsnd = AICF_MISS_LEFT;
	}
	else
	{
		msgnum = AIC_MISS_RIGHT;
		msgsnd = AICF_MISS_RIGHT;
	}

	distft = dFACRange;
	sprintf(tempstr3, "%d", distft);
	if(!LANGGetTransMessage(tempstr2, 64, AIC_DIST_FEET, g_iLanguageId, tempstr3))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIC_FAC_MissSnd(planenum, fac, voice, msgsnd, distft);

	iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT|FAC_HIT_DAMAGE|FAC_HIT_DESTROYED|FAC_HIT_FRIEND|FAC_LASER_ON);
	dFACRange = 0;
}


//**************************************************************************************
void AIC_FAC_MissSnd(int planenum, int facnum, int voice, int msgsnd, int distft)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int usevoice = (facnum == -2) ? voice : Planes[facnum].AI.lAIVoice;

//	if(lpDD)
//	  lpDD->FlipToGDISurface();

//	SimPause = 1;

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, msgsnd, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	done = 0;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
		}
		else if(spchvar == 2)
		{
			if(numval == 1)
			{
				int workdist = distft;
				if(distft >= 10000)
				{
					sndids[numids] = SPCH_ZERO + (workdist / 10000) + usevoice;
					numids ++;
					workdist %= 10000;
				}

				if(distft >= 1000)
				{
					sndids[numids] = SPCH_ZERO + (workdist / 1000) + usevoice;
					numids ++;
					workdist %= 1000;
				}

				if(distft >= 100)
				{
					sndids[numids] = SPCH_ZERO + (workdist / 100) + usevoice;
					numids ++;
					workdist %= 100;
				}

				if(distft >= 10)
				{
					sndids[numids] = SPCH_ZERO + (workdist / 10) + usevoice;
					numids ++;
					workdist %= 10;
				}

				sndids[numids] = SPCH_ZERO + workdist + usevoice;
				numids ++;

				sndids[numids] = 342 + usevoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + usevoice;
			numids ++;
		}
	}

	if(numids)
	{
		AIRSendSentence(planenum, numids, sndids);
	}
}

//**************************************************************************************
void AICGetFACWeather(int facnum, char *tempstr, int *sndids, int *numids)
{
}

//**************************************************************************************
int AICGetTargetType(void *pTarget, int iTargetType, char *tempstr)
{
	DBWeaponType *weapon;
	MovingVehicleParams *vehiclepnt = NULL;
	BasicInstance *walker;
	DBVehicleType *pvehicletype=NULL;
	int cnt;
	int gun, sam, numw;

	if(iTargetType == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)pTarget;

		if(!vehiclepnt->iShipType)
		{
			pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];
			gun = sam = numw = 0;
			for(cnt = 0; cnt < 2; cnt ++)
			{
				if(!cnt)
				{
					weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
				}
				else
				{
					weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
				}
				if(weapon)
				{
					numw ++;
					if((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11))
					{
						gun = 1;
					}
					else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
					{
						sam = 1;
					}
				}
			}
			if(numw == 1)
			{
				if(sam)
				{
					if(!LANGGetTransMessage(tempstr, 128, AIC_MOBILE_SAM_SITE, g_iLanguageId))
					{
						return(-1);
					}
					return(356);
				}
				else if(gun)
				{
					if(!LANGGetTransMessage(tempstr, 128, AIC_MOBILE_AAA_SITE, g_iLanguageId))
					{
						return(-1);
					}
					return(354);
				}
			}
			if(pvehicletype->lVehicleType == VEHICLE_TYPE_TANK)
			{
				if(!LANGGetTransMessage(tempstr, 128, AIC_TANKS, g_iLanguageId))
				{
					return(-1);
				}
				return(348);
			}
			else if((pvehicletype->lVehicleType == VEHICLE_TYPE_TRUCK) && (pvehicletype->iArmorType == ARMOR_TYPE_ARMORED))
			{
				if(numw == 2)
				{
					if(!LANGGetTransMessage(tempstr, 128, AIC_ARMOR, g_iLanguageId))
					{
						return(-1);
					}
					return(350);
				}
				else
				{
					if(!LANGGetTransMessage(tempstr, 128, AIC_APCS, g_iLanguageId))
					{
						return(-1);
					}
					return(349);
				}
			}
			if(!LANGGetTransMessage(tempstr, 128, AIC_TRUCKS, g_iLanguageId))
			{
				return(-1);
			}
			return(351);
		}
		else
		{
			gun = sam = 0;
			for(cnt = 0; cnt < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; cnt ++)
			{
				if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_RADAR)
				{
					continue;
				}
				weapon = get_weapon_ptr((long)pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID);
				if(weapon)
				{
					if((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11))
					{
						gun = 1;
					}
					else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
					{
						sam = 1;
					}
				}
			}
			if(sam)
			{
				if(!LANGGetTransMessage(tempstr, 128, AIC_MOBILE_SAM_SITE, g_iLanguageId))
				{
					return(-1);
				}
				return(356);
			}
			else if(gun)
			{
				if(!LANGGetTransMessage(tempstr, 128, AIC_MOBILE_AAA_SITE, g_iLanguageId))
				{
					return(-1);
				}
				return(354);
			}
			else
			{
				if(!LANGGetTransMessage(tempstr, 128, AIC_TRUCKS, g_iLanguageId))
				{
					return(-1);
				}
				return(351);
			}
		}
		return(-1);
	}

	walker = (BasicInstance *)pTarget;
	weapon = GetWeaponPtr((BasicInstance *)pTarget);

	if(weapon)
	{
		if((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11) && (weapon->iSeekerType != 9))
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_AAA_SITE, g_iLanguageId))
			{
				return(-1);
			}
			return(353);
		}
		else if(weapon->iWeaponType == WEAPON_TYPE_GUN)
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_ARTILLERY, g_iLanguageId))
			{
				return(-1);
			}
			return(352);
		}
		else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_SAM_SITE, g_iLanguageId))
			{
				return(-1);
			}
			return(355);
		}
	}

	if(!LANGGetTransMessage(tempstr, 128, AIC_SUPPLY_DUMP, g_iLanguageId))
	{
		return(-1);
	}
	return(358);
}

//**************************************************************************************
int AICGetThreatType(void *pTarget, int iTargetType, char *tempstr)
{
	DBWeaponType *weapon;
	DBWeaponType *maxweapon = NULL;
	MovingVehicleParams *vehiclepnt = NULL;
	float maxrange = 0;
	int maxcnt, cnt;
	BasicInstance *walker;

	if(!pTarget)
		return(-1);

	if(iTargetType == GROUNDOBJECT)
	{
		walker = (BasicInstance *)pTarget;
		maxweapon = GetWeaponPtr((BasicInstance *)pTarget);
	}
	else if(iTargetType == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)pTarget;

		if (!vehiclepnt->iShipType)
			maxcnt = 2;
		else
			maxcnt = pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems;

		weapon=NULL;
		for(cnt = 0; cnt < maxcnt; cnt ++)
		{
			if (!vehiclepnt->iShipType)
			{
				if(!cnt)
				{
					weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
				}
				else
				{
					weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
				}
			}
			else
			{
				if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_RADAR)
				{
					continue;
				}
				weapon = get_weapon_ptr((long)pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID);
			}
			if(weapon)
			{
				if(((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11)) || (weapon->iWeaponType == WEAPON_TYPE_SAM))
				{
					if(maxrange <= weapon->iRange)
					{
						maxweapon = weapon;
						maxrange = weapon->iRange;
					}
				}
			}
		}

	}
	else
	{
		return(-1);
	}

	if(maxweapon->iWeaponType == WEAPON_TYPE_GUN)
	{
		if(!LANGGetTransMessage(tempstr, 128, AIC_AAA_BEARING, g_iLanguageId))
		{
			return(-1);
		}
		return(359);
	}
	else if(vehiclepnt)
	{
		if(!vehiclepnt->iShipType)
		{
			if(pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID == 56)
			{
				if(!LANGGetTransMessage(tempstr, 128, AIC_MANPADS_BEARING, g_iLanguageId))
				{
					return(-1);
				}
				return(360);
			}
		}
		if((maxweapon->iSeekerType == 2) || (maxweapon->iSeekerType == 3))
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_MOBLE_IR_BEARING, g_iLanguageId))
			{
				return(-1);
			}
			return(361);
		}
		else
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_MOBILE_RADAR_BEARING, g_iLanguageId))
			{
				return(-1);
			}
			return(362);
		}
	}
	else
	{
		if((maxweapon->iSeekerType == 2) || (maxweapon->iSeekerType == 3))
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_MOBLE_IR_BEARING, g_iLanguageId))
			{
				return(-1);
			}
			return(361);
		}
		else
		{
			if(!LANGGetTransMessage(tempstr, 128, AIC_RADAR_SAM_BEARING, g_iLanguageId))
			{
				return(-1);
			}
			return(363);
		}
	}
	return(-1);
}

//**************************************************************************************
void AICGetClosestThreat(FPointDouble position, int iSide, float maxrange, void **pTarget, int *iTargetType)
{
	AAWeaponInstance *currweapon;
	DBWeaponType *weapon;
	float dx, dz, tdist;
	void *threat = NULL;
	int threattype = -1;
	float founddist = maxrange;
	MovingVehicleParams *vehiclepnt;
	int maxcnt, cnt;

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		if(!((currweapon->Basics.Flags & BI_NOT_ACTIVE) || !InstanceIsBombable((BasicInstance *)currweapon) || (GDConvertGrndSide((BasicInstance *)currweapon) == iSide)))
		{
			dx = position.X - currweapon->Basics.Position.X;
			dz = position.Z - currweapon->Basics.Position.Z;

			tdist = QuickDistance(dx, dz);

			if((tdist < founddist) && tdist)
			{
				if ((DWORD)currweapon->Type->WeaponPtr < 0x400)
				{
					weapon = get_weapon_ptr((long)currweapon->Type->WeaponPtr);
				}
				else
				{
					weapon = (DBWeaponType *)currweapon->Type->WeaponPtr;
				}
				if(weapon)
				{
					if(((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11)) || (weapon->iWeaponType == WEAPON_TYPE_SAM))
					{
						if(maxrange <= weapon->iRange)
						{
							threat = (BasicInstance *)currweapon;
							threattype = GROUNDOBJECT;
							maxrange = weapon->iRange;
							founddist = tdist;
						}
					}
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))) && (!(vehiclepnt->Status & (VL_INVISIBLE))) && (iSide != vehiclepnt->iSide))
		{
			dx = position.X - vehiclepnt->WorldPosition.X;
			dz = position.Z - vehiclepnt->WorldPosition.Z;

			tdist = QuickDistance(dx, dz);

			if((tdist < founddist) && tdist)
			{
				if (!vehiclepnt->iShipType)	// SCOTT FIX
					maxcnt = 2;
				else
					maxcnt = pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems;

				weapon=NULL;
				for(cnt = 0; cnt < maxcnt; cnt ++)
				{
					if (!vehiclepnt->iShipType)	// SCOTT FIX
					{
						if(!cnt)
						{
							weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
						}
						else
						{
							weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
						}
					}
					else
					{
						if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_RADAR)
						{
							continue;
						}
						weapon = get_weapon_ptr((long)pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID);
					}
					if(weapon)
					{
						if(((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11)) || (weapon->iWeaponType == WEAPON_TYPE_SAM))
						{
							if(maxrange <= weapon->iRange)
							{
								threat = vehiclepnt;
								threattype = MOVINGVEHICLE;
								maxrange = weapon->iRange;
								founddist = tdist;
							}
						}
					}
				}
			}
		}
	}

	*pTarget = threat;
	*iTargetType = threattype;
}

//**************************************************************************************
int AICGetClosestWP(FPointDouble position, int planenum)
{
	MBWayPoints *lookway;
	int waycnt;
	int numwpts;
	PlaneParams *planepnt = &Planes[planenum];
	float dx, dz, tdist;
	float founddist = -1;
	int returnval = 0;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		dx = (lookway->lWPx * FTTOWU) - position.X;
		dz = (lookway->lWPz * FTTOWU) - position.Z;
		tdist = QuickDistance(dx, dz);

		if((tdist < founddist) || (founddist < 0))
		{
			returnval = waycnt;
			founddist = tdist;
		}

		lookway ++;
	}

	return(returnval);
}

//**************************************************************************************
void AIC_GenericMsgPlane(int planenum, int targetnum)
{
	char tempstr[1024];
	int voice = 0;
	int msgnum, msgsnd;
	char callsign[256];
	int placeingroup;
	PlaneParams *leadplane, *playerlead;
	int usecallsign = 0;
	int msgtype = 1;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	playerlead = AIGetLeader(PlayerPlane);
	leadplane = AIGetLeader(&Planes[planenum]);

	switch(targetnum)
	{
		case 1:
			msgnum = AIC_WINCHESTER_2;
			if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
			{
				msgnum = AIC_WINCHESTER_RUSF_2;
			}

			msgsnd = AICF_WINCHESTER;
			break;
		case 2:
			msgnum = AI_BINGO_FUEL2;
			msgsnd = AICF_BINGO_FUEL2;
			break;
		case 3:
			msgnum = AIC_FLIGHT_RTB;
			msgsnd = AICF_FLIGHT_RTB;
			break;
		case 4:
			msgsnd = AICF_WING_HIT2;
			if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
			{
				msgnum = AIC_WING_WE_HIT_RUSA;
			}
			else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
			{
				msgnum = AIC_WING_WE_HIT_RUSF;
			}
			else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
			{
				msgnum = AIC_WING_WE_HIT_RUSA;
			}
			else
			{
				msgnum = AIC_WING_HIT;
			}
			break;
		case 5:
			msgnum = AIC_STANDBY_FOR_NINE;
			msgsnd = AICF_STANDBY_FOR_NINE;
			break;
		case 6:
			msgnum = AIC_STANDBY_FOR_FOUR;
			msgsnd = AICF_STANDBY_FOR_FOUR;
			break;
		case 7:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_4;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_4;
			break;
		case 8:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_2;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_2;
			break;
		case 9:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_5;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_5;
			break;
		case 10:
			msgnum = AIC_GROUND_FRIENDLY_FIRE_US_6;
			msgsnd = AICF_GROUND_FRIENDLY_FIRE_US_6;
			break;
		case 11:
			msgnum = AIC_GROUND_GOOD_US_4;
			msgsnd = AICF_GROUND_GOOD_US_4;
			break;
		case 12:
			msgnum = AIC_GROUND_GOOD_US_5;
			msgsnd = AICF_GROUND_GOOD_US_5;
			break;
		case 13:
			msgnum = AIC_GROUND_GOOD_US_3;
			msgsnd = AICF_GROUND_GOOD_US_3;
			break;
		case 14:
			msgnum = AIC_FAC_TARGET_CLEAR;
			msgsnd = AICF_FAC_TARGET_CLEAR;
			break;
		case 15:
			msgnum = AIC_COPY_RTB;
			msgsnd = AICF_COPY_RTB;
			break;
		case 16:
			msgnum = AIC_HIT;
			msgsnd = AICF_HIT;
			break;
		case 17:
			msgnum = AIC_TARGET_DESTROYED;
			msgsnd = AICF_TARGET_DESTROYED;
			break;
		case 18:
			msgnum = AIC_TARGET_DAMAGED;
			msgsnd = AICF_TARGET_DAMAGED;
			break;
		case 19:
			msgnum = AIC_ENGAGE_MSG;
			msgsnd = AICF_ENGAGE_MSG;
			break;
		case 20:
			msgnum = AIC_HELP_ME_NEG_2_A;
			msgsnd = AICF_HELP_ME_NEG_2_A;
			break;
		case 21:
			msgnum = AIC_WING_TARGET_DESTROYED_REFORMING;
			msgsnd = AICF_WING_TARGET_DESTROYED_REFORMING;
			break;
		case 22:
			msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT;
			if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
			{
				msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT_RAF;
			}
			else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
			{
				msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT_RUSA;
			}
			msgsnd = AICF_WING_TARGETS_SIGHTED_IN_HOT;
			break;
		case 23:
			msgnum = AIC_BANDIT_BANDIT_2;
			msgsnd = AICF_BANDIT_BANDIT;
			break;
		case 24:
			msgnum = AIC_COMM_SAM_LAUNCH;
			msgsnd = AICF_COMM_SAM_LAUNCH;
			break;
		case 25:
			msgnum = AIC_COMM_MISSILE_LAUNCH;
			msgsnd = AICF_COMM_MISSILE_LAUNCH;
			break;
		case 26:
			msgnum = AIC_WING_AAA_SPOT_4;
			msgsnd = AICF_WING_NUM_AAA_SPOT_4;
			break;
		case 27:
			msgnum = AIC_COMM_SPIKE_SPIKE;
			msgsnd = AICF_COMM_SPIKE_SPIKE;
			break;
		case 28:
			msgnum = AIC_ROGER;
			msgsnd = AICF_ROGER;
			break;
		case 29:
			msgnum = AIC_BASIC_NEG_1;
			msgsnd = AICF_BASIC_NEG_1;
			break;
		case 30:
			msgnum = AIC_WING_LASER_ON;
			msgsnd = AICF_WING_LASER_ON;
			usecallsign = 1;
			break;
		case 31:
			msgnum = AIC_WING_LASER_OFF;
			msgsnd = AICF_WING_LASER_OFF;
			usecallsign = 1;
			break;
		case 32:
			msgnum = AIC_ON_STATION;
			msgsnd = AICF_ON_STATION;
			usecallsign = 1;
			break;
		case 33:
			msgnum = AIC_REQUEST_RELIEF;
			msgsnd = AICF_REQUEST_RELIEF;
			usecallsign = 1;
			break;
		case 34:
			voice = SPCH_STRIKE;
			msgnum = AIC_STRIKE_DIVERT;
			msgsnd = AICF_STRIKE_DIVERT;
			usecallsign = 1;
			break;
		case 35:
			voice = AICGetTowerVoice(planenum);
			msgnum = AIC_ARRIVAL_EMERGENCY;
			msgsnd = AICF_ARRIVAL_EMERGENCY;
			usecallsign = 1;
			break;
		case 36:
			voice = SPCH_STRIKE;
			msgnum = AIC_STRIKE_SWEET_OUT;
			msgsnd = AICF_STRIKE_SWEET_OUT;
			usecallsign = 1;
			msgtype = 76;
			break;
		case 37:
			msgnum = AIC_AIRBORNE;
			msgsnd = AICF_AIRBORNE;
			usecallsign = 1;
			msgtype = 77;
			break;
		case 38:
			msgnum = AIC_PASSING_2_5;
			msgsnd = AICF_PASSING_2_5;
			usecallsign = 1;
			msgtype = 77;
			break;
		case 39:
			msgnum = AIC_ARCING;
			msgsnd = AICF_ARCING;
			usecallsign = 1;
			msgtype = 77;
			break;
		case 40:
			msgnum = AIC_OUTBOUND;
			msgsnd = AICF_OUTBOUND;
			usecallsign = 1;
			msgtype = 77;
			break;
		case 41:
			msgnum = AIC_UP_FOR_CHECKS;
			msgsnd = AICF_UP_FOR_CHECKS;
			usecallsign = 1;
			msgtype = 76;
			break;
		case 42:
			msgnum = AIC_WING_DUCKS_AWAY;
			msgsnd = AICF_WING_DUCKS_AWAY;
			break;
		default:
			msgnum = AIC_AWACS_COPY;
			msgsnd = AICF_FAC_COPY;
			break;
	}

	if(!AICAllowThisRadio(planenum, msgtype))
	{
		return;
	}

	if((leadplane == playerlead) && (!usecallsign))
	{
		placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

		if(!LANGGetTransMessage(callsign, 256, AIC_ZERO + placeingroup, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		AIC_Get_Callsign_With_Number(planenum, callsign);
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, planenum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AICDoCommSort(PlaneParams *planepnt)
{
	int cnt;
	int targetcnt = -1;
	int planenum;
	int currid;
	double centerx, centerz;
	float centerdist[AA_MAX_DET_PLANES * 2];
	float radardist[AA_MAX_DET_PLANES * 2];
	int headingid[AA_MAX_DET_PLANES * 2];
	float heading[AA_MAX_DET_PLANES * 2];
	float closest;
	int closestid = -1;
	float furthest = 0.0;
	int furthestid = -1;
	float dx, dz, tdist, theading;
	int headingcnt[8];
	float furthestheading[8];
	int furthestheadingid[8];
	int midnumber = 1;
	int middlecnt = 0;
	int cnt2;
	int maxnum, checkloop;

	planenum = planepnt - Planes;

	iAISortNumber = 0;

	centerx = centerz = 0;

	if(!(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			currid = CurFramePlanes.Planes[cnt].PlaneId;
			if(Planes[planenum].AADesignate)
			{
				if((Planes[planenum].AADesignate - Planes) == currid)
				{
					targetcnt = cnt;
				}
			}
			centerx += Planes[currid].WorldPosition.X;
			centerz += Planes[currid].WorldPosition.Z;
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
				if(Planes[planenum].AADesignate)
				{
					if((Planes[planenum].AADesignate - Planes) == currid)
					{
						targetcnt = cnt + AA_MAX_DET_PLANES;
					}
				}
				centerx += Planes[currid].WorldPosition.X;
				centerz += Planes[currid].WorldPosition.Z;
			}
		}
	}

	if(targetcnt == -1)
	{
		return;
	}


	if( GetCurrentAARadarMode() == AA_TWS_MODE )
	{
		centerx /= (CurFramePlanes.NumPlanes + AgePlanes[0].NumPlanes);
		centerz /= (CurFramePlanes.NumPlanes + AgePlanes[0].NumPlanes);
	}
	else
	{
		centerx /= CurFramePlanes.NumPlanes;
		centerz /= CurFramePlanes.NumPlanes;
	}

	for(cnt = 0; cnt < 8; cnt ++)
	{
		headingcnt[cnt] = 0;
		furthestheading[cnt] = -1;
		furthestheadingid[cnt] = -1;
	}

	if( GetCurrentAARadarMode() == AA_TWS_MODE )
	{
		checkloop = 2;
	}
	else
	{
		checkloop = 1;
	}

	for(cnt2 = 0; cnt2 < checkloop; cnt2 ++)
	{
		if(cnt2)
		{
			maxnum = AgePlanes[0].NumPlanes;
		}
		else
		{
			maxnum = CurFramePlanes.NumPlanes;
		}

		for(cnt = 0; cnt < maxnum; cnt ++)
		{
			if(cnt2)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
			}
			else
			{
				currid = CurFramePlanes.Planes[cnt].PlaneId;
			}
			dx = Planes[currid].WorldPosition.X - centerx;
			dz = Planes[currid].WorldPosition.Z - centerz;
			tdist = QuickDistance(dx, dz);
			theading = -atan2(-dx, -dz) * 57.2958;

			theading = AICapAngle(theading, 0, 360);

			if(cnt2)
			{
				centerdist[cnt + AA_MAX_DET_PLANES] = tdist;
				heading[cnt + AA_MAX_DET_PLANES] = theading;
				headingid[cnt + AA_MAX_DET_PLANES] = (theading + 22.5) / 45;
				if(headingid[cnt + AA_MAX_DET_PLANES] > 7)
				{
					headingid[cnt + AA_MAX_DET_PLANES] = 0;
				}

				if(furthestheading[headingid[cnt + AA_MAX_DET_PLANES]] < tdist)
				{
					furthestheading[headingid[cnt + AA_MAX_DET_PLANES]] = tdist;
					furthestheadingid[headingid[cnt + AA_MAX_DET_PLANES]] = cnt;
				}

				headingcnt[headingid[cnt + AA_MAX_DET_PLANES]] = headingcnt[headingid[cnt + AA_MAX_DET_PLANES]] + 1;
			}
			else
			{
				centerdist[cnt] = tdist;
				heading[cnt] = theading;
				headingid[cnt] = (theading + 22.5) / 45;
				if(headingid[cnt] > 7)
				{
					headingid[cnt] = 0;
				}

				if(furthestheading[headingid[cnt]] < tdist)
				{
					furthestheading[headingid[cnt]] = tdist;
					furthestheadingid[headingid[cnt]] = cnt;
				}

				headingcnt[headingid[cnt]] = headingcnt[headingid[cnt]] + 1;
			}

			dx = Planes[currid].WorldPosition.X - PlayerPlane->WorldPosition.X;
			dz = Planes[currid].WorldPosition.Z - PlayerPlane->WorldPosition.Z;

			tdist = QuickDistance(dx, dz);

			if((closestid == -1) || (closest > tdist))
			{
				closest = tdist;
				if(cnt2)
				{
					closestid = cnt + AA_MAX_DET_PLANES;
				}
				else
				{
					closestid = cnt;
				}
			}
			if((furthestid == -1) || (furthest < tdist))
			{
				furthest = tdist;
				if(cnt2)
				{
					furthestid = cnt + AA_MAX_DET_PLANES;
				}
				else
				{
					furthestid = cnt;
				}
			}
			if(cnt2)
			{
				radardist[cnt + AA_MAX_DET_PLANES] = tdist;
			}
			else
			{
				radardist[cnt] = tdist;
			}
		}
	}

	if(closestid == targetcnt)
	{
		AICDoSortMsg(planenum, 10, midnumber);
		if((MultiPlayer) && (planepnt == PlayerPlane))
		{
 			NetPutGenericMessage3(planepnt, GM3_SORT_MSG, 10, midnumber);
		}
	}
	else if(furthestid == targetcnt)
	{
		AICDoSortMsg(planenum, 12, midnumber);
		if((MultiPlayer) && (planepnt == PlayerPlane))
		{
 			NetPutGenericMessage3(planepnt, GM3_SORT_MSG, 12, midnumber);
		}
	}
	else if((headingcnt[headingid[targetcnt]] == 1) || (furthestheadingid[headingid[targetcnt]] == targetcnt))
	{
		AICDoSortMsg(planenum, headingid[targetcnt], midnumber);
		if((MultiPlayer) && (planepnt == PlayerPlane))
		{
 			NetPutGenericMessage3(planepnt, GM3_SORT_MSG, headingid[targetcnt], midnumber);
		}
	}
	else
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			if((headingcnt[headingid[targetcnt]] != 1) && (furthestheadingid[headingid[cnt]] != cnt) && (cnt != targetcnt))
			{
				middlecnt++;
				if(radardist[cnt] < radardist[targetcnt])
				{
					midnumber ++;
				}
			}
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				if((headingcnt[headingid[targetcnt]] != 1) && (furthestheadingid[headingid[cnt + AA_MAX_DET_PLANES]] != (cnt + AA_MAX_DET_PLANES)) && ((cnt + AA_MAX_DET_PLANES) != targetcnt))
				{
					middlecnt++;
					if(radardist[cnt + AA_MAX_DET_PLANES] < radardist[targetcnt])
					{
						midnumber ++;
					}
				}
			}
		}

		if(!middlecnt)
		{
			midnumber = 0;
		}

		AICDoSortMsg(planenum, 11, midnumber);
		if((MultiPlayer) && (planepnt == PlayerPlane))
		{
 			NetPutGenericMessage3(planepnt, GM3_SORT_MSG, 11, midnumber);
		}
	}
}

//**************************************************************************************
void AIC_GenericMsgPlaneBearing(int planenum, int targetnum, float bearing)
{
	char tempstr[1024];
	int voice = 0;
	int msgnum, msgsnd;
	char callsign[256];
	int placeingroup;
	PlaneParams *leadplane, *playerlead;
	char bearstr[128];
	int bearing360;
	float tbearing;
	int justcallsign = 0;

#if 0
	if(voicenum == SPCH_MARSHAL)
	{
		voice = 2;
	}
	else if(voicenum == SPCH_FINAL)
	{
		voice = 3;
	}
	else if(voicenum == SPCH_LSO)
	{
		voice = 4;
	}
#endif

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	playerlead = AIGetLeader(PlayerPlane);
	leadplane = AIGetLeader(&Planes[planenum]);

	switch(targetnum)
	{
		case 1:
			msgnum = AIC_WING_SHIP_SPOT_1;
			msgsnd = AICF_WING_SHIP_SPOT_1A;
			break;
		case 2:
			msgnum = AIC_WING_STROBE_DIR;
			msgsnd = AICF_WING_STROBE_DIRA;
		case 3:
			voice = SPCH_FINAL;  //  Final voice
			if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
			{
				return;
			}
			justcallsign = 1;
			msgnum = AIC_FINAL_BOLTER1;
			msgsnd = AICF_FINAL_BOLTER1;
			break;
		case 4:
			voice = SPCH_FINAL;  //  Final voice
			if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
			{
				return;
			}
			justcallsign = 1;
			msgnum = AIC_FINAL_BOLTER2;
			msgsnd = AICF_FINAL_BOLTER2;
			break;
		case 5:
			voice = SPCH_FINAL;  //  Final voice
			if(!(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING))
			{
				return;
			}
			justcallsign = 1;
			msgnum = AIC_NEW_FINAL_BEARING;
			msgsnd = AICF_NEW_FINAL_BEARING;
			break;
		default:
			msgnum = AIC_WING_GRND_TRG_SPOT_1;
			msgsnd = AICF_WING_GRND_TRG_SPOT_1A;
			break;
	}

	if((leadplane == playerlead) && (!justcallsign))
	{
		placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

		if(!LANGGetTransMessage(callsign, 256, AIC_ZERO + placeingroup, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		AIC_Get_Callsign_With_Number(planenum, callsign);
	}

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, bearstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, planenum, planenum, bearing, 0, 0, 0, 0, 0, 0, voice);
}

//**************************************************************************************
void AICDoCommTargetSpot()
{
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	FPointDouble position;
	float dx, dz, offangle;
	int msgnum;

	if(!Av.AGRadar.Target)
		return;

	if(Av.AGRadar.TargetType == AG_REGISTERED_OBJECT)
	{
		walker = (BasicInstance *)Av.AGRadar.Target;
		position = walker->Position;
		msgnum = 0;
	}
	else
	{
		vehiclepnt = (MovingVehicleParams *)Av.AGRadar.Target;
		position = vehiclepnt->WorldPosition;
		if(vehiclepnt->iShipType)
		{
			msgnum = 1;
		}
		else
		{
			msgnum = 0;
		}
	}

	dx = position.X - PlayerPlane->WorldPosition.X;
	dz = position.Z - PlayerPlane->WorldPosition.Z;

	offangle = atan2(-dx, -dz) * 57.2958;

	if(MultiPlayer)
	{
		NetPutGenericMessage2Float(PlayerPlane, msgnum, offangle);
	}
}

//**************************************************************************************
void AICDoCommStrobeSpot()
{
	PlaneParams *checkplane;
	float dx, dy, dz, tdist, offangle, offpitch;
	float toffangle, toffpitch;
	float jammingrange = 100.0f * WUTOFT;
	float minrange = jammingrange;
	int widthok;
	int heightok;
	ANGLE wangle;
	float wheading;
	long sojmask = (AI_SO_JAMMING);  //  (AI_SO_JAMMING|AI_SOJ_ON_BOARD);
	PlaneParams *planepnt = PlayerPlane;
	float radarangleyaw = AIConvertAngleTo180Degree(planepnt->Heading);
	float radaranglepitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	float widthangle = 60.0f;
	float heightangle = 60.0f;
	float finalangle = radarangleyaw;



	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if((checkplane->AI.iAIFlags2 & sojmask) == sojmask)
		{
			if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (checkplane->OnGround == 0))
			{
				widthok = 0;
				heightok = 0;
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
	//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
				if((fabs(dx) < minrange) && (fabs(dz) < minrange))
				{
					tdist = QuickDistance(dx, dz);
	//				if(tdist < nlfoundrange)
					if(tdist < minrange)
					{
						offangle = atan2(-dx, -dz) * 57.2958;
	//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
						toffangle = offangle - radarangleyaw;
						offangle = AICapAngle(offangle);

						wangle = AIConvert180DegreeToAngle(offangle) + 0x8000;
						wangle -= checkplane->Heading;
						wheading = AIConvertAngleTo180Degree(wangle);

						toffangle = AICapAngle(toffangle);

						if((fabs(toffangle) < widthangle) && (fabs(wheading) < 30.0f))
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

						if(widthok && heightok)
						{
							finalangle = offangle;
							minrange = tdist;
						}
					}
				}
			}
		}
	}

	if(MultiPlayer)
	{
		NetPutGenericMessage2Float(PlayerPlane, 2, finalangle);
	}
}

//**************************************************************************************
void AIC_BolterStart(int planenum, int targetnum)
{
	double deckangle = DECK_ANGLE;
	PlaneParams *planepnt = &Planes[planenum];

	if((planepnt->OnGround == 2) || (planepnt->PlaneCopy))
	{
		return;
	}

	MovingVehicleParams *carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	float offangle = AIConvertAngleTo180Degree(carrier->Heading) + deckangle + 180.0f;

	offangle = AICapAngle(offangle);
	AIC_GenericMsgPlaneBearing(planenum, 3, offangle);
}

//**************************************************************************************
void AIC_Ask_Declare(int orgplanenum, int targetnum)
{
	PlaneParams *planepnt = &Planes[orgplanenum];
	PlaneParams *checkplane;
	float foundrange = -1;
	PlaneParams *foundplane = NULL;
	int foundplanenum;
	float dx, dy, dz, tdist;
	float nearrange = 10.0f * NMTOWU;
	int cnt, currid;
	int done = 0;
	int firstpass = 1;
	int currentcnt = 0;
	long nextdelay = 0;
	int numhide;
	int hasgroups = 0;
	int planenum;
	BYTE nettime;

	foundplane = planepnt->AADesignate;

	if(!foundplane)
		return;

	numhide = 0;
	if( GetCurrentAARadarMode() == AA_STT_MODE)
	{
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (checkplane != foundplane))
			{
				dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < nearrange)
				{
					numhide ++;
					break;
				}
			}
		}
	}
	else
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			currid = CurFramePlanes.Planes[cnt].PlaneId;
			checkplane = &Planes[currid];
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (checkplane != foundplane))
			{
				dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < nearrange)
				{
					numhide ++;
					break;
				}
			}
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
				checkplane = &Planes[currid];
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (checkplane != foundplane))
				{
					dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if(tdist < nearrange)
					{
						numhide ++;
						break;
					}
				}
			}
		}
	}

	if(numhide)
	{
		planenum |= 0x4000;
		hasgroups = 1;
	}
	currentcnt ++;
	planenum |= currentcnt << 17;

	foundplanenum = foundplane - Planes;

	foundplanenum |= 0x2000;
	foundplanenum |= 0x40000000;

	if(MultiPlayer && (targetnum == 1))
	{
		nettime = nextdelay / 1000.0f;
		NetPutGenericMessage3Long(NULL, GM3L_BANDIT_CALL, planenum, foundplanenum, nettime);
	}
	AICAddSoundCall(AICBroadcastBanditCall, planenum, nextdelay, 50, foundplanenum);

	nextdelay += 10000;

	AICAddSoundCall(AIC_AWACS_Declare, orgplanenum, nextdelay, 50, foundplane - Planes);
}

//**************************************************************************************
void AIC_AWACS_Declare(int planenum, int targetnum)
{
	PlaneParams *planepnt = &Planes[planenum];
	int awacsnum;
	int voice = 0;
	char declarestr[64];
	char callsign[64];
	char tempstr[1024];
	int msgnum, msgsnd;
	float rangenm = 200.0f;
	int radarid;
	DBRadarType *radar;
	FPointDouble position;
	MovingVehicleParams *carrier;

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
	{
		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			voice = SPCH_STRIKE;
			awacsnum = 1;
			carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];

			if((carrier->Status & VL_DESTROYED) || (!(carrier->Status & VL_ACTIVE)))
			{
				return;
			}
			position = carrier->WorldPosition;

		}
		else
		{
			return;
		}
	}
	else
	{
		radarid = pDBAircraftList[Planes[awacsnum].AI.iPlaneIndex].lRadarID;
		radar = GetRadarPtr(radarid);
		rangenm = radar->iMaxRange;
		position = Planes[awacsnum].WorldPosition;
	}

	if(((position - Planes[targetnum].WorldPosition) * WUTONM) > rangenm)
	{
		msgnum = AIC_UNABLE_CLEAN;
		msgsnd = AICF_UNABLE_CLEAN;
	}
	else if((Planes[targetnum].AI.iSide == planepnt->AI.iSide) || (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN)))
	{
		msgnum = AIC_FRIENDLY;
		msgsnd = AICF_FRIENDLY;
	}
	else if(Planes[targetnum].AI.iSide == AI_NEUTRAL)
	{
		msgnum = AIC_BOGEY;
		msgsnd = AICF_BOGEY;
	}
	else if(iAI_ROE[planepnt->AI.iSide] < 2)
	{
		msgnum = AIC_BANDIT;
		msgsnd = AICF_BANDIT;
	}
	else
	{
		msgnum = AIC_HOSTILE;
		msgsnd = AICF_HOSTILE;
	}

	if(!LANGGetTransMessage(declarestr, 64, msgnum, g_iLanguageId))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_BANDIT_CALL, g_iLanguageId, callsign, declarestr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);

	if(MultiPlayer && (PlayerPlane == planepnt))
	{
		NetPutGenericMessage2(planepnt, GM2_AWACS_DECLARE, targetnum);
	}
}

//**************************************************************************************
void AIC_AWACS_CheckInOut()
{
	if(iAICommFrom < 0)
	{
		int checkin = 0;
		int awacsnum;

		awacsnum = AIGetClosestAWACS(PlayerPlane);
		if(awacsnum < 0)
			awacsnum = -2;

		if(lBombFlags & WSO_AWACS_CHECK)
		{
			lBombFlags &= ~(WSO_AWACS_CHECK);
			checkin = 0;
		}
		else
		{
			lBombFlags |= (WSO_AWACS_CHECK);
			checkin = 1;
		}


		AIC_GenericMsgPlane(PlayerPlane - Planes, 33 - checkin);
		AICAddSoundCall(AIC_AWACS_Acknowledged, PlayerPlane - Planes, 6000, 50, awacsnum);

		if(MultiPlayer)
		{
			NetPutGenericMessage2(PlayerPlane, GM2_GENERIC_RADIO, 33 - checkin);
		}
	}
}

//**************************************************************************************
void AICPlayerRequestSAR()
{
	PlaneParams *checkplane;

	if(iAICommFrom >= 0)
		return;

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if(checkplane->Status & PL_ACTIVE)
		{
			if(AIInPlayerGroup(checkplane))
			{
				if((checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)) && (!(checkplane->AI.iAICombatFlags2 & AI_SAR_CALLED)))
				{
					checkplane->AI.iAICombatFlags2 |= AI_SAR_CALLED;
					AIC_Wingman_Ejected_Msg(checkplane - Planes, PlayerPlane - Planes);
					if(MultiPlayer)
					{
						NetPutGenericMessage2(checkplane, GM2_WING_EJECT, (BYTE)(PlayerPlane - Planes));
					}
					break;
				}
			}
		}
	}
}

extern int		AA_shootQ;		// range between Rmax and Rmin (icon#/0)
extern float	AA_Rmax;		//   max range, really Rmax1 or Raero(nm)
extern float	AA_range;		// radar range to PDT (nm)
extern int		AA_breakX;		// break X displayed (t/f)
//**************************************************************************************
void AICCheckAdvisor()
{
	FPoint position;
	float dx, dy, dz;
	float offangle, offpitch, tdist;
	int done = 0;
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	BombTarget *pActionBombTarget;
	AttackTargetActionType *pActionAttackTarget;
	int placeingroup;
	int bombtarget;
	BasicInstance *walker;
	int cnt;
	int skipwpt = 0;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	if((PlayerPlane->AI.AirThreat) && (lAdvisorFlags & ADV_TARGET))
	{
		lAdvisorTimer = 5000;
	}
	else
	{
		lAdvisorTimer = 15000;
	}

	if(PlayerPlane->OnGround)
	{
		return;
	}

	if(lAdvisorFlags & ADV_TARGET)
	{
		position.X = -1.0f;
		if((PlayerPlane->AI.AirThreat) && (!(PlayerPlane->AI.AirThreat->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			position = PlayerPlane->AI.AirThreat->WorldPosition;
			done = 1;
		}
		else if((PlayerPlane->AI.AirTarget) && (!(PlayerPlane->AI.AirTarget->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			position = PlayerPlane->AI.AirTarget->WorldPosition;
			done = 1;
		}
		else if(PlayerPlane->AGDesignate.X >= 0)
		{
			position = PlayerPlane->AGDesignate;
			done = 1;
		}
		else
		{
			lookway = PlayerPlane->AI.CurrWay;

			startact = lookway->iStartAct;
			endact = startact + lookway->iNumActs;
			for(actcnt = startact; actcnt < endact; actcnt ++)
			{
				if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
				{
					done = 1;
					pActionBombTarget=(BombTarget *)AIActions[actcnt].pAction;
					placeingroup = (PlayerPlane->AI.iAIFlags1  & AIFLIGHTNUMS);
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

					if(walker)
					{
						position = walker->Position;
					}
					else
					{
						position = PlayerPlane->AI.WayPosition;
					}
				}
				else if(AIActions[actcnt].ActionID == ACTION_ATTACKTARGET)
				{
					pActionAttackTarget = (AttackTargetActionType *)AIActions[actcnt].pAction;
					if(pActionAttackTarget->lTargetType == AIRCRAFT)
					{
						done = 1;
						position = Planes[pActionAttackTarget->lTargetID].WorldPosition;
					}
					else if(pActionAttackTarget->lTargetType == GROUNDOBJECT)
					{
						walker = FindInstance(AllInstances, pActionAttackTarget->lTargetID);
						if(walker)
						{
							done = 1;
							position = walker->Position;
						}
					}
					else if(pActionAttackTarget->lTargetType == MOVINGVEHICLE)
					{
						done = 1;
						position = MovingVehicles[pActionAttackTarget->lTargetID].WorldPosition;
					}
				}
			}
		}
		if(done)
		{
			skipwpt = 1;
			dx = position.X - PlayerPlane->WorldPosition.X;
			dy = position.Y - PlayerPlane->WorldPosition.Y;
			dz = position.Z - PlayerPlane->WorldPosition.Z;

			offangle = atan2(-dx, -dz) * 57.2958;

			offangle -= AIConvertAngleTo180Degree(PlayerPlane->Heading);

			offangle = AICapAngle(offangle);

			tdist = QuickDistance(dx, dz);
			offpitch = (atan2(dy, tdist) * 57.2958);

			if(offpitch > 60.0f)
			{
				if(!(lAdvisorFlags & ADV_TARGET_UP))
				{
					lAdvisorFlags &= ~(0x3FF);
					lAdvisorFlags |= ADV_TARGET_UP;
					AICDoAdvisorMsg(AIC_ADV_TARGET_ABOVE);
					return;
				}
			}
			else if(offpitch < -60.0f)
			{
				if(!(lAdvisorFlags & ADV_TARGET_BELOW))
				{
					lAdvisorFlags &= ~(0x3FF);
					lAdvisorFlags |= ADV_TARGET_BELOW;
					AICDoAdvisorMsg(AIC_ADV_TARGET_BELOW);
					return;
				}
			}
			else if(fabs(offangle) < 45.0f)
			{
				if(!(lAdvisorFlags & ADV_TARGET_FRONT))
				{
					lAdvisorFlags &= ~(0x3FF);
					lAdvisorFlags |= ADV_TARGET_FRONT;
					AICDoAdvisorMsg(AIC_ADV_TARGET_FRONT);
					return;
				}
			}
			else if(fabs(offangle) > 135.0f)
			{
				if(!(lAdvisorFlags & ADV_TARGET_BACK))
				{
					lAdvisorFlags &= ~(0x3FF);
					lAdvisorFlags |= ADV_TARGET_BACK;
					AICDoAdvisorMsg(AIC_ADV_TARGET_BEHIND);
					return;
				}
			}
			else if(offangle < 0)
			{
				if(!(lAdvisorFlags & ADV_TARGET_RIGHT))
				{
					lAdvisorFlags &= ~(0x3FF);
					lAdvisorFlags |= ADV_TARGET_RIGHT;
					AICDoAdvisorMsg(AIC_ADV_TARGET_RIGHT);
					return;
				}
			}
			else
			{
				if(!(lAdvisorFlags & ADV_TARGET_LEFT))
				{
					lAdvisorFlags &= ~(0x3FF);
					lAdvisorFlags |= ADV_TARGET_LEFT;
					AICDoAdvisorMsg(AIC_ADV_TARGET_LEFT);
					return;
				}
			}
		}
	}

	if((lAdvisorFlags & ADV_WAYPT) && (!skipwpt))
	{
		dx = PlayerPlane->AI.WayPosition.X - PlayerPlane->WorldPosition.X;
		dz = PlayerPlane->AI.WayPosition.Z - PlayerPlane->WorldPosition.Z;

		offangle = atan2(-dx, -dz) * 57.2958;

		offangle -= AIConvertAngleTo180Degree(PlayerPlane->Heading);

		offangle = AICapAngle(offangle);

		if(fabs(offangle) < 45.0f)
		{
			if(!(lAdvisorFlags & ADV_WAYPT_FRONT))
			{
				lAdvisorFlags &= ~(0x3FF);
				lAdvisorFlags |= ADV_WAYPT_FRONT;
				AICDoAdvisorMsg(AIC_ADV_WAYPT_FRONT);
				return;
			}
		}
		else if(fabs(offangle) > 135.0f)
		{
			if(!(lAdvisorFlags & ADV_WAYPT_BACK))
			{
				lAdvisorFlags &= ~(0x3FF);
				lAdvisorFlags |= ADV_WAYPT_BACK;
				AICDoAdvisorMsg(AIC_ADV_WAYPT_BEHIND);
				return;
			}
		}
		else if(offangle < 0)
		{
			if(!(lAdvisorFlags & ADV_WAYPT_RIGHT))
			{
				lAdvisorFlags &= ~(0x3FF);
				lAdvisorFlags |= ADV_WAYPT_RIGHT;
				AICDoAdvisorMsg(AIC_ADV_WAYPT_RIGHT);
				return;
			}
		}
		else
		{
			if(!(lAdvisorFlags & ADV_WAYPT_LEFT))
			{
				lAdvisorFlags &= ~(0x3FF);
				lAdvisorFlags |= ADV_WAYPT_LEFT;
				AICDoAdvisorMsg(AIC_ADV_WAYPT_LEFT);
				return;
			}
		}
	}

	if((lAdvisorFlags & ADV_MISSILE) && (PlayerPlane->AADesignate) && (!(PlayerPlane->AADesignate->AI.iAICombatFlags2 & AI_ESCORTABLE)))
	{
		AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

		if((UFC.MasterMode == AA_MODE) && (CurAAWeap != NULL) && (Primary->Id != -1) && (!WeapStores.GunsOn))
		{
			if(AA_shootQ)
			{
				if(!(lAdvisorFlags & ADV_SHOOT))
				{
					if(AIPlaneAlreadyTarget(AIRCRAFT, PlayerPlane, PlayerPlane->AADesignate))
					{
						AICDoAdvisorMsg(AIC_ADV_TARGET_IN_RANGE + ((rand() & 0x1) * 3));
					}
					else
					{
						AICDoAdvisorMsg(AIC_ADV_LAUNCH_WEAPON + (rand() & 0x1));
					}
					lAdvisorFlags |= ADV_SHOOT;
					lAdvisorFlags &= ~(ADV_TOO_CLOSE|ADV_TOO_FAR);
				}
				else
				{
					if(AIPlaneAlreadyTarget(AIRCRAFT, PlayerPlane, PlayerPlane->AADesignate))
					{
						lAdvisorFlags |= ADV_MISSILE_FIRED;
					}
					else if(lAdvisorFlags & ADV_MISSILE_FIRED)
					{
						AICDoAdvisorMsg(AIC_ADV_LAUNCH_WEAPON + (rand() & 0x1));
						lAdvisorFlags &= ~(ADV_MISSILE_FIRED);
					}
				}
			}
			else if(AA_breakX)
			{
				if(!(lAdvisorFlags & ADV_TOO_CLOSE))
				{
			 		AICDoAdvisorMsg(AIC_ADV_TARGET_TOO_CLOSE);
				}
				lAdvisorFlags |= ADV_TOO_CLOSE;
				lAdvisorFlags &= ~(ADV_SHOOT|ADV_TOO_FAR);
			}
			else if(AA_range > AA_Rmax)
			{
				if((!(lAdvisorFlags & ADV_TOO_FAR)) && (lAdvisorFlags & ADV_SHOOT))
				{
			 		AICDoAdvisorMsg(AIC_ADV_TARGET_TOO_FAR);
				}
				lAdvisorFlags |= ADV_TOO_FAR;
				lAdvisorFlags &= ~(ADV_SHOOT|ADV_TOO_CLOSE);
			}
		}
		else
		{
			lAdvisorFlags &= ~(ADV_SHOOT|ADV_TOO_FAR|ADV_TOO_CLOSE);
		}
	}
}

//**************************************************************************************
void AICCheckTakeOffAdvisor()
{
	float goodthrottle = 95.0f;
	lAdvisorTimer = 5000;
	MovingVehicleParams *carrier;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		return;

	if(!(lAdvisorFlags & ADV_TAKEOFF_START))
	{
		if(lAdvisorFlags & ADV_CARRIER_LAUNCH)
		{
			if((PlayerPlane->AI.Behaviorfunc != CTWaitingForLaunch) || (PlayerPlane->AI.lVar3 != 100))
				return;

			if(PlayerPlane->Brakes)
			{
		 		AICDoAdvisorMsg(AIC_ADV_SIGNAL_CAT);
			}
			AICAddSoundCall(AICDoAdvisorMsg, AIC_ADV_NO_TOUCH, 6000, 50);
			lAdvisorTimer += 7000;
			lAdvisorFlags |= ADV_FIRST_PULL;
		}
		else
		{
			if(PlayerPlane->Brakes)
			{
		 		AICDoAdvisorMsg(AIC_ADV_RELEASE_BRAKES);
			}
		}
		lAdvisorFlags |= ADV_TAKEOFF_START;
		return;
	}
	else if(!PlayerPlane->Brakes)
	{
		if((!(lAdvisorFlags & ADV_CARRIER_LAUNCH)) && (!(lAdvisorFlags & ADV_SPEED_ADVISOR)))
		{
	 		AICDoAdvisorMsg(AIC_ADV_TAKEOFF_SPD);
			lAdvisorFlags |= ADV_SPEED_ADVISOR;
			return;
		}

		if((PlayerPlane->AI.Behaviorfunc == CTWaitingForLaunch) && (PlayerPlane->AI.lTimer2 > 9000))
		{
			return;
		}

		if(PlayerPlane->Knots < 250.0f)
		{
			if(!(lAdvisorFlags & ADV_THROTTLE_UP))
			{
				if(PlayerPlane->LeftThrustPercent < goodthrottle)
				{
		 			AICDoAdvisorMsg(AIC_ADV_THROTTLE_MAX + (rand() % 3));
					lAdvisorFlags |= ADV_THROTTLE_UP;
					return;
				}
				else if(!(lAdvisorFlags & ADV_GOOD_THROTTLE))
				{
		 			AICDoAdvisorMsg(AIC_ADV_THROTTLES_GOOD);
					lAdvisorFlags |= ADV_GOOD_THROTTLE;
					return;
				}
			}
			else
			{
				if(PlayerPlane->LeftThrustPercent > goodthrottle)
				{
					lAdvisorFlags &= ~(ADV_THROTTLE_UP);
					if(!(lAdvisorFlags & ADV_GOOD_THROTTLE))
					{
		 				AICDoAdvisorMsg(AIC_ADV_THROTTLES_GOOD);
						lAdvisorFlags |= ADV_GOOD_THROTTLE;
						return;
					}
				}
			}
		}

		if(!(lAdvisorFlags & ADV_FIRST_PULL))
		{
			if(PlayerPlane->Knots > 130.0f)
			{
				if(PlayerPlane->OnGround)
				{
		 			AICDoAdvisorMsg(AIC_ADV_GENTLY_BACK + (rand() & 0x1));
				}
				lAdvisorFlags |= ADV_FIRST_PULL;
				return;
			}
		}

		if((PlayerPlane->HeightAboveGround > (1000.0f * FTTOWU)) && (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
 			AICDoAdvisorMsg(AIC_ADV_RAISE_GEAR + (rand() & 0x1));
			lAdvisorTimer += 5000;
		}

		if((AIConvertAngleTo180Degree(AIGetPlanesFlightPitch(PlayerPlane)) < 1.0f) && (!PlayerPlane->OnGround) && (PlayerPlane->HeightAboveGround < (1200.0f * FTTOWU)))
		{
 			AICDoAdvisorMsg(AIC_ADV_INC_ALT + (rand() % 3));
		}

		if(!(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			if(lAdvisorFlags & ADV_CARRIER_LAUNCH)
			{
				carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
				if((carrier->WorldPosition - PlayerPlane->WorldPosition) > (7.0f * NMTOWU))
				{
					lAdvisorFlags &= ~(ADV_TAKEOFF);
				}
			}
			else
			{
				lAdvisorFlags &= ~(ADV_TAKEOFF);
				PlayerPlane->AI.iAICombatFlags1 &= ~(AI_CARRIER_LANDING);
			}
		}
	}
}

//**************************************************************************************
void AICCheckPlayerTakeOff()
{
	float tdist, dx, dy, dz, offangle;
	MovingVehicleParams *carrier;

	if(PlayerPlane->OnGround == 1)
	{
		lAdvisorFlags &= ~(ADV_CARRIER_LAUNCH);
		PlayerPlane->AI.lVar2 = 110;
	}
	else if(PlayerPlane->OnGround == 2)
	{
		lAdvisorFlags |= ADV_CARRIER_LAUNCH;
	}
	else if((PlayerPlane->Status & PL_DEVICE_DRIVEN) && (PlayerPlane->AI.lVar2 < 100) && (lAdvisorFlags & ADV_CARRIER_LAUNCH))
	{
		PlayerPlane->AI.lVar2 = 100;
	}

	if((lAdvisorFlags & ADV_CARRIER_LAUNCH) && (lAdvisorTimer > 5000) && (!(lAdvisorFlags & ADV_TAKEOFF_START)))
	{
		lAdvisorTimer = 5000;
	}

	lAdvisorTimer -= DeltaTicks;
	if(lAdvisorFlags & ADV_TAKEOFF)
	{
		if(lAdvisorTimer < 0)
		{
			AICCheckTakeOffAdvisor();
		}
	}

	if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
	else
		carrier = NULL;

	if(!(PlayerPlane->Status & AL_AI_DRIVEN))
	{
		if(0 <= PlayerPlane->AI.lTimer2)
		{
			PlayerPlane->AI.lTimer2 -= DeltaTicks;
		}
	}

	if(carrier)
	{
		switch(PlayerPlane->AI.lVar2)
		{
			case 100:
				tdist = (carrier->WorldPosition - PlayerPlane->WorldPosition) * WUTONM;
				if(tdist > 2.5)
				{
					PlayerPlane->AI.lVar2 = 101;

					AIC_GenericMsgPlane(PlayerPlane - Planes, 38);  //  Passing 2.5

					if(MultiPlayer)
					{
						NetPutGenericMessage2(PlayerPlane, GM2_GENERIC_RADIO, 38);
					}
				}
				break;
			case 101:		//  Arcing
				tdist = (carrier->WorldPosition - PlayerPlane->WorldPosition) * WUTONM;
				if(tdist > 7)
				{
					PlayerPlane->AI.lVar2 = 102;

					AIC_GenericMsgPlane(PlayerPlane - Planes, 39);

					if(MultiPlayer)
					{
						NetPutGenericMessage2(PlayerPlane, GM2_GENERIC_RADIO, 39);
					}
				}
				break;
			case 102:	//  outbound
				offangle = AIComputeWayPointHeading(PlayerPlane, &tdist, &dx ,&dy, &dz, 1);
				if(fabs(offangle) < 10.0f)
				{
					PlayerPlane->AI.lVar2 = 103;

					AIC_GenericMsgPlane(PlayerPlane - Planes, 40);

					if(MultiPlayer)
					{
						NetPutGenericMessage2(PlayerPlane, GM2_GENERIC_RADIO, 40);
					}
					PlayerPlane->AI.lTimer2 = 4000;
				}
				break;
			case 103:	//  up for checks
				if(PlayerPlane->AI.lTimer2 < 0)
				{
					PlayerPlane->AI.lVar2 = 104;
					AIC_GenericMsgPlane(PlayerPlane - Planes, 41);

					if(MultiPlayer)
					{
						NetPutGenericMessage2(PlayerPlane, GM2_GENERIC_RADIO, 41);
					}
					PlayerPlane->AI.lTimer2 = 6000;
				}
				break;
			case 104:	//  sweet and sweet
				if(PlayerPlane->AI.lTimer2 < 0)
				{
					AIC_GenericMsgPlane(PlayerPlane - Planes, 36);

					if(MultiPlayer)
					{
						NetPutGenericMessage2(PlayerPlane, GM2_GENERIC_RADIO, 36);
					}
					PlayerPlane->AI.iAICombatFlags1 &= ~(AI_CARRIER_LANDING);

					int awacsnum = AIGetClosestAWACS(PlayerPlane);

					AICAddSoundCall(AICStrikeSwitchControl, PlayerPlane - Planes, 8000, 50, awacsnum);
				}
				break;
		}
	}
}

//**************************************************************************************
void AICDoAdvisorMsg(int msgnum, int targetnum)
{
	char tempstr[1024];
	int msgsnd;
	int sndids[30];
	int numids;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	msgsnd = ADV_SPCH_START + (msgnum - AIC_ADV_THROTTLE_MAX);
	sndids[0] = msgsnd;
	numids = 1;
	AIRSendSentence(0, numids, sndids, -2);
}

//**************************************************************************************
void AIC_ACLS_Switch(PlaneParams *planepnt, int on)
{
	MovingVehicleParams *carrier;
	float tdist;

	if((PlayerPlane->AI.Behaviorfunc == AIFlyTankerFormation) || (PlayerPlane->AI.Behaviorfunc == AITankerDisconnect))
	{
		return;
	}

	if(on)
	{
		if((planepnt == PlayerPlane) && (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14) || ((planepnt->AI.lVar2 > 7) && (planepnt->AI.lVar2 <= 10))))))
		{
			return;
		}

		StartCasualAutoPilotPlane(planepnt);

		//  For Patch, setting casual Autopilot
		if(on == 2)
		{
			return;
		}

		carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];

		tdist = (carrier->WorldPosition - planepnt->WorldPosition) * WUTONM;
		if(tdist > 4.0f)
		{
			planepnt->AI.Behaviorfunc = AIFlyToDME3;
		}
		else
		{
			planepnt->AI.Behaviorfunc = AIFlyCarrierFinal;
		}

		if(MultiPlayer && (planepnt == PlayerPlane))
		{
			NetPutGenericMessage2(planepnt, GM2_ACLS_SET, on);
		}
	}
	else
	{
		if(MultiPlayer && (planepnt == PlayerPlane))
		{
			NetPutGenericMessage2(planepnt, GM2_ACLS_SET, on);
		}
		EndCasualAutoPilotPlane(planepnt);
		if(planepnt == PlayerPlane)
		{
			UFC.APStatus = 0;
			PlayerPlane->AutoPilotMode = PL_AP_OFF;
			DisplayWarning(AUTO_ALERT,OFF,0);
		}
	}
}

//**************************************************************************************
void AICRequestEmergencyApproach(int planenum, int targetnum)
{
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;
	long delaycnt = 1000;
	RunwayInfo *tower;
	PlaneParams *planepnt = &Planes[planenum];

	if(!AICAllowThisRadio(planepnt - Planes, 1))
	{
		return;
	}

	if((PlayerPlane->OnGround) || ((PlayerPlane->HeightAboveGround * WUTOFT) < 10.0f))  //  will need to make a function to check for nearby airport.  SRE
	{
		return;
	}

	tower = (RunwayInfo *)AIGetClosestAirField(planepnt, planepnt->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, (20.0f * NMTOWU), 0);

	if(!tower)
	{
		AICAddSoundCall(AIC_GenericMsgPlane, planenum, delaycnt, 50, 34);

		delaycnt = delaycnt + 5000;
		AICAddSoundCall(AICAlphaCheck, planenum, delaycnt, 50, 6);
		return;
	}

	AICAddSoundCall(AIC_WSOLandingRequestMsg, planepnt - Planes, delaycnt, 50);
	delaycnt = delaycnt + 5000;

	AICAddSoundCall(AIC_GenericMsgPlane, planenum, delaycnt, 50, 35);
	delaycnt = delaycnt + 5000;

	AICAddSoundCall(AICTowerLandingReplyMsg, planepnt - Planes, delaycnt, 50, 1);

	AICHoldMarshalStack(planenum);
}

//**************************************************************************************
int AICGetTowerVoice(int planenum)
{
	long lrx, lrz;
	FPointDouble runwaypos = Planes[planenum].WorldPosition;

	lrx = runwaypos.X * WUTOFT;
	lrz = runwaypos.Z * WUTOFT;

	if(!AICCheckOneFence(FenceArea[3].iNumPoints, FenceArea[3].point, lrx, lrz))
	{
		return(SPCH_TWR_NATO);
	}
	else
	{
		return(SPCH_TWR_RUS);
	}

	return(SPCH_TWR_NATO);
}

//**************************************************************************************
void AICHoldMarshalStack(int planenum)
{
}

//**************************************************************************************
int AICDeckClear(PlaneParams *planepnt)
{
	PlaneParams *checkplane;
	MovingVehicleParams *carrier;
	int goaround = 0;

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

	carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
	if(carrier->lAIFlags1 & V_ON_FIRE)
	{
		CTSendToDivertField(planepnt);
		return(0);
	}

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if ((!(checkplane->Status & PL_ACTIVE)) || (checkplane->AI.iAIFlags2 & AIINVISIBLE))  continue;

		if (checkplane->FlightStatus & PL_STATUS_CRASHED)  continue;

		if(checkplane == planepnt)  continue;

		if(checkplane->AI.iAICombatFlags1 & AI_HOME_AIRPORT)	continue;

		if(checkplane->AI.iHomeBaseId != planepnt->AI.iHomeBaseId)	continue;

		if(checkplane == PlayerPlane)
		{
			if(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)
			{
				if(PlayerPlane->AI.lVar2 == 10)
					goaround = 1;
			}
			else if(PlayerPlane->AI.Behaviorfunc == AICarrierTrap)
			{
				goaround = 1;
			}
		}
		else if((checkplane->Status & PL_COMM_DRIVEN) || (checkplane->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
		{
			if(checkplane->OnGround == 2)
			{
				if(checkplane->AI.Behaviorfunc == AICarrierTrap)
				{
					goaround = 1;
				}
			}
			else if(checkplane->OnGround == 0)
			{
				if((checkplane->WorldPosition - carrier->WorldPosition) < NMTOWU)
				{
					goaround = 1;
				}
			}
		}
		else
		{
			if((checkplane->AI.Behaviorfunc == AIFlyCarrierFinal) && (checkplane->AI.lVar2 == 3))
				goaround = 1;
			else if(checkplane->AI.Behaviorfunc == AIWaitForTrapOrBolter)
				goaround = 1;
			else if(checkplane->AI.Behaviorfunc == AICarrierTrap)
				goaround = 1;
		}
	}

	if(goaround)
	{
		if((planepnt == (PlaneParams *)Camera1.AttachedObject) && (planepnt == PlayerPlane))
		{
			AICJustMessage(AIC_WAVEOFF_DECK, AICF_WAVEOFF_DECK, SPCH_LSO);
		}
		if(planepnt == PlayerPlane)
		{
			AICAddSoundCall(AIC_BolterStart, PlayerPlane - Planes, 4000, 50);
			PlayerPlane->AI.iAICombatFlags2 |= AI_BOLTERED;
			PlayerPlane->AI.lTimer2 = 120000;
			PlayerPlane->AI.lVar2 = 11;
		}
		else if(!((planepnt->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
		{
			AICAddSoundCall(AIC_BolterStart, planepnt - Planes, 4000, 50);
			planepnt->AI.Behaviorfunc = AIFlyBolter;
			planepnt->AI.lTimer3 = 120000;
		}

		return(0);
	}

	return(1);
}

//**************************************************************************************
void AICPlayerSeesBall(int planenum, int targetnum)
{
	if((iAICommFrom < 0) && ((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14))))))
		return;

	if(Planes[planenum].OnGround)
		return;

	AICSeeBall(planenum, -1);
	Planes[planenum].AI.lVar2 = 10;
	Planes[planenum].AI.lVar3 = 0;
}

//**************************************************************************************
void AICPlayerClara(int planenum, int targetnum)
{
	if((iAICommFrom < 0) && ((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14))))))
		return;

	if(Planes[planenum].OnGround)
		return;

	AICSeeBall(planenum, 1);
//	Planes[planenum].AI.lVar2 = 14;  Moved this so it happens after Player hears keep coming
	Planes[planenum].AI.lVar3 = 0;
}


//**************************************************************************************
void AICPlayerRequestTaxi(int planenum, int targetnum)
{
}

//**************************************************************************************
void AICTowerInbound(int planenum, int targetnum)
{
	PlaneParams *planepnt = &Planes[planenum];

	AICRequestLanding(planepnt);

	if(planepnt == PlayerPlane)
	{
		PlayerPlane->AI.lVar2 = -1;
		iATCRadio |= 2;
	}
}

//**************************************************************************************
void AICGenericBettySpeech(int msgtype, int bearing, int rangenm, int cnt, int other1, int other2)
{
	int sndids[30];
	int numval;
	int workval;
	int allowinaccel = 0;
	int notwhilelanding = 0;

	if (!g_iBaseBettySoundLevel)
		return;

	switch(msgtype)
	{
		case BETTY_WAY_DIST:  //  10s of miles to waypoint
			sndids[0] = SND_BETTY_SIXTY + (6 - rangenm);
			sndids[1] = SND_BETTY_MILES;
//			sndids[2] = SND_BETTY_TO;
//			sndids[3] =
			numval = 2;
			notwhilelanding = 1;
			break;
		case BETTY_CHAFF:  //  chaff
			sndids[0] = SND_BETTY_CHAFF;
			sndids[1] = SND_BETTY_CHAFF;
			numval = 2;
			break;
		case BETTY_FLARE:  //  flare
			sndids[0] = SND_BETTY_FLARE;
			sndids[1] = SND_BETTY_FLARE;
			numval = 2;
			break;
		case BETTY_LEFT:  //  left
			sndids[0] = SND_BETTY_LEFT;
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_RIGHT:  //  right
			sndids[0] = SND_BETTY_RIGHT;
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_MISSILE_OCLOCK:  //  missile clock position;
			sndids[0] = SND_BETTY_MISSILE;
			sndids[1] = SND_BETTY_START_OCLOCK + bearing;
			numval = 2;
			break;
		case BETTY_TARGET_DIST:  //  10s of miles to target
			if(rangenm > 6)
				return;
			sndids[0] = SND_BETTY_SIXTY + (6 - rangenm);
			sndids[1] = SND_BETTY_MILES;
			sndids[2] = SND_BETTY_TO;
			sndids[3] = SND_BETTY_TARGET;
			numval = 4;
			notwhilelanding = 1;
			break;
		case BETTY_PULL_UP:		//  Pull Up (using for loft bomb).
			sndids[0] = SND_BETTY_PULLUP;
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_PULL_SECS:		//  Seconds till Pull
			if(cnt == 5)
			{
				sndids[0] = SND_BETTY_FIVE;
			}
			else
			{
				workval = cnt / 10;
				if((cnt < 6) && (cnt))
				{
					sndids[0] = SND_BETTY_SIXTY + (6 - workval);
				}
			}
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_ZERO:		//  Zero
			sndids[0] = SND_BETTY_ZERO;
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_TARGET_INTACT:
			sndids[0] = SND_BETTY_TARGET;		//  Target
			sndids[1] = SND_BETTY_INTACT;		//  Intact
			numval = 2;
			break;
		case BETTY_TARGET_DESTROYED:
			sndids[0] = SND_BETTY_TARGET;		//  Target
			sndids[1] = SND_BETTY_DESTROYED;		//  Destroyed
			numval = 2;
			break;
		case BETTY_TARGET_DAMAGED:
			sndids[0] = SND_BETTY_TARGET;		//  Target
			sndids[1] = SND_BETTY_DAMAGED;		//  Damaged
			numval = 2;
			break;
		case BETTY_ALERT:
			sndids[0] = SND_BETTY_ALERT;		//  Alert
			sndids[1] = SND_BETTY_ALERT;		//  Alert
			numval = 2;
			break;
		case BETTY_SAM_LAUNCH:  //  SAM launch clock position;
			sndids[0] = SND_BETTY_SAM;
			sndids[1] = SND_BETTY_LAUNCH;
			sndids[2] = SND_BETTY_START_OCLOCK + bearing;
			numval = 3;
			break;
		case BETTY_ENEMY_U_D:	//  Enemy Up Down (might want to use high, low but it's for WEEDS/VERTICAL so it's a hack anyway
			sndids[0] = 110;	// Enemy
			if(bearing == 0)
				sndids[1] = SND_BETTY_DOWN;		//  Down
			else
				sndids[1] = SND_BETTY_UP;		//  Up
			numval = 2;
			break;
		case BETTY_ENEMY_POSITION:	//  Enemy Position
			sndids[0] = SND_BETTY_ENEMY;	//  Enemy
			if(cnt == 0)
				sndids[1] = SND_BETTY_HELICOPTER;		//  Helicopter
			else
				sndids[1] = SND_BETTY_AIRCRAFT;		//  Aircraft
			sndids[2] = SND_BETTY_START_OCLOCK + bearing;	//  o'clock position

			if(rangenm < 0)
			{
				sndids[3] = SND_BETTY_LOW2;		//  low
				numval = 4;
			}
			else if(rangenm > 0)
			{
				sndids[3] = SND_BETTY_HIGH;		//  high
				numval = 4;
			}
			else
			{
				numval = 3;
			}
			break;
		case BETTY_NET_HACK:	//  Net Hack of database detected.
			sndids[0] = SND_BETTY_WARNING;
			sndids[1] = SND_BETTY_CHECK;
			sndids[2] = SND_BETTY_COMPUTER;
			numval = 3;
			break;
		case BETTY_MISSILE_LAUNCH:	//  Missile launch clock position;
			sndids[0] = SND_BETTY_MISSILE;
			sndids[1] = SND_BETTY_LAUNCH;
			sndids[2] = SND_BETTY_START_OCLOCK + bearing;
			numval = 3;
			break;
		case BETTY_UP:	//  Up
			sndids[0] = SND_BETTY_UP;
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_DOWN:	//  Down
			sndids[0] = SND_BETTY_DOWN;		//  Down
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_COUNT:
			if(rangenm > 6)
				return;
			sndids[0] = SND_BETTY_SIXTY + (6 - rangenm);
			numval = 1;
			notwhilelanding = 1;
			break;
		case BETTY_LOW_ALT:
			if(lAdvisorFlags & ADV_CASUAL_BETTY)
			{
				sndids[0] = SND_BETTY_LOW;
				sndids[1] = SND_BETTY_ALTITUDE;
				sndids[2] = SND_BETTY_LOW;
				sndids[3] = SND_BETTY_ALTITUDE;
				numval = 4;
			}
			else
			{
				sndids[0] = SND_BETTY_ALTITUDE;
				sndids[1] = SND_BETTY_ALTITUDE;
				numval = 2;
			}
			allowinaccel = 1;
			break;
		case BETTY_SINK_RATE:
			sndids[0] = SND_BETTY_SINK_RATE;
			sndids[1] = SND_BETTY_SINK_RATE;
			numval = 2;
			allowinaccel = 1;
			break;
		case BETTY_FLIGHT_CONTROLS:
			sndids[0] = SND_BETTY_FLIGHT_CONTROLS;
			sndids[1] = SND_BETTY_FLIGHT_CONTROLS;
			numval = 2;
			allowinaccel = 1;
			break;
	}

	if((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && (notwhilelanding))
	{
		return;
	}

	AIRSendSentence(0, numval, sndids, -2, allowinaccel, g_iBaseBettySoundLevel);
}

//**************************************************************************************
void AICStrikeSwitchControl(int planenum, int targetnum)
{
	char callsign[64];
	char awacscallsign[64];
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int placeingroup;

	if(!AICAllowThisRadio(planenum, 76))
	{
		return;
	}

	if(targetnum < 0)
		return;
	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, awacscallsign);
	if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_SWITCH_CONTACT, g_iLanguageId, callsign, awacscallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_STRIKE_SWITCH_CONTACT, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	done = 0;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			if(numval == 2)
			{
				sndids[numids] = AIRGetCallSignID(planenum) + SPCH_STRIKE;
				numids ++;

				sndids[numids] = SPCH_ZERO + Planes[planenum].AI.cCallSignNum + SPCH_STRIKE;
				numids ++;

				placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
				sndids[numids] = AIRWingNumSndID(planenum, placeingroup) + SPCH_STRIKE;
				numids ++;

				sndids[numids] = DEAD_SPACE;
				numids ++;
			}
			else
			{
				numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, SPCH_STRIKE);
			}
		}
		else
		{
			sndids[numids] = numval + SPCH_STRIKE;
			numids ++;
		}
	}

	if(numids)
	{
		AIRSendSentence(planenum, numids, sndids);
	}
}

//**************************************************************************************
void AICGiveHackMessage(int planenum)
{
	char tempstr[1024];
	char callsign[64];

	if(planenum == -1) 
		strcpy(callsign, "?????");
	else
	{
		NetGetMultiPlayerName(planenum, callsign);
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_NET_HACK_DETECTED, g_iLanguageId, callsign))
	{
		sprintf(tempstr, "DAMN WE HAD A PROBLEM");
		AICAddAIRadioMsgs(tempstr, 50);

		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AICGenericBettySpeech(BETTY_NET_HACK);
}

//**************************************************************************************
void AICDelayedSoundEffect(int soundeffect, int volume)
{
	SndQueueSound(soundeffect,1,volume);
}

void DisplayInitText(char *sTxt, int SkipLine, int showrelease=0);
//**************************************************************************************
void AICInitExternalVars()
{
	int hMissionFile;
	char szFileName[260];
	char sDebugTxt[260];
	char workstr[80];
	char workstrnum[80];
	int worknum;
	char *workptr;

	iOtherRadioRangeNM = 20;
	iHUDR = -1;
	iHUDG = -1;
	iHUDB = -1;
	iCommR = 1;
	iCommG = 1;
	iCommB = 1;
	iInactiveCommR = 128;
	iInactiveCommG = 128;
	iInactiveCommB = 128;
	iCommBoxR = 160;
	iCommBoxG = 160;
	iCommBoxB = 160;
	iEndBoxOpt = 1;
	iEscOpt = 0;
	iKeyboardOpt = 0;
	iMShowFriend = 0;

	iMsgBoxColor = -20;
	iMsgTextColor = -10;
	iNoSelectTextColor = -30;
	fMeatBallOffsetFT = 0;

	fFCWarningAdjust = 0;
	iAllowMultiPause = 0;

	char *pszPath = GetRegValue ("resource");
	strcpy(szFileName,pszPath);
	strcat(szFileName,"\\");
	strcat(szFileName,"f18misc.ini");

	if ((hMissionFile=_open(szFileName,_O_BINARY | _O_RDONLY)) == -1)
	{
		sprintf(sDebugTxt,"No %s, using default settings",szFileName);
		DisplayInitText(sDebugTxt,1);
		return;
	}

	while(!_eof(hMissionFile))
	{
		workptr = workstr;
		_read(hMissionFile,workptr,1);
		while((*workptr != '=') && (!_eof(hMissionFile)))
		{
			if(*workptr == 10)
			{
				workptr = workstr;
			}
			else if(!((*workptr == 32) || (*workptr == 10)))
			{
				workptr ++;
			}
			_read(hMissionFile,workptr,1);
		}

		if((*workptr == '=') && (!_eof(hMissionFile)))
		{
			*workptr = 0;
			workptr = workstrnum;
			_read(hMissionFile,workptr,1);
			while((*workptr != 10) && (!_eof(hMissionFile)))
			{
				workptr ++;
				_read(hMissionFile,workptr,1);
			}

			if(!_eof(hMissionFile))
				*workptr = 0;

			worknum = atoi(workstrnum);

			if(strcmpi(workstr, "HUD_R") == 0)
			{
				iHUDR = worknum;
			}
			else if(strcmpi(workstr, "HUD_G") == 0)
			{
				iHUDG = worknum;
			}
			else if(strcmpi(workstr, "HUD_B") == 0)
			{
				iHUDB = worknum;
			}
			else if(strcmpi(workstr, "COMM_R") == 0)
			{
				iCommR = worknum;
			}
			else if(strcmpi(workstr, "COMM_G") == 0)
			{
				iCommG = worknum;
			}
			else if(strcmpi(workstr, "COMM_B") == 0)
			{
				iCommB = worknum;
			}
			else if(strcmpi(workstr, "INACTIVE_COMM_R") == 0)
			{
				iInactiveCommR = worknum;
			}
			else if(strcmpi(workstr, "INACTIVE_COMM_G") == 0)
			{
				iInactiveCommG = worknum;
			}
			else if(strcmpi(workstr, "INACTIVE_COMM_B") == 0)
			{
				iInactiveCommB = worknum;
			}
			else if(strcmpi(workstr, "COMM_BOX_R") == 0)
			{
				iCommBoxR = worknum;
			}
			else if(strcmpi(workstr, "COMM_BOX_G") == 0)
			{
				iCommBoxG = worknum;
			}
			else if(strcmpi(workstr, "COMM_BOX_B") == 0)
			{
				iCommBoxB = worknum;
			}
			else if(strcmpi(workstr, "END_BOX") == 0)
			{
				iEndBoxOpt = worknum;
			}
			else if(strcmpi(workstr, "ESC_OPTION") == 0)
			{
				iEscOpt = worknum;
			}
			else if(strcmpi(workstr, "ORIGINAL_KEYBOARD") == 0)
			{
				iKeyboardOpt = worknum;
			}
			else if(strcmpi(workstr, "M_SHOW_FRIEND") == 0)
			{
				iMShowFriend = worknum;
			}
			else if(strcmpi(workstr, "MEATBALL_OFFSET") == 0)
			{
				fMeatBallOffsetFT = (float)worknum;
			}
			else if(strcmpi(workstr, "OTHER_RADIO_RANGE") == 0)
			{
				iOtherRadioRangeNM = worknum;
			}
			else if(strcmpi(workstr, "FC_WARNING_ADJUST") == 0)
			{
				fFCWarningAdjust = atof(workstrnum);
			}
			else if(strcmpi(workstr, "FC_WARNING_ADJUST_ROLL") == 0)
			{
				fFCWarningAdjustRoll = atof(workstrnum);
			}
		}
	}
	_close(hMissionFile);
}

//**************************************************************************************
void SetSeatVars()
{
	if(!pSeatData)
		return;

	pSeatData->IndicatedAirSpeed = PlayerPlane->IndicatedAirSpeed;
	pSeatData->Knots = PlayerPlane->Knots;
	pSeatData->Roll = PlayerPlane->Roll;
	pSeatData->Pitch = PlayerPlane->Pitch;
	pSeatData->Heading = PlayerPlane->Heading;
	pSeatData->Altitude = PlayerPlane->Altitude;
	pSeatData->GForce = PlayerPlane->GForce;
	pSeatData->LeftRPM = EMD_lSpin;
	pSeatData->RightRPM = EMD_rSpin;
	pSeatData->LeftThrustPercent = PlayerPlane->LeftThrustPercent;
	pSeatData->RightThrustPercent = PlayerPlane->RightThrustPercent;
	pSeatData->SimPause = SimPause|iEndGameState;
	if(PlayerPlane->LandingGearStatus)
	{
		pSeatData->GearState = 1;
	}
	else if(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)
	{
		pSeatData->GearState = 2;
	}
	else 
	{
		pSeatData->GearState = 0;
	}

	if((PlayerPlane->AI.Behaviorfunc == CTDoCarrierLaunch) && (PlayerPlane->AI.lVar3 == 0))
	{
		pSeatData->OnGround = 3;
	}
	else if((PlayerPlane->TailHookFlags & TH_HOOKED) && (PlayerPlane->V >= 0))
	{
		pSeatData->OnGround = 4;
	}
	else
	{
		pSeatData->OnGround = PlayerPlane->OnGround;
	}

	//  This shouldn't be needed but I have as a double check;
	if(PlayerPlane->Trigger1 == 0)
		pSeatData->Gun = 0;

	pSeatData->Mach = PlayerPlane->Mach;
	pSeatData->VerticalVel = PlayerPlane->IfVelocity.Y;
	pSeatData->SpeedBrake = PlayerPlane->SpeedBrake;
	pSeatData->Flaps = PlayerPlane->Flaps;
	pSeatData->DamageHeading = 0;
	pSeatData->DamagePitch = 0;
	pSeatData->DamageDone = 0;
	pSeatData->DamageType = -1;
	pSeatData->WeaponReleaseID = -1;
	pSeatData->WeaponReleaseSide = 0;
	pSeatData->WeaponReleaseWeight = 0;
	if(pSeatData->ProbeConnection != 2)
	{
		pSeatData->ProbeConnection = (PlayerPlane->RefuelProbeCommandedPos > 10.0f) ? 1 : 0;
	}

	if(PlayerPlane->FlightStatus & PL_STATUS_CRASHED)
	{
		pSeatData->CrashState |= 0x1;
	}
	else 
	{
		pSeatData->CrashState &= ~(0x1);
	}

	if(pPlayerChute)
	{
		pSeatData->CrashState |= 0x2;
	}
	else 
	{
		pSeatData->CrashState &= ~(0x2);
	}

	pSeatData->SystemInactive = PlayerPlane->SystemInactive;
	pSeatData->Rudder = PlayerPlane->Rudder;
	pSeatData->Brakes = PlayerPlane->Brakes;

	pSeatData->Counter ++;
	if((pSeatData->Counter > 32000) || (pSeatData->Counter < 0))
	{
		pSeatData->Counter = 0;
	}

}
