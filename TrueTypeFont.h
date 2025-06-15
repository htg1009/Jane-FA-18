#ifndef __TRUE_TYPE_FONT_HEADER__
#define __TRUE_TYPE_FONT_HEADER__

#include <ddraw.h>
#include "Control.h"
#include "flyfont.h"
#define TTF_ALIGN_LEFT		0
#define TTF_ALIGN_CENTER	1
#define TTF_ALIGN_RIGHT	2

#define TTFONTCOLOR_PRIMARY		0
#define TTFONTCOLOR_HIGHLIGHT	1
#define TTFONTCOLOR_DISABLED	2
#define TTFONTCOLOR_USER		3
#define NUM_TRUETYPE_PALETTES	4
#define	NUM_TRUETYPE_PALENTRIES	16

#define GETPALETTE(index)		(m_Palettes + (index * NUM_TRUETYPE_PALENTRIES))

class __declspec(dllexport) CTrueTypeFont
{
protected:
	int						m_rSizeShift;
	int						m_gSizeShift;
	int						m_bSizeShift;
	int						m_rPlaceShift;
	int						m_gPlaceShift;
	int						m_bPlaceShift;
	int						m_rMask;
	int						m_gMask;
	int						m_bMask;
	int						m_nDstPitch;
	int						m_dwSurfHeight;
	int						m_nFontHeight;
	int						m_nSpaceWidth;
	void *					m_pFontFile;
	ARGB					m_Palettes[NUM_TRUETYPE_PALETTES * NUM_TRUETYPE_PALENTRIES];

protected:
	void					GetMaskBits (DWORD dwValue, int *pnSizeShift, int *pnPlaceShift);
	void					GetPixelMasks (PDDSURF pSurf);
	void					FreeFont (void);
	void					Reset (void);
	void					SetFontPaletteNormal (ARGB foreargb, ARGB backargb, int nPaletteIndex);
	void					SetFontPalette(ARGB Palette[16], ARGB foreargb);
	char *					GetNextLine (char *pszText, int nWidth);

public:
							CTrueTypeFont (void);
							CTrueTypeFont (PDDSURF pSurface);
							~CTrueTypeFont (void);
	void					SetSurface (PDDSURF pSurface);
	int						LoadFont (BYTE *pszFontMemoryImage, DWORD dwSize );
	int						LoadFont (char *pszFullPath);
	void					GetTextRect(const char* pCaption, int* pX, int* pY, int* pW, int* pH);
	void					AlphaBlendPreAttenuatedPixels (WORD *pD, const ARGB *pS);
	void					DrawShape4to16(void *pSurf, int nDstPitch, int nDstHeight, const SHAPE* pShape4, int sx1, int sy1, int u, int v, int w, int h, int nPalette);
	void					DrawText16 (void * pDst, int x, int y, const char* str, int nPalette,int iLength=-1);
	void					DrawTextCount16 (int nCount, void *pDst, int x, int y, const char *str, int nPalette);
	void					SetFontColors (COLORREF clrPrimary, COLORREF clrHilite = 0, COLORREF clrDisable = 0, COLORREF clrUser = 0);
	int						GetTextExtent (char *pszString);
	int						GetHeight (void) { return (m_nFontHeight); };
	void					DrawTextEx (void *pSurf, LPRECT prc, char *pszText, int nFlags = TTF_ALIGN_LEFT, int nPalette = TTFONTCOLOR_PRIMARY);
	int						Paginate (int nWidth, char *pszBuffer, char **ppszLinePointers, int nMaxLines);
	int						GetFontHeight (void) { return (this ? m_nFontHeight : 0); };
	int						GetCharWidth (unsigned char byChar);
};

#endif