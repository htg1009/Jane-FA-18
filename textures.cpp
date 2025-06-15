#include "grvars.h"

void FreeTexture3Dfx( unsigned char *texture);
extern BOOL NoObjectCacheing;

#ifndef GAME_VERSION

GrBuff *GetD3DCompatibleBuffer()
{
	return (GrBuff *)NULL;
}

#endif

char *SpecialTexturePath;

//******************************************************************************************************************

void SetTextureFormat(FormatType new_type)
{
	OldCurrentLoadingFormat = CurrentLoadingFormat;
	CurrentLoadingFormat = new_type;
	if (CurrentLoadingFormat > FT_16BIT_DATA)
	{
		CurrentLoadingDDPalette = GamePals[((int)CurrentLoadingFormat) - 1];
		CurrentLoadingRGBTable = GameRGBTables[((int)CurrentLoadingFormat) - 1];
	}
	else
	{
		CurrentLoadingDDPalette = NULL;
		CurrentLoadingRGBTable = NULL;
	}
}

void ReSetTextureFormat(void)
{
	CurrentLoadingFormat = OldCurrentLoadingFormat;
	if (CurrentLoadingFormat > FT_16BIT_DATA)
	{
		CurrentLoadingDDPalette = GamePals[((int)CurrentLoadingFormat) - 1];
		CurrentLoadingRGBTable = GameRGBTables[((int)CurrentLoadingFormat) - 1];
	}
	else
	{
		CurrentLoadingDDPalette = NULL;
		CurrentLoadingRGBTable = NULL;
	}
}


//******************************************************************************************************************

void RemoveTexture(TextureBuff *source)
{
	if (source->GlideTexture)
	{
		GrFreeGrBuff(source->GlideTexture);
		source->GlideTexture = NULL;
	}

	if (source->D3DSurface)
	{
		source->D3DTexture->Release();
		source->D3DTexture = NULL;
		source->D3DSurface->SetPalette(NULL);
		source->D3DSurface->Release();
		source->D3DSurface = NULL;
	}

}

//******************************************************************************************************************

void Free3DTexture(TextureBuff *texture)
{
	TextureBuff **last_walker,*walker,*next_walker;

	if (texture == FREE_ALL_TEXTURES)
	{
			walker = TBRoot;
			while(walker)
			{
				next_walker = walker->Next;
				RemoveTexture(walker);
				EasyFree(walker);
				walker = next_walker;
			}
			TBRoot = NULL;
	}
	else
		if (texture && !--texture->Users)
		{
			last_walker = &TBRoot;
			walker = TBRoot;
			while(walker && (walker != texture))
			{
				last_walker = &walker->Next;
				walker = walker->Next;
			}

			if (walker)
				*last_walker = walker->Next;

			RemoveTexture(texture);
			EasyFree(texture);
		}
}

//******************************************************************************************************************

LPDIRECTDRAWSURFACE4 CreateCompatibleSurface(int width,int height, UsageType utype,BOOL as_texture,int mip_maps,BOOL use_565)
{
	LPDIRECTDRAWSURFACE4	ddsurface;
	DDSURFACEDESC2       	ddsd;
	LPDIRECTDRAWPALETTE 	usepal;
	DDPIXELFORMAT 			*lpddpf;

	usepal = CurrentLoadingDDPalette;

	if ((utype == UT_DEFAULT) && (CurrentLoadingFormat == FT_16BIT_DATA))
	{
		if ((use_565) && (D3DTextureFormat.ddpf16bpp565.dwRGBBitCount))
			lpddpf = &D3DTextureFormat.ddpf16bpp565;
		else
			lpddpf = &D3DTextureFormat.ddpf16bpp;

		usepal = NULL;
	}
	else
	{
		if (CurrentLoadingFormat > FT_16BIT_DATA)
			usepal = CurrentLoadingDDPalette;
		else
			usepal = NULL;

		switch (utype)
		{
			case UT_DEFAULT:
				lpddpf = &D3DTextureFormat.ddpf;
				break;

			case UT_PURE_ALPHA:
				lpddpf = &D3DTextureFormat.ddpfAlpha;
				break;

			case UT_RGB_ALPHA:
				lpddpf = &D3DTextureFormat.ddpfColorAlpha;
				break;
		}
	}

	/* -----------------12/10/98 11:56AM-------------------------------------------------------------------
	/* let's create the other buffer first to avoid memory fragging
	/* ----------------------------------------------------------------------------------------------------*/

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	memcpy(&ddsd.ddpfPixelFormat,lpddpf,sizeof(DDPIXELFORMAT));

	if (as_texture)
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
   		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

		if (mip_maps)
		{
			ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
			ddsd.dwMipMapCount = mip_maps+1;
			ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
		}
	}
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;


    if (lpDD->CreateSurface(&ddsd, &ddsurface, NULL) == DD_OK)
	{
		// we may be loading an alpha-only image
		if (usepal)
			ddsurface->SetPalette(usepal);

		DDBLTFX		BltFX;
		if (ddsurface)
		{
			BltFX.dwSize = sizeof (BltFX);
			BltFX.dwFillColor = 0;
			ddsurface->Blt (NULL, NULL, NULL, DDBLT_COLORFILL, &BltFX);
		}

		DDCOLORKEY key;
		memset(&key,0,sizeof(DDCOLORKEY));

		ddsurface->SetColorKey(DDCKEY_SRCBLT,&key);
	}
	else
		ddsurface = NULL;

	return ddsurface;
}


TextureBuff *CreateTexture(TextureRef *file, int width,int height, UsageType utype,int mip_maps )
{
	TextureBuff *returner = (TextureBuff *)EasyCalloc(MEM_CLASS_3D_MAP,1,sizeof(TextureBuff));

	if (returner)
	{
		memcpy(&returner->Info,file,sizeof(TextureRef));

		if (_3dxl == _3DXL_GLIDE)
		{
//			if (returner->GlideTexture = GrAllocGrBuff();LoadPCXGrBuff(NULL,RegPath("textures",file->Name)))
//			{
//				returner->Width = (float)returner->GlideTexture->Width;
//				returner->Height = (float)returner->GlideTexture->Height;
//			}
//			else
//			{
//				EasyFree(returner);
//				return NULL;
//			}

//			returner->Usage = utype;
//			returner->Format = CurrentLoadingFormat;
		}
		else
		{
			if (returner->D3DSurface = CreateCompatibleSurface(width,height,utype,TRUE,mip_maps))
			{

				returner->Width = (float)width;
				returner->Height = (float)height;

				returner->WidthMulter = 1.0f;
				returner->HeightMulter = 1.0f;

//				returner->Info.CellWidth /= returner->Width;
//				returner->Info.CellHeight /= returner->Height;

				returner->D3DSurface->QueryInterface(IID_IDirect3DTexture2,(void **)&returner->D3DTexture);
			}
			else
			{
				EasyFree(returner);
				return NULL;
			}

			returner->Usage = utype;
			returner->Format = CurrentLoadingFormat;
		}

	}

	return(returner);

}


TextureBuff *FetchTextureWithMipMaps(TextureRef *file, int maps, UsageType utype )
{
	char path[_MAX_PATH];
	LPDIRECTDRAWPALETTE usepal;
	LPDIRECTDRAWSURFACE4 temp_surface;
	PCXHeader header;
	int width,height;
	TextureBuff *returner = NULL;
	LPDIRECTDRAWSURFACE4 lpDDLevel, lpDDNextLevel;
	DDSCAPS2 ddsCaps;
	HRESULT ddres;
	DDBLTFX bfx;

	BOOL worked;

	if (SpecialTexturePath)
		if (SpecialTexturePath[strlen(SpecialTexturePath)-1] == '\\')
			sprintf(path,"%s%s",SpecialTexturePath,file->Name);
		else
			sprintf(path,"%s\\%s",SpecialTexturePath,file->Name);
	else
		strcpy(path,RegPath("textures",file->Name));

	worked = GetPCXHeader(path,header);

	if (!worked && SpecialTexturePath)
	{
		strcpy(path,RegPath("textures",file->Name));
		worked = GetPCXHeader(path,header);
	}

	if (worked)
	{

		header.x_max++;
		header.y_max++;

		width = 1;
		while(width < header.x_max)
			width <<= 1;

		height = 1;
		while(height < header.y_max)
			height <<= 1;

		usepal = CurrentLoadingDDPalette;

		if (returner = CreateTexture(file, width,height,utype,maps ))
		{
			if (temp_surface = LoadPCXDDSurface(path,CurrentLoadingFormat,TRUE,utype))
			{
				INIT_DDOBJECT(bfx);
				bfx.dwROP = SRCCOPY;

				returner->D3DSurface->Blt(NULL,temp_surface,NULL,DDBLT_ROP,&bfx);

				lpDDLevel = returner->D3DSurface;
				lpDDLevel->AddRef();
				ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
				ddres = DD_OK;

				while (ddres == DD_OK)
				{
    				ddres = lpDDLevel->GetAttachedSurface(&ddsCaps, &lpDDNextLevel);
    				lpDDLevel->Release();
    				lpDDLevel = lpDDNextLevel;

					if (ddres == DD_OK)
						lpDDLevel->Blt(NULL,temp_surface,NULL,DDBLT_ROP,&bfx);
				}

				returner->WidthMulter = ((float)LastImageSize.cx) / returner->Width;
				returner->HeightMulter = ((float)LastImageSize.cy) / returner->Height;

//				returner->Info.CellWidth /= returner->Width;
//				returner->Info.CellHeight /= returner->Height;

				temp_surface->SetPalette(NULL);
				temp_surface->Release();
			}
			else
			{
				RemoveTexture(returner);
				EasyFree(returner);
			}

		}

	}

	return(returner);
}

TextureBuff *FetchTexture(TextureRef *file, UsageType utype )
{
	char path[_MAX_PATH];

	TextureBuff *returner = (TextureBuff *)EasyCalloc(MEM_CLASS_3D_MAP,1,sizeof(TextureBuff));

	if (returner)
	{
		memcpy(&returner->Info,file,sizeof(TextureRef));

//		if (_3dxl == _3DXL_GLIDE)
//		{
//			if (returner->GlideTexture = LoadPCXGrBuff(NULL,RegPath("textures",file->Name)))
//			{
//				returner->Width = (float)returner->GlideTexture->Width;
//				returner->Height = (float)returner->GlideTexture->Height;
//			}
//			else
//			{
//				EasyFree(returner);
//				return NULL;
//			}

//			returner->Usage = utype;
//			returner->Format = CurrentLoadingFormat;
//		}
//		else
//		{
			LPDIRECTDRAWPALETTE usepal;

			usepal = CurrentLoadingDDPalette;

			if (SpecialTexturePath)
				if (SpecialTexturePath[strlen(SpecialTexturePath)-1] == '\\')
					sprintf(path,"%s%s",SpecialTexturePath,file->Name);
				else
					sprintf(path,"%s\\%s",SpecialTexturePath,file->Name);
			else
				strcpy(path,RegPath("textures",file->Name));

			if (!(returner->D3DSurface = LoadPCXDDSurface(path,CurrentLoadingFormat,TRUE,utype)) && SpecialTexturePath)
			{
				strcpy(path,RegPath("textures",file->Name));
				returner->D3DSurface = LoadPCXDDSurface(path,CurrentLoadingFormat,TRUE,utype);
			}

			if (returner->D3DSurface)
			{
				DDSURFACEDESC2 ddsd;

				ZeroMemory(&ddsd,sizeof(ddsd));
				ddsd.dwSize = sizeof(ddsd);

				returner->D3DSurface->GetSurfaceDesc(&ddsd);

				returner->Width = (float)ddsd.dwWidth;
				returner->Height = (float)ddsd.dwHeight;

				returner->WidthMulter = ((float)LastImageSize.cx) / returner->Width;
				returner->HeightMulter = ((float)LastImageSize.cy) / returner->Height;

//				returner->Info.CellWidth /= returner->Width;
//				returner->Info.CellHeight /= returner->Height;

				returner->D3DSurface->QueryInterface(IID_IDirect3DTexture2,(void **)&returner->D3DTexture);
			}
			else
			{
				EasyFree(returner);
				return NULL;
			}

			returner->Usage = utype;
			returner->Format = CurrentLoadingFormat;
//		}

	}

	return(returner);
}

//******************************************************************************************************************

TextureBuff *Load3DTextureWithAlphaMap(TextureRef *file)
{
	TextureBuff *returner;
	TextureBuff *walker;
	TextureBuff **last_walker;
	char path[_MAX_PATH];
	char new_name[_MAX_FNAME];

	if (!NoObjectCacheing)
	{
		last_walker = &TBRoot;
		walker = TBRoot;

		while(walker && ((walker->Usage != UT_RGB_ALPHAMAX) || stricmp(walker->Info.Name,file->Name)))
		{
			last_walker = &walker->Next;
			walker = walker->Next;
		}
	}
	else
	{
		last_walker = NULL;
		walker = NULL;
	}

	if (walker)
		returner = walker;
	else
	{
		if (returner = FetchTexture(file,UT_RGB_ALPHAMAX))
		{
			LPDIRECTDRAWPALETTE usepal;
			LPDIRECTDRAWSURFACE4 ddsurf;

			// we've loaded the color map, now we want to load the alpha map

			strcpy(new_name,file->Name);
			new_name[3] = 'a';

			usepal = CurrentLoadingDDPalette;

			if (SpecialTexturePath)
				if (SpecialTexturePath[strlen(SpecialTexturePath)-1] == '\\')
					sprintf(path,"%s%s",SpecialTexturePath,new_name);
				else
					sprintf(path,"%s\\%s",SpecialTexturePath,new_name);
			else
				strcpy(path,RegPath("textures",new_name));

			if (!(ddsurf = LoadPCXDDSurface(path,FT_16BIT_DATA,TRUE,UT_PURE_ALPHA)) && SpecialTexturePath)
			{
				strcpy(path,RegPath("textures",new_name));
				ddsurf = LoadPCXDDSurface(path,FT_16BIT_DATA,TRUE,UT_PURE_ALPHA);
			}

			if (ddsurf)
			{
				DDSURFACEDESC2 ddsdsource,ddsddest;
				WORD *lpsource;
				WORD *lpdest;
				DWORD source_offset,dest_offset;
				int cx,cy;

				ZeroMemory(&ddsddest, sizeof(ddsddest));
				ddsddest.dwSize = sizeof(ddsddest);
				ddsddest.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

				returner->D3DSurface->Lock(NULL,&ddsddest,DDLOCK_SURFACEMEMORYPTR | DD_LOCK_FLAGS,NULL);

				lpdest = (WORD *)ddsddest.lpSurface;
				dest_offset = (ddsddest.lPitch >> 1) - ddsddest.dwWidth;

				ZeroMemory(&ddsdsource, sizeof(ddsdsource));
				ddsdsource.dwSize = sizeof(ddsdsource);
				ddsdsource.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

				ddsurf->Lock(NULL,&ddsdsource,DDLOCK_SURFACEMEMORYPTR | DD_LOCK_FLAGS,NULL);

				lpsource = (WORD *)ddsdsource.lpSurface;
				source_offset = (ddsdsource.lPitch>>1) - ddsddest.dwWidth;

				cy = ddsddest.dwHeight;
				while(cy--)
				{
					cx = ddsddest.dwWidth;
					while(cx--)
					{
						*lpdest &= ~ddsddest.ddpfPixelFormat.dwRGBAlphaBitMask;
			 			*lpdest++ |= *lpsource++ & ddsddest.ddpfPixelFormat.dwRGBAlphaBitMask;
					}

					lpdest += dest_offset;
					lpsource += source_offset;
				}

				returner->D3DSurface->Unlock(NULL);

				ddsurf->Unlock(NULL);

				ddsurf->SetPalette(NULL);
				ddsurf->Release();

			}

			if (last_walker)
				*last_walker = returner;
		}
	}

	if (returner)
		returner->Users++;

	return(returner);

}


TextureBuff *Load3DTexture(TextureRef *file, UsageType utype )
{
	TextureBuff *returner;
	TextureBuff *walker;
	TextureBuff **last_walker;

	if (!NoObjectCacheing)
	{
		last_walker = &TBRoot;
		walker = TBRoot;

		while(walker && ((walker->Usage != utype) || stricmp(walker->Info.Name,file->Name)))
		{
			last_walker = &walker->Next;
			walker = walker->Next;
		}
	}
	else
	{
		walker = NULL;
		last_walker = NULL;
	}

	if (walker)
		returner = walker;
	else
	{
		if (returner = FetchTexture(file,utype))
		{
			if (last_walker)
				*last_walker = returner;
		}
	}

	if (returner)
		returner->Users++;

	return(returner);

}

TextureBuff *Load3DTextureWithMipMaps(TextureRef *file, int maps,UsageType utype)
{
	TextureBuff *returner;
	TextureBuff *walker;
	TextureBuff **last_walker;

	if (!NoObjectCacheing)
	{
		last_walker = &TBRoot;
		walker = TBRoot;

		while(walker && ((walker->Usage != utype) || stricmp(walker->Info.Name,file->Name)))
		{
			last_walker = &walker->Next;
			walker = walker->Next;
		}
	}
	else
	{
		walker = NULL;
		last_walker = NULL;
	}

	if (walker)
		returner = walker;
	else
	{
		if (returner = FetchTextureWithMipMaps(file,maps,utype))
		{
			/* ------------------------------------------12/5/97 10:01AM-------------------------------------------
			 * I know, I know.  I shouldn't use strings like this. But sometimes you gotta take your lumps
			 * ----------------------------------------------------------------------------------------------------*/
			if (!_stricmp(file->Name,"hdpack.pcx"))
				BackTexture = returner;

			if (last_walker)
				*last_walker = returner;
		}
	}

	if (returner)
		returner->Users++;

	return(returner);

}



//******************************************************************************************************************

void ReloadTexture( TextureBuff *pTbuf, char *filename )
{
	TextureBuff *new_texture;
	TextureRef new_ref;

	if (pTbuf)
	{
		new_ref = pTbuf->Info;
		strcpy(new_ref.Name,filename);
		SetTextureFormat(pTbuf->Format);
		new_texture = FetchTexture(&new_ref,pTbuf->Usage);

		RemoveTexture(pTbuf);

		*pTbuf = *new_texture;

		EasyFree(new_texture);
	}

}

/* -----------------1/11/99 5:50PM---------------------------------------------------------------------
/*
/*  class TextureSaver
/*  {
/*  	public:
/*  		TextureSaver(void) {ZeroMemory(&ddsd,sizeof(ddsd));ddsd.dwSize = sizeof(ddsd);imagedata = NULL;};
/*  		~TextureSaver(void) {RemoveData();};
/*  		void RemoveData(void) {if (imagedata) free(imagedata);imagedata = NULL;};
/*  		void SwallowSurface(LPDIRECTDRAWSURFACE4);
/*  		LPDIRECTDRAWSURFACE4 RegurgitateSurface(void);
/*
/*  	private:
/*  		DDSURFACEDESC2	ddsd;
/*  		BYTE			*imagedata;
/*
/*  }
/*
/* ----------------------------------------------------------------------------------------------------*/


void TextureSaver::SwallowSurface(LPDIRECTDRAWSURFACE4 lp_dds,FormatType form,UsageType use)
{
	DDSURFACEDESC2 new_ddsd;

	lp_dds->GetSurfaceDesc(&ddsd);

	ddsd.ddsCaps.dwCaps &= (DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_ALPHABITDEPTH | DDSD_MIPMAPCOUNT);

	imagesize = ddsd.dwLinearSize * ddsd.dwHeight;
	imagedata = (BYTE *)malloc(imagesize);

	ZeroMemory(&new_ddsd,sizeof(new_ddsd));
	new_ddsd.dwSize = sizeof(new_ddsd);

	lp_dds->Lock(NULL,&new_ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,NULL);
	memcpy((void *)imagedata,new_ddsd.lpSurface,imagesize);
	lp_dds->Unlock(NULL);

	format = form;
	usage = use;

}

LPDIRECTDRAWSURFACE4 TextureSaver::RegurgitateSurface(void)
{
	LPDIRECTDRAWSURFACE4	ddsurface;
	DDSURFACEDESC2       	new_ddsd;
	LPDIRECTDRAWPALETTE 	usepal;
	DDPIXELFORMAT 			*lpddpf;
	BOOL					can_memcopy = FALSE;

	usepal = GamePals[((int)format) - 1];

	if ((usage == UT_DEFAULT) && (format == FT_16BIT_DATA))
	{
		lpddpf = &D3DTextureFormat.ddpf16bpp;
		usepal = NULL;
		can_memcopy = TRUE;
	}
	else
	{
		if (format > FT_16BIT_DATA)
			usepal = GamePals[((int)format) - 1];
		else
			usepal = NULL;

		if (usage != UT_DEFAULT)
		{
			switch (usage)
			{
				case UT_PURE_ALPHA:
					lpddpf = &D3DTextureFormat.ddpfAlpha;
					break;

				case UT_RGB_ALPHA:
					lpddpf = &D3DTextureFormat.ddpfColorAlpha;
					break;
			}
			can_memcopy = TRUE;
		}
		else
		{
			if (D3DTextureFormat.ddpf.dwRGBBitCount == 8)
				can_memcopy = TRUE;
			lpddpf = &D3DTextureFormat.ddpf;
		}
	}

	/* -----------------12/10/98 11:56AM-------------------------------------------------------------------
	/* let's create the other buffer first to avoid memory fragging
	/* ----------------------------------------------------------------------------------------------------*/

    ZeroMemory(&new_ddsd, sizeof(new_ddsd));
    new_ddsd.dwSize = sizeof(new_ddsd);

    new_ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

    new_ddsd.dwWidth = ddsd.dwWidth;
    new_ddsd.dwHeight = ddsd.dwHeight;

	memcpy(&new_ddsd.ddpfPixelFormat,lpddpf,sizeof(DDPIXELFORMAT));

	new_ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
   	new_ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

    if (lpDD->CreateSurface(&new_ddsd, &ddsurface, NULL) == DD_OK)
	{
		DDCOLORKEY key;
		memset(&key,0,sizeof(DDCOLORKEY));
		ddsurface->SetColorKey(DDCKEY_SRCBLT,&key);

		// set the palette if we have an 8 bit surface
		ZeroMemory(&new_ddsd,sizeof(new_ddsd));
		new_ddsd.dwSize = sizeof(new_ddsd);
		ddsurface->GetSurfaceDesc(&new_ddsd);

		if (new_ddsd.ddpfPixelFormat.dwRGBBitCount == 8)
			ddsurface->SetPalette(usepal);


		/* -----------------1/12/99 9:09AM---------------------------------------------------------------------
		/* okay, we have a surface, now we have to transfer the data in from the old surface.  If the surface
		/* formats are different, however, we can't just memcopy we have to actually translate through a palette
		/* ----------------------------------------------------------------------------------------------------*/

		if (can_memcopy)
		{
			ZeroMemory(&new_ddsd,sizeof(new_ddsd));
			new_ddsd.dwSize = sizeof(new_ddsd);
			if (ddsurface->Lock(NULL,&new_ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL) == DD_OK)
			{
				/* -----------------1/12/99 9:15AM---------------------------------------------------------------------
				/* if the pitch is the same, we can copy the whole thing in one big chunk
				/* ----------------------------------------------------------------------------------------------------*/
				if (new_ddsd.dwLinearSize == ddsd.dwLinearSize)
					memcpy(new_ddsd.lpSurface,imagedata,imagesize);
				else
				{
					/* -----------------1/12/99 9:21AM---------------------------------------------------------------------
					/* otherwise we have to copy line by line
					/* ----------------------------------------------------------------------------------------------------*/
					int y,pitch;
					BYTE *src,*dst;

					pitch = __min(new_ddsd.dwLinearSize,ddsd.dwLinearSize);
					dst = (BYTE *)new_ddsd.lpSurface;
					src = imagedata;
					y = new_ddsd.dwHeight;
					while(y--)
					{
						memcpy(dst,src,pitch);
						dst += new_ddsd.dwLinearSize;
						src += ddsd.dwLinearSize;
					}

				}
				ddsurface->Unlock(NULL);
			}

		}
		else
		{
			/* -----------------1/12/99 9:24AM---------------------------------------------------------------------
			/* okay, we have an 16 bit destination, but an 8 bit source.  What I'm gonna do is make an 8 bit surface
			/* in system memory and let windows do the conversion for us.
			/* ----------------------------------------------------------------------------------------------------*/
			DDSURFACEDESC2 ddsd8bit;
			LPDIRECTDRAWSURFACE4 lpdds8bit;

    		ZeroMemory(&ddsd8bit, sizeof(ddsd8bit));
    		ddsd8bit.dwSize = sizeof(ddsd8bit);

    		ddsd8bit.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    		ddsd8bit.dwWidth = ddsd.dwWidth;
    		ddsd8bit.dwHeight = ddsd.dwHeight;

			ddsd8bit.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			ddsd8bit.ddpfPixelFormat.dwRGBBitCount = 8;
			ddsd8bit.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
			ddsd8bit.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

			if (lpDD->CreateSurface(&ddsd8bit,&lpdds8bit,NULL) == DD_OK)
			{
				lpdds8bit->SetPalette(usepal);

				ZeroMemory(&new_ddsd,sizeof(new_ddsd));
				new_ddsd.dwSize = sizeof(new_ddsd);
				if (lpdds8bit->Lock(NULL,&new_ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL) == DD_OK)
				{
					if (new_ddsd.dwLinearSize == ddsd.dwLinearSize)
						memcpy(new_ddsd.lpSurface,imagedata,imagesize);
					else
					{
						/* -----------------1/12/99 9:21AM---------------------------------------------------------------------
						/* otherwise we have to copy line by line
						/* ----------------------------------------------------------------------------------------------------*/
						int y,pitch;
						BYTE *src,*dst;

						pitch = __min(new_ddsd.dwLinearSize,ddsd.dwLinearSize);
						dst = (BYTE *)new_ddsd.lpSurface;
						src = imagedata;
						y = new_ddsd.dwHeight;
						while(y--)
						{
							memcpy(dst,src,pitch);
							dst += new_ddsd.dwLinearSize;
							src += ddsd.dwLinearSize;
						}

					}
					ddsurface->Unlock(NULL);

					/* -----------------1/12/99 9:31AM---------------------------------------------------------------------
					/* okay, now the new surface has the right data, let's do a blit to get it over to the other surface
					/* ----------------------------------------------------------------------------------------------------*/

					HDC ddc;
					HDC sdc;

					ddsurface->GetDC(&ddc);
					lpdds8bit->GetDC(&sdc);
					BitBlt(ddc,0,0,ddsd.dwWidth,ddsd.dwHeight,sdc,0,0,SRCCOPY);
					ddsurface->ReleaseDC(ddc);
					lpdds8bit->ReleaseDC(sdc);

					/* -----------------1/12/99 9:33AM---------------------------------------------------------------------
					/* and, now that we're done with this surface, we'll get rid of it
					/* ----------------------------------------------------------------------------------------------------*/

					lpdds8bit->SetPalette(NULL);
					lpdds8bit->Release();
				}
//				else
//					ASSERT(0);

			}
//			else
//				ASSERT(0);
		}
	}
	else
		ddsurface = NULL;

	return ddsurface;
}

void HideTexturesFromD3DRelease(void)
{
	TextureBuff *walker;

	LPDIRECTDRAWSURFACE4 surface;
	TextureSaver *saver;

	walker = TBRoot;
	while(walker)
	{
		surface = walker->D3DSurface;
		saver = new TextureSaver;
		saver->SwallowSurface(surface,walker->Format,walker->Usage);
		walker->D3DSurface = (LPDIRECTDRAWSURFACE4)saver;
		walker->D3DTexture->Release();
		surface->SetPalette(NULL);
		surface->Release();

		walker = walker->Next;
	}
}

void RestoreTexturesFromHiding(void)
{
	TextureBuff *walker;

	TextureSaver *saver;

	walker = TBRoot;
	while(walker)
	{
		saver = (TextureSaver *)walker->D3DSurface;
		walker->D3DSurface = saver->RegurgitateSurface();
		walker->D3DSurface->QueryInterface(IID_IDirect3DTexture2,(void **)&walker->D3DTexture);
		delete saver;

		walker = walker->Next;
	}

}