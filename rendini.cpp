
#include "F18.h"
#include "particle.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "clouds.h"

void LoadDestroyedPlaneParts( void);
void FreePlanePartTypes( void);
BOOL ReadPalette(unsigned char *pal1,int table_number1,unsigned char *pal2 = NULL,int table_number2 = 0);
void GreenifyPalette(unsigned char *greener);
void SetF18HardwareD3D( void);
void InitLandTexturePals(void);
void LoadCloudTexture( void);
void ReleaseShadowEntry(void *pt);
extern double max_roll; // max of 6.38/2.0; these are for ships rolling etc.
extern double max_pitch; // max of 1.8/2.0;

extern int g_nMissionType;
extern TerCache *MovingMapMediumCache;
extern TerCache *MovingMapHighCache;

extern BOOL pigs;

extern char *SpecialTexturePath;

char tpath[_MAX_PATH];

char *get_texture_path(char *filename,int tries)
{
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(filename,NULL,NULL,fname,ext);
	_makepath(tpath,"g:","\\nellis\\objects\\",fname,ext);

	return tpath;
}

int tick_pauser;

void PauseForDiskHit(void)
{
	tick_pauser = GetTickCount() - CurrentTick;
}

void UnPauseFromDiskHit(void)
{
	if (MultiPlayer)  return;

	CurrentTick = GetTickCount()-tick_pauser;
}

void FixGameTextures()
{
	float *uwalk,*vwalk;
	SpriteType *sprite;
	float current_v;
	float current_u;
	int i;

	sprite = &SpriteTypes[0];
	while(sprite < &SpriteTypes[MAX_SPRITE_TYPES])
	{
		if (sprite->Texture)
		{
			sprite->UWidth /= sprite->Texture->Width;
			sprite->VHeight /= sprite->Texture->Height;

			current_u = 0.5f/sprite->Texture->Width;
			current_v = 0.5f/sprite->Texture->Height;
			uwalk = &sprite->ULU[0];
			vwalk = &sprite->ULV[0];

			i = sprite->NumFrames;
			while(i--)
			{
				*uwalk++ = current_u;
				*vwalk++ = current_v;
				current_u += sprite->UWidth;
				if (current_u >= sprite->Texture->WidthMulter)
				{
					current_u = 0.5f/sprite->Texture->Width;
					current_v += sprite->VHeight;
				}
			}
			sprite->UWidth  -= 1.0f/sprite->Texture->Width;
			sprite->VHeight -= 1.0f/sprite->Texture->Width;

		}
		sprite++;
	}

}

void InitWeather()
{
	LoadCloudTexture();
	LoadSkyPalettes((WorldParams.Weather & WR_FLATCLOUD) >> 4);

	InitClouds();
	InitRain();


	if (WorldParams.Weather & WR_FLATCLOUD)
	{
		max_roll = 0.25 * 2* ((WorldParams.Weather & WR_FLATCLOUD) >> 4); // max of 6.38/2.0; these are for ships rolling etc.
		max_pitch = 0.65 * ((WorldParams.Weather & WR_FLATCLOUD) >> 4); // max of 1.8/2.0;
	}


}

DWORD BombVars[] = {0,0x10000,0x20000};
SubObjectHandler SourceSubs[MAX_SUBPARTS];
BYTE WhichBombVars[MAX_SUBPARTS] = {0,  //	0,    //"GEAR NOSE",
									0,	 //	0,	//"GEAR REAR LEFT",
									0,	 //	0,	//"GEAR REAR RIGHT",
									0,	 //	0,	//"GEAR TAIL",
									0,	 //	0,	//"BEEF",
									0,	 //	0,	//"BEEF",
									0,	 //	0,	//"BEEF",
									0,	 //	0,	//"BEEF",
									0,	 //	0,	//"BOMB WING RIGHT OUTBOARD",
									0,	 //	0,	//"BOMB WING RIGHT MIDDLE",
									0,	 //	0,	//"BOMB WING RIGHT INBOARD",
									0,	 //	0,	//"BOMB WING LEFT OUTBOARD",
									0,	 //	0,	//"BOMB WING LEFT MIDDLE",
									0,	 //	0,	//"BOMB WING LEFT INBOARD",
									0,	 //	0,	//"BOMB CENTER",
									0,	 //	1,	//"BOMB CONFORMAL RAIL LEFT SMALL FORWARD",
									0,	 //	1,	//"BOMB CONFORMAL RAIL LEFT SMALL MIDDLE",
									0,	 //	1,	//"BOMB CONFORMAL RAIL LEFT SMALL REAR",
									0,	 //	1,	//"BOMB CONFORMAL RAIL LEFT MED FORWARD",
									0,	 //	1,	//"BOMB CONFORMAL RAIL LEFT MED REAR",
									0,	 //	2,	//"BOMB CONFORMAL RAIL LEFT LARGE FORWARD",
									0,	 //	2,	//"BOMB CONFORMAL RAIL LEFT LARGE REAR",
									0,	 //	1,	//"BOMB CONFORMAL RAIL RIGHT SMALL FORWARD",
									0,	 //	1,	//"BOMB CONFORMAL RAIL RIGHT SMALL MIDDLE",
									0,	 //	1,	//"BOMB CONFORMAL RAIL RIGHT SMALL REAR",
									0,	 //	1,	//"BOMB CONFORMAL RAIL RIGHT MED FORWARD",
									0,	 //	1,	//"BOMB CONFORMAL RAIL RIGHT MED REAR",
									0,	 //	2,	//"BOMB CONFORMAL RAIL RIGHT LARGE FORWARD",
									0,	 //	2,	//"BOMB CONFORMAL RAIL RIGHT LARGE REAR",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT LEFT FORWARD",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT LEFT MIDDLE",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT LEFT REAR",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT RIGHT FORWARD",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT RIGHT MIDDLE",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT RIGHT REAR",
									0,	 //	0,	//"BOMB WING RACK LEFT INBOARD",
									0,	 //	0,	//"BOMB WING RACK LEFT MIDDLE",
									0,	 //	0,	//"BOMB WING RACK LEFT OUTBOARD",
									0,	 //	0,	//"BOMB WING RACK RIGHT INBOARD",
									0,	 //	0,	//"BOMB WING RACK RIGHT MIDDLE",
									0,	 //	0,	//"BOMB WING RACK RIGHT OUTBOARD",
									0,	 //	0,	//"FLARE MISSILE",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT LEFT LARGE FORWARD",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT LEFT LARGE REAR",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT RIGHT LARGE FORWARD",
									0,	 //	0,	//"BOMB CONFORMAL MOUNT RIGHT LARGE REAR"
									0xFF//	0xFF
};

void LoadSpritesSmoke( void )
{

	TextureRef temp_ref;
	FPoint color;
	DWORD add_flag;

	if (TerrainYesNos & YN_NO_ITERATED_ALPHA)
		add_flag = ST_ADD;
	else
		add_flag = 0;



	temp_ref.CellColumns = 0;
	temp_ref.TotalCells = 0;
	temp_ref.CellWidth = 1.0f;
	temp_ref.CellHeight = 1.0f;

	SetTextureFormat(FT_16BIT_DATA);

//	strcpy(temp_ref.Name,"smokefir.pcx");
//	color = 1.0f;
//	SmokeTypes[ENGINE_FIRE].Init(Load3DTextureWithMipMaps(&temp_ref, 3,UT_DEFAULT),ST_FIRE_FLAGS,color,1.0f,2.0,12.0f FEET,25,0.0,0.05f,0.2f,0.15f,0.5f,10);

	SmokeTypes[ENGINE_FIRE].Texture = NULL;

	strcpy(temp_ref.Name,"smokeap.pcx");
	color = 0.1f;
	SmokeTypes[HEAVY_BLACK_SMOKE].Init(Load3DTextureWithMipMaps(&temp_ref, 3,UT_PURE_ALPHA),ST_DEFAULT | ST_HOT,color,1.0f,2.0f,12.0f FEET,250,0.0,0.05f,0.0f,0.45f,0.5f,10);

	strcpy(temp_ref.Name,"smkwsp.pcx");
	color = 0.9f;
	SmokeTypes[FUEL_DUMP].Init(Load3DTextureWithMipMaps(&temp_ref, 3,/*UT_DEFAULT*/ UT_PURE_ALPHA),ST_DEFAULT | ST_NO_FADEIN/* | ST_ADD*/,color,1.0f,2.0f,3.0f FEET,100,0.0,0.0f,0.10f,0.6f,0.3f,10);

	strcpy(temp_ref.Name,"smokeap.pcx");
	color = 0.9f;
	SmokeTypes[WHITE_MISSILE_SMOKE].Init(Load3DTextureWithMipMaps(&temp_ref, 3,/*UT_DEFAULT*/ UT_PURE_ALPHA),ST_DEFAULT | ST_HOT/* | ST_ADD*/,color,1.0f,3.0f,4.0f FEET,500,0.0f,0.0,0.3f,0.1f,0.6f);

	strcpy(temp_ref.Name,"smkwsp.pcx");
	color = 0.9f;
	SmokeTypes[WHITE_WINGTIP_VAPOR].Init(Load3DTextureWithMipMaps(&temp_ref, 3,/*UT_DEFAULT*/ UT_PURE_ALPHA),ST_DEFAULT/* | ST_ADD*/,color,1.0f,3.0f,0.25f FEET,100,0.0,0.0f,0.0f,0.5f,0.5f);

	strcpy(temp_ref.Name,"smokeap.pcx");
	color.SetValues(80.0/255.0,64/255.0,56/255.0);
	SmokeTypes[BROWN_ENGINE_SMOKE].Init(Load3DTextureWithMipMaps(&temp_ref, 3,UT_PURE_ALPHA),ST_DEFAULT | ST_HOT,color,0.10f,3.0f,6.0f FEET,800,0.0f,0.0,0.0f,0.4f,0.6f);

	SetTextureFormat(FT_FIRE_PALETTE);

	/* ------------------------------------------2/20/98 9:39AM--------------------------------------------
	 *
	 * ----------------------------------------------------------------------------------------------------*/
	strcpy(temp_ref.Name,"plexp.pcx");
	SpriteTypes[PLANE_EXPLODES].Texture = Load3DTexture(&temp_ref,UT_RGB_ALPHA);
	SpriteTypes[PLANE_EXPLODES].Flags = MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[PLANE_EXPLODES].NumFrames = 8;
	SpriteTypes[PLANE_EXPLODES].PolyType = PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[PLANE_EXPLODES].RWWidth  = 80.0f FEET;
	SpriteTypes[PLANE_EXPLODES].RWHeight = 80.0f FEET;

	SpriteTypes[PLANE_EXPLODES].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[PLANE_EXPLODES].YVel = 0.0f;
	SpriteTypes[PLANE_EXPLODES].XVel = 0.0f;

	SpriteTypes[PLANE_EXPLODES].FramesToFade = 3.0f;
	SpriteTypes[PLANE_EXPLODES].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[PLANE_EXPLODES].UWidth = SpriteTypes[PLANE_EXPLODES].VHeight = 64.0f; //pixels;


	/* ------------------------------------------2/20/98 9:39AM--------------------------------------------
 	*
 	* ----------------------------------------------------------------------------------------------------*/

	strcpy(temp_ref.Name,"plexp.pcx");
	SpriteTypes[PLANE_BURNS].Texture = Load3DTexture(&temp_ref,UT_DEFAULT);
	SpriteTypes[PLANE_BURNS].Flags = MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[PLANE_BURNS].NumFrames = 8;
	SpriteTypes[PLANE_BURNS].PolyType = PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[PLANE_BURNS].RWWidth  = 40.0f FEET;
	SpriteTypes[PLANE_BURNS].RWHeight = 40.0f FEET;

	SpriteTypes[PLANE_BURNS].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[PLANE_BURNS].YVel = 0.0f;
	SpriteTypes[PLANE_BURNS].XVel = 0.0f;

	SpriteTypes[PLANE_BURNS].FramesToFade = 3.0f;
	SpriteTypes[PLANE_BURNS].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[PLANE_BURNS].UWidth = SpriteTypes[PLANE_BURNS].VHeight = 64.0f; //pixels


	/* ------------------------------------------2/20/98 9:39AM--------------------------------------------
	 *
	 * ----------------------------------------------------------------------------------------------------*/

	strcpy(temp_ref.Name,"cloud05.pcx");
	SpriteTypes[SPT_ALPHA_MAP].Texture		= Load3DTexture(&temp_ref, UT_PURE_ALPHA);
	SpriteTypes[SPT_ALPHA_MAP].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_ALPHA_MAP].NumFrames	= 1;
	SpriteTypes[SPT_ALPHA_MAP].PolyType	= PRIM_TEXTURE | PRIM_ALPHA;

	SpriteTypes[SPT_ALPHA_MAP].RWWidth	= 120 FEET;//40 FEET * 3.0f;
	SpriteTypes[SPT_ALPHA_MAP].RWHeight= 120 FEET;//40 FEET * 3.0f;

	SpriteTypes[SPT_ALPHA_MAP].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_ALPHA_MAP].YVel = 0.0f;
	SpriteTypes[SPT_ALPHA_MAP].XVel = 0.0f;

	SpriteTypes[SPT_ALPHA_MAP].FramesToFade = 3.0f;
	SpriteTypes[SPT_ALPHA_MAP].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_ALPHA_MAP].UWidth = SpriteTypes[SPT_ALPHA_MAP].VHeight = 64.0f; //pixels


	/* ------------------------------------------2/20/98 9:39AM--------------------------------------------
	 *
	 * ----------------------------------------------------------------------------------------------------*/
	strcpy(temp_ref.Name,"spuffsm2.pcx");
	SpriteTypes[SPT_BLACK_PUFF].Texture		= Load3DTexture(&temp_ref, UT_PURE_ALPHA);
	SpriteTypes[SPT_BLACK_PUFF].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_BLACK_PUFF].NumFrames	= 1;
	SpriteTypes[SPT_BLACK_PUFF].PolyType	= PRIM_TEXTURE | PRIM_ALPHA;

	SpriteTypes[SPT_BLACK_PUFF].RWWidth	= 120 FEET;//40 FEET * 3.0f;
	SpriteTypes[SPT_BLACK_PUFF].RWHeight= 120 FEET;//40 FEET * 3.0f;

	SpriteTypes[SPT_BLACK_PUFF].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_BLACK_PUFF].YVel = 0.0f;
	SpriteTypes[SPT_BLACK_PUFF].XVel = 0.0f;

	SpriteTypes[SPT_BLACK_PUFF].FramesToFade = 3.0f;
	SpriteTypes[SPT_BLACK_PUFF].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_BLACK_PUFF].UWidth = SpriteTypes[SPT_BLACK_PUFF].VHeight = 64.0f; //pixels

	/* ------------------------------------------2/20/98 9:39AM--------------------------------------------
	 *
	 * ----------------------------------------------------------------------------------------------------*/

	SetTextureFormat(FT_16BIT_DATA);

	strcpy(temp_ref.Name,"hrdflsh.pcx");
	SpriteTypes[SPT_FLASH].Texture	= Load3DTexture(&temp_ref, UT_DEFAULT);
	SpriteTypes[SPT_FLASH].Flags	 = MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_FLASH].NumFrames = 1;
	SpriteTypes[SPT_FLASH].PolyType  = PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_FLASH].RWWidth	= 20;//40 FEET * 3.0f;
	SpriteTypes[SPT_FLASH].RWHeight	= 20;//40 FEET * 3.0f;

	SpriteTypes[SPT_FLASH].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_FLASH].YVel = 0.0f;
	SpriteTypes[SPT_FLASH].XVel = 0.0f;

	SpriteTypes[SPT_FLASH].FramesToFade = 3.0f;
	SpriteTypes[SPT_FLASH].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_FLASH].UWidth = SpriteTypes[SPT_FLASH].VHeight = 64.0f; //pixels

	//strcpy(temp_ref.Name,"spuffsm.pcx");
	strcpy(temp_ref.Name,"cloud01.pcx");
	SpriteTypes[SPT_WHITE_PUFF].Texture		= Load3DTexture(&temp_ref, UT_DEFAULT);
	SpriteTypes[SPT_WHITE_PUFF].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_WHITE_PUFF].NumFrames	= 1;
	SpriteTypes[SPT_WHITE_PUFF].PolyType	= PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_WHITE_PUFF].RWWidth	= 120 FEET;//40 FEET * 3.0f;
	SpriteTypes[SPT_WHITE_PUFF].RWHeight= 120 FEET;//40 FEET * 3.0f;

	SpriteTypes[SPT_WHITE_PUFF].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_WHITE_PUFF].YVel = 0.0f;
	SpriteTypes[SPT_WHITE_PUFF].XVel = 0.0f;

	SpriteTypes[SPT_WHITE_PUFF].FramesToFade = 3.0f;
	SpriteTypes[SPT_WHITE_PUFF].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_WHITE_PUFF].UWidth = SpriteTypes[SPT_WHITE_PUFF].VHeight = 64.0f; //pixels

	strcpy(temp_ref.Name,"airexp_h.pcx");//software
	SpriteTypes[SPT_AIR_EXP].Texture	= Load3DTexture(&temp_ref, UT_DEFAULT);
	SpriteTypes[SPT_AIR_EXP].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_AIR_EXP].NumFrames	= 3;
	SpriteTypes[SPT_AIR_EXP].PolyType	= PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_AIR_EXP].RWWidth = 100 FEET;
	SpriteTypes[SPT_AIR_EXP].RWHeight= 100 FEET;

	SpriteTypes[SPT_AIR_EXP].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_AIR_EXP].YVel = 0.0f;
	SpriteTypes[SPT_AIR_EXP].XVel = 0.0f;

	SpriteTypes[SPT_AIR_EXP].FramesToFade = 3.0f;
	SpriteTypes[SPT_AIR_EXP].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_AIR_EXP].UWidth = SpriteTypes[SPT_AIR_EXP].VHeight = 64.0f; //pixels


	strcpy(temp_ref.Name,"sunrgb.pcx");
	SpriteTypes[SPT_SUN].Texture	= Load3DTexture(&temp_ref, UT_DEFAULT);
	SpriteTypes[SPT_SUN].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_SUN].NumFrames = 1;//4;
	SpriteTypes[SPT_SUN].PolyType = PRIM_TEXTURE | PRIM_NO_Z_WRITE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_SUN].RWWidth	= 32; //40 FEET
	SpriteTypes[SPT_SUN].RWHeight	= 32; //40 FEET

	SpriteTypes[SPT_SUN].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_SUN].YVel = 0.0f;
	SpriteTypes[SPT_SUN].XVel = 0.0f;

	SpriteTypes[SPT_SUN].FramesToFade = 3.0f;
	SpriteTypes[SPT_SUN].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_SUN].UWidth = SpriteTypes[SPT_SUN].VHeight = 64.0f; //pixels

	strcpy(temp_ref.Name,"bigsun.pcx");
	SpriteTypes[SPT_SUN_CORONA].Texture	= Load3DTexture(&temp_ref, UT_DEFAULT);
	SpriteTypes[SPT_SUN_CORONA].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_SUN_CORONA].NumFrames = 1;//4;
	SpriteTypes[SPT_SUN_CORONA].PolyType = PRIM_TEXTURE | PRIM_NO_Z_WRITE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_SUN_CORONA].RWWidth	= 96; //40 FEET
	SpriteTypes[SPT_SUN_CORONA].RWHeight	= 96; //40 FEET

	SpriteTypes[SPT_SUN_CORONA].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_SUN_CORONA].YVel = 0.0f;
	SpriteTypes[SPT_SUN_CORONA].XVel = 0.0f;

	SpriteTypes[SPT_SUN_CORONA].FramesToFade = 3.0f;
	SpriteTypes[SPT_SUN_CORONA].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_SUN_CORONA].UWidth = SpriteTypes[SPT_SUN_CORONA].VHeight = 128.0f; //pixels

	strcpy(temp_ref.Name,"moon.pcx");
	SpriteTypes[SPT_MOON].Texture	= Load3DTexture(&temp_ref, UT_DEFAULT);
	SpriteTypes[SPT_MOON].Flags		= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_MOON].NumFrames = 1;//4;
	SpriteTypes[SPT_MOON].PolyType = PRIM_TEXTURE | PRIM_NO_Z_WRITE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_MOON].RWWidth	= 32; //40 FEET
	SpriteTypes[SPT_MOON].RWHeight	= 32; //40 FEET

	SpriteTypes[SPT_MOON].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_MOON].YVel = 0.0f;
	SpriteTypes[SPT_MOON].XVel = 0.0f;

	SpriteTypes[SPT_MOON].FramesToFade = 3.0f;
	SpriteTypes[SPT_MOON].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_MOON].UWidth = SpriteTypes[SPT_MOON].VHeight = 128.0f; //pixels

	strcpy(temp_ref.Name,"oilfire2.pcx");
	SpriteTypes[SPT_2D_OILFIRE].Texture = Load3DTexture(&temp_ref,UT_RGB_ALPHA);//UT_PURE_ALPHA);//
	SpriteTypes[SPT_2D_OILFIRE].Flags = MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_2D_OILFIRE].NumFrames = 8;
	SpriteTypes[SPT_2D_OILFIRE].PolyType = PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_2D_OILFIRE].RWWidth  = 80.0f FEET;
	SpriteTypes[SPT_2D_OILFIRE].RWHeight = 80.0f FEET;

	SpriteTypes[SPT_2D_OILFIRE].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_2D_OILFIRE].YVel = 0.0f;
	SpriteTypes[SPT_2D_OILFIRE].XVel = 0.0f;

	SpriteTypes[SPT_2D_OILFIRE].FramesToFade = 3.0f;
	SpriteTypes[SPT_2D_OILFIRE].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_2D_OILFIRE].UWidth = SpriteTypes[SPT_2D_OILFIRE].VHeight = 64.0f; //pixels

	ReSetTextureFormat();


	/* ------------------------------------------2/20/98 9:39AM--------------------------------------------
	 *
	 * ----------------------------------------------------------------------------------------------------*/

	strcpy(temp_ref.Name,"flares.pcx");
	SpriteTypes[SPT_FLARE].Texture	= Load3DTexture(&temp_ref, UT_PURE_ALPHA);
	SpriteTypes[SPT_FLARE].Flags	= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_FLARE].NumFrames= 4;
	SpriteTypes[SPT_FLARE].PolyType = PRIM_TEXTURE | PRIM_ALPHA | add_flag;

	SpriteTypes[SPT_FLARE].RWWidth	= 32;//40 FEET;
	SpriteTypes[SPT_FLARE].RWHeight	= 32;//40 FEET;

	SpriteTypes[SPT_FLARE].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_FLARE].YVel = 0.0f;
	SpriteTypes[SPT_FLARE].XVel = 0.0f;

	SpriteTypes[SPT_FLARE].FramesToFade = 3.0f;
	SpriteTypes[SPT_FLARE].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_FLARE].UWidth = SpriteTypes[SPT_FLARE].VHeight = 64.0f; //pixels


	strcpy(temp_ref.Name,"rainspot.pcx");
	SpriteTypes[SPT_RAINSPOT].Texture	= Load3DTexture(&temp_ref, UT_PURE_ALPHA);
	SpriteTypes[SPT_RAINSPOT].Flags	= MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_RAINSPOT].NumFrames= 1;
	SpriteTypes[SPT_RAINSPOT].PolyType = PRIM_TEXTURE | PRIM_ALPHA | add_flag;

	SpriteTypes[SPT_RAINSPOT].RWWidth	= 16;
	SpriteTypes[SPT_RAINSPOT].RWHeight	= 16;

	SpriteTypes[SPT_RAINSPOT].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_RAINSPOT].YVel = 0.0f;
	SpriteTypes[SPT_RAINSPOT].XVel = 0.0f;

	SpriteTypes[SPT_RAINSPOT].FramesToFade = 3.0f;
	SpriteTypes[SPT_RAINSPOT].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_RAINSPOT].UWidth = SpriteTypes[SPT_RAINSPOT].VHeight = 32.0f; //pixels


	strcpy(temp_ref.Name,"oilfire3.pcx");
	//strcpy(temp_ref.Name,"oilfre.pcx");
	SpriteTypes[SPT_OILFIRE].Texture = Load3DTexture(&temp_ref,UT_RGB_ALPHA);//UT_PURE_ALPHA);//
	SpriteTypes[SPT_OILFIRE].Flags = MASK_FADE_TYPE | RANDOM_ORIENTATION;
	SpriteTypes[SPT_OILFIRE].NumFrames = 8;
	SpriteTypes[SPT_OILFIRE].PolyType = PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD;

	SpriteTypes[SPT_OILFIRE].RWWidth  = 80.0f FEET;
	SpriteTypes[SPT_OILFIRE].RWHeight = 80.0f FEET;

	SpriteTypes[SPT_OILFIRE].FrameMultiplier = 8.0f/50.0f;
	SpriteTypes[SPT_OILFIRE].YVel = 0.0f;
	SpriteTypes[SPT_OILFIRE].XVel = 0.0f;

	SpriteTypes[SPT_OILFIRE].FramesToFade = 3.0f;
	SpriteTypes[SPT_OILFIRE].OneOverFramesToFade = 1.0f/3.0f;

	SpriteTypes[SPT_OILFIRE].UWidth = SpriteTypes[SPT_OILFIRE].VHeight = 64.0f; //pixels

}

ObjectHandler *S2Rack, *S3Rack, *S4Rack, *S6Rack, *S8Rack, *S9Rack, *S10Rack;
void InitRenderer(void)
{
	float sun_angle;
	SubObjectHandler *swalker;
	BYTE *bwalker;

	InitLights();
	ZeroMemory( SourceSubs,sizeof(SourceSubs));

	swalker = SourceSubs;
	bwalker = WhichBombVars;

	while(*bwalker != 0xFF)
	{
		if (*bwalker)
			swalker->AllVars = (unsigned char *)&BombVars[*bwalker];
		bwalker++;
		swalker++;
	}

	pigs = FALSE;

	//WorldParams.Weather = WR_CLOUD2_CLEAR | WR_VIS_NRM | WR_CLOUD1_CLEAR;


//	WorldParams.FogNearZ = 0.0f;
//	WorldParams.FogFarZ  = -1.0f;


	Sun.SetValues(-0.95694f,-0.290285f,-0.0f);
	sun_angle = atan2(fabs(Sun.Y),-Sun.X);
	sun_angle = 1.0f - fabs(sin(sun_angle));
	sun_angle *= sun_angle * sun_angle;
	sun_angle = 1.0f - sun_angle;
//	Set3DLighting(Sun.X, Sun.Y, Sun.Z, 1.0f, sun_angle);

	LastSmokeTrail = &SmokeTrails[-1];
	memset(&SmokeTrails[0],0,sizeof(SmokeTrail)*(MAX_SMOKE_TRAILS-1));

	Set3DGrBuff(BackSurface);

// 	Set3DZoomFactor(0.5);

	get_path = get_texture_path;
	SortObjectVector = F18Sorter;
	DrawObjectVector = F18Drawer;
	AboutToHitDisk   = PauseForDiskHit;
	DoneWithDiskHit  = UnPauseFromDiskHit;


	UpdateCanisterPositionVec = UpdateCanisterPos;
	FreeCanisterHandleVec	  = FreeCanisterHandle;
	GetSteamSmokeHandleVec	  = GetSteamSmokeHandle;
	GetWellHeadFireVec		  = GetWellHeadSmokeHandle;
	GetBurnoffFireVec		  = GetBurnoffSmokeHandle;

	LeftMavRack = Load3DObject( RegPath("objects","wmrackl.3dg") );
	RightMavRack = Load3DObject( RegPath("objects","wmrackr.3dg") );

	S2Rack = Load3DObject( RegPath("objects","2fer2.3dg") );
	S3Rack = Load3DObject( RegPath("objects","2fer3.3dg") );
	S4Rack = Load3DObject( RegPath("objects","2fer4.3dg") );
	S6Rack = Load3DObject( RegPath("objects","2fer06.3dg") );
	S8Rack = Load3DObject( RegPath("objects","2fer08.3dg") );
	S9Rack = Load3DObject( RegPath("objects","2fer09.3dg") );
	S10Rack = Load3DObject( RegPath("objects","2fer10.3dg") );

	Meatball = Load3DObject(RegPath("objects","nitlght4.3dg"));
	FuelBasket = Load3DObject(RegPath("objects","fuelbask.3dg"));
	TowedDecoy = Load3DObject(RegPath("objects","ale50.3dg"));

	LoadDestroyedPlaneParts();
	InitParticleSystem();
	LoadVirtualCockpit( );


	LoadSpritesSmoke();

	FixGameTextures();

	LastGenerator = &Generators[-1];
	memset(Generators,0,sizeof(Generator)*MAX_GENERATORS);

	LastSprite = &Sprites[-1];
	memset(Sprites,0,sizeof(Sprite)*MAX_SPRITES);

	InitTerrain();
//	InitTerrainPoints();
	InitGroundFX();

}


void LoadPlaneShadowsSoft( void )
{
//	TextureRef temp_ref;
//	char shadowname[20];

//	for (int i=0; i<MAX_PLANE_TYPES; i++)
//	{
//		if (PlaneTypes[i].Shadow)
//		{
//			memcpy( shadowname, PlaneTypes[i].Shadow->Info.Name, 20);
//			Free3DTexture(PlaneTypes[i].Shadow);

//			temp_ref.CellColumns = 0;
//			temp_ref.TotalCells  = 0;
//			temp_ref.CellWidth   = 1.0f;
//			temp_ref.CellHeight  = 1.0f;

//			int len = strlen( shadowname);
//			shadowname[len-5] = 'd';

//			sprintf(temp_ref.Name,"%s",shadowname);
//			PlaneTypes[i].Shadow = Load3DTexture(&temp_ref,TP_ALPHA);
//		}
//	}

}

void LoadPlaneShadowsHardware( void )
{
//	TextureRef temp_ref;
//	char shadowname[20];

//	for (int i=0; i<MAX_PLANE_TYPES; i++)
//	{
//		if (PlaneTypes[i].Shadow)
//		{
//			memcpy( shadowname, PlaneTypes[i].Shadow->Info.Name, 20);
//			Free3DTexture( PlaneTypes[i].Shadow );

//			temp_ref.CellColumns = 0;
//			temp_ref.TotalCells  = 0;
//			temp_ref.CellWidth   = 1.0f;
//			temp_ref.CellHeight  = 1.0f;

//			int len = strlen( shadowname) ;
//			shadowname[len-5] = 'h';

//			sprintf(temp_ref.Name,"%s",shadowname);
//			PlaneTypes[i].Shadow = Load3DTexture(&temp_ref,UT_PURE_ALPHA);
//		}
//	}

}


// Set Software Var/Textures/ect for Software 3Dfx
void SetF18Hardware( void)
{
//	TextureRef temp_ref;
//	temp_ref.CellColumns = 0;
//	temp_ref.TotalCells	 = 0;
//	temp_ref.CellWidth	 = 1.0f;
//	temp_ref.CellHeight	 = 1.0f;

//	// Set Shadow Textures
//	LoadPlaneShadowsHardware();

//	// Set Missle Textures
//	if (g_Settings.gr.nWeaponSmoke)
//	{
//		ReloadTexture( SmokeTypes[HEAVY_BLACK_SMOKE].Texture, "smokeap.pcx");
//		ReloadTexture( SmokeTypes[WHITE_MISSILE_SMOKE].Texture, "smokeap.pcx");
//	}

//	strcpy(temp_ref.Name,"hrdflsh.pcx");
//	Free3DTexture(SpriteTypes[SPT_FLASH].Texture);
//	SpriteTypes[SPT_FLASH].Texture	= Load3DTexture( &temp_ref,TP_ALPHA );


//	SpriteTypes[SPT_FLASH].RWWidth	= 32;//40 FEET * 3.0f;
//	SpriteTypes[SPT_FLASH].RWHeight	= 32;//40 FEET * 3.0f;

//	strcpy(temp_ref.Name,"spuffsm.pcx");
//	Free3DTexture(SpriteTypes[SPT_BLACK_PUFF].Texture);
//	SpriteTypes[SPT_BLACK_PUFF].Texture	= Load3DTexture( &temp_ref ,TP_ALPHA);


//	Free3DTexture(SpriteTypes[SPT_WHITE_PUFF].Texture);
//	SpriteTypes[SPT_WHITE_PUFF].Texture	= Load3DTexture( &temp_ref ,TP_ALPHA);

//	strcpy(temp_ref.Name,"sunhrd.pcx");
//	Free3DTexture(SpriteTypes[SPT_SUN].Texture);
//	SpriteTypes[SPT_SUN].Texture	= Load3DTexture( &temp_ref ,TP_ALPHA);

//	SpriteTypes[SPT_SUN].UWidth = SpriteTypes[SPT_SUN].VHeight = 63.0f*(float)0x8000000/256.0f;
//	SpriteTypes[SPT_SUN].RWWidth	= 32;
//	SpriteTypes[SPT_SUN].RWHeight	= 32;

//	strcpy(temp_ref.Name,"airexp_h.pcx");
//	Free3DTexture(SpriteTypes[SPT_AIR_EXP].Texture);
//	SpriteTypes[SPT_AIR_EXP].Texture	= Load3DTexture(&temp_ref,TP_ALPHA);


//	FixGameTextures(0.5f*(float)0x8000000/256.0f);

//	ChangeTextureCoordinates(TSF_HARDWARE);

//	TerrainLoaded = FALSE;
//	if (MediumTextureCache)
//	{

//		FreeTexture3Dfx(MediumTextureCache->Buffer);
//		MediumTextureCache->BufferPtr3DFX = NULL;
//	}

//	if (HighTextureCache)
//	{
//		FreeTexture3Dfx(HighTextureCache->Buffer);
//		HighTextureCache->BufferPtr3DFX = NULL;
//	}

//	if (MovingMapMediumCache)
//	{
//		FreeTexture3Dfx(MovingMapMediumCache->Buffer);
//		MovingMapMediumCache->BufferPtr3DFX = NULL;
//	}

//	if (MovingMapHighCache)
//	{
//		FreeTexture3Dfx(MovingMapHighCache->Buffer);
//		MovingMapHighCache->BufferPtr3DFX = NULL;
//	}

}


void SetF18Software(void)
{
//	TextureRef temp_ref;
//	temp_ref.CellColumns = 0;
//	temp_ref.TotalCells	 = 0;
//	temp_ref.CellWidth   = 1.0f;
//	temp_ref.CellHeight  = 1.0f;

//	// Reset Shadow textures
//	LoadPlaneShadowsSoft();

//	// Reset Missle textures
//	if (g_Settings.gr.nWeaponSmoke)
//	{
//		ReloadTexture( SmokeTypes[HEAVY_BLACK_SMOKE].Texture, "smksoft.pcx");
//		ReloadTexture( SmokeTypes[WHITE_MISSILE_SMOKE].Texture, "smksoft.pcx"); //smokeb.pcx
//	}

//	strcpy(temp_ref.Name,"sftflsh.pcx");
//	Free3DTexture(SpriteTypes[SPT_FLASH].Texture);
//	SpriteTypes[SPT_FLASH].Texture	= Load3DTexture( &temp_ref );
//	SpriteTypes[SPT_FLASH].RWWidth	= 20;//40 FEET * 3.0f;
//	SpriteTypes[SPT_FLASH].RWHeight	= 20;//40 FEET * 3.0f;

//	strcpy(temp_ref.Name,"spuffbsf.pcx");
//	Free3DTexture(SpriteTypes[SPT_BLACK_PUFF].Texture);
//	SpriteTypes[SPT_BLACK_PUFF].Texture	= Load3DTexture( &temp_ref );

//	strcpy(temp_ref.Name,"spuffwsf.pcx");
//	Free3DTexture(SpriteTypes[SPT_WHITE_PUFF].Texture);
//	SpriteTypes[SPT_WHITE_PUFF].Texture	= Load3DTexture( &temp_ref );

//	strcpy(temp_ref.Name,"sunsoft.pcx");
//	Free3DTexture(SpriteTypes[SPT_SUN].Texture);
//	SpriteTypes[SPT_SUN].Texture= Load3DTexture( &temp_ref );
//	SpriteTypes[SPT_SUN].UWidth = SpriteTypes[SPT_SUN].VHeight = 31.0f*(float)0x8000000/256.0f;
//	SpriteTypes[SPT_SUN].RWWidth	= 40 FEET;
//	SpriteTypes[SPT_SUN].RWHeight	= 40 FEET;

//	strcpy(temp_ref.Name,"airexp_s.pcx");
//	Free3DTexture(SpriteTypes[SPT_AIR_EXP].Texture);
//	SpriteTypes[SPT_AIR_EXP].Texture= Load3DTexture(&temp_ref);

//	FixGameTextures(0.0);

//	ChangeTextureCoordinates(TSF_SOFTWARE);

//	TerrainLoaded = FALSE;
}

void CloseMovingMap();
void ShutDownLights(void);

void CloseRenderer(void)
{
	int i;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	CloseMovingMap();

	for( i=0; i<MAX_SMOKE_TYPES; i++)
	{
		if( SmokeTypes[i].Texture)
			Free3DTexture(SmokeTypes[i].Texture);
		SmokeTypes[i].Texture = NULL;
	}

	for( i=0; i<MAX_SPRITE_TYPES; i++)
	{
		if( SpriteTypes[i].Texture)
			Free3DTexture(SpriteTypes[i].Texture);
		SpriteTypes[i].Texture = NULL;
	}


	Free3DObject(RightMavRack);
	Free3DObject(LeftMavRack);

	Free3DObject(S2Rack);
	Free3DObject(S3Rack);
	Free3DObject(S4Rack);
	Free3DObject(S6Rack);
	Free3DObject(S8Rack);
	Free3DObject(S9Rack);
	Free3DObject(S10Rack);

	Free3DObject(Meatball);
	Free3DObject(FuelBasket);


	FreeGroundFX();
	FreePlanePartTypes();

	Free3DObject(FREE_ALL_OBJECTS);
	Free3DTexture(FREE_ALL_TEXTURES);

	FreeAlphaLists();

	TexturesSetFor = TSF_SOFTWARE;

	if ((WorldParams.Weather & WR_CLOUD1) == WR_CLOUD1_OVERCAST)
	{
		_splitpath(FadingFilename,drive,dir,fname,NULL);

		if (fname[strlen(fname)-2] != '-')
			fname[strlen(fname)-1] = 0;
		else
			fname[strlen(fname)-3] = 0;

		_makepath(FadingFilename,drive,dir,fname,"clr");

//		ReadFadeTable();
	}

	ShutDownLights();

#ifdef __DEMO__
	if( DemoLogo)
		GrFreeGrBuff(DemoLogo);
	DemoLogo = NULL;
#endif


}

extern BOOL NoObjectCacheing;

int LoadPlaneModel(PlaneType *type,char *path_to_model)
{
	char path[_MAX_PATH];
	TextureRef temp_ref;
	char *name = NULL;
	char *shadowname = NULL;
	int file;
	int size;
	int ret_val = FALSE;

	if (g_nMissionType == SIM_MULTI)
		NoObjectCacheing = TRUE;

	int iAircraftIndex = RegisterAircraft(type->TypeNumber);
	if (iAircraftIndex==-1)
	{
		// Could not find aircraft, so force F-15 Model
		iAircraftIndex = RegisterAircraft(0);	// F-15 Model
	}

	name = pDBAircraftList[iAircraftIndex].sObjectName;
	shadowname = pDBAircraftList[iAircraftIndex].sShadowName;


	type->EngineConfig = pDBAircraftList[iAircraftIndex].iEngineConfig;
	type->ShadowULXOff = pDBAircraftList[iAircraftIndex].fShadowULXOff*FOOT_MULTER;
	type->ShadowULZOff = pDBAircraftList[iAircraftIndex].fShadowULZOff*FOOT_MULTER;
	type->ShadowLRXOff = pDBAircraftList[iAircraftIndex].fShadowLRXOff*FOOT_MULTER;
	type->ShadowLRZOff = pDBAircraftList[iAircraftIndex].fShadowLRZOff*FOOT_MULTER;

	type->GearDownHeight = pDBAircraftList[iAircraftIndex].fGearDownHeight*FOOT_MULTER;
	type->GearUpHeight   = pDBAircraftList[iAircraftIndex].fGearUpHeight*FOOT_MULTER;
	type->TailHeight     = pDBAircraftList[iAircraftIndex].fTailHeight*FOOT_MULTER;

	for (int i=0; i<10; i++)
	{
		type->Offsets[i].SetValues( (pDBAircraftList[iAircraftIndex].OffSet[i].X*FOOT_MULTER),
									(pDBAircraftList[iAircraftIndex].OffSet[i].Y*FOOT_MULTER),
									(pDBAircraftList[iAircraftIndex].OffSet[i].Z*FOOT_MULTER) );
	}

	type->Offsets[OFFSET_LEFT_FUEL_DUMP].SetValues( (pDBAircraftList[iAircraftIndex].OffSet[15].X*FOOT_MULTER),
													(pDBAircraftList[iAircraftIndex].OffSet[15].Y*FOOT_MULTER),
													(pDBAircraftList[iAircraftIndex].OffSet[15].Z*FOOT_MULTER));

	type->Offsets[OFFSET_RIGHT_FUEL_DUMP].SetValues((pDBAircraftList[iAircraftIndex].OffSet[16].X*FOOT_MULTER),
													(pDBAircraftList[iAircraftIndex].OffSet[16].Y*FOOT_MULTER),
													(pDBAircraftList[iAircraftIndex].OffSet[16].Z*FOOT_MULTER));


	temp_ref.CellColumns = 0;
	temp_ref.TotalCells  = 0;
	temp_ref.CellWidth   = 1.0f;
	temp_ref.CellHeight  = 1.0f;


	//Load Destroyed Hulks if they exisit for this plane
	switch  (pDBAircraftList[iAircraftIndex].lDestroyModel)
	{
		case AIRCRAFT_DAM_F18_HULK:
			type->DestroyedModel = Load3DObject(RegPath("objects","buf18eh.3dg"));
			break;

		case AIRCRAFT_DAM_ALLIED_HULK:
			type->DestroyedModel = Load3DObject(RegPath("objects","buemh2.3dg"));
			break;

		case AIRCRAFT_DAM_ENEMY_HULK:
			type->DestroyedModel = Load3DObject(RegPath("objects","buemh.3dg"));
			break;

		case AIRCRAFT_DAM_HELO_HULK:
			type->DestroyedModel = Load3DObject(RegPath("objects","buhelh.3dg"));
			break;

		case AIRCRAFT_DAM_BUE2:
			type->DestroyedModel = Load3DObject(RegPath("objects","bue2.3dg"));
			break;

		case AIRCRAFT_DAM_BUEA6:
			type->DestroyedModel = Load3DObject(RegPath("objects","buea6.3dg"));
			break;

		case AIRCRAFT_DAM_BUF14:
			type->DestroyedModel = Load3DObject(RegPath("objects","buf14.3dg"));
			break;

		case AIRCRAFT_DAM_BUMIG25:
			type->DestroyedModel = Load3DObject(RegPath("objects","bumig25.3dg"));
			break;

		case AIRCRAFT_DAM_BUMIG29:
			type->DestroyedModel = Load3DObject(RegPath("objects","bumig29.3dg"));
			break;

		case AIRCRAFT_DAM_BUS3:
			type->DestroyedModel = Load3DObject(RegPath("objects","bus3.3dg"));
			break;

		case AIRCRAFT_DAM_BUSU24:
			type->DestroyedModel = Load3DObject(RegPath("objects","busu24.3dg"));
			break;

		case AIRCRAFT_DAM_BUSU25:
			type->DestroyedModel = Load3DObject(RegPath("objects","busu25.3dg"));
			break;

		case AIRCRAFT_DAM_BUSU27:
			type->DestroyedModel = Load3DObject(RegPath("objects","busu27.3dg"));
			break;

		case AIRCRAFT_DAM_BUSU35:
			type->DestroyedModel = Load3DObject(RegPath("objects","busu35.3dg"));
			break;

		case AIRCRAFT_DAM_BUV22:
			type->DestroyedModel = Load3DObject(RegPath("objects","buv22.3dg"));
			break;

		case 0:
		default:
			type->DestroyedModel = NULL;
			break;
	}


	sprintf(temp_ref.Name,"%s.pcx",shadowname);

	temp_ref.Name[strlen(temp_ref.Name)-5] = 'h';

	type->Shadow = Load3DTexture(&temp_ref,UT_PURE_ALPHA);

	if ((type->TypeNumber == PT_F18e) && (g_nMissionType == SIM_MULTI))
		name = "f15em";

	if (path_to_model)
	{
//		if (path_to_model[strlen(path_to_model)-1] == '\\')
//			sprintf(path,"%s%s.3dg",path_to_model,name);
//		else
//			sprintf(path,"%s\\%s.3dg",path_to_model,name);

		SpecialTexturePath = path_to_model;
	}
	else
		sprintf(path,"%s.3dg",name);

	type->Model = Load3DObject(RegPath("objects",path));

	if (type->Model)
	{

		// free all subparts
		for (i=0; i < MAX_CONNECTED_PARTS; i++)
		{
			if (type->SubParts[i])  Free3DObject(type->SubParts[i]);
			type->SubParts[i] = NULL;
		}

		sprintf(path,"%s.spn",name);
		file = _open(RegPath("objects",path),_O_BINARY|_O_RDONLY);
		if (file >= 0)
		{

			for (i=0; i < MAX_CONNECTED_PARTS; i++)
			{
				if (read(file,&size,sizeof(int)) == sizeof(int)
				&&  size > 0
				&&	read(file,path,size) == size)
					type->SubParts[i] = Load3DObject(RegPath("objects",path));
			}

			_close(file);
		}

		/* -----------------7/13/99 9:23AM---------------------------------------------------------------------
		/* figure out where all the special parts are.  Like tailhooks and refueling hoses etc.
		/* ----------------------------------------------------------------------------------------------------*/

		FMatrix standard;

		standard.Identity();
		type->TailHookPoint = 0;
		type->TailHookPivot = 0;
		type->TailHookLength = 0;
		type->TailHookRestAngle = 0;

		if (FindDot(type->Model,SO_TAIL_HOOK,NULL,type->TailHookPoint,standard) && FindDot(type->Model,SO_TAIL_HOOK_PIVOT,NULL,type->TailHookPivot,standard))
		{
			type->TailHookLength = type->TailHookPoint.Length();
			type->TailHookRestAngle = atan2(type->TailHookPoint.Y,type->TailHookPoint.Z);
		}

		standard.Identity();
		type->FrontWheelContact = 0;
		FindDot(type->Model,SO_FRONT_WHEEL_CONTACT,NULL,type->FrontWheelContact,standard);

		standard.Identity();
		type->RightRearWheelContact = 0;
		FindDot(type->Model,SO_RIGHT_REAR_WHEEL_CONTACT,NULL,type->RightRearWheelContact,standard);

		standard.Identity();
		type->LeftRearWheelContact = 0;
		FindDot(type->Model,SO_LEFT_REAR_WHEEL_CONTACT,NULL,type->LeftRearWheelContact,standard);

		standard.Identity();
		type->MachineGun = 0;
		FindDot(type->Model,SO_MACHINE_GUN,NULL,type->MachineGun,standard);

		standard.Identity();
		type->TowedDecoyPosition = 0;
		FindDot(type->Model,SO_DECOY_DISPENSER,NULL,type->TowedDecoyPosition,standard);

		i = 5;
		while (i--)
		{
			standard.Identity();
			type->RefuelingHosePositions[i] = 0;
			FindDot(type->Model,SO_REFUEL_1+i,NULL,type->RefuelingHosePositions[i],standard);
		}

		ret_val = TRUE;
	}

	NoObjectCacheing = FALSE;
	SpecialTexturePath = NULL;

	return ret_val;
}

#if 0
typedef struct _WeaponType
{
	int			  TypeNumber;
	ObjectHandler *Model;
	ObjectHandler *SubParts[MAX_CONNECTED_PARTS];
	FPoint		  Offsets[MAX_OFFSETS];
} WeaponType;
#endif

int LoadWeaponModel(WeaponType *type)
{
	char path[_MAX_PATH];
	char *name = NULL;
//	char fname[_MAX_FNAME+_MAX_EXT];
//	TextureRef temp_ref;
//	int file;
//	int size;
//	int i;

	int iWeaponIndex = RegisterWeapon(type->TypeNumber);
	if (iWeaponIndex==-1)
	{
		iWeaponIndex = RegisterWeapon(0);
	}

	name = pDBWeaponList[iWeaponIndex].sObjectFileName;

	sprintf(path,"%s.3dg",name);
	if (type->Model = Load3DObject(RegPath("objects",path)))
		return TRUE;
	else
	{
		char sTxt[80];
#ifdef _DEBUG
		DebugBreak();
#endif
		sprintf(sTxt,"Weapon Model %s [%s.3dg] File Not Found.",pDBWeaponList[iWeaponIndex].sName,name);
		MessageBox(hwnd,sTxt,"File Error",MB_OK);
		return FALSE;
	}

}

extern int GetMovingVehicleIndex(long lVehicleID);

int LoadVehicleModel(MovingVehicleType *type)
{
	char path[_MAX_PATH];
	char *name = NULL;
	int i;
	FPoint *pwalker;

	int iMovingVehicleIndex = GetMovingVehicleIndex(type->TypeNumber);
	name = pDBVehicleList[iMovingVehicleIndex].sFileName;

	type->Flags = 0;

	sprintf(path,"%s.3dg",name);
	if (type->Model = Load3DObject(RegPath("objects",path)))
	{
		type->WakeGen.NumPoints = 0;

		type->LowUL.SetValues(10000.0f,10000.0f,10000.0f);
		type->HighLR.SetValues(-10000.0f,-10000.0f,-10000.0f);

		i = *(int *)(((int)type->Model->ObjectPoints) - sizeof(int));
		pwalker = type->Model->ObjectPoints;

		while(i--)
		{
			if (pwalker->X < type->LowUL.X)
				type->LowUL.X = pwalker->X;

			if (pwalker->Y < type->LowUL.Y)
				type->LowUL.Y = pwalker->Y;

			if (pwalker->Z < type->LowUL.Z)
				type->LowUL.Z = pwalker->Z;

			if (pwalker->X > type->HighLR.X)
				type->HighLR.X = pwalker->X;

			if (pwalker->Y > type->HighLR.Y)
				type->HighLR.Y = pwalker->Y;

			if (pwalker->Z > type->HighLR.Z)
				type->HighLR.Z = pwalker->Z;

			pwalker++;
		}

		if (type->HighLR.Y >= (float)(20.0 FEET))
			type->Flags |= VT_COLLIDEABLE;

		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		DebugBreak();
#endif
		char sTxt[80];
		sprintf(sTxt,"MovingVehicle Model %s [%s.3dg] File Not Found.",pDBVehicleList[iMovingVehicleIndex].sName,name);
		MessageBox(hwnd,sTxt,"File Error",MB_OK);
		return FALSE;
	}

}

int LoadDestroyedVehicleModel(MovingVehicleType *type)
{
	char path[_MAX_PATH];
	char *name = NULL;
	int i;
	FPoint *pwalker;

	int iMovingVehicleIndex = GetMovingVehicleIndex(type->TypeNumber);
	name = pDBVehicleList[iMovingVehicleIndex].sBlownObjectFileName;

	type->Flags = 0;

	sprintf(path,"%s.3dg",name);
	if (type->Model = Load3DObject(RegPath("objects",path)))
	{
		type->WakeGen.NumPoints = 0;

		type->LowUL.SetValues(10000.0f,10000.0f,10000.0f);
		type->HighLR.SetValues(-10000.0f,-10000.0f,-10000.0f);

		i = *(int *)(((int)type->Model->ObjectPoints) - sizeof(int));
		pwalker = type->Model->ObjectPoints;

		while(i--)
		{
			if (pwalker->X < type->LowUL.X)
				type->LowUL.X = pwalker->X;

			if (pwalker->Y < type->LowUL.Y)
				type->LowUL.Y = pwalker->Y;

			if (pwalker->Z < type->LowUL.Z)
				type->LowUL.Z = pwalker->Z;

			if (pwalker->X > type->HighLR.X)
				type->HighLR.X = pwalker->X;

			if (pwalker->Y > type->HighLR.Y)
				type->HighLR.Y = pwalker->Y;

			if (pwalker->Z > type->HighLR.Z)
				type->HighLR.Z = pwalker->Z;

			pwalker++;
		}

		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		DebugBreak();
#endif
		char sTxt[80];
		sprintf(sTxt,"MovingVehicle Model %s [%s.3dg] File Not Found.",pDBVehicleList[iMovingVehicleIndex].sName,name);
		MessageBox(hwnd,sTxt,"File Error",MB_OK);
		return FALSE;
	}

}


extern int GetShipIndex(long lShipID);

char *NimitzNames[] = {"nitb5.3dg","nitb5",
					   "nitb5m.3dg","nitb5m",
					   "nitb5l.3dg","nitb5l"};


int LoadShipByName(MovingVehicleType *type,const char *name)
{
	char path[_MAX_PATH];
	int i;
	FPoint *pwalker;
	int filein;

	type->Flags = 0;

	sprintf(path,"%s.3dg",name);
	if (type->Model = Load3DObject(RegPath("objects",path)))
	{
		sprintf(path,"%s.wak",name);
		if ((filein = _open(RegPath("objects",path),_O_RDONLY | _O_BINARY)) != -1)
		{
			_read(filein,&type->WakeGen,sizeof(WakeBounds));
			_close(filein);
		}
		else
			type->WakeGen.NumPoints = 0;

		sprintf(path,"%s.twp",name);
		if ((filein = _open(RegPath("objects",path),_O_RDONLY | _O_BINARY)) != -1)
		{
			int pt_nums[4];
			FPoint calc;
			_read(filein,&pt_nums[0],4*sizeof(int));
			_close(filein);

			i = 4;
			while(i--)
			{
				if (pt_nums[i] != -1)
				{
					type->CatchWires.Wires[i] = type->Model->ObjectPoints + pt_nums[i];
					type->CatchWires.Values[i] = *type->CatchWires.Wires[i];
				}
				else
				{
					type->CatchWires.Wires[i] = NULL;
					type->CatchWires.Values[i].SetValues(0.0f);
				}
			}

			type->CatchWires.WireNormal.MakeVectorToFrom(type->CatchWires.Values[0],type->CatchWires.Values[1]);
			type->CatchWires.WireNormal.Y = 0.0f;
			type->CatchWires.WireNormal.Normalize();

			type->CatchWires.WireDs[0] = type->CatchWires.Values[0]*type->CatchWires.WireNormal;
			type->CatchWires.WireDs[1] = type->CatchWires.Values[1]*type->CatchWires.WireNormal;
			type->CatchWires.WireDs[2] = type->CatchWires.Values[2]*type->CatchWires.WireNormal;
			type->CatchWires.WireDs[3] = type->CatchWires.Values[3]*type->CatchWires.WireNormal;

			type->CatchWires.LeftNormal.X = type->CatchWires.WireNormal.Z;
			type->CatchWires.LeftNormal.Y = type->CatchWires.WireNormal.Y;
			type->CatchWires.LeftNormal.Z = -type->CatchWires.WireNormal.X;

			calc = type->CatchWires.Values[0];
			calc.AddScaledVector(-61.0f FEET,type->CatchWires.LeftNormal);
			type->CatchWires.LeftNegD = calc*type->CatchWires.LeftNormal;

			type->CatchWires.RightNormal.AddScaledVector(-1.0f,type->CatchWires.LeftNormal);
			calc = type->CatchWires.Values[0];
			calc.AddScaledVector(-61.0f FEET,type->CatchWires.RightNormal);
			type->CatchWires.RightNegD = calc*type->CatchWires.RightNormal;
		}


		type->LowUL.SetValues(10000.0f,10000.0f,10000.0f);
		type->HighLR.SetValues(-10000.0f,-10000.0f,-10000.0f);

		i = *(int *)(((int)type->Model->ObjectPoints) - sizeof(int));
		pwalker = type->Model->ObjectPoints;

		while(i--)
		{
			if (pwalker->X < type->LowUL.X)
				type->LowUL.X = pwalker->X;

			if (pwalker->Y < type->LowUL.Y)
				type->LowUL.Y = pwalker->Y;

			if (pwalker->Z < type->LowUL.Z)
				type->LowUL.Z = pwalker->Z;

			if (pwalker->X > type->HighLR.X)
				type->HighLR.X = pwalker->X;

			if (pwalker->Y > type->HighLR.Y)
				type->HighLR.Y = pwalker->Y;

			if (pwalker->Z > type->HighLR.Z)
				type->HighLR.Z = pwalker->Z;

			pwalker++;
		}

		if (type->HighLR.Y >= (float)(20.0 FEET))
			type->Flags |= VT_COLLIDEABLE;

		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		DebugBreak();
#endif
		char sTxt[80];
		sprintf(sTxt,"MovingVehicle Model %s [%s.3dg] File Not Found.",pDBShipList[type->TypeNumber].sName,name);
		MessageBox(hwnd,sTxt,"File Error",MB_OK);
		return FALSE;
	}

}

int ReloadShipModel(MovingVehicleType *type,const char *name)
{
	ObjectHandler *old_model = type->Model;

	if (LoadShipByName(type,name))
		Free3DObject(old_model);
	else
		type->Model = old_model;

	return TRUE;
}

void ReloadNimitzClassCarriers( void )
{
	char **swalk;
	char **new_name = &NimitzNames[GetHighest3DLOD()*2];
	MovingVehicleParams *W;

	for (W=MovingVehicles; W<=LastMovingVehicle; W++)
		if(W->Type)
			W->Type->Flags &= ~(MVT_CHECKED | MVT_NIMITZ_MODEL);

	for (W=MovingVehicles; W<=LastMovingVehicle; W++)
	{
		if(W->Type)
		{
			if (W->Type->Flags & MVT_NIMITZ_MODEL)
			{
				if (W->Shadow)
				{
					ReleaseShadowEntry(&W->Shadow);
					W->Shadow = NULL;
				}
			}
			else
				if (!(W->Type->Flags & MVT_CHECKED))
				{
					swalk = NimitzNames;

					while(swalk != &NimitzNames[6])
					{
					 	if ((swalk != new_name) && !stricmp(W->Type->Model->Name,*swalk))
						{
							W->Type->Flags |= MVT_NIMITZ_MODEL;
							ReloadShipModel(W->Type,new_name[1]);
							if (W->Shadow)
							{
								ReleaseShadowEntry(&W->Shadow);
								W->Shadow = NULL;
							}
							break;
						}
						else
							swalk += 2;
					}

					W->Type->Flags |= MVT_CHECKED;
				}
		}
	}
}

int LoadShipModel(MovingVehicleType *type)
{
	char *name = NULL;

	int iMovingVehicleIndex = GetShipIndex(type->TypeNumber);
	name = pDBShipList[iMovingVehicleIndex].sObjectFileName;

	// the nimitz carrier has different models for different detail levels
	// make sure we get the right one.
	if (!stricmp(name,NimitzNames[1]))
		name = NimitzNames[GetHighest3DLOD()*2+1];

	return LoadShipByName(type,name);
}

int LoadDestroyedShipModel(MovingVehicleType *type)
{
	char path[_MAX_PATH];
	char *name = NULL;
	int i;
	FPoint *pwalker;
	int filein;

	int iMovingVehicleIndex = GetShipIndex(type->TypeNumber);
	name = pDBShipList[iMovingVehicleIndex].sBlownObjectFileName;

	type->Flags = 0;

	sprintf(path,"%s.3dg",name);
	if (type->Model = Load3DObject(RegPath("objects",path)))
	{
		sprintf(path,"%s.wak",name);
		if ((filein = _open(RegPath("objects",path),_O_RDONLY | _O_BINARY)) != -1)
		{
			_read(filein,&type->WakeGen,sizeof(WakeBounds));
			_close(filein);
		}
		else
			type->WakeGen.NumPoints = 0;

		type->LowUL.SetValues(10000.0f,10000.0f,10000.0f);
		type->HighLR.SetValues(-10000.0f,-10000.0f,-10000.0f);

		i = *(int *)(((int)type->Model->ObjectPoints) - sizeof(int));
		pwalker = type->Model->ObjectPoints;

		while(i--)
		{
			if (pwalker->X < type->LowUL.X)
				type->LowUL.X = pwalker->X;

			if (pwalker->Y < type->LowUL.Y)
				type->LowUL.Y = pwalker->Y;

			if (pwalker->Z < type->LowUL.Z)
				type->LowUL.Z = pwalker->Z;

			if (pwalker->X > type->HighLR.X)
				type->HighLR.X = pwalker->X;

			if (pwalker->Y > type->HighLR.Y)
				type->HighLR.Y = pwalker->Y;

			if (pwalker->Z > type->HighLR.Z)
				type->HighLR.Z = pwalker->Z;

			pwalker++;
		}

		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		DebugBreak();
#endif
		char sTxt[80];
		sprintf(sTxt,"MovingVehicle Model %s [%s.3dg] File Not Found.",pDBShipList[iMovingVehicleIndex].sName,name);
		MessageBox(hwnd,sTxt,"File Error",MB_OK);
		return FALSE;
	}

}
