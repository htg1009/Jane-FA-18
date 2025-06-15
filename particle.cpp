//  Tom Whittaker 1/26
//
//		* Needs Attention
//		+ Added
//		- Removed
//		? Question
//		! Idea
//
//	Need
//		LOD ideas in terms of Rendering(dots, lines, less sprites) and Not render on detail setting
//		Types of Predefined Effects
//		Util to Generate Effects by manual tweaking vaules
//		Pass Veloectiy to Particles  From Canister.. Ie Initials Flying parts int he direction we want them to go
//
//	- Canister -- Emiting Device
//		-  pos , velocity, time, ect
//		-  emits particles for its duration/number of particles/emission rate
//		-  Emmit Multi-Particles per frame with EmmisionRate or Create Several Canisters
//		-  No Object* ... add hCanister handle to objects, since we don't have a generic object type to connect to
//		-  Objects should update canister positions
//		- Use Johns subobject code to attach sprites to a Objet Point
//
//
//	- Paritcle -- Just like a canister except no emitter
//				- This is what gets Rendered
//				- Particle can be a Sprites or Objects or both..
//
//			age
//			Bitmap		have a bitmap associated with them
//			Object		have a 3dobject associated with object
//			duration	is idependant of emitor
//			ParentId	Not Available :  Once emitted Paritcle is idenpendant of its Parent Canistor  ?Canister can terminato allow back association...Ie use Sortheader for Emitor
//			Size		Grow linear with time... Get larger smaller..could make this a function ptr later for non linear
//			Alpha		For Disapperaing/Apperaing effects
//
//	= Code-- Like to keep list on top off static arrays for faster walking
//
//	Examples
///			Burning Smoke
//			Damage Smoke
//			Spider Burst
//			Metal Sharpnels-- Plane Parts flying off
//			Water Splasheds
//			Part Flying off WITH another Canister for burning trail
//
//
//  Functions/Implentations	like code above
//			Initialize   - One time init
//			NewCanisters - add canister to world
//			MoveCanisters - update canisters phyiscs and emit particles if necessary
//			MoveParticles - move particles
//			InsertParticles - Insert particles into world sorter if visible
//			RenderParticels - Render Visble particle objects ( sprites/objects)
//

//			Util Functions
//				MoveCanisters - update a canisters posiont.. ussaly a attached canister to a object
//				StopCanisteers - turn off canister ... blow up object etc
//				AccelarteCanister - start canister already in prgross for some time.. Smoke stacks are already burning
//
//
//	Notes:
//		Particle/Canister can be removed:
//			- implictedly with time/duration
//			- expliced with util function
//			- collision with ground...
//					- Need to add a flag if we want to leave it there
//					- Change Veclocity for Bounce
//			- Program End

#include "f18.h"
#include "particle.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "clouds.h"



void LogDestroyedObject(WeaponParams *W,int iType,int iID);

BOOL DiscreteLights;
BOOL NoFlashDuringExplosion;
extern BOOL PushBackFog;
extern FPoint MissileFlameLightColor;
void Render2DSpriteSX( Sprite *spt, float r, float g, float b, float a, float scale, int sx, int sy);
void RenderSprite( Sprite *spt, float r, float g, float b, float a, float scale,DWORD unwanted_flags = 0,DWORD addnl_poly_flags = 0,BOOL rotate_flare = FALSE,float rangle = 0.0f);
void InitDiffusionTable( void );
void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition);

#ifdef COLOR_SOURCE_DIRT
#undef COLOR_SOURCE_DIRT
#define COLOR_SOURCE_DIRT  0x91
#endif

FPoint random_diffusion[3][32];

FPoint wind_vector;


int NumCanisters;
int NumParticles;

Canister Canisters[MAX_CANISTERS];
PlanePartType PlanePartTypes[NUM_PLANE_PART_MODELS];
extern FPoint PositionToDrawSun;
extern FPoint PositionToDrawMoon;

Canister *pLastCanister;

int DebugKey = 0;
void SetTomDebugToggle( void )
{
	static set=0;
	static FPointDouble Position;
	FPoint tmp;
	DebugKey = ~DebugKey;

	if( !set)
	{
		Position = Camera1.CameraLocation;
		Position.Y = LandHeight( Position.X, Position.Z);
		set = 1;
	}
	tmp.SetValues( 0.0f);
	NewCanister( CT_EXPLOSION_DOME, Position, tmp, 20 METERS );

/*
	DBWeaponType *pweapon_type = NULL;

	int i = iNumWeaponList;
	while( i--)
	{
		pweapon_type = &pDBWeaponList[i];
		if( pweapon_type->iWarHead ==WARHEAD_STANDARD_MEDIUM )
			break;
		if(pweapon_type->iWarHead ==WARHEAD_NUKE )
			break;
		if(pweapon_type->iWarHead ==WARHEAD_SAM_LARGE )
			break;
		if( pweapon_type->iWarHead ==WARHEAD_AAA_LARGE)
			break;

	}
	if( pweapon_type)
		MakeShockWave( Position, pweapon_type, FALSE);


*/

}



//--------------------------------------------------------------------------------------
int CanisterPriorities;
void InitParticleSystem( void)
{
	NumCanisters = 0;
	NumParticles = 0;
	CanisterPriorities = 0;

	pLastCanister = NULL;//&Canisters[-1];
	ZeroMemory(	&Canisters[0], sizeof(Canister)  * MAX_CANISTERS);

	InitDiffusionTable();

	//wind_vector.SetValues((float)((2.0f METERS) *frand()+ (2.0f METERS)), 0.0f, (float) ((1.0f METERS) * frand() + (1.0f METERS)) );
	wind_vector.SetValues((float)((4.0f METERS) *frand()), 0.0f, (float) ((2.0f METERS) * frand() ) );




}

float heightscale;

void NewExplosionForcedWeapon(FPointDouble &position, float radius, int damage,FMatrix *matrix,BOOL cratering,CanisterType ctype )
{
	int numspines;
	int i;
	CanisterType cantype;
	FMatrix lmatrix;
	FPoint vel(0.0f);

	if (InHighResTerrainRegion(position))
	{
		cantype = ctype;

		if (!matrix)
		{
			GetMatrix(position,0,lmatrix);
			matrix = &lmatrix;
		}

		// Get Distances for For Explosin

		// Get Position
		position.Y += 3 METERS;				// move explosion up a little

		// Get Number of Spines and Height Scale for Explosion
		if (damage > 50)
			numspines = 8+damage/100.0f;
		else
			numspines = 1;


		if( InWater(position.X, position.Z) )
		{
			if (numspines == 1)
			{
				heightscale = 50.0f;
				radius *=  4.0f;
			}
			else
			{
				if (_3dxl == _3DXL_SOFTWARE)
					numspines >>= 1;
				heightscale = 1.5f;
				if (cratering)
				   radius *= 4.0f;
			}

			heightscale = 1.5f;
			if( cantype == CT_NONE)
				cantype = CT_EXPLOSION_WATER;
			for( int i = 0; i< numspines; i++)
				NewCanister( cantype , position, vel, radius);

			if ((cratering) || (numspines == 1))
				radius = (float)damage*4.0f/100.0f;
			else
				radius = (float)damage/100.0f;

		}
		else
		{
			if( cratering )
			{

				numspines *= 2.0/3.0;// 2/3 the num spines
				heightscale = 3.5f;
				for( i = 0; i< numspines/2; i++)
					NewCanister( CT_EXPLOSION_DIRT , position, vel, radius, 0.0f, matrix);
				radius /= 2.0f;
				for( i = 0; i< numspines/2; i++)
					NewCanister( CT_EXPLOSION_BLACK , position, vel, radius, 0.0f, matrix);
				radius = (float)damage*4.0f/100.0f;
			}
			else
			{
				if (numspines > 1.0)
				{
					if (_3dxl == _3DXL_SOFTWARE)
						numspines >>= 1;
					heightscale = 0.3f;
				}
				else
					heightscale = 20.0f;

				if( cantype == CT_NONE)
					cantype = CT_EXPLOSION_FIRE;
				for( int i = 0; i< numspines; i++)
					NewCanister( cantype , position, vel, radius, 0.0f, matrix);

				radius = (float)damage/100.0f;
			}
		}

		if (!NoFlashDuringExplosion)
			NewCanister(CT_FLASH,position,vel,radius);
	}

}

void NewExplosionWeapon( DBWeaponType *pweapon_type, FPointDouble &position, CanisterType ctype,  int iWarHead )
{
	int numspines;
	float radius;
	int i;
	int warhead;
	CanisterType cantype;
	FMatrix matrix;
	FPoint vel(0.0f);
	BOOL cratering;

	if (InHighResTerrainRegion(position))
	{
		if( iWarHead)
			warhead = iWarHead;
		else
			warhead = pweapon_type->iWarHead;
		cantype = ctype;

		GetMatrix(position,0,matrix);

		// Get Distances for For Explosin
		radius = pweapon_type->iDamageRadius;// *0.333;

		// Get Position
		position.Y += 1.5 METERS;				// move explosion up a little

		// Get Number of Spines and Height Scale for Explosion
		if (pweapon_type->iDamageValue > 50)
			numspines = 8+pweapon_type->iDamageValue/100.0f;
		else
			numspines = 1;

		cratering = (BOOL)((warhead == WARHEAD_CRATERING) || (warhead == WARHEAD_PENETRATION));

		if( InWater(position.X, position.Z) )
		{
			if (numspines == 1)
			{
				heightscale = 50.0f;
				radius *=  4.0f;
			}
			else
			{
				if (_3dxl == _3DXL_SOFTWARE)
					numspines >>= 1;

				heightscale = 1.5f;
				if (cratering)
				   radius *= 4.0f;
			}

			if( cantype == CT_NONE)
				cantype = CT_EXPLOSION_WATER;


			//if( pweapon_type->iWeaponType == WEAPON_TYPE_GUN )
			//	cantype = CT_NEW_BULLET_EFFECT;



			for( int i = 0; i< numspines; i++)
				NewCanister( cantype , position, vel, radius);

			if ((cratering) || (numspines == 1))
				radius = (float)pweapon_type->iDamageValue*4.0f/100.0f;
			else
				radius = (float)pweapon_type->iDamageValue/100.0f;
		}
		else
		{
			if( cratering )
			{
				numspines *= 2.0/3.0;// 2/3 the num spines
				heightscale = 4.5f;
				radius *= 4.0;	        // four times
				for( i = 0; i< numspines/2; i++)
					NewCanister( CT_EXPLOSION_DIRT , position, vel, radius,0.0f,&matrix);
				radius *= 2.0;			// eight times
				for( i = 0; i< numspines/2; i++)
					NewCanister( CT_EXPLOSION_BLACK , position, vel, radius,0.0f,&matrix);
				radius = (float)pweapon_type->iDamageValue*4.0f/100.0f;
			}
			else
			{
				if (numspines > 1.0)
				{
					if (_3dxl == _3DXL_SOFTWARE)
						numspines >>= 1;
					heightscale = 0.3f;
				}
				else
					heightscale = 20.0f;

				if( cantype == CT_NONE)
					cantype = CT_EXPLOSION_BLACK;

				for( int i = 0; i< numspines; i++)
					NewCanister( cantype , position, vel, radius,0.0f,&matrix);

				if (numspines == 1)
					radius = (float)pweapon_type->iDamageValue*4.0f/100.0f;
				else
					radius = (float)pweapon_type->iDamageValue/100.0f;
			}
		}
	//	if ((pweapon_type->iWeaponType != WEAPON_TYPE_GUN)  || (warhead == WARHEAD_ICENDIARY))
		if (!NoFlashDuringExplosion)
			NewCanister(CT_FLASH,position,vel,radius);
	}

}



void NewExplosionUser( CanisterType  ctype,  FPointDouble &position, float radius, float hscale, int numspines )
{
	FPoint vel(0.0f);

	heightscale = hscale;
	for( int i=0; i<numspines; i++)
		NewCanister( ctype, position, vel, radius);

}


//--------------------------------------------------------------------------------------
void NewCanisterExplosion( CanisterType type, FPointDouble &position, float size, float fire, FPoint *normal )
{
	FPoint vel(0.0f);

	for( int i = 0; i< 12; i++)
		NewCanister( type, position, vel, size);
}

PointLightSource **FindNearbyLight(Canister *us,FPointDouble &position,float radius)
{
	Canister *pCan;
	PointLightSource **ret;
	float dist;

	if (!DiscreteLights)
	{
		radius *= 0.5f;

		pCan = Canisters;
		while(pCan <= pLastCanister)
		{
			if((pCan->Type) && (pCan->Type == us->Type) && (pCan != us) && (pCan->Light) && !(pCan->Flag & CF_ATTACHED_TO_VEHICLE))
			{
				dist = (*pCan->Light)->WorldPosition - position;
				if ((dist < (*pCan->Light)->Radius1*0.5f) || (dist < radius))
				{
					pCan->Flag &= ~(CF_FADE_LIGHT | CF_FLICKER_LIGHT);
					ret = pCan->Light;
					pCan->Light = NULL;
					return ret;
				}
			}

			pCan++;
		}
	}

	return NULL;
}


DWORD NewCanister( CanisterType type, FPointDouble &position, FPoint &velocity,  float radius ,float life_time,FMatrix *matrix,float delay_start)
{
	Canister *pCanister;

	// Initialize New Canister. Particles won't get emmited until MoveCanister()
	pCanister = &Canisters[0];

	while((pCanister <= pLastCanister) && (pCanister->Type))
		pCanister++;

	if (pCanister < &Canisters[MAX_CANISTERS])
	{
	  	pCanister->NumParticles		= 0;

		pCanister->Type				= type;
		pCanister->Position			= position;
		pCanister->Velocity			= velocity;
		pCanister->Time				= 0.0f;
		pCanister->ParticlesToEmmit = 0.0f;
		pCanister->Caller 			= NULL;
		pCanister->PlanePtr			= NULL;
		pCanister->Priority			= 5;
		pCanister->CanPriority		= CanisterPriorities++ & 0x7;
		pCanister->Light  			= NULL;
		pCanister->DelayStart		= 0.0f;

		if (radius != 0.0)
			pCanister->Scale		= radius;
		else
			pCanister->Scale 		= 1.0f;

		SetCanisterType( pCanister, type, life_time, matrix );							// Fill in Canister Type

		if (!(pCanister->Flag & CF_DELAY_START))
		{
			pCanister->DelayStart		= delay_start;
			if (delay_start)
				pCanister->Flag |= CF_DELAY_START;
		}

		if(pCanister > pLastCanister)
			pLastCanister = pCanister;

		pCanister->pLastParticle = &(pCanister->Particles[-1]);

		ZeroMemory(&pCanister->Particles[0],sizeof(Particle)*MAX_PARTICLES_PER_CAN);

		NumCanisters++;

	}
	else
		pCanister = NULL;

	return (DWORD)pCanister;
}


DWORD OLDNewCanister( CanisterType type, FPointDouble &position, FPoint &velocity,  float size )
{
	int i;
	Canister *pCanister;

	// Initialize New Canister. Particles won't get emmited until MoveCanister()
	pCanister = &Canisters[0];
	for( i=0; i<MAX_CANISTERS; i++)
	{
		if( !pCanister->Type )
		{
			pCanister->Type				= type;
			pCanister->Position			= position;
			pCanister->Velocity			= velocity;
			pCanister->Time				= 0.0f;
			pCanister->NumParticles		= 0;
			pCanister->ParticlesToEmmit = 0.0f;

			// Just for Explosion
			pCanister->MaxDestDistance	= 50 METERS + size * 150 METERS;//WeaponDB
			pCanister->Scale			= pCanister->MaxDestDistance /(100 METERS);  //0.5+ frand() * 1.5f;//
			pCanister->StepPercentage	= 0.005f / pCanister->Scale;// + frand()/200.0f -  size*0.003f;

			SetCanisterType( pCanister, type );							// Fill in Canister Type
			NumCanisters++;
			return (i+1);		// handles offset +1  so we can return 0;
		}
		pCanister++;
	}
	return 0;
}

void SetCanisterType( Canister *pCanister, CanisterType type , float life_time, FMatrix *matrix)
{
	PointLightSource *light;
	FPointDouble light_location;
	float light_radius;

	if( type >= CT_MAX_CANISTER_TYPES )
		return;

	pCanister->TotalTime = life_time; //Seconds

	switch( type)
	{

		case CT_BURNOFF_SMOKE:
			pCanister->Flag				= CF_UNLIMTED_PARTICLES;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_BURNOFF_SMOKE;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= 7.0 + g_Settings.gr.nGroundSmoke*4.0;//1.4;//3.5;			//1.4  Low   // 3.5 HIGH detail
			pCanister->RandomDiffusion	= 8;
  	  		if (pCanister->TotalTime == 0.0f)
				pCanister->TotalTime	= 3*60*60; //Seconds
			break;

		case CT_BLACK_SMOKE:
			pCanister->Flag				= CF_NONE;//CF_UNLIMTED_PARTICLES;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_BLACK_SMOKE;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= (1.5 + frand()*1.5*g_Settings.gr.nGroundSmoke)*2.0;
			pCanister->RandomDiffusion	= 2;//index into random_diffusion[]
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 40; //Seconds
			break;

		case CT_WELLHEAD_FIRE:
			pCanister->Flag				= CF_UNLIMTED_PARTICLES | CF_TOGGLE_TO_SMOKE;//CF_UNLIMTED_PARTICLES;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_WELLHEAD_FIRE;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= (3.0 + frand()*3.0*g_Settings.gr.nGroundSmoke)*2.0;//3.5;			//1.4  Low   // 3.5 HIGH detail
			pCanister->RandomDiffusion	= 2;
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 3*60*60; //Seconds

			light_radius = SpriteTypes[SPT_FLASH].RWWidth*pCanister->Scale*5.0f;
			light_location = pCanister->Position;
			light_location.Y += 0.5f FEET;
			if (pCanister->Light = FindNearbyLight(pCanister,light_location,light_radius))
			{
				light = *pCanister->Light;
				light->WorldPosition *= light->Radius1;
				light->WorldPosition.AddScaledVector(light_radius,light_location);
				light->WorldPosition *= 1.0f/(light->Radius1 + light_radius);

				light->Radius1 += light_radius;
				light->ooRadius1 = 1.0f/light->Radius1;
				light->Radius2 = light->Radius1 * 0.2f;
				light->ooRadius2 = 1.0f/light->Radius2;
			}
			else
				if (pCanister->Light = (PointLightSource **)GetNewLight())
				{
					PointLightSource *new_light = new PointLightSource;
					new_light->WorldPosition = light_location;
					new_light->Color.SetValues(1.0,0.75,0.21);
					new_light->Flags |= LIGHT_LARGE_SPOT;
					new_light->Radius1 = light_radius;
					new_light->ooRadius1 = 1.0f/new_light->Radius1;
					new_light->Radius2 = new_light->Radius1 * 0.2f;
					new_light->ooRadius2 = 1.0f/new_light->Radius2;
					*(pCanister->Light) = new_light;
				}

			break;

		case CT_STEAM_SMOKE:
			pCanister->Flag				= CF_UNLIMTED_PARTICLES;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_STEAM_SMOKE;
			pCanister->MaxParticles		= 16;//32;//64;
			pCanister->EmmisionRate		= 1.5 + frand()*1.5*g_Settings.gr.nGroundSmoke;
			pCanister->RandomDiffusion	= 2;//index into random_diffusion[]
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 3*60*60; //Seconds
			break;

		case CT_DAMAGE_SMOKE:
			pCanister->Flag				= CF_NONE;//CF_UNLIMTED_PARTICLES;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_DAMAGE_SMOKE;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= 1.5 + frand()*1.5*g_Settings.gr.nGroundSmoke;
			pCanister->RandomDiffusion	= 2;//index into random_diffusion[]
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 30; //Seconds
			break;

		case CT_OIL_FIRE:
			pCanister->Flag				= CF_TOGGLE_TO_SMOKE | CF_FLICKER_LIGHT;//CF_UNLIMTED_PARTICLES;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_OIL_FIRE;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= (1.5 + frand()*1.5*g_Settings.gr.nGroundSmoke)*2.0;
			pCanister->RandomDiffusion	= 8;
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 60; //Seconds

			light_radius = SpriteTypes[SPT_FLASH].RWWidth*pCanister->Scale*5.0f;
			light_location = pCanister->Position;
			light_location.Y += 0.5f FEET;
			if (pCanister->Light = FindNearbyLight(pCanister,light_location,light_radius))
			{
				light = *pCanister->Light;
				light->WorldPosition *= light->Radius1;
				light->WorldPosition.AddScaledVector(light_radius,light_location);
				light->WorldPosition *= 1.0f/(light->Radius1 + light_radius);

				light->Radius1 += light_radius;
				light->ooRadius1 = 1.0f/light->Radius1;
				light->Radius2 = light->Radius1 * 0.2f;
				light->ooRadius2 = 1.0f/light->Radius2;
			}
			else
				if (pCanister->Light = (PointLightSource **)GetNewLight())
				{
					PointLightSource *new_light = new PointLightSource;
					new_light->WorldPosition = light_location;
					new_light->Color.SetValues(1.0,0.75,0.21);
					new_light->Flags |= LIGHT_LARGE_SPOT;
					new_light->Radius1 = light_radius;
					new_light->ooRadius1 = 1.0f/new_light->Radius1;
					new_light->Radius2 = new_light->Radius1 * 0.2f;
					new_light->ooRadius2 = 1.0f/new_light->Radius2;
					*(pCanister->Light) = new_light;
				}

			break;


		case CT_SPIDER_SMOKE:
			pCanister->Flag				= CF_UNLIMTED_PARTICLES;
			pCanister->TypeParticle		= PT_SPIDER_SMOKE;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= 12.5;
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 4; //Seconds
			//PVel.X						= (vel - (rand() % vel)) << 1;
			//PVel.Y						= (15 METERS  + (rand() % vel) );
			//PVel.Z						= (vel - (rand() % vel)) << 1;
			break;

		case CT_EXPLOSION_GRAY:
		case CT_EXPLOSION_BLACK:
		case CT_EXPLOSION_DIRT:
				// Make these guys a little smaller.
				pCanister->Scale *= 0.5;
		case CT_EXPLOSION_FIRE :
			pCanister->Dest.SetValues( (frand() - 0.5f) * 2.0f, frand(), (frand() - 0.5f) * 2.0f);			//100 METERS;

			if (pCanister->Type != CT_EXPLOSION_FIRE)
				pCanister->Dest.Y			+= heightscale;
			else
				pCanister->Dest.Y			+= 0.8f;

			pCanister->Dest.Normalize();

			pCanister->Position.X 		+= (double)(pCanister->Dest.X * pCanister->Scale * 0.03f);
			pCanister->Position.Z 		+= (double)(pCanister->Dest.Z * pCanister->Scale * 0.03f);

			pCanister->MaxDestDistance	= pCanister->Scale *0.233;	//50 METERS + size * 150 METERS;//WeaponDB
			pCanister->Scale			= 0.01 + pCanister->Scale/300.0f;///0.5+ frand() * 1.5f;
			if (pCanister->Scale >= 1.0f)
				pCanister->StepPercentage	= 0.05f / pCanister->Scale;// + frand()/200.0f -  size*0.003f;
			else
				pCanister->StepPercentage	= 0.05f;// + frand()/200.0f -  size*0.003f;

			pCanister->Flag				= CF_NONE;
			pCanister->TypeParticle		= (ParticleType)pCanister->Type;
			pCanister->MaxParticles		= 2 + g_Settings.gr.nExplosionDetail * 3;
			pCanister->EmmisionRate		= 10000;//Emitt Max Particles at Once
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 1; //Seconds

			pCanister->Dest  			*= pCanister->MaxDestDistance * ( 0.5+0.5*frand());


			break;


		case CT_EXPLOSION_WATER:
			pCanister->Dest.SetValues( (frand() - 0.5f) * 2.0f, frand(), (frand() - 0.5f) * 2.0f);			//100 METERS;
			pCanister->Dest.Y			+= heightscale;//1.5f;//5.0f;
			pCanister->Dest.Normalize();

			pCanister->Position.X 		+= (double)(pCanister->Dest.X * pCanister->Scale * 0.05f);
			pCanister->Position.Z 		+= (double)(pCanister->Dest.Z * pCanister->Scale * 0.05f);

			pCanister->MaxDestDistance	= pCanister->Scale *0.233;	//50 METERS + size * 150 METERS;//WeaponDB
			pCanister->Scale			= 0.01 + pCanister->Scale/300.0f;///0.5+ frand() * 1.5f;
			if (pCanister->Scale >= 1.0f)
				pCanister->StepPercentage	= 0.03f / pCanister->Scale;// + frand()/200.0f -  size*0.003f;
			else
				pCanister->StepPercentage	= 0.03f;// + frand()/200.0f -  size*0.003f;

			pCanister->Flag				= CF_NONE;
			pCanister->TypeParticle		= PT_EXPLOSION_WATER;
			pCanister->MaxParticles		= 2 + g_Settings.gr.nExplosionDetail * 3;
			pCanister->EmmisionRate		= 10000;//Emitt Max Particles at Once
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 1; //Seconds
			pCanister->Dest  			*= (pCanister->MaxDestDistance*1.10f);
			break;


		case CT_MISSLE_EXPLODE:
			pCanister->Flag				= CF_NONE | CF_FADE_LIGHT;
			pCanister->TypeParticle		= PT_MISSLE_EXPLODE;
			pCanister->MaxParticles		= 1;
			pCanister->EmmisionRate		= 30.0;
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 5.01; //Seconds

			light_radius = SpriteTypes[SPT_FLASH].RWWidth*pCanister->Scale;
			light_location = pCanister->Position;
			light_location.Y += 0.5f FEET;
			if (pCanister->Light = FindNearbyLight(pCanister,light_location,light_radius))
			{
				light = *pCanister->Light;
				light->WorldPosition *= light->Radius1;
				light->WorldPosition.AddScaledVector(light_radius,light_location);
				light->WorldPosition *= 1.0f/(light->Radius1 + light_radius);

				light->Radius1 += light_radius;
				light->ooRadius1 = 1.0f/light->Radius1;
				light->Radius2 = light->Radius1 * 0.26667f;
				light->ooRadius2 = 1.0f/light->Radius2;
			}
			else
				if (pCanister->Light = (PointLightSource **)GetNewLight())
				{
					PointLightSource *new_light = new PointLightSource;
					new_light->WorldPosition = light_location;
					new_light->Color.SetValues(0.8,0.7,0.6);
					new_light->Flags |= LIGHT_SMALL_SPOT;
					new_light->Radius1 = light_radius;
					new_light->ooRadius1 = 1.0f/new_light->Radius1;
					new_light->Radius2 = new_light->Radius1 * 0.26667f;
					new_light->ooRadius2 = 1.0f/new_light->Radius2;
					*(pCanister->Light) = new_light;
				}

			break;

		case CT_FLYING_PARTS:
			pCanister->Flag				= CF_NONE;
			pCanister->TypeParticle		= PT_FLYING_PARTS;
			pCanister->MaxParticles		= 1 + g_Settings.gr.nExplosionDetail;
			pCanister->EmmisionRate		= 10000;
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 1; //Seconds
			break;

		case CT_EXPLOSION_DOME:
			pCanister->Flag				= CF_NONE;
			pCanister->TypeParticle		= PT_DOME;
			pCanister->MaxParticles		= 10;
			pCanister->EmmisionRate		= 10000;
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 1; //Seconds
			pCanister->Segments			= 9;
			break;

		case CT_FLYING_GROUND_PARTS :
			pCanister->Flag				= CF_NONE;
			pCanister->TypeParticle		= PT_FLYING_GROUND_PARTS;
			pCanister->MaxParticles		= 1 + g_Settings.gr.nExplosionDetail;
			pCanister->EmmisionRate		= 10000;
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 1; //Seconds
			break;

		case CT_FLASH:
			pCanister->Flag				= CF_NONE | CF_FADE_LIGHT;
			pCanister->TypeParticle		= PT_FLASH;
			pCanister->MaxParticles		= 1;
			pCanister->EmmisionRate		= 10000;
			pCanister->RandomDiffusion	= 0;
			pCanister->TotalTime		= 0.76; //Seconds

			light_radius = SpriteTypes[SPT_FLASH].RWWidth*pCanister->Scale*5.0f;
			light_location = pCanister->Position;
			light_location.Y += 0.5f FEET;
			if (pCanister->Light = FindNearbyLight(pCanister,light_location,light_radius))
			{
				light = *pCanister->Light;
				light->WorldPosition *= light->Radius1;
				light->WorldPosition.AddScaledVector(light_radius,light_location);
				light->WorldPosition *= 1.0f/(light->Radius1 + light_radius);

				light->Radius1 += light_radius;
				light->ooRadius1 = 1.0f/light->Radius1;
				light->Radius2 = light->Radius1 * 0.2f;
				light->ooRadius2 = 1.0f/light->Radius2;
			}
			else
				if (pCanister->Light = (PointLightSource **)GetNewLight())
				{
					PointLightSource *new_light = new PointLightSource;
					new_light->WorldPosition = light_location;
					new_light->Color.SetValues(1.0,1.0,1.0);
					new_light->Flags |= LIGHT_LARGE_SPOT;
					new_light->Radius1 = light_radius;
					new_light->ooRadius1 = 1.0f/new_light->Radius1;
					new_light->Radius2 = new_light->Radius1 * 0.2f;
					new_light->ooRadius2 = 1.0f/new_light->Radius2;
					*(pCanister->Light) = new_light;
				}

			pCanister->Scale *= 0.5;
			break;

		case CT_CAT_STEAM:
			pCanister->Flag				= CF_ATTACHED_TO_VEHICLE | CF_CALLER_UNAWARE;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= PT_CAT_STEAM;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= 3.0f+ frand()*3.0*g_Settings.gr.nGroundSmoke;
			pCanister->RandomDiffusion	= 2;//index into random_diffusion[]
			pCanister->RelPosition = pCanister->Position;
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 1.0f; //Seconds
			break;

		case CT_GREEN_MARKER_SMOKE:
		case CT_RED_MARKER_SMOKE:
		case CT_BLUE_MARKER_SMOKE:
		case CT_YELLOW_MARKER_SMOKE:
		case CT_ORANGE_MARKER_SMOKE:
			pCanister->Flag				= CF_NONE;
			pCanister->MaxParticles		= 0;
			pCanister->TypeParticle		= (ParticleType)type;
			pCanister->MaxParticles		= 64;
			pCanister->EmmisionRate		= 1.5 + frand()*1.5*g_Settings.gr.nGroundSmoke;
			pCanister->RandomDiffusion	= 2;//index into random_diffusion[]
			if (pCanister->TotalTime == 0.0f)
			   pCanister->TotalTime		= 40; //Seconds
			break;

	}

	if (matrix)	                        // this is only going to be true for some explosions.
		pCanister->Dest *= *matrix;


}


void MoveCanisters( void)
{
	Canister *pCanister;

	pCanister = &Canisters[0];
	while( pCanister <= pLastCanister )
	{
		// Processs valid canisters
		if( pCanister->Type )
			MoveCanister( pCanister);
		pCanister++;
	}

}


void MoveCanister( Canister *pCanister)
{
	Particle *pParticle;
	FPoint distance;
	float old_seconds_per_frame;
	float timer;
	float old_parts;


	pCanister->Time += seconds_per_frame;

	if (pCanister->Flag & CF_DELAY_START)
	{
		if (pCanister->Time >= pCanister->DelayStart)
		{
			pCanister->Time -= pCanister->DelayStart;
			pCanister->DelayStart = 0.0f;
			pCanister->Flag &= ~CF_DELAY_START;
		}
		else
			return;
	}

	if( !(pCanister->Flag & CF_UNLIMTED_PARTICLES) && ((pCanister->Time > pCanister->TotalTime)  || (pCanister->EmmisionRate==0.0f)) )
	{
		if( (!pCanister->NumParticles) || (pCanister->pLastParticle < &pCanister->Particles[0]))
		{
			RemoveCanister( pCanister);
			return;
		}
		else
			if (pCanister->Flag & CF_ATTACHED_TO_VEHICLE)
			{
				MovingVehicleParams *v = (MovingVehicleParams *)pCanister->Caller;
				pCanister->Position = pCanister->RelPosition;
				pCanister->Position *= v->Attitude;
				pCanister->Position += v->WorldPosition;

				if (pCanister->Light)
					(*(pCanister->Light))->WorldPosition = pCanister->Position;
			}

		if (pCanister->Light)
		{
			if (pCanister->Flag & CF_FADE_LIGHT)
				(*(pCanister->Light))->Intensity = 0.0f;
			else
			{
				float mult= 1.0f - (pCanister->Time - pCanister->TotalTime);
				if (mult <= 0.0f)
					(*(pCanister->Light))->Intensity = 0.0f;
				else
				{
					if (mult > 1.0f)
						mult = 1.0f;

					if (pCanister->Flag & CF_FLICKER_LIGHT)
						(*(pCanister->Light))->Intensity = (1.0f - 0.4f + 0.3f*(1.0f - fmod(pCanister->ParticlesToEmmit,1.0f)) + 0.3f*frand())*mult;
					else
						(*(pCanister->Light))->Intensity = mult;
				}
			}
		}
	}
	else
	{
		if (pCanister->Light)
		{
			if (pCanister->Flag & CF_FADE_LIGHT)
				(*(pCanister->Light))->Intensity = 1.0f- ((pCanister->Time / pCanister->TotalTime));

			if (pCanister->Flag & CF_FLICKER_LIGHT)
				(*(pCanister->Light))->Intensity = 0.4f + 0.3f*(1.0f - fmod(pCanister->ParticlesToEmmit,1.0f)) + 0.3f*frand();
		}

		if (pCanister->Flag & CF_DMG_CALLER_INSTANCE)
			DamageAttachedInstance(pCanister);

		if (pCanister->Flag & CF_DMG_CALLER_VEHICLE)
			DamageAttachedVehicle(pCanister);

		if (pCanister->Flag & CF_ATTACHED_TO_VEHICLE)
		{
			MovingVehicleParams *v = (MovingVehicleParams *)pCanister->Caller;
			pCanister->Position = pCanister->RelPosition;
			pCanister->Position *= v->Attitude;
			pCanister->Position += v->WorldPosition;

			if (pCanister->Light)
				(*(pCanister->Light))->WorldPosition = pCanister->Position;

		}
		else
		if( pCanister->Flag & CF_PHYSICS)	//This Only Moves the castister if it has a velocity;
		{
			distance = pCanister->Velocity;
			distance *= seconds_per_frame;	//d = vt
			pCanister->Position += distance;
			if (pCanister->Light)
				(*(pCanister->Light))->WorldPosition = pCanister->Position;

			//Add With/WO Gravity Check
			AttenuateVelocity( &(pCanister->Velocity), 0.4f);

		}

		// Check Emitor - Emit new particles
		old_seconds_per_frame = seconds_per_frame;
		old_parts = pCanister->ParticlesToEmmit;
		pCanister->ParticlesToEmmit += pCanister->EmmisionRate * seconds_per_frame;
		if (pCanister->EmmisionRate > 100)
		{
			seconds_per_frame = 0.0f;
			timer = 0.0f;
		}
		else
		{
			timer = 1.0/pCanister->EmmisionRate;
			seconds_per_frame = old_parts * timer;
		}

		while(pCanister->ParticlesToEmmit > 1.0f )
		{
			if (pParticle = NewParticle( pCanister))
			{
				if (seconds_per_frame)
					MoveParticle(pParticle);

				pCanister->ParticlesToEmmit--;

				seconds_per_frame += timer;
			}
			else
				pCanister->ParticlesToEmmit = 0.0f;

		}

		seconds_per_frame = old_seconds_per_frame;
	}

	// Move Canister's Paritcles
	pParticle = &pCanister->Particles[0];
	while((pCanister->Type) && (pParticle <= pCanister->pLastParticle))
	{
		if( pParticle->Type)
			MoveParticle( pParticle );
		pParticle++;
	}

}

void RemoveCanisterFromCaller(Canister *pCanister)
{
	if (pCanister->Caller)
	{
		if (pCanister->Flag & CF_CALLER_VEHICLE)
		{
			int i;
			MovingVehicleParams *V = (MovingVehicleParams *)pCanister->Caller;
			i =MAX_VEHICLE_SMOKE_SLOTS;
			while(i-- && (V->Smoke[i] != (DWORD)pCanister));
			if (i != -1)
				V->Smoke[i] = 0;
		}
		else
			if (!(pCanister->Flag & CF_CALLER_UNAWARE))
				*pCanister->Caller = NULL;


		pCanister->Caller = NULL;
	}
	pCanister->Flag &= ~(CF_CALLER_UNAWARE | CF_CALLER_VEHICLE | CF_CALLER_HANDLE | CF_ATTACHED_TO_VEHICLE | CF_DMG_CALLER_INSTANCE | CF_DMG_CALLER_VEHICLE);
}


void RemoveCanister( Canister *pCanister)
{
	NumCanisters--;
	NumParticles -= pCanister->NumParticles;

	RemoveCanisterFromCaller(pCanister);

	pCanister->Type = CT_NONE;
	if (pCanister->Light)
	{
		RemoveLight((LightSource **)pCanister->Light);
		pCanister->Light = NULL;
	}

	while((pLastCanister >= &Canisters[0]) && (pLastCanister->Type == CT_NONE))
		pLastCanister--;

}


//-----------------------------UTIL Functions  Should Inline these
void UpdateCanisterPos( DWORD hCan, FPointDouble &pos)
{
	Canister *pCanister = (Canister *)hCan;

	if( pCanister )
		SetCanisterPos( pCanister, pos);
}

void FreeCanisterHandle( DWORD hCan )
{
	Canister *pCanister = (Canister *)hCan;

	if(pCanister)
	{
		pCanister->Time = pCanister->TotalTime;
		pCanister->EmmisionRate = 0.0f;
		RemoveCanisterFromCaller(pCanister);
	}
}

/* ------------------------------------------2/27/98 11:00AM-------------------------------------------
 * These two are used only to put fires on vehicles
 * ----------------------------------------------------------------------------------------------------*/

DWORD GetFireHandle(FPointDouble &position,DWORD *ptr,CanisterType fire_type, float fire_size, float fire_duration)
{
	FPoint vel(0.0f);
	DWORD hCanister;

	if (g_Settings.gr.nGroundSmoke != 1)
	{
		if (hCanister = NewCanister( fire_type, position, vel,fire_size ,fire_duration ))
		{
			((Canister *)hCanister)->Caller = ptr;
			((Canister *)hCanister)->Flag |= CF_CALLER_HANDLE;
		}

		if (ptr)
			*ptr = hCanister;
	}
	else
		hCanister = 0;

	return( hCanister );
}

DWORD GetVehicleFireHandle(FPointDouble &position,void *ptr,CanisterType fire_type, float fire_size, float fire_duration,void *plane,float delay_start)
{
	MovingVehicleParams *V = (MovingVehicleParams *)ptr;
	FPoint vel(0.0f);
	DWORD hCanister;
	Canister *pCan;
	int i;
	int keeper;
	float difference;
	float d1;
	FPointDouble delta;
	FMatrix mat;

	if (g_Settings.gr.nGroundSmoke != 1)
	{
		/* -----------------8/12/99 2:23PM---------------------------------------------------------------------
		/* first thing we need to do is see if this vehicle has any open slots
		/* ----------------------------------------------------------------------------------------------------*/
		i = MAX_VEHICLE_SMOKE_SLOTS;

		while(i-- && V->Smoke[i]);

		if (i == -1)
		{
			/* -----------------8/12/99 2:25PM---------------------------------------------------------------------
			/* no open slots, lets see if we can remove one that's already there
			/* ----------------------------------------------------------------------------------------------------*/
			difference = 0.0;
			keeper = -1;
			i = MAX_VEHICLE_SMOKE_SLOTS;
			while(i-- && V->Smoke[i])
			{
				pCan = (Canister *)(V->Smoke[i]);
				if ((d1 = (pCan->Scale - fire_size)) > difference)
				{
					difference = d1;
					keeper = i;
				}
			}

			if (keeper != -1)
			{
				pCan = (Canister *)(V->Smoke[keeper]);
				pCan->Time = pCan->TotalTime;
				pCan->EmmisionRate=0.0f;
				pCan->Caller = 0;
				pCan->Flag &= ~CF_UNLIMTED_PARTICLES;
			}
			else
				return 0;

			i = keeper;
		}

		delta.MakeVectorToFrom(position,V->WorldPosition);
		mat = V->Attitude;
		mat.Transpose();
		delta *= mat;

		DiscreteLights = TRUE;
		if (hCanister = NewCanister( fire_type, delta, vel,fire_size ,fire_duration,NULL,delay_start ))
		{
			((Canister *)hCanister)->Caller = (DWORD *)V;
			((Canister *)hCanister)->Flag |= CF_ATTACHED_TO_VEHICLE | CF_CALLER_VEHICLE;
			((Canister *)hCanister)->RelPosition = ((Canister *)hCanister)->Position;
			((Canister *)hCanister)->PlanePtr = (DWORD)plane;

			if (((Canister *)hCanister)->Light)
				(*(((Canister *)hCanister)->Light))->WorldPosition += V->WorldPosition;

			if (plane)
				((Canister *)hCanister)->Flag |= CF_DMG_CALLER_VEHICLE;
		}
		DiscreteLights = FALSE;

		V->Smoke[i] = hCanister;
	}
	else
		hCanister = 0;

	return( hCanister );

}

void RemoveFireHandleCaller(DWORD handle)
{
	Canister *pCanister = (Canister *)handle;

	if (pCanister)
		pCanister->Caller = NULL;
}

void SaveVehicleSmoke(MovingVehicleParams *V,CanisterSaver &can)
{
	int i;
	SavedCan *cwalk;
	Canister *cptr;

	if (V)
	{
		i = MAX_VEHICLE_SMOKE_SLOTS;
		cwalk = &can.Cans[0];
		while(i--)
		{
			if (cptr = (Canister *)V->Smoke[i])
			{
				cwalk->Type = cptr->Type;
				cwalk->RelPosition = cptr->RelPosition;
				cwalk->Scale = cptr->Scale;
				cwalk->TotalTimeLeft = cptr->TotalTime - cptr->Time;
			}
			else
				cwalk->Type = CT_NONE;
			cwalk++;
		}
	}
}

void RestoreVehicleSmoke(MovingVehicleParams *V,CanisterSaver &can)
{
	int i;
	SavedCan *cwalk;
	Canister *cptr;
	FPoint vel(0.0f);

	if (V)
	{
		i = MAX_VEHICLE_SMOKE_SLOTS;
		cwalk = &can.Cans[0];
		while(i--)
		{
			if (cwalk->Type)
			{
				DiscreteLights = TRUE;
				if (cptr = (Canister *)(V->Smoke[i] = NewCanister( cwalk->Type, cwalk->RelPosition, vel,cwalk->Scale ,cwalk->TotalTimeLeft )))
				{
					cptr->Caller = (DWORD *)V;
					cptr->Flag |= CF_ATTACHED_TO_VEHICLE | CF_CALLER_VEHICLE;
					cptr->RelPosition = cptr->Position;
					if (cptr->Light)
						(*cptr->Light)->WorldPosition += V->WorldPosition;

				}
				DiscreteLights = FALSE;
			}
			else
				V->Smoke[i] = 0;
			cwalk++;
		}
	}
}
/* ------------------------------------------2/27/98 11:00AM-------------------------------------------
 * These three are used only to put permenant fires onto world objects
 * ----------------------------------------------------------------------------------------------------*/

void SetInstanceBurning(FPointDouble &position,void *ptr,DWORD fire_type, float fire_size, float fire_duration,void *pptr,float delay_start)
{
	DWORD hCanister;
	FPoint vel(0.0f);

	DiscreteLights = TRUE;
	if (hCanister = NewCanister( (CanisterType)fire_type, position, vel,fire_size ,fire_duration,NULL,delay_start ))
	{
		((Canister *)hCanister)->Caller = (DWORD *)ptr;
		((Canister *)hCanister)->Flag |= CF_CALLER_UNAWARE;
		((Canister *)hCanister)->PlanePtr = (DWORD)pptr;
		if (pptr)
			((Canister *)hCanister)->Flag |= CF_DMG_CALLER_INSTANCE;
	}
	DiscreteLights = FALSE;
}

void DamageAttachedInstance(Canister *pCan)
{

	BasicInstance *inst = (BasicInstance *)pCan->Caller;
	PlaneParams *plane = (PlaneParams *)pCan->PlanePtr;
	BOOL blew_something_up=FALSE;
	int totalsecondaries = 0;
	float fDamageValue;

	fDamageValue = seconds_per_frame * frand()*0.5;

	if	(DamageAnInstance(inst,DTE_DIRECT_HIT | DTE_NORMAL_TARGET,0.0f,fDamageValue,totalsecondaries,blew_something_up))
	{
		int ifacTargetType = -1;
		void *pfacTarget = NULL;
		int facnum;

		if(plane && (lBombFlags & WSO_FAC_CHECK))
		{
			facnum = AICGetClosestFAC(plane);
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

		if(ifacTargetType == GROUNDOBJECT)
		{
			if(inst == (BasicInstance *)pfacTarget)
			{
				if(plane == PlayerPlane)
				{
					iFACHitFlags |= FAC_HIT_DESTROYED;
				}
				else
				{
					AIC_FACAck(plane - Planes, 17);
				}
			}
		}

		FreeCanisterHandle((DWORD)pCan);

		LogDestroyedObject(NULL,GROUNDOBJECT,inst->SerialNumber);
	}
}

void DamageAttachedVehicle(Canister *pCan)
{
	MovingVehicleParams *checkvehicle = (MovingVehicleParams *)pCan->Caller;
	PlaneParams *plane = (PlaneParams *)pCan->PlanePtr;
	BOOL blew_something_up=FALSE;
	int totalsecondaries = 0;
	float fDamageValue;
	int num_secondaries;
	float secondary_delay;
	MultipleExplosionType secondaries;

	fDamageValue = seconds_per_frame * frand()*0.5f;

	if (VDamageVehicle(checkvehicle, DTE_DIRECT_HIT | DTE_NORMAL_TARGET, GameLoop, fDamageValue, &num_secondaries, &secondary_delay, &secondaries))
	{
		int ifacTargetType = -1;
		void *pfacTarget = NULL;
		int facnum;

		if(plane && (lBombFlags & WSO_FAC_CHECK))
		{
			facnum = AICGetClosestFAC(plane);
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

		if(ifacTargetType == MOVINGVEHICLE)
		{
			if(checkvehicle == (MovingVehicleParams *)pfacTarget)
			{
				if(plane == PlayerPlane)
				{
					iFACHitFlags |= FAC_HIT_DESTROYED;
				}
				else
				{
					AIC_FACAck(plane - Planes, 17);
				}
			}
		}


		LogDestroyedObject(NULL,MOVINGVEHICLE,(checkvehicle-MovingVehicles));

		if (num_secondaries)
		{
			if (!secondary_delay)
				ScheduleExplosion(checkvehicle->WorldPosition,secondaries, 0.0f,TRUE,NULL,NULL,checkvehicle);
			else
			{
				ScheduleExplosion(checkvehicle->WorldPosition,secondaries, 0.0f,FALSE,NULL,NULL,checkvehicle);
				ScheduleExplosion(checkvehicle->WorldPosition,secondaries, secondary_delay);
			}
		}
		else
			ScheduleExplosion(checkvehicle->WorldPosition,secondaries, 0.0f,FALSE,NULL,NULL,checkvehicle);

		FreeCanisterHandle((DWORD)pCan);
	}
}


DWORD GetWellHeadSmokeHandle( FPointDouble &position,DWORD *ptr)
{
	FPoint vel(0.0f);
	DWORD hCanister;

	if (g_Settings.gr.nGroundSmoke != 1)
	{
		DiscreteLights = TRUE;
		if (hCanister = NewCanister( CT_WELLHEAD_FIRE, position, vel, 3.0f ))
			((Canister *)hCanister)->Caller = ptr;
		DiscreteLights = FALSE;


		if (ptr)
			*ptr = hCanister;
	}
	else
		hCanister = 0;

	return( hCanister );

}

DWORD GetBurnoffSmokeHandle( FPointDouble &position,DWORD *ptr)
{
	FPoint vel(0.0f);
	DWORD hCanister;

	if (g_Settings.gr.nGroundSmoke != 1)
	{
		if (hCanister = NewCanister( CT_BURNOFF_SMOKE, position, vel, 0.0f ))
			((Canister *)hCanister)->Caller = ptr;

		if (ptr)
			*ptr = hCanister;
	}
	else
		hCanister = 0;

	return( hCanister );

}

DWORD GetSteamSmokeHandle( FPointDouble &position,DWORD *ptr)
{
	FPoint vel(0.0f);
	DWORD hCanister;

	if (g_Settings.gr.nGroundSmoke != 1)
	{
		if (hCanister = NewCanister( CT_STEAM_SMOKE, position, vel, 0.0f ))
			((Canister *)hCanister)->Caller = ptr;

		if (ptr)
			*ptr = hCanister;
	}
	else
		hCanister = 0;

	return( hCanister );

}


void MakeCatSteamCanister(FPointDouble &rel_position,void *ptr)
{
	MovingVehicleParams *v = (MovingVehicleParams *)ptr;
	FPoint vel(0.0f);
	DWORD hCanister;

	if (g_Settings.gr.nGroundSmoke != 1)
	{
		if (hCanister = NewCanister( CT_CAT_STEAM, rel_position, vel, 1.0f ))
			((Canister *)hCanister)->Caller = (DWORD *)ptr;
	}
	else
		hCanister = 0;
}


void SetCanisterPos( Canister *pCanister, FPointDouble &position)
{
	pCanister->Position = position;
	if (pCanister->Light)
		(*(pCanister->Light))->WorldPosition = position;
}


void ApplayTimeToCanister( Canister *pCanister, float seconds )
{
	// only really for smoke ie: burning smoke stacks don't start up on runways
	int i;
	int numsteps = 32;

	seconds_per_frame = seconds/numsteps;				// create dt
	for( i=0; i<numsteps; i++)
		MoveCanister( pCanister );
}



int NextPriority[] = {3,4,5,1,2,0};
//---------------------------------------------------------------------------------------
Particle *NewParticle( Canister *pCanister)
{
	Particle *pParticle;

	if ((pCanister->NumParticles < pCanister->MaxParticles) || (pCanister->Flag & CF_UNLIMTED_PARTICLES))
	{
		pParticle = &pCanister->Particles[0];
		while((pParticle <= pCanister->pLastParticle) && (pParticle->Type))
			pParticle++;

		if (pParticle < &pCanister->Particles[MAX_PARTICLES_PER_CAN])
		{
			pParticle->pCanister= pCanister;
			pParticle->Type		= pCanister->TypeParticle;
			pParticle->Position = pCanister->Position;
			pParticle->Time		= 0.0f;
			pParticle->FlirHeat = 0.0f;
			pParticle->Priority = pCanister->Priority;
			pCanister->Priority = NextPriority[pCanister->Priority];

			if( pParticle> pCanister->pLastParticle)
				pCanister->pLastParticle = pParticle;

			SetParticleType( pCanister, pParticle );

			pCanister->NumParticles++;
			NumParticles++;
		}
		else
			pParticle = NULL;
	}
	else
		pParticle = NULL;

	return pParticle;;
}

void SetParticleType( Canister *pCanister, Particle *pParticle )
{
	float tmp;
	int color_source;

	switch(pParticle->Type)
	{

		case PT_EXPLOSION_GRAY:
			color_source = COLOR_SOURCE_GRAY;
//			pParticle->PalNum			= color_source * 3;
			pParticle->FlirHeat			= 0.2f;
			pParticle->Red				= 0.5f;
			pParticle->Green			= 0.5f;
			pParticle->Blue				= 0.5f;
			goto start_going;

		case PT_EXPLOSION_BLACK:
			color_source = COLOR_SOURCE_BLACK;
//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.4f;
//			pParticle->FlirPalNum		= 0x43 * 3;
			pParticle->Red				= 0.1f;
			pParticle->Green			= 0.1f;
			pParticle->Blue				= 0.1f;
			goto start_going;

		case PT_EXPLOSION_DIRT:
			color_source = COLOR_SOURCE_DIRT;
//			pParticle->PalNum			= color_source * 3;
			pParticle->FlirHeat			= 0.2f;//0x43 * 3;
			pParticle->Red				= 0.7f;
			pParticle->Green			= 0.5f;
			pParticle->Blue				= 0.5f;

			start_going:

			pParticle->Flag				= PF_ALHPA_RATE | PF_DEST_BASED| PF_GRAVITY|PF_FRAME_RATE | PF_CALC_LIGHTING;
			pParticle->SType			= &SpriteTypes[SPT_BLACK_PUFF];
			pParticle->Model			= NULL;

			pParticle->Velocity.SetValues( 0.0f);
			pParticle->TotalTime		= 2.0 + 1.5*pCanister->Scale;//115			// and disappear after 5 seconds or so



			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 192.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2 ;

			tmp							= 1.0 - ( ( (float)pCanister->NumParticles+frand()) / (( float)pCanister->MaxParticles+1.0f) );

			pParticle->DestTime			= 0.5 * (pCanister->Scale * pCanister->Scale);//1.0;							// Time to reach Dest

			pParticle->Dest				= pCanister->Dest;
		  	pParticle->Dest				*= 1.0-(tmp * tmp);
			pParticle->Dest				+= pParticle->Position;

			pParticle->StepPercentage	= pCanister->StepPercentage;

			pParticle->Frame			= 0;//pParticle->SType->NumFrames;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= 2.0f + 1.0f * (frand() - 0.5f);	//Frams/Sec

			if( tmp> 0.5f)
				pParticle->FrameRate	*= 1.0 + ((tmp * tmp))*5.0;
			else
				pParticle->FrameRate	*= 1.0 + (1.0-(tmp * tmp))*5.0; //orig

			pParticle->Orientation		= (pCanister->NumParticles + GameLoop ) & 0xF;

			pParticle->Scale			= pCanister->Scale * 0.1f;		//EndScale
			pParticle->StartScale		= pCanister->Scale * (tmp);
			pParticle->ScaleRate		= ((pParticle->Scale- pParticle->StartScale)/(float)pCanister->MaxParticles);
			pParticle->ScaleRate		*= (float)(pCanister->MaxParticles - pCanister->NumParticles);
			pParticle->ScaleRate		=  (pParticle->ScaleRate/pParticle->DestTime);
			pParticle->gop				= PRIM_TEXTURE | PRIM_ALPHA;
			break;


		case PT_EXPLOSION_FIRE:
			pParticle->Type				= PT_EXPLOSION_FIRE;
			pParticle->Flag				= PF_ALHPA_RATE | PF_DEST_BASED| PF_GRAVITY|PF_FRAME_RATE;
			pParticle->SType			= &SpriteTypes[SPT_OILFIRE];
			pParticle->Model			= NULL;

			pParticle->Velocity.SetValues( 0.0f);
			pParticle->TotalTime		= 2.0 + 1.5*pCanister->Scale;//115			// and disappear after 5 seconds or so

//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.9;
			pParticle->Frame			= 0;
			pParticle->Red				= 1.0;
			pParticle->Green			= 1.0;
			pParticle->Blue				= 1.0;

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 192.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2 ;

			//tmp	= 1.0 - ( ( (float)pCanister->CurrentParticle+0.5) / (( float)pCanister->MaxParticles+1.0f) );
			tmp	= 1.0 - ( ( (float)pCanister->NumParticles+frand()) / (( float)pCanister->MaxParticles+1.0f) );


			pParticle->DestTime			= 0.5 * (pCanister->Scale * pCanister->Scale);//1.0;							// Time to reach Dest

			pParticle->Dest				= pCanister->Dest;
		  	pParticle->Dest				*= 1.0-(tmp * tmp);
			pParticle->Dest				+= pParticle->Position;

			//pParticle->Dest				*= 1.0f/pParticle->DestTime;
		  	//pParticle->Dest				*= ((float)pCanister->CurrentParticle/(float)pCanister->MaxParticles);

			pParticle->StepPercentage	= pCanister->StepPercentage;

			pParticle->Frame			= 0;
			pParticle->StartFrame		= 0;
			/*
			pParticle->FrameRate		= 2.0f + 1.0f * (frand() - 0.5f);	//Frams/Sec

			if( tmp> 0.5f)
				pParticle->FrameRate	*= 1.0 + ((tmp * tmp))*5.0;
			else
				pParticle->FrameRate	*= 1.0 + (1.0-(tmp * tmp))*5.0; //orig
			*/

			pParticle->FrameRate 		= 1.0+(1.0-tmp)*6.0f;
			pParticle->Orientation		= (pCanister->NumParticles + GameLoop ) & 0xF;

			pParticle->Scale			= pCanister->Scale * 0.1f;		//EndScale
			pParticle->StartScale		= pCanister->Scale * (tmp);
			pParticle->ScaleRate		= ((pParticle->Scale- pParticle->StartScale)/(float)pCanister->MaxParticles);
			pParticle->ScaleRate		*= (float)(pCanister->MaxParticles - pCanister->NumParticles);
			pParticle->ScaleRate		=  (pParticle->ScaleRate/pParticle->DestTime);
			pParticle->gop				= PRIM_ALPHA_ADD |PRIM_TEXTURE|PRIM_ALPHA;

			break;

		case PT_BURNOFF_SMOKE:
			pParticle->Type				= PT_BURNOFF_SMOKE;
			pParticle->Flag				= PF_FRAME_RATE| PF_ALHPA_RATE | PF_RANDOM_DIFFUSION | PF_ANGULAR_VELOCITY;
			pParticle->SType			= &SpriteTypes[SPT_OILFIRE];
			pParticle->Model			= NULL;
			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 10.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 2.5;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= (2.0f + 2.0f * frand() - 1.0f);	//Frams/Sec
			pParticle->Orientation		= (pCanister->NumParticles + GameLoop ) & 0xF;

			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (720.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,360.0f DEGREES,10.0f DEGREES);

//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.9f;
			pParticle->Red				= 1.0f;
			pParticle->Green			= 1.0f;
			pParticle->Blue				= 1.0f;

			pParticle->Scale			= pCanister->Scale * 0.15f;		//EndScale
			pParticle->StartScale		= pCanister->Scale * 0.05f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 254.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 0 ;
			pParticle->gop				= PRIM_ALPHA_ADD |PRIM_TEXTURE|PRIM_ALPHA;
			break;

		case PT_GREEN_MARKER_SMOKE:
			pParticle->Red				= 84.0f/255.0f;
			pParticle->Green			= 149.0f/255.0f;
			pParticle->Blue				= 90.0f/255.0f;
			goto finish_marker_smoke;

		case PT_RED_MARKER_SMOKE:
			pParticle->Red				= 191.0f/255.0f;
			pParticle->Green			= 71.0f/255.0f;
			pParticle->Blue				= 68.0f/255.0f;
			goto finish_marker_smoke;

		case PT_BLUE_MARKER_SMOKE:
			pParticle->Red				= 65.0f/255.0f;
			pParticle->Green			= 73.0f/255.0f;
			pParticle->Blue				= 149.0f/255.0f;
			goto finish_marker_smoke;

		case PT_YELLOW_MARKER_SMOKE:
			pParticle->Red				= 190.0f/255.0f;
			pParticle->Green			= 170.0f/255.0f;
			pParticle->Blue				= 56.0f/255.0f;
			goto finish_marker_smoke;

		case PT_ORANGE_MARKER_SMOKE:
			pParticle->Red				= 190.0f/255.0f;
			pParticle->Green			= 135.0f/255.0f;
			pParticle->Blue				= 56.0f/255.0f;

		finish_marker_smoke:
			pParticle->Type				= PT_STEAM_SMOKE;
			pParticle->Flag				= PF_RANDOM_DIFFUSION |PF_ALHPA_RATE |PF_SCALE_RATE | PF_CALC_LIGHTING | PF_SPRITE | PF_ANGULAR_VELOCITY;

			pParticle->SType			= &SpriteTypes[SPT_WHITE_PUFF];
			pParticle->Model			= NULL;

			pParticle->Pitch			 = (90.0f DEGREES) *frand();
			pParticle->Heading			 = (90.0f DEGREES) *frand();

			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 6.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;

			pParticle->TotalTime		= 15;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;
			pParticle->FlirHeat			= 0.1f;
//			pParticle->PalNum			= 0;

			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (720.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,360.0f DEGREES,10.0f DEGREES);

			pParticle->Scale			= pCanister->Scale;		//EndScale
			pParticle->StartScale		= pCanister->Scale * 0.2f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0f;
			pParticle->StartAlpha		= 0.9f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2;
			pParticle->gop				= PRIM_TEXTURE| PRIM_ALPHA;
			break;



		case PT_BLACK_SMOKE:
			pParticle->Type				= PT_BLACK_SMOKE;
			pParticle->Flag				= PF_RANDOM_DIFFUSION |PF_ALHPA_RATE |PF_SCALE_RATE | PF_CALC_LIGHTING| PF_SPRITE | PF_ANGULAR_VELOCITY;
			pParticle->SType			= &SpriteTypes[SPT_BLACK_PUFF];
			pParticle->Model			= NULL;
			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 4.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 12;// SECONDS; // rand() % 30 + MinTime


			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (720.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,720.0f DEGREES,10.0f DEGREES);

//			pParticle->PalNum			= 0;
			pParticle->FlirHeat	   		= 0.4f;
			pParticle->Red				= 0.1f;
			pParticle->Green			= 0.1f;
			pParticle->Blue				= 0.1f;
			pParticle->Frame			= 0;

			pParticle->Scale			= pCanister->Scale*1.0;		//EndScale
//			pParticle->StartScale		= pCanister->Scale * 0.1f;
			pParticle->StartScale		= pCanister->Scale* 0.35f;
			pParticle->ScaleRate		= (pParticle->Scale - pParticle->StartScale)/ ((float)(pParticle->SType->NumFrames - 3)/pParticle->FrameRate); ;

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 0.9f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2;
			pParticle->gop				= PRIM_TEXTURE| PRIM_ALPHA;
			break;

		case PT_WELLHEAD_FIRE:
			pParticle->Type				= PT_WELLHEAD_FIRE;
			pParticle->Flag				= PF_FRAME_RATE | PF_FIRE_ALPHA_RATE | PF_SPRITE | PF_ANGULAR_VELOCITY;// |PF_RANDOM_DIFFUSION;
			pParticle->SType			= &SpriteTypes[SPT_2D_OILFIRE];
			pParticle->Model			= NULL;
			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 20.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 8.0;// SECONDS; // rand() % 30 + MinTime

			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (720.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,360.0f DEGREES,10.0f DEGREES);

			pParticle->Frame			= 0;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= (4.0f + 1.0f * frand() - 0.5f);	//Frams/Sec
			pParticle->Orientation		= (pCanister->NumParticles + GameLoop ) & 0xF;

			pParticle->FlirHeat	   		= 0.9f;
			pParticle->Red				= 1.0f;
			pParticle->Green			= 1.0f;
			pParticle->Blue				= 1.0f;


			pParticle->Scale			= pCanister->Scale * 0.30f;		//EndScale
			pParticle->StartScale		= pCanister->Scale * 0.05f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;


			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 254.0f/255.0f;

			pParticle->FireTimeToFirstAlpha = (pParticle->SType->NumFrames-3)/pParticle->FrameRate;

			pParticle->FireAlpha	    = (pParticle->Alpha - pParticle->StartAlpha)/ (pParticle->TotalTime-pParticle->FireTimeToFirstAlpha);
			pParticle->RandomDiffusion	= 0 ;
			pParticle->gop				= PRIM_ALPHA_ADD |PRIM_TEXTURE|PRIM_ALPHA;

			break;


		case PT_FLASH:
			pParticle->Type				= PT_FLASH;
			pParticle->Flag				= PF_ALHPA_RATE | PF_SPRITE;
			pParticle->SType			= &SpriteTypes[SPT_FLASH];
			pParticle->Model			= NULL;
			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 0.0f;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 0.75;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;
//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 1.0f;
			pParticle->Red				= 0.8f;
			pParticle->Green			= 0.8f;
			pParticle->Blue				= 0.9f;


			pParticle->Scale			= pCanister->Scale;		//EndScale

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 1.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;
			pParticle->gop				= PRIM_TEXTURE | PRIM_ALPHA;


			break;

		case PT_STEAM_SMOKE:
			pParticle->Type				= PT_STEAM_SMOKE;
			pParticle->Flag				= PF_RANDOM_DIFFUSION |PF_ALHPA_RATE |PF_SCALE_RATE | PF_CALC_LIGHTING | PF_SPRITE  | PF_ANGULAR_VELOCITY;

			if( frand() < 0.75)
				pParticle->Flag	|= PF_ANGULAR_VELOCITY;
			pParticle->SType			= &SpriteTypes[SPT_WHITE_PUFF];
			pParticle->Model			= NULL;

			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (360.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,90.0f DEGREES,10.0f DEGREES);
			//pParticle->AngularVelocity.Z = 5.0f DEGREES * frand();

			pParticle->Pitch			 = (90.0f DEGREES) *frand();
			pParticle->Heading			 = (90.0f DEGREES) *frand();


			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 6.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;

			pParticle->TotalTime		= 15;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;
			pParticle->FlirHeat			= 0.4f;
//			pParticle->PalNum			= 0;
			pParticle->Red				= 0.9f;
			pParticle->Green			= 0.9f;
			pParticle->Blue				= 0.9f;


			pParticle->Scale			= pCanister->Scale;		//EndScale
			pParticle->StartScale		= pCanister->Scale * 0.2f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0f;
			pParticle->StartAlpha		= 150.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2;
			pParticle->gop				= PRIM_TEXTURE| PRIM_ALPHA;
			break;



		case PT_DAMAGE_SMOKE:
			pParticle->Type				= PT_DAMAGE_SMOKE;
			pParticle->Flag				= PF_RANDOM_DIFFUSION |PF_ALHPA_RATE |PF_SCALE_RATE | PF_CALC_LIGHTING | PF_SPRITE | PF_ANGULAR_VELOCITY;
			pParticle->SType			= &SpriteTypes[SPT_BLACK_PUFF];
			pParticle->Model			= NULL;
			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 4.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 10;// SECONDS; // rand() % 30 + MinTime

			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (720.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,360.0f DEGREES,10.0f DEGREES);

			pParticle->Frame			= 0;
//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.4f;
			pParticle->Red				= 0.1f;
			pParticle->Green			= 0.1f;
			pParticle->Blue				= 0.1f;

			pParticle->Scale			= pCanister->Scale;		//EndScale
			pParticle->StartScale		= 0.1f * pCanister->Scale;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 128.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2;
			pParticle->gop				= PRIM_TEXTURE | PRIM_ALPHA;
			break;


		case PT_OIL_FIRE:
			pParticle->Type				= PT_OIL_FIRE;
			pParticle->Flag				= PF_FRAME_RATE | PF_ALHPA_RATE |PF_SCALE_RATE |PF_RANDOM_DIFFUSION | PF_SPRITE | PF_ANGULAR_VELOCITY;
			pParticle->SType			= &SpriteTypes[SPT_2D_OILFIRE];
			pParticle->Model			= NULL;


			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES) * frand() + (720.0f DEGREES),(10.0f DEGREES) * frand()); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,360.0f DEGREES,10.0f DEGREES);

			pParticle->Pitch			 = (90.0f DEGREES) *frand();
			pParticle->Heading			 = (90.0f DEGREES) *frand();


			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 10.0f*pCanister->Scale;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 6;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;//pParticle->SType->NumFrames;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= (2.0f + 2.0f * frand() - 1.0f)*2.0;	//Frams/Sec
			pParticle->Orientation		= (pCanister->NumParticles + GameLoop ) & 0xF;

			//pParticle->Frame			= 0;
//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.9f;
			pParticle->Red				= 0.9f;//1.0f;
			pParticle->Green			= 0.65f;//1.0f;
			pParticle->Blue				= 0.02f;//1.0f;

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 3.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ ((float)(pParticle->SType->NumFrames - 3)/pParticle->FrameRate);

			pParticle->Scale			= pCanister->Scale*1.0;		//EndScale
//			pParticle->StartScale		= pCanister->Scale * 0.1f;
			pParticle->StartScale		= pCanister->Scale* 0.35f;
			pParticle->ScaleRate		= (pParticle->Scale - pParticle->StartScale)/ ((float)(pParticle->SType->NumFrames - 3)/pParticle->FrameRate); ;


			pParticle->FireAlpha		= (pParticle->Alpha - pParticle->StartAlpha)/ (10 - (5.0f/pParticle->Frame));
			pParticle->RandomDiffusion	= 0 ;
			pParticle->gop				= PRIM_ALPHA_ADD |PRIM_TEXTURE|PRIM_ALPHA;
			break;


		case PT_EXPLOSION_WATER:
			pParticle->Type				= PT_EXPLOSION_WATER;
			pParticle->Flag				= PF_ALHPA_RATE | PF_DEST_BASED| PF_GRAVITY|PF_FRAME_RATE | PF_CALC_LIGHTING | PF_SPRITE;
			pParticle->SType			= &SpriteTypes[SPT_WHITE_PUFF];
			pParticle->Model			= NULL;

			pParticle->Velocity.SetValues( 0.0f);
			pParticle->TotalTime		= 2.0 + 2.5*pCanister->Scale;//115			// and disappear after 5 seconds or so

//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.2f;
			pParticle->Red				= 0.9f;
			pParticle->Green			= 0.9f;
			pParticle->Blue				= 0.9f;


			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 128.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2 ;

			//tmp	= 1.0 - ( ( (float)pCanister->CurrentParticle+0.5) / (( float)pCanister->MaxParticles+1.0f) );
			tmp	= 1.0 - ( ( (float)pCanister->NumParticles+frand()) / (( float)pCanister->MaxParticles+1.0f) );


			pParticle->DestTime			= 0.5 * (pCanister->Scale * pCanister->Scale);//1.0;							// Time to reach Dest

			pParticle->Dest				= pCanister->Dest;
		  	pParticle->Dest				*= 1.0-(tmp * tmp);
			pParticle->Dest				+= pParticle->Position;

			//pParticle->Dest				*= 1.0f/pParticle->DestTime;
		  	//pParticle->Dest				*= ((float)pCanister->CurrentParticle/(float)pCanister->MaxParticles);

			pParticle->StepPercentage	= pCanister->StepPercentage;

			pParticle->Frame			= 0;//pParticle->SType->NumFrames;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= 2.0f + 1.0f * (frand() - 0.5f);	//Frams/Sec

			if( tmp> 0.5f)
				pParticle->FrameRate	*= 1.0 + ((tmp * tmp))*5.0;
			else
				pParticle->FrameRate	*= 1.0 + (1.0-(tmp * tmp))*5.0; //orig

			pParticle->Orientation		= (pCanister->NumParticles + GameLoop ) & 0xF;

			pParticle->Scale			= pCanister->Scale * 0.1f;		//EndScale
			pParticle->StartScale		= pCanister->Scale * (tmp);
			pParticle->ScaleRate		= ((pParticle->Scale- pParticle->StartScale)/(float)pCanister->MaxParticles);
			pParticle->ScaleRate		*= (float)(pCanister->MaxParticles - pCanister->NumParticles);
			pParticle->ScaleRate		=  (pParticle->ScaleRate/pParticle->DestTime);

			pParticle->gop				= PRIM_TEXTURE | PRIM_ALPHA;

			break;


		case PT_MISSLE_EXPLODE:
			pParticle->Type				= PT_MISSLE_EXPLODE;
			pParticle->Flag				= PF_FRAME_RATE | PF_ALHPA_RATE | PF_CALC_LIGHTING | PF_SPRITE;
			pParticle->SType			= &SpriteTypes[SPT_AIR_EXP];
			pParticle->Model			= NULL;
			pParticle->Velocity.X		= 0.0f;
			pParticle->Velocity.Y		= 0.5f;
			pParticle->Velocity.Z		= 0.0f;
			pParticle->TotalTime		= 5;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= 6.0f;

//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.9f;
			pParticle->Red				= 0.9f;
			pParticle->Green			= 0.9f;
			pParticle->Blue				= 0.9f;

			pParticle->Scale			= pCanister->Scale;		//EndScale

			pParticle->Alpha			= 0.0f;		//EndAlpha
			pParticle->StartAlpha		= 200.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;
			pParticle->gop				= PRIM_TEXTURE ;

			break;


		case PT_SPIDER_SMOKE:

			pParticle->Type				= PT_SPIDER_SMOKE;
			pParticle->Flag				= PF_GRAVITY |PF_ALHPA_RATE |PF_SCALE_RATE | PF_CALC_LIGHTING;
			pParticle->SType			= &SpriteTypes[SPT_WHITE_PUFF];
			pParticle->Model			= NULL;

			//pParticle->Velocity.X		= PVel.X;//(vel - (rand() % vel)) << 1;
			//pParticle->Velocity.Y		= PVel.Y;//(15 METERS  + (rand() % vel) );
			//pParticle->Velocity.Z		= PVel.Z;//(vel - (rand() % vel)) << 1;
			pParticle->TotalTime		= 4;//115			// and disappear after 5 seconds or so

			pParticle->Frame			= pParticle->SType->NumFrames;
			pParticle->StartFrame		= 0;
			pParticle->FrameRate		= 10;	//Frams/Sec

			pParticle->Frame			= 0;
//			pParticle->PalNum			= 0;
			pParticle->FlirHeat			= 0.4f;
			pParticle->Red				= 0.9f;
			pParticle->Green			= 0.9f;
			pParticle->Blue				= 0.9f;

			pParticle->Scale			= 0.1f;		//EndScale
			pParticle->StartScale		= 1.0f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0;		//EndAlpha
			pParticle->StartAlpha		= 128.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2 ;
			pParticle->gop				= PRIM_TEXTURE | PRIM_ALPHA;
			break;



		case PT_FLYING_PARTS:
			pParticle->Velocity.X		 = (100 METERS * 2.0) * (frand()-0.5);
			pParticle->Velocity.Y		 = (100 METERS * 2.0) * (frand()-0.5);
			pParticle->Velocity.Z		 = (100 METERS * 2.0) * (frand()-0.5);
			goto finish_parts;

		case PT_FLYING_GROUND_PARTS:
			pParticle->Velocity.X		 = (100 METERS) * (frand()-0.5);
			pParticle->Velocity.Y		 = 2 METERS + (50 METERS) * frand();
			pParticle->Velocity.Z		 = (100 METERS) * (frand()-0.5);

		finish_parts:
			pParticle->Type				 = PT_FLYING_PARTS;
			pParticle->Flag				 = PF_GRAVITY | PF_ANGULAR_VELOCITY;
			pParticle->Model			 = PlanePartTypes[rand() % NUM_PLANE_PART_MODELS].Model;
			pParticle->AngularVelocity.X = 90.0f DEGREES;
			pParticle->AngularVelocity.Y = 360.0f*3.0f DEGREES; // Deg/Sec
			pParticle->AngularVelocity.Z = 10.0f DEGREES;
			pParticle->AngularAcceleration = 0.0f;
			pParticle->TotalTime		 = 10;// SECONDS; // rand() % 30 + MinTime
			pParticle->Roll				 = rand() & 0xFFFF;
			pParticle->Pitch			 = 0;
			pParticle->Heading			 = rand() & 0xFFFF;
			pParticle->SType			 = NULL;
			break;

		case PT_DOME:
			pParticle->Type				= PT_DOME;
			pParticle->Flag				= PF_ALHPA_RATE |PF_SCALE_RATE| PF_ANGULAR_VELOCITY;//| PF_FRAME_RATE ;
			pParticle->SType			= &SpriteTypes[SPT_ALPHA_MAP];//&SpriteTypes[SPT_OILFIRE];;//
			pParticle->Model			= NULL;
			pParticle->TotalTime		= 0.76f;// SECONDS; // rand() % 30 + MinTime


			pParticle->AngularVelocity.Y = (90.0f DEGREES) * frand(); // Deg/Sec
			pParticle->AngularVelocity.Z = (10.0f DEGREES) * frand();

			pParticle->Pitch			 = (90.0f DEGREES) *frand();
			pParticle->Heading			 = (90.0f DEGREES) *frand();



			pParticle->Velocity.X		 = 0.0f;
			pParticle->Velocity.Y		 = 0.0f;
			pParticle->Velocity.Z		 = 0.0f;

			pParticle->Red				= 0.9f;
			pParticle->Green			= 0.9f;
			pParticle->Blue				= 0.9f;

			pParticle->Scale			= 4.0f;		//EndScale
			pParticle->StartScale		= 0.1f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0;		//EndAlpha
			pParticle->StartAlpha		= 0.6f;//128.0f/255.0f;

			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;
			pParticle->gop				= PRIM_ALPHA_ADD |PRIM_TEXTURE|PRIM_ALPHA;
			break;

		case PT_CAT_STEAM:
			pParticle->Type				= PT_CAT_STEAM;
			pParticle->Flag				= PF_RANDOM_DIFFUSION |PF_ALHPA_RATE |PF_SCALE_RATE | PF_CALC_LIGHTING | PF_AT_LEAST_ONE | PF_ATTACHED_TO_CANISTER | PF_ANGULAR_VELOCITY;

			pParticle->SType			= &SpriteTypes[SPT_WHITE_PUFF];
			pParticle->Model			= NULL;

			pParticle->AngularVelocity.SetValues(0.0f,(90.0f DEGREES )* frand() + (720.0f DEGREES ),0.0f); // Deg/Sec
			pParticle->AngularAcceleration.SetValues(0.0f,600.0f DEGREES,0.0f);
			//pParticle->AngularVelocity.Z = 5.0f DEGREES * frand();

			pParticle->Pitch			 = (90.0f DEGREES) *frand();
			pParticle->Heading			 = (90.0f DEGREES) *frand();


			pParticle->Velocity = pCanister->Velocity;
//			pParticle->Velocity.X		= 0.0f;
//			pParticle->Velocity.Y		= 0.5f*pCanister->Scale;
//			pParticle->Velocity.Z		= 0.0f;

			pParticle->TotalTime		= 6;// SECONDS; // rand() % 30 + MinTime

			pParticle->Frame			= 0;
			pParticle->FlirHeat			= 0.4f;
//			pParticle->PalNum			= 0;
			pParticle->Red				= 0.9f;
			pParticle->Green			= 0.9f;
			pParticle->Blue				= 0.9f;


			pParticle->Scale			= pCanister->Scale*0.15;		//EndScale
			pParticle->StartScale		= pCanister->Scale * 0.04f;
			pParticle->ScaleRate		= (pParticle->Scale- pParticle->StartScale)/pParticle->TotalTime;

			pParticle->Alpha			= 0.0f;
			pParticle->StartAlpha		= 150.0f/255.0f;
			pParticle->AlphaRate		= (pParticle->Alpha - pParticle->StartAlpha)/ pParticle->TotalTime;

			pParticle->RandomDiffusion	= 2;
			pParticle->gop				= PRIM_TEXTURE| PRIM_ALPHA;
			break;

	}

}




void RemoveParticle( Particle *pParticle)
{
	Particle *pLastParticle;

	pParticle->Type = PT_NONE;
	//if(pParticle->SortHeader )
	// UnsortObject( pParticle );

	pParticle->SortHeader = NULL;

	pParticle->pCanister->NumParticles--;
	NumParticles--;

	if (pParticle->pCanister->NumParticles)
	{
		if (pParticle == (pLastParticle = pParticle->pCanister->pLastParticle))
		{
			do{
				pLastParticle--;
			} while ((pLastParticle >= &pParticle->pCanister->Particles[0]) && (pLastParticle->Type == PT_NONE));

			pParticle->pCanister->pLastParticle = pLastParticle;
		}
	}
	else
		RemoveCanister(pParticle->pCanister);

}

void DecelerateOneAxis(float &value,float &accel)
{
	if (value > 0.0f)
		if (accel >= value)
		{
			accel = -value;
			value = 0.0f;
		}
		else
		{
			value -= accel;
			accel = -accel;
		}
	else
		if (value < 0.0f)
			if (-accel <= value)
			{
				value = 0.0f;
				accel = -value;
			}
			else
				value += accel;
		else
			accel = 0.0f;
}

void MoveParticle( Particle *pParticle)
{
	// Check Totat Time Remove if Necessay KEEP TRACK OF NUM PARTICLES
	if (!(pParticle->Flag & PF_AT_LEAST_ONE))
		pParticle->Time += seconds_per_frame;
	else
		pParticle->Flag &= ~PF_AT_LEAST_ONE;

	if( pParticle->Time > pParticle->TotalTime)						// Check Totat Time .. Remove if Necesart
		RemoveParticle( pParticle);
	else
	{

		//if( GroundCollision( pParticle->Position.X, pParticle->Position.Y, pParticle->Position.Z) )
			//RemoveParticle( pParticle);				// MIGHT WANT TO ADDED BOUNCING


		//TEST
		//if(	pParticle->Flag	& PF_GRAVITY)
		//	AttenuateVelocity( &(pParticle->Velocity),  (pParticle->Time/ pParticle->TotalTime) );


		if (pParticle->Flag & PF_ATTACHED_TO_CANISTER)
			pParticle->Position = pParticle->pCanister->Position;
		else
			if( pParticle->Flag & PF_DEST_BASED)
			{
				pParticle->DestTime -= seconds_per_frame;


				FPoint dist;
				// Move Particle Towards Destination
				dist.MakeVectorToFrom(pParticle->Dest, pParticle->Position);

				float tmp = sqrtDeltaTicks * pParticle->StepPercentage;//0.005f; //MAKE Sqrt a global
				if (tmp > 1.0f)
					tmp = 1.0f;

				dist *= tmp;

				pParticle->Position += dist;

				pParticle->Scale += (pParticle->StartScale - pParticle->Scale)*tmp;

				if (pParticle->Flag	& PF_GRAVITY)
				{
					pParticle->Dest.AddScaledVector(seconds_per_frame, pParticle->Velocity);
					pParticle->Dest.Y -= seconds_per_frame*seconds_per_frame * (8.0 METERS * 0.5);

					pParticle->Velocity.Y -= seconds_per_frame * 8.0 METERS;
				}

				if (dist.Y < 0.0)
				{
					float h;
					h = LandHeight( pParticle->Position.X, pParticle->Position.Z);
					if( pParticle->Position.Y< h)
						pParticle->Position.Y = h;
				}


	//				if( pParticle->DestTime > 0.0)
				//if( pParticle->SType->NumFrames-3 )
	//				{

				//	pParticle->Scale = pParticle->ScaleRate * pParticle->Time;
					//pParticle->Dest.Y -= seconds_per_frame * 20 * 9.8 METERS * (1-pParticle->Scale);
	//				}

			}
			else
			{
				if( (pParticle->Velocity.Y < 0.0f) && GroundCollision( pParticle->Position.X, pParticle->Position.Y, pParticle->Position.Z))
				{
					RemoveParticle( pParticle);
					return;
				}

				// Applay Physics on Paritcle
				pParticle->Position.AddScaledVector(seconds_per_frame,pParticle->Velocity);

				if(	pParticle->Flag	& PF_RANDOM_DIFFUSION )
				{

					int rand_index;


					rand_index = rand()&31;
					pParticle->Position += wind_vector;			//add wind vector to position
					pParticle->Position += random_diffusion[pParticle->RandomDiffusion][rand_index];
				}

				if(	pParticle->Flag	& PF_GRAVITY)
					AttenuateVelocity( &(pParticle->Velocity), 0.4f);
				//else
				//	AttenuateVelocityNoG( &(pParticle->Velocity), pParticle->Time/pParticle->TotalTime);
			}



		if(	pParticle->Flag	& PF_ANGULAR_VELOCITY)
		{
			//Apply Angular Rotaion Physics On Object
			FPoint angular(seconds_per_frame,pParticle->AngularVelocity);

			pParticle->Roll    += angular.X;
			pParticle->Pitch   += angular.Y;
			pParticle->Heading += angular.Z;

			angular.SetValues(seconds_per_frame,pParticle->AngularAcceleration);
			// Do we Want To attenuateAngluar ie Make it accleate +-

			/* -----------------8/17/99 9:49AM---------------------------------------------------------------------
			/* this adjusts the angular velocity and also sets angular to be the correct sign and magnitude
			/* ----------------------------------------------------------------------------------------------------*/
			DecelerateOneAxis(pParticle->AngularVelocity.X,angular.X);
			DecelerateOneAxis(pParticle->AngularVelocity.Y,angular.Y);
			DecelerateOneAxis(pParticle->AngularVelocity.Z,angular.Z);

			angular *= 0.5*seconds_per_frame;

			pParticle->Roll    += angular.X;
			pParticle->Pitch   += angular.Y;
			pParticle->Heading += angular.Z;
		}


		if( pParticle->SType)
		{
			// Apply Bitmap physcs on Frame
			//pParticle->age += canister->dissipation_rate * seconds_per_frame;			//update age

			// Adjust Alpha
			if(	pParticle->Flag	& PF_ALHPA_RATE)
			{
				pParticle->Alpha = pParticle->AlphaRate * pParticle->Time + pParticle->StartAlpha;
				if (pParticle->Alpha > 1.0f)
					pParticle->Alpha = 1.0f;
				else
					if (pParticle->Alpha < 0.0f)
						pParticle->Alpha = 0.0f;
			}

			if( pParticle->Flag & PF_FIRE_ALPHA_RATE)
			{
				if( pParticle->Frame> (pParticle->SType->NumFrames-3))
					pParticle->Alpha = pParticle->FireAlpha * (pParticle->Time - pParticle->FireTimeToFirstAlpha) + pParticle->StartAlpha;
				else
					pParticle->Alpha = pParticle->StartAlpha;

				if (pParticle->Alpha > 1.0f)
					pParticle->Alpha = 1.0f;
				else
					if (pParticle->Alpha < 0.0f)
						pParticle->Alpha = 0.0f;
			}




			// Adjust Size
			if(	pParticle->Flag	& PF_SCALE_RATE)
				pParticle->Scale = pParticle->ScaleRate * pParticle->Time + pParticle->StartScale;

			// Adjust Frame			//add loopable and Terminate Hang
			if(	pParticle->Flag	& PF_FRAME_RATE)
			{
				pParticle->Frame += pParticle->FrameRate * seconds_per_frame;

				if( ((pParticle->SType == &SpriteTypes[SPT_OILFIRE]) || (pParticle->SType == &SpriteTypes[SPT_2D_OILFIRE])) && (pParticle->Frame >= (pParticle->SType->NumFrames-3)) )
				{
					if( pParticle->Type == PT_BURNOFF_SMOKE)
						RemoveParticle( pParticle);
					else
					{
						if( (pParticle->Type == PT_OIL_FIRE) || (pParticle->Type == PT_EXPLOSION_FIRE) || (pParticle->Type == PT_WELLHEAD_FIRE))
						{
							pParticle->pCanister->Flag ^= (pParticle->pCanister->Flag & CF_TOGGLE_TO_SMOKE)<<1;
							if (!(pParticle->pCanister->Flag & CF_TOGGLE_TO_SMOKE) || (pParticle->pCanister->Flag & CF_TOGGLE_THIS_ONE))
							{
								pParticle->Flag		|= PF_SPRITE;
								pParticle->Flag		&= ~PF_SCALE_RATE ;

								pParticle->FlirHeat			= pParticle->FlirHeat * (1.0 - (pParticle->Frame / pParticle->SType->NumFrames));
								pParticle->SType			= &SpriteTypes[SPT_BLACK_PUFF];
//								pParticle->PalNum			= 0;
								pParticle->Red				= 0.1f;
								pParticle->Green			= 0.1f;
								pParticle->Blue				= 0.1f;
								pParticle->Flag 			|= PF_CALC_LIGHTING;
								pParticle->gop &= ~PRIM_ALPHA_ADD;

								//F18 test for darker smoke
//								pParticle->StartAlpha = 1.0f;
//								pParticle->AlphaRate = 0.0f;//1.0f/pParticle->Time;

								pParticle->StartAlpha = 1.0f+frand();
								pParticle->AlphaRate = -pParticle->StartAlpha/(pParticle->TotalTime - pParticle->Time);
								pParticle->StartAlpha -= pParticle->AlphaRate * pParticle->Time;
								pParticle->Alpha	   = 1.0f;


								pParticle->Frame = frand() * (pParticle->SType->NumFrames-1);


								if (pParticle->Type != PT_EXPLOSION_FIRE)
								{
									if (pParticle->Type != PT_WELLHEAD_FIRE)
										pParticle->Velocity *= 4.0f/10.0f;
									else
										pParticle->Velocity *= 4.0f/20.0f;

									pParticle->Velocity.X = pParticle->Velocity.Y * (0.5 + frand()*0.5 - 0.25);
								}

								if( pParticle->Frame > pParticle->SType->NumFrames-1)
									pParticle->Frame = pParticle->SType->NumFrames -1;
							}
							else
								RemoveParticle(pParticle);

							//pParticle->Alpha			= 255;
						}
						else
							if( pParticle->Frame > pParticle->SType->NumFrames-1)
								pParticle->Frame = pParticle->SType->NumFrames -1;
					}
				}
				else
					if( pParticle->Frame > pParticle->SType->NumFrames-1)
						pParticle->Frame = pParticle->SType->NumFrames -1;
			}

		}
	}
}



//-------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
void DrawParticleDome(Particle *pParticle )
{

	FPoint Position;
	FPoint RelPosition;

	RelPosition =  pParticle->Position;
	RelPosition -= Camera1.CameraLocation;

//	if( RadiusInView( RelPosition, radius) )
//		DrawSpherePreBuilt( radius, hemi, RelPosition, alpha, diffuse, gop );

}

Sprite LensSprite;

void DrawParticle( Particle *pParticle, float alpha)
{
	float radius;
	float scale;
	FPoint RelPosition;
	float hemi = 1.0f;
	FPoint diffuse(0.0f);
	float uscale, vscale, uoffset, voffset;
	float rangle;
	int gop =  PRIM_NO_Z_WRITE;//|PRIM_ALPHA_ADD;//| PRIM_ALPHAIMM;////
	FMatrix *pMat = NULL;
	FPoint color;

	gop |=pParticle->gop;

	if( pParticle->SType)
	{
		CurrentTexture = pParticle->SType->Texture;

		if( pParticle->Flag | PF_ANGULAR_VELOCITY )
		{
			if (pParticle->Flag & PF_SPRITE)
				rangle = ANGLE_TO_RADIANS(pParticle->Pitch);
			else
			{
				pParticle->Attitude.SetHPR ( pParticle->Heading, pParticle->Pitch, pParticle->Roll);
				pMat = &(pParticle->Attitude);
			}
		}
		else
			rangle = 0.0f;


		pParticle->iFrame = (int)pParticle->Frame;

		RelPosition =  pParticle->Position;
		RelPosition -= Camera1.CameraLocation;
		pParticle->Rotated.RotateInto(RelPosition,ViewMatrix);

		//CalcFrameUV( &uscale, &uoffset, &vscale, &voffset);

		uscale	= vscale	= 1.0f;
		voffset = uoffset	= 0.0f;

		uoffset = pParticle->SType->ULU[pParticle->iFrame];
		voffset = pParticle->SType->ULV[pParticle->iFrame];

		uscale = pParticle->SType->UWidth;
		vscale = pParticle->SType->VHeight;


		SetUVScaleOffset( uscale, uoffset, vscale, voffset);


		scale = pParticle->Scale;

		if ((ImagingMethod & IT_FLIR) && ((pParticle->SType == &SpriteTypes[SPT_OILFIRE]) || (pParticle->SType == &SpriteTypes[SPT_2D_OILFIRE])))
		{
			LensSprite.Type		  	= &SpriteTypes[SPT_FLARE];
			LensSprite.Frame	  	= 3;
			LensSprite.Orientation 	= ORIENTATION_ROTATE_0;
			LensSprite.Rotated 		= pParticle->Rotated;
			LensSprite.Position	   	= pParticle->Position;

			if (scale = (pParticle->Scale * 3.0*(1.0 - (pParticle->Frame / SpriteTypes[SPT_OILFIRE].NumFrames))))
			{
				radius = pParticle->SType->RWWidth * scale ;// * 0.5f

			//	diffuse.SetValues( 1.0f, 0.95f, 0.60f);//1.0f);
				if( RadiusInView( RelPosition, radius) )
					if( pParticle->Flag & PF_SPRITE)
						RenderSprite( &LensSprite, 1.0f, 1.0f, 1.0f, 1.0f, scale, 0, gop,0,rangle);
					else
						DrawSpherePreBuilt( radius, hemi, RelPosition, alpha, diffuse, gop, pMat  );
			}
		}
		else
		{
			if (pParticle->Flag & PF_CALC_LIGHTING)
			{
				FPoint specular(0.0f);
				FPoint smoke_color;
				LightSource **light_walker = GlobalLights;

				if (pParticle->PalNum)
				{
					smoke_color.SetValues((float)CurrentWorldPalette[pParticle->PalNum], (float)CurrentWorldPalette[pParticle->PalNum+1] , (float)CurrentWorldPalette[pParticle->PalNum+2]);
					smoke_color *= 1.0f/255.0f;
				}
				else
					smoke_color.SetValues((float)pParticle->Red, (float)pParticle->Green, (float)pParticle->Blue);


				while(light_walker <= LastLight)
				{
					if (*light_walker)
						(*light_walker)->CalculateSaturation(pParticle->Position,diffuse,specular);
					light_walker++;
				}

				diffuse.Limit(1.0f);
				diffuse *= smoke_color;
				diffuse += specular;

				if (ImagingMethod & IT_FLIR)
					diffuse += pParticle->FlirHeat * (1.0 - (pParticle->Frame / pParticle->SType->NumFrames));
				diffuse.Limit(1.0f);


				scale = pParticle->Scale;
				radius = pParticle->SType->RWWidth * scale ;// * 0.5f

				if( RadiusInView( RelPosition, radius) )
					if( pParticle->Flag & PF_SPRITE)
					{
						if (gop & PRIM_ALPHA_ADD)
						{
							diffuse *= pParticle->Alpha;
							RenderSprite( (Sprite *)pParticle, diffuse.Red, diffuse.Green, diffuse.Blue, 1.0f, pParticle->Scale,0, gop,0,rangle);
						}
						else
					 		RenderSprite( (Sprite *)pParticle, diffuse.X,diffuse.Y,diffuse.Z, pParticle->Alpha, pParticle->Scale,0, gop,0,rangle);
					}
					else
						DrawSpherePreBuilt( radius, hemi, RelPosition, alpha, diffuse, gop, pMat  );

			}
			else
			{
				scale = pParticle->Scale;
				radius = pParticle->SType->RWWidth * scale ;// * 0.5f

				diffuse.SetValues(pParticle->Red, pParticle->Green, pParticle->Blue);

				if( RadiusInView( RelPosition, radius) )
					if( pParticle->Flag & PF_SPRITE)
					{
						if (gop & PRIM_ALPHA_ADD)
						{
							diffuse *= pParticle->Alpha;
							RenderSprite( (Sprite *)pParticle, diffuse.Red, diffuse.Green, diffuse.Blue, 1.0f, pParticle->Scale,0, gop,0,rangle);
						}
						else
							RenderSprite( (Sprite *)pParticle, pParticle->Red, pParticle->Green, pParticle->Blue, pParticle->Alpha, pParticle->Scale,0, gop,0,rangle);
					}
					else
						DrawSpherePreBuilt( radius, hemi, RelPosition, alpha, diffuse, gop, pMat );


			}
		}
		SetUVScaleOffset( 1.0f, 0.0f, 1.0f, 0.0f);	//reset


	}
	else
	{
		RelPosition =  pParticle->Position;
		RelPosition -= Camera1.CameraLocation;
		pParticle->Attitude.SetHPR ( pParticle->Heading, pParticle->Pitch, pParticle->Roll);
		Draw3DObjectMatrix( pParticle->Model, RelPosition, pParticle->Attitude, NULL );
	}


}



void DisplayCanister(CameraInstance *camera, Canister *pCanister)
{
	Particle *pParticle,*pLastParticle;
	float segs, alpha;
	float dist;
	FPoint RelPosition;
	BOOL sphere_built = FALSE;

	RelPosition = pCanister->Position;
	RelPosition -=  camera->CameraLocation;
	dist = RelPosition.QuickLength();

	CalcSegsAlpha( dist, &segs, &alpha);
	if( segs)
	{
		if( pCanister->Segments)
			segs = pCanister->Segments;
		else
			segs = 7; //we need a frame  rate here.

				// Build 1 sphere for entire cloud group


		// render canister using built sphere
		pParticle = &pCanister->Particles[0];
		pLastParticle = pCanister->pLastParticle;
		while( pParticle <= pLastParticle )
		{
			if( (pParticle->Type)  )///&& (pParticle->Priority > prio))
			{
				if (!sphere_built)
				{
					BuildSphere(  segs );
					sphere_built = TRUE;
				}
				DrawParticle( pParticle, pParticle->Alpha * alpha );
			}
			pParticle++;
		}
	}

}


void DisplayCanisters( CameraInstance *camera)
{
	Canister *pCanister;
	//Particle *pParticle,*pLastParticle;
	int prio;
	double distance;
	double dmulter;
	FPoint windchange;
	FPoint wind;

	dmulter = EyeToScreen / -512.0f;
	dmulter *= dmulter;


	windchange.SetValues(0.0f);
	if(frand() < 0.20f)// change randomly
		windchange.SetValues((float) (2.0f METERS) * ( frand()- 0.5f), 0.0f, (float)(2.0f METERS) *(frand()- 0.5f) );

	wind = wind_vector;
	//wind_vector += windchange;
	//else
	//	wind_vector.SetValues((float)(4.0f METERS) *frand(), 0.0f, (float) (8.0f METERS) * frand());



	pCanister = &Canisters[0];
	while( pCanister <= pLastCanister)
	{
		if( (pCanister->Type) && !(pCanister->Flag & CF_DELAY_START))
		{
			if (pCanister->NumParticles > 4)
			{
				distance = (pCanister->Position - Camera1.CameraLocation) / dmulter;

				if (distance < 12000.0)
				{
					if (distance < 7000.0)
					{
						prio = (int)(floor(sqrt(distance)/14.0))-2;

						if (_3dxl == _3DXL_SOFTWARE)
						{
							if (prio < 1)
								prio = 1;
							else
								prio++;
						}

						if (prio > 4)
							prio = 4;
					}
					else
					{
						if (((distance-7000.0) / 1000.0) >= pCanister->CanPriority)
						{
							pCanister++;
							continue;
						}
						else
							prio=4;
					}
				}
				else
				{
					pCanister++;
					continue;
				}
			}
			else
				if (_3dxl == _3DXL_SOFTWARE)
					prio = 1;
				else
					prio = -1;

			if (prio < 5)
			{
				/*
				InsertCanister(camera, pCanister);

				pParticle = &pCanister->Particles[0];
				pLastParticle = pCanister->pLastParticle;
				while( pParticle <= pLastParticle )
				{
					if( (pParticle->Type) && (pParticle->Priority > prio))
						InsertParticle( camera, pParticle );
					pParticle++;
				}
				*/
				DisplayCanister( camera, pCanister);

			}

		}
		pCanister++;
	}

}



/*
void InsertCanisters( CameraInstance *camera)
{
	Canister *pCanister;
	Particle *pParticle,*pLastParticle;
	int prio;
	double distance;
	double dmulter;

	dmulter = EyeToScreen / -512.0f;
	dmulter *= dmulter;

	pCanister = &Canisters[0];
	while( pCanister <= pLastCanister)
	{
		if( pCanister->Type)
		{
			if (pCanister->NumParticles > 4)
			{
				distance = (pCanister->Position - Camera1.CameraLocation) / dmulter;

				if (distance < 12000.0)
				{
					if (distance < 7000.0)
					{
						prio = (int)(floor(sqrt(distance)/14.0))-2;

						if (_3dxl == _3DXL_SOFTWARE)
						{
							if (prio < 1)
								prio = 1;
							else
								prio++;
						}

						if (prio > 4)
							prio = 4;
					}
					else
					{
						if (((distance-7000.0) / 1000.0) >= pCanister->CanPriority)
						{
							pCanister++;
							continue;
						}
						else
							prio=4;
					}
				}
				else
				{
					pCanister++;
					continue;
				}
			}
			else
				if (_3dxl == _3DXL_SOFTWARE)
					prio = 1;
				else
					prio = -1;

			if (prio < 5)
			{
				InsertCanister(camera, pCanister);

				pParticle = &pCanister->Particles[0];
				pLastParticle = pCanister->pLastParticle;
				while( pParticle <= pLastParticle )
				{
					if( (pParticle->Type) && (pParticle->Priority > prio))
						InsertParticle( camera, pParticle );
					pParticle++;
				}
			}

		}
		pCanister++;
	}

}


void InsertParticle(CameraInstance *camera, Particle *pParticle )
{
	if (!pParticle->SortHeader)
		GetSortHeader( pParticle, PARTICLE_OBJECT);

	if( pParticle->SortHeader )
	{
		pParticle->SortHeader->RelativePosition.MakeVectorToFrom( pParticle->Position, camera->CameraLocation );
		//pParticle->SortHeader->Object = pParticle;
		if( pParticle->SType)
			InsertObject(pParticle->SType->RWWidth*pParticle->Scale, pParticle->SortHeader,FALSE); // store and pass along
		else
			InsertObject(GetObjectRadius(pParticle->Model), pParticle->SortHeader,FALSE);
	}
}
*/



// Insert Particles Into The World Sorter
// ?BAsically we are just checking particle.Z versus ZClipplane.
/*
void InsertParticles(CameraInstance *camera )
{
	int i;
	Particle *pParticle;

	pParticle = &Particles[0];
	for( i=0; i< MAX_PARTICLES; i++)	//NUMPARTCILES		// need a linked list here
	{
		if( pParticle->Type )
		{
			if (!pParticle->SortHeader)
				GetSortHeader( pParticle, PARTICLE);
			if( pParticle->SortHeader )
			{
				FPoint rel;
				rel.MakeVectorToFrom( pParticle->Position, camera->CameraLocation );
				pParticle->Rotated.Z = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R2C0);
				if (pParticle->Rotated.Z < ZClipPlane)
				{
					pParticle->Rotated.X = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R0C0);
					pParticle->Rotated.Y = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R1C0);
					pParticle->SortHeader->RelativePosition = rel;
					pParticle->SortHeader->Object = pParticle;
					InsertObject(GetObjectRadius(pParticle->Model), pParticle->SortHeader,FALSE);
				}
			}
		}
		pParticle++;
	}

}
*/



/*
// Called From Drawer Vector in  Render.cpp
void DrawParticle( Particle *pParticle)
{
	float scale;
	//Wrapper Rendre function for a Sprite /Object


	if( pParticle->SType)
	{
		pParticle->iFrame = (int)pParticle->Frame;

		pParticle->Rotated.RotateInto(pParticle->SortHeader->RelativePosition,ViewMatrix);

		scale = pParticle->Scale;

//		if ((pParticle->Type == PT_FLASH) && (_3dxl == _3DXL_SOFTWARE))
//			scale *= 0.5;

		if ((ImagingMethod & IT_FLIR) && (pParticle->SType == &SpriteTypes[SPT_OILFIRE]))
		{
			LensSprite.Type		  	= &SpriteTypes[SPT_FLARE];
			LensSprite.Frame	  	= 3;
			LensSprite.Orientation 	= ORIENTATION_ROTATE_0;
			LensSprite.Rotated 		= pParticle->Rotated;
			LensSprite.Position	   	= pParticle->Position;

			if (scale = (pParticle->Scale * 3.0*(1.0 - (pParticle->Frame / SpriteTypes[SPT_OILFIRE].NumFrames))))
				RenderSprite( &LensSprite, 1.0f, 1.0f, 1.0f, 1.0f, scale);
		}
		else
		{
			if (pParticle->Flag & PF_CALC_LIGHTING)
			{
				FPoint diffuse(0.0f);
				FPoint specular(0.0f);
				FPoint smoke_color;
				LightSource **light_walker = GlobalLights;

				if (pParticle->PalNum)
				{
					smoke_color.SetValues((float)CurrentWorldPalette[pParticle->PalNum], (float)CurrentWorldPalette[pParticle->PalNum+1] , (float)CurrentWorldPalette[pParticle->PalNum+2]);
					smoke_color *= 1.0f/255.0f;
				}
				else
					smoke_color.SetValues((float)pParticle->Red, (float)pParticle->Green, (float)pParticle->Blue);


				while(light_walker <= LastLight)
				{
					if (*light_walker)
						(*light_walker)->CalculateSaturation(pParticle->Position,diffuse,specular);
					light_walker++;
				}

				diffuse.Limit(1.0f);
				diffuse *= smoke_color;
				diffuse += specular;

				if (ImagingMethod & IT_FLIR)
					diffuse += pParticle->FlirHeat * (1.0 - (pParticle->Frame / pParticle->SType->NumFrames));
				diffuse.Limit(1.0f);

				RenderSprite( (Sprite *)pParticle, diffuse.X,diffuse.Y,diffuse.Z, pParticle->Alpha, pParticle->Scale);
			}
			else
				RenderSprite( (Sprite *)pParticle, pParticle->Red, pParticle->Green, pParticle->Blue, pParticle->Alpha, pParticle->Scale);
		}


	}
	else
	{
		pParticle->Attitude.SetHPR ( pParticle->Heading, pParticle->Pitch, pParticle->Roll);
		Draw3DObjectMatrix( pParticle->Model, pParticle->SortHeader->RelativePosition, pParticle->Attitude, NULL );
	}


}


*/











//--------------------------------------------------------------------------------
BOOL GroundCollision( float x, float y, float z)
{
	float height;

	height = LandHeight( x, z ) + 2 METERS;
	if (y < height)
		return TRUE;
	else
		return FALSE;
}

void AttenuateVelocity( FPoint *pVel, float att)
{
	// pre-multiply with time first
	att = att * seconds_per_frame;

	// get velocity for next frame
	pVel->X -= att * pVel->X;
	pVel->Y -= att * pVel->Y;
	pVel->Z -= att * pVel->Z;

	// add gravity
	pVel->Y -= seconds_per_frame * 9.8 METERS;

}

void AttenuateVelocityNoG( FPoint *pVel, float att)
{
	// pre-multiply with time first
	att = att * seconds_per_frame;

	// get velocity for next frame
	pVel->X -= att * pVel->X;
	pVel->Y -= att * pVel->Y;
	pVel->Z -= att * pVel->Z;
}




///-------------------------------------------------------------------------------------------------------------------
///---- Misc Effects

Sprite SunSprite;
Sprite SkySprite;

int SunSX, SunSY,MoonSX,MoonSY;
extern BYTE SubPartLevel;

void RenderCounterMeasureFlare(RotPoint3D *rotpt ,FPoint &color)
{
	float scale;
//	float rndom;
	DWORD irand;

	irand = rand();

	LensSprite.Frame	  = 0;
	LensSprite.Type		  = &SpriteTypes[SPT_FLASH];
	LensSprite.SortHeader = NULL;

	if (_3dxl)
	{
//		if (!(irand & 0xF0))
//			LensSprite.Orientation = irand&0xF;
//		else
			LensSprite.Orientation = ORIENTATION_ROTATE_0;

	}
	else
	{
//		if (!(irand & 0x30))
//			LensSprite.Orientation = rand()&0xF;
//		else
			LensSprite.Orientation = ORIENTATION_ROTATE_0;
	}


	LensSprite.Rotated.Z = rotpt->Rotated.Z;
	if (LensSprite.Rotated.Z < ZClipPlane)
	{
		LensSprite.Rotated.X = rotpt->Rotated.X;
		LensSprite.Rotated.Y = rotpt->Rotated.Y;

		scale = -LensSprite.Rotated.Z * (1.0f / (float)(150.0f FEET));


		if (scale > 0.0f)
		{
//			rndom = frand();
//			if (rndom > 0.5)
//				rndom = 1.0;
//			else
//				rndom = 0.9+0.2*rndom;

			if (ImagingMethod & IT_FLIR)
			{
 				if (scale > 0.40f)
					scale = 0.40f;
			}
			else
			{
 				if (scale > 0.10f)
					scale = 0.10f;
			}

//			scale *= rndom;
			RenderSprite( &LensSprite, color.X,color.Y, color.Z, 1.0f, scale,0,0,TRUE);
		}
	}
}

void RenderCameraFlareWithSize( RotPoint3D *rotpt ,FPoint &c,float alpha,float size,BOOL limit_size)
{
	float scale;
//	float rndom;
	FPoint color = c;

	LensSprite.Frame	  = 0;
	LensSprite.Type		  = &SpriteTypes[SPT_FLASH];
	LensSprite.SortHeader = NULL;

	if (_3dxl)
	{
//		if (!(irand & 0xF0))
//			LensSprite.Orientation = irand&0xF;
//		else
			LensSprite.Orientation = ORIENTATION_ROTATE_0;

	}
	else
	{
//		if (!(irand & 0x30))
//			LensSprite.Orientation = rand()&0xF;
//		else
			LensSprite.Orientation = ORIENTATION_ROTATE_0;
	}


	LensSprite.Rotated.Z = rotpt->Rotated.Z;
	if (LensSprite.Rotated.Z < ZClipPlane)
	{
		LensSprite.Rotated.X = rotpt->Rotated.X;
		LensSprite.Rotated.Y = rotpt->Rotated.Y;

		if (size == 0.0)
			scale = -LensSprite.Rotated.Z * (1.0f / (float)(150.0f FEET));
		else
			scale = size;

		if (alpha == 0.0f)
			alpha = 1.0f;


		if (scale > 0.0f)
		{
//			rndom = frand();
//			if (rndom > 0.5)
//				rndom = 1.0;
//			else
//				rndom = 0.9+0.2*rndom;

			if (limit_size)
				if (ImagingMethod & IT_FLIR)
				{
 					if (scale > 0.40f)
						scale = 0.40f;
				}
				else
				{
 					if (scale > 0.10f)
						scale = 0.10f;
				}

//			scale *= rndom;
			color *= alpha;
			RenderSprite( &LensSprite, color.X ,color.Y,color.Z, 1.0f, scale,0,0,TRUE);
		}
	}

}


void RenderCameraFlare( RotPoint3D *rotpt ,FPoint &c,float alpha,float size)
{
	RenderCameraFlareWithSize (rotpt,c,alpha,size,TRUE);
}

float small_sun_scale = 0.45f;
float small_sun_bright = 0.6f;

float moon_scale = 0.45f;
float moon_bright = 0.6f;

float big_sun_scale = 0.8f;
float big_sun_bright = 0.4f;

BOOL SunOnScreen;
BOOL MoonOnScreen;


void RenderSun( int which )
{
	float bright;

	if ((ImagingMethod & IT_FLIR) && (which == SUN_CORONA))
		return;

	FPoint rel;
	if (which == SUN_MOON)
		rel.SetValues(-1.0f,PositionToDrawMoon);
	else
		rel.SetValues(-1.0f,PositionToDrawSun);
	FPoint color;

	rel*=-2000 FEET;

	RotPoint3D center[1];
	RotPoint3D *walker;

	SunSprite.Frame		  = 0;

	SunSprite.SortHeader  = NULL;
	SunSprite.Position    = rel;
	SunSprite.Orientation = ORIENTATION_ROTATE_0;

	SunSprite.Rotated.Z = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R2C0);
	if (SunSprite.Rotated.Z < ZClipPlane)
	{
		SunSprite.Rotated.X = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R0C0);
		SunSprite.Rotated.Y = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R1C0);
		SunSprite.Fade = 0;

		if (ImagingMethod & IT_FLIR)
		{
			SunSprite.Type		  = &SpriteTypes[SPT_SUN];
			color = SunIntensity;
			RenderSprite( &SunSprite, color.X, color.Y , color.Z, (float)0x90/255.0f, 0.6f);

			center[0].Rotated = SunSprite.Rotated;
			walker = &center[0];
			walker->Flags = PT3_ROTATED;
			PerspectPoint(walker);
			if (!(walker->Flags & PT3_NODRAW))
			{
				switch(which)
				{
					case SUN_SUN:
						SunOnScreen = TRUE;
						SunSX = walker->iSX;
						SunSY = walker->iSY;
						break;
					case SUN_MOON:
						MoonOnScreen = TRUE;
						MoonSX = walker->iSX;
						MoonSY = walker->iSY;
						break;
				}
			}

		}
		else
		{
			switch(which)
			{
				case SUN_SUN:
					SunSprite.Type		  = &SpriteTypes[SPT_SUN];
					color = SunIntensity * small_sun_bright;
					RenderSprite( &SunSprite, color.X, color.Y , color.Z, 1.0f, small_sun_scale);

					center[0].Rotated = SunSprite.Rotated;
					walker = &center[0];
					walker->Flags = PT3_ROTATED;
					PerspectPoint(walker);
					if (!(walker->Flags & PT3_NODRAW))
					{
						SunOnScreen = TRUE;
						SunSX = walker->iSX;
						SunSY = walker->iSY;
					}
					break;

				case SUN_MOON:
					SunSprite.Type		  = &SpriteTypes[SPT_MOON];
					color = MoonIntensity * moon_bright;
					RenderSprite( &SunSprite, color.X, color.Y , color.Z, 1.0f, moon_scale);

					center[0].Rotated = SunSprite.Rotated;
					walker = &center[0];
					walker->Flags = PT3_ROTATED;
					PerspectPoint(walker);
					if (!(walker->Flags & PT3_NODRAW))
					{
						MoonOnScreen = TRUE;
						MoonSX = walker->iSX;
						MoonSY = walker->iSY;
					}
					break;


				case SUN_CORONA:
					bright = 1.0f - SunIntensity;
					color = (1.0f - (bright*bright))*big_sun_bright;
					SunSprite.Type = &SpriteTypes[SPT_SUN_CORONA];
					RenderSprite( &SunSprite, color.X, color.Y , color.Z, 1.0f, big_sun_scale);
					break;

			}
		}
	}
}


typedef struct _SunFlare
{
	int Frame;
	float Scale;
	float Distance;
	float r;
	float g;
	float b;
	float a;
}SunFlare;


#define NUM_SUN_FLARES  9
SunFlare SunFlares[NUM_SUN_FLARES] =
{//  F   Scale   Dist	R     G     B     A
//	{4, 0.35f,  0.10f,   255.0f/255.0f   6.0f/255.0f    6.0f/255.0f 0x20},
	{2, 0.35f,  0.10f,   255.0f/255.0f,   6.0f/255.0f,    6.0f/255.0f, (float)0x10/255.0f},
	{2, 0.55f,  0.20f,   48.0f/255.0f,   48.0f/255.0f,   66.0f/255.0f, (float)0x20/255.0f},
//	{5, 0.55f,  0.45f,  125.0f/255.0f,  214.0f/255.0f,  169.0f/255.0f, (float)0x20/255.0f},
	{3, 0.55f,  0.45f,  125.0f/255.0f,  214.0f/255.0f,  169.0f/255.0f, (float)0x20/255.0f},
	{2, 0.35f,  0.55f,   10.0f/255.0f,  251.0f/255.0f,   61.0f/255.0f, (float)0x20/255.0f},

//	{4, 0.35f,  0.65f,  128.0f/255.0f,  214.0f/255.0f,  169.0f/255.0f, (float)0x20/255.0f},
	{2, 0.35f,  0.65f,  128.0f/255.0f,  214.0f/255.0f,  169.0f/255.0f, (float)0x10/255.0f},
	{1, 1.20f,  0.85f,  250.0f/255.0f,   17.0f/255.0f,  169.0f/255.0f, (float)0x20/255.0f},
//	{4, 2.00f,  0.90f,  147.0f/255.0f,   66.0f/255.0f,   60.0f/255.0f, (float)0x20/255.0f},
	{2, 2.00f,  0.90f,  147.0f/255.0f,   66.0f/255.0f,   60.0f/255.0f, (float)0x10/255.0f},
//	{4, 0.60f,  0.95f,   10.0f/255.0f,   61.0f/255.0f,   61.0f/255.0f, (float)0x20/255.0f},
	{3, 0.60f,  0.95f,   10.0f/255.0f,   61.0f/255.0f,   61.0f/255.0f, (float)0x10/255.0f},
	{0, 2.40f,  1.00f,  128.0f/255.0f,  214.0f/255.0f,  169.0f/255.0f, (float)0x20/255.0f},

};


void RenderSunLensFlare( void)//int SunSX, int SunSY )
{
	int x1, y1,x2, y2;
	int screenwidth  = ScreenSize.cx;
	int screenheight = ScreenSize.cy;
	int MidX = (screenwidth>>1);
	int MidY = (screenheight>>1);
	float alpha;
	float scale;
	int px, py;
	float pd;
	SunFlare *pFlare;


	if( CurrentView & COCKPIT_VIRTUAL_SEAT)			// no flare in virutal cockpit
		return;


	/* ------------------------------------------3/8/98 4:43PM---------------------------------------------
	 * hopefully this should keep the lens flare from showing up in the missile cameras
	 * ----------------------------------------------------------------------------------------------------*/
	if((ImagingMethod & IT_FLIR) || (!SunIntensity) )//  This fix doesnt work anymore|| (fabs(EyeToScreen+512.0f) > 0.001))
		return;


	if(SunOnScreen && SunSX>0 && SunSX< screenwidth && SunSY>0 && SunSY<screenheight)
	{
	//	if (!(Camera1.Flags & CF_SUN_VISIBLE))
	//		return;

		x1 = SunSX;
		y1 = SunSY;

		// Adjust endpoint func
		float dsx = x1 - MidX;
		float dsy = y1 - MidY;

		float endptscale = 1.0f - sqrt(dsx * dsx + dsy * dsy)/ sqrt( (float)(MidX*MidX + MidY*MidY) );
		endptscale *= endptscale;
		endptscale -= 1.0f;

		x2 = dsx*endptscale + MidX;
		y2 = dsy*endptscale + MidY;

		// Linear endpt
		//x2 *= -endptscale;
		//y2 *= -endptscale;	// Linear Motion Of endpoint

		float dx = abs(x2 - x1);
		float dy = abs(y2 - y1);

		float dis =  QuickDistance( dx, dy);

		float MaxDis = MidX;//320.0f;
		float alpha_mult = 1.0f/(float)MidX;//320.0f;//64;

		if( dis >= MaxDis)
			alpha =0;
		else
			alpha = (MaxDis - dis) * alpha_mult;

		// adjust alpha for diatance from Horizon
		//float Sv = WorldParams.SunY;
		//if( Sv > -0.20f && Sv< 0.05f)
		//	alpha *= (Sv-0.05f)/(-0.25f);  //78.6 deg


		LensSprite.Type			= &SpriteTypes[SPT_FLARE];
		LensSprite.SortHeader	= NULL;
		LensSprite.Frame		= 3;
		LensSprite.Orientation	= ORIENTATION_ROTATE_0;

		pFlare = &SunFlares[0];
		for(int i=0; i<NUM_SUN_FLARES; i++)
		{
			scale = pFlare->Scale;
			pd = pFlare->Distance;
			LensSprite.Frame = pFlare->Frame;

			px = (1.0f - pd)* x1 + pd * x2;
			py = (1.0f - pd)* y1 + pd * y2;

			alpha = alpha *pFlare->a*10.0f;
			if( alpha > 1.0f)
				alpha = 1.0f;
			alpha *= SunIntensity;
			Render2DSpriteSX( &LensSprite, pFlare->r, pFlare->g, pFlare->b, alpha, scale, px, py);

			pFlare++;
		}
	}
//	SunSX = SunSY = -1;

}


void Render2DSpriteSX( Sprite *spt, float r, float g, float b, float a, float scale, int sx, int sy)
{
	spt->Rotated.X = sx - GrBuffFor3D->MidX;
	spt->Rotated.Y = GrBuffFor3D->MidY - sy ;
	spt->Rotated.Z = EyeToScreen;//-512;//ZNearClipPlane

	spt->Rotated *= (ZClipPlane/EyeToScreen);		// scale it right to the front clip plane

	scale *= (ZClipPlane/EyeToScreen);


	RenderSprite( spt, r*a, g*a, b*a, 1.0f, scale, PRIM_ALPHA,PRIM_NO_Z_WRITE | PRIM_ALPHAIMM| PRIM_ALPHA_ADD);
}


void RenderSprite( Sprite *spt, float r, float g, float b, float a, float ns,DWORD unwanted_flags,DWORD addnl_poly_flags,BOOL rotate_flare,float rangle)
{
	RotPoint3D Points[4];
	RotPoint3D *walker;
	int clip_flags,draw_flags;
	float w;
	SpriteType *type;
	Sprite *sprite;
//	unsigned int color;
//	float alpha;
	FPoint diffuse;
	float ULU,ULV,LRU,LRV;
	FPoint relp;
	float scale;

	sprite = spt;
	type = sprite->Type;

	if (ImagingMethod & IT_GREEN)
	{
		relp.SetValues(0.8f,sprite->Rotated);
		scale =ns*0.8f;
		PushBackFog = TRUE;

	}
	else
	{
		relp = sprite->Rotated;
		scale = ns;
	}


	w = type->RWWidth * scale ;

	if (rotate_flare)
		rangle += atan2(relp.Y,relp.X);

	if (rotate_flare || (rangle != 0.0f))
	{
		float sa,ca;
		FPoint new_xy;

		w*=0.5;

		addnl_poly_flags |= PRIM_CLAMP_UV;

		Points[0].Rotated.SetValues(-w,-w,0.0f);
		Points[1].Rotated.SetValues(w,-w,0.0f);
		Points[2].Rotated.SetValues(w,w,0.0f);
		Points[3].Rotated.SetValues(-w,w,0.0f);

		sa = sin(rangle);
		ca = cos(rangle);

		int i = 4;
		walker = &Points[0];
		new_xy.Z = 0.0f;
		while(i--)
		{
			new_xy.X = -(walker->Rotated.X*ca+walker->Rotated.Y*sa);
			new_xy.Y = -(walker->Rotated.Y*ca-walker->Rotated.X*sa);
			walker->Rotated.MakeVectorToFrom(relp,new_xy);
			walker++;
		}

	}
	else
	{
		Points[0].Rotated = relp;
		Points[1].Rotated = relp;
		Points[2].Rotated = relp;
		Points[3].Rotated = relp;

		Points[0].Rotated.Y += w ;

		Points[1].Rotated.X += w;

		Points[2].Rotated.Y -= w;

		Points[3].Rotated.X -= w;
	}

	walker = &Points[0];
	int i = 4;
	while(i--)
	{
		walker->Flags = PT3_ROTATED;
		PerspectPoint(walker++);
	}

	draw_flags = Points[0].Flags & Points[1].Flags & Points[2].Flags & Points[3].Flags;

	if (!(draw_flags & PT3_NODRAW))
	{
		RotPoint3D *points[4];
		diffuse.SetValues(r,g,b);

		ULU = type->ULU[sprite->Frame];
		ULV = type->ULV[sprite->Frame];

		LRU = ULU+type->UWidth;
		LRV = ULV+type->VHeight;

		if (sprite->Orientation & ORIENTATION_FLIP_U)
			Swap4(&ULU,&LRU);

		if (sprite->Orientation & ORIENTATION_FLIP_V)
			Swap4(&ULV,&LRV);

		switch(sprite->Orientation & ORIENTATION_ROTATION)
		{
			case ORIENTATION_ROTATE_0:
				Points[0].U = ULU;
				Points[0].V = ULV;
				Points[2].U = LRU;
				Points[2].V = LRV;
				break;

			case ORIENTATION_ROTATE_90:
				Points[0].U = ULU;
				Points[0].V = LRV;
				Points[2].U = LRU;
				Points[2].V = ULV;
				break;

			case ORIENTATION_ROTATE_180:
				Points[0].U = LRU;
				Points[0].V = LRV;
				Points[2].U = ULU;
				Points[2].V = ULV;
				break;

			case ORIENTATION_ROTATE_270:
				Points[0].U = LRU;
				Points[0].V = ULV;
				Points[2].U = ULU;
				Points[2].V = LRV;
				break;
		}

		Points[1].U = Points[2].U;
		Points[1].V = Points[0].V;
		Points[3].U = Points[0].U;
		Points[3].V = Points[2].V;


		Points[0].Diffuse = Points[1].Diffuse = Points[2].Diffuse = Points[3].Diffuse = diffuse;
		Points[0].Specular = Points[1].Specular = Points[2].Specular = Points[3].Specular.SetValues(0.0f);

		CurrentTexture = sprite->Type->Texture;

		clip_flags = Points[0].Flags | Points[1].Flags | Points[2].Flags | Points[3].Flags;

		points[0] = &Points[0];
		points[1] = &Points[1];
		points[2] = &Points[2];
		points[3] = &Points[3];

		points[0]->Alpha = points[1]->Alpha = points[2]->Alpha = points[3]->Alpha = a;//color;
		//PRIM_TEXTURE | PRIM_ALPHAIMM|
		//DrawPolyClipped((sprite->Type->PolyType & ~unwanted_flags) | addnl_poly_flags| PRIM_NO_Z_WRITE ,CL_CLIP_UV,4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);

		DrawPolyClipped((sprite->Type->PolyType & ~unwanted_flags) | addnl_poly_flags | PRIM_ZFUNC_LE,CL_CLIP_UV,4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);
	}

	PushBackFog = FALSE;

}

void InitDiffusionTable( void )
{
	int i,j;
	float diffusion_speed;

	for(i=0; i<3; i++)
	{
		diffusion_speed = 0.2f + i * 0.2f;
		for(j=0; j<32; j++)
		{
			random_diffusion[i][j].X = (((rand()&4095)/2048)-0.5f) * diffusion_speed;
			random_diffusion[i][j].Y = (((rand()&4095)/2048)-0.5f) * diffusion_speed;
			random_diffusion[i][j].Z = (((rand()&4095)/2048)-0.5f) * diffusion_speed;
		}
	}

}



void LoadDestroyedPlaneParts( void)
{
	PlanePartTypes[0].Model = Load3DObject(RegPath("objects","bup01.3dg"));
	PlanePartTypes[1].Model = Load3DObject(RegPath("objects","bup02.3dg"));
	PlanePartTypes[2].Model = Load3DObject(RegPath("objects","bup03.3dg"));
}

void FreePlanePartTypes( void)
{
	int i;

	for( i=0; i< NUM_PLANE_PART_MODELS; i++)
	{
		Free3DObject(PlanePartTypes[i].Model);
		PlanePartTypes[i].Model = NULL;
	}
}

void PlaneHitGround( FPointDouble &pos, FPointDouble &vel, float fuel,FPoint *vector,void *vehicle)
{
	int numspines = 6;
	FPointDouble position = pos;
	FMatrix lmatrix;
	FMatrix matrix;
	FPoint lnormal;
	FPoint pnormal;
	FPoint tmp(0.0f);
	float radius;
	double fdot;
	double flength;

	if (InHighResTerrainRegion(position))
	{
		numspines = 6 + g_Settings.gr.nExplosionDetail*3;

		if( (!vector) && InWater(position.X, position.Z) )
		{
 			position.Y += 3 METERS;				// move explosion up a little
			heightscale = 2.5;
			for( int i = 0; i< numspines; i++)
				NewCanister( CT_EXPLOSION_WATER , position, tmp, 200.0);
			PlayPositionalExplosion(WARHEAD_STANDARD_MEDIUM,Camera1,position);
		}
		else
		{
			if (!vector)
			{
				position.Y = LandHeight(position.X,position.Z);
				LandNormal(position,lnormal);
				GetMatrix(position,0,matrix);
				AddCrater(position,75);
			}
			else
				lnormal = *vector;

			position.Y += 3 METERS;				// move explosion up a little
			flength = vel.Length();
			if (flength == 0.0)
				flength = 1.0;
			else
				flength = 1.0/flength;

			radius = 200.0;

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
			for( int i = 0; i< numspines; i++)
				NewCanister( CT_EXPLOSION_FIRE , position, tmp, 200.0f, 0.0f, &matrix);

			NewCanister(CT_FLASH,position,tmp,1.0f);

			if (vehicle)
				GetVehicleFireHandle(position,(void *)vehicle,CT_OIL_FIRE, 2.0f , 5.0f*60.0f );
			else
				NewCanister( CT_OIL_FIRE, position, tmp,2.0f , 5.0f*60.0f );

			PlayPositionalExplosion(WARHEAD_STANDARD_MEDIUM,Camera1,position);

		}

	}

}