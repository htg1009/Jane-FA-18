// keysmsg.cpp
//

#include "F18.h"
#include "keystuff.h"
#include "keysmsg.h"

#define _SYES_NO(n) ( (n) ? "Yes" : "No")
#define _SON_OFF(n) ( (n) ? "On" : "Off")

BOOL KeyGetKeyMessage(DWORD dwKey, char *szMsg, int value)
{
	// This is for us English language people only
	if (g_iLanguageId==MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN) ||
		g_iLanguageId==MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH))
		return FALSE;

	switch (dwKey)
	{
		case KSYS_PAUSE:
			if (value)
				wsprintf(szMsg,"Simulation Paused");
			break;

		case KSYS_TOGGLE_ROADS:
			wsprintf(szMsg, "Roads %s", _SON_OFF(value));
			break;

		case KSYS_TOGGLE_SHADOWS:
			wsprintf(szMsg, "Shadows %s", _SON_OFF(value));
			break;

		case KSYS_TOGGLE_SPECTRAL_LT:
			wsprintf(szMsg, "Spectral Lighting %s", _SON_OFF(!value));
			break;

		case KSYS_TOGGLE_TRANSITIONS:
			wsprintf(szMsg, "Transitions %s", _SON_OFF(value));
			break;

		case KSYS_SCENE_COMPLEXITY:
			strcpy(szMsg, "Scene Complexity = ");

			if (value>3)
				value &= BI_DETAIL;

			switch (value)
			{
				case BI_DETAIL_0:
					strcat(szMsg,"0");
					break;

				case BI_DETAIL_1:
				case 1:
					strcat(szMsg,"1");
					break;

				case BI_DETAIL_2:
				case 2:
					strcat(szMsg,"2");
					break;

				case BI_DETAIL_3:
				case 3:
					strcat(szMsg,"3");
					break;
			}
			break;

		case KSYS_OBJECT_COMPLEXITY:
			wsprintf(szMsg, "Object Complexity = %d", value);
			break;

		case KSYS_TEXTURE_DETAIL:
			wsprintf(szMsg, "Texture Detail = %d", value);
			break;

		case KSYS_DITHER_DETAIL:
			wsprintf(szMsg, "Dither Detail = %d", value);
			break;

		case KSYS_CHANGE_3DFXGAMMA:
			wsprintf(szMsg, "3DFX Gamma = %0.2f", (float)value / 10.0f);
			break;

		case KSYS_TOGGLE_SOUND:
			wsprintf(szMsg, "Sound %s", _SON_OFF(value));
			break;

		case KSYS_VIEW_DISTANCE:
			wsprintf(szMsg, "View Distance = %d", value);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}