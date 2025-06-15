#ifndef __F18MAIN_HEADER__
#define __F18MAIN_HEADER__

/////////////////////////////////////////////////////////////////
// Defines used in F18Main.cpp
/////////////////////////////////////////////////////////////////

#define	GAMEWNDCLASS		"Jane's F18"
#define SIMWNDCLASS		"F18 Simulation"
#define GAMETITLE		"Jane's F18"
#define	FULLSCREEN_WIDTH	640
#define FULLSCREEN_HEIGHT	480

/////////////////////////////////////////////////////////////////
// Function prototypes used in F18Main.cpp
/////////////////////////////////////////////////////////////////
BOOL		InitInstance (void);
void		DoRender (void);
BOOL		GameAlreadyRunning (void);
BOOL		CreateGameWindow (void);
BOOL		CreateSimWindow (LPVOID lpUser);
LRESULT		CALLBACK GameWndProc (HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParm);
LRESULT		CALLBACK SimWndProc (HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParm);
BOOL		LoadWrappers (DWORD dwStartScreen);
void		FreeWrappers (void);
void		StartSim (LPVOID lpUser);
void		StopSim (void);
void		LeaveSimEnterWrappers (DWORD dwState);
void		LeaveWrappersEnterSim (DWORD dwState);

/////////////////////////////////////////////////////////////////
// Variables used throughout game
/////////////////////////////////////////////////////////////////
extern char	g_szResourceFile[260];

#endif