
#include "F18.h"
#include "gamesettings.h"


void GrDrawSprite( TextureBuff *pTexture,int x, int y, float  r, float g, float b, int orientation, float u, float v, float u2, float v2);
void GrDrawSpritePolyBuf( GrBuff *dest, TextureBuff *pTexture, float x, float y, float r, float g, float b, int orientation);
int CeilToPow2( int num );

CurSelectedTargetInfoType CurTargetInfo;
RadarInfoType RadarInfo;

//***********************************************************************************************************************************
int GetCurrentAATargetIndex()
{
	 return(-1);
}

//***********************************************************************************************************************************
//
void Aim7Fired()
{
}

//*****************************************************************************************************************************************
int GetAARadarSubMode(void)
{
	return(0);
}

//*****************************************************************************************************************************************
void DrawAvRadarSymbol(int ScrX, int ScrY, int SymbolId,int Red,int Green,int Blue, GrBuff *Buff)
{
	float a, r, g, b;
	float scale = 1.0f/255.0f;
	RDRSymType *pRDRSym;
	pRDRSym = &RDRSym[SymbolId];

	b = (float) Blue * scale;
	g = (float)Green * scale;
	r = (float)Red * scale;
	a = 1.0f;


	if( Buff->pGrBuffPolygon)
		GrDrawSpritePolyBuf( Buff, pRDRSym->pTexture,ScrX - pRDRSym->AnchX, ScrY - pRDRSym->AnchY, r, g, b, 0);
	else
		GrDrawSprite( pRDRSym->pTexture, ScrX - pRDRSym->AnchX, ScrY - pRDRSym->AnchY, r, g, b, 0, 0.0, 0.0, pRDRSym->Width, pRDRSym->Height);
}

//*****************************************************************************************************************************************
void DrawRadarSymbol(int ScrX, int ScrY, int SymbolId,int Color, GrBuff *Buff)
{

	float a, r, g, b;
	float scale = 1.0f/255.0f;
	RDRSymType *pRDRSym;
	pRDRSym = &RDRSym[SymbolId];


	b = (float)GET_CURRENT_BLUE( Color)	* scale;
	g = (float)GET_CURRENT_GREEN( Color)* scale;
	r = (float)GET_CURRENT_RED( Color)	* scale;
	a = 1.0f;


	if( Buff->pGrBuffPolygon)
		GrDrawSpritePolyBuf( Buff, pRDRSym->pTexture,ScrX - pRDRSym->AnchX, ScrY - pRDRSym->AnchY, r, g, b, 0);
	else
		GrDrawSprite( pRDRSym->pTexture, ScrX - pRDRSym->AnchX, ScrY - pRDRSym->AnchY, r, g, b, 0, 0.0, 0.0, pRDRSym->Width, pRDRSym->Height);
}

//***********************************************************************************************************************************
void DrawRadarSymbolKeepColors(int ScrX, int ScrY, int SymbolId)
{

	float a, r, g, b;
	float scale = 1.0f/255.0f;
	RDRSymType *pRDRSym;
	pRDRSym = &RDRSym[SymbolId];

//WE NEED A COLOR HERE BECAUSE THERE IS NO COLOR IN THE TEXTURE ANYMORE
	b = 1.0;
	g = 0.0f;
	r = 1.0f;
	a = 1.0f;

	if( GrBuffFor3D->pGrBuffPolygon)
		GrDrawSpritePolyBuf( GrBuffFor3D, pRDRSym->pTexture,ScrX - pRDRSym->AnchX, ScrY - pRDRSym->AnchY, r, g, b, 0);
	else
		GrDrawSprite( pRDRSym->pTexture, ScrX - pRDRSym->AnchX, ScrY - pRDRSym->AnchY, r, g, b, 0, 0.0, 0.0, pRDRSym->Width, pRDRSym->Height);
}

//***********************************************************************************************************************************
PlaneParams *GetTargetForAmraam()
{
	 return(NULL);
}

//***********************************************************************************************************************************
void SetAutoAcq(int Mode)
{
}

//***********************************************************************************************************************************
//   USED FOR RESETTING RADAR FROM MULTIPLAYER
void ResetAARadarToSearchMode()
{
}

 //***********************************************************************************************************************************
void CleanupRadarStuff(void)
{

 	RDRSymType *pRDRSym;

	pRDRSym = &RDRSym[0];
	for (int i=0;i<NUM_RDR_SYMBOLS;i++)
	{

		if (pRDRSym->Buff)
			GrFreeGrBuff(pRDRSym->Buff);
		pRDRSym->Buff = NULL;

		if( pRDRSym->pTexture)
		{
			//Free3DTexture(pRDRSym->pTexture );
			RemoveTexture( pRDRSym->pTexture  );
			EasyFree(pRDRSym->pTexture  );
			pRDRSym->pTexture = NULL;
		}

		pRDRSym++;
	}

	CleanupAGRadar();

}

//***********************************************************************************************************************************
void SetIff(int State)
{
}

//***********************************************************************************************************************************
int GetCalculatedCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT=FALSE)
{
	 return(6);
}

//***********************************************************************************************************************************
void InitRadar(void)
{
	// Load Radar Symbology sprites
	LPDIRECTDRAWSURFACE4	CutSurface;
	RECT SrcRect, dstRect;
	TextureRef our_ref;
	RDRSymType *pRDRSym;

	ZeroMemory(&our_ref,sizeof(our_ref));
	our_ref.CellWidth = 1.0f;
	our_ref.CellHeight = 1.0f;

	dstRect.left = 0;
	dstRect.top = 0;



	SetTextureFormat( FT_16BIT_DATA );

	CutSurface = LoadPCXDDSurface(RegPath("Cockpits","rdrspr.pcx"), CurrentLoadingFormat,TRUE,UT_PURE_ALPHA);
	pRDRSym = &RDRSym[0];
	for (int i=0;i<NUM_RDR_SYMBOLS;i++)
	{
		if (i < AV_AG_BACKGROUND)
		{
			pRDRSym->Buff =  NULL;
			pRDRSym->Width =  (pRDRSym->XOfs2-pRDRSym->XOfs)+1;
			pRDRSym->Height = (pRDRSym->YOfs2-pRDRSym->YOfs)+1;
			pRDRSym->pTexture = CreateTexture( &our_ref,  CeilToPow2( pRDRSym->Width),	CeilToPow2( pRDRSym->Height ),	UT_PURE_ALPHA,0 );

			pRDRSym->pTexture->WidthMulter = (float)pRDRSym->Width/(float)CeilToPow2( pRDRSym->Width);
			pRDRSym->pTexture->HeightMulter = (float)pRDRSym->Height/(float)CeilToPow2( pRDRSym->Height);


			SrcRect.left	= pRDRSym->XOfs;
			SrcRect.top		= pRDRSym->YOfs;
			SrcRect.right	= pRDRSym->XOfs + pRDRSym->Width;
			SrcRect.bottom	= pRDRSym->YOfs + pRDRSym->Height;

			dstRect.right = pRDRSym->Width;
			dstRect.bottom = pRDRSym->Height;
			pRDRSym->pTexture->D3DSurface->Blt( &dstRect, CutSurface, &SrcRect, DDBLT_WAIT , NULL);//		NewGrCopyRect8(pRDRSym->Buff, 0,0, CutBuff, pRDRSym->XOfs, pRDRSym->YOfs, Width, Height);
		}

		pRDRSym++;

	}
	if( CutSurface)
	{
		CutSurface->Release();
		CutSurface = NULL;
	}

	ReSetTextureFormat();

	InitAGRadar();  // JLM temporary, move elswhere
	InitAGArm();

	InitTewsInfo(); // JLM Debug for now I am piggybackin all inits from this procedure
	InitTsd();
	RadarInfo.DiffCount = 0;  //  Moved from UpdateRadarScan;

}