// GameSettings.h
//

#ifndef __GAMESETTINGS_HEADER__
#define __GAMESETTINGS_HEADER__

// flight model bit settings, bit=1 == ENABLED
//#define GP_FLIGHT_REALISTIC_FLIGHT					0x00000001
#define GP_FLIGHT_REALISTIC_LANDINGS					0x00000002
#define GP_FLIGHT_CRASHES								0x00000004
#define GP_FLIGHT_WEAPON_WEIGHT_DRAG					0x00000008
#define GP_FLIGHT_LIMITED_FUEL							0x00000010
#define GP_FLIGHT_AIR_COLLISIONS						0x00000020
#define GP_FLIGHT_READOUT_BLACKOUT						0x00000040
//#define GP_FLIGHT_VULNERABLE							0x00000080
#define GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL				0x00000100
#define GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_TOLERANCE	0x00000100	//same
#define GP_FLIGHT_REALISTIC_AUTOPILOTS					0x00000200
#define GP_FLIGHT_GFORCE_BREATH							0x00000400
#define GP_FLIGHT_REALISTIC_INFLIGHT_REFUEL_DURATION	0x00000800



// weapon bit settings, bit=1 == ENABLED
//#define GP_WEAPON_LIMITED_AMMO						0x00000001
#define GP_WEAPON_REAL_GUN_ACCURACY						0x00000002
#define GP_WEAPON_REAL_MISSILE_ACCURACY					0x00000004
#define GP_WEAPON_REAL_BOMB_ACCURACY					0x00000008
#define GP_WEAPON_REAL_WEAPON_DAMAGE					0x00000010


// cheat bit settings, bit=1 == ENABLED
#define GP_CHEATS_INVULNERABLE							0x00000001
#define GP_CHEATS_UNLIMITED_AMMO						0x00000002
#define GP_CHEATS_DESTRUCTO_CAM							0x00000004
#define GP_CHEATS_AIRCRAFT_LABELS						0x00000008
#define GP_CHEATS_GRND_TARG_LABELS						0x00000010
#define GP_CHEATS_VOICE_WARNINGS						0x00000020
#define GP_CHEATS_NAV_SPEECH							0x00000040
#define GP_CHEATS_NAV_VISUAL							0x00000080
#define GP_CHEATS_TARG_SPEECH							0x00000100
#define GP_CHEATS_TARG_VISUAL							0x00000200
#define GP_CHEATS_MULTI_REGEN							0x00000400
#define GP_CHEATS_MULTI_REARM							0x00000800
#define GP_CHEATS_RADIO									0x00001000
#define GP_CHEATS_HUD_THROTTLE							0x00002000




// graphics bit settings, bit=1 == ENABLED
#define GP_GRAPH_SHADOWS								0x00000001
#define GP_GRAPH_SUNGLARE								0x00000002
#define GP_GRAPH_CLOUDS									0x00000004
#define GP_GRAPH_LENSFLARE								0x00000008
#define GP_GRAPH_LIFT_LINE								0x00000010
#define GP_GRAPH_SHOCK_WAVES							0x00000020
#define GP_GRAPH_ROADS									0x00000040
#define GP_GRAPH_WEAPONS_ON_PLANE						0x00000080
#define GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT			0x00000100
#define GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT				0x00000200
#define GP_GRAPH_TEXTURE_TRANSITIONS					0x00000400
#define GP_GRAPH_SPECTRAL_LIGHTING						0x00000800
#define GP_GRAPH_POINT_LIGHT_SRC						0x00001000
#define GP_GRAPH_LIGHT_MAPS								0x00002000
#define GP_GRAPH_CANOPY_REFLECTION						0x00004000
#define GP_GRAPH_CLOUD_TRANSITION						0x00008000
#define GP_GRAPH_VOLUMETRIC_CLOUDS						0x00010000
#define GP_GRAPH_VOLUMETRIC_EXPLOD						0x00020000
#define GP_GRAPH_LOW_LEVEL_NOISE						0x00040000
#define GP_GRAPH_SPARKLING_WATER						0x00080000
#define GP_GRAPH_RAIN_SNOW								0x00100000
#define GP_GRAPH_LIGHTNING								0x00200000
#define GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT			0x00400000
#define GP_GRAPH_RESOLUTION_800							0x00800000
#define GP_GRAPH_RESOLUTION_1024						0x01000000
#define GP_GRAPH_HIGH_DETAIL_TEXTURE					0x02000000




// miscellaneous bit settings, bit=1 == ENABLED
#define GP_MISC_INFO_TIPS								0x00000001
#define GP_MISC_INTRO_VIDEO								0x00000002
#define GP_MISC_TRAILER_VIDEO							0x00000004
#define GP_MISC_VIEW_PADLOCK_FOV_LIMITS					0x00000008
#define GP_MISC_VIEW_MPDS_POPUPS_STICKY					0x00000010
#define GP_MISC_UNUSED									0x00000020
#define GP_MISC_JANES_VIDEO								0x00000040
#define GP_MISC_VIEW_REVERSE_PITCH						0x00000080
#define GP_MISC_VIEW_REVERSE_YAW						0x00000100
#define GP_MISC_SHOW_MULTI_INFO							0x00000200

// gameplay type
#define GP_TYPE_CASUAL									0
#define GP_TYPE_ADVANCED								1
#define GP_TYPE_CUSTOM									2

// graphics type
#define GP_GRTYPE_SIMPLE								0
#define GP_GRTYPE_CUSTOM								1

// avionics, inflight refueling
#define GP_AUTHENTIC									0
#define GP_SIMPLE										1
#define GP_ARCADE										2

#define GP_RELAXED										GP_SIMPLE
#define GP_EASY											GP_ARCADE

// enemy skill level
#define GP_ACE											0
#define GP_EXPERT										1
#define GP_VETERAN										2
#define GP_NOVICE										3

// detail values
#define GP_DLOW											0
#define GP_DMEDIUM										1
#define GP_DHIGH										2

// graphic detail values
#define GP_NONE											0
#define GP_LOW											1
#define GP_MEDIUM										2
#define GP_HIGH											3

#define GP_REQUIRED_ONLY								GP_NONE
#define GP_SOME											GP_LOW
#define GP_ALL											GP_MEDIUM
#define GP_HOLLYWOOD									GP_DHIGH
#define GP_OFF											GP_NONE

#define GP_NEAR											GP_DLOW
#define GP_FAR											GP_DHIGH


// graphic slider levels
#define GP_GRMIN										0
#define GP_GRMED										2
#define GP_GRMAX										5

// default gamma value
#define GP_GAMMA_DEFAULT								1.0f

// default sound volume
#define GP_VOLUME_DEFAULT								100

#define GP_SOUND_HOLLYWOOD								0x0001

//#define	GP_NOPAGEFLIPPING								0x01
//#define GP_LIGHT_MAPS									0x02
//#define GP_RES800x600									0x04
//#define GP_TERRAIN_DETAIL								0x08
//#define GP_WATER_DETAIL									0x10
//#define GP_WATER_SPARKLE								0x20
//#define GP_CLOUDS_VOLUMETRIC							0x40
//#define GP_POINT_LIGHTS									0x80


#define	GP_BURNER_BASE									0x01
#define GP_NO_EXTERNAL_SOUNDS_IN_COCKPIT				0x02


#pragma pack(push)
#pragma pack(1)

typedef struct _GAMEPLAY
{
	DWORD		dwFlags;								// reserved flags
	DWORD		dwFlight;								// GP_FLIGHT_ bit settings
	DWORD		dwWeapons;								// GP_WEAPON_ bit settings
	DWORD		dwCheats;								// GP_CHEATS_ bit settings

	BYTE		nAARADAR;								// GP_TYPE_CASUAL/ADVANCED
	BYTE		nAGRADAR;								//  will ALWAYS match nAARADAR
	BYTE		nTEWS;									// GP_TYPE_CASUAL/ADVANCED
	BYTE		nFLIR;									// No longer used

	BYTE		nEnemyAircraft;							// GP_ACE/EXPERT/VETERAN/NOVICE
	BYTE		nGroundDefense;							// GP_ACE/EXPERT/VETERAN/NOVICE
	BYTE		nType;									// GP_TYPE_CASUAL/ADVANCED/CUSTOM

} GAMEPLAY, *PGAMEPLAY;


typedef struct _GRAPHICS
{
	BYTE		byFlags;								// reserved flags

	BYTE		nSceneComplexity;						// scene complexity 0-3
	BYTE		nType;									// GP_GRTYPE_SIMPLE/GP_GRTYPE_CUSTOM

	DWORD		dwGraph;								// GP_GRAPH_ bit settings

	BYTE		nNumGroundObjs;							// GP_REQUIRED/SOME/ALL
	BYTE		nObjectDetail;							// GP_DLOW/DMEDIUM/DHIGH

	BYTE		nExplosionDetail;						// GP_DLOW/DMEDIUM/DHIGH
	BYTE		nGroundSmoke;							// GP_OFF/GP_LOW/GP_MEDIUM

	BYTE		nTerrainDistance;						// GP_NEAR/MEDIUM/FAR

	DWORD		nLevel;									// GP_GRMIN..GP_GRMAX


	//BYTE		nWeaponSmoke;							// GP_OFF/GP_LOW/GP_MEDIUM
	//float		fGamma;									// gamma value
	//BYTE		nCalcLight;								// GP_NONE/LOW/MEDIUM/HIGH
	//BYTE		nTerrainTexturing;						// GP_NONE/LOW/MEDIUM/HIGH
	//BYTE		nTerrainDithering;						// GP_NONE/LOW/MEDIUM/HIGH
} GRAPHICS, *PGRAPHICS;


typedef struct _SOUNDVOLUME
{
	BYTE		byReserved1;
	BYTE		byReserved2;
	BYTE		byReserved3;
	BYTE		byExplosions;				// Explosion Sound Levels
	BYTE		byCautionSeekTones;			// Caution Tones
	BYTE		byEngine;					// Internal Engine Sounds
	BYTE		byExternalSFX;				// Misc. External Sounds
	BYTE		byRadioSpeech;				// Radio Speech
	BYTE		byMenuMusic;				// Wrapper Menu Music
	BYTE		byMenuSfx;
	BYTE		byBetty;					// Betty Speech
	BYTE		byExternalAircraftLevel;	// Aircraft Speech
	BYTE		byFlags;					// Flags
} SOUNDVOLUME, *PSOUNDVOLUME;


typedef struct _JOYPROFILE
{
	DWORD		dwFlags;
	BYTE		byProfile[9];
	BYTE		byDeband;
	BYTE		byPad;
} JOYPROFILE;


typedef struct _MISCSET
{
	WORD		wPad;
	BYTE		byPad;
	BYTE		byACMPanSpeed;							// ACM pan speed 0-255
	DWORD		dwMisc;									// GP_MISC_ bit settings
	BOOL		bFilter;								// enable Joystick filtering
	JOYPROFILE	axes[3];								// Pitch/Roll/Yaw profiles
	DWORD		dwMIL;									// Thrust MIL setting
	POINT		ptCenter;								// Deadband center
} MISCSET, *PMISCSET;


typedef struct _MPDLOC
{
	int			MPDOrder[3];
	int			MPDSlot[3];
} MPDLOC, *PMPDLOC;


typedef struct _FFBACK
{
	BOOL	bEnable;									// ForceFeedback ON
	BYTE	byMasterGain;

	BYTE	byGunRecoil;								// All Scaled 0..100
	BYTE	byMissleRelease;
	BYTE	byWeaponRelease;
	BYTE	byHit;
	BYTE	byExplosions;
	BYTE	byStall;
	BYTE	byTransonic;
	BYTE	byCarrierTakeoff;
	BYTE	byCarrierLanding;
	BYTE	byGroundRoll;
	BYTE	byDitch;
	BYTE	byCrash;

}FFBACK, *PFFBACK;

typedef struct _GAMESETTINGS
{
	DWORD		dwFlags;								// reserved flags
	DWORD		dwVersion;								// settings magic/version value
	GAMEPLAY	gp;										// settings related to gameplay
	GRAPHICS	gr;										// settings related to visual detail
	SOUNDVOLUME	snd;									// settings related to audio detail
	MISCSET		misc;									// miscellaneous settings
	MPDLOC		mpdloc;									// MPD view locations
	FFBACK		ff;										// settings related to force feedback
} GAMESETTINGS, *PGAMESETTINGS;

#pragma pack(pop)

extern GAMESETTINGS g_Settings;							// Global settings

#endif // __GAMESETTINGS_HEADER__