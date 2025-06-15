// keystuff.h

#ifndef __KEYSTUFF_HEADER__
#define __KEYSTUFF_HEADER__

#define KEYINIFILE		"F18keys.ini"
#define KEYINIFILE_F	"F18keysf.ini"
#define KEYINIFILE_G	"F18keysg.ini"

#define KEYBUFFSIZE	4096

// Language virtual key codes

#define VK_NONE						0x00

// English specific vk codes

#define VK_A						'A'
#define VK_B						'B'
#define VK_C						'C'
#define VK_D						'D'
#define VK_E						'E'
#define VK_F						'F'
#define VK_G						'G'
#define VK_H						'H'
#define VK_I						'I'
#define VK_J						'J'
#define VK_K						'K'
#define VK_L						'L'
#define VK_M						'M'
#define VK_N						'N'
#define VK_O						'O'
#define VK_P						'P'
#define VK_Q						'Q'
#define VK_R						'R'
#define VK_S						'S'
#define VK_T						'T'
#define VK_U						'U'
#define VK_V						'V'
#define VK_W						'W'
#define VK_X						'X'
#define VK_Y						'Y'
#define VK_Z						'Z'
#define VK_SEMICOLON				0xBA
#define VK_EQUAL					0xBB
#define VK_COMMA					0xBC
#define VK_MINUS					0xBD
#define VK_PERIOD					0xBE
#define VK_BACK_SLASH				0xBF
#define VK_ACCENT					0xC0
#define VK_LBRACKET					0xDB
#define VK_SLASH					0xDC
#define VK_RBRACKET					0xDD
#define VK_APOSTROPHE				0xDE

// French specific vk codes

#define VK_A_FR						'A'
#define VK_B_FR						'B'
#define VK_C_FR						'C'
#define VK_D_FR						'D'
#define VK_E_FR						'E'
#define VK_F_FR						'F'
#define VK_G_FR						'G'
#define VK_H_FR						'H'
#define VK_I_FR						'I'
#define VK_J_FR						'J'
#define VK_K_FR						'K'
#define VK_L_FR						'L'
#define VK_M_FR						'M'
#define VK_N_FR						'N'
#define VK_O_FR						'O'
#define VK_P_FR						'P'
#define VK_Q_FR						'Q'
#define VK_R_FR						'R'
#define VK_S_FR						'S'
#define VK_T_FR						'T'
#define VK_U_FR						'U'
#define VK_V_FR						'V'
#define VK_W_FR						'W'
#define VK_X_FR						'X'
#define VK_Y_FR						'Y'
#define VK_Z_FR						'Z'
#define VK_TWO_FR					0xDE
#define VK_COMMA_FR					0xBC
#define VK_ACCENTU_FR				0xC0
#define VK_SEMICOLON_FR				0xBE
#define VK_COLON_FR					0xBF
#define VK_EXCLAMATION_FR			0xDF
#define VK_CARROT_FR				0xDD
#define VK_DOLLAR_FR				0xBA
#define VK_RPARENTHES_FR			0XDB
#define VK_EQUAL_FR					0xBB
#define VK_LESSTHAN_FR				0xE2
#define VK_ASTERIK_FR				0xDC

// German specific vk codes

#define VK_A_GR						'A'
#define VK_B_GR						'B'
#define VK_C_GR						'C'
#define VK_D_GR						'D'
#define VK_E_GR						'E'
#define VK_F_GR						'F'
#define VK_G_GR						'G'
#define VK_H_GR						'H'
#define VK_I_GR						'I'
#define VK_J_GR						'J'
#define VK_K_GR						'K'
#define VK_L_GR						'L'
#define VK_M_GR						'M'
#define VK_N_GR						'N'
#define VK_O_GR						'O'
#define VK_P_GR						'P'
#define VK_Q_GR						'Q'
#define VK_R_GR						'R'
#define VK_S_GR						'S'
#define VK_T_GR						'T'
#define VK_U_GR						'U'
#define VK_V_GR						'V'
#define VK_W_GR						'W'
#define VK_X_GR						'X'
#define VK_Y_GR						'Y'
#define VK_Z_GR						'Z'
#define VK_MINUS_GR					0xBD
#define VK_OMLATO_GR				0xC0
#define VK_OMLATA_GR				0xDE
#define VK_OMLATU_GR				0xBA
#define VK_PLUS_GR					0xBB
#define VK_BETA_GR					0xDB
#define VK_ACCENT_GR				0xDD
#define VK_PERIOD_GR				0xBE
#define VK_COMMA_GR					0xBC
#define VK_POUND_GR					0xBF
#define VK_LESSTHAN_GR				0xE2
#define VK_SEMICOLON_GR				0xBC
#define VK_CARROT_GR				0xDC

//

#define MAXSECTION_LEN				256

// KEYBOARDMAP bit flags
#define KEYF_NO_DEP					0x00000000
#define KEYF_S_DEP					0x00000001		// Sensor category
#define KEYF_W_DEP					0x00000002		// Weapon
#define KEYF_F_DEP					0x00000004		// Flight
#define KEYF_A_DEP					0x00000008		// Avionic
#define KEYF_G_DEP					0x00000010		// Graphic
#define KEYF_V_DEP					0x00000020		// View
#define KEYF_C_DEP					0x00000040		// Coms
#define KEYF_M_DEP					0x00000080		// Misc
#define KEYF_NOTEMPLEMENT			0x10000000		// Indicates null function (not implemented yet)
#define KEYF_DEBUGONLY				0x20000000		// function available in debug version only
#define KEYF_REPEAT					0x40000000		// function can be repeated.
#define KEYF_ALLOW_PAUSE			0x80000000		// function can be accessed during paused game

#define KEYFLAGS_MASK				0x000000ff

// KEYASSIGN bit flags
#define KEYASSIGNF_DEBOUNCE			0x00000001		// debounce check flag
#define KEYASSIGNF_DISABLE			0x00000002		// is set if the key is disabled

#define LOOKF_KEYSTROKE				0				// vkCode corresponds to a Win32 Virtual Key code
#define LOOKF_JOY1					1				// vkCode corresponds to primary joy button bit mask
#define LOOKF_JOY2					2				// vkCode corresponds to secondary joy button bit mask
#define LOOKF_JOYHAT1				3				// vkCode corresponds to a primary button hat
#define LOOKF_JOYHAT2				4				// vkCode corresponds to a secondary button hat

typedef struct _VKCODE
{
	WORD	vkCode;									// virtual key code (may be joy button)
	WORD	wFlags;									// vkCode Type
	DWORD	dwExtraCode;							// any extra VK code, eg. SHIFT, CTRL, ALT
} VKCODE, *PVKCODE;


typedef struct _VKLOOKUP
{
	char			*pszSortName;					// name used for sorting, NULL use Friendlyname
	char			*pszFriendlyName;				// keystroke friendly name - actual .ini specification
	WORD			vkCode;							// corresponding VK code
	WORD			wFlags;							// vkCode Type
	DWORD			dwLookup;						// ordinal ID value
	DWORD			dwExtraCode;					// any extra VK code, eg. SHIFT, CTRL, ALT
} VKLOOKUP, *PVKLOOKUP;


typedef struct _KEYBOARDMAP
{
	char			*pszFriendlyName;				// keyboard function friendly name
	void			(*pfnKeyFunction)(VKCODE vk);	// function code
	DWORD			dwLangID;						// Infotip language ID
	DWORD			dwFlags;
	DWORD			dwSysMsgID;						// non-zero ID to display toggle activation message
} KEYBOARDMAP, *PKEYBOARDMAP;


typedef struct _KEYASSIGN
{
	WORD			vkCode;							// virtual key code
	WORD			wFlags;							// vkCode Type
	DWORD			dwExtraCode;					// extra VK code, eg. SHIFT, CTRL, ALT
	LONG			nKeyCount;						// number of function assignments
	DWORD			dwFlags;
	KEYBOARDMAP		**pKeyList;						// pointer list to keyMap
	void			(*pfnOverrideFn)(VKCODE vk);	// override function, APP can set this
} KEYASSIGN, *PKEYASSIGN;


typedef struct _KEYLAYOUT
{
	DWORD			dwCount;
	char			szSectionName[MAXSECTION_LEN];
	KEYASSIGN		*pKeys;
} KEYLAYOUT, *PKEYLAYOUT;


typedef enum
{
	EG_NO_ANSWER,
	EG_WANTS_OUT,
	EG_WANTS_STAY,
	EG_FORCE_DWORD = 0xFFFFFFFF
} ENDGAMERESONSE;


extern ENDGAMERESONSE dwPlayerEndGameResponse;

// super high level calls
DWORD InitDefaultKeyAssignments(void);
void FreeDefaultKeyAssignments(void);

// high level calls
extern "C" __declspec( dllexport ) DWORD KeyAllocateAssignments(char *pszSection, KEYLAYOUT *pKeyLayout);
extern "C" __declspec( dllexport ) void KeyFreeAssignments(KEYLAYOUT *pKeyLayout);
void KeySetDefaultLayout(KEYLAYOUT *pLayout);
void KeyClearAllDebounce(void);

void KeySysMessageKey(int value);

// new med level calls
VKLOOKUP * KeyFunctionNameToVkCode(char *pszFunctionName);
BOOL KeyEnableKey(char *pszKeyFriendlyName, BOOL bEnable);

void KeyEnableSystem (BOOL bEnable);
BOOL KeyIsSystemEnabled (void);

// process function calls
BOOL KeyExcecuteKey(LONG vkCode);									// call during WM_KEYDOWND
BOOL KeyScanButtonLayout(void);										// call during joyGetPosEx
BOOL KeyScanButtonLayoutForEndMenu(void);  // called when endmenu is activated
extern "C" __declspec( dllexport ) BOOL KeyOkExtraCode(KEYASSIGN *p);
extern "C" __declspec( dllexport ) BOOL GetVkStatus(VKCODE vk);		// call anytime to get VKCODE state

// low level calls
VKLOOKUP *KeyFriendlyToVkCode(char *pszFriendlyName);
KEYBOARDMAP *KeyFriendlyToFunction(char *pszFriendlyName);
DWORD KeyAssignKey(char *pszSection, VKLOOKUP *pVk, KEYASSIGN *pKey);

char *GetFullIniPathName(void);

 // pointer to the current keyboard layout
extern KEYLAYOUT *gpCurrentLayout;

// keystroke function list - in keyfunc.cpp
extern KEYBOARDMAP keyMap[];
extern KEYBOARDMAP *gpLastMap; // last map key executed

// used for Wrappers to access KEYBOARDMAP list
extern "C" __declspec( dllexport ) KEYBOARDMAP *GetKeyboardMapList(void);
extern "C" __declspec( dllexport ) VKLOOKUP *GetVkLookup(void);


void cam_front_view(VKCODE vk);
#endif // __KEYSTUFF_HEADER__