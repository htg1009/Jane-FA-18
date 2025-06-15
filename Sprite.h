//////////////////////////////////////////
// Sprite class definition
//	derived from the CSprite class, but minus the MFC garbage
//		JDugan	7Mar97
//////////////////////////////////////////

#ifndef __SPRITE_HEADER__
#define	__SPRITE_HEADER__
#include "ddraw.h"
//#include "Control.h"

#pragma warning(disable: 4275)	// Derived Class exported from non exported class warning

// Do we REALLY need to type 19 letters for this stupid thing?
#define	LPDDSURF			LPDIRECTDRAWSURFACE4

class __declspec(dllexport) CSprite
{
// Construction
public:
	CSprite (void);
	virtual ~CSprite (void);

// Attributes
private:
	DWORD					m_dwWidth;
	DWORD					m_dwHeight;
	BOOL					m_bTransparent;
	DDCOLORKEY				m_ddColorKey;
	RECT					m_rcDefault;

public:
	LPDIRECTDRAWSURFACE4	m_pSurf;

// Operations
public:
	// Simple info type routines
	DWORD					Width (void) { return (m_dwWidth); };
	DWORD					Height (void) { return (m_dwHeight); };
	BOOL					IsLoaded (void);

	// Simple draw function, full image blitting assumed
	BOOL					Draw (HDC hDrawDC, int nDstX, int nDstY);
	BOOL					Draw (LPDDSURF pDrawSurf, int nDstX, int nDstY);

	// Non Clipping direct draw-BltFast call.
	// !!! *** USE WITH EXTREME CAUTION *** !!!
	// - MAKE SURE YOU KNOW THAT CLIPPING ISN'T NEEDED!
	BOOL					DrawFast (LPDDSURF pDrawSurf, int x, int y);

	// Use DrawClip to blit only a portion of image; Note that source width & height are optional
	BOOL					DrawClip (HDC hDrawDC, int nDstX, int nDstY, int nSrcX, int nSrcY, int nDstWidth = -1, int nDstHeight = -1);
	BOOL					DrawClip (LPDDSURF pDrawSurf, int nDstX, int nDstY, int nSrcX, int nSrcY, int nDstWidth = -1, int nDstHeight = -1);

	// Load from file resource
	BOOL					Load (LPDIRECTDRAW4 pDD, int nResource, BOOL bTopLeftTransparent = FALSE, LPDDCOLORKEY pColorKey = NULL);

	//	Load from file on disk
	BOOL					Load (LPDIRECTDRAW4 pDD, char *pszFile, BOOL bTopLeftTransparent = FALSE, LPDDCOLORKEY pColorKey = NULL);

	// Load from memory image of file
	BOOL					Load (LPDIRECTDRAW4 pDD, LPBYTE pMemImage, BOOL bTopLeftTransparent = FALSE, LPDDCOLORKEY pColorKey = NULL);

	// Called when done
	void					Free (void);

	// Called to inform sprite that DirectDraw is no longer loaded, surface is invalid
	void					DirectDrawFreed (void);

private:
	BOOL					CreateImage (LPDIRECTDRAW4 pDD, LPBYTE pMemFile, BOOL bTopLeftTrans, LPDDCOLORKEY pColorKeyTransparent);
	void					ResetVars (void);
	HDC						GetSurfDC (void);
	void					ReleaseSurfDC (HDC hDC);
	BOOL					BltClip (HDC hDrawDC, int nDstX, int nDstY, int nSrcX, int nSrcY, int nDstWidth, int nDstHeight);
};

/////////////////////////////////////////////////////////
// Load sprite routine for bitmaps loaded as resources in this dll.
/////////////////////////////////////////////////////////
CSprite *LoadResourceBitmap (char *pszString, LPDIRECTDRAW4 pDirDraw, BOOL bTransparent);

/////////////////////////////////////////////////////////
// Simple client functions to allocate and free sprites in one easy step
/////////////////////////////////////////////////////////
__declspec(dllexport) CSprite *AllocSprite (void);
__declspec(dllexport) void FreeSprite (CSprite *pSprite);

// If bTransparent is true, the TOP-LEFT pixel and all other pixels with the same RGB are transparent
// Otherwise, if pColorKey is not NULL, then IT will be used as transparent color value
__declspec(dllexport) CSprite *AllocSprite (LPDIRECTDRAW4 pDD, LPSTR lpSpriteFile, BOOL bTopLeftTransparent = FALSE, LPDDCOLORKEY pColorKey = NULL);
__declspec(dllexport) CSprite *AllocSprite (LPDIRECTDRAW4 pDD, int nResourceID, BOOL bTopLeftTransparent = FALSE, LPDDCOLORKEY pColorKey = NULL);

#endif