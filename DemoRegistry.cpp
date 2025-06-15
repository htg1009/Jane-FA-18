#include "F18.h"
#include "crtdbg.h"
#include "resource.h"
#include "WrapScreens.h"
#include "GameSettings.h"
#include "LoopProcess.h"

#define REG_TRANSLATIONS	"Translations"
#define REG_TERRAIN			"terrain"
#define REG_WORLDPATH		"WorldPath"
#define REG_MISSION			"mission"
#define REG_WORLDNAME		"WorldName"
#define	REG_OBJECTS			"objects"
#define REG_WRAPPER			"wrapper"
#define REG_COCKPITS		"Cockpits"
#define REG_DATA			"data"
#define REG_SOUND			"sound"
#define REG_MUSIC			"music"
#define	REG_MOVIES			"movies"
#define REG_TEXTURES		"textures"
#define REG_RESOURCE		"resource"
#define REG_CAMPAIGN		"campaign"
#define REG_TRAINING		"training"
#define REG_INSTALLTYPE		"InstallType"
#define REGKEY_KEYPLAYSTYLE	"Playstyle"

BOOL GetExePath (char *pszDstBuffer, int nDstBufferSize)
{
	char	*pPath;
	DWORD	dwLen;

	// Store off path so we can load wrapper.dll explicitly
	dwLen = GetModuleFileName (NULL, pszDstBuffer, nDstBufferSize);
	if (dwLen)
	{
		pPath = strrchr (pszDstBuffer, '\\');

		// Terminate filename so only path remains (without trailing backslash)
		if (pPath)
			*pPath = NULL;

		return (TRUE);
	}

	return (FALSE);
}

BOOL EstablishRegistrySettings (void)
{
	LONG				lError;
	HKEY				hKey;
	DWORD				dwDisp;
	DWORD				dwSize = sizeof (int);
	DWORD				dwStrLen;
	int					nGamePlay;
	char				szExePath[260];
	char				szBuffer[260];

	// Find path to our file.
	if (!GetExePath (szExePath, 260))
		return (FALSE);

	// Create key to manipulate registry
	lError = RegCreateKeyEx (HKEY_LOCAL_MACHINE, "Software\\Jane's Combat Simulations\\F18 Demo",
			0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &hKey, &dwDisp);

	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** WorldName ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	strcpy (szBuffer, "iraq");
	lError = RegSetValueEx (hKey, REG_WORLDNAME, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** Translations ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\lng", szExePath);
	lError = RegSetValueEx (hKey, REG_TRANSLATIONS, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** mission ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\mission\\", szExePath);
	lError = RegSetValueEx (hKey, REG_MISSION, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** terrain ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\iraq\\", szExePath);
	lError = RegSetValueEx (hKey, REG_TERRAIN, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** WorldPath ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\iraq\\", szExePath);
	lError = RegSetValueEx (hKey, REG_WORLDPATH, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** objects ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\iraq\\objects", szExePath);
	lError = RegSetValueEx (hKey, REG_OBJECTS, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** wrapper ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\wrapper", szExePath);
	lError = RegSetValueEx (hKey, REG_WRAPPER, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** Cockpits ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\Cockpits", szExePath);
	lError = RegSetValueEx (hKey, REG_COCKPITS, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** data ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\Data", szExePath);
	lError = RegSetValueEx (hKey, REG_DATA, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** sound ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\sound", szExePath);
	lError = RegSetValueEx (hKey, REG_SOUND, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** music ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\Sound", szExePath);
	lError = RegSetValueEx (hKey, REG_MUSIC, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** movies ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\Movies", szExePath);
	lError = RegSetValueEx (hKey, REG_MOVIES, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** textures ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\iraq\\objects", szExePath);
	lError = RegSetValueEx (hKey, REG_TEXTURES, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** resource ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\resource", szExePath);
	lError = RegSetValueEx (hKey, REG_RESOURCE, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** campaign ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\Campaign", szExePath);
	lError = RegSetValueEx (hKey, REG_CAMPAIGN, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** training ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	sprintf (szBuffer, "%s\\training\\", szExePath);
	lError = RegSetValueEx (hKey, REG_TRAINING, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	//////////////////////////////////////////////
	//  Check ** InstallType ** 
	//////////////////////////////////////////////
	dwStrLen = 256;
	strcpy (szBuffer, "2");
	lError = RegSetValueEx (hKey, REG_INSTALLTYPE, 0, REG_SZ, (BYTE *)szBuffer, strlen (szBuffer) + 1);
	if (lError != ERROR_SUCCESS)
		return (FALSE);

	RegCloseKey (hKey);

	// Create key to manipulate registry
	lError = RegCreateKeyEx (HKEY_LOCAL_MACHINE, "Software\\Jane's Combat Simulations\\F18 Demo\\Config",
			0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &hKey, &dwDisp);

	//////////////////////////////////////////////
	//  Check ** Playstyle ** 
	//////////////////////////////////////////////
	dwStrLen = sizeof (nGamePlay);
	nGamePlay = 0;
	lError = RegQueryValueEx (hKey, REGKEY_KEYPLAYSTYLE, 0, NULL, (BYTE *)&nGamePlay, &dwStrLen);
	if (lError != ERROR_SUCCESS)
	{
		lError = RegSetValueEx (hKey, REGKEY_KEYPLAYSTYLE, 0, REG_DWORD, (BYTE *)&nGamePlay, sizeof (int));
		if (lError != ERROR_SUCCESS)
			return (FALSE);
	}



	return (TRUE);
}
