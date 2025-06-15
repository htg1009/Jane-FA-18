//*****************************************************************************************
//  MULTIAI.CPP
//*****************************************************************************************
#define MULTI_AI_SOURCE 1

#include "F18.h"
#include "dplay.h"		// delete this when DX5 to all
#include "WrapInterface.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"
#include "MultiDefs.h"
#include "flightfx.h"
//
//	SkunkNet currently has a "C" linkage because I can find no way to get
//
//			--- F18API __declspec(naked) UINT P5time() ---
//
//	to compile in a CPP file.  ERROR: not all control paths return a value.
//	In a C file we get:  "warning C4035: 'P5time' : no return value"
//
#define    F18LIB
#include "SkunkNet.h"

extern int LoadWeaponModel(WeaponType *type);
extern DBRadarType *GetRadarPtr(long id);
extern DBWeaponType *get_weapon_ptr(long id);

extern int NetWeapIX;	// weapon GUID (unique system-wide) - HIWORD(slot#) | LOWORD(seq#)
extern int	newTime;	// current physics tick, ie. T1
extern int	MySlot;		// my player's slot index [0..N]
extern NetSlot Slot[MAXAIOBJS];  //  Was PLAYERS
extern BOOL	g_bIAmHost;
extern CameraInstance	 OldCamera;
extern int OldCockpitSeat, OldCurrentCockpit;

extern int NetSend( int dpID, int flags, void *lpMsg, int size );
extern void RPH_to_Orientation( PlaneParams *P );
extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
extern void LogDestroyedObject(WeaponParams *W,int iType,int iID);
void DestroyVehicle(MovingVehicleParams *vehiclepnt,BOOL play_sound);
extern void DestroyShip(MovingVehicleParams *vehiclepnt,BOOL play_sound);

extern void LogPlaneShotdown(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type,int iCrashed);
extern void LogPlaneDamage(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type);
extern void LogWeaponHit(WeaponParams *W,int iType,int iID);
extern void LogDestroyedObject(WeaponParams *W,int iType,int iID);

int clearpackets = 1;
int netsendanyway = 5;
int iWatchMAISends = 1;

extern void ShutdownClouds( void);
extern void LoadCloudTexture( void);
extern BOOL SnowOn, doLightning, doRain;

extern void DisplayInitText(char *sTxt, int SkipLine, int showrelease=0);

extern int iNetBulletHit;

//============================================================================
//		AI Position MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutAIPos()
 *
 *		Tell the world where an AI is.
 *
 */
void NetPutAIPos(PlaneParams *P)
{
	double distance;
	PlaneParams *planepnt;
	int loopoffset;
	int nobehave = 0;

	if(!(P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		return;
	}

	if(P->AI.Behaviorfunc == CTWaitingForLaunch)
	{
		return;
	}

#if 1
	if(iMultiCast >= 0)
	{
		NetPutAIPosSmall(P);
		return;
	}
#endif

	loopoffset = P - Planes;
	NetAIPos.type = NET_AI_POS;
	NetAIPos.time = newTime;
	NetAIPos.slot = PlayerPlane - Planes;

	NetAIPos.aislot = P - Planes;		// ai plane slot #
	NetAIPos.fpdWorldPosition = P->WorldPosition;  //  ai's position in the world
	NetAIPos.Heading = P->Heading;	//  ai's new heading
	NetAIPos.Pitch = P->Pitch;		//  ai's new pitch
	NetAIPos.Roll = P->Roll;		//  ai's new roll
	NetAIPos.V = P->V;			//  ai's velocity in ft/sec
	NetAIPos.DesiredPitch = P->DesiredPitch;		//  ai's desired pitch
	NetAIPos.DesiredRoll = P->DesiredRoll;		//  ai's desired roll
	NetAIPos.DesiredSpeed = P->DesiredSpeed;		//  ai's desired speed in ft/sec

	NetAIPos.lFlags1 = 0;
	if(P->SpeedBrakeState)
	{
		NetAIPos.lFlags1 |= NET_SBRAKE;
	}
	else
	{
		NetAIPos.lFlags1 &= ~NET_SBRAKE;
	}

	if(P->FlightStatus & PL_GEAR_DOWN_LOCKED)
	{
		NetAIPos.lFlags1 |= NET_GEAR;
	}
	else
	{
		NetAIPos.lFlags1 &= ~NET_GEAR;
	}

	if(P->Flaps == 0)
	{
		NetAIPos.lFlags1 &= ~NET_FLAPS;
	}
	else
	{
		NetAIPos.lFlags1 |= NET_FLAPS;
	}

	if(P->AI.iAICombatFlags1 & AI_MULTI_FIRE_GUN)
	{
		NetAIPos.lFlags1 |= NET_AI_GUN;
		P->AI.iAICombatFlags1 &= ~AI_MULTI_FIRE_GUN;
	}
	else
	{
		NetAIPos.lFlags1 &= ~NET_AI_GUN;
	}

	long ltempflags = P->AI.cMultiCheckFlags;
	NetAIPos.lFlags1 |= (ltempflags << MC_SHIFT);

	if(P->AI.AirTarget)
	{
		NetAIPos.iAirTarget = P->AI.AirTarget - Planes;
	}
	else
	{
		NetAIPos.iAirTarget = -1;
	}
	if(P->AI.AirThreat)
	{
		NetAIPos.iAirThreat = P->AI.AirThreat - Planes;
	}
	else
	{
		NetAIPos.iAirThreat = -1;
	}


	NetAIPos.iwaypoint = P->AI.CurrWay - AIWayPoints;

	if(iMAISendTo >= 0)
	{
		NetAddToBigPacket(iMAISendTo, &NetAIPos, sizeof(NetAIPos) );
		return;
	}

//   	NetSend( DPID_ALLPLAYERS, 0, &NetAIPos, sizeof(NetAIPos) );
//	(!(GameLoop % 10))

	int X, Y;

	MAIGetHotZoneIndex((float)P->WorldPosition.X, (float)P->WorldPosition.Z, &X, &Y);
	int sendto = (MAICheckHotZones(X, Y));

	for (int i=0; i<PLAYERS; i++)
	{
		int planex = NetGetPlaneIndexFromSlot(i);
		if(planex == -1) continue;
		if (i==MySlot || Slot[planex].dpid==0)  continue;

		if(iSlotToPlane[i] != -1)
		{
			if(!(Planes[iSlotToPlane[i]].Status & (PL_ACTIVE)))
			{
				continue;
			}
		}
		else
		{
			continue;
		}

		planepnt = &Planes[i];

		//  Since nobehave doesn't get set by anything I think this was during early prototype stage
		//  This will cancel out a number of these if statements.
		if((P->AI.iMultiPlayerDist[i] < 20) && (nobehave))  //  5 updates/second
		{
			distance = P->WorldPosition - planepnt->WorldPosition;
			distance *= WUTONM;
			P->AI.iMultiPlayerDist[i] = distance;

			if(iMultiCast < 0)
			{
			   	NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
			}
			else
			{
				NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
			}
		}
		else if((P->AI.iMultiPlayerDist[i] < 2) && (P->AI.iSide != Planes[iSlotToPlane[i]].AI.iSide))
		{
			if(!((GameLoop + loopoffset) % 50))  //  1 update/second
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

				if(iMultiCast < 0)
				{
			   		NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
				}
				else
				{
					NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
				}
			}
		}
		else if((P->AI.iMultiPlayerDist[i] < 6) && ((P->AI.Behaviorfunc == AITankingFlight) || (P->AI.Behaviorfunc == AITankingFlight2AI) || (P->AI.Behaviorfunc == AITankingFlight3AI)))
		{
			//  This updates the tanker position more often when close to another player.
			if(!((GameLoop + loopoffset) % 250))  //  5 update/second
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

				if(iMultiCast < 0)
				{
			   		NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
				}
				else
				{
					NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
				}
			}
		}
		else if((P->AI.iMultiPlayerDist[i] < 40) && (nobehave))
		{
			if(!((GameLoop + loopoffset) % 50))  //  1 update/second
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

				if(iMultiCast < 0)
				{
			   		NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
				}
				else
				{
					NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
				}
			}
		}
		else if(((P->AI.iMultiPlayerDist[i] < 80) || (sendto & (1<<i))) && ((nobehave) || (P->AI.iAIFlags2 & AI_IN_DOGFIGHT)))
		{
			if(!((GameLoop + loopoffset) % 250))  //  1 update/5 seconds
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

				if(iMultiCast < 0)
				{
			   		NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
				}
				else
				{
					NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
				}
			}
		}
		else if((P->AI.iMultiPlayerDist[i] < 120) && (nobehave))
		{
			if(!((GameLoop + loopoffset) % 1500))  //  1 update/30 seconds
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

				if(iMultiCast < 0)
				{
			   		NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
				}
				else
				{
					NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
				}
			}
		}
		else
		{
			if(!((GameLoop + loopoffset) % 3000))  //  1 update/1 minute
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

				if(iMultiCast < 0)
				{
			   		NetSend( Slot[i].dpid, 0, &NetAIPos, sizeof(NetAIPos) );
				}
				else
				{
					NetAddToBigPacket(i, &NetAIPos, sizeof(NetAIPos) );
				}
			}
		}
	}
}

extern double  netKnob;	// feedback loop control valve (50%)
/*----------------------------------------------------------------------------
 *
 *	NetGetAIPos()
 *
 *		Update an AI's World Pos.
 *
 */
void NetGetAIPos( DPMSG_AI_POS *Net )
{
	PlaneParams *P;

	NetSlot *S = &Slot[Net->aislot];

	if(Planes[Net->aislot].AI.iAICombatFlags1 & AI_MULTI_ACTIVE)
	{
		return;
	}

	if((Net->time - S->netTime) < 0)	return;

	//  Save most recent Wpos for next "MovePlane"
	S->netTime = Net->time;
	S->netWPos = Net->fpdWorldPosition;

	P = &Planes[Net->aislot];		// ai plane slot #
	P->AI.iMultiPlayerAIController = Net->slot;

#if 0
	if(S->nPacks++ == 0)
	{
//		NetSetPlane(Net->aislot);
		P->WorldPosition = Net->fpdWorldPosition;
	}
#else

	if(S->nPacks++ == 0)
	{
		P->WorldPosition = Net->fpdWorldPosition;
	}
	else if(((P->WorldPosition - Net->fpdWorldPosition) * WUTOFT) > 3000.0f)
	{
		P->WorldPosition = Net->fpdWorldPosition;
	}
	else
	{
		FPointDouble  nuPos;	// WPos interpolated at Tn

#if 0	//  Since we are doing this as soon as we get this I don't think we need to do this here
		// move oldWPos from T0 to Tn
		nuPos  = P->WorldPosition;
		nuPos -= S->oldWPos;
		nuPos *= dt;
		nuPos += S->oldWPos;	// nuPos = our value interpolated at Tn
#else
		nuPos  = P->WorldPosition;
#endif

		S->netWPos -= nuPos;	// error term: actual minus physics
		S->netDiff  = S->netWPos.QuickLength();
		S->netTime  = 0;		// mark net input as consumed

		S->netWPos *= netKnob;			// scale down error term for feedback
		P->WorldPosition += S->netWPos;	// add in feedback
	}

//	P->WorldPosition = Net->fpdWorldPosition;
#endif

	Net->type = NET_AI_POS;
	Net->time = newTime;
	Net->slot = PlayerPlane - Planes;

//	P->WorldPosition = Net->fpdWorldPosition;  //  ai's position in the world
	P->Heading = Net->Heading;	//  ai's new heading
	P->Pitch = Net->Pitch;		//  ai's new pitch
	P->Roll = Net->Roll;		//  ai's new roll
	P->V = Net->V;			//  ai's velocity in ft/sec
	P->DesiredPitch = Net->DesiredPitch;		//  ai's desired pitch
	P->DesiredRoll = Net->DesiredRoll; 		//  ai's desired roll
	P->DesiredSpeed = Net->DesiredSpeed;		//  ai's desired speed in ft/sec
	//  lFlags stuff.
	if(Net->lFlags1 & NET_SBRAKE)
	{
		P->SpeedBrakeState = 1;
	}
	else
	{
		P->SpeedBrakeState = 0;
	}

	if(Net->lFlags1 & NET_GEAR)
	{
		if(!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			RaiseLowerLandingGear(P,LOWER_LANDING_GEAR);
		}
	}
	else
	{
		if(P->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			RaiseLowerLandingGear(P,RAISE_LANDING_GEAR);
		}
	}

	long ltempflags = (Net->lFlags1 & MC_lF1_MASK)>> MC_SHIFT;
	DPMSG_GENERIC_2_SM NetGSm;
	NetGSm.arg1 = GM2_MULTI_FLAGS;
	NetGSm.arg2 = (BYTE)ltempflags;
	NetGSm.slot = P - Planes;
	NetGetGenericMessage2Small(&NetGSm);

	if(Net->lFlags1 & NET_FLAPS)
	{
		P->Flaps = 30.0f;
		P->FlapsCommandedPos = 30.0f;
	}
	else
	{
		P->Flaps = 0;
		P->FlapsCommandedPos = 0;
	}

	if(Net->lFlags1 & NET_AI_GUN)
	{
		P->AI.iAICombatFlags1 |= AI_MULTI_FIRE_GUN;
	}
	else
	{
		P->AI.iAICombatFlags1 &= ~AI_MULTI_FIRE_GUN;
	}

	if(Net->iAirTarget >= 0)
	{
		P->AI.AirTarget = &Planes[Net->iAirTarget];
	}
	else
	{
		P->AI.AirTarget = NULL;
	}
	if(Net->iAirThreat >= 0)
	{
		P->AI.AirThreat = &Planes[Net->iAirThreat];
	}
	else
	{
		P->AI.AirThreat = NULL;
	}


	RPH_to_Orientation(P);

	S->oldWPos = P->WorldPosition;	// save time/place for next bump

	if(Net->iwaypoint != (P->AI.CurrWay - AIWayPoints))
	{
		MAISetToNewWaypoint(P, Net->iwaypoint);
	}
}


/*----------------------------------------------------------------------------
 *
 *	NetSetAIPlanes()
 *
 *		First time set up 7 wingmen for a Player
 *
 */
void NetSetAIPlanes(int planenum)
{
	return;

	int ix = NetGetSlotFromPlaneIndex(planenum);

	if(ix == -1)
		return;

	int firstingroup = planenum;
	int firstai;
	int cnt2, cnt;
	int workval;
	long numAIs;

	numAIs = GetRegValueL("nummais");
	if(numAIs <= 0)
	{
		return;
	}
	else if(numAIs > 7)
	{
		numAIs = 7;
	}

	cnt =  (iNumAIObjs - 1) + (ix * 7);

	// Try to remember why we did the +1???
	firstai = cnt + 1;
	for(cnt2 = 0; cnt2 < numAIs; cnt2 ++)
	{
		cnt ++;

		//  Might want to run it throught IGAddMover for things like damage or other plane unique things
		Planes[cnt] = Planes[firstingroup];
//			if (!Planes[cnt].SortHeader)
//				GetSortHeader(&Planes[cnt],PLANE_OBJECT);

		Planes[cnt].AI.winglead = -1;
		Planes[cnt].AI.wingman = -1;
		Planes[cnt].AI.prevpair = -1;
		Planes[cnt].AI.nextpair = -1;

		Planes[cnt].AI.iAIFlags1 |= (cnt2 + 1);
		Planes[cnt].AI.lHumanTimer = cnt2 * 7500;

//			Planes[cnt].WorldPosition.X += ConvertWayLoc(75 * cnt);  //  TEMP UNTIL FORMATIONS DONE
//			Planes[cnt].WorldPosition.Z -= ConvertWayLoc(75 * cnt);  //  TEMP UNTIL FORMATIONS DONE
		Planes[cnt].AI.lRadarDelay += (cnt2 + 1) * 1000;


		Planes[cnt].AI.iSkill = 0;

		Planes[cnt].Status &= ~(AL_DEVICE_DRIVEN|AL_COMM_DRIVEN);
		Planes[cnt].Status |= AL_AI_DRIVEN;
		Planes[cnt].DoControlSim = CalcF18ControlSurfacesSimple;
		Planes[cnt].DoCASSim = CalcF18CASUpdatesSimple;
		Planes[cnt].DoForces = CalcAeroForcesSimple;
		Planes[cnt].DoPhysics = CalcAeroDynamicsSimple;
		Planes[cnt].dt = 0;
		Planes[cnt].UpdateRate = HIGH_AERO;


		Planes[cnt].MaxPitchRate = 10.0;
		Planes[cnt].MaxRollRate = 90.0;
		if(pDBAircraftList[Planes[cnt].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
		{
			Planes[cnt].MaxRollRate /= 3.0f;
		}

		Planes[cnt].YawRate = pDBAircraftList[Planes[cnt].AI.iPlaneIndex].iMaxYawRate;

		Planes[cnt].MaxSpeedAccel = pDBAircraftList[Planes[cnt].AI.iPlaneIndex].iAcceleration;
		Planes[cnt].SpeedPercentage = 1.0;

		if(planenum == (PlayerPlane - Planes))
		{
			Planes[cnt].AI.iAIFlags1 |= AIPLAYERGROUP;
			Planes[cnt].AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
			Planes[cnt].AI.iMultiPlayerAIController = planenum;
		}

		Planes[cnt].AI.iAICombatFlags1 |= AI_MULTI_REMOVEABLE;

		Planes[cnt].dt = 0;
		// This will need to be updated once wingmen have assigned voices.  SRE.
		Planes[cnt].AI.lAIVoice = ((cnt2 + 2) * 1000);

		if(cnt2 & 1)
		{
			workval = cnt - 2;
			if(workval < firstai)
			{
				workval = firstingroup;
			}

			Planes[workval].AI.nextpair = cnt;
			Planes[cnt].AI.prevpair = workval;
		}
		else
		{
			workval = cnt - 1;
			if(workval < firstai)
			{
				workval = firstingroup;
			}

			Planes[workval].AI.wingman = cnt;
			Planes[cnt].AI.winglead = workval;
		}
	}
	AIInitFormation(&Planes[firstingroup], 2, -1);
	if(LastPlane < &Planes[cnt])
		LastPlane = &Planes[cnt];
}

/*----------------------------------------------------------------------------
 *
 *	NetReconnectAIPlanes()
 *
 *		After a Player has died then reconnect any wingmen remaining with the Player.
 *
 */
void NetReconnectAIPlanes(int planenum)
{
	return;

	int ix = NetGetSlotFromPlaneIndex(planenum);

	if(ix == -1)
		return;

	int cnt, firstai;
	long numAIs;

	numAIs = GetRegValueL("nummais");
	if(numAIs <= 0)
	{
		return;
	}
	else if(numAIs > 7)
	{
		numAIs = 7;
	}

	//  This must match how it is done in NetSetAIPlanes
	firstai =  ((iNumAIObjs - 1) + (ix * 7)) + 1;
	for(cnt = firstai; cnt < (firstai + numAIs); cnt ++)
	{
		if (Planes[cnt].FlightStatus & (PL_STATUS_CRASHED|PL_OUT_OF_CONTROL))  continue;

		if(cnt == firstai)
		{
			Planes[planenum].AI.wingman = cnt;
			Planes[cnt].AI.winglead = planenum;
			Planes[planenum].AI.nextpair = Planes[cnt].AI.nextpair;
			Planes[cnt].AI.nextpair = -1;
			if(Planes[planenum].AI.nextpair >= 0)
			{
				Planes[Planes[planenum].AI.nextpair].AI.prevpair = planenum;
			}
			return;
		}
		else
		{
			Planes[planenum].AI.nextpair = cnt;
			Planes[cnt].AI.prevpair = planenum;
			return;
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	MAIFireStraightGun(PlaneParams *P)
 *		P is a pointer to the plane firing.
 *
 *		A plane on another machine is firing, have this plane fire as well (bullets won't be checked).
 *
 */
void MAIFireStraightGun(PlaneParams *P)
{
	FPoint offset;
	FPOffSet toffset;
	int gunslot = -1;
	int cnt;

	for(cnt = 0; cnt <MAX_F18E_STATIONS ; cnt ++)
	{
		if(/*  (planepnt->WeapLoad[cnt].Count > 0) && */(P->WeapLoad[cnt].WeapId != 75) && (P->WeapLoad[cnt].WeapId != 76) && (P->WeapLoad[cnt].WeapId != 95) && (P->WeapLoad[cnt].WeapId != 96) && (P->WeapLoad[cnt].WeapId != 97))
		{
	  		if(pDBWeaponList[P->WeapLoad[cnt].WeapIndex].iWeaponType == WEAPON_TYPE_GUN)
			{
				gunslot = cnt;
				break;
			}
		}
	}

	if(gunslot < 0)
	{
		return;
	}

	toffset = pDBAircraftList[P->AI.iPlaneIndex].OffSet[10];
	offset.X = toffset.X * FOOT_MULTER;
	offset.Y = toffset.Y * FOOT_MULTER;
	offset.Z = toffset.Z * FOOT_MULTER;

	InstantiateTurretBullet(P, TRUE, P->WeapLoad[gunslot].Type, P->Heading, P->Pitch, &offset, 0);
}

//============================================================================
//		GENERAL MISSILE MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutMissile()
 *
 *		Tell the world that we fired a missile.
 *
 *		Hook "InstantiateMissile()" and send a guaranteed message.
 *
 */
int NetPutMissileGeneral( PlaneParams *P, int Station, void *GroundLauncher, int sitetype, PlaneParams *pTarget)
{
	BasicInstance  *currweapon;

	if(iMultiCast >= 0)
	{
		return(NetPutMissileGeneralSmall(P, Station, GroundLauncher, sitetype, pTarget));
	}

	NetMissileGeneral.type   = NET_MISSILE_GENERAL;
	NetMissileGeneral.time   = newTime;

	if(P)
	{
		NetMissileGeneral.slot	  = P - Planes;
	}
	else
	{
		NetMissileGeneral.slot	  = 255;
	}

	NetMissileGeneral.target = pTarget ? (pTarget - Planes) : -1;
	NetMissileGeneral.weapon = Station;
	NetMissileGeneral.weapIX = ++NetWeapIX;	// assign next GUID

	if((sitetype == MOVINGVEHICLE) && (GroundLauncher))
	{
		NetMissileGeneral.dwGrndSerialNum = (MovingVehicleParams *)GroundLauncher - MovingVehicles;
	}
	else if(GroundLauncher)
	{
		currweapon  = (BasicInstance *)GroundLauncher;
		NetMissileGeneral.dwGrndSerialNum = currweapon->SerialNumber;

#if 0
		char netstr[80];
		AAWeaponInstance *launcher;
		launcher = (AAWeaponInstance *)GroundLauncher;
		sprintf(netstr, "PUT missile from %s SN %ld, at %d", launcher->Physicals.Object->Filename, currweapon->SerialNumber, NetMissileGeneral.target);
		AICAddAIRadioMsgs(netstr, 50);
#endif
	}
	else
	{
		NetMissileGeneral.dwGrndSerialNum = 0;
	}

	NetMissileGeneral.sitetype = sitetype;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetMissileGeneral, sizeof(NetMissileGeneral) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMissileGeneral()
 *
 *		Fire a missile for an AI
 *
 */
void NetGetMissileGeneral( DPMSG_MISSILE_GENERAL *Net )
{
	PlaneParams *P;
	PlaneParams *Target;
	BasicInstance  *walker;
	void *GroundLauncher;
	MovingVehicleParams *vehiclepnt;
	int Station, radardatnum;

	Target = (Net->target == -1) ? NULL : &Planes[Net->target];
	if(Net->slot != 255)
	{
		P = &Planes[Net->slot];
		P->AADesignate = Target;
		Station = Net->weapon;
		radardatnum = 0;
		if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
		{
			if(P->WeapLoad[Station & ~(0x60)].Count > 0)
			{
				P->WeapLoad[Station & ~(0x60)].Count--;
			}
		}
	}
	else
	{
		P = NULL;
		Station = 0;
		radardatnum = Net->weapon;
	}

	if(Net->sitetype == MOVINGVEHICLE)
	{
		if(Net->dwGrndSerialNum != 255)
		{
			vehiclepnt = &MovingVehicles[Net->dwGrndSerialNum];
			if(vehiclepnt->bDamageLevel >= 2)
			{
				NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
			}

			GroundLauncher = vehiclepnt;
			radardatnum = Station = Net->weapon;
		}
		else
		{
			GroundLauncher = NULL;
		}
	}
	else if(Net->dwGrndSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);

		if(walker)
		{
			GDRadarData *tempradardat = GDGetRadarData(walker);
			if((MultiPlayer) && tempradardat && (tempradardat->lRFlags1 & GD_I_AM_DEAD))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_DEAD, GROUNDOBJECT, Net->dwGrndSerialNum);
			}
		}

		GroundLauncher = walker;
	}
	else
	{
		GroundLauncher = NULL;
	}

	if(!(P || GroundLauncher))
	{
		return;
	}

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)walker;
	sprintf(netstr, "GET missile from %s SN %ld, at %d", launcher->Physicals.Object->Filename, walker->SerialNumber, Net->target);
	AICAddAIRadioMsgs(netstr, 50);
#endif

	InstantiateMissile( P, Station, GroundLauncher, Net->sitetype, Target, Net->weapIX, radardatnum);
}

/*----------------------------------------------------------------------------
 *
 *	MAIIsActiveWeapon(WeaponParams *W)
 *
 *		Determines if a weapon should be processed as a "real" weapon
 *
 */
int MAIIsActiveWeapon(WeaponParams *W, int checkplanes, int checkground)
{
	PlaneParams *target;

	if(!MultiPlayer)
	{
		return(0);
	}

	if(checkplanes)
	{
		if(((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))
		{
			return(1);
		}
	}

	if(checkground)
	{
		target = (PlaneParams *)W->pTarget;

		if(W->P == NULL)
		{
			if(target)
			{
				if((target->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (target == PlayerPlane))
				{
					return(1);
				}
			}
			else
			{
//				if((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)) || ((W->P == NULL) && (lAINetFlags1 & NGAI_ACTIVE)))
				if(lAINetFlags1 & NGAI_ACTIVE)
				{
					return(1);
				}
			}
		}
	}

	return(0);
}


//============================================================================
//		GENERAL GROUND RADAR LOCKUP MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutGroundLock(BasicInstance *foundweapon, BasicInstance *radarsite, PlaneParams *planepnt)
 *
 *		Sets things up in MultiPlayer so that TEWS will show the correct lock information on all systems.
 *
 */
void NetPutGroundLock(BasicInstance *foundweapon, BasicInstance *radarsite, PlaneParams *planepnt)
{
	if(iMultiCast >= 0)
	{
		NetPutGroundLockSmall(foundweapon, radarsite, planepnt);
		return;
	}

	NetGroundLock.type   = NET_GROUND_LOCK;
	NetGroundLock.time   = newTime;

	NetGroundLock.slot	  = PlayerPlane - Planes;

	NetGroundLock.target = planepnt ? (planepnt - Planes) : -1;

	if(foundweapon)
	{
		NetGroundLock.dwWeaponSerialNum = foundweapon->SerialNumber;
	}
	else
	{
		NetGroundLock.dwWeaponSerialNum = 0;
	}

	if(radarsite)
	{
		NetGroundLock.dwRadarSerialNum = radarsite->SerialNumber;
	}
	else
	{
		NetGroundLock.dwRadarSerialNum = 0;
	}

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)foundweapon;
	sprintf(netstr, "PUT lock from %s SN %ld radar %ld, on %d", launcher->Physicals.Object->Filename, NetGroundLock.dwWeaponSerialNum, NetGroundLock.dwRadarSerialNum, NetGroundLock.target);
	AICAddAIRadioMsgs(netstr, 50);
#endif

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetGroundLock, sizeof(NetGroundLock) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGroundLock()
 *
 *		Lock up a plane with a ground radar
 *
 */
void NetGetGroundLock( DPMSG_GROUND_LOCK *Net )
{
	PlaneParams *planepnt;
	BasicInstance  *walker;
	BasicInstance  *radarsite;
	GDRadarData *radardat;
	int notradar;
	int isaaa;
	AAWeaponInstance *foundweapon;
	float currdist;
	DBWeaponType *weapon;
	int debugnum = 0;

	planepnt = (Net->target == -1) ? NULL : &Planes[Net->target];

	if(Net->dwWeaponSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwWeaponSerialNum);
		if(walker)
		{
			GDRadarData *tempradardat = GDGetRadarData(walker);
			if((MultiPlayer) && tempradardat && (tempradardat->lRFlags1 & GD_I_AM_DEAD))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_DEAD, GROUNDOBJECT, Net->dwWeaponSerialNum);
			}
		}
	}
	else
	{
		walker = NULL;
	}

	if(Net->dwRadarSerialNum == Net->dwWeaponSerialNum)
	{
		radarsite = walker;
	}
	else if(Net->dwRadarSerialNum)
	{
		radarsite = FindInstance(AllInstances, Net->dwRadarSerialNum);
		if(radarsite)
		{
			GDRadarData *tempradardat = GDGetRadarData(radarsite);
			if((MultiPlayer) && tempradardat && (tempradardat->lRFlags1 & GD_I_AM_DEAD))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_DEAD, GROUNDOBJECT, Net->dwRadarSerialNum);
			}
		}

	}
	else
	{
		radarsite = NULL;
	}

	if(!(planepnt && walker))
	{
		walker = FindInstance(AllInstances, Net->dwWeaponSerialNum);

		foundweapon = AllAAWeapons;
		walker = (BasicInstance *)foundweapon;

		while (foundweapon && (walker->SerialNumber != Net->dwWeaponSerialNum))
		{
			foundweapon = (AAWeaponInstance *)foundweapon->Basics.NextRelatedInstance;
			if(foundweapon)
			{
				walker = (BasicInstance *)foundweapon;
			}
		}
		return;
	}

	if(radarsite)
	{
		GDUpgradeTreeAlert(GDGetInfoLeader(radarsite), radarsite, planepnt->WorldPosition);
	}

	if(!iAAAFireAlreadyChecked)
		GDNoticeAAAFire(&walker->Position, GDConvertGrndSide(walker));

	currdist = planepnt->WorldPosition - walker->Position;

	foundweapon = (AAWeaponInstance *)walker;

	radardat = GDGetRadarData(walker);

	radardat->Target = planepnt;
	radardat->iTargetDist = currdist * WUTONM;

	weapon = GetWeaponPtr(walker);

	if(!weapon)
	{
		return;
	}

	if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10) || (weapon->iWeaponType == 6))
	{
		isaaa = 1;
		radardat->lWFlags1 &= ~GD_W_ACQUIRING;
	}
	else if((weapon->iSeekerType != 1) && (weapon->iSeekerType != 7))
	{
		notradar = 1;
		isaaa = 0;
	}
	else
	{
		notradar = 0;
		isaaa = 0;
	}

	if((!radarsite) || (radarsite == walker))
	{
		debugnum = 7;
		if(planepnt->AI.pPaintedBy == NULL)
		{
			planepnt->AI.pPaintedBy = walker;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = currdist * WUTONM;
		}
		else if(planepnt->AI.iPaintDist > (currdist * WUTONM))
		{
			planepnt->AI.pPaintedBy = walker;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = currdist * WUTONM;
		}
		if(weapon->iSeekerType == 1)
		{
			radardat->lRFlags1 |= GD_CHECK_MISSILES;
		}
		else if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			GDCheckForPingReport(walker, planepnt);
		}
		radardat->pWRadarSite = radarsite;
		if(isaaa)
		{
			debugnum = 8;
			GDFireInitialAAA(foundweapon, planepnt);
		}
	}
	else
	{
		radardat->pWRadarSite = radarsite;
		planepnt->AI.pPaintedBy = radarsite;
		planepnt->AI.iPaintedByType = GROUNDOBJECT;
		planepnt->AI.iPaintDist = (radarsite->Position - planepnt->WorldPosition) * WUTONM;
		radardat = GDGetRadarData(radarsite);
		debugnum = 1;
		if(weapon->iSeekerType == 1)
		{
			radardat->lRFlags1 |= GD_CHECK_MISSILES;
			radardat->Target = planepnt;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
			debugnum = 2;
		}
		else if(weapon->iSeekerType == 7)
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			radardat->Target = planepnt;
			GDCheckForPingReport(radarsite, planepnt);
			debugnum = 3;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
		}
		else if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10))  // Not sure about 10
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			radardat->Target = planepnt;
			GDCheckForPingReport(radarsite, planepnt);
			debugnum = 4;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
			if(isaaa)
			{
				debugnum = 5;
				GDFireInitialAAA(foundweapon, planepnt);
			}
		}
		else
		{
			if(isaaa)
			{
				debugnum = 6;
				GDFireInitialAAA(foundweapon, planepnt);
			}
		}
	}

	if(radarsite)
	{
		if(planepnt == PlayerPlane)
		{
			if(GetRadarPtr(radarsite))
			{
				radardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
			}
		}
	}

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)walker;
	sprintf(netstr, "GET lock from %s SN %ld radar %ld, on %d, DN %d", launcher->Physicals.Object->Filename, Net->dwWeaponSerialNum, Net->dwRadarSerialNum, Net->target, debugnum);
	AICAddAIRadioMsgs(netstr, 50);
#endif
}

//============================================================================
//		GENERAL BOMB MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutBombGeneral()
 *
 *		Tell the world that we dropped a bomb.
 *
 *		Hook "InstantiateBomb()" and send a guaranteed message.
 *
 */
int NetPutBombGeneral( PlaneParams *P, int Station , double burst_height, void *GroundTarget, int groundtype, double GX, double GY, double GZ)
{
	BasicInstance  *currweapon;
	MovingVehicleParams *vehiclepnt;

	if(iMultiCast >= 0)
	{
		return(NetPutBombGeneralSmall(P, Station, burst_height, GroundTarget, groundtype, GX, GY, GZ));
	}

	NetBombGeneral.type   = NET_BOMB_GENERAL;
	NetBombGeneral.time   = newTime;

	if(P)
	{
		NetBombGeneral.slot	  = P - Planes;
	}
	else
	{
		NetBombGeneral.slot	  = 255;
	}

	NetBombGeneral.weapon = Station;
	NetBombGeneral.weapIX = ++NetWeapIX;	// assign next GUID
	NetBombGeneral.burst_height = burst_height;

	if(GroundTarget)
	{
		if(groundtype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)GroundTarget;
			NetBombGeneral.dwGrndSerialNum = vehiclepnt - MovingVehicles;
		}
		else
		{
			currweapon  = (BasicInstance *)GroundTarget;
			NetBombGeneral.dwGrndSerialNum = currweapon->SerialNumber;
		}
	}
	else
	{
		groundtype = NO_TARGET;
		NetBombGeneral.dwGrndSerialNum = 0;
	}
	NetBombGeneral.groundtype = groundtype;

	NetBombGeneral.GX = GX;
	NetBombGeneral.GY = GY;
	NetBombGeneral.GZ = GZ;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetBombGeneral, sizeof(NetBombGeneral) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetBombGeneral()
 *
 *		Drop a bomb for an AI
 *
 */
void NetGetBombGeneral( DPMSG_BOMB_GENERAL *Net )
{
	PlaneParams *P;
	BasicInstance  *walker;

	if(Net->slot != 255)
	{
		P = &Planes[Net->slot];
		if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
		{
			if(P->WeapLoad[Net->weapon & ~(0x40)].Count > 0)
			{
				P->WeapLoad[Net->weapon & ~(0x40)].Count--;
			}
		}
	}
	else
	{
		P = NULL;
	}

	if(Net->dwGrndSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);
	}
	else
	{
		walker = NULL;
	}

	if(!(P || walker))
	{
		return;
	}

	InstantiateBomb( P, Net->weapon, Net->burst_height, walker, Net->groundtype, Net->GX, Net->GY, Net->GZ, Net->weapIX );
}

//============================================================================
//		GENERAL AG MISSILE MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutAGMissileGeneral()
 *
 *		Tell the world that we dropped a bomb.
 *
 *		Hook "InstantiateAGMissile()" and send a guaranteed message.
 *
 */
int NetPutAGMissileGeneral(PlaneParams *P, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile)
{
	BasicInstance  *currweapon;
	MovingVehicleParams *vehiclepnt;

	if(iMultiCast >= 0)
	{
		return(NetPutAGMissileGeneralSmall(P, Station, GroundTarget, targetloc, groundtype, flightprofile));
	}

	NetAGMissileGeneral.type   = NET_AG_MISSILE_GENERAL;
	NetAGMissileGeneral.time   = newTime;

	if(P)
	{
		NetAGMissileGeneral.slot	  = P - Planes;
	}
	else
	{
		NetAGMissileGeneral.slot	  = 255;
	}

	NetAGMissileGeneral.weapon = Station;
	NetAGMissileGeneral.weapIX = ++NetWeapIX;	// assign next GUID

	if(!GroundTarget)
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneral.dwGrndSerialNum = 0;
	}
	else if(groundtype == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)GroundTarget;
		NetAGMissileGeneral.dwGrndSerialNum = vehiclepnt - MovingVehicles;
	}
	else if(GroundTarget)
	{
		currweapon  = (BasicInstance *)GroundTarget;
		NetAGMissileGeneral.dwGrndSerialNum = currweapon->SerialNumber;
	}
	else
	{
		NetAGMissileGeneral.dwGrndSerialNum = 0;
	}
	NetAGMissileGeneral.groundtype = groundtype;

	NetAGMissileGeneral.targetloc = targetloc;
	NetAGMissileGeneral.profile = flightprofile;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAGMissileGeneral, sizeof(NetAGMissileGeneral) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetAGMissileGeneral()
 *
 *		Launch an AG Missile
 *
 */
void NetGetAGMissileGeneral( DPMSG_AG_MISSILE_GENERAL *Net )
{
	PlaneParams *P;
	BasicInstance  *walker;
	MovingVehicleParams *vehiclepnt;
	void *GroundTarget;

	if(Net->groundtype == MOVINGVEHICLE)
	{
		if(Net->dwGrndSerialNum != 255)
		{
			vehiclepnt = &MovingVehicles[Net->dwGrndSerialNum];
			GroundTarget = vehiclepnt;
		}
		else
		{
			GroundTarget = NULL;
		}
	}
	else if(Net->dwGrndSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);
		GroundTarget = walker;
	}
	else
	{
		GroundTarget = NULL;
	}

//	if(!GroundTarget)
//	{
//		return;
//	}

	if(Net->weapon & 0x20)
	{
		if(Net->weapon & 0x40)
		{
			if(Net->slot != 255)
			{
		 		vehiclepnt = &MovingVehicles[Net->slot];
				if(vehiclepnt->bDamageLevel >= 2)
				{
					NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
				}
			}
			else
			{
				return;
			}

			WCheckWeaponNowPlane(vehiclepnt, MOVINGVEHICLE, GroundTarget, Net->groundtype, Net->targetloc, Net->weapIX, (Net->weapon & ~(0x60)), Net->profile);
		}
		else
		{
			if(Net->slot != 255)
			{
				P = &Planes[Net->slot];
				if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
				{
					if(P->WeapLoad[Net->weapon & ~(0x60)].Count > 0)
					{
						P->WeapLoad[Net->weapon & ~(0x60)].Count--;
					}
				}
			}
			else
			{
				return;
			}

			WCheckWeaponNowPlane(P, AIRCRAFT, GroundTarget, Net->groundtype, Net->targetloc, Net->weapIX, (Net->weapon & ~(0x60)), Net->profile);
		}
	}
	else if(Net->weapon & 0x40)
	{
		if(Net->slot != 255)
		{
		 	vehiclepnt = &MovingVehicles[Net->slot];
			if(vehiclepnt->bDamageLevel >= 2)
			{
				NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
			}
		}
		else
		{
			return;
		}

		InstantiateSSMissile(vehiclepnt, MOVINGVEHICLE, GroundTarget, Net->groundtype, Net->targetloc, Net->weapIX, (Net->weapon & ~(0x40)), Net->profile);
	}
	else
	{
		if(Net->slot != 255)
		{
			P = &Planes[Net->slot];
			if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
			{
				if(P->WeapLoad[Net->weapon & ~(0x60)].Count > 0)
				{
					P->WeapLoad[Net->weapon & ~(0x60)].Count--;
				}
			}
		}
		else
		{
			return;
		}

		InstantiateAGMissile( P, Net->weapon, GroundTarget, Net->targetloc, Net->groundtype, Net->weapIX, Net->profile);
	}
}

void NetAddToBigPacket(int packet_num, void *lpMsg, int size)
{
	void *pputpacket;
	int planenum;
	DPMSG_F18GENERIC *Net;

	if(!MultiPlayer)
	{
		return;
	}

	if(clearpackets)
	{
		int cnt;

		clearpackets = 0;
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			iBigPacketSize[cnt] = sizeof(DPMSG_BIG_PACKET_HEAD);
			iStartNextPacket[cnt] = 0;
			dwLastBPSent[cnt] = 0;
			BigPackets[cnt].type = NET_BIG_PACKET;

//			BigPackets[cnt].slot = PlayerPlane - Planes;

			if(iSlotToPlane[MySlot] >= 0)
				BigPackets[cnt].slot = iSlotToPlane[MySlot];
			else
				BigPackets[cnt].slot = 0;

			BigPackets[cnt].num_packets = 0;
		}
	}

	if((iMAISendTo >= 0) && (iMAISendTo < MAX_HUMANS) && (packet_num == MySlot))
	{
		NetAddToBigPacket(iMAISendTo, lpMsg, size);
		return;
	}

	if((iMultiCast == 0) && (packet_num == MySlot))
	{
		NoMultiCastBigPackets(lpMsg, size);
		return;
	}

	if((iBigPacketSize[packet_num] + size) > iMaxPacketSize)
	{
		BigPackets[packet_num].time = newTime;;
		if(iSlotToPlane[MySlot] >= 0)
			BigPackets[packet_num].slot = iSlotToPlane[MySlot];

		if(packet_num == MySlot)
		{
			NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &BigPackets[packet_num], iBigPacketSize[packet_num]);
		}
		else
		{
			planenum = NetGetPlaneIndexFromSlot(packet_num);
			if(planenum != -1)
			{
				NetSend( Slot[planenum].dpid, DPSEND_GUARANTEED, &BigPackets[packet_num], iBigPacketSize[packet_num]);
			}
		}
		for(int cnt = 0; cnt < 4; cnt ++)
		{
			lCurrBytes[cnt] += iBigPacketSize[packet_num];
		}
		lTotalBytes += iBigPacketSize[packet_num];

		iBigPacketSize[packet_num] = sizeof(DPMSG_BIG_PACKET_HEAD);
		iStartNextPacket[packet_num] = 0;
		dwLastBPSent[packet_num] = 0;
		BigPackets[packet_num].num_packets = 0;
		pLastBPPos[packet_num] = NULL;
	}

	if(dwLastBPSent[packet_num] == 0)
	{
		dwLastBPSent[packet_num] = GetTickCount();
	}

	BYTE bpackettype;
	Net = (DPMSG_F18GENERIC *)lpMsg;
	bpackettype = Net->type;

	pputpacket = &BigPackets[packet_num].smaller_packets[iStartNextPacket[packet_num]];
	if((bpackettype == NET_PLANEDATA_SM) && (size == sizeof(DPMSG_PLANEDATA_SM)))
	{
		if(pLastBPPos[packet_num] == NULL)
		{
			pLastBPPos[packet_num] = (DPMSG_PLANEDATA_SM *)pputpacket;
		}
		else
		{
			pputpacket = (void *)pLastBPPos[packet_num];
			memcpy(pputpacket, lpMsg, size);
			return;
		}
	}
	memcpy(pputpacket, lpMsg, size);
	iStartNextPacket[packet_num] = iStartNextPacket[packet_num] + size;
	iBigPacketSize[packet_num] = iBigPacketSize[packet_num] + size;
	BigPackets[packet_num].num_packets = BigPackets[packet_num].num_packets + 1;
}

void NetGetBigPacket(DPMSG_BIG_PACKET	*Net)
{
 	char *ppacket;
	BYTE bpackettype;
	int fromslot = Net->slot;

	ppacket = Net->smaller_packets;
	while(Net->num_packets > 0)
	{
		bpackettype = (BYTE)*ppacket;
		switch (bpackettype)
		{
			case NET_PLANEDATA:
				NetGetPlane(   (DPMSG_PLANEDATA *)ppacket );
				ppacket += sizeof(DPMSG_PLANEDATA);
				break;
			case NET_CHATTER:
				NetGetChat(    (DPMSG_CHATTER   *)ppacket );
				ppacket += sizeof(DPMSG_CHATTER);
				break;
			case NET_PAUSED:
				NetGetPause(   (DPMSG_PAUSED    *)ppacket );
				ppacket += sizeof(DPMSG_PAUSED);
				break;
			case NET_MISSILE:
				NetGetMissile( (DPMSG_MISSILE   *)ppacket );
				ppacket += sizeof(DPMSG_MISSILE);
				break;
			case NET_TARGET:
				NetGetTarget(  (DPMSG_TARGET    *)ppacket );
				ppacket += sizeof(DPMSG_TARGET);
				break;
			case NET_STRIKE:
				NetGetStrike(  (DPMSG_STRIKE    *)ppacket );
				ppacket += sizeof(DPMSG_STRIKE);
				break;
			case NET_DAMAGE:
				NetGetDamage(  (DPMSG_DAMAGE    *)ppacket );
				ppacket += sizeof(DPMSG_DAMAGE);
				break;
			case NET_CRASH:
				NetGetCrash(   (DPMSG_CRASH     *)ppacket );
				ppacket += sizeof(DPMSG_CRASH);
				break;
			case NET_DECOY:
				NetGetDecoy(   (DPMSG_DECOY     *)ppacket );
				ppacket += sizeof(DPMSG_DECOY);
				break;
			case NET_EJECT:
				NetGetEject(   (DPMSG_EJECT     *)ppacket );
				ppacket += sizeof(DPMSG_EJECT);
				break;
			case NET_AI_POS:
				NetGetAIPos(   (DPMSG_AI_POS    *)ppacket );
				ppacket += sizeof(DPMSG_AI_POS);
				break;
			case NET_MISSILE_GENERAL:
				NetGetMissileGeneral((DPMSG_MISSILE_GENERAL    *)ppacket );
				ppacket += sizeof(DPMSG_MISSILE_GENERAL);
				break;
			case NET_GROUND_LOCK:
				NetGetGroundLock((DPMSG_GROUND_LOCK *)ppacket );
				ppacket += sizeof(DPMSG_GROUND_LOCK);
				break;
			case NET_BOMB_GENERAL:
				NetGetBombGeneral((DPMSG_BOMB_GENERAL *)ppacket );
				ppacket += sizeof(DPMSG_BOMB_GENERAL);
				break;
			case NET_AG_MISSILE_GENERAL:
				NetGetAGMissileGeneral((DPMSG_AG_MISSILE_GENERAL    *)ppacket );
				ppacket += sizeof(DPMSG_AG_MISSILE_GENERAL);
				break;
			case NET_ZONES:
				NetGetZones((DPMSG_CONTROL_ZONES *)ppacket);
				ppacket += sizeof(DPMSG_CONTROL_ZONES);
				break;
			case NET_AI_COMMS:
				NetGetAIComms((DPMSG_AI_COMMS *)ppacket );
				ppacket += sizeof(DPMSG_AI_COMMS);
				break;
			case NET_AI_HEADING:
				NetGetDesiredHeading((DPMSG_DESIRED_HEADING *)ppacket);
				ppacket += sizeof(DPMSG_DESIRED_HEADING);
				break;
			case NET_CAS_DATA:
				NetGetCASData((DPMSG_CAS_DATA *)ppacket);
				ppacket += sizeof(DPMSG_CAS_DATA);
				break;
			case NET_GENERIC_1:
				NetGetGenericMessage1((DPMSG_GENERIC_1 *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_1);
				break;
			case NET_GENERIC_2:
				NetGetGenericMessage2((DPMSG_GENERIC_2 *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2);
				break;
			case NET_GENERIC_3:
				NetGetGenericMessage3((DPMSG_GENERIC_3 *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_3);
				break;
			case NET_GENERIC_4:
				NetGetGenericMessage4((DPMSG_GENERIC_4 *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_4);
				break;
			case NET_DF_UPDATE:
				NetGetDogfightUpdate( (DPMSG_DOGFIGHT_UPDATE *)ppacket );
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE);
				break;
			case NET_DF_UPDATE_CPOS:
				NetGetDogfightCPosUpdate( (DPMSG_DOGFIGHT_UPDATE_CPOS *)ppacket );
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE_CPOS);
				break;
			case NET_MISSILE_BREAK:
				NetGetMissileBreak( (DPMSG_MISSILE_BREAK *)ppacket );
				ppacket += sizeof(DPMSG_MISSILE_BREAK);
				break;
			case NET_WEAPON_G_EXPLODE:
				NetGetWeaponGExplode( (DPMSG_WEAPON_G_EXPLODE *)ppacket );
				ppacket += sizeof(DPMSG_WEAPON_G_EXPLODE);
				break;
			case NET_GENERIC_2_LONG:
				NetGetGenericMessage2Long((DPMSG_GENERIC_2_LONG *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_LONG);
				break;
			case NET_GENERIC_3_LONG:
				NetGetGenericMessage3Long((DPMSG_GENERIC_3_LONG *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_3_LONG);
				break;
			case NET_GENERIC_4_LONG:
				NetGetGenericMessage4Long((DPMSG_GENERIC_4_LONG *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_4_LONG);
				break;
			case NET_GENERIC_2_FPOINT:
				NetGetGenericMessage2FPoint((DPMSG_GENERIC_2_FPOINT *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_FPOINT);
				break;
			case NET_GENERIC_OBJ_POINT:
				NetGetGenericMessageObjectPoint((DPMSG_GENERIC_OBJ_POINT *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_OBJ_POINT);
				break;
			case NET_GENERIC_2_FLOAT:
				NetGetGenericMessage2Float((DPMSG_GENERIC_2_FLOAT *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_FLOAT);
				break;
			case NET_GENERIC_3_FLOAT:
				NetGetGenericMessage3Float((DPMSG_GENERIC_3_FLOAT *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_3_FLOAT);
				break;
			case NET_CHECK_DATA_HACK:
				NetGetDBInfo((DPMSG_WEAPON_HACK_CHECK *)ppacket );
				ppacket += sizeof(DPMSG_WEAPON_HACK_CHECK);
				break;
			case NET_PLANEDATA_SM:
				NetGetPlaneSmall(fromslot, (DPMSG_PLANEDATA_SM *)ppacket );
				ppacket += sizeof(DPMSG_PLANEDATA_SM);
				break;
			case NET_CHATTER_SM:
				NetGetChatSmall(    (DPMSG_CHATTER_SM   *)ppacket );
				ppacket += sizeof(DPMSG_CHATTER_SM);
				break;
			case NET_MISSILE_SM:
				NetGetMissileSmall( (DPMSG_MISSILE_SM   *)ppacket );
				ppacket += sizeof(DPMSG_MISSILE_SM);
				break;
			case NET_TARGET_SM:
				NetGetTargetSmall(  (DPMSG_TARGET_SM    *)ppacket );
				ppacket += sizeof(DPMSG_TARGET_SM);
				break;
			case NET_STRIKE_SM:
			case NET_BULLET_STRIKE_SM:
				NetGetStrikeSmall(  (DPMSG_STRIKE_SM    *)ppacket );
				ppacket += sizeof(DPMSG_STRIKE_SM);
				break;
			case NET_DAMAGE_SM:
				NetGetDamageSmall(  (DPMSG_DAMAGE_SM    *)ppacket );
				ppacket += sizeof(DPMSG_DAMAGE_SM);
				break;
			case NET_CRASH_SM:
				NetGetCrashSmall(   (DPMSG_CRASH_SM     *)ppacket );
				ppacket += sizeof(DPMSG_CRASH_SM);
				break;
			case NET_DECOY_SM:
				NetGetDecoySmall(   (DPMSG_DECOY_SM     *)ppacket );
				ppacket += sizeof(DPMSG_DECOY_SM);
				break;
			case NET_EJECT_SM:
				NetGetEjectSmall(   (DPMSG_EJECT_SM     *)ppacket );
				ppacket += sizeof(DPMSG_EJECT_SM);
				break;
			case NET_AI_POS_SM:
				NetGetAIPosSmall(fromslot, (DPMSG_AI_POS_SM    *)ppacket );
				ppacket += sizeof(DPMSG_AI_POS_SM);
				break;
			case NET_MISSILE_GENERAL_SM:
				NetGetMissileGeneralSmall((DPMSG_MISSILE_GENERAL_SM    *)ppacket );
				ppacket += sizeof(DPMSG_MISSILE_GENERAL_SM);
				break;
			case NET_GROUND_LOCK_SM:
				NetGetGroundLockSmall((DPMSG_GROUND_LOCK_SM *)ppacket );
				ppacket += sizeof(DPMSG_GROUND_LOCK_SM);
				break;
			case NET_BOMB_GENERAL_SM:
				NetGetBombGeneralSmall((DPMSG_BOMB_GENERAL_SM *)ppacket );
				ppacket += sizeof(DPMSG_BOMB_GENERAL_SM);
				break;
			case NET_AG_MISSILE_GENERAL_SM:
				NetGetAGMissileGeneralSmall((DPMSG_AG_MISSILE_GENERAL_SM    *)ppacket );
				ppacket += sizeof(DPMSG_AG_MISSILE_GENERAL_SM);
				break;
			case NET_ZONES_SM:
				NetGetZonesSmall(fromslot, (DPMSG_CONTROL_ZONES_SM *)ppacket);
				ppacket += sizeof(DPMSG_CONTROL_ZONES_SM);
				break;
			case NET_AI_COMMS_SM:
				NetGetAICommsSmall(fromslot, (DPMSG_AI_COMMS_SM *)ppacket );
				ppacket += sizeof(DPMSG_AI_COMMS_SM);
				break;
			case NET_AI_HEADING_SM:
				NetGetDesiredHeadingSmall((DPMSG_DESIRED_HEADING_SM *)ppacket);
				ppacket += sizeof(DPMSG_DESIRED_HEADING_SM);
				break;
			case NET_CAS_DATA_SM:
				NetGetCASDataSmall((DPMSG_CAS_DATA_SM *)ppacket);
				ppacket += sizeof(DPMSG_CAS_DATA_SM);
				break;
			case NET_GENERIC_1_SM:
				NetGetGenericMessage1Small((DPMSG_GENERIC_1_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_1_SM);
				break;
			case NET_GENERIC_2_SM:
				NetGetGenericMessage2Small((DPMSG_GENERIC_2_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_SM);
				break;
			case NET_GENERIC_3_SM:
				NetGetGenericMessage3Small((DPMSG_GENERIC_3_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_3_SM);
				break;
			case NET_GENERIC_4_SM:
				NetGetGenericMessage4Small((DPMSG_GENERIC_4_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_4_SM);
				break;
			case NET_DF_UPDATE_SM:
				NetGetDogfightUpdateSmall( (DPMSG_DOGFIGHT_UPDATE_SM *)ppacket );
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE_SM);
				break;
			case NET_DF_UPDATE_CPOS_SM:
				NetGetDogfightCPosUpdateSmall( (DPMSG_DOGFIGHT_UPDATE_CPOS_SM *)ppacket );
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE_CPOS_SM);
				break;
			case NET_MISSILE_BREAK_SM:
				NetGetMissileBreakSmall( (DPMSG_MISSILE_BREAK_SM *)ppacket );
				ppacket += sizeof(DPMSG_MISSILE_BREAK_SM);
				break;
			case NET_WEAPON_G_EXPLODE_SM:
				NetGetWeaponGExplodeSmall( (DPMSG_WEAPON_G_EXPLODE_SM *)ppacket );
				ppacket += sizeof(DPMSG_WEAPON_G_EXPLODE_SM);
				break;
			case NET_GENERIC_2_LONG_SM:
				NetGetGenericMessage2LongSmall((DPMSG_GENERIC_2_LONG_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_LONG_SM);
				break;
			case NET_GENERIC_3_LONG_SM:
				NetGetGenericMessage3LongSmall((DPMSG_GENERIC_3_LONG_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_3_LONG_SM);
				break;
			case NET_GENERIC_4_LONG_SM:
				NetGetGenericMessage4LongSmall((DPMSG_GENERIC_4_LONG_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_4_LONG_SM);
				break;
			case NET_GENERIC_2_FPOINT_SM:
				NetGetGenericMessage2FPointSmall((DPMSG_GENERIC_2_FPOINT_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_FPOINT_SM);
				break;
			case NET_GENERIC_OBJ_POINT_SM:
				NetGetGenericMessageObjectPointSmall((DPMSG_GENERIC_OBJ_POINT_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_OBJ_POINT_SM);
				break;
			case NET_VGROUND_LOCK:
				NetGetVehicleGroundLock((DPMSG_VGROUND_LOCK *)ppacket );
				ppacket += sizeof(DPMSG_VGROUND_LOCK);
				break;
			case NET_VGROUND_LOCK_SM:
				NetGetVehicleGroundLockSmall((DPMSG_VGROUND_LOCK_SM *)ppacket );
				ppacket += sizeof(DPMSG_VGROUND_LOCK_SM);
				break;
			case NET_GENERIC_2_FLOAT_SM:
				NetGetGenericMessage2FloatSmall((DPMSG_GENERIC_2_FLOAT_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_2_FLOAT_SM);
				break;
			case NET_GENERIC_3_FLOAT_SM:
				NetGetGenericMessage3FloatSmall((DPMSG_GENERIC_3_FLOAT_SM *)ppacket );
				ppacket += sizeof(DPMSG_GENERIC_3_FLOAT_SM);
				break;
			case NET_CHECK_DATA_HACK_SM:
				NetGetDBInfoSmall(fromslot, (DPMSG_WEAPON_HACK_CHECK_SM *)ppacket );
				ppacket += sizeof(DPMSG_WEAPON_HACK_CHECK_SM);
				break;
			default:
				bpackettype = 0;
				break;
		}
		Net->num_packets --;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutMissileSmall()
 *
 *		Tell the world that we fired a missile.
 *
 *		Hook "InstantiateMissile()" and send a guaranteed message.
 *
 */
int NetPutMissileSmall( PlaneParams *P, int station, PlaneParams *pTarget )
{
	DPMSG_MISSILE_SM NetMissileSmall;

	NetMissileSmall.type   = NET_MISSILE_SM;
	NetMissileSmall.slot	  = P - Planes;
	NetMissileSmall.target = pTarget ? (pTarget - Planes) : 255;
	NetMissileSmall.weapon = station;
	NetMissileSmall.weapIX = ++NetWeapIX;	// assign next GUID

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetMissileSmall, sizeof(NetMissileSmall) );
	NetAddToBigPacket(MySlot, &NetMissileSmall, sizeof(NetMissileSmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMissileSmall()
 *
 *		Fire a missile for a remote player.
 *
 */
void NetGetMissileSmall( DPMSG_MISSILE_SM *Net )
{
	PlaneParams *P = &Planes[Net->slot];

	if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
	{
		if(P->WeapLoad[Net->weapon & ~(0x60)].Count > 0)
		{
			P->WeapLoad[Net->weapon & ~(0x60)].Count--;
		}
	}

	P->AADesignate = (Net->target == 255) ? NULL : &Planes[Net->target];

	InstantiateMissile( P, Net->weapon, NULL,0, P->AADesignate, Net->weapIX );
}

/*----------------------------------------------------------------------------
 *
 *	NetPutTargetSmall()
 *
 *		Tell the world that we locked a target.
 *
 *		Hook "SendNetWeaponTargetChange()" and send a guaranteed message.
 *
 *		void SendNetWeaponTargetChange(int weaponid, int targetid)
 */
void NetPutTargetSmall( int weapIX, int target  )
{
	DPMSG_TARGET_SM	NetTargetSmall;

	NetTargetSmall.type   = NET_TARGET_SM;
	NetTargetSmall.target = target;
	NetTargetSmall.weapIX = weapIX;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetTargetSmall, sizeof(NetTargetSmall) );
	NetAddToBigPacket(MySlot, &NetTargetSmall, sizeof(NetTargetSmall) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetTargetSmall()
 *
 *		Lock a target for a remote player.
 *
 *		void UpdateNetWeaponTarget(int weaponid, int targetid)
 *
 */
void NetGetTargetSmall( DPMSG_TARGET_SM *Net )
{
	int target;

	target = Net->target;
	UpdateNetWeaponTarget( Net->weapIX, target );
}

/*----------------------------------------------------------------------------
 *
 *	NetPutStrikeSmall()
 *
 *		Intercept DamagePlane().
 *
 */
void NetPutStrikeSmall( PlaneParams *P )
{
	DPMSG_STRIKE_SM  NetStrikeSmall;

	NetStrikeSmall.type	 = NET_STRIKE_SM;

	if(iNetBulletHit)
	{
		NetStrikeSmall.type	 = NET_BULLET_STRIKE_SM;
		iNetBulletHit = 0;
	}

	if(iWeaponFiredBy == -1)
	{
		NetStrikeSmall.slot	 = 255;
	}
	else
	{
		NetStrikeSmall.slot	 = iWeaponFiredBy;
	}

	NetStrikeSmall.target = P - Planes;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetStrikeSmall, sizeof(NetStrikeSmall) );
	NetAddToBigPacket(MySlot, &NetStrikeSmall, sizeof(NetStrikeSmall) );

	// Moved this after added to big packet so that I know if this is a new person hitting this target.
	// Assign credit for strike
	NetSetKill( iWeaponFiredBy, P-Planes );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetStrikeSmall()
 *
 *		Tally strike for network planes.
 *
 */
void NetGetStrikeSmall( DPMSG_STRIKE_SM *Net )
{
	// Assign credit for Strike

	if((Net->type == NET_BULLET_STRIKE_SM) && (Net->target == (PlayerPlane - Planes)))
	{
		int iLevel = g_iSoundLevelExternalSFX-10;
		if (iLevel<0) iLevel=0;
		if (iLevel)
		{
			SndQueueSound(Implgin2,1,iLevel,-1,-1,64);
		}
	}

	if(Net->slot == 255)
	{
		NetSetKill( -1, Net->target );
	}
	else
	{
		NetSetKill( Net->slot, Net->target );
	}
}


/*----------------------------------------------------------------------------
 *
 *	NetPutDamageSmall()
 *
 *		Intercept DestroyPlaneItem().
 *
 */
void NetPutDamageSmall( PlaneParams *P, long itemid, int critical )
{
	DPMSG_DAMAGE_SM  NetDamageSmall;

	if (netCall)  return;	// don't recurse RPC

	// Assign credit for strike

	//  See note in NetPutDamage
//	NetSetKill( MySlot, P-Planes, 1 );	// F15v102f compatibility

	// RPC: Remote::DestroyPlaneItem()

	NetDamageSmall.type	 = NET_DAMAGE_SM;
	NetDamageSmall.slot	 = PlayerPlane - Planes;
	NetDamageSmall.target = P - Planes;
	NetDamageSmall.system = itemid;
	NetDamageSmall.status = critical;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetDamageSmall, sizeof(NetDamageSmall) );
	NetAddToBigPacket(MySlot, &NetDamageSmall, sizeof(NetDamageSmall) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDamageSmall()
 *
 *		Inflict damage to network planes.
 *
 */
void NetGetDamageSmall( DPMSG_DAMAGE_SM *Net )
{
	// Assign credit for Strike

	//  See note in NetPutDamage
//	NetSetKill( Net->slot, Net->target, 1 );	// F15v102f compatibility

	netCall = 1;	// prevent recursion for remote procedure calls

	DestroyPlaneItem( &Planes[Net->target], Net->system, Net->status );

	netCall = 0;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutCrashSmall()  --  *** This version only used for CRASHes ***
 *
 *		Intercept CrashPlane().
 *
 */
int NetPutCrashSmall( PlaneParams *P, int flight_status, int failed_system )
{
	DPMSG_CRASH_SM  NetCrashSmall;

	if (netCall)  return TRUE;	// don't recurse RPC

	 // Allow DEATH SPIRAL to crash locally.
	//

	if (P->Status & PL_AI_DRIVEN  )
	{
		if(P == PlayerPlane)
		{
			NetLogKill( PlayerPlane - Planes );
		}
		return TRUE;
	}

	 // Don't crash network planes.  Let the owner do it.
	//
	if (P->Status & PL_COMM_DRIVEN)  return FALSE;

	// I crashed, log the kill to the last one to hit me.
	NetLogKill( PlayerPlane - Planes );

	// RPC: Remote::CrashPlane()

	NetCrashSmall.type	= NET_CRASH_SM;
	NetCrashSmall.slot	= PlayerPlane - Planes;
	NetCrashSmall.system = failed_system;
	NetCrashSmall.status = flight_status;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetCrashSmall, sizeof(NetCrashSmall) );
	NetAddToBigPacket(MySlot, &NetCrashSmall, sizeof(NetCrashSmall) );

	return TRUE;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetCrashSmall()
 *
 *		Inflict Crash to network planes.
 *
 */
void NetGetCrashSmall( DPMSG_CRASH_SM *Net )
{
	// Log the kill
	NetLogKill( Net->slot );

	netCall = 1;	// prevent recursion for remote procedure calls

	CrashPlane( &Planes[Net->slot], Net->status, Net->system );

	if((Planes[Net->slot].Status & PL_COMM_DRIVEN) && (Planes[Net->slot].FlightStatus & PL_STATUS_CRASHED))
	{
		Planes[Net->slot].Status &= ~(PL_COMM_DRIVEN);
		Planes[Net->slot].Status |= (PL_AI_DRIVEN);
	}

	netCall = 0;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDecoySmall()
 *
 *		Tell the world that we fired a Decoy.
 *
 *		Hook "InstantiateCounterMeasure()" and send a guaranteed message.
 *
 */
int NetPutDecoySmall( PlaneParams *P, int cmtype )
{
	DPMSG_DECOY_SM  NetDecoySmall;

	NetDecoySmall.type   = NET_DECOY_SM;
	NetDecoySmall.slot	= P - Planes;
	NetDecoySmall.cmtype = cmtype;
	NetDecoySmall.weapIX = ++NetWeapIX;	// assign next GUID

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetDecoySmall, sizeof(NetDecoySmall) );
	NetAddToBigPacket(MySlot, &NetDecoySmall, sizeof(NetDecoySmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDecoySmall()
 *
 *		Fire a Decoy for a remote player.
 *
 */
void NetGetDecoySmall( DPMSG_DECOY_SM *Net )
{
	PlaneParams *P = &Planes[Net->slot];

	if(Net->cmtype >= 16)
	{
		InstantiateCounterMeasure( P, (int)(Net->cmtype - 16), -1, Net->weapIX );
	}
	else
	{
		InstantiateCounterMeasure( P, CHAFF_FLARE, Net->cmtype, Net->weapIX );
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutEjectSmall()
 *
 *		Tell the world that we Ejected.
 *
 *		Hook "InstantiateEjectionSeat()" and send a guaranteed message.
 *
 */
int NetPutEjectSmall( PlaneParams *P )
{
	DPMSG_EJECT_SM  NetEjectSmall;

	NetEjectSmall.type   = NET_EJECT_SM;
	NetEjectSmall.slot	= P - Planes;
	NetEjectSmall.weapIX = ++NetWeapIX;	// assign next GUID

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetEjectSmall, sizeof(NetEjectSmall) );
	NetAddToBigPacket(MySlot, &NetEjectSmall, sizeof(NetEjectSmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetEjectSmall()
 *
 *		Eject a remote player.
 *
 */
void NetGetEjectSmall( DPMSG_EJECT_SM *Net )
{
	PlaneParams *P = &Planes[Net->slot];

	InstantiateEjectionSeat( P, 1, 0, NULL, Net->weapIX );
}

/*----------------------------------------------------------------------------
 *
 *	NetPutPlaneSmall()
 *
 *		Send my plane's data to other players.
 *
 *		OUT-OF-CONTROL planes become AI_DRIVEN and then
 *		perform the DeathSpiral.  Keep sending AI_DRIVEN
 *		packets since these are not guaranteed and we use
 *		the first received packet to log the kill.
 *
 */
void NetPutPlaneSmall()
{
	DPMSG_PLANEDATA_SM	NetPutSmall;

	PlaneParams *P = PlayerPlane;

	NetPutSmall.type = NET_PLANEDATA_SM;
	NetPutSmall.time = newTime;

	NetPutSmall.Status    			= P->Status;
	NetPutSmall.Flaps	    		= P->Flaps;
	NetPutSmall.BitFlags				= 0;
	NetPutSmall.BitFlags				|= (P->Brakes) ? NET_BRAKES : 0;
	NetPutSmall.LandingGear			= P->LandingGear;

	NetPutSmall.StickX				= P->StickX;
	NetPutSmall.StickY				= P->StickY;
	NetPutSmall.BitFlags				|= (P->Trigger1) ? NET_AI_GUN : 0;
	NetPutSmall.BitFlags				|= (P->Trigger2) ? NET_TRIGGER2 : 0;

	NetPutSmall.BitFlags				|= (P->GunFireRate == 2) ? NET_GUN_RATE : 0;
	NetPutSmall.ThrottlePos	 		= P->ThrottlePos;
	NetPutSmall.RudderPedals			= P->RudderPedals;
	NetPutSmall.BitFlags				|= (P->SpeedBrakeState) ? NET_SBRAKE : 0;

	NetPutSmall.TotalWeight			= P->TotalWeight;

	NetPutSmall.BfLinVelX			= P->BfLinVel.X;
	NetPutSmall.BitFlags				|= (P->OnGround) ? NET_ON_GROUND : 0;

	NetPutSmall.BitFlags				|= (P->AI.iAIFlags1 & AIJAMMINGON) ? NET_JAMMER : 0;
	NetPutSmall.BitFlags			|= (IsDecoyJamming()) ? NET_TOWED_JAMMING : 0;
	NetPutSmall.BitFlags			|= (P->FlightStatus & PL_OUT_OF_CONTROL) ? NET_OUT_OF_CONTROL : 0;

	NetPutSmall.BitFlags			|= (PlayerPlane->TailHookState) ? NET_HOOK : 0;

	NetPutSmall.BitFlags			|= (PlayerPlane->Status & (NET_RUNNING_LIGHTS|NET_FORMATION_LIGHTS));

	NetPutSmall.WorldPosition		= P->WorldPosition;

	P->DoAttitude( P );	// make sure RPH are current

	NetPutSmall.Roll		  		= P->Roll;
	NetPutSmall.Pitch				= P->Pitch;
	NetPutSmall.Heading				= P->Heading;

	//  Added for Patch
	NetPutSmall.SymetricalElevator	= P->SymetricalElevator;

/*	NetPutSmall.Orientation.I		= P->Orientation.I;
	NetPutSmall.Orientation.J		= P->Orientation.J;
	NetPutSmall.Orientation.K		= P->Orientation.K;  */

	NetPutSmall.Radar = NetPutTEWS();

	for (int i=0; i<PLAYERS; i++)
	{
		int planex = NetGetPlaneIndexFromSlot(i);
		if(planex == -1) continue;
		if (i==MySlot || Slot[planex].dpid==0)  continue;

		if(netsendanyway <= 0)
		{
			if(iSlotToPlane[i] != -1)
			{
				if(!(Planes[iSlotToPlane[i]].Status & (PL_ACTIVE)))
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}

//		NetPutSmall.netDelta = Slot[i].netDelta;
		NetPutSmall.netDelta = Slot[planex].netDelta;
//		NetSend( Slot[i].dpid, 0, &NetPutSmall, sizeof(NetPutSmall) );
		NetAddToBigPacket(i, &NetPutSmall, sizeof(NetPutSmall) );
	}

	if(netsendanyway > 0)
	{
		netsendanyway --;
	}

	cSend++;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetPlaneSM()
 *
 *		Get a network plane's data.
 *
 *		Update all inputs and states and save position/time for later.
 *
 *
 *		Discard out-of-sequence packet.
 *
 *		N.B.  "if (this-last < 0)" is NOT the same as "if (this < last)"
 *
 *		The first emits "JS exit", the second emits "JB/JL exit"
 *
 *		Unsigned comparisons break if we bridge 0xFFFF.FFFF
 *  	  Signed comparisons break if we bridge 0x7FFF.FFFF
 *
 *		If time were infinite and hence monotonic this would be true,
 *  	but even 32 bits runs out eventually (49+ days).  So to cover
 *		the case of wrap around we intentionally do a difference and
 *  	then a signed comparison.  This treats the difference as a
 *		signed number (+/- 25 days) that is negative if the new packet
 *  	is older than the last one.
 *
 */
DPMSG_PLANEDATA_SM *NetGetSmall;
DPMSG_PLANEDATA_SM NetGetSmallArray[MAX_HUMANS];

void NetGetPlaneSmall(int slot, DPMSG_PLANEDATA_SM *Net )
{
	int slotnum;
	NetGetSmall = Net;
	int ignoreOOC = 0;

	int planenum = slot;

	NetSlot     *S = &Slot[planenum];

	slotnum = NetGetSlotFromPlaneIndex(planenum);
	if(slotnum != -1)
	{
		if(iSlotToPlane[slotnum] == -1)
		{
			iSlotToPlane[slotnum] = planenum;
		}
	}

	if((slotnum >= 0) && (slotnum < MAX_HUMANS))
	{
		NetGetSmallArray[slotnum] = *Net;
	}

	PlaneParams *P = &Planes[planenum];

	if((P->Status & PL_AI_DRIVEN) && (P->AI.Behaviorfunc == CTWaitingForLaunch))
	{
		return;
	}

	// First alert of death spiral; ie. AI this frame but not last
#if 0
	if (((P->Status & PL_AI_DRIVEN) && (P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	&& !(Net->Status & PL_AI_DRIVEN) )
	{
		if(!(P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			NetLogKill( P-Planes );

		if((slotnum >= 0) && (slotnum < MAX_HUMANS))
			NetSetPlane(slotnum);
	}
#endif

	Net->time += S->netDelta;

	// Discard out-of-sequence packet.
	if ( (Net->time - S->netTime) < 0)  return;

	// Save most recent Wpos for next "MovePlane"
	S->netTime = Net->time;
	S->netWPos = Net->WorldPosition;

	// Activate the plane at first update.
	if (S->nPacks++ == 0)
	{
		if(slotnum != -1)
		{
			NetSetPlane( slotnum );
		}
		P->WorldPosition = Net->WorldPosition;
		ignoreOOC = 1;
	}

	// AI driven now, no need for data update.

	// First alert of death spiral  I THINK THIS IS CAUSING BIG PROBLEMS WITH CARRIER STUFF.
	//  Follow up.  I think we'll be OK if we check for out of control or crashed.
	//  Otherwise we are never setting comm planes to AIs.
	if((Net->Status & AL_AI_DRIVEN) && (!(P->Status & AL_AI_DRIVEN)) && (P->FlightStatus & (PL_STATUS_CRASHED|PL_OUT_OF_CONTROL)))  NetSetDeath( P );

	if (P->Status & AL_AI_DRIVEN)  return;

	// Update GearDown bit
	P->FlightStatus		   &= ~PL_GEAR_DOWN_LOCKED;
	if (Net->LandingGear == 255)
	P->FlightStatus		   |=  PL_GEAR_DOWN_LOCKED;

	P->Flaps				= Net->Flaps;
	P->Brakes				= (Net->BitFlags & NET_BRAKES) ? 1: 0;
	P->LandingGear			= Net->LandingGear;

	P->StickX				= Net->StickX;
	P->StickY				= Net->StickY;
	P->Trigger1				= (Net->BitFlags & NET_AI_GUN) ? 1: 0;
//	P->Trigger2				= Net->Trigger2;

	P->GunFireRate			= (Net->BitFlags & NET_GUN_RATE) ? 2: 1;
	P->ThrottlePos			= Net->ThrottlePos;
	P->LeftThrottlePos		= Net->ThrottlePos;
	P->RightThrottlePos		= Net->ThrottlePos;
	P->RudderPedals			= Net->RudderPedals;
	P->SpeedBrakeState		= (Net->BitFlags & NET_SBRAKE) ? 1: 0;

	if(Net->BitFlags & NET_JAMMER)
	{
		P->AI.iAIFlags1 |= AIJAMMINGON;
	}
	else
	{
		P->AI.iAIFlags1 &= ~(AIJAMMINGON);
	}

	if(Net->BitFlags & NET_TOWED_JAMMING)
	{
		if(!(P->AI.iAICombatFlags1 & AI_MULTI_TOWED_DECOY))
		{
			DeployTowedDecoy(P);
		}

		P->AI.iAICombatFlags1 |= AI_MULTI_TOWED_DECOY;
	}
	else
	{
		if(P->AI.iAICombatFlags1 & AI_MULTI_TOWED_DECOY)
		{
			RetractTowedDecoy(P);
		}
		P->AI.iAICombatFlags1 &= ~(AI_MULTI_TOWED_DECOY);
	}

	if((Net->BitFlags & NET_OUT_OF_CONTROL) && (Net->Status & PL_AI_DRIVEN) && (!(P->Status & PL_AI_DRIVEN)) && (ignoreOOC == 0))
	{
		P->FlightStatus |= PL_OUT_OF_CONTROL;
	}

	if(Net->BitFlags & NET_HOOK)
	{
		if(P->TailHookState == 0)
		{
			P->TailHookState = 1;
			P->TailHookCommandedPos = 90.0f;
		}
	}
	else
	{
		if(P->TailHookState == 1)
		{
			P->TailHookState = 0;
			P->TailHookCommandedPos = 0.0f;
		}
	}

	P->Status &= ~(PL_RUNNING_LIGHTS|PL_FORMATION_LIGHTS);
	P->Status |= (Net->BitFlags & (NET_RUNNING_LIGHTS|NET_FORMATION_LIGHTS));

//	P->InternalFuel			= Net->InternalFuel;
//	P->WingDropFuel			= Net->WingDropFuel;
//	P->CenterDropFuel		= Net->CenterDropFuel;
//	P->AircraftDryWeight	= Net->AircraftDryWeight;

//	P->TotalFuel			= Net->TotalFuel;
//	P->TotalMass			= Net->TotalMass;
	P->TotalWeight			= Net->TotalWeight;
	P->TotalFuel			= P->TotalWeight - P->AircraftDryWeight;

	P->BfLinVel.X			= Net->BfLinVelX;

//	P->HeightAboveGround	= Net->HeightAboveGround;

/*	P->Orientation.I		= Net->Orientation.I;
	P->Orientation.J		= Net->Orientation.J;
	P->Orientation.K		= Net->Orientation.K;  */

	P->Roll					= Net->Roll;
	P->Pitch				= Net->Pitch;
	P->Heading				= Net->Heading;

	P->SymetricalElevator	= Net->SymetricalElevator;

	void RPH_to_Orientation( PlaneParams *P );
	RPH_to_Orientation( P );

	NetGetTEWS( P, Net->Radar );

	S->tPing = abs( S->netDelta + Net->netDelta );
}

/*----------------------------------------------------------------------------
 *
 *	NetPutChatSmall()
 *
 *		Send a chat message to selected player(s).
 *
 */
void NetPutChatSmall( int dpid, char *chat )
{
	DPMSG_CHATTER_SM	NetChatSmall;

	NetChatSmall.type = NET_CHATTER_SM;
	NetChatSmall.slot = PlayerPlane - Planes;
	NetChatSmall.chatlen = strlen(chat);
	strncpy( NetChatSmall.chat, chat, sizeof(NetChatSmall.chat)-1 );

	NetSend( dpid, 0, &NetChatSmall, sizeof(NetChatSmall) - sizeof(NetChatSmall.chat) + strlen(NetChatSmall.chat) + 1 );
#if 0
	for(int cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		NetAddToBigPacket(i, &NetEjectSmall, sizeof(NetEjectSmall) );
	}
#endif
}

/*----------------------------------------------------------------------------
 *
 *	NetGetChatSmall()
 *
 *		Display a new CHAT message.
 *
 */
void NetGetChatSmall( DPMSG_CHATTER_SM *Net )
{
	static char szMsg[MAX_PATH];
	char tempstr[256];

	memcpy(tempstr, Net->chat, Net->chatlen);
	tempstr[Net->chatlen] = 0;

	sprintf( szMsg,"%s: %s", Slot[Net->slot].name, Net->chat );
	AICAddAIRadioMsgs( szMsg, 50 );

	extern int g_iSoundLevelCaution;
	SndQueueSound( 51, 1, g_iSoundLevelCaution );	// "1000pair.wav"

	if (hWrap)  SendMessage( hWrap, CWM_MULTI_CHAT_UPDATE, (WPARAM)szMsg, 0 );
}

//============================================================================
//		Small AI Position MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutAIPosSmall()
 *
 *		Tell the world where an AI is.
 *
 */
void NetPutAIPosSmall(PlaneParams *P)
{
	double distance;
	PlaneParams *planepnt;
	DPMSG_AI_POS_SM NetAIPosSmall;
	int loopoffset;
	int nobehave = 0;

	if(!(P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		return;
	}

	loopoffset = P - Planes;
	NetAIPosSmall.type = NET_AI_POS_SM;
	NetAIPosSmall.time = newTime;

	NetAIPosSmall.aislot = P - Planes;		// ai plane slot #
	NetAIPosSmall.fpWorldPosition = P->WorldPosition;  //  ai's position in the world
	NetAIPosSmall.Heading = P->Heading;	//  ai's new heading
	NetAIPosSmall.Pitch = P->Pitch;		//  ai's new pitch
	NetAIPosSmall.Roll = P->Roll;		//  ai's new roll
	NetAIPosSmall.V = P->V;			//  ai's velocity in ft/sec
	NetAIPosSmall.DesiredPitch = P->DesiredPitch;		//  ai's desired pitch
	NetAIPosSmall.DesiredRoll = P->DesiredRoll;		//  ai's desired roll
	NetAIPosSmall.DesiredSpeed = P->DesiredSpeed;		//  ai's desired speed in ft/sec

	NetAIPosSmall.lFlags1 = 0;
	if(P->SpeedBrakeState)
	{
		NetAIPosSmall.lFlags1 |= NET_SBRAKE;
	}
	else
	{
		NetAIPosSmall.lFlags1 &= ~NET_SBRAKE;
	}

	if(P->FlightStatus & PL_GEAR_DOWN_LOCKED)
	{
		NetAIPosSmall.lFlags1 |= NET_GEAR;
	}
	else
	{
		NetAIPosSmall.lFlags1 &= ~NET_GEAR;
	}

	if(P->Flaps == 0)
	{
		NetAIPosSmall.lFlags1 &= ~NET_FLAPS;
	}
	else
	{
		NetAIPosSmall.lFlags1 |= NET_FLAPS;
	}

	if(P->AI.iAICombatFlags1 & AI_MULTI_FIRE_GUN)
	{
		NetAIPosSmall.lFlags1 |= NET_AI_GUN;
		P->AI.iAICombatFlags1 &= ~AI_MULTI_FIRE_GUN;
	}
	else
	{
		NetAIPosSmall.lFlags1 &= ~NET_AI_GUN;
	}

	long ltempflags = P->AI.cMultiCheckFlags;
	NetAIPosSmall.lFlags1 |= (ltempflags << MC_SHIFT);

	if(P->AI.AirTarget)
	{
		NetAIPosSmall.iAirTarget = P->AI.AirTarget - Planes;
	}
	else
	{
		NetAIPosSmall.iAirTarget = 255;
	}

	if(P->AI.AirThreat)
	{
		NetAIPosSmall.iAirThreat = P->AI.AirThreat - Planes;
	}
	else
	{
		NetAIPosSmall.iAirThreat = 255;
	}

	NetAIPosSmall.iwaypoint = P->AI.CurrWay - AIWayPoints;

//   	NetSend( DPID_ALLPLAYERS, 0, &NetAIPos, sizeof(NetAIPos) );
//	(!(GameLoop % 10))

	int X, Y;

	MAIGetHotZoneIndex((float)P->WorldPosition.X, (float)P->WorldPosition.Z, &X, &Y);
	int sendto = (MAICheckHotZones(X, Y));

	for (int i=0; i<PLAYERS; i++)
	{
		int planex = NetGetPlaneIndexFromSlot(i);
		if(planex == -1) continue;
		if (i==MySlot || Slot[planex].dpid==0)  continue;

		if(iSlotToPlane[i] != -1)
		{
			if(!(Planes[iSlotToPlane[i]].Status & (PL_ACTIVE)))
			{
				continue;
			}
		}
		else
		{
			continue;
		}

		planepnt = &Planes[i];

		//  Since nobehave doesn't get set by anything I think this was during early prototype stage
		//  This will cancel out a number of these if statements.
		if((P->AI.iMultiPlayerDist[i] < 20) && (nobehave))  //  5 updates/second
		{
			distance = P->WorldPosition - planepnt->WorldPosition;
			distance *= WUTONM;
			P->AI.iMultiPlayerDist[i] = distance;

//		   	NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
		}
		else if((P->AI.iMultiPlayerDist[i] < 3) && (P->AI.iSide != Planes[iSlotToPlane[i]].AI.iSide))
		{
			if(!((GameLoop + loopoffset + (loopoffset * 50)) % 50))  //  1 update/second
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

//		   		NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
				NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			}
		}
		else if((P->AI.iMultiPlayerDist[i] < 6) && ((P->AI.Behaviorfunc == AITankingFlight) || (P->AI.Behaviorfunc == AITankingFlight2AI) || (P->AI.Behaviorfunc == AITankingFlight3AI)))
		{
			//  This updates the tanker position more often when close to another player.
			if(!((GameLoop + loopoffset + (loopoffset * 50)) % 250))  //  1 update/5 seconds  was 10
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

//		   		NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
				NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			}
		}
//		else if((P->AI.iMultiPlayerDist[i] < 40) && (nobehave))
		else if(((P->AI.iMultiPlayerDist[i] < 40) || (sendto & (1<<i))) && ((nobehave) || (P->AI.iAIFlags2 & AIMISSILEDODGE) || (P->AI.CombatBehavior)))
		{
			if(!((GameLoop + loopoffset + (loopoffset * 50)) % 100))  //  1 update/2 second
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

//		   		NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
				NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			}
		}
//		else if((P->AI.iMultiPlayerDist[i] < 80) && ((nobehave) || (P->AI.iAIFlags2 & AI_IN_DOGFIGHT)))
		else if(((P->AI.iMultiPlayerDist[i] < 80) || (sendto & (1<<i))) && ((nobehave) || (P->AI.iAIFlags2 & AI_IN_DOGFIGHT)))
		{
			if(!((GameLoop + loopoffset + (loopoffset * 50)) % 250))  //  1 update/5 seconds  was 10
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

//		   		NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
				NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			}
		}
		else if(((P->AI.iMultiPlayerDist[i] < 120) && (nobehave)) || (sendto & (1<<i)))
		{
			if(!((GameLoop + loopoffset + (loopoffset * 50)) % 1500))  //  1 update/30 seconds
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

//		   		NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
				NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			}
		}
		else
		{
			if(!((GameLoop + loopoffset + (loopoffset * 50)) % 3000))  //  1 update/1 minute
			{
				distance = P->WorldPosition - planepnt->WorldPosition;
				distance *= WUTONM;
				P->AI.iMultiPlayerDist[i] = distance;

//		   		NetSend( Slot[i].dpid, 0, &NetAIPosSmall, sizeof(NetAIPosSmall) );
				NetAddToBigPacket(i, &NetAIPosSmall, sizeof(NetAIPosSmall) );
			}
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetGetAIPosSmall()
 *
 *		Update an AI's World Pos.
 *
 */
void NetGetAIPosSmall(int fromslot, DPMSG_AI_POS_SM *Net )
{
	PlaneParams *P;
	int update = 1;

	NetSlot *S = &Slot[Net->aislot];

	if(Planes[Net->aislot].AI.iAICombatFlags1 & AI_MULTI_ACTIVE)
	{
		return;
	}

	if((Net->time - S->netTime) < 0)	return;

#ifdef _DEBUG
	char tempstr[256];
	sprintf(tempstr, "Got %d, at %d, feet %f", Net->aislot, Net->time, ((Planes[Net->aislot].WorldPosition - Net->fpWorldPosition) * WUTOFT));
	AICAddAIRadioMsgs(tempstr, 50);
#endif

	//  Save most recent Wpos for next "MovePlane"
	S->netTime = Net->time;
	S->netWPos = Net->fpWorldPosition;

	P = &Planes[Net->aislot];		// ai plane slot #
	P->AI.iMultiPlayerAIController = fromslot;

#if 0
	if(S->nPacks++ == 0)
	{
//		NetSetPlane(Net->aislot);
		P->WorldPosition = Net->fpdWorldPosition;
	}
#else

	if(S->nPacks++ == 0)
	{
		P->WorldPosition = Net->fpWorldPosition;
	}
	else if(((P->WorldPosition - Net->fpWorldPosition) * WUTOFT) > 3000.0f)
	{
		P->WorldPosition = Net->fpWorldPosition;
	}
	else
	{
		FPointDouble  nuPos;	// WPos interpolated at Tn

#if 0	//  Since we are doing this as soon as we get this I don't think we need to do this here
		// move oldWPos from T0 to Tn
		nuPos  = P->WorldPosition;
		nuPos -= S->oldWPos;
		nuPos *= dt;
		nuPos += S->oldWPos;	// nuPos = our value interpolated at Tn
#else
		nuPos  = P->WorldPosition;
#endif

		S->netWPos -= nuPos;	// error term: actual minus physics
		S->netDiff  = S->netWPos.QuickLength();
		S->netTime  = 0;		// mark net input as consumed

		S->netWPos *= netKnob;			// scale down error term for feedback
		if((QuickDistance3D(S->netWPos.X, S->netWPos.Y , S->netWPos.Z) * WUTOFT) > 75.0f)
		{
			P->WorldPosition += S->netWPos;	// add in feedback
		}
		else
		{
			update = 0;
		}
	}

//	P->WorldPosition = Net->fpdWorldPosition;
#endif

	Net->type = NET_AI_POS;
	Net->time = newTime;
//	Net->slot = MySlot;

//	P->WorldPosition = Net->fpdWorldPosition;  //  ai's position in the world
	if(update)
	{
		P->Heading = Net->Heading;	//  ai's new heading
		P->Pitch = Net->Pitch;		//  ai's new pitch
		P->Roll = Net->Roll;		//  ai's new roll
	}
	P->V = Net->V;			//  ai's velocity in ft/sec
	P->DesiredPitch = Net->DesiredPitch;		//  ai's desired pitch
	P->DesiredRoll = Net->DesiredRoll; 		//  ai's desired roll
	P->DesiredSpeed = Net->DesiredSpeed;		//  ai's desired speed in ft/sec
	//  lFlags stuff.
	if(Net->lFlags1 & NET_SBRAKE)
	{
		P->SpeedBrakeState = 1;
	}
	else
	{
		P->SpeedBrakeState = 0;
	}

	if(Net->lFlags1 & NET_GEAR)
	{
		if(!(P->FlightStatus & PL_GEAR_DOWN_LOCKED))
		{
			RaiseLowerLandingGear(P,LOWER_LANDING_GEAR);
		}
	}
	else
	{
		if(P->FlightStatus & PL_GEAR_DOWN_LOCKED)
		{
			RaiseLowerLandingGear(P,RAISE_LANDING_GEAR);
		}
	}

	if(Net->lFlags1 & NET_FLAPS)
	{
		P->Flaps = 30.0f;
		P->FlapsCommandedPos = 30.0f;
	}
	else
	{
		P->Flaps = 0;
		P->FlapsCommandedPos = 0;
	}

	if(Net->lFlags1 & NET_AI_GUN)
	{
		P->AI.iAICombatFlags1 |= AI_MULTI_FIRE_GUN;
	}
	else
	{
		P->AI.iAICombatFlags1 &= ~AI_MULTI_FIRE_GUN;
	}

	long ltempflags = (Net->lFlags1 & MC_lF1_MASK)>> MC_SHIFT;
	DPMSG_GENERIC_2_SM NetGSm;
	NetGSm.arg1 = GM2_MULTI_FLAGS;
	NetGSm.arg2 = (BYTE)ltempflags;
	NetGSm.slot = P - Planes;
	NetGetGenericMessage2Small(&NetGSm);

	if(Net->iAirTarget != 255)
	{
		P->AI.AirTarget = &Planes[Net->iAirTarget];
	}
	else
	{
		P->AI.AirTarget = NULL;
	}
	if(Net->iAirThreat != 255)
	{
		P->AI.AirThreat = &Planes[Net->iAirThreat];
	}
	else
	{
		P->AI.AirThreat = NULL;
	}


	RPH_to_Orientation(P);

	S->oldWPos = P->WorldPosition;	// save time/place for next bump

	if(Net->iwaypoint != (P->AI.CurrWay - AIWayPoints))
	{
		MAISetToNewWaypoint(P, Net->iwaypoint);
	}
}

//============================================================================
//		Small GENERAL MISSILE MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutMissileSmall()
 *
 *		Tell the world that we fired a missile.
 *
 *		Hook "InstantiateMissile()" and send a guaranteed message.
 *
 */
int NetPutMissileGeneralSmall( PlaneParams *P, int Station, void *GroundLauncher, int sitetype, PlaneParams *pTarget)
{
	BasicInstance  *currweapon;
	DPMSG_MISSILE_GENERAL_SM NetMissileGeneralSmall;

	NetMissileGeneralSmall.type   = NET_MISSILE_GENERAL_SM;

	if(P)
	{
		NetMissileGeneralSmall.slot	  = P - Planes;
	}
	else
	{
		NetMissileGeneralSmall.slot	  = 255;
	}

	NetMissileGeneralSmall.target = pTarget ? (pTarget - Planes) : 255;
	NetMissileGeneralSmall.weapon = Station;
	NetMissileGeneralSmall.weapIX = ++NetWeapIX;	// assign next GUID

	if(sitetype == MOVINGVEHICLE)
	{
		NetMissileGeneralSmall.dwGrndSerialNum = (MovingVehicleParams *)GroundLauncher - MovingVehicles;
#if 0
		char netstr[80];
		sprintf(netstr, "PUT missile from SN %ld, at %d", NetMissileGeneralSmall.dwGrndSerialNum, NetMissileGeneralSmall.target);
		AICAddAIRadioMsgs(netstr, 50);
#endif
	}
	else if(GroundLauncher)
	{
		currweapon  = (BasicInstance *)GroundLauncher;
		NetMissileGeneralSmall.dwGrndSerialNum = currweapon->SerialNumber;

#if 0
		char netstr[80];
//		AAWeaponInstance *launcher;
//		launcher = (AAWeaponInstance *)GroundLauncher;
//		sprintf(netstr, "PUT missile from %s SN %ld, at %d", launcher->Physicals.Object->Filename, currweapon->SerialNumber, NetMissileGeneralSmall.target);
		sprintf(netstr, "PUT missile from SN %ld, at %d", NetMissileGeneralSmall.dwGrndSerialNum, NetMissileGeneralSmall.target);
		AICAddAIRadioMsgs(netstr, 50);
#endif
	}
	else
	{
		NetMissileGeneralSmall.dwGrndSerialNum = 0;
	}

	NetMissileGeneralSmall.sitetype = sitetype;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetMissileGeneralSmall, sizeof(NetMissileGeneralSmall) );
	NetAddToBigPacket(MySlot, &NetMissileGeneralSmall, sizeof(NetMissileGeneralSmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMissileGeneralSmall()
 *
 *		Fire a missile for an AI
 *
 */
void NetGetMissileGeneralSmall( DPMSG_MISSILE_GENERAL_SM *Net )
{
	PlaneParams *P;
	PlaneParams *Target;
	BasicInstance  *walker;
	MovingVehicleParams *vehiclepnt;
	void *GroundLauncher = NULL;
	int radardatnum = 0;

	Target = (Net->target == 255) ? NULL : &Planes[Net->target];
	if(Net->slot != 255)
	{
		P = &Planes[Net->slot];
		P->AADesignate = Target;
		if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
		{
			if(P->WeapLoad[Net->weapon & ~(0x60)].Count > 0)
			{
				P->WeapLoad[Net->weapon & ~(0x60)].Count--;
			}
		}
	}
	else
	{
		P = NULL;
	}

	if(Net->sitetype == MOVINGVEHICLE)
	{
		if(Net->dwGrndSerialNum != 255)
		{
			vehiclepnt = &MovingVehicles[Net->dwGrndSerialNum];
			if(vehiclepnt->bDamageLevel >= 2)
			{
				NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
			}
			GroundLauncher = vehiclepnt;
			radardatnum = Net->weapon;
		}
		else
		{
			GroundLauncher = NULL;
		}
	}
	else if(Net->dwGrndSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);
		if(walker)
		{
			GDRadarData *tempradardat = GDGetRadarData(walker);
			if((MultiPlayer) && tempradardat && (tempradardat->lRFlags1 & GD_I_AM_DEAD))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_DEAD, GROUNDOBJECT, Net->dwGrndSerialNum);
			}
		}

		GroundLauncher = walker;
	}
	else
	{
		GroundLauncher = NULL;
	}

	if(!(P || GroundLauncher))
	{
		return;
	}

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)walker;
	sprintf(netstr, "GET missile from %s SN %ld, at %d", launcher->Physicals.Object->Filename, walker->SerialNumber, Net->target);
	AICAddAIRadioMsgs(netstr, 50);
#endif

	InstantiateMissile( P, Net->weapon, GroundLauncher, Net->sitetype, Target, Net->weapIX, radardatnum);
}

//============================================================================
//		Small GENERAL GROUND RADAR LOCKUP MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutGroundLockSmall(BasicInstance *foundweapon, BasicInstance *radarsite, PlaneParams *planepnt)
 *
 *		Sets things up in MultiPlayer so that TEWS will show the correct lock information on all systems.
 *
 */
void NetPutGroundLockSmall(BasicInstance *foundweapon, BasicInstance *radarsite, PlaneParams *planepnt)
{
	DPMSG_GROUND_LOCK_SM	NetGroundLockSmall;

	NetGroundLockSmall.type   = NET_GROUND_LOCK_SM;

	NetGroundLockSmall.slot	  = PlayerPlane - Planes;

	NetGroundLockSmall.target = planepnt ? (planepnt - Planes) : 255;

	if(foundweapon)
	{
		NetGroundLockSmall.dwWeaponSerialNum = foundweapon->SerialNumber;
	}
	else
	{
		NetGroundLockSmall.dwWeaponSerialNum = 0;
	}

	if(radarsite)
	{
		NetGroundLockSmall.dwRadarSerialNum = radarsite->SerialNumber;
	}
	else
	{
		NetGroundLockSmall.dwRadarSerialNum = 0;
	}

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)foundweapon;
	sprintf(netstr, "PUT lock from %s SN %ld radar %ld, on %d", launcher->Physicals.Object->Filename, NetGroundLockSmall.dwWeaponSerialNum, NetGroundLockSmall.dwRadarSerialNum, NetGroundLockSmall.target);
	AICAddAIRadioMsgs(netstr, 50);
#endif

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetGroundLockSmall, sizeof(NetGroundLockSmall) );
	NetAddToBigPacket(MySlot, &NetGroundLockSmall, sizeof(NetGroundLockSmall) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGroundLockSmall()
 *
 *		Lock up a plane with a ground radar
 *
 */
void NetGetGroundLockSmall( DPMSG_GROUND_LOCK_SM *Net )
{
	PlaneParams *planepnt;
	BasicInstance  *walker;
	BasicInstance  *radarsite;
	GDRadarData *radardat;
	int notradar;
	int isaaa;
	AAWeaponInstance *foundweapon;
	float currdist;
	DBWeaponType *weapon;
	int debugnum = 0;

	planepnt = (Net->target == 255) ? NULL : &Planes[Net->target];

	if(Net->dwWeaponSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwWeaponSerialNum);
		if(walker)
		{
			GDRadarData *tempradardat = GDGetRadarData(walker);
			if((MultiPlayer) && tempradardat && (tempradardat->lRFlags1 & GD_I_AM_DEAD))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_DEAD, GROUNDOBJECT, Net->dwWeaponSerialNum);
			}
		}
	}
	else
	{
		walker = NULL;
	}

	if(Net->dwRadarSerialNum == Net->dwWeaponSerialNum)
	{
		radarsite = walker;
	}
	else if(Net->dwRadarSerialNum)
	{
		radarsite = FindInstance(AllInstances, Net->dwRadarSerialNum);
		if(radarsite)
		{
			GDRadarData *tempradardat = GDGetRadarData(radarsite);
			if((MultiPlayer) && tempradardat && (tempradardat->lRFlags1 & GD_I_AM_DEAD))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_DEAD, GROUNDOBJECT, Net->dwRadarSerialNum);
			}
		}
	}
	else
	{
		radarsite = NULL;
	}

	if(!(planepnt && walker))
	{
		walker = FindInstance(AllInstances, Net->dwWeaponSerialNum);

		foundweapon = AllAAWeapons;
		walker = (BasicInstance *)foundweapon;

		while (foundweapon && (walker->SerialNumber != Net->dwWeaponSerialNum))
		{
			foundweapon = (AAWeaponInstance *)foundweapon->Basics.NextRelatedInstance;
			if(foundweapon)
			{
				walker = (BasicInstance *)foundweapon;
			}
		}
		return;
	}

	if(radarsite)
	{
		GDUpgradeTreeAlert(GDGetInfoLeader(radarsite), radarsite, planepnt->WorldPosition);
	}

	if(!iAAAFireAlreadyChecked)
		GDNoticeAAAFire(&walker->Position, GDConvertGrndSide(walker));

	currdist = planepnt->WorldPosition - walker->Position;

	foundweapon = (AAWeaponInstance *)walker;

	radardat = GDGetRadarData(walker);

	radardat->Target = planepnt;
	radardat->iTargetDist = currdist * WUTONM;

	weapon = GetWeaponPtr(walker);

	if(!weapon)
	{
		return;
	}

	if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10) || (weapon->iWeaponType == 6))
	{
		isaaa = 1;
		radardat->lWFlags1 &= ~GD_W_ACQUIRING;
	}
	else if((weapon->iSeekerType != 1) && (weapon->iSeekerType != 7))
	{
		notradar = 1;
		isaaa = 0;
	}
	else
	{
		notradar = 0;
		isaaa = 0;
	}

	if((!radarsite) || (radarsite == walker))
	{
		debugnum = 7;
		if(planepnt->AI.pPaintedBy == NULL)
		{
			planepnt->AI.pPaintedBy = walker;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = currdist * WUTONM;
		}
		else if(planepnt->AI.iPaintDist > (currdist * WUTONM))
		{
			planepnt->AI.pPaintedBy = walker;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = currdist * WUTONM;
		}
		if(weapon->iSeekerType == 1)
		{
			radardat->lRFlags1 |= GD_CHECK_MISSILES;
		}
		else if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			GDCheckForPingReport(walker, planepnt);
		}
		radardat->pWRadarSite = radarsite;
		if(isaaa)
		{
			debugnum = 8;
			GDFireInitialAAA(foundweapon, planepnt);
		}
	}
	else
	{
		radardat->pWRadarSite = radarsite;
		planepnt->AI.pPaintedBy = radarsite;
		planepnt->AI.iPaintedByType = GROUNDOBJECT;
		planepnt->AI.iPaintDist = (radarsite->Position - planepnt->WorldPosition) * WUTONM;
		radardat = GDGetRadarData(radarsite);
		debugnum = 1;
		if(weapon->iSeekerType == 1)
		{
			radardat->lRFlags1 |= GD_CHECK_MISSILES;
			radardat->Target = planepnt;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
			debugnum = 2;
		}
		else if(weapon->iSeekerType == 7)
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			radardat->Target = planepnt;
			GDCheckForPingReport(radarsite, planepnt);
			debugnum = 3;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
		}
		else if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10))  // Not sure about 10
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			radardat->Target = planepnt;
			GDCheckForPingReport(radarsite, planepnt);
			debugnum = 4;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
			if(isaaa)
			{
				debugnum = 5;
				GDFireInitialAAA(foundweapon, planepnt);
			}
		}
		else
		{
			if(isaaa)
			{
				debugnum = 6;
				GDFireInitialAAA(foundweapon, planepnt);
			}
		}
	}

	if(radarsite)
	{
		if(planepnt == PlayerPlane)
		{
			if(GetRadarPtr(radarsite))
			{
				radardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
			}
		}
	}

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)walker;
	sprintf(netstr, "GET lock from %s SN %ld radar %ld, on %d, DN %d", launcher->Physicals.Object->Filename, Net->dwWeaponSerialNum, Net->dwRadarSerialNum, Net->target, debugnum);
	AICAddAIRadioMsgs(netstr, 50);
#endif
}

//============================================================================
//		Small GENERAL BOMB MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutBombGeneralSmall()
 *
 *		Tell the world that we dropped a bomb.
 *
 *		Hook "InstantiateBomb()" and send a guaranteed message.
 *
 */
int NetPutBombGeneralSmall( PlaneParams *P, int Station , double burst_height, void *GroundTarget, int groundtype, double GX, double GY, double GZ)
{
	BasicInstance  *currweapon;
	MovingVehicleParams *vehiclepnt;
	DPMSG_BOMB_GENERAL_SM NetBombGeneralSmall;

	NetBombGeneralSmall.type   = NET_BOMB_GENERAL_SM;

	if(P)
	{
		NetBombGeneralSmall.slot	  = P - Planes;
	}
	else
	{
		NetBombGeneralSmall.slot	  = 255;
	}

	NetBombGeneralSmall.weapon = Station;
	NetBombGeneralSmall.weapIX = ++NetWeapIX;	// assign next GUID
	NetBombGeneralSmall.burst_height = burst_height;

	if(GroundTarget)
	{
		if(groundtype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)GroundTarget;
			NetBombGeneralSmall.dwGrndSerialNum = vehiclepnt - MovingVehicles;
		}
		else
		{
			currweapon  = (BasicInstance *)GroundTarget;
			NetBombGeneralSmall.dwGrndSerialNum = currweapon->SerialNumber;
		}
	}
	else
	{
		groundtype = NO_TARGET;
		NetBombGeneralSmall.dwGrndSerialNum = 0;
	}
	NetBombGeneralSmall.groundtype = groundtype;

	NetBombGeneralSmall.GX = GX;
	NetBombGeneralSmall.GY = GY;
	NetBombGeneralSmall.GZ = GZ;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetBombGeneralSmall, sizeof(NetBombGeneralSmall) );
	NetAddToBigPacket(MySlot, &NetBombGeneralSmall, sizeof(NetBombGeneralSmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetBombGeneralSmall()
 *
 *		Drop a bomb for an AI
 *
 */
void NetGetBombGeneralSmall( DPMSG_BOMB_GENERAL_SM *Net )
{
	PlaneParams *P;
	BasicInstance  *walker;

	if(Net->slot != 255)
	{
		P = &Planes[Net->slot];
		if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
		{
			if(P->WeapLoad[Net->weapon & ~(0x60)].Count > 0)
			{
				P->WeapLoad[Net->weapon & ~(0x60)].Count--;
			}
		}
	}
	else
	{
		P = NULL;
	}

	if(Net->dwGrndSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);
	}
	else
	{
		walker = NULL;
	}

	if(!(P || walker))
	{
		return;
	}

	InstantiateBomb( P, Net->weapon, Net->burst_height, walker, Net->groundtype, Net->GX, Net->GY, Net->GZ, Net->weapIX );
}

//============================================================================
//		Small GENERAL AG MISSILE MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutAGMissileGeneralSmall()
 *
 *		Tell the world that we dropped a bomb.
 *
 *		Hook "InstantiateAGMissile()" and send a guaranteed message.
 *
 */
int NetPutAGMissileGeneralSmall(PlaneParams *P, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile)
{
	BasicInstance  *currweapon;
	MovingVehicleParams *vehiclepnt;

	DPMSG_AG_MISSILE_GENERAL_SM NetAGMissileGeneralSmall;

	NetAGMissileGeneralSmall.type   = NET_AG_MISSILE_GENERAL_SM;

	if(P)
	{
		NetAGMissileGeneralSmall.slot	  = P - Planes;
	}
	else
	{
		NetAGMissileGeneralSmall.slot	  = 255;
	}

	NetAGMissileGeneralSmall.weapon = Station;
	NetAGMissileGeneralSmall.weapIX = ++NetWeapIX;	// assign next GUID

	if(!GroundTarget)
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneral.dwGrndSerialNum = 0;
	}
	else if(groundtype == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)GroundTarget;
		NetAGMissileGeneralSmall.dwGrndSerialNum = vehiclepnt - MovingVehicles;
	}
	else if(GroundTarget)
	{
		currweapon  = (BasicInstance *)GroundTarget;
		NetAGMissileGeneralSmall.dwGrndSerialNum = currweapon->SerialNumber;
	}
	else
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneralSmall.dwGrndSerialNum = 0;
	}
	NetAGMissileGeneralSmall.groundtype = groundtype;

	NetAGMissileGeneralSmall.targetloc = targetloc;
	NetAGMissileGeneralSmall.profile = flightprofile;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAGMissileGeneralSmall, sizeof(NetAGMissileGeneralSmall) );
	NetAddToBigPacket(MySlot, &NetAGMissileGeneralSmall, sizeof(NetAGMissileGeneralSmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetAGMissileGeneralSmall()
 *
 *		Launch an AG Missile
 *
 */
void NetGetAGMissileGeneralSmall( DPMSG_AG_MISSILE_GENERAL_SM *Net )
{
	PlaneParams *P;
	BasicInstance  *walker;
	MovingVehicleParams *vehiclepnt;
	void *GroundTarget;

	if(Net->groundtype == MOVINGVEHICLE)
	{
		if(Net->dwGrndSerialNum != 255)
		{
			vehiclepnt = &MovingVehicles[Net->dwGrndSerialNum];
			GroundTarget = vehiclepnt;
		}
		else
		{
			GroundTarget = NULL;
		}
	}
	else if(Net->dwGrndSerialNum)
	{
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);
		GroundTarget = walker;
	}
	else
	{
		GroundTarget = NULL;
	}

//*******  WHAT IN THE HELL WAS I THINKING??????
//	if(!GroundTarget)
//	{
//		return;
//	}

	if(Net->weapon & 0x20)
	{
		if(Net->weapon & 0x40)
		{
			if(Net->slot != 255)
			{
		 		vehiclepnt = &MovingVehicles[Net->slot];
				if(vehiclepnt->bDamageLevel >= 2)
				{
					NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
				}
			}
			else
			{
				return;
			}

			WCheckWeaponNowPlane(vehiclepnt, MOVINGVEHICLE, GroundTarget, Net->groundtype, Net->targetloc, Net->weapIX, (Net->weapon & ~(0x60)), Net->profile);
		}
		else
		{
			if(Net->slot != 255)
			{
				P = &Planes[Net->slot];
			}
			else
			{
				return;
			}

			WCheckWeaponNowPlane(P, AIRCRAFT, GroundTarget, Net->groundtype, Net->targetloc, Net->weapIX, (Net->weapon & ~(0x60)), Net->profile);
		}
	}
	else if(Net->weapon & 0x40)
	{
		if(Net->slot != 255)
		{
		 	vehiclepnt = &MovingVehicles[Net->slot];
			if(vehiclepnt->bDamageLevel >= 2)
			{
				NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
			}
		}
		else
		{
			return;
		}

		InstantiateSSMissile(vehiclepnt, MOVINGVEHICLE, GroundTarget, Net->groundtype, Net->targetloc, Net->weapIX, (Net->weapon & ~(0x40)), Net->profile);
	}
	else
	{
		if(Net->slot != 255)
		{
			P = &Planes[Net->slot];
			if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane) || (P->Status & PL_COMM_DRIVEN) || (P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
			{
				if(P->WeapLoad[Net->weapon & ~(0x60)].Count > 0)
				{
					P->WeapLoad[Net->weapon & ~(0x60)].Count--;
				}
			}
		}
		else
		{
			return;
		}

		InstantiateAGMissile( P, Net->weapon, GroundTarget, Net->targetloc, Net->groundtype, Net->weapIX, Net->profile);
	}
}

/*----------------------------------------------------------------------------
 *
 *	NoMultiCastBigPackets()
 *
 *		Only add to Big Packets that really need it (IE don't clog bandwidth)
 *
 */
void NoMultiCastBigPackets(void *lpMsg, int size)
{
	BYTE bpackettype;
	int sendto = 0;
	int cnt;
	int X, Y;
 	char *ppacket;

	ppacket = (char *)lpMsg;
	bpackettype = (BYTE)*ppacket;
	switch (bpackettype)
	{
		case NET_STRIKE_SM:
		case NET_BULLET_STRIKE_SM:
			DPMSG_STRIKE_SM *nstrike_sm;
			int src, trg;
			nstrike_sm = (DPMSG_STRIKE_SM *)lpMsg;
			src = nstrike_sm->slot;
			trg = nstrike_sm->target;

			if(Slot[trg].lastHit == src)
			{
				sendto |= 1<<src;
				sendto |= 1<<trg;
			}
			else
			{
				sendto = -1;
			}
			Slot[trg].lastHit = src;
			break;

		case NET_PLANEDATA_SM:
			MAIGetHotZoneIndex((float)PlayerPlane->WorldPosition.X, (float)PlayerPlane->WorldPosition.Z, &X, &Y);
			sendto = MAICheckHotZones(X, Y);
			break;
		case NET_AI_POS_SM:
			sendto = MAILimitAIPosSend((DPMSG_AI_POS_SM*)lpMsg);
			break;

		case NET_MISSILE_SM:
			sendto = MAILimitMissileSend((DPMSG_MISSILE_SM*)lpMsg);
			break;

		case NET_TARGET_SM:
			sendto = MAILimitTargetSend((DPMSG_TARGET_SM*)lpMsg);
			break;
		case NET_DECOY_SM:
			sendto = MAILimitDecoySend((DPMSG_DECOY_SM*)lpMsg);
			break;
		case NET_EJECT_SM:
			sendto = MAILimitEjectSend((DPMSG_EJECT_SM*)lpMsg);
			break;
		case NET_MISSILE_GENERAL_SM:
			sendto = MAILimitMissileGeneralSend((DPMSG_MISSILE_GENERAL_SM*)lpMsg);
			break;
		case NET_GROUND_LOCK_SM:
			sendto = MAILimitGroundLockSend((DPMSG_GROUND_LOCK_SM*)lpMsg);
			break;
		case NET_BOMB_GENERAL_SM:
			sendto = MAILimitBombGeneralSend((DPMSG_BOMB_GENERAL_SM*)lpMsg);
			break;
		case NET_AG_MISSILE_GENERAL_SM:
			sendto = MAILimitAGMissileGeneralSend((DPMSG_AG_MISSILE_GENERAL_SM*)lpMsg);
			break;
		case NET_DF_UPDATE_SM:
			DPMSG_DOGFIGHT_UPDATE_SM *NetUpdate;
			NetUpdate = (DPMSG_DOGFIGHT_UPDATE_SM*)lpMsg;
			MAIGetHotZoneIndex((float)Planes[NetUpdate->slot].WorldPosition.X, (float)Planes[NetUpdate->slot].WorldPosition.Z, &X, &Y);
			sendto = MAICheckHotZones(X, Y);
			break;
		case NET_DF_UPDATE_CPOS_SM:
			DPMSG_DOGFIGHT_UPDATE_CPOS_SM *NetCUpdate;
			NetCUpdate = (DPMSG_DOGFIGHT_UPDATE_CPOS_SM*)lpMsg;
			MAIGetHotZoneIndex((float)Planes[NetCUpdate->slot].WorldPosition.X, (float)Planes[NetCUpdate->slot].WorldPosition.Z, &X, &Y);
			sendto = MAICheckHotZones(X, Y);
			break;
		case NET_MISSILE_BREAK_SM:
			DPMSG_MISSILE_BREAK_SM *NetBreak;
			NetBreak = (DPMSG_MISSILE_BREAK_SM*)lpMsg;
			MAIGetHotZoneIndex((float)Planes[NetBreak->slot].WorldPosition.X, (float)Planes[NetBreak->slot].WorldPosition.Z, &X, &Y);
			sendto = MAICheckHotZones(X, Y);
			break;
		case NET_VGROUND_LOCK_SM:
			sendto = MAILimitVGroundLockSend((DPMSG_VGROUND_LOCK_SM*)lpMsg);
			break;

		case NET_CHATTER:
		case NET_CHATTER_SM:
		case NET_CRASH_SM:
		case NET_DAMAGE_SM:
		case NET_ZONES_SM:
		case NET_AI_COMMS:
		case NET_AI_COMMS_SM:
		case NET_AI_HEADING:
		case NET_AI_HEADING_SM:
		case NET_CAS_DATA:
		case NET_CAS_DATA_SM:
		case NET_GENERIC_1:
		case NET_GENERIC_1_SM:
		case NET_GENERIC_2:
		case NET_GENERIC_2_SM:
		case NET_GENERIC_3:
		case NET_GENERIC_3_SM:
		case NET_GENERIC_4:
		case NET_GENERIC_4_SM:
		case NET_WEAPON_G_EXPLODE:
		case NET_WEAPON_G_EXPLODE_SM:
		case NET_GENERIC_2_LONG:
		case NET_GENERIC_2_LONG_SM:
		case NET_GENERIC_3_LONG:
		case NET_GENERIC_3_LONG_SM:
		case NET_GENERIC_2_FPOINT:
		case NET_GENERIC_2_FPOINT_SM:
		case NET_GENERIC_OBJ_POINT:
		case NET_GENERIC_OBJ_POINT_SM:
		case NET_GENERIC_2_FLOAT:
		case NET_GENERIC_2_FLOAT_SM:
		case NET_GENERIC_3_FLOAT:
		case NET_GENERIC_3_FLOAT_SM:
		case NET_CHECK_DATA_HACK:		//  should not get here
		case NET_CHECK_DATA_HACK_SM:	//  should not get here
		case NET_GENERIC_4_LONG:
		case NET_GENERIC_4_LONG_SM:
//		case NET_GROUND_EXPLODE_SM:
			sendto = -1;
			break;

		default:
			sendto = 0;
			return;
			break;
	}

	if(((bpackettype != NET_CHATTER) && (bpackettype != NET_CHATTER_SM)) && (!iMultiCast))
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if(iSlotToPlane[cnt] == -1)
			{
				sendto &= ~(1<<cnt);
			}
			else
			{
				if(!(Planes[iSlotToPlane[cnt]].Status & (PL_ACTIVE)))
				{
					sendto &= ~(1<<cnt);
				}
			}
		}
	}
	else if(!iMultiCast)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if(Slot[cnt].dpid == 0)
			{
				sendto &= ~(1<<cnt);
			}
		}
	}

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if(cnt != MySlot)
		{
			if(sendto & (1<<cnt))
			{
				NetAddToBigPacket(cnt, lpMsg, size);
			}
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	MAIGetHotZoneIndex
 *
 *		Get the X,Y index position based off World Coordinates.
 *
 */
void MAIGetHotZoneIndex(float X, float Y, int *OutX, int *OutY)
{
	double dworkx, dworky;
	int workx, worky;

	dworkx = (double)X / dHotZoneDivideX;
	workx = (int)dworkx;
	if(workx < 0)
	{
		workx = 0;
	}
	else if(workx > iHotZoneMaxX)
	{
		workx = iHotZoneMaxX;
	}
	*OutX = workx;

	dworky = (double)Y / dHotZoneDivideY;
	worky = (int)dworky;
	if(worky < 0)
	{
		worky = 0;
	}
	else if(worky > iHotZoneMaxY)
	{
		worky = iHotZoneMaxY;
	}
	*OutY = worky;
}

/*----------------------------------------------------------------------------
 *
 *	MAICheckHotZones
 *
 *		Set big flags for Humans controlling things around Hot Zone
 *
 */
int MAICheckHotZones(int X, int Y)
{
	int cnt;
	int workval = 0;

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if(cnt != MySlot)
		{
			if(Planes[cnt].Status & PL_ACTIVE)
			{
				if(MAICheckAZone(X, Y, cnt))
				{
					workval |= (1<<cnt);
				}
			}
		}
	}

	return(workval);
}

#if 1
/*----------------------------------------------------------------------------
 *
 *	MAICheckAZone
 *
 *		Set up the bit flags to compare against Human controlled bit flags
 *
 */
int MAICheckAZone(int X, int Y, int slot)
{
	__int64 checkflags = 0;
	int workslot;

	workslot = X + (Y * (iHotZoneMaxX + 1));
	if(workslot < 0)
	{
		workslot = 0;
	}
	else if(workslot > 63)
	{
		workslot = 63;
	}
	checkflags |= ((__int64)1<<workslot);

	if(checkflags & tHotZones[slot].active_areas)
	{
		return(1);
	}

	return(0);
}

#else
/*----------------------------------------------------------------------------
 *
 *	MAICheckAZone
 *
 *		Set up the bit flags to compare against Human controlled bit flags
 *
 */
int MAICheckAZone(int X, int Y, int slot)
{
	__int64 checkflags = 0;
	int workslot;

	workslot = X + (Y * (iHotZoneMaxX + 1));
	if(workslot < 0)
	{
		workslot = 0;
	}
	else if(workslot > 63)
	{
		workslot = 63;
	}
	checkflags |= ((__int64)1<<workslot);

	if(Y > 0)
	{
		if(X > 0)
		{
			workslot = (X - 1) + ((Y - 1) * iHotZoneMaxX);
			checkflags |= ((__int64)1<<workslot);
		}

		workslot = (X) + ((Y - 1) * iHotZoneMaxX);
		checkflags |= ((__int64)1<<workslot);

		if(X < iHotZoneMaxX)
		{
			workslot = (X + 1) + ((Y - 1) * iHotZoneMaxX);
			checkflags |= ((__int64)1<<workslot);
		}
	}

	if(Y < iHotZoneMaxY)
	{
		if(X > 0)
		{
			workslot = (X - 1) + ((Y + 1) * iHotZoneMaxX);
			checkflags |= ((__int64)1<<workslot);
		}

		workslot = (X) + ((Y + 1) * iHotZoneMaxX);
		checkflags |= ((__int64)1<<workslot);

		if(X < iHotZoneMaxX)
		{
			workslot = (X + 1) + ((Y + 1) * iHotZoneMaxX);
			checkflags |= ((__int64)1<<workslot);
		}
	}

	if(checkflags & tHotZones[slot].active_areas)
	{
		return(1);
	}

	return(0);
}
#endif

/*----------------------------------------------------------------------------
 *
 *	MAILimitAIPosSend
 *
 *		Only send AIPos packets to those who need them.
 *
 */
int MAILimitAIPosSend(DPMSG_AI_POS_SM *Net)
{
	int X, Y;

	MAIGetHotZoneIndex((float)Planes[Net->aislot].WorldPosition.X, (float)Planes[Net->aislot].WorldPosition.Z, &X, &Y);
	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitMissileSend
 *
 *		Only send Missile packets to those who need them.
 *
 */
int MAILimitMissileSend(DPMSG_MISSILE_SM *Net)
{
	int X, Y;

	if(AIObjects[Net->slot].iBriefingGroup)
		return(-1);

	MAIGetHotZoneIndex((float)Planes[Net->slot].WorldPosition.X, (float)Planes[Net->slot].WorldPosition.Z, &X, &Y);
	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitTargetSend
 *
 *		Only send Target packets to those who need them.
 *
 */
int MAILimitTargetSend(DPMSG_TARGET_SM *Net)
{
	int X, Y;
	WeaponParams *W = MAIGetWeaponPointer(Net->weapIX);

	if(!W)
		return(0);

	MAIGetHotZoneIndex((float)W->Pos.X, (float)W->Pos.Z, &X, &Y);
	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitDecoySend
 *
 *		Only send Decoy packets to those who need them.
 *
 */
int MAILimitDecoySend(DPMSG_DECOY_SM *Net)
{
	int X, Y;
	PlaneParams *P;
	WeaponParams *W = MAIGetWeaponPointer(Net->weapIX);

	if(W)
	{
		MAIGetHotZoneIndex((float)W->Pos.X, (float)W->Pos.Z, &X, &Y);
		return(MAICheckHotZones(X, Y));
	}
	else
	{
		if(Net->slot != 255)
		{
			P = &Planes[Net->slot];
		}
		else
		{
			P = NULL;
		}
		if(P)
		{
			MAIGetHotZoneIndex((float)P->WorldPosition.X, (float)P->WorldPosition.Z, &X, &Y);
			return(MAICheckHotZones(X, Y));
		}
	}

	return(0);
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitEjectSend
 *
 *		Only send Eject packets to those who need them.
 *
 */
int MAILimitEjectSend(DPMSG_EJECT_SM *Net)
{
	int X, Y;
	WeaponParams *W = MAIGetWeaponPointer(Net->weapIX);

	if(!W)
		return(0);

	MAIGetHotZoneIndex((float)W->Pos.X, (float)W->Pos.Z, &X, &Y);
	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitMissileGeneralSend
 *
 *		Only send MissileGeneral packets to those who need them.
 *
 */
int MAILimitMissileGeneralSend(DPMSG_MISSILE_GENERAL_SM *Net)
{
	int X, Y;
	BasicInstance  *walker;
	MovingVehicleParams *vehiclepnt;
	PlaneParams *Target;
	int returnval;

	if(Net->slot == 255)
	{
		walker = NULL;
		if(Net->sitetype == MOVINGVEHICLE)
		{
			vehiclepnt = NULL;

			if((long)Net->dwGrndSerialNum <= (LastMovingVehicle - MovingVehicles))
			{
				vehiclepnt = &MovingVehicles[Net->dwGrndSerialNum];
			}

			if(vehiclepnt == NULL)
				return(0);

			MAIGetHotZoneIndex((float)vehiclepnt->WorldPosition.X, (float)vehiclepnt->WorldPosition.Z, &X, &Y);
		}
		else if(Net->dwGrndSerialNum)
		{
			walker = FindInstance(AllInstances, Net->dwGrndSerialNum);

			if(walker == NULL)
				return(0);

			MAIGetHotZoneIndex((float)walker->Position.X, (float)walker->Position.Z, &X, &Y);
		}
	}
	else
	{
		if(AIObjects[Net->slot].iBriefingGroup)
			return(-1);

		MAIGetHotZoneIndex((float)Planes[Net->slot].WorldPosition.X, (float)Planes[Net->slot].WorldPosition.Z, &X, &Y);
	}

	returnval = MAICheckHotZones(X, Y);

	if(returnval)
		return(returnval);

	Target = (Net->target == -1) ? NULL : &Planes[Net->target];

	if(Target)
	{
		MAIGetHotZoneIndex((float)Target->WorldPosition.X, (float)Target->WorldPosition.Z, &X, &Y);
		return(MAICheckHotZones(X, Y));
	}
	return(0);
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitGroundLockSend
 *
 *		Only send GroundLock packets to those who need them.
 *
 */
int MAILimitGroundLockSend(DPMSG_GROUND_LOCK_SM *Net)
{
	int X, Y;

	if(Net->target > 200)
	{
		return(-1);
	}

	MAIGetHotZoneIndex((float)Planes[Net->target].WorldPosition.X, (float)Planes[Net->target].WorldPosition.Z, &X, &Y);
	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitBombGeneralSend
 *
 *		Only send AIPos packets to those who need them.
 *
 */
int MAILimitBombGeneralSend(DPMSG_BOMB_GENERAL_SM *Net)
{
	int X, Y;
	PlaneParams *P;
	WeaponParams *W = MAIGetWeaponPointer(Net->weapIX);

	if(W)
	{
		MAIGetHotZoneIndex((float)W->Pos.X, (float)W->Pos.Z, &X, &Y);
		return(MAICheckHotZones(X, Y));
	}
	else
	{
		if(Net->slot != 255)
		{
			P = &Planes[Net->slot];
		}
		else
		{
			P = NULL;
		}
		if(P)
		{
			if(AIObjects[P - Planes].iBriefingGroup)
				return(-1);

			MAIGetHotZoneIndex((float)P->WorldPosition.X, (float)P->WorldPosition.Z, &X, &Y);
			return(MAICheckHotZones(X, Y));
		}
	}

	return(0);
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitAGMissileGeneralSend
 *
 *		Only send AGMissileGeneral packets to those who need them.
 *
 */
int MAILimitAGMissileGeneralSend(DPMSG_AG_MISSILE_GENERAL_SM *Net)
{
	DBWeaponType *pweapon_type;
	int X, Y;
	int station = Net->weapon & ~(0x60);
	MovingVehicleParams *vehiclepnt;
	int radardatnum = station;

	if(Net->weapon & 0x40)
	{
		vehiclepnt = &MovingVehicles[Net->slot];
		if (!vehiclepnt->iShipType)	// SCOTT FIX
		{
			if(radardatnum == 0)
			{
				pweapon_type = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
			}
			else
			{
				pweapon_type = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
			}
		}
		else
		{
			pweapon_type = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID);
		}

		MAIGetHotZoneIndex((float)vehiclepnt->WorldPosition.X, (float)vehiclepnt->WorldPosition.Z, &X, &Y);
	}
	else
	{
		if(AIObjects[Net->slot].iBriefingGroup)
			return(-1);

		pweapon_type = &pDBWeaponList[Planes[Net->slot].WeapLoad[station].WeapIndex];

		MAIGetHotZoneIndex((float)Planes[Net->slot].WorldPosition.X, (float)Planes[Net->slot].WorldPosition.Z, &X, &Y);
	}

	if(WGetWeaponToPlaneID(pweapon_type) != -1)
	{
		return(-1);
	}

	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	MAIGetWeaponPointer
 *
 *		Get a pointer to a weapon off a net ID.
 *
 */
WeaponParams *MAIGetWeaponPointer(int weaponid)
{
	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(weaponid == W->iNetID)
			{
				return(W);
			}
		}
	}

	return(NULL);
}

/*----------------------------------------------------------------------------
 *
 *	MAISetAHotZone
 *
 *		Make sure the bit representing this area is set.
 *		radius is in NM while X and Y are in WU.
 *
 */
void MAISetAHotZone(float X, float Y, int grndstatic, float radius)
{
	double dworkx, dworky;
	int workx, worky;
	__int64 tempwork;
	int cnt;
	double addx, addy;
	double radiusWU = radius * NMTOWU;

	for(cnt = 0; cnt < 9; cnt ++)
	{
		switch(cnt)
		{
			case 0:
				addx = 0;
				addy = 0;
				break;
			case 1:
				addx = -radiusWU;
				addy = -radiusWU;
				break;
			case 2:
				addx = 0;
				addy = -radiusWU;
				break;
			case 3:
				addx = radiusWU;
				addy = -radiusWU;
				break;
			case 4:
				addx = -radiusWU;
				addy = 0;
				break;
			case 5:
				addx = radiusWU;
				addy = 0;
				break;
			case 6:
				addx = -radiusWU;
				addy = radiusWU;
				break;
			case 7:
				addx = 0;
				addy = radiusWU;
				break;
			case 8:
				addx = radiusWU;
				addy = radiusWU;
				break;
		}

		dworkx = ((double)X + addx) / dHotZoneDivideX;
		workx = (int)dworkx;
		if(workx < 0)
		{
			workx = 0;
		}
		else if(workx > iHotZoneMaxX)
		{
			workx = iHotZoneMaxX;
		}

		dworky = ((double)Y + addy) / dHotZoneDivideY;
		worky = (int)dworky;
		if(worky < 0)
		{
			worky = 0;
		}
		else if(worky > iHotZoneMaxY)
		{
			worky = iHotZoneMaxY;
		}

		tempwork = workx + (worky * (iHotZoneMaxX + 1));
		if(tempwork < 0)
		{
			tempwork = 0;
		}
		else if(tempwork > 63)
		{
			tempwork = 63;
		}

		tHotZones[MySlot].active_areas |= (__int64)1<<tempwork;
		if(grndstatic)
		{
			tGroundDefHotZones.active_areas |= (__int64)1<<tempwork;
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutZones()
 *
 *		Let the other humans know what areas we control things in.
 *
 */
void NetPutZones(int slotnumber)
{
	DPMSG_CONTROL_ZONES NetZones;

	MAISetAHotZone((float) Camera1.CameraLocation.X, (float) Camera1.CameraLocation.Y);

	if(!tHotZones[MySlot].active_areas)
	{
		return;
	}

	if(iMultiCast >= 0)
	{
		NetPutZonesSmall(slotnumber);
		return;
	}

	NetZones.type   = NET_ZONES;
	NetZones.time   = newTime;
	NetZones.slot	= PlayerPlane - Planes;
	NetZones.zones = tHotZones[MySlot];

	if(slotnumber == -1)
	{
		tLastHotZonesSent = NetZones.zones;
		tHotZones[MySlot] = tGroundDefHotZones;
		iHotZoneCheckFlags |= ZONES_GET_MOVERS;

//		NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetZones, sizeof(NetZones) );
		NetAddToBigPacket(MySlot, &NetZones, sizeof(NetZones) );
	}
	else
	{
		NetAddToBigPacket(slotnumber, &NetZones, sizeof(NetZones) );
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetGetZones()
 *
 *		Update Zones for another human
 *
 */
void NetGetZones( DPMSG_CONTROL_ZONES *Net )
{
	tHotZones[Net->slot] = Net->zones;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutZonesSmall()
 *
 *		Let the other humans know what areas we control things in.
 *
 */
void NetPutZonesSmall(int slotnumber)
{
	DPMSG_CONTROL_ZONES_SM NetZones;

	NetZones.type   = NET_ZONES_SM;
	NetZones.zones = tHotZones[MySlot];

	if(slotnumber == -1)
	{
		tLastHotZonesSent = NetZones.zones;
		tHotZones[MySlot] = tGroundDefHotZones;
		iHotZoneCheckFlags |= ZONES_GET_MOVERS;

//		NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetZones, sizeof(NetZones) );
		NetAddToBigPacket(MySlot, &NetZones, sizeof(NetZones) );
	}
	else
	{
//		NetSend( Slot[slotnumber].dpid, DPSEND_GUARANTEED, &NetZones, sizeof(NetZones) );
		NetAddToBigPacket(slotnumber, &NetZones, sizeof(NetZones) );
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetGetZonesSmall()
 *
 *		Update Zones for another human
 *
 */
void NetGetZonesSmall(int sendfrom, DPMSG_CONTROL_ZONES_SM *Net )
{
	int slotnum = NetGetSlotFromPlaneIndex(sendfrom);
	if(slotnum == -1)
		return;

	tHotZones[slotnum] = Net->zones;
}

int trashcnt111 = 0;
void NetCheckZones()
{
	iHotZoneCheckFlags &= ~ZONES_GET_MOVERS;
	if((!lLastZonesSent) || ((lLastZonesSent + 120000) < newTime) || (iHotZoneCheckFlags & ZONES_SEND_FIRST))
	{
		lLastZonesSent = newTime;
		MAISetAHotZone((float) Camera1.CameraLocation.X, (float) Camera1.CameraLocation.Y);
		if((tLastHotZonesSent.active_areas != tHotZones[MySlot].active_areas) || (iHotZoneCheckFlags & ZONES_SEND_FIRST))
		{
			trashcnt111 ++;
			if(trashcnt111 > 50)
			{
	PauseForDiskHit();
	if(lpDD)
	  lpDD->FlipToGDISurface();
	DebugBreak();
	UnPauseFromDiskHit();
				trashcnt111 = 0;
			}
			iHotZoneCheckFlags &= ~ZONES_SEND_FIRST;
			NetPutZones();
		}
		else
		{
			iHotZoneCheckFlags &= ~ZONES_SEND_FIRST;
		}
	}
}



//============================================================================
//		NET AI COMMS MESSAGE PROCESSING
//============================================================================
/*----------------------------------------------------------------------------
 *
 *	NetPutAIComms()
 *
 *		A human has told an AI to do something.  Have this happen on all machines
 *
 */
void NetPutAIComm(int keyflag, WPARAM wParam)
{
	DPMSG_AI_COMMS	NetAIComms;

	if(!gAICommMenu.AICommKeyfunc)
	{
		return;
	}

	if(iMultiCast >= 0)
	{
		NetPutAICommSmall(keyflag, wParam);
		return;
	}

	NetAIComms.type   = NET_AI_COMMS;
	NetAIComms.time   = newTime;
	NetAIComms.slot   = MySlot;
	NetAIComms.keyflag = keyflag;
	NetAIComms.wParam = wParam;

	NetAIComms.commid = 255;
	if(gAICommMenu.AICommKeyfunc == AICOtherCommKeyOps)
	{
		NetAIComms.commid = 1;
	}
//	else if(gAICommMenu.AICommKeyfunc == AICGroupCommKeyOps)
//	{
//		NetAIComms.commid = 2;
//	}
	else if(gAICommMenu.AICommKeyfunc == AICFlightCommKeyOps)
	{
		NetAIComms.commid = 3;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDivisionCommKeyOps)
	{
		NetAIComms.commid = 4;
	}
	else if(gAICommMenu.AICommKeyfunc == AICElementCommKeyOps)
	{
		NetAIComms.commid = 5;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWingmanCommKeyOps)
	{
		NetAIComms.commid = 6;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 7;
	}
	else if(gAICommMenu.AICommKeyfunc == AICStatusCommKeyOps)
	{
		NetAIComms.commid = 8;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFormationCommKeyOps)
	{
		NetAIComms.commid = 9;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDInterceptCommKeyOps)
	{
		NetAIComms.commid = 10;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 11;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEInterceptCommKeyOps)
	{
		NetAIComms.commid = 12;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 13;
	}
	else if(gAICommMenu.AICommKeyfunc == AICTacticalCommKeyOps)
	{
		NetAIComms.commid = 14;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWInterceptCommKeyOps)
	{
		NetAIComms.commid = 15;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 16;
	}
	else if(gAICommMenu.AICommKeyfunc == AIC_AWACS_CommKeyOps)
	{
		NetAIComms.commid = 17;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEscortReleaseCommKeyOps)
	{
		NetAIComms.commid = 18;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 19;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 20;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 21;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 22;
	}
	else if(gAICommMenu.AICommKeyfunc == AICJSTARSCommKeyOps)
	{
		NetAIComms.commid = 23;
	}
	else if(gAICommMenu.AICommKeyfunc == AICTankerCommKeyOps)
	{
		NetAIComms.commid = 24;
	}
	else if(gAICommMenu.AICommKeyfunc == AICTowerCommKeyOps)
	{
		NetAIComms.commid = 25;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFACCommKeyOps)
	{
		NetAIComms.commid = 26;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFlightReportCommKeyOps)
	{
		NetAIComms.commid = 27;
	}
	else if(gAICommMenu.AICommKeyfunc == AICContactReportCommKeyOps)
	{
		NetAIComms.commid = 28;
	}
	else if(gAICommMenu.AICommKeyfunc == AICThreatCallCommKeyOps)
	{
		NetAIComms.commid = 29;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEngagementCommKeyOps)
	{
		NetAIComms.commid = 30;
	}
	else if(gAICommMenu.AICommKeyfunc == AICAlphaCheckCommKeyOps)
	{
		NetAIComms.commid = 31;
	}

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAIComms, sizeof(NetAIComms) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetAIComms()
 *
 *		Get the information for an AI behavior a human is requesting.
 *
 */
void NetGetAIComms( DPMSG_AI_COMMS *Net )
{
	LTAICommMenuData	orgCommMenu = gAICommMenu;
	iAICommFrom = Net->slot;
	if(iSlotToPlane[iAICommFrom] >= 0)
	{
		iAICommFrom = iSlotToPlane[iAICommFrom];
	}

#ifdef _DEBUG
	AICAddAIRadioMsgs("Got Comm", 50);
#endif

	switch(Net->commid)
	{
		case 1:
			AICOtherCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 2:
			AICGroupCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 3:
			AICFlightCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 4:
			AICDivisionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 5:
			AICElementCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 6:
			AICWingmanCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 7:
			AICFGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 8:
			AICStatusCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 9:
			AICFormationCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 10:
			AICDInterceptCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 11:
			AICDGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 12:
			AICEInterceptCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 13:
			AICEGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 14:
			AICTacticalCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 15:
			AICWInterceptCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 16:
			AICWGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 17:
			AIC_AWACS_CommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 18:
			AICEscortReleaseCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 19:
			AICWWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 20:
			AICEWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 21:
			AICDWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 22:
			AICFWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 23:
			AICJSTARSCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 24:
			AICTankerCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 25:
			AICTowerCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 26:
			AICFACCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 27:
			AICFlightReportCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 28:
			AICContactReportCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 29:
			AICThreatCallCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 30:
			AICEngagementCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 31:
			AICAlphaCheckCommKeyOps(Net->keyflag, Net->wParam);
			break;
	}

	gAICommMenu = orgCommMenu;
	iAICommFrom = -1;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutAICommsSmall()
 *
 *		A human has told an AI to do something.  Have this happen on all machines
 *
 */
void NetPutAICommSmall(int keyflag, WPARAM wParam)
{
	DPMSG_AI_COMMS_SM	NetAIComms;

	if(!gAICommMenu.AICommKeyfunc)
	{
		return;
	}

	NetAIComms.type   = NET_AI_COMMS_SM;
	NetAIComms.keyflag = keyflag;
	NetAIComms.wParam = wParam;

	NetAIComms.commid = 255;
	if(gAICommMenu.AICommKeyfunc == AICOtherCommKeyOps)
	{
		NetAIComms.commid = 1;
	}
//	else if(gAICommMenu.AICommKeyfunc == AICGroupCommKeyOps)
//	{
//		NetAIComms.commid = 2;
//	}
	else if(gAICommMenu.AICommKeyfunc == AICFlightCommKeyOps)
	{
		NetAIComms.commid = 3;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDivisionCommKeyOps)
	{
		NetAIComms.commid = 4;
	}
	else if(gAICommMenu.AICommKeyfunc == AICElementCommKeyOps)
	{
		NetAIComms.commid = 5;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWingmanCommKeyOps)
	{
		NetAIComms.commid = 6;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 7;
	}
	else if(gAICommMenu.AICommKeyfunc == AICStatusCommKeyOps)
	{
		NetAIComms.commid = 8;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFormationCommKeyOps)
	{
		NetAIComms.commid = 9;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDInterceptCommKeyOps)
	{
		NetAIComms.commid = 10;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 11;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEInterceptCommKeyOps)
	{
		NetAIComms.commid = 12;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 13;
	}
	else if(gAICommMenu.AICommKeyfunc == AICTacticalCommKeyOps)
	{
		NetAIComms.commid = 14;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWInterceptCommKeyOps)
	{
		NetAIComms.commid = 15;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWGroundAttackCommKeyOps)
	{
		NetAIComms.commid = 16;
	}
	else if(gAICommMenu.AICommKeyfunc == AIC_AWACS_CommKeyOps)
	{
		NetAIComms.commid = 17;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEscortReleaseCommKeyOps)
	{
		NetAIComms.commid = 18;
	}
	else if(gAICommMenu.AICommKeyfunc == AICWWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 19;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 20;
	}
	else if(gAICommMenu.AICommKeyfunc == AICDWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 21;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFWeaponOptionCommKeyOps)
	{
		NetAIComms.commid = 22;
	}
	else if(gAICommMenu.AICommKeyfunc == AICJSTARSCommKeyOps)
	{
		NetAIComms.commid = 23;
	}
	else if(gAICommMenu.AICommKeyfunc == AICTankerCommKeyOps)
	{
		NetAIComms.commid = 24;
	}
	else if(gAICommMenu.AICommKeyfunc == AICTowerCommKeyOps)
	{
		NetAIComms.commid = 25;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFACCommKeyOps)
	{
		NetAIComms.commid = 26;
	}
	else if(gAICommMenu.AICommKeyfunc == AICFlightReportCommKeyOps)
	{
		NetAIComms.commid = 27;
	}
	else if(gAICommMenu.AICommKeyfunc == AICContactReportCommKeyOps)
	{
		NetAIComms.commid = 28;
	}
	else if(gAICommMenu.AICommKeyfunc == AICThreatCallCommKeyOps)
	{
		NetAIComms.commid = 29;
	}
	else if(gAICommMenu.AICommKeyfunc == AICEngagementCommKeyOps)
	{
		NetAIComms.commid = 30;
	}
	else if(gAICommMenu.AICommKeyfunc == AICAlphaCheckCommKeyOps)
	{
		NetAIComms.commid = 31;
	}

	NetAddToBigPacket(MySlot, &NetAIComms, sizeof(NetAIComms) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetAICommsSmall()
 *
 *		Get the information for an AI behavior a human is requesting.
 *
 */
void NetGetAICommsSmall(int fromslot, DPMSG_AI_COMMS_SM *Net )
{
	LTAICommMenuData	orgCommMenu = gAICommMenu;
	iAICommFrom = fromslot;

#ifdef _DEBUG
	AICAddAIRadioMsgs("Got Comm", 50);
#endif

	switch(Net->commid)
	{
		case 1:
			AICOtherCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 2:
			AICGroupCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 3:
			AICFlightCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 4:
			AICDivisionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 5:
			AICElementCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 6:
			AICWingmanCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 7:
			AICFGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 8:
			AICStatusCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 9:
			AICFormationCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 10:
			AICDInterceptCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 11:
			AICDGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 12:
			AICEInterceptCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 13:
			AICEGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 14:
			AICTacticalCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 15:
			AICWInterceptCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 16:
			AICWGroundAttackCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 17:
			AIC_AWACS_CommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 18:
			AICEscortReleaseCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 19:
			AICWWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 20:
			AICEWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 21:
			AICDWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 22:
			AICFWeaponOptionCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 23:
			AICJSTARSCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 24:
			AICTankerCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 25:
			AICTowerCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 26:
			AICFACCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 27:
			AICFlightReportCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 28:
			AICContactReportCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 29:
			AICThreatCallCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 30:
			AICEngagementCommKeyOps(Net->keyflag, Net->wParam);
			break;
		case 31:
			AICAlphaCheckCommKeyOps(Net->keyflag, Net->wParam);
			break;
	}

	gAICommMenu = orgCommMenu;
	iAICommFrom = -1;
}

void MAISetToNewWaypoint(PlaneParams *planepnt, int waypoint)
{
	int numwpts;

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	if((waypoint < planepnt->AI.startwpts) || (waypoint > (planepnt->AI.startwpts + numwpts)))
	{
		return;
	}

	planepnt->AI.CurrWay = &AIWayPoints[waypoint];
	planepnt->AI.numwaypts = numwpts - (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);

	AISetUpNewWayPoint(planepnt);
}



/*----------------------------------------------------------------------------
 *
 *	NetPutDesiredHeading()
 *
 *		Update Desired Heading for things like CAS
 *
 */
void NetPutDesiredHeading(int planenum, ANGLE newheading)
{
	DPMSG_DESIRED_HEADING NetHeading;

	if(iMultiCast >= 0)
	{
		NetPutDesiredHeadingSmall(planenum, newheading);
		return;
	}

	NetHeading.type = NET_AI_HEADING;
	NetHeading.time   = newTime;
	NetHeading.slot	= planenum;
	NetHeading.new_heading = newheading;

	NetAddToBigPacket(MySlot, &NetHeading, sizeof(NetHeading) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDesiredHeading()
 *
 *		Get new Desired Heading for things like CAS
 *
 */
void NetGetDesiredHeading( DPMSG_DESIRED_HEADING *Net )
{
	Planes[Net->slot].AI.DesiredHeading = Net->new_heading;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDesiredHeadingSmall()
 *
 *		Put new Desired Heading for things like CAS
 *
 */
void NetPutDesiredHeadingSmall(int planenum, ANGLE newheading)
{
	DPMSG_DESIRED_HEADING_SM NetHeading;

	NetHeading.type = NET_AI_HEADING_SM;
	NetHeading.slot	= planenum;
	NetHeading.new_heading = newheading;

	NetAddToBigPacket(MySlot, &NetHeading, sizeof(NetHeading) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDesiredHeadingSmall()
 *
 *		Get New Desired Heading for things like CAS
 *
 */
void NetGetDesiredHeadingSmall( DPMSG_DESIRED_HEADING_SM *Net )
{
	Planes[Net->slot].AI.DesiredHeading = Net->new_heading;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutCASData()
 *
 *		Have client AIs also start CAS attack.
 *
 */
void NetPutCASData(PlaneParams *planepnt, int attacktype)
{
	DPMSG_CAS_DATA NetCAS;

	if(!MultiPlayer)
		return;

	if(!planepnt->AI.pGroundTarget)
		return;

	NetCAS.type = NET_CAS_DATA;
	NetCAS.time   = newTime;
	NetCAS.slot	= planepnt - Planes;

	NetCAS.groundtype = planepnt->AI.lGroundTargetFlag;

	if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
	{
		BasicInstance *walker;

		walker = (BasicInstance *)planepnt->AI.pGroundTarget;

		NetCAS.dwGrndSerialNum = walker->SerialNumber;
	}
	else if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		NetCAS.dwGrndSerialNum = (MovingVehicleParams *)planepnt->AI.pGroundTarget - MovingVehicles;
	}

	NetCAS.attacktype = attacktype;
	NetCAS.numrel = planepnt->AI.cNumWeaponRelease;
	NetCAS.weapon = planepnt->AI.cActiveWeaponStation;

	NetAddToBigPacket(MySlot, &NetCAS, sizeof(NetCAS) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetCASData()
 *
 *		Have client AIs get data for CAS attack.
 *
 */
void NetGetCASData( DPMSG_CAS_DATA *Net)
{
	DPMSG_CAS_DATA_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.groundtype = Net->groundtype;
	NetSmall.dwGrndSerialNum = Net->dwGrndSerialNum;
	NetSmall.attacktype = Net->attacktype;
	NetSmall.numrel = Net->numrel;
	NetSmall.weapon = Net->weapon;

	NetGetCASDataSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutCASDataSmall()
 *
 *		Have client AIs also start CAS attack.
 *
 */
void NetPutCASDataSmall(PlaneParams *planepnt, int attacktype)
{
	DPMSG_CAS_DATA NetCAS;

	if(!MultiPlayer)
		return;

	if(!planepnt->AI.pGroundTarget)
		return;

	NetCAS.type = NET_CAS_DATA_SM;
	NetCAS.time   = newTime;
	NetCAS.slot	= planepnt - Planes;

	NetCAS.groundtype = planepnt->AI.lGroundTargetFlag;

	if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
	{
		BasicInstance *walker;

		walker = (BasicInstance *)planepnt->AI.pGroundTarget;

		NetCAS.dwGrndSerialNum = walker->SerialNumber;
	}
	else if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		NetCAS.dwGrndSerialNum = (MovingVehicleParams *)planepnt->AI.pGroundTarget - MovingVehicles;
	}

	NetCAS.attacktype = attacktype;
	NetCAS.numrel = planepnt->AI.cNumWeaponRelease;
	NetCAS.weapon = planepnt->AI.cActiveWeaponStation;

	NetAddToBigPacket(MySlot, &NetCAS, sizeof(NetCAS) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetCASDataSmall()
 *
 *		Have client AIs get data for CAS attack.
 *
 */
void NetGetCASDataSmall( DPMSG_CAS_DATA_SM *Net)
{
	PlaneParams *planepnt;
	int bombtype;
	PlaneParams *leadplane;

	planepnt = &Planes[Net->slot];

	if(planepnt->AI.Behaviorfunc == AIFlyEscort)
	{
		planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK|AI_RETURN_SEAD_COVER);
	}
	else
	{
		planepnt->AI.iAIFlags2 |= (AI_SEAD_ATTACK|AI_CAS_ATTACK);
	}

	if(planepnt->AI.LinkedPlane == NULL)
	{
		AITransferCASLead(planepnt);
	}

	AIDelayCASRadar(AIGetLeader(planepnt));

	if(Net->groundtype == GROUNDOBJECT)
	{
		BasicInstance *walker;
		walker = FindInstance(AllInstances, Net->dwGrndSerialNum);

		if(!walker)
			return;

		planepnt->AI.pGroundTarget = walker;
		planepnt->AI.WayPosition.X = walker->Position.X;
		planepnt->AI.WayPosition.Y = walker->Position.Y + (500 * FTTOWU);
		planepnt->AI.WayPosition.Z = walker->Position.Z;
		planepnt->AI.TargetPos = walker->Position;
	}
	else if(Net->groundtype == MOVINGVEHICLE)
	{
		MovingVehicleParams *vehiclepnt;

		vehiclepnt = &MovingVehicles[(int)Net->dwGrndSerialNum];
		planepnt->AI.pGroundTarget = vehiclepnt;
		planepnt->AI.WayPosition.X = vehiclepnt->WorldPosition.X;
		planepnt->AI.WayPosition.Y = vehiclepnt->WorldPosition.Y + (500 * FTTOWU);
		planepnt->AI.WayPosition.Z = vehiclepnt->WorldPosition.Z;
		planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
	}
	else
	{
		return;
	}

	if(planepnt->AI.iAICombatFlags2 & AI_FAC)
	{
		planepnt->AI.Behaviorfunc = AIStraightRocketAttack;
	}
	else if(Net->attacktype == 1)
	{
		planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
	}
	else if(Net->attacktype == 2)
	{
		planepnt->AI.Behaviorfunc = AIAwayFromTarget;
	}
	else if(Net->attacktype == 4)
	{
		planepnt->AI.Behaviorfunc = AIStraightRocketAttack;
	}
	else if(Net->attacktype == 8)
	{
		planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
	}
	else
	{
		planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
	}

	planepnt->AI.cNumWeaponRelease = Net->numrel;
	planepnt->AI.cActiveWeaponStation = Net->weapon;

	bombtype = pDBWeaponList[planepnt->WeapLoad[Net->weapon].WeapIndex].iWeaponType;

	leadplane = AIGetLeader(planepnt);
	if(bombtype == WEAPON_TYPE_AG_MISSILE)
	{
		if(planepnt->AI.Behaviorfunc != AIAwayFromTarget)
		{
			if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || (leadplane->Status & PL_COMM_DRIVEN) || (planepnt->AI.iAIFlags2 & (AI_SEAD_ATTACK|AI_CAS_ATTACK)))
			{
				planepnt->AI.Behaviorfunc = AILevelBomb;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
			}
			planepnt->AI.lTimer1 = 60000 + ((rand() & 15) * 1000);
		}
	}
	else if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3|AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
	{
		if(planepnt->AI.Behaviorfunc != AIAwayFromTarget)
		{
			if((planepnt->AI.iAIFlags1 & AIPLAYERGROUP) || (leadplane->Status & PL_COMM_DRIVEN))
			{
				planepnt->AI.Behaviorfunc = AILevelBomb;
			}
			else
			{
				planepnt->AI.Behaviorfunc = AIGroupLevelBomb;
			}
		}
	}

	if(Net->attacktype != 3)
	{
#if 0
		if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
		{
			AICSEADAttackMsg(planepnt - Planes, 1);
		}
		else if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
#endif
		if(!(planepnt->AI.iAICombatFlags2 & AI_FAC))
		{
	 		AICWingCASAttackMsg(planepnt - Planes);
		}
	}
	else
	{
		planepnt->AI.iAIFlags2 &= ~(AI_CAS_ATTACK|AI_SEAD_ATTACK);
		planepnt->AI.pGroundTarget = NULL;
		planepnt->AI.Behaviorfunc = AIFlyFormation;
		AINextWayPoint(planepnt);
//		if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
//		{
			AICSEADWinchesterMsg(planepnt - Planes, -1);
//		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage1()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		one BYTE argument.
 *
 */
void NetPutGenericMessage1(PlaneParams *planepnt, BYTE arg1)
{
	DPMSG_GENERIC_1 NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage1Small(planepnt, arg1);
		return;
	}

	NetGen.type = NET_GENERIC_1;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage1()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		one BYTE argument.
 *
 */
void NetGetGenericMessage1( DPMSG_GENERIC_1 *Net)
{
	DPMSG_GENERIC_1_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;

	NetGetGenericMessage1Small(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage1Small()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		one BYTE argument.
 *
 */
void NetPutGenericMessage1Small(PlaneParams *planepnt, BYTE arg1)
{
	DPMSG_GENERIC_1_SM NetGen;

	NetGen.type = NET_GENERIC_1_SM;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage1Small()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		one BYTE argument.
 *
 */
void NetGetGenericMessage1Small( DPMSG_GENERIC_1_SM *Net )
{
	PlaneParams *planepnt;
	PlaneParams *tanker;
	PlaneParams *checkplane;
	BYTE refuel;
	int workvar;
	BYTE bworkvar;
	FPoint fpworkvar;
	int workval;
	int cnt;
	MovingVehicleParams *carrier;
	double deckangle = DECK_ANGLE;
	float offangle;

	switch(Net->arg1)
	{
		case GM_EGRESS:
			AISetUpEgress(&Planes[Net->slot]);
			break;
		case GM_CAP_END:
			AICCAPDone(Net->slot);
			Planes[Net->slot].AI.Behaviorfunc = AIFlyFormation;
			AINextWayPoint(&Planes[Net->slot]);
			break;
		case GM_GROUP_EVADE:
			planepnt = &Planes[Net->slot];
			if(planepnt->AI.cInteriorDamage[AI_REAR_GUN] == -1)
			{
				if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER))
				{
					planepnt->AI.lTimer1 = 300000 - (planepnt->AI.iSkill * 100000);
				}
				else
				{
					planepnt->AI.lTimer1 = 180000;
				}
			}

			if(planepnt->AI.wingman >= 0)
			{
				AISetGroupEvade(&Planes[planepnt->AI.wingman], planepnt);
			}

			if(planepnt->AI.nextpair >= 0)
			{
				AISetGroupEvade(&Planes[planepnt->AI.nextpair], planepnt);
			}
			break;
		case GM_KILL_PLANE:
			planepnt = &Planes[Net->slot];
			planepnt->FlightStatus |= PL_STATUS_CRASHED;
			AIRemoveFromFormation(planepnt);

			if(planepnt->AI.lPlaneID == 85)
			{
				planepnt->Status = 0;
				planepnt->AI.iAIFlags2 = 0;

				OrphanAllPlaneSmoke(planepnt);
			}
			break;
		case GM_TANKER_DISCO:
			planepnt = &Planes[Net->slot];
			planepnt->AI.Behaviorfunc = AITankerDisconnectAI;
			planepnt->AI.lTimer2 = 10000;
			break;
		case GM_TANKER_DISCO_HUMAN:
			planepnt = &Planes[Net->slot];
			planepnt->AI.Behaviorfunc = AITankerDisconnect;
			break;
		case GM_TANKER_DISCO_HUMAN2:
			planepnt = &Planes[Net->slot];
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			AIChangeGroupBehavior(planepnt, AIFlyTankerFormation, AIFlyFormation);
			if(planepnt == PlayerPlane)
			{
				planepnt->Status |= AL_DEVICE_DRIVEN;
			}
			else
			{
				planepnt->Status |= AL_COMM_DRIVEN;
			}
			planepnt->Status &= ~AL_AI_DRIVEN;
			planepnt->DoControlSim = CalcF18ControlSurfaces;
			planepnt->DoCASSim     = CalcF18CASUpdates;
			planepnt->DoForces     = CalcAeroForces;
			planepnt->DoPhysics    = CalcAeroDynamics;
			planepnt->dt = HIGH_FREQ;
			planepnt->UpdateRate = HIGH_AERO;
			planepnt->AI.iAIFlags2 &= ~(AI_FAKE_AOA);
			planepnt->RefuelProbeCommandedPos = 0.0f;
			planepnt->AI.lVar3 = 0;

			tanker = AICheckTankerCloseBy(planepnt);  // PlayerPlane
			if(tanker)
			{
				refuel = tanker->AI.cUseWeapon;
			}
			else
			{
				refuel = 0;
			}

			checkplane = planepnt;
			while(checkplane)
			{
				if(checkplane->AI.wingman >= 0)
				{
					RefuelFuelPlane(&Planes[checkplane->AI.wingman], refuel);
				}
				if(checkplane->AI.nextpair >= 0)
				{
					checkplane = &Planes[checkplane->AI.nextpair];
					RefuelFuelPlane(checkplane, refuel);
				}
				else
				{
					checkplane = NULL;
				}
			}
			break;
		case GM_END_CASUAL_AUTO:
			planepnt = &Planes[Net->slot];
			EndCasualAutoPilotPlane(planepnt);
			break;
		case GM_BOLTER:
			planepnt = &Planes[Net->slot];
			planepnt->AI.Behaviorfunc = AIFlyBolter;
			planepnt->AI.lTimer3 = 120000;

			if(AICAllowThisRadio(planepnt - Planes, 1))
			{
				AICJustMessage(AIC_BOLTER, AICF_BOLTER, SPCH_LSO);
				AICAddSoundCall(AIC_BolterStart, Net->slot, 4000, 50);
			}
			break;
		case GM_CLEAN:
			AICCleanCall(Net->slot);
			break;
		case GM_DROP_CHUTES:
			int wingman, nextsec;

			planepnt = &Planes[Net->slot];
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIDropChuteMines);
			planepnt->AI.lTimer2 = -1;
			wingman = planepnt->AI.wingman;
			nextsec = planepnt->AI.nextpair;
			while((wingman >= 0) || (nextsec >= 0))
			{
				if(wingman >= 0)
				{
					Planes[wingman].AI.lTimer2 = rand() & 0x3FF;
				}
				if(nextsec >= 0)
				{
					Planes[nextsec].AI.lTimer2 = rand() & 0x3FF;
					wingman = Planes[nextsec].AI.wingman;
					nextsec = Planes[nextsec].AI.nextpair;
				}
				else
				{
					wingman = -1;
					nextsec = -1;
				}
			}
			planepnt->AI.cNumWeaponRelease = 0;
			planepnt->AI.cActiveWeaponStation = -1;
			break;
		case GM_ESCORT_ENGAGE:
			planepnt = &Planes[Net->slot];
			planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
			planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
			planepnt->AI.iAIFlags2 &= ~(AI_SEAD_COVER);
			AIC_Basic_Ack(Net->slot);
			break;
		case GM_ESCORT_SEAD:
			planepnt = &Planes[Net->slot];
			planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
			planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
			planepnt->AI.iAIFlags2 |= AI_SEAD_COVER;
			AIC_Basic_Ack(Net->slot);
			break;
		case GM_ESCORT_CAS:
			planepnt = &Planes[Net->slot];
			planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_STAY);
			planepnt->AI.iAICombatFlags2 |= AI_ESCORT_CAS;
			planepnt->AI.iAIFlags2 &= ~AI_SEAD_COVER;
			AIC_Basic_Ack(Net->slot);
			break;
		case GM_ESCORT_REJOIN:
			planepnt = &Planes[Net->slot];
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyEscort, 0, 1);
			planepnt->AI.iAICombatFlags2 |= (AI_ESCORT_STAY);
			planepnt->AI.iAIFlags2 &= ~AI_SEAD_COVER;
			planepnt->AI.iAICombatFlags2 &= ~(AI_ESCORT_CAS);
			if(AIIsSEADPlane(planepnt))
			{
				planepnt->AI.iAIFlags2 |= AI_SEAD_COVER;
			}
			AIC_Basic_Ack(Net->slot);
			break;
		case GM_ALLOW_REGEN:
			iAllowRegen = 1;
			break;
		case GM_REQUEST_WEATHER:
			if(g_bIAmHost)
			{
				planepnt = &Planes[Net->slot];
				workvar = NetGetSlotFromPlaneIndex(planepnt - Planes);
				if((planepnt - Planes) == iSlotToPlane[workvar])
				{
					iNetHasWeather |= 1<<workvar;
					iMAISendTo = workvar;
					bworkvar = 0;
					if(SnowOn)
					{
						bworkvar |= 0x1;
					}
					if(doLightning)
					{
						bworkvar |= 0x2;
					}
					if(doRain)
					{
						bworkvar |= 0x4;
					}
					fpworkvar.X = WorldParams.Weather;
					fpworkvar.Y = WorldParams.CloudAlt;
					fpworkvar.Z = WorldParams.Visibility;
					NetPutGenericMessage2FPoint(NULL, GM2FP_WEATHER_INFO, fpworkvar, bworkvar);
					iMAISendTo = -1;
				}
			}
			break;
		case GM_HOST_EXITS:  //  Make it so that the rest of the players have to leave the game.  Also to Player Exits.
			iEndGameState = 5;
			if(lHostLeftTimer < 0)
			{
				lHostLeftTimer = 3000;
			}

			OldCockpitSeat    = CockpitSeat;
			OldCurrentCockpit = CurrentCockpit;
			memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));

			GetAsyncKeyState(VK_ESCAPE);
			GetAsyncKeyState(VK_RETURN);
		case GM_PLAYER_EXITS:
			if(Planes[Net->slot].Status)
			{
				NewGenerator(PLANE_EXPLODES,Planes[Net->slot].WorldPosition,0.0,2.0f,50);
				Planes[Net->slot].Status = 0;
				Planes[Net->slot].AI.iAIFlags2 = 0;
				AIRemoveFromFormation(&Planes[Net->slot]);
				Slot[Net->slot].name[0] = 0;
			}

			OrphanAllPlaneSmoke(&Planes[Net->slot]);
			break;
		case GM_RELOAD_PLANE:
			planepnt = &Planes[Net->slot];
			workval = NetGetSlotFromPlaneIndex(Net->slot);
			if(workval == Net->slot)
			{
				if(workval >= MAX_HUMANS)
					return;
				if(iSlotToPlane[workval] != workval)
					return;
			}
			for(cnt = 0; cnt < MAX_F18E_STATIONS; cnt ++)
			{
				planepnt->WeapLoad[cnt] = NetRegenPlanes[workval].WeapLoad[cnt];
			}
			break;
		case GM_ESTABLISHED:
			planepnt = &Planes[Net->slot];
			AICEstablishedMarshal(planepnt - Planes);
			planepnt->AI.lVar2 = 2;
			break;
		case GM_COMMENCE_NOW:
			planepnt = &Planes[Net->slot];
			AICCommenceNow(planepnt - Planes);
			AICAddSoundCall(AICCommencingPush, (planepnt - Planes), 5000, 40, -1);
			planepnt->AI.lVar2 = 4;
			break;
		case GM_RETURN_MARSHAL:
			planepnt = &Planes[Net->slot];
			AICReturnToMarshall(planepnt - Planes);
			planepnt->AI.lVar2 = 3;
			break;
		case GM_PLATFORM:
			planepnt = &Planes[Net->slot];
			AICPlatform(planepnt - Planes);
			planepnt->AI.lVar2 = 5;
			break;
		case GM_ACLS_CONTACT:
			planepnt = &Planes[Net->slot];
			AIC_ACLSContact(planepnt - Planes);
			planepnt->AI.lVar2 = 8;
			break;
		case GM_BOLTER_RADIO:
			planepnt = &Planes[Net->slot];
			if((planepnt->AI.iAICombatFlags1 & AI_HOME_AIRPORT) || (planepnt->AI.iHomeBaseId < 0))
			{
				return;
			}
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
			planepnt->AI.iAICombatFlags2 &= ~(AI_BOLTERED);
			AIC_GenericMsgPlaneBearing(planepnt - Planes, 4, AIConvertAngleTo180Degree(carrier->Heading + 0x8000 + AIConvert180DegreeToAngle(deckangle)));
			planepnt->AI.lVar2 = 12;
			planepnt->AI.lTimer2 = -1;
			break;
		case GM_BOLTER_RADIO_2:
			planepnt = &Planes[Net->slot];
			if((planepnt->AI.iAICombatFlags1 & AI_HOME_AIRPORT) || (planepnt->AI.iHomeBaseId < 0))
			{
				return;
			}
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
			offangle = AIConvertAngleTo180Degree(carrier->Heading) + deckangle;
			offangle = AICapAngle(offangle);

			AIC_GenericMsgPlaneBearing(planepnt - Planes, 4, offangle);
			planepnt->AI.lVar2 = 7;
			break;
		case GM_SET_RELAUNCH:
			planepnt = &Planes[Net->slot];
			planepnt->AI.lTimer3 = -1;
			CTCheckForRelaunch(planepnt);
			break;
		case GM_RELEASE_HOOK:
			planepnt = &Planes[Net->slot];
			if((planepnt->AI.iAICombatFlags1 & AI_HOME_AIRPORT) || (planepnt->AI.iHomeBaseId < 0))
			{
				return;
			}
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];
			PlaneReleaseTailhook(planepnt,carrier);
			if(planepnt->PlaneCopy)
			{
				PlaneReleaseTailhook(planepnt->PlaneCopy, carrier);
			}
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		two BYTE arguments.
 *
 */
void NetPutGenericMessage2(PlaneParams *planepnt, BYTE arg1, BYTE arg2)
{
	DPMSG_GENERIC_2 NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage2Small(planepnt, arg1, arg2);
		return;
	}

	NetGen.type = NET_GENERIC_2;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		two BYTE argument.
 *
 */
void NetGetGenericMessage2( DPMSG_GENERIC_2 *Net)
{
	DPMSG_GENERIC_2_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;

	NetGetGenericMessage2Small(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2Small()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		two BYTE argument.
 *
 */
void NetPutGenericMessage2Small(PlaneParams *planepnt, BYTE arg1, BYTE arg2)
{
	DPMSG_GENERIC_2_SM NetGen;

	NetGen.type = NET_GENERIC_2_SM;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2Small()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		two BYTE argument.
 *
 */
void NetGetGenericMessage2Small( DPMSG_GENERIC_2_SM *Net )
{
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	PlaneParams *tanker;
	MovingVehicleParams *vehiclepnt;
	float fworkspeed;

	planepnt = &Planes[Net->slot];
	switch(Net->arg1)
	{
		case GM2_VECTOR:
			NetVectorIntercept(Net->slot, Net->arg2);
			break;
		case GM2_ENGAGE_SET:
			if(planepnt->AI.Behaviorfunc == AIAirInvestigate)
			{
				planepnt = &Planes[Net->slot];
				planepnt->AI.Behaviorfunc = AIAirEngage;
				planepnt->AI.lVar2 = Net->arg2;
				leadplane = AIGetLeader(planepnt);
				if(leadplane != planepnt)
				{
					planepnt->AI.TargetPos = leadplane->AI.TargetPos;
					planepnt->AI.iAIFlags1 &= ~AIINFORMATION;
				}
			}
			break;
		case GM2_NEW_TARGET_PING:
		case GM2_NEW_TARGET:
			NetUpdateTarget(Net);
			break;
		case GM2_NEW_THREAT:
			planepnt->AI.AirThreat = (Net->arg2 == 255) ? NULL : &Planes[Net->arg2];
			break;
		case GM2_MULTI_FLAGS:
			if(Net->arg2 & MC_SUPPORT)
			{
				planepnt->AI.iAIFlags2 |= AISUPPORT;
			}
			else
			{
				planepnt->AI.iAIFlags2 &= ~(AISUPPORT);
			}
			if(Net->arg2 & MC_DEFENSIVE)
			{
				planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
			}
			else
			{
				planepnt->AI.iAIFlags2 &= ~(AIDEFENSIVE);
			}
			if(Net->arg2 & MC_MISSILE_AVOID)
			{
				planepnt->AI.iAIFlags2 |= AIMISSILEDODGE;
			}
			else
			{
				planepnt->AI.iAIFlags2 &= ~(AIMISSILEDODGE);
			}
			if(Net->arg2 & MC_DOGFIGHT)
			{
				planepnt->AI.iAIFlags2 |= AI_IN_DOGFIGHT;
			}
			else
			{
				planepnt->AI.iAIFlags2 &= ~(AI_IN_DOGFIGHT);
			}

			if(!(Net->arg2 & MC_COMBAT_BEHAVE))
			{
				planepnt->AI.CombatBehavior = NULL;
			}

			if(Net->arg2 & MC_WINCHESTER_AA)
			{
				planepnt->AI.iAICombatFlags1 |= AI_WINCHESTER_AA;
			}
			else
			{
				planepnt->AI.iAICombatFlags1 &= ~(AI_WINCHESTER_AA);
			}

			if(Net->arg2 & MC_WINCHESTER_AG)
			{
				planepnt->AI.iAICombatFlags1 |= AI_WINCHESTER_AG;
			}
			else
			{
				planepnt->AI.iAICombatFlags1 &= ~(AI_WINCHESTER_AG);
			}
			break;
		case GM2_UPDATE_BEHAVE:
			if(Net->arg2 != 255)
			{
				MAIUpdateBehavior(Net->arg2, planepnt, 0);
			}
			break;
		case GM2_UPDATE_ORGBEHAVE:
			if(Net->arg2 != 255)
			{
				MAIUpdateBehavior(Net->arg2, planepnt, 1);
			}
		case GM2_LINK_PLANE:
			if(Net->arg2 != 255)
			{
				planepnt->AI.LinkedPlane = &Planes[Net->arg2];
			}
			else
			{
				planepnt->AI.LinkedPlane = NULL;
			}
			break;
		case GM2_ATTACH_HOSE:
			//  planepnt == tanker, leadplane == tanking
			if(Net->arg2 != 255)
			{
				leadplane = &Planes[Net->arg2];
				AttachPlanePointToRefuelingHose(planepnt,(DWORD)leadplane,0);
			}
			else
			{
				planepnt->RefuelingHoses[0].Status &= ~RH_CONNECTED;
				planepnt->RefuelingHoses[0].ConnectedPlane = NULL;
			}
			break;
		case GM2_TRANSFER_CONTROL:
			if(Net->arg2 != 255)
			{
				leadplane = &Planes[Net->arg2];
				leadplane->AI.iMultiPlayerAIController = Net->slot;
				leadplane->AI.iAICombatFlags1 &= ~(AI_MULTI_ACTIVE);
			}
			break;
		case GM2_CONTACT_REFUELER_NO_B:
			if(Net->arg2 != 255)
			{
				tanker = &Planes[Net->arg2];

				tanker->AI.LinkedPlane = planepnt;
				if(planepnt->Status & PL_AI_DRIVEN)
				{
					planepnt->RefuelProbeCommandedPos = 45.0f;
					planepnt->AI.lVar3 = 0x4;
				}
				else
				{
					planepnt->AI.Behaviorfunc = AIFlyTankerFormation;
					planepnt->AI.lVar3 = 0x4;
				}

				long delaycnt = 0;

				AICAddSoundCall(AICTankerOn1, tanker - Planes, delaycnt, 50);
				delaycnt = delaycnt + 5000;

				AICAddSoundCall(AICTankerOn2, tanker - Planes, delaycnt, 50);
				delaycnt = delaycnt + 5000;

				AICAddSoundCall(AICTankerContact1, planepnt - Planes, delaycnt, 50, tanker->AI.cUseWeapon);

				tanker->AI.fVarA[0]	= CTGetTankingAmount(planepnt, tanker->AI.cUseWeapon);

				tanker->AI.Behaviorfunc = AITankingFlight;

				if((planepnt == PlayerPlane) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
				{
					iAIHumanTankerFlags = 0;
					NetPutGenericMessage2(planepnt, GM2_TRANSFER_CONTROL, (tanker - Planes));
				}
			}
			break;
		case GM2_DONE_TANKING:
			if(Net->arg2 != 255)
			{
				AICTankerDisconnecting(planepnt - Planes, Net->arg2);
				leadplane = &Planes[Net->arg2];
				leadplane = AIGetLeader(leadplane);
				AIChangeGroupBehavior(leadplane, AIFlyTankerFormation, AIFlyFormation);
				AIUpdateGroupPlaneLink(leadplane - Planes, NULL);
				AINextWayPointNoActions(leadplane);
				AISetUpNewWayPoint(leadplane);
//				AIResetTanker(planepnt);
				if(g_bIAmHost)
				{
					planepnt->AI.iMultiPlayerAIController = PlayerPlane - Planes;
					planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
					NetPutGenericMessage2(PlayerPlane, GM2_TRANSFER_CONTROL, (planepnt - Planes));
				}
			}
			else
			{
				AIResetTanker(planepnt);
				if(g_bIAmHost)
				{
					planepnt->AI.iMultiPlayerAIController = PlayerPlane - Planes;
					planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
					NetPutGenericMessage2(PlayerPlane, GM2_TRANSFER_CONTROL, (planepnt - Planes));
				}
			}
			break;
		case GM2_AUTOTANK_1:
			if(Net->arg2 != 255)
			{
				tanker = &Planes[Net->arg2];

				if(planepnt->Status & (AL_DEVICE_DRIVEN | AL_COMM_DRIVEN))
				{
					double aoadeg, cosroll, aoaoffset;

					aoadeg = RadToDeg(planepnt->Alpha);
					cosroll = cos(DegToRad(fabs(AIConvertAngleTo180Degree(planepnt->Roll))));
					aoaoffset = cosroll * aoadeg;
					planepnt->AI.AOADegPitch = AIConvert180DegreeToAngle(aoaoffset);
					planepnt->AI.iAIFlags2 |= (AI_FAKE_AOA);
				}
				planepnt->Status &= ~(AL_DEVICE_DRIVEN|AL_COMM_DRIVEN);
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
				planepnt->AI.Behaviorfunc = AIFlyTankerFormation;
				planepnt->AI.lVar3 |= 0x4;
			}
			break;
		case GM2_AUTOTANK_2:
			if(Net->arg2 != 255)
			{
				tanker = &Planes[Net->arg2];

				planepnt->AI.Behaviorfunc = AIFlyTankerFormation;
				planepnt->Status &= ~(AL_DEVICE_DRIVEN|AL_COMM_DRIVEN);
				planepnt->Status |= AL_AI_DRIVEN;
		//		AICAddAIRadioMsgs("AI", 50);
				planepnt->DoControlSim = CalcF18ControlSurfacesSimple;
				planepnt->DoCASSim = CalcF18CASUpdatesSimple;
				planepnt->DoForces = CalcAeroForcesSimple;
				planepnt->DoPhysics = CalcAeroDynamicsSimple;

				planepnt->MaxPitchRate = 10.0;
				planepnt->MaxRollRate = 90.0;
				planepnt->YawRate = 15.0;

				planepnt->MaxSpeedAccel = 20;	//15
				planepnt->SpeedPercentage = 1.0;

				planepnt->BfLinVel.X = planepnt->V;
				planepnt->BfLinVel.Y = 0;
				planepnt->BfLinVel.Z = 0;
				planepnt->AI.lVar3 |= 0x4;
			}
			break;
		case GM2_GET_MARSHAL:
			if(g_bIAmHost)
			{
				AIContactCarrierMarshallPattern(planepnt);
			}
			break;
		case GM2_AWACS_DECLARE:
			AIC_AWACS_Declare(Net->slot, Net->arg2);
			break;
		case GM2_GENERIC_RADIO:
			AIC_GenericMsgPlane(Net->slot, Net->arg2);

			//  I might want to separate this out to another call, but should work for now.
			if((Net->arg2 == 32) || (Net->arg2 == 33) || (Net->arg2 == 36))
			{
				int awacsnum;

				awacsnum = AIGetClosestAWACS(planepnt);
				if(awacsnum < 0)
					awacsnum = -2;

				if(Net->arg2 == 36)
				{
					AICAddSoundCall(AICStrikeSwitchControl, Net->slot, 8000, 50, awacsnum);
				}
				else
				{
					AICAddSoundCall(AIC_AWACS_Acknowledged, planepnt - Planes, 6000, 50, awacsnum);
				}
			}
			break;
		case GM2_SAR_PLANE:
			leadplane = &Planes[Net->arg2];
			leadplane->AI.iAICombatFlags2 |= AI_SAR_CALLED;
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToSARPoint);
			planepnt->AI.TargetPos = leadplane->WorldPosition;
			planepnt->AI.TargetPos.Y += 1000 * FTTOWU;
			planepnt->AI.lVar2 = leadplane - Planes;
			planepnt->AI.lTimer2 = 30000;
			AICAddSoundCall(AIC_SAR_On_Way_Msg, Net->slot, 30000, 50);
			planepnt->AI.lGroundTargetFlag = AIRCRAFT;
			planepnt->AI.pGroundTarget = leadplane;

			if(AIInPlayerGroup(leadplane))
			{
				for(WeaponParams *W=Weapons; W<=LastWeapon; W++)
				{
					if((W->P == leadplane) && (W->Type == pGuyOnGroundType) && (W->Flags & WEAPON_INUSE) && (W->fpTempWay.X == 1))
					{
						FPoint updatedpos;
						updatedpos = W->Pos;

						updatedpos.X += ((frand() * 1000.0f) - 500.0f) * FTTOWU;
						updatedpos.Y += 1000 * FTTOWU;
						updatedpos.Z += (frand() * 1000.0f) - 500.0f * FTTOWU;

						planepnt->AI.TargetPos = updatedpos;

						if(rand() & 1)
						{
							AICAddSoundCall(AIC_Help_On_Way, leadplane - Planes, 10000, 50, PlayerPlane - Planes);
						}
						else
						{
							AICAddSoundCall(AIC_SAR_Inbound, leadplane - Planes, 10000, 50, PlayerPlane - Planes);
						}

						break;
					}
				}
			}
			break;
		case GM2_SAR_VEHICLE:
			vehiclepnt = &MovingVehicles[Net->arg2];
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyToSARPoint);
			planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
			planepnt->AI.TargetPos.Y += 1000 * FTTOWU;
			planepnt->AI.lVar2 = vehiclepnt - MovingVehicles;
			planepnt->AI.lTimer2 = 30000;
			AICAddSoundCall(AIC_SAR_On_Way_Msg, Net->slot, 30000, 50);
			planepnt->AI.lGroundTargetFlag = MOVINGVEHICLE;
			planepnt->AI.pGroundTarget = vehiclepnt;
			break;
		case GM2_REPLACE_CAP:
			leadplane = &Planes[Net->arg2];
			planepnt->AI.iAIFlags2 &= ~AIINVISIBLE;
			planepnt->Status |= PL_ACTIVE;
			planepnt->AI.CurrWay = leadplane->AI.CurrWay;
			planepnt->AI.startwpts = leadplane->AI.startwpts;
			planepnt->AI.numwaypts = leadplane->AI.numwaypts;
			planepnt->AI.Behaviorfunc = AIFlyFormation;
			AISetUpNewWayPoint(planepnt);
			if(planepnt->OnGround)
			{
				planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
				planepnt->AI.Behaviorfunc = AIFlyTakeOff;
			}
			break;
		case GM2_SAR_CAP:
			leadplane = &Planes[Net->arg2];
			fworkspeed = 0.8f * pDBAircraftList[planepnt->AI.iPlaneIndex].iMaxSpeed;
			planepnt->AI.lDesiredSpeed = fworkspeed;
			AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AISetUpSARCAP, 0);
			planepnt->AI.WayPosition = leadplane->AI.TargetPos;
			planepnt->AI.WayPosition.Y = 10000 * FTTOWU;
			AIUpdateGroupPlaneLink(planepnt - Planes, leadplane);

			if(planepnt->AI.iAIFlags2 & AIINVISIBLE)
			{
				planepnt->AI.iAIFlags2 &= ~AIINVISIBLE;
				planepnt->Status |= PL_ACTIVE;
				if(planepnt->OnGround)
				{
					planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
					planepnt->AI.Behaviorfunc = AIFlyTakeOff;
				}
			}
			break;
		case GM2_WING_EJECT:
			AIC_Wingman_Ejected_Msg(Net->slot, Net->arg2);
			break;
		case GM2_ACLS_SET:
			AIC_ACLS_Switch(planepnt, Net->arg2);
			break;
		case GM2_LANDING_DIST:
			AICLandingDistance(planepnt - Planes, Net->arg2);
			planepnt->AI.lVar2 += 2;
			break;
		case GM2_UPDATE_VOICE:
			planepnt->AI.lAIVoice = (int)Net->arg2 * 1000;
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		three BYTE arguments.
 *
 */
void NetPutGenericMessage3(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3)
{
	DPMSG_GENERIC_3 NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage3Small(planepnt, arg1, arg2, arg3);
		return;
	}

	NetGen.type = NET_GENERIC_3;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage3()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		three BYTE argument.
 *
 */
void NetGetGenericMessage3( DPMSG_GENERIC_3 *Net)
{
	DPMSG_GENERIC_3_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;
	NetSmall.arg3 = Net->arg3;

	NetGetGenericMessage3Small(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3Small()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		three BYTE argument.
 *
 */
void NetPutGenericMessage3Small(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3)
{
	DPMSG_GENERIC_3_SM NetGen;

	NetGen.type = NET_GENERIC_3_SM;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage3Small()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		three BYTE argument.
 *
 */
void NetGetGenericMessage3Small( DPMSG_GENERIC_3_SM *Net )
{
	PlaneParams *planepnt;
	PlaneParams *target;
	PlaneParams *wingplane = NULL;
	WeaponParams W;
	PlaneParams *tanker;
	MovingVehicleParams *vehiclepnt;
	int cnt;

	planepnt = &Planes[Net->slot];
	switch(Net->arg1)
	{
		case GM3_CAP_1:
		case GM3_CAP_2:
		case GM3_CAP_3:
		case GM3_CAP_4:
			planepnt->AI.iVar1 = AIConvert8ToHeading(Net->arg2);
			planepnt->AI.lVar2 = Net->arg3;
			NetAdjustCAPLegs(planepnt, Net->arg1);
			break;
		case GM3_WING_HELP:
			if(Net->arg3 == 255)
			{
				planepnt->AI.AirTarget = NULL;
			}
			else
			{
				planepnt->AI.AirTarget = &Planes[Net->arg3];
			}
			planepnt->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
			planepnt->AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);
			planepnt->AI.iAIFlags2 |= AIKEEPTARGET;
			if(Net->arg2)
			{
				planepnt->AI.lCombatTimer = -1;
				planepnt->AI.CombatBehavior = NULL;
			}
			break;
		case GM3_CAP_ATTACK:
			target = (Net->arg3 == 255) ? NULL : &Planes[Net->arg3];
			switch(Net->arg2)
			{
				case 1:
				case 2:
					AISetUpPlayerCAPIntercept(planepnt, target);
					break;
				case 3:
					if(planepnt->AI.wingman >= 0)
					{
						wingplane = &Planes[planepnt->AI.wingman];
					}
					else if(planepnt->AI.winglead >= 0)
					{
						wingplane = &Planes[planepnt->AI.winglead];
					}
					else
					{
						return;
					}

					planepnt->AI.OrgBehave = wingplane->AI.OrgBehave = AISetUpCAPStartSinglePair;
					if((wingplane->AI.AirTarget != planepnt->AI.AirTarget) && (!(wingplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						wingplane->AI.lCombatTimer = -1;
						wingplane->AI.CombatBehavior = NULL;
					}
					wingplane->AI.lTimer1 = 360000;
					break;
				case 4:
					planepnt->AI.OrgBehave = AISetUpCAPStartSinglePair;
					break;
			}
			break;
		case GM3_UPDATE_BEHAVES:
			if(Net->arg2 != 255)
			{
				MAIUpdateBehavior(Net->arg2, planepnt, 0);
			}
			if(Net->arg3 != 255)
			{
				MAIUpdateBehavior(Net->arg3, planepnt, 1);
			}
			else
			{
				planepnt->AI.OrgBehave = NULL;
			}
			break;
		case GM3_DESIRED_BOMBS:
			planepnt->AI.cActiveWeaponStation = Net->arg2;
			planepnt->AI.cNumWeaponRelease = Net->arg3;
			break;
		case GM3_WING_UPD:
			planepnt->AI.winglead = (Net->arg2 == 255) ? -1 : Net->arg2;
			planepnt->AI.wingman = (Net->arg3 == 255) ? -1 : Net->arg3;
			break;
		case GM3_PAIR_UPD:
			planepnt->AI.prevpair = (Net->arg2 == 255) ? -1 : Net->arg2;
			planepnt->AI.nextpair = (Net->arg3 == 255) ? -1 : Net->arg3;
			break;
		case GM3_LOG_V_DEST:
			for(cnt = 0; cnt < MAX_WEAPON_TYPES; cnt ++)
			{
				W.P = planepnt;
				if(WeaponTypes[cnt].TypeNumber == Net->arg2)
				{
					W.Type = &WeaponTypes[cnt];
					break;
				}
				else if(WeaponTypes[cnt].TypeNumber == -1)
				{
					return;
				}
				LogDestroyedObject(&W, MOVINGVEHICLE, Net->arg3);
			}
			break;
		case GM3_CONTACT_REFUELER:
			if(Net->arg2 != 255)
			{
				tanker = &Planes[Net->arg2];

				tanker->AI.LinkedPlane = planepnt;
				if(planepnt->Status & PL_AI_DRIVEN)
				{
					AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyTankerFormation);
					AIUpdateGroupPlaneLink(planepnt - Planes, AIGetLeader(tanker));
					planepnt->RefuelProbeCommandedPos = 45.0f;
					planepnt->AI.lVar3 = 0x4;
				}
				else
				{
					planepnt->AI.lVar3 = 0x4;
					planepnt->AI.Behaviorfunc = AIFlyTankerFormation;
				}
				tanker->AI.Behaviorfunc = AITankingFlight;
				tanker->AI.lVar2 = (5.0f * NMTOFT) + 200;  //  + 200 kludge to handle loss of precision.

				tanker->AI.lTimer2 = -1;
				tanker->AI.TargetPos.Y = -1.0;
				planepnt->AI.lTimer2 = -1;

				tanker->AI.cUseWeapon = Net->arg3;

				tanker->AI.fVarA[0]	= CTGetTankingAmount(planepnt, Net->arg3);

				tanker->AI.fVarA[1] = GetRegValueL("refuel_rate");
				if(tanker->AI.fVarA[1] < 1500.0f)
				{
					tanker->AI.fVarA[1] = 1000.0f;
				}
			}
			break;
		case GM3_WAVEOFF:
			planepnt->AI.Behaviorfunc = AIFlyBolter;
			planepnt->AI.lTimer3 = Net->arg2 * 10000;

			if(AICAllowThisRadio(planepnt - Planes, 1))
			{
				if(Net->arg3 == 2)
				{
					AICJustMessage(AICF_WAVEOFF_INTENSE, AICF_WAVEOFF_INTENSE, SPCH_LSO);
				}
				else if(Net->arg3 == 3)
				{
					AICJustMessage(AIC_WAVEOFF_3, AICF_WAVEOFF_3, SPCH_LSO);
				}
				else if(Net->arg3 == 4)
				{
					AICJustMessage(AICF_WAVEOFF_DECK, AICF_WAVEOFF_DECK, SPCH_LSO);
				}
				else
				{
					AICJustMessage(AICF_WAVEOFF, AICF_WAVEOFF, SPCH_LSO);
				}
			}
			AICAddSoundCall(AIC_BolterStart, Net->slot, 4000, 50);
			break;
		case GM3_SORT_MSG:
			AICDoSortMsg(Net->slot, Net->arg2, Net->arg3);
			break;
		case GM3_NUM_IN_PIC:
			AIC_TellNumInPicture(Net->slot, Net->arg2, Net->arg3);
			break;
		case GM3_SINK_SHIP:
				netCall = 1;	// prevent recursion for remote procedure calls
				vehiclepnt = &MovingVehicles[Net->arg2];
				if(vehiclepnt->iShipType)
				{
					DestroyShip(vehiclepnt, Net->arg3 & 1);
					vehiclepnt->Status &= ~(VL_SINKING_POS|VL_SINKING_NEG);
					if(Net->arg3 & 2)
						vehiclepnt->Status |= VL_SINKING_POS;
					else
						vehiclepnt->Status |= VL_SINKING_NEG;
				}
				else
				{
					DestroyVehicle(vehiclepnt, Net->arg3 & 1);
				}

				netCall = 0;
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage4()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		four BYTE arguments.
 *
 */
void NetPutGenericMessage4(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3, BYTE arg4)
{
	DPMSG_GENERIC_4 NetGen;

	if(planepnt == PlayerPlane)
	{
		if((planepnt->AI.Behaviorfunc == CTWaitingForLaunch) || (planepnt->AI.Behaviorfunc == CTDoCarrierLaunch))
		{
			return;
		}
	}

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage4Small(planepnt, arg1, arg2, arg3, arg4);
		return;
	}

	NetGen.type = NET_GENERIC_4;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;
	NetGen.arg4 = arg4;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage4()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		four BYTE argument.
 *
 */
void NetGetGenericMessage4( DPMSG_GENERIC_4 *Net)
{
	DPMSG_GENERIC_4_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;
	NetSmall.arg3 = Net->arg3;
	NetSmall.arg4 = Net->arg4;

	NetGetGenericMessage4Small(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage4Small()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		four BYTE argument.
 *
 */
void NetPutGenericMessage4Small(PlaneParams *planepnt, BYTE arg1, BYTE arg2, BYTE arg3, BYTE arg4)
{
	DPMSG_GENERIC_4_SM NetGen;

	NetGen.type = NET_GENERIC_4_SM;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;
	NetGen.arg4 = arg4;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage4Small()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		four BYTE argument.
 *
 */
void NetGetGenericMessage4Small( DPMSG_GENERIC_4_SM *Net )
{
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	PlaneParams *wingplane;
	PlaneParams *checkplane;
	int cnt;
	int workval;
	int j;

	switch(Net->arg1)
	{
		case GM4_START_ATTACK:
			PlaneParams *target, *threat;

			target = (Net->arg3 == 255) ? NULL : &Planes[Net->arg3];
			threat = (Net->arg4 == 255) ? NULL : &Planes[Net->arg3];
			NetStartAttack(&Planes[Net->slot], Net->arg2, target, threat);
			break;
		case GM4_DISENGAGE:
			planepnt = &Planes[Net->slot];
			if(Net->arg2 == 1)
			{
				ANGLE atemp1, atemp2;

				AISetFighterDisengage(planepnt);
				atemp1 = Net->arg3;
				atemp2 = Net->arg4;
				atemp1 <<= 8;
				planepnt->AI.DesiredHeading = atemp1 + atemp2;
			}
			else if(Net->arg2 == 4)
			{
				planepnt->AI.iAICombatFlags1 &= ~(AI_FINISH_BEHAVIOR);
				planepnt->AI.lCombatTimer = -1;
				planepnt->AI.CombatBehavior = NULL;
			}
			else
			{
				if(Net->arg2 == 2)
				{
					if (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
					{
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

				if((planepnt->AI.OrgBehave == NULL) || (Net->arg2 == 2) || (Net->arg2 == 6))
				{
					if(!((planepnt->AI.Behaviorfunc == CTWaitingForLaunch) || (planepnt->AI.Behaviorfunc == CTDoCarrierLaunch) || (planepnt->AI.Behaviorfunc == AIFlyTakeOff)))
					{
						planepnt->AI.Behaviorfunc = AIFlyFormation;
						planepnt->AI.OrgBehave = NULL;
					}
				}
				else if(Net->arg2 == 10)
				{
					AIChangeGroupBehavior(planepnt, planepnt->AI.Behaviorfunc, AIFlyFormation);
				}
				else
				{
					planepnt->AI.Behaviorfunc = planepnt->AI.OrgBehave;
				}

				if((Net->arg2 == 6) || (Net->arg2 == 7))
				{
					AISetUpNewWayPoint(planepnt);
				}
			}
			break;
		case GM4_SWITCH_ATTACK:
			planepnt = &Planes[Net->slot];
			if(Net->arg2 == 1)
			{
				planepnt->AI.Behaviorfunc = AIMissileBreakToHeading;
				return;
			}

			planepnt->AI.iAIFlags2 |= (AI_IN_DOGFIGHT);
			if(planepnt->AI.OrgBehave == NULL)
			{
				planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
			}
			switch(Net->arg2)
			{
				case 2:
					ANGLE atemp1, atemp2;

					planepnt->AI.lTimer1 = 300000 - (planepnt->AI.iSkill * 100000);
					atemp1 = Net->arg3;
					atemp2 = Net->arg4;
					atemp1 <<= 8;
					planepnt->AI.DesiredHeading = atemp1 + atemp2;
					planepnt->AI.Behaviorfunc = AIDisengage;
					leadplane = AIGetLeader(planepnt);
					AISetGroupDisengage(leadplane, leadplane);
					planepnt->AI.LinkedPlane = NULL;
					break;
				case 3:
					planepnt->AI.Behaviorfunc = AIAllOutAttack;
					break;
				case 4:
					planepnt->AI.Behaviorfunc = AIFluidFour;
					break;
				case 5:
					planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
					break;
				case 6:
					planepnt->AI.Behaviorfunc = AIGaggle;
					break;
				case 7:
					planepnt->AI.Behaviorfunc = AIDoubleAttack;
					break;
				case 8:
					planepnt->AI.Behaviorfunc = AILooseDeuce;
					break;
			}
			if(Net->arg3 & 1)
			{
				planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
			}

			if(Net->arg3 & 2)
			{
				planepnt->AI.iAIFlags2 |= AIMISSILEDODGE;
			}

			if(Net->arg4 == 255)
			{
				planepnt->AI.AirTarget = NULL;
				planepnt->AI.AirThreat = NULL;
			}
			else
			{
				if(Net->arg3 & 4)
				{
					planepnt->AI.AirTarget = NULL;
					planepnt->AI.AirThreat = &Planes[Net->arg4];
				}
				else
				{
					planepnt->AI.AirTarget = &Planes[Net->arg4];
				}
			}
			break;
		case GM4_SUPPORT:
			planepnt = &Planes[Net->slot];

			if(planepnt->AI.wingman >= 0)
			{
				wingplane = &Planes[planepnt->AI.wingman];
			}
			else if(planepnt->AI.winglead >= 0)
			{
				wingplane = &Planes[planepnt->AI.winglead];
			}
			else
			{
				wingplane = NULL;
			}

			if(Net->arg2 & 1)
			{
				planepnt->AI.iAIFlags2 |= AISUPPORT;
				if((wingplane) && (Net->arg2 & 0x8))
				{
					wingplane->AI.iAIFlags2 &= ~AISUPPORT;
				}
			}
			else
			{
				planepnt->AI.iAIFlags2 &= ~AISUPPORT;
				if((wingplane) && (Net->arg2 & 0x8))
				{
					wingplane->AI.iAIFlags2 |= AISUPPORT;
				}
			}

			if(Net->arg2 & 2)
			{
				planepnt->AI.iAIFlags2 |= AIDEFENSIVE;
			}
			else
			{
				planepnt->AI.iAIFlags2 &= ~AIDEFENSIVE;
			}

			if(Net->arg3 == 255)
			{
				planepnt->AI.AirTarget = NULL;
			}
			else
			{
				planepnt->AI.AirTarget = &Planes[Net->arg3];
			}

			if(wingplane)
			{
				if(Net->arg2 & 4)
				{
					wingplane->AI.iAIFlags2 |= AIDEFENSIVE;
				}
				else
				{
					wingplane->AI.iAIFlags2 &= ~AIDEFENSIVE;
				}
				if(Net->arg4 == 255)
				{
					wingplane->AI.AirTarget = NULL;
				}
				else
				{
					wingplane->AI.AirTarget = &Planes[Net->arg4];
				}
			}

			break;
		case GM4_WEAP_LOAD:
				planepnt = &Planes[Net->slot];
				planepnt->WeapLoad[Net->arg2].WeapId = Net->arg3;
				planepnt->WeapLoad[Net->arg2].Count = (Net->arg4 & 0x3F);
				planepnt->WeapLoad[Net->arg2].bNumPods = ((Net->arg4 & 0xC0)>> 6);

				for(cnt = 0; cnt < MAX_WEAPON_TYPES; cnt ++)
				{
					if(planepnt->WeapLoad[Net->arg2].WeapId == WeaponTypes[cnt].TypeNumber)
					{
						planepnt->WeapLoad[Net->arg2].Type = &WeaponTypes[cnt];
						break;
					}
					else if(WeaponTypes[cnt].TypeNumber == -1)
					{
						break;
					}
				}
				if(cnt == MAX_WEAPON_TYPES)
				{
					planepnt->WeapLoad[Net->arg2].Count = 0;
					planepnt->WeapLoad[Net->arg2].Type = NULL;
					return;
				}
				else if(WeaponTypes[cnt].TypeNumber == -1)
				{
					planepnt->WeapLoad[Net->arg2].Count = 0;
					planepnt->WeapLoad[Net->arg2].Type = NULL;
					return;
				}

				planepnt->WeapLoad[Net->arg2].WeapIndex = -1;
				for (j=0; j<iNumWeaponList; j++)
				{
					if (pDBWeaponList[j].lWeaponID == Net->arg3)
					{
						planepnt->WeapLoad[Net->arg2].WeapIndex = j;
						break;
					}
				}

				if(planepnt->WeapLoad[Net->arg2].WeapIndex == -1)
				{
					planepnt->WeapLoad[Net->arg2].Count = 0;
					planepnt->WeapLoad[Net->arg2].Type = NULL;
					return;
				}

				workval = NetGetSlotFromPlaneIndex(Net->slot);
				if(workval == Net->slot)
				{
					if(workval >= MAX_HUMANS)
						return;
					if(iSlotToPlane[workval] != workval)
						return;
				}
				NetRegenPlanes[workval].WeapLoad[Net->arg2] = planepnt->WeapLoad[Net->arg2];
				break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetVectorIntercept()
 *
 *		Send client AIs to intercept a target.
 *
 */
void NetVectorIntercept(BYTE planenum, BYTE targetnum)
{
	PlaneParams *foundplane = &Planes[planenum];
	PlaneParams *target = &Planes[targetnum];
	PlaneParams *leadplane;

	leadplane = AIGetLeader(foundplane);

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
}

/*----------------------------------------------------------------------------
 *
 *	NetStartAttack()
 *
 *		Have flight begin attack on bandits on client machines.
 *
 */
void NetStartAttack(PlaneParams *planepnt, int attacktype, PlaneParams *target, PlaneParams *threat)
{
	PlaneParams *wingplane;
	float tdist, dx, dy, dz;

	planepnt->AI.AirTarget = target;
	planepnt->AI.AirThreat = threat;

	switch(attacktype)
	{
		case 1:
			planepnt->AI.Behaviorfunc = AIAllOutAttack;
			break;
		case 2:
			planepnt->AI.Behaviorfunc = AIFluidFour;
			break;
		case 3:
			planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
			break;
		case 4:
			planepnt->AI.Behaviorfunc = AIGaggle;
			break;
		case 5:
			planepnt->AI.Behaviorfunc = AIDoubleAttack;
			break;
		case 6:
			planepnt->AI.Behaviorfunc = AILooseDeuce;
			break;
		case 7:
			planepnt->AI.Behaviorfunc = AIFightingWing;  //  AILooseDeuce;
			break;
		case 8:
			planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
			break;
	}

	planepnt->AI.iAIFlags2 &= ~AISUPPORT;

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
			wingplane->AI.iAIFlags2 |= AISUPPORT;
			wingplane->AI.lTimer1 = planepnt->AI.lTimer1;
		}

		if(wingplane->AI.AirTarget == NULL)
		{
			if(planepnt->AI.AirTarget != NULL)
			{
				if((wingplane->AI.AirTarget != planepnt->AI.AirTarget) && (!(wingplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					wingplane->AI.lCombatTimer = -1;
					wingplane->AI.CombatBehavior = NULL;
				}

				wingplane->AI.AirTarget = planepnt->AI.AirTarget;
			}
			else if(planepnt->AI.AirThreat != NULL)
			{
				if((wingplane->AI.AirTarget != planepnt->AI.AirThreat) && (!(wingplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
				{
					wingplane->AI.lCombatTimer = -1;
					wingplane->AI.CombatBehavior = NULL;
				}

				wingplane->AI.AirTarget = planepnt->AI.AirThreat;
			}
			if(wingplane->AI.AirTarget != NULL)
			{
				if(wingplane->AI.AirTarget != NULL)
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

void NetAdjustCAPLegs(PlaneParams *planepnt, BYTE capleg)
{
	double angxx, angxz, angzx, angzz;
	double heading;
	PlaneParams *wingplane;
	float xoff, zoff;

	heading = DegToRad((double)planepnt->AI.iVar1);
	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	switch(capleg)
	{
		case GM3_CAP_1:
			xoff = 1.5 * NMTOFT;
			zoff = -8 * NMTOFT;
			planepnt->AI.Behaviorfunc = AIFlyCAPSide1;
			planepnt->AI.lDesiredSpeed = 400;
			break;
		case GM3_CAP_2:
			xoff = -1.5 * NMTOFT;
			zoff = -6 * NMTOFT;
			planepnt->AI.Behaviorfunc = AIFlyCAPSide2;
			planepnt->AI.lDesiredSpeed = 400;
			break;
		case GM3_CAP_3:
			xoff = -1.5 * NMTOFT;
			zoff = 8 * NMTOFT;
			planepnt->AI.Behaviorfunc = AIFlyCAPSide3;
			planepnt->AI.lDesiredSpeed = 400;
			break;
		case GM3_CAP_4:
			xoff = 1.5 * NMTOFT;
			zoff = 6 * NMTOFT;
			planepnt->AI.Behaviorfunc = AIFlyCAPSide4;
			planepnt->AI.lDesiredSpeed = 400;
			break;
	}

	planepnt->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
	planepnt->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
	planepnt->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));

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
		wingplane->AI.iVar1 = planepnt->AI.iVar1;
		wingplane->AI.lVar2 = planepnt->AI.lVar2;
		switch(capleg)
		{
			case GM3_CAP_1:
				// Going HOT
				if(wingplane->Status & PL_DEVICE_DRIVEN)
				{
					AICGoingHotMsg(planepnt);
				}

				if(wingplane->AI.Behaviorfunc == planepnt->AI.Behaviorfunc)
				{
					wingplane->AI.lDesiredSpeed = 450;
				}
				return;
				break;
			case GM3_CAP_2:
				//  Going COLD
				if(wingplane->Status & PL_DEVICE_DRIVEN)
				{
					AICGoingColdMsg(planepnt);
				}
				xoff = 1.5 * NMTOFT;
				zoff = -8 * NMTOFT;
				wingplane->AI.Behaviorfunc = AIFlyCAPSide1;
				break;
			case GM3_CAP_3:
				return;
				break;
			case GM3_CAP_4:
				return;
				break;
		}

		wingplane->AI.TargetPos.X = planepnt->AI.WayPosition.X + ConvertWayLoc((angxx * xoff) + (angzx * zoff));
		wingplane->AI.TargetPos.Y = planepnt->AI.WayPosition.Y;
		wingplane->AI.TargetPos.Z = planepnt->AI.WayPosition.Z + ConvertWayLoc(-(angxz * xoff) + (angzz * zoff));
		wingplane->AI.lDesiredSpeed = 300;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDogfightUpdate()
 *
 *		Update Combat Behavior and lCombatTimer
 *
 */
void NetPutDogfightUpdate(PlaneParams *planepnt, BYTE behaveid, long ltimer)
{
	DPMSG_DOGFIGHT_UPDATE NetUpdate;

	if(iMultiCast >= 0)
	{
		NetPutDogfightUpdateSmall(planepnt, behaveid, ltimer);
		return;
	}

	NetUpdate.type = NET_DF_UPDATE;
	NetUpdate.time   = newTime;
	NetUpdate.slot	= planepnt - Planes;
	NetUpdate.behavior = behaveid;
	NetUpdate.lTimer = ltimer;

	planepnt->AI.cMultiLastCombatBehaveID = behaveid;
	planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_SEND_CPOS;

	NetAddToBigPacket(MySlot, &NetUpdate, sizeof(NetUpdate) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDogfightUpdate()
 *
 *		Update Combat Behavior and lCombatTimer
 *
 */
void NetGetDogfightUpdate( DPMSG_DOGFIGHT_UPDATE *Net)
{
	DPMSG_DOGFIGHT_UPDATE_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.behavior = Net->behavior;
	NetSmall.lTimer = Net->lTimer;

	NetGetDogfightUpdateSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDogfightUpdateSmall()
 *
 *		Update Combat Behavior and lCombatTimer
 *
 */
void NetPutDogfightUpdateSmall(PlaneParams *planepnt, BYTE behaveid, long ltimer)
{
	DPMSG_DOGFIGHT_UPDATE_SM NetUpdate;

	NetUpdate.type = NET_DF_UPDATE_SM;
	NetUpdate.slot	= planepnt - Planes;
	NetUpdate.behavior = behaveid;
	NetUpdate.lTimer = ltimer;

	planepnt->AI.cMultiLastCombatBehaveID = behaveid;
	planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_SEND_CPOS;

	NetAddToBigPacket(MySlot, &NetUpdate, sizeof(NetUpdate) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDogfightUpdateSmall()
 *
 *		Update Combat Behavior and lCombatTimer
 *
 */
void NetGetDogfightUpdateSmall( DPMSG_DOGFIGHT_UPDATE_SM *Net )
{
	PlaneParams *planepnt;

	planepnt = &Planes[Net->slot];

	planepnt->AI.lCombatTimer = Net->lTimer;
	NetUpdateCombatBehavior(planepnt, Net->behavior);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDogfightCPosUpdate()
 *
 *		Update Combat Behavior, lCombatTimer and Combat Position
 *
 */
void NetPutDogfightCPosUpdate(PlaneParams *planepnt, BYTE behaveid, long ltimer, FPoint cpos)
{
	DPMSG_DOGFIGHT_UPDATE_CPOS NetUpdate;

	if(iMultiCast >= 0)
	{
		NetPutDogfightCPosUpdateSmall(planepnt, behaveid, ltimer, cpos);
		return;
	}

	NetUpdate.type = NET_DF_UPDATE_CPOS;
	NetUpdate.time   = newTime;
	NetUpdate.slot	= planepnt - Planes;
	NetUpdate.behavior = behaveid;
	NetUpdate.lTimer = ltimer;
	NetUpdate.CombatPos = cpos;

	planepnt->AI.cMultiLastCombatBehaveID = behaveid;
	planepnt->AI.iAICombatFlags1 |= AI_MULTI_SEND_CPOS;

	NetAddToBigPacket(MySlot, &NetUpdate, sizeof(NetUpdate) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDogfightCPosUpdate()
 *
 *		Update Combat Behavior, lCombatTimer and Combat Position
 *
 */
void NetGetDogfightCPosUpdate( DPMSG_DOGFIGHT_UPDATE_CPOS *Net)
{
	DPMSG_DOGFIGHT_UPDATE_CPOS_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.behavior = Net->behavior;
	NetSmall.lTimer = Net->lTimer;
	NetSmall.CombatPos = Net->CombatPos;

	NetGetDogfightCPosUpdateSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDogfightCPosUpdateSmall()
 *
 *		Update Combat Behavior, lCombatTimer and Combat Position
 *
 */
void NetPutDogfightCPosUpdateSmall(PlaneParams *planepnt, BYTE behaveid, long ltimer, FPoint cpos)
{
	DPMSG_DOGFIGHT_UPDATE_CPOS_SM NetUpdate;

	NetUpdate.type = NET_DF_UPDATE_CPOS_SM;
	NetUpdate.slot	= planepnt - Planes;
	NetUpdate.behavior = behaveid;
	NetUpdate.lTimer = ltimer;
	NetUpdate.CombatPos = cpos;

	planepnt->AI.cMultiLastCombatBehaveID = behaveid;
	planepnt->AI.iAICombatFlags1 |= AI_MULTI_SEND_CPOS;

	NetAddToBigPacket(MySlot, &NetUpdate, sizeof(NetUpdate) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDogfightUpdateSmall()
 *
 *		Update Combat Behavior, lCombatTimer and Combat Position
 *
 */
void NetGetDogfightCPosUpdateSmall( DPMSG_DOGFIGHT_UPDATE_CPOS_SM *Net )
{
	PlaneParams *planepnt;

	planepnt = &Planes[Net->slot];

	planepnt->AI.lCombatTimer = Net->lTimer;
	planepnt->AI.CombatPos = Net->CombatPos;
	NetUpdateCombatBehavior(planepnt, Net->behavior);
}

/*----------------------------------------------------------------------------
 *
 *	NetUpdateCombatBehavior()
 *
 *		General Function to update Combat Behaviors
 *
 */
void NetUpdateCombatBehavior(PlaneParams *planepnt, BYTE behaviorid)
{
	switch(behaviorid)
	{
		case 0:
 			planepnt->AI.CombatBehavior = NULL;
			break;
		case 1:
 			planepnt->AI.CombatBehavior = AIBarrelAvoid;
			break;
		case 2:
			planepnt->AI.CombatBehavior = AILowYoYo;
			break;
		case 3:
			planepnt->AI.CombatBehavior = AIHighYoYo;
			break;
		case 4:
			planepnt->AI.CombatBehavior = AITurnAway;
			break;
		case 9:
			if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
			{
				PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
				lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
				iLastWSOWarnType = TARGET_PLANE;
				pLastWSOWarnObj = planepnt;
				AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_EXTENDING, 3000, 50, AICF_ENEMY_EXTENDING);
			}
			/*  NO BREAK;  */
		case 5:	/* NOTE, 9 CONTINUES HERE AS WELL  */
			planepnt->AI.CombatBehavior = AIExtend;
			break;
		case 11:
			planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;
			/*  NO BREAK;  */
		case 6:	/* NOTE, 11 CONTINUES HERE AS WELL  */
			planepnt->AI.CombatBehavior = AIImmelmann;
			if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
			{
				PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
				lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
				iLastWSOWarnType = TARGET_PLANE;
				pLastWSOWarnObj = planepnt;
				AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 3000, 50, AICF_ENEMY_VERTICLE);
			}
			break;
		case 7:
			planepnt->AI.CombatBehavior = AISplitS;
			if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
			{
				PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
				lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
				iLastWSOWarnType = TARGET_PLANE;
				pLastWSOWarnObj = planepnt;
				AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_WEEDS, 3000, 50, AICF_ENEMY_WEEDS);
			}
			break;
		case 8:
			planepnt->AI.CombatBehavior = AICuban8;
			if((PlayerPlane->AI.lSpeechOverloadTimer < 0) && (lDogSpeechTimer < 0) && (PlayerPlane->AI.AirThreat == planepnt) && (planepnt->AI.AirTarget == PlayerPlane))
			{
				PlayerPlane->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
				lDogSpeechTimer = DOGFIGHT_SPEECH_DELAY;
				iLastWSOWarnType = TARGET_PLANE;
				pLastWSOWarnObj = planepnt;
				AICAddSoundCall(AICDoWSONoVariablesMsg, AIC_ENEMY_VERTICLE, 1000, 50, AICF_ENEMY_VERTICLE);
			}
			break;
		case 10:
			planepnt->AI.CombatBehavior = AIStartLagPursuitRoll;
			break;
		case 12:
			planepnt->AI.iAICombatFlags1 |= AI_FINISH_BEHAVIOR;
		case 17:
			planepnt->AI.CombatBehavior = AIPitchAndTurn;
			break;
		case 13:
			planepnt->AI.CombatBehavior = AIPrepareLeadTurn;
			break;
		case 14:
			planepnt->AI.CombatBehavior = AIJoust;
			break;
		case 15:
		case 16:
			if(behaviorid == 15)
			{
				planepnt->AI.iAICombatFlags1 |= AI_BLOW_THROUGH;
			}
			else
			{
				planepnt->AI.iAICombatFlags1 &= ~AI_BLOW_THROUGH;
			}
			planepnt->AI.CombatBehavior = AIGetSeperation;
			break;
		case 18:
			planepnt->AI.CombatBehavior = AIDoLeadTurn;
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetUpdateCombatBehavior()
 *
 *		General Function to update Combat Behaviors
 *
 */
void NetUpdateSupport(PlaneParams *planepnt, int support, PlaneParams *wingman)
{
	PlaneParams *wingplane;
	BYTE arg2, arg3, arg4;

	if(support)
	{
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 |= AISUPPORT;
			if(wingman)
				wingman->AI.iAIFlags2 &= ~AISUPPORT;
		}

		if(planepnt->AI.iAIFlags2 & AISUPPORT)
		{
			return;
		}
		arg2 = 0;
	}
	else
	{
		if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
		{
			planepnt->AI.iAIFlags2 &= ~AISUPPORT;
			if(wingman)
				wingman->AI.iAIFlags2 |= AISUPPORT;
		}

		if(!(planepnt->AI.iAIFlags2 & AISUPPORT))
		{
			return;
		}
		arg2 = 1;
	}

	if((!MultiPlayer) || (!(planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
	{
		return;
	}

	if(planepnt->AI.iAIFlags2 & AIDEFENSIVE)
	{
		arg2 |= 0x2;
	}

	arg3 = (planepnt->AI.AirTarget) ? (planepnt->AI.AirTarget - Planes) : 255;

	if(wingman)
	{
		wingplane = wingman;
	}
	else if(planepnt->AI.wingman >= 0)
	{
		wingplane = &Planes[planepnt->AI.wingman];
	}
	else if(planepnt->AI.winglead >= 0)
	{
		wingplane = &Planes[planepnt->AI.winglead];
	}
	else
	{
		wingplane = NULL;
	}

	if(wingplane)
	{
		if(wingplane->AI.iAIFlags2 & AIDEFENSIVE)
		{
			arg2 |= 0x4;
		}
		arg4 = (wingplane->AI.AirTarget) ? (wingplane->AI.AirTarget - Planes) : 255;
		if(wingman)
		{
			arg2 |= 0x8;
		}
	}
	else
	{
		arg4 = 255;
	}

	NetPutGenericMessage4(planepnt, GM4_SUPPORT, arg2, arg3, arg4);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutMissileBreak()
 *
 *		Tell a client it should be avoiding a missile
 *
 */
void NetPutMissileBreak(PlaneParams *planepnt, BYTE breakid, WeaponParams *weaponpnt)
{
	DPMSG_MISSILE_BREAK NetBreak;

	if(iMultiCast >= 0)
	{
		NetPutMissileBreakSmall(planepnt, breakid, weaponpnt);
		return;
	}

	NetBreak.type = NET_MISSILE_BREAK;
	NetBreak.slot = planepnt - Planes;
	NetBreak.time = newTime;
	NetBreak.breakid = breakid;
	NetBreak.new_heading = planepnt->AI.DesiredHeading;
	NetBreak.weapIX = weaponpnt->iNetID;

	NetAddToBigPacket(MySlot, &NetBreak, sizeof(NetBreak) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMissileBreak()
 *
 *		Tell a client it should be avoiding a missile
 *
 */
void NetGetMissileBreak( DPMSG_MISSILE_BREAK *Net)
{
	DPMSG_MISSILE_BREAK_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.breakid = Net->breakid;
	NetSmall.new_heading = Net->new_heading;
	NetSmall.weapIX = Net->weapIX;

	NetGetMissileBreakSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutMissileBreakSmall()
 *
 *		Tell a client it should be avoiding a missile
 *
 */
void NetPutMissileBreakSmall(PlaneParams *planepnt, BYTE breakid, WeaponParams *weaponpnt)
{
	DPMSG_MISSILE_BREAK_SM NetBreak;

	NetBreak.type = NET_MISSILE_BREAK_SM;
	NetBreak.slot = planepnt - Planes;
	NetBreak.breakid = breakid;
	NetBreak.new_heading = planepnt->AI.DesiredHeading;
	NetBreak.weapIX = weaponpnt->iNetID;

	NetAddToBigPacket(MySlot, &NetBreak, sizeof(NetBreak) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMissileBreakSmall()
 *
 *		Tell a client it should be avoiding a missile
 *
 */
void NetGetMissileBreakSmall( DPMSG_MISSILE_BREAK_SM *Net)
{
	PlaneParams *planepnt = &Planes[Net->slot];

	planepnt->AI.DesiredHeading = Net->new_heading;

	if(Net->breakid != 16)
	{
		if(planepnt->AI.iAICombatFlags1 & (AI_FUEL_TANKS))
		{
			AIDropTanks(planepnt);
		}

		if(planepnt->AI.OrgBehave == NULL)
		{
			planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
			planepnt->AI.iAIFlags2 |= AI_CONTINUE_ORG_BEHAVE;
		}
		else if(Net->breakid & 1)
		{
			planepnt->AI.iAIFlags2 |= AI_CONTINUE_ORG_BEHAVE;
		}

		planepnt->AI.lTimer2 = 7000;
		if(Net->breakid & 2)
		{
			planepnt->AI.iAIFlags2 |= AI_IN_DOGFIGHT;
			planepnt->AI.iAIFlags2 |= AIMISSILEDODGE;
		}
		else if(Net->breakid & 4)
		{
			planepnt->AI.Behaviorfunc = AIMissileBreakToHeading;
		}
//		else
//		{
//			AISwitchToAttack(planepnt, 1, 1);
//		}
	}
	else
	{
		planepnt->AI.iAIFlags2 |= AIMISSILEDODGE;
	}

	WeaponParams *W = MAIGetWeaponPointer(Net->weapIX);

	if(W)
	{
		planepnt->AI.iMissileThreat = W - Weapons;
	}
}

void NetUpdateTarget(DPMSG_GENERIC_2_SM *Net)
{
	PlaneParams *planepnt;
	PlaneParams *orgtarget;
	int ifoundplane;
	int inburner = 0;
	float targbearing;
	float targpitch;
	float foundrange;
	float dx, dy, dz, tdist;
	PlaneParams *checkplane;
	PlaneParams *leadplane;
	float visualrange;
	int isnight = 0;
	int hours;
	float visualrangenm;

	planepnt = &Planes[Net->slot];
	orgtarget = planepnt->AI.AirTarget;

	checkplane = planepnt->AI.AirTarget = (Net->arg2 == 255) ? NULL : &Planes[Net->arg2];

	if(!planepnt->AI.AirTarget)
	{
		planepnt->AI.iAIFlags2 &= ~AILOCKEDUP;
		planepnt->AI.AirTarget = NULL;
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
		return;
	}

	if((Net->arg1 == GM2_NEW_TARGET_PING) && (planepnt->AI.AirTarget))
	{
		GDCheckForPingReport(NULL, planepnt->AI.AirTarget, planepnt);
	}

	dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
	dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
	dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
	foundrange = tdist = QuickDistance(dx, dz);
	targbearing = atan2(-dx, -dz) * 57.2958;
	targpitch = (atan2(dy, tdist) * 57.2958);

	if(Net->arg1 & 1)
	{
		planepnt->AI.fRadarAngleYaw = targbearing;
		planepnt->AI.fRadarAnglePitch = targpitch;
	}
	else
	{
		planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
		planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
	}

	if((orgtarget == NULL) && (planepnt->AI.AirTarget))
	{
		ifoundplane = -1;

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
	}
}

/*----------------------------------------------------------------------------
 *
 *	GeneralSetNewAirTarget()
 *
 *		Sets AirTarget for a plane and handles Network messages and conditions.
 *
 */
PlaneParams *GeneralSetNewAirTarget(PlaneParams *planepnt, PlaneParams *target)
{
	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirTarget != target))
		{
			NetPutGenericMessage2(planepnt, GM2_NEW_TARGET, (BYTE)((target) ? (target - Planes) : 255));
		}
		planepnt->AI.AirTarget = target;
	}

	return(target);
}

/*----------------------------------------------------------------------------
 *
 *	GeneralSetNewAirThreat()
 *
 *		Sets AirThreat for a plane and handles Network messages and conditions.
 *
 */
PlaneParams *GeneralSetNewAirThreat(PlaneParams *planepnt, PlaneParams *threat)
{
	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
#if 0
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.AirThreat != threat))
		{
			NetPutGenericMessage2(planepnt, GM2_NEW_THREAT, (BYTE)((threat) ? (threat - Planes) : 255));
		}
#endif
		planepnt->AI.AirThreat = threat;
	}

	return(threat);
}

void MAICheckFlags(PlaneParams *planepnt)
{
	BYTE checkflag = 0;
	BYTE orgflag = 0;

	checkflag |= (planepnt->AI.iAIFlags2 & AISUPPORT) ? MC_SUPPORT : 0;
	checkflag |= (planepnt->AI.iAIFlags2 & AIDEFENSIVE) ? MC_DEFENSIVE : 0;
	checkflag |= (planepnt->AI.iAIFlags2 & AIMISSILEDODGE) ? MC_MISSILE_AVOID : 0;
	checkflag |= (planepnt->AI.iAIFlags2 & AI_IN_DOGFIGHT) ? MC_DOGFIGHT : 0;
	checkflag |= (planepnt->AI.CombatBehavior) ? MC_COMBAT_BEHAVE : 0;
	checkflag |= (planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA) ? MC_WINCHESTER_AA : 0;
	checkflag |= (planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AG) ? MC_WINCHESTER_AG : 0;
	orgflag = checkflag;

	if(checkflag != planepnt->AI.cMultiCheckFlags)
	{
		if(checkflag & ~(planepnt->AI.cMultiCheckFlags) & MC_MISSILE_AVOID)
		{
			checkflag &= ~(MC_MISSILE_AVOID);
			if(planepnt->AI.iMissileThreat != -1)
			{
				NetPutMissileBreak(planepnt, 16, &Weapons[planepnt->AI.iMissileThreat]);
			}
		}
		if(checkflag != planepnt->AI.cMultiCheckFlags)
		{
			NetPutGenericMessage2(planepnt, GM2_MULTI_FLAGS, (BYTE)(checkflag));
		}
	}
	planepnt->AI.cMultiCheckFlags = orgflag;
}

/*----------------------------------------------------------------------------
 *
 *	GeneralClearCombatBehavior()
 *
 *		Clear CombatBehavior and handles Network messages and conditions.
 *
 */
void GeneralClearCombatBehavior(PlaneParams *planepnt)
{
	if((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if((MultiPlayer) && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) && (planepnt->AI.CombatBehavior))
		{
			NetPutGenericMessage4(planepnt, GM4_DISENGAGE, 4, 0, 0);
		}
		planepnt->AI.lCombatTimer = -1;
		planepnt->AI.CombatBehavior = NULL;
		planepnt->AI.iAICombatFlags1 &= ~(AI_FINISH_BEHAVIOR);
	}
	return;
}

/*----------------------------------------------------------------------------
 *
 *	NetGetSlotFromPlaneIndex()
 *
 *		Determine slot[0-7] number from index of plane in Planes array.
 *
 */
int NetGetSlotFromPlaneIndex(int planenum, int showerr)
{
	int cnt;

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if(iSlotToPlane[cnt] == planenum)
		{
			return(cnt);
		}
	}

	if(showerr)
	{
		return(-1);
	}
	return(planenum);
}

/*----------------------------------------------------------------------------
 *
 *	NetGetPlaneIndexFromSlot()
 *
 *		Determine index of plane in Planes array from slot[0-7] number.
 *
 */
int NetGetPlaneIndexFromSlot(int slotnum, int showerr)
{
	if(iSlotToPlane[slotnum] == -1)
	{
		if(showerr)
		{
			return(-1);
		}
		return(slotnum);
	}
	else
	{
		return(iSlotToPlane[slotnum]);
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutWeaponGExplode()
 *
 *		Send a message so we can duplicate the same explosion that
 *		is happening on another machine.
 *
 */
void NetPutWeaponGExplode(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type,BOOL skip_graphic)
{
	DPMSG_WEAPON_G_EXPLODE NetExp;

	if(iMultiCast >= 0)
	{
		NetPutWeaponGExplodeSmall(pweapon, do_big_bang, position, pweapon_type, skip_graphic);
		return;
	}

	NetExp.type = NET_WEAPON_G_EXPLODE;
	NetExp.time   = newTime;
	NetExp.slot	= PlayerPlane - Planes;
	if(position)
	{
		NetExp.fpWorldPosition.X = position->X;
		NetExp.fpWorldPosition.Y = position->Y;
		NetExp.fpWorldPosition.Z = position->Z;
	}
	else if(pweapon)
	{
		NetExp.fpWorldPosition = pweapon->Pos;
	}
	else
	{
		return;
	}

	if(pweapon)
	{
		if(pweapon->P)
		{
			NetExp.launcher	= pweapon->P - Planes;
		}
		else
		{
			NetExp.launcher = 255;
		}
	}

	NetExp.bFlags = 0;
	if(do_big_bang)
	{
		NetExp.bFlags |= GX_DO_BIG_BANG;
	}
	if(skip_graphic)
	{
		NetExp.bFlags |= GX_SKIP_GRAPHIC;
	}
	if(pweapon)
	{
		if(pweapon->P == PlayerPlane)
		{
			if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_BOMB_ACCURACY))
			{
				NetExp.bFlags |= GX_FAKE_ACCURACY;
			}
			if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))
			{
				NetExp.bFlags |= GX_FAKE_DAMAGE;
			}
		}
		if ((int)pweapon->Type < 0x200)
			NetExp.wtype = AIGetWeaponIndex((int)pweapon->Type);
		else
			NetExp.wtype = AIGetWeaponIndex(pweapon->Type->TypeNumber);

		NetExp.timer = GameLoop - (DWORD)pweapon->lTimer;
	}
	else if(pweapon_type)
	{
		NetExp.wtype = pweapon_type - pDBWeaponList;
		NetExp.timer = -1;
	}

	iClusterRandSeed = rand() & 0xFF;

	sClusterTimer = NetExp.timer;

	NetExp.randseed = (BYTE)iClusterRandSeed;

	NetAddToBigPacket(MySlot, &NetExp, sizeof(NetExp) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetWeaponGExplode()
 *
 *		Duplicate the same explosion that is happening on another machine.
 *
 */
void NetGetWeaponGExplode( DPMSG_WEAPON_G_EXPLODE *Net)
{
	DPMSG_WEAPON_G_EXPLODE_SM NetSmall;

	NetSmall.launcher = Net->launcher;
	NetSmall.wtype = Net->wtype;
	NetSmall.fpWorldPosition = Net->fpWorldPosition;
	NetSmall.bFlags = Net->bFlags;

	NetGetWeaponGExplodeSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutWeaponGExplodeSmall()
 *
 *		Send a message so we can duplicate the same explosion that
 *		is happening on another machine.
 *
 */
void NetPutWeaponGExplodeSmall(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type,BOOL skip_graphic)
{
	DPMSG_WEAPON_G_EXPLODE_SM NetExp;

	NetExp.type = NET_WEAPON_G_EXPLODE_SM;

	if(pweapon)
	{
		if(pweapon->P)
		{
			NetExp.launcher	= pweapon->P - Planes;
		}
		else
		{
			NetExp.launcher = 255;
		}
	}

	if(position)
	{
		NetExp.fpWorldPosition.X = position->X;
		NetExp.fpWorldPosition.Y = position->Y;
		NetExp.fpWorldPosition.Z = position->Z;
	}
	else if(pweapon)
	{
		NetExp.fpWorldPosition = pweapon->Pos;
	}
	else
	{
		return;
	}

	NetExp.bFlags = 0;
	if(do_big_bang)
	{
		NetExp.bFlags |= GX_DO_BIG_BANG;
	}
	if(skip_graphic)
	{
		NetExp.bFlags |= GX_SKIP_GRAPHIC;
	}
	if(pweapon)
	{
		if(pweapon->P == PlayerPlane)
		{
			if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_BOMB_ACCURACY))
			{
				NetExp.bFlags |= GX_FAKE_ACCURACY;
			}
			if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))
			{
				NetExp.bFlags |= GX_FAKE_DAMAGE;
			}
		}
		if ((int)pweapon->Type < 0x200)
			NetExp.wtype = AIGetWeaponIndex((int)pweapon->Type);
		else
			NetExp.wtype = AIGetWeaponIndex(pweapon->Type->TypeNumber);

		NetExp.timer = GameLoop - (DWORD)pweapon->lTimer;
	}
	else if(pweapon_type)
	{
		NetExp.wtype = pweapon_type - pDBWeaponList;
		NetExp.timer = -1;
	}

	iClusterRandSeed = rand() & 0xFF;

	sClusterTimer = NetExp.timer;

	NetExp.randseed = (BYTE)iClusterRandSeed;

	NetAddToBigPacket(MySlot, &NetExp, sizeof(NetExp) );

#ifdef _DEBUG
	char tstr[256];
	sprintf(tstr, "Put Explode %u, bFlags %x, X %f, Y %f, Z %f, Launcher %d, Weap %d \n\r", iClusterRandSeed, NetExp.bFlags, NetExp.fpWorldPosition.X, NetExp.fpWorldPosition.Y, NetExp.fpWorldPosition.Z, NetExp.launcher, NetExp.wtype);

	OutputDebugString( tstr );
#endif
}

/*----------------------------------------------------------------------------
 *
 *	NetGetWeaponGExplodeSmall()
 *
 *		Duplicate the same explosion that is happening on another machine.
 *
 */
void NetGetWeaponGExplodeSmall( DPMSG_WEAPON_G_EXPLODE_SM *Net )
{
	WeaponParams nweapon;
	BOOL do_big_bang = 0;
	BOOL skip_graphic = 0;

	nweapon.iNetID = 0;  //  May pass this across, but for now just make 0;
	nweapon.Pos = Net->fpWorldPosition;
	nweapon.Smoke = NULL;

	if(Net->launcher == 255)
	{
		nweapon.P = NULL;
		nweapon.LauncherType = GROUNDOBJECT;
		nweapon.Launcher = NULL;
	}
	else
	{
		nweapon.P = &Planes[Net->launcher];
		nweapon.Launcher = nweapon.P;
		nweapon.LauncherType = AIRCRAFT;
	}

	nweapon.Type = (WeaponType *)pDBWeaponList[Net->wtype].lWeaponID;

	if(Net->bFlags & GX_DO_BIG_BANG)
	{
		do_big_bang = 1;
	}
	if(Net->bFlags & GX_SKIP_GRAPHIC)
	{
		skip_graphic = 1;
	}

	iClusterRandSeed = Net->randseed;

	sClusterTimer = Net->timer;

#ifdef _DEBUG
	char tstr[256];
	sprintf(tstr, "Got Explode %u, bFlags %x, X %f, Y %f, Z %f, Launcher %d, Weap %d \n\r", iClusterRandSeed, Net->bFlags, Net->fpWorldPosition.X, Net->fpWorldPosition.Y, Net->fpWorldPosition.Z, Net->launcher, Net->wtype);

	OutputDebugString( tstr );
#endif

	netCall = 1 + (Net->bFlags & (GX_FAKE_ACCURACY|GX_FAKE_DAMAGE));	// prevent recursion for remote procedure calls
	DoExplosionOnGround(&nweapon, do_big_bang, NULL, NULL, skip_graphic);
	netCall = 0;	// prevent recursion for remote procedure calls

#ifdef _DEBUG
	sprintf(tstr, "After Explode %u \n\r", iClusterRandSeed);

	OutputDebugString( tstr );
#endif
}

/*----------------------------------------------------------------------------
 *
 *	MAINetDebugMessage()
 *
 *		Writes a debug message to netmlog.txt to let me know what sort of messages
 *		have been sent.
 *
 */
void MAINetDebugMessage(DPMSG_F18GENERIC *lpMsg, int dpID, int size)
{
	char tstr[256];
	switch (lpMsg->type)
	{
		case NET_PLANEDATA:
			sprintf(tstr, "Plane data to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_CHATTER:
			sprintf(tstr, "Chatter to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_PAUSED:
			sprintf(tstr, "Paused to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_MISSILE:
			sprintf(tstr, "Missile to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_TARGET:
			sprintf(tstr, "Target to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_DAMAGE:
			sprintf(tstr, "Damage to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_DECOY:
			sprintf(tstr, "Decoy to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_EJECT:
			sprintf(tstr, "Eject to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_CRASH:
			sprintf(tstr, "Crash to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_STRIKE:
			sprintf(tstr, "Strike to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_AI_POS:
			sprintf(tstr, "AI Pos to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_MISSILE_GENERAL:
			sprintf(tstr, "Missile General to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_GROUND_LOCK:
			sprintf(tstr, "Ground Lock to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_BOMB_GENERAL:
			sprintf(tstr, "Bomb General to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_AG_MISSILE_GENERAL:
			sprintf(tstr, "AG Missile General to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_SLOTREQUEST:
			sprintf(tstr, "Slot Request to %d size %d time %d", dpID, size, lpMsg->time);
			break;
		case NET_BIG_PACKET:
			sprintf(tstr, "BIG PACKET *** to %d size %d time %d", dpID, size, lpMsg->time);

			if(hNetDebugFile != -1)
			{
				_write(hNetDebugFile,tstr,(strlen(tstr)));  //  + 1

				sprintf(tstr, "\n");
				_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1
		//		OutputDebugString( tstr );
		//		OutputDebugString("\n\r");
			}

			MAINetDebugBigPacket((DPMSG_BIG_PACKET *) lpMsg);

			sprintf(tstr, "Done Big Packet ***");
			break;
		default:
			sprintf(tstr, "Unknown %d to %d size %d time %d", lpMsg->type, dpID, size, lpMsg->time);
			break;
	}
	if(hNetDebugFile != -1)
	{
		_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1

		sprintf(tstr, "\n");
		_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1

//		OutputDebugString( tstr );
//		OutputDebugString("\n\r");
	}
}

/*----------------------------------------------------------------------------
 *
 *	MAINetDebugBigPacket()
 *
 *		Writes a debug message to netmlog.txt to let me know what sort of messages
 *		have been sent inside a big packet.
 *
 */
void MAINetDebugBigPacket(DPMSG_BIG_PACKET *Net )
{
	char tstr[256];
 	char *ppacket;
	BYTE bpackettype;
	int fromslot = Net->slot;
	int numpackets = Net->num_packets;
	DPMSG_GENERIC_1_SM *NetGen1;
	DPMSG_GENERIC_2_SM *NetGen2;
	DPMSG_GENERIC_3_SM *NetGen3;
	DPMSG_GENERIC_4_SM *NetGen4;
	DPMSG_GENERIC_2_LONG_SM *NetGen2Long;
	DPMSG_GENERIC_3_LONG_SM *NetGen3Long;
	DPMSG_GENERIC_4_LONG_SM *NetGen4Long;
	DPMSG_GENERIC_2_FPOINT_SM *NetGen2FPoint;
	DPMSG_GENERIC_OBJ_POINT_SM *NetGenObjPoint;
	DPMSG_VGROUND_LOCK_SM *NetVGroundLock;
	DPMSG_GENERIC_2_FLOAT_SM *NetGen2F;
	DPMSG_GENERIC_3_FLOAT_SM *NetGen3F;
	DPMSG_WEAPON_HACK_CHECK_SM *NetHack;

	ppacket = Net->smaller_packets;
	while(numpackets > 0)
	{
		bpackettype = (BYTE)*ppacket;
		switch (bpackettype)
		{
			case NET_PLANEDATA:
				sprintf(tstr, "     Plane Data");
				ppacket += sizeof(DPMSG_PLANEDATA);
				break;
			case NET_CHATTER:
				sprintf(tstr, "     Chatter");
				ppacket += sizeof(DPMSG_CHATTER);
				break;
			case NET_PAUSED:
				sprintf(tstr, "     Paused");
				ppacket += sizeof(DPMSG_PAUSED);
				break;
			case NET_MISSILE:
				sprintf(tstr, "    Missile ");
				ppacket += sizeof(DPMSG_MISSILE);
				break;
			case NET_TARGET:
				sprintf(tstr, "     Target");
				ppacket += sizeof(DPMSG_TARGET);
				break;
			case NET_STRIKE:
				sprintf(tstr, "     Strike");
				ppacket += sizeof(DPMSG_STRIKE);
				break;
			case NET_DAMAGE:
				sprintf(tstr, "     Damage");
				ppacket += sizeof(DPMSG_DAMAGE);
				break;
			case NET_CRASH:
				sprintf(tstr, "     Crash");
				ppacket += sizeof(DPMSG_CRASH);
				break;
			case NET_DECOY:
				sprintf(tstr, "     Decoy");
				ppacket += sizeof(DPMSG_DECOY);
				break;
			case NET_EJECT:
				sprintf(tstr, "     Eject");
				ppacket += sizeof(DPMSG_EJECT);
				break;
			case NET_AI_POS:
				sprintf(tstr, "     AI Pos");
				ppacket += sizeof(DPMSG_AI_POS);
				break;
			case NET_MISSILE_GENERAL:
				sprintf(tstr, "     Missile General");
				ppacket += sizeof(DPMSG_MISSILE_GENERAL);
				break;
			case NET_GROUND_LOCK:
				sprintf(tstr, "     Ground Lock");
				ppacket += sizeof(DPMSG_GROUND_LOCK);
				break;
			case NET_BOMB_GENERAL:
				sprintf(tstr, "     Bomb General");
				ppacket += sizeof(DPMSG_BOMB_GENERAL);
				break;
			case NET_AG_MISSILE_GENERAL:
				sprintf(tstr, "     AG Missile General");
				ppacket += sizeof(DPMSG_AG_MISSILE_GENERAL);
				break;
			case NET_ZONES:
				sprintf(tstr, "     Zones");
				ppacket += sizeof(DPMSG_CONTROL_ZONES);
				break;
			case NET_AI_COMMS:
				sprintf(tstr, "     AI_Comms");
				ppacket += sizeof(DPMSG_AI_COMMS);
				break;
			case NET_AI_HEADING:
				sprintf(tstr, "     AI Heading");
				ppacket += sizeof(DPMSG_DESIRED_HEADING);
				break;
			case NET_CAS_DATA:
				sprintf(tstr, "     CAS Data");
				ppacket += sizeof(DPMSG_CAS_DATA);
				break;
			case NET_GENERIC_1:
				sprintf(tstr, "     Generic 1");
				ppacket += sizeof(DPMSG_GENERIC_1);
				break;
			case NET_GENERIC_2:
				sprintf(tstr, "     Generic 2");
				ppacket += sizeof(DPMSG_GENERIC_2);
				break;
			case NET_GENERIC_3:
				sprintf(tstr, "     Generic 3");
				ppacket += sizeof(DPMSG_GENERIC_3);
				break;
			case NET_GENERIC_4:
				sprintf(tstr, "     Generic 4");
				ppacket += sizeof(DPMSG_GENERIC_4);
				break;
			case NET_DF_UPDATE:
				sprintf(tstr, "     DF Update");
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE);
				break;
			case NET_DF_UPDATE_CPOS:
				sprintf(tstr, "     Update CPOS");
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE_CPOS);
				break;
			case NET_MISSILE_BREAK:
				sprintf(tstr, "     Missile Break");
				ppacket += sizeof(DPMSG_MISSILE_BREAK);
				break;
			case NET_WEAPON_G_EXPLODE:
				sprintf(tstr, "     Weapon G Explode");
				ppacket += sizeof(DPMSG_WEAPON_G_EXPLODE);
				break;
			case NET_PLANEDATA_SM:
				sprintf(tstr, "     Plane Data SM");
				ppacket += sizeof(DPMSG_PLANEDATA_SM);
				break;
			case NET_CHATTER_SM:
				sprintf(tstr, "     Chatter SM");
				ppacket += sizeof(DPMSG_CHATTER_SM);
				break;
			case NET_MISSILE_SM:
				sprintf(tstr, "     Missile SM");
				ppacket += sizeof(DPMSG_MISSILE_SM);
				break;
			case NET_TARGET_SM:
				sprintf(tstr, "     Target SM");
				ppacket += sizeof(DPMSG_TARGET_SM);
				break;
			case NET_STRIKE_SM:
			case NET_BULLET_STRIKE_SM:
				sprintf(tstr, "     Strike SM");
				ppacket += sizeof(DPMSG_STRIKE_SM);
				break;
			case NET_DAMAGE_SM:
				sprintf(tstr, "     Damage SM");
				ppacket += sizeof(DPMSG_DAMAGE_SM);
				break;
			case NET_CRASH_SM:
				sprintf(tstr, "     Crash SM");
				ppacket += sizeof(DPMSG_CRASH_SM);
				break;
			case NET_DECOY_SM:
				sprintf(tstr, "     Decoy SM");
				ppacket += sizeof(DPMSG_DECOY_SM);
				break;
			case NET_EJECT_SM:
				sprintf(tstr, "     Eject SM");
				ppacket += sizeof(DPMSG_EJECT_SM);
				break;
			case NET_AI_POS_SM:
				sprintf(tstr, "     AI Pos SM");
				ppacket += sizeof(DPMSG_AI_POS_SM);
				break;
			case NET_MISSILE_GENERAL_SM:
				sprintf(tstr, "     Missile General SM");
				ppacket += sizeof(DPMSG_MISSILE_GENERAL_SM);
				break;
			case NET_GROUND_LOCK_SM:
				sprintf(tstr, "     Ground Lock SM");
				ppacket += sizeof(DPMSG_GROUND_LOCK_SM);
				break;
			case NET_BOMB_GENERAL_SM:
				sprintf(tstr, "     Bomb General SM");
				ppacket += sizeof(DPMSG_BOMB_GENERAL_SM);
				break;
			case NET_AG_MISSILE_GENERAL_SM:
				sprintf(tstr, "     AG Missile General SM");
				ppacket += sizeof(DPMSG_AG_MISSILE_GENERAL_SM);
				break;
			case NET_ZONES_SM:
				sprintf(tstr, "     Zones SM");
				ppacket += sizeof(DPMSG_CONTROL_ZONES_SM);
				break;
			case NET_AI_COMMS_SM:
				sprintf(tstr, "     AI_Comms SM");
				ppacket += sizeof(DPMSG_AI_COMMS_SM);
				break;
			case NET_AI_HEADING_SM:
				sprintf(tstr, "     AI Heading SM");
				ppacket += sizeof(DPMSG_DESIRED_HEADING_SM);
				break;
			case NET_CAS_DATA_SM:
				sprintf(tstr, "     CAS Data SM");
				ppacket += sizeof(DPMSG_CAS_DATA_SM);
				break;
			case NET_GENERIC_1_SM:
				NetGen1 = (DPMSG_GENERIC_1_SM *)ppacket;
				sprintf(tstr, "     Generic 1 SM T %d, S %d, A1 %d", NetGen1->type, NetGen1->slot, NetGen1->arg1);
				ppacket += sizeof(DPMSG_GENERIC_1_SM);
				break;
			case NET_GENERIC_2_SM:
				NetGen2 = (DPMSG_GENERIC_2_SM *)ppacket;
				sprintf(tstr, "     Generic 2 SM T %d, S %d, A1 %d, A2 %d", NetGen2->type, NetGen2->slot, NetGen2->arg1, NetGen2->arg2);
				ppacket += sizeof(DPMSG_GENERIC_2_SM);
				break;
			case NET_GENERIC_3_SM:
				NetGen3 = (DPMSG_GENERIC_3_SM *)ppacket;
				sprintf(tstr, "     Generic 3 SM T %d, S %d, A1 %d, A2 %d, A3 %d", NetGen3->type, NetGen3->slot, NetGen3->arg1, NetGen3->arg2, NetGen3->arg3);
				ppacket += sizeof(DPMSG_GENERIC_3_SM);
				break;
			case NET_GENERIC_4_SM:
				NetGen4 = (DPMSG_GENERIC_4_SM *)ppacket;
				sprintf(tstr, "     Generic 4 SM T %d, S %d, A1 %d, A2 %d, A3 %d, A4 %d", NetGen4->type, NetGen4->slot, NetGen4->arg1, NetGen4->arg2, NetGen4->arg3, NetGen4->arg4);
				ppacket += sizeof(DPMSG_GENERIC_4_SM);
				break;
			case NET_DF_UPDATE_SM:
				sprintf(tstr, "     DF Update SM");
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE_SM);
				break;
			case NET_DF_UPDATE_CPOS_SM:
				sprintf(tstr, "     Update CPOS SM");
				ppacket += sizeof(DPMSG_DOGFIGHT_UPDATE_CPOS_SM);
				break;
			case NET_MISSILE_BREAK_SM:
				sprintf(tstr, "     Missile Break SM");
				ppacket += sizeof(DPMSG_MISSILE_BREAK_SM);
				break;
			case NET_WEAPON_G_EXPLODE_SM:
				sprintf(tstr, "     Weapon G Explode SM");
				ppacket += sizeof(DPMSG_WEAPON_G_EXPLODE_SM);
				break;
			case NET_GENERIC_2_LONG_SM:
				NetGen2Long = (DPMSG_GENERIC_2_LONG_SM *)ppacket;
				sprintf(tstr, "     Generic 2 LONG SM T %d, S %d, A1 %d, A2 %ld", NetGen2Long->type, NetGen2Long->slot, NetGen2Long->arg1, NetGen2Long->arg2);
				ppacket += sizeof(DPMSG_GENERIC_2_LONG_SM);
				break;
			case NET_GENERIC_3_LONG_SM:
				NetGen3Long = (DPMSG_GENERIC_3_LONG_SM *)ppacket;
				sprintf(tstr, "     Generic 3 LONG SM T %d, S %d, A1 %d, A2 %ld, A3 %ld", NetGen3Long->type, NetGen3Long->slot, NetGen3Long->arg1, NetGen3Long->arg2, NetGen3Long->arg3);
				ppacket += sizeof(DPMSG_GENERIC_3_LONG_SM);
				break;
			case NET_GENERIC_4_LONG_SM:
				NetGen4Long = (DPMSG_GENERIC_4_LONG_SM *)ppacket;
				sprintf(tstr, "     Generic 4 LONG SM T %d, S %d, A1 %d, A2 %ld, A3 %ld, A4 %ld", NetGen4Long->type, NetGen4Long->slot, NetGen4Long->arg1, NetGen4Long->arg2, NetGen4Long->arg3, NetGen4Long->arg4);
				ppacket += sizeof(DPMSG_GENERIC_4_LONG_SM);
				break;
			case NET_GENERIC_2_FPOINT_SM:
				NetGen2FPoint = (DPMSG_GENERIC_2_FPOINT_SM *)ppacket;
				sprintf(tstr, "     Generic 2 FPOINT SM T %d, S %d, A1 %d", NetGen2FPoint->type, NetGen2FPoint->slot, NetGen2FPoint->arg1);
				ppacket += sizeof(DPMSG_GENERIC_2_FPOINT_SM);
				break;
			case NET_GENERIC_OBJ_POINT_SM:
				NetGenObjPoint = (DPMSG_GENERIC_OBJ_POINT_SM *)ppacket;
				sprintf(tstr, "     Generic OBJ POINT SM T %d, S %d, A1 %d, OT %d, OID %ld", NetGenObjPoint->type, NetGenObjPoint->slot, NetGenObjPoint->arg1, NetGenObjPoint->objtype, NetGenObjPoint->dwObjSerialNum);
				ppacket += sizeof(DPMSG_GENERIC_OBJ_POINT_SM);
				break;
			case NET_VGROUND_LOCK_SM:
				NetVGroundLock = (DPMSG_VGROUND_LOCK_SM *)ppacket;
				sprintf(tstr, "     VGround Lock SM T %d, S %d, VNum %d, RDNum %d, WRDNum %d", NetVGroundLock->type, NetVGroundLock->slot, NetVGroundLock->VNum, NetVGroundLock->RDNum, NetVGroundLock->WRDNum);
				ppacket += sizeof(DPMSG_VGROUND_LOCK_SM);
				break;
			case NET_GENERIC_2_FLOAT_SM:
				NetGen2F = (DPMSG_GENERIC_2_FLOAT_SM *)ppacket;
				sprintf(tstr, "     Gen 2 Float SM T %d, S %d, A1 %d, A2 %d", NetGen2F->type, NetGen2F->slot, NetGen2F->arg1, NetGen2F->arg2);
				ppacket += sizeof(DPMSG_GENERIC_2_FLOAT_SM);
				break;
			case NET_GENERIC_3_FLOAT_SM:
				NetGen3F = (DPMSG_GENERIC_3_FLOAT_SM *)ppacket;
				sprintf(tstr, "     Gen 3 Float SM T %d, S %d, A1 %d, A2 %d, A3 %d", NetGen3F->type, NetGen3F->slot, NetGen3F->arg1, NetGen3F->arg2, NetGen3F->arg3);
				ppacket += sizeof(DPMSG_GENERIC_3_FLOAT_SM);
				break;
			case NET_CHECK_DATA_HACK_SM:
				NetHack = (DPMSG_WEAPON_HACK_CHECK_SM *)ppacket;
				sprintf(tstr, "     Net Hack T %d, WID %d", NetHack->type, NetHack->weapid);
				ppacket += sizeof(DPMSG_WEAPON_HACK_CHECK_SM);
				break;
			default:
				sprintf(tstr, "UNKNOWN %d", bpackettype);
				bpackettype = 0;
				if(hNetDebugFile != -1)
				{
					_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1

					sprintf(tstr, "\n");
					_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1

			//		OutputDebugString( tstr );
			//		OutputDebugString("\n\r");
				}
				return;
				break;
		}
		if(hNetDebugFile != -1)
		{
			_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1

			sprintf(tstr, "\n");
			_write(hNetDebugFile,tstr,(strlen(tstr)));  // + 1

	//		OutputDebugString( tstr );
	//		OutputDebugString("\n\r");
		}

		numpackets --;
	}
}

/*----------------------------------------------------------------------------
 *
 *	MAIGetIDForBehaviorfunc()
 *
 *		Gives an int ID so I can send a behavior change across the net.
 *
 */
int MAIGetIDForBehaviorfunc(void *behaviorfunc)
{
	if(behaviorfunc == AIAirEngage)
		return(0);
	if(behaviorfunc == AIAirInvestigate)
		return(1);
	if(behaviorfunc == AIAllOutAttack)
		return(2);
	if(behaviorfunc == AIAwayFromTarget)
		return(3);
	if(behaviorfunc == AIBreakToHeading)
		return(4);
	if(behaviorfunc == AIBreakToPitch)
		return(5);
	if(behaviorfunc == AIDeathSpiral)
		return(6);
	if(behaviorfunc == AIDisengage)
		return(7);
	if(behaviorfunc == AIDiveBomb)
		return(8);
	if(behaviorfunc == AIDoubleAttack)
		return(9);
	if(behaviorfunc == AIEgressToHeading)
		return(10);
	if(behaviorfunc == AIEgressToWayPt)
		return(11);
	if(behaviorfunc == AIEnterLandingPattern)
		return(12);
	if(behaviorfunc == AIEscortOrbitCCSet)
		return(13);
	if(behaviorfunc == AIEscortOrbitCSet)
		return(14);
	if(behaviorfunc == AIFightingWing)
		return(15);
	if(behaviorfunc == AIFluidFour)
		return(16);
	if(behaviorfunc == AIFlyCAPSide1)
		return(17);
	if(behaviorfunc == AIFlyCAPSide2)
		return(18);
	if(behaviorfunc == AIFlyCAPSide3)
		return(19);
	if(behaviorfunc == AIFlyCAPSide4)
		return(20);
	if(behaviorfunc == AIFlyCAS)
		return(21);
	if(behaviorfunc == AIFlyCASOrbit)
		return(22);
	if(behaviorfunc == AIFlyEscort)
		return(23);
	if(behaviorfunc == AIFlyEscortOrbit)
		return(24);
	if(behaviorfunc == AIFlyFormation)
		return(25);
	if(behaviorfunc == AIFlyHeloLanding)
		return(26);
	if(behaviorfunc == AIFlyHover)
		return(27);
	if(behaviorfunc == AIFlyMarshallPattern)
		return(28);
	if(behaviorfunc == AIFlyOrbit)
		return(29);
	if(behaviorfunc == AIFlySlowMarshallPattern)
		return(30);
	if(behaviorfunc == AIFlyTakeOff)
		return(31);
	if(behaviorfunc == AIFlyTanker)
		return(32);
	if(behaviorfunc == AIFlyTankerFormation)
		return(33);
	if(behaviorfunc == AIFlyToCASArea)
		return(34);
	if(behaviorfunc == AIFlyToEscortMeeting)
		return(35);
	if(behaviorfunc == AIFlyToHoverPoint)
		return(36);
	if(behaviorfunc == AIFlyToLandingPoint)
		return(37);
	if(behaviorfunc == AIFlyToOrbitPoint)
		return(38);
	if(behaviorfunc == AIFlyToPopPoint)
		return(39);
	if(behaviorfunc == AIFlyToTankerPoint)
		return(40);
	if(behaviorfunc == AIFormationFlying)
		return(41);
	if(behaviorfunc == AIFormationFlyingLinked)
		return(42);
	if(behaviorfunc == AIGaggle)
		return(43);
	if(behaviorfunc == AIGainAlt)
		return(44);
	if(behaviorfunc == AIGroupFlyToDivePoint)
		return(45);
	if(behaviorfunc == AIGroupFlyToLoftPoint)
		return(46);
	if(behaviorfunc == AIGroupFlyToPopPoint)
		return(47);
	if(behaviorfunc == AIGroupLevelBomb)
		return(48);
	if(behaviorfunc == AIHeloSinkToGround)
		return(49);
	if(behaviorfunc == AIJustHeloLand)
		return(50);
	if(behaviorfunc == AILandingApproachBreak)
		return(51);
	if(behaviorfunc == AILandingBaseLeg)
		return(52);
	if(behaviorfunc == AILandingBaseLeg2)
		return(53);
	if(behaviorfunc == AILandingDownwind)
		return(54);
	if(behaviorfunc == AILandingFinal)
		return(55);
	if(behaviorfunc == AILandingLevelBreak)
		return(56);
	if(behaviorfunc == AILandingLevelBreak2)
		return(57);
	if(behaviorfunc == AILandingRollout)
		return(58);
	if(behaviorfunc == AILandingSlowBaseLeg2)
		return(59);
	if(behaviorfunc == AILandingTaxi)
		return(60);
	if(behaviorfunc == AILandingTouchdown)
		return(61);
	if(behaviorfunc == AILevelBomb)
		return(62);
	if(behaviorfunc == AILoftBomb)
		return(63);
	if(behaviorfunc == AILooseDeuce)
		return(64);
	if(behaviorfunc == AIMissileBreakToHeading)
		return(65);
	if(behaviorfunc == AIParked)
		return(66);
	if(behaviorfunc == AIPlaneLandingFlyToField)
		return(67);
	if(behaviorfunc == AIPopUpBomb)
		return(68);
	if(behaviorfunc == AIResumeEscort)
		return(69);
	if(behaviorfunc == AISanitizing)
		return(70);
	if(behaviorfunc == AISARWait)
		return(71);
	if(behaviorfunc == AISetUpCAP1)
		return(72);
	if(behaviorfunc == AIStartDiveBomb)
		return(73);
	if(behaviorfunc == AITankerDisconnect)
		return(74);
	if(behaviorfunc == AITankingBreakaway)
		return(75);
	if(behaviorfunc == AITankingFlight)
		return(76);
	if(behaviorfunc == AITankingFlight2)
		return(77);
	if(behaviorfunc == AITankingFlight3)
		return(78);
	if(behaviorfunc == AITankingFlight4)
		return(79);
	if(behaviorfunc == AITankingFlight5)
		return(80);
	if(behaviorfunc == FlyCruiseMissile)
		return(81);
	if(behaviorfunc == NULL)
		return(82);

	return(-1);
}

/*----------------------------------------------------------------------------
 *
 *	MAIUpdateBehavior
 *
 *		Using an ID from the net update a behavior.
 *
 */
void MAIUpdateBehavior(int behaviornum, PlaneParams *planepnt, int updatebehavior)
{
	PlaneParams tempplane;

	tempplane.AI.Behaviorfunc = NULL;

	switch(behaviornum)
	{
		case 0:
			tempplane.AI.Behaviorfunc = AIAirEngage;
			break;
		case 1:
			tempplane.AI.Behaviorfunc = AIAirInvestigate;
			break;
		case 2:
			tempplane.AI.Behaviorfunc = AIAllOutAttack;
			break;
		case 3:
			tempplane.AI.Behaviorfunc = AIAwayFromTarget;
			break;
		case 4:
			tempplane.AI.Behaviorfunc = AIBreakToHeading;
			break;
		case 5:
			tempplane.AI.Behaviorfunc = AIBreakToPitch;
			break;
		case 6:
			tempplane.AI.Behaviorfunc = AIDeathSpiral;
			break;
		case 7:
			tempplane.AI.Behaviorfunc = AIDisengage;
			break;
		case 8:
			tempplane.AI.Behaviorfunc = AIDiveBomb;
			break;
		case 9:
			tempplane.AI.Behaviorfunc = AIDoubleAttack;
			break;
		case 10:
			tempplane.AI.Behaviorfunc = AIEgressToHeading;
			break;
		case 11:
			tempplane.AI.Behaviorfunc = AIEgressToWayPt;
			break;
		case 12:
			tempplane.AI.Behaviorfunc = AIEnterLandingPattern;
			break;
		case 13:
			tempplane.AI.Behaviorfunc = AIEscortOrbitCCSet;
			break;
		case 14:
			tempplane.AI.Behaviorfunc = AIEscortOrbitCSet;
			break;
		case 15:
			tempplane.AI.Behaviorfunc = AIFightingWing;
			break;
		case 16:
			tempplane.AI.Behaviorfunc = AIFluidFour;
			break;
		case 17:
			tempplane.AI.Behaviorfunc = AIFlyCAPSide1;
			break;
		case 18:
			tempplane.AI.Behaviorfunc = AIFlyCAPSide2;
			break;
		case 19:
			tempplane.AI.Behaviorfunc = AIFlyCAPSide3;
			break;
		case 20:
			tempplane.AI.Behaviorfunc = AIFlyCAPSide4;
			break;
		case 21:
			tempplane.AI.Behaviorfunc = AIFlyCAS;
			break;
		case 22:
			tempplane.AI.Behaviorfunc = AIFlyCASOrbit;
			break;
		case 23:
			tempplane.AI.Behaviorfunc = AIFlyEscort;
			break;
		case 24:
			tempplane.AI.Behaviorfunc = AIFlyEscortOrbit;
			break;
		case 25:
			tempplane.AI.Behaviorfunc = AIFlyFormation;
			break;
		case 26:
			tempplane.AI.Behaviorfunc = AIFlyHeloLanding;
			break;
		case 27:
			tempplane.AI.Behaviorfunc = AIFlyHover;
			break;
		case 28:
			tempplane.AI.Behaviorfunc = AIFlyMarshallPattern;
			break;
		case 29:
			tempplane.AI.Behaviorfunc = AIFlyOrbit;
			break;
		case 30:
			tempplane.AI.Behaviorfunc = AIFlySlowMarshallPattern;
			break;
		case 31:
			tempplane.AI.Behaviorfunc = AIFlyTakeOff;
			break;
		case 32:
			tempplane.AI.Behaviorfunc = AIFlyTanker;
			break;
		case 33:
			tempplane.AI.Behaviorfunc = AIFlyTankerFormation;
			break;
		case 34:
			tempplane.AI.Behaviorfunc = AIFlyToCASArea;
			break;
		case 35:
			tempplane.AI.Behaviorfunc = AIFlyToEscortMeeting;
			break;
		case 36:
			tempplane.AI.Behaviorfunc = AIFlyToHoverPoint;
			break;
		case 37:
			tempplane.AI.Behaviorfunc = AIFlyToLandingPoint;
			break;
		case 38:
			tempplane.AI.Behaviorfunc = AIFlyToOrbitPoint;
			break;
		case 39:
			tempplane.AI.Behaviorfunc = AIFlyToPopPoint;
			break;
		case 40:
			tempplane.AI.Behaviorfunc = AIFlyToTankerPoint;
			break;
		case 41:
			tempplane.AI.Behaviorfunc = AIFormationFlying;
			break;
		case 42:
			tempplane.AI.Behaviorfunc = AIFormationFlyingLinked;
			break;
		case 43:
			tempplane.AI.Behaviorfunc = AIGaggle;
			break;
		case 44:
			tempplane.AI.Behaviorfunc = AIGainAlt;
			break;
		case 45:
			tempplane.AI.Behaviorfunc = AIGroupFlyToDivePoint;
			break;
		case 46:
			tempplane.AI.Behaviorfunc = AIGroupFlyToLoftPoint;
			break;
		case 47:
			tempplane.AI.Behaviorfunc = AIGroupFlyToPopPoint;
			break;
		case 48:
			tempplane.AI.Behaviorfunc = AIGroupLevelBomb;
			break;
		case 49:
			tempplane.AI.Behaviorfunc = AIHeloSinkToGround;
			break;
		case 50:
			tempplane.AI.Behaviorfunc = AIJustHeloLand;
			break;
		case 51:
			tempplane.AI.Behaviorfunc = AILandingApproachBreak;
			break;
		case 52:
			tempplane.AI.Behaviorfunc = AILandingBaseLeg;
			break;
		case 53:
			tempplane.AI.Behaviorfunc = AILandingBaseLeg2;
			break;
		case 54:
			tempplane.AI.Behaviorfunc = AILandingDownwind;
			break;
		case 55:
			tempplane.AI.Behaviorfunc = AILandingFinal;
			break;
		case 56:
			tempplane.AI.Behaviorfunc = AILandingLevelBreak;
			break;
		case 57:
			tempplane.AI.Behaviorfunc = AILandingLevelBreak2;
			break;
		case 58:
			tempplane.AI.Behaviorfunc = AILandingRollout;
			break;
		case 59:
			tempplane.AI.Behaviorfunc = AILandingSlowBaseLeg2;
			break;
		case 60:
			tempplane.AI.Behaviorfunc = AILandingTaxi;
			break;
		case 61:
			tempplane.AI.Behaviorfunc = AILandingTouchdown;
			break;
		case 62:
			tempplane.AI.Behaviorfunc = AILevelBomb;
			break;
		case 63:
			tempplane.AI.Behaviorfunc = AILoftBomb;
			break;
		case 64:
			tempplane.AI.Behaviorfunc = AILooseDeuce;
			break;
		case 65:
			tempplane.AI.Behaviorfunc = AIMissileBreakToHeading;
			break;
		case 66:
			tempplane.AI.Behaviorfunc = AIParked;
			break;
		case 67:
			tempplane.AI.Behaviorfunc = AIPlaneLandingFlyToField;
			break;
		case 68:
			tempplane.AI.Behaviorfunc = AIPopUpBomb;
			break;
		case 69:
			tempplane.AI.Behaviorfunc = AIResumeEscort;
			break;
		case 70:
			tempplane.AI.Behaviorfunc = AISanitizing;
			break;
		case 71:
			tempplane.AI.Behaviorfunc = AISARWait;
			break;
		case 72:
			tempplane.AI.Behaviorfunc = AISetUpCAP1;
			break;
		case 73:
			tempplane.AI.Behaviorfunc = AIStartDiveBomb;
			break;
		case 74:
			tempplane.AI.Behaviorfunc = AITankerDisconnect;
			break;
		case 75:
			tempplane.AI.Behaviorfunc = AITankingBreakaway;
			break;
		case 76:
			tempplane.AI.Behaviorfunc = AITankingFlight;
			break;
		case 77:
			tempplane.AI.Behaviorfunc = AITankingFlight2;
			break;
		case 78:
			tempplane.AI.Behaviorfunc = AITankingFlight3;
			break;
		case 79:
			tempplane.AI.Behaviorfunc = AITankingFlight4;
			break;
		case 80:
			tempplane.AI.Behaviorfunc = AITankingFlight5;
			break;
		case 81:
			tempplane.AI.Behaviorfunc = FlyCruiseMissile;
			break;
		case 82:
			tempplane.AI.Behaviorfunc = NULL;
			break;
		default:
			tempplane.AI.Behaviorfunc = NULL;
			break;
	}

	if(updatebehavior == 1)
	{
		planepnt->AI.OrgBehave = tempplane.AI.Behaviorfunc;
	}
	else if(tempplane.AI.Behaviorfunc)
	{
		planepnt->AI.Behaviorfunc = tempplane.AI.Behaviorfunc;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutFullAIUpdate
 *
 *		For MultiPlayer games where people can join in later we will want to
 *	more fully update the information about AIs controlled by this Player.
 *	When possible I'm using existing message to update the information.
 *
 */
void NetPutFullAIUpdate(PlaneParams *planepnt)
{
	int ibehaveid;
	BYTE	bbehaveid, bobehaveid;
	BYTE	bobjecttype;
	DWORD	objectid;
	BasicInstance *walker;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		NetPutGenericMessage1(planepnt, GM_KILL_PLANE);
	}

	NetPutAIPos(planepnt);

	ibehaveid = MAIGetIDForBehaviorfunc(planepnt->AI.Behaviorfunc);
	bbehaveid = (ibehaveid >= 0) ? ibehaveid : 255;

	ibehaveid = MAIGetIDForBehaviorfunc(planepnt->AI.OrgBehave);
	bobehaveid = (ibehaveid >= 0) ? ibehaveid : 255;

	NetPutGenericMessage3(planepnt, GM3_UPDATE_BEHAVES, bbehaveid, bobehaveid);

	NetPutGenericMessage3(planepnt, GM3_WING_UPD, ((planepnt->AI.winglead == -1) ? 255 : planepnt->AI.winglead), ((planepnt->AI.wingman == -1) ? 255 : planepnt->AI.wingman));

	NetPutGenericMessage3(planepnt, GM3_PAIR_UPD, ((planepnt->AI.prevpair == -1) ? 255 : planepnt->AI.prevpair), ((planepnt->AI.nextpair == -1) ? 255 : planepnt->AI.nextpair));

	if(planepnt->AI.cNumWeaponRelease > 0)
	{
		NetPutGenericMessage3(planepnt, GM3_DESIRED_BOMBS, planepnt->AI.cActiveWeaponStation, planepnt->AI.cNumWeaponRelease);
	}

	if(planepnt->AI.AirTarget)
	{
		GeneralSetNewAirTarget(planepnt, planepnt->AI.AirTarget);
	}
	if(planepnt->AI.AirThreat)
	{
		GeneralSetNewAirThreat(planepnt, planepnt->AI.AirThreat);
	}

	if(planepnt->AI.CombatBehavior)
	{
		if(planepnt->AI.iAICombatFlags1 & AI_MULTI_SEND_CPOS)
		{
			NetPutDogfightCPosUpdate(planepnt, planepnt->AI.cMultiLastCombatBehaveID, planepnt->AI.lCombatTimer, planepnt->AI.CombatPos);
		}
		else
		{
			NetPutDogfightUpdate(planepnt, planepnt->AI.cMultiLastCombatBehaveID, planepnt->AI.lCombatTimer);
		}
	}

	if(planepnt->AI.LinkedPlane)
	{
		NetPutGenericMessage2(planepnt, GM2_LINK_PLANE, (planepnt->AI.LinkedPlane - Planes));
	}
	else
	{
		NetPutGenericMessage2(planepnt, GM2_LINK_PLANE, 255);
	}

	NetPutGenericMessage2Long(planepnt, GM2L_COMBAT_FLAGS, planepnt->AI.iAICombatFlags1);

	NetPutGenericMessage3Long(planepnt, GM3L_TIMERS, planepnt->AI.lTimer1, planepnt->AI.lTimer2);
	NetPutGenericMessage3Long(planepnt, GM3L_VARS, planepnt->AI.iVar1, planepnt->AI.lVar2);
	NetPutGenericMessage3Long(planepnt, GM3L_AI_FLAGS, planepnt->AI.iAIFlags1, planepnt->AI.iAIFlags2);

	NetPutGenericMessage2FPoint(planepnt, GM2FP_WAYPOS, planepnt->AI.WayPosition);
	NetPutGenericMessage2FPoint(planepnt, GM2FP_FORM_POS, planepnt->AI.FormationPosition);
	FPoint fptemp;
	fptemp.X = planepnt->AI.fStoresWeight;
	fptemp.Y = 0;
	fptemp.Z = 0;
	NetPutGenericMessage2FPoint(planepnt, GM2FP_STORES, fptemp);

	if((planepnt->AI.pGroundTarget) && (planepnt->AI.lGroundTargetFlag != -1))
	{
		bobjecttype = planepnt->AI.lGroundTargetFlag;
		switch(bobjecttype)
		{
			case AIRCRAFT:
				objectid = (PlaneParams *)planepnt->AI.pGroundTarget - Planes;
				break;
			case MOVINGVEHICLE:
			case SHIP:
				objectid = (MovingVehicleParams *)planepnt->AI.pGroundTarget - MovingVehicles;
				break;
			case GROUNDOBJECT:
				walker = (BasicInstance *)planepnt->AI.pGroundTarget;
				objectid = walker->SerialNumber;
				break;
		}
	}
	else
	{
		bobjecttype = 255;
		objectid = 0;
	}

	NetPutGenericMessageObjectPoint(planepnt, GMOP_GROUND_TARG, bobjecttype, objectid);

	if((planepnt->AI.pPaintedBy) && (planepnt->AI.iPaintedByType != -1))
	{
		bobjecttype = planepnt->AI.iPaintedByType;
		switch(bobjecttype)
		{
			case AIRCRAFT:
				objectid = (PlaneParams *)planepnt->AI.pPaintedBy - Planes;
				break;
			case MOVINGVEHICLE:
			case SHIP:
				objectid = (MovingVehicleParams *)planepnt->AI.pPaintedBy - MovingVehicles;
				break;
			case GROUNDOBJECT:
				walker = (BasicInstance *)planepnt->AI.pPaintedBy;
				objectid = walker->SerialNumber;
				break;
		}
	}
	else
	{
		bobjecttype = 255;
		objectid = 0;
	}

	NetPutGenericMessageObjectPoint(planepnt, GMOP_PAINTED_BY, bobjecttype, objectid);

}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2Long()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 1 long arguements.
 *
 */
void NetPutGenericMessage2Long(PlaneParams *planepnt, BYTE arg1, long arg2, BYTE slotnum)
{
	DPMSG_GENERIC_2_LONG NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage2LongSmall(planepnt, arg1, arg2, slotnum);
		return;
	}

	NetGen.type = NET_GENERIC_2_LONG;
	NetGen.time   = newTime;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2Long()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 1 long.
 *
 */
void NetGetGenericMessage2Long( DPMSG_GENERIC_2_LONG *Net)
{
	DPMSG_GENERIC_2_LONG_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;

	NetGetGenericMessage2LongSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2LongSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 1 long arguement.
 *
 */
void NetPutGenericMessage2LongSmall(PlaneParams *planepnt, BYTE arg1, long arg2, BYTE slotnum)
{
	DPMSG_GENERIC_2_LONG_SM NetGen;

	NetGen.type = NET_GENERIC_2_LONG_SM;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}

	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2LongSmall()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 1 long argument.
 *
 */
void NetGetGenericMessage2LongSmall( DPMSG_GENERIC_2_LONG_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;
	GDRadarData *radardat;
	int slotnum;
	long bitflag;

	switch(Net->arg1)
	{
		case GM2L_COMBAT_FLAGS:
			planepnt = &Planes[Net->slot];
			planepnt->AI.iAICombatFlags1 = Net->arg2 & ~(AI_MULTI_ACTIVE|AI_MULTI_SEND_CPOS);
			return;
			break;
		case GM2L_V_RADARS_DEAD:
			vehiclepnt = &MovingVehicles[Net->slot];
			if(vehiclepnt->iShipType)
			{
				bitflag = Net->arg2;
				for(slotnum = 0; slotnum < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; slotnum ++)
				{
					if(bitflag & (1<<slotnum))
					{
						radardat = (GDRadarData *)&vehiclepnt->RadarWeaponData[slotnum];
						radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
						radardat->lRFlags1 |= (GD_I_AM_DEAD);
					}
				}
			}
			else
			{
				radardat = (GDRadarData *)&vehiclepnt->RadarWeaponData[0];
				radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
				radardat->lRFlags1 |= (GD_I_AM_DEAD);
			}
			break;
		case GM2L_CHK_R_CROSS_SIG:
			planepnt = &Planes[Net->slot];
			if(pDBAircraftList[planepnt->AI.iPlaneIndex].iRadarCrossSignature != Net->arg2)
			{
				AICGiveHackMessage(Net->slot);
			}
			break;
		case GM2L_UPDATE_LVAR2:
			planepnt = &Planes[Net->slot];
			planepnt->AI.lVar2 = Net->arg2;
			break;
		case GM2L_UPDATE_LVAR3:
			planepnt = &Planes[Net->slot];
			planepnt->AI.lVar3 = Net->arg2;
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3Long()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 2 long arguements.
 *
 */
void NetPutGenericMessage3Long(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, BYTE slotnum)
{
	DPMSG_GENERIC_3_LONG NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage3LongSmall(planepnt, arg1, arg2, arg3, slotnum);
		return;
	}

	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}

	NetGen.type = NET_GENERIC_3_LONG;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage3Long()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 2 long arguements.
 *
 */
void NetGetGenericMessage3Long( DPMSG_GENERIC_3_LONG *Net)
{
	DPMSG_GENERIC_3_LONG_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;
	NetSmall.arg3 = Net->arg3;

	NetGetGenericMessage3LongSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3LongSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and two long arguments.
 *
 */
void NetPutGenericMessage3LongSmall(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, BYTE slotnum)
{
	DPMSG_GENERIC_3_LONG_SM NetGen;

	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}

	NetGen.type = NET_GENERIC_3_LONG_SM;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage3LongSmall()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 2 long arguements.
 *
 */
void NetGetGenericMessage3LongSmall( DPMSG_GENERIC_3_LONG_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *carrier;
	DBShipType *pshiptype;
	long delaycnt;
	long pushtime = 0;
	float maxheight = 0;

	planepnt = &Planes[Net->slot];
	switch(Net->arg1)
	{
		case GM3L_TIMERS:
			planepnt->AI.lTimer1 = Net->arg2;
			planepnt->AI.lTimer2 = Net->arg3;
			break;
		case GM3L_VARS:
			planepnt->AI.iVar1 = Net->arg2;
			planepnt->AI.lVar2 = Net->arg3;
			break;
		case GM3L_AI_FLAGS:
			planepnt->AI.iAIFlags1 = Net->arg2;
			planepnt->AI.iAIFlags2 = Net->arg3;
			break;
		case GM3L_TAKEOFF_TIME:
			if((planepnt->AI.Behaviorfunc == CTWaitingForLaunch) && (planepnt->AI.lVar3 == 100))
			{
				planepnt->AI.lTimer2 = Net->arg2;
				planepnt->AI.lTimer3 = Net->arg3;
				planepnt->Brakes = 0;
				if(planepnt == PlayerPlane)
				{
					DisplayWarning(BRAKE, OFF, 1);
				}
			}
			planepnt->AI.iAICombatFlags2 |= AI_READY_FOR_LAUNCH;
			break;
		case GM3L_HOOK_WIRE:
			planepnt->TailHookFlags = (DWORD)Net->arg3;
			carrier = &MovingVehicles[Net->arg2];
			carrier->CatchWire.CaughtPlane = planepnt;
			carrier->CatchWire.Flags = CW_PLANE_CAUGHT | ((planepnt->TailHookFlags & 0x70000) >> 16);
 			carrier->lAIFlags1 |= PLANES_ON_DECK;
			planepnt->TailHookFlags |= TH_HOOKED;
//			if ((planepnt == PlayerPlane) || ((planepnt->Status & PL_CARRIER_REL) && (PlayerPlane->PlaneCopy == planepnt)))
			if (planepnt->Status & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN))
			{
				planepnt->Status |= PL_SET_PLAYER_DECEL;
				StartCasualAutoPilotPlane(planepnt);
			}

			planepnt->AI.iHomeBaseId = Net->arg2;
			planepnt->AI.Behaviorfunc = AICarrierTrap;
			planepnt->AI.lVar3 = 0;
			planepnt->UpdateRate = HIGH_FREQ;
		 	carrier->CatchWire.LastPlace = planepnt->TailHookPosition;
			break;
		case GM3L_GIVE_MARSHAL:
			delaycnt = 14000 + ((planepnt - Planes) * 50.0f);
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			pshiptype = &pDBShipList[carrier->iVDBIndex];

			if((pshiptype->lShipType & ~(SHIP_TYPE_CARRIER)) && (pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
			{
				planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
				planepnt->AI.lVar2 = 0;
				planepnt->AI.lTimer3 = -1;

				maxheight = 500.0f * FTTOWU;
				pushtime = 3;

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

			NetSetUpMarshal(planepnt, Net->arg2, Net->arg3, delaycnt);

			break;
		case GM3L_UPDATE_MARSHAL:
			planepnt->AI.lVar2 = Net->arg2;
			planepnt->AI.lTimer3 = Net->arg3;
			NetUpdateMarshal(planepnt);
			break;
		case GM3L_BANDIT_CALL:
		  	AICAddSoundCall(AICBroadcastBanditCall, Net->arg2, Net->slot * 1000, 50, Net->arg3);
			break;
		case GM3L_ESCORT_RELEASE:
			if(AIIsSEADVoice(&Planes[Net->arg2]))
			{
				AICPlayerSEADCoverDone(Net->arg2, Net->slot);
			}
			else
			{
				AICPlayerCoverDone(Net->arg2, Net->slot);
			}

			AIReleaseEscort(planepnt, Net->arg2, Net->arg3);
			break;
		case GM3L_SAR_RESPONSE:
			long ltimer = labs(Net->arg3);
			if(Net->arg3 < 0)
			{
				AICAddSoundCall(AIC_AWACS_SAR_OK, Net->slot, ltimer, 50, Net->arg2);
			}
			else
			{
				AICAddSoundCall(AICNoCover, Net->slot, ltimer, 50, Net->arg2);
			}
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage4Long()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 4 long arguements.
 *
 */
void NetPutGenericMessage4Long(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, long arg4, BYTE slotnum)
{
	DPMSG_GENERIC_4_LONG NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage4LongSmall(planepnt, arg1, arg2, arg3, arg4, slotnum);
		return;
	}

	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}

	NetGen.type = NET_GENERIC_4_LONG;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;
	NetGen.arg4 = arg4;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage4Long()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 4 long arguements.
 *
 */
void NetGetGenericMessage4Long( DPMSG_GENERIC_4_LONG *Net)
{
	DPMSG_GENERIC_4_LONG_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;
	NetSmall.arg3 = Net->arg3;
	NetSmall.arg4 = Net->arg4;

	NetGetGenericMessage4LongSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3LongSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and two long arguments.
 *
 */
void NetPutGenericMessage4LongSmall(PlaneParams *planepnt, BYTE arg1, long arg2, long arg3, long arg4, BYTE slotnum)
{
	DPMSG_GENERIC_4_LONG_SM NetGen;

	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}

	NetGen.type = NET_GENERIC_4_LONG_SM;
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;
	NetGen.arg4 = arg4;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage4LongSmall()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 3 long arguements.
 *
 */
void NetGetGenericMessage4LongSmall( DPMSG_GENERIC_4_LONG_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;
	WeaponParams weapon;
	int launchertype;
	int damage_type;
	long weaponid;
	int crash;

//	planepnt = &Planes[Net->slot];
	switch(Net->arg1)
	{
		case GM4L_LOG_PLANE_DAMAGE:
			planepnt = &Planes[Net->slot];
			launchertype = (Net->arg3 & 0xFF);
			damage_type = (Net->arg3 & 0x10000) ? DTE_DIRECT_HIT : 0;
			weapon.Launcher = NetGetObjectPointer(launchertype, (DWORD)Net->arg2);
			if(weapon.Launcher)
			{
				if(launchertype == AIRCRAFT)
				{
					weapon.P = (PlaneParams *)weapon.Launcher;
				}
				else
				{
					weapon.P = NULL;
				}

				weaponid = (Net->arg3 & 0xFF00)>>8;
				weapon.LauncherType = launchertype;
				weapon.iNetID = Net->arg4;
				weapon.Pos = planepnt->WorldPosition;
				weapon.Type = (WeaponType *)weaponid;
				netCall = 1;	// prevent recursion for remote procedure calls
				LogPlaneDamage(planepnt,&weapon,damage_type);
				netCall = 0;	// prevent recursion for remote procedure calls

				if((damage_type & DTE_DIRECT_HIT) && (pDBWeaponList[AIGetWeaponIndex(weaponid)].iWeaponType == WEAPON_TYPE_GUN) && (pDBWeaponList[AIGetWeaponIndex(weaponid)].iSeekerType!=10) && (planepnt == PlayerPlane))
				{
					int iLevel = g_iSoundLevelExternalSFX-10;
					if (iLevel<0) iLevel=0;
					if (iLevel)
					{
						SndQueueSound(Implgin2,1,iLevel,-1,-1,64);
					}
				}

				if(launchertype != AIRCRAFT)
				{
					NetSetKill( -1, Net->slot );
				}
				else
				{
					NetSetKill( Net->arg2, Net->slot );
				}
			}
			else
			{
				netCall = 1;	// prevent recursion for remote procedure calls
				LogPlaneDamage(planepnt,NULL,damage_type);
				netCall = 0;	// prevent recursion for remote procedure calls
			}
			break;
		case GM4L_LOG_PLANE_DESTROY:
			planepnt = &Planes[Net->slot];
			launchertype = (Net->arg3 & 0xFF);
			damage_type = (Net->arg3 & 0x10000) ? DTE_DIRECT_HIT : 0;
			crash = (Net->arg3 & 0x20000) ? 1 : 0;
			weapon.Launcher = NetGetObjectPointer(launchertype, (DWORD)Net->arg2);
			if(weapon.Launcher)
			{
				if(launchertype == AIRCRAFT)
				{
					weapon.P = (PlaneParams *)weapon.Launcher;
				}
				else
				{
					weapon.P = NULL;
				}

				weaponid = (Net->arg3 & 0xFF00)>>8;
				weapon.iNetID = Net->arg4;
				weapon.LauncherType = launchertype;
				weapon.Pos = planepnt->WorldPosition;
				weapon.Type = (WeaponType *)weaponid;
				netCall = 1;	// prevent recursion for remote procedure calls
				LogPlaneShotdown(planepnt,&weapon,damage_type,crash);
				netCall = 0;	// prevent recursion for remote procedure calls

				if((damage_type & DTE_DIRECT_HIT) && (pDBWeaponList[AIGetWeaponIndex(weaponid)].iWeaponType == WEAPON_TYPE_GUN) && (pDBWeaponList[AIGetWeaponIndex(weaponid)].iSeekerType!=10) && (planepnt == PlayerPlane))
				{
					int iLevel = g_iSoundLevelExternalSFX-10;
					if (iLevel<0) iLevel=0;
					if (iLevel)
					{
						SndQueueSound(Implgin2,1,iLevel,-1,-1,64);
					}
				}

				if(launchertype != AIRCRAFT)
				{
					NetSetKill( -1, Net->slot );
				}
				else
				{
					NetSetKill( Net->arg2, Net->slot );
				}
			}
			else
			{
				netCall = 1;	// prevent recursion for remote procedure calls
				LogPlaneShotdown(planepnt,NULL,damage_type,crash);
				netCall = 0;	// prevent recursion for remote procedure calls
			}
			break;
		case GM4L_LOG_VEH_DAMAGE:
			vehiclepnt = &MovingVehicles[Net->slot];
			launchertype = (Net->arg3 & 0xFF);
			damage_type = (Net->arg3 & 0x10000) ? DTE_DIRECT_HIT : 0;
			weapon.Launcher = NetGetObjectPointer(launchertype, (DWORD)Net->arg2);
			netCall = 1;	// prevent recursion for remote procedure calls

			if(weapon.Launcher)
			{
				if(launchertype == AIRCRAFT)
				{
					weapon.P = (PlaneParams *)weapon.Launcher;
				}
				else
				{
					weapon.P = NULL;
				}

				weaponid = (Net->arg3 & 0xFF00)>>8;
				weapon.LauncherType = launchertype;
				weapon.iNetID = Net->arg4;
				weapon.Pos = vehiclepnt->WorldPosition;
				weapon.Type = (WeaponType *)weaponid;
				if(!vehiclepnt->iShipType)
				{
					LogWeaponHit(&weapon,MOVINGVEHICLE,vehiclepnt - MovingVehicles);
				}
				else
				{
					LogWeaponHit(&weapon,SHIP,vehiclepnt - MovingVehicles);
				}
			}
			else
			{
				if(!vehiclepnt->iShipType)
				{
					LogWeaponHit(NULL,MOVINGVEHICLE,vehiclepnt - MovingVehicles);
				}
				else
				{
					LogWeaponHit(NULL,SHIP,vehiclepnt - MovingVehicles);
				}
			}
			netCall = 0;	// prevent recursion for remote procedure calls
			break;
		case GM4L_LOG_VEH_DESTROY:
			vehiclepnt = &MovingVehicles[Net->slot];
			launchertype = (Net->arg3 & 0xFF);
			damage_type = (Net->arg3 & 0x10000) ? DTE_DIRECT_HIT : 0;
			weapon.Launcher = NetGetObjectPointer(launchertype, (DWORD)Net->arg2);
			netCall = 1;	// prevent recursion for remote procedure calls
			if(weapon.Launcher)
			{
				if(launchertype == AIRCRAFT)
				{
					weapon.P = (PlaneParams *)weapon.Launcher;
				}
				else
				{
					weapon.P = NULL;
				}

				weapon.LauncherType = launchertype;
				weaponid = (Net->arg3 & 0xFF00)>>8;
				weapon.iNetID = Net->arg4;
				weapon.Pos = vehiclepnt->WorldPosition;
				weapon.Type = (WeaponType *)weaponid;
				if(!vehiclepnt->iShipType)
				{
					LogDestroyedObject(&weapon,MOVINGVEHICLE,vehiclepnt - MovingVehicles);
				}
				else
				{
					LogDestroyedObject(&weapon,SHIP,vehiclepnt - MovingVehicles);
				}
			}
			else
			{
				if(!vehiclepnt->iShipType)
				{
					LogDestroyedObject(NULL,MOVINGVEHICLE,vehiclepnt - MovingVehicles);
				}
				else
				{
					LogDestroyedObject(NULL,SHIP,vehiclepnt - MovingVehicles);
				}
			}
			netCall = 0;	// prevent recursion for remote procedure calls
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetSetUpMarshal()
 *
 *		Set up a group of planes for Marshal Times and stuff like that
 *
 */
void NetSetUpMarshal(PlaneParams *planepnt, long lmaxheight, long lpushtime, long ldelaycnt)
{
	long delaycnt = ldelaycnt;
	float maxheight;
	long pushtime = lpushtime;
	double dworkvar, dworkangle, angels;
	double tdistft;
	int numhelos = 0;
	long helopushtime = -1;
	long heloendpushtime = 0;
	float maxheloheight = 0;

#ifdef _DEBUG
	char tempstr[80];
	sprintf(tempstr, "Marshal %d height %ld, PT %ld", planepnt - Planes, lmaxheight, lpushtime);
	AICAddAIRadioMsgs(tempstr, 50);
#endif


	planepnt->AI.TargetPos.Y = lmaxheight * FTTOWU;
	if(planepnt == PlayerPlane)
	{
		planepnt->AI.lTimer2 = lpushtime * 60000;
	}
	else
	{
		planepnt->AI.lTimer3 = lpushtime * 60000;
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
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

		if(planepnt == PlayerPlane)
		{
			AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (5.0f * NMTOWU), 1, deckangle);
		}
		else
		{
			AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + (5.0f * NMTOWU), 10, deckangle);
		}

		maxheight = planepnt->AI.TargetPos.Y + (1000 * FTTOWU);

		planepnt->AI.Behaviorfunc = AIFlyCarrierMarshallPattern;
	}
	else
	{
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
	}

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		pushtime ++;
	}
	else
	{
		pushtime += 3;
	}

	AICAddSoundCall(AICStrikeContactMarshal, (planepnt - Planes), delaycnt, 40, 1);
	delaycnt += 5000.0f;

	AICAddSoundCall(AICNotifyShipInbound, (planepnt - Planes), delaycnt, 40, 2);
	delaycnt += 10000.0f;

//	long minutes = ((((int)WorldParams.WorldTime%3600)/60) + (planepnt->AI.lTimer3 / 60000)) % 60;
	long minutes = ((((int)WorldParams.WorldTime%3600)/60) + (lpushtime)) % 60;

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
}

/*----------------------------------------------------------------------------
 *
 *	NetUpdateMarshal()
 *
 *		Set up a group of planes for Marshal Times and stuff like that
 *
 */
void NetUpdateMarshal(PlaneParams *planepnt)
{
	double deckangle;
	float tdist;

	if(!(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
	{
		deckangle = DECK_ANGLE;
		planepnt->AI.Behaviorfunc = AIFlyCarrierMarshallPattern;
		switch(planepnt->AI.lVar2)
		{
			case 0:
				planepnt->AI.Behaviorfunc = AIFlyToDME10;
				AICCommencingPush(planepnt - Planes);
				planepnt->AI.lVar2 = 0;
				break;
			case 1:
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 2, deckangle);
				break;
			case 2:
				if(planepnt->AI.lTimer3 < 10000)
				{
					planepnt->AI.Behaviorfunc = AIFlyToDME10;
					AICCommencingPush(planepnt - Planes);
					planepnt->AI.lVar2 = 0;
					return;
				}

				if(planepnt->AI.lTimer3 < 50000)
				{
					planepnt->AI.TargetPos.X = 360.0f / ((float)planepnt->AI.lTimer3 / 1000.0f);
					planepnt->AI.lTimer2 = planepnt->AI.lTimer3;
					planepnt->AI.lVar2 = 6;
				}
				else
				{
					planepnt->AI.TargetPos.X = 180.0f / 20.0f;
					planepnt->AI.lTimer2 = 20000;
					planepnt->AI.lVar2 = 3;
				}
				break;
			case 3:
				if(planepnt->AI.lTimer3 < 330000)
				{
					planepnt->AI.lTimer2 = ((planepnt->AI.lTimer3 - 20000) / 2);
					if(planepnt->AI.lTimer2 < 0)
					{
						planepnt->AI.lTimer2 = 1;
					}
				}
				else
				{
					planepnt->AI.lTimer2 = 160000;
				}
				planepnt->AI.lVar2 = 4;
				break;
			case 4:
				planepnt->AI.TargetPos.X = 180.0f / 20.0f;
				planepnt->AI.lTimer2 = 20000;
				planepnt->AI.lVar2 = 5;
				break;
			case 5:
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
				tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
				break;
			case 6:
				//  Start Push
				planepnt->AI.Behaviorfunc = AIFlyToDME10;
				AICCommencingPush(planepnt - Planes);
				planepnt->AI.lVar2 = 0;
				return;
				break;
			case 10:
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
				tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
				AICEstablishedMarshal(planepnt - Planes);
				break;
		}
	}
	else
	{
		deckangle = HELO_ANGLE;
		planepnt->AI.Behaviorfunc = AIFlyCarrierHeloMarshallPattern;
		switch(planepnt->AI.lVar2)
		{
			case 0:
				planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
				AICCommencingPush(planepnt - Planes);
				planepnt->AI.lVar2 = 0;
				planepnt->AI.lTimer3 = -1;
				break;
			case 1:
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 2, deckangle);
				break;
			case 2:
				if(planepnt->AI.lTimer3 < 10000)
				{
					planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
					AICCommencingPush(planepnt - Planes);
					planepnt->AI.lVar2 = 0;
					planepnt->AI.lTimer3 = -1;
					return;
				}

				if(planepnt->AI.lTimer3 < 50000)
				{
					planepnt->AI.TargetPos.X = 360.0f / ((float)planepnt->AI.lTimer3 / 1000.0f);
					planepnt->AI.lTimer2 = planepnt->AI.lTimer3;
					planepnt->AI.lVar2 = 6;
				}
				else
				{
					planepnt->AI.TargetPos.X = 180.0f / 20.0f;
					planepnt->AI.lTimer2 = 20000;
					planepnt->AI.lVar2 = 3;
				}
				break;
			case 3:
				if(planepnt->AI.lTimer3 < 330000)
				{
					planepnt->AI.lTimer2 = ((planepnt->AI.lTimer3 - 20000) / 2);
					if(planepnt->AI.lTimer2 < 0)
					{
						planepnt->AI.lTimer2 = 1;
					}
				}
				else
				{
					planepnt->AI.lTimer2 = 160000;
				}
				planepnt->AI.lVar2 = 4;
				break;
			case 4:
				planepnt->AI.TargetPos.X = 180.0f / 20.0f;
				planepnt->AI.lTimer2 = 20000;
				planepnt->AI.lVar2 = 5;
				break;
			case 5:
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
				tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
				break;
			case 6:
				planepnt->AI.Behaviorfunc = AIFlyToHeloDME3;
				AICCommencingPush(planepnt - Planes);
				planepnt->AI.lVar2 = 0;
				planepnt->AI.lTimer3 = -1;
				return;
				break;
			case 10:
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z, 1, deckangle);
				tdist = (planepnt->WorldPosition - planepnt->AI.WayPosition) / 2.0f;
				AISetOffCarrierRelPoint(planepnt, 0, planepnt->AI.TargetPos.Y, planepnt->AI.TargetPos.Z + tdist, 1, deckangle);
				AICEstablishedMarshal(planepnt - Planes);
				break;
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2FPoint()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 1 FPoint arguements.
 *
 */
void NetPutGenericMessage2FPoint(PlaneParams *planepnt, BYTE arg1, FPoint arg2, BYTE slotnum)
{
	DPMSG_GENERIC_2_FPOINT NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage2FPointSmall(planepnt, arg1, arg2, slotnum);
		return;
	}

	NetGen.type = NET_GENERIC_2_FPOINT;
	NetGen.time   = newTime;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2FPoint()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 1 FPoint.
 *
 */
void NetGetGenericMessage2FPoint( DPMSG_GENERIC_2_FPOINT *Net)
{
	DPMSG_GENERIC_2_FPOINT_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;

	NetGetGenericMessage2FPointSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2FPointSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 1 FPoint arguement.
 *
 */
void NetPutGenericMessage2FPointSmall(PlaneParams *planepnt, BYTE arg1, FPoint arg2, BYTE slotnum)
{
	DPMSG_GENERIC_2_FPOINT_SM NetGen;

	NetGen.type = NET_GENERIC_2_FPOINT_SM;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2FPointSmall()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 1 FPoint argument.
 *
 */
void NetGetGenericMessage2FPointSmall( DPMSG_GENERIC_2_FPOINT_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;
	MultipleExplosionType secondaries;
	DBShipType *pshiptype;
	DBVehicleType *pvehicletype;
	int num_secondaries;
	float secondary_delay;
	float dist_delay;
	FPointDouble coffset;
	BYTE bworkvar;

	switch(Net->arg1)
	{
		case GM2FP_WAYPOS:
			planepnt = &Planes[Net->slot];
			planepnt->AI.WayPosition = Net->arg2;
			break;
		case GM2FP_FORM_POS:
			planepnt = &Planes[Net->slot];
			planepnt->AI.FormationPosition = Net->arg2;
			break;
		case GM2FP_STORES:
			planepnt = &Planes[Net->slot];
			planepnt->AI.fStoresWeight = Net->arg2.X;
			if(Net->arg2.Y)
				planepnt->TotalFuel = Net->arg2.Y;
			if(Net->arg2.Z)
				planepnt->AircraftDryWeight = Net->arg2.Z;
			break;
		case GM2FP_VEH_POS:

			if(GameLoop > 500)
			{
				vehiclepnt = &MovingVehicles[Net->slot];

				vehiclepnt->WorldPosition.X = Net->arg2.X;
				vehiclepnt->WorldPosition.Z = Net->arg2.Z;
				vehiclepnt->Heading = AIConvert180DegreeToAngle(Net->arg2.Y);

				if(vehiclepnt->lAIFlags1 & PLANES_ON_DECK)
				{
					VUpdatePlanesOnDeck(vehiclepnt);
				}
			}
			break;
		case GM2FP_VEH_S_EXP:
			num_secondaries = (int)Net->arg2.X;
			secondary_delay = Net->arg2.Z;
			dist_delay = Net->arg2.Y;

			vehiclepnt = &MovingVehicles[Net->slot];
			if(vehiclepnt->iShipType)
			{
				pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
				secondaries.WarheadID = pshiptype->lExpWeaponType;
				secondaries.NumberOfExplosions = pshiptype->iNumExplosions;
				secondaries.Radius = (BYTE)((float)pshiptype->iRadius FEET);
				secondaries.Flags = 0;  //  Not sure
				secondaries.TimeBetweenExplosions = pshiptype->iTicksBetweenBlasts;
				secondaries.Deviation = pshiptype->iVarianceBetweenBlasts;  // Not sure

			}
			else
			{
				pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];
				secondaries.WarheadID = pvehicletype->lExpWeaponType;
				secondaries.NumberOfExplosions = pvehicletype->iNumExplosions;
				secondaries.Radius = (BYTE)((float)pvehicletype->iRadius FEET);
				secondaries.Flags = 0;  //  Not sure
				secondaries.TimeBetweenExplosions = pvehicletype->iTicksBetweenBlasts;
				secondaries.Deviation = pvehicletype->iVarianceBetweenBlasts;  // Not sure
			}

			if (num_secondaries)
			{
				if (!secondary_delay)
					ScheduleExplosion(vehiclepnt->WorldPosition,secondaries, dist_delay,TRUE,NULL,NULL,vehiclepnt);
				else
				{
					ScheduleExplosion(vehiclepnt->WorldPosition,secondaries, dist_delay,FALSE,NULL,NULL,vehiclepnt);
					ScheduleExplosion(vehiclepnt->WorldPosition,secondaries, secondary_delay + dist_delay);
				}
			}
			else
				ScheduleExplosion(vehiclepnt->WorldPosition,secondaries, dist_delay,FALSE,NULL,NULL,vehiclepnt);

			break;
		case GM2FP_PUT_ON_DECK:
			planepnt = &Planes[Net->slot];

			if((planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (!planepnt->PlaneCopy) && (planepnt->OnGround != 2))
			{
				planepnt->OnGround = 2;
				vehiclepnt = &MovingVehicles[Planes[Net->slot].AI.iHomeBaseId];
				coffset = Net->arg2;
				planepnt->WorldPosition = vehiclepnt->WorldPosition;
				planepnt->WorldPosition += coffset;
				if (planepnt->PlaneCopy)
				{
					free(planepnt->PlaneCopy);
					planepnt->PlaneCopy = NULL;
				}
				planepnt->PlaneCopy = GetNewCarrierPlane(planepnt);
			}
			break;
		case GM2FP_BUDDY_LASER:
			if(iSlotToPlane[Net->slot] != -1)
			{
				if(Planes[iSlotToPlane[Net->slot]].AI.iSide == PlayerPlane->AI.iSide)
				{
					if((Net->arg2.X >= 0) && (NetBuddyLasingData[Net->slot].laserloc.X < 0))
					{
						AIC_GenericMsgPlane(iSlotToPlane[Net->slot], 30);
					}
					else if((Net->arg2.X < 0) && (NetBuddyLasingData[Net->slot].laserloc.X >= 0))
					{
						AIC_GenericMsgPlane(iSlotToPlane[Net->slot], 31);
					}
					NetBuddyLasingData[Net->slot].laserloc = Net->arg2;
				}
			}
			break;
		case GM2FP_WEATHER_INFO:
			if(!g_bIAmHost)
			{
				bworkvar = Net->slot;
				SnowOn = (bworkvar & 0x1) ? 1 : 0;
				doLightning = (bworkvar & 0x2) ? 1 : 0;
				doRain = (bworkvar & 0x4) ? 1 : 0;

				WorldParams.Weather = (int)Net->arg2.X;
				WorldParams.CloudAlt = Net->arg2.Y;
				WorldParams.Visibility = Net->arg2.Z;
				ShutdownClouds();
				LoadCloudTexture();
				LoadSkyPalettes((WorldParams.Weather & WR_FLATCLOUD) >> 4);
			}
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessageObjectPoint()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		a type and ID value for an object
 *
 */
void NetPutGenericMessageObjectPoint(PlaneParams *planepnt, BYTE arg1, BYTE objtype, DWORD objectid)
{
	DPMSG_GENERIC_OBJ_POINT NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessageObjectPointSmall(planepnt, arg1, objtype, objectid);
		return;
	}

	NetGen.type = NET_GENERIC_OBJ_POINT;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.objtype = objtype;
	NetGen.dwObjSerialNum = objectid;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessageObjectPoint()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		a type and ID value for an object
 *
 */
void NetGetGenericMessageObjectPoint( DPMSG_GENERIC_OBJ_POINT *Net)
{
	DPMSG_GENERIC_OBJ_POINT_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.objtype = Net->objtype;
	NetSmall.dwObjSerialNum = Net->dwObjSerialNum;

	NetGetGenericMessageObjectPointSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessageObjectPointSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		a type and ID value for an object
 *
 */
void NetPutGenericMessageObjectPointSmall(PlaneParams *planepnt, BYTE arg1, BYTE objtype, DWORD objectid)
{
	DPMSG_GENERIC_OBJ_POINT_SM NetGen;

	NetGen.type = NET_GENERIC_OBJ_POINT_SM;
	NetGen.slot	= planepnt - Planes;
	NetGen.arg1 = arg1;
	NetGen.objtype = objtype;
	NetGen.dwObjSerialNum = objectid;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessageObjectPointSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		a type and ID value for an object
 *
 */
void NetGetGenericMessageObjectPointSmall( DPMSG_GENERIC_OBJ_POINT_SM *Net)
{
	PlaneParams *planepnt;
	BasicInstance *target;
	AAWeaponInstance *gweapon;
	InfoProviderInstance *radarprovider = NULL;
	GDRadarData *radardat;

	planepnt = &Planes[Net->slot];
	switch(Net->arg1)
	{
		case GMOP_GROUND_TARG:
			planepnt->AI.lGroundTargetFlag = Net->objtype;
			planepnt->AI.pGroundTarget = NetGetObjectPointer(Net->objtype, Net->dwObjSerialNum);
			break;
		case GMOP_PAINTED_BY:
			planepnt->AI.iPaintedByType = Net->objtype;
			planepnt->AI.pPaintedBy = NetGetObjectPointer(Net->objtype, Net->dwObjSerialNum);
			break;
		case GMOP_FAC_TARG:
			iFACTargetType = Net->objtype;
			pFACTarget = NetGetObjectPointer(Net->objtype, Net->dwObjSerialNum);
			break;
		case GMOP_GROUND_DEAD:
		case GMOP_GROUND_NO_WEAPON:
		case GMOP_GROUND_NO_RADAR:
			target = FindInstance(AllInstances, Net->dwObjSerialNum);
			if(target)
			{
				if(target->Family == FAMILY_INFOPROVIDER)
				{
					radarprovider = (InfoProviderInstance *)target;
					radardat = (GDRadarData *)radarprovider->AIDataBuf;
					radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
					radardat->lRFlags1 |= (GD_I_AM_DEAD);
				}
				else if(target->Family == FAMILY_AAWEAPON)
				{
					gweapon = (AAWeaponInstance *)target;
					radardat = (GDRadarData *)gweapon->AIDataBuf;
					radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
					radardat->lRFlags1 |= (GD_I_AM_DEAD);
				}
			}
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetGetObjectPointer()
 *
 *		Takes and object type (VEHCILE, GROUND_OBJECT, ...) and a serial
 *	number/id and returns a void pointer to that object.
 *
 */
void *NetGetObjectPointer(int objtype, DWORD serialnumber)
{
	switch(objtype)
	{
		case AIRCRAFT:
			return(&Planes[serialnumber]);
			break;
		case MOVINGVEHICLE:
		case SHIP:
			return(&MovingVehicles[serialnumber]);
			break;
		case GROUNDOBJECT:
			return(FindInstance(AllInstances, serialnumber));
			break;
	}
	return(NULL);
}

NetSlot Slotarray[MAX_HUMANS + 1];
int iInGameSlots = 0;
/*----------------------------------------------------------------------------
 *
 *	NetGetMissionHumanAssignments()
 *
 *		When reading in the mission figure out which human goes with which plane.
 *
 *
 */
int NetGetMissionHumanAssignments()
{
	int cnt;
	PlaneParams tempplanes[MAX_HUMANS];
	PlaneParams *planepnt;
	PlaneParams *leadplane;
	char sDebugTxt[260];
	NetSlot S;
	int planenum;
	PlaneParams *pLastPlane = &Planes[iNumAIObjs - 1];
	DPNAME *dpName = NULL;

	iInGameSlots = 1;

#if 1
	int hMissionFile;

	hMissionFile=_open(RegPath("resource","pindex.dat"),_O_BINARY | _O_RDONLY);

	if (hMissionFile == -1)
	{
		sprintf(sDebugTxt,"%s:Error - Could not open File %s",__FILE__,"slot to plane");
		OutputDebugString (sDebugTxt);

		LogError("Could not open file",__FILE__,__LINE__,0);
		return(0);
	}

	_read(hMissionFile,iMissionSlotToPlane,sizeof(int) * MAX_HUMANS);

	_close(hMissionFile);

#else
	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		iMissionSlotToPlane[cnt] = -1;
	}
	iMissionSlotToPlane[0] = 0;
	iMissionSlotToPlane[1] = 13;
#endif

	int tempwix = NetWeapIX;

	//  I think this may get get goofed up, check for sure.
	NetWeapIX = (MySlot << 16) + 256;	// Init my GUID

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if((cnt != MySlot) && (Slot[cnt].dpid))
		{
			DPNAME *dpName = NULL;
			dpGetPlayerName( Slot[cnt].dpid, (void**)&dpName );

			if (dpName)
				strncpy( Slot[cnt].name, dpName->lpszShortNameA, sizeof(Slot[cnt].name)-1 );

		}

		tempplanes[cnt] = Planes[cnt];
		iSlotToPlane[cnt] = iMissionSlotToPlane[cnt];
		Slotarray[cnt] = Slot[cnt];
		Slot[cnt] = Slot[MAX_HUMANS];
	}
	Slotarray[MAX_HUMANS] = Slot[MAX_HUMANS];

	for(planepnt = Planes, planenum = 0; planenum < iNumAIObjs; planepnt ++, planenum ++)
	{
//		planepnt->Status &= ~(PL_DEVICE_DRIVEN|PL_COMM_DRIVEN);
//		planepnt->Status |= PL_AI_DRIVEN;
		planepnt->Status = 0;
	}

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if((iSlotToPlane[cnt] >= 0) && (iSlotToPlane[cnt] <= iNumAIObjs))
		{
			if(MySlot == cnt)
			{
				Planes[iSlotToPlane[cnt]].Status |= (PL_DEVICE_DRIVEN|PL_ACTIVE|PL_NEED_ATTITUDE);
			}
			else
			{
				Planes[iSlotToPlane[cnt]].Status |= (PL_COMM_DRIVEN|PL_ACTIVE|PL_NEED_ATTITUDE);
			}
			planenum = iSlotToPlane[cnt];

#if 1
 			Slot[planenum] = Slotarray[cnt];
#else  //  Below has problems, just cleared out Slots up to MaxHumans and did above instead.
			if(planenum != cnt)
			{
				if((planenum >= MAX_HUMANS))
				{
					S = Slot[planenum];
					Slot[planenum] = Slot[cnt];
					Slot[cnt] = S;
				}
				else if((iSlotToPlane[planenum] == -1))
				{
					S = Slot[planenum];
					Slot[planenum] = Slot[cnt];
					Slot[cnt] = S;
				}
				else
				{
					Slot[planenum] = Slotarray[cnt];
				}
			}
#endif
		}
	}

	for(planepnt = Planes, planenum = 0; planenum < iNumAIObjs; planepnt ++, planenum ++)
	{
		Slot[planenum].x.chTeam = AIObjects[planenum].iSide;
		planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;
		if(!(planepnt->Status & (PL_COMM_DRIVEN|PL_DEVICE_DRIVEN)))
		{
			leadplane = planepnt;
			if(g_bIAmHost)
			{
				planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
			}

#if 0
			while(leadplane)
			{
				if(leadplane->AI.winglead >= 0)
				{
					leadplane = &Planes[leadplane->AI.winglead];
				}
				else if(leadplane->AI.prevpair >= 0)
				{
					leadplane = &Planes[leadplane->AI.prevpair];
				}
				else
				{
					leadplane = NULL;
				}

				if(leadplane)
				{
					if(leadplane->Status & AL_COMM_DRIVEN)
					{
						leadplane = NULL;
						planepnt->AI.iAICombatFlags1 &= ~(AI_MULTI_ACTIVE);
					}
					else if(leadplane->Status & AL_DEVICE_DRIVEN)
					{
						planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
						leadplane = NULL;
					}
				}
			}
#endif
		}
		else if(planepnt->Status & (PL_DEVICE_DRIVEN))
		{
			planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
		}
	}
	return(1);
}

//============================================================================
//		GENERAL VEHICLE GROUND RADAR LOCKUP MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutVehicleGroundLock(Vehicle number, Index into RadarWeaponData, PlaneParams *planepnt)
 *
 *		Sets things up in MultiPlayer so that TEWS will show the correct lock information on all systems.
 *
 */
void NetPutVehicleGroundLock(int vnum, int windex, int radardatindex, PlaneParams *planepnt)
{
	DPMSG_VGROUND_LOCK  NetVGroundLock;	// static packet for sending

	if(iMultiCast >= 0)
	{
		NetPutVehicleGroundLockSmall(vnum, windex, radardatindex, planepnt);
		return;
	}

	NetVGroundLock.type   = NET_VGROUND_LOCK;
	NetVGroundLock.time   = newTime;

	NetVGroundLock.slot	  = PlayerPlane - Planes;

	NetVGroundLock.target = planepnt ? (planepnt - Planes) : 255;

	NetVGroundLock.VNum = vnum;

	NetVGroundLock.WRDNum = windex;

	NetVGroundLock.RDNum = radardatindex;

#if 0
	char netstr[80];
	AAWeaponInstance *launcher;
	launcher = (AAWeaponInstance *)foundweapon;
	sprintf(netstr, "PUT lock from %s SN %ld radar %ld, on %d", launcher->Physicals.Object->Filename, NetGroundLock.dwWeaponSerialNum, NetGroundLock.dwRadarSerialNum, NetGroundLock.target);
	AICAddAIRadioMsgs(netstr, 50);
#endif

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetVGroundLock, sizeof(NetVGroundLock) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetVehicleGroundLock()
 *
 *		Lock up a plane with a ground radar
 *
 */
void NetGetVehicleGroundLock( DPMSG_VGROUND_LOCK *Net )
{
	DPMSG_VGROUND_LOCK_SM	NetVGroundLockSmall;

	NetVGroundLockSmall.slot = Net->slot;
	NetVGroundLockSmall.target = Net->target;
	NetVGroundLockSmall.VNum = Net->VNum;
	NetVGroundLockSmall.RDNum = Net->RDNum;

	NetGetVehicleGroundLockSmall(&NetVGroundLockSmall);
}

//============================================================================
//		Small GENERAL VEHICLE GROUND RADAR LOCKUP MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutVehicleGroundLockSmall(int vnum, int radardatindex, PlaneParams *planepnt)
 *
 *		Sets things up in MultiPlayer so that TEWS will show the correct lock information on all systems.
 *
 */
void NetPutVehicleGroundLockSmall(int vnum, int windex, int radardatindex, PlaneParams *planepnt)
{
	DPMSG_VGROUND_LOCK_SM	NetVGroundLockSmall;

	NetVGroundLockSmall.type   = NET_VGROUND_LOCK_SM;

	NetVGroundLockSmall.slot	  = PlayerPlane - Planes;

	NetVGroundLockSmall.target = planepnt ? (planepnt - Planes) : 255;

	NetVGroundLockSmall.VNum = vnum;

	NetVGroundLockSmall.WRDNum = windex;

	NetVGroundLockSmall.RDNum = radardatindex;

#if 0
	char netstr[80];
	sprintf(netstr, "PUT lock from VN %d W %d radar %d, on %d", NetVGroundLockSmall.VNum, NetVGroundLockSmall.WRDNum, NetVGroundLockSmall.RDNum, NetVGroundLockSmall.target);
	AICAddAIRadioMsgs(netstr, 50);
#endif

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetGroundLockSmall, sizeof(NetGroundLockSmall) );
	NetAddToBigPacket(MySlot, &NetVGroundLockSmall, sizeof(NetVGroundLockSmall) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetVehicleGroundLockSmall()
 *
 *		Lock up a plane with a ground radar
 *
 */
void NetGetVehicleGroundLockSmall( DPMSG_VGROUND_LOCK_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;
	GDRadarData *wradardat;
	GDRadarData *radardat;
	int notradar;
	int isaaa;
	float currdist;
	DBWeaponType *weapon;
	DBRadarType *radar;
	int debugnum = 0;
	int radardatnum = 0;
	int rradardatnum = 0;

	planepnt = (Net->target == 255) ? NULL : &Planes[Net->target];

	if(Net->VNum != 255)
	{
		vehiclepnt = &MovingVehicles[Net->VNum];
		if(vehiclepnt->bDamageLevel > 1)
		{
			NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
		}
	}
	else
	{
		vehiclepnt = NULL;
	}

	if((Net->WRDNum != 255) && (vehiclepnt))
	{
		wradardat = &vehiclepnt->RadarWeaponData[Net->WRDNum];
		radardatnum = Net->WRDNum;
	}
	else
	{
		wradardat = NULL;
	}


	if((Net->RDNum != 255) && (vehiclepnt))
	{
		radardat = &vehiclepnt->RadarWeaponData[Net->RDNum];
		rradardatnum = Net->RDNum;
	}
	else
	{
		radardat = NULL;
	}

	if(!(planepnt && vehiclepnt))
	{
		return;
	}

	currdist = planepnt->WorldPosition - vehiclepnt->WorldPosition;

	if(radardat)
	{
		radardat->Target = planepnt;
		radardat->iTargetDist = currdist * WUTONM;
	}

	if (!vehiclepnt->iShipType) // DONE XSCOTT FIX
	{
		radar = GetRadarPtr(pDBVehicleList[vehiclepnt->iVDBIndex].lRadarID);
		if(!radardatnum)
		{
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
		}
		else
		{
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
		}
	}
	else
	{
		radar = GetRadarPtr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[rradardatnum].lTypeID);
		weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID);
	}


	if(!weapon)
	{
		return;
	}

	if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10) || (weapon->iWeaponType == 6))
	{
		isaaa = 1;
		radardat->lWFlags1 &= ~GD_W_ACQUIRING;
	}
	else if((weapon->iSeekerType != 1) && (weapon->iSeekerType != 7))
	{
		notradar = 1;
		isaaa = 0;
	}
	else
	{
		notradar = 0;
		isaaa = 0;
	}

	if(!radardat)
		return;

	if((radardat == wradardat) || (!radardat))
	{
		debugnum = 7;
		if(radar && ((planepnt->AI.pPaintedBy == NULL) || (planepnt->AI.iPaintDist > (currdist * WUTONM))))
		{
			planepnt->AI.pPaintedBy = vehiclepnt;
			planepnt->AI.iPaintedByType = MOVINGVEHICLE;
			planepnt->AI.iPaintDist = currdist * WUTONM;
		}

		if((radardat) && (radar))
		{
//			if(weapon->iSeekerType == 1)
			if(radar->iRadarType & RADAR_TYPE_TWS)
			{
				radardat->lRFlags1 |= GD_CHECK_MISSILES;
			}
			else //  if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
			{
				radardat->lRFlags1 |= GD_RADAR_LOCK;
				GDCheckForPingReport(vehiclepnt, planepnt, NULL, MOVINGVEHICLE);
			}
		}

		if(wradardat)
			wradardat->pWRadarSite = radardat;

		if(isaaa)
		{
			debugnum = 8;
			GDFireInitialAAA(vehiclepnt, planepnt, MOVINGVEHICLE, radardatnum);
		}
	}
	else
	{
		if(wradardat)
			wradardat->pWRadarSite = radardat;

		if((planepnt->AI.pPaintedBy == NULL) || (planepnt->AI.iPaintDist > (currdist * WUTONM)))
		{
			planepnt->AI.pPaintedBy = vehiclepnt;
			planepnt->AI.iPaintedByType = MOVINGVEHICLE;
			planepnt->AI.iPaintDist = currdist * WUTONM;
		}

		debugnum = 1;
//		if(weapon->iSeekerType == 1)
		if(radar && (radar->iRadarType & RADAR_TYPE_TWS))
		{
			radardat->lRFlags1 |= GD_CHECK_MISSILES;
			radardat->Target = planepnt;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
			debugnum = 2;
		}
		else if(weapon->iSeekerType == 7)
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			radardat->Target = planepnt;
			GDCheckForPingReport(vehiclepnt, planepnt, NULL, MOVINGVEHICLE);
			debugnum = 3;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
		}
		else if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10))  // Not sure about 10
		{
			radardat->lRFlags1 |= GD_RADAR_LOCK;
			radardat->Target = planepnt;
			GDCheckForPingReport(vehiclepnt, planepnt, NULL, MOVINGVEHICLE);
			debugnum = 4;
			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
				if(radardat->lRActionTimer > 60000)
				{
					radardat->lRActionTimer = 60000;
				}
			}
			if(isaaa)
			{
				debugnum = 5;
				GDFireInitialAAA(vehiclepnt, planepnt, MOVINGVEHICLE, radardatnum);
			}
		}
		else
		{
			if(isaaa)
			{
				debugnum = 6;
				GDFireInitialAAA(vehiclepnt, planepnt, MOVINGVEHICLE, radardatnum);
			}
		}
	}

	if(radardat)
	{
		if(planepnt == PlayerPlane)
		{
			if(radar)
			{
				radardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
			}
		}
	}

#if 0
	char netstr[80];
	sprintf(netstr, "GET lock from VN %d W %d radar %d, on %d", Net->VNum, Net->WRDNum, Net->RDNum, Net->target);
	AICAddAIRadioMsgs(netstr, 50);
#endif
}

//============================================================================
//		GENERAL SS MISSILE MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutSSMissileGeneral()
 *
 *		Tell the world that we dropped a bomb.
 *
 *		Hook "InstantiateAGMissile()" and send a guaranteed message.
 *
 */
int NetPutSSMissileGeneral(MovingVehicleParams *vehiclepnt, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile)
{
	BasicInstance  *currweapon;

	if(iMultiCast >= 0)
	{
		return(NetPutSSMissileGeneralSmall(vehiclepnt, Station, GroundTarget, targetloc, groundtype, flightprofile));
	}

	NetAGMissileGeneral.type   = NET_AG_MISSILE_GENERAL;
	NetAGMissileGeneral.time   = newTime;

	if(vehiclepnt)
	{
		NetAGMissileGeneral.slot	  = vehiclepnt - MovingVehicles;
	}
	else
	{
		NetAGMissileGeneral.slot	  = 255;
	}

	NetAGMissileGeneral.weapon = Station | 0x40;
	NetAGMissileGeneral.weapIX = ++NetWeapIX;	// assign next GUID
	NetAGMissileGeneral.profile = flightprofile;

	if(GroundTarget)
	{
		if(groundtype == MOVINGVEHICLE)
		{
			NetAGMissileGeneral.dwGrndSerialNum = (MovingVehicleParams *)GroundTarget - MovingVehicles;
		}
		else
		{
			currweapon  = (BasicInstance *)GroundTarget;
			NetAGMissileGeneral.dwGrndSerialNum = currweapon->SerialNumber;
		}
	}
	else
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneral.dwGrndSerialNum = 0;
	}
	NetAGMissileGeneral.groundtype = groundtype;

	NetAGMissileGeneral.targetloc = targetloc;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAGMissileGeneral, sizeof(NetAGMissileGeneral) );

	return NetWeapIX;
}

//============================================================================
//		Small GENERAL AG MISSILE MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutSSMissileGeneralSmall()
 *
 *		Tell the world that we dropped a bomb.
 *
 *		Hook "InstantiateAGMissile()" and send a guaranteed message.
 *
 */
int NetPutSSMissileGeneralSmall(MovingVehicleParams *vehiclepnt, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile)
{
	BasicInstance  *currweapon;
	DPMSG_AG_MISSILE_GENERAL_SM NetAGMissileGeneralSmall;

	NetAGMissileGeneralSmall.type   = NET_AG_MISSILE_GENERAL_SM;

	if(vehiclepnt)
	{
		NetAGMissileGeneralSmall.slot	  = vehiclepnt - MovingVehicles;
	}
	else
	{
		NetAGMissileGeneralSmall.slot	  = 255;
	}

	NetAGMissileGeneralSmall.weapon = Station | 0x40;
	NetAGMissileGeneralSmall.weapIX = ++NetWeapIX;	// assign next GUID
	NetAGMissileGeneralSmall.profile = flightprofile;

	if(GroundTarget)
	{
		if(groundtype == MOVINGVEHICLE)
		{
			NetAGMissileGeneralSmall.dwGrndSerialNum = (MovingVehicleParams *)GroundTarget - MovingVehicles;
		}
		else
		{
			currweapon  = (BasicInstance *)GroundTarget;
			NetAGMissileGeneralSmall.dwGrndSerialNum = currweapon->SerialNumber;
		}
	}
	else
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneralSmall.dwGrndSerialNum = 0;
	}
	NetAGMissileGeneralSmall.groundtype = groundtype;

	NetAGMissileGeneralSmall.targetloc = targetloc;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAGMissileGeneralSmall, sizeof(NetAGMissileGeneralSmall) );
	NetAddToBigPacket(MySlot, &NetAGMissileGeneralSmall, sizeof(NetAGMissileGeneralSmall) );

	return NetWeapIX;
}

//============================================================================
//		Weapon As Plane MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutWeaponPlaneGeneral()
 *
 *		Host tells world a weapon plane has been generated.
 *
 *
 *
 */
int NetPutWeaponPlaneGeneral(void *launcher, int launchertype, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile, int planenum)
{
	BasicInstance  *currweapon;

	if(iMultiCast >= 0)
	{
		return(NetPutWeaponPlaneGeneralSmall(launcher, launchertype, Station, GroundTarget, targetloc, groundtype, flightprofile, planenum));
	}

	NetAGMissileGeneral.type   = NET_AG_MISSILE_GENERAL;
	NetAGMissileGeneral.time   = newTime;

	if(launchertype == MOVINGVEHICLE)
	{
		NetAGMissileGeneral.slot	  = (MovingVehicleParams *)launcher - MovingVehicles;
	}
	else
	{
		NetAGMissileGeneral.slot	  = (PlaneParams *)launcher - Planes;
	}

	NetAGMissileGeneral.weapon = Station | 0x20;

	if(launchertype == MOVINGVEHICLE)
		NetAGMissileGeneral.weapon |= 0x40;

	NetAGMissileGeneral.weapIX = planenum;	// assign next GUID
	NetAGMissileGeneral.profile = flightprofile;

	if(GroundTarget)
	{
		if(groundtype == MOVINGVEHICLE)
		{
			NetAGMissileGeneral.dwGrndSerialNum = (MovingVehicleParams *)GroundTarget - MovingVehicles;
		}
		else
		{
			currweapon  = (BasicInstance *)GroundTarget;
			NetAGMissileGeneral.dwGrndSerialNum = currweapon->SerialNumber;
		}
	}
	else
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneral.dwGrndSerialNum = 0;
	}
	NetAGMissileGeneral.groundtype = groundtype;

	NetAGMissileGeneral.targetloc = targetloc;

	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAGMissileGeneral, sizeof(NetAGMissileGeneral) );

	return NetWeapIX;
}

//============================================================================
//		Small Weapon As Plane MESSAGE PROCESSING
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	NetPutWeaponPlaneGeneralSmall()
 *
 *		Host tells world a weapon plane has been generated.
 *
 *
 *
 */
int NetPutWeaponPlaneGeneralSmall(void *launcher, int launchertype, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int flightprofile, int planenum)
{
	BasicInstance  *currweapon;
	DPMSG_AG_MISSILE_GENERAL_SM NetAGMissileGeneralSmall;

	NetAGMissileGeneralSmall.type   = NET_AG_MISSILE_GENERAL_SM;

	if(launchertype == MOVINGVEHICLE)
	{
		NetAGMissileGeneralSmall.slot	  = (MovingVehicleParams *)launcher - MovingVehicles;
	}
	else
	{
		NetAGMissileGeneralSmall.slot	  = (PlaneParams *)launcher - Planes;
	}

	NetAGMissileGeneralSmall.weapon = Station | 0x20;

	if(launchertype == MOVINGVEHICLE)
		NetAGMissileGeneralSmall.weapon |= 0x40;

	NetAGMissileGeneralSmall.weapIX = planenum;	// assign next GUID
	NetAGMissileGeneralSmall.profile = flightprofile;

	if(GroundTarget)
	{
		if(groundtype == MOVINGVEHICLE)
		{
			NetAGMissileGeneralSmall.dwGrndSerialNum = (MovingVehicleParams *)GroundTarget - MovingVehicles;
		}
		else
		{
			currweapon  = (BasicInstance *)GroundTarget;
			NetAGMissileGeneralSmall.dwGrndSerialNum = currweapon->SerialNumber;
		}
	}
	else
	{
		groundtype = NO_TARGET;
		NetAGMissileGeneralSmall.dwGrndSerialNum = 0;
	}
	NetAGMissileGeneralSmall.groundtype = groundtype;

	NetAGMissileGeneralSmall.targetloc = targetloc;

//	NetSend( DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetAGMissileGeneralSmall, sizeof(NetAGMissileGeneralSmall) );

	NetAddToBigPacket(MySlot, &NetAGMissileGeneralSmall, sizeof(NetAGMissileGeneralSmall) );

	return NetWeapIX;
}

/*----------------------------------------------------------------------------
 *
 *	MAILimitVGroundLockSend
 *
 *		Only send VGroundLock packets to those who need them.
 *
 */
int MAILimitVGroundLockSend(DPMSG_VGROUND_LOCK_SM *Net)
{
	int X, Y;

	if(Net->target > 200)
	{
		return(-1);
	}

	MAIGetHotZoneIndex((float)Planes[Net->target].WorldPosition.X, (float)Planes[Net->target].WorldPosition.Z, &X, &Y);
	return(MAICheckHotZones(X, Y));
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2Float()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 1 Float arguements.
 *
 */
void NetPutGenericMessage2Float(PlaneParams *planepnt, BYTE arg1, float arg2, BYTE slotnum)
{
	DPMSG_GENERIC_2_FLOAT NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage2FloatSmall(planepnt, arg1, arg2, slotnum);
		return;
	}

	NetGen.type = NET_GENERIC_2_FLOAT;
	NetGen.time   = newTime;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2Float()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 1 Float.
 *
 */
void NetGetGenericMessage2Float( DPMSG_GENERIC_2_FLOAT *Net)
{
	DPMSG_GENERIC_2_FLOAT_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;

	NetGetGenericMessage2FloatSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage2FloatSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 1 Float arguement.
 *
 */
void NetPutGenericMessage2FloatSmall(PlaneParams *planepnt, BYTE arg1, float arg2, BYTE slotnum)
{
	DPMSG_GENERIC_2_FLOAT_SM NetGen;

	NetGen.type = NET_GENERIC_2_FLOAT_SM;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage2FloatSmall()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 1 Float argument.
 *
 */
void NetGetGenericMessage2FloatSmall( DPMSG_GENERIC_2_FLOAT_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;

	switch(Net->arg1)
	{
		case GM2F_VEH_DAMAGE:
			vehiclepnt = &MovingVehicles[Net->slot];
			NetVehicleDamage(vehiclepnt, Net->arg2);
			break;
		case GM2F_TARGET_Y:
			planepnt = &Planes[Net->slot];
			planepnt->AI.TargetPos.Y = Net->arg2;
			break;
		case GM2F_GRND_TARG_AT:
			AIC_GenericMsgPlaneBearing(Net->slot, 0, Net->arg2);
			break;
		case GM2F_SHIP_AT:
			AIC_GenericMsgPlaneBearing(Net->slot, 1, Net->arg2);
			break;
		case GM2F_STROBE_AT:
			AIC_GenericMsgPlaneBearing(Net->slot, 2, Net->arg2);
			break;
		case GM2F_CHK_SHADOW:
			planepnt = &Planes[Net->slot];
			if(pDBAircraftList[planepnt->AI.iPlaneIndex].fShadowLRZOff != Net->arg2)
			{
				AICGiveHackMessage(Net->slot);
			}
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3Float()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 2 Float arguements.
 *
 */
void NetPutGenericMessage3Float(PlaneParams *planepnt, BYTE arg1, float arg2, float arg3, BYTE slotnum)
{
	DPMSG_GENERIC_3_FLOAT NetGen;

	if(iMultiCast >= 0)
	{
		NetPutGenericMessage2FloatSmall(planepnt, arg1, arg2, slotnum);
		return;
	}

	NetGen.type = NET_GENERIC_3_FLOAT;
	NetGen.time   = newTime;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage3Float()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 2 Float.
 *
 */
void NetGetGenericMessage3Float( DPMSG_GENERIC_3_FLOAT *Net)
{
	DPMSG_GENERIC_3_FLOAT_SM NetSmall;

	NetSmall.slot = Net->slot;
	NetSmall.arg1 = Net->arg1;
	NetSmall.arg2 = Net->arg2;
	NetSmall.arg3 = Net->arg3;

	NetGetGenericMessage3FloatSmall(&NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutGenericMessage3FloatSmall()
 *
 *		Generic Message routine for basic message passing with a plane ID and
 *		1 BYTE and 2 Float arguements.
 *
 */
void NetPutGenericMessage3FloatSmall(PlaneParams *planepnt, BYTE arg1, float arg2, float arg3, BYTE slotnum)
{
	DPMSG_GENERIC_3_FLOAT_SM NetGen;

	NetGen.type = NET_GENERIC_3_FLOAT_SM;
	if(planepnt)
	{
		NetGen.slot	= planepnt - Planes;
	}
	else
	{
		NetGen.slot = slotnum;
	}
	NetGen.arg1 = arg1;
	NetGen.arg2 = arg2;
	NetGen.arg3 = arg3;

	NetAddToBigPacket(MySlot, &NetGen, sizeof(NetGen) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetGenericMessage3FloatSmall()
 *
 *		Get Generic Message for basic message passing with a plane ID and
 *		1 BYTE and 2 Float argument.
 *
 */
void NetGetGenericMessage3FloatSmall( DPMSG_GENERIC_3_FLOAT_SM *Net )
{
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;

	switch(Net->arg1)
	{
		case 0:  //  BOGUS for now
			planepnt = &Planes[Net->slot];
			break;
		default:
			vehiclepnt = &MovingVehicles[Net->slot];
			break;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetVehicleDamage()
 *
 *		Update the total permanent damage to a vehicle.
 *		I'm not worrying about sustained since the only person who's weapons are going
 *		to hit close enough in time is the original damager it didn't seem worth the bandwidth.
 *
 */
void NetVehicleDamage(MovingVehicleParams *vehiclepnt, float newdamage)
{
	int damagelevel;
	DBShipType *pshiptype;
	DBVehicleType *pvehicletype;
	BOOL return_value = FALSE;
	int our_num_seconds = 0;

	vehiclepnt->fTotalDamage = newdamage;

	if(vehiclepnt->iShipType)
	{
		pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];

		damagelevel = 0;
		if(vehiclepnt->fTotalDamage > pshiptype->iDamageLevel1)
		{
			damagelevel = 1;
			if(vehiclepnt->fTotalDamage > pshiptype->iDamageLevel2)
			{
				damagelevel = 2;
				if(vehiclepnt->fTotalDamage > pshiptype->iDamageLevel3)
				{
					damagelevel = 3;
					if(vehiclepnt->fTotalDamage > pshiptype->iPermanentDamage)
					{
						damagelevel = 4;
					}
				}
			}
		}

		if(damagelevel > vehiclepnt->bDamageLevel)
		{
			vehiclepnt->bDamageLevel = damagelevel;
			VSetShipDamageLevel(vehiclepnt - MovingVehicles, damagelevel, 0);
		}

		if (vehiclepnt->fTotalDamage > pshiptype->iPermanentDamage)
		{
			return_value = TRUE;
		}
	}
	else
	{
		pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];
		if (vehiclepnt->fTotalDamage > pvehicletype->iPermanentDamage)
		{
			return_value = TRUE;
		}
	}

	if(!return_value)
		return;

	//  I copied this out of DestroyVehicle and DestroyShip.  I think there are conditions where they wouldn't be called.
	if(!(vehiclepnt->Status & VL_GOAL_CHECKED))
	{
		AIUpdateGoalStates(vehiclepnt->iVListID, vehiclepnt->iVListID, vehiclepnt->iVListID, MOVINGVEHICLE);
		vehiclepnt->Status |= VL_GOAL_CHECKED;
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetConvertFirstAndLastValid()
 *
 *		For MultiPlayer determine new first and last valid for wingman comm stuff so
 *		that non-flightlead humans can control their wingmen.
 *
 */
void NetConvertFirstAndLastValid(int firstvalid, int lastvalid, int *retfirst, int *retlast)
{
	PlaneParams *planepnt;
	int leadplace;
	int workfirst, worklast;
	int minfirst, maxlast;

	if(!MultiPlayer)
	{
		*retfirst = firstvalid;
		*retlast = lastvalid;
		return;
	}

	if(iAICommFrom < 0)
		planepnt = PlayerPlane;
	else
		planepnt = &Planes[iAICommFrom];

	leadplace = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);

	workfirst = firstvalid + leadplace;
	worklast = lastvalid + leadplace;

	switch(leadplace)
	{
		case 0:
			minfirst = 0;
			maxlast = 7;
			break;
		case 1:
			minfirst = 1;
			maxlast = 7;
			break;
		case 2:
			minfirst = 2;
			maxlast = 3;
			break;
		case 3:
			minfirst = 3;
			maxlast = 3;
			break;
		case 4:
			minfirst = 4;
			maxlast = 7;
			break;
		case 5:
			minfirst = 5;
			maxlast = 7;
			break;
		case 6:
			minfirst = 6;
			maxlast = 7;
			break;
		case 7:
			minfirst = 7;
			maxlast = 7;
			break;
		default:
			minfirst = 0;
			maxlast = 7;
			break;
	}

	if((planepnt->AI.winglead < 0) && (planepnt->AI.prevpair < 0))
	{
		minfirst = 0;
		maxlast = 7;
	}

	if(workfirst > maxlast)
	{
		*retfirst = 8;
		*retlast = 8;
		return;
	}

	if(workfirst < minfirst)
	{
		workfirst = minfirst;
	}
	if(worklast > maxlast)
	{
		worklast = maxlast;
	}
	*retfirst = workfirst;
	*retlast = worklast;
}

/*----------------------------------------------------------------------------
 *
 *	NetCheckDatabase()
 *
 *		Run through all the planes that the Player could control the loadouts for
 *		and send their loadout info across the net.
 *
 */
void NetCheckDatabase()
{
	int cnt, wlcnt, cnt2;
	int done;
	BYTE weapid, npods_cnt;
	FPoint	fptemp;
	int voiceid;
	PlaneParams *checkplane;

	if(iLoadUpdateStatus[MySlot])
	{
		done = 0;
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if((cnt == MySlot) || (!iLoadUpdateStatus[cnt]))
				continue;

			lLoadUpdateTimer[cnt] -= DeltaTicks;
			if(lLoadUpdateTimer[cnt] < 0)
			{
				lLoadUpdateTimer[cnt] = 10000;
				weapid = pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].WeapId;
				npods_cnt = pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].Count;
				npods_cnt |= (pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].bNumPods << 6);
				if(iLoadUpdateStation[cnt] == 0)
				{
					iMAISendTo = cnt;
					NetPutGenericMessage2Float(pPlaneLoadUpdate[cnt], GM2F_CHK_SHADOW, pDBAircraftList[pPlaneLoadUpdate[cnt]->AI.iPlaneIndex].fShadowLRZOff);
					NetPutGenericMessage2Long(pPlaneLoadUpdate[cnt], GM2L_CHK_R_CROSS_SIG, pDBAircraftList[pPlaneLoadUpdate[cnt]->AI.iPlaneIndex].iRadarCrossSignature);
					fptemp.X = PlayerPlane->AI.fStoresWeight;
					fptemp.Y = PlayerPlane->TotalFuel;
					fptemp.Z = PlayerPlane->AircraftDryWeight;
					NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);

					checkplane = PlayerPlane;
					while(checkplane)
					{
						if((checkplane == PlayerPlane) || ((checkplane->Status & PL_AI_DRIVEN) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
						{
							voiceid = checkplane->AI.lAIVoice / 1000;
							NetPutGenericMessage2(checkplane, GM2_UPDATE_VOICE, voiceid);
						}

						if(checkplane->AI.wingman)
						{
							if((Planes[checkplane->AI.wingman].Status & PL_AI_DRIVEN) && (Planes[checkplane->AI.wingman].AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								voiceid = Planes[checkplane->AI.wingman].AI.lAIVoice / 1000;
								NetPutGenericMessage2(&Planes[checkplane->AI.wingman], GM2_UPDATE_VOICE, voiceid);
							}
						}

						if(checkplane->AI.nextpair < 0)
						{
							checkplane = NULL;
						}
						else
						{
							checkplane = &Planes[checkplane->AI.nextpair];
						}
					}

					iMAISendTo = -1;
				}
				iMAISendTo = cnt;
				NetPutGenericMessage4(pPlaneLoadUpdate[cnt], GM4_WEAP_LOAD, iLoadUpdateStation[cnt], weapid, npods_cnt);
				iMAISendTo = -1;

				NetPutDBInfo(cnt, pPlaneLoadUpdate[cnt] - Planes, iLoadUpdateStation[cnt]);
				while(!done)
				{
					iLoadUpdateStation[cnt] = iLoadUpdateStation[cnt] + 1;
					if(iLoadUpdateStation[cnt] < MAX_F18E_STATIONS)
					{
						if(pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].Count >= 0)
						{
							done = 1;
							for(wlcnt = 0; wlcnt < iLoadUpdateStation[cnt]; wlcnt ++)
							{
								if(pPlaneLoadUpdate[cnt]->WeapLoad[wlcnt].WeapId == pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].WeapId)
								{
									weapid = pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].WeapId;
									npods_cnt = pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].Count;
									npods_cnt |= (pPlaneLoadUpdate[cnt]->WeapLoad[iLoadUpdateStation[cnt]].bNumPods << 6);

									iMAISendTo = cnt;
									NetPutGenericMessage4(pPlaneLoadUpdate[cnt], GM4_WEAP_LOAD, iLoadUpdateStation[cnt], weapid, npods_cnt);
									iMAISendTo = -1;

									done = 0;
								}
							}
						}
					}
					else
					{
						done = 1;

						pPlaneLoadUpdate[cnt] = NULL;
						iLoadUpdateStation[cnt] = 0;
						iLoadUpdateStatus[cnt] = 0;
						lLoadUpdateTimer[cnt] = -1;

						for(cnt2 = 0; cnt2 < MAX_HUMANS; cnt2 ++)
						{
							if((cnt2 != MySlot) && (iLoadUpdateStatus[cnt2]))
								done = 2;
						}
						if(done > 1)
							iLoadUpdateStatus[MySlot] = 0;
					}
				}
			}
		}
	}
	return;
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDBInfo()
 *
 *		Send information to another player so they can see if we are cheating.
 *		Each player should see this message about the other player
 *
 */
//  slotnum is the single human I want to send this info to
void NetPutDBInfo(int slotnum, int planenum, int stationnum)
{
	DPMSG_WEAPON_HACK_CHECK NetHack;
	DBWeaponType *pweapon;

	if(iMultiCast >= 0)
	{
		NetPutDBInfoSmall(slotnum, planenum, stationnum);
		return;
	}

	pweapon = &pDBWeaponList[Planes[planenum].WeapLoad[stationnum].WeapIndex];
	NetHack.acceleration = pweapon->iAcceleration;
	NetHack.burntime = pweapon->iBurnTime;
	NetHack.damagerad = pweapon->iDamageRadius;
	NetHack.damageval = pweapon->iDamageValue;
	NetHack.ecmresist = pweapon->iECMResistance;
	NetHack.maxalt = pweapon->iMaxAlt;
	NetHack.maxrange = pweapon->iRange;
	NetHack.maxspeed = pweapon->fMaxSpeed;
	NetHack.maxyawrate = pweapon->iMaxYawRate;
	NetHack.MinRange = pweapon->fMinRange;
	NetHack.seekerfov = pweapon->iSeekerFOV;
	NetHack.slot = MySlot;
	NetHack.time = newTime;
	NetHack.type = NET_CHECK_DATA_HACK;
	NetHack.weapflags = (pweapon->iSeekerType | (pweapon->iRocketMotor<< 4));
	NetHack.weapid = pweapon->lWeaponID;

	NetSend( Slot[slotnum].dpid, 0, &NetHack, sizeof(NetHack) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDBInfo()
 *
 *		Get database information from another player and compare it with ours.
 *		If different flag the error.  Check will be done in small data size
 *		version of this function.
 */
void NetGetDBInfo(DPMSG_WEAPON_HACK_CHECK *Net)
{
	DPMSG_WEAPON_HACK_CHECK_SM NetSmall;
	int fromslot = Net->slot;

	NetSmall.acceleration = Net->acceleration;
	NetSmall.burntime = Net->burntime;
	NetSmall.damagerad = Net->damagerad;
	NetSmall.damageval = Net->damageval;
	NetSmall.ecmresist = Net->ecmresist;
	NetSmall.maxalt = Net->maxalt;
	NetSmall.maxrange = Net->maxrange;
	NetSmall.maxspeed = Net->maxspeed;
	NetSmall.maxyawrate = Net->maxyawrate;
	NetSmall.MinRange = Net->MinRange;
	NetSmall.seekerfov = Net->seekerfov;
	NetSmall.type = Net->type;
	NetSmall.weapflags = Net->weapflags;
	NetSmall.weapid = Net->weapid;

	NetGetDBInfoSmall(fromslot, &NetSmall);
}

/*----------------------------------------------------------------------------
 *
 *	NetPutDBInfoSmall()
 *
 *		Send information to another player so they can see if we are cheating.
 *		Each player should see this message about the other player
 *
 */
//  slotnum is the single human I want to send this info to
void NetPutDBInfoSmall(int slotnum, int planenum, int stationnum)
{
	DPMSG_WEAPON_HACK_CHECK_SM NetHack;
	DBWeaponType *pweapon;

	pweapon = &pDBWeaponList[Planes[planenum].WeapLoad[stationnum].WeapIndex];
	NetHack.acceleration = pweapon->iAcceleration;
	NetHack.burntime = pweapon->iBurnTime;
	NetHack.damagerad = pweapon->iDamageRadius;
	NetHack.damageval = pweapon->iDamageValue;
	NetHack.ecmresist = pweapon->iECMResistance;
	NetHack.maxalt = pweapon->iMaxAlt;
	NetHack.maxrange = pweapon->iRange;
	NetHack.maxspeed = pweapon->fMaxSpeed;
	NetHack.maxyawrate = pweapon->iMaxYawRate;
	NetHack.MinRange = pweapon->fMinRange;
	NetHack.seekerfov = pweapon->iSeekerFOV;
	NetHack.type = NET_CHECK_DATA_HACK_SM;
	NetHack.weapflags = (pweapon->iSeekerType | (pweapon->iRocketMotor<< 4));
	NetHack.weapid = pweapon->lWeaponID;

	NetAddToBigPacket(slotnum, &NetHack, sizeof(NetHack) );
}

/*----------------------------------------------------------------------------
 *
 *	NetGetDBInfoSmall()
 *
 *		Get database information from another player and compare it with ours.
 *		If different flag the error.
 *		
 */
void NetGetDBInfoSmall(int fromslot, DPMSG_WEAPON_HACK_CHECK_SM *Net)
{
	int isdiff;
	DBWeaponType *pweapon;

	if(iLoadUpdateStation[MySlot] & (1<<fromslot))
	{
		return;
	}

	int weapon_index = AIGetWeaponIndex(Net->weapid);
	if(weapon_index < 0)
	{
		isdiff = 1;
	}
	else
	{
		pweapon = &pDBWeaponList[weapon_index];
		if(Net->acceleration != pweapon->iAcceleration)
			isdiff = 1;
		else if(Net->burntime != pweapon->iBurnTime)
			isdiff = 1;
		else if(Net->damagerad != pweapon->iDamageRadius)
			isdiff = 1;
		else if(Net->damageval != pweapon->iDamageValue)
			isdiff = 1;
		else if(Net->ecmresist != pweapon->iECMResistance)
			isdiff = 1;
		else if(Net->maxalt != pweapon->iMaxAlt)
			isdiff = 1;
		else if(Net->maxrange != pweapon->iRange)
			isdiff = 1;
		else if(Net->maxspeed != pweapon->fMaxSpeed)
			isdiff = 1;
		else if(Net->maxyawrate != pweapon->iMaxYawRate)
			isdiff = 1;
		else if(Net->MinRange != pweapon->fMinRange)
			isdiff = 1;
		else if(Net->seekerfov != pweapon->iSeekerFOV)
			isdiff = 1;
		else if((Net->weapflags & 0xF) != pweapon->iSeekerType)
			isdiff = 1;
		else if(((Net->weapflags & 0x30) >> 4) != pweapon->iRocketMotor)
			isdiff = 1;
		else
			isdiff = 0;
	}

	if(isdiff)
	{
		iLoadUpdateStation[MySlot] |= (1<<fromslot);

		int planenum = NetGetPlaneIndexFromSlot(fromslot);

		AICGiveHackMessage(planenum);
	}
}

/*----------------------------------------------------------------------------
 *
 *	NetGetMultiPlayerName()
 *
 *		Get name for multiplayer plane.  For Jay so he doesn't have a .h conflict
 *		
 */
void NetGetMultiPlayerName(int planenum, char *callsign)
{
	strcpy(callsign, Slot[planenum].name);
	return;
}

/*----------------------------------------------------------------------------
 *
 *	NetCheckStationUpdated()
 *
 *		See if we've updated a station for a Player before sending a weapon
 *		instantiate message.
 *		
 */
void NetCheckStationUpdated(PlaneParams *P, int station)
{
	int cnt;
	BYTE weapid, npods_cnt;

	if(!MultiPlayer)
		return;

	if(!iLoadUpdateStatus[MySlot])
		return;

	if(!P)
		return;

	if(P != PlayerPlane)
		return;

	for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
	{
		if((cnt == MySlot) || (!iLoadUpdateStatus[cnt]))
			continue;

		if(iLoadUpdateStation[cnt] > station)
			continue;

		weapid = P->WeapLoad[station].WeapId;
		npods_cnt = P->WeapLoad[station].Count;
		npods_cnt |= (P->WeapLoad[station].bNumPods << 6);
		NetPutGenericMessage4(P, GM4_WEAP_LOAD, station, weapid, npods_cnt);
		break;
	}
	return;
}

/*----------------------------------------------------------------------------
 *
 *	NetSendPlayerExit()
 *
 *		Send that a Player has left the game.
 *		
 */
void NetSendPlayerExit(PlaneParams *planepnt)
{
	DPMSG_GENERIC_1 NetGen;

	if(planepnt != PlayerPlane)
		return;

	NetGen.type = NET_GENERIC_1;
	NetGen.time   = newTime;
	NetGen.slot	= planepnt - Planes;

	if(g_bIAmHost)
	{
		NetGen.arg1 = GM_HOST_EXITS;
	}
	else
	{
		NetGen.arg1 = GM_PLAYER_EXITS;
	}
	dpSendGuaranteed (  DPID_ALLPLAYERS, DPSEND_GUARANTEED, &NetGen, sizeof(NetGen) );
	return;
}


/*----------------------------------------------------------------------------
 *
 *	NetSendLogDamageOrDestroy()
 *
 *		Make sure vehicle and plane log damage/destroyed are synced.
 *		Ground Objects should be OK since they don't move so we just create the explosions on the fly.
 *		
 */
void NetSendLogDamageOrDestroy(PlaneParams *planepnt, MovingVehicleParams *vehiclepnt, WeaponParams *W, int damage_type, int destroy)
{
	BYTE slot = 0;
	BYTE arg1;
	long arg2, arg3, arg4;
	DBWeaponType *pweapon_type;
	BasicInstance *glauncher;
	PlaneParams *plauncher;
	MovingVehicleParams *vlauncher;

	if(planepnt)
	{
		if(destroy)
		{
			arg1 = GM4L_LOG_PLANE_DESTROY;
		}
		else
		{
			arg1 = GM4L_LOG_PLANE_DAMAGE;
		}
	}
	else if(vehiclepnt)
	{
		slot = vehiclepnt - MovingVehicles;
		if(destroy)
		{
			arg1 = GM4L_LOG_VEH_DESTROY;
		}
		else
		{
			arg1 = GM4L_LOG_VEH_DAMAGE;
		}
	}
	else
		return;

	if(!W)
	{
		pweapon_type = NULL;
	}
	else if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	arg2 = 0;
	arg3 = NO_TARGET;
	arg4 = 0;
	if((!W) || (!W->Launcher))
	{
		arg3 = NO_TARGET;
		arg2 = 0;
	}
	else if(W->LauncherType == AIRCRAFT)
	{
		plauncher = (PlaneParams *)W->Launcher;
		arg2 = plauncher - Planes;
	}
	else if(W->LauncherType == GROUNDOBJECT)
	{
		glauncher = (BasicInstance *)W->Launcher;
		arg2 = glauncher->SerialNumber;
	}
	else
	{
		vlauncher = (MovingVehicleParams *)W->Launcher;
		arg2 = vlauncher - MovingVehicles;
	}

	if((W) && (W->Launcher))
	{
		arg3 = W->LauncherType;
		arg3 |= (pweapon_type->lWeaponID<<8);
		arg4 = W->iNetID;
	}
	arg3 |= (damage_type & DTE_DIRECT_HIT) ? 0x10000 : 0;
	arg3 |= (destroy & 0x2) ? 0x20000 : 0;

	NetPutGenericMessage4Long(planepnt, arg1, arg2, arg3, arg4, slot);

#if 0
	char tempstr[128];

	switch(arg1)
	{
		case GM4L_LOG_PLANE_DESTROY:
			if(W && pweapon_type)
			{
				sprintf(tempstr, "Plane %d destroyed by ID %d %s, DT %d, C %d, NID %d", planepnt - Planes, pweapon_type->lWeaponID, pweapon_type->sName, damage_type, destroy & 0x2, arg4);
			}
			else
			{
				sprintf(tempstr, "Plane %d destroyed C %d", planepnt - Planes, destroy & 0x2);
			}
			break;
		case GM4L_LOG_PLANE_DAMAGE:
			if(W && pweapon_type)
			{
				sprintf(tempstr, "Plane %d damaged by ID %d %s, DT %d, C %d, NID %d", planepnt - Planes, pweapon_type->lWeaponID, pweapon_type->sName, damage_type, destroy & 0x2, arg4);
			}
			else
			{
				sprintf(tempstr, "Plane %d damaged C %d", planepnt - Planes, destroy & 0x2);
			}
			break;
		case GM4L_LOG_VEH_DESTROY:
			if(W && pweapon_type)
			{
				sprintf(tempstr, "Vehicle %d destroyed by ID %d %s, DT %d, C %d, NID %d", slot, pweapon_type->lWeaponID, pweapon_type->sName, damage_type, destroy & 0x2, arg4);
			}
			else
			{
				sprintf(tempstr, "Vehicle %d destroyed C %d", slot, destroy & 0x2);
			}
			break;
		case GM4L_LOG_VEH_DAMAGE:
			if(W && pweapon_type)
			{
				sprintf(tempstr, "Vehicle %d damaged by ID %d %s, DT %d, C %d, NID %d", slot, pweapon_type->lWeaponID, pweapon_type->sName, damage_type, destroy & 0x2, arg4);
			}
			else
			{
				sprintf(tempstr, "Vehicle %d damaged C %d", slot, destroy & 0x2);
			}
			break;
	}

	AICAddAIRadioMsgs(tempstr, 50);
#endif
}
