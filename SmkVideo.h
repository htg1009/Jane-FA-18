#ifndef __CSMKVIDEO_HEADER__
#define __CSMKVIDEO_HEADER__

#include "smack.h"
#include "ddraw.h"

// Event flags, passed as WPARAM in notify message
#define	SMKVIDEO_DONE			1

// Notify message sent to m_hWndCallback, if not NULL; WPARAM = event
#define	CWM_SMKVIDEO_NOTIFY		(WM_USER+4613)

class __declspec(dllexport) CSmkVideo
{
private:
	Smack *					m_pSmack;
	LPDIRECTDRAW4			m_pDirDraw;
	LPDIRECTDRAWSURFACE4	m_pSurf;
	LPDIRECTDRAWSURFACE4	m_pRenderSurf;
	LPDIRECTDRAWSURFACE4	m_pOverlay;
	LPDIRECTDRAWSURFACE4	m_pMixSurf;
	DDSURFACEDESC2			m_ddsd;
	LPDIRECTDRAWPALETTE		m_pDirPal;
	PALETTEENTRY			m_pal[256];
	u32						m_nLastKey;
	u32						m_nSurfaceType;   // zero if 8-bit surface, otherwise RGB bitmask
	BOOL					m_bPlaying;
	BOOL					m_bSoundOn;
	BOOL					m_bTransparent;
	int						m_nLoopCount;
	void *					m_hDigDriver;
	int						m_nX;
	int						m_nY;
	HWND					m_hWndCallback;
	RECT					m_rcOverlaySrc;
	RECT					m_rcOverlayDst;
	BOOL					m_bOverlayActive;

protected:
	u32						GetSurfaceType (LPDIRECTDRAWSURFACE4 pSurface);
	void					InitPal (HDC hDC);
	void					Free (void);
	void					ResetVars (void);
	void					DrawCurrentFrame (void);
	void					SetPal(void);
	void					NotifyCallback (int nEvent);

public:
							CSmkVideo (void);
							~CSmkVideo (void);
	void					Init (LPDIRECTDRAW4 pDirDraw, LPDIRECTDRAWSURFACE4 pSurf, void *hDigDriver, LPDIRECTDRAWPALETTE pPal, HWND hWndCallback);
	BOOL					Load (char *pszSmkVideo, int x, int y, BOOL bTransparent);
	void					NextSmackerFrame (void);
	void					InitRenderSurface (void);
	void					Play (void);
	void					Stop (void);
	void					Pause (void);
	void 					Resume (void);
	void					Close (void);
	void 					SetLoopCount (int nCount);
	void					EnableSound (BOOL bEnabled);
	void					DrawStill (void);
	void					Service (void);
	void					SetSpriteOverlay (LPDIRECTDRAWSURFACE4 pSpriteSurf, LPRECT prcSrc, LPRECT prcDst);
	void					DisplaySpriteOverlay (BOOL bShow);
};

#endif