// keyfunc.cpp

#include "F18.h"
#include <dsound.h>

#include "3dfxF18.h"
#include "keystuff.h"
#include "buildmsg.h"
#include "gamesettings.h"
#include "keysmsg.h"
#include "particle.h"
#include "flightfx.h"
#include "dplay.h"		// delete this when DX5 to all

//
// local data defines
//

VKCODE vkUpACM, vkDownACM, vkFrontACM, vkBackACM, vkLeftACM;
VKCODE vkRightACM, vkFrontLeftACM, vkFrontRightACM, vkBackLeftACM;
VKCODE vkBackRightACM;
VKCODE vkGlanceFrontACM;
VKCODE vkVirtualMPD;
int CountObjects(double region_size,double step_size, FPointDouble *highestUL);
void AICAddAIRadioMsgs(char *radiomsg, int priority);
BOOL pigs;
void DoUFCMainNavFlir(void);
void aic_request_takeoff(VKCODE vk);
extern int	newTime;	// current physics tick, ie. T1
extern int	MySlot;		// my player's slot index [0..N]
extern int NetSend( int dpID, int flags, void *lpMsg, int size );
extern void StopFFForPauseOrEnd();

extern CameraInstance	 OldCamera;
extern int OldCockpitSeat, OldCurrentCockpit,OldCursor;
extern int CursorIsVisible;
extern char OldInteract;

//
// external data defines
//

extern BOOL FastLand;
extern int TimeExcel;
extern HWND	g_hGameWnd;
extern int XmitChannel;
extern BOOL SimpleTerrainLoaded;

extern int g_iBaseBettySoundLevel;
extern int DrawPopMpds;
extern int InteractCockpitMode;

extern AvionicsType Av;
extern WeapStoresType WeapStores;

extern VKCODE RDRLeft;
extern VKCODE RDRRight;
extern VKCODE MasterPress;

extern VKCODE FlirSlewUpPress;
extern VKCODE FlirSlewDownPress;
extern VKCODE FlirSlewLeftPress;
extern VKCODE FlirSlewRightPress;

extern VKCODE KeyFlyUpVk;
extern VKCODE KeyFlyDownVk;
extern VKCODE KeyFlyLeftVk;
extern VKCODE KeyFlyRightVk;

extern VKCODE TdcLeftVk;
extern VKCODE TdcRightVk;
extern VKCODE TdcUpVk;
extern VKCODE TdcDownVk;

extern VKCODE CaptBarsVk;

//
// external keystroke functions
//

void ScreenCapture();
void set_primary(VKCODE vk);
void set_secondary(VKCODE vk);
void dprintf( PCSTR fmt, ... );
int NetCycleChannel(int nChannel);
void DoRightDownSwitch(void);
void NetPutPause();
void ReloadNimitzClassCarriers( void );
void FlirContrastUp();
void FlirContrastDown();
void FlirBrightnessUp();
void FlirBrightnessDown();
void FlirTogglePolarity();
void FlirToggleDeclutter();
void NavFlirContrastUp();
void NavFlirContrastDown();
void NavFlirBrightnessUp();
void NavFlirBrightnessDown();
void NavFlirTogglePolarity();
void NavFlirToggleDeclutter();
void NavFlirClick(int *State);
void CycleSteeringMode(void);
void HsiSteeringDown();
void HsiSteeringUp();
void ToggleGunFireRate();
void AAChangeMode();
void AGChangeMode();
void AzimScanChange();
void AGAzimScanChange();

extern int TDCMpdNum;   // set to the current mpd that has control for capt bars and other things
extern MpdButtonPressType MpdButton;


#define VIEWCHECK(def,key,fn)	if (!(ViewKeyFlags & def))\
								{\
									fn(FALSE);\
									ViewKeyFlags |= def;\
								}\
								key = vk;


extern void SetNextViewLocation();
extern void InitLocations();

//***********************************************************************************
//
// keystroke functions
//
//***********************************************************************************

void SendNetKey(BYTE commid, WPARAM wParam)
{
	if(MultiPlayer)
	{
		if(iMultiCast >= 0)
		{
			DPMSG_AI_COMMS_SM	NetAICommsSM;

			NetAICommsSM.type   = NET_AI_COMMS_SM;
			NetAICommsSM.keyflag = 0;
			NetAICommsSM.wParam = wParam;
			NetAICommsSM.commid = commid;
			NetAddToBigPacket(MySlot, &NetAICommsSM, sizeof(NetAICommsSM) );
		}
		else
		{
			DPMSG_AI_COMMS	NetAIComms;

			NetAIComms.type   = NET_AI_COMMS;
			NetAIComms.time   = newTime;
			NetAIComms.slot   = MySlot;
			NetAIComms.keyflag = 0;
			NetAIComms.wParam = wParam;
			NetAIComms.commid = commid;
			NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAIComms, sizeof(NetAIComms) );
		}
	}
}

void NextViewLocation(VKCODE vk)
{
	SetNextViewLocation();
}

void InitViewLocations(VKCODE vk)
{
	InitLocations();
}

void setup_no_cockpit_art()
{
	if ((Camera1.CameraMode == CAMERA_COCKPIT) || (CockpitSeat != NO_SEAT))
	{
		LoadCockpit(FRONT_NO_COCKPIT);
		CockpitSeat = NO_SEAT;
		SetupView(COCKPIT_NOART);//FRONT_NO_COCKPIT);
	}
}


void DebugMemory(VKCODE vk)
{
	char tempstr[80];

	MEMORYSTATUS  mStat = { sizeof(mStat) };

	GlobalMemoryStatus( &mStat );

	sprintf(tempstr,"memory load: %d total virt: %d avail virt: %d",
		mStat.dwMemoryLoad,
		mStat.dwTotalPageFile,
		mStat.dwAvailPageFile);

	AICAddAIRadioMsgs(tempstr, -1);
}

void DebugCountObjects(VKCODE vk)
{
#ifdef _DEBUG
	int i;
	double step_size = 83.0f*2.0*TILE_SIZE;
	FPointDouble point;
	char message[512];

//#ifdef _DEBUG
	i = CountObjects(3.0*step_size,step_size,&point);
	sprintf(message,"%i objects at %f east by %f south",i,point.X/(TILE_SIZE*2.0),point.Z/(TILE_SIZE*2.0));
	AICAddAIRadioMsgs(message, -2);
#endif

}

//extern unsigned char fadevar;//64; /// add key for testing
void DebugVarUp( VKCODE vk)
{
	//fadevar+=2;
}
void DebugVarDown( VKCODE vk)
{
	//fadevar-=2;
}


void ThrottleOff(VKCODE vk)
{
	KeyThrottleOff();
}
//***********************************************************************************
void ThrottleIdle(VKCODE vk)
{
	KeyThrottleIdle();
}
//***********************************************************************************
void ThrottleMil(VKCODE vk)
{
	KeyThrottleMil();
}
//***********************************************************************************
void ThrottleAB(VKCODE vk)
{
	KeyThrottleAB();
}
//***********************************************************************************
void ThrottleUp(VKCODE vk)
{
	KeyThrottleUp();
}
//***********************************************************************************
void ThrottleDown(VKCODE vk)
{
	KeyThrottleDown();
}
//***********************************************************************************
void toggle_tf(VKCODE vk)
{
	DoUFCMainTf();
}

//extern float GKValue;

void do_Aim9Select(VKCODE vk)
{
	SelectMediumOrShortRangeAAWeapon(AIM9_STAT);
}

void do_Aim7Select(VKCODE vk)
{
	SelectMediumOrShortRangeAAWeapon(AIM7_STAT);
}

void do_Aim120Select(VKCODE vk)
{
	SelectMediumOrShortRangeAAWeapon(AIM120_STAT);
}

void do_GunSelect(VKCODE vk)
{
  AAToggleGuns();
}

void do_MissleReject(VKCODE vk)
{
	MissleReject();
}

void do_IffInterrogate(VKCODE vk)
{
  InterrogateIffPlanes();
}

void do_IffNorm(VKCODE vk)
{
	DoUFCMainIff();
//  ToggleIffNorm();
}

void do_Aim9Scan(VKCODE vk)
{
  ToggleAim9Scan();
}

void hud_in_cmd(VKCODE vk)
{
	UFC.HudInCmd ^= 1;
}


void queue_betty(VKCODE vk)
{
	return;
	SndQueueSpeech (SIMSND_YAWRATE, CHANNEL_BETTY,g_iBaseBettySoundLevel);
}



void toggle_autopilot(VKCODE vk)
{
	AutoPilotOnOff();
}


void toggle_speedbreak(VKCODE vk)
{
	ToggleSpeedBrake();
}


extern VKCODE DisableGLimit;
void GLimitOverride(VKCODE vk)
{
	DisableGLimit = vk;
}

void toggle_radar(VKCODE vk)
{
}


void display_fps(VKCODE vk)
{
	DisplayFrameRate ^= 1;
}


void pause_sim(VKCODE vk)
{
	if (MultiPlayer)
#ifdef _DEBUG
		NetPutPause();
#else
	{
		if(!iAllowMultiPause)
		{
			return;
		}
		else
		{
			NetPutPause();
		}
	}
#endif

	SimPause ^= 1;
	if (!SimPause)
		JustLeftPausedState = 1;
	else
		StopFFForPauseOrEnd();
	KeySysMessageKey((int)SimPause);
}


void toggle_flaps(VKCODE vk)
{
	ToggleFlaps();
}


void toggle_gears(VKCODE vk)
{
	ToggleGear();
}

void toggle_hook(VKCODE vk)
{
	if(PlayerPlane->TailHookFlags & TH_HOOKED)
		return;

	ToggleHook();

	if(PlayerPlane->TailHookState == 0)
		SetCockpitItemState(123,0);
	else
		SetCockpitItemState(123,1);
}

void toggle_probe(VKCODE vk)
{
	ToggleRefuelingProbe();

}

void toggle_brakes(VKCODE vk)
{
	PlaneParams *P = PlayerPlane;

	if (P->OnGround)
	{
		if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			P->Brakes ^= 1;
			if (P->Brakes)
				DisplayWarning(BRAKE, ON, 1);
			else
			{
				DisplayWarning(BRAKE, OFF, 1);
				if(P->OnGround == 2)
				{
					aic_request_takeoff(vk);
				}
			}
		}
	}
	else
	{
		if (P->Brakes)
		{
			P->Brakes = 0;
			DisplayWarning(BRAKE, OFF, 1);
		}
	}
}


void prev_waypoint(VKCODE vk)
{
	HsiSteeringDown();
}


void next_waypoint(VKCODE vk)
{
//	PlaneParams *P;

//	P = PlayerPlane;
////	AINextWayPoint(P);				// advance to next waypoint
//  	AINextWayPointNoActions(P);
//	UFCAdvanceWaypointVisual();
	HsiSteeringUp();
}


void texture_detail(VKCODE vk)
{
	//AmountToTexture++;
	//AmountToTexture &= 3;
	FastLand ^= 1;
	KeySysMessageKey(FastLand);
}


void dither_detail(VKCODE vk)
{
	AmountToDither++;
	AmountToDither &= 3;
	KeySysMessageKey(AmountToDither);
}


void sun_angle_up(VKCODE vk)
{
#ifndef _DEBUG
	if(MultiPlayer)
		return;
#endif

#if 0
	float sun_angle;

	sun_angle = atan2(fabs(Sun.Y),-Sun.X);
	sun_angle += PI/128;

	if (sun_angle > PI)
		sun_angle = PI;

	Sun.X = (float)-cos(sun_angle);
	Sun.Y =(float)-fabs(sin(sun_angle));
	sun_angle = 1.0f - fabs(sin(sun_angle));
	sun_angle *= sun_angle * sun_angle;
	sun_angle = 1.0f - sun_angle;
	Set3DLighting(Sun.X, Sun.Y, Sun.Z, 1.0f, (float)sun_angle);
#else
	WorldParams.WorldTime += 600.0f;
	while(WorldParams.WorldTime >= (24.0f HOURS))
		WorldParams.WorldTime -= 24.0f HOURS;
#endif
}


void sun_angle_down(VKCODE vk)
{
#ifndef _DEBUG
	if(MultiPlayer)
		return;
#endif

#if 0
	float sun_angle;

	sun_angle = atan2(fabs(Sun.Y),-Sun.X);
	sun_angle -= PI/128;

	if (sun_angle < 0.0f)
		sun_angle = 0.0f;

	Sun.X = -cos(sun_angle);
	Sun.Y =-fabs(sin(sun_angle));
	sun_angle = 1.0f - fabs(sin(sun_angle));
	sun_angle *= sun_angle * sun_angle;
	sun_angle = 1.0f - sun_angle;
	Set3DLighting(Sun.X, Sun.Y, Sun.Z, 1.0f, (float)sun_angle);
#else
	WorldParams.WorldTime -= 600.0f;
	while(WorldParams.WorldTime < 0.0f)
		WorldParams.WorldTime += 24.0f HOURS;
#endif
}



void do_screen_capture(VKCODE vk)
{
	ScreenCapture();
//	SndQueueSound(SIMSND_WARNING,1);
	SndQueueSound(304, 1);  //  same beep as multiplayer chat notification
}

#pragma warning( 3: 700 )
#pragma check_stack+

void do_damage_plane(VKCODE vk)
{
	PlaneParams *P;


	FPointDouble Pos;

	P = (PlaneParams *)Camera1.AttachedObject;

	Pos = Camera1.CameraLocation;
	Pos.AddScaledVector( 1000 FEET, ViewVector);



	static type;


	FPoint vel( 0.0f);

	//NewCanister( CT_OIL_FIRE,	  Pos, vel, 0.0f );
	//NewCanister( CT_BLACK_SMOKE,	  Pos, vel, 0.0f );
	//NewCanister( CT_STEAM_SMOKE,	  Pos, vel, 0.0f );
	//NewCanister( CT_BURNOFF_SMOKE,	  Pos, vel, 0.0f );
	//NewCanister( CT_WELLHEAD_FIRE,	  Pos, vel, 0.0f );

	//NewCanisterExplosion( CT_SPIDER_SMOKE2, Pos, frand(), 0.0f, NULL);

	//NewCanisterExplosion( CT_EXPLOSION_WATER, Pos, frand(), 0.0f, NULL);
	//NewCanisterExplosion( CT_EXPLOSION_FIRE, Pos, 1.0, 0.0f, NULL);

	//WeaponType weaptype;
	DBWeaponType *pweapon;

	//weaptype.TypeNumber = 101;
	//LoadWeaponModel(&weaptype);

	pweapon= &pDBWeaponList[AIGetWeaponIndex(101)];

	if( pweapon)
		NewExplosionWeapon( pweapon, Pos);

/*


	if (Camera1.AttachedObjectType == CO_PLANE)
	{
		P = (PlaneParams *)Camera1.AttachedObject;

		if (P->Smoke[SMOKE_ENGINE_REAR_LEFT])
		{
			if (P->Smoke[SMOKE_ENGINE_REAR_RIGHT])
			{
				MakeOrphan(P->Smoke[SMOKE_ENGINE_REAR_LEFT]);
				P->Smoke[SMOKE_ENGINE_REAR_LEFT] = NULL;
			}
			else
				CrashPlane(P,0,PL_ENGINE_REAR_RIGHT);
		}
		else
		{
			if (P->Smoke[SMOKE_ENGINE_REAR_RIGHT])
			{
				MakeOrphan(P->Smoke[SMOKE_ENGINE_REAR_RIGHT]);
				P->Smoke[SMOKE_ENGINE_REAR_RIGHT] = NULL;
			}
			else
				CrashPlane(P,0,PL_ENGINE_REAR_LEFT);
		}
	}
	*/
}

void increase_time(VKCODE vk)
{
#ifdef _DEBUG
	if (MultiPlayer)
		return;
#else
	if ((MultiPlayer) ||
//		((PlayerPlane->AGL < 20) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED)))
	((PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED)))

		return;
#endif

	TimeExcel++;

	if (TimeExcel > 3)
		TimeExcel = 3;
}


void decrease_time(VKCODE vk)
{
	if ((MultiPlayer))
//		||
//		((PlayerPlane->AGL < 20) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED)))
		return;

	TimeExcel--;

	if (TimeExcel < 0)
		TimeExcel = 0;
}

extern AvRGB *HUDColorRGB;
extern AvRGB HUDColorList[11];

void do_hud_color(VKCODE vk)
{
	 // old F15 code
	//CurrentHudColorIndex++;
	//if (CurrentHudColorIndex >= MAX_HUD_COLOR)
	 //	CurrentHudColorIndex = 0;

	//HUDColor = HUDColorChoices[CurrentHudColorIndex];


 HUDColorRGB++;
 if(HUDColorRGB - &HUDColorList[0] > 10)
	HUDColorRGB = &HUDColorList[0];
}

void do_msg_color(VKCODE vk)
{
// old F15 code
	CurrentHudColorIndex++;
	if (CurrentHudColorIndex >= MAX_HUD_COLOR)
		CurrentHudColorIndex = 0;

	HUDColor = HUDColorChoices[CurrentHudColorIndex];
}

void do_debug_hit(VKCODE vk)
{
	PauseForDiskHit();
	if(lpDD)
	  lpDD->FlipToGDISurface();
	DebugBreak();
	UnPauseFromDiskHit();
}

void confirm_done(VKCODE vk)
{
	iEndGameState = 4;
	GetAsyncKeyState(VK_ESCAPE);
	GetAsyncKeyState(VK_RETURN);
	StopFFForPauseOrEnd();

	OldCockpitSeat    = CockpitSeat;
	OldCurrentCockpit = CurrentCockpit;
	memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));
	OldCursor = CursorIsVisible;
	OldInteract = InteractCockpitMode ;

	if(MultiPlayer)
		NetSendPlayerExit(PlayerPlane);
}

void do_sim_done(VKCODE vk)
{
	StopFFForPauseOrEnd();
	SndStreamClose();
	PostMessage (g_hGameWnd, CWM_SIM_DONE, 0, 0L);
}


void set_acq_wide(VKCODE vk)
{
 if(!WeapStores.GunsOn)
   SetAcqMode(AA_WIDE_ACQ_MODE);
}

void set_acq_bst(VKCODE vk)
{
 if(!WeapStores.GunsOn)
  SetAcqMode(AA_BORE_SIGHT_MODE);
}

void set_acq_vertical(VKCODE vk)
{
 if(!WeapStores.GunsOn)
	SetAcqMode(AA_VERTICAL_ACQ_MODE);
}

void refuel_Player_plane(VKCODE vk)
{
	if (MultiPlayer)  return;

	RefuelFuelPlane(PlayerPlane);
	LogMissionStatistic(LOG_FLIGHT_CHEATED,1,1,2);
}

int DamageNum = 0;

// JLM testing remove code

void drop_chaff(VKCODE vk)
{
	DropSingleChaff();
}

void drop_flare(VKCODE vk)
{
	DropSingleFlare();
}

void master_mode_step(VKCODE vk)
{
	StepThroughMasterModes();
}


void ufc_inst_mode(VKCODE vk)
{
	UFCSetINSTMode();
}

void ufc_nav_mode(VKCODE vk)
{
	UFCSetNAVMode();
}


void ufc_ag_mode(VKCODE vk)
{
	UFCSetAGMode();
}


void ufc_aa_mode(VKCODE vk)
{
	UFCSetAAMode();
}


//  Wingman (and other) communication functions
void aic_w_engage_bandits(VKCODE vk)
{
	SendNetKey(6, '1');

	AICWEngageBandits();
}

void aic_e_engage_bandits(VKCODE vk)
{
	SendNetKey(5, '1');

	AICEEngageBandits();
}

void aic_d_engage_bandits(VKCODE vk)
{
	SendNetKey(4, '1');

	AICDEngageBandits();
}

void aic_f_engage_bandits(VKCODE vk)
{
	SendNetKey(3, '1');

	AICFEngageBandits();
}

void aic_w_rtb(VKCODE vk)
{
	SendNetKey(6, '0');

	AICW_RTB();
}

void aic_e_rtb(VKCODE vk)
{
	SendNetKey(5, '9');

	AICE_RTB();
}

void aic_d_rtb(VKCODE vk)
{
	SendNetKey(4, '9');

	AICD_RTB();
}

void aic_f_rtb(VKCODE vk)
{
	SendNetKey(3, '0');

	AICF_RTB();
}

void aic_d_sanitize_right(VKCODE vk)
{
	SendNetKey(4, '2');

	AIDSanitizeRight();
}

void aic_e_sanitize_right(VKCODE vk)
{
	SendNetKey(5, '2');

	AIESanitizeRight();
}

void aic_w_sanitize_right(VKCODE vk)
{
	SendNetKey(6, '2');

	AIWSanitizeRight();
}

void aic_d_sanitize_left(VKCODE vk)
{
	SendNetKey(4, '3');

	AIDSanitizeLeft();
}

void aic_e_sanitize_left(VKCODE vk)
{
	SendNetKey(5, '3');

	AIESanitizeLeft();
}

void aic_w_sanitize_left(VKCODE vk)
{
	SendNetKey(6, '3');
	AIWSanitizeLeft();
}

void aic_f_radar_toggle(VKCODE vk)
{
	SendNetKey(3, '2');

	AICRadarToggle();
}

void aic_f_jammer_toggle(VKCODE vk)
{
	SendNetKey(3, '3');

	AICJammingToggle();
}

void aic_flight_status(VKCODE vk)
{
	SendNetKey(8, '1');

	AICFlightStatus();
}

void aic_flight_weapon_check(VKCODE vk)
{
	SendNetKey(8, '2');

	AICFlightWeaponCheck();
}

void aic_flight_position_check(VKCODE vk)
{
	SendNetKey(8, '3');

	AICFlightPositionCheck();
}

void aic_flight_fuel_check(VKCODE vk)
{
	SendNetKey(8, '4');

	AICFlightFuelCheck();
}

void aic_v_formation(VKCODE vk)
{
	SendNetKey(9, '3');

	AICFormationChange(2);
}

void aic_wedge_formation(VKCODE vk)
{
	SendNetKey(9, '4');

	AICWedgeFormationChange();
}

void aic_line_abreast_formation(VKCODE vk)
{
	SendNetKey(9, '5');

	AICFormationChange(9);
}

void aic_echelon_formation(VKCODE vk)
{
	SendNetKey(9, '6');

	AICFormationChange(10);
}

void aic_trail_formation(VKCODE vk)
{
	SendNetKey(9, '7');

	AICFormationChange(7);
}

void aic_wall_formation(VKCODE vk)
{
	SendNetKey(9, '8');

	AICFormationChange(9);
}

void aic_formation_loosen(VKCODE vk)
{
	SendNetKey(9, '1');

	AICFormationLoosen();
}

void aic_formation_tighten(VKCODE vk)
{
	SendNetKey(9, '2');

	AICFormationTighten();
}

void aic_w_rejoin_flight(VKCODE vk)
{
	SendNetKey(6, '9');
	AICWRejoinFlight();
}

void aic_e_rejoin_flight(VKCODE vk)
{
	SendNetKey(5, '8');

	AICERejoinFlight();
}

void aic_d_rejoin_flight(VKCODE vk)
{
	SendNetKey(4, '8');
	if(MultiPlayer)
	{
		DPMSG_AI_COMMS	NetAIComms;

		NetAIComms.type   = NET_AI_COMMS;
		NetAIComms.time   = newTime;
		NetAIComms.slot   = MySlot;
		NetAIComms.keyflag = 0;
		NetAIComms.wParam = '8';
		NetAIComms.commid = 4;
		NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAIComms, sizeof(NetAIComms) );
	}

	AICDRejoinFlight();
}

void aic_f_rejoin_flight(VKCODE vk)
{
	SendNetKey(3, '9');

	AICFRejoinFlight();
}

void aic_w_all_dumb(VKCODE vk)
{
	SendNetKey(19, '1');

	AICWWeaponOptionUnguidedAll();
}

void aic_e_all_dumb(VKCODE vk)
{
	SendNetKey(20, '1');

	AICEWeaponOptionUnguidedAll();
}

void aic_d_all_dumb(VKCODE vk)
{
	SendNetKey(21, '1');

	AICDWeaponOptionUnguidedAll();
}

void aic_f_all_dumb(VKCODE vk)
{
	SendNetKey(22, '1');

	AICFWeaponOptionUnguidedAll();
}

void aic_w_half_dumb(VKCODE vk)
{
	SendNetKey(19, '2');

	AICWWeaponOptionUnguidedHalf();
}

void aic_e_half_dumb(VKCODE vk)
{
	SendNetKey(20, '2');

	AICEWeaponOptionUnguidedHalf();
}

void aic_d_half_dumb(VKCODE vk)
{
	SendNetKey(21, '2');

	AICDWeaponOptionUnguidedHalf();
}

void aic_f_half_dumb(VKCODE vk)
{
	SendNetKey(22, '2');

	AICFWeaponOptionUnguidedHalf();
}

void aic_w_single_dumb(VKCODE vk)
{
	SendNetKey(19, '3');

	AICWWeaponOptionUnguidedSingle();
}

void aic_e_single_dumb(VKCODE vk)
{
	SendNetKey(20, '3');

	AICEWeaponOptionUnguidedSingle();
}

void aic_d_single_dumb(VKCODE vk)
{
	SendNetKey(21, '3');

	AICDWeaponOptionUnguidedSingle();
}

void aic_f_single_dumb(VKCODE vk)
{
	SendNetKey(22, '3');

	AICFWeaponOptionUnguidedSingle();
}

void aic_w_all_guided(VKCODE vk)
{
	SendNetKey(19, '4');

	AICWWeaponOptionGuidedAll();
}

void aic_e_all_guided(VKCODE vk)
{
	SendNetKey(20, '4');

	AICEWeaponOptionGuidedAll();
}

void aic_d_all_guided(VKCODE vk)
{
	SendNetKey(21, '4');

	AICDWeaponOptionGuidedAll();
}

void aic_f_all_guided(VKCODE vk)
{
	SendNetKey(22, '4');

	AICFWeaponOptionGuidedAll();
}

void aic_w_half_guided(VKCODE vk)
{
	SendNetKey(19, '5');

	AICWWeaponOptionGuidedHalf();
}

void aic_e_half_guided(VKCODE vk)
{
	SendNetKey(20, '5');

	AICEWeaponOptionGuidedHalf();
}

void aic_d_half_guided(VKCODE vk)
{
	SendNetKey(21, '5');

	AICDWeaponOptionGuidedHalf();
}

void aic_f_half_guided(VKCODE vk)
{
	SendNetKey(22, '5');

	AICFWeaponOptionGuidedHalf();
}

void aic_w_single_guided(VKCODE vk)
{
	SendNetKey(19, '6');

	AICWWeaponOptionGuidedSingle();
}

void aic_e_single_guided(VKCODE vk)
{
	SendNetKey(20, '6');

	AICEWeaponOptionGuidedSingle();
}

void aic_d_single_guided(VKCODE vk)
{
	SendNetKey(21, '6');

	AICDWeaponOptionGuidedSingle();
}

void aic_f_single_guided(VKCODE vk)
{
	SendNetKey(22, '6');

	AICFWeaponOptionGuidedSingle();
}

void aic_w_all_missile(VKCODE vk)
{
	SendNetKey(19, '7');

	AICWWeaponOptionMissileAll();
}

void aic_e_all_missile(VKCODE vk)
{
	SendNetKey(20, '7');

	AICEWeaponOptionMissileAll();
}

void aic_d_all_missile(VKCODE vk)
{
	SendNetKey(21, '7');

	AICDWeaponOptionMissileAll();
}

void aic_f_all_missile(VKCODE vk)
{
	SendNetKey(22, '7');

	AICFWeaponOptionMissileAll();
}

void aic_w_half_missile(VKCODE vk)
{
	SendNetKey(19, '8');

	AICWWeaponOptionMissileHalf();
}

void aic_e_half_missile(VKCODE vk)
{
	SendNetKey(20, '8');

	AICEWeaponOptionMissileHalf();
}

void aic_d_half_missile(VKCODE vk)
{
	SendNetKey(21, '8');

	AICDWeaponOptionMissileHalf();
}

void aic_f_half_missile(VKCODE vk)
{
	SendNetKey(22, '8');

	AICFWeaponOptionMissileHalf();
}

void aic_w_single_missile(VKCODE vk)
{
	SendNetKey(19, '9');

	AICWWeaponOptionMissileSingle();
}

void aic_e_single_missile(VKCODE vk)
{
	SendNetKey(20, '9');

	AICEWeaponOptionMissileSingle();
}

void aic_d_single_missile(VKCODE vk)
{
	SendNetKey(21, '9');

	AICDWeaponOptionMissileSingle();
}

void aic_f_single_missile(VKCODE vk)
{
	SendNetKey(22, '9');

	AICFWeaponOptionMissileSingle();
}

void aic_w_default_weapon(VKCODE vk)
{
	SendNetKey(19, '0');

	AICWWeaponOptionDefault();
}

void aic_e_default_weapon(VKCODE vk)
{
	SendNetKey(20, '0');

	AICEWeaponOptionDefault();
}

void aic_d_default_weapon(VKCODE vk)
{
	SendNetKey(21, '0');

	AICDWeaponOptionDefault();
}

void aic_f_default_weapon(VKCODE vk)
{
	SendNetKey(22, '0');

	AICFWeaponOptionDefault();
}

void aic_w_grnd_attack_prime(VKCODE vk)
{
	SendNetKey(16, '1');
	AICWGrndAttackPrime();
}

void aic_e_grnd_attack_prime(VKCODE vk)
{
	SendNetKey(13, '1');
	AICEGrndAttackPrime();
}

void aic_d_grnd_attack_prime(VKCODE vk)
{
	SendNetKey(11, '1');
	AICDGrndAttackPrime();
}

void aic_f_grnd_attack_prime(VKCODE vk)
{
	SendNetKey(7, '1');

	AICFGrndAttackPrime();
}

void aic_w_grnd_attack_second(VKCODE vk)
{
	SendNetKey(16, '2');
	AICWGrndAttackSecond();
}

void aic_e_grnd_attack_second(VKCODE vk)
{
	SendNetKey(13, '2');
	AICEGrndAttackSecond();
}

void aic_d_grnd_attack_second(VKCODE vk)
{
	SendNetKey(11, '2');
	AICDGrndAttackSecond();
}

void aic_f_grnd_attack_second(VKCODE vk)
{
	SendNetKey(7, '2');
	AICFGrndAttackSecond();
}

void aic_w_grnd_attack_opportunity(VKCODE vk)
{
	SendNetKey(16, '3');
	AICWGrndAttackOpportunity();
}

void aic_e_grnd_attack_opportunity(VKCODE vk)
{
	SendNetKey(13, '3');
	AICEGrndAttackOpportunity();
}

void aic_d_grnd_attack_opportunity(VKCODE vk)
{
	SendNetKey(11, '3');
	AICDGrndAttackOpportunity();
}

void aic_f_grnd_attack_opportunity(VKCODE vk)
{
	SendNetKey(7, '3');
	AICFGrndAttackOpportunity();
}

void aic_w_grnd_attack_sead(VKCODE vk)
{
	SendNetKey(16, '4');
	AICWGrndAttackSEAD();
}

void aic_e_grnd_attack_sead(VKCODE vk)
{
	SendNetKey(13, '4');
	AICEGrndAttackSEAD();
}

void aic_d_grnd_attack_sead(VKCODE vk)
{
	SendNetKey(11, '4');
	AICDGrndAttackSEAD();
}

void aic_f_grnd_attack_sead(VKCODE vk)
{
	SendNetKey(7, '4');
	AICFGrndAttackSEAD();
}

void aic_f_report_contacts(VKCODE vk)
{
	SendNetKey(3, '4');

	AICAllReportContacts();
}

void aic_d_cover(VKCODE vk)
{
	SendNetKey(4, '4');

	AIDCover();
}

void aic_e_cover(VKCODE vk)
{
	SendNetKey(5, '4');

	AIECover();
}

void aic_w_cover(VKCODE vk)
{
	SendNetKey(6, '4');

	AIWCover();
}

void aic_d_orbit_here(VKCODE vk)
{
	SendNetKey(4, '5');

	AIDOrbit();
}

void aic_e_orbit_here(VKCODE vk)
{
	SendNetKey(5, '5');

	AIEOrbit();
}

void aic_w_orbit_here(VKCODE vk)
{
	SendNetKey(6, '5');

	AIWOrbit();
}

void aic_w_bracket_right(VKCODE vk)
{
	SendNetKey(15, '1');

	AICWBracketRight();
}

void aic_e_bracket_right(VKCODE vk)
{
	SendNetKey(12, '1');

	AICEBracketRight();
}

void aic_d_bracket_right(VKCODE vk)
{
	SendNetKey(10, '1');

	AICDBracketRight();
}

void aic_w_bracket_left(VKCODE vk)
{
	SendNetKey(15, '2');

	AICWBracketLeft();
}

void aic_e_bracket_left(VKCODE vk)
{
	SendNetKey(12, '2');

	AICEBracketLeft();
}

void aic_d_bracket_left(VKCODE vk)
{
	SendNetKey(10, '2');

	AICDBracketLeft();
}

void aic_w_split_high(VKCODE vk)
{
	SendNetKey(15, '3');

	AICWSplitHigh();
}

void aic_e_split_high(VKCODE vk)
{
	SendNetKey(12, '3');

	AICESplitHigh();
}

void aic_d_split_high(VKCODE vk)
{
	SendNetKey(10, '3');

	AICDSplitHigh();
}

void aic_w_split_low(VKCODE vk)
{
	SendNetKey(15, '4');

	AICWSplitLow();
}

void aic_e_split_low(VKCODE vk)
{
	SendNetKey(12, '4');

	AICESplitLow();
}

void aic_d_split_low(VKCODE vk)
{
	SendNetKey(10, '4');

	AICDSplitLow();
}

void aic_w_drag_right(VKCODE vk)
{
	SendNetKey(15, '5');

	AICWDragRight();
}

void aic_e_drag_right(VKCODE vk)
{
	SendNetKey(12, '5');

	AICEDragRight();
}

void aic_d_drag_right(VKCODE vk)
{
	SendNetKey(10, '5');

	AICDDragRight();
}

void aic_w_drag_left(VKCODE vk)
{
	SendNetKey(15, '6');

	AICWDragLeft();
}

void aic_e_drag_left(VKCODE vk)
{
	SendNetKey(12, '6');

	AICEDragLeft();
}

void aic_d_drag_left(VKCODE vk)
{
	SendNetKey(10, '6');

	AICDDragLeft();
}

void aic_w_break_right(VKCODE vk)
{
	SendNetKey(14, '1');

	AIBreakRight();
}

void aic_w_break_left(VKCODE vk)
{
	SendNetKey(14, '2');

	AIBreakLeft();
}

void aic_w_break_high(VKCODE vk)
{
	SendNetKey(14, '3');

	AIBreakHigh();
}

void aic_w_break_low(VKCODE vk)
{
	SendNetKey(14, '4');

	AIBreakLow();
}

void aic_request_picture(VKCODE vk)
{
	SendNetKey(17, '1');

	AICRequestPlayerPicture();
}

void aic_request_bogey_dope(VKCODE vk)
{
	SendNetKey(17, '2');

	AICRequestPlayerBogieDope();
}

void aic_request_cover(VKCODE vk)
{
	SendNetKey(17, '3');

	AICRequestPlayerCover();
}

void aic_request_sead(VKCODE vk)
{
	SendNetKey(17, '4');

	AICRequestPlayerSEAD();
}

void aic_request_sar(VKCODE vk)
{
	SendNetKey(1, '5');

	AICPlayerRequestSAR();
}

void aic_alpha_bullseye(VKCODE vk)
{
	SendNetKey(31, '1');

	AICAlphaCheck(PlayerPlane - Planes, 0);
}

void aic_alpha_ip(VKCODE vk)
{
	SendNetKey(31, '2');

	AICAlphaCheck(PlayerPlane - Planes, 1);
}

void aic_alpha_target(VKCODE vk)
{
	SendNetKey(31, '3');

	AICAlphaCheck(PlayerPlane - Planes, 2);
}

void aic_alpha_package(VKCODE vk)
{
	SendNetKey(31, '4');

	AICAlphaCheck(PlayerPlane - Planes, 3);
}

void aic_alpha_homeplate(VKCODE vk)
{
	SendNetKey(31, '5');

	AICAlphaCheck(PlayerPlane - Planes, 4);
}

void aic_alpha_tanker(VKCODE vk)
{
	SendNetKey(31, '6');

	AICAlphaCheck(PlayerPlane - Planes, 5);
}

void aic_alpha_divert_field(VKCODE vk)
{
	SendNetKey(31, '7');

	AICAlphaCheck(PlayerPlane - Planes, 6);
}

void aic_jstars_check_in_out(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

	SendNetKey(23, '1');

	if(lBombFlags & WSO_JSTARS_CHECK)
	{
		AICPlayerJSTARSCheckOut();
	}
	else
	{
		AICPlayerJSTARSCheckIn();
	}
}

void aic_jstars_target_loc(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

	SendNetKey(23, '2');

	AIC_JSTARS_Give_Target_Loc();
}

void aic_jstars_next_target(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

	SendNetKey(23, '3');

	AIC_JSTARS_Give_Next_Target_Loc();
}

void aic_tanker_contact(VKCODE vk)
{
	if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
	{
		SendNetKey(24, '1');

		AICPlayerContactRefueler();
	}
}

void aic_tanker_contact_max_trap(VKCODE vk)
{
	if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
	{
		SendNetKey(24, '2');

		AICPlayerContactRefuelerMaxTrap();
	}
}

void aic_tanker_contact_4K(VKCODE vk)
{
	if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
	{
		SendNetKey(24, '3');

		AICPlayerContactRefueler4K();
	}
}

void aic_request_landing(VKCODE vk)
{
	if(PlayerPlane->OnGround)
		return;

	SendNetKey(25, '1');
	AICPlayerRequestLanding();
}

void aic_request_takeoff(VKCODE vk)
{
	SendNetKey(25, '2');
	AICPlayerRequestTakeOff();
}

void aic_msg_like_hud_toggle(VKCODE vk)
{
	if(iRadioDisplayFlags & 2)
		iRadioDisplayFlags &= ~2;
	else
		iRadioDisplayFlags |= 2;
}

void aic_msg_history_toggle(VKCODE vk)
{
	if(iRadioDisplayFlags & 1)
		iRadioDisplayFlags &= ~1;
	else
		iRadioDisplayFlags |= 1;
}

void aic_msg_subtitle_toggle(VKCODE vk)
{
	if(iRadioDisplayFlags & 4)
		iRadioDisplayFlags &= ~4;
	else
		iRadioDisplayFlags |= 4;
}

void aic_next_msg_text_color(VKCODE vk)
{
	AICNextTextColor();
}

void aic_next_msg_box_color(VKCODE vk)
{
	AICNextBoxColor();
}

void pad_WSO_warn(VKCODE vk)
{
	PLWatchWSOWarn();
}

void pad_agdesignated(VKCODE vk)
{
	PLWatchAGDesignated();
}

void pad_next_center_obj(VKCODE vk)
{
	PLGetNextObjectToCenter();
}


void ToggleCockpitInteraction( VKCODE vk)
{
 	if( InteractCockpitMode)
		InteractCockpitMode = 0;
	else
		InteractCockpitMode = 1;
}

void ToggleMpdPopUps(VKCODE vk)
{
	vkVirtualMPD = vk;

	//if (g_Settings.misc.dwMisc & GP_MISC_VIEW_MPDS_POPUPS_STICKY)
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & (COCKPIT_NOART |COCKPIT_VIRTUAL_SEAT) ))
		{
			ToggleVirtualMpds();
			/*
			if (!DrawPopMpds)
			{
				ToggleVirtualMpds();
				OurShowCursor( TRUE );
			}
			else
			{
				ToggleVirtualMpds();

				LONG x = GetSystemMetrics(SM_CXSCREEN)>>1;
				LONG y = GetSystemMetrics(SM_CYSCREEN)>>1;

				SetCursorPos(x, y);
				ppLastMouse.x = x;
				ppLastMouse.y = y;
				ppCurrMouse.x = 0;
				ppCurrMouse.y = 0;

				OurShowCursor( FALSE );
			}
			*/
		}
	}

}

void auto_tanking_toggle(VKCODE vk)
{
	PlaneParams *tanker;

	if(PlayerPlane->Status & AL_DEVICE_DRIVEN)
	{
		tanker = AICheckTankerCloseBy(PlayerPlane);

		if((PlayerPlane->AI.Behaviorfunc != AIFlyTankerFormation) || (!tanker))
		{
			return;
		}

		PlayerPlane->AI.fVarA[0] = PlayerPlane->AI.lVar2;

		LogMissionStatistic(LOG_FLIGHT_AUTOREFUELED,1,1,2);

		PlayerPlane->Status &= ~AL_DEVICE_DRIVEN;
		PlayerPlane->Status |= AL_AI_DRIVEN;
//		AICAddAIRadioMsgs("AI", 50);
		PlayerPlane->DoControlSim = CalcF18ControlSurfacesSimple;
		PlayerPlane->DoCASSim = CalcF18CASUpdatesSimple;
		PlayerPlane->DoForces = CalcAeroForcesSimple;
		PlayerPlane->DoPhysics = CalcAeroDynamicsSimple;

		PlayerPlane->MaxPitchRate = 10.0;
		PlayerPlane->MaxRollRate = 90.0;
		PlayerPlane->YawRate = 15.0;

		PlayerPlane->MaxSpeedAccel = 20;	//15
		PlayerPlane->SpeedPercentage = 1.0;

		PlayerPlane->BfLinVel.X = PlayerPlane->V;
		PlayerPlane->BfLinVel.Y = 0;
		PlayerPlane->BfLinVel.Z = 0;

		if(MultiPlayer)
		{
			NetPutGenericMessage2(PlayerPlane, GM2_AUTOTANK_2, (tanker - Planes));
		}
	}
	else
	{
		if(MultiPlayer)
		{
			NetPutGenericMessage1(PlayerPlane, GM_END_CASUAL_AUTO);
		}

		EndCasualAutoPilot();
	}
}

void prepare_for_lightspeed(VKCODE vk)
{
	if (MultiPlayer)  return;

	if(!(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		SetUpForPlayerJump();
	}
}

void wso_bomb_left(VKCODE vk)
{
	lBombFlags |= WSO_TURN_BOMB_L;
}

void wso_bomb_right(VKCODE vk)
{
	lBombFlags |= WSO_TURN_BOMB_R;
}

void wso_bomb_long(VKCODE vk)
{
	lBombFlags |= WSO_BOMB_LONGER;
}

void wso_bomb_short(VKCODE vk)
{
	lBombFlags |= WSO_BOMB_SHORTER;
}

void wso_toggle_jammer(VKCODE vk)
{
	ToggleICS();
}


void radio_channel1(VKCODE vk)
{
	UFC.UFCClear = 2;
	NetCycleChannel(1);
}


void radio_channel2(VKCODE vk)
{
	UFC.UFCClear = 2;
	NetCycleChannel(2);
}


void radio_multichat1(VKCODE vk)
{
	XmitChannel = 1;
}


void radio_multichat2(VKCODE vk)
{
	XmitChannel = 2;
}


void radio_wingmen(VKCODE vk)
{
	if (MultiPlayer)
		radio_multichat1(vk);
	else
	{
		int menuedge = 10;
		gAICommMenu.AICommMenufunc = AICShowGroupCommOps;
		gAICommMenu.lMaxStringPixelLen = AICPixelLenGroupCommOps() + menuedge;
		gAICommMenu.AICommKeyfunc = AICGroupCommKeyOps;
		gAICommMenu.lTimer = AICOMMDISPLAY;
		AICSetUpForCommKeys();
	}
}


void radio_others(VKCODE vk)
{
	if (MultiPlayer)
		radio_multichat2(vk);
	else
	{
		int menuedge = 10;

		gAICommMenu.AICommMenufunc = AICShowOtherCommOps;
		gAICommMenu.lMaxStringPixelLen = AICPixelLenOtherCommOps() + menuedge;
		gAICommMenu.AICommKeyfunc = AICOtherCommKeyOps;
		gAICommMenu.lTimer = AICOMMDISPLAY;
		AICSetUpForCommKeys();
	}
}

void radio_AI_wingmen(VKCODE vk)
{
	int menuedge = 10;

	gAICommMenu.AICommMenufunc = AICShowGroupCommOps;
	gAICommMenu.lMaxStringPixelLen = AICPixelLenGroupCommOps() + menuedge;
	gAICommMenu.AICommKeyfunc = AICGroupCommKeyOps;
	gAICommMenu.lTimer = AICOMMDISPLAY;
	AICSetUpForCommKeys();
}


void radio_AI_others(VKCODE vk)
{
	int menuedge = 10;

	gAICommMenu.AICommMenufunc = AICShowOtherCommOps;
	gAICommMenu.lMaxStringPixelLen = AICPixelLenOtherCommOps() + menuedge;
	gAICommMenu.AICommKeyfunc = AICOtherCommKeyOps;
	gAICommMenu.lTimer = AICOMMDISPLAY;
	AICSetUpForCommKeys();
}

void comm_option_cancel(VKCODE vk)
{
	gAICommMenu.AICommMenufunc = NULL;
	gAICommMenu.AICommKeyfunc = NULL;
	gAICommMenu.lTimer = -1;
}

void aic_w_help(VKCODE vk)
{
	SendNetKey(14, '6');

	AICWHelpMe();
}

void aic_w_attack_my_targ(VKCODE vk)
{
	SendNetKey(14, '5');

	AICWAttackMyTarget();
}

void aic_f_sort_bandits(VKCODE vk)
{
	SendNetKey(3, '5');

	AICFSortBandits();
}

void debug_radar_in(VKCODE vk)
{
	char tempstr[180];

	iAIShowRadarRange >>= 1;
	sprintf(tempstr, "%d", iAIShowRadarRange);
	AICAddAIRadioMsgs(tempstr, 40);
}

void debug_radar_out(VKCODE vk)
{
	char tempstr[180];

//	if (MultiPlayer)
//		return;

	if(iAIShowRadarRange >= 5)
	{
		iAIShowRadarRange <<= 1;
		if(iAIShowRadarRange > 80)
		{
			iAIShowRadarRange = 80;
		}
	}
	else
	{
		iAIShowRadarRange = 5;
	}
	sprintf(tempstr, "%d", iAIShowRadarRange);
	AICAddAIRadioMsgs(tempstr, 40);
}

void do_RotateObjectComplexity(VKCODE vk)
{
	int i;
	i = GetHighest3DLOD();
	if (i == 2)
		i = 0;
	else
		i++;
	SetHighest3DLOD(i);
	KeySysMessageKey(2-i);
	ReloadNimitzClassCarriers();
}

void do_RotateSceneComplexity(VKCODE vk)
{
	AmountToObject += BI_DETAIL_1;
	AmountToObject &= BI_DETAIL;
//	if (AmountToObject == BI_DETAIL_1)
//		AmountToObject = BI_DETAIL_1;

//	if (MediumTextureCache)
//		MediumTextureCache->UpdateObjects();

	SimpleTerrainLoaded = 0;
	KeySysMessageKey(AmountToObject);
}

void do_ToggleRoads(VKCODE vk)
{
	TerrainYesNos ^= YN_DRAW_ROADS;
	KeySysMessageKey(TerrainYesNos & YN_DRAW_ROADS);
}

void do_ToggleShadows(VKCODE vk)
{
	TerrainYesNos ^= YN_DRAW_SHADOWS;
	KeySysMessageKey(TerrainYesNos & YN_DRAW_SHADOWS);
}

void do_ToggleSpectralLighting(VKCODE vk)
{
	TerrainYesNos ^= YN_NO_SPECTRAL;
	KeySysMessageKey(TerrainYesNos & YN_NO_SPECTRAL);
}

void do_ToggleTransitions(VKCODE vk)
{
	TerrainYesNos ^= YN_NO_TRANSITIONS;
	KeySysMessageKey((TerrainYesNos & YN_NO_TRANSITIONS)^YN_NO_TRANSITIONS);
}

void do_ToggleShinyWater(VKCODE vk)
{
	TerrainYesNos ^= YN_DRAW_SPARKLES;
}

void do_ToggleDetailMapping(VKCODE vk)
{
	TerrainYesNos ^= YN_DRAW_DETAIL_MAPS;
	TerrainYesNos ^= YN_DRAW_WATER_SURFACE;
}

void do_ToggleLightMaps(VKCODE vk)
{
	TerrainYesNos ^= YN_DRAW_LIGHT_MAPS;
}

void do_TogglePointLights(VKCODE vk)
{
	TerrainYesNos ^= YN_DRAW_POINT_LIGHTS;
}


void do_RotateViewDistance(VKCODE vk)
{
	AmountToDraw++;
	if (AmountToDraw > DETAIL_NORMAL)
		AmountToDraw = DETAIL_HIGH_ONLY;
	KeySysMessageKey(AmountToDraw);
}

void Do_BreakAALock(VKCODE vk)
{
  if(UFC.MasterMode == AA_MODE)
  {
		AABreakLock();
	}
  else if(UFC.MasterMode == AG_MODE)
  {
		BreakAGLock();
  }
}

extern int AARadarShowCrossSig;

void Do_ShowCrossSigInfo(VKCODE vk)
{
  AARadarShowCrossSig = !AARadarShowCrossSig;
}

void Do_EasyModeChangeWeapon(VKCODE vk)
{
  //EasyModeChangeWeapon();
  SelectNextWeapon();
}

void Do_EasyModeChangeTarget(VKCODE vk)
{
 SelectNextTarget();
}

void Do_EasyModeFirstTarget(VKCODE vk)
{
   if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
      EasyModeAGResetTarget();
}

void toggle_cockpit_light(VKCODE vk)
{
	if (Camera1.CameraMode == CAMERA_COCKPIT)
		DoRightDownSwitch();
}

void Do_NextAGTarget(VKCODE vk)
{
	SelectNextAGTarget();
}

void Do_PrevAGTarget(VKCODE vk)
{
	SelectPrevAGTarget();
}

void Do_FirstAGTarget(VKCODE vk)
{
	SelectFirstAGTarget();
}

	/* ------------------------------------------2/3/98 4:40PM---------------------------------------------
 * View functionality
 * ----------------------------------------------------------------------------------------------------*/

/* ------------------------------------------2/3/98 4:40PM---------------------------------------------
 * Camera Movement stuff
 * ----------------------------------------------------------------------------------------------------*/

void camera_slow_out(VKCODE vk)
{
	VIEWCHECK(CAMERA_OUT_SLOW_PRESSED,vkOut_slow,view_out_slow_pressed);
}

void camera_slow_in(VKCODE vk)
{
	VIEWCHECK(CAMERA_IN_SLOW_PRESSED,vkIn_slow,view_in_slow_pressed);
}

void camera_out(VKCODE vk)
{
	VIEWCHECK(CAMERA_OUT_PRESSED,vkOut,view_out_pressed);
}

void camera_in(VKCODE vk)
{
	VIEWCHECK(CAMERA_IN_PRESSED,vkIn,view_in_pressed);
}

void camera_left(VKCODE vk)
{
	VIEWCHECK(CAMERA_LEFT_PRESSED,vkLeft,view_left_pressed);
}


void camera_right(VKCODE vk)
{
	VIEWCHECK(CAMERA_RIGHT_PRESSED,vkRight,view_right_pressed);
}

void cam_pitch_up(VKCODE vk)
{
	VIEWCHECK(CAMERA_UP_PRESSED,vkUp,view_up_pressed);
}

void cam_pitch_down(VKCODE vk)
{
	VIEWCHECK(CAMERA_DOWN_PRESSED,vkDown,view_down_pressed);
}

void cam_slew_north(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_NORTH_PRESSED,vkSlew_north,view_slew_north_pressed);
}

void cam_slew_north_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_NORTH_SLOW_PRESSED,vkSlew_north_slow,view_slew_north_slow_pressed);
}

void cam_slew_south(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_SOUTH_PRESSED,vkSlew_south,view_slew_south_pressed);
}

void cam_slew_south_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_SOUTH_SLOW_PRESSED,vkSlew_south_slow,view_slew_south_slow_pressed);
}

void cam_slew_east(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_EAST_PRESSED,vkSlew_east,view_slew_east_pressed);
}

void cam_slew_east_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_EAST_SLOW_PRESSED,vkSlew_east_slow,view_slew_east_slow_pressed);
}

void cam_slew_west(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_WEST_PRESSED,vkSlew_west,view_slew_west_pressed);
}

void cam_slew_west_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_WEST_SLOW_PRESSED,vkSlew_west_slow,view_slew_west_slow_pressed);
}

void cam_slew_up(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_UP_PRESSED,vkSlew_up,view_slew_up_pressed);
}

void cam_slew_up_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_UP_SLOW_PRESSED,vkSlew_up_slow,view_slew_up_slow_pressed);
}

void cam_slew_down(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_DOWN_PRESSED,vkSlew_down,view_slew_down_pressed);
}

void cam_slew_down_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLEW_DOWN_SLOW_PRESSED,vkSlew_down_slow,view_slew_down_slow_pressed);
}

void cam_slide_left(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLIDE_LEFT_PRESSED,vkSlide_left,view_slide_left_pressed);
}

void cam_slide_left_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLIDE_LEFT_SLOW_PRESSED,vkSlide_left_slow,view_slide_left_slow_pressed);
}

void cam_slide_right(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLIDE_RIGHT_PRESSED,vkSlide_right,view_slide_right_pressed);
}

void cam_slide_right_slow(VKCODE vk)
{
	VIEWCHECK(CAMERA_SLIDE_RIGHT_SLOW_PRESSED,vkSlide_right_slow,view_slide_right_slow_pressed);
}

void vrtl_glance_forward(VKCODE vk)
{
	vkGlanceFrontACM = vk;
}

/* ------------------------------------------2/3/98 4:41PM---------------------------------------------
 * now view modes             1
 * ----------------------------------------------------------------------------------------------------*/

BOOL CheckForEjected()
{
	if (PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
	{
		if (pPlayerChute->Type != pGuyOnGroundType)
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_FRONT | COCKPIT_NOART;
			LoadCockpit(FRONT_NO_COCKPIT);
			CockpitSeat = NO_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( FALSE );

			AssignCameraSubject((void *)pPlayerChute,CO_WEAPON);
		}
		else
		{
			AssignCameraSubject((void *)pPlayerChute,CO_WEAPON);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}

		return TRUE;
	}
	else
		return FALSE;

}

void cam_front_view(VKCODE vk)
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
			LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( TRUE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}

void cam_front_down(VKCODE vk)//front2
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT| COCKPIT_45DOWN;//(COCKPIT_FRONT | COCKPIT_45DOWN);
			LoadCockpit( VIRTUAL_MODEL);//FRONT_DOWN_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( TRUE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}


void cam_back_front(VKCODE vk)
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT| COCKPIT_45DOWN;//(COCKPIT_FRONT | COCKPIT_BACK_SEAT);
			LoadCockpit( BACK_FORWARD_COCKPIT );
			CockpitSeat = BACK_SEAT;
			SetupView( Camera1.SubType );//BACK_FORWARD_COCKPIT);
			OurShowCursor( TRUE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}
}


void cam_frontopen_view(VKCODE vk)
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_FRONT | COCKPIT_NOART;
			LoadCockpit(FRONT_NO_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}


void cam_left_view(VKCODE vk)
{
	int Choice;

	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			if (CockpitSeat == NO_SEAT)
				CockpitSeat = FRONT_SEAT;
			Choice = (CockpitSeat == BACK_SEAT) ? BACK_LEFT_COCKPIT : FRONT_LEFT_COCKPIT;
			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_LEFT;
			LoadCockpit(Choice);
			SetupView( Camera1.SubType );
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}


void cam_right_view(VKCODE vk)
{
	int Choice;

	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			if (CockpitSeat == NO_SEAT)
				CockpitSeat = FRONT_SEAT;
			Choice = (CockpitSeat == BACK_SEAT) ? BACK_RIGHT_COCKPIT : FRONT_RIGHT_COCKPIT;
			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_RIGHT;
			LoadCockpit(Choice);
			SetupView( Camera1.SubType  );
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}


void cam_back_left_view(VKCODE vk)
{
	int Choice;

	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Choice = (CockpitSeat == BACK_SEAT) ? BACK_BACK_LEFT_COCKPIT : FRONT_BACK_LEFT_COCKPIT;
			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_BACK_LEFT;
			LoadCockpit(Choice);
			SetupView( Camera1.SubType );
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}


}


void cam_back_right_view(VKCODE vk)
{
	int Choice;

	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Choice = (CockpitSeat == BACK_SEAT) ? BACK_BACK_RIGHT_COCKPIT : FRONT_BACK_RIGHT_COCKPIT;
			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_BACK_RIGHT;
			LoadCockpit(Choice);
			SetupView( Camera1.SubType  );
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}


void cam_frontno_view(VKCODE vk)
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_FRONT  | COCKPIT_NOART;
			LoadCockpit(FRONT_NO_COCKPIT);
			CockpitSeat = FRONT_SEAT;//NO_SEAT;
			SetupView( Camera1.SubType  );
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}
}


void cam_frontup_view(VKCODE vk)
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_FRONT | COCKPIT_45UP;
			LoadCockpit(FRONT_NO_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );//FRONT_NO_COCKPIT);
			OurShowCursor( FALSE );
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}
}

void camera_mode_tactical(VKCODE vk)
{
	if ((Camera1.CameraMode != CAMERA_TACTICAL) && (Camera1.CameraMode != CAMERA_REVERSE_TACTICAL))
	{
		if (UFC.MasterMode == AA_MODE)
		{
			if ((PlayerPlane->AADesignate) && ((PlayerPlane->AADesignate->Status & PL_ACTIVE) && !(PlayerPlane->AADesignate->FlightStatus & PL_STATUS_CRASHED)))
			{
				setup_no_cockpit_art();

				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
				AssignCameraTarget((void *)PlayerPlane->AADesignate,CO_PLANE);

				if ((Camera1.CameraMode != CAMERA_CHASE) && (Camera1.CameraMode != CAMERA_FIXED_CHASE))
				{
					camera_setup_initial_relative_quats();
					camera_setup_initial_distance(0x100 FEET);
				}
				ChangeViewModeOrSubject(CAMERA_TACTICAL);
			}

		}
		else
			if (UFC.MasterMode == AG_MODE)
			{
				if ((PlayerPlane->AGDesignate.X != -1.0) && (PlayerPlane->AGDesignate.Y != -1.0) && (PlayerPlane->AGDesignate.Z != -1.0))
				{
					setup_no_cockpit_art();

					AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

					AssignCameraTarget((void *)&PlayerPlane->AGDesignate,CO_POINT);

					if ((Camera1.CameraMode != CAMERA_CHASE) && (Camera1.CameraMode != CAMERA_FIXED_CHASE))
					{
						camera_setup_initial_relative_quats();
						camera_setup_initial_distance(0x100 FEET);
					}
					ChangeViewModeOrSubject(CAMERA_TACTICAL);
				}

			}

	}
	else
		ChangeViewModeOrSubject(CAMERA_TACTICAL);
}

void camera_mode_reverse(VKCODE vk)
{
	if ((Camera1.CameraMode != CAMERA_TACTICAL) || (Camera1.AttachedObject != (int *)PlayerPlane))
		camera_mode_tactical(vk);

	if ((Camera1.CameraMode == CAMERA_TACTICAL) && (Camera1.AttachedObject == (int *)PlayerPlane))
		ChangeViewModeOrSubject(CAMERA_REVERSE_TACTICAL);
}

void camera_mode_enemy_tactical(VKCODE vk)
{
	PlaneParams *plane;
	PlaneParams *closest_plane;
	PlaneParams *our_plane;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;

	closest_plane = NULL;
	our_plane = NULL;

	if ((Camera1.CameraMode == CAMERA_REVERSE_TACTICAL) && (Camera1.AttachedObject != (int *)PlayerPlane))
	{
		ChangeViewModeOrSubject(CAMERA_TACTICAL);
		return;
	}

	if ((Camera1.CameraMode == CAMERA_TACTICAL) && (Camera1.AttachedObject != (int *)PlayerPlane))
	{
		our_plane = (PlaneParams *)Camera1.AttachedObject;
		our_dist = PlayerPlane->WorldPosition / our_plane->WorldPosition;
	}

	plane = &Planes[0];

	while(plane <= LastPlane)
	{
		if ((plane != PlayerPlane) && (plane != our_plane) && (plane->Status & PL_ACTIVE) && (!(plane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide != plane->AI.iSide) && (plane->AADesignate == PlayerPlane))
		{
			dist = plane->WorldPosition / PlayerPlane->WorldPosition;
			if ((dist > our_dist) && (dist <= CurrentMaxDistance) && ((!closest_plane) || (dist < closest_dist)))
			{
				closest_plane = plane;
				closest_dist = dist;
			}
		}

		plane++;
	}

	if (closest_plane)
	{
		setup_no_cockpit_art();

		AssignCameraSubject((void *)closest_plane,CO_PLANE);
		AssignCameraTarget((void *)PlayerPlane,CO_PLANE);

		if ((Camera1.CameraMode != CAMERA_CHASE) && (Camera1.CameraMode != CAMERA_FIXED_CHASE))
		{
			camera_setup_initial_relative_quats();
			camera_setup_initial_distance(0x100 FEET);
		}
		ChangeViewModeOrSubject(CAMERA_TACTICAL);
	}

}

void camera_mode_enemy_reverse(VKCODE vk)
{
	PlaneParams *plane;
	PlaneParams *closest_plane;
	PlaneParams *our_plane;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;

	closest_plane = NULL;
	our_plane = NULL;

	if ((Camera1.CameraMode == CAMERA_TACTICAL) && (Camera1.AttachedObject != (int *)PlayerPlane))
	{
		ChangeViewModeOrSubject(CAMERA_REVERSE_TACTICAL);
		return;
	}

	if ((Camera1.CameraMode == CAMERA_REVERSE_TACTICAL) && (Camera1.AttachedObject != (int *)PlayerPlane))
	{
		our_plane = (PlaneParams *)Camera1.AttachedObject;
		our_dist = PlayerPlane->WorldPosition / our_plane->WorldPosition;
	}

	plane = &Planes[0];

	while(plane <= LastPlane)
	{
		if ((plane != PlayerPlane) && (plane != our_plane) && (plane->Status & PL_ACTIVE) && (!(plane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide != plane->AI.iSide) && (plane->AADesignate == PlayerPlane))
		{
			dist = plane->WorldPosition / PlayerPlane->WorldPosition;
			if ((dist > our_dist) && (dist <= CurrentMaxDistance) && ((!closest_plane) || (dist < closest_dist)))
			{
				closest_plane = plane;
				closest_dist = dist;
			}
		}

		plane++;
	}

	if (closest_plane)
	{
		setup_no_cockpit_art();

		AssignCameraSubject((void *)closest_plane,CO_PLANE);
		AssignCameraTarget((void *)PlayerPlane,CO_PLANE);

		if ((Camera1.CameraMode != CAMERA_CHASE) && (Camera1.CameraMode != CAMERA_FIXED_CHASE))
		{
			camera_setup_initial_relative_quats();
			camera_setup_initial_distance(0x100 FEET);
		}
		ChangeViewModeOrSubject(CAMERA_REVERSE_TACTICAL);
	}
}



void camera_mode_chase(VKCODE vk)
{
	if (Camera1.CameraMode != CAMERA_CHASE)
	{
		if (Camera1.AttachedObjectType != CO_WEAPON)
			AssignCameraTarget(NULL,CO_NADA);
		setup_no_cockpit_art();

		ChangeViewModeOrSubject(CAMERA_CHASE);
	}
}

void camera_mode_fixed_chase(VKCODE vk)
{
	if (Camera1.CameraMode != CAMERA_FIXED_CHASE)
	{
		if (Camera1.AttachedObjectType != CO_WEAPON)
			AssignCameraTarget(NULL,CO_NADA);
		setup_no_cockpit_art();

		ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
	}
}


void camera_mode_rc_view(VKCODE vk)
{
	if (Camera1.CameraMode != CAMERA_RC_VIEW)
	{
		if (Camera1.AttachedObjectType != CO_WEAPON)
		{
			AssignCameraTarget(NULL,CO_NADA);
		}
		else
		{
			WeaponParams *weap = (WeaponParams *)Camera1.AttachedObject;
			if(weap->Kind == EJECTION_SEAT)
			{
				AssignCameraTarget(NULL,CO_NADA);
			}
		}

		setup_no_cockpit_art();
		ChangeViewModeOrSubject(CAMERA_RC_VIEW);
	}
}

void camera_mode_free(VKCODE vk)
{
	if (Camera1.CameraMode != CAMERA_FREE)
	{
		if (Camera1.AttachedObjectType != CO_WEAPON)
		{
			AssignCameraTarget(NULL,CO_NADA);
		}
		else
		{
			WeaponParams *weap = (WeaponParams *)Camera1.AttachedObject;
			if(weap->Kind == EJECTION_SEAT)
			{
				AssignCameraTarget(NULL,CO_NADA);
			}
		}

		setup_no_cockpit_art();
		ChangeViewModeOrSubject(CAMERA_FREE);
	}
}

void camera_mode_flyby(VKCODE vk)
{
	if (Camera1.AttachedObjectType != CO_WEAPON)
		AssignCameraTarget(NULL,CO_NADA);
	setup_no_cockpit_art();
	ChangeViewModeOrSubject(CAMERA_FLYBY);
}

void virtual_free_look( VKCODE vk )
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			if ((Camera1.CameraMode != CAMERA_COCKPIT) || !(Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				Camera1.CurrentHeading = 0.0;
				Camera1.CurrentPitch = 0.0;
				Camera1.CurrentRoll = 0.0;
				Camera1.Heading = 0;
				Camera1.Pitch = 0;
				Camera1.Roll = 0;
			}

			Camera1.TargetHeading = 0.0;
			Camera1.TargetPitch = 0.0;
			Camera1.TargetRoll = 0.0;

			AssignCameraSubject(PlayerPlane,CO_PLANE);

			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
			LoadCockpit( VIRTUAL_MODEL );
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );

			//  I took this out for patch since we have an interactive cockpit
#if 0
			if (!DrawPopMpds)
				OurShowCursor( FALSE );
#endif
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}

void virtual_padlock_view( VKCODE vk )
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			if ((Camera1.AttachedObject == (int *)PlayerPlane) || (Camera1.AttachedObjectType == CO_NADA))
				virtual_free_look(vk);
			else
			{
				AssignCameraTarget(Camera1.TargetObject,Camera1.TargetObjectType);
				AssignCameraSubject(PlayerPlane,CO_PLANE);

				if ((Camera1.CameraMode != CAMERA_COCKPIT) || !(Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
				{
					Camera1.CurrentHeading = 0.0;
					Camera1.CurrentPitch = 0.0;
					Camera1.CurrentRoll = 0.0;
					Camera1.Heading = 0;
					Camera1.Pitch = 0;
					Camera1.Roll = 0;
				}

				Camera1.TargetHeading = 0.0;
				Camera1.TargetPitch = 0.0;
				Camera1.TargetRoll = 0.0;

				Camera1.CameraMode = CAMERA_COCKPIT;
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				LoadCockpit( VIRTUAL_MODEL );
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
			//  I took this out for patch since we have an interactive cockpit
#if 0
				if (!DrawPopMpds)
					OurShowCursor( FALSE );
#endif
			}
		}
		else
		{
			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}

}

DWORD ScanACMKeyViews(void)
{
	DWORD ACMViewRequested = 0;

	if (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
	{
		if (GetVkStatus(vkGlanceFrontACM))
			ACMViewRequested = COCKPIT_ACM_FRONT;
		else
		{
			if (GetVkStatus(vkUpACM))
			{
				ACMViewRequested &= ~COCKPIT_ACM_DOWN;
				ACMViewRequested |= COCKPIT_ACM_UP;
			}

			if (GetVkStatus(vkDownACM))
			{
				ACMViewRequested &= ~COCKPIT_ACM_UP;
				ACMViewRequested |= COCKPIT_ACM_DOWN;
			}

			if (GetVkStatus(vkLeftACM))
			{
				ACMViewRequested &= ~COCKPIT_ACM_RIGHT;
				ACMViewRequested |= COCKPIT_ACM_LEFT;
			}

			if (GetVkStatus(vkRightACM))
			{
				ACMViewRequested &= ~COCKPIT_ACM_LEFT;
				ACMViewRequested |= COCKPIT_ACM_RIGHT;
			}

			if (GetVkStatus(vkBackACM))
			{
				ACMViewRequested &= ~COCKPIT_ACM_FRONT;
				ACMViewRequested |= COCKPIT_ACM_BACK;
			}

			if (GetVkStatus(vkFrontACM))
			{
				ACMViewRequested &= ~COCKPIT_ACM_BACK;
				ACMViewRequested |= COCKPIT_ACM_FRONT;
			}

			if (GetVkStatus(vkFrontLeftACM))
			{
				ACMViewRequested &= ~(COCKPIT_ACM_BACK | COCKPIT_ACM_RIGHT);
				ACMViewRequested |= (COCKPIT_ACM_FRONT | COCKPIT_ACM_LEFT);
			}

			if (GetVkStatus(vkFrontRightACM))
			{
				ACMViewRequested &= ~(COCKPIT_ACM_BACK | COCKPIT_ACM_LEFT);
				ACMViewRequested |= (COCKPIT_ACM_FRONT | COCKPIT_ACM_RIGHT);
			}

			if (GetVkStatus(vkBackLeftACM))
			{
				ACMViewRequested &= ~(COCKPIT_ACM_FRONT | COCKPIT_ACM_RIGHT);
				ACMViewRequested |= (COCKPIT_ACM_BACK | COCKPIT_ACM_LEFT);
			}

			if (GetVkStatus(vkBackRightACM))
			{
				ACMViewRequested &= ~(COCKPIT_ACM_FRONT | COCKPIT_ACM_LEFT);
				ACMViewRequested |= (COCKPIT_ACM_BACK | COCKPIT_ACM_RIGHT);
			}

//			if (!ACMViewRequested)
//				ACMViewRequested = COCKPIT_ACM_FRONT;


	//		if( ACMViewRequested == COCKPIT_ACM_FRONT ||ACMViewRequested == (COCKPIT_ACM_DOWN ) )
	//		{
	//			VKCODE vk;
	//			vk.vkCode = 0;									// virtual key code (may be joy button)
	//			vk.wFlags = 0;									// vkCode Type
	//			vk.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
	//			virtual_free_look( vk);	//force ACM back to Virual Free look mode if not using a ACM VIEW
	//			return;
	//		}

			if (g_Settings.misc.dwMisc & GP_MISC_VIEW_PADLOCK_FOV_LIMITS)
			{
				if (ACMViewRequested == COCKPIT_ACM_BACK)
					ACMViewRequested = COCKPIT_ACM_BACK | COCKPIT_ACM_UP;
				else
					if ((ACMViewRequested != (COCKPIT_ACM_BACK | COCKPIT_ACM_UP)) && (ACMViewRequested & (COCKPIT_ACM_BACK | COCKPIT_ACM_LEFT | COCKPIT_ACM_RIGHT)) == COCKPIT_ACM_BACK)
						ACMViewRequested = 0;//COCKPIT_ACM_FRONT;
			}
	//		else
	//			if ((ACMViewRequested != (COCKPIT_ACM_BACK | COCKPIT_ACM_UP)) && ((ACMViewRequested & (COCKPIT_ACM_BACK | COCKPIT_ACM_LEFT | COCKPIT_ACM_RIGHT)) == COCKPIT_ACM_BACK))
	//				ACMViewRequested = COCKPIT_ACM_FRONT;
		}

		/* ------------------------------------------2/2/98 5:59PM---------------------------------------------
	 	* actually need to change view modes here
	 	* ----------------------------------------------------------------------------------------------------*/
	//	if (!(ACMViewRequested) || (((DWORD)Camera1.CameraMode == CAMERA_COCKPIT) && ((DWORD)Camera1.SubType == ((COCKPIT_ACM_MODE | COCKPIT_VIRTUAL_SEAT) | ACMViewRequested))))
	//		return;

	//	if (!CheckForEjected())
	//	{
	//		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
	//		{
	//			/*
	//			if ((ACMViewRequested & (COCKPIT_ACM_DOWN | COCKPIT_ACM_LEFT | COCKPIT_ACM_RIGHT)) == COCKPIT_ACM_DOWN)
	//			{
	//				if (((DWORD)Camera1.CameraMode == CAMERA_COCKPIT) && ((DWORD)Camera1.SubType == (COCKPIT_ACM_MODE | COCKPIT_ACM_FRONT | COCKPIT_ACM_DOWN)))
	//					return;
	//				Camera1.SubType = COCKPIT_ACM_MODE | COCKPIT_ACM_FRONT | COCKPIT_ACM_DOWN;
	//				LoadCockpit( FRONT_DOWN_COCKPIT );
	//				OurShowCursor( TRUE );
	//			}
	//			else
	//			*/
	//			{
	//				Camera1.SubType = (COCKPIT_ACM_MODE | COCKPIT_VIRTUAL_SEAT) | ACMViewRequested;
	//				LoadCockpit( VIRTUAL_MODEL );
	//				if (!DrawPopMpds)
	//					OurShowCursor( FALSE );
	//			}

	//			Camera1.CameraMode = CAMERA_COCKPIT;
	//			CockpitSeat = FRONT_SEAT;
	//			SetupView( Camera1.SubType );
	//		}
	//		else
	//		{
	//			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
	//			setup_no_cockpit_art();
	//			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
	//		}
	//	}
	}

	return ACMViewRequested;
}


void acm_front( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkFrontACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//
			SetupView( Camera1.SubType );
		}

}

void acm_back( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkBackACM = vk;
//			if (!(g_Settings.misc.dwMisc & GP_MISC_VIEW_PADLOCK_FOV_LIMITS))
			{
				if (Camera1.CameraMode != CAMERA_COCKPIT)
  					AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//				Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
				if(Camera1.SubType & COCKPIT_NOART)
				{
					iReturnACMView = 1;
					if(DrawPopMpds)
					{
						iReturnACMView |= 2;
					}

					Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
					LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
					CockpitSeat = FRONT_SEAT;
					SetupView( Camera1.SubType );
					OurShowCursor( TRUE );
				}
//

				SetupView( Camera1.SubType );
			}
		}
}

void acm_front_left( VKCODE vk)
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkFrontLeftACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

// Added for patch so we can ACM from FWRD no cockpit view
				if(Camera1.SubType & COCKPIT_NOART)
				{
					iReturnACMView = 1;
					if(DrawPopMpds)
					{
						iReturnACMView |= 2;
					}

					Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
					LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
					CockpitSeat = FRONT_SEAT;
					SetupView( Camera1.SubType );
					OurShowCursor( TRUE );
				}
//


//			Camera1.SubType |= COCKPIT_ACM_MODE;
			SetupView( Camera1.SubType );
		}
}

void acm_front_right( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkFrontRightACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//

			SetupView( Camera1.SubType );
		}
}

void acm_right( VKCODE vk )
{
	if (!CheckForEjected())
	{
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkRightACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//

			SetupView( Camera1.SubType );
		}
	}
}

void acm_left( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkLeftACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//


			SetupView( Camera1.SubType );
		}
}

void acm_back_right( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkBackRightACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//

			SetupView( Camera1.SubType );
		}
}

void acm_back_left( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkBackLeftACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//

			SetupView( Camera1.SubType );
		}
}

void acm_toggle_up( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkUpACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//

			SetupView( Camera1.SubType );
		}
}

void acm_toggle_down( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			vkDownACM = vk;
			if (Camera1.CameraMode != CAMERA_COCKPIT)
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			Camera1.SubType |= COCKPIT_ACM_MODE;

// Added for patch so we can ACM from FWRD no cockpit view
			if(Camera1.SubType & COCKPIT_NOART)
			{
				iReturnACMView = 1;
				if(DrawPopMpds)
				{
					iReturnACMView |= 2;
				}

				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
				LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
				CockpitSeat = FRONT_SEAT;
				SetupView( Camera1.SubType );
				OurShowCursor( TRUE );
			}
//

			SetupView( Camera1.SubType );
		}
}

/* -----------------7/22/99 4:15PM---------------------------------------------------------------------
/* zoom up on the mpds
/* ----------------------------------------------------------------------------------------------------*/

void zoom_mpd_left( VKCODE vk )
{

	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			if (Camera1.CameraMode != CAMERA_COCKPIT)
			{
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
				Camera1.CameraMode = CAMERA_COCKPIT;
			}

			if (Camera1.SubType & COCKPIT_ZOOM_LEFT_MPD)
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
			else
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_ZOOM_LEFT_MPD;//COCKPIT_FRONT;

			LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( TRUE );

		}
}

void zoom_mpd_right( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			if (Camera1.CameraMode != CAMERA_COCKPIT)
			{
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
				Camera1.CameraMode = CAMERA_COCKPIT;
			}

			if (Camera1.SubType & COCKPIT_ZOOM_CENTER_MPD)
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
			else
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_ZOOM_CENTER_MPD;//COCKPIT_FRONT;

			LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( TRUE );

		}
}

void zoom_mpd_center( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			if (Camera1.CameraMode != CAMERA_COCKPIT)
			{
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
				Camera1.CameraMode = CAMERA_COCKPIT;
			}

			if (Camera1.SubType & COCKPIT_ZOOM_RIGHT_MPD)
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
			else
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_ZOOM_RIGHT_MPD;//COCKPIT_FRONT;

			LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( TRUE );

		}
}

void zoom_mpd_down( VKCODE vk )
{
	if (!CheckForEjected())
		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		{
			if (Camera1.CameraMode != CAMERA_COCKPIT)
			{
  				AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
				Camera1.CameraMode = CAMERA_COCKPIT;
			}

			if (Camera1.SubType & COCKPIT_ZOOM_DOWN_MPD)
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT;//COCKPIT_FRONT;
			else
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_ZOOM_DOWN_MPD;//COCKPIT_FRONT;

			LoadCockpit( VIRTUAL_MODEL);//FRONT_FORWARD_COCKPIT);
			CockpitSeat = FRONT_SEAT;
			SetupView( Camera1.SubType );
			OurShowCursor( TRUE );

		}
}


/* ------------------------------------------2/3/98 4:41PM---------------------------------------------
 * and, finally, view subjects
 * ----------------------------------------------------------------------------------------------------*/

void watch_target(VKCODE vk)
{
	BOOL is_good = FALSE;
	if (UFC.MasterMode == AA_MODE)
	{
		if ((PlayerPlane->AADesignate) && (PlayerPlane->AADesignate->Status & PL_ACTIVE) && !(PlayerPlane->AADesignate->FlightStatus & PL_STATUS_CRASHED) && ((PlayerPlane->WorldPosition / PlayerPlane->AADesignate->WorldPosition) <= CurrentMaxDistance))
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget((void *)PlayerPlane->AADesignate,CO_PLANE);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject((void *)PlayerPlane->AADesignate,CO_PLANE);
				is_good = TRUE;
			}
		}
	}
	else
		if (UFC.MasterMode == AG_MODE)
		{
			if ((PlayerPlane->AGDesignate.X != -1.0) && (PlayerPlane->AGDesignate.Y != -1.0) && (PlayerPlane->AGDesignate.Z != -1.0) && ((PlayerPlane->WorldPosition / PlayerPlane->AGDesignate) <= CurrentMaxDistance))
			{
				if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
				{
					AssignCameraTarget(&PlayerPlane->AGDesignate,CO_POINT);
					Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
					SetupView( Camera1.SubType );
				}
				else
				{
					SwitchCameraSubjectToPoint(PlayerPlane->AGDesignate,TRUE);
					is_good = TRUE;
				}
			}

		}

	if (is_good)
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
		{
			setup_no_cockpit_art();
			Camera1.CameraMode = CAMERA_FIXED_CHASE;
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
		else
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
	}
}

void pdlk_target(VKCODE vk)
{
	virtual_free_look(vk);
	watch_target(vk);
}

#if 1		//**  There is now a phantom weapon inside WeaponPlanes so we don't need to do the plane check anymore
			//**  since the phantom weapon will be watched.
void watch_weapon(VKCODE vk)
{
	WeaponParams *weap = NULL;
	int pass = 2;

	while(pass--)
	{
		if((weap == NULL) && (Camera1.AttachedObjectType == CO_WEAPON))
		{
			weap = (WeaponParams *)Camera1.AttachedObject;
			weap++;
		}
		else
		{
			pass = 0;
			weap = &Weapons[0];
		}

		while(weap <= LastWeapon)
		{
			if ((weap != (WeaponParams *)Camera1.AttachedObject) && (weap->Flags & WEAPON_INUSE) && (weap->Kind != BULLET) && (weap->P == PlayerPlane) && (weap->Type != pFlareType) && (weap->Type != pChaffType) && (weap->Kind != EJECTION_SEAT))
			{
				if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
				{
					AssignCameraTarget((void *)weap,CO_WEAPON);
					Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
					SetupView( Camera1.SubType );
				}
				else
				{
					setup_no_cockpit_art();
					AssignCameraSubject((void *)weap,CO_WEAPON);
					if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
					{
						setup_no_cockpit_art();
						Camera1.CameraMode = CAMERA_FIXED_CHASE;
						ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
					}
					else
						if (Camera1.CameraMode == CAMERA_FREE)
	 						ChangeViewModeOrSubject(CAMERA_RC_VIEW);
						else
	 						ChangeViewModeOrSubject(Camera1.CameraMode);
				}
				return;
			}
			else
				weap++;
		}
	}
}
#else
void watch_weapon(VKCODE vk)
{
	WeaponParams *weap = NULL;
	PlaneParams *plane = NULL;
	int pass = 2;

	while(pass--)
	{
		if ((weap == NULL) && (plane == NULL))
		{
			if (Camera1.AttachedObjectType == CO_WEAPON)
			{
				weap = (WeaponParams *)Camera1.AttachedObject;
				weap++;
			}
			else
				if ((Camera1.AttachedObjectType == CO_PLANE) && WIsWeaponPlane((PlaneParams *)Camera1.AttachedObject))
				{
					plane = (PlaneParams *)Camera1.AttachedObject;
					plane++;
				}
				else
				{
					pass = 0;
				 	weap = &Weapons[0];
				}

		}
		else
		{
			pass = 0;
			weap = &Weapons[0];
		}

		if (weap)
		{
			while(weap <= LastWeapon)
			{
				if ((weap != (WeaponParams *)Camera1.AttachedObject) && (weap->Flags & WEAPON_INUSE) && (weap->Kind != BULLET) && (weap->P == PlayerPlane) && (weap->Type != pFlareType) && (weap->Type != pChaffType) && (weap->Kind != EJECTION_SEAT))
				{
					if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
					{
						AssignCameraTarget((void *)weap,CO_WEAPON);
						Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
						SetupView( Camera1.SubType );
					}
					else
					{
						setup_no_cockpit_art();
						AssignCameraSubject((void *)weap,CO_WEAPON);
						if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
						{
							setup_no_cockpit_art();
							Camera1.CameraMode = CAMERA_FIXED_CHASE;
							ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
						}
						else
							if (Camera1.CameraMode == CAMERA_FREE)
	 							ChangeViewModeOrSubject(CAMERA_RC_VIEW);
							else
	 							ChangeViewModeOrSubject(Camera1.CameraMode);
					}
					return;
				}
				else
					weap++;
			}

			weap = NULL;
			plane = Planes;
		}

		if (plane)
		{
			while(plane <= LastPlane)
			{
				if ((plane != PlayerPlane) && (plane->Status & PL_ACTIVE) && WIsWeaponPlane(plane) && (plane != (PlaneParams *)Camera1.AttachedObject) && (plane->AI.LinkedPlane == PlayerPlane))
				{
					if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
					{
						AssignCameraTarget((void *)plane,CO_PLANE);
						Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
						SetupView( Camera1.SubType );
					}
					else
					{
						setup_no_cockpit_art();
						AssignCameraSubject((void *)plane,CO_PLANE);
						if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
						{
							setup_no_cockpit_art();
							Camera1.CameraMode = CAMERA_FIXED_CHASE;
							ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
						}
						else
							if (Camera1.CameraMode == CAMERA_FREE)
	 							ChangeViewModeOrSubject(CAMERA_RC_VIEW);
							else
	 							ChangeViewModeOrSubject(Camera1.CameraMode);
					}
					return;
				}
				else
					plane++;
			}
		}
	}
}
#endif

void pdlk_weapon(VKCODE vk)
{
	virtual_free_look(vk);
	watch_weapon(vk);
}


void watch_me(VKCODE vk)
{
	AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

	if (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL)
	{
		Camera1.CameraMode = CAMERA_FIXED_CHASE;
		ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
	}
	else
		if (Camera1.CameraMode == CAMERA_FREE)
	 		ChangeViewModeOrSubject(CAMERA_RC_VIEW);
		else
	 		ChangeViewModeOrSubject(Camera1.CameraMode);
}

void watch_threat_weapon(VKCODE vk)
{
	WeaponParams *weap = NULL;
	int pass = 2;

	while(pass--)
	{
		if ((weap == NULL) && (Camera1.AttachedObjectType == CO_WEAPON))
		{
			weap = (WeaponParams *)Camera1.AttachedObject;
			weap++;
		}
		else
		{
			pass = 0;
			weap = &Weapons[0];
		}

		while(weap <= LastWeapon)
		{
			if ((weap != (WeaponParams *)Camera1.AttachedObject) && (weap->Flags & WEAPON_INUSE) && (weap->Kind != BULLET) && (weap->pTarget == PlayerPlane) && (weap->Type != pFlareType) && (weap->Type != pChaffType) && (weap->Kind != EJECTION_SEAT))
			{
				if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
				{
					AssignCameraTarget((void *)weap,CO_WEAPON);
					Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
					SetupView( Camera1.SubType );
				}
				else
				{
					setup_no_cockpit_art();
					AssignCameraSubject((void *)weap,CO_WEAPON);
					if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
					{
						setup_no_cockpit_art();
						Camera1.CameraMode = CAMERA_FIXED_CHASE;
						ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
					}
					else
						if (Camera1.CameraMode == CAMERA_FREE)
	 						ChangeViewModeOrSubject(CAMERA_RC_VIEW);
						else
	 						ChangeViewModeOrSubject(Camera1.CameraMode);
				}
				return;
			}
			else
				weap++;
		}
	}
}

void pdlk_threat_weapon(VKCODE vk)
{
	virtual_free_look(vk);
	watch_threat_weapon(vk);
}


void watch_next_friendly(VKCODE vk)
{
	PlaneParams *plane;
	PlaneParams *closest_plane;
	PlaneParams *our_plane;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;

	if ((Camera1.CameraMode != CAMERA_TACTICAL) && (Camera1.CameraMode != CAMERA_REVERSE_TACTICAL))
	{
		closest_plane = NULL;
		our_plane = NULL;

		if (Camera1.CameraMode == CAMERA_RC_VIEW)
		{
			CheckLocation = Camera1.CameraLocation;
			if ((Camera1.AttachedObjectType == CO_PLANE) && (PlayerPlane->AI.iSide == ((PlaneParams *)Camera1.AttachedObject)->AI.iSide) && !WIsWeaponPlane((PlaneParams *)Camera1.AttachedObject))
			{
			 	our_plane = (PlaneParams *)Camera1.AttachedObject;
				our_dist = Camera1.SubjectLocation / CheckLocation;
			}
			else
				our_dist = 0.0;
		}
		else
		{
			CheckLocation = PlayerPlane->WorldPosition;
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				if ((Camera1.TargetObjectType == CO_PLANE) && (PlayerPlane->AI.iSide == ((PlaneParams *)Camera1.TargetObject)->AI.iSide) && !WIsWeaponPlane((PlaneParams *)Camera1.TargetObject))
				{
					our_plane = (PlaneParams *)Camera1.TargetObject;
					our_dist = Camera1.TargetLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
			else
			{
				if ((Camera1.AttachedObjectType == CO_PLANE) && (PlayerPlane->AI.iSide == ((PlaneParams *)Camera1.AttachedObject)->AI.iSide) && !WIsWeaponPlane((PlaneParams *)Camera1.AttachedObject))
				{
					our_plane = (PlaneParams *)Camera1.AttachedObject;
					our_dist = Camera1.SubjectLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
		}

		plane = &Planes[0];

		while(plane <= LastPlane)
		{
			if ((plane != PlayerPlane) && (plane != our_plane) && (plane->Status & PL_ACTIVE) && !WIsWeaponPlane(plane) && (!(plane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide == plane->AI.iSide))
			{
				dist = plane->WorldPosition / CheckLocation;
				if ((dist > our_dist) && (dist <= CurrentMaxDistance) && ((!closest_plane) || (dist < closest_dist)))
				{
					closest_plane = plane;
					closest_dist = dist;
				}
			}

			plane++;
		}

		if (closest_plane)
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(closest_plane,CO_PLANE);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(closest_plane,CO_PLANE);
				if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
				{
					setup_no_cockpit_art();
					Camera1.CameraMode = CAMERA_FIXED_CHASE;
					ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
				}
				else
					if (Camera1.CameraMode == CAMERA_FREE)
	 					ChangeViewModeOrSubject(CAMERA_RC_VIEW);
					else
	 					ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
		else
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(NULL,CO_NADA);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(PlayerPlane,CO_PLANE);
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
			}

		}
	}
}

void pdlk_next_friendly(VKCODE vk)
{
	virtual_free_look(vk);
	watch_next_friendly(vk);
}

void watch_next_enemy(VKCODE vk)
{
	PlaneParams *plane;
	PlaneParams *closest_plane;
	PlaneParams *our_plane;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;
	int pass_count;

	if ((Camera1.CameraMode != CAMERA_TACTICAL) && (Camera1.CameraMode != CAMERA_REVERSE_TACTICAL))
	{
		closest_plane = NULL;
		our_plane = NULL;

		if (Camera1.CameraMode == CAMERA_RC_VIEW)
		{
			CheckLocation = Camera1.CameraLocation;
			if ((Camera1.AttachedObjectType == CO_PLANE) && (PlayerPlane->AI.iSide != ((PlaneParams *)Camera1.AttachedObject)->AI.iSide) && !WIsWeaponPlane((PlaneParams *)Camera1.AttachedObject))
			{
				our_plane = (PlaneParams *)Camera1.AttachedObject;
				our_dist = Camera1.SubjectLocation / CheckLocation;
			}
			else
				our_dist = 0.0;
		}
		else
		{
			CheckLocation = PlayerPlane->WorldPosition;
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				if ((Camera1.TargetObjectType == CO_PLANE) && (PlayerPlane->AI.iSide != ((PlaneParams *)Camera1.TargetObject)->AI.iSide) && !WIsWeaponPlane((PlaneParams *)Camera1.TargetObject))
				{
					our_plane = (PlaneParams *)Camera1.TargetObject;
					our_dist = Camera1.TargetLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
			else
			{
				if ((Camera1.AttachedObjectType == CO_PLANE) && (PlayerPlane->AI.iSide != ((PlaneParams *)Camera1.AttachedObject)->AI.iSide) && !WIsWeaponPlane((PlaneParams *)Camera1.AttachedObject))
				{
					our_plane = (PlaneParams *)Camera1.AttachedObject;
					our_dist = Camera1.SubjectLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
		}

		if ((Camera1.SubType & COCKPIT_PADLOCK) && (Camera1.TargetObjectType == CO_PLANE))
		{
			plane = (PlaneParams *)Camera1.TargetObject;
			if (PlayerPlane->AI.iSide == plane->AI.iSide)
				plane = Planes;
		}
		else
			plane = Planes;

		if (plane == Planes)
			pass_count = 1;
		else
			pass_count = 2;

		closest_plane = NULL;

		while(pass_count--)
		{
			while(plane <= LastPlane)
			{
				if ((plane != PlayerPlane) && (plane != our_plane) && (plane->Status & PL_ACTIVE) && !WIsWeaponPlane(plane) && (!(plane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide != plane->AI.iSide))
				{
					dist = plane->WorldPosition / CheckLocation;
					if ((dist > our_dist) && (dist <= CurrentMaxDistance) && (dist <= (6.0f * NMTOWU)) && ((!closest_plane) || (dist < closest_dist)))
					{
						if(AILOS(plane->WorldPosition, CheckLocation))
						{
							closest_plane = plane;
							closest_dist = dist;
						}
					}
				}

				plane++;
			}

			if (closest_plane)
				pass_count = 0;
			else
				plane = Planes;
		}

		if (closest_plane)
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(closest_plane,CO_PLANE);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(closest_plane,CO_PLANE);
				if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
				{
					setup_no_cockpit_art();
					Camera1.CameraMode = CAMERA_FIXED_CHASE;
					ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
				}
				else
					if (Camera1.CameraMode == CAMERA_FREE)
	 					ChangeViewModeOrSubject(CAMERA_RC_VIEW);
					else
	 					ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
		else
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(NULL,CO_NADA);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(PlayerPlane,CO_PLANE);
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
			}

		}
	}
}

void pdlk_next_enemy(VKCODE vk)
{
	virtual_free_look(vk);
	watch_next_enemy(vk);
}

void watch_next_ground(VKCODE vk)
{
	RegisteredObject *rwalker;
	BasicInstance *binst;
	BasicInstance *our_object;
	BasicInstance *closest_binst;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;

	if ((Camera1.CameraMode != CAMERA_TACTICAL) && (Camera1.CameraMode != CAMERA_REVERSE_TACTICAL))
	{
		closest_binst = NULL;
		our_object = NULL;

		if (Camera1.CameraMode == CAMERA_RC_VIEW)
		{
			CheckLocation = Camera1.CameraLocation;
			if (Camera1.TargetObjectType == CO_GROUND_OBJECT)
			{
				our_object = (BasicInstance *)Camera1.AttachedObject;
				our_dist = Camera1.SubjectLocation / CheckLocation;
			}
			else
				our_dist = 0.0;
		}
		else
		{
			CheckLocation = PlayerPlane->WorldPosition;
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				if (Camera1.TargetObjectType == CO_GROUND_OBJECT)
				{
					our_object = (BasicInstance *)Camera1.TargetObject;
					our_dist = Camera1.TargetLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
			else
			{
				if (Camera1.AttachedObjectType == CO_GROUND_OBJECT)
				{
					our_object = (BasicInstance *)Camera1.AttachedObject;
					our_dist = Camera1.SubjectLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
		}

		rwalker = FirstInRangeObject;

		while(rwalker)
		{
			if (rwalker->Flags & RO_OBJECT_FALLING)
				binst = ((FallingObject *)rwalker->Instance)->Instance;
			else
				binst = rwalker->Instance;

			if ((binst) && (binst != our_object) && (((StructureInstance *)binst)->Type) && !(((StructureInstance *)binst)->Type->Basics.Flags & (BF_ONLY_LIGHTS | BF_LAYS_FLAT | BF_WHOLE_TILE)))
			{
				dist = binst->Position / CheckLocation;
				if ((dist > our_dist) && (dist <= CurrentMaxDistance) && ((!closest_binst) || (dist < closest_dist)))
				{
					closest_binst = binst;
					closest_dist = dist;
				}
			}

			rwalker = rwalker->NextObject;
		}

		if (closest_binst)
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(closest_binst,CO_GROUND_OBJECT);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(closest_binst,CO_GROUND_OBJECT);
				if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
				{
					setup_no_cockpit_art();
					Camera1.CameraMode = CAMERA_FIXED_CHASE;
					ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
				}
				else
					if (Camera1.CameraMode == CAMERA_FREE)
	 					ChangeViewModeOrSubject(CAMERA_RC_VIEW);
					else
	 					ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
		else
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(NULL,CO_NADA);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(PlayerPlane,CO_PLANE);
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
			}

		}
	}
}

void pdlk_next_ground(VKCODE vk)
{
	virtual_free_look(vk);
	watch_next_ground(vk);
}

void watch_next_vehicle(VKCODE vk)
{
	MovingVehicleParams *vehicle;
	MovingVehicleParams *closest_vehicle;
	MovingVehicleParams *our_vehicle;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;

	if ((Camera1.CameraMode != CAMERA_TACTICAL) && (Camera1.CameraMode != CAMERA_REVERSE_TACTICAL))
	{
		closest_vehicle = NULL;
		our_vehicle = NULL;

		if (Camera1.CameraMode == CAMERA_RC_VIEW)
		{
			CheckLocation = Camera1.CameraLocation;
			if (Camera1.AttachedObjectType == CO_VEHICLE)
			{
				our_vehicle = (MovingVehicleParams *)Camera1.AttachedObject;
				our_dist = Camera1.SubjectLocation / CheckLocation;
			}
			else
				our_dist = 0.0;
		}
		else
		{
			CheckLocation = PlayerPlane->WorldPosition;
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				if (Camera1.AttachedObjectType == CO_VEHICLE)
				{
					our_vehicle = (MovingVehicleParams *)Camera1.TargetObject;
					our_dist = Camera1.TargetLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
			else
			{
				if (Camera1.AttachedObjectType == CO_VEHICLE)
				{
					our_vehicle = (MovingVehicleParams *)Camera1.AttachedObject;
					our_dist = Camera1.SubjectLocation / CheckLocation;
				}
				else
					our_dist = 0.0;
			}
		}

		vehicle = &MovingVehicles[0];

		while(vehicle <= LastMovingVehicle)
		{
			if (/*(vehicle->SortHeader) && */(vehicle != our_vehicle) && (vehicle->Status & VL_ACTIVE))
			{
				dist = vehicle->WorldPosition / CheckLocation;
				if ((dist > our_dist) && (dist <= CurrentMaxDistance) && ((!closest_vehicle) || (dist < closest_dist)))
				{
					closest_vehicle = vehicle;
					closest_dist = dist;
				}
			}

			vehicle++;
		}

		if (closest_vehicle)
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(closest_vehicle,CO_VEHICLE);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(closest_vehicle,CO_VEHICLE);
				if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
				{
					setup_no_cockpit_art();
					Camera1.CameraMode = CAMERA_FIXED_CHASE;
					ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
				}
				else
					if (Camera1.CameraMode == CAMERA_FREE)
	 					ChangeViewModeOrSubject(CAMERA_RC_VIEW);
					else
	 					ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
		else
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget(NULL,CO_NADA);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject(PlayerPlane,CO_PLANE);
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
			}

		}
	}
}

void pdlk_next_vehicle(VKCODE vk)
{
	virtual_free_look(vk);
	watch_next_vehicle(vk);
}


void watch_wso_threat(VKCODE vk)
{
	WeaponParams *weap;
	PlaneParams *plane;

	if ((Camera1.CameraMode != CAMERA_TACTICAL) && (Camera1.CameraMode != CAMERA_REVERSE_TACTICAL))
	{
		if(pLastWSOWarnObj)
		{
			if(iLastWSOWarnType == TARGET_WEAPON)
			{
				weap = (WeaponParams *)pLastWSOWarnObj;
				pLastWSOWarnObj = NULL;
				if (weap->Flags & WEAPON_INUSE)
				{
					if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
					{
						AssignCameraTarget(weap,CO_WEAPON);
						Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
						SetupView( Camera1.SubType );
					}
					else
					{
						AssignCameraSubject(weap,CO_WEAPON);
						if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
						{
							setup_no_cockpit_art();
							Camera1.CameraMode = CAMERA_FIXED_CHASE;
							ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
						}
						else
							if (Camera1.CameraMode == CAMERA_FREE)
	 							ChangeViewModeOrSubject(CAMERA_RC_VIEW);
							else
	 							ChangeViewModeOrSubject(Camera1.CameraMode);
					}
				}

			}
			else if(iLastWSOWarnType == TARGET_PLANE)
			{
				plane = (PlaneParams *)pLastWSOWarnObj;
				pLastWSOWarnObj = NULL;
				if ((plane != PlayerPlane) && (plane->Status & PL_ACTIVE) && (!(plane->FlightStatus & PL_STATUS_CRASHED)))
				{
					if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
					{
						AssignCameraTarget(plane,CO_PLANE);
						Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
						SetupView( Camera1.SubType );
					}
					else
					{
						AssignCameraSubject(plane,CO_PLANE);
						if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
						{
							setup_no_cockpit_art();
							Camera1.CameraMode = CAMERA_FIXED_CHASE;
							ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
						}
						else
							if (Camera1.CameraMode == CAMERA_FREE)
	 							ChangeViewModeOrSubject(CAMERA_RC_VIEW);
							else
	 							ChangeViewModeOrSubject(Camera1.CameraMode);
					}
				}
			}
		}
		else
			watch_next_enemy(vk);
	}
}

void pdlk_wso_threat(VKCODE vk)
{
	virtual_free_look(vk);
	watch_wso_threat(vk);
}

void watch_center_object(VKCODE vk)
{
	void *curr_target;
	void *curr_subject;

	void *closest_object;
	CameraObjectType closest_object_type;
	double closest_object_angle;

	PlaneParams *pwalk;
	RegisteredObject *rwalk;
	BasicInstance *binst;
	FPointDouble CheckLocation;
	double angle;
	double dist;

	if (Camera1.TargetObjectType != CO_NADA)
		curr_target = (void *)Camera1.TargetObject;
	else
		curr_target = NULL;

	curr_subject = (void *)Camera1.AttachedObject;

	closest_object = NULL;

	MovingVehicleParams *vwalk;
	WeaponParams *wwalk;

	/* ------------------------------------------2/5/98 3:47PM---------------------------------------------
	 * check the planes first
	 * ----------------------------------------------------------------------------------------------------*/

	pwalk = &Planes[0];
	while(pwalk <= LastPlane)
	{
		if (((void *)pwalk != curr_subject) && ((void *)pwalk != curr_target) && (pwalk->Status & PL_ACTIVE) && (!(pwalk->FlightStatus & PL_STATUS_CRASHED)) && (!(pwalk->AI.iAIFlags2 & AI_PADLOCKED)))
		{
			CheckLocation.MakeVectorToFrom(pwalk->WorldPosition,Camera1.CameraLocation);
			dist = CheckLocation.Normalize();
			angle = CheckLocation * ViewVector;
			if ((angle > 0.0f) && ((!closest_object) || (angle > closest_object_angle)) && (dist <= CurrentMaxDistance))
			{
				closest_object = (void *)pwalk;
				closest_object_type = CO_PLANE;
				closest_object_angle= angle;
			}
		}

		pwalk++;
	}

	/* ------------------------------------------2/5/98 3:47PM---------------------------------------------
	 * now the vehicles
	 * ----------------------------------------------------------------------------------------------------*/

	vwalk = &MovingVehicles[0];
	while(vwalk <= LastMovingVehicle)
	{
		if (/*(vwalk->SortHeader) &&*/ CloseEnoughToDraw(vwalk->WorldPosition) && ((void *)vwalk != curr_subject) && ((void *)vwalk != curr_target) && (vwalk->Status & VL_ACTIVE))
		{
			CheckLocation.MakeVectorToFrom(vwalk->WorldPosition,Camera1.CameraLocation);
			dist = CheckLocation.Normalize();
			angle = CheckLocation * ViewVector;
			if ((angle > 0.0f) && ((!closest_object) || (angle > closest_object_angle)) && (dist <= CurrentMaxDistance))
			{
				closest_object = (void *)vwalk;
				closest_object_type = CO_VEHICLE;
				closest_object_angle= angle;
			}
		}

		vwalk++;
	}

	/* ------------------------------------------2/5/98 3:47PM---------------------------------------------
	 * now the weapons
	 * ----------------------------------------------------------------------------------------------------*/

	wwalk = &Weapons[0];
	while(wwalk <= LastWeapon)
	{
		if (/*(wwalk->SortHeader) &&*/ CloseEnoughToDraw(wwalk->Pos) && ((void *)wwalk != curr_subject) && ((void *)wwalk != curr_target) && (wwalk->Flags & WEAPON_INUSE) && (wwalk->Kind != BULLET) && (wwalk->Type != pFlareType) && (wwalk->Type != pChaffType))
		{
			CheckLocation.MakeVectorToFrom(wwalk->Pos,Camera1.CameraLocation);
			dist = CheckLocation.Normalize();
			angle = CheckLocation * ViewVector;
			if ((angle > 0.0f) && ((!closest_object) || (angle > closest_object_angle)) && (dist <= CurrentMaxDistance))
			{
				closest_object = (void *)wwalk;
				closest_object_type = CO_WEAPON;
				closest_object_angle= angle;
			}
		}

		wwalk++;
	}

	/* ------------------------------------------2/5/98 3:47PM---------------------------------------------
	 * now the ground objects
	 * ----------------------------------------------------------------------------------------------------*/

	rwalk = FirstInRangeObject;

	while(rwalk)
	{
//		if (rwalk->SortHeader)          /* all objects in this list are visible */
		{
			if (rwalk->Flags & RO_OBJECT_FALLING)
				binst = ((FallingObject *)rwalk->Instance)->Instance;
			else
				binst = rwalk->Instance;

			if ((binst) && (binst != curr_subject) && (binst != curr_subject) && (((StructureInstance *)binst)->Type) && !(((StructureInstance *)binst)->Type->Basics.Flags & (BF_ONLY_LIGHTS | BF_LAYS_FLAT | BF_WHOLE_TILE)))
			{
				CheckLocation.MakeVectorToFrom(binst->Position,Camera1.CameraLocation);
				CheckLocation.Y += rwalk->ExtentHeight*0.5f;
				dist = CheckLocation.Normalize();
				angle = CheckLocation * ViewVector;
				if ((angle > 0.0f) && ((!closest_object) || (angle > closest_object_angle)) && (dist <= CurrentMaxDistance))
				{
					closest_object = (void *)binst;
					closest_object_type = CO_GROUND_OBJECT;
					closest_object_angle= angle;
				}
			}
		}

		rwalk = rwalk->NextObject;
	}

	/* ------------------------------------------2/5/98 5:21PM---------------------------------------------
	 * now set the view
	 * ----------------------------------------------------------------------------------------------------*/

	if (closest_object)
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			if(closest_object_type == CO_PLANE)
			{
				pwalk = (PlaneParams *)closest_object;
				pwalk->AI.iAIFlags2 |= AI_PADLOCKED;
			}
			else
			{
				pwalk = &Planes[0];
				while(pwalk <= LastPlane)
				{
					pwalk->AI.iAIFlags2 &= ~AI_PADLOCKED;
					pwalk++;
				}
			}

			AssignCameraTarget(closest_object,closest_object_type);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(closest_object,closest_object_type);
			if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
			{
				setup_no_cockpit_art();
				Camera1.CameraMode = CAMERA_FIXED_CHASE;
				ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
			}
			else
			{
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
	}
	else
	{
		pwalk = &Planes[0];
		while(pwalk <= LastPlane)
		{
			pwalk->AI.iAIFlags2 &= ~AI_PADLOCKED;
			pwalk++;
		}
		AssignCameraTarget(NULL,CO_NADA);
	}
}

void pdlk_center_object(VKCODE vk)
{
	if(!(Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
	{
		AssignCameraTarget(NULL,CO_NADA);
	}
	virtual_free_look(vk);
	watch_center_object(vk);
}


void watch_wingman(VKCODE vk)
{
	PlaneParams *closest_plane = NULL;

	if (PlayerPlane->AI.wingman != -1)
		closest_plane = &Planes[PlayerPlane->AI.wingman];
	else if(PlayerPlane->AI.winglead != -1)
		closest_plane = &Planes[PlayerPlane->AI.winglead];

	if (closest_plane && (closest_plane != PlayerPlane) && (closest_plane->Status & PL_ACTIVE) && (!(closest_plane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide == closest_plane->AI.iSide))
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			AssignCameraTarget(closest_plane,CO_PLANE);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(closest_plane,CO_PLANE);
			if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
			{
				setup_no_cockpit_art();
				Camera1.CameraMode = CAMERA_FIXED_CHASE;
				ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
			}
			else
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
		}
	}
	else
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			AssignCameraTarget(NULL,CO_NADA);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(PlayerPlane,CO_PLANE);
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
		}

	}

}

void pdlk_wingman(VKCODE vk)
{
	virtual_free_look(vk);
	watch_wingman(vk);
}

void watch_tanker(VKCODE vk)
{
	PlaneParams *plane;
	PlaneParams *closest_plane;
	PlaneParams *our_plane;
	FPointDouble CheckLocation;
	double dist;
	double our_dist;
	double closest_dist;

	closest_plane = NULL;
	our_plane = NULL;

	if (Camera1.CameraMode == CAMERA_RC_VIEW)
	{
		CheckLocation = Camera1.CameraLocation;
		if ((Camera1.AttachedObjectType == CO_PLANE) && (PlayerPlane->AI.iSide == ((PlaneParams *)Camera1.AttachedObject)->AI.iSide))
		{
			our_plane = (PlaneParams *)Camera1.AttachedObject;
			our_dist = Camera1.SubjectLocation / CheckLocation;
		}
		else
			our_dist = 0.0;
	}
	else
	{
		CheckLocation = PlayerPlane->WorldPosition;
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			if ((Camera1.TargetObjectType == CO_PLANE) && (PlayerPlane->AI.iSide == ((PlaneParams *)Camera1.TargetObject)->AI.iSide))
			{
				our_plane = (PlaneParams *)Camera1.TargetObject;
				our_dist = Camera1.TargetLocation / CheckLocation;
			}
			else
				our_dist = 0.0;
		}
		else
		{
			if ((Camera1.AttachedObjectType == CO_PLANE) && (PlayerPlane->AI.iSide == ((PlaneParams *)Camera1.AttachedObject)->AI.iSide))
			{
				our_plane = (PlaneParams *)Camera1.AttachedObject;
				our_dist = Camera1.SubjectLocation / CheckLocation;
			}
			else
				our_dist = 0.0;
		}
	}

	plane = &Planes[0];

	while(plane <= LastPlane)
	{
		if ((plane != PlayerPlane) && (plane != our_plane) && (plane->Status & PL_ACTIVE) && (!(plane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide == plane->AI.iSide))
		{
			if(pDBAircraftList[plane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
			{
				dist = plane->WorldPosition / CheckLocation;
				if ((dist > our_dist) && (dist <= CurrentMaxDistance) && ((!closest_plane) || (dist < closest_dist)))
				{
					closest_plane = plane;
					closest_dist = dist;
				}
			}
		}

		plane++;
	}

	if (closest_plane)
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			AssignCameraTarget(closest_plane,CO_PLANE);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(closest_plane,CO_PLANE);
			if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
			{
				setup_no_cockpit_art();
				Camera1.CameraMode = CAMERA_FIXED_CHASE;
				ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
			}
			else
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
		}
	}
	else
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			AssignCameraTarget(NULL,CO_NADA);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(PlayerPlane,CO_PLANE);
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
		}

	}
}

void pdlk_tanker(VKCODE vk)
{
	virtual_free_look(vk);
	watch_tanker(vk);
}

void watch_package(VKCODE vk)
{
	FPoint	position;
	int waycnt;
	int numwpts;
	PlaneParams *package = NULL;
	Escort	*pEscortAction;
	MBWayPoints *lookway;
	int startact, endact, actcnt;
	PlaneParams *planepnt = PlayerPlane;
	PlaneParams *playerlead = AIGetLeader(PlayerPlane);
	int planenum = PlayerPlane - Planes;
	PlaneParams *checkplane;
	int pass;

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

	if(!package)
	{
		for(pass = 0; pass < 2; pass ++)
		{
			if(pass == 0)
			{
				if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
				{
					if((Camera1.TargetObjectType == CO_PLANE) && (Camera1.TargetObject))
					{
						checkplane = (PlaneParams *)Camera1.TargetObject;
						checkplane ++;
					}
					else
					{
						pass ++;
						checkplane = &Planes[0];
					}
				}
				else
				{
					if((Camera1.AttachedObjectType == CO_PLANE) && (Camera1.AttachedObject))
					{
						checkplane = (PlaneParams *)Camera1.AttachedObject;
						checkplane ++;
					}
					else
					{
						pass ++;
						checkplane = &Planes[0];
					}
				}
			}
			else
			{
				checkplane = &Planes[0];
			}

			while (checkplane <= LastPlane)
			{
				if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
				{
#if 0
					if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == playerlead))
					{
						if((checkplane->AI.iAIFlags2 & AI_SEAD_COVER))
						{
							package = checkplane;
							pass = 3;
							break;
						}
						else if((checkplane->AI.iAICombatFlags2 & AI_ESCORT_CAS))
						{
							package = checkplane;
							pass = 3;
							break;
						}
						else
						{
							package = checkplane;
							pass = 3;
							break;
						}
					}
#else
					if(checkplane->AI.LinkedPlane == playerlead)
					{
						package = checkplane;
						pass = 3;
						break;
					}
#endif
				}
				checkplane ++;
			}
		}
	}

	if (package && (package != PlayerPlane) && (package->Status & PL_ACTIVE) && (!(package->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide == package->AI.iSide))
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			AssignCameraTarget(package,CO_PLANE);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(package,CO_PLANE);
			if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
			{
				setup_no_cockpit_art();
				Camera1.CameraMode = CAMERA_FIXED_CHASE;
				ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
			}
			else
				if (Camera1.CameraMode == CAMERA_FREE)
	 				ChangeViewModeOrSubject(CAMERA_RC_VIEW);
				else
	 				ChangeViewModeOrSubject(Camera1.CameraMode);
		}
	}
	else
	{
		if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
		{
			AssignCameraTarget(NULL,CO_NADA);
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
			SetupView( Camera1.SubType );
		}
		else
		{
			AssignCameraSubject(PlayerPlane,CO_PLANE);
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
		}

	}

}

void pdlk_package(VKCODE vk)
{
	virtual_free_look(vk);
	watch_package(vk);
}

extern int iWatchPlane;
/* ------------------------------------------2/3/98 5:58PM---------------------------------------------
 * these are debug view modes
 * ----------------------------------------------------------------------------------------------------*/
void do_next_plane(VKCODE vk)
{
	PlaneParams *P;
	PlaneParams *orgP;

//	if (MultiPlayer)
//		return;

	orgP = P = (PlaneParams *)Camera1.AttachedObject;
	P++;
	if (P > LastPlane)
		P = &Planes[0];
	while((P != orgP) && (!(P->Status & PL_ACTIVE)))
	{
		P++;
		if (P > LastPlane)
			P = &Planes[0];
	}

	iWatchPlane = P - Planes;

	if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
	{
		AssignCameraTarget((void *)P,CO_PLANE);
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		SetupView( Camera1.SubType );
	}
	else
	{
		AssignCameraSubject((void *)P,CO_PLANE);
		if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
		{
			setup_no_cockpit_art();
			Camera1.CameraMode = CAMERA_FIXED_CHASE;
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
		else
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
	}
}

extern int iWatchPlane;
/* ------------------------------------------2/10/98 5:30PM---------------------------------------------
 * these are debug view modes
 * ----------------------------------------------------------------------------------------------------*/
void do_next_watch_plane(VKCODE vk)
{
	PlaneParams *P;
	PlaneParams *orgP;

//	if (MultiPlayer)
//		return;

	if(iWatchPlane < 0)
	{
		iWatchPlane = 0;
	}

	orgP = P = &Planes[iWatchPlane];
	P++;
	if (P > LastPlane)
		P = &Planes[0];
	while((P != orgP) && (!(P->Status & PL_ACTIVE)))
	{
		P++;
		if (P > LastPlane)
			P = &Planes[0];
	}

	iWatchPlane = P - Planes;
}

void do_closest_plane(VKCODE vk)
{
	PlaneParams *P;

	P = (PlaneParams *)Camera1.AttachedObject;
	PlaneParams *ClosestPlane,*PWalk;
	double dist,closest_dist;

	if (P != PlayerPlane)
		Camera1.AttachedObject = (int *)PlayerPlane;
	else
	{
		PWalk = &Planes[0];
		ClosestPlane= NULL;
		while(PWalk <= LastPlane)
		{
			if ((PWalk != P) && (PWalk->Type != P->Type))
			{
				dist = PWalk->WorldPosition - P->WorldPosition;
				if ((!ClosestPlane) || (dist < closest_dist))
				{
					ClosestPlane = PWalk;
					closest_dist = dist;
				}
			}
			PWalk++;
		}
		if (ClosestPlane)
		{
			if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
			{
				AssignCameraTarget((void *)ClosestPlane,CO_PLANE);
				Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
				SetupView( Camera1.SubType );
			}
			else
			{
				AssignCameraSubject((void *)ClosestPlane,CO_PLANE);
				if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
				{
					setup_no_cockpit_art();
					Camera1.CameraMode = CAMERA_FIXED_CHASE;
					ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
				}
				else
					if (Camera1.CameraMode == CAMERA_FREE)
	 					ChangeViewModeOrSubject(CAMERA_RC_VIEW);
					else
	 					ChangeViewModeOrSubject(Camera1.CameraMode);
			}
		}
	}
}

void next_camera_eye(VKCODE vk)
{
	EVNextEyePoint();
}

void player_eject(VKCODE vk)
{
	if(!pPlayerChute)
	{
		for(WeaponParams *W = Weapons; W <= LastWeapon; W ++)
		{
			if (W->Flags & WEAPON_INUSE)
			{
				if((W->P == PlayerPlane) && (W->Kind == EJECTION_SEAT))
				{
					return;
				}
			}
		}

		AIRemoveFromFormation(PlayerPlane);
		BeginEjectionSequence(PlayerPlane - Planes);
	}
}

void do_ZoomInAll(VKCODE vk)
{
  if( (g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) || (g_Settings.gp.nTEWS == GP_TYPE_CASUAL) )
  {
    if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
	{
      //AARadarDecRange();
      AGRadarDecRange();
      HsiDecRange();
	  TsdDecRange();
	}

	TewsDecRange();

  }
}

void do_ZoomOutAll(VKCODE vk)
{
  if( (g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) || (g_Settings.gp.nTEWS == GP_TYPE_CASUAL) )
  {
    if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL)
	{
	  //AARadarIncRange();
      AGRadarIncRange();
      HsiIncRange();
	  TsdIncRange();
	}

    TewsIncRange();

  }
}

extern int g_nMissionType;
void do_RearmPlayer(VKCODE vk)
{
	if (MultiPlayer )  return;

	#ifndef _DEBUG

	if ((g_nMissionType == SIM_CAMPAIGN) || (g_nMissionType == SIM_SINGLE))
		return;

	#endif

	F18LoadPlayerWeaponInfo();
	LogMissionStatistic(LOG_FLIGHT_CHEATED,1,1,2);
}

void do_AARadarUp(VKCODE vk)
{
  SlewAAAntenna(0);
}

void do_AARadarDown(VKCODE vk)
{
  SlewAAAntenna(1);
}

extern void CenterAARadar();

void do_AARadarCenter(VKCODE vk)
{
	CenterAARadar();
}

void ToggleLeftEngStatus(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

	ToggleLeftEngineStatus();
}

void ToggleRightEngStatus(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

	ToggleRightEngineStatus();
}

void TrimNoseUp(VKCODE vk)
{
	NoseTrimUp();
}

void TrimNoseDown(VKCODE vk)
{
	NoseTrimDown();
}

void TrimAilLeft(VKCODE vk)
{
	AileronTrimLeft();
}
void TrimAilRight(VKCODE vk)
{
	AileronTrimRight();
}

void TrimRdrRight(VKCODE vk)
{
	RudderTrimRight();
}
void TrimRdrLeft(VKCODE vk)
{
	RudderTrimLeft();
}

void do_CenterJoy(VKCODE vk)
{
	extern void CenterJoy();
	CenterJoy();
	DefaultTrim();
}

void RudderRight(VKCODE vk)
{
	RDRRight = vk;
}

void RudderLeft(VKCODE vk)
{
	RDRLeft = vk;
}

void ILSToggle(VKCODE vk)
{
	DoUFCMainIls();
}

/* ------------------------------------------3/5/98 7:07PM---------------------------------------------
 * new detail stuff
 * ----------------------------------------------------------------------------------------------------*/

void do_reverse_yaw(VKCODE vk)
{
	g_Settings.misc.dwMisc ^= GP_MISC_VIEW_REVERSE_YAW;

}

void do_reverse_pitch(VKCODE vk)
{
	g_Settings.misc.dwMisc ^= GP_MISC_VIEW_REVERSE_PITCH;
}

void do_inc_pan_speed(VKCODE vk)
{
	if (g_Settings.misc.byACMPanSpeed < 0xF0)
		g_Settings.misc.byACMPanSpeed += 0x10;

}

void do_dec_pan_speed(VKCODE vk)
{
	if (g_Settings.misc.byACMPanSpeed > 0x10)
		g_Settings.misc.byACMPanSpeed -= 0x10;
}

void do_toggle_city_lights(VKCODE vk)
{
	TerrainYesNos ^= YN_NO_CITY_LIGHTS;
//	if (MediumTextureCache)
//		MediumTextureCache->UpdateObjects();

	TerrainLoaded = 0;
}

void do_toggle_sticky_mpd(VKCODE vk)
{
	g_Settings.misc.dwMisc ^= GP_MISC_VIEW_MPDS_POPUPS_STICKY;
}

void do_toggle_vc_view_lock(VKCODE vk)
{
	g_Settings.misc.dwMisc ^= GP_MISC_VIEW_PADLOCK_FOV_LIMITS;
}

void do_toggle_hivc(VKCODE vk)
{
	switch(g_Settings.gr.dwGraph & (GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT))
	{
		case GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT: //medium
			g_Settings.gr.dwGraph &= ~(GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT);
			g_Settings.gr.dwGraph |= GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT;
			break;

		case GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT:
			g_Settings.gr.dwGraph &= ~(GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT);
			g_Settings.gr.dwGraph |= GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT;
			break;

		case GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT:
		default:
			g_Settings.gr.dwGraph &= ~(GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT);
			g_Settings.gr.dwGraph |= GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT;
			break;
	}

	LoadVirtualCockpit(TRUE);
	InitVCInteractions();
}

void do_plane_weapons(VKCODE vk)
{
	g_Settings.gr.dwGraph ^= GP_GRAPH_WEAPONS_ON_PLANE;
}

void do_g_effects(VKCODE vk)
{
	g_Settings.gp.dwFlight ^= GP_FLIGHT_READOUT_BLACKOUT;
}

//
// NULL function - used for place holder functions
//

void NULL_FUNCTION(VKCODE vk)
{
	VKLOOKUP *pVk;

	pVk = GetVkLookup();
	while (pVk->pszFriendlyName)
	{
		if (pVk->vkCode==vk.vkCode && pVk->wFlags==vk.wFlags && pVk->dwExtraCode==vk.dwExtraCode)
		{
			dprintf("Key (%s) mapped to NULL function\n",pVk->pszFriendlyName);
			return;
		}
		pVk++;
	}

	dprintf("Key assignment mapped to NULL function\n");
}

void F182pig(VKCODE vk)
{
	TextureRef temp_ref;

	temp_ref.CellColumns = 0;
	temp_ref.TotalCells  = 0;
	temp_ref.CellWidth   = 1.0f;
	temp_ref.CellHeight  = 1.0f;

	for (int i=0; i<MAX_PLANE_TYPES; i++)
	{
		if ((PlaneTypes[i].TypeNumber == PT_F18e) && (PlaneTypes[i].Model))
		{
			if (PlaneTypes[i].Model )  Free3DObject (PlaneTypes[i].Model );
			if (PlaneTypes[i].DestroyedModel )  Free3DObject (PlaneTypes[i].DestroyedModel );
			if (PlaneTypes[i].Shadow)  Free3DTexture(PlaneTypes[i].Shadow);

			PlaneTypes[i].Model  = NULL;
			PlaneTypes[i].DestroyedModel  = NULL;
			PlaneTypes[i].Shadow = NULL;

			PlaneTypes[i].Model = Load3DObject(RegPath("objects","scrpl.3dg"));
			PlaneTypes[i].DestroyedModel = Load3DObject(RegPath("objects","buscrpl.3dg"));

			if (_3dxl)
				strcpy(temp_ref.Name,"scplshh.pcx");
			else
				strcpy(temp_ref.Name,"scplshd.pcx");

			PlaneTypes[i].Shadow = Load3DTexture(&temp_ref);

			PlaneTypes[i].ShadowULXOff 	 = -0.618f;
			PlaneTypes[i].ShadowULZOff 	 = -2.742f;
			PlaneTypes[i].ShadowLRXOff 	 = 0.698f;
			PlaneTypes[i].ShadowLRZOff   = 1.168f;

			PlaneTypes[i].GearDownHeight = -1.912f;
			PlaneTypes[i].GearUpHeight   = -1.912f;
			PlaneTypes[i].TailHeight     = 0.75f;

			PlaneTypes[i].Offsets[OFFSET_WING_LEFT].SetValues(-0.618f,-0.75f,0.0f);
			PlaneTypes[i].Offsets[OFFSET_WING_RIGHT].SetValues(0.618f,-0.75f,0.0f);

			PlaneTypes[i].Offsets[OFFSET_ENGINE_REAR_LEFT].SetValues(0.0f,8.0f*FTTOWU,-2.742f*FTTOWU*2.0f);
			PlaneTypes[i].Offsets[OFFSET_ENGINE_REAR_RIGHT].SetValues(0.0f,8.0f*FTTOWU,-2.742f*FTTOWU*2.0f);

			pigs = TRUE;

		}
	}
}

void running_lights(VKCODE vk)
{
	if (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
		PlayerPlane->Status ^= PL_RUNNING_LIGHTS;

  SetCockpitItemState(255,(PlayerPlane->Status & PL_RUNNING_LIGHTS) ? 0 : 1);
}

void formation_lights(VKCODE vk)
{
	int count;

	if (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
	{
		count = ((PlayerPlane->Status & PL_FORMATION_LIGHTS)+0x1000)&PL_FORMATION_LIGHTS;
		PlayerPlane->Status &= ~PL_FORMATION_LIGHTS;
		PlayerPlane->Status |= count;
	}

  SetCockpitItemState(124,(PlayerPlane->Status & PL_FORMATION_LIGHTS) ? 0 : 2);
}


void corvette(VKCODE vk)
{
}

void Do_JettisonAll(VKCODE vk)
{
  SetJettFromKeyboard(2);
}

void Do_JettisonSelected(VKCODE vk)
{
  SetJettFromKeyboard(3);
}

void Do_MasterArm(VKCODE vk)
{
  ToggleMasterArmSwitch();
}

void Do_FireExtAmad(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

  ActivateAmadFireExt();
}

void Do_FireExtLeng(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

  ActivateLeftEngFireExt();
}

void Do_FireExtReng(VKCODE vk)
{
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		return;

  ActivateRightEngFireExt();
}

void Do_GunRate(VKCODE vk)
{
	ToggleGunFireRate();
//  ToggleGunRate();
}

void Do_AGArmBombMode(VKCODE vk)
{
  StepThroughBombMode();
}

void Do_AGArmRWing(VKCODE vk)
{
   if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) return;
   AGArmSelectStation(AG_RIGHT);
}

void Do_AGArmRConformal(VKCODE vk)
{
   if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) return;
  AGArmSelectStation(FRONT_R);
}

void Do_AGArmCenter(VKCODE vk)
{
  if(g_Settings.gp.nAARADAR == GP_TYPE_CASUAL) return;
  AGArmSelectStation(AG_CENTER);
}

void Do_AGArmLWing(VKCODE vk)
{
  AGArmSelectStation(AG_LEFT);
}

void Do_AGArmLConformal(VKCODE vk)
{
  AGArmSelectStation(FRONT_L);
}


void Do_RadarMode(VKCODE vk)
{
	if( (UFC.MasterMode == AA_MODE) || (UFC.MasterMode == NAV_MODE) )
		AAChangeMode();
	else
		AGChangeMode();
  //if(!AreMpdsInGivenMode(AA_RADAR_MODE)) return;
}


void Do_RadarAzScan(VKCODE vk)
{
	if( (UFC.MasterMode == AA_MODE) || (UFC.MasterMode == NAV_MODE) )
		AzimScanChange();
	else
		AGAzimScanChange();
  //if(!AreMpdsInGivenMode(AA_RADAR_MODE)) return;
   // AARadarChangeAzimScanLimit();
}

void Do_RadarRangeInc(VKCODE vk)
{
	AvionicsRangeUp();
}

void Do_RadarRangeDec(VKCODE vk)
{
	AvionicsRangeDown();
}

void Do_AvDesignate(VKCODE vk)
{
	 int Mode = GetTDCMode();

	 if(Mode == -1) return;

	 switch(Mode)
	 {
	  case STORES_MODE	:  if(UFC.MasterMode == AG_MODE)
		                     {
		  									 		if(Av.Weapons.CurAGWeap)
												 		{
				 												if( ((Av.Weapons.CurAGWeap->WeapPage == AGM_65_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == AGM_65F_WEAP_PAGE)) && !(Av.Gbu.WeapFired) )
							 												MavDesignate();
				 												else if( (Av.Weapons.CurAGWeap->WeapPage == WALLEYE_WEAP_PAGE) || (Av.Gbu.WeapFired) )
							 												GbuDesignate();
												 		}
													else if(Av.Gbu.WeapFired)
													{
		 												GbuDesignate();
													}
												 }
                         break;

	  case TGT_IR_MODE	:  FlirDesignate();
                         break;

	  case RADAR_MODE :    if(UFC.MasterMode != AG_MODE)
			                       AARadarDesignateTarget();
												 else
												 {
                             AGRadarDesignateTarget();
												 }
												 break;
	 }

}

void Do_AvUnDesignate(VKCODE vk)
{
	 int Mode = GetTDCMode();

	 if(Mode == -1) return;

	 switch(Mode)
	 {
	  case STORES_MODE	:  if(UFC.MasterMode == AG_MODE)
		                     {
		  									 		if(Av.Weapons.CurAGWeap)
												 		{
				 												if( ((Av.Weapons.CurAGWeap->WeapPage == AGM_65_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == AGM_65F_WEAP_PAGE)) && !(Av.Gbu.WeapFired) )
							 												MavUnDesignate();
				 												else if( (Av.Weapons.CurAGWeap->WeapPage == WALLEYE_WEAP_PAGE) || (Av.Gbu.WeapFired) )
							 												GbuUnDesignate();
				 												else if(Av.Weapons.CurAGWeap->WeapPage == HARM_WEAP_PAGE)
							 												HarmUnDesignate();
												 		}
												 }
                         break;

	  case TGT_IR_MODE	:  FlirUnDesignate();
                         break;

	  case RADAR_MODE :    if(UFC.MasterMode != AG_MODE)
			                     AARadarUnDesignateTarget();
												 else
												 {
		                       BreakAGLock();
												 }
												 break;
	 }
}

void Do_ToggleSilent(VKCODE vk)
{
	 int Mode = GetTDCMode();

	 if(Mode == -1) return;

	 switch(Mode)
	 {
	  case RADAR_MODE :    if(UFC.MasterMode != AG_MODE)
			                     ToggleSilent();
												 else
			                     AGToggleSilent();
												 break;
	 }
}


void Do_StepTDC(VKCODE vk)
{
  ToggleTDCMpd();
}

void Do_AAAutoAcqTarget(VKCODE vk)
{
  AARadarAutoAcqTarget();
}

void Do_TFlirFov(VKCODE vk)
{
  FlirToggleFov();
}

void Do_TFlirPolarity(VKCODE vk)
{
  if(AreMpdsInGivenMode(TGT_IR_MODE))
	FlirTogglePolarity();
}

void Do_TFlirMode(VKCODE vk)
{
  if(AreMpdsInGivenMode(TGT_IR_MODE))
  	ToggleTFlirAquireMode();
}

void Do_TFlirTrack(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
 	ToggleTFlirTrack();
}

void Do_TFlirDes(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
 	ToggleTFlirCdes();
}

void kFlirContrastUp(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
	FlirContrastUp();
}

void kFlirContrastDown(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
	FlirContrastDown();
}

void kFlirBrightnessUp(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
	FlirBrightnessUp();
}

void kFlirBrightnessDown(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
	FlirBrightnessDown();
}

void kFlirDeclutter(VKCODE vk)
{
 if(AreMpdsInGivenMode(TGT_IR_MODE))
	FlirToggleDeclutter();
}

void HudFlirConUp(VKCODE vk)
{
	NavFlirContrastUp();
}

void HudFlirConDown(VKCODE vk)
{
	NavFlirContrastDown();
}

void HudFlirBrtUp(VKCODE vk)
{
	NavFlirBrightnessUp();
}

void HudFlirBrtDown(VKCODE vk)
{
	NavFlirBrightnessDown();
}


void HudFlirDeclutter(VKCODE vk)
{
  if(AreMpdsInGivenMode(NAVFLIR_MODE))
	NavFlirToggleDeclutter();

}

void HudFlirPolarity(VKCODE vk)
{
  if(AreMpdsInGivenMode(NAVFLIR_MODE))
  	ToggleTFlirPolarity();
}



void Do_TFlirUp(VKCODE vk)
{
	//TactCamSlew(0);
	//return;


  //if(!AreMpdsInGivenMode(TGT_IR_MODE)) return;
  //FlirSlewUpPress = vk;
}

void Do_TFlirDown(VKCODE vk)
{
	//TactCamSlew(1);
	//return;

  //if(!AreMpdsInGivenMode(TGT_IR_MODE)) return;
  //FlirSlewDownPress = vk;
}

void Do_TFlirLeft(VKCODE vk)
{
	//TactCamSlew(2);
	//return;

 //if(!AreMpdsInGivenMode(TGT_IR_MODE)) return;
 //FlirSlewLeftPress = vk;
}

void Do_TFlirRight(VKCODE vk)
{
	//TactCamSlew(3);
	//return;

 //if(!AreMpdsInGivenMode(TGT_IR_MODE)) return;
 //FlirSlewRightPress = vk;
}

void Do_TFlirCtr(VKCODE vk)
{
  FlirBoreSightKeypress();
}

void Do_LaserToggle(VKCODE vk)
{
  ToggleFlirLaser();
}

void Do_WpnUp(VKCODE vk)
{
	TdcUpVk = vk;
}

void Do_WpnDown(VKCODE vk)
{
	TdcDownVk = vk;
}

void Do_WpnLeft(VKCODE vk)
{
	TdcLeftVk = vk;
}

void Do_WpnRight(VKCODE vk)
{
	TdcRightVk = vk;
}

void Do_WpnCtr(VKCODE vk)
{
 CenterWpn();
}

void Do_WpnMode(VKCODE vk)
{
 ToggleWpnMode();
}

void Do_WpnFov(VKCODE vk)
{
 ToggleWpnFov();
}

void Do_WpnTrack(VKCODE vk)
{
   ToggleWpnTrack();
}

void Do_MasterCaution(VKCODE vk)
{
  MasterCautionKeypress();
  // MasterPress = vk;
}

void Do_HsiTacan(VKCODE vk)
{
  if(!AreMpdsInGivenMode(HSI_MODE)) return;
  HsiSelectTacan();
}

void Do_HsiNav(VKCODE vk)
{
  if(!AreMpdsInGivenMode(HSI_MODE)) return;
  HsiSelectNAV();
}

void Do_EmisLimit(VKCODE vk)
{
	UFC.EMISState = !UFC.EMISState;
	SetCockpitItemState(199,UFC.EMISState);
}

void Do_HsiIncRange(VKCODE vk)
{
  if(!AreMpdsInGivenMode(HSI_MODE)) return;
  HsiIncRange();
}

void Do_HsiDecRange(VKCODE vk)
{
  if(!AreMpdsInGivenMode(HSI_MODE)) return;
  HsiDecRange();
}

void Do_RadarSniff(VKCODE vk)
{
  //if( !AreMpdsInGivenMode(AA_RADAR_MODE) && !AreMpdsInGivenMode(AG_RADAR_MODE)) return;
  //AARadarToggleSniff();
}

void Do_SteeringCycle(VKCODE vk)
{
	if(AreMpdsInGivenMode(HSI_MODE))
		CycleSteeringMode();
}


void Do_Canopy_Reflections(VKCODE vk)
{
	g_Settings.gr.dwGraph ^= GP_GRAPH_CANOPY_REFLECTION;
}

extern BYTE LiftLineFade;

void ToggleLiftLine(VKCODE vk)
{
	if (g_Settings.gr.dwGraph & GP_GRAPH_LIFT_LINE)
	{
		if (LiftLineFade > 63)
			LiftLineFade -= 64;
		else
		{
			LiftLineFade = 0;
			g_Settings.gr.dwGraph &= ~GP_GRAPH_LIFT_LINE;
		}
	}
	else
	{
		g_Settings.gr.dwGraph |= GP_GRAPH_LIFT_LINE;
		LiftLineFade = 0xff;
	}
}

void Do_Volumetric_Explosions(VKCODE vk)
{
	g_Settings.gr.dwGraph ^= GP_GRAPH_VOLUMETRIC_EXPLOD;
}

void Do_Volumetric_Clouds(VKCODE vk)
{
	g_Settings.gr.dwGraph ^= GP_GRAPH_VOLUMETRIC_CLOUDS;
}


//*****************************************************************************************************************************************
// COUNTER MEASURES  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void Do_IDECMMode(VKCODE vk)
{
  ToggleIDECM();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Do_CMDDSControl(VKCODE vk)
{
  ToggleCMDDSDecoyControl();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Do_CMDDSTransmit(VKCODE vk)
{
	ToggleCMDDSTransmit();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Do_ChaffFlareProgInitiate(VKCODE vk)
{
	CounterMeasuresProgramInitiate();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Do_ChaffFlareProgSelect(VKCODE vk)
{
	ToggleTewsProgramStep();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Do_ICSTransmit(VKCODE vk)
{
  ToggleICS();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void Do_AgArmProgram(VKCODE vk)
{
  AGChangeProgram();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Do_StickLeft(VKCODE vk)
{
  KeyFlyLeftVk = vk;
}

void Do_StickRight(VKCODE vk)
{
  KeyFlyRightVk = vk;
}

void Do_StickUp(VKCODE vk)
{
  KeyFlyUpVk = vk;
}

void Do_StickDown(VKCODE vk)
{
  KeyFlyDownVk = vk;
}

void Do_CaptBar(VKCODE vk)
{
	CaptBarsVk = vk;
}

void Do_TDCUp(VKCODE vk)
{
	TdcUpVk = vk;
}

void Do_TDCDown(VKCODE vk)
{
	TdcDownVk = vk;
}

void Do_TDCLeft(VKCODE vk)
{
	TdcLeftVk = vk;
}

void Do_TDCRight(VKCODE vk)
{
	TdcRightVk = vk;
}

void Do_TewsCommandMode(VKCODE vk)
{
  if(!AreMpdsInGivenMode(TEWS_MODE)) return;
  StepThroughTewsCmdMode();
}

void HudFlir(VKCODE vk)
{
	int state;

	NavFlirClick(&state);
//	DoUFCMainNavFlir();
}

void do_awacs_tanker(VKCODE vk)
{
	int awacsnum;

	awacsnum = AIGetClosestAWACS(PlayerPlane);

	if(awacsnum >= 0)
	{
		AICRequestClosestTanker();
	}
}

void awacs_go_bullseye(VKCODE vk)
{
	 AIC_AWACSGoBullseye(-1,-1);
}

void awacs_go_digital_bullseye(VKCODE vk)
{
	AIC_AWACSGoDigitalBullseye(-1, -1);
}

void awacs_go_tactical(VKCODE vk)
{
	AIC_AWACSGoTactical(-1, -1);
}

void Do_ToggleDirArrows(VKCODE vk)
{
}

void Do_ToggleLandingGates(VKCODE vk)
{
}

extern char ToggleDisplayTestMPDs;

void Do_ToggleDestructoCam(VKCODE vk)
{
  ToggleDestructoCam();
 //ToggleDisplayTestMPDs++;
 //if(ToggleDisplayTestMPDs > 1) ToggleDisplayTestMPDs = 0;
}

extern int DebugTewsTest;

void ToggleTewsTest(VKCODE vk)
{
	DebugTewsTest = !DebugTewsTest;
}

void Do_CageUncage(VKCODE vk)
{
 	 AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;

	 if( (UFC.MasterMode == AA_MODE) && (CurAAWeap) )
	 {
		 if(CurAAWeap->WeapPage == AIM9_WEAP_PAGE)
       ToggleAim9Scan();
		 else if(CurAAWeap->WeapPage == AIM7_WEAP_PAGE)
       ToggleAim7Scan();
		 else if(CurAAWeap->WeapPage == AIM120_WEAP_PAGE)
       ToggleAim120Scan();
		 return;
	 }

	 int Mode = GetTDCMode();
	 if(Mode == -1) return;

	 int Type = -1;

	 if( (UFC.MasterMode == AG_MODE) && (Mode == STORES_MODE) )
	 {
			if(Av.Weapons.CurAGWeap)
			{
				if((Av.Weapons.CurAGWeap->WeapPage == AGM_65_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == AGM_65F_WEAP_PAGE)
						|| (Av.Weapons.CurAGWeap->WeapPage == AGM_65E_WEAP_PAGE))
					Type = MAVERIC_TYPE;
				else if(Av.Weapons.CurAGWeap->WeapPage == WALLEYE_WEAP_PAGE)
					Type = GBU15_TYPE;
			}
			else if(Av.Gbu.WeapFired)
			{
				Type = GBU15_TYPE;
			}
	}

		if(Type != -1)
       CageUnCageWeapon(Type);
}

void Do_DumpFuel(VKCODE vk)
{
	ToggleDumpFuel();
}

void Do_ZoomLeftMdi(VKCODE vk)
{

}

void Do_ZoomRightMdi(VKCODE vk)
{

}

void Do_ZoomUfc(VKCODE vk)
{

}

void Do_ZoomMpcd(VKCODE vk)
{


}

void Do_HarmOverride(VKCODE vk)
{
  ToggleHarmOverride();
}

void Do_ToggleAircraftLabels(VKCODE vk)
{
	if((!MultiPlayer) || (bMultiLabels & 1))
	{
		if(g_Settings.gp.dwCheats & GP_CHEATS_AIRCRAFT_LABELS)
			g_Settings.gp.dwCheats &= ~GP_CHEATS_AIRCRAFT_LABELS;
		else
			g_Settings.gp.dwCheats |= GP_CHEATS_AIRCRAFT_LABELS;
	}
}


void Do_ToggleGroundLabels(VKCODE vk)
{
	if((!MultiPlayer) || (bMultiLabels & 2))
	{
		if(g_Settings.gp.dwCheats & GP_CHEATS_GRND_TARG_LABELS)
			g_Settings.gp.dwCheats &= ~GP_CHEATS_GRND_TARG_LABELS;
		else
			g_Settings.gp.dwCheats |= GP_CHEATS_GRND_TARG_LABELS;
	}
}




extern PlaneParams *carrwatch;
extern int gmoveaction;

/* ------------------------------------------4/7/99 ---------------------------------------------
 * these are debug view modes
 * ----------------------------------------------------------------------------------------------------*/
void do_next_carrier_deck_plane(VKCODE vk)
{
	PlaneParams *P;
	PlaneParams *orgP;

//	if (MultiPlayer)
//		return;

	orgP = P = (PlaneParams *)Camera1.AttachedObject;
	P++;
	if (P > LastPlane)
		P = &Planes[0];
	while((P != orgP) && ((!(P->Status & PL_ACTIVE)) || (P->OnGround != 2)))
	{
		P++;
		if (P > LastPlane)
			P = &Planes[0];
	}

	iWatchPlane = P - Planes;
	carrwatch = P;
	gmoveaction |= 1;

	if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
	{
		AssignCameraTarget((void *)P,CO_PLANE);
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		SetupView( Camera1.SubType );
	}
	else
	{
		AssignCameraSubject((void *)P,CO_PLANE);
		if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
		{
			setup_no_cockpit_art();
			Camera1.CameraMode = CAMERA_FIXED_CHASE;
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
		else
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
	}
}

/* ------------------------------------------4/7/99 ---------------------------------------------
 * these are debug view modes
 * ----------------------------------------------------------------------------------------------------*/
void do_prev_carrier_deck_plane(VKCODE vk)
{
	PlaneParams *P;
	PlaneParams *orgP;

//	if (MultiPlayer)
//		return;

	orgP = P = (PlaneParams *)Camera1.AttachedObject;
	P--;
	if (P < &Planes[0])
		P = LastPlane;
	while((P != orgP) && ((!(P->Status & PL_ACTIVE)) || (P->OnGround != 2)))
	{
		P--;
		if (P < &Planes[0])
			P = LastPlane;
	}

	iWatchPlane = P - Planes;
	carrwatch = P;
	gmoveaction |= 1;

	if ((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_VIRTUAL_SEAT))
	{
		AssignCameraTarget((void *)P,CO_PLANE);
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		SetupView( Camera1.SubType );
	}
	else
	{
		AssignCameraSubject((void *)P,CO_PLANE);
		if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
		{
			setup_no_cockpit_art();
			Camera1.CameraMode = CAMERA_FIXED_CHASE;
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
		else
			if (Camera1.CameraMode == CAMERA_FREE)
	 			ChangeViewModeOrSubject(CAMERA_RC_VIEW);
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
	}
}

/* ------------------------------------------4/7/99 ---------------------------------------------
 * these are debug view modes
 * ----------------------------------------------------------------------------------------------------*/
void do_carrier_deck_plane_right(VKCODE vk)
{
	gmoveaction |= 0x2;
}

void do_carrier_deck_plane_right_fast(VKCODE vk)
{
	gmoveaction |= 0x22;
}

void do_carrier_deck_plane_left(VKCODE vk)
{
	gmoveaction |= 0x4;
}

void do_carrier_deck_plane_left_fast(VKCODE vk)
{
	gmoveaction |= 0x24;
}

void do_carrier_deck_plane_forward(VKCODE vk)
{
	gmoveaction |= 0x8;
}

void do_carrier_deck_plane_forward_fast(VKCODE vk)
{
	gmoveaction |= 0x28;
}

void do_carrier_deck_plane_back(VKCODE vk)
{
	gmoveaction |= 0x10;
}

void do_carrier_deck_plane_back_fast(VKCODE vk)
{
	gmoveaction |= 0x30;
}

void do_carrier_deck_plane_clockwise(VKCODE vk)
{
	gmoveaction |= 0x100;
}

void do_carrier_deck_plane_clockwise_fast(VKCODE vk)
{
	gmoveaction |= 0x500;
}

void do_carrier_deck_plane_counterclockwise(VKCODE vk)
{
	gmoveaction |= 0x200;
}

void do_carrier_deck_plane_counterclockwise_fast(VKCODE vk)
{
	gmoveaction |= 0x600;
}

extern double gftx, gfty, gftz;
extern double ganghead;
void do_carrier_deck_plane_print_pos(VKCODE vk)
{
	char tstr[256];

	sprintf(tstr, "Plane %d, X %2.2f, Z %2.2f, H %2.2f", carrwatch->AI.lVar2, gftx, gftz, ganghead);
	AICAddAIRadioMsgs(tstr, 50);
}

/* ------------------------------------------4/7/99 ---------------------------------------------
 * END of these debug view modes
 * ----------------------------------------------------------------------------------------------------*/

void lso_view(VKCODE vk)
{
	FPointDouble camerapos;
	FPointDouble offset;
	PlaneParams *planepnt;

	if(!(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER))
	{
		return;
	}

	if(Camera1.AttachedObjectType != CO_PLANE)
	{
		return;
	}

	planepnt = (PlaneParams *)Camera1.AttachedObject;
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
	camerapos = carrier->WorldPosition;

	setup_no_cockpit_art();

	setup_no_cockpit_art();
	ChangeViewModeOrSubject(CAMERA_RC_VIEW);

	iCarrierWatch = 1;

	offset.X = -64.0f * FTTOWU;
	offset.Y = 72.0f * FTTOWU;
	offset.Z = 416.0f * FTTOWU;

	offset *= carrier->Attitude;

	camerapos.X = carrier->WorldPosition.X + offset.X;
	camerapos.Y = carrier->WorldPosition.Y + offset.Y;
	camerapos.Z = carrier->WorldPosition.Z + offset.Z;

	Camera1.CameraLocation.X = camerapos.X;
	Camera1.CameraLocation.Y = camerapos.Y;
	Camera1.CameraLocation.Z = camerapos.Z;

	Camera1.CameraTargetLocation = Camera1.CameraLocation;

	offset.MakeVectorToFrom(planepnt->WorldPosition,Camera1.CameraLocation);
	Camera1.Roll = 0;
	Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
	Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

	camera_setup_initial_relative_quats();
}

void carrier_tower_view(VKCODE vk)
{
	FPointDouble camerapos;
	FPointDouble offset;
	PlaneParams *planepnt;

	if(!(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER))
	{
		return;
	}

	if(Camera1.AttachedObjectType != CO_PLANE)
	{
		return;
	}

	planepnt = (PlaneParams *)Camera1.AttachedObject;
	MovingVehicleParams *carrier;
	carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];
	camerapos = carrier->WorldPosition;

	setup_no_cockpit_art();

	setup_no_cockpit_art();
	ChangeViewModeOrSubject(CAMERA_RC_VIEW);

	iCarrierWatch = 0;

	offset.X = 67.0f * FTTOWU;
	offset.Y = 91.0f * FTTOWU;
	offset.Z = 135.0f * FTTOWU;

	offset *= carrier->Attitude;

	camerapos.X = carrier->WorldPosition.X + offset.X;
	camerapos.Y = carrier->WorldPosition.Y + offset.Y;
	camerapos.Z = carrier->WorldPosition.Z + offset.Z;

	Camera1.CameraLocation.X = camerapos.X;
	Camera1.CameraLocation.Y = camerapos.Y;
	Camera1.CameraLocation.Z = camerapos.Z;

	Camera1.CameraTargetLocation = Camera1.CameraLocation;

	offset.MakeVectorToFrom(planepnt->WorldPosition,Camera1.CameraLocation);
	Camera1.Roll = 0;
	Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
	Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

	camera_setup_initial_relative_quats();
}

void speech_mute_toggle(VKCODE vk)
{
	switch(iSpeechMute)
	{
		case 1:
			iSpeechMute = 3;
			break;
		case 3:
			iSpeechMute = 0;
			break;
		case 0:
		default:
			iSpeechMute = 1;
			break;
	}
}

void call_ball(VKCODE vk)
{
	if((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14)))))
		return;

	SendNetKey(25, '5');
	AICPlayerSeesBall(PlayerPlane - Planes);
}

void call_clara(VKCODE vk)
{
	if((PlayerPlane->OnGround) || (!((PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING) && ((PlayerPlane->AI.lVar2 == 13) || (PlayerPlane->AI.lVar2 == 14)))))
		return;

	SendNetKey(25, '6');
	AICPlayerClara(PlayerPlane - Planes);
}

void request_emergency(VKCODE vk)
{
	SendNetKey(25, '7');
	AICRequestEmergencyApproach(PlayerPlane - Planes);
}

void aic_tower_inbound(VKCODE vk)
{
	SendNetKey(25, '4');
	AICTowerInbound(PlayerPlane - Planes);
}

void aic_tower_taxi(VKCODE vk)
{
	SendNetKey(25, '3');
	AICPlayerRequestTaxi(PlayerPlane - Planes);
}

void report_spike(VKCODE vk)
{
	SendNetKey(28, '1');
}

void report_sort(VKCODE vk)
{
	if(MultiPlayer)
		AICDoCommSort(PlayerPlane);
}

void report_contacts(VKCODE vk)
{
	if(MultiPlayer)
		AIC_WSO_Report_Contacts(PlayerPlane - Planes, 1);
}

void report_grnd_target(VKCODE vk)
{
	if(MultiPlayer)
		AICDoCommTargetSpot();
}

void report_strobe(VKCODE vk)
{
	if(MultiPlayer)
		AICDoCommStrobeSpot();
}

void multi_threat_bandit(VKCODE vk)
{
	SendNetKey(29, '1');
}

void multi_threat_sam(VKCODE vk)
{
	SendNetKey(29, '2');
}

void multi_threat_missile(VKCODE vk)
{
	SendNetKey(29, '3');
}

void multi_threat_aaa(VKCODE vk)
{
	SendNetKey(29, '4');
}

void multi_winchester(VKCODE vk)
{
	SendNetKey(27, '3');
}

void multi_engaged_offensive(VKCODE vk)
{
	SendNetKey(30, '1');
}

void multi_engaged_defensive(VKCODE vk)
{
	SendNetKey(30, '2');
}

void multi_target_destroyed(VKCODE vk)
{
	SendNetKey(30, '3');
}

void multi_attacking_ground(VKCODE vk)
{
	SendNetKey(30, '4');
}

void multi_taken_damage(VKCODE vk)
{
	SendNetKey(27, '5');
}

void multi_bingo(VKCODE vk)
{
	SendNetKey(27, '6');
}

void multi_rtb(VKCODE vk)
{
	SendNetKey(27, '7');
}

extern BOOL DrawMeatball;
extern MovingVehicleParams *MeatBallCarrier;

void toggle_iflols(VKCODE vk)
{
	if (DrawMeatball)
		DrawMeatball = FALSE;
	else
		if (MeatBallCarrier)
			DrawMeatball = TRUE;
}

void wingman_radio_toggle(VKCODE vk)
{
	iWingmanRadio ^= 1;
	//  Put some Status Output message here.

	char tempstr[80];
	if(iWingmanRadio)
	{
		switch(PRIMARYLANGID(g_iLanguageId))
		{
			case LANG_GERMAN:
				sprintf(tempstr, "Funkverkehr mit Flügelmann an");
				break;
			default:
				sprintf(tempstr, "Wingman Radio On");
				break;
		}
	}
	else
	{
		switch(PRIMARYLANGID(g_iLanguageId))
		{
			case LANG_GERMAN:
				sprintf(tempstr, "Funkverkehr mit Flügelmann aus");
				break;
			default:
				sprintf(tempstr, "Wingman Radio Off");
				break;
		}
	}

	AICAddAIRadioMsgs(tempstr, 69);
}

void atc_radio_toggle(VKCODE vk)
{
	iATCRadio ^= 1;

	//  Put some Status Output message here.
	char tempstr[80];
	if(iATCRadio)
	{
		switch(PRIMARYLANGID(g_iLanguageId))
		{
			case LANG_GERMAN:
				sprintf(tempstr, "Funkverkehr mit Flugsicherung an");
				break;
			default:
				sprintf(tempstr, "ATC Radio On");
				break;
		}
	}
	else
	{
		switch(PRIMARYLANGID(g_iLanguageId))
		{
			case LANG_GERMAN:
				sprintf(tempstr, "Funkverkehr mit Flugsicherung aus");
				break;
			default:
				sprintf(tempstr, "ATC Radio Off");
				break;
		}
	}

	AICAddAIRadioMsgs(tempstr, 69);
}

void other_radio_toggle(VKCODE vk)
{
	iOtherRadio ^= 1;
	//  Put some Status Output message here.

	char tempstr[80];
	if(iOtherRadio)
	{
		switch(PRIMARYLANGID(g_iLanguageId))
		{
			case LANG_GERMAN:
				sprintf(tempstr, "Sonstiger Flugverkehr an");
				break;
			default:
				sprintf(tempstr, "Other Radio On");
				break;
		}
	}
	else
	{
		switch(PRIMARYLANGID(g_iLanguageId))
		{
			case LANG_GERMAN:
				sprintf(tempstr, "Sonstiger Flugverkehr aus");
				break;
			default:
				sprintf(tempstr, "Other Radio Off");
				break;
		}
	}

	AICAddAIRadioMsgs(tempstr, 69);
}

//void kFlirContrastUp(VKCODE vk)
//{
//	FlirContrastDown();
//}

//void kFlirContrastDown(VKCODE vk)
//{
//	FlirContrastDown();
//}

//void kFlirBrightnessUp(VKCODE vk)
//{
//	FlirBrightnessUp();
//}

//void kFlirBrightnessDown(VKCODE vk)
//{
//	FlirBrightnessDown();
//}


//
// function mappings
//

void tdc_lmdi(VKCODE vk)
{
	TDCMpdNum = 0;
}

void tdc_rmdi(VKCODE vk)
{
	TDCMpdNum = 1;
}

void tdc_mpcd(VKCODE vk)
{
	TDCMpdNum = 2;
}

void tdc_ufc(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		TDCMpdNum = MPD6;
	}
}

void lmdi_pb1(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 4;
}

void lmdi_pb2(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 3;
}

void lmdi_pb3(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 2;
}

void lmdi_pb4(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 1;
}

void lmdi_pb5(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 0;
}

void lmdi_pb6(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 19;
}

void lmdi_pb7(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 18;
}

void lmdi_pb8(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 17;
}

void lmdi_pb9(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 16;
}

void lmdi_pb10(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 15;
}

void lmdi_pb11(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 14;
}

void lmdi_pb12(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 13;
}

void lmdi_pb13(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 12;
}

void lmdi_pb14(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 11;
}

void lmdi_pb15(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 10;
}

void lmdi_pb16(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 9;
}

void lmdi_pb17(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 8;
}

void lmdi_pb18(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 7;
}

void lmdi_pb19(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 6;
}

void lmdi_pb20(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 0;
	MpdButton.ButtonNum = 5;
}

void rmdi_pb1(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 4;
}

void rmdi_pb2(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 3;
}

void rmdi_pb3(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 2;
}

void rmdi_pb4(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 1;
}

void rmdi_pb5(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 0;
}

void rmdi_pb6(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 19;
}

void rmdi_pb7(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 18;
}

void rmdi_pb8(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 17;
}

void rmdi_pb9(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 16;
}

void rmdi_pb10(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 15;
}

void rmdi_pb11(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 14;
}

void rmdi_pb12(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 13;
}

void rmdi_pb13(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 12;
}

void rmdi_pb14(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 11;
}

void rmdi_pb15(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 10;
}

void rmdi_pb16(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 9;
}

void rmdi_pb17(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 8;
}

void rmdi_pb18(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 7;
}

void rmdi_pb19(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 6;
}

void rmdi_pb20(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 1;
	MpdButton.ButtonNum = 5;
}

void mpcd_pb1(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 4;
}

void mpcd_pb2(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 3;
}

void mpcd_pb3(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 2;
}

void mpcd_pb4(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 1;
}

void mpcd_pb5(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 0;
}

void mpcd_pb6(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 19;
}

void mpcd_pb7(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 18;
}

void mpcd_pb8(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 17;
}

void mpcd_pb9(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 16;
}

void mpcd_pb10(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 15;
}

void mpcd_pb11(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 14;
}

void mpcd_pb12(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 13;
}

void mpcd_pb13(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 12;
}

void mpcd_pb14(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 11;
}

void mpcd_pb15(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 10;
}

void mpcd_pb16(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 9;
}

void mpcd_pb17(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 8;
}

void mpcd_pb18(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 7;
}

void mpcd_pb19(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 6;
}

void mpcd_pb20(VKCODE vk)
{
	MpdButton.Pressed = TRUE;
	MpdButton.MpdNum = 2;
	MpdButton.ButtonNum = 5;
}

void ufc_pb1(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 4;
	}
}

void ufc_pb2(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 3;
	}
}

void ufc_pb3(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 2;
	}
}

void ufc_pb4(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 1;
	}
}

void ufc_pb5(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 0;
	}
}

void ufc_pb6(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 19;
	}
}

void ufc_pb7(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 18;
	}
}

void ufc_pb8(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 17;
	}
}

void ufc_pb9(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 16;
	}
}

void ufc_pb10(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 15;
	}
}

void ufc_pb11(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 14;
	}
}

void ufc_pb12(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 13;
	}
}

void ufc_pb13(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 12;
	}
}

void ufc_pb14(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 11;
	}
}

void ufc_pb15(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 10;
	}
}

void ufc_pb16(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 9;
	}
}

void ufc_pb17(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 8;
	}
}

void ufc_pb18(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 7;
	}
}

void ufc_pb19(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 6;
	}
}

void ufc_pb20(VKCODE vk)
{
	if(Av.Ufc.Mode == UFC_MDI_MODE)
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = MPD6;
		MpdButton.ButtonNum = 5;
	}
}

void tdc_pb1(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 4;
	}
}

void tdc_pb2(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 3;
	}
}

void tdc_pb3(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 2;
	}
}

void tdc_pb4(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 1;
	}
}

void tdc_pb5(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 0;
	}
}

void tdc_pb6(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 19;
	}
}

void tdc_pb7(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 18;
	}
}

void tdc_pb8(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 17;
	}
}

void tdc_pb9(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 16;
	}
}

void tdc_pb10(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 15;
	}
}

void tdc_pb11(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 14;
	}
}

void tdc_pb12(VKCODE vk)
{
	if( (Av.Ufc.Mode == UFC_MDI_MODE)  )
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 13;
	}
}

void tdc_pb13(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 12;
	}
}

void tdc_pb14(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 11;
	}
}

void tdc_pb15(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 10;
	}
}

void tdc_pb16(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 9;
	}
}

void tdc_pb17(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 8;
	}
}

void tdc_pb18(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 7;
	}
}

void tdc_pb19(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 6;
	}
}

void tdc_pb20(VKCODE vk)
{
	if((Av.Ufc.Mode == UFC_MDI_MODE) || (TDCMpdNum == 0) || (TDCMpdNum == 1) || (TDCMpdNum == 2))
	{
		MpdButton.Pressed = TRUE;
		MpdButton.MpdNum = TDCMpdNum;
		MpdButton.ButtonNum = 5;
	}
}

KEYBOARDMAP *GetKeyboardMapList(void)
{
	return keyMap;
}


KEYBOARDMAP keyMap[] = {
	{"AUTOPILOT",				toggle_autopilot,				MB_KID_AUTOPILOT,					KEYF_A_DEP,										0},
	{"SPEED_BRAKE",				toggle_speedbreak,				MB_KID_SPEEDBREAK,					KEYF_F_DEP,										0},
	{"PAUSE",					pause_sim,						MB_KID_PAUSE,						KEYF_M_DEP|KEYF_ALLOW_PAUSE,					KSYS_PAUSE},
	{"FLAPS",					toggle_flaps,					MB_KID_TOGGLE_FLAPS,				KEYF_F_DEP,										0},
	{"LANDING_GEAR",			toggle_gears,					MB_KID_TOGGLE_GEAR,					KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WHEEL_BRAKES",			toggle_brakes,					MB_KID_TOGGLE_BRAKES,				KEYF_F_DEP,										0},
	{"TAILHOOK",				toggle_hook,					MB_KID_TOGGLE_HOOK,				 	KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"REFUELING_PROBE",			toggle_probe,					MB_KID_TOGGLE_PROBE,				KEYF_F_DEP,										0},
	{"WAYPOINT_PREVIOUS",		prev_waypoint,					MB_KID_NEXT_WAYPOINT,				KEYF_A_DEP,										0},
	{"WAYPOINT_NEXT",			next_waypoint,					MB_KID_NEXT_WAYPOINT,				KEYF_A_DEP,										0},
	{"NAVFLIR_MODE",			HudFlir,						MB_KID_NAVFLIR_MODE,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"NAVFLIR_CON_UP",			HudFlirConUp,					MB_KID_NAVFLIR_CON_UP,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"NAVFLIR_CON_DWN",			HudFlirConDown,					MB_KID_NAVFLIR_CON_DWN,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"NAVFLIR_BRT_UP",			HudFlirBrtUp,					MB_KID_NAVFLIR_BRT_UP,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"NAVFLIR_BRT_DWN",			HudFlirBrtDown,					MB_KID_NAVFLIR_BRT_DWN,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"NAVFLIR_DECLUTTER",		HudFlirDeclutter,				MB_KID_NAVFLIR_DECLUTTER,			KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"G_LIMIT_OVERRIDE",		GLimitOverride,					MB_KID_G_LIMIT_OVERRIDE,			KEYF_F_DEP,										0},
	{"SCREEN_CAPTURE",			do_screen_capture,				MB_KID_SCREEN_CAPTURE,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"INC_TIME",				increase_time,					MB_KID_INC_TIME,					KEYF_M_DEP|KEYF_ALLOW_PAUSE,					0},
	{"DEC_TIME",				decrease_time,					MB_KID_DEC_TIME,					KEYF_M_DEP|KEYF_ALLOW_PAUSE,					0},
	{"HUD_COLOR",				do_hud_color,					MB_KID_HUD_COLOR,					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"SIM_DONE",				confirm_done,					MB_KID_SIM_DONE,					KEYF_M_DEP,										0},
	{"AIM9_SELECT",				do_Aim9Select,					MB_KID_SRM_SELECT,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"AIM7_SELECT",				do_Aim7Select,					MB_KID_MRM_SELECT,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"AIM120_SELECT",			do_Aim120Select,				MB_KID_MRM_SELECT,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WEAPON_STEP",				do_MissleReject,				MB_KID_MISSILE_REJECT,				KEYF_W_DEP|KEYF_ALLOW_PAUSE,				0},
	{"ACQ_WIDE",				set_acq_wide,					MB_KID_ACQ_SS,						KEYF_S_DEP,										0},
	{"ACQ_BST",					 set_acq_bst,					MB_KID_ACQ_BST,						KEYF_S_DEP,										0},
	{"ACQ_VERTICAL",			set_acq_vertical,				MB_KID_ACQ_LRBST,					KEYF_S_DEP,										0},
	{"GUN_TRIGGER",				set_primary,					MB_KID_GUN_TRIGGER,					KEYF_W_DEP|KEYF_REPEAT,							0},
	{"WEAPON_PICKLE",			set_secondary,					MB_KID_WEAPON_PICKLE,				KEYF_W_DEP|KEYF_REPEAT,							0},
	{"MASTER_MODE_CYCLE",		master_mode_step,				MB_KID_MASTER_MODE,					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MASTER_MODE_AA",			ufc_aa_mode,					MB_KID_MASTER_AA,					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MASTER_MODE_AG",			ufc_ag_mode,					MB_KID_MASTER_AG,					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MASTER_MODE_NAV",			ufc_nav_mode,					MB_KID_MASTER_NAV,					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CHAFF",					drop_chaff,						MB_KID_CHAFF,						KEYF_A_DEP,										0},
	{"FLARE",					drop_flare,						MB_KID_FLARES,						KEYF_A_DEP,										0},
	{"TWR_REQUEST_LANDING",		aic_request_landing,			MB_KID_TWR_REQUEST_LANDING,			KEYF_C_DEP,										0},
	{"TWR_REQUEST_TAKEOFF",		aic_request_takeoff,			MB_KID_TWR_REQUEST_TAKEOFF,			KEYF_C_DEP,										0},
	{"TWR_INBOUND",				aic_tower_inbound,				MB_KID_TWR_INBOUND,					KEYF_C_DEP,										0},
	{"TWR_TAXI",				aic_tower_taxi,					MB_KID_TWR_TAXI,					KEYF_C_DEP,										0},
	{"TOGGLE_IFLOLS",           toggle_iflols,					MB_KID_TOGGLE_IFLOLS,				KEYF_A_DEP | KEYF_ALLOW_PAUSE,					0},
	{"WM_ENGAGE",				aic_w_engage_bandits,			MB_KID_WM_ENGAGE,					KEYF_C_DEP,										0},
	{"WM_COVER",				aic_w_cover,					MB_KID_WM_COVER,					KEYF_C_DEP,										0},
	{"WM_SANITIZE_RGT",	   		aic_w_sanitize_right,			MB_KID_WM_SANITIZE_RIGHT,			KEYF_C_DEP,										0},
	{"WM_SANITIZE_LFT",			aic_w_sanitize_left,			MB_KID_WM_SANITIZE_LEFT,			KEYF_C_DEP,										0},
	{"WM_ORBIT",				aic_w_orbit_here,				MB_KID_WM_ORBIT,					KEYF_C_DEP,										0},
	{"WM_REJOIN_FLIGHT",		aic_w_rejoin_flight,			MB_KID_WM_REJOIN_FLIGHT,			KEYF_C_DEP,										0},
	{"WM_BREAK_RGT",			aic_w_break_right,				MB_KID_WM_BREAK_RIGHT,				KEYF_C_DEP,										0},
	{"WM_BREAK_LFT",			aic_w_break_left,				MB_KID_WM_BREAK_LEFT,				KEYF_C_DEP,										0},
	{"WM_BREAK_HIGH",			aic_w_break_high,				MB_KID_WM_BREAK_HIGH,				KEYF_C_DEP,										0},
	{"WM_BREAK_LOW",			aic_w_break_low,				MB_KID_WM_BREAK_LOW,				KEYF_C_DEP,										0},
	{"WM_BRACKET_RGT",	   		aic_w_bracket_right,			MB_KID_WM_BRACKET_RIGHT,			KEYF_C_DEP,										0},
	{"WM_BRACKET_LFT",			aic_w_bracket_left,				MB_KID_WM_BRACKET_LEFT,				KEYF_C_DEP,										0},
	{"WM_SPLIT_HIGH",			aic_w_split_high,				MB_KID_WM_SPLIT_HIGH,				KEYF_C_DEP,										0},
	{"WM_SPLIT_LOW",			aic_w_split_low,				MB_KID_WM_SPLIT_LOW,				KEYF_C_DEP,										0},
	{"WM_DRAG_RGT",				aic_w_drag_right,				MB_KID_WM_DRAG_RIGHT,				KEYF_C_DEP,										0},
	{"WM_DRAG_LFT",				aic_w_drag_left,				MB_KID_WM_DRAG_LEFT,				KEYF_C_DEP,										0},
	{"WM_ATK_PRIMARY",			aic_w_grnd_attack_prime,		MB_KID_WM_ATK_PRIMARY,				KEYF_C_DEP,										0},
	{"WM_ATK_SECONDARY",		aic_w_grnd_attack_second,		MB_KID_WM_ATK_SECONDARY,			KEYF_C_DEP,										0},
	{"WM_ATK_ANY_GRND",			aic_w_grnd_attack_opportunity,	MB_KID_WM_ATK_ANY_GRND,				KEYF_C_DEP,										0},
	{"WM_ATK_AIR_DEF",			aic_w_grnd_attack_sead,			MB_KID_WM_ATK_AIR_DEF,				KEYF_C_DEP,										0},
	{"WM_HELP",					aic_w_help,						MB_KID_WM_HELP,						KEYF_C_DEP,										0},
	{"WM_ATK_MY_TARGET",		aic_w_attack_my_targ,			MB_KID_WM_ATK_MY_TARGET,			KEYF_C_DEP,										0},
	{"WM_RETURN_TO_BASE",		aic_w_rtb,						MB_KID_WM_RETURN_TO_BASE,			KEYF_C_DEP,										0},
	{"WM_ALL_DUMB",				aic_w_all_dumb,					MB_KID_WM_ALL_DUMB,					KEYF_C_DEP,										0},
	{"WM_HALF_DUMB",			aic_w_half_dumb,				MB_KID_WM_HALF_DUMB,				KEYF_C_DEP,										0},
	{"WM_SINGLE_DUMB",			aic_w_single_dumb,				MB_KID_WM_SINGLE_DUMB,				KEYF_C_DEP,										0},
	{"WM_ALL_GUIDED",			aic_w_all_guided,				MB_KID_WM_ALL_GUIDED,				KEYF_C_DEP,										0},
	{"WM_HALF_GUIDED",			aic_w_half_guided,				MB_KID_WM_HALF_GUIDED,				KEYF_C_DEP,										0},
	{"WM_SINGLE_GUIDED",		aic_w_single_guided,			MB_KID_WM_SINGLE_GUIDED,			KEYF_C_DEP,										0},
	{"WM_ALL_AGM",				aic_w_all_missile,				MB_KID_WM_ALL_AGM,					KEYF_C_DEP,										0},
	{"WM_HALF_AGM",				aic_w_half_missile,				MB_KID_WM_HALF_AGM,					KEYF_C_DEP,										0},
	{"WM_SINGLE_AGM",			aic_w_single_missile,			MB_KID_WM_SINGLE_AGM,				KEYF_C_DEP,										0},
	{"WM_DEFAULT_WEAPON",		aic_w_default_weapon,			MB_KID_WM_DEFAULT_WEAPON,			KEYF_C_DEP,										0},
	{"ELM_ENGAGE",				aic_e_engage_bandits,			MB_KID_ELM_ENGAGE,					KEYF_C_DEP,										0},
	{"ELM_SANITIZE_RGT",		aic_e_sanitize_right,			MB_KID_ELM_SANITIZE_RIGHT,			KEYF_C_DEP,										0},
	{"ELM_SANITIZE_LFT",		aic_e_sanitize_left,			MB_KID_ELM_SANITIZE_LEFT,			KEYF_C_DEP,										0},
	{"ELM_COVER",				aic_e_cover,					MB_KID_ELM_COVER,					KEYF_C_DEP,										0},
	{"ELM_ORBIT",				aic_e_orbit_here,				MB_KID_ELM_ORBIT,					KEYF_C_DEP,										0},
	{"ELM_BRACKET_RGT",			aic_e_bracket_right,			MB_KID_ELM_BRACKET_RIGHT,			KEYF_C_DEP,										0},
	{"ELM_BRACKET_LFT",			aic_e_bracket_left,				MB_KID_ELM_BRACKET_LEFT,			KEYF_C_DEP,										0},
	{"ELM_SPLIT_HIGH",			aic_e_split_high,				MB_KID_ELM_SPLIT_HIGH,				KEYF_C_DEP,										0},
	{"ELM_SPLIT_LOW",			aic_e_split_low,				MB_KID_ELM_SPLIT_LOW,				KEYF_C_DEP,										0},
	{"ELM_DRAG_RGT",			aic_e_drag_right,				MB_KID_ELM_DRAG_RIGHT,				KEYF_C_DEP,										0},
	{"ELM_DRAG_LFT",			aic_e_drag_left,				MB_KID_ELM_DRAG_LEFT,				KEYF_C_DEP,										0},
	{"ELM_ATK_PRIMARY",			aic_e_grnd_attack_prime,		MB_KID_ELM_ATK_PRIMARY,				KEYF_C_DEP,										0},
	{"ELM_ATK_SECONDARY",		aic_e_grnd_attack_second,		MB_KID_ELM_ATK_SECONDARY,			KEYF_C_DEP,										0},
	{"ELM_ATK_ANY_GRND",		aic_e_grnd_attack_opportunity,	MB_KID_ELM_ATK_ANY_GRND,			KEYF_C_DEP,										0},
	{"ELM_ATK_AIR_DEF",			aic_e_grnd_attack_sead,			MB_KID_ELM_ATK_AIR_DEF,				KEYF_C_DEP,										0},
	{"ELM_REJOIN_FLIGHT",		aic_e_rejoin_flight,			MB_KID_ELM_REJOIN_FLIGHT,			KEYF_C_DEP,										0},
	{"ELM_RETURN_TO_BASE",		aic_e_rtb,						MB_KID_ELM_RETURN_TO_BASE,			KEYF_C_DEP,										0},
	{"ELM_ALL_DUMB",			aic_e_all_dumb,					MB_KID_ELM_ALL_DUMB,				KEYF_C_DEP,										0},
	{"ELM_HALF_DUMB",			aic_e_half_dumb,				MB_KID_ELM_HALF_DUMB,				KEYF_C_DEP,										0},
	{"ELM_SINGLE_DUMB",			aic_e_single_dumb,				MB_KID_ELM_SINGLE_DUMB,				KEYF_C_DEP,										0},
	{"ELM_ALL_GUIDED",			aic_e_all_guided,				MB_KID_ELM_ALL_GUIDED,				KEYF_C_DEP,										0},
	{"ELM_HALF_GUIDED",			aic_e_half_guided,				MB_KID_ELM_HALF_GUIDED,				KEYF_C_DEP,										0},
	{"ELM_SINGLE_GUIDED",		aic_e_single_guided,			MB_KID_ELM_SINGLE_GUIDED,			KEYF_C_DEP,										0},
	{"ELM_ALL_AGM",				aic_e_all_missile,				MB_KID_ELM_ALL_AGM,					KEYF_C_DEP,										0},
	{"ELM_HALF_AGM",			aic_e_half_missile,				MB_KID_ELM_HALF_AGM,				KEYF_C_DEP,										0},
	{"ELM_SINGLE_AGM",			aic_e_single_missile,			MB_KID_ELM_SINGLE_AGM,				KEYF_C_DEP,										0},
	{"ELM_DEFAULT_WEAPON",		aic_e_default_weapon,			MB_KID_ELM_DEFAULT_WEAPON,			KEYF_C_DEP,										0},
	{"DIV_ENGAGE",				aic_d_engage_bandits,			MB_KID_DIV_ENGAGE,					KEYF_C_DEP,										0},
	{"DIV_SANITIZE_RGT",		aic_d_sanitize_right,			MB_KID_DIV_SANITIZE_RIGHT,			KEYF_C_DEP,										0},
	{"DIV_SANITIZE_LFT",		aic_d_sanitize_left,			MB_KID_DIV_SANITIZE_LEFT,			KEYF_C_DEP,										0},
	{"DIV_COVER",				aic_d_cover,					MB_KID_DIV_COVER,					KEYF_C_DEP,										0},
	{"DIV_ORBIT",				aic_d_orbit_here,				MB_KID_DIV_ORBIT,					KEYF_C_DEP,										0},
	{"DIV_BRACKET_RGT",			aic_d_bracket_right,			MB_KID_DIV_BRACKET_RIGHT,			KEYF_C_DEP,										0},
	{"DIV_BRACKET_LFT",			aic_d_bracket_left,				MB_KID_DIV_BRACKET_LEFT,			KEYF_C_DEP,										0},
	{"DIV_SPLIT_HIGH",			aic_d_split_high,				MB_KID_DIV_SPLIT_HIGH,				KEYF_C_DEP,										0},
	{"DIV_SPLIT_LOW",			aic_d_split_low,				MB_KID_DIV_SPLIT_LOW,				KEYF_C_DEP,										0},
	{"DIV_DRAG_RGT",			aic_d_drag_right,				MB_KID_DIV_DRAG_RIGHT,				KEYF_C_DEP,										0},
	{"DIV_DRAG_LFT",			aic_d_drag_left,				MB_KID_DIV_DRAG_LEFT,				KEYF_C_DEP,										0},
	{"DIV_ATK_PRIMARY",			aic_d_grnd_attack_prime,		MB_KID_DIV_ATK_PRIMARY,				KEYF_C_DEP,										0},
	{"DIV_ATK_SECONDARY",		aic_d_grnd_attack_second,		MB_KID_DIV_ATK_SECONDARY,			KEYF_C_DEP,										0},
	{"DIV_ATK_ANY_GRND",		aic_d_grnd_attack_opportunity,	MB_KID_DIV_ATK_ANY_GRND,			KEYF_C_DEP,										0},
	{"DIV_ATK_AIR_DEF",			aic_d_grnd_attack_sead,			MB_KID_DIV_ATK_AIR_DEF,				KEYF_C_DEP,										0},
	{"DIV_REJOIN_FLIGHT",		aic_d_rejoin_flight,			MB_KID_DIV_REJOIN_FLIGHT,			KEYF_C_DEP,										0},
	{"DIV_RETURN_TO_BASE",		aic_d_rtb,						MB_KID_DIV_RETURN_TO_BASE,			KEYF_C_DEP,										0},
	{"DIV_ALL_DUMB",			aic_d_all_dumb,					MB_KID_DIV_ALL_DUMB,				KEYF_C_DEP,										0},
	{"DIV_HALF_DUMB",			aic_d_half_dumb,				MB_KID_DIV_HALF_DUMB,				KEYF_C_DEP,										0},
	{"DIV_SINGLE_DUMB",			aic_d_single_dumb,				MB_KID_DIV_SINGLE_DUMB,				KEYF_C_DEP,										0},
	{"DIV_ALL_GUIDED",			aic_d_all_guided,				MB_KID_DIV_ALL_GUIDED,				KEYF_C_DEP,										0},
	{"DIV_HALF_GUIDED",			aic_d_half_guided,				MB_KID_DIV_HALF_GUIDED,				KEYF_C_DEP,										0},
	{"DIV_SINGLE_GUIDED",		aic_d_single_guided,			MB_KID_DIV_SINGLE_GUIDED,			KEYF_C_DEP,										0},
	{"DIV_ALL_AGM",				aic_d_all_missile,				MB_KID_DIV_ALL_AGM,					KEYF_C_DEP,										0},
	{"DIV_HALF_AGM",			aic_d_half_missile,				MB_KID_DIV_HALF_AGM,				KEYF_C_DEP,										0},
	{"DIV_SINGLE_AGM",			aic_d_single_missile,			MB_KID_DIV_SINGLE_AGM,				KEYF_C_DEP,										0},
	{"DIV_DEFAULT_WEAPON",		aic_d_default_weapon,			MB_KID_DIV_DEFAULT_WEAPON,			KEYF_C_DEP,										0},
	{"FLT_ENGAGE",				aic_f_engage_bandits,			MB_KID_FLT_ENGAGE,					KEYF_C_DEP,										0},
	{"FLT_RADAR_TOGGLE",		aic_f_radar_toggle,				MB_KID_FLT_RADAR_TOGGLE,			KEYF_C_DEP,										0},
	{"FLT_JAMMER_TOGGLE",		aic_f_jammer_toggle,			MB_KID_FLT_JAMMER_TOGGLE,			KEYF_C_DEP,										0},
	{"FLT_STATUS",				aic_flight_status,				MB_KID_FLT_STATUS,					KEYF_C_DEP,										0},
	{"FLT_WEAPON_CHK",			aic_flight_weapon_check,		MB_KID_FLT_WEAPON_CHK,				KEYF_C_DEP,										0},
	{"FLT_FUEL_CHK",			aic_flight_fuel_check,			MB_KID_FLT_FUEL_CHK,				KEYF_C_DEP,										0},
	{"FLT_POSITION_CHK",		aic_flight_position_check,		MB_KID_FLT_POSITION_CHK,			KEYF_C_DEP,										0},
	{"FLT_V_FORMATION",			aic_v_formation,				MB_KID_FLT_V_FORMATION,				KEYF_C_DEP,										0},
	{"FLT_WEDGE_FORMATION",		aic_wedge_formation,			MB_KID_FLT_WEDGE_FORMATION,			KEYF_C_DEP,										0},
	{"FLT_LINE_FORMATION",		aic_line_abreast_formation,		MB_KID_FLT_LINE_FORMATION,			KEYF_C_DEP,										0},
	{"FLT_ECHELON_FORMATION",	aic_echelon_formation,			MB_KID_FLT_ECHELON_FORMATION,		KEYF_C_DEP,										0},
	{"FLT_TRAIL_FORMATION",		aic_trail_formation,			MB_KID_FLT_TRAIL_FORMATION,			KEYF_C_DEP,										0},
	{"FLT_WALL_FORMATION",		aic_wall_formation,				MB_KID_FLT_WALL_FORMATION,	 		KEYF_C_DEP,										0},
	{"FLT_LOOSEN_FORMATION",	aic_formation_loosen,			MB_KID_FLT_LOOSEN_FORMATION,		KEYF_C_DEP,										0},
	{"FLT_TIGHTEN_FORMATION",	aic_formation_tighten,			MB_KID_FLT_TIGHTEN_FORMATION,		KEYF_C_DEP,										0},
	{"FLT_ATK_PRIMARY",			aic_f_grnd_attack_prime,		MB_KID_FLT_ATK_PRIMARY,				KEYF_C_DEP,										0},
	{"FLT_ATK_SECONDARY",		aic_f_grnd_attack_second,		MB_KID_FLT_ATK_SECONDARY,			KEYF_C_DEP,										0},
	{"FLT_ATK_ANY_GRND",		aic_f_grnd_attack_opportunity,	MB_KID_FLT_ATK_ANY_GRND,			KEYF_C_DEP,										0},
	{"FLT_ATK_AIR_DEF",			aic_f_grnd_attack_sead,			MB_KID_FLT_ATK_AIR_DEF,			 	KEYF_C_DEP,										0},
	{"FLT_SORT_BANDITS",		aic_f_sort_bandits,				MB_KID_FLT_SORT_BANDITS,			KEYF_C_DEP,										0},
	{"FLT_REPORT_CONTACTS",		aic_f_report_contacts,			0,									KEYF_C_DEP,										0},
	{"FLT_REJOIN_FLIGHT",		aic_f_rejoin_flight,			MB_KID_FLT_REJOIN_FLIGHT,			KEYF_C_DEP,										0},
	{"FLT_RETURN_TO_BASE",		aic_f_rtb,						MB_KID_FLT_RETURN_TO_BASE,			KEYF_C_DEP,										0},
	{"FLT_ALL_DUMB",			aic_f_all_dumb,					MB_KID_FLT_ALL_DUMB,				KEYF_C_DEP,										0},
	{"FLT_HALF_DUMB",			aic_f_half_dumb,				MB_KID_FLT_HALF_DUMB,				KEYF_C_DEP,										0},
	{"FLT_SINGLE_DUMB",			aic_f_single_dumb,				MB_KID_FLT_SINGLE_DUMB,				KEYF_C_DEP,										0},
	{"FLT_ALL_GUIDED",			aic_f_all_guided,				MB_KID_FLT_ALL_GUIDED,				KEYF_C_DEP,										0},
	{"FLT_HALF_GUIDED",			aic_f_half_guided,				MB_KID_FLT_HALF_GUIDED,				KEYF_C_DEP,										0},
	{"FLT_SINGLE_GUIDED",		aic_f_single_guided,			MB_KID_FLT_SINGLE_GUIDED,			KEYF_C_DEP,										0},
	{"FLT_ALL_AGM",				aic_f_all_missile,				MB_KID_FLT_ALL_AGM,					KEYF_C_DEP,										0},
	{"FLT_HALF_AGM",			aic_f_half_missile,				MB_KID_FLT_HALF_AGM,				KEYF_C_DEP,										0},
	{"FLT_SINGLE_AGM",			aic_f_single_missile,			MB_KID_FLT_SINGLE_AGM,				KEYF_C_DEP,										0},
	{"FLT_DEFAULT_WEAPON",		aic_f_default_weapon,			MB_KID_FLT_DEFAULT_WEAPON,			KEYF_C_DEP,										0},
	{"AWACS_PICTURE",			aic_request_picture,			MB_KID_AWACS_PICTURE,				KEYF_C_DEP,										0},
	{"AWACS_BOGEY_DOPE",		aic_request_bogey_dope,			MB_KID_AWACS_BOGEY_DOPE,			KEYF_C_DEP,										0},
	{"AWACS_REQUEST_COVER",		aic_request_cover,				MB_KID_AWACS_REQUEST_COVER,			KEYF_C_DEP,										0},
	{"AWACS_REQUEST_SEAD",		aic_request_sead,				MB_KID_AWACS_REQUEST_SEAD,			KEYF_C_DEP,										0},
	{"AWACS_GO_BULLSEYE",		awacs_go_bullseye,				MB_KID_AWACS_GO_BULLSEYE,			KEYF_C_DEP,										0},
	{"AWACS_GO_DIGIT_BULL",		awacs_go_digital_bullseye,		MB_KID_AWACS_GO_DIGIT_BULL,			KEYF_C_DEP,										0},
	{"AWACS_GO_TACTICAL",		awacs_go_tactical,				MB_KID_AWACS_GO_TACTICAL,			KEYF_C_DEP,										0},
	{"AWACS_ALPHA_BULLSEYE",	aic_alpha_bullseye,				MB_KID_AWACS_ALPHA_BULLSEYE,		KEYF_C_DEP,										0},
	{"AWACS_ALPHA_IP",			aic_alpha_ip,					MB_KID_AWACS_ALPHA_IP,				KEYF_C_DEP,										0},
	{"AWACS_ALPHA_TARGET",		aic_alpha_target,				MB_KID_AWACS_ALPHA_TARGET,			KEYF_C_DEP,										0},
	{"AWACS_ALPHA_PACKAGE",		aic_alpha_package,				MB_KID_AWACS_ALPHA_PACKAGE,			KEYF_C_DEP,										0},
	{"AWACS_ALPHA_HOMEPLATE",	aic_alpha_homeplate,			MB_KID_AWACS_ALPHA_HOMEPLATE,		KEYF_C_DEP,										0},
	{"AWACS_ALPHA_TANKER",		aic_alpha_tanker,				MB_KID_AWACS_ALPHA_TANKER,			KEYF_C_DEP,										0},
	{"AWACS_ALPHA_FIELD",		aic_alpha_divert_field,			MB_KID_AWACS_ALPHA_FIELD,			KEYF_C_DEP,										0},
	{"AWACS_REQUEST_SAR",		aic_request_sar,				MB_KID_AWACS_REQUEST_SAR,			KEYF_C_DEP,										0},
	{"JSTAR_CHECK_IN_OUT",		aic_jstars_check_in_out,		MB_KID_JSTAR_CHECK_IN_OUT,			KEYF_C_DEP,										0},
	{"JSTAR_TGT_LOCATION",		aic_jstars_target_loc,			MB_KID_JSTAR_TARGET_LOC,			KEYF_C_DEP,										0},
	{"JSTAR_NEXT_TARGET",		aic_jstars_next_target,			MB_KID_JSTAR_NEXT_TARGET,			KEYF_C_DEP,										0},
	{"TANKER_FUEL",				aic_tanker_contact,				MB_KID_TANKER_FUEL,					KEYF_C_DEP,										0},
	{"TANKER_FUEL_MAX_TRAP",	aic_tanker_contact_max_trap,	MB_KID_TANKER_FUEL_MAX_TRAP,		KEYF_C_DEP,										0},
	{"TANKER_FUEL_4K",			aic_tanker_contact_4K,			MB_KID_TANKER_FUEL_4K,				KEYF_C_DEP,										0},
	{"CALL_BALL",				call_ball,						MB_KID_CALL_BALL,					KEYF_C_DEP,										0},
	{"CALL_CLARA",				call_clara,						MB_KID_CALL_CLARA,					KEYF_C_DEP,										0},
	{"MULTI_REPORT_SPIKE",		report_spike,					MB_KID_MULTI_REPORT_SPIKE,			KEYF_C_DEP,										0},
	{"MULTI_REPORT_SORT",		report_sort,					MB_KID_MULTI_REPORT_SORT,			KEYF_C_DEP,										0},
	{"MULTI_REPORT_CONTACTS",	report_contacts,				MB_KID_MULTI_REPORT_CONTACTS,		KEYF_C_DEP,										0},
	{"MULTI_REPORT_GRND_TARG",	report_grnd_target,				MB_KID_MULTI_REPORT_GRND_TARG,		KEYF_C_DEP,										0},
	{"MULTI_REPORT_STROBE",		report_strobe,					MB_KID_MULTI_REPORT_STROBE,			KEYF_C_DEP,										0},
	{"MULTI_THREAT_BANDIT",		multi_threat_bandit,			MB_KID_MULTI_THREAT_BANDIT,			KEYF_C_DEP,										0},
	{"MULTI_THREAT_SAM",		multi_threat_sam,				MB_KID_MULTI_THREAT_SAM,			KEYF_C_DEP,										0},
	{"MULTI_THREAT_MISSILE",	multi_threat_missile,			MB_KID_MULTI_THREAT_MISSILE,		KEYF_C_DEP,										0},
	{"MULTI_THREAT_AAA",		multi_threat_aaa,				MB_KID_MULTI_THREAT_AAA,			KEYF_C_DEP,										0},
	{"MULTI_WINCHESTER",		multi_winchester,				MB_KID_MULTI_WINCHESTER,			KEYF_C_DEP,										0},
	{"MULTI_ENGAGED_OFFENSIVE",	multi_engaged_offensive,		MB_KID_MULTI_ENGAGED_OFFENSIVE,		KEYF_C_DEP,										0},
	{"MULTI_ENGAGED_DEFENSIVE",	multi_engaged_defensive,		MB_KID_MULTI_ENGAGED_DEFENSIVE,		KEYF_C_DEP,										0},
	{"MULTI_TARGET_DESTROYED",	multi_target_destroyed,			MB_KID_MULTI_TARGET_DESTROYED,		KEYF_C_DEP,										0},
	{"MULTI_ATTACKING_GROUND",	multi_attacking_ground,			MB_KID_MULTI_ATTACKING_GROUND,		KEYF_C_DEP,										0},
	{"MULTI_TAKEN_DAMAGE",		multi_taken_damage,				MB_KID_MULTI_TAKEN_DAMAGE,			KEYF_C_DEP,										0},
	{"MULTI_BINGO",				multi_bingo,					MB_KID_MULTI_BINGO,					KEYF_C_DEP,										0},
	{"MULTI_RTB",				multi_rtb,						MB_KID_MULTI_RTB,					KEYF_C_DEP,										0},
	{"MSG_HISTORY_TOGGLE",		aic_msg_history_toggle,			MB_KID_MSG_HISTORY_TOGGLE,			KEYF_C_DEP,										0},
	{"MSG_LIKE_HUD_TOGGLE",		aic_msg_like_hud_toggle,		MB_KID_MSG_LIKE_HUD_TOGGLE,			KEYF_C_DEP,										0},
	{"MSG_NEXT_TEXT_COLOR",		aic_next_msg_text_color,		MB_KID_MSG_NEXT_TEXT_COLOR,			KEYF_C_DEP,										0},
	{"MSG_NEXT_BOX_COLOR",		aic_next_msg_box_color,			MB_KID_MSG_NEXT_BOX_COLOR,			KEYF_C_DEP,										0},
	{"MSG_COLOR",				do_msg_color,					MB_KID_MSG_COLOR,				  	KEYF_C_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MSG_SUBTITLE_TOGGLE",		aic_msg_subtitle_toggle,		MB_KID_MSG_SUBTITLE_TOGGLE,			KEYF_C_DEP,										0},
	{"AUTO_REFUEL",				auto_tanking_toggle,			MB_KID_AUTO_REFUEL,					KEYF_M_DEP,										0},
	//{"JUMP",					prepare_for_lightspeed,			MB_KID_JUMP,						KEYF_M_DEP,										0},
	//{"WEAPON_LFT",				wso_bomb_left,					MB_KID_WEAPON_LEFT,					KEYF_W_DEP|KEYF_REPEAT,							0},
	//{"WEAPON_RGT",				wso_bomb_right,					MB_KID_WEAPON_RIGHT,				KEYF_W_DEP|KEYF_REPEAT,							0},
	//{"WEAPON_DWN",				wso_bomb_short,					MB_KID_WEAPON_DOWN,					KEYF_W_DEP|KEYF_REPEAT,							0},
	//{"WEAPON_UP",				wso_bomb_long,					MB_KID_WEAPON_UP,					KEYF_W_DEP|KEYF_REPEAT,							0},
	{"JAMMER",					wso_toggle_jammer,				MB_KID_JAMMER,						KEYF_A_DEP,										0},
	{"RADIO1_TRANSMIT",			radio_wingmen,					MB_KID_RADIO1_TRANSMIT,				KEYF_C_DEP,										0},
	{"RADIO2_TRANSMIT",			radio_others,					MB_KID_RADIO2_TRANSMIT,				KEYF_C_DEP,										0},
	{"RADIO1_AI_TRANSMIT",		radio_AI_wingmen,				MB_KID_RADIO1_TRANSMIT,				KEYF_C_DEP,										0},
	{"RADIO2_AI_TRANSMIT",		radio_AI_others,				MB_KID_RADIO2_TRANSMIT,				KEYF_C_DEP,										0},
	{"RADIO1_CHANNEL",			radio_channel1,					MB_KID_RADIO1_CHANNEL,				KEYF_C_DEP,										0},
	{"RADIO2_CHANNEL",			radio_channel2,					MB_KID_RADIO2_CHANNEL,				KEYF_C_DEP,										0},
	{"COM_MENU_CANCEL",			comm_option_cancel,				MB_KID_COM_MENU_CANCEL,				KEYF_C_DEP,										0},
	{"IFF_INTERROGATE",			do_IffInterrogate,				MB_KID_IFF_INTERROGATE,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"THROTTLE_IDLE",			ThrottleIdle,					MB_KID_THROTTLE_IDLE,				KEYF_F_DEP,										0},
	{"THROTTLE_MIL",			ThrottleMil,					MB_KID_THROTTLE_MIL,				KEYF_F_DEP,										0},
	{"THROTTLE_AB",				ThrottleAB,						MB_KID_THROTTLE_AB,					KEYF_F_DEP,										0},
	{"THROTTLE_UP",				ThrottleUp,						MB_KID_THROTTLE_UP,					KEYF_F_DEP,										0},
	{"THROTTLE_DWN",			ThrottleDown,					MB_KID_THROTTLE_DOWN,				KEYF_F_DEP,										0},
	{"GUN_SELECT",		  	    do_GunSelect,					MB_KID_GUN_SELECT,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TERRAIN_TEXTURING",		texture_detail,					MB_KID_TEXTURE_DETAIL,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_TEXTURE_DETAIL},
	{"OBJECT_DETAIL",			do_RotateObjectComplexity,		MB_KID_OBJECT_COMPLEXITY,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_OBJECT_COMPLEXITY},
	{"OBJECT_NUMBER",			do_RotateSceneComplexity,		MB_KID_SCENE_COMPLEXITY,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_SCENE_COMPLEXITY},
	{"SHADOWS",					do_ToggleShadows,				MB_KID_TOGGLE_SHADOWS,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_TOGGLE_SHADOWS},
	{"ROADS",					do_ToggleRoads,					MB_KID_TOGGLE_ROADS,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_TOGGLE_ROADS},
//	{"SPECTRAL_LIGHTING",		do_ToggleSpectralLighting,		MB_KID_TOGGLE_SPECTRAL,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_TOGGLE_SPECTRAL_LT},
	{"TERRAIN_DISTANCE",		do_RotateViewDistance,			MB_KID_VIEWDISTANCE,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_VIEW_DISTANCE},
	{"TEXTURE_TRANSITION",		do_ToggleTransitions,			MB_KID_TOGGLE_TRANSITIONS,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					KSYS_TOGGLE_TRANSITIONS},
	{"SPARKLING_WATER",				do_ToggleShinyWater,		MB_KID_SPARKLING_WATER,									KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"WAVES",					do_ToggleWaves,					0,									KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LOW_LEVEL_NOISE",			do_ToggleDetailMapping,			MB_KID_LOW_LEVEL_NOISE,									KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LIGHT_MAPS",				do_ToggleLightMaps,				MB_KID_LIGHT_MAPS,									KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"POINT_LIGHTS",			do_TogglePointLights,			MB_KID_POINT_LIGHTS,									KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"REV_MOUSE_YAW",			do_reverse_yaw,					MSG_VW_MOUSEYAW,				    KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"REV_MOUSE_PITCH",			do_reverse_pitch,				MSG_VW_MOUSEPITCH,				    KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"INC_ACM_PAN_SPEED",		do_inc_pan_speed,				MB_KID_INC_ACM_PAN_SPEED,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"DEC_ACM_PAN_SPEED",		do_dec_pan_speed,				MB_KID_DEC_ACM_PAN_SPEED,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"STICKY_POPUP_MDI",		do_toggle_sticky_mpd,			MB_KID_STICKY_POPUP_MPDS,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VR_CPIT_DETAIL",			do_toggle_hivc,					MB_KID_VR_CPIT_DETAIL,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LIMIT_VR_CPIT_VIEW",		do_toggle_vc_view_lock,			MB_KID_LIMIT_VR_CPIT_VIEW,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"CITY_LIGHTS",				do_toggle_city_lights,			MB_KID_CITY_LIGHTS,					KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WEAPONS_ON_PLANES",		do_plane_weapons,				MB_KID_WOP,							KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PILOT_G_EFFECTS",			do_g_effects,					MB_KID_PILOT_G_EFFECTS,									KEYF_G_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CYCLE_WEAPON",			Do_EasyModeChangeWeapon,        MB_KID_CHANGE_WEAPON,				KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"CYCLE_TARGET",			Do_EasyModeChangeTarget,		MB_KID_CHANGE_TARGET,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"FIRST_TARGET",			Do_EasyModeFirstTarget,		    MB_KID_CHANGE_TARGET,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_PILOT_FWD",			cam_front_view,					MB_KID_FRONT_VIEW,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_PILOT_MPD",			cam_front_down,					MB_KID_FRONT_DOWN,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_WSO_FWD",			cam_back_front,					MB_KID_FRONTBACK_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_CKPT_LFT",			cam_left_view,					MB_KID_FRONTLEFT_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_CKPT_RGT",			cam_right_view,					MB_KID_FRONTRIGHT_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_CKPT_LFT_SHLD",		cam_back_left_view,				MB_KID_BACKLEFT_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_CKPT_RGT_SHLD",		cam_back_right_view,			MB_KID_BACKRIGHT_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_PILOT_NO_CKPT",		cam_frontno_view,				MB_KID_FRONTNO_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"VIEW_PILOT_FWD_UP",		cam_frontup_view,				MB_KID_FRONTUP_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_VR_COCKPIT",			virtual_free_look,				MB_KID_FREE_LOOK,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_VR_LOOK_FWD",		vrtl_glance_forward,			MB_KID_PADLOCK_GLANCE_FRONT,		KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"VIEW_POPUP_MDI",			ToggleMpdPopUps,				MB_KID_MPD_INTERACTION,             KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
//	{"COCKPIT_INTERACTION",		ToggleCockpitInteraction,		MB_KID_MPD_INTERACTION,				KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_CHASE",				camera_mode_chase,				MB_KID_CHASE_VIEW,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_FIXED_CHASE",		camera_mode_fixed_chase,		MB_KID_FIXED_CHASE,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_TRACKING_CAM",		camera_mode_rc_view,            MB_KID_TRACKING_CAM,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_UNLIMITED_CAM",		camera_mode_free,				MB_KID_DROP_CAMERA,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_TACTL",				camera_mode_tactical,			MB_KID_TACTICAL_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_REV_TACTL",			camera_mode_reverse,			MB_KID_REVERSE_VIEW,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_FLYBY",				camera_mode_flyby,				MB_KID_FLYBY_VIEW,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_ENEMY_TAC",			camera_mode_enemy_tactical,		MB_KID_VIEW_ENEMY_TACTL,			KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_ENEMY_RV_TAC",		camera_mode_enemy_reverse,		MB_KID_VIEW_ENEMY_REV_TACTL,		KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_LSO",				lso_view,						MB_KID_VIEW_LSO,								KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"VIEW_TOWER",				carrier_tower_view,				MB_KID_VIEW_TOWER,								KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_DESIGNATED",			pdlk_target,					MB_KID_TARGETED_OBJECT,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_PLAYER_WPN",			pdlk_weapon,					MB_KID_CAMERA_WEAPON,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_THREAT_WPN",			pdlk_threat_weapon,				MB_KID_THREAT_WEAPON,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_FRIENDLIES",			pdlk_next_friendly,				MB_KID_NEXT_PLANE,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_ENEMIES",			pdlk_next_enemy,				MB_KID_NEXT_ENEMY,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_GROUND_OBJS",		pdlk_next_ground,				MB_KID_NEAREST_GND_OBJECT,			KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_VEHICLES",	   		pdlk_next_vehicle,				MB_KID_VEHICLES,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_THREAT",	    		pdlk_wso_threat,				MB_KID_PADLOCK_WSO,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_CENTER",				pdlk_center_object,				MB_KID_PADLOCK_CENTER,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_WINGMAN",			pdlk_wingman,					MB_KID_WINGMAN_AIRCRAFT,			KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_TANKERS",			pdlk_tanker,					MB_KID_TANKER,						KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PDLK_PACKAGE",			pdlk_package,					MB_KID_PDLK_PACKAGE,									KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"ACM_FWD",					acm_front,						MB_KID_ACM_FRONT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_UP",					acm_toggle_up,					MB_KID_ACM_TOGGLE_UP,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_BACK_LFT",			acm_back_left,					MB_KID_ACM_BACK_LEFT,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_LFT",					acm_left,						MB_KID_ACM_LEFT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_RGT",					acm_right,						MB_KID_ACM_RIGHT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_BACK_RGT",			acm_back_right,					MB_KID_ACM_BACK_RIGHT,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_FWD_LFT",				acm_front_left,					MB_KID_ACM_FRONT_LEFT,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_FWD_RGT",				acm_front_right,				MB_KID_ACM_FRONT_RIGHT,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_BACK_MODIFIER",		acm_back,						MB_KID_ACM_BACK,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"ACM_DWN_MODIFIER",		acm_toggle_down,				MB_KID_ACM_TOGGLE_DWN,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
//	{"ZOOM_MPD_LEFT",			zoom_mpd_left,					0,									KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
//	{"ZOOM_MPD_CENTER",			zoom_mpd_center,				0,									KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
//	{"ZOOM_MPD_RIGHT",			zoom_mpd_right,					0,									KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
//	{"ZOOM_MPD_DOWN",			zoom_mpd_down,					0,									KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_OUT",					camera_out,						MB_KID_CAMERA_QUICK_OUT,			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_IN",					camera_in,						MB_KID_CAMERA_QUICK_IN,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_OUT_SLOW",			camera_slow_out,				MB_KID_CAMERA_OUT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_IN_SLOW",				camera_slow_in,					MB_KID_CAMERA_IN,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_SLIDE_LFT",			cam_slide_left,					MB_KID_CAM_LEFT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_SLIDE_LFT_SLOW",		cam_slide_left_slow,			MB_KID_CAM_LEFT_SLOW,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_SLIDE_RGT",			cam_slide_right,				MB_KID_CAM_RIGHT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_SLIDE_RGT_SLOW",		cam_slide_right_slow,			MB_KID_CAM_RIGHT_SLOW,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_EAST",				cam_slew_east,					MB_KID_SLEW_CAM_EAST,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_EAST_SLOW",			cam_slew_east_slow,				MB_KID_SLEW_CAM_EAST_SLOW,			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_WEST",				cam_slew_west,					MB_KID_SLEW_CAM_WEST,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_WEST_SLOW",			cam_slew_west_slow,				MB_KID_SLEW_CAM_WEST_SLOW, 			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_NORTH",				cam_slew_north,					MB_KID_SLEW_CAM_NORTH,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_NORTH_SLOW",			cam_slew_north_slow,			MB_KID_SLEW_CAM_NORTH_SLOW,			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_SOUTH",				cam_slew_south,					MB_KID_SLEW_CAM_SOUTH,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_SOUTH_SLOW",			cam_slew_south_slow,			MB_KID_SLEW_CAM_SOUTH_SLOW,			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_ASCEND",		   		cam_slew_up,					MB_KID_CAM_ASCEND,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_ASCEND_SLOW",   		cam_slew_up_slow,				MB_KID_CAM_ASCEND_SLOW,	 			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_DESCEND",		   		cam_slew_down,					MB_KID_CAM_DESCEND,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_DESCEND_SLOW",   		cam_slew_down_slow,				MB_KID_CAM_DESCEND_SLOW, 			KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_ROTATE_LFT",			camera_left,					MB_KID_ROT_LEFT,					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_ROTATE_RGT",	   		camera_right,					MB_KID_ROT_RIGHT, 					KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_ROTATE_UP",			cam_pitch_up,					MB_KID_CAM_PITCH_UP,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"CAM_ROTATE_DWN",			cam_pitch_down,					MB_KID_CAM_PITCH_DOWN,				KEYF_V_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"WATCH_PLAYER",			watch_me,						MB_KID_WATCH_PLAYER,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_DESIGNATED",		watch_target,					MB_KID_TARGETED_OBJECT,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_PLAYER_WPN",		watch_weapon,					MB_KID_CAMERA_WEAPON,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_THREAT_WPN",		watch_threat_weapon,	 		MB_KID_THREAT_WEAPON,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_FRIENDLIES",		watch_next_friendly,			MB_KID_NEXT_PLANE,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_ENEMIES",			watch_next_enemy,				MB_KID_NEXT_ENEMY,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_GROUND_OBJS",		watch_next_ground,				MB_KID_NEAREST_GND_OBJECT,			KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_VEHICLES",	   		watch_next_vehicle,				MB_KID_WATCH_VEHICLES,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_THREAT",			watch_wso_threat,				MB_KID_PADLOCK_WSO,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_CENTER",	   		watch_center_object,			MB_KID_WATCH_CENTER,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_WINGMAN",			watch_wingman,					MB_KID_WINGMAN_AIRCRAFT,			KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"WATCH_TANKERS",			watch_tanker,					MB_KID_TANKER,						KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"EJECT",					player_eject,					MB_KID_EJECT,						KEYF_F_DEP,										0},
	{"REARM",					do_RearmPlayer,					MB_KID_REARM,						KEYF_M_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"BREAK_LOCK",				Do_BreakAALock,					MB_KID_UNDESIGNATE_TARGET,			KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RADAR_ANT_UP",			do_AARadarUp,					MB_KID_RADAR_ANT_UP,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RADAR_ANT_DWN",			do_AARadarDown,					MB_KID_RADAR_ANT_DOWN,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RADAR_ANT_CENTER",		do_AARadarCenter,				MB_KID_RADAR_ANT_CENTER,			KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CENTER_JOYSTICK",			do_CenterJoy,					MB_KID_CENTER_JOYSTICK,				KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"MASTER_LFT_ENGINE",		ToggleLeftEngStatus,	 		MB_KID_MASTER_LEFT_ENGINE,			KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"MASTER_RGT_ENGINE",		ToggleRightEngStatus,			MB_KID_MASTER_RIGHT_ENGINE,			KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"NOSE_TRIM_DWN",			TrimNoseDown,					MB_KID_NOSE_TRIM_DOWN,				KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"NOSE_TRIM_UP",			TrimNoseUp,						MB_KID_NOSE_TRIM_UP,				KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"RUDDER_LFT",				RudderLeft,						MB_KID_RUDDER_LEFT,					KEYF_F_DEP|KEYF_REPEAT,							0},
	{"RUDDER_RGT",				RudderRight,					MB_KID_RUDDER_RIGHT,				KEYF_F_DEP|KEYF_REPEAT,							0},
	{"AIL_LFT_TRIM",  			TrimAilLeft,					MB_KID_TRIM_LEFT,					KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"AIL_RGT_TRIM",  			TrimAilRight,					MB_KID_TRIM_RIGHT,					KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"RUDDER_RGT_TRIM",			TrimRdrRight,					MB_KID_RUDDER_RIGHT_TRIM,			KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"RUDDER_LFT_TRIM",			TrimRdrLeft,					MB_KID_RUDDER_LEFT_TRIM,			KEYF_F_DEP|KEYF_NOTEMPLEMENT,					0},
	{"ILS_TOGGLE",				ILSToggle,						MB_KID_ILS_TOGGLE,					KEYF_A_DEP|KEYF_NOTEMPLEMENT,					0},
	{"LIGHTS_ANTICOLLISION",	running_lights,					MB_KID_RUNNING_LIGHTS,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LIGHTS_FORMATION",		formation_lights,				MB_KID_FORMATION_LIGHTS,			KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"JETTISON_ALL",			Do_JettisonAll,					MB_KID_JETTISON_ALL,				KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"JETTISON_SELECTED",		Do_JettisonSelected,			MB_KID_JETTISON_AG,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MASTER_ARM",				Do_MasterArm,					MB_KID_MASTER_ARM,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"FIRE_EXT_AMAD",			Do_FireExtAmad,					MB_KID_FIRE_EXT_AMAD,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"FIRE_EXT_LENG",			Do_FireExtLeng,					MB_KID_FIRE_EXT_LENG,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"FIRE_EXT_RENG",			Do_FireExtReng,					MB_KID_FIRE_EXT_RENG,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"GUN_RATE",				Do_GunRate,					    MB_KID_GUN_RATE,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RADAR_MODE",				Do_RadarMode,					MB_KID_RADAR_MODE,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RADAR_AZ_SCAN",			Do_RadarAzScan,					MB_KID_RADAR_AZ_SCAN,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RANGE_INCREASE",			Do_RadarRangeInc,				MB_KID_RADAR_RNG_INC,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RANGE_DECREASE",			Do_RadarRangeDec,				MB_KID_RADAR_RNG_DEC,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_FOV",				Do_TFlirFov,					MB_KID_TFLIR_FOV,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"TFLIR_POLARITY",			Do_TFlirPolarity,				MB_KID_TFLIR_POLARITY,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"TFLIR_MODE",				Do_TFlirMode,					MB_KID_TFLIR_MODE,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"TFLIR_TRACK",				Do_TFlirTrack,					MB_KID_TFLIR_TRACK,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"TFLIR_CDES",				Do_TFlirDes,					MB_KID_TFLIR_CDES,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_BST",				Do_TFlirCtr,					MB_KID_TFLIR_CTR,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_CON_UP",			kFlirContrastUp,    			MB_KID_TFLIR_CON_UP,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_CON_DWN",			kFlirContrastDown,  			MB_KID_TFLIR_CON_DWN,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_BRT_UP",			kFlirBrightnessUp,  			MB_KID_TFLIR_BRT_UP,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_BRT_DWN",			kFlirBrightnessDown,			MB_KID_TFLIR_BRT_DWN,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TFLIR_DECLUTTER",			kFlirDeclutter,					MB_KID_TFLIR_DECLUTTER,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LASER_TOGGLE",			Do_LaserToggle,					MB_KID_LASER_TOGGLE,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MASTER_CAUTION",			Do_MasterCaution,				MB_KID_MASTER_CAUTION,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"EMCON",					Do_EmisLimit,					MB_KID_MAIN_EMIS_LIMIT,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"HSI_INC_RANGE",			Do_HsiIncRange,				    MB_KID_TEWS_RANGE_SELECT,			KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"HSI_DEC_RANGE",			Do_HsiDecRange,				    MB_KID_TEWS_RANGE_SELECT,			KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RADAR_SILENT",			Do_ToggleSilent,				MB_KID_RADAR_SNIFF,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CMDS_MODE",				Do_TewsCommandMode,			    MB_KID_TEWS_CMDS_MODE,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TARGET_DESIGNATE",		Do_AvDesignate,					MB_KID_TARGET_DESIGNATE,			KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TARGET_UNDESIGNATE",		Do_AvUnDesignate,				MB_KID_TARGET_UNDESIGNATE,          KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"STICK_LEFT",				Do_StickLeft,					MB_KID_STICK_LEFT,			        KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"STICK_RIGHT",				Do_StickRight,					MB_KID_STICK_RIGHT,					KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"STICK_UP",				Do_StickUp,						MB_KID_STICK_UP,					KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"STICK_DOWN",				Do_StickDown,					MB_KID_STICK_DOWN,					KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_JOYSTICK",			Do_CaptBar,						MB_KID_TDC_JOYSTICK,				KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_UP",					Do_TDCUp,						MB_KID_TDC_UP,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_DOWN",				Do_TDCDown,						MB_KID_TDC_DOWN,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_LEFT",				Do_TDCLeft,						MB_KID_TDC_LEFT,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_RIGHT",				Do_TDCRight,					MB_KID_TDC_RIGHT,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"ACQ_AUTO",				Do_AAAutoAcqTarget,				MB_KID_ACQ_AUTO,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CAGE_UNCAGE",				Do_CageUncage,					MB_KID_CAGE_UNCAGE,					KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_CYCLE",				Do_StepTDC,						MB_KID_TDC_CYCLE,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
//	{"IDECM_MODE",				Do_IDECMMode,    				MB_KID_IDECM_MODE",					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"DECOY_DEPLOY",			Do_CMDDSControl,  				MB_KID_DECOY_DEPLOY,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"DECOY_XMIT",				Do_CMDDSTransmit,   			MB_KID_DECOY_XMIT,					KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CMDS_PROGRAM_INITIATE",	Do_ChaffFlareProgInitiate,		MB_KID_CMDS_PROGRAM_INITIATE,		KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"CMDS_PROGRAM_SELECT",		Do_ChaffFlareProgSelect,   		MB_KID_CMDS_PROGRAM_SELECT,			KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"AGARM_PROGRAM",			Do_AgArmProgram,   				MB_KID_AGARM_PROGRAM,				KEYF_W_DEP|KEYF_ALLOW_PAUSE,					0},
	{"STEERING_CYCLE",			Do_SteeringCycle,				MB_KID_STEERING_CYCLE,				KEYF_A_DEP|KEYF_ALLOW_PAUSE,					0},
	{"DUMP_FUEL",				Do_DumpFuel,   					MB_KID_DUMP_FUEL,					KEYF_F_DEP|KEYF_ALLOW_PAUSE,					0},
	{"ZOOM_LEFT_MDI",			zoom_mpd_left,   				MB_KID_ZOOM_LEFT_MDI,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"ZOOM_RIGHT_MDI",			zoom_mpd_right,   				MB_KID_ZOOM_RIGHT_MDI,				KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"ZOOM_UFC",				zoom_mpd_center,   				MB_KID_ZOOM_UFC,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"ZOOM_MPCD",				zoom_mpd_down,   				MB_KID_ZOOM_MPCD,					KEYF_V_DEP|KEYF_ALLOW_PAUSE,					0},
	{"NEXT_MISSION_TGT",        Do_NextAGTarget,   				MB_KID_NEXT_MISSION_TGT,			KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"PREV_MISSION_TGT",        Do_PrevAGTarget,   				MB_KID_PREV_MISSION_TGT,			KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"FIRST_MISSION_TGT",       Do_FirstAGTarget,   			MB_KID_FIRST_MISSION_TGT,			KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"DESTRUCTO_CAM",	  		Do_ToggleDestructoCam,			MB_KID_DESTRUCTO_CAM,				KEYF_M_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"CANOPY_REFLECTIONS",	    Do_Canopy_Reflections,			MB_KID_CANOPY_REFLECTIONS,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"VC_LIFT_LINE",			ToggleLiftLine,					MB_KID_VC_LIFT_LINE,				KEYF_G_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"VOLUMETRIC_EXPLOSIONS",   Do_Volumetric_Explosions,		MB_KID_VOLUMETRIC_EXPLOSIONS,		KEYF_G_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"VOLUMETRIC_CLOUDS",		Do_Volumetric_Clouds,			MB_KID_VOLUMETRIC_CLOUDS,			KEYF_G_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"HARM_OVERRIDE",			Do_HarmOverride,				MB_KID_HARM_OVERRIDE,				KEYF_W_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"LABELS_AIRCRAFT",	        Do_ToggleAircraftLabels,		MB_KID_LABELS_AIRCRAFT,				KEYF_M_DEP|KEYF_ALLOW_PAUSE,	      	0},
	{"LABELS_GROUND_TGT",		Do_ToggleGroundLabels,          MB_KID_LABELS_GROUND_TGT,			KEYF_M_DEP|KEYF_ALLOW_PAUSE,	      	0},
	//
	// Debug functions
	//
	{"SUN_ANGLE_UP",			sun_angle_up,					MB_KID_SUN_ANGLE_UP,				KEYF_G_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},
	{"SUN_ANGLE_DWN",			sun_angle_down,					MB_KID_SUN_ANGLE_DOWN,				KEYF_G_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE,		0},

//#ifdef _DEBUG // BEGIN --- DEBUG FUNCTIONS ------------------------------------------------------------------

	{"DEBUG_HIT",				do_debug_hit,					0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},
	{"BR_IN",					debug_radar_in,					0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},
	{"BR_OUT",					debug_radar_out,				0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},
	{"COMMAND_RADAR",			toggle_radar,					0x0,								KEYF_DEBUGONLY,									0},
	{"QUEUE_BETTY",				queue_betty,					0x0,								KEYF_DEBUGONLY,									0},
	{"TOGGLE_3DFXDEBUG",		toggle_3DfxDebug,				0x0,								KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,	0},
	{"DISPLAY_FPS",				display_fps,					0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},
	{"DAMAGE_PLANE",			do_damage_plane,				0x0,								KEYF_DEBUGONLY,									0},
	{"REFUEL_PLANE",			refuel_Player_plane,			0x0,								KEYF_DEBUGONLY|KEYF_ALLOW_PAUSE,				0},
	{"DEBUG_EYE_POINT",			next_camera_eye,				0x0,								KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_NEXT_PLANE",		do_next_plane,					0x0,								KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_CLOSEST_PLANE",		do_closest_plane,				0x0,								KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_OLD_WSO_THREAT",	pad_WSO_warn,					0x0,								KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_OLD_NEXTCENTER",	pad_next_center_obj,			0x0,								KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_MEMORY",			DebugMemory,					0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},
	{"DEBUG_COUNT_OBJECTS",		DebugCountObjects,				0x0,								KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_RCS",			    Do_ShowCrossSigInfo,            0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_DIR_ARROWS",		Do_ToggleDirArrows,				0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_LANDING_GATES",		Do_ToggleLandingGates,			0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_DESTRUCTO_CAM",		ToggleTewsTest,					0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_NEXT_WATCH_PLANE",	do_next_watch_plane,            0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_N_CARR_PLANE",		do_next_carrier_deck_plane,		0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_P_CARR_PLANE",		do_prev_carrier_deck_plane,		0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_RIGHT",	do_carrier_deck_plane_right,    0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_R_F",	do_carrier_deck_plane_right_fast,0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_LEFT",	do_carrier_deck_plane_left,     0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_L_F",	do_carrier_deck_plane_left_fast,0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_FORWARD",do_carrier_deck_plane_forward,  0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_F_F",	do_carrier_deck_plane_forward_fast,0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_BACK",	do_carrier_deck_plane_back,     0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_B_F",	do_carrier_deck_plane_back_fast,0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_CW",		do_carrier_deck_plane_clockwise,0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_CW_F",	do_carrier_deck_plane_clockwise_fast,     0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_CCW",	do_carrier_deck_plane_counterclockwise,     0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_CARR_PLANE_CCW_F",	do_carrier_deck_plane_counterclockwise_fast,     0x0,					            KEYF_W_DEP|KEYF_REPEAT|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
    {"DEBUG_PRINT_CARR_PLANE_POS",	do_carrier_deck_plane_print_pos,     0x0,					            KEYF_W_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_VAR_UP",			DebugVarUp,						0x0,					            KEYF_W_DEP|KEYF_REPEAT| KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"DEBUG_VAR_DOWN",			DebugVarDown,					0x0,					            KEYF_W_DEP|KEYF_REPEAT| KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,		0},
	{"NEXT_VIEW_LOCATION",NextViewLocation,						0x0,							KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,					0},
	{"INIT_VIEW_LOCATION",InitViewLocations,					0x0,							KEYF_V_DEP|KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,					0},

//#endif // END --- DEBUG FUNCTIONS -------------------------------------------------------------------------

	/* ------------------------------------------3/7/98 1:56PM---------------------------------------------
	 * easter egg keys
	 * ----------------------------------------------------------------------------------------------------*/

	{"OINK_OINK",				F182pig,						0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},
	{"MEANGRN",					corvette,						0x0,								KEYF_ALLOW_PAUSE|KEYF_DEBUGONLY,				0},

	//
	// Not implemented yet functions - CHECK for your commands here, and insert ABOVE THIS LINE
	//
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//	{"TOGGLE_SOUND",			NULL_FUNCTION,					MB_KID_TOGGLE_SOUND,				KEYF_M_DEP|KEYF_NOTEMPLEMENT,					KSYS_TOGGLE_SOUND},
	{"TOGGLE_SPEECH",			speech_mute_toggle,				0x0,								KEYF_M_DEP|KEYF_NOTEMPLEMENT,					KSYS_TOGGLE_SOUND},
//	{"MPD1_SCROLL",				NULL_FUNCTION,					MB_KID_MPD1_SCROLL,					KEYF_A_DEP|KEYF_NOTEMPLEMENT,					0},
//	{"MPD2_SCROLL",				NULL_FUNCTION,					MB_KID_MPD2_SCROLL,					KEYF_A_DEP|KEYF_NOTEMPLEMENT,					0},
//	{"MPD3_SCROLL",				NULL_FUNCTION,					MB_KID_MPD3_SCROLL,					KEYF_A_DEP|KEYF_NOTEMPLEMENT,					0},
	{"AWACS_REQUEST_TANKER",	do_awacs_tanker,				MB_KID_AWACS_REQUEST_TANKER,		KEYF_C_DEP|KEYF_NOTEMPLEMENT,					0},

	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//
	// Not implemented yet functions - CHECK for your commands here, and insert BEFORE NULL_FUNCTION area
	//

	{"WING_RADIO_TOGGLE",		wingman_radio_toggle,			MB_KID_WING_RADIO_TOGGLE,			KEYF_C_DEP,										0},
	{"ATC_RADIO_TOGGLE",		atc_radio_toggle,				MB_KID_ATC_RADIO_TOGGLE,			KEYF_C_DEP,										0},
	{"OTHER_RADIO_TOGGLE",		other_radio_toggle,				MB_KID_OTHER_RADIO_TOGGLE,			KEYF_C_DEP,										0},

	{"TDC_LMDI",				tdc_lmdi,						MB_KID_TDC_LMDI,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_RMDI",				tdc_rmdi,						MB_KID_TDC_RMDI,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_MPCD",				tdc_mpcd,						MB_KID_TDC_MPCD,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_UFC",					tdc_ufc,						MB_KID_TDC_UFC,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},

	{"LMDI_PB1",				lmdi_pb1,						MB_KID_LMDI_PB1,					KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB2",				lmdi_pb2,						MB_KID_LMDI_PB2,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB3",				lmdi_pb3,						MB_KID_LMDI_PB3,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB4",				lmdi_pb4,						MB_KID_LMDI_PB4,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB5",				lmdi_pb5,						MB_KID_LMDI_PB5,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB6",				lmdi_pb6,						MB_KID_LMDI_PB6,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB7",				lmdi_pb7,						MB_KID_LMDI_PB7,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB8",				lmdi_pb8,						MB_KID_LMDI_PB8,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB9",				lmdi_pb9,						MB_KID_LMDI_PB9,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB10",				lmdi_pb10,						MB_KID_LMDI_PB10,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB11",				lmdi_pb11,						MB_KID_LMDI_PB11,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB12",				lmdi_pb12,						MB_KID_LMDI_PB12,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB13",				lmdi_pb13,						MB_KID_LMDI_PB13,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB14",				lmdi_pb14,						MB_KID_LMDI_PB14,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB15",				lmdi_pb15,						MB_KID_LMDI_PB15,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB16",				lmdi_pb16,						MB_KID_LMDI_PB16,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB17",				lmdi_pb17,						MB_KID_LMDI_PB17,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB18",				lmdi_pb18,						MB_KID_LMDI_PB18,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB19",				lmdi_pb19,						MB_KID_LMDI_PB19,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"LMDI_PB20",				lmdi_pb20,						MB_KID_LMDI_PB20,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},

	{"RMDI_PB1",				rmdi_pb1,						MB_KID_RMDI_PB1,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB2",				rmdi_pb2,						MB_KID_RMDI_PB2,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB3",				rmdi_pb3,						MB_KID_RMDI_PB3,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB4",				rmdi_pb4,						MB_KID_RMDI_PB4,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB5",				rmdi_pb5,						MB_KID_RMDI_PB5,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB6",				rmdi_pb6,						MB_KID_RMDI_PB6,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB7",				rmdi_pb7,						MB_KID_RMDI_PB7,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB8",				rmdi_pb8,						MB_KID_RMDI_PB8,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB9",				rmdi_pb9,						MB_KID_RMDI_PB9,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB10",				rmdi_pb10,						MB_KID_RMDI_PB10,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB11",				rmdi_pb11,						MB_KID_RMDI_PB11,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB12",				rmdi_pb12,						MB_KID_RMDI_PB12,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB13",				rmdi_pb13,						MB_KID_RMDI_PB13,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB14",				rmdi_pb14,						MB_KID_RMDI_PB14,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB15",				rmdi_pb15,						MB_KID_RMDI_PB15,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB16",				rmdi_pb16,						MB_KID_RMDI_PB16,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB17",				rmdi_pb17,						MB_KID_RMDI_PB17,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB18",				rmdi_pb18,						MB_KID_RMDI_PB18,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB19",				rmdi_pb19,						MB_KID_RMDI_PB19,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"RMDI_PB20",				rmdi_pb20,						MB_KID_RMDI_PB20,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},

	{"MPCD_PB1",				mpcd_pb1,						MB_KID_MPCD_PB1,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB2",				mpcd_pb2,						MB_KID_MPCD_PB2,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB3",				mpcd_pb3,						MB_KID_MPCD_PB3,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB4",				mpcd_pb4,						MB_KID_MPCD_PB4,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB5",				mpcd_pb5,						MB_KID_MPCD_PB5,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB6",				mpcd_pb6,						MB_KID_MPCD_PB6,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB7",				mpcd_pb7,						MB_KID_MPCD_PB7,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB8",				mpcd_pb8,						MB_KID_MPCD_PB8,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB9",				mpcd_pb9,						MB_KID_MPCD_PB9,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB10",				mpcd_pb10,						MB_KID_MPCD_PB10,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB11",				mpcd_pb11,						MB_KID_MPCD_PB11,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB12",				mpcd_pb12,						MB_KID_MPCD_PB12,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB13",				mpcd_pb13,						MB_KID_MPCD_PB13,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB14",				mpcd_pb14,						MB_KID_MPCD_PB14,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB15",				mpcd_pb15,						MB_KID_MPCD_PB15,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB16",				mpcd_pb16,						MB_KID_MPCD_PB16,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB17",				mpcd_pb17,						MB_KID_MPCD_PB17,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB18",				mpcd_pb18,						MB_KID_MPCD_PB18,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB19",				mpcd_pb19,						MB_KID_MPCD_PB19,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"MPCD_PB20",				mpcd_pb20,						MB_KID_MPCD_PB20,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},

	{"UFC_PB1",					ufc_pb1,						MB_KID_UFC_PB1,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB2",					ufc_pb2,						MB_KID_UFC_PB2,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB3",					ufc_pb3,						MB_KID_UFC_PB3,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB4",					ufc_pb4,						MB_KID_UFC_PB4,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB5",					ufc_pb5,						MB_KID_UFC_PB5,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB6",					ufc_pb6,						MB_KID_UFC_PB6,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB7",					ufc_pb7,						MB_KID_UFC_PB7,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB8",					ufc_pb8,						MB_KID_UFC_PB8,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB9",					ufc_pb9,						MB_KID_UFC_PB9,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB10",				ufc_pb10,						MB_KID_UFC_PB10,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB11",				ufc_pb11,						MB_KID_UFC_PB11,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB12",				ufc_pb12,						MB_KID_UFC_PB12,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB13",				ufc_pb13,						MB_KID_UFC_PB13,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB14",				ufc_pb14,						MB_KID_UFC_PB14,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB15",				ufc_pb15,						MB_KID_UFC_PB15,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB16",				ufc_pb16,						MB_KID_UFC_PB16,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB17",				ufc_pb17,						MB_KID_UFC_PB17,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB18",				ufc_pb18,						MB_KID_UFC_PB18,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB19",				ufc_pb19,						MB_KID_UFC_PB19,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"UFC_PB20",				ufc_pb20,						MB_KID_UFC_PB20,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},

	{"TDC_PB1",					tdc_pb1,						MB_KID_TDC_PB1,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB2",					tdc_pb2,						MB_KID_TDC_PB2,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB3",					tdc_pb3,						MB_KID_TDC_PB3,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB4",					tdc_pb4,						MB_KID_TDC_PB4,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB5",					tdc_pb5,						MB_KID_TDC_PB5,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB6",					tdc_pb6,						MB_KID_TDC_PB6,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB7",					tdc_pb7,						MB_KID_TDC_PB7,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB8",					tdc_pb8,						MB_KID_TDC_PB8,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB9",					tdc_pb9,						MB_KID_TDC_PB9,							KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB10",				tdc_pb10,						MB_KID_TDC_PB10,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB11",				tdc_pb11,						MB_KID_TDC_PB11,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB12",				tdc_pb12,						MB_KID_TDC_PB12,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB13",				tdc_pb13,						MB_KID_TDC_PB13,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB14",				tdc_pb14,						MB_KID_TDC_PB14,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB15",				tdc_pb15,						MB_KID_TDC_PB15,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB16",				tdc_pb16,						MB_KID_TDC_PB16,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB17",				tdc_pb17,						MB_KID_TDC_PB17,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB18",				tdc_pb18,						MB_KID_TDC_PB18,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB19",				tdc_pb19,						MB_KID_TDC_PB19,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},
	{"TDC_PB20",				tdc_pb20,						MB_KID_TDC_PB20,						KEYF_S_DEP|KEYF_ALLOW_PAUSE,					0},

	{NULL,NULL,0,0,0}
};


BOOL CheckForEjected();