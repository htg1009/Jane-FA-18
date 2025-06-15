/*****************************************************************************
*
*	airdam.cpp  --  F18 damage aircraft code
*
*
*-----------------------------------------------------------------------------
*	Copyright (c) 1997 by Origin Systems, Inc., All Rights Reserved.
*****************************************************************************/

#include "F18.h"
#include "resources.h"
#include "GameSettings.h"
#include "spchcat.h"
#include "particle.h"
#include "3dfxF18.h"
#include "snddefs.h"
#include "flightfx.h"
#include "seatstf.h"

extern AvionicsType Av;
extern BYTE WarHeadTypeConversion[];

extern int g_iBaseBettySoundLevel;
extern int g_iSoundLevelExternalSFX;
extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)

extern int g_iNumWeaponReleasedInAreaGoal;
extern int g_iNumAroundGoalObjectsHit;
extern int g_iNumWeaponAreaGoal;
extern int g_iNumWeaponAreaGoalHit;

int AreaGoalExists(WeaponParams *W);
int WeaponAreaGoalExists(WeaponParams *W);

void camera_mode_fixed_chase(VKCODE vk);
extern void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition);
extern void ShutLeftEngineDown(void);
extern void ShutRightEngineDown(void);

void LogWeaponHit(WeaponParams *W,int iVehicle=0);
void LogDestroyedObject(WeaponParams *W,int iType,int iID);
extern void LogMissionStatisticKill(int iAircraftIndex, int iType, long lObjectID);


void PlaneHitGround( FPointDouble &pos, FPointDouble &vel, float fuel,FPoint *vector = NULL,void *vehicle = NULL);


extern void LogEventPlaneHit(PlaneParams *P,WeaponParams *W,DWORD damage_type);
extern void LogEventPlaneDamage(PlaneParams *P,WeaponParams *W,DWORD damage_type);
extern void LogEventPlaneDestroyed(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane);
extern void LogEventGroundObjectDestroyed(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID);
extern void LogEventVehicleDestroyed(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID,int iVehicleType,int iVehicleID);
extern void LogEventVehicleDamaged(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID,int iVehicleType,int iVehicleID);
extern void LogEventGroundObjectDamaged(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID);

extern void NetSendLogDamageOrDestroy(PlaneParams *planepnt, MovingVehicleParams *vehiclepnt, WeaponParams *W, int damage_type, int destroy);

extern void DisplayInitText(char *sTxt, int SkipLine, int showrelease=0);
extern int g_iSoundLevelCaution;

int iNetBulletHit = 0;

/*----------------------------------------------------------------------------
 *
 *	DoExplosionInAir()
 *
 */
void DoExplosionInAir(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type, PlaneParams *skipplane)
{
	float fBlastRadius=0;
	float fDamageValue;
	float fDistance;
	float fMultiplier;
	float fCheckDistance;
	DWORD hit_value;
	PlaneParams *checkplane;
	FPointDouble relative_position;
	FMatrix de_rotate;
	long tempdamage,tempstatus ,tempflight;

	if (pweapon)
	{
		do_big_bang = (BOOL)(pweapon->Flags & (BOMB_INUSE | MISSILE_INUSE));
		position = &pweapon->Pos;
		if ((int)pweapon->Type < 0x200)
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)pweapon->Type)];
		else
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex(pweapon->Type->TypeNumber)];
	}

	fBlastRadius = (pweapon_type->iDamageRadius * FTTOWU);  // Feet

	if((fBlastRadius) && (pweapon_type))
	{
		fCheckDistance = fBlastRadius;

		fMultiplier = ((float)pweapon_type->iDamageValue) / (fBlastRadius * fBlastRadius);

		checkplane = Planes;
		while(checkplane <= LastPlane) // && (!blew_something_up))
		{
			if((checkplane != skipplane) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
			{
				fDistance = checkplane->WorldPosition - *position;

				//  Added so that we take the size of the plane into account for hits
				//  We were getting solid hit that weren't doing any damage and I think
				//  it was since we weren't subtracting the plane size from the hit distance.  SRE
				fDistance -= (float)checkplane->Type->ShadowLRZOff;
				if(fDistance < 0)
				{
					fDistance = 0;
				}

				if(fDistance < fCheckDistance)
				{
					hit_value = 0;

					if (fDistance < fBlastRadius)
					{

						fDamageValue = fBlastRadius - fDistance;
						fDamageValue = fDamageValue * fDamageValue * fMultiplier;

						hit_value |= (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];

						relative_position.MakeVectorToFrom(*position,checkplane->WorldPosition);

						de_rotate = checkplane->Attitude;
			  			de_rotate.Transpose();

						relative_position *= de_rotate;
						relative_position.Normalize();

						tempdamage = checkplane->DamageFlags;
						tempstatus = checkplane->SystemsStatus;
						tempflight = checkplane->FlightStatus;


						//Not quite working right
						DamagePlane(checkplane,hit_value,GameLoop,fDamageValue, relative_position, fBlastRadius, pweapon, pweapon_type);
						if( tempdamage != checkplane->DamageFlags ||
						  ((tempstatus & PL_ENGINES ) != (checkplane->DamageFlags & PL_ENGINES))||
						  ((tempflight & (PL_STATUS_CRASHED| PL_PLANE_BLOWN_UP |PL_OUT_OF_CONTROL)) != (checkplane->FlightStatus &(PL_STATUS_CRASHED| PL_PLANE_BLOWN_UP |PL_OUT_OF_CONTROL))) )
						{
							FPoint tmp(0.0f);// = checkplane->IfVelocity;
							NewCanister( CT_FLYING_PARTS, checkplane->WorldPosition, tmp, 0.0f );
						}

					}
				}
			}
			checkplane ++;
		}
	}
}

void LogPlaneShotdown(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type,int iCrashed);
void LogPlaneDamage(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type);

extern void ProximitySoundHit(DBWeaponType *pweapon_type);


/*----------------------------------------------------------------------------
 *
 *	DamagePlane()
 *
 */

void OrphanAllPlaneSmoke(PlaneParams *P)
{
	void **swalker,**last_smoke_trail;

	swalker  = &P->Smoke[0];
	last_smoke_trail = &P->Smoke[MAX_PLANE_SMOKE_TRAILS];

	while(swalker < last_smoke_trail)
	{
		if (*swalker)
			((SmokeTrail *)*swalker)->MakeOrphan();
		swalker++;
	}

}

void DamagePlane(PlaneParams *planepnt, DWORD damage_type, DWORD game_loop, float damage, FPointDouble relative_position, float damage_radius, WeaponParams *W, DBWeaponType *pweapon_type)
{
	int directhit = 0;
	int target_type;
	DamageProportions *ratio;
	float heading, pitch;
	double dworkval;
	long tempval;
	int orgflightstat, failed_system;
	long weaponflags = 0;
	long orgdflags, workdflags;
	int cnt;
	int isbullet = 0;

	if(W)
	{
		if(W->P)
		{
			iWeaponFiredBy = W->P - Planes;
		}
	}

	if ((iInJump) && ((planepnt == PlayerPlane) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP)))
	{
		iWeaponFiredBy = -1;
		return;
	}

#if 0
	void NetPutStrike( PlaneParams * );
	if (MultiPlayer)  
	{
		if((damage_type & DTE_DIRECT_HIT) && (pweapon_type))
		{
			if((planepnt->Status & AL_DEVICE_DRIVEN) || (planepnt->Status & AL_COMM_DRIVEN) || (planepnt->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED))
			{
				if ((pweapon_type->iWeaponType == WEAPON_TYPE_GUN) && (pweapon_type->iSeekerType!=10))
				{
					iNetBulletHit = 1;
				}
			}
		}
		NetPutStrike( planepnt );
		iNetBulletHit = 0;
	}
#endif

	orgflightstat = planepnt->FlightStatus;
	orgdflags = planepnt->DamageFlags;
	if(planepnt == PlayerPlane)
	{
		for(cnt = 0; cnt < 32; cnt ++)
		{
			if((cnt != WARN_CHAFF) && (cnt != WARN_SPARE1) && (cnt != WARN_FUEL_LOW) && (cnt != WARN_MINIMUM) && (cnt != WARN_FLARE) && (cnt != WARN_SPARE2))
			{
				orgdflags |= (1<<cnt);
			}
		}
	}

	target_type = DTE_SOFT_TARGET;
	target_type |= (damage_type & DTE_TARGET_TYPE) << 2;

	ratio = &DamageMultipliers[target_type << 1];

	if (damage_type & DTE_DIRECT_HIT)
	{
		ratio++;
		directhit = 1;
		heading = ADCheckSpecificHit(planepnt, W, pweapon_type, &pitch, relative_position);
		if(heading <= -999)
		{
			dworkval = (planepnt->Orientation.K * relative_position);
			tempval = rand() & 1023;
			if(dworkval <= 0.0f)
			{
				heading = ((float)tempval / 1024.0f) * -180.0f;
			}
			else
			{
				heading = ((float)tempval / 1024.0f) * 180.0f;
			}
			pitch = 0;
		}
		if (planepnt == PlayerPlane)
		{
			if (pweapon_type)
			{
//				if ((pweapon_type->iSeekerType==8) || (pweapon_type->iSeekerType==9) || ((pweapon_type->iWeaponType == WEAPON_TYPE_GUN) && (pweapon_type->iSeekerType!=10)))
				if ((pweapon_type->iWeaponType == WEAPON_TYPE_GUN) && (pweapon_type->iSeekerType!=10))
				{
					isbullet = 1;
					int iLevel = g_iSoundLevelExternalSFX-10;
					if (iLevel<0) iLevel=0;
					if (iLevel)
					{
						SndQueueSound(Implgin2,1,iLevel,-1,-1,64);
					}
				}
			}
		}
	}
	else
	{
		heading = atan2(-relative_position.X,-relative_position.Z) * 57.2958;
		pitch = asin(relative_position.Y) * 57.2958;

		if (planepnt == PlayerPlane)
		{
			if (pweapon_type)
			{
				ProximitySoundHit(pweapon_type);
			}
		}
	}

	if(W)
	{
		weaponflags = W->Flags;

		if (pweapon_type)
		{
			if(pweapon_type->iWeaponType != WEAPON_TYPE_GUN)
			{
				weaponflags |= WEP_FIRST_CHECK;
			}
		}
		if((W->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE)))
		{
			weaponflags |= WEP_SPREAD_DAMAGE;
		}
	}

	if((pSeatData) && (planepnt == PlayerPlane))
	{
		if(pweapon_type)
		{
			pSeatData->DamageType = pweapon_type->iWeaponType;
		}
		else
		{
			pSeatData->DamageType = 101;
		}

		pSeatData->DamageDone = damage;
		pSeatData->Heading = heading;
		pSeatData->Pitch = pitch;
	}

	if((planepnt != PlayerPlane) || (cPlayerVulnerable) || (MultiPlayer))
	{
		DistributeDamage(planepnt, damage, ratio, pitch, heading, damage_radius, weaponflags);
		if(planepnt->AI.lPlaneID == 61)
		{
			if(fabs(heading) > 150)
			{
				if((damage / 25.0f) > frand())
				{
					ADDamageTowedDecoy(planepnt);
				}
			}
		}
	}

	failed_system = planepnt->FlightStatus;

	if(planepnt == PlayerPlane)
	{
		int delaycnt = 3000;
		if(isbullet)
		{
			delaycnt = 1500;
		}

		AICAddSoundCall(AICDelayedPlayerDamageMsgs, ((~orgflightstat) & failed_system), delaycnt, 50, pweapon_type - pDBWeaponList);
	}
	else
	{
		AICCheckForDamageMsgs(planepnt, orgflightstat, failed_system, W, NULL, pweapon_type);
	}

	if((planepnt == PlayerPlane) && (!(orgflightstat & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		workdflags = planepnt->DamageFlags & ~(orgdflags);
		AIC_WSO_ReportDamage(planepnt - Planes, workdflags);
	}

	if ((!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		LogPlaneDamage(planepnt,W,damage_type);
	}

	if ((!(orgflightstat & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		if (planepnt->FlightStatus & (PL_OUT_OF_CONTROL)) //|PL_STATUS_CRASHED))
		{
			LogPlaneShotdown(planepnt,W,damage_type,0);
		}
	}

	if (planepnt->FlightStatus & PL_OUT_OF_CONTROL)
	{
		AIRemoveFromFormation(planepnt);
	}

	iWeaponFiredBy = -1;
}

/*----------------------------------------------------------------------------
 *
 *	GetPlaneItemArmor()
 *
 */
float GetPlaneItemArmor(PlaneParams *planepnt, long itemnum)
{
	return(5.0f);
}

/*----------------------------------------------------------------------------
 *
 *	GetPlaneItemMaxDamage()
 *
 */
float GetPlaneItemMaxDamage(PlaneParams *planepnt, long itemnum)
{
	return(5.0f);
}

/*----------------------------------------------------------------------------
 *
 *	DelayDestroyPlaneItem()
 *
 */
void DelayDestroyPlaneItem(PlaneParams *planepnt, long itemid, float destroyedperc)
{
	int cnt;
	int leastcnt;
	long leasttime;
	float invdestperc;

	invdestperc = (1.0f - destroyedperc) * 100.0f;

	for(cnt = 0; cnt < MAX_DELAYED_DAMAGE; cnt ++)
	{
		if(gDamageEvents[cnt].lDamageTimer < 0)
		{
			gDamageEvents[cnt].iPlaneNum = planepnt - Planes;
			gDamageEvents[cnt].lDamageTimer = (invdestperc * invdestperc) * 25;  // * 100 / 4
			gDamageEvents[cnt].lDamagedSystem = itemid;
			return;
		}
	}

	leastcnt = 0;
	leasttime = gDamageEvents[0].lDamageTimer;
	for(cnt = 1; cnt < MAX_DELAYED_DAMAGE; cnt ++)
	{
		if(gDamageEvents[cnt].lDamageTimer < leasttime)
		{
			leastcnt = cnt;
			leasttime = gDamageEvents[cnt].lDamageTimer;
		}
	}

	DestroyPlaneItem(&Planes[gDamageEvents[leastcnt].iPlaneNum], gDamageEvents[leastcnt].lDamagedSystem, -1);

	gDamageEvents[leastcnt].iPlaneNum = planepnt - Planes;
	gDamageEvents[leastcnt].lDamageTimer = (invdestperc * invdestperc) * 25;
	gDamageEvents[leastcnt].lDamagedSystem = itemid;
}

/*----------------------------------------------------------------------------
 *
 *	CheckDelayedDestruction()
 *
 */
void CheckDelayedDestruction()
{
	int cnt;
	int orgdflags, workdflags;
	int orgflightstat;
	long tempdamage,tempstatus ,tempflight;
	PlaneParams *checkplane;

	orgdflags = PlayerPlane->DamageFlags;
	for(cnt = 0; cnt < 32; cnt ++)
	{
		if((cnt != WARN_CHAFF) && (cnt != WARN_SPARE1) && (cnt != WARN_FUEL_LOW) && (cnt != WARN_MINIMUM) && (cnt != WARN_FLARE) && (cnt != WARN_SPARE2))
		{
			orgdflags |= (1<<cnt);
		}
	}

	orgflightstat = PlayerPlane->FlightStatus;

	for(cnt = 0; cnt < MAX_DELAYED_DAMAGE; cnt ++)
	{
		if(gDamageEvents[cnt].lDamageTimer >= 0)
		{
			gDamageEvents[cnt].lDamageTimer -= DeltaTicks;

			if(gDamageEvents[cnt].lDamageTimer < 0)
			{
				checkplane = &Planes[gDamageEvents[cnt].iPlaneNum];
				tempdamage = checkplane->DamageFlags;
				tempstatus = checkplane->SystemsStatus;
				tempflight = checkplane->FlightStatus;

				DestroyPlaneItem(checkplane, gDamageEvents[cnt].lDamagedSystem, -1);

				if( tempdamage != checkplane->DamageFlags ||
				  ((tempstatus & PL_ENGINES ) != (checkplane->DamageFlags & PL_ENGINES))||
				  ((tempflight & (PL_STATUS_CRASHED| PL_PLANE_BLOWN_UP |PL_OUT_OF_CONTROL)) != (checkplane->FlightStatus &(PL_STATUS_CRASHED| PL_PLANE_BLOWN_UP |PL_OUT_OF_CONTROL))) )
				{
					FPoint tmp(0.0f);// = checkplane->IfVelocity;
					NewCanister( CT_FLYING_PARTS, checkplane->WorldPosition, tmp, 0.0f );
				}

				if(checkplane != PlayerPlane)
				{
					AICCheckForDamageMsgs(checkplane, tempflight, checkplane->FlightStatus, NULL, NULL, NULL);
				}
			}
		}
	}

	if((PlayerPlane->DamageFlags != orgdflags) && (!(orgflightstat & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		workdflags = PlayerPlane->DamageFlags & ~(orgdflags);
		AIC_WSO_ReportDamage(PlayerPlane - Planes, workdflags);
	}
}

/*----------------------------------------------------------------------------
 *
 *	ConvertAIDamageToFlag()
 *
 */
long ConvertAIDamageToFlag(int hititem)
{
	switch(hititem)
	{
		case AI_ENGINE_LO:
			return(DAMAGE_LO_ENGINE);
			break;
		case AI_ENGINE_LI:
			return(DAMAGE_L_ENGINE);
			break;
		case AI_ENGINE_RI:
			return(DAMAGE_R_ENGINE);
			break;
		case AI_ENGINE_RO:
			return(DAMAGE_RO_ENGINE);
			break;
		case AI_RADAR_SLOT:
			return(DAMAGE_RADAR);
			break;
		case AI_HYDRAULICS:
			return(DAMAGE_EMER_HYD);
			break;
		case AI_FUEL_SLOT:
			return(DAMAGE_FUEL_TANKS);
			break;
		case AI_GUN_SLOT:
			return(DAMAGE_GUN);
			break;
		case AI_WEAPON_SLOT:
			return(DAMAGE_PACS);
			break;
		case AI_REAR_GUN:
			return(DAMAGE_GUN);
			break;
		default:
			return(DAMAGE_GUN);
			break;
	}
	return(DAMAGE_GUN);
}

/*----------------------------------------------------------------------------
 *
 *	DestroyPlaneItem()
 *
 */
void DestroyPlaneItem(PlaneParams *planepnt, long itemid, int critical)
{
	int cnt;
	int weapontype;
	FPoint vel;
	long enginelo, engineli, enginero, engineri, allengines;
	long warnflags = 0;
	long checkhyd = 0;
	long mhyd;
	long turnoffflags = 0;
	int orgflightstat = planepnt->FlightStatus;

	if(planepnt->DamageFlags & (1<<itemid))
	{
		return;
	}

	// Broadcast ALL damage done by PlayerPlane weapons.  NetWork prevents recursion.
	void NetPutDamage( PlaneParams *planepnt, long itemid, int critical );
	if (MultiPlayer)
		NetPutDamage( planepnt, itemid, critical );

	if(critical)
	{
		vel.MakeVectorToFrom(planepnt->WorldPosition,planepnt->LastWorldPosition);
	}

	switch(itemid)
	{
		case DAMAGE_L_BLEED_AIR:
			warnflags |= (1<<itemid);
			if(planepnt == PlayerPlane)
				ADSetUpPlayerHeatUp(1, 600000);
			if (planepnt==PlayerPlane)
			{
				if(!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						for(cnt = 0; cnt < 2; cnt ++)
						{
							SndQueueSpeech (SND_BETTY_BLEED_AIR, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_LEFT, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
						}
						SndServiceSound();
					}
				}
			}
			break;
		case DAMAGE_L_OIL_PRESS:
			warnflags |= (1<<itemid);
			if(planepnt == PlayerPlane)
				ADSetUpPlayerHeatUp(1, 300000);
			break;
		case DAMAGE_R_BLEED_AIR:
			warnflags |= (1<<itemid);
			if(planepnt == PlayerPlane)
				ADSetUpPlayerHeatUp(0, 600000);
			if (planepnt==PlayerPlane)
			{
				if(!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						for(cnt = 0; cnt < 2; cnt ++)
						{
							SndQueueSpeech (SND_BETTY_BLEED_AIR, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_RIGHT, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
						}
						SndServiceSound();
					}
				}
			}
			break;
		case DAMAGE_R_OIL_PRESS:
			warnflags |= (1<<itemid);
			if(planepnt == PlayerPlane)
				ADSetUpPlayerHeatUp(0, 300000);
			break;
		case DAMAGE_L_GEN:
			warnflags |= (1<<itemid);
			if(planepnt == PlayerPlane)
				ADSetUpPlayerHeatUp(1, 3600000, 1);
			break;
		case DAMAGE_R_GEN:
			warnflags |= (1<<itemid);
			if(planepnt == PlayerPlane)
				ADSetUpPlayerHeatUp(0, 3600000, 1);
			break;
		case DAMAGE_FLT_CONT:
			if (planepnt==PlayerPlane)
			{
				if(!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						SndQueueSpeech (SND_BETTY_FLIGHT_CONTROLS, CHANNEL_BETTY,g_iBaseBettySoundLevel,2);
						SndServiceSound();
					}
				}
			}
			warnflags |= (1<<itemid);
		break;
		case DAMAGE_L_ENG_CONT:
		case DAMAGE_TARGET_IR:
		case DAMAGE_CEN_COMP:
		case DAMAGE_ADC:
		case DAMAGE_OXYGEN:
		case DAMAGE_R_ENG_CONT:
		case DAMAGE_AUTOPILOT:
		case DAMAGE_NAV_FLIR:
		case DAMAGE_TEWS:
		case DAMAGE_HUD:
			warnflags |= (1<<itemid);
		break;
		case DAMAGE_R_BURNER:
			warnflags |= (1<<itemid);
		break;
		case DAMAGE_L_BURNER:
			warnflags |= (1<<itemid);
		break;
		case DAMAGE_R_FUEL_PUMP:
		case DAMAGE_L_FUEL_PUMP:
			warnflags |= (1<<itemid);
		break;
		case DAMAGE_L_MAIN_HYD:
			checkhyd = 1;
			break;
		case DAMAGE_PACS:
			if((planepnt != PlayerPlane) && (!MultiPlayer))
			{
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					if(planepnt->WeapLoad[cnt].Count > 0)
					{
						weapontype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
						if((weapontype != WEAPON_TYPE_GUN) && (weapontype != WEAPON_TYPE_FLARE) && (weapontype != WEAPON_TYPE_CHAFF) && (weapontype != WEAPON_TYPE_FUEL_TANK))
						{
							planepnt->WeapLoad[cnt].Count = 0;
						}
					}
				}
			}
			else
			{
				warnflags |= (DAMAGE_BIT_PACS);
			}
			break;
		case DAMAGE_RADAR:
			planepnt->AI.iAIFlags1 &= ~AIRADARON;
			warnflags |= (DAMAGE_BIT_RADAR);
			break;
		case DAMAGE_LO_ENGINE:
			if(planepnt->Type->EngineConfig == FOUR_WING_ENGINES)
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD])
					 	((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD])->MakeOrphan();

					planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_WING_LEFT_OUTBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->SystemsStatus |= PL_ENGINE_WING_LEFT_OUTBOARD;
					planepnt->EngineFires[SMOKE_ENGINE_WING_LEFT_OUTBOARD].TotalTimeLeft = (5+rand()&7)<<6;
					NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
				}
			}
			break;
		case DAMAGE_L_ENGINE:
			if (planepnt==PlayerPlane)
			{
				if(!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						for(cnt = 0; cnt < 2; cnt ++)
						{
							SndQueueSpeech (SND_BETTY_ENGINE, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_LEFT, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
						}
						SndServiceSound();
					}
				}
			}

			if(planepnt->Type->EngineConfig == ONE_REAR_ENGINE)
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_REAR] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_REAR])
					 ((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR])->MakeOrphan();

					planepnt->Smoke[SMOKE_ENGINE_REAR] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->EngineFires[SMOKE_ENGINE_REAR].TotalTimeLeft = (5+rand()&7)<<6;
				}
				planepnt->SystemsStatus |= PL_ENGINE_REAR;
				NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
				planepnt->DamageFlags |= (DAMAGE_BIT_R_ENGINE)|(DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_LO_ENGINE);
				if(planepnt == PlayerPlane)
				{
					turnoffflags |= (DAMAGE_BIT_R_ENGINE)|(DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_LO_ENGINE)|(DAMAGE_BIT_L_ENG_CONT)|(DAMAGE_BIT_R_ENG_CONT)|(DAMAGE_BIT_L_OIL_PRESS)|(DAMAGE_BIT_R_OIL_PRESS)|(DAMAGE_BIT_L_BURNER)|(DAMAGE_BIT_R_BURNER)|(DAMAGE_BIT_L_FUEL_PUMP)|(DAMAGE_BIT_R_FUEL_PUMP)|(DAMAGE_BIT_L_GEN)|(DAMAGE_BIT_R_GEN|(DAMAGE_BIT_L_GEN)|(DAMAGE_BIT_R_GEN));
					warnflags |= (1<<WARN_L_ENG_CONT)|(1<<WARN_R_ENG_CONT)|(1<<WARN_L_OIL_PRESS)|(1<<WARN_R_OIL_PRESS)|(1<<WARN_L_BURNER)|(1<<WARN_R_BURNER)|(1<<WARN_L_FUEL_PUMP)|(1<<WARN_R_FUEL_PUMP)|(1<<WARN_L_GEN)|(1<<WARN_R_GEN);
				}
			}
			else if(planepnt->Type->EngineConfig == TWO_REAR_ENGINES)
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT])
					 	((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT])->MakeOrphan();

//					if (!planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (*((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT]) == BROWN_ENGINE_SMOKE))
//						planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR_LEFT,ENGINE_FIRE,PLANE_BURNING);
//					else
						planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR_LEFT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = (5+rand()&7)<<6;
				}
				planepnt->SystemsStatus |= PL_ENGINE_REAR_LEFT;
				NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
				planepnt->DamageFlags |= (DAMAGE_BIT_LO_ENGINE);
				if(planepnt == PlayerPlane)
				{
					turnoffflags |= (DAMAGE_BIT_LO_ENGINE)|(DAMAGE_BIT_L_ENG_CONT)|(DAMAGE_BIT_L_OIL_PRESS)|(DAMAGE_BIT_L_BURNER)|(DAMAGE_BIT_L_FUEL_PUMP)|(DAMAGE_BIT_L_GEN);
					warnflags |= (1<<WARN_L_ENG_CONT)|(1<<WARN_L_OIL_PRESS)|(1<<WARN_L_BURNER)|(1<<WARN_L_FUEL_PUMP)|(1<<WARN_L_GEN);
				}
			}
			else
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD])
					 	((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD])->MakeOrphan();

					planepnt->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_WING_LEFT_INBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->SystemsStatus |= PL_ENGINE_WING_LEFT_INBOARD;
					if(planepnt->Type->EngineConfig == TWO_WING_ENGINES)
					{
						planepnt->DamageFlags |= (DAMAGE_BIT_LO_ENGINE);
					}

					planepnt->EngineFires[SMOKE_ENGINE_WING_LEFT_INBOARD].TotalTimeLeft = (5+rand()&7)<<6;
					NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
				}
			}
//			warnflags |= (DAMAGE_BIT_L_ENGINE);

			break;
		case DAMAGE_EMER_HYD:
			if(planepnt != PlayerPlane)
			{
				planepnt->MaxRollRate *= 0.5;
				planepnt->YawRate *= 0.5;
			}
			checkhyd = 1;
			break;
		case DAMAGE_GUN:
			if((planepnt != PlayerPlane) && (!MultiPlayer))
			{
				for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
				{
					if(planepnt->WeapLoad[cnt].Count > 0)
					{
						weapontype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
						if(weapontype == WEAPON_TYPE_GUN)
						{
							planepnt->WeapLoad[cnt].Count = 0;
						}
					}
				}
			}
			else
			{
				warnflags |= (DAMAGE_BIT_GUN);
			}
			break;
		case DAMAGE_FUEL_TANKS:
//			warnflags |= (DAMAGE_BIT_FUEL_TANKS);
			break;
		case DAMAGE_R_MAIN_HYD:
			checkhyd = 1;
			break;
		case DAMAGE_RO_ENGINE:
			if(planepnt->Type->EngineConfig == FOUR_WING_ENGINES)
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD])
					 	((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD])->MakeOrphan();

					planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_WING_RIGHT_OUTBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->SystemsStatus |= PL_ENGINE_WING_RIGHT_OUTBOARD;
					planepnt->EngineFires[SMOKE_ENGINE_WING_RIGHT_OUTBOARD].TotalTimeLeft = (5+rand()&7)<<6;
					NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
				}
			}
			break;
		case DAMAGE_R_ENGINE:
			if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD])
					 	((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD])->MakeOrphan();

					planepnt->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_WING_RIGHT_INBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->SystemsStatus |= PL_ENGINE_WING_RIGHT_INBOARD;
					if(planepnt->Type->EngineConfig == TWO_WING_ENGINES)
					{
						planepnt->DamageFlags |= (DAMAGE_BIT_RO_ENGINE);
					}
					planepnt->EngineFires[SMOKE_ENGINE_WING_RIGHT_INBOARD].TotalTimeLeft = (5+rand()&7)<<6;
					NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
				}
//				warnflags |= (DAMAGE_BIT_R_ENGINE);
			}
			else if(planepnt->Type->EngineConfig == TWO_REAR_ENGINES)
			{
				if (!planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					 	((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT])->MakeOrphan();

//					if (!planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT] || (*((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT]) == BROWN_ENGINE_SMOKE))
//						planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR_RIGHT,ENGINE_FIRE,PLANE_BURNING);
//					else
						planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR_RIGHT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
					planepnt->SystemsStatus |= PL_ENGINE_REAR_RIGHT;
					planepnt->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = (5+rand()&7)<<6;
					NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,vel,2.0f,25);
					planepnt->DamageFlags |= (DAMAGE_BIT_RO_ENGINE);
				}
				if(planepnt == PlayerPlane)
				{
					if(!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
					{
						if (g_iBaseBettySoundLevel)
						{
							for(cnt = 0; cnt < 2; cnt ++)
							{
								SndQueueSpeech (SND_BETTY_ENGINE, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
								SndQueueSpeech (SND_BETTY_RIGHT, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							}
							SndServiceSound();
						}
					}

					turnoffflags |= (DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_R_ENG_CONT)|(DAMAGE_BIT_R_OIL_PRESS)|(DAMAGE_BIT_R_BURNER)|(DAMAGE_BIT_R_FUEL_PUMP)|(DAMAGE_BIT_R_GEN);
					warnflags |= (1<<WARN_R_ENG_CONT)|(1<<WARN_R_OIL_PRESS)|(1<<WARN_R_BURNER)|(1<<WARN_R_FUEL_PUMP)|(1<<WARN_R_GEN);
				}
//				warnflags |= (DAMAGE_BIT_R_ENGINE);
			}
			break;
	}
	planepnt->DamageFlags |= (1<<itemid);

	if(planepnt == PlayerPlane)
	{
		planepnt->SystemInactive |= turnoffflags | planepnt->DamageFlags;
		for(cnt = 0; cnt < 32; cnt ++)
		{
			if(warnflags & (1<<cnt))
			{
				DisplayWarning(MASTER_CAUTION, ON, 0);
				SetMasterWarning(cnt);
			}
		}
		if(checkhyd)
		{
			mhyd = DAMAGE_BIT_L_MAIN_HYD | DAMAGE_BIT_R_MAIN_HYD;
			if((planepnt->DamageFlags & mhyd) == mhyd)
			{
				SetMasterWarning(WARN_MAIN_HYD);
				if(planepnt->DamageFlags & DAMAGE_BIT_EMER_HYD)
				{
					DisplayWarning(MASTER_CAUTION, ON, 0);
					SetMasterWarning(WARN_ENER_HYD);
				}
			}
		}
	}

	if(critical > 1)
	{
		if(planepnt != PlayerPlane)
		{
			if(frand() < 0.25f)
			{
				planepnt->FlightStatus |= (PL_OUT_OF_CONTROL|PL_PLANE_BLOWN_UP);
			}
			else
			{
				planepnt->FlightStatus |= (PL_OUT_OF_CONTROL);
			}
		}
		else if((frand() < 0.05f) && (g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))
		{
			planepnt->FlightStatus |= (PL_OUT_OF_CONTROL|PL_PLANE_BLOWN_UP);
		}
		else
		{
			planepnt->FlightStatus |= (PL_OUT_OF_CONTROL);
		}
	}
	else
	{
		enginelo = DAMAGE_BIT_LO_ENGINE;
		engineli = DAMAGE_BIT_L_ENGINE;
		enginero = DAMAGE_BIT_RO_ENGINE;
		engineri = DAMAGE_BIT_R_ENGINE;
		allengines = enginelo | engineli | enginero | engineri;
		if((planepnt->DamageFlags & allengines) == allengines)
		{
			planepnt->FlightStatus |= PL_OUT_OF_CONTROL;
		}
	}

	if((planepnt == PlayerPlane) && (MultiPlayer))
	{
		if((lNetSpeechTimer < 0) || ((orgflightstat & PL_OUT_OF_CONTROL) != (planepnt->FlightStatus & PL_OUT_OF_CONTROL)))
		{
			AICAddSoundCall(AICDelayedPlayerDamageMsgs, ((~orgflightstat) & planepnt->FlightStatus), 2000, 50, NULL);
			lNetSpeechTimer = 10000;
		}
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3))
	{
		if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TANKER))
		{
		}

		AISendGroupHome(planepnt);
	}
}

/*----------------------------------------------------------------------------
 *
 *	DamageAIPlaneItem()
 *
 */
float DamageAIPlaneItem(PlaneParams *planepnt, int hititem, float startdamage, float add_to_total, float our_max_total, int big_boom)
{
	float tempdamage = startdamage;
	float fworkval;
	float randperc;
	long ltempval;
	long itemid;
	int critical;

	tempdamage += add_to_total;
	if(tempdamage >= our_max_total)
	{
		tempdamage = our_max_total;
		itemid = ConvertAIDamageToFlag(hititem);
		if(add_to_total > our_max_total)
		{
			critical = 1;
			if(big_boom)
			{
				critical ++;
			}
		}
		else
		{
			critical = 0;
		}

		DestroyPlaneItem(planepnt, itemid, critical);
	}
	else
	{
		ltempval = rand() & 1023;
		randperc = (float)ltempval / 1024;
		fworkval = tempdamage / our_max_total;
		if(fworkval > randperc)
		{
			itemid = ConvertAIDamageToFlag(hititem);
			DelayDestroyPlaneItem(planepnt, itemid, fworkval);
		}
	}

	return(tempdamage);
}

/*----------------------------------------------------------------------------
 *
 *	DamagePlayerPlaneItem()
 *
 */
float DamagePlayerPlaneItem(PlaneParams *planepnt, int hititem, float startdamage, float add_to_total, float our_max_total, int big_boom)
{
	float tempdamage = startdamage;
	float fworkval;
	float randperc;
	long ltempval;
	long itemid;
	int critical;

	tempdamage += add_to_total;
	if(tempdamage > our_max_total)
	{
		tempdamage = our_max_total;
		itemid = hititem;
		if(add_to_total > our_max_total)
		{
			critical = 1;
			if(big_boom)
			{
				critical ++;
			}
		}
		else
		{
			critical = 0;
		}

		DestroyPlaneItem(planepnt, itemid, critical);
	}
	else
	{
		ltempval = rand() & 1023;
		randperc = (float)ltempval / 1024;
		fworkval = tempdamage / our_max_total;
		if(fworkval < randperc)
		{
			itemid = hititem;
			DelayDestroyPlaneItem(planepnt, itemid, fworkval);
		}
	}

	return(tempdamage);
}

/*----------------------------------------------------------------------------
 *
 *	DistributeDamage()
 *
 */
void DistributeDamage(PlaneParams *planepnt, float damage, DamageProportions *ratio, float pitch, float heading, float damage_radius, long weaponflags)
{
	char possiblehits[AI_MAX_DAMAGE_SLOTS];
	int cnt;
	int numpossible = 0;
	int tempval;
//	float fworkval;
//	long lworkval;
	float add_to_sustain, add_to_total;
	float our_max_total,our_max_sustainable;
	float temparmor, tempdamage;
	int hititem;
	int bigboom = 0;
	float armorleft;

	if((planepnt == PlayerPlane) || (MultiPlayer && (!(planepnt->Status & PL_AI_DRIVEN))))
	{
		DistributePlayerDamage(planepnt, damage, ratio, pitch, heading, damage_radius, weaponflags);
		return;
	}

	for(cnt = 0; cnt < AI_MAX_DAMAGE_SLOTS; cnt ++)
	{
		possiblehits[cnt] = 0;
	}

	if(weaponflags & WEP_TRACK_LI)
	{
		possiblehits[numpossible] = AI_ENGINE_LI;
		numpossible = 1;
		if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
		{
			heading = 90;
		}
		else
		{
			heading = 180;
		}
	}
	else if(weaponflags & WEP_TRACK_LO)
	{
		possiblehits[numpossible] = AI_ENGINE_LO;
		numpossible = 1;
		if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
		{
			heading = 90;
		}
		else
		{
			heading = 180;
		}
	}
	else if(weaponflags & WEP_TRACK_RI)
	{
		possiblehits[numpossible] = AI_ENGINE_RI;
		numpossible = 1;
		if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
		{
			heading = -90;
		}
		else
		{
			heading = 180;
		}
	}
	else if(weaponflags & WEP_TRACK_RO)
	{
		possiblehits[numpossible] = AI_ENGINE_RO;
		numpossible = 1;
		if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
		{
			heading = -90;
		}
		else
		{
			heading = 180;
		}
	}
	else
	{
		switch (planepnt->Type->EngineConfig)
		{
			case ONE_REAR_ENGINE:
			case TWO_REAR_ENGINES:
				if(fabs(heading) < 45.0f)
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_RADAR)))
					{
						possiblehits[numpossible++] = AI_RADAR_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_GUN)))
					{
						possiblehits[numpossible++] = AI_GUN_SLOT;
					}
				}
				else if(fabs(heading) < 135.0f)
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_EMER_HYD)))
					{
						possiblehits[numpossible++] = AI_HYDRAULICS;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_FUEL_TANKS)))
					{
						possiblehits[numpossible++] = AI_FUEL_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_PACS)))
					{
						possiblehits[numpossible++] = AI_WEAPON_SLOT;
					}
				}
				else if(heading > -999)
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
					{
						possiblehits[numpossible++] = AI_ENGINE_LI;
					}
					if((planepnt->Type->EngineConfig == TWO_REAR_ENGINES) && (!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE))))
					{
						possiblehits[numpossible++] = AI_ENGINE_RI;
					}
					if((!(planepnt->DamageFlags & (DAMAGE_BIT_GUN))) && (planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1))
					{
						possiblehits[numpossible++] = AI_REAR_GUN;
					}
				}
				else
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_RADAR)))
					{
						possiblehits[numpossible++] = AI_RADAR_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_GUN)))
					{
						possiblehits[numpossible++] = AI_GUN_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_EMER_HYD)))
					{
						possiblehits[numpossible++] = AI_HYDRAULICS;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_FUEL_TANKS)))
					{
						possiblehits[numpossible++] = AI_FUEL_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_PACS)))
					{
						possiblehits[numpossible++] = AI_WEAPON_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
					{
						possiblehits[numpossible++] = AI_ENGINE_LI;
					}
					if((planepnt->Type->EngineConfig == TWO_REAR_ENGINES) && (!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE))))
					{
						possiblehits[numpossible++] = AI_ENGINE_RI;
					}
					if((!(planepnt->DamageFlags & (DAMAGE_BIT_GUN))) && (planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1))
					{
						possiblehits[numpossible++] = AI_REAR_GUN;
					}
				}
				break;
			case TWO_WING_ENGINES:
			case FOUR_WING_ENGINES:
				if(fabs(heading) < 45.0f)
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_GUN)))
					{
						possiblehits[numpossible++] = AI_GUN_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_EMER_HYD)))
					{
						possiblehits[numpossible++] = AI_HYDRAULICS;
					}
				}
				else if(fabs(heading) < 135.0f)
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_FUEL_TANKS)))
					{
						possiblehits[numpossible++] = AI_FUEL_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_PACS)))
					{
						possiblehits[numpossible++] = AI_WEAPON_SLOT;
					}
					if(heading > 0)
					{
						if(!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
						{
							possiblehits[numpossible++] = AI_ENGINE_LI;
						}
						if((planepnt->Type->EngineConfig == FOUR_WING_ENGINES) && (!(planepnt->DamageFlags & (DAMAGE_BIT_LO_ENGINE))))
						{
							possiblehits[numpossible++] = AI_ENGINE_LO;
						}
					}
					else
					{
						if(!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
						{
							possiblehits[numpossible++] = AI_ENGINE_RI;
						}
						if((planepnt->Type->EngineConfig == FOUR_WING_ENGINES) && (!(planepnt->DamageFlags & (DAMAGE_BIT_RO_ENGINE))))
						{
							possiblehits[numpossible++] = AI_ENGINE_RO;
						}
					}
				}
				else if(heading > -999)
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_RADAR)))
					{
						possiblehits[numpossible++] = AI_RADAR_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_PACS)))
					{
						possiblehits[numpossible++] = AI_WEAPON_SLOT;
					}
					if((!(planepnt->DamageFlags & (DAMAGE_BIT_GUN))) && (planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1))
					{
						possiblehits[numpossible++] = AI_REAR_GUN;
					}
				}
				else
				{
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_RADAR)))
					{
						possiblehits[numpossible++] = AI_RADAR_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_GUN)))
					{
						possiblehits[numpossible++] = AI_GUN_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_EMER_HYD)))
					{
						possiblehits[numpossible++] = AI_HYDRAULICS;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_FUEL_TANKS)))
					{
						possiblehits[numpossible++] = AI_FUEL_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_PACS)))
					{
						possiblehits[numpossible++] = AI_WEAPON_SLOT;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
					{
						possiblehits[numpossible++] = AI_ENGINE_LI;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
					{
						possiblehits[numpossible++] = AI_ENGINE_RI;
					}
					if((planepnt->Type->EngineConfig == FOUR_WING_ENGINES) && (!(planepnt->DamageFlags & (DAMAGE_BIT_LO_ENGINE))))
					{
						possiblehits[numpossible++] = AI_ENGINE_LO;
					}
					if((planepnt->Type->EngineConfig == FOUR_WING_ENGINES) && (!(planepnt->DamageFlags & (DAMAGE_BIT_RO_ENGINE))))
					{
						possiblehits[numpossible++] = AI_ENGINE_RO;
					}
					if((!(planepnt->DamageFlags & (DAMAGE_BIT_GUN))) && (planepnt->AI.cInteriorDamage[AI_REAR_GUN] != -1))
					{
						possiblehits[numpossible++] = AI_REAR_GUN;
					}
				}
				break;
		}
	}

	if(numpossible == 0)
	{
		if(heading > -999)
		{
			if(((planepnt->Type->ShadowLRZOff * 4.0f) <= (damage_radius)) || (weaponflags & (WEP_FIRST_CHECK|WEP_SPREAD_DAMAGE)))
			{
				DistributeDamage(planepnt, damage, ratio, pitch, -999, damage_radius, (weaponflags & WEP_SPREAD_DAMAGE));
			}
		}
		return;
	}

	tempval = rand() % numpossible;
	hititem = possiblehits[tempval];

	our_max_sustainable = GetPlaneItemArmor(planepnt, hititem);

	our_max_total = GetPlaneItemMaxDamage(planepnt, hititem);

	add_to_sustain = damage*ratio->SustainablePercentage;

//	temparmor = (float)planepnt->AI.cArmorDamage[hititem] / 10.0f;
	armorleft = (float)planepnt->AI.cArmorDamage[hititem] / 5.0f;
	temparmor = (our_max_sustainable > armorleft) ? our_max_sustainable - armorleft : 0;
	tempdamage = (float)planepnt->AI.cInteriorDamage[hititem] / 10.0f;

	add_to_total = (temparmor += add_to_sustain) - our_max_sustainable;

	if (add_to_total < 0.0f)
		add_to_total = 0.0f;
	else
		temparmor = our_max_sustainable;

	// then figure out how many points will go to total
	add_to_total += damage*ratio->TotalPercentage;

	if(add_to_total > 0.0f)
	{
		if(planepnt->AI.lPlaneID == 85)
		{
			NewGenerator(PLANE_EXPLODES,planepnt->WorldPosition,0.0,2.0f,50);
			if(MultiPlayer)
			{
				NetPutGenericMessage1(planepnt, GM_KILL_PLANE);
			}
			planepnt->Status = 0;
			planepnt->AI.iAIFlags2 = 0;

			OrphanAllPlaneSmoke(planepnt);

			return;
		}

//		if(((planepnt->Type->ShadowLRZOff) >= (damage_radius * 4.0f)) && (heading <= -999))
		if(((planepnt->Type->ShadowLRZOff * 4.0f) <= (damage_radius)) && (heading <= -999))
		{
			bigboom = 1;
		}
		tempdamage = DamageAIPlaneItem(planepnt, hititem, tempdamage, add_to_total, our_max_total, bigboom);
	}

	temparmor *= 5.0f;
	tempdamage *= 10.0f;
	planepnt->AI.cArmorDamage[hititem] -= temparmor;
	if(planepnt->AI.cArmorDamage[hititem] < 0)
	{
		planepnt->AI.cArmorDamage[hititem] = 0;
	}
	planepnt->AI.cInteriorDamage[hititem] = tempdamage;

//	if((planepnt->Type->ShadowLRZOff) >= (damage_radius * 2.0f))
//	{
//		return;
//	}

	if(ratio->SustainablePercentage)
	{
		if(ratio->SustainablePercentage > 1.0f)
		{
			damage -= our_max_sustainable * ratio->SustainablePercentage;
		}
		else
		{
			damage -= our_max_sustainable;
		}
	}
	if(ratio->TotalPercentage)
	{
		if(ratio->TotalPercentage > 1.0f)
		{
			damage -= our_max_total * ratio->TotalPercentage;
		}
		else
		{
			damage -= our_max_total;
		}
	}

	if(damage > 0)
	{
//		damage *= 0.5;
		DistributeDamage(planepnt, damage, ratio, pitch, heading, damage_radius, (weaponflags & WEP_SPREAD_DAMAGE));
	}
}

/*----------------------------------------------------------------------------
 *
 *	DistributePlayerDamage()
 *
 */
void DistributePlayerDamage(PlaneParams *planepnt, float damage, DamageProportions *ratio, float pitch, float heading, float damage_radius, long weaponflags)
{
	char possiblehits[40];
	int cnt;
	int numpossible = 0;
	int tempval;
//	float fworkval;
//	long lworkval;
	float add_to_sustain, add_to_total;
	float our_max_total,our_max_sustainable;
	float temparmor, tempdamage;
	int hititem;
	int bigboom = 0;
	long warningflags = 0;
	float armorleft;
	long tempflags = 0;
	int slotnum = 0;

	if(MultiPlayer)
	{
		slotnum = NetGetSlotFromPlaneIndex(planepnt - Planes);
		if(slotnum == -1)
			return;
	}

	for(cnt = 0; cnt < 40; cnt ++)
	{
		possiblehits[cnt] = 0;
	}

	if(weaponflags & (WEP_TRACK_LI|WEP_TRACK_LO|WEP_TRACK_RI|WEP_TRACK_RO))
	{
		heading = 180;
	}

	//  From relative heading/pitch from where missile exploded, determine what systems could possibly be damaged.
	if(fabs(heading) < 90.0f)
	{
		if(pitch >= 0)
		{
			tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_OXYGEN|DAMAGE_BIT_HUD);
		}
		if(pitch <= 0)
		{
			tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_FLT_CONT|DAMAGE_BIT_PACS|DAMAGE_BIT_CEN_COMP|DAMAGE_BIT_ADC|DAMAGE_BIT_AUTOPILOT|DAMAGE_BIT_TEWS|DAMAGE_BIT_TARGET_IR|DAMAGE_BIT_NAV_FLIR);
			if(heading < -20)
			{
				tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_GUN);
			}
			if(fabs(heading) > 20)
			{
				tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_FUEL_TANKS);
			}
		}
		tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_RADAR);
	}
	else if(heading > -999)
	{
		if((heading > 0) || (heading < -150))
		{
			tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_L_BLEED_AIR|DAMAGE_BIT_L_ENG_CONT|DAMAGE_BIT_L_OIL_PRESS|DAMAGE_BIT_L_BURNER|DAMAGE_BIT_L_FUEL_PUMP|DAMAGE_BIT_L_GEN|DAMAGE_BIT_L_MAIN_HYD|DAMAGE_BIT_L_ENGINE);
		}
		if(((heading < 0) || (heading > 150)) && (planepnt->Type->EngineConfig == TWO_REAR_ENGINES))
		{
			tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_R_BLEED_AIR|DAMAGE_BIT_R_ENG_CONT|DAMAGE_BIT_R_OIL_PRESS|DAMAGE_BIT_R_BURNER|DAMAGE_BIT_R_FUEL_PUMP|DAMAGE_BIT_R_GEN|DAMAGE_BIT_R_MAIN_HYD|DAMAGE_BIT_R_ENGINE);
			if(pitch < -60)
			{
				tempflags |= ~(planepnt->DamageFlags) & (DAMAGE_BIT_EMER_HYD);
			}
		}
	}
	else
	{
		tempflags |= ~(planepnt->DamageFlags) & ~(DAMAGE_LO_ENGINE|DAMAGE_RO_ENGINE);
		if(planepnt->Type->EngineConfig == ONE_REAR_ENGINE)
		{
			tempflags |= ~(planepnt->DamageFlags) & ~(DAMAGE_R_ENGINE);
		}
	}

	for(cnt = 0; cnt < 32; cnt ++)
	{
		if(tempflags & (1<<cnt))
		{
			possiblehits[numpossible++] = cnt;
		}
	}

	if(numpossible == 0)
	{
		if(heading > -999)
		{
			if(((planepnt->Type->ShadowLRZOff * 4.0f) < (damage_radius)) || (weaponflags & (WEP_FIRST_CHECK|WEP_SPREAD_DAMAGE)))
			{
				DistributeDamage(planepnt, damage, ratio, pitch, -999, damage_radius);
			}
		}
		return;
	}

	tempval = rand() % numpossible;
	hititem = possiblehits[tempval];

	our_max_sustainable = GetPlaneItemArmor(planepnt, hititem);

	our_max_total = GetPlaneItemMaxDamage(planepnt, hititem);

	add_to_sustain = damage*ratio->SustainablePercentage;

//	temparmor = (float)cPlayerArmor[slotnum][hititem] / 10.0f;
	armorleft = (float)cPlayerArmor[slotnum][hititem] / 5.0f;
	temparmor = (our_max_sustainable > armorleft) ? our_max_sustainable - armorleft : 0;
	tempdamage = (float)cPlayerDamage[slotnum][hititem] / 10.0f;

	add_to_total = (temparmor += add_to_sustain) - our_max_sustainable;

	if (add_to_total < 0.0f)
		add_to_total = 0.0f;
	else
		temparmor = our_max_sustainable;

	// then figure out how many points will go to total
	add_to_total += damage*ratio->TotalPercentage;

	if(add_to_total > 0.0f)
	{
		if(((planepnt->Type->ShadowLRZOff * 4.0f) <= (damage_radius)) && (heading <= -999))
		{
			bigboom = 1;
		}

		tempdamage = DamagePlayerPlaneItem(planepnt, hititem, tempdamage, add_to_total, our_max_total, bigboom);
	}

	temparmor *= 5.0f;
	tempdamage *= 10.0f;
	cPlayerArmor[slotnum][hititem] -= temparmor;
	if(cPlayerArmor[slotnum][hititem] < 0)
	{
		cPlayerArmor[slotnum][hititem] = 0;
	}
	cPlayerDamage[slotnum][hititem] = tempdamage;

//	if((planepnt->Type->ShadowLRZOff) >= (damage_radius * 2.0f))
//	{
//		return;
//	}

	if(ratio->SustainablePercentage)
	{
		if(ratio->SustainablePercentage > 1.0f)
		{
			damage -= our_max_sustainable * ratio->SustainablePercentage;
		}
		else
		{
			damage -= our_max_sustainable;
		}
	}
	if(ratio->TotalPercentage)
	{
		if(ratio->TotalPercentage > 1.0f)
		{
			damage -= our_max_total * ratio->TotalPercentage;
		}
		else
		{
			damage -= our_max_total;
		}
	}

	if(damage > 0)
	{
		DistributeDamage(planepnt, damage, ratio, pitch, heading, damage_radius);
	}
}

//**************************************************************************************

/* -----------------7/20/99 2:40PM---------------------------------------------------------------------
/* called when tail is scraping
/* ----------------------------------------------------------------------------------------------------*/
void ScrapePlayerTail(float severity,int item)
{
	float add_to_total;
	float our_max_total,our_max_sustainable;
	float temparmor, tempdamage;
	float armorleft;
	float damage;
	int slotnum = 0;

	if (RealisticCrashes)
	{
		if(MultiPlayer)
		{
			slotnum = NetGetSlotFromPlaneIndex(PlayerPlane - Planes);
			if(slotnum == -1)
				return;
		}

		our_max_sustainable = GetPlaneItemArmor(PlayerPlane, item);

		our_max_total = GetPlaneItemMaxDamage(PlayerPlane, item);

		damage = our_max_total*(1.0f/50.0f)*severity;

	//	temparmor = (float)cPlayerArmor[slotnum][hititem] / 10.0f;
		armorleft = (float)cPlayerArmor[slotnum][item] / 5.0f;
		temparmor = (our_max_sustainable > armorleft) ? our_max_sustainable - armorleft : 0;
		tempdamage = (float)cPlayerDamage[slotnum][item] / 10.0f;

		add_to_total = temparmor - our_max_sustainable;

		if (add_to_total < 0.0f)
			add_to_total = 0.0f;
		else
			temparmor = our_max_sustainable;

		// then figure out how many points will go to total

		add_to_total += damage;

		if(add_to_total > 0.0f)
		{
			tempdamage = DamagePlayerPlaneItem(PlayerPlane, item, tempdamage, damage, our_max_total, TRUE);

			temparmor *= 5.0f;
			tempdamage *= 10.0f;
			cPlayerArmor[slotnum][item] -= temparmor;
			if(cPlayerArmor[slotnum][item] < 0)
			{
				cPlayerArmor[slotnum][item] = 0;
			}
			cPlayerDamage[slotnum][item] = tempdamage;
		}
	}
}



//**************************************************************************************
void LogPlaneShotdown(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type,int iCrashedPlane)
{
	int iShotdownAircraft=-1;
	int iWeaponSide=0;

	if(MultiPlayer && (!netCall))
	{
		int destroy = (iCrashedPlane) ? 3 : 1;

		NetSendLogDamageOrDestroy(planepnt, NULL, W, damage_type, destroy);
	}

	LogEventPlaneDestroyed(planepnt,W,damage_type,iCrashedPlane);

	iShotdownAircraft = planepnt-Planes;

	LogMissionStatistic(LOG_FLIGHT_DAMAGED,iShotdownAircraft,1,2);

	if (iCrashedPlane)
	{
		int	iGroundType = GetGroundType(planepnt->WorldPosition,NULL,NULL,TRUE);

		switch(iGroundType)
		{
			case TT_LAND:
				LogMissionStatistic(LOG_FLIGHT_CRASHED_GROUND,iShotdownAircraft,1,1);
				LogMissionStatistic(LOG_FLIGHT_CRASHED_AIRCRAFT,iShotdownAircraft,1,1);
			break;
			case TT_WATER:
				LogMissionStatistic(LOG_FLIGHT_CRASHED_WATER,iShotdownAircraft,1,1);
				LogMissionStatistic(LOG_FLIGHT_CRASHED_AIRCRAFT,iShotdownAircraft,1,1);
			break;
			case TT_AIRPORT:
				LogMissionStatistic(LOG_FLIGHT_CRASHED_RUNWAY,iShotdownAircraft,1,1);
				LogMissionStatistic(LOG_FLIGHT_CRASHED_AIRCRAFT,iShotdownAircraft,1,1);
			break;
			case TT_RUNWAY:
				LogMissionStatistic(LOG_FLIGHT_CRASHED_RUNWAY,iShotdownAircraft,1,1);
				LogMissionStatistic(LOG_FLIGHT_CRASHED_AIRCRAFT,iShotdownAircraft,1,1);
			break;
			case TT_DESTROYED_RUNWAY:
				LogMissionStatistic(LOG_FLIGHT_CRASHED_RUNWAY,iShotdownAircraft,1,1);
				LogMissionStatistic(LOG_FLIGHT_CRASHED_AIRCRAFT,iShotdownAircraft,1,1);
			break;
		}

		if (!(planepnt->AI.iAIFlags1 & AI_HAS_EJECTED))
		{
			LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_KIA,iShotdownAircraft,1,1);
		}
	}

	LogMissionStatistic(LOG_FLIGHT_DESTROYED,iShotdownAircraft,1,1);

	if (iCrashedPlane) return;

	if(W)
	{
		if(W->LauncherType == AIRCRAFT)
		{
			iWeaponSide = W->P->AI.iSide;
		}
		else if(W->LauncherType == GROUNDOBJECT)
		{
			iWeaponSide = GDConvertGrndSide((BasicInstance *)W->Launcher);
		}

		if(W->LauncherType == AIRCRAFT)
		{
			if (W->P==planepnt)
			{
				if (!(damage_type & DTE_DIRECT_HIT))
				{
					LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_OWN_FRAG,iShotdownAircraft,1,1);
				} else {
					// Shotdown by something else we shot
				}
			}
		}

		long iWeaponType;

		if ((int)W->Type < 0x200)
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].iWeaponType;
		else
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].iWeaponType;

		int iLauncherAircraft=(W->P-Planes);

		switch(iWeaponType)
		{
			case WEAPON_TYPE_DUMB_BOMB:
			case WEAPON_TYPE_GUIDED_BOMB:
			case WEAPON_TYPE_AG_MISSILE:
			case WEAPON_TYPE_AGROCKET:
			case WEAPON_TYPE_CLUSTER_BOMB:
			case WEAPON_TYPE_NAPALM:
			case WEAPON_TYPE_NAVAL_MINE:
			case WEAPON_TYPE_ANTI_SHIP_MISSILE:
			case WEAPON_TYPE_FUEL_AIR_EXPLOSIVE:
			{
				if (planepnt->AI.iSide==iWeaponSide)
				{
					if (!(damage_type & DTE_DIRECT_HIT))
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_FRIENDLY_BOMB,iShotdownAircraft,1,1);
					else
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_FRIENDLY_FRAG,iShotdownAircraft,1,1);

					LogMissionStatistic(LOG_FLIGHT_FRIENDLY_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
					LogMissionStatistic(LOG_FLIGHT_BOMBS_HIT,iLauncherAircraft,1,1);
					LogMissionStatistic(LOG_FLIGHT_BOMB_KILLS,iLauncherAircraft,1,1);
					LogMissionStatisticKill(iLauncherAircraft,AIRCRAFT,iShotdownAircraft);
				} else {
					if (!(damage_type & DTE_DIRECT_HIT))
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_ENEMY_BOMB,iShotdownAircraft,1,1);
					else
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_ENEMY_FRAG,iShotdownAircraft,1,1);

					if (!planepnt->AI.iSide)
					{
						LogMissionStatistic(LOG_FLIGHT_ENEMY_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
					} else {
						LogMissionStatistic(LOG_FLIGHT_NEUTRAL_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
					}
					LogMissionStatistic(LOG_FLIGHT_BOMBS_HIT,iLauncherAircraft,1,1);
					LogMissionStatistic(LOG_FLIGHT_BOMB_KILLS,iLauncherAircraft,1,1);
					LogMissionStatisticKill(iLauncherAircraft,AIRCRAFT,iShotdownAircraft);
				}
			}
			break;
			case WEAPON_TYPE_AA_SHORT_RANGE_MISSILE:
			case WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE:
			case WEAPON_TYPE_AA_LONG_RANGE_MISSILE:
			{
				if (planepnt->AI.iSide==iWeaponSide)
				{
					LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_FRIENDLY_MISSILE,iShotdownAircraft,1,1);

					LogMissionStatistic(LOG_FLIGHT_FRIENDLY_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
					LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_HIT,iLauncherAircraft,1,1);
					LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_KILLS,iLauncherAircraft,1,1);
					LogMissionStatisticKill(iLauncherAircraft,AIRCRAFT,iShotdownAircraft);
				} else {
					LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_ENEMY_MISSILE,iShotdownAircraft,1,1);
					if (!planepnt->AI.iSide)
					{
						LogMissionStatistic(LOG_FLIGHT_ENEMY_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
					} else {
						LogMissionStatistic(LOG_FLIGHT_NEUTRAL_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
					}

					LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_HIT,iLauncherAircraft,1,1);
					LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_KILLS,iLauncherAircraft,1,1);
					LogMissionStatisticKill(iLauncherAircraft,AIRCRAFT,iShotdownAircraft);
				}
			}
			break;
			case WEAPON_TYPE_GUN:
			{
				if (planepnt->AI.iSide==iWeaponSide)
				{
					if(W->LauncherType == AIRCRAFT)
					{
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_FRIENDLY_GUNS,iShotdownAircraft,1,1);
						LogMissionStatistic(LOG_FLIGHT_FRIENDLY_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
						LogMissionStatistic(LOG_FLIGHT_GUN_ROUNDS_HIT,iLauncherAircraft,1,1);
						LogMissionStatistic(LOG_FLIGHT_GUN_ROUND_KILLS,iLauncherAircraft,1,1);
						LogMissionStatisticKill(iLauncherAircraft,AIRCRAFT,iShotdownAircraft);
					}
					else
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_FRIENDLY_AAA,iShotdownAircraft,1,1);
				} else {
					if(W->LauncherType == AIRCRAFT)
					{
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_ENEMY_GUNS,iShotdownAircraft,1,1);

						if (!planepnt->AI.iSide)
						{
							LogMissionStatistic(LOG_FLIGHT_ENEMY_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
						} else {
							LogMissionStatistic(LOG_FLIGHT_NEUTRAL_AIRCRAFT_KILLS,iLauncherAircraft,1,1);
						}
						LogMissionStatistic(LOG_FLIGHT_GUN_ROUNDS_HIT,iLauncherAircraft,1,1);
						LogMissionStatistic(LOG_FLIGHT_GUN_ROUND_KILLS,iLauncherAircraft,1,1);
						LogMissionStatisticKill(iLauncherAircraft,AIRCRAFT,iShotdownAircraft);
					}
					else
						LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_ENEMY_AAA,iShotdownAircraft,1,1);
				}
			}
			break;
			case WEAPON_TYPE_SCUD:
			case WEAPON_TYPE_SAM:
				if (planepnt->AI.iSide==iWeaponSide)
				{
					LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_FRIENDLY_SAM,iShotdownAircraft,1,1);
				} else {
					LogMissionStatistic(LOG_FLIGHT_SHOTDOWN_ENEMY_SAM,iShotdownAircraft,1,1);
				}
			break;
		}
	}
}

//**************************************************************************************
void LogPlaneDamage(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type)
{
	int iDamagedFlight=-1;

	if(MultiPlayer && (!netCall))
	{
		NetSendLogDamageOrDestroy(planepnt, NULL, W, damage_type, 0);
	}

	// Record plane damage event
	LogEventPlaneDamage(planepnt,W,damage_type);

	iDamagedFlight = planepnt-Planes;

	LogMissionStatistic(LOG_FLIGHT_DAMAGED,iDamagedFlight,1,2);

	if(W)
	{
		long iWeaponType;

		if ((int)W->Type < 0x200)
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].iWeaponType;
		else
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].iWeaponType;

		if(W->LauncherType != AIRCRAFT) return;

		int iLauncherAircraft = W->P-Planes;

		switch(iWeaponType)
		{
			case WEAPON_TYPE_DUMB_BOMB:
			case WEAPON_TYPE_GUIDED_BOMB:
			case WEAPON_TYPE_CLUSTER_BOMB:
			case WEAPON_TYPE_NAVAL_MINE:
			case WEAPON_TYPE_NAPALM:
			case WEAPON_TYPE_FUEL_AIR_EXPLOSIVE:
			{
				LogMissionStatistic(LOG_FLIGHT_BOMBS_HIT,iLauncherAircraft,1,1);
			}
			break;
			case WEAPON_TYPE_AG_MISSILE:
			case WEAPON_TYPE_AGROCKET:
			case WEAPON_TYPE_ANTI_SHIP_MISSILE:
			{
				LogMissionStatistic(LOG_FLIGHT_AG_MISSILE_HITS,iLauncherAircraft,1,1);
			}
			break;
			case WEAPON_TYPE_AA_SHORT_RANGE_MISSILE:
			case WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE:
			case WEAPON_TYPE_AA_LONG_RANGE_MISSILE:
			{
				LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_HIT,iLauncherAircraft,1,1);
			}
			break;
			case WEAPON_TYPE_GUN:
			{
				LogMissionStatistic(LOG_FLIGHT_GUN_ROUNDS_HIT,iLauncherAircraft,1,1);
			}
			break;
			case WEAPON_TYPE_SCUD:
			case WEAPON_TYPE_SAM:
			break;
		}
	}
}

//**************************************************************************************
void LogWeaponInstantiate(PlaneParams *planepnt,WeaponParams *W)
{
	int iLauncherAircraft=-1;

	if (!planepnt) return;

	iLauncherAircraft = planepnt-Planes;

	if(W)
	{
		long iWeaponType;

		if ((int)W->Type < 0x200)
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].iWeaponType;
		else
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].iWeaponType;

		switch(iWeaponType)
		{
			case WEAPON_TYPE_DUMB_BOMB:
			case WEAPON_TYPE_GUIDED_BOMB:
			case WEAPON_TYPE_CLUSTER_BOMB:
			case WEAPON_TYPE_NAVAL_MINE:
			case WEAPON_TYPE_NAPALM:
			case WEAPON_TYPE_FUEL_AIR_EXPLOSIVE:
				LogMissionStatistic(LOG_FLIGHT_BOMBS_RELEASED,iLauncherAircraft,1,1);
//				if (planepnt==PlayerPlane)
				if(AIObjects[planepnt - Planes].iBriefingGroup)
				{
					if (AreaGoalExists(W))
					{
						g_iNumWeaponReleasedInAreaGoal++;
					}
					WeaponAreaGoalExists(W);
				}
			break;
			case WEAPON_TYPE_AG_MISSILE:
			case WEAPON_TYPE_AGROCKET:
			case WEAPON_TYPE_ANTI_SHIP_MISSILE:
				LogMissionStatistic(LOG_FLIGHT_AG_MISSILES_RELEASED,iLauncherAircraft,1,1);
//				if (planepnt==PlayerPlane)
				if(AIObjects[planepnt - Planes].iBriefingGroup)
				{
					if (AreaGoalExists(W))
					{
						g_iNumWeaponReleasedInAreaGoal++;
					}
					WeaponAreaGoalExists(W);
				}
			break;
			case WEAPON_TYPE_AA_SHORT_RANGE_MISSILE:
			case WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE:
			case WEAPON_TYPE_AA_LONG_RANGE_MISSILE:
				LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_RELEASED,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_GUN:
				LogMissionStatistic(LOG_FLIGHT_GUN_ROUNDS_FIRED,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_SCUD:
			case WEAPON_TYPE_SAM:
			break;
		}
	}
}

//**************************************************************************************
void LogWeaponHit(WeaponParams *W,int iType,int iID)
{
	int iLauncherAircraft=-1;

	if  (!W) return;
	if (!W->P) return;

	iLauncherAircraft = W->P-Planes;


	switch(iType)
	{
		case MOVINGVEHICLE:
			if(MultiPlayer && (!netCall))
			{
				NetSendLogDamageOrDestroy(NULL, &MovingVehicles[iID], W, 0, 0);
			}

			if (MovingVehicles[iID].iVDBIndex!=-1)
			{
				LogEventVehicleDamaged(NULL,W,0,0,iID,MOVINGVEHICLE,pDBVehicleList[MovingVehicles[iID].iVDBIndex].lVehicleID);
			}
		break;
		case SHIP:
			if(MultiPlayer && (!netCall))
			{
				NetSendLogDamageOrDestroy(NULL, &MovingVehicles[iID], W, 0, 0);
			}

			if (MovingVehicles[iID].iVDBIndex!=-1)
			{
				LogEventVehicleDamaged(NULL,W,0,0,iID,SHIP,pDBShipList[MovingVehicles[iID].iVDBIndex].lShipID);
			}
		break;
		case GROUNDOBJECT:
			LogEventGroundObjectDamaged(NULL,W,0,0,iID);
		break;
	}


	if(W)
	{
		long iWeaponType;

		if ((int)W->Type < 0x200)
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].iWeaponType;
		else
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].iWeaponType;

		switch(iWeaponType)
		{
			case WEAPON_TYPE_DUMB_BOMB:
			case WEAPON_TYPE_GUIDED_BOMB:
			case WEAPON_TYPE_CLUSTER_BOMB:
			case WEAPON_TYPE_NAVAL_MINE:
			case WEAPON_TYPE_NAPALM:
			case WEAPON_TYPE_FUEL_AIR_EXPLOSIVE:
				LogMissionStatistic(LOG_FLIGHT_BOMBS_HIT,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_AG_MISSILE:
			case WEAPON_TYPE_AGROCKET:
			case WEAPON_TYPE_ANTI_SHIP_MISSILE:
				LogMissionStatistic(LOG_FLIGHT_AG_MISSILE_HITS,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_AA_SHORT_RANGE_MISSILE:
			case WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE:
			case WEAPON_TYPE_AA_LONG_RANGE_MISSILE:
				LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_HIT,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_GUN:
				LogMissionStatistic(LOG_FLIGHT_GUN_ROUNDS_HIT,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_SCUD:
			case WEAPON_TYPE_SAM:
			break;
		}
	}
}

extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);

//**************************************************************************************
void LogDestroyedObject(WeaponParams *W,int iType,int iID)
{
	int iLauncherAircraft=-1;

	if (!W) return;
	if (!W->P) return;

	iLauncherAircraft=W->P-Planes;

	if((iType==MOVINGVEHICLE) || (iType==SHIP))
	{
		if(MultiPlayer && (!netCall))
		{
			NetSendLogDamageOrDestroy(NULL, &MovingVehicles[iID], W, 0, 1);
		}

		if (!MovingVehicles[iID].iShipType)
		{
			if (MovingVehicles[iID].iSide == AI_FRIENDLY)
			{
				LogMissionStatistic(LOG_FLIGHT_FRIENDLY_MOVING_VEHICLE_KILLS,iLauncherAircraft,1,1);
			} else {
				LogMissionStatistic(LOG_FLIGHT_ENEMY_MOVING_VEHICLE_KILLS,iLauncherAircraft,1,1);
			}
			LogMissionStatisticKill(iLauncherAircraft,MOVINGVEHICLE,iID);

			if (MovingVehicles[iID].iVDBIndex!=-1)
			{
				LogEventVehicleDestroyed(NULL,W,0,0,iID,MOVINGVEHICLE,pDBVehicleList[MovingVehicles[iID].iVDBIndex].lVehicleID);
			}
		} else {
			if (MovingVehicles[iID].iSide == AI_FRIENDLY)
			{
				LogMissionStatistic(LOG_FLIGHT_FRIENDLY_SHIP_KILLS,iLauncherAircraft,1,1);
			} else {
				LogMissionStatistic(LOG_FLIGHT_ENEMY_SHIP_KILLS,iLauncherAircraft,1,1);
			}
			LogMissionStatisticKill(iLauncherAircraft,SHIP,iID);
			if (MovingVehicles[iID].iVDBIndex!=-1)
			{
				LogEventVehicleDestroyed(NULL,W,0,0,iID,SHIP,pDBShipList[MovingVehicles[iID].iVDBIndex].lShipID);
			}
		}
	} else {
		BasicInstance *walker = FindInstance(AllInstances, iID);

		if (walker)
		{
			int iSide = GDConvertGrndSide(walker);
			switch(iSide)
			{
				case AI_FRIENDLY:
					LogMissionStatistic(LOG_FLIGHT_FRIENDLY_GROUND_TARGET_KILLS,iLauncherAircraft,1,1);
				break;
				case AI_NEUTRAL:
					LogMissionStatistic(LOG_FLIGHT_NEUTRAL_GROUND_TARGET_KILLS,iLauncherAircraft,1,1);
				break;
				case AI_ENEMY:
				default:
					LogMissionStatistic(LOG_FLIGHT_ENEMY_GROUND_TARGET_KILLS,iLauncherAircraft,1,1);
				break;
			}
		} else {
			LogMissionStatistic(LOG_FLIGHT_ENEMY_GROUND_TARGET_KILLS,iLauncherAircraft,1,1);
		}
		LogEventGroundObjectDestroyed(NULL,W,0,0,iID);
		LogMissionStatisticKill(iLauncherAircraft,GROUNDOBJECT,iID);
	}

	if(W)
	{
		long iWeaponType;

		if ((int)W->Type < 0x200)
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].iWeaponType;
		else
			iWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].iWeaponType;

		switch(iWeaponType)
		{
			case WEAPON_TYPE_DUMB_BOMB:
			case WEAPON_TYPE_GUIDED_BOMB:
			case WEAPON_TYPE_CLUSTER_BOMB:
			case WEAPON_TYPE_NAVAL_MINE:
			case WEAPON_TYPE_NAPALM:
			case WEAPON_TYPE_FUEL_AIR_EXPLOSIVE:
				LogMissionStatistic(LOG_FLIGHT_BOMB_KILLS,iLauncherAircraft,1,1);
//				if (W->P==PlayerPlane)
				if((W->P) && (AIObjects[W->P - Planes].iBriefingGroup))
				{
					if (AreaGoalExists(W))
					{
						g_iNumAroundGoalObjectsHit++;
					}
				}
			break;
			case WEAPON_TYPE_AG_MISSILE:
			case WEAPON_TYPE_ANTI_SHIP_MISSILE:
				LogMissionStatistic(LOG_FLIGHT_AG_MISSILE_KILLS,iLauncherAircraft,1,1);
//				if (W->P==PlayerPlane)
				if((W->P) && (AIObjects[W->P - Planes].iBriefingGroup))
				{
					if (AreaGoalExists(W))
					{
						g_iNumAroundGoalObjectsHit++;
					}
				}
			break;
			case WEAPON_TYPE_AA_SHORT_RANGE_MISSILE:
			case WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE:
			case WEAPON_TYPE_AA_LONG_RANGE_MISSILE:
				LogMissionStatistic(LOG_FLIGHT_AA_MISSILES_KILLS,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_GUN:
				LogMissionStatistic(LOG_FLIGHT_GUN_ROUND_KILLS,iLauncherAircraft,1,1);
			break;
			case WEAPON_TYPE_SCUD:
			case WEAPON_TYPE_SAM:
			break;
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	ADCheckSpecificHit()
 *
 */

float ADCheckSpecificHit(PlaneParams *planepnt, WeaponParams *W, DBWeaponType *pweapon_type, float *pitch, FPointDouble relative_position)
{
	float lasth, lastp, currh, currp;
	float elasth, elastp, ecurrh, ecurrp;
	float heading = -999.0f;
	double dworkval;
	FPointDouble engine_location;
	FPoint *offset;
	int offsetid;
	long tempval;

	if(!W)
	{
		*pitch = 0;
		return(heading);
	}

	if(W->Flags & (WEP_TRACK_LI|WEP_TRACK_LO|WEP_TRACK_RI|WEP_TRACK_RO))
	{
		*pitch = 0;
		return(0);
	}

	PLGetRelativeHP(planepnt, W->LastPos, &lasth, &lastp);
	PLGetRelativeHP(planepnt, W->Pos, &currh, &currp);

	if(((lastp >= 0) && (currp >= 0)) || ((lastp < 0) && (currp < 0)))
	{
		*pitch = (lastp + currp) / 2;
	}
	else
	{
		*pitch = 0;
	}

	if(((lasth < 0) && (currh >= 0)) || ((lasth >= 0) && (currh < 0)))
	{
		if(fabs(lasth) > 165)
		{
			if(rand() & 1)
			{
				heading = -175.0f;
			}
			else
			{
				heading = 175.0f;
			}
		}
		else
		{
			heading = -999;
		}
		return(heading);
	}

	if(pweapon_type->iWeaponType != WEAPON_TYPE_GUN)
	{
		heading = (lasth + currh) / 2;
		return(heading);
	}

	if((planepnt->Type->EngineConfig == TWO_REAR_ENGINES) || (planepnt->Type->EngineConfig == TWO_REAR_ENGINES))
	{
		dworkval = (planepnt->Orientation.K * relative_position);

		if(((dworkval >= 0) && (currh >= 0)) || ((dworkval < 0) && (currh < 0)))
		{
			if(fabs(lasth) > 165)
			{
				if(rand() & 1)
				{
					heading = -175.0f;
				}
				else
				{
					heading = 175.0f;
				}
			}
			else  //  Hit wing on "far" side of plane.  Well we can't really do that so hit systems on the "far" side of plane.
			{
				tempval = rand() & 1023;
				if(dworkval <= 0.0f)
				{
					heading = ((float)tempval / 1024.0f) * 180.0f;
				}
				else
				{
					heading = ((float)tempval / 1024.0f) * -180.0f;
				}
			}
		}
		else
		{
			heading = -999;
		}
		return(heading);
	}

	if(currh >= 0)
	{
		offsetid = GetEngineArrayOffset(planepnt, WEP_TRACK_LI);
	}
	else
	{
		offsetid = GetEngineArrayOffset(planepnt, WEP_TRACK_RI);
	}

	offset = & planepnt->Type->Offsets[offsetid];
	engine_location.X =  planepnt->WorldPosition.X +  planepnt->Orientation.I.X*(double)offset->Z +  planepnt->Orientation.J.X*(double)offset->X +  planepnt->Orientation.K.X*(double)offset->Y;
	engine_location.Y =  planepnt->WorldPosition.Y +  planepnt->Orientation.I.Y*(double)offset->Z +  planepnt->Orientation.J.Y*(double)offset->X +  planepnt->Orientation.K.Y*(double)offset->Y;
	engine_location.Z =  planepnt->WorldPosition.Z +  planepnt->Orientation.I.Z*(double)offset->Z +  planepnt->Orientation.J.Z*(double)offset->X +  planepnt->Orientation.K.Z*(double)offset->Y;

	ADGetRelativeHPPoint(planepnt, engine_location, W->LastPos, &elasth, &elastp);
	ADGetRelativeHPPoint(planepnt, engine_location, W->Pos, &ecurrh, &ecurrp);

	if(((fabs(elasth) < 45) || (fabs(elasth) > 135)) && ((fabs(ecurrh) < 45) || (fabs(ecurrh) > 135)))
	{
		if(currh >= 0)
		{
			W->Flags |= WEP_TRACK_LI;
		}
		else
		{
			W->Flags |= WEP_TRACK_RI;
		}
		return(0);
	}

	if(planepnt->Type->EngineConfig == FOUR_WING_ENGINES)
	{
		if(currh >= 0)
		{
			offsetid = GetEngineArrayOffset(planepnt, WEP_TRACK_LO);
		}
		else
		{
			offsetid = GetEngineArrayOffset(planepnt, WEP_TRACK_RO);
		}

		offset = & planepnt->Type->Offsets[offsetid];
		engine_location.X =  planepnt->WorldPosition.X +  planepnt->Orientation.I.X*(double)offset->Z +  planepnt->Orientation.J.X*(double)offset->X +  planepnt->Orientation.K.X*(double)offset->Y;
		engine_location.Y =  planepnt->WorldPosition.Y +  planepnt->Orientation.I.Y*(double)offset->Z +  planepnt->Orientation.J.Y*(double)offset->X +  planepnt->Orientation.K.Y*(double)offset->Y;
		engine_location.Z =  planepnt->WorldPosition.Z +  planepnt->Orientation.I.Z*(double)offset->Z +  planepnt->Orientation.J.Z*(double)offset->X +  planepnt->Orientation.K.Z*(double)offset->Y;

		ADGetRelativeHPPoint(planepnt, engine_location, W->LastPos, &elasth, &elastp);
		ADGetRelativeHPPoint(planepnt, engine_location, W->Pos, &ecurrh, &ecurrp);
		if(((fabs(elasth) < 45) || (fabs(elasth) > 135)) && ((fabs(ecurrh) < 45) || (fabs(ecurrh) > 135)))
		{
			if(currh >= 0)
			{
				W->Flags |= WEP_TRACK_LO;
			}
			else
			{
				W->Flags |= WEP_TRACK_RO;
			}
			return(0);
		}
	}

	dworkval = (planepnt->Orientation.K * relative_position);

	if(((dworkval >= 0) && (currh >= 0)) || ((dworkval < 0) && (currh < 0)))
	{
		if(fabs(lasth) > 165)
		{
			if(rand() & 1)
			{
				heading = -175.0f;
			}
			else
			{
				heading = 175.0f;
			}
		}
		else  //  Hit wing on "far" side of plane.  Well we can't really do that so hit systems on the "far" side of plane.
		{
			tempval = rand() & 1023;
			if(dworkval <= 0.0f)
			{
				heading = ((float)tempval / 1024.0f) * 180.0f;
			}
			else
			{
				heading = ((float)tempval / 1024.0f) * -180.0f;
			}
		}
	}
	return(heading);
}

/*----------------------------------------------------------------------------
 *
 *	ADGetRaltiveHPPoint()
 *
 */
void ADGetRelativeHPPoint(PlaneParams *planepnt, FPointDouble engineposition, FPointDouble worldposition, float *heading, float *pitch)
{
	FPointDouble relative_position;
	FMatrix de_rotate;

	relative_position.MakeVectorToFrom(worldposition,engineposition);

	de_rotate = planepnt->Attitude;
	de_rotate.Transpose();

	relative_position *= de_rotate;
	relative_position.Normalize();

	*heading = atan2(-relative_position.X,-relative_position.Z) * 57.2958;
	*pitch = asin(relative_position.Y) * 57.2958;
}

/*----------------------------------------------------------------------------
 *
 *	ADSetUpPlayerHeatUp()
 *
 */
void ADSetUpPlayerHeatUp(int leftside, long timetillfire, int amad)
{
	float fworkval1;
	float forgval;

	amad = 0;/// TW: no apu or amad fires  CM

	if(timetillfire == 0)
		timetillfire = 1;
	if(lLOrgAMADTimeToFire == 0)
		lLOrgAMADTimeToFire = -1;
	if(lLeftOrgTimeToFire == 0)
		lLeftOrgTimeToFire = -1;
	if(lROrgAMADTimeToFire == 0)
		lROrgAMADTimeToFire = -1;
	if(lRightOrgTimeToFire == 0)
		lRightOrgTimeToFire = -1;

	if(leftside)
	{
		if(!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_LEFT))
		{
			if(amad)
			{
				if(!(lFireFlags & HEAT_UP_AMAD_LEFT))
				{
					forgval = -1;
					lLAMADTimeToFire = timetillfire;
					lFireFlags |= HEAT_UP_AMAD_LEFT;
				}
				else if(lLAMADTimeToFire > timetillfire)
				{
					forgval = lLAMADTimeToFire / lLOrgAMADTimeToFire;
					lLAMADTimeToFire = timetillfire;
				}
				else
				{
					forgval = lLAMADTimeToFire / lLOrgAMADTimeToFire;
				}
				if( (timetillfire < lLOrgAMADTimeToFire) || (lLOrgAMADTimeToFire < 0) )
				{
				  if(forgval >= 0)
				  {
					  fworkval1 = lLAMADTimeToFire / timetillfire;
					  if((forgval > fworkval1) || (!forgval))
					  {
						  lLOrgAMADTimeToFire = timetillfire;
					  }
					  else
					  {
						  lLOrgAMADTimeToFire = lLAMADTimeToFire / forgval;
					  }
				  }
				  else
				  {
				    lLOrgAMADTimeToFire = timetillfire;
				  }
				}
			}
			else
			{
				if(!(lFireFlags & HEAT_UP_LEFT_ENGINE))
				{
					lLeftTimeToFire = timetillfire;
					lFireFlags |= HEAT_UP_LEFT_ENGINE;
					forgval = -1;
				}
				else if(lLeftTimeToFire > timetillfire)
				{
					lLeftTimeToFire = timetillfire;
					forgval = lLeftTimeToFire / lLeftOrgTimeToFire;
				}
				else
				{
					forgval = lLeftTimeToFire / lLeftOrgTimeToFire;
				}
				if( (timetillfire < lLeftOrgTimeToFire) || (lLeftOrgTimeToFire < 0) )
				{
				  if(forgval >= 0)
				  {
					  fworkval1 = lLeftTimeToFire / timetillfire;
					  if((forgval > fworkval1) || (!forgval))
					  {
						  lLeftOrgTimeToFire = timetillfire;
					  }
					  else
					  {
						  lLeftOrgTimeToFire = lLeftTimeToFire / forgval;
					  }
				  }
				  else
				  {
					lLeftOrgTimeToFire = timetillfire;
				  }
				}
			}
		}
	}
	else
	{
		if(!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_RIGHT))
		{
			if(amad)
			{
				if(!(lFireFlags & HEAT_UP_AMAD_RIGHT))
				{
					lRAMADTimeToFire = timetillfire;
					lFireFlags |= HEAT_UP_AMAD_RIGHT;
					forgval = -1;
				}
				else if(lRAMADTimeToFire > timetillfire)
				{
					lRAMADTimeToFire = timetillfire;
					forgval = lRAMADTimeToFire / lROrgAMADTimeToFire;
				}
				else
				{
					forgval = lRAMADTimeToFire / lROrgAMADTimeToFire;
				}
				if( (timetillfire < lROrgAMADTimeToFire) || (lROrgAMADTimeToFire < 0) )
				{
				  if(forgval >= 0)
				  {
					  fworkval1 = lRAMADTimeToFire / timetillfire;
					  if((forgval > fworkval1) || (!forgval))
					  {
						  lROrgAMADTimeToFire = timetillfire;
					  }
					  else
					  {
						  lROrgAMADTimeToFire = lRAMADTimeToFire / forgval;
					  }
				  }
				  else
				  {
					lROrgAMADTimeToFire = timetillfire;
				  }
				}
			}
			else
			{
				if(!(lFireFlags & HEAT_UP_RIGHT_ENGINE))
				{
					lRightTimeToFire = timetillfire;
					lFireFlags |= HEAT_UP_RIGHT_ENGINE;
					forgval = -1;
				}
				else if(lRightTimeToFire > timetillfire)
				{
					lRightTimeToFire = timetillfire;
					forgval = lRightTimeToFire / lRightOrgTimeToFire;
				}
				else
				{
					forgval = lRightTimeToFire / lRightOrgTimeToFire;
				}
				if( (timetillfire < lRightOrgTimeToFire) || (lRightOrgTimeToFire < 0) )
				{
				  if(forgval >= 0)
				  {
					  fworkval1 = lRightTimeToFire / timetillfire;
					  if((forgval > fworkval1) || (!forgval))
					  {
						  lRightOrgTimeToFire = timetillfire;
					  }
					  else
					  {
						  lRightOrgTimeToFire = lRightTimeToFire / forgval;
					  }
				  }
				  else
				  {
					lRightOrgTimeToFire = timetillfire;
				  }
				}
			}
		}
	}

	if(lLOrgAMADTimeToFire == 0)
		lLOrgAMADTimeToFire = -1;
	if(lLeftOrgTimeToFire == 0)
		lLeftOrgTimeToFire = -1;
	if(lROrgAMADTimeToFire == 0)
		lROrgAMADTimeToFire = -1;
	if(lRightOrgTimeToFire == 0)
		lRightOrgTimeToFire = -1;
}

/*----------------------------------------------------------------------------
 *
 *	ADEngineFireOut()
 *
 */
void ADEngineFireOut(int leftside, int amad)
{
	long warnflags = 0;
	long turnoffflags = 0;
	int cnt;

	amad = 0;/// TW: no apu or amad fires  CM

	if(amad)
		lFireFlags &= ~(HEAT_UP_AMAD_LEFT|FIRE_AMAD_LEFT|HEAT_UP_AMAD_RIGHT|FIRE_AMAD_RIGHT);

	if(leftside)
	{
		lFireFlags &= ~(HEAT_UP_LEFT_ENGINE|FIRE_LEFT_ENGINE);

		if((PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT]) && (!(lFireFlags & (FIRE_AMAD_LEFT|FIRE_LEFT_ENGINE))))
		{
			if(PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft > 150)
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = 150;
		}

		ShutLeftEngineDown();
//		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_LEFT;
//		PlayerPlane->DamageFlags |= (DAMAGE_BIT_LO_ENGINE|DAMAGE_BIT_L_ENGINE);

		turnoffflags |= (DAMAGE_BIT_LO_ENGINE)|(DAMAGE_BIT_L_ENG_CONT)|(DAMAGE_BIT_L_OIL_PRESS)|(DAMAGE_BIT_L_BURNER)|(DAMAGE_BIT_L_FUEL_PUMP)|(DAMAGE_BIT_L_GEN);
		warnflags |= (1<<WARN_L_ENG_CONT)|(1<<WARN_L_OIL_PRESS)|(1<<WARN_L_BURNER)|(1<<WARN_L_FUEL_PUMP)|(1<<WARN_L_GEN);
		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_LEFT;
	}
	else
	{
		lFireFlags &= ~(HEAT_UP_RIGHT_ENGINE|FIRE_RIGHT_ENGINE);

		if((PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT]) && (!(lFireFlags & (FIRE_AMAD_LEFT|FIRE_RIGHT_ENGINE))))
		{
			if(PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft > 150)
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = 150;
		}

		ShutRightEngineDown();
//		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_RIGHT;
//		PlayerPlane->DamageFlags |= (DAMAGE_BIT_RO_ENGINE|DAMAGE_BIT_R_ENGINE);

		turnoffflags |= (DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_R_ENG_CONT)|(DAMAGE_BIT_R_OIL_PRESS)|(DAMAGE_BIT_R_BURNER)|(DAMAGE_BIT_R_FUEL_PUMP)|(DAMAGE_BIT_R_GEN);
		warnflags |= (1<<WARN_R_ENG_CONT)|(1<<WARN_R_OIL_PRESS)|(1<<WARN_R_BURNER)|(1<<WARN_R_FUEL_PUMP)|(1<<WARN_R_GEN);
		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_RIGHT;
	}

	if((~PlayerPlane->SystemInactive) & warnflags)
	{
		for(cnt = 0; cnt < 32; cnt ++)
		{
			if(warnflags & (1<<cnt))
			{
				DisplayWarning(MASTER_CAUTION, ON, 0);
				SetMasterWarning(cnt);
			}
		}
	}
	PlayerPlane->SystemInactive |= turnoffflags | PlayerPlane->DamageFlags;
	PlayerPlane->DamageFlags |= turnoffflags;
}

/*----------------------------------------------------------------------------
 *
 *	ADEngineFireTempOut()
 *
 */
void ADEngineFireTempOut(int leftside, int amad)
{
	long warnflags = 0;
	long turnoffflags = 0;
	int cnt;

	amad = 0;/// TW: no apu or amad fires  CM


	if(leftside)
	{
		ADEngineFireOut(leftside, amad);
		if(amad)
		{
			lFireFlags |= (HEAT_UP_AMAD_RIGHT|FIRE_AMAD_RIGHT|HEAT_UP_AMAD_LEFT|FIRE_AMAD_LEFT);
			lLAMADTimeToFire = 20000;
			lLAMADTimeToExplode += 20000;
		}
		else
		{
			lFireFlags |= (HEAT_UP_LEFT_ENGINE|FIRE_LEFT_ENGINE);
			lLeftTimeToFire = 20000;
			lLeftTimeToExplode += 20000;
		}

		ShutLeftEngineDown();
//		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_LEFT;
//		PlayerPlane->DamageFlags |= (DAMAGE_BIT_LO_ENGINE|DAMAGE_BIT_L_ENGINE);

		turnoffflags |= (DAMAGE_BIT_LO_ENGINE)|(DAMAGE_BIT_L_ENG_CONT)|(DAMAGE_BIT_L_OIL_PRESS)|(DAMAGE_BIT_L_BURNER)|(DAMAGE_BIT_L_FUEL_PUMP)|(DAMAGE_BIT_L_GEN);
		warnflags |= (1<<WARN_L_ENG_CONT)|(1<<WARN_L_OIL_PRESS)|(1<<WARN_L_BURNER)|(1<<WARN_L_FUEL_PUMP)|(1<<WARN_L_GEN);
		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_LEFT;
	}
	else
	{
		ADEngineFireOut(leftside, amad);
		if(amad)
		{
			lFireFlags |= (HEAT_UP_AMAD_LEFT|FIRE_AMAD_LEFT |HEAT_UP_AMAD_RIGHT|FIRE_AMAD_RIGHT);
			lLAMADTimeToFire = 20000;
			lLAMADTimeToExplode += 20000;
		}
		else
		{
			lFireFlags |= (HEAT_UP_RIGHT_ENGINE|FIRE_RIGHT_ENGINE);
			lRightTimeToFire = 20000;
			lRightTimeToExplode += 20000;
		}

		ShutRightEngineDown();
//		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_RIGHT;
//		PlayerPlane->DamageFlags |= (DAMAGE_BIT_RO_ENGINE|DAMAGE_BIT_R_ENGINE);

		turnoffflags |= (DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_R_ENG_CONT)|(DAMAGE_BIT_R_OIL_PRESS)|(DAMAGE_BIT_R_BURNER)|(DAMAGE_BIT_R_FUEL_PUMP)|(DAMAGE_BIT_R_GEN);
		warnflags |= (1<<WARN_R_ENG_CONT)|(1<<WARN_R_OIL_PRESS)|(1<<WARN_R_BURNER)|(1<<WARN_R_FUEL_PUMP)|(1<<WARN_R_GEN);
		PlayerPlane->SystemsStatus |= PL_ENGINE_REAR_RIGHT;
	}

	PlayerPlane->SystemInactive |= turnoffflags | PlayerPlane->DamageFlags;
	for(cnt = 0; cnt < 32; cnt ++)
	{
		if(warnflags & (1<<cnt))
		{
			DisplayWarning(MASTER_CAUTION, ON, 0);
			SetMasterWarning(cnt);
		}
	}
}

#define SECONDS_TO_NORMAL 10
/*----------------------------------------------------------------------------
 *
 *	ADQuickFireSim()
 *
 */
void ADQuickFireSim()
{
	FPoint vel;
	int tempval;
	FPoint tmp(0.0f);// = checkplane->IfVelocity;
	float maxval, workval;
	long lworkval;
	int cnt;

	if((LeftEngineOn) && (!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_LEFT)) && (lFireFlags & HEAT_UP_LEFT_ENGINE))
	{
		if(lLeftTimeToFire >= 0)
		{
			if(lLeftTimeToFire >= 120000)
			{
				tempval = 1;
			}
			else
			{
				tempval = 0;
			}

			if(lLeftOrgTimeToFire < lLeftTimeToFire)
			{
				maxval = (float)lLeftOrgTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));

				workval = (maxval - lLeftOrgTimeToFire) / SECONDS_TO_NORMAL;

				workval *= ((float)DeltaTicks / 1000);

				lworkval = workval;

				lLeftTimeToFire -= lworkval;
			}
			else if(PlayerPlane->LeftThrustPercent > 60.0)
			{
				lLeftTimeToFire -= DeltaTicks<<2;
			}
			else
			{
				lLeftTimeToFire -= DeltaTicks;
			}

			if(lLeftTimeToFire < 0)
			{
				// Turn on VC Fire Light
				//if( !(lFireFlags & FIRE_LEFT_ENGINE) )
				//SetCockpitItemState( 208, 1);

				lFireFlags |= FIRE_LEFT_ENGINE;


				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						for(cnt = 0; cnt < 2; cnt ++)
						{
							SndQueueSpeech (SND_BETTY_ENGINE, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_FIRE, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_LEFT, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
						}
					}
				}
			}
			else if((tempval) && (lLeftTimeToFire < 120000))
			{
				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						// JLM commented. This speach is in flight,cpp. it is on at 1000 temperature
					}
				}
			}
		}
	}
	else if(((!LeftEngineOn) || (!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_LEFT))) && (lFireFlags & HEAT_UP_LEFT_ENGINE))
	{
		maxval = (float)lLeftOrgTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));
		lLeftTimeToFire += DeltaTicks>>1;
		if(lLeftTimeToFire > maxval)
		{
			maxval = lLeftTimeToFire;
		}
	}

	if(lFireFlags & FIRE_LEFT_ENGINE)
	{
		if(lLeftTimeToExplode >= 10000)
		{
			if(PlayerPlane->LeftThrustPercent > 60.0)
			{
				lLeftTimeToExplode -= DeltaTicks<<2;
			}
			else
			{
				lLeftTimeToExplode -= DeltaTicks;
			}


			if(lLeftTimeToExplode < 10000)
			{
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->MakeOrphan();
					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_LEFT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = 500;
				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AICAddSoundCall(AIC_WSO_On_Fire, PlayerPlane - Planes, 1000, 50);
			}
		}
		else if(lLeftTimeToExplode >= 0)
		{
			lLeftTimeToExplode -= DeltaTicks;

			if(lLeftTimeToExplode < 0)
			{
				PlayerPlane->FlightStatus |= (PL_OUT_OF_CONTROL|PL_PLANE_BLOWN_UP);
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_LEFT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = (5+rand()&7)<<6;
				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AIRGenericSpeech(AICF_WSO_SCREAM + (rand() & 1), PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);


				if (Camera1.AttachedObjectType == CO_PLANE)
				{
					if(PlayerPlane == (PlaneParams *)Camera1.AttachedObject)
					{
						if(Camera1.CameraMode == CAMERA_COCKPIT)
						{
							VKCODE hack;
							hack.vkCode = 0;									// virtual key code (may be joy button)
							hack.wFlags = 0;									// vkCode Type
							hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

							camera_mode_fixed_chase(hack);
						}
					}
				}
				PlayPositionalExplosion(WARHEAD_STANDARD_HUGE,Camera1,PlayerPlane->WorldPosition);
				NewCanister( CT_FLYING_PARTS, PlayerPlane->WorldPosition, tmp, 0.0f );
			}
		}
	}

	if((RightEngineOn) && (!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_RIGHT)) && (lFireFlags & HEAT_UP_RIGHT_ENGINE))
	{
		if(lRightTimeToFire >= 0)
		{
			if(lRightTimeToFire >= 120000)
			{
				tempval = 1;
			}
			else
			{
				tempval = 0;
			}

			if(lRightOrgTimeToFire < lRightTimeToFire)
			{
				maxval = (float)lRightOrgTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));

				workval = (maxval - lRightOrgTimeToFire) / SECONDS_TO_NORMAL;

				workval *= ((float)DeltaTicks / 1000);

				lworkval = workval;

				lRightTimeToFire -= lworkval;
			}
			else if(PlayerPlane->RightThrustPercent > 60.0)
			{
				lRightTimeToFire -= DeltaTicks<<2;
			}
			else
			{
				lRightTimeToFire -= DeltaTicks;
			}

			if(lRightTimeToFire < 0)
			{
				//if( lFireFlags & FIRE_RIGHT_ENGINE )	// Turn on VC light flag
				//SetCockpitItemState( 209. 1 );

				lFireFlags |= FIRE_RIGHT_ENGINE;
				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						for(cnt = 0; cnt < 2; cnt ++)
						{
							SndQueueSpeech (SND_BETTY_ENGINE, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_FIRE, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
							SndQueueSpeech (SND_BETTY_RIGHT, CHANNEL_BETTY,g_iBaseBettySoundLevel,1);
						}
					}
				}
			}
			else if((tempval) && (lRightTimeToFire < 120000))
			{
				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						// JLM commented. This speach is in flight,cpp. it is on at 1000 temperature
					}
				}
			}
		}
	}
	else if(((!RightEngineOn) || (PlayerPlane->SystemsStatus & PL_ENGINE_REAR_RIGHT)) && (lFireFlags & HEAT_UP_RIGHT_ENGINE))
	{
		maxval = (float)lRightOrgTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));
		lRightTimeToFire += DeltaTicks>>1;
		if(lRightTimeToFire > maxval)
		{
			maxval = lRightTimeToFire;
		}
	}

	if(lFireFlags & FIRE_RIGHT_ENGINE)
	{
		if(lRightTimeToExplode >= 10000)
		{
			if(PlayerPlane->RightThrustPercent > 60.0)
			{
				lRightTimeToExplode -= DeltaTicks<<2;
			}
			else
			{
				lRightTimeToExplode -= DeltaTicks;
			}


			if(lRightTimeToExplode < 10000)
			{
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_RIGHT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = 500;
				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AICAddSoundCall(AIC_WSO_On_Fire, PlayerPlane - Planes, 1000, 50);
			}
		}
		else if(lRightTimeToExplode >= 0)
		{
			lRightTimeToExplode -= DeltaTicks;

			if(lRightTimeToExplode < 0)
			{
				PlayerPlane->FlightStatus |= (PL_OUT_OF_CONTROL|PL_PLANE_BLOWN_UP);
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_RIGHT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = (5+rand()&7)<<6;
				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AIRGenericSpeech(AICF_WSO_SCREAM + (rand() & 1), PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);


				if (Camera1.AttachedObjectType == CO_PLANE)
				{
					if(PlayerPlane == (PlaneParams *)Camera1.AttachedObject)
					{
						if(Camera1.CameraMode == CAMERA_COCKPIT)
						{
							VKCODE hack;
							hack.vkCode = 0;									// virtual key code (may be joy button)
							hack.wFlags = 0;									// vkCode Type
							hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

							camera_mode_fixed_chase(hack);
						}
					}
				}
				PlayPositionalExplosion(WARHEAD_STANDARD_HUGE,Camera1,PlayerPlane->WorldPosition);
				NewCanister( CT_FLYING_PARTS, PlayerPlane->WorldPosition, tmp, 0.0f );
			}
		}
	}


//  AMAD  ***********************************
	if((LeftEngineOn) && (!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_LEFT)) && (lFireFlags & HEAT_UP_AMAD_LEFT))
	{
		if(lLAMADTimeToFire >= 0)
		{
			if(lLAMADTimeToFire >= 240000)
			{
				tempval = 1;
			}
			else
			{
				tempval = 0;
			}

			if(lLOrgAMADTimeToFire < lLAMADTimeToFire)
			{
				maxval = (float)lLOrgAMADTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));

				workval = (maxval - lLOrgAMADTimeToFire) / SECONDS_TO_NORMAL;

				workval *= ((float)DeltaTicks / 1000);

				lworkval = workval;

				lLAMADTimeToFire -= lworkval;
			}
			else if(PlayerPlane->LeftThrustPercent > 60.0)
			{
				lLAMADTimeToFire -= DeltaTicks<<2;
			}
			else
			{
				lLAMADTimeToFire -= DeltaTicks;
			}

			if(lLAMADTimeToFire < 0)
			{
				lFireFlags |= FIRE_AMAD_LEFT;
			}
			else if((tempval) && (lLAMADTimeToFire < 240000))
			{
				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						// JLM commented. This speach is in flight,cpp. it is on at 1000 temperature
					}
				}
			}
		}
	}
	else if(((!LeftEngineOn) || (PlayerPlane->SystemsStatus & PL_ENGINE_REAR_LEFT)) && (lFireFlags & HEAT_UP_AMAD_LEFT))
	{
		maxval = (float)lLOrgAMADTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));
		lLAMADTimeToFire += DeltaTicks>>1;
		if(lLAMADTimeToFire > maxval)
		{
			maxval = lLAMADTimeToFire;
		}
	}

	if(lFireFlags & FIRE_AMAD_LEFT)
	{
		if(lLAMADTimeToExplode >= 10000)
		{
			if(PlayerPlane->LeftThrustPercent > 60.0)
			{
				lLAMADTimeToExplode -= DeltaTicks<<2;
			}
			else
			{
				lLAMADTimeToExplode -= DeltaTicks;
			}


			if(lLAMADTimeToExplode < 10000)
			{
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_LEFT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = 500;
//				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AICAddSoundCall(AIC_WSO_On_Fire, PlayerPlane - Planes, 1000, 50);
			}
		}
		else if(lLAMADTimeToExplode >= 0)
		{
			lLAMADTimeToExplode -= DeltaTicks;

			if(lLAMADTimeToExplode < 0)
			{
				PlayerPlane->FlightStatus |= (PL_OUT_OF_CONTROL|PL_PLANE_BLOWN_UP);
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_LEFT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = (5+rand()&7)<<6;
				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AIRGenericSpeech(AICF_WSO_SCREAM + (rand() & 1), PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);


				if (Camera1.AttachedObjectType == CO_PLANE)
				{
					if(PlayerPlane == (PlaneParams *)Camera1.AttachedObject)
					{
						if(Camera1.CameraMode == CAMERA_COCKPIT)
						{
							VKCODE hack;
							hack.vkCode = 0;									// virtual key code (may be joy button)
							hack.wFlags = 0;									// vkCode Type
							hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

							camera_mode_fixed_chase(hack);
						}
					}
				}
				PlayPositionalExplosion(WARHEAD_STANDARD_HUGE,Camera1,PlayerPlane->WorldPosition);
				NewCanister( CT_FLYING_PARTS, PlayerPlane->WorldPosition, tmp, 0.0f );
			}
		}
	}

	if((RightEngineOn) && (!(PlayerPlane->SystemsStatus & PL_ENGINE_REAR_RIGHT)) && (lFireFlags & HEAT_UP_AMAD_RIGHT))
	{
		if(lRAMADTimeToFire >= 0)
		{
			if(lRAMADTimeToFire >= 240000)
			{
				tempval = 1;
			}
			else
			{
				tempval = 0;
			}

			if(lROrgAMADTimeToFire < lRAMADTimeToFire)
			{
				maxval = (float)lROrgAMADTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));

				workval = (maxval - lROrgAMADTimeToFire) / SECONDS_TO_NORMAL;

				workval *= ((float)DeltaTicks / 1000);

				lworkval = workval;

				lRAMADTimeToFire -= lworkval;
			}
			else if(PlayerPlane->RightThrustPercent > 60.0)
			{
				lRAMADTimeToFire -= DeltaTicks<<2;
			}
			else
			{
				lRAMADTimeToFire -= DeltaTicks;
			}

			if(lRAMADTimeToFire < 0)
			{
				lFireFlags |= FIRE_AMAD_RIGHT;
			}
			else if((tempval) && (lRAMADTimeToFire < 120000))
			{
				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				{
					if (g_iBaseBettySoundLevel)
					{
						// JLM commented. This speach is in flight,cpp. it is on at 1000 temperature
					}
				}
			}
		}
	}
	else if(((!RightEngineOn) || (PlayerPlane->SystemsStatus & PL_ENGINE_REAR_RIGHT)) && (lFireFlags & HEAT_UP_AMAD_RIGHT))
	{
		maxval = (float)lROrgAMADTimeToFire * (1.0f - ((20 - UFC.MilMaxTemp) / (1500.0f - UFC.MilMaxTemp)));
		lRAMADTimeToFire += DeltaTicks>>1;
		if(lRAMADTimeToFire > maxval)
		{
			maxval = lRAMADTimeToFire;
		}
	}

	if(lFireFlags & FIRE_AMAD_RIGHT)
	{
		if(lRAMADTimeToExplode >= 10000)
		{
			if(PlayerPlane->RightThrustPercent > 60.0)
			{
				lRAMADTimeToExplode -= DeltaTicks<<2;
			}
			else
			{
				lRAMADTimeToExplode -= DeltaTicks;
			}


			if(lRAMADTimeToExplode < 10000)
			{
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_RIGHT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = 500;
//				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AICAddSoundCall(AIC_WSO_On_Fire, PlayerPlane - Planes, 1000, 50);
			}
		}
		else if(lRAMADTimeToExplode >= 0)
		{
			lRAMADTimeToExplode -= DeltaTicks;

			if(lRAMADTimeToExplode < 0)
			{
				PlayerPlane->FlightStatus |= (PL_OUT_OF_CONTROL|PL_PLANE_BLOWN_UP);
				vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
				if (!PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
				{
					if (PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					 	((SmokeTrail *)PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT])->MakeOrphan();

					PlayerPlane->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(PlayerPlane,OFFSET_ENGINE_REAR_RIGHT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
				}
				PlayerPlane->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = (5+rand()&7)<<6;
				NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);
				AIRGenericSpeech(AICF_WSO_SCREAM + (rand() & 1), PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);


				if (Camera1.AttachedObjectType == CO_PLANE)
				{
					if(PlayerPlane == (PlaneParams *)Camera1.AttachedObject)
					{
						if(Camera1.CameraMode == CAMERA_COCKPIT)
						{
							VKCODE hack;
							hack.vkCode = 0;									// virtual key code (may be joy button)
							hack.wFlags = 0;									// vkCode Type
							hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

							camera_mode_fixed_chase(hack);
						}
					}
				}
				PlayPositionalExplosion(WARHEAD_STANDARD_HUGE,Camera1,PlayerPlane->WorldPosition);
				NewCanister( CT_FLYING_PARTS, PlayerPlane->WorldPosition, tmp, 0.0f );
			}
		}
	}
}


/* ------------------------------------------3/8/98 10:15AM--------------------------------------------
 *
 * ----------------------------------------------------------------------------------------------------*/

int DontCheck;
extern float OilParts[];

void CheckForPlaneCollisions()
{
	PlaneParams *P;
	ObjectTypeInfo *object_type;
	FPoint tmp(0.0f);
	FPointDouble pnow,plast;
	FPointDouble tnow,tlast;
	FPointDouble vector;
	FPointDouble vector2;
	FPoint corner_points[5];
	FPoint rel_vector2;
	float our_rad;
	float our_moved;
	float total_damage;
	float total_movement;
	float their_radius;
	float our_rad2;
	BOOL do_check;
	BOOL was_hit;
	FPointDouble point;
	FPoint bvector;
	FPoint closest_bvector;
	int numspines;
	FPoint *fpwalker;
	FPoint fpfrom,fpto;
	FPointDouble closest_point;
	double distance;
	double closest_distance;
	FPoint rel_now,rel_last;
	int i;
	MovingVehicleParams *MOStruck = NULL;

	/* ------------------------------------------3/8/98 3:04PM---------------------------------------------
	 * reset don't check flag at beginning of mission
	 * ----------------------------------------------------------------------------------------------------*/
	if (GameLoop < 50)
		DontCheck = 0;

	/* ------------------------------------------3/8/98 10:40AM--------------------------------------------
	 * don't do checking at the immediate start of a mission, only after a few seconds
	 * ----------------------------------------------------------------------------------------------------*/

//	if (!DontCheck && (GameLoop > 10*50) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED) && !MultiPlayer && (g_nMissionType != SIM_TRAIN) && (g_Settings.gp.dwFlight & GP_FLIGHT_AIR_COLLISIONS))
	if (!DontCheck && (GameLoop > 10*50) && !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED) && (g_nMissionType != SIM_TRAIN) && (g_Settings.gp.dwFlight & GP_FLIGHT_AIR_COLLISIONS))
	{
		/* ------------------------------------------3/8/98 10:33AM--------------------------------------------
	 	 * we're only going to check the players plane versus other planes
	 	 * ----------------------------------------------------------------------------------------------------*/

		pnow = PlayerPlane->WorldPosition;
		plast = PlayerPlane->LastWorldPosition;
		our_moved = PlayerPlane->DistanceMoved;
		vector2.MakeVectorToFrom(pnow,plast);
		P = Planes;
		our_rad = GetObjectRadius(PlayerPlane->Type->Model)*0.8f;
		our_rad2 = our_rad * our_rad;

		/* ------------------------------------------3/12/98 8:48PM--------------------------------------------
		 * unless the plane is below 600 feet
		 * ----------------------------------------------------------------------------------------------------*/

		if (PlayerPlane->HeightAboveGround < (600.0 FEET))
		{
			RegisteredObject *rwalker;
			BasicInstance *instance;

			rwalker = FirstInRangeObject;  //Don't have to check flat objects, you can't collide with them.

			while(rwalker)
			{
				if ((rwalker->Flags & (RO_COLLIDEABLE | RO_WAS_DRAWN)) == (RO_COLLIDEABLE | RO_WAS_DRAWN))
				{
					if (!(rwalker->Flags & RO_OBJECT_FALLING))
					{
						instance = rwalker->Instance;

						if (object_type = GetObjectTypeInfoFromInstance(instance))
						{
							if (IntersectBoundingBox(rwalker->Matrix,instance->Position,object_type->XYZExtentHigh,object_type->XYZExtentLow,plast,pnow,&point,&bvector))
							{

								if (((StructureInstance *)instance)->Type->Basics.Flags & BF_OIL_PLATFORM)
								{
									/* ----------------3/13/98 12:07PM-----------------------------------------------------------------
							 		* if this is an oil platform, we want to make sure we do the right hit detection on it.
							 		* ------------------------------------------------------------------------------------------------*/

									fpwalker = (FPoint *)OilParts;

									closest_distance = 10000000000.0;

									while(fpwalker < (FPoint *)(&OilParts[22*3]))
									{
										if (IntersectBoundingBox(rwalker->Matrix,instance->Position,fpwalker[1],*fpwalker,plast,pnow,&point,&bvector))
										{
											distance = point / plast;
											if (distance < closest_distance)
											{
												closest_point = point;
												closest_distance = distance;
												closest_bvector = bvector;
											}
										}
										fpwalker+=2;
									}

									if (closest_distance > 1000000000.0)
									{
										rwalker = rwalker->NextObject;
										continue;
									}
									else
									{
										point = closest_point;
										bvector = closest_bvector;
									}

								}


								/* --------------------------------------3/12/98 8:48PM----------------------------------------
								 * okay, our plane has smacked broadside into something now we have to blow it up
								 * --------------------------------------------------------------------------------------------*/

								DestroyPlaneItem(PlayerPlane, DAMAGE_L_ENGINE, 2);
								DestroyPlaneItem(PlayerPlane, DAMAGE_R_ENGINE, 1);

								/* ------------------------------------------3/5/98 5:18PM---------------------------------------------
		 						* If we're in the plane that just crashed, we've got to do some fancy footwork!
		 						* ----------------------------------------------------------------------------------------------------*/
								goto shameless_use_of_a_goto;

								return ;
							}
						}
					}
				}
				rwalker = rwalker->NextObject;
			}

			/* -----------------6/25/99 5:11PM---------------------------------------------------------------------
			 * here's where we put the ship collisions.
			 * ----------------------------------------------------------------------------------------------------*/

			if (PlayerPlane->HeightAboveGround < (300.0 FEET))
			{
				FPointDouble corner_points_from[5];
				FPointDouble corner_points_to[5];
				FMatrix tmat;

				//Nose
				corner_points[0].SetValues(-PlayerPlane->Type->ShadowULZOff,P->Orientation.I);

				//left wing tip

				FPoint *offset = &PlayerPlane->Type->Offsets[OFFSET_WING_LEFT];
				corner_points[1].X = PlayerPlane->Orientation.I.X*(double)offset->Z + PlayerPlane->Orientation.J.X*(double)offset->X + PlayerPlane->Orientation.K.X*(double)offset->Y;
				corner_points[1].Y = PlayerPlane->Orientation.I.Y*(double)offset->Z + PlayerPlane->Orientation.J.Y*(double)offset->X + PlayerPlane->Orientation.K.Y*(double)offset->Y;
				corner_points[1].Z = PlayerPlane->Orientation.I.Z*(double)offset->Z + PlayerPlane->Orientation.J.Z*(double)offset->X + PlayerPlane->Orientation.K.Z*(double)offset->Y;

				offset = &PlayerPlane->Type->Offsets[OFFSET_WING_RIGHT];
				corner_points[2].X = PlayerPlane->Orientation.I.X*(double)offset->Z + PlayerPlane->Orientation.J.X*(double)offset->X + PlayerPlane->Orientation.K.X*(double)offset->Y;
				corner_points[2].Y = PlayerPlane->Orientation.I.Y*(double)offset->Z + PlayerPlane->Orientation.J.Y*(double)offset->X + PlayerPlane->Orientation.K.Y*(double)offset->Y;
				corner_points[2].Z = PlayerPlane->Orientation.I.Z*(double)offset->Z + PlayerPlane->Orientation.J.Z*(double)offset->X + PlayerPlane->Orientation.K.Z*(double)offset->Y;

				offset = &PlayerPlane->Type->Offsets[OFFSET_ENGINE_REAR_LEFT];
				corner_points[3].X = PlayerPlane->Orientation.I.X*(double)offset->Z + PlayerPlane->Orientation.J.X*(double)offset->X + PlayerPlane->Orientation.K.X*(double)offset->Y;
				corner_points[3].Y = PlayerPlane->Orientation.I.Y*(double)offset->Z + PlayerPlane->Orientation.J.Y*(double)offset->X + PlayerPlane->Orientation.K.Y*(double)offset->Y;
				corner_points[3].Z = PlayerPlane->Orientation.I.Z*(double)offset->Z + PlayerPlane->Orientation.J.Z*(double)offset->X + PlayerPlane->Orientation.K.Z*(double)offset->Y;

				offset = &PlayerPlane->Type->Offsets[OFFSET_ENGINE_REAR_RIGHT];
				corner_points[4].X = PlayerPlane->Orientation.I.X*(double)offset->Z + PlayerPlane->Orientation.J.X*(double)offset->X + PlayerPlane->Orientation.K.X*(double)offset->Y;
				corner_points[4].Y = PlayerPlane->Orientation.I.Y*(double)offset->Z + PlayerPlane->Orientation.J.Y*(double)offset->X + PlayerPlane->Orientation.K.Y*(double)offset->Y;
				corner_points[4].Z = PlayerPlane->Orientation.I.Z*(double)offset->Z + PlayerPlane->Orientation.J.Z*(double)offset->X + PlayerPlane->Orientation.K.Z*(double)offset->Y;

				for (MovingVehicleParams *W=MovingVehicles; W<=LastMovingVehicle; W++)
				{
					if((W->Status & VL_ACTIVE) && (W->Type->iShipType) && W->Type && (!(W->Status & VL_INVISIBLE)))
					{
						rel_now.MakeVectorToFrom(pnow,W->WorldPosition);
						rel_last.MakeVectorToFrom(plast,W->WorldPosition);

						their_radius = GetObjectRadius(W->Type->Model);
						their_radius *= their_radius;
						their_radius += our_rad2+our_moved*our_moved;

						if ((rel_now.PartialLength() < their_radius) || (rel_last.PartialLength() < their_radius))
						{
							tmat = W->Attitude;
							tmat.Transpose();

							rel_vector2 = vector2;
							rel_vector2 *= tmat;

							rel_now *= tmat;
							rel_last *= tmat;

							fpwalker = corner_points;
							i = 5;

							while(i--)
							{
								fpfrom.RotateInto(*fpwalker++,tmat);
								fpto = fpfrom;
								fpto += rel_now;
								fpfrom += rel_last;

								if (LineCollidesWithModel(W->Type->Model,fpfrom,fpto,rel_vector2,point,bvector,(W->Status & VL_DESTROYED) ? FALSE : TRUE))
								{
									DestroyPlaneItem(PlayerPlane, DAMAGE_L_ENGINE, 2);
									DestroyPlaneItem(PlayerPlane, DAMAGE_R_ENGINE, 1);

									/* ------------------------------------------3/5/98 5:18PM---------------------------------------------
		 							* If we're in the plane that just crashed, we've got to do some fancy footwork!
		 							* ----------------------------------------------------------------------------------------------------*/

									point *= W->Attitude;
									point += W->WorldPosition;
									bvector *= W->Attitude;

									MOStruck = W;

									goto shameless_use_of_a_goto;

									return;

								}
							}
						}
					}
				}
			}
		}

		while(P <= LastPlane)
		{
			if ((P != PlayerPlane) && (P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED) && (P->Type && P->Type->Model) && (!(WIsWeaponPlane(P) && (P->AI.LinkedPlane == PlayerPlane))))
			{
				if ((fabs(P->WorldPosition.X - pnow.X) < TILE_SIZE) && (fabs(P->WorldPosition.Z - pnow.Z) <  TILE_SIZE))
				{
					do_check = FALSE;

					their_radius = GetObjectRadius(P->Type->Model)*0.8f;
					if(their_radius < our_rad)
						their_radius = our_rad;

					if(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
						their_radius = 0;

					if (our_moved > P->DistanceMoved)
					{
	 					if (((plast-P->LastWorldPosition) <= (our_moved + their_radius)) || ((plast-P->WorldPosition) <= (our_moved + their_radius)))
							do_check = 1;
					}
					else
	 					if (((P->LastWorldPosition-plast) <= (P->DistanceMoved + their_radius)) || ((P->LastWorldPosition-pnow) <= (P->DistanceMoved + their_radius)))
							do_check = 1;

					if (do_check)
					{
						was_hit = FALSE;


						if(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
						{
							vector.SetValues(-35 FEET,P->Orientation.I);
							vector.AddScaledVector(-16 FEET,P->Orientation.K);

							tnow = P->WorldPosition;
							tnow += vector;

							tlast = P->LastWorldPosition;
							tlast += vector;

							if (was_hit = CheckIntersection(plast,pnow,tlast,tnow,19 FEET))
								vector.MakeVectorToFrom(tnow,tlast);
							else
							{
								vector.SetValues(-12 FEET,P->Orientation.I);
								vector.AddScaledVector(-8 FEET,P->Orientation.K);

								tnow = P->WorldPosition;
								tnow += vector;

								tlast = P->LastWorldPosition;
								tlast += vector;

								if (was_hit = CheckIntersection(plast,pnow,tlast,tnow,12 FEET))
									vector.MakeVectorToFrom(tnow,tlast);
								else
								{
									vector.SetValues(12 FEET,P->Orientation.I);
									vector.AddScaledVector(-8 FEET,P->Orientation.K);

									tnow = P->WorldPosition;
									tnow += vector;

									tlast = P->LastWorldPosition;
									tlast += vector;

									if (was_hit = CheckIntersection(plast,pnow,tlast,tnow,12 FEET))
										vector.MakeVectorToFrom(tnow,tlast);
								}
							}

							their_radius = our_rad;
						}
						else
						{
							their_radius = GetObjectRadius(P->Type->Model)*0.8f;
							if(their_radius < our_rad)
								their_radius = our_rad;

							if (was_hit = CheckIntersection(plast,pnow,P->LastWorldPosition,P->WorldPosition,their_radius))
								vector.MakeVectorToFrom(P->WorldPosition,P->LastWorldPosition);
						}


						if (was_hit)
						{
							/* ----------------------------------------3/8/98 11:31AM------------------------------------------
							 * POW!!!
							 * ------------------------------------------------------------------------------------------------*/
							total_movement = P->DistanceMoved*our_moved;
							if (total_movement)
							{
								FMatrix temp_mat;
								float heading;
								float pitch;

								vector -= vector2;
								total_damage = (float)(vector.Length()/(0.05866 FEET)); //100 mph closure = 100 pts

								if (total_damage > 100.0f)
									total_damage = 100.0f;

								if (total_damage < 0.0f)
									total_damage = 0.0f;

								if(cPlayerVulnerable)
								{

									PlayerPlane->DoAttitude(PlayerPlane);
									temp_mat = PlayerPlane->Attitude;
									temp_mat.Transpose();
									vector *= temp_mat;
									heading = atan2(vector.Z,vector.X)*180.0f/PI;
									pitch = atan2(vector.Y,sqrt(vector.X*vector.X+vector.Z*vector.Z))*180.0f/PI;

									if (total_damage > (80.0f + frand()*20.0f))
									{
										FPoint x,y,z;
										FMatrix matrix;

										LogPlaneShotdown(PlayerPlane,NULL,0,1);

#if 0  //  removing smoke if plane disintegrated.
										OrphanAllPlaneSmoke(PlayerPlane);
#endif
										PlayerPlane->FlightStatus |= PL_OUT_OF_CONTROL;  //  |= PL_STATUS_CRASHED;
										AIRemoveFromFormation(PlayerPlane);
										DestroyPlaneItem(PlayerPlane, DAMAGE_LO_ENGINE, 1);
										DestroyPlaneItem(PlayerPlane, DAMAGE_L_ENGINE, 2);
										DestroyPlaneItem(PlayerPlane, DAMAGE_R_ENGINE, 1);
										DestroyPlaneItem(PlayerPlane, DAMAGE_RO_ENGINE, 1);


										/* ------------------------------------------3/5/98 5:18PM---------------------------------------------
		 								* If we're in the plane that just crashed, we've got to do some fancy footwork!
		 								* ----------------------------------------------------------------------------------------------------*/

										if ((PlaneParams *)Camera1.AttachedObject == PlayerPlane)
										{
											VKCODE bogus;

											bogus.vkCode = 0;									// virtual key code (may be joy button)
											bogus.wFlags = 0;									// vkCode Type
											bogus.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

											AssignCameraSubject(PlayerPlane,CO_PLANE);
											camera_mode_fixed_chase(bogus);
										}

										point = plast;
										point += pnow;
										point *= 0.5f;

										y = vector2;
										y.Normalize();

										matrix.m_Data.RC.R0C1 = y.X;
										matrix.m_Data.RC.R1C1 = y.Y;
										matrix.m_Data.RC.R2C1 = y.Z;

										if (fabs(y.Y) < 0.98f)
											z.SetValues(0.0f,1.0f,0.0f);
										else
											z.SetValues(0.0f,0.0f,1.0f);

										x = y;
										x %= z;
										x.Normalize();

										matrix.m_Data.RC.R0C0 = x.X;
										matrix.m_Data.RC.R1C0 = x.Y;
										matrix.m_Data.RC.R2C0 = x.Z;

										z = x;
										z %= y;

										matrix.m_Data.RC.R0C2 = z.X;
										matrix.m_Data.RC.R1C2 = z.Y;
										matrix.m_Data.RC.R2C2 = z.Z;

										if (_3dxl == _3DXL_SOFTWARE)
											numspines = 3 + g_Settings.gr.nExplosionDetail*2;
										else
											numspines = 6 + g_Settings.gr.nExplosionDetail*3;

										for( int i = 0; i< numspines; i++)
											NewCanister( CT_EXPLOSION_FIRE , point, tmp, 200.0f, 0.0f, &matrix);

										NewCanister(CT_FLASH,point,tmp,1.0f);
										PlayPositionalExplosion(WARHEAD_STANDARD_MEDIUM,Camera1,point);
									}
									else
									{
										DistributePlayerDamage(PlayerPlane,total_damage, DamageMultipliers, pitch, heading, their_radius*2.0, WEP_SPREAD_DAMAGE);
										if (total_damage > (40.0f + frand()+40.0f))
											PlayerPlane->FlightStatus |= PL_OUT_OF_CONTROL;
									}

								}

								P->DoAttitude(P);
								temp_mat = P->Attitude;
								temp_mat.Transpose();

								vector.RotateInto(vector2,temp_mat);

								heading = atan2(vector.Z,vector.X)*180.0f/PI;
								pitch = atan2(vector.Y,sqrt(vector.X*vector.X+vector.Z*vector.Z))*180.0f/PI;

								if (!(pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER) && (total_damage > (80.0f + frand()*20.0f)))
								{
									FPoint x,y,z;
									FMatrix matrix;

									LogPlaneShotdown(P,NULL,0,1);


#if 0  //  removing smoke if plane disintegrated.
									OrphanAllPlaneSmoke(P);
#endif

									P->FlightStatus |= PL_OUT_OF_CONTROL;  //  |= PL_STATUS_CRASHED;
									AIRemoveFromFormation(P);
									DestroyPlaneItem(P, DAMAGE_LO_ENGINE, 1);
									DestroyPlaneItem(P, DAMAGE_L_ENGINE, 2);
									DestroyPlaneItem(P, DAMAGE_R_ENGINE, 1);
									DestroyPlaneItem(P, DAMAGE_RO_ENGINE, 1);


									/* ------------------------------------------3/5/98 5:18PM---------------------------------------------
		 							* If we're in the plane that just crashed, we've got to do some fancy footwork!
		 							* ----------------------------------------------------------------------------------------------------*/

									if ((PlaneParams *)Camera1.AttachedObject == P)
									{
										VKCODE bogus;

										bogus.vkCode = 0;									// virtual key code (may be joy button)
										bogus.wFlags = 0;									// vkCode Type
										bogus.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT

										AssignCameraSubject(P,CO_PLANE);
										camera_mode_fixed_chase(bogus);
									}

									point = tlast;
									point += tnow;
									point *= 0.5f;

									y.MakeVectorToFrom(P->WorldPosition,P->LastWorldPosition);
									y.Normalize();

									matrix.m_Data.RC.R0C1 = y.X;
									matrix.m_Data.RC.R1C1 = y.Y;
									matrix.m_Data.RC.R2C1 = y.Z;

									if (fabs(y.Y) < 0.98f)
										z.SetValues(0.0f,1.0f,0.0f);
									else
										z.SetValues(0.0f,0.0f,1.0f);

									x = y;
									x %= z;
									x.Normalize();

									matrix.m_Data.RC.R0C0 = x.X;
									matrix.m_Data.RC.R1C0 = x.Y;
									matrix.m_Data.RC.R2C0 = x.Z;

									z = x;
									z %= y;

									matrix.m_Data.RC.R0C2 = z.X;
									matrix.m_Data.RC.R1C2 = z.Y;
									matrix.m_Data.RC.R2C2 = z.Z;

									if (_3dxl == _3DXL_SOFTWARE)
										numspines = 3 + g_Settings.gr.nExplosionDetail*2;
									else
										numspines = 6 + g_Settings.gr.nExplosionDetail*3;

									for( int i = 0; i< numspines; i++)
										NewCanister( CT_EXPLOSION_FIRE , point, tmp, 200.0f, 0.0f, &matrix);

									NewCanister(CT_FLASH,point,tmp,1.0f);
									PlayPositionalExplosion(WARHEAD_STANDARD_MEDIUM,Camera1,point);
								}
								else
								{
									DistributeDamage(P, total_damage*0.3, DamageMultipliers, pitch, heading, their_radius*2.0, WEP_SPREAD_DAMAGE);
								 	if (!(P->FlightStatus & PL_OUT_OF_CONTROL) && (total_damage > (30.0f + frand()+50.0f)))
									{
										P->FlightStatus |= PL_OUT_OF_CONTROL;
										AIRemoveFromFormation(P);
									}
								}


								tnow += tlast;
								tnow += pnow;
								tnow += plast;

								tnow *= 0.25;

 								NewCanister( CT_FLYING_PARTS, tnow, tmp, 0.0f );

								/* --------------------------------------3/8/98 3:05PM-----------------------------------------
								 * dont check again for 5 seconds
								 * --------------------------------------------------------------------------------------------*/

								DontCheck = 250;

								if (PlayerPlane->FlightStatus & PL_OUT_OF_CONTROL)  //  |= PL_STATUS_CRASHED)
									return;
							}

						}

					}
				}

			}

			P++;
		}
	}

	if (DontCheck-- <= 0)
		DontCheck = 0;

	return;

shameless_use_of_a_goto:

 	PlaneHitGround(point,PlayerPlane->IfVelocity,10.0f,&bvector,(void *)MOStruck);  //the 10.0f is just an arbitrary number for now

	PlaneReleaseTailhook(PlayerPlane);

	LogPlaneShotdown(PlayerPlane,NULL,0,1);

	OrphanAllPlaneSmoke(PlayerPlane);

	PlayerPlane->FlightStatus |= PL_STATUS_CRASHED;
	AIRemoveFromFormation(PlayerPlane);

	/* ------------------------------------------3/5/98 5:18PM---------------------------------------------
		* If we're in the plane that just crashed, we've got to do some fancy footwork!
		* ----------------------------------------------------------------------------------------------------*/

	if (((PlaneParams *)Camera1.AttachedObject == PlayerPlane) && (Camera1.CameraMode == CAMERA_COCKPIT))
	{
		VKCODE bogus;
		bogus.vkCode = 0;									// virtual key code (may be joy button)
		bogus.wFlags = 0;									// vkCode Type
		bogus.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


		AssignCameraSubject(PlayerPlane,CO_PLANE);
		camera_mode_fixed_chase(bogus);
	}

	if(WIsWeaponPlane(PlayerPlane))
		PlayerPlane->Status = 0;
}

//***********************************************************************************************************
void ADDamageTowedDecoy(PlaneParams *planepnt)
{
	if(planepnt == PlayerPlane)
	{
		DamageCMDDS();
		if(Av.Tews.CM.CurCMDDS < 0)
		{
			planepnt->AI.iAICombatFlags2 &= ~(AI_NOISE_JAMMER);
		}
	}
	else
	{
		if(planepnt->TowedDecoyPosition == 0.0f)
			return;

		if(planepnt->WeapLoad[MAX_F18E_STATIONS - 1].Count < 0)
		{
			planepnt->WeapLoad[MAX_F18E_STATIONS - 1].Count = planepnt->WeapLoad[MAX_F18E_STATIONS - 1].Count + 1;
			CutTowedDecoyCable(planepnt);
		}
		if(planepnt->WeapLoad[MAX_F18E_STATIONS - 1].Count == 0)
		{
			planepnt->AI.iAICombatFlags2 &= ~(AI_NOISE_JAMMER);
		}
	}
	return;
}