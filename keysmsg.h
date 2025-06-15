// keysmsg.h
//

#ifndef __KEYSMSG_HEADER__
#define __KEYSMSG_HEADER__

#define KSYS_TOGGLE_SOUND					0x100
#define	KSYS_TOGGLE_TRANSITIONS				0x101
#define KSYS_TOGGLE_SPECTRAL_LT				0x102
#define KSYS_TOGGLE_ROADS					0x103
#define KSYS_TOGGLE_SHADOWS					0x104
#define KSYS_SCENE_COMPLEXITY				0x105
#define KSYS_OBJECT_COMPLEXITY				0x106
#define KSYS_TEXTURE_DETAIL					0x107
#define KSYS_DITHER_DETAIL					0x108
#define KSYS_PAUSE							0x109
#define KSYS_CHANGE_3DFXGAMMA				0x10A
#define KSYS_VIEW_DISTANCE					0x10B

BOOL KeyGetKeyMessage(DWORD dwKey, char *szMsg, int value);

#endif // __KEYSMSG_HEADER__
