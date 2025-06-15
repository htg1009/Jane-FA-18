#ifndef __PARTICLE_H__
#define __PARTICLE_H__


#include "F18.h"

extern float seconds_per_frame;

#define MAX_CANISTERS  256
#define MAX_PARTICLES_PER_CAN  64
//#define MAX_PARTICLES  MAX_CANISTERS * MAX_PARTICLES_PER_CAN


typedef enum
{
	PT_NONE,					// 0
	PT_DAMAGE_SMOKE,			// 1
	PT_OIL_FIRE,				// 2
	PT_FLYING_PARTS,			// 3
	PT_SPIDER_SMOKE,			// 4
	PT_EXPLOSION_FIRE,			// 5
	PT_MISSLE_EXPLODE,			// 6

	PT_STEAM_SMOKE,				// 7
	PT_BURNOFF_SMOKE,			// 8
	PT_BLACK_SMOKE,				// 9
	PT_WELLHEAD_FIRE,			// 10

	PT_EXPLOSION_WATER,			// 11
	PT_EXPLOSION_DIRT,			// 12
	PT_EXPLOSION_GRAY,			// 13
	PT_EXPLOSION_BLACK,			// 14
	PT_FLYING_GROUND_PARTS,		// 15

	PT_FLASH,					// 16
	PT_DOME,					// 17
	PT_CAT_STEAM,               // 18

	PT_GREEN_MARKER_SMOKE,      // 19
	PT_RED_MARKER_SMOKE,        // 20
	PT_BLUE_MARKER_SMOKE,       // 21
	PT_YELLOW_MARKER_SMOKE,     // 22
	PT_ORANGE_MARKER_SMOKE,     // 23

	PT_MAX_PARTICLE_TYPES		// 24


} ParticleType;


//please reflect any changes to the similar defines at the top of family.cpp

typedef enum
{
	CT_NONE,					// 0
	CT_DAMAGE_SMOKE,			// 1
	CT_OIL_FIRE,				// 2
	CT_FLYING_PARTS,			// 3
	CT_SPIDER_SMOKE,			// 4
	CT_EXPLOSION_FIRE,			// 5
	CT_MISSLE_EXPLODE,			// 6

	CT_STEAM_SMOKE,				// 7
	CT_BURNOFF_SMOKE,			// 8
	CT_BLACK_SMOKE,				// 9
	CT_WELLHEAD_FIRE,			// 10

	CT_EXPLOSION_WATER,			// 11
	CT_EXPLOSION_DIRT,			// 12
	CT_EXPLOSION_GRAY,			// 13
	CT_EXPLOSION_BLACK,			// 14
	CT_FLYING_GROUND_PARTS,		// 15

	CT_FLASH,					// 16
	CT_EXPLOSION_DOME,			// 17
	CT_CAT_STEAM,				// 18

	CT_GREEN_MARKER_SMOKE,      // 19
	CT_RED_MARKER_SMOKE,        // 20
	CT_BLUE_MARKER_SMOKE,       // 21
	CT_YELLOW_MARKER_SMOKE,     // 22
	CT_ORANGE_MARKER_SMOKE,     // 23

	CT_MAX_CANISTER_TYPES		// 24
} CanisterType;


typedef struct _SavedCan
{
	CanisterType Type;
	FPointDouble RelPosition;
	float Scale;
	float TotalTimeLeft;
} SavedCan;

typedef struct _CanisterSaver
{
	SavedCan Cans[MAX_VEHICLE_SMOKE_SLOTS];
} CanisterSaver;

// Canister Flags
#define CF_NONE					0x0000000
#define CF_PHYSICS				0x0000001
#define CF_GROUND_HIT			0x0000002
#define CF_CANISTER_BOUNCES		0x0000004
#define CF_UNLIMTED_PARTICLES	0x0000008
#define CF_TOGGLE_TO_SMOKE		0x0000010
#define CF_TOGGLE_THIS_ONE		0x0000020
#define CF_FADE_LIGHT			0x0000100
#define CF_ATTACHED_TO_VEHICLE  0x0000200
#define CF_FLICKER_LIGHT		0x0000400
#define CF_CALLER_UNAWARE		0x0000800
#define CF_CALLER_VEHICLE		0x0001000
#define CF_CALLER_HANDLE		0x0002000
#define CF_DMG_CALLER_INSTANCE  0x0004000
#define CF_DMG_CALLER_VEHICLE 	0x0008000
#define CF_DELAY_START			0x0010000

//---- Particle Flags
#define PF_GRAVITY				0x0000001
#define PF_RANDOM_DIFFUSION		0x0000002
#define PF_ANGULAR_VELOCITY		0x0000004
#define PF_ALHPA_RATE			0x0000008
#define PF_SCALE_RATE			0x0000010
#define PF_FRAME_RATE			0x0000020
#define PF_DEST_BASED			0x0000040
#define PF_FIRE_ALPHA_RATE		0x0000080
#define PF_AT_LEAST_ONE			0x0000100
#define PF_CALC_LIGHTING		0x0000200
#define PF_SPRITE				0x0000400
#define PF_ATTACHED_TO_CANISTER 0x0000800


typedef struct _Particle
{
	ObjectSortHeader *SortHeader;
	SpriteType	*SType;
	FPointDouble Position;			// Current Position in WorldSpace
	FPoint Rotated;					// Rotated Position into ViewSpace
	int		Orientation;
	int 	iFrame;
	float   Alpha;			// Alpha += AlphaRate * time + StartAlpha				a=ar*t
	int		gop;					// gop modifier

	ParticleType Type;				// Sprite / Object

	//int	   Number;
	FPoint  Dest;
	float	DestTime;
	float   StepPercentage;			// walk jp curve


	int	   Flag;					// USes diffsustion

	FPoint Velocity;				// Velocity
	float  Time;					// Delta Time
	float  TotalTime;				// SECONDS Delat Time this paritcle lasts
	int	   RandomDiffusion;			// Index into Random_diffision array

	//Sprite Bassed

	float	Frame;
	float	StartFrame;
	float	FrameRate;

	float	Scale;			// Scale = ScaleRate * time	+ Startsize					s=sr*t
	float	StartScale;
	float	ScaleRate;		// ScaleRate = (EndSize  - StartSize ) / TotalTime		sr=ds/dt

	float   StartAlpha;
	float	AlphaRate;		// AplhaRate = (EndAlpha - StartAlpha) / TotalTime		ar=da/dt

	float	FireAlpha;
	float 	FireTimeToFirstAlpha;

	DWORD   PalNum;
	float   FlirHeat;
	float   Red;
	float   Green;
	float	Blue;

	int 	Priority;

	// Object Based
	//_ParticleObject *// Object *
	ObjectHandler *Model;
	FPoint  AngularVelocity;
	FPoint  AngularAcceleration;
	FMatrix	Attitude;
	ANGLE   Roll;
	ANGLE   Pitch;
	ANGLE   Heading;

	struct _Canister *pCanister;
} Particle;


typedef struct _Canister
{
	ObjectSortHeader *SortHeader;	// Might Sort Based On Canister: Issues Unsortheader.. Must keep This canister as long as its Particles are around else will mess up SortHeaderLists
	CanisterType   Type;			// Type..0 - Not Active 1+ Unknown
	ParticleType   TypeParticle;
	FPointDouble Position;			// Current Position
	FPoint		 RelPosition;
	FPoint Velocity;				// Current Velocity
	FPoint AngularVelocity;			// Angular Velocity of Canister

	FPoint Dest;					//  Destiont Point for farthest particl
	float  StepPercentage;			//  Used to move pts to dest point along Curve
	float  MaxDestDistance;			//  Max distance of Dest
	float  Scale;					//  Scale size

	float  DelayStart;				// time to wait before the action starts
	float  Time;					// Current Time. This is delta time from 0
	float  TotalTime;				// SECONDS Total time emmitor works

	int   Priority;
	int   CanPriority;

	int		Segments;				// used for Spheres

	unsigned int Flag;				// No MAxPartices, Collides with ground, Bounces Off Ground, Associated with and object, Has Physics Model,etc

	float EmmisionRate;				// Rate of Emmision for a new paritcle
	int	  NumParticles;			// Current Paritcle Emited
	int   MaxParticles;				// Max Paritcles
	float ParticlesToEmmit;			// Particles to Emmit   PE = EmmisRate*Time;
	int	  RandomDiffusion;			// Added Random Diffusion to Emitor;
	DWORD *Caller;
	DWORD PlanePtr;				    // Gut that dropped the weapon that caused this fire (only set if CF_DMG_CALLER_INSTANCE or CF_DMG_CALLER_VEHICLE is set)
	PointLightSource **Light;

	struct _Particle *pLastParticle;

	struct _Particle  Particles[MAX_PARTICLES_PER_CAN];

}Canister;




typedef struct _ParticleObject
{


}ParticleObject;

typedef struct _ParticleSprite
{


}ParticleSprite;


#define NUM_PLANE_PART_MODELS		3
#define MAX_PLANE_PARTS				32

typedef struct _PlanePartType
{
	ObjectHandler *Model;
}PlanePartType;





//-- System Functions
void InitParticleSystem( void);

// Canister Functions
void NewExplosionForcedWeapon(FPointDouble &position, float radius, int damage,FMatrix *matrix = NULL,BOOL cratering = FALSE,CanisterType ctype = CT_NONE);
//Canister *NewCanister( CanisterType type, FPoint position, FPoint velocity,  float StepPercentage, float maxdistance );
DWORD NewCanister( CanisterType type, FPointDouble &position, FPoint &velocity,  float size ,float life_time= 0.0f, FMatrix *matrix = NULL,float delay_start = 0.0f);
void NewCanisterExplosion( CanisterType type, FPointDouble &position, float size, float fire, FPoint *normal );
void MakeCatSteamCanister(FPointDouble &rel_position,void *ptr);


void DisplayCanisters( CameraInstance *camera);
void MoveCanisters( void);
void MoveCanister( Canister *pCanister);

void RemoveCanister( Canister *pCanister);
void SetCanisterType( Canister *pCanister, CanisterType type ,float life_time = 0.0f, FMatrix *matrix = NULL);
void SetCanisterPos( Canister *pCanister, FPointDouble &pos);
void InsertCanisters( CameraInstance *camera);


// Particle Functions
//Particle *NewParticle( Canister *pCanister);
Particle *NewParticle( Canister *pCanister);
void RemoveParticle( Particle *pParticle);
void MoveParticle( Particle *pParticle);

void SetParticleType( Canister *pCanister, Particle *pParticle );


void InsertParticles(CameraInstance *camera );
void InsertParticle(CameraInstance *camera, Particle *pParticle );

void DrawParticle( Particle *pParticle);



// Physices Functions
BOOL GroundCollision( float x, float y, float z);
void SetCanisterPos( Canister *pCanister, FPointDouble &position);
void ApplayTimeToCanister( Canister *pCanister );
void AttenuateVelocity( FPoint *pVel, float attenuation);
void AttenuateVelocityNoG( FPoint *pVel, float att);



//Interface
void UpdateCanisterPos( DWORD hCanister, FPointDouble &position);
void FreeCanisterHandle( DWORD hCanister );
DWORD GetFireHandle(FPointDouble &position,DWORD *ptr,CanisterType fire_type, float fire_size, float fire_duration);
DWORD GetWellHeadSmokeHandle( FPointDouble &position,DWORD *ptr);
DWORD GetBurnoffSmokeHandle( FPointDouble &position,DWORD *ptr);
DWORD GetSteamSmokeHandle( FPointDouble &position,DWORD *ptr);
void RemoveFireHandleCaller(DWORD handle);

//void NewExplosionWeapon( DBWeaponType *pweapon_type, FPointDouble &position );
void NewExplosionWeapon( DBWeaponType *pweapon_type, FPointDouble &position, CanisterType ctype=CT_NONE,  int iWarHead=0 );

void NewExplosionUser( CanisterType  ctype,  FPointDouble &position, float radius, float hscale, int numspines );

#define SUN_SUN 	0
#define SUN_MOON 	1
#define SUN_CORONA 	2

//Misc Effects
void RenderSun(int which);
void RenderSky(void);
void RenderSunLensFlare(void);
void RenderCameraFlare( RotPoint3D *rotpt ,FPoint &color,float alpha = 0.0,float size = 0.0);
void RenderCounterMeasureFlare(RotPoint3D *rotpt ,FPoint &color);
void SaveVehicleSmoke(MovingVehicleParams *V,CanisterSaver &can);
void RestoreVehicleSmoke(MovingVehicleParams *V,CanisterSaver &can);
void DamageAttachedVehicle(Canister *pCan);
void DamageAttachedInstance(Canister *pCan);
void SetInstanceBurning(FPointDouble &position,void *ptr,DWORD fire_type, float fire_size, float fire_duration,void *pptr = NULL,float delay_start = 0.0f);
DWORD GetVehicleFireHandle(FPointDouble &position,void *ptr,CanisterType fire_type, float fire_size, float fire_duration,void *plane = NULL,float delay_start = 0.0f);


#endif