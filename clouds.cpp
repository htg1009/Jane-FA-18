// Lod
// Generate Clouds using parametric alpha textures and spheres.
#include "F18.h"
#include "clouds.h"
#include "gamesettings.h"
#include "particle.h"

// Draw cloud
#define MAX_SPHERES_PER_CLOUD	32
#define MAX_CLOUDS_PER_WORLD	72
#define MAX_SEGMENTS			8.0f
#define MAX_CLOUDS_PER_FRAME	64
#define MAX_PTS_PER_SLICE		32

//extern TextureBuff *pAdiTexture;
extern TextureBuff *pLightningTexture, *pLightningTexture1, *pLightningTexture2, *pLightningTexture3;

void Render2DSpriteSX( Sprite *spt, float r, float g, float b, float a, float scale, int sx, int sy);
void DrawLightCloudMaps( RotPoint3D **lpoints,int NumPoints,FPoint &normal, DWORD light_bits);
void SoundThunder(int iVolume, int iPan, int iMode);
void DrawUnlitAddBillboardRel(FPoint &btm,FPoint &top,float width,TextureBuff *texture,float alpha = 1.0f,FPoint *color = NULL,BOOL mip_maps = FALSE);
void RemoveLightning( void);







// Clouds.
typedef struct _gpSphere
{
	FPoint Position;

	float radius;
	float segments;
	float hemisphere;

}gpSphere;

typedef struct _gpCloud
{

	int	NumSpheres;
	FPoint Position;
	FPoint GridOffset;
	float BoundingRadius;								// determing Visiblity
	gpSphere Sphere[MAX_SPHERES_PER_CLOUD];
}gpCloud;


typedef struct _SpherePrimative
{
	float EdgePts;
	int Segments;
	RotPoint3D  SpherePoints[ MAX_PTS_PER_SLICE * MAX_PTS_PER_SLICE];

}SpherePrimative;

SpherePrimative  SphereModel;								// Currently Built Sphere.




void DrawPoly(GrBuff *dest, int numpts, RotPoint3D **points );
void DrawPolyNew(GrBuff *dest, int numpts, RotPoint3D **ppPoints, int gop);

void RenderCloud( gpCloud *pCloud, int segments, float alpha);
void RenderFlatClouds( float CloudAlt, float alpha, float tscale, int ShowLightning=0);


void LoadCloudTexture( void);
void DrawSphere( float Radius, int segments, float hemisphere, FPoint &RelPosition);
void DrawSpherePreBuilt( float Radius,  float hemisphere, FPoint &RelPosition, float alpha, FPoint diffuse);

void CalculateLighting( FPoint &Position, FPoint &Normal,float radius, FPoint &diffuse );
void CalculateViewLighting( FPoint &Position, FPoint &Normal,float radius, FPoint &diffuse );

void BuildSphere( int segments );

void DisplayRain( BYTE density );
void RenderCloudWall( float KA );
void DrawCloudWall( float alpha, float tscale, float uoffset, float voffset, float Ka=1.0f, char Inverse=0 );



//Lightning
void AddLightning( FPoint wposition, float scale );
void MoveLightning( void);
void DisplayLighningSprite(FPointDouble &top, FPointDouble &bottom);


void PlayThunderSound( FPoint WorldPosition, int iMode=0);
extern BOOL PushBackFog;



typedef struct _CloudSort
{
	gpCloud *pCloud;
	float	reldist;
	_CloudSort *pBack;
	_CloudSort *pFront;

}CloudSort;

CloudSort ClouldSortList[ MAX_CLOUDS_PER_FRAME];



gpCloud WorldClouds[ MAX_CLOUDS_PER_WORLD];


TextureBuff *pCloudTexture=NULL;
TextureBuff *pCloudFlatTexture=NULL;

BOOL SnowOn, doLightning=0, doRain = 0;

int cloudalt, cloudalt2;
DWORD cloudlightbits;

typedef struct _tLightning
{
	float Timer;
	char Active;
	int Flag;
	float Time;
	float TotalTime;
	float vFlicker;
	float EmmisonRate;
	PointLightSource **ppLight;
	PointLightSource **ppLightLow;
}tLightning;

tLightning Lightning;



float MaxDist	  = 8 MILES;
float FadeOutDist = 1 MILES;
float CloudGridSize = 16 MILES;//32 MILES;//64 MILES;

float uscale, vscale, uoffset, voffset;



void SetUVScaleOffset( float Uscale, float Uoffset, float Vscale, float Voffset)
{
	uscale = Uscale;
	vscale = Vscale;
	uoffset = Uoffset;
	voffset = Voffset;

}




//Note This should be fast enought to render on the fly.. but we might want to speed it up a bit by saving the pts.
// or just doing one LOD sphere for a cloud group and scale it for each cloud

void LoadCloudTexture( void)
{
	TextureRef temp_ref;
	DDCOLORKEY key;

	memset(&key,0,sizeof(DDCOLORKEY));


	ZeroMemory(&temp_ref, sizeof(temp_ref));
	temp_ref.CellColumns = 0;
	temp_ref.TotalCells = 0;
	temp_ref.CellWidth = 1.0f;
	temp_ref.CellHeight = 1.0f;
	sprintf( temp_ref.Name, "Cloud00.pcx");//Cloud01

	bNoAlphaFilter = TRUE;
	SetTextureFormat( FT_16BIT_DATA );
	pCloudTexture = Load3DTexture(&temp_ref, UT_PURE_ALPHA);
	//pCloudTexture = Load3DTexture(&temp_ref, UT_DEFAULT);

	if (WorldParams.Weather & WR_FLATCLOUD_OVERCAST) //Includes Stormy
		sprintf( temp_ref.Name, "cloudoc.pcx");//0a, 3a, 8a
	else
	{

		if (WorldParams.Weather & WR_FLATCLOUD_SCATTERED)
			sprintf( temp_ref.Name, "cloudsc.pcx");//0a, 3a, 8a
		else
			sprintf( temp_ref.Name, "cloud0a.pcx");//0a, 3a, 8a
	}

	pCloudFlatTexture = Load3DTexture(&temp_ref, UT_DEFAULT);//UT_PURE_ALPHA

	bNoAlphaFilter = FALSE;

	ReSetTextureFormat();


}

void ShutdownClouds( void)
{

	if( pCloudTexture )
		Free3DTexture(pCloudTexture);
	pCloudTexture  = NULL;

	if( pCloudFlatTexture )
		Free3DTexture(pCloudFlatTexture );
	pCloudFlatTexture = NULL;


	RemoveLightning( );


}


void InitClouds( void )
{

	doRain = doLightning = SnowOn = 0;
	ZeroMemory( &Lightning, sizeof(tLightning) );

	if( (g_Settings.gr.dwGraph & GP_GRAPH_RAIN_SNOW )  && (WorldParams.Weather & WR_FLATCLOUD_STORMY) == WR_FLATCLOUD_STORMY)
	{
		if( frand() < 0.90f )						// 90% chance of rain
			doRain = 1;
		if( doRain && (frand() < 0.20f) )		// 20% chance of snow
			SnowOn = 1;

		if( (g_Settings.gr.dwGraph &GP_GRAPH_LIGHTNING) && doRain && !SnowOn )
			doLightning=1;
	}


	// Initialize the clouds some how.  Big Small etc.
	// Might want to create a quick tool to do this
	int i;
	float rx, rz;
	float rradius;
	float MaxSize = 300 METERS;
	float MaxRadius = 400 METERS;
	FPoint Pos;
	float wx, wz;


	//TEMP UNTIL IN CONFIG SCREEN
	CloudGridSize = 32 MILES;

//	char *res = GetRegValue( "Clouds" );

//	if( res )
//	{
//		if( !strcmp( res, "Max"))
//			CloudGridSize = 16 MILES;
//		else
//			CloudGridSize = 32 MILES;

//		doClouds =1;
//	}



//	WorldParams.CloudAlt = 8000 FEET;
//	if( GetRegValue( "CloudAlt" ) )
//	{
//		int alt  = GetRegValueL( "CloudAlt" );
//		WorldParams.CloudAlt  = alt FEET;
//	}



	gpCloud *pCloud = &WorldClouds[ 0];
	ZeroMemory( WorldClouds, sizeof( WorldClouds) );
	gpSphere *pSphere;
/*
	wx = wz =  0;
	for( i=0; i< MAX_CLOUDS_PER_WORLD; i++,pCloud++)
	{
		wx = frand() * CloudGridSize;
		wz = frand() * CloudGridSize;

		Pos.SetValues( wx, WorldParams.CloudAlt, wz);
		pCloud->NumSpheres = 16;// 8 ;
		pCloud->BoundingRadius = 0.0f; //Not in use
		pCloud->Position = Pos;
		pSphere = pCloud->Sphere;
		for( int i=0; i<pCloud->NumSpheres; i++)
		{
			rx = (frand() - 0.5f) * 2.0f;
			rz = (frand() - 0.5f) * 2.0f;
			rradius = frand();

			pSphere->Position = Pos;
			pSphere->Position.X += rx * MaxRadius;
			pSphere->Position.Z += rz * MaxRadius;
			pSphere->radius = rradius * MaxSize + (100 METERS);
			pSphere->Position.Y += pSphere->radius ;// adjust for same height
			pSphere++;
		}

	}
*/
	wx = wz =  0;
	float scale;
	scale = 1.0f;
	scale = frand();
	if( scale < 0.25)
		scale += 1.2;
	else
		scale = 1.0f;


	for( i=0; i< MAX_CLOUDS_PER_WORLD; i++,pCloud++)
	{

		wx = frand() * CloudGridSize;
		wz = frand() * CloudGridSize;

		Pos.SetValues( wx, (float)(WorldParams.CloudAlt- (3000.0f FEET)), wz);
		//Pos.SetValues( wx, (float)(10000.0f FEET), wz);

		pCloud->NumSpheres =  14;//12;//
		pCloud->BoundingRadius = 0.0f; //Not in use
		pCloud->Position = Pos;
		pSphere = pCloud->Sphere;
		for( int i=0; i<pCloud->NumSpheres; i++)
		{
			rx = (frand() - 0.5f) * 2.0f;
			rz = (frand() - 0.5f) * 2.0f;
			rradius = frand();

			pSphere->Position = Pos;
			pSphere->Position.X += rx * MaxRadius;
			pSphere->Position.Z += rz * MaxRadius;
			pSphere->radius = rradius * MaxSize + (100 METERS);
			pSphere->radius *= scale;
			pSphere->Position.Y += pSphere->radius ;// adjust for same height
			pSphere++;
			Pos.Z += pSphere->radius;

		}

	}


}

CloudSort *GetSortHeader( void )
{
	CloudSort *pSort, *pLast;

	pSort = &ClouldSortList[0];
	pLast = &ClouldSortList[MAX_CLOUDS_PER_FRAME];

	while( pSort< pLast)
	{
		if( !pSort->pCloud)
			return pSort;
		pSort++;
	}
	return NULL;

}

void InsertSort( CloudSort *pNew, CloudSort *pList)
{
	if( pNew->reldist > pList->reldist)
	{
		if( !pList->pBack)
		{
			pList->pBack = pNew;
			return;
		}
		else
			InsertSort( pNew, pList->pBack);
	}
	else
	{
		if( !pList->pFront)
		{
			pList->pFront = pNew;
			return;
		}
		else
			InsertSort( pNew, pList->pFront);
	}


}

void RenderSortedCloud( CloudSort *pSort)
{
	float segs;
	float dist = pSort->reldist;
	float Ka=1.00;		// Ambient Alpha


	if( dist > MaxDist)
	{
		segs = 4;
		Ka = 1.0f -  ( (dist-MaxDist) * 1.0f/(FadeOutDist));
	}
	else
	{
		segs = MAX_SEGMENTS *(1.0f - dist/MaxDist) + 4;
		Ka = 1.0f;
	}

	 BuildSphere(  segs );		// Build 1 sphere for entire cloud group

	//if( RadiusInView( pCloud->Position, pCloud->BoundingRadius) ) /// NEED TO ADD BOunding SPHERE for CLOUD Gruop!!!
		RenderCloud( pSort->pCloud, segs, Ka );


}



void InsertCloud( gpCloud *pCloud, float dist)
{
	CloudSort *pSort, *pNew;

	pSort = &ClouldSortList[0];
	pNew = GetSortHeader( );

	if( pNew )
	{
		pNew->pCloud = pCloud;
		pNew->reldist = dist;
		if( pNew!= pSort )
			InsertSort( pNew, pSort);
	}

}

void RenderList( CloudSort *pSort)
{
	if( pSort->pBack)
		RenderList( pSort->pBack);
	if( pSort->pCloud)
		RenderSortedCloud( pSort );
	if( pSort->pFront)
		RenderList( pSort->pFront);

	pSort->pCloud = NULL;
	pSort->pFront = NULL;
	pSort->pBack  = NULL;

}



void RenderCloudList( void )
{
	//back to front
	RenderList( &ClouldSortList[0]);
}

extern BOOL NoFogging;

void DisplayClouds( CameraInstance *camera )
{

	float Ka, Ka2;
	float dist, dist2;
	float CloudEffectRange=4000 FEET;
	float FadeCloudLayerStart= 200 FEET;
	float FadeCloudLayerDist= 800 FEET;
	int ShowLightning= 0;



	//lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
   	//lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 	   FALSE);
	//NoFogging = TRUE;


	cloudalt = WorldParams.CloudAlt + 2000 FEET;
	cloudalt2 = cloudalt + ( 2000 FEET);



	dist = fabs(camera->CameraLocation.Y -cloudalt);
	if( dist < (FadeCloudLayerDist + FadeCloudLayerStart))
	{
		Ka = (dist-FadeCloudLayerStart);
		if (Ka <= 0)
			Ka = 0;
		else
			Ka *= 1.0f/FadeCloudLayerDist;
	}
	else
		Ka = 1.0f;

	dist2 = fabs(camera->CameraLocation.Y -cloudalt2);
	if( dist2 < (FadeCloudLayerDist+FadeCloudLayerStart))
	{
		Ka2 = (dist2-FadeCloudLayerStart);
		if (Ka2 <= 0)
			Ka2 = 0;
		else
			Ka2 *= 1.0f/FadeCloudLayerDist;
	}
	else
		Ka2 = 1.0f;
	//Ka2 *= 0.75;

	Ka*= Ka;
	Ka*= Ka;

	Ka2 *= Ka2;
	Ka2 *= Ka2;


	if( camera->CameraLocation.Y < cloudalt)//WorldParams.CloudAlt )
	{
		if ((WorldParams.Weather & WR_FLATCLOUD) || (!(g_Settings.gr.dwGraph & GP_GRAPH_VOLUMETRIC_CLOUDS ) && (WorldParams.Weather & WR_PUFFCLOUD_SCATTERED)))
		{
			ShowLightning = doLightning;
			RenderFlatClouds( cloudalt, Ka, 8.0f, ShowLightning );
			RenderFlatClouds( cloudalt2, Ka2, 10.0f, FALSE );

			if( doLightning)
			{
				tLightning *pLightning;
				pLightning  = &Lightning;

				if( (frand()< 0.40f) && pLightning->ppLightLow && pLightning->Time < 0.5f ) //> pLightning->TotalTime)
					DisplayLighningSprite((*(pLightning->ppLight))->WorldPosition, (*(pLightning->ppLightLow))->WorldPosition);
				else
				{
					if( pLightning->ppLightLow )
						RemoveLight((LightSource **)pLightning->ppLightLow );
					pLightning->ppLightLow = NULL;
				}
			}

		}

		if( (g_Settings.gr.dwGraph & GP_GRAPH_VOLUMETRIC_CLOUDS ) && (WorldParams.Weather & WR_PUFFCLOUD_SCATTERED))
			ProcessClouds( );
	}
	else
	{
		FlushAlphaLists();
		ResetAlphaLists();

		if(( g_Settings.gr.dwGraph & GP_GRAPH_VOLUMETRIC_CLOUDS) && (WorldParams.Weather & WR_PUFFCLOUD_SCATTERED))
			ProcessClouds( );

		if ((WorldParams.Weather & WR_FLATCLOUD) || (!(g_Settings.gr.dwGraph & GP_GRAPH_VOLUMETRIC_CLOUDS ) && (WorldParams.Weather & WR_PUFFCLOUD_SCATTERED)))
		{
			if( camera->CameraLocation.Y < cloudalt2)
			{
				tLightning *pLightning;
				pLightning  = &Lightning;

				ShowLightning = doLightning;
				if( (frand()< 0.40f) && pLightning->ppLightLow && pLightning->Time < 0.5f ) //> pLightning->TotalTime)
					DisplayLighningSprite((*(pLightning->ppLight))->WorldPosition, (*(pLightning->ppLightLow))->WorldPosition);
				else
				{
					if( pLightning->ppLightLow )
						RemoveLight((LightSource **)pLightning->ppLightLow );
					pLightning->ppLightLow = NULL;
				}
			}

			//Put check for overcast days... draw a dark flat cloud instead
			//if( overcast && 
			RenderFlatClouds( cloudalt, Ka, 8.0f, ShowLightning);



			ShowLightning = doLightning;
			RenderFlatClouds( cloudalt2 , Ka2, 10.0f, ShowLightning  );
		}
	}

	if (WorldParams.Weather & WR_FLATCLOUD)
	{
		if( dist2< dist)
			dist = dist2; // get close layer
		if( dist < CloudEffectRange )
			RenderCloudWall( 1.0f - (dist/CloudEffectRange) );
	}



	if( doRain )//(WorldParams.Weather & WR_FLATCLOUD_STORMY) == WR_FLATCLOUD_STORMY)
	{

		if( camera->CameraLocation.Y>WorldParams.CloudAlt && camera->CameraLocation.Y< cloudalt)
		{
			float density;
			density = 1.0f -(camera->CameraLocation.Y - WorldParams.CloudAlt)/(cloudalt - WorldParams.CloudAlt);
			density *= 255.0f;
			DisplayRain( density);
		}
		else if( camera->CameraLocation.Y<WorldParams.CloudAlt )
			DisplayRain( 255 );
	}



	//	NoFogging = FALSE;
	//lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 	   TRUE);
	//lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE,		 TRUE);


}


void ProcessClouds( void )
{
	int i;
	FPoint CloudGridOffset;
	FPoint RelPosition;
	gpCloud *pCloud;
	int gx, gy;


	SetUVScaleOffset( 1.0f, 0.0f, 1.0f, 0.0f);	//reset


	ZeroMemory( &ClouldSortList[0], sizeof( ClouldSortList));						// reset head of sort list


	CloudGridOffset.X = ((int)(Camera1.CameraLocation.X/CloudGridSize)) * CloudGridSize;
	CloudGridOffset.Y = 0.0f;
	CloudGridOffset.Z = ((int)(Camera1.CameraLocation.Z/CloudGridSize)) * CloudGridSize;



	if( (Camera1.CameraLocation.X -CloudGridOffset.X ) < ( CloudGridOffset.X+CloudGridSize - Camera1.CameraLocation.X) )
		gx = -CloudGridSize;
	else
		gx = CloudGridSize;

	// draw 3 closets grids
	CloudGridOffset.X += gx;



	pCloud = &WorldClouds[0];
	for( i=0; i< MAX_CLOUDS_PER_WORLD; i++,pCloud++)
	{
		if( pCloud->NumSpheres )
		{
			RelPosition= pCloud->Position;
			pCloud->GridOffset = CloudGridOffset;
			RelPosition +=  CloudGridOffset;

			RelPosition -= Camera1.CameraLocation;
			float dist  = RelPosition.QuickLength();
			if( dist < (MaxDist + FadeOutDist))
				InsertCloud( pCloud, dist);
		}
	}
	RenderCloudList();


	CloudGridOffset.X -= gx;


	/// Y PIECE
	if( (Camera1.CameraLocation.Z -CloudGridOffset.Z ) < ( CloudGridOffset.Z+CloudGridSize - Camera1.CameraLocation.Z) )
		gy = -CloudGridSize;
	else
		gy = CloudGridSize;

	// draw 3 closets grids
	CloudGridOffset.Z += gy;



	pCloud = &WorldClouds[0];
	for( i=0; i< MAX_CLOUDS_PER_WORLD; i++,pCloud++)
	{
		if( pCloud->NumSpheres )
		{
			RelPosition= pCloud->Position;
			pCloud->GridOffset = CloudGridOffset;
			RelPosition +=  CloudGridOffset;

			RelPosition -= Camera1.CameraLocation;
			float dist  = RelPosition.QuickLength();
			if( dist < (MaxDist + FadeOutDist))
				InsertCloud( pCloud, dist);
		}
	}
	RenderCloudList();




	/// CORNER PIECES
	// draw 3 closets grids
	CloudGridOffset.X += gx;
	//CloudGridOffset.Y = 0.0f;
	//CloudGridOffset.Z = ((int)(Camera1.CameraLocation.Z/CloudGridSize)) * (CloudGridSize)+gy;


	pCloud = &WorldClouds[0];
	for( i=0; i< MAX_CLOUDS_PER_WORLD; i++,pCloud++)
	{
		if( pCloud->NumSpheres )
		{
			RelPosition= pCloud->Position;
			pCloud->GridOffset = CloudGridOffset;
			RelPosition +=  CloudGridOffset;

			RelPosition -= Camera1.CameraLocation;
			float dist  = RelPosition.QuickLength();
			if( dist < (MaxDist + FadeOutDist))
				InsertCloud( pCloud, dist);
		}
	}
	RenderCloudList();


	CloudGridOffset.X -= gx;
	CloudGridOffset.Z -= gy;
//	CloudGridOffset.X = ((int)(Camera1.CameraLocation.X/CloudGridSize)) * CloudGridSize;
//	CloudGridOffset.Y = 0.0f;
//	CloudGridOffset.Z = ((int)(Camera1.CameraLocation.Z/CloudGridSize)) * CloudGridSize;


	pCloud = &WorldClouds[0];
	for( i=0; i< MAX_CLOUDS_PER_WORLD; i++,pCloud++)
	{
		if( pCloud->NumSpheres )
		{
			RelPosition= pCloud->Position;
			pCloud->GridOffset = CloudGridOffset;
			RelPosition +=  CloudGridOffset;
			RelPosition -= Camera1.CameraLocation;
			float dist  = RelPosition.QuickLength();
			if( dist < (MaxDist + FadeOutDist))
				InsertCloud( pCloud, dist);
		}
	}
	RenderCloudList();




	//**************
	//RemoveViewpointLights(); // This should be in render.cpp

}


void RenderCloud( gpCloud *pCloud, int segments, float alpha)
{
	FPoint Diffuse;
	FPoint RelPosition;
	float hemi = 1.0f;
	int gop = PRIM_TEXTURE | PRIM_ALPHAIMM| PRIM_NO_Z_WRITE;

	gpSphere *pSphere = pCloud->Sphere;
	CurrentTexture = pCloudTexture;

	Diffuse.SetValues( 0.8f);



	for( int i=0; i< pCloud->NumSpheres; i++)
	{
		RelPosition =  pSphere->Position;
		RelPosition += pCloud->GridOffset ;
		RelPosition -= Camera1.CameraLocation;



		if( RadiusInView( RelPosition, pSphere->radius) )
			DrawSpherePreBuilt( pSphere->radius,  hemi, RelPosition, alpha,  Diffuse, gop);
			//DrawSphere( pSphere->radius, segments, hemi, RelPosition );
		pSphere++;
	}

}


int  VisibleFace( int numpts, RotPoint3D **ppPoints)
{
	FPoint N, A, B;
	FPoint EyeVector;
	float dot;

	//Cross Product
	A =  (*(ppPoints+1))->Rotated;
	A -= (*(ppPoints))->Rotated;
	B =	 (*(ppPoints+2))->Rotated;
	B -= (*(ppPoints))->Rotated;

	N = A;
	N %= B;			//Cross Product

	EyeVector = (*ppPoints)->Rotated;
	EyeVector *= -1.0f;


	// Dot Product
	dot = EyeVector * N;
	if( dot>=0.0f)
		return 0;
	else
		return 1;
}


void BuildSphere( int segments )
{

	int i, j;
	float slicepts;
	float sliceangle, loftangle;
	float angle, radian;
	RotPoint3D firstedge[MAX_PTS_PER_SLICE];
	RotPoint3D *pCurrent, *pFirst;
	float sn, cs;
	float uscale = 1.0f /360.0f;
	float vscale = 1.0f /180.0f;
	float Radius = 1.0f;


	if( segments <4 )						// need atleast 4 to make something
		return;

	SphereModel.EdgePts = slicepts = segments-1;
	SphereModel.Segments = segments;

	if( slicepts >MAX_PTS_PER_SLICE)
		return ;

	loftangle = 360.0f/(float)segments;
	sliceangle= 180.0f/ (float)(slicepts-1); //180.0f* hemisphere

	// Generate first edge which we will loft
	pFirst = &firstedge[0];
	angle = 0;
	for( i=0; i< slicepts; i++)
	{
		radian = angle * (3.14159265359f/180.0f);
		sn = sin(radian);
		cs = cos(radian);
		pFirst->Rotated.X = sn * Radius;
		pFirst->Rotated.Y = cs * Radius;
		pFirst->Rotated.Z = 0.0f;
		pFirst->U = 0.0f;
		pFirst->V = angle * vscale;

		angle += sliceangle;
		pFirst++;
	}

	// loft the first edge to build the polys
	angle = 0.0;//loftangle;
	pCurrent = &SphereModel.SpherePoints[0];
	for( i=0; i< segments; i++)
	{

		// Rotate new edge		// Note We are actually rotating the 2 End pts which we don't neeed to do
								// unlese we use the hemisphere opitoin.
		pFirst = &firstedge[0];
		radian = angle * (3.14159265359f/180.0f);
		for( j=0; j<slicepts; j++)
		{
			sn = sin(radian);
			cs = cos(radian);

			pCurrent->Rotated.Y = pFirst->Rotated.Y;
			pCurrent->Rotated.X = cs * pFirst->Rotated.X;				// loft around y
			pCurrent->Rotated.Z = sn * pFirst->Rotated.X;
			pCurrent->U = angle * uscale;
			pCurrent->V = pFirst->V;

			pCurrent->SP_Rotated = pCurrent->Rotated;		// save the Normal here temporarly
			pCurrent->SP_Rotated.Normalize();//Don't really need this


			pCurrent++;
			pFirst++;
		}

		angle += loftangle;
	}
	//copy first row to last row
	memcpy( pCurrent, SphereModel.SpherePoints, slicepts * sizeof( RotPoint3D) );
	for( i=0; i< slicepts; i++)
		(pCurrent++)->U = 1.0f;



	// Rotate Points
	pCurrent = &SphereModel.SpherePoints[0];
	for( i=0; i< ((segments+1) * slicepts); i++)
	{
		pCurrent->Rotated *= ViewMatrix;
		pCurrent->SP_Rotated *= ViewMatrix; //rotate normals
		pCurrent++;
	}

}


void CalcAlphaAndLighting( RotPoint3D  *pPt, float Radius ,int gop)
{
	float dot;
	FPoint Normal, EyeVector;
	FPoint Diffuse;

	if( pPt->Flags)
		return;

	EyeVector = pPt->Rotated;
	EyeVector *= -1.0f;
	EyeVector.Normalize();

	Normal = pPt->SP_Rotated;			// we stored the normal in here

	//if( FlipSphereNormals )
	//	Normal *= -1.0f;
	dot = Normal * EyeVector;
	if( dot>1.0f)
		dot = 1.0f;

	if( dot>0)
		pPt->Alpha *= dot*dot*dot*dot            ;//* Ka;
	else
		pPt->Alpha = 0.0f;

	Diffuse.SetValues(0.0f);
	CalculateViewLighting( pPt->Rotated, Normal, Radius, Diffuse);
	pPt->Diffuse *= Diffuse;
	pPt->Diffuse.Limit( 1.0f);

	if (gop & PRIM_ALPHA_ADD)
	{
		pPt->Diffuse *= pPt->Alpha;
		pPt->Alpha = 1.0f;
	}
}



void DrawSpherePreBuilt(  float R, float hemisphere, FPoint &RP, float alpha, FPoint d, int gop, FMatrix *pMat)
{
	int i, j;
	RotPoint3D  Sphere[MAX_PTS_PER_SLICE * MAX_PTS_PER_SLICE];// too big for the stack?
	RotPoint3D *pPoint, *pEdge, *pNext;
	RotPoint3D *ppPoints[4];
	FPoint offset;
	float dist;
	int slicepts = SphereModel.EdgePts;
	int segments = SphereModel.Segments;
	FPoint short_pos;
	FPoint Diffuse;

	FPoint &RelPosition = (ImagingMethod & IT_GREEN) ? short_pos : RP;
	float Radius = R;

	if (ImagingMethod & IT_GREEN)
	{
		RelPosition.SetValues(0.8f,RP);
		Radius *= 0.8f;
		PushBackFog = TRUE;

	}


	Diffuse = d;
	if( gop & PRIM_ALPHA_ADD)
	{
		Diffuse *= alpha;
		alpha = 1.0f;
	}

	offset = RelPosition;
	offset *= ViewMatrix;

	dist = RelPosition.QuickLength();
	// mult radius + offset
	pPoint = &SphereModel.SpherePoints[0];
	pEdge = &Sphere[0];


	for( i=0; i< (slicepts * (segments+1)); i++)
	{
		pEdge->Rotated = pPoint->Rotated;
		pEdge->Rotated *= Radius;
		if( pMat)
			pEdge->Rotated *= *pMat;

		pEdge->Rotated += offset;//RelPosition;

		pEdge->U = pPoint->U  * uscale + uoffset;	//scale and offsets are for textures with frames only
		pEdge->V = pPoint->V  * vscale + voffset;

		pEdge->Diffuse = Diffuse;
		pEdge->Alpha = alpha;

		//pEdge->ooz =  dist;			//constantz holder
		pEdge->Flags = 0;
		pEdge->SP_Rotated = pPoint->SP_Rotated;

		if( pMat)
			pEdge->SP_Rotated *= *pMat;


		pEdge++;
		pPoint++;
	}

	//CurrentTexture = pCloudTexture;

	// Draw + Light Polys
	pEdge = &Sphere[0];
	pNext = pEdge+slicepts;
	for( j=0; j<segments; j++ )
	{
		// First and last segs are Tris.. rest are quads
		ppPoints[0] = pEdge;
		ppPoints[1] = pEdge+1;
		ppPoints[2] = pNext+1;

		// calc visibliy
		if( VisibleFace( 3, ppPoints) )
		{
			CalcAlphaAndLighting( ppPoints[0], Radius ,gop);
			CalcAlphaAndLighting( ppPoints[1], Radius ,gop );
			CalcAlphaAndLighting( ppPoints[2], Radius ,gop );
			DrawPolyNew( GrBuffFor3D,  3, ppPoints, gop );
		}
		pEdge++;
		pNext++;

		for( i=1; i< (slicepts-2); i++ )
		{
			ppPoints[0] = pEdge;
			ppPoints[1] = pEdge+1;
			ppPoints[2] = pNext+1;
			ppPoints[3] = pNext;

			// calc visibliy
			if( VisibleFace( 3, ppPoints) )
			{
				CalcAlphaAndLighting( ppPoints[0], Radius ,gop);
				CalcAlphaAndLighting( ppPoints[1], Radius ,gop);
				CalcAlphaAndLighting( ppPoints[2], Radius ,gop);
				CalcAlphaAndLighting( ppPoints[3], Radius ,gop);
				DrawPolyNew( GrBuffFor3D,  4, ppPoints, gop );
			}
			pEdge++;
			pNext++;
		}
		ppPoints[0] = pEdge;
		ppPoints[1] = pEdge+1;
		ppPoints[2] = pNext;

		// calc visibliy
		if( VisibleFace( 3, ppPoints) )
		{
			CalcAlphaAndLighting( ppPoints[0], Radius ,gop);
			CalcAlphaAndLighting( ppPoints[1], Radius ,gop);
			CalcAlphaAndLighting( ppPoints[2], Radius ,gop);
			DrawPolyNew( GrBuffFor3D,  3, ppPoints, gop );
		}
		pEdge++;
		pEdge++;

		pNext++;
		pNext++;
	}

	PushBackFog = FALSE;


}

/*
void DrawSphere( float Radius, int segments, float hemisphere, FPoint &RelPosition)
{
	int i, j;
	float slicepts;
	float sliceangle, loftangle;
	float angle, radian;
	RotPoint3D  firstedge[MAX_PTS_PER_SLICE], EdgeA[MAX_PTS_PER_SLICE], EdgeB[MAX_PTS_PER_SLICE], *pCurrent, *pLast, *pFirst, *pDstPt;
	RotPoint3D *pPoints[8];
	FPoint EyeVector, Normal;
	float sn, cs;
	float dot;
	FPoint offset;
	FPoint tmp;
	float FlipSphereNormals;
	float uscale = 1.0f /360.0f;
	float vscale = 1.0f /180.0f;


	if( segments <4 )						// need atleast 4 to make something
		return;

	slicepts = segments-1;
	if( slicepts >MAX_PTS_PER_SLICE)
		return;

	if( RelPosition.QuickLength() <Radius)
		FlipSphereNormals = -1.0f;
	else
		FlipSphereNormals = 1.0f;

	offset = RelPosition;
	offset *= ViewMatrix;


	loftangle = 360.0f/(float)segments;
	sliceangle= 180.0f/ (float)(slicepts-1); //180.0f* hemisphere


	// Generate first edge which we will loft
	pFirst = &firstedge[0];
	pLast = &EdgeB[0];
	angle = 0;
	for( i=0; i< slicepts; i++)
	{
		radian = angle * (3.14159265359f/180.0f);
		sn = sin(radian);
		cs = cos(radian);
		pFirst->Rotated.X = sn * Radius;
		pFirst->Rotated.Y = cs * Radius;
		pFirst->Rotated.Z = 0.0f;
		pFirst->U = 0.0f;
		pFirst->V = angle * vscale;

		EyeVector = Camera1.CameraLocation;
		tmp = pFirst->Rotated;
		tmp += TmpPos;
		EyeVector -=tmp;
		EyeVector.Normalize();


		Normal.SetValues( sn, cs, 0.0f);		//Normal

		if( FlipSphereNormals == -1)
			Normal *= FlipSphereNormals;
		dot = Normal * EyeVector;
		if( dot>1.0f)
				dot = 1.0f;

		pFirst->oow = dot;					// tenp store dot product here

		if( dot>0)
			pFirst->Alpha = dot*dot*dot* Ka;
			else
			pFirst->Alpha = 0.0f;

		pFirst->Diffuse.SetValues(0.0f);
		CalculateLighting( tmp, Normal, Radius, pFirst->Diffuse);

		pLast->Rotated = pFirst->Rotated;
		pLast->Rotated *= ViewMatrix;
		pLast->Rotated += offset;

		pLast->U =  pFirst->U ;
		pLast->V =  pFirst->V ;
		pLast->Alpha = pFirst->Alpha;
		pLast->Diffuse = pFirst->Diffuse;
		pLast->oow = pFirst->oow ;


		angle += sliceangle;
		pFirst++;
		pLast++;
	}


	// loft the first edge to build the polys
	angle = loftangle;
	pLast = &EdgeB[0];///firstedge[0];
	pCurrent = &EdgeA[0];
	for( i=0; i< segments; i++)
	{

		// Rotate new edge		// Note We are actually rotating the 2 End pts which we don't neeed to do
								// unlese we use the hemisphere opitoin.
		pFirst = &firstedge[0];
		pDstPt = pCurrent;
		radian = angle * (3.14159265359f/180.0f);
		for( j=0; j<slicepts; j++)
		{
			sn = sin(radian);
			cs = cos(radian);

			pDstPt->Rotated.Y = pFirst->Rotated.Y;
			pDstPt->Rotated.X = cs * pFirst->Rotated.X;				// loft around y
			pDstPt->Rotated.Z = sn * pFirst->Rotated.X;
			pDstPt->U = angle * uscale;
			pDstPt->V = pFirst->V;


			EyeVector = Camera1.CameraLocation;
			tmp = pDstPt->Rotated;
			tmp += TmpPos;
			EyeVector -=tmp;
			EyeVector.Normalize();


			Normal = pDstPt->Rotated;//.SetValues( cs, pFirst->oow, sn);		//Normal Directional Cos..oow is the ycos
			Normal.Normalize();

			if( FlipSphereNormals == -1)
				Normal *= FlipSphereNormals;

			dot = Normal * EyeVector;
			if( dot>1.0f)
				dot = 1.0f;

			pDstPt->oow = dot;
			if( dot>0)
				pDstPt->Alpha = dot * dot*dot* Ka;
			else
				pDstPt->Alpha = 0;

			pDstPt->Diffuse.SetValues(0.0f);
			CalculateLighting( tmp, Normal, Radius, pDstPt->Diffuse);
			//pDstPt->Diffuse.SetValues( 0.0f, 0.0f, pDstPt->Alpha); //temp erase me

			pDstPt->Rotated *= ViewMatrix;
			pDstPt->Rotated += offset;

			pDstPt++;
			pFirst++;
		}

		// Draw Polys using pLast and pCurrent
		pDstPt = pCurrent;

		//First one is a tri
		pPoints[0] = pLast;
		pPoints[1] = (pLast+1);
		pPoints[2] = (pDstPt+1);
		//if( (pPoints[0]->Alpha + pPoints[1]->Alpha + pPoints[2]->Alpha )>=0.0f )
		//if( (pPoints[0]->Alpha>=0.0f) && (pPoints[1]->Alpha>=0.0f)&&(pPoints[2]->Alpha>=0.0f) )
		//if( (pPoints[0]->oow + pPoints[1]->oow + pPoints[2]->oow) >=0.0f )
		DrawPoly( GrBuffFor3D,  3, &pPoints[0] );
		pDstPt++;
		pLast++;

		for( j = 1; j<(slicepts-2); j++)
		{
			pPoints[0] = pLast;
			pPoints[1] = (pLast+1);
			pPoints[2] = (pDstPt+1);
			pPoints[3] = pDstPt;

			//if( (pPoints[0]->Alpha + pPoints[1]->Alpha + pPoints[2]->Alpha + pPoints[3]->Alpha )>=0.0f )
			//if( (pPoints[0]->Alpha>=0.0f) && (pPoints[1]->Alpha>=0.0f)&&(pPoints[2]->Alpha>=0.0f)&& (pPoints[3]->Alpha>=0.0f) )
		//	if( (pPoints[0]->oow + pPoints[1]->oow + pPoints[2]->oow + pPoints[3]->oow) >=0.0f )
				DrawPoly( GrBuffFor3D,  4, &pPoints[0] );

			pLast++;
			pDstPt++;
		}
		//last one is a tri
		pPoints[0] = pLast;
		pPoints[1] = (pLast+1);
		pPoints[2] = pDstPt;
		//if( (pPoints[0]->Alpha + pPoints[1]->Alpha + pPoints[2]->Alpha )>=0.0f )
		//if( (pPoints[0]->Alpha>=0.0f) && (pPoints[1]->Alpha>=0.0f)&&(pPoints[2]->Alpha>=0.0f) )
		//if( (pPoints[0]->oow + pPoints[1]->oow + pPoints[2]->oow) >=0.0f )
			DrawPoly( GrBuffFor3D,  3, &pPoints[0] );

		angle += loftangle;
		pLast = pCurrent;
		if( pCurrent == &EdgeA[0])
			pCurrent = &EdgeB[0];
		else
			pCurrent = &EdgeA[0];

	}

}
*/

void CalculateViewLighting( FPoint &Position, FPoint &Normal,float radius, FPoint &diffuse )
{

	FPoint specular(0.0f);
	LightSource **light_walker = GlobalViewpointLights;


	while(light_walker <= LastViewpointLight)
	{
		if (*light_walker)
			(*light_walker)->CalculateCloudLight(Position, Normal, radius,diffuse );
		light_walker++;
	}
	diffuse += specular;
	diffuse.Limit(1.0f);


}


void CalculateLighting( FPoint &Position, FPoint &Normal,float radius, FPoint &diffuse )
{

	FPoint specular(0.0f);
	LightSource **light_walker = GlobalLights;


	while(light_walker <= LastLight)
	{
		if (*light_walker)
			(*light_walker)->CalculateCloudLight(Position, Normal, radius,diffuse );
		light_walker++;
	}
	diffuse += specular;
	diffuse.Limit(1.0f);


}



void DrawPolyNew(GrBuff *dest, int numpts, RotPoint3D **ppPoints, int gop)
{
	RotPoint3D *pPoint, **ppPt;
	int clip_flags,draw_flags;


	clip_flags = 0;
	draw_flags = -1;


	ppPt = ppPoints;
	pPoint = *ppPt;
	for( int i=0; i<numpts; i++)
	{
		if( !pPoint->Flags )
		{
			if (pPoint->Rotated.Z > ZClipPlane)
				pPoint->Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				pPoint->Flags = PT3_ROTATED;
				PerspectPoint(pPoint);
			}
			//pPoint->Diffuse.SetValues( (const float)red, (const float)green, (const float)blue);
			pPoint->Specular.SetValues( 0.0f);
			//pPoint->Alpha = 1.0f;
			pPoint->oow  = 1.0f;
			pPoint->ooz  = 1.0f;
			pPoint->fog  = 1.0f;
		}
		clip_flags |= pPoint->Flags;
		draw_flags &= pPoint->Flags;

		ppPt++;
		pPoint = *ppPt;
	}


	if (!(draw_flags & PT3_NODRAW))
			DrawPolyClipped(gop | PRIM_ZFUNC_LE,CL_CLIP_UV,numpts, (RotPoint3D **)ppPoints, clip_flags & PT3_CLIP_FLAGS);
}


void CalcSegsAlpha( float dist, float *pSegs, float *pAlpha)
{

	if( dist > ( MaxDist + FadeOutDist) )
		*pSegs = *pAlpha = 0.0f;
	else if( dist > MaxDist)
	{
		*pSegs = 4;
		*pAlpha = 1.0f -  ( (dist-MaxDist) * 1.0f/(FadeOutDist));
	}
	else
	{
		*pSegs = MAX_SEGMENTS *(1.0f - dist/MaxDist) + 4;
		*pSegs = 8*(1.0f - dist/MaxDist) + 4;

		*pAlpha = 1.0f;
	}
}




//-----------------------------------------------
// Flat Plane Cloud Stuff------------------------
//-----------------------------------------------


#define  SKYMAXZ	(float)(LP_LOW_MAX_DISTANCE  + TILE_SIZE*2.0f)


typedef struct _SkyPoint
{
	float  X;
	float  Z;
	float  Alpha;
	float  Tx;
	float  Ty;
}SkyPoint;

#define NUM_SKY_PTS			13
SkyPoint SkyPoints[NUM_SKY_PTS]=
{
	{  -64.0f,  110.0f,   0.0f,	 64.0f, 18.0f },	//0
	{   64.0f,  110.0f,   0.0f,	192.0f, 18.0f },	//1

	{  -48.0f,   83.0f,  0.0f,	 80.0f, 45.0f },	//2
	{   48.0f,   83.0f,  0.0f,	176.0f, 45.0f },	//3

	{ -128.0f,    0.0f,  0.0f,	  0.0f, 128.0f},	//4
 	{  -96.0f,    0.0f,  0.0f,	 32.0f, 128.0f},	//5
	{    0.0f,    0.0f,  1.0f,	128.0f, 128.0f},	//6
  	{   96.0f,    0.0f,  0.0f,	224.0f, 128.0f},	//7
  	{  128.0f,    0.0f,  0.0f,	255.0f, 128.0f},	//8

 	{  -48.0f,  -83.0f,  0.0f,	 80.0f, 211.0f},	//9
 	{   48.0f,  -83.0f,  0.0f,	176.0f, 211.0f},	//10

 	{  -64.0f, -110.0f,  0.0f,  64.0f, 238.0f},	//11
 	{   64.0f, -110.0f,  0.0f,	192.0f, 238.0f}		//12

};

/*old
SkyPoint SkyPoints[NUM_SKY_PTS]=
{
	{  -64.0f,  110.0f,   0.0f,	 64.0f, 18.0f },	//0
	{   64.0f,  110.0f,   0.0f,	192.0f, 18.0f },	//1

	{  -48.0f,   83.0f,  1.0f,	 80.0f, 45.0f },	//2
	{   48.0f,   83.0f,  1.0f,	176.0f, 45.0f },	//3

	{ -128.0f,    0.0f,  0.0f,	  0.0f, 128.0f},	//4
 	{  -96.0f,    0.0f,  1.0f,	 32.0f, 128.0f},	//5
	{    0.0f,    0.0f,  1.0f,	128.0f, 128.0f},	//6
  	{   96.0f,    0.0f,  1.0f,	224.0f, 128.0f},	//7
  	{  128.0f,    0.0f,  0.0f,	255.0f, 128.0f},	//8

 	{  -48.0f,  -83.0f,  1.0f,	 80.0f, 211.0f},	//9
 	{   48.0f,  -83.0f,  1.0f,	176.0f, 211.0f},	//10

 	{  -64.0f, -110.0f,  0.0f,  64.0f, 238.0f},	//11
 	{   64.0f, -110.0f,  0.0f,	192.0f, 238.0f}		//12
};
*/

typedef struct _SkyPoly
{
	unsigned char pt0;
	unsigned char pt1;
	unsigned char pt2;
}SkyPoly;

#define NUM_SKY_POLYS	6//18
SkyPoly SkyPolyList[NUM_SKY_POLYS]=
{
// Outside Alphas
	/*
	{0,1,3},
	{0,3,2},

	{1,8,7},
	{1,7,3},

	{8,12,10},
	{8,10,7},

	{12,11,9},
	{12,9,10},

	{11,4,5},
	{11,5,9},

	{4,0,2},
	{4,2,5},
	*/

	//Inside Soild Normal
	{2,3,6},
	{3,7,6},
	{7,10,6},
	{10,9,6},
	{9,5,6},
	{5,2,6}

};



void  DrawSkyPoly( RotPoint3D *pt0, RotPoint3D *pt1, RotPoint3D *pt2, int levels)
{
	int draw_flags;
	RotPoint3D newpt;
	FPoint tmp;
	float len0, len1, len2;
	int maxsubdiv = 4;
	FPoint normal;
	RotPoint3D *points[4];



	draw_flags = pt0->Flags & pt1->Flags & pt2->Flags;

	if (!(draw_flags & PT3_NODRAW))
	{
		//Subdive tris
		if( levels < maxsubdiv)
		{
			len0 = pt0->Rotated ^ pt1->Rotated;
			len1 = pt1->Rotated ^ pt2->Rotated;
			len2 = pt2->Rotated ^ pt0->Rotated;

			if( len2> len0 && len2> len1)
			{
				newpt.Rotated.X =  (pt0->Rotated.X - pt2->Rotated.X) *0.5f + pt2->Rotated.X;
				newpt.Rotated.Y =  (pt0->Rotated.Y - pt2->Rotated.Y) *0.5f + pt2->Rotated.Y;
				newpt.Rotated.Z =  (pt0->Rotated.Z - pt2->Rotated.Z) *0.5f + pt2->Rotated.Z;

				newpt.Diffuse.X =  (pt0->Diffuse.X - pt2->Diffuse.X) *0.5f + pt2->Diffuse.X;
				newpt.Diffuse.Y =  (pt0->Diffuse.Y - pt2->Diffuse.Y) *0.5f + pt2->Diffuse.Y;
				newpt.Diffuse.Z =  (pt0->Diffuse.Z - pt2->Diffuse.Z) *0.5f + pt2->Diffuse.Z;


				newpt.Alpha =  (pt0->Alpha- pt2->Alpha) *0.5f + pt2->Alpha;
				newpt.U =  (pt0->U- pt2->U) *0.5f + pt2->U;
				newpt.V =  (pt0->V- pt2->V) *0.5f + pt2->V;

				newpt.Specular.SetValues(0.0f);
				newpt.oow  = 1.0f;
				newpt.ooz  = 1.0f;
				newpt.fog  = 1.0f;

				if (newpt.Rotated.Z > ZClipPlane)
					newpt.Flags = PT3_RZHIGH | PT3_ROTATED;
				else
				{
					newpt.Flags = PT3_ROTATED;
					PerspectPoint( &newpt );
				}



				levels++;
				DrawSkyPoly( pt0,		pt1,	&newpt, levels);
				DrawSkyPoly( &newpt,	pt1,	pt2,	levels);

			}
			else if( len0 >len1)
			{
				newpt.Rotated.X =  (pt0->Rotated.X - pt1->Rotated.X) *0.5f + pt1->Rotated.X;
				newpt.Rotated.Y =  (pt0->Rotated.Y - pt1->Rotated.Y) *0.5f + pt1->Rotated.Y;
				newpt.Rotated.Z =  (pt0->Rotated.Z - pt1->Rotated.Z) *0.5f + pt1->Rotated.Z;

				newpt.Diffuse.X =  (pt0->Diffuse.X - pt1->Diffuse.X) *0.5f + pt1->Diffuse.X;
				newpt.Diffuse.Y =  (pt0->Diffuse.Y - pt1->Diffuse.Y) *0.5f + pt1->Diffuse.Y;
				newpt.Diffuse.Z =  (pt0->Diffuse.Z - pt1->Diffuse.Z) *0.5f + pt1->Diffuse.Z;


				newpt.Alpha =  (pt0->Alpha- pt1->Alpha) *0.5f + pt1->Alpha;
				newpt.U =  (pt0->U- pt1->U) *0.5f + pt1->U;
				newpt.V =  (pt0->V- pt1->V) *0.5f + pt1->V;

				newpt.Specular.SetValues(0.0f);
				newpt.oow  = 1.0f;
				newpt.ooz  = 1.0f;
				newpt.fog  = 1.0f;

				if (newpt.Rotated.Z > ZClipPlane)
					newpt.Flags = PT3_RZHIGH | PT3_ROTATED;
				else
				{
					newpt.Flags = PT3_ROTATED;
					PerspectPoint( &newpt );
				}


				levels++;
				DrawSkyPoly( pt0,		pt2,	&newpt, levels);
				DrawSkyPoly( &newpt,	pt2,	pt1,	levels);

			}
			else
			{
				newpt.Rotated.X =  (pt2->Rotated.X - pt1->Rotated.X) *0.5f + pt1->Rotated.X;
				newpt.Rotated.Y =  (pt2->Rotated.Y - pt1->Rotated.Y) *0.5f + pt1->Rotated.Y;
				newpt.Rotated.Z =  (pt2->Rotated.Z - pt1->Rotated.Z) *0.5f + pt1->Rotated.Z;

				newpt.Diffuse.X =  (pt2->Diffuse.X - pt1->Diffuse.X) *0.5f + pt1->Diffuse.X;
				newpt.Diffuse.Y =  (pt2->Diffuse.Y - pt1->Diffuse.Y) *0.5f + pt1->Diffuse.Y;
				newpt.Diffuse.Z =  (pt2->Diffuse.Z - pt1->Diffuse.Z) *0.5f + pt1->Diffuse.Z;


				newpt.Alpha =  (pt2->Alpha- pt1->Alpha) *0.5f + pt1->Alpha;
				newpt.U =  (pt2->U- pt1->U) *0.5f + pt1->U;
				newpt.V =  (pt2->V- pt1->V) *0.5f + pt1->V;

				newpt.Specular.SetValues(0.0f);
				newpt.oow  = 1.0f;
				newpt.ooz  = 1.0f;
				newpt.fog  = 1.0f;

				if (newpt.Rotated.Z > ZClipPlane)
					newpt.Flags = PT3_RZHIGH | PT3_ROTATED;
				else
				{
					newpt.Flags = PT3_ROTATED;
					PerspectPoint( &newpt );
				}


				levels++;
				DrawSkyPoly( pt1,		pt0,	&newpt, levels);
				DrawSkyPoly( &newpt,	pt0,	pt2,	levels);

			}
		}
		else
		{
			int poly_type;
			int clip_type, clip_flags;

			points[0] = pt0;
			points[1] = pt1;
			points[2] = pt2;

			clip_flags = pt0->Flags | pt1->Flags | pt2->Flags;
			clip_type = CL_CLIP_UV;

			if (WorldParams.Weather & WR_FLATCLOUD_OVERCAST) //Includes Stormy
				poly_type =PRIM_TEXTURE |PRIM_NO_Z_WRITE | PRIM_ALPHAIMM;//
			else
				poly_type =PRIM_TEXTURE |PRIM_NO_Z_WRITE |PRIM_ALPHAIMM| PRIM_ALPHA_ADD;//

			CurrentTexture = pCloudFlatTexture;//pAdiTexture;//


			DrawPolyClipped(poly_type, clip_type, 3, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);

			if( doLightning)
			{
				normal.SetValues( 0.0f, -1.0f, 0.0f);
				normal *= ViewMatrix;
				DrawLightCloudMaps( (RotPoint3D **)&points[0], 3, normal, cloudlightbits);

			}


		}

	}

}


void RenderFlatClouds( float CloudAlt, float Ka, float tscale, int ShowLightning )
{

	RotPoint3D *pPt3D, Points[NUM_SKY_PTS];
	float relalt;
	SkyPoly *pSkyPoly;
	SkyPoint *pSkyPt;
	float Scale = SKYMAXZ/128.0f;
	float ooscale = 1.0f/255.0f;
	float oldRad1[32], oldRad2[32];
	FPoint oldLPos[32];
	tLightning *pLightning;
	pLightning  = &Lightning;



	if( 1)
	{

		relalt = (float)( CloudAlt - Camera1.CameraLocation.Y);//(float)( WorldParams.CloudAlt - Camera1.CameraLocation.Y);

		pSkyPt = &SkyPoints[0];
		pPt3D  = &Points[0];

		// Rotate Points
		while( pSkyPt < &SkyPoints[NUM_SKY_PTS] )
		{

			pPt3D->Rotated.X = pSkyPt->X;
			pPt3D->Rotated.Y = relalt;
			pPt3D->Rotated.Z = pSkyPt->Z;

			pPt3D->Rotated.X *= Scale;
			pPt3D->Rotated.Z *= Scale;

			pPt3D->Rotated *= 0.05f;


			pPt3D->U = pSkyPt->Tx;
			pPt3D->V = pSkyPt->Ty;

			pPt3D->U *=  ooscale;
			pPt3D->V *=  ooscale;


			pPt3D->U *= tscale;
			pPt3D->V *= tscale;


			pPt3D->U += (tscale/(SKYMAXZ)) *	fmod( Camera1.CameraLocation.X, (double)(SKYMAXZ) );
			pPt3D->V -= (tscale/(SKYMAXZ)) *	fmod( Camera1.CameraLocation.Z, (double)(SKYMAXZ) );


			if (WorldParams.Weather & WR_FLATCLOUD_OVERCAST) //Includes Stormy
			{
				pPt3D->Diffuse = NaturalLight;
				pPt3D->Diffuse.Limit(1.0f);
				pPt3D->Alpha = pSkyPt->Alpha *Ka;
			}
			else
			{
				pPt3D->Diffuse = NaturalLight;
				pPt3D->Diffuse *= pSkyPt->Alpha;// * Ka;
				pPt3D->Diffuse *= Ka;
				pPt3D->Diffuse.Limit(1.0f);
				pPt3D->Alpha = 1.0f;//pSkyPt->Alpha;
			}


			pPt3D->Specular.SetValues(0.0f);

			pPt3D->oow  = 1.0f;
			pPt3D->ooz  = 1.0f;
			pPt3D->fog  = 1.0f;

			pPt3D->Rotated *= ViewMatrix;

			if (pPt3D->Rotated.Z > ZClipPlane)
				pPt3D->Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				pPt3D->Flags = PT3_ROTATED;
				PerspectPoint(pPt3D);
			}
			pPt3D++;
			pSkyPt++;
		}


		if( ShowLightning)
		{
			cloudlightbits= 0;
			int flag = 1;
			int i=0;

			LightSource **swalker;

			swalker = GlobalViewpointLights+1; //this skips the sky and sun lights

			while((swalker <= LastViewpointLight) && flag)
			{
				if( (*swalker)->Type == LIGHT_POINT  && fabs(((PointLightSource *)((*swalker)->Parent))->WorldPosition.Y - CloudAlt) < ((PointLightSource *)(*swalker))->Radius1)
				{
					oldRad1[i] = ((PointLightSource *)(*swalker))->Radius1;
					oldRad2[i] = ((PointLightSource *)(*swalker))->Radius2;
					oldLPos[i] = ((PointLightSource *)(*swalker))->WorldPosition;
					cloudlightbits|= flag;


					if( pLightning->ppLight && ((PointLightSource *)(*swalker))->Parent == *(pLightning->ppLight) )
					{
						((PointLightSource *)(*swalker))->Radius1  *= 0.50f;
						((PointLightSource *)(*swalker))->Radius2  *= 0.50f;
						((PointLightSource *)(*swalker))->WorldPosition *= 0.05f;
						((PointLightSource *)(*swalker))->ooRadius1  *= 2.0f;
						((PointLightSource *)(*swalker))->ooRadius2  *= 2.0f;

					}
					else
					{
						((PointLightSource *)(*swalker))->Radius1  *= 0.05f;
						((PointLightSource *)(*swalker))->Radius2  *= 0.05f;
						((PointLightSource *)(*swalker))->WorldPosition *= 0.05f;
						((PointLightSource *)(*swalker))->ooRadius1  *= 20.0f;
						((PointLightSource *)(*swalker))->ooRadius2  *= 20.0f;
					}
				}
				i++;
				flag <<=1;
				swalker++;
			}



		}

		// Render Polys
		pSkyPoly = &SkyPolyList[0];
		while( pSkyPoly < &SkyPolyList[NUM_SKY_POLYS])
		{
			DrawSkyPoly( &Points[pSkyPoly->pt0], &Points[pSkyPoly->pt1], &Points[pSkyPoly->pt2], 0);
			pSkyPoly++;
		}



		if( ShowLightning)
		{
			int flag = 1;
			int i=0;


			LightSource **swalker;

			swalker = GlobalViewpointLights+1; //this skips the sky and sun lights

			while( 	cloudlightbits )
			{
				if( cloudlightbits & 1)
				{
					((PointLightSource *)(*swalker))->Radius1  =	oldRad1[i];
					((PointLightSource *)(*swalker))->Radius2  =	oldRad2[i];
					((PointLightSource *)(*swalker))->WorldPosition = oldLPos[i];
					((PointLightSource *)(*swalker))->ooRadius1  = 1.0f/((PointLightSource *)(*swalker))->Radius1;
					((PointLightSource *)(*swalker))->ooRadius2  = 1.0f/((PointLightSource *)(*swalker))->Radius2;

				}
				i++;
				cloudlightbits >>=1;
				swalker++;
			}


		}

	}
}


/*
void SetCloudFog( void )
{
//	return;
//	float relalt;

//	relalt = (float)( WorldParams.CloudAlt - Camera1.CameraLocation.Y);
//	relalt = fabs( relalt);
//	if( relalt < 1000 FEET)
//	{
//		WorldParams.FogNearZ = 0.0f ;
//		WorldParams.FogFarZ  = relalt * 64;
//		SetFogRange(  WorldParams.FogNearZ,  WorldParams.FogFarZ  );
//	}
}

//extern Sprite LensSprite;
//void GrFillRectAlpha( GrBuff *dest,int Sx,int Sy,int Sw,int Sh, int r, int g, int b, int a);



void CloudPal( void)
{
//	static float LastRelativeCloudHeight=0.0;
//	char drive[_MAX_DRIVE];
//	char dir[_MAX_DIR];
//	char fname[_MAX_FNAME];

//	if( LastRelativeCloudHeight>0.0 && RelativeCloudHeight <= 0.0)
//	{
//		// If we were cloudy lets go clear and turn on stars
//		if ((WorldParams.Weather & WR_VIS) == WR_VIS_LOW)
//		{
//			_splitpath(FadingFilename,drive,dir,fname,NULL);

//			if (fname[strlen(fname)-3] == 'c')
//				strcpy(fname, "iraq-l");
//			else
//				strcpy(fname, "iraq");

//			_makepath(FadingFilename,drive,dir,fname,"clr");

//			ReadFadeTable();
//			WorldParams.CurrentPaletteNumber = -1;
//		}

//	}
//	else if( LastRelativeCloudHeight<=0.0 && RelativeCloudHeight >0.0 )
//	{
//		//Reload if Cloudy else remains the same
//		if ((WorldParams.Weather & WR_VIS) == WR_VIS_LOW)
//		{
//			_splitpath(FadingFilename,drive,dir,fname,NULL);

//			if (fname[strlen(fname)-2] != '-')
//				strcat(fname,"c");
//			else
//			{
//				fname[strlen(fname)-2] = 0;
//				strcat(fname,"c-l");
//			}

//			_makepath(FadingFilename,drive,dir,fname,"clr");

//			ReadFadeTable();
//			WorldParams.CurrentPaletteNumber = -1;
//		}
//	}
//	LastRelativeCloudHeight = RelativeCloudHeight;

}

void DrawCloudWall( void)
{
//	float relalt;
//	float alpha;
//	float r, g, b;

//	if(  _3dxl==_3DXL_GLIDE)
//	{

//		relalt = RelativeCloudHeight;
//		relalt = fabs( relalt);

//		if( (WorldParams.Weather & WR_CLOUD1) &&  relalt < (5000 FEET))
//		{
//			alpha = (1.0f -(relalt /((float)5000 FEET))) * 255.0f;
//			r = GET_CURRENT_RED(COLOR_SOURCE_FOG)* 1.2;
//			g = GET_CURRENT_GREEN(COLOR_SOURCE_FOG)* 1.2;
//			b = GET_CURRENT_BLUE(COLOR_SOURCE_FOG)* 1.2;//COLOR_SOURCE_WHITE
//			if( r>255.0f)
//				r = 255.0f;
//			if( g>255.0f)
//				g = 255.0f;
//			if( b>255.0f)
//				b = 255.0f;

//			GrFillRectAlpha( NULL, GrBuffFor3D->ClipLeft, GrBuffFor3D->ClipTop, GrBuffFor3D->ClipRight-GrBuffFor3D->ClipLeft, GrBuffFor3D->ClipBottom-GrBuffFor3D->ClipTop, r, g, b, alpha);
//			CloudPal( );

//		}
//	}
}
*/



void DrawCloudWall( float alpha, float tscale, float uoffset, float voffset, float Ka, char Inverse )
{
	RotPoint3D *pPt3D, Points[4];
	RotPoint3D *points[4];
	int poly_type= 	PRIM_TEXTURE | PRIM_ALPHAIMM| PRIM_ALPHA_ADD|PRIM_NO_Z_WRITE;
	int clip_flags;
	FPoint diffuse;
	int screenwidth = ScreenSize.cx;
	int screenheight = ScreenSize.cy;
	float ulx, uly, lrx,  lry;


	// Calc Cordins
	ulx = -1.0f;
	uly = -1.0f;
	lrx = 1.0f;
	lry = 1.0f;

	ulx = ulx * tscale;
	uly = uly * tscale;
	lrx = lrx * tscale;
	lry = lry * tscale;

	ulx += uoffset;
	uly += voffset;
	lrx += uoffset;
	lry += voffset;


	alpha *= Ka;




	// Just Draw the wall
	CurrentTexture = pCloudFlatTexture;

	pPt3D  = &Points[0];

	diffuse  = NaturalLight;

	// Setup corner points
	pPt3D  = &Points[0];
	pPt3D->Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
	pPt3D->fSX = 0.0f;
	pPt3D->fSY = 0.0f;
	pPt3D->U = ulx;		//leff/right
	pPt3D->V = uly;		//up/down
	pPt3D->Alpha = alpha;	//forward back
	pPt3D->Diffuse = diffuse;
	pPt3D->Specular.SetValues(0.0f);
	pPt3D->oow  = 1.0f;
	pPt3D->ooz  = 0.0f;
	pPt3D->fog  = 1.0f;

	pPt3D  = &Points[1];
	pPt3D->Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
	pPt3D->fSX = screenwidth;
	pPt3D->fSY = 0;
	pPt3D->U = lrx;
	pPt3D->V = uly;
	pPt3D->Alpha = alpha;
	pPt3D->Diffuse = diffuse;
	pPt3D->Specular.SetValues(0.0f);
	pPt3D->oow  = 1.0f;
	pPt3D->ooz  = 0.0f;
	pPt3D->fog  = 1.0f;

	pPt3D  = &Points[2];
	pPt3D->Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
	pPt3D->fSX = screenwidth;
	pPt3D->fSY = screenheight;
	pPt3D->U = lrx;//1.0f;
	pPt3D->V = lry;//1.0f;
	pPt3D->Alpha = alpha;
	pPt3D->Diffuse = diffuse;
	pPt3D->Specular.SetValues(0.0f);
	pPt3D->oow  = 1.0f;
	pPt3D->ooz  = 0.0f;
	pPt3D->fog  = 1.0f;

	pPt3D  = &Points[3];
	pPt3D->Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
	pPt3D->fSX = 0;
	pPt3D->fSY = screenheight;
	pPt3D->U = ulx;
	pPt3D->V = lry;
	pPt3D->Alpha = alpha;
	pPt3D->Diffuse = diffuse;
	pPt3D->Specular.SetValues(0.0f);
	pPt3D->oow  = 1.0f;
	pPt3D->ooz  = 0.0f;
	pPt3D->fog  = 1.0f;

	clip_flags = Points[0].Flags | Points[1].Flags | Points[2].Flags | Points[3].Flags;

	points[0] = &Points[0];
	points[1] = &Points[1];
	points[2] = &Points[2];
	points[3] = &Points[3];

	DrawPolyClipped(poly_type, CL_CLIP_UV, 4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);
}




void RenderCloudWall( float kA )
{
	float dist;
	FMatrix *pMat, tmpmat;
	FPoint MoveV, To, From;
	FPoint vpn, vleft, vup;
	static float alpha=0.5f, alpha2=0.0f;
	static float uoffset=0, voffset=0;
	static float uoffset2=0.5f, voffset2=0.3f;

	static float tscale2=0, tscale1= 0.0f;

	float fowardback, updown, leftright;
	float tscale;


	pMat =  &Camera1.CameraMatrix;//&ViewMatrix;

	To = Camera1.CameraLocation;
	From = Camera1.LastCameraLocation;
	MoveV = To;
	MoveV -= From;
	dist = MoveV.QuickLength();
	if( dist > 0.001)
	{
		MoveV.Normalize();

		vleft.SetValues(pMat->m_Data.RC.R0C0, pMat->m_Data.RC.R1C0, pMat->m_Data.RC.R2C0 );
		vup.SetValues(  pMat->m_Data.RC.R0C1, pMat->m_Data.RC.R1C1, pMat->m_Data.RC.R2C1 );
		vpn.SetValues(  pMat->m_Data.RC.R0C2, pMat->m_Data.RC.R1C2, pMat->m_Data.RC.R2C2 );


		fowardback = vpn   * MoveV;
		leftright  = vleft * MoveV;
		updown     = vup   * MoveV;
		fowardback = -fowardback;


		static maxd =60;
		if(dist >maxd)
			dist = maxd;

		//get forward movement

		alpha = alpha  + fowardback * (dist/maxd) * 0.1f;
		tscale1= tscale1 + fowardback * (dist/maxd) * 0.1f;
		if( alpha >1.0f)
		{
			uoffset = frand() ;
			voffset = frand() ;
			alpha = alpha-1.0f;
		}
		else if ( alpha <0.0f)
		{
			uoffset = frand() ;
			voffset = frand() ;
			alpha = alpha + 1.0f;
		}


		alpha2 = alpha2 + fowardback * (dist/maxd) * 0.1f;
		tscale2= tscale2 + fowardback * (dist/maxd) * 0.1f;

		if( alpha2 >1.0f)
		{
			uoffset2 = frand() ;
			voffset2 = frand() ;
			alpha2 = alpha2-1.0f;
		}
		else if ( alpha2 <0.0f)
		{
			uoffset2 = frand() ;
			voffset2 = frand() ;
			alpha2 = alpha2 + 1.0f;
		}


		//probalably wand to start a random offset for each new wall
		//left/right/up/down
		uoffset = uoffset + leftright* (dist/maxd) * 0.1f;
		voffset = voffset + updown   * (dist/maxd) * 0.1f;;

		uoffset2 = uoffset2 + leftright* (dist/maxd) * 0.1f;
		voffset2 = voffset2 + updown   * (dist/maxd) * 0.1f;;


	}

	float tmpAlpha;

	tscale = 1.0f - alpha;//fowardback * 0.75f + 0.25f; //keep scale 0.25..0.75f
	if( alpha>=0.5f)
		tmpAlpha= (1.0f- (2.0f*(alpha-0.5f)));
	else
		tmpAlpha= 2.0f*alpha ;

	DrawCloudWall( tmpAlpha, tscale, uoffset, voffset, kA );


	tscale = 1.0f - alpha2;
	tmpAlpha = sqrt( 1.0f- (tmpAlpha * tmpAlpha ));
	DrawCloudWall( tmpAlpha, tscale, uoffset2, voffset2, kA );

	/*
	tscale = 1.0f - alpha2;
	if( alpha2>=0.5f)
		DrawCloudWall( (1.0f- (2.0f*(alpha2-0.5f))) * kA, tscale, uoffset2, voffset2);
	else
		DrawCloudWall( 2.0f*alpha2 * kA, tscale, uoffset2, voffset2);
	*/




	//TESTING HERE

/*
	char TmpStr[128];
	sprintf(TmpStr, " fowardback %f", fowardback);
	GrDrawString(GrBuffFor3D, SmHUDFont, 10,50, TmpStr,  255);
	sprintf(TmpStr, " leftright %f", leftright);
	GrDrawString(GrBuffFor3D, SmHUDFont, 10,60, TmpStr,  255);
	sprintf(TmpStr, " updown %f", updown );
	GrDrawString(GrBuffFor3D, SmHUDFont, 10,70, TmpStr,  255);
	sprintf(TmpStr, " tscale %f, dist %f", tscale, dist);
	GrDrawString(GrBuffFor3D, SmHUDFont, 10,80, TmpStr,  255);
*/

}




typedef struct _Rain
{
	FPoint pt;
	FPoint pt2;

	FPoint alpha;
	BYTE  Density;
	float Velocity;

}Rain;

#define MAX_RAIN_DROPS	800

Rain RainDrops[ MAX_RAIN_DROPS ];

typedef struct _RainSpot
{
	BYTE  active;
	float sx, sy;
	float scale;
	float t;
}RainSpot;

#define MAX_RAIN_SPOTS  8
RainSpot RainSpots[ MAX_RAIN_SPOTS];

float RainGridSize  = 50.0f METERS;
float RainGridSize2x = RainGridSize * RainGridSize;
float RainMaxLength = 2.3f METERS;

void InitRain( void)
{
	Rain *pRain;

	ZeroMemory( &RainSpots[0], sizeof( RainSpots) );

	pRain = &RainDrops[ 0];
	while( pRain< &RainDrops[MAX_RAIN_DROPS])
	{
		pRain->pt.X = RainGridSize * (frand() - 0.5f);
		pRain->pt.Z = RainGridSize * (frand() - 0.5f);
		pRain->pt.Y = RainGridSize * (frand() - 0.5f);


		pRain->pt2 = pRain->pt;
		pRain->pt2.Y -= RainMaxLength;//0.5 METERS;
		pRain->Velocity = (2.0f METERS) + (8.0f METERS) * frand();		// var velocitys a bit
		pRain->Density = (unsigned char)(255.0f * frand());

		pRain++;
	}
}

void AddRainSpot( void )
{
	RainSpot *pRainSpot;

	pRainSpot = &RainSpots[0];
	while( pRainSpot < &RainSpots[ MAX_RAIN_SPOTS] )
	{
		if( !pRainSpot->active )
		{

			pRainSpot->active = 1;
			pRainSpot->t = 0.0f;
			pRainSpot->sx = frand() * ScreenSize.cx;
			pRainSpot->sy = frand() * ScreenSize.cy;
			pRainSpot->scale = 1.0f + 3.0f * frand();
			return;
		}
		pRainSpot++;
	}

}


void DrawRainSpots( void )
{
	Sprite spt;
	RainSpot *pRainSpot;
	float speed;

	pRainSpot = &RainSpots[0];
	ZeroMemory( &spt, sizeof( Sprite));
	spt.Type = &SpriteTypes[SPT_RAINSPOT];

	float r, g, b;
	r = NaturalLight.Red;
	g = NaturalLight.Green;
	b = NaturalLight.Blue;


	speed =(PlayerPlane->Knots*0.01f);
	float maxtime = 0.5f- speed*0.3f;
	float alphascaledown;
	if( speed > 0.8f)
		alphascaledown = 1.0f - (1.0f - speed) * 5.0f; // 80 - 100 knots
	else
		alphascaledown = 1.0f;


	while( pRainSpot < &RainSpots[ MAX_RAIN_SPOTS] )
	{
		float a;
		float scale;



		if( pRainSpot->active)
		{
			pRainSpot->t += seconds_per_frame;
			scale = pRainSpot->scale * pRainSpot->t;
			a = 1.0f - pRainSpot->t;
			if( a< 0.0f)
				a = 0.0f;
			a *= 0.2f;
			a *= alphascaledown;

			Render2DSpriteSX( &spt, r, g, b, a, scale, pRainSpot->sx, pRainSpot->sy);

			if(pRainSpot->t > maxtime)
				pRainSpot->active =0;;
		}
		pRainSpot++;
	}

}




void MoveRain( void)
{
	float dist;
	FMatrix *pMat, tmpmat;
	FPoint MoveV, To, From;
	FPoint vpn, vleft, vup;
	FPoint vRain;
	Rain *pRain;
	float maxd = 30;


	//if( frand() < 0.10f)
	AddRainSpot( );

	//Might not be the best spot for this
	MoveLightning( );

	if( doLightning )
	{
		FMatrix Mat;
		FPoint pos;
		float dist;
		FPoint viewvector;

		Mat	= ViewMatrix;
		Mat.Transpose();


		//Get Point and Unit Vecotr;
		viewvector.X = frand() * ScreenSize.cx - GrBuffFor3D->MidX ;
		viewvector.Y = 0;
		viewvector.Z = EyeToScreen;//-512//-GrBuffFor3D->MidX;// ;
		viewvector *= Mat;
		viewvector.Normalize();

		dist = frand() * 8000 METERS + 3000 METERS;
		viewvector *= dist;
		viewvector += Camera1.CameraLocation;
		pos = viewvector;
		pos.Y = cloudalt - (10 FEET);	//pos.Y = LandHeight( pos.X, pos.Z)+ 10 FEET;//

		AddLightning( pos, 50 METERS);
	}







	pMat =  &Camera1.CameraMatrix;

	To = Camera1.CameraLocation;
	From = Camera1.LastCameraLocation;
	MoveV = To;
	MoveV -= From;
	dist = MoveV.QuickLength();//PartialLength();
	if( SnowOn || dist < 0.41 )
	{
		MoveV.SetValues( 0.0f, 1.0f, 0.0f);
		dist = 0.f;
	}
	else
	{
		MoveV.Normalize();
		if( dist >maxd)
			dist = maxd;
		dist = dist/maxd;
	}

	pRain = &RainDrops[ 0];
	while( pRain< &RainDrops[MAX_RAIN_DROPS])
	{
		//move rain

		//vRain.SetValues( 0.0f, -1.0f, 0.0f);
		//vRain = (vRain *MoveV) *vRain;
		vRain = MoveV;

		vRain *= -1.0f;
		if( SnowOn)
		{
			vRain *= seconds_per_frame * pRain->Velocity;
			vRain.Y *= 0.25f;
		}
		else
			vRain *= seconds_per_frame * pRain->Velocity;




		pRain->pt  += vRain;

		float hwidth = RainGridSize * 0.5f;
		float width = RainGridSize ;
		// wrap rain drops
		if( pRain->pt.X >= hwidth)
			pRain->pt.X -= width;
		if( pRain->pt.Y >= hwidth)
			pRain->pt.Y -= width;
		if( pRain->pt.Z >= hwidth)
			pRain->pt.Z -= width;

		if( pRain->pt.X <= -hwidth)
			pRain->pt.X += width;
		if( pRain->pt.Y <= -hwidth)
			pRain->pt.Y += width;
		if( pRain->pt.Z <= -hwidth)
			pRain->pt.Z += width;



		pRain->pt2 = vRain;
		pRain->pt2 *= (RainMaxLength * dist) + (1.3f METERS);
		pRain->pt2 += pRain->pt;


		pRain++;
	}



}

void DrawRainCube(  BYTE density,  FPoint Offset)
{
	FPoint rel, rel2;
	RotPoint3D Points[2], *pPoint;
	RotPoint3D *points[2];
	Rain *pRain;
	int draw_flags;
	int clip_flags;
	float kA;

	FPoint Diffuse;
	Diffuse = NaturalLight;
	Diffuse *= 1.50f; //0.7f
	Diffuse.Limit( 1.0f);


	pRain = &RainDrops[ 0];
	while( pRain< &RainDrops[MAX_RAIN_DROPS])
	{
		if( pRain->Density< density)
		{
			clip_flags =  0;
			draw_flags = -1;

			rel = pRain->pt;
			rel -= Offset;
			rel2 = pRain->pt2;
			rel2 -= Offset;


			float hwidth = RainGridSize * 0.5f;
			float width = RainGridSize ;



			// wrap rain drops
			if( rel.X > hwidth)
			{
				rel.X -= width;
				rel2.X -= width;
			}

			if( rel.Y > hwidth)
			{
				rel.Y -= width;
				rel2.Y -= width;
			}

			if( rel.Z > hwidth)
			{
				rel.Z -= width;
				rel2.Z -= width;
			}

			if( rel.X <= -hwidth)
			{
				rel.X += width;
				rel2.X += width;
			}

			if( rel.Y <= -hwidth)
			{
				rel.Y += width;
				rel2.Y += width;
			}

			if( rel.Z <= -hwidth)
			{
				rel.Z += width;
				rel2.Z += width;
			}

			kA =  rel.QuickLength();
			if( kA > hwidth )
				kA = 0;
			else
			if( (kA > (hwidth  * 0.5f)) && (kA < hwidth) )
				kA = ((1.0f-(kA/hwidth ))* 2.0f);
			else
				kA = 1.0f;



			pPoint = &Points[0];
			//rel = pRain->pt;
	//		rel -= Offset;
	//		rel -= Camera1.CameraLocation;
			pPoint->Rotated.RotateInto( rel,ViewMatrix);

			pPoint = &Points[1];
			pPoint->Rotated.RotateInto( rel2,ViewMatrix);


			pPoint = &Points[0];
			for( int i=0; i<2; i++)
			{
				if (pPoint->Rotated.Z > ZClipPlane)
					pPoint->Flags = PT3_RZHIGH | PT3_ROTATED;
				else
				{
					pPoint->Flags = PT3_ROTATED;
					PerspectPoint(pPoint );
				}
				clip_flags |= pPoint->Flags;
				draw_flags &= pPoint->Flags;
				pPoint->Diffuse= Diffuse;
				pPoint->Specular.SetValues( 0.0f);
				pPoint->oow  = 1.0f;
				pPoint->ooz  = 1.0f;
				pPoint->fog  = 1.0f;
				points[i] = pPoint;
				pPoint++;
			}

			Points[0].Alpha  = 0.0f;
			Points[1].Alpha  = 0.13f * kA;

			if( kA)
			if (!(draw_flags & PT3_NODRAW) )
				DrawPolyClipped( PRIM_ALPHAIMM, CL_CLIP_NONE, 2, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS );
		}
		pRain++;
	}
}
void DrawSnowCube(  BYTE density,  FPoint Offset)
{
	FPoint rel;
	RotPoint3D Points[1], *pPoint;
	RotPoint3D *points[1];
	Rain *pRain;
	int draw_flags;
	int clip_flags;
	float kA;

	FPoint Diffuse;
	Diffuse = NaturalLight;
	Diffuse *= 1.50f; //0.7f
	Diffuse.Limit( 1.0f);

	Sprite spt;
	ZeroMemory( &spt, sizeof( Sprite));
	spt.Type = &SpriteTypes[SPT_RAINSPOT];



	pRain = &RainDrops[ 0];
	while( pRain< &RainDrops[MAX_RAIN_DROPS])
	{
		if( pRain->Density< density)
		{
			clip_flags =  0;
			draw_flags = -1;

			rel = pRain->pt;
			rel -= Offset;


			float hwidth = RainGridSize * 0.5f;
			float width = RainGridSize ;

			// wrap rain drops
			if( rel.X > hwidth)
				rel.X -= width;

			if( rel.Y > hwidth)
				rel.Y -= width;

			if( rel.Z > hwidth)
				rel.Z -= width;

			if( rel.X <= -hwidth)
				rel.X += width;

			if( rel.Y <= -hwidth)
				rel.Y += width;

			if( rel.Z <= -hwidth)
				rel.Z += width;

			kA =  rel.QuickLength();
			if( kA > hwidth )
				kA = 0;
			else
			if( (kA > (hwidth  * 0.5f)) && (kA < hwidth) )
				kA = ((1.0f-(kA/hwidth ))* 2.0f);
			else
				kA = 1.0f;



			pPoint = &Points[0];
			pPoint->Rotated.RotateInto( rel,ViewMatrix);
			//if( RadiusInView( rel, radius) )
			if (pPoint->Rotated.Z > ZClipPlane)
				pPoint->Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				pPoint->Flags = PT3_ROTATED;
				PerspectPoint(pPoint );
			}
			clip_flags |= pPoint->Flags;
			draw_flags &= pPoint->Flags;
			pPoint->Diffuse= Diffuse;
			pPoint->Specular.SetValues( 0.0f);
			pPoint->oow  = 1.0f;
			pPoint->ooz  = 1.0f;
			pPoint->fog  = 1.0f;
			pPoint->Alpha  = 0.5f * kA;

			points[0] = pPoint;

			float scale = pPoint->Alpha*0.50f;

			if(kA && !(draw_flags & PT3_NODRAW) )
				Render2DSpriteSX( &spt, pPoint->Diffuse.Red,pPoint->Diffuse.Green, pPoint->Diffuse.Blue, pPoint->Alpha, scale, pPoint->fSX, pPoint->fSY);
		}
		pRain++;
	}
}

void DisplayRain( BYTE density )
{

	FPoint RainGridOffset, tmp;
	RainGridOffset.X = fmod (Camera1.CameraLocation.X,  RainGridSize);
	RainGridOffset.Y = fmod (Camera1.CameraLocation.Y,  RainGridSize);
	RainGridOffset.Z = fmod (Camera1.CameraLocation.Z,  RainGridSize);

	if( !SnowOn)
		DrawRainCube( density, RainGridOffset);
	else
		DrawSnowCube( density, RainGridOffset);

	if ( (CurrentView & COCKPIT_VIRTUAL_SEAT ) && (PlayerPlane->Knots < 100.0f ))
		DrawRainSpots( );

}




void RemoveLightning( void)
{
	tLightning *pLightning;
	pLightning  = &Lightning;

	pLightning->Active = 0;
	pLightning->Timer = 0;
	if( pLightning->ppLight )
		RemoveLight((LightSource **)pLightning->ppLight );
	if( pLightning->ppLightLow )
		RemoveLight((LightSource **)pLightning->ppLightLow );

	pLightning->ppLight    = NULL;
	pLightning->ppLightLow = NULL;
}



void MoveLightning( void)
{
	tLightning *pLightning;
	pLightning  = &Lightning;

	if( pLightning->Active)
	{
		pLightning->Time += seconds_per_frame;
		if( pLightning->Time > pLightning->TotalTime)
			RemoveLightning( );
		else
		{
			if ((pLightning->Flag & CF_FADE_LIGHT) && pLightning->ppLight)
				(*(pLightning->ppLight))->Intensity = 1.0f- ((pLightning->Time / pLightning->TotalTime));

			if ((pLightning->Flag & CF_FLICKER_LIGHT) && pLightning->ppLight)
				(*(pLightning->ppLight))->Intensity = 0.4f + 0.3f*(1.0f - fmod(pLightning->vFlicker,1.0f)) + 0.3f*frand();


			if( pLightning->ppLightLow )
			{
				(*(pLightning->ppLightLow))->Intensity  =(*(pLightning->ppLight))->Intensity;

				if(pLightning->Time > 0.5f)
				{
					//PlayThunderSound( (*(pLightning->ppLight))->WorldPosition, 1.0f );
					RemoveLight((LightSource **)pLightning->ppLightLow );
					pLightning->ppLightLow = NULL;
				}

			}



		}
	}

}

void AddLightning( FPoint wposition, float scale )
{
	tLightning *pLightning;
	pLightning  = &Lightning;
	float radius = 1000.0f METERS;
	FPoint Color;

	Color.SetValues(1.0f, 1.0f, 1.0f);

	pLightning->Timer += seconds_per_frame;
	if( !pLightning->Active)
	{
		if( pLightning->Timer> pLightning->EmmisonRate  )
		{
			pLightning->Timer = 0;
			pLightning->Active = 1;
			pLightning->Time = 0.0f;
			pLightning->EmmisonRate  =  2.0f + 10.0f * frand( );; //seconds
			pLightning->TotalTime = 0.5f + 1.0f * frand( );// Seconds
			pLightning->Flag = CF_FLICKER_LIGHT | CF_FADE_LIGHT;
			pLightning->vFlicker = 2.0f * frand();

			//Place 2 balls ground, sky
			if (pLightning->ppLight = (PointLightSource **)GetNewLight())
			{
				PointLightSource *new_light = new PointLightSource;
				new_light->WorldPosition = wposition;
				new_light->Color = Color;
				new_light->Flags |= LIGHT_LARGE_SPOT;
				new_light->Radius1 = radius;
				new_light->ooRadius1 = 1.0f/new_light->Radius1;
				new_light->Radius2 = new_light->Radius1 * 0.8f;
				new_light->ooRadius2 = 1.0f/new_light->Radius2;
				*(pLightning->ppLight) = new_light;
			}


			float low;
			low = LandHeight( wposition.X, wposition.Z);

			wposition.Y = frand() *((wposition.Y - low)* 0.5f)  + low;

			if (pLightning->ppLightLow = (PointLightSource **)GetNewLight())
			{
				PointLightSource *new_light = new PointLightSource;
				new_light->WorldPosition = wposition;
				new_light->Color = Color;
				new_light->Flags |= LIGHT_LARGE_SPOT;
				new_light->Radius1 = radius;
				new_light->ooRadius1 = 1.0f/new_light->Radius1;
				new_light->Radius2 = new_light->Radius1 * 0.8f;
				new_light->ooRadius2 = 1.0f/new_light->Radius2;
				*(pLightning->ppLightLow) = new_light;
			}

			PlayThunderSound( (*(pLightning->ppLight))->WorldPosition );
		}
	}

}




void DisplayLighningSprite(FPointDouble &top, FPointDouble &bottom)
{
	FPoint p1,p2;
	float width = 100.0f METERS;


	p1 = top;
	p1 -= Camera1.CameraLocation;

	p2 = bottom;
	p2 -= Camera1.CameraLocation;


	float r = frand();
	if( r<0.25f)
		DrawUnlitAddBillboardRel(p2, p1, width, pLightningTexture);
	else if( r<0.50f)
		DrawUnlitAddBillboardRel(p2, p1, width, pLightningTexture1);
	else if( r<0.75f)
		DrawUnlitAddBillboardRel(p2, p1, width, pLightningTexture2);
	else
		DrawUnlitAddBillboardRel(p2, p1, width, pLightningTexture3);

}



void PlayThunderSound( FPoint WorldPosition, int iMode )
{
	int iVolume, iPan;
	RotPoint3D  rotpt;
	float sx;
	float MaxDist =8000 METERS + 3000 METERS;//Just a made up number. tweak as necessary


	rotpt.Rotated = WorldPosition;
	rotpt.Rotated -= Camera1.CameraLocation;
	rotpt.Rotated *= ViewMatrix;

	PerspectPoint( &rotpt );

	sx = rotpt.fSX;

	if( sx <0.0f)
		sx = 0.0f;
	if( sx > ScreenSize.cx)
		sx = ScreenSize.cx;

	iPan = (int)((sx/(float)ScreenSize.cx) * 127.0f);

	float dist = rotpt.Rotated.QuickLength();
	if( dist> MaxDist)
		dist = MaxDist;

	iVolume =(int) ((1.0f- (dist/MaxDist)) * 127.0f);

	SoundThunder( iVolume, iPan, iMode);


}