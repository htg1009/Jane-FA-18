#include "Sprite.h"
#include "crtdbg.h"

#ifndef PDIRDRAW
#define PDIRDRAW LPDIRECTDRAW4
#endif

#ifndef PDDSURF
#define PDDSURF LPDIRECTDRAWSURFACE4
#endif

#ifndef DDSURFDESC
#define DDSURFDESC DDSURFACEDESC2
#endif

void DoAllocFailedMessageBox (void)
{
	WORD	wLangId = GetUserDefaultLangID ();
	WORD	wPrimary = PRIMARYLANGID(wLangId);
	char *	pszError;

	switch (wPrimary)
	{
		case LANG_FRENCH:
			pszError = "Il y a une erreur dans l'attribution de l'espace mémoire.";
			break;

		case LANG_GERMAN:
			pszError = "Es ist zu einem Fehler bei der Speicherzuweisung gekommen.";
			break;

		case LANG_ENGLISH:
		default:
			pszError = "There is an error allocating surface.";
			break;
	}

	MessageBox (NULL, pszError, NULL, MB_OK|MB_TOPMOST);
}

void CSprite::ResetVars (void)
{
	m_dwWidth = m_dwHeight = 0;
	m_bTransparent = 0;
	memset (&m_ddColorKey, 0, sizeof (DDCOLORKEY));
	SetRect (&m_rcDefault, 0, 0, 0, 0);
	m_pSurf = NULL;
}

CSprite::CSprite (void)
{
	ResetVars ();
}

void CSprite::Free (void)
{
	if (m_pSurf)
		m_pSurf->Release ();

	ResetVars ();
}

void CSprite::DirectDrawFreed (void)
{
	m_pSurf = NULL;
}

CSprite::~CSprite (void)
{
	Free ();
}

BOOL CSprite::DrawClip (HDC hDrawDC, int nDstX, int nDstY, int nSrcX, int nSrcY, int nDstWidth, int nDstHeight)
{
	return (BltClip (hDrawDC, nDstX, nDstY, nSrcX, nSrcY, nDstWidth, nDstHeight));
}

BOOL CSprite::DrawClip (LPDDSURF pDrawSurf, int nDstX, int nDstY, int nSrcX, int nSrcY, int nDstWidth, int nDstHeight)
{
	HRESULT			hResult;
	DDBLTFX			ddBltFx;
	DWORD			dwFlags;
	RECT			rcDst;
	RECT			rcSrc;
	int				nFinalWidth;
	int				nFinalHeight;

	if (!m_pSurf)
		return (FALSE);

	nFinalWidth = (nDstWidth == -1) ? m_dwWidth : nDstWidth;
	nFinalHeight = (nDstHeight == -1) ? m_dwHeight : nDstHeight;

	if (nFinalWidth > (int) m_dwWidth)
		nFinalWidth = m_dwWidth;

	if (nFinalHeight > (int) m_dwHeight)
		nFinalHeight = m_dwHeight;

	rcDst.left = nDstX;
	rcDst.top = nDstY;
	rcDst.right = nDstX + nFinalWidth;// - 1;
	rcDst.bottom = nDstY + nFinalHeight; // - 1;

	rcSrc.left = nSrcX;
	rcSrc.top = nSrcY;
	rcSrc.right = nSrcX + nFinalWidth; // - 1;
	rcSrc.bottom = nSrcY + nFinalHeight; // - 1;

	ddBltFx.dwSize = sizeof (ddBltFx);
	dwFlags = m_bTransparent ? DDBLT_KEYSRC | DDBLT_WAIT : DDBLT_WAIT;

	hResult = pDrawSurf->Blt (&rcDst, m_pSurf, &rcSrc, dwFlags, &ddBltFx);

	return (hResult == DD_OK);
}

BOOL CSprite::Draw (LPDDSURF pDrawSurf, int x, int y)
{
	return (DrawClip (pDrawSurf, x, y, 0, 0, -1, -1));
}

BOOL CSprite::Draw (HDC hDrawDC, int x, int y)
{
	return (BltClip (hDrawDC, x, y, 0, 0, -1, -1));
}

BOOL CSprite::DrawFast (LPDDSURF pDrawSurf, int x, int y)
{
	DWORD	dwFlags = m_bTransparent ? DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT : DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT;
	HRESULT	hResult = pDrawSurf->BltFast (x, y, m_pSurf, &m_rcDefault, dwFlags);
	return (hResult == DD_OK);
}

BOOL CSprite::BltClip (HDC hDrawDC, int nDstX, int nDstY, int nSrcX, int nSrcY, int nDstWidth, int nDstHeight)
{
	BOOL		bRet;
	int			nWidth, nHeight;
	POINT		pt;
	HDC			hWorkDC;

	hWorkDC = GetSurfDC ();
	if (!hWorkDC)
		return (FALSE);

	// If default width & height used, get dimensions from object itself
	pt.x = (nDstWidth == -1) ? m_dwWidth : nDstWidth;
	pt.y = (nDstHeight == -1) ? m_dwHeight : nDstHeight;

	// Convert from device coordinates to logical coordinates in case mapping mode of DC isn't MM_TEXT
	DPtoLP (hDrawDC, &pt, 1);
	nWidth = pt.x;
	nHeight	= pt.y;

	// Copy image onto drawing surface
	bRet = BitBlt (hDrawDC, nDstX, nDstY, nWidth, nHeight, hWorkDC, nSrcX, nSrcY, SRCCOPY);
	_ASSERT(bRet);

	ReleaseSurfDC (hWorkDC);

	return (bRet);
}


BOOL CSprite::Load (PDIRDRAW pDD, int nResourceID, BOOL bTopLeftTransparent, LPDDCOLORKEY pColorKey)
{
	HMODULE hModule;
	HGLOBAL hGlobal;
	HRSRC hrsrc;

	hModule = GetModuleHandle("SKUNKUTL.DLL");
	hrsrc = FindResource(hModule,MAKEINTRESOURCE(nResourceID),"RAWDATA");

	if (hrsrc && (hGlobal = LoadResource(hModule, hrsrc))!=NULL)
	{
		LPBYTE pData = (LPBYTE)LockResource(hGlobal);

		if (pData)
			return Load (pDD, pData, bTopLeftTransparent, pColorKey);
	}

	return FALSE;
}

BOOL CSprite::Load (PDIRDRAW pDD, char *pszFile, BOOL bTopLeftTransparent, LPDDCOLORKEY pColorKey)
{
	HANDLE		hFile;
	DWORD		dwLength;
	LPBYTE		pData;
	DWORD		dwRead = 0;
	BOOL		bRet = FALSE;

	hFile = CreateFile (pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		dwLength = GetFileSize (hFile, NULL);

		// load into memory
		pData = (BYTE *)malloc (dwLength);
		if (pData)
		{
			bRet = ReadFile (hFile, pData, dwLength, &dwRead, NULL);
			_ASSERT (bRet && (dwLength == dwRead));

			// if read successfully into memory,
			//	call version which works with raw memory images
			if (dwRead == dwLength)
				bRet = Load (pDD, pData, bTopLeftTransparent, pColorKey);

			free (pData);
		}

		CloseHandle (hFile);
	}

	return (bRet);
}

BOOL CSprite::Load (PDIRDRAW pDD, LPBYTE pMemImage, BOOL bTopLeftTransparent, LPDDCOLORKEY pColorKey)
{
	BOOL	bOk;

	// Free previous sprite data, if any
	Free ();
	bOk = CreateImage (pDD, pMemImage, bTopLeftTransparent, pColorKey);

	if (!bOk)
		DoAllocFailedMessageBox ();

	return (bOk);
}

extern LPDIRECTDRAWSURFACE4	lpDDSPrimary;			  // DirectDraw primary surface

BOOL CSprite::CreateImage (PDIRDRAW pDD, LPBYTE pData, BOOL bTopLeftTransparent, LPDDCOLORKEY pColorKeyTransparent)
{
	HBITMAP				hbmImage;
	HBITMAP				hbmOld;
	BITMAPINFOHEADER *	pBmpHeader;
	BITMAPFILEHEADER *	pBmpFile;
	BITMAP				bmp;
	HDC					hDC, hSurfaceDC;
	BYTE *				pBuffer;
	BYTE *				pColors;
	UINT				uSize, uColorSize;
	BYTE *				pReadPtr;
	DDSURFACEDESC2		ddsd;
	HRESULT				hResult;
	BOOL				bOk;
	HDC					hHotDC;

	pColors = pBuffer = NULL;

#ifndef _SKUNKCTL_DLL
	if (lpDDSPrimary)
		lpDDSPrimary->GetDC(&hHotDC);
	else
		hHotDC = GetDC (NULL);
#else
	hHotDC = GetDC (NULL);
#endif

	// Get Screen compatible dc
	hDC = CreateCompatibleDC (hHotDC);

	// Set header & read pointers
	pReadPtr = pData;
	pBmpFile = (LPBITMAPFILEHEADER)pData;

	// Calculate size of buffer & allocate
	uSize = pBmpFile->bfSize - pBmpFile->bfOffBits;
	uColorSize = pBmpFile->bfSize - uSize;

	// Allocate buffer to hold DIB bits
	pBuffer = (BYTE *)malloc (uSize);
	if (!pBuffer)
	{
		DoAllocFailedMessageBox ();
		return (FALSE);
	}

	// Allocate buffer to hold Color Data
	pColors = (BYTE *)malloc (uColorSize);
	if (!pColors)
	{
		DoAllocFailedMessageBox ();
		return (FALSE);
	}

	// Read Color Data and BitmapInfoHeader data in one shot
	pReadPtr = pData + sizeof (BITMAPFILEHEADER);
	memcpy (pColors, pReadPtr, uColorSize);

	// Set header pointer
	pBmpHeader = (LPBITMAPINFOHEADER)pColors;

	// Read in image bits
	pReadPtr = pData + pBmpFile->bfOffBits;
	memcpy (pBuffer, pReadPtr, uSize);

	// Set width & height member variables
	m_dwWidth = pBmpHeader->biWidth;
	m_dwHeight = pBmpHeader->biHeight > 0 ? pBmpHeader->biHeight : -pBmpHeader->biHeight;

	// Set default rectangle for bltting
	SetRect (&m_rcDefault, 0, 0, m_dwWidth, m_dwHeight);

	// Create DDB from DIB data, and get DDB info
	hbmImage = CreateDIBitmap (hHotDC, pBmpHeader, CBM_INIT, pBuffer, (BITMAPINFO *)pColors, DIB_RGB_COLORS);
	_ASSERT (hbmImage);
	GetObject (hbmImage, sizeof (BITMAP), &bmp);

	// Select new bitmap image into our temp dc
	hbmOld = (HBITMAP) SelectObject (hDC, hbmImage);
	_ASSERT (hbmOld);

	// Initialize DDSURFACEDESC2 with sprite surface description
	memset (&ddsd, 0, sizeof (ddsd));
	ddsd.dwSize = sizeof (ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
	ddsd.dwHeight = m_dwHeight;
	ddsd.dwWidth = m_dwWidth;

	// Create directdraw surface
	hResult = pDD->CreateSurface (&ddsd, &m_pSurf, NULL);
	_ASSERT (hResult == DD_OK);

	// Copy bitmap image from hbitmap onto directdraw surface
	if (hResult == DD_OK)
	{
		hResult = m_pSurf->GetDC (&hSurfaceDC);
		if (hResult == DD_OK)
		{
			bOk = BitBlt (hSurfaceDC, 0, 0, m_dwWidth, m_dwHeight, hDC, 0, 0, SRCCOPY);
			_ASSERT (bOk);
			m_pSurf->ReleaseDC (hSurfaceDC);

			// Set ColorKey value, if applicable
			// Determine value to be used as transparent color value, based on parameters
			if (bTopLeftTransparent || pColorKeyTransparent)
			{
				m_bTransparent = TRUE;

				if (bTopLeftTransparent)
				{
					/*
					clrRef = GetPixel (hDC, 0, 0);
					ddsd.dwSize = sizeof (ddsd);
					ddsd.dwFlags = DDSD_HEIGHT|DDSD_WIDTH;
					hResult = pSurf->GetSurfaceDesc (&ddsd);
					// Get top left pixel value and store it as transparent color
					m_ddColorKey.dwColorSpaceLowValue = m_ddColorKey.dwColorSpaceHighValue = GetPixel (hDC, 0, 0);
					*/


//					WORKS, but not optimal solution
					// change to accept rgb values & create value

					memset (&ddsd, 0, sizeof (ddsd));
					ddsd.dwSize = sizeof (ddsd);
					hResult = m_pSurf->Lock (NULL, &ddsd, 0, NULL);
					_ASSERT (hResult == DD_OK);

					if (hResult == DD_OK)
					{
						LPWORD	pWord = (LPWORD) ddsd.lpSurface;
						WORD	w = *pWord;
						m_ddColorKey.dwColorSpaceLowValue = m_ddColorKey.dwColorSpaceHighValue = w;
						m_pSurf->Unlock (NULL);
					}
				}
				else if (pColorKeyTransparent) 	// Take client's transparent color request value as-is
					m_ddColorKey = *pColorKeyTransparent;

				hResult = m_pSurf->SetColorKey (DDCKEY_SRCBLT, &m_ddColorKey);
				_ASSERT (hResult == DD_OK);
			}
		}
	}

	// Put original bitmap back into our temp dc
	SelectObject (hDC, hbmOld);

	// Finally, release all resources and memory buffers used
	if (hDC)
		DeleteDC (hDC);
	if (pBuffer)
		free (pBuffer);
	if (pColors)
		free (pColors);
	if (hbmImage)
		DeleteObject (hbmImage);

#ifndef _SKUNKCTL_DLL
	if (hHotDC)
		if (lpDDSPrimary)
			lpDDSPrimary->ReleaseDC(hHotDC);
		else
			ReleaseDC (NULL, hHotDC);
#else
	if (hHotDC)
		ReleaseDC (NULL, hHotDC);
#endif

	return (m_pSurf != NULL);
}

CSprite *AllocSprite (void)
{
	CSprite *pNew = new CSprite;
	return (pNew);
}

void FreeSprite (CSprite *pDDSprite)
{
	delete (pDDSprite);
}


CSprite *AllocSprite (PDIRDRAW pDD, int nResourceID, BOOL bTopLeftTransparent, LPDDCOLORKEY pColorKey)
{
	BOOL	bLoaded;
	CSprite *pNew;

	pNew = new CSprite;
	if (pNew)
	{
		// Attempt to load sprite specified
		bLoaded = pNew->Load (pDD, nResourceID, bTopLeftTransparent, pColorKey);

		if (!bLoaded)
		{
			// If unable to load it, free sprite and return NULL
			FreeSprite (pNew);
			pNew = NULL;
		}
	}

	return (pNew);
}


CSprite *AllocSprite (PDIRDRAW pDD, LPSTR lpSpriteFile, BOOL bTopLeftTransparent, LPDDCOLORKEY pColorKey)
{
	BOOL	bLoaded;
	CSprite *pNew;

	pNew = new CSprite;
	if (pNew)
	{
		// Attempt to load sprite specified
		bLoaded = pNew->Load (pDD, lpSpriteFile, bTopLeftTransparent, pColorKey);

		if (!bLoaded)
		{
			// If unable to load it, free sprite and return NULL
			FreeSprite (pNew);
			pNew = NULL;
		}
	}

	return (pNew);
}

HDC CSprite::GetSurfDC (void)
{
	HRESULT		hResult;
	HDC			hDC;

	if (m_pSurf)
	{
		hResult = m_pSurf->GetDC (&hDC);
		if (hResult != DD_OK)
		{
			m_pSurf->Restore ();
			hResult = m_pSurf->GetDC (&hDC);
		}

		if (hResult == DD_OK)
			return (hDC);
	}

	return (NULL);
}

void CSprite::ReleaseSurfDC (HDC hDC)
{
	if (m_pSurf && hDC)
		m_pSurf->ReleaseDC (hDC);
}

BOOL CSprite::IsLoaded (void)
{
	return (m_pSurf != NULL);
}