// WrapGrab.cpp

#include "F18.h"
#include <io.h>

void _copy_displaydc(char *dst, int width, int height, int pitch, DWORD size, BITMAPINFO *pbm);
void _copy_surfacedc(LPDIRECTDRAWSURFACE4 lpDDS,char *dst, int width, int height, int pitch, DWORD size, BITMAPINFO *pbm);

void _screen_write(char * szFilename, LPVOID tmp, BITMAPINFO *pbm, DWORD size);

extern int g_iWrapperMode;

void NewScreenGrab( LPDIRECTDRAWSURFACE4 lpDDS )
{
	char *tmp, filename[MAX_PATH];
	struct _finddata_t fileinfo;
	int width, height, pitch;
	long filefind;
	long filestat=0;
	LPBITMAPINFO lpbmi;
	DWORD size;
	char txtnum[5];
	int num=0;
	
	filefind = _findfirst("DUMP*.BMP", &fileinfo);
	filestat=filefind;
	while(filestat != -1)
	{
		filestat = _findnext(filefind, &fileinfo);
		num++;
	}
	_findclose(filefind);

	strcpy(filename,"DUMP");
	sprintf(txtnum,"%03d",num);
	strcat(filename,txtnum);
	strcat(filename,".BMP");

	width  = ScreenSize.cx;//640;
	height = ScreenSize.cy;//480;
	pitch  =  3;//(bInSim) ? 1 :

	if (g_iWrapperMode==SIM_BUILDER)
	{
		char *pszPath = GetRegValue("resource");
		sprintf (filename, "%s\\brief.bmp", pszPath);
		width=384;
		height=240;
	}

	size = width * height * pitch;
	tmp = (char *)malloc(size);
	lpbmi = (LPBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + ((pitch==1) ? sizeof(RGBQUAD)*256 : 0));
	if( lpDDS )
		_copy_surfacedc(lpDDS, tmp, width, height, pitch, size, lpbmi);
	else
		_copy_displaydc(tmp, width, height, pitch, size, lpbmi);

	_screen_write(filename, tmp, lpbmi, size);
	free(lpbmi);
	free(tmp);
}

//
// local functions
//
void _copy_surfacedc(LPDIRECTDRAWSURFACE4 lpDDS,char *dst, int width, int height, int pitch, DWORD size, BITMAPINFO *pbm)
{
	HDC hdc;
	HDC memDC;
	HBITMAP obm,hbm;
	LPVOID lpdata;
	
	lpDDS->GetDC(&hdc);
	memDC = CreateCompatibleDC(hdc);

	pbm->bmiHeader.biSize			= sizeof( BITMAPINFOHEADER);
	pbm->bmiHeader.biWidth			= width; 
	pbm->bmiHeader.biHeight			= height; 
	pbm->bmiHeader.biPlanes			= 1; 
	pbm->bmiHeader.biBitCount		= pitch << 3;
	pbm->bmiHeader.biCompression	= BI_RGB; 
	pbm->bmiHeader.biSizeImage		= size;
	pbm->bmiHeader.biXPelsPerMeter	= 0; 
	pbm->bmiHeader.biYPelsPerMeter	= 0;
	pbm->bmiHeader.biClrUsed		= 0;
	pbm->bmiHeader.biClrImportant	= 0;

	if (pitch==1)
	{
		PALETTEENTRY TmpPal[256];
		LPRGBQUAD lprgb = pbm->bmiColors;

		lpDDSPalette->GetEntries(0,0,256,TmpPal);

		for (LPPALETTEENTRY p=TmpPal; p<&TmpPal[256]; p++, lprgb++)
		{
			lprgb->rgbRed = p->peRed;
			lprgb->rgbGreen = p->peGreen;
			lprgb->rgbBlue = p->peBlue;
			lprgb->rgbReserved = 0;
		}
		pbm->bmiHeader.biClrUsed = 256;
		pbm->bmiHeader.biClrImportant = 256;
	}

	hbm = CreateDIBSection(memDC, pbm, DIB_RGB_COLORS, &lpdata, NULL, 0);
	obm = (HBITMAP)SelectObject(memDC,hbm);

	if (g_iWrapperMode==SIM_BUILDER)
	{
		int iOffSetX=0;
		int iOffSetZ=0;
		if (ScreenSize.cx==640)
		{
			iOffSetX=128;
			iOffSetZ=120;
		}
		else if(ScreenSize.cx==800)
		{
			iOffSetX=208;
			iOffSetZ=360;
		}
		else if(ScreenSize.cx== 1024)
		{
			iOffSetX = 320;
			iOffSetZ = 264;
		}


		BitBlt(memDC,0,0,width,height,hdc,iOffSetX,iOffSetZ,SRCCOPY);
	}
	else
		BitBlt(memDC,0,0,width,height,hdc,0,0,SRCCOPY);

	CopyMemory(dst,lpdata,size);

	DeleteObject(SelectObject(memDC,obm));
	DeleteDC(memDC);
	lpDDS->ReleaseDC(hdc);

}



void _copy_displaydc(char *dst, int width, int height, int pitch, DWORD size, BITMAPINFO *pbm)
{
	HDC hdc;
	HDC memDC;
	HBITMAP obm,hbm;
	LPVOID lpdata;
	
	hdc = CreateDC("DISPLAY",NULL,NULL,NULL);
	memDC = CreateCompatibleDC(hdc);

	pbm->bmiHeader.biSize			= sizeof( BITMAPINFOHEADER);
	pbm->bmiHeader.biWidth			= width; 
	pbm->bmiHeader.biHeight			= height; 
	pbm->bmiHeader.biPlanes			= 1; 
	pbm->bmiHeader.biBitCount		= pitch << 3;
	pbm->bmiHeader.biCompression	= BI_RGB; 
	pbm->bmiHeader.biSizeImage		= size;
	pbm->bmiHeader.biXPelsPerMeter	= 0; 
	pbm->bmiHeader.biYPelsPerMeter	= 0;
	pbm->bmiHeader.biClrUsed		= 0;
	pbm->bmiHeader.biClrImportant	= 0;

	if (pitch==1)
	{
		PALETTEENTRY TmpPal[256];
		LPRGBQUAD lprgb = pbm->bmiColors;

		lpDDSPalette->GetEntries(0,0,256,TmpPal);

		for (LPPALETTEENTRY p=TmpPal; p<&TmpPal[256]; p++, lprgb++)
		{
			lprgb->rgbRed = p->peRed;
			lprgb->rgbGreen = p->peGreen;
			lprgb->rgbBlue = p->peBlue;
			lprgb->rgbReserved = 0;
		}
		pbm->bmiHeader.biClrUsed = 256;
		pbm->bmiHeader.biClrImportant = 256;
	}

	hbm = CreateDIBSection(memDC, pbm, DIB_RGB_COLORS, &lpdata, NULL, 0);
	obm = (HBITMAP)SelectObject(memDC,hbm);

	BitBlt(memDC,0,0,width,height,hdc,0,0,SRCCOPY);
	CopyMemory(dst,lpdata,size);

	DeleteObject(SelectObject(memDC,obm));
	DeleteDC(memDC);
	DeleteDC(hdc);
}


void _screen_write(char * szFilename, LPVOID tmp, BITMAPINFO *pbm, DWORD size)
{
	FILE *fp;
	DWORD dwSize;

	dwSize = pbm->bmiHeader.biClrUsed * sizeof(RGBQUAD);

	fp = fopen( szFilename, "wb" );
	BITMAPFILEHEADER bmpfh;

    bmpfh.bfType = 'MB';
    bmpfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size + dwSize;
    bmpfh.bfReserved1 = 0; 
    bmpfh.bfReserved2 = 0; 
    bmpfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwSize; 
	fwrite( &bmpfh, sizeof(BITMAPFILEHEADER), 1, fp);

	fwrite( &pbm->bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	if (dwSize) fwrite( pbm->bmiColors, dwSize, 1, fp);

	fwrite( tmp, 1, size, fp);
	fclose( fp );
}
