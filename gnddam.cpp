#include "F18.h"
#include "particle.h"
#include "GameSettings.h"
#include "3dfxF18.h"
#include "clouds.h"


ShockWave ShockWaves[MAX_SHOCK_WAVES],*LastShockWave;
ScheduledExplosion ScheduledExplosions[MAX_SCHEDULED_EXPLOSIONS],*LastScheduledExplosion;
Crater Craters[MAX_CRATERS];
LPDIRECT3DTEXTURE2 CraterModels[MAX_CRATER_MODELS];
int NumCraterModels;
extern CameraInstance *glob_camera;
extern FPoint *LastSurfaceNormal;
extern FMatrix *LastObjToViewMatrix;

FPoint ShockVectors4[4];
FPoint ShockVectors6[6];
FPoint ShockVectors8[8];
FPoint ShockVectors10[10];
FPoint ShockVectors12[12];
FPoint ShockVectors14[14];
FPoint ShockVectors16[16];

// These convert from the game specific weapon type to the much more general damage equivalence

BYTE WarHeadTypeConversion[] = {DTE_NORMAL_TARGET,		//WARHEAD_STANDARD_TINY
                             	DTE_NORMAL_TARGET,		//WARHEAD_STANDARD_SMALL
							 	DTE_NORMAL_TARGET,		//WARHEAD_STANDARD_MEDIUM
							 	DTE_NORMAL_TARGET,		//WARHEAD_STANDARD_LARGE
							 	DTE_NORMAL_TARGET,		//WARHEAD_STANDARD_HUGE
							 	DTE_ARMORED_TARGET,		//WARHEAD_ANTIARMOR_TINY
							 	DTE_ARMORED_TARGET,		//WARHEAD_ANTIARMOR_SMALL
							 	DTE_SOFT_TARGET,		//WARHEAD_ANTI_PERSONNEL
							 	DTE_SOFT_TARGET,		//WARHEAD_ICENDIARY
							 	DTE_HARDENED_TARGET,	//WARHEAD_PENETRATION
							 	DTE_HARDENED_TARGET,	//WARHEAD_CRATERING
							 	DTE_SOFT_TARGET,		//WARHEAD_CHEMICAL
							 	DTE_NORMAL_TARGET,		//WARHEAD_NUKE
							 	DTE_NORMAL_TARGET,		//WARHEAD_SAM_LARGE
							 	DTE_NORMAL_TARGET,		//WARHEAD_SAM_MEDIUM
							 	DTE_NORMAL_TARGET,		//WARHEAD_SAM_SMALL
							 	DTE_NORMAL_TARGET,		//WARHEAD_SAM_TINY
							 	DTE_NORMAL_TARGET,		//WARHEAD_AAA_LARGE
							 	DTE_NORMAL_TARGET,		//WARHEAD_AAA_MEDIUM
							 	DTE_NORMAL_TARGET,		//WARHEAD_AAA_SMALL
							 	DTE_NORMAL_TARGET,		//WARHEAD_AAA_TINY
								DTE_SOFT_TARGET,  		//WARHEAD_FUEL_AIR_EXPLOSIVE
								DTE_SOFT_TARGET,  	 	//WARHEAD_NAPALM
								DTE_SOFT_TARGET};  		//WARHEAD_SMOKE



double ShockWaveOneOverLog2Base;
extern BOOL NoFlashDuringExplosion;

#define MAKESWVECTOR dangle = 0x10000 / i; angle = 0; while (i--) {matrix.SetRPH(0,0,angle); angle += dangle; walker++->RotateInto(accum,matrix);}


void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition);
void WeaponHitsWall( FPointDouble &pos, FPointDouble &vel, float radius, float fuel, FPoint *vector,BOOL dust);
void PlaneHitGround( FPointDouble &pos, FPointDouble &vel, float fuel,FPoint *vector,void *vehicle=NULL);

char *CraterNames[] = {"SCRC-DK.PCX",
 	                   "SCRCDK.PCX",
 	                   "SCRCDK01.PCX",
 	                   "SCRCDK02.PCX",
 	                   "SCRCDK03.PCX",
 	                   "SCRCDK04.PCX",
 	                   "SCRCDK05.PCX",
 	                   "SCRCDK06.PCX",
					   "SPLASH.PCX",
					   "SPLASH2.PCX"};

typedef enum {
	EC_NEARBY_ONLY,
	EC_FARAWAY_ONLY,
	EC_BOTH
} EDGE_CHECK_VALUES;

EDGE_CHECK_VALUES GetEdgeCheckingState(const FPointDouble &pt,const float radius)
{
	double xh,xl,zh,zl;

	xl = MediumHeightCache->BLoadedULX - pt.X;
	if (xl > radius)
		return EC_FARAWAY_ONLY;

	xh = pt.X - MediumHeightCache->BLoadedLRX;
	if (xh > radius)
		return EC_FARAWAY_ONLY;

	zl = MediumHeightCache->BLoadedULY - pt.Z;
	if (zl > radius)
		return EC_FARAWAY_ONLY;

	zh = pt.Z - MediumHeightCache->BLoadedLRY;
	if (zh > radius)
		return EC_FARAWAY_ONLY;

	/* -----------------11/11/99 10:13AM-------------------------------------------------------------------
	/* we're not outside, so we must be inside or both
	/* ----------------------------------------------------------------------------------------------------*/

	if (-xl < radius)
		return EC_BOTH;

	if (-xh < radius)
		return EC_BOTH;

	if (-zl < radius)
		return EC_BOTH;

	if (-zh < radius)
		return EC_BOTH;

	return EC_NEARBY_ONLY;
}


void InitGroundFX()
{

	TextureBuff *temp_buff;
	TextureRef our_ref;
	char **cnwalk;
	int i;
	LPDIRECT3DTEXTURE2 *lptwalk;
	UsageType ut = UT_RGB_ALPHA;

	ZeroMemory(&our_ref,sizeof(our_ref));
	our_ref.CellWidth = 1.0f;
	our_ref.CellHeight = 1.0f;

	SetTextureFormat(FT_OBJECT_PALETTE);

	cnwalk = CraterNames;
	lptwalk = CraterModels;
	i = MAX_CRATER_MODELS;
	while (i--)
	{
		strcpy(our_ref.Name,*cnwalk++);
		temp_buff = FetchTexture(&our_ref, ut );
		*lptwalk++ = temp_buff->D3DTexture;
		temp_buff->D3DSurface->SetPalette(NULL);
		temp_buff->D3DSurface->Release();
		EasyFree(temp_buff);

		if (i == 2)
		{
			ut = UT_PURE_ALPHA;
			ReSetTextureFormat();
			SetTextureFormat(FT_16BIT_DATA);
		}
	}

	ReSetTextureFormat();

}

void FreeGroundFX()
{
	int i;

	i = MAX_CRATER_MODELS;
	while(i--)
		if (CraterModels[i])
			CraterModels[i]->Release();

	memset(&CraterModels[0],0,MAX_CRATER_MODELS * sizeof(LPDIRECT3DTEXTURE2));
}

void InitShockWaveStuff()
{
	//Set up vectors for the spines of the hexagon

	FPoint *walker,accum;
	FMatrix matrix;
	ANGLE angle = 0;
	ANGLE dangle;
	int i;

	accum.SetValues(0.0f,0.0f,1.0f);

	i = 16;
	walker = &ShockVectors16[0];
	MAKESWVECTOR;

	i = 14;
	walker = &ShockVectors14[0];
	MAKESWVECTOR;

	i = 12;
	walker = &ShockVectors12[0];
	MAKESWVECTOR;

	i = 10;
	walker = &ShockVectors10[0];
	MAKESWVECTOR;

	i = 8;
	walker = &ShockVectors8[0];
	MAKESWVECTOR;

	i = 6;
	walker = &ShockVectors6[0];
	MAKESWVECTOR;

	i = 4;
	walker = &ShockVectors4[0];
	MAKESWVECTOR;

	//This is to help us know how many sides to put on the polygon.
	ShockWaveOneOverLog2Base = 1.0 / (log(pow(LP_LOW_MAX_DISTANCE*0.75,0.166666)) / log(2.0));

	memset(&ScheduledExplosions[0],0,MAX_SCHEDULED_EXPLOSIONS*sizeof(ScheduledExplosion));

	memset(&Craters[0],0,MAX_CRATERS * sizeof(Crater));
	memset(&ShockWaves[0],0,MAX_SHOCK_WAVES*sizeof(ShockWave));
	LastShockWave = &ShockWaves[-1];
	LastScheduledExplosion = &ScheduledExplosions[-1];
}


extern void LogDestroyedObject(WeaponParams *W,int iType,int iID);
extern void LogWeaponHit(WeaponParams *W,int iType,int iID);

void DoClusterBombDamage(WeaponParams *pweapon,DBWeaponType *pweapon_type)
{
	MovingVehicleParams *checkvehicle;
	float radius,distance;
	float damage_per_unit;
	float object_area;
	float damage_to_object;
	DBWeaponType *bomblet_type;

	BOOL done_checking;
	BOOL check_mode;
	BasicInstance *walker,*iwalker;
	RegisteredObject *rwalker;

	BOOL blew_something_up=FALSE;
	BOOL damaged_something=FALSE;
	MultipleExplosionType *secondaries;
	MultipleExplosionType *swalker;
	MultipleExplosionType damage_cache;
	int num_secondaries;
	DWORD hit_value;
	float secondary_delay;
	int grndside = 0;
	int totalsecondaries = 0;
	int easy_direct_hit = 0;
	int objectnear = 0;
	int skip = 0;
	int directhit = 0;

	unsigned int iOrgCRS;
#ifdef _DEBUG
	char tstr[256];
#endif _DEBUG
	float randval;
	int allownet = (MultiPlayer) ? 0 : 1;

	if(MultiPlayer)
	{
		if(pweapon)
		{
			if((pweapon->P==PlayerPlane) || (pweapon->P && (pweapon->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
			{
				allownet = 1;
			}
			else if((!pweapon->P) && (lAINetFlags1 & NGAI_ACTIVE))
			{
				allownet = 1;
			}
		}
	}


	DamagePasser dpasser;

	float dist_delay;
	BasicInstance *instance_in;

	bomblet_type = &pDBWeaponList[AIGetWeaponIndex(pweapon_type->lSubWeaponID)];

	if (bomblet_type && pweapon_type->lNumberOfSubWeapon)
	{
		/* ------------------------------------------2/27/98 6:30PM--------------------------------------------
		 * first get the radius of the bomblet dispersal
		 * ----------------------------------------------------------------------------------------------------*/

		if(!MultiPlayer)
		{
			radius = 0.39f*(float)(GameLoop - (DWORD)pweapon->lTimer);
		}
		else
		{
			radius = 0.39f*(float)(sClusterTimer);
		}

		if (radius > 255.0f)
			radius = 255.0f;

		if (radius < 30.0f)
			radius = 30.0f;

		/* ------------------------------------------2/27/98 6:32PM--------------------------------------------
		 * now figure out the total damage units of this weapon, that is # of bomblets * damage / radius of dispersal
		 * ----------------------------------------------------------------------------------------------------*/

		damage_per_unit = ( float )(pweapon_type->lNumberOfSubWeapon * bomblet_type->iDamageValue)/(float)radius;

		/* ------------------------------------------2/27/98 6:41PM--------------------------------------------
		 * now we have to set up the graphic effect
		 * ----------------------------------------------------------------------------------------------------*/

		int number = pweapon_type->lNumberOfSubWeapon >> 4;

		if (number < 30)
			number = 30;

		if (number > 250)
			number = 250;

		int timebetween = 7*50/(float)number;
		int deviation = timebetween;
		if (!deviation)
			deviation = 1;

		MultipleExplosionType exp = {pweapon_type->lSubWeaponID,
									(BYTE)number,
									(BYTE)radius,
									MET_NO_DAMAGE | SE_CULL_LIGHTS,
									(SHORT)timebetween,
									(SHORT)deviation};


		ScheduleExplosion(pweapon->Pos,exp,0.0f);

		/* ------------------------------------------2/27/98 7:39PM--------------------------------------------
		 * now we'll go through the objects inside the radius, and create explosions for them so it looks like
		 * things land on top of them.
		 * ----------------------------------------------------------------------------------------------------*/

		done_checking = FALSE;

		EDGE_CHECK_VALUES edge_checking_state = GetEdgeCheckingState(pweapon->Pos,radius);

		if (edge_checking_state == EC_NEARBY_ONLY)
		{
			damaged_something = FALSE;

			if (rwalker = FirstInRangeObject)
				check_mode = 2;
			else
			{
				if (rwalker = FirstOutOfRangeObject)
					check_mode = 1;
				else
					done_checking = TRUE;

			}
		}
		else
		{
			iwalker = AllInstances;
			check_mode = 0;
		}

#ifdef _DEBUG
		sprintf(tstr, "START damage_per_unit %f, Timer %ld, %f radius \n\r", damage_per_unit, sClusterTimer, radius);

		OutputDebugString( tstr );
#endif _DEBUG


		while(!done_checking) // && (!blew_something_up))
		{
			if (check_mode)
				if (!(rwalker->Flags & RO_OBJECT_FALLING))
					walker = rwalker->Instance;
				else
					walker = NULL;
			else
			{
				walker = iwalker;
				if ((edge_checking_state == EC_FARAWAY_ONLY) && ((StructureInstance *)iwalker)->Registration)
					walker = NULL;
			}

			if (walker && (walker->Family != FAMILY_RUNWAYPIECE) && InstanceIsClusterBombable(walker))
			{
				distance = walker->Position - pweapon->Pos;
				if (distance < radius)
				{
					/* ------------------------------------------2/27/98 8:05PM--------------------------------------------
					 * get the (approximate) radius of the object and compute the radius at PI*r^2
					 * ----------------------------------------------------------------------------------------------------*/

					if (check_mode)
						object_area = (QuickDistance(rwalker->ExtentLeft,rwalker->ExtentTop) + QuickDistance(rwalker->ExtentRight,rwalker->ExtentBottom))*0.5;
					else
						object_area = 12 FEET;

					object_area *= object_area*PI;

					iOrgCRS = iClusterRandSeed;

					if(MultiPlayer)
					{
						srand(iClusterRandSeed);
						iClusterRandSeed += rand();
					}

					randval = frand();
//					damage_to_object = object_area*damage_per_unit*(1.5f - frand());
					damage_to_object = object_area*damage_per_unit*(1.5f - randval);

					if (!(BOOL)(rand()&3))
					{
						hit_value = DTE_DIRECT_HIT;
						directhit = 1;
					}
					else
						hit_value = 0;

#ifdef _DEBUG
					sprintf(tstr, "CBomb SN %ld, CRS %u, damage %f, dirhit %d, frand %f \n\r", walker->SerialNumber, iOrgCRS, damage_to_object, directhit, randval);

					OutputDebugString( tstr );
#endif _DEBUG


					hit_value |= (DWORD)WarHeadTypeConversion[bomblet_type->iWarHead];

					if(((pweapon->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))) || (netCall & GX_FAKE_DAMAGE))
					{
						hit_value = GDGetUnRealHitValue(hit_value, walker);
					}

					LogWeaponHit(pweapon,GROUNDOBJECT,walker->SerialNumber);

					dpasser.CacheDamage = FALSE;
					dpasser.ExplosionSize = 0;
					dpasser.FireType = CT_NONE;

					damaged_something = TRUE;

					if (DamageInstance(walker,hit_value,GameLoop,damage_to_object,&num_secondaries,&secondary_delay,&secondaries,&dpasser))
					{
						LogDestroyedObject(pweapon,GROUNDOBJECT,walker->SerialNumber);

						blew_something_up = TRUE;
						totalsecondaries += num_secondaries;

						if(dpasser.CacheDamage)
						{
							instance_in = walker;
							dist_delay = frand()*(float)(timebetween*number)*0.45f;
							if ((!num_secondaries) || (secondary_delay > dist_delay))
							{
								num_secondaries++;
								swalker = &damage_cache;
							}
							else
								swalker = secondaries;
						}
						else
						{
							instance_in = FALSE;
							swalker = secondaries;
						}


						if ((dpasser.ExplosionSize || dpasser.FireType) && (!num_secondaries))
						{
							dist_delay = 0;
							num_secondaries = 1;
							secondaries = &damage_cache;
						}

						while(num_secondaries--)
						{
							ScheduleExplosion(walker->Position,*swalker, dist_delay,(BOOL)(swalker != &damage_cache),instance_in,&dpasser);
							instance_in = NULL;
							dpasser.ExplosionSize = 0;
							dpasser.FireType = CT_NONE;
							dist_delay = secondary_delay;
							if (swalker++ == &damage_cache)
								swalker = secondaries;
						}

						//NewGenerator(PLANE_EXPLODES,walker->Position,0.0,2.0f,450); 					//This should be where the visual effects for the instance
						//New Smoke TEst
//								AddSmoke(FX_FLYING_DEBRIS, (StructureInstance* )walker, 0, *position, 0);
						//AddSmoke(FX_DAMAGE_SMOKE, NULL, 0, *position, 0);
						//FPoint vel(0.0f);
						//NewCanister( CT_BLACK_SMOKE, *position, vel, 0.0f );

						AIUpdateGoalStates(walker->SerialNumber,0,0,GROUNDOBJECT);
						GDCheckForClients(walker);
						if(PlayerPlane->AI.iSide == GDConvertGrndSide(walker))
						{
							grndside = PlayerPlane->AI.iSide;
						}
					}
					else if(iFACState)
					{
						if((pweapon->P == PlayerPlane) && (walker->SerialNumber == FACObject.dwSerialNumber))
						{
							AIC_FAC_NearMiss_Msg(PlayerPlane - Planes);
						}
					}
				}
			}

			if (check_mode)
			{
				rwalker = rwalker->NextObject;
				if (!rwalker)
				{
					if (!damaged_something && (check_mode-- == 2))
					{
						rwalker = FirstOutOfRangeObject;
						if (!rwalker)
						{
							done_checking=TRUE;
						}
					}
					else
						done_checking = TRUE;
				}
			}
			else
				if (!(iwalker = (BasicInstance *)iwalker->NextInstance))
					done_checking = TRUE;

		}

		/* ------------------------------------------2/27/98 8:46PM--------------------------------------------
		 * now for the vehicles
		 * ----------------------------------------------------------------------------------------------------*/

		for(checkvehicle = MovingVehicles; (checkvehicle <= LastMovingVehicle) && (allownet); checkvehicle ++)
		{
			if((checkvehicle->Status & (VL_ACTIVE)) && (!(checkvehicle->Status & (VL_DESTROYED|VL_INVISIBLE))))
			{
				distance = checkvehicle->WorldPosition - pweapon->Pos;
				if (distance < radius)
				{
					if (checkvehicle->Type && checkvehicle->Type->Model)
						object_area = GetObjectRadius(checkvehicle->Type->Model);
					else
						object_area = 12 FEET;

					if((checkvehicle->iShipType) && (object_area > (12 FEET)))
					{
						object_area = 12 FEET;
					}

					if(object_area > radius)
						object_area = radius;

					object_area *= object_area*PI;

					damage_to_object = object_area*damage_per_unit*(1.5f - frand());

					if (!(BOOL)(rand()&3))
					{
						hit_value = DTE_DIRECT_HIT;
						directhit = 1;
					}
					else
						hit_value = 0;

					hit_value |= (DWORD)WarHeadTypeConversion[bomblet_type->iWarHead];

					if(((pweapon->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))) || (netCall & GX_FAKE_DAMAGE))
					{
						hit_value = GDGetUnRealHitValue(hit_value, walker);
					}

					if (checkvehicle->iShipType)
						LogWeaponHit(pweapon,SHIP,(checkvehicle-MovingVehicles));
					else
						LogWeaponHit(pweapon,MOVINGVEHICLE,(checkvehicle-MovingVehicles));

					if (VDamageVehicle(checkvehicle, hit_value, GameLoop, damage_to_object, &num_secondaries, &secondary_delay, &damage_cache))
					{

						LogDestroyedObject(pweapon,MOVINGVEHICLE,(checkvehicle - MovingVehicles));

						blew_something_up = TRUE;
						totalsecondaries += num_secondaries;

						dist_delay = frand()*(float)(timebetween*number)*0.45f;

						if (num_secondaries)
						{

							if (!secondary_delay)
								ScheduleExplosion(checkvehicle->WorldPosition,damage_cache, dist_delay,TRUE,NULL,NULL,checkvehicle);
							else
							{
								ScheduleExplosion(checkvehicle->WorldPosition,damage_cache, dist_delay,FALSE,NULL,NULL,checkvehicle);
								ScheduleExplosion(checkvehicle->WorldPosition,damage_cache, secondary_delay + dist_delay);
							}
						}
						else
							ScheduleExplosion(checkvehicle->WorldPosition,damage_cache, dist_delay,FALSE,NULL,NULL,checkvehicle);

						if(PlayerPlane->AI.iSide == checkvehicle->iSide)
						{
							grndside = PlayerPlane->AI.iSide;
						}
					}
				}
			}
		}

		if (pweapon)
			AICBombResultSpch(pweapon,blew_something_up, totalsecondaries, grndside, directhit);
	}
}

BOOL NapalmCopied;
DBWeaponType FakeNapalmWeapon;


void DoNapalmDamage(WeaponParams *pweapon,DBWeaponType *pweapon_type)
{
	/* -----------------8/15/99 3:12PM---------------------------------------------------------------------
	/* all this really does is set a bunch of fires on vehicles and small structures
	/* ----------------------------------------------------------------------------------------------------*/


	if (!NapalmCopied)
	{
		memcpy(&FakeNapalmWeapon,pweapon_type,sizeof(DBWeaponType));
		FakeNapalmWeapon.iDamageRadius = 100.0f;
		FakeNapalmWeapon.iDamageValue = 5.0f;
		NapalmCopied = TRUE;
	}

	int allownet = (MultiPlayer) ? 0 : 1;

	if(MultiPlayer)
	{
		if(pweapon)
		{
			if((pweapon->P==PlayerPlane) || (pweapon->P && (pweapon->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
			{
				allownet = 1;
			}
			else if((!pweapon->P) && (lAINetFlags1 & NGAI_ACTIVE))
			{
				allownet = 1;
			}
		}
	}

	/* -----------------8/15/99 3:13PM---------------------------------------------------------------------
	/* first thing we want to do is set the fires on the ground
	/* They will be delayed roughly by their distance / bomb's horizontal velocity
	/* ----------------------------------------------------------------------------------------------------*/

	FPointDouble position = pweapon->Pos;
	FPointDouble start_position;
	FPointDouble fire_position;

	FPoint lnormal;
	FPoint velocity_vec,move_normal;
	FPoint perp_vector;
	FPoint perp_h_vector;
	FPoint perp_h_normal;
	FPoint tmp(0.0f);
	FMatrix land_mat;
//	float perp_velocity;
	float velocity;
	float perp_distance;
	float circle_like;
	float water_mod = 1.0f;

	// get the normal to the location we hit
	if (InHighResTerrainRegion(position))
	{
		if (InWater(position.X, position.Z))
			water_mod = 0.25f;
	 	LandNormal(position,lnormal);
	 	GetMatrix(position,0,land_mat);
	}
	else
	{
		lnormal.SetValues(0.0f,1.0f,0.0f);
		land_mat.Identity();
	}


	// get our vector
	velocity_vec.MakeVectorToFrom(pweapon->Pos,pweapon->LastPos);
	move_normal = velocity_vec;
	velocity = move_normal.Normalize();

	/* -----------------8/15/99 4:02PM---------------------------------------------------------------------
	/* figure out how we're moving perpendicular to the ground.  This is for the length of the blast
	/* ----------------------------------------------------------------------------------------------------*/
	perp_vector = move_normal;
	perp_vector.AddScaledVector(-(move_normal * lnormal),lnormal);

	if (!perp_vector.Normalize())
		perp_vector.SetValues(land_mat.m_Data.RC.R0C2,land_mat.m_Data.RC.R1C2,land_mat.m_Data.RC.R2C2);

	/* -----------------8/15/99 4:03PM---------------------------------------------------------------------
	/* get the vector for the width of the blast
	/* ----------------------------------------------------------------------------------------------------*/
	perp_h_normal = perp_vector;
	perp_h_normal %= lnormal;
	perp_h_normal.Normalize();

	// if we hit straight down, the blast should be more of a circle than a line
	circle_like = fabs(lnormal * move_normal);

	/* -----------------8/15/99 3:49PM---------------------------------------------------------------------
	/* 30 degree angle of incidence is optimal. Less will be treated as 30 (0.5 == cos 30)
	/*
	/* We'll keep the range 0 to 0.5 so that 1-circle like is in the range 0.5 - 1.0.  That let's us figure
	/* out the radius of the circle more easily
	/* ----------------------------------------------------------------------------------------------------*/
	if (circle_like < 0.5f)
		circle_like = 0.5f;
	else
		circle_like = circle_like - 0.5f;

	float fBlastRadius = pweapon_type->iDamageRadius * FTTOWU * water_mod;


	fBlastRadius *= 1.0f-circle_like;

	/* -----------------8/15/99 4:00PM---------------------------------------------------------------------
	/* now we have to figure out where to start setting the fires.
	/* ----------------------------------------------------------------------------------------------------*/
	perp_distance = -fBlastRadius;
	float final_dist = fBlastRadius * (1.0f-circle_like);

	perp_distance += final_dist;

	float spread_width = final_dist * (0.1f + circle_like)*(1.0f/0.6f);
	float oospread_width = 1.0f/spread_width;

	perp_h_vector.SetValues(spread_width,perp_h_normal);


	start_position = position;
	start_position.AddScaledVector(perp_distance,perp_vector);
	float z_dist = perp_distance;
	float spread_length = (final_dist - z_dist);
	float oospread_length = 1.0f/spread_length;

	// 5 fires +- the fire detail level
	float step_distance = (final_dist - z_dist)/(4.0f+3.0f*(float)g_Settings.gr.nExplosionDetail);

	/* -----------------8/15/99 4:09PM---------------------------------------------------------------------
	/* we're just gonna say that it takes 2 seconds for full dispersal.  That's a cheat, but so what
	/* ----------------------------------------------------------------------------------------------------*/
	float tmult = 2.0f/final_dist;
	float t;
	float move_dist;
	float fire_size;
	float delay_time;
	float new_t;

	NewCanister(CT_FLASH,position,tmp,1.5f);
	PlayPositionalExplosion(pweapon_type->iWarHead,Camera1,pweapon->Pos);

	NoFlashDuringExplosion = TRUE;
	NewExplosionWeapon( &FakeNapalmWeapon, pweapon->Pos );
	NoFlashDuringExplosion = FALSE;

	position = start_position;
	water_mod *= water_mod;

	while(z_dist < final_dist)
	{
		move_dist = (1.0f + (frand()*0.5-0.25f))*step_distance;
		z_dist += move_dist;
		position.AddScaledVector(move_dist,perp_vector);

		t = fabs(z_dist *  tmult * 0.5f);

		fire_position = position;
		new_t = t*2.0f*frand() - t;
		fire_position.AddScaledVector(new_t,perp_h_vector);
		new_t = sqrt(new_t*new_t*t*t)*(0.7072);
		fire_size =  + 0.5f + frand();
		delay_time = new_t*2.0f;

		NewCanister( CT_OIL_FIRE, fire_position, tmp,fire_size * (1.0f - new_t)*1.5f, (new_t*3.0f+2.0f*frand())*5.0f*water_mod,NULL,delay_time);
		NewCanister( CT_EXPLOSION_FIRE , fire_position, tmp, (1.0f - new_t)*350.0f, 0.0f, &land_mat,delay_time);
	}

	/* -----------------8/15/99 5:56PM---------------------------------------------------------------------
	/* now we have to burn vehicles and small instances which are nearby
	/* ----------------------------------------------------------------------------------------------------*/

	int grndside = 0;
	BOOL damaged_something=FALSE;

	if (water_mod == 1.0f)
	{
		MovingVehicleParams *checkvehicle;

		BOOL done_checking;
		BasicInstance *walker,*iwalker;
		RegisteredObject *rwalker;

		FPoint ddelta;
		BOOL check_mode;
		int objectnear = 0;
		int skip = 0;

		float vt,ht;
		float dx, dz, offangle, tdist, fworkdistx, fworkdistz, fworkdist;
		int facnum;
		int ifacTargetType = -1;
		void *pfacTarget = NULL;

		if((pweapon->P) && (lBombFlags & WSO_FAC_CHECK))
		{
			facnum = AICGetClosestFAC(pweapon->P);
			if(facnum == -2)
			{
				pfacTarget = pFACTarget;
				ifacTargetType = iFACTargetType;
			}
			else if(facnum != -1)
			{
				pfacTarget = Planes[facnum].AI.pGroundTarget;
				ifacTargetType = Planes[facnum].AI.lGroundTargetFlag;
			}
		}

		done_checking = FALSE;

		EDGE_CHECK_VALUES edge_checking_state = GetEdgeCheckingState(pweapon->Pos,fBlastRadius);

		if (edge_checking_state == EC_NEARBY_ONLY)
		{
			damaged_something = FALSE;

			if (rwalker = FirstInRangeObject)
				check_mode = 2;
			else
			{
				if (rwalker = FirstOutOfRangeObject)
					check_mode = 1;
				else
					done_checking = TRUE;

			}
		}
		else
		{
			iwalker = AllInstances;
			check_mode = 0;
		}

		while(!done_checking) // && (!blew_something_up))
		{
			if (check_mode)
				if (!(rwalker->Flags & RO_OBJECT_FALLING))
					walker = rwalker->Instance;
				else
					walker = NULL;
			else
			{
				walker = iwalker;
				if ((edge_checking_state == EC_FARAWAY_ONLY) && ((StructureInstance *)iwalker)->Registration)
					walker = NULL;
			}

			if (walker && (walker->Family != FAMILY_RUNWAYPIECE) && InstanceIsClusterBombable(walker))
			{
				ddelta.MakeVectorToFrom(walker->Position,start_position);
				vt = (ddelta * perp_vector) * oospread_length;
				ht = fabs(ddelta * perp_h_normal) * oospread_width;
				if ((vt >= 0.0) && (vt <=  1.0f) && (ht <= 1.0f))
				{
					t = sqrt(vt*vt+ht*ht)*(0.7072);

					InstanceStartNapalmFire(walker,t,pweapon->P);
					damaged_something = TRUE;

					if(PlayerPlane->AI.iSide == GDConvertGrndSide(walker))
						grndside = PlayerPlane->AI.iSide;
				}
				else
					if((pweapon->P == PlayerPlane) && (walker->SerialNumber == FACObject.dwSerialNumber) && (vt > -0.5f) && (vt < 1.5f) && (ht < 1.5f))
						AIC_FAC_NearMiss_Msg(PlayerPlane - Planes);
					else
						if((ifacTargetType == GROUNDOBJECT) && (pweapon->P == PlayerPlane))
						{
							if(walker == (BasicInstance *)pfacTarget)
							{
								dx = pweapon->Pos.X - walker->Position.X;
								dz = pweapon->Pos.Z - walker->Position.Z;

								offangle = atan2(-dx, -dz) * 57.2958;
								offangle -= AIConvertAngleTo180Degree(pweapon->Heading);

								offangle = AICapAngle(offangle);

								tdist = QuickDistance(dx,dz) * WUTOFT;

								fworkdistz = fabs(cos(DegToRad(offangle)) * tdist);
								fworkdistx = fabs(sin(DegToRad(offangle)) * tdist);

								if(fabs(offangle) < 45.0f)
								{
									fworkdist = fabs(fworkdistz);

									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_LONG;
									}
								}
								else if(fabs(offangle) > 135.0f)
								{
									fworkdist = fabs(fworkdistz);

									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_SHORT;
									}
								}
								else if(offangle < 0)
								{
									fworkdist = fabs(fworkdistx);

									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_RIGHT;
									}
								}
								else
								{
									fworkdist = fabs(fworkdistx);
									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_LEFT;
									}
								}
							}
						}
			}

			if (check_mode)
			{
				rwalker = rwalker->NextObject;
				if (!rwalker)
				{
					if (!damaged_something && (check_mode-- == 2))
					{
						rwalker = FirstOutOfRangeObject;
						if (!rwalker)
						{
							done_checking=TRUE;
						}
					}
					else
						done_checking = TRUE;
				}
			}
			else
				if (!(iwalker = (BasicInstance *)iwalker->NextInstance))
					done_checking = TRUE;

		}

		/* ------------------------------------------2/27/98 8:46PM--------------------------------------------
			* now for the vehicles
			* ----------------------------------------------------------------------------------------------------*/

		for(checkvehicle = MovingVehicles; (checkvehicle <= LastMovingVehicle) && (allownet); checkvehicle ++)
		{
			if((checkvehicle->Status & (VL_ACTIVE)) && (!(checkvehicle->Status & (VL_DESTROYED|VL_INVISIBLE))))
			{
				ddelta.MakeVectorToFrom(checkvehicle->WorldPosition,start_position);
				vt = (ddelta * perp_vector) * oospread_length;
				ht = fabs(ddelta * perp_h_normal) * oospread_width;
				if ((vt >= 0.0) && (vt <=  1.0f) && (ht <= 1.0f))
				{
					t = sqrt(vt*vt+ht*ht)*(0.7072);

					VehicleStartNapalmFire(checkvehicle,t,pweapon->P);
					if (checkvehicle->iShipType)
						LogWeaponHit(pweapon,SHIP,(checkvehicle-MovingVehicles));
					else
						LogWeaponHit(pweapon,MOVINGVEHICLE,(checkvehicle-MovingVehicles));
					damaged_something = TRUE;

					if(PlayerPlane->AI.iSide == checkvehicle->iSide)
						grndside = PlayerPlane->AI.iSide;
				}
				else
				{
					if ((vt >= -1.5) && (vt <=  2.5f) && (ht <= 2.5f))
					{
						// only stop the cars if the fire is in front of them
						if ((ddelta.X*checkvehicle->Attitude.m_Data.RC.R0C2 + ddelta.Y*checkvehicle->Attitude.m_Data.RC.R1C2 + ddelta.Z*checkvehicle->Attitude.m_Data.RC.R2C2) >= 0 )
						{
							checkvehicle->lAITimer1 = 15000;
							checkvehicle->Status |= (VL_WAITING|VL_WAIT_DESTRUCTION);
							checkvehicle->Status &= ~(VL_CHECK_FLAGS|VL_MOVING);
						}
					}

					if((ifacTargetType == MOVINGVEHICLE) && (pweapon->P == PlayerPlane))
					{
						if(checkvehicle == (MovingVehicleParams *)pfacTarget)
						{
							dx = pweapon->Pos.X - checkvehicle->WorldPosition.X;
							dz = pweapon->Pos.Z - checkvehicle->WorldPosition.Z;

							offangle = atan2(-dx, -dz) * 57.2958;
							offangle -= AIConvertAngleTo180Degree(pweapon->Heading);

							offangle = AICapAngle(offangle);

							tdist = QuickDistance(dx,dz) * WUTOFT;

							if(fabs(offangle) < 45.0f)
							{
								fworkdist = fabs(cos(DegToRad(offangle)) * tdist);
								if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
								{
									dFACRange = fworkdist;
									iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
									iFACHitFlags |= FAC_HIT_LONG;
								}
							}
							else if(fabs(offangle) > 135.0f)
							{
								fworkdist = fabs(cos(DegToRad(offangle)) * tdist);
								if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
								{
									dFACRange = fworkdist;
									iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
									iFACHitFlags |= FAC_HIT_SHORT;
								}
							}
							else if(offangle < 0)
							{
								fworkdist = fabs(sin(DegToRad(offangle)) * tdist);
								if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
								{
									dFACRange = fworkdist;
									iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
									iFACHitFlags |= FAC_HIT_RIGHT;
								}
							}
							else
							{
								fworkdist = fabs(sin(DegToRad(offangle)) * tdist);
								if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
								{
									dFACRange = fworkdist;
									iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
									iFACHitFlags |= FAC_HIT_LEFT;
								}
							}
						}
					}

				}
			}
		}
	}

	AICBombResultSpch(pweapon,FALSE, 0, grndside, damaged_something);

}

#define D_CLIP_Y_HIGH	0x01
#define D_CLIP_Y_LOW	0x02
#define D_CLIP_Y		0x03

#define D_CLIP_X_HIGH	0x04
#define D_CLIP_X_LOW	0x08
#define D_CLIP_X		0x0C

#define D_CLIP_Z_HIGH	0x10
#define D_CLIP_Z_LOW	0x20
#define D_CLIP_Z		0x30

BOOL IntersectBoundingBox(FMatrix &mat,FPointDouble &center,FPoint &High,FPoint &Low,FPointDouble &froms,FPointDouble &tos,FPointDouble *inter,FPoint *normal)
{
	FPointDouble from,to;
	FMatrix new_mat;
	FPointDouble clippers;
	FPointDouble delta;
	double result;
	double t;
	BYTE code;

	new_mat = mat;
	new_mat.Transpose();

	from.MakeVectorToFrom(froms,center);
	from *= new_mat;

	to.MakeVectorToFrom(tos,center);
	to *= new_mat;

	/* -----------------3/3/98 6:33PM--------------------
	 * first we'll do some trivial rejection stuff
	 * --------------------------------------------------*/

	code = 0;

	if (from.Y > High.Y)
	{
		if (to.Y > High.Y)
			return FALSE;
		else
		{
			clippers.Y = High.Y-0.25;
			code |= D_CLIP_Y_HIGH;
		}
	}
	else
		if (from.Y < Low.Y)
		{
			if (to.Y < Low.Y)
				return FALSE;
			else
			{
				clippers.Y = Low.Y+0.25;
				code |= D_CLIP_Y_LOW;
			}
		}


	if (from.X < Low.X)
	{
		if (to.X < Low.X)
			return FALSE;
		 else
		 {
			clippers.X = Low.X+0.25;
		 	code |= D_CLIP_X_LOW;
		 }
	}
	else
		if (from.X > High.X)
		{
			if (to.X > High.X)
				return FALSE;
			else
		 	{
				clippers.X = High.X-0.25;
				code |= D_CLIP_X_HIGH;
			}
		}

	if (from.Z < Low.Z)
	{
		if (to.Z < Low.Z)
			return FALSE;
		 else
		 {
			clippers.Z = Low.Z+0.25;
		 	code |= D_CLIP_Z_LOW;
		 }
	}
	else
		if (from.Z > High.Z)
		{
			if (to.Z > High.Z)
				return FALSE;
			else
			{
				clippers.Z = High.Z-0.25;
				code |= D_CLIP_Z_HIGH;
			}
		}

	/* -----------------3/3/98 6:39PM--------------------
	 * if the from point is inside the box, then we're already done!
	 * --------------------------------------------------*/

	if (!code)
	{
		if (inter)
			*inter = froms;
		return TRUE;
	}

	delta.MakeVectorToFrom(to,from);

	if (code & D_CLIP_X)
	{
		t = (clippers.X - from.X)/(to.X - from.X);
		result = from.Y + delta.Y*t;

		if ((result <= High.Y) && (result >= Low.Y))
		{
			result = from.Z + delta.Z*t;
			if ((result <= High.Z) && (result >= Low.Z))
			{
				if (inter)
				{
					*inter = froms;
					inter->X += (tos.X - froms.X)*t;
					inter->Y += (tos.Y - froms.Y)*t;
					inter->Z += (tos.Z - froms.Z)*t;
				}

				if (normal)
				{
					if (to.X > from.X)
						normal->SetValues(-1.0f,0.0f,0.0f);
					else
						normal->SetValues(1.0f,0.0f,0.0f);

					*normal *= mat;

				}
				return TRUE;
			}
		}
	}

	if (code & D_CLIP_Z)
	{
		t = (clippers.Z - from.Z)/(to.Z - from.Z);
		result = from.Y + delta.Y*t;

		if ((result <= High.Y) && (result >= Low.Y))
		{
			result = from.X + delta.X*t;
			if ((result <= High.X) && (result >= Low.X))
			{
				if (inter)
				{
					*inter = froms;
					inter->X += (tos.X - froms.X)*t;
					inter->Y += (tos.Y - froms.Y)*t;
					inter->Z += (tos.Z - froms.Z)*t;
				}

				if (normal)
				{
					if (to.Z > from.Z)
						normal->SetValues(0.0f,0.0f,-1.0f);
					else
						normal->SetValues(0.0f,0.0f,1.0f);

					*normal *= mat;
				}

				return TRUE;
			}
		}

	}

	if (code & D_CLIP_Y)
	{
		t = (clippers.Y - from.Y)/(to.Y - from.Y);
		result = from.X + delta.X*t;

		if ((result <= High.X) && (result >= Low.X))
		{
			result = from.Z + delta.Z*t;
			if ((result <= High.Z) && (result >= Low.Z))
			{
				if (inter)
				{
					*inter = froms;
					inter->X += (tos.X - froms.X)*t;
					inter->Y += (tos.Y - froms.Y)*t;
					inter->Z += (tos.Z - froms.Z)*t;
				}

				if (normal)
				{
					if (to.Y > from.Y)
						normal->SetValues(0.0f,-1.0f,0.0f);
					else
						normal->SetValues(0.0f,1.0f,0.0f);

					*normal *= mat;
				}

				return TRUE;
			}
		}

	}

	return FALSE;

}

/*
 *                                                       ÚÄÄÄ¿
 *                                                       ³   ³
 *                                                       ³   ³
 *                                                       ³   ³
 *                                                       ³   ³
 *                                                       ³   ³
 *                                                       ³   ³
 *                                                       ³   ³
 *   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                   ³ 10³
 *   ³ 0             1              2³                   ³   ³
 *   ³                               ³              ÚÄÄÄÄÁÄÄÄÁÄÄÄÄÄ¿
 *   ³                               ³              ³              ³
 *   ³                               ³              ³      9       ³
 *   ³                               ³    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *   ³                               ³    ³                                  ³
 *   ³                               ³    ³                8                 ³
 *   ³                               ³    ³ÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄ´
 *   ³                              3³    ³   ³           ³  ³            ³  ³
 *   ³ 7                             ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³                               ³    ³   ³           ³  ³            ³  ³
 *   ³ 6             5              4³    ³   ³           ³  ³            ³  ³
 *   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    ÀÄÄÄÙ           ÀÄÄÙ            ÀÄÄÙ
 */




float OilParts[22*3] = {-270.0f FEET,    0.0f FEET,-270.0f FEET, //Leg 0
					    -230.0f FEET,  200.0f FEET,-230.0f FEET,

					     -20.0f FEET,    0.0f FEET,-270.0f FEET, //Leg 1
					      20.0f FEET,  200.0f FEET,-230.0f FEET,

						 230.0f FEET,    0.0f FEET,-270.0f FEET, //Leg 2
						 270.0f FEET,  200.0f FEET,-230.0f FEET,

						 230.0f FEET,    0.0f FEET, -20.0f FEET, //Leg 3
						 270.0f FEET,  200.0f FEET,  20.0f FEET,

						 230.0f FEET,    0.0f FEET, 230.0f FEET, //Leg 4
						 270.0f FEET,  200.0f FEET, 270.0f FEET,

						 -20.0f FEET,	 0.0f FEET,	230.0f FEET, //Leg 5
						  20.0f FEET,  200.0f FEET,	270.0f FEET,

						-270.0f FEET,    0.0f FEET, 230.0f FEET, //Leg 6
						-230.0f FEET,  200.0f FEET, 270.0f FEET,

						-270.0f FEET,    0.0f FEET, -20.0f FEET, //Leg 7
						-230.0f FEET,  200.0f FEET,  20.0f FEET,

						-300.0f FEET,  200.0f FEET,-300.0f FEET, //Platform
						 300.0f FEET,  250.0f FEET, 300.0f FEET,

						-176.0f FEET,  250.0f FEET,-198.0f FEET, //squat platform
						 176.0f FEET,  300.0f FEET, 198.0f FEET,

						 -25.0f FEET,  300.0f FEET, -25.0f FEET, //tower
						  25.0f FEET,  966.0f FEET,  25.0f FEET};



BOOL CheckMissileAgainstGroundTargets(WeaponParams *W)
{
	RegisteredObject *walker;
	BasicInstance *instance;
	ObjectTypeInfo *object_type;
	MovingVehicleParams *vwalker;
	FPointDouble point;
	FPoint *fpwalker;
	FPointDouble closest_point;
	double distance;
	double closest_distance;
	FMatrix tmat;
	FPoint fpfrom,fpto;
	FPoint rel_now,rel_last;
	FPoint bvector;
	float their_radius;
	float our_rad;
	float our_rad2;
	float our_moved;
	float work_damage,work_radius;
	FPoint rel_vector2;
	FPointDouble vector2;
	DBWeaponType *pweapon_type;

	walker = FirstInRangeObject;
	W->CollisionCheckTimer = 5;

	if ((int)W->Type < 0x200)
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	else
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];


	while(walker)
	{
		if (walker->Flags & RO_COLLIDEABLE)
		{
			if (!(walker->Flags & RO_OBJECT_FALLING))
			{
				instance = walker->Instance;

				if (object_type = GetObjectTypeInfoFromInstance(instance))
				{
					if (IntersectBoundingBox(walker->Matrix,instance->Position,object_type->XYZExtentHigh,object_type->XYZExtentLow,W->LastPos,W->Pos,&point,&bvector))
					{
						BOOL do_graphic;

						if (((StructureInstance *)instance)->Type->Basics.Flags & BF_OIL_PLATFORM)
						{
							/* ----------------3/13/98 12:07PM-----------------------------------------------------------------
							 * if this is an oil platform, we want to make sure we do the right hit detection on it.
							 * ------------------------------------------------------------------------------------------------*/

							fpwalker = (FPoint *)OilParts;

							closest_distance = 10000000000.0;

							while(fpwalker < (FPoint *)(&OilParts[22*3]))
							{
								if (IntersectBoundingBox(walker->Matrix,instance->Position,fpwalker[1],*fpwalker,W->LastPos,W->Pos,&point,&bvector))
								{
									distance = point / W->LastPos;
									if (distance < closest_distance)
									{
										closest_point = point;
										closest_distance = distance;
									}
								}
								fpwalker+=2;
							}

							if (closest_distance > 1000000000.0)
							{
								walker = walker->NextObject;
								continue;
							}
							else
								point = closest_point;
						}

						W->Pos = point;
						if (W->Smoke)
						{
							((SmokeTrail *)W->Smoke)->Update();
							((SmokeTrail *)W->Smoke)->MakeOrphan();
						}
						W->Smoke = NULL;

						if (bvector.Y > 0.99f)
							do_graphic = FALSE;
						else
							do_graphic = TRUE;

						work_radius = (float)pweapon_type->iDamageRadius FEET * 2.0;
						work_damage = (float)pweapon_type->iDamageValue;

						if (work_damage < 100.0f)
						{
							work_damage *= 1.8f;
							work_radius *= 1.8f;
						}
						else
							if (work_damage < 500.0f)
							{
								work_damage *= 1.2f;
								work_radius *= 1.2f;
							}
						work_damage = 1.0/850.0f;

						if (pweapon_type->iWarHead == WARHEAD_NAPALM)
							PlaneHitGround(W->Pos,W->Vel,1.0f,&bvector,NULL);
						else
						{
							WeaponHitsWall(W->Pos,W->Vel,work_radius,work_damage,&bvector,(BOOL)(pweapon_type->iWeaponType == WEAPON_TYPE_GUN));
							W->Pos.Y = LandHeight( W->Pos.X, W->Pos.Z);
							do_graphic &= ((point.Y - W->Pos.Y) < work_radius);
							DoExplosionOnGround(W,0,NULL,NULL,(~do_graphic) & 1);
						}
						return TRUE;
					}
				}
			}
		}
		walker = walker->NextObject;
	}

	vwalker = &MovingVehicles[0];

	vector2.MakeVectorToFrom(W->Pos,W->LastPos);

	our_moved = vector2.Length();

	our_rad = GetObjectRadius(W->Type->Model)*0.8f + our_moved;
	our_rad2 = our_rad * our_rad;

	while(vwalker <= LastMovingVehicle)
	{
//		if (vwalker->Type && (vwalker->Type->Flags & VT_COLLIDEABLE) && ((vwalker->Status & (VL_ACTIVE | VL_DESTROYED | VL_FALLING | VL_INVISIBLE | VL_IN_VISUAL_RANGE)) == (VL_ACTIVE | VL_IN_VISUAL_RANGE)))
		if (vwalker->Type && (vwalker->Type->Flags & VT_COLLIDEABLE) && ((vwalker->Status & (VL_ACTIVE | VL_DESTROYED | VL_FALLING | VL_INVISIBLE)) == (VL_ACTIVE)))
		{
			rel_now.MakeVectorToFrom(W->Pos,vwalker->WorldPosition);
			rel_last.MakeVectorToFrom(W->LastPos,vwalker->WorldPosition);

			their_radius = GetObjectRadius(vwalker->Type->Model);
			their_radius *= their_radius;
			their_radius += our_rad2+our_moved*our_moved;

			if ((rel_now.PartialLength() < their_radius) || (rel_last.PartialLength() < their_radius))
			{

				tmat = vwalker->Attitude;
				tmat.Transpose();

				rel_vector2 = vector2;
				rel_vector2 *= tmat;

				rel_now *= tmat;
				rel_last *= tmat;

				if (LineCollidesWithModel(vwalker->Type->Model,rel_last,rel_now,rel_vector2,point,bvector,FALSE))
				{

					BOOL do_graphic;

					point *= vwalker->Attitude;
					point += vwalker->WorldPosition;

					if (bvector.Y > 0.99f)
						do_graphic = FALSE;
					else
						do_graphic = TRUE;

					bvector *= vwalker->Attitude;

					W->Pos = point;
					if (W->Smoke)
					{
						((SmokeTrail *)W->Smoke)->Update();
						((SmokeTrail *)W->Smoke)->MakeOrphan();
					}
					W->Smoke = NULL;

					work_radius = (float)pweapon_type->iDamageRadius FEET * 2.0;
					work_damage = (float)pweapon_type->iDamageValue;

					if (work_damage < 100.0f)
					{
						work_damage *= 1.8f;
						work_radius *= 1.8f;
					}
					else
						if (work_damage < 500.0f)
						{
							work_damage *= 1.2f;
							work_radius *= 1.2f;
						}
					work_damage = 1.0/850.0f;

					if (pweapon_type->iWarHead == WARHEAD_NAPALM)
						PlaneHitGround(W->Pos,W->Vel,1.0f,&bvector,(void *)vwalker);
					else
					{
						WeaponHitsWall(W->Pos,W->Vel,work_radius,work_damage,&bvector,(BOOL)(pweapon_type->iWeaponType == WEAPON_TYPE_GUN));

						W->Pos.Y = LandHeight( W->Pos.X, W->Pos.Z);
						do_graphic &= ((point.Y - W->Pos.Y) < work_radius);

						DoExplosionOnGround(W,0,NULL,NULL,(~do_graphic) & 1);
					}

					return TRUE;
				}
			}
		}
		vwalker++;
	}


	W->AccelOrientation = W->Pos;

	return FALSE;

}

double GetDistanceFromNormalBox(FMatrix &mat,FPointDouble &center,FPoint &High,FPoint &Low,FPointDouble &test,BOOL skip_radius)
{
	FPointDouble new_test;
	FPointDouble test_point;
	FMatrix new_mat;
	int count;
	new_mat = mat;

	count = 2;

	new_mat.Transpose();

	new_test.MakeVectorToFrom(test,center);
	new_test *= new_mat;

	if (skip_radius)
	{
		if ((new_test.X <= High.X) && (new_test.X >= Low.X) && (new_test.Y <= High.Y) && (new_test.Y >= Low.Y) && (new_test.Z <= High.Z) && (new_test.Z >= Low.Z))
			return 0.0;
		else
			return 10000.0;
	}
	else
	{
		if (new_test.X <= High.X)
			if (new_test.X >= Low.X)
			{
				count--;
				test_point.X = new_test.X;
			}
			else
				test_point.X = Low.X;
		else
			test_point.X = High.X;

		if (new_test.Y <= High.Y)
			if (new_test.Y >= Low.Y)
			{
				count--;
				test_point.Y = new_test.Y;
			}
			else
				test_point.Y = Low.Y;
		else
			test_point.Y = High.Y;

		if (new_test.Z <= High.Z)
			if (new_test.Z >= Low.Z)
			{
				if (!count)
					return (0.0);
				else
					test_point.Z = new_test.Z;
			}
			else
				test_point.Z = Low.Z;
		else
			test_point.Z = High.Z;
	}


	return test_point - new_test;

}

// This will damage objects from the explosion, and initiate graphics and sound

BOOL DamageAnInstance(BasicInstance *walker,DWORD hit_value,float fDistance,float fDamageValue,int &totalsecondaries,BOOL &blew_something_up)
{
	BOOL return_val;
	DamagePasser dpasser;
	BasicInstance *instance_in;
	float dist_delay;
	int num_secondaries;
	float secondary_delay;
	MultipleExplosionType *secondaries;
	MultipleExplosionType *swalker;
	MultipleExplosionType damage_cache;

	dpasser.CacheDamage = FALSE;
	dpasser.ExplosionSize = 0;
	dpasser.FireType = CT_NONE;

	if (return_val = DamageInstance(walker,hit_value,GameLoop,fDamageValue,&num_secondaries,&secondary_delay,&secondaries,&dpasser))
	{

		AIUpdateGoalStates(walker->SerialNumber,0,0,GROUNDOBJECT);

		GDCheckForClients(walker);


		blew_something_up = TRUE;
		totalsecondaries += num_secondaries;

		if(dpasser.CacheDamage)
		{
			instance_in = walker;
			dist_delay = fDistance * (float)(1.0 / MACH_1_GU_PER_50TH);
			if ((num_secondaries) && (!secondary_delay))
				swalker = secondaries;
			else
			{
				secondary_delay += dist_delay;
				num_secondaries++;
				swalker = &damage_cache;
			}
		}
		else
		{
			instance_in = FALSE;
			swalker = secondaries;
		}

		if ((dpasser.ExplosionSize || dpasser.FireType) && (!num_secondaries))
		{
			dist_delay = 0;
			num_secondaries = 1;
			secondaries = &damage_cache;
		}

		while(num_secondaries--)
		{
			if ((swalker->WarheadID == -1) && (BOOL)(swalker != &damage_cache))
			{
				PauseForDiskHit();
				if(lpDD)
				  lpDD->FlipToGDISurface();
				DebugBreak();
				UnPauseFromDiskHit();
			}

			ScheduleExplosion(walker->Position,*swalker, dist_delay,(BOOL)(swalker != &damage_cache),instance_in,&dpasser);
			instance_in = NULL;
			dpasser.ExplosionSize = 0;
			dpasser.FireType = CT_NONE;
			dist_delay = secondary_delay;
			if (swalker++ == &damage_cache)
				swalker = secondaries;
		}

		if (((StructureInstance *)walker)->Type->Basics.Flags & BF_OIL_PLATFORM)
		{
			FPointDouble aposition( 300.0 FEET, 0.0, -180.0 FEET);
			FPointDouble bposition(-300.0 FEET, 0.0, -300.0 FEET);
			FPointDouble pos1(0.0, 0.0,-300.0 FEET);
			FMatrix mat;
			mat.SetRPH(0,0,walker->Heading);
			aposition *= mat;
			bposition *= mat;
			pos1 *= mat;
			pos1 *= -0.66;
			aposition += walker->Position;
			bposition += walker->Position;

			num_secondaries = 3;
			dpasser.ExplosionSize = 700.0f;
			dpasser.ExplosionRadius = 700.0f;
			dpasser.FireType = CT_NONE;
			dist_delay = 3.0*50.0f;

			while(num_secondaries--)
			{

				ScheduleExplosion(aposition,damage_cache, dist_delay,FALSE,NULL,&dpasser);
				dist_delay+=16;
				ScheduleExplosion(bposition,damage_cache, dist_delay,FALSE,NULL,&dpasser);
				dist_delay+=16;
				aposition += pos1;
				bposition += pos1;
			}
		}


	}

	return return_val;
}


void DoExplosionOnGround(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type,BOOL skip_graphic)
{
	float fBlastRadius=0;
	float fDamageValue;
	float fDistance;
	float fMultiplier;
	float fCheckDistance;
	BOOL blew_something_up=FALSE;
	BOOL damaged_something=FALSE;
	BOOL done_checking;
	BOOL check_mode;
	BasicInstance *walker,*iwalker;
	RegisteredObject *rwalker;
	ObjectTypeInfo *object_type;
	DWORD hit_value;
	FPointDouble closest_corner;
	FPointDouble UL,LR;
	int grndside = 0;
	int totalsecondaries = 0;
	int close;
	int check_runways;
	int easy_direct_hit = 0;
	int isgun = 0;
	int objectnear = 0;
	int directhit = 0;
	int allownet = (MultiPlayer) ? 0 : 1;
	int facnum;
	int ifacTargetType = -1;
	int tick_pauser;
	void *pfacTarget = NULL;
	float dx, dz, offangle, tdist, fworkdistx, fworkdistz, fworkdist;
	EDGE_CHECK_VALUES edge_checking_state;

	if(MultiPlayer)
	{
		if(pweapon)
		{
			if((pweapon->P==PlayerPlane) || (pweapon->P && (pweapon->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
			{
				NetPutWeaponGExplode(pweapon, do_big_bang, position, pweapon_type, skip_graphic);
				allownet = 1;
			}
			else if((!pweapon->P) && (lAINetFlags1 & NGAI_ACTIVE))
			{
				NetPutWeaponGExplode(pweapon, do_big_bang, position, pweapon_type, skip_graphic);
				allownet = 1;
			}
			else if(!netCall)
			{
				return;
			}
		}
	}

	tick_pauser = GetTickCount() - CurrentTick;
//   	PauseForDiskHit();  //remove this

	check_runways = FALSE;

	if((pweapon) && ((!MultiPlayer) || (pweapon->Launcher != NULL)))
	{
		position = &pweapon->Pos;
		if ((int)pweapon->Type < 0x200)
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)pweapon->Type)];
		else
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex(pweapon->Type->TypeNumber)];

		if(pweapon_type->iWeaponType != WEAPON_TYPE_GUN)
		{
			GDNoticeExplosion(pweapon);
		}

		if (pweapon_type->iWeaponType == WEAPON_TYPE_CLUSTER_BOMB)
		{
			DoClusterBombDamage(pweapon,pweapon_type);
			goto finis;
		}

		if (pweapon_type->iWarHead == WARHEAD_NAPALM)
		{
			DoNapalmDamage(pweapon,pweapon_type);
			goto finis;
		}

		switch(pweapon_type->iWarHead)
		{
			case WARHEAD_STANDARD_HUGE:
			case WARHEAD_STANDARD_LARGE:
			case WARHEAD_CRATERING:
				check_runways = TRUE;

			case WARHEAD_STANDARD_MEDIUM:
			case WARHEAD_FUEL_AIR_EXPLOSIVE:
			case WARHEAD_NUKE:
			case WARHEAD_SAM_LARGE:
			case WARHEAD_AAA_LARGE:
				do_big_bang = TRUE;
				break;

			default:
				do_big_bang = FALSE;
				break;
		}

	}
	else if(pweapon)
	{
		position = &pweapon->Pos;
		if ((int)pweapon->Type < 0x200)
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)pweapon->Type)];
		else
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex(pweapon->Type->TypeNumber)];
	}

	if(pweapon)
	{
//		if((pweapon->P == PlayerPlane) && (lBombFlags & WSO_FAC_CHECK))
		if((pweapon->P) && (lBombFlags & WSO_FAC_CHECK))
		{
			facnum = AICGetClosestFAC(pweapon->P);
			if(facnum == -2)
			{
				pfacTarget = pFACTarget;
				ifacTargetType = iFACTargetType;
			}
			else if(facnum != -1)
			{
				pfacTarget = Planes[facnum].AI.pGroundTarget;
				ifacTargetType = Planes[facnum].AI.lGroundTargetFlag;
			}
		}
	}

	if(pweapon_type)
	{
		if(pweapon_type->lWeaponID == 134)
		{
			FPoint vel(0.0f);
			DWORD hCanister;

//			hCanister = NewCanister( CT_STEAM_SMOKE, *position, vel, 0.0f, 300);
			int smokeid = 0;
			if(pweapon)
			{
				if(pweapon->P)
				{
					smokeid = (pweapon->P - Planes) % 5;
				}
			}

			switch(smokeid)
			{
				case 1:
					hCanister = NewCanister( CT_GREEN_MARKER_SMOKE, *position, vel, 0.0f, 300);
					break;
				case 2:
					hCanister = NewCanister( CT_RED_MARKER_SMOKE, *position, vel, 0.0f, 300);
					break;
				case 3:
					hCanister = NewCanister( CT_BLUE_MARKER_SMOKE, *position, vel, 0.0f, 300);
					break;
				case 4:
					hCanister = NewCanister( CT_YELLOW_MARKER_SMOKE, *position, vel, 0.0f, 300);
					break;
				default:
					hCanister = NewCanister( CT_ORANGE_MARKER_SMOKE, *position, vel, 0.0f, 300);
					break;
			}
			goto finis;
		}
	}

	if(pweapon_type)
	{
		if(pweapon_type->iWeaponType == WEAPON_TYPE_GUN)
		{
			isgun = 1;
		}
	}
	else if(pweapon)
	{
		if(pweapon->Kind == BULLET)
		{
			isgun = 1;
		}
	}

	fBlastRadius = (pweapon_type->iDamageRadius * FTTOWU);  // Feet

	if (fBlastRadius < TILE_SIZE*2)
		fCheckDistance = TILE_SIZE*2;
	else
	{
		if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
			fCheckDistance = fBlastRadius * 1.5f;
		else
			fCheckDistance = fBlastRadius;
	}


	done_checking = FALSE;

	edge_checking_state = GetEdgeCheckingState(*position,fCheckDistance);

	if (edge_checking_state == EC_NEARBY_ONLY)
	{
		damaged_something = FALSE;

		if (rwalker = FirstInRangeObject)
		{
			if (check_runways)
				check_mode = 3;//first check the in range objects, then the flat ones
			else
				check_mode = 2;
		}
		else
		{
			if (rwalker = FirstOutOfRangeObject)
				check_mode = 1;
			else
				done_checking = TRUE;

		}
	}
	else
	{
		if (iwalker = AllInstances)
			check_mode = 0;
		else
			done_checking = TRUE;
	}

	// This will now place an explosion and mark the goal blow up
	// flag for everything within the blast rectangle.
	// If you it to be for just on target then put return value
	// back in the conditional of the while

	if (do_big_bang)
		MakeShockWave(*position,pweapon_type);

	// This is to prevent the following code from crashing given there
	// are no objects (no instances) in the world

	if (fBlastRadius)
	{

		fMultiplier = ((float)pweapon_type->iDamageValue) / (fBlastRadius * fBlastRadius);

		if(isgun)
		{
			if(pweapon)
			{
				if(pweapon->lTargetDistFt)
				{
					fMultiplier *= pweapon->lTargetDistFt;
				}
			}
		}

		while(!done_checking) // && (!blew_something_up))
		{
			if (check_mode)
				if (!(rwalker->Flags & RO_OBJECT_FALLING))
					walker = rwalker->Instance;
				else
					walker = NULL;
			else
			{
				walker = iwalker;
				if ((edge_checking_state == EC_FARAWAY_ONLY) && ((StructureInstance *)iwalker)->Registration)
					walker = NULL;
			}

			if (walker && InstanceIsBombable(walker))
			{
				if ((check_runways && (walker->Family == FAMILY_RUNWAYPIECE)) && (!isgun))
				{
					if (RunwayInterract((RunwayPieceInstance *)walker,*position,TRUE,&close,NULL,(int *)&hit_value))
					{
						if (!hit_value)
						{
							blew_something_up = TRUE;

							LogWeaponHit(pweapon,GROUNDOBJECT,walker->SerialNumber);

							if (DamageRunwayPieceInstance((RunwayPieceInstance *)walker))
							{
								//AddSmoke(FX_DAMAGE_SMOKE, NULL, 0, *position, 0);
								//FPoint vel(0.0f);
								//NewCanister( CT_BLACK_SMOKE, *position, vel, 0.0f );

								LogDestroyedObject(pweapon,GROUNDOBJECT,walker->SerialNumber);

								AIUpdateGoalStates(walker->SerialNumber,0,0,GROUNDOBJECT);
								GDCheckForClients(walker);
								if(PlayerPlane->AI.iSide == GDConvertGrndSide(walker))
								{
									grndside = PlayerPlane->AI.iSide;
								}
							}
						}
					}
//					if(close)
//					{
//						directhit = 1;
//					}
				}
				else
					if ((walker->Family != FAMILY_RUNWAYPIECE) && (fabs(walker->Position.X - position->X) < fCheckDistance) && (fabs(walker->Position.Z - position->Z) < fCheckDistance))
					{
						hit_value = 0;
						objectnear |= 1;

						if (check_mode && (object_type = GetObjectTypeInfoFromInstance(walker)))
						{
							fDistance = (float)GetDistanceFromNormalBox(rwalker->Matrix,walker->Position,object_type->XYZExtentHigh,object_type->XYZExtentLow,*position,isgun);

							if((pweapon && (pweapon->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_BOMB_ACCURACY))) || (netCall & GX_FAKE_ACCURACY))
							{
								fDistance *= 0.5;
								if (fDistance < GetObjectRadius(object_type->Model))
								{
									hit_value = DTE_DIRECT_HIT;
									directhit = 1;
								}
								else
									if (isgun)
										fDistance = 1000.0f;

							}
							else
								if (fDistance < ( float )(2.0 FEET))
								{
									hit_value = DTE_DIRECT_HIT;
									directhit = 1;
								}
								else
									if (isgun)
										fDistance = 1000.0f;

						}
						else
						{
							fDistance = walker->Position - *position;
							if (fDistance < (float)(100.0 FEET))
							{
								hit_value = DTE_DIRECT_HIT;
								fDistance = 0.0f;
								directhit = 1;
							}
							else
								if (isgun)
								{
									if (fDistance < (float)(400.0 FEET))
										objectnear = 2;
									fDistance = 1000.0f;
								}
						}

						if (fDistance < fBlastRadius)
						{

							fDamageValue = fBlastRadius - fDistance;
							fDamageValue = fDamageValue * fDamageValue * fMultiplier;

							damaged_something = TRUE;

							hit_value |= (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];

							if(pweapon)
							{
								if(((pweapon->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))) || (netCall & GX_FAKE_DAMAGE))
								{
									hit_value = GDGetUnRealHitValue(hit_value, walker);
								}
							}

							LogWeaponHit(pweapon,GROUNDOBJECT,walker->SerialNumber);

//							if((!isgun) && (!soundplayed))
//							{
//								PlayPositionalExplosion(pweapon_type->iWarHead,Camera1,walker->Position);
//								soundplayed = 1;
//							}


							if (DamageAnInstance(walker,hit_value,fDistance,fDamageValue,totalsecondaries,blew_something_up))
							{
								if(ifacTargetType == GROUNDOBJECT)
								{
									if(walker == (BasicInstance *)pfacTarget)
									{
										if(pweapon->P == PlayerPlane)
										{
											iFACHitFlags |= FAC_HIT_DESTROYED;
										}
										else
										{
											AIC_FACAck(pweapon->P - Planes, 17);
										}
									}
								}

								LogDestroyedObject(pweapon,GROUNDOBJECT,walker->SerialNumber);

								if(PlayerPlane->AI.iSide == GDConvertGrndSide(walker))
								{
									grndside = PlayerPlane->AI.iSide;
								}

							}
							else
							{
								if ((pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE) && InstanceIsClusterBombable(walker))
									InstanceStartHeatFire(walker,fDistance,fBlastRadius,(pweapon) ? (void *)pweapon->P : NULL);

								if((pweapon) && (iFACState) && (isgun == 0) && (lBombFlags & WSO_FAC_CHECK) && (pweapon->P == PlayerPlane))
								{
									if((pweapon->P == PlayerPlane) && (walker->SerialNumber == FACObject.dwSerialNumber))
									{
										AIC_FAC_NearMiss_Msg(PlayerPlane - Planes);
									}
									else if(PlayerPlane->AI.iSide == GDConvertGrndSide(walker))
									{
										grndside = PlayerPlane->AI.iSide;
										iFACHitFlags |= FAC_HIT_FRIEND;
									}
									else if(ifacTargetType == GROUNDOBJECT)
									{
										if(walker == (BasicInstance *)pfacTarget)
										{
											iFACHitFlags |= FAC_HIT_DAMAGE;
										}
									}
								}
							}

							if((isgun) && (pweapon))
							{
								AICBombResultSpch(pweapon,blew_something_up, totalsecondaries, grndside, directhit);
								DeleteBullet(pweapon);
								goto finis;
							}
						}
						else
						{
							if ((pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE) && InstanceIsClusterBombable(walker))
								InstanceStartHeatFire(walker,fDistance,fBlastRadius,(pweapon) ? (void *)pweapon->P : NULL);

							if((ifacTargetType == GROUNDOBJECT) && (pweapon->P == PlayerPlane))
							{
								if(walker == (BasicInstance *)pfacTarget)
								{
									dx = position->X - walker->Position.X;
									dz = position->Z - walker->Position.Z;

									offangle = atan2(-dx, -dz) * 57.2958;
									offangle -= AIConvertAngleTo180Degree(pweapon->Heading);

									offangle = AICapAngle(offangle);

									tdist = QuickDistance(dx,dz) * WUTOFT;

									fworkdistz = fabs(cos(DegToRad(offangle)) * tdist);
									fworkdistx = fabs(sin(DegToRad(offangle)) * tdist);

									if(fabs(offangle) < 45.0f)
									{
										fworkdist = fabs(fworkdistz);

										if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
										{
											dFACRange = fworkdist;
											iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
											iFACHitFlags |= FAC_HIT_LONG;
										}
									}
									else if(fabs(offangle) > 135.0f)
									{
										fworkdist = fabs(fworkdistz);

										if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
										{
											dFACRange = fworkdist;
											iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
											iFACHitFlags |= FAC_HIT_SHORT;
										}
									}
									else if(offangle < 0)
									{
										fworkdist = fabs(fworkdistx);

										if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
										{
											dFACRange = fworkdist;
											iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
											iFACHitFlags |= FAC_HIT_RIGHT;
										}
									}
									else
									{
										fworkdist = fabs(fworkdistx);
										if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
										{
											dFACRange = fworkdist;
											iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
											iFACHitFlags |= FAC_HIT_LEFT;
										}
									}
								}
							}
						}
					}
				else if (((!check_runways) && (walker->Family == FAMILY_RUNWAYPIECE)) && (!isgun))
				{
					FPointDouble local;

					local.X = position->X;
					local.Y = position->Y;
					local.Z = position->Z;
					local -= walker->Position;

					if(!((fabs(local.X) > (TILE_SIZE*0.502)) || (fabs(local.Z) > (TILE_SIZE*0.502))))
					{
						directhit = 1;
					}
				}
			}

			if (check_mode)
			{
				rwalker = rwalker->NextObject;
				if (!rwalker)
				{
					if (check_mode == 3)
					{
						if (rwalker=FirstInRangeFlatObject)// have to check for runway hits
							check_mode = 2;
						else
						{
							check_mode = 1;
							if ((!damaged_something) || !(rwalker = FirstOutOfRangeObject))
								done_checking=TRUE;
						}

					}
					else
						if (!damaged_something && (check_mode-- == 2))
						{
							rwalker = FirstOutOfRangeObject;
							if (!rwalker)
							{
								done_checking=TRUE;
							}
						}
						else
							done_checking = TRUE;
				}
			}
			else
				if (!(iwalker = (BasicInstance *)iwalker->NextInstance))
					done_checking = TRUE;
		}

		//**************************************************************************
		//  Bomb Result Speech is being called in DoExplosionOnVehicles so we don't
		//  get mixed messages.
		//
		//**************************************************************************
//		if (pweapon)
//			AICBombResultSpch(pweapon,blew_something_up, totalsecondaries, grndside, directhit);


		//else
		// Speach for secondaries?

		//NewGenerator(PLANE_EXPLODES,*position,0.0,2.0f,50);	//This should be the explosion graphic for the weapon type
//		AddSmoke(FX_DAMAGE_SMOKE, NULL, 0, *position, 0);
		//for( int x=0; x<8 ; x++)
		//	AddSmoke(FX_FLYING_DEBRIS, (StructureInstance* )walker, 0, *position, 0);
		//FPointDouble pos;
		//pos = *position;
		//pos.Y += 3 METERS;
		//if( InWater(pos.X, pos.Z) )
		//	NewCanisterExplosion( CT_EXPLOSION_WATER, pos, frand(), 0.0f, NULL);
		//else
		//	NewCanisterExplosion( CT_EXPLOSION_FIRE , pos, frand(), 0.0f, NULL);

		if(/*(isgun == 0) &&*/ (!skip_graphic))
		{
			NewExplosionWeapon( pweapon_type, *position );
		}
	}

	if(allownet)
	{
		DoExplosionOnVehicles(pweapon, do_big_bang, position, pweapon_type, blew_something_up, totalsecondaries, grndside, directhit);
	}

	if((!isgun) && (pweapon))
	{
		PlayPositionalExplosion(pweapon_type->iWarHead,Camera1,pweapon->Pos);
	}

	if((pweapon) && (isgun))
	{
		if((objectnear & 2) && ((pweapon->lTimer > 20) || (pweapon->lTimer < 0)))
		{
			pweapon->lTimer = 20;
		}
		else if((objectnear & 1) && ((pweapon->lTimer > 100) || (pweapon->lTimer < 0)))
		{
			pweapon->lTimer = 100;
		}
		else if((pweapon->lTimer > 500) || (pweapon->lTimer < 0))
		{
			pweapon->lTimer = 500;
		}
	}

//	if(isgun == 0 && (!MultiPlayer || (pweapon && (pweapon->P == PlayerPlane))))
	if(isgun == 0 && (allownet))
	{
		DoExplosionInAir(pweapon, do_big_bang, position, pweapon_type);
	}
//	UnPauseFromDiskHit(); //remove this

	finis:
		if (!MultiPlayer)
			CurrentTick = GetTickCount()-tick_pauser;
}

// This will damage objects from the explosion, and initiate graphics and sound
void DoExplosionOnVehicles(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type, BOOL blew_something_up, int totalsecondaries, int grndside, int orgdirecthit)
{
	float fBlastRadius=0;
	float fDamageValue;
	float fDistance;
	float fMultiplier;
	float fCheckDistance;
//	BOOL blew_something_up=FALSE;
	BOOL damaged_something=FALSE;
	MovingVehicleParams *checkvehicle;
	MultipleExplosionType secondaries;
	int num_secondaries;
	DWORD hit_value;
	float secondary_delay;
	int objectnear = 0;
	int isgun = 0;
	float dist_delay;
	int scaleup;
	int directhit = orgdirecthit;
	int facnum;
	int ifacTargetType = -1;
	void *pfacTarget = NULL;
	float dx, dz, offangle, tdist, fworkdist;
//	int grndside = 0;
//	int totalsecondaries = 0;

//   	PauseForDiskHit();  //remove this

	if (pweapon)
	{
		position = &pweapon->Pos;
		if ((int)pweapon->Type < 0x200)
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)pweapon->Type)];
		else
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex(pweapon->Type->TypeNumber)];
//		if(pweapon_type->iWeaponType != WEAPON_TYPE_GUN)
//		{
//			GDNoticeExplosion(pweapon);
//		}
		switch(pweapon_type->iWarHead)
		{
			case WARHEAD_STANDARD_MEDIUM:
			case WARHEAD_STANDARD_LARGE:
			case WARHEAD_STANDARD_HUGE:
			case WARHEAD_PENETRATION:
			case WARHEAD_CRATERING:
			case WARHEAD_NUKE:
			case WARHEAD_SAM_LARGE:
			case WARHEAD_AAA_LARGE:
				do_big_bang = TRUE;
				break;

			default:
				do_big_bang = FALSE;
				break;
		}
	}

	// This will now place an explosion and mark the goal blow up
	// flag for everything within the blast rectangle.

	fBlastRadius = (pweapon_type->iDamageRadius * FTTOWU);  // Feet

	if(pweapon_type)
	{
		if(pweapon_type->iWeaponType == WEAPON_TYPE_GUN)
		{
			isgun = 1;
		}
	}

	if(pweapon)
	{
		if((pweapon->P) && (lBombFlags & WSO_FAC_CHECK))
		{
			facnum = AICGetClosestFAC(pweapon->P);
			if(facnum == -2)
			{
				pfacTarget = pFACTarget;
				ifacTargetType = iFACTargetType;
			}
			else if(facnum != -1)
			{
				pfacTarget = Planes[facnum].AI.pGroundTarget;
				ifacTargetType = Planes[facnum].AI.lGroundTargetFlag;
			}
		}
	}

	if (fBlastRadius)
	{
		if (fBlastRadius < TILE_SIZE*2)
			fCheckDistance = TILE_SIZE*2;
		else
		{
			if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
				fCheckDistance = fBlastRadius*1.5f;
			else
				fCheckDistance = fBlastRadius;
		}

		fMultiplier = ((float)pweapon_type->iDamageValue) / (fBlastRadius * fBlastRadius);

		for(checkvehicle = MovingVehicles; checkvehicle <= LastMovingVehicle; checkvehicle ++)
		{
			if((checkvehicle->Status & (VL_ACTIVE)) && ((!(checkvehicle->Status & (VL_DESTROYED|VL_INVISIBLE))) || (pweapon_type->iSeekerType == 13)))
			{
				fDistance = (checkvehicle->WorldPosition - *position) - GetObjectRadius(checkvehicle->Type->Model);
				if (fDistance < fCheckDistance)
				{
					hit_value = 0;
					objectnear |= 1;

					scaleup = 0;
					if(checkvehicle->Type)
					{
						if(!(checkvehicle->Type->Flags & VT_COLLIDEABLE))
						{
							scaleup = 1;
						}
					}

					if((scaleup) && (isgun))
					{
						FPoint tempHigh = checkvehicle->DestroyedType->HighLR;
						FPoint tempLow = checkvehicle->DestroyedType->LowUL;
						tempHigh *= 3.0f;  //  Upped this a bit since small vehicles seemed extra hard to hit.
						tempLow *= 3.0f;	//  Remember that each bullet represents 16 so if he was close with one, he probably would have hit with one of the "missing"
						fDistance = (float)GetDistanceFromNormalBox(checkvehicle->Attitude,checkvehicle->WorldPosition,tempHigh,tempLow,*position,isgun);
					}
					else
					{
						fDistance = (float)GetDistanceFromNormalBox(checkvehicle->Attitude,checkvehicle->WorldPosition,checkvehicle->DestroyedType->HighLR,checkvehicle->DestroyedType->LowUL,*position,isgun);
					}

					if((pweapon && (pweapon->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_BOMB_ACCURACY))) || (netCall & GX_FAKE_ACCURACY))
					{
						fDistance *= 0.5;
						if (fDistance < GetObjectRadius(checkvehicle->DestroyedType->Model))
						{
							hit_value = DTE_DIRECT_HIT;
							directhit = 1;
						}
						else
							if (isgun)
								fDistance = 1000.0f;

					}
					else
					{
						if (fDistance < (GetObjectRadius(checkvehicle->DestroyedType->Model)*0.50f))
						{
							hit_value = DTE_DIRECT_HIT;
							directhit = 1;
						}
						else
							if (isgun)
								fDistance = 1000.0f;
					}


					if ((fDistance < fBlastRadius) || (isgun))
					{
						fDamageValue = fBlastRadius - fDistance;
						fDamageValue = fDamageValue * fDamageValue * fMultiplier;

						hit_value |= (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];

						if((!isgun) || (hit_value & DTE_DIRECT_HIT))
						{
							if (checkvehicle->iShipType)
								LogWeaponHit(pweapon,SHIP,(checkvehicle-MovingVehicles));
							else
								LogWeaponHit(pweapon,MOVINGVEHICLE,(checkvehicle-MovingVehicles));

							VDamageVehicleRadars(checkvehicle, position, fMultiplier, fBlastRadius);

							if (VDamageVehicle(checkvehicle, hit_value, GameLoop, fDamageValue, &num_secondaries, &secondary_delay, &secondaries))
							{
//								if((isgun) && (!soundplayed))
//								{
//									PlayPositionalExplosion(pweapon_type->iWarHead,Camera1,checkvehicle->WorldPosition);
//									soundplayed = 1;
//								}

								if (num_secondaries && (secondaries.WarheadID == -1))
								{
									PauseForDiskHit();
									if(lpDD)
									  lpDD->FlipToGDISurface();
									DebugBreak();
									UnPauseFromDiskHit();
								}

								if(ifacTargetType == MOVINGVEHICLE)
								{
									if(checkvehicle == (MovingVehicleParams *)pfacTarget)
									{
										if(pweapon->P == PlayerPlane)
										{
											iFACHitFlags |= FAC_HIT_DESTROYED;
										}
										else
										{
											AIC_FACAck(pweapon->P - Planes, 17);
										}
									}
								}

								LogDestroyedObject(pweapon,MOVINGVEHICLE,(checkvehicle-MovingVehicles));

								blew_something_up = TRUE;
								totalsecondaries += num_secondaries;

								dist_delay = fDistance * (float)(1.0 / MACH_1_GU_PER_50TH);

								if (num_secondaries)
								{

									if (!secondary_delay)
										ScheduleExplosion(checkvehicle->WorldPosition,secondaries, dist_delay,TRUE,NULL,NULL,checkvehicle);
									else
									{
										ScheduleExplosion(checkvehicle->WorldPosition,secondaries, dist_delay,FALSE,NULL,NULL,checkvehicle);
										ScheduleExplosion(checkvehicle->WorldPosition,secondaries, secondary_delay + dist_delay);
									}
								}
								else
									ScheduleExplosion(checkvehicle->WorldPosition,secondaries, dist_delay,FALSE,NULL,NULL,checkvehicle);

								//NewGenerator(PLANE_EXPLODES,checkvehicle->WorldPosition,0.0,2.0f,450); 					//This should be where the visual effects for the instance
								//New Smoke TEst
	//							AddSmoke(FX_FLYING_DEBRIS, (StructureInstance* )walker, 0, *position, 0);
								//NewCanisterExplosion( CT_EXPLOSION_FIRE, *position, frand(), 0.0f, NULL);

								if(PlayerPlane->AI.iSide == checkvehicle->iSide)
								{
									grndside = PlayerPlane->AI.iSide;
								}
							}
							else
							{
								if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
									VehicleStartHeatFire(checkvehicle,*position,fDistance,fBlastRadius,(pweapon) ? (void *)pweapon->P : NULL);

								if((pweapon) && (iFACState) && (isgun == 0) && (lBombFlags & WSO_FAC_CHECK) && (pweapon->P == PlayerPlane))
								{
									/* ----------------8/13/99 3:50PM------------------------------------------------------------------
									/* possible start a fire
									/* ------------------------------------------------------------------------------------------------*/
									if(PlayerPlane->AI.iSide == checkvehicle->iSide)
									{
										grndside = PlayerPlane->AI.iSide;
										iFACHitFlags |= FAC_HIT_FRIEND;
									}
									else if(ifacTargetType == MOVINGVEHICLE)
									{
										if(checkvehicle == (MovingVehicleParams *)pfacTarget)
										{
											iFACHitFlags |= FAC_HIT_DAMAGE;
										}
									}
								}
							}
						}
						else if(isgun)
						{
							if(fDistance < (LANE_WIDTH * FTTOWU * 4))
							{
								objectnear |= 2;
							}
						}
					}
					else
					{
						if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
							VehicleStartHeatFire(checkvehicle,*position,fDistance,fBlastRadius,(pweapon) ? (void *)pweapon->P : NULL);

						if((ifacTargetType == MOVINGVEHICLE) && (pweapon->P == PlayerPlane))
						{
							if(checkvehicle == (MovingVehicleParams *)pfacTarget)
							{
								dx = position->X - checkvehicle->WorldPosition.X;
								dz = position->Z - checkvehicle->WorldPosition.Z;

								offangle = atan2(-dx, -dz) * 57.2958;
								offangle -= AIConvertAngleTo180Degree(pweapon->Heading);

								offangle = AICapAngle(offangle);

								tdist = QuickDistance(dx,dz) * WUTOFT;

								if(fabs(offangle) < 45.0f)
								{
									fworkdist = fabs(cos(DegToRad(offangle)) * tdist);
									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_LONG;
									}
								}
								else if(fabs(offangle) > 135.0f)
								{
									fworkdist = fabs(cos(DegToRad(offangle)) * tdist);
									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_SHORT;
									}
								}
								else if(offangle < 0)
								{
									fworkdist = fabs(sin(DegToRad(offangle)) * tdist);
									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_RIGHT;
									}
								}
								else
								{
									fworkdist = fabs(sin(DegToRad(offangle)) * tdist);
									if((fworkdist < dFACRange) || (!(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))))
									{
										dFACRange = fworkdist;
										iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT);
										iFACHitFlags |= FAC_HIT_LEFT;
									}
								}
							}
						}
					}
				}
			}
		}

		if (pweapon)
			AICBombResultSpch(pweapon,blew_something_up, totalsecondaries, grndside, directhit);
		//else
		// Speach for secondaries?

		//NewGenerator(PLANE_EXPLODES,*position,0.0,2.0f,50);	//This should be the explosion graphic for the weapon type
		//AddSmoke(FX_DAMAGE_SMOKE, NULL, 0, *position, 0);
		//NewCanisterExplosion( CT_EXPLOSION_FIRE, *position, frand(), 0.0f, NULL);

//		for( int x=0; x<8 ; x++)
//			AddSmoke(FX_FLYING_DEBRIS, (StructureInstance* )walker, 0, *position, 0);


	}

	if((pweapon) && (isgun))
	{
		if(objectnear & 2)
		{
			pweapon->lTimer = 20;
		}
		else if(objectnear & 1)
		{
			pweapon->lTimer = 100;
		}
		else
		{
			pweapon->lTimer = 500;
		}
	}
}

ShockWave *MakeShockWave(const FPointDouble &location,const DBWeaponType *weapon,BOOL second_water)
{
	ShockWave *walker;
	float worker;
	float work_radius;
	float work_damage;
	int minimum_damage = 150;
	BOOL in_water;

//	Make sure the weapon is valid and that we're somewhere near the camera
	if (!weapon || !InHighResTerrainRegion((FPointDouble &)location))
		return NULL;

	if (!second_water)
	{
		if (in_water = InWater(location.X,location.Z))
		{
			second_water = TRUE;
			minimum_damage = 30;
			AddCrater(location,weapon->iDamageRadius,TRUE);
		}

		if (weapon->iDamageValue < minimum_damage)
		{
			if (!in_water && (weapon->iWarHead == WARHEAD_CRATERING))
				AddCrater(location,weapon->iDamageRadius);
			return NULL;
		}
	}

	walker = &ShockWaves[0];

	while((walker <= LastShockWave) && walker->Flags)
		walker++;

	if (walker >= &ShockWaves[MAX_SHOCK_WAVES])
		return NULL;

	if (walker > LastShockWave)
		LastShockWave = walker;

	ANGLE heading = (ANGLE)(frand() * (float)0x10000);

	//	Get the lay of the land (Tee-Hee!)
	GetMatrix(location,heading,walker->Matrix);
	LandNormal(location,walker->Normal);
	work_radius = (float)weapon->iDamageRadius FEET;
	work_damage = (float)weapon->iDamageValue;

	if (work_damage < 100.0f)
	{
		work_damage *= 1.8f;
		work_radius *= 1.8f;
	}
	else
		if (work_damage < 500.0f)
		{
			work_damage *= 1.2f;
			work_radius *= 1.2f;
		}

	walker->Flags = SW_INUSE;

	if (second_water)
	{
		walker->MaxRadius = work_radius * 3.0f;
		walker->ColorSpread = 0.5*(float)(work_damage)*1.0/850.0f;
	}
	else
	{
		if (in_water)
		{
			walker->Flags |= SW_IN_WATER;
			walker->ColorSpread = 0.9*(float)(work_damage)*1.0/850.0f;
			MakeShockWave(location,weapon,TRUE);
		}
		else
		{
			walker->ColorSpread = 0.8*(float)(work_damage)*1.0/850.0f;
//			if (weapon->iDamageValue > 700)
				AddCrater(location,weapon->iDamageRadius);
//			else
//				AddCrater(location,CRATER_MEDIUM);
		}

		walker->MaxRadius = work_radius * 1.0f;
	}

	walker->Position = location;
	walker->Position.Y += 1.0 FEET;

	walker->Radius = work_radius * 0.2;
	walker->FirstArea = (PI*walker->Radius*walker->Radius);

	worker = walker->MaxRadius * walker->MaxRadius;
	walker->OneOverMaxArea = 1.0/(PI*worker - walker->FirstArea);
	walker->LastLoop = GameLoop;

//	This may be used later to cause damage to fragile, far away things.
	walker->Objects = NULL;

	return walker;

}

void DeleteShockWave(ShockWave *wave)
{
	wave->Flags = 0;

	if (wave->Objects)
		free(wave->Objects);

//	UnsortObject(wave);

	wave->Objects = NULL;

	if (wave == LastShockWave)
		while((LastShockWave > &ShockWaves[-1]) && (!LastShockWave->Flags))
			LastShockWave--;
}


int GetNumberOfSpines(float distance)
{
	int num_sides;

	distance -= LP_LOW_MAX_DISTANCE*0.25;
	if (distance <= 1.0)
		num_sides = 6;
	else
	{
		distance = LP_LOW_MAX_DISTANCE*0.75 - distance;
		_asm
		{
			fld		ShockWaveOneOverLog2Base
			fld		distance
			fyl2x
			fistp	num_sides
		}

		if (num_sides > 6)
			num_sides = 6;
	}

	return (num_sides<<1)+4;
}

void FixShockWaveColor(RotPoint3D &point)
{
	LightSource **light_walker;
	FPoint specular(0.0f);

	light_walker = GlobalViewpointLights;
	point.Diffuse.SetValues(0.0f);
 	point.Specular.SetValues(0.0f);
	while(light_walker <= LastViewpointLight)
	{
		if (*light_walker)
			(*light_walker)->CalculateSaturation(point.Rotated,point.Diffuse,specular);
		light_walker++;
	}

	point.Diffuse *= 0.9f;
	point.Diffuse += specular;
	point.Diffuse.Limit(1.0f);
}


void RenderShockWave(ShockWave *wave,FPoint &relative_position)
{
	FPoint *spine_walker;
	FPoint rotated_spine;
	FPoint *spines;
	RotPoint3D center_point;
	RotPoint3D array_N[3];
	RotPoint3D array_1[3];
	RotPoint3D array_2[3];
	RotPoint3D *left_array;
	RotPoint3D *right_array;
	RotPoint3D *points[3];
	FMatrix matrix;
	float scale_1,scale_2;
	float alpha;
	int clip_flags,draw_flags;
	int num_sides;
	FPoint normal;


	if (relative_position.QuickLength() > LP_LOW_MAX_DISTANCE)
	//if (wave->SortHeader->Distance > LP_LOW_MAX_DISTANCE)
	{
		DeleteShockWave(wave);
		return;
	}

	scale_1 = wave->Radius * 0.8f;
	scale_2 = wave->Radius;

	alpha = wave->ColorSpread*(1-((PI*scale_2*scale_2 - wave->FirstArea) * wave->OneOverMaxArea));

//	if (wave->Flags & SW_IN_WATER)
//		alpha = 31.0f - 15.0f*;
//	else
//		alpha = 23.0f - 7.0f*((PI*scale_2*scale_2 - wave->FirstArea) * wave->OneOverMaxArea);

	if (alpha < 0.0f)
	{
		DeleteShockWave(wave);
		return;
	}

	if (alpha > 1.0)
		alpha = 1.0;

	num_sides = GetNumberOfSpines(relative_position.QuickLength());

	switch(num_sides)
	{
		case 4 :
			spines = &ShockVectors4[0];
			break;

		case 6 :
			spines = &ShockVectors6[0];
			break;

		case 8 :
			spines = &ShockVectors8[0];
			break;

		case 10 :
			spines = &ShockVectors10[0];
			break;

		case 12 :
			spines = &ShockVectors12[0];
			break;

		case 14 :
			spines = &ShockVectors14[0];
			break;

		case 16 :
			spines = &ShockVectors16[0];
			break;
	}

	//draw sphere concussion
	int gop = PRIM_NO_Z_WRITE|PRIM_ALPHA_ADD;//| PRIM_ALPHAIMM;////PRIM_TEXTURE |

//	BuildSphere(  num_sides*0.5 + 2 );
//	if( RadiusInView( relative_position, wave->Radius) )
//		DrawSpherePreBuilt( wave->Radius, 1.0f, relative_position, alpha, alpha, gop );




//	First do the center point
	center_point.Rotated.RotateInto(relative_position,ViewMatrix);

	if (center_point.Rotated.Z > ZClipPlane)
		center_point.Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		center_point.Flags = PT3_ROTATED;
		PerspectPoint(&center_point);
	}

	FixShockWaveColor(center_point);
	center_point.Alpha = 0.0f;

//	Now the spines
	matrix = wave->Matrix;
	matrix *= ViewMatrix;

	/* -----------------5/14/99 1:54PM---------------------------------------------------------------------
	 * now get the view space "up" vector
	 * ----------------------------------------------------------------------------------------------------*/
	normal.SetValues(0.0f,1.0f,0.0f);
	LastSurfaceNormal = &normal;
	LastObjToViewMatrix = &matrix;

	//We'll precompute the first spine

	rotated_spine.RotateInto(*spines++,matrix);

	array_N[1].Rotated.SetValues(scale_2,rotated_spine);
	array_N[1].Rotated += center_point.Rotated;
	array_N[1].Alpha = 0.0f;
	if (array_N[1].Rotated.Z > ZClipPlane)
		array_N[1].Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		array_N[1].Flags = PT3_ROTATED;
		PerspectPoint(&array_N[1]);
	}

	FixShockWaveColor(array_N[1]);

	array_N[0].Rotated.SetValues(scale_1,rotated_spine);
	array_N[0].Rotated += center_point.Rotated;
	array_N[0].Alpha = alpha;
	if (array_N[0].Rotated.Z > ZClipPlane)
		array_N[0].Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		array_N[0].Flags = PT3_ROTATED;
		PerspectPoint(&array_N[0]);
	}

	FixShockWaveColor(array_N[0]);

	//Now we'll start the loop which will do the rest of them

//	SelectFadingTable(WHITE_HAZE_SHADING_TABLE);

	spine_walker = spines;
	left_array = &array_N[0];

	while(num_sides--)
	{
		right_array = left_array;
		if (num_sides)
		{
			if (num_sides & 1)
				left_array = &array_1[0];
			else
				left_array = &array_2[0];

			rotated_spine.RotateInto(*spine_walker++,matrix);

			left_array[1].Rotated.SetValues(scale_2,rotated_spine);
			left_array[1].Rotated += center_point.Rotated;
			left_array[1].Alpha = 0.0f;
			if (left_array[1].Rotated.Z > ZClipPlane)
				left_array[1].Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				left_array[1].Flags = PT3_ROTATED;
				PerspectPoint(&left_array[1]);
			}

			FixShockWaveColor(left_array[1]);


			left_array[0].Rotated.SetValues(scale_1,rotated_spine);
			left_array[0].Rotated += center_point.Rotated;
			left_array[0].Alpha = alpha;
			if (left_array[0].Rotated.Z > ZClipPlane)
				left_array[0].Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				left_array[0].Flags = PT3_ROTATED;
				PerspectPoint(&left_array[0]);
			}

			FixShockWaveColor(left_array[0]);

		}
		else
			left_array = &array_N[0];

		//okay, we've rotated and perspected everything, now we just have to build the polys!

		//we'll do the center first, then the outer two
		points[0] = &center_point;
		points[1] = &left_array[0];
		points[2] = &right_array[0];

		draw_flags = points[0]->Flags & points[1]->Flags & points[2]->Flags;
		if (!(draw_flags & PT3_NODRAW))
		{
			clip_flags = points[0]->Flags | points[1]->Flags | points[2]->Flags;
			DrawPolyClipped(PRIM_ALPHAIMM | PRIM_ZBIAS,CL_CLIP_NONE,3,&points[0],clip_flags & PT3_CLIP_FLAGS);
		}

		points[0] = &left_array[0];
		points[1] = &right_array[1];
		points[2] = &right_array[0];

		draw_flags = points[0]->Flags & points[1]->Flags & points[2]->Flags;
		if (!(draw_flags & PT3_NODRAW))
		{
			clip_flags = points[0]->Flags | points[1]->Flags | points[2]->Flags;
			DrawPolyClipped(PRIM_ALPHAIMM | PRIM_ZBIAS,CL_CLIP_NONE,3,&points[0],clip_flags & PT3_CLIP_FLAGS);
		}

		points[0] = &left_array[1];
		points[1] = &right_array[1];
		points[2] = &left_array[0];

		draw_flags = points[0]->Flags & points[1]->Flags & points[2]->Flags;
		if (!(draw_flags & PT3_NODRAW))
		{
			clip_flags = points[0]->Flags | points[1]->Flags | points[2]->Flags;
			DrawPolyClipped(PRIM_ALPHAIMM | PRIM_ZBIAS,CL_CLIP_NONE,3,&points[0],clip_flags & PT3_CLIP_FLAGS);
		}
	}

}

void DisplayShockWaves(CameraInstance *camera)
{
	ShockWave *walker;
	DWORD loops;
	FPoint relative_position;

//   	PauseForDiskHit();  //remove this

	walker = &ShockWaves[0];

	while(walker <= LastShockWave)
	{
		if (walker->Flags & SW_INUSE)
		{
			//Let's calculate where this walker is now
			loops = GameLoop - walker->LastLoop;

			if (loops)
			{
				//How far did the radius move at Mach 1.0
				if (walker->Flags & SW_IN_WATER)
					walker->Radius += ((float)loops) * (1087.1 FEET * 1/200.0);
				else
					walker->Radius += ((float)loops) * MACH_1_GU_PER_50TH;

				if (walker->Radius > walker->MaxRadius)
				{
					//Otherwise trash it!
					DeleteShockWave(walker);
					walker++;
					continue;
				}

			}

			if (CloseEnoughToDraw(walker->Position))
			{
				walker->LastLoop = GameLoop;

				relative_position.MakeVectorToFrom(walker->Position,camera->CameraLocation);

				// do test for backface culling
				if ((relative_position * walker->Normal) <= 0)
					RenderShockWave(walker,relative_position);
			}
		}
		walker++;
	}

//	UnPauseFromDiskHit(); //remove this
}

void Explode(ScheduledExplosion *sewalker)
{
	FPointDouble point;
	double theta;
	double dist;
	BOOL skip_explosion = FALSE;
	FPoint vel(0.0f);
	FPointDouble tmp;

	while(sewalker->ExplosionsLeft && !sewalker->TicksLeft)
	{
		sewalker->ExplosionsLeft--;

		if (sewalker->Instance)
		{

			ReRegisterInstance(sewalker->Instance);

			if (((StructureInstance *)(sewalker->Instance))->Registration)
			{
				tmp = sewalker->Instance->Position;
				tmp.Y += ((StructureInstance *)(sewalker->Instance))->Registration->ExtentHeight * 0.5f;
//					 		NewCanister( CT_FLYING_GROUND_PARTS, tmp, vel, 0.0f );
			}

			sewalker->Instance = NULL;
		}

		if (sewalker->Vehicle)
			DestroyVehicle((MovingVehicleParams *)sewalker->Vehicle,TRUE);

		if (sewalker->FireType != CT_NONE)
		{
			tmp = sewalker->Center;
			tmp.Y += 3 METERS;
			NewCanister((CanisterType)sewalker->FireType,tmp,vel,sewalker->FireSize,sewalker->FireDuration);
			sewalker->FireType = CT_NONE;
		}

		if ((sewalker->Flags & SE_CULL_LIGHTS) && (rand() & 3))
			NoFlashDuringExplosion = TRUE;

		if (sewalker->DamageSize)
		{
			NewExplosionForcedWeapon(sewalker->Center,sewalker->DamageRadius,sewalker->DamageSize);
			if (sewalker->WarheadForSound)
				PlayPositionalExplosion(sewalker->WarheadForSound,Camera1,sewalker->Center);
			sewalker->DamageSize = 0;
			skip_explosion = TRUE;
		}

		if (sewalker->Flags & SE_USE_EXP)
		{
			theta = (double)frand() * (double)2.0*PI;
			dist = (double)frand() * (double)sewalker->Radius;

			point.SetValues(-sin(theta)*dist,0.0,-cos(theta)*dist);
			point += sewalker->Center;

			if (!(sewalker->Flags & SE_NO_DAMAGE))
				DoExplosionOnGround(NULL,sewalker->Flags & SE_BIG_BANG,&point,sewalker->WarheadType,skip_explosion);
			else
			{
				NewExplosionWeapon( sewalker->WarheadType, point );
				PlayPositionalExplosion(sewalker->WarheadType->iWarHead,Camera1,point);
			}

			skip_explosion = FALSE;

			if (sewalker->TimeBetweenExplosions | sewalker->Deviation)
			{
				sewalker->TicksLeft = (rand() % 2*sewalker->Deviation)-sewalker->Deviation + sewalker->TimeBetweenExplosions;
				if (sewalker->TicksLeft < 0)
					sewalker->TicksLeft = 0;
			}
		}

		NoFlashDuringExplosion = FALSE;

	}
}


ScheduledExplosion *ScheduleExplosion(FPointDouble &location,MultipleExplosionType &exp, float delay_start,BOOL use_exp,BasicInstance *instance,DamagePasser *pDamage,void *vehicle)
{
	ScheduledExplosion *sewalker = NULL;
	ScheduledExplosion local_explosion;


	if ((use_exp && exp.NumberOfExplosions) || instance || vehicle || pDamage)
	{
		if ((location.X < MediumHeightCache->BLoadedULX) ||
			(location.X > MediumHeightCache->BLoadedLRX) ||
			(location.Z < MediumHeightCache->BLoadedULY) ||
			(location.Z > MediumHeightCache->BLoadedLRY))
		{
			return NULL;
		}

		sewalker = &ScheduledExplosions[0];

		while((sewalker <= LastScheduledExplosion) && sewalker->Flags)
			sewalker++;

		if (sewalker >= &ScheduledExplosions[MAX_SCHEDULED_EXPLOSIONS])
			sewalker = &local_explosion;
		else
			if (sewalker > LastScheduledExplosion)
				LastScheduledExplosion = sewalker;

		sewalker->Instance = instance;

		sewalker->Vehicle = vehicle;

		if (pDamage)
		{
			sewalker->DamageSize = pDamage->ExplosionSize;
			sewalker->DamageRadius = pDamage->ExplosionRadius;
			if (g_Settings.gr.nGroundSmoke == 1)
				sewalker->FireType = CT_NONE;
			else
				sewalker->FireType = pDamage->FireType;
			sewalker->FireSize = pDamage->FireSize;
			sewalker->FireDuration = pDamage->FireDuration;

			if ((delay_start > 8) && (sewalker->DamageSize))
			{
				if (sewalker->DamageSize >= 700)
					sewalker->WarheadForSound = WARHEAD_STANDARD_HUGE;
				else
					if ( sewalker->DamageSize > 399)
						sewalker->WarheadForSound = WARHEAD_STANDARD_LARGE;
					else
						if (sewalker->DamageSize >= 199)
							sewalker->WarheadForSound = WARHEAD_STANDARD_MEDIUM;
						else
							sewalker->WarheadForSound = WARHEAD_STANDARD_SMALL;
			}
			else
				sewalker->WarheadForSound = 0;
		}
		else
		{
			sewalker->DamageSize = 0;
			sewalker->FireType = CT_NONE;
			sewalker->WarheadForSound = 0;
		}

		sewalker->TicksLeft = (int)delay_start;
		if (sewalker->TicksLeft < 0)
			sewalker->TicksLeft = 0;
		sewalker->Center = location;

		if (use_exp)
		{
			int weap_num = AIGetWeaponIndex(exp.WarheadID);

			if (weap_num >= 0)
				sewalker->WarheadType = &pDBWeaponList[weap_num];
			else
			{
				PauseForDiskHit();
				if(lpDD)
				  lpDD->FlipToGDISurface();
				DebugBreak();
				UnPauseFromDiskHit();
				sewalker->WarheadType = &pDBWeaponList[0];
			}

			memcpy(&sewalker->ExplosionsLeft,&exp.NumberOfExplosions,sizeof(MultipleExplosionType)-sizeof(long));
			sewalker->Flags = SE_INUSE | SE_USE_EXP | (sewalker->Flags & (SE_NO_DAMAGE | SE_CULL_LIGHTS));
			sewalker->WarheadForSound = sewalker->WarheadType->iWarHead;
			switch(sewalker->WarheadType->iWarHead)
			{
				case WARHEAD_STANDARD_MEDIUM:
				case WARHEAD_STANDARD_LARGE:
				case WARHEAD_STANDARD_HUGE:
				case WARHEAD_PENETRATION:
				case WARHEAD_CRATERING:
				case WARHEAD_NUKE:
				case WARHEAD_SAM_LARGE:
				case WARHEAD_AAA_LARGE:
					sewalker->Flags |= SE_BIG_BANG;
					break;
			}
		}
		else
		{
			sewalker->WarheadType = NULL;
			sewalker->Flags = SE_INUSE;
			sewalker->ExplosionsLeft = 1;
			sewalker->WarheadForSound = 0;
		}


		if(netCall)
		{
			sewalker->Flags |= SE_NO_DAMAGE;
		}

	}

	if (sewalker == &local_explosion)
	{
		Explode(sewalker);
		sewalker = NULL;
	}

	return sewalker;

}

void DeleteScheduledExplosion(ScheduledExplosion *exp)
{

	exp->Flags = 0;
	exp->WarheadType = NULL;

	if (exp == LastScheduledExplosion)
		while((LastScheduledExplosion > &ScheduledExplosions[-1]) && (!LastScheduledExplosion->Flags))
			LastScheduledExplosion--;
}

void UpdateScheduledExplosions()
{
	ScheduledExplosion *sewalker;

	sewalker = &ScheduledExplosions[0];

	while(sewalker <= LastScheduledExplosion)
	{
		if (sewalker->Flags & SE_INUSE)
		{
			if (!(sewalker->TicksLeft--))
			{
				sewalker->TicksLeft = 0;

				Explode(sewalker);

				if (!sewalker->ExplosionsLeft)
				{
					DeleteScheduledExplosion(sewalker);
					sewalker++;
					continue;
				}

			}
		}
		sewalker++;
	}
}

void AddCrater(const FPointDouble &location,const int size,BOOL splash)
{
	Crater *cwalker,*oldest;
	DWORD diff,smallest_diff;
	float new_size=(float)size FEET;

	cwalker= &Craters[0];

	oldest = NULL;
	smallest_diff = 0xFFFFFFFF;

	if(InHighResTerrainRegion((FPointDouble)location))
	{
		while(cwalker < &Craters[MAX_CRATERS])
		{
			if (cwalker->D3DTexture)
			{
				if (new_size > cwalker->Size)
				{
					if ((fabs(location.X - cwalker->Location.X) < new_size) || (fabs(location.Z - cwalker->Location.Z) < new_size))
						return;
				}
				else
					if ((fabs(location.X - cwalker->Location.X) < cwalker->Size) || (fabs(location.Z - cwalker->Location.Z) < cwalker->Size))
						return;

				diff = GameLoop - cwalker->BirthTick;
				if (diff < smallest_diff)
				{
					smallest_diff = diff;
					oldest = cwalker;
				}
			}
			else
			{
				oldest = cwalker;
				break;
			}

			cwalker++;
		}

		if (oldest)
		{
			int initial = 0;

			oldest->BirthTick = GameLoop;
			oldest->Location = location;

			if (!splash)
			{
				oldest->D3DTexture = CraterModels[rand()&7];
				oldest->AltD3DTexture = NULL;
			}
			else
			{
				oldest->D3DTexture = CraterModels[8];
				oldest->AltD3DTexture = CraterModels[9];
			}

			oldest->MapFlags = SME_NO_SUN_FADE | SME_DOWN_VECTORS;

			oldest->CraterFlags = CRATER_CALC_HEIGHT;
			oldest->Size = new_size;
		}
	}
}

void DisplayCraters(CameraInstance *camera)
{
	Crater *cwalker;
	FPoint rel;
	BOOL changed_z = FALSE;
	FPoint up;
	FPoint right;
	FPoint fwd;
	float theta;
	float total_time;
	float total_fade;
	float s1_fade;
	float s2_fade;

	cwalker = &Craters[0];

	if (!(ImagingMethod & IT_FLIR))
		SetFLIRIntensity(0.0f);

	while(cwalker < &Craters[MAX_CRATERS])
	{
		if (cwalker->D3DTexture)
		{
			if(InHighResTerrainRegion(cwalker->Location))
			{
				rel.MakeVectorToFrom(cwalker->Location,camera->CameraLocation);

				if (CloseEnoughToDraw(cwalker->Location))
				{
					if (cwalker->CraterFlags & CRATER_CALC_HEIGHT)
					{
						cwalker->Location.Y = LandHeight(cwalker->Location.X,cwalker->Location.Z);

						theta = PI*2.0f*frand();

						cwalker->VectorV.SetValues(cos(theta),0.0,-sin(theta));
						LandNormal(cwalker->Location,up);

						cwalker->VectorV %= up;
						cwalker->VectorV.Normalize();

						cwalker->VectorU = up;
						cwalker->VectorU %= cwalker->VectorV;
						cwalker->VectorU.Normalize();

						/* -----------------8/2/99 4:16PM----------------------------------------------------------------------
						/* now we need to calculate the right scaling
						/* ----------------------------------------------------------------------------------------------------*/


						cwalker->ScaleRight = 1.0f/cwalker->Size;
						cwalker->ScaleRight *= 1.0f - 2.0f/64.0f;
						cwalker->OffsetU = 0.5f;

						cwalker->ScaleDown = cwalker->ScaleRight;
						cwalker->OffsetV = cwalker->OffsetU;

						cwalker->CraterFlags &= ~CRATER_CALC_HEIGHT;
					}

//					if ((ImagingMethod & IT_FLIR))
//						SetFLIRIntensity(1.0f - (float)(GameLoop - cwalker->BirthTick) * (1.0f/30000.0f));

					if (RadiusInView(rel,cwalker->Size))
					{
						cwalker->RelVectorU = cwalker->VectorU;
						cwalker->RelVectorV = cwalker->VectorV;
						if (cwalker->AltD3DTexture)
						{
							total_time = (float)(GameLoop-cwalker->BirthTick)/(cwalker->Size * (1.0f/(0.8f FEET))); //rough estimation of time
							total_fade = (1.0f - total_time)*0.3;
							if (total_fade <= 0.0)
							{
								cwalker->D3DTexture = NULL;
								cwalker->AltD3DTexture = NULL;
								cwalker++;
								continue;
							}
							else
								if (total_fade > 0.3f)
									total_fade = 0.3f;

							if (total_time > 0.05)
							{
								if (total_time < 0.1)
									s1_fade = PI*0.5;
								else
									if (total_time < 0.6)
									{
										s1_fade = (total_time-0.1f)+0.5f;
										s1_fade *= PI;
									}
									else
										s1_fade = 0.0f;
							}
							else
								s1_fade = (total_time) * (PI*0.5f)/0.05f;

							if (s1_fade > 0.0f)
							{
								s1_fade = sin(s1_fade)*total_fade;

								if (s1_fade > 0.0)
								{
									if (s1_fade > 1.0f)
										s1_fade = 1.0f;
									InsertShadow(0,(DWORD)&cwalker->D3DTexture,cwalker->Location,camera->CameraLocation,s1_fade,&cwalker->Size);
								}
							}

							if (total_time < 0.1)
								s2_fade = 0.0f;
							else
								if (total_time > 0.6)
									s2_fade = 1.0f;
								else
									s2_fade = (total_time - 0.1f)*2.0f;

							if (s2_fade)
								InsertShadow(SI_USE_ALT_TEXTURE,(DWORD)&cwalker->D3DTexture,cwalker->Location,camera->CameraLocation,s2_fade*total_fade,&cwalker->Size);

						}
						else
							InsertShadow(0,(DWORD)&cwalker->D3DTexture,cwalker->Location,camera->CameraLocation,1.0f,&cwalker->Size);
						cwalker->RelVectorU *= ViewMatrix;
						cwalker->RelVectorV *= ViewMatrix;
					}
				}
			}
			else
				cwalker->D3DTexture = NULL;
		}

		cwalker++;
	}

}

/*----------------------------------------------------------------------------
 *
 *	GDGetUnRealHitValue()
 *
 */
DWORD GDGetUnRealHitValue(DWORD org_hit_value, BasicInstance *walker)
{
	int target_type;
	DamageProportions *ratio;
	int cnt, sbestcnt, tbestcnt;
	int temp_type;
	float sbestval = 0.0f;
	float tbestval = 0.0f;
	DWORD return_value;

	switch(walker->Family)
	{
		case FAMILY_STRUCTURE :
			StructureFamily *sfamily;
			StructureInstance *sinstance;

			sinstance = (StructureInstance *)walker;
			sfamily = sinstance->Type;

			target_type = sfamily->Physicals.Flags & DTE_TARGET_TYPE;
			break;

		/* ------------------------------------------1/3/98 7:31PM---------------------------------------------
		* this is handled as a special case, the runway function is called directly
		* ----------------------------------------------------------------------------------------------------*/

		case FAMILY_RUNWAYPIECE :
			return(org_hit_value);
			break;

		case FAMILY_INFOPROVIDER :
			InfoProviderFamily *ifamily;
			InfoProviderInstance *iinstance;

			iinstance = (InfoProviderInstance *)walker;
			ifamily = iinstance->Type;

			target_type = ifamily->Physicals.Flags & DTE_TARGET_TYPE;
			break;

		case FAMILY_AAWEAPON :
			AAWeaponFamily *wfamily;
			AAWeaponInstance *winstance;

			winstance = (AAWeaponInstance *)walker;
			wfamily = winstance->Type;

			target_type = wfamily->Physicals.Flags & DTE_TARGET_TYPE;
			break;
		default:
			return(org_hit_value);
			break;
	}

	if ((walker->Flags & BI_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
		target_type = DTE_SOFT_TARGET;

	for(cnt = 0, sbestcnt = -1, tbestcnt = -1; cnt < 5; cnt ++)
	{
		temp_type = target_type | ((cnt & DTE_TARGET_TYPE) << 2);

		ratio = &DamageMultipliers[temp_type << 1];
		if (org_hit_value & DTE_DIRECT_HIT)
			ratio++;

		if(ratio->SustainablePercentage > sbestval)
		{
			sbestcnt = cnt;
			sbestval = ratio->SustainablePercentage;
		}
		if(ratio->TotalPercentage > tbestval)
		{
			tbestcnt = cnt;
			tbestval = ratio->TotalPercentage;
		}
	}

	if((tbestcnt >= 0) && (tbestval))
	{
		return_value = org_hit_value & ~(DTE_TARGET_TYPE);
		return_value |= tbestcnt;
		return(return_value);
	}
	else if((sbestcnt >= 0) && (sbestval))
	{
		return_value = org_hit_value & ~(DTE_TARGET_TYPE);
		return_value |= sbestcnt;
		return(return_value);
	}
	return(org_hit_value);
}

extern float heightscale;

void WeaponHitsWall( FPointDouble &pos, FPointDouble &vel, float radius, float fuel, FPoint *vector,BOOL dust)
{
	int numspines = 6;
	FPointDouble position = pos;
	FMatrix lmatrix;
	FMatrix matrix;
	FPoint lnormal;
	FPoint pnormal;
	FPoint tmp(0.0f);
	double fdot;
	double flength;
	CanisterType can_type;

	if (InHighResTerrainRegion(position))
	{
		numspines = 6*fuel + g_Settings.gr.nExplosionDetail*3;

		lnormal = *vector;

		position.AddScaledVector(1.0 METERS,*vector);

		flength = vel.Length();
		if (flength == 0.0)
			flength = 1.0;
		else
			flength = 1.0/flength;

		heightscale = 1.02f - ( float )((fdot = fabs(vel.X*(double)lnormal.X + vel.Y*(double)lnormal.Y + vel.Z*(double)lnormal.Z))*flength);
		heightscale *= 20.0f;

		if (heightscale > (20.0f * 0.10))
		{
			lnormal*=fdot*2.0f;

			lnormal += vel;

			lnormal.Normalize();

			matrix.m_Data.RC.R0C1 = lnormal.X;
			matrix.m_Data.RC.R1C1 = lnormal.Y;
			matrix.m_Data.RC.R2C1 = lnormal.Z;

			flength = -flength;
			pnormal.SetValues((float)(vel.X * flength),(float)(vel.Y * flength),(float)(vel.Z * flength));
			pnormal %= lnormal;
			pnormal.Normalize();

			matrix.m_Data.RC.R0C0 = pnormal.X;
			matrix.m_Data.RC.R1C0 = pnormal.Y;
			matrix.m_Data.RC.R2C0 = pnormal.Z;

			lnormal %= pnormal;
			lnormal.Normalize();

			matrix.m_Data.RC.R0C2 = lnormal.X;
			matrix.m_Data.RC.R1C2 = lnormal.Y;
			matrix.m_Data.RC.R2C2 = lnormal.Z;
		}
		else
			if (vector)
			{

				FPoint z,x;

				matrix.m_Data.RC.R0C1 = vector->X;
				matrix.m_Data.RC.R1C1 = vector->Y;
				matrix.m_Data.RC.R2C1 = vector->Z;

				if (fabs(vector->Y) < 0.98f)
					z.SetValues(0.0f,1.0f,0.0f);
				else
					z.SetValues(0.0f,0.0f,1.0f);

				x = *vector;
				x %= z;
				x.Normalize();

				matrix.m_Data.RC.R0C0 = x.X;
				matrix.m_Data.RC.R1C0 = x.Y;
				matrix.m_Data.RC.R2C0 = x.Z;

				z = x;
				z %= *vector;

				matrix.m_Data.RC.R0C2 = z.X;
				matrix.m_Data.RC.R1C2 = z.Y;
				matrix.m_Data.RC.R2C2 = z.Z;
			}

		if (dust)
			can_type = CT_EXPLOSION_DIRT;
		else
			can_type = CT_EXPLOSION_FIRE;

		for( int i = 0; i< numspines; i++)
		{
			if (i && (frand() > fuel))
				NewCanister( CT_EXPLOSION_DIRT , position, tmp, radius, 0.0f, &matrix);
			else
				NewCanister( can_type , position, tmp, radius, 0.0f, &matrix);
		}

		if (!dust)
			NewCanister(CT_FLASH,position,tmp,1.0f);

		//tmp.SetValues( 0.0f);
		//NewCanister( CT_EXPLOSION_DOME,position, tmp, 20 METERS );

	}

}