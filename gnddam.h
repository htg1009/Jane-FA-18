#ifndef _GNDDAM_H_

#define _GNDDAM_H_

#define MACH_1_GU_PER_50TH	(1087.1 FEET * 1/50.0)

#define MAX_SHOCK_WAVES					25
#define MAX_SCHEDULED_EXPLOSIONS		50
#define MAX_CRATERS						50
#define MAX_CRATER_MODELS				10

#define CRATER_CALC_HEIGHT				1

#define SW_INUSE	1
#define SW_IN_WATER	2

typedef struct _ShockWave
{
	ObjectSortHeader 	*SortHeader;
	FPointDouble		Position;

	DWORD				Flags;
	float				MaxRadius;
	float				OneOverMaxArea;
	float				FirstArea;
	float				Radius;
	float				ColorSpread;
	DWORD				LastLoop;
	FPoint				Normal;
	FMatrix				Matrix;

	BasicInstance		**Objects;
} ShockWave;

#define SE_INUSE			0x01
#define SE_BIG_BANG			0x02
#define SE_USE_EXP			0x04
#define SE_NO_DAMAGE		0x08
#define SE_CULL_LIGHTS		0x10

typedef struct _ScheduledExplosion
{
	int 			TicksLeft;
	FPointDouble 	Center;
	DBWeaponType 	*WarheadType;
	BasicInstance   *Instance;
	void 			*Vehicle;
	int				DamageSize;
	float			DamageRadius;
	int				FireType;
	float			FireSize;
	float			FireDuration;
	int				WarheadForSound;
	BYTE 			ExplosionsLeft;
	BYTE 			Radius;
	WORD 			Flags;
	SHORT 			TimeBetweenExplosions;
	SHORT 			Deviation;
} ScheduledExplosion;

typedef struct _Crater
{
	LPDIRECT3DTEXTURE2	D3DTexture;
	WORD 				CraterFlags;
	WORD 				MapFlags;
	float 				ScaleRight;
	float 				ScaleDown;
	float 				OffsetU;
	float 				OffsetV;

	FPoint				RelVectorU;
	FPoint				RelVectorV;

	/* -----------------8/11/99 3:09PM---------------------------------------------------------------------
	/* this is used by the splashes
	/* ----------------------------------------------------------------------------------------------------*/
	LPDIRECT3DTEXTURE2	AltD3DTexture;

	DWORD		 		BirthTick;
	FPointDouble 		Location;
	FPoint				VectorU;
	FPoint				VectorV;
	float				Size;
} Crater;

extern ScheduledExplosion ScheduledExplosions[],*LastScheduledExplosion;
extern ShockWave ShockWaves[],*LastShockWave;
extern Crater Craters[];
extern LPDIRECT3DTEXTURE2 CraterModels[];
extern int NumCraterModels;

void InitShockWaveStuff();
void DeleteShockWave(ShockWave *wave);
ShockWave *MakeShockWave(const FPointDouble &location,const DBWeaponType *weapon,BOOL second_water = FALSE);
void DisplayShockWaves(CameraInstance *camera);
void RenderShockWave(ShockWave *wave,FPoint &relative_position);
void DoExplosionOnGround(WeaponParams *pweapon,BOOL do_big_bang = 0,FPointDouble *position = NULL,DBWeaponType *pweapon_type=NULL,BOOL skip_graphic = FALSE);
void DoExplosionOnVehicles(WeaponParams *pweapon,BOOL do_big_bang,FPointDouble *position,DBWeaponType *pweapon_type, BOOL blew_something_up, int totalsecondaries, int grndside, int orgdirecthit);
void UpdateScheduledExplosions();
ScheduledExplosion *ScheduleExplosion(FPointDouble &location,MultipleExplosionType &exp, float delay_start,BOOL use_exp = TRUE,BasicInstance *instance = NULL,DamagePasser *pDamage = NULL,void *vehicle = NULL);
void InitGroundFX();
void FreeGroundFX();
void AddCrater(const FPointDouble &location,const int size,BOOL splash = FALSE);
void DisplayCraters(CameraInstance *camera);
DWORD GDGetUnRealHitValue(DWORD org_hit_value, BasicInstance *walker);
BOOL DamageAnInstance(BasicInstance *walker,DWORD hit_value,float fDistance,float fDamageValue,int &totalsecondaries,BOOL &blew_something_up);

BOOL IntersectBoundingBox(FMatrix &mat,FPointDouble &center,FPoint &High,FPoint &Low,FPointDouble &froms,FPointDouble &tos,FPointDouble *inter,FPoint *normal = NULL);

#endif _GNDDAM_H_