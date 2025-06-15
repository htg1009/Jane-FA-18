#ifndef __BIGBIRD_MULTIPLAYER_HEADER__
#define __BIGBIRD_MULTIPLAYER_HEADER__

#include "dplay.h"

#define PROTOCOL_IPX		0
#define	PROTOCOL_TCPIP		1
#define PROTOCOL_MODEM		2
#define	PROTOCOL_SERIAL		3

#define TEAM_RED			0
#define TEAM_YELLOW			1
#define TEAM_BLUE			2
#define TEAM_BLACK			3
#define TEAM_GREEN			4
#define TEAM_REDBLACK		5
#define TEAM_YELLOWBLACK	6
#define TEAM_BLUEBLACK		7
#define TEAM_TOTAL			8

#define MAX_MULTI_SESSIONS	256
#define MAX_MULTI_PLAYERS	8

// Bitmasks for dwUser1
#define MULTIMASK_EAIR_SKILL	0x0003
#define MULTIMASK_EGRND_SKILL	0x000C
#define MULTIMASK_TANK_TOL		0x0010
#define MULTIMASK_TANK_DUR		0x0020
#define MULTIMASK_A_LABELS		0x0040
#define MULTIMASK_G_LABELS		0x0080
#define MULTIMASK_REARM			0x0100


#define	MULTIMASK_TIME		0x0003  //  Not used
#define	MULTIMASK_WEATHER	0x000C  //  Not used
#define MULTIMASK_ARMING	0x00F0  //  Not used
#define MULTIMASK_SHOWPOS	0x0100  //  Not used
#define	MULTIMASK_LOCKCFG	0x0200  //  Not used
#define MULTIMASK_REGEN		0x0400  
#define MULTIMASK_AGRADAR	0x0800  //  Not used
#define MULTIMASK_TFLIR		0x1000  //  Not used
#define MULTIMASK_GNDDEF	0x6000  //  Not used
#define MULTIMASK_USEME		0x8000  //  Not used

// Bitmasks for dwUser2
#define MULTIMASK_AMMO		0x0001
#define MULTIMASK_GUNACCY	0x0002
#define MULTIMASK_MISSACCY	0x0004
#define MULTIMASK_BOMBACCY	0x0008
#define MULTIMASK_WPNDAMAGE	0x0010
#define MULTIMASK_FLIGHT	0x0020
#define MULTIMASK_LANDINGS	0x0040
#define MULTIMASK_CRASHES	0x0080
#define MULTIMASK_WEAPDRAG	0x0100
#define MULTIMASK_LIMITFUEL	0x0200
#define MULTIMASK_COLLISION	0x0400
#define MULTIMASK_REDOUT	0x0800
#define MULTIMASK_AARADAR	0x3000
#define MULTIMASK_TEWS		0xC000

// Function to quickly get value from dpsessiondesc.
int		GetMultiSetting (int nSetting);

typedef struct
{
	char		szGameName[64];
	int			nPlayersAllowed;
	int			nPlayersActual;
	int			nTimeOfDay;
	int			nWeather;
	int			nDisplayPosition;
	int			nConfiguration;
	int			nLoadout;
	int			nRegeneration;
	int			nGndDefenses;

	// new stuff

	BOOL		bLimitAmmo;
	BOOL		bGunAccurate;
	BOOL		bMissileAccurate;
	BOOL		bBombAccurate;
	BOOL		bWeaponDamage;
	BOOL		bRealFlight;
	BOOL		bCrashes;
	BOOL		bLimitFuel;
	BOOL		bMidAirHits;
	BOOL		bRedOut;
	int			nAARadar;
	int			nTEWS;
} MULTIGAMEDATA;


#endif