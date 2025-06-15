/*****************************************************************************
*	Jane's F/A-18
*
*	DebugHelper.cpp  
*-----------------------------------------------------------------------------
*	by IAM
*****************************************************************************/

#include "F18.h"
#include "grfont.h"
//#include "grfileio.h"


//Global declarations:
FILE *fp=NULL;

bool checkIfFileExist(char cRegEntry[16], char cLogFile[20])
{	
	//iam: function to check if a file exist - returns true (1) or false (0)	
	struct _stat buffer;
	int result;
	
	result = _stat(RegPath(cRegEntry,cLogFile),&buffer);
	
	if (result == 0) return TRUE;

	return FALSE;
}

void logDisplaySettings()
{
	//iam: This function enumerates display settings

	DWORD  iDevNum = 0 ; 
	DWORD  iModeNum = 0 ; 
	DISPLAY_DEVICE ddi ; 
	DEVMODE  dmi ; 
	TCHAR  szBuffer [100] ; 
	ZeroMemory (&ddi, sizeof(ddi)) ; 
	ddi.cb = sizeof(ddi) ; 
	ZeroMemory (&dmi, sizeof(dmi)) ; 
	dmi.dmSize = sizeof(dmi) ; 
	DWORD previous_settings = 0 ;
	
	char cRegEntry[6] = "DEBUG";
	char cLogFile[24] = "EnumDisplaySettings.log";

	//if file exist - delete it, we start with a fresh file @ each execution...
	if (checkIfFileExist(cRegEntry,cLogFile)) remove(RegPath(cRegEntry,cLogFile));
	
	//open log file	
	fp = fopen(RegPath(cRegEntry,cLogFile),"a+t");

	while (EnumDisplayDevices (NULL, iDevNum++, &ddi, 0)) 
	{  
		while (EnumDisplaySettings (ddi.DeviceName, iModeNum++, &dmi))
		{	
			//log entry only if resolution is 800x600 or rgeater, higher than 8bpp and skip identical settings with different refresh-rate:
			if (dmi.dmPelsWidth >= 800 && dmi.dmBitsPerPel > 8 && previous_settings != dmi.dmPelsWidth+dmi.dmBitsPerPel)
			{
				sprintf (szBuffer, "%s (ModeIndex):%i %ix%ix%i %ihz\n",ddi.DeviceString, iModeNum,dmi.dmPelsWidth, dmi.dmPelsHeight, dmi.dmBitsPerPel, dmi.dmDisplayFrequency) ;

				if (fp)	fwrite(szBuffer,strlen(szBuffer),1,fp);

				previous_settings = dmi.dmPelsWidth+dmi.dmBitsPerPel;

				ZeroMemory (&dmi, sizeof(dmi)) ;
				ddi.cb = sizeof(ddi) ;
			}
			
		}
		fclose(fp);
		
		ZeroMemory (&ddi, sizeof(ddi)) ;
		ddi.cb = sizeof(ddi) ;
		iModeNum = 0 ;
	}
}

