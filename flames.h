#ifndef __FLAMES_H__

#define __FLAMES_H__

#define MAX_SMOKE_TRAILS	64
#define MAX_POINTS_PER_TRAIL 64
#define MAX_FLAME_CELLS 1
#define MAX_LOOPS_PER_SWITCH 50

typedef union _ObjectPointer
{
	PlaneParams *SmokingPlane;
	WeaponParams *SmokingWeapon;
	void *SmokingVoid;
} ObjectPointer;

#define SP_STATE_0	0
#define SP_STATE_1	1
#define SP_STATE_2	2
#define SP_STATE_3	3
#define SP_STATE_4	4
#define SP_STATE	7


class SmokePoint
{
	public:
		FPointDouble Location;
		float		 Alpha;
		float		 V;
		WORD		 Data;
		WORD		 TotalTicks;
		WORD		 StateTicks;
		WORD		 Flags;
};

//typedef struct _SmokePoint
//{
//  FPointDouble	Center;
//  float			Alpha;
//  int			Age;
//} SmokePoint;

/*various smoke types */
#define HEAVY_BLACK_SMOKE	0
#define WHITE_MISSILE_SMOKE 1
#define WHITE_WINGTIP_VAPOR 2
#define BROWN_ENGINE_SMOKE	3
#define ENGINE_FIRE			4
#define FUEL_DUMP			5

#define MAX_SMOKE_TYPES     6

#define ST_DEFAULT		0
#define ST_NOLIGHT  	1
#define ST_ADD			2
#define ST_NO_FADEIN	4
#define ST_HOT			8

#define ST_FIRE_FLAGS   (ST_NOLIGHT | ST_ADD)

class SmokeType
{
	public:
		DWORD 		Flags;
		TextureBuff *Texture;
		FPoint		Color;
		float		Alpha;
		float		Radius;
		short		LoopsToFade;
		short		DropRate;
		short		LoopsDormant;
		short		LoopsIn;
		short		LoopsWait;
		short		LoopsRand;
		short		LoopsOut;
		float		VMulter;
		float 		ooLoopsIn;
		float		ooLoopsOut;

	public:
		void Init(TextureBuff *texture,DWORD flags,FPoint &color,float alpha,float stretch,float radius,int time_to_fade,float dormant,float alpha_in,float min_wait,float rand_wait,float alpha_out,short min_ticks = -1);
};

//typedef struct _SmokeType
//{
//	TextureBuff *Texture;
//	FPoint		Color;
//	float		Width;
//	int			LoopsTilFade;
//	int			DropRate;
//}SmokeType;

/* defines for flags variable */

#define ST_ACTIVE	0x00000010
#define ST_ORPHANED 0x00000020
#define ST_OFFSET   0x0000000F
#define ST_VOID     0x80000000
#define ST_PLANE    0x40000000
#define ST_MISSILE  0x20000000

class SmokeTrail
{
	public:
		SmokePoint Points[MAX_POINTS_PER_TRAIL];
		SmokePoint *LastPoint;
		SmokeType  *Type;
		ObjectPointer SmokingObject;
		PointLightSource **Light;
		unsigned int Flags;
		int Ticks;

	public:
		void *NewSmokeTrail(void *object,int offset,int smoke_type,BOOL has_light = FALSE,FPoint *color = NULL,float radius1 = 0.0f, float radius2 = 0.0f);
		void RemoveSmokeTrail();
		void Update(float alpha = 1.0f);
		void UpdateOrphan( void );
		void MakeOrphan();
		void Draw(FPointDouble Eye);
		int Identify( void );
		BOOL operator==(int type);
};

//typedef struct _SmokeTrail
//{
//	SmokePoint Points[MAX_POINTS_PER_TRAIL];
//	SmokePoint *LastPoint;
//	SmokeType  *Type;
//	ObjectPointer SmokingObject;
//	PointLightSource **Light;
//	unsigned int Flags;
//	int GameLoopsTillSwitch;
//	int FinalLoopsTillSwitch;
//} SmokeTrail;

/* sprite type flags */

#define PLANE_EXPLODES    0
#define PLANE_BURNS		  1
#define SPT_FLASH	  	  2
#define SPT_SUN			  3
#define SPT_SKY			  4
#define SPT_WISP		  5
#define SPT_FLARE		  6
#define SPT_AIR_EXP		  7
#define SPT_OILFIRE		  8
#define SPT_WHITE_PUFF	  9
#define SPT_BLACK_PUFF	 10
#define SPT_ALPHA_MAP	 11
#define SPT_SUN_CORONA	 12
#define SPT_MOON		 13
#define SPT_RAINSPOT	 14
#define SPT_2D_OILFIRE	 15
#define MAX_SPRITE_TYPES 16


#define MASK_FADE_TYPE	   0x001
#define TRANSPARENT_TYPE   0x002
#define RANDOM_ORIENTATION 0x100

typedef struct _SpriteType
{
	TextureBuff *Texture;
	int	  Flags;
	int   NumFrames;
	float ULU[16];
	float ULV[16];
	float UWidth;
	float VHeight;
	float RWWidth;
	float RWHeight;
	float FrameMultiplier;
	float YVel;
	float XVel;
	float FramesToFade;
	float OneOverFramesToFade;
	DWORD PolyType;
	//added rgba and flag for use

} SpriteType;

#define GENERATE_SPRITE_EXPLOSION	0

typedef struct _Generator
{
	FPointDouble	Position;
	SpriteType *Type;
	FPoint Vel;
	float Frequency;
	float FreqRem;
	float LoopsLeft;
	void *Plane;
	int   PreviousLoop;
	PointLightSource **Light;
} Generator;

#define ORIENTATION_FLIP_U	     0x8
#define ORIENTATION_FLIP_V	     0x4
#define ORIENTATION_ROTATION     0x3
#define ORIENTATION_ROTATE_0	 0x0
#define ORIENTATION_ROTATE_90	 0x1
#define ORIENTATION_ROTATE_180   0x2
#define ORIENTATION_ROTATE_270   0x3

typedef struct _Sprite
{
	ObjectSortHeader *SortHeader;
	SpriteType *Type;
	FPointDouble Position;
	FPoint Rotated;
	int Orientation;
	int Frame;
	float Fade;
	unsigned int FirstLoopNumber;
} Sprite;


extern SmokeTrail SmokeTrails[],*LastSmokeTrail;
extern SmokeType SmokeTypes[];
extern float SmokeUs[2];

#define MAX_GENERATORS		20
#define MAX_SPRITES			100

extern SpriteType SpriteTypes[MAX_SPRITE_TYPES];
extern Generator Generators[MAX_GENERATORS],*LastGenerator;
extern Sprite Sprites[MAX_SPRITES],*LastSprite;

//void *NewSmokeTrail(void *object,int offset,int smoke_type,BOOL has_light = FALSE,FPoint *color = NULL,float radius1 = 0.0f, float radius2 = 0.0f);
//void RemoveSmokeTrail(SmokeTrail *trail);
//void UpdateSmokeTrail(void *nada);
void UpdateOrphanedSmokeTrails(void);
////int CompareSmokeDrawerWithPoint(SmokeDrawer *drawer,const FPointDouble &point,int relative);
//int SortSmokeTrail(ObjectSortHeader *tree,ObjectSortHeader *object);
void DisplaySmokeTrails(CameraInstance *camera);
////void DrawSmokeTrail(SmokeDrawer *drawer);
//void MakeOrphan(void *nada);
void FreeGenerator(Generator *generator);
void *NewGenerator(int sprite_type,const FPointDouble &start_position,const FPointDouble &velocity,float frequency,int loops_left);
void FreeSprite(Sprite *sprite);
void *NewSprite(SpriteType *type,const FPointDouble &position,unsigned int loop_number = 0,int orientation=0);
void MoveGenerators(void);
void MoveSprites(void);
void DisplaySprites(CameraInstance *camera);
void DrawSprite(Sprite *sprite);

#endif