#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"
#include "SkunkNet.h"

extern BOOL	g_bIAmHost;
extern int GetAirframeCount (RESOURCEFILE *);
extern int GetAircraftIndex (RESOURCEFILE *pRes, int nRequestedIndex);
extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
int IGGetTail(int planenum, RESOURCEFILE  *resFile);

//***********************************************************************************************************************************
//**************************************************************************************
void IGAddMover(int planenum, MBObject oneobject)
{
	int cnt;
	FPoint Eye;
	int pathnum;
	float tdist;
	float dx, dy, dz;
	MBWayPoints *lookway;
	FPoint nextway;
	int vnum=0;
	int parked = 0;
	char tempstr[128];

	int oldStatus = Planes[planenum].Status;
	//Can do this because PlaneParam is a class with the "=" operator overloaded!
	Planes[planenum] = F18_PlaneInit;
	Planes[planenum].Type = IGLoadPlaneType(oneobject.iType);

//	PlaneTypes[cnt].Model = Load3DObject("Plane.3d");  // Will Need eventually
//	GetSortHeader(&Planes[planenum],PLANE_OBJECT);
	Planes[planenum].Status = (oneobject.iFlags & 0xF) | PL_NEED_ATTITUDE;

	if(MultiPlayer)
	{
		Planes[planenum].Status &= ~(PL_DEVICE_DRIVEN|PL_COMM_DRIVEN);
		Planes[planenum].Status |= (PL_AI_DRIVEN);
	}

	if(oldStatus)
	{
		Planes[planenum].Status = oldStatus;
	}
//	Planes[planenum].Type = &PlaneTypes[0];  //  This will need to be updated.

	//  I moved the DataBase stuff into IGAddMover so that I could set Yaw rates and things like that.  SRE
	// DataBase Stuff
	Planes[planenum].AI.lPlaneID = oneobject.iType;
	Planes[planenum].AI.iPlaneIndex = GetPlaneIndex(Planes[planenum].AI.lPlaneID);


	Planes[planenum].WorldPosition.X = ConvertWayLoc(oneobject.X);  //  Only if boarder   + (384.0 * TILE_SIZE);
	Planes[planenum].WorldPosition.Z = ConvertWayLoc(oneobject.Z);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(oneobject.Y > 0)
	{
		Planes[planenum].WorldPosition.Y = ConvertWayLoc(oneobject.Y);
	}
	else
	{
//		Eye.X = Planes[planenum].WorldPosition.X;
//		Eye.Z = Planes[planenum].WorldPosition.Z;
//		LoadTerrain(Eye);
//		Planes[planenum].WorldPosition.Y = LandHeight(Planes[planenum].WorldPosition.X,Planes[planenum].WorldPosition.Z) + ConvertWayLoc(labs(oneobject.Y));

		OffDiskTerrainInfo->m_Location = Planes[planenum].WorldPosition;
		OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
		Planes[planenum].WorldPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(oneobject.Y));

		if(oneobject.Y == 0)
		{
			Planes[planenum].OnGround = 1;
			Planes[planenum].Flaps = 30.0;
			Planes[planenum].Brakes = 1;
			Planes[planenum].WorldPosition.Y += Planes[planenum].Type->GearDownHeight;
			SetLandingGearUpDown(&Planes[planenum],LOWER_LANDING_GEAR);
		}
	}

//	Planes[planenum].WorldPosition.X = 40000;
//	Planes[planenum].WorldPosition.Y = 1000;
// 	Planes[planenum].WorldPosition.Z = 40000;

	Planes[planenum].Orientation.I.SetValues(0.0f,0.0f,-1.0f);

	Planes[planenum].Orientation.J.SetValues(1.0f,0.0f,0.0f);

	Planes[planenum].Orientation.K.SetValues(0.0f,-1.0f,0.0f);

	Planes[planenum].AI.DesiredRoll = 0;
	Planes[planenum].AI.DesiredPitch = 0;
	Planes[planenum].AI.DesiredHeading = 0;

	if(Planes[planenum].Status & AL_AI_DRIVEN)
	{
		Planes[planenum].ElevatorTrim = 1.0;  //  TEMP
		Planes[planenum].DoControlSim = CalcF18ControlSurfacesSimple;
		Planes[planenum].DoCASSim = CalcF18CASUpdatesSimple;
		Planes[planenum].DoForces = CalcAeroForcesSimple;
		Planes[planenum].DoPhysics = CalcAeroDynamicsSimple;

		Planes[planenum].MaxPitchRate = 10.0;
		Planes[planenum].MaxRollRate = 90.0;
//		Planes[planenum].YawRate = 15.0;
		Planes[planenum].YawRate = pDBAircraftList[Planes[planenum].AI.iPlaneIndex].iMaxYawRate;
		if(Planes[planenum].YawRate == 0)
		{
			Planes[planenum].YawRate = 1;
#ifdef _DEBUG
			char errmsg[256];
			sprintf(errmsg, "Bad Yaw Rate for plane %d type %d", planenum, Planes[planenum].AI.lPlaneID);
			MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
		}

//		Planes[planenum].MaxSpeedAccel = 20;	//15
		Planes[planenum].MaxSpeedAccel = pDBAircraftList[Planes[planenum].AI.iPlaneIndex].iAcceleration;
		if(Planes[planenum].MaxSpeedAccel == 0)
		{
			Planes[planenum].MaxSpeedAccel = 1;
#ifdef _DEBUG
			char errmsg[256];
			sprintf(errmsg, "Bad Acceleration for plane %d type %d", planenum, Planes[planenum].AI.lPlaneID);
			MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
		}
		Planes[planenum].SpeedPercentage = 1.0;


#ifdef _DEBUG
		char errmsg[256];

		if(pDBAircraftList[Planes[planenum].AI.iPlaneIndex].iMaxSpeed == 0)
		{
			sprintf(errmsg, "Bad Max Speed for plane %d type %d", planenum, Planes[planenum].AI.lPlaneID);
			MessageBox(hwnd, errmsg, "Error", MB_OK);
			pDBAircraftList[Planes[planenum].AI.iPlaneIndex].iMaxSpeed = 500;
		}
		if(pDBAircraftList[Planes[planenum].AI.iPlaneIndex].iCeiling == 0)
		{
			sprintf(errmsg, "Bad Ceiling for plane %d type %d", planenum, Planes[planenum].AI.lPlaneID);
			MessageBox(hwnd, errmsg, "Error", MB_OK);
			pDBAircraftList[Planes[planenum].AI.iPlaneIndex].iCeiling = 30000;
		}
		if(strlen(pDBAircraftList[Planes[planenum].AI.iPlaneIndex].sObjectName) == 0)
		{
			sprintf(errmsg, "Bad Object Name for plane %d type %d", planenum, Planes[planenum].AI.lPlaneID);
			MessageBox(hwnd, errmsg, "Error", MB_OK);
		}
		if(strlen(pDBAircraftList[Planes[planenum].AI.iPlaneIndex].sShadowName) == 0)
		{
			sprintf(errmsg, "Bad Object Name for plane %d type %d", planenum, Planes[planenum].AI.lPlaneID);
			MessageBox(hwnd, errmsg, "Error", MB_OK);
		}
#endif
	}
	Planes[planenum].AI.LastRoll = 0;
	Planes[planenum].AI.LastPitch = 0;
	Planes[planenum].AI.LastHeading = 0;
	Planes[planenum].AI.AOADegPitch = 0;
	Planes[planenum].AI.lDesiredSpeed = 400;
	Planes[planenum].LeftThrustPercent = Planes[planenum].RightThrustPercent = 40.0;
	Planes[planenum].AI.Behaviorfunc = AIFlyFormation;
	Planes[planenum].AI.OrgBehave = NULL;
	Planes[planenum].AI.CombatBehavior = NULL;

	Planes[planenum].AI.FormationPosition.ZeroLength();
	Planes[planenum].AI.iAIFlags1 = (AIDETAILALTCHECK|AIRADARON);
	Planes[planenum].AI.iAIFlags2 = 0;
	Planes[planenum].AI.lAltCheckDelay = 0;
	Planes[planenum].AI.lRadarDelay = 5000;
	Planes[planenum].AI.dRollAccumulator = 0;
	Planes[planenum].AI.dPitchAccumulator = 0;
	Planes[planenum].AI.fRadarAngleYaw = 0.0f;
	Planes[planenum].AI.fRadarAnglePitch = 0.0f;
	Planes[planenum].AI.lAreaEventFlags = 0;
	Planes[planenum].AI.l2AreaEventFlags = 0;
	Planes[planenum].AI.cFXarray[0] = 86;
	Planes[planenum].AI.cFXarray[1] = 0;
	Planes[planenum].AI.AirTarget = NULL;
	Planes[planenum].AI.AirThreat = NULL;
	Planes[planenum].AI.LastAirThreat = NULL;
	Planes[planenum].AI.LinkedPlane = NULL;
	Planes[planenum].AI.pGroundTarget = NULL;
	Planes[planenum].AI.lGroundTargetFlag = 0;
	Planes[planenum].AI.iMissileThreat = -1;
	Planes[planenum].AI.lHumanTimer = -1;
	Planes[planenum].AI.lLockTimer = -1;
	Planes[planenum].AI.lSpeechOverloadTimer = -1;
	Planes[planenum].AI.lInterceptTimer = -1;
	Planes[planenum].AI.lGunTimer = -1;
	Planes[planenum].AI.lCombatTimer = -1;
	Planes[planenum].AI.lEffectTimer = -1;
	Planes[planenum].AI.pPaintedBy = NULL;
	Planes[planenum].AI.iPaintedByType = -1;
	Planes[planenum].AI.iPaintDist = -1;
	Planes[planenum].AI.cMultiCheckFlags = 0;
	Planes[planenum].AI.cUseWeapon = 0;

#ifdef _DEBUG
	if (AIObjects[planenum].iHomeBaseId>=g_iNumMovingVehicle)
	{
		sprintf(tempstr, "Bad Carrier ID %d for Plane %d", AIObjects[planenum].iHomeBaseId, planenum);
		AICAddAIRadioMsgs(tempstr, 50);
	}
#endif

	if ((AIObjects[planenum].iHomeBaseId >= 0) && (AIObjects[planenum].iHomeBaseId<g_iNumMovingVehicle))
	{
		for(cnt = 0; cnt < AIObjects[planenum].iHomeBaseId; cnt ++)
		{
			vnum += g_pMovingVehicleList[cnt].iNumGroup;
		}
		Planes[planenum].AI.iHomeBaseId = vnum;

		if(MovingVehicles[vnum].iShipType)
		{
			Planes[planenum].AI.iAICombatFlags1 |= AI_HOME_CARRIER;
		}
		else
		{
			Planes[planenum].AI.iAICombatFlags1 |= AI_HOME_AIRPORT;
			Planes[planenum].AI.iHomeBaseId = -1;

#ifdef _DEBUG
			sprintf(tempstr, "NOT SHIP Carrier ID %d VN %d for Plane %d", AIObjects[planenum].iHomeBaseId, vnum, planenum);
			AICAddAIRadioMsgs(tempstr, 50);
#endif
		}
		vnum = 0;
	}
	else
	{
		Planes[planenum].AI.iAICombatFlags1 |= AI_HOME_AIRPORT;
		Planes[planenum].AI.iHomeBaseId = -1;
	}

#if 0 // New Stuff Added
	DBShipType *pshiptype;

		for(cnt = 0; cnt < g_iNumMovingVehicle; cnt ++)
		{
			if(AIObjects[planenum].iHomeBaseId == g_pMovingVehicleList[cnt].iID)
			{
				if(MovingVehicles[cnt].iShipType)
				{
				 	pshiptype = &pDBShipList[MovingVehicles[cnt].iVDBIndex];
					if(pshiptype->lShipType & SHIP_TYPE_CARRIER)
					{
						break;
					}
				}
			}
			vnum += g_pMovingVehicleList[cnt].iNumGroup;
		}

		vnum = 0;
		if(cnt == g_iNumMovingVehicle)  //  OK, we've hit a bug!  Flag and take best guess
		{
#ifdef _DEBUG
			sprintf(tempstr, "Bad Carrier ID %d for Plane %d", AIObjects[planenum].iHomeBaseId, planenum);
			AICAddAIRadioMsgs(tempstr, 50);
#endif
			for(cnt = 0; cnt < g_iNumMovingVehicle; cnt ++)
			{
				if(AIObjects[planenum].iSide == MovingVehicles[cnt].iSide)
				{
					if(MovingVehicles[cnt].iShipType)
					{
				 		pshiptype = &pDBShipList[MovingVehicles[cnt].iVDBIndex];
						if(pshiptype->lShipType == SHIP_TYPE_CARRIER)
						{
							break;
						}
					}
				}
				vnum += g_pMovingVehicleList[cnt].iNumGroup;
			}
		}
		Planes[planenum].AI.iHomeBaseId = vnum;
	}
	else
	{
		Planes[planenum].AI.iAICombatFlags1 |= AI_HOME_AIRPORT;
		Planes[planenum].AI.iHomeBaseId = -1;
	}
#endif

	Planes[planenum].AI.iAICombatFlags2 = 0;

	for(cnt = 0; cnt < 4; cnt ++)
	{
		Planes[planenum].AI.lMinAltFt[cnt] = 0;
//		Planes[planenum].AI.dwAltTimer[cnt] = 0;
		Planes[planenum].AI.fDistToHeight[cnt] = 0.0;
	}



	Planes[planenum].DesiredSpeed = Planes[planenum].BfLinVel.X = 700.0f;

//	Planes[planenum].iNumInGroup = oneobject.numingroup;
	Planes[planenum].AI.WayPosition.SetValues(-1.0f);

#ifdef MISSIONID1VALID
	if(oneobject.iNumWP)
	{
		Planes[planenum].AI.CurrWay = &AIWayPoints[oneobject.iStartWP];
		Planes[planenum].AI.numwaypts = oneobject.iNumWP;
		Planes[planenum].AI.startwpts = oneobject.iStartWP;
	}
	else
	{
		Planes[planenum].AI.CurrWay = NULL;
		Planes[planenum].AI.numwaypts = 0;
		Planes[planenum].AI.startwpts = 0;
	}
#else
	pathnum = oneobject.iWPPathNum;
	if((pathnum >= 0) && (pathnum < iNumWPPaths))
	{
		Planes[planenum].AI.CurrWay = &AIWayPoints[AIWPPaths[pathnum].iStartWP];
		Planes[planenum].AI.numwaypts = AIWPPaths[pathnum].iNumWP;
		Planes[planenum].AI.startwpts = AIWPPaths[pathnum].iStartWP;
	}
	else
	{
		Planes[planenum].AI.CurrWay = NULL;
		Planes[planenum].AI.numwaypts = 0;
		Planes[planenum].AI.startwpts = 0;
	}
#endif
	if(Planes[planenum].AI.CurrWay != NULL)
	{
		Planes[planenum].WorldPosition.X = ConvertWayLoc(Planes[planenum].AI.CurrWay ->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
		Planes[planenum].WorldPosition.Z = ConvertWayLoc(Planes[planenum].AI.CurrWay ->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
		if(oneobject.Y > 0)
		{
			Planes[planenum].WorldPosition.Y = ConvertWayLoc(Planes[planenum].AI.CurrWay ->lWPy);
		}
		else
		{
//			Eye.X = Planes[planenum].WorldPosition.X;
//			Eye.Z = Planes[planenum].WorldPosition.Z;
//			LoadTerrain(Eye);
//			Planes[planenum].WorldPosition.Y = LandHeight(Planes[planenum].WorldPosition.X,Planes[planenum].WorldPosition.Z) + ConvertWayLoc(labs(Planes[planenum].AI.CurrWay ->lWPy));

			OffDiskTerrainInfo->m_Location = Planes[planenum].WorldPosition;
			OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
			Planes[planenum].WorldPosition.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(Planes[planenum].AI.CurrWay ->lWPy));

			if(oneobject.Y == 0)
			{
				Planes[planenum].OnGround = 1;
				Planes[planenum].Flaps = 30.0;
				Planes[planenum].Brakes = 1;
				Planes[planenum].WorldPosition.Y += Planes[planenum].Type->GearDownHeight;
				SetLandingGearUpDown(&Planes[planenum],LOWER_LANDING_GEAR);
			}
		}

		if((Planes[planenum].AI.numwaypts == 2) && (!Planes[planenum].AI.CurrWay->iSpeed) && (!Planes[planenum].AI.CurrWay->lWPy))
		{
			lookway = Planes[planenum].AI.CurrWay + 1;
			if((!lookway->iSpeed) && (!lookway->lWPy))
			{
				parked = 1;
			}
		}

		if(parked || ((Planes[planenum].AI.numwaypts > 1) && ((MultiPlayer) || (Planes[planenum].OnGround == 0) || (GetRegValueL("rrtraining") == 1))))
		{
			lookway = Planes[planenum].AI.CurrWay + 1;
			nextway.X = ConvertWayLoc(lookway->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
			nextway.Z = ConvertWayLoc(lookway->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
			nextway.Y = 0;
			if(lookway->lWPy > 0)
			{
				nextway.Y = ConvertWayLoc(lookway->lWPy);
			}
			else
			{
				OffDiskTerrainInfo->m_Location = nextway;
				OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);
				nextway.Y = OffDiskTerrainInfo->m_Location.Y + ConvertWayLoc(labs(lookway->lWPy)) + (7.0 * FOOT_MULTER);
			}
			AISetOrientation(&Planes[planenum], AIComputeHeadingToPoint(&Planes[planenum], nextway, &tdist, &dx ,&dy, &dz, 1));
		}
	}

	Planes[planenum].AI.WayPosition = Planes[planenum].WorldPosition;

	Planes[planenum].AI.iSide = oneobject.iSide;
	Planes[planenum].AI.lCallSign = oneobject.lCallSign;
	Planes[planenum].AI.cCallSignNum = oneobject.lSubCallSignIndex + 1;
//	Planes[planenum].AI.iFuelLoad = oneobject.iFuelLoad;
#if 0
	for(cnt = 0; cnt < NUMORD; cnt ++)
	{
		if(oneobject.Weapons[cnt].iWeaponCnt)
		{
#if MBWDATFILEEXISTS
			Planes[planenum].AI.Weapons[cnt].iWeaponID = IGLoadOrdinance(oneobject.Weapons[cnt].iWeaponID);
#else
			Planes[planenum].AI.Weapons[cnt].iWeaponID = -1;
#endif
			Planes[planenum].AI.Weapons[cnt].iWeaponCnt = oneobject.Weapons[cnt].iWeaponCnt;
		}
		else
		{
			Planes[planenum].AI.Weapons[cnt].iWeaponID = -1;
			Planes[planenum].AI.Weapons[cnt].iWeaponCnt = 0;
		}
	}
#endif

	for(cnt = 0; cnt < AI_MAX_DAMAGE_SLOTS; cnt ++)
	{
		Planes[planenum].AI.cArmorDamage[cnt] = (char)125;  //  25 point of armor per item * 5
		Planes[planenum].AI.cInteriorDamage[cnt] = 0;
	}

	Planes[planenum].AI.lAIVoice = AICRGetAIVoice(&Planes[planenum]);

	Planes[planenum].AircraftDryWeight = 34000;
	Planes[planenum].InternalFuel      = 20000;
	Planes[planenum].CenterDropFuel    = 160;
	Planes[planenum].WingDropFuel      = 80;

	Planes[planenum].TotalWeight = 	Planes[planenum].AircraftDryWeight +
					 	Planes[planenum].InternalFuel +
					 	Planes[planenum].CenterDropFuel +
					 	Planes[planenum].WingDropFuel;

	Planes[planenum].TotalMass = Planes[planenum].TotalWeight/32.0;

	Planes[planenum].TotalFuel = 20000;
}

//***********************************************************************************************************************************

//**************************************************************************************
void LoadWingmenWeaponInfo()
{
	RESOURCEFILE  resFile;
	HANDLE  hfile;
	int cnt;
	PlaneParams *leadplane;
	PlaneParams *playerlead;
	PlaneParams *planepnt;
	PlaneParams *walker;
	int placeingroup, checkplace;
	float weight;
	int weapid;


	planepnt = PlayerPlane;

#if 0  //  I think this was from original F-15.  No longer valid
	int cnt2;

	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_PLAYERS; cnt ++)
		{
			if(PlayerPlane != &Planes[cnt])
			{
				for(cnt2 = 0; cnt2 < 40; cnt2 ++)
				{
					cPlayerArmor[cnt][cnt2] = (char)125;  //  25 total armor points per item * 5;
					cPlayerDamage[cnt][cnt2] = 0;
				}
				for(cnt2 = 0; cnt2 < MAX_F18E_STATIONS; cnt2 ++)
				{
					Planes[cnt].WeapLoad[cnt2] = PlayerPlane->WeapLoad[cnt2];
				}
			}
		}
	}
#endif

#ifndef __DEMO__
//	if(planepnt->Status & PL_AI_DRIVEN)
//	{
//		return;
//	}
#endif

	hfile = LoadResourceFile (g_szResourceFile, &resFile);

	// Establish aircraft tailfin art
	walker = planepnt;
	int	nWingmen = GetAirframeCount (&resFile);
	for (cnt = 0, walker = planepnt; cnt < nWingmen; cnt++, walker++)
	{
		int nIndex = GetAircraftIndex (&resFile, cnt);

		// mark off our tail identification for rendering
		walker->TailYear = resFile.ActiveAircraft[nIndex].AirFrame.sYear;
		walker->TailSerial = resFile.ActiveAircraft[nIndex].AirFrame.sSerial;
		walker->TailSquadron = resFile.ActiveAircraft[nIndex].nSquadron;  //  3
		walker->TailBase = 3;  //  3;
	}

	for(cnt = 0, walker = &Planes[0]; cnt < iNumAIObjs; cnt ++, walker ++)
	{
		walker->TailSquadron = IGGetTail(cnt, &resFile);
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);
	planepnt->AI.lAIVoice = (((resFile.ActiveAircraft[placeingroup].FlightCrew.nVoice) + 1) * 1000);

//	planepnt ++;

	//  For multiplayer have to make sure to handle first plane in group
	playerlead = AIGetLeader(PlayerPlane);
	planepnt = playerlead;

	leadplane = AIGetLeader(planepnt);

	checkplace = 0;
	while(leadplane == playerlead)
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);
		if(placeingroup != checkplace)
		{
			break;
		}
		checkplace ++;

		if(planepnt == PlayerPlane)
		{
			planepnt ++;
			leadplane = AIGetLeader(planepnt);
			continue;
		}

		planepnt->AI.iAICombatFlags1 |= AI_WINCHESTER_AA;
		planepnt->AI.iAICombatFlags1 |= AI_WINCHESTER_AG;

		if(resFile.ActiveAircraft[placeingroup].AirFrame.nID != EMPTY_SLOT)
		{
			planepnt->InternalFuel = resFile.ActiveAircraft[placeingroup].Loadout.nInternalFuelWeight;
			if((planepnt->InternalFuel <= 0) || (g_nMissionType == SIM_QUICK))
			{
				planepnt->InternalFuel = WEIGHT_MAX_INTERNALFUEL;
			}

			planepnt->CenterDropFuel = 0;
			planepnt->WingDropFuel = 0;
			for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
			{
				planepnt->WeapLoad[cnt].WeapId = resFile.ActiveAircraft[placeingroup].Loadout.Station[cnt].iWeaponId;
				planepnt->WeapLoad[cnt].Count = resFile.ActiveAircraft[placeingroup].Loadout.Station[cnt].iWeaponCnt;
				planepnt->WeapLoad[cnt].WeapIndex=RegisterWeapon(planepnt->WeapLoad[cnt].WeapId);
				if(cnt == CHAFF_FLARE_STATION)
				{
					if(planepnt->WeapLoad[CHAFF_FLARE_STATION].Count > 0)
					{
						if(planepnt->WeapLoad[CHAFF_FLARE_STATION].WeapId == 7)
						{
							iAIChaffTotal[placeingroup - 1] = 60;
							iAIFlareTotal[placeingroup - 1] = 90;
						}
						else if(planepnt->WeapLoad[CHAFF_FLARE_STATION].WeapId == 1)
						{
							iAIChaffTotal[placeingroup - 1] = 120;
							iAIFlareTotal[placeingroup - 1] = 60;
						}
						else
						{
							iAIChaffTotal[placeingroup - 1] = 180;
							iAIFlareTotal[placeingroup - 1] = 30;
						}
					}
					else
					{
						iAIChaffTotal[placeingroup - 1] = 0;
						iAIFlareTotal[placeingroup - 1] = 0;
					}
				}

				if(cnt == GUNS_STATION)
				{
					if(planepnt->WeapLoad[GUNS_STATION].Count > 0)
					   planepnt->WeapLoad[GUNS_STATION].Count *= 500;
				}

				if(planepnt->WeapLoad[cnt].Count)
				{
					int Type = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
					switch(Type)
					{
						case DUMB_AG:
						case GUIDED_AG:
						case AG_MISSILE:
						case CLUSTER:
						case WEAPON_TYPE_AA_ANTIRADAR_MISSILE:
						case WEAPON_TYPE_AGROCKET:
						case WEAPON_TYPE_ANTI_SHIP_MISSILE:
						case WEAPON_TYPE_FUEL_AIR_EXPLOSIVE:
						case WEAPON_TYPE_NAPALM:
							planepnt->AI.iAICombatFlags1 &= ~(AI_WINCHESTER_AG);
							break;
						case SHORT_RANGE:
						case MED_RANGE:
							planepnt->AI.iAICombatFlags1 &= ~(AI_WINCHESTER_AA);
							break;
						case WEAPON_TYPE_FUEL_TANK:
							planepnt->AI.iAICombatFlags1 |= (AI_FUEL_TANKS);
							if (pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 142)	// 330 gallon tank
								weight = planepnt->WeapLoad[cnt].Count * WEIGHT_330_GALLONS_FUEL;
							else // 480 gallon tank
								weight = planepnt->WeapLoad[cnt].Count * WEIGHT_480_GALLONS_FUEL;

							if(cnt == 8)
								planepnt->CenterDropFuel += weight;
							else
								planepnt->WingDropFuel += weight;
							break;
					}
				}
			}

#ifdef __DEMO__
	  		planepnt->AI.lAIVoice = SPCH_WNG1;
#else
			planepnt->AI.lAIVoice = (((resFile.ActiveAircraft[placeingroup].FlightCrew.nVoice) + 1) * 1000);
#endif
		}

		planepnt ++;
		leadplane = AIGetLeader(planepnt);
	}
	CloseResourceFile (hfile);

	for(planepnt = Planes; planepnt <= LastPlane; planepnt++)
	{
		if(planepnt != PlayerPlane)
		{
			planepnt->AircraftDryWeight = (pDBAircraftList[planepnt->AI.iPlaneIndex].iWeight * 2) / 3;

			planepnt->TotalWeight = 	planepnt->AircraftDryWeight +
					 			planepnt->InternalFuel +
					 			planepnt->CenterDropFuel +
					 			planepnt->WingDropFuel;

			planepnt->TotalMass = planepnt->TotalWeight/32.0;
		}

		if(planepnt->AI.lPlaneID == 61)
		{
			planepnt->AI.iAICombatFlags2 |= AI_NOISE_JAMMER;
			if(planepnt->WeapLoad[MAX_F18E_STATIONS - 1].Count <= 0)
			{
				planepnt->WeapLoad[MAX_F18E_STATIONS - 1].Count = -3;
			}

			planepnt->AI.iAICombatFlags2 |= AI_GATE_STEALER;
		}

		planepnt->AI.fStoresWeight = 0;

		if(!(planepnt->AI.iAICombatFlags1 & AI_WINCHESTER_AA))
		{
			PlaneHasAAWeapons(planepnt);
		}

		if (planepnt->WeapLoad[LEFT2_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags1 |= AI_S2_RACK;
		}
		else
		{
			weapid = pDBWeaponList[planepnt->WeapLoad[LEFT2_STATION].WeapIndex].lWeaponID;

			if((weapid == 24) || (weapid == 133) || (weapid == 36) || (weapid == 37) || (weapid == 109))
			{
				planepnt->AI.iAICombatFlags1 |= AI_S2_RACK;
			}
			else if((weapid == 87) || (weapid == 38) || (weapid == 34) || (weapid == 35) || (weapid == 108))
			{
				planepnt->AI.iAICombatFlags1 |= AI_S2_RACK;
			}
		}

		if (planepnt->WeapLoad[LEFT3_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags1 |= AI_S3_RACK;
		}
		if (planepnt->WeapLoad[LEFT4_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags1 |= AI_S4_RACK;
		}
		if(planepnt->WeapLoad[CENTER6_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags2 |= AI_C6_RACK;
		}
		if (planepnt->WeapLoad[RIGHT8_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags1 |= AI_S8_RACK;
		}
		if (planepnt->WeapLoad[RIGHT9_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags1 |= AI_S9_RACK;
		}
		if (planepnt->WeapLoad[RIGHT10_STATION].Count > 1)
		{
			planepnt->AI.iAICombatFlags1 |= AI_S10_RACK;
		}
		else
		{
			weapid = pDBWeaponList[planepnt->WeapLoad[RIGHT10_STATION].WeapIndex].lWeaponID;

			if((weapid == 24) || (weapid == 133) || (weapid == 36) || (weapid == 37) || (weapid == 109))
			{
				planepnt->AI.iAICombatFlags1 |= AI_S10_RACK;
			}
			else if((weapid == 87) || (weapid == 38) || (weapid == 34) || (weapid == 35) || (weapid == 108))
			{
				planepnt->AI.iAICombatFlags1 |= AI_S10_RACK;
			}
		}


		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			if(planepnt->WeapLoad[cnt].Count >= 30)
			{
				planepnt->AI.fStoresWeight += pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeight;
				planepnt->WeapLoad[cnt].bNumPods = 1;
			}
			else if(planepnt->WeapLoad[cnt].Count > 0)
			{
				planepnt->AI.fStoresWeight += pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeight * planepnt->WeapLoad[cnt].Count;
				planepnt->WeapLoad[cnt].bNumPods = planepnt->WeapLoad[cnt].Count;
			}

			if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 133)
			{
				planepnt->WeapLoad[cnt].Count *= 4;
			}
			else if((pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 134) || (pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 176))
			{
				planepnt->WeapLoad[cnt].Count *= 19;
			}
			else if((pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 102) || (pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 103) || (pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 177))
			{
				planepnt->AI.iAICombatFlags2 |= AI_NOISE_JAMMER;
			}
			else if(pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID == 178)
			{
				planepnt->AI.iAICombatFlags2 |= AI_GATE_STEALER;
			}
		}
	}
}

//**************************************************************************************
void IGGetPlanes(int numobjs)
{
	int cnt;
	int cnt2;
	int numingroup;
	MBObject oneobject;
	int firstingroup;
	int checkescort = -1;
	int addnum = numobjs;
	float ftempval;
	int intrand;
	int maxenemyskill = (GP_NOVICE - g_Settings.gp.nEnemyAircraft) + 1;
	int playerplane;
	PlaneParams *planepnt;
	int orgStatus;
	int makeAI;
	int iorgCombatFlags1;
	int fixingroup;
	int humanfound = 0;

	if(addnum > MAX_PLANES)
	{
		addnum = MAX_PLANES;
#ifdef _DEBUG
		char errmsg[256];
		sprintf(errmsg, "Max Number of Planes %d exceeded %d", addnum, numobjs);
	    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
	}

	if(MultiPlayer)
	{
		NetGetMissionHumanAssignments();
	}

	for(cnt = 0; cnt < addnum; cnt ++)
	{
		fixingroup = 0;
		oneobject = AIObjects[cnt];

		// We need to setup the loadout stuff also
		// The planes structure/class should have a 10 item array
		// of a type that has three items (both shorts,possible chars).
		// These items will be the WeaponID,WeaponIndex and count.
		// The WeaponId is the unique id for that weapon
		// The WeaponIndex is the index into the Weapon Data List
		// The count is the number of weapons of WeaponID type that
		// this aircraft currently has.

		IGAddMover(cnt, oneobject);
		numingroup = oneobject.iNumInFlight - 1;
		firstingroup = cnt;
		Planes[cnt].AI.winglead = -1;
		Planes[cnt].AI.wingman = -1;
		Planes[cnt].AI.prevpair = -1;
		Planes[cnt].AI.nextpair = -1;

		//  I moved the DataBase stuff into IGAddMover so that I could set Yaw rates and things like that.  SRE
		// DataBase Stuff
//		Planes[cnt].AI.lPlaneID = AIObjects[cnt].iType;
//		Planes[cnt].AI.iPlaneIndex = GetPlaneIndex(Planes[cnt].AI.lPlaneID);


//		if(AIObjects[cnt].flag1 & AL_DEVICE_DRIVEN)
		playerplane = 0;
		if(((cnt == 0) || (Planes[cnt].Status & AL_DEVICE_DRIVEN)) && (!humanfound))
		{
			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_FRONT;
			Camera1.AttachedObject = (int *)&Planes[cnt];
			Camera1.DistanceFromObject = 300.0f*FOOT_MULTER;
			UFC.iCurrLKAhead = Planes[cnt].AI.numwaypts;
			UFC.pWPLKAhead = Planes[cnt].AI.CurrWay;
			UFC.pWPPlane = &Planes[cnt];

			RESOURCEFILE	resFile;
			HANDLE hfile = LoadResourceFile (g_szResourceFile, &resFile);
			Planes[cnt].Stats->Weight = resFile.ActiveAircraft[0].nWeight;
			CloseResourceFile (hfile);
			if(Planes[cnt].Status & AL_DEVICE_DRIVEN)
			{
				humanfound = 1;
				playerplane = 1;
				fixingroup = 1;
				checkescort = cnt;
				PlayerPlane = (PlaneParams *)Camera1.AttachedObject;  //  If this variable is needed, make sure it is after InitWaypoints
				if(!(JoyCap1.wCaps & JOYCAPS_HASZ))
				{
					ForceKeyboardThrust(60.0f);
				}
			}
#ifdef __DEMO__																			 
			if(Planes[cnt].Status & AL_DEVICE_DRIVEN)
			{
				AutoPilotOnOff();
				StartCasualAutoPilot();
			}
#endif
			if(GetRegValueL("fore3") == 1)
			{
				AutoPilotOnOff();
				StartCasualAutoPilot();
			}
		}
		else if((Planes[cnt].Status & AL_DEVICE_DRIVEN) && (humanfound))
		{
			Planes[cnt].Status &= ~(PL_COMM_DRIVEN|PL_DEVICE_DRIVEN);
			Planes[cnt].Status |= PL_AI_DRIVEN;
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
			Planes[cnt].dt = 0;
			// This will need to be updated once wingmen have assigned voices.  SRE.
			Planes[cnt].AI.lAIVoice = ((cnt2 + 2) * 1000);
		}

		if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
		{
			Planes[cnt].AI.iSkill = AIObjects[cnt].lDifficulty & 3;
		}
		else if(MultiPlayer)
		{
			Planes[cnt].AI.iSkill = g_Settings.gp.nEnemyAircraft;
		}
		else
		{
			if(Planes[cnt].AI.iSide == AI_ENEMY)
			{
				ftempval = frand() * (float)maxenemyskill;
				intrand = ftempval;
				Planes[cnt].AI.iSkill = GP_NOVICE - intrand;
				if(GetRegValueL("rskill") == 1)
				{
					Planes[cnt].AI.iSkill = g_Settings.gp.nEnemyAircraft;
				}
				if(Planes[cnt].AI.iSkill < 0)
				{
					Planes[cnt].AI.iSkill = 0;
				}
			}
			else
			{
				Planes[cnt].AI.iSkill = 0;
			}
		}

		if(Planes[cnt].Status & AL_AI_DRIVEN)
		{
			Planes[cnt].dt = 0;
			if(MultiPlayer && (g_bIAmHost))
			{
				Planes[cnt].AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
			}
		}

		Planes[cnt].MaxSpeedAccel = pDBAircraftList[Planes[cnt].AI.iPlaneIndex].iAcceleration;
		Planes[cnt].SpeedPercentage = 1.0;

		for(cnt2 = 0; cnt2 < numingroup; cnt2 ++)
		{
			cnt ++;
			oneobject=AIObjects[cnt];

			//  Might want to run it throught IGAddMover for things like damage or other plane unique things
			orgStatus = Planes[cnt].Status;
			makeAI = 0;
			
			if(Planes[firstingroup].Status & (PL_COMM_DRIVEN|PL_DEVICE_DRIVEN))
			{
				if(!(orgStatus & (PL_COMM_DRIVEN|PL_DEVICE_DRIVEN)))
				{
					makeAI = 1;
				}
				else if(!MultiPlayer)
				{
					makeAI = 1;
					Planes[cnt].Status &= ~(PL_COMM_DRIVEN|PL_DEVICE_DRIVEN);
					Planes[cnt].Status |= PL_AI_DRIVEN;
					orgStatus = Planes[cnt].Status;
				}
			}

			iorgCombatFlags1 = Planes[cnt].AI.iAICombatFlags1;

			Planes[cnt] = Planes[firstingroup];

			if(iorgCombatFlags1 & AI_MULTI_ACTIVE)
			{
				Planes[cnt].AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
			}
			else
			{
				Planes[cnt].AI.iAICombatFlags1 &= ~(AI_MULTI_ACTIVE);
			}

			if(orgStatus & (PL_DEVICE_DRIVEN))
			{
				Planes[cnt].Status &= ~(PL_COMM_DRIVEN|PL_AI_DRIVEN);
				Planes[cnt].Status |= PL_DEVICE_DRIVEN;

				Camera1.CameraMode = CAMERA_COCKPIT;
				Camera1.SubType = COCKPIT_FRONT;
				Camera1.AttachedObject = (int *)&Planes[cnt];
				Camera1.DistanceFromObject = 300.0f*FOOT_MULTER;
				UFC.iCurrLKAhead = Planes[cnt].AI.numwaypts;
				UFC.pWPLKAhead = Planes[cnt].AI.CurrWay;
				UFC.pWPPlane = &Planes[cnt];

				RESOURCEFILE	resFile;
				HANDLE hfile = LoadResourceFile (g_szResourceFile, &resFile);
				Planes[cnt].Stats->Weight = resFile.ActiveAircraft[0].nWeight;
				CloseResourceFile (hfile);
				if(Planes[cnt].Status & AL_DEVICE_DRIVEN)
				{
					playerplane = 1;
					fixingroup = 1;
					checkescort = cnt;
					PlayerPlane = (PlaneParams *)Camera1.AttachedObject;  //  If this variable is needed, make sure it is after InitWaypoints
					if(!(JoyCap1.wCaps & JOYCAPS_HASZ))
					{
						ForceKeyboardThrust(60.0f);
					}
				}
#ifdef __DEMO__
				if(Planes[cnt].Status & AL_DEVICE_DRIVEN)
				{
					AutoPilotOnOff();
					StartCasualAutoPilot();
				}
#endif
				if(GetRegValueL("fore3") == 1)
				{
					AutoPilotOnOff();
					StartCasualAutoPilot();
				}
			}
			else if(orgStatus & (PL_COMM_DRIVEN))
			{
				Planes[cnt].Status &= ~(PL_AI_DRIVEN|PL_DEVICE_DRIVEN);
				Planes[cnt].Status |= PL_COMM_DRIVEN;
			}


//			if (!Planes[cnt].SortHeader)
//				GetSortHeader(&Planes[cnt],PLANE_OBJECT);

			Planes[cnt].AI.winglead = -1;
			Planes[cnt].AI.wingman = -1;
			Planes[cnt].AI.prevpair = -1;
			Planes[cnt].AI.nextpair = -1;

			Planes[cnt].AI.iAIFlags1 |= (cnt2 + 1);
			Planes[cnt].AI.lHumanTimer = cnt2 * 7500;

			Planes[cnt].AI.lAIVoice = AICRGetAIVoice(&Planes[cnt]);

//			Planes[cnt].WorldPosition.X += ConvertWayLoc(75 * cnt);  //  TEMP UNTIL FORMATIONS DONE
//			Planes[cnt].WorldPosition.Z -= ConvertWayLoc(75 * cnt);  //  TEMP UNTIL FORMATIONS DONE
			Planes[cnt].AI.lRadarDelay += (cnt2 + 1) * 1000;


			if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
			{
				Planes[cnt].AI.iSkill = AIObjects[cnt].lDifficulty & 3;
			}
			else
			{
				if(Planes[cnt].AI.iSide == AI_ENEMY)
				{
					ftempval = frand() * (float)maxenemyskill;
					intrand = ftempval;
					Planes[cnt].AI.iSkill = GP_NOVICE - intrand;
					if(GetRegValueL("rskill") == 1)
					{
						Planes[cnt].AI.iSkill = g_Settings.gp.nEnemyAircraft;
					}
					if(Planes[cnt].AI.iSkill < 0)
					{
						Planes[cnt].AI.iSkill = 0;
					}
				}
				else
				{
					Planes[cnt].AI.iSkill = 0;
				}
			}

			if(((Planes[cnt].Status & AL_DEVICE_DRIVEN) || (playerplane) || (makeAI)) && (!(orgStatus & (PL_DEVICE_DRIVEN|PL_COMM_DRIVEN))))
			{
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
				Planes[cnt].AI.iAIFlags1 |= AIPLAYERGROUP;
				Planes[cnt].dt = 0;
				// This will need to be updated once wingmen have assigned voices.  SRE.
				Planes[cnt].AI.lAIVoice = ((cnt2 + 2) * 1000);
			}

			if(cnt2 & 1)
			{
				Planes[cnt - 2].AI.nextpair = cnt;
				Planes[cnt].AI.prevpair = cnt - 2;
			}
			else
			{
				Planes[cnt - 1].AI.wingman = cnt;
				Planes[cnt].AI.winglead = cnt - 1;
			}
		}
		if((fixingroup) && MultiPlayer)
		{
			for(cnt2 = 0; cnt2 <= numingroup; cnt2 ++)
			{
				if(Planes[firstingroup + cnt2].Status & (PL_COMM_DRIVEN|PL_DEVICE_DRIVEN))
				{
					Planes[firstingroup + cnt2].AI.iAIFlags1 |= AIPLAYERGROUP;
				}
			}
		}

		AIInitFormation(&Planes[firstingroup], 2, -1);
	 	LastPlane = &Planes[cnt];
	}

	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if(iSlotToPlane[cnt] >= 0)
			{
				planepnt = &Planes[iSlotToPlane[cnt]];

				if(planepnt->Status & PL_DEVICE_DRIVEN)
				{
					if(planepnt->AI.iSide == AI_NEUTRAL)
					{
						iMultiSides = (MS_AGGRESIVE_NEUTRALS|MS_IGNORE_FENCES);
					}

					planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
					while(planepnt)
					{
						if(planepnt->AI.wingman >= 0)
						{
							if(Planes[planepnt->AI.wingman].Status & PL_AI_DRIVEN)
							{
								Planes[planepnt->AI.wingman].AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
							}
						}
						if(planepnt->AI.nextpair >= 0)
						{
							planepnt = &Planes[planepnt->AI.nextpair];
							if(planepnt->Status & PL_AI_DRIVEN)
							{
								planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
							}
							else
							{
								break;
							}
						}
						else
						{
							planepnt = NULL;
						}
					}
				}
				else if(planepnt->Status & PL_COMM_DRIVEN)
				{
					if(planepnt->AI.iSide == AI_NEUTRAL)
					{
						iMultiSides = (MS_AGGRESIVE_NEUTRALS|MS_IGNORE_FENCES);
					}

					planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;

					while(planepnt)
					{
						if(planepnt->AI.wingman >= 0)
						{
							if(Planes[planepnt->AI.wingman].Status & PL_AI_DRIVEN)
							{
								Planes[planepnt->AI.wingman].AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;
							}
						}
						if(planepnt->AI.nextpair >= 0)
						{
							planepnt = &Planes[planepnt->AI.nextpair];
							if(planepnt->Status & PL_AI_DRIVEN)
							{
								planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;
							}
							else
							{
								break;
							}
						}
						else
						{
							planepnt = NULL;
						}
					}
				}
			}
		}
	}

	if(checkescort != -1)
	{
		AICheckIfPlayerDoingEscort(checkescort);
	}

	// For WeaponPlanes, must also update EvaluateAircraft() in initdb.cpp
	IGLoadPlaneType(69);  //  AGM-84E SLAM
	IGLoadPlaneType(70);  //  AGM-84H SLAM ER+
	IGLoadPlaneType(71);  //  AS-4 Kitchen
	IGLoadPlaneType(72);  //  AS-16 Kickback
	IGLoadPlaneType(73);  //  Moskit
	IGLoadPlaneType(74);  //  AGM-84A Harpoon
	IGLoadPlaneType(75);  //  SS-N-19 Shipwreck
	IGLoadPlaneType(76);  //  SS-N-12 Sandbox
	IGLoadPlaneType(77);  //  BGM-109B Tamahawk
	IGLoadPlaneType(78);  //  RGM-84A Harpoon (ship)
//	IGLoadPlaneType(79);  //  Tomahawk
	IGLoadPlaneType(84);  //  SS-N-22 Sunburn
	IGLoadPlaneType(85);  //  TALD
	IGLoadPlaneType(88);  //  SS-N-2A Styx
}

#if 0
//**************************************************************************************
int InitWaypointsMID1(int MissionFile)
{
	int numobjs;
	int numwaypts;
	int numactions;
	long ltempvar;

	iNumWPPaths = 0;

	_read(MissionFile,&lMissionCheckSum,sizeof(lMissionCheckSum));

	_read(MissionFile,&lMBWorldID,sizeof(lMBWorldID));
	//  DoInGameWorldLoadStuff(lMBWorldID);  //  Function Does Not Exist Yet

	_read(MissionFile,&lMissionTime,sizeof(lMissionTime));

	_read(MissionFile,&iMissionWeather,sizeof(iMissionWeather));
	//  See if we have to do anything for weather

	_read(MissionFile,&ltempvar,sizeof(ltempvar));

	_read(MissionFile,MissionTitle,ltempvar);

	_read(MissionFile,&ltempvar,sizeof(ltempvar));

	_read(MissionFile,MissionText,ltempvar);

	_read(MissionFile,&ltempvar,sizeof(ltempvar));

	_read(MissionFile,MissionSText,ltempvar);

	_read(MissionFile,&numobjs,sizeof(numobjs));
	iNumAIObjs = numobjs;
	if(numobjs)
//		_read(MissionFile,AIObjects,sizeof(MBObject) * numobjs);
	{
		IGGetPlanesMID1(numobjs, MissionFile);
	}

	_read(MissionFile,&numwaypts,sizeof(numwaypts));
	iNumWayPts = numwaypts;
	if(numwaypts)
		_read(MissionFile,AIWayPoints,sizeof(MBWayPoints) * numwaypts);

	_read(MissionFile,&numactions,sizeof(numactions));
	iNumActions = numactions;
	if(numactions)
		_read(MissionFile,AIActions,sizeof(MBActions) * numactions);

	AIInitFirstWayPt();

	_read(MissionFile,GoalList,sizeof(MBGoalInfo) * NUMGOALS);

	_read(MissionFile,GoalText,sizeof(MBGoalText) * NUMGOALS);

	_read(MissionFile,MessageList,sizeof(MBOneMessage) * MAXMESSAGES);
	//  Do we need to init sound stuff?

	_read(MissionFile, Sides,sizeof(MBOneSide) * NUMSIDES);

	_read(MissionFile, sScenarioFile, LONGFILENAME);

	_close(MissionFile);

	IGReadScenarioData(sScenarioFile);

	return(1);
}

//**************************************************************************************
void IGGetPlanesMID1(int numobjs, int MissionFile)
{
	int cnt;
	int cnt2;
	int numingroup;
	MBObject oneobject;
	MBObjectMID1 oneoldobj;
	int firstingroup;
	int numingroup2;

	numingroup2 = 0;
	for(cnt = 0; cnt < numobjs; cnt ++)
	{
		_read(MissionFile,&oneoldobj,sizeof(MBObjectMID1));
		MBConvertFromID1Obj(&oneobject, oneoldobj, &numingroup2, cnt);
		numingroup2 --;

//		PlaneTypes[cnt].Model = Load3DObject("Plane.3d");  // Will Need eventually
		IGAddMover(cnt, oneobject);
		//  Do I Need to separate ground vehicle here!!!!!!!!!
		numingroup = oneobject.numgroup - 1;
		firstingroup = cnt;
		Planes[cnt].AI.winglead = -1;
		Planes[cnt].AI.wingman = -1;
		Planes[cnt].AI.prevpair = -1;
		Planes[cnt].AI.nextpair = -1;

//		if(AIObjects[cnt].flag1 & AL_DEVICE_DRIVEN)
		if((cnt == 0) || (Planes[cnt].Status & AL_DEVICE_DRIVEN))
		{
			Camera1.CameraMode = CAMERA_COCKPIT;
			Camera1.SubType = COCKPIT_FRONT;
			Camera1.AttachedObject = (int *)&Planes[cnt];
			Camera1.DistanceFromObject = 300.0f*FOOT_MULTER;
		}


		for(cnt2 = 0; cnt2 < numingroup; cnt2 ++)
		{
			cnt ++;
			_read(MissionFile,&oneoldobj,sizeof(MBObjectMID1));
			MBConvertFromID1Obj(&oneobject, oneoldobj, &numingroup2, cnt);
			numingroup2 --;

			//  Might want to run it throught IGAddMover for things like damage or other plane unique things
			Planes[cnt] = Planes[firstingroup];
//			GetSortHeader(&Planes[cnt],PLANE_OBJECT);

			Planes[cnt].AI.winglead = -1;
			Planes[cnt].AI.wingman = -1;
			Planes[cnt].AI.prevpair = -1;
			Planes[cnt].AI.nextpair = -1;


//			Planes[cnt].WorldPosition.X += ConvertWayLoc(75 * cnt);  //  TEMP UNTIL FORMATIONS DONE
			Planes[cnt].WorldPosition.Z -= ConvertWayLoc(75 * cnt);  //  TEMP UNTIL FORMATIONS DONE



			if(Planes[cnt].Status & AL_DEVICE_DRIVEN)
			{
				Planes[cnt].Status &= ~AL_DEVICE_DRIVEN;
				Planes[cnt].Status |= AL_AI_DRIVEN;
			}
			if(cnt2 & 1)
			{
				Planes[cnt - 2].AI.nextpair = cnt;
				Planes[cnt].AI.prevpair = cnt - 2;
			}
			else
			{
				Planes[cnt - 1].AI.wingman = cnt;
				Planes[cnt].AI.winglead = cnt - 1;
			}
		}
	 	LastPlane = &Planes[cnt];
	}
}
#endif

//**************************************************************************************
PlaneType *IGLoadPlaneType(int typeidnum)
{
#if MBPDATFILEEXISTS
	int PlaneDataFile;
#endif
	int cnt;
//	PlaneData	oneplanedata;

	for(cnt = 0; cnt < MAX_PLANE_TYPES/*  NUM ACTIVE TYPES  */; cnt ++)
	{
		if(PlaneTypes[cnt].TypeNumber == typeidnum)
			return(&PlaneTypes[cnt]);
		else if(PlaneTypes[cnt].TypeNumber == -1)
			break;
	}
	if(cnt == MAX_PLANE_TYPES)
	{
#ifdef _DEBUG
		char errmsg[256];
		sprintf(errmsg, "Max Number of Plane Types (%d) exceeded", cnt);
	    MessageBox(hwnd, errmsg, "Error", MB_OK);
#endif
		return(&PlaneTypes[cnt -1]);
	}
	PlaneTypes[cnt].TypeNumber = typeidnum;
	/*  NUM ACTIVE TYPE + 1  */

	//  Get type info from Data File

#if MBPDATFILEEXISTS
	if ((PlaneDataFile=_open("planetypes.dat",_O_BINARY | _O_RDONLY))==NULL)
		return(NULL);

	while(!_eof(PlaneDataFile))
	{
		_read(PlaneDataFile,&oneplanedata,sizeof(PlaneData));
		if(oneplanedata.itypeid == typeidnum)
			break;
	}
#endif

	LoadPlaneModel(&PlaneTypes[cnt]);

//	PlaneTypes[cnt].Model = Load3dObject(  /*  File name from Data File  */  );
//	PlaneTypes[cnt].iRadar = IGLoadRadars(  /*  File name from Data File  */  );
//	PlaneTypes[cnt].iMaxSpeed = oneplanedata.;
//	PlaneTypes[cnt].iMaxAccel = oneplanedata.;
//	PlaneTypes[cnt].iMaxAlt = oneplanedata.;
//	PlaneTypes[cnt].iManeuver = oneplanedata.;
//	PlaneTypes[cnt].iFuelUsage = oneplanedata.;
#if MBPDATFILEEXISTS
	_close(PlaneDataFile);
#endif
	return(&PlaneTypes[cnt]);
}

//**************************************************************************************
int GetNumRocketsPerPod(int weaponid)
{
	switch(weaponid)
	{
		case 133:
			return(4);
			break;
		case 134:
		case 176:
			return(19);
			break;
		default:
			return(0);
			break;
	}
	return(0);
}

int IGGetTail(int planenum, RESOURCEFILE  *resFile)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char			szSquadron[260];	
	int groupnum = AIObjects[planenum].iBriefingGroup;

	if((groupnum < 1) || (groupnum > 8))
		return(0);

	switch(groupnum)
	{
		case 2:
			strcpy(szSquadron, resFile->szSquadron1);
			break;
		case 3:
			strcpy(szSquadron, resFile->szSquadron2);
			break;
		case 4:
			strcpy(szSquadron, resFile->szSquadron3);
			break;
		case 5:
			strcpy(szSquadron, resFile->szSquadron4);
			break;
		case 6:
			strcpy(szSquadron, resFile->szSquadron5);
			break;
		case 7:
			strcpy(szSquadron, resFile->szSquadron6);
			break;
		case 8:
			strcpy(szSquadron, resFile->szSquadron7);
			break;
		case 1:
		default:
			strcpy(szSquadron, resFile->szSquadron0);
			break;
	}
	_splitpath(szSquadron, drive, dir, fname, ext);

	if(!_stricmp("VFA82", fname))
	{
		return(0);
	}
	else if(!_stricmp("VFA37", fname))
	{
		return(1);
	}
	else if(!_stricmp("VFA136", fname))
	{
		return(2);
	}
	else if(!_stricmp("VFA15", fname))
	{
		return(3);
	}
	else if(!_stricmp("VFA34", fname))
	{
		return(4);
	}

	return(0);
}

