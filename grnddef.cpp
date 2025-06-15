//*******************************************************************************
//*  grnddef.cpp
//*
//*  This file contains functions dealing with ground defenses.
//*******************************************************************************
#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "particle.h"
#include "GameSettings.h"

extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)

extern DBWeaponType *get_weapon_ptr(long id);
extern WeaponType *IGLoadWeaponType(int typeidnum);
extern InfoProviderInstance *AllInfoProviders;
extern InfoProviderInstance *AllSectorCommandProviders;
extern InfoProviderInstance *AllGCIProviders;
extern InfoProviderInstance *AllSAMProviders;
extern InfoProviderInstance *AllAAAProviders;
extern AAWeaponInstance *AllAAWeapons;
extern BYTE WarHeadTypeConversion[];
extern float GetCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);
extern int GetCalculatedCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);
float GetCrossSectionFromGround(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);

GDRadarData *DEBUGradardat = NULL;

#define G_TIX	(GRAVITY*FTTOWU/2500.0)	// Gravity in WU/tick squared
//#define AAATEST
//#define AAALONGTEST
#define IGNORETYPE 10

DBWeaponType *pointweapon = NULL;

void AICheckGrndRadar()
{
	InfoProviderInstance *currgci;
	BasicInstance *clients;
	DBWeaponType *weapon;
	DBRadarType *radar;
	GDRadarData *radardat;
	int cnt;
	AAWeaponInstance *wclient;
	int tempvar;

	currgci = AllInfoProviders;

	while(currgci)
	{
		radar = GetRadarPtr((BasicInstance *)currgci);
		weapon = GetWeaponPtr((BasicInstance *)currgci);
		radardat = (GDRadarData *)currgci->AIDataBuf;
		if(radardat->lRAlertTimer >= 0)
		{
			tempvar = radardat->lRFlags1 & GD_HIGH_ALERT;
			tempvar = radardat->lRFlags1 & GD_MED_ALERT;
		}
		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = currgci->Clients[cnt];
			if(clients != NULL)
			{
				if(clients->Family == FAMILY_AAWEAPON)
				{
					weapon = GetWeaponPtr(clients);
					wclient = (AAWeaponInstance *)clients;
					radardat = (GDRadarData *)wclient->AIDataBuf;
				}
			}
		}
		currgci = (InfoProviderInstance *)currgci->Basics.NextRelatedInstance;
	}

	wclient = AllAAWeapons;
	while(wclient)
	{
		if (!(wclient->Basics.Flags & BI_NOT_ACTIVE))
		{
			radar = GetRadarPtr((BasicInstance *)wclient);
			weapon = GetWeaponPtr((BasicInstance *)wclient);
			radardat = (GDRadarData *)wclient->AIDataBuf;
			if(radardat->lRAlertTimer >= 0)
			{
				tempvar = radardat->lRFlags1 & GD_HIGH_ALERT;
				tempvar = radardat->lRFlags1 & GD_MED_ALERT;
			}
		}
		wclient = (AAWeaponInstance *)wclient->Basics.NextRelatedInstance;
	}


	WeaponParams *W = &Weapons[0];
	BasicInstance *tbasic;
	BasicInstance *tradar;
	AAWeaponInstance *tweapon;

	cnt = 0;
	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->iTargetType == TARGET_PLANE) && (W->pTarget == PlayerPlane))
			{
				if(W->LauncherType == GROUNDOBJECT)
				{
					tbasic = (BasicInstance *)W->Launcher;
					if(tbasic->Family == FAMILY_AAWEAPON)
					{
						tweapon = (AAWeaponInstance *)tbasic;
						radardat = (GDRadarData *)tweapon->AIDataBuf;
						tradar = (BasicInstance *)radardat->pWRadarSite;
					}
					else
					{
						tradar = NULL;
					}

					if(tradar)
					{
						if(tradar->Family == FAMILY_AAWEAPON)
						{
							wclient = (AAWeaponInstance *)tradar;
							radardat = (GDRadarData *)wclient->AIDataBuf;
						}
						else
						{
							currgci = (InfoProviderInstance *)tradar;
							radardat = (GDRadarData *)currgci->AIDataBuf;
						}
						if(radardat->lRFlags1 & GD_RADAR_LOCK)
						{
							if(PlayerPlane == radardat->Target)
							{
								cnt ++;
							}
						}
					}
				}
			}
		}
		W++;
	}
}

extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
//***********************************************************************************************************************************
void GDInitDefenses()
{
	InfoProviderInstance *currgci;
	InfoProviderInstance *ourprovider;
	InfoProviderInstance *tempprovider;
	BasicInstance *clients;
	GDRadarData *radardat;
	DBRadarType *radar;
	int cnt;
	AAWeaponInstance *currweapon;
	AAWeaponInstance *wclient;
	int timecnt = 0;
	int tempval;
	DBWeaponType *weapon=NULL;
	long frstactiontime = 100;
	GroundObject *pgroundobj=NULL;
	int isdestroyed;
	MovingVehicleParams *vehiclepnt;
	int maxenemyskill;
	float ftempval;
	int intrand;
	char sTxt[80];

	if(pChaffType == NULL)
	{
		pChaffType = IGLoadWeaponType(1);
	}
	if(pFlareType == NULL)
	{
		pFlareType = IGLoadWeaponType(2);
	}
	if(pEjectioSeatType == NULL)
	{
		pEjectioSeatType = IGLoadWeaponType(104);
	}
	if(pFriendlyChuteType == NULL)
	{
		pFriendlyChuteType = IGLoadWeaponType(105);
	}
	if(pEnemyChuteType == NULL)
	{
		pEnemyChuteType = IGLoadWeaponType(106);
	}
	if(pGuyOnGroundType == NULL)
	{
		pGuyOnGroundType = IGLoadWeaponType(107);
	}
	if(pLandingGateType == NULL)
	{
		pLandingGateType = IGLoadWeaponType(196);
	}

	if(pRocket5 == NULL)
	{
		if(!(pRocket5 = Load3DObject(RegPath("objects","lau10r.3dg"))))
		{
#ifdef _DEBUG
			DebugBreak();
#endif
			sprintf(sTxt,"Weapon Model 5 Inch Rocket File Not Found.");
			MessageBox(hwnd,sTxt,"File Error",MB_OK);
		}
	}
	if(pRocket275 == NULL)
	{
		if(!(pRocket275 = Load3DObject(RegPath("objects","lau68r.3dg"))))
		{
#ifdef _DEBUG
			DebugBreak();
#endif
			sprintf(sTxt,"Weapon Model 2.75 Inch Rocket File Not Found.");
			MessageBox(hwnd,sTxt,"File Error",MB_OK);
		}
	}


	if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
	{
		maxenemyskill = (GP_NOVICE - AIObjects[0].lDifficulty & 3) + 1;
		iMaxEnemyGrndSkill = AIObjects[0].lDifficulty & 3;
	}
	else
	{
		maxenemyskill = (GP_NOVICE - g_Settings.gp.nGroundDefense) + 1;
		iMaxEnemyGrndSkill = g_Settings.gp.nGroundDefense;
	}

#if 1
	BasicInstance *walker;
	BasicInstance *checkobject;
	long sideval;
	int friendly;

	if(g_nMissionType != SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
	{
		checkobject = walker = AllInstances;
		while(checkobject)
		{
			if(!(checkobject->SerialNumber & SN_PLUG_IN_MASK))
			{
				if(!AICCheckAllFences((checkobject->Position.X * WUTOFT), (checkobject->Position.Z * WUTOFT)))
				{
					checkobject->Flags |= BI_FRIENDLY;
					friendly = 1;
				}
				else
				{
					friendly = 0;
				}
				walker = checkobject;
				checkobject = (BasicInstance *)checkobject->NextInstance;
				while(checkobject)
				{
					if((fabs(checkobject->Position.X - walker->Position.X) > (10.0f * NMTOWU)) || (fabs(checkobject->Position.Z - walker->Position.Z) > (10.0f * NMTOWU)))
					{
						break;
					}
					else if(!(checkobject->SerialNumber & SN_PLUG_IN_MASK))
					{
						if(friendly)
						{
							checkobject->Flags |= BI_FRIENDLY;
						}
					}
					else
					{
						break;
					}
					checkobject = checkobject->NextInstance;
				}
			}
			else
			{
				checkobject = (BasicInstance *)checkobject->NextInstance;
			}
		}
	}

	if (iNumGroundObjects)
	{
		pgroundobj = pGroundObjectList;
		for(cnt = 0; cnt < iNumGroundObjects; cnt ++)
		{
			if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
			{
				sideval = 0;
			}
			else if(pgroundobj->iSide == AI_FRIENDLY)
			{
				sideval = BI_FRIENDLY;
			}
			else if(pgroundobj->iSide == AI_NEUTRAL)
			{
				sideval = BI_NEUTRAL;
			}
			else
			{
				sideval = 0;
			}

			walker = FindInstance(AllInstances, pgroundobj->dwSerialNumber);
			if(walker)
			{
				if(sideval)
				{
					walker->Flags |= sideval;
				}
				else
				{
					walker->Flags &= ~(BI_FRIENDLY|BI_NEUTRAL);;
				}
				if(pgroundobj->dwSerialNumber & SN_PLUG_IN_MASK)
				{
					walker = (BasicInstance *)walker->NextInstance;
					while((walker->SerialNumber & SN_PLUG_IN_MASK) == (pgroundobj->dwSerialNumber & SN_PLUG_IN_MASK))
					{
						if(sideval)
						{
							walker->Flags |= sideval;
						}
						else
						{
							walker->Flags &= ~(BI_FRIENDLY|BI_NEUTRAL);;
						}
						walker = (BasicInstance *)walker->NextInstance;
					}
				}
			}
			else if(pgroundobj->dwSerialNumber & SN_PLUG_IN_MASK)
			{
				walker = AllInstances;
				while (walker && (walker->SerialNumber < pgroundobj->dwSerialNumber))
				{
					walker = (BasicInstance *)walker->NextInstance;
				}
				if(walker)
				{
					while(walker && ((walker->SerialNumber & SN_PLUG_IN_MASK) == (pgroundobj->dwSerialNumber & SN_PLUG_IN_MASK)))
					{
						if(sideval)
						{
							walker->Flags |= sideval;
						}
						else
						{
							walker->Flags &= ~(BI_FRIENDLY|BI_NEUTRAL);;
						}
//							tempval = GDConvertGrndSide(walker);
						walker = (BasicInstance *)walker->NextInstance;
					}
				}
			}
			pgroundobj ++;
		}
	}
#endif

	currgci = AllInfoProviders;

	while(currgci)
	{
		radardat = (GDRadarData *)currgci->AIDataBuf;
//		radardat->lRActionTimer = (timecnt * 100);
		radardat->lRActionTimer = frstactiontime;
		radardat->lRLockTimer = -999999;
		radardat->lRGenericTimer = -1;
		radardat->lRAlertTimer = -1;
		radardat->lRFlags1 = (GD_RADAR_ON|GD_LOW_ALERT);
		radardat->iConfidence = 0;
		radardat->FireHeadingANG = 0;
		radardat->HeadingOffsetANG = 0;
		radardat->FirePitchANG = 0;
		radardat->PitchOffsetANG = 0;
		if(GDConvertGrndSide((BasicInstance *) currgci) == AI_ENEMY)
		{
			if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
			{
				radardat->iSkill = iMaxEnemyGrndSkill;
			}
			else
			{
				ftempval = frand() * (float)maxenemyskill;
				intrand = ftempval;
				radardat->iSkill = GP_NOVICE - intrand;
			}
			if(radardat->iSkill < 0)
			{
				radardat->iSkill = 0;
			}
		}
		else
		{
			radardat->iSkill = 0;
		}

		//  Check to see if currgci's provider has a radar, if so, currgci will not be on all the time.
		ourprovider = currgci->OurProvider;

		if(ourprovider == currgci)
		{
			currgci->OurProvider = NULL;
			ourprovider = NULL;
		}

		isdestroyed = 0;

		if(!InstanceIsBombable((BasicInstance *)currgci))
		{
			isdestroyed = 1;
			ourprovider = NULL;
			radardat->lRFlags1 |= GD_I_AM_DEAD;
		}


		if(ourprovider != NULL)
		{
			radar = GetRadarPtr((BasicInstance *)ourprovider);
			if(radar != NULL)
			{
				tempval = rand() & 0xF;
				if(tempval)
				{
					radardat->lRActionTimer += tempval * 60000;
					radardat->lRFlags1 = (GD_RADAR_TEMP_OFF|GD_LOW_ALERT);
				    radardat->Target = NULL;
				}
				else
				{
					tempval = rand() & 3;
					radardat->lRGenericTimer += tempval * 60000;
					radardat->lRFlags1 = (GD_RADAR_TEMP_ON|GD_LOW_ALERT);
				}
			}
		}

		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = currgci->Clients[cnt];

			if((BasicInstance *)currgci == clients)
			{
				currgci->Clients[cnt] = NULL;
				clients = NULL;
			}
			if(clients != NULL)
			{
				if(clients->Family == FAMILY_AAWEAPON)
				{
					wclient = (AAWeaponInstance *)clients;

					if ((wclient->Basics.Flags & (BI_NOT_ACTIVE | BI_DESTROYED)) || (isdestroyed))
					{
						wclient->OurProvider = NULL;
				 		currgci->Clients[cnt] = NULL;
					}

					radardat = (GDRadarData *)wclient->AIDataBuf;
					radardat->lRActionTimer = -1;
					radardat->lRLockTimer = -999999;
					radardat->lRAlertTimer = -1;
					radardat->iRBurstCounter = 0;
					radardat->lRBurstTimer = -1;
					radardat->pRBurstTarget = NULL;
					radardat->lRLockTimer = -999999;
					radardat->lRGenericTimer = -1;
					radardat->lRFlags1 = (GD_LOW_ALERT);
					radardat->iConfidence = 0;
					radardat->lWActionTimer = -1;
					radardat->lWGenericTimer = -1;
					radardat->pWRadarSite = NULL;
					radardat->lWReloadTimer = -1;
					radardat->lWNumberFired = 0;
					radardat->lWFlags1 = 0;
					radardat->Target = NULL;
					radardat->iTargetDist = -1;
					radardat->FireHeadingANG = 0;
					radardat->HeadingOffsetANG = 0;
					radardat->FirePitchANG = 0;
					radardat->PitchOffsetANG = 0;;
					if ((DWORD)wclient->Type->WeaponPtr < 0x400)
					{
						weapon = get_weapon_ptr((long)wclient->Type->WeaponPtr);
					}
					else
					{
						weapon = (DBWeaponType *)wclient->Type->WeaponPtr;
					}
					radardat->iWeaponIndex=RegisterWeapon(weapon->lWeaponID);
					radardat->WType = IGLoadWeaponType(weapon->lWeaponID);
					radardat->pAAAStream = 0;
					radardat->fpAimPoint.SetValues(-1.0f, -1.0f, -1.0f);
					radardat->lTimeDiff = 0;
					radardat->lTimeDiffOffset = 0;

					if(g_nMissionType == SIM_QUICK)
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
					}

					if(ourprovider)
					{
						radar = GetRadarPtr((BasicInstance *)ourprovider);
						if(radar != NULL)
						{
							tempval = rand() & 0xF;
							if(tempval)
							{
								radardat->lRActionTimer += tempval * 60000;

								if(g_nMissionType == SIM_QUICK)
								{
									radardat->lRFlags1 = (GD_RADAR_TEMP_OFF|GD_HIGH_ALERT);
									radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
								    radardat->Target = NULL;
								}
								else
								{
									radardat->lRFlags1 = (GD_RADAR_TEMP_OFF|GD_LOW_ALERT);
								    radardat->Target = NULL;
								}
							}
							else
							{
								tempval = rand() & 3;
								radardat->lRGenericTimer += tempval * 60000;
								if(g_nMissionType == SIM_QUICK)
								{
									radardat->lRFlags1 = (GD_RADAR_TEMP_ON|GD_HIGH_ALERT);
									radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
								}
								else
								{
									radardat->lRFlags1 = (GD_RADAR_TEMP_ON|GD_LOW_ALERT);
								}
							}
						}
					}
					else
					{
						radar = GetRadarPtr((BasicInstance *)wclient);
						if(radar != NULL)
						{
			//				radardat->lRActionTimer = (timecnt * 100);
							radardat->lRActionTimer = frstactiontime;
							radardat->lRLockTimer = -999999;
							radardat->lRAlertTimer = -1;
							radardat->lRLockTimer = -999999;
							radardat->lRGenericTimer = -1;
							if(g_nMissionType == SIM_QUICK)
							{
								radardat->lRFlags1 = (GD_RADAR_TEMP_ON|GD_HIGH_ALERT);
								radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
							}
							else
							{
								radardat->lRFlags1 = (GD_RADAR_ON|GD_LOW_ALERT);
							}
							radardat->iConfidence = 0;
							frstactiontime += 10000;
							if(frstactiontime > 180000)
							{
								frstactiontime -= 179000;
							}
							timecnt ++;
						}
						else
						{
							radardat->lRFlags1 = GD_VISUAL_SEARCH;
							radardat->lRAlertTimer = -1;
							radardat->lRLockTimer = -999999;
			//				radardat->lRActionTimer = (timecnt * 100);
							radardat->lRActionTimer = frstactiontime;
							frstactiontime += 10000;
							if(frstactiontime > 180000)
							{
								frstactiontime -= 179000;
							}
							timecnt ++;
						}
					}
				}
				else if(clients->Family == FAMILY_INFOPROVIDER)
				{
					tempprovider = (InfoProviderInstance *)clients;

					if(isdestroyed)
					{
						tempprovider->OurProvider = NULL;
						currgci->Clients[cnt] = NULL;
					}
				}
				if(!InstanceIsBombable(clients))
				{
					currgci->Clients[cnt] = NULL;
				}
			}
		}
		currgci = (InfoProviderInstance *)currgci->Basics.NextRelatedInstance;
		frstactiontime += 10000;
		if(frstactiontime > 180000)
		{
			frstactiontime -= 179000;
		}
		timecnt ++;
	}

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		ourprovider = currweapon->OurProvider;
		if((currweapon->Basics.Flags & BI_NOT_ACTIVE) || !InstanceIsBombable((BasicInstance *)currweapon))
		{
			radardat = (GDRadarData *)currweapon->AIDataBuf;
			radardat->lRFlags1 |= GD_I_AM_DEAD;
		}
		else if(ourprovider == NULL)
		{
			wclient = currweapon;
			radardat = (GDRadarData *)wclient->AIDataBuf;
			radardat->iRBurstCounter = 0;
			radardat->lRBurstTimer = -1;
			radardat->lRAlertTimer = -1;
			radardat->pRBurstTarget = NULL;
			radardat->lRActionTimer = -1;
			radardat->lRLockTimer = -999999;
			radardat->lRGenericTimer = -1;
			radardat->lRFlags1 = (GD_LOW_ALERT);
			radardat->iConfidence = 0;
			radardat->lWActionTimer = -1;
			radardat->lWGenericTimer = -1;
			radardat->pWRadarSite = NULL;
			radardat->lWReloadTimer = -1;
			radardat->lWNumberFired = 0;
			radardat->lWFlags1 = 0;
			radardat->Target = NULL;
			radardat->iTargetDist = -1;
			radardat->FireHeadingANG = 0;
			radardat->HeadingOffsetANG = 0;
			radardat->FirePitchANG = 0;
			radardat->PitchOffsetANG = 0;;
			radardat->pAAAStream = 0;
			radardat->fpAimPoint.SetValues(-1.0f, -1.0f, -1.0f);
			radardat->lTimeDiff = 0;
			radardat->lTimeDiffOffset = 0;
			radardat->iShotsTillNextCheck = 3;

			if(g_nMissionType == SIM_QUICK)
			{
				radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
				radardat->lRFlags1 |= GD_HIGH_ALERT;
				radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
			}

			if ((DWORD)wclient->Type->WeaponPtr < 0x400)
			{
				weapon = get_weapon_ptr((long)wclient->Type->WeaponPtr);
			}
			else
			{
				weapon = (DBWeaponType *)wclient->Type->WeaponPtr;
			}
			radardat->iWeaponIndex=RegisterWeapon(weapon->lWeaponID);
			radardat->WType = IGLoadWeaponType(weapon->lWeaponID);

			radar = GetRadarPtr((BasicInstance *)currweapon);
			if(radar != NULL)
			{
//				radardat->lRActionTimer = (timecnt * 100);
				radardat->lRActionTimer = frstactiontime;
				radardat->lRLockTimer = -999999;
				radardat->lRAlertTimer = -1;
				radardat->lRLockTimer = -999999;
				radardat->lRGenericTimer = -1;
				if(g_nMissionType == SIM_QUICK)
				{
					radardat->lRFlags1 = (GD_RADAR_TEMP_ON|GD_HIGH_ALERT);
					radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
				}
				else
				{
					radardat->lRFlags1 = (GD_RADAR_ON|GD_LOW_ALERT);
				}
				radardat->iConfidence = 0;
				frstactiontime += 10000;
				if(frstactiontime > 180000)
				{
					frstactiontime -= 179000;
				}
				timecnt ++;
			}
			else
			{
				radardat->lRFlags1 = GD_VISUAL_SEARCH;
				radardat->lRAlertTimer = -1;
				radardat->lRLockTimer = -999999;
//				radardat->lRActionTimer = (timecnt * 100);
				radardat->lRActionTimer = frstactiontime;
				frstactiontime += 10000;
				if(frstactiontime > 180000)
				{
					frstactiontime -= 179000;
				}
				timecnt ++;
			}
		}

		if(GDConvertGrndSide((BasicInstance *) currweapon) == AI_ENEMY)
		{
			if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
			{
				radardat->iSkill = iMaxEnemyGrndSkill;
			}
			else
			{
				ftempval = frand() * (float)maxenemyskill;
				intrand = ftempval;
				radardat->iSkill = GP_NOVICE - intrand;
			}
			if(radardat->iSkill < 0)
			{
				radardat->iSkill = 0;
			}
		}
		else
		{
			radardat->iSkill = 0;
		}

		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	int radarcnt;
	int weaponcnt;
	int radardatcnt;
	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		radarcnt = -1;
		weaponcnt = -1;
		radardatcnt = -1;
		int maxcnt;

		VClearRadardat(vehiclepnt);

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
					radardatcnt ++;
					weaponcnt ++;
					weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
				}
				else
				{
					radardatcnt ++;
					weaponcnt ++;
					weapon = get_weapon_ptr((long)pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
				}
			}
			else
			{
				if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_RADAR)
				{
					continue;
				}
				weaponcnt ++;
				radardatcnt ++;
				weapon = get_weapon_ptr((long)pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID);
			}

			radardat = &vehiclepnt->RadarWeaponData[radardatcnt];
			if(weapon)
			{
				vehiclepnt->RadarWeaponData[radardatcnt].iWeaponIndex=RegisterWeapon(weapon->lWeaponID);
				vehiclepnt->RadarWeaponData[radardatcnt].WType = IGLoadWeaponType(weapon->lWeaponID);
				if((weapon->iSeekerType == 2) || (weapon->iSeekerType == 3) || (weapon->iSeekerType == 8)
						|| (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10))
				{
					radardat->lRFlags1 = GD_VISUAL_SEARCH;
				}
			}
			else
			{
				vehiclepnt->RadarWeaponData[radardatcnt].iWeaponIndex=0;
				vehiclepnt->RadarWeaponData[radardatcnt].WType = NULL;
			}

			if(vehiclepnt->iSide == AI_ENEMY)
			{
				if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
				{
					radardat->iSkill = iMaxEnemyGrndSkill;
				}
				else
				{
					ftempval = frand() * (float)maxenemyskill;
					intrand = ftempval;
					vehiclepnt->RadarWeaponData[radardatcnt].iSkill = GP_NOVICE - intrand;
				}
				if(radardat->iSkill < 0)
				{
					radardat->iSkill = 0;
				}
			}
			else
			{
				vehiclepnt->RadarWeaponData[radardatcnt].iSkill = 0;
			}
		}
	}
}

//***********************************************************************************************************************************
void GDInitRuwaySides()
{
	int tempfence;
	RunwayInfo *runway;

	for (runway = AllRunways; runway != NULL; runway = runway->NextRunway)
	{
		if (runway->Flags & RI_DESTROYED)
			continue;

		if(runway->Pieces[0]->Basics.SerialNumber & SN_PLUG_IN_MASK)
		{
			runway->Flags |= (runway->Pieces[0]->Basics.Flags & (BI_FRIENDLY|BI_NEUTRAL));
		}
		else
		{
			tempfence = AICCheckAllFences((runway->ILSLandingPoint.X * WUTOFT), (runway->ILSLandingPoint.Z * WUTOFT));

			if(!tempfence)
			{
				runway->Flags |= BI_FRIENDLY;
			}
			//  May want to do something here to update "side" of all pieces of runway.  Might have to do it by distance.
		}
	}
}

//**************************************************************************************
void GDCheckDefenses()
{
	InfoProviderInstance *currinfo;
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	float currdist, tdist;
	PlaneParams *planepnt;
	AAWeaponInstance *foundweapon;
	float dx, dz;
	int reloading;
	DBWeaponType *weapon=NULL;

	iAAAFireAlreadyChecked = 0;

	currinfo = AllInfoProviders;

	while(currinfo)
	{
		radardat = (GDRadarData *)currinfo->AIDataBuf;
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF)))
		{
			if((iHotZoneCheckFlags & ZONES_GET_STATIC) && (lAINetFlags1 & NGAI_ACTIVE))
			{
				MAISetAHotZone((float)currinfo->Basics.Position.X, (float)currinfo->Basics.Position.Z, 1);
			}

			if(radardat->lRActionTimer >= 0)
			{
				if(radardat->lRFlags1 & GD_RADAR_LOCK)
				{
					AICheckForGateStealerG((BasicInstance *)currinfo, radardat, (PlaneParams *)radardat->Target);
				}
				radardat->lRActionTimer -= DeltaTicks;
				if(radardat->lRActionTimer < 0)
				{
					if(radardat->lRFlags1 & GD_RADAR_TEMP_OFF)
					{
						if((radardat->lRFlags1 & (GD_RADAR_OFF_FOR_HARM|GD_RADAR_TEMP_ON)) == GD_RADAR_OFF_FOR_HARM)
						{
							radardat->lRFlags1 |= GD_RADAR_ON;
						}
						else
						{
							radardat->lRGenericTimer = 5 * 60000;
							radardat->lRFlags1 |= (GD_RADAR_TEMP_ON);
						}
						radardat->lRFlags1 &= ~(GD_RADAR_TEMP_OFF|GD_RADAR_OFF_FOR_HARM);
					}
					AISimpleGroundRadar((BasicInstance *)currinfo);
				}
			}
			if(radardat->lRGenericTimer >= 0)
			{
				radardat->lRGenericTimer -= DeltaTicks;
				if(radardat->lRGenericTimer < 0)
				{
					if(GD_RADAR_TEMP_ON)
					{
						radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_PLAYER_PING|GD_RADAR_LOCK);
						radardat->lRFlags1 |= (GD_RADAR_TEMP_OFF);
					    radardat->Target = NULL;
						radardat->lRActionTimer = 15 * 60000;
					}
				}
			}
			if(radardat->lRAlertTimer >= 0)
			{
				radardat->lRAlertTimer -= DeltaTicks;
				if(radardat->lRAlertTimer < 0)
				{
					if(radardat->lRFlags1 & GD_HIGH_ALERT)
					{
						radardat->lRFlags1 &= ~GD_HIGH_ALERT;
						radardat->lRFlags1 |= GD_MED_ALERT;
						radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
					}
					else if(radardat->lRFlags1 & GD_MED_ALERT)
					{
						radardat->lRFlags1 &= ~GD_MED_ALERT;
						radardat->lRFlags1 |= GD_LOW_ALERT;
					}
					else
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_HIGH_ALERT);
						radardat->lRFlags1 |= GD_LOW_ALERT;
					}
				}
			}
			if(radardat->lRBurstTimer >= 0)
			{
				radardat->lRBurstTimer -= DeltaTicks;
				if(radardat->lRBurstTimer < 0)
				{
					if((radardat->iRBurstCounter) && (radardat->pRBurstTarget))
					{
						planepnt = (PlaneParams *)radardat->pRBurstTarget;
						if(((iInJump) && ((planepnt == PlayerPlane) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP))) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
						{
							radardat->pRBurstTarget = NULL;
						}
						else
						{
							dx = planepnt->WorldPosition.X - currinfo->Basics.Position.X;
							dz = planepnt->WorldPosition.Z - currinfo->Basics.Position.Z;
							tdist = QuickDistance(dx, dz);

							currdist = -1;
							foundweapon = GDCheckProviderForFreeSAM((BasicInstance *)currinfo, planepnt, tdist, &currdist);

							if(foundweapon != NULL)
							{
								if(!GDSetUpWeaponTargeting(foundweapon, planepnt, currdist))
								{
									radardat->pRBurstTarget = NULL;
								}
							}
							radardat->iRBurstCounter = radardat->iRBurstCounter - 1;
							radardat->lRBurstTimer = 4000 + ((rand() & 3) * 1000);
						}
					}
					if((radardat->iRBurstCounter <= 0) || (radardat->pRBurstTarget == NULL))
					{
						radardat->iRBurstCounter = -1;
						radardat->lRFlags1 &= ~(GD_BURST_MODE);
						radardat->pRBurstTarget = NULL;
					}
				}
			}
		}
		currinfo = (InfoProviderInstance *)currinfo->Basics.NextRelatedInstance;
	}

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		radardat = (GDRadarData *)currweapon->AIDataBuf;

		if((iHotZoneCheckFlags & ZONES_GET_STATIC) && (lAINetFlags1 & NGAI_ACTIVE))
		{
			MAISetAHotZone((float)currweapon->Basics.Position.X, (float)currweapon->Basics.Position.Z, 1);
		}

		if(lNoticeTimer < 0)
		{
			radardat->lRFlags1 &= ~(GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN);
		}
		reloading = 0;
		if(radardat->lWReloadTimer >= 0)
		{
			reloading = 1;
			radardat->lWReloadTimer -= DeltaTicks;
			if(radardat->lWReloadTimer < 0)
			{
				weapon = GetWeaponPtr((BasicInstance *)currweapon);

				if(!weapon)
				{
					radardat->lWNumberFired = 0;
				}
				else
				{
					if(weapon->iWeaponType != WEAPON_TYPE_GUN)
					{
						radardat->lWNumberFired = 0;
					}
					else if(radardat->lWNumberFired >= weapon->iBurstRate)
					{
						radardat->lWNumberFired = 0;
					}
				}
			}
		}
		if(radardat->lWGenericTimer >= 0)
		{
			radardat->lWGenericTimer -= DeltaTicks;
		}
		if((!reloading) && (!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_FIRING|GD_W_PREP_RANDOM)))
		{
			if(radardat->lWActionTimer >= 0)
			{
				radardat->lWActionTimer -= DeltaTicks;
				if(radardat->lWActionTimer < 0)
				{
					AIGroundWeaponAction(currweapon, radardat);
				}
				else if(radardat->lWFlags1 & (GD_W_SINGLE|GD_W_BURST|GD_W_STREAM))
				{
					GDUpdateImpactPointLite(currweapon);
				}
			}
		}
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF|GD_VISUAL_SEARCH)))
		{
			if(radardat->lRActionTimer >= 0)
			{
				if(radardat->lRFlags1 & GD_RADAR_LOCK)
				{
					AICheckForGateStealerG((BasicInstance *)currweapon, radardat, (PlaneParams *)radardat->Target);
				}

				radardat->lRActionTimer -= DeltaTicks;
				if(radardat->lRActionTimer < 0)
				{
					if(radardat->lRFlags1 & GD_RADAR_TEMP_OFF)
					{
						if((radardat->lRFlags1 & (GD_RADAR_OFF_FOR_HARM|GD_RADAR_TEMP_ON)) == GD_RADAR_OFF_FOR_HARM)
						{
							radardat->lRFlags1 |= GD_RADAR_ON;
						}
						else
						{
							radardat->lRGenericTimer = 5 * 60000;
							radardat->lRFlags1 |= (GD_RADAR_TEMP_ON);
						}
						radardat->lRFlags1 &= ~(GD_RADAR_TEMP_OFF|GD_RADAR_OFF_FOR_HARM);
					}
					AISimpleGroundRadar((BasicInstance *)currweapon);
				}
			}
			if(radardat->lRAlertTimer >= 0)
			{
				radardat->lRAlertTimer -= DeltaTicks;
				if(radardat->lRAlertTimer < 0)
				{
					if(radardat->lRFlags1 & GD_HIGH_ALERT)
					{
						radardat->lRFlags1 &= ~GD_HIGH_ALERT;
						radardat->lRFlags1 |= GD_MED_ALERT;
						radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
					}
					else if(radardat->lRFlags1 & GD_MED_ALERT)
					{
						radardat->lRFlags1 &= ~GD_MED_ALERT;
						radardat->lRFlags1 |= GD_LOW_ALERT;
					}
					else
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_HIGH_ALERT);
						radardat->lRFlags1 |= GD_LOW_ALERT;
					}
				}
			}
			if(radardat->lRBurstTimer >= 0)
			{
				radardat->lRBurstTimer -= DeltaTicks;
				if(radardat->lRBurstTimer < 0)
				{
					if((radardat->iRBurstCounter) && (radardat->pRBurstTarget))
					{
						planepnt = (PlaneParams *)radardat->pRBurstTarget;
						if(((iInJump) && ((planepnt == PlayerPlane) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP))) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
						{
							radardat->pRBurstTarget = NULL;
						}
						else
						{
							dx = planepnt->WorldPosition.X - currweapon->Basics.Position.X;
							dz = planepnt->WorldPosition.Z - currweapon->Basics.Position.Z;
							tdist = QuickDistance(dx, dz);

							currdist = -1;
							foundweapon = GDCheckProviderForFreeSAM((BasicInstance *)currweapon, planepnt, tdist, &currdist);

							if(foundweapon != NULL)
							{
								if(!GDSetUpWeaponTargeting(foundweapon, planepnt, currdist))
								{
									radardat->pRBurstTarget = NULL;
								}
							}
							radardat->iRBurstCounter = radardat->iRBurstCounter - 1;
							radardat->lRBurstTimer = 4000 + ((rand() & 3) * 1000);
						}
					}
					if((radardat->iRBurstCounter <= 0) || (radardat->pRBurstTarget == NULL))
					{
						radardat->iRBurstCounter = -1;
						radardat->lRFlags1 &= ~(GD_BURST_MODE);
						radardat->pRBurstTarget = NULL;
					}
				}
			}
		}

		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}
}

//**************************************************************************************
void AIGroundWeaponAction(AAWeaponInstance *currweapon, GDRadarData *radardat)
{
	DBWeaponType *weapon;
	BasicInstance *radarsite;
	GDRadarData *tempdat;

	if(radardat->lWFlags1 & GD_W_PREP_RANDOM)
	{
		GDSetUpRandomAAA(currweapon);
	}
	else if(radardat->lWFlags1 & (GD_W_SINGLE|GD_W_BURST|GD_W_STREAM))
	{
		GDUpdateImpactPoint(currweapon);
		GDFireAAA(currweapon);
		if(radardat->lWGenericTimer < 0)
		{
			radardat->lWFlags1 &= ~(GD_W_FIRING|GD_W_RANDOM_FIRE);
			radardat->Target = NULL;
			//radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening Maybe Not Needed JAP
			radardat->lWActionTimer = -1;

			if(radardat->lWFlags1 & (GD_W_STREAM))
			{
				radardat->lWReloadTimer = 4000 + (rand() & 0x7FF);
			}

			if(radardat->pWRadarSite)
			{
				radarsite = (BasicInstance *)radardat->pWRadarSite;
				tempdat = GDGetRadarData(radarsite);
				tempdat->lRFlags1 &= ~GD_RADAR_LOCK;
				tempdat->Target = NULL;  //  I think this should be OK...  SRE
			}
		}
	}
	else if(radardat->lWFlags1 & (GD_W_ACQUIRING))
	{
		InstantiateMissile(NULL, 0, (BasicInstance *)currweapon);
		radardat->lWFlags1 &= ~(GD_W_ACQUIRING);
		radardat->lWFlags1 |= (GD_W_FIRING);
		radardat->lWNumberFired = radardat->lWNumberFired + 1;
		weapon = GetWeaponPtr((BasicInstance *)currweapon);
		if(radardat->lWNumberFired >= weapon->iBurstRate)
		{
			radardat->lWReloadTimer = weapon->iRateOfFire * 60000;
		}
	}
}

//**************************************************************************************
void AISimpleGroundRadar(BasicInstance *radarsite)
{
  	PlaneParams *checkplane = &Planes[0];
//	float widthangle, heightangle;
	float radarrange, rangenm;
//	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	PlaneParams *foundplane = NULL;
	float nlfoundrange;
	PlaneParams *nlfoundplane = NULL;
//	float offangle, toffangle, toffpitch, offpitch;
//	float targbearing, nltargbearing;
//	float targpitch, nltargpitch;
//	PlaneParams *leadplane;
//	PlaneParams *orgtarget;
//	int ifoundplane;
//	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
//	int stationnum;
	int noupdates = 1;
	DBRadarType *radar;
	GDRadarData *radardat;
	PlaneParams *endplane;
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;
	float fworkval;
	float percsee;
	int foundweapon;
	int itemp;
	int grndside = GDConvertGrndSide(radarsite);
	int weaponsearch = 1;
	int paintcheck;
	float jammods[36];
	float offangle;
	int cnt;
	int highalert = 0;
	int hours;
	int isnight = 0;
	int inburner = 0;
	float visualrange = AI_VISUAL_RANGE_NM * NMTOWU;
	int shorterwait = 0;
	int isprovider = 0;
	int minalt = 0;
	int crosssig;

	int unpaused = 0;

	if(!radarsite)
	{
		return;
	}

	radar = GetRadarPtr((BasicInstance *)radarsite);

	if(radarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)radarsite;
		radardat = (GDRadarData *)&tweapon->AIDataBuf;
	}
	else
	{
		tprovider = (InfoProviderInstance *)radarsite;
		radardat = (GDRadarData *)&tprovider->AIDataBuf;
		isprovider = 1;
	}

	if(radar == NULL)
	{
		if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			hours = WorldParams.WorldTime/3600;

			if((hours < 6) || (hours > 18))
			{
				isnight = 1;
			}

			rangenm = AI_VISUAL_RANGE_NM;

			DBWeaponType *weapon;

			weapon = GetWeaponPtr((BasicInstance *)radarsite);

		}
		else
		{
			radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
			return;
		}
	}
	else
	{
		rangenm = radar->iMaxRange;
		minalt = radar->lMinElevation * FTTOWU;
	}

	radarrange = rangenm * NMTOWU;
	radarrange *= 1.0f - (radardat->iSkill * 0.1);
	foundrange = radarrange;
	foundplane = NULL;
	nlfoundrange = radarrange;
	nlfoundplane = NULL;

	if(isnight)
	{
		nlfoundrange = foundrange = radarrange * 8;
		visualrange /= 4;
	}

	radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;

	endplane = LastPlane;

//	if((radardat->lRFlags1 & GD_RADAR_LOCK) && (radar) && (!(radar->iRadarType & RADAR_TYPE_TWS)))
	if(radardat->lRFlags1 & GD_RADAR_LOCK)
	{
		if(radardat->Target == NULL)
		{
			radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
		}
		else
		{
			if(!AISeeIfStillGroundMissiled(radarsite, (PlaneParams *)radardat->Target, 1))
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
				radardat->Target = NULL;
			}
			else
			{
				checkplane = (PlaneParams *)radardat->Target;
				if(checkplane->HeightAboveGround > minalt)
				{
					endplane = checkplane;
				}
				else
				{
					radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
					radardat->Target = NULL;
				}
			}
		}
	}
	else
	{
		radardat->lRLockTimer = -999999;
	}

	AIGetGroundJammingMod(radarsite->Position, grndside, jammods);

#ifdef SRE_SAM_CHECK
	if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
	{
		char tempstr[80];
		sprintf(tempstr, "Doing Radar Sweep");
		AICAddAIRadioMsgs(tempstr, 50);
	}
#endif

	while (checkplane <= LastPlane)
	{
		if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (checkplane->OnGround == 0) && (checkplane->AI.iSide != grndside))
		{
			dx = checkplane->WorldPosition.X - radarsite->Position.X;
			dy = checkplane->WorldPosition.Y - radarsite->Position.Y;
			dz = checkplane->WorldPosition.Z - radarsite->Position.Z;

			if(isnight)
			{
				if((checkplane->RightThrustPercent > 0.75f) || (checkplane->LeftThrustPercent > 0.75f))
				{
					inburner = 1;
				}
				else
				{
					inburner = 0;
				}
			}

//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			if((fabs(dx) < (radarrange * (1 + (inburner * 7)))) && (fabs(dz) < (radarrange * (1 + (inburner * 7)))))
			{
				if(isprovider)
				{
					shorterwait = 1;
				}

				percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);

				if(percsee)
				{
					tdist = QuickDistance(dx, dz);

					percsee *= GDModifyNoticePercent(radarsite, radardat, checkplane);

					offangle = atan2(-dx, -dz) * 57.2958;
					while(offangle > 359)
						offangle -= 360;
					while(offangle < 0)
						offangle += 360;
					cnt = offangle / 10;

					percsee *= jammods[cnt];

					crosssig = AICheckRadarCrossSig(radarsite, checkplane, dy * WUTOFT, tdist * WUTONM, radar, (radardat->lRFlags1 & GD_RADAR_LOCK));

					if(!crosssig)
					{
						radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
					}

					if(tdist < (radarrange * jammods[cnt]))
					{
						shorterwait = 1;
						if((!(radardat->lRFlags1 & GD_VISUAL_SEARCH)) && (radar))
						{
	 						AIUpdateGroundRadarThreat(radarsite, checkplane, tdist, GROUNDOBJECT, radardat);
						}

						if((radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) && (checkplane->HeightAboveGround > minalt))
						{
							if((AISeeIfStillGroundMissiled(radarsite, checkplane, 1)) && crosssig)
							{
								noupdates = 0;
								if(percsee)
									percsee = 1.0f;
							}
						}
					}
					else if(radarsite == (BasicInstance *)checkplane->AI.pPaintedBy)
					{
						checkplane->AI.pPaintedBy = NULL;
						checkplane->AI.iPaintedByType = -1;
						checkplane->AI.iPaintDist = -1;
					}

					if((tdist < nlfoundrange) && (tdist < (radarrange * jammods[cnt])) && (checkplane->HeightAboveGround > minalt))
					{
					//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
					//  If was previous target, keep lock regardless of chance.

						if(checkplane)
						{
							if(checkplane->AI.iPaintedByType == MOVINGVEHICLE)
							{
								paintcheck = 0;
							}
							else
							{
								paintcheck = (checkplane->AI.pPaintedBy && (radarsite != (BasicInstance *)checkplane->AI.pPaintedBy) && (!GDAParentOfB(radarsite, (BasicInstance *)checkplane->AI.pPaintedBy)) && (GDSameAncestor(radarsite, (BasicInstance *)checkplane->AI.pPaintedBy)));
							}
						}
						else
						{
							paintcheck = 0;
						}

						if(!crosssig)
						{
							if(tdist > (visualrange * (1 + (inburner * 4))))
							{
								percsee = 0;
							}
						}

						if((checkplane->AI.lPlaneID == 85) && (tdist < (visualrange * 0.5f)))
						{
							percsee = 0;
						}

						if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
						{
							paintcheck = 0;
						}

						if(paintcheck)
						{
//							percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);

							percsee *= 2.0f;

							itemp = rand() & 127;

							if(itemp < (percsee * 128.0f))
							{
								foundplane = checkplane;
								foundrange = tdist;

								fworkval = checkplane->AI.iPaintDist * NMTOWU;
								if(tdist < fworkval)
								{
									if(!AIPlaneAlreadyTarget(GROUNDOBJECT, radarsite, checkplane))
									{
										nlfoundplane = checkplane;
										nlfoundrange = tdist;
									}
								}
							}
						}
						else
						{
//							percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);
							if(radarsite == (BasicInstance *)checkplane->AI.pPaintedBy)
							{
								percsee *= 3.0f;
							}

							itemp = rand() & 127;

							if(itemp < (percsee * 128.0f))
							{
								foundplane = checkplane;
								foundrange = tdist;
								if(!AISeeIfAlreadyTarget(AIRCRAFT, checkplane))  // new to allow targeting of more than 1 aircraft
								{
									nlfoundplane = checkplane;
									nlfoundrange = tdist;
								}
							}
						}

					}
					else if(radarsite == (BasicInstance *)checkplane->AI.pPaintedBy)
					{
//						percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);
						if(radarsite == (BasicInstance *)checkplane->AI.pPaintedBy)
						{
							percsee *= 3.0f;
						}

						itemp = rand() & 127;

						if(itemp < (percsee * 128.0f))
						{
							checkplane->AI.iPaintDist = tdist * WUTONM;
						}
						else if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK))) // We have missiles flying we need to keep eye on.
						{
							checkplane->AI.pPaintedBy = NULL;
							checkplane->AI.iPaintedByType = -1;
							checkplane->AI.iPaintDist = -1;
						}
					}
				}
				else if(radarsite == (BasicInstance *)checkplane->AI.pPaintedBy)
				{
					checkplane->AI.pPaintedBy = NULL;
					checkplane->AI.iPaintedByType = -1;
					checkplane->AI.iPaintDist = -1;
				}
			}
			else if(radarsite == (BasicInstance *)checkplane->AI.pPaintedBy)
			{
				checkplane->AI.pPaintedBy = NULL;
				checkplane->AI.iPaintedByType = -1;
				checkplane->AI.iPaintDist = -1;
			}

		}
		checkplane ++;
	}

#ifdef SRE_SAM_CHECK
	if((foundplane == NULL) && (radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK))) // We have missiles flying we need to keep eye on.
	{
		if(lpDD)
		  lpDD->FlipToGDISurface();
		SimPause = 1;
	}
#endif

	if(noupdates)
	{
		radardat->lRFlags1 &= ~GD_CHECK_MISSILES;
	}

	if(foundplane == NULL)
	{
		for(cnt = 0; cnt < 36; cnt ++)
		{
			if(jammods[cnt] < 0.75)
			{
				highalert = 1;
			}
		}

		radardat->Target = NULL;
		radardat->lRLockTimer = -999999;
		if(highalert)
		{
			if(lNoticeTimer >= 0)
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
			}
			else
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK|GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN);
			}
			radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
			radardat->lRFlags1 |= GD_HIGH_ALERT;
			radardat->lRActionTimer = 10000;  //  15 seconds until next sweep
			radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
			if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
			{
				radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
			}
			else
			{
				radardat->lRActionTimer = 10000 + (radardat->iSkill * 3000);  //  15 seconds until next sweep
			}
		}
		else if(radardat->lRFlags1 & GD_HIGH_ALERT)
		{
//			radardat->lRFlags1 &= ~GD_HIGH_ALERT;
//			radardat->lRFlags1 |= GD_MED_ALERT;
			radardat->lRActionTimer = 10000 + (radardat->iSkill * 3000);  //  15 seconds until next sweep
		}
		else if((radardat->lRFlags1 & GD_MED_ALERT) || (shorterwait))
		{
//			radardat->lRFlags1 &= ~GD_MED_ALERT;
//			radardat->lRFlags1 |= GD_LOW_ALERT;
			radardat->lRActionTimer = 40000 + (radardat->iSkill * 10000);  //  1 min until next sweep
		}
		else
		{
			radardat->lRActionTimer = 120000 + (radardat->iSkill * 15000);  //  3 mins until next sweep
		}
		if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			if(radardat->lRActionTimer > 60000)
			{
				radardat->lRActionTimer = 60000 + (radardat->iSkill * 10000);  //  1 mins until next sweep
			}
		}
		if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
		{
			radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		}
		return;
	}

	if(nlfoundplane != NULL)
	{
		foundplane = nlfoundplane;
		foundrange = nlfoundrange;
	}

	if((!MultiPlayer) || (lAINetFlags1 & NGAI_ACTIVE))
	{
		radardat->Target = foundplane;
		if((!foundplane->AI.pPaintedBy) || (foundplane->AI.iPaintedByType == MOVINGVEHICLE))
		{
			itemp = AICheckIfAnyInterception(AIGetLeader(foundplane), grndside);

			if(!itemp)
			{
				AIContactAirCommand(radarsite, foundplane, grndside);
			}
			else if(itemp == -1)
			{
				weaponsearch = 0;
			}
		}

		//***  Don't shoot at escortable planes (weapons hold) or neutral planes that are not weapons free.
		if((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[grndside]))
		{
		 	weaponsearch = 0;
		}
		else if((foundplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2))
		{
		 	weaponsearch = 0;
		}

		foundweapon = 0;

		if(weaponsearch)
		{
#ifdef SRE_SAM_CHECK
			char tempstr[80];

			sprintf(tempstr, "Looking for weapon for %d", foundplane - Planes);
			AICAddAIRadioMsgs(tempstr, 50);
#endif

			foundweapon = GDCheckForAAAFire(radarsite, foundplane, foundrange);

#ifndef AAATEST
			int maxnum = 3 - iMaxEnemyGrndSkill;
			if(maxnum < 1)
			{
				maxnum = 1;
			}


			if((GDNumLaunchingAtPlane(foundplane) + AINumMissilesAtPlane(foundplane)) < maxnum)
			{
				foundweapon += GDLookForFreeSAM(radarsite, foundplane, foundrange);
			}
#endif
			GDUpgradeTreeAlert(GDGetInfoLeader(radarsite), radarsite, foundplane->WorldPosition);
		}
	}
	else
	{
		if((!foundplane->AI.pPaintedBy) || (foundplane->AI.iPaintedByType == MOVINGVEHICLE))
		{
			itemp = AICheckIfAnyInterception(AIGetLeader(foundplane), grndside);

			if(itemp == -1)
			{
				weaponsearch = 0;
			}
		}

		if(weaponsearch)
		{
			GDUpgradeTreeAlert(GDGetInfoLeader(radarsite), radarsite, foundplane->WorldPosition);
		}
	}

	if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
	{
//		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
//		radardat->lRFlags1 |= GD_HIGH_ALERT;
		if(radardat->lRActionTimer < 0)
		{
			radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		}

		if(foundweapon && (!(radardat->lRFlags1 & GD_HIGH_ALERT)))
		{
			radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
			radardat->lRFlags1 |= GD_HIGH_ALERT;
			radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
		}
	}
	else if(foundweapon && (!(radardat->lRFlags1 & GD_HIGH_ALERT)))
	{
		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_HIGH_ALERT;
		radardat->lRActionTimer = 10000 + (radardat->iSkill * 3000);  //  15 seconds until next sweep
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else if(radardat->lRFlags1 & GD_HIGH_ALERT)
	{
		radardat->lRActionTimer = 10000 + (radardat->iSkill * 3000);  //  15 seconds until next sweep
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else
	{
		radardat->lRFlags1 &= ~(GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_MED_ALERT;
		radardat->lRActionTimer = 40000 + (radardat->iSkill * 10000);  //  1 min until next sweep
		radardat->lRAlertTimer = 600000;	//  Med Alert for 10 minutes
	}

	if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
	{
		if(radardat->lRActionTimer > 60000)
		{
			radardat->lRActionTimer = 60000 + (radardat->iSkill * 10000);  //  1 mins until next sweep
		}
	}

	if(unpaused)
	{
		UnPauseFromDiskHit();
	}
}

//***********************************************************************************************************************************
int GDCheckForAAAFire(BasicInstance *radarsite, PlaneParams *planepnt, float targdist)
{
	AAWeaponInstance *foundweapon;
	InfoProviderInstance *treetop;
	float currdist = -1;
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;
	int returnval = 0;
	int setplane = 0;
	int maxnum;

	maxnum = 3 - iMaxEnemyGrndSkill;
	if(maxnum < 1)
	{
		maxnum = 1;
	}


	if((!radarsite) || (!planepnt))
	{
		return(0);
	}

	foundweapon = GDCheckProviderForFreeAAA(radarsite, planepnt, targdist, &currdist);

	if(GDNumAAAFiringAtPlane(planepnt) >= maxnum)
	{
		foundweapon = NULL;
	}

#if 0  //  Won't have other nodes randomly fire
	if(foundweapon != NULL)
	{
		setplane = GDSetUpWeaponTargeting(foundweapon, planepnt, currdist);

		returnval = 1;
	}
	else
	{
		if(radarsite->Family == FAMILY_AAWEAPON)
		{
			tweapon = (AAWeaponInstance *)radarsite;
			treetop = tweapon->OurProvider;
		}
		else
		{
			tprovider = (InfoProviderInstance *)radarsite;
			treetop = tprovider->OurProvider;
		}

		if(treetop != NULL)
		{
			while(treetop->OurProvider)
			{
				treetop = treetop->OurProvider;
			}
			foundweapon = GDCheckTreeForFreeAAA(treetop, radarsite, planepnt, targdist, &currdist);

			if(GDNumAAAFiringAtPlane(planepnt) >= maxnum)
			{
				foundweapon = NULL;
			}

			if(foundweapon != NULL)
			{
				setplane = GDSetUpWeaponTargeting(foundweapon, planepnt, currdist);
			}
		}
	}
#else
	if(foundweapon != NULL)
	{
		setplane = GDSetUpWeaponTargeting(foundweapon, planepnt, currdist);

		returnval = 1;
	}

	if(radarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)radarsite;
		treetop = tweapon->OurProvider;
	}
	else
	{
		tprovider = (InfoProviderInstance *)radarsite;
		treetop = tprovider->OurProvider;
	}

	if(treetop != NULL)
	{
		while(treetop->OurProvider)
		{
			treetop = treetop->OurProvider;
		}
		foundweapon = GDCheckTreeForFreeAAA(treetop, radarsite, planepnt, targdist, &currdist);

		if((GDNumAAAFiringAtPlane(planepnt) >= maxnum) || returnval)
		{
			foundweapon = NULL;
		}

		if(foundweapon != NULL)
		{
			setplane = GDSetUpWeaponTargeting(foundweapon, planepnt, currdist);
		}
	}
#endif

	if(!setplane)
	{
		if(planepnt->AI.pPaintedBy == NULL)
		{
			planepnt->AI.pPaintedBy = (BasicInstance *)radarsite;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = targdist * WUTONM;
		}
	}

	return(returnval);
}

//***********************************************************************************************************************************
int GDLookForFreeSAM(BasicInstance *radarsite, PlaneParams *planepnt, float targdist)
{
	AAWeaponInstance *foundweapon;
	InfoProviderInstance *treetop;
	float currdist = -1;
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;
	int returnval = 0;
	int setplane = 0;

	if((!radarsite) || (!planepnt))
	{
		return(0);
	}

	foundweapon = GDCheckProviderForFreeSAM(radarsite, planepnt, targdist, &currdist);

	if(foundweapon != NULL)
	{
		setplane = GDSetUpWeaponTargeting(foundweapon, planepnt, currdist);

		returnval = 1;
	}
	else
	{
		if(radarsite->Family == FAMILY_AAWEAPON)
		{
			tweapon = (AAWeaponInstance *)radarsite;
			treetop = tweapon->OurProvider;
		}
		else
		{
			tprovider = (InfoProviderInstance *)radarsite;
			treetop = tprovider->OurProvider;
		}

		if(treetop != NULL)
		{
			while(treetop->OurProvider)
			{
				treetop = treetop->OurProvider;
			}
			foundweapon = GDCheckTreeForFreeSAM(treetop, radarsite, planepnt, targdist, &currdist);
			if(foundweapon != NULL)
			{
				setplane = GDSetUpWeaponTargeting(foundweapon, planepnt, currdist);
			}
		}
	}

	if(!setplane)
	{
		if(planepnt->AI.pPaintedBy == NULL)
		{
			planepnt->AI.pPaintedBy = (BasicInstance *)radarsite;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = targdist * WUTONM;
		}
	}

	return(returnval);
}

//***********************************************************************************************************************************
int GDSetUpWeaponTargeting(AAWeaponInstance *foundweapon, PlaneParams *planepnt, float currdist)
{
	DBRadarType *radar;
	GDRadarData *radardat;
	InfoProviderInstance *treetop;
	DBWeaponType *weapon;
	int notradar = 0;
	int tempnum;
	int isaaa;
	long lsecstolaunch;
	float tdistnm = currdist * WUTONM;

	if((!foundweapon) || (!planepnt))
	{
		return(0);
	}

	radardat = (GDRadarData *)foundweapon->AIDataBuf;
	radardat->Target = planepnt;
	radardat->iTargetDist = currdist * WUTONM;
	lsecstolaunch = radardat->lWActionTimer = 7000 + (radardat->iSkill * 3000);	//  7 seconds to acquire target;
	radardat->lWFlags1 &= ~(GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE);
	radardat->lWFlags1 |= GD_W_ACQUIRING;
	//radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening Maybe Not Needed JAP

	radar = GetRadarPtr((BasicInstance *)foundweapon);
	weapon = GetWeaponPtr((BasicInstance *)foundweapon);

	if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10) || (weapon->iWeaponType == 6))
	{
//		if(weapon->iSeekerType == 10)
//		{
//			notradar = 1;
//		}
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

	if((radar != NULL) || (notradar))
	{
		if((!notradar) && (!GDCheckNumRadarTracks((BasicInstance *)foundweapon, radar, TARGET_PLANE)))
		{
			radardat = (GDRadarData *)foundweapon->AIDataBuf;
			radardat->Target = NULL;
			radardat->iTargetDist = -1;
			radardat->lWActionTimer = -1;	//  0 seconds to acquire target;
			radardat->lWFlags1 &= ~GD_W_ACQUIRING;
			return(0);
		}

		if(radardat->lRActionTimer > 3000)
			radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_HIGH_ALERT;
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
		if((planepnt == PlayerPlane) && (radar != NULL))
		{
			radardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
		}

		if(notradar)
		{
			radardat->pWRadarSite = NULL;
			if(isaaa)
			{
				GDFireInitialAAA(foundweapon, planepnt);
			}
			if(MultiPlayer)
			{
				NetPutGroundLock((BasicInstance *)foundweapon, NULL, planepnt);
			}
		}
		else
		{
			if(planepnt->AI.pPaintedBy == NULL)
			{
				planepnt->AI.pPaintedBy = (BasicInstance *)foundweapon;
				planepnt->AI.iPaintedByType = GROUNDOBJECT;
				planepnt->AI.iPaintDist = currdist * WUTONM;
			}
			else if(planepnt->AI.iPaintDist > (currdist * WUTONM))
			{
				planepnt->AI.pPaintedBy = (BasicInstance *)foundweapon;
				planepnt->AI.iPaintedByType = GROUNDOBJECT;
				planepnt->AI.iPaintDist = currdist * WUTONM;
			}

	//		if(weapon->iSeekerType == 1)

			if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)))
			{
				radardat->lRActionTimer = lsecstolaunch + 1000;  //  radardat->lWActionTimer + 1000;
			}

			if((radar->iRadarType & RADAR_TYPE_TWS) && ((weapon->iSeekerType == 1) || (weapon->iSeekerType == 7)))
			{
				radardat->lRFlags1 |= GD_CHECK_MISSILES;
			}
			else  //  if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
			{
				radardat->lRFlags1 |= GD_RADAR_LOCK;
				GDCheckForPingReport((BasicInstance *)foundweapon, planepnt);
			}

			if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				radardat->lRGenericTimer = 300000;
			}

			radardat->pWRadarSite = (BasicInstance *)foundweapon;
			if(isaaa)
			{
				GDFireInitialAAA(foundweapon, planepnt);
			}
			else if(!(radardat->lRFlags1 & GD_BURST_MODE))
			{
				if(radar->iSearchRate > 1)
				{
					tempnum = rand() & 127;
					tempnum = ((tempnum * radar->iSearchRate) / 128);
					if(tempnum)
					{
						radardat->iRBurstCounter = tempnum;
						radardat->lRBurstTimer = 4000 + ((rand() & 3) * 1000);
						radardat->pRBurstTarget = planepnt;
						radardat->lRFlags1 |= GD_BURST_MODE;
					}
				}
			}
			if(MultiPlayer)
			{
				NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)foundweapon, planepnt);
			}
		}
		return(1);
	}
	else
	{
		treetop = foundweapon->OurProvider;
		while(treetop)
		{
			radar = GetRadarPtr((BasicInstance *)treetop);
			if((radar) && (GDCheckNumRadarTracks((BasicInstance *)treetop, radar, TARGET_PLANE)) && (radar->iRadarType & (RADAR_TYPE_TWS|RADAR_TYPE_STT)) && (radar->iMaxRange >= tdistnm) && (AICheckRadarCrossSig((BasicInstance *)treetop, planepnt, (planepnt->WorldPosition.Y - treetop->Basics.Position.Y) * WUTOFT, tdistnm, radar, (!(radar->iRadarType & RADAR_TYPE_TWS)))))
			{
				radardat->pWRadarSite = (BasicInstance *)treetop;
				planepnt->AI.pPaintedBy = (BasicInstance *)treetop;
				planepnt->AI.iPaintedByType = GROUNDOBJECT;
				planepnt->AI.iPaintDist = (treetop->Basics.Position - planepnt->WorldPosition) * WUTONM;
				radardat = (GDRadarData *)treetop->AIDataBuf;

				radardat->Target = planepnt;
				if(radardat->lRActionTimer > 3000)
					radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
				radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
				radardat->lRFlags1 |= GD_HIGH_ALERT;
				radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
				if(planepnt == PlayerPlane)
				{
					radardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
				}

				if(!DEBUGradardat)
					DEBUGradardat = radardat;
//				if(weapon->iSeekerType == 1)
				if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)))
				{
					radardat->lRActionTimer = lsecstolaunch + 1000;  //  radardat->lWActionTimer + 1000;
				}

				if((radar->iRadarType & RADAR_TYPE_TWS) && ((weapon->iSeekerType == 1) || (weapon->iSeekerType == 7)))
				{
					radardat->lRFlags1 |= GD_CHECK_MISSILES;
					if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
					{
						radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
						radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
					}
					if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
					{
						radardat->lRGenericTimer = 300000;
						if(radardat->lRActionTimer > 3000)  //  60000 ?
						{
							radardat->lRActionTimer = 3000;  //  60000 ?
						}
					}
					if(!(radardat->lRFlags1 & GD_BURST_MODE))
					{
						if(radar->iSearchRate > 1)
						{
							tempnum = rand() & 127;
							tempnum = ((tempnum * radar->iSearchRate) / 128);
							if(tempnum)
							{
								radardat->iRBurstCounter = tempnum;
								radardat->lRBurstTimer = 9000 + ((rand() & 3) * 1000);
								radardat->pRBurstTarget = planepnt;
								radardat->lRFlags1 |= GD_BURST_MODE;
							}
						}
					}
					else
					{
						radardat->lWActionTimer = 1000;	//  7 seconds to acquire target;
					}

					if(MultiPlayer)
					{
						NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)treetop, planepnt);
					}

					return(1);
				}
				else if(weapon->iSeekerType == 7)
				{
					if((!(radardat->lRFlags1 & GD_RADAR_LOCK)) || ((radardat->Target == NULL) || (radardat->Target == planepnt)))
					{
						radardat->lRFlags1 |= GD_RADAR_LOCK;
						radardat->Target = planepnt;
						GDCheckForPingReport((BasicInstance *)treetop, planepnt);
						if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
						{
							radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
							radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
						}
						if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
						{
							radardat->lRGenericTimer = 300000;
							if(radardat->lRActionTimer > 3000)  //  60000 ?
							{
								radardat->lRActionTimer = 3000;  //  60000 ?
							}
						}
						if(!(radardat->lRFlags1 & GD_BURST_MODE))
						{
							if(radar->iSearchRate > 1)
							{
								tempnum = rand() & 127;
								tempnum = ((tempnum * radar->iSearchRate) / 128);
								if(tempnum)
								{
									radardat->iRBurstCounter = tempnum;
									radardat->lRBurstTimer = 9000 + ((rand() & 3) * 1000);
									radardat->pRBurstTarget = planepnt;
									radardat->lRFlags1 |= GD_BURST_MODE;
								}
							}
						}
						else
						{
							radardat->lWActionTimer = 1000;	//  7 seconds to acquire target;
						}

						if(MultiPlayer)
						{
							NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)treetop, planepnt);
						}
						return(1);
					}
				}
				else if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10))  // Not sure about 10
				{
					if((!(radardat->lRFlags1 & GD_RADAR_LOCK)) || ((radardat->Target == NULL) || (radardat->Target == planepnt)))
					{
						radardat->lRFlags1 |= GD_RADAR_LOCK;
						radardat->Target = planepnt;
						GDCheckForPingReport((BasicInstance *)treetop, planepnt);
						if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
						{
							radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
							radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
						}
						if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
						{
							radardat->lRGenericTimer = 300000;
							if(radardat->lRActionTimer > 3000)  //  60000 ?
							{
								radardat->lRActionTimer = 3000;  //  60000 ?
							}
						}
						else
						{
							radardat->lWActionTimer = 1000;	//  1 seconds to acquire target;
						}
						if(isaaa)
						{
							GDFireInitialAAA(foundweapon, planepnt);
						}
						if(MultiPlayer)
						{
							NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)treetop, planepnt);
						}
						return(1);
					}
				}
				else
				{
					if(isaaa)
					{
						GDFireInitialAAA(foundweapon, planepnt);
					}

					if(MultiPlayer)
					{
						NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)treetop, planepnt);
					}
					return(1);
				}
			}
			if(treetop == foundweapon->OurProvider)
			{
				if(GDCheckChildrensRadar(foundweapon, planepnt, currdist, treetop))
				{
					return(1);
				}
			}
			treetop = treetop->OurProvider;
		}
	}

	if(isaaa)
	{
		radardat = (GDRadarData *)foundweapon->AIDataBuf;
		radardat->pWRadarSite = NULL;
		GDFireInitialAAA(foundweapon, planepnt);
		if(MultiPlayer)
		{
			NetPutGroundLock((BasicInstance *)foundweapon, NULL, planepnt);
		}
		return(1);
	}

	radardat = (GDRadarData *)foundweapon->AIDataBuf;
	radardat->Target = NULL;
	radardat->iTargetDist = -1;
	radardat->lWActionTimer = -1;	//  0 seconds to acquire target;
	radardat->lWFlags1 &= ~GD_W_ACQUIRING;
	return(0);
}

//***********************************************************************************************************************************
int GDCheckChildrensRadar(AAWeaponInstance *foundweapon, PlaneParams *planepnt, float currdist, InfoProviderInstance *treetop)
{
	BasicInstance *clients;
	InfoProviderInstance *infochild;
	DBRadarType *radar;
	GDRadarData *radardat;
	DBWeaponType *weapon;
	int tempnum;
	int isaaa = 0;
	float tdistnm = currdist * WUTONM;

	weapon = GetWeaponPtr((BasicInstance *)foundweapon);

	if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10) || (weapon->iWeaponType == 6))
	{
		isaaa = 1;
	}

	for(int cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
	{
		clients = treetop->Clients[cnt];
		radar = NULL;
		if(clients != NULL)
		{
			if(clients->Family == FAMILY_INFOPROVIDER)
			{
				radar = GetRadarPtr(clients);
				infochild = (InfoProviderInstance *)clients;
			}
		}
//		if(radar)
		if((radar) && (GDCheckNumRadarTracks(clients, radar, TARGET_PLANE)) && (radar->iRadarType & (RADAR_TYPE_TWS|RADAR_TYPE_STT)) && (radar->iMaxRange >= tdistnm) && (AICheckRadarCrossSig(clients, planepnt, (planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT, tdistnm, radar, (!(radar->iRadarType & RADAR_TYPE_TWS)))))
		{
			radardat = (GDRadarData *)infochild->AIDataBuf;
			radardat->pWRadarSite = (BasicInstance *)infochild;
			planepnt->AI.pPaintedBy = (BasicInstance *)infochild;
			planepnt->AI.iPaintedByType = GROUNDOBJECT;
			planepnt->AI.iPaintDist = (infochild->Basics.Position - planepnt->WorldPosition) * WUTONM;
			if(weapon->iSeekerType == 1)
			{
				radardat->Target = planepnt;
				radardat->lRFlags1 |= GD_CHECK_MISSILES;
				if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
				{
					radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
					radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
				}
				if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
				{
					radardat->lRGenericTimer = 300000;
					if(radardat->lRActionTimer > 3000)  //  60000 ?
					{
						radardat->lRActionTimer = 3000;  //  60000 ?
					}
				}
				if(!(radardat->lRFlags1 & GD_BURST_MODE))
				{
					if(radar->iSearchRate > 1)
					{
						tempnum = rand() & 127;
						tempnum = ((tempnum * radar->iSearchRate) / 128);
						if(tempnum)
						{
							radardat->iRBurstCounter = tempnum;
							radardat->lRBurstTimer = 9000 + ((rand() & 3) * 1000);
							radardat->pRBurstTarget = planepnt;
							radardat->lRFlags1 |= GD_BURST_MODE;
						}
					}
				}
				else
				{
					radardat->lWActionTimer = 1000;	//  7 seconds to acquire target;
				}

				if(MultiPlayer)
				{
					NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)infochild, planepnt);
				}

				return(1);
			}
			else if(weapon->iSeekerType == 7)
			{
				if((!(radardat->lRFlags1 & GD_RADAR_LOCK)) || ((radardat->Target == NULL) || (radardat->Target == planepnt)))
				{
					radardat->lRFlags1 |= GD_RADAR_LOCK;
					radardat->Target = planepnt;
					GDCheckForPingReport((BasicInstance *)infochild, planepnt);
					if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
					{
						radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
						radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
					}
					if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
					{
						radardat->lRGenericTimer = 300000;
						if(radardat->lRActionTimer > 3000)  //  60000 ?
						{
							radardat->lRActionTimer = 3000;  //  60000 ?
						}
					}
					if(!(radardat->lRFlags1 & GD_BURST_MODE))
					{
						if(radar->iSearchRate > 1)
						{
							tempnum = rand() & 127;
							tempnum = ((tempnum * radar->iSearchRate) / 128);
							if(tempnum)
							{
								radardat->iRBurstCounter = tempnum;
								radardat->lRBurstTimer = 9000 + ((rand() & 3) * 1000);
								radardat->pRBurstTarget = planepnt;
								radardat->lRFlags1 |= GD_BURST_MODE;
							}
						}
					}
					else
					{
						radardat->lWActionTimer = 1000;	//  7 seconds to acquire target;
					}

					if(MultiPlayer)
					{
						NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)infochild, planepnt);
					}
					return(1);
				}
			}
			else if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10))  // Not sure about 10
			{
				if((!(radardat->lRFlags1 & GD_RADAR_LOCK)) || ((radardat->Target == NULL) || (radardat->Target == planepnt)))
				{
					radardat->lRFlags1 |= GD_RADAR_LOCK;
					radardat->Target = planepnt;
					GDCheckForPingReport((BasicInstance *)infochild, planepnt);
					if((radardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
					{
						radardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
						radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
					}
					if(radardat->lRFlags1 & GD_RADAR_TEMP_ON)
					{
						radardat->lRGenericTimer = 300000;
						if(radardat->lRActionTimer > 3000)  //  60000 ?
						{
							radardat->lRActionTimer = 3000;  //  60000 ?
						}
					}
					else
					{
						radardat->lWActionTimer = 1000;	//  1 seconds to acquire target;
					}
					if(isaaa)
					{
						GDFireInitialAAA(foundweapon, planepnt);
					}
					if(MultiPlayer)
					{
						NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)infochild, planepnt);
					}
					return(1);
				}
			}
			else
			{
				if(isaaa)
				{
					GDFireInitialAAA(foundweapon, planepnt);
				}

				if(MultiPlayer)
				{
					NetPutGroundLock((BasicInstance *)foundweapon, (BasicInstance *)infochild, planepnt);
				}
				return(1);
			}
		}
	}
	return(0);
}


//***********************************************************************************************************************************
AAWeaponInstance *GDCheckProviderForFreeSAM(BasicInstance *currinst, PlaneParams *planepnt, float targdist, float *currdist)
{
	InfoProviderInstance *currinfo;
	BasicInstance *clients;
	DBWeaponType *weapon;
	GDRadarData *radardat;
	int cnt;
	AAWeaponInstance *wclient;
	AAWeaponInstance *foundweapon = NULL;
	AAWeaponInstance *childweapon;
	float targdistnm = targdist * WUTONM;
	double our_dist;
	double our_distnm;
	int ignoreweap = 0;

	if((!currinst) || (!planepnt))
	{
		return(0);
	}

	if(currinst->Family == FAMILY_AAWEAPON)
	{
		weapon = GetWeaponPtr(currinst);
		if(weapon)
		{
//			if((weapon->iWeaponType == 8) && ((float)weapon->iRange > targdistnm) && (weapon->fMinRange <= targdistnm) && (((planepnt->WorldPosition.Y - currinst->Position.Y) * WUTOFT) < weapon->iMaxAlt))
			if((weapon->iWeaponType == 8) && (((planepnt->WorldPosition.Y - currinst->Position.Y) * WUTOFT) < weapon->iMaxAlt))
			{
				our_dist = planepnt->WorldPosition - currinst->Position;
				our_distnm = our_dist * WUTONM;
				if(((float)weapon->iRange > our_distnm) && (weapon->fMinRange <= our_distnm))
				{
					wclient = (AAWeaponInstance *)currinst;
					radardat = (GDRadarData *)wclient->AIDataBuf;
					ignoreweap = 0;
					if((radardat->lRFlags1 & GD_RADAR_LOCK) && (!((radardat->lRFlags1 & GD_BURST_MODE) && (radardat->pRBurstTarget == planepnt))))
					{
						ignoreweap = 1;
					}
					if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (radardat->Target == NULL) && (radardat->lWActionTimer < 0) && (ignoreweap == 0))
					{
						if((our_dist < *currdist) || (*currdist < 0))
						{
							foundweapon = wclient;
							*currdist = our_dist;
						}
					}
				}
			}
		}
	}
	else if(currinst->Family == FAMILY_INFOPROVIDER)
	{
		currinfo = (InfoProviderInstance *)currinst;
		radardat = (GDRadarData *)currinfo->AIDataBuf;
		ignoreweap = 0;
		if((radardat->lRFlags1 & GD_RADAR_LOCK) && (!((radardat->lRFlags1 & GD_BURST_MODE) && (radardat->pRBurstTarget == planepnt))))
		{
			ignoreweap = 1;
		}
		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = currinfo->Clients[cnt];

			if(currinst == clients)
			{
				continue;
			}

			if(clients != NULL)
			{
				if(clients->Family == FAMILY_AAWEAPON)
				{
					weapon = GetWeaponPtr(clients);

					if(weapon)
					{
						if((ignoreweap == 0) || ((weapon->iSeekerType != 1) && (weapon->iSeekerType != 7)))
						{
//							if((weapon->iWeaponType == 8) && ((float)weapon->iRange > targdistnm) && (weapon->fMinRange <= targdistnm) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
							if((weapon->iWeaponType == 8) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
							{
								our_dist = planepnt->WorldPosition - clients->Position;
								our_distnm = our_dist * WUTONM;
								if(((float)weapon->iRange > our_distnm) && (weapon->fMinRange <= our_distnm))
								{
									wclient = (AAWeaponInstance *)clients;
									radardat = (GDRadarData *)wclient->AIDataBuf;
									if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (radardat->Target == NULL) && (radardat->lWActionTimer < 0))
									{
										if((our_dist < *currdist) || (*currdist < 0))
										{
											foundweapon = wclient;
											*currdist = our_dist;
										}
									}
								}
							}
						}
					}
				}
				else if(clients->Family == FAMILY_INFOPROVIDER)
				{
					childweapon = GDCheckProviderForFreeSAM(clients, planepnt, targdist, currdist);
					if(childweapon != NULL)
					{
						foundweapon = childweapon;
					}
				}
			}
		}
	}
	return(foundweapon);
}

//***********************************************************************************************************************************
AAWeaponInstance *GDCheckTreeForFreeSAM(InfoProviderInstance *currinfo, BasicInstance *skipinst, PlaneParams *planepnt, float targdist, float *currdist)
{
	BasicInstance *clients;
	DBWeaponType *weapon;
	GDRadarData *radardat;
	int cnt;
	AAWeaponInstance *wclient;
	AAWeaponInstance *foundweapon = NULL;
	AAWeaponInstance *childweapon;
	float targdistnm = targdist * WUTONM;
	double our_dist;
	double our_distnm;
	int ignoreweap;

	if((!currinfo) || (!planepnt))
	{
		return(0);
	}

	radardat = (GDRadarData *)currinfo->AIDataBuf;
	ignoreweap = 0;
	if(radardat->lRFlags1 & GD_RADAR_LOCK)
	{
		ignoreweap = 1;
	}
	for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
	{
		clients = currinfo->Clients[cnt];

		if((BasicInstance *)currinfo == clients)
		{
			continue;
		}

		if(clients != NULL)
		{
			if(clients->Family == FAMILY_AAWEAPON)
			{
				weapon = GetWeaponPtr(clients);

				if(weapon)
				{
					if(ignoreweap == 0)
					{
//						if((weapon->iWeaponType == 8) && ((float)weapon->iRange > targdistnm) && (weapon->fMinRange <= targdistnm) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
						if((weapon->iWeaponType == 8)  && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
						{
						 	our_dist = planepnt->WorldPosition - clients->Position;
							our_distnm = our_dist * WUTONM;
							if(((float)weapon->iRange > our_distnm) && (weapon->fMinRange <= our_distnm))
							{
								wclient = (AAWeaponInstance *)clients;
								radardat = (GDRadarData *)wclient->AIDataBuf;
								if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (radardat->Target == NULL) && (radardat->lWActionTimer < 0))
								{
									if((our_dist < *currdist) || (*currdist < 0))
									{
										foundweapon = wclient;
										*currdist = our_dist;
									}
								}
							}
						}
					}
				}
			}
			else if((clients->Family == FAMILY_INFOPROVIDER) && (clients != skipinst))
			{
				childweapon = GDCheckProviderForFreeSAM(clients, planepnt, targdist, currdist);
				if(childweapon != NULL)
				{
					foundweapon = childweapon;
				}
			}
		}
	}
	return(foundweapon);
}

//***********************************************************************************************************************************
AAWeaponInstance *GDCheckProviderForFreeAAA(BasicInstance *currinst, PlaneParams *planepnt, float targdist, float *currdist)
{
	InfoProviderInstance *currinfo;
	BasicInstance *clients;
	DBWeaponType *weapon;
	GDRadarData *radardat;
	int cnt;
	AAWeaponInstance *wclient;
	AAWeaponInstance *foundweapon = NULL;
	AAWeaponInstance *childweapon;
	float targdistnm = targdist * WUTONM;
	double our_dist;
	double our_distnm;
	int ignoreweap = 0;
	int willfire = 0;

	if((!currinst) || (!planepnt))
	{
		return(0);
	}

	if(currinst->Family == FAMILY_AAWEAPON)
	{
		weapon = GetWeaponPtr(currinst);
		if(weapon)
		{
//			if((weapon->iWeaponType == 6) && ((float)(weapon->iRange<<2) > targdistnm) && (weapon->fMinRange <= targdistnm) && (((planepnt->WorldPosition.Y - currinst->Position.Y) * WUTOFT) < weapon->iMaxAlt))
			if((weapon->iWeaponType == 6) && (((planepnt->WorldPosition.Y - currinst->Position.Y) * WUTOFT) < weapon->iMaxAlt))
			{
				our_dist = planepnt->WorldPosition - currinst->Position;
				our_distnm = our_dist * WUTONM;
				if(((float)(weapon->iRange<<2) > our_distnm) && (weapon->fMinRange <= our_distnm))
				{
					wclient = (AAWeaponInstance *)currinst;
					radardat = (GDRadarData *)wclient->AIDataBuf;
					ignoreweap = 0;
					if((radardat->lWFlags1 & GD_W_FIRING) && (!(radardat->lWFlags1 & (GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET))))
					{
						ignoreweap = 1;
					}
					if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (ignoreweap == 0))
					{
#ifdef AAALONGTEST
	//					if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) > targdistnm) && (weapon->iSeekerType != 10) && (weapon->iSeekerType != IGNORETYPE))
						if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) > our_distnm) && (weapon->iSeekerType != 10) && (weapon->iSeekerType != IGNORETYPE))
#else
	//					if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) >= targdistnm) && (weapon->iSeekerType != 10))
						if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) >= our_distnm) && (weapon->iSeekerType != 10))
#endif
						{
							foundweapon = wclient;
							*currdist = our_dist;
						}
#ifndef AAALONGTEST
						if(!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))
						{
							radardat->lWFlags1 |= GD_W_PREP_RANDOM;
							if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
							{
								radardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
							}
							if(!iAAAFireAlreadyChecked)
								GDNoticeAAAFire(&wclient->Basics.Position, GDConvertGrndSide((BasicInstance *)wclient));
						}
	//					if(((float)weapon->iRange >= targdistnm) && (weapon->iSeekerType == 10))
						if(((float)weapon->iRange >= our_distnm) && (weapon->iSeekerType == 10))
						{
							if((rand() & 4) == 1)
							{
								radardat->Target = planepnt;
							}
						}
#endif
					}
				}
			}
		}
	}
	else if(currinst->Family == FAMILY_INFOPROVIDER)
	{
		currinfo = (InfoProviderInstance *)currinst;
		radardat = (GDRadarData *)currinfo->AIDataBuf;
		ignoreweap = 0;
		if((radardat->lRFlags1 & GD_RADAR_LOCK) && (!((radardat->lRFlags1 & GD_BURST_MODE) && (radardat->pRBurstTarget == planepnt))))
		{
			ignoreweap = 1;
		}
		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = currinfo->Clients[cnt];

			if(currinst == clients)
			{
				continue;
			}

			if(clients != NULL)
			{
				if(clients->Family == FAMILY_AAWEAPON)
				{
					weapon = GetWeaponPtr(clients);

					if(weapon)
					{
						if((ignoreweap == 0) && ((!(radardat->lWFlags1 & GD_W_FIRING)) || (radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET))))
						{
//							if((weapon->iWeaponType == 6) && ((float)(weapon->iRange<<2) > targdistnm) && (weapon->fMinRange <= targdistnm) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
							if((weapon->iWeaponType == 6) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
							{
								our_dist = planepnt->WorldPosition - clients->Position;
								our_distnm = our_dist * WUTONM;
								if(((float)(weapon->iRange<<2) > our_distnm) && (weapon->fMinRange <= our_distnm))
								{
									wclient = (AAWeaponInstance *)clients;
									radardat = (GDRadarData *)wclient->AIDataBuf;
									if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (radardat->Target == NULL))
									{
#ifdef AAALONGTEST
//										if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) > targdistnm) && (weapon->iSeekerType != 10) && (weapon->iSeekerType != IGNORETYPE))
										if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) > our_distnm) && (weapon->iSeekerType != 10) && (weapon->iSeekerType != IGNORETYPE))
#else
//										if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) >= targdistnm) && (weapon->iSeekerType != 10))
										if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) >= our_distnm) && (weapon->iSeekerType != 10))
#endif
										{
											foundweapon = wclient;
											*currdist = our_dist;
										}
#ifndef AAALONGTEST
										if(!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))
										{
											radardat->lWFlags1 |= GD_W_PREP_RANDOM;
											if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
											{
												radardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
											}
											if(!iAAAFireAlreadyChecked)
												GDNoticeAAAFire(&wclient->Basics.Position, GDConvertGrndSide((BasicInstance *)wclient));
										}
//										if(((float)weapon->iRange >= targdistnm) && (weapon->iSeekerType == 10))
										if(((float)weapon->iRange >= our_distnm) && (weapon->iSeekerType == 10))
										{
											if((rand() & 4) == 1)
											{
												radardat->Target = planepnt;
											}
										}
#endif
									}
								}
							}
						}
					}
				}
				else if(clients->Family == FAMILY_INFOPROVIDER)
				{
					childweapon = GDCheckProviderForFreeAAA(clients, planepnt, targdist, currdist);
					if(childweapon != NULL)
					{
						foundweapon = childweapon;
					}
				}
			}
		}
	}
	return(foundweapon);
}

//***********************************************************************************************************************************
AAWeaponInstance *GDCheckTreeForFreeAAA(InfoProviderInstance *currinfo, BasicInstance *skipinst, PlaneParams *planepnt, float targdist, float *currdist)
{
	BasicInstance *clients;
	DBWeaponType *weapon;
	GDRadarData *radardat;
	int cnt;
	AAWeaponInstance *wclient;
	AAWeaponInstance *foundweapon = NULL;
	AAWeaponInstance *childweapon;
	float targdistnm = targdist * WUTONM;
	double our_dist;
	double our_distnm;
	int ignoreweap;

	if((!currinfo) || (!planepnt))
	{
		return(0);
	}

	radardat = (GDRadarData *)currinfo->AIDataBuf;
	ignoreweap = 0;
	if(radardat->lRFlags1 & GD_RADAR_LOCK)
	{
		ignoreweap = 1;
	}
	for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
	{
		clients = currinfo->Clients[cnt];

		if((BasicInstance *)currinfo == clients)
		{
			continue;
		}

		if(clients != NULL)
		{
			if(clients->Family == FAMILY_AAWEAPON)
			{
				weapon = GetWeaponPtr(clients);

				if(weapon)
				{
					if((ignoreweap == 0) && ((!(radardat->lWFlags1 & GD_W_FIRING)) || (radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET))))
					{
//						if((weapon->iWeaponType == 6) && ((float)(weapon->iRange<<2) > targdistnm) && (weapon->fMinRange <= targdistnm) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
						if((weapon->iWeaponType == 6) && (((planepnt->WorldPosition.Y - clients->Position.Y) * WUTOFT) < weapon->iMaxAlt))
						{
							our_dist = planepnt->WorldPosition - clients->Position;
							our_distnm = our_dist * NMTOWU;
							if(((float)(weapon->iRange<<2) > our_distnm) && (weapon->fMinRange <= our_distnm))
							{
								wclient = (AAWeaponInstance *)clients;
								radardat = (GDRadarData *)wclient->AIDataBuf;
								if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (radardat->Target == NULL) && (radardat->lWActionTimer < 0))
								{
#ifdef AAALONGTEST
//									if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) > targdistnm) && (weapon->iSeekerType != 10) && (weapon->iSeekerType != IGNORETYPE))
									if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) > our_distnm) && (weapon->iSeekerType != 10) && (weapon->iSeekerType != IGNORETYPE))
#else
//									if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) >= targdistnm) && (weapon->iSeekerType != 10))
									if(((our_dist < *currdist) || (*currdist < 0)) && ((float)(weapon->iRange<<1) >= our_distnm) && (weapon->iSeekerType != 10))
#endif
									{
										foundweapon = wclient;
										*currdist = our_dist;
									}
#ifndef AAALONGTEST
									if(!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))
									{
										radardat->lWFlags1 |= GD_W_PREP_RANDOM;
										if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
										{
											radardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
										}
										if(!iAAAFireAlreadyChecked)
											GDNoticeAAAFire(&wclient->Basics.Position, GDConvertGrndSide((BasicInstance *)wclient));
									}
//									if(((float)weapon->iRange >= targdistnm) && (weapon->iSeekerType == 10))
									if(((float)weapon->iRange >= our_distnm) && (weapon->iSeekerType == 10))
									{
										if((rand() & 4) == 1)
										{
											radardat->Target = planepnt;
										}
									}
#endif
								}
							}
						}
					}
				}
			}
			else if((clients->Family == FAMILY_INFOPROVIDER) && (clients != skipinst))
			{
				childweapon = GDCheckProviderForFreeAAA(clients, planepnt, targdist, currdist);
				if(childweapon != NULL)
				{
					foundweapon = childweapon;
				}
			}
		}
	}
	return(foundweapon);
}

//***********************************************************************************************************************************
int GDAParentOfB(BasicInstance *radarsite, BasicInstance *cradarsite)
{
	InfoProviderInstance *treetop;
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;

	if(radarsite == cradarsite)
	{
		return(0);
	}

	if((!radarsite) || (!cradarsite))
	{
		return(0);
	}

	if(radarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)radarsite;
		treetop = tweapon->OurProvider;
	}
	else
	{
		tprovider = (InfoProviderInstance *)radarsite;
		treetop = tprovider->OurProvider;
	}

	while(treetop)
	{
		if(radarsite == (BasicInstance *)treetop)
		{
			return(1);
		}
		treetop = treetop->OurProvider;
	}

	return(0);
}

//***********************************************************************************************************************************
void GDCheckForClients(BasicInstance *walker)
{
	int cnt;
	BasicInstance *clients;
	AAWeaponInstance *wclient;
	InfoProviderInstance *iclient;
	InfoProviderInstance *currinfo;
	InfoProviderInstance *ourprovider = NULL;
	DBRadarType *radar;
	int tempval;
	GDRadarData *radardat;

	if(!walker)
	{
		return;
	}

	if(JSTARList.iNumJSTARObjects)
	{
		for(cnt = 0; cnt < JSTARList.iNumJSTARObjects; cnt ++)
		{
			if(JSTARList.JSTARObjectList[cnt].dwSerialNumber == walker->SerialNumber)
			{
				lJSTARSDead |= 1<<cnt;
			}
		}
	}

	if(walker->SerialNumber == FACObject.dwSerialNumber)
	{
		if(iFACState)
		{
			AIC_FAC_Dead_Msg(PlayerPlane - Planes);
		}
		iFACState = 0;
		iFACTargetPos = -1;
		lFACTimer = -1;
		fpFACPosition.SetValues(-1.0f, -1.0f, -1.0f);
		FACObject.iType = -1;
	}

	if(walker->Family == FAMILY_INFOPROVIDER)
	{
		currinfo = (InfoProviderInstance *)walker;

		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = currinfo->Clients[cnt];

			if(walker == clients)
			{
				continue;
			}

			if(clients != NULL)
			{
				if(clients->Family == FAMILY_AAWEAPON)
				{
					radar = GetRadarPtr((BasicInstance *)clients);
					wclient = (AAWeaponInstance *)clients;

					if(radar)
					{
						radardat = (GDRadarData *)wclient->AIDataBuf;
						if(!(radardat->lRFlags1 & (GD_RADAR_ON)))
						{
							tempval = rand() & 127;
							if(tempval > 90)
							{
								radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
								radardat->lRGenericTimer = 5 * 60000 + ((rand() & 63) * 1000);
								radardat->lRActionTimer = (rand() & 15) * 4000;
							}
							else if(tempval < 60)
							{
								radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
							}
						}
					}
					wclient->OurProvider = NULL;
				}
				else if(clients->Family == FAMILY_INFOPROVIDER)
				{
					radar = GetRadarPtr((BasicInstance *)clients);
					iclient = (InfoProviderInstance *)clients;

					if(radar)
					{
						radardat = (GDRadarData *)iclient->AIDataBuf;
						if(!(radardat->lRFlags1 & (GD_RADAR_ON)))
						{
							tempval = rand() & 127;
							if(tempval > 90)
							{
								radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
							}
							else if(tempval < 60)
							{
								radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
							}
						}
					}
					iclient->OurProvider = NULL;
				}
				currinfo->Clients[cnt] = NULL;
			}
		}
		radardat = (GDRadarData *)currinfo->AIDataBuf;
		radardat->lRFlags1 = GD_I_AM_DEAD;
	    radardat->Target = NULL;
		ourprovider = currinfo->OurProvider;
		currinfo->OurProvider = NULL;
	}
	else if(walker->Family == FAMILY_AAWEAPON)
	{
		wclient = (AAWeaponInstance *)walker;
		radardat = (GDRadarData *)wclient->AIDataBuf;
		radardat->lRFlags1 = GD_I_AM_DEAD;
	    radardat->Target = NULL;
		radardat->lWFlags1 = 0;
		ourprovider = wclient->OurProvider;
		wclient->OurProvider = NULL;
	}

	if(ourprovider)
	{
		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = ourprovider->Clients[cnt];
			if(clients == walker)
			{
				ourprovider->Clients[cnt] = NULL;
			}
		}
	}
}

//***********************************************************************************************************************************
int GDSameAncestor(BasicInstance *radarsite, BasicInstance *cradarsite)
{
	InfoProviderInstance *treetop;
	InfoProviderInstance *treetop2;
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;

	if(radarsite == NULL)
	{
		return(0);
	}

	if(cradarsite == NULL)
	{
		return(0);
	}

	if(radarsite == cradarsite)
	{
		return(1);
	}

	if(radarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)radarsite;
		treetop = tweapon->OurProvider;
		if(treetop == NULL)
		{
			return(0);
		}
	}
	else
	{
		tprovider = (InfoProviderInstance *)radarsite;
		treetop = tprovider;
	}

	if(treetop)
	{
		while(treetop->OurProvider)
		{
			treetop = treetop->OurProvider;
		}
	}

	if(cradarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)cradarsite;
		treetop2 = tweapon->OurProvider;
		if(treetop2 == NULL)
		{
			return(0);
		}
	}
	else
	{
		tprovider = (InfoProviderInstance *)cradarsite;
		treetop2 = tprovider;
	}

	if(treetop2)
	{
		while(treetop2->OurProvider)
		{
			treetop2 = treetop2->OurProvider;
		}
	}

	if(treetop2 == treetop)
	{
		return(1);
	}

	return(0);
}

//************************************************************************************************
void GDClearWeaponLauncher(WeaponParams *skipweapon)
{
	WeaponParams *W;
	BasicInstance *launcher;
	AAWeaponInstance *tweapon;
	MovingVehicleParams *vehiclepnt;
	GDRadarData *radardat;

	if(skipweapon->LauncherType == MOVINGVEHICLE)
	{
		if(skipweapon->LaunchStation != -1)
		{
			vehiclepnt = (MovingVehicleParams *)skipweapon->Launcher;
			radardat = &vehiclepnt->RadarWeaponData[skipweapon->LaunchStation];
		}
		if(radardat->lWFlags1 & GD_W_ACQUIRING)
		{
			return;
		}
		if(radardat->Target == radardat->Target)
		{
			radardat->Target = NULL;
		}
		return;
	}

	if(skipweapon->LauncherType != GROUNDOBJECT)
	{
		return;
	}


	W = &Weapons[0];
	launcher = (BasicInstance *)skipweapon->Launcher;

	if(launcher->Family != FAMILY_AAWEAPON)
	{
		return;
	}

	while(W <= LastWeapon)
	{
		if((W != skipweapon) && (W->LauncherType == GROUNDOBJECT))
		{
			if(W->Launcher == launcher)
			{
				return;
			}
		}
		W++;
	}
	tweapon = (AAWeaponInstance *)launcher;
	radardat = (GDRadarData *)tweapon->AIDataBuf;

	if(radardat->lWFlags1 & GD_W_ACQUIRING)
	{
		return;
	}
	radardat->Target = NULL;
	return;
}

//************************************************************************************************
int AIRadarOnCheck(BasicInstance *checkobject)
{
	GDRadarData *radardat;

	radardat = GDGetRadarData(checkobject);
	if(radardat == NULL)
	{
		return(0);
	}
	else if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
	{
		return(1);
	}

	return(0);
}

//************************************************************************************************
GDRadarData *GDGetRadarData(BasicInstance *checkobject)
{
	AAWeaponInstance *weapon;
	InfoProviderInstance *infoprov;

	if(checkobject->Family == FAMILY_AAWEAPON)
	{
		weapon = (AAWeaponInstance *)checkobject;
		return((GDRadarData *)weapon->AIDataBuf);
	}
	else if(checkobject->Family == FAMILY_INFOPROVIDER)
	{
		infoprov = (InfoProviderInstance *)checkobject;
		return((GDRadarData *)infoprov->AIDataBuf);
	}
	return(NULL);
}

//**************************************************************************************
void GDNoticeExplosion(WeaponParams *W)
{
	InfoProviderInstance *currinfo;
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	float dx, dz;
	float checkrange = 10 * NMTOWU;
	FPointDouble *position;
	DBWeaponType *weapon;
	int tempval;
	int weaponside, grndside;
	MovingVehicleParams *vehiclepnt;

	position = &W->Pos;

	currinfo = AllInfoProviders;

	weaponside = AI_FRIENDLY;
	if(W->LauncherType == AIRCRAFT)
	{
		if(!W->P)
			return;

		weaponside = W->P->AI.iSide;
	}
	else if(W->LauncherType == GROUNDOBJECT)
	{
		if(!W->Launcher)
			return;

		weaponside = GDConvertGrndSide((BasicInstance *)W->Launcher);
	}
	else if(W->LauncherType == GROUNDOBJECT)
	{
		if(!W->Launcher)
			return;

		weaponside = GDConvertGrndSide((BasicInstance *)W->Launcher);
	}

	while(currinfo)
	{

		radardat = (GDRadarData *)currinfo->AIDataBuf;
		if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
		{
			if(GetRadarPtr((BasicInstance *)currinfo))
			{
				dx = currinfo->Basics.Position.X - position->X;
				dz = currinfo->Basics.Position.Z - position->Z;
				if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
				{
					grndside = GDConvertGrndSide((BasicInstance *)currinfo);
					if((iAI_ROE[grndside] != 2) && (grndside != AI_NEUTRAL))
					{
						if(grndside != weaponside)
						{
							iAI_ROE[grndside] = 2;
						}
					}

					if(!(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
					{
						tempval = rand() & 127;
						if(tempval > 50)
						{
							radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
							radardat->lRGenericTimer = 5 * 60000 + ((rand() & 63) * 1000);
							radardat->lRActionTimer = (rand() & 15) * 4000;
						}
						else if(tempval < 10)
						{
							radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
						}
						if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
						{
							radardat->lRFlags1 |= (GD_RADAR_TEMP_ON);
							radardat->lRFlags1 &= ~(GD_RADAR_TEMP_OFF);
							radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
							radardat->lRFlags1 |= GD_HIGH_ALERT;
							radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
						}
					}
					else if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
		}
		currinfo = (InfoProviderInstance *)currinfo->Basics.NextRelatedInstance;
	}

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		radardat = (GDRadarData *)currweapon->AIDataBuf;
		if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
		{
			if(GetRadarPtr((BasicInstance *)currweapon))
			{
				dx = currweapon->Basics.Position.X - position->X;
				dz = currweapon->Basics.Position.Z - position->Z;
				if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
				{
					if(!(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
					{
						tempval = rand() & 127;
						if(tempval > 50)
						{
							radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
							radardat->lRGenericTimer = 5 * 60000 + ((rand() & 63) * 1000);
							radardat->lRActionTimer = (rand() & 15) * 4000;
						}
						else if(tempval < 10)
						{
							radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
						}
						if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
						{
							radardat->lRFlags1 |= (GD_RADAR_TEMP_ON);
							radardat->lRFlags1 &= ~(GD_RADAR_TEMP_OFF);
							radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
							radardat->lRFlags1 |= GD_HIGH_ALERT;
							radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
						}
					}
					else if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
			weapon = GetWeaponPtr((BasicInstance *)currweapon);
			if(weapon)
			{
				if(weapon->iWeaponType == 6)  //  Gun
				{
					dx = currweapon->Basics.Position.X - position->X;
					dz = currweapon->Basics.Position.Z - position->Z;
					if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
					{
						grndside = GDConvertGrndSide((BasicInstance *)currweapon);

						if((iAI_ROE[grndside] != 2) && (grndside != AI_NEUTRAL))
						{
							if(grndside != weaponside)
							{
								iAI_ROE[grndside] = 2;
							}
						}

						if((!(radardat->lWFlags1 & (GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM))) && (grndside != weaponside))
						{
							radardat->lWFlags1 |= GD_W_PREP_RANDOM;
							if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
							{
//								radardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
								radardat->lWActionTimer = ((rand() & 0x7FFF) * 4);  //  128 seconds
							}
						}
						//  will want to have guns fire into the air, even if no target.
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (vehiclepnt->Status & (VL_FIRE_WEAPONS)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
		{
			weapon=NULL;
			if (!vehiclepnt->iShipType)	// SCOTT FIX
				weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);

			if (!weapon) continue;

			radardat = vehiclepnt->RadarWeaponData;
			if(weapon->iWeaponType == 6)  //  Gun
			{
				dx = vehiclepnt->WorldPosition.X - position->X;
				dz = vehiclepnt->WorldPosition.Z - position->Z;
				if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
				{
					grndside = vehiclepnt->iSide;

					if((iAI_ROE[grndside] != 2) && (grndside != AI_NEUTRAL))
					{
						if(grndside != weaponside)
						{
							iAI_ROE[grndside] = 2;
						}
					}

#if 0  //  I don't think I want random fire from vehicles.
					if((!(radardat->lWFlags1 & (GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM))) && (grndside != weaponside))
					{
						radardat->lWFlags1 |= GD_W_PREP_RANDOM;
						if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
						{
//								radardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
							radardat->lWActionTimer = ((rand() & 0x7FFF) * 4);  //  128 seconds
						}
					}
#endif
					//  will want to have guns fire into the air, even if no target.
					radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
					radardat->lRFlags1 |= GD_HIGH_ALERT;
					radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
				}
			}
		}
	}
}

//**************************************************************************************
void GDNoticeAAAFire(FPointDouble *position, int side)
{
	InfoProviderInstance *currinfo;
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	float dx, dz;
	float checkrange = 10 * NMTOWU;
	DBWeaponType *weapon;
	int tempval;
	MovingVehicleParams *vehiclepnt;

//	iAAAFireAlreadyChecked = 1;

	currinfo = AllInfoProviders;

	while(currinfo)
	{

		radardat = (GDRadarData *)currinfo->AIDataBuf;
		if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
		{
			if(GetRadarPtr((BasicInstance *)currinfo))
			{
				dx = currinfo->Basics.Position.X - position->X;
				dz = currinfo->Basics.Position.Z - position->Z;
				if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
				{
					if(!(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
					{
						tempval = rand() & 127;
						if(tempval > 50)
						{
							radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
							radardat->lRGenericTimer = 5 * 60000 + ((rand() & 63) * 1000);
							radardat->lRActionTimer = (rand() & 15) * 4000;
						}
						else if(tempval < 10)
						{
							radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
						}
						if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
						{
							radardat->lRFlags1 |= (GD_RADAR_TEMP_ON);
							radardat->lRFlags1 &= ~(GD_RADAR_TEMP_OFF);
							radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
							radardat->lRFlags1 |= GD_HIGH_ALERT;
							radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
						}
					}
					else if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
		}
		currinfo = (InfoProviderInstance *)currinfo->Basics.NextRelatedInstance;
	}

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		radardat = (GDRadarData *)currweapon->AIDataBuf;
		if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
		{
			if(GetRadarPtr((BasicInstance *)currweapon))
			{
				dx = currweapon->Basics.Position.X - position->X;
				dz = currweapon->Basics.Position.Z - position->Z;
				if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
				{
					if(!(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
					{
						tempval = rand() & 127;
						if(tempval > 50)
						{
							radardat->lRFlags1 |= GD_RADAR_TEMP_ON;
							radardat->lRGenericTimer = 5 * 60000 + ((rand() & 63) * 1000);
							radardat->lRActionTimer = (rand() & 15) * 4000;
						}
						else if(tempval < 10)
						{
							radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
						}
						if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
						{
							radardat->lRFlags1 |= (GD_RADAR_TEMP_ON);
							radardat->lRFlags1 &= ~(GD_RADAR_TEMP_OFF);
							radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
							radardat->lRFlags1 |= GD_HIGH_ALERT;
							radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
						}
					}
					else if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
			weapon = GetWeaponPtr((BasicInstance *)currweapon);
			if(weapon)
			{
				if((weapon->iWeaponType == 6) && (GDConvertGrndSide((BasicInstance *)currweapon) == side))  //  Gun
				{
					dx = currweapon->Basics.Position.X - position->X;
					dz = currweapon->Basics.Position.Z - position->Z;
					if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
					{
						if(!(radardat->lWFlags1 & (GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))
						{
							radardat->lWFlags1 |= GD_W_PREP_RANDOM;
							if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
							{
								radardat->lWActionTimer = ((rand() & 0x7FFF) * 4) + 15000;  //  128 seconds + 15secs
							}
						}
						//  will want to have guns fire into the air, even if no target.
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (vehiclepnt->Status & (VL_FIRE_WEAPONS)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
		{

			if (vehiclepnt->iShipType) continue; // SCOTT FIX
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
			radardat = vehiclepnt->RadarWeaponData;
			if((weapon->iWeaponType == 6) && (vehiclepnt->iSide == side))  //  Gun
			{
				dx = vehiclepnt->WorldPosition.X - position->X;
				dz = vehiclepnt->WorldPosition.Z - position->Z;
				if((fabs(dx) < checkrange) && (fabs(dz) < checkrange))
				{
#if 0  //  I don't think I want random fire from vehicles.
					if(!(radardat->lWFlags1 & (GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))					{
						radardat->lWFlags1 |= GD_W_PREP_RANDOM;
						if((radardat->lWActionTimer < 0) && (radardat->lWReloadTimer < 0))
						{
//								radardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
							radardat->lWActionTimer = ((rand() & 0x7FFF) * 4);  //  128 seconds
						}
					}
#endif
					//  will want to have guns fire into the air, even if no target.
					radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
					radardat->lRFlags1 |= GD_HIGH_ALERT;
					radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
				}
			}
		}
	}
}

//**************************************************************************************
InfoProviderInstance *GDGetInfoLeader(BasicInstance *radarsite)
{
	InfoProviderInstance *treetop;
	InfoProviderInstance *tprovider;
	AAWeaponInstance *tweapon;

	if(radarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)radarsite;
		treetop = tweapon->OurProvider;
		if(treetop == NULL)
		{
			return(NULL);
		}
	}
	else
	{
		tprovider = (InfoProviderInstance *)radarsite;
		treetop = tprovider;
	}

	if(treetop)
	{
		while(treetop->OurProvider)
		{
			treetop = treetop->OurProvider;
		}
	}
	return(treetop);
}

//**************************************************************************************
void GDUpgradeTreeAlert(InfoProviderInstance *currinfo, BasicInstance *skipinst, FPoint position)
{
	BasicInstance *clients;
	int cnt;
	float dx, dz;
	float checkrange = 10 * NMTOWU;
	GDRadarData *radardat;

	if(currinfo == NULL)
	{
		return;
	}

	radardat = (GDRadarData *)currinfo->AIDataBuf;

	dx = currinfo->Basics.Position.X - position.X;
	dz = currinfo->Basics.Position.Z - position.Z;
	if((fabs(dx) < checkrange) && (fabs(dx) < checkrange))
	{
		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_HIGH_ALERT;
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}

	for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
	{
		clients = currinfo->Clients[cnt];

		if((BasicInstance *)currinfo == clients)
		{
			continue;
		}

		if(clients != NULL)
		{
			if(clients->Family == FAMILY_AAWEAPON)
			{
				radardat = GDGetRadarData(clients);
				if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
				{
					dx = clients->Position.X - position.X;
					dz = clients->Position.Z - position.Z;
					if((fabs(dx) < checkrange) && (fabs(dx) < checkrange))
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
						radardat->lRFlags1 |= GD_HIGH_ALERT;
						radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
					}
				}
			}
			else if((clients->Family == FAMILY_INFOPROVIDER) && (clients != skipinst))
			{
				radardat = GDGetRadarData(clients);
				if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
				{
					GDUpgradeTreeAlert((InfoProviderInstance *)clients, skipinst, position);
				}
			}
		}
	}
}

//************************************************************************************************
int GDCheckForPingReport(void *radarsite, PlaneParams *planepnt, PlaneParams *pingplane, int sitetype)
{
	if(planepnt == NULL)
	{
		return(0);
	}
	else if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return(0);
	}

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return(0);
	}

	if(planepnt == PlayerPlane)
	{
		AIC_WSO_SpikeCall(planepnt, radarsite, pingplane, sitetype);
		return(1);
	}
	else
	{
		if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
		{
			AIC_Wingman_SpikeCall(planepnt, radarsite, pingplane, sitetype);
			return(1);
		}
		else if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
		{
			AIC_SEAD_SpikeCall(planepnt, radarsite, pingplane, sitetype);
			return(1);
		}
	}
	return(0);
}


//**************************************************************************************
int GDNumLaunchingAtPlane(PlaneParams *planepnt)
{
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	int numlaunching = 0;
	InfoProviderInstance *currinfo;
	MovingVehicleParams *vehiclepnt;
	int cnt;

	currinfo = AllInfoProviders;

	while(currinfo)
	{
		radardat = (GDRadarData *)currinfo->AIDataBuf;
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF)))
		{
			if(radardat->lRBurstTimer >= 0)
			{
				if(planepnt == (PlaneParams *)radardat->pRBurstTarget)
				{
					numlaunching += radardat->iRBurstCounter;
				}
			}
		}
		currinfo = (InfoProviderInstance *)currinfo->Basics.NextRelatedInstance;
	}

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		radardat = (GDRadarData *)currweapon->AIDataBuf;
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_FIRING)))
		{
			if(radardat->lWFlags1 & (GD_W_ACQUIRING))
			{
				if(planepnt == (PlaneParams *)radardat->Target)
				{
					numlaunching ++;
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (vehiclepnt->Status & (VL_FIRE_WEAPONS)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
		{

			for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
			{
				radardat = &vehiclepnt->RadarWeaponData[cnt];
				if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_FIRING)))
				{
					if(radardat->lWFlags1 & (GD_W_ACQUIRING))
					{
						if(planepnt == (PlaneParams *)radardat->Target)
						{
							numlaunching ++;
						}
					}
				}
			}
		}
	}

	return(numlaunching);
}

//**************************************************************************************
int GDNumAAAFiringAtPlane(PlaneParams *planepnt)
{
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	int numlaunching = 0;
	InfoProviderInstance *currinfo;

	currinfo = AllInfoProviders;

#if 0
	while(currinfo)
	{
		radardat = (GDRadarData *)currinfo->AIDataBuf;
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF)))
		{
			if(radardat->lRBurstTimer >= 0)
			{
				if(planepnt == (PlaneParams *)radardat->pRBurstTarget)
				{
					numlaunching += radardat->iRBurstCounter;
				}
			}
		}
		currinfo = (InfoProviderInstance *)currinfo->Basics.NextRelatedInstance;
	}
#endif

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		radardat = (GDRadarData *)currweapon->AIDataBuf;
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_FIRING)))
		{
//			if(radardat->lWFlags1 & (GD_W_SINGLE|GD_W_BURST|GD_W_STREAM))
			if(radardat->lWFlags1 & (GD_W_BURST|GD_W_STREAM))
			{
				if(planepnt == (PlaneParams *)radardat->Target)
				{
					numlaunching ++;
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}
	return(numlaunching);
}

//************************************************************************************************
float GDModifyNoticePercent(void *radarsite, GDRadarData *radardat, PlaneParams *checkplane, int sitetype)
{
	float modperc = 1.0f;

	if(radardat->lRFlags1 & GD_HIGH_ALERT)
	{
		modperc *= 1.0f;
	}
	else if(radardat->lRFlags1 & GD_MED_ALERT)
	{
		modperc *= 0.75f;
	}
	else
	{
		modperc *= 0.5f;
	}

	modperc *= 1.0f - (radardat->iSkill * 0.10);

	if(checkplane->AI.iAIFlags1 & AIJAMMINGON)
	{
		modperc *= 1.25f;
	}

	return(modperc);
}

//************************************************************************************************
int GDConvertGrndSide(BasicInstance *walker)
{
	if((MultiPlayer) && (g_nMissionType == SIM_MULTI))
	{
		double basex;
		double basez;
		double dworkval;
		for(int cnt = 0; cnt < 8; cnt ++)
		{
			switch(cnt)
			{
				case 0:
					basex = ConvertWayLoc(5574795);
					basez = ConvertWayLoc(1695901);
					break;
				case 1:
					basex = ConvertWayLoc(5868294);
					basez = ConvertWayLoc(1774282);
					break;
				case 2:
					basex = ConvertWayLoc(6278038);
					basez = ConvertWayLoc(1608187);
					break;
				case 3:
					basex = ConvertWayLoc(6362861);
					basez = ConvertWayLoc(1228992);
					break;
				case 4:
					basex = ConvertWayLoc(6255687);
					basez = ConvertWayLoc(916899);
					break;
				case 5:
					basex = ConvertWayLoc(5594489);
					basez = ConvertWayLoc(813408);
					break;
				case 6:
					basex = ConvertWayLoc(5324754);
					basez = ConvertWayLoc(1135883);
					break;
				case 7:
					basex = ConvertWayLoc(5293757);
					basez = ConvertWayLoc(1398543);
					break;
				default:
					return((walker->Flags & (BI_FRIENDLY|BI_NEUTRAL)) >> 21);
					break;
			}
			dworkval = fabs(basex - walker->Position.X);
			if(dworkval < (20 * NMTOWU))
			{
				dworkval = fabs(basez - walker->Position.Z);
				if(dworkval < (20 * NMTOWU))
				{
					return(cnt);
				}
			}
		}
	}

	return((walker->Flags & (BI_FRIENDLY|BI_NEUTRAL)) >> 21);
}

//************************************************************************************************
int GDConvertRunwaySide(RunwayInfo *walker)
{
	if((MultiPlayer) && (g_nMissionType == SIM_MULTI))
	{
		double basex;
		double basez;
		double dworkval;
		for(int cnt = 0; cnt < 8; cnt ++)
		{
			switch(cnt)
			{
				case 0:
					basex = ConvertWayLoc(5574795);
					basez = ConvertWayLoc(1695901);
					break;
				case 1:
					basex = ConvertWayLoc(5868294);
					basez = ConvertWayLoc(1774282);
					break;
				case 2:
					basex = ConvertWayLoc(6278038);
					basez = ConvertWayLoc(1608187);
					break;
				case 3:
					basex = ConvertWayLoc(6362861);
					basez = ConvertWayLoc(1228992);
					break;
				case 4:
					basex = ConvertWayLoc(6255687);
					basez = ConvertWayLoc(916899);
					break;
				case 5:
					basex = ConvertWayLoc(5594489);
					basez = ConvertWayLoc(813408);
					break;
				case 6:
					basex = ConvertWayLoc(5324754);
					basez = ConvertWayLoc(1135883);
					break;
				case 7:
					basex = ConvertWayLoc(5293757);
					basez = ConvertWayLoc(1398543);
					break;
				default:
					return((walker->Flags & (BI_FRIENDLY|BI_NEUTRAL)) >> 21);
					break;
			}
			dworkval = fabs(basex - walker->Pieces[0]->Basics.Position.X);
			if(dworkval < (20 * NMTOWU))
			{
				dworkval = fabs(basez - walker->Pieces[0]->Basics.Position.Z);
				if(dworkval < (20 * NMTOWU))
				{
					return(cnt);
				}
			}
		}
	}

	return((walker->Flags & (BI_FRIENDLY|BI_NEUTRAL)) >> 21);
}

//************************************************************************************************
float GDGetTimeBulletIntercept(PlaneParams *planepnt, FPointDouble gunposition, float bulletvel)
{
	FPointDouble Delta;
	double ddist;
	float dx, dy, dz, tdist;
	float vel1, vel2, time;
	float offangle;

	Delta.X = (planepnt->WorldPosition.X - gunposition.X);
	Delta.Y = (planepnt->WorldPosition.Y - gunposition.Y);
	Delta.Z = (planepnt->WorldPosition.Z - gunposition.Z);

	ddist = sqrt((Delta.X*Delta.X) + (Delta.Y*Delta.Y) + (Delta.Z*Delta.Z)) * WUTOFT;

	offangle = AIComputeHeadingToPoint(planepnt, gunposition, &tdist, &dx ,&dy, &dz, 1);

	vel1 = cos(DegToRad(offangle)) * planepnt->V;

	vel2 = bulletvel;

	time = (ddist) / (vel1 + vel2);

  	return(time);
}

//************************************************************************************************
void GDComputeGunHP(PlaneParams *planepnt, FPointDouble gunposition, FPoint *aimpoint, float bulletvel, float *heading, float *pitch, float *usetime, float timemod, int truepitch)
{
	float disttravel;
	float fworkvar;
	float time, timesqr;
	float dx, dy, dz, offangle, offpitch;
	double ddist;
	ANGLE workpitch;

	if(*usetime < 0)
	{
		time = GDGetTimeBulletIntercept(planepnt, gunposition, bulletvel);
		*usetime = time;
	}
	else
	{
		time = *usetime;
	}
	time += timemod;

	disttravel = planepnt->V * time;

	workpitch = AIGetPlanesFlightPitch(planepnt, 1);

	fworkvar = (sin(DegToRad((double)workpitch / DEGREE)) * disttravel);
	aimpoint->Y = planepnt->WorldPosition.Y + (fworkvar / WUTOFT);

	disttravel = (cos(DegToRad((double)workpitch / DEGREE)) * disttravel);

	fworkvar = (sin(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	aimpoint->X = planepnt->WorldPosition.X - (fworkvar / WUTOFT);

	fworkvar = (cos(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	aimpoint->Z = planepnt->WorldPosition.Z - (fworkvar / WUTOFT);

	dx = aimpoint->X - gunposition.X;
	dy = aimpoint->Y - gunposition.Y;
	dz = aimpoint->Z - gunposition.Z;

	offangle = atan2(-dx, -dz) * 57.2958;

	*heading = offangle;

#if 1
	ddist = sqrt((dx*dx) + (dy*dy) + (dz*dz)) * WUTOFT;


	timesqr = (ddist / bulletvel);
	timesqr = timesqr * timesqr;

	dy *= WUTOFT;
	dy += 0.5 * GRAVITY * timesqr;
#else
	ddist = sqrt((dx*dx) + (dy*dy) + (dz*dz));

	timesqr = (ddist / (bulletvel * FTTOWU));
	timesqr = timesqr * timesqr;

	float dy1, dy2, dyft, ddistft;
	ddistft = ddist * WUTOFT;

	dy1 = 0.5 * GRAVITY * timesqr * WUTOFT;
	dy += 0.5 * GRAVITY * timesqr * FTTOWU;

	dyft = dy * WUTOFT;
	offpitch = atan2(dy, (float)ddist) * 57.2958;

	ddist = sqrt((dx*dx) + (dy*dy) + (dz*dz)) * WUTOFT;

	timesqr = (ddist / bulletvel);
	timesqr = timesqr * timesqr;

	dy *= WUTOFT;
	dy2 = 0.5 * GRAVITY * timesqr;
	dy += 0.5 * GRAVITY * timesqr;
#endif

	offpitch = atan2(dy, (float)ddist) * 57.2958;


//  Removed this for now SRE
//	if((offpitch < 5.0f) && (!truepitch))
//	{
//		offpitch = 5.0f;
//	}

	*pitch = offpitch;
}

FPoint xtemppos;
//************************************************************************************************
void GDFireInitialAAA(void *ogun, PlaneParams *planepnt, int sitetype, int radardatnum)
{
	GDRadarData *radardat;
	float heading, pitch, time;
	DBWeaponType *weapon=NULL;
	float bulletvel;
	float timediff = 2.0f;
	float fworkval, ftempval;
	FPointDouble position;
	AAWeaponInstance *gun = NULL;
	MovingVehicleParams *vgun = NULL;
	FPointDouble weaponoffset;

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	if(sitetype == GROUNDOBJECT)
	{
		gun = (AAWeaponInstance *)ogun;
		weapon = GetWeaponPtr((BasicInstance *)gun);
		radardat = GDGetRadarData((BasicInstance *)gun);
		position = gun->Basics.Position;
	}
	else
	{
		vgun = (MovingVehicleParams *)ogun;
		radardat = &vgun->RadarWeaponData[radardatnum];
		if (!vgun->iShipType) // SCOTT FIX
		{
			if(radardatnum == 0)
				weapon = get_weapon_ptr(pDBVehicleList[vgun->iVDBIndex].lWeaponType);
			else
				weapon = get_weapon_ptr(pDBVehicleList[vgun->iVDBIndex].lWeaponType2);
		}
		else
		{
			weapon = VGDGetShipWeaponForRDNum(vgun, radardatnum);
		}

		VGDGetWeaponPosition(ogun, sitetype, radardatnum, &weaponoffset);

		position.X = vgun->WorldPosition.X + weaponoffset.X;
		position.Y = vgun->WorldPosition.Y + weaponoffset.Y;
		position.Z = vgun->WorldPosition.Z + weaponoffset.Z;
	}

	if (!weapon) return;

	bulletvel = weapon->fMaxSpeed;
	if(bulletvel == 0)
	{
		bulletvel = 3380;
	}

	timediff = (15.0f * (float)weapon->iBurstRate)  / (float)weapon->iRateOfFire;
	if(timediff > 2.0f)
	{
		timediff = 2.0f;
	}

	time = -1.0f;
	GDComputeGunHP(planepnt, position, &radardat->fpAimPoint, bulletvel, &heading, &pitch, &time, timediff);

	radardat->Target = planepnt;
	radardat->fImpactTime = time;
	timediff *= 1000.0f;
	radardat->lTimeDiffOffset = radardat->lTimeDiff = timediff;
	radardat->lWFlags1 |= (GD_W_TIME_DEC|GD_W_PITCH_DEC);
	radardat->lWFlags1 |= GD_W_FIRING;

	fworkval = (time * time) * 0.30;  //  0.25;
	if(fworkval > 3.0)  //  1.25
	{
		fworkval = 3.0;
	}
	ftempval = fworkval;
	fworkval += pitch;

#if 1
	if((!radardat->pWRadarSite) && (sitetype != MOVINGVEHICLE))
	{
		pitch -= ftempval;
		fworkval += ftempval;
	}
#endif

//	if((pitch + fworkval) > 90)
	if(fworkval > 90)
	{
		fworkval = 90;
	}

#if 0
	radardat->PitchOffsetANG = AIConvert180DegreeToAngle(fworkval); //  holds highest pitch gun will spray to.
	radardat->HeadingOffsetANG = AIConvert180DegreeToAngle(pitch);  //  lowest pitch gun will spray to.
	radardat->FirePitchANG = AIConvert180DegreeToAngle(fworkval);   //  start at highest pitch and work our way down.
	radardat->FireHeadingANG = AIConvert180DegreeToAngle(heading);
#else
	radardat->PitchOffsetANG = AIConvert180DegreeToAngle(fworkval); //  holds highest pitch gun will spray to.
	radardat->HeadingOffsetANG = AIConvert180DegreeToAngle(pitch);  //  lowest pitch gun will spray to.
	float midpitch = (fworkval + pitch) / 2.0f;
	radardat->FirePitchANG = AIConvert180DegreeToAngle(midpitch);   //  start at highest pitch and work our way down.
	radardat->FireHeadingANG = AIConvert180DegreeToAngle(heading);
	radardat->lWFlags1 &= ~GD_W_PITCH_DEC;
	radardat->lWFlags1 |= GD_W_PITCH_INC;
#endif


#if 0
	if(weapon->iSeekerType == 8)
	{
		radardat->lWGenericTimer = 13000;
	}
	else if(weapon->iSeekerType == 9)
	{
		radardat->lWGenericTimer = 13000;
	}
	else if(weapon->iSeekerType == 10)
	{
		radardat->lWGenericTimer = 5000;
	}
#endif

//	radardat->lWGenericTimer = 120000;  //  This will be the random fire time
//	radardat->lWGenericTimer = 30000;  // Think this could be causing my group firing.
	if(weapon->iSeekerType == 8)
	{
		radardat->lWGenericTimer = 4000 + (rand() & 0xFFF);
	}
	else
	{
		radardat->lWGenericTimer = 15000 + (rand() & 0x7FFF);
	}

	GDFireAAA(ogun, sitetype, radardatnum);

#if 0
	if(Camera1.CameraMode != CAMERA_FREE)
	{
		Camera1.CameraLocation.X = radardat->fpAimPoint.X;
		Camera1.CameraLocation.Y = radardat->fpAimPoint.Y;
		Camera1.CameraLocation.Z = radardat->fpAimPoint.Z;
		Camera1.AttachedObject = (int *)planepnt;

		Camera1.CameraMode = CAMERA_FREE;  //  TRACKING;
		LoadCockpit(FRONT_NO_COCKPIT);
		SetupView(COCKPIT_FRONT|COCKPIT_NOART);

		CockpitSeat = NO_SEAT;
#if 0
		Camera1.CameraLocation.X = position.X;
		Camera1.CameraLocation.Y = position.Y;
		Camera1.CameraLocation.Z = position.Z;
		Camera1.CameraMode = CAMERA_FREE;
		Camera1.Heading = AIConvert180DegreeToAngle(heading);
		Camera1.Pitch = AIConvert180DegreeToAngle(pitch);
		Camera1.Roll = 0;
#endif
		xtemppos.X = radardat->fpAimPoint.X;
		xtemppos.Y = radardat->fpAimPoint.Y;
		xtemppos.Z = radardat->fpAimPoint.Z;

		camera_setup_initial_location(Camera1.CameraLocation);

		InstantiatePlaceHolder(radardat->fpAimPoint);
//		SimPause ^= 1;
//		if (!SimPause)
//			JustLeftPausedState = 1;
	}
#endif

}

//************************************************************************************************
void GDSetUpRandomAAA(void *ogun, int firsttime, int sitetype, int radardatnum)
{
	GDRadarData *radardat;
	float heading, pitch;
	DBWeaponType *weapon=NULL;
	float fworkval;
	float maxdelta = 6.0f;
	AAWeaponInstance *gun = NULL;
	MovingVehicleParams *vgun = NULL;

	if(sitetype == MOVINGVEHICLE)
	{
		vgun = (MovingVehicleParams *)ogun;
		radardat = &vgun->RadarWeaponData[radardatnum];
		if (!vgun->iShipType) // SCOTT FIX
		{
			if(radardatnum == 0)
				weapon = get_weapon_ptr(pDBVehicleList[vgun->iVDBIndex].lWeaponType);
			else
				weapon = get_weapon_ptr(pDBVehicleList[vgun->iVDBIndex].lWeaponType2);
		}
		else
		{
			weapon = VGDGetShipWeaponForRDNum(vgun, radardatnum);
		}
	}
	else
	{
		gun = (AAWeaponInstance *)ogun;
		weapon = GetWeaponPtr((BasicInstance *)gun);
		radardat = GDGetRadarData((BasicInstance *)gun);
	}

	if (!weapon) return;

	//radardat->pAAAStream = NULL;  //  Do this so we don't get long weird lines when gun quickly turns.

	fworkval = (rand() & 1023);
	fworkval = (fworkval / 1024.0f) * 40.0f;
	pitch = 45 + fworkval;
	radardat->FirePitchANG = AIConvert180DegreeToAngle(pitch);
	fworkval = (rand() & 1023);
	fworkval = ((fworkval / 1024.0f) * (maxdelta * 2.0f)) - maxdelta;
	if(weapon->iSeekerType == 8)
	{
		fworkval /= 5.0;
	}
	radardat->PitchOffsetANG = AIConvert180DegreeToAngle(fworkval);

	fworkval = (rand() & 1023);
	fworkval = (fworkval / 1024.0f) * 360.0f;
	heading = fworkval;
	radardat->FireHeadingANG = AIConvert180DegreeToAngle(heading);
	fworkval = (rand() & 1023);
	fworkval = ((fworkval / 1024.0f) * (maxdelta * 2.0f)) - maxdelta;
	if(weapon->iSeekerType == 8)
	{
		fworkval /= 5.0;
	}
	radardat->HeadingOffsetANG = AIConvert180DegreeToAngle(fworkval);

	radardat->fImpactTime = 0;
	radardat->lTimeDiffOffset = 0;

	if(firsttime)
	{
		radardat->lWFlags1 &= ~GD_W_PREP_RANDOM;
		radardat->lWFlags1 |= (GD_W_FIRING|GD_W_RANDOM_FIRE);
//		radardat->lWGenericTimer = 120000;  //  This will be the random fire time
		radardat->lWGenericTimer = 90000 + (rand() & 0x7FFF);

		GDFireAAA(ogun, sitetype, radardatnum);
	}
}

void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition);
inline BOOL GDInView(AAWeaponInstance *inst)
{
	if ((inst->Basics.Position - Camera1.CameraLocation) < LP_FADEOUT_MAX_DISTANCE)
		return TRUE;
	else
		return FALSE;
}

extern void	GunSounds(DBWeaponType *weapon,FPointDouble position);


int debugwatch = 0;
//************************************************************************************************
void GDFireAAA(void *ogun, int sitetype, int radardatnum)
{
	GDRadarData *radardat=NULL;
	DBWeaponType *weapon=NULL;
	int seekertype;
	float fworkval, fworkval2;
	long numfired;
	FPointDouble position;
	int workval;
	float randperc;
	PlaneParams *planepnt;
	double yoffset;
	float workrange;
	double orgy;
	AAWeaponInstance *gun = NULL;
	MovingVehicleParams *vgun = NULL;
	int side;
	FPoint novel(0.0f);

	if(sitetype == MOVINGVEHICLE)
	{
		vgun = (MovingVehicleParams *)ogun;
		radardat = &vgun->RadarWeaponData[radardatnum];
		if (!vgun->iShipType) // SCOTT FIX
		{
			if(radardatnum == 0)
				weapon = get_weapon_ptr(pDBVehicleList[vgun->iVDBIndex].lWeaponType);
			else
				weapon = get_weapon_ptr(pDBVehicleList[vgun->iVDBIndex].lWeaponType2);
			position = vgun->WorldPosition;
		}
		else
		{
			weapon = VGDGetShipWeaponForRDNum(vgun, radardatnum);

			//  Need to add offset of weapon here.
			position = vgun->WorldPosition;
		}
		orgy = position.Y;
		side = vgun->iSide;
	}
	else
	{
		gun = (AAWeaponInstance *)ogun;
		weapon = GetWeaponPtr((BasicInstance *)gun);
		radardat = GDGetRadarData((BasicInstance *)gun);
		position = gun->Basics.Position;
		orgy = position.Y;
		GDConvertGrndSide((BasicInstance *) gun);
	}

	if (!weapon) return;

	GunSounds(weapon,position);

	seekertype = weapon->iSeekerType;

	if((!cAAAFiring) && (!(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)) && (side != PlayerPlane->AI.iSide))
	{
		if(lPlayerGroupTimer < 0)
		{
			lPlayerGroupTimer = rand() & 0x1FFF;
		}
		cAAAFiring = 1;
		cCurrentLook = rand()&7;
	}

	fworkval = 60.0f / (float)weapon->iRateOfFire;
	fworkval *= 1000.0f;
	radardat->lWActionTimer = fworkval;

	fworkval2 = (float)DeltaTicks / fworkval;
	if(seekertype == 8)
	{
		fworkval2 += 0.5f;
	}
	if(fworkval2 < 1)
	{
		fworkval2 = 1.0f;
	}
	numfired = (long)fworkval2;

	radardat->lWNumberFired = radardat->lWNumberFired + numfired;

	if(seekertype == 8)
	{
		if(sitetype == MOVINGVEHICLE)
		{
			radardat->pAAAStream = NULL;
			InstantiateAAABullet((MovingVehicleParams *)ogun, radardat->FirePitchANG, radardat->FireHeadingANG, weapon->lWeaponID, radardatnum);
			radardat->lWFlags1 |= GD_W_BURST;
		}
		else
		{
			if(!iInJump)
			{
				if (!GDInView((AAWeaponInstance *)ogun))
				{
					if (!radardat->iShotsTillNextCheck--)
					{
						radardat->iShotsTillNextCheck = 3;
						DBWeaponType *weapon = GetWeaponTypeFromAAWeaponFamily(gun->Type);
						if (weapon)
							GDCheckForRandomAAAHit(((BasicInstance *)ogun)->Position, weapon, NULL, (AAAStreamVertex *)TRUE);
					}
				}
				else
					radardat->pAAAStream = InstantiateAAAShots((AAWeaponInstance *)ogun, radardat->pAAAStream, radardat->FirePitchANG, radardat->FireHeadingANG);
			}
			radardat->lWFlags1 |= GD_W_STREAM;
		}

		if(radardat->lWNumberFired >= weapon->iBurstRate)
		{
			if(radardat->lWFlags1 & GD_W_RANDOM_FIRE)
			{
				GDSetUpRandomAAA(ogun, 0, sitetype);
			}

			radardat->lWReloadTimer = 4000 + (rand() & 0xFFF);
		}
	}
	else if(seekertype == 9)
	{
		radardat->pAAAStream = NULL;
		if(sitetype == MOVINGVEHICLE)
		{
			InstantiateAAABullet((MovingVehicleParams *)ogun, radardat->FirePitchANG, radardat->FireHeadingANG, weapon->lWeaponID, radardatnum);
		}
		else
		{
			InstantiateAAABullet((BasicInstance *)ogun, radardat->FirePitchANG, radardat->FireHeadingANG, weapon->lWeaponID);
		}
		radardat->lWFlags1 |= GD_W_BURST;
		if(radardat->lWNumberFired >= weapon->iBurstRate)
		{
			if(radardat->lWFlags1 & GD_W_RANDOM_FIRE)
			{
				GDSetUpRandomAAA(ogun, 0, sitetype);
			}
			radardat->lWReloadTimer = 20000 + (rand() & 0x7FFF);  //  8000;
		}
	}
	else if(seekertype == 10)  //  should be GD_W_SINGLE
	{
		radardat->pAAAStream = NULL;
		planepnt = (PlaneParams *)radardat->Target;

		float fworkrad;
		float fworkrange;
		FPointDouble ex_pos = position;
		ex_pos.Y += (10 * FTTOWU);

		NewCanister( CT_FLASH, ex_pos, novel, weapon->iDamageValue*(0.15f));

		ex_pos = position;
		ex_pos.Y += (5 * FTTOWU);

		fworkrad = (float)weapon->iDamageRadius * FTTOWU;  //   * 0.25;
		if(fworkrad <= 0)
			fworkrad = 1;

		fworkrange = (float)weapon->iDamageValue;  //   * 0.25;
		if(fworkrange <= 0)
			fworkrange = 1;

		NewExplosionForcedWeapon(ex_pos, fworkrad, fworkrange, FALSE, 0, CT_NONE );

		workval = 0;
		if(planepnt)
		{
			if((planepnt->WorldPosition - position) < (weapon->iRange * NMTOWU))
			{
				workval = 1;
			}
		}


		if(workval)
		{
			workval = rand() & 4095;
			randperc = ((float)workval / 2048.0f) - 1.0f;
			position.X = planepnt->WorldPosition.X + (randperc * 2000.0f * FTTOWU);

			workval = rand() & 4095;
			randperc = ((float)workval / 2048.0f) - 1.0f;
			position.Z = planepnt->WorldPosition.Z + (randperc * 2000.0f * FTTOWU);

			workval = rand() & 4095;
			randperc = ((float)workval / 2048.0f) - 1.0f;
			yoffset = (planepnt->WorldPosition.Y - position.Y) + (randperc * 2000.0f * FTTOWU);
		}
		else
		{
			workrange = (float)weapon->iRange;
			if(workrange > 2.0f)
			{
				workrange = 2.0f;
			}

			workval = rand() & 4095;
			randperc = ((float)workval / 2048.0f) - 1.0f;
			position.X += randperc * workrange * NMTOWU;

			workval = rand() & 4095;
			randperc = ((float)workval / 2048.0f) - 1.0f;
			position.Z += randperc * workrange * NMTOWU;

			workval = rand() & 2047;
			randperc = ((float)workval / 4096.0f) + 0.25f;
			yoffset = randperc * (float)weapon->iMaxAlt * FTTOWU;
		}

		if(yoffset < ((weapon->iMaxAlt/4) * FTTOWU))
		{
			yoffset = (weapon->iMaxAlt / 4) * FTTOWU;
		}
		position.Y += yoffset;

		randperc = (position.Y - orgy) * WUTOFT;
		yoffset *= WUTOFT;
		workval = position.Y * WUTOFT;

		NewCanister( CT_FLASH, position, novel, weapon->iDamageValue*(1.0f/2.0f));
		NewGenerator(PLANE_EXPLODES,position,0.0,2.0f,50);
		DoExplosionInAir(NULL, 1, &position, weapon, NULL);

//		NewCanister( CT_MISSLE_EXPLODE,	position, novel, weapon->iDamageValue*(1.0f/3.0f) );

		PlayPositionalExplosion(weapon->iWarHead,Camera1,position);

		radardat->lWFlags1 |= GD_W_SINGLE;
		if(radardat->lWNumberFired >= weapon->iBurstRate)
		{
			radardat->lWReloadTimer = 600000;
			if(radardat->lWFlags1 & GD_W_RANDOM_FIRE)
			{
				GDSetUpRandomAAA(ogun, 0, sitetype);
			}
		}
	}
//	else  // This was a recursive bad idea and I can't remember why I did it.
//	{
//		planepnt = (PlaneParams *)radardat->Target;
//		GDFireInitialAAA(ogun, planepnt, sitetype, radardatnum);
//	}
}

//************************************************************************************************
void GDUpdateImpactPoint(void *ocurrweapon, int sitetype, int radardatnum)
{
	GDRadarData *radardat;
	float heading, pitch, dpitch, dheading, fworkval;
	DBWeaponType *weapon=NULL;
	float bulletvel;
	PlaneParams *planepnt;
	long dticks;
	AAWeaponInstance *currweapon = NULL;
	MovingVehicleParams *vcurrweapon = NULL;
	FPointDouble position;
	ANGLE tempangle, holdangle;
	float maxyaw = 6.0f;
	float deltamod = ((float)DeltaTicks / 1000.0f);
	FPointDouble weaponoffset;

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	if(sitetype == MOVINGVEHICLE)
	{
		vcurrweapon = (MovingVehicleParams *)ocurrweapon;
		radardat = &vcurrweapon->RadarWeaponData[radardatnum];
		if (!vcurrweapon->iShipType) // SCOTT FIX
		{
			if(radardatnum == 0)
				weapon = get_weapon_ptr(pDBVehicleList[vcurrweapon->iVDBIndex].lWeaponType);
			else
				weapon = get_weapon_ptr(pDBVehicleList[vcurrweapon->iVDBIndex].lWeaponType2);
		}
		else
		{
			weapon = VGDGetShipWeaponForRDNum(vcurrweapon, radardatnum);
		}

		//  Add weapon offset here.  SRE

		VGDGetWeaponPosition(ocurrweapon, sitetype, radardatnum, &weaponoffset);
		position.X = vcurrweapon->WorldPosition.X + weaponoffset.X;
		position.Y = vcurrweapon->WorldPosition.Y + weaponoffset.Y;
		position.Z = vcurrweapon->WorldPosition.Z + weaponoffset.Z;
	}
	else
	{
		currweapon = (AAWeaponInstance *)ocurrweapon;
		radardat = GDGetRadarData((BasicInstance *)currweapon);
		weapon = GetWeaponPtr((BasicInstance *)currweapon);
		position = currweapon->Basics.Position;
	}

	if (!weapon) return;

	planepnt = (PlaneParams *)radardat->Target;
	if((planepnt) && (!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM))))
	{
		if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			radardat->lWFlags1 &= ~(GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM);
			radardat->lWGenericTimer = -1;
			return;
		}

		bulletvel = weapon->fMaxSpeed;
		if(bulletvel == 0)
		{
			bulletvel = 3380;
		}

		if(radardat->lWFlags1 & GD_W_PITCH_INC)
		{
			pitch = AIConvertAngleTo180Degree(radardat->FirePitchANG);
			dpitch = (float)DeltaTicks / 1000;
			pitch += dpitch;
			if(pitch > 90)
			{
				pitch = 90;
			}
			if(pitch >= AIConvertAngleTo180Degree(radardat->PitchOffsetANG))
			{
				radardat->lWFlags1 &= ~GD_W_PITCH_INC;
				radardat->lWFlags1 |= GD_W_PITCH_DEC;
			}
			radardat->FirePitchANG = AIConvert180DegreeToAngle(pitch);
		}
		else if(radardat->lWFlags1 & GD_W_PITCH_DEC)
		{
			pitch = AIConvertAngleTo180Degree(radardat->FirePitchANG);
			dpitch = (float)DeltaTicks / 1000;
			pitch -= dpitch;
			if(pitch >= AIConvertAngleTo180Degree(radardat->HeadingOffsetANG))
			{
				radardat->lWFlags1 &= ~GD_W_PITCH_DEC;
				radardat->lWFlags1 |= GD_W_PITCH_INC;
			}
			radardat->FirePitchANG = AIConvert180DegreeToAngle(pitch);
		}

		if(radardat->lTimeDiffOffset)
		{
			if(radardat->lWFlags1 & GD_W_TIME_DEC)
			{
				if(labs(radardat->lTimeDiff) < 500)
				{
					dticks = -((long)DeltaTicks>>1);
				}
				else
				{
					dticks = -(long)DeltaTicks;
				}

				radardat->lTimeDiff += dticks;

				if(radardat->lTimeDiff < -radardat->lTimeDiffOffset)
				{
					radardat->lTimeDiff = -radardat->lTimeDiffOffset;
					radardat->fImpactTime = -1.0f;
					radardat->lWFlags1 &= ~GD_W_TIME_DEC;
					radardat->lWFlags1 |= GD_W_TIME_INC;
				}
				else if(labs(radardat->lTimeDiff) < labs(dticks))
				{
					radardat->fImpactTime = -1.0f;
				}
			}
			else if(radardat->lWFlags1 & GD_W_TIME_INC)
			{
				if(labs(radardat->lTimeDiff) < 500)
				{
					dticks = DeltaTicks>>1;
				}
				else
				{
					dticks = DeltaTicks;
				}

				radardat->lTimeDiff += dticks;

				if(radardat->lTimeDiff > radardat->lTimeDiffOffset)
				{
					radardat->lTimeDiff = radardat->lTimeDiffOffset;
					radardat->fImpactTime = -1.0f;
					radardat->lWFlags1 |= GD_W_TIME_DEC;
					radardat->lWFlags1 &= ~GD_W_TIME_INC;
				}
				else if(labs(radardat->lTimeDiff) < labs(dticks))
				{
					radardat->fImpactTime = -1.0f;
				}
			}
			else
			{
				radardat->fImpactTime = -1.0f;
				radardat->lTimeDiff = 0;
				radardat->lTimeDiffOffset = 0;
			}
		}
		else
		{
			radardat->fImpactTime = -1.0f;
			radardat->lTimeDiff = 0;
			radardat->lTimeDiffOffset = 0;
			return;
		}

		GDComputeGunHP(planepnt, position, &radardat->fpAimPoint, bulletvel, &heading, &pitch, &radardat->fImpactTime, ((float)radardat->lTimeDiff * 0.001));
//		radardat->FirePitchANG = AIConvert180DegreeToAngle(pitch);
//		radardat->FireHeadingANG = AIConvert180DegreeToAngle(heading);
		holdangle = AIConvert180DegreeToAngle(heading);
		tempangle = holdangle - radardat->FireHeadingANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FireHeadingANG = holdangle;

		holdangle = AIConvert180DegreeToAngle(pitch);
		tempangle = holdangle - radardat->FirePitchANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FirePitchANG = holdangle;
	}
	else if((sitetype == MOVINGVEHICLE) && (!planepnt))
	{
		radardat->lWFlags1 &= ~(GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM);
	}
	else
	{
		if((!planepnt) && (!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE))))
		{
			GDSetUpRandomAAA(ocurrweapon, 0, sitetype);
			radardat->lWFlags1 &= ~GD_W_PREP_RANDOM;
			radardat->lWFlags1 |= (GD_W_FIRING|GD_W_RANDOM_FIRE);
		}
		pitch = AIConvertAngleTo180Degree(radardat->FirePitchANG);
		dpitch = AIConvertAngleTo180Degree(radardat->PitchOffsetANG);
		fworkval = dpitch * ((float)DeltaTicks / 1000.0f);
		pitch += dpitch;
		if(pitch < 45)
		{
			pitch += (fabs(fworkval) * 2.0f);
			dpitch = fabs(dpitch);
			radardat->PitchOffsetANG = AIConvert180DegreeToAngle(dpitch);
		}
		else if(pitch > 90)
		{
			pitch -= (fabs(fworkval) * 2.0f);
			dpitch = -fabs(dpitch);
			radardat->PitchOffsetANG = AIConvert180DegreeToAngle(dpitch);
		}

		heading = AIConvertAngleTo180Degree(radardat->FireHeadingANG);
		dheading = AIConvertAngleTo180Degree(radardat->HeadingOffsetANG);
		dheading *= deltamod;  //  ((float)DeltaTicks / 1000);
		heading += dheading;

		holdangle = AIConvert180DegreeToAngle(heading);
		tempangle = holdangle - radardat->FireHeadingANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FireHeadingANG = holdangle;

		holdangle = AIConvert180DegreeToAngle(pitch);
		tempangle = holdangle - radardat->FirePitchANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FirePitchANG = holdangle;
	}
}

//************************************************************************************************
void GDUpdateImpactPointLite(void *ocurrweapon, int sitetype, int radardatnum)
{
	GDRadarData *radardat;
	float heading, pitch, dpitch, dheading, fworkval;
	DBWeaponType *weapon;
	float bulletvel;
	PlaneParams *planepnt;
	long dticks;
	AAWeaponInstance *currweapon = NULL;
	MovingVehicleParams *vcurrweapon = NULL;
	FPointDouble position;
	ANGLE tempangle, holdangle;
	float maxyaw = 6.0f;
	float deltamod = ((float)DeltaTicks / 1000.0f);
	FPointDouble weaponoffset;

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	if(sitetype == MOVINGVEHICLE)
	{
		vcurrweapon = (MovingVehicleParams *)ocurrweapon;
		radardat = &vcurrweapon->RadarWeaponData[radardatnum];
		if (!vcurrweapon->iShipType) // SCOTT FIX
		{
			if(radardatnum == 0)
				weapon = get_weapon_ptr(pDBVehicleList[vcurrweapon->iVDBIndex].lWeaponType);
			else
				weapon = get_weapon_ptr(pDBVehicleList[vcurrweapon->iVDBIndex].lWeaponType2);
		}
		else
		{
			weapon = VGDGetShipWeaponForRDNum(vcurrweapon, radardatnum);
		}

		//  Add offset for weapon here.  SRE.
		VGDGetWeaponPosition(ocurrweapon, sitetype, radardatnum, &weaponoffset);
		position.X = vcurrweapon->WorldPosition.X + weaponoffset.X;
		position.Y = vcurrweapon->WorldPosition.Y + weaponoffset.Y;
		position.Z = vcurrweapon->WorldPosition.Z + weaponoffset.Z;
	}
	else
	{
		currweapon = (AAWeaponInstance *)ocurrweapon;
		radardat = GDGetRadarData((BasicInstance *)currweapon);
		weapon = GetWeaponPtr((BasicInstance *)currweapon);
		position = currweapon->Basics.Position;
	}

	if (!weapon) return;

	planepnt = (PlaneParams *)radardat->Target;
	if((planepnt) && (!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM))))
	{
		if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			radardat->lWFlags1 &= ~(GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM);
			radardat->lWGenericTimer = -1;
			return;
		}

		bulletvel = weapon->fMaxSpeed;
		if(bulletvel == 0)
		{
			bulletvel = 3380;
		}

		if(radardat->lWFlags1 & GD_W_TIME_DEC)
		{
			if(labs(radardat->lTimeDiff) < 500)
			{
				dticks = -((long)DeltaTicks>>2);
			}
			else
			{
				dticks = -(long)DeltaTicks;
			}

			radardat->lTimeDiff += dticks;

			if(radardat->lTimeDiff < -2000)
			{
				radardat->lTimeDiff = -2000;
				radardat->fImpactTime = -1.0f;
				radardat->lWFlags1 &= ~GD_W_TIME_DEC;
				radardat->lWFlags1 |= GD_W_TIME_INC;
			}
			else if(labs(radardat->lTimeDiff) < labs(dticks))
			{
				radardat->fImpactTime = -1.0f;
			}
			else
			{
				return;
			}
		}
		else if(radardat->lWFlags1 & GD_W_TIME_INC)
		{
			if(labs(radardat->lTimeDiff) < 500)
			{
				dticks = DeltaTicks>>2;
			}
			else
			{
				dticks = DeltaTicks;
			}

			radardat->lTimeDiff += dticks;

			if(radardat->lTimeDiff > 2000)
			{
				radardat->lTimeDiff = 2000;
				radardat->fImpactTime = -1.0f;
				radardat->lWFlags1 |= GD_W_TIME_DEC;
				radardat->lWFlags1 &= ~GD_W_TIME_INC;
			}
			else if(labs(radardat->lTimeDiff) < labs(dticks))
			{
				radardat->fImpactTime = -1.0f;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}

		GDComputeGunHP(planepnt, position, &radardat->fpAimPoint, bulletvel, &heading, &pitch, &radardat->fImpactTime, ((float)radardat->lTimeDiff * 0.001));
//		radardat->FirePitchANG = AIConvert180DegreeToAngle(pitch);
//		radardat->FireHeadingANG = AIConvert180DegreeToAngle(heading);
		holdangle = AIConvert180DegreeToAngle(heading);
		tempangle = holdangle - radardat->FireHeadingANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FireHeadingANG = holdangle;

		holdangle = AIConvert180DegreeToAngle(pitch);
		tempangle = holdangle - radardat->FirePitchANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FirePitchANG = holdangle;
	}
	else
	{
		if((!planepnt) && (!(radardat->lWFlags1 & (GD_W_RANDOM_FIRE))))
		{
			GDSetUpRandomAAA(ocurrweapon, 0, sitetype);
			radardat->lWFlags1 &= ~GD_W_PREP_RANDOM;
			radardat->lWFlags1 |= (GD_W_FIRING|GD_W_RANDOM_FIRE);
		}
		pitch = AIConvertAngleTo180Degree(radardat->FirePitchANG);
		dpitch = AIConvertAngleTo180Degree(radardat->PitchOffsetANG);
		fworkval = dpitch * ((float)DeltaTicks / 1000.0f);
		pitch += dpitch;
		if(pitch < 45)
		{
			pitch += (fabs(fworkval) * 2);
			dpitch = fabs(dpitch);
			radardat->PitchOffsetANG = AIConvert180DegreeToAngle(dpitch);
		}
		else if(pitch > 90)
		{
			pitch -= (fabs(fworkval) * 2);
			dpitch = -fabs(dpitch);
			radardat->PitchOffsetANG = AIConvert180DegreeToAngle(dpitch);
		}

		heading = AIConvertAngleTo180Degree(radardat->FireHeadingANG);
		dheading = AIConvertAngleTo180Degree(radardat->HeadingOffsetANG);
		dheading *= deltamod;  //  ((float)DeltaTicks / 1000.0f);
		heading += dheading;

//		radardat->FireHeadingANG = AIConvert180DegreeToAngle(heading);
//		radardat->FirePitchANG = AIConvert180DegreeToAngle(pitch);
		holdangle = AIConvert180DegreeToAngle(heading);
		tempangle = holdangle - radardat->FireHeadingANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FireHeadingANG = holdangle;

		holdangle = AIConvert180DegreeToAngle(pitch);
		tempangle = holdangle - radardat->FirePitchANG;
		if(fabs(AIConvertAngleTo180Degree(tempangle) / deltamod) > maxyaw)
		{
//			radardat->pAAAStream = NULL;  //  Do this to try to keep long weird lines from happening
		}
		radardat->FirePitchANG = holdangle;
	}
}

//************************************************************************************************
float GDGetWeaponVelocity(DBWeaponType *weapon)
{
	float bulletvel;

	if(weapon == NULL)
	{
		return(3380.0f);
	}

	bulletvel = weapon->fMaxSpeed;
	if(bulletvel == 0)
	{
		bulletvel = 3380;
	}
	return(bulletvel);
}

//************************************************************************************************
long GDGetAAADuration(DBWeaponType *weapon)
{
	float bulletvel;
	float fworkval;

	if(weapon == NULL)
	{
		return(250);
	}

	bulletvel = GDGetWeaponVelocity(weapon);
	fworkval = weapon->iMaxAlt;
	if(fworkval < 6076)		//  will go aleast a mile
	{
		fworkval = 6076;
	}
	fworkval *= 1.5;		//  add some range for shooting max alt.
	fworkval /= bulletvel;	//  determine time it will take to get there.
	fworkval *= 50;			//  determine number of 50Hz cycles
	return((long)fworkval);
}

//************************************************************************************************
void GDUpdateReportedArea(FPointDouble Position, int sitetype, double use_radius)
{
	InfoProviderInstance *currgci;
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	DBWeaponType *weapon;
	double radius;
	MovingVehicleParams *vehiclepnt;
	int cnt;
	int setreported;
	int maxcheck;

	currgci = AllInfoProviders;

	radius = (double)NMTOWU * 2.0f;
	if(use_radius > radius)
	{
		radius = use_radius;
	}

	while(currgci)
	{
		if((fabs(currgci->Basics.Position.X - Position.X) < radius) && (fabs(currgci->Basics.Position.Z - Position.Z) < radius))
		{
			radardat = (GDRadarData *)currgci->AIDataBuf;
			radardat->lRFlags1 |= GD_HAS_BEEN_REP_GEN;
			if((currgci->Type->ProviderFlags & IPF_PROVIDER_TYPE) == (DWORD)sitetype)
			{
				radardat->lRFlags1 |= GD_HAS_BEEN_REPORTED;
			}
		}
		currgci = (InfoProviderInstance *)currgci->Basics.NextRelatedInstance;
	}

	currweapon = AllAAWeapons;
	while(currweapon)
	{
		if((fabs(currweapon->Basics.Position.X - Position.X) < radius) && (fabs(currweapon->Basics.Position.Z - Position.Z) < radius))
		{
			radardat = (GDRadarData *)currweapon->AIDataBuf;
			radardat->lRFlags1 |= GD_HAS_BEEN_REP_GEN;
			if(sitetype == IPF_LOCAL_SAM_PROVIDER)
			{
				weapon = GetWeaponPtr((BasicInstance *)currweapon);
				if(weapon)
				{
					if(weapon->iWeaponType != 6)
					{
						radardat->lRFlags1 |= GD_HAS_BEEN_REPORTED;
					}
				}
			}
			else if(sitetype == IPF_LOCAL_AAA_PROVIDER)
			{
				weapon = GetWeaponPtr((BasicInstance *)currweapon);
				if(weapon)
				{
					if(weapon->iWeaponType == 6)
					{
						radardat->lRFlags1 |= GD_HAS_BEEN_REPORTED;
					}
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
//		if((vehiclepnt->Status & (VL_ACTIVE)) && (vehiclepnt->Status & (VL_FIRE_WEAPONS)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
		{
			if((fabs(vehiclepnt->WorldPosition.X - Position.X) < radius) && (fabs(vehiclepnt->WorldPosition.Z - Position.Z) < radius))
			{
				setreported = 0;
				if(vehiclepnt->iShipType)
				{
					maxcheck = NUM_DEFENSE_LIST_ITEMS;
				}
				else
				{
					maxcheck = 2;
				}

				for(cnt = 0; cnt < maxcheck; cnt ++)
				{
					radardat = &vehiclepnt->RadarWeaponData[cnt];
					radardat->lRFlags1 |= GD_HAS_BEEN_REP_GEN;
					radardat->lRFlags1 |= GD_HAS_BEEN_REPORTED;
#if 0
					weapon=NULL;
					if (!vehiclepnt->iShipType)	// SCOTT FIX
					{
						if(!cnt)
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
						weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID);
					}

					if (!weapon) continue;

					if(sitetype == IPF_LOCAL_SAM_PROVIDER)
					{
						if(weapon)
						{
							if(weapon->iWeaponType != 6)
							{
								setreport = 1;
							}
						}
					}
					else if(sitetype == IPF_LOCAL_AAA_PROVIDER)
					{
						if(weapon)
						{
							if(weapon->iWeaponType == 6)
							{
								setreport = 1;
							}
						}
					}
#endif
				}
			}
		}
	}
}

//************************************************************************************************
void GDAAASeen(PlaneParams *planepnt)
{
	double visualrange;
	GDRadarData *radardat;
	AAWeaponInstance *currweapon;
	DBWeaponType *weapon;
	float heading, pitch;
	float foundheading = -999;
	float workheading;
	BasicInstance *walker;
	FPointDouble position;


	visualrange = ((double)AI_VISUAL_RANGE_NM * NMTOWU);

	currweapon = AllAAWeapons;

	while(currweapon)
	{
		if((fabs(currweapon->Basics.Position.X - planepnt->WorldPosition.X) < visualrange) && (fabs(currweapon->Basics.Position.Z - planepnt->WorldPosition.Z) < visualrange))
		{
			radardat = (GDRadarData *)currweapon->AIDataBuf;
			if((!(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)) && (GDConvertGrndSide((BasicInstance *) currweapon) != planepnt->AI.iSide))
			{
				weapon = GetWeaponPtr((BasicInstance *)currweapon);
				if((weapon->iWeaponType == 6) && (radardat->lWFlags1 & GD_W_FIRING))
				{
					PLGetRelativeHP(planepnt, currweapon->Basics.Position, &heading, &pitch);

					if(pitch < 0)
					{
						pitch += 50;	//  Added since Clip will leave center of view point well above rail, also AAA moves up.
					}

					if(!PLClipViewHP(&heading, &pitch))
					{
						radardat->lRFlags1 |= (GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN);
						lNoticeTimer += 60000;
						walker = (BasicInstance *)currweapon;
						position = walker->Position;
						if(planepnt != PlayerPlane)
						{
							GDUpdateReportedArea(position, IPF_LOCAL_AAA_PROVIDER);
							foundheading = heading;
							AICWingmanAAAReport(planepnt - Planes, (int)heading);
							lPlayerGroupTimer = 30000;  //  15000;
							return;
						}
						else
						{
							GDUpdateReportedArea(position, IPF_LOCAL_AAA_PROVIDER);
							if(foundheading == -999)
							{
								foundheading = heading;
							}
							else if(foundheading != -888)
							{
								workheading = heading - foundheading;
								workheading = AICapAngle(workheading);

								if(fabs(workheading) > 45)
								{
									foundheading = -888;
								}
							}
						}
					}
				}
			}
		}
		currweapon = (AAWeaponInstance *)currweapon->Basics.NextRelatedInstance;
	}

	if((planepnt == PlayerPlane) && (foundheading != -999))
	{
		AICWSOAAAReport(planepnt - Planes, (int)heading);
		lPlayerGroupTimer = 30000;  //  15000;
	}
}

//************************************************************************************************
int AICheckIfAnyInterception(PlaneParams *planepnt, int side)
{
	PlaneParams *checkplane;
	int tempfence = 0;

	if(planepnt->AI.iSide == side)
	{
		return(-1);
	}
	else if((!(iMultiSides & (MS_AGGRESIVE_NEUTRALS))) && (((planepnt->AI.iSide == AI_ENEMY) || (planepnt->AI.iSide == AI_NEUTRAL)) && ((side == AI_ENEMY) || (side == AI_NEUTRAL))))
	{
		return(-1);
	}
	else if((iAI_ROE[side] < 2) && (!(iMultiSides & (MS_IGNORE_FENCES))))
	{
		tempfence = AICCheckAllFences((planepnt->WorldPosition.X * WUTOFT), (planepnt->WorldPosition.Z * WUTOFT));

		if((!tempfence) && ((planepnt->AI.iSide == AI_FRIENDLY) || (planepnt->AI.iSide == AI_NEUTRAL)) && (side == AI_ENEMY))
		{
			return(-1);
		}
		else if((tempfence) && (planepnt->AI.iSide == AI_ENEMY))
		{
			return(-1);
		}

	}

	if(planepnt->AI.lInterceptTimer >= 0)
	{
		return(1);
	}

	checkplane = &Planes[0];
	while(checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (side == checkplane->AI.iSide)
				&& (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_FIGHTER) && (checkplane->AI.AirTarget))
		{
			if(AIGetLeader(checkplane->AI.AirTarget) == planepnt)
			{
				return(1);
			}
		}
		checkplane ++;
	}
	return(0);
}

//************************************************************************************************
void AIContactAirCommand(BasicInstance *radarsite, PlaneParams *foundplane, int side)
{
	InfoProviderInstance *treetop;
	float interceptnm = 120.0f;
	AAWeaponInstance *tweapon;
	InfoProviderInstance *tprovider;

	if(radarsite->Family == FAMILY_AAWEAPON)
	{
		tweapon = (AAWeaponInstance *)radarsite;
		treetop = tweapon->OurProvider;
		if(treetop == NULL)
		{
			return;
		}
	}
	else
	{
		tprovider = (InfoProviderInstance *)radarsite;
		treetop = tprovider->OurProvider;
	}

	if(treetop != NULL)
	{
		while(treetop->OurProvider)
		{
			treetop = treetop->OurProvider;
		}
	}
	else
	{
		treetop = (InfoProviderInstance *)radarsite;
	}

	switch (treetop->Type->ProviderFlags & IPF_PROVIDER_TYPE)
	{
		case IPF_SECTOR_COMMAND_PROVIDER :
			interceptnm = 120.0f;
			break;

		case IPF_GCI_PROVIDER :
			interceptnm = 60.0f;
			break;

		case IPF_LOCAL_SAM_PROVIDER :
			return;
			break;

		case IPF_LOCAL_AAA_PROVIDER :
			return;
			break;

		default:
			return;
			break;
	}

	AIVectorIntercepts(foundplane, side, interceptnm);
}

//**************************************************************************************
void AIGetGroundJammingMod(FPointDouble position, int grndside, float *modifiers)
{
	PlaneParams *checkplane;
	float dx, dy, dz, tdist, offangle;
	float jammingrange = 100.0f * NMTOWU;
	float tempval;;
	int cnt;
	int iworkval, iworkval2;
	ANGLE wangle;
	float wheading;
	long sojmask = (AI_SO_JAMMING);  //  (AI_SO_JAMMING|AI_SOJ_ON_BOARD);

	for(cnt = 0; cnt < 36; cnt ++)
	{
		modifiers[cnt] = jammingrange;
	}

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if((checkplane->AI.iAIFlags2 & sojmask) == sojmask)
		{
			if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (grndside != checkplane->AI.iSide) && (checkplane->OnGround == 0))
			{
				dx = checkplane->WorldPosition.X - position.X;
				dy = checkplane->WorldPosition.Y - position.Y;
				dz = checkplane->WorldPosition.Z - position.Z;
	//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
				if((fabs(dx) < jammingrange) && (fabs(dz) < jammingrange))
				{
					tdist = QuickDistance(dx, dz);
	//				if(tdist < nlfoundrange)
					if(tdist < jammingrange)
					{
						offangle = atan2(-dx, -dz) * 57.2958;
						while(offangle > 359)
							offangle -= 360;
						while(offangle < 0)
							offangle += 360;
						cnt = offangle / 10;

						wangle = AIConvert180DegreeToAngle(offangle) + 0x8000;
						wangle -= checkplane->Heading;
						wheading = AIConvertAngleTo180Degree(wangle);
						if(fabs(wheading) < 30.0f)
						{
							for(iworkval = (cnt - 3); iworkval <= (cnt + 3); iworkval ++)
							{
								iworkval2 = iworkval;
								while(iworkval2 < 0)
								{
									iworkval2 += 36;
								}
								while(iworkval2 > 35)
								{
									iworkval2 -= 36;
								}
								if(modifiers[iworkval2] > tdist)
								{
									modifiers[iworkval2] = tdist;
								}
							}
						}
					}
				}
			}
		}
	}

	for(cnt = 0; cnt < 36; cnt ++)
	{
		if(modifiers[cnt] >= jammingrange)
		{
			modifiers[cnt] = 1.0f;

		}
		else if(modifiers[cnt] < (jammingrange * 0.5f))
		{
			modifiers[cnt] = 0.5f;
		}
		else
		{
			tempval = (modifiers[cnt] * 2.0f) / jammingrange;
			modifiers[cnt] = 0.5f + (0.5f * (tempval * tempval));
		}
	}
}

//************************************************************************************************
void GDCheckForRandomAAAHit(FPointDouble Pos, DBWeaponType *pweapon_type, WeaponParams *W, AAAStreamVertex *AAA)
{
	double aaarangewu = (double)pweapon_type->iRange * NMTOWU;
	double aaamaxalt = (double)pweapon_type->iMaxAlt * FTTOWU;
	FPointDouble new_point;
	int orgflightstat;

	if((MultiPlayer) && (!(lAINetFlags1 & NGAI_ACTIVE)))
	{
		return;
	}

	for (PlaneParams *P=Planes; P<=LastPlane; P++)
	{
		if (!(P->Status & PL_ACTIVE))  continue;

		if (fabs(Pos.X - P->WorldPosition.X) < aaarangewu)
		{
			if (fabs(Pos.Z - P->WorldPosition.Z) < aaarangewu)
			{
				if (fabs(Pos.Y - P->WorldPosition.Y) < aaamaxalt)
				{
					if ((rand() & 0x7FFF) == 0)
					{
						new_point.MakeVectorToFrom( P->WorldPosition, Pos );

						orgflightstat = P->FlightStatus;

						int hit_value;
						float fBlastRadius;
						float fMultiplier, fDamageValue;
						FPoint vel;

						hit_value = DTE_DIRECT_HIT;
						hit_value |= (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];
						fBlastRadius = (pweapon_type->iDamageRadius * FTTOWU);
						fMultiplier = ((float)pweapon_type->iDamageValue) / (fBlastRadius * fBlastRadius);
						fDamageValue = fBlastRadius * fBlastRadius * fMultiplier;

						DamagePlane(P, hit_value, GameLoop, fDamageValue, new_point, fBlastRadius, W, pweapon_type);

						//PlayPositionalExplosion(pweapon_type->iWarHead,Camera1,P->WorldPosition);

						if(W)
						{
							AICCheckForHitMsgs( P, W->P, orgflightstat, W );
						}
						else
						{
							AICCheckForDamageMsgs(P, orgflightstat, (P->FlightStatus & (PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT|PL_ENGINE_REAR_LEFT)), NULL, AAA, AAA->Weapon);
						}

						return;
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------------
 *
 *	AGClosureRate()
 *
 *		Return TRUE rate of closure between
 *		a ground object and an aircraft target.  (+/- KTS)
 *
 */
float AGClosureRate( BasicInstance *walker, PlaneParams *T )
{
	FPoint tVector;

	tVector  = T->WorldPosition;
	tVector -= walker->Position;
	tVector.Normalize();		// unit vector ME -> target

	float V2 = T->IfVelocity * tVector;	// target's speed away from me

	return (-V2) * FTSEC_TO_KNOTS;
}

//**************************************************************************************
int AICheckRadarCrossSig(BasicInstance *walker, PlaneParams *checkplane, float daltft, float distnm, DBRadarType *radar, int inSTT)
{
	int crossval = 0;
	int workval;
	int crosssig = pDBAircraftList[checkplane->AI.iPlaneIndex].iRadarCrossSignature;
	int checkedone = 0;
	int checkaspect = 0;
	int useHPRF = 1;
	int useLPRF = 0;

	if(!radar)
	{
		return(0);
	}

	if(radar->iRadarType & RADAR_TYPE_MPRF)
	{
//	   	crossval = GetCalculatedCrossSection(crosssig, MPRF, daltft, AGClosureRate(walker, checkplane), distnm, inSTT);
	   	crossval = GetCrossSectionFromGround(crosssig, MPRF, daltft, AGClosureRate(walker, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		checkedone = 1;
		useHPRF = 0;
	}


	if(radar->iRadarType & RADAR_TYPE_LPRF)
	{
//	   	workval = GetCalculatedCrossSection(crosssig, HPRF, daltft, AGClosureRate(walker, checkplane), distnm, inSTT);
	   	workval = GetCrossSectionFromGround(crosssig, LPRF, daltft, AGClosureRate(walker, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		if(workval > crossval)
		{
			crossval = workval;
		}
		useHPRF = 0;
		useLPRF = 1;
		checkedone = 1;
	}

	if((radar->iRadarType & RADAR_TYPE_HPRF) || (!checkedone))
	{
//	   	workval = GetCalculatedCrossSection(crosssig, HPRF, daltft, AGClosureRate(walker, checkplane), distnm, inSTT);
	   	workval = GetCrossSectionFromGround(crosssig, HPRF, daltft, AGClosureRate(walker, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		if(workval > crossval)
		{
			crossval = workval;
		}
		useHPRF = 1;
	}

	if(crossval >= 3)	//  was 4
	{
		checkaspect = 1;;
	}
	if((crossval >= 3) && (inSTT))
	{
		checkaspect = 1;
	}

	if(!checkaspect)
	{
		return(FALSE);
	}
	if(useLPRF)
	{
		return(TRUE);
	}

	FPointDouble FromRadarToTarget;

	FromRadarToTarget.MakeVectorToFrom(checkplane->WorldPosition,walker->Position);

	FromRadarToTarget.Normalize();  //  I think we need to do this

  FPointDouble ProjPoint;
//	ProjPoint.SetValues(-500.0*checkplane->Attitude.m_Data.RC.R0C2,-500.0*checkplane->Attitude.m_Data.RC.R1C2,
//							                     -500.0*checkplane->Attitude.m_Data.RC.R2C2);
	ProjPoint.SetValues(checkplane->Attitude.m_Data.RC.R0C2,checkplane->Attitude.m_Data.RC.R1C2,
							                     checkplane->Attitude.m_Data.RC.R2C2);


	float Dot = FromRadarToTarget * ProjPoint;

	float Threshold;
	if(radar->iGeneration == 1)
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G1 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G1;
	}
	else if(radar->iGeneration == 2)
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G2 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G2;
	}
	else
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G3 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G3;
	}

	return( fabs(Dot) > Threshold );
}

#define MAX_TRACKS 50
//**************************************************************************************
int GDCheckNumRadarTracks(BasicInstance *radarsite, DBRadarType *radar, int converttarget)
{
	int numtargets = 0;
	PlaneParams *checkarray[MAX_TRACKS + 1];
	int cnt, numplanes;
	PlaneParams *checkplane;
	int skip;

	if(!radar)
		return(0);

	if(MAX_TRACKS < radar->iNumberOfTracks)
		return(1);

	for(cnt = 0; cnt < MAX_TRACKS; cnt ++)
	{
		checkarray[cnt] = NULL;
	}
	numplanes = 0;

	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(W->iTargetType == converttarget)
			{
				if(W->LauncherType == GROUNDOBJECT)
				{
					if(radarsite == (BasicInstance *)W->Launcher)
					{
						checkplane = (PlaneParams *)W->pTarget;

						if(checkplane)
						{
							skip = 0;
							for(cnt = 0; cnt < numplanes; cnt ++)
							{
								if(checkarray[cnt] == checkplane)
								{
									skip = 1;
									break;
								}
							}
							if(!skip)
							{
								numtargets++;
								checkarray[numplanes] = checkplane;
								numplanes ++;
								if(numtargets > radar->iNumberOfTracks)
								{
									return(0);
								}
							}
						}
					}
				}
			}
		}
		W++;
	}

	if(numtargets > radar->iNumberOfTracks)
	{
		return(0);
	}
	return(1);
}

extern void StartEngineSounds();
//============================================================================
//		Place holder object CODE  --  INSTANTIATE / DELETE / MOVE
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	InstantiatePlace()
 *
 */
WeaponParams *InstantiatePlaceHolder(FPointDouble position)
{
	int Index;

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return(NULL);

    W->Type = pLandingGateType;
	Index = AIGetWeaponIndex(W->Type->TypeNumber);

	W->InitialVelocity = 0;

	W->Kind = EJECTION_SEAT;
	W->WeaponMove = MovePlaceHolder;
	W->P = NULL;

	W->Flags = BOMB_INUSE|WEP_CHUTE_OUT;

	W->Vel.X = 0;
	W->Vel.Y = 0;
	W->Vel.Z = 0;
	W->Vel.SetValues((float)PlayerPlane->dt * FTTOWU,PlayerPlane->IfVelocity);

	W->Pos = position;

	W->lBurnTimer = 0;  //  10 second diration.
	W->lTimer = -1;
	W->LifeTime = 2000; //  10 seconds;

	W->Smoke = NULL;

	return(W);
}

/*----------------------------------------------------------------------------
 *
 *	DeletePlaceHolder()
 *
 */
void DeletePlaceHolder(WeaponParams *W)
{
	W->Flags = 0;
	W->Kind = NONE;

	UnsortObject(W);

	if (W == LastWeapon)
		while((LastWeapon > &Weapons[-1]) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;
}

/*----------------------------------------------------------------------------
 *
 *	MovePlaceHolder()
 *
 */
void MovePlaceHolder(WeaponParams *W)
{
	if(!(W->LifeTime--))
		DeletePlaceHolder(W);
}

//**************************************************************************************
int AIPutInfoProviderInTEWSCenter(InfoProviderInstance *radarsite, GDRadarData *radardat)
{
	if((radardat->lRFlags1 & GD_RADAR_LOCK) && (radardat->Target == PlayerPlane) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) && (!(radardat->lRFlags1 & (GD_I_AM_DEAD|GD_RADAR_TEMP_OFF))))
	{
		return(1);
	}

	if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK))) // We have missiles flying we need to keep eye on.
		return(0);

	return(AISeeIfStillGroundMissiled((BasicInstance *)radarsite, PlayerPlane, 0));
}

//**************************************************************************************
int AIPutWeaponProviderInTEWSCenter(AAWeaponInstance *radarsite, GDRadarData *radardat)
{
	if((!(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))) || (radardat->lRFlags1 & (GD_I_AM_DEAD|GD_RADAR_TEMP_OFF)))
	{
		return(0);
	}

	if((radardat->lRFlags1 & GD_RADAR_LOCK) && (radardat->Target == PlayerPlane) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) && (!(radardat->lRFlags1 & (GD_I_AM_DEAD|GD_RADAR_TEMP_OFF))))
	{
		return(1);
	}

	if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK))) // We have missiles flying we need to keep eye on.
		return(0);

	return(AISeeIfStillGroundMissiled((BasicInstance *)radarsite, PlayerPlane, 0));
}

//**************************************************************************************
int AICheckForGateStealerG(BasicInstance *radarsite, GDRadarData *radardat, PlaneParams *target)
{
	float fworkvar;
	int iworkvar;
	DBRadarType *radar;
	int breaklock;

	if(MultiPlayer)
	{
//		if((MultiPlayer) && ((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane)))
  		if(!(lAINetFlags1 & NGAI_ACTIVE))
			return(0);
	}

	if(!target)
		return(0);

	if(!(target->AI.iAICombatFlags2 & AI_GATE_STEALER))
	{
		return(0);
	}

	if(!(radardat->lRFlags1 & GD_RADAR_LOCK))
		return(0);

	if(target->Status & PL_AI_DRIVEN)
	{
		target->AI.iAICombatFlags2 |= AI_STT_THREAT;
	}

	// MAY HAVE TO DO SOMETHING HUMAN SPECIFIC
	if(!(target->AI.iAIFlags1 & AIJAMMINGON))
		return(0);

	if(radardat->lRLockTimer > -99999)
	{
		if(radardat->lRLockTimer >= 0)
		{
			radardat->lRLockTimer -= DeltaTicks;
			if(radardat->lRLockTimer < 0)
			{
				radar = GetRadarPtr((BasicInstance *)radarsite);

				fworkvar = frand() * 100.0f;
					
				breaklock = 0;
				switch(radar->iGeneration)
				{
					case 1:
						if(fworkvar < 40.0f)
							breaklock = 1;
						break;
					case 2:
						if(fworkvar < 30.0f)
							breaklock = 1;
						break;
					default:
						if(fworkvar < 20.0f)
							breaklock = 1;
						break;
				}
				if(breaklock)
				{
					radardat->lRFlags1 &= ~(GD_RADAR_LOCK|GD_CHECK_MISSILES);
					radardat->Target = NULL;
					if(radardat->lRActionTimer < 15000)
						radardat->lRActionTimer = 15000;

					if(MultiPlayer)
					{
					}
					return(1);
				}
			}
		}
	}
	else
	{
		fworkvar = frand() * 20000;
		iworkvar = target->AI.iSkill * 6000;
		radardat->lRLockTimer = 10000 + fworkvar + iworkvar;
	}

	return(0);
}

//*****************************************************************************************************************************************
float GetCrossSectionFromGround(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange)
{
	float Alt,Velocity, Distance;
	float RangeAdj, VelAdj, AltAdj, AvgAdj,MaxRange;

	if(orgMaxRange > 0)
	{
		MaxRange = orgMaxRange;
	}
	else
	{
		MaxRange = 80.0;   // JLM get max range from database
	}

	Alt      = DeltaAlt;
	Velocity = fabs(RelVel);
	Distance = TargetDist;

	if(Prf != LPRF)
	{
		if(Prf == HPRF)
		{
			if(Velocity > 1500.0)
				Velocity = 1500.0;
			VelAdj = (Velocity/1500.0)*3.5;
		} 
		else if(Prf == MPRF)
		{
			if(Distance > MaxRange)
				Distance = MaxRange;
	//		float Modifier = (Prf == MPRF) ? 0.75 : 0.50;
			float Modifier = 1.00f;
			float ModifiedMaxRange = MaxRange * Modifier;
			RangeAdj = ((ModifiedMaxRange - Distance)/ModifiedMaxRange)*2.0;
		}
		else
			return(0);

		if(Alt > 20000)
			Alt = 20000;
		else if(Alt < -20000) Alt = -20000;

		Alt += 20000;
		AltAdj   = (Alt/40000)*((Prf == LPRF) ? 3.0 : 2.0);

		if(Prf == HPRF)
			AvgAdj = (VelAdj + AltAdj)/2.0;
		else
			AvgAdj = (RangeAdj + AltAdj)/2.0;
	}
	else
	{
		if(Alt < 1000)
		{
			AltAdj = 0.1f;
		}
		else if(Alt < 5000)
		{
			AltAdj = 0.25f;
		}
		else if(Alt < 10000)
		{
			AltAdj = 0.75f;
		}
		else if(Alt < 20000)
		{
			AltAdj = 1.0f;
		}
		else if(Alt < 30000)
		{
			AltAdj = 1.25f;
		}
		else
		{
			AltAdj = 1.5f;
		}
		AvgAdj = AltAdj;
	}

  float Additive = 0;
  if(TargetDist <= MaxRange*0.1)
  {
    if( (TargetDist > MaxRange*0.05))
	    Additive = 1;
	  else if( (TargetDist > MaxRange*0.01) && (TargetDist <= MaxRange*0.05) )
	   Additive = 2;
	  else
	   Additive = 3;
  }
  else if(TargetDist >= MaxRange*0.65)
  {
    if( (TargetDist >= MaxRange*0.91))
	    Additive = -3;
	  else if(TargetDist >= MaxRange*0.81)
	    Additive = -2;
	  else
	   Additive = -1;
  }

  int FinalCrossSignature = /*round(*/ (AvgAdj*(float)CrossSignature) + Additive /*)*/;

  return(FinalCrossSignature);
}


