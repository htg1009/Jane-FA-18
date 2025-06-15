#include "F18.h"
#include "3dfxF18.h"
#include "resources.h"
#include "spchcat.h"
#include "gamesettings.h"

//#define SCALE	1
extern int HudLeft;		// HUD clip window for virtual cockpit
extern int HudRight;
extern int HudTop;
extern int HudBottom;


extern int TranX;			// HUD offsets for virtual cockpit
extern int TranY;			// 0,0 for normal F1 HUD
extern float grTextureCenter;

extern AvRGB *HUDColorRGB;
extern AvRGB HUDColorList[11];



void TestHudSymbols( void)
{
	GrIcon *pHudsym;
	int maxy;
	int x, y;

	maxy =x = y = 0;
	pHudsym = &HUDsym[0];
	for (int i=0; i<27; i++,pHudsym++) //27=SM_NEXT_ICON
	{
		HUD_icon( x, y, i );
		x += pHudsym->wide;
		if( maxy< pHudsym->high )
			maxy = pHudsym->high;

		if( x>ScreenSize.cx)
		{
			y += maxy+5;
			maxy = 0;
			x=0;
		}

	}
	lpDDSPrimary->Blt(NULL,lpDDSBack, NULL,DDBLT_ASYNC,NULL);


}



#define ScalePt(pt) (pt * ScreenSize.cx* (1.0f/640.0f) );

//*****************************************************************************
//=============================================================================
//		H U D   H E L P E R   F U N C T I O N S
//=============================================================================
//*****************************************************************************
//
//	N.B.  all HUDxxx() functions take relative points and use "xPos" & "yPos"
//		  all CRTxxx() functions take absolute points and use "PosX" & "PosY"
//

/*-----------------------------------------------------------------------------
 *
 *	HUDtextSm()		4x5 characters, 3x5 digits and space
 *
 */
void HUDtextSm( int X, int Y, PCSTR fmt, ... )
{
#ifdef SCALE
	//float scale = ScreenSize.cx * (1.0f/640.0f);
	float scale = 1.0f;

#else
	float scale = 1.0f;
#endif
	float fx, fy;

	fx  = X;
	fy  = Y;

#ifdef SCALE
	fx = ScalePt(fx+TranX);
	fy = ScalePt(fy+TranY);
#else
	fx += TranX;
	fy += TranY;
#endif
	char	szTxt[320];

	vsprintf( szTxt, fmt, (char *)(&fmt+1) );

	//iam: this handles some of the HUD elements...
	GrDrawStringClipped( GrBuffFor3D, SmHUDFont, fx, fy, szTxt, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0, scale );


}

/*-----------------------------------------------------------------------------
 *
 *	HUDtextLg()		5x7 characters, 4x7 digits and space
 *
 */
void HUDtextLg( int X, int Y, PCSTR fmt, ... )
{
#ifdef SCALE
	//float scale = ScreenSize.cx * (1.0f/640.0f);
	float scale = 1.0f;
#else
	float scale = 1.0f;
#endif

	float fx, fy;

	fx  = X;
	fy  = Y;

#ifdef SCALE
	fx = ScalePt(fx+TranX);
	fy = ScalePt(fy+TranY);
#else
	fx += TranX;
	fy += TranY;
#endif

	char	szTxt[320];

	vsprintf( szTxt, fmt, (char *)(&fmt+1) );

	GrDrawStringClipped( GrBuffFor3D, LgHUDFont, fx, fy, szTxt, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0, scale );
}

/*-----------------------------------------------------------------------------
 *
 *	HUDclip()
 *
 *		For relative (non-translated) points.  FALSE if in HUD.
 *
 */
int HUDclip( int xPos, int yPos )
{
	//return (xPos < HUD_LEFT) || (xPos > HUD_RIGHT )	|| (yPos < HUD_TOP ) || (yPos > HUD_BOTTOM);
	return ((xPos+TranX) < HudLeft) || ((xPos+TranX)  > HudRight )	|| ((yPos+TranY)  < HudTop ) || ((yPos+TranY)   > HudBottom );

}

/*-----------------------------------------------------------------------------
 *
 *	HUDline()
 *
 */
void HUDline( int X1, int Y1, int X2, int Y2, int color)
{
	float fx1, fx2, fy1, fy2;

	fx1  = X1;
	fy1  = Y1;
	fx2  = X2;
	fy2  = Y2;

#ifdef SCALE
	fx1 = ScalePt(fx1 + TranX);
	fy1 = ScalePt(fy1 + TranY);
	fx2 = ScalePt(fx2 + TranX);
	fy2 = ScalePt(fy2 + TranY);
#else
	fx1 += TranX;
	fy1 += TranY;
	fx2 += TranX;
	fy2 += TranY;
#endif


	GrDrawLineClipped( GrBuffFor3D, fx1, fy1, fx2, fy2, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
}

/*-----------------------------------------------------------------------------
 *
 *	HUDbox()
 *
 */
void HUDbox( int X1, int Y1, int X2, int Y2, int color)
{
	float fx1, fx2, fy1, fy2;

	fx1  = X1;
	fy1  = Y1;
	fx2  = X2;
	fy2  = Y2;

#ifdef SCALE
	fx1 = ScalePt(fx1 + TranX );
	fy1 = ScalePt(fy1 + TranY );
	fx2 = ScalePt(fx2 + TranX );
	fy2 = ScalePt(fy2 + TranY );
#else
	fx1 += TranX;
	fy1 += TranY;
	fx2 += TranX;
	fy2 += TranY;
#endif

	GrDrawLineClipped( GrBuffFor3D, fx1, fy1, fx2, fy1, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, fx1, fy2, fx2, fy2, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, fx1, fy1, fx1, fy2, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, fx2, fy1, fx2, fy2, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
}

/*-----------------------------------------------------------------------------
 *
 *	HUDdash()
 *
 */
void HUDdash( int X1, int Y1, int X2, int Y2, int color)
{
	float fx1, fx2, fy1, fy2;

	fx1  = X1;
	fy1  = Y1;
	fx2  = X2;
	fy2  = Y2;

#ifdef SCALE
	fx1 = ScalePt(fx1 + TranX );
	fy1 = ScalePt(fy1 + TranY );
	fx2 = ScalePt(fx2 + TranX );
	fy2 = ScalePt(fy2 + TranY );
#else
	fx1 += TranX;
	fy1 += TranY;
	fx2 += TranX;
	fy2 += TranY;
#endif

	GrDrawDashedLineClipped( GrBuffFor3D,  fx1, fy1, fx2, fy2, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0  );
}

/*-----------------------------------------------------------------------------
 *
 *	HUDcircle()
 *
 */
void HUDcircle( int X, int Y, int R, int color)
{
	float fx, fy, fr;

	X+=TranX;
	Y+=TranY;

	fx = X;
	fy = Y;
	fr = R;
#ifdef SCALE
	fx = ScalePt(fx);
	fy = ScalePt(fy);
	fr = ScalePt(fr);
#endif

	if (((X-GrBuffFor3D->ClipLeft) >= R) && ((GrBuffFor3D->ClipRight - X) >= R) && ((Y-GrBuffFor3D->ClipTop) >= R) && ((GrBuffFor3D->ClipBottom - Y) >= R))
		DrawCircleNoClip( GrBuffFor3D, fx, fy, fr, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);//color );
	else
		DrawCircleClip( GrBuffFor3D, fx, fy, fr, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);//color );
}

/*-----------------------------------------------------------------------------
 *
 *	HUDcircleDash()
 *
 */

void HUDcircleDash( int X, int Y, int R, int color)
{
	float fx, fy, fr;

	//extern void DrawCircleDashed(GrBuff *Buff, int OriginX, int OriginY, int Radius, int Color, int Clip=0);

	X+=TranX;
	Y+=TranY;


	fx = X;
	fy = Y;
	fr = R;
#ifdef SCALE
	fx = ScalePt(fx);
	fy = ScalePt(fy);
	fr  = ScalePt(fr);
#endif

	if (((X-GrBuffFor3D->ClipLeft) >= R) && ((GrBuffFor3D->ClipRight - X) >= R) && ((Y-GrBuffFor3D->ClipTop) >= R) && ((GrBuffFor3D->ClipBottom - Y) >= R))
		DrawCircleDashed( GrBuffFor3D, fx, fy, fr, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);//color );
	else
		DrawCircleDashed( GrBuffFor3D, fx, fy, fr, HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0,1);
}

/*-----------------------------------------------------------------------------
 *
 *	HUDicon()
 *
 */
void HUDicon( int X, int Y, int icon, int skewX, int skewY, int color)
{
	GrIcon *pIcon = &HUDsym[icon];
	float a, r, g, b;
	float scale = 1.0f/255.0f;

	b = (float)HUDColorRGB->Blue* scale;
	g = (float)HUDColorRGB->Green* scale;
	r = (float)HUDColorRGB->Red* scale;
	a = 1.0f;


	GrDrawSprite( pIcon->pTexture, X - pIcon->hotX + TranX, Y - pIcon->hotY + TranY, r, g, b, 0, skewX, skewY, pIcon->wide, pIcon->high);
	//GrCopyRectMaskColor( BackSurface, X - Icon->hotX + TranX, Y - Icon->hotY + TranY,
	//					  Icon->buff, skewX, skewY, Icon->wide, Icon->high, color );
}

/*-----------------------------------------------------------------------------
 *
 *	HUD_icon()
 *
 */
void HUD_icon( int X, int Y, int icon, int skewX, int skewY, int color)
{
	GrIcon *pIcon = &HUDsym[icon];
	float a, r, g, b;
	float scale = 1.0f/255.0f;

	b = (float)HUDColorRGB->Blue* scale;
	g = (float)HUDColorRGB->Green* scale;
	r = (float)HUDColorRGB->Red* scale;
	a = 1.0f;

	GrDrawSprite( pIcon->pTexture, X - pIcon->hotX + TranX, Y - pIcon->hotY + TranY, r, g, b, 0, skewX, skewY, pIcon->wide, pIcon->high);

	//GrCopyRectMaskColor( GrBuffFor3D, X - Icon->hotX + TranX, Y - Icon->hotY + TranY,
	//					  Icon->buff, skewX, skewY, Icon->wide, Icon->high, color );
}


/*-----------------------------------------------------------------------------
 *
 *	HUDiconCmp()
 *
 */
void HUDiconCmp( int X, int Y, int icon, int value, int color)
{
	GrIcon *pIcon = &HUDsym[icon];
	float a, r, g, b;
	float scale = 1.0f/255.0f;

	b = (float)HUDColorRGB->Blue* scale;
	g = (float)HUDColorRGB->Green* scale;
	r = (float)HUDColorRGB->Red* scale;
	a = 1.0f;


	GrDrawSprite( pIcon->pTexture, X - pIcon->hotX + TranX, Y - pIcon->hotY + TranY, r, g, b, 0, 0.0f, 0.0f, pIcon->wide, pIcon->high);

	//GrCopyRectMaskColorCmpVal( BackSurface, X - Icon->hotX + TranX, Y - Icon->hotY + TranY,
	//							Icon->buff, 0,0, Icon->wide, Icon->high, color, value );
}

/*-----------------------------------------------------------------------------
 *
 *	CRTline()
 *
 */
void CRTline( int X1, int Y1, int X2, int Y2, int color)
{
	GrDrawLineClipped( GrBuffFor3D, X1, Y1, X2, Y2,HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0);
}

/*-----------------------------------------------------------------------------
 *
 *	CRT_line()
 *
 */
void CRT_line( int X1, int Y1, int X2, int Y2, int color)
{
	GrDrawLineClipped( GrBuffFor3D, X1, Y1, X2, Y2,HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
}

/*-----------------------------------------------------------------------------
 *
 *	CRTbox()
 *
 */
void CRTbox( int X1, int Y1, int X2, int Y2, int color)
{
	GrDrawLineClipped( GrBuffFor3D, X1, Y1, X2, Y1,HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, X1, Y2, X2, Y2,HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, X1, Y1, X1, Y2,HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
	GrDrawLineClipped( GrBuffFor3D, X2, Y1, X2, Y2,HUDColorRGB->Red, HUDColorRGB->Green, HUDColorRGB->Blue, 0 );
}

/*-----------------------------------------------------------------------------
 *
 *	CRTcircle()
 *
 */
void CRTcircle( int X, int Y, int R, int color)
{
	DrawCircleClip( GrBuffFor3D, X, Y, R, color );
}

/*-----------------------------------------------------------------------------
 *
 *	CRTicon()
 *
 */
void CRTicon( int X, int Y, int icon, int skewX, int skewY, int color)
{
	GrIcon *pIcon = &HUDsym[icon];
	float a, r, g, b;
	float scale = 1.0f/255.0f;

	b = (float)HUDColorRGB->Blue* scale;
	g = (float)HUDColorRGB->Green* scale;
	r = (float)HUDColorRGB->Red* scale;
	a = 1.0f;


	GrDrawSprite( pIcon->pTexture, X - pIcon->hotX, Y - pIcon->hotY, r, g, b, 0, skewX, skewY, pIcon->wide, pIcon->high);


	//GrCopyRectMaskColor( BackSurface, X - Icon->hotX, Y - Icon->hotY,
	//					  Icon->buff, skewX, skewY, Icon->wide, Icon->high, color );
}

/*-----------------------------------------------------------------------------
 *
 *	CRT_icon()
 *
 */
void CRT_icon( int X, int Y, int icon, int skewX, int skewY, int color)
{
	GrIcon *pIcon = &HUDsym[icon];

	float a, r, g, b;
	float scale = 1.0f/255.0f;

	b = (float)HUDColorRGB->Blue* scale;
	g = (float)HUDColorRGB->Green* scale;
	r = (float)HUDColorRGB->Red* scale;
	a = 1.0f;

	GrDrawSprite( pIcon->pTexture, X - pIcon->hotX, Y - pIcon->hotY, r, g, b, 0, skewX, skewY, pIcon->wide, pIcon->high);


	//GrCopyRectMaskColor( BackSurface, X - Icon->hotX, Y - Icon->hotY,
	//					  Icon->buff, skewX, skewY, Icon->wide, Icon->high, color );
}

/*-----------------------------------------------------------------------------
 *
 *	CRTiconCmp()
 *
 */
void CRTiconCmp( int X, int Y, int icon, int value, int color)
{
	GrIcon *pIcon = &HUDsym[icon];

	float a, r, g, b;
	float scale = 1.0f/255.0f;

	b = (float)HUDColorRGB->Blue* scale;
	g = (float)HUDColorRGB->Green* scale;
	r = (float)HUDColorRGB->Red* scale;
	a = 1.0f;

	GrDrawSprite( pIcon->pTexture, X - pIcon->hotX, Y - pIcon->hotY, r, g, b, 0, 0, 0, pIcon->wide, pIcon->high);


	//GrCopyRectMaskColorCmpVal( BackSurface, X - Icon->hotX, Y - Icon->hotY,
	//							Icon->buff, 0,0, Icon->wide, Icon->high, color, value );
}

/*-----------------------------------------------------------------------------
 *
 *	CheckInHUD()  -- for translated (absolute) points
 *
 *		Only used locally for HUDinCMD and VelVect checks.
 *
 */
int CheckInHUD( int PosX, int PosY )
{
	return (PosX >= GrBuffFor3D->ClipLeft) && (PosX <= GrBuffFor3D->ClipRight )
		&& (PosY >= GrBuffFor3D->ClipTop ) && (PosY <= GrBuffFor3D->ClipBottom);
}

/*-----------------------------------------------------------------------------
 *
 *	CheckDesignateInHUD()  -- for translated (absolute) points
 *
 *		Only called from cockpit.cpp to qualify AG_designate.
 *		Therefore GrBuffFor3D has not yet been set to the HUD.
 *
 *		Can't be called from virtual cockpit.
 *
 */
int CheckDesignateInHUD( int PosX, int PosY )
{
	//return (PosX >= HUD_LEFT) && (PosX <= HUD_RIGHT ) && (PosY >= HUD_TOP ) && (PosY <= HUD_BOTTOM);
	return (PosX >= HudLeft ) && (PosX <= HudRight  ) && (PosY >= HudTop ) && (PosY <= HudBottom );

}

/*-----------------------------------------------------------------------------
 *
 *	ClipToHUD()  --  for translated (absolute) points
 *
 */
void ClipToHUD( int *X, int *Y )
{
	if (*X < GrBuffFor3D->ClipLeft  )  *X = GrBuffFor3D->ClipLeft;
	if (*X > GrBuffFor3D->ClipRight )  *X = GrBuffFor3D->ClipRight;
	if (*Y < GrBuffFor3D->ClipTop   )  *Y = GrBuffFor3D->ClipTop;
	if (*Y > GrBuffFor3D->ClipBottom)  *Y = GrBuffFor3D->ClipBottom;
}

/*-----------------------------------------------------------------------------
 *
 *	HUDcliptest()
 *
 */
void HUDcliptest( void )
{
		HUDbox( HUD_LEFT,HUD_TOP, HUD_RIGHT,HUD_BOTTOM );

		HUDline( HUD_LEFT,HUD_TOP, HUD_RIGHT,HUD_BOTTOM );

		HUDline( HUD_MIDX,HUD_TOP, HUD_MIDX,HUD_BOTTOM );

		HUDline( HUD_LEFT,HUD_MIDY, HUD_RIGHT,HUD_MIDY );

		HUDtextLg( HUD_LEFT-1, HUD_MIDY+00, "1" );
		HUDtextLg( HUD_LEFT+0, HUD_MIDY+10, "2" );
		HUDtextLg( HUD_LEFT+1, HUD_MIDY+20, "3" );

		HUDtextLg( HUD_RIGHT-2, HUD_MIDY+00, "1" );
		HUDtextLg( HUD_RIGHT-3, HUD_MIDY+10, "2" );
		HUDtextLg( HUD_RIGHT-4, HUD_MIDY+20, "3" );

		HUDtextLg( HUD_MIDX+00, HUD_TOP-1, "1" );
		HUDtextLg( HUD_MIDX+10, HUD_TOP+0, "2" );
		HUDtextLg( HUD_MIDX+20, HUD_TOP+1, "3" );

		HUDtextLg( HUD_MIDX+00, HUD_BOTTOM-4, "1" );
		HUDtextLg( HUD_MIDX+10, HUD_BOTTOM-6, "2" );
		HUDtextLg( HUD_MIDX+20, HUD_BOTTOM-7, "3" );
}

extern "C" __declspec(naked) int x_ftol()
{
	__asm
	{
		mov		edx,[esp]
		fistp	dword ptr [esp]
		pop		eax
		jmp		edx
	}
}

void GrCopyRectNegativeMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color)
{
	int had_to_lock = 0;
	unsigned char *destbuffer=0;
	unsigned char *srcbuffer=0;
	int SrcBufferPitch = 0;
	int DstBufferPitch = 0;
	int DDxSw,DDySh;


	if ((dest == BackSurface) && _3dxlGrCopyRectNegativeMask8Color(dest,DDx,DDy, source, SSx, SSy, SSw, SSh, Color ))
		return;

	srcbuffer = GrGetBuffPointer(source,&had_to_lock);

#ifdef DEBUG_VERSION
	destbuffer = GrGetBuffPointer(dest,&had_to_lock);
#else
	if (srcbuffer && (destbuffer = GrGetBuffPointer(dest,&had_to_lock)))
#endif

	{
		_asm
		{
			push	edi
			push	esi

			mov		eax,DDx
			add		eax,SSw
			mov		DDxSw,eax
			mov		ebx,DDy
			add		ebx,SSh
			mov		DDySh,ebx

			mov		edi,dest
			mov		eax,[edi]GrBuff.ClipLeft
			mov		ebx,[edi]GrBuff.ClipTop
			mov		ecx,[edi]GrBuff.ClipRight
			mov		edx,[edi]GrBuff.ClipBottom
			cmp		DDx,eax
			jl		CheckOutside
			cmp		DDy,ebx
			jl		CheckOutside
			cmp		DDxSw,ecx
			jg		CheckOutside
			cmp		DDySh,edx
			jg		CheckOutside
			jmp		ComputeDstAddr

		CheckOutside:
			cmp		DDxSw,eax
			jle		AllDone
			cmp		DDySh,ebx
			jle		AllDone
			cmp		DDx,ecx
			jge		AllDone
			cmp		DDy,edx
			jge		AllDone

			cmp		DDx,eax
			jge		F1
			mov		esi,eax
			sub		esi,DDx
			sub		SSw,esi
			add		SSx,esi
			mov		DDx,eax

		F1: cmp		DDy,ebx
			jge		F2
			mov		esi,ebx
			sub		esi,DDy
			sub		SSh,esi
			add		SSy,esi
			mov		DDy,ebx

		F2:	cmp		DDxSw,ecx
			jle		F3
			sub		ecx,DDx
			mov		SSw,ecx
			inc		SSw//gk

		F3:	cmp		DDySh,edx
			jle		ComputeDstAddr
			sub		edx,DDy
			mov		SSh,edx
			inc		SSh//gk

		ComputeDstAddr:
			cld
			mov		eax,DDx
			mov		ebx,DDy
			mov  	ecx,[edi]GrBuff.Pitch
			mov		DstBufferPitch,ecx
			test	[edi]GrBuff.Type,GR_INDEXED
			jz		short DoMultDst
			mov		edi,[edi]GrBuff.LineStartIndex
			mov		edi,[edi+ebx*4]
			lea		edi,[edi+eax]
			add		edi,destbuffer
			jmp		short ComputeSrcAddr

		DoMultDst:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		edi,destbuffer
			add		edi,edx

		ComputeSrcAddr:
			mov		eax,SSx
			mov		ebx,SSy
			mov		esi,source
			mov  	ecx,[esi]GrBuff.Pitch
			mov		SrcBufferPitch,ecx
			test	[esi]GrBuff.Type,GR_INDEXED
			jz		short DoMultSrc
			mov		esi,[esi]GrBuff.LineStartIndex
			mov		esi,[esi+ebx*4]
			lea		esi,[esi+eax]
			add		esi,srcbuffer
			jmp		short SetupLoop

		DoMultSrc:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		esi,srcbuffer
			add		esi,edx

		SetupLoop:
			mov		ebx,SSh						// total number of scan lines (height)

		HeightLoop:
			mov		ecx,SSw

			push	ebx
			mov		ebx,Color

		MaskLoop:
			mov		al,byte ptr [esi]
			cmp		al,bl
			je		NextByte
			mov		[edi],al

//			mov		al,byte ptr [esi]
//			cmp		al,0
//			je		NextByte
//			mov		[edi],bl

		NextByte:
			inc		esi
			inc		edi
			dec		ecx
			jnz		MaskLoop

			pop		ebx

			mov		eax,SrcBufferPitch
			sub		eax,SSw
			add		esi,eax
			mov		eax,DstBufferPitch
			sub		eax,SSw
			add		edi,eax
			dec		ebx
			jnz		HeightLoop

		AllDone:
			pop		esi
			pop		edi
		}
#ifndef DEBUG_VERSION
		GrTieOffBuffPointer(dest,destbuffer,had_to_lock);
#endif
	}
}


//*********************************************************************************
void GrCopyRectMaskColor(GrBuff *dest,int Dx,int Dy, GrBuff *source, int Sx,int Sy,int Sw,int Sh, int Color)
{
	RECT rintersection,rdest,rsource,rtemp1,rtemp2;

	rtemp1.left = source->ClipLeft;
	rtemp1.top = source->ClipTop;
	rtemp1.right = source->ClipRight+1;
	rtemp1.bottom = source->ClipBottom+1;

	rtemp2.left = Sx;
	rtemp2.top = Sy;
	rtemp2.right = (Sx+Sw);
	rtemp2.bottom = (Sy+Sh);

	if (IntersectRect(&rsource,&rtemp1,&rtemp2))
	{
		Sx = rsource.left;
		Sy = rsource.top;
		Sw = (rsource.right - rsource.left);
		Sh = (rsource.bottom - rsource.top);

		rsource.left = Dx;
		rsource.top = Dy;
		rsource.right = Dx+Sw;
		rsource.bottom = Dy+Sh;

		rdest.left = dest->ClipLeft;
		rdest.top = dest->ClipTop;
		rdest.right = dest->ClipRight+1;
		rdest.bottom = dest->ClipBottom+1;

		if (IntersectRect(&rintersection,&rsource,&rdest))
		{
			Sx += rintersection.left-Dx;
			Sy += rintersection.top-Dy;

			Dx = rintersection.left;
			Dy = rintersection.top;

			Sw = (rintersection.right - rintersection.left);
			Sh = (rintersection.bottom - rintersection.top);


   			if (dest->Type & GR_8BIT)
				GrCopyRectMask8Color( dest,Dx,Dy, source, Sx,Sy,Sw,Sh, Color);
			else
			{
				if (source->Type & GR_8BIT)
				{
					GrTable8To16 *pal;
					pal = &Global8To16Conversion;
					GrCopyRectMask8To16Color(dest, Dx, Dy, source, Sx, Sy, Sw, Sh, pal, Color );
					//GrCopyRectMask8To16Color(dest,Dx,Dy,source,Sx,Sy,Sw,Sh,pal) ; //NEED A COLOR MASK COPY VERSION
				}
				//else
				// 16 to 16	GrCopyRectMask16(dest,Dx,Dy,source,Sx,Sy,Sw,Sh,mask_slot);

			}
		}

	}
}




//*********************************************************************************
void GrCopyRectMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color)
{
	int had_to_lock = 0;
	unsigned char *destbuffer=0;
	unsigned char *srcbuffer=0;
	int SrcBufferPitch = 0;
	int DstBufferPitch = 0;
	int DDxSw,DDySh;

	if ((dest == BackSurface) && _3dxlGrCopyRectMask8Color( dest, DDx, DDy, source, SSx, SSy, SSw, SSh, Color ))
		return;

	srcbuffer = GrGetBuffPointer(source,&had_to_lock);

#ifdef DEBUG_VERSION
	destbuffer = GrGetBuffPointer(dest,&had_to_lock);
#else
	if (srcbuffer && (destbuffer = GrGetBuffPointer(dest,&had_to_lock)))
#endif

	{
		_asm
		{
			push	edi
			push	esi

			mov		eax,DDx
			add		eax,SSw
			mov		DDxSw,eax
			mov		ebx,DDy
			add		ebx,SSh
			mov		DDySh,ebx

			mov		edi,dest
			mov		eax,[edi]GrBuff.ClipLeft
			mov		ebx,[edi]GrBuff.ClipTop
			mov		ecx,[edi]GrBuff.ClipRight
			mov		edx,[edi]GrBuff.ClipBottom
			cmp		DDx,eax
			jl		CheckOutside
			cmp		DDy,ebx
			jl		CheckOutside
			cmp		DDxSw,ecx
			jg		CheckOutside
			cmp		DDySh,edx
			jg		CheckOutside
			jmp		ComputeDstAddr

		CheckOutside:
			cmp		DDxSw,eax
			jle		AllDone
			cmp		DDySh,ebx
			jle		AllDone
			cmp		DDx,ecx
			jge		AllDone
			cmp		DDy,edx
			jge		AllDone

			cmp		DDx,eax
			jge		F1
			mov		esi,eax
			sub		esi,DDx
			sub		SSw,esi
			add		SSx,esi
			mov		DDx,eax

		F1: cmp		DDy,ebx
			jge		F2
			mov		esi,ebx
			sub		esi,DDy
			sub		SSh,esi
			add		SSy,esi
			mov		DDy,ebx

		F2:	cmp		DDxSw,ecx
			jle		F3
			sub		ecx,DDx
			mov		SSw,ecx
			inc		SSw//gk

		F3:	cmp		DDySh,edx
			jle		ComputeDstAddr
			sub		edx,DDy
			mov		SSh,edx
			inc		SSh//gk

		ComputeDstAddr:
			cld
			mov		eax,DDx
			mov		ebx,DDy
			mov 	ecx,[edi]GrBuff.Pitch
			mov		DstBufferPitch,ecx
			test	[edi]GrBuff.Type,GR_INDEXED
			jz		short DoMultDst
			mov		edi,[edi]GrBuff.LineStartIndex
			mov		edi,[edi+ebx*4]
			lea		edi,[edi+eax]
			add		edi,destbuffer
			jmp		short ComputeSrcAddr

		DoMultDst:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		edi,destbuffer
			add		edi,edx

		ComputeSrcAddr:
			mov		eax,SSx
			mov		ebx,SSy
			mov		esi,source
			mov		ecx,[esi]GrBuff.Pitch
			mov		SrcBufferPitch,ecx
			test	[esi]GrBuff.Type,GR_INDEXED
			jz		short DoMultSrc
			mov		esi,[esi]GrBuff.LineStartIndex
			mov		esi,[esi+ebx*4]
			lea		esi,[esi+eax]
			add		esi,srcbuffer
			jmp		short SetupLoop

		DoMultSrc:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		esi,srcbuffer
			add		esi,edx

		SetupLoop:
			mov		ebx,SSh						// total number of scan lines (height)

		HeightLoop:
			mov		ecx,SSw

			push	ebx
			mov		ebx,Color

		MaskLoop:
			mov		al,byte ptr [esi]
			cmp		al,0
			je		NextByte
			mov		[edi],bl

		NextByte:
			inc		esi
			inc		edi
			dec		ecx
			jnz		MaskLoop

			pop		ebx

			mov		eax,SrcBufferPitch
			sub		eax,SSw
			add		esi,eax
			mov		eax,DstBufferPitch
			sub		eax,SSw
			add		edi,eax
			dec		ebx
			jnz		HeightLoop

		AllDone:
			pop		esi
			pop		edi
		}
#ifndef DEBUG_VERSION
		GrTieOffBuffPointer(dest,destbuffer,had_to_lock);
#endif
	}
}
//*********************************************************************************
//*********************************************************************************
void GrCopyRectMaskColorCmpVal(GrBuff *dest,int Dx,int Dy, GrBuff *source, int Sx,int Sy,int Sw,int Sh, int Color, int CmpVal)
{
	RECT rintersection,rdest,rsource,rtemp1,rtemp2;

	rtemp1.left = source->ClipLeft;
	rtemp1.top = source->ClipTop;
	rtemp1.right = source->ClipRight+1;
	rtemp1.bottom = source->ClipBottom+1;

	rtemp2.left = Sx;
	rtemp2.top = Sy;
	rtemp2.right = (Sx+Sw);
	rtemp2.bottom = (Sy+Sh);

	if (IntersectRect(&rsource,&rtemp1,&rtemp2))
	{
		Sx = rsource.left;
		Sy = rsource.top;
		Sw = (rsource.right - rsource.left);
		Sh = (rsource.bottom - rsource.top);

		rsource.left = Dx;
		rsource.top = Dy;
		rsource.right = Dx+Sw;
		rsource.bottom = Dy+Sh;

		rdest.left = dest->ClipLeft;
		rdest.top = dest->ClipTop;
		rdest.right = dest->ClipRight+1;
		rdest.bottom = dest->ClipBottom+1;

		if (IntersectRect(&rintersection,&rsource,&rdest))
		{
			Sx += rintersection.left-Dx;
			Sy += rintersection.top-Dy;

			Dx = rintersection.left;
			Dy = rintersection.top;

			Sw = (rintersection.right - rintersection.left);
			Sh = (rintersection.bottom - rintersection.top);


   			if (dest->Type & GR_8BIT)
				GrCopyRectMask8ColorCmpVal( dest,Dx,Dy, source, Sx,Sy,Sw,Sh, Color, CmpVal);
			else
			{
				if (source->Type & GR_8BIT)
				{
					GrTable8To16 *pal;
					pal = &Global8To16Conversion;
					GrCopyRectMask8to16ColorCmpVal( dest, Dx, Dy, source, Sx, Sy, Sw, Sh, pal, Color, CmpVal );
					//GrCopyRect8To16(dest,Dx,Dy,source,Sx,Sy,Sw,Sh,pal) ; //NEED A COLOR CMP MASK COPY VERSION
				}
				//else
				// 16 to 16	GrCopyRectMask16(dest,Dx,Dy,source,Sx,Sy,Sw,Sh,mask_slot);

			}
		}

	}
}



void GrCopyRectMask8ColorCmpVal(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color, int CmpVal)
{
	int had_to_lock = 0;
	unsigned char *destbuffer=0;
	unsigned char *srcbuffer=0;
	int SrcBufferPitch = 0;
	int DstBufferPitch = 0;
	int DDxSw,DDySh;


	if ((dest == BackSurface) && _3dxlGrCopyRectMask8ColorCmpVal( dest, DDx, DDy, source, SSx, SSy, SSw, SSh, Color, CmpVal ))
		return;

	srcbuffer = GrGetBuffPointer(source,&had_to_lock);

#ifdef DEBUG_VERSION
	destbuffer = GrGetBuffPointer(dest,&had_to_lock);
#else
	if (srcbuffer && (destbuffer = GrGetBuffPointer(dest,&had_to_lock)))
#endif

	{
		_asm
		{
			push	edi
			push	esi

			mov		eax,DDx
			add		eax,SSw
			mov		DDxSw,eax
			mov		ebx,DDy
			add		ebx,SSh
			mov		DDySh,ebx

			mov		edi,dest
			mov		eax,[edi]GrBuff.ClipLeft
			mov		ebx,[edi]GrBuff.ClipTop
			mov		ecx,[edi]GrBuff.ClipRight
			mov		edx,[edi]GrBuff.ClipBottom
			cmp		DDx,eax
			jl		CheckOutside
			cmp		DDy,ebx
			jl		CheckOutside
			cmp		DDxSw,ecx
			jg		CheckOutside
			cmp		DDySh,edx
			jg		CheckOutside
			jmp		ComputeDstAddr

		CheckOutside:
			cmp		DDxSw,eax
			jle		AllDone
			cmp		DDySh,ebx
			jle		AllDone
			cmp		DDx,ecx
			jge		AllDone
			cmp		DDy,edx
			jge		AllDone

			cmp		DDx,eax		; ClipLeft
			jge		F1
			mov		esi,eax
			sub		esi,DDx
			sub		SSw,esi
			add		SSx,esi
			mov		DDx,eax

		F1: cmp		DDy,ebx		; ClipTop
			jge		F2
			mov		esi,ebx
			sub		esi,DDy
			sub		SSh,esi
			add		SSy,esi
			mov		DDy,ebx

		F2:	cmp		DDxSw,ecx	; ClipRight
			jle		F3
			sub		ecx,DDx
			mov		SSw,ecx
			inc		SSw//gk

		F3:	cmp		DDySh,edx	; ClipBottom
			jle		ComputeDstAddr
			sub		edx,DDy
			mov		SSh,edx
			inc		SSh//gk

		ComputeDstAddr:
			cld
			mov		eax,DDx
			mov		ebx,DDy
			mov		ecx,[edi]GrBuff.Pitch
			mov		DstBufferPitch,ecx
			test	[edi]GrBuff.Type,GR_INDEXED
			jz		short DoMultDst
			mov		edi,[edi]GrBuff.LineStartIndex
			mov		edi,[edi+ebx*4]
			lea		edi,[edi+eax]
			add		edi,destbuffer
			jmp		short ComputeSrcAddr

		DoMultDst:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		edi,destbuffer
			add		edi,edx

		ComputeSrcAddr:
			mov		eax,SSx
			mov		ebx,SSy
			mov		esi,source
			mov		ecx,[esi]GrBuff.Pitch
			mov		SrcBufferPitch,ecx
			test	[esi]GrBuff.Type,GR_INDEXED
			jz		short DoMultSrc
			mov		esi,[esi]GrBuff.LineStartIndex
			mov		esi,[esi+ebx*4]
			lea		esi,[esi+eax]
			add		esi,srcbuffer
			jmp		short SetupLoop

		DoMultSrc:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		esi,srcbuffer
			add		esi,edx

		SetupLoop:
			mov		ebx,SSh						// total number of scan lines (height)

		HeightLoop:
			mov		ecx,SSw

			push	ebx
			mov		ebx,Color

		MaskLoop:
			mov		al,byte ptr [esi]
			cmp		al,0
			je		NextByte
			cmp		al,byte ptr CmpVal
			jg		NextByte
			mov		[edi],bl

		NextByte:
			inc		esi
			inc		edi
			dec		ecx
			jnz		MaskLoop

			pop		ebx

			mov		eax,SrcBufferPitch
			sub		eax,SSw
			add		esi,eax
			mov		eax,DstBufferPitch
			sub		eax,SSw
			add		edi,eax
			dec		ebx
			jnz		HeightLoop

		AllDone:
			pop		esi
			pop		edi
		}
#ifndef DEBUG_VERSION
		GrTieOffBuffPointer(dest,destbuffer,had_to_lock);
#endif
	}
}
//*********************************************************************************
void GrCopyRectMask8Fast(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh)
{
	int had_to_lock = 0;
	unsigned char *destbuffer=0;
	unsigned char *srcbuffer=0;
	int SrcBufferPitch = 0;
	int DstBufferPitch = 0;
	int DDxSw,DDySh;


	if ((dest == BackSurface) && _3dxlGrCopyRectMask8Fast( dest, DDx, DDy, source, SSx, SSy, SSw, SSh ))
		return;


	srcbuffer = GrGetBuffPointer(source,&had_to_lock);

#ifdef DEBUG_VERSION
	destbuffer = GrGetBuffPointer(dest,&had_to_lock);
#else
	if (srcbuffer && (destbuffer = GrGetBuffPointer(dest,&had_to_lock)))
#endif

	{
		_asm
		{
			push	edi
			push	esi

			mov		eax,DDx
			add		eax,SSw
			mov		DDxSw,eax
			mov		ebx,DDy
			add		ebx,SSh
			mov		DDySh,ebx

			mov		edi,dest
			mov		eax,[edi]GrBuff.ClipLeft
			mov		ebx,[edi]GrBuff.ClipTop
			mov		ecx,[edi]GrBuff.ClipRight
			mov		edx,[edi]GrBuff.ClipBottom
			cmp		DDx,eax
			jl		CheckOutside
			cmp		DDy,ebx
			jl		CheckOutside
			cmp		DDxSw,ecx
			jg		CheckOutside
			cmp		DDySh,edx
			jg		CheckOutside
			jmp		ComputeDstAddr

		CheckOutside:
			cmp		DDxSw,eax
			jle		AllDone
			cmp		DDySh,ebx
			jle		AllDone
			cmp		DDx,ecx
			jge		AllDone
			cmp		DDy,edx
			jge		AllDone

			cmp		DDx,eax
			jge		F1
			mov		esi,eax
			sub		esi,DDx
			sub		SSw,esi
			add		SSx,esi
			mov		DDx,eax

		F1: cmp		DDy,ebx
			jge		F2
			mov		esi,ebx
			sub		esi,DDy
			sub		SSh,esi
			add		SSy,esi
			mov		DDy,ebx

		F2:	cmp		DDxSw,ecx
			jle		F3
			sub		ecx,DDx
			mov		SSw,ecx
			inc		SSw//gk

		F3:	cmp		DDySh,edx
			jle		ComputeDstAddr
			sub		edx,DDy
			mov		SSh,edx
			inc		SSh//gk

		ComputeDstAddr:
			cld
			mov		eax,DDx
			mov		ebx,DDy
			mov		ecx,[edi]GrBuff.Pitch
			mov		DstBufferPitch,ecx
			test	[edi]GrBuff.Type,GR_INDEXED
			jz		short DoMultDst
			mov		edi,[edi]GrBuff.LineStartIndex
			mov		edi,[edi+ebx*4]
			lea		edi,[edi+eax]
			add		edi,destbuffer
			jmp		short ComputeSrcAddr

		DoMultDst:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		edi,destbuffer
			add		edi,edx

		ComputeSrcAddr:
			mov		eax,SSx
			mov		ebx,SSy
			mov		esi,source
			mov	    ecx,[esi]GrBuff.Pitch
			mov		SrcBufferPitch,ecx
			test	[esi]GrBuff.Type,GR_INDEXED
			jz		short DoMultSrc
			mov		esi,[esi]GrBuff.LineStartIndex
			mov		esi,[esi+ebx*4]
			lea		esi,[esi+eax]
			add		esi,srcbuffer
			jmp		short SetupLoop

		DoMultSrc:
			mov		edx,ecx
			imul	edx,ebx
			add		edx,eax
			mov		esi,srcbuffer
			add		esi,edx

		SetupLoop:
			mov		ebx,SSh						// total number of scan lines (height)

		HeightLoop:
			mov		edx,esi
			neg		edx
			mov		ecx,SSw

		SkipLeadingColorZeroD:
			cmp		dword ptr [esi],0
			jne		SkipLeadingColorZero
			add		esi,4
			sub		ecx,4
			jg		SkipLeadingColorZeroD

		FixupLineEnd:
			add		edi,DstBufferPitch
			add		esi,SrcBufferPitch
			add		ecx,SSw
			add		esi,ecx

			dec		ebx
			jnz		HeightLoop

			jmp		Alldone

		SkipLeadingColorZero:
			cmp		byte ptr [esi],0
			jne		PreMaskLoop
			inc		esi
			dec		ecx
			jnz		SkipLeadingColorZero

			jmp		FixupLineEnd

		PreMaskLoop:
			add		edx,esi
			add		edi,edx
			jmp		GoodStuffLoop


			ALIGN	16
		GoodStuffLoop:
			mov		al,byte ptr [esi]
			//This is out of order
			inc		edi
			or		al,al
			lea		esi,[esi+1]
			jz		NextScanLine
			dec		ecx
			mov		[edi]-1,al
			jnz		GoodStuffLoop

			//because if we didn't come through here, we'd have already added one to ecx
			inc		ecx

		NextScanLine:
			//the minus 1 corrects esi being 1 to high
			sub		ecx,SSw
			lea		esi,[esi+ecx-1]
			add		esi,SrcBufferPitch

			//the minus 1 corrects edi being 1 to high
			lea		edi,[edi+ecx-1]
			add		edi,DstBufferPitch

			dec		ebx
			jnz		HeightLoop

		AllDone:
			pop		esi
			pop		edi
		}
#ifndef DEBUG_VERSION
		GrTieOffBuffPointer(dest,destbuffer,had_to_lock);
#endif
	}
}



void GrDrawSprite( TextureBuff *pTexture,int x, int y, float  r, float g, float b, int orientation, float u, float v, float uwidth, float vwidth)
{

	//float cwidth, cheight;
	float ULU,ULV,LRU,LRV;
	RotPoint3D Points[4];
	RotPoint3D *points[4];
	int clip_flags;
	FPoint Color((float)r, (float)g, (float)b);


	GrBuff *dest = GrBuffFor3D;
	RECT rintersection,rdest,rsource;
	float Sx, Sy, Sw, Sh, Dx, Dy;

	Sx = u;
	Sy = v;
	Sw = uwidth;
	Sh = vwidth;
	Dx = x;
	Dy = y;

	rsource.left = x;
	rsource.top = y;
	rsource.right = x+Sw;
	rsource.bottom = y+Sh;

	rdest.left = dest->ClipLeft;
	rdest.top = dest->ClipTop;
	rdest.right = dest->ClipRight+1;
	rdest.bottom = dest->ClipBottom+1;

	if (IntersectRect(&rintersection,&rsource,&rdest))
	{
		Sx += rintersection.left-Dx;
		Sy += rintersection.top-Dy;

		Dx = rintersection.left;
		Dy = rintersection.top;

		Sw = (rintersection.right - rintersection.left);
		Sh = (rintersection.bottom - rintersection.top);


		//cwidth  = uwidth ;//pTexture->Width;
		//cheight = vwidth ;//pTexture->Height;

		// Set Points
		Points[0].Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
		Points[0].iSX = Dx;
		Points[0].iSY = Dy;
		Points[0].fSX = (float)Dx;
		Points[0].fSY = (float)Dy;
		Points[0].oow  = 1.0f;
		Points[0].ooz  = 0.0f;
		Points[0].fog  = 1.0f;
		Points[0].Alpha = 1.0f;
		Points[0].Specular.SetValues(0.0f);
		Points[0].Diffuse = Color;


		Points[1].Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
		Points[1].iSX = Dx + Sw;
		Points[1].iSY = Dy;
		Points[1].fSX = (float)(Dx+ Sw);
		Points[1].fSY = (float)Dy;
		Points[1].oow  = 1.0f;
		Points[1].ooz  = 0.0f;
		Points[1].fog  = 1.0f;
		Points[1].Alpha = 1.0f;
		Points[1].Specular.SetValues(0.0f);
		Points[1].Diffuse = Color;


		Points[2].Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
		Points[2].iSX = Dx + Sw;
		Points[2].iSY = Dy + Sh;
		Points[2].fSX = (float)(Dx+ Sw);
		Points[2].fSY = (float)(Dy+ Sh);
		Points[2].oow  = 1.0f;

		Points[2].ooz  = 0.0f;
		Points[2].fog  = 1.0f;
		Points[2].Alpha = 1.0f;
		Points[2].Specular.SetValues(0.0f);
		Points[2].Diffuse = Color;

		Points[3].Flags = PT3_ROTATED |PT3_PERSPECTED | PT3_OOW_SET;
		Points[3].iSX = Dx;
		Points[3].iSY = Dy + Sh;
		Points[3].fSX = (float)(Dx);
		Points[3].fSY = (float)(Dy+ Sh);
		Points[3].oow  = 1.0f;
		Points[3].ooz  = 0.0f;
		Points[3].fog  = 1.0f;
		Points[3].Alpha = 1.0f;
		Points[3].Specular.SetValues(0.0f);
		Points[3].Diffuse = Color;



		// Set UV
		ULU = Sx * (1.0f/ pTexture->Width) + grTextureCenter* (1.0f/ pTexture->Width);
		LRU= ULU + Sw  * (1.0f/ pTexture->Width);

		ULV = Sy * (1.0f/ pTexture->Height)+ grTextureCenter* (1.0f/ pTexture->Height);
		LRV= ULV + Sh   * (1.0f/ pTexture->Height);




		if (orientation  & ORIENTATION_FLIP_U)
			Swap4(&ULU,&LRU);

		if (orientation & ORIENTATION_FLIP_V)
			Swap4(&ULV,&LRV);

		switch(orientation & ORIENTATION_ROTATION)
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


		clip_flags = Points[0].Flags | Points[1].Flags | Points[2].Flags | Points[3].Flags;


		CurrentTexture = pTexture;

		points[0] = &Points[0];
		points[1] = &Points[1];
		points[2] = &Points[2];
		points[3] = &Points[3];

		//| PRIM_ALPHAIMM/*PRIM_COLOR_KEY|*/
		DrawPolyClipped(PRIM_NO_Z_WRITE |PRIM_TEXTURE | PRIM_POINT_FILTER | PRIM_ALPHAIMM,CL_CLIP_UV,4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);
	}
}



void GrDrawSpritePolyBuf( GrBuff *dest, TextureBuff *pTexture, float x, float y, float r, float g, float b, int orientation)
{

	RotPoint3D Points[4], *pPoint;
	RotPoint3D *points[4];
	int clip_flags,draw_flags;
	FPoint LeftVect, DownVect;
	float scale;
	float ULU,ULV,LRU,LRV;


	float x2 = x + pTexture->Width * pTexture->WidthMulter;
	float y2 = y +  pTexture->Height * pTexture->HeightMulter;


	// Poly Buff should already be rotated and have a LRVect, UDVect and ULPoint;
	clip_flags = 0;
	draw_flags = -1;


	//-- Normailze 2D cords-- NOTE: We might remove the Normailize 2D cords and just use the grBuff information if we can change the MPD stuff to just use their own grbuFf width/height
	//x  -= dest->pGrBuffPolygon->x2D;
	//x2 -= dest->pGrBuffPolygon->x2D;
	scale = 1.0f/ dest->pGrBuffPolygon->Width2D; //dest->Width;
	x  *= scale;
	x2 *= scale;

	//y  -= dest->pGrBuffPolygon->y2D;
	//y2 -= dest->pGrBuffPolygon->y2D;
	scale = 1.0f/ dest->pGrBuffPolygon->Height2D; //dest->Height;
	y  *= scale;
	y2 *= scale;



	// Mult  n2d by Edge vectors
	LeftVect = dest->pGrBuffPolygon->LRVector;
	LeftVect *= x;
	DownVect = dest->pGrBuffPolygon->UDVector;
	DownVect *= y;
	Points[0].Rotated = LeftVect;
	Points[0].Rotated += DownVect;
	Points[0].Rotated += dest->pGrBuffPolygon->ULRotPoint;

	LeftVect = dest->pGrBuffPolygon->LRVector;
	LeftVect *= x2;
	DownVect = dest->pGrBuffPolygon->UDVector;
	DownVect *= y;
	Points[1].Rotated = LeftVect;
	Points[1].Rotated += DownVect;
	Points[1].Rotated += dest->pGrBuffPolygon->ULRotPoint;


	LeftVect = dest->pGrBuffPolygon->LRVector;
	LeftVect *= x2;
	DownVect = dest->pGrBuffPolygon->UDVector;
	DownVect *= y2;
	Points[2].Rotated = LeftVect;
	Points[2].Rotated += DownVect;
	Points[2].Rotated += dest->pGrBuffPolygon->ULRotPoint;

	LeftVect = dest->pGrBuffPolygon->LRVector;
	LeftVect *= x;
	DownVect = dest->pGrBuffPolygon->UDVector;
	DownVect *= y2;
	Points[3].Rotated = LeftVect;
	Points[3].Rotated += DownVect;
	Points[3].Rotated += dest->pGrBuffPolygon->ULRotPoint;

	// Setup Texture Cordinates
	ULU = 0;
	LRU= pTexture->WidthMulter;//1.0f;

	ULV = 0;
	LRV= pTexture->HeightMulter;//1.0f;


	if (orientation  & ORIENTATION_FLIP_U)
		Swap4(&ULU,&LRU);

	if (orientation & ORIENTATION_FLIP_V)
		Swap4(&ULV,&LRV);

	switch(orientation & ORIENTATION_ROTATION)
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

	CurrentTexture = pTexture;


	// Rot/Perspect Pts
	pPoint = &Points[0];
	FPoint Offset;
	Offset = dest->pGrBuffPolygon->Offset ;
	Offset *= ViewMatrix;

	pPoint = &Points[0];
	for( int i=0; i<4; i++)
	{
		points[i] = pPoint;

		pPoint->Rotated +=  Offset;

		if (pPoint->Rotated.Z > ZClipPlane)
			pPoint->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			pPoint->Flags = PT3_ROTATED;
			PerspectPoint(pPoint);
		}
		//pPoint->Flags |= PT3_OOW_SET;
		clip_flags |= pPoint->Flags;
		draw_flags &= pPoint->Flags;
		pPoint->Diffuse.SetValues( (const float)r, (const float)g, (const float)b);
		pPoint->Specular.SetValues( 0.0f);
		pPoint->Alpha = 1.0f;
		pPoint->oow  = 1.0f;
		pPoint->ooz  = 1.0f;
		pPoint->fog  = 1.0f;

		pPoint++;
	}



	if (!(draw_flags & PT3_NODRAW))
		DrawPolyClipped(/*PRIM_COLOR_KEY|*/PRIM_NO_Z_WRITE |PRIM_TEXTURE | PRIM_POINT_FILTER | PRIM_ALPHAIMM,CL_CLIP_UV,4, (RotPoint3D **)&points[0], clip_flags & PT3_CLIP_FLAGS);

}