#include "F18.h"
#include "3dfxF18.h"

/* -----------------6/25/98 5:21PM---------------------------------------------------------------------
/* return TRUE if handled in the routine
/* ----------------------------------------------------------------------------------------------------*/

extern unsigned char GreenPal[];
extern unsigned char MapTSDPal[];

void _3dxlBeginFrame()
{
	/* -----------------11/25/98 10:24AM-------------------------------------------------------------------
	/* clear the transparent polys here
	/* ----------------------------------------------------------------------------------------------------*/

	switch (_3dxl)
	{
		case _3DXL_GLIDE:
		case _3DXL_NONE:
			break;

		case _3DXL_D3D:
		case _3DXL_SOFTWARE:

			if (lpDDSPrimary->IsLost() == DDERR_SURFACELOST)
				lpDDSPrimary->Restore();

			if (lpDDSBack->IsLost() == DDERR_SURFACELOST)
				lpDDSBack->Restore();

			ResetAlphaLists();
			while (lpD3DDevice->BeginScene() != DD_OK);
			break;
	}

	/* -----------------12/1/98 2:58PM---------------------------------------------------------------------
	/* init dynamic memory for objects
	/* ----------------------------------------------------------------------------------------------------*/

	StartRenderingFunctions();

}

void _3dxlEndFrame()
{
	/* -----------------11/25/98 10:25AM-------------------------------------------------------------------
	/* draw the transparent polys here
	/* ----------------------------------------------------------------------------------------------------*/
	switch (_3dxl)
	{
		case _3DXL_GLIDE:
		case _3DXL_NONE:
			break;

		case _3DXL_D3D:
		case _3DXL_SOFTWARE:
			FlushAlphaLists();
			lpD3DDevice->EndScene();
			break;
	}

	/* -----------------12/1/98 2:59PM---------------------------------------------------------------------
	/* finished with dynamic memory for objects
	/* ----------------------------------------------------------------------------------------------------*/

	StopRenderingFunctions();

}


BOOL _3dxlGrCopyRectNegativeMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color)
{
	BOOL ret_value;

	switch(_3dxl)
	{
		case _3DXL_GLIDE:
//			GrCopyRectMask8Color3Dfx( dest, DDx, DDy, source, SSx, SSy, SSw, SSh, Color );
			ret_value = 1;
			break;

		default:
			ret_value = 0;
			break;
	}

	return ret_value;
}

BOOL _3dxlGrCopyRectMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color)
{
	BOOL ret_value;

	switch(_3dxl)
	{
		case _3DXL_GLIDE:
			GrCopyRectMask8Color3Dfx( dest, DDx, DDy, source, SSx, SSy, SSw, SSh, Color );
			ret_value = 1;
			break;

		default:
			ret_value = 0;
			break;
	}

	return ret_value;
}

BOOL _3dxlGrCopyRectMask8ColorCmpVal(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color, int CmpVal)
{
	BOOL ret_value;

	switch(_3dxl)
	{
		case _3DXL_GLIDE:
			GrCopyRectMask8ColorCmpVal3Dfx( dest, DDx, DDy, source, SSx, SSy, SSw, SSh, Color, CmpVal );
			ret_value = 1;
			break;

		default:
			ret_value = 0;
			break;
	}

	return ret_value;
}


BOOL _3dxlGrCopyRectMask8Fast(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh)
{
	BOOL ret_value;

	switch(_3dxl)
	{
		case _3DXL_GLIDE:
			GrCopyRectHUD83Dfx( dest, DDx, DDy, source, SSx, SSy, SSw, SSh );
			ret_value = 1;
			break;

		default:
			ret_value = 0;
			break;
	}

	return ret_value;
}

void _3dxlSetupNavFlirZValues(float &z_value,float &ooz_value,float &oow_value,float &light_value)
{
	switch(_3dxl)
	{
		case _3DXL_GLIDE:
			z_value = -0.01f;
			ooz_value = oow_value = -1.0f/z_value;
			light_value = 0.0f;
			break;

		case _3DXL_D3D:
		case _3DXL_SOFTWARE:
//			z_value = -LP_LOW_MAX_DISTANCE*1.019f;
//			oow_value = -1.0f/(z_value-1.0f);
//			ooz_value = (float)(LP_LOW_MAX_DISTANCE  + TILE_SIZE*2.0f+1.0f) * oow_value;

			z_value = -(float)(LP_LOW_MAX_DISTANCE  + TILE_SIZE*2.0f)*0.5;

			oow_value = -1.0f/z_value;
			ooz_value = 1.0f;

			light_value = 0.0f;
			break;

//			z_value = -100.0f;
//			oow_value = -1.0f/(z_value-1.0f);
//			ooz_value = (float)(LP_LOW_MAX_DISTANCE  + TILE_SIZE*2.0f+1.0f) * oow_value;
//			light_value = 16.0f * LIGHT_MULTER;
//			break;
	}
}

void _3dxlDisplayPage(GrBuff *buff)
{
	switch(_3dxl)
	{
		case _3DXL_GLIDE:
			GrCopyBuff(BackSurface,buff);
			GrFlipPage3DFX();
			break;

		default:
			if(  VisibleSurface->ConnectedSurface && buff->ConnectedSurface )
				VisibleSurface->ConnectedSurface->Blt(NULL,buff->ConnectedSurface, NULL,DDBLT_ASYNC,NULL);
			else
				GrCopyBuff(VisibleSurface,buff);

			break;
	}
}

float _3dxlMaxBulletAsLineDistance()
{
	float ret_value = (float)(3380 FEET);

	switch (_3dxl)
	{
		case _3DXL_GLIDE:
			ret_value = (float)(1000 FEET);
			break;
	}

	return ret_value;

}

/* -----------------7/9/98 4:19PM----------------------------------------------------------------------
/* these functions change the palette in mid stream
/* ----------------------------------------------------------------------------------------------------*/

void _3dxlPrepareForSpecialPalette(unsigned char *dpal,int table,int FXID)
{
//	unsigned char *spal;

////    SelectFadingTable(table);

//	if (_3dxl)
//	{
//		switch (FXID)
//		{
//			case FX_GREENPAL: 					spal = GreenPal; break;
//			case FX_MAPTSDPAL:                  spal = MapTSDPal; break;
//			case FX_WGREENPAL:                  spal = WGreenPal; break;
//			case FX_BWPAL:                      spal = BWPal; break;
//			case FX_WBWPAL:                     spal = WBWPal; break;
//			case FX_GREENMISSILEPAL:            spal = GreenMissilePal; break;
//			case FX_ADJUSTEDGREENMISSILEPAL:    spal = AdjustedGreenMissilePal; break;
//			case FX_BWMISSILEPAL:               spal = BWMissilePal; break;
//			case FX_ADJUSTEDBWMISSILEPAL:       spal = AdjustedBWMissilePal; break;
//		}

//		switch (_3dxl)
//		{
//			case _3DXL_GLIDE:
//			case _3DXL_D3D:
//				memcpy(dpal,CurrentWorldPalette,256*3);
//				memcpy(CurrentWorldPalette,spal,256*3);
//				FXPaletteID = FXID;
//				ddUpdatePalette(CurrentWorldPalette, 0,256);
//				break;
//		}
//	}
}

void _3dxlFinishSpecialPalette(unsigned char *pal)
{
//	switch (_3dxl)
//	{
//		case _3DXL_GLIDE:
//		case _3DXL_D3D:
//			memcpy(CurrentWorldPalette,pal,256*3);
//			FXPaletteID = FX_NORMAL_PAL;
//			ddUpdatePalette(CurrentWorldPalette, 0,256);
//			break;
//	}
}