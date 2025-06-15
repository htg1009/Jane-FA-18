/*****************************************************************************
*
*	weapons.cpp  --  F18 weapon code
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
#include "avlocal.h"
#include "seatstf.h"

extern BombProgType BombProgs[MAX_BOMB_PROGS];
extern BombProgType ClusterBombProgs[MAX_BOMB_PROGS];
extern HarpoonProgType HarpoonProgs[MAX_BOMB_PROGS];
extern SlamerProgType SlamerProgs[MAX_BOMB_PROGS];
extern JsowProgType JsowProgs[MAX_BOMB_PROGS];
extern JsowProgType JdamProgs[MAX_BOMB_PROGS];
extern HarmProgType HarmProgs[MAX_BOMB_PROGS];
extern TewsProgType TewsProgs[MAX_TEWS_PROGS];
extern void GetAcquiredTarget(void **Target, int *TargetType, FPointDouble *Loc);

#define W_DROP_SPEED	35.0f
int WGetStrongestShipRadar(MovingVehicleParams *vehiclepnt);

extern int	MySlot;		// my player's slot index [0..N]
extern BOOL	g_bIAmHost;
extern void StopEngineSounds();
// extern int SimJettisonOn;
extern CPitType Cpit;
extern FPointDouble LaserLoc;
extern BYTE WarHeadTypeConversion[];
extern int BombStations[];
extern int g_nMissionType;
//  #define CHECKMISSFLIGHT 1
//  #define WATCHFALL 1

// extern RadarInfoType RadarInfo;
extern NumMisslesChasing;
extern ChasingMisslesType MisslesInFlight[MAX_CHASING_MISSLES];
extern int AA_Tpre;
extern int AA_Tact;		//	 prelaunch time-to-active (sec)
extern PlaneParams  *AA_lockon; // IRscan plane pointer
extern DBRadarType *GetRadarPtr(long id);
extern DBWeaponType *get_weapon_ptr(long id);

extern void LogWeaponInstantiate(PlaneParams *planepnt,WeaponParams *W);
extern void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition);
extern int g_iSoundLevelExternalSFX;
void WeaponInitAltitude( WeaponParams *W );

extern void ProximitySoundHit(DBWeaponType *pweapon_type);
extern void setup_no_cockpit_art();
void PlaneHitGround( FPointDouble &pos, FPointDouble &vel, float fuel,FPoint *vector = NULL,void *vehicle = NULL);

int PlaneHasAAWeapons(PlaneParams *P);
int PlaneHasAGWeapons(PlaneParams *P);
BOOL CheckMissileAgainstGroundTargets(WeaponParams *W);
void camera_mode_fixed_chase(VKCODE vk);
extern BOOL pigs;

#define MAX_BOMB_PROGS 5
extern BombProgType BombProgs[MAX_BOMB_PROGS];

void WHomeOnJammerSearch(WeaponParams *W);
int WGetGunSndLevel(int startlevel);

extern WeaponParams *SkipThisWeap;
// extern FlirInfoType FlirInfo;
BOOL PrimaryIsAOT(DetectedPlaneListType *Plist);
extern BOOL AVIsNoiseJamming(PlaneParams *planepnt);


extern long g_iWeaponID;
extern DWORD g_dwGunSndHandle;
extern int		AA_CAGE;		// seeker head caged (t/f)
extern int		AA_SCAN;		// SCAN button (nutation)
extern float	fLast_Rmax;
extern long lShowLostTimer;
extern DWORD g_dwAIMSndHandle;
extern DWORD g_dwAIMLockSndHandle;
extern void KillSound( DWORD *SndHandle );

extern long lHARMToF;
extern long lHARMLaunched;

typedef struct
{
  int AquireMode;
  int FOVToUse;
  int TrackOn;
  int TargetSelected;
  FPointDouble Target;
  int LeftBayStat;
  int RightBayStat;
  float SlewLeftRight;
  float SlewUpDown;
  int TargetInKeyhole;
  int   GuideMethod;
  int   LaunchMode;
  int TargetObtained;
  int Magnify;
  int TargetType;
  int TrackPosObtained;
  int MarkerLight;
  int MarkersOn;
  int WithinSlewLimits;
  int DotNum;
  FPointDouble TrackPos;
  BasicInstance *GrndTrgt;
  MovingVehicleParams *VehTrgt;
  PlaneParams *PlaneTrgt;
  FPointDouble LocTrgt;
  int WeapFired;
  int LaunchTimer;
  int CameraShown;
  int AutoSet;
  int CamType;
  int LastStation;
  FPointDouble CamPos;
  FPointDouble AGDesignate;
  WeaponParams *Weap;
  FMatrix Matrix;
  FMatrix RelMatrix;
} CamInfoType;

extern DetectedPlaneListType CurFramePlanes;
extern DetectedPlaneListType PrevFramePlanes;
extern TargetInfoType *Primary;
extern void DisplayInitText(char *sTxt, int SkipLine, int showrelease=0);

// extern CamInfoType GbuInfo;
#define GBU_TRANS 1

#ifdef WATCHFALL
WeaponParams *placeholder = NULL;
WeaponParams *agpoint = NULL;
#endif

FPoint MissileFlameLightColor(0.8f,0.8f,0.65f);
FPoint CounterFlareLightColor(1.0f,0.8f,0.8f);
FPoint BurningPlaneLightColor(0.7f,0.2f,0.0f);

///////////////////////
//					 //
//   Local Defines   //
//					 //
///////////////////////

#define SIN_GUN_SLOPE	0.0262		// sin(1.5*DEGREE)
#define COS_GUN_SLOPE	0.9997		// cos(1.5*DEGREE)

#define G_TIX	(GRAVITY*FTTOWU/2500.0)	// Gravity in WU/tick squared

//////////////////////////
//					    //
//   Global Variables   //
//						//
//////////////////////////

extern AvionicsType Av;
extern WeapStoresType WeapStores;



//============================================================================
//		GENERAL WEAPONS MANAGEMENT
//============================================================================

VKCODE vkPrimary, vkSecondary;

void set_primary(VKCODE vk)
{
	vkPrimary = vk;
}


void set_secondary(VKCODE vk)
{
	vkSecondary = vk;
}


/*----------------------------------------------------------------------------
 *
 *	InitWeapons()
 *
 */
void InitWeapons(void)
{
	ZeroMemory(Weapons,sizeof(WeaponParams)*MAX_WEAPON_SLOTS);
	LastWeapon = &Weapons[-1];

	ZeroMemory( AAAVertexes, sizeof(AAAStreamVertex) * MAX_AAA_STREAM_VERTEXES );
	LastAAAVertex = &AAAVertexes[0];
	AAAStreams = NULL;

	InitShockWaveStuff();

	InitFunnel();
}

/*----------------------------------------------------------------------------
 *
 *	GetWeaponSlot()
 *
 *		Return pointer to first free weapon slot or NULL for none available.
 *		Also update "LastWeapon" as required.
 *
 */
WeaponParams *GetWeaponSlot()
{
	for (WeaponParams *W=Weapons; W<=LastWeapon && (W->Flags & WEAPON_INUSE); W++) {}

	if (W == &Weapons[MAX_WEAPON_SLOTS])  return NULL;

	if (W > LastWeapon)  LastWeapon = W;

	return W;
}

/*----------------------------------------------------------------------------
 *
 *	Do50HzPlaneBasedWeaponStuff()
 *
 *		Called from MovePlanes()  --  Instantiate repetitive weapons
 *
 */
void Do50HzPlaneBasedWeaponStuff( PlaneParams *P )
{
	//
	// Shoot Bullets
	//
	if (P->Trigger1)
	{
		if (++P->ElapsedFireRate >= P->GunFireRate)
		{
			if((P->Status & PL_DEVICE_DRIVEN) || (P == PlayerPlane))
			{
				Button1Press();
			}
			else
			{
				// put stub here for AI function to decrement/track bullet inventory
				if (!P->MachineGunLight)
				{
					if ((P->Type->MachineGun.Z) && (P->MachineGunLight = (PointLightSource **)GetNewLight()))
					{
						PointLightSource *new_light = new PointLightSource;
						new_light->WorldPosition = P->WorldPosition;
				//		new_light->Color = WhiteishFireColor;
						new_light->Color.SetValues(0.85,0.85,0.75);
						new_light->Flags |= LIGHT_TINY_SPOT;
						new_light->Radius1 = 20.0 FEET;
						new_light->ooRadius1 = 1.0f/(20.0 FEET);
						new_light->Radius2 = 5.0 FEET;
						new_light->ooRadius2 = 1.0f/(5.0 FEET);
						*(P->MachineGunLight) = new_light;
					}
				}

				InstantiateBullet(P,TRUE);
			}

			if (P->MachineGunLight)
				(*(P->MachineGunLight))->Intensity = 0.5f * frand();

			P->ElapsedFireRate = 0;
		}
		else
			if (P->MachineGunLight)
			{
				if (P->GunFireRate)
					(*(P->MachineGunLight))->Intensity = (0.5f * frand())*((float)P->ElapsedFireRate / (float)P->GunFireRate);
				else
					(*(P->MachineGunLight))->Intensity = 0.5f * frand();
			}
	}
	else
	{
		P->ElapsedFireRate = 0;
		if (P->MachineGunLight)
		{
			RemoveLight((LightSource **)P->MachineGunLight);
			P->MachineGunLight = NULL;
		}
	}


	//
	//	COMM planes only do bullet processing
	//
	if (P->Status & PL_COMM_DRIVEN)  return;

	//
	//	Mark funnel spots
	//
	static int FunnelUpdate = 0;

	if ((UFC.MasterMode == AA_MODE) && !P->OnGround && (++FunnelUpdate & 1))
		InstantiateFunnel( P );

	//
	//	Drop ripple bombs
	//
	static int SimOneShot = 0;

    int SimActivate = Av.Weapons.Bomb.Drop.Activate;

	if (SimActivate)
	 	DoBombReleaseSim(SimActivate != SimOneShot);

	SimOneShot = SimActivate;

	static int SimOneShotRocket = 0;

	if(Av.Weapons.Bomb.Rocket.Activate)
		DoRocketReleaseSim(Av.Weapons.Bomb.Rocket.Activate != SimOneShotRocket);

  	SimOneShotRocket = Av.Weapons.Bomb.Rocket.Activate;

}

/* -----------------8/11/99 9:39AM---------------------------------------------------------------------
/* check to see if cluster bombs, durandels and FAE weapons should burst
/* ----------------------------------------------------------------------------------------------------*/

void SetAirBurstEffect(WeaponParams *W,float scale)
{
	FPoint novel(0.0f,0.0f,0.0f);
	W->Flags |= ALREADY_BURST;
	W->lTimer = GameLoop;
	NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, scale);
}

BOOL CheckForAirBurst(WeaponParams *W)
{
	DBWeaponType *pweapon_type;

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	float burst_height;

	if(W->WeaponMove == MoveBombs)
	{
		burst_height = W->fpTempWay.Y;
	}
	else
	{
		if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
		{
			burst_height = 350 FEET;
		}
		else if (pweapon_type->iWarHead == WARHEAD_CRATERING)
		{
			burst_height = 500 FEET;
		}
		else
		{
			burst_height = 1000 FEET;
		}
	}

	if ((W->Flags & CHECK_FOR_BURST) && ((GameLoop - (DWORD)W->lTimer) > 50*3) && (W->Altitude < burst_height))
	{
		W->Flags &= ~CHECK_FOR_BURST;

		if (pweapon_type->iWarHead == WARHEAD_CRATERING)
		{
			W->Flags |= DURANDEL_STAGE2;
			if (!W->Smoke)
			{
				W->Smoke  = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);
				((SmokeTrail *)W->Smoke)->Update();
			}
		}
		else
			if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
			{
			 	SetAirBurstEffect(W,2.0f);
				W->Flags |= FUEL_AIR_STAGE2;
			}
			else
				if (pweapon_type->iWeaponType == WEAPON_TYPE_CLUSTER_BOMB)
					SetAirBurstEffect(W,0.5f);

		return TRUE;
	}

	return FALSE;
}

BOOL CanDoGroundExplosion(WeaponParams *W)
{
	if (((W->Flags & ALREADY_BURST) || !(W->Flags & CHECK_FOR_BURST)) && (!(W->Flags & WEP_JETTISONED)))
		return TRUE;
	else
		return FALSE;
}

/*----------------------------------------------------------------------------
 *
 *	MoveWeapons()
 *
 *		Companion to MovePlanes()  --  do weapon physics @ 50Hz
 *
 */
void MoveWeapons(void)
{
	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
		if (W->Flags & WEAPON_INUSE)
			W->WeaponMove(W);

	MoveFunnel();

	lBombFlags &= ~(WSO_TURN_BOMB_R|WSO_TURN_BOMB_L|WSO_BOMB_LONGER|WSO_BOMB_SHORTER);
}


int PlaneAttitudeWithinConstraints(PlaneParams *P)
{
	int RetVal = 1;
	int TmpRoll = P->Roll/DEGREE;
	int TmpPitch = P->Pitch/DEGREE;

	if ((((TmpRoll > 80) && (TmpRoll  < 280))  ||		// +- 80 degree roll/pitch
 		((TmpPitch > 80) && (TmpPitch < 280))) || (P->GForce < 0.5f))
	{
		RetVal = 0;									 // don't release bombs
	}
	return (RetVal);
}
/*----------------------------------------------------------------------------
 *
 *	DetectWeaponFire()
 *
 *		Called from ContolPlanes() --> GetF18StickPos()
 *
 *		Check for Trigger1 or Trigger2 and respond.
 *
 */
void DetectWeaponFire(PlaneParams *P)
{
	static int WeaponRelease = 0;
	int iLevel=g_iSoundLevelExternalSFX-10;
	if (iLevel<0) iLevel=0;

	//**  Moved this lower.  There's a problem if you have crashes off and are firing the cannon when you hit the ground.
//	if ((P->OnGround) || (P->AGL <= 20))  return;	// Disable ALL ordnance till take-off.

	if(P->AI.iAIFlags1 & AI_HAS_EJECTED)
	{
		return;
	}

	// so sounds are not played when master arm is off
	if( (GetVkStatus(vkPrimary)) && (UFC.MasterArmState == OFF))
	{
  		P->Trigger1 = 0;

		if(pSeatData)
		{
			pSeatData->Gun = 0;
		}

		if (g_dwGunSndHandle)
		{
			SndEndSound(g_dwGunSndHandle);
			if (iLevel)
			{
				iLevel = WGetGunSndLevel(iLevel);
				if (PlayerPlane->GunFireRate==1)
				{
					SndQueueSound(Canhofi,1,iLevel);
				}
				if (PlayerPlane->GunFireRate==2)
				{
					SndQueueSound(Canlofi,1,iLevel);
				}
			}
		}
  		g_dwGunSndHandle  = 0;
		return;
	}

	// can't fire in aguns mode unless in range and pipper on enemy
	if(WeapStores.GunsOn && WeapStores.AGunOn && !WeapStores.AGunCanFire)
	{
  		P->Trigger1 = 0;

		if(pSeatData)
		{
			pSeatData->Gun = 0;
		}

		if (g_dwGunSndHandle)
		{
			SndEndSound(g_dwGunSndHandle);
			iLevel = WGetGunSndLevel(iLevel);
			if (iLevel)
			{
				if (PlayerPlane->GunFireRate==1)
				{
					SndQueueSound(Canhofi,1,iLevel);
				}
				if (PlayerPlane->GunFireRate==2)
				{
					SndQueueSound(Canlofi,1,iLevel);
				}
			}
		}
  		g_dwGunSndHandle  = 0;
		return;
	}

	// Detect GunFire

	if ((GetVkStatus(vkPrimary) && P->WeapLoad[GUNS_STATION].Count > 0) && (!(P->SystemInactive & (DAMAGE_BIT_GUN))) && (!((P->OnGround) || (P->AGL <= 20))) && (!(P->FlightStatus & PL_GEAR_DOWN_LOCKED)))
	{
		if (P->Trigger1 == 0)
		{
			P->ElapsedFireRate = P->GunFireRate;  // this makes first bullet fire immediately

			if (!g_dwGunSndHandle)
			{
				if (PlayerPlane->GunFireRate==1)
				{
					iLevel = WGetGunSndLevel(iLevel);
					if (iLevel)
					{
						SndQueueSound(Canhoni,1,iLevel);
						g_dwGunSndHandle = SndQueueSound(Canhlpi, 0,iLevel);
					}
				}
				if (PlayerPlane->GunFireRate==2)
				{
					iLevel = WGetGunSndLevel(iLevel);
					if (iLevel)
					{
						SndQueueSound(Canloni,1,iLevel);
						g_dwGunSndHandle = SndQueueSound(Canllpi, 0,iLevel);
					}
				}
			}
		}
		if(g_dwGunSndHandle)
		{
			iLevel = WGetGunSndLevel(iLevel);
			SndSetSoundVolume(g_dwGunSndHandle, iLevel);
		}

		if(pSeatData)
		{
			pSeatData->Gun = P->GunFireRate;
		}

		P->Trigger1 = 1;
	}
	else
	{
  		P->Trigger1 = 0;

		if(pSeatData)
		{
			pSeatData->Gun = 0;
		}

		if (g_dwGunSndHandle)
		{
			SndEndSound(g_dwGunSndHandle);
			if (iLevel)
			{
				iLevel = WGetGunSndLevel(iLevel);
				if (PlayerPlane->GunFireRate==1)
				{
					SndQueueSound(Canhofi,1,iLevel);
				}
				if (PlayerPlane->GunFireRate==2)
				{
					SndQueueSound(Canlofi,1,iLevel);
				}
			}
		}
  		g_dwGunSndHandle  = 0;
  	}

	// Detect Bomb Release

	if ((P->OnGround) || (P->AGL <= 20))  return;	// Disable ALL ordnance till take-off.

	if ((GetVkStatus(vkSecondary)) && (!(P->SystemInactive & (DAMAGE_BIT_PACS)))
		&& (P->AGL > 20))
	{
		if ((UFC.MasterMode == AG_MODE) && PlaneAttitudeWithinConstraints(P) )
		{
			extern int AG_guided;
			if ( (UFC.AGSubMode == CDIP_AG) || (UFC.AGSubMode == MAN_AG) || AG_guided)
			{
				P->Trigger2 = 1;
				if (!WeaponRelease)		// this is a one shot, make sure ripple works though
				{
					WeaponRelease = 1;
					Button2Press();
				}
			}
			else
				if ((UFC.AGSubMode == AUTO_AG) || (UFC.AGSubMode == AUTO_LOFT_AG))
				{
					P->Trigger2 = 1;
					if (!WeaponRelease && (UFC.TRelCountDown < 1))	// if consent given
					{
						WeaponRelease = 1;
						Button2Press();
					}
				}
		}
		else
			if (UFC.MasterMode != AG_MODE)		// fire missiles
			{
				P->Trigger2 = 1;
				if (!WeaponRelease)
				{
					WeaponRelease = 1;
					Button2Press();
				}
			}
	}
	else
	{
  		P->Trigger2 = 0;
		WeaponRelease = 0;
	}
}

//============================================================================
//		FUNNEL CODE  --  INIT / INSTANTIATE / DELETE / MOVE / DISPLAY
//============================================================================

#define FUNNEL_TIX	 70		// LifeTime in tix (70 = 5000'/3380fps * 50Hz)

#define MAX_FUNNEL	(FUNNEL_TIX / 2)	// divide by FunnelUpdate rate

typedef struct _FunnelCake
{
	int			 LifeTime;		// life in ticks

	FPointDouble Vel;			// velocity vector (WU/tick)
	FPointDouble PosL;			// left  position  (WU)
	FPointDouble PosR;			// right position  (WU)

} FunnelCake;

// The queue will always have one wasted slot when full since head==tail
// is our empty indicator.  Hence the max size is "+1"
//
FunnelCake  Funnel[MAX_FUNNEL+1];	// one wasted slot

FunnelCake *LastFunnel = &Funnel[MAX_FUNNEL];		// end sentinel

FunnelCake *FunnelHead;		// next free slot
FunnelCake *FunnelTail;		// oldest slot in use if head != tail

/*----------------------------------------------------------------------------
 *
 *	InitFunnel()
 *
 */
void InitFunnel( void )
{
	FunnelHead = FunnelTail = Funnel;	// MTQ
}

/*----------------------------------------------------------------------------
 *
 *	GetBallisticWUGivenDistance()
 *
 */
FPoint GetBallisticWUGivenDistance(PlaneParams *P, double WUDist)
{
	FunnelCake *F = FunnelTail;

	while (F != FunnelHead)
	{
		if ((F->PosL - P->WorldPosition) < WUDist) break;

	  	if (++F > LastFunnel)  F = Funnel;	// wrap
	}
	FPointDouble tVector;

	tVector  = F->PosR;
	tVector -= F->PosL;
	tVector *= 0.5;
	tVector += F->PosL;

	return tVector;
}
/*----------------------------------------------------------------------------
 *
 *	InstantiateFunnel()
 *
 */
void InstantiateFunnel( PlaneParams *P )
{
	FunnelCake *F;

	F = FunnelHead++;

	if (FunnelHead > LastFunnel)
		FunnelHead = Funnel;

	if (FunnelHead == FunnelTail) {		// queue is FULL
		FunnelHead = F;					// so back up head
		return;							// and return
	}

	double	InitialVelocity = 3380.0 * FTTOWU / 50;

	F->Vel.X = (P->Orientation.I.X*COS_GUN_SLOPE - P->Orientation.K.X*SIN_GUN_SLOPE)*InitialVelocity;
	F->Vel.Y = (P->Orientation.I.Y*COS_GUN_SLOPE - P->Orientation.K.Y*SIN_GUN_SLOPE)*InitialVelocity;
	F->Vel.Z = (P->Orientation.I.Z*COS_GUN_SLOPE - P->Orientation.K.Z*SIN_GUN_SLOPE)*InitialVelocity;

	F->PosL.X = P->WorldPosition.X + P->Orientation.I.X*(250.0*FTTOWU) - P->Orientation.K.X*(1.5*FTTOWU) + P->Orientation.J.X*(-20.0*FTTOWU);
	F->PosL.Y = P->WorldPosition.Y + P->Orientation.I.Y*(250.0*FTTOWU) - P->Orientation.K.Y*(1.5*FTTOWU) + P->Orientation.J.Y*(-20.0*FTTOWU);
	F->PosL.Z = P->WorldPosition.Z + P->Orientation.I.Z*(250.0*FTTOWU) - P->Orientation.K.Z*(1.5*FTTOWU) + P->Orientation.J.Z*(-20.0*FTTOWU);

	F->PosR.X = P->WorldPosition.X + P->Orientation.I.X*(250.0*FTTOWU) - P->Orientation.K.X*(1.5*FTTOWU) + P->Orientation.J.X*(+20.0*FTTOWU);
	F->PosR.Y = P->WorldPosition.Y + P->Orientation.I.Y*(250.0*FTTOWU) - P->Orientation.K.Y*(1.5*FTTOWU) + P->Orientation.J.Y*(+20.0*FTTOWU);
	F->PosR.Z = P->WorldPosition.Z + P->Orientation.I.Z*(250.0*FTTOWU) - P->Orientation.K.Z*(1.5*FTTOWU) + P->Orientation.J.Z*(+20.0*FTTOWU);

	F->LifeTime	= FUNNEL_TIX;
}

/*----------------------------------------------------------------------------
 *
 *	DeleteFunnel()
 *
 */
void DeleteFunnel( FunnelCake *F )
{
	F = FunnelTail++;	// this should always == F input

	if (FunnelTail > LastFunnel)
		FunnelTail = Funnel;
}

/*----------------------------------------------------------------------------
 *
 *	MoveFunnel()
 *
 */
void MoveFunnel( void )
{
	FunnelCake *F = FunnelTail;

	while (F != FunnelHead)
	{
		if (--F->LifeTime > 0)
		{
			F->PosL   += F->Vel;
			F->PosR   += F->Vel;
			F->PosL.Y -= G_TIX/2;
			F->PosR.Y -= G_TIX/2;
			F->Vel.Y  -= G_TIX;
		}
		else
			DeleteFunnel(F);

		if (++F > LastFunnel)  F = Funnel;	// wrap
	}
}

//extern void CRT_icon( int, int, int, int skewX=0, int skewY=0, int color=HUDColor );
//extern void CRT_line( int, int, int, int, int color=HUDColor );

//*****************************************************************************************************************************************
// DISPLAY FUNNEL  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DisplayFunnel(int ShowFunnel,PlaneParams *P , int *FunnelX, int *FunnelY)
{
	if (P->OnGround)  return;

	int			i = 0;		// loop counter for l/r phase and init.
	int			p = 3;		// pipper counter
	FunnelCake *F = FunnelTail;
	int PipperSet = FALSE;
	float TargetDist;

	if(P->AADesignate)
	{
		TargetDist = PlayerPlane->WorldPosition / P->AADesignate->WorldPosition;
	}

	float pip[] = { 0, 1000*FTTOWU, 2250*FTTOWU, 4000*FTTOWU };


	while (F != FunnelHead)
	{
		struct { int X,Y; } newP, oldP[2];	// last left & right points
		FPoint   fPos;

		int j = i&1;	// left/right phase flag

		fPos  = j ? F->PosR : F->PosL;
		fPos -= Camera1.CameraLocation;//P->WorldPosition;
		fPos *= ViewMatrix;


		if (!fPos.Perspect( &newP.X, &newP.Y) )
			return;		// don't know how to handle this odd case, so be safe

		if (i++ >= 2)	// ie. we have our left and right start points
		{
			if(ShowFunnel)
			  CRT_line( oldP[j].X, oldP[j].Y, newP.X, newP.Y );
		}

		oldP[j] = newP;

		// check for pippers @ 1000' 2250' and 4000'
		if (j && p && (F->PosL - P->WorldPosition < pip[p]))
		{
			int pipX = (oldP[0].X + oldP[1].X) / 2;
			int pipY = (oldP[0].Y + oldP[1].Y) / 2;


			//if ((CurrWeapMode == GUN_MODE) && (UFC.AASubMode == AA_GUN_FNL))
			//{
			  if(ShowFunnel)
				  CRT_icon( pipX, pipY, 12 /*SM_CIRCLE_4*/ );



			//}
			p--;
		}

		if(P->AADesignate)
		{
		  if( j && p && (F->PosL - P->WorldPosition < TargetDist))
			{
				int pipX = (oldP[0].X + oldP[1].X) / 2;
				int pipY = (oldP[0].Y + oldP[1].Y) / 2;

				if(!PipperSet)
				{
				  *FunnelX = pipX;
				  *FunnelY = pipY;
					PipperSet = TRUE;
				}
			}
		}

		if (j && (++F > LastFunnel))  F = Funnel;	// wrap
	}

}

//============================================================================
//		BULLET CODE  --  INSTANTIATE / DELETE / MOVE / DISPLAY
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	InstantiateBullet()
 *
 */
void InstantiateBullet( PlaneParams *P, BOOL tracer )
{
	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	if(P == PlayerPlane)
	{
		if(!AICheckForMoreBullets(PlayerPlane, W))
		{
			iPlayerBombFlags &= ~(WSO_BULLET_HIT);
		}
	}

	W->SortType = BULLET_OBJECT;
//	GetSortHeader(W,BULLET_OBJECT);

	AICheckROE(P, 2);
	W->P				= P;

	W->iNetID			= -1;

	W->Launcher			= (void *)P;
	W->LauncherType		= AIRCRAFT;
	W->Kind				= BULLET;
	W->Flags			= BULLET_INUSE;
	W->Smoke = NULL;
	if(P == PlayerPlane)
	{
//		if(!(PlayerPlane->WeapLoad[GUNS_STATION].Count & 0xE))
		if(!(lPlayerBulletCount))
		{
			W->Flags |= WEP_BULLET_GROUND_CHECK;
			W->lTargetDistFt = 8;  //  Bullet multiplier for ground stuff
			W->lTimer = -1;
		}
		lPlayerBulletCount ++;
		lPlayerBulletCount &= 0x7;
	}
	W->WeaponMove		= MoveBullets;
    W->Type				= P->WeapLoad[GUNS_STATION].Type;
	W->InitialVelocity  = 3380;				// this will ultimately be pulled from weapons structure
	W->InitialVelocity *= FTTOWU / 50;		// convert to WU/tick
	W->LifeTime			= 125;				// 2.5 secs * 50Hz
	W->GroundHeight		= 0.0f;

	/* this is specific to the F18e and should be changed */
	if (P->Type->MachineGun.Z)
	{
		W->Pos = P->WorldPosition;
		W->Pos.AddScaledVector(P->Type->MachineGun.X,P->Orientation.J);
		W->Pos.AddScaledVector(-P->Type->MachineGun.Y,P->Orientation.K);
		W->Pos.AddScaledVector(-P->Type->MachineGun.Z,P->Orientation.I);

//		W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(7.0*FTTOWU) - P->Orientation.K.X*(1.5*FTTOWU) + P->Orientation.J.X*(5.75*FTTOWU)
//		W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(7.0*FTTOWU) - P->Orientation.K.Y*(1.5*FTTOWU) + P->Orientation.J.Y*(5.75*FTTOWU)
//		W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(7.0*FTTOWU) - P->Orientation.K.Z*(1.5*FTTOWU) + P->Orientation.J.Z*(5.75*FTTOWU)
	}
	else
	{
		W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(7.0*FTTOWU) - P->Orientation.K.X*(1.5*FTTOWU) + P->Orientation.J.X*(5.75*FTTOWU);
		W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(7.0*FTTOWU) - P->Orientation.K.Y*(1.5*FTTOWU) + P->Orientation.J.Y*(5.75*FTTOWU);
		W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(7.0*FTTOWU) - P->Orientation.K.Z*(1.5*FTTOWU) + P->Orientation.J.Z*(5.75*FTTOWU);
	}
//	else
//	{
//		W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(2.742f*FTTOWU) + P->Orientation.K.X*(4.0f*FTTOWU);// - P->Orientation.K.X*(1.5*FTTOWU) + P->Orientation.J.X*(5.75*FTTOWU);
//		W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(2.742f*FTTOWU) + P->Orientation.K.Y*(4.0f*FTTOWU);// - P->Orientation.K.Y*(1.5*FTTOWU) + P->Orientation.J.Y*(5.75*FTTOWU);
//		W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(2.742f*FTTOWU) + P->Orientation.K.Z*(4.0f*FTTOWU);// - P->Orientation.K.Z*(1.5*FTTOWU) + P->Orientation.J.Z*(5.75*FTTOWU);
//	}

	W->Vel.X = (P->Orientation.I.X*COS_GUN_SLOPE - P->Orientation.K.X*SIN_GUN_SLOPE)*W->InitialVelocity;
	W->Vel.Z = (P->Orientation.I.Z*COS_GUN_SLOPE - P->Orientation.K.Z*SIN_GUN_SLOPE)*W->InitialVelocity;
	W->Vel.Y = (P->Orientation.I.Y*COS_GUN_SLOPE - P->Orientation.K.Y*SIN_GUN_SLOPE)*W->InitialVelocity;

	FPointDouble new_up,new_right;

	new_up.SetValues((frand()*2.0 - 1.0)*0.005*W->InitialVelocity,P->Orientation.K);
	new_right.SetValues((frand()*2.0 - 1.0)*0.005*W->InitialVelocity,P->Orientation.J);

	new_up += new_right;

	W->Vel += new_up;

	if((P == PlayerPlane) && (cPlayerLimitWeapons))
	{
		P->AircraftDryWeight -= 0.5 * 2;		    // weight per bullet pair
		P->AI.fStoresWeight -= 0.5 * 2;
	}
	else if(P->Status & PL_AI_DRIVEN)
	{
//		P->AI.fStoresWeight -= pDBWeaponList[P->WeapLoad[GUNS_STATION].WeapIndex].iWeight * 2;
		P->AI.fStoresWeight -= 0.5 * 2;
	}
	else if(cPlayerLimitWeapons)
	{
		P->AircraftDryWeight -= 0.5 * 2;		    // weight per bullet pair
		P->AI.fStoresWeight -= 0.5 * 2;
	}

	if ((P->AADesignate != NULL) && (P->Status & PL_AI_DRIVEN))
	{
		W->iTargetType	= TARGET_PLANE;
		W->pTarget		= P->AADesignate;
	}
	else
	{
		W->iTargetType	= NONE;
		W->pTarget		= NULL;
	}

	AIC_WSO_Guns_Msgs(P, W);
	LogWeaponInstantiate(P,W);
	LogWeaponInstantiate(P,W);
}

/*----------------------------------------------------------------------------
 *
 *	InstantiateTurretBullet()
 *
 */
void InstantiateTurretBullet( PlaneParams *P, BOOL tracer, WeaponType *wtype, ANGLE heading, ANGLE pitch, FPoint *offset, int numbershot)
{
	int cnt;
	DBWeaponType *pweapon_type;
	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	if(P == PlayerPlane)
	{
		if(!AICheckForMoreBullets(PlayerPlane, W))
		{
			iPlayerBombFlags &= ~(WSO_BULLET_HIT);
		}
	}

	W->SortType = BULLET_OBJECT;
//	GetSortHeader(W,BULLET_OBJECT);

	AICheckROE(P, 2);
	W->P				= P;
	W->iNetID			= -1;
	W->Launcher			= (void *)P;
	W->LauncherType		= AIRCRAFT;
	W->Kind				= BULLET;
	W->Flags			= BULLET_INUSE;
	W->WeaponMove		= MoveBullets;
    W->Type				= wtype;
	W->Smoke = NULL;

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	W->InitialVelocity = GDGetWeaponVelocity(pweapon_type);
	W->InitialVelocity = W->InitialVelocity * FTTOWU / 50;		// convert to WU/tick

//	W->LifeTime			= 125;				// 2.5 secs * 50Hz
	W->LifeTime			= GDGetAAADuration(pweapon_type);

	W->GroundHeight		= 0.0f;

	/* this is specific to the F18e and should be changed */
	if(!offset)
	{
		W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(7.0*FTTOWU) - P->Orientation.K.X*(1.5*FTTOWU) + P->Orientation.J.X*(5.75*FTTOWU);
		W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(7.0*FTTOWU) - P->Orientation.K.Y*(1.5*FTTOWU) + P->Orientation.J.Y*(5.75*FTTOWU);
		W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(7.0*FTTOWU) - P->Orientation.K.Z*(1.5*FTTOWU) + P->Orientation.J.Z*(5.75*FTTOWU);
	}
	else
	{
		W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(double)offset->Z + P->Orientation.J.X*(double)offset->X + P->Orientation.K.X*(double)offset->Y;
		W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(double)offset->Z + P->Orientation.J.Y*(double)offset->X + P->Orientation.K.Y*(double)offset->Y;
		W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(double)offset->Z + P->Orientation.J.Z*(double)offset->X + P->Orientation.K.Z*(double)offset->Y;
	}

//	W->Vel.X = (P->Orientation.I.X*COS_GUN_SLOPE - P->Orientation.K.X*SIN_GUN_SLOPE)*W->InitialVelocity;
//	W->Vel.Y = (P->Orientation.I.Y*COS_GUN_SLOPE - P->Orientation.K.Y*SIN_GUN_SLOPE)*W->InitialVelocity;
//	W->Vel.Z = (P->Orientation.I.Z*COS_GUN_SLOPE - P->Orientation.K.Z*SIN_GUN_SLOPE)*W->InitialVelocity;

#if 1
	double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
	W->Vel.X = FSinTimes(heading, -hvel);
	W->Vel.Z = FCosTimes(heading, -hvel);
#else
	W->Vel.X = FSinTimes(heading, -W->InitialVelocity);
	W->Vel.Z = FCosTimes(heading, -W->InitialVelocity);
#endif
	W->Vel.Y = FSinTimes(pitch, W->InitialVelocity);

#if 0
	if(Camera1.CameraMode != CAMERA_FREE)
	{
		setup_no_cockpit_art();
		AssignCameraSubject((void *)W,CO_WEAPON);
		if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
		{
			setup_no_cockpit_art();
			Camera1.CameraMode = CAMERA_FREE;
			ChangeViewModeOrSubject(CAMERA_FREE);
		}
		else
	 		ChangeViewModeOrSubject(Camera1.CameraMode);

		SimPause ^= 1;
	}
#endif

	if((P == PlayerPlane) && (cPlayerLimitWeapons))
	{
#if 1 //  400 rounds at 200 lbs mean 1/2 lpb per bullet.
		P->AircraftDryWeight -= 0.5 * 2;		    // weight per bullet pair
		P->AI.fStoresWeight -= 0.5 * 2;
#else //  old values
		P->AircraftDryWeight -= 0.5644 * 2;		    // weight per bullet pair
		P->AI.fStoresWeight -= 0.5644 * 2;
#endif
	}
	else if(P->Status & PL_AI_DRIVEN)
	{
//		P->AI.fStoresWeight -= pDBWeaponList[P->WeapLoad[GUNS_STATION].WeapIndex].iWeight * 2;
		P->AI.fStoresWeight -= 0.5 * 2;
	}
	else if(cPlayerLimitWeapons)
	{
#if 1 //  400 rounds at 200 lbs mean 1/2 lpb per bullet.
		P->AircraftDryWeight -= 0.5 * 2;		    // weight per bullet pair
		P->AI.fStoresWeight -= 0.5 * 2;
#else //  old values
		P->AircraftDryWeight -= 0.5644 * 2;		    // weight per bullet pair
		P->AI.fStoresWeight -= 0.5644 * 2;
#endif
	}

	if ((P->AADesignate != NULL) && (P->Status & PL_AI_DRIVEN))
	{
		W->iTargetType	= TARGET_PLANE;
		W->pTarget		= P->AADesignate;
	}
	else
	{
		W->iTargetType	= NONE;
		W->pTarget		= NULL;
	}

	AIC_WSO_Guns_Msgs(P, W);

	for(cnt = 0; cnt < numbershot; cnt ++)
	{
		LogWeaponInstantiate(P,W);
	}
}

void InstantiateAAABullet( BasicInstance *P,ANGLE pitch, ANGLE heading,long id)
{
	double h_vel;
	double fang;
	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;
	DBWeaponType *pweapon_type;
	float bulletvel;
	GDRadarData *radardat;

	W->SortType = BULLET_OBJECT;
//	GetSortHeader(W,BULLET_OBJECT);

	W->P				= NULL;
	W->iNetID			= -1;
	W->Launcher			= (void *)P;
	W->LauncherType		= GROUNDOBJECT;
	W->Kind				= BULLET;
	W->Flags			= BULLET_INUSE | RED_TRACER;
	W->WeaponMove		= MoveBullets;
    W->Type				= (WeaponType *)id;
	W->GroundHeight		= 0.0f;
	W->Smoke = NULL;

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}


//	W->InitialVelocity  = 3380;				// this will ultimately be pulled from weapons structure
	bulletvel = GDGetWeaponVelocity(pweapon_type);
	W->InitialVelocity = bulletvel * FTTOWU / 50;		// convert to WU/tick

//	W->LifeTime			= 125;				// 2.5 secs * 50Hz
	W->LifeTime			= GDGetAAADuration(pweapon_type);				// 2.5 secs * 50Hz

	/* this is specific to the F18e and should be changed */
	W->Pos.X = P->Position.X;
	W->Pos.Y = P->Position.Y;
	W->Pos.Z = P->Position.Z;

	fang = ANGLE_TO_RADIANS(pitch);

	W->Vel.Y = W->InitialVelocity * sin(fang);
	h_vel =    W->InitialVelocity * cos(fang);

	fang = ANGLE_TO_RADIANS(heading);

	W->Vel.X = -sin(fang)*h_vel; //the negatives are cuz the h_vel is always positive
	W->Vel.Z = -cos(fang)*h_vel;

	radardat = GDGetRadarData(P);
	if(radardat->lWFlags1 & (GD_W_RANDOM_FIRE))
	{
		if(iDoAllAAA == 0)
		{
			GDCheckForRandomAAAHit(W->Pos, pweapon_type, W);
		}

		W->Flags |= WEP_NO_CHECK;
		W->iTargetType	= NONE;
		W->pTarget		= NULL;
	}
	else
	{
		W->iTargetType	= TARGET_PLANE;
		W->pTarget		= radardat->Target;
	}
}

void InstantiateAAABullet( MovingVehicleParams *P,ANGLE pitch, ANGLE heading,long id, int radardatnum)
{
	double h_vel;
	double fang;
	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;
	DBWeaponType *pweapon_type;
	float bulletvel;
	FPointDouble weaponoffset;

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	W->SortType = BULLET_OBJECT;
//	GetSortHeader(W,BULLET_OBJECT);

	W->P				= NULL;
	W->iNetID			= -1;
	W->Launcher			= (void *)P;
	W->LauncherType		= MOVINGVEHICLE;
	W->Kind				= BULLET;
	W->Flags			= BULLET_INUSE | RED_TRACER;
	W->WeaponMove		= MoveBullets;
    W->Type				= (WeaponType *)id;
	W->GroundHeight		= 0.0f;

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}


//	W->InitialVelocity  = 3380;				// this will ultimately be pulled from weapons structure
	bulletvel = GDGetWeaponVelocity(pweapon_type);
	W->InitialVelocity = bulletvel * FTTOWU / 50;		// convert to WU/tick

//	W->LifeTime			= 125;				// 2.5 secs * 50Hz
	W->LifeTime			= GDGetAAADuration(pweapon_type);				// 2.5 secs * 50Hz

	/* this is specific to the F18e and should be changed */
	W->Pos.X = P->WorldPosition.X;
	W->Pos.Y = P->WorldPosition.Y;
	W->Pos.Z = P->WorldPosition.Z;

	VGDGetWeaponPosition(P, MOVINGVEHICLE, radardatnum, &weaponoffset);
	W->Pos.X = W->Pos.X + weaponoffset.X;
	W->Pos.Y = W->Pos.Y + weaponoffset.Y;
	W->Pos.Z = W->Pos.Z + weaponoffset.Z;

	fang = ANGLE_TO_RADIANS(pitch);

	W->Vel.Y = W->InitialVelocity * sin(fang);
	h_vel =    W->InitialVelocity * cos(fang);

	fang = ANGLE_TO_RADIANS(heading);

	W->Vel.X = -sin(fang)*h_vel; //the negatives are cuz the h_vel is always positive
	W->Vel.Z = -cos(fang)*h_vel;

	if(P->RadarWeaponData[radardatnum].lWFlags1 & (GD_W_RANDOM_FIRE))
	{
		if((iDoAllAAA == 0) || (iInJump))
		{
			GDCheckForRandomAAAHit(W->Pos, pweapon_type, W);
			if(iInJump)
			{
				DeleteBullet(W);
				return;
			}
		}

		W->Flags |= WEP_NO_CHECK;
		W->iTargetType	= NONE;
		W->pTarget		= NULL;
	}
	else
	{
		W->iTargetType	= TARGET_PLANE;
		W->pTarget		= P->RadarWeaponData[radardatnum].Target;
	}
}

/*----------------------------------------------------------------------------
 *
 *	DeleteBullet()
 *
 */
void DeleteBullet( WeaponParams *W )
{
	W->Flags = 0;
	W->Kind  = NONE;
	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	}
	W->Smoke = NULL;

//	UnsortObject(W);

	if (W == LastWeapon)
		while((LastWeapon >= Weapons) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;
}

/*----------------------------------------------------------------------------
 *
 *	MoveBullets()
 *
 */
void MoveBullets( WeaponParams *W )
{
	PlaneParams *weaponhitplane = NULL;

	if (W->LifeTime--)
	{
		W->LastPos = W->Pos;
		W->Pos    += W->Vel;
		W->Pos.Y  -= G_TIX/2;
		W->Vel.Y  -= G_TIX;

		if((iDoAllAAA != 0) || (!(W->Flags & WEP_NO_CHECK)))
		{
			weaponhitplane = WeaponHitsPlane(W);
		}

		if (weaponhitplane)
		{
//		 	DoExplosionInAir(W);
			DeleteBullet(W);
		}
		else if (WeaponIntersectsGround(W))
		{
			if(W->Flags & WEP_BULLET_GROUND_CHECK)
			{
				W->Pos.Y = LandHeight( W->Pos.X, W->Pos.Z) + (4.0f * FTTOWU);
				DoExplosionOnGround(W);
			}
			DeleteBullet(W);
		}
#if 0
		else if(W->Flags & WEP_BULLET_GROUND_CHECK)
		{
			if(W->Altitude < (600.0f * FTTOWU))
			{
				if(W->lTimer >= 0)
				{
					W->lTimer -= 20;
				}

				if(W->lTimer < 0)
				{
					DoExplosionOnGround(W);
				}
			}
		}
#endif
	}
	else
		DeleteBullet(W);
}


/*----------------------------------------------------------------------------
 *
 *	DisplayWeapons()
 *
 */
void DisplayWeapons( CameraInstance *camera )
{
	FPoint rel;

	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
	{
		if ((W->Flags & WEAPON_INUSE) && !(W->Flags & ALREADY_BURST) && (W!=SkipThisWeap))
		{
//			if (!W->SortHeader)
//			{
//				if (W->Flags & BULLET_INUSE)
//					GetSortHeader(W, BULLET_OBJECT);
//				else
//					if (W->Flags & BOMB_INUSE)
//						GetSortHeader(W,BOMB_OBJECT);
//			}

			if ((W->Pos.X >= VisibleNW.X) && (W->Pos.X < VisibleSE.X) && (W->Pos.Z >= VisibleNW.Z) && (W->Pos.Z < VisibleSE.Z))
			{
				rel.MakeVectorToFrom(W->Pos,camera->CameraLocation);

				float radius;

				if (W->Flags & BULLET_INUSE)
					radius = W->InitialVelocity;
				else
					radius = GetObjectRadius(W->Type->Model);

				if (RadiusInView(rel,radius))
					DrawWeapon(W,rel);
			}
		}
	}
}

//============================================================================
//		BOMB CODE  --  INSTANTIATE / DELETE / MOVE
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	InstantiateBomb()
 *
 */
void InstantiateBomb( PlaneParams *P, int orgStation , double burst_height, void *GroundTarget, int groundtype, double GX, double GY, double GZ, int netid)
{
	FPoint x,y,z;
	float weapondrag = 1.0f;
	float gravity = GRAVITY;
	float dx, dz, fdist;
	int dodelay;
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	DBWeaponType *pDBWeapon;
	int Station = (orgStation & ~(0x40));
	int tempStation = Station;
	int allow_retarded = 1;
	WeaponParams *W = NULL;

	if(P == PlayerPlane)
	{
		BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

		if(B->Drag != WEAP_STORES_RETARDED_MODE)
		{
			tempStation |= 0x40;
			allow_retarded = 0;
		}
	}
	else if(orgStation & 0x40)
	{
		allow_retarded = 0;
	}

	if(!(P->Status & PL_ACTIVE))
		return;

	if(P == PlayerPlane)
	{
		if(!AICheckForMoreAG(PlayerPlane, NULL))
		{
			iPlayerBombFlags &= ~WSO_BOMB_HIT;
		}
	}

	if(netid == -1)
		NetCheckStationUpdated(P, Station);

//	if(WCheckWeaponNowPlane(P, AIRCRAFT, GroundTarget, groundtype, targetloc, netid, Station, flightprofile))
	FPoint targetloc;
	targetloc.X = GX;
	targetloc.Y = GY;
	targetloc.Z = GZ;
	if(!Cpit.SimJettOn)
	{
		if((g_bIAmHost) || (netid == -1) || (!MultiPlayer))
		{
			if(WCheckWeaponNowPlane(P, AIRCRAFT, GroundTarget, groundtype, targetloc, netid, Station))
			{
				if((!g_bIAmHost) && (netid == -1) && (MultiPlayer))
				{
					if (MultiPlayer && (P==PlayerPlane || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1)
					{
						netid = NetPutBombGeneral(P, tempStation , burst_height, GroundTarget, groundtype, GX, GY, GZ);
					}
				}
				return;
			}
		}
		else if((!g_bIAmHost) && (netid != -1) && (MultiPlayer))
		{
			pDBWeapon = &pDBWeaponList[ P->WeapLoad[Station].WeapIndex ];
			if(WGetWeaponToPlaneID(pDBWeapon) != -1)
				return;
		}
	}

	//  I think I want this after WCheckWeaponNowPlane since that function will do the same thing as below
	if((WIsWeaponPlane(P)) || (!P->AI.CurrWay))
	{
		W = &Weapons[(int)P->AI.fVarA[1]];
		if(W->WeaponMove == WMovePhantomWeapon)
		{
			netid = W->iNetID;
		}
		else
		{
			W = NULL;
		}
	}

	if(!W)
	{
		if (MultiPlayer && (P==PlayerPlane || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1)
			netid = NetPutBombGeneral(P, tempStation , burst_height, GroundTarget, groundtype, GX, GY, GZ);
		else if((!MultiPlayer) || (netid == -1))  //  Check with Mike.  I think he was incrimenting this for debrief replay.  Screws up MultiPlayer if done this way hence added else condition
			netid = g_iWeaponID++;

		W = GetWeaponSlot();
		if (!W)  return;
	}

	pDBWeapon = &pDBWeaponList[ P->WeapLoad[Station].WeapIndex ];

	AICheckROE(P, 2);
	W->P				= P;
	W->iNetID			= netid;
	W->Kind				= BOMB;
	W->Flags			= BOMB_INUSE;
	W->Flags2			= 0;
 	W->WeaponMove		= MoveBombs;
	W->Type				= P->WeapLoad[Station].Type;
	if(!W->Type)
	{
		W->Type			= (WeaponType	 *)P->WeapLoad[Station].WeapIndex;
	}

	W->GroundHeight		= 0.0f;
	W->iTargetType		= 0;
	W->pTarget			= NULL;
	W->Smoke = NULL;

	W->Launcher			= P;
	W->LauncherType 	= AIRCRAFT;
	if(WIsWeaponPlane(P))
	{
		if(P->AI.CurrWay)
		{
			W->P = P;
			W->Launcher			= P;
			W->LauncherType 	= AIRCRAFT;
		}
		else if(P->AI.iVar1 == AIRCRAFT)
		{
			W->P = (PlaneParams *)P->AI.LinkedPlane;
			W->Launcher			= (PlaneParams *)P->AI.LinkedPlane;
			W->LauncherType 	= P->AI.iVar1;
		}
		else if(P->AI.iVar1 == MOVINGVEHICLE)
		{
			W->Launcher			= (MovingVehicleParams *)P->AI.LinkedPlane;
			W->LauncherType 	= P->AI.iVar1;
		}
		else if(P->AI.iVar1 == GROUNDOBJECT)
		{
			W->Launcher			= (BasicInstance *)P->AI.LinkedPlane;
			W->LauncherType 	= P->AI.iVar1;
		}
	}

	W->lTargetDistFt  = -1;
	W->lBurnTimer = 1500; //make sure baloots and fins don't pop out prematurely.  Give them 1.5 seconds


//	if((P->Status & PL_AI_DRIVEN) || ((pDBWeapon->lWeaponID >= 25) && (pDBWeapon->lWeaponID <= 28)))
	if((P->Status & PL_AI_DRIVEN) || ((pDBWeapon->lWeaponID >= 25) && (pDBWeapon->lWeaponID <= 28)) || (pDBWeapon->lWeaponID == GetWeapId(AGM62_ID) || (pDBWeapon->iSeekerType == 12) || (pDBWeapon->iSeekerType == 4)))
	{
		if(GroundTarget)
		{
			if(groundtype == MOVINGVEHICLE)
			{
				W->iTargetType = TARGET_VEHICLE;
				W->pTarget = GroundTarget;
			}
			else if(groundtype == GROUNDOBJECT)
			{
				W->iTargetType = TARGET_GROUND;
				W->pTarget = GroundTarget;
			}
		}
		else
		{
			W->iTargetType = NONE;
			W->pTarget = NULL;
		}
	}
	else
	{
		W->iTargetType = NONE;
		W->pTarget = NULL;
	}

	if(pDBWeapon->fDrag > 1.0f)
	{
		if(!Cpit.SimJettOn)
		{
		 	W->WeaponMove		= MoveGuidedBombs;
		}
		W->Vel.SetValues( HIGH_FREQ * FTTOWU, P->IfVelocity );
		gravity = W->fGravity = 32.0f / pDBWeapon->fDrag;
		W->fMinGravity = 32.0f / (sqrt(pDBWeapon->fDrag - 0.75f) + pDBWeapon->fDrag);
		W->InitialVelocity	= sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Z * W->Vel.Z));

		W->Heading	= P->Heading;
		W->Pitch	= P->Pitch;

		if((P->Status & PL_AI_DRIVEN) || ((pDBWeapon->lWeaponID >= 25) && (pDBWeapon->lWeaponID <= 28)) || ((pDBWeapon->lWeaponID == GetWeapId(AGM62_ID)) || (pDBWeapon->iSeekerType == 12) || (pDBWeapon->iSeekerType == 4)) )
		{
			W->fpTempWay.X = GX;
			W->fpTempWay.Y = GY;
			W->fpTempWay.Z = GZ;
			if(P->AGDesignate.X != -1)
			{
				W->GroundHeight = W->fpTempWay.Y;
			}
//			else if((P == PlayerPlane) && (!W->pTarget))  //  Use this if you want to be able to direct mode designate a point on the ground.
			if(((P == PlayerPlane) && (!W->pTarget) && (pDBWeapon->iSeekerType != 12)) || (pDBWeapon->lWeaponID == GetWeapId(AGM62_ID)))
			{
				if(Av.Weapons.HasDataLink)
					pCurrentGBU = W;
			}
		}

		dodelay = 0;
		if(GroundTarget)
		{
			if(groundtype == GROUNDOBJECT)
			{
				walker = (BasicInstance *)GroundTarget;
				dx = P->WorldPosition.X - walker->Position.X;
				dz = P->WorldPosition.Z - walker->Position.Z;
			}
			else
			{
				vehiclepnt = (MovingVehicleParams *)GroundTarget;
				dx = P->WorldPosition.X - vehiclepnt->WorldPosition.X;
				dz = P->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
			}
			dodelay = 1;
		}
		else if(GX >= 0)
		{
			dx = P->WorldPosition.X - GX;
			dz = P->WorldPosition.Z - GZ;
			dodelay = 1;
		}

		if(dodelay)
		{
			fdist = QuickDistance(dx, dz) * WUTOFT;
			W->LifeTime = (GetImpactTime(W, W->Vel.Y, NULL) * 1000) / 3;
		}
		else
		{
			W->LifeTime = (GetImpactTime(W, W->Vel.Y, NULL) * 1000) / 3;
		}

		if((pDBWeapon->lWeaponID >= 25) && (pDBWeapon->lWeaponID <= 28))
		{
			if(((W->fpTempWay.X != -1) && (W->fpTempWay.Z != -1)) || (W->pTarget))
			{
				W->Flags |= WEP_DIRECT_LAUNCH;
			}
			W->LifeTime = 1750;
		}
	}
	else
	{
		W->Heading	= P->Heading;
		W->Pitch	= P->Pitch;

		if(!Cpit.SimJettOn)
		{
			weapondrag = pDBWeapon->fDrag;
			if(!allow_retarded)
			{
				if(WIsRetardedWeapon(pDBWeapon->lWeaponID))
				{
					weapondrag = 0.97;
					W->Flags2 |= BOMB_FORCE_FREEFALL;
				}
				else
				{
					weapondrag = pDBWeapon->fDrag;
				}
			}

			W->InitialVelocity	= P->V * weapondrag;
			W->Vel.SetValues( HIGH_FREQ * FTTOWU * weapondrag, P->IfVelocity );
		}
		else
		{
			weapondrag = 0.95f;
			W->InitialVelocity	= P->V * 0.95f;
//			W->Vel.SetValues( HIGH_FREQ * FTTOWU * pDBWeapon->fDrag, P->IfVelocity );
			W->Vel.SetValues( HIGH_FREQ * FTTOWU * weapondrag, P->IfVelocity );
		}

		gravity = W->fGravity = 32.0f;
		if(P->Status & PL_AI_DRIVEN)
		{
			W->GroundHeight = P->AI.TargetPos.Y;
		}
	}

	//Calculate attitude of bomb from it velocity

	z.SetValues(-W->Vel.X,-W->Vel.Y,-W->Vel.Z);
	z.Normalize();

	if (fabs(z.Y) < 0.98f)
		y.SetValues(0.0f,1.0f,0.0f);
	else
		y.SetValues(0.0f,0.0f,1.0f);

	x = y;
	x %= z;
	x.Normalize();

	y = z;
	y %= x;
	y.Normalize();

	W->Attitude.m_Data.RC.R0C0 = x.X;
	W->Attitude.m_Data.RC.R1C0 = x.Y;
	W->Attitude.m_Data.RC.R2C0 = x.Z;

	W->Attitude.m_Data.RC.R0C1 = y.X;
	W->Attitude.m_Data.RC.R1C1 = y.Y;
	W->Attitude.m_Data.RC.R2C1 = y.Z;

	W->Attitude.m_Data.RC.R0C2 = z.X;
	W->Attitude.m_Data.RC.R1C2 = z.Y;
	W->Attitude.m_Data.RC.R2C2 = z.Z;

	W->Pos				= GetHardPointPos( P, Station );
	WeaponInitAltitude(W);

	if(pDBWeapon->iWeaponType != WEAPON_TYPE_AGROCKET)
	{
		if((P == PlayerPlane) && (cPlayerLimitWeapons))
		{
			P->AircraftDryWeight -= pDBWeapon->iWeight;
			P->AI.fStoresWeight -= pDBWeapon->iWeight;
		}
		else if(P->Status & PL_AI_DRIVEN)
		{
			P->AI.fStoresWeight -= pDBWeapon->iWeight;
		}
		else if(cPlayerLimitWeapons)
		{
			P->AircraftDryWeight -= pDBWeapon->iWeight;
			P->AI.fStoresWeight -= pDBWeapon->iWeight;
		}

		if((pSeatData) && (P == PlayerPlane))
		{
			if(Station <= LEFT5_STATION)
			{
				pSeatData->WeaponReleaseSide |= 2;
			}
			else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
			{
				pSeatData->WeaponReleaseSide |= 1;
			}
			else
			{
				pSeatData->WeaponReleaseSide |= 0x3;
			}
			pSeatData->WeaponReleaseWeight += pDBWeapon->iWeight;
			if(Cpit.SimJettOn)
			{
				pSeatData->WeaponReleaseID = -2;
			}
			else
			{
				pSeatData->WeaponReleaseID = pDBWeapon->lWeaponID;
			}
		}
	}

	DBWeaponType *pweapon_type = pDBWeapon;  //  &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];

	if (P == PlayerPlane)
	{
		if((!Cpit.SimJettOn) && ((pweapon_type->iWeaponType == WEAPON_TYPE_GUIDED_BOMB) && (!((lBombFlags & WSO_BOMB_IMPACT) || (P->AI.iAIFlags2 & AI_RIPPLE_BOMB)))))
		{
			double VertVel = P->IfVelocity.Y;  //  Get ImpactTime will take care of weapon drag modifications
//			double VertVel = P->IfVelocity.Y * weapondrag;
//			double VertHgt = P->HeightAboveGround * WUTOFT;

//			double Time = (VertVel + sqrt( VertVel*VertVel + 2*gravity*VertHgt )) / gravity;
			double Time = GetImpactTime(P, VertVel, NULL) + 5.0f;

			lBombFlags |= WSO_BOMB_IMPACT;
			lBombTimer = (long)(Time * 1000);

			if((pweapon_type->iSeekerType == 6 /*laser*/) && AreMpdsInGivenMode(TGT_IR_MODE) && (UFC.MasterMode != AA_MODE))
			{
		  		Av.Flir.TimeToImpact = lBombTimer;
      	  		Av.Flir.TimpactTimer.Set(3.0,GameLoopInTicks);
			}

		}
	}

	if (pweapon_type->iWarHead == WARHEAD_FUEL_AIR_EXPLOSIVE)
	{
		W->Flags |= CHECK_FOR_BURST;
		if(W->WeaponMove == MoveBombs)
		{
			W->fpTempWay.Y = 350 FEET;
		}
		W->lTimer = GameLoop;
	}
	else
		if (pweapon_type->iWarHead == WARHEAD_CRATERING)
		{
			W->Flags |= CHECK_FOR_BURST;
			if(W->WeaponMove == MoveBombs)
			{
				W->fpTempWay.Y = 500 FEET;
			}
			W->lTimer = GameLoop;
		}
		else
			if (pweapon_type->iWeaponType == WEAPON_TYPE_CLUSTER_BOMB)
			{
				W->Flags |= CHECK_FOR_BURST;
				if(W->WeaponMove == MoveBombs)
				{
					W->fpTempWay.Y = burst_height;
				}
				W->lTimer = GameLoop;
			}

	W->SortType = BOMB_OBJECT;
//	GetSortHeader(W,BOMB_OBJECT);

	if(Cpit.SimJettOn)
	{
		W->Flags |= WEP_JETTISONED;
	}

#if 0  //  Was used for WSO speech for weapon jettison.  Jay is using this varible for multiple things now.
	if(Cpit.SimJettOn != 2)
	{
		AIC_WSO_Bomb_Msgs(P, W);
		if(Cpit.SimJettOn == 1)
		{
			Cpit.SimJettOn = 2;
		}
	}
#endif

	if(!WIsWeaponPlane(P))
		LogWeaponInstantiate(P,W);

	PlaneHasAAWeapons(P);
	PlaneHasAGWeapons(P);

	if (P==PlayerPlane)
	{
		int iLevel=g_iSoundLevelExternalSFX-60;
		if (iLevel<0) iLevel=0;

		if (iLevel)
		{
			SndQueueSound(Radthum,1,iLevel);
		}

		if (!(W->Flags & CHECK_FOR_BURST))
		{
			W->Flags |= COLLISION_CHECK;
			W->CollisionCheckTimer = 0;
			W->AccelOrientation = W->Pos;
		}
	}

#ifdef WATCHFALL
	FPointDouble offset;
	FPoint ImpactPoint;
	FPointDouble dImpactPoint;

	setup_no_cockpit_art();

	camera_setup_initial_location(W->Pos);

	GetImpactTime(W, W->Vel.Y, &ImpactPoint);

	dImpactPoint = ImpactPoint;
	offset.MakeVectorToFrom(dImpactPoint,Camera1.CameraLocation);

	Camera1.Roll = 0;
	Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
	Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

	camera_setup_initial_relative_quats();
	Camera1.SubType = 0;
	Camera1.CameraMode = CAMERA_RC_VIEW;

	placeholder = InstantiatePlaceHolder(P->WorldPosition);
	agpoint = InstantiatePlaceHolder(P->WorldPosition);
#endif

//	if (GetJoyB1())
//	{
	/*
		Camera1.CameraMode = CAMERA_FREE;
		LoadCockpit(FRONT_NO_COCKPIT);
		SetupView(FRONT_NO_COCKPIT);

		CockpitSeat = NO_SEAT;
	*/
//	}
}

/*----------------------------------------------------------------------------
 *
 *	DeleteBomb()
 *
 */
void DeleteBomb( WeaponParams *W )
{
	if(pCurrentGBU == W)
	{
		pCurrentGBU = NULL;
	}

	if(lBombFlags & WSO_BOMB_IMPACT)
	{
		DBWeaponType *pweapon_type;
		if ((int)W->Type < 0x200)
		{
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
		}
		else
		{
			pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
		}

	 	if(((W->WeaponMove == MoveGuidedBombs) || (pweapon_type->iSeekerType == 6)) && (W->P == PlayerPlane))
		{
			lBombFlags &= ~(WSO_BOMB_IMPACT|WSO_BOMB_TREL|WSO_BOMB_TPULL|WSO_STEERING_MSG|WSO_NAV_MSGS);
			lBombTimer = -1;
		}
	}

	W->Flags = 0;
	W->Kind = NONE;
	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	}
	W->Smoke = NULL;

//	UnsortObject(W);

	/* ------------------------------------------1/31/98 4:50PM--------------------------------------------
	 * make sure the camera abandons the object when we blow it up
	 * ----------------------------------------------------------------------------------------------------*/
	if (Camera1.AttachedObject == (int *)W)
		SwitchCameraSubjectToPoint(W->Pos);

	if (Camera1.TargetObject == (int *)W)
		SwitchCameraTargetToPoint(W->Pos);

	if(W == (WeaponParams *)pPadLockTarget)
	{
		iPadLockType = TARGET_LOCATION;
		fpdPadLockLocation = W->Pos;
		pPadLockTarget = NULL;
	}

	if(iLastWSOWarnType == TARGET_WEAPON)
	{
		if(W == (WeaponParams *)pLastWSOWarnObj)
		{
			iLastWSOWarnType = NONE;
			pLastWSOWarnObj = NULL;
		}
	}

	if (W == LastWeapon)
		while((LastWeapon > &Weapons[-1]) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;

#ifdef WATCHFALL
	AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
	Camera1.CameraMode = CAMERA_COCKPIT;
	Camera1.SubType = COCKPIT_FRONT;
	LoadCockpit(FRONT_FORWARD_COCKPIT);
	SetupView( Camera1.SubType );
	CockpitSeat = FRONT_SEAT;
	OurShowCursor( TRUE );
	placeholder = NULL;
	agpoint = NULL;
#endif
}

//	if (GetJoyB1())
//	{
	/*
		Camera1.SubType = FRONT_FORWARD_COCKPIT;
		Camera1.CameraMode = CAMERA_COCKPIT;
		LoadCockpit(FRONT_FORWARD_COCKPIT);
		SetupView(FRONT_FORWARD_COCKPIT);
		CockpitSeat = FRONT_SEAT;
	*/
//	}

/*----------------------------------------------------------------------------
 *
 *	MoveBombs()
 *
 */
extern BOOL NoFlashDuringExplosion;

void MoveBombs( WeaponParams *W )
{
	PlaneParams *planepnt;

	if (W->Flags & FUEL_AIR_STAGE2)
	{
		if ((GameLoop - W->lTimer) > 55)  /* about 1 second */
		{
			DBWeaponType *pweapon_type;
			int numspines;
			FPoint tmp(0.0f);
			FPoint offset;
			FPointDouble position;
			FMatrix new_mat;
			float h,p;

			numspines = 8 + g_Settings.gr.nExplosionDetail*3;

			for( int i = 0; i< numspines; i++)
			{
				p = frand()*2.0*PI;
				h = frand()*2.0*PI;

				new_mat.SetRadRPH(0.0f,p,h);
				offset.SetValues(new_mat.m_Data.RC.R0C1,new_mat.m_Data.RC.R1C1,new_mat.m_Data.RC.R2C1);
				offset *= frand()*(10 FEET);

				position = offset;
				position += W->Pos;

				NewCanister( CT_EXPLOSION_FIRE , W->Pos, tmp, 350.0f, 0.0f, &new_mat);
			}


			if ((int)W->Type < 0x200)
				pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
			else
				pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];

			NewCanister(CT_FLASH,W->Pos,tmp,(float)pweapon_type->iDamageValue/100.0f);

			W->Pos.Y = LandHeight(W->Pos.X,W->Pos.Z);

			NoFlashDuringExplosion = TRUE;
			DoExplosionOnGround(W);
			NoFlashDuringExplosion = FALSE;

			DeleteBomb(W);
		}
	}
	else
	{
		if (W->lBurnTimer >= 0)
		{
			W->lBurnTimer -= 20;
			if (W->lBurnTimer < 0)
				W->lBurnTimer = -1;
		}

		W->LastPos = W->Pos;
		W->Pos    += W->Vel;
		W->Pos.Y  -= G_TIX/2;
		if (!(W->Flags & DURANDEL_STAGE2))
			W->Vel.Y  -= G_TIX;
		else
		{
			W->Vel.Y += ((-250.0 METERS / 50.0f) - W->Vel.Y)*0.05;
			if (W->Smoke)  ((SmokeTrail *) W->Smoke )->Update();
		}

	#ifdef WATCHFALL
		FPointDouble relative_position;
		FPoint ImpactPoint;
		double viewpitch, viewheading;

		Camera1.CameraLocation.X = W->Pos.X;
		Camera1.CameraLocation.Y = W->Pos.Y;
		Camera1.CameraLocation.Z = W->Pos.Z;

		GetImpactTime(W, W->Vel.Y, &ImpactPoint);

		relative_position.MakeVectorToFrom(ImpactPoint,Camera1.CameraLocation);
		relative_position.Normalize();

		viewheading = (atan2(relative_position.X,relative_position.Z) * 57.2958);
		Camera1.Heading = AIConvert180DegreeToAngle(viewheading) + 0x8000;
		viewpitch = (asin(relative_position.Y) * 57.2958);
		Camera1.Pitch = AIConvert180DegreeToAngle(viewpitch);
		if(placeholder)
		{
			placeholder->Pos = ImpactPoint;
			placeholder->LifeTime = 2000;
			if(agpoint)
			{
				agpoint->LifeTime = 2000;
			}
		}
	#endif

		if(pPadLockTarget == NULL)
		{
			W->Flags &= ~WEP_PADLOCKED;
		}

		planepnt = WeaponHitsPlane(W);
		if(planepnt)
		{
			/* ------------------------------------------2/11/98 10:14AM-------------------------------------------
		 	* Oh My god we hit something airborn!!!
		 	* ----------------------------------------------------------------------------------------------------*/
			DoExplosionInAir(W, 0, NULL, NULL, planepnt);
			if (Camera1.AttachedObject == (int *)W)
		 		AssignCameraSubject(planepnt,CO_PLANE);
			DeleteBomb(W);
		}
		else
		{
			if (!WeaponIntersectsGround(W))
			{
				if(!(W->Flags & WEP_JETTISONED) && !CheckForAirBurst(W))
				{
					if ((W->Flags & COLLISION_CHECK) && (W->Altitude < (500.0 FEET)) && CheckMissileAgainstGroundTargets(W))
						DeleteBomb(W);
				}
			}
			else
			{
				DBWeaponType *pDBWeapon;
				if ((int)W->Type < 0x200)
				{
					pDBWeapon = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
				}
				else
				{
					pDBWeapon = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
				}

				if((pDBWeapon->iSeekerType == 13) && (pDBWeapon->iWeaponType == WEAPON_TYPE_DUMB_BOMB))
				{
			 		W->WeaponMove = MoveTorpedo;
					W->LifeTime = 0;
					W->InitialVelocity	= sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Y * W->Vel.Y) + (W->Vel.Z * W->Vel.Z));

					for(int cnt = 0; cnt < iNumWeaponList; cnt ++)
					{
						pDBWeapon = &pDBWeaponList[cnt];
						if((pDBWeapon->iWeaponType == WEAPON_TYPE_GUN) && (pDBWeapon->iDamageRadius <= 10))
						{
							FPointDouble position = W->Pos;

							NewExplosionWeapon( pDBWeapon, position);
						}
					}
					return;
				}

				if (CanDoGroundExplosion(W) && !CheckMissileAgainstGroundTargets(W))
					DoExplosionOnGround(W);

				DeleteBomb(W);
			}
		}
	}
}

#ifdef WATCHFALL
FPointDouble gImpactPoint;
#endif
/*----------------------------------------------------------------------------
 *
 *	MoveGuidedBombs()
 *
 */
void MoveGuidedBombs( WeaponParams *W )
{
	PlaneParams *planepnt;

	if (W->lBurnTimer >= 0)
	{
		W->lBurnTimer -= 20;
		if (W->lBurnTimer < 0)
			W->lBurnTimer = -1;
	}

	W->LastPos = W->Pos;
	W->Pos    += W->Vel;
	W->Pos.Y  -= (W->fGravity*FTTOWU/2500.0f)/2;
	W->Vel.Y  -= (W->fGravity*FTTOWU/2500.0f);

#ifdef WATCHFALL
	if (W->Altitude < (W->Vel.Y*-100.0))
	{
		Camera1.CameraMode = CAMERA_FREE;
	}
	else
	{
		FPointDouble relative_position;
		FPoint ImpactPoint;
		void *tempptr;

		camera_setup_initial_location(W->Pos);

		GetImpactTime(W, W->Vel.Y, &ImpactPoint);

		gImpactPoint = ImpactPoint;
		Camera1.TargetObjectType = CO_POINT;
		tempptr = &gImpactPoint;
		Camera1.TargetObject = (int *)tempptr;
		Camera1.AttachedObjectType = CO_POINT;
		tempptr = &gImpactPoint;
		Camera1.AttachedObject = (int *)tempptr;

		relative_position.MakeVectorToFrom(ImpactPoint,Camera1.CameraLocation);
		relative_position.Normalize();

		Camera1.Heading = AIConvert180DegreeToAngle(atan2(relative_position.X,relative_position.Z) * 57.2958) + 0x8000;
		Camera1.Pitch = AIConvert180DegreeToAngle(asin(relative_position.Y) * 57.2958);
		if(placeholder)
		{
			placeholder->Pos = ImpactPoint;
			placeholder->LifeTime = 2000;
		}
	}
#endif

	GuideBomb(W, W->fpTempWay);

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	planepnt = WeaponHitsPlane(W);
	if(planepnt)
	{
		DoExplosionInAir(W, 0, NULL, NULL, planepnt);
		DeleteBomb(W);
	}
	else
	{
		if (!WeaponIntersectsGround(W))
			CheckForAirBurst(W);
		else
		{
			if (CanDoGroundExplosion(W))
				DoExplosionOnGround(W);

			DeleteBomb(W);
		}
	}
}

//	if (GetJoyB1())
//	{
	/*
		Camera1.CameraLocation.X = W->Pos.X;
		Camera1.CameraLocation.Y = W->Pos.Y;
		Camera1.CameraLocation.Z = W->Pos.Z;
	*/
//	}

/*----------------------------------------------------------------------------
 *
 *	GuideBomb()
 *
 */
void GuideBomb(WeaponParams *W, FPointDouble AGDesignate)
{
	double heading;
	double dheading;
	double turnrate;
	DBWeaponType *pDBWeapon;
	float dx, dy, dz;
	double fdist;
	double vel;
	float Time;
	float currTime;
	double VertVel;
	double desiredgravity;
	double weaponheight = (W->Pos.Y - LandHeight(AGDesignate.X, AGDesignate.Z)) * WUTOFT;
	double maxgravity = GRAVITY * 2.0f; //    / 0.75;
	double gravitymod = 0.6f;  //    0.2f;
	int dostandard;
	FPointDouble position;
	PlaneParams *planepnt;
	MovingVehicleParams *vehiclepnt;
	BasicInstance *walker;

	if(!W)
	{
		return;
	}

	if(W->LifeTime >= 0)
	{
		W->LifeTime -= 20;
		return;
	}

	if ((int)W->Type < 0x200)
	{
		pDBWeapon = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pDBWeapon = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	if(!pDBWeapon)
	{
		return;;
	}

	if(W->pTarget)
	{
		if(W->iTargetType == TARGET_PLANE)
		{
			planepnt = (PlaneParams *)W->pTarget;
			position = planepnt->WorldPosition;
			weaponheight = (W->Pos.Y - position.Y) * WUTOFT;
		}
		else if(W->iTargetType == TARGET_VEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)W->pTarget;
			position = vehiclepnt->WorldPosition;
			weaponheight = (W->Pos.Y - position.Y) * WUTOFT;
		}
		else
		{
			walker = (BasicInstance *)W->pTarget;
			position = walker->Position;
			weaponheight = (W->Pos.Y - position.Y) * WUTOFT;
		}
	}
	else
	{
		position = W->fpTempWay;
		weaponheight = (W->Pos.Y - position.Y) * WUTOFT;
	}

	if((pDBWeapon->lWeaponID >= 25) && (pDBWeapon->lWeaponID <= 28) && (!(W->Flags & WEP_DIRECT_LAUNCH)))
	{
		if(IsGbuSteerable())
		{
			if((W->Flags|lBombFlags) & (WSO_TURN_BOMB_L|WSO_TURN_BOMB_R|WSO_BOMB_LONGER|WSO_BOMB_SHORTER))
			{
				W->fpTempWay.X = -1;
				W->fpTempWay.Z = -1;
				position.X = -1;
				position.Z = -1;
				W->pTarget = NULL;
				W->iTargetType = -1;
			}

			if ((position.X == -1) || (position.Z == -1))
			{
				WSOControlWeapon(W, pDBWeapon);
			}
			return;
		}
		else if(FALSE) // (GbuInfo.LaunchMode == GBU_TRANS)
		{
			lBombFlags &= ~(WSO_BOMB_LONGER|WSO_BOMB_SHORTER);

			if(lBombFlags & (WSO_TURN_BOMB_L|WSO_TURN_BOMB_R))
			{
				WSOControlWeapon(W, pDBWeapon);
			}
		}
	}
	else if((pDBWeapon->lWeaponID < 25) || (pDBWeapon->lWeaponID > 28))
	{
		if((W->P == PlayerPlane) && (pDBWeapon->iSeekerType != 12) && (pDBWeapon->iSeekerType != 4))
		{
			if((lLaserStatusFlags & WSO_LASER_MASKED) || (!(lLaserStatusFlags & WSO_LASER_IS_ON)))
			{
				if(!WFoundOtherLaser(W, pDBWeapon->iSeekerType, &position))
				{
					return;
				}
			}
			else
			{
				position = LaserLoc;
				if((LaserLoc.X <= 0) && (LaserLoc.Y <= 0) && (LaserLoc.Z <= 0))
				{
					FPointDouble FlirPos;
					if( GetFlirTargetPosition(&FlirPos) )
					{
						position = FlirPos;
					}
					else if(!WFoundOtherLaser(W, pDBWeapon->iSeekerType, &position))
					{
						return;
					}
				}
				weaponheight = (W->Pos.Y - position.Y) * WUTOFT;
			}
		}
	}

	if ((position.X == -1) || (position.Z == -1))
	{
		return;
	}

	dx = position.X - W->Pos.X;
	dy = position.Y - W->Pos.Y;
	dz = position.Z - W->Pos.Z;

	heading = atan2(-dx, -dz) * 57.2958;

	dheading = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(heading) - W->Heading);

	if(fabs(dheading) > 60)
	{
		return;
	}

	turnrate = pDBWeapon->iMaxYawRate / 50.0;

	if((fabs(dheading) * 2) > turnrate)
	{
		if(dheading < 0)
		{
			W->Heading -= AIConvert180DegreeToAngle(turnrate);
		}
		else
		{
			W->Heading += AIConvert180DegreeToAngle(turnrate);
		}
#if 1
		double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
	}

	fdist = sqrt((dx * dx) + (dz * dz));
	vel = sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Z * W->Vel.Z)) * 50;

	Time = (fdist / vel);
	VertVel = W->Vel.Y * 50 * WUTOFT;

	desiredgravity = GetDesiredGravity(Time, VertVel, weaponheight);

	if(desiredgravity > maxgravity)
	{
		desiredgravity = maxgravity;
	}
	else if(desiredgravity < W->fMinGravity)
	{
		desiredgravity = W->fMinGravity;
	}

	dostandard = 1;
	if((Time > 10.0f) && ((pDBWeapon->lWeaponID < 25) || (pDBWeapon->lWeaponID > 28)))
	{
		currTime = GetImpactTime(W, W->Vel.Y, NULL);

		if(fabs(currTime - Time) < (fabs(Time * 0.05f)))
		{
			if(W->Flags & WEP_PITCH_UP)
			{
				W->fGravity -= gravitymod;
			}
			else
			{
				W->fGravity += gravitymod;
			}
			dostandard = 0;
		}
	}

	if(((fabs(desiredgravity - W->fGravity) * 2) > gravitymod) && (dostandard))
	{
		if(desiredgravity < W->fGravity)
		{
			W->fGravity -= gravitymod;
			W->Flags |= WEP_PITCH_UP;
		}
		else
		{
			W->fGravity += gravitymod;
			W->Flags &= ~WEP_PITCH_UP;
		}
	}

#ifdef WATCHFALL
	if(agpoint)
	{
		agpoint->Pos = position;
		agpoint->Pos.Y += 50.0f * FTTOWU;
		agpoint->LifeTime = 2000;
	}
#endif
}

/*----------------------------------------------------------------------------
 *
 *	WSOControlWeapon()
 *
 */
void WSOControlWeapon(WeaponParams *W, DBWeaponType *pDBWeapon)
{
	float turnrate = pDBWeapon->iMaxYawRate / 50.0;
	double maxgravity = GRAVITY * 2.0f;  //     / 0.75;
	float desiredgravity;
	double gravitymod = 0.6f;  //    0.2f;

	if(lBombFlags & WSO_TURN_BOMB_R)
	{
		W->Heading -= AIConvert180DegreeToAngle(turnrate);
	}
	else if(lBombFlags & WSO_TURN_BOMB_L)
	{
		W->Heading += AIConvert180DegreeToAngle(turnrate);
	}

	desiredgravity = W->fGravity;

	if(lBombFlags & WSO_BOMB_LONGER)
	{
		desiredgravity -= gravitymod;
	}
	else if(lBombFlags & WSO_BOMB_SHORTER)
	{
		desiredgravity += gravitymod;
	}

	if(desiredgravity > maxgravity)
	{
		desiredgravity = maxgravity;
	}
	else if(desiredgravity < W->fMinGravity)
	{
		desiredgravity = W->fMinGravity;
	}

#if 0
	char weapstr[256];

	sprintf(weapstr, "DesiredGravity : %2.2f", desiredgravity);

	int fontheight;
	int fontcolor = HUDColorChoices[MAX_HUD_COLOR - 5];
	TXTWriteStringInBox(weapstr, GrBuffFor3D, MessageFont, 320, 10, 100, fontheight, fontcolor, 0, NULL, 1, 0);
#endif

	W->fGravity = desiredgravity;

#if 1
	double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
}

/*----------------------------------------------------------------------------
 *
 *	GetDesiredGravity()
 *
 */
double GetDesiredGravity(float Time, double VertVel, double weaponheight)
{
	double gravity;

	gravity = (2 * (weaponheight + (Time * VertVel))) / (Time * Time);

	return(gravity);
}

//*******************************************************************************************************************
//*
//*	SetGroundTarget - For things like setting GBU target once weapon is in flight;
//*
//*******************************************************************************************************************
void SetGroundTarget(WeaponParams *W, void *GroundTarget, FPoint targetloc, int groundtype)
{
	AAWeaponInstance *gweapon = NULL;
	int targtype;
	void *targetpnt;

	targtype = TARGET_GROUND;

	if (GroundTarget)
	{
		if(groundtype == AIRCRAFT)
		{
			targtype = TARGET_PLANE;
			targetpnt = GroundTarget;
			W->iTargetType = targtype;
			W->pTarget = targetpnt;
			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, ((PlaneParams *)GroundTarget - Planes) + 1, W);
			}
		}
		else
		{
			if(groundtype == MOVINGVEHICLE)
			{
				targtype = TARGET_VEHICLE;
			}
			targetpnt = GroundTarget;
			W->iTargetType = targtype;
			W->pTarget = targetpnt;
		}
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = -1;
		W->LifeTime = -1;
	}
	else if (targetloc.X >= 0)
	{
		targetpnt = NULL;
		W->iTargetType = targtype;
		W->pTarget = targetpnt;
		W->fpTempWay = targetloc;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = -1;
		W->LifeTime = -1;
	}
	else
	{
		W->iTargetType = NONE;
		W->pTarget = NULL;
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE;
		W->lTimer = -1;
		W->LifeTime = -1;
	}
}

//============================================================================
//		MISSILE CODE  --  INSTANTIATE / DELETE / MOVE
//============================================================================

extern void GroundLaunchMissileSound(DBWeaponType *pWeapon,FPointDouble position);

/*----------------------------------------------------------------------------
 *
 *	InstantiateMissile()
 *
 */
void InstantiateMissile( PlaneParams *P, int orgStation, void *GroundLauncher, int sitetype, PlaneParams *target, int netid, int radardatnum)
{
	PlaneParams  *pTarget;
	DBWeaponType *pDBWeapon=NULL;
	double ybuffer = 0.0f;
	float planeroll;
	double sinroll, cosroll, aoadeg, aoaoffset;
	FPointDouble weaponoffset;
	float wingtipoffset;
	PlaneParams *temptarget;
	int Station = (orgStation & ~(0x40));
	int tempStation;
	int cnt;
#ifdef SRE_SAM_CHECK
	char tempstr[80];
#endif

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	W->LaunchStation = -1;
	W->Flags2 = 0;
	if (P)
	{		//	MultiPlayer Remote Procedure Call

		if(!(P->Status & PL_ACTIVE))
			return;

		if(netid == -1)
			NetCheckStationUpdated(P, Station);

		int NetPutMissile( PlaneParams *P, int station, PlaneParams *pTarget );

		pDBWeapon = &pDBWeaponList[ P->WeapLoad[Station].WeapIndex ];

		if(MultiPlayer)
		{
			if(pDBWeapon && ((pDBWeapon->iSeekerType == 1) || (pDBWeapon->iSeekerType == 7)))
			{
				P->AI.iAIFlags2 |= AI_CHECK_MISSILES;
			}
		}

		if(P == PlayerPlane)
		{
			if((pDBWeapon->iSeekerType == 1) || (pDBWeapon->iSeekerType == 7))
				fLast_Rmax = GetMissileRMaxNM( pDBWeapon, P->Altitude*FTTOWU );

			lShowLostTimer = 5000;

			if(g_dwAIMSndHandle)
				KillSound( &g_dwAIMSndHandle );
			if(g_dwAIMLockSndHandle)
				KillSound( &g_dwAIMLockSndHandle );

			if(pDBWeapon->lWeaponID == 109)
			{
//				if((!target) && (!P->AADesignate))
				if((!target) && (!AA_SCAN))  //  (!AA_CAGE))
				{
					if(((Camera1.SubType & (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) == (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK)) && (Camera1.TargetObjectType == CO_PLANE) && (Camera1.TargetObject))
					{
						target = (PlaneParams *)Camera1.TargetObject;
						if(WCheckIRAspect(pDBWeapon, P, target) <= 0)
						{
							target = NULL;
						}
					}
				}
			}
			if(((target == NULL) || (!AA_SCAN)) && (pDBWeapon->iSeekerType == 2) && (AA_lockon))
			{
				target = AA_lockon;
			}
		}

		if (MultiPlayer && (P==PlayerPlane || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1)
		{
			if(((WeapStores.Aim120VisualOn) && (pDBWeapon->lWeaponID == 38)) && (P == PlayerPlane))
			{
				temptarget = (target) ? target : NULL;
			}
			else
			{
				temptarget = (target) ? target : P->AADesignate;
			}

			tempStation = Station;
			if((P == PlayerPlane) && (WeapStores.Aim7LoftOn) && (pDBWeapon->iSeekerType == 7))
			{
				tempStation |= 0x40;
			}
			netid = NetPutMissile( P, tempStation, temptarget );
		}
		else if((!MultiPlayer) || (netid == -1))  //  Check with Mike.  I think he was incrimenting this for debrief replay.  Screws up MultiPlayer if done this way hence added else condition
			netid = g_iWeaponID++;

		AICheckROE(P, 2);
		W->P = P;
	    W->Type = P->WeapLoad[Station].Type;

		if(pDBWeapon->iWeaponType != WEAPON_TYPE_AGROCKET)
		{
			if((P == PlayerPlane) && (cPlayerLimitWeapons))
			{
				P->AircraftDryWeight -= pDBWeapon->iWeight;
				P->AI.fStoresWeight -= pDBWeapon->iWeight;
			}
			else if(P->Status & PL_AI_DRIVEN)
			{
				P->AI.fStoresWeight -= pDBWeapon->iWeight;
			}
			else if(cPlayerLimitWeapons)
			{
				P->AircraftDryWeight -= pDBWeapon->iWeight;
				P->AI.fStoresWeight -= pDBWeapon->iWeight;
			}

			if((pSeatData) && (P == PlayerPlane))
			{
				if(Station <= LEFT5_STATION)
				{
					pSeatData->WeaponReleaseSide |= 2;
				}
				else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
				{
					pSeatData->WeaponReleaseSide |= 1;
				}
				else
				{
					pSeatData->WeaponReleaseSide |= 0x3;
				}
				pSeatData->WeaponReleaseWeight += pDBWeapon->iWeight;
				pSeatData->WeaponReleaseID = pDBWeapon->lWeaponID;
			}
		}
		else
		{
			float weight;
			if(pDBWeapon->lWeaponID == 133)
			{
				weight = 131;
			}
			else
			{
				weight = 21;
			}
			if((P == PlayerPlane) && (cPlayerLimitWeapons))
			{
				P->AircraftDryWeight -= weight;
				P->AI.fStoresWeight -= weight;
			}
			else if(P->Status & PL_AI_DRIVEN)
			{
				P->AI.fStoresWeight -= weight;
			}
			else if(cPlayerLimitWeapons)
			{
				P->AircraftDryWeight -= weight;
				P->AI.fStoresWeight -= weight;
			}

			if((pSeatData) && (P == PlayerPlane))
			{
				if(Station <= LEFT5_STATION)
				{
					pSeatData->WeaponReleaseSide |= 2;
				}
				else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
				{
					pSeatData->WeaponReleaseSide |= 1;
				}
				else
				{
					pSeatData->WeaponReleaseSide |= 0x3;
				}
				pSeatData->WeaponReleaseWeight += weight;
				pSeatData->WeaponReleaseID = pDBWeapon->lWeaponID;
			}
		}

//		W->InitialVelocity = MachToFtSec(P, 2.5)/(WUTOFT*50.0);
		W->InitialVelocity = P->V/(WUTOFT*50.0);

		W->Attitude = P->Attitude;
		W->Heading	= P->Heading;
		W->Pitch	= P->Pitch;

		//  Start stuff to make missiles go down
		if((P == PlayerPlane) || (P->AI.iAIFlags2 & AI_FAKE_AOA))
		{
			planeroll = AIConvertAngleTo180Degree(P->Roll);
			aoadeg = RadToDeg(P->Alpha);
			if(aoadeg > 5.0)
			{
				aoadeg = 5.0;
			}
			if(aoadeg < -5.0)
			{
				aoadeg = -5.0;
			}
			cosroll = cos(DegToRad(fabs(planeroll)));
			aoaoffset = cosroll * (double)aoadeg;
			W->Pitch -= AIConvert180DegreeToAngle(aoaoffset);

			sinroll = sin(DegToRad(fabs(planeroll)));
			aoaoffset = sinroll * (double)aoadeg;
			W->Heading -= AIConvert180DegreeToAngle(aoaoffset);

			W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );

		}
		//  End stuff to make missiles go down

		W->Vel.X = P->Orientation.I.X * W->InitialVelocity;
		W->Vel.Y = P->Orientation.I.Y * W->InitialVelocity;
		W->Vel.Z = P->Orientation.I.Z * W->InitialVelocity;

		W->Pos = GetHardPointPos( P, Station );

		if(P->AI.lPlaneID == 61)
		{
			wingtipoffset = pDBAircraftList[P->AI.iPlaneIndex].OffSet[OFFSET_WING_RIGHT].X*FOOT_MULTER;
			if(Station == LEFT1_STATION)
			{
				W->Pos.X -= P->Orientation.J.X * wingtipoffset;
				W->Pos.Y -= P->Orientation.J.Y * wingtipoffset;
				W->Pos.Z -= P->Orientation.J.Z * wingtipoffset;
			}
			else if(Station == RIGHT11_STATION)
			{
				W->Pos.X += P->Orientation.J.X * wingtipoffset;
				W->Pos.Y += P->Orientation.J.Y * wingtipoffset;
				W->Pos.Z += P->Orientation.J.Z * wingtipoffset;
			}
		}

		W->Launcher		= P;
		W->LauncherType = AIRCRAFT;

		if(target)
		{
			pTarget	= target;
		}
		else
		{
//			pTarget	= P->AADesignate;
			if(((WeapStores.Aim120VisualOn) && (pDBWeapon->lWeaponID == 38)) && (P == PlayerPlane))
			{
				pTarget	= NULL;
			}
			else
				if (P->AADesignate)
				{
					int old_prim_id = Primary->Id;
					Primary->Id = P->AADesignate - Planes;

					if ((Primary->Id >= 0) && (Primary->Id < MAX_PLANES))
					{
						if ((pDBWeapon->iSeekerType == 7) || (pDBWeapon->iSeekerType == 1) || !PrimaryIsAOT(&CurFramePlanes))
							pTarget	= P->AADesignate;
						else
							pTarget	= NULL;
					}
					else
						pTarget	= P->AADesignate;

					Primary->Id = old_prim_id;
				}
				else
					pTarget = NULL;

		}

		LogWeaponInstantiate(P,W);

		PlaneHasAAWeapons(P);
		PlaneHasAGWeapons(P);

		if (P==PlayerPlane)
		{
			int iLevel=g_iSoundLevelExternalSFX-60;
			if (iLevel<0) iLevel=0;

			if (iLevel)
			{
				SndQueueSound(Radthum,1,iLevel);
			}
		}
	}
	else if (GroundLauncher)
	{
		AAWeaponInstance  *gweapon;
		GDRadarData		  *radardat;
		MovingVehicleParams *vehiclepnt;
		FPointDouble tPos;
		double tdist, offangle, offpitch;
		ybuffer = 6.0f;  //  50.0f;

		W->P = NULL;
		W->Launcher = GroundLauncher;

		if(sitetype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)GroundLauncher;
			W->LauncherType = MOVINGVEHICLE;
			radardat = &vehiclepnt->RadarWeaponData[radardatnum];
			W->LaunchStation = radardatnum;

			if (!vehiclepnt->iShipType)	// SCOTT FIX
			{
				if(radardatnum == 0)
				{
					pDBWeapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
				}
				else
				{
					pDBWeapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
				}
			}
			else
			{
				pDBWeapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID);
			}

			if(target)
			{
				pTarget	= target;
				radardat->Target = pTarget;
			}
			else
			{
				pTarget  = (PlaneParams *)radardat->Target;
			}

			if (!pTarget)
			{
				return;
			}

			if (MultiPlayer && (lAINetFlags1 & NGAI_ACTIVE) && netid==-1)
			{
//				netid = NetPutMissileGeneral(P, Station, GroundLauncher, sitetype, pTarget);
				netid = NetPutMissileGeneral(P, radardatnum, GroundLauncher, sitetype, pTarget);
			}
			else if(MultiPlayer && netid != -1)
			{
				radardat->Target = pTarget;
				radardat->lWFlags1 &= ~(GD_W_ACQUIRING);
				radardat->lWFlags1 |= (GD_W_FIRING);
				radardat->lWNumberFired = radardat->lWNumberFired + 1;
				if(radardat->lWNumberFired >= pDBWeapon->iBurstRate)
				{
					radardat->lWReloadTimer = pDBWeapon->iRateOfFire * 1000;
				}
			}
			else if(netid == -1)
			{
				netid = g_iWeaponID++;
			}

			tPos  = pTarget->WorldPosition;
			tPos -= vehiclepnt->WorldPosition;

			W->Pos.X = vehiclepnt->WorldPosition.X;
			W->Pos.Y = vehiclepnt->WorldPosition.Y;
			W->Pos.Z = vehiclepnt->WorldPosition.Z;

			VGDGetWeaponPosition(GroundLauncher, sitetype, radardatnum, &weaponoffset);
			W->Pos.X = W->Pos.X + weaponoffset.X;
			W->Pos.Y = W->Pos.Y + weaponoffset.Y;
			W->Pos.Z = W->Pos.Z + weaponoffset.Z;
		}
		else
		{
			gweapon  = (AAWeaponInstance *)GroundLauncher;
			radardat = (GDRadarData *)gweapon->AIDataBuf;
			pDBWeapon = &pDBWeaponList[ radardat->iWeaponIndex ];
			W->LauncherType = GROUNDOBJECT;

			if(target)
			{
				pTarget	= target;
			}
			else
			{
				pTarget  = (PlaneParams *)radardat->Target;
			}
			if (!pTarget)  return;

#ifdef SRE_SAM_CHECK
	sprintf(tempstr, "Missile %d Launched at %d", W - Weapons, (PlaneParams *)pTarget - Planes);
	AICAddAIRadioMsgs(tempstr, 50);
#endif

			if (MultiPlayer && (lAINetFlags1 & NGAI_ACTIVE) && netid==-1)
				netid = NetPutMissileGeneral(P, Station, GroundLauncher, sitetype, pTarget);
			else if(MultiPlayer && netid!=-1)
			{
				radardat->Target = pTarget;
				radardat->lWFlags1 &= ~(GD_W_ACQUIRING);
				radardat->lWFlags1 |= (GD_W_FIRING);
				radardat->lWNumberFired = radardat->lWNumberFired + 1;
				if(radardat->lWNumberFired >= pDBWeapon->iBurstRate)
				{
					radardat->lWReloadTimer = pDBWeapon->iRateOfFire * 60000;
				}
			}
			else if(netid == -1)
			{
				netid = g_iWeaponID++;
			}

			tPos  = pTarget->WorldPosition;
			tPos -= gweapon->Basics.Position;

			W->Pos.X = gweapon->Basics.Position.X;
			W->Pos.Y = gweapon->Basics.Position.Y;
			W->Pos.Z = gweapon->Basics.Position.Z;
		}

		GroundLaunchMissileSound(pDBWeapon,W->Pos);

		W->Type = radardat->WType;
		if(!W->Type)
		{
			for(cnt = 0; cnt < MAX_WEAPON_TYPES; cnt ++)
			{
				if(WeaponTypes[cnt].TypeNumber == pDBWeapon->lWeaponID)
				{
					W->Type = &WeaponTypes[cnt];
					break;
				}
			}
			if(cnt == MAX_WEAPON_TYPES)
			{
				DeleteMissile(W);
				return;
			}
		}

//		W->InitialVelocity = MachAToFtSec( GroundLauncher->Position.Y * WUTOFT, 2.5) / (WUTOFT*50.0);
		W->InitialVelocity = 0;

		offangle = atan2(-tPos.X, -tPos.Z) * RADIANS_TO_DEGREES;

		W->Heading = AIConvert180DegreeToAngle(offangle);

		tdist = QuickDistance(tPos.X,tPos.Z);

		if(pDBWeapon->iVerticalLaunch)
		{
			W->Pitch = AIConvert180DegreeToAngle(89.0f);
			W->Flags2 |= MISSILE_VERTICAL_PHASE;
		}
		else
		{
			offpitch = atan2(tPos.Y, tdist) * RADIANS_TO_DEGREES;

			W->Pitch = AIConvert180DegreeToAngle(offpitch);
		}

		W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );

#if 0
	if(Camera1.CameraMode != CAMERA_TRACKING)
	{
		Camera1.AttachedObject = (int *)pTarget;

		Camera1.CameraMode = CAMERA_TRACKING;
		LoadCockpit(FRONT_NO_COCKPIT);
		SetupView(COCKPIT_FRONT|COCKPIT_NOART);

		CockpitSeat = NO_SEAT;
		Camera1.CameraLocation = W->Pos;

		SimPause ^= 1;
		if (!SimPause)
			JustLeftPausedState = 1;
	}
#endif



#if 1
		double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
		W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);

#if 0
		if (pTarget == PlayerPlane)
		{
			setup_no_cockpit_art();
			AssignCameraSubject((void *)W,CO_WEAPON);
			if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
			{
				setup_no_cockpit_art();
				Camera1.CameraMode = CAMERA_FIXED_CHASE;
				ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
			}
			else
	 			ChangeViewModeOrSubject(Camera1.CameraMode);
		}
#endif

	}
	else  return;	// P==NULL && GroundLauncher==NULL

	WeaponInitAltitude(W);
	if(pTarget)
	{
		if(iAI_ROE[pTarget->AI.iSide] < 2)
		{
			iAI_ROE[pTarget->AI.iSide] = 2;
		}
	}

	 // Valid missile, allocate weapon slot
	//
	W->iNetID			= netid;
	W->Flags	  = MISSILE_INUSE;

	W->Kind		  = MISSILE;
	W->WeaponMove = MoveMissile;
	W->GroundHeight		= 0.0f;

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	if (pTarget)
	{
		W->pTarget		= pTarget;
		W->iTargetType	= TARGET_PLANE;
		W->Flags	   |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer		= 0;
		W->LifeTime		= 0;
		if((pDBWeapon->iSeekerType == 2) || (pDBWeapon->iSeekerType == 3))
		{
			LockHeaterOnEngine(pTarget, W);
		}
		else
			if(((pDBWeapon->iSeekerType == 7) || (pDBWeapon->iSeekerType == 1)) && (PrimaryIsAOT(&CurFramePlanes)))
				W->Flags2 |= MISSILE_HOME_ON_JAM;
	}
	else
	{
		W->pTarget		= NULL;
		W->iTargetType	= NONE;
		W->LifeTime		= 0;

		switch (pDBWeapon->iSeekerType)
		{
			case 1:
				W->Flags |= BOOST_PHASE|ACTIVE_SEARCH;
				W->lTimer = 2000;
				break;

			case 7:
				if(W->P == PlayerPlane)
				{
					W->Flags |= BOOST_PHASE|ACTIVE_SEARCH;
					W->lTimer = 2000;
				}
				else
				{
					W->Flags |= BOOST_PHASE;
					W->lTimer = 2000;
				}
				break;

			case 2:
			case 3:
				W->Flags |= BOOST_PHASE|ACTIVE_SEARCH|ACTIVE_SEEKER;
				W->lTimer = 0;
				break;
			default:
				W->Flags |= BOOST_PHASE;
				W->lTimer = 2000;
				break;
		}
	}

	AIC_WSO_Fox_Msgs(P, W);

	W->fpTempWay.X = -1;

	if((pDBWeapon->lWeaponID == 34) ||  (pDBWeapon->lWeaponID == 35) || (pDBWeapon->lWeaponID == 94) || (pDBWeapon->lWeaponID == 108) || (pDBWeapon->lWeaponID == 156) || (pDBWeapon->lWeaponID == 157) || (pDBWeapon->lWeaponID == 158) || (pDBWeapon->lWeaponID == 159))
	{
		float downvel = W_DROP_SPEED/(WUTOFT*50.0);

		if((pDBWeapon->lWeaponID == 94) || (pDBWeapon->lWeaponID == 159))
		{
			downvel *= 2.0f;
		}

		W->Vel.X *= 0.99f;
		W->Vel.Y *= 0.99f;
		W->Vel.Z *= 0.99f;

		W->Vel.X += P->Orientation.K.X * downvel;
		W->Vel.Y += P->Orientation.K.Y * downvel;
		W->Vel.Z += P->Orientation.K.Z * downvel;
		W->Flags2 |= MISSILE_DROP_PHASE;
		W->lTimer += 1000;
	}

	if(((P == PlayerPlane) && (WeapStores.Aim7LoftOn) && (pDBWeapon->iSeekerType == 7)) || (orgStation & (0x40)))
	{
		W->Flags2 |= MISSILE_LOFT_AIM_7;
	}

	W->lBurnTimer = pDBWeapon->iBurnTime * 1000;
	if((pDBWeapon->iRocketMotor) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
	{
		W->Smoke	  = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);

	}
	else
	{
		W->Smoke	  = NULL;
	}

	W->Pos.Y += (ybuffer * FTTOWU);

	if ((P == PlayerPlane) && ((pDBWeapon->iWeaponType == WEAPON_TYPE_AG_MISSILE) || (pDBWeapon->iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE)))
	{
		W->Flags |= COLLISION_CHECK;
		W->CollisionCheckTimer = 0;
		W->AccelOrientation = W->Pos;
	}

	W->lTargetDistFt  = -1;
	W->pDecoyed		  = NULL;


	if((P==PlayerPlane) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
	{
		int iVolumeMod=0;
		int iSoundID=ltmiss;
		int iLevel=g_iSoundLevelExternalSFX-80;

		switch(pDBWeapon->lWeaponID)
		{
			case 34:
			case 35:
			case 38:
				iLevel=g_iSoundLevelExternalSFX-80;
				iSoundID=ltmiss;
			break;
			case 36:
			case 37:
			case 41:
				iLevel=g_iSoundLevelExternalSFX-70;
				iSoundID=ltmiss;
			break;
		}

		if (iLevel<0) iLevel=0;

		if (iLevel)
		{
			SndQueueSound(iSoundID,1,iLevel);
		}
	}

	if((pDBWeapon->iSeekerType == 1) && (W->pTarget))
	{
		WGetActivationPoint(W);
	}


#if 0
	if (Camera1.AttachedObjectType != CO_WEAPON)
	{
		setup_no_cockpit_art();
		AssignCameraSubject((void *)W,CO_WEAPON);
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
#endif
}

/*----------------------------------------------------------------------------
 *
 *	LockHeaterOnEngine()
 *
 */
void LockHeaterOnEngine(PlaneParams *planepnt, WeaponParams *W)
{
	float tempheading, temppitch;
	int enginecnt;
	int tempnum;

	PLGetRelativeHP(planepnt, W->Pos, &tempheading, &temppitch);
	if(fabs(tempheading) < 45)
	{
		return;
	}

	if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
	{
		if(planepnt->Type->EngineConfig == TWO_WING_ENGINES)
		{
			if(tempheading < 0)
			{
				if((!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE))) || (planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
				{
					W->Flags |= WEP_TRACK_LI;
				}
				else
				{
					W->Flags |= WEP_TRACK_RI;
				}
			}
			else
			{
				if((!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE))) || (planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
				{
					W->Flags |= WEP_TRACK_RI;
				}
				else
				{
					W->Flags |= WEP_TRACK_LI;
				}
			}
		}
		if(planepnt->Type->EngineConfig == FOUR_WING_ENGINES)
		{
			if(tempheading < 0)
			{
				enginecnt = 0;
				if(!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
				{
					enginecnt ++;
				}
				if(!(planepnt->DamageFlags & (DAMAGE_BIT_LO_ENGINE)))
				{
					enginecnt ++;
				}
				if(enginecnt || ((planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)) && (planepnt->DamageFlags & (DAMAGE_BIT_RO_ENGINE))))
				{
					if(!(enginecnt & 1))
					{
						tempnum = rand() & 1;
					}
					else
					{
						tempnum = 1;
					}

					if(tempnum && ((enginecnt == 0) || (!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))))
					{
						W->Flags |= WEP_TRACK_LI;
					}
					else
					{
						W->Flags |= WEP_TRACK_LO;
					}
				}
				else
				{
					enginecnt = 0;
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
					{
						enginecnt ++;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_RO_ENGINE)))
					{
						enginecnt ++;
					}

					if(!(enginecnt & 1))
					{
						tempnum = rand() & 1;
					}
					else
					{
						tempnum = 1;
					}

					if(tempnum && ((enginecnt == 0) || (!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))))
					{
						W->Flags |= WEP_TRACK_RI;
					}
					else
					{
						W->Flags |= WEP_TRACK_RO;
					}
				}
			}
			else
			{
				enginecnt = 0;
				if(!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
				{
					enginecnt ++;
				}
				if(!(planepnt->DamageFlags & (DAMAGE_BIT_RO_ENGINE)))
				{
					enginecnt ++;
				}
				if(enginecnt || ((planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)) && (planepnt->DamageFlags & (DAMAGE_BIT_LO_ENGINE))))
				{
					if(!(enginecnt & 1))
					{
						tempnum = rand() & 1;
					}
					else
					{
						tempnum = 1;
					}

					if(tempnum && ((enginecnt == 0) || (!(planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))))
					{
						W->Flags |= WEP_TRACK_RI;
					}
					else
					{
						W->Flags |= WEP_TRACK_RO;
					}
				}
				else
				{
					enginecnt = 0;
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))
					{
						enginecnt ++;
					}
					if(!(planepnt->DamageFlags & (DAMAGE_BIT_LO_ENGINE)))
					{
						enginecnt ++;
					}

					if(!(enginecnt & 1))
					{
						tempnum = rand() & 1;
					}
					else
					{
						tempnum = 1;
					}

					if(tempnum && ((enginecnt == 0) || (!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)))))
					{
						W->Flags |= WEP_TRACK_LI;
					}
					else
					{
						W->Flags |= WEP_TRACK_LO;
					}
				}
			}
		}
	}
	else if(planepnt->Type->EngineConfig == TWO_REAR_ENGINES)
	{
		if(fabs(tempheading) < 90)
		{
			return;
		}

		tempnum = rand() & 1;

		if(((!(planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE))) && (tempnum)) || (planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
		{
			W->Flags |= WEP_TRACK_LI;
		}
		else
		{
			W->Flags |= WEP_TRACK_RI;
		}
	}
	else
	{
		if(fabs(tempheading) < 90)
		{
			return;
		}

		W->Flags |= WEP_TRACK_LI;
	}
}

/*----------------------------------------------------------------------------
 *
 *	DeleteMissile()
 *
 */
void DeleteMissile( WeaponParams *W, int ignorenet)
{
//	if((MultiPlayer) && ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))
	if((MAIIsActiveWeapon(W)) && (!ignorenet))
	{
		SendNetWeaponTargetChange(W->iNetID, -3, W);
	}

	if(lBombFlags & WSO_BOMB_IMPACT)
	{
		DBWeaponType *pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	 	if(((W->WeaponMove == MoveGuidedBombs) || (pweapon_type->iSeekerType == 6)) && (W->P == PlayerPlane))
		{
			lBombFlags &= ~(WSO_BOMB_IMPACT|WSO_BOMB_TREL|WSO_BOMB_TPULL|WSO_STEERING_MSG|WSO_NAV_MSGS);
			lBombTimer = -1;
		}
	}

	GDClearWeaponLauncher(W);

	W->Flags = 0;
	W->Kind  = NONE;
	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	}
	W->Smoke = NULL;

//	UnsortObject(W);

	if (Camera1.AttachedObject == (int *)W)
		SwitchCameraSubjectToPoint(W->Pos);

	if (Camera1.TargetObject == (int *)W)
		SwitchCameraTargetToPoint(W->Pos);

	if(W == (WeaponParams *)pPadLockTarget)
	{
		iPadLockType = TARGET_LOCATION;
		fpdPadLockLocation = W->Pos;
		pPadLockTarget = NULL;
	}

	if(iLastWSOWarnType == TARGET_WEAPON)
	{
		if(W == (WeaponParams *)pLastWSOWarnObj)
		{
			iLastWSOWarnType = NONE;
			pLastWSOWarnObj = NULL;
		}
	}

	if (W == LastWeapon)
		while((LastWeapon >= Weapons) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;

	PlaneParams *P;

	if (W->iTargetType == TARGET_PLANE)
		P = (PlaneParams *)W->pTarget;
	else
		P = NULL;

	if (P)
	{
		if (P->AI.iMissileThreat == (W - Weapons))
		{
			P->AI.iMissileThreat = -1;
			if (AIPlaneAlreadyTarget(-1, NULL, P))
			{
				P->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN);
			}
			else
			{
				P->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN|AI_WSO_ENGAGE_DEFENSIVE);
			}
		}
	}

	W->pTarget = NULL;

#ifdef WATCHFALL
	Camera1.CameraMode = CAMERA_COCKPIT;
	Camera1.SubType = COCKPIT_FRONT;
	LoadCockpit(FRONT_FORWARD_COCKPIT);
	SetupView( Camera1.SubType );
	CockpitSeat = FRONT_SEAT;
	OurShowCursor( TRUE );
	placeholder = NULL;
	agpoint = NULL;
#endif
}


#if 1

#ifdef CHECKMISSFLIGHT
double trashvarsb = 0;
double trashvarsnb = 0;
double trashvarst = 0;
double trashvarsg = 0;
long trashvarstimer = 0;
double trashvarsrange = 0;
#endif

//
// common Missile variables (MoveMissile to newAttitude)
//
double		 maxyaw;
FPointDouble Delta;

/*----------------------------------------------------------------------------
 *
 *	newAttitude()
 *
 */
int newAttitude( WeaponParams *W)
{
	double  dH, dP;
	double  ddist;
	float yawmod = 0.2f;
	PlaneParams *P;
	float maxangle = 60.0f;
	int returnval = 1;
//	float keepheightangle = -30.0f;
	float seekerFOV = pDBWeaponList[ AIGetWeaponIndex(W->Type->TypeNumber) ].iSeekerFOV;
	float orgseekerFOV = seekerFOV;
	int ballistic = 0;

	if(W->Flags2 & MISSILE_VERTICAL_PHASE)
	{
		seekerFOV = 150.0f;
		if((W->LifeTime * WUTOFT) < 500.0f)
		{
			maxyaw = 0.01f / 50.0f;		//  I really don't want to change stuff.
		}
		else
		{
			maxyaw = (30.0f / 50.0f);
		}
	}

	maxangle = seekerFOV;

	dH = atan2(-Delta.X, -Delta.Z) * RADIANS_TO_DEGREES;

	if (dH >  180)  dH -= 360;
	if (dH < -180)  dH += 360;

	dH -= AIConvertAngleTo180Degree(W->Heading);

	if (dH >  180)  dH -= 360;
 	if (dH < -180)  dH += 360;

	dH = -dH;

	if(W->lTargetDistFt < (2 * NMTOFT))
	{
		maxangle = 40.0f;
	}

	//  Might not want to do this if the missile is within 200 ft of plane
//	if ((fabs(dH) > maxangle) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
	if((fabs(dH) > maxangle) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
	{
		ballistic = 1;
		if (W->pDecoyed)
		{
			W->pDecoyed = NULL;
			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, -1, W);
			}
		}
		else
		{
			W->Flags	   |= LOST_LOCK;
			W->fpTempWay.X	= -1;

			if (W->iTargetType == TARGET_PLANE)
				P = (PlaneParams *)W->pTarget;
			else
				P = NULL;

			if (P)
			{
				if (P->AI.iMissileThreat == (W - Weapons))
				{
					P->AI.iMissileThreat = -1;
					if (AIPlaneAlreadyTarget(-1, NULL, P))
					{
						P->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN);
					}
					else
					{
						P->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN|AI_WSO_ENGAGE_DEFENSIVE);
					}
				}
			}

			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, 0, W);
			}
			//  Do this after multiplayer stuff so we know this was the machine controlling the weapon.
			W->iTargetType	=  0;
			W->pTarget		= NULL;
			if((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT))
			{
				returnval = 0;
			}
		}
	}

	if (dH < -maxyaw)  dH = -maxyaw;
	if (dH >  maxyaw)  dH =  maxyaw;

//	ddist = (double)QuickDistance(Delta.X, Delta.Z);
	ddist = sqrt((Delta.X*Delta.X) + (Delta.Z*Delta.Z));

	dP = atan2(Delta.Y, ddist) * RADIANS_TO_DEGREES;	/* pi/2 */

	if (dP >  180)  dP -= 360;
	if (dP < -180)  dP += 360;

#if 0
	if((dP < 0) && (dP > keepheightangle) && (W->Flags & BOOST_PHASE))
	{
		dP = 0.0f;
	}
#endif

	dP -= AIConvertAngleTo180Degree(W->Pitch);

	if(W->Flags2 & MISSILE_LOFT_AIM_7)
	{
		dP += (seekerFOV * 0.75f);
	}

	if (dP >  180)  dP -= 360;
 	if (dP < -180)  dP += 360;

	//  Might not want to do this if the missile is within 200 ft of plane
//	if ((fabs(dP) > maxangle) && (W->WeaponMove != MoveAGMissile) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
	if((fabs(dP) > maxangle) && (W->WeaponMove != MoveAGMissile) && (W->WeaponMove != MoveTorpedo) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
	{
		if (W->pDecoyed)
		{
			W->pDecoyed = NULL;
			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, -1, W);
			}
		}
		else
		{
			W->Flags       |= LOST_LOCK;
			W->fpTempWay.X  = -1;

			if (W->iTargetType == TARGET_PLANE)
				P = (PlaneParams *)W->pTarget;
			else
				P = NULL;

			if (P)
			{
				if (P->AI.iMissileThreat == (W - Weapons))
				{
					P->AI.iMissileThreat = -1;
					if (AIPlaneAlreadyTarget(-1, NULL, P))
					{
						P->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN);
					}
					else
					{
						P->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN|AI_WSO_ENGAGE_DEFENSIVE);
					}
				}
			}

			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, 0, W);
			}
			//  Do this after multiplayer stuff so we know this was the machine controlling the weapon.
			W->iTargetType  = 0;
			W->pTarget      = NULL;
			if((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT))
			{
				returnval = 0;
			}
		}
	}

	if(W->Flags2 & MISSILE_VERTICAL_PHASE)
	{
		if((fabs(dP) < (orgseekerFOV * 0.5)) || (W->Pitch > 0x8000))
		{
			W->Flags2 &= ~(MISSILE_VERTICAL_PHASE);
		}
	}

	if (dP < -maxyaw)  dP = -maxyaw;
	if (dP >  maxyaw)  dP =  maxyaw;

	if(!ballistic)
	{
		W->Heading -= (ANGLE)(dH*DEGREE);  // limit to 1/2 deg angle change/second
		W->Pitch   += (ANGLE)(dP*DEGREE);
	}

	if(W->WeaponMove != MoveAGMissile)
	{
#ifdef CHECKMISSFLIGHT
		if (W->Flags & BOOST_PHASE)
		{
			trashvarsb += (fabs(FSinTimes((ANGLE)(dH*DEGREE),  W->InitialVelocity))) * yawmod;
			trashvarst += (fabs(FSinTimes((ANGLE)(dH*DEGREE),  W->InitialVelocity))) * yawmod;
		}
		else
		{
			trashvarsnb += (fabs(FSinTimes((ANGLE)(dH*DEGREE),  W->InitialVelocity))) * yawmod;
			trashvarst += (fabs(FSinTimes((ANGLE)(dH*DEGREE),  W->InitialVelocity))) * yawmod;
		}
#endif
		W->InitialVelocity -= (fabs(FSinTimes((ANGLE)(dH*DEGREE),  W->InitialVelocity)) * yawmod) / CalcAltEffectOnMissile(W->Pos.Y);
		W->InitialVelocity -= (fabs(FSinTimes((ANGLE)(dP*DEGREE),  W->InitialVelocity)) * yawmod) / CalcAltEffectOnMissile(W->Pos.Y);
	}

	W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );
	return(returnval);
}

/*----------------------------------------------------------------------------
 *
 *	MoveMissile()
 *
 */
void MoveMissile( WeaponParams *W )
{
	PlaneParams *P = NULL;
	long ldistft, ldistnm;
	int timerchange = 0;
	int ballistic = 0;
	int realweapons = 1;
	int seeker;
	PlaneParams *targplane = NULL;
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	GDRadarData *rradardat = NULL;
	double ddist;
	WeaponParams *decoy;
	double desiredspeed;
	float speedinc = 0;
	long lastdistft, currdistft;
	long tlastdistft, tcurrdistft;
	FPoint vel;
	PlaneParams *planepnt;
	FPointDouble engine_location;
	FPoint *offset;
	int offsetid;
	float tempheading, temppitch;
	float jammingval = 0.0f;
	float randjam;
	int tempval;
	int torpedo = 0;
	float ftemp;
	float workspeed;
	double orgvelocity = W->InitialVelocity;
	ANGLE workangle;
	int altchk = 0;
	int offbore = 0;
	InfoProviderInstance *radarprovider;
	MovingVehicleParams *vweapon;
	PlaneParams *orgtarg = (PlaneParams *)W->pTarget;
#ifdef SRE_SAM_CHECK
	char tempstr[80];
#endif

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	DBWeaponType *pDBWeapon = &pDBWeaponList[ AIGetWeaponIndex(W->Type->TypeNumber) ];

	if(!pDBWeapon)
	{
		goto Killer;
	}

	desiredspeed = MachAToFtSec((20000.0f), (float)pDBWeapon->fMaxSpeed)/(WUTOFT*50.0);
//	desiredspeed *= CalcAltEffectOnMissile(W->Pos.Y);

	seeker = pDBWeapon->iSeekerType;
	maxyaw = pDBWeapon->iMaxYawRate / 50.0;
	maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

#if 0
	// Check for offbore
	if(pDBWeapon-> something or another)
	{
		if(W->LifeTime < (4500 * FTTOWU))
		{
			offbore = 1;
		}
	}
#endif

//	maxyaw *= 0.5f + ((W->InitialVelocity / desiredspeed) * 0.5f);
	maxyaw *= (W->InitialVelocity / desiredspeed) * (0.5f + (CalcAltEffectOnMissile(W->Pos.Y) * 0.5f));

	if (W->LauncherType == AIRCRAFT)
	{
		targplane = W->P->AADesignate;
		if((W->P != PlayerPlane) && (seeker == 7))
		{
			if((!(W->P->AI.iAIFlags2 & AILOCKEDUP)) && (W->P->Status & PL_AI_DRIVEN) && (!(W->P->AI.iAICombatFlags1 & AI_HUMAN_CONTROLLED)))
			{
				if(targplane == PlayerPlane)
				{
					W->P->AI.iAIFlags2 |= AI_RADAR_PING;
				}
				targplane = NULL;
			}
		}
	}
	else if(W->LauncherType == MOVINGVEHICLE)
	{
		vweapon = (MovingVehicleParams *)W->Launcher;
		if(W->LaunchStation != -1)
		{
			radardat = &vweapon->RadarWeaponData[W->LaunchStation];
			targplane = (PlaneParams *)radardat->Target;
			rradardat = (GDRadarData *)radardat->pWRadarSite;
			if(!rradardat)
				rradardat = radardat;
			if(!((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF)))))
			{
				targplane = NULL;
				if(rradardat->Target)
				{
					if((rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(rradardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
					{
						targplane = (PlaneParams *)rradardat->Target;
					}
				}
			}
			if((seeker == 7) && (!(rradardat->lRFlags1 & GD_RADAR_LOCK)))
			{
				targplane = NULL;
			}
			if((W->pTarget == PlayerPlane) && (rradardat) && ((seeker == 1) || (seeker == 7)))
			{
				if(rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))
				{
					rradardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
				}
			}
		}
	}
	else
	{
		gweapon = (AAWeaponInstance *)W->Launcher;
		radardat = (GDRadarData *)gweapon->AIDataBuf;
		targplane = (PlaneParams *)radardat->Target;

		if((!((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF)))) && (W->Launcher != radardat->pWRadarSite)))
		{
			radarprovider = (InfoProviderInstance *)radardat->pWRadarSite;
			targplane = NULL;
			if(radarprovider)
			{
				rradardat = (GDRadarData *)radarprovider->AIDataBuf;
				if(rradardat->Target)
				{
					if((rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(rradardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
					{
						targplane = (PlaneParams *)rradardat->Target;
					}
				}
			}
			else
			{
				rradardat = radardat;
			}
			if((seeker == 7) && (!(rradardat->lRFlags1 & GD_RADAR_LOCK)))
			{
				targplane = NULL;
			}
		}
		else
		{
			if((seeker == 7) && (!(radardat->lRFlags1 & GD_RADAR_LOCK)))
			{
				targplane = NULL;
			}
		}
		if((W->pTarget == PlayerPlane) && (rradardat) && ((seeker == 1) || (seeker == 7)))
		{
			if(rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))
			{
				rradardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
			}
		}
	}

	if((W->Flags & BOOST_PHASE) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
	{

		if (W->InitialVelocity < desiredspeed)
		{
			speedinc = (float)pDBWeapon->iAcceleration/(WUTOFT*2500.0);
			if (W->LifeTime < (6000.0f * FTTOWU))
			{
				speedinc *= 8;
			}

			W->InitialVelocity += speedinc;
#if 0
			if (W->InitialVelocity > desiredspeed)
			{
				MBDoDebugBreak();
				temppitch = (float)W->LifeTime * WUTONM;
				UnPauseFromDiskHit();
			}
#endif
		}

		W->lBurnTimer -= 20;
		if (W->lBurnTimer < 0)
		{
			if (W->Smoke)
				((SmokeTrail *)W->Smoke)->MakeOrphan();
			W->Smoke = NULL;

			W->Flags &= ~(BOOST_PHASE);
			W->Flags |= GLIDE_PHASE;

			temppitch = (float)W->LifeTime * WUTONM;
		}
	}
	else
	{
		W->lBurnTimer -= 20;
		if(W->Pitch > 0x8000)
		{
#ifdef CHECKMISSFLIGHT
			trashvarsg += FCosTimes(W->Pitch, G_TIX);
#endif
			W->InitialVelocity -= (FCosTimes(W->Pitch, G_TIX) / CalcAltEffectOnMissile(W->Pos.Y));
		}
		else
		{
#ifdef CHECKMISSFLIGHT
			trashvarsg += G_TIX + FSinTimes(W->Pitch, G_TIX);
#endif
			W->InitialVelocity -= (G_TIX + FSinTimes(W->Pitch, G_TIX));
		}

		if (W->InitialVelocity < (desiredspeed / 3))
		{
			if(W->Pitch < 0x8000)
			{
				workangle = W->Pitch;
				if(workangle > 0x2000)
				{
					workangle = 0x2000;
				}
				workspeed = FCosTimes(workangle, (desiredspeed / 3));
				if(workspeed > orgvelocity)
				{
					workspeed = orgvelocity;
				}

				if(W->InitialVelocity < workspeed)
				{
					W->InitialVelocity = workspeed;
				}
			}
			else
			{
				if(orgvelocity > (desiredspeed / 3))
				{
					W->InitialVelocity = desiredspeed / 3;
				}
				else
				{
					W->InitialVelocity = orgvelocity;
				}
			}

#ifdef CHECKMISSFLIGHT
//			MBDoDebugBreak();
			trashvarstimer += 20;
			if(trashvarsrange == 0)
			{
				trashvarsrange = (float)W->LifeTime * WUTONM;
			}
//			UnPauseFromDiskHit();
#endif
		}
	}

	if (W->lTimer >= 0)
	{
//		W->lTimer -= DeltaTicks;
		W->lTimer -= 20;
		if (W->lTimer < 0)
		{
			if(W->Flags2 & MISSILE_DROP_PHASE)
			{
				W->Flags2 &= ~(MISSILE_DROP_PHASE);
				if((pDBWeapon->iRocketMotor) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
				{
					W->Smoke = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);
				}
				if(W->P==PlayerPlane)
				{
					int iVolumeMod=0;
					int iSoundID=ltmiss;
					int iLevel=g_iSoundLevelExternalSFX-80;

					switch(pDBWeapon->lWeaponID)
					{
						case 34:
						case 35:
						case 38:
							iLevel=g_iSoundLevelExternalSFX-80;
							iSoundID=ltmiss;
						break;
						case 36:
						case 37:
						case 41:
							iLevel=g_iSoundLevelExternalSFX-70;
							iSoundID=ltmiss;
						break;
					}

					if (iLevel<0) iLevel=0;

					if (iLevel)
					{
						SndQueueSound(iSoundID,1,iLevel);
					}
				}
			}

			timerchange = 1;
		}
		else if((W->Flags2 & MISSILE_DROP_PHASE) && (W->lTimer < 2000))
		{
			if((W->lTimer + 20) >= 2000)
			{
				W->Flags2 &= ~(MISSILE_DROP_PHASE);
				if((pDBWeapon->iRocketMotor) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
				{
					W->Smoke = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);
				}
				if(W->P==PlayerPlane)
				{
					int iVolumeMod=0;
					int iSoundID=ltmiss;
					int iLevel=g_iSoundLevelExternalSFX-80;

					switch(pDBWeapon->lWeaponID)
					{
						case 34:
						case 35:
						case 38:
							iLevel=g_iSoundLevelExternalSFX-80;
							iSoundID=ltmiss;
						break;
						case 36:
						case 37:
						case 41:
							iLevel=g_iSoundLevelExternalSFX-70;
							iSoundID=ltmiss;
						break;
					}

					if (iLevel<0) iLevel=0;

					if (iLevel)
					{
						SndQueueSound(iSoundID,1,iLevel);
					}
				}
			}
		}
	}

	if (W->pDecoyed)
	{
		if (timerchange)
		{
			CMCheckIsStillDecoyed(W);
		}
		if (W->lTimer > 1000)
		{
			W->lTimer = 1000;
		}
	}

	P = NULL;
	lastdistft = -1;

	if(W->Flags & WEP_SUN_TRACK)
	{
		SteerWeaponToSun(W);
		altchk = 1;
	}
	else if((W->iTargetType == TARGET_PLANE) && (W->pTarget))
	{
		P = (PlaneParams *)W->pTarget;
		lastdistft = W->lTargetDistFt;

		if(P == PlayerPlane)
		{
			if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY))
			{
				maxyaw /= 2;  // Return maxyaw to original database levels.
			}
		}

//		if(((P == PlayerPlane) && (IsDecoyJamming())) || ((P != PlayerPlane) && (P->AI.iAIFlags1 & AIJAMMINGON) && (P->AI.iAICombatFlags2 & AI_NOISE_JAMMER)) || (P->AI.iAICombatFlags1 & AI_MULTI_TOWED_DECOY))
		if(((P == PlayerPlane) && (IsDecoyJamming())) || ((P != PlayerPlane) && AVIsNoiseJamming(P)))
		{
			if((seeker == 1) || (seeker == 7))
			{
				if(pDBWeapon->iECMResistance & ECM_JAMMING)
				{
					if(pDBWeapon->iECMResistance & ECM_DECOY)  //  Enhanced Seeker
					{
						jammingval = 1.5f;
					}
					else
					{
						jammingval = 2.0f;
					}
				}
				else
				{
					jammingval = 4.0f;
				}
			}
		}

		if(!(W->Flags & (WEP_TRACK_LI|WEP_TRACK_LO|WEP_TRACK_RI|WEP_TRACK_RO)))
		{
			Delta.X = (P->WorldPosition.X - W->Pos.X);
			Delta.Y = (P->WorldPosition.Y - W->Pos.Y);
			Delta.Z = (P->WorldPosition.Z - W->Pos.Z);
		}
		else
		{
			offsetid = GetEngineArrayOffset(P, W->Flags);
			offset = &P->Type->Offsets[offsetid];
			engine_location.X = P->WorldPosition.X + P->Orientation.I.X*(double)offset->Z + P->Orientation.J.X*(double)offset->X + P->Orientation.K.X*(double)offset->Y;
			engine_location.Y = P->WorldPosition.Y + P->Orientation.I.Y*(double)offset->Z + P->Orientation.J.Y*(double)offset->X + P->Orientation.K.Y*(double)offset->Y;
			engine_location.Z = P->WorldPosition.Z + P->Orientation.I.Z*(double)offset->Z + P->Orientation.J.Z*(double)offset->X + P->Orientation.K.Z*(double)offset->Y;

			Delta.X = (engine_location.X - W->Pos.X);
			Delta.Y = (engine_location.Y - W->Pos.Y);
			Delta.Z = (engine_location.Z - W->Pos.Z);
		}

		ddist = (double)QuickDistance(Delta.X, Delta.Z);
		ldistft = ddist * WUTOFT;
		ldistnm = ddist * WUTONM;
		W->lTargetDistFt = currdistft = ldistft;
		if(W->lTargetDistFt < (5.0f * NMTOFT))
		{
			W->Flags2 &= ~(MISSILE_LOFT_AIM_7);
		}

		AICheckTargetReaction(W, 0);

		if (W->pDecoyed)
		{
			decoy = (WeaponParams *)W->pDecoyed;
			Delta.X = (decoy->Pos.X - W->Pos.X);
			Delta.Y = (decoy->Pos.Y - W->Pos.Y);
			Delta.Z = (decoy->Pos.Z - W->Pos.Z);

			ddist = (double)QuickDistance(Delta.X, Delta.Z);
			ldistft = ddist * WUTOFT;
			ldistnm = ddist * WUTONM;
//			W->lTargetDistFt = ldistft;
		}
		else
		{
			W->fpTempWay.X = -1;
			if(!((seeker == 2) || (seeker == 3) || ((seeker == 7) && (W->LauncherType == AIRCRAFT)) || (W->Flags2 & MISSILE_HOME_ON_JAM)))
			{
				WGetActivationPoint(W);
			}

			if(W->fpTempWay.X != -1)
			{
				Delta.X = (W->fpTempWay.X - W->Pos.X);
				Delta.Y = (W->fpTempWay.Y - W->Pos.Y);
				Delta.Z = (W->fpTempWay.Z - W->Pos.Z);
			}
		}

		if (ldistnm > 15)
		{
			Delta.Y += (5000 * FTTOWU);
		}

		if ((seeker == 2) || (seeker == 3))
		{
			if(realweapons)
			{
				if (W->lTimer < 0)
				{
					W->lTimer = 1000;
				}

				if(seeker == 3)
				{
					//  Condition for easy weapons accuracy
//					if((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))
					if((!MultiPlayer) || MAIIsActiveWeapon(W))
					{
						if((W->P != PlayerPlane) || (g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY))
						{
							PLGetRelativeHP(P, W->Pos, &tempheading, &temppitch);
							if((fabs(tempheading) < 120) || (fabs(temppitch) > 60) || (WCheckIRAspect(pDBWeapon, W, (PlaneParams *)W->pTarget) <= 0))
							{
								if(MultiPlayer)
								{
									SendNetWeaponTargetChange(W->iNetID, 0, W);
								}
								W->Flags       |= LOST_LOCK;
								W->fpTempWay.X  = -1;
								W->iTargetType  = 0;
								W->pTarget      = NULL;
								WGetActivationPoint(W);
								if(((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT)) && (W->Altitude > (1000.0f * FTTOWU)))
								{
#ifdef SRE_SAM_CHECK
									sprintf(tempstr, "Missile Deleted at %d", orgtarg ? (orgtarg - Planes) : -1);
									AICAddAIRadioMsgs(tempstr, 50);
#endif
									KillGSAM(W);
								}
							}
						}
					}
				}
//				if((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))
				if((!MultiPlayer) || MAIIsActiveWeapon(W))
				{
					if((W->P != PlayerPlane) || (g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY))
					{
						if((HeatSeekerSunFactor(W->Heading, W->Pitch, W->Pos, 10.0f) > frand()) && (!(pDBWeapon->iECMResistance & ECM_DECOY)))
						{
							if(MultiPlayer)
							{
								SendNetWeaponTargetChange(W->iNetID, 0, W);
								W->Flags       |= LOST_LOCK;
								W->fpTempWay.X  = -1;
								W->iTargetType  = 0;
								W->pTarget      = NULL;
								if((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT))
								{
#ifdef SRE_SAM_CHECK
									sprintf(tempstr, "Missile Deleted at %d", orgtarg ? (orgtarg - Planes) : -1);
									AICAddAIRadioMsgs(tempstr, 50);
#endif
									KillGSAM(W);
								}
							}
							else
							{
								W->Flags |= WEP_SUN_TRACK;
							}
						}
						else
						{
							 if(WCheckIRAspect(pDBWeapon, W, (PlaneParams *)W->pTarget) <= 0)
							 {
								if(MultiPlayer)
								{
									SendNetWeaponTargetChange(W->iNetID, 0, W);
								}
								W->Flags       |= LOST_LOCK;
								W->fpTempWay.X  = -1;
								W->iTargetType  = 0;
								W->pTarget      = NULL;
								WGetActivationPoint(W);
								if(((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT)) && (W->Altitude > (1000.0f * FTTOWU)))
								{
#ifdef SRE_SAM_CHECK
									sprintf(tempstr, "Missile Deleted at %d", orgtarg ? (orgtarg - Planes) : -1);
									AICAddAIRadioMsgs(tempstr, 50);
#endif
									KillGSAM(W);
								}
							 }
						}
					}
				}
			}
		}
		else if(W->Flags2 & MISSILE_HOME_ON_JAM)
		{
			if ((realweapons) && (timerchange))
			{
				if (W->lTimer < 0)
				{
					W->lTimer = 1000;
				}

//				if ((W->Flags & STT_MISSILE) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
				if ((!MultiPlayer) || MAIIsActiveWeapon(W))
				{
					WCheckAntiRadarLock(W);
				}
			}
		}
		else
		{
			if (ldistnm < 10)
			{
//				if ((!(W->Flags & FINAL_PHASE)) && ((seeker == 1) || (seeker == 2) || (seeker == 3)))
				if ((!(W->Flags & FINAL_PHASE)) && (seeker == 1))
				{
					W->Flags &= ~(LOSING_LOCK|LOST_LOCK);
					W->Flags |= FINAL_PHASE;
//					if((MultiPlayer) && ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))
					if((!MultiPlayer) || MAIIsActiveWeapon(W))
					{
						SendNetWeaponTargetChange(W->iNetID, -2, W);
						SendNetWeaponTargetChange(W->iNetID, (P - Planes) + 1, W);
					}
				}
			}

//			if ((realweapons) && (!(W->Flags & FINAL_PHASE)) && (timerchange))
			if ((realweapons) && (timerchange))
			{
				if (W->lTimer < 0)
				{
					W->lTimer = 1000;
				}

//				if ((W->Flags & STT_MISSILE) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
				if ((W->Flags & STT_MISSILE) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
				{
					if ((P != targplane) || ((W->P == PlayerPlane) && (seeker == 7) && (GetCurrentAARadarMode() != AA_STT_MODE)))  //  This should eventually be is the target still seen by radar (ie TWS can see more than one aircraft)
					{
	#if 0
						if (WCheckDesignateTarget(W))
						{
							W->pTarget = targplane;
						}
						else
	#endif
						if (!WCheckDesignateTarget(W))
						{
							if (!(W->Flags & LOSING_LOCK))
							{
								W->lTimer = 4000;
								W->Flags |= LOSING_LOCK;
#ifdef SRE_SAM_CHECK
								sprintf(tempstr, "Losing Lock Weapon %d", W - Weapons);
								AICAddAIRadioMsgs(tempstr, 50);
#endif
							}
							else if (timerchange)
							{
								if(MultiPlayer)
								{
									SendNetWeaponTargetChange(W->iNetID, 0, W);
								}
								W->Flags |= LOST_LOCK;
								WGetActivationPoint(W);
								W->iTargetType = 0;
								W->pTarget = NULL;
								if((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT) || (seeker == 7))
								{
#ifdef SRE_SAM_CHECK
									sprintf(tempstr, "Missile Deleted at %d", orgtarg ? (orgtarg - Planes) : -1);
									AICAddAIRadioMsgs(tempstr, 50);
#endif
									KillGSAM(W);
								}
							}
						}
					}
				}
			}
		}

		if((seeker == 2) || (seeker == 3))
		{
			if(pDBWeapon->lWeaponID == 72)
			{
				if((W->LifeTime < (200.0f * FTTOWU)) || (W->LauncherType != AIRCRAFT))
				{
					ballistic = 1;
				}
			}
			else if(pDBWeapon->lWeaponID == 109)
			{
				if (W->LifeTime < (250.0f * FTTOWU))
				{
					ballistic = 1;
				}
			}
			else
			{
				if (W->LifeTime < (500.0f * FTTOWU))
				{
					ballistic = 1;
				}
			}
		}
		else
		{
			if (W->LifeTime < (1000.0f * FTTOWU))
			{
				ballistic = 1;
			}
		}
	}
	else if (W->Flags & ACTIVE_SEARCH)
	{
		if (W->Flags & ACTIVE_SEEKER)
		{
			if (timerchange)
			{
				WSeekTarget(W);
				W->lTimer = 1000;
			}
		}
		else
		{
			if (timerchange)
			{
				W->Flags |= ACTIVE_SEEKER;
				W->lTimer = 1000;
			}
		}
		if (W->fpTempWay.X != -1)
		{
			Delta.X = (W->fpTempWay.X - W->Pos.X);
			Delta.Y = (W->fpTempWay.Y - W->Pos.Y);
			Delta.Z = (W->fpTempWay.Z - W->Pos.Z);
			ddist = (double)QuickDistance(Delta.X, Delta.Z);
			ldistft = ddist * WUTOFT;
			ldistnm = ddist * WUTONM;
			if (ldistnm < 3)
			{
				W->fpTempWay.X = -1;
			}
		}
		else
		{
			if((pDBWeapon->lWeaponID == 38) && ((W->P != PlayerPlane) || (g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY)))
			{
				torpedo = 1;
			}
			else
			{
				ballistic = 1;
			}
		}
	}
	else if (W->Flags & LOST_LOCK)
	{
		if (W->fpTempWay.X != -1)
		{
			Delta.X = (W->fpTempWay.X - W->Pos.X);
			Delta.Y = (W->fpTempWay.Y - W->Pos.Y);
			Delta.Z = (W->fpTempWay.Z - W->Pos.Z);
			ddist = (double)QuickDistance(Delta.X, Delta.Z);
			ldistft = ddist * WUTOFT;
			ldistnm = ddist * WUTONM;
			if (ldistnm < 10)
			{
//				if((seeker == 1) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
				if((seeker == 1) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
				{
					W->Flags &= ~(LOST_LOCK|LOSING_LOCK);
					W->Flags |= ACTIVE_SEARCH|ACTIVE_SEEKER;
					if (W->pDecoyed != NULL)
					{
						W->pDecoyed = NULL;
						if(MultiPlayer)
						{
							SendNetWeaponTargetChange(W->iNetID, -1, W);
						}
					}
					W->lTimer = 0;
				}
				else
				{
					W->fpTempWay.X = -1;
				}
			}
			else if (ldistnm > 15)
			{
				Delta.Y += (5000 / WUTOFT);
			}

		}
//		else if(((seeker == 1) || (seeker == 2)) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
		else if(((seeker == 1) || (seeker == 2)) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
		{
			W->Flags &= ~(LOST_LOCK|LOSING_LOCK);
			W->Flags |= ACTIVE_SEARCH|ACTIVE_SEEKER;
			if (W->pDecoyed != NULL)
			{
				W->pDecoyed = NULL;
				if(MultiPlayer)
				{
					SendNetWeaponTargetChange(W->iNetID, -1, W);
				}
			}
			W->lTimer = 0;
		}
		else if((seeker == 7) && (W->P == PlayerPlane) && ((PlayerPlane->WorldPosition - W->Pos) < (10.0f * NMTOWU)) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
		{
			W->Flags &= ~(LOST_LOCK|LOSING_LOCK);
			W->Flags |= ACTIVE_SEARCH|ACTIVE_SEEKER;
			if (W->pDecoyed != NULL)
			{
				W->pDecoyed = NULL;
				if(MultiPlayer)
				{
					SendNetWeaponTargetChange(W->iNetID, -1, W);
				}
			}
			W->lTimer = 0;
		}
		else
		{
			ballistic = 1;
		}
	}
	else
	{
		ballistic = 1;
	}

	ftemp = (pDBWeapon->fMinRange * NMTOWU * 0.65f);
	if ((W->LifeTime < ftemp) && (!(W->Flags2 & MISSILE_VERTICAL_PHASE)))
	{
		ballistic = 1;
	}

	 //	End of pre-calc, do the missile's flight
	//

	if(!(ballistic||torpedo))
	{
		if(!newAttitude(W))
		{
#ifdef SRE_SAM_CHECK
									sprintf(tempstr, "Missile Deleted at %d", orgtarg ? (orgtarg - Planes) : -1);
									AICAddAIRadioMsgs(tempstr, 50);
#endif
			KillGSAM(W);
		}
	}
	else if(!ballistic)
	{
		if((timerchange) || ((W->lTimer / 250) != ((W->lTimer + 20) / 250)))
		{
			tempval = rand() & 3;
			switch(tempval)
			{
				case 1:
					W->Flags &= ~(WEP_TURN_RIGHT);
					W->Flags |= (WEP_TURN_LEFT);
					break;
				case 2:
					W->Flags &= ~(WEP_TURN_RIGHT|WEP_TURN_LEFT);
					break;
				case 3:
					W->Flags &= ~(WEP_TURN_LEFT);
					W->Flags |= (WEP_TURN_RIGHT);
					break;
				default:
					break;
			}
		}

		if(W->Flags & WEP_TURN_RIGHT)
		{
			W->Heading -= (ANGLE)((maxyaw / 4)*DEGREE);  // limit to 1/2 deg angle change/second
		}
		else if(W->Flags & WEP_TURN_LEFT)
		{
			W->Heading += (ANGLE)((maxyaw / 4)*DEGREE);  // limit to 1/2 deg angle change/second
		}
	}

	if(!(W->Flags2 & MISSILE_DROP_PHASE))
	{
#if 1
		double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
		W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);
	}

	W->LastPos = W->Pos;	// WeaponHitsPlane() & WeaponIntersectsGround() need this
	W->Pos    += W->Vel;

	if (W->Smoke)  ((SmokeTrail *) W->Smoke )->Update();

	if(ballistic||torpedo||altchk)
	{
		if(W->Pos.Y > (80000.0f * FTTOWU))
		{
			goto Killer;
		}
	}

	planepnt = WeaponHitsPlane(W);

	if(planepnt)
	{
		/* ------------------------------------------1/20/98 7:40PM--------------------------------------------
		 * Missile Scored a direct hit!
		 * ----------------------------------------------------------------------------------------------------*/

		DoExplosionInAir(W, 0, NULL, NULL, planepnt);

		PlayPositionalExplosion(pDBWeapon->iWarHead,Camera1,planepnt->WorldPosition);

		FPoint novel(0.0f);
		NewCanister( CT_FLASH, W->Pos, novel, pDBWeapon->iDamageValue/50.0f);
		NewGenerator(PLANE_EXPLODES,W->Pos,W->Vel,1.0f,5 + (rand()&0x7));

		if (Camera1.AttachedObject == (int *)W)
		 	AssignCameraSubject(planepnt,CO_PLANE);

		goto Killer;
	}
//	else if((lastdistft != -1) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
	else if((lastdistft != -1) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
	{
		tempval = rand() & 1023;
		randjam = (float)(tempval * jammingval) / 1024;
		tlastdistft = lastdistft - ((float)P->Type->ShadowLRZOff + ((float)P->Type->ShadowLRZOff * randjam)) * WUTOFT;

//		if(tlastdistft < 0)
//			tlastdistft = 0;

		tempval = rand() & 1023;
		randjam = (float)(tempval * jammingval) / 1024;
		tcurrdistft = currdistft - ((float)P->Type->ShadowLRZOff + ((float)P->Type->ShadowLRZOff * randjam)) * WUTOFT;

//		if(tcurrdistft < 0)
//			tcurrdistft = 0;

		if(tlastdistft < tcurrdistft)
		{
			if(tlastdistft < pDBWeapon->iDamageRadius)
			{
				vel.MakeVectorToFrom(W->Pos,W->LastPos);
				FPoint novel( 0.0f);
				NewCanister( CT_FLASH, W->Pos, novel, pDBWeapon->iDamageValue/50.0f);
				NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, 0.0f );
				NewGenerator(PLANE_EXPLODES,W->Pos,vel,1.0f,2);
				DoExplosionInAir(W);

				PlayPositionalExplosion(pDBWeapon->iWarHead,Camera1,W->Pos);

				goto Killer;
			}
		}
		else if(!W->pTarget)
		{
			if(tcurrdistft < pDBWeapon->iDamageRadius)
			{
				vel.MakeVectorToFrom(W->Pos,W->LastPos);
				FPoint novel( 0.0f);
				NewCanister( CT_FLASH, W->Pos, novel, pDBWeapon->iDamageValue/50.0f);
				NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, 0.0f );
				NewGenerator(PLANE_EXPLODES,W->Pos,vel,1.0f,2);
				DoExplosionInAir(W);

				PlayPositionalExplosion(pDBWeapon->iWarHead,Camera1,W->Pos);

				goto Killer;
			}
		}
	}

	if (WeaponIntersectsGround(W))
	{
		DoExplosionOnGround(W);
		goto Killer;
	}

	// update total distance traveled
//	W->LifeTime += W->Vel.Length() / CalcAltEffectOnMissile(W->Pos.Y);
	if((W->LauncherType == GROUNDOBJECT) || (W->LauncherType == MOVINGVEHICLE))
	{
		W->LifeTime += W->Vel.Length();
	}
	else
	{
		W->LifeTime += W->Vel.Length() / CalcAltEffectOnMissile(W->Pos.Y);
	}

	if (W->LifeTime > pDBWeapon->iRange * NMTOWU)
	{
#ifdef CHECKMISSFLIGHT
		MBDoDebugBreak();
		tempheading = FCosTimes(0, G_TIX);
		double ddd3 = MachAToFtSec(((float)W->Pos.Y * WUTOFT), (float)pDBWeapon->fMaxSpeed) * 60 * 60 / 6076;;
		double ddd4 = (W->InitialVelocity * WUTOFT * 50 * 60 * 60) / 6076;;
		trashvarsb *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
		trashvarsnb *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
		trashvarst *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
		trashvarsg *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
		UnPauseFromDiskHit();
#endif

		if((W->LauncherType == GROUNDOBJECT) || (W->LauncherType == MOVINGVEHICLE))
		{
			vel.MakeVectorToFrom(W->Pos,W->LastPos);

			  FPoint novel( 0.0f);
			NewCanister( CT_FLASH, W->Pos, novel, pDBWeapon->iDamageValue/50.0f);
			NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, 0.0f );
			NewGenerator(PLANE_EXPLODES,W->Pos,vel,1.0f,2);
			DoExplosionInAir(W);
			goto Killer;
		}
		else
		{
			if (W->Smoke)
			{
				((SmokeTrail *)W->Smoke)->Update();
				((SmokeTrail *)W->Smoke)->MakeOrphan();
			}
			W->Smoke = NULL;

			W->WeaponMove = MoveBombs;

			W->fGravity = 32.0f;

			if (W->iTargetType == TARGET_PLANE)
				targplane = (PlaneParams *)W->pTarget;
			else
				targplane = NULL;

			W->pTarget = NULL;
			if (targplane)
			{
				if (targplane->AI.iMissileThreat == (W - Weapons))
				{
					targplane->AI.iMissileThreat = -1;
					if (AIPlaneAlreadyTarget(-1, NULL, targplane))
					{
						targplane->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN);
					}
					else
					{
						targplane->AI.iAIFlags1 &= ~(AI_WSO_MISSILE_WARN|AI_WSO_ENGAGE_DEFENSIVE);
					}
				}
			}
		}
//		goto Killer;
	}

	return;		// normal exit

   //------------------------
  //
 //	Delete the spent missile.
//
Killer:
#ifdef CHECKMISSFLIGHT
	MBDoDebugBreak();
	temppitch = (float)W->LifeTime * WUTONM;
	tempheading = FCosTimes(0, G_TIX);
	double ddd1 = MachAToFtSec(((float)W->Pos.Y * WUTOFT), (float)pDBWeapon->fMaxSpeed) * 60 * 60 / 6076;;
	double ddd2 = (W->InitialVelocity * WUTOFT * 50 * 60 * 60) / 6076;;
	trashvarsb *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
	trashvarsnb *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
	trashvarst *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
	trashvarsg *= (WUTOFT * 50.0f * 60.0f * 60.0f) / 6076.115f;
	UnPauseFromDiskHit();
#endif

#ifdef SRE_SAM_CHECK
	sprintf(tempstr, "Missile Deleted at %d", orgtarg ? (orgtarg - Planes) : -1);
	AICAddAIRadioMsgs(tempstr, 50);
#endif

	DeleteMissile(W);
}

/*----------------------------------------------------------------------------
 *
 *	GetEngineArrayOffset()
 *
 */
int GetEngineArrayOffset(PlaneParams *planepnt, long engineid, int idtype)
{
	if((planepnt->Type->EngineConfig == TWO_WING_ENGINES) || (planepnt->Type->EngineConfig == FOUR_WING_ENGINES))
	{
		if(engineid & WEP_TRACK_LI)
		{
			return(OFFSET_ENGINE_WING_LEFT_INBOARD);
		}
		else if(engineid & WEP_TRACK_LO)
		{
			return(OFFSET_ENGINE_WING_LEFT_OUTBOARD);
		}
		else if(engineid & WEP_TRACK_RI)
		{
			return(OFFSET_ENGINE_WING_RIGHT_INBOARD);
		}
		else if(engineid & WEP_TRACK_RO)
		{
			return(OFFSET_ENGINE_WING_RIGHT_OUTBOARD);
		}
	}
	else if(planepnt->Type->EngineConfig == TWO_REAR_ENGINES)
	{
		if(engineid & WEP_TRACK_LI)
		{
			return(OFFSET_ENGINE_REAR_LEFT);
		}
		else if(engineid & WEP_TRACK_RI)
		{
			return(OFFSET_ENGINE_REAR_RIGHT);
		}
	}
	else
	{
		if(engineid & WEP_TRACK_LI)
		{
			return(OFFSET_ENGINE_REAR);
		}
	}
	return(0);
}

/*----------------------------------------------------------------------------
 *
 *	WCheckDesignateTarget()
 *
 */
int WCheckDesignateTarget(WeaponParams *W)
{
	FPointDouble Delta;
	float Dh, Dp;
	PlaneParams *targplane = NULL;
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	GDRadarData *pradardat = NULL;
	InfoProviderInstance *radarprovider = NULL;
	int seeker;
	DBWeaponType *pDBWeapon = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	PlaneParams *wtarget;
	double angleheight;
	float angleperc;
	MovingVehicleParams *vweapon = NULL;
	GDRadarData *rradardat = NULL;
	int radaron = 0;
	float offangle, offpitch;

	seeker = pDBWeapon->iSeekerType;

	wtarget = (PlaneParams *)W->pTarget;

	if(!W->pTarget)
	{
		return(0);
	}

	if((!(pDBWeapon->iECMResistance & ECM_DECOY)) && ((wtarget->HeightAboveGround * WUTOFT) < 3000.0f))
	{
		if((seeker == 7) || ((seeker == 1) && (W->Flags & FINAL_PHASE)))
		{
			Delta.X = (wtarget->WorldPosition.X - W->Pos.X);
			Delta.Y = (wtarget->WorldPosition.Y - W->Pos.Y);
			Delta.Z = (wtarget->WorldPosition.Z - W->Pos.Z);

			Dp = atan2(Delta.Y, (double)QuickDistance(Delta.X,Delta.Z)) * RADIANS_TO_DEGREES;	/* pi/2 */
			if (Dp > 180)  Dp -= 360;
			if (Dp < -180) Dp += 360;

			if(Dp)
			{
				angleheight = (wtarget->HeightAboveGround * WUTOFT) / sin(DegToRad(-Dp));
				if((angleheight >= 0) && (angleheight < 3000.0f))
				{
					angleperc = 0.5f + ((angleheight / 3000.0f) * 0.5f);
					if(angleperc < frand())
					{
						return(0);
					}
				}
			}
		}
	}

	if(W->Flags & FINAL_PHASE)
	{
		return(1);
	}

	if(W->LauncherType == AIRCRAFT)
	{
		if(!W->P)
			return(0);

		if(W->P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			return(0);
		}

		if(W->P == PlayerPlane)
		{
			if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY))
			{
				return(1);
			}

			if(IsPlaneDetected(wtarget - Planes))
			{
				if(seeker == 7)
				{
					if (UFC.EMISState || Av.AARadar.SilentOn)  return 0;

					if(GetCurrentAARadarMode() != AA_STT_MODE)
					{
						targplane = (PlaneParams *)W->pTarget;
						if(targplane)
						{
							if((targplane->WorldPosition - PlayerPlane->WorldPosition) < (10.0f * NMTOWU))
							{
								PLGetRelativeHP(PlayerPlane, targplane->WorldPosition, &offangle, &offpitch);
								if((fabs(offangle) < 12.0f) && (fabs(offpitch) < 12.0f))
								{
									return(1);
								}
							}
						}
						return(0);
					}
					else if(PlayerPlane->AADesignate != W->pTarget)
					{
						return(0);
					}
				}
				return(1);
			}
		}
		else if(W->P->AI.iAIFlags1 & AIRADARON)
		{
			if((seeker != 7) || (W->P->AI.iAIFlags2 & AILOCKEDUP))
			{
				targplane = W->P->AADesignate;
				if(!targplane)
				{
					targplane = W->P->AI.AirTarget;
				}
			}
			else
			{
				targplane = NULL;
			}
		}
	}
	else if(W->LauncherType == MOVINGVEHICLE)
	{
		vweapon = (MovingVehicleParams *)W->Launcher;
		if(W->LaunchStation == -1)
		{
			return(0);
		}
		radardat = &vweapon->RadarWeaponData[W->LaunchStation];
		targplane = (PlaneParams *)radardat->Target;
		rradardat = (GDRadarData *)radardat->pWRadarSite;
		if(!rradardat)
			rradardat = radardat;
		if(!((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF)))))
		{
			targplane = NULL;
			if(rradardat->Target)
			{
				if((rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(rradardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
				{
					targplane = (PlaneParams *)rradardat->Target;
				}
			}
		}
		if((seeker == 7) && (!(rradardat->lRFlags1 & GD_RADAR_LOCK)))
		{
			targplane = NULL;
		}
	}
	else
	{
		gweapon = (AAWeaponInstance *)W->Launcher;
		radardat = (GDRadarData *)gweapon->AIDataBuf;
		rradardat = radardat;
		if((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
		{
			targplane = (PlaneParams *)radardat->Target;
		}
		else if(W->Launcher != radardat->pWRadarSite)
		{
			radarprovider = (InfoProviderInstance *)radardat->pWRadarSite;
			targplane = NULL;

			if(radarprovider)
			{
				rradardat = (GDRadarData *)radarprovider->AIDataBuf;
				if(rradardat->Target)
				{
					if((rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(rradardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
					{
						targplane = (PlaneParams *)rradardat->Target;
					}
				}
			}
			else
			{
				rradardat = radardat;
			}
		}
		if((seeker == 7) && (!(rradardat->lRFlags1 & GD_RADAR_LOCK)))
		{
			targplane = NULL;
		}
	}


	if(targplane == NULL)
	{
		return(0);
	}

	if(targplane == (PlaneParams *)W->pTarget)
	{
		return(1);
	}

	if(W->LauncherType == AIRCRAFT)
	{
		if(W->P->AI.iAIFlags2 & AI_CHECK_MISSILES)
		{
			return(0);
		}
	}
	else
	{
		if(W->LauncherType == MOVINGVEHICLE)
		{
			if(radardat->lRFlags1 & GD_CHECK_MISSILES)
			{
				return(0);
			}
		}
		else if(radardat->pWRadarSite == (BasicInstance *)gweapon)
		{
			if(radardat->lRFlags1 & GD_CHECK_MISSILES)
			{
				return(0);
			}
		}
		else
		{
			radarprovider = (InfoProviderInstance *)radardat->pWRadarSite;
			pradardat = (GDRadarData *)radarprovider->AIDataBuf;
			if(pradardat->lRFlags1 & GD_CHECK_MISSILES)
			{
				return(0);
			}
		}
	}

	Delta.X = (targplane->WorldPosition.X - W->Pos.X);
	Delta.Y = (targplane->WorldPosition.Y - W->Pos.Y);
	Delta.Z = (targplane->WorldPosition.Z - W->Pos.Z);

	Dh = atan2(-Delta.X, -Delta.Z) * RADIANS_TO_DEGREES;
	if (Dh > 180)  Dh -= 360;
	if (Dh < -180) Dh += 360;
	Dh -= AIConvertAngleTo180Degree(W->Heading);
	if (Dh > 180)  Dh -= 360;
 	if (Dh < -180) Dh += 360;
	Dh = -Dh;

	Dp = atan2(Delta.Y, (double)QuickDistance(Delta.X,Delta.Z)) * RADIANS_TO_DEGREES;	/* pi/2 */
	if (Dp > 180)  Dp -= 360;
	if (Dp < -180) Dp += 360;
	Dp -= AIConvertAngleTo180Degree(W->Pitch);
	if (Dp > 180)  Dp -= 360;
 	if (Dp < -180) Dp += 360;

	if((fabs(Dh) < 12) && (fabs(Dp) < 12))
	{
		W->pTarget = targplane;
		return(1);
	}

	return(0);
}

/*----------------------------------------------------------------------------
 *
 *	WGetActivationPoint()
 *
 */
void WGetActivationPoint(WeaponParams *W)
{
	PlaneParams *planepnt;
	FPointDouble Delta;
	double ddist;
	float dx, dy, dz, tdist, offangle;
	double vel1, vel2;
	float time;
	float disttravel;
	float fworkvar;
	int weaponindex, seeker;

	weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
	seeker = pDBWeaponList[weaponindex].iSeekerType;

	if((seeker == 2) || (seeker == 3) || ((seeker == 7) && (W->LauncherType == AIRCRAFT)))
	{
		if((seeker == 7) && (W->P != PlayerPlane))
			return;

		W->Flags |= ACTIVE_SEARCH|ACTIVE_SEEKER;
		W->lTimer = 0;
		return;
	}

	planepnt = (PlaneParams *)W->pTarget;

	if(!planepnt)
	{
		return;
	}

	Delta.X = (planepnt->WorldPosition.X - W->Pos.X);
	Delta.Y = (planepnt->WorldPosition.Y - W->Pos.Y);
	Delta.Z = (planepnt->WorldPosition.Z - W->Pos.Z);

	ddist = sqrt((Delta.X*Delta.X) + (Delta.Y*Delta.Y) + (Delta.Z*Delta.Z)) * WUTOFT;

//	ddist -= 60760; // (offset activation point by 10 nm)  I think this is redundant since I activate when I am 10 nm for point

	offangle = AIComputeHeadingToPoint(planepnt, W->Pos, &tdist, &dx ,&dy, &dz, 1);

	vel1 = cos(DegToRad(offangle)) * planepnt->V;

	vel2 = W->InitialVelocity * (WUTOFT*50.0);

	time = (ddist) / (vel1 + vel2);

	disttravel = planepnt->V * time;

	fworkvar = (sin(DegToRad((double)planepnt->Pitch / DEGREE)) * disttravel);
	W->fpTempWay.Y = planepnt->WorldPosition.Y + (fworkvar / WUTOFT);

	disttravel = (cos(DegToRad((double)planepnt->Pitch / DEGREE)) * disttravel);

	fworkvar = (sin(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	W->fpTempWay.X = planepnt->WorldPosition.X - (fworkvar / WUTOFT);

	fworkvar = (cos(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	W->fpTempWay.Z = planepnt->WorldPosition.Z - (fworkvar / WUTOFT);
}

/*----------------------------------------------------------------------------
 *
 *	WGetActivationPoint()
 *
 */
void WGetLaunchInterceptPoint(PlaneParams *launcher, int weaponindex, PlaneParams *planepnt, double avgvelfps, FPointDouble *InterceptPoint)
{
	FPointDouble Delta;
	double ddist;
	float dx, dy, dz, tdist, offangle;
	double vel1, vel2;
	float time;
	float disttravel;
	float fworkvar;

	if(!planepnt)
	{
		return;
	}

	Delta.X = (planepnt->WorldPosition.X - launcher->WorldPosition.X);
	Delta.Y = (planepnt->WorldPosition.Y - launcher->WorldPosition.Y);
	Delta.Z = (planepnt->WorldPosition.Z - launcher->WorldPosition.Z);

	ddist = sqrt((Delta.X*Delta.X) + (Delta.Y*Delta.Y) + (Delta.Z*Delta.Z)) * WUTOFT;

//	ddist -= 60760; // (offset activation point by 10 nm)  I think this is redundant since I activate when I am 10 nm for point

	offangle = AIComputeHeadingToPoint(planepnt, launcher->WorldPosition, &tdist, &dx ,&dy, &dz, 1);

	vel1 = cos(DegToRad(offangle)) * planepnt->V;

	vel2 = avgvelfps;

	time = (ddist) / (vel1 + vel2);

	disttravel = planepnt->V * time;

	fworkvar = (sin(DegToRad((double)planepnt->Pitch / DEGREE)) * disttravel);
	InterceptPoint->Y = planepnt->WorldPosition.Y + (fworkvar / WUTOFT);

	disttravel = (cos(DegToRad((double)planepnt->Pitch / DEGREE)) * disttravel);

	fworkvar = (sin(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	InterceptPoint->X = planepnt->WorldPosition.X - (fworkvar / WUTOFT);

	fworkvar = (cos(DegToRad((double)planepnt->Heading / DEGREE)) * disttravel);
	InterceptPoint->Z = planepnt->WorldPosition.Z - (fworkvar / WUTOFT);
}

/*----------------------------------------------------------------------------
 *
 *	WGetLaunchAvgSpeed()
 *
 */
float WGetLaunchAvgSpeed(int weaponindex, float distft, float startfps, float avgaltft, float *time)
{
	double avgboostfps;
	double avgburnfps, burndist, burnsecs;
	double avgsustainfps, sustaindist, sustainsecs;
	double avgglidefps, glidedist, glidesecs;
	double avgendfps, enddist, endsecs;
	double totaldist = 0;
	double workval;
	double accelfps = pDBWeaponList[weaponindex].iAcceleration;
	double maxspeedfps = MachAToFtSec(20000.0f, pDBWeaponList[weaponindex].fMaxSpeed);
//	maxspeedfps *= CalcAltEffectOnMissile(avgaltft * FTTOWU);
	double minspeedfps = maxspeedfps / 3;
	double timetomin;
	double burntime = pDBWeaponList[weaponindex].iBurnTime;
	double boostsecs, boostspeedfps;
	double boostdist = 6000.0f / CalcAltEffectOnMissile(avgaltft * FTTOWU);
	double timetomax;

	if(!accelfps)
	{
		accelfps = 1.0f;
	}
	if(!maxspeedfps)
	{
		maxspeedfps = 1.0f;
	}

	timetomax = (maxspeedfps - startfps) / accelfps;

	boostsecs = (-startfps + sqrt((startfps * startfps) + (2 * 8 * accelfps * boostdist))) / (accelfps * 8);

	if(timetomax > boostsecs)
	{
		timetomax -= boostsecs;
	}
	else
	{
		timetomax = (maxspeedfps - startfps) / (accelfps * 8);
		boostsecs = timetomax;
		boostdist = (startfps * timetomax) + (0.5f * accelfps * 8 * timetomax * timetomax);
	}




	*time = 0;

	if(distft < boostdist)
	{
		boostsecs = (-startfps + sqrt((startfps * startfps) + (2 * 8 * accelfps * distft))) / (accelfps * 8);
		maxspeedfps = startfps + (accelfps * boostsecs);
		avgboostfps = (maxspeedfps + startfps) * 0.5;
		*time = boostsecs;
		return(avgboostfps);
	}

	if(!boostsecs)
	{
		boostsecs = 1.0f;
	}

	*time = *time + boostsecs;
	totaldist += boostdist;

	boostspeedfps = startfps + (accelfps * boostsecs * 8);
	avgboostfps = startfps + (accelfps * boostsecs * 4.0f);

	if(burntime < timetomax)
	{
		burnsecs = burntime - boostsecs;
		if(burnsecs > 0)
		{
			burndist = (boostspeedfps * burnsecs) + (0.5f * accelfps * burnsecs * burnsecs);
			maxspeedfps = boostspeedfps + (accelfps * burnsecs);
			avgburnfps = boostspeedfps + (accelfps * burnsecs * 0.5f);
		}
		else
		{
			maxspeedfps = boostspeedfps;
			burndist = 0;
			avgburnfps = boostspeedfps + (accelfps * 4 * boostsecs);
		}

		avgsustainfps = maxspeedfps;
		sustaindist = 0;
		sustainsecs = 0;
	}
	else
	{
		burnsecs = timetomax - boostsecs;
		burndist = (boostspeedfps * burnsecs) + (0.5f * accelfps * burnsecs * burnsecs);
		avgburnfps = boostspeedfps + (accelfps * burnsecs * 0.5f);
	}

	if((totaldist + burndist) > distft)
	{
		burnsecs = (-boostspeedfps + sqrt((boostspeedfps * boostspeedfps) + (2 * accelfps * (distft - totaldist)))) / accelfps;
		maxspeedfps = boostspeedfps + (accelfps * burnsecs);
		avgburnfps = (maxspeedfps + boostspeedfps) * 0.5f;
		*time = *time + burnsecs;
		workval = ((avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (burnsecs + boostsecs);
		return(workval);
	}

	*time = *time + burnsecs;
	totaldist += burndist;

	if(burntime > timetomax)
	{
		avgsustainfps = maxspeedfps;
		sustainsecs = burntime - timetomax;
		sustaindist = sustainsecs * avgsustainfps;
	}

	if((totaldist + sustaindist) > distft)
	{
		sustaindist = distft - totaldist;
		sustainsecs = sustaindist / maxspeedfps;

		workval = ((avgsustainfps * sustainsecs) + (avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (sustainsecs + burnsecs + boostsecs);
		*time = *time + sustainsecs;
		return(workval);
	}

	*time = *time + sustainsecs;
	totaldist += sustaindist;

	timetomin = (maxspeedfps - minspeedfps) / 32.0f;
	glidesecs = timetomin;
	glidedist = (maxspeedfps * timetomin) + (-16.0f * timetomin * timetomin);
	avgglidefps = (maxspeedfps + minspeedfps) * 0.5f;

	if((totaldist + glidedist) > distft)
	{
		glidedist = distft - totaldist;

		glidesecs = (-maxspeedfps + sqrt((maxspeedfps * maxspeedfps) + (2 * -32 * glidedist))) / -32;
		workval = maxspeedfps + (-32.0f * glidesecs);
		avgglidefps = (maxspeedfps + workval) * 0.5f;
		workval = ((avgglidefps * glidesecs) + (avgsustainfps * sustainsecs) + (avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (glidesecs + sustainsecs + burnsecs + boostsecs);
		*time = *time + glidesecs;
		return(workval);
	}

	*time = *time + glidesecs;
	totaldist += glidedist;

	if(!minspeedfps)
	{
		minspeedfps = 1.0f;
	}

	avgendfps = minspeedfps;
	enddist = distft - totaldist;
	endsecs = enddist / avgendfps;

	*time = *time + endsecs;

	workval = ((avgendfps * endsecs) + (avgglidefps * glidesecs) + (avgsustainfps * sustainsecs) + (avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (endsecs + glidesecs + sustainsecs + burnsecs + boostsecs);
	return(workval);
}

/*----------------------------------------------------------------------------
 *
 *	WGetTimeToAAImpact()
 *
 *		returns the time to impact in seconds (float)
 *
 *		PlaneParams *P	-	pointer to plane which is shooting (PlayerPlane)
 *		PlaneParams *target	-	pointer to plane which is target
 *									(&Planes[CurTargetInfo.CurTargetId])
 *		float *timeactive	-	if not null will hold time to activation in seconds.
 *
 */
float WGetTimeToAAImpact(PlaneParams *P, PlaneParams *target, float *timeactive)
{
	int weaponindex= -1;
	double distance;
//	float avgalt = (P->WorldPosition.Y + target->WorldPosition.Y) * WUTOFT * 0.5f;
	float avgalt = (P->WorldPosition.Y) * WUTOFT * 0.5f;
	float avgvel;
	FPointDouble intercept;
	float ftimeimpact;
	double workdistance;
	double workangle;
	double maxdist;

	if((!P) || (!target))
	{
		if(timeactive)
		{
			*timeactive = 0;
		}
		return(0);
	}

	if(P == PlayerPlane)
	{
		if(Av.Weapons.UseAASeq)
		{
            if(Av.Weapons.CurAASeqWeap)
			{
              weaponindex = Av.Weapons.CurAASeqWeap->W->WeapIndex;
 			}
		}
		else
		{
            if(Av.Weapons.CurAAWeap)
			{
              weaponindex = Av.Weapons.CurAAWeap->W->WeapIndex;
			}
		}
	}
	else
	{
		if(timeactive)
		{
			*timeactive = 0;
		}
		return(0);
	}

	if(weaponindex == -1)
	{
		if(timeactive)
		{
			*timeactive = 0;
		}
		return(0);
	}

	distance = (P->WorldPosition - target->WorldPosition) * WUTOFT;

	if((P == PlayerPlane) && (WeapStores.Aim7LoftOn) && (pDBWeaponList[P->WeapLoad[P->AI.cActiveWeaponStation].WeapIndex].iSeekerType == 7))
	{
		workdistance = distance;
		workdistance -= (5.0f * NMTOFT);
		if(workdistance > 0)
		{
			workangle = fabs(cos(DegToRad(pDBWeaponList[P->WeapLoad[P->AI.cActiveWeaponStation].WeapIndex].iSeekerFOV * 0.75f)));
			if(workangle)
				workdistance = (workdistance / workangle);
			workdistance += (5.0f * NMTOFT);
		}
	}

	avgvel = WGetLaunchAvgSpeed(weaponindex, distance, P->V, avgalt, &ftimeimpact);
	WGetLaunchInterceptPoint(P, weaponindex, target, avgvel, &intercept);

	distance = (P->WorldPosition - intercept) * WUTOFT;

	maxdist = (float)pDBWeaponList[weaponindex].iRange * CalcAltEffectOnMissile(P->WorldPosition.Y) * NMTOFT;

	if(distance > maxdist)
		distance = maxdist;

	avgvel = WGetLaunchAvgSpeed(weaponindex, distance, P->V, avgalt, &ftimeimpact);

	if(timeactive)
	{
		distance -= (10.0f * NMTOFT);
		if(distance < 0)
		{
			*timeactive = 0;
		}
		else
		{
			avgvel = WGetLaunchAvgSpeed(weaponindex, distance, P->V, avgalt, timeactive);
		}
	}

	return(ftimeimpact);
}

/*----------------------------------------------------------------------------
 *
 *	WSeekTarget()
 *
 */
void WSeekTarget(WeaponParams *W)
{
	FPointDouble Delta;
	float Dh, Dp;

	if(W->Flags2 & MISSILE_HOME_ON_JAM)
	{
		WHomeOnJammerSearch(W);
		return;
	}

	PlaneParams *P;
	PlaneParams *exclude;
	PlaneParams *foundplane = NULL;
	float drange, foundoffset, friendmod;
	double fdist;
	float tempheading, temppitch;
	DBWeaponType *pDBWeapon = &pDBWeaponList[ AIGetWeaponIndex(W->Type->TypeNumber) ];
	float offangle, offpitch;
	int weaponside = -1;

	P = &Planes[0];
	exclude = W->P;
	foundoffset = pDBWeapon->iSeekerFOV;
	friendmod = foundoffset * 0.2f;
	drange = (10 * 6076) / WUTOFT;

	if(W->P)
	{
		weaponside = W->P->AI.iSide;
	}

	if(pDBWeapon->iSeekerType == 7)
	{
		if(W->P != PlayerPlane)
		{
			W->Flags &= ~(ACTIVE_SEARCH|ACTIVE_SEEKER);
			return;
		}

		if (UFC.EMISState || Av.AARadar.SilentOn)
		{
			return;
		}

		fdist = (W->P->WorldPosition - W->Pos);
		if((fdist > (10.0f * NMTOWU)) || ((W->P->AADesignate) && (GetCurrentAARadarMode() == AA_STT_MODE)))
		{
			W->Flags &= ~(ACTIVE_SEARCH|ACTIVE_SEEKER);
			return;
		}
	}

	while(P <= LastPlane)
	{
		if ((P!=exclude) && (P->Status & PL_ACTIVE))
		{
			Delta.X = (P->WorldPosition.X - W->Pos.X);
			Delta.Y = (P->WorldPosition.Y - W->Pos.Y);
			Delta.Z = (P->WorldPosition.Z - W->Pos.Z);
			if((fabs(Delta.X) < drange) && (fabs(Delta.Z) < drange))
			{
				fdist = (double)QuickDistance(Delta.X,Delta.Z);
				if(fdist < drange)
				{
					Dh = atan2(-Delta.X, -Delta.Z) * RADIANS_TO_DEGREES;
					if (Dh > 180)  Dh -= 360;
					if (Dh < -180) Dh += 360;
					Dh -= AIConvertAngleTo180Degree(W->Heading);
					if (Dh > 180)  Dh -= 360;
 					if (Dh < -180) Dh += 360;
					Dh = -Dh;

					Dp = atan2(Delta.Y, fdist) * RADIANS_TO_DEGREES;	/* pi/2 */
					if (Dp > 180)  Dp -= 360;
					if (Dp < -180) Dp += 360;
					Dp -= AIConvertAngleTo180Degree(W->Pitch);
					if (Dp > 180)  Dp -= 360;
 					if (Dp < -180) Dp += 360;

					if(weaponside == P->AI.iSide)
					{
						if(Dh < 0)
						{
							Dh -= friendmod;
						}
						else
						{
							Dh += friendmod;
						}
						if (Dh > 180)  Dh -= 360;
 						if (Dh < -180) Dh += 360;

						if(Dp < 0)
						{
							Dp -= friendmod;
						}
						else
						{
							Dp += friendmod;
						}
						if (Dp > 180)  Dp -= 360;
 						if (Dp < -180) Dp += 360;
					}

					if((fabs(Dp) < foundoffset) && (fabs(Dh) < foundoffset))
					{
						if(pDBWeapon->iSeekerType == 3)
						{
							if(P->AI.Behaviorfunc != AIParked)
							{
								PLGetRelativeHP(P, W->Pos, &tempheading, &temppitch);
								if(!((fabs(tempheading) < 120) || (fabs(temppitch) > 60)))
								{
									if(WCheckIRAspect(pDBWeapon, W, P) > 0)
									{
										foundplane = P;
										foundoffset = fabs(Dp);
										if(fabs(Dh) > foundoffset)
										{
											foundoffset = fabs(Dh);
										}
									}
								}
							}
						}
						else if(pDBWeapon->iSeekerType == 7)
						{
							if((PlayerPlane->WorldPosition - P->WorldPosition) < (10.0f * NMTOWU))
							{
								PLGetRelativeHP(PlayerPlane, P->WorldPosition, &offangle, &offpitch);
								if((fabs(offangle) < 12.0f) && (fabs(offpitch) < 12.0f))
								{
									foundplane = P;
									foundoffset = fabs(Dp);
									if(fabs(Dh) > foundoffset)
									{
										foundoffset = fabs(Dh);
									}
								}
							}
						}
						else if(pDBWeapon->iSeekerType == 2)
						{
							if(P->AI.Behaviorfunc != AIParked)
							{
								if(WCheckIRAspect(pDBWeapon, W, P) > 0)
								{
									foundplane = P;
									foundoffset = fabs(Dp);
									if(fabs(Dh) > foundoffset)
									{
										foundoffset = fabs(Dh);
									}
								}
							}
						}
						else
						{
							if(!((P->Knots <= 60) || (P->FlightStatus & (PL_STATUS_CRASHED))))
							{
								foundplane = P;
								foundoffset = fabs(Dp);
								if(fabs(Dh) > foundoffset)
								{
									foundoffset = fabs(Dh);
								}
							}
						}
					}
				}
			}
		}
		P ++;
	}

	if(foundplane != NULL)
	{
		W->iTargetType = TARGET_PLANE;
		W->pTarget = foundplane;
		W->Flags &= ~(ACTIVE_SEARCH|ACTIVE_SEEKER);

		if(pDBWeapon->iSeekerType != 7)
		{
			W->Flags |= FINAL_PHASE;
		}

		W->lTimer = -1;

		if((pDBWeapon->iSeekerType == 2) || (pDBWeapon->iSeekerType == 3))
		{
			LockHeaterOnEngine(foundplane, W);
		}
//		if((MultiPlayer) && ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))
		if(MAIIsActiveWeapon(W))
		{
			SendNetWeaponTargetChange(W->iNetID, -2, W);
			SendNetWeaponTargetChange(W->iNetID, (foundplane - Planes) + 1, W);
		}
	}

	if(((pDBWeapon->iSeekerType == 2) || (pDBWeapon->iSeekerType == 3)) && (!MultiPlayer))
	{
		if((W->P != PlayerPlane) || (g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY))
		{
			if((HeatSeekerSunFactor(W->Heading, W->Pitch, W->Pos, 10.0f) > frand()) && (!(pDBWeapon->iECMResistance & ECM_DECOY)))
			{
				W->Flags |= WEP_SUN_TRACK;
			}
		}
	}
}

#else

//**************************************************************************
//****************   O L D   M I S S I L E   C O D E   *********************
//**************************************************************************
void MoveMissile(WeaponParams *W)
{
	FPointDouble Delta;
	FPointDouble NewPoint;
//	ANGLE NewH, NewP;

	if (W->P->AADesignate != NULL)
	{
		Delta.X = (W->P->AADesignate->WorldPosition.X - W->Pos.X);
		Delta.Y = (W->P->AADesignate->WorldPosition.Y - W->Pos.Y);
		Delta.Z = (W->P->AADesignate->WorldPosition.Z - W->Pos.Z);

		dH = atan2(-Delta.X, -Delta.Z) * RADIANS_TO_DEGREES;
		if (dH > 180)  dH -= 360;
		if (dH < -180) dH += 360;
		dH -= AIConvertAngleTo180Degree(W->Heading);
		if (dH > 180)  dH -= 360;
 		if (dH < -180) dH += 360;
		dH = -dH;

		dP = atan2(Delta.Y, (double)QuickDistance(Delta.X,Delta.Z)) * RADIANS_TO_DEGREES;	/* pi/2 */
		if (dP > 180)  dP -= 360;
		if (dP < -180) dP += 360;
		dP -= AIConvertAngleTo180Degree(W->Pitch);
		if (dP > 180)  dP -= 360;
 		if (dP < -180) dP += 360;

		W->Heading = (ANGLE)((ANGLE)W->Heading - (ANGLE)(dH*DEGREE));  // limit to 1/2 deg angle change/second
		W->Pitch   = (ANGLE)((ANGLE)W->Pitch   + (ANGLE)(dP*DEGREE));

	  	W->Attitude.SetHPR (W->Heading, W->Pitch, 0);

		NewPoint = W->Pos;

#if 1
		double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
		W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);

		NewPoint += W->Vel;

		if (WeaponHitsPlane(W))
		{
		 	DoExplosionInAir(W);
			DeleteMissile(W);
		}
		else if (WeaponIntersectsGround(W))
		{
			DoExplosionOnGround(W);
			DeleteMissile(W);
		}
	}
}
#endif

//============================================================================
//		COUNTERMEASURES CODE  --  INSTANTIATE / DELETE / MOVE
//============================================================================

/*----------------------------------------------------------------------------
 *
 *	InstantiateCounterMeasure()
 *
 */
void InstantiateCounterMeasure(PlaneParams *P, int Station, int cmtype, int netid)
{
	int Index;

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	//	MultiPlayer Remote Procedure Call
	int NetPutDecoy( PlaneParams *P, int cmtype );

	if (MultiPlayer && ((P==PlayerPlane) || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1)
	{
		if(netid == -1)
			NetCheckStationUpdated(P, Station);

		if(cmtype >= 0)
		{
			netid = NetPutDecoy( P, cmtype );
		}
		else
		{
			netid = NetPutDecoy( P, (Station + 16));
		}
	}
	else if((!MultiPlayer) || (netid == -1))  //  Check with Mike.  I think he was incrimenting this for debrief replay.  Screws up MultiPlayer if done this way hence added else condition
	{
		netid = g_iWeaponID++;
	}

	if(cmtype >= 0)
	{
		if(cmtype == 1)
		{
		    W->Type = pChaffType;
		}
		else if(cmtype == 2)
		{
		    W->Type = pFlareType;
		}
		else
		{
			DeleteCounterMeasure(W);
			return;
		}
		Index = AIGetWeaponIndex(W->Type->TypeNumber);
	}
	else
	{
		Index = P->WeapLoad[Station].WeapIndex;
	    W->Type = P->WeapLoad[Station].Type;
	}

	W->InitialVelocity = P->V*pDBWeaponList[Index].fDrag;

	W->Kind = BOMB;
	W->WeaponMove = MoveCounterMeasure;
	W->P = P;
	W->iNetID			= netid;

#if 1 //  counter measures should be less than 25 lbs for all so fudging in a big way.
	//  120 cm per container
	if((P == PlayerPlane) && (cPlayerLimitWeapons))
	{
		P->AircraftDryWeight -= 0.1667f;
		P->AI.fStoresWeight -= 0.1667f;
	}
	else if(P->Status & PL_AI_DRIVEN)
	{
		P->AI.fStoresWeight -= 0.1667f;
	}
	else if(cPlayerLimitWeapons)
	{
		P->AircraftDryWeight -= 0.1667f;
		P->AI.fStoresWeight -= 0.1667f;
	}
#else
	if((P == PlayerPlane) && (cPlayerLimitWeapons))
	{
		W->P->AircraftDryWeight -= pDBWeaponList[Index].iWeight;
	}
//	P->AI.fStoresWeight -= pDBWeaponList[Index].iWeight;
	P->AI.fStoresWeight -= 1;
#endif

	W->Flags = BOMB_INUSE;

	W->Vel.SetValues((float)HIGH_FREQ * FTTOWU,P->IfVelocity);

	W->Vel.X *= pDBWeaponList[Index].fDrag;
//	W->Vel.Y *= pDBWeaponList[Index].fDrag;
	W->Vel.Z *= pDBWeaponList[Index].fDrag;

	W->Pos = P->WorldPosition;


	W->GroundHeight		= 0.0f;

	W->lBurnTimer = 10000;  //  10 second diration.
	W->lTimer = -1;

	if(pDBWeaponList[Index].iWeaponType == WEAPON_TYPE_FLARE)
	{
		W->Smoke = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,COUNTER_FLARE_LIGHT);
	}
	else
	{
		if (P->Type && (P->Type->TypeNumber == 61))
		{
			FPoint novel(0.0f,0.0f,0.0f);

			W->Pos.AddScaledVector(4.5 FEET,P->Orientation.K);
			W->Pos.AddScaledVector(8.5 FEET,P->Orientation.I);

			NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, 1.0f/16.0f);

			W->Pos.AddScaledVector(4.5 FEET,P->Orientation.K);
			W->Pos.AddScaledVector(-2.5 FEET,P->Orientation.I);

		}
		W->Smoke = NULL;
	}

	W->SortType = BOMB_OBJECT;
//	GetSortHeader(W,BOMB_OBJECT);
}

/*----------------------------------------------------------------------------
 *
 *	DeleteCounterMeasure()
 *
 */
void DeleteCounterMeasure(WeaponParams *W)
{
	W->Flags = 0;
	W->Kind = NONE;
	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	}
	W->Smoke = NULL;

//	UnsortObject(W);

	if (W == LastWeapon)
		while((LastWeapon > &Weapons[-1]) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;
}

/*----------------------------------------------------------------------------
 *
 *	MoveCounterMeasure()
 *
 */
void MoveCounterMeasure(WeaponParams *W)
{
	float gravityeffect = 32.0f;
	int Index = AIGetWeaponIndex(W->Type->TypeNumber);

#if 0
	if(pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)].iWeaponType == WEAPON_TYPE_CHAFF)
	{
		gravityeffect = 8.0f;
	}
#endif

	W->LastPos = W->Pos;

	W->Vel.Y -= gravityeffect/(WUTOFT*2500.0);

	W->Pos += W->Vel;
	W->Pos.Y -= (0.5*(gravityeffect/(WUTOFT*2500.0)));

	W->lBurnTimer -= 20;

	if (WeaponIntersectsGround(W) || (W->lBurnTimer < 0))
	{
		CMDecoyClear(W);
		DeleteCounterMeasure(W);
	}
	else
	{
		if (W->Smoke)
			((SmokeTrail *)W->Smoke)->Update();

		W->lTimer -= 20;
		if(W->lTimer < 0)
		{
			W->lTimer = 1000;
			CMDecoyCheck(W);
		}
	}

}

/*----------------------------------------------------------------------------
 *
 *	CMCheckIsStillDecoyed()
 *
 */
void CMCheckIsStillDecoyed(WeaponParams *W)
{
	float offhead, ohperc, genperc, goalperc, fworkval;
	int weaponindex, seekertype;
	int isresistant = 0;
	PlaneParams *planepnt;
	int itemp;

//	if((W->pTarget == NULL) || (W->iTargetType != TARGET_PLANE) || ((MultiPlayer) && ((W->P != PlayerPlane) && (!(W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))))
	if((W->pTarget == NULL) || (W->iTargetType != TARGET_PLANE) || ((MultiPlayer) && !MAIIsActiveWeapon(W)))
	{
		return;
	}

	weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
	seekertype = pDBWeaponList[weaponindex].iSeekerType;
	planepnt = (PlaneParams *)W->pTarget;

	if((seekertype == 1) || (seekertype == 7))
	{
		if(pDBWeaponList[weaponindex].iECMResistance & ECM_CHAFF)
		{
			isresistant = 1;
		}
	}
	else if((seekertype == 2) || (seekertype == 3))
	{
		if(pDBWeaponList[weaponindex].iECMResistance & ECM_FLARE)
		{
			isresistant = 1;
		}
		if((planepnt->RightThrustPercent > 80) || (planepnt->LeftThrustPercent > 80))
		{
			if(seekertype == 3)
			{
				W->pDecoyed = NULL;
				if(MultiPlayer)
				{
					SendNetWeaponTargetChange(W->iNetID, -1, W);
				}
				return;
			}
			else
			{
				offhead = AIConvertAngleTo180Degree(W->Heading - planepnt->Heading);
				if(fabs(offhead) < 95)
				{
					W->pDecoyed = NULL;
					if(MultiPlayer)
					{
						SendNetWeaponTargetChange(W->iNetID, -1, W);
					}
					return;
				}
			}
		}
	}

	offhead = AIConvertAngleTo180Degree(W->Heading - planepnt->Heading);
	if(fabs(offhead) < 65)
	{
		ohperc = 1.3333f; //  was 0.85	// was 1.25 (I think I went the wrong way percent wise)
	}
	else if(fabs(offhead) > 115)
	{
		ohperc = 3.0f;  //2.0f  was 0.75	// was 1.25 (I think I went the wrong way percent wise)
	}
	else
	{
		ohperc = 1.0f;
	}

	genperc = 0.20f;

	goalperc = ohperc * genperc;

	if(isresistant)
	{
		goalperc *= 2;
		if(pDBWeaponList[weaponindex].iECMResistance & ECM_DECOY)  //  Enhanced seeker
		{
			goalperc *= 1.5;
		}
	}

	if((W->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY)))
	{
		goalperc *= 2;
	}

	itemp = rand() & 127;
	fworkval = (float)itemp / 127.0f;

	if(fworkval <= goalperc)
	{
		W->pDecoyed = NULL;
		if(MultiPlayer)
		{
			SendNetWeaponTargetChange(W->iNetID, -1, W);
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	CMDecoyCheck()
 *
 */
void CMDecoyCheck(WeaponParams *decoy)
{
	int decoyindex, decoytype;
	int weaponindex, seekertype;
	int docheck;
	float dx, dy, dz, tdist;
	float offpitch, offangle;
	float toffpitch, toffangle;
	PlaneParams *planepnt;
	float offhead;
	int isresistant;
	float ohperc, genperc, goalperc, fworkval;
	int itemp;

	decoyindex = AIGetWeaponIndex(decoy->Type->TypeNumber);
	decoytype = pDBWeaponList[decoyindex].iWeaponType;

	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
//		if ((W->Flags & WEAPON_INUSE) && (W->Kind == MISSILE) && ((!MultiPlayer) || ((W->P == PlayerPlane) || (W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))))
		if ((W->Flags & WEAPON_INUSE) && (W->Kind == MISSILE) && ((!MultiPlayer) || MAIIsActiveWeapon(W)))
		{
			if(W->pTarget && (W->iTargetType == TARGET_PLANE) && (W->pDecoyed == NULL))
			{
				docheck = 0;
				isresistant = 0;
				weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
				seekertype = pDBWeaponList[weaponindex].iSeekerType;
				planepnt = (PlaneParams *)W->pTarget;
				if((decoytype == WEAPON_TYPE_CHAFF) && ((seekertype == 1) || (seekertype == 7)))
				{
					if(pDBWeaponList[weaponindex].iECMResistance & ECM_CHAFF)
					{
						isresistant = 1;
					}
					docheck = 1;
				}
				else if((decoytype == WEAPON_TYPE_FLARE) && ((seekertype == 2) || (seekertype == 3)))
				{
					if(pDBWeaponList[weaponindex].iECMResistance & ECM_FLARE)
					{
						isresistant = 1;
					}
					if((planepnt->RightThrustPercent > 80) || (planepnt->LeftThrustPercent > 80))
					{
						if(seekertype == 3)
						{
							docheck = 0;
						}
						else
						{
							offhead = AIConvertAngleTo180Degree(W->Heading - planepnt->Heading);
							if(fabs(offhead) < 95)
							{
								docheck = 0;
							}
							else
							{
								docheck = 1;
							}
						}
					}
					else
					{
						docheck = 1;
					}
				}
				if(docheck)
				{
					dx = (planepnt->WorldPosition.X - W->Pos.X);
					dy = (planepnt->WorldPosition.Y - W->Pos.Y);
					dz = (planepnt->WorldPosition.Z - W->Pos.Z);

					tdist = (double)QuickDistance(dx, dz);

					toffangle = atan2(-dx, -dz) * RADIANS_TO_DEGREES;
					if (toffangle > 180)  toffangle -= 360;
 					if (toffangle < -180) toffangle += 360;
					toffangle -= AIConvertAngleTo180Degree(W->Heading);
					if (toffangle > 180)  toffangle -= 360;
 					if (toffangle < -180) toffangle += 360;
					if(toffangle < 0)
					{
						toffangle -= 1;
					}
					else
					{
						toffangle += 1;
					}


					toffpitch = atan2(dy, tdist) * RADIANS_TO_DEGREES;	/* pi/2 */
					if (toffpitch > 180)  toffpitch -= 360;
 					if (toffpitch < -180) toffpitch += 360;
					toffpitch -= AIConvertAngleTo180Degree(W->Pitch);
					if (toffpitch > 180)  toffpitch -= 360;
 					if (toffpitch < -180) toffpitch += 360;
					if(toffpitch < 0)
					{
						toffpitch -= 1;
					}
					else
					{
						toffpitch += 1;
					}


					dx = (decoy->Pos.X - W->Pos.X);
					dy = (decoy->Pos.Y - W->Pos.Y);
					dz = (decoy->Pos.Z - W->Pos.Z);

					tdist = (double)QuickDistance(dx, dz);

					offangle = atan2(-dx, -dz) * RADIANS_TO_DEGREES;
					offangle -= AIConvertAngleTo180Degree(W->Heading);
					if (offangle > 180)  offangle -= 360;
 					if (offangle < -180) offangle += 360;


					offpitch = atan2(dy, tdist) * RADIANS_TO_DEGREES;	/* pi/2 */
					offpitch -= AIConvertAngleTo180Degree(W->Pitch);
					if (offpitch > 180)  offpitch -= 360;
 					if (offpitch < -180) offpitch += 360;

					if((fabs(offpitch) < fabs(toffpitch)) && (fabs(offangle) < fabs(toffangle)) && (fabs(offpitch) < 60.0f) && (fabs(offangle) < 60))
					{
						offhead = AIConvertAngleTo180Degree(W->Heading - planepnt->Heading);
						if(fabs(offhead) < 65)
						{
							ohperc = 1.333;	//  was 0.85  // was 1.25 (I think I went the wrong way percent wise)
						}
						else if(fabs(offhead) > 115)
						{
							ohperc = 3.0f;	//2.0f  was 0.75  // was 1.5 (I think I went the wrong way percent wise)
						}
						else
						{
							ohperc = 1.0f;
						}

						genperc = 0.20f;

						goalperc = ohperc * genperc;

						if(isresistant)
						{
							goalperc *= 2;
							if(pDBWeaponList[weaponindex].iECMResistance & ECM_DECOY)  //  Enhanced seeker
							{
								goalperc *= 1.5f;
							}
						}

						if((W->P == PlayerPlane) && (!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_MISSILE_ACCURACY)))
						{
							goalperc *= 2;
						}

						itemp = rand() & 127;
						fworkval = (float)itemp / 127.0f;

						if(fworkval > goalperc)
						{
							W->pDecoyed = decoy;
							if(MultiPlayer)
							{
								SendNetWeaponTargetChange(W->iNetID, decoy->iNetID, W);
							}
						}
					}
				}
			}
		}
		W++;
	}
}

/*----------------------------------------------------------------------------
 *
 *	CMDecoyClear()
 *
 */
void CMDecoyClear(WeaponParams *decoy)
{
	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
	{
		if (W->pDecoyed == decoy)
		{
			W->pDecoyed = NULL;
			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, -1, W);
			}
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	CMDropChaff()
 *
 */
void CMDropChaff(PlaneParams *planepnt)
{
	int cnt;
	int weapontype;
	int placeingroup;

	if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) - 1;
		if(iAIChaffTotal[placeingroup] > 0)
		{
			InstantiateCounterMeasure(planepnt, -1, 1);
			iAIChaffTotal[placeingroup] = iAIChaffTotal[placeingroup] - 1;
		}
	}
	else
	{
		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			weapontype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
			if(weapontype == WEAPON_TYPE_CHAFF)
			{
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					InstantiateCounterMeasure(planepnt, cnt);
					planepnt->WeapLoad[cnt].Count --;
					return;
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------
 *
 *	CMDropFlare()
 *
 */
void CMDropFlare(PlaneParams *planepnt)
{
	int cnt;
	int weapontype;
	int placeingroup;

	// flares handled differently for player plane
	if(planepnt == PlayerPlane)
	{
		if(FlareTotal > 0)
		{
			InstantiateCounterMeasure(planepnt,CHAFF_FLARE,2);
			if(cPlayerLimitWeapons)
				FlareTotal--;
		}
	}
	else if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) - 1;
		if(iAIChaffTotal[placeingroup] > 0)
		{
			InstantiateCounterMeasure(planepnt, -1, 2);
			iAIFlareTotal[placeingroup] = iAIFlareTotal[placeingroup] - 1;
		}
	}
	else
	{
		for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
		{
			weapontype = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
			if(weapontype == WEAPON_TYPE_FLARE)
			{
				if(planepnt->WeapLoad[cnt].Count > 0)
				{
					InstantiateCounterMeasure(planepnt, cnt);
					planepnt->WeapLoad[cnt].Count --;
					return;
				}
			}
		}
    }
}


//*******************************************************************************************************************
//*
//*	Air To Ground Missile Stuff
//*
//*******************************************************************************************************************
void InstantiateAGMissile(PlaneParams *P, int Station, void *GroundTarget, FPoint targetloc, int groundtype, int netid, int flightprofile)
{
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	int targtype;
	void *targetpnt;
	int weaponindex;
	double sinroll, cosroll, aoadeg, aoaoffset;
	float planeroll;
	int iworkvar;
	DBWeaponType *pweapon_type;
	double dx, dy, dz, tdist, toffpitch;
	MovingVehicleParams *vehiclepnt;

	if(!(P->Status & PL_ACTIVE))
		return;

	if ((int)P->WeapLoad[Station].Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)P->WeapLoad[Station].Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(P->WeapLoad[Station].Type->TypeNumber)];
	}

	if((pweapon_type->iWeaponType == WEAPON_TYPE_GUIDED_BOMB) || (pweapon_type->lWeaponID == 118))
	{
		InstantiateBomb(P, Station , (1000 * (1.0f/5.7435)), GroundTarget, groundtype, targetloc.X, targetloc.Y, targetloc.Z, netid);
		return;
	}

	if(pweapon_type->iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE)
	{
	 	if(groundtype != MOVINGVEHICLE)
		{
			GroundTarget = NULL;
		}
		else
		{
			vehiclepnt = (MovingVehicleParams *)GroundTarget;
			if(vehiclepnt->iShipType == 0)
			{
				GroundTarget = NULL;
			}
		}
	}

	if(netid == -1)
		NetCheckStationUpdated(P, Station);

//	if(WCheckWeaponNowPlane(P, AIRCRAFT, GroundTarget, groundtype, targetloc, netid, Station, flightprofile))
	if((g_bIAmHost) || (netid == -1) || (!MultiPlayer))
	{
		if(WCheckWeaponNowPlane(P, AIRCRAFT, GroundTarget, groundtype, targetloc, netid, Station, flightprofile))
		{
			if((!g_bIAmHost) && (netid == -1) && (MultiPlayer))
			{
				if (MultiPlayer && (P==PlayerPlane || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1)
					netid = NetPutAGMissileGeneral(P, Station, GroundTarget, targetloc, groundtype, flightprofile);
			}
			return;
		}
	}
	else if((!g_bIAmHost) && (netid != -1) && (MultiPlayer))
	{
		if(WGetWeaponToPlaneID(pweapon_type) != -1)
			return;
	}

	//  I think I want this after WCheckWeaponNowPlane since that function will do the same thing as below
	if (MultiPlayer && (P==PlayerPlane || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1)
		netid = NetPutAGMissileGeneral(P, Station, GroundTarget, targetloc, groundtype, flightprofile);
	else if((!MultiPlayer) || (netid == -1))  //  Check with Mike.  I think he was incrimenting this for debrief replay.  Screws up MultiPlayer if done this way hence added else condition
		netid = g_iWeaponID++;

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	if(P == PlayerPlane)
	{
		if(!AICheckForMoreAG(PlayerPlane, NULL))
		{
			iPlayerBombFlags &= ~WSO_BOMB_HIT;
		}
	}

	W->Flags = MISSILE_INUSE;
	W->Flags2 = 0;

	targtype = TARGET_GROUND;

	if (GroundTarget)
	{
		if(groundtype == AIRCRAFT)
		{
			targtype = TARGET_PLANE;
			targetpnt = GroundTarget;
			W->iTargetType = targtype;
			W->pTarget = targetpnt;
			if(MultiPlayer)
			{
				SendNetWeaponTargetChange(W->iNetID, ((PlaneParams *)targetpnt - Planes) + 1, W);
			}
		}
		else
		{
			if(groundtype == MOVINGVEHICLE)
			{
				targtype = TARGET_VEHICLE;
				iworkvar = WGetStrongestShipRadar((MovingVehicleParams *)GroundTarget);
				if(iworkvar >= 0)
				{
					W->Flags2 = iworkvar;
				}
			}
			targetpnt = GroundTarget;
			W->iTargetType = targtype;
			W->pTarget = targetpnt;
		}
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}
	else if (targetloc.X >= 0)
	{
		targetpnt = NULL;
		W->iTargetType = targtype;
		W->pTarget = targetpnt;
		W->fpTempWay = targetloc;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}
	else
	{
		W->iTargetType = NONE;
		W->pTarget = NULL;
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}

	W->Kind = MISSILE;
	W->WeaponMove = MoveAGMissile;

	AICheckROE(P, 2);
	W->P = P;
	W->iNetID			= netid;
	if(pweapon_type->iWeaponType != WEAPON_TYPE_AGROCKET)
	{
		if((P == PlayerPlane) && (cPlayerLimitWeapons))
		{
			P->AircraftDryWeight -= pweapon_type->iWeight;
			P->AI.fStoresWeight -= pweapon_type->iWeight;
		}
		else if(P->Status & PL_AI_DRIVEN)
		{
			P->AI.fStoresWeight -= pweapon_type->iWeight;
		}
		else if(cPlayerLimitWeapons)
		{
			P->AircraftDryWeight -= pweapon_type->iWeight;
			P->AI.fStoresWeight -= pweapon_type->iWeight;
		}

		if((pSeatData) && (P == PlayerPlane))
		{
			if(Station <= LEFT5_STATION)
			{
				pSeatData->WeaponReleaseSide |= 2;
			}
			else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
			{
				pSeatData->WeaponReleaseSide |= 1;
			}
			else
			{
				pSeatData->WeaponReleaseSide |= 0x3;
			}
			pSeatData->WeaponReleaseWeight += pweapon_type->iWeight;
			pSeatData->WeaponReleaseID = pweapon_type->lWeaponID;
		}
	}
	else
	{
		float weight;
		if(pweapon_type->lWeaponID == 133)
		{
			weight = 131;
		}
		else
		{
			weight = 21;
		}
		if((P == PlayerPlane) && (cPlayerLimitWeapons))
		{
			P->AircraftDryWeight -= weight;
			P->AI.fStoresWeight -= weight;
		}
		else if(P->Status & PL_AI_DRIVEN)
		{
			P->AI.fStoresWeight -= weight;
		}
		else if(cPlayerLimitWeapons)
		{
			P->AircraftDryWeight -= weight;
			P->AI.fStoresWeight -= weight;
		}

		if((pSeatData) && (P == PlayerPlane))
		{
			if(Station <= LEFT5_STATION)
			{
				pSeatData->WeaponReleaseSide |= 2;
			}
			else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
			{
				pSeatData->WeaponReleaseSide |= 1;
			}
			else
			{
				pSeatData->WeaponReleaseSide |= 0x3;
			}
			pSeatData->WeaponReleaseWeight += weight;
			pSeatData->WeaponReleaseID = pweapon_type->lWeaponID;
		}
	}

	W->InitialVelocity = P->V/(WUTOFT*50.0);

	W->Launcher = P;
	W->LauncherType = AIRCRAFT;
    W->Type = P->WeapLoad[Station].Type;
	W->Attitude = P->Attitude;
	W->Heading = P->Heading;
	W->Pitch = P->Pitch;

	//  Moved up top to counter Jay passing in Guided bombs as missiles
	//  What the heck, let's keep this here as well, just in case.
	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	//  Start stuff to make missiles go down
	if(P == PlayerPlane)
	{
		planeroll = AIConvertAngleTo180Degree(P->Roll);
		aoadeg = RadToDeg(P->Alpha);
		if(aoadeg > 5.0)
		{
			aoadeg = 5.0;
		}
		if(aoadeg < -5.0)
		{
			aoadeg = -5.0;
		}
		cosroll = cos(DegToRad(fabs(planeroll)));
		aoaoffset = cosroll * (double)aoadeg;
		W->Pitch -= AIConvert180DegreeToAngle(aoaoffset);

		sinroll = sin(DegToRad(fabs(planeroll)));
		aoaoffset = sinroll * (double)aoadeg;
		W->Heading -= AIConvert180DegreeToAngle(aoaoffset);

		W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );

	}
	//  End stuff to make missiles go down

	W->Vel.X = P->Orientation.I.X*W->InitialVelocity;
	W->Vel.Y = P->Orientation.I.Y*W->InitialVelocity;
	W->Vel.Z = P->Orientation.I.Z*W->InitialVelocity;

	W->Pos = GetHardPointPos( P, Station );
	WeaponInitAltitude(W);
	W->GroundHeight	= 0.0f;

	if((pweapon_type->iWeaponType == WEAPON_TYPE_AGROCKET) && (P->Status & PL_AI_DRIVEN))
	{
		if((pweapon_type->lWeaponID == 134) && (P))
		{
			if(P->AI.iAICombatFlags2 & AI_FAC)
			{
				if(!AICheckForMoreAG(P, W))
				{
					AIC_GenericFACCall(P - Planes, ((rand() & 1) * 3));
				}
			}
		}

		dx = P->AI.TargetPos.X - P->WorldPosition.X;
		dy = P->AI.TargetPos.Y - P->WorldPosition.Y;
		dz = P->AI.TargetPos.Z - P->WorldPosition.Z;

		tdist = sqrt((dx * dx) + (dz * dz));
		toffpitch = atan2(dy, tdist);

		toffpitch *= 57.2958;
		W->Pitch = AIConvert180DegreeToAngle(toffpitch);

		W->fpTempWay = P->AI.WayPosition;
	}

	if(!((pweapon_type->iWeaponType == WEAPON_TYPE_AGROCKET) || (pweapon_type->lWeaponID == 3) ||  (pweapon_type->lWeaponID == 4) || (pweapon_type->lWeaponID == 88) || (pweapon_type->lWeaponID == 110) || (pweapon_type->lWeaponID == 111)))
	{
		float downvel = W_DROP_SPEED/(WUTOFT*50.0);

		W->Vel.X += P->Orientation.K.X * downvel;
		W->Vel.Y += P->Orientation.K.Y * downvel;
		W->Vel.Z += P->Orientation.K.Z * downvel;
		W->Flags2 |= MISSILE_DROP_PHASE;
		W->lTimer = 1000;
	}

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	AIC_WSO_Fox_Msgs(P, W);

	LogWeaponInstantiate(P,W);
	if (P==PlayerPlane)
	{
		int iLevel=g_iSoundLevelExternalSFX-60;
		if (iLevel<0) iLevel=0;
		if (iLevel)
		{
			SndQueueSound(Radthum,1,iLevel);
		}
	}


	weaponindex = P->WeapLoad[Station].WeapIndex;

	W->lBurnTimer = pDBWeaponList[weaponindex].iBurnTime * 1000;
	if((pDBWeaponList[weaponindex].iRocketMotor) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
	{
		W->Smoke = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);

	}
	else
	{
		W->Smoke = NULL;
	}
	W->lTargetDistFt = -1;
	W->pDecoyed = NULL;

	if (P==PlayerPlane)
	{
		if(!(W->Flags2 & MISSILE_DROP_PHASE))
		{
			int iVolumeMod=0;
			int iSoundID=ltmiss;
			int iLevel=g_iSoundLevelExternalSFX-60;

			switch(pDBWeaponList[weaponindex].lWeaponID)
			{
				case 3:
				case 4:
					iLevel=g_iSoundLevelExternalSFX-60;
					iSoundID=Rocketi;
				break;
			}

			if (iLevel<0) iLevel=0;

			if (iLevel)
			{
				SndQueueSound(iSoundID,1,iLevel);
			}
		}

		W->Flags |= COLLISION_CHECK;
		W->CollisionCheckTimer = 0;
		W->AccelOrientation = W->Pos;
	}

	if (P == PlayerPlane)
	{
		//  do for HARM (87) or skipper (116)

		if((pweapon_type->lWeaponID == 87) && (Av.Harm.Mode == HARM_PB_MODE))
		{
			W->Flags2 |= MISSILE_GPS_ONLY;
		}

		if((!Cpit.SimJettOn) && ((pweapon_type->lWeaponID == 87) || (((pweapon_type->lWeaponID == 116) || (pweapon_type->lWeaponID == 110)) && (!((lBombFlags & WSO_BOMB_IMPACT) || (P->AI.iAIFlags2 & AI_RIPPLE_BOMB))))))
		{
			FPointDouble TargetPos;
			float distance;
			int tempval = 0;
			float avgalt = (P->WorldPosition.Y) * WUTOFT;
			float avgvel;
			float Time;
			float maxdist;

			int FlightProfile;
			int GroundTargetType;
			void *Trgt;

			GetWeaponData(Av.Weapons.CurAGWeap->WeapPage,&FlightProfile,&Trgt,&GroundTargetType,&TargetPos);

			if(TargetPos.X >= 0)
			{
				distance = (TargetPos - PlayerPlane->WorldPosition) * WUTOFT;
			}
			else
			{
				distance = (float)pDBWeaponList[P->WeapLoad[Station].WeapIndex].iRange * CalcAltEffectOnMissile(P->WorldPosition.Y) * NMTOFT;
			}

			avgvel = MachAToFtSec((20000.0f), (float)pweapon_type->fMaxSpeed);  //  WGetLaunchAvgSpeed(P->WeapLoad[Station].WeapIndex, distance, P->V, avgalt, &ftimeimpact);

			maxdist = (float)pDBWeaponList[weaponindex].iRange * CalcAltEffectOnMissile(P->WorldPosition.Y) * NMTOFT;

			if(distance > maxdist)
				distance = maxdist;

			Time = distance / avgvel;

			Time += (float)W->lBurnTimer / 2000.0f;

			if(W->Flags2 & MISSILE_DROP_PHASE)
			{
				Time += 1.0f;
			}

			Time += 10.0f;  //  Just a fudge factor.

			if(pweapon_type->lWeaponID == 87)
			{
				lHARMToF = (long)(Time * 1000);
				lHARMLaunched = GameLoopInTicks;
			}
			else
			{
				lBombFlags |= WSO_BOMB_IMPACT;
				lBombTimer = (long)(Time * 1000);

				if((pweapon_type->iSeekerType == 6 /*laser*/) && AreMpdsInGivenMode(TGT_IR_MODE) && (UFC.MasterMode != AA_MODE))
				{
		  			Av.Flir.TimeToImpact = lBombTimer;
      	  			Av.Flir.TimpactTimer.Set(3.0,GameLoopInTicks);
				}
			}
		}
	}

	if(P)
	{
		PlaneHasAAWeapons(P);
		PlaneHasAGWeapons(P);
	}


#ifdef WATCHFALL
	setup_no_cockpit_art();
	if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
	{
		Camera1.CameraMode = CAMERA_FREE;
		ChangeViewModeOrSubject(CAMERA_FREE);
	}
	else
	 	ChangeViewModeOrSubject(Camera1.CameraMode);
	placeholder = InstantiatePlaceHolder(P->WorldPosition);
	agpoint = InstantiatePlaceHolder(P->AGDesignate);
#endif
}

//*******************************************************************************************************************
void MoveAGMissile(WeaponParams *W)
{
	PlaneParams *planepnt = NULL;
	long ldistft, ldistnm;
	int timerchange = 0;
	int ballistic = 0;
	int realweapons = 1;
	int weaponindex;
	PlaneParams *targplane = NULL;
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	double ddist;
	FPoint position;
	BasicInstance *target;
	double desiredspeed;
	float speedinc = 0;
	double dP;
	MovingVehicleParams *vtarget;
	PlaneParams *ptarget;
	DBWeaponType *pweapon_type;
	int iworkvar;
	FPointDouble fpd_position;

	position.SetValues(-1.0f, -1.0f, -1.0f);

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
	maxyaw = (float)pDBWeaponList[weaponindex].iMaxYawRate / 50.0f;
	maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

	if((W->Flags & BOOST_PHASE) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
	{
		desiredspeed = MachAToFtSec((20000.0f), (float)pDBWeaponList[weaponindex].fMaxSpeed)/(WUTOFT*50.0);
//		desiredspeed *= CalcAltEffectOnMissile(W->Pos.Y);

		if (W->InitialVelocity < desiredspeed)
		{
			speedinc = (float)pDBWeaponList[weaponindex].iAcceleration/(WUTOFT*2500.0);
			if (W->LifeTime < (2000.0f * FTTOWU))
			{
				speedinc *= 4;
			}

			W->InitialVelocity += speedinc;
		}

		W->lBurnTimer -= 20;

		if (W->lBurnTimer < 0)
		{
			if (W->Smoke)
				((SmokeTrail *)W->Smoke)->MakeOrphan();
			W->Smoke = NULL;

			W->Flags &= ~(BOOST_PHASE);
			W->Flags |= GLIDE_PHASE;
		}
	}

	if(W->Flags2 & MISSILE_DROP_PHASE)
	{
		W->lTimer -= 20;
		if(W->lTimer < 0)
		{
			W->Flags2 &= ~(MISSILE_DROP_PHASE);
			if((pweapon_type->iRocketMotor) && (!(W->Flags2 & MISSILE_DROP_PHASE)))
			{
				W->Smoke = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);
			}

			if(W->P == PlayerPlane)
			{
				int iVolumeMod=0;
				int iSoundID=ltmiss;
				int iLevel=g_iSoundLevelExternalSFX-60;

				switch(pweapon_type->lWeaponID)
				{
					case 3:
					case 4:
						iLevel=g_iSoundLevelExternalSFX-60;
						iSoundID=Rocketi;
					break;
				}

				if (iLevel<0) iLevel=0;

				if (iLevel)
				{
					SndQueueSound(iSoundID,1,iLevel);
				}
			}

			W->lTimer = 0;
		}
	}

	if(pweapon_type->iWeaponType == WEAPON_TYPE_AGROCKET)
	{
		ballistic = 1;
	}

	if((W->P == PlayerPlane) && (pweapon_type->iSeekerType == 6))
	{
		if((lLaserStatusFlags & WSO_LASER_MASKED) || (!(lLaserStatusFlags & WSO_LASER_IS_ON)))
		{
			target = NULL;
//			fpd_position = position;
			if(!WFoundOtherLaser(W, pweapon_type->iSeekerType, &fpd_position))
			{
				target = NULL;
				W->fpTempWay.X = -1.0f;
				position = W->fpTempWay;
				if(W->fpTempWay.X < 0)
				{
					ballistic = 1;
				}
			}
			else
			{
				position = fpd_position;
				W->fpTempWay = position;
				W->GroundHeight = position.Y;
			}
		}
		else
		{
			position = LaserLoc;
			if((LaserLoc.X <= 0) && (LaserLoc.Y <= 0) && (LaserLoc.Z <= 0))
			{
				FPointDouble FlirPos;
				if( GetFlirTargetPosition(&FlirPos) )
				{
					position = FlirPos;
				}
			}
			W->fpTempWay = position;
			W->GroundHeight = position.Y;
		}
	}

	if((!W->pTarget) && (W->fpTempWay.X < 0))
	{
		ballistic = 1;
	}
	else if ((W->iTargetType == TARGET_GROUND) || (W->iTargetType == TARGET_VEHICLE) || (W->iTargetType == TARGET_PLANE) || (W->fpTempWay.X >= 0))
	{
		if (W->pTarget)
		{
			if(W->iTargetType == TARGET_GROUND)
			{
				target = (BasicInstance *)W->pTarget;
				position.X = target->Position.X;
				position.Y = target->Position.Y;
				position.Z = target->Position.Z;
				W->GroundHeight = position.Y;
			}
			else if(W->iTargetType == TARGET_PLANE)
			{
				ptarget = (PlaneParams *)W->pTarget;
				position = ptarget->WorldPosition;
				W->GroundHeight = position.Y;
			}
			else
			{
				vtarget = (MovingVehicleParams *)W->pTarget;
				position = vtarget->WorldPosition;
				if((vtarget->iShipType) && (pweapon_type->iSeekerType == 11))
				{
					iworkvar = W->Flags2 & ANTI_RAD_SHIP_MASK;
					if(iworkvar < NUM_DEFENSE_LIST_ITEMS)
					{
						fpd_position = position;
						VGDGetWeaponPosition(vtarget, MOVINGVEHICLE, iworkvar, &fpd_position);
						position = fpd_position;
					}
				}
				W->GroundHeight = position.Y;
			}
		}
		else
		{
			target = NULL;
			position = W->fpTempWay;
			if(W->fpTempWay.X < 0)
			{
				ballistic = 1;
			}
		}

		Delta.X = (position.X - W->Pos.X);
		Delta.Y = (position.Y - W->Pos.Y);
		Delta.Z = (position.Z - W->Pos.Z);

		ddist = (double)QuickDistance(Delta.X, Delta.Z);

		ldistft = ddist * WUTOFT;
		ldistnm = ldistft / 6076;
		W->lTargetDistFt = ldistft;


#if 0
		if ((ddist > fabs(Delta.Y)) && (ddist > (30000 * FTTOWU)))
		{
			Delta.Y += (10000.0f * FTTOWU);
		}
#else
		if ((ddist > fabs(Delta.Y * 3)) && (ddist > (12000 * FTTOWU)))
		{
			if(fabs(Delta.Y) < (1200.0f * FTTOWU))
			{
				Delta.Y += (1200.0f * FTTOWU);
			}
			else
			{
				Delta.Y = 0;
			}

		}
#endif

		if (W->LifeTime < (2000.0f * FTTOWU))
		{
			ballistic = 1;
			if(W->LauncherType == AIRCRAFT)
			{
				planepnt = (PlaneParams *)W->Launcher;
				if(planepnt != PlayerPlane)
				{
					if((W->LifeTime < (250.0f * FTTOWU)) && (pweapon_type->iWeaponType != WEAPON_TYPE_AGROCKET))
					{
						dP = 5.0f - AIConvertAngleTo180Degree(W->Pitch);
						if (dP >  180)  dP -= 360;
					 	if (dP < -180)  dP += 360;
						if (dP < -maxyaw)  dP = -maxyaw;
						if (dP >  maxyaw)  dP =  maxyaw;
						W->Pitch   += (ANGLE)(dP*DEGREE);
						W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );
					}
				}
			}
		}

#ifdef WATCHFALL
		FPointDouble relative_position;
		FPoint ImpactPoint;

		Camera1.CameraLocation.X = W->Pos.X;
		Camera1.CameraLocation.Y = W->Pos.Y;
		Camera1.CameraLocation.Z = W->Pos.Z;

		relative_position.MakeVectorToFrom(position,Camera1.CameraLocation);
		relative_position.Normalize();

		Camera1.Heading = AIConvert180DegreeToAngle(atan2(relative_position.X,relative_position.Z) * 57.2958) + 0x8000;
		Camera1.Pitch = AIConvert180DegreeToAngle(asin(relative_position.Y) * 57.2958);
		if(placeholder)
		{
			placeholder->Pos = position;
			placeholder->LifeTime = 2000;
			if(agpoint)
			{
				agpoint->LifeTime = 2000;
			}
		}
#endif

	}
	else
	{
		ballistic = 1;
	}

	if((W->LifeTime > (2000.0f * FTTOWU)) && (pweapon_type->iSeekerType == 11))
	{
		W->lTimer -= 20;
		if((W->lTimer < 0) && (!(W->Flags2 & MISSILE_GPS_ONLY)))
		{
			WCheckAntiRadarLock(W);
		}
	}

	 //	End of pre-calc, do the missile's flight
	//
	if (!ballistic)
	{
		newAttitude(W);
	}
	else
	{
		if(W->Pos.Y > (80000.0f * FTTOWU))
		{
			goto Killer;
		}
	}

#if 0  //  I don't think we need this anymore
	double dx, dy, dz, tdist, toffpitch;

	if((pweapon_type->iWeaponType == WEAPON_TYPE_AGROCKET) && (W->P) && (W->P->Status & PL_AI_DRIVEN))
	{
		dx = position.X - W->Pos.X;
		dy = position.Y - W->Pos.Y;
		dz = position.Z - W->Pos.Z;

		tdist = sqrt((dx * dx) + (dz * dz));
		toffpitch = atan2(dy, tdist) * 57.2958;
		W->Pitch = AIConvert180DegreeToAngle(toffpitch);
		tdist *= WUTONM;
	}
#endif

	if(!(W->Flags2 & MISSILE_DROP_PHASE))
	{
#if 1
		double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
		W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
		W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif

		W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);
	}

	W->LastPos = W->Pos;	// HitPlane() & WeaponIntersectsGround() need this
	W->Pos += W->Vel;

	if (W->Smoke)  ((SmokeTrail *) W->Smoke )->Update();

	planepnt = WeaponHitsPlane(W);
	if(planepnt)
	{
		DoExplosionInAir(W, 0, NULL, NULL, planepnt);
		goto Killer;
	}

	if(((pweapon_type->iSeekerType == 11) && (W->Pos.Y < W->GroundHeight)) || (WeaponIntersectsGround(W)))
	{
		if((W->pTarget) && (pweapon_type->iSeekerType == 11))
		{
			WKillRadar(W);
		}

		if (!(W->Flags & COLLISION_CHECK) || !CheckMissileAgainstGroundTargets(W))
			DoExplosionOnGround(W);
		goto Killer;
	}
	else
		if ((W->Flags & COLLISION_CHECK) && (W->Altitude < (500.0 FEET)))
		{
//			float yvel = fabs(W->Vel.Y);

//			if ((yvel < fabs(W->Vel.X * 0.6)) || (yvel < fabs(W->Vel.Z * 0.6)))
//			{
				if (/*!W->CollisionCheckTimer-- &&*/ CheckMissileAgainstGroundTargets(W))
					goto Killer;
//			}
//			else
//				W->CollisionCheckTimer = 0;
		}
//		else
//			W->CollisionCheckTimer = 0;


	// update total distance traveled
	W->LifeTime += sqrt( (W->Vel.X*W->Vel.X) + (W->Vel.Y*W->Vel.Y) + (W->Vel.Z*W->Vel.Z) );

	if (W->LifeTime > pDBWeaponList[weaponindex].iRange * NMTOWU)
	{
		if (W->Smoke)
		{
			((SmokeTrail *)W->Smoke)->Update();
			((SmokeTrail *)W->Smoke)->MakeOrphan();
		}
		W->Smoke = NULL;


		W->WeaponMove = MoveBombs;
//		goto Killer;
	}

	return;		// normal exit

   //------------------------
  //
 //	Delete the spent missile.
//
Killer:
	DeleteMissile(W);
}

/*----------------------------------------------------------------------------
 *
 *	WGetAGLaunchAvgSpeed()
 *
 */
float WGetAGLaunchAvgSpeed(int weaponindex, float distft, float startfps, float avgaltft, float *time)
{
	double avgboostfps;
	double avgburnfps, burndist, burnsecs;
	double avgsustainfps, sustaindist, sustainsecs;
	double avgglidefps, glidedist, glidesecs;
	double avgendfps, enddist, endsecs;
	double totaldist = 0;
	double workval;
	double accelfps = pDBWeaponList[weaponindex].iAcceleration;
	double maxspeedfps = MachAToFtSec(20000, pDBWeaponList[weaponindex].fMaxSpeed);
//	maxspeedfps *= CalcAltEffectOnMissile(avgaltft * FTTOWU);
	double minspeedfps = maxspeedfps / 3;
	double timetomin;
	double burntime = pDBWeaponList[weaponindex].iBurnTime;
	double boostsecs, boostspeedfps;
	double boostdist = 2000.0f;  //   / CalcAltEffectOnMissile(avgaltft * FTTOWU);
	double timetomax;

	if(!accelfps)
	{
		accelfps = 1.0f;
	}
	if(!maxspeedfps)
	{
		maxspeedfps = 1.0f;
	}

	timetomax = (maxspeedfps - startfps) / accelfps;

	boostsecs = (-startfps + sqrt((startfps * startfps) + (2 * 4 * accelfps * boostdist))) / (accelfps * 4);

	if(timetomax > boostsecs)
	{
		timetomax -= boostsecs;
	}
	else
	{
		timetomax = (maxspeedfps - startfps) / (accelfps * 4);
		boostsecs = timetomax;
		boostdist = (startfps * timetomax) + (0.5f * accelfps * 4 * timetomax * timetomax);
	}




	*time = 0;

	if(distft < boostdist)
	{
		boostsecs = (-startfps + sqrt((startfps * startfps) + (2 * 4 * accelfps * distft))) / (accelfps * 4);
		maxspeedfps = startfps + (accelfps * boostsecs);
		avgboostfps = (maxspeedfps + startfps) * 0.5;
		*time = boostsecs;
		return(avgboostfps);
	}

	if(!boostsecs)
	{
		boostsecs = 1.0f;
	}

	*time = *time + boostsecs;
	totaldist += boostdist;

	boostspeedfps = startfps + (accelfps * boostsecs * 4);
	avgboostfps = startfps + (accelfps * boostsecs * 2.0f);

	if(burntime < timetomax)
	{
		burnsecs = burntime - boostsecs;
		if(burnsecs > 0)
		{
			burndist = (boostspeedfps * burnsecs) + (0.5f * accelfps * burnsecs * burnsecs);
			maxspeedfps = boostspeedfps + (accelfps * burnsecs);
			avgburnfps = boostspeedfps + (accelfps * burnsecs * 0.5f);
		}
		else
		{
			maxspeedfps = boostspeedfps;
			burndist = 0;
			avgburnfps = boostspeedfps + (accelfps * 4 * boostsecs);
		}

		avgsustainfps = maxspeedfps;
		sustaindist = 0;
		sustainsecs = 0;
	}
	else
	{
		burnsecs = timetomax - boostsecs;
		burndist = (boostspeedfps * burnsecs) + (0.5f * accelfps * burnsecs * burnsecs);
		avgburnfps = boostspeedfps + (accelfps * burnsecs * 0.5f);
	}

	if((totaldist + burndist) > distft)
	{
		burnsecs = (-boostspeedfps + sqrt((boostspeedfps * boostspeedfps) + (2 * accelfps * (distft - totaldist)))) / accelfps;
		maxspeedfps = boostspeedfps + (accelfps * burnsecs);
		avgburnfps = (maxspeedfps + boostspeedfps) * 0.5f;
		*time = *time + burnsecs;
		workval = ((avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (burnsecs + boostsecs);
		return(workval);
	}

	*time = *time + burnsecs;
	totaldist += burndist;

	if(burntime > timetomax)
	{
		avgsustainfps = maxspeedfps;
		sustainsecs = burntime - timetomax;
		sustaindist = sustainsecs * avgsustainfps;
	}

	if((totaldist + sustaindist) > distft)
	{
		sustaindist = distft - totaldist;
		sustainsecs = sustaindist / maxspeedfps;

		workval = ((avgsustainfps * sustainsecs) + (avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (sustainsecs + burnsecs + boostsecs);
		*time = *time + sustainsecs;
		return(workval);
	}

	*time = *time + sustainsecs;
	totaldist += sustaindist;

	timetomin = (maxspeedfps - minspeedfps) / 32.0f;
	glidesecs = timetomin;
	glidedist = (maxspeedfps * timetomin) + (-16.0f * timetomin * timetomin);
	avgglidefps = (maxspeedfps + minspeedfps) * 0.5f;

	if((totaldist + glidedist) > distft)
	{
		glidedist = distft - totaldist;

		glidesecs = (-maxspeedfps + sqrt((maxspeedfps * maxspeedfps) + (2 * -32 * glidedist))) / -32;
		workval = maxspeedfps + (-32.0f * glidesecs);
		avgglidefps = (maxspeedfps + workval) * 0.5f;
		workval = ((avgglidefps * glidesecs) + (avgsustainfps * sustainsecs) + (avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (glidesecs + sustainsecs + burnsecs + boostsecs);
		*time = *time + glidesecs;
		return(workval);
	}

	*time = *time + glidesecs;
	totaldist += glidedist;

	if(!minspeedfps)
	{
		minspeedfps = 1.0f;
	}

	avgendfps = minspeedfps;
	enddist = distft - totaldist;
	endsecs = enddist / avgendfps;

	*time = *time + endsecs;

	workval = ((avgendfps * endsecs) + (avgglidefps * glidesecs) + (avgsustainfps * sustainsecs) + (avgburnfps * burnsecs) + (avgboostfps * boostsecs)) / (endsecs + glidesecs + sustainsecs + burnsecs + boostsecs);
	return(workval);
}



//*******************************************************************************************************************
//*
//*	Surface to Surface Missile Stuff (Ground to Ground)
//*
//*******************************************************************************************************************
void InstantiateSSMissile(void *launcher, int launchertype, void *GroundTarget, int targettype, FPoint targetloc, int netid, int radardatnum, int flightprofile)
{
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	void *targetpnt;
	BasicInstance *walker = NULL;
	MovingVehicleParams *vehiclepnt = NULL;
	FPointDouble position;
	FPointDouble targetpos;
	FPointDouble workpos;
	float offangle, tdist, offpitch;
	double ybuffer = 50.0f;
	DBWeaponType *pDBWeapon;
	FPointDouble weaponoffset;
	int fxindex = -1;

	if (MultiPlayer && (lAINetFlags1 & NGAI_ACTIVE) && netid==-1)
		netid = NetPutSSMissileGeneral((MovingVehicleParams *)launcher, radardatnum, GroundTarget, targetloc, targettype, flightprofile);
	else if((!MultiPlayer) || (netid == -1))  //  Check with Mike.  I think he was incrimenting this for debrief replay.  Screws up MultiPlayer if done this way hence added else condition
		netid = g_iWeaponID++;

//	if(WCheckWeaponNowPlane(launcher, launchertype, GroundTarget, targettype, targetloc, netid, radardatnum, flightprofile))
	if(WCheckWeaponNowPlane(launcher, launchertype, GroundTarget, targettype, targetloc, -1, radardatnum, flightprofile))
	{
		return;
	}

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	W->Flags = MISSILE_INUSE;
	W->Flags2 = 0;

	W->LaunchStation = -1;
	if (GroundTarget)
	{
		targetpnt = GroundTarget;
		if(targettype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)GroundTarget;
			targetpos = vehiclepnt->WorldPosition;
			W->iTargetType = TARGET_VEHICLE;
		}
		else
		{
			walker = (BasicInstance *)GroundTarget;
			targetpos = walker->Position;
			W->iTargetType = TARGET_GROUND;
		}
		W->pTarget = targetpnt;
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}
	else if (targetloc.X >= 0)
	{
		targetpnt = NULL;
		W->iTargetType = 0;
		W->pTarget = targetpnt;
		W->fpTempWay = targetloc;
		targetpos = targetloc;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}
	else
	{
		W->iTargetType = NONE;
		W->pTarget = NULL;
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE;
		W->lTimer = 0;
		W->LifeTime = 0;
		targetpos = W->fpTempWay;
	}

	W->Kind = MISSILE;
	W->WeaponMove = LaunchSSMissile;

	W->P = NULL;
	W->iNetID			= netid;
	W->InitialVelocity = 0;

	W->Launcher = launcher;
	W->LauncherType = launchertype;
	W->LaunchStation = radardatnum;
	if(launchertype == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)launcher;
		radardat = &vehiclepnt->RadarWeaponData[radardatnum];
		position = vehiclepnt->WorldPosition;

		if(vehiclepnt->iShipType)
		{
		}
		else
		{
			if(radardatnum == 1)
			{
				fxindex = 0;
			}
		}

		VGDGetWeaponPosition(launcher, launchertype, radardatnum, &weaponoffset);
		position += weaponoffset;
	}
	else
	{
		walker  = (BasicInstance *)launcher;
		radardat = GDGetRadarData(walker);
		position = walker->Position;
	}
	pDBWeapon = &pDBWeaponList[ radardat->iWeaponIndex ];
	W->Type = radardat->WType;

	GroundLaunchMissileSound(pDBWeapon,position);


	workpos = targetpos;
	workpos -= position;

	offangle = atan2(-workpos.X, -workpos.Z) * RADIANS_TO_DEGREES;

	W->Heading = AIConvert180DegreeToAngle(offangle);

	if((fxindex != -1) && (vehiclepnt))
	{
		BYTE bdesiredangle;

		offangle -= AIConvertAngleTo180Degree(vehiclepnt->Heading);

		offangle = AICapAngle(offangle);

		while(offangle < 0)
		{
			offangle += 180;
		}

		offangle = (offangle / 360.0f) * 255.0f;

		bdesiredangle = offangle;
		bdesiredangle += 128;
		vehiclepnt->bFXDesiredArray[fxindex] = bdesiredangle;
	}

	tdist = QuickDistance(workpos.X,workpos.Z);

//	offpitch = atan2(workpos.Y, tdist) * RADIANS_TO_DEGREES;
	if(pDBWeapon->iMaxAlt <= 0)
	{
		offpitch = atan2(workpos.Y, tdist) * RADIANS_TO_DEGREES;	/* pi/2 */
		W->WeaponMove = MoveAGMissile;
	}
	else if(pDBWeapon->lWeaponID == 101)
	{
		offpitch = 85;
	}
	else
	{
		offpitch = 45;
	}

	if(pDBWeapon->iVerticalLaunch)
	{
		offpitch = 85;
		W->Flags2 |= MISSILE_VERTICAL_PHASE;
	}

	W->Pitch = AIConvert180DegreeToAngle(offpitch);
	W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );

#if 1
	double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
	W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);

	if(position.Y < 0)
	{
		position.Y = 10.0f * FTTOWU;
	}

	W->Pos = position;
	WeaponInitAltitude(W);
	W->GroundHeight	= 0.0f;

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	if((launchertype == MOVINGVEHICLE) || (launchertype == GROUNDOBJECT))
	{
		if(pDBWeapon->iMaxAlt <= 0)
		{
			W->Pos.Y += (6.0f * FTTOWU);
		}
		else
		{
			W->Pos.Y += (ybuffer * FTTOWU);
		}
	}

	W->lBurnTimer = pDBWeapon->iBurnTime * 1000;
	if (pDBWeapon->iRocketMotor)
	{
		W->Smoke = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);

	}
	else
	{
		W->Smoke = NULL;
	}

	W->lTargetDistFt = -1;
	W->pDecoyed = NULL;

#ifdef WATCHFALL
	setup_no_cockpit_art();
	if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
	{
		Camera1.CameraMode = CAMERA_FREE;
		ChangeViewModeOrSubject(CAMERA_FREE);
	}
	else
	 	ChangeViewModeOrSubject(Camera1.CameraMode);
	placeholder = InstantiatePlaceHolder(position);
	agpoint = InstantiatePlaceHolder(position);
#endif
}

//*******************************************************************************************************************
void LaunchSSMissile(WeaponParams *W)
{
	PlaneParams *planepnt = NULL;
	long ldistft, ldistnm;
	int timerchange = 0;
	int ballistic = 0;
	int realweapons = 1;
	int weaponindex;
	PlaneParams *targplane = NULL;
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	FPointDouble position;
	BasicInstance *target;
	double desiredspeed;
	float speedinc = 0;
	MovingVehicleParams *vtarget;
	float desiredpitch;
	FPointDouble junkpos;
	double  ddist;

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
	maxyaw = (float)pDBWeaponList[weaponindex].iMaxYawRate / 50.0f;
	maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

	if (W->Flags & BOOST_PHASE)
	{
		desiredspeed = MachAToFtSec((20000.0f), (float)pDBWeaponList[weaponindex].fMaxSpeed)/(WUTOFT*50.0);
//		desiredspeed *= CalcAltEffectOnMissile(W->Pos.Y);

		if (W->InitialVelocity < desiredspeed)
		{
			speedinc = (float)pDBWeaponList[weaponindex].iAcceleration/(WUTOFT*2500.0);
			if (W->LifeTime < (2000.0f * FTTOWU))
			{
				speedinc *= 4;
			}

			W->InitialVelocity += speedinc;
		}

		W->lBurnTimer -= 20;

		if (W->lBurnTimer < 0)
		{
			if (W->Smoke)
				((SmokeTrail *)W->Smoke)->MakeOrphan();
			W->Smoke = NULL;

			W->Flags &= ~(BOOST_PHASE);
			W->Flags |= GLIDE_PHASE;
		}
	}

	if(W->Flags2 & MISSILE_VERTICAL_PHASE)
	{
		ballistic = 1;
		if(W->LifeTime > 1000.0f)
		{
			W->Flags2 &= ~(MISSILE_VERTICAL_PHASE);
		}
	}
	else if ((W->iTargetType == TARGET_GROUND) || (W->iTargetType == TARGET_VEHICLE))
	{
		if (W->pTarget)
		{
			if(W->iTargetType == TARGET_GROUND)
			{
				target = (BasicInstance *)W->pTarget;
				position.X = target->Position.X;
				position.Y = target->Position.Y;
				position.Z = target->Position.Z;
				W->GroundHeight = position.Y;
			}
			else
			{
				vtarget = (MovingVehicleParams *)W->pTarget;
				position = vtarget->WorldPosition;
				W->GroundHeight = position.Y;
			}
		}
		else
		{
			target = NULL;
			position = W->fpTempWay;
		}

		Delta.X = (position.X - W->Pos.X);
		Delta.Y = (position.Y - W->Pos.Y);
		Delta.Z = (position.Z - W->Pos.Z);

		ddist = (double)QuickDistance(Delta.X, Delta.Z);

		ldistft = ddist * WUTOFT;
		ldistnm = ldistft / 6076;
		W->lTargetDistFt = ldistft;


		if ((ddist < fabs(Delta.Y * 3)) || (ddist < (13000 * FTTOWU)))
		{
//			if(LOS(&W->Pos, &position, &junkpos, MED_RES_LOS, FALSE))
//			{
				W->WeaponMove = MoveAGMissile;
//			}
			desiredpitch = 0;
		}
		else if(W->Type->TypeNumber == 101)
		{
			desiredpitch = 60;
		}
		else
		{
			desiredpitch = 45;
		}

		if(((W->Pos.Y * WUTOFT) > pDBWeaponList[weaponindex].iMaxAlt) && (pDBWeaponList[weaponindex].iMaxAlt > 0))
		{
			desiredpitch = 0;
		}

		if (W->LifeTime < (6000.0f * FTTOWU))
		{
			ballistic = 1;
		}
#ifdef WATCHFALL
		FPointDouble relative_position;
		FPoint ImpactPoint;

		Camera1.CameraLocation.X = W->Pos.X;
		Camera1.CameraLocation.Y = W->Pos.Y;
		Camera1.CameraLocation.Z = W->Pos.Z;

		relative_position.MakeVectorToFrom(position,Camera1.CameraLocation);
		relative_position.Normalize();

		Camera1.Heading = AIConvert180DegreeToAngle(atan2(relative_position.X,relative_position.Z) * 57.2958) + 0x8000;
		Camera1.Pitch = AIConvert180DegreeToAngle(asin(relative_position.Y) * 57.2958);
		if(placeholder)
		{
			placeholder->Pos = position;
			placeholder->LifeTime = 2000;
			if(agpoint)
			{
				agpoint->LifeTime = 2000;
			}
		}
#endif

	}
	else
	{
		ballistic = 1;
		if(!(W->Flags & BOOST_PHASE))
		{
			W->WeaponMove = MoveBombs;
		}
	}

	 //	End of pre-calc, do the missile's flight
	//
	if (!ballistic)
	{
		double  dH, dP;

		dH = atan2(-Delta.X, -Delta.Z) * RADIANS_TO_DEGREES;

		if (dH >  180)  dH -= 360;
		if (dH < -180)  dH += 360;

		dH -= AIConvertAngleTo180Degree(W->Heading);

		if (dH >  180)  dH -= 360;
 		if (dH < -180)  dH += 360;

		dH = -dH;

		if (dH < -maxyaw)  dH = -maxyaw;
		if (dH >  maxyaw)  dH =  maxyaw;

		dP = desiredpitch;

		dP -= AIConvertAngleTo180Degree(W->Pitch);

		if (dP >  180)  dP -= 360;
 		if (dP < -180)  dP += 360;

		if (dP < -maxyaw)  dP = -maxyaw;
		if (dP >  maxyaw)  dP =  maxyaw;

		W->Heading -= (ANGLE)(dH*DEGREE);  // limit to 1/2 deg angle change/second
		W->Pitch   += (ANGLE)(dP*DEGREE);

		W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );
	}

#if 1
	double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
	W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);

	W->LastPos = W->Pos;	// HitPlane() & WeaponIntersectsGround() need this
	W->Pos += W->Vel;

	if (W->Smoke)  ((SmokeTrail *) W->Smoke )->Update();

	planepnt = WeaponHitsPlane(W);
	if(planepnt)
	{
		DoExplosionInAir(W, 0, NULL, NULL, planepnt);
		goto Killer;
	}

	if (WeaponIntersectsGround(W))
	{
		DoExplosionOnGround(W);
		goto Killer;
	}

	// update total distance traveled
	W->LifeTime += sqrt( (W->Vel.X*W->Vel.X) + (W->Vel.Y*W->Vel.Y) + (W->Vel.Z*W->Vel.Z) );

	if (W->LifeTime > pDBWeaponList[weaponindex].iRange * NMTOWU)
		goto Killer;

	return;		// normal exit

   //------------------------
  //
 //	Delete the spent missile.
//
Killer:
	DeleteMissile(W);
}


//============================================================================
//		HIT DETECTION CODE  --
//============================================================================
/*----------------------------------------------------------------------------
 *
 *	WeaponInitAltitude()
 *
 */
void WeaponInitAltitude( WeaponParams *W )
{
	double landheight;
	double orgheight;
	MovingVehicleParams *vehiclepnt;
	BasicInstance *walker;

	orgheight = landheight = LandHeight( W->Pos.X, W->Pos.Z);
	if((landheight < W->GroundHeight) && (!(InHighResTerrainRegion(W->Pos))))
	{
		if(W->pTarget)
		{
			if(W->iTargetType == TARGET_GROUND)
			{
				walker = (BasicInstance *)W->pTarget;
				W->GroundHeight = walker->Position.Y;
			}
			else if(W->iTargetType == TARGET_VEHICLE)
			{
				vehiclepnt = (MovingVehicleParams *)W->pTarget;
				if(vehiclepnt->WorldPosition.Y < W->GroundHeight)
				{
					W->GroundHeight = vehiclepnt->WorldPosition.Y;
				}
			}
		}

		if(landheight < W->GroundHeight)
		{
			landheight = W->GroundHeight;
		}
	}

	W->Altitude = W->Pos.Y - landheight;
}

/*----------------------------------------------------------------------------
 *
 *	WeaponIntersectsGround()
 *
 */
BOOL WeaponIntersectsGround( WeaponParams *W )
{
	double old_alt = W->Altitude;
	double landheight;
	double orgheight;
	MovingVehicleParams *vehiclepnt;
	BasicInstance *walker;

	orgheight = landheight = LandHeight( W->Pos.X, W->Pos.Z);
	if((landheight < W->GroundHeight) && (!(InHighResTerrainRegion(W->Pos))))
	{
		if(W->pTarget)
		{
			if(W->iTargetType == TARGET_GROUND)
			{
				walker = (BasicInstance *)W->pTarget;
				W->GroundHeight = walker->Position.Y;
			}
			else if(W->iTargetType == TARGET_VEHICLE)
			{
				vehiclepnt = (MovingVehicleParams *)W->pTarget;
				if(vehiclepnt->WorldPosition.Y < W->GroundHeight)
				{
					W->GroundHeight = vehiclepnt->WorldPosition.Y;
				}
			}
		}

		if(landheight < W->GroundHeight)
		{
			landheight = W->GroundHeight;
		}
	}

	W->Altitude = W->Pos.Y - landheight;

	if (W->Altitude > 0.0)
	{
		return FALSE;	// above the ground, NO intersection
	}
	else if(MultiPlayer)
	{
		if((W->LifeTime < (1000.0f * FTTOWU)) && ((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT)))
		{
			W->Pos.Y = landheight + (100.0f * WUTOFT);
			W->Altitude = 100.0f * WUTOFT;
			return FALSE;	// above the ground, NO intersection
		}
	}

	double dDAlt = (old_alt - W->Altitude);

	if(!dDAlt)
		dDAlt = 1;

	double dT = old_alt / dDAlt;

	// back up partway to LastPos
	W->Pos -= W->LastPos;
	W->Pos *= dT;
	W->Pos += W->LastPos;

	return TRUE;
}


/*----------------------------------------------------------------------------
 *
 *	CheckIntersection()
 *
 */
BOOL CheckIntersection(FPointDouble &Line1P1,FPointDouble &Line1P2,FPointDouble &Line2P1,FPointDouble &Line2P2,float radius)
{
	FPointDouble L1P1,L1P2;
	FPointDouble L2P1,L2P2;

	 if (IntersectLines3D(Line1P1,Line1P2,Line2P1,Line2P2,NULL,NULL,&L1P1,&L2P1,&L1P2,&L2P2))
		if (((L1P1-L2P1) <= (double)radius) || ((L1P2-L2P2) <= (double)radius))
			return TRUE;

	return FALSE;
}

/*----------------------------------------------------------------------------
 *
 *	WeaponHitsPlane()
 *
 */
PlaneParams *WeaponHitsPlane( WeaponParams *W , int *bullethit)
{
	int do_check;
	int orgflightstat;
	double d;
	FPointDouble new_point;
	PlaneParams *StartPlane = Planes;
	PlaneParams *EndPlane = LastPlane;
	long tempdamage,tempstatus ,tempflight;
	WeaponParams *checkweapon;
	double tempdist;
	float pgunmod = 1.0f;
	float pmissilemod = 1.0f;
	float tempmod;

	double distance = W->Pos - W->LastPos;

	if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_WEAPON_DAMAGE))
	{
		pmissilemod = 2.0f;
	}

	if(!(g_Settings.gp.dwWeapons & GP_WEAPON_REAL_GUN_ACCURACY))
	{
		pgunmod = 2.0f;
	}

	if(W->Kind == EJECTION_SEAT)
	{
		if(bullethit)
		{
			*bullethit = 0;
		}
		for (checkweapon = Weapons; checkweapon <= LastWeapon; checkweapon++)
		{
			if (checkweapon->Flags & WEAPON_INUSE)
			{
				if((checkweapon->P == PlayerPlane) && (checkweapon->Kind != EJECTION_SEAT))
				{
					tempdist = checkweapon->Pos - checkweapon->LastPos;
					d = distance + tempdist;

					if(checkweapon->Kind == BULLET)
					{
						tempmod = pgunmod;
						d *= tempmod;
					}
					else if(checkweapon->Kind == MISSILE)
					{
						tempmod = pmissilemod;
						d *= tempmod;
					}
					else
					{
						tempmod = 1.0f;
					}


					if (fabs(W->Pos.X - checkweapon->Pos.X) < d)
					{
						if (fabs(W->Pos.Z - checkweapon->Pos.Z) < d)
						{
							if (fabs(W->Pos.Y - checkweapon->Pos.Y) < d)
							{
								do_check = 0;
 								if (((checkweapon->LastPos-W->LastPos) <= tempdist) || ((checkweapon->LastPos-W->Pos) <= tempdist))
									do_check = 1;

								if (do_check)
								{
									if (CheckIntersection(W->LastPos,W->Pos,checkweapon->LastPos,checkweapon->Pos,(20.0f * FTTOWU * tempmod)))
									{
										if(bullethit)
										{
											*bullethit = 1;
										}
										return PlayerPlane;
									}
								}
							}
						}
					}
				}
			}
		}
	}

//	if((MultiPlayer) && (W->P != PlayerPlane) && (!(W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && (!((W->P == NULL) && (lAINetFlags1 & NGAI_ACTIVE))))
	if((MultiPlayer) && (W->P != PlayerPlane) && (!(W->P && (W->P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && (W->P != NULL))
	{
		return(NULL);
	}

//	if((iDoAllAAA == 0) && (W->WeaponMove == MoveBullets) && ((W->LauncherType == GROUNDOBJECT) || (W->LauncherType == MOVINGVEHICLE)))
//	{
	if ((W->iTargetType == TARGET_PLANE) && (W->pTarget))
	{
		StartPlane = (PlaneParams *)W->pTarget;
		EndPlane = (PlaneParams *)W->pTarget;
	}

	for (PlaneParams *P=StartPlane; P<=EndPlane; P++)
	{
		if (P == W->P)  continue;

		if (!(P->Status & PL_ACTIVE))  continue;

		if(WIsWeaponPlane(P) && (P->AI.LinkedPlane == W->P))
		{
			continue;
		}

		if(MultiPlayer)
		{
			if((W->P == NULL) && (!((P== PlayerPlane) || (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))))	continue;
		}

		d = distance + P->DistanceMoved;

		if(W->P == PlayerPlane)
		{
			if(W->Kind == BULLET)
			{
				tempmod = pgunmod;
				d *= tempmod;
			}
			else if(W->Kind == MISSILE)
			{
				tempmod = pmissilemod;
				d *= tempmod;
			}
			else
			{
				tempmod = 1.0f;
			}
		}
		else
		{
			tempmod = 1.0f;
		}

		if (fabs(W->Pos.X - P->WorldPosition.X) < d)
		{
			if (fabs(W->Pos.Z - P->WorldPosition.Z) < d)
			{
				if (fabs(W->Pos.Y - P->WorldPosition.Y) < d)
				{
					do_check = 0;
					if (distance > P->DistanceMoved)
					{
	 					if (((W->LastPos-P->LastWorldPosition) <= (distance * tempmod)) || ((W->LastPos-P->WorldPosition) <= (distance * tempmod)))
							do_check = 1;
					}
					else
	 					if (((P->LastWorldPosition-W->LastPos) <= (P->DistanceMoved * tempmod)) || ((P->LastWorldPosition-W->Pos) <= (P->DistanceMoved * tempmod)))
							do_check = 1;

					if (do_check)
					{
						if ((DWORD)W->Type > 0x400)
  							AINearMiss( P, W->P );
//						else
//							AIAAANearMiss(P,(AAWeaponInstance *)W->Launcher);


						if ((CheckIntersection(W->LastPos,W->Pos,P->LastWorldPosition,P->WorldPosition,P->Type->ShadowLRZOff * tempmod)) || (!P->V))
						{
							new_point.MakeVectorToFrom( W->Pos, W->LastPos );

							orgflightstat = P->FlightStatus;

#if 1
							int hit_value;
							DBWeaponType *pweapon_type;
							float fBlastRadius;
							float fMultiplier, fDamageValue;
							FPoint vel;

							if ((int)W->Type < 0x200)
								pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
							else
								pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];

							hit_value = DTE_DIRECT_HIT;
							hit_value |= (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];
							fBlastRadius = (pweapon_type->iDamageRadius * FTTOWU);
							fMultiplier = ((float)pweapon_type->iDamageValue) / (fBlastRadius * fBlastRadius);
							fDamageValue = fBlastRadius * fBlastRadius * fMultiplier;

							tempdamage = P->DamageFlags;
							tempstatus = P->SystemsStatus;
							tempflight = P->FlightStatus;


							//Not quite working right
							DamagePlane(P, hit_value, GameLoop, fDamageValue, new_point, fBlastRadius, W, pweapon_type);
							if( tempdamage != P->DamageFlags ||
							  ((tempstatus & PL_ENGINES ) != (P->DamageFlags & PL_ENGINES))||
							  ((tempflight & (PL_STATUS_CRASHED| PL_PLANE_BLOWN_UP |PL_OUT_OF_CONTROL)) != (P->FlightStatus &(PL_STATUS_CRASHED| PL_PLANE_BLOWN_UP |PL_OUT_OF_CONTROL))) )
							{
								FPoint tmp(0.0f);// = P->IfVelocity;
								NewCanister( CT_FLYING_PARTS, P->WorldPosition, tmp, 0.0f );
							}

							if(W->Kind == MISSILE)
							{
								vel.MakeVectorToFrom(P->WorldPosition,P->LastWorldPosition);
								NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
							}
#else
							if ((P->Orientation.K * new_point) <= 0.0)
								CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT, W);
							else
								CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_LEFT,  W);
#endif

							AICCheckForHitMsgs( P, W->P, orgflightstat, W );

							return P;
						}
					}
				}
			}
		}
	}
	return NULL;
}


void AAAHitsPlane( AAAStreamVertex *W )
{
	int do_check;
	int orgflightstat;
	double d;
	double damage;
	FPointDouble new_point;
	FPointDouble center;
	FPointDouble last_position;
	FPointDouble damage_vector;
	GDRadarData *radardat;
	PlaneParams *StartPlane = Planes;
	PlaneParams *EndPlane = LastPlane;

	if((W->Flags & AAA_NO_CHECK) && (iDoAllAAA == 0))
	{
		return;
	}

	if(W->Gun)
	{
		radardat = GDGetRadarData((BasicInstance *)W->Gun);
		if(radardat->Target)
		{
			StartPlane = EndPlane = (PlaneParams *)radardat->Target;
			if(MultiPlayer)
			{
				if(!((StartPlane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (StartPlane == PlayerPlane)))
				{
					return;
				}
			}
		}
		else if(MultiPlayer)
		{
			if(!(lAINetFlags1 & NGAI_ACTIVE))
			{
				return;
			}
		}
	}

	center = W->NextVertex->Position;
	center += W->Position;
	center *= 0.5;

	last_position = W->NextVertex->Position;

	double distance = (last_position - W->Position)*0.5;

	for (PlaneParams *P=StartPlane; P<=EndPlane; P++)
	{
		if (!(P->Status & PL_ACTIVE))  continue;

		d = distance + P->DistanceMoved;

		if (fabs(center.X - P->WorldPosition.X) < d)
		{
			if (fabs(center.Z - P->WorldPosition.Z) < d)
			{
				if (fabs(center.Y - P->WorldPosition.Y) < d)
				{
					do_check = 0;
					if (distance > P->DistanceMoved)
					{
	 					if (((last_position - P->LastWorldPosition) <= distance) || ((last_position - P->WorldPosition) <= distance))
							do_check = 1;
					}
					else
	 					if (((P->LastWorldPosition - last_position) <= P->DistanceMoved) || ((P->LastWorldPosition - W->Position) <= P->DistanceMoved))
							do_check = 1;

					if (do_check)
					{
//						AIAAANearMiss(P,W);
						if (CheckIntersection(last_position,W->Position,P->LastWorldPosition,P->WorldPosition,P->Type->ShadowLRZOff))
						{
							orgflightstat = P->FlightStatus;

							new_point.MakeVectorToFrom( W->Position, last_position );
							new_point.Normalize();

							damage_vector.MakeVectorToFrom(P->WorldPosition,P->LastWorldPosition);
							damage_vector.Normalize();

							damage = 0.5 + 5.0 * fabs(damage_vector * new_point);

#if 1
							int hit_value;
							DBWeaponType *pweapon_type;
							float fBlastRadius;
							float fMultiplier, fDamageValue;

							pweapon_type = W->Weapon;

							hit_value = DTE_DIRECT_HIT;
							hit_value |= (DWORD)WarHeadTypeConversion[pweapon_type->iWarHead];
							fBlastRadius = (pweapon_type->iDamageRadius * FTTOWU);
							fMultiplier = ((float)pweapon_type->iDamageValue) / (fBlastRadius * fBlastRadius);
							fDamageValue = fBlastRadius * fBlastRadius * fMultiplier;
							fDamageValue *= damage;

							DamagePlane(P, hit_value, GameLoop, fDamageValue, new_point, fBlastRadius, NULL, pweapon_type);
#else

							if ((P->Orientation.K * new_point) <= 0.0)
								CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT, NULL);
							else
								CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_LEFT,  NULL);
#endif

							AICCheckForDamageMsgs(P, orgflightstat, (P->FlightStatus & (PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT|PL_ENGINE_REAR_LEFT)), NULL, W, W->Weapon);
						}
					}
				}
			}
		}
	}
}

//This is a good line intersects polygon function
//void AAAHitsPlane( AAAStreamVertex *W )
//{
//	int do_check;
//	int orgflightstat;
//	int still_good;
//	double NegD;
//	double d;
//	double a,b,c;
//	int code;
//	int i;
//	FPointDouble intersect;
//	FPointDouble AAANormal;
//	FPointDouble AAACorners[4];
//	FPointDouble AAAVectors[4];
//	FPointDouble work1,work2;
//	FPointDouble AAAcenter;
//	FPointDouble *from_vector;
//	FPointDouble *from_point;
//	FPointDouble *to_point;

//	FPointDouble new_point;

//	double distance;

//	AAAcenter = (AAACorners[0] = W->Position);
//	AAAcenter += (AAACorners[1] = W->LastPosition);
//	AAAcenter += (AAACorners[2] = W->NextVertex->LastPosition);
//	AAAcenter += (AAACorners[3] = W->NextVertex->Position);

//	AAAcenter *= 0.25;
//	distance = AAAcenter-W->Position;

//	AAANormal.MakeVectorToFrom(W->LastPosition,W->Position);
//	work1.MakeVectorToFrom(W->NextVertex->Position,W->Position);

//	AAANormal %= work1;
//	AAANormal.Normalize();
//	NegD = AAANormal * W->Position;

//	a = fabs(AAANormal.X);
//	b = fabs(AAANormal.Y);
//	c = fabs(AAANormal.Z);

//	code = (a>b);
//	code |= (a>c)<<1;
//	code |= (b>c)<<2;

//	from_point = &AAACorners[0];
//	i = 4;

//	switch (code)
//	{
//		case 7:
//		case 3:
//			while(i--)
//			{
//				from_point->X = from_point->Z;
//				from_point++;
//			}
//			break;

//		case 4:
//		case 6:
//			while(i--)
//			{
//				from_point->Y = from_point->Z;
//				from_point++;
//			}
//			break;
//	}

//	from_point = &AAACorners[0];
//	to_point = &AAACorners[1];
//	from_vector = &AAAVectors[0];
//	i = 4;

//	while(i--)
//	{
//		if (!i)
//			to_point = &AAACorners[0];

//		from_vector->MakeVectorToFrom(*to_point,*from_point);

//		from_vector->Z = -from_vector->Y;
//		from_vector->Y = from_vector->X;
//		from_vector->X = from_vector->Z;

//		from_point = to_point;
//		to_point++;
//		from_vector++;
//	}

//	for (PlaneParams *P=Planes; P<=LastPlane; P++)
//	{
//		if (!(P->Status & PL_ACTIVE))  continue;

//		d = distance + P->DistanceMoved;

//		if (fabs(AAAcenter.X - P->WorldPosition.X) < d)
//		{
//			if (fabs(AAAcenter.Z - P->WorldPosition.Z) < d)
//			{
//				if (fabs(AAAcenter.Y - P->WorldPosition.Y) < d)
//				{
//					do_check = ((P->WorldPosition*AAANormal) < NegD);

//					if (do_check != ((P->LastWorldPosition*AAANormal) < NegD))
//					{
//						if (LineIntersectsPlane(AAANormal,NegD,P->LastWorldPosition,P->WorldPosition,NULL,&intersect))
//						{
////					   		AIAAANearMiss(P,W->Gun);

//							if (distance > (intersect - AAAcenter))
//							{
//								switch (code)
//								{
//									case 7:
//									case 3:
//										intersect.X = intersect.Z;
//										break;

//									case 4:
//									case 6:
//										intersect.Y = intersect.Z;
//										break;
//								}

//								do_check = 0;
//								still_good = TRUE;

//								i = 4;
//								from_point = &AAACorners[0];
//								from_vector = &AAAVectors[0];

//								while(still_good && i--)
//								{
//									work1.MakeVectorToFrom(*from_point,intersect);
//									a = work1 * *from_vector;

//									if (do_check)
//									{
//										if ((a < 0.0) && (do_check == 1))
//											still_good = FALSE;
//										else
//											if ((a > 0.0) && (do_check == -1))
//												still_good = FALSE;
//									}
//									else
//										if (a < 0.0)
//											do_check = -1;
//										else
//											if (a > 0.0)
//												do_check = 1;
//								}

//								if (still_good)
//								{
//									orgflightstat = P->FlightStatus;

//									new_point.MakeVectorToFrom(W->Position,W->NextVertex->Position);
////									new_point.Normalize();

////									intersect.MakeVectorToFrom(P->WorldPosition,P->LastWorldPosition);
////									intersect.Normalize();

////									damage = 0.5 + 5.0 * (1.0 - fabs(intersect * new_point));

//									if ((P->Orientation.K * new_point) <= 0.0)
//										CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_RIGHT, NULL);
//									else
//										CrashPlane(P,PL_OUT_OF_CONTROL,PL_ENGINE_REAR_LEFT,  NULL);

////									AICCheckForHitMsgs( P, W->P, orgflightstat, W );

//									//return P;
//								}
//							}


//						}
//					}
//				}
//			}
//		}
//	}
//}

extern void LogPlaneShotdown(PlaneParams *planepnt,WeaponParams *W,DWORD damage_type,int iCrashed);

/*----------------------------------------------------------------------------
 *
 *	CrashPlane()
 *
 */
void CrashPlane( PlaneParams *P, int flight_status, int failed_system, WeaponParams *W )
{
	FPoint vel;
	int engines = failed_system & PL_ENGINES;
	int orgflightstat = P->FlightStatus;

	int NetPutCrash(PlaneParams *P,int,int);
	if (MultiPlayer && !NetPutCrash(P,flight_status,failed_system)) return;

	if((!MultiPlayer) || (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		if((P->AI.Behaviorfunc == AIDeathSpiral) && (WIsWeaponPlane(P)))
		{
			InstantiateBomb(P, 0, (1000 * (1.0f/5.7435)), P->AI.pGroundTarget, P->AI.lGroundTargetFlag);
		}
	}

	if ((iInJump) && ((P == PlayerPlane) || (P->AI.iAIFlags1 & AIPLAYERGROUP)))
	{
		return;
	}

	if (engines & PL_ENGINES)
	{
		if ((P->SystemsStatus & PL_ENGINES) != engines)
		{
			vel.MakeVectorToFrom(P->WorldPosition,P->LastWorldPosition);

			switch (P->Type->EngineConfig)
			{
				case ONE_REAR_ENGINE:
					if (!P->Smoke[SMOKE_ENGINE_REAR] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_REAR])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
					{
						if (P->Smoke[SMOKE_ENGINE_REAR])
					 		((SmokeTrail *)P->Smoke[SMOKE_ENGINE_REAR])->MakeOrphan();

						P->Smoke[SMOKE_ENGINE_REAR] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_REAR,HEAVY_BLACK_SMOKE,PLANE_BURNING);
						P->SystemsStatus |= PL_ENGINE_REAR;
						P->EngineFires[SMOKE_ENGINE_REAR].TotalTimeLeft = (5+rand()&7)<<6;
						NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
					}
					break;

				case TWO_REAR_ENGINES:
					if (engines & PL_ENGINE_REAR_LEFT)
					{
						if (!P->Smoke[SMOKE_ENGINE_REAR_LEFT] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_REAR_LEFT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
						{
							if (P->Smoke[SMOKE_ENGINE_REAR_LEFT])
					 			((SmokeTrail *)P->Smoke[SMOKE_ENGINE_REAR_LEFT])->MakeOrphan();

							P->Smoke[SMOKE_ENGINE_REAR_LEFT] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_REAR_LEFT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
							P->SystemsStatus |= PL_ENGINE_REAR_LEFT;
							P->EngineFires[SMOKE_ENGINE_REAR_LEFT].TotalTimeLeft = (5+rand()&7)<<6;
							NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
						}
					}
					else
						if (!P->Smoke[SMOKE_ENGINE_REAR_RIGHT] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_REAR_RIGHT])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
						{
							if (P->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					 			((SmokeTrail *)P->Smoke[SMOKE_ENGINE_REAR_RIGHT])->MakeOrphan();

							P->Smoke[SMOKE_ENGINE_REAR_RIGHT] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_REAR_RIGHT,HEAVY_BLACK_SMOKE,PLANE_BURNING);
							P->SystemsStatus |= PL_ENGINE_REAR_RIGHT;
							P->EngineFires[SMOKE_ENGINE_REAR_RIGHT].TotalTimeLeft = (5+rand()&7)<<6;
							NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
						}
					break;

				case FOUR_WING_ENGINES:
						if (engines & PL_ENGINE_REAR_LEFT)
						{
							if (!P->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
							{
								if (P->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD])
					 				((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD])->MakeOrphan();

								P->Smoke[SMOKE_ENGINE_WING_LEFT_OUTBOARD] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_WING_LEFT_OUTBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
								P->SystemsStatus |= PL_ENGINE_WING_LEFT_OUTBOARD;
								P->EngineFires[SMOKE_ENGINE_WING_LEFT_OUTBOARD].TotalTimeLeft = (5+rand()&7)<<6;
								NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
								flight_status &= ~PL_OUT_OF_CONTROL;
							}
							else
								if (!P->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
								{
									if (P->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD])
					 					((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD])->MakeOrphan();

									P->Smoke[SMOKE_ENGINE_WING_LEFT_INBOARD] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_WING_LEFT_INBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
									P->SystemsStatus |= PL_ENGINE_WING_LEFT_INBOARD;
									P->EngineFires[SMOKE_ENGINE_WING_LEFT_INBOARD].TotalTimeLeft = (5+rand()&7)<<6;
									NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
									flight_status &= ~PL_OUT_OF_CONTROL;
								}
						}
						else
							if (!P->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
							{
								if (P->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD])
					 				((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD])->MakeOrphan();

								P->Smoke[SMOKE_ENGINE_WING_RIGHT_OUTBOARD] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_WING_RIGHT_OUTBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
								P->SystemsStatus |= PL_ENGINE_WING_RIGHT_OUTBOARD;
								P->EngineFires[SMOKE_ENGINE_WING_RIGHT_OUTBOARD].TotalTimeLeft = (5+rand()&7)<<6;
								NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
								flight_status &= ~PL_OUT_OF_CONTROL;
							}
							else
								if (!P->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD] || (((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD])->Type == &SmokeTypes[BROWN_ENGINE_SMOKE]))
								{
									if (P->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD])
					 					((SmokeTrail *)P->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD])->MakeOrphan();

									P->Smoke[SMOKE_ENGINE_WING_RIGHT_INBOARD] = SmokeTrails->NewSmokeTrail(P,OFFSET_ENGINE_WING_RIGHT_INBOARD,HEAVY_BLACK_SMOKE,PLANE_BURNING);
									P->SystemsStatus |= PL_ENGINE_WING_RIGHT_INBOARD;
									P->EngineFires[SMOKE_ENGINE_WING_RIGHT_INBOARD].TotalTimeLeft = (5+rand()&7)<<6;
									NewGenerator(PLANE_EXPLODES,P->WorldPosition,vel,2.0f,25);
									flight_status &= ~PL_OUT_OF_CONTROL;
								}

					break;

			}
		}
	}


	if (flight_status & PL_OUT_OF_CONTROL)
	{
		P->FlightStatus |= PL_OUT_OF_CONTROL;

		PlaneReleaseTailhook(P);

		AICCheckForDamageMsgs(P, orgflightstat, failed_system, W);

		if (P->FlightStatus & PL_OUT_OF_CONTROL)
		{
			AIRemoveFromFormation(P);
		}
	}
	else
	{
		AICCheckForDamageMsgs(P, orgflightstat, (failed_system|flight_status), W);
	}

	if (!(P->FlightStatus & PL_STATUS_CRASHED) && (flight_status & PL_STATUS_CRASHED))
	{
		if (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
		{
			FPointDouble position;

			NewExplosionForcedWeapon(P->WorldPosition,812.0f,800);
			position = P->WorldPosition;
			position.Y += 10 FEET;
			NewCanister((CanisterType)CT_OIL_FIRE,position,vel,6.0f,5.0f*60.0f);
			PlayPositionalExplosion(WARHEAD_STANDARD_LARGE,Camera1,P->WorldPosition);

		}
		else if((!WIsWeaponPlane(P)) || (P->AI.iAIFlags2 & AI_CAN_CRASH))
		{
			if ((P->OnGround > 1) && (P->TerrainType >= TT_CARRIER_DECK))
			{
				FPoint vec(0.0f,1.0f,0.0f);

				PlaneHitGround(P->WorldPosition,P->IfVelocity,10.0f,&vec,P->ShipWeAreAbove);  //the 10.0f is just an arbitrary number for now
			}
			else
				PlaneHitGround(P->WorldPosition,P->IfVelocity,10.0f);  //the 10.0f is just an arbitrary number for now
		}

		PlaneReleaseTailhook(P);

		LogPlaneShotdown(P,W,0,1);

		OrphanAllPlaneSmoke(P);

		P->FlightStatus |= PL_STATUS_CRASHED;
		AIRemoveFromFormation(P);

		/* ------------------------------------------3/5/98 5:18PM---------------------------------------------
		 * If we're in the plane that just crashed, we've got to do some fancy footwork!
		 * ----------------------------------------------------------------------------------------------------*/

		if ((P == PlayerPlane) && ((PlaneParams *)Camera1.AttachedObject == P) && (Camera1.CameraMode == CAMERA_COCKPIT))
		{
			VKCODE bogus;
			bogus.vkCode = 0;									// virtual key code (may be joy button)
			bogus.wFlags = 0;									// vkCode Type
			bogus.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT


			AssignCameraSubject(P,CO_PLANE);
			camera_mode_fixed_chase(bogus);
		}

		if(WIsWeaponPlane(P))
		{
			P->Status = 0;
		}

	}
}

/*----------------------------------------------------------------------------
 *
 *	GetPlaneGroundRadarVisibility()
 *
 */
float GetPlaneGroundRadarVisibility( BasicInstance *site, PlaneParams *P, BOOL plane_to_radar )
{
	float vis;
	double range;
	double height;
	double our_dist;
	double worker;
	double wslop;
	double third;
	double above_1000;
	double below_200;
	double airspeed;
	double theta,t;
	double d1,d2,ov1,ov2;
	int index1,index2;
	DBRadarType *radar;
	RadarTable *table;
	RadarEntry *entry1,*entry2;
	FPointDouble vector;
	FPointDouble vector2;
	BOOL is_a_radar = TRUE;

	if (site->Family == FAMILY_INFOPROVIDER)
	{
		radar = GetRadarPtr(site);
		if (((InfoProviderInstance *)site)->Physicals.Flags & IP_HAS_RADAR)
			table = (RadarTable *)((int)site + sizeof(InfoProviderInstance));
		else
			table = NULL;
	}
	else
		if (site->Family == FAMILY_AAWEAPON)
		{
			radar = GetRadarPtr(site);
			if (((AAWeaponInstance *)site)->Physicals.Flags & IP_HAS_RADAR)
				table = (RadarTable *)((int)site + sizeof(AAWeaponInstance));
			else
				table = NULL;
		}
		else
		{
			radar = NULL;
			table = NULL;
			is_a_radar = FALSE;
		}

	if ((radar == NULL) && (plane_to_radar))
		return 0.0f;

	if (is_a_radar)
	{
		if (radar)
			range = (double)(radar->iMaxRange) * NMTOWU;
		else
			range = RADAR_MAX_OPTICAL_RANGE;

		our_dist = P->WorldPosition - site->Position;

		if (our_dist > range)
			vis = 0.0f;
		else
		{
//			Initial visibility should be based on range
			if (our_dist < (third = (range * 1.0/3.0)))
			{
//				if less than a twelfth go to 3x normal visibility
				if (our_dist < (range * 1.0/12.0))
					vis = 3.0f;
				else
				{
//					between a twelfth and a third, scale up from 1 to 3 times
					vis = (float)(3.0-(third-our_dist)*4.0/third);  //1.0+2.0*(1.0-(third-our_dist)/(third*0.5))
				}
			}
			else
				vis = (range-our_dist)/(range - third);
//			now determine visibility from the max elevation
 			height = P->WorldPosition.Y - site->Position.Y;

			if (radar && !plane_to_radar)
			{
				worker = tan((PI*(1.0/180.0))*(double)radar->iSearchElevation) * our_dist;
				wslop = worker * RADAR_SLOP_VALUE;
				worker += wslop;
				wslop *= 2.0;
			}
			else
			{
//				this makes sure we make it through the next part without looking at radar
				wslop = 1.0;
				worker = height + wslop*10.0;
			}

			if (height >= worker)
				vis = 0.0;
			else
			{
				worker -= height;
				if (worker < wslop)
					vis *= (float)(1.0 - worker/wslop);

//				Now factor in the curve of the earth

				worker = (EARTH_RADIUS-cos(PI*(our_dist/EARTH_RADIUS)/2.0)*EARTH_RADIUS);
				wslop = worker * RADAR_SLOP_VALUE;
				worker -= wslop;
				wslop *= 2.0;

				if (height < worker)
					vis = 0.0;
				else
				{
					worker = height - worker;
					if (worker < wslop)
						vis *= 1.0 - worker/wslop;

					/* ------------------------------------------1/8/98 0:45AM---------------------------------------------
					 * here's where we check the radar table
					 * ----------------------------------------------------------------------------------------------------*/
					if (table)
					{
						vector = P->WorldPosition;
						vector -= site->Position;

						theta = atan2(-vector.X,-vector.Z);
						if (theta < 0.0)
							theta += 2.0*PI;

						theta *= 128.0/(2.0*PI);
						index1 = (int)floor(theta);
						t = theta - (double)index1;
						index2 = index1 + 1;
						if (index2 >= 128)
							index2 -= 128;

						entry1 = (RadarEntry *)((int)table + (int)table->PieSlices[index1]);

						if (!our_dist)
							our_dist = 1.0;

						while(our_dist < entry1->Distance)
							entry1++;

						if (entry1->Distance)
						{
							d1 = entry1->Distance;
							ov1 = entry1->UpOverOut;
						}
						else
						{
							d1 = our_dist;
							ov1 = 0.0;
						}

						entry2 = (RadarEntry *)((int)table + (int)table->PieSlices[index2]);

						while(our_dist < entry2->Distance)
							entry2++;

						if (entry2->Distance)
						{
							d2 = entry2->Distance;
							ov2 = entry2->UpOverOut;
						}
						else
						{
							d2 = our_dist;
							ov2 = 0.0;
						}

						ov1 += (ov2-ov1)*t;

						if (vector.Y < our_dist*ov1)
							vis = 0.0;

					}

//					okay, vis is correctly set for distance and visibility, now we check the radar dependent variables

					if ((vis != 0.0) && (!plane_to_radar))
					{
						// We'll do optical first
						if (!radar)
						{
							// if they're coming RIGHT at us, we'll reduce visibility to 1/2
							vector = P->WorldPosition;
							vector -= site->Position;
							if (range = vector.Normalize())
							{
								worker = fabs(vector * P->Orientation.I);
								vis *= 1.0 - 0.5 * worker * worker;
							}
						}
						else
						{
							vector = P->WorldPosition;
							vector -= site->Position;
							if (range = vector.Normalize())
							{
								// convert from FEET/Second to Nautical Miles / Hour
								vector2.SetValues(WUTONM FEET * 3600,P->IfVelocity);

								// dot product with our location will give us the Z component in our direction
								vector *= vector2;

								airspeed = vector.Length();

								switch ( radar->iRadarType & RADAR_TYPE_PULSE_RATE)
								{
									case RADAR_TYPE_LPRF:
										above_1000 = 1.0;
										below_200  = 1.0;
										break;

									case RADAR_TYPE_MPRF:
										above_1000 = 1.25;
										below_200  = 0.5;
										break;

									case RADAR_TYPE_HPRF:
										above_1000 = 2.0;
										below_200  = 0.25;
										break;

									case RADAR_TYPE_CW:
									default:
										above_1000 = 1.5;
										below_200  = 0.2;
										break;

								}

								if (airspeed > 1000.0)
									vis *= above_1000;
								else
									if (airspeed < 200.0)
										vis *= below_200;
									else
										vis *= (airspeed - 200.0)*(above_1000 - below_200)*1.0/800.0 + below_200;
							}
						}
					}
				}
			}
		}
	}
	else
		vis = 0.0f;

	return vis;
}

/*----------------------------------------------------------------------------
 *
 *	GetPlaneGroundRadarVisibility()
 *
 */
float GetPlaneVehicleRadarVisibility( MovingVehicleParams *site, PlaneParams *P, BOOL plane_to_radar, int radarid)
{
	float vis;
	double range;
	double height;
	double our_dist;
	double worker;
	double wslop;
	double third;
	double above_1000;
	double below_200;
	double airspeed;
	DBRadarType *radar;
	FPointDouble vector;
	FPointDouble vector2;
	FPointDouble junkpos;

//	int radarid;
//	if (!site->iShipType)	// SCOTT FIX
//		radarid = pDBVehicleList[site->iVDBIndex].lRadarID;
//	else
//		return 0.0f;

	radar = GetRadarPtr(radarid);

	if ((radar == NULL) && (plane_to_radar))
		return 0.0f;

//	if(!(LOS(&site->WorldPosition, &P->WorldPosition, &junkpos, MED_RES_LOS, FALSE)))
//		return(0.0f);

	if (radar)
		range = (double)(radar->iMaxRange) * NMTOWU;
	else
		range = RADAR_MAX_OPTICAL_RANGE;

	our_dist = P->WorldPosition - site->WorldPosition;

	if (our_dist > range)
		vis = 0.0f;
	else
	{
//			Initial visibility should be based on range
		if (our_dist < (third = (range * 1.0/3.0)))
		{
//				if less than a twelfth go to 3x normal visibility
			if (our_dist < (range * 1.0/12.0))
				vis = 3.0f;
			else
			{
//					between a twelfth and a third, scale up from 1 to 3 times
				vis = (float)(3.0-(third-our_dist)*4.0/third);  //1.0+2.0*(1.0-(third-our_dist)/(third*0.5))
			}
		}
		else
			vis = (range-our_dist)/(range - third);
//			now determine visibility from the max elevation
 		height = P->WorldPosition.Y - site->WorldPosition.Y;

		if (radar && !plane_to_radar)
		{
			worker = tan((PI*(1.0/180.0))*(double)radar->iSearchElevation) * our_dist;
			wslop = worker * RADAR_SLOP_VALUE;
			worker += wslop;
			wslop *= 2.0;
		}
		else
		{
//				this makes sure we make it through the next part without looking at radar
			wslop = 1.0;
			worker = height + wslop*10.0;
		}

		if (height >= worker)
			vis = 0.0;
		else
		{
			worker -= height;
			if (worker < wslop)
				vis *= (float)(1.0 - worker/wslop);

//				Now factor in the curve of the earth

			worker = (EARTH_RADIUS-cos(PI*(our_dist/EARTH_RADIUS)/2.0)*EARTH_RADIUS);
			wslop = worker * RADAR_SLOP_VALUE;
			worker -= wslop;
			wslop *= 2.0;

			if (height < worker)
				vis = 0.0;
			else
			{
				worker = height - worker;
				if (worker < wslop)
					vis *= 1.0 - worker/wslop;

//					okay, vis is correctly set for distance and visibility, now we check the radar dependent variables

				if (!plane_to_radar)
				{
					// We'll do optical first
					if (!radar)
					{
						// if they're coming RIGHT at us, we'll reduce visibility to 1/2
						vector = P->WorldPosition;
						vector -= site->WorldPosition;
						if (range = vector.Normalize())
						{
							worker = fabs(vector * P->Orientation.I);
							vis *= 1.0 - 0.5 * worker * worker;
						}
					}
					else
					{
						vector = P->WorldPosition;
						vector -= site->WorldPosition;
						if (range = vector.Normalize())
						{
							// convert from FEET/Second to Nautical Miles / Hour
							vector2.SetValues(WUTONM FEET * 3600,P->IfVelocity);

							// dot product with our location will give us the Z component in our direction
							vector *= vector2;

							airspeed = vector.Length();

							switch ( radar->iRadarType & RADAR_TYPE_PULSE_RATE)
							{
								case RADAR_TYPE_LPRF:
									above_1000 = 1.0;
									below_200  = 1.0;
									break;

								case RADAR_TYPE_MPRF:
									above_1000 = 1.25;
									below_200  = 0.5;
									break;

								case RADAR_TYPE_HPRF:
									above_1000 = 2.0;
									below_200  = 0.25;
									break;

								case RADAR_TYPE_CW:
								default:
									above_1000 = 1.5;
									below_200  = 0.2;
									break;

							}

							if (airspeed > 1000.0)
								vis *= above_1000;
							else
								if (airspeed < 200.0)
									vis *= below_200;
								else
									vis *= (airspeed - 200.0)*(above_1000 - below_200)*1.0/800.0 + below_200;
						}
					}
				}
			}
		}
	}

	return vis;
}

#if 1
/*----------------------------------------------------------------------------
 *
 *	GetHardPoint()
 *
 *		Convert from weapon station and count to F18 3D object hardpoint index.
 *
 */
int GetHardPoint( PlaneParams *P, int station, int count )
{
	DBWeaponType *pDBWeapon;
	int weapid;
	// Convert from Planes.WeapLoad index to 3Dobject hardpoint index.
//	static char getdot[] = { 11,13,12, 8,10, 9,18,25,14, 0, 0, 0, 0,19,26 };
	static char getdot[] = {  8,11,14,17,20,23,26,29,32,35,38,41,44,19,26 };
						//	  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14

	// Get max CFT loads indexed by "WeapId"
	static char maxdot[] = { 0,0,0,0,0, 1,6,0,6,2, 6,0,6,6,6, 6,6,6,6,6, 6,6,1,1,4, 0,0,0,0,0, 0,0,6,2,2, 2,0,0,2,0, 0,0,1,4,0, 0,0 };
						//   0          5         10         15         20         25         30         35         40         45

	// Reverse release sequence w/N stations per CFT loaded.
	static int CFT_1L[] = { 16 };					// CFT_1R[] = { 23 };
	static int CFT_2L[] = { 18,19 };				// CFT_2R[] = { 25,26 };
	static int CFT_4L[] = { 20,21,18,19 };			// CFT_4R[] = { 27,28,25,26 };
	static int CFT_6L[] = { 16,30,29,31,15,17 };	// CFT_6R[] = { 23,33,32,34,22,24 };

//----->

	if(!P) return(0);

	if(!P->Type) return(0);

	if (P->Type->TypeNumber!=MBF18ETYPE) return (0);	// Only F-18E should enter here


	int dot = getdot[station];	// simple conversion

	switch(station)
	{
		case LEFT2_STATION:
			if(P->AI.iAICombatFlags1 & AI_S2_RACK)
			{
				weapid = pDBWeaponList[P->WeapLoad[LEFT2_STATION].WeapIndex].lWeaponID;

				if((weapid == 24) || (weapid == 133) || (weapid == 36) || (weapid == 37) || (weapid == 109))
				{
					dot += 1;
				}
				else if((weapid == 87) || (weapid == 38) || (weapid == 34) || (weapid == 35) || (weapid == 108))
				{
					dot += 2;
				}
				else
				{
					dot += 3 - count;
				}
			}
			break;
		case LEFT3_STATION:
			if(P->AI.iAICombatFlags1 & AI_S3_RACK)
				dot += 3 - count;
			break;
		case LEFT4_STATION:
			if(P->AI.iAICombatFlags1 & AI_S4_RACK)
				dot += 3 - count;
			break;
		case LEFT5_STATION:
			pDBWeapon = &pDBWeaponList[ P->WeapLoad[station].WeapIndex ];
			if((pDBWeapon->iWeaponType == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE)
					|| (pDBWeapon->iWeaponType == WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE)
					|| (pDBWeapon->iWeaponType == WEAPON_TYPE_AA_LONG_RANGE_MISSILE))
				dot += 1;
			else
				dot += 2;
			break;
		case CENTER6_STATION:
			if(P->AI.iAICombatFlags2 & AI_C6_RACK)
			{
				dot += count;
			}
			break;
		case RIGHT7_STATION:
			pDBWeapon = &pDBWeaponList[ P->WeapLoad[station].WeapIndex ];
			if((pDBWeapon->iWeaponType == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE)
					|| (pDBWeapon->iWeaponType == WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE)
					|| (pDBWeapon->iWeaponType == WEAPON_TYPE_AA_LONG_RANGE_MISSILE))
				dot += 1;
			else
				dot += 2;
			break;
		case RIGHT8_STATION:
			if(P->AI.iAICombatFlags1 & AI_S8_RACK)
				dot += count;
			break;
		case RIGHT9_STATION:
			if(P->AI.iAICombatFlags1 & AI_S9_RACK)
				dot += count;
			break;
		case RIGHT10_STATION:
			if(P->AI.iAICombatFlags1 & AI_S10_RACK)
			{
				weapid = pDBWeaponList[P->WeapLoad[LEFT2_STATION].WeapIndex].lWeaponID;

				if((weapid == 24) || (weapid == 133) || (weapid == 36) || (weapid == 37) || (weapid == 109))
				{
					dot += 1;
				}
				else if((weapid == 87) || (weapid == 38) || (weapid == 34) || (weapid == 35) || (weapid == 108))
				{
					dot += 2;
				}
				else
				{
					dot += count;
				}
			}
			break;
	}

#if 0
	//	If not AG, use simple conversion.
	if (P->WeapLoad[station].Status != AG_STAT)  return dot;

	//	If Maverick AGM-65Ds on wing, use add-on wing rack hardpoints
	if (P->WeapLoad[station].WeapId == 3)
	{
		if (station == AG_LEFT )  return 35+count-1;
		if (station == AG_RIGHT)  return 38+count-1;
	}

	//	If AG, but not CFT, use simple conversion.
	if (station != FRONT_L && station != FRONT_R)  return dot;

	// Special processing for CFT loadouts
	int dots = maxdot[P->WeapLoad[station].WeapId];

	if (count > dots)  return 0;
	if (count <= 0  )  return 0;

	switch (dots)
	{
		case 1:  dot = CFT_1L[count-1];  break;
		case 2:  dot = CFT_2L[count-1];  break;
		case 4:  dot = CFT_4L[count-1];  break;
		case 6:  dot = CFT_6L[count-1];  break;
		default:  return 0;
	}

	// Adjust hardpoints for right CFT.
	if (station == FRONT_R)  dot += (dot < 29) ? 7 : 3;
#endif

	return dot;
}

#else
/*----------------------------------------------------------------------------
 *
 *	GetHardPoint()
 *
 *		Convert from weapon station and count to F18 3D object hardpoint index.
 *
 */
int GetHardPoint( PlaneParams *P, int station, int count )
{
	// Convert from Planes.WeapLoad index to 3Dobject hardpoint index.
	static char getdot[] = { 11,13,12, 8,10, 9,18,25,14, 0, 0, 0, 0,19,26 };
						//	  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14

	// Get max CFT loads indexed by "WeapId"
	static char maxdot[] = { 0,0,0,0,0, 1,6,0,6,2, 6,0,6,6,6, 6,6,6,6,6, 6,6,1,1,4, 0,0,0,0,0, 0,0,6,2,2, 2,0,0,2,0, 0,0,1,4,0, 0,0 };
						//   0          5         10         15         20         25         30         35         40         45

	// Reverse release sequence w/N stations per CFT loaded.
	static int CFT_1L[] = { 16 };					// CFT_1R[] = { 23 };
	static int CFT_2L[] = { 18,19 };				// CFT_2R[] = { 25,26 };
	static int CFT_4L[] = { 20,21,18,19 };			// CFT_4R[] = { 27,28,25,26 };
	static int CFT_6L[] = { 16,30,29,31,15,17 };	// CFT_6R[] = { 23,33,32,34,22,24 };

//----->

	int dot = getdot[station];	// simple conversion

	//	If not AG, use simple conversion.
	if (P->WeapLoad[station].Status != AG_STAT)  return dot;

	//	If Maverick AGM-65Ds on wing, use add-on wing rack hardpoints
	if (P->WeapLoad[station].WeapId == 3)
	{
		if (station == AG_LEFT )  return 35+count-1;
		if (station == AG_RIGHT)  return 38+count-1;
	}

	//	If AG, but not CFT, use simple conversion.
	if (station != FRONT_L && station != FRONT_R)  return dot;

	// Special processing for CFT loadouts
	int dots = maxdot[P->WeapLoad[station].WeapId];

	if (count > dots)  return 0;
	if (count <= 0  )  return 0;

	switch (dots)
	{
		case 1:  dot = CFT_1L[count-1];  break;
		case 2:  dot = CFT_2L[count-1];  break;
		case 4:  dot = CFT_4L[count-1];  break;
		case 6:  dot = CFT_6L[count-1];  break;
		default:  return 0;
	}

	// Adjust hardpoints for right CFT.
	if (station == FRONT_R)  dot += (dot < 29) ? 7 : 3;

	return dot;
}
#endif

/*----------------------------------------------------------------------------
 *
 *	GetHardPointPos()
 *
 *		Convert from weapon station and count to F18 3D object hardpoint WPos.
 *
 */

SubObjectHandler *GetSubObjects(PlaneParams *P);

FPointDouble GetHardPointPos( PlaneParams *P, int station )
{
	FPointDouble  hPoint;
	int dot;

	hPoint.SetValues(0.0f, 0.0f, 0.0f);

	if(!P)
		return(hPoint);

	if(pDBWeaponList[P->WeapLoad[station].WeapIndex].lWeaponID == 133)
	{
		if((P->WeapLoad[station].bNumPods > 1) && (P->WeapLoad[station].Count > 4))
		{
			dot = GetHardPoint( P, station, 2);
		}
		else
		{
			dot = GetHardPoint( P, station, 1);
		}
	}
	else if((pDBWeaponList[P->WeapLoad[station].WeapIndex].lWeaponID == 134) || (pDBWeaponList[P->WeapLoad[station].WeapIndex].lWeaponID == 176))
	{
		if((P->WeapLoad[station].bNumPods > 1) && (P->WeapLoad[station].Count > 19))
		{
			dot = GetHardPoint( P, station, 2);
		}
		else
		{
			dot = GetHardPoint( P, station, 1);
		}
	}
	else
	{
		dot = GetHardPoint( P, station, P->WeapLoad[station].Count );
	}

	FPoint   delta(0.0f);
	FMatrix  nuAttitude(P->Attitude);

	if (P->Type->TypeNumber==MBF18ETYPE)
	{
		FindDot( P->Type->Model, dot, GetSubObjects(P), delta, nuAttitude );
	}

	hPoint = P->WorldPosition;
	hPoint += delta;

	return ( hPoint);


//	// Fake the wing racks with wing middle to allow NULL subparts below.
//	if (dot >= 35 && dot <= 37)  dot = 12;
//	if (dot >= 38 && dot <= 40)  dot =  9;

//	FPoint   delta = *P->WeapLoad[station].Type->Model->MountingOffset;

//	FMatrix  nuAttitude = P->Attitude;

//	FindDot( P->Type->Model, dot, NULL, delta, nuAttitude );

//	FPointDouble  hPoint = P->WorldPosition;

//	hPoint += delta;

//	return ( hPoint);

}

/*----------------------------------------------------------------------------
 *
 *	GetF18WeaponDrag()
 *
 */
float GetF18WeaponDrag( PlaneParams *P )
{
	float F18WeaponDrag = 0.0f;

	for (int i=0; i<MAX_F18E_STATIONS; i++)
	{
		WeaponLoadoutInfoType *W = &P->WeapLoad[i];

		float drag = pDBWeaponList[W->WeapIndex].fAircraftDrag;

		if (W->Count > 0)  F18WeaponDrag += drag;
		if (W->Count > 2)  F18WeaponDrag += drag;
	}

	return F18WeaponDrag;
}


/*----------------------------------------------------------------------------
 *
 *	CalcAltEffectOnMissile()
 *
 */
float CalcAltEffectOnMissile(double weaponaltwu)
{
	float altft;
	float workperc = 0.0f;

	altft = weaponaltwu * WUTOFT;

	if(altft < 20000)	//  weapon database values are for a weapon at 20000 ft.
	{
		workperc = (0.5f + (altft / 40000));	//  0.5 to 1.0
	}
	else
	{
		workperc = 1.0f + ((altft - 20000) / 20000); //  1.0f + 0.5f for every 10000 ft above 20000;
	}

	return(workperc);
}


/*----------------------------------------------------------------------------
 *
 *	GetMissileRMaxNM()
 *
 */
float GetMissileRMaxNM(DBWeaponType *pweapon_type, double weaponaltwu)
{
	float returnval;

	if(!pweapon_type)
	{
		return(0);
	}

	returnval = (float)pweapon_type->iRange * CalcAltEffectOnMissile(weaponaltwu);

	if((WeapStores.Aim7LoftOn) && (pweapon_type->iSeekerType == 7))
	{
		returnval *= 1.15f;
	}

	return(returnval);
}

/*----------------------------------------------------------------------------
 *
 *	GetMissileROptNM()
 *
 */
float GetMissileROptNM(DBWeaponType *pweapon_type, double weaponaltwu)
{
	return(GetMissileRMaxNM(pweapon_type, weaponaltwu) * 0.75f);
}

/*----------------------------------------------------------------------------
 *
 *	GetMissileRTR_NM()
 *
 */
float GetMissileRTR_NM(PlaneParams *targetplane, DBWeaponType *pweapon_type, double weaponaltwu)
{
	float planevel, wvel, time, ROpt, fworkval, returnval;

	if(!pweapon_type)
	{
		return(0);
	}

	if(targetplane)
	{
		planevel = targetplane->V;
	}
	else
	{
		planevel = 0;
	}

	wvel = MachAToFtSec((20000.0f), (float)pweapon_type->fMaxSpeed) * 0.5;
//	wvel *= CalcAltEffectOnMissile(weaponaltwu);

	if(wvel == 0)
	{
		wvel = 1.0f;
	}

	ROpt = GetMissileROptNM(pweapon_type, weaponaltwu) * NMTOFT;

	time = ROpt / wvel;		//  Time for weapon to get to ROpt

	fworkval = ROpt - (planevel * time);  //  move plane back distance so weapon can catch up at ROpt.

	returnval = fworkval * FTTONM;

	return(returnval);
}

/*----------------------------------------------------------------------------
 *
 *	GetImpactTime()
 *
 */
float GetImpactTime(PlaneParams *P, double orgVertVel, FPoint *ImpactPoint)
{
	float weapondrag = 1.0f;
	double VertVel;
	float Time;
	FPoint TargetPoint;
	int cnt;
	float gravity = 32.0f;
	double dheight = P->HeightAboveGround;
	DBWeaponType *pDBWeapon;
	double orgdheight;

	if(P->AGDesignate.X >= 0)
	{
		dheight = P->WorldPosition.Y - P->AGDesignate.Y;
	}

	orgdheight = dheight;

	if(P == PlayerPlane)
	{
		for (cnt = 0; cnt < MAX_F18E_STATIONS; cnt++)
		{
			if(BombStations[cnt]==TRUE)
			{
				weapondrag = pDBWeaponList[P->WeapLoad[cnt].WeapIndex].fDrag;
				BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];
				pDBWeapon = &pDBWeaponList[P->WeapLoad[cnt].WeapIndex];

				if(pDBWeapon->lWeaponID == 132)
				{
					dheight -= (350.0f * FTTOWU);
				}

				if(B->Drag != WEAP_STORES_RETARDED_MODE)
				{
					if(WIsRetardedWeapon(pDBWeapon->lWeaponID))
					{
						weapondrag = 0.97;
					}
				}

				break;
			}
		}
	}
	else
	{
		pDBWeapon = &pDBWeaponList[P->WeapLoad[P->AI.cActiveWeaponStation].WeapIndex];

		if(pDBWeapon->lWeaponID == 132)
		{
			dheight -= (350.0f * FTTOWU);
		}

	 	weapondrag = pDBWeapon->fDrag;
	}

	if(weapondrag > 1.0f)
	{
		VertVel = orgVertVel;
		gravity /= weapondrag;
		weapondrag = 1.0f;
	}
	else
	{
		VertVel = orgVertVel * weapondrag;
	}

	Time = (float)(1.0f/gravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*gravity)*(dheight * WUTOFT))));

	if(ImpactPoint)
	{
		TargetPoint.SetValues((float)(P->WorldPosition.X + (P->IfVelocity.X * weapondrag * Time)*FTTOWU),
							  (float)(P->WorldPosition.Y-orgdheight),
							  (float)(P->WorldPosition.Z + (P->IfVelocity.Z * weapondrag * Time)*FTTOWU));
		*ImpactPoint = TargetPoint;
	}

	return(Time);
}

/*----------------------------------------------------------------------------
 *
 *	GetImpactTime()
 *
 */
float GetImpactTime(WeaponParams *W, double orgVertVel, FPoint *ImpactPoint)
{
	float weapondrag = 1.0f;
	double VertVel;
	float Time;
	FPoint TargetPoint;
	float gravity = W->fGravity;
	double weaponheight = (W->Pos.Y - LandHeight(W->Pos.X, W->Pos.Z));

	DBWeaponType *pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];

	if(PlayerPlane->AGDesignate.X >= 0)
	{
		weaponheight = W->Pos.Y - PlayerPlane->AGDesignate.Y;
	}

	if(pweapon_type)
	{
		if(pweapon_type->lWeaponID == 132)
		{
			weaponheight -= (350.0f * FTTOWU);
		}
	}

	if(weaponheight < 0)
		weaponheight = 0;

	VertVel = orgVertVel * 50 * WUTOFT;

	Time = (float)(1.0f/gravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*gravity)*(weaponheight * WUTOFT))));

	if(ImpactPoint)
	{
		TargetPoint.SetValues((float)(W->Pos.X + (W->Vel.X * 50 * weapondrag * Time)),
							  (float)(W->Pos.Y-weaponheight),
							  (float)(W->Pos.Z + (W->Vel.Z * 50 * weapondrag * Time)));
		*ImpactPoint = TargetPoint;
	}

	return(Time);
}

//***********************************************************************************************************************************
// *****

void AddMissleToChasingMissleList(PlaneParams *Target)
{
  if(NumMisslesChasing+1 < MAX_CHASING_MISSLES)
  {
    NumMisslesChasing++;
    ChasingMisslesType *Cm = &MisslesInFlight[NumMisslesChasing - 1];

    Cm->EstimatedTime = AA_Tpre*1000;
	Cm->ActivateTime = AA_Tact*1000;
    Cm->TimeFired     = GameLoopInTicks;
	  Cm->Target        = Target;

	 AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;
	  if(CurAAWeap)
	  {
		  Cm->WeaponPage = CurAAWeap->WeapPage;
	  }
  }
}

//***********************************************************************************************************************************
// *****

void RemoveMissleFromChasingMissleList(int Index)
{
   ChasingMisslesType *Cm = &MisslesInFlight[Index];
   ChasingMisslesType *LastCm = &MisslesInFlight[NumMisslesChasing - 1];

   Cm->EstimatedTime = LastCm->EstimatedTime;
   Cm->TimeFired     = LastCm->TimeFired;
   Cm->Target        = LastCm->Target;
   Cm->ActivateTime  = LastCm->ActivateTime;
   Cm->WeaponPage    = LastCm->WeaponPage;
   NumMisslesChasing--;
}

//***********************************************************************************************************************************
void WGetGBUMavMinMax(PlaneParams *planepnt, DBWeaponType *pDBWeapon, float *minft, float *maxft, float *optft)
{
	float mingravity;
	double maxgravity;
	double optgravity;
	double gravitymod = 0.6f;  //  0.2f;
	float weaponheight = planepnt->HeightAboveGround;
	float Time;
	double groundvel;
	double VertVel;
	double edgemod = 1.3;  //  had been 1.2.  I think this is to max extremes obtainable.
	float weapondrag;
	float ftempvar;

	if(planepnt->AGDesignate.X >= 0)
	{
		weaponheight = PlayerPlane->WorldPosition.Y - PlayerPlane->AGDesignate.Y;
	}

	if(weaponheight < 0)
	{
		weaponheight = 0;
	}

	groundvel = planepnt->IfHorzVelocity;

	if((pDBWeapon->iWeaponType == WEAPON_TYPE_AG_MISSILE) || (pDBWeapon->iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE))
	{
		*minft = ((weaponheight * 2.2f * WUTOFT) + 2000.0f);
		*maxft = pDBWeapon->iRange * NMTOFT * 0.80f;

		if(planepnt == PlayerPlane)
		{

			if((pDBWeapon->lWeaponID == 112) || (pDBWeapon->lWeaponID == 114))
			{
				int FlightProfile;
				FPointDouble TargetLoc;
				int GroundTargetType;
				void *Trgt;

				if(pDBWeapon->lWeaponID == 114)
				{
					GetWeaponData(SLAMER_WEAP_PAGE,&FlightProfile,&Trgt,&GroundTargetType,&TargetLoc);
				}
				else
				{
					GetWeaponData(HARPOON_WEAP_PAGE,&FlightProfile,&Trgt,&GroundTargetType,&TargetLoc);
				}

				ftempvar = *maxft;
				switch(FlightProfile)
				{
					case MED_ATTACK:
						ftempvar *= CalcAltEffectOnMissile(10000.0f * FTTOWU);
						break;
					case HIGH_ATTACK:
						ftempvar *= CalcAltEffectOnMissile(20000.0f * FTTOWU);
						break;
					case LOW_POP:
					case LOW_LOW:
					default:
						ftempvar *= CalcAltEffectOnMissile(200.0f * FTTOWU);
						break;
				}
				*maxft = ftempvar;
			}
		}

		if(optft)
		{
			*optft = (*minft + *maxft) / 2;
		}
	}
	else if(pDBWeapon->fDrag > 1.0f)
	{
		mingravity = 32.0f / (sqrt(pDBWeapon->fDrag - 0.75f) + pDBWeapon->fDrag);
		maxgravity = GRAVITY * 2.0f;  //   / 0.75;
		VertVel = planepnt->IfVelocity.Y;  //  Get ImpactTime will take care of weapon drag modifications
		Time = (float)(1.0f/maxgravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*maxgravity)*(weaponheight * WUTOFT))));
		*minft = (Time * groundvel) * edgemod;
		Time = (float)(1.0f/mingravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*mingravity)*(weaponheight * WUTOFT))));
		*maxft = (Time * groundvel) / edgemod;
		if(optft)
		{
			optgravity = 32.0f / pDBWeapon->fDrag;
			if(optgravity < mingravity)
			{
				optgravity = (maxgravity + mingravity) / 2.0f;
			}
			Time = (float)(1.0f/optgravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*optgravity)*(weaponheight * WUTOFT))));
			*optft = (Time * groundvel) / edgemod;
			if(((*optft) > (*maxft)) || ((*optft) < (*minft)))
			{
				*optft = (*maxft + *minft) / 2.0f;
			}
		}
	}
	else
	{
		weapondrag = pDBWeapon->fDrag;
		if(planepnt == PlayerPlane)
		{
			BombProgType *B = &BombProgs[WeapStores.DumbBomb.CurProg];

			if(B->Drag != WEAP_STORES_RETARDED_MODE)
			{
				if(WIsRetardedWeapon(pDBWeapon->lWeaponID))
				{
					weapondrag = 0.97;
				}
			}
		}

//		VertVel = planepnt->IfVelocity.Y * pDBWeapon->fDrag;  //  Get ImpactTime will take care of weapon drag modifications
		VertVel = planepnt->IfVelocity.Y * weapondrag;  //  Get ImpactTime will take care of weapon drag modifications
		maxgravity = 32.0f;
		Time = (float)(1.0f/maxgravity)*(VertVel+sqrt((VertVel*VertVel)+((2.0f*maxgravity)*(weaponheight * WUTOFT))));
		*minft = (Time * groundvel) * 1.2;
		*maxft = *minft;
		if(optft)
		{
			*optft = *minft;
		}
	}
}

//**************************************************************************************
void InstantiateEjectionSeat(PlaneParams *P, int numejects, int sar_on_way, WeaponParams *WSO, int netid)
{
	int Index;
	float speedinc = 0;
	FPointDouble accelval;
	FPoint offset;
	FPOffSet toffset;
	int badchute = 0;

	if(P->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	if (MultiPlayer && ((P==PlayerPlane) || (P && (P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))) && netid==-1 && numejects==1)
	{
		int NetPutEject( PlaneParams *P );
		netid = NetPutEject( P );		//	MultiPlayer Remote Procedure Call
	}
	else if((!MultiPlayer) || (netid == -1))  //  Check with Mike.  I think he was incrimenting this for debrief replay.  Screws up MultiPlayer if done this way hence added else condition
	{
		netid = g_iWeaponID++;
	}

	int iEjectedAircraft=P-Planes;

	if (numejects == 1)
	{
		// Determine where I am
		int tempfence = AICCheckAllFences((P->WorldPosition.X * WUTOFT), (P->WorldPosition.Z * WUTOFT));

		if(P->Knots>600)  //   && (g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_FLIGHT))
		{
			LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_KIA,iEjectedAircraft,1,0);
			badchute = 1;
		}

		if (tempfence)
		{
			LogMissionStatistic(LOG_FLIGHT_EJECTED_IN_ENEMY,iEjectedAircraft,1,2);
		} else {
			LogMissionStatistic(LOG_FLIGHT_EJECTED_IN_FRIENDLY,iEjectedAircraft,1,2);
		}
	}
	else if(numejects == 1)
	{
		if(P->Knots > 600)
		{
			badchute = 1;
		}
	}

	if(numejects == 1)
	{
		P->AI.iAIFlags1 |= AI_HAS_EJECTED;
	}

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

    W->Type = pEjectioSeatType;
	Index = AIGetWeaponIndex(W->Type->TypeNumber);

	W->InitialVelocity = P->V*pDBWeaponList[Index].fDrag;

	W->iNetID			= netid;
	W->Kind = EJECTION_SEAT;
	W->WeaponMove = MoveEjectionSeat;
	W->P = P;
	W->Launcher			= (void *)P;
	W->LauncherType		= AIRCRAFT;

	W->Flags = MISSILE_INUSE|BOOST_PHASE|WEP_GUY_IN_CHAIR;

	if(badchute)
	{
		W->Flags |= WEP_DAMAGED_GUY_IN_CHAIR;
	}

	if(P->AI.iSide != AI_FRIENDLY)
	{
		if(frand() < (0.1f - (0.01 * P->AI.iSkill)))
		{
			W->Flags |= (WEP_DAMAGED_CHUTE);
		}
	}

	W->Vel.SetValues((float)HIGH_FREQ * FTTOWU,P->IfVelocity);

	W->Attitude = P->Attitude;
	W->Heading = P->Heading;
	W->Pitch = P->Pitch;
	W->Roll = P->Roll;
	W->AccelOrientation = P->Orientation.K;
	W->AccelOrientation *= -1.0f;

	W->Vel.X *= pDBWeaponList[Index].fDrag;
//	W->Vel.Y *= pDBWeaponList[Index].fDrag;
	W->Vel.Z *= pDBWeaponList[Index].fDrag;

	accelval = P->Orientation.I;
	accelval *= -(0.15f + (0.1f * (float)(numejects - 1)));
	W->Vel += accelval;

	accelval = W->AccelOrientation;
	accelval *= 0.6f;
	W->Vel += accelval;
	W->pTarget = NULL;
	W->iTargetType	= NONE;

	if(WSO)
	{
		WSO->pTarget = (void *)W;
	}

	if((numejects == 1) || (pDBAircraftList[P->AI.iPlaneIndex].OffSet[3].Z == 0))
	{
		toffset = pDBAircraftList[P->AI.iPlaneIndex].OffSet[2];
	}
	else
	{
		toffset = pDBAircraftList[P->AI.iPlaneIndex].OffSet[3];
	}

	offset.X = toffset.X * FOOT_MULTER;
	offset.Y = toffset.Y * FOOT_MULTER;
	offset.Z = toffset.Z * FOOT_MULTER;

	W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(double)offset.Z + P->Orientation.J.X*(double)offset.X + P->Orientation.K.X*(double)offset.Y;
	W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(double)offset.Z + P->Orientation.J.Y*(double)offset.X + P->Orientation.K.Y*(double)offset.Y;
	W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(double)offset.Z + P->Orientation.J.Z*(double)offset.X + P->Orientation.K.Z*(double)offset.Y;

	W->GroundHeight		= 0.0f;

	W->lBurnTimer = pDBWeaponList[Index].iBurnTime * 1000;  //  10 second diration.
	W->lTimer = 3000;

	W->Smoke	  = SmokeTrails->NewSmokeTrail((void *)W,0,WHITE_MISSILE_SMOKE,EJECTION_FLARE_LIGHT);


	W->LifeTime = 0;

	W->fpTempWay.X = numejects;
	W->fpTempWay.Y = 0;
	W->fpTempWay.Z = sar_on_way;

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	if(P == PlayerPlane)
	{
		if(numejects == 1)
		{
			pPlayerChute = W;
			PlayerPlane->FlightStatus |= PL_OUT_OF_CONTROL;
			StopEngineSounds();
		}
		else if(CockpitSeat == BACK_SEAT)
		{
			pPlayerChute = W;
		}
		PlayPositionalExplosion(2,Camera1,P->WorldPosition);
	}
	else if(numejects == 1)
	{
		if (Camera1.AttachedObjectType == CO_PLANE)
		{
			if(P == (PlaneParams *)Camera1.AttachedObject)
			{
				setup_no_cockpit_art();
				AssignCameraSubject((void *)W,CO_WEAPON);
			}
		}

		if(P->Status & PL_COMM_DRIVEN)
		{
			P->FlightStatus |= PL_OUT_OF_CONTROL;
		}
	}

//	setup_no_cockpit_art();
//	AssignCameraSubject((void *)W,CO_WEAPON);
//	Camera1.CameraMode = CAMERA_FIXED_CHASE;
//	ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
}

//**************************************************************************************
void InstantiateBomberEjections(PlaneParams *P, int numejects, int sar_on_way, WeaponParams *WSO, int netid, WeaponType	 *Type)
{
	int Index;
	float speedinc = 0;
	FPointDouble accelval;
	FPoint offset;
	FPOffSet toffset;
	int checktype = 0;

	if(P->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	W->Kind = EJECTION_SEAT;

	if(Type)
	{
		W->Type = Type;
		checktype = 1;

		DBWeaponType *pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];

		if((pweapon_type->lWeaponID != 105) && (pweapon_type->lWeaponID != 106) && (pweapon_type->lWeaponID != 107))
		{
			W->Kind	= BOMB;
		}
#if 0
		if(!pweapon_type)
		{
			if(pweapon_type->iWeaponType == WEAPON_TYPE_NAVAL_MINE)
			{
				isperson = 0;
			}
			else if(pweapon_type->iUsageIndex == 22)
			{
				if(planepnt)
				{
					if(!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
					{
						isperson = 0;
					}
				}
				else
				{
					isperson = 0;
				}
			}
		}
#endif
	}
	else if(P->AI.iSide == AI_FRIENDLY)
	{
	    W->Type = pFriendlyChuteType;
	}
	else
	{
	    W->Type = pEnemyChuteType;
	}

	Index = AIGetWeaponIndex(W->Type->TypeNumber);

	W->InitialVelocity = P->V*pDBWeaponList[Index].fDrag;

	W->WeaponMove = MoveJumpers;
	W->iNetID			= netid;
	W->P = P;
	W->Launcher			= (void *)P;
	W->LauncherType		= AIRCRAFT;

	W->Flags = MISSILE_INUSE;

	W->Vel.SetValues((float)HIGH_FREQ * FTTOWU,P->IfVelocity);

	W->Attitude = P->Attitude;
	W->Heading = P->Heading;
	W->Pitch = P->Pitch;
	W->Roll = P->Roll;
	W->AccelOrientation = P->Orientation.K;

	W->pTarget = NULL;
	W->iTargetType	= NONE;

	if(WSO)
	{
		WSO->pTarget = (void *)W;
	}


	W->Vel.X *= 0.8;
//	W->Vel.Y *= pDBWeaponList[Index].fDrag;
	W->Vel.Z *= 0.8;

	accelval = W->AccelOrientation;
	accelval *= 0.06f;
	W->Vel += accelval;

	toffset = pDBAircraftList[P->AI.iPlaneIndex].OffSet[2];

	offset.X = toffset.X * FOOT_MULTER;
	offset.Y = toffset.Y * FOOT_MULTER;
	offset.Z = toffset.Z * FOOT_MULTER;

	if(W->Kind != BOMB)
	{
		W->Pos.X = P->WorldPosition.X + P->Orientation.I.X*(double)offset.Z + P->Orientation.J.X*(double)offset.X + P->Orientation.K.X*(double)offset.Y;
		W->Pos.Y = P->WorldPosition.Y + P->Orientation.I.Y*(double)offset.Z + P->Orientation.J.Y*(double)offset.X + P->Orientation.K.Y*(double)offset.Y;
		W->Pos.Z = P->WorldPosition.Z + P->Orientation.I.Z*(double)offset.Z + P->Orientation.J.Z*(double)offset.X + P->Orientation.K.Z*(double)offset.Y;
	}
	else
	{
		W->Pos = GetHardPointPos(P, sar_on_way);
	}

	W->GroundHeight		= 0.0f;

	W->lTimer = 4000;

	W->Smoke	  = NULL;

	W->LifeTime = 0;

	W->fpTempWay.X = numejects;
	W->fpTempWay.Y = 0;
	W->fpTempWay.Z = sar_on_way;

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	if((numejects == 1) && ((W->Type == pFriendlyChuteType) || (W->Type == pEnemyChuteType)))
	{
		if (Camera1.AttachedObjectType == CO_PLANE)
		{
			if(P == (PlaneParams *)Camera1.AttachedObject)
			{
				setup_no_cockpit_art();
				AssignCameraSubject((void *)W,CO_WEAPON);
			}
		}
	}

	if(checktype)
	{
		DBWeaponType *pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];

		if((pweapon_type->lWeaponID == 135) || (pweapon_type->lWeaponID == 136))
		{
			LogWeaponInstantiate(P,W);
		}
	}

//	setup_no_cockpit_art();
//	AssignCameraSubject((void *)W,CO_WEAPON);
//	Camera1.CameraMode = CAMERA_FIXED_CHASE;
//	ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
}

//**************************************************************************************
void InstantiateChute(WeaponParams *Seat)
{
	int Index;
	PlaneParams *P = Seat->P;
	WeaponParams *tempweap;

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	if(pPlayerChute == Seat)
	{
		pPlayerChute = W;

	}

	W->P = P;
	W->Launcher			= (void *)P;
	W->LauncherType		= AIRCRAFT;

	if(W->P->AI.iSide == AI_FRIENDLY)
	{
	    W->Type = pFriendlyChuteType;
	}
	else
	{
	    W->Type = pEnemyChuteType;
	}

	Index = AIGetWeaponIndex(W->Type->TypeNumber);

	W->InitialVelocity = Seat->InitialVelocity;

	W->Kind = EJECTION_SEAT;
	W->WeaponMove = MoveChute;

	W->Flags = MISSILE_INUSE;
	W->Flags |= (Seat->Flags & WEP_DAMAGED_GUY_IN_CHAIR);

	W->Vel = Seat->Vel;
	W->iNetID = Seat->iNetID;

	W->Vel.X *= pDBWeaponList[Index].fDrag;
//	W->Vel.Y *= pDBWeaponList[Index].fDrag;
	W->Vel.Z *= pDBWeaponList[Index].fDrag;

	W->Vel.Y += 0.04f;

	W->Attitude = Seat->Attitude;
	W->Heading = Seat->Heading;
	W->Pitch = Seat->Pitch;
	W->Roll = Seat->Roll;

	W->Pos = Seat->Pos;
	W->GroundHeight		= 0.0f;

	W->lBurnTimer = 0;  //  10 second diration.
	W->lTimer = 1;

	W->Smoke = NULL;
	W->LifeTime = 0;

	W->pTarget = NULL;
	W->iTargetType	= NONE;

	if(Seat)
	{
		if(Seat->pTarget == NULL)
		{
			Seat->pTarget = (void *)W;
		}
		else
		{
			tempweap = (WeaponParams *)Seat->pTarget;
			if((tempweap >= Weapons) && (tempweap <= LastWeapon))
			{
				if((tempweap->Flags & WEAPON_INUSE) && (tempweap->Kind == EJECTION_SEAT))
				{
					W->pTarget = Seat->pTarget;
				}
			}
		}
	}
	else
	{
		W->pTarget = NULL;
	}

	W->fpTempWay.X = Seat->fpTempWay.X;
	W->fpTempWay.Y = 0;
//	W->fpTempWay.Y = Seat->fpTempWay.Z;
	W->fpTempWay.Z = Seat->fpTempWay.Z;

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	if (Camera1.AttachedObjectType == CO_WEAPON)
	{
		if(Seat == (WeaponParams *)Camera1.AttachedObject)
		{
			setup_no_cockpit_art();
			AssignCameraSubject((void *)W,CO_WEAPON);
		}
	}
}

//**************************************************************************************
BOOL DeleteEjectionSeat( WeaponParams *W )
{
	PlaneParams *planepnt = W->P;

	if (Camera1.AttachedObjectType == CO_WEAPON)
	{
		if(W == (WeaponParams *)Camera1.AttachedObject)
		{
			AssignCameraTarget(NULL,CO_NADA);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FREE);
		}
	}

	if(planepnt)
	{
		int iAircraft=planepnt-Planes;

		if ((W->Flags & WEP_GUY_IN_CHAIR) && (W->fpTempWay.X == 1))
		{
			LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_KIA,iAircraft,1,0);
		}
	}

	FPointDouble lloc = W->Pos;

	lloc.Y = (double)LandHeight(lloc.X,lloc.Z);
	NoFlashDuringExplosion = TRUE;
	NewExplosionForcedWeapon(lloc, 3, 3,NULL,TRUE,CT_NONE );
	NoFlashDuringExplosion = FALSE;

//	if((W->P == PlayerPlane) && !(W->Flags & WEP_GUY_IN_CHAIR) && !InWater(W->Pos.X, W->Pos.Z))
//	{
////		W->WeaponMove = ChuteStayOnGround;
////		return FALSE;

//	}

	W->Flags = 0;
	W->Kind  = NONE;
	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	}
	W->Smoke = NULL;


//	UnsortObject(W);

	if (Camera1.AttachedObject == (int *)W)
		SwitchCameraSubjectToPoint(W->Pos);

	if (Camera1.TargetObject == (int *)W)
		SwitchCameraTargetToPoint(W->Pos);

	if(W == (WeaponParams *)pPadLockTarget)
	{
		iPadLockType = TARGET_LOCATION;
		fpdPadLockLocation = W->Pos;
		pPadLockTarget = NULL;
	}

	if (W == LastWeapon)
		while((LastWeapon >= Weapons) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;

	return TRUE;
}

//**************************************************************************************
void DeleteChute( WeaponParams *W )
{
	PlaneParams *planepnt = W->P;
	float randchance;
	DBWeaponType *pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	int isperson = 1;

	if ((Camera1.AttachedObjectType == CO_WEAPON) && (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		if(W == (WeaponParams *)Camera1.AttachedObject)
		{
			AssignCameraTarget(NULL,CO_NADA);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FREE);
		}
	}

	if(W->Kind != EJECTION_SEAT)
	{
		isperson = 0;
	}

	if(isperson)
	{
		if(planepnt)
		{
			int iAircraft=planepnt-Planes;

			if(W->fpTempWay.X == 1)
			{
				float actvel = sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Y * W->Vel.Y) + (W->Vel.Z * W->Vel.Z)) * WUTOFT * 50.0f * FTSEC_TO_MLHR*MLHR_TO_KNOTS;
				if (actvel > 50.0f)
				{
					LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_KIA,iAircraft,1,0);
					W->Flags |= WEP_DAMAGED_CHUTE;
				}
				else
				{
					if(AICCheckAllFences((planepnt->WorldPosition.X * WUTOFT), (planepnt->WorldPosition.Z * WUTOFT)))
					{
						randchance = 0.25;
						if(!W->fpTempWay.Z)
						{
							randchance += 0.25;
						}

						if(AIC_Enemy_Near(planepnt) && (frand() > randchance))
						{
							LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_MIA,iAircraft,1,0);
							W->fpTempWay.Z = 0;
							//  Remove any other chutes from group
						}
						else if((W->fpTempWay.Z) && (frand() < 0.25f))
						{
							LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_MIA,iAircraft,1,0);
							W->fpTempWay.Z = 0;
							//  Remove any other chutes from group
						}
						else
						{
							LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_SURVIVED,iAircraft,1,0);
						}
					}
					else
					{
						LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_SURVIVED,iAircraft,1,0);
					}
				}
			}
		}

		if((W->P == PlayerPlane) || (W->fpTempWay.Z))
		{
			//  if Player and Captured , make helo divert

			if(!((W->P < Planes) || (W->P > LastPlane)))
			{
				W->WeaponMove = ChuteStayOnGround;
				if(W->Flags & WEP_DAMAGED_CHUTE)
				{
					W->Flags &= ~(WEP_CHUTE_OUT);
		#if 0  //  I don't think I want to do this since I don't want the Helos to "find" him.
					if(W->fpTempWay.X == 1)
					{
						AIUpdateChuteSAR(W);
					}
		#endif
				}
				else
				{
					W->Flags &= ~(WEP_CHUTE_OUT|WEP_CHUTE_HALF_OUT);
					if(W->fpTempWay.X == 1)
					{
						AIUpdateChuteSAR(W);
					}
					W->Type = pGuyOnGroundType;
				}

		//		if(!((W->Flags & WEP_DAMAGED_CHUTE) && (InWater(W->Pos.X, W->Pos.Z))))
				if(!(W->Flags & WEP_DAMAGED_CHUTE))  //  Too much water in F/A-18, I made the chute guys "swim" by lowering their Y position.
					return;
			}
		}
		else if((W->P) && (!((W->P < Planes) || (W->P > LastPlane))))
		{
			if((W->P->AI.iAIFlags1 & AIPLAYERGROUP) && (!(W->Flags & WEP_DAMAGED_CHUTE)))
			{
				W->WeaponMove = ChuteStayOnGround;
				W->Flags &= ~(WEP_CHUTE_OUT|WEP_CHUTE_HALF_OUT);
				if(W->fpTempWay.X == 1)
				{
					AIUpdateChuteSAR(W);
				}
				W->Type = pGuyOnGroundType;
				return;
			}
		}
	}

	W->Flags = 0;
	W->Kind  = NONE;
	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	}
	W->Smoke = NULL;


//	UnsortObject(W);

	if (Camera1.AttachedObject == (int *)W)
		SwitchCameraSubjectToPoint(W->Pos);

	if (Camera1.TargetObject == (int *)W)
		SwitchCameraTargetToPoint(W->Pos);

	if(W == (WeaponParams *)pPadLockTarget)
	{
		iPadLockType = TARGET_LOCATION;
		fpdPadLockLocation = W->Pos;
		pPadLockTarget = NULL;
	}

	if (W == LastWeapon)
		while((LastWeapon >= Weapons) && !(LastWeapon->Flags & WEAPON_INUSE))
			LastWeapon--;
}

extern double		 maxyaw;
//**************************************************************************************
void MoveEjectionSeat( WeaponParams *W )
{
	DBWeaponType *pweapon_type;
	PlaneParams *planepnt;

	PlaneParams *P = NULL;
	int timerchange = 0;
	int ballistic = 0;
	int realweapons = 1;
	float speedinc = 0;
	FPoint vel;
	FPointDouble accelval;
	float maxroll;
	float dP, dR, dH;
	WeaponParams *pilot;
	float actvel;
	int bullethit;
	ANGLE addangle;

	actvel = sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Y * W->Vel.Y) + (W->Vel.Z * W->Vel.Z)) * WUTOFT * 50.0f * FTSEC_TO_MLHR*MLHR_TO_KNOTS;

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	if(W->pTarget)
	{
		pilot = (WeaponParams *)W->pTarget;
		if((pilot >= Weapons) && (pilot <= LastWeapon))
		{
			if((pilot->pTarget) && (pilot->Flags & WEAPON_INUSE) && (pilot->Kind == EJECTION_SEAT))
			{
				W->pTarget = pilot->pTarget;
			}
		}
	}

	if(W->Flags & BOOST_PHASE)
	{
		maxyaw = pweapon_type->iMaxYawRate / 50.0;
		maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

	//	maxyaw *= 0.5f + ((W->InitialVelocity / desiredspeed) * 0.5f);

		speedinc = (float)pweapon_type->iAcceleration/(WUTOFT*2500.0);

		accelval = W->AccelOrientation;
		accelval *= speedinc;

		W->Vel += accelval;

		W->lBurnTimer -= 20;
		if (W->lBurnTimer < 0)
		{
			if (W->Smoke)
				((SmokeTrail *)W->Smoke)->MakeOrphan();
			W->Smoke = NULL;


			W->Flags &= ~(BOOST_PHASE);
			W->Flags |= GLIDE_PHASE;
		}

		 //	End of pre-calc, do the missile's flight
		//

//		if(!ballistic)
//			newAttitude(W);

		W->LastPos = W->Pos;	// WeaponHitsPlane() & WeaponIntersectsGround() need this
		W->Pos    += W->Vel;

		if (W->Smoke)  ((SmokeTrail *) W->Smoke )->Update();

		// update total distance traveled

		if((W->LifeTime < 500) && ((W->LifeTime + 20) >= 500) && (W->fpTempWay.X > 1))
		{
			InstantiateEjectionSeat(W->P, (W->fpTempWay.X - 1), (int)W->fpTempWay.Z, W);
		}

		W->LifeTime += 20;
	}
	else if(actvel < 450.0f)
	{
		W->LastPos = W->Pos;
		W->Pos    += W->Vel;
		W->Pos.Y  -= G_TIX/2;
		W->Vel.Y  -= G_TIX;
	}
	else
	{
		W->Vel *= 0.999f;
		W->LastPos = W->Pos;
		W->Pos    += W->Vel;
		W->Pos.Y  -= (G_TIX/2) * 0.95f;
		W->Vel.Y  -= G_TIX * 0.95f;
	}

	planepnt = WeaponHitsPlane(W, &bullethit);

	if(planepnt)
	{
		W->Flags |= WEP_DAMAGED_GUY_IN_CHAIR;
		if(!bullethit)
		{
			ProximitySoundHit(pweapon_type);
		}
		if((W->P) && (W->fpTempWay.X == 1))
		{
			if((W->P - Planes) == PlayerPlane->AI.wingman)
			{
				char wavestr[_MAX_PATH];
				char *tempstr;
				char drive[_MAX_DRIVE];
				char dir[_MAX_DIR];
				char fname[_MAX_FNAME];
				char ext[_MAX_EXT];

				_splitpath("kenny.wav", drive, dir, fname, ext);
				tempstr = GetRegValue("mission");

				sprintf(wavestr, "%s\\%s%s", tempstr, fname, ext);

				if(SndStreamInit(wavestr))
				{
					SndStreamPlay();
				}
			}
		}
	}

	if (WeaponIntersectsGround(W) && DeleteEjectionSeat(W))
		return;

	if((W->lTimer >= 0) && (W->lTimer < 2000))
	{
		if(W->Roll)
		{
			maxroll = 90.0f / 50.0f;

			dR = -AIConvertAngleTo180Degree(W->Roll);

			if (dR < -maxroll)  dR = -maxroll;
			if (dR >  maxroll)  dR =  maxroll;

			addangle = (ANGLE)(dR*DEGREE);
			if(addangle == 0)
			{
				W->Roll = 0;
			}
			else
			{
				W->Roll += addangle;
			}

			W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
		}
		else
		{
			maxyaw = pweapon_type->iMaxYawRate / 50.0;
			maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

			dP = AIConvertAngleTo180Degree(0x3000 - W->Pitch);

			if (dP < -maxyaw)  dP = -maxyaw;
			if (dP >  maxyaw)  dP =  maxyaw;

			W->Pitch += (ANGLE)(dP*DEGREE);
			W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
		}


		if((W->lTimer > 100) || (((W->Pos.Y < (10000 * FTTOWU)) || ((W->Pos.Y - LandHeight( W->Pos.X, W->Pos.Z)) < (5000.0f * FTTOWU))) && (actvel < 450.0f)))
		{
			W->lTimer -= 20;
		}

		if (W->lTimer < 0)
		{
			W->Flags &= ~(WEP_GUY_IN_CHAIR);

			InstantiateChute(W);

			if(rand() & 1)
			{
				W->Flags |= WEP_TURN_LEFT;
			}
			else
			{
				W->Flags |= WEP_TURN_RIGHT;
			}
		}
	}
	else if(W->lTimer > 0)
	{
		W->lTimer -= 20;
	}
	else
	{
		maxyaw = pweapon_type->iMaxYawRate / 50.0;
		maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

		if (W->Flags & WEP_TURN_RIGHT)  dH = -maxyaw;
		else if (W->Flags & WEP_TURN_LEFT)  dH =  maxyaw;
		else dH = 0;

		W->Heading += (ANGLE)(dH*DEGREE);
		W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
	}

	if(W == pPlayerChute)
	{
		CheckNotInPlayerPlane();
	}
}

//**************************************************************************************
void MoveJumpers( WeaponParams *W )
{
	DBWeaponType *pweapon_type;
	PlaneParams *planepnt;
	WeaponParams *pilot;
	int bullethit;

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	if(W->pTarget)
	{
		pilot = (WeaponParams *)W->pTarget;
		if((pilot >= Weapons) && (pilot <= LastWeapon))
		{
			if((pilot->pTarget) && (pilot->Flags & WEAPON_INUSE) && (pilot->Kind == EJECTION_SEAT))
			{
				W->pTarget = pilot->pTarget;
			}
		}
	}

	if((W->lTimer > 1000) && (((W->Pos.Y < (10000 * FTTOWU)) || ((W->Pos.Y - LandHeight( W->Pos.X, W->Pos.Z)) < (5000.0f * FTTOWU))) || (W->LifeTime < 1700)))
	{
		W->lTimer -= 20;
	}
	else
	{
		W->WeaponMove = MoveChute;
	}

	W->LastPos = W->Pos;
	W->Pos    += W->Vel;

//	if((W->LifeTime < 1500) && ((W->LifeTime + 20) >= 4000)  && (W->fpTempWay.X > 1))
	if((W->LifeTime < 1500) && ((W->LifeTime + 20) >= 1500)  && (W->fpTempWay.X > 1))
	{
		if(W->Kind == EJECTION_SEAT)
		{
			InstantiateBomberEjections(W->P, (W->fpTempWay.X - 1), (int)W->fpTempWay.Z, W, -1);
		}
		else
		{
			InstantiateBomberEjections(W->P, (W->fpTempWay.X - 1), (int)W->fpTempWay.Z, W, -1, W->Type);
		}
	}

	W->LifeTime += 20;

	W->Pos.Y  -= (G_TIX/2) * 0.9f;
	W->Vel.Y  -= G_TIX * 0.9f;

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	planepnt = WeaponHitsPlane(W, &bullethit);
	if(planepnt)
	{
		if(!bullethit)
		{
			ProximitySoundHit(pweapon_type);
		}

		if(W->Flags & WEP_CHUTE_OUT)
		{
			W->Flags |= (WEP_DAMAGED_CHUTE|WEP_CHUTE_HALF_OUT);
			W->Flags &= ~(WEP_CHUTE_OUT);
		}
//		DoExplosionInAir(W, 0, NULL, NULL, planepnt);
//		DeleteBomb(W);
	}
	else
	{
		if (WeaponIntersectsGround(W))
		{
			DeleteChute(W);
		}
	}

	if(W == pPlayerChute)
	{
		CheckNotInPlayerPlane();
	}
}

//**************************************************************************************
void MoveChute( WeaponParams *W )
{
	DBWeaponType *pweapon_type;
	PlaneParams *planepnt;
	FPointDouble velnorm;
	double desiredpitch;
	ANGLE adesiredpitch;
	double actvel;
	float maxroll;
	float dP, dR;
	double heading;
	ANGLE aheading;
	float accmultval = 16.0f;
	double avgval;
	double accinc;
	float ftempval;
	WeaponParams *pilot;
	int bullethit;
	ANGLE addangle;


	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	if(W->pTarget)
	{
		pilot = (WeaponParams *)W->pTarget;
		if((pilot >= Weapons) && (pilot <= LastWeapon))
		{
			if((pilot->pTarget) && (pilot->Flags & WEAPON_INUSE) && (pilot->Kind == EJECTION_SEAT))
			{
				W->pTarget = pilot->pTarget;
			}
		}
	}

	if((W->lTimer >= 0) && (((W->Flags & WEP_CHUTE_HALF_OUT) && (!(W->Flags & WEP_DAMAGED_CHUTE))) || (W->Pos.Y < (10000 * FTTOWU)) || ((W->Pos.Y - LandHeight( W->Pos.X, W->Pos.Z)) < (5000.0f * FTTOWU))))
	{
		W->lTimer -= 20;
		if(W->lTimer < 0)
		{
			if((W->Flags & WEP_CHUTE_HALF_OUT) && (!(W->Flags & WEP_DAMAGED_CHUTE)))
			{
				W->Flags &= ~(WEP_CHUTE_HALF_OUT);
				W->Flags |= WEP_CHUTE_OUT;
			}
			else
			{
				W->Flags |= WEP_CHUTE_HALF_OUT;
				W->lTimer = 3000;
			}
		}
	}

	if((W->Roll) && (W->Roll != 0x8000))
	{
		maxroll = 90.0f / 50.0f;

		dR = -AIConvertAngleTo180Degree(W->Roll);

		if (dR < -maxroll)  dR = -maxroll;
		if (dR >  maxroll)  dR =  maxroll;

		addangle = (ANGLE)(dR*DEGREE);
		if(addangle == 0)
		{
			W->Roll = 0;
		}
		else
		{
			W->Roll += addangle;
		}

		W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
	}
	else
	{
		maxyaw = pweapon_type->iMaxYawRate / 50.0;
		maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

		velnorm = W->Vel;
		velnorm.Normalize();

		desiredpitch = -RadToDeg(acos(velnorm.Y)) + 180.0f;
		desiredpitch = AICapAngle(desiredpitch);


		if(velnorm.Z)
		{
			heading = atan2(-velnorm.X,-velnorm.Z) * 57.2958;
			aheading = AIConvert180DegreeToAngle(heading);
			heading = AIConvertAngleTo180Degree(aheading - W->Heading);
			if(fabs(heading) > 90)
			{
				desiredpitch = -desiredpitch;
			}
		}

		adesiredpitch = AIConvert180DegreeToAngle(desiredpitch);

		dP = AIConvertAngleTo180Degree(adesiredpitch - W->Pitch);

		ftempval = 180 * accmultval;

		accinc = dP - (W->fpTempWay.Y / accmultval);
		accinc = AICapAngle(accinc);

		W->fpTempWay.Y += accinc;

		if(W->fpTempWay.Y > ftempval)
		{
			W->fpTempWay.Y -= ftempval * 2;
		}
		else if(W->fpTempWay.Y <= -ftempval)
		{
			W->fpTempWay.Y += ftempval * 2;
		}
		avgval = W->fpTempWay.Y / accmultval;
		dP = avgval;

		if (dP < -maxyaw)  dP = -maxyaw;
		if (dP >  maxyaw)  dP =  maxyaw;

		W->Pitch += (ANGLE)(dP*DEGREE);
		W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
	}

	W->LastPos = W->Pos;
	W->Pos    += W->Vel;

	if(W->Flags & WEP_CHUTE_OUT)
	{
		actvel = sqrt((W->Vel.X * W->Vel.X) + (W->Vel.Y * W->Vel.Y) + (W->Vel.Z * W->Vel.Z)) * WUTOFT * 50.0f * FTSEC_TO_MLHR*MLHR_TO_KNOTS;

		if(actvel > (30.0f))
		{
			W->Vel *= 0.985f;
			W->Pos.Y  -= (G_TIX/2) * 0.25f;
			W->Vel.Y  -= G_TIX * 0.25f;
		}
		else
		{
			W->Pos.Y  -= (G_TIX/2) * 0.5f;
			W->Vel.Y  -= G_TIX * 0.5f;
			W->Vel.X *= 0.99;
			W->Vel.Z *= 0.99;
		}
	}
	else if(W->Flags & WEP_CHUTE_HALF_OUT)
	{
		W->Pos.Y  -= (G_TIX/2) * 0.8f;
		W->Vel.Y  -= G_TIX * 0.8f;
	}
	else
	{
		W->Pos.Y  -= (G_TIX/2) * 0.9f;
		W->Vel.Y  -= G_TIX * 0.9f;
	}

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	planepnt = WeaponHitsPlane(W, &bullethit);
	if(planepnt)
	{
		if(!bullethit)
		{
			ProximitySoundHit(pweapon_type);
		}

		if((W->P) && (W->fpTempWay.X == 1))
		{
			if((W->P - Planes) == PlayerPlane->AI.wingman)
			{
				char wavestr[_MAX_PATH];
				char *tempstr;
				char drive[_MAX_DRIVE];
				char dir[_MAX_DIR];
				char fname[_MAX_FNAME];
				char ext[_MAX_EXT];

				_splitpath("kenny.wav", drive, dir, fname, ext);
				tempstr = GetRegValue("mission");

				sprintf(wavestr, "%s\\%s%s", tempstr, fname, ext);

				if(SndStreamInit(wavestr))
				{
					SndStreamPlay();
				}
			}
		}

		if(W->Flags & WEP_CHUTE_OUT)
		{
			W->Flags |= (WEP_DAMAGED_CHUTE|WEP_CHUTE_HALF_OUT);
			W->Flags &= ~(WEP_CHUTE_OUT);
		}
//		DoExplosionInAir(W, 0, NULL, NULL, planepnt);
//		DeleteBomb(W);
	}
	else
	{
		if (WeaponIntersectsGround(W))
		{
			DeleteChute(W);
		}
	}

	if(W == pPlayerChute)
	{
		CheckNotInPlayerPlane();
	}
}

//**************************************************************************************
void CheckNotInPlayerPlane()
{
	if(!pPlayerChute)
		return;

	if (Camera1.AttachedObjectType == CO_PLANE)
	{
		if(PlayerPlane == (PlaneParams *)Camera1.AttachedObject)
		{
			setup_no_cockpit_art();
			AssignCameraSubject((void *)pPlayerChute,CO_WEAPON);
		}
	}
	else
	{
		if ((Camera1.AttachedObjectType == CO_WEAPON) && pPlayerChute && (Camera1.AttachedObject == (void *)pPlayerChute) && (pPlayerChute->Type == pGuyOnGroundType) && (Camera1.CameraMode == CAMERA_COCKPIT))
		{
			AssignCameraSubject((void *)pPlayerChute,CO_WEAPON);
			setup_no_cockpit_art();
			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
		}
	}
}

//**************************************************************************************
void ChuteStayOnGround(WeaponParams *W)
{
	float speed = .17f;
	float dx, dz, offangle;
	WeaponParams *pilot;
	float tdist, goaldist;
	long lgoaldist;

	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
		W->Smoke = NULL;
	}


	W->Pos.Y = LandHeight( W->Pos.X, W->Pos.Z);

	int Index = AIGetWeaponIndex(W->Type->TypeNumber);

#if 0	//  Too much water in F/A-18, just sink guy down in water
	if(pDBWeaponList[Index].lWeaponID == 104)
	{
		if(!(GameLoop & 0xF))
		{
			if(InWater(W->Pos.X, W->Pos.Z))
			{
				DeleteChute(W);
			}
		}

		return;
	}
#endif

	if(W->Flags & WEP_DAMAGED_CHUTE)
	{
		W->Roll = 0;
		W->Pitch = 0x4000;
		W->Pos.Y += 1.0f * FTTOWU;
		if(InHighResTerrainRegion(W->Pos))
		{
			GetMatrix(W->Pos, W->Heading, W->Attitude);
			W->Attitude.GetRPH(&W->Roll,&W->Pitch,&W->Heading);
			W->Pitch += 0x4000;
			W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
		}
		if(!(GameLoop & 0xF))
		{
			if(InWater(W->Pos.X, W->Pos.Z))
			{
				DeleteChute(W);
			}
		}
		return;
	}

#if 0
	if(!(GameLoop & 0xF))
	{
		if(InWater(W->Pos.X, W->Pos.Z))
		{
			if(W->Type != pChaffType)
			{
				W->Type = pChaffType;
			}
		}
		else
		{
			if(W->Type == pChaffType)
			{
				W->Type = pGuyOnGroundType;
			}
		}
	}
#else
	W->Type = pGuyOnGroundType;
#endif

	if(WeaponHitsPlane(W))
	{
		W->Flags |= WEP_DAMAGED_CHUTE;
	}

	if(InWater(W->Pos.X, W->Pos.Z))
	{
		W->Pos.Y -= 2.0f * FTTOWU;
	}
	else
	{
		W->Pos.Y += 2.5f * FTTOWU;
	}

	if((W->Roll) || (W->Pitch))
	{
		W->Roll = W->Pitch = 0;
		W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
	}

	if(W->pTarget)
	{
		pilot = (WeaponParams *)W->pTarget;

		if((pilot >= Weapons) && (pilot <= LastWeapon))
		{
			if((pilot->Flags & WEAPON_INUSE) && (pilot->Kind == EJECTION_SEAT))
			{
				dx = pilot->Pos.X - W->Pos.X;
				dz = pilot->Pos.Z - W->Pos.Z;

				offangle = atan2(-dx, -dz) * 57.2958;

				W->Heading = AIConvert180DegreeToAngle(offangle);
				W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);

				lgoaldist = (((W - Weapons) & 0xF) * 2) + 4;

				goaldist = lgoaldist;
				goaldist *= FTTOWU;
				tdist = QuickDistance(dx, dz);

				if(tdist > goaldist)
				{
					W->Pos.X -= sin(DegToRad(offangle)) * speed;
					W->Pos.Z -= cos(DegToRad(offangle)) * speed;
				}
			}
		}
	}

	if(W == pPlayerChute)
	{
		CheckNotInPlayerPlane();
	}
}

//**************************************************************************************
void ChuteMoveToHelo(WeaponParams *W)
{
	float speed = 0.17f;
	float dx, dz, offangle, dy;
	PlaneParams *helo;
	float tdist, goaldist;

	if (W->Smoke)
	{
		((SmokeTrail *)W->Smoke)->Update();
		((SmokeTrail *)W->Smoke)->MakeOrphan();
		W->Smoke = NULL;
	}


	if((W->Roll) || (W->Pitch))
	{
		W->Roll = W->Pitch = 0;
		W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);
	}

#if 0
	if(!(GameLoop & 0xF))
	{
		if(InWater(W->Pos.X, W->Pos.Z))
		{
			if(W->Type != pChaffType)
			{
				W->Type = pChaffType;
			}
		}
		else
		{
			if(W->Type == pChaffType)
			{
				W->Type = pGuyOnGroundType;
			}
		}
	}
#else
	W->Type = pGuyOnGroundType;
#endif

	W->Pos.Y = LandHeight( W->Pos.X, W->Pos.Z);

	if(InWater(W->Pos.X, W->Pos.Z))
	{
		W->Pos.Y -= 2.0f * FTTOWU;
	}
	else
	{
		W->Pos.Y += 2.5f * FTTOWU;
	}

	if(WeaponHitsPlane(W))
	{
		W->Flags |= WEP_DAMAGED_CHUTE;
		W->WeaponMove = ChuteStayOnGround;
	}

	if(W->pTarget)
	{
		helo = (PlaneParams *)W->pTarget;

		if((helo >= Planes) && (helo <= LastPlane))
		{
			dx = helo->WorldPosition.X - W->Pos.X;
			dz = helo->WorldPosition.Z - W->Pos.Z;

			offangle = atan2(-dx, -dz) * 57.2958;

			W->Heading = AIConvert180DegreeToAngle(offangle);
			W->Attitude.SetRPH(W->Roll, W->Pitch, W->Heading);

			goaldist = 35.0f * FTTOWU;
			tdist = QuickDistance(dx, dz);

			if(tdist > goaldist)
			{
				W->Pos.X -= sin(DegToRad(offangle)) * speed;
				W->Pos.Z -= cos(DegToRad(offangle)) * speed;
			}
			else
			{
				dy = helo->WorldPosition.Y - W->Pos.Y;

				if(dy < (5.0f * WUTOFT))
				{
					if (Camera1.AttachedObjectType == CO_WEAPON)
					{
						if(W == (WeaponParams *)Camera1.AttachedObject)
						{
							setup_no_cockpit_art();
							AssignCameraSubject((void *)helo,CO_PLANE);
						}
					}


					W->P = NULL;
					W->fpTempWay.Z = 0;
					DeleteChute(W);
				}
			}
		}
	}

	if(W == pPlayerChute)
	{
		CheckNotInPlayerPlane();
	}
}

//**************************************************************************************
void BeginEjectionSequence(int planenum, int targetnum)
{
	int numejects;
	PlaneParams *P = &Planes[planenum];
	int sar_on_way = 0;

	if((Planes[planenum].AI.lPlaneID >= 69) && (Planes[planenum].AI.lPlaneID <= 78))
		return;

	for(WeaponParams *W = Weapons; W <= LastWeapon; W ++)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if((W->P == P) && (W->Kind == EJECTION_SEAT))
			{
				return;
			}
		}
	}

	if(!(P->AI.iAIFlags1 & AIPLAYERGROUP))
	{
		sar_on_way = AICheckForSAR(P);
	}

	if(pDBAircraftList[P->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_HELICOPTER))
	{
		return;
	}

	if(pDBAircraftList[P->AI.iPlaneIndex].OffSet[3].Z == 0)
	{
		if(pDBAircraftList[P->AI.iPlaneIndex].OffSet[3].Y == 0)
		{
			numejects = 1;
		}
		else
		{
			numejects = pDBAircraftList[P->AI.iPlaneIndex].OffSet[3].Y + 1;
		}
	}
	else
	{
		numejects = 2;
	}

	if(pDBAircraftList[P->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_TRANSPORT|AIRCRAFT_TYPE_TANKER|AIRCRAFT_TYPE_C3))
	{
		InstantiateBomberEjections(P, numejects, sar_on_way);
	}
	else
	{
		InstantiateEjectionSeat(P, numejects, sar_on_way);
	}
}



//***********************************************************************************************************************************
int PlaneHasAAWeapons(PlaneParams *P)
{
	BOOL bEmpty=TRUE;

	for(int i=0; i<MAX_F18E_STATIONS; i++)
	{
      if(P->WeapLoad[i].Count > 0)
	  {
			int Type = pDBWeaponList[P->WeapLoad[i].WeapIndex].iWeaponType;

			if( (Type == SHORT_RANGE) || (Type == MED_RANGE) )
			{
				bEmpty=FALSE;
			}
	  }
	}

	if (bEmpty)
	{
		int iAircraft=P-Planes;

		P->AI.iAICombatFlags1 |= AI_WINCHESTER_AA;

		LogMissionStatistic(LOG_FLIGHT_DEPLEATED_ALL_AA_WEAPON,iAircraft,1,0);

		if(P == PlayerPlane)
		{
			AIChangeEventFlag(MISSION_FLAG_START + 1);
		}

		if((!(P->AI.iAIFlags1&AIPLAYERGROUP)) && (P->AI.iAIFlags2 & AI_IN_DOGFIGHT) && ((g_nMissionType != SIM_TRAIN) || (g_nMissionType != SIM_QUICK)))
		{
			if(P->DamageFlags)
			{
				P->AI.lTimer1 = 0;
			}
			else
			{
				P->AI.lTimer1 /= 2;
			}
		}
	}
	return(!bEmpty);
}

//***********************************************************************************************************************************
int PlaneHasAGWeapons(PlaneParams *P)
{
	BOOL bEmpty=TRUE;

	for(int i=0; i<MAX_F18E_STATIONS; i++)
   	{
		if(P->WeapLoad[i].Count > 0)
		{
			int Type = pDBWeaponList[P->WeapLoad[i].WeapIndex].iWeaponType;
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
					bEmpty=FALSE;
				break;
			}
		}
	}

	if (bEmpty)
	{
		int iAircraft=P-Planes;

		P->AI.iAICombatFlags1 |= AI_WINCHESTER_AG;

		LogMissionStatistic(LOG_FLIGHT_DEPLEATED_ALL_AG_WEAPON,iAircraft,1,0);

		if(P == PlayerPlane)
		{
			AIChangeEventFlag(MISSION_FLAG_START);
		}
	}
	return(!bEmpty);
}

//***********************************************************************************************************************************
void UpdateNetWeaponTarget(int weaponid, int targetid)
{
	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(weaponid == W->iNetID)
			{
				if(targetid == -3)
				{
					DBWeaponType *pDBWeapon;

					if ((int)W->Type < 0x200)
					{
						pDBWeapon = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
					}
					else
					{
						pDBWeapon = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
					}

					if(pDBWeapon)
					{
						FPoint vel;
						vel.MakeVectorToFrom(W->Pos,W->LastPos);
						FPoint novel( 0.0f);

						NewCanister( CT_FLASH, W->Pos, novel, pDBWeapon->iDamageValue/50.0f);
						NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, 0.0f );
						NewGenerator(PLANE_EXPLODES,W->Pos,vel,1.0f,2);

						PlayPositionalExplosion(pDBWeapon->iWarHead,Camera1,W->Pos);
					}
					DeleteMissile(W);
				}
				else if(targetid == -2)
				{
					W->Flags |= FINAL_PHASE;
				}
				else if(targetid == -1)
				{
					W->pDecoyed = NULL;
				}
				else if(targetid == 0)
				{
					W->Flags       |= LOST_LOCK;
					W->fpTempWay.X  = -1;
					W->iTargetType  = 0;
					W->pTarget      = NULL;
					if((W->LauncherType == MOVINGVEHICLE) || (W->LauncherType == GROUNDOBJECT))
					{
						KillGSAM(W, 0);
					}
				}
//				else if(targetid < 9)
				else if(targetid < 256)
				{
					W->iTargetType = TARGET_PLANE;
					W->pTarget = &Planes[targetid - 1];
					W->Flags &= ~(ACTIVE_SEARCH|ACTIVE_SEEKER);
					W->lTimer = -1;

					DBWeaponType *pDBWeapon = &pDBWeaponList[ AIGetWeaponIndex(W->Type->TypeNumber) ];
					if((pDBWeapon->iSeekerType == 2) || (pDBWeapon->iSeekerType == 3))
					{
						LockHeaterOnEngine(&Planes[targetid - 1], W);
					}
				}
				else
				{
					for (WeaponParams *decoy=Weapons; decoy<=LastWeapon; decoy++)
					{
						if (decoy->Flags & WEAPON_INUSE)
						{
							if(decoy->iNetID == targetid)
							{
								W->pDecoyed = decoy;
								break;
							}
						}
					}
				}
				break;
			}
		}
	}
}

//***********************************************************************************************************************************
void SendNetWeaponTargetChange(int weaponid, int targetid, WeaponParams *W)
{
	if ( (PlayerPlane - Planes) != HIWORD( weaponid ))
	{
		if(W)
		{
			if(!MAIIsActiveWeapon(W))
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	//	MultiPlayer Remote Procedure Call
	void NetPutTarget( int weaponid, int targetid );

	if (MultiPlayer)
		NetPutTarget( weaponid, targetid );
}

//***********************************************************************************************************************************
float HeatSeekerSunFactor(ANGLE heading, ANGLE pitch, FPointDouble seekerpos, float maxangle, float *sunoffyaw, float *sunoffpitch)
{
	float fheading, fpitch;
	float sun_angle;
	float offpitch, offheading;
	float fceiling;
	float returnval = 0;
	float workval;
	FPointDouble targetpos;
	float horzperc = 1.0f;
	float sun_heading;
	float tdist;
	float sunoy, sunop;

	if(sunoffyaw)
	{
		*sunoffyaw = 0;
	}
	if(sunoffpitch)
	{
		*sunoffpitch = 0;
	}

	if(WorldParams.Weather & WR_CLOUD1)
	{
		fceiling = WorldParams.CloudAlt;
		if(seekerpos.Y < fceiling)
		{
			return(-2.0);
		}
	}

	sun_heading = atan2(WorldParams.SunX, WorldParams.SunZ) * 57.2958;

	fheading = AIConvertAngleTo180Degree(heading);

#if 0
	if((fabs(fheading) < (90.0f - maxangle)) || (fabs(fheading) > (90.0f + maxangle)))
	{
		return(-2.0);
	}
#else
//	offheading = fabs(AICapAngle(sun_heading - fheading));
	sunoy = AICapAngle(sun_heading - fheading);
	offheading = fabs(sunoy);
	if(offheading > maxangle)
	{
		return(-2.0f);
	}
#endif

	tdist = sqrt((WorldParams.SunX * WorldParams.SunX) + (WorldParams.SunZ * WorldParams.SunZ));
	if(WorldParams.SunX < 0)
	{
		tdist = -tdist;
	}

//	sun_angle = atan2(WorldParams.SunY,-WorldParams.SunX) * -57.2958;
	sun_angle = atan2(WorldParams.SunY,-tdist) * -57.2958;

	if((sun_angle < -5.0f) && (sun_angle > -175.0f))
	{
		return(-2.0);
	}
	else if(sun_angle <= -175.0f)
	{
		horzperc = 1.0f + ((-180 + sun_angle) / 5.0f);
	}
	else if(sun_angle < 0)
	{
		horzperc = 1.0f + (sun_angle / 5.0f);
	}

	fpitch = AIConvertAngleTo180Degree(pitch);

	if(fpitch < (-maxangle))
	{
		return(-2.0);
	}

	if(fheading > 0)
	{
		fpitch = 180.0f - fpitch;
	}

//	offpitch = fabs(fpitch - sun_angle);

	sunop = (sun_angle - fpitch);
	offpitch = fabs(sunop);

	if(offpitch > maxangle)
	{
		return(-2.0);
	}

	targetpos = seekerpos;
	targetpos.X -= WorldParams.SunX * (10.0f * NMTOWU);
	targetpos.Y -= WorldParams.SunY * (10.0f * NMTOWU);
	targetpos.Z -= WorldParams.SunZ * (10.0f * NMTOWU);

	if(!(AILOS(seekerpos, targetpos)))
	{
		return(-2.0);
	}

//	offheading = fabs(90.0f - fabs(fheading));

	if(offheading > offpitch)
	{
		returnval = 1.0f - (offheading / maxangle);
	}
	else
	{
		returnval = 1.0f - (offpitch / maxangle);
	}

	workval = sin(DegToRad(fabs(sun_angle - 90.0f))) * 0.5;

	returnval *= workval + 0.5f;

	returnval *= horzperc;

	if(sunoffyaw)
	{
		*sunoffyaw = sunoy;
	}
	if(sunoffpitch)
	{
		*sunoffpitch = sunop;
	}

	returnval *= 1.5f;  //  Make it so that sun can be "locked" on an more powerful than engines directly.

	return(returnval);
}

//***********************************************************************************************************************************
void SteerWeaponToSun(WeaponParams *W)
{
	Delta.X = -WorldParams.SunX * (10.0f * NMTOWU);
	Delta.Y = -WorldParams.SunY * (10.0f * NMTOWU);
	Delta.Z = -WorldParams.SunZ * (10.0f * NMTOWU);
}

/*----------------------------------------------------------------------------
 *
 *	KillGSAM(WeaponParams *W)
 *
 *		Detonate and unguided Ground Launched SAM
 *
 */
void KillGSAM(WeaponParams *W, int dodamage)
{
	FPoint vel;
	DBWeaponType *pDBWeapon = &pDBWeaponList[ AIGetWeaponIndex(W->Type->TypeNumber) ];

	vel.MakeVectorToFrom(W->Pos,W->LastPos);

	FPoint novel( 0.0f);
	NewCanister( CT_FLASH, W->Pos, novel, pDBWeapon->iDamageValue/50.0f);
	NewCanister( CT_MISSLE_EXPLODE,	W->Pos, novel, 0.0f );
	NewGenerator(PLANE_EXPLODES,W->Pos,vel,1.0f,2);
	if(dodamage)
	{
		DoExplosionInAir(W);
	}

	DeleteMissile(W, !dodamage);
}

//***********************************************************************************************************************************
int WCheckWeaponNowPlane(void *launcher, int launchertype, void *GroundTarget, int targettype, FPoint targetloc, int netid, int orgradardatnum, int flightprofile)
{
	PlaneParams *planepnt;
	PlaneParams *tplanepnt;
	MovingVehicleParams *vehiclepnt;
	AAWeaponInstance  *gweapon;
	GDRadarData		  *radardat;
	DBWeaponType *weapon;
	FPointDouble launchpos;
	ANGLE heading, pitch;
	int smoke = 0;
	int side;
	float offangle;
	double ybuffer = 50.0f;
	int cnt;
	int is_ai = 1;
	FPointDouble targetpos;
	float tdist;
	float fworkvar;
	float heightaboveground = 0;
	float initv = 0;
	PlaneParams *linked = NULL;
	int linkedtype = AIRCRAFT;
	float planeroll;
	double sinroll, cosroll, aoadeg, aoaoffset;
	int radardatnum = orgradardatnum &  ~(0x60);

#if 0  //  Desided to try something different but keeping this around in case that doesn't work and I want to try this again
	//  Unless I am the Host (whose message will make (radardatnum != orgradardatnum)) I don't want to process this from other planes (netid != -1).
	if((MultiPlayer) && (radardatnum == orgradardatnum) && (netid != -1) && (!g_bIAmHost))
		return(0);
#endif

	if(launchertype == MOVINGVEHICLE)
	{
		linked = (PlaneParams *)launcher;
		linkedtype = MOVINGVEHICLE;
		vehiclepnt = (MovingVehicleParams *)launcher;
		if (!vehiclepnt->iShipType)	// SCOTT FIX
		{
			if(radardatnum == 0)
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
			weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID);
		}
		launchpos = vehiclepnt->WorldPosition;
		if(weapon)
		{
			if((weapon->lWeaponID == 101) || (weapon->iVerticalLaunch))
			{
				pitch = AIConvert180DegreeToAngle(85.0f);
			}
			else
			{
				pitch = AIConvert180DegreeToAngle(45.0);
			}
		}
		smoke = 1;
		side = vehiclepnt->iSide;
	}
	else if(launchertype == GROUNDOBJECT)
	{
		linked = (PlaneParams *)launcher;
		linkedtype = GROUNDOBJECT;
		gweapon  = (AAWeaponInstance *)launcher;
		radardat = (GDRadarData *)gweapon->AIDataBuf;
		weapon = &pDBWeaponList[ radardat->iWeaponIndex ];
		launchpos = gweapon->Basics.Position;
		if(weapon)
		{
			if((weapon->lWeaponID == 101) || (weapon->iVerticalLaunch))
			{
				pitch = AIConvert180DegreeToAngle(85.0f);
			}
			else
			{
				pitch = AIConvert180DegreeToAngle(45.0);
			}
		}
		smoke = 1;
		side = GDConvertGrndSide((BasicInstance *) gweapon);
	}
	else if(launchertype == AIRCRAFT)
	{
		planepnt = (PlaneParams *)launcher;

		if(!(planepnt->Status & PL_ACTIVE))
			return(0);

		if(WIsWeaponPlane(planepnt))
			return(0);

		if(!(planepnt->Status & PL_AI_DRIVEN))
		{
			is_ai = 0;
		}
		weapon = &pDBWeaponList[ planepnt->WeapLoad[radardatnum].WeapIndex ];
		launchpos = GetHardPointPos( planepnt, radardatnum );
		heading = planepnt->Heading;
		pitch = planepnt->Pitch;

		planeroll = AIConvertAngleTo180Degree(planepnt->Roll);
		aoadeg = RadToDeg(planepnt->Alpha);  //  Counter AOA and give an extra downward boost.
		if(aoadeg > 5.0)
		{
			aoadeg = 5.0;
		}
		if(aoadeg < -5.0)
		{
			aoadeg = -5.0;
		}
		aoadeg += 5.0f;
		cosroll = cos(DegToRad(fabs(planeroll)));
		aoaoffset = cosroll * (double)aoadeg;
		pitch -= AIConvert180DegreeToAngle(aoaoffset);

		sinroll = sin(DegToRad(fabs(planeroll)));
		aoaoffset = sinroll * (double)aoadeg;
		heading -= AIConvert180DegreeToAngle(aoaoffset);

		side = planepnt->AI.iSide;
		heightaboveground = planepnt->HeightAboveGround;
		initv = planepnt->V;
		linked = planepnt;
		linkedtype = AIRCRAFT;
	}


	int planeid = WGetWeaponToPlaneID(weapon);

	if(planeid == -1)
	{
		return(0);
	}

	if((!g_bIAmHost) && (netid == -1) && (MultiPlayer))
	{
		return(1);
	}

	if((g_bIAmHost) || (!MultiPlayer))
	{
		for(planepnt = Planes; planepnt <= LastPlane; planepnt++)
		{
			if(!(planepnt->Status & PL_ACTIVE))
			{
				if(!(planepnt->AI.iAIFlags2 & AIINVISIBLE))
				{
					break;
				}
			}
		}

		if(planepnt > LastPlane)
		{
			if(LastPlane == &Planes[MAX_PLANES - 1])
			{
				return(0);
			}
			else
			{
				LastPlane = planepnt;
			}
		}
	}
	else
	{
		planepnt = &Planes[netid];
		if(planepnt > LastPlane)
		{
			if(LastPlane == &Planes[MAX_PLANES - 1])
			{
				return(0);
			}
			else
			{
				LastPlane = planepnt;
			}
		}
	}

#ifdef _DEBUG
	char tempstr[80];
	sprintf(tempstr, "Adding weapon plane %d", planepnt - Planes);
	AICAddAIRadioMsgs(tempstr, 50);
#endif

	WInitializeWeaponPlane(planepnt - Planes, planeid);

	planepnt->AI.pGroundTarget = GroundTarget;
	planepnt->AI.lGroundTargetFlag = targettype;

	if(launchpos.Y < 0)
	{
		launchpos.Y = 10.0f * FTTOWU;
	}

	if((launchertype == MOVINGVEHICLE) || (launchertype == GROUNDOBJECT))
	{
		launchpos.Y += (ybuffer * FTTOWU);
	}

	planepnt->WorldPosition = launchpos;
	planepnt->AI.iSide = side;
	planepnt->HeightAboveGround = heightaboveground;
	planepnt->BfLinVel.X = planepnt->V = initv;
	planepnt->AI.LinkedPlane = linked;
	planepnt->AI.iVar1 = linkedtype;

	if(GroundTarget)
	{
		if(targettype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)GroundTarget;
			planepnt->AI.WayPosition = vehiclepnt->WorldPosition;
		}
		else if(targettype == GROUNDOBJECT)
		{
			gweapon  = (AAWeaponInstance *)GroundTarget;
			planepnt->AI.WayPosition = gweapon->Basics.Position;
		}
		else if(targettype == AIRCRAFT)
		{
			tplanepnt = (PlaneParams *)GroundTarget;
			planepnt->AI.WayPosition = tplanepnt->WorldPosition;
		}
		else
		{
			planepnt->AI.WayPosition = targetloc;
		}
	}
	else
	{
		planepnt->AI.WayPosition = targetloc;
	}

	if(launchertype != AIRCRAFT)
	{
		launchpos = planepnt->AI.WayPosition;
		launchpos -= planepnt->WorldPosition;

		offangle = atan2(-launchpos.X, -launchpos.Z) * RADIANS_TO_DEGREES;

		heading = AIConvert180DegreeToAngle(offangle);
	}

	planepnt->Pitch = pitch;
	planepnt->Heading = heading;

	extern void RPH_to_Orientation( PlaneParams *P );
	RPH_to_Orientation(planepnt);

	if (weapon->iRocketMotor)
	{
		planepnt->Smoke[SMOKE_ENGINE_REAR] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);
		planepnt->EngineFires[SMOKE_ENGINE_REAR].TotalTimeLeft = weapon->iBurnTime * 50;
	}
	else if(smoke)
	{
		planepnt->Smoke[SMOKE_ENGINE_REAR] = SmokeTrails->NewSmokeTrail(planepnt,OFFSET_ENGINE_REAR,WHITE_MISSILE_SMOKE,MISSILE_FLAME_LIGHT);
		planepnt->EngineFires[SMOKE_ENGINE_REAR].TotalTimeLeft = 500;
	}
	else
	{
		planepnt->Smoke[SMOKE_ENGINE_REAR] = NULL;
	}

	planepnt->WeapLoad[0].Count = 1;
	planepnt->WeapLoad[0].WeapId = weapon->lWeaponID;
	planepnt->WeapLoad[0].WeapIndex = weapon - pDBWeaponList;
	for(cnt = 0; cnt < MAX_WEAPON_TYPES; cnt ++)
	{
		if(WeaponTypes[cnt].TypeNumber == weapon->lWeaponID)
		{
			planepnt->WeapLoad[0].Type = &WeaponTypes[cnt];
			break;
		}
		else if(WeaponTypes[cnt].TypeNumber == -1)
		{
			planepnt->Status = 0;
			return(0);
			break;
		}
	}

	if(planepnt->WeapLoad[0].Type)
	{
		WInstantiatePhantomWeapon(planepnt, launcher, launchertype, radardatnum, planepnt->WeapLoad[0].Type, GroundTarget, targettype);
	}

	if(launchertype == AIRCRAFT)
	{
		WeaponParams logweapon;

		logweapon.Type = planepnt->WeapLoad[0].Type;

		LogWeaponInstantiate((PlaneParams *)launcher, &logweapon);

		PlaneParams *P = (PlaneParams *)launcher;
		if((P == PlayerPlane) && (cPlayerLimitWeapons))
		{
			P->AircraftDryWeight -= weapon->iWeight;
			P->AI.fStoresWeight -= weapon->iWeight;
		}
		else if(P->Status & PL_AI_DRIVEN)
		{
			P->AI.fStoresWeight -= weapon->iWeight;
		}
		else if(cPlayerLimitWeapons)
		{
			P->AircraftDryWeight -= weapon->iWeight;
			P->AI.fStoresWeight -= weapon->iWeight;
		}

		if((pSeatData) && (P == PlayerPlane) && (weapon))
		{
			if(radardatnum <= LEFT5_STATION)
			{
				pSeatData->WeaponReleaseSide |= 2;
			}
			else if((radardatnum >= RIGHT7_STATION) && (radardatnum <= RIGHT11_STATION))
			{
				pSeatData->WeaponReleaseSide |= 1;
			}
			else
			{
				pSeatData->WeaponReleaseSide |= 0x3;
			}
			pSeatData->WeaponReleaseWeight += weapon->iWeight;
			pSeatData->WeaponReleaseID = weapon->lWeaponID;
		}
	}

	if(is_ai)
	{
		targetpos = planepnt->AI.WayPosition;
		launchpos = planepnt->WorldPosition;
		launchpos.Y = targetpos.Y;

		tdist = targetpos - launchpos;
		tdist *= WUTONM;

		fworkvar = tdist / (float)weapon->iRange;
		if(fworkvar < 0.4f)
		{
			planepnt->AI.lVar3 = LOW_LOW;
		}
		else if(fworkvar < 0.65f)
		{
			planepnt->AI.lVar3 = MED_ATTACK;
		}
		else
		{
			planepnt->AI.lVar3 = HIGH_ATTACK;
		}
		planepnt->AI.fVarA[0] = 0;
		planepnt->AI.lVar2 = WEAP_STORES_SMALL;
	}
	else
	{
		int searchmode;
		float searchrange;

		GetHarpoonData(&searchrange, &searchmode);
		planepnt->AI.fVarA[0] = searchrange;
		planepnt->AI.lVar2 = searchmode;
		planepnt->AI.lVar3 = flightprofile;
	}

	if(g_bIAmHost)
	{
		NetPutWeaponPlaneGeneral(launcher, launchertype, radardatnum, GroundTarget, targetloc, targettype, planepnt->AI.lVar3, planepnt - Planes);
	}

	planepnt->AI.WorldFormationPos = planepnt->WorldPosition;

//	if(MultiPlayer && (g_iWeaponID == (netid + 1)))
	if(MultiPlayer)
	{
		if(launchertype == AIRCRAFT)
		{
			PlaneParams *launchplane = (PlaneParams *)launcher;
			if((launchplane == PlayerPlane) || (launchplane && (launchplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)))
			{
				planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
			}
			else
			{
				planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;
			}
		}
		else if(g_bIAmHost)
		{
			planepnt->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
		}
		else
		{
			planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;
		}
	}
	else
	{
		planepnt->AI.iAICombatFlags1 &= ~AI_MULTI_ACTIVE;
	}

	return(1);
}

//***********************************************************************************************************************************
int WGetWeaponToPlaneID(DBWeaponType *weapon)
{
	if(!weapon)
	{
		return(-1);
	}

	switch(weapon->lWeaponID)
	{
		case 113:  //  SLAM
			return(69);
			break;
		case 114:  //  SLAM ER
			return(70);
			break;
		case 171:  //  Kh-22 (AS-4 Kitchen) / P-270
			return(71);
			break;
		case 172:  //  Kh-15 (AS-16 Kickback)
			return(72);
			break;
		case 173:  //  Kh-41 (3M80 Moskit)
			return(73);
			break;
		case 112:  //  AGM-84A Harpoon
			return(74);
			break;
		case 187:  //  P-300 (SS-N-19 Shipwreck)
			return(75);
			break;
		case 188:  //  4K80 (SS-N-12 Sandbox)
			return(76);
			break;
		case 189:  //  BGM-109B Tomahawk
			return(77);
			break;
		case 190:  //  RGM-84A Harpoon
			return(78);
			break;
		case 195:  //  SS-N-22 Sunburn
			return(84);
			break;
		case 140:	//  TALD
//		case 127:	//  Temp to test TALD
			return(85);
			break;
		case 100:  //  SS-N-2A Styx
			return(88);
			break;
		default:
			return(-1);
			break;
	}
	return(-1);
}

//***********************************************************************************************************************************
void WInitializeWeaponPlane(int planenum, int planeid)
{
	int cnt;

	int oldStatus = Planes[planenum].Status;
	//Can do this because PlaneParam is a class with the "=" operator overloaded!
	Planes[planenum] = F18_PlaneInit;
	Planes[planenum].Type = IGLoadPlaneType(planeid);

//	PlaneTypes[cnt].Model = Load3DObject("Plane.3d");  // Will Need eventually
//	GetSortHeader(&Planes[planenum],PLANE_OBJECT);
	Planes[planenum].Status = PL_ACTIVE | PL_AI_DRIVEN | PL_NEED_ATTITUDE;

	//  I moved the DataBase stuff into IGAddMover so that I could set Yaw rates and things like that.  SRE
	// DataBase Stuff
	Planes[planenum].AI.lPlaneID = planeid;
	Planes[planenum].AI.iPlaneIndex = GetPlaneIndex(Planes[planenum].AI.lPlaneID);


	Planes[planenum].Orientation.I.SetValues(0.0f,0.0f,-1.0f);

	Planes[planenum].Orientation.J.SetValues(1.0f,0.0f,0.0f);

	Planes[planenum].Orientation.K.SetValues(0.0f,-1.0f,0.0f);

	Planes[planenum].AI.DesiredRoll = 0;
	Planes[planenum].AI.DesiredPitch = 0;
	Planes[planenum].AI.DesiredHeading = 0;

	Planes[planenum].AI.lCallSign = 0;
	Planes[planenum].AI.cCallSignNum = 0;

	if(Planes[planenum].Status & AL_AI_DRIVEN)
	{
		Planes[planenum].ElevatorTrim = 1.0;  //  TEMP
		Planes[planenum].DoControlSim = CalcF18ControlSurfacesSimple;
		Planes[planenum].DoCASSim = CalcF18CASUpdatesSimple;
		Planes[planenum].DoForces = CalcAeroForcesSimple;
		Planes[planenum].DoPhysics = CalcAeroDynamicsSimple;

		Planes[planenum].MaxPitchRate = 10.0;
		Planes[planenum].MaxRollRate = 90.0;
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
	Planes[planenum].AI.Behaviorfunc = WeaponAsPlane;
	Planes[planenum].AI.OrgBehave = NULL;
	Planes[planenum].AI.CombatBehavior = NULL;

	Planes[planenum].AI.FormationPosition.ZeroLength();
	Planes[planenum].AI.iAIFlags1 = (AIDETAILALTCHECK);
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
	Planes[planenum].AI.lTimer2 = -1;

	Planes[planenum].AI.iAICombatFlags1 = AI_HOME_AIRPORT;
	Planes[planenum].AI.iHomeBaseId = -1;

	for(cnt = 0; cnt < 4; cnt ++)
	{
		Planes[planenum].AI.lMinAltFt[cnt] = 0;
//		Planes[planenum].AI.dwAltTimer[cnt] = 0;
		Planes[planenum].AI.fDistToHeight[cnt] = 0.0;
	}



	Planes[planenum].DesiredSpeed = Planes[planenum].BfLinVel.X = 700.0f;

//	Planes[planenum].iNumInGroup = oneobject.numingroup;
	Planes[planenum].AI.WayPosition.SetValues(-1.0f);

	Planes[planenum].AI.CurrWay = NULL;
	Planes[planenum].AI.numwaypts = 0;
	Planes[planenum].AI.startwpts = 0;

	Planes[planenum].AI.lCallSign = 0;
//	Planes[planenum].AI.iFuelLoad = oneobject.iFuelLoad;

	for(cnt = 0; cnt < AI_MAX_DAMAGE_SLOTS; cnt ++)
	{
		Planes[planenum].AI.cArmorDamage[cnt] = (char)125;  //  25 point of armor per item * 5
		Planes[planenum].AI.cInteriorDamage[cnt] = 0;
	}

	Planes[planenum].AI.lAIVoice = 0;

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

	Planes[planenum].AI.winglead = -1;
	Planes[planenum].AI.wingman = -1;
	Planes[planenum].AI.prevpair = -1;
	Planes[planenum].AI.nextpair = -1;

	for(cnt = 0; cnt < MAX_DELAYED_DAMAGE; cnt ++)
	{
		if(gDamageEvents[cnt].iPlaneNum == planenum)
		{
			gDamageEvents[cnt].iPlaneNum = -1;
			gDamageEvents[cnt].lDamageTimer = -1;
			gDamageEvents[cnt].lDamagedSystem = 0;
		}
	}
}

//*******************************************************************************************************************
int WIsWeaponPlane(PlaneParams *planepnt)
{
	if((planepnt->AI.lPlaneID >= 69) && (planepnt->AI.lPlaneID <= 78))
	{
		return(1);
	}
	if(planepnt->AI.lPlaneID == 84)
	{
		return(1);
	}
	if(planepnt->AI.lPlaneID == 85)
	{
		return(1);
	}
	if(planepnt->AI.lPlaneID == 88)
	{
		return(1);
	}
	return(0);
}

//*******************************************************************************************************************
//*
//*	Surface to Surface Cannon Stuff (Ground to Ground)
//*
//*******************************************************************************************************************
void InstantiateCannon(void *launcher, int launchertype, void *GroundTarget, int targettype, FPoint targetloc, int netid, int radardatnum)
{
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	void *targetpnt;
	BasicInstance *walker = NULL;
	MovingVehicleParams *vehiclepnt = NULL;
	FPointDouble position;
	FPointDouble targetpos;
	FPointDouble workpos;
	float offangle, tdist, offpitch;
	double ybuffer = 50.0f;
	DBWeaponType *pDBWeapon;
	FPointDouble weaponoffset;
	float fworkvar;
	int isship = 0;
	float missrad = 100.0f;
	float fworkrad;
	float fworkrange;
	int warheadtype;
	int fxindex = -1;

	weaponoffset.SetValues(0.0f, 0.0f, 0.0f);

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	W->Flags = MISSILE_INUSE;

	W->LaunchStation = -1;
	if (GroundTarget)
	{
		targetpnt = GroundTarget;
		if(targettype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)GroundTarget;
			targetpos = vehiclepnt->WorldPosition;
			W->iTargetType = TARGET_VEHICLE;
			if(vehiclepnt->iShipType)
			{
				isship = 1;
			}
			missrad = GetObjectRadius(vehiclepnt->Type->Model) * WUTOFT * 4.0f;
		}
		else
		{
			walker = (BasicInstance *)GroundTarget;
			targetpos = walker->Position;
			W->iTargetType = TARGET_GROUND;
		}
		W->pTarget = targetpnt;
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}
	else if (targetloc.X >= 0)
	{
		targetpnt = NULL;
		W->iTargetType = 0;
		W->pTarget = targetpnt;
		W->fpTempWay = targetloc;
		targetpos = targetloc;
		W->Flags |= BOOST_PHASE|GUIDED_WEAPON|STT_MISSILE;
		W->lTimer = 0;
		W->LifeTime = 0;
	}
	else
	{
		W->iTargetType = NONE;
		W->pTarget = NULL;
		W->fpTempWay.X = -1;
		W->Flags |= BOOST_PHASE;
		W->lTimer = 0;
		W->LifeTime = 0;
		targetpos = W->fpTempWay;
	}

	W->Kind = BULLET;
	W->WeaponMove = MoveCannonShell;

	W->P = NULL;
	W->iNetID			= netid;

	W->Launcher = launcher;
	W->LauncherType = launchertype;
	W->LaunchStation = radardatnum;
	if(launchertype == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)launcher;
		radardat = &vehiclepnt->RadarWeaponData[radardatnum];
		position = vehiclepnt->WorldPosition;

		VGDGetWeaponPosition(launcher, launchertype, radardatnum, &weaponoffset);
		position += weaponoffset;

		if(vehiclepnt->iShipType)
		{
		}
		else
		{
			if(radardatnum == 1)
			{
				fxindex = 0;
			}
		}

	}
	else
	{
		walker  = (BasicInstance *)launcher;
		radardat = GDGetRadarData(walker);
		position = walker->Position;
	}
	pDBWeapon = &pDBWeaponList[ radardat->iWeaponIndex ];
	W->Type = radardat->WType;

	W->InitialVelocity = GDGetWeaponVelocity(pDBWeapon) * FTTOWU / 50;

	workpos = targetpos;
	workpos -= position;

	offangle = atan2(-workpos.X, -workpos.Z) * RADIANS_TO_DEGREES;

	W->Heading = AIConvert180DegreeToAngle(offangle);

	if((fxindex != -1) && (vehiclepnt))
	{
		BYTE bdesiredangle;

		offangle -= AIConvertAngleTo180Degree(vehiclepnt->Heading);

		offangle = AICapAngle(offangle);

		while(offangle < 0)
		{
			offangle += 180;
		}

		offangle = (offangle / 360.0f) * 255.0f;

		bdesiredangle = offangle;
		bdesiredangle += 128;
		vehiclepnt->bFXDesiredArray[fxindex] = bdesiredangle;
	}

	tdist = QuickDistance(workpos.X,workpos.Z);

	offpitch = 45;

	W->Pitch = AIConvert180DegreeToAngle(offpitch);
	W->Attitude.SetHPR( W->Heading, W->Pitch, 0 );

#if 1
	double hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
	W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);

	if(position.Y < 0)
	{
		position.Y = 10.0f * FTTOWU;
	}

	W->Pos = position;
	WeaponInitAltitude(W);
	W->GroundHeight	= 0.0f;

	AIOffsetFromPoint(&W->Pos, AIConvertAngleTo180Degree(W->Heading), 0, 0, -10.0f * FTTOWU);

	W->SortType = MISSILE_OBJECT;
//	GetSortHeader(W,MISSILE_OBJECT);

	fworkvar = ((tdist * WUTOFT) / GDGetWeaponVelocity(pDBWeapon)) * 1000.0f;
	W->lBurnTimer = (long)fworkvar;

	W->Smoke = NULL;

	if(isship)
	{
		fworkrad = tan(DegToRad(1)) * tdist;
		W->lTargetDistFt = fworkrad;
	}
	else
	{
		fworkrange = pDBWeapon->iRange * FTTOWU;
		fworkrad = tdist / fworkrange;
		if(fworkrad < 0.5f)
		{
			W->lTargetDistFt = -1;
		}
		else
		{
			fworkvar = (fworkrad - 0.5f) * 2.0f;
			fworkrad = fworkvar * missrad * frand();
			W->lTargetDistFt = fworkrad;
		}
	}

	W->pDecoyed = NULL;

	//  Add smoke effect here
//	NewExplosionForcedWeapon(W->Pos, pDBWeapon->iDamageRadius * FTTOWU * 0.25, pDBWeapon->iDamageValue * 0.25, FALSE, 0, CT_NONE );
	fworkrad = (float)pDBWeapon->iDamageRadius * FTTOWU;  //   * 0.25;
	if(fworkrad <= 0)
		fworkrad = 1;

	fworkrange = (float)pDBWeapon->iDamageValue;  //   * 0.25;
	if(fworkrange <= 0)
		fworkrange = 1;

	NewExplosionForcedWeapon(W->Pos, fworkrad, fworkrange, FALSE, 0, CT_NONE );

	warheadtype = pDBWeapon->iWarHead;

	switch(warheadtype)
	{
		case WARHEAD_STANDARD_MEDIUM:
		break;
		case WARHEAD_STANDARD_LARGE:
			warheadtype = WARHEAD_STANDARD_MEDIUM;
		break;
		case WARHEAD_STANDARD_HUGE:
			warheadtype = WARHEAD_STANDARD_LARGE;
		break;
	}

	PlayPositionalExplosion(warheadtype,Camera1,W->Pos);

	W->Pos.Y = -10.0f;
}

//*******************************************************************************************************************
void MoveCannonShell(WeaponParams *W)
{
	FPointDouble position;
	BasicInstance *target;
	PlaneParams *ptarget;
	MovingVehicleParams *vtarget;
	double angzx, angzz;
	double heading;

	W->lBurnTimer -= 20;

	if(W->lBurnTimer < 0)
	{
		if (W->pTarget)
		{
			if(W->iTargetType == TARGET_GROUND)
			{
				target = (BasicInstance *)W->pTarget;
				position.X = target->Position.X;
				position.Y = target->Position.Y;
				position.Z = target->Position.Z;
				W->GroundHeight = position.Y;
			}
			else if(W->iTargetType == TARGET_PLANE)
			{
				ptarget = (PlaneParams *)W->pTarget;
				position = ptarget->WorldPosition;
				W->GroundHeight = position.Y;
			}
			else
			{
				vtarget = (MovingVehicleParams *)W->pTarget;
				position = vtarget->WorldPosition;
				W->GroundHeight = position.Y;
			}
		}
		else
		{
			target = NULL;
			position = W->fpTempWay;
			if(W->fpTempWay.X < 0)
			{
				DeleteMissile(W);
				return;
			}
		}

		heading = DegToRad((360.0f * frand()) - 180.0f);
		angzx = sin(heading);
		angzz = cos(heading);

		W->Pos.X = position.X + (angzx * W->lTargetDistFt);
		W->Pos.Y = position.Y;
		W->Pos.Z = position.Z + (angzz * W->lTargetDistFt);

		DoExplosionOnGround(W);

		DeleteMissile(W);
	}
}

//*******************************************************************************************************************
void WCheckAntiRadarLock(WeaponParams *W)
{
	FPoint position;
	BasicInstance *target;
	MovingVehicleParams *vtarget;
	PlaneParams *ptarget;
	AAWeaponInstance *gweapon;
	InfoProviderInstance *radarprovider = NULL;
	GDRadarData *radardat;
	double angzx, angzz;
	double heading;
	int iworkvar;
	FPointDouble fpd_position;

	W->lTimer = 5000;
	if (W->pTarget)
	{
		if(W->iTargetType == TARGET_GROUND)
		{
			target = (BasicInstance *)W->pTarget;
			position.X = target->Position.X;
			position.Y = target->Position.Y;
			position.Z = target->Position.Z;
			if(target->Family == FAMILY_INFOPROVIDER)
			{
				radarprovider = (InfoProviderInstance *)target;
				radardat = (GDRadarData *)radarprovider->AIDataBuf;
				if((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
				{
					if(!WCheckForHARMNotice(target, 1, W))
					{
						return;
					}
				}
			}
			else if(target->Family == FAMILY_AAWEAPON)
			{
				gweapon = (AAWeaponInstance *)target;
				radardat = (GDRadarData *)gweapon->AIDataBuf;
				if((radardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON)) && (!(radardat->lRFlags1 & (GD_RADAR_TEMP_OFF))))
				{
					if(!WCheckForHARMNotice(target, 0, W))
					{
						return;
					}
				}
			}
		}
		else if(W->iTargetType == TARGET_PLANE)
		{
			ptarget = (PlaneParams *)W->pTarget;
			position = ptarget->WorldPosition;
			W->GroundHeight = position.Y;
			if(W->Flags2 & MISSILE_HOME_ON_JAM)
			{
				if( ((ptarget->AI.iAIFlags1 & AIJAMMINGON) && (ptarget->AI.iAICombatFlags2 & AI_NOISE_JAMMER)) || (ptarget->AI.iAIFlags2 & AI_SO_JAMMING) )
				{
					return;
				}
				else
				{
					W->pTarget = NULL;
				}
			}
			else if(ptarget->AI.iAIFlags1 & AIRADARON)
			{
				return;
			}
		}
		else
		{
			vtarget = (MovingVehicleParams *)W->pTarget;
			position = vtarget->WorldPosition;
			if(vtarget->iShipType)
			{
				iworkvar = W->Flags2 & ANTI_RAD_SHIP_MASK;
				if(iworkvar < NUM_DEFENSE_LIST_ITEMS)
				{
					fpd_position = position;
					VGDGetWeaponPosition(vtarget, MOVINGVEHICLE, iworkvar, &fpd_position);
					position = fpd_position;
				}
			}
			W->GroundHeight = position.Y;
			if(vtarget->iShipType)
			{
				if(vtarget->RadarWeaponData[W->Flags2 & ANTI_RAD_SHIP_MASK].lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))
				{
					return;
				}
			}
			else
			{
				if(vtarget->RadarWeaponData[0].lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))
				{
					return;
				}
			}
		}
	}

	if(W->pTarget)
	{
		double distance = W->Pos - position;
		float fworkrad;

		fworkrad = tan(DegToRad(1)) * distance;

		heading = DegToRad((360.0f * frand()) - 180.0f);
		angzx = sin(heading);
		angzz = cos(heading);

		W->fpTempWay.X = position.X + (angzx * fworkrad);
		W->fpTempWay.Y = position.Y;
		W->fpTempWay.Z = position.Z + (angzz * fworkrad);
		W->pTarget = NULL;
	}
	else
	{
		WAntiRadarSearch(W);
	}
}

//*******************************************************************************************************************
int WGetStrongestShipRadar(MovingVehicleParams *vehiclepnt)
{
	GDRadarData *radardat;
	int maxrange = -1;
	int returnval = -1;
	DBRadarType *radar;
	int radarid;
	int slotnum;

	if(!vehiclepnt->iShipType)
	{
		return(returnval);
	}

	for(slotnum = 0; slotnum < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; slotnum ++)
	{
		radardat = &vehiclepnt->RadarWeaponData[slotnum];
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
		{
			radarid = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID;
			radar = GetRadarPtr(radarid);
			if(radar->iMaxRange > maxrange)
			{
				maxrange = radar->iMaxRange;
				returnval = slotnum;
			}
		}
	}
	return(returnval);
}

//*******************************************************************************************************************
void WHomeOnJammerSearch(WeaponParams *W)
{
	PlaneParams *checkplane;
  	void *foundobject = NULL;
	long objectflag;
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	float offangle, toffangle, toffpitch, offpitch;
	int widthok, heightok, visualok;
	float radarangleyaw, radaranglepitch;
	int startpass;
	int nlfrmult, nlrfrmult, frmult, rfrmult;
	int attacktype = 0;

	W->Flags |= ACTIVE_SEARCH|ACTIVE_SEEKER;

	nlfrmult = nlrfrmult = frmult = rfrmult = 0;

	visualrangenm = AI_VISUAL_RANGE_NM / 2;
	visualrange = visualrangenm * NMTOWU;
	rangenm = 80;  //  Get this from database SRE

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	foundobject = NULL;
	widthangle = 20;	//  might get from database
	heightangle = 20;	//  might get from database

	radarangleyaw = AIConvertAngleTo180Degree(W->Heading);

	radaranglepitch = AIConvertAngleTo180Degree(W->Pitch);

	startpass = 0;

	for(checkplane = Planes; ((checkplane <= LastPlane) && (startpass < 100)); checkplane ++)
	{
		if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (checkplane->OnGround == 0))
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = checkplane->WorldPosition.X - W->Pos.X;
			dy = checkplane->WorldPosition.Y - W->Pos.Y;
			dz = checkplane->WorldPosition.Z - W->Pos.Z;
			if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < foundrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					toffangle = offangle - radarangleyaw;
					offangle = AICapAngle(offangle);

				    toffangle = AICapAngle(toffangle);
					if(fabs(toffangle) < widthangle)
					{
						widthok = 1;
					}

					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					heightok = 1;

					if( ((checkplane->AI.iAIFlags1 & AIJAMMINGON) && (checkplane->AI.iAICombatFlags2 & AI_NOISE_JAMMER)) || (checkplane->AI.iAIFlags2 & AI_SO_JAMMING) )
					{
						visualok = 1;
					}

					if((widthok && heightok) && (visualok))
					{
						foundobject = checkplane;
						objectflag = TARGET_PLANE;
						foundrange = tdist;
					}
				}
			}
		}
	}

	if(foundobject)
	{
		W->pTarget = foundobject;
		W->iTargetType = objectflag;
	}
}

//*******************************************************************************************************************
void WAntiRadarSearch(WeaponParams *W)
{
  	BasicInstance *checkobject;
	MovingVehicleParams *vehiclepnt;
  	void *foundobject = NULL;
	long objectflag;
	float radarrange, widthangle, heightangle, rangenm;
	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	float offangle, toffangle, toffpitch, offpitch;
	int widthok, heightok, visualok;
	float radarangleyaw, radaranglepitch;
	long cnt_obj_flag;
	int pass, startpass;
	GDRadarData *radardat;
	int nlfrmult, nlrfrmult, frmult, rfrmult;
	int attacktype = 0;
	int iworkvar;

	if(W->Flags2 & MISSILE_HOME_ON_JAM)
	{
		WHomeOnJammerSearch(W);
		return;
	}

	nlfrmult = nlrfrmult = frmult = rfrmult = 0;

	visualrangenm = AI_VISUAL_RANGE_NM / 2;
	visualrange = visualrangenm * NMTOWU;
	rangenm = 40;  //  Get this from database SRE

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	foundobject = NULL;
	widthangle = 20;	//  might get from database
	heightangle = 20;	//  might get from database

	radarangleyaw = AIConvertAngleTo180Degree(W->Heading);

	radaranglepitch = AIConvertAngleTo180Degree(W->Pitch);

	startpass = 0;

	for(pass = startpass; pass < 2; pass ++)
	{
		if(pass == 0)
		{
	  		checkobject = (BasicInstance *)AllInfoProviders;
		}
		else
		{
	  		checkobject = (BasicInstance *)AllAAWeapons;
			nlfrmult = nlrfrmult = frmult = rfrmult = 1;
		}

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject))
			{
				widthok = 0;
				heightok = 0;
				visualok = 0;
				dx = checkobject->Position.X - W->Pos.X;
				dy = checkobject->Position.Y - W->Pos.Y;
				dz = checkobject->Position.Z - W->Pos.Z;
				if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
				{
					tdist = QuickDistance(dx, dz);
					if(tdist < foundrange)
					{
						offangle = atan2(-dx, -dz) * 57.2958;
	//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
						toffangle = offangle - radarangleyaw;
						offangle = AICapAngle(offangle);

						toffangle = AICapAngle(toffangle);
						if(fabs(toffangle) < widthangle)
						{
							widthok = 1;
						}

						offpitch = (atan2(dy, tdist) * 57.2958);
	//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
						toffpitch = offpitch - radaranglepitch;
						toffpitch = AICapAngle(toffpitch);
						heightok = 1;

						if((checkobject->Family == FAMILY_INFOPROVIDER) || (checkobject->Family == FAMILY_AAWEAPON))
						{
							radardat = GDGetRadarData(checkobject);
							if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
							{
								visualok = 1;
							}
						}


						if(widthok && heightok && (visualok))
						{
							cnt_obj_flag = 0;
							if((tdist * (nlrfrmult + 1)) < foundrange)
							{
								foundobject = checkobject;
								objectflag = TARGET_GROUND;
								foundrange = tdist;
							}
						}
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	for(vehiclepnt = MovingVehicles; ((vehiclepnt <= LastMovingVehicle) && (startpass < 100)); vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))))
		{
			widthok = 0;
			heightok = 0;
			visualok = 0;
			dx = vehiclepnt->WorldPosition.X - W->Pos.X;
			dy = vehiclepnt->WorldPosition.Y - W->Pos.Y;
			dz = vehiclepnt->WorldPosition.Z - W->Pos.Z;
			if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < foundrange)
				{
					offangle = atan2(-dx, -dz) * 57.2958;
//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
					toffangle = offangle - radarangleyaw;
					offangle = AICapAngle(offangle);

				    toffangle = AICapAngle(toffangle);
					if(fabs(toffangle) < widthangle)
					{
						widthok = 1;
					}

					offpitch = (atan2(dy, tdist) * 57.2958);
//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
					toffpitch = offpitch - radaranglepitch;
					toffpitch = AICapAngle(toffpitch);
					heightok = 1;

					if(VGDRadarOn(vehiclepnt))
					{
						visualok = 1;
					}

					if((widthok && heightok) && (visualok))
					{
						foundobject = vehiclepnt;
						objectflag = TARGET_VEHICLE;
						foundrange = tdist;
					}
				}
			}
		}
	}

	if(foundobject)
	{
		W->pTarget = foundobject;
		W->iTargetType = objectflag;
		if(objectflag == TARGET_VEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)foundobject;
			if(vehiclepnt->iShipType)
			{
				W->Flags2 &= ~(ANTI_RAD_SHIP_MASK);
				iworkvar |= WGetStrongestShipRadar(vehiclepnt);
				if(iworkvar >= 0)
				{
					W->Flags2 |= iworkvar & ANTI_RAD_SHIP_MASK;
				}
			}
		}
	}
}

//*******************************************************************************************************************
void WKillRadar(WeaponParams *W)
{
	BasicInstance *target;
	MovingVehicleParams *vtarget;
	AAWeaponInstance *gweapon;
	InfoProviderInstance *radarprovider = NULL;
	GDRadarData *radardat;

	if (W->pTarget)
	{
		if(W->iTargetType == TARGET_GROUND)
		{
			target = (BasicInstance *)W->pTarget;
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
			if((MultiPlayer) && MAIIsActiveWeapon(W))
			{
				NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_GROUND_NO_RADAR, GROUNDOBJECT, target->SerialNumber);
			}
		}
		else if(W->iTargetType == TARGET_PLANE)
		{
			return;
		}
		else
		{
			vtarget = (MovingVehicleParams *)W->pTarget;
			if(vtarget->iShipType)
			{
				radardat = (GDRadarData *)&vtarget->RadarWeaponData[W->Flags2 & ANTI_RAD_SHIP_MASK];
				radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
				radardat->lRFlags1 |= (GD_I_AM_DEAD);
				if((MultiPlayer) && MAIIsActiveWeapon(W))
				{
					NetPutGenericMessage2Long(NULL, GM2L_V_RADARS_DEAD, (1<<(W->Flags2 & ANTI_RAD_SHIP_MASK)), vtarget - MovingVehicles);
				}
			}
			else
			{
				radardat = (GDRadarData *)&vtarget->RadarWeaponData[0];
				radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
				radardat->lRFlags1 |= (GD_I_AM_DEAD);
				if((MultiPlayer) && MAIIsActiveWeapon(W))
				{
					NetPutGenericMessage2Long(NULL, GM2L_V_RADARS_DEAD, 1, vtarget - MovingVehicles);
				}
			}
		}
	}
}

//*******************************************************************************************************************
void MoveTorpedo(WeaponParams *W)
{
	PlaneParams *planepnt = NULL;
	long ldistft, ldistnm;
	int timerchange = 0;
	int ballistic = 0;
	int realweapons = 1;
	int weaponindex;
	PlaneParams *targplane = NULL;
	AAWeaponInstance *gweapon = NULL;
	GDRadarData *radardat = NULL;
	double ddist;
	FPoint position;
	BasicInstance *target;
	double desiredspeed;
	float speedinc = 0;
	MovingVehicleParams *vtarget;
	DBWeaponType *pweapon_type;
	int iworkvar;
	FPointDouble fpd_position;
	float accel;
	double hvel;

	if ((int)W->Type < 0x200)
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex((int)W->Type)];
	}
	else
	{
		pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	}

	if(pPadLockTarget == NULL)
	{
		W->Flags &= ~WEP_PADLOCKED;
	}

	weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
	maxyaw = (float)pDBWeaponList[weaponindex].iMaxYawRate / 50.0f;
	maxyaw *= 2;  // Values in database are a little low, but make a good easy level.

	desiredspeed = ((float)pDBWeaponList[weaponindex].fMaxSpeed / (FTSEC_TO_MLHR*MLHR_TO_KNOTS))/(WUTOFT*50.0);
//		desiredspeed *= CalcAltEffectOnMissile(W->Pos.Y);

	accel = (float)pDBWeaponList[weaponindex].iAcceleration;

	if(!accel)
	{
		accel = 6;
	}

	if (W->InitialVelocity < desiredspeed)
	{
		speedinc = accel/(WUTOFT*2500.0);

		W->InitialVelocity += speedinc;

		if (W->InitialVelocity > desiredspeed)
		{
			W->InitialVelocity = desiredspeed;
		}
	}
	else if (W->InitialVelocity > desiredspeed)
	{
		speedinc = (accel/(WUTOFT*2500.0)) * 8;

		W->InitialVelocity -= speedinc;

		if (W->InitialVelocity < desiredspeed)
		{
			W->InitialVelocity = desiredspeed;
		}
	}

	if (W->Smoke)
		((SmokeTrail *)W->Smoke)->MakeOrphan();
	W->Smoke = NULL;

	W->Flags &= ~(BOOST_PHASE);
	W->Flags |= GLIDE_PHASE;

	if((!W->pTarget) && (W->fpTempWay.X < 0))
	{
		ballistic = 1;
	}
	else if(W->iTargetType == TARGET_PLANE)
	{
		ballistic = 1;
	}
	else if ((W->iTargetType == TARGET_GROUND) || (W->iTargetType == TARGET_VEHICLE))
	{
		if (W->pTarget)
		{
			if(W->iTargetType == TARGET_GROUND)
			{
				target = (BasicInstance *)W->pTarget;
				position.X = target->Position.X;
				position.Y = target->Position.Y;
				position.Z = target->Position.Z;
				W->GroundHeight = position.Y;
			}
			else
			{
				vtarget = (MovingVehicleParams *)W->pTarget;
				position = vtarget->WorldPosition;
				if((vtarget->iShipType) && (pweapon_type->iSeekerType == 11))
				{
					iworkvar = W->Flags2 & ANTI_RAD_SHIP_MASK;
					if(iworkvar < NUM_DEFENSE_LIST_ITEMS)
					{
						fpd_position = position;
						VGDGetWeaponPosition(vtarget, MOVINGVEHICLE, iworkvar, &fpd_position);
						position = fpd_position;
					}
				}
				W->GroundHeight = position.Y;
			}
		}
		else
		{
			target = NULL;
			position = W->fpTempWay;
			if(W->fpTempWay.X < 0)
			{
				ballistic = 1;
			}
		}

		if(position.Y > (-10.0f * FTTOWU))
		{
			position.Y = (-10.0f * FTTOWU);
		}

		Delta.X = (position.X - W->Pos.X);
		Delta.Y = (position.Y - W->Pos.Y);
		Delta.Z = (position.Z - W->Pos.Z);

		ddist = (double)QuickDistance(Delta.X, Delta.Z);

		ldistft = ddist * WUTOFT;
		ldistnm = ldistft * FTTONM;

		W->lTargetDistFt = ldistft;
	}
	else
	{
		ballistic = 1;
	}

	if (!ballistic)
	{
		newAttitude(W);
	}
	else
	{
		if(W->Pos.Y > (80000.0f * FTTOWU))
		{
			goto Killer;
		}
	}

#if 1
	hvel = fabs(FCosTimes(W->Pitch,                  W->InitialVelocity));
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  hvel);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -hvel);
#else
	W->Vel.X =  FSinTimes(((ANGLE)0xFFFF-W->Heading),  W->InitialVelocity);
	W->Vel.Z =  FCosTimes(((ANGLE)0xFFFF-W->Heading), -W->InitialVelocity);
#endif
	W->Vel.Y =  FSinTimes(W->Pitch,                    W->InitialVelocity);

	W->LastPos = W->Pos;	// HitPlane() & WeaponIntersectsGround() need this
	W->Pos += W->Vel;

	if ((W->Flags & COLLISION_CHECK) && (W->Altitude < (500.0 FEET)))
	{
		if (/*!W->CollisionCheckTimer-- &&*/ CheckMissileAgainstGroundTargets(W))
			goto Killer;
	}

	// update total distance traveled
	W->LifeTime += sqrt( (W->Vel.X*W->Vel.X) + (W->Vel.Y*W->Vel.Y) + (W->Vel.Z*W->Vel.Z) ) * 1000.0f;

	if((W->LifeTime > ((float)pDBWeaponList[weaponindex].iRange * NMTOWU * 1000.0f)) || (!W->pTarget))
	{
		W->Pos.Y = 0;
		DoExplosionOnGround(W);
		goto Killer;
	}

	return;		// normal exit

   //------------------------
  //
 //	Delete the spent missile.
//
Killer:
	DeleteMissile(W);
}

//*******************************************************************************************************************
int WGetNumWeaponsShownOnStation(PlaneParams *planepnt, int stationnum)
{
	if((pDBWeaponList[planepnt->WeapLoad[stationnum].WeapIndex].lWeaponID == 133) || (pDBWeaponList[planepnt->WeapLoad[stationnum].WeapIndex].lWeaponID == 134) || (pDBWeaponList[planepnt->WeapLoad[stationnum].WeapIndex].lWeaponID == 176))
	{
		return(planepnt->WeapLoad[stationnum].bNumPods);
	}
	else
	{
		return(planepnt->WeapLoad[stationnum].Count);
	}
	return(0);
}

//*******************************************************************************************************************
int WFoundOtherLaser(WeaponParams *W, int seeker, FPointDouble *rposition)
{
	float foffhead = 60.0f;
	float offhead;
	int foundone = 0;
	void *pTarget;
	int iTargetType;
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	FPoint position;
	FPoint	foundpos;
	float dx, dy, dz;
	int cnt;
	float secs;
	FPoint ImpactPoint;
	DBWeaponType *pweapon_type = &pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)];
	int isbomb = 0;

	position.SetValues(-1.0f, -1.0f, -1.0f);
	*rposition = position;

	if((pweapon_type->iWeaponType == WEAPON_TYPE_GUIDED_BOMB) || (pweapon_type->iWeaponType == WEAPON_TYPE_DUMB_BOMB))
	{
		secs = GetImpactTime(W, W->Vel.Y, &ImpactPoint);


		if(secs > 15.0f)
			return(0);

		isbomb = 1;
	}

	if(iFACHitFlags & FAC_LASER)
	{
		pTarget = pFACTarget;
		iTargetType = iFACTargetType;
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
			pTarget = NULL;
			position.X = -1.0f;
			position.Z = -1.0f;
		}

		if ((position.X > -1) && (position.Z > -1))
		{
			dx = position.X - W->Pos.X;
			dy = position.Y - W->Pos.Y;
			dz = position.Z - W->Pos.Z;

			offhead = atan2(-dx, -dz) * 57.2958;

			offhead = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(offhead) - W->Heading);

			if(fabs(offhead) <= foffhead)
			{
				if(QuickDistance(dx, dz) < (BUDDY_LAS_RANGE_NM * NMTOWU))
				{
					foffhead = fabs(offhead);
					*rposition = position;
					foundone = -2;
				}
			}
		}
	}

	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			if(cnt == MySlot)
				continue;

			if(iSlotToPlane[cnt] >= 0)
			{
				if(Planes[iSlotToPlane[cnt]].Status & PL_ACTIVE)
				{
					position = NetBuddyLasingData[cnt].laserloc;

					if ((position.X > -1) && (position.Z > -1))
					{
						dx = position.X - W->Pos.X;
						dy = position.Y - W->Pos.Y;
						dz = position.Z - W->Pos.Z;

						offhead = atan2(-dx, -dz) * 57.2958;

						offhead = AIConvertAngleTo180Degree(AIConvert180DegreeToAngle(offhead) - W->Heading);

						if(fabs(offhead) <= foffhead)
						{
							if(QuickDistance(dx, dz) < (BUDDY_LAS_RANGE_NM * NMTOWU))
							{
								foffhead = fabs(offhead);
								*rposition = position;
								foundone = cnt + 1;
							}
						}
					}
				}
			}
		}
	}

	if(foundone == -2)
	{
		if(!(iFACHitFlags & FAC_LASER_ON))
		{
			if(!isbomb)
			{
		 		AICAddSoundCall(AIC_GenericFACCall, -2, 3000 + ((rand() & 0x3) * 1000), 50, (1 + (rand() & 0x1)));
			}
			else
			{
				AIC_GenericFACCall(-2, 1 + (rand() & 0x1));
			}
			iFACHitFlags |= FAC_LASER_ON;
		}
	}

	return(foundone);
}

#define IR_SEEKER_RANGE 2.0f
//**************************************************************************************
float WCheckIRAspect(DBWeaponType *pDBWeapon, WeaponParams *W, PlaneParams *target)
{
	float offangle, offpitch;
	float offangle2, offpitch2;
	float rangenm, rangemod, tdist;
	float percent;
	float fHeat;
	float offbore, offboreperc, alteffect;
	float offbore2, offboreperc2;

	if(!target)
		return(0);

	// Check for occlusions  (Moved into WCheckIRAspect
	if(W->Altitude > (200.0f * FTTOWU))
	{
		if (!AILOS( W->Pos, target->WorldPosition ))  return 0;
	}

	WGetRelativeHP(W, target->WorldPosition, &offangle, &offpitch);
	if((fabs(offangle) > pDBWeapon->iSeekerFOV) || (fabs(offpitch) > pDBWeapon->iSeekerFOV))
	{
		return(0);
	}

	offbore = fabs(offangle);
	if(fabs(offpitch) > offbore)
	{
		offbore = fabs(offpitch);
	}
	offboreperc = (offbore / 180.0f);

	PLGetRelativeHP(target, W->Pos, &offangle2, &offpitch2);
	offbore2 = fabs(offangle2);
	if(fabs(offpitch2) > offbore2)
	{
		offbore2 = fabs(offpitch2);
	}
	offboreperc2 = (offbore2 / 180.0f);

	tdist = (target->WorldPosition - W->Pos) * WUTONM;
//	rangenm = pDBWeapon->iRange;
	rangenm = IR_SEEKER_RANGE;

	fHeat = 0.5;	// = T->Knots / 500;
	if (target->LeftThrustPercent > 80)  fHeat *= 1 + (target->LeftThrustPercent - 80) / 20;
	if (target->Knots > 400)				fHeat *= 1 + (((target->Knots - 400) / 500) * (1.0f - offboreperc2));
	if (fHeat >  1)  fHeat = 1;		// clip to 1.0
	if (fHeat <= 0)  return  0;
	fHeat *= 2.0f;

	if((W->Pos.Y * WUTOFT) <= 20000.0f)
	{
		alteffect = 1.0f;
	}
	else
	{
		alteffect = CalcAltEffectOnMissile(W->Pos.Y);
	}

//	rangemod = 1.0f * fHeat * (1.0f + offboreperc2) * alteffect;  // this will be modified by aspect, AB, and possibly speed
	rangemod = 1.0f * fHeat * (1.0f + (offboreperc2 * 1.5f)) * alteffect;  // this will be modified by aspect, AB, and possibly speed

	if(pDBWeapon->iECMResistance & ECM_DECOY)
		rangemod *= 1.5f;

	rangenm *= rangemod;

	if(rangenm < tdist)
	{
		return(0);
	}

	if((rangenm * 0.5f) > tdist)
	{
		return(1.0f);
	}

	percent = ((rangenm - tdist) / (rangenm)) * 2.0f;

	if(percent < 0.04f)
		percent = 0.04f;

	if(percent > 1.0f)
		return(1.0f);

	return(percent);
}

//**************************************************************************************
float WCheckIRAspect(DBWeaponType *pDBWeapon, PlaneParams *planepnt, PlaneParams *target)
{
	float offangle, offpitch;
	float offangle2, offpitch2;
	float rangenm, rangemod, tdist;
	float percent;
	float fHeat;
	float offbore, offboreperc, alteffect;
	float offbore2, offboreperc2;
	float edgeperc = 1.0f;

	if(!target)
		return(0);

	// Check for occlusions  (Moved into WCheckIRAspect
	if (!AILOS( planepnt->WorldPosition, target->WorldPosition ))  return 0;

	PLGetRelativeHP(planepnt, target->WorldPosition, &offangle, &offpitch);
	if((fabs(offangle) > pDBWeapon->iSeekerFOV) || (fabs(offpitch) > pDBWeapon->iSeekerFOV))
	{
		return(0);
	}

	offbore = fabs(offangle);
	if(fabs(offpitch) > offbore)
	{
		offbore = fabs(offpitch);
	}
	offboreperc = (offbore / 180.0f);

	PLGetRelativeHP(target, planepnt->WorldPosition, &offangle2, &offpitch2);
	offbore2 = fabs(offangle2);
	if(fabs(offpitch2) > offbore2)
	{
		offbore2 = fabs(offpitch2);
	}
	offboreperc2 = (offbore2 / 180.0f);

	if((offbore - (float)pDBWeapon->iSeekerFOV) < 2.0f)
	{
		edgeperc = ((offbore - (float)pDBWeapon->iSeekerFOV) / 2.0f);
	}

	tdist = (target->WorldPosition - planepnt->WorldPosition) * WUTONM;

	//  rangenm = pDBWeapon->iRange;
	rangenm = IR_SEEKER_RANGE;

	fHeat = 0.5;	// = T->Knots / 500;
	if (target->LeftThrustPercent > 80)  fHeat *= 1 + (target->LeftThrustPercent - 80) / 20;
	if (target->Knots > 400)				fHeat *= 1 + (((target->Knots - 400) / 500) * (1.0f - offboreperc2));
	if (fHeat >  1)  fHeat = 1;		// clip to 1.0
	if (fHeat <= 0)  return  0;
	fHeat *= 2.0f;

	if((planepnt->WorldPosition.Y * WUTOFT) <= 20000.0f)
	{
		alteffect = 1.0f;
	}
	else
	{
		alteffect = CalcAltEffectOnMissile(planepnt->WorldPosition.Y);
	}

	rangemod = 1.0f * fHeat * (1.0f + (offboreperc2 * 1.5f)) * alteffect;  // this will be modified by aspect, AB, and possibly speed

	if(pDBWeapon->iECMResistance & ECM_DECOY)
		rangemod *= 1.5f;

	rangenm *= rangemod;

	if(rangenm < tdist)
	{
		return(0);
	}

	if((rangenm * 0.5f) > tdist)
	{
		return(1.0f);
	}

	percent = ((rangenm - tdist) / (rangenm)) * 2.0f;

	if(percent < 0.04f)
		percent = 0.04f;

	if(percent > 1.0f)
		return(1.0f);

	return(percent);
}

//**************************************************************************************
void WGetRelativeHP(WeaponParams *W, FPointDouble worldposition, float *heading, float *pitch)
{
	FPointDouble relative_position;
	FMatrix de_rotate;

	relative_position.MakeVectorToFrom(worldposition,W->Pos);

	de_rotate = W->Attitude;
	de_rotate.Transpose();

	relative_position *= de_rotate;
	relative_position.Normalize();

	*heading = atan2(-relative_position.X,-relative_position.Z) * 57.2958;
	*pitch = asin(relative_position.Y) * 57.2958;
}

//**************************************************************************************
int WCheckForHARMNotice(BasicInstance *target, int infoprovider, WeaponParams *W)
{
	AAWeaponInstance *gweapon;
	InfoProviderInstance *radarprovider = NULL;
	DBRadarType *radar;
	int radarid;
	float secsoff;
	float tdistft;
	BasicInstance *clients;
	GDRadarData *radardat;
	int cnt;
	int childrenOK = 0;

	radar = GetRadarPtr(target);
	if(!radar)
		return(0);

	radarid = radar->lRadarID;

	if(!((radarid == 8) || (radarid == 9) || (radarid == 17) || (radarid == 47) || (radarid == 49)))
	{
		return(0);
	}
	else
	{
		float frandnum = (frand() * 100.0f);
		if(frandnum > 7.0f)		//  7% chance per 5 seconds that harm is noticed.
			return(0);
	}

	tdistft = (W->Pos - target->Position) * WUTOFT;
	secsoff = (tdistft / (W->InitialVelocity * 50.0f * WUTOFT)) * 3000.0f;  // 3 for long enough off and 1000 for 1000/sec

	if(infoprovider)
	{
		radarprovider = (InfoProviderInstance *)target;
		childrenOK = 1;
	}
	else
	{
		gweapon = (AAWeaponInstance *)target;
		radarprovider = (InfoProviderInstance *)gweapon->OurProvider;
	}

	if(radarprovider)
	{
		radardat = (GDRadarData *)radarprovider->AIDataBuf;
		radardat->lRFlags1 |= (GD_RADAR_TEMP_OFF|GD_RADAR_OFF_FOR_HARM);
		radardat->lRActionTimer = secsoff;
		radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;
		radardat->Target = NULL;
		for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
		{
			clients = radarprovider->Clients[cnt];
			if(clients != NULL)
			{
				if(clients->Family == FAMILY_AAWEAPON)
				{
					gweapon = (AAWeaponInstance *)clients;
					radardat = (GDRadarData *)gweapon->AIDataBuf;
					radardat->lRFlags1 |= (GD_RADAR_TEMP_OFF|GD_RADAR_OFF_FOR_HARM);
					radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;
					radardat->lRActionTimer = secsoff;
					radardat->Target = NULL;
					childrenOK = 2;
				}
			}
		}
	}

	if((childrenOK == 1) && (radarprovider))
	{
		radarprovider = radarprovider->OurProvider;
		if(radarprovider)
		{
			radardat = (GDRadarData *)radarprovider->AIDataBuf;
			radardat->lRFlags1 |= (GD_RADAR_TEMP_OFF|GD_RADAR_OFF_FOR_HARM);
			radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;
			radardat->lRActionTimer = secsoff;
			radardat->Target = NULL;
			for(cnt = 0; cnt < MAX_COMPOUND_CHILDREN; cnt ++)
			{
				clients = radarprovider->Clients[cnt];
				if(clients != NULL)
				{
					if(clients->Family == FAMILY_AAWEAPON)
					{
						gweapon = (AAWeaponInstance *)clients;
						radardat = (GDRadarData *)gweapon->AIDataBuf;
						radardat->lRFlags1 |= (GD_RADAR_TEMP_OFF|GD_RADAR_OFF_FOR_HARM);
						radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;
						radardat->lRActionTimer = secsoff;
						radardat->Target = NULL;
						childrenOK = 2;
					}
				}
			}
		}
	}

	return(1);
}

//**************************************************************************************
void WJettisonRocketPod(PlaneParams *planepnt, int Station)
{
	int org_simjett = Cpit.SimJettOn;
	int weight;
	int fullweight = pDBWeaponList[planepnt->WeapLoad[Station].WeapIndex].iWeight;
	int weapid;
	int fullpod;
	float fworkweight;

	if(planepnt->WeapLoad[Station].bNumPods)
	{
		Cpit.SimJettOn = 1;
		weapid = planepnt->WeapLoad[Station].WeapId;
		if(weapid == 133)
		{
			weight = 131;
			fullpod = 4;
		}
		else
		{
			weight = 21;
			fullpod = 19;
		}

		if(planepnt->WeapLoad[Station].bNumPods > 1)
		{
			fworkweight = fullweight - ((fullpod - planepnt->WeapLoad[Station].Count) * weight);
			if((planepnt == PlayerPlane) && (cPlayerLimitWeapons))
			{
				planepnt->AircraftDryWeight -= (fworkweight - (weight * 2.0f));
				planepnt->AI.fStoresWeight -= (fworkweight - (weight * 2.0f));
			}
			else if(planepnt->Status & PL_AI_DRIVEN)
			{
				planepnt->AI.fStoresWeight -= (fworkweight - (weight * 2.0f));
			}
			else if(cPlayerLimitWeapons)
			{
				planepnt->AircraftDryWeight -= (fworkweight - (weight * 2.0f));
				planepnt->AI.fStoresWeight -= (fworkweight - (weight * 2.0f));
			}
			planepnt->WeapLoad[Station].Count = 2;  //  20;

			if((pSeatData) && (planepnt == PlayerPlane))
			{
				if(Station <= LEFT5_STATION)
				{
					pSeatData->WeaponReleaseSide |= 2;
				}
				else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
				{
					pSeatData->WeaponReleaseSide |= 1;
				}
				else
				{
					pSeatData->WeaponReleaseSide |= 0x3;
				}
				pSeatData->WeaponReleaseWeight += (fworkweight - (weight * 2.0f));
				pSeatData->WeaponReleaseID = -2;
			}
		}
		else if(planepnt->WeapLoad[Station].bNumPods)
		{
			fworkweight = fullweight - ((fullpod - planepnt->WeapLoad[Station].Count) * weight);
			if((planepnt == PlayerPlane) && (cPlayerLimitWeapons))
			{
				planepnt->AircraftDryWeight -= fworkweight;
				planepnt->AI.fStoresWeight -= fworkweight;
			}
			else if(planepnt->Status & PL_AI_DRIVEN)
			{
				planepnt->AI.fStoresWeight -= fworkweight;
			}
			else if(cPlayerLimitWeapons)
			{
				planepnt->AircraftDryWeight -= fworkweight;
				planepnt->AI.fStoresWeight -= fworkweight;
			}
			planepnt->WeapLoad[Station].Count = 1;

			if((pSeatData) && (planepnt == PlayerPlane))
			{
				if(Station <= LEFT5_STATION)
				{
					pSeatData->WeaponReleaseSide |= 2;
				}
				else if((Station >= RIGHT7_STATION) && (Station <= RIGHT11_STATION))
				{
					pSeatData->WeaponReleaseSide |= 1;
				}
				else
				{
					pSeatData->WeaponReleaseSide |= 0x3;
				}
				pSeatData->WeaponReleaseWeight += fworkweight;
				pSeatData->WeaponReleaseID = -2;
			}
		}
		InstantiateBomb(planepnt,Station,0);
		planepnt->WeapLoad[Station].bNumPods = planepnt->WeapLoad[Station].bNumPods - 1;
	}
	else
	{
		planepnt->WeapLoad[Station].Count = 0;
	}
	Cpit.SimJettOn = org_simjett;
}

//**************************************************************************************
int WIsRetardedWeapon(int weapid)
{
	if((weapid == 8) || (weapid == 9) || (weapid == 126) || (weapid == 128))
	{
		return(1);
	}

	return(0);
}

//**************************************************************************************
void WInstantiatePhantomWeapon(PlaneParams *P, void *launcher, int launchertype, int Station, WeaponType *Type, void *GroundTarget, int targettype)
{
	int netid = g_iWeaponID++;

	WeaponParams *W = GetWeaponSlot();
	if (!W)  return;

	W->Flags = MISSILE_INUSE;
	W->Flags2 = 0;
	W->Kind = MISSILE;
	W->WeaponMove = WMovePhantomWeapon;
	if(launchertype == AIRCRAFT)
	{
		W->P = (PlaneParams *)launcher;
	}
	else
	{
		W->P = NULL;
	}

	W->iTargetType = NONE;
	W->pTarget = P;

	W->iNetID			= netid;

	W->Launcher = launcher;
	W->LauncherType = launchertype;
    W->Type = Type;
	W->Attitude = P->Attitude;
	W->Heading = P->Heading;
	W->Pitch = P->Pitch;
	W->Pos = P->WorldPosition;
	W->LaunchStation = Station;
	P->AI.fVarA[1] = W - Weapons;
	W->Smoke = NULL;
	W->Vel.SetValues( HIGH_FREQ * FTTOWU, P->IfVelocity );
	W->InitialVelocity = P->V;
	W->Altitude = W->Pos.Y - LandHeight( W->Pos.X, W->Pos.Z);
}

//**************************************************************************************
void WMovePhantomWeapon(WeaponParams *W)
{
	PlaneParams *P = (PlaneParams *)W->pTarget;

	if(!P)
	{
		DeleteBomb(W);
		return;
	}

	W->Attitude = P->Attitude;
	W->Heading	= P->Heading;
	W->Pitch	= P->Pitch;
	W->Pos = P->WorldPosition;
	W->Vel.SetValues( HIGH_FREQ * FTTOWU, P->IfVelocity );
	W->InitialVelocity = P->V;
	W->Altitude = W->Pos.Y - LandHeight( W->Pos.X, W->Pos.Z);

	if(!(P->Status & PL_ACTIVE))
	{
		DeleteBomb(W);
	}
}

//**************************************************************************************
int WGetGunSndLevel(int startlevel)
{
	int worklevel = startlevel;
	ANGLE camera_heading, camera_pitch, camera_roll;
	float dx,dy,dz,fDist;
	ANGLE offangle;
	float fCanHearYou = (1320 * FTTOWU);	// 1/4 Mile
	int iGunVolume;
	float behind_perc = 0;

	Camera1.CameraMatrix.GetRPH(&camera_roll,&camera_pitch,&camera_heading);
	if(Camera1.CameraMode == CAMERA_COCKPIT)
	{
		return(worklevel);
	}

	dx = PlayerPlane->WorldPosition.X - Camera1.CameraLocation.X;
	dy = PlayerPlane->WorldPosition.Y - Camera1.CameraLocation.Y;
	dz = PlayerPlane->WorldPosition.Z - Camera1.CameraLocation.Z;
	fDist = QuickDistance3D(dx,dy,dz);

	if ((fDist<fCanHearYou) && (fDist>0))
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
		iGunVolume = ((1.0f - fVolPercent)*(worklevel*(1.0f - (behind_perc * 0.80f))));
		if (iGunVolume<0) iGunVolume=0;
		if (iGunVolume>startlevel) iGunVolume=startlevel;
		return(iGunVolume);
	}
	else
	{
		return(0);
	}
	return(worklevel);
}