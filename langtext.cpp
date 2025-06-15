//*******************************************************************************
//*  Langtext.cpp
//*
//*  This file contains functions dealing with language and text actions.
//*******************************************************************************
#define SRELANGTEXTSTUFF
#include "F18.h"
#include "resources.h"
#include "spchcat.h"
#include "GameSettings.h"
#include "3dfxF18.h"
#include "particle.h"

#define DONOSOUNDS 1

extern BOOL	g_bIAmHost;
extern int	MySlot;		// my player's slot index [0..N]
extern FPointDouble LaserLoc;
extern int		TimeExcel;
extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
extern DBRadarType *GetRadarPtr(long id);
extern DetectedPlaneListType CurFramePlanes;
extern DBWeaponType *get_weapon_ptr(long id);
// extern RadarInfoType RadarInfo;
extern CurSelectedTargetInfoType CurTargetInfo;
extern DetectedPlaneListType PrevFramePlanes;
extern void StopFFForPauseOrEnd();
//extern DetectedPlaneListType Age1Planes;

#define AA_MAX_AGE_PLANES 10
extern DetectedPlaneListType AgePlanes[AA_MAX_AGE_PLANES];

extern int   NumDTWSTargets;
extern int   DTWSTargets[];
//extern void FixBridges();
extern void StartEngineSounds();
extern DWORD g_dwAIMSndHandle;
extern DWORD g_dwAOASndHandle;
extern DWORD g_dwAIMLockSndHandle;
extern void KillSound( DWORD *SndHandle );
extern void setup_no_cockpit_art();
extern void DoGHeartBeat( PlaneParams *P );

extern int iWatchMAISends;
extern int XmitChannel;

extern HWND	g_hGameWnd;

extern int g_iBaseSpeechSoundLevel;
extern void do_sim_done(VKCODE vk);
extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
extern int		AA_weapID;		// WeapID of priority missile
extern int Aim9X_Id;
extern int Aim9L_Id;
extern int Aim9M_Id;
extern WeapStoresType WeapStores;
extern int InteractCockpitMode;


int WrapperGetLanguageId (void)
{
	return (g_iLanguageId);
}

void InitLanguage (LPSTR lpCmdLine)
{
	LANGID			wLangId;
	WORD			wPrimary;
	char			sLanguage[30];

	// set default language
	g_iLanguageId = MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US);

	char *pszLanguage = GetRegValue ("language");

	if (pszLanguage)
	{
		strncpy(sLanguage,pszLanguage,30);

		if (!strcmp(sLanguage,"german"))
		{
			g_iLanguageId = MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN);
		}
		if (!strcmp(sLanguage,"GERMAN"))
		{
			g_iLanguageId = MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN);
		}
#if 0
		if (!strcmp(sLanguage,"french"))
		{
			g_iLanguageId = MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH);
		}
		if (!strcmp(sLanguage,"FRENCH"))
		{
			g_iLanguageId = MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH);
		}
#endif
	}
	else
	{
		wLangId = GetUserDefaultLangID ();
		wPrimary = PRIMARYLANGID(wLangId);

		switch (wPrimary)
		{
#if 0
			case LANG_FRENCH:
				g_iLanguageId = MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH);
			break;
#endif
			case LANG_GERMAN:
				g_iLanguageId = MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN);
			break;
			default:
			break;
		}
	}
}

//***********************************************************************************************************************************
BOOL LANGGetTransMessage(char *pszBuffer, DWORD maxbuffsize, DWORD msgid, DWORD languageid, ... )
//*  char *pszBuffer -  Is where the message string will be put.
//*  DWORD maxbuffersize -  Is the size of pszBuffer.
//*  DWORD msgid -  Is the id of the message we want.  These are gotten from the .H file created
//*                 when the .MC file is compiled with mc.exe
//*  DWORD languageid -  This id identifies what language the message should be in.  This value
//*                      can be created using the MAKELANGID macro and using the defines in "WINRES.H".
//*  Any remaining arguements are going to be used as arguments for the message.
{
    va_list marker;

    va_start( marker, languageid );

//    if(!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL,
    if(!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE, NULL,
            msgid, languageid, (LPTSTR)pszBuffer, maxbuffsize, &marker)) {

#if 0
		LPTSTR lpMsgBuf;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);

		// Display the string.
		//MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );
		OutputDebugString(lpMsgBuf);

		// Free the buffer.
		LocalFree( lpMsgBuf );
#endif
		sprintf(pszBuffer, "");
		return(0);
    }
    va_end( marker );

    return(1);
}


//**************************************************************************************
int TXTWriteStringInBox(char *wstr, GrBuff *SREBuff, GrFontBuff *font, int MinX, int MinZ, int width, int height, int color, int newlinespace, long *xstart, int center, int spcbetweenlines)
{
	long twidth, theight, twidthlast, twidthlastc;
	char *wptr, *placeptr;
	int cnt = 0, cnt2 = 0;
	char tempstr[1024];
	unsigned char ch;
	int fontheight;
	int wordcnt = 0;
	long dwidth;
	int xstarttext = 0;
	int redval, greenval, blueval;

 	fontheight = TXTGetMaxFontHeight(font);

	wptr = placeptr = wstr;
	tempstr[0] = (char)'/n';

	theight = twidth = 0;
	if(xstart != NULL)
	{
		twidth = xstarttext = *xstart;
		wordcnt = 1;
	}
	twidthlast = twidthlastc = twidth;
	while(*wptr != NULL)
	{
		while((*wptr != 32) && (*wptr != NULL) && (*wptr != 10))
		{

			ch = tempstr[cnt2] = *wptr;

			if(twidth)
				twidthlastc = twidth - font->iCharSpacing;

			twidth += font->aucCharLayoutWidth[ch]+font->iCharSpacing;

			if(twidth >= width)
			{
				break;
			}
			wptr++;
			cnt2 ++;
		}
		if((twidth >= width) || ((*wptr == 10) && (!newlinespace)))
		{
			if(*wptr == 10)
			{
				wordcnt ++;
				tempstr[cnt2] = *wptr;
				tempstr[cnt2 + 1] = (char)'\n';

				cnt = cnt2;
				wptr++;
				placeptr = wptr;
				cnt2 ++;
			}
			if(wordcnt)
			{
				tempstr[cnt] = NULL;
				wptr = placeptr;
			}
			else
			{
				tempstr[cnt2] = NULL;
				placeptr = wptr;
			}

			if(xstarttext)
			{
				AICGetRGBColors(color, &redval, &greenval, &blueval);

				GrDrawString(SREBuff, font, MinX + xstarttext, MinZ + theight, tempstr, redval, greenval, blueval, 0);
				xstarttext = 0;
			}
			else if(center)
			{
				if(twidthlast)
				{
					dwidth = (width - twidthlast)>>1;
				}
				else
				{
					dwidth = (width - twidthlastc)>>1;
					twidthlast = twidthlastc;
				}

				if(dwidth < 0)
					dwidth = 0;

				AICGetRGBColors(color, &redval, &greenval, &blueval);

				GrDrawString(SREBuff, font, MinX + dwidth, MinZ + theight, tempstr, redval, greenval, blueval, 0);
			}
			else
			{
				AICGetRGBColors(color, &redval, &greenval, &blueval);

				GrDrawString(SREBuff, font, MinX, MinZ + theight, tempstr, redval, greenval, blueval, 0);
			}
			theight += fontheight + spcbetweenlines;
			cnt = cnt2 = 0;

			if(newlinespace)
			{
				twidth = (font->aucCharLayoutWidth[32]+(font->iCharSpacing<<1));
#ifdef OLDSPACING
				twidth += font->iCharSpacing;
#endif
				tempstr[cnt2] = ' ';
				tempstr[cnt2 + 1] = ' ';
				cnt = cnt2 = 2;
				wordcnt = 0;
			}
			else
			{
				twidth = 0;
				wordcnt = 0;
			}
			twidthlast = twidthlastc = twidth;


			if((theight + fontheight) > height)
			{
				if(xstart != NULL)
				{
					*xstart = 0;
				}
				return(-theight);
			}
		}
		else if(*wptr == 10)
		{
			wordcnt ++;
			tempstr[cnt2] = *wptr;
			tempstr[cnt2 + 1] = (char)'\n';

			cnt = cnt2;
			wptr++;
			placeptr = wptr;
			cnt2 ++;
		}
		else if(*wptr == 32)
		{
			wordcnt ++;
			tempstr[cnt2] = *wptr;
			tempstr[cnt2 + 1] = (char)'\n';
			placeptr = wptr;

//			if(!newlinespace)
				placeptr ++;

			cnt = cnt2;
			wptr++;
			cnt2 ++;
			twidthlast = twidthlastc = twidth - font->iCharSpacing;
			twidth += font->aucCharLayoutWidth[32]+font->iCharSpacing;
#ifdef OLDSPACING
			twidth += font->iCharSpacing;
#endif
//			GrDrawString(SREBuff, font, 10, 10 + theight, tempstr, color);
		}

		if(*wptr == NULL)
		{
			tempstr[cnt2] = NULL;
			twidthlastc = twidth - font->iCharSpacing;
//				twidthlast = twidthlastc;  //  testing
			if(xstarttext)
			{
				AICGetRGBColors(color, &redval, &greenval, &blueval);

				GrDrawString(SREBuff, font, MinX + xstarttext, MinZ + theight, tempstr, redval, greenval, blueval, 0);
				xstarttext = 0;
			}
			else if(center)
			{
#if 0
				if(twidthlast)
				{
					dwidth = (width - twidthlast)>>1;
				}
				else
				{
#endif
					dwidth = (width - twidthlastc)>>1;
					twidthlast = twidthlastc;
#if 0
				}
#endif

				if(dwidth < 0)
					dwidth = 0;

				AICGetRGBColors(color, &redval, &greenval, &blueval);

				GrDrawString(SREBuff, font, MinX + dwidth, MinZ + theight, tempstr, redval, greenval, blueval, 0);
			}
			else
			{
				AICGetRGBColors(color, &redval, &greenval, &blueval);

				GrDrawString(SREBuff, font, MinX, MinZ + theight, tempstr, redval, greenval, blueval, 0);
			}
			twidthlast = twidthlastc = twidth;
			if(xstart == NULL)
				theight += fontheight + spcbetweenlines;
		}
	}
	if(xstart != NULL)
	{
		*xstart = twidthlast;
	}
	return(theight);
}

//**************************************************************************************
int TXTHeightOfStringInBox(char *wstr, GrFontBuff *font, int width, int height, int newlinespace)
{
	long twidth, theight;
	char *wptr, *placeptr;
	int cnt = 0, cnt2 = 0;
	unsigned char ch;
	int fontheight;
	int wordcnt = 0;

 	fontheight = TXTGetMaxFontHeight(font);

	wptr = placeptr = wstr;

	theight = twidth = 0;
	while(*wptr != NULL)
	{
		while((*wptr != 32) && (*wptr != NULL) && (*wptr != 10))
		{

			ch = *wptr;
			twidth += font->aucCharLayoutWidth[ch]+font->iCharSpacing;

			if(twidth >= width)
			{
				break;
			}
			wptr++;
			cnt2 ++;
		}
		if((twidth >= width) || (*wptr == 10))
		{
			if(*wptr == 10)
			{
				wordcnt ++;
				placeptr = wptr;

				if(!newlinespace)
					placeptr ++;

				cnt = cnt2;
				wptr++;
				cnt2 ++;
			}
			if(wordcnt)
			{
				wptr = placeptr;
			}
			else
			{
				placeptr = wptr;
			}
			theight += fontheight;
			cnt = cnt2 = 0;

			if(newlinespace)
			{
				twidth = font->aucCharLayoutWidth[32]+font->iCharSpacing;
#ifdef OLDSPACING
				twidth += font->iCharSpacing;
#endif
				cnt = cnt2 = 1;
			}
			else
			{
				twidth = 0;
				wordcnt = 0;
			}


			if((theight + fontheight) > height)
			{
				return(-theight);
			}
		}
		else if(*wptr == 32)
		{
			wordcnt ++;
			placeptr = wptr;

			if(!newlinespace)
				placeptr ++;

			cnt = cnt2;
			wptr++;
			cnt2 ++;
			twidth += font->aucCharLayoutWidth[32]+font->iCharSpacing;
#ifdef OLDSPACING
			twidth += font->iCharSpacing;
#endif
		}
		else if(*wptr == NULL)
		{
			theight += fontheight;
		}
	}
	return(theight);
}

//**************************************************************************************
int TXTGetMaxFontHeight(GrFontBuff *font)
{
#if 0
	int cnt;
	int tempheight, currheight;

	tempheight = 0;
	for (cnt=0; cnt<MAX_ASCII_CHAR; cnt++)
	{
		currheight = font->aucCharacterHeight[cnt];
		if(currheight > tempheight)
			tempheight = currheight;
	}
	return(tempheight);
#endif
	return(font->iMaxFontHeight);
}

//**************************************************************************************
int TXTGetMaxFontWidth(GrFontBuff *font)
{
#if 0
	int cnt;
	int tempwidth, currwidth;

	tempwidth = 0;
	for (cnt=0; cnt<MAX_ASCII_CHAR; cnt++)
	{
		currwidth = font->aucCharLayoutWidth[cnt];
		if(currwidth > tempwidth)
			tempwidth = currwidth;
	}
	return(tempwidth);
#endif
	return(font->iMaxFontWidth);
}

//**************************************************************************************
int TXTGetMaxFontWidthNum(GrFontBuff *font)
{
	int cnt;
	int tempwidth, currwidth;

	tempwidth = 0;
	for (cnt=48; cnt<58; cnt++)
	{
		currwidth = font->aucCharLayoutWidth[cnt];
		if(currwidth > tempwidth)
			tempwidth = currwidth;
	}
	return(tempwidth);
}

//**************************************************************************************
void TXTDrawBoxFillABS(GrBuff *SREBuff, long MinX, long MinZ, long MaxX, long MaxZ, int bcolor)
{
	int redval, greenval, blueval;
	if((bcolor < 0) && (bcolor != -10) && (bcolor != -20) && (bcolor != -30))
		return;

	AICGetRGBColors(bcolor, &redval, &greenval, &blueval);

	GrFillRectNoClip(SREBuff, MinX, MinZ, (MaxX - MinX) + 1, (MaxZ - MinZ) + 1, redval, greenval, blueval, 0);
}

//**************************************************************************************
void TXTDrawBoxFillRel(GrBuff *SREBuff, long MinX, long MinZ, long bwidth, long bheight, int bcolor)
{
	int redval, greenval, blueval;
	if((bcolor < 0) && (bcolor != -10) && (bcolor != -20) && (bcolor != -30))
		return;

	AICGetRGBColors(bcolor, &redval, &greenval, &blueval);

//	GrFillRectNoClip(SREBuff, MinX, MinZ, MinX + bwidth, MinZ + bheight, redval, greenval, blueval, 0);
	GrFillRectNoClip(SREBuff, MinX, MinZ, bwidth + 1, bheight + 1, redval, greenval, blueval, 0);
}

//**************************************************************************************
long TXTGetStringPixelLength(char *wstr, GrFontBuff *font)
{
	long sze = 0;
	unsigned char ch;

	while((ch = *wstr++)!=NULL)
	{
		sze += font->aucCharLayoutWidth[ch]+font->iCharSpacing;
		if(*(wstr) == 0x32)
		{
			sze+=font->iCharSpacing;
		}
	}
	return(sze);
}

#ifdef _DEBUG
extern int CTAddFuelTest(PlaneParams *planepnt, float fuelamount);
int srefueltest = 0;
#endif
//**************************************************************************************
void AICDoMessagesAndComms()
{
	int cnt;
	PlaneParams *planepnt;

//	_3dxlStopZBuffer();

#ifdef _DEBUG
//	SetWindowPos(hwnd, HWND_BOTTOM, 0,0, 640, 480, 0);
#endif

	if((GrBuffFor3D->MidY == 640 >> 1) && (GrBuffFor3D->ClipBottom == 479) && iAIShowRadarRange)
	{
		AIDrawSimpleRadar();
	}
#if 0  // for dogfight testing
	else if((GrBuffFor3D->MidY == 480 >> 1) && (GrBuffFor3D->ClipBottom == 479))
	{
		char speedstr[1024];
		sprintf(speedstr, "KNOTS : %3.1f         IAS : %3.1f", PlayerPlane->Knots, PlayerPlane->IndicatedAirSpeed);
		TXTWriteStringInBox(speedstr, GrBuffFor3D, MessageFont, 320, 10, 320, 50, HUDColor, 0, NULL, 0, 0);
		if(PlayerPlane->AADesignate)
		{
			sprintf(speedstr, "KNOTS : %3.1f         IAS : %3.1f", PlayerPlane->AADesignate->Knots, PlayerPlane->AADesignate->IndicatedAirSpeed);
			TXTWriteStringInBox(speedstr, GrBuffFor3D, MessageFont, 320, 30, 320, 50, HUDColor, 0, NULL, 0, 0);
		}
	}
#endif

	if(gAICommMenu.AICommMenufunc != NULL)
	{
		gAICommMenu.AICommMenufunc();
		gAICommMenu.lTimer -= (DeltaTicks>>TimeExcel);
		if(gAICommMenu.lTimer < 0)
		{
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			AICRestoreFromCommKeys();
		}
	}
	else
	{
		AICDisplayAIRadioMsgs();
		if(gAICommMenu.lTimer != -9999)
		{
			AICRestoreFromCommKeys();
		}
	}

	for(cnt = 0; cnt < MAXAISOUNDQUEUES; cnt++)
	{
		if(gAISoundQueue[cnt].lTimer >= 0)
		{
			gAISoundQueue[cnt].lTimer -= DeltaTicks;
			if(gAISoundQueue[cnt].lTimer < 0)
			{
				gAISoundQueue[cnt].Soundfunc(gAISoundQueue[cnt].planenum, gAISoundQueue[cnt].targetnum);
			}
		}
	}

	lEventMinuteTimer -= DeltaTicks;
	if(lEventMinuteTimer < 0)
	{
		lEventMinuteTimer += 60000;
		lEventMinutesPassed ++;
		AICheckEvents(EVENT_TIME, lEventMinutesPassed);
	}

	lAreaEventTimer -= DeltaTicks;
	if(lAreaEventTimer < 0)
	{
		lAreaEventTimer += 3000;
		AICheckEvents(EVENT_AREA, 0);
		AICheckEvents(EVENT_MOVING_AREA, 0);
	}


	for(cnt = 0; cnt < g_iNumEvents; cnt ++)
	{
		if(iAIEventDelay[cnt] >= 0)
		{
			iAIEventDelay[cnt] = iAIEventDelay[cnt] - DeltaTicks;
			if(iAIEventDelay[cnt] < 0)
			{
				AIDoDelayedEvents(cnt);
			}
		}
	}

	if(iAIHumanLanding)
	{
		if(lAIHumanLandingTimer >= 0)
		{
			lAIHumanLandingTimer -= DeltaTicks;
			if(lAIHumanLandingTimer < 0)
			{
				if(!AICheckHumanMarshallPattern())
				{
					lAIHumanLandingTimer = 60000;
				}
				else
				{
					iAIHumanLanding = 0;
				}
			}
		}
	}

	if(PlayerPlane->AI.AirThreat)
	{
		if(PlayerPlane->AI.AirThreat->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			PlayerPlane->AI.AirThreat = NULL;
		}
	}

#if 0
	if(((iFACState & 0xFFFF) >= 1) && ((iFACState & 0xFFFF) <= 2) && (lFACTimer < 0))
	{
		AICheckInitialFACCalls(PlayerPlane);
	}
	else
#endif
	if(((iFACState & 0xFFFF) == 3) && (lFACTimer < 0))
	{
		AIReCheckingForGroundEnemies(fpFACPosition, 10, NULL, AI_FRIENDLY, iFACTargetPos);
	}

	if((iFACState & 0xFFFF) && (lFACTimer >= 0))
	{
		lFACTimer -= DeltaTicks;
	}

	if(lWSOSpeakTimer >= 0)
	{
		lWSOSpeakTimer -= DeltaTicks;
	}
	else
	{
		if((lLaserStatusFlags & WSO_LASER_IS_ON) && (!(lBombFlags & WSO_LASER_ON)))
		{
			lBombFlags |= WSO_LASER_ON;
			AIC_WSO_Laser_On(PlayerPlane - Planes);
			lWSOSpeakTimer = 3000;
		}
		else if((!(lLaserStatusFlags & WSO_LASER_IS_ON)) && (lBombFlags & WSO_LASER_ON))
		{
			lBombFlags &= ~WSO_LASER_ON;
			AIC_WSO_Laser_Off(PlayerPlane - Planes);
			lWSOSpeakTimer = 3000;
		}
		else if((lLaserStatusFlags & WSO_LASER_IS_ON) && (lLaserStatusFlags & WSO_LASER_MASK_WARN) && (!(lBombFlags & WSO_MASK_WARNING)))
		{
			lBombFlags |= WSO_MASK_WARNING;
#if F15_SPEECH
			AIC_WSO_Loosing_Lase(PlayerPlane - Planes);
			lWSOSpeakTimer = 3000;
#endif
		}
		else if((lLaserStatusFlags & WSO_LASER_IS_ON) && (!(lLaserStatusFlags & WSO_LASER_MASK_WARN)) && (lBombFlags & WSO_MASK_WARNING))
		{
			lBombFlags &= ~WSO_MASK_WARNING;
		}
		else if((lLaserStatusFlags & WSO_LASER_IS_ON) && (lLaserStatusFlags & WSO_LASER_MASKED) && (!(lBombFlags & WSO_MASKING)))
		{
			lBombFlags |= WSO_MASKING;
#if F15_SPEECH
			AIC_WSO_Pod_Masked(PlayerPlane - Planes);
			lWSOSpeakTimer = 3000;
#endif
		}
		else if((lLaserStatusFlags & WSO_LASER_IS_ON) && (!(lLaserStatusFlags & WSO_LASER_MASKED)) && (lBombFlags & WSO_MASKING))
		{
			lBombFlags &= ~WSO_MASKING;
		}
		else if((PlayerPlane->AI.iAIFlags1 & AIJAMMINGON) && (!(lBombFlags & WSO_MUSIC_ON)))
		{
			lBombFlags |= WSO_MUSIC_ON;
			AIC_WSO_Music_On(PlayerPlane - Planes);
			lWSOSpeakTimer = 3000;
		}
		else if((!(PlayerPlane->AI.iAIFlags1 & AIJAMMINGON)) && (lBombFlags & WSO_MUSIC_ON))
		{
			lBombFlags &= ~WSO_MUSIC_ON;
			AIC_WSO_Music_Off(PlayerPlane - Planes);
			lWSOSpeakTimer = 3000;
		}

		if((!(lLaserStatusFlags & WSO_LASER_MASK_WARN)) && (lBombFlags & WSO_MASK_WARNING))
		{
			lBombFlags &= ~WSO_MASK_WARNING;
		}
		else if((!(lLaserStatusFlags & WSO_LASER_MASKED)) && (lBombFlags & WSO_MASKING))
		{
			lBombFlags &= ~WSO_MASKING;
		}
	}

	if(PlayerPlane->AI.lCMTimer >= 0)
	{
		PlayerPlane->AI.lCMTimer = PlayerPlane->AI.lCMTimer - DeltaTicks;
	}

	if(PlayerPlane->AI.lSpeechOverloadTimer >= 0)
	{
		PlayerPlane->AI.lSpeechOverloadTimer -= DeltaTicks;
	}

	if(lAWACSPictTimer >= 0)
	{
		lAWACSPictTimer -= DeltaTicks;
	}

	if(lPlayerSARTimer >= 0)
	{
		lPlayerSARTimer -= DeltaTicks;
	}

	if(lPlayerDownedTimer >= 0)
	{
		lPlayerDownedTimer	-= DeltaTicks;
	}

	if(lDogSpeechTimer >= 0)
	{
		lDogSpeechTimer	-= DeltaTicks;
	}

	if((lDefensiveTimer <= -9999) && (!PlayerPlane->AI.AirThreat))
	{
		lDefensiveTimer = 0;
	}

	if(lVisualOverloadTimer >= 0)
	{
		lVisualOverloadTimer -= DeltaTicks;
	}


	if(lNetSpeechTimer >= 0)
	{
		lNetSpeechTimer -= DeltaTicks;
	}

	if(lPadLockTimer >= 0)
	{
		lPadLockTimer  -= DeltaTicks;
		if(lPadLockTimer < 0)
		{
			pPadLockTarget = NULL;
			fpdPadLockLocation.SetValues(-1.0f, -1.0f, -1.0f);
			iPadLockType = 0;
		}
	}

	if(lCarrierBirdsAfirmTimer >= 0)
	{
		lCarrierBirdsAfirmTimer -= DeltaTicks;
	}

	if(lCarrierHitMsgTimer >= 0)
	{
		lCarrierHitMsgTimer -= DeltaTicks;
	}

	lFenceCheckTimer -= DeltaTicks;
	if(lFenceCheckTimer < 0)
	{
		int tempfence = 0;

		tempfence = AICCheckAllFences((PlayerPlane->WorldPosition.X * WUTOFT), (PlayerPlane->WorldPosition.Z * WUTOFT));

		if(tempfence != iFenceIn)
		{
			iFenceIn = tempfence;
#ifndef __DEMO__
			AICFenceCall(iFenceIn);
#endif
			LogMissionStatistic(LOG_FLIGHT_FENCE_IN,1,1,2);
			lFenceCheckTimer = 300000;
			if(tempfence)
			{
				AIChangeEventFlag(MISSION_FLAG_START+10);  // Set Fence In Flag
				LogMissionStatistic(LOG_FLIGHT_FENCE_IN,1,1,0);
				LogMissionStatistic(LOG_FLIGHT_FENCE_IN_TIME,1,GameLoop,0);
			} else {
				LogMissionStatistic(LOG_FLIGHT_FENCE_OUT_TIME,1,GameLoop,0);
			}

		}
		else
		{
			lFenceCheckTimer = 60000;
		}
	}


#if F15_SPEECH
	if(lBombFlags & WSO_BOMB_IMPACT)
	{
		AIC_WSO_Bomb_Speech(PlayerPlane, WSO_BOMB_IMPACT, lBombTimer);
	}
	else if(UFC.MasterMode == NAV_MODE)
#else
	if(UFC.MasterMode == NAV_MODE)
#endif
	{
		AIC_WSO_Nav_Speech(PlayerPlane);
	}
	else
	{
		lBombFlags &= ~WSO_NAV_MSGS;
	}

	if(lNoticeTimer >= 0)
	{
		lNoticeTimer -= DeltaTicks;
	}

	if(lNoBulletHitMsgs >= 0)
	{
		lNoBulletHitMsgs -= DeltaTicks;
	}
	if(lNoOtherBulletHitMsgs >= 0)
	{
		lNoOtherBulletHitMsgs -= DeltaTicks;
	}
	if(lNoCheckPlanelMsgs >= 0)
	{
		lNoCheckPlanelMsgs -= DeltaTicks;
	}
	if(0 <= PlayerPlane->AI.lInterceptTimer)
	{
		PlayerPlane->AI.lInterceptTimer -= DeltaTicks;
	}

	if(0 <= lViewTimer)
	{
		lViewTimer -= DeltaTicks;
		if(0 > lViewTimer)
		{
			AIRestoreViewActionCamera();
		}
	}

	if(iEasyBombVar)
	{
		if(lEasyBomberTimer >= 0)
		{
			lEasyBomberTimer -= DeltaTicks;
			if(lEasyBomberTimer < 0)
			{
				iEasyBombVar ++;
			}
		}
	}
	if(iEasyAAVar)
	{
		if(lEasyAATimer >= 0)
		{
			lEasyAATimer -= DeltaTicks;
			if(lEasyAATimer < 0)
			{
				iEasyAAVar ++;
			}
		}
	}


	if(cAAAFiring)
	{
		lPlayerGroupTimer -= DeltaTicks;
		if(lPlayerGroupTimer < 0)
		{
			cAAAFiring = 0;
			planepnt = PlayerPlane + cCurrentLook;
			if(planepnt > LastPlane)
			{
				planepnt = PlayerPlane;
				cCurrentLook = 0;
			}
 			if((!(((planepnt == PlayerPlane) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP)) && (planepnt->Status & PL_ACTIVE))) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
			{
				planepnt = PlayerPlane;
				cCurrentLook = 0;
			}
 			if((planepnt->Status & PL_ACTIVE) && (!(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
			{
				GDAAASeen(planepnt);
			}
			cCurrentLook ++;
		}
	}

	GDCheckDefenses();

	CheckDelayedDestruction();

	if((lFireFlags) && (!(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
	{
		ADQuickFireSim();
	}

	if(g_Settings.gp.dwFlight & GP_FLIGHT_GFORCE_BREATH)
		DoGHeartBeat(PlayerPlane);
//		AICDoWSOgStuff();

	if(PlayerPlane->FlightStatus & PL_OUT_OF_CONTROL)
	{
		PlayerPlane->AI.Behaviorfunc = AIDeathSpiral;
		PlayerPlane->Status &= ~AL_DEVICE_DRIVEN;
		PlayerPlane->Status |= AL_AI_DRIVEN;
		PlayerPlane->DoControlSim = CalcF18ControlSurfacesSimple;
		PlayerPlane->DoCASSim = CalcF18CASUpdatesSimple;
		PlayerPlane->DoForces = CalcAeroForcesSimple;
		PlayerPlane->DoPhysics = CalcAeroDynamicsSimple;
		PlayerPlane->dt = 0;
		PlayerPlane->UpdateRate = HIGH_AERO;
	}
	if(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
	{
		AICAfterEjectionSoundCancel();
	}

	if(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)
	{
		AICCheckPlayerCarrierLanding();
	}
	else if(lAdvisorFlags)
	{
		lAdvisorTimer -= DeltaTicks;
		if(lAdvisorTimer < 0)
		{
			AICCheckAdvisor();
		}
	}
	iFirstTimeInFrame = 1;

	if(MultiPlayer)
	{
		NetCheckDatabase();

		if(NetBuddyLasingData[MySlot].lasertimer >= 0)
		{
			NetBuddyLasingData[MySlot].lasertimer -= DeltaTicks;
		}

		if(((lLaserStatusFlags & WSO_LASER_MASKED) || (!(lLaserStatusFlags & WSO_LASER_IS_ON))) && (NetBuddyLasingData[MySlot].laserloc.X >= 0))
		{
			NetBuddyLasingData[MySlot].laserloc.X = -1.0f;
			NetBuddyLasingData[MySlot].laserloc.Y = -1.0f;
			NetBuddyLasingData[MySlot].laserloc.Z = -1.0f;
			NetPutGenericMessage2FPoint(NULL, GM2FP_BUDDY_LASER, NetBuddyLasingData[MySlot].laserloc, MySlot);
		}
		else if((!((lLaserStatusFlags & WSO_LASER_MASKED) || (!(lLaserStatusFlags & WSO_LASER_IS_ON)))) && ((NetBuddyLasingData[MySlot].laserloc.X < 0) || (NetBuddyLasingData[MySlot].lasertimer < 0)))
		{
			if(NetBuddyLasingData[MySlot].laserloc.X < 0)
			{
				NetBuddyLasingData[MySlot].laserloc = LaserLoc;
				NetPutGenericMessage2FPoint(NULL, GM2FP_BUDDY_LASER, NetBuddyLasingData[MySlot].laserloc, MySlot);
			}
			else
			{
				if(((NetBuddyLasingData[MySlot].laserloc - LaserLoc) * WUTOFT) > 10.0f)
				{
					NetBuddyLasingData[MySlot].laserloc = LaserLoc;
					NetPutGenericMessage2FPoint(NULL, GM2FP_BUDDY_LASER, NetBuddyLasingData[MySlot].laserloc, MySlot);
				}
			}
			NetBuddyLasingData[MySlot].lasertimer = 3000;
		}
	}

	if( GetCurrentAARadarMode() == AA_STT_MODE )
	{
		AICheckForGateStealerP(PlayerPlane, PlayerPlane->AADesignate);
	}
	else
	{
		PlayerPlane->AI.lGateTimer = -99999;
	}

	if(dwSpeedBrakeSnd)
	{
		if(!(SndIsSoundPlaying(dwSpeedBrakeSnd)))
		{
			dwSpeedBrakeSnd = NULL;
		}
	}

	if((!( (AA_weapID == Aim9X_Id) || (AA_weapID == Aim9M_Id) || (AA_weapID == Aim9L_Id) )) || (UFC.MasterMode != AA_MODE) || (Camera1.CameraMode != CAMERA_COCKPIT) || (PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED) || (WeapStores.GunsOn) || (PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
	{
		KillSound( &g_dwAIMSndHandle );
		KillSound( &g_dwAIMLockSndHandle );
	}
	else
	{
		if(g_dwAIMSndHandle)
		{
			if(!SndIsSoundPlaying(g_dwAIMSndHandle))
			{
				KillSound( &g_dwAIMSndHandle );
			}
		}

		if(g_dwAIMLockSndHandle)
		{
			if(!SndIsSoundPlaying(g_dwAIMLockSndHandle))
			{
				KillSound( &g_dwAIMLockSndHandle );
			}
		}
	}

	if(MultiPlayer)
	{
		if(PlayerPlane->Knots > 50)
		{
			lBombFlags |= WSO_MP_RELOAD_ALLOWED;
		}
		else if((PlayerPlane->OnGround == 1) && (PlayerPlane->Knots < 1.0f) && (lBombFlags & WSO_MP_RELOAD_ALLOWED) && (g_Settings.gp.dwCheats & GP_CHEATS_MULTI_REARM))
		{
			lBombFlags &= ~(WSO_MP_RELOAD_ALLOWED);
			F18LoadPlayerWeaponInfo();
			NetPutGenericMessage1(PlayerPlane, GM_RELOAD_PLANE);
		}
	}

#ifdef _DEBUG
	if(srefueltest)
	{
		float fuelgiven = 4000.0f;

		fuelgiven /= 60.0f;  //  lbs / min to lbs / sec;

		fuelgiven *= ((float)DeltaTicks / 1000.0f);

	//	if(planepnt->AI.lTimer2 < 0)
		if(CTAddFuelTest(PlayerPlane, fuelgiven))
		{
			srefueltest = 0;
		}
	}

//	char text[256];
//	sprintf( text, "JoyY %ld, StkY %02.02f, RG %02.02f ", GetJoyPosY(), PlayerPlane->StickY, PlayerPlane->RequestedG);

//	GrDrawString( GrBuffFor3D, LgHUDFont, 400,  20, text, HUDColor );

#endif

//	_3dxlStartZBuffer();
}

//**************************************************************************************
void AICShowGroupCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int firstvalid, lastvalid;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_WINGMAN_MENU, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ELEMENT_MENU, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_DIVISION_MENU, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FLIGHT_MENU, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(MultiPlayer)
	{
		usecolor = textcolor;
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FLIGHT_REPORTS, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
		usecolor = textcolor;
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ROGER_MSG, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
		usecolor = textcolor;
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_NEGATIVE_MSG, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
}

//**************************************************************************************
int AICheckPartsOfGroup(PlaneParams *planepnt, int firstvalid, int lastvalid)
{
	int placeingroup;
	int islead = (MultiPlayer) ? 0 : 1;  //  See if person sending message is leader

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return(0);
	}

	if(!(AIInPlayerGroup(planepnt)))
	{
		return(0);
	}

	if((PlayerPlane->AI.winglead < 0) && (PlayerPlane->AI.prevpair < 0))
	{
		islead = 1;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid) && ((!(planepnt->AI.iAIFlags2 & AILANDING)) || (planepnt->AI.lTimer2 > 0)))
	{
		return(1);
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(AICheckPartsOfGroup(&Planes[planepnt->AI.wingman], firstvalid, lastvalid))
		{
			return(1);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AICheckPartsOfGroup(&Planes[planepnt->AI.nextpair], firstvalid, lastvalid));
	}

	return(0);
}

//**************************************************************************************
void AICShowFlightCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ENGAGE_BANDITS, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);

//	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RADAR_ON_OFF, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	if(lBombFlags & WSO_RADAR_OFF)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RADAR_ON, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RADAR_OFF, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}

//	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MUSIC_ON_OFF, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	if(lBombFlags & WSO_JAMMER_ON)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MUSIC_OFF, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MUSIC_ON, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REPORT_CONTACTS, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SORT_BANDITS, 5, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_GROUND_MENU, 6, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_STATUS_MENU, 7, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FORMATION_MENU, 8, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REJOIN_FLIGHT, 9, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RETURN_TO_BASE, 0, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowDivisionElementCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int covertype = -1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ENGAGE_BANDITS, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SANITIZE_RIGHT, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SANITIZE_LEFT, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);

	covertype = AIGetPlayerCoverType();
	if(covertype == 2)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_CONTINUE_ESCORT, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	else if(covertype == 3)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BEGIN_CAP, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_COVER_ME, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ORBIT_REJOIN, 5, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_INTERCEPT_MENU, 6, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_GROUND_MENU, 7, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REJOIN_FLIGHT, 8, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RETURN_TO_BASE, 9, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowWingmanCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int covertype = -1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ENGAGE_BANDITS, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SANITIZE_RIGHT, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SANITIZE_LEFT, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);

	covertype = AIGetPlayerCoverType();
	if(covertype == 2)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_CONTINUE_ESCORT, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	else if(covertype == 3)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BEGIN_CAP, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_COVER_ME, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ORBIT_REJOIN, 5, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TACTICAL_MENU, 6, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_INTERCEPT_MENU, 7, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_GROUND_MENU, 8, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REJOIN_FLIGHT, 9, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_RETURN_TO_BASE, 0, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowGroundAttackCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	MBWayPoints	*bombway;	//  This is a pointer to the bomb waypoint.
	int planenum = PlayerPlane - Planes;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);


	bombway = AICGetBombWaypoint(planenum, 1);
	if(bombway != NULL)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AG_PRIMARY, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	bombway = AICGetBombWaypoint(planenum, 2);
	if(bombway != NULL)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AG_SECONDARY, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

#if 0 //  Taken out for patch so that CAS can be done at any time.
	bombway = AICGetBombWaypoint(planenum, 3);
	if((bombway != NULL) || (iFACState))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
#endif

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AG_OPPORTUNITY, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);


	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ATTACK_AIR_DEF, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ATTACK_WITH, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

//	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AG_ABORT, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowWeaponOptionCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int planenum = PlayerPlane - Planes;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);


	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_UNGUIDED_ALL, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_UNGUIDED_HALF, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_UNGUIDED_SINGLE, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_GUIDED_ALL, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_GUIDED_HALF, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_GUIDED_SINGLE, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MISSILE_ALL, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MISSILE_HALF, 8, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_MISSILE_SINGLE, 9, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_DEFAULT, 0, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowInterceptCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BRACKET_RIGHT, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BRACKET_LEFT, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SPLIT_HIGH, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SPLIT_LOW, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_DRAG_RIGHT, 5, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_DRAG_LEFT, 6, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowTacticalCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BREAK_RIGHT, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BREAK_LEFT, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BREAK_HIGH, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_BREAK_LOW, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ATTACK_MY_TARGET, 5, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_HELP_ME, 6, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowStatusCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FLIGHT_CHECK, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_WEAPONS_CHECK, 2, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SAY_POSITION, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_SAY_FUEL, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AICShowFormationCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int usecolor;
	int invalidcolor = iNoSelectTextColor;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_LOOSEN_FORM, 1, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);

	if(AITightenSpacingOK(PlayerPlane - Planes, 0.5))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TIGHTEN_FORM, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_V_FORM, 3, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_WEDGE_FORM, 4, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_LINE_ABREAST_FORM, 5, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ECHELON_FORM, 6, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TRAIL_FORM, 7, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_WALL_FORM, 8, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
int TXTGetFrontOfOptions(char *tempstr, int maxbuffersize, int optionnum)
{
	if(!LANGGetTransMessage(tempstr, maxbuffersize, AIC_FRONT_OF_OPTIONS, g_iLanguageId, optionnum))
	{
		return(-1);
	}
	return(strlen(tempstr));
}

//**************************************************************************************
void AICSetUpForCommKeys()
{
	KeyEnableKey("KEY_0", 0);
	KeyEnableKey("KEY_1", 0);
	KeyEnableKey("KEY_2", 0);
	KeyEnableKey("KEY_3", 0);
	KeyEnableKey("KEY_4", 0);
	KeyEnableKey("KEY_5", 0);
	KeyEnableKey("KEY_6", 0);
	KeyEnableKey("KEY_7", 0);
	KeyEnableKey("KEY_8", 0);
	KeyEnableKey("KEY_9", 0);
}

//**************************************************************************************
void AICRestoreFromCommKeys()
{
	KeyEnableKey("KEY_0", 1);
	KeyEnableKey("KEY_1", 1);
	KeyEnableKey("KEY_2", 1);
	KeyEnableKey("KEY_3", 1);
	KeyEnableKey("KEY_4", 1);
	KeyEnableKey("KEY_5", 1);
	KeyEnableKey("KEY_6", 1);
	KeyEnableKey("KEY_7", 1);
	KeyEnableKey("KEY_8", 1);
	KeyEnableKey("KEY_9", 1);
	gAICommMenu.lTimer = -9999;
	ViewChangeCountDown = 2;  // for the next two frames allow static stuff to be drawn
}

//**************************************************************************************
void AICCheckAIComm(int keyflag, WPARAM wParam)
{
	if((keyflag == 0) && (wParam == VK_SHIFT))
	{
		iAIShiftDown = 1;
	}
#if 0
	else if((keyflag == 0) && (wParam == VK_TAB))
	{
		if(iAIShiftDown)
		{
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowOtherCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenOtherCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICOtherCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			AICSetUpForCommKeys();
		}
		else
		{
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowGroupCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenGroupCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICGroupCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			AICSetUpForCommKeys();
		}
	}
#endif
	else
	{
		if(gAICommMenu.AICommKeyfunc != NULL)
		{
			if(MultiPlayer)
			{
				NetPutAIComm(keyflag, wParam);
			}
			gAICommMenu.AICommKeyfunc(keyflag, wParam);
		}
	}
}

//**************************************************************************************
void debug_do_next_plane()
{
	PlaneParams *P;
	PlaneParams *orgP;

	orgP = P = (PlaneParams *)Camera1.AttachedObject;
	P++;
	if (P > LastPlane)
		P = &Planes[0];
	while((P != orgP) && (!(P->Status & PL_ACTIVE)))
	{
		P++;
		if (P > LastPlane)
			P = &Planes[0];
	}

	Camera1.AttachedObject = (int *)P;
}


//**************************************************************************************
void debug_do_closest_plane()
{
	PlaneParams *P;

	P = (PlaneParams *)Camera1.AttachedObject;
	PlaneParams *ClosestPlane,*PWalk;
	double dist,closest_dist;

	if (P != PlayerPlane)
		Camera1.AttachedObject = (int *)PlayerPlane;
	else
	{
		PWalk = &Planes[0];
		ClosestPlane= NULL;
		while(PWalk <= LastPlane)
		{
			if ((PWalk != P) && (PWalk->Type != P->Type))
			{
				dist = PWalk->WorldPosition - P->WorldPosition;
				if ((!ClosestPlane) || (dist < closest_dist))
				{
					ClosestPlane = PWalk;
					closest_dist = dist;
				}
			}
			PWalk++;
		}
		if (ClosestPlane)
			Camera1.AttachedObject = (int *)ClosestPlane;
	}
}

long lDamageID = 0;
//**************************************************************************************
void AICGroupCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;
	VKCODE hack;
	hack.vkCode = 0;									// virtual key code (may be joy button)
	hack.wFlags = 0;									// vkCode Type
	hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
	int sndids[30];
	int cnt;
	int firstvalid, lastvalid;


	switch(wParam)
	{
		case '0':
#ifdef _DEBUG
//			EVNextEyePoint();
			debug_do_next_plane();
//			AICheckGrndRadar();
#endif
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
		case '9':
#ifdef _DEBUG
//			debug_do_closest_plane();
//			lMaxBytes = 0;
		DelayDestroyPlaneItem(PlayerPlane, lDamageID, .9f);
#endif
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
		case '8':
#ifdef _DEBUG
			if(MultiPlayer)
			{
				if(iWatchMAISends)
				{
					iWatchMAISends = 0;
				}
				else
				{
					iWatchMAISends = 1;
					OutputDebugString("RESTART WATCH ****************************");
				}
			}
			else
			{
				for(cnt = 0; cnt < 10; cnt ++)
				{
					sndids[cnt] = 35000 + cnt;
				}
				AIRSendSentence(PlayerPlane - Planes, 10, sndids);
			}

//			EVSetNewEyePoint();
//			PLGetNextClosestFriendly();
#endif
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
		case '7':
#ifdef _DEBUG
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 29);
			}
			else
			{
				AIC_ACLS_Switch(PlayerPlane, 1);
			}
#endif

			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
		case '6':
#ifdef _DEBUG
			if(iAICommFrom >= 0)
			{
				AIC_GenericMsgPlane(iAICommFrom, 28);
			}
#endif
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
		case '5':
			if(MultiPlayer)
			{
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowFlightReportCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenFlightReportCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICFlightReportCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			else
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
			}
			break;
		case '4':
			NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
//			if(AICheckPartsOfGroup(PlayerPlane, 1, 7))
			if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
			{
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowFlightCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenFlightCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICFlightCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			break;
		case '3':
			NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
//			if(AICheckPartsOfGroup(PlayerPlane, 4, 7))
			if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
			{
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowDivisionElementCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenDivisionElementCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICDivisionCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			break;
		case '2':
			NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
//			if(AICheckPartsOfGroup(PlayerPlane, 2, 3))
			if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
			{
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowDivisionElementCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenDivisionElementCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICElementCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			break;
		case '1':
			NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
//			if(AICheckPartsOfGroup(PlayerPlane, 1, 1))
			if(AICheckPartsOfGroup(PlayerPlane, firstvalid, lastvalid))
			{
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowWingmanCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenWingmanCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICWingmanCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			break;
	}
}

//**************************************************************************************
void AICFlightCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFEngageBandits();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICRadarToggle();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICJammingToggle();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICAllReportContacts();
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFSortBandits();
			break;
		case '6':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowGroundAttackCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenGroundAttackCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICFGroundAttackCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '7':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowStatusCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenStatusCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICStatusCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '8':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowFormationCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenFormationCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICFormationCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '9':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFRejoinFlight();
			break;
		case '0':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICF_RTB();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICDivisionCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICDEngageBandits();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIDSanitizeRight();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIDSanitizeLeft();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIDCover();
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIDOrbit();
			break;
		case '6':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowInterceptCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenInterceptCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICDInterceptCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '7':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowGroundAttackCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenGroundAttackCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICDGroundAttackCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '8':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICDRejoinFlight();
			break;
		case '9':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICD_RTB();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICElementCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICEEngageBandits();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIESanitizeRight();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIESanitizeLeft();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIECover();
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIEOrbit();
			break;
		case '6':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowInterceptCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenInterceptCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEInterceptCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '7':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowGroundAttackCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenGroundAttackCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICEGroundAttackCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '8':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICERejoinFlight();
			break;
		case '9':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICE_RTB();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICWingmanCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICWEngageBandits();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIWSanitizeRight();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIWSanitizeLeft();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIWCover();
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIWOrbit();
			break;
		case '6':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowTacticalCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenTacticalCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICTacticalCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '7':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowInterceptCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenInterceptCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICWInterceptCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '8':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowGroundAttackCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenGroundAttackCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICWGroundAttackCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		case '9':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICWRejoinFlight();
			break;
		case '0':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICW_RTB();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICFGroundAttackCommKeyOps(int keyflag, WPARAM wParam)
{
	AICGroundAttackCommKeyOps(keyflag, wParam, 4);
}

//**************************************************************************************
void AICDGroundAttackCommKeyOps(int keyflag, WPARAM wParam)
{
	AICGroundAttackCommKeyOps(keyflag, wParam, 3);
}

//**************************************************************************************
void AICEGroundAttackCommKeyOps(int keyflag, WPARAM wParam)
{
	AICGroundAttackCommKeyOps(keyflag, wParam, 2);
}

//**************************************************************************************
void AICWGroundAttackCommKeyOps(int keyflag, WPARAM wParam)
{
	AICGroundAttackCommKeyOps(keyflag, wParam, 1);
}

//BOOL InitSound (void);
//void ShutdownSound (void);
//extern DWORD	g_dwEng2Snd;
//**************************************************************************************
void AICGroundAttackCommKeyOps(int keyflag, WPARAM wParam, int sentto)
{
	int ispaused = 0;
	MBWayPoints	*bombway;	//  This is a pointer to the bomb waypoint.
	int planenum = PlayerPlane - Planes;
	int menuedge = 0;

	switch(wParam)
	{
		case '1':
			bombway = AICGetBombWaypoint(planenum, 1);
			if(bombway != NULL)
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
				switch(sentto)
				{
					case 1:
						AICWGrndAttackPrime();
						break;
					case 2:
						AICEGrndAttackPrime();
						break;
					case 3:
						AICDGrndAttackPrime();
						break;
					case 4:
						AICFGrndAttackPrime();
						break;
				}
			}
			break;
		case '2':
			bombway = AICGetBombWaypoint(planenum, 2);
			if(bombway != NULL)
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
				switch(sentto)
				{
					case 1:
						AICWGrndAttackSecond();
						break;
					case 2:
						AICEGrndAttackSecond();
						break;
					case 3:
						AICDGrndAttackSecond();
						break;
					case 4:
						AICFGrndAttackSecond();
						break;
				}
			}
			break;
		case '3':
//			bombway = AICGetBombWaypoint(planenum, 3);
//			if((bombway != NULL) || (iFACState))
//			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
				switch(sentto)
				{
					case 1:
						AICWGrndAttackOpportunity();
						break;
					case 2:
						AICEGrndAttackOpportunity();
						break;
					case 3:
						AICDGrndAttackOpportunity();
						break;
					case 4:
						AICFGrndAttackOpportunity();
						break;
				}
//			}
			break;
		case '4':
//			bombway = AICGetBombWaypoint(planenum, 3);
//			if((bombway != NULL) || (iFACState))
//			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
				switch(sentto)
				{
					case 1:
						AICWGrndAttackSEAD();
						break;
					case 2:
						AICEGrndAttackSEAD();
						break;
					case 3:
						AICDGrndAttackSEAD();
						break;
					case 4:
						AICFGrndAttackSEAD();
						break;
				}
//			}
			break;
		case '5':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowWeaponOptionCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenWeaponOptionCommOps() + menuedge;
				switch(sentto)
				{
					case 1:
						gAICommMenu.AICommKeyfunc = AICWWeaponOptionCommKeyOps;
						break;
					case 2:
						gAICommMenu.AICommKeyfunc = AICEWeaponOptionCommKeyOps;
						break;
					case 3:
						gAICommMenu.AICommKeyfunc = AICDWeaponOptionCommKeyOps;
						break;
					case 4:
						gAICommMenu.AICommKeyfunc = AICFWeaponOptionCommKeyOps;
						break;
				}
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}


//**************************************************************************************
void AICFWeaponOptionCommKeyOps(int keyflag, WPARAM wParam)
{
	AICWeaponOptionCommKeyOps(keyflag, wParam, 4);
}

//**************************************************************************************
void AICDWeaponOptionCommKeyOps(int keyflag, WPARAM wParam)
{
	AICWeaponOptionCommKeyOps(keyflag, wParam, 3);
}

//**************************************************************************************
void AICEWeaponOptionCommKeyOps(int keyflag, WPARAM wParam)
{
	AICWeaponOptionCommKeyOps(keyflag, wParam, 2);
}

//**************************************************************************************
void AICWWeaponOptionCommKeyOps(int keyflag, WPARAM wParam)
{
	AICWeaponOptionCommKeyOps(keyflag, wParam, 1);
}

//**************************************************************************************
void AICWeaponOptionCommKeyOps(int keyflag, WPARAM wParam, int sentto)
{
	int ispaused = 0;
	int planenum = PlayerPlane - Planes;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionUnguidedAll();
					break;
				case 2:
					AICEWeaponOptionUnguidedAll();
					break;
				case 3:
					AICDWeaponOptionUnguidedAll();
					break;
				case 4:
					AICFWeaponOptionUnguidedAll();
					break;
			}
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionUnguidedHalf();
					break;
				case 2:
					AICEWeaponOptionUnguidedHalf();
					break;
				case 3:
					AICDWeaponOptionUnguidedHalf();
					break;
				case 4:
					AICFWeaponOptionUnguidedHalf();
					break;
			}
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionUnguidedSingle();
					break;
				case 2:
					AICEWeaponOptionUnguidedSingle();
					break;
				case 3:
					AICDWeaponOptionUnguidedSingle();
					break;
				case 4:
					AICFWeaponOptionUnguidedSingle();
					break;
			}
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionGuidedAll();
					break;
				case 2:
					AICEWeaponOptionGuidedAll();
					break;
				case 3:
					AICDWeaponOptionGuidedAll();
					break;
				case 4:
					AICFWeaponOptionGuidedAll();
					break;
			}
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionGuidedHalf();
					break;
				case 2:
					AICEWeaponOptionGuidedHalf();
					break;
				case 3:
					AICDWeaponOptionGuidedHalf();
					break;
				case 4:
					AICFWeaponOptionGuidedHalf();
					break;
			}
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionGuidedSingle();
					break;
				case 2:
					AICEWeaponOptionGuidedSingle();
					break;
				case 3:
					AICDWeaponOptionGuidedSingle();
					break;
				case 4:
					AICFWeaponOptionGuidedSingle();
					break;
			}
			break;
		case '7':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionMissileAll();
					break;
				case 2:
					AICEWeaponOptionMissileAll();
					break;
				case 3:
					AICDWeaponOptionMissileAll();
					break;
				case 4:
					AICFWeaponOptionMissileAll();
					break;
			}
			break;
		case '8':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionMissileHalf();
					break;
				case 2:
					AICEWeaponOptionMissileHalf();
					break;
				case 3:
					AICDWeaponOptionMissileHalf();
					break;
				case 4:
					AICFWeaponOptionMissileHalf();
					break;
			}
			break;
		case '9':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionMissileSingle();
					break;
				case 2:
					AICEWeaponOptionMissileSingle();
					break;
				case 3:
					AICDWeaponOptionMissileSingle();
					break;
				case 4:
					AICFWeaponOptionMissileSingle();
					break;
			}
			break;
		case '0':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWWeaponOptionDefault();
					break;
				case 2:
					AICEWeaponOptionDefault();
					break;
				case 3:
					AICDWeaponOptionDefault();
					break;
				case 4:
					AICFWeaponOptionDefault();
					break;
			}
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICStatusCommKeyOps(int keyflag, WPARAM wParam)
{
	FPoint fptemp;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFlightStatus();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFlightWeaponCheck();
			break;
		case '3':  //  position
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFlightPositionCheck();
			break;
		case '4':  //  fuel
			if(MultiPlayer)
			{
				fptemp.X = PlayerPlane->AI.fStoresWeight;
				fptemp.Y = PlayerPlane->TotalFuel;
				fptemp.Z = PlayerPlane->AircraftDryWeight;
				NetPutGenericMessage2FPoint(PlayerPlane, GM2FP_STORES, fptemp);
			}

			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFlightFuelCheck();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICFormationCommKeyOps(int keyflag, WPARAM wParam)
{
	int planenum;
	int placeingroup;
	int islead = 1;

	if(MultiPlayer)
	{
		if(iAICommFrom >= 0)
		{
			placeingroup = (Planes[iAICommFrom].AI.iAIFlags1 & AIFLIGHTNUMS);
			iLeadPlace = placeingroup<<28;
		}
		else
		{
			placeingroup = (PlayerPlane->AI.iAIFlags1 & AIFLIGHTNUMS);
			iLeadPlace = placeingroup<<28;
		}

		NetConvertFirstAndLastValid(0, 7, &iFirstPlace, &iLastPlace);
	}

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFormationLoosen();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if(iAICommFrom == -1)
			{
				planenum = PlayerPlane - Planes;
			}
			else
			{
				planenum = iAICommFrom;
			}
			if(AITightenSpacingOK(planenum, 0.5))
			{
				AICFormationTighten();
			}
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFormationChange(2);
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICWedgeFormationChange();
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFormationChange(9);
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFormationChange(10);
			break;
		case '7':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFormationChange(7);
			break;
		case '8':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICFormationChange(4);
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}

	if(MultiPlayer)
	{
		iLeadPlace = 0;
		iFirstPlace = 0;
		iLastPlace = 7;
	}
}

//**************************************************************************************
void AICDInterceptCommKeyOps(int keyflag, WPARAM wParam)
{
	AICInterceptCommKeyOps(keyflag, wParam, 3);
}

//**************************************************************************************
void AICEInterceptCommKeyOps(int keyflag, WPARAM wParam)
{
	AICInterceptCommKeyOps(keyflag, wParam, 2);
}

//**************************************************************************************
void AICWInterceptCommKeyOps(int keyflag, WPARAM wParam)
{
	AICInterceptCommKeyOps(keyflag, wParam, 1);
}

//**************************************************************************************
void AICInterceptCommKeyOps(int keyflag, WPARAM wParam, int sentto)
{
	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWBracketRight();
					break;
				case 2:
					AICEBracketRight();
					break;
				case 3:
					AICDBracketRight();
					break;
			}
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWBracketLeft();
					break;
				case 2:
					AICEBracketLeft();
					break;
				case 3:
					AICDBracketLeft();
					break;
			}
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWSplitHigh();
					break;
				case 2:
					AICESplitHigh();
					break;
				case 3:
					AICDSplitHigh();
					break;
			}
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWSplitLow();
					break;
				case 2:
					AICESplitLow();
					break;
				case 3:
					AICDSplitLow();
					break;
			}
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWDragRight();
					break;
				case 2:
					AICEDragRight();
					break;
				case 3:
					AICDDragRight();
					break;
			}
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			switch(sentto)
			{
				case 1:
					AICWDragLeft();
					break;
				case 2:
					AICEDragLeft();
					break;
				case 3:
					AICDDragLeft();
					break;
			}
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICTacticalCommKeyOps(int keyflag, WPARAM wParam)
{
	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIBreakRight();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIBreakLeft();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIBreakHigh();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AIBreakLow();
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICWAttackMyTarget();
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICWHelpMe();
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
int AICDisplayAnAICommMenuOption(GrBuff *usebuff, int messageid, int optionnum, int orgboxheight, int boxcolor, int textcolor, GrFontBuff *usefont, int startx, int starty, int width)
{
	char tempstr[1024];
	int theight;
	int tstrsize;
	int messageheight;
	int boxheight;
	int done = 0;
	int tempbottom = usebuff->ClipBottom;

	usebuff->ClipBottom = 240;

	boxheight = orgboxheight;

	while(!done)
	{
		theight = starty + 1;
		TXTDrawBoxFillRel(usebuff, startx, theight - 1, width, boxheight + 2, boxcolor);
		tstrsize = TXTGetFrontOfOptions(tempstr, 1024, optionnum);
		if(tstrsize == -1)
		{
			usebuff->ClipBottom = tempbottom;
			return(starty);
		}
		if(!LANGGetTransMessage(&tempstr[tstrsize], 1024 - tstrsize, messageid, g_iLanguageId))
		{
			usebuff->ClipBottom = tempbottom;
			return(starty);
		}
		messageheight = TXTWriteStringInBox(tempstr, usebuff, usefont, 1, theight, width - 2, 480 - (starty + 2 + orgboxheight), textcolor, 1, NULL, 0, 2);
		theight += messageheight;
		if(boxheight >= (messageheight - 2))
		{
			done = 1;
		}
		else
		{
			boxheight = messageheight - 2;
		}
	}
	usebuff->ClipBottom = tempbottom;
	return(theight);
}

//**************************************************************************************
int AICGetMaxMenuPixelLen(int messageid, int optionnum, GrFontBuff *usefont)
{
	char tempstr[1024];
	int tstrsize;

	tstrsize = TXTGetFrontOfOptions(tempstr, 1024, optionnum);
	if(tstrsize == -1)
	{
		return(0);
	}
	if(!LANGGetTransMessage(&tempstr[tstrsize], 1024 - tstrsize, messageid, g_iLanguageId))
	{
		return(0);
	}
	return(TXTGetStringPixelLength(tempstr, usefont));
}

//**************************************************************************************
int AICPixelLenGroupCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_WINGMAN_MENU, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_ELEMENT_MENU, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_DIVISION_MENU, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_FLIGHT_MENU, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	if(MultiPlayer)
	{
		worklen = AICGetMaxMenuPixelLen(AIC_FLIGHT_REPORTS, 5, MessageFont);
		if(worklen > messagelen)
		{
			messagelen = worklen;
		}

		worklen = AICGetMaxMenuPixelLen(AIC_ROGER_MSG, 6, MessageFont);
		if(worklen > messagelen)
		{
			messagelen = worklen;
		}

		worklen = AICGetMaxMenuPixelLen(AIC_NEGATIVE_MSG, 7, MessageFont);
		if(worklen > messagelen)
		{
			messagelen = worklen;
		}
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenFlightCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_ENGAGE_BANDITS, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_RADAR_ON_OFF, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_MUSIC_ON_OFF, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_REPORT_CONTACTS, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_SORT_BANDITS, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_GROUND_MENU, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_STATUS_MENU, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_FORMATION_MENU, 8, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_REJOIN_FLIGHT, 9, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_RETURN_TO_BASE, 0, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenDivisionElementCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_ENGAGE_BANDITS, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_SANITIZE_RIGHT, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_SANITIZE_LEFT, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_COVER_REJOIN, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_CONTINUE_ESCORT, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_BEGIN_CAP, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ORBIT_REJOIN, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_INTERCEPT_MENU, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_GROUND_MENU, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_REJOIN_FLIGHT, 8, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_RETURN_TO_BASE, 9, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenWingmanCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_ENGAGE_BANDITS, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_SANITIZE_RIGHT, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_SANITIZE_LEFT, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_COVER_REJOIN, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_CONTINUE_ESCORT, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_BEGIN_CAP, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ORBIT_REJOIN, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_TACTICAL_MENU, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_INTERCEPT_MENU, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_GROUND_MENU, 8, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_REJOIN_FLIGHT, 9, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_RETURN_TO_BASE, 0, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenGroundAttackCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_AG_PRIMARY, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_AG_SECONDARY, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_AG_OPPORTUNITY, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
//	worklen = AICGetMaxMenuPixelLen(AIC_AG_ABORT, 4, MessageFont);
//	if(worklen > messagelen)
//	{
//		messagelen = worklen;
//	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenWeaponOptionCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_UNGUIDED_ALL, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_UNGUIDED_HALF, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_UNGUIDED_SINGLE, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_GUIDED_ALL, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_GUIDED_HALF, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_GUIDED_SINGLE, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_MISSILE_ALL, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_MISSILE_HALF, 8, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_MISSILE_SINGLE, 9, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_DEFAULT, 0, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenInterceptCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_BRACKET_RIGHT, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_BRACKET_LEFT, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_SPLIT_HIGH, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_SPLIT_LOW, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_DRAG_RIGHT, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_DRAG_LEFT, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenTacticalCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_BREAK_RIGHT, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_BREAK_LEFT, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_BREAK_HIGH, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_BREAK_LOW, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ATTACK_MY_TARGET, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_HELP_ME, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenStatusCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_FLIGHT_CHECK, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_WEAPONS_CHECK, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_SAY_POSITION, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_SAY_FUEL, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenFormationCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_LOOSEN_FORM, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_TIGHTEN_FORM, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_V_FORM, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_WEDGE_FORM, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_LINE_ABREAST_FORM, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_ECHELON_FORM, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_TRAIL_FORM, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_WALL_FORM, 8, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICWEngageBandits()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICEngageBandits(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICEEngageBandits()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICEngageBandits(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICDEngageBandits()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICEngageBandits(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICFEngageBandits()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICEngageBandits(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICEngageBandits(int firstvalid, int lastvalid)
{
	long delaycnt = 1000;
	PlaneParams *planepnt = PlayerPlane;

#ifdef NO_AI_PLAYER
	if(iAICommFrom < 0)
	{
		if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
			return;
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}
#else
	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}
#endif

	if(planepnt->AI.wingman >= 0)
	{
		AICEngageTogglePlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICEngageTogglePlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid);
	}
	return;
}

//**************************************************************************************
void AICRadarToggle()
{

	long delaycnt = 1000;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
	if(iAICommFrom < 0)
	{
		if(!(PlayerPlane->Status & AL_DEVICE_DRIVEN))
			return;
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}
#else
	if(iAICommFrom < 0)
	{
		planepnt = PlayerPlane;
	}
	else
	{
		planepnt = &Planes[iAICommFrom];
	}
#endif

	if(planepnt->AI.wingman >= 0)
	{
		AICRadarTogglePlane(planepnt->AI.wingman, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICRadarTogglePlane(planepnt->AI.nextpair, &delaycnt);
	}

	if(lBombFlags & WSO_RADAR_OFF)
	{
		lBombFlags &= ~(WSO_RADAR_OFF);
	}
	else
	{
		lBombFlags |= (WSO_RADAR_OFF);
	}
	return;
}

//**************************************************************************************
void AICRadarTogglePlane(int planenum, long *delaycnt)
{
	int badradar = 0;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);
//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

//	if(Planes[planenum].AI.iAIFlags1 & AIRADARON)
	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(!(lBombFlags & WSO_RADAR_OFF))
		{
			Planes[planenum].AI.iAIFlags1 &= ~AIRADARON;
		}
		else
		{
			if(Planes[planenum].DamageFlags & DAMAGE_RADAR)
			{
				badradar = 1;
			}
			else
			{
				Planes[planenum].AI.iAIFlags1 |= AIRADARON;
			}
		}

		//  Set Sound Q for BasicAck for Plane planenum, delay of delaycnt;
		if(AICAllowThisRadio(planenum, 1))
		{
			if(badradar)
			{
				AICAddSoundCall(AICNoJoyMsg, planenum, *delaycnt, 50);
			}
			else
			{
				AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
			}
		}

		*delaycnt = *delaycnt + 2000;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICRadarTogglePlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICRadarTogglePlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICAddSoundCall(void (*Soundfunc)(int planenum, int targetnum), int planenum, long delaycnt, int priority, int targetnum)
{
	int cnt;
	int replacecnt = 0;
	int mustreplace = 0;
	int replacetime;

	for(cnt = 0; cnt < MAXAISOUNDQUEUES; cnt ++)
	{
		if(gAISoundQueue[cnt].lTimer < 0)
		{
			gAISoundQueue[cnt].Soundfunc = Soundfunc;
			gAISoundQueue[cnt].planenum = planenum;
			gAISoundQueue[cnt].targetnum = targetnum;
			gAISoundQueue[cnt].lTimer = delaycnt;
			gAISoundQueue[cnt].priority = priority;
			return;
		}

		if(gAISoundQueue[cnt].priority >= gAISoundQueue[replacecnt].priority)
		{
			if(gAISoundQueue[cnt].lTimer < gAISoundQueue[replacecnt].lTimer)
			{
				replacecnt = cnt;
			}
		}
	}

	if(cnt == MAXAISOUNDQUEUES)
	{
		cnt = MAXAISOUNDQUEUES - 1;
		replacetime = gAISoundQueue[0].lTimer;
		for(cnt = 1; cnt < MAXAISOUNDQUEUES; cnt ++)
		{
			if(gAISoundQueue[cnt].lTimer < replacetime)
			{
				replacetime = gAISoundQueue[cnt].lTimer;
				mustreplace = cnt;
			}
		}
		cnt = mustreplace;
	}

	if(replacetime > delaycnt)
	{
		Soundfunc(planenum, targetnum);
	}
	else
	{
		gAISoundQueue[cnt].Soundfunc(gAISoundQueue[cnt].planenum, gAISoundQueue[cnt].targetnum);
		gAISoundQueue[cnt].Soundfunc = Soundfunc;
		gAISoundQueue[cnt].planenum = planenum;
		gAISoundQueue[cnt].targetnum = targetnum;
		gAISoundQueue[cnt].lTimer = delaycnt;
		gAISoundQueue[cnt].priority = priority;
	}
	return;
}

//**************************************************************************************
void AICBasicAck(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	//  Make sound call for basic ack.

	AIRBasicAckSnd(planenum, placeingroup);

	//  Add message to message array.
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
	if(!LANGGetTransMessage(tempstr, 1024, AIC_BASIC_ACK, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
void AICAddAIRadioMsgs(char *radiomsg, int priority)
{
//	LTMessageData	holdmsg;  //  used for priority sorting
//	LTMessageData	swapmsg;  //  used for priority sorting
	int cnt;
	int tstrsize;
	float timercheck;
	int newmsgid = 0;

	tstrsize = strlen(radiomsg);
	if(tstrsize <= 0)
		return;

	if(tstrsize > (MAXRADIOMSGSIZE - 2))
	{
		radiomsg[MAXRADIOMSGSIZE - 2] = 0;
		tstrsize = MAXRADIOMSGSIZE;
	}

	if((gRadioMsgs[0].iMessagePriority != -1))
	{
		for(cnt = MAXNUMRADIOMSGS - 1; cnt > 0; cnt --)  //  Not used when sorting by priority
		{
			gRadioMsgs[cnt] = gRadioMsgs[cnt - 1];
		}
	}

	if((gRadioMsgs[0].iMessagePriority == -2))
	{
		newmsgid = 1;
	}

//	holdmsg = gRadioMsgs[newmsgid];
	strcpy(gRadioMsgs[newmsgid].sMessageText, radiomsg);
	gRadioMsgs[newmsgid].iMessagePriority = priority;
	gRadioMsgs[newmsgid].lTimer = lRadioDisplayDur;
	timercheck = tstrsize / 10.0f * 1250.0f;
	if(gRadioMsgs[newmsgid].lTimer < timercheck)
	{
		gRadioMsgs[newmsgid].lTimer = timercheck;
	}
	if(gRadioMsgs[newmsgid].lTimer < gRadioMsgs[newmsgid + 1].lTimer)
	{
		gRadioMsgs[newmsgid].lTimer = gRadioMsgs[newmsgid + 1].lTimer;
	}

#if 0  //  When sorting by priortiry
	for(cnt = 1; cnt < MAXNUMRADIOMSGS; cnt ++)
	{
		if((gRadioMsgs[cnt].lTimer < 0) || (gRadioMsgs[cnt].iMessagePriority >= holdmsg.iMessagePriority))
		{
			swapmsg = gRadioMsgs[cnt];
			gRadioMsgs[cnt] = holdmsg;
			holdmsg = swapmsg;
		}
	}
#endif
}

//**************************************************************************************
void AICDisplayAIRadioMsgs()
{
	int cnt;
	int numshow;
	int startx = 0;
	int starty = 0;
	int width = 640;
	int fontheight;
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	GrBuff *usebuff	= GrBuffFor3D;
	GrFontBuff *usefont = MessageFont;
	int theight;
	int messageheight;
	int boxheight;
	int done;
	int ignoretime = 0;
	int drawbox = 1;


	fontheight = TXTGetMaxFontHeight(usefont);

#if 0	//  Removed to allow things to show up when subtitles off.
	if(iRadioDisplayFlags & 4)
	{
		return;
	}
#endif

	if(iRadioDisplayFlags & 1)
	{
		numshow = iNumRadioHistDisplay;
		if((Camera1.SubType & (COCKPIT_FRONT|COCKPIT_45DOWN)) == (COCKPIT_FRONT|COCKPIT_45DOWN))
		{
			if(numshow > 8)
			{
				numshow = 8;
			}
		}

		ignoretime = 1;
	}
	else
	{
		numshow = iNumRadioDisplay;
	}

	if(iRadioDisplayFlags & 2)
	{
		drawbox = 0;
		textcolor =   -1;  //  Should have us using the HUD colorHUDColor;
	}

	if(XmitChannel)
	{
		starty += fontheight + (fontheight >> 1);
	}

	for(cnt = 0; cnt < numshow; cnt ++)
	{
		if((gRadioMsgs[cnt].iMessagePriority == -2) && (!SimPause))
		{
			gRadioMsgs[cnt].sMessageText[0] = 0;
			gRadioMsgs[cnt].iMessagePriority = -1;
		}

		if(((!(iRadioDisplayFlags & 4)) || (gRadioMsgs[cnt].iMessagePriority == 69)) && ((gRadioMsgs[cnt].lTimer >= 0) || ((ignoretime) && (strlen(gRadioMsgs[cnt].sMessageText)))))
		{
			boxheight = fontheight;
			done = 0;

			while((!done) && (!iInJump))
			{
				theight = starty + 1;
				if(drawbox)
				{
					TXTDrawBoxFillRel(usebuff, startx, theight - 1, width, boxheight + 2, boxcolor);
				}
				messageheight = TXTWriteStringInBox(gRadioMsgs[cnt].sMessageText, usebuff, usefont, 1, theight, width - 2, 480 - (starty + 2 + fontheight), textcolor, 1, NULL, 0, 2);
				theight += messageheight;
				if(boxheight >= (messageheight - 2))
				{
					done = 1;
				}
				else
				{
					boxheight = messageheight - 2;
				}
			}
			starty = theight;
		}
	}

	for(cnt = 0; cnt < MAXNUMRADIOMSGS; cnt ++)
	{
		if(gRadioMsgs[cnt].lTimer >= 0)
		{
			gRadioMsgs[cnt].lTimer -= DeltaTicks;
		}
	}
}

//**************************************************************************************
void AICDisplayPausedRadioMsgs()
{
	int cnt;
	int numshow;
	int startx = 0;
	int starty = 0;
	int width = 640;
	int fontheight;
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	GrBuff *usebuff	= GrBuffFor3D;
	GrFontBuff *usefont = MessageFont;
	int theight;
	int messageheight;
	int boxheight;
	int done;
	int ignoretime = 0;
	int drawbox = 1;


	fontheight = TXTGetMaxFontHeight(usefont);

	if(iRadioDisplayFlags & 1)
	{
		numshow = iNumRadioHistDisplay;
		ignoretime = 1;
	}
	else
	{
		numshow = iNumRadioDisplay;
	}

#if 0
	if(iRadioDisplayFlags & 4)
	{
		if(gRadioMsgs[0].iMessagePriority == -2)
		{
			numshow = 1;
		}
		else
		{
			return;
		}
	}
#endif

	if(iRadioDisplayFlags & 2)
	{
		drawbox = 0;
		textcolor =   -1;  //  Should have us using the HUD colorHUDColor;
	}

	if(XmitChannel)
	{
		starty += fontheight + (fontheight >> 1);
	}

	for(cnt = 0; cnt < numshow; cnt ++)
	{
		if(((!(iRadioDisplayFlags & 4)) || (gRadioMsgs[cnt].iMessagePriority == 69) || (gRadioMsgs[cnt].iMessagePriority == -2)) && ((gRadioMsgs[cnt].iMessagePriority < 0) || (g_nMissionType == SIM_TRAIN) || (ignoretime)) && ((gRadioMsgs[cnt].lTimer >= 0) || ((ignoretime) && (strlen(gRadioMsgs[cnt].sMessageText)))))
		{
			boxheight = fontheight;
			done = 0;

			if(gRadioMsgs[cnt].iMessagePriority == -2)
			{
				gRadioMsgs[cnt].lTimer = 1;
			}

			while((!done) && (!iInJump))
			{
				theight = starty + 1;
				if(drawbox)
				{
					TXTDrawBoxFillRel(usebuff, startx, theight - 1, width, boxheight + 2, boxcolor);
				}
				messageheight = TXTWriteStringInBox(gRadioMsgs[cnt].sMessageText, usebuff, usefont, 1, theight, width - 2, 480 - (starty + 2 + fontheight), textcolor, 1, NULL, 0, 2);
				theight += messageheight;
				if(boxheight >= (messageheight - 2))
				{
					done = 1;
				}
				else
				{
					boxheight = messageheight - 2;
				}
			}
			starty = theight;
		}
	}

#if 0
	for(cnt = 0; cnt < MAXNUMRADIOMSGS; cnt ++)
	{
		if(gRadioMsgs[cnt].lTimer >= 0)
		{
			gRadioMsgs[cnt].lTimer -= DeltaTicks;
		}
	}
#endif
}

//**************************************************************************************
void AICDisengageMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int placeingroup;
	int tempnum, msgnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}


	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_DISENGAGE_MSG;
	}
	else
	{
		msgnum = AIC_DISENGAGE_MSG_2;
	}

	//  Make sound call
	AIRDisengageSnd(planenum, placeingroup, tempnum);

//	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
//	if(!LANGGetTransMessage(tempstr, 1024, AIC_DISENGAGE_MSG, g_iLanguageId, callsign, tempstr2))
	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICGetCallSign(int planenum, char *callsign, int putlast)
{
	char tempstr[128];
	char flightnum[64];
	char wingnum[64];

//	strcpy(callsign, CallSignList[AICGetCallSignNum(Planes[planenum].AI.lCallSign)].sName);
	strcpy(tempstr, CallSignList[Planes[planenum].AI.lCallSign].sName);

	if(!LANGGetTransMessage(flightnum, 64, AIC_ZERO + Planes[planenum].AI.cCallSignNum, g_iLanguageId))
	{
		return;
	}

//	if(!LANGGetTransMessage(wingnum, 64, AIC_ZERO + (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS) + 1, g_iLanguageId))
	if(!LANGGetTransMessage(wingnum, 64, AIC_ONE, g_iLanguageId))
	{
		return;
	}

	if(putlast)
	{
		sprintf(callsign, "%s %s %s", tempstr, flightnum, wingnum);
	}
	else
	{
		sprintf(callsign, "%s %s", tempstr, flightnum);
	}

	return;
}

//**************************************************************************************
long AICGetCallSignNum(long callsignnum)
{
#if 0
	int i;
	int planetype, callsignid;

	planetype = (callsignnum & 0xFF00) >> 8;
	callsignid = callsignnum & 0xFF;
	for (i=0;i<(sizeof(CallSignList)/sizeof(CallSignType)); i++)
	{
		if ((callsignid == CallSignList[i].iID) && (planetype == CallSignList[i].iPlaneType))
		{
			return(i);
		}
	}
	return(0);
#else
	return(callsignnum & 0xFF);
#endif
}

//**************************************************************************************
void AICEngageTogglePlane(int planenum, long *delaycnt, int firstvalid, int lastvalid)
{
	int placeingroup;
//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid))
	{
		Planes[planenum].AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
		Planes[planenum].AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);
		Planes[planenum].AI.iAIFlags2 &= ~(AIKEEPTARGET);

		if(Planes[planenum].AI.AirTarget == NULL)
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICNoJoyMsg, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 3000;
			}
		}
		else
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICEngageMsg, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 3000;
			}
			if(Planes[planenum].AI.OrgBehave == NULL)
			{
				Planes[planenum].AI.OrgBehave = Planes[planenum].AI.Behaviorfunc;
//				Planes[planenum].AI.Behaviorfunc = AIHeadAtAirTarget;
				Planes[planenum].AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;

			}
			else
			{
//				Planes[planenum].AI.OrgBehave = AIHeadAtAirTarget;
				Planes[planenum].AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
			}

			//  Temp for now, make HeadAtAir not time Human plane groups;
//			Planes[planenum].AI.lTimer1 = 360000;
			Planes[planenum].AI.lTimer1 = 660000 - (Planes[planenum].AI.iSkill * 100000);
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICEngageTogglePlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICEngageTogglePlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid);
	}
	return;
}

//**************************************************************************************
void AICNoJoyMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	//  Make sound call for basic ack.
//	AIRNoJoySnd(planenum, placeingroup);
	AIRGenericSpeech(AICF_NO_JOY_MSG, planenum);

//	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
	if(Planes[planenum].AI.lAIVoice != SPCH_FTR_RUS)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_BASIC_NEG_2, g_iLanguageId, tempstr2))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_BASIC_NEG_2_RUSF, g_iLanguageId, tempstr2))
		{
			return;
		}
	}

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICEngageMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
//	char heading[256];
//	char grouptext[256];
	int placeingroup;
	int headingval = 0;
	int numingroup = 0;
	int tempnum, msgnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	tempnum = rand() % 3;

	if(tempnum == 0)
	{
		if((Planes[planenum].AI.lAIVoice != SPCH_FTR_RUS) && (Planes[planenum].AI.lAIVoice != SPCH_FTR_RAF))
		{
			msgnum = AIC_ENGAGE_MSG;
		}
		else
		{
			msgnum = AIC_ENGAGE_MSG_RAF;
		}
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_ENGAGE_MSG_2;
	}
	else
	{
		msgnum = AIC_ENGAGE_MSG_3;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	//  Make sound call for basic ack.

//	AICGetCallSign(planenum, callsign);
//	headingval = AICGetTextHeadingToPlane(heading, &Planes[planenum], Planes[planenum].AI.AirTarget);
//	numingroup = AICGetIfPlaneGroupText(grouptext, Planes[planenum].AI.AirTarget);

	//  Don't think headingval and numingroup gets used.
	AIREngageSnd(planenum, placeingroup, headingval, numingroup, tempnum);

	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
//	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, tempstr2, heading, grouptext))
	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
int AICGetIfPlaneGroupText(char *grouptext, PlaneParams *planepnt, int singletxt)
{
	int morethanone = 0;

	grouptext[0] = 0;

	//  Probably want to change this to look for planes around this area  SRE
	morethanone = AICMorePlanesNear(planepnt);

	if(morethanone)
	{
		if(!LANGGetTransMessage(grouptext, 256, AIC_GROUP, g_iLanguageId))
		{
			return(-1);
		}
		return(10);
	}
	else
	{
		if(singletxt)
		{
			if(!LANGGetTransMessage(grouptext, 256, AIC_SINGLE, g_iLanguageId))
			{
				return(-1);
			}
			return(1);
		}
		else
		{
			if(!LANGGetTransMessage(grouptext, 256, AIC_BANDIT, g_iLanguageId))
			{
				return(-1);
			}
			return(2);
		}
	}

	return(1);
}

//**************************************************************************************
int AICGetIfPlaneGroupRadarText(char *grouptext, PlaneParams *planepnt, int *spch_id, int *snd_id, int voice, int ispopup)
{
	int morethanone = 0;

	grouptext[0] = 0;

	//  Probably want to change this to look for planes around this area  SRE
	morethanone = AICMorePlanesNear(planepnt);

	if(morethanone)
	{
		if((!ispopup) || (ispopup == 2))
		{
			*spch_id = AIC_GROUP_INFO;
			if(ispopup == 2)
			{
				*snd_id = AICF_CS_GROUP_INFO;
			}
			else
			{
				*snd_id = AICF_GROUP_INFO;
			}
			if(!LANGGetTransMessage(grouptext, 256, AIC_GROUP_INFO, g_iLanguageId))
			{
				return(-1);
			}
		}
		else if(ispopup == -1)
		{
			*spch_id = AIC_LONE_GROUP;
			*snd_id = AICF_LONE_GROUP;
			if(!LANGGetTransMessage(grouptext, 256, AIC_LONE_GROUP, g_iLanguageId))
			{
				return(-1);
			}
		}
		else
		{
			*spch_id = AIC_POPUP_GROUP;
			*snd_id = AICF_POPUP_GROUP;
			if(!LANGGetTransMessage(grouptext, 256, AIC_POPUP_GROUP, g_iLanguageId))
			{
				return(-1);
			}
		}
		return(10);
	}
	else
	{
		if((!ispopup) || (ispopup == 2))
		{
			*spch_id = AIC_SINGLE_INFO;
			if(ispopup == 2)
			{
				*snd_id = AICF_CS_SINGLE_INFO;
			}
			else
			{
				*snd_id = AICF_SINGLE_INFO;
			}
			if(!LANGGetTransMessage(grouptext, 256, AIC_SINGLE_INFO, g_iLanguageId))
			{
				return(-1);
			}
		}
		else if(ispopup == -1)
		{
			*spch_id = AIC_LONE_SINGLE;
			*snd_id = AICF_LONE_SINGLE;
			if(!LANGGetTransMessage(grouptext, 256, AIC_LONE_SINGLE, g_iLanguageId))
			{
				return(-1);
			}
		}
		else
		{
			*spch_id = AIC_POPUP_SINGLE;
			*snd_id = AICF_POPUP_SINGLE;
			if(!LANGGetTransMessage(grouptext, 256, AIC_POPUP_SINGLE, g_iLanguageId))
			{
				return(-1);
			}
		}
		return(1);
	}

	if(!ispopup)
	{
		*spch_id = AIC_SINGLE_INFO;
		*snd_id = AICF_SINGLE_INFO;
		if(!LANGGetTransMessage(grouptext, 256, AIC_SINGLE_INFO, g_iLanguageId))
		{
			return(-1);
		}
	}
	else if(ispopup == -1)
	{
		*spch_id = AIC_LONE_SINGLE;
		*snd_id = AICF_LONE_SINGLE;
		if(!LANGGetTransMessage(grouptext, 256, AIC_LONE_SINGLE, g_iLanguageId))
		{
			return(-1);
		}
	}
	else
	{
		*spch_id = AIC_POPUP_SINGLE;
		*snd_id = AICF_POPUP_SINGLE;
		if(!LANGGetTransMessage(grouptext, 256, AIC_POPUP_SINGLE, g_iLanguageId))
		{
			return(-1);
		}
	}
	return(1);
}

//**************************************************************************************
int AICGetTextHeadingToPlane(char *headingtxt, PlaneParams *planepnt, PlaneParams *target)
{
	float heading = 0;
	float dx, dz;

	dx = target->WorldPosition.X - planepnt->WorldPosition.X;
	dz = target->WorldPosition.Z - planepnt->WorldPosition.Z;

	heading = atan2(-dx, -dz) * 57.2958;

	heading = AICapAngle(heading);

	return(AICGetTextHeadingGeneral(headingtxt, heading));
}

//**************************************************************************************
int AICGetTextHeadingGeneral(char *headingtxt, float orgheading)
{
	float heading = -orgheading;

	while(heading > 360)
		heading -= 360;

	while(heading < 0)
		heading += 360;

	if(heading < 22.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_NORTH, g_iLanguageId))
		{
			return(-1);
		}
		return(0);
	}
	else if(heading < 67.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_NORTH_EAST, g_iLanguageId))
		{
			return(-1);
		}
		return(1);
	}
	else if(heading < 112.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_EAST, g_iLanguageId))
		{
			return(-1);
		}
		return(2);
	}
	else if(heading < 157.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_SOUTH_EAST, g_iLanguageId))
		{
			return(-1);
		}
		return(3);
	}
	else if(heading < 202.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_SOUTH, g_iLanguageId))
		{
			return(-1);
		}
		return(4);
	}
	else if(heading < 247.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_SOUTH_WEST, g_iLanguageId))
		{
			return(-1);
		}
		return(5);
	}
	else if(heading < 292.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_WEST, g_iLanguageId))
		{
			return(-1);
		}
		return(6);
	}
	else if(heading < 337.5)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_NORTH_WEST, g_iLanguageId))
		{
			return(-1);
		}
		return(7);
	}
	else
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_NORTH, g_iLanguageId))
		{
			return(-1);
		}
		return(0);
	}
	return(0);
}

//**************************************************************************************
void AICInitAIRadio()
{
	int cnt;

	gAICommMenu.AICommMenufunc = NULL;  //***  SRE NEED TO PUT INTO TRUE VERSION

	gAICommMenu.AICommKeyfunc = NULL;
	for(cnt = 0; cnt < MAXNUMRADIOMSGS; cnt ++)
	{
		gRadioMsgs[cnt].iMessagePriority = -1;		 	//  Priority of Message.
		gRadioMsgs[cnt].lTimer = -1;						//  Holds how many ticks left to display message.
		gRadioMsgs[cnt].sMessageText[0] = 0;
	}
	for(cnt = 0; cnt < MAXAISOUNDQUEUES; cnt ++)
	{
		gAISoundQueue[cnt].lTimer = -1;
	}
}

//**************************************************************************************
void AICJammingToggle()
{

	long delaycnt = 1000;
#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}


	if(lBombFlags & WSO_JAMMER_ON)
	{
		lBombFlags &= ~(WSO_JAMMER_ON);
	}
	else
	{
		lBombFlags |= (WSO_JAMMER_ON);
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICJammingTogglePlane(planepnt->AI.wingman, &delaycnt);
	}

	if(PlayerPlane->AI.nextpair >= 0)
	{
		AICJammingTogglePlane(planepnt->AI.nextpair, &delaycnt);
	}
	return;
}

//**************************************************************************************
void AICJammingTogglePlane(int planenum, long *delaycnt)
{
//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);

	if(!AIInPlayerGroup(&Planes[planenum]))
	{
		return;
	}

	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(Planes[planenum].AI.iAIFlags1 & AI_KEEP_JAMMER_ON)
		{
			Planes[planenum].AI.iAIFlags1 &= ~(AIJAMMINGON|AI_KEEP_JAMMER_ON);
		}
		else
		{
			Planes[planenum].AI.iAIFlags1 |= (AIJAMMINGON|AI_KEEP_JAMMER_ON);
		}

		if(AICAllowThisRadio(planenum, 1))
		{
			//  Set Sound Q for BasicAck for Plane planenum, delay of delaycnt;
			AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
			*delaycnt = *delaycnt + 2000;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICJammingTogglePlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICJammingTogglePlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICFlightStatus()
{

	long delaycnt = 3000;
	PlaneParams *planepnt = PlayerPlane;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
#ifdef NO_AI_PLAYER
	else if(!(PlayerPlane->Status & AL_DEVICE_DRIVEN))
	{
		return;
	}
#endif
	else
	{
		planepnt = PlayerPlane;
	}

#if F15_SPEECH
	if(AICAllowThisRadio(planepnt - Planes, 1))
	{
		AIC_WSO_Check_Msgs(planepnt - Planes, 0);
	}
#endif

	if(planepnt->AI.wingman >= 0)
	{
		AICFlightStatusPlane(planepnt->AI.wingman, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICFlightStatusPlane(planepnt->AI.nextpair, &delaycnt);
	}
	return;
}

//**************************************************************************************
void AICFlightStatusPlane(int planenum, long *delaycnt)
{
	int planeok = 1;
	PlaneParams *planepnt = &Planes[planenum];
	long adddelay;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(AICAllowThisRadio(planenum, 1))
		{
			adddelay = 0;
			if(planepnt->DamageFlags & (DAMAGE_BIT_RADAR))
			{
				adddelay = adddelay + 2000;
				planeok = 0;
			}
			if(planepnt->DamageFlags & (DAMAGE_BIT_EMER_HYD))
			{
				adddelay = adddelay + 2000;
				planeok = 0;
			}
			if(planepnt->DamageFlags & (DAMAGE_BIT_FUEL_TANKS))
			{
				adddelay = adddelay + 2000;
				planeok = 0;
			}

			if((planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)) || (planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
			{
				adddelay = adddelay + 2000;
				planeok = 0;
			}

			if(planeok)
			{
				AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 2000;
			}
			else
			{
				AICAddSoundCall(AICReportWingmanDamage, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 2000 + adddelay;
			}
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICFlightStatusPlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICFlightStatusPlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICWedgeFormationChange()
{
	AICFormationChange(8);
}

//**************************************************************************************
void AICFormationChange(int formationid)
{

	long delaycnt = 1000;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	AIInitFormation(planepnt, formationid, 0);

	if(planepnt->AI.wingman >= 0)
	{
		AICBasicAckPlane(planepnt->AI.wingman, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICBasicAckPlane(planepnt->AI.nextpair, &delaycnt);
	}
	return;
}

//**************************************************************************************
void AICBasicAckPlane(int planenum, long *delaycnt)
{
	int planeok = 1;

	if((!(AIInPlayerGroup(&Planes[planenum]))) && (iAICommFrom < 0))
	{
		return;
	}

	if((!MultiPlayer) || (!iLeadPlace) || (((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		if(AICAllowThisRadio(planenum, 1))
		{
			AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
			*delaycnt = *delaycnt + 2000;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICBasicAckPlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICBasicAckPlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICFormationLoosen()
{

	long delaycnt = 1000;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICFormationModifyPlane(planepnt->AI.wingman, &delaycnt, 2.0);
	}

	if(PlayerPlane->AI.nextpair >= 0)
	{
		AICFormationModifyPlane(planepnt->AI.nextpair, &delaycnt, 2.0);
	}
	return;
}

//**************************************************************************************
void AICFormationTighten()
{

	long delaycnt = 1000;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICFormationModifyPlane(planepnt->AI.wingman, &delaycnt, 0.5);
	}

	if(PlayerPlane->AI.nextpair >= 0)
	{
		AICFormationModifyPlane(planepnt->AI.nextpair, &delaycnt, 0.5);
	}
	return;
}

//**************************************************************************************
void AICFormationModifyPlane(int planenum, long *delaycnt, float modifyby)
{
	int planeok = 1;
	float fworkval;

	if(((AIGetLeader(&Planes[planenum], 1) == AIGetLeader(PlayerPlane, 1)) && (!AIInPlayerGroup(&Planes[planenum]))) && (iAICommFrom < 0))
	{
		return;
	}

	if((!MultiPlayer) || (!iLeadPlace) || (((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= iFirstPlace) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= iLastPlace)))
	{
		Planes[planenum].AI.iAICombatFlags2 &= ~(AI_LEADER_MASK);
		if((MultiPlayer) && (iLeadPlace))
		{
			Planes[planenum].AI.iAICombatFlags2 |= (iLeadPlace);
		}

		fworkval = (float)Planes[planenum].AI.FormationPosition.X * modifyby;
		Planes[planenum].AI.FormationPosition.X = fworkval;

		fworkval = (float)Planes[planenum].AI.FormationPosition.Y * modifyby;
		Planes[planenum].AI.FormationPosition.Y = fworkval;

		fworkval = (float)Planes[planenum].AI.FormationPosition.Z * modifyby;
		Planes[planenum].AI.FormationPosition.Z = fworkval;

		int placeingroup = Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS;
		if(placeingroup < 0)
		{
			placeingroup = 0;
		}
		else if(placeingroup > 7)
		{
			placeingroup = 7;
		}

		fpPlayerGroupFormation[placeingroup] *= modifyby;

		if(AICAllowThisRadio(planenum, 1))
		{
			if(modifyby > 1.0f)
			{
				AICAddSoundCall(AICWingSpreadOutMsg, planenum, *delaycnt, 50);
			}
			else if(modifyby < 1.0f)
			{
				AICAddSoundCall(AICWingCloseUpMsg, planenum, *delaycnt, 50);
			}
			else
			{
				AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
			}

			*delaycnt = *delaycnt + 2000;
		}

		if(Planes[planenum].AI.wingman >= 0)
		{
			AICFormationModifyPlane(Planes[planenum].AI.wingman, delaycnt, modifyby);
		}

		if(Planes[planenum].AI.nextpair >= 0)
		{
			AICFormationModifyPlane(Planes[planenum].AI.nextpair, delaycnt, modifyby);
		}
	}
	return;
}

//**************************************************************************************
void AICWRejoinFlight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICRejoinFlight(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICERejoinFlight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICRejoinFlight(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICDRejoinFlight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICRejoinFlight(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICFRejoinFlight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICRejoinFlight(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICRejoinFlight(int firstvalid, int lastvalid)
{
	long delaycnt = 1000;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICRejoinFlightPlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid);
	}

	if(PlayerPlane->AI.nextpair >= 0)
	{
		AICRejoinFlightPlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid);
	}
	return;
}

//**************************************************************************************
void AICRejoinFlightPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid)
{
	int placeingroup;
//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if(((placeingroup >= firstvalid) && (placeingroup <= lastvalid)) && ((!(Planes[planenum].AI.iAIFlags2 & AILANDING)) || (Planes[planenum].AI.lTimer2 > 0)) && (!Planes[planenum].OnGround))
	{
		if((Planes[planenum].AI.iAIFlags1 & AIENGAGED) && (Planes[planenum].AI.AirTarget))
		{
			Planes[planenum].AI.iAIFlags1 &= ~(AIENGAGED|AICANENGAGE);
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICDisengageMsg, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 5000;
			}
		}
		else
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 2000;
			}
		}
		Planes[planenum].AI.iAIFlags2 &= ~(AILANDING|AI_CAS_ATTACK);

		//  May have to do something here to check for defensive action being done.  SRE
		if(placeingroup < 0)
		{
			Planes[planenum].AI.FormationPosition = fpPlayerGroupFormation[0];
		}
		else if(placeingroup > 7)
		{
			Planes[planenum].AI.FormationPosition = fpPlayerGroupFormation[7];
		}
		else
		{
			Planes[planenum].AI.FormationPosition = fpPlayerGroupFormation[placeingroup];
		}

		Planes[planenum].AI.Behaviorfunc = AIFlyFormation;
		Planes[planenum].AI.OrgBehave = NULL;
	}

//	AIUpdateCruiseFormation(&Planes[planenum], &placeingroup, 1);

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICRejoinFlightPlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICRejoinFlightPlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid);
	}
	return;
}

//**************************************************************************************
void AICWGrndAttackPrime()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICEGrndAttackPrime()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICDGrndAttackPrime()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICFGrndAttackPrime()
{
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
		if(AICAllowThisRadio(iAICommFrom, 1))
		{
			AIC_WSO_Begin_Attack_Msgs(planepnt - Planes, 0);
		}
	}
	else
	{
		planepnt = PlayerPlane;
		AIC_WSO_Begin_Attack_Msgs(planepnt - Planes, 0);
	}

	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICWGrndAttackSecond()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICEGrndAttackSecond()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICDGrndAttackSecond()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICFGrndAttackSecond()
{
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
	 	planepnt = &Planes[iAICommFrom];
		if(AICAllowThisRadio(iAICommFrom, 1))
		{
			AIC_WSO_Begin_Attack_Msgs(planepnt - Planes, 0);
		}
	}
	else
	{
		planepnt = PlayerPlane;
		AIC_WSO_Begin_Attack_Msgs(PlayerPlane - Planes, 0);
	}

	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICWGrndAttackOpportunity()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICEGrndAttackOpportunity()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICDGrndAttackOpportunity()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICFGrndAttackOpportunity()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICWGrndAttackSEAD()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICEGrndAttackSEAD()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICDGrndAttackSEAD()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICFGrndAttackSEAD()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICGrndAttack(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICGrndAttack(int firstvalid, int lastvalid, int targetpriority)
{
	long delaycnt = 1000;
	PlaneParams *planepnt;

	if((firstvalid == 1) && (lastvalid == 7) && (targetpriority < 3))
	{
		delaycnt += 2000;
	}

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom))
		return;
#endif

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICGrndAttackPlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid, targetpriority);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICGrndAttackPlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid, targetpriority);
	}
	return;
}

//**************************************************************************************
void AICGrndAttackPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int targetpriority)
{
	int placeingroup;
	MBWayPoints	*bombway;	//  This is a pointer to the bomb waypoint.

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid) && ((!(Planes[planenum].AI.iAIFlags2 & AILANDING)) || (Planes[planenum].AI.lTimer2 > 0)) && (!Planes[planenum].OnGround))
	{
		if(Planes[planenum].AI.iAICombatFlags1 & AI_WINCHESTER_AG)
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AIC_Basic_Neg, planenum, *delaycnt, 50, 1);
				*delaycnt = *delaycnt + 3000;
			}
		}
		else
		{
			if(Planes[planenum].AI.iAIFlags1 & AIENGAGED)
			{
				Planes[planenum].AI.iAIFlags1 &= ~(AIENGAGED|AICANENGAGE);
			}

			bombway = AICGetBombWaypoint(planenum, targetpriority);

//			if((bombway != NULL) || ((targetpriority == 3) && (iFACState)))
			if((bombway != NULL) || (targetpriority >= 3))  //  For patch so can always bomb stuff.
			{
				if(targetpriority < 3)
				{
					if(targetpriority == 1)
					{
						Planes[planenum].AI.iAIFlags2 |= AI_BOMBED_PRIME;
					}
					else
					{
						Planes[planenum].AI.iAIFlags2 |= AI_BOMBED_SECOND;
					}
					Planes[planenum].AI.WayPosition.Y = ConvertWayLoc(bombway->lWPy);
					AICGetBombAction(planenum, bombway);
				}
				else
				{
					AICSetUpPlayerCAS(planenum, bombway);
					Planes[planenum].AI.WayPosition.X = -1;
					if(targetpriority == 4)
					{
						Planes[planenum].AI.iAIFlags2 |= (AI_CAS_ATTACK|AI_SEAD_ATTACK);
					}
				}

				if(Planes[planenum].AI.cActiveWeaponStation == -1)
				{
					if(AICAllowThisRadio(planenum, 1))
					{
						AICAddSoundCall(AIC_Basic_Neg, planenum, *delaycnt, 50, 1);
						*delaycnt = *delaycnt + 3000;
					}
				}
				else
				{
					if(AICAllowThisRadio(planenum, 1))
					{
						AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
						*delaycnt = *delaycnt + 2000;
					}
					if(bombway)
					{
						Planes[planenum].AI.CurrWay = bombway;
					}
				}
			}
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICGrndAttackPlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid, targetpriority);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICGrndAttackPlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid, targetpriority);
	}
	return;
}

//**************************************************************************************
MBWayPoints	*AICGetBombWaypoint(int planenum, int targetpriority)
{
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
 	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return(NULL);
#endif

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return(NULL);
	}

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}


	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
			{
				bombfound ++;
				if(bombfound >= targetpriority)
				{
					return(lookway);
				}
			}
			if((AIActions[actcnt].ActionID == ACTION_ATTACKTARGET))
			{
				bombfound ++;
				if(bombfound >= targetpriority)
				{
					return(lookway);
				}
			}
			if((AIActions[actcnt].ActionID == ACTION_RELEASE_CHUTES))
			{
				bombfound ++;
				if(bombfound >= targetpriority)
				{
					return(lookway);
				}
			}
			if((AIActions[actcnt].ActionID == ACTION_CAS) || (AIActions[actcnt].ActionID == ACTION_SEAD))
			{
				if(targetpriority == 3)
				{
					return(lookway);
				}
			}
		}
		lookway ++;
	}
	return(NULL);
}

//**************************************************************************************
void AICGetBombAction(int planenum, MBWayPoints *bombway)
{
	int actcnt, startact, endact;
	void (*bombfunc)(PlaneParams *planepnt);		//  This is a pointer to the correct bombing function.
	BombTarget *pActionBombTarget;
	int placeingroup;
	BasicInstance *walker;
	int bombtarget;
	int cnt;

	Planes[planenum].AI.iAIFlags2 &= ~AI_CAS_ATTACK;
	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	startact = bombway->iStartAct;
	endact = startact + bombway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		switch (AIActions[actcnt].ActionID)
		{
			case ACTION_BOMB_TARGET:
				pActionBombTarget=(BombTarget *)AIActions[actcnt].pAction;
				switch(pActionBombTarget->iAttackProfile)
				{
					case 1:
						bombfunc = AILevelBomb;
						break;
					case 2:		//  Will be Pop-up
						bombfunc = AIFlyToPopPoint;
						break;
					case 3:
						bombfunc = AIFlyToLoftPoint;
						break;
					case 4:
						bombfunc = AIFlyToDivePoint;
						break;
					default:
						bombfunc = AILevelBomb;
						break;
				}
				Planes[planenum].AI.iVar1 = 0;


				//  Check this SRE
#if 0
				if(Planes[planenum].AI.OrgBehave == NULL)
				{
					Planes[planenum].AI.Behaviorfunc = bombfunc;
				}
				else
				{
					Planes[planenum].AI.OrgBehave = bombfunc;
				}
#else
				Planes[planenum].AI.Behaviorfunc = bombfunc;
				Planes[planenum].AI.OrgBehave = NULL;
#endif

				if(Planes[planenum].AI.iAIFlags1 & AIPLAYERGROUP)
				{
					int numwpts;
					numwpts = PlayerPlane->AI.numwaypts + (PlayerPlane->AI.CurrWay - &AIWayPoints[PlayerPlane->AI.startwpts]);
					Planes[planenum].AI.CurrWay = bombway;
					Planes[planenum].AI.numwaypts = numwpts - (Planes[planenum].AI.CurrWay - &AIWayPoints[Planes[planenum].AI.startwpts]);

					if(bombway->iSpeed)
					{
						Planes[planenum].AI.lDesiredSpeed = bombway->iSpeed;
					}

					if(Planes[planenum].AI.lDesiredSpeed <= 100)
					{
						if(!(pDBAircraftList[Planes[planenum].AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_HELICOPTER|AIRCRAFT_TYPE_VERTICAL)))
						{
							Planes[planenum].AI.lDesiredSpeed = 350;
						}
						else if(Planes[planenum].AI.lDesiredSpeed <= 0)
						{
							Planes[planenum].AI.lDesiredSpeed = 50;
						}
					}

				}

				bombtarget = placeingroup;

				if(pActionBombTarget->FlightAttackList[bombtarget].dwSerialNumber == 0)
				{
					cnt = 0;
					while((pActionBombTarget->FlightAttackList[cnt].dwSerialNumber) && (cnt < 8))
					{
						cnt ++;
					}

					if(cnt)
					{
						bombtarget = placeingroup % cnt;
					}
					else
					{
						bombtarget = 0;
					}
				}

				walker = FindInstance(AllInstances, pActionBombTarget->FlightAttackList[bombtarget].dwSerialNumber);
				Planes[planenum].AI.pGroundTarget = walker;
//				AIDetermineBestBomb(&Planes[planenum], pActionBombTarget->FlightAttackList[bombtarget].lType);
				AIDetermineBestBomb(&Planes[planenum], Planes[planenum].AI.pGroundTarget);

				AIGetPlaceInAttack(&Planes[planenum]);

				if(walker)
				{
					Planes[planenum].AI.WayPosition.X = walker->Position.X;
				//	Planes[planenum].AI.WayPosition.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
					Planes[planenum].AI.WayPosition.Z = walker->Position.Z;
					Planes[planenum].AI.TargetPos = walker->Position;
					Planes[planenum].AI.lGroundTargetFlag = GROUNDOBJECT;
				}
				else
				{
					FPoint temppos;
					temppos.X = pActionBombTarget->FlightAttackList[bombtarget].X;
					temppos.Y = pActionBombTarget->FlightAttackList[bombtarget].Y;
					temppos.Z = pActionBombTarget->FlightAttackList[bombtarget].Z;

					if(((pActionBombTarget->FlightAttackList[bombtarget].X > 0) || (pActionBombTarget->FlightAttackList[bombtarget].Y > 0) || (pActionBombTarget->FlightAttackList[bombtarget].Z > 0)) && ((temppos - Planes[planenum].AI.WayPosition) < (50.0f * NMTOWU)))
					{
						Planes[planenum].AI.WayPosition.X = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].X);
					//	Planes[planenum].AI.WayPosition.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
						Planes[planenum].AI.WayPosition.Z = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Z);
						Planes[planenum].AI.TargetPos.X = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].X);
						Planes[planenum].AI.TargetPos.Y = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Y);
						Planes[planenum].AI.TargetPos.Z = ConvertWayLoc(pActionBombTarget->FlightAttackList[bombtarget].Z);
						Planes[planenum].AI.lGroundTargetFlag = 0;
					}
					else
					{
						Planes[planenum].AI.WayPosition.X = ConvertWayLoc(bombway->lWPx);
						Planes[planenum].AI.WayPosition.Z = ConvertWayLoc(bombway->lWPz);
						Planes[planenum].AI.TargetPos.X = ConvertWayLoc(bombway->lWPx);
						Planes[planenum].AI.TargetPos.Y = ConvertWayLoc(bombway->lWPy);
						Planes[planenum].AI.TargetPos.Z = ConvertWayLoc(bombway->lWPz);
						Planes[planenum].AI.lGroundTargetFlag = 0;
					}
				}

				if(((Planes[planenum].AI.WayPosition.Y - Planes[planenum].AI.TargetPos.Y) < (2000.0f * FTTOWU)) && ((Planes[planenum].AI.Behaviorfunc != AIFlyToPopPoint) && (Planes[planenum].AI.Behaviorfunc != AIFlyToLoftPoint) && (Planes[planenum].AI.OrgBehave != AIFlyToPopPoint) && (Planes[planenum].AI.OrgBehave != AIFlyToLoftPoint)))
				{
					Planes[planenum].AI.WayPosition.Y = Planes[planenum].AI.TargetPos.Y + (2000.0f * FTTOWU);
				}

				break;
			case ACTION_ATTACKTARGET:
				AISetUpAttackTarget(&Planes[planenum], (AttackTargetActionType *)AIActions[actcnt].pAction);
				break;
			case ACTION_RELEASE_CHUTES:
				AISetUpPlayerGroupChuteDrop(&Planes[planenum], bombway);
				break;
		}
	}
}

//**************************************************************************************
void AICInHotMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int placeingroup;
	int planenum;

	if(planepnt == PlayerPlane)
	{
		return;
	}

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	//  Make sound call for basic ack.
//	AIRInHotSnd(planenum, placeingroup);

	if(AICAllowThisRadio(planenum, 1))
	{
		AIRGenericSpeech(AICF_IN_HOT, planenum);

	//	AICGetCallSign(planenum, callsign);
		if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
		{
			return;
		}
	//	if(!LANGGetTransMessage(tempstr, 1024, AIC_IN_HOT, g_iLanguageId, callsign, tempstr2))
		if(!LANGGetTransMessage(tempstr, 1024, AIC_IN_HOT, g_iLanguageId, tempstr2))
		{
			return;
		}
		AICAddAIRadioMsgs(tempstr, 40);
	}
	return;
}

//**************************************************************************************
void AICBombDropMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int placeingroup;
	int planenum;
	int tempnum = rand() & 1;
	int msgnum;
	PlaneParams *leadplane, *playerlead;

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(tempnum == 0)
	{
		msgnum = AIC_BOMB_DROP;
	}
	else
	{
		msgnum = AIC_BOMB_DROP_2;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	//  Make sound call for basic ack.
	AIRBombDropSnd(planenum, placeingroup, tempnum);

//	AICGetCallSign(planenum, callsign);
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	playerlead = AIGetLeader(PlayerPlane);
	leadplane = AIGetLeader(&Planes[planenum]);

	if(leadplane == playerlead)
	{
		if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		AIC_Get_Callsign_With_Number(planenum, tempstr2);
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICBomberBombDropMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int planenum;
	int tempnum = rand() & 1;
	int msgnum, msgsnd;
	int placeingroup;

	planenum = planepnt - Planes;

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

	if(tempnum == 0)
	{
		if(planepnt->AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_BOMBS_GONE;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_BOMBS_GONE;
		}
		msgsnd = AICF_BOMBER_BOMBS_GONE;
	}
	else
	{
		msgnum = AIC_BOMBER_BOMBS_AWAY;
		msgsnd = AICF_BOMBER_BOMBS_AWAY;
	}

	AIC_Get_Callsign_With_Number(planenum, tempstr2);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICBomberAttackMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int planenum;
	int tempnum = rand() & 1;
	int msgnum, msgsnd;
	char playercallsign[256];
	int placeingroup;

#ifndef F15_SPEECH
	return;
#endif

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(tempnum == 0)
	{
		if(planepnt->AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_BREAK_TO_ATTACK;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_BREAK_TO_ATTACK;
		}
		msgsnd = AICF_BOMBER_BREAK_TO_ATTACK;
	}
	else
	{
		if(planepnt->AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_COMMENCE_ATTACK;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_COMMENCE_ATTACK;
		}
		msgsnd = AICF_BOMBER_COMMENCE_ATTACK;
	}

	AIC_Get_Callsign_With_Number(planenum, tempstr2);
	AIC_Get_Callsign_With_Number(PlayerPlane - Planes, playercallsign);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(msgsnd, planenum, PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICMaverickLaunchMsg(PlaneParams *planepnt, int kind)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int placeingroup;
	int planenum;
	int tempnum;
	int msgnum;
	PlaneParams *leadplane, *playerlead;

	planenum = planepnt - Planes;

//	if(planepnt == PlayerPlane)
//	{
//		return;
//	}

	if(!AIRIsAttack(planenum))
	{
		return;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	playerlead = AIGetLeader(PlayerPlane);
	leadplane = AIGetLeader(&Planes[planenum]);

	if(leadplane == playerlead)
	{
		if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		AIC_Get_Callsign_With_Number(planenum, tempstr2);
	}
	//  Make sound call for basic ack.

	tempnum = rand() & 1;

	if(kind == 11)
	{
		msgnum = AIC_WING_MAGNUM;
		tempnum = 11;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_MAVERICK_LAUNCH;
	}
	else
	{
		msgnum = AIC_MAVERICK_LAUNCH_2;
	}

	AIRMaverickLaunchSnd(planenum, placeingroup, tempnum);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
//void AICEndGrndAttackMsg(PlaneParams *planepnt, float heading)
void AICEndGrndAttackMsg(int planenum, int heading)
{
	char tempstr[1024];
	char tempstr2[256];
	char headingtxt[256];
	int placeingroup;
	int headingval;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(planenum == (PlayerPlane - Planes))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

//	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	headingval = AICGetTextHeadingGeneral(headingtxt, heading);
//	AIREndGrndAttackSnd(planenum, placeingroup, headingval);
	AIRGenericSpeech(AICF_END_GRND_ATTACK, planenum, 0, 0, headingval);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_END_GRND_ATTACK, g_iLanguageId, tempstr2, headingtxt))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIRWingmenRadioCall(long soundid, int planenum)
{
	int orgrate = 11025;
#ifndef DONOSOUNDS
	DWORD dwHandle;
	int soundrate;

	if(TimeExcel || iInJump)
	{
		return;
	}

//	soundrate = AIRGetWingSoundRate(planenum, 11025);
	soundrate = orgrate;
 	dwHandle = SndQueueSpeech(soundid, CHANNEL_WINGMEN, 127, -1, soundrate);
	SndServiceSound();
#endif
	return;
}

//**************************************************************************************
int AIRGetWingSoundRate(int planenum, int orgrate)
{
	int worknum;
	int returnnum;

	worknum = planenum & 0x7;

	switch(worknum)
	{
		case 0:
			returnnum = 250;
			break;
		case 1:
			returnnum = -1500;
			break;
		case 2:
			returnnum = 0;
			break;
		case 3:
			returnnum = -400;
			break;
		case 4:
			returnnum = 500;
			break;
		case 5:
			returnnum = -750;
			break;
		case 6:
			returnnum = 750;
			break;
		case 7:
			returnnum = -1000;
			break;
		default:
			returnnum = 0;

	}
	return(orgrate + returnnum);
}

//**************************************************************************************
void AIRSendHeadingRadio(int headingval, int planenum)
{
	switch(headingval)
	{
		case 0:
			AIRWingmenRadioCall(AIR_NORTH, planenum);
			break;
		case 1:
			AIRWingmenRadioCall(AIR_NORTH_EAST, planenum);
			break;
		case 2:
			AIRWingmenRadioCall(AIR_EAST, planenum);
			break;
		case 3:
			AIRWingmenRadioCall(AIR_SOUTH_EAST, planenum);
			break;
		case 4:
			AIRWingmenRadioCall(AIR_SOUTH, planenum);
			break;
		case 5:
			AIRWingmenRadioCall(AIR_SOUTH_WEST, planenum);
			break;
		case 6:
			AIRWingmenRadioCall(AIR_WEST, planenum);
			break;
		case 7:
			AIRWingmenRadioCall(AIR_NORTH_WEST, planenum);
			break;
		default:
			AIRWingmenRadioCall(AIR_NORTH, planenum);
			break;
	}
}

//**************************************************************************************
int AICGetNextSndInt(char *tsptrorg, int *strcnt, int *done, int *spchvar)
{
	int numval;
	char numstr[4];
	char *tsptr;

	if(spchvar != NULL)
	{
		*spchvar = 0;
	}

	*strcnt = 0;
	tsptr = tsptrorg;
	while((*tsptr != 32) && (*tsptr != 0) && (*strcnt < 6))
	{
		if((*tsptr < 48) || (*tsptr > 57))
		{
			if(spchvar == NULL)
			{
				*done = 1;
				if(*strcnt == 0)
				{
					return(-1);
				}
				break;
			}
			else
			{
				if((*tsptr == 'v') || (*tsptr == 'V'))
				{
					*spchvar = 1;
					numstr[*strcnt] = '0';
					tsptr ++;
					*strcnt = *strcnt + 1;
				}
				else if((*tsptr == 'r') || (*tsptr == 'R'))
				{
					*spchvar = 2;
					numstr[*strcnt] = '0';
					tsptr ++;
					*strcnt = *strcnt + 1;
				}
				else
				{
					*done = 1;
					if(*strcnt == 0)
					{
						return(-1);
					}
					break;
				}
			}
		}
		else
		{
			numstr[*strcnt] = *tsptr;
			tsptr ++;
			*strcnt = *strcnt + 1;
		}
	}
	numstr[*strcnt] = 0;

	while(*tsptr == 32)
	{
		tsptr ++;
		*strcnt = *strcnt + 1;
	}
	if(*tsptr == NULL)
	{
		*done = 1;
	}
	numval = atoi(numstr);
	return(numval);
}

//**************************************************************************************
void AIRBasicAckSndOld(int placeingroup)
{
	AIRWingmenRadioCall(AIR_ZERO + placeingroup, placeingroup);
}

//**************************************************************************************
void AIRBasicAckSnd(int planenum, int placeingroup)
{
	int numids;
	int sndids[2];
	int channel;

	numids = 1;

	channel =  AIRGetChannel(planenum);

	sndids[0] = AIRWingNumSndID(planenum, placeingroup) + Planes[planenum].AI.lAIVoice;

	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
long AIRWingNumSndID(int planenum, int placeingroup)
{
	long returnval;
	long channel;

	returnval = (SPCH_ZERO + placeingroup);

	return(returnval);

	channel =  AIRGetChannel(planenum);

#if F15_SPEECH
	if(channel == CHANNEL_WINGMEN)
	{
		returnval = (SPCH_WING_ACK + (placeingroup - 2));
	}
	else
	{
		returnval = (SPCH_BEARING + placeingroup);
	}
#else
	returnval = (SPCH_WING_ACK + placeingroup);
#endif

	return(returnval);
}

//**************************************************************************************
long AIRIntWingNumSndID(int planenum, int placeingroup)
{
	long returnval;
	long channel;

	returnval = (SPCH_ZERO_INTENSE + placeingroup);

	return(returnval);

	channel =  AIRGetChannel(planenum);

	if(channel == CHANNEL_WINGMEN)
	{
		if(placeingroup == 1)
		{
			returnval = (SPCH_BEARING + placeingroup);
		}
		else
		{
			returnval = (SPCH_INT_WING_ACK + (placeingroup - 2));
		}
	}
	else
	{
		returnval = (SPCH_BEARING + placeingroup);
	}

	return(returnval);
}

//**************************************************************************************
void AIRDisengageSnd(int planenum, int placeingroup, int tempnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  Planes[planenum].AI.lAIVoice;  //  AIRIsFighter(planenum);

	if((channel < SPCH_WNG1) || (channel > SPCH_NAVY5))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AICF_DISENGAGE_MSG, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (WING_DISENGAGE + tempnum) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
//  headingval and numingroup do not appear to be used now.   SRE
//**************************************************************************************
void AIREngageSnd(int planenum, int placeingroup, int headingval, int numingroup, int tempnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(channel != CHANNEL_WINGMEN)
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AICF_ENGAGE_MSG, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (WING_ENGAGED_OFFENSIVE + tempnum) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}

	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AIRBombDropSnd(int planenum, int placeingroup, int tempnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(channel != CHANNEL_WINGMEN)
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AICF_DISENGAGE_MSG, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (WING_BOMBS_GONE + tempnum) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AIRMaverickLaunchSnd(int planenum, int placeingroup, int tempnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(!LANGGetTransMessage(tempstr, 1024, AICF_MAVERICK_LAUNCH, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				if(tempnum == 11)
				{
					sndids[numids] = (WING_MAGNUM) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (WING_MAVERICK + tempnum) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

#ifdef F15_SPEECH
//**************************************************************************************
void AICRadarBanditCall(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float range)
{
	int placeingroup;
	char callsign[128];
	char tempstr[1024];
	char placestr[128];
	char groupstr[128];
	char altstr[128];
	char sidestr[128];
	char eheadingstr[128];
	char bearstr[128];
	char rangestr[128];
	int numingroup;
	int bearing360;
	int planenum;
	float rangenm;
	float tbearing;
	int headingval;
	int altval;

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(placestr, 128, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
	numingroup = AICGetIfPlaneGroupText(groupstr, targplane, 1);
//	numingroup = AICGetIfPlaneGroupRadarText(groupstr, targplane, &msgnum, &msgsnd, -1, popup);

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if((targplane->WorldPosition.Y * WUTOFT) > 40000)
	{
		altval = 0;
		if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->WorldPosition.Y * WUTOFT) > 30000)
	{
		altval = 1;
		if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->HeightAboveGround * WUTOFT) < 10000)
	{
		altval = 3;
		if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		altval = 2;
		if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
		{
			return;
		}
	}

	headingval = AICGetTextHeadingGeneral(eheadingstr, AIConvertAngleTo180Degree(targplane->Heading));

	if(targplane->AI.iSide == AI_ENEMY)
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_BANDIT_BANDIT, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_UNKNOWN, g_iLanguageId))
		{
			return;
		}
	}

	rangenm = (range * WUTONM);
	AICGetRangeText(rangestr, rangenm);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_RADAR_CONTACT, g_iLanguageId, placestr, groupstr, bearstr, rangestr, altstr, eheadingstr, sidestr))
	{
		return;
	}

	AIRGenericSpeech(AICF_RADAR_CONTACT, planenum, targplane - Planes, bearing, headingval, 0, rangenm, altval, 0, numingroup);

//	AIRRadarBanditCallSnd(planepnt, targplane, bearing360, rangenm, headingval, numingroup);
	AICAddAIRadioMsgs(tempstr, 40);
}
#else
//**************************************************************************************
void AICRadarBanditCall(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float range)
{
	int placeingroup;
	char callsign[128];
	char tempstr[1024];
	char tempstr2[1024];
	char placestr[128];
	char groupstr[128];
	char altstr[128];
	char taltstr[128];
	char aspectstr[128];
	char sidestr[128];
	char eheadingstr[128];
	char bearstr[128];
	char rangestr[128];
	int numingroup;
	int bearing360;
	int planenum = planepnt - Planes;
	float rangenm;
	float tbearing;
	int headingval;
	int altval, altvalorg;
	int msgnum, msgsnd;
	int headingval2;
	char bcardinal[128];
	int aspectval;
	float trange, dx, dz;
	int digitheading2;
	int popup = 1;
	int talknum = planenum;
	int msgnumb, msgsndb;
	int voice = 0;

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(placestr, 128, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
//	numingroup = AICGetIfPlaneGroupText(groupstr, targplane, 1);
	numingroup = AICGetIfPlaneGroupRadarText(groupstr, targplane, &msgnum, &msgsnd, -1, 1);

	AIRGenericSpeech(msgsnd, planenum, targplane - Planes, 0, 0, 0, 0, 0, 0, numingroup, voice);

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		digitheading2 = tbearing = AIComputeHeadingFromBullseye(targplane->WorldPosition, &trange, &dx, &dz);
		headingval2 = AICGetTextHeadingGeneral(bcardinal, tbearing);
	}

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	aspectval = AICGetAspectText(aspectstr, bearing, targplane);

	altvalorg = (targplane->WorldPosition.Y * WUTOFT);
	altval = (float)altvalorg / 1000.0f;

	sprintf(altstr, "%d", altval);

	if(!LANGGetTransMessage(taltstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
	{
		return;
	}

	if((targplane->WorldPosition.Y * WUTOFT) > 40000)
	{
		altval = 0;
		if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->WorldPosition.Y * WUTOFT) > 30000)
	{
		altval = 1;
		if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->HeightAboveGround * WUTOFT) < 10000)
	{
		altval = 3;
		if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		altval = 2;
		if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
		{
			return;
		}
	}

	headingval = AICGetTextHeadingGeneral(eheadingstr, AIConvertAngleTo180Degree(targplane->Heading));

	if(targplane->AI.iSide == AI_ENEMY)
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_BANDIT_BANDIT, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_UNKNOWN, g_iLanguageId))
		{
			return;
		}
	}

	rangenm = (range * WUTONM);
	if((planepnt) && (planepnt->AI.lAIVoice < 14000))
	{
		if(rangenm > 100)
			rangenm = 100;
	}

	AICGetRangeText(rangestr, rangenm);

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)) && (rangenm > 15))
	{
		rangenm = (trange * WUTONM);

		if((planepnt) && (planepnt->AI.lAIVoice < 14000))
		{
			if(rangenm > 100)
				rangenm = 100;
		}

		AICGetRangeText(rangestr, rangenm);

		if(iUseBullseye == 2)
		{
			msgnumb = AIC_DO_DIGIT_BULLSEYE;
			msgsndb = AICF_DO_DIGIT_BULLSEYE;

			tbearing = -digitheading2;
			headingval2 = digitheading2;
			if(tbearing < 0)
			{
				bearing360 = tbearing + 360;
			}
			else
			{
				bearing360 = tbearing;
			}
			sprintf(bearstr, "%03d", bearing360);

			if(!LANGGetTransMessage(tempstr, 1024, msgnumb, g_iLanguageId, rangestr, bearstr, taltstr, eheadingstr))
			{
				return;
			}
			AIRGenericSpeech(msgsndb, talknum, targplane - Planes, bearing, headingval, headingval2, rangenm, targplane->WorldPosition.Y * WUTOFT, 0, numingroup, voice);
		}
		else
		{
			msgnumb = AIC_DO_BULLSEYE;
			msgsndb = AICF_DO_BULLSEYE;

			if(!LANGGetTransMessage(tempstr, 1024, msgnumb, g_iLanguageId, rangestr, bcardinal, altstr, eheadingstr))
			{
				return;
			}
			AIRGenericSpeech(msgsndb, talknum, targplane - Planes, bearing, headingval, headingval2, rangenm, altval, 0, numingroup, voice);
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_DO_BRA, g_iLanguageId, bearstr, rangestr, taltstr, aspectstr))
		{
			return;
		}

		AIRGenericSpeech(AICF_DO_RADAR_BRA, planenum, targplane - Planes, bearing, headingval, headingval2, rangenm, altvalorg, 0, numingroup);
	}

	if(!LANGGetTransMessage(tempstr2, 1024, msgnum, g_iLanguageId, callsign, tempstr))
	{
		return;
	}

//	AIRRadarBanditCallSnd(planepnt, targplane, bearing360, rangenm, headingval, numingroup);
	AICAddAIRadioMsgs(tempstr2, 40);
}
#endif

//**************************************************************************************
void AICVisualBanditCall(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float range)
{
	int placeingroup;
	char callsign[256];
	char tempstr[1024];
	char placestr[256];
	char altstr[256];
	char lrsidestr[256];
	char ethreatstr[256];
	char clockstr[256];
	char rangestr[256];
	int bearing360;
	int planenum;
	float rangenm;
	float altdiff;
	int highlow;
	float fworkval;
	float rbearing;
	int numingroup;
	int msgnum;


	planenum = planepnt - Planes;

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	numingroup = AICGetIfPlaneGroupText(placestr, targplane, 1);

	if(!LANGGetTransMessage(placestr, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	rbearing = bearing - AIConvertAngleTo180Degree(planepnt->Heading);

	rbearing = AICapAngle(rbearing);

	if(rbearing < 0)
	{
		if(!LANGGetTransMessage(lrsidestr, 256, AIC_RIGHT, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(lrsidestr, 256, AIC_LEFT, g_iLanguageId))
		{
			return;
		}
	}

	if(bearing < 0)
	{
		bearing360 = -bearing;
	}
	else
	{
		bearing360 = -bearing + 360;
	}


	bearing360 = (bearing360 + 20) / 40;
	if(bearing360 < 1)
		bearing360 = 12;

	altdiff = (targplane->WorldPosition.Y - planepnt->WorldPosition.Y) * WUTOFT;
	if(altdiff > 1000)
	{
		highlow = 1;
		if(!LANGGetTransMessage(altstr, 256, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if(altdiff < -1000)
	{
		highlow = -1;
		if(!LANGGetTransMessage(altstr, 256, AIC_LOW, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		highlow = 0;
		altstr[0] = 0;
	}

	if(pDBAircraftList[targplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		if(numingroup > 1)
		{
			msgnum = AIC_WING_CHOPPER_SPOT_2;
		}
		else
		{
			msgnum = AIC_WING_CHOPPER_SPOT_1;
		}
	}
	else
	{
		if(numingroup > 1)
		{
			msgnum = AIC_WING_ENEMY_SPOT_2;
		}
		else
		{
			msgnum = AIC_WING_ENEMY_SPOT_1;
		}
	}

	rangenm = (range * WUTONM);
	AICGetRangeText(rangestr, rangenm);

	AICGetAspectText(ethreatstr, bearing, targplane);

	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(clockstr, planenum, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, clockstr, altstr, ethreatstr))
	{
		return;
	}

//	AIRVisualBanditCallSnd(planepnt, targplane, bearing, rangenm, highlow);

	AIRGenericSpeech(AICF_VISUAL_CONTACT_2, planenum, targplane - Planes, bearing, 0, 0, rangenm, highlow, 0, numingroup);

	AICAddAIRadioMsgs(tempstr, 40);
}



//**************************************************************************************
void AICAllReportContacts()
{
	iAISortNumber = -1;

//	if((RadarInfo.CurMode == SGL_TRT_MODE) || (g_Settings.gp.nType == GP_TYPE_CASUAL))
//	if(((RadarInfo.CurMode != SEARCH_MODE) && (RadarInfo.CurMode != VCTR_MODE) && (RadarInfo.CurMode != VS_MODE)) || (g_Settings.gp.nType == GP_TYPE_CASUAL) || (!((Camera1.CameraMode == CAMERA_COCKPIT) && (Camera1.SubType & COCKPIT_FRONT))))
//	{
		AIC_WSO_Report_Contacts(PlayerPlane - Planes);
//	}
}

//**************************************************************************************
void AICWingReportContacts()
{

	long delaycnt = 2500;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}


	if(planepnt->AI.wingman >= 0)
	{
		AICReportContactsPlane(planepnt->AI.wingman, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICReportContactsPlane(planepnt->AI.nextpair, &delaycnt);
	}
	return;
}

//**************************************************************************************
void AICReportContactsPlane(int planenum, long *delaycnt)
{
	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom))
	{
		return;
	}

	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);

	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(!AICAllowThisRadio(planenum, 1))
		{
			AICAddSoundCall(AICMakeContactCall, planenum, *delaycnt, 50);
			*delaycnt = *delaycnt + 8000;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICReportContactsPlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICReportContactsPlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICMakeContactCall(int planenum, int targetnum)
{
	int placeingroup;
	int placefound;
	PlaneParams *planepnt;
	int foundplane = -1;

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	planepnt = &Planes[planenum];
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	if(Planes[planenum].AI.AirTarget == NULL)
	{
		AICCleanCall(planenum);
	}
	else if(AICheckIfAnyTargets(AIGetLeader(planepnt), AIGetLeader(planepnt->AI.AirTarget), planepnt, &foundplane))
	{
		if((foundplane >= 0) && (!(Planes[planenum].AI.AirTarget->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
		{
			placefound = (Planes[foundplane].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
			if(placeingroup < placefound)
			{
				AICReportContact(planenum, AICPreviousTargetCall(AIGetLeader(planepnt), placeingroup));
			}
			else
			{
				AICBasicAck(planenum);
			}
		}
		else
		{
				AICBasicAck(planenum);
		}
	}
	else
	{
		if(!(Planes[planenum].AI.AirTarget->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT))
		{
			AICReportContact(planenum, AICPreviousTargetCall(AIGetLeader(planepnt), placeingroup));
		}
		else
		{
		 	AICBasicAck(planenum);
		}
	}
}

//**************************************************************************************
void AICCleanCall(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
//	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

	//  Make sound call

#ifdef F15_SPEECH
	if(&Planes[planenum] == PlayerPlane)
	{
		AIRGenericSpeech(AICF_WSO_CLEAN_MSG, planenum);
	}
	else
	{
		AIRGenericSpeech(AICF_CLEAN_MSG, planenum);
	}
#else
	AIRGenericSpeech(AICF_CLEAN_MSG, planenum);
#endif

//	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
	if(!LANGGetTransMessage(tempstr, 1024, AIC_CLEAN_MSG, g_iLanguageId, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICReportContact(int planenum, int isaddition)
{
	AICReportContactPlane(planenum, isaddition, NULL);
}

//**************************************************************************************
void AICReportContactPlane(int planenum, int isaddition, PlaneParams *orgtarget)
{
	PlaneParams *targplane;
	PlaneParams *planepnt;
	float dx, dy, dz, range, offangle;
	int placeingroup;
	char callsign[128];
	char tempstr[1024];
	char placestr[128];
	char groupstr[128];
	char altstr[128];
	char sidestr[128];
	char eheadingstr[128];
	char bearstr[128];
	char rangestr[128];
	int numingroup;
	int bearing360;
	float rangenm;
	float tbearing;
	int altval;
	int headingval;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	planepnt = &Planes[planenum];

	if(orgtarget)
	{
		targplane = orgtarget;
	}
	else
	{
		targplane = planepnt->AI.AirTarget;
	}
	if(targplane == NULL)
		return;

	dx = targplane->WorldPosition.X - planepnt->WorldPosition.X;
	dy = targplane->WorldPosition.Y - planepnt->WorldPosition.Y;
	dz = targplane->WorldPosition.Z - planepnt->WorldPosition.Z;
	range = QuickDistance(dx, dz);

	offangle = atan2(-dx, -dz) * 57.2958;
	offangle = AICapAngle(offangle);

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(placestr, 128, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
	numingroup = AICGetIfPlaneGroupText(groupstr, targplane, 1);

	tbearing = -offangle;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if((planepnt->WorldPosition.Y * WUTOFT) > 40000)
	{
		if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
		{
			return;
		}
		altval = 0;
	}
	else if((planepnt->WorldPosition.Y * WUTOFT) > 30000)
	{
		if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
		altval = 1;
	}
	else if((planepnt->HeightAboveGround * WUTOFT) < 10000)
	{
		if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
		{
			return;
		}
		altval = 3;
	}
	else
	{
		if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
		{
			return;
		}
		altval = 2;
	}

	headingval = AICGetTextHeadingGeneral(eheadingstr, AIConvertAngleTo180Degree(targplane->Heading));

	if(targplane->AI.iSide == AI_ENEMY)
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_BANDIT_BANDIT, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(sidestr, 128, AIC_UNKNOWN, g_iLanguageId))
		{
			return;
		}
	}

	rangenm = (range * WUTONM);
	AICGetRangeText(rangestr, rangenm);

	if(planepnt == PlayerPlane)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_CONTACT_CALL, g_iLanguageId, placestr, groupstr, bearstr, rangestr, altstr, eheadingstr, sidestr))
		{
			return;
		}
//		AIRAddContact(planenum, 1, placeingroup, numingroup, bearing360, rangenm, altval);
		AIRGenericSpeech(AICF_CONTACT_CALL, planenum, targplane - Planes, offangle, headingval, 0, rangenm, altval, 0, numingroup);
	}
	else if(isaddition)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_ADD_CONTACT, g_iLanguageId, placestr, groupstr, bearstr, rangestr, altstr, eheadingstr))
		{
			return;
		}
//		AIRAddContact(planenum, 3, placeingroup, numingroup, bearing360, rangenm, altval);
		AIRGenericSpeech(AICF_ADD_CONTACT, planenum, targplane - Planes, offangle, headingval, 0, rangenm, altval, 0, numingroup);
	}
	else if(placeingroup > 2)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_NL_CONTACT, g_iLanguageId, placestr, groupstr, bearstr, rangestr, altstr, eheadingstr))
		{
			return;
		}
//		AIRAddContact(planenum, 2, placeingroup, numingroup, bearing360, rangenm, altval);
		AIRGenericSpeech(AICF_NL_CONTACT, planenum, targplane - Planes, offangle, headingval, 0, rangenm, altval, 0, numingroup);
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_CONTACT_CALL, g_iLanguageId, placestr, groupstr, bearstr, rangestr, altstr, eheadingstr, sidestr))
		{
			return;
		}
//		AIRAddContact(planenum, 1, placeingroup, numingroup, bearing360, rangenm, altval);
		AIRGenericSpeech(AICF_CONTACT_CALL, planenum, targplane - Planes, offangle, headingval, 0, rangenm, altval, 0, numingroup);
	}
	AICAddAIRadioMsgs(tempstr, 40);
}

//**************************************************************************************
int AICPreviousTargetCall(PlaneParams *planepnt, int stopplace)
{
	int returnval = 0;
	int placeingroup;
	PlaneParams *checkplane;

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	if(placeingroup >= stopplace)
		return(0);

	if(planepnt == PlayerPlane)
	{
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if(checkplane->AI.iAIFlags2 & (AI_REPORTED_AS_CONTACT))
			{
				return(1);
			}
		}
	}
	else if(planepnt->AI.AirTarget != NULL)
	{
		return(1);
	}

	if(planepnt->AI.wingman >= 0)
	{
		if(AICPreviousTargetCall(&Planes[planepnt->AI.wingman], stopplace))
		{
			return(1);
		}
	}

	if(planepnt->AI.nextpair >= 0)
	{
		return(AICPreviousTargetCall(&Planes[planepnt->AI.nextpair], stopplace));
	}
	return(0);
}

//**************************************************************************************
void AICGoingHotMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	int planenum;

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(AICF_GOING_HOT_MSG, planenum);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_GOING_HOT_MSG, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICGoingColdMsgPN(int planenum, int targetnum)
{
	AICGoingColdMsg(&Planes[planenum]);
}

//**************************************************************************************
void AICGoingColdMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];
	int placeingroup;
	int planenum;

	planenum = planepnt - Planes;

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(AICF_GOING_COLD_MSG, planenum);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_GOING_COLD_MSG, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

#if 0
//**************************************************************************************
void AICTurnBackMsg(PlaneParams *planepnt)
{
	char tempstr[1024];

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TURN_BACK, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	return;
}
#endif

//**************************************************************************************
int AIGetPlayerCoverType()
{
	MBWayPoints *lookway;
	int bombfound = 0;
	int startact, endact, actcnt;
	PlaneParams *planepnt;

#ifdef NO_AI_PLAYER
 	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return(NULL);
#endif

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	lookway = planepnt->AI.CurrWay;

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_CAP))
		{
			return(3);
		}
	}

#if 1
	if(AICheckHumanCover(planepnt) != -1)
	{
		return(2);
	}
#else
	int waycnt;
	int numwpts;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				return(2);
			}
		}
		lookway ++;
	}
#endif
	return(1);
}

//**************************************************************************************
void AIDCover()
{
	int tempvar;
	PlaneParams *planepnt;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	tempvar = AIGetPlayerCoverType();

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(tempvar == 3)
	{
		AISetUpCAPPlayer(planepnt, firstvalid, lastvalid);
	}
	else if(tempvar == 2)
	{
		AICSetUpEscortPlayer(planepnt, firstvalid, lastvalid, AICheckHumanCover(planepnt));
	}
	else
	{
		AICSetUpEscortPlayer(planepnt, firstvalid, lastvalid, planepnt - Planes);
	}
}

//**************************************************************************************
void AIECover()
{
	int tempvar;
	PlaneParams *planepnt;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	tempvar = AIGetPlayerCoverType();
	if(tempvar == 3)
	{
		AISetUpCAPPlayer(planepnt, firstvalid, lastvalid);
	}
	else if(tempvar == 2)
	{
		AICSetUpEscortPlayer(planepnt, firstvalid, lastvalid, AICheckHumanCover(planepnt));
	}
	else
	{
		AICSetUpEscortPlayer(planepnt, firstvalid, lastvalid, planepnt - Planes);
	}
}

//**************************************************************************************
void AIWCover()
{
	int tempvar;
	PlaneParams *planepnt;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}


	tempvar = AIGetPlayerCoverType();
	if(tempvar == 3)
	{
		AISetUpCAPPlayer(planepnt, firstvalid, lastvalid);
	}
	else if(tempvar == 2)
	{
		AICSetUpEscortPlayer(planepnt, firstvalid, lastvalid, AICheckHumanCover(planepnt));
	}
	else
	{
		AICSetUpEscortPlayer(planepnt, firstvalid, lastvalid, planepnt - Planes);
	}
}

//**************************************************************************************
void AIDOrbit()
{
	PlaneParams *planepnt;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	AISetUpOrbitPlayer(planepnt, firstvalid, lastvalid);
}

//**************************************************************************************
void AIEOrbit()
{
	PlaneParams *planepnt;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	AISetUpOrbitPlayer(planepnt, firstvalid, lastvalid);
}

//**************************************************************************************
void AIWOrbit()
{
	PlaneParams *planepnt;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	AISetUpOrbitPlayer(planepnt, firstvalid, lastvalid);
}

//**************************************************************************************
void AISetUpOrbitPlayer(PlaneParams *orgplanepnt, int firstvalid, int lastvalid)
{
	PlaneParams *planepnt;
	PlaneParams *wingplane;
	int done = 0;
	MBWayPoints *waypnt;
	int placeingroup;
	long delaycnt = 1000;
	float radft, desiredspeed;
	float degspersec, fworkvar;

#ifdef NO_AI_PLAYER
 	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	radft = 2.5 * NMTOFT;

	planepnt = orgplanepnt;

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		desiredspeed = 80;
	}
	else
	{
		desiredspeed = 350;
	}

	waypnt = planepnt->AI.CurrWay;

	while(!done)
	{
		placeingroup = (planepnt->AI.iAIFlags1  & AIFLIGHTNUMS);

		if((((placeingroup >= firstvalid) && (placeingroup <= lastvalid))
			|| ((planepnt == PlayerPlane) && (firstvalid <= 1))) && ((!(planepnt->AI.iAIFlags2 & AILANDING)) || (planepnt->AI.lTimer2 > 0)) && (!planepnt->OnGround) && (AIInPlayerGroup(planepnt)))
		{
#if 0
			if(planepnt->AI.OrgBehave == NULL)
			{
				planepnt->AI.Behaviorfunc = AIFlyToOrbitPoint;
			}
			else
			{
				planepnt->AI.OrgBehave = AIFlyToOrbitPoint;
			}
#else
			planepnt->AI.Behaviorfunc = AIFlyToOrbitPoint;
			planepnt->AI.OrgBehave = NULL;
#endif


			degspersec = AIGetTurnRateForOrbit(radft, desiredspeed, orgplanepnt->Altitude);
			if(degspersec > planepnt->YawRate)
			{
				degspersec = planepnt->YawRate;
			}
			fworkvar = asin(degspersec / planepnt->YawRate);
			fworkvar = RadToDeg(fworkvar);
			planepnt->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
			if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
			{
				planepnt->AI.lDesiredSpeed = 80;
			}
			else
			{
				planepnt->AI.lDesiredSpeed = 350;
			}
			planepnt->AI.lTimer1 = 2000;
			if(placeingroup > 1)
			{
				AICAddSoundCall(AICWingOrbitHereMsg, (int)(planepnt - Planes), delaycnt, 50);
				planepnt->AI.WayPosition = orgplanepnt->WorldPosition;
				delaycnt = delaycnt + 2000;
				planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
				planepnt->AI.iVar1 = 0;
				planepnt->AI.lVar2 = 2.5 * NMTOFT;
			}
			if(planepnt->AI.wingman >= 0)
			{
				wingplane = &Planes[planepnt->AI.wingman];
				wingplane->AI.WayPosition = orgplanepnt->WorldPosition;
				wingplane->AI.iAIFlags1 |= AINOFORMUPDATE;
				degspersec = AIGetTurnRateForOrbit(radft, desiredspeed, orgplanepnt->Altitude);
				if(degspersec > wingplane->YawRate)
				{
					degspersec = wingplane->YawRate;
				}
				fworkvar = asin(degspersec / wingplane->YawRate);
				fworkvar = RadToDeg(fworkvar);
				wingplane->AI.DesiredRoll = AIConvert180DegreeToAngle(fworkvar);
#if 0
				if(wingplane->AI.OrgBehave == NULL)
				{
					wingplane->AI.Behaviorfunc = AIFlyOrbit;
				}
				else
				{
					wingplane->AI.OrgBehave = AIFlyOrbit;
				}
#else
				wingplane->AI.Behaviorfunc = AIFlyOrbit;
				wingplane->AI.OrgBehave = NULL;
#endif

				wingplane->AI.iVar1 = 0;
				wingplane->AI.lVar2 = 2.5 * NMTOFT;
				if(pDBAircraftList[wingplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
				{
					wingplane->AI.lDesiredSpeed = 80;
				}
				else
				{
					wingplane->AI.lDesiredSpeed = 350;
				}
				wingplane->AI.lTimer1 = 2000;
				AICAddSoundCall(AICWingOrbitHereMsg, (int)(wingplane - Planes), delaycnt, 50);
				delaycnt = delaycnt + 2000;
			}
		}

		if(planepnt->AI.nextpair >= 0)
		{
			planepnt = &Planes[planepnt->AI.nextpair];
		}
		else
		{
			done = 1;
		}
	}
	return;
}

//**************************************************************************************
void AICWBracketRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICEBracketRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICDBracketRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 1);
	return;
}

//**************************************************************************************
void AICWBracketLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICEBracketLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICDBracketLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 2);
	return;
}

//**************************************************************************************
void AICWSplitHigh()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICESplitHigh()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICDSplitHigh()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 3);
	return;
}

//**************************************************************************************
void AICWSplitLow()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICESplitLow()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICDSplitLow()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 4);
	return;
}

//**************************************************************************************
void AICWDragRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICEDragRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICDDragRight()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 5);
	return;
}

//**************************************************************************************
void AICWDragLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICEDragLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICDDragLeft()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICSetBracket(firstvalid, lastvalid, 6);
	return;
}

//**************************************************************************************
void AICSetBracket(int firstvalid, int lastvalid, int whichway)
{
	long delaycnt = 1000;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif
	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICSetBracketPlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid, whichway);
	}

	if(PlayerPlane->AI.nextpair >= 0)
	{
		AICSetBracketPlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid, whichway);
	}
	return;
}


//**************************************************************************************
void AICSetBracketPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int whichway)
{
	int placeingroup;
	PlaneParams *planepnt;
	PlaneParams *target;
	PlaneParams *wingplane;
	PlaneParams *leadplane;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	planepnt = &Planes[planenum];
	leadplane = AIGetLeader(planepnt);

	void (*Behaviorfunc)(PlaneParams *planepnt); //  This is a pointer to the AI function the plane is currently using.

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid) && ((!(Planes[planenum].AI.iAIFlags2 & AILANDING)) || (Planes[planenum].AI.lTimer2 > 0)) && (!Planes[planenum].OnGround))
	{
		target = planepnt->AI.AirTarget;

		if(target != NULL)
		{
			if(target->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
			{
				target = planepnt->AI.AirTarget = NULL;
				if(!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR))
				{
					planepnt->AI.lCombatTimer = -1;
					planepnt->AI.CombatBehavior = NULL;
				}
			}
		}

		if(target == NULL)
		{
			wingplane = NULL;
			if(planepnt->AI.wingman >= 0)
			{
				wingplane = &Planes[planepnt->AI.wingman];
			}
			else if(planepnt->AI.winglead >= 0)
			{
				wingplane = &Planes[planepnt->AI.winglead];
			}

			if(wingplane != NULL)
			{
				if(wingplane->AI.AirTarget != NULL)
				{
					wingplane->AI.lRadarDelay = -1;
					AISimpleRadar(wingplane);

					if((planepnt->AI.AirTarget != wingplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						planepnt->AI.lCombatTimer = -1;
						planepnt->AI.CombatBehavior = NULL;
					}

					target = GeneralSetNewAirTarget(planepnt, wingplane->AI.AirTarget);
				}
				else if(!(leadplane->Status & PL_AI_DRIVEN))
				{
					leadplane->AI.lRadarDelay = -1;
					AISimpleRadar(leadplane);

					if((planepnt->AI.AirTarget != leadplane->AI.AirTarget) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
					{
						planepnt->AI.lCombatTimer = -1;
						planepnt->AI.CombatBehavior = NULL;
					}

					target = GeneralSetNewAirTarget(planepnt, leadplane->AI.AirTarget);
				}
			}
		}

		if(target != NULL)
		{
			Behaviorfunc = AIBracketTargetRight;
			switch(whichway)
			{
				case 1:
					Behaviorfunc = AIBracketTargetRight;
					break;
				case 2:
					Behaviorfunc = AIBracketTargetLeft;
					break;
				case 3:
					Behaviorfunc = AISplitHighTarget;
					break;
				case 4:
					Behaviorfunc = AISplitLowTarget;
					break;
				case 5:
					Behaviorfunc = AIDragTargetRight;
					break;
				case 6:
					Behaviorfunc = AIDragTargetLeft;
					break;

			}
			if(planepnt->AI.OrgBehave == NULL)
			{
				planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
				planepnt->AI.Behaviorfunc = Behaviorfunc;
			}
			else
			{
				planepnt->AI.Behaviorfunc = Behaviorfunc;
			}
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICBasicAck, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 2000;
			}
			planepnt->AI.iAIFlags1 |= AINOFORMUPDATE;
		}
		else
		{
			Planes[planenum].AI.Behaviorfunc = AIFlyFormation;
			Planes[planenum].AI.OrgBehave = NULL;
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AICNoJoyMsg, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 5000;
			}
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICSetBracketPlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid, whichway);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICSetBracketPlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid, whichway);
	}
	return;
}

//**************************************************************************************
void AICCheckAIKeyUp(int keyflag, WPARAM wParam)
{
	if((keyflag == 0) && (wParam == VK_SHIFT))
	{
		iAIShiftDown = 0;
	}
}

//**************************************************************************************
void AICShowOtherCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int awacs = 0;
	int jstars = 0;
	RunwayInfo *tower;
	int escorts = 0;
	int temp1, numsead;
	int humanescort;
	PlaneParams *linkedplane;
	float tdistnm;
	int planenum;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}


	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	if(AIGetClosestAWACS(PlayerPlane) >= 0)
	{
		awacs = 1;
	}
	else
	{
		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			MovingVehicleParams *carrier;
			carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];

			if(!((carrier->Status & VL_DESTROYED) || (!(carrier->Status & VL_ACTIVE))))
			{
				awacs = 1;
			}
		}
	}

	if((awacs) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(AIGetClosestJSTARS(PlayerPlane) >= 0)
	{
		jstars = 1;
	}

	if((jstars) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

#if 0
	if(AIGetClosestJSTARS(PlayerPlane) >= 0)
	{
		jstars = 1;
	}

	if((jstars) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;


	if(lBombFlags & WSO_JSTARS_CHECK)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_CHECK_OUT, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_CHECK_IN, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_TARGET_REQUEST, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_NEW_TARGET, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
#else
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_JSTARS_MENU, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
#endif

	if((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)) && (PlayerPlane->OnGround == 0))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TANKER_REQUEST, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	tower = (RunwayInfo *)AIGetClosestAirField(PlayerPlane, PlayerPlane->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, (20.0f * NMTOWU), 0);
//	if((!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)) && (tower) && (!((PlayerPlane->OnGround) || ((PlayerPlane->HeightAboveGround * WUTOFT) < 10.0f))))  //  will need to make a function to check for nearby airport.  SRE
	if((!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)) && ((tower) || (!(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_AIRPORT))))  //  will need to make a function to check for nearby airport.  SRE
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_TOWER, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = textcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_REQUEST_SAR, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	usecolor = invalidcolor;
	if((lBombFlags & WSO_FAC_CHECK) || (AICCheckFACClose(PlayerPlane)))
	{
		usecolor = textcolor;
	}

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_FAC_MENU, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	escorts = AIGetNumEscorting(PlayerPlane, PlayerPlane, &temp1, &numsead);

	escorts += numsead;

	humanescort = 0;

	linkedplane = NULL;

	planenum = AIGetHumanEscortPlane(PlayerPlane - Planes);
	if(planenum >= 0)
		linkedplane = &Planes[planenum];

	if(linkedplane)
	{
		if(linkedplane->AI.iAIFlags1 & AI_SEEN_PLAYER)
		{
		 	humanescort = 1;
		}
		else
		{
			tdistnm = (linkedplane->WorldPosition - PlayerPlane->WorldPosition) * WUTONM;
			if(tdistnm < (AI_VISUAL_RANGE_NM * 2.0f))
			{
				humanescort = 1;
			}
		}
		if((linkedplane->OnGround) || (PlayerPlane->OnGround))
			humanescort = 0;
	}

	if(((escorts) || (humanescort) || (AIRejoinableEscorts(PlayerPlane, AIGetLeader(PlayerPlane)))) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
		usecolor = textcolor;
	else
		usecolor = invalidcolor;
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_ESCORTS, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
void AIC_AWACS_CommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int usecolor;
	int fontheight;
	int theight = 0;
	int width;
	int awacs = 0;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	if(AIGetClosestAWACS(PlayerPlane) >= 0)
	{
		awacs = 1;
	}
	else
	{
		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			MovingVehicleParams *carrier;
			carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];

			if(!((carrier->Status & VL_DESTROYED) || (!(carrier->Status & VL_ACTIVE))))
			{
				awacs = 1;
			}
		}
	}

	if(awacs)
		usecolor = textcolor;
	else
		usecolor = invalidcolor;

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_REQUEST_PICTURE, 1, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_REQUEST_BOGIE_DOPE, 2, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_REQUEST_ASSIST, 3, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_REQUEST_WEASELS, 4, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

//	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_REQUEST_TANKER, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_ALPHA_CHECKS, 5, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);

	if(lBombFlags & WSO_AWACS_CHECK)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_RELIEF, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_ON_STATION, 6, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}

	if(lBombFlags & WSO_AWACS_TACTICAL)
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_BULLSEYE, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}
	else
	{
		theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_TACTICAL, 7, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
	}

	if((PlayerPlane->AADesignate) && (awacs))
	{
		usecolor = textcolor;
	}
	else
	{
		usecolor = invalidcolor;
	}
	theight = AICDisplayAnAICommMenuOption(GrBuffFor3D, AIC_AWACS_DECLARE, 8, fontheight, boxcolor, usecolor, MessageFont, 0, theight, width);
}

//**************************************************************************************
PlaneParams *AICheckTankerCloseBy(PlaneParams *planepnt, int findclosest)
{
	PlaneParams *checkplane;
	PlaneParams *returnplane = NULL;
	float dx, dy, dz, tdist, tdistnm;
	float founddist = 50.0f;

	if(findclosest < 0)
	{
		founddist = -1.0f;
	}

	checkplane = &Planes[0];

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_OUT_OF_CONTROL))) && (planepnt->AI.iSide == checkplane->AI.iSide)
				&& ((checkplane->AI.Behaviorfunc == AITankingFlight2AI) || (checkplane->AI.Behaviorfunc == AITankingFlight3AI)
				|| (checkplane->AI.Behaviorfunc == AIFlyTanker) || (checkplane->AI.Behaviorfunc == AIFlyToTankerPoint) || (checkplane->AI.Behaviorfunc == AITankingFlight) || (checkplane->AI.Behaviorfunc == AITankingFlight2) || (checkplane->AI.Behaviorfunc == AITankingFlight3) || (checkplane->AI.Behaviorfunc == AITankingFlight4) || (checkplane->AI.Behaviorfunc == AITankingFlight5)
				|| (checkplane->AI.Behaviorfunc == AITankingFlight2HumanLong) || (checkplane->AI.Behaviorfunc == AITankingFlight3HumanLong) || (checkplane->AI.Behaviorfunc == AITankingBreakaway)))
		{
			dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
			tdist = QuickDistance(dx, dz);
			tdistnm = (tdist * WUTONM);
			if((tdistnm < 15.0) || (findclosest == -1))
			{
				if(findclosest == 0)
				{
					return(checkplane);
				}
				else
				{
					if((founddist > tdistnm) || (founddist < 0))
					{
						returnplane = checkplane;
						founddist = tdistnm;
					}
				}
			}
		}
		checkplane ++;
	}
	return(returnplane);
}

//**************************************************************************************
int AICPixelLenOtherCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_AWACS, 1, MessageFont);

#if 0
	if(lBombFlags & WSO_JSTARS_CHECK)
	{
		worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_CHECK_OUT, 2, MessageFont);
	}
	else
	{
		worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_CHECK_IN, 2, MessageFont);
	}

	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_TARGET_REQUEST, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_NEW_TARGET, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
#else
	worklen = AICGetMaxMenuPixelLen(AIC_JSTARS_MENU, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
#endif

	worklen = AICGetMaxMenuPixelLen(AIC_TANKER_REQUEST, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_TOWER, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_REQUEST_SAR, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_FAC_MENU, 6, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_ESCORTS, 7, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
int AICPixelLenAWACSCommOps()
{
	int messagelen = 0;
	int worklen;

	messagelen = AICGetMaxMenuPixelLen(AIC_AWACS_REQUEST_PICTURE, 1, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_AWACS_REQUEST_BOGIE_DOPE, 2, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_AWACS_REQUEST_WEASELS, 3, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}
	worklen = AICGetMaxMenuPixelLen(AIC_AWACS_REQUEST_ASSIST, 4, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

//	worklen = AICGetMaxMenuPixelLen(AIC_AWACS_REQUEST_TANKER, 5, MessageFont);
	worklen = AICGetMaxMenuPixelLen(AIC_AWACS_ALPHA_CHECKS, 5, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	if(lBombFlags & WSO_AWACS_CHECK)
	{
		worklen = AICGetMaxMenuPixelLen(AIC_AWACS_RELIEF, 6, MessageFont);
	}
	else
	{
		worklen = AICGetMaxMenuPixelLen(AIC_AWACS_ON_STATION, 6, MessageFont);
	}
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}


	if(lBombFlags & WSO_AWACS_TACTICAL)
	{
		worklen = AICGetMaxMenuPixelLen(AIC_AWACS_BULLSEYE, 7, MessageFont);
	}
	else
	{
		worklen = AICGetMaxMenuPixelLen(AIC_AWACS_TACTICAL, 7, MessageFont);
	}

	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	worklen = AICGetMaxMenuPixelLen(AIC_AWACS_DECLARE, 8, MessageFont);
	if(worklen > messagelen)
	{
		messagelen = worklen;
	}

	return(messagelen);
}

//**************************************************************************************
void AICOtherCommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;
	RunwayInfo *tower;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;
	int escorts = 0;
	int temp1, numsead;
	int humanescort;
	PlaneParams *linkedplane;
	float tdistnm;
	int planenum;

	switch(wParam)
	{
		case '1':
			menuedge = 10;
			if((AIGetClosestAWACS(PlayerPlane) >= 0) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
			{
				gAICommMenu.AICommMenufunc = AIC_AWACS_CommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenAWACSCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AIC_AWACS_CommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			else
			{
				if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
				{
					MovingVehicleParams *carrier;
					carrier = &MovingVehicles[PlayerPlane->AI.iHomeBaseId];

					if(!((carrier->Status & VL_DESTROYED) || (!(carrier->Status & VL_ACTIVE))))
					{
						gAICommMenu.AICommMenufunc = AIC_AWACS_CommOps;
						gAICommMenu.lMaxStringPixelLen = AICPixelLenAWACSCommOps() + menuedge;
						gAICommMenu.AICommKeyfunc = AIC_AWACS_CommKeyOps;
						gAICommMenu.lTimer = AICOMMDISPLAY;
					}
					else
					{
						gAICommMenu.AICommMenufunc = NULL;
						gAICommMenu.AICommKeyfunc = NULL;
						gAICommMenu.lTimer = -1;
					}
				}
				else
				{
					gAICommMenu.AICommMenufunc = NULL;
					gAICommMenu.AICommKeyfunc = NULL;
					gAICommMenu.lTimer = -1;
				}
			}
			break;
#if 0
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if((lBombFlags & WSO_JSTARS_CHECK) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
			{
				AICPlayerJSTARSCheckOut();
			}
			else
			{
				AICPlayerJSTARSCheckIn();
			}
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				AIC_JSTARS_Give_Target_Loc();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
				AIC_JSTARS_Give_Next_Target_Loc();
			break;
#else
		case '2':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowJSTARSCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenJSTARSCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICJSTARSCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
			break;
#endif
		case '3':
			if(!((AICheckTankerCloseBy(PlayerPlane) != NULL) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))) && (PlayerPlane->OnGround == 0))
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
			}
			else
			{
//				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
//					AICPlayerContactRefueler();
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowTankerCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenTankerCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICTankerCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			break;
		case '4':
			tower = (RunwayInfo *)AIGetClosestAirField(PlayerPlane, PlayerPlane->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, (20.0f * NMTOWU), 0);
//			if(!((!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)) && (tower) && (!((PlayerPlane->OnGround) || ((PlayerPlane->HeightAboveGround * WUTOFT) < 10.0f)))))  //  will need to make a function to check for nearby airport.  SRE
			if(!((!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)) && ((tower) || (!(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_AIRPORT)))))  //  will need to make a function to check for nearby airport.  SRE
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
			}
			else
			{
//				if(!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED))
//					AICPlayerRequestLanding();
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowTowerCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenTowerCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICTowerCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			break;
		case '5':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICPlayerRequestSAR();
			// request SAR
			break;
		case '6':
			menuedge = 10;
			if((lBombFlags & WSO_FAC_CHECK) || (AICCheckFACClose(PlayerPlane)))
			{
				gAICommMenu.AICommMenufunc = AICShowFACCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenFACCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICFACCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			else
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
			}
			break;
		case '7':
			escorts = AIGetNumEscorting(PlayerPlane, PlayerPlane, &temp1, &numsead);

			escorts += numsead;

			menuedge = 10;
			humanescort = 0;

			linkedplane = NULL;

			planenum = AIGetHumanEscortPlane(PlayerPlane - Planes);
			if(planenum >= 0)
				linkedplane = &Planes[planenum];

			if(linkedplane)
			{
				if(linkedplane->AI.iAIFlags1 & AI_SEEN_PLAYER)
				{
		 			humanescort = 1;
				}
				else
				{
					tdistnm = (linkedplane->WorldPosition - PlayerPlane->WorldPosition) * WUTONM;
					if(tdistnm < (AI_VISUAL_RANGE_NM * 2.0f))
					{
						humanescort = 1;
					}
				}
				if((linkedplane->OnGround) || (PlayerPlane->OnGround))
					humanescort = 0;
			}

			if(((escorts) || (humanescort) || (AIRejoinableEscorts(PlayerPlane, AIGetLeader(PlayerPlane)))) && (!(PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)))
			{
//				gAICommMenu.AICommMenufunc = AICShowEscortReleaseCommOps;
//				gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortReleaseCommOps() + menuedge;
//				gAICommMenu.AICommKeyfunc = AICEscortReleaseCommKeyOps;
//				gAICommMenu.lTimer = AICOMMDISPLAY;
				menuedge = 10;
				gAICommMenu.AICommMenufunc = AICShowEscortCommOps;
				gAICommMenu.lMaxStringPixelLen = AICPixelLenEscortCommOps() + menuedge;
				gAICommMenu.AICommKeyfunc = AICEscortCommKeyOps;
				gAICommMenu.lTimer = AICOMMDISPLAY;
			}
			else
			{
				gAICommMenu.AICommMenufunc = NULL;
				gAICommMenu.AICommKeyfunc = NULL;
				gAICommMenu.lTimer = -1;
			}
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AIC_AWACS_CommKeyOps(int keyflag, WPARAM wParam)
{
	int menuedge;

	switch(wParam)
	{
		case '1':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICRequestPlayerPicture();
			break;
		case '2':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICRequestPlayerBogieDope();
			break;
		case '3':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICRequestPlayerCover();
			break;
		case '4':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			AICRequestPlayerSEAD();
			break;
		case '5':
			menuedge = 10;
			gAICommMenu.AICommMenufunc = AICShowAlphaCheckCommOps;
			gAICommMenu.lMaxStringPixelLen = AICPixelLenAlphaCheckCommOps() + menuedge;
			gAICommMenu.AICommKeyfunc = AICAlphaCheckCommKeyOps;
			gAICommMenu.lTimer = AICOMMDISPLAY;
//			gAICommMenu.AICommMenufunc = NULL;
//			gAICommMenu.AICommKeyfunc = NULL;
//			gAICommMenu.lTimer = -1;
//			AICRequestClosestTanker();
			break;
		case '6':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			// Eventually Move this to function
			AIC_AWACS_CheckInOut();
			break;
		case '7':
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			// Eventually Move this to function
			if(lBombFlags & WSO_AWACS_TACTICAL)
			{
				AIC_AWACSGoBullseye(-1, -1);
			}
			else
			{
				AIC_AWACSGoTactical(-1, -1);
			}
			break;
		case '8':
			if((iAICommFrom < 0) && (PlayerPlane->AADesignate))
			{
				AIC_Ask_Declare(PlayerPlane - Planes, 1);
			}
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
		default:
			gAICommMenu.AICommMenufunc = NULL;
			gAICommMenu.AICommKeyfunc = NULL;
			gAICommMenu.lTimer = -1;
			break;
	}
}

//**************************************************************************************
void AICPlayerContactRefueler()
{
	if((iAICommFrom == -1) || (!MultiPlayer))
	{
		AICContactRefueler(PlayerPlane);
	}
	else
	{
		AICContactRefueler(&Planes[iAICommFrom]);
	}
}

//**************************************************************************************
void AICPlayerContactRefuelerMaxTrap()
{
	if((iAICommFrom == -1) || (!MultiPlayer))
	{
		AICContactRefueler(PlayerPlane, 1);
	}
	else
	{
		AICContactRefueler(&Planes[iAICommFrom], 1);
	}
}

//**************************************************************************************
void AICPlayerContactRefueler4K()
{
	if((iAICommFrom == -1) || (!MultiPlayer))
	{
		AICContactRefueler(PlayerPlane, 4);
	}
	else
	{
		AICContactRefueler(&Planes[iAICommFrom], 4);
	}
}

//**************************************************************************************
void AICContactRefueler(PlaneParams *planepnt, int fuelrequested)
{
	PlaneParams *tanker;
	long delaycnt = 1000;
	long tempheading;
	PlaneParams *checkplane;

	tanker = AICheckTankerCloseBy(planepnt);  // PlayerPlane

	if(tanker == NULL)
		return;

	iAIHumanTankerFlags = 0;

	if(AICAllowThisRadio(planepnt - Planes, 1))
	{
//		if(planepnt == PlayerPlane)  //  Playerized for demo SRE
//		{
			AICAddSoundCall(AICTankerContact1, planepnt - Planes, delaycnt, 50, fuelrequested);
			delaycnt = delaycnt + 5000;

			AICAddSoundCall(AICTankerHeading, planepnt - Planes, delaycnt, 50);

			delaycnt = delaycnt + 25000;

			AICAddSoundCall(AICTankerContact2, tanker - Planes, delaycnt, 50);
//		}
	}

	tanker->AI.DesiredHeading = planepnt->Heading;

	tempheading = AIConvertAngleTo180Degree(tanker->AI.DesiredHeading);
	tempheading += 5;

	tempheading = AICapAngle(tempheading);

	tempheading /= 10;

	tempheading *= 10;

	tanker->AI.DesiredHeading = AIConvert180DegreeToAngle(tempheading);

	tanker->AI.Behaviorfunc = AITankingFlight;
	tanker->AI.LinkedPlane = planepnt;
	tanker->AI.lVar2 = (5.0f * NMTOFT) + 200;  //  + 200 kludge to handle loss of precision.

	tanker->AI.lTimer2 = -1;
	tanker->AI.TargetPos.Y = -1.0;
	planepnt->AI.lTimer2 = -1;

	planepnt->AI.Behaviorfunc = AIFlyTankerFormation;

	tanker->AI.cUseWeapon = fuelrequested;

	tanker->AI.fVarA[0]	= CTGetTankingAmount(planepnt, fuelrequested);

	tanker->AI.fVarA[1] = GetRegValueL("refuel_rate");
	if(tanker->AI.fVarA[1] < 1000.0f)
	{
		tanker->AI.fVarA[1] = 1500.0f;
	}

	planepnt->AI.lVar3 = 0x4;

	checkplane = planepnt;
	while(checkplane)
	{
		if(checkplane->AI.wingman >= 0)
		{
			Planes[checkplane->AI.wingman].AI.lVar3 = 0x2;
		}
		if(checkplane->AI.nextpair >= 0)
		{
			checkplane = &Planes[checkplane->AI.nextpair];
			checkplane->AI.lVar3 = 0x2;
		}
		else
		{
			checkplane = NULL;
		}
	}

	if(MultiPlayer)
	{
		if((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))
		{
	 		NetPutGenericMessage3(planepnt, GM3_CONTACT_REFUELER, (tanker - Planes), tanker->AI.cUseWeapon);
			NetPutDesiredHeading(tanker - Planes, tanker->AI.DesiredHeading);

//			if(!(tanker->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
//			{
				tanker->AI.iMultiPlayerAIController = PlayerPlane - Planes;
				tanker->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
				NetPutGenericMessage2(planepnt, GM2_TRANSFER_CONTROL, (tanker - Planes));
//			}
		}
	}

//	AIChangeGroupBehavior(planepnt, AIFlyFormation, AIFlyTankerFormation);
}

//**************************************************************************************
void AICTankerContact1(int planenum, int targetnum)
{
	char tempstr[1024];
//	char tempstr2[256];
	char tankcallsign[256];
	char callsign[256];
	char kfuel[256];
	PlaneParams *tanker;
	int tankamount;
	float fworkvar;
//	int cnt;
//	int id;
//	int bombtype;
//	int weight;
//	float fueladd;
	PlaneParams *planepnt = &Planes[planenum];


	//  Make sound call

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tanker = AICheckTankerCloseBy(&Planes[planenum]);

	if(tanker == NULL)
	{
		return;
	}

	fworkvar = CTGetTankingAmount(planepnt, targetnum);

	fworkvar += 500;
	fworkvar /= 1000;

	if(fworkvar < 0)
		fworkvar = 0;

	tankamount = fworkvar;

	if(MultiPlayer)
	{
		planepnt->AI.lVar3 &= 0x1;
		planepnt->AI.lVar3 |= 0x4;

		if((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))
		{
	 		NetPutGenericMessage3(planepnt, GM3_CONTACT_REFUELER, (tanker - Planes), tanker->AI.cUseWeapon);
			NetPutDesiredHeading(tanker - Planes, tanker->AI.DesiredHeading);
//			if(!(tanker->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
//			{
				tanker->AI.iMultiPlayerAIController = PlayerPlane - Planes;
				tanker->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
				NetPutGenericMessage2(planepnt, GM2_TRANSFER_CONTROL, (tanker - Planes));
//			}
		}
	}

	if(AICAllowThisRadio(planenum, 1))
	{
		AIC_Get_Callsign_With_Number(tanker - Planes, tankcallsign);
		AIC_Get_Callsign_With_Number(planenum, callsign);
		sprintf(tempstr, "%d", tankamount);
		if(!LANGGetTransMessage(kfuel, 1024, AIC_TANKER_GIVES, g_iLanguageId, tempstr))
		{
			return;
		}
	//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
	//	{
	//		return;
	//	}
//		if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_REQUEST_TANKER_MSG, g_iLanguageId, tankcallsign, callsign))
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_LOOKING_FOR, g_iLanguageId, tankcallsign, callsign, kfuel))
		{
			return;
		}
		AICAddAIRadioMsgs(tempstr, 40);

//		AIRGenericSpeech(AICF_WSO_REQUEST_TANKER_MSG, planenum, tanker - Planes);
		AICTankerContact1Snd(planenum, tanker - Planes, tankamount);
	}
	return;
}

//**************************************************************************************
void AICTankerContact1Snd(int planenum, int tankernum, int tankamount)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  Planes[planenum].AI.lAIVoice;  //  AIRIsFighter(planenum);

	if(!LANGGetTransMessage(tempstr, 1024, AICF_TANKER_LOOKING_FOR, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, tankernum);
		}
		else if(spchvar == 2)
		{
			if(numval == 3)
			{
				if(tankamount <= 0)
					tankamount = 1;

				if(tankamount > 9)
				{
					sndids[numids] = SPCH_ZERO + Planes[planenum].AI.lAIVoice + (tankamount / 10);
					numids ++;
				}
				sndids[numids] = SPCH_ZERO + Planes[planenum].AI.lAIVoice + (tankamount % 10);
				numids ++;

				sndids[numids] = SPCH_KAY + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AICTankerContact2(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	PlaneParams *planepnt, *tanker;
	tanker = &Planes[planenum];
	planepnt = tanker->AI.LinkedPlane;
	if(MultiPlayer)
	{
		planepnt->AI.lVar3 &= 0x1;
		planepnt->AI.lVar3 |= 0x4;

		if((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))
		{
	 		NetPutGenericMessage3(planepnt, GM3_CONTACT_REFUELER, (tanker - Planes), tanker->AI.cUseWeapon);
			NetPutDesiredHeading(tanker - Planes, tanker->AI.DesiredHeading);
//			if(!(tanker->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
//			{
				tanker->AI.iMultiPlayerAIController = PlayerPlane - Planes;
				tanker->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
				NetPutGenericMessage2(planepnt, GM2_TRANSFER_CONTROL, (tanker - Planes));
//			}
		}
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, tempstr2);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_PRECONTACT_MSG, g_iLanguageId, callsign, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_TANKER_PRECONTACT_MSG, planenum, (Planes[planenum].AI.LinkedPlane - Planes));

	return;
}

//**************************************************************************************
void AICTankerOn1(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char tankcallsign[256];
	char callsign[256];
	PlaneParams *tanker;

	return;  //  Not needed for F/A-18

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tanker = &Planes[planenum];  //  AICheckTankerCloseBy(PlayerPlane);

	if(tanker == NULL)
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	PlaneParams *planepnt;
	tanker = &Planes[planenum];
	planepnt = tanker->AI.LinkedPlane;
	if(MultiPlayer)
	{
		planepnt->AI.lVar3 &= 0x1;
		planepnt->AI.lVar3 |= 0x4;

		if((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))
		{
	 		NetPutGenericMessage3(planepnt, GM3_CONTACT_REFUELER, (tanker - Planes), tanker->AI.cUseWeapon);
			NetPutDesiredHeading(tanker - Planes, tanker->AI.DesiredHeading);
//			if(!(tanker->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
//			{
				tanker->AI.iMultiPlayerAIController = PlayerPlane - Planes;
				tanker->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
				NetPutGenericMessage2(planepnt, GM2_TRANSFER_CONTROL, (tanker - Planes));
//			}
		}
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

//	AICGetCallSign(planenum, tankcallsign);
//	AICGetCallSign((Planes[planenum].AI.LinkedPlane - Planes), callsign);
	AIC_Get_Callsign_With_Number(planenum, tankcallsign);
	AIC_Get_Callsign_With_Number((Planes[planenum].AI.LinkedPlane - Planes), callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
	{
		return;
	}
	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_PRECONTACT_READY_MSG, g_iLanguageId, tankcallsign, callsign, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_WSO_PRECONTACT_READY_MSG, (Planes[planenum].AI.LinkedPlane - Planes), planenum);
	return;
}

//**************************************************************************************
void AICTankerOn2(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	PlaneParams *planepnt, *tanker;
	tanker = &Planes[planenum];
	planepnt = tanker->AI.LinkedPlane;
	if(MultiPlayer)
	{
		planepnt->AI.lVar3 &= 0x1;
		planepnt->AI.lVar3 |= 0x4;

		if((planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (planepnt == PlayerPlane))
		{
	 		NetPutGenericMessage3(planepnt, GM3_CONTACT_REFUELER, (tanker - Planes), tanker->AI.cUseWeapon);
			NetPutDesiredHeading(tanker - Planes, tanker->AI.DesiredHeading);
//			if(!(tanker->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
//			{
				tanker->AI.iMultiPlayerAIController = PlayerPlane - Planes;
				tanker->AI.iAICombatFlags1 |= AI_MULTI_ACTIVE;
				NetPutGenericMessage2(planepnt, GM2_TRANSFER_CONTROL, (tanker - Planes));
//			}
		}
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number((Planes[planenum].AI.LinkedPlane - Planes), callsign);
//	AICGetCallSign((int)(Planes[planenum].AI.LinkedPlane - Planes), callsign);
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
//	{
//		return;
//	}
	AICGetCallSign(planenum, tempstr2);
	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_ON_MSG, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_TANKER_ON_MSG, planenum, (Planes[planenum].AI.LinkedPlane - Planes));
	return;
}

//**************************************************************************************
void AICReceivingFuel(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char tankcallsign[256];

	//  Make sound call

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(Planes[planenum].AI.LinkedPlane - Planes, 1))
	{
		return;
	}

	AICGetCallSign((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
	AICGetCallSign((int) (&Planes[planenum] - Planes), tankcallsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_FUELING_MSG, g_iLanguageId, tankcallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_WSO_FUELING_MSG, (Planes[planenum].AI.LinkedPlane - Planes), planenum);
	return;
}

//**************************************************************************************
void AICTankerGoodHunting(int planenum, int targetnum)
{
	char tempstr[1024];
//	char tempstr2[256];
	char callsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AICGetCallSign((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
//	{
//		return;
//	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_HUNTING_MSG, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_TANKER_HUNTING_MSG, planenum, (Planes[planenum].AI.LinkedPlane - Planes));
	return;
}

//**************************************************************************************
void AICTankerHaveDay(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AICGetCallSign((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
	{
		return;
	}
	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_HANDAY_MSG, g_iLanguageId, callsign, tempstr2))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_TANKER_HANDAY_MSG, planenum, (Planes[planenum].AI.LinkedPlane - Planes));
	return;
}

//**************************************************************************************
void AICWSOThanksForGas(int planenum, int targetnum)
{
	char tempstr[1024];
//	char tempstr2[256];
	char callsign[256];
	char tankcallsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(Planes[planenum].AI.LinkedPlane - Planes, 1))
	{
		return;
	}

	AICGetCallSign((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
	AICGetCallSign((int) (planenum), tankcallsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_TANKER_THANKS_MSG_2, g_iLanguageId, tankcallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_WSO_TANKER_THANKS_MSG, (Planes[planenum].AI.LinkedPlane - Planes), planenum);
	return;
}

//**************************************************************************************
void AICWSOAcknowledged(int planenum, int targetnum)
{
	char tempstr[1024];
//	char tempstr2[256];
	char callsign[256];
	char tankcallsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(Planes[planenum].AI.LinkedPlane - Planes, 1))
	{
		return;
	}

	AICGetCallSign((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
	AICGetCallSign((int) (planenum), tankcallsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_ACK_1, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_WSO_TANKER_ACKNOWLEDGED_MSG, (Planes[planenum].AI.LinkedPlane - Planes), planenum);
	return;
}

//**************************************************************************************
void AICTankerDisconnecting(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char tankcallsign[256];
	PlaneParams *tanker;

	if(targetnum < 0)
	{
		tanker = AICheckTankerCloseBy(&Planes[planenum]);
	}
	else
	{
		tanker = &Planes[targetnum];
	}

	if(!tanker)
		return;

	//  Make sound call

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AICGetCallSign((int) planenum, callsign);
	AICGetCallSign((int) (tanker - Planes), tankcallsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_DISCONNECT_MSG, g_iLanguageId, tankcallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
//	AIRGenericSpeech(AICF_TANKER_DISCONNECT_MSG, planenum, tanker - Planes);
	AIRGenericSpeech(AICF_TANKER_DISCONNECT_MSG, tanker - Planes, planenum);
	return;
}

//**************************************************************************************
void AICWSODisconnecting(int planenum, int targetnum)
{
	char tempstr[1024];

	//  Make sound call

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(Planes[planenum].AI.LinkedPlane - Planes, 1))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_DISCONNECT_MSG, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_WSO_DISCONNECT_MSG, (Planes[planenum].AI.LinkedPlane - Planes));
	return;
}

//**************************************************************************************
void AICEndTanking(PlaneParams *planepnt, long delayticks)
{
	int endreply;

	AICAddSoundCall(AICTankerDisconnecting, planepnt - Planes, delayticks - 3000, 50);

	AICAddSoundCall(AICWSODisconnecting, planepnt - Planes, delayticks + 1000, 50);

	if((rand() & 7) == 0)
	{
		AICAddSoundCall(AICWSOThanksForGas, planepnt - Planes, delayticks + 4000, 50);
	}

	endreply = rand()%10;

	switch(endreply)
	{
		case 1:
			AICAddSoundCall(AICTankerHaveDay, planepnt - Planes, delayticks + 10000, 50);
			if(rand() & 1)
			{
				AICAddSoundCall(AICWSOAcknowledged, planepnt - Planes, delayticks + 15000, 50);
			}
			break;
		case 2:
			AICAddSoundCall(AICTankerGoodHunting, planepnt - Planes, delayticks + 10000, 50);
			if(rand() & 1)
			{
				AICAddSoundCall(AICWSOAcknowledged, planepnt - Planes, delayticks + 15000, 50);
			}
			break;
	}
}

//**************************************************************************************
void AICTankerCheckSwitches(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char callsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!Planes[planenum].AI.LinkedPlane)
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AICGetCallSign((int) (Planes[planenum].AI.LinkedPlane - Planes), callsign);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_CHECK_SWITCHES, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_TANKER_CHECK_SWITCHES, planenum, (Planes[planenum].AI.LinkedPlane - Planes));
	return;
}

//**************************************************************************************
void AICTankerHeading(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char tankcallsign[256];
	char callsign[256];
	char headingstr[256];
	char altstr[256];
	PlaneParams *tanker;
	long tempalt;
	long tempheading;

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tanker = AICheckTankerCloseBy(PlayerPlane);

	if(tanker == NULL)
	{
		return;
	}

	if(!AICAllowThisRadio(tanker - Planes, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(tanker - Planes, tankcallsign);
	AIC_Get_Callsign_With_Number(planenum, callsign);

#if 0
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + 1, g_iLanguageId))
	{
		return;
	}
#endif
	tempstr2[0] = 0;

	tempheading = -AIConvertAngleTo180Degree(tanker->AI.DesiredHeading);
	tempheading += 5;

	while(tempheading > 360)
		tempheading -= 360;

	while(tempheading < 0)
		tempheading += 360;

	tempheading /= 10;

	tempheading *= 10;

	while(tempheading > 360)
		tempheading -= 360;

	while(tempheading < 0)
		tempheading += 360;

	sprintf(headingstr, "%03ld", tempheading);

	tempalt = tanker->Altitude + 500;
	tempalt /= 1000;

#if 0
	if((tempalt < 10) || (tempalt > 30))
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_HEADING_NOALT, g_iLanguageId, callsign, tempstr2, tankcallsign, headingstr))
		{
			return;
		}
		AICTankerHeadingSpch(tanker - Planes, planenum, AICF_TANKER_HEADING_NOALT, tempheading, 10);
	}
	else
	{
#endif
		sprintf(altstr, "%ld", tempalt);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_HEADING_ALT, g_iLanguageId, callsign, tempstr2, tankcallsign, headingstr, altstr))
		{
			return;
		}
		AICTankerHeadingSpch(tanker - Planes, planenum, AICF_TANKER_HEADING_ALT, tempheading, tempalt);
//	}
	AICAddAIRadioMsgs(tempstr, 40);

	return;
}

//**************************************************************************************
void AICTankerHeadingSpch(int planenum, int tankingnum, int msgid, long heading, long alt)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;
	int theading;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(!LANGGetTransMessage(tempstr, 1024, msgid, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, tankingnum);
		}
		else if(spchvar == 2)
		{
			if(numval == 4)
			{
				sndids[numids] = SPCH_ZERO + (heading / 100) + Planes[planenum].AI.lAIVoice;
				numids ++;

				theading = heading % 100;
				sndids[numids] = SPCH_ZERO + (theading / 10) + Planes[planenum].AI.lAIVoice;
				numids ++;

				sndids[numids] = SPCH_ZERO + Planes[planenum].AI.lAIVoice;
				numids ++;

				sndids[numids] = 322 + Planes[planenum].AI.lAIVoice;
				numids ++;

				sndids[numids] = SPCH_ZERO + 3 + Planes[planenum].AI.lAIVoice;
				numids ++;

				sndids[numids] = SPCH_ZERO + Planes[planenum].AI.lAIVoice;
				numids ++;

				sndids[numids] = SPCH_ZERO + Planes[planenum].AI.lAIVoice;
				numids ++;

			}
			if(numval == 5)
			{
				sndids[numids] = 74 + Planes[planenum].AI.lAIVoice;
				numids ++;

				if(alt == 0)
				{
					sndids[numids] = SPCH_ZERO + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = SPCH_COUNT_ONE + (alt - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AICTankerBreakAway(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];

	//  Make sound call
//	AIRDisengageSnd(placeingroup);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AICGetCallSign(targetnum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_BREAKAWAY, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRGenericSpeech(AICF_WSO_BREAKAWAY, planenum, targetnum);
	return;
}

//**************************************************************************************
int AICheckHumanCover(PlaneParams *humanplane)
{
	MBWayPoints *lookway;
	int bombfound = 0;
	int escortfound = 0;
	int startact, endact, actcnt;
	int waycnt, startway;
	int returnval;
	Escort	*pEscortAction;

	returnval = -1;
	lookway = humanplane->AI.CurrWay;

	startact = lookway->iStartAct;
	endact = startact + lookway->iNumActs;
	for(actcnt = startact; actcnt < endact; actcnt ++)
	{
		if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET))
		{
			bombfound ++;
			break;
		}
	}

	if(bombfound)
	{
		return(returnval);
	}

	if(lookway <= &AIWayPoints[humanplane->AI.startwpts])
	{
		startway = 1;
	}
	else
	{
		startway = 0;
		lookway --;
	}

	for(waycnt = startway; waycnt < 2; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				escortfound ++;
				pEscortAction = (Escort *)AIActions[actcnt].pAction;
				returnval = pEscortAction->iPlaneNum;
				break;
			}
		}
		if(escortfound)
		{
			break;
		}
		lookway ++;
	}

	return(returnval);
}

//**************************************************************************************
void AICSetUpEscortPlayer(PlaneParams *humanplane, int firstvalid, int lastvalid, int escortplane)
{
	long delaycnt = 1000;

	if(PlayerPlane->Status & AL_AI_DRIVEN)
		return;

	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}


	if(planepnt->AI.wingman >= 0)
	{
		AICSetUpEscortPlayerGroup(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid, escortplane);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICSetUpEscortPlayerGroup(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid, escortplane);
	}
	return;
}

//**************************************************************************************
void AICSetUpEscortPlayerGroup(int planenum, long *delaycnt, int firstvalid, int lastvalid, int escortplane)
{
	int placeingroup;
//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid) && ((!(Planes[planenum].AI.iAIFlags2 & AILANDING)) || (Planes[planenum].AI.lTimer2 > 0)) && (!Planes[planenum].OnGround))
	{
		Planes[planenum].AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
		Planes[planenum].AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);
		Planes[planenum].AI.iAIFlags2 &= ~(AIKEEPTARGET);

#if 0
		if(Planes[planenum].AI.OrgBehave == NULL)
		{
			Planes[planenum].AI.Behaviorfunc = AIFlyEscort;
		}
		else
		{
			Planes[planenum].AI.OrgBehave = AIFlyEscort;
		}
#else
		Planes[planenum].AI.Behaviorfunc = AIFlyEscort;
		Planes[planenum].AI.OrgBehave = NULL;
#endif
		Planes[planenum].AI.LinkedPlane = &Planes[escortplane];
		AICAddSoundCall(AICWingCoverMsg, planenum, *delaycnt, 50);
		*delaycnt = *delaycnt + 2000;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICSetUpEscortPlayerGroup(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid, escortplane);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICSetUpEscortPlayerGroup(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid, escortplane);
	}
	return;
}

//**************************************************************************************
void AICW_RTB()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 1, &firstvalid, &lastvalid);
	AICReturnToBase(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICE_RTB()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(2, 3, &firstvalid, &lastvalid);
	AICReturnToBase(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICD_RTB()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(4, 7, &firstvalid, &lastvalid);
	AICReturnToBase(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICF_RTB()
{
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(1, 7, &firstvalid, &lastvalid);
	AICReturnToBase(firstvalid, lastvalid);
	return;
}

//**************************************************************************************
void AICReturnToBase(int firstvalid, int lastvalid)
{
	long delaycnt = 1000;
	int firstreturn = -1;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICReturnToBasePlane(planepnt->AI.wingman, &delaycnt, firstvalid, lastvalid, &firstreturn);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICReturnToBasePlane(planepnt->AI.nextpair, &delaycnt, firstvalid, lastvalid, &firstreturn);
	}
	return;
}

//**************************************************************************************
void AICReturnToBasePlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int *firstreturn)
{
	int placeingroup;
	long xftoff = 0;
	long zftoff = 120000;
	FPointDouble fptemp;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType ActionTakeOff;

//	void (*Soundfunc)(int planenum);

//	Soundfunc = AICBasicAck;

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1  & AIFLIGHTNUMS);

	if((placeingroup >= firstvalid) && (placeingroup <= lastvalid) && ((!(Planes[planenum].AI.iAIFlags2 & AILANDING)) || (Planes[planenum].AI.lTimer2 > 0)) && (!Planes[planenum].OnGround))
	{
		if(lastvalid < 8)
		{
			if(AICAllowThisRadio(planenum, 1))
			{
				AICAddSoundCall(AIC_RTB_Msg, planenum, *delaycnt, 50);
				*delaycnt = *delaycnt + 4000;
			}
		}

		//  May have to do something here to check for defensive action being done.  SRE
		Planes[planenum].AI.DesiredPitch = 0;
		if(*firstreturn == -1)
		{
			*firstreturn = planenum;
			if(!AICheckPlaneLandingFlyToCarrier(&Planes[planenum], &ActionTakeOff))
			{
				Planes[planenum].AI.Behaviorfunc = AIPlaneLandingFlyToField;
				Planes[planenum].AI.OrgBehave = NULL;

				//  Eventually get this off the aActionTakeOff information.
				fptemp.SetValues(-1.0f,-1.0f,-1.0f);
				AIGetClosestAirField(&Planes[planenum], fptemp, &runwaypos, &runwayheading, &ActionTakeOff, -1.0f, 0);

				Planes[planenum].AI.DesiredHeading = runwayheading;
				Planes[planenum].AI.TargetPos = runwaypos;

				AIGetXZOffset((float)Planes[planenum].AI.DesiredHeading / DEGREE, Planes[planenum].AI.TargetPos.X, Planes[planenum].AI.TargetPos.Z, xftoff, zftoff, &Planes[planenum].AI.WayPosition.X, &Planes[planenum].AI.WayPosition.Z);
	//			Planes[planenum].AI.WayPosition.Y = ConvertWayLoc(10000);
				Planes[planenum].AI.LinkedPlane = NULL;
				Planes[planenum].AI.WayPosition.Y = AIGetMarshallHeight(&Planes[planenum]);
			}
		}
		else
		{
			if(!AICheckPlaneLandingFlyToCarrier(&Planes[planenum], &ActionTakeOff))
			{
				Planes[planenum].AI.Behaviorfunc = AIFormationFlyingLinked;
				Planes[planenum].AI.OrgBehave = NULL;
				Planes[planenum].AI.LinkedPlane = &Planes[*firstreturn];
				Planes[planenum].AI.TargetPos = Planes[*firstreturn].AI.TargetPos;
				Planes[planenum].AI.DesiredHeading = Planes[*firstreturn].AI.DesiredHeading;
			}
		}
		Planes[planenum].AI.lTimer2 = 120000;
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICReturnToBasePlane(Planes[planenum].AI.wingman, delaycnt, firstvalid, lastvalid, firstreturn);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICReturnToBasePlane(Planes[planenum].AI.nextpair, delaycnt, firstvalid, lastvalid, firstreturn);
	}
	return;
}

//**************************************************************************************
void AIC_RTB_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	long msgval, msgsnd;
	int tempval;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	//  Make sound call for basic ack.

//	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, tempstr2);

	if((Planes[planenum].AI.lAIVoice >= SPCH_WNG1) && (Planes[planenum].AI.lAIVoice <= SPCH_NAVY5))
	{
		tempval = rand() % 3;
		msgval = AIC_RTB_1 + tempval;
		if(!LANGGetTransMessage(tempstr, 1024, msgval, g_iLanguageId, tempstr2))
		{
			return;
		}
	}
	else
	{
		tempval = 1 + (rand() & 1);

		if(tempval == 2)
		{
			msgval = AIC_RTB_3;
		}
		else
		{
			msgval = AIC_FLIGHT_RTB;
		}

		if(!LANGGetTransMessage(tempstr, 1024, msgval, g_iLanguageId, tempstr2))
		{
			return;
		}
	}

	msgsnd = AICF_RTB_1 + tempval, planenum;

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(AICF_RTB_1 + tempval, planenum);

	return;
}

//**************************************************************************************
void AIC_Set_Takeoff_Msgs(int planenum, int targetnum)
{
	long delaycnt = 3000;
	int closestenemy;
	int awacsnum;

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	closestenemy = AICheckForEnemies(planenum, 80);

	if(closestenemy != -1)
	{
		awacsnum = AIGetClosestAWACS(&Planes[planenum]);
		delaycnt = 1000;
		AICAddSoundCall(AIC_Scramble_Takeoff_Msg, planenum, delaycnt, 50, closestenemy);
		if((Planes[planenum].AI.wingman >= 0) || (Planes[planenum].AI.nextpair >= 0))
		{
			AICAddSoundCall(AIC_WSO_Run_em, planenum, 8000, 50, closestenemy);
			delaycnt = delaycnt + 5000;
		}
		delaycnt = delaycnt + 17000;
		AICAddSoundCall(AIC_AWACS_Threat_Msg, planenum, delaycnt, 50, closestenemy);
		delaycnt += 10000;
		AICAddSoundCall(AICCheckForFriendlyTraffic, planenum, delaycnt, 40, 0);
	}
	else
	{
		AICAddSoundCall(AIC_Request_Takeoff_Msg, planenum, delaycnt, 50);
		delaycnt = delaycnt + 6000;
		AICAddSoundCall(AIC_Clear_Takeoff_Msg, planenum, delaycnt, 50);
		delaycnt += 17000;
		if((Planes[planenum].AI.wingman >= 0) || (Planes[planenum].AI.nextpair >= 0))
		{
			AICAddSoundCall(AIC_WSO_Run_em, planenum, delaycnt, 50);
			delaycnt += 4000;
		}
		delaycnt += 6000;
		AICAddSoundCall(AICCheckForFriendlyTraffic, planenum, delaycnt, 40, 0);
	}
}

//**************************************************************************************
void AIC_Request_Takeoff_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	long towernum;
	char towercallsign[256];
	long towervoice;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);
	towernum = AIGetTowerCallsign(planenum, &towervoice);

//	strcpy(towercallsign, CallSignList[towernum].sName);
	if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_REQUEST_TAKEOFF_2, g_iLanguageId, towercallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_REQUEST_TAKEOFF, planenum, towernum + 5, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Clear_Takeoff_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char towercallsign[256];
	char awacscallsign[256];
	char winddirstr[256];
	char ceilingstr[256];
	char visiblestr[256];
	int awacsnum;
	long towernum, towervoice;
	long awacssndid = 800;
	long winddirnum, windspeednum, ceilingnum, visiblenum;
	int departure = 0;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);
	towernum = AIGetTowerCallsign(planenum, &towervoice);
//	strcpy(towercallsign, CallSignList[towernum].sName);
	if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
	{
		return;
	}

	winddirnum = AICGetWindDirSpeed(planenum, winddirstr, &windspeednum);
	ceilingnum = AICGetCeiling(planenum, ceilingstr);
	if(ceilingnum > 33)
	{
		ceilingstr[0] = 0;
	}
	visiblenum = AICGetVisibility(planenum, visiblestr);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum >= 0)
	{
		awacssndid = AIRGetIntenseCallSignID(awacsnum);
		if((awacssndid == 608) || (awacssndid == 609) || (awacssndid == 610))
		{
			awacssndid -= 2;
		}
		else
		{
			awacssndid = 0;
		}
	}
	else
	{
		awacssndid = 0;
	}

	if(departure)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_CLEARED_FOR_TAKEOFF_DEPARTURE, g_iLanguageId, callsign))
		{
			return;
		}
		AICRDoClearTakeoff(AICF_CLEARED_FOR_TAKEOFF_DEPARTURE, towernum + 5, planenum, towervoice, winddirnum * 10, windspeednum, ceilingnum, visiblenum, awacssndid);
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TOWER_CLEAR_TAKEOFF_2, g_iLanguageId, callsign, winddirstr, ceilingstr, visiblestr))
		{
			return;
		}
		AICRDoClearTakeoff(AICF_TOWER_CLEAR_TAKEOFF, towernum + 5, planenum, towervoice, winddirnum * 10, windspeednum, ceilingnum, visiblenum, awacssndid);
	}
	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
long AICGetWindDirSpeed(int planenum, char *winddirstr, long *windspeednum)
{
	float fwindbearing;
	int windbearing;
	char bearstr[6];
	char windspeedstr[256];
//	char tempstr[256];

	fwindbearing = 360.0f * frand();
	windbearing = fwindbearing;
	windbearing /= 10;
	if(windbearing >= 36)
	{
		windbearing = 35;
	}

	sprintf(bearstr, "%03d", windbearing * 10);

	*windspeednum = AICGetWindSpeed(planenum, windspeedstr);

	if(*windspeednum == 0)
	{
		bearstr[0] = 0;
		bearstr[1] = 0;

		if(!LANGGetTransMessage(winddirstr, 256, AIC_WINDS, g_iLanguageId, windspeedstr))
		{
			return(windbearing);
		}
	}
	else
	{
		if(!LANGGetTransMessage(winddirstr, 256, AIC_TOWER_WINDS_ARE, g_iLanguageId, bearstr, windspeedstr))
		{
			return(windbearing);
		}
	}

	return(windbearing);
}

//**************************************************************************************
long AICGetWindSpeed(int planenum, char *windspeedstr)
{
	float fwindvel;
	int windvel;
	char tempstr2[256];

	if((WorldParams.Weather & (WR_VIS_LOW|WR_CLOUD1_OVERCAST)) == (WR_VIS_LOW | WR_CLOUD1_OVERCAST))
	{
		fwindvel = frand() * 34.0f;
	}
	else
	{
		fwindvel = frand() * 19.0f;
	}

	windvel = fwindvel;
	windvel /= 5;
	if(windvel > 5)
	{
		windvel = 5;
	}

	if(windvel)
	{

#if 1
		sprintf(tempstr2, "%d", windvel * 5);
#else
		if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + (windvel * 5), g_iLanguageId))
		{
			return(windvel);
		}
#endif

		if(!LANGGetTransMessage(windspeedstr, 256, AIC_TOWER_WIND_SPEED, g_iLanguageId, tempstr2))
		{
			return(windvel);
		}
	}
	else
	{
		if(!LANGGetTransMessage(windspeedstr, 256, AIC_TOWER_WINDS_CALM, g_iLanguageId))
		{
			return(windvel);
		}
	}

	return(windvel);
}

//**************************************************************************************
long AICGetCeiling(int planenum, char *ceilingstr)
{
	int ceiling;
	char tempstr2[256];
	float fceiling;

	if(!(WorldParams.Weather & WR_CLOUD1))
	{
		ceiling = 100;
	}
	else
	{
		fceiling = WorldParams.CloudAlt * WUTOFT;
		fceiling /= 1000;
		ceiling = fceiling;
	}


	if(ceiling < 1)
	{
		ceiling = 1;
	}
	else if(ceiling > 36)
	{
		ceiling = 36;

		if(!LANGGetTransMessage(ceilingstr, 256, AIC_TOWER_CEILING_UNLIMITED, g_iLanguageId, tempstr2))
		{
			return(ceiling);
		}
		return(ceiling);
	}

#if 1
		sprintf(tempstr2, "%d", ceiling);
#else
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + ceiling, g_iLanguageId))
	{
		return(ceiling);
	}
#endif

	if(!LANGGetTransMessage(ceilingstr, 256, AIC_TOWER_CEILING, g_iLanguageId, tempstr2))
	{
		return(ceiling);
	}

	return(ceiling);
}

//**************************************************************************************
long AICGetVisibility(int planenum, char *visiblestr)
{
	float fvis;
	int visibility;
	char tempstr2[256];
	int allowhalf = 0;

//	visibility = 100;

	fvis = CurrentMaxDistance * WUTONM;
	visibility = fvis;

	if((visibility == 0) && allowhalf)
	{
		if(!LANGGetTransMessage(visiblestr, 256, AIC_TOWER_VIS_HALF_MILE, g_iLanguageId))
		{
			return(visibility);
		}
	}
	else if(visibility <= 1)
	{
		if(!LANGGetTransMessage(visiblestr, 256, AIC_TOWER_VIS_ONE_MILE, g_iLanguageId))
		{
			return(visibility);
		}
	}
	else if(visibility < 25)
	{
#if 1
		sprintf(tempstr2, "%d", visibility);
#else
		if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + visibility, g_iLanguageId))
		{
			return(visibility);
		}
#endif
		if(!LANGGetTransMessage(visiblestr, 256, AIC_TOWER_VISABILITY, g_iLanguageId, tempstr2))
		{
			return(visibility);
		}

//		if(visibility == 24)
//		{
//			visibility = 17;
//		}
	}
	else
	{
		visibility = 25;	//  18;
		if(!LANGGetTransMessage(visiblestr, 256, AIC_TOWER_VIS_UNLIMITED, g_iLanguageId))
		{
			return(visibility);
		}
	}

	return(visibility);
}

//**************************************************************************************
void AICRDoClearTakeoff(int msgid, int towernum, int planenum, long towervoice, int winddirnum, int windspeednum, int ceilingnum, int visiblenum, int awacssndid)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;
	int workvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(!LANGGetTransMessage(tempstr, 1024, msgid, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, towernum, planenum, 0, 0, 0, 0, 0, 0, 0, towervoice);
		}
		else if(spchvar == 2)
		{
			if(numval == 3)  // winds
			{
//				sndids[numids] = (TOWER_WINDS_ARE - SPCH_TOWER1) + towervoice;
				sndids[numids] = (TOWER_WINDS_ARE) + towervoice;
				numids ++;
				if(windspeednum != 0)
				{
//					sndids[numids] = ((TOWER_WINDS_000_AT - SPCH_TOWER1) + winddirnum) + towervoice;
//					numids ++;
					sndids[numids] = (SPCH_ZERO + (winddirnum / 100)) + towervoice;
					numids ++;

					workvar = winddirnum %100;
					sndids[numids] = (SPCH_ZERO + (workvar / 10)) + towervoice;
					numids ++;

					workvar = winddirnum %10;
					sndids[numids] = (SPCH_ZERO + workvar) + towervoice;
					numids ++;

					sndids[numids] = (439) + towervoice;  // at
					numids ++;
				}

				if(windspeednum == 0)
				{
					sndids[numids] = TOWER_WINDS_CALM + towervoice;
					numids ++;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + ((windspeednum * 5) - 1)) + towervoice;
					numids ++;
				}
			}
			if(numval == 4)  // ceiling
			{
				if(ceilingnum <= 33)
				{
					sndids[numids] = 436 + towervoice;
					numids ++;
					sndids[numids] = ((TOWER_CEILING_ANGELS_1) + (ceilingnum - 1)) + towervoice;
					numids ++;
				}
			}
			if(numval == 5)  // visibility
			{
				if(visiblenum < 1)
				{
					visiblenum = 1;
				}
				sndids[numids] = 437 + towervoice;
				numids ++;
				if(visiblenum < 25)
				{
					sndids[numids] = ((SPCH_COUNT_ONE) + visiblenum - 1) + towervoice;
				}
				else
				{
					sndids[numids] = 300 + towervoice;
				}
				numids ++;
			}
			if(numval == 6)
			{
				sndids[numids] = awacssndid + towervoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + towervoice;
			numids ++;
		}
	}
//	AIRSendSentence(planenum, numids, sndids);
	AIRSendSentence(planenum, numids, sndids, towervoice);
}

//**************************************************************************************
void AIC_WSO_Run_em(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_ROLLING, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_ROLLING, planenum);
	return;
}

//**************************************************************************************
int AIGetClosestAWACS(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (planepnt->AI.iSide == checkplane->AI.iSide) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3)  && (pDBAircraftList[checkplane->AI.iPlaneIndex].lPlaneID != 20))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (AIIsCAPing(checkplane)))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if((tdist < foundrange) || (foundrange == -1))
				{
					foundrange = tdist;
					foundplane = checkplane - Planes;
				}
			}
		}
		checkplane ++;
	}
	return(foundplane);
}

//**************************************************************************************
int AIGetClosestJSTARS(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (planepnt->AI.iSide == checkplane->AI.iSide) && (pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3)  && (pDBAircraftList[checkplane->AI.iPlaneIndex].lPlaneID == 20))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && ((AIIsCAPing(checkplane)) || AIIsCampaignJSTARS(checkplane)))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if((tdist < foundrange) || (foundrange == -1))
				{
					foundrange = tdist;
					foundplane = checkplane - Planes;
				}
			}
		}
		checkplane ++;
	}
	return(foundplane);
}

//**************************************************************************************
int AIIsCAPing(PlaneParams *planepnt)
{
	if(planepnt->AI.Behaviorfunc == AISetUpCAP1)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AISetUpCAP2)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyCAPSide1)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyCAPSide2)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyCAPSide3)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlyCAPSide4)
	{
		return(1);
	}
	return(0);
}

//**************************************************************************************
int AIIsSARCAPing(PlaneParams *planepnt)
{
	if(planepnt->AI.Behaviorfunc == AISetUpSARCAP)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlySARCAPSide1)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlySARCAPSide2)
	{
		return(1);
	}
	if(planepnt->AI.Behaviorfunc == AIFlySARCAPSide3)
	{
		return(1);
	}
	return(0);
}

//**************************************************************************************
int AICheckForEnemies(int planenum, int distnm)
{
	PlaneParams *checkplane = &Planes[0];
	PlaneParams *planepnt = &Planes[planenum];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	float tdistnm;
	int foundplane = -1;
	int enemyval;

	if(planepnt->AI.iSide == AI_ENEMY)
	{
		enemyval = AI_FRIENDLY;
	}
	else
	{
		enemyval = AI_ENEMY;
	}

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (enemyval == checkplane->AI.iSide) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1) && (!(checkplane->OnGround)))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				tdistnm = (tdist * WUTONM);
				if(((tdist < foundrange) || (foundrange == -1)) && (tdistnm < distnm))
				{
					foundrange = tdist;
					foundplane = checkplane - Planes;
				}
			}
		}
		checkplane ++;
	}
	return(foundplane);
}

//**************************************************************************************
void AICCheckForNewThreat(PlaneParams *planepnt)
{
	float dx, dz, tdist, tdistnm;

	if(planepnt->AI.iSide == PlayerPlane->AI.iSide)
		return;

	dx = planepnt->WorldPosition.X - PlayerPlane->WorldPosition.X;
	dz = planepnt->WorldPosition.Z - PlayerPlane->WorldPosition.Z;

	tdist = QuickDistance(dx,dz);
	tdistnm = (tdist * WUTONM);

	if(tdistnm < 80)
	{
		AIC_AWACS_Threat_Takeoff_Msg(PlayerPlane - Planes, planepnt - Planes);
		return;
	}

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		dx = planepnt->WorldPosition.X - ConvertWayLoc(BullsEye.x);
		dz = planepnt->WorldPosition.Z - ConvertWayLoc(BullsEye.z);

		tdist = QuickDistance(dx,dz);
		tdistnm = (tdist * WUTONM);

		if(tdistnm < 150)
		{
			AIC_AWACS_Threat_Takeoff_Msg(PlayerPlane - Planes, planepnt - Planes);
			return;
		}
	}
}


//**************************************************************************************
void AIC_AWACS_Threat_Msg(int planenum, int targetnum)
{
	AIC_AWACS_Calls(1, &Planes[planenum], &Planes[targetnum]);
}

//**************************************************************************************
void AIC_AWACS_Bogey_Dope_Msg(int planenum, int targetnum)
{
	AIC_AWACS_Calls(2, &Planes[planenum], &Planes[targetnum]);
}

//**************************************************************************************
void AIC_AWACS_Picture_Msg(int planenum, int targetnum)
{
	AIC_AWACS_Calls(3, &Planes[planenum], &Planes[targetnum]);
}

//**************************************************************************************
void AIC_AWACS_Threat_Takeoff_Msg(int planenum, int targetnum)
{
	AIC_AWACS_Calls(4, &Planes[planenum], &Planes[targetnum]);
}

//**************************************************************************************
void AIC_AWACS_Calls(int calltype, PlaneParams *planepnt, PlaneParams *targplane)
{
	char callsign[128];
	char awacscallsign[128];
	char tempstr[1024];
	char altstr[128];
	char aspectstr[128];
	char bearstr[128];
	char rangestr[128];
	char speedstr[128];
	char groupstr[256];
	char headingstr[256];
	int bearing360;
	float rangenm;
	float tbearing;
	int planenum;
	float dx, dy, dz, bearing, range;
	int awacsnum;
	int aspectval;
	int altval;
	int dobullseye = 0;
	int speedval;
	int groupval;
	int headingval;
	int status;
	int voice = 0;
	int awacsid;

	if(targplane == NULL)
	{
		return;
	}

	awacsid = awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
	{
		awacsnum = -2;
		voice = SPCH_STRIKE;
		awacsid = 1;
  		if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			status = MovingVehicles[planepnt->AI.iHomeBaseId].Status;
			if((!status) || (status & VL_SINKING))
			{
				return;
			}
		}
		else
		{
			return;
		}

		if(!AICAllowThisRadio(planepnt - Planes, 1))
		{
			return;
		}
	}
	else
	{
		if(!AICAllowThisRadio(awacsnum, 1))
		{
			return;
		}
	}

	planenum = planepnt - Planes;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)) && ((calltype == 3) || (calltype == 4)))
	{
		dobullseye = 1;
		bearing = AIComputeHeadingFromBullseye(targplane->WorldPosition, &range, &dx, &dz);

		rangenm = (range * WUTONM);
		if(rangenm > 140)
		{
			dobullseye = 0;
			bearing = AIComputeHeadingToPoint(planepnt, targplane->WorldPosition, &range, &dx ,&dy, &dz, 0);
		}
		else if(rangenm < 5)
		{
			rangenm = 5;
		}
	}
	else
	{
		bearing = AIComputeHeadingToPoint(planepnt, targplane->WorldPosition, &range, &dx ,&dy, &dz, 0);

		rangenm = (range * WUTONM);
	}

	headingval = AICGetTextHeadingGeneral(headingstr, bearing);

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
	}
	else
	{
		if(!LANGGetTransMessage(awacscallsign, 128, AIC_STRIKE, g_iLanguageId))
		{
			return;
		}
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if((targplane->WorldPosition.Y * WUTOFT) > 40000)
	{
		altval = 0;
		if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->WorldPosition.Y * WUTOFT) > 30000)
	{
		altval = 1;
		if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->HeightAboveGround * WUTOFT) < 10000)
	{
		altval = 3;
		if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		altval = 2;
		if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
		{
			return;
		}
	}

	groupval = AICGetIfPlaneGroupText(groupstr, targplane, 1);

	if((targplane->V * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) < 300)
	{
		speedval = 0;
		if(!LANGGetTransMessage(speedstr, 128, AIC_SLOW_MOVING, g_iLanguageId))
		{
			return;
		}
	}
	else if(((targplane->V * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) > 600) || (AICalculateMach(targplane->Altitude, (float)targplane->V) > 1.0f))
	{
		speedval = 1;
		if(!LANGGetTransMessage(speedstr, 128, AIC_FAST_MOVING, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		speedval = 2;
		speedstr[0] = 0;
	}

	aspectval = AICGetAspectText(aspectstr, bearing, targplane);
	AICGetRangeText(rangestr, rangenm);

	if((rangenm < 5) && (calltype != 4))
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_MERGED, g_iLanguageId, callsign, awacscallsign))
		{
			return;
		}
		AIRGenericSpeech(AICF_AWACS_MERGED, awacsid, planenum, bearing, 0, aspectval, rangenm, altval, 0, 0, voice);
	}
	else if(targplane->AI.iSide == planepnt->AI.iSide)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_FRIENDLY_BEARING, g_iLanguageId, callsign, awacscallsign, bearstr, rangestr, altstr, aspectstr))
		{
			return;
		}
		AIRGenericSpeech(AICF_AWACS_FRIENDLY_BEARING, awacsid, planenum, bearing, 0, aspectval, rangenm, altval, 0, 0, voice);
	}
	else
	{
		if(calltype == 4)
		{
			if(dobullseye)
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_NEW_THREAT_BULLSEYE, g_iLanguageId, awacscallsign, headingstr, rangestr))
				{
					return;
				}
				AIRGenericSpeech(AICF_AWACS_NEW_THREAT_BULLSEYE, awacsid, planenum, bearing, 0, headingval, rangenm, altval, speedval, groupval, voice);
			}
			else
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_NEW_THREAT_BEARING, g_iLanguageId, callsign, awacscallsign, bearstr, rangestr))
				{
					return;
				}
				AIRGenericSpeech(AICF_AWACS_NEW_THREAT_BEARING, awacsid, planenum, bearing, 0, 0, rangenm, altval, speedval, groupval, voice);
			}
		}
		else if(calltype == 3)
		{
			if(dobullseye)
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_BULLSEYE_PICT, g_iLanguageId, awacscallsign, speedstr, groupstr, headingstr, rangestr, altstr))
				{
					return;
				}
				AIRGenericSpeech(AICF_AWACS_BULLSEYE_PICT, awacsid, planenum, bearing, 0, headingval, rangenm, altval, speedval, groupval, voice);
			}
			else
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_BEARING_PICT, g_iLanguageId, callsign, awacscallsign, speedstr, groupstr, bearstr, rangestr, altstr))
				{
					return;
				}
				AIRGenericSpeech(AICF_AWACS_BEARING_PICT, awacsid, planenum, bearing, 0, 0, rangenm, altval, speedval, groupval, voice);
			}
		}
		else if(calltype == 2)
		{
			if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_BEARING, g_iLanguageId, callsign, awacscallsign, speedstr, groupstr, bearstr, rangestr, altstr, aspectstr))
			{
				return;
			}
			AIRGenericSpeech(AICF_AWACS_BEARING, awacsid, planenum, bearing, 0, aspectval, rangenm, altval, speedval, groupval, voice);
		}
		else
		{
			if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_THREAT_BEARING, g_iLanguageId, callsign, awacscallsign, speedstr, bearstr, rangestr, altstr, aspectstr))
			{
				return;
			}
			AIRGenericSpeech(AICF_AWACS_THREAT_BEARING, awacsid, planenum, bearing, 0, aspectval, rangenm, altval, speedval, 0, voice);
		}
	}

	AICAddAIRadioMsgs(tempstr, 40);
}

//**************************************************************************************
int AICGetAspectText(char *aspectstr, float bearing, PlaneParams *planepnt)
{
	float workbearing;
	float tbearing;
	int returnval = 0;

	workbearing = bearing - 180;
	workbearing -= AIConvertAngleTo180Degree(planepnt->Heading);
	tbearing = -workbearing;

	tbearing = AICapAngle(tbearing);

	if(fabs(tbearing) < 45)
	{
		returnval = 1;
		if(!LANGGetTransMessage(aspectstr, 128, AIC_HOT, g_iLanguageId))
		{
			return(returnval);
		}
	}
	else if(fabs(tbearing) > 135)
	{
		returnval = 2;
		if(!LANGGetTransMessage(aspectstr, 128, AIC_COLD, g_iLanguageId))
		{
			return(returnval);
		}
	}
	else
	{
		returnval = 0;
		if(!LANGGetTransMessage(aspectstr, 128, AIC_FLANKING, g_iLanguageId))
		{
			return(returnval);
		}
	}
	return(returnval);
}

//**************************************************************************************
void AICGetRangeText(char *rangestr, float rangenm)
{
	long lrangenm;
	long workrange;
	char tstr[128];

	lrangenm = (long)rangenm;

	if(lrangenm == 1)
	{
		if(!LANGGetTransMessage(rangestr, 128, AIC_WSO_MILE_TO_TARGET, g_iLanguageId))
		{
			return;
		}
	}
	else if(lrangenm <= 80)
	{
		if(lrangenm > 60)
		{
			lrangenm /= 5;
			lrangenm *= 5;
		}

		if(!LANGGetTransMessage(tstr, 128, AIC_ZERO + lrangenm, g_iLanguageId))
		{
			return;
		}

		if(!LANGGetTransMessage(rangestr, 128, AIC_WSO_MILES_TO_TARGET, g_iLanguageId, tstr))
		{
			return;
		}
	}
	else if(lrangenm <= 200)
	{
		if(!LANGGetTransMessage(tstr, 256, AIC_EIGHTY + (((lrangenm / 5) - 16)), g_iLanguageId))
		{
			return;
		}

		if(!LANGGetTransMessage(rangestr, 128, AIC_WSO_MILES_TO_TARGET, g_iLanguageId, tstr))
		{
			return;
		}
	}
#if 0
	else if(lrangenm <= 150)
	{
		if(!LANGGetTransMessage(tstr, 256, AIC_ONE_HUNDRED + ((lrangenm / 10) - 10), g_iLanguageId))
		{
			return;
		}

		if(!LANGGetTransMessage(rangestr, 128, AIC_WSO_MILES_TO_TARGET, g_iLanguageId, tstr))
		{
			return;
		}
	}
#endif
	else
	{
		workrange = lrangenm / 10;
		workrange *= 10;
		sprintf(rangestr, "%ld", workrange);
	}
}

//**************************************************************************************
void AIC_Scramble_Takeoff_Msg(int planenum, int targetnum)
{
	float dx, dy, dz, tdist, offangle;
	char eheadingstr[128];
	char tempstr[1024];
	char callsign[256];
	char towercallsign[256];
	long towernum, towervoice;
	int headingval;
	float tbearing;
	int bearing360;
	char bearstr[128];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	offangle = AIComputeHeadingToPoint(&Planes[planenum], Planes[targetnum].WorldPosition, &tdist, &dx ,&dy, &dz, 0);

	headingval = AICGetTextHeadingGeneral(eheadingstr, offangle);

	tbearing = -offangle;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	AICGetCallSign(planenum, callsign);
	towernum = AIGetTowerCallsign(planenum, &towervoice);
//	strcpy(towercallsign, CallSignList[towernum].sName);
	if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TOWER_SCRAMBLE_1, g_iLanguageId, callsign, bearstr))  //  eheadingstr))
	{
		return;
	}

	AIRGenericSpeech(AICF_TOWER_SCRAMBLE, towernum + 5, planenum, offangle, headingval, 0, 0, 0, 0, 0, towervoice);
	AICAddAIRadioMsgs(tempstr, 40);
}

//**************************************************************************************
void AICPlayerRequestLanding()
{
	PlaneParams *planepnt;
	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
		if(planepnt->OnGround)
			return;
	}
	else
	{
		planepnt = PlayerPlane;
		if(planepnt->OnGround)
			return;
		iATCRadio |= 2;
	}

	AICRequestLanding(planepnt);
}

//**************************************************************************************
void AICRequestLanding(PlaneParams *planepnt)
{
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;
	long delaycnt = 1000;
	RunwayInfo *tower;

	if(planepnt->OnGround)
		return;

	if(!AICAllowThisRadio(planepnt - Planes, 1))
	{
		return;
	}

	tower = (RunwayInfo *)AIGetClosestAirField(planepnt, planepnt->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, (20.0f * NMTOWU), 0);
	if((!tower) || ((PlayerPlane->OnGround) || ((PlayerPlane->HeightAboveGround * WUTOFT) < 10.0f)))  //  will need to make a function to check for nearby airport.  SRE
	{
		if(AICheckPlaneLandingFlyToCarrier(planepnt, &TempActionTakeOff) != 2)
		{
			return;
		}
	}

	AICAddSoundCall(AIC_WSOLandingRequestMsg, planepnt - Planes, delaycnt, 50);
	delaycnt = delaycnt + 5000;

	AICAddSoundCall(AICTowerLandingReplyMsg, planepnt - Planes, delaycnt, 50);
}

//**************************************************************************************
void AIC_WSOLandingRequestMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char towercallsign[256];
	long towernum, towervoice;
	int foundairfield = 0;
	FPointDouble checkpos;
	FPointDouble runwaypos;
	ANGLE runwayheading;
	TakeOffActionType TempActionTakeOff;

	if ((UFC.SteeringMode == TCN_STR) && (UFC.CurrentTacanPtr))
	{
		if(UFC.CurrentTacanPtr->iType == GROUNDOBJECT)
		{
			checkpos = UFC.TacanPos;
			if(AIGetClosestAirField(PlayerPlane, checkpos, &runwaypos, &runwayheading, &TempActionTakeOff, 50.0f * NMTOWU, 0))
			{
				foundairfield = 1;
			}
		}
	}

	if(!foundairfield)
	{
		if(AIGetClosestAirField(PlayerPlane, PlayerPlane->AI.WayPosition, &runwaypos, &runwayheading, &TempActionTakeOff, 50.0f * NMTOWU, 0))
		{
			foundairfield = 1;
		}
	}

	if(!foundairfield)
	{
		AIGetClosestAirField(PlayerPlane, PlayerPlane->WorldPosition, &runwaypos, &runwayheading, &TempActionTakeOff, -1.0f, 0);
	}

	Planes[planenum].AI.TargetPos.X = runwaypos.X;
	Planes[planenum].AI.TargetPos.Z = runwaypos.Z;
	//  Planes[planenum].AI.TargetPos.Y = Landing point from takeoff info.
	Planes[planenum].AI.TargetPos.Y = LandHeight(Planes[planenum].AI.TargetPos.X, Planes[planenum].AI.TargetPos.Z);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	towernum = AIGetTowerCallsign(planenum, &towervoice);
//	strcpy(towercallsign, CallSignList[towernum].sName);
	if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
	{
		return;
	}


	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_REQUEST_LANDING_2, g_iLanguageId, towercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(AICF_WSO_REQUEST_LANDING, planenum, towernum + 5, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICTowerLandingReplyMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char towercallsign[256];
	float marshallht;
	int ianglesht;
	long towernum, towervoice;
	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	towernum = AIGetTowerCallsign(planenum, &towervoice);
//	strcpy(towercallsign, CallSignList[towernum].sName);
	if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
	{
		return;
	}

	if(targetnum != -1)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TOWER_CLEAR_LAND_1, g_iLanguageId, callsign, towercallsign))
		{
			return;
		}
		AIRGenericSpeech(AICF_TOWER_CLEAR_LAND, towernum + 5, planenum, 0, 0, 0, 0, 0, 0, 0, towervoice);
		Planes[planenum].AI.iAIFlags2 |= AILANDING;
		AICReturnToBase(1, 8);
		AILetFormedOnLand(&Planes[planenum]);
	}
	else if(!AICheckMarshallPattern(&Planes[planenum]))
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TOWER_CLEAR_LAND_1, g_iLanguageId, callsign, towercallsign))
		{
			return;
		}
		AIRGenericSpeech(AICF_TOWER_CLEAR_LAND, towernum + 5, planenum, 0, 0, 0, 0, 0, 0, 0, towervoice);
		Planes[planenum].AI.iAIFlags2 |= AILANDING;
		AICReturnToBase(1, 8);
		AILetFormedOnLand(&Planes[planenum]);
	}
	else
	{
#if 0
		if(!LANGGetTransMessage(tempstr, 1024, AIC_TOWER_HOLD_ALTITUDE, g_iLanguageId, callsign, towercallsign))
		{
			return;
		}
#endif

		if((Planes[planenum].Status & PL_DEVICE_DRIVEN) && (!(Planes[planenum].AI.iAIFlags2 & AILANDING)))
		{
			AICAddSoundCall(AICCheckForFriendlyTraffic, planenum, 12000, 40, 0);
		}

		marshallht = AIGetMarshallHeight(&Planes[planenum]);
		Planes[planenum].AI.TargetPos.Y = marshallht;
		ianglesht = ((marshallht * WUTOFT) + 500) / 1000;
		AICAddSoundCall(AICTowerAnglesHoldMsg, planenum, 5000, 50, ianglesht);

		Planes[planenum].AI.iAIFlags2 |= AILANDING;
		iAIHumanLanding = 1;
		lAIHumanLandingTimer = 60000;
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
void AICTowerAnglesHoldMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char towercallsign[256];
	long towernum, towervoice;
	float altval;
	char altstr[128];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	towernum = AIGetTowerCallsign(planenum, &towervoice);
//	strcpy(towercallsign, CallSignList[towernum].sName);
	if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
	{
		return;
	}


#if 0
	char angelsstr[256];
	if(!LANGGetTransMessage(angelsstr, 256, AIC_ZERO + targetnum, g_iLanguageId))
	{
		return;
	}
#endif
	altval = targetnum;
	if(altval > 33)
	{
		altval = 33;
	}

	sprintf(altstr, "%d", altval);

	if(!LANGGetTransMessage(altstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_HOLD_TRAFFIC_ANGELS, g_iLanguageId, callsign, altstr))
	{
		return;
	}

	AICTowerAnglesHoldSnd(towernum + 5, planenum, towervoice, altval);

	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
void AICTowerAnglesHoldSnd(int towernum, int planenum, long towervoice, int holdalt)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_TOWER_HOLD_ALTITUDE, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, towernum, planenum, 0, 0, 0, 0, holdalt * 1000, 0, 0, towervoice);
		}
		else if(spchvar == 2)
		{
			if(numval == 3)
			{
//				sndids[numids] = (TOWER_ANGELS_10 - SPCH_TOWER1) + (holdalt - 10) + towervoice;
				sndids[numids] = (TOWER_ANGELS_10) + (holdalt - 10) + towervoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + towervoice;
			numids ++;
		}
	}
//	AIRSendSentence(planenum, numids, sndids);
	AIRSendSentence(planenum, numids, sndids, towervoice);
}

//**************************************************************************************
void AICSetUpPlayerCAS(int planenum, MBWayPoints *bombway)
{
	Planes[planenum].AI.iAIFlags2 |= AI_CAS_ATTACK;
	Planes[planenum].AI.LinkedPlane = AIGetLeader(&Planes[planenum]);
	Planes[planenum].AI.Behaviorfunc = AIFlyCAS;
	Planes[planenum].AI.OrgBehave = NULL;
	Planes[planenum].AI.iVar1 = 35;
	Planes[planenum].AI.pGroundTarget = NULL;
	Planes[planenum].AI.lDesiredSpeed = PlayerPlane->AI.CurrWay->iSpeed;
	if(Planes[planenum].AI.lDesiredSpeed < 450)
	{
		Planes[planenum].AI.lDesiredSpeed = 450;
	}
}

//**************************************************************************************
void AICheckInitialFACCalls(PlaneParams *planepnt)
{
	float dx, dz, tdist, tdistnm;

	dx = fpFACPosition.X - planepnt->WorldPosition.X;
	dz = fpFACPosition.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx, dz);
	tdistnm = (tdist * WUTONM);

	if((tdistnm < 20) && (iFACState == 1) && (lBombFlags & (WSO_FAC_CHECK)))
	{
		AIC_FAC_Contact_Msg(planepnt - Planes);
		lFACTimer = 20000;
		iFACState = 8;  //  2;
	}
#if 0
	else if((tdistnm < 10) && (iFACState == 2))
	{
		AIC_FAC_Request_Msg(planepnt - Planes);
		lFACTimer = 20000;
		iFACState = 3;
	}
#endif
}

//**************************************************************************************
void AIC_FAC_Contact_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char faccallsign[256];
	int facmsg, facsnd;
	int tempnum;
	long facnum, facvoice;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	facnum = AIGetFACCallsign(planenum, &facvoice);
	strcpy(faccallsign, CallSignList[facnum].sName);

	tempnum = rand() % 3;

	if(FACObject.lSide == 0)
	{
		facmsg = AIC_GROUND_CONTACT_US_1 + tempnum;
		facsnd = AICF_GROUND_CONTACT_US_1 + tempnum;
	}
	else
	{
		facmsg = AIC_GROUND_CONTACT_UK_1 + tempnum;
		facsnd = AICF_GROUND_CONTACT_UK_1 + tempnum;
	}

	if(!LANGGetTransMessage(tempstr, 1024, facmsg, g_iLanguageId, callsign, faccallsign))
	{
		return;
	}

	AIRGenericSpeech(facsnd, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, facvoice);

	AICAddAIRadioMsgs(tempstr, 50);

	AICAddSoundCall(AIC_WSO_FAC_Hang_In, facnum, 7000, 50, planenum);
	return;
}

//**************************************************************************************
void AIC_WSO_FAC_Hang_In(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int tempnum, msgnum, msgsnd;


	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	strcpy(callsign, CallSignList[planenum].sName);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	tempnum = rand() & 1;

	if(tempnum)
	{
		msgnum = AIC_WSO_HANG_IN_THERE;
		msgsnd = AICF_WSO_GRND_HANG_IN_THERE;
	}
	else
	{
		msgnum = AIC_WSO_ON_WAY;
		msgsnd = AICF_WSO_GRND_ON_WAY;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, playercallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}



//**************************************************************************************
void AIC_FAC_Request_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char faccallsign[256];
	char eheadingstr[128];
	float eheading;
	int facmsg, facsnd, tempnum;
	long facnum, facvoice;
	int headingval;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	facnum = AIGetFACCallsign(planenum, &facvoice);
	strcpy(faccallsign, CallSignList[facnum].sName);

	tempnum = rand() % 3;
	if(FACObject.lSide == 0)
	{
		facmsg = AIC_GROUND_NEED_HELP_1 + tempnum;
		facsnd = AICF_GROUND_NEED_HELP_1 + tempnum;
	}
	else
	{
		facmsg = AIC_GROUND_NEED_HELP_1 + tempnum;
		facsnd = AICF_GROUND_NEED_HELP_1 + tempnum;
	}

	eheading = AICheckForGroundEnemies(fpFACPosition, 5, NULL, AI_FRIENDLY);

	AICAddSoundCall(AIC_WSO_Grnd_Ack, facnum, 7000, 50, planenum);

	if(eheading <= -900)
	{
		if(iFACTargetPos == -1)
		{
			AIC_FAC_EnemyGone_Msg(PlayerPlane - Planes);
		}
		else
		{
			AIC_FAC_GoodJob_Msg(PlayerPlane - Planes);
		}
		iFACState = 4;
		return;
	}

	if(iFACTargetPos == -1)
	{
		iFACTargetPos = 0;
	}

	headingval = AICGetTextHeadingGeneral(eheadingstr, eheading);

	iFACTargetPos |= (1<<headingval);

	if(!LANGGetTransMessage(tempstr, 1024, facmsg, g_iLanguageId, callsign, faccallsign, eheadingstr))
	{
		return;
	}

	AIRFACHelpSnd(facsnd, facnum, planenum, headingval, facvoice);

	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
void AIRFACHelpSnd(int msgid, int facnum, int planenum, int headingval, long groundvoice)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, msgid, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, groundvoice);
		}
		else if(spchvar == 2)
		{
			if(numval == 4)
			{
				sndids[numids] = 215 + headingval + groundvoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + groundvoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids, groundvoice);
}

//**************************************************************************************
void AIC_FAC_NearMiss_Msg(int planenum, int targetnum)
{
	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_FACAck(planenum, (7 + (rand() & 3)));

	return;

#if 0
	char tempstr[1024];
	char callsign[256];
	int facmsg, facsnd;
	int tempnum;
	long facnum, facvoice;

	AIC_Get_Callsign_With_Number(planenum, callsign);
	facnum = AIGetFACCallsign(planenum, &facvoice);

	tempnum = rand() & 3;

	if(rand == 3)
	{
		facmsg = AIC_GROUND_FRIENDLY_FIRE_US_2;
		facsnd = AICF_GROUND_FRIENDLY_FIRE_US_2;
	}
	else
	{
		facmsg = AIC_GROUND_FRIENDLY_FIRE_US_4 + tempnum;
		facsnd = AICF_GROUND_FRIENDLY_FIRE_US_4 + tempnum;
	}

	if(!LANGGetTransMessage(tempstr, 1024, facmsg, g_iLanguageId, callsign))
	{
		return;
	}

	AIRGenericSpeech(facsnd, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, facvoice);

	AICAddAIRadioMsgs(tempstr, 50);
	return;
#endif
}

//**************************************************************************************
void AIC_FAC_Dead_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char faccallsign[256];
	int facmsg, facsnd;
	int tempnum;
	long facnum, facvoice;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	facnum = AIGetFACCallsign(planenum, &facvoice);
	strcpy(faccallsign, CallSignList[facnum].sName);

	tempnum = rand() & 1;
	if(FACObject.lSide == 0)
	{
		facmsg = AIC_GROUND_DEAD_US_1 + tempnum;
		facsnd = AICF_GROUND_DEAD_US_1 + tempnum;
	}
	else
	{
		facmsg = AIC_GROUND_DEAD_UK_1 + tempnum;
		facsnd = AICF_GROUND_DEAD_UK_1 + tempnum;
	}

	if(!LANGGetTransMessage(tempstr, 1024, facmsg, g_iLanguageId, callsign, faccallsign))
	{
		return;
	}

	AIRGenericSpeech(facsnd, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, facvoice);

	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
void AIC_FAC_GoodJob_Msg(int planenum, int targetnum)
{
	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_FACAck(planenum, (11 + (rand() % 3)));

#if 0
	char tempstr[1024];
	char callsign[256];
	int facmsg, facsnd;
	int tempnum;
	long facnum, facvoice;

	AIC_Get_Callsign_With_Number(planenum, callsign);
	facnum = AIGetFACCallsign(planenum, &facvoice);

	tempnum = rand() % 3;
	if(FACObject.lSide == 0)
	{
		facmsg = AIC_GROUND_GOOD_US_1 + tempnum;
		facsnd = AICF_GROUND_GOOD_US_1 + tempnum;
	}
	else
	{
		facmsg = AIC_GROUND_GOOD_UK_1 + tempnum;
		facsnd = AICF_GROUND_GOOD_UK_1 + tempnum;
	}

	if(!LANGGetTransMessage(tempstr, 1024, facmsg, g_iLanguageId, callsign))
	{
		return;
	}

	AIRGenericSpeech(facsnd, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, facvoice);

	AICAddAIRadioMsgs(tempstr, 50);
	return;
#endif
}

//**************************************************************************************
void AIC_FAC_EnemyGone_Msg(int planenum, int targetnum)
{
	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_FACAck(planenum, 14);

#if 0
	char tempstr[1024];
	char callsign[256];
	int facmsg, facsnd;
	long facnum, facvoice;

	AIC_Get_Callsign_With_Number(planenum, callsign);
	facnum = AIGetFACCallsign(planenum, &facvoice);

	if(FACObject.lSide == 0)
	{
		facmsg = AIC_GROUND_ENEMY_GONE_US;
		facsnd = AICF_GROUND_ENEMY_GONE_US;
	}
	else
	{
		facmsg = AIC_GROUND_ENEMY_GONE_UK;
		facsnd = AICF_GROUND_ENEMY_GONE_UK;
	}

	if(!LANGGetTransMessage(tempstr, 1024, facmsg, g_iLanguageId, callsign))
	{
		return;
	}

	AIRGenericSpeech(facsnd, facnum, planenum, 0, 0, 0, 0, 0, 0, 0, facvoice);

	AICAddAIRadioMsgs(tempstr, 50);
	return;
#endif
}

//**************************************************************************************
float AICheckForGroundEnemies(FPoint centerpos, int distnm, int *founddistnm, int searchside, int facnum, float *pmindist, int sameside)
{
  	BasicInstance *checkobject;
	MovingVehicleParams *vehiclepnt;
	float foundrange;
  	void *foundobject = NULL;
	int foundtype = -1;
	float offangle;
	float dx, dz, tdist;
	int enemyval;
	int pass;
	float mindist = 0;
	FPointDouble position;
	if(pmindist)
		mindist = *pmindist;

	enemyval = AI_ENEMY;

	foundrange = (distnm * NMTOWU);

	BasicInstance ***checklist = &AllTargetTypes[0];
	pass = NumTargetTypes;

	while(pass--)
	{
		checkobject = **checklist++;

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject) && ((GDConvertGrndSide(checkobject) != searchside) || ((GDConvertGrndSide(checkobject) == searchside) && (sameside))))
			{
				dx = (checkobject->Position.X) - centerpos.X;
				dz = (checkobject->Position.Z) - centerpos.Z;
				if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
				{
					tdist = QuickDistance(dx, dz);
					if((tdist < foundrange) && (tdist > mindist))
					{
						foundobject = checkobject;
						foundtype = GROUNDOBJECT;
						foundrange = tdist;
						position = checkobject->Position;
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))) && (!(vehiclepnt->Status & (VL_INVISIBLE))) && ((searchside != vehiclepnt->iSide) || ((searchside != vehiclepnt->iSide) && (sameside))))
		{
			dx = (vehiclepnt->WorldPosition.X) - centerpos.X;
			dz = (vehiclepnt->WorldPosition.Z) - centerpos.Z;
			if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
			{
				tdist = QuickDistance(dx, dz);
				if((tdist < foundrange) && (tdist > mindist))
				{
					foundobject = vehiclepnt;
					foundtype = MOVINGVEHICLE;
					foundrange = tdist;
					position = vehiclepnt->WorldPosition;
				}
			}
		}
	}

	if((facnum == -2) && (!sameside))
	{
		if((pFACTarget != foundobject) && (MultiPlayer) && (g_bIAmHost))
		{
			DWORD	objectid;
			switch(iFACTargetType)
			{
				case AIRCRAFT:
					objectid = (PlaneParams *)pFACTarget - Planes;
					break;
				case MOVINGVEHICLE:
				case SHIP:
					objectid = (MovingVehicleParams *)pFACTarget - MovingVehicles;
					break;
				case GROUNDOBJECT:
					checkobject = (BasicInstance *)pFACTarget;
					objectid = checkobject->SerialNumber;
					break;
			}
			NetPutGenericMessageObjectPoint(PlayerPlane, GMOP_FAC_TARG, iFACTargetType, objectid);
		}
		pFACTarget = foundobject;
		iFACTargetType = foundtype;
	}

	if(foundobject == NULL)
	{
		if((mindist > 1.0f) && (!sameside))
		{
			*pmindist = 0;
			return(AICheckForGroundEnemies(centerpos, distnm, founddistnm, searchside, facnum, pmindist, sameside));
		}

		return(-999);
	}
	dx = (position.X) - centerpos.X;
	dz = (position.Z) - centerpos.Z;

	offangle = atan2(-dx, -dz) * 57.2958;

	if(founddistnm != NULL)
	{
		*founddistnm = (foundrange * WUTONM);
	}
	if(pmindist)
		*pmindist = foundrange + (5 * FTTOWU);

	return(offangle);
}

//**************************************************************************************
void AIReCheckingForGroundEnemies(FPoint centerpos, int distnm, int *founddistnm, int searchside, int validpos)
{
  	BasicInstance *checkobject;
	float foundrange;
  	BasicInstance *foundobject = NULL;
	float offangle;
	float dx, dz, tdist;
	int enemyval;
	int pass;
	char eheadingstr[128];
	int headingval = 0;
	MovingVehicleParams *vehiclepnt;

	lFACTimer = 60000;

	if(pFACTarget)
	{
		if(iFACTargetType == GROUNDOBJECT)
		{
			if(InstanceIsBombable((BasicInstance *)pFACTarget))
			{
				return;
			}
		}
		else
		{
			vehiclepnt = (MovingVehicleParams *)pFACTarget;
			if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))) && (!(vehiclepnt->Status & (VL_INVISIBLE))) && (searchside != vehiclepnt->iSide))
			{
				return;
			}
		}
		AICheckForGroundEnemies(fpFACPosition, 10, NULL, AI_FRIENDLY, -2, &fFACRange);
	}

	if(pFACTarget)
		return;

	enemyval = AI_ENEMY;

	foundrange = (distnm * NMTOWU);

	BasicInstance ***checklist = &AllTargetTypes[0];
	pass = NumTargetTypes;

	while(pass--)
	{
		checkobject = **checklist++;

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject) && (GDConvertGrndSide(checkobject) != searchside))
			{
				dx = (checkobject->Position.X) - centerpos.X;
				dz = (checkobject->Position.Z) - centerpos.Z;
				if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
				{
					tdist = QuickDistance(dx, dz);
					if(tdist < foundrange)
					{
						offangle = atan2(-dx, -dz) * 57.2958;
						headingval = AICGetTextHeadingGeneral(eheadingstr, offangle);
						if(iFACTargetPos & (1<<headingval))
						{
							foundobject = checkobject;
							foundrange = tdist;
						}
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	if(foundobject == NULL)
	{
		AIC_FAC_Request_Msg(PlayerPlane - Planes);
	}
}

//**************************************************************************************
void AICDoAltPathMsgs(int planenum)
{
	AICAddSoundCall(AIC_AWACS_Task_Msg, planenum, 0, 50);
	AICAddSoundCall(AIC_AWACS_Task2_Msg, planenum, 10000, 50);
}

//**************************************************************************************
void AIC_AWACS_Task_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(!AICAllowThisRadio(awacsnum, 1))
	{
		return;
	}

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_STANDBY_1, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(AICF_AWACS_STANDBY, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	return;
}

//**************************************************************************************
void AIC_AWACS_Task2_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int facnum;
	int msgnum, msgsnd;
	int tempnum;
	FPoint facloc;
	MBWayPoints *waypnt;




	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	waypnt = Planes[planenum].AI.CurrWay;

	Planes[planenum].AI.WayPosition.X = ConvertWayLoc(waypnt->lWPx);  //  Only if boarder   + (384.0 * TILE_SIZE);
	Planes[planenum].AI.WayPosition.Z = ConvertWayLoc(waypnt->lWPz);  //  Only if boarder   + (384.0 * TILE_SIZE);
	if(waypnt->lWPy > 0)
	{
		Planes[planenum].AI.WayPosition.Y = ConvertWayLoc(waypnt->lWPy);
	}
	else if(AIIsTakeOff(waypnt))
	{
		Planes[planenum].AI.WayPosition.Y = ConvertWayLoc(8000);
	}
	else
	{
		Planes[planenum].AI.WayPosition.Y = LandHeight(Planes[planenum].AI.WayPosition.X, Planes[planenum].AI.WayPosition.Z) + ConvertWayLoc(labs(waypnt->lWPy)) + (7.0 * FOOT_MULTER);
	}

	if(&Planes[planenum] == PlayerPlane)
	{
		UFCAdvanceWaypointVisual();
	}


	AIC_Get_Callsign_With_Number(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum >= 0)
	{
		if(!AICAllowThisRadio(awacsnum, 1))
		{
			return;
		}
	}

	facnum = AICheckForFAC(&facloc);

	if(facnum)
	{
		tempnum = rand() & 1;
		if(tempnum)
		{
			msgnum = AIC_AWACS_GROUND_FAC;
			msgsnd = AICF_AWACS_GROUND_FAC;
		}
		else
		{
			msgnum = AIC_AWACS_SUPPORT_GROUND;
			msgsnd = AICF_AWACS_SUPPORT_GROUND;
		}
		iFACState = 1;
		iFACTargetPos = -1;
		lFACTimer = 10000;
		fpFACPosition = facloc;
		AICAddSoundCall(AIC_WSO_New_Path_Msg, planenum, 10000, 50, 1);
	}
	else
	{
		msgnum = AIC_AWACS_NEW_COORDINATES;
		msgsnd = AICF_AWACS_NEW_COORDINATES;
		AICAddSoundCall(AIC_WSO_New_Path_Msg, planenum, 10000, 50, 0);
	}

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	return;
}

//**************************************************************************************
void AIC_WSO_New_Path_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
//	int tempnum;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(targetnum)
	{
		msgnum = AIC_WSO_FAC_MARKED;
		msgsnd = AICF_WSO_FAC_MARKED;
	}
	else
	{
		msgnum = AIC_WSO_NEW_STEERING_READY;
		msgsnd = AICF_WSO_NEW_STEERING_READY;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	return;
}

//**************************************************************************************
int AICheckForFAC(FPoint *facloc)
{
	BasicInstance *walker;

	if(FACObject.iType == -1)
	{
		return(0);
	}

	if((facloc != NULL) && (iFACState == 0))
	{
		walker = FindInstance(AllInstances, FACObject.dwSerialNumber);
		facloc->X = ConvertWayLoc(FACObject.lX);
		facloc->Z = ConvertWayLoc(FACObject.lZ);
		if(walker)
		{
			facloc->Y = walker->Position.Y;
		}
		else
		{
			facloc->Y = LandHeight(facloc->X, facloc->Z);
		}
	}
	else if(facloc)
	{
		*facloc = fpFACPosition;
	}
	return(1);
}


//**************************************************************************************
void AIC_On_Station_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if((GameLoop / 50) < 120)
	{
		return;
	}

//	if(!((Planes[planenum].AI.lAIVoice == SPCH_CAP1) || (Planes[planenum].AI.lAIVoice == SPCH_CAP2) || (Planes[planenum].AI.lAIVoice == SPCH_CAP3) || (Planes[planenum].AI.lAIVoice == SPCH_SEAD1) || (Planes[planenum].AI.lAIVoice == SPCH_SEAD2)))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum == planenum)
		return;

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_ON_STATION, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(AICF_SEAD_ON_STATION, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Beginning_Human_Escort_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char escortcallsign[256];
	int msgnum, msgsnd, tempnum;
	long voice;
	int placeingroup;

	if(lNoRepeatFlags[planenum] & FORMING_UP_SAID)
	{
		return;
	}

	lNoRepeatFlags[planenum] |= FORMING_UP_SAID;
	if(lNoRepeatTimer[planenum] < 60000)
	{
		lNoRepeatTimer[planenum] = 60000;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(targetnum, escortcallsign);

	voice = Planes[planenum].AI.lAIVoice;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, voice, placeingroup))
	{
		return;
	}

	tempnum = rand()&1;
	if(tempnum)
	{
		msgnum = AIC_CAP_FORM_PLAYER;
		msgsnd = AICF_CAP_FORM_PLAYER;
	}
	else
	{
		msgnum = AIC_CAP_WITH_YOU;
		msgsnd = AICF_CAP_WITH_YOU;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, escortcallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICRequestPlayerSEAD()
{
	AICRequestSEAD(PlayerPlane);
}

//**************************************************************************************
void AICRequestSEAD(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	int awacsnum;
	int status;

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
	{
  		if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			status = MovingVehicles[planepnt->AI.iHomeBaseId].Status;
			if((!status) || (status & VL_SINKING))
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

//	if(!AICAllowThisRadio(awacsnum, 1))
//	{
//		return;
//	}

	foundrange = (ASSET_RANGE * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (planepnt->AI.iSide == checkplane->AI.iSide) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if(checkplane->AI.Behaviorfunc == AIFlyOrbit)
				{
					if(AIIsSEADPlane(checkplane))  //  May want to check to see if the plane is supposed to cover someone else further on in his waypoint list.
					{
						dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
						dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
						dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
						tdist = QuickDistance(dx, dz);
						if(tdist < foundrange)
						{
							foundrange = tdist;
							foundplane = checkplane - Planes;
						}
					}
				}
			}
		}
		checkplane ++;
	}
	if(foundplane == -1)
	{
		AICAddSoundCall(AIC_AWACS_Acknowledged, planepnt - Planes, 1000, 50, awacsnum);
		AICAddSoundCall(AICNoWeasels, planepnt - Planes, 10000, 50, awacsnum);
	}
	else
	{
		AICAddSoundCall(AIC_AWACS_Acknowledged, planepnt - Planes, 1000, 50, awacsnum);
		AICAddSoundCall(AICWeaselsInBound, planepnt - Planes, 10000, 50, awacsnum);
		AICAddSoundCall(AICWeaselsOnWay, planepnt - Planes, 25000, 50, foundplane);
		AISetUpEscort(&Planes[foundplane], planepnt - Planes);
	}
}

//**************************************************************************************
void AICNoWeasels(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int voice = 0;

	AICGetCallSign(planenum, callsign);

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_NO_WEASELS_1, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_NO_WEASELS, g_iLanguageId, callsign))
		{
			return;
		}
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_NO_WEASELS, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AICWeaselsInBound(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int voice = 0;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_WEASELS_1, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_WEASELS_INBOUND, g_iLanguageId, callsign))
		{
			return;
		}
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_WEASELS, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AICWeaselsOnWay(int planenum, int weaselnum)
{
	char tempstr[1024];
	char callsign[256];
	char weaselcallsign[256];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(weaselnum >= 0)
	{
		AICGetCallSign(weaselnum, weaselcallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_SEAD_ON_WAY, g_iLanguageId, callsign, weaselcallsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SEAD_ON_WAY, weaselnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICRequestPlayerCover()
{
	AICRequestCover(PlayerPlane);
}

//**************************************************************************************
void AICRequestCover(PlaneParams *planepnt)
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	int awacsnum;
	int status;

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
	{
  		if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			status = MovingVehicles[planepnt->AI.iHomeBaseId].Status;
			if((!status) || (status & VL_SINKING))
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

//	if(!AICAllowThisRadio(awacsnum, 1))
//	{
//		return;
//	}

	foundrange = (80 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if (((checkplane->Status & PL_ACTIVE) || (checkplane->AI.iAIFlags2 & AIINVISIBLE)) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (planepnt->AI.iSide == checkplane->AI.iSide) && (!((pDBAircraftList[checkplane->AI.iPlaneIndex].lType & (AIRCRAFT_TYPE_CIVILIAN|AIRCRAFT_TYPE_C3)) || (checkplane->AI.iAIFlags2 & AI_SO_JAMMING))))
		{
			if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
			{
				if((AIIsCAPing(checkplane)) || (checkplane->AI.Behaviorfunc == AIFlySweep) || (checkplane->AI.iAIFlags2 & AIINVISIBLE))
				{
					dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if(tdist < foundrange)
					{
						foundrange = tdist;
						foundplane = checkplane - Planes;
					}
				}
			}
		}
		checkplane ++;
	}
	if(foundplane == -1)
	{
		AICAddSoundCall(AIC_AWACS_Acknowledged, planepnt - Planes, 1000, 50, awacsnum);
		AICAddSoundCall(AICNoCover, planepnt - Planes, 10000, 50, awacsnum);
	}
	else
	{
		AICAddSoundCall(AIC_AWACS_Acknowledged, planepnt - Planes, 1000, 50, awacsnum);
		AICAddSoundCall(AICCoverInBound, planepnt - Planes, 10000, 50, awacsnum);
		AICAddSoundCall(AICCoverOnWay, planepnt - Planes, 25000, 50, foundplane);
		checkplane = &Planes[foundplane];

		if(!(planepnt->AI.iAIFlags2 & AIINVISIBLE))
		{
			AIReplaceCAPorSweep(checkplane);
		}

		AISetUpEscort(&Planes[foundplane], planepnt - Planes);
		if(checkplane->AI.iAIFlags2 & AIINVISIBLE)
		{
			checkplane->AI.iAIFlags2 &= ~AIINVISIBLE;
			checkplane->Status |= PL_ACTIVE;
			if(checkplane->OnGround)
			{
				checkplane->AI.OrgBehave = checkplane->AI.Behaviorfunc;
				checkplane->AI.Behaviorfunc = AIFlyTakeOff;
			}
		}
	}
}

//**************************************************************************************
void AICNoCover(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int voice = 0;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_NO_FIGHTERS_1, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_NO_ASSETS, g_iLanguageId, callsign))
		{
			return;
		}
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_NO_FIGHTERS, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AICCoverInBound(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int voice = 0;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_VECTORING_FIGHTERS_1, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_STRIKE_VECTORING_FIGHTERS, g_iLanguageId, callsign))
		{
			return;
		}
		voice = SPCH_STRIKE;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_VECTORING_FIGHTERS, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AICCoverOnWay(int planenum, int covernum)
{
	char tempstr[1024];
	char callsign[256];
	char covercallsign[256];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(covernum >= 0)
	{
		AICGetCallSign(covernum, covercallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_CAP_ON_WAY, g_iLanguageId, callsign, covercallsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_CAP_ON_WAY, covernum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_On_Way_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(awacsnum >= 0)
	{
		tempnum = rand() % 3;
		if(tempnum == 0)
		{
			msgnum = AIC_SAR_INBOUND;
			msgsnd = AICF_SAR_INBOUND;
		}
		else if(tempnum == 1)
		{
			msgnum = AIC_SAR_ON_WAY;
			msgsnd = AICF_SAR_ON_WAY;
		}
		else
		{
			msgnum = AIC_SAR_SCRAMBLE;
			msgsnd = AICF_SAR_SCRAMBLE;
		}

		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_Approach_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum >= 0)
	{
		tempnum = rand() % 3;
		if(tempnum == 0)
		{
			msgnum = AIC_SAR_MOVING_IN_LZ;
			msgsnd = AICF_SAR_MOVING_IN_LZ;
		}
		else if(tempnum == 1)
		{
			msgnum = AIC_SAR_ADVISED_MOVING_IN;
			msgsnd = AICF_SAR_ADVISED_MOVING_IN;
		}
		else
		{
			msgnum = AIC_SAR_APPROACHING_LZ;
			msgsnd = AICF_SAR_APPROACHING_LZ;
		}

		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_Land_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum >= 0)
	{
		tempnum = rand() % 3;
		if(tempnum == 0)
		{
			msgnum = AIC_SAR_DOWN_AT_LZ;
			msgsnd = AICF_SAR_DOWN_AT_LZ;
		}
		else if(tempnum == 1)
		{
			msgnum = AIC_SAR_NOW_AT_LZ;
			msgsnd = AICF_SAR_NOW_AT_LZ;
		}
		else
		{
			msgnum = AIC_SAR_TOUCHING_LZ;
			msgsnd = AICF_SAR_TOUCHING_LZ;
		}

		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_Success_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	AICGetCallSign(planenum, callsign);
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum >= 0)
	{
		tempnum = rand() & 1;
		if(tempnum == 0)
		{
			msgnum = AIC_SAR_PICKUP_OK_NAVY;
			msgsnd = AICF_SAR_PICKUP_OK;
		}
		else
		{
			msgnum = AIC_SAR_GOT_EM;
			msgsnd = AICF_SAR_GOT_EM;
		}

		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_Fail_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum >= 0)
	{
		tempnum = rand() & 1;
		if(tempnum == 0)
		{
			msgnum = AIC_SAR_NO_FIND;
			msgsnd = AICF_SAR_NO_FIND;
		}
		else
		{
			msgnum = AIC_SAR_NO_FRIENDLIES_NAVY;
			msgsnd = AICF_SAR_NO_FRIENDLIES;
		}

		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//******  STOPPPED LOOKING FOR SPEACH SRE  ********/
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************

//**************************************************************************************
void AICCheckForHitMsgs(PlaneParams *planepnt, PlaneParams *shooter, int orgflightstat, WeaponParams *W)
{
	int wingman;
	int shooterside = AI_ENEMY;
	int shooterplayer = 0;
	int shooterleadplayer = 0;

#if 0
	if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
	{
		if(SndStreamInit("kenny.wav"))
		{
			SndStreamPlay();
		}
	}
#endif
	if(shooter)
	{
		if((shooter == PlayerPlane) && (planepnt->AI.iSide == shooter->AI.iSide))
		{
			if((pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)].iWeaponType != WEAPON_TYPE_GUN) || (!(iPlayerBombFlags & WSO_BULLET_HIT)))
			{
//		 		AIC_WSO_Wise_Ass_Msg(shooter - Planes, planepnt - Planes);
				if(pDBWeaponList[AIGetWeaponIndex(W->Type->TypeNumber)].iWeaponType == WEAPON_TYPE_GUN)
				{
					iPlayerBombFlags |= WSO_BULLET_HIT;
				}
			}
		}
		shooterside = shooter->AI.iSide;
		if(shooter == PlayerPlane)
		{
			shooterplayer = 1;
			shooterleadplayer = 1;
		}
		else if(AIGetLeader(shooter) == PlayerPlane)
		{
			shooterleadplayer = 1;
		}

	}

	if((shooter == PlayerPlane) && (planepnt->AI.iSide != shooterside))
	{
		if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
		{
//			AICAddSoundCall(AIC_WSO_Kill_Msg, shooter - Planes, 1500, 50, planepnt - Planes);
			wingman = AICCheckForWingmanInVisualRange(shooter, planepnt, 1);

			if(wingman != -1)
			{
				AICAddSoundCall(AICWingmanConfirmKill, shooter - Planes, 7000 + ((rand() % 5) * 1000), 50, wingman);
			}
		}
	}
	else if((shooterleadplayer) && (planepnt->AI.iSide != shooter->AI.iSide))
	{
		if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
		{
			AICAddSoundCall(AIC_Wingman_Kill_Msg, shooter - Planes, 1500, 50, planepnt - Planes);

			wingman = AICCheckForWingmanInVisualRange(shooter, planepnt, 1);

			if((wingman != -1) && (&Planes[wingman] != PlayerPlane))
			{
				AICAddSoundCall(AICWingmanConfirmKill, shooter - Planes, 7000 + ((rand() % 5) * 1000), 50, wingman);
			}
		}
	}
	else if(PlayerPlane->AI.iSide == planepnt->AI.iSide)
	{
		if((planepnt->AI.lAIVoice == SPCH_TANK_USN) || (planepnt->AI.lAIVoice == SPCH_TANK_USMC))
		{
			if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
			{
			 	AICAddSoundCall(AICTankerDeadMsg, planepnt - Planes, ((rand() % 3) * 1000), 50);
			}
		}
		else if(planepnt->AI.lAIVoice == SPCH_SAR1)
		{
			if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
			{
			 	AICAddSoundCall(AICSARDeadMsg, planepnt - Planes, 2000 + ((rand() % 3) * 1000), 50);
			}
		}
		else if(planepnt->AI.lAIVoice == SPCH_JSTARS1)
		{
			if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
			{
			 	AICAddSoundCall(AICJSTARSDeadMsg, planepnt - Planes, 2000 + ((rand() % 3) * 1000), 50);
			}
		}
		else if((planepnt->AI.lAIVoice == SPCH_AWACS1) || (planepnt->AI.lAIVoice == SPCH_AWACS2))
		{
			if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
			{
			 	AICAddSoundCall(AICAWACSDeadMsg, planepnt - Planes, 2000 + ((rand() % 3) * 1000), 50);
			}
		}
		else if(planepnt != PlayerPlane)
		{
			if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
			{
			 	AICAddSoundCall(AICCAPDeadMsg, planepnt - Planes, 2000 + ((rand() % 3) * 1000), 50);
			}
		}
	}
	else if(shooterside == PlayerPlane->AI.iSide)
	{
		if(shooter)
		{
			if(((shooter->AI.lAIVoice >= SPCH_WNG1) && (shooter->AI.lAIVoice <= SPCH_NAVY5)) || ((shooter->AI.lAIVoice >= SPCH_FTR_USAF) && (shooter->AI.lAIVoice <= SPCH_FTR_RUS)))
			{
				if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (!(orgflightstat & PL_OUT_OF_CONTROL)))
				{
					AICAddSoundCall(AIC_CAP_Kill_Msg, shooter - Planes, 1500, 50, planepnt - Planes);
				}
			}
		}
	}
}

//**************************************************************************************
void AICDelayedPlayerDamageMsgs(int system_diffs, int weapon_index)
{
	if(PlayerPlane->FlightStatus & PL_OUT_OF_CONTROL)
	{
		return;
	}

	if(weapon_index < 0)
	{
		AICCheckForDamageMsgs(PlayerPlane, 0, system_diffs, NULL, NULL, NULL);
	}
	else
	{
		AICCheckForDamageMsgs(PlayerPlane, 0, system_diffs, NULL, NULL, &pDBWeaponList[weapon_index]);
	}
}

//**************************************************************************************
void AICCheckForDamageMsgs(PlaneParams *planepnt, int orgflightstat, int failed_system, WeaponParams *weaponpnt, AAAStreamVertex *AAA, DBWeaponType *pweapon_type)
{
	int kind;
	int wingman;
	long delaycnt;
	int skipspeech;
	int weapontype;
	int checkweapon = 1;

	if(orgflightstat & PL_OUT_OF_CONTROL)
	{
		return;
	}

	if(!(planepnt->FlightStatus & PL_OUT_OF_CONTROL))
	{
		int planenum = planepnt - Planes;
		if(lNoRepeatFlags[planenum] & WE_HIT_SAID)
		{
			return;
		}
		lNoRepeatFlags[planenum] |= WE_HIT_SAID;
		if(lNoRepeatTimer[planenum] < 60000)
		{
			lNoRepeatTimer[planenum] = 60000;
		}
	}

	if((weaponpnt == NULL) && (pweapon_type == NULL) && (AAA == NULL))
	{
		kind = NONE;
		checkweapon = 0;
	}
	else if(weaponpnt != NULL)
	{
		kind = weaponpnt->Kind;
		if(kind == BULLET)
		{
			if(weaponpnt->LauncherType == GROUNDOBJECT)
			{
				kind = -1;
			}
		}
	}
	else if(pweapon_type)
	{
		weapontype = pweapon_type->iWeaponType;
		if(weapontype == WEAPON_TYPE_GUN)
		{
			if((pweapon_type->iSeekerType == 8) || (pweapon_type->iSeekerType == 9) || (pweapon_type->iSeekerType == 10))
			{
				kind = -1;
			}
			else
			{
				kind = BULLET;
			}
		}
		else if((weapontype == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AA_LONG_RANGE_MISSILE) || (weapontype == WEAPON_TYPE_AG_MISSILE) || (weapontype == WEAPON_TYPE_SCUD) || (weapontype == WEAPON_TYPE_SAM) || (weapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
		{
			kind = MISSILE;
		}
		else if((weapontype == WEAPON_TYPE_DUMB_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_CLUSTER_BOMB))
		{
			kind = BOMB;
		}
		else
		{
			kind = NONE;
		}
	}
	else if(AAA)
	{
		kind = -1;
	}
	else
	{
		kind = NONE;
	}

	if((kind == NONE) && (pweapon_type))
	{
		if((pweapon_type->iSeekerType == 8) || (pweapon_type->iSeekerType == 9) || (pweapon_type->iSeekerType == 10))
		{
			kind = -1;
		}
	}

	if(planepnt == PlayerPlane)
	{
		if(checkweapon)
		{
			AIC_WSO_Hit_Msg(planepnt - Planes, kind);
		}
		else if((orgflightstat & PL_STATUS_CRASHED) != (failed_system & PL_STATUS_CRASHED))
		{
			AIC_WSO_Hit_Msg(planepnt - Planes, -999);
		}

		if(planepnt->FlightStatus & PL_OUT_OF_CONTROL)
		{
#ifdef NO_GRND_DEF_DAMAGE
			if(weaponpnt)
			{
				if(weaponpnt->LauncherType == GROUNDOBJECT)
				{
					planepnt->FlightStatus &= ~PL_OUT_OF_CONTROL;
					if (planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT])
					{
						((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT])->RemoveSmokeTrail();
						planepnt->Smoke[SMOKE_ENGINE_REAR_LEFT] = NULL;
					}
					if (planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT])
					{
						((SmokeTrail *)planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT])->RemoveSmokeTrail();
						planepnt->Smoke[SMOKE_ENGINE_REAR_RIGHT] = NULL;
					}
					planepnt->SystemsStatus &= ~(PL_ENGINE_REAR_RIGHT|PL_ENGINE_REAR_LEFT);
					return;
				}
			}
#endif
			delaycnt = 0;
			wingman = AICCheckForWingmanInVisualRange(planepnt, NULL, 0);
			if(wingman != -1)
			{
				if(AIRGetChannel(wingman) == CHANNEL_WINGMEN)
				{
					delaycnt += 3000;
					AICAddSoundCall(AIC_Eject_Wingman_Msg, planepnt - Planes, delaycnt, 50, wingman);
				}
			}
		}
	}
	else if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
	{
		if(checkweapon)
		{
			if((kind == BULLET) || (kind == -1))
			{
				if(lNoOtherBulletHitMsgs < 0)
				{
					AIC_Wingman_Hit_Msg(planepnt - Planes, kind);
					lNoOtherBulletHitMsgs = 10000;
				}
			}
			else
			{
				AIC_Wingman_Hit_Msg(planepnt - Planes, kind);
			}
		}

		if(planepnt->FlightStatus & PL_OUT_OF_CONTROL)
		{
			delaycnt = 0;
			wingman = AICCheckForWingmanInVisualRange(planepnt, NULL, 0);
			if(wingman != -1)
			{
				if(AIRGetChannel(wingman) == CHANNEL_WINGMEN)
				{
					delaycnt += 3000;
					AICAddSoundCall(AIC_Eject_Wingman_Msg, planepnt - Planes, delaycnt, 50, wingman);
				}
			}
			delaycnt += 3000 + ((rand() % 5) * 1000);
			AICAddSoundCall(AIC_Wingman_Ejecting_Msg, planepnt - Planes, delaycnt, 50);

#if 0
			if((!(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (PlayerPlane->Status & PL_ACTIVE))
			{
				delaycnt += 7000 + ((rand() % 5) * 1000);
				AICAddSoundCall(AIC_Wingman_Ejected_Msg, planepnt - Planes, delaycnt, 50, PlayerPlane - Planes);
			}
#endif
		}
	}
	else
	{
		if(WIsWeaponPlane(planepnt))
		{
			return;
		}

		if(((kind == BULLET) || (kind == -1)) && (checkweapon))
		{
			if(lNoOtherBulletHitMsgs < 0)
			{
				skipspeech = 0;
				lNoOtherBulletHitMsgs = 10000;
			}
		}
		else
		{
			skipspeech = 1;
		}

		if((skipspeech == 0) && ((planepnt->AI.lAIVoice == SPCH_SAR1) || (planepnt->AI.lAIVoice == SPCH_SAR2)))
		{
			AIC_SAR_Hit_Msg(planepnt - Planes, kind);
		}
		else if((skipspeech == 0) && ((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1)))
		{
			AICBomberHitMsg(planepnt - Planes, kind);
		}

		if((planepnt->FlightStatus & PL_OUT_OF_CONTROL) && (planepnt->AI.lPlaneID != 30) && (!MultiPlayer))
		{

			AICAddSoundCall(BeginEjectionSequence, planepnt - Planes, 1000 + (rand() & 1023), 50, -1);
//			AICheckForSAR(planepnt);
		}
	}
}

//**************************************************************************************
void AIC_WSO_Kill_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(pDBAircraftList[Planes[targetnum].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		tempnum = rand() % 6;
	}
	else
	{
		tempnum = rand() % 5;
	}

	if(tempnum == 1)
	{
		msgnum = AIC_WSO_KILL;
		msgsnd = AICF_WSO_KILL;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_WSO_GOT_HIM;
		msgsnd = AICF_WSO_GOT_HIM;
	}
	else if(tempnum == 3)
	{
		msgnum = AIC_WSO_HISTORY;
		msgsnd = AICF_WSO_HISTORY;
	}
	else if(tempnum == 4)
	{
		msgnum = AIC_WSO_SPLASH;
		msgsnd = AICF_WSO_SPLASH;
	}
	else if(tempnum == 5)
	{
		msgnum = AIC_WSO_SPLASH_HELO;
		msgsnd = AICF_WSO_SPLASH_HELO;
	}
	else
	{
		msgnum = AIC_WSO_SPLASH_BANDIT;
		msgsnd = AICF_WSO_SPLASH_BANDIT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_Kill_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum;
	char callsign[256];
	char killtxt[256];
	int killnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	killnum = 0;  //  This will need to call a function to get this number sometime.

	if((killnum > 1) && (killnum < 9))
	{
		tempnum = rand() % 3;
	}
	else
	{
		tempnum = rand() & 1;
	}

	if(tempnum == 1)
	{

		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_RUSF_SPLASH_1;
		}
		else
		{
			msgnum = AIC_WING_SPLASH_1;
		}
		msgsnd = AICF_WING_SPLASH_1;
	}
	else if(tempnum == 2)
	{
		//  killnum = number of kills (talk to Chris about this //  SRE)
		killnum = 1;

		if(!LANGGetTransMessage(killtxt, 256, AIC_ZERO + killnum, g_iLanguageId))
		{
			return;
		}

		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			if(!LANGGetTransMessage(tempstr, 1024, AIC_RUSF_SPLASH_2, g_iLanguageId, callsign, killtxt))
			{
				return;
			}
		}
		else
		{
			if(!LANGGetTransMessage(tempstr, 1024, AIC_WING_SPLASH_2, g_iLanguageId, callsign, killtxt))
			{
				return;
			}
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRWingNumberKillSnd(AICF_WING_SPLASH_2, planenum, killnum);
		return;
	}
	else
	{
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_RAF_SPLASH_3;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_RUSF_SPLASH_3;
		}
		else
		{
			msgnum = AIC_WING_SPLASH_3;
		}
		msgsnd = AICF_WING_SPLASH_3;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIRWingNumberKillSnd(int msgsnd, int planenum, int killnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(channel != CHANNEL_WINGMEN)
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgsnd, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (SPCH_COUNT_ONE + killnum - 1) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AIC_WSO_Wise_Ass_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tempnum = rand() & 1;

	if(tempnum == 1)
	{
		msgnum = AIC_WSO_THAT_WAS_STUPID;
		msgsnd = AICF_WSO_THAT_WAS_STUPID;
	}
	else
	{
		msgnum = AIC_WSO_WHAT_HELL_WRONG;
		msgsnd = AICF_WSO_WHAT_HELL_WRONG;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Hit_Msg(int planenum, int targetnum)
{
	return;

	if(((targetnum == NONE) && (Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) || (targetnum == -999))
	{
		int msgsnd;

		if(!(Planes[planenum].FlightStatus & PL_PLANE_BLOWN_UP))
		{
			msgsnd = AICF_WSO_SCREAM + (rand() & 1);
			AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		return;
	}

	if((targetnum == NONE) || (targetnum == BOMB))
	{
		if(lNoBulletHitMsgs < 0)
		{
			lNoBulletHitMsgs = 10000;
			AIC_WSO_Unknown_Hit_Msg(planenum);
		}
	}
	else if(targetnum == MISSILE)
	{
		if(lNoBulletHitMsgs < 0)
		{
			lNoBulletHitMsgs = 5000;
			AIC_WSO_Missile_Hit_Msg(planenum);
		}
	}
	else if(targetnum == BULLET)
	{
		if(lNoBulletHitMsgs < 0)
		{
			lNoBulletHitMsgs = 10000;
			AIC_WSO_Bullet_Hit_Msg(planenum);
		}
	}
	else
	{
		if(lNoBulletHitMsgs < 0)
		{
			lNoBulletHitMsgs = 10000;
			AIC_WSO_AAA_Hit_Msg(planenum);
		}
	}
}

//**************************************************************************************
void AIC_WSO_Missile_Hit_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	tempnum = rand() & 3;

	if(tempnum == 1)
	{
		msgnum = AIC_WSO_MISSILE_HIT_1;
		msgsnd = AICF_WSO_MISSILE_HIT_1;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WSO_MISSILE_HIT_2;
		msgsnd = AICF_WSO_MISSILE_HIT_2;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_WSO_MISSILE_HIT_3;
		msgsnd = AICF_WSO_MISSILE_HIT_3;
	}
	else
	{
		msgnum = AIC_WSO_MISSILE_HIT_4;
		msgsnd = AICF_WSO_MISSILE_HIT_4;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Bullet_Hit_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	tempnum = rand() % 3;

	if(tempnum == 0)
	{
		msgnum = AIC_WSO_CANNON_HIT_1;
		msgsnd = AICF_WSO_CANNON_HIT_1;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WSO_CANNON_HIT_2;
		msgsnd = AICF_WSO_CANNON_HIT_2;
	}
	else
	{
		msgnum = AIC_WSO_CANNON_HIT_3;
		msgsnd = AICF_WSO_CANNON_HIT_3;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_AAA_Hit_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	tempnum = rand() & 3;

	if(tempnum == 1)
	{
		msgnum = AIC_WSO_AAA_HITS_1;
		msgsnd = AICF_WSO_AAA_HITS_1;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WSO_AAA_HITS_2;
		msgsnd = AICF_WSO_AAA_HITS_2;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_WSO_AAA_HITS_3;
		msgsnd = AICF_WSO_AAA_HITS_3;
	}
	else
	{
		msgnum = AIC_WSO_AAA_HITS_4;
		msgsnd = AICF_WSO_AAA_HITS_4;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Unknown_Hit_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	tempnum = rand() % 3;

	if(tempnum == 1)
	{
		msgnum = AIC_WSO_CANNON_HIT_1;
		msgsnd = AICF_WSO_CANNON_HIT_1;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WSO_MISSILE_HIT_3;
		msgsnd = AICF_WSO_MISSILE_HIT_3;
	}
	else
	{
		msgnum = AIC_WSO_MISSILE_HIT_4;
		msgsnd = AICF_WSO_MISSILE_HIT_4;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_Hit_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);


	if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
	{
		tempnum = rand() % 3;
		if(tempnum == 2)
			tempnum = 3;
	}
	else if((targetnum == NONE) || (targetnum == BOMB) || (targetnum == MISSILE) || (targetnum == BULLET))
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_USAF) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			tempnum = rand() & 1;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_FTR_USAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_NATO))
		{
			tempnum = 1;
		}
		else
		{
			tempnum = rand() % 3;
		}
	}
	else
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS))
		{
			tempnum = rand() & 1;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_FTR_USAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_NATO))
		{
			tempnum = rand() & 1;
			if(tempnum == 0)
			{
				tempnum = 3;
			}
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_USAF) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			tempnum = rand() % 3;
			if(tempnum == 2)
			{
				tempnum = 3;
			}
		}
		else
		{
			tempnum = rand() % 4;
		}
	}

	if(tempnum == 0)
	{
		msgsnd = AICF_WING_HIT;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_WING_WE_HIT_RUSA;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_WING_WE_HIT_RUSF;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
		{
			msgnum = AIC_WING_WE_HIT_RUSA;
		}
		else
		{
			msgnum = AIC_WING_HIT;
		}
	}
	else if(tempnum == 1)
	{
		msgsnd = AICF_WING_HIT_HURT;
		if((Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS))
		{
			msgnum = AIC_RAF_WE_HIT_HURT;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_FTR_USAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_NATO))
		{
			msgnum = AIC_USAF_WE_HIT_HURT;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
		{
			msgnum = AIC_WING_WE_HIT_HURT_RUSA;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
		{
			msgnum = AIC_WING_WE_HIT_HURT_RAF;
		}
		else
		{
			msgnum = AIC_WING_HIT_HURT;
		}
	}
	else if(tempnum == 2)
	{
		msgsnd = AICF_WING_HIT_MAJOR;
		msgnum = AIC_WING_HIT_MAJOR;
	}
	else
	{
		msgsnd = AICF_WING_HITS_FROM_GROUND;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_WING_WE_HIT_RUSA;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_WING_WE_HIT_RUSF;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
		{
			msgnum = AIC_WING_L_HIT_RAF;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
		{
			msgnum = AIC_WING_HITS_FROM_GROUND_RAF;
		}
		else
		{
			msgnum = AIC_WING_HITS_FROM_GROUND;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_Ejecting_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
//	long delaycnt;
	int msgnum = AIC_WING_EJECTING;

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	if(WIsWeaponPlane(&Planes[planenum]))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
	{
		msgnum = AIC_WING_EJECTING_RAF_ATK;
	}
	else if((Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS))
	{
		msgnum = AIC_WING_EJECTING_RAF_FTR;
	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	if((Planes[planenum].AI.lPlaneID != 30) && (!MultiPlayer))
	{
		AICAddSoundCall(BeginEjectionSequence, planenum, 3000 + (rand() & 1023), 50, -1);
	}

#if 0  //  Removed so that Player has to make call
	if((!(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (PlayerPlane->Status & PL_ACTIVE))
	{
		delaycnt = 7000 + ((rand() % 5) * 1000);
		AICAddSoundCall(AIC_Wingman_Ejected_Msg, planenum, delaycnt, 50, PlayerPlane - Planes);
	}
#endif


	AIRGenericSpeech(AICF_WING_EJECTING, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	AICAddAIRadioMsgs(tempstr, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_Fox_Msgs(PlaneParams *planepnt, WeaponParams *weaponpnt)
{
	int weaponindex;
	int kind, seeker, wtype;

	if(planepnt)
	{
		if(planepnt->FlightStatus & PL_OUT_OF_CONTROL)
		{
			return;
		}
	}

	if((weaponpnt != NULL) && (weaponpnt->Type))
	{
		weaponindex = AIGetWeaponIndex(weaponpnt->Type->TypeNumber);
		seeker = pDBWeaponList[weaponindex].iSeekerType;
		wtype = pDBWeaponList[weaponindex].iWeaponType;

		if((wtype == WEAPON_TYPE_AA_SHORT_RANGE_MISSILE) || (wtype == WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE) || (wtype == WEAPON_TYPE_AA_LONG_RANGE_MISSILE) || (wtype == WEAPON_TYPE_SAM))
		{
			if(seeker == 7)
			{
				kind = 1;
			}
			else if(seeker == 1)
			{
				kind = 3;
			}
			else
			{
				kind = 2;
			}
		}
		else if((wtype == WEAPON_TYPE_AG_MISSILE) || (wtype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
		{
			if(pDBWeaponList[weaponindex].lWeaponID == 116)
			{
				AICBombDropMsg(planepnt);
				return;
			}

			if(planepnt->Status & PL_AI_DRIVEN)
			{
				if(planepnt->AI.cNumWeaponRelease > 1)
				{
					return;
				}
			}

			if(seeker == 11)
			{
				kind = 11;
			}
			else
			{
				kind = 10;
			}
		}
		else
		{
			kind = 0;
		}
	}
	else
	{
		kind = 0;
	}

	if(planepnt)
	{
		if(planepnt->AI.iSide == PlayerPlane->AI.iSide)
		{
			AIC_Fox_Missile_Msg(planepnt - Planes, kind);
		}
	}

	if(weaponpnt->iTargetType == TARGET_PLANE)
	{
		AICheckForMissileWarning((PlaneParams *)weaponpnt->pTarget, weaponpnt - Weapons);
	}
}

//**************************************************************************************
void AIC_Fox_Missile_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, imsgsnd;
	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	switch(targetnum)
	{
		case 1:
			msgnum = AIC_FOX_1;
			msgsnd = AICF_FOX_1;
			imsgsnd = AICF_INT_FOX_1;
			break;

		case 2:
			msgnum = AIC_FOX_2;
			msgsnd = AICF_FOX_2;
			imsgsnd = AICF_INT_FOX_2;
			break;

		case 3:
			msgnum = AIC_FOX_3;
			msgsnd = AICF_FOX_3;
			imsgsnd = AICF_INT_FOX_3;
			break;

		case 10:
		case 11:
			AICMaverickLaunchMsg(&Planes[planenum], targetnum);
			return;
#if 0
			tempnum = rand() & 1;
			if(tempnum == 1)
			{
				msgnum = AIC_MAVERICK_LAUNCH_2;
				msgsnd = AICF_MAVERICK_LAUNCH_2;
			}
			else
			{
				msgnum = AIC_MAVERICK_LAUNCH;
				msgsnd = AICF_MAVERICK_LAUNCH;
			}
#endif
			break;

		default:
			return;
			break;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

//	if((Planes[planenum].AI.lAIVoice == SPCH_CAP1) || (Planes[planenum].AI.lAIVoice == SPCH_CAP2) || (Planes[planenum].AI.lAIVoice == SPCH_CAP3))
//	{
//		msgsnd = imsgsnd;
//	}

	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Guns_Msgs(PlaneParams *planepnt, WeaponParams *weaponpnt)
{
	if(weaponpnt->iTargetType == TARGET_PLANE)
	{
		if((PlaneParams *)weaponpnt->pTarget == PlayerPlane)
		{
			if(AICNoShotsAtPlayer(weaponpnt))
			{
				if(weaponpnt->P)
				{
					iLastWSOWarnType = TARGET_PLANE;
					pLastWSOWarnObj = weaponpnt->P;
				}

				if(lNoBulletHitMsgs < 0)
				{
					AICDoWSOGunsBreakMsg(PlayerPlane - Planes);
				}
			}
		}
	}

	return;
}

//**************************************************************************************
int AICNoShotsAtPlayer(WeaponParams *ignoreweapon)
{
	WeaponParams *weaponpnt = &Weapons[0];

	while(weaponpnt <= LastWeapon)
	{
		if((weaponpnt != ignoreweapon) && (weaponpnt->Flags & WEAPON_INUSE) && (weaponpnt->Flags & BULLET_INUSE))
		{
			if(weaponpnt->iTargetType == TARGET_PLANE)
			{
				if((PlaneParams *)weaponpnt->pTarget == PlayerPlane)
				{
					return(0);
				}
			}
		}
		weaponpnt++;
	}
	return(1);
}

//**************************************************************************************
void AICDoWSOGunsBreakMsg(int planenum, int targetnum)
{
	char tempstr[1024];

	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		AICGenericBettySpeech(BETTY_ALERT);
	}

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_ENEMY_GUNS_JINK, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(AICF_ENEMY_GUNS_JINK, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	AICAddAIRadioMsgs(tempstr, 50);
}

//**************************************************************************************
void AIC_WSO_Bomb_Msgs(PlaneParams *planepnt, WeaponParams *weaponpnt)
{
	int jettisoned = 0;

	if(!(planepnt->AI.iAIFlags2 & AI_RIPPLE_BOMB))
	{
		if(planepnt == PlayerPlane)
		{
			if(weaponpnt->Flags & WEP_JETTISONED)
			{
				jettisoned = 1;
			}

			AIC_WSO_Release_Msgs(planepnt - Planes, jettisoned);
		}
	}
	return;
}

//**************************************************************************************
void AIC_WSO_Release_Msgs(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(targetnum == 1)
	{
		tempnum = 1;
	}
	else
	{
		tempnum = rand() & 3;
	}

	if(tempnum == 0)
	{
		msgnum = AIC_WSO_BOMBS_GONE;
		msgsnd = AICF_WSO_BOMBS_GONE;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WSO_RELEASE;
		msgsnd = AICF_WSO_RELEASE;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_WSO_GOOD_RELEASE;
		msgsnd = AICF_WSO_GOOD_RELEASE;
	}
	else
	{
		msgnum = AIC_WSO_PICKLE;
		msgsnd = AICF_WSO_PICKLE;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Get_Callsign_With_Number(int planenum, char *tempstr)
{
	char tempstr2[256];
	char callsign[256];
	int placeingroup;

	tempstr[0] = 0;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	AICGetCallSign(planenum, callsign, 0);
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 256, AIC_CALLSIGN_NUMBER, g_iLanguageId, callsign, tempstr2))
	{
		return;
	}
}

//**************************************************************************************
void AIC_Get_Flight_Callsign(int planenum, char *tempstr)
{
	AIC_Get_Callsign_With_Number(planenum, tempstr);
	return;

	char callsign[256];

	tempstr[0] = 0;

	AICGetCallSign(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 256, AIC_FLIGHT_CALLSIGN, g_iLanguageId, callsign))
	{
		return;
	}
}

//**************************************************************************************
void AIC_WSO_Bomb_Speech(PlaneParams *planepnt, int bombphase, long timervar)
{
	return;

	if((lBombFlags & WSO_BOMB_IMPACT) && (bombphase != WSO_BOMB_IMPACT))
	{
		return;
	}

	switch(bombphase)
	{
		case WSO_BOMB_IMPACT:
			AIC_WSO_Impact(planepnt, timervar);
			break;
		case WSO_BOMB_TREL:
			AIC_WSO_Release(planepnt, timervar);
			AIC_WSO_Check_Steering(planepnt);
			break;
		case WSO_BOMB_TPULL:
			AIC_WSO_Pull(planepnt, timervar);
			AIC_WSO_Check_Steering(planepnt);
			break;
	}
}

//**************************************************************************************
void AIC_WSO_Impact(PlaneParams *planepnt, long timervar)
{
	long orgtime, newtime;

	return;

	if(timervar < 0)
		return;

	orgtime = lBombTimer / 1000;
	lBombTimer -= DeltaTicks;
	if(lBombTimer < 0)
	{
		lBombFlags &= ~(WSO_BOMB_IMPACT|WSO_BOMB_TREL|WSO_BOMB_TPULL|WSO_STEERING_MSG|WSO_NAV_MSGS);
	}

	newtime = lBombTimer / 1000;

	if((newtime > 4) && (newtime <= 60))
	{
		if(orgtime != newtime)
		{
			if(((newtime % 10) == 0) || (newtime == 5))
			{
				AIC_WSO_Impact_Msg(planepnt - Planes, newtime);
			}
		}
	}
}

//**************************************************************************************
void AIC_WSO_Impact_Msg(int planenum, int targetnum)
{
	char seconds[256];
	char tempstr[1024];

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!LANGGetTransMessage(seconds, 256, AIC_ZERO + targetnum, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_TIME_TO_IMPACT, g_iLanguageId, seconds))
	{
		return;
	}

	if(lWSOSpeakTimer < 0)
	{
		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(AICF_WSO_TIME_TO_IMPACT, planenum, 0, 0, 0, 0, 0, 0, targetnum, 0, 0);
		lWSOSpeakTimer = 3000;
	}
}

//**************************************************************************************
void AIC_WSO_Release(PlaneParams *planepnt, long timervar)
{
	int miles;

	return;

	if(timervar < 0)
		return;

	if(!(lBombFlags & WSO_BOMB_TREL))
	{
		lBombFlags |= WSO_BOMB_TREL;
		lBombFlags &= ~WSO_BOMB_TPULL;
		lBombTimer = 161;
	}

	if(timervar < 0)
	{
		return;
	}
	if(timervar < 65)
	{
		if((lBombTimer > timervar) && (((timervar % 10) == 0) || (timervar == 5)))
		{
			lBombTimer = timervar;
			AIC_WSO_Release_Msg(planepnt - Planes, timervar);
		}
	}
	else
	{
		miles = (int)UFC.DataCurrTGTNavDist;

		if((lBombTimer > (miles + 100)) && ((miles % 10) == 0) && (miles > 0))
		{
			lBombTimer = miles + 100;
			AIC_WSO_TGT_Miles_Msg(planepnt - Planes, miles);
		}
	}
}

//**************************************************************************************
void AIC_WSO_Release_Msg(int planenum, int targetnum)
{
	char seconds[256];
	char tempstr[1024];

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(targetnum == 0)
	{
#if 1
		if(lAdvisorFlags & ADV_CASUAL_BETTY)
		{
			AICGenericBettySpeech(BETTY_ZERO);
		}
		return;
#else
		if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_RELEASE, g_iLanguageId))
		{
			return;
		}
#endif
	}
	else
	{
		if(lAdvisorFlags & ADV_CASUAL_BETTY)
		{
			AICGenericBettySpeech(BETTY_PULL_SECS, 0, 0, targetnum);
		}
		return;

		if(!LANGGetTransMessage(seconds, 256, AIC_ZERO + targetnum, g_iLanguageId))
		{
			return;
		}

		if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_RELEASE_TIME, g_iLanguageId, seconds))
		{
			return;
		}
	}

	if(lWSOSpeakTimer < 0)
	{
		AICAddAIRadioMsgs(tempstr, 50);

		AIRGenericSpeech(AICF_WSO_RELEASE_TIME, planenum, 0, 0, 0, 0, 0, 0, targetnum, 0, 0);

		lWSOSpeakTimer = 3000;
	}
}

//**************************************************************************************
void AIC_WSO_TGT_Miles_Msg(int planenum, int targetnum)
{
	char miles[256];
	char tempstr[1024];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		AICGenericBettySpeech(BETTY_TARGET_DIST, 0, targetnum / 10);
	}
	return;

	if(!LANGGetTransMessage(miles, 256, AIC_ZERO + targetnum, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_TARGET_DIST, g_iLanguageId, miles))
	{
		return;
	}

	if(targetnum == 1)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_MILE_TO_TARGET, g_iLanguageId, miles))
		{
			return;
		}
	}

	if(lWSOSpeakTimer < 0)
	{
		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(AICF_WSO_TARGET_DIST, planenum, 0, 0, 0, 0, targetnum, 0, 0, 0, 0);
		lWSOSpeakTimer = 3000;
	}
}

//**************************************************************************************
void AIC_WSO_Pull(PlaneParams *planepnt, long timervar)
{
	int miles;

	if(timervar < 0)
		return;

	if(!(lBombFlags & WSO_BOMB_TPULL))
	{
		lBombFlags |= WSO_BOMB_TPULL;
		lBombFlags &= ~WSO_BOMB_TREL;
		lBombTimer = 161;
	}

	if(timervar < 0)
	{
		return;
	}
	if(timervar < 65)
	{
		if((lBombTimer > timervar) && (((timervar % 10) == 0) || (timervar == 5)))
		{
			lBombTimer = timervar;
			AIC_WSO_Pull_Msg(planepnt - Planes, timervar);
		}
	}
	else
	{
		miles = (int)UFC.DataCurrTGTNavDist;

		if((lBombTimer > (miles + 100)) && ((miles % 10) == 0) && (miles > 0))
		{
			lBombTimer = miles + 100;
			AIC_WSO_TGT_Miles_Msg(planepnt - Planes, miles);
		}
	}
}

//**************************************************************************************
void AIC_WSO_Pull_Msg(int planenum, int targetnum)
{
	char seconds[256];
	char tempstr[1024];
	int msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(targetnum == 0)
	{
		if(lAdvisorFlags & ADV_CASUAL_BETTY)
		{
			AICGenericBettySpeech(BETTY_PULL_UP);
		}
		return;
		if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_PULL_NOW, g_iLanguageId))
		{
			return;
		}
		msgsnd = AICF_WSO_PULL_NOW;
	}
	else
	{
		if(lAdvisorFlags & ADV_CASUAL_BETTY)
		{
			AICGenericBettySpeech(BETTY_PULL_SECS, 0, 0, targetnum);
		}
		return;

		if(!LANGGetTransMessage(seconds, 256, AIC_ZERO + targetnum, g_iLanguageId))
		{
			return;
		}

		if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_PULL_TIME, g_iLanguageId, seconds))
		{
			return;
		}
		msgsnd = AICF_WSO_PULL_TIME;
	}

	if(lWSOSpeakTimer < 0)
	{
		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, targetnum, 0, 0);
		lWSOSpeakTimer = 3000;
	}
}

//**************************************************************************************
void AIC_WSO_Check_Steering(PlaneParams *planepnt)
{
	return;

	if(fabs(UFC.DataCurrTGTRelBrg) < 2)
	{
		if((lBombFlags & WSO_STEERING_MSG) && (lWSOSpeakTimer < 0))
		{
			AIC_WSO_Looking_Good_Msg(planepnt - Planes);
			lWSOSpeakTimer = 3000;
			lBombFlags &= ~WSO_STEERING_MSG;
		}
	}
	else if((!(lBombFlags & WSO_STEERING_MSG)) && (lWSOSpeakTimer < 0))
	{
		if(((planepnt->Roll > 0x8000) && (UFC.DataCurrTGTRelBrg > 0)) || ((planepnt->Roll < 0x8000) && (UFC.DataCurrTGTRelBrg < 0)))
		{
			AIC_WSO_Watch_Steering_Msg(planepnt - Planes);
			lWSOSpeakTimer = 3000;
			lBombFlags |= WSO_STEERING_MSG;
		}
	}
}

//**************************************************************************************
void AIC_WSO_Looking_Good_Msg(int planenum, int targetnum)
{
	char tempstr[1024];

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_LOOKING_GOOD, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_LOOKING_GOOD, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

//**************************************************************************************
void AIC_WSO_Watch_Steering_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tempnum = rand() % 3;

	if(tempnum == 0)
	{
		msgnum = AIC_WSO_WATCH_STEERING;
		msgsnd = AICF_WSO_WATCH_STEERING;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WSO_TAKE_STEERING;
		msgsnd = AICF_WSO_TAKE_STEERING;
	}
	else
	{
		msgnum = AIC_WSO_CENTER_STEERING;
		msgsnd = AICF_WSO_CENTER_STEERING;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}


#if 0  //  F-15
//**************************************************************************************
void AIC_WSO_Nav_Speech(PlaneParams *planepnt)
{
	int minutes;
	float offangle;

	return;

	if(!(lBombFlags & WSO_NAV_MSGS))
	{
		lBombFlags |= WSO_NAV_MSGS;
		lBombTimer = (UFC.DataCurrSPDistFeet / (planepnt->V * 60));
	}

	minutes = (UFC.DataCurrSPDistFeet / (planepnt->IfHorzVelocity * 60));

	offangle = UFC.DataCurrSPBrg;
	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

	offangle = AICapAngle(offangle);

	if((lBombTimer > (minutes)) && (((minutes + 1) % 10) == 0) && (minutes > 0) && (minutes < 125) && (fabs(offangle) < 30) && (!(planepnt->OnGround)))
	{
		lBombTimer = minutes;
		AIC_WSO_Nav_Minutes_Msg(planepnt - Planes, minutes + 1);
	}
}
#else
//**************************************************************************************
void AIC_WSO_Nav_Speech(PlaneParams *planepnt)
{
	int tdistnm;
	float offangle;
	int tdistnm10;

	if(!(lBombFlags & WSO_NAV_MSGS))
	{
		lBombFlags |= WSO_NAV_MSGS;
		lBombTimer = UFC.DataCurrSPDistFeet * FTTONM;
	}

	tdistnm = UFC.DataCurrSPDistFeet * FTTONM;
	offangle = UFC.DataCurrSPBrg;
	offangle -= AIConvertAngleTo180Degree(planepnt->Heading);

	offangle = AICapAngle(offangle);

	tdistnm10 = tdistnm / 10;
	if(((lBombTimer / 10) > (tdistnm10)) && (tdistnm10 < 6) && (fabs(offangle) < 30) && (!(planepnt->OnGround)))
	{
		if((lAdvisorFlags & ADV_CASUAL_BETTY) && ((lBombTimer / 10) == (tdistnm10 + 1)))
		{
			AICGenericBettySpeech(BETTY_WAY_DIST, 0, tdistnm10 + 1);
		}
		lBombTimer = tdistnm;
	}
}

#endif

//**************************************************************************************
void AIC_WSO_Nav_Minutes_Msg(int planenum, int targetnum)
{
	char minutes[256];
	char tempstr[1024];
	long workrange;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(targetnum <= 80)
	{
		if(targetnum > 60)
		{
			targetnum /= 5;
			targetnum *= 5;
		}

		if(!LANGGetTransMessage(minutes, 256, AIC_ZERO + targetnum, g_iLanguageId))
		{
			return;
		}
	}
	else if(targetnum <= 200)
	{
		if(!LANGGetTransMessage(minutes, 256, AIC_EIGHTY + (((targetnum / 5) - 16)), g_iLanguageId))
		{
			return;
		}
	}
#if 0
	else
	{
		if(!LANGGetTransMessage(minutes, 256, AIC_ONE_HUNDRED + ((targetnum / 10) - 10), g_iLanguageId))
		{
			return;
		}
	}
#else
	else
	{
		workrange = targetnum / 10;
		workrange *= 10;
		sprintf(minutes, "%ld", workrange);
	}
#endif


	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_MINUTES_TO_NAV, g_iLanguageId, minutes))
	{
		return;
	}

	if(lWSOSpeakTimer < 0)
	{
		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(AICF_WSO_MINUTES_TO_NAV, planenum, 0, 0, 0, 0, 0, 0, targetnum, 0, 0);
		lWSOSpeakTimer = 3000;
	}
}

//*******************************************************************************************
int AICCheckForWingmanInVisualRange(PlaneParams *planepnt, PlaneParams *targetplane, int sametarget)
{
	PlaneParams *checkplane;
	PlaneParams *returnplane = NULL;
	float dx, dy, dz, tdist, tdistnm;
	float founddist = 50.0f;
	int planenum = planepnt - Planes;

	checkplane = &Planes[0];

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (planepnt->AI.iSide == checkplane->AI.iSide)
				&& ((checkplane->AI.wingman == planenum) || (checkplane->AI.winglead == planenum)))
		{
			if(targetplane != NULL)
			{
				dx = checkplane->WorldPosition.X - targetplane->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - targetplane->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - targetplane->WorldPosition.Z;
			}
			else
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
			}
			tdist = QuickDistance(dx, dz);
			tdistnm = (tdist * WUTONM);
			if(tdistnm < AI_VISUAL_RANGE_NM)
			{
				if(founddist > tdistnm)
				{
					if(sametarget)
					{
						if((checkplane->AI.AirTarget == targetplane) || (checkplane->AI.AirThreat == targetplane))
						{
							return(checkplane - Planes);
						}
					}
					else
					{
						return(checkplane - Planes);
					}
				}
			}
		}
		checkplane ++;
	}
	return(-1);
}

//*******************************************************************************************
void AICWingmanConfirmKill(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum;
	char callsign[256];
	int placeingroup;
	int msgnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS);

	if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
	{
		tempnum = 1;
	}
	else
	{
		tempnum = rand() & 1;
	}

#if 0
	if(tempnum == 0)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_PLAYER_KILL_1, g_iLanguageId))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(AICF_PLAYER_KILL_1, targetnum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		return;
	}
	else if(tempnum == 2)
#endif
	if(tempnum == 1)
	{
		AIC_Get_Callsign_With_Number(planenum, callsign);

		msgnum = AIC_PLAYER_KILL_3;

		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_RUSF_KILL_3;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_RAF_KILL_3;
		}

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(AICF_PLAYER_KILL_3, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
		return;
	}
	else
	{
		AIC_Get_Callsign_With_Number(targetnum, callsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_PLAYER_KILL_2, g_iLanguageId, callsign))
		{
			return;
		}

		AIRGenericSpeech(AICF_PLAYER_KILL_2, targetnum, 0, 0, 0, 0, 0, 0, 0, 0, 0);

		AICAddAIRadioMsgs(tempstr, 50);
		return;
	}
}

//**************************************************************************************
void AIC_Eject_Wingman_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & PL_STATUS_CRASHED)
	{
		return;
	}

	if(WIsWeaponPlane(&Planes[planenum]))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);


	tempnum = rand() % 3;

	if(tempnum == 0)
	{
		msgsnd = AICF_WING_EJECT_CALL_1;
		msgnum = AIC_WING_EJECT_CALL_1;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_RUSF_EJECT_CALL_1;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_RAF_EJECT_CALL_1;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
		{
			msgnum = AIC_WING_EJECT_CALL_1_RUSA;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
		{
			msgnum = AIC_WING_EJECT_CALL_1_RAF;
		}
	}
	else if(tempnum == 1)
	{
		msgsnd = AICF_WING_EJECT_CALL_2;
		msgnum = AIC_WING_EJECT_CALL_2;
	}
	else
	{
		msgsnd = AICF_WING_EJECT_CALL_3;
		msgnum = AIC_WING_EJECT_CALL_3;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_WING_EJECT_CALL_1_RUSA;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_RAF_EJECT_CALL_3;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
		{
			msgnum = AIC_WING_EJECT_CALL_3_RAF;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
		{
			msgnum = AIC_WING_EJECT_CALL_3_RAF;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	AIRGenericSpeech(msgsnd, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);

	return;
}

//**************************************************************************************
void AIC_Wingman_Ejected_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int sarmoving = 0;
	long ltimer;
	PlaneParams *planepnt = &Planes[targetnum];

	if(((!MultiPlayer) || (Planes[targetnum].AI.iAICombatFlags1 & AI_MULTI_ACTIVE)) && (iAICommFrom < 0))
	{
		sarmoving = AICheckForSAR(&Planes[planenum]);
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum < 0)
	{
		awacsnum = -2;
	}

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
	}
	else
	{
		if(!LANGGetTransMessage(awacscallsign, 128, AIC_STRIKE, g_iLanguageId))
		{
			return;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_SAR_CALL, g_iLanguageId, awacscallsign, callsign))
	{
		return;
	}

	if(((!MultiPlayer) || (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE)) && (iAICommFrom < 0))
	{
		if(lPlayerSARTimer < 0)
		{
			if(sarmoving)
			{
				ltimer = 5000 + ((rand() & 3) * 1000);
				AICAddSoundCall(AIC_AWACS_SAR_OK, targetnum, ltimer, 50, awacsnum);
				ltimer *= -1;
				NetPutGenericMessage3Long(&Planes[targetnum], GM3L_SAR_RESPONSE, awacsnum, ltimer);
			}
			else
			{
				ltimer = 5000 + ((rand() & 3) * 1000);
				AICAddSoundCall(AICNoCover, targetnum, ltimer, 50, awacsnum);
				NetPutGenericMessage3Long(&Planes[targetnum], GM3L_SAR_RESPONSE, awacsnum, ltimer);
			}
			lPlayerSARTimer = 600000;
		}
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIC_Wingman_Ejected_Snd(awacsnum, targetnum, planenum);
	return;
}

//**************************************************************************************
void AIC_Wingman_Ejected_Snd(int awacsnum, int planenum, int crashingnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int placeingroup;

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_WSO_SAR_CALL, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			if((numval == 27) && (awacsnum < 0))
			{
				numids += AIRProcessSpeechVars(&sndids[numids], 35, planenum, 1);
			}
			else
			{
				numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, awacsnum);
			}
		}
		else if(spchvar == 2)
		{
			if(numval == 3)
			{
				placeingroup = (Planes[crashingnum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
				sndids[numids] = AIRGetWngCallNum(crashingnum, placeingroup) + Planes[planenum].AI.lAIVoice;
				if(AIRCheckCallSignSpchExists(sndids[numids]))
				{
					numids ++;
				}
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AICDoSAROnWayMsgs(PlaneParams *planepnt, int guyhurt)
{
	float dx, dy, dz, tdist, tdistnm;
	float maxguard = 40;
	int planenum, playernum;
	int ishurt = guyhurt;
	int enemynear;
	int delaycnt;

	if((planepnt < Planes) || (planepnt > LastPlane))
		return;

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}
	dx = planepnt->WorldPosition.X - PlayerPlane->WorldPosition.X;
	dy = planepnt->WorldPosition.Y - PlayerPlane->WorldPosition.Y;
	dz = planepnt->WorldPosition.Z - PlayerPlane->WorldPosition.Z;
	tdist = QuickDistance(dx, dz);
	tdistnm = (tdist * WUTONM);

	if(tdistnm > maxguard)
	{
		return;
	}

	planenum = planepnt - Planes;
	playernum = PlayerPlane - Planes;

	delaycnt = 15000;
	if((planepnt->AI.iAIFlags2 & AIPLAYERGROUP) && (rand() & 1))
	{
		AICAddSoundCall(AIC_Player_Read_Me, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
	else
	{
		AICAddSoundCall(AIC_Anyone_Read_Me, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}

	if(rand() & 1)
	{
		AICAddSoundCall(AIC_Help_On_Way, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
	else
	{
		AICAddSoundCall(AIC_SAR_Inbound, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}

	enemynear = AIC_Enemy_Near(planepnt);

	if(enemynear)
	{
		AICAddSoundCall(AIC_Enemy_Near_Hiding, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		AICAddSoundCall(AIC_WSO_Hang_In, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
	else
	{
		if(rand() & 1)
		{
			AICAddSoundCall(AIC_Wingman_I_Understood, planenum, delaycnt, 50, playernum);
			delaycnt += 10000;
		}
		else
		{
			AICAddSoundCall(AIC_Wingman_I_Ack, planenum, delaycnt, playernum);
			delaycnt += 10000;
		}
	}

	if(ishurt)
	{
		AICAddSoundCall(AIC_Wingman_Hurt, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		if(!enemynear)
		{
			if(rand() & 1)
			{
				AICAddSoundCall(AIC_WSO_Hang_In, planenum, delaycnt, 50, playernum);
				delaycnt += 10000;
			}
			else
			{
				AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
				delaycnt += 10000;
			}
		}
		else
		{
			AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
			delaycnt += 10000;
		}
	}
	else
	{
		AICAddSoundCall(AIC_Wingman_OK, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
}

//**************************************************************************************
void AICDoNoSAROnWayMsgs(PlaneParams *planepnt, int guyhurt)
{
	float dx, dy, dz, tdist, tdistnm;
	float maxguard = 40;
	int planenum, playernum;
	int ishurt = guyhurt;
	int enemynear;
	int delaycnt;

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}
	dx = planepnt->WorldPosition.X - PlayerPlane->WorldPosition.X;
	dy = planepnt->WorldPosition.Y - PlayerPlane->WorldPosition.Y;
	dz = planepnt->WorldPosition.Z - PlayerPlane->WorldPosition.Z;
	tdist = QuickDistance(dx, dz);
	tdistnm = (tdist * WUTONM);

	if(tdistnm > maxguard)
	{
		return;
	}

	planenum = planepnt - Planes;
	playernum = PlayerPlane - Planes;
	enemynear = AIC_Enemy_Near(planepnt);

	delaycnt = 15000;
	if((planepnt->AI.iAIFlags2 & AIPLAYERGROUP) && (rand() & 1))
	{
		AICAddSoundCall(AIC_Player_Read_Me, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
	else
	{
		AICAddSoundCall(AIC_Anyone_Read_Me, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}

	AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
	delaycnt += 10000;

	if(enemynear)
	{
		AICAddSoundCall(AIC_Enemy_Near_Hiding, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		AICAddSoundCall(AIC_WSO_Hang_In, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}

	if(ishurt)
	{
		AICAddSoundCall(AIC_Wingman_Hurt, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		if(!enemynear)
		{
			if(rand() & 1)
			{
				AICAddSoundCall(AIC_WSO_Hang_In, planenum, delaycnt, 50, playernum);
				delaycnt += 10000;
			}
			else
			{
				AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
				delaycnt += 10000;
			}
		}
		else
		{
			AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
			delaycnt += 10000;
		}
	}
	else
	{
		AICAddSoundCall(AIC_Wingman_OK, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
}

//**************************************************************************************
void AICShortSARMsgs(PlaneParams *planepnt, int guyhurt)
{
	float dx, dy, dz, tdist, tdistnm;
	float maxguard = 40;
	int planenum, playernum;
	int ishurt = guyhurt;
	int delaycnt;

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}
	dx = planepnt->WorldPosition.X - PlayerPlane->WorldPosition.X;
	dy = planepnt->WorldPosition.Y - PlayerPlane->WorldPosition.Y;
	dz = planepnt->WorldPosition.Z - PlayerPlane->WorldPosition.Z;
	tdist = QuickDistance(dx, dz);
	tdistnm = (tdist * WUTONM);

	if(tdistnm > maxguard)
	{
		return;
	}

	planenum = planepnt - Planes;
	playernum = PlayerPlane - Planes;
	if(Planes[playernum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	delaycnt = 15000;

	if(ishurt)
	{
		AICAddSoundCall(AIC_Wingman_Hurt, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		if(rand() & 1)
		{
			AICAddSoundCall(AIC_WSO_Hang_In, planenum, delaycnt, 50, playernum);
			delaycnt += 10000;
		}
		else
		{
			AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
			delaycnt += 10000;
		}
	}
	else
	{
		AICAddSoundCall(AIC_Wingman_OK, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;

		AICAddSoundCall(AIC_WSO_Ack, planenum, delaycnt, 50, playernum);
		delaycnt += 10000;
	}
}

//**************************************************************************************
void AIC_Player_Read_Me(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_READ_ME_1, g_iLanguageId, playercallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_READ_ME, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Anyone_Read_Me(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int msgnum = AIC_SHOT_DOWN_1;

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
	{
		msgnum = AIC_SHOT_DOWN_1_RAF;
	}
	else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
	{
		msgnum = AIC_SHOT_DOWN_1_RAF;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_1, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Help_On_Way(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_HELP_ON_WAY_2, g_iLanguageId, callsign, playercallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_HELP_ON_WAY, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_Inbound(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_SANDY_INBOUND, g_iLanguageId, callsign, playercallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_SANDY_INBOUND, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Enemy_Near_Hiding(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int msgnum = AIC_SHOT_DOWN_HIDING_US;

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
	{
		msgnum = AIC_SHOT_DOWN_1_RAF;
	}
	else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
	{
		msgnum = AIC_SHOT_DOWN_1_RAF;
	}
	if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
	{
		msgnum = AIC_SHOT_DOWN_HIDING_RUSA_1;
	}
	else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
	{
		msgnum = AIC_SHOT_DOWN_HIDING_US_1_RAF;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_HIDING_US, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Hang_In(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_HANG_IN_THERE, g_iLanguageId, callsign, playercallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_HANG_IN_THERE, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_I_Understood(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_UNDERSTOOD_1, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_UNDERSTOOD, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_I_Ack(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if((planenum < 0) || (planenum > (LastPlane - Planes)))
		return;

	if((targetnum < 0) || (targetnum > (LastPlane - Planes)))
		return;

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_ACK_1, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_ACK, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Ack(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_ACK_1, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_ACK, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Grnd_Ack(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	strcpy(callsign, CallSignList[planenum].sName);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);


	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_ACK, g_iLanguageId, callsign, playercallsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_GRND_ACK, targetnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_Hurt(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int msgnum = AIC_SHOT_DOWN_BEAT_UP_US_2;

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
	{
		msgnum = AIC_SHOT_DOWN_BEAT_UP_RUSF_2;
	}
	else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
	{
		msgnum = AIC_SHOT_DOWN_BEAT_UP_RAFF_2;
	}
	if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS)
	{
		msgnum = AIC_SHOT_DOWN_BEAT_UP_RUSA_2;
	}
	else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
	{
		msgnum = AIC_SHOT_DOWN_HIDING_US_1_RAF;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_BEAT_UP_US, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_OK(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_SHOT_DOWN_SAFE_1, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_SHOT_DOWN_SAFE, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
int AIC_Enemy_Near(PlaneParams *planepnt)
{
  	BasicInstance *checkobject;
	float rangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	int pass;

#if 0
	int tempfence = 0;

	tempfence = AICCheckAllFences((planepnt->WorldPosition.X * WUTOFT), (planepnt->WorldPosition.Z * WUTOFT));

	if(tempfence == 0)
	{
		return(0);
	}
#endif

	rangenm = 10;
	foundrange = (rangenm * NMTOWU);

	BasicInstance ***checklist = &AllTargetTypes[0];
	pass = NumTargetTypes;

	while(pass--)
	{
		checkobject = **checklist++;

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject) && (GDConvertGrndSide(checkobject) != planepnt->AI.iSide))
			{
				dx = (checkobject->Position.X) - planepnt->WorldPosition.X;
				dy = (checkobject->Position.Y) - planepnt->WorldPosition.Y;
				dz = (checkobject->Position.Z) - planepnt->WorldPosition.Z;
				if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
				{
					tdist = QuickDistance(dx, dz);
					if(tdist < foundrange)
					{
						return(1);
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	return(0);
}

//**************************************************************************************
void AICheckForMissileWarning(PlaneParams *planepnt, int weaponnum)
{
	if(planepnt == NULL)
	{
		return;
	}
	else if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(planepnt == PlayerPlane)
	{
		if(AIMissileNoticed(&Weapons[weaponnum]))
		{
			AICAddSoundCall(AIC_WSO_MissileLaunch, planepnt - Planes, 3000, 10, weaponnum);
			Weapons[weaponnum].Flags |= MISSILE_SPOTTED;
		}
	}
	else
	{
		if(AIMissileNoticed(&Weapons[weaponnum]))
		{
			if(planepnt->AI.iAIFlags1 & AIPLAYERGROUP)
			{
				if((weaponnum == planepnt->AI.iMissileThreat) || (planepnt->AI.iMissileThreat < 0))
				{
					AICAddSoundCall(AIC_MissileLaunch, planepnt - Planes, 3000, 10, weaponnum);
				}

				if(planepnt->AI.Behaviorfunc == AIFlyFormation)
				{
					if(!(planepnt->AI.iAIFlags1 & AI_ASK_PLAYER_ENGAGE))
					{
						AICAddSoundCall(AICWingReadyToEngageMsg, planepnt - Planes, 6000, 10);
						planepnt->AI.iAIFlags1 |= AI_ASK_PLAYER_ENGAGE;
					}
				}
			}
			else if((planepnt->AI.lAIVoice == SPCH_SAR1) || (planepnt->AI.lAIVoice == SPCH_SAR2))
			{
				if((weaponnum == planepnt->AI.iMissileThreat) || (planepnt->AI.iMissileThreat < 0))
				{
					AICAddSoundCall(AIC_SARMissileWarn, planepnt - Planes, 3000, 10, weaponnum);
				}
			}
			else if((planepnt->AI.lAIVoice == SPCH_AWACS1) || (planepnt->AI.lAIVoice == SPCH_AWACS2))
			{
				if((weaponnum == planepnt->AI.iMissileThreat) || (planepnt->AI.iMissileThreat < 0))
				{
					AICAddSoundCall(AIC_AWACSMissileWarn, planepnt - Planes, 3000, 10, weaponnum);
				}
			}
			else if(planepnt->AI.lAIVoice == SPCH_JSTARS1)
			{
				if((weaponnum == planepnt->AI.iMissileThreat) || (planepnt->AI.iMissileThreat < 0))
				{
					AICAddSoundCall(AIC_JSTARSAttackWarn, planepnt - Planes, 3000, 10, weaponnum);
				}
			}

			Weapons[weaponnum].Flags |= MISSILE_SPOTTED;
		}
	}
}

//**************************************************************************************
void AIC_WSO_MissileLaunch(int planenum, int targetnum)
{
	char tempstr[1024];
	char positionstr[256];
	int tempnum, msgnum, msgsnd;
	float fworkval, tdist, dx, dy, dz;
	int workval;
	float bearing;


	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	iLastWSOWarnType = TARGET_WEAPON;
	pLastWSOWarnObj = &Weapons[targetnum];

	bearing = AIComputeHeadingToPoint(&Planes[planenum], Weapons[targetnum].Pos, &tdist, &dx ,&dy, &dz, 1);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	workval = (fworkval + 15) / 30;

	if(workval <= 0)
	{
		workval = 12;
	}

	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		if(Weapons[targetnum].LauncherType == AIRCRAFT)
		{
			AICGenericBettySpeech(BETTY_MISSILE_LAUNCH, workval);
		}
		else
		{
			AICGenericBettySpeech(BETTY_SAM_LAUNCH, workval);
		}
	}

	return;

	if(Weapons[targetnum].LauncherType == GROUNDOBJECT)
	{
		tempnum = rand() % 3;
		if(tempnum == 0)
		{
			msgnum = AIC_WSO_SAM_SPOT_1;
			msgsnd = AICF_WSO_SAM_SPOT_1;
		}
		else if(tempnum == 1)
		{
			msgnum = AIC_WSO_SAM_SPOT_2;
			msgsnd = AICF_WSO_SAM_SPOT_2;
		}
		else
		{
			msgnum = AIC_WSO_SAM_SPOT_3;
			msgsnd = AICF_WSO_SAM_SPOT_3;
		}
	}
	else
	{
		tempnum = rand() % 3;
		if(tempnum == 0)
		{
			msgnum = AIC_INCOMING_MISSILE;
			msgsnd = AICF_INCOMING_MISSILE;
		}
		else if(tempnum == 1)
		{
			msgnum = AIC_MISSILE_INBOUND;
			msgsnd = AICF_MISSILE_INBOUND;
		}
		else if(tempnum == 2)
		{
			msgnum = AIC_HEADS_UP_MISSILE;
			msgsnd = AICF_HEADS_UP_MISSILE;
		}
		else if(tempnum == 3)
		{
			msgnum = AIC_ENEMY_MISSILE_OFF;
			msgsnd = AICF_ENEMY_MISSILE_OFF;
		}
	}

	bearing = AIComputeHeadingToPoint(&Planes[planenum], Weapons[targetnum].Pos, &tdist, &dx ,&dy, &dz, 1);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planenum, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_MissileLaunch(int planenum, int targetnum)
{
	char tempstr[1024];
//	char tempstr2[256];
	char callsign[256];
	char positionstr[256];
	float fworkval, tdist, dx, dy, dz;
//	int placeingroup;
	float bearing;
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

//	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	bearing = AIComputeHeadingToPoint(&Planes[planenum], Weapons[targetnum].Pos, &tdist, &dx ,&dy, &dz, 0);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planenum, fworkval);

	if(Weapons[targetnum].LauncherType == GROUNDOBJECT)
	{
		if((Planes[planenum].AI.lAIVoice >= SPCH_WNG1) && (Planes[planenum].AI.lAIVoice <= SPCH_NAVY5))
		{
			tempnum = rand() %3;
		}
		if((Planes[planenum].AI.lAIVoice >= SPCH_ATK_USAF) && (Planes[planenum].AI.lAIVoice <= SPCH_ATK_RUS))
		{
			tempnum = 0;
		}
		else
		{
			tempnum = 2;
		}

		if(tempnum == 0)
		{
			msgnum = AIC_WING_SAM_SPOT_2;
			msgsnd = AICF_WING_SAM_SPOT_2;
		}
		else if(tempnum == 1)
		{
			msgnum = AIC_WING_SAM_SPOT_3;
			msgsnd = AICF_WING_SAM_SPOT_3;
		}
		else if(tempnum == 2)
		{
			msgnum = AIC_WING_SAM_SPOT_4;
			msgsnd = AICF_WING_SAM_SPOT_4;

			if((Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS))
			{
				msgnum = AIC_WING_SAM_SPOT_2;
			}
		}


		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, positionstr))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planenum, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_WING_RADAR_DETECT_6, g_iLanguageId, callsign, positionstr))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(AICF_WING_RADAR_DETECT_6, planenum, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
	}
	return;
}

//**************************************************************************************
void AIGetPPositionStr(char *positionstr, int planenum, float heading, int highlow)
{
	float rheading;

	rheading = heading + AIConvertAngleTo180Degree(Planes[planenum].Heading);

	while(rheading >= 360)
		rheading -= 360;

	while(rheading < 0)
		rheading += 360;

	AIGetPositionStr(positionstr, rheading, highlow);
}


//**************************************************************************************
void AIGetPositionStr(char *positionstr, int heading, int highlow)
{
	char clocknum[128];
	int workval;
	char tempstr[1024];
	char altstr[256];

	workval = (heading + 15) / 30;

	if(workval <= 0)
	{
		workval = 12;
	}

	if(!LANGGetTransMessage(clocknum, 128, AIC_ZERO + workval, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(positionstr, 256, AIC_POSITION, g_iLanguageId, clocknum))
	{
		return;
	}
	if(highlow)
	{
		if(highlow == 1)
		{
			if(!LANGGetTransMessage(altstr, 256, AIC_HIGH, g_iLanguageId))
			{
				return;
			}
		}
		else
		{
			if(!LANGGetTransMessage(altstr, 256, AIC_LOW, g_iLanguageId))
			{
				return;
			}
		}
		if(!LANGGetTransMessage(tempstr, 256, AIC_POSITION_HL, g_iLanguageId, positionstr, altstr))
		{
			return;
		}
		strcpy(positionstr, tempstr);
	}
}

//**************************************************************************************
void AICFlightWeaponCheck()
{

	long delaycnt = 3000;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	PlaneParams *planepnt;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

//	AIC_WSO_Check_Msgs(planepnt - Planes, 1);

	if(planepnt->AI.wingman >= 0)
	{
		AICFlightWeaponCheckPlane(planepnt->AI.wingman, &delaycnt);
	}

	if(planepnt->AI.nextpair >= 0)
	{
		AICFlightWeaponCheckPlane(planepnt->AI.nextpair, &delaycnt);
	}
	return;
}

//**************************************************************************************
void AICFlightWeaponCheckPlane(int planenum, long *delaycnt)
{
	int planeok = 1;
	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);

	if((!AIInPlayerGroup(&Planes[planenum])) && (iAICommFrom < 0))
	{
		return;
	}

	if(((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) >= firstvalid) && ((Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) <= lastvalid))
	{
		if(AICAllowThisRadio(planenum, 1))
		{
			AICAddSoundCall(AICWeaponCheck, planenum, *delaycnt, 50);
			*delaycnt = *delaycnt + 7000;
		}
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICFlightWeaponCheckPlane(Planes[planenum].AI.wingman, delaycnt);
	}

	if(Planes[planenum].AI.nextpair >= 0)
	{
		AICFlightWeaponCheckPlane(Planes[planenum].AI.nextpair, delaycnt);
	}
	return;
}

//**************************************************************************************
void AICWeaponCheck(int planenum, int targetnum)
{
	int numheater, numradar, numgun, numharm, numagm, numguided, numdumb, numcluster, numpod;
	char heaterstr[128];
	char radarstr[128];
	char numstr[128];
	int placeingroup;
	char tempstr[1024];
	char tempstr2[256];
	char tempstr3[256];
	int msgnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

//	AIC_Get_Callsign_With_Number(planenum, callsign);
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	numradar = AIGetNumWeaponType(planenum, 1);
	numheater = AIGetNumWeaponType(planenum, 2);
	numgun = AIGetNumWeaponType(planenum, 3);
	numharm = AIGetNumWeaponType(planenum, 4);
	numagm = AIGetNumWeaponType(planenum, 5);
	numguided = AIGetNumWeaponType(planenum, 6);
	numdumb = AIGetNumWeaponType(planenum, 7);
	numcluster = AIGetNumWeaponType(planenum, 8);
	numpod = AIGetNumWeaponType(planenum, 9);

	if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numheater, g_iLanguageId))
	{
		return;
	}

	msgnum = AIC_HEAT_COUNT;
	if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
	{
		msgnum = AIC_HEAT_COUNT_RUSF;
	}

	if(!LANGGetTransMessage(heaterstr, 128, msgnum, g_iLanguageId, numstr))
	{
		return;
	}

	if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numradar, g_iLanguageId))
	{
		return;
	}
	if(!LANGGetTransMessage(radarstr, 128, AIC_RADAR_COUNT, g_iLanguageId, numstr))
	{
		return;
	}

	if(numharm)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numharm, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_HARM_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numagm)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numagm, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_A2M_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numguided)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numguided, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_GUIDED_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numdumb)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numdumb, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_DUMB_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numcluster)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numcluster, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_CLUSTER_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}
	if(numpod)
	{
		strcat(heaterstr, " ");
		if(!LANGGetTransMessage(numstr, 128, AIC_ZERO + numpod, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr3, 128, AIC_POD_COUNT, g_iLanguageId, numstr))
		{
			return;
		}
		strcat(heaterstr, tempstr3);
	}






	if(numgun)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_WEAPONS_GUN, g_iLanguageId, tempstr2, radarstr, heaterstr))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_WEAPONS_NO_GUN, g_iLanguageId, tempstr2, radarstr, heaterstr))
		{
			return;
		}
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AICWeaponCheckSnd(planenum, numradar, numheater, numgun, numharm, numagm, numguided, numdumb, numcluster, numpod);
	return;
}

//**************************************************************************************
void AICWeaponCheckSnd(int planenum, int numradar, int numheater, int numgun, int numharm, int numagm, int numguided, int numdumb, int numcluster, int numpod)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(channel != CHANNEL_WINGMEN)
	{
		return;
	}

	if(numgun)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_WEAPONS_GUN, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_WEAPONS_NO_GUN, g_iLanguageId))
		{
			return;
		}
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				if(numradar < 10)
				{
					sndids[numids] = (SPCH_ZERO + numradar) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numradar - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (715) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if(numval == 3)
			{
				if(numheater < 10)
				{
					sndids[numids] = (SPCH_ZERO + numheater) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numheater - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (716) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 4) && (numharm))
			{
				if(numharm < 10)
				{
					sndids[numids] = (SPCH_ZERO + numharm) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numharm - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (717) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 5) && (numagm))
			{
				if(numagm < 10)
				{
					sndids[numids] = (SPCH_ZERO + numagm) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numagm - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (718) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 6) && (numguided))
			{
				if(numguided < 10)
				{
					sndids[numids] = (SPCH_ZERO + numguided) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numguided - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (719) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 7) && (numdumb))
			{
				if(numdumb < 10)
				{
					sndids[numids] = (SPCH_ZERO + numdumb) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numdumb - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (720) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 8) && (numcluster))
			{
				if(numcluster < 10)
				{
					sndids[numids] = (SPCH_ZERO + numcluster) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numcluster - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (721) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			if((numval == 9) && (numpod))
			{
				if(numpod < 10)
				{
					sndids[numids] = (SPCH_ZERO + numpod) + Planes[planenum].AI.lAIVoice;
				}
				else
				{
					sndids[numids] = (SPCH_COUNT_ONE + numpod - 1) + Planes[planenum].AI.lAIVoice;
				}
				numids ++;
				sndids[numids] = (722) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
int AIGetNumWeaponType(int planenum, int weapontype)
{
	long weaponid;
	int cnt;
	int returnval = 0;
	PlaneParams *planepnt;
	int wtlist, seeker;
	int tempval;

	planepnt = &Planes[planenum];

	weaponid = pDBWeaponList[planepnt->WeapLoad[planepnt->AI.cActiveWeaponStation].WeapIndex].lWeaponID;
	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		weaponid = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].lWeaponID;
		wtlist = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		if((weapontype < 4) && ((wtlist == 2) || (wtlist == 3) || (wtlist == 4) || (wtlist == 6)))
		{
			seeker = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iSeekerType;
			if(((weapontype == 1) && ((seeker == 1) || (seeker == 7))) || ((weapontype == 2) && ((seeker == 2) || (seeker == 3))) || ((weapontype == 3) && (wtlist == 6)))
			{
				returnval += planepnt->WeapLoad[cnt].Count;
			}
		}
		else if(weapontype >= 4)
		{
			if(weapontype == 4)		//  harms
			{
				if(wtlist == WEAPON_TYPE_AA_ANTIRADAR_MISSILE)
				{
					returnval += planepnt->WeapLoad[cnt].Count;
				}
			}
			else if(weapontype == 5)	//  AG Missiles
			{
				if((wtlist == WEAPON_TYPE_AG_MISSILE) || (wtlist == WEAPON_TYPE_ANTI_SHIP_MISSILE))
				{
					returnval += planepnt->WeapLoad[cnt].Count;
				}
			}
			else if(weapontype == 6)	//  guided
			{
				if(wtlist == WEAPON_TYPE_GUIDED_BOMB)
				{
					returnval += planepnt->WeapLoad[cnt].Count;
				}
			}
			else if(weapontype == 7)	//  dumb
			{
				if(wtlist == WEAPON_TYPE_DUMB_BOMB)
				{
					returnval += planepnt->WeapLoad[cnt].Count;
				}
			}
			else if(weapontype == 8)	//  cluster
			{
				if(wtlist == WEAPON_TYPE_CLUSTER_BOMB)
				{
					returnval += planepnt->WeapLoad[cnt].Count;
				}
			}
			else if(weapontype == 9)	//  Rocket Pods
			{
				if(wtlist == WEAPON_TYPE_AGROCKET)
				{
					tempval = GetNumRocketsPerPod(weaponid);

					if((!tempval) || (!planepnt->WeapLoad[cnt].Count))
					{
						returnval += planepnt->WeapLoad[cnt].Count;
					}
					else
					{
						if(tempval >= planepnt->WeapLoad[cnt].Count)
						{
							returnval += 1;
						}
						else
						{
							returnval += 2;
						}
					}
				}
			}
		}
	}
	return(returnval);
}

//**************************************************************************************
void AISetBreakBehavior(PlaneParams *planepnt, float offangle, WeaponParams *weaponpnt)
{
	float desiredhead;
	float planeheading;
	BYTE breakid = 0;

	if(planepnt->AI.iAICombatFlags1 & (AI_FUEL_TANKS))
	{
		AIDropTanks(planepnt);
	}

	planeheading = AIConvertAngleTo180Degree(planepnt->Heading);

	if(fabs(offangle) > 90)
	{
		desiredhead = (offangle < 0) ? planeheading + (offangle + 90) : planeheading + (offangle - 90);
	}
	else
	{
		desiredhead = (offangle < 0) ? planeheading + (-90 - offangle) : planeheading + (90 - offangle);
	}

	planepnt->AI.DesiredHeading = AIConvert180DegreeToAngle(desiredhead);

	if(planepnt->AI.OrgBehave == NULL)
	{
		planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
		planepnt->AI.iAIFlags2 |= AI_CONTINUE_ORG_BEHAVE;
		breakid = 1;
	}

	if(planepnt->AI.iAICombatFlags2 & AI_ESCORTABLE)
	{
		planepnt->AI.Behaviorfunc = AIMissileBreakToHeading;
		breakid |= 4;
	}
	else if(planepnt->AI.iAIFlags2 & (AI_IN_DOGFIGHT))
	{
		planepnt->AI.iAIFlags2 |= AIMISSILEDODGE;
		planepnt->AI.lTimer2 = 7000;
		breakid |= 2;
	}
	else
	{
		if(weaponpnt->LauncherType != AIRCRAFT)
		{
			planepnt->AI.Behaviorfunc = AIMissileBreakToHeading;
			breakid |= 4;
		}
		else
		{
			AISwitchToAttack(planepnt, 1, 1);
			breakid |= 8;
		}
		planepnt->AI.lTimer2 = 7000;
	}
	if(MultiPlayer && (planepnt->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
	{
		NetPutMissileBreak(planepnt, breakid, weaponpnt);
	}
}

//**************************************************************************************
void AIC_WSO_DoBreakMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		if(targetnum == 0)
		{
			AICGenericBettySpeech(BETTY_LEFT);
		}
		else
		{
			AICGenericBettySpeech(BETTY_RIGHT);
		}
	}

	return;

	if(targetnum == 0)
	{
		msgnum = AIC_BREAK_LEFT;
		msgsnd = AICF_WSO_BREAK_LEFT;
	}
	else
	{
		msgnum = AIC_BREAK_RIGHT;
		msgsnd = AICF_WSO_BREAK_RIGHT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_DoCMMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		if((targetnum == 1) || (targetnum == 7))
		{
			AICGenericBettySpeech(BETTY_CHAFF);
		}
		else
		{
			AICGenericBettySpeech(BETTY_FLARE);
		}
	}

	return;

	if((targetnum == 1) || (targetnum == 7))
	{
		msgnum = AIC_WSO_CHAFF;
		msgsnd = AICF_WSO_CHAFF;
	}
	else
	{
		msgnum = AIC_WSO_FLARE;
		msgsnd = AICF_WSO_FLARE;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Wingman_Defensive_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int weaponindex, seeker;
	int msgnum, msgsnd;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if((Weapons[targetnum].LauncherType == GROUNDOBJECT) || (Weapons[targetnum].LauncherType == MOVINGVEHICLE))
	{
		AICWSOEngageDefensive(planenum, targetnum);
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	weaponindex = AIGetWeaponIndex(Weapons[targetnum].Type->TypeNumber);
	seeker = pDBWeaponList[weaponindex].iSeekerType;

	if((seeker == 1) || (seeker == 7))
	{
		msgnum = AIC_WING_DEFENSIVE_ALAMO;
		msgsnd = AICF_WING_DEFENSIVE_ALAMO;
	}
	else if(seeker == 3)
	{
		msgnum = AIC_WING_DEFENSIVE_ARCHER;
		msgsnd = AICF_WING_DEFENSIVE_ARCHER;
	}
	else
	{
		msgnum = AIC_WING_DEFENSIVE_ARCHER;
		msgsnd = AICF_WING_DEFENSIVE_ARCHER;
//		msgnum = AIC_WING_DEFENSIVE_ATOLL;
//		msgsnd = AICF_WING_DEFENSIVE_ATOLL;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_MissileInbound(int planenum, int targetnum)
{
	char tempstr[1024];
	char positionstr[256];
	int tempnum, msgnum, msgsnd;
	float fworkval, tdist, dx, dy, dz;
	float offangle;
	int workval;


	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	offangle = AIComputeHeadingToPoint(&Planes[planenum], Weapons[targetnum].Pos, &tdist, &dx ,&dy, &dz, 1);

	fworkval = -offangle;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	workval = (fworkval + 15) / 30;

	if(workval <= 0)
	{
		workval = 12;
	}

	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		AICGenericBettySpeech(BETTY_MISSILE_OCLOCK, workval);
	}

	return;

	tempnum = rand() & 2;
	if(tempnum == 0)
	{
		msgnum = AIC_INCOMING_MISSILE;
		msgsnd = AICF_INCOMING_MISSILE;
	}
	else
	{
		msgnum = AIC_MISSILE_INBOUND;
		msgsnd = AICF_MISSILE_INBOUND;
	}

	offangle = AIComputeHeadingToPoint(&Planes[planenum], Weapons[targetnum].Pos, &tdist, &dx ,&dy, &dz, 1);

	fworkval = -offangle;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planenum, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, offangle, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICWSOEngageDefensive(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	int msgnum, msgsnd;
	int weaponindex, weaponid;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(Weapons[targetnum].LauncherType != GROUNDOBJECT)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	weaponindex = AIGetWeaponIndex(Weapons[targetnum].Type->TypeNumber);

	weaponid = pDBWeaponList[weaponindex].lWeaponID;

	switch(weaponid)
	{
		case 60:
			msgnum = AIC_WING_DEFENSIVE_ROLAND;
			msgsnd = AICF_WING_DEFENSIVE_ROLAND;
			break;
		case 61:
			msgnum = AIC_WING_DEFENSIVE_HAWK;
			msgsnd = AICF_WING_DEFENSIVE_HAWK;
			break;
		case 47:
			msgnum = AIC_WING_DEFENSIVE_SA_2;
			msgsnd = AICF_WING_DEFENSIVE_SA_2;
			break;
		case 48:
			msgnum = AIC_WING_DEFENSIVE_SA_3;
			msgsnd = AICF_WING_DEFENSIVE_SA_3;
			break;
		case 49:
			msgnum = AIC_WING_DEFENSIVE_SA_6;
			msgsnd = AICF_WING_DEFENSIVE_SA_6;
			break;
		case 50:
			msgnum = AIC_WING_DEFENSIVE_SA_7;
			msgsnd = AICF_WING_DEFENSIVE_SA_7;
			break;
		case 51:
			msgnum = AIC_WING_DEFENSIVE_SA_8;
			msgsnd = AICF_WING_DEFENSIVE_SA_8;
			break;
		case 52:
			msgnum = AIC_WING_DEFENSIVE_SA_9;
			msgsnd = AICF_WING_DEFENSIVE_SA_9;
			break;
		case 53:
			msgnum = AIC_WING_DEFENSIVE_SA_10;
			msgsnd = AICF_WING_DEFENSIVE_SA_10;
			break;
		case 55:
			msgnum = AIC_WING_DEFENSIVE_SA_13;
			msgsnd = AICF_WING_DEFENSIVE_SA_13;
			break;
		default:
			msgnum = AIC_WING_DEFENSIVE_SAM;
			msgsnd = AICF_WING_DEFENSIVE_SAM;
			break;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIRAddContact(int planenum, int calltype, int placeingroup, int numingroup, int bearing360, int rangenm, int altval)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int sndids[30];
	int numids = 0;

	if(calltype == 3)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_ADD_CONTACT, g_iLanguageId))
		{
			return;
		}
	}
	else if(calltype == 2)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_NL_CONTACT, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_CONTACT_CALL, g_iLanguageId))
		{
			return;
		}
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done);
		tsptr += strcnt;
		switch(numval)
		{
			case 1:
//				AIRWingmenRadioCall(AIR_WC_TWO + (placeingroup - 1), placeingroup);
				sndids[numids] = AIR_WC_TWO + (placeingroup - 1);
				numids ++;
				break;
			case 2:
				if(calltype == 3)
				{
					if(numingroup > 9)
					{
//						AIRWingmenRadioCall(AIR_ADD_GROUP_BR, placeingroup);
						sndids[numids] = AIR_ADD_GROUP_BR;
						numids ++;
					}
					else
					{
//						AIRWingmenRadioCall(AIR_ADD_SINGLE_BR, placeingroup);
						sndids[numids] = AIR_ADD_SINGLE_BR;
						numids ++;
					}
				}
				else
				{
					if(numingroup > 9)
					{
//						AIRWingmenRadioCall(AIR_GROUP_BR, placeingroup);
						sndids[numids] = AIR_GROUP_BR;
						numids ++;
					}
					else
					{
//						AIRWingmenRadioCall(AIR_SINGLE_BR, placeingroup);
						sndids[numids] = AIR_SINGLE_BR;
						numids ++;
					}
				}
				break;
			case 3:
				numids += AIRSendBearingRadio(planenum, bearing360, placeingroup, &sndids[numids]);
				break;
			case 4:
//				AIRWingmenRadioCall(AIR_ONE_MILE + (rangenm - 1), placeingroup);
				sndids[numids] = AIR_ONE_MILE + (rangenm - 1);
				numids ++;
				break;
			case 5:
//				AIRWingmenRadioCall(AIR_ALT_WEEDS - altval, placeingroup);
				sndids[numids] = AIR_ALT_WEEDS - altval;
				numids ++;
				break;
		}
	}

#if 0
	DWORD dwHandle;

 	dwHandle = SndQueueSentence(CHANNEL_WINGMEN, numids, sndids, g_iBaseSpeechSoundLevel);
	SndServiceSound();
#endif
}

//**************************************************************************************
int AIRSendBearingRadio(int planenum, int bearing360, int placeingroup, int *sndids, long groundvoice)
{
	int workval1, workval2;
	int *temppnt;
	int spchstrt;
	long voice;

	if(groundvoice == 0)
	{
		voice = Planes[planenum].AI.lAIVoice;
	}
	else
	{
		voice = groundvoice;
	}
	spchstrt = SPCH_BEARING + voice;

	temppnt = sndids;

	workval1 = bearing360 / 100;
	workval2 = bearing360 % 100;

	if(temppnt == NULL)
	{
		AIRWingmenRadioCall(spchstrt + workval1, placeingroup);
	}
	else
	{
		*temppnt = spchstrt + workval1;
		temppnt ++;
	}

	workval1 = workval2 / 10;
	workval2 = workval2 % 10;

	if(temppnt == NULL)
	{
		AIRWingmenRadioCall(spchstrt + workval1, placeingroup);
	}
	else
	{
		*temppnt = spchstrt + workval1;
		temppnt ++;
	}

	if(temppnt == NULL)
	{
		AIRWingmenRadioCall(spchstrt + workval2, placeingroup);
	}
	else
	{
		*temppnt = spchstrt + workval2;
		temppnt ++;
	}

	return(3);
}

//**************************************************************************************
long AICRGetAIVoice(PlaneParams *planepnt)
{
	long workval;
	int country;

	if(planepnt->Status & PL_DEVICE_DRIVEN)
	{
//		return(SPCH_WSO);
		return(SPCH_WNG1);
	}

	country = AICGetCountryFromPlaneID(pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID);

	if(AIIsFACVoice(planepnt))
	{
		workval = SPCH_FAC_AIR;
		return(workval);
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		workval = SPCH_SAR1;
		return(workval);
	}
	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TANKER)
	{
		if(AIIsTankerVoice(planepnt))
		{
			if(MultiPlayer)
			{
				workval = SPCH_TANK_USN + (((planepnt - Planes) & 1) * 1000);
			}
			else
			{
				workval = SPCH_TANK_USN + ((rand() & 1) * 1000);
			}
			return(workval);
		}
	}
	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_C3)
	{
		if(pDBAircraftList[planepnt->AI.iPlaneIndex].lPlaneID == 20)
		{
			workval = SPCH_JSTARS1;
			return(workval);
		}
		else
		{
			if(MultiPlayer)
			{
				workval = SPCH_AWACS1 + (((planepnt - Planes) & 1) * 1000);
			}
			else
			{
				workval = SPCH_AWACS1 + ((rand() & 1) * 1000);
			}
		}
		return(workval);
	}

	if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
	{
		if(MultiPlayer)
		{
			workval = SPCH_NAVY1 + (((planepnt - Planes) % 5) * 1000);
		}
		else
		{
			workval = SPCH_NAVY1 + ((rand() % 5) * 1000);
		}
		return(workval);
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_BOMBER)
	{
		if(AIIsFACVoice(planepnt))
		{
			workval = SPCH_FAC_AIR;
			return(workval);
		}

		if(AIIsBomberVoice(planepnt) || AIIsSEADVoice(planepnt))
		{
			if(country == AI_US)
			{
				workval = SPCH_ATK_USAF;
			}
			else if(country == AI_RUS)
			{
				workval = SPCH_ATK_RUS;
			}
			else
			{
				workval = SPCH_ATK_RAF;
			}

			return(workval);
		}
	}

	if(pDBAircraftList[planepnt->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_TRANSPORT)
	{
		if(country == AI_US)
		{
			workval = SPCH_ATK_USAF;
		}
		else if(country == AI_RUS)
		{
			workval = SPCH_ATK_RUS;
		}
		else
		{
			workval = SPCH_ATK_RAF;
		}

		return(workval);
	}

	if(country == AI_US)
	{
		workval = SPCH_FTR_USAF;
	}
	else if(country == AI_RUS)
	{
		workval = SPCH_FTR_RUS;
	}
	else if(country == AI_NATO)
	{
		workval = SPCH_FTR_NATO;
	}
	else
	{
		workval = SPCH_FTR_RAF;
	}

	return(workval);
}

//**************************************************************************************
int AICGetCountryFromPlaneID(int planeid)
{
	switch(planeid)
	{
		case 9:
		case 10:
		case 21:
		case 23:
		case 28:
		case 41:
		case 44:
		case 45:
		case 50:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 66:
		case 67:
		case 68:
		case 79:
		case 80:
		case 81:
			return(AI_RUS);
			break;
		case 25:
		case 26:
		case 35:
		case 36:
			return(AI_UK);
			break;
		case 6:
		case 46:
		case 60:
			return(AI_NATO);
			break;
		default:
			return(AI_US);
			break;
	}
	return(AI_US);
}

//**************************************************************************************
int AIIsFACVoice(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_FAC))
			{
				return(1);
			}
		}
		lookway ++;
	}
	return(0);
}

//**************************************************************************************
int AIIsTankerVoice(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_REFUEL_PATTERN))
			{
				return(1);
			}
		}
		lookway ++;
	}
	return(0);
}

//**************************************************************************************
int AIIsSEADVoice(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;

#if 1
	return(AIIsSEADPlane(planepnt));
#endif

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_SEAD) || (AIActions[actcnt].ActionID == ACTION_ESCORT))
			{
				return(1);
			}
		}
		lookway ++;
	}
	return(0);
}

//**************************************************************************************
int AIIsBomberVoice(PlaneParams *planepnt)
{
	MBWayPoints *lookway;
	int waycnt;
	int bombfound = 0;
	int startact, endact, actcnt;
	int numwpts;

	lookway = &AIWayPoints[planepnt->AI.startwpts];

	numwpts = planepnt->AI.numwaypts + (planepnt->AI.CurrWay - &AIWayPoints[planepnt->AI.startwpts]);
	for(waycnt = 0; waycnt < numwpts; waycnt ++)
	{
		startact = lookway->iStartAct;
		endact = startact + lookway->iNumActs;
		for(actcnt = startact; actcnt < endact; actcnt ++)
		{
			if((AIActions[actcnt].ActionID == ACTION_BOMB_TARGET) || (AIActions[actcnt].ActionID == ACTION_CAS)
					|| (AIActions[actcnt].ActionID == ACTION_ATTACKTARGET) || (AIActions[actcnt].ActionID == ACTION_RELEASE_CHUTES))
			{
				return(1);
			}
		}
		lookway ++;
	}
	return(0);
}

extern void LogSentenceEvent(int planenum,int groundvoice,int iChannel,int iVolume,int numids,int *sndids);

//**************************************************************************************
void AIRSendSentence(int planenum, int numids, int *sndids, long groundvoice, int allowinaccel, float volperc)
{
	DWORD dwHandle;
	int channel;
	int iworkvol;
	float fworkvol;

	if((TimeExcel && (!allowinaccel)) || iInJump)
	{
		return;
	}

	if(groundvoice == -2)
	{
		channel = CHANNEL_BETTY;
	}
	else if(groundvoice == 0)
	{
		channel = AIRGetChannel(planenum);
	}
	else
	{
		if((groundvoice == SPCH_FINAL) || (groundvoice == SPCH_LSO))
		{
			channel = CHANNEL_WSO;
		}
		else
		{
			channel = CHANNEL_SPEECH;
		}
	}

	if((iSpeechMute & 1) && (channel != CHANNEL_BETTY))
	{
		return;
	}
	else if((iSpeechMute & 2) && (channel == CHANNEL_BETTY))
	{
		return;
	}

	fworkvol = g_iBaseSpeechSoundLevel;

	if(volperc < 1.0f)
	{
		fworkvol *= volperc;
	}

	if((channel != CHANNEL_WSO) && (channel != CHANNEL_BETTY))
	{
		fworkvol *= 0.9;
	}

	iworkvol = fworkvol;

	if(iworkvol < 0)
	{
		iworkvol = 0;
	}
	else if(iworkvol > g_iBaseSpeechSoundLevel)
	{
		iworkvol = g_iBaseSpeechSoundLevel;
	}

	if(iworkvol)
	{
		PauseForDiskHit();
 		dwHandle = SndQueueSentence(channel, numids, sndids, iworkvol);
		LogSentenceEvent(planenum,groundvoice,channel,iworkvol,numids,sndids);
		SndServiceSound();
		UnPauseFromDiskHit();
	}
}

//**************************************************************************************
int AIRGetChannel(int planenum)
{
	long voice;

	voice = Planes[planenum].AI.lAIVoice;

	switch(voice)
	{
		case SPCH_BETTY:
			return(CHANNEL_BETTY);
			break;
//		case SPCH_WSO:
//			return(CHANNEL_WSO);
//			break;
		case SPCH_WNG1:
		case SPCH_WNG2:
		case SPCH_WNG3:
		case SPCH_WNG4:
		case SPCH_WNG5:
		case SPCH_WNG6:
		case SPCH_WNG7:
		case SPCH_WNG8:
		case SPCH_NAVY1:
		case SPCH_NAVY2:
		case SPCH_NAVY3:
		case SPCH_NAVY4:
		case SPCH_NAVY5:
			return(CHANNEL_WINGMEN);
			break;
		case SPCH_LSO:
		case SPCH_AWACS1:
		case SPCH_AWACS2:
		case SPCH_JSTARS1:
		case SPCH_SAR1:
		case SPCH_FTR_USAF:
		case SPCH_FTR_NATO:
		case SPCH_FTR_RAF:
		case SPCH_FTR_RUS:
		case SPCH_ATK_USAF:
		case SPCH_ATK_RAF:
		case SPCH_ATK_RUS:
		case SPCH_TWR_NATO:
		case SPCH_TWR_RUS:
		case SPCH_STRIKE:
		case SPCH_MARSHAL:
		case SPCH_FINAL:
		case SPCH_FAC_AIR:
		case SPCH_FAC_GRND:
		case SPCH_TANK_USN:
		case SPCH_TANK_USMC:
			return(CHANNEL_SPEECH);
			break;
		default:
			return(CHANNEL_SPEECH);
			break;
	}
	return(CHANNEL_SPEECH);
}

//**************************************************************************************
long AIRGetCallSignID(int planenum)
{
	return(AIRGetCallSignSndID(Planes[planenum].AI.lCallSign, 0));
}

//**************************************************************************************
long AIRGetIntenseCallSignID(int planenum)
{
	return(AIRGetCallSignSndID(Planes[planenum].AI.lCallSign, 1));
}

//**************************************************************************************
long AIRGetCallSignSndID(long callsignnum, int intense)
{
	if(intense)
	{
		return(CallSignList[callsignnum].lIntSpeechID);
	}
	else
	{
		return(CallSignList[callsignnum].lSpeechID);
	}

	return(872);

	int i;
	int planetype, callsignid;

	planetype = (callsignnum & 0xFF00) >> 8;
	callsignid = callsignnum & 0xFF;
	for (i=0;i<(sizeof(CallSignList)/sizeof(CallSignType)); i++)
	{
		if ((callsignid == CallSignList[i].iID) && (planetype == CallSignList[i].iPlaneType))
		{
			if(intense)
			{
				return(CallSignList[i].lIntSpeechID);
			}
			else
			{
				return(CallSignList[i].lSpeechID);
			}
		}
	}
	if(intense)
	{
		return(968);
	}
	else
	{
		return(800);
	}
}

//**************************************************************************************
long AIRGetWngCallNum(int planenum, int placeingroup)
{
//	int i;
//	int planetype, callsignid;
	int callsignid;
	long workval;

#if 0
	if((placeingroup <= 1) || (Planes[planenum].AI.lAIVoice < SPCH_WNG1) || (Planes[planenum].AI.lAIVoice > SPCH_WNG8))
	{
		return(AIRGetCallSignID(planenum));
	}

	planetype = (Planes[planenum].AI.lCallSign & 0xFF00) >> 8;
	callsignid = Planes[planenum].AI.lCallSign & 0xFF;

	workval = 800;
	for (i=0;i<(sizeof(CallSignList)/sizeof(CallSignType)); i++)
	{
		if ((callsignid == CallSignList[i].iID) && (planetype == CallSignList[i].iPlaneType))
		{
			workval = CallSignList[i].lSpeechID;
		}
	}

	workval -= 800;

	workval = 824 + (workval * 7) + (placeingroup - 2);

	return(workval);
#else
	callsignid = Planes[planenum].AI.lCallSign & 0xFF;
	workval = CallSignList[callsignid].lSpeechID;

	workval -= 800;

	workval = 824 + (workval * 7) + (placeingroup - 2);

	return(workval);
#endif
}

//**************************************************************************************
long AIRGetIntWngCallNum(int planenum, int placeingroup)
{
//	int i;
//	int planetype, callsignid;
	int callsignid;
	long workval;

#if 0
	if((placeingroup <= 1) || (Planes[planenum].AI.lAIVoice < SPCH_WNG1) || (Planes[planenum].AI.lAIVoice > SPCH_WNG8))
	{
		return(AIRGetIntenseCallSignID(planenum));
	}

	planetype = (Planes[planenum].AI.lCallSign & 0xFF00) >> 8;
	callsignid = Planes[planenum].AI.lCallSign & 0xFF;

	workval = 968;
	for (i=0;i<(sizeof(CallSignList)/sizeof(CallSignType)); i++)
	{
		if ((callsignid == CallSignList[i].iID) && (planetype == CallSignList[i].iPlaneType))
		{
			workval = CallSignList[i].lIntSpeechID;
		}
	}

	workval -= 968;

	workval = 600 + (workval * 7) + (placeingroup - 2);

	return(workval);
#else
	callsignid = Planes[planenum].AI.lCallSign & 0xFF;

	workval = CallSignList[callsignid].lSpeechID;

	workval -= 968;

	workval = 600 + (workval * 7) + (placeingroup - 2);

	return(workval);
#endif
}

//**************************************************************************************
long AIRGetHeadingSpeechID(int headingval, int planenum)
{
	return(SPCH_CARDINAL + headingval);
}

//**************************************************************************************
long AIRGetHeading2SpeechID(int headingval, int planenum)
{
	return(SPCH_CARDINAL2 + headingval);
}

//**************************************************************************************
void AIRRadarBanditCallSnd(PlaneParams *planepnt, PlaneParams *targplane, int bearing360, float rangenm, int headingval, int numingroup)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int sndids[30];
	int numids = 0;
	int planenum = planepnt - Planes;
	int placeingroup;
	int tempval;

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_RADAR_CONTACT, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done);
		tsptr += strcnt;
		switch(numval)
		{
			case 2:
//				AIRWingmenRadioCall(AIR_WC_TWO + (placeingroup - 1), placeingroup);
				sndids[numids] = AIRWingNumSndID(planenum, placeingroup);
				numids ++;
				break;
			case 3:
				if(numingroup > 9)
				{
//					AIRWingmenRadioCall(AIR_GROUP, placeingroup);
					sndids[numids] = (WING_GROUP_BEARING) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				else if(numingroup == 1)
				{
// **					AIRWingmenRadioCall(AIR_SINGLE, placeingroup);
//					AIRWingmenRadioCall(AIR_BANDIT, placeingroup);
					sndids[numids] = (WING_SINGLE_BEARING) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				else
				{
//					AIRWingmenRadioCall(AIR_BANDIT, placeingroup);
					sndids[numids] = (WING_SINGLE_BEARING) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				break;
			case 4:
				numids += AIRSendBearingRadio(planenum, bearing360, placeingroup, &sndids[numids]);
				break;
			case 5:
//				AIRWingmenRadioCall(AIR_ONE_MILE + (rangenm - 1), placeingroup);
#if 1
				tempval = AICGetRangeSpeechID(planenum, rangenm, sndids, Planes[planenum].AI.lAIVoice);
				numids += tempval;
#else
				tempval = AICGetRangeSpeechID(planenum, rangenm);
				if(tempval >= 0)
				{
					sndids[numids] = tempval + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
#endif
				break;
			case 6:
//				AIRWingmenRadioCall(AIR_ALT_WEEDS - altval, placeingroup);
				if((targplane->WorldPosition.Y * WUTOFT) > 30000)
				{
					sndids[numids] = Planes[planenum].AI.lAIVoice + SPCH_ALT;
					numids ++;
				}
				else if((targplane->HeightAboveGround * WUTOFT) < 10000)
				{
					sndids[numids] = Planes[planenum].AI.lAIVoice + SPCH_ALT + 2;
					numids ++;
				}
				else if((targplane->HeightAboveGround * WUTOFT) < 1000)
				{
					sndids[numids] = Planes[planenum].AI.lAIVoice + SPCH_ALT + 4;
					numids ++;
				}
				else
				{
					sndids[numids] = Planes[planenum].AI.lAIVoice + SPCH_ALT + 3;
					numids ++;
				}
				break;
			case 7:
				sndids[numids] = AIRGetHeadingSpeechID(headingval, planenum) + Planes[planenum].AI.lAIVoice;
				numids ++;
				break;
			case 8:
				if(targplane->AI.iSide == AI_ENEMY)
				{
					sndids[numids] = (WING_BANDIT_BANDIT) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				else
				{
					sndids[numids] = (WING_UNKNOWN) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				break;
		}
	}

	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AIRVisualBanditCallSnd(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float rangenm, int highlow)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int sndids[30];
	int numids = 0;
	int planenum = planepnt - Planes;
	int placeingroup;
	float fworkval;
	int tempval;

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_VISUAL_CONTACT, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done);
		tsptr += strcnt;
		switch(numval)
		{
			case 2:
//				AIRWingmenRadioCall(AIR_WC_TWO + (placeingroup - 1), placeingroup);
				sndids[numids] = AIRWingNumSndID(planenum, placeingroup);
				numids ++;
				break;
			case 3:
				if(bearing < 0)
				{
					sndids[numids] = (WING_RIGHT) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				else
				{
					sndids[numids] = (WING_LEFT) + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
				break;
			case 4:
				fworkval = -bearing;
				if(fworkval < 0)
				{
					fworkval += 360;
				}

//				sndids[numids] = AIGetPositionSpeechID(planenum, fworkval, highlow, 0) + Planes[planenum].AI.lAIVoice;
				tempval = AIGetPositionSpeechID(planenum, fworkval, highlow, sndids, Planes[planenum].AI.lAIVoice, 0);
				numids += tempval;
				break;
			case 6:
//				AIRWingmenRadioCall(AIR_ONE_MILE + (rangenm - 1), placeingroup);
#if 1
				tempval = AICGetRangeSpeechID(planenum, rangenm, sndids, Planes[planenum].AI.lAIVoice);
				numids += tempval;
#else
				tempval = AICGetRangeSpeechID(planenum, rangenm);
				if(sndids[numids] >= 0)
				{
					sndids[numids] = tempval + Planes[planenum].AI.lAIVoice;
					numids ++;
				}
#endif
				break;
			case 7:
				sndids[numids] = AICGetAspectSpeechID(planepnt - Planes, bearing, targplane);
				numids ++;
				break;
		}
	}

	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
long AICGetAspectSpeechID(int planenum, float bearing, PlaneParams *planepnt)
{
	float workbearing;
	float tbearing;

	workbearing = bearing - 180;
	workbearing -= AIConvertAngleTo180Degree(planepnt->Heading);
	tbearing = -workbearing;

	tbearing = AICapAngle(tbearing);

	if(fabs(tbearing) < 45)  //  CLOSING
	{
		return((WING_CLOSING) + Planes[planenum].AI.lAIVoice);
	}
	else if(fabs(tbearing) > 135)  //  GOING AWAY
	{
		return((WING_GOING_AWAY) + Planes[planenum].AI.lAIVoice);
	}
	else  // FLANKING
	{
		return((WING_FLANKING) + Planes[planenum].AI.lAIVoice);
	}
	return((WING_FLANKING) + Planes[planenum].AI.lAIVoice);
}

//**************************************************************************************
long AICGetRangeSpeechID(int planenum, float rangenm, int *sndids, int voice)
{
	long lrangenm;
	long workrange;
	int numids = 0;

	lrangenm = (long)rangenm;

	if(lrangenm <= 1)
	{
		sndids[numids] = SPCH_ONE_MILE + voice;
		numids ++;
	}
	else if(lrangenm <= 60)
	{
		sndids[numids] = (SPCH_MILES + lrangenm) + voice;
		numids ++;
		if(voice != 17000)
		{
			sndids[numids] = SPCH_MILES + voice;
			numids ++;
		}
	}
	else if(lrangenm <= 200)
	{
		workrange = lrangenm / 5;
		sndids[numids] = SPCH_MILES + 60 + (workrange - 12) + voice;
		numids ++;
		if(voice != 17000)
		{
			sndids[numids] = SPCH_MILES + voice;
			numids ++;
		}
		 //	if(!LANGGetTransMessage(tstr, 128, AIC_EIGHTY + ((workrange - 7) * 2), g_iLanguageId))
	}

	return(numids);
}


//**************************************************************************************
int AIGetPositionSpeechID(int planenum, float heading, int highlow, int *sndids, int voice, int intense)
{
	int workval;
	float rheading;
	int numids = 0;

	rheading = heading + AIConvertAngleTo180Degree(Planes[planenum].Heading);

	while(rheading > 360)
		rheading -= 360;

	while(rheading < 0)
		rheading += 360;

	workval = rheading;

	workval = (workval + 15) / 30;

	if(workval <= 0)
	{
		workval = 12;
	}

	if(!intense)
	{
		sndids[numids] = SPCH_COUNT_ONE + (workval - 1) + voice;
		numids ++;
		if(highlow == 1)
		{
			sndids[numids] = SPCH_O_CLOCK_H + voice;
			numids ++;
		}
		else if (highlow == -1)
		{
			sndids[numids] = SPCH_O_CLOCK_L + voice;
			numids ++;
		}
		else
		{
			sndids[numids] = SPCH_O_CLOCK + voice;
			numids ++;
		}
	}
	else
	{
		if(workval < 10)
		{
			sndids[numids] = SPCH_ZERO_INTENSE + (workval) + voice;
		}
		else
		{
			sndids[numids] = SPCH_COUNT_ONE + (workval - 1) + voice;
		}

		numids ++;
		if(highlow == 1)
		{
			sndids[numids] = SPCH_INT_O_CLOCK_H + voice;
			numids ++;
		}
		else if (highlow == -1)
		{
			sndids[numids] = SPCH_INT_O_CLOCK_H + 1 + voice;
			numids ++;
		}
		else
		{
			sndids[numids] = SPCH_INT_O_CLOCK_H + 2 + voice;
			numids ++;
		}
	}
	return(numids);
}

//**************************************************************************************
void AIRGenericSpeech(long messageid, int planenum, int targetnum, float bearing, int headingval, int headingval2, float rangenm, long alt, long secs, int numingroup, long groundvoice, int allowinaccel, float volperc)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int spchvar;
	int numval;
	int strcnt;
	int sndids[30];
	int numids = 0;
	long voice;

	if((TimeExcel && (!allowinaccel)) || iInJump)
	{
		return;
	}

	if(groundvoice)
	{
		voice = groundvoice;
	}
	else
	{
		voice = Planes[planenum].AI.lAIVoice;
	}


	if(!LANGGetTransMessage(tempstr, 1024, messageid, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, targetnum, bearing, headingval, headingval2, rangenm, alt, secs, numingroup, groundvoice);
		}
		else
		{
			sndids[numids] = numval + voice;
			numids ++;
		}
	}

	AIRSendSentence(planenum, numids, sndids, groundvoice);
}

//**************************************************************************************
int AIRProcessSpeechVars(int *sndids, int genericid, int planenum, int targetnum, float bearing, int headingval, int headingval2, float rangenm, long alt, long secs, int numingroup, long groundvoice)
{
	int placeingroup, tplaceingroup;
	float tbearing;
	int bearing360;
	long voice;
	int tempval;
	float rbearing;
	int weirdfloathack;
	PlaneParams *leadplane, *playerlead;

	playerlead = AIGetLeader(PlayerPlane);
	if(groundvoice)
	{
		placeingroup = 1;
		voice = groundvoice;
		leadplane = NULL;
	}
	else
	{
		placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
		voice = Planes[planenum].AI.lAIVoice;
		leadplane = AIGetLeader(&Planes[planenum]);
	}

	switch(genericid)
	{
		case 1:		//  Callsign
			if(groundvoice)
			{
				if(planenum > 100)
				{
					*sndids = planenum + voice;
					return(1);
				}

				*sndids = SPCH_STRIKE_CS + (planenum - 1) + voice;
				return(1);
			}

			*sndids = AIRGetCallSignID(planenum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO + Planes[planenum].AI.cCallSignNum + voice;

			sndids ++;
			*sndids = SPCH_ZERO + 1 + voice;

			sndids ++;
			*sndids = DEAD_SPACE;
//			if(AIRCheckCallSignSpchExists(*sndids))
//			{
//				return(1);
//			}
//			else
//			{
//				return(0);
//			}
			return(4);
			break;
		case 2:		//  Callsign number
			if(groundvoice)
			{
				*sndids = SPCH_STRIKE_CS + (planenum - 1) + voice;

				sndids ++;
				*sndids = DEAD_SPACE;
				return(2);
			}

			*sndids = AIRGetCallSignID(planenum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO + Planes[planenum].AI.cCallSignNum + voice;
			sndids ++;

			*sndids = AIRWingNumSndID(planenum, placeingroup) + voice;

			sndids ++;
			*sndids = DEAD_SPACE;
//			*sndids = AIRGetWngCallNum(planenum, placeingroup) + voice;
			return(4);
			break;
		case 3:		//  number
			if(groundvoice)
			{
				*sndids = SPCH_STRIKE_CS + (planenum - 1) + voice;
				sndids ++;
				*sndids = DEAD_SPACE;
				return(2);
			}

			if(leadplane != playerlead)
				return(AIRProcessSpeechVars(sndids, 2, planenum, targetnum, bearing, headingval, headingval2, rangenm, alt, secs, numingroup, groundvoice));

			*sndids = AIRWingNumSndID(planenum, placeingroup) + voice;

			sndids ++;
			*sndids = DEAD_SPACE;
			return(2);
			break;
		case 4:		//  Callsign intense
			*sndids = AIRGetIntenseCallSignID(planenum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO_INTENSE + Planes[planenum].AI.cCallSignNum + voice;

			sndids ++;
			*sndids = SPCH_ZERO_INTENSE + 1 + voice;

			sndids ++;
			*sndids = DEAD_SPACE;
			return(4);
			break;
		case 5:		//  Callsign number intense
			*sndids = AIRGetIntenseCallSignID(planenum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO_INTENSE + Planes[planenum].AI.cCallSignNum + voice;
			sndids ++;

			*sndids = AIRIntWingNumSndID(planenum, placeingroup) + voice;

			sndids ++;
			*sndids = DEAD_SPACE;
//			*sndids = AIRGetIntWngCallNum(planenum, placeingroup) + voice;
			return(4);
			break;
		case 6:		//  number intense
			if(leadplane != playerlead)
				return(AIRProcessSpeechVars(sndids, 5, planenum, targetnum, bearing, headingval, headingval2, rangenm, alt, secs, numingroup, groundvoice));

			*sndids = AIRIntWingNumSndID(planenum, placeingroup) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(2);
			break;
		case 7:		//  bearing
			tbearing = -bearing;
			if(tbearing < 0)
			{
				bearing360 = tbearing + 360;
			}
			else
			{
				bearing360 = tbearing;
			}
			return(AIRSendBearingRadio(planenum, bearing360, placeingroup, sndids, groundvoice));
			break;
		case 8:		//  Cardinal Heading
			if((voice == 26000) || (voice == 27000))
				return(AIRProcessSpeechVars(sndids, 7, planenum, targetnum, bearing, headingval, headingval2, rangenm, alt, secs, numingroup, groundvoice));

			*sndids = AIRGetHeadingSpeechID(headingval, planenum) + voice;
			return(1);
			break;
		case 9:		//  Cardinal Heading 2  (western or 'west of bullseye'  depending on plane)
			*sndids = AIRGetHeading2SpeechID(headingval2, planenum) + voice;
			return(1);
			break;
		case 10:	//  rangenm
#if 1
			tempval = AICGetRangeSpeechID(planenum, rangenm, sndids, voice);
			return(tempval);
#else
			if(rangenm > 60)
			{
				if((voice == SPCH_CAP1) || (voice == SPCH_CAP2) || (voice == SPCH_CAP3))
				{
//					*sndids = AICGetRangeSpeechID(planenum, 60) + voice;
//					return(1);
					return(0);
				}
			}
			tempval = AICGetRangeSpeechID(planenum, rangenm);
			if(tempval >= 0)
			{
				*sndids = tempval + voice;
				return(1);
			}
			else
			{
				return(0);
			}
#endif
			break;
		case 11:	//  Altitude (high, med, low)
			*sndids = voice + SPCH_ALT + alt;
			return(1);
			break;
		case 12:	//  Seconds to impact (may need more of these for type (nav, bomb, loft)
			if(voice == SPCH_WSO)
			{
				if(secs == 5)
				{
					*sndids = voice + (WSO_5_SEC_IMPACT - SPCH_WSO);
				}
				else
				{
					*sndids = voice + ((WSO_60_SEC_IMPACT - SPCH_WSO) + (6 - (secs / 10)));
				}
				return(1);
			}
			else
			{
				return(0);
			}
			break;
		case 13:	//  Altitude (high, low, level)
			*sndids = SPCH_VERY_HIGH + alt + voice;
			return(1);
			break;
		case 14:	//  Bandit, unknown
			if(Planes[targetnum].AI.iSide == AI_ENEMY)
			{
				*sndids = WING_BANDIT_BANDIT + voice;
			}
			else
			{
				*sndids = WING_UNKNOWN + voice;
			}
			return(1);
			break;
		case 15:	//  Aspect
			*sndids = AICGetAspectSpeechID(planenum, bearing, &Planes[targetnum]);
			return(1);
			break;
		case 16:	//  Seconds to release
			if(voice == SPCH_WSO)
			{
				if(secs == 5)
				{
					*sndids = voice + (WSO_5_SEC_RELEASE - SPCH_WSO);
				}
				else
				{
					*sndids = voice + ((WSO_60_SEC_RELEASE - SPCH_WSO) + (6 - (secs / 10)));
				}
				return(1);
			}
			else
			{
				return(0);
			}
			break;
		case 17:	//  Seconds to pull
			if(voice == SPCH_WSO)
			{
				if(secs == 0)
				{
					*sndids = voice + (WSO_PULL_NOW - SPCH_WSO);
				}
				if(secs == 5)
				{
					*sndids = voice + (WSO_5_SEC_PULL - SPCH_WSO);
				}
				else
				{
					*sndids = voice + ((WSO_60_SEC_PULL - SPCH_WSO) + (6 - (secs / 10)));
				}
				return(1);
			}
			else
			{
				return(0);
			}
			break;
		case 18:	//  Minutes to nav point
			if(voice == SPCH_WSO)
			{
				*sndids = voice + ((WSO_120_MIN_NAV - SPCH_WSO) + (12 - (secs / 10)));
				return(1);
			}
			else
			{
				return(0);
			}
			break;
		case 19:	//  Miles to target
			if(voice == SPCH_WSO)
			{
				weirdfloathack = rangenm;
				*sndids = voice + ((WSO_60_MILE_TARGET - SPCH_WSO) + (6 - (weirdfloathack / 10)));
				return(1);
			}
			else
			{
				return(0);
			}
			break;
		case 20:
			if(numingroup > 9)
			{
				*sndids = (WING_GROUP) + voice;
			}
			else if(numingroup == 1)
			{
				*sndids = (WING_SINGLE) + voice;
			}
			else
			{
				*sndids = (WING_BANDIT) + voice;
			}
			return(1);
			break;
		case 21:			//  Not yet converted
			if(voice == SPCH_WSO)
			{
				if(numingroup > 9)
				{
					*sndids = (567) + voice;
				}
				else if(numingroup == 1)
				{
					*sndids = (568) + voice;
				}
				else
				{
					*sndids = (569) + voice;
				}
				sndids ++;
				*sndids = 500 + voice;
				return(2);
			}
			else
			{
				if(numingroup > 9)
				{
					*sndids = (WING_GROUP_BEARING - SPCH_WNG1) + voice;
					return(1);
				}
				else if(numingroup == 1)
				{
	// **					AIRWingmenRadioCall(AIR_SINGLE, placeingroup);
					*sndids = (WING_SINGLE_BEARING - SPCH_WNG1) + voice;
					return(1);
				}
				else
				{
					*sndids = (WING_SINGLE_BEARING - SPCH_WNG1) + voice;
					return(1);
				}
			}
			break;
		case 22:			//  Not yet converted
			if(numingroup > 9)
			{
				*sndids = (WING_ADDITIONAL_GROUP_BEARING - SPCH_WNG1) + voice;
				return(1);
			}
			else if(numingroup == 1)
			{
// **					AIRWingmenRadioCall(AIR_SINGLE, placeingroup);
				*sndids = (WING_ADDITIONAL_SINGLE_BEARING - SPCH_WNG1) + voice;
				return(1);
			}
			else
			{
				*sndids = (WING_ADDITIONAL_SINGLE_BEARING - SPCH_WNG1) + voice;
				return(1);
			}
			break;
		case 23:
			rbearing = bearing - AIConvertAngleTo180Degree(Planes[planenum].Heading);

			while(rbearing > 180)
				rbearing -= 360;

			while(rbearing <= -180)
				rbearing += 360;

			if(rbearing < 0)
			{
				*sndids = (WING_RIGHT) + voice;
				return(1);
			}
			else
			{
				*sndids = (WING_LEFT) + voice;
				return(1);
			}
			break;
		case 24:
			tbearing = -bearing;
			if(tbearing < 0)
			{
				tbearing += 360;
			}

//			*sndids = AIGetPositionSpeechID(planenum, tbearing, alt, 0) + voice;
			tempval = AIGetPositionSpeechID(planenum, tbearing, alt, sndids, voice, 0);
			return(tempval);
			break;
		case 25:
//			*sndids = AIRGetFlightSndID(planenum) + voice;
			*sndids = AIRGetCallSignID(planenum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO + Planes[planenum].AI.cCallSignNum + voice;

			sndids ++;
			*sndids = SPCH_ZERO + 1 + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(4);
			break;
		case 26:
//			*sndids = AIRGetFlightSndID(targetnum) + voice;
			*sndids = AIRGetCallSignID(targetnum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO + Planes[targetnum].AI.cCallSignNum + voice;

			sndids ++;
			*sndids = SPCH_ZERO + 1 + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(4);
			break;
		case 27:		//  Callsign
			if(targetnum > (LastPlane - Planes))
			{
//				*sndids = AIRGetCallSignSndID(targetnum, 0) + voice;
				*sndids = targetnum + voice;
				sndids ++;
				*sndids = DEAD_SPACE;
				return(2);
			}

			*sndids = AIRGetCallSignID(targetnum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO + Planes[targetnum].AI.cCallSignNum + voice;

			sndids ++;
			*sndids = SPCH_ZERO + 1 + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(4);
			break;
		case 28:		//  Callsign number
			*sndids = AIRGetCallSignID(targetnum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO + Planes[targetnum].AI.cCallSignNum + voice;

			sndids ++;
			tplaceingroup = (Planes[targetnum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
			*sndids = AIRWingNumSndID(targetnum, tplaceingroup) + voice;
//			*sndids = AIRGetWngCallNum(targetnum, tplaceingroup) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(4);
			break;
		case 29:		//  number
			if(AIGetLeader(&Planes[targetnum]) != playerlead)
				return(AIRProcessSpeechVars(sndids, 2, planenum, targetnum, bearing, headingval, headingval2, rangenm, alt, secs, numingroup, groundvoice));

			tplaceingroup = (Planes[targetnum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
			*sndids = AIRWingNumSndID(targetnum, tplaceingroup) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(2);
			break;
		case 30:		//  Callsign intense
			*sndids = AIRGetIntenseCallSignID(targetnum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO_INTENSE + Planes[targetnum].AI.cCallSignNum + voice;

			sndids ++;
			*sndids = SPCH_ZERO_INTENSE + 1 + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(4);
			break;
		case 31:		//  Callsign number intense
			*sndids = AIRGetIntenseCallSignID(targetnum) + voice;
			sndids ++;

			*sndids = SPCH_ZERO_INTENSE + Planes[targetnum].AI.cCallSignNum + voice;

			sndids ++;
			tplaceingroup = (Planes[targetnum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
			*sndids = AIRIntWingNumSndID(targetnum, tplaceingroup) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
//			*sndids = AIRGetIntWngCallNum(planenum, tplaceingroup) + voice;
			return(4);
			break;
		case 32:		//  number intense
			if(AIGetLeader(&Planes[targetnum]) != playerlead)
				return(AIRProcessSpeechVars(sndids, 31, planenum, targetnum, bearing, headingval, headingval2, rangenm, alt, secs, numingroup, groundvoice));

			tplaceingroup = (Planes[targetnum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
			*sndids = AIRIntWingNumSndID(planenum, tplaceingroup) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(2);
			break;
		case 33:	//  number
			*sndids = AICGetFtSpeechID(planenum, rangenm) + voice;
			return(1);
			break;
		case 34:	//  Ground callsign (planenum)
//			*sndids = CallSignList[planenum].lSpeechID + voice;
			*sndids = SPCH_STRIKE_CS + (planenum - 1) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(2);
			break;
		case 35:	//  Ground callsign (targetnum)
//			*sndids = CallSignList[targetnum].lSpeechID + voice;
			*sndids = SPCH_STRIKE_CS + (targetnum - 1) + voice;
			sndids ++;
			*sndids = DEAD_SPACE;
			return(2);
			break;
		case 36:  // General Aspect
			*sndids = WING_FLANKING + voice + headingval2;
			return(1);
		case 37:  //  Intense Clock Position
			tbearing = -bearing;
			if(tbearing < 0)
			{
				tbearing += 360;
			}

//			*sndids = AIGetPositionSpeechID(planenum, tbearing, alt, 0) - SPCH_O_CLOCK_H + SPCH_INT_O_CLOCK_H + voice;
			tempval = AIGetPositionSpeechID(planenum, tbearing, alt, sndids, voice, 1);

			return(tempval);
			break;
		case 38:
			if(secs == 2)
			{
				return(0);
			}
			if(secs == 1)
			{
				*sndids = 462 + voice;  //   Right now just Fast
			}
			else
			{
				*sndids = 463 + voice;	//  Right now just Slow
			}
			return(1);
			break;
		case 39:		//  Altitude3  Angles X
		case 40:		//  Altitude2  X Thousand
			if(genericid == 39)
			{
				*sndids = SPCH_ANGELS + voice;
				sndids ++;
			}
//			tempval = (alt + 500) / 1000;
			tempval = alt / 1000;

			*sndids = AIRGetNumSpeech(tempval) + voice;

			if(genericid == 40)
			{
				sndids ++;
				*sndids = SPCH_THOUSAND + voice;
			}

			return(2);
			break;
		case 41:	//
			if(pDBAircraftList[Planes[targetnum].AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
			{
				*sndids = ((numingroup == 1) ? 502: 503) + voice;  //  Enemy chopper / enemy choppers at
			}
			else
			{
				*sndids = ((numingroup == 1) ? 500: 501) + voice;  //  Bandit Bandit / has Bandits at
			}
			return(1);
			break;
		case 42:
			*sndids = SPCH_STATE + voice;
			sndids ++;
			tempval = Planes[planenum].TotalFuel / 1000.0f;
			*sndids = AIRGetNumSpeech(tempval) + voice;
			sndids ++;

			*sndids = SPCH_POINT + voice;
			sndids ++;

			tempval = Planes[planenum].TotalFuel + 50;
			tempval = (tempval % 1000) / 100;
			*sndids = AIRGetNumSpeech(tempval) + voice;

			return(4);
			break;
		case 43:	//  Cardinal Bullseye off headingval2
			*sndids = AIRGetHeadingSpeechID(headingval2, planenum) + voice;
			sndids ++;

			*sndids = SPCH_BULLSEYE + voice;
			return(2);
			break;
		case 44:	//  Digital Bullseye off headingval2
			*sndids = SPCH_BULLSEYE + voice;
			sndids ++;

			tbearing = -headingval2;
			if(tbearing < 0)
			{
				bearing360 = tbearing + 360;
			}
			else
			{
				bearing360 = tbearing;
			}
			return(AIRSendBearingRadio(planenum, bearing360, placeingroup, sndids, groundvoice) + 1);
			break;
		case 45:
			if(numingroup < 0)
			{
				*sndids = SPCH_ZERO + voice;
			}
			else if(numingroup > 20)
			{
				*sndids = SPCH_FIRST + voice;
			}
			else
			{
				*sndids = SPCH_FIRST + (numingroup - 1) + voice;
			}
			return(1);
			break;
	}

	return(0);
}

//**************************************************************************************
int AIRGetNumSpeech(int tempval)
{
	int sndids;

	if(tempval <= 0)
	{
		sndids = SPCH_ZERO;
	}
	else if(tempval <= 60)
	{
		sndids = SPCH_COUNT_ONE - 1 + tempval;
	}
	else
	{
		if(tempval > 200)
		{
			tempval = 200;
		}

		tempval /= 5;
		sndids = SPCH_COUNT_ONE + 60 + (tempval - 12) - 1;
	}
	return(sndids);
}

//**************************************************************************************
long AIRGetFlightSndID(int planenum)
{
	return(900);  //  I don't think I"m calling this anymore.  Would have to be changed for F/A-18
				//  anyway.



//	int i;
//	int planetype, callsignid;
	int callsignid;
	long workval;

#if 0
	planetype = (Planes[planenum].AI.lCallSign & 0xFF00) >> 8;
	callsignid = Planes[planenum].AI.lCallSign & 0xFF;

	workval = 800;
	for (i=0;i<(sizeof(CallSignList)/sizeof(CallSignType)); i++)
	{
		if ((callsignid == CallSignList[i].iID) && (planetype == CallSignList[i].iPlaneType))
		{
			workval = CallSignList[i].lSpeechID;
		}
	}

	workval -= 800;

	workval = 812 + workval;

	return(workval);
#else
	callsignid = Planes[planenum].AI.lCallSign & 0xFF;
	workval = CallSignList[callsignid].lSpeechID;
	workval -= 800;

	workval = 812 + workval;

	return(workval);
#endif
}

//**************************************************************************************
void AICSetUpTowerVoice(FPointDouble runwaypos)
{
	long lrx, lrz;

	lrx = runwaypos.X * WUTOFT;
	lrz = runwaypos.Z * WUTOFT;

	if(!AICCheckOneFence(FenceArea[3].iNumPoints, FenceArea[3].point, lrx, lrz))
	{
		lPlayerRunwayVoice = SPCH_TWR_NATO;
	}
	else
	{
		lPlayerRunwayVoice = SPCH_TWR_RUS;
	}

	//  May want to randomize here for Ground/Tower/Arrival
	iPlayerRunwayID = 1;
}

//**************************************************************************************
long AIGetTowerCallsign(int planenum, long *retvoice)
{
	long returnval;
	long callsignnum;
	long voice;

	callsignnum = iPlayerRunwayID;
	if((callsignnum < 0) || (callsignnum > 2))
	{
		callsignnum = 1;
	}

	returnval = callsignnum;

	voice = lPlayerRunwayVoice;
	if((lPlayerRunwayVoice != SPCH_TWR_NATO) && (lPlayerRunwayVoice != SPCH_TWR_RUS))
	{
		voice = SPCH_TWR_NATO;
	}

	if(retvoice != NULL)
	{
		*retvoice = voice;
	}

	return(returnval);
}

//**************************************************************************************
long AIGetFACCallsign(int planenum, long *retvoice)
{
	long returnval;

#if 0
	if((FACObject.lCallSign >> 8) == 0)
	{
		if(FACObject.lSide)
		{
//			returnval = (CALLSIGN_GROUNDUNIT_UKFAC<<8) + FACObject.lCallSign;
			returnval = FACObject.lCallSign;
		}
		else
		{
//			returnval = (CALLSIGN_GROUNDUNIT_USFAC<<8) + FACObject.lCallSign;
			returnval = FACObject.lCallSign;
		}
	}
#else
	returnval = FACObject.lCallSign;
#endif

	if(retvoice != NULL)
	{
//		if(FACObject.lSide)
//		{
//			*retvoice = SPCH_UK_GRND1;
//		}
//		else
//		{
//			*retvoice = SPCH_US_GRND1;
//		}
		*retvoice = SPCH_FAC_GRND;
	}

	returnval = AICGetCallSignNum(returnval);
	return(returnval);
}

//**************************************************************************************
void AICCheckForFriendlyTraffic(int planenum, int targetnum)
{
	PlaneParams *checkplane;
	PlaneParams *returnplane = NULL;
	float dx, dy, dz, tdist, tdistnm;
	float offangle;
	float founddist = 50.0f;
	long ldistnm;
	PlaneParams *planepnt;
	int altval;
	char callsign[256];
	long towernum;
	char towercallsign[256];
	long towervoice;
	char altstr[256];
	char bearstr[256];
	char rangestr[128];
	float tbearing;
	int bearing360;
	char tempstr[1024];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	planepnt = &Planes[planenum];
	checkplane = &Planes[targetnum];

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (planepnt->AI.iSide == checkplane->AI.iSide)
				&& (checkplane->OnGround == 0) && (checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
		{
			dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
			tdist = QuickDistance(dx, dz);
			tdistnm = (tdist * WUTONM);
			ldistnm = (long)tdistnm;
			if((ldistnm <= 15.0) && (ldistnm >= 5))
			{
				offangle = atan2(-dx, -dz) * 57.2958;

				offangle = AICapAngle(offangle);


				AICGetCallSign(planenum, callsign);
				towernum = AIGetTowerCallsign(planenum, &towervoice);
//				strcpy(towercallsign, CallSignList[towernum].sName);
				if(!LANGGetTransMessage(towercallsign, 256, AIC_GROUND_CS + towernum, g_iLanguageId))
				{
					return;
				}

				tbearing = -offangle;
				if(tbearing < 0)
				{
					bearing360 = tbearing + 360;
				}
				else
				{
					bearing360 = tbearing;
				}
				sprintf(bearstr, "%03d", bearing360);

#if 0
				if((checkplane->WorldPosition.Y * WUTOFT) > 40000)
				{
					altval = 0;
					if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
					{
						return;
					}
				}
				else if((checkplane->WorldPosition.Y * WUTOFT) > 30000)
				{
					altval = 1;
					if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
					{
						return;
					}
				}
				else if((checkplane->HeightAboveGround * WUTOFT) < 10000)
				{
					altval = 3;
					if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
					{
						return;
					}
				}
				else
				{
					altval = 2;
					if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
					{
						return;
					}
				}
#else
				altval = (checkplane->WorldPosition.Y * WUTOFT) / 1000.0f;
				if(altval > 33)
				{
					altval = 33;
				}

				sprintf(altstr, "%d", altval);

				if(!LANGGetTransMessage(altstr, 128, AIC_THOUSAND, g_iLanguageId, altstr))
				{
					return;
				}
#endif

				AICGetRangeText(rangestr, ldistnm);

				if(!LANGGetTransMessage(tempstr, 1024, AIC_TOWER_FRIENDLY_TRAFFIC_2, g_iLanguageId, callsign, bearstr, rangestr, altstr))
				{
					return;
				}

				AICAddAIRadioMsgs(tempstr, 40);

				AIRGenericSpeech(AICF_TOWER_FRIENDLY_TRAFFIC, towernum + 5, planenum, offangle, 0, 0, ldistnm, (checkplane->WorldPosition.Y * WUTOFT), 0, 0, towervoice);

				AICAddSoundCall(AICCheckForFriendlyTraffic, planenum, 9000, 40, (checkplane - Planes) + 1);

				return;
			}
		}
		checkplane ++;
	}
	return;
}

//**************************************************************************************
void AICTankerDeadMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	tempnum = rand() & 3;

	if(tempnum == 1)
	{
		msgnum = AIC_TANKER_GOING_DOWN;
		msgsnd = AICF_TANKER_GOING_DOWN;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_TANKER_BAIL_OUT;
		msgsnd = AICF_TANKER_BAIL_OUT;
	}
	else if(tempnum == 3)
	{
		msgnum = AIC_TANKER_ABANDON_SHIP;
		msgsnd = AICF_TANKER_ABANDON_SHIP;
	}
	else
	{
		msgnum = AIC_TANKER_HIT_GOING_IN;
		msgsnd = AICF_TANKER_HIT_GOING_IN;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICSARDeadMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	int awacsnum;
	char awacscallsign[256];
	char callsign[256];
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum == -1)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AICGetCallSign(awacsnum, awacscallsign);

	tempnum = rand() % 3;

	if(tempnum == 1)
	{
		msgnum = AIC_SAR_GOING_DOWN_1;
		msgsnd = AICF_SAR_GOING_DOWN;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_SAR_MAYDAY;
		msgsnd = AICF_SAR_MAYDAY;
	}
	else
	{
		msgnum = AIC_SAR_MAYDAY_DOWN;
		msgsnd = AICF_SAR_MAYDAY_DOWN;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SAR_Hit_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum == -1)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AICGetCallSign(awacsnum, awacscallsign);


	if((targetnum == NONE) || (targetnum == BOMB) || (targetnum == MISSILE) || (targetnum == BULLET))
	{
		tempnum = rand() % 6;
	}
	else
	{
		tempnum = rand() % 8;
	}

	if(tempnum == 0)
	{
		msgnum = AIC_SAR_WE_TAKING_FIRE;
		msgsnd = AICF_SAR_WE_TAKING_FIRE;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_SAR_TAKING_FIRE;
		msgsnd = AICF_SAR_TAKING_FIRE;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_SAR_WE_ARE_HIT;
		msgsnd = AICF_SAR_WE_ARE_HIT;
	}
	else if(tempnum == 3)
	{
		msgnum = AIC_SAR_HEAVY_DAMAGE;
		msgsnd = AICF_SAR_HEAVY_DAMAGE;
	}
	else if(tempnum == 4)
	{
		msgnum = AIC_SAR_NEED_COVER;
		msgsnd = AICF_SAR_NEED_COVER;
	}
	else if(tempnum == 5)
	{
		msgnum = AIC_SAR_NEED_HELP;
		msgsnd = AICF_SAR_NEED_HELP;
	}
	else if(tempnum == 6)
	{
		msgnum = AIC_SAR_AAA_FIRE;
		msgsnd = AICF_SAR_AAA_FIRE;
	}
	else
	{
		msgnum = AIC_SAR_HEAVY_AAA;
		msgsnd = AICF_SAR_HEAVY_AAA;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_SARMissileWarn(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	char awacscallsign[256];
	int awacsnum;
	int issam = 0;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum == -1)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AICGetCallSign(awacsnum, awacscallsign);


	if(issam)
	{
		tempnum = rand() & 3;
	}
	else
	{
		tempnum = rand() & 1;
	}

	if(tempnum == 0)
	{
		msgnum = AIC_SAR_LAUNCH_LAUNCH;
		msgsnd = AICF_SAR_LAUNCH_LAUNCH;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_SAR_MISSILE_LAUNCH;
		msgsnd = AICF_SAR_MISSILE_LAUNCH;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_SAR_SAM_SAM;
		msgsnd = AICF_SAR_SAM_SAM;
	}
	else
	{
		msgnum = AICF_SAR_SAM_LAUNCH;
		msgsnd = AICF_SAR_SAM_LAUNCH;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICAWACSDeadMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(!LANGGetTransMessage(callsign, 256, AIC_THIS_IS, g_iLanguageId, callsign))
	{
		return;
	}

	tempnum = rand() % 3;

	if(tempnum == 1)
	{
		msgnum = AIC_AWACS_GOING_IN;
		msgsnd = AICF_AWACS_GOING_IN;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_AWACS_MAYDAY;
		msgsnd = AICF_AWACS_MAYDAY;
	}
	else
	{
		msgnum = AIC_AWACS_HIT_SEND_HELP;
		msgsnd = AICF_AWACS_HIT_SEND_HELP;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_AWACSMissileWarn(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(!LANGGetTransMessage(callsign, 256, AIC_THIS_IS, g_iLanguageId, callsign))
	{
		return;
	}

	if(AIIsCAPing(&Planes[planenum]))
	{
		tempnum = 1;
	}
	else
	{
		tempnum = 0;
	}

	if(tempnum == 0)
	{
		msgnum = AIC_AWACS_MISSILES_INBOUND;
		msgsnd = AICF_AWACS_MISSILES_INBOUND;
	}
	else
	{
		msgnum = AIC_AWACS_RUNNING;
		msgsnd = AICF_AWACS_RUNNING;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICJSTARSDeadMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(!LANGGetTransMessage(callsign, 256, AIC_THIS_IS, g_iLanguageId, callsign))
	{
		return;
	}

	tempnum = rand() % 3;

	if(tempnum == 1)
	{
		msgnum = AIC_JSTARS_GOING_DOWN;
		msgsnd = AICF_JSTARS_GOING_DOWN;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_JSTARS_MAYDAY;
		msgsnd = AICF_JSTARS_MAYDAY;
	}
	else
	{
		msgnum = AIC_JSTARS_SEND_HELP;
		msgsnd = AICF_JSTARS_SEND_HELP;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_JSTARSAttackWarn(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		AIC_JSTARSAttackWarn(AIGetLeader(&Planes[planenum]) - Planes, targetnum);
	}

	AICGetCallSign(planenum, callsign);

	if(!LANGGetTransMessage(callsign, 256, AIC_THIS_IS, g_iLanguageId, callsign))
	{
		return;
	}

	tempnum = rand() & 1;

	if(tempnum == 0)
	{
		msgnum = AIC_JSTARS_UNDER_ATTACK;
		msgsnd = AICF_JSTARS_UNDER_ATTACK;
	}
	else
	{
		msgnum = AIC_JSTARS_BANDITS_CLOSING;
		msgsnd = AICF_JSTARS_BANDITS_CLOSING;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICBomberDeadMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char playercallsign[256];
	char callsign[256];
	int placeingroup;

	return;

	if(!((Planes[planenum].AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)) && (Planes[planenum].AI.iAIFlags1 & (AI_SEEN_PLAYER))))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(PlayerPlane - Planes, playercallsign);

	tempnum = rand() & 1;

	if(tempnum == 1)
	{
		msgnum = AIC_BOMBER_EJECT;
		msgsnd = AICF_BOMBER_EJECT;
	}
	else
	{
		if(Planes[planenum].AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_MAYDAY;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_MAYDAY;
		}
		msgsnd = AICF_BOMBER_MAYDAY;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICBomberHitMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char playercallsign[256];
	char callsign[256];
	int placeingroup;

	return;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!((Planes[planenum].AI.iAIFlags1 & (AI_FORMED_PLAYER|AI_ESCORTED_PLAYER)) && (Planes[planenum].AI.iAIFlags1 & (AI_SEEN_PLAYER))))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(PlayerPlane - Planes, playercallsign);

	tempnum = rand() & 1;

	if(tempnum == 1)
	{
		if(Planes[planenum].AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_DAMAGE;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_DAMAGE;
		}
		msgsnd = AICF_BOMBER_DAMAGE;
	}
	else
	{
		if(Planes[planenum].AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_WE_ARE_HIT;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_WE_ARE_HIT;
		}
		msgsnd = AICF_BOMBER_WE_ARE_HIT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICSEADDeadMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	msgnum = AIC_WING_EJECTING;
	msgsnd = AICF_WING_EJECTING;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICCoverSweepMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	char covercallsign[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, covercallsign);


	if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF) || (Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
	{
		msgnum = AIC_BEGINNING_SWEEP_RAF;  //  AIC_CAP_BEGINNING_SWEEP;
	}
	else
	{
		msgnum = AIC_BEGINNING_SWEEP;  //  AIC_CAP_BEGINNING_SWEEP;
	}
	msgsnd = AICF_CAP_BEGINNING_SWEEP;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, covercallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICCheckForBanditCalls(PlaneParams *planepnt, PlaneParams *targetpnt, float targetbearing, float targetrange)
{
	float visualrange, visualrangenm;

	visualrangenm = AI_VISUAL_RANGE_NM;
	visualrange = (visualrangenm * NMTOWU);

	if((planepnt->AI.lAIVoice == SPCH_CAP1) || (planepnt->AI.lAIVoice == SPCH_CAP2) || (planepnt->AI.lAIVoice == SPCH_CAP3))
	{
		if(AIIsCAPing(planepnt))
		{
			AICCAPRadarCall(planepnt, targetpnt, targetbearing, targetrange);
		}
		else if(planepnt->AI.LinkedPlane == PlayerPlane)
		{
			if(targetrange < visualrange)
			{
				AICCAPTallyCall(planepnt, targetpnt, targetbearing, targetrange);
			}
			else
			{
				AICCAPRadarCall(planepnt, targetpnt, targetbearing, targetrange);
			}
		}
	}
	else if((planepnt->AI.lAIVoice == SPCH_US_BOMB1) || (planepnt->AI.lAIVoice == SPCH_UK_BOMB1))
	{
		AICBomberBanditCall(planepnt, targetpnt, targetbearing, targetrange);
	}
}



//**************************************************************************************
void AICCAPRadarCall(PlaneParams *planepnt, PlaneParams *targplane, float targetbearing, float targetrange)
{
	int placeingroup;
	char callsign[128];
	char tempstr[1024];
	char groupstr[128];
	char altstr[128];
	char headingstr[128];
	char bearstr[128];
	char rangestr[128];
	char speedstr[128];
	char awacscallsign[256];
	int numingroup;
	int bearing360;
	int planenum;
	float rangenm;
	float tbearing;
	int headingval;
	int altval;
	int dobullseye = 0;
	int awacsnum;
	int speedval = 0;
	float bearing = targetbearing;
	float range = targetrange;
	float dx, dy, dz;

	planenum = planepnt - Planes;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		planepnt = AIGetLeader(planepnt);
		planenum = planepnt - Planes;
		targetbearing = AIComputeHeadingToPoint(planepnt, targplane->WorldPosition, &targetrange, &dx ,&dy, &dz, 0);
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum == -1)
	{
		return;
	}

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		dobullseye = 1;
		bearing = AIComputeHeadingFromBullseye(targplane->WorldPosition, &range, &dx, &dz);

		rangenm = (range * WUTONM);
		if(rangenm > 140)
		{
			dobullseye = 0;
			bearing = targetbearing;
			range = targetrange;
		}
	}

	AICGetCallSign(awacsnum, awacscallsign);

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	AIC_Get_Callsign_With_Number(planenum, callsign);

	numingroup = AICGetIfPlaneGroupText(groupstr, targplane, 1);

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if((targplane->WorldPosition.Y * WUTOFT) > 40000)
	{
		altval = 0;
		if(!LANGGetTransMessage(altstr, 128, AIC_VERY_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->WorldPosition.Y * WUTOFT) > 30000)
	{
		altval = 1;
		if(!LANGGetTransMessage(altstr, 128, AIC_HIGH, g_iLanguageId))
		{
			return;
		}
	}
	else if((targplane->HeightAboveGround * WUTOFT) < 10000)
	{
		altval = 3;
		if(!LANGGetTransMessage(altstr, 128, AIC_LOW, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		altval = 2;
		if(!LANGGetTransMessage(altstr, 128, AIC_MEDIUM, g_iLanguageId))
		{
			return;
		}
	}

	if((targplane->V * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) < 300)
	{
		speedval = 0;
		if(!LANGGetTransMessage(speedstr, 128, AIC_SLOW_MOVING, g_iLanguageId))
		{
			return;
		}
	}
	else if(((targplane->V * (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) > 600) || (AICalculateMach(targplane->Altitude, (float)targplane->V) > 1.0f))
	{
		speedval = 1;
		if(!LANGGetTransMessage(speedstr, 128, AIC_FAST_MOVING, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		speedval = 2;
		speedstr[0] = 0;
	}

	headingval = AICGetTextHeadingGeneral(headingstr, bearing);

	rangenm = (range * WUTONM);
	AICGetRangeText(rangestr, rangenm);

	if(dobullseye)
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_CAP_CONTACT, g_iLanguageId, awacscallsign, callsign, speedstr, groupstr, headingstr, rangestr, altstr))
		{
			return;
		}

		AIRGenericSpeech(AICF_CAP_CONTACT, planenum, awacsnum, bearing, 0, headingval, rangenm, altval, speedval, numingroup);
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AIC_CAP_CONTACT_BEARING, g_iLanguageId, awacscallsign, callsign, speedstr, groupstr, bearstr, rangestr, altstr))
		{
			return;
		}

		AIRGenericSpeech(AICF_CAP_CONTACT_BEARING, planenum, awacsnum, bearing, 0, headingval, rangenm, altval, speedval, numingroup);
	}

	AICAddAIRadioMsgs(tempstr, 40);
}

//**************************************************************************************
void AICCAPTallyCall(PlaneParams *planepnt, PlaneParams *targetpnt, float targetbearing, float targetrange)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	int morethanone = 0;
	int planenum = planepnt - Planes;
	float dx, dy, dz;
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		planepnt = AIGetLeader(planepnt);
		planenum = planepnt - Planes;
		targetbearing = AIComputeHeadingToPoint(planepnt, targetpnt->WorldPosition, &targetrange, &dx ,&dy, &dz, 0);
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if((targetpnt->AI.wingman >= 0) || (targetpnt->AI.winglead >= 0) || (targetpnt->AI.nextpair >= 0) || (targetpnt->AI.prevpair >= 0))
	{
		morethanone = 1;
	}

	if(morethanone)
	{
		msgnum = AIC_CAP_TALLY_BANDITS;
		msgsnd = AICF_CAP_TALLY_BANDITS;
	}
	else
	{
		msgnum = AIC_CAP_TALLY_BANDIT;
		msgsnd = AICF_CAP_TALLY_BANDIT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICBomberBanditCall(PlaneParams *planepnt, PlaneParams *targetpnt, float targetbearing, float targetrange)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
//	char callsign[256];
	char clockstr[256];
	int morethanone = 0;
	int planenum = planepnt - Planes;
	float altdiff;
	int highlow;
	float fworkval;
	float dx, dy, dz;
	int placeingroup;

	return;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		planepnt = AIGetLeader(planepnt);
		planenum = planepnt - Planes;
		targetbearing = AIComputeHeadingToPoint(planepnt, targetpnt->WorldPosition, &targetrange, &dx ,&dy, &dz, 0);
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	fworkval = -targetbearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	altdiff = (targetpnt->WorldPosition.Y - planepnt->WorldPosition.Y) * WUTOFT;
	if(altdiff > 1000)
	{
		highlow = 1;
	}
	else if(altdiff < -1000)
	{
		highlow = -1;
	}
	else
	{
		highlow = 0;
	}

	AIGetPPositionStr(clockstr, planenum, fworkval, highlow);


//	AIC_Get_Callsign_With_Number(planenum, callsign);

	tempnum = rand() & 1;
	if(tempnum)
	{
		if(planepnt->AI.lAIVoice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_HEADS_UP_BANDITS;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_HEADS_UP_BANDITS;
		}
		msgsnd = AICF_BOMBER_HEADS_UP_BANDITS;
	}
	else
	{
		msgnum = AIC_BOMBER_BANDITS;
		msgsnd = AICF_BOMBER_BANDITS;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, clockstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, targetbearing, 0, 0, 0, highlow, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICCheckForEngageMsg(int planenum, int targetnum)
{
	PlaneParams *planepnt;
	PlaneParams *targetplane;
	int channel;

	planepnt = &Planes[planenum];
	targetplane = &Planes[targetnum];

	if(planepnt->AI.iSide != PlayerPlane->AI.iSide)
	{
		return;
	}

	channel =  AIRGetChannel(planenum);

#if 1
	AICCAPEngagingMsg(planenum, targetnum);
#else
	if(channel == CHANNEL_WINGMEN)
	{
		AICCAPEngagingMsg(planenum, targetnum);
//		AICEngageMsg(planenum, targetnum);
	}
	else
	{
		if(AIIsCAPing(planepnt))
		{
			AICCAPEngagingMsg(planenum, targetnum);
		}
		else if(planepnt->AI.LinkedPlane == PlayerPlane)
		{
			AICEscortInterceptMsg(planenum, targetnum);
		}
	}
#endif
}

//**************************************************************************************
void AICCAPEngagingMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	int morethanone = 0;
	PlaneParams *targetpnt = &Planes[targetnum];
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		planenum = AIGetLeader(&Planes[planenum]) - Planes;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	morethanone = AICMorePlanesNear(targetpnt);

	if(morethanone)
	{
		if(rand() & 1)
		{
			msgnum = AIC_CAP_ENGAGING_BANDITS;
			msgsnd = AICF_CAP_ENGAGING_BANDITS;
		}
		else
		{
			msgnum = AIC_CAP_INTERCEPT_BANDITS_2;
			msgsnd = AICF_CAP_INTERCEPT_BANDITS;
		}
	}
	else
	{
		if(rand() & 1)
		{
			msgnum = AIC_CAP_ENGAGING_BANDIT;
			msgsnd = AICF_CAP_ENGAGING_BANDIT;
		}
		else
		{
			msgnum = AIC_CAP_INTERCEPT_BANDIT_2;
			msgsnd = AICF_CAP_INTERCEPT_BANDIT;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICEscortInterceptMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	char escortedcallsign[256];
	int morethanone = 0;
	PlaneParams *planepnt = &Planes[planenum];
	PlaneParams *targetpnt = &Planes[targetnum];
	int placeingroup;


	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		planepnt = AIGetLeader(planepnt);
		planenum = planepnt - Planes;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!planepnt->AI.LinkedPlane)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planepnt->AI.LinkedPlane - Planes, escortedcallsign);

	morethanone = AICMorePlanesNear(targetpnt);

	if(morethanone)
	{
		msgnum = AIC_CAP_INTERCEPT_BANDIT;
		msgsnd = AICF_CAP_INTERCEPT_BANDIT;
	}
	else
	{
		msgnum = AIC_CAP_INTERCEPT_BANDITS;
		msgsnd = AICF_CAP_INTERCEPT_BANDITS;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, escortedcallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, planepnt->AI.LinkedPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_CAP_Kill_Msg(int planenum, int targetnum)
{
	AIC_Wingman_Kill_Msg(planenum, targetnum);
	return;

	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];
	int morethanone = 0;
	PlaneParams *targetpnt = &Planes[targetnum];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	tempnum = rand() & 1;

	if(tempnum)
	{
		msgnum = AIC_CAP_BANDIT_KILLED;
		msgsnd = AICF_CAP_BANDIT_KILLED;
	}
	else
	{
		msgnum = AIC_CAP_SPLASH_ONE;
		msgsnd = AICF_CAP_SPLASH_ONE;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_CAPMissileWarn(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	char callsign[256];
	int issam = 0;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(issam)
	{
		tempnum = 1;
	}
	else
	{
		return;
	}

	if(tempnum == 1)
	{
		msgnum = AIC_WING_DEFENSIVE_SAM;
		msgsnd = AICF_CAP_ENGAGED_SAM;
	}
	else
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICCAPDeadMsg(int planenum, int targetnum)
{
	AIC_Wingman_Ejecting_Msg(planenum, targetnum);

	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	int awacsnum;
	char awacscallsign[256];
	char callsign[256];
	int placeingroup;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
	if((Planes[planenum].AI.wingman < 0) && (Planes[planenum].AI.nextpair < 0))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(awacsnum == -1)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AICGetCallSign(awacsnum, awacscallsign);

	tempnum = 1;

	if(tempnum == 1)
	{
		msgnum = AIC_CAP_EJECTING;
		msgsnd = AICF_CAP_EJECTING;
	}
	else
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICCAPDone(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	int awacsnum;
	char awacscallsign[256];
	char callsign[256];
	PlaneParams *planepnt = &Planes[planenum];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

	if(targetnum != -1)
	{
		awacsnum = targetnum;
	}

	if(awacsnum == -1)
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AICGetCallSign(awacsnum, awacscallsign);

#if 0
	if((planepnt->AI.numwaypts > 2) && (PlayerPlane != planepnt))
	{
		msgnum = AIC_CAP_FUEL_TANKER;
		msgsnd = AICF_CAP_FUEL_TANKER;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}
		return;
	}
	else
	{
#endif
		if(rand() & 1)
		{
			msgnum = AIC_WSO_J_HEADING_HOME;
			msgsnd = AICF_WSO_J_HEADING_HOME;

			if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
			{
				return;
			}
		}
		else
		{
			msgnum = AIC_RTB_3;
			msgsnd = AICF_RTB_3;
			if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, callsign))
			{
				return;
			}
		}
//	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_CAP_WinchesterMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	int placeingroup;
	int awacsnum;
	char awacscallsign[256];

	if(!(lNoRepeatFlags[planenum] & WINCHESTER_SAID))
	{
		lNoRepeatFlags[planenum] |= WINCHESTER_SAID;
		if(lNoRepeatTimer[planenum] < 60000)
		{
			lNoRepeatTimer[planenum] = 60000;
		}
		awacsnum = AIGetClosestAWACS(&Planes[planenum]);

		if(awacsnum < 0)
		{
			return;
		}

		AICGetCallSign(awacsnum, awacscallsign);

		//  Make sound call for basic ack.

	//  	AIRBasicAckSnd(placeingroup);

		//  Add message to message array.

		if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			return;
		}

		if(!AICAllowThisRadio(planenum, 1))
		{
			return;
		}

		placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	//	{
	//		return;
	//	}

		AIC_Get_Callsign_With_Number(planenum, callsign);

		msgnum = AIC_WINCHESTER_2;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_WINCHESTER_RUSF_2;
		}

		msgsnd = AICF_CAP_WINCHESTER;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	}

	AICCAPDone(planenum, targetnum);

	if(AIAllGroupAAWinchester(AIGetLeader(&Planes[planenum])))
	{
		AISendGroupHome(&Planes[planenum]);
	}
	return;
}

//**************************************************************************************
void AICPlayerCoverDone(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char escortcallsign[256];
	char callsign[256];
	PlaneParams *planepnt = &Planes[planenum];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, escortcallsign);

	if(Planes[planenum].AI.numwaypts > 2)
	{
		msgnum = AIC_CAP_ON_WAY;
		msgsnd = AICF_CAP_ON_WAY;
	}
	else
	{
		AICCAPDone(planenum, targetnum);
		return;

		tempnum = rand() & 1;

		if(tempnum)
		{
			msgnum = AIC_CAP_HEADED_HOME;
			msgsnd = AICF_CAP_HEADED_HOME;
		}
		else
		{
			msgnum = AIC_CAP_RTB;
			msgsnd = AICF_CAP_RTB;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, escortcallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICPlayerSEADCoverDone(int planenum, int targetnum)
{
	AICCAPDone(planenum, targetnum);
	return;

	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char escortcallsign[256];
	char callsign[256];
	PlaneParams *planepnt = &Planes[planenum];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}
	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);
	AIC_Get_Callsign_With_Number(targetnum, escortcallsign);

	if(Planes[planenum].AI.numwaypts > 2)
	{
		msgnum = AIC_SEAD_ON_WAY;
		msgsnd = AICF_SEAD_ON_WAY;
	}
	else
	{
		tempnum = 1;

		if(tempnum)
		{
			msgnum = AIC_SEAD_HEADED_HOME;
			msgsnd = AICF_SEAD_HEADED_HOME;
		}
		else
		{
			return;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, escortcallsign, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICShowEscortReleaseCommOps()
{
	int boxcolor = iMsgBoxColor;
	int textcolor = iMsgTextColor;
	int invalidcolor = iNoSelectTextColor;
	int fontheight;
	int theight = 0;
	int width;
	int planenum = PlayerPlane - Planes;
	PlaneParams *checkplane;
	int optionnum = 1;

	width = gAICommMenu.lMaxStringPixelLen;
	if(width > 640)
	{
		width = 640;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(optionnum == 10)
					{
						optionnum = 0;
					}
					theight = AICDisplayEscorts(GrBuffFor3D, AIC_RELEASE_ESCORT_OPTIONS, optionnum, fontheight, boxcolor, textcolor, MessageFont, 0, theight, width, checkplane - Planes);
					if(optionnum == 0)
					{
						return;
					}
					optionnum ++;
				}
			}
		}
		checkplane ++;
	}
}

//**************************************************************************************
int AICPixelLenEscortReleaseCommOps()
{
	int messagelen = 0;
	int worklen;
	char tempstr[1024];
	char callsign[256];
	char escortstr[256];
	PlaneParams *checkplane;
	int optionnum = 1;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					AIC_Get_Callsign_With_Number(checkplane - Planes, callsign);
					if((checkplane->AI.lAIVoice == SPCH_SEAD1) || (checkplane->AI.lAIVoice == SPCH_SEAD2))
					{
						if(!LANGGetTransMessage(escortstr, 256, AIC_SEAD, g_iLanguageId))
						{
							return(640);
						}
					}
					else
					{
						if(!LANGGetTransMessage(escortstr, 256, AIC_COVER, g_iLanguageId))
						{
							return(640);
						}
					}
					if(!LANGGetTransMessage(tempstr, 1024, AIC_RELEASE_ESCORT_OPTIONS, g_iLanguageId, optionnum, callsign, escortstr))
					{
						return(640);
					}
					worklen = TXTGetStringPixelLength(tempstr, MessageFont);
					if(worklen > messagelen)
					{
						messagelen = worklen;
					}

					optionnum ++;
				}
			}
		}
		checkplane ++;
	}

	return(messagelen + 5);
}

//**************************************************************************************
void AICEscortReleaseCommKeyOps(int keyflag, WPARAM wParam)
{
	int planenum;
	int optionnum = 1;
	int lookforopt;
	PlaneParams *checkplane;

	if(iAICommFrom == -1)
	{
		planenum = PlayerPlane - Planes;
	}
	else
	{
		planenum = iAICommFrom;
	}

	lookforopt = wParam - '0';
	if((lookforopt < 0) || (lookforopt > 9))
	{
		gAICommMenu.AICommMenufunc = NULL;
		gAICommMenu.AICommKeyfunc = NULL;
		gAICommMenu.lTimer = -1;
		return;
	}
	else if(lookforopt == 0)
	{
		lookforopt = 10;
	}

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.Behaviorfunc == AIFlyEscort) && (checkplane->AI.LinkedPlane == PlayerPlane))
			{
				if((checkplane->AI.winglead == -1) && (checkplane->AI.prevpair == -1))
				{
					if(optionnum == lookforopt)
					{
						if(AIIsSEADVoice(checkplane))
						{
							AICPlayerSEADCoverDone(checkplane - Planes, planenum);
						}
						else
						{
							AICPlayerCoverDone(checkplane - Planes, planenum);
						}

						if(checkplane->AI.numwaypts > 1)
						{
							AIReleaseEscort(PlayerPlane, checkplane - Planes, (checkplane->AI.CurrWay - AIWayPoints) + 1);
							if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutGenericMessage3Long(PlayerPlane, GM3L_ESCORT_RELEASE, checkplane - Planes, (checkplane->AI.CurrWay - AIWayPoints) + 1);
							}

						}
						else
						{
							AIReleaseEscort(PlayerPlane, checkplane - Planes, checkplane->AI.startwpts);
							if((MultiPlayer) && (checkplane->AI.iAICombatFlags1 & AI_MULTI_ACTIVE))
							{
								NetPutGenericMessage3Long(PlayerPlane, GM3L_ESCORT_RELEASE, checkplane - Planes, (checkplane->AI.startwpts));
							}
						}
						gAICommMenu.AICommMenufunc = NULL;
						gAICommMenu.AICommKeyfunc = NULL;
						gAICommMenu.lTimer = -1;
						return;
					}
					optionnum ++;
				}
			}
		}
		checkplane ++;
	}

	gAICommMenu.AICommMenufunc = NULL;
	gAICommMenu.AICommKeyfunc = NULL;
	gAICommMenu.lTimer = -1;
}

//**************************************************************************************
int AICDisplayEscorts(GrBuff *usebuff, int messageid, int optionnum, int orgboxheight, int boxcolor, int textcolor, GrFontBuff *usefont, int startx, int starty, int width, int planenum)
{
	char tempstr[1024];
	int theight;
	int messageheight;
	int boxheight;
	int done = 0;
	char callsign[256];
	char escortstr[256];
	PlaneParams *planepnt = &Planes[planenum];
	int tempbottom = usebuff->ClipBottom;

	usebuff->ClipBottom = 240;

//	if(lpDD)
//	  lpDD->FlipToGDISurface();

	boxheight = orgboxheight;

	AIC_Get_Callsign_With_Number(planenum, callsign);
//	if((planepnt->AI.lAIVoice == SPCH_SEAD1) || (planepnt->AI.lAIVoice == SPCH_SEAD2))
	if(AIIsSEADVoice(planepnt))
	{
		if(!LANGGetTransMessage(escortstr, 256, AIC_SEAD, g_iLanguageId))
		{
			usebuff->ClipBottom = tempbottom;
			return(starty);
		}
	}
	else
	{
		if(!LANGGetTransMessage(escortstr, 256, AIC_COVER, g_iLanguageId))
		{
			usebuff->ClipBottom = tempbottom;
			return(starty);
		}
	}
	if(!LANGGetTransMessage(tempstr, 1024, messageid, g_iLanguageId, optionnum, callsign, escortstr))
	{
  		usebuff->ClipBottom = tempbottom;
		return(starty);
	}

	while(!done)
	{
		theight = starty + 1;
		TXTDrawBoxFillRel(usebuff, startx, theight - 1, width, boxheight + 2, boxcolor);
		messageheight = TXTWriteStringInBox(tempstr, usebuff, usefont, 1, theight, width - 2, 480 - (starty + 2 + orgboxheight), textcolor, 1, NULL, 0, 2);
		theight += messageheight;
		if(boxheight >= (messageheight - 2))
		{
			done = 1;
		}
		else
		{
			boxheight = messageheight - 2;
		}
	}
  	usebuff->ClipBottom = tempbottom;
	return(theight);
}

//**************************************************************************************
int AICMorePlanesNear(PlaneParams *planepnt)
{
	float nearrange = (10 * NMTOWU);
	float dx, dz;
	PlaneParams *checkplane;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if(((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.iSide == planepnt->AI.iSide) && (checkplane != planepnt))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				if((fabs(dx) < nearrange) && (fabs(dz) < nearrange))
				{
					return(1);
				}
			}
		}
		checkplane ++;
	}
	return(0);
}

//**************************************************************************************
void AICRequestPlayerPicture()
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	int foundplane = -1;
	int awacsnum;
	int minrange = -1;
	float dx, dz, tdist;
	float radarrange;
	DBRadarType *radar;
	PlaneParams *planepnt;
	FPointDouble apos;
  	MovingVehicleParams *carrier = NULL;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
	{
		if(planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
//			MovingVehicleParams *carrier;
			carrier = &MovingVehicles[planepnt->AI.iHomeBaseId];

			if(!((carrier->Status & VL_DESTROYED) || (!(carrier->Status & VL_ACTIVE))))
			{
				awacsnum = -2;
				apos = carrier->WorldPosition;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		apos = Planes[awacsnum].WorldPosition;
	}

	if(lAWACSPictTimer >= 0)
	{
		AICAddSoundCall(AIC_AWACS_Unchanged, awacsnum, 2000, 50);
		return;
	}

	lAWACSPictTimer = 300000;


	if((awacsnum == -2) && (carrier))
	{
		radar = VGetMaxShipRadarPtr(carrier);
	}
	else if(awacsnum < 0)
	{
		radar = GetRadarPtr(pDBAircraftList[planepnt->AI.iPlaneIndex].lRadarID);
	}
	else
	{
		radar = GetRadarPtr(pDBAircraftList[Planes[awacsnum].AI.iPlaneIndex].lRadarID);
	}

	if(!radar)
	{
		return;
	}

	radarrange = radar->iMaxRange * NMTOWU;

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)))
		{
			if(checkplane->AI.iSide != planepnt->AI.iSide)
			{
				//  Ingore anyone out of AWACS radar range.
				dx = checkplane->WorldPosition.X - apos.X;
				dz = checkplane->WorldPosition.Z - apos.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist <= radarrange)
				{
					checkplane->AI.iAIFlags1 &= ~AI_AWACS_IGNORE;
				}
				else
				{
					checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
				}
			}
			else
			{
				checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
			}
		}
		checkplane ++;
	}

	if(awacsnum >- 0)
	{
		Planes[awacsnum].AI.lVar3 = 0;
	}

#ifndef __DEMO__
	AICAddSoundCall(AIC_AWACS_Acknowledged, planepnt - Planes, 1000, 50, awacsnum);
#endif
	AICAddSoundCall(AICContinuePicture, 10000, 5000, 50, minrange);
}

//**************************************************************************************
void AICDelayPictureRequest(int planenum, int tempnum)
{
//	AICRequestPlayerPicture();
	AICRequestPlayerBogieDope();
}

//**************************************************************************************
void AICContinuePicture(int nextdelay, int ftminrange)
{
	PlaneParams *planepnt;
	int awacsnum;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
		awacsnum = -2;

	AICContinuePictureNearPlane(&nextdelay, ftminrange, planepnt, awacsnum, 0, 0, 0);
}

//**************************************************************************************
int AICContinuePictureNearPlane(int *nextdelay, int ftminrange, PlaneParams *planepnt, int awacsnum, int currentcnt, int hasgroups, int additional)
{
	float minrange;
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	float maxrange;
	int numhide;
	int holddelay = *nextdelay;
	int returnval;
//	int planenum;
	float offangle, toffangle, toffpitch, offpitch;
	float radarangleyaw, radaranglepitch;
	int widthok, heightok;
	float widthangle, heightangle;
	int radarid;
	DBRadarType *radar;
	float ftempval;
	FPointDouble targetpos;
	int isseen;
	float jammingmod;
	float radarrange;


	if(awacsnum == -1)
	{
		if(!planepnt->AI.AirTarget)
		{
			planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
			planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
		}
		radarangleyaw = planepnt->AI.fRadarAngleYaw;
		radaranglepitch = planepnt->AI.fRadarAnglePitch;

		radarid = pDBAircraftList[planepnt->AI.iPlaneIndex].lRadarID;
		radar = GetRadarPtr(radarid);

		if(radar)
		{
			ftempval = radar->iSearchAzimuth;
			ftempval *= 1.0f - (planepnt->AI.iSkill * 0.1f);
			widthangle = ftempval;

			ftempval = radar->iSearchElevation;
			ftempval *= 1.0f - (planepnt->AI.iSkill * 0.1f);
			heightangle = ftempval;

			jammingmod = AIGetPlaneJammingMod(planepnt, radaranglepitch, radarangleyaw, widthangle, heightangle);

			radarrange = radar->iMaxRange * jammingmod * NMTOWU;
		}
		else
		{
			ftempval = 20.0f * (1.0f - (planepnt->AI.iSkill * 0.1f));
			widthangle = ftempval;
			ftempval = 20.0f * (1.0f - (planepnt->AI.iSkill * 0.1f));
			heightangle = ftempval;
			radarrange = 0;
		}
	}

	minrange = ftminrange * FTTOWU;
	foundrange = maxrange = (140 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)) && (checkplane->OnGround == 0))
		{
			if((checkplane->AI.iSide != planepnt->AI.iSide) && (!((checkplane->AI.iAIFlags1 & AI_AWACS_IGNORE) && (awacsnum != -1))) && (!(((checkplane->AI.iAICombatFlags2 & (AI_TEMP_IGNORE_CHECK)) || (checkplane->AI.iAIFlags2 & (AI_REPORTED_AS_CONTACT))) && (awacsnum == -1))))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < minrange)
				{
					checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
					checkplane->AI.iAICombatFlags2 |= (AI_TEMP_IGNORE_CHECK);
				}
				else if(tdist < foundrange)
				{
					if(awacsnum == -1)
					{
						dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
						dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
						dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
						tdist = QuickDistance(dx, dz);
						isseen = 0;
						widthok = 0;
						heightok = 0;

						if(radarrange > tdist)
						{
							offangle = atan2(-dx, -dz) * 57.2958;
							toffangle = offangle - radarangleyaw;
							toffangle = AICapAngle(toffangle);
							if(fabs(toffangle) < widthangle)
							{
								widthok = 1;
							}
							offpitch = (atan2(dy, tdist) * 57.2958);
							toffpitch = offpitch - radaranglepitch;
							toffpitch = AICapAngle(toffpitch);
							if(fabs(toffpitch) < heightangle)  //  Plane in radar area
							{
								heightok = 1;
							}
						}

						if(widthok && heightok)
						{
							targetpos = checkplane->WorldPosition;

							if(tdist < (2.0f * NMTOWU))
							{
								targetpos.Y += 500 * FTTOWU;
							}

							if(AILOS(planepnt->WorldPosition, targetpos))
							{
								if(AICheckRadarCrossSig(planepnt, checkplane, dy * WUTOFT, tdist * WUTONM, radar, (planepnt->AI.iAIFlags2 & AILOCKEDUP)))
								{
									isseen = 1;
								}
							}
						}
					}
					else
					{
						isseen = 1;
					}

					if(isseen)
					{
						foundrange = tdist;
						foundplane = checkplane - Planes;
					}
				}
			}
		}
		checkplane ++;
	}
	if(foundplane == -1)
	{
		if(ftminrange == -1)
		{
			return(AICContinueBullsEyePicture(nextdelay, -2, planepnt, awacsnum, currentcnt, hasgroups, additional));
		}
		else
		{
			return(AICContinueBullsEyePicture(nextdelay, -1, planepnt, awacsnum, currentcnt, hasgroups, additional));
		}
	}
	else
	{
		numhide = AIC_AWACS_Hide_Near_Planes(&Planes[foundplane], awacsnum);
		Planes[foundplane].AI.iAIFlags1 |= AI_AWACS_IGNORE;
		Planes[foundplane].AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
		Planes[foundplane].AI.iAICombatFlags2 |= (AI_TEMP_IGNORE_CHECK);
//	  	AICAddSoundCall(AIC_AWACS_Picture_Msg, planepnt - Planes, 0, 50, foundplane);
		int planenum = planepnt - Planes;
		if(numhide)
		{
			planenum |= 0x4000;
			hasgroups = 1;
		}
		currentcnt ++;
		planenum |= currentcnt << 17;

		minrange = foundrange * WUTOFT;
//		AICAddSoundCall(AICContinuePicture, 0, 5000 + ((rand() & 3) * 1000), 50, (int)minrange);
		*nextdelay = *nextdelay + 5000 + ((rand() & 3) * 1000);
		returnval = AICContinuePictureNearPlane(nextdelay, (int)minrange, planepnt, awacsnum, currentcnt, hasgroups, additional);

		if((currentcnt == 1) && (returnval == 1))
		{
			foundplane |= 0x4000;
		}
		if(awacsnum == -2)
		{
			foundplane |= 0x8000;
		}
		else if(awacsnum < 0)
		{
			foundplane |= 0x2000;
		}
		else
		{
			foundplane |= awacsnum << 17;
		}

	  	AICAddSoundCall(AICBroadcastBanditCall, planenum, holddelay, 50, foundplane);
	}
	return(0);
}

//**************************************************************************************
int AICContinueBullsEyePicture(int *nextdelay, int ftminrange, PlaneParams *planepnt, int awacsnum, int currentcnt, int hasgroups, int additional)
{
	float minrange;
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	float maxrange;
	int numhide;
//	int planenum;
	int holddelay = *nextdelay;
	int returnval;
	int nextplane;
	int planenum = planepnt - Planes;
	float offangle, toffangle, toffpitch, offpitch;
	float radarangleyaw, radaranglepitch;
	int widthok, heightok;
	float widthangle, heightangle;
	int radarid;
	DBRadarType *radar;
	float ftempval;
	FPointDouble targetpos;
	int isseen;
	float jammingmod;
	float radarrange;


	if((!iUseBullseye) || ((BullsEye.x == 0) && (BullsEye.z == 0)))
	{
		if(ftminrange == -2)
		{
			AICAddSoundCall(AIC_AWACS_Pict_Clean, awacsnum, 5000, 50);
		}
		else if(currentcnt > 1)
		{
			AIC_TellNumInPicture(awacsnum, currentcnt, hasgroups);
		}
		return(0);
	}

	if(awacsnum == -1)
	{
		if(!planepnt->AI.AirTarget)
		{
			planepnt->AI.fRadarAngleYaw = AIConvertAngleTo180Degree(planepnt->Heading);
			planepnt->AI.fRadarAnglePitch = AIConvertAngleTo180Degree(planepnt->Pitch);
		}
		radarangleyaw = planepnt->AI.fRadarAngleYaw;
		radaranglepitch = planepnt->AI.fRadarAnglePitch;

		radarid = pDBAircraftList[planepnt->AI.iPlaneIndex].lRadarID;
		radar = GetRadarPtr(radarid);

		if(radar)
		{
			ftempval = radar->iSearchAzimuth;
			ftempval *= 1.0f - (planepnt->AI.iSkill * 0.1f);
			widthangle = ftempval;

			ftempval = radar->iSearchElevation;
			ftempval *= 1.0f - (planepnt->AI.iSkill * 0.1f);
			heightangle = ftempval;

			jammingmod = AIGetPlaneJammingMod(planepnt, radaranglepitch, radarangleyaw, widthangle, heightangle);

			radarrange = radar->iMaxRange * jammingmod * NMTOWU;
		}
		else
		{
			ftempval = 20.0f * (1.0f - (planepnt->AI.iSkill * 0.1f));
			widthangle = ftempval;
			ftempval = 20.0f * (1.0f - (planepnt->AI.iSkill * 0.1f));
			heightangle = ftempval;
			radarrange = 0;
		}
	}

	minrange = ftminrange * FTTOWU;
	foundrange = maxrange = (140 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)) && (checkplane->OnGround == 0))
		{
			if((checkplane->AI.iSide != planepnt->AI.iSide) && (!((checkplane->AI.iAIFlags1 & AI_AWACS_IGNORE) && (awacsnum != -1))) && (!(((checkplane->AI.iAICombatFlags2 & (AI_TEMP_IGNORE_CHECK)) || (checkplane->AI.iAIFlags2 & (AI_REPORTED_AS_CONTACT))) && (awacsnum == -1))))
			{
				dx = checkplane->WorldPosition.X - ConvertWayLoc(BullsEye.x);
				dz = checkplane->WorldPosition.Z - ConvertWayLoc(BullsEye.z);
				tdist = QuickDistance(dx, dz);
				if(tdist < minrange)
				{
					checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
					checkplane->AI.iAICombatFlags2 |= (AI_TEMP_IGNORE_CHECK);
				}
				else if(tdist < foundrange)
				{
					if(awacsnum == -1)
					{
						dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
						dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
						dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
						tdist = QuickDistance(dx, dz);
						isseen = 0;
						widthok = 0;
						heightok = 0;

						if(radarrange > tdist)
						{
							offangle = atan2(-dx, -dz) * 57.2958;
							toffangle = offangle - radarangleyaw;
							toffangle = AICapAngle(toffangle);
							if(fabs(toffangle) < widthangle)
							{
								widthok = 1;
							}
							offpitch = (atan2(dy, tdist) * 57.2958);
							toffpitch = offpitch - radaranglepitch;
							toffpitch = AICapAngle(toffpitch);
							if(fabs(toffpitch) < heightangle)  //  Plane in radar area
							{
								heightok = 1;
							}
						}

						if(widthok && heightok)
						{
							targetpos = checkplane->WorldPosition;

							if(tdist < (2.0f * NMTOWU))
							{
								targetpos.Y += 500 * FTTOWU;
							}

							if(AILOS(planepnt->WorldPosition, targetpos))
							{
								if(AICheckRadarCrossSig(planepnt, checkplane, dy * WUTOFT, tdist * WUTONM, radar, (planepnt->AI.iAIFlags2 & AILOCKEDUP)))
								{
									isseen = 1;
								}
							}
						}
					}
					else
					{
						isseen = 1;
					}

					if(isseen)
					{
						foundrange = tdist;
						foundplane = checkplane - Planes;
					}
				}
				else if(tdist > maxrange)
				{
					checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
					checkplane->AI.iAICombatFlags2 |= (AI_TEMP_IGNORE_CHECK);
				}
			}
		}
		checkplane ++;
	}
	if(foundplane == -1)
	{
		nextplane = -1;
		if(Planes[planenum].AI.wingman >= 0)
		{
			nextplane = Planes[planenum].AI.wingman;
		}
		else if(Planes[planenum].AI.winglead >= 0)
		{
			nextplane = Planes[planenum].AI.wingman;
			if(Planes[nextplane].AI.nextpair >= 0)
			{
				nextplane = Planes[nextplane].AI.nextpair;
			}
			else
			{
				nextplane = -1;
			}
		}

		if(ftminrange == -2)
		{
			if(awacsnum != -1)
			{
				AICAddSoundCall(AIC_AWACS_Pict_Clean, awacsnum, 5000, 50);
			}
			else
			{
				if(Planes[planenum].AI.AirTarget)
				{
					AICAddSoundCall(AICBasicAck, planenum, 5000, 50);
				}
				else
				{
					AICCleanCall(planenum);
				}
				if(nextplane >= 1)
				{
					AICAddSoundCall(AICContinueWingPicture, nextplane, *nextdelay, 50, 5000);
				}
			}
		}
		else
		{
			if(currentcnt > 1)
			{
				if(awacsnum != -1)
				{
					AIC_TellNumInPicture(awacsnum, currentcnt, hasgroups);
				}
				else
				{
					AIC_TellNumInPicture(planenum, currentcnt, hasgroups);
					if(nextplane >= 1)
					{
						AICAddSoundCall(AICContinueWingPicture, nextplane, *nextdelay, 50, 5000);
					}
				}
			}
			else if(currentcnt == 1)
			{
				if(nextplane >= 1)
				{
					AICAddSoundCall(AICContinueWingPicture, nextplane, *nextdelay, 50, 5000);
				}
				return(1);
			}
		}
	}
	else
	{
		numhide = AIC_AWACS_Hide_Near_Planes(&Planes[foundplane], awacsnum);
		Planes[foundplane].AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
		Planes[foundplane].AI.iAIFlags1 |= AI_AWACS_IGNORE;
//	  	AICAddSoundCall(AIC_AWACS_Picture_Msg, planepnt - Planes, 5000 + ((rand() & 3) * 1000), 50, foundplane);
		int planenum = planepnt - Planes;
		if(numhide)
		{
			planenum |= 0x4000;
			hasgroups = 1;
		}
		currentcnt ++;
		planenum |= currentcnt << 17;

		minrange = foundrange * WUTOFT;
//		AICAddSoundCall(AICContinueBullsEyePicture, 0, 5000 + ((rand() & 3) * 1000), 50, (int)minrange);
		*nextdelay = *nextdelay + 5000 + ((rand() & 3) * 1000);
		returnval = AICContinueBullsEyePicture(nextdelay, (int)minrange, planepnt, awacsnum, currentcnt, hasgroups, additional);

		if((currentcnt == 1) && (returnval == 1))
		{
			foundplane |= 0x4000;
		}
		if(awacsnum == -2)
		{
			foundplane |= 0x8000;
		}
		else if(awacsnum < 0)
		{
			foundplane |= 0x2000;
		}
		else
		{
			foundplane |= awacsnum << 17;
		}

	  	AICAddSoundCall(AICBroadcastBanditCall, planenum, holddelay, 50, foundplane);
	}
	return(0);
}

//**************************************************************************************
int AIC_AWACS_Hide_Near_Planes(PlaneParams *planepnt, int awacsnum)
{
	float nearrange = (10 * NMTOWU);
	float dx, dz;
	PlaneParams *checkplane;
	int cnt = 0;

	checkplane = &Planes[0];
	while (checkplane <= LastPlane)
	{
		if((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))))
		{
			if((checkplane->AI.iSide == planepnt->AI.iSide) && (checkplane != planepnt) && (!((checkplane->AI.iAIFlags1 & AI_AWACS_IGNORE) && (awacsnum != -1))) && (!(((checkplane->AI.iAICombatFlags2 & (AI_TEMP_IGNORE_CHECK)) || (checkplane->AI.iAIFlags2 & (AI_REPORTED_AS_CONTACT))) && (awacsnum == -1))))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				if((fabs(dx) < nearrange) && (fabs(dz) < nearrange))
				{
					checkplane->AI.iAIFlags1 |= AI_AWACS_IGNORE;
					checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
					checkplane->AI.iAICombatFlags2 |= (AI_TEMP_IGNORE_CHECK);
					cnt ++;
				}
			}
		}
		checkplane ++;
	}
	return(cnt);
}

//**************************************************************************************
void AICRequestPlayerBogieDope()
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dy, dz, tdist;
	int foundplane = -1;
	int awacsnum;
	int status;

	awacsnum = AIGetClosestAWACS(PlayerPlane);

	if(awacsnum < 0)
	{
		awacsnum = -2;
  		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			status = MovingVehicles[PlayerPlane->AI.iHomeBaseId].Status;
			if((!status) || (status & VL_SINKING))
			{
				return;
			}
		}
		else
		{
			return;
		}
	}


	foundrange = (140 * NMTOWU);

	while (checkplane <= LastPlane)
	{
		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))) && (!(pDBAircraftList[checkplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_CIVILIAN)) && (checkplane->OnGround == 0))
		{
			if(checkplane->AI.iSide != PlayerPlane->AI.iSide)
			{
				dx = checkplane->WorldPosition.X - PlayerPlane->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - PlayerPlane->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - PlayerPlane->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < foundrange)
				{
					foundrange = tdist;
					foundplane = checkplane - Planes;
				}
			}
		}
		checkplane ++;
	}
	if(foundplane == -1)
	{
		AICAddSoundCall(AIC_AWACS_Clean, PlayerPlane - Planes, 2000, 50, awacsnum);
	}
	else
	{
	  	AICAddSoundCall(AIC_AWACS_Bogey_Dope_Msg, PlayerPlane - Planes, 2000 + ((rand() & 3) * 1000), 50, foundplane);
	}
}

//**************************************************************************************
void AIC_AWACS_Unchanged(int planenum, int targetnum)
{
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	int voice = 0;
	int awacsnum = planenum;

	if(awacsnum < 0)
	{
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}

	tempnum = rand() & 1;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(tempnum)
	{
		msgnum = AIC_AWACS_PICT_UNCHANGED;
		msgsnd = AICF_AWACS_PICT_UNCHANGED;
	}
	else
	{
		msgnum = AIC_AWACS_PICT_SAME;
		msgsnd = AICF_AWACS_PICT_SAME;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AIC_AWACS_Clean(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_CLEAN, g_iLanguageId, callsign, awacscallsign))
		{
			return;
		}
	}
	else
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_CLEAN, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_AWACS_Pict_Clean(int planenum, int targetnum)
{
	char tempstr[1024];
	int voice = 0;

	if(planenum < 0)
	{
		voice = SPCH_STRIKE;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_PICT_CLEAN, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_PICT_CLEAN, planenum, 0, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AIC_AWACS_SAR_OK(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
//	char awacscallsign[256];
	int voice = 0;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, callsign);

	if(awacsnum < 0)
		voice = SPCH_STRIKE;


//	AICGetCallSign(awacsnum, awacscallsign);
  	if(!LANGGetTransMessage(tempstr, 1024, AIC_AWACS_SAR_ACK_1, g_iLanguageId, callsign))
  	{
  		return;
  	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_AWACS_SAR_ACK, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
void AICBombResultSpch(WeaponParams *W, int hit, int secondaries, int grndside, int directhit)
{
	int morebombs;
	int weaponindex;
	int weapontype;

	if ((int)W->Type < 0x200)
		weaponindex = AIGetWeaponIndex((int)W->Type);
	else
		weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);

	weapontype = pDBWeaponList[weaponindex].iWeaponType;

	if(W->Launcher == NULL)
	{
		return;
	}

	if(!((weapontype == WEAPON_TYPE_DUMB_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_AG_MISSILE) || (weapontype == WEAPON_TYPE_CLUSTER_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_AG_MISSILE) || (weapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE)))
	{
		if((weapontype != WEAPON_TYPE_GUN) || (W->P != PlayerPlane) || (!hit))
			return;
	}

	if(W->P)
	{
//		if(W->P->AI.iAIFlags1 & AIPLAYERGROUP)
		if(W->P != PlayerPlane)
		{
			morebombs = AICheckForMoreAG(W->P, W);

			if((hit) && (!(W->P->AI.iAIFlags1 & AI_BOMBS_HIT)))
			{
				AICWingFinishCASAttackMsg(W->P - Planes);
				W->P->AI.iAIFlags1 |= AI_BOMBS_HIT;
			}

			if(!morebombs)
			{
				if(!(W->P->AI.iAIFlags1 & AI_BOMBS_HIT))
				{
//					AIC_WSO_Bomb_Miss(PlayerPlane - Planes, (weapontype == WEAPON_TYPE_AG_MISSILE));
					AICWingFinishCASAttackMsg(W->P - Planes, 100);
				}
				W->P->AI.iAIFlags1 &= ~AI_BOMBS_HIT;
			}
		}
	}
	else
	{
		return;
	}

	if(W->P != PlayerPlane)
	{
		return;
	}

	morebombs = AICheckForMoreAG(PlayerPlane, W);

	if((hit) && (!(iPlayerBombFlags & WSO_BOMB_HIT)))
	{
		if(grndside != PlayerPlane->AI.iSide)
		{
			if(lAdvisorFlags & ADV_CASUAL_BETTY)
			{
				AICGenericBettySpeech(BETTY_TARGET_DESTROYED);
			}
//			AIC_WSO_Bomb_Hit(PlayerPlane - Planes, (weapontype != WEAPON_TYPE_GUN));
//			AICAddSoundCall(AIC_WSO_Bomb_Hit, PlayerPlane - Planes, 1000 + (rand() & 0x7FF), 50, (weapontype != WEAPON_TYPE_GUN));
		}
		else
		{
#ifdef NOT_E3  //  REMOVED FOR E3
			AIC_WSO_Wise_Ass_Msg(PlayerPlane - Planes, -1);
#endif

			iPlayerBombFlags |= WSO_SECONDAIRES;
		}

		iPlayerBombFlags |= WSO_BOMB_HIT;
	}

	if(hit)
	{
		if((lBombFlags & WSO_FAC_CHECK) && (iFACHitFlags & FAC_HIT_DESTROYED))
		{
			AIC_FACAck(PlayerPlane - Planes, 17);
		}
	}

	if(directhit)
	{
		iPlayerBombFlags |= WSO_NO_BOMB_MISS;
	}

	if((secondaries) && (!(iPlayerBombFlags & WSO_SECONDAIRES)))
	{
		if(grndside != PlayerPlane->AI.iSide)
		{
		  	AICAddSoundCall(AIC_WSO_Secondaires, PlayerPlane - Planes, 5000 + ((rand() & 3) * 1000), 50);
			iPlayerBombFlags |= WSO_SECONDAIRES;
		}
	}

	if(!morebombs)
	{
		if(!((iPlayerBombFlags & (WSO_BOMB_HIT|WSO_NO_BOMB_MISS))))
		{
//			AIC_WSO_Bomb_Miss(PlayerPlane - Planes, (weapontype == WEAPON_TYPE_AG_MISSILE));
//			AICAddSoundCall(AIC_WSO_Bomb_Miss, PlayerPlane - Planes, 1000 + (rand() & 0x7FF), 50, (weapontype == WEAPON_TYPE_AG_MISSILE));
			if(lAdvisorFlags & ADV_CASUAL_BETTY)
			{
				AICGenericBettySpeech(BETTY_TARGET_INTACT);
			}
		}
		else if((iPlayerBombFlags & (WSO_BOMB_HIT|WSO_NO_BOMB_MISS)) == WSO_NO_BOMB_MISS)
		{
			if(lAdvisorFlags & ADV_CASUAL_BETTY)
			{
				AICGenericBettySpeech(BETTY_TARGET_DAMAGED);
			}
		}
		if(lBombFlags & WSO_FAC_CHECK)
		{
			if(iFACHitFlags & FAC_HIT_DESTROYED)
			{
				iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT|FAC_HIT_DAMAGE|FAC_HIT_DESTROYED|FAC_HIT_FRIEND|FAC_LASER_ON);
			}
			else if(iFACHitFlags & FAC_HIT_FRIEND)
			{
				AIC_FAC_NearMiss_Msg(PlayerPlane - Planes);
				iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT|FAC_HIT_DAMAGE|FAC_HIT_DESTROYED|FAC_HIT_FRIEND|FAC_LASER_ON);
				dFACRange = 0;
			}
			else if(iFACHitFlags & FAC_HIT_DAMAGE)
			{
				AIC_FACAck(PlayerPlane - Planes, 16 + ((rand() & 1) * 2));
				iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT|FAC_HIT_DAMAGE|FAC_HIT_DESTROYED|FAC_HIT_FRIEND|FAC_LASER_ON);
				dFACRange = 0;
			}
			else if(iFACHitFlags & (FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT))
			{
			  	AICAddSoundCall(AIC_FAC_Miss, PlayerPlane - Planes, 1000 + ((rand() & 3) * 1000), 50);
			}
			else
			{
				iFACHitFlags &= ~(FAC_HIT_SHORT|FAC_HIT_LONG|FAC_HIT_LEFT|FAC_HIT_RIGHT|FAC_HIT_DAMAGE|FAC_HIT_DESTROYED|FAC_HIT_FRIEND|FAC_LASER_ON);
				dFACRange = 0;
			}
		}

		iPlayerBombFlags = 0;
	}
}

//**************************************************************************************
void AIC_WSO_Bomb_Hit(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd, tempnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!targetnum)
	{
		tempnum = 2;
	}
	else
	{
		tempnum = rand() & 3;
	}

	switch(tempnum)
	{
		case 1:
			msgnum = AIC_WSO_IMPACT;
			msgsnd = AICF_WSO_IMPACT;
			break;
		case 2:
			msgnum = AIC_WSO_GOOD_HIT;
			msgsnd = AICF_WSO_GOOD_HIT;
			break;
		case 3:
			msgnum = AIC_WSO_SHACK;
			msgsnd = AICF_WSO_SHACK;
			break;
		default:
			msgnum = AIC_WSO_BOOM;
			msgsnd = AICF_WSO_BOOM;
			break;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Secondaires(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd, tempnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tempnum = rand() % 6;

	switch(tempnum)
	{
		case 1:
			msgnum = AIC_WSO_LOOK_SECONDARIES;
			msgsnd = AICF_WSO_LOOK_SECONDARIES;
			break;
		case 2:
			msgnum = AIC_WSO_GOT_SECONDARIES;
			msgsnd = AICF_WSO_GOT_SECONDARIES;
			break;
		case 3:
			msgnum = AIC_WSO_BURN;
			msgsnd = AICF_WSO_BURN;
			break;
		case 4:
			msgnum = AIC_WSO_BOOM_BOOM;
			msgsnd = AICF_WSO_BOOM_BOOM;
			break;
		case 5:
			msgnum = AIC_WSO_DESTROYED_DALLAS;
			msgsnd = AICF_WSO_DESTROYED_DALLAS;
			break;
		default:
			msgnum = AIC_WSO_WOW_LOOK;
			msgsnd = AICF_WSO_WOW_LOOK;
			break;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Bomb_Miss(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd, tempnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if((Planes[planenum].AI.AirThreat) || (Planes[planenum].AI.iMissileThreat >= 0))
	{
		msgnum = AIC_WSO_MISSED_DAMN;
		msgsnd = AICF_WSO_MISSED_DAMN;
	}
	else
	{
		if(!targetnum)
		{
			tempnum = rand() % 3;
		}
		else
		{
			tempnum = rand() & 1;
		}

		switch(tempnum)
		{
			case 1:
				msgnum = AIC_WSO_MISSED_DAMN;
				msgsnd = AICF_WSO_MISSED_DAMN;
				break;
			case 2:
				msgnum = AIC_WSO_LOOKS_MISSED;
				msgsnd = AICF_WSO_LOOKS_MISSED;
				break;
			default:
				msgnum = AIC_WSO_MISSED_SUCKS;
				msgsnd = AICF_WSO_MISSED_SUCKS;
				break;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_AWACS_Acknowledged(int planenum, int awacsnum)
{
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int tempnum, msgnum, msgsnd;
	int voice = 0;

	if(awacsnum < 0)
	{
		voice = SPCH_STRIKE;
		awacsnum = 1;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	tempnum = rand() & 1;

	if(tempnum)
	{
		msgnum = AIC_AWACS_ACKNOWLEDGED_1;
		msgsnd = AICF_AWACS_ACKNOWLEDGED;
	}
	else
	{
		msgnum = AIC_AWACS_COPY_1;
		msgsnd = AICF_AWACS_COPY;
	}

	if(awacsnum >= 0)
	{
		AICGetCallSign(awacsnum, awacscallsign);
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(awacscallsign, 256, AIC_STRIKE, g_iLanguageId))
		{
			return;
		}
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
		{
			return;
		}
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, awacsnum, planenum, 0, 0, 0, 0, 0, 0, 0, voice);
	return;
}

//**************************************************************************************
int AICheckForMoreAG(PlaneParams *planepnt, WeaponParams *ingnore)
{
	int weapontype;
	int weaponindex;
	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
		if ((W->Flags & WEAPON_INUSE) && (W != ingnore))
		{
			if(W->P == planepnt)
			{
				weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
				weapontype = pDBWeaponList[weaponindex].iWeaponType;
				if((weapontype == WEAPON_TYPE_DUMB_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_AG_MISSILE) || (weapontype == WEAPON_TYPE_CLUSTER_BOMB) || (weapontype == WEAPON_TYPE_GUIDED_BOMB) || (weapontype == WEAPON_TYPE_AG_MISSILE) || (weapontype == WEAPON_TYPE_AGROCKET) || (weapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE))
				{
					return(1);
				}
			}
		}
		W++;
	}
	return(0);
}

//**************************************************************************************
int AICheckForMoreBullets(PlaneParams *planepnt, WeaponParams *ingnore)
{
	int weapontype;
	int weaponindex;
	WeaponParams *W = &Weapons[0];

	while(W <= LastWeapon)
	{
		if ((W->Flags & WEAPON_INUSE) && (W != ingnore))
		{
			if(W->P == planepnt)
			{
				weaponindex = AIGetWeaponIndex(W->Type->TypeNumber);
				weapontype = pDBWeaponList[weaponindex].iWeaponType;
				if((weapontype == WEAPON_TYPE_GUN))
				{
					return(1);
				}
			}
		}
		W++;
	}
	return(0);
}

//**************************************************************************************
void AIC_FormOn_Human_Msg(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int msgnum, msgsnd;  //  , tempnum;
	long voice;
	int placeingroup;

	lNoRepeatFlags[planenum] |= FORMING_UP_SAID;
	if(lNoRepeatTimer[planenum] < 60000)
	{
		lNoRepeatTimer[planenum] = 60000;
	}

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	voice = Planes[planenum].AI.lAIVoice;

	if(Planes[planenum].AI.iAIFlags1 & AI_SEEN_PLAYER)
	{
		msgnum = AIC_CAP_FORM_PLAYER;
		msgsnd = AICF_CAP_FORM_PLAYER;
#if 0
		tempnum = rand()&1;
		if(tempnum)
		{
			msgnum = AIC_BOMBER_REFORMING;
			msgsnd = AICF_BOMBER_REFORMING;
		}
		else
		{
			msgnum = AIC_BOMBER_REFORMING_YOU;
			msgsnd = AICF_BOMBER_REFORMING_YOU;
		}
#endif
	}
	else
	{
		AIUpdateGroupFlags(planenum, AI_SEEN_PLAYER, 0);
		msgnum = AIC_CAP_FORM_PLAYER;
		msgsnd = AICF_CAP_FORM_PLAYER;
#if 0
		tempnum = rand()&1;
		if(tempnum)
		{
			if(voice == SPCH_US_BOMB1)
			{
				msgnum = AIC_BOMBER_FORM_UP;
			}
			else
			{
				msgnum = AIC_UK_BOMBER_FORM_UP;
			}
			msgsnd = AICF_BOMBER_FORM_UP;
		}
		else
		{
			if(voice == SPCH_US_BOMB1)
			{
				msgnum = AIC_BOMBER_TALLY_FORM;
			}
			else
			{
				msgnum = AIC_UK_BOMBER_TALLY_FORM;
			}
			msgsnd = AICF_BOMBER_TALLY_FORM;
		}
#endif
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AILookPlayerEscort(PlaneParams *planepnt)
{
	float dx, dz, tdist;

	dx = PlayerPlane->WorldPosition.X - planepnt->WorldPosition.X;
	dz = PlayerPlane->WorldPosition.Z - planepnt->WorldPosition.Z;

	tdist = QuickDistance(dx,dz);

	if((tdist * WUTOFT) < (AI_VISUAL_RANGE_NM * NMTOFT))
	{
		AIC_Human_Beginning_Escort_Msg(PlayerPlane - Planes, planepnt - Planes);
		AICAddSoundCall(AIC_Spot_Human_Escort_Msg, planepnt - Planes, 5000, 50, PlayerPlane - Planes);
	}
}

//**************************************************************************************
void AIC_Spot_Human_Escort_Msg(int planenum, int targetnum)
{
	AIUpdateGroupFlags(planenum, AI_SEEN_PLAYER, 0);
	return;

	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int msgnum, msgsnd;
	long voice;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	voice = Planes[planenum].AI.lAIVoice;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, voice, placeingroup))
	{
		return;
	}

	if(voice == SPCH_US_BOMB1)
	{
		msgnum = AIC_BOMBER_VISUAL_GLAD;
	}
	else if(voice == SPCH_UK_BOMB1)
	{
		msgnum = AIC_UK_BOMBER_VISUAL_GLAD;
	}
	else
	{
		return;
	}
	msgsnd = AICF_BOMBER_VISUAL_GLAD;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Release_Human_Escort_Msg(int planenum, int targetnum)
{
	AIUpdateGroupFlags(planenum, AI_RELEASED_PLAYER, 0);
	AICCAPDone(planenum, targetnum);
	return;

	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int msgnum, msgsnd, tempnum;
	long voice;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	voice = Planes[planenum].AI.lAIVoice;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, voice, placeingroup))
	{
		return;
	}

	AIUpdateGroupFlags(planenum, AI_RELEASED_PLAYER, 0);

	tempnum = rand()&1;
	if(tempnum)
	{
		if(voice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_THANKS;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_THANKS;
		}
		msgsnd = AICF_BOMBER_THANKS;
	}
	else
	{
		if(voice == SPCH_US_BOMB1)
		{
			msgnum = AIC_BOMBER_RTB;
		}
		else
		{
			msgnum = AIC_UK_BOMBER_RTB;
		}
		msgsnd = AICF_BOMBER_RTB;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Bombers_RTB_Msg(int planenum, int targetnum)
{
	AICCAPDone(planenum, targetnum);
	return;

	char tempstr[1024];
	char callsign[256];
	char playercallsign[256];
	int msgnum, msgsnd;
	long voice;
	int placeingroup;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.
	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	AIC_Get_Callsign_With_Number(targetnum, playercallsign);

	voice = Planes[planenum].AI.lAIVoice;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, voice, placeingroup))
	{
		return;
	}

	AIUpdateGroupFlags(planenum, AI_RELEASED_PLAYER, 0);

	if(voice == SPCH_US_BOMB1)
	{
		msgnum = AIC_BOMBER_RTB;
	}
	else
	{
		msgnum = AIC_UK_BOMBER_RTB;
	}
	msgsnd = AICF_BOMBER_RTB;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, playercallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICMagnumLaunchMsg(PlaneParams *planepnt)
{
	char tempstr[1024];
	int placeingroup;
//	char tempstr2[256];
	char callsign[256];
	int planenum = planepnt - Planes;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!AICCheckVoiceNumInGroup(planepnt - Planes, planepnt->AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WING_MAGNUM, g_iLanguageId, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 40);
	AIRGenericSpeech(AICF_WING_MAGNUM, planepnt - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICMusicOnOffMsg(int planenum, int musicon)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(musicon)
	{
		msgnum = AIC_WING_MUSIC_ON;
		msgsnd = AICF_SEAD_MUSIC_ON;
	}
	else
	{
		msgnum = AIC_WING_MUSIC_OFF;
		msgsnd = AICF_SEAD_MUSIC_OFF;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICSEADAttackMsg(int planenum, int attacktype)
{
	char tempstr[1024];
	int msgnum, msgsnd;
//	int awacsnum;
	char callsign[256];
//	char awacscallsign[256];
	int placeingroup;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	awacsnum = AIGetClosestAWACS(&Planes[planenum]);

//	if(awacsnum < 0)
//	{
//		return;
//	}

//	AICGetCallSign(awacsnum, awacscallsign);

	AIC_Get_Callsign_With_Number(planenum, callsign);

#if 0
	if(attacktype)
	{
		msgnum = AIC_SEAD_ATTACKING_SAM;
		msgsnd = AICF_SEAD_ATTACKING_SAM;
	}
	else
	{
		msgnum = AIC_SEAD_ATTACKING_AAA;
		msgsnd = AICF_SEAD_ATTACKING_AAA;
	}
#else
	if(attacktype == 1)
	{
		msgnum = AIC_WING_ATTACKING_DUG_GUNS;
		msgsnd = AICF_WING_ATTACKING_DUG_GUNS;
	}
	else if(attacktype == 2)
	{
		msgnum = AIC_WING_ATTACKING_DUG_SAM;
		msgsnd = AICF_WING_ATTACKING_DUG_SAM;
	}
	else if(attacktype == 3)
	{
		msgnum = AIC_WING_ATTACKING_AAA;
		msgsnd = AICF_WING_ATTACKING_AAA;
	}
	else if(attacktype == 4)
	{
		msgnum = AIC_WING_ATTACKING_MOBILE_SAM;
		msgsnd = AICF_WING_ATTACKING_MOBILE_SAM;
	}
	else if(attacktype == 5)
	{
		msgnum = AIC_WING_ATTACKING_SHIP;
		msgsnd = AICF_WING_ATTACKING_SHIP;
	}
	else if(attacktype == 8)
	{
		msgnum = AIC_WING_ATTACKING_CAN;
		msgsnd = AICF_WING_ATTACKING_CAN;
	}
	else
	{
		msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT;
		msgsnd = AICF_WING_TARGETS_SIGHTED_IN_HOT;
	}
#endif

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICSEADWinchesterMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	int placeingroup;
	int awacsnum;
	char awacscallsign[256];

	if(!(lNoRepeatFlags[planenum] & WINCHESTER_SAID))
	{
		lNoRepeatFlags[planenum] |= WINCHESTER_SAID;
		if(lNoRepeatTimer[planenum] < 60000)
		{
			lNoRepeatTimer[planenum] = 60000;
		}
		awacsnum = AIGetClosestAWACS(&Planes[planenum]);

		if(awacsnum < 0)
		{
			return;
		}

		AICGetCallSign(awacsnum, awacscallsign);

		//  Make sound call for basic ack.

	//  	AIRBasicAckSnd(placeingroup);

		//  Add message to message array.

		if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
		{
			return;
		}

		placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
		if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
		{
			return;
		}

		AIC_Get_Callsign_With_Number(planenum, callsign);

	//	msgnum = AIC_SEAD_WINCHESTER;
	//	msgsnd = AICF_SEAD_WINCHESTER;
		msgnum = AIC_WINCHESTER_2;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_WINCHESTER_RUSF_2;
		}

		msgsnd = AICF_CAP_WINCHESTER;


		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planenum, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
	}

	if(AIAllGroupAGWinchester(AIGetLeader(&Planes[planenum])))
	{
		AISendGroupHome(&Planes[planenum]);
	}
	return;
}

//**************************************************************************************
void AIC_WSO_Check_Msgs(int planenum, int checktype)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Flight_Callsign(planenum, callsign);

	if(checktype == 0)
	{
		msgnum = AIC_WSO_CHECK_CALL;
		msgsnd = AICF_WSO_CHECK_CALL;
	}
	else
	{
		msgnum = AIC_WSO_STORES_CHECK;
		msgsnd = AICF_WSO_STORES_CHECK;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_Human_Beginning_Escort_Msg(int planenum, int targetnum)
{
	AIC_Beginning_Human_Escort_Msg(planenum, targetnum);
	return;

	char tempstr[1024];
	char callsign[256];
	char escortcallsign[256];
	int msgnum, msgsnd, tempnum;
	long voice;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	AIC_Get_Callsign_With_Number(targetnum, escortcallsign);

	voice = Planes[planenum].AI.lAIVoice;

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planenum, voice, placeingroup))
	{
		return;
	}

	tempnum = rand() % 3;
	if(tempnum == 1)
	{
		msgnum = AIC_WSO_FORM_BOMBER;
		msgsnd = AICF_WSO_FORM_BOMBER;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_WSO_ON_WAY;
		msgsnd = AICF_WSO_ON_WAY;
	}
	else
	{
		msgnum = AIC_WSO_WITH_YOU;
		msgsnd = AICF_WSO_WITH_YOU;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, escortcallsign, callsign))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_Begin_Attack_Msgs(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Flight_Callsign(planenum, callsign);

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_WSO_CLEARED_HOT;
		msgsnd = AICF_WSO_CLEARED_HOT;
	}
	else  //   if(tempnum == 2)
	{
		msgnum = AIC_WSO_COMMENCE_ATTACK;
		msgsnd = AICF_WSO_COMMENCE_ATTACK;
	}
//	else
//	{
//		msgnum = AIC_WSO_SHOWTIME;
//		msgsnd = AICF_WSO_SHOWTIME;
//	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICPlayerJSTARSCheckIn()
{
	int jstarsnum;

	jstarsnum = AIGetClosestJSTARS(PlayerPlane);

	if(jstarsnum < 0)
		return;

	AICAddSoundCall(AIC_WSO_JSTARS_Check_In, PlayerPlane - Planes, 0, 50, jstarsnum);
	AICAddSoundCall(AIC_JSTARS_Check_Reply, jstarsnum, 4000, 50, PlayerPlane - Planes);
}

//**************************************************************************************
void AICPlayerJSTARSCheckOut()
{
	int jstarsnum;

	jstarsnum = AIGetClosestJSTARS(PlayerPlane);

	if(jstarsnum < 0)
		return;

	AICAddSoundCall(AIC_WSO_JSTARS_Check_Out, PlayerPlane - Planes, 0, 50, jstarsnum);
	AICAddSoundCall(AIC_JSTARS_Acknowledge, jstarsnum, 4000, 50, PlayerPlane - Planes);
}

//**************************************************************************************
void AIC_WSO_JSTARS_Check_In(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];
	char jstarcall[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	lBombFlags |= WSO_JSTARS_CHECK;

	AIC_Get_Flight_Callsign(planenum, callsign);
	AICGetCallSign(targetnum, jstarcall);

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_WSO_J_CHECK_IN;
		msgsnd = AICF_WSO_J_CHECK_IN;
	}
	else
	{
		msgnum = AIC_WSO_J_AVAILABLE;
		msgsnd = AICF_WSO_J_AVAILABLE;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, jstarcall, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_JSTARS_Check_Reply(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];
	char jstarcall[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, jstarcall);
	AIC_Get_Callsign_With_Number(targetnum, callsign);

	if(g_iNumJSTARObjects <= 0)
	{
		tempnum = rand() % 3;
	}
	else
	{
		tempnum = rand() & 3;
	}

	if(tempnum == 1)
	{
		msgnum = AIC_JSTARS_COPY_STANDBY;
		msgsnd = AICF_JSTARS_COPY_STANDBY;
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_JSTARS_COPY_CLEARED_PATROL;
		msgsnd = AICF_JSTARS_COPY_CLEARED_PATROL;
	}
	else if(tempnum == 3)
	{
		msgnum = AIC_JSTARS_POSSIBLE_TARG;
		msgsnd = AICF_JSTARS_POSSIBLE_TARG;
	}
	else
	{
		msgnum = AIC_JSTARS_COPY_GOOD_HUNT;
		msgsnd = AICF_JSTARS_COPY_GOOD_HUNT;
	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, jstarcall))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	AICAddSoundCall(AIC_JSTARS_Target_Loc, planenum, 4000 + ((rand() & 3) * 1000), 50, PlayerPlane - Planes);
	return;
}

//**************************************************************************************
void AIC_WSO_JSTARS_Check_Out(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];
	char jstarcall[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	lBombFlags &= ~WSO_JSTARS_CHECK;
	AIC_Get_Flight_Callsign(planenum, callsign);
	AICGetCallSign(targetnum, jstarcall);

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_WSO_J_HEADING_HOME;
		msgsnd = AICF_WSO_J_HEADING_HOME;
	}
	else
	{
		msgnum = AIC_WSO_J_RTB;
		msgsnd = AICF_WSO_J_RTB;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, jstarcall, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_WSO_JSTARS_Request_New(int planenum, int targetnum)
{
	char tempstr[1024];
	char callsign[256];
	char jstarcall[256];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Flight_Callsign(planenum, callsign);
	AICGetCallSign(targetnum, jstarcall);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_WSO_J_REQUEST_TARGET, g_iLanguageId, jstarcall, callsign))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(AICF_WSO_J_REQUEST_TARGET, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_JSTARS_Acknowledge(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];
	char jstarcall[256];

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, jstarcall);
	AIC_Get_Callsign_With_Number(targetnum, callsign);

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_AWACS_ACKNOWLEDGED;
		msgsnd = AICF_JSTARS_ACKNOWLEDGED;
	}
	else
	{
		msgnum = AIC_AWACS_COPY;
		msgsnd = AICF_JSTARS_COPY;
	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, jstarcall))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, targetnum, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AIC_JSTARS_Give_Target_Loc()
{
	int jstarnum;

	jstarnum = AIGetClosestJSTARS(PlayerPlane);

	if(jstarnum < 0)
		return;

	AICAddSoundCall(AIC_JSTARS_Target_Loc, jstarnum, ((rand() & 3) * 1000), 50, PlayerPlane - Planes);
}

//**************************************************************************************
void AIC_JSTARS_Give_Next_Target_Loc()
{
	int jstarnum;

	jstarnum = AIGetClosestJSTARS(PlayerPlane);

	if(jstarnum < 0)
		return;

	if(AIC_JSTARS_GetNextTarget() == -1)
	{
		AIC_JSTARS_No_Targets(jstarnum, PlayerPlane - Planes);
		return;
	}

	AIC_WSO_JSTARS_Request_New(PlayerPlane - Planes, jstarnum);
	AICAddSoundCall(AIC_JSTARS_Target_Loc, jstarnum, 4000 + ((rand() & 3) * 1000), 50, PlayerPlane - Planes);
}

//**************************************************************************************
int AIC_JSTARS_GetNextTarget()
{
	int cnt;
	int done = 0;
	int numdead, numchecked;
	int numlook, randval, passone;
	MovingVehicleParams *vehiclepnt;
	long ignorethispass = 0;
	int numignored;
	int invisible;

	if(JSTARList.iNumJSTARObjects)
	{
		if(!JSTARList.iMode)
		{
			lJSTARSFlags |= 1<<iJSTARSTarget;

			numignored = numdead = numchecked = 0;

			for(cnt = 0; cnt < JSTARList.iNumJSTARObjects; cnt ++)
			{
				if(lJSTARSDead & (1<<cnt))
				{
					numdead ++;
				}
				else if(lJSTARSFlags & (1<<cnt))
				{
					numchecked ++;
				}
				if((JSTARList.JSTARObjectList[cnt].iObjectType == MOVINGVEHICLE) && (!(lJSTARSDead & (1<<cnt))))
				{
					invisible = 0;
					for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
					{
						if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))))
						{
							if((DWORD)vehiclepnt->iVListID == JSTARList.JSTARObjectList[cnt].dwSerialNumber)
							{
								if(vehiclepnt->Status & VL_INVISIBLE)
								{
									invisible = 1;
								}
								else
								{
									break;
								}
							}
						}
					}
					if(vehiclepnt > LastMovingVehicle)
					{
						if(invisible)
						{
							ignorethispass |= (1<<cnt);
							if(!(lJSTARSFlags & (1<<cnt)))
							{
								numignored ++;
							}
						}
						else
						{
							lJSTARSDead |= (1<<cnt);
							numdead ++;
						}
					}
				}
			}
			if(numdead >= JSTARList.iNumJSTARObjects)
			{
				return(-1);
			}

			if((numdead + numchecked + numignored) >= JSTARList.iNumJSTARObjects)
			{
				if((JSTARList.iNumJSTARObjects - numdead) > 1)
				{
					numchecked = 1;
					lJSTARSFlags = 1<<iJSTARSTarget;
				}
				else
				{
					numchecked = 0;
					lJSTARSFlags = 0;
				}
			}

			numlook = JSTARList.iNumJSTARObjects - (numdead + numchecked + numignored);

			randval = (rand() % numlook) + 1;

			passone = 0;
			while(randval)
			{
				iJSTARSTarget ++;
				if(iJSTARSTarget >= JSTARList.iNumJSTARObjects)
				{
					iJSTARSTarget = 0;
					passone ++;
					if(passone == 4)
					{
						return(-1);
					}
				}
				if(!((lJSTARSDead & (1<<iJSTARSTarget)) || (ignorethispass & (1<<iJSTARSTarget)) || (lJSTARSFlags & (1<<iJSTARSTarget))))
				{
					randval --;
					if(passone == 2)
					{
						randval = 0;
					}
				}
			}
		}
		else
		{
			iJSTARSTarget ++;
			while((done == 0) && (iJSTARSTarget < JSTARList.iNumJSTARObjects))
			{
				if(!((lJSTARSDead & (1<<iJSTARSTarget)) || (ignorethispass & (1<<iJSTARSTarget))))
				{
					done = 1;
				}
			}

			if(!done)
			{
				iJSTARSTarget = 0;

				while((done == 0) && (iJSTARSTarget < JSTARList.iNumJSTARObjects))
				{
					if(!((lJSTARSDead & (1<<iJSTARSTarget)) || (ignorethispass & (1<<iJSTARSTarget))))
					{
						done = 1;
					}
					else
					{
						iJSTARSTarget ++;
					}
				}
			}

			if(!done)
			{
				return(-1);
			}
		}
	}

	return(0);
}

//**************************************************************************************
void AIC_JSTARS_Target_Loc(int jstarnum, int playernum)
{
	char callsign[128];
	char jcallsign[128];
	char tempstr[1024];
	char bearstr[128];
	char rangestr[128];
	char headingstr[256];
	int bearing360;
	float rangenm;
	float tbearing;
	int planenum;
	int dobullseye = 0;
	int headingval;
	float dx, dy, dz, range;
	FPoint targetloc;
	int isvehicle = 0;
	int tempnum, msgsnd;
	float bearing;
	MovingVehicleParams *vehiclepnt;
	MovingVehicleParams *foundvehicle = NULL;
	int invisible = 0;

//	if(g_iNumJSTARObjects <= 0)

	planenum = playernum;

	if(Planes[playernum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(JSTARList.iNumJSTARObjects <= 0)
	{
		AIC_JSTARS_No_Targets(jstarnum, playernum);
		return;
	}

	if(lJSTARSDead & (1<<iJSTARSTarget))
	{
		AIC_JSTARS_Give_Next_Target_Loc();
	}

	if(JSTARList.JSTARObjectList[iJSTARSTarget].iObjectType == MOVINGVEHICLE)
	{
		for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
		{
			if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED))))
			{
				if((DWORD)vehiclepnt->iVListID == JSTARList.JSTARObjectList[iJSTARSTarget].dwSerialNumber)
				{
					if(vehiclepnt->Status & VL_INVISIBLE)
					{
						invisible = 1;
					}
					else
					{
						foundvehicle = vehiclepnt;
						break;
					}
				}
			}
		}
		if(foundvehicle)
		{
			targetloc = foundvehicle->WorldPosition;
			if(foundvehicle->Status & VL_MOVING)
			{
				isvehicle = 1;
			}
		}
		else
		{
			if(!invisible)
			{
				lJSTARSDead |= (1<<iJSTARSTarget);
			}
			AIC_JSTARS_Give_Next_Target_Loc();
			return;
		}
	}
	else
	{
		targetloc.X = ConvertWayLoc(JSTARList.JSTARObjectList[iJSTARSTarget].lX);
		targetloc.Y = ConvertWayLoc(JSTARList.JSTARObjectList[iJSTARSTarget].lY);
		targetloc.Z = ConvertWayLoc(JSTARList.JSTARObjectList[iJSTARSTarget].lZ);
	}

	if(iUseBullseye && ((BullsEye.x != 0) || (BullsEye.z != 0)))
	{
		dobullseye = 1;
		bearing = AIComputeHeadingFromBullseye(targetloc, &range, &dx, &dz);

		rangenm = (range * WUTONM);
		if(rangenm > 140)
		{
			dobullseye = 0;
			bearing = AIComputeHeadingToPoint(&Planes[playernum], targetloc, &range, &dx ,&dy, &dz, 0);
		}
		else if(rangenm < 5)
		{
			rangenm = 5;
		}
	}
	else
	{
		bearing = AIComputeHeadingToPoint(&Planes[playernum], targetloc, &range, &dx ,&dy, &dz, 0);
	}

	headingval = AICGetTextHeadingGeneral(headingstr, bearing);

	rangenm = (range * WUTONM);

	AICGetCallSign(jstarnum, jcallsign);

	AIC_Get_Callsign_With_Number(planenum, callsign);

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	if(rangenm < 5)
	{
		rangenm = 5;
	}

	AICGetRangeText(rangestr, rangenm);

	if(dobullseye)
	{
		if(isvehicle)
		{
			tempnum = rand() & 1;
			if(tempnum)
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_JSTARS_MOVERS_AT, g_iLanguageId, callsign, jcallsign, headingstr, rangestr))
				{
					return;
				}
				msgsnd = AICF_JSTARS_MOVERS_AT;
			}
			else
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_JSTARS_VEHICLES_AT, g_iLanguageId, callsign, jcallsign, headingstr, rangestr))
				{
					return;
				}
				msgsnd = AICF_JSTARS_VEHICLES_AT;
			}
		}
		else
		{
			if(!LANGGetTransMessage(tempstr, 1024, AIC_JSTARS_GROUND_AT, g_iLanguageId, callsign, jcallsign, headingstr, rangestr))
			{
				return;
			}
			msgsnd = AICF_JSTARS_GROUND_AT;
		}
	}
	else
	{
		if(isvehicle)
		{
			tempnum = rand() & 1;
			if(tempnum)
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_JSTARS_MOVERS_BEARING, g_iLanguageId, callsign, jcallsign, bearstr, rangestr))
				{
					return;
				}
				msgsnd = AICF_JSTARS_MOVERS_BEARING;
			}
			else
			{
				if(!LANGGetTransMessage(tempstr, 1024, AIC_JSTARS_VEHCILES_BEARING, g_iLanguageId, callsign, jcallsign, bearstr, rangestr))
				{
					return;
				}
				msgsnd = AICF_JSTARS_VEHCILES_BEARING;
			}
		}
		else
		{
			if(!LANGGetTransMessage(tempstr, 1024, AIC_JSTARS_GROUND_BEARING, g_iLanguageId, callsign, jcallsign, bearstr, rangestr))
			{
				return;
			}
			msgsnd = AICF_JSTARS_GROUND_BEARING;
		}
	}

	AIRGenericSpeech(msgsnd, jstarnum, planenum, bearing, headingval, headingval, rangenm, 0, 0, 0, 0);

	AICAddAIRadioMsgs(tempstr, 40);
}

//**************************************************************************************
void AIC_JSTARS_No_Targets(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd, tempnum;
	char callsign[256];
	char jstarcall[256];
	int msgto;

	//  Make sound call for basic ack.

//  	AIRBasicAckSnd(placeingroup);

	//  Add message to message array.

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AICGetCallSign(planenum, jstarcall);
	if(targetnum != -1)
	{
		AIC_Get_Callsign_With_Number(targetnum, callsign);
		tempnum = rand() % 3;
		msgto = targetnum;
	}
	else
	{
		AIC_Get_Callsign_With_Number(PlayerPlane - Planes, callsign);
		tempnum = rand() & 1;
		if(tempnum == 1)
		{
			tempnum = 2;
		}
		msgto = PlayerPlane - Planes;
	}

	if(tempnum == 1)
	{
		msgnum = AIC_JSTARS_UNSURE;
		msgsnd = AICF_JSTARS_UNSURE;
		AICAddSoundCall(AIC_JSTARS_No_Targets, planenum, 2000 + ((rand() & 3) * 1000), 50, msgto);
	}
	else if(tempnum == 2)
	{
		msgnum = AIC_JSTARS_BROKEN;
		msgsnd = AICF_JSTARS_BROKEN;
	}
	else
	{
		msgnum = AIC_JSTARS_AREA_CLEAR;
		msgsnd = AICF_JSTARS_AREA_CLEAR;
	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, jstarcall))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, msgto, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICWingCASAttackMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	int tempnum, msgnum, msgsnd;
	BasicInstance *walker;
	MovingVehicleParams *vehiclepnt;
	int attackspch = 0;
	PlaneParams *planepnt = &Planes[planenum];
	DBWeaponType *weapon;
	int cnt;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

#if 0
	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
	{
		return;
	}
#endif

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}


	if(planepnt->AI.lGroundTargetFlag == GROUNDOBJECT)
	{
		walker = (BasicInstance *)planepnt->AI.pGroundTarget;
		if(walker->Family == FAMILY_AAWEAPON)
		{
			weapon = GetWeaponPtr(walker);
			if(weapon)
			{
				if(weapon->iWeaponType == WEAPON_TYPE_GUN)
				{
					if(weapon->iSeekerType == 10)
					{
						attackspch = 1;
					}
					else
					{
						attackspch = 3;
					}
				}
				else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
				{
					attackspch = 2;
				}
			}
		}
	}
	else if(planepnt->AI.lGroundTargetFlag == MOVINGVEHICLE)
	{
		vehiclepnt = (MovingVehicleParams *)planepnt->AI.pGroundTarget;
		if(vehiclepnt->iShipType)
		{
			if(pDBShipList[vehiclepnt->iVDBIndex].lSizeType & (SHIP_SIZE_VSMALL|SHIP_SIZE_SMALL))
			{
				attackspch = 8;
			}
			else
			{
				attackspch = 5;
			}
		}
		else
		{
			if(pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleType & VEHICLE_TYPE_TANK)
			{
				attackspch = 6;
			}
			else
			{
				for(cnt = 0; cnt < 2; cnt ++)
				{
					if(!cnt)
					{
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
					}
					else
					{
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
					}

					if(!weapon)
					{
						attackspch = 7;
					}
					else if((weapon->iWeaponType == WEAPON_TYPE_GUN) && (attackspch < 3))
					{
						attackspch = 3;
					}
					else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
					{
						attackspch = 4;
					}
				}
			}
			if((!attackspch) && (pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleType & VEHICLE_TYPE_OTHER))
			{
				attackspch = 7;
			}
		}
	}

	if(attackspch == 1)
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			msgnum = AIC_WING_ATTACKING_DUG_GUNS_RAF;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
		{
			msgnum = AIC_WING_ATTACKING_DUG_GUNS_RUSA;
		}
		else
		{
			msgnum = AIC_WING_ATTACKING_DUG_GUNS;
		}

		msgsnd = AICF_WING_ATTACKING_DUG_GUNS;
	}
	else if(attackspch == 2)
	{
		msgnum = AIC_WING_ATTACKING_DUG_SAM;
		msgsnd = AICF_WING_ATTACKING_DUG_SAM;
	}
	else if(attackspch == 3)
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			msgnum = AIC_WING_ATTACKING_AAA_RAF;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
		{
			msgnum = AIC_WING_ATTACKING_AAA_RUSA;
		}
		else
		{
			msgnum = AIC_WING_ATTACKING_AAA;
		}
		msgsnd = AICF_WING_ATTACKING_AAA;
	}
	else if(attackspch == 4)
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			msgnum = AIC_WING_ATTACKING_MOBILE_SAM_RAF;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
		{
			msgnum = AIC_WING_ATTACKING_MOBILE_SAM_RUSA;
		}
		else
		{
			msgnum = AIC_WING_ATTACKING_MOBILE_SAM;
		}
		msgsnd = AICF_WING_ATTACKING_MOBILE_SAM;
	}
	else if(attackspch == 5)
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			msgnum = AIC_WING_ATTACKING_SHIP_RAF;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
		{
			msgnum = AIC_WING_ATTACKING_SHIP_RUSA;
		}
		else
		{
			msgnum = AIC_WING_ATTACKING_SHIP;
		}
		msgsnd = AICF_WING_ATTACKING_SHIP;
	}
	else if(attackspch == 6)
	{
		msgnum = AIC_WING_ATTACKING_ARMOR;
		msgsnd = AICF_WING_ATTACKING_ARMOR;
	}
	else if(attackspch == 7)
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			msgnum = AIC_WING_ATTACKING_TRUCKS_RAF;
		}
		else
		{
			msgnum = AIC_WING_ATTACKING_TRUCKS;
		}
		msgsnd = AICF_WING_ATTACKING_TRUCKS;
	}
	else if(attackspch == 8)
	{
		if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
		{
			msgnum = AIC_WING_ATTACKING_CAN_RAF;
		}
		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
		{
			msgnum = AIC_WING_ATTACKING_CAN_RAF;
		}
		else
		{
			msgnum = AIC_WING_ATTACKING_CAN;
		}
		msgsnd = AICF_WING_ATTACKING_CAN;
	}
	else
	{
		tempnum = rand() & 1;
		if(tempnum == 1)
		{
			msgnum = AIC_WING_EGAGING_GOMERS;
			if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
			{
				msgnum = AIC_WING_EGAGING_GOMERS_RAF;
			}
	//		else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
	//		{
	//			msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT_RUSA;
	//		}
			msgsnd = AICF_WING_EGAGING_GOMERS;
		}
		else
		{
			msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT;
			if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF))
			{
				msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT_RAF;
			}
			else if((Planes[planenum].AI.lAIVoice == SPCH_ATK_RUS))
			{
				msgnum = AIC_WING_TARGETS_SIGHTED_IN_HOT_RUSA;
			}
			msgsnd = AICF_WING_TARGETS_SIGHTED_IN_HOT;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICWingFinishCASAttackMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, tempstr2);

	if(targetnum == 100)
	{
		if(lNoRepeatFlags[planenum] & BOMB_MISS_SAID)
		{
			return;
		}

		lNoRepeatFlags[planenum] |= BOMB_MISS_SAID;
		if(lNoRepeatTimer[planenum] < 60000)
		{
			lNoRepeatTimer[planenum] = 60000;
		}

		msgnum = AIC_WING_HITS_IN_AREA;
		msgsnd = AICF_WING_HITS_IN_AREA;
	}
	else
	{
		tempnum = rand() & 1;
		if(tempnum == 1)
		{
			msgnum = AIC_WING_TARGET_DESTROYED_REFORMING;
			msgsnd = AICF_WING_TARGET_DESTROYED_REFORMING;
		}
		else
		{
			msgnum = AIC_WING_GROUND_DESTROYED;
			msgsnd = AICF_WING_GROUND_DESTROYED;
		}
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//  Air to Air attack.
//**************************************************************************************
void AICWingReadyToEngageMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, tempstr2);

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_ENGAGE_READY_MSG_3;
		msgsnd = AICF_ENGAGE_READY_MSG_2;
	}
	else
	{
		msgnum = AIC_ENGAGE_READY_MSG_1;
		msgsnd = AICF_ENGAGE_READY_MSG_1;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}
//  Air to Ground Attack.
//**************************************************************************************
void AICWingReadyToAttackMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}
	AIC_Get_Callsign_With_Number(planenum, tempstr2);


	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_WING_READY_TO_ATTACK;
		msgsnd = AICF_WING_READY_TO_ATTACK;
	}
	else
	{
		msgnum = AIC_WING_REQUEST_ATTACK;
		msgsnd = AICF_WING_REQUEST_ATTACK;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICWingSpreadOutMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}
	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_SPREADING_OUT, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(AICF_SPREADING_OUT, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICWingCloseUpMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}
	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_CLOSING_UP, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(AICF_CLOSING_UP, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICWingOrbitHereMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}
	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_ORBIT_RESPONSE_2;
		msgsnd = AICF_ORBIT_RESPONSE_2;
	}
	else
	{
		msgnum = AIC_ORBIT_RESPONSE_1;
		msgsnd = AICF_ORBIT_RESPONSE_1;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICWingCoverMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	int placeingroup;
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}
	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}


	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	tempnum = rand() & 1;
	if(tempnum == 1)
	{
		msgnum = AIC_COVER_RESPONSE_2;
		msgsnd = AICF_COVER_RESPONSE_2;
	}
	else
	{
		msgnum = AIC_COVER_RESPONSE_1;
		msgsnd = AICF_COVER_RESPONSE_1;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_SpikeCall(PlaneParams *planepnt, void *site, PlaneParams *pingplane, int grndtype)
{
	return;

	FPoint position;
	float bearing, fworkval;
	char positionstr[256];
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	GDRadarData *radardat, *actradardat;
	InfoProviderInstance *tprovider;
	int sitetype;
	DBWeaponType *weapon=NULL;
	float tdist, dx, dy, dz;
	BasicInstance *radarsite = NULL;
	MovingVehicleParams *vehiclesite = NULL;


	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(pingplane)
	{
		position = pingplane->WorldPosition;
		if((planepnt->AI.lSpeechOverloadTimer < 0) || (!(pingplane->AI.iAIFlags1 & AIWARNINGGIVEN)))
		{
			planepnt->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			pingplane->AI.iAIFlags1 |= AIWARNINGGIVEN;
			lNoticeTimer += 60000;
			if(planepnt->AI.iSide == pingplane->AI.iSide)
			{
				msgnum = AIC_BUDDY_SPIKE;
				msgsnd = AICF_BUDDY_SPIKE;
			}
			else
			{
				tempnum = rand() & 1;
				if(tempnum)
				{
					msgnum = AIC_WSO_MIG_SPIKE;
					msgsnd = AICF_WSO_MIG_SPIKE;
				}
				else
				{
					msgnum = AIC_WSO_SPIKE;
					msgsnd = AICF_WSO_SPIKE;
				}
			}
		}
		else
		{
			lNoticeTimer += 60000;
		}
	}
	else
	{
		if(grndtype == GROUNDOBJECT)
		{
			radarsite = (BasicInstance *)site;
			position.X = radarsite->Position.X;
			position.Y = radarsite->Position.Y;
			position.Z = radarsite->Position.Z;

			actradardat = radardat = GDGetRadarData(radarsite);
		}
		else
		{
			vehiclesite = (MovingVehicleParams *)site;
			position = vehiclesite->WorldPosition;

			if (!vehiclesite->iShipType) // SCOTT FIX
			{
				actradardat = radardat = vehiclesite->RadarWeaponData;
			}
			else
			{
				radardat = vehiclesite->RadarWeaponData;
				actradardat = VGDGetRadarDatAtPlane(vehiclesite, planepnt);
			}
		}

		lNoticeTimer += 60000;
		if(actradardat->lRFlags1 & GD_RADAR_LOCK)
		{
			if((planepnt->AI.lSpeechOverloadTimer < 0) || (!(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)))
			{
				msgnum = AIC_WSO_MUD_LOCK;
				msgsnd = AICF_WSO_MUD_LOCK;
				GDUpdateReportedArea(position, -1);
				planepnt->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			}
			else
			{
				GDUpdateReportedArea(position, -1);
				return;
			}
		}
		else
		{
			if(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)
			{
				return;
			}
			tempnum = rand() & 1;
			if(tempnum)
			{
				if(grndtype == MOVINGVEHICLE)
				{
					if (!vehiclesite->iShipType)
						weapon = get_weapon_ptr(pDBVehicleList[vehiclesite->iVDBIndex].lWeaponType);

					if (!weapon) return;

					if(weapon->iWeaponType == 6)
					{
						sitetype = IPF_LOCAL_AAA_PROVIDER;
					}
					else
					{
						sitetype = IPF_LOCAL_SAM_PROVIDER;
					}
				}
				else if(radarsite->Family == FAMILY_AAWEAPON)
				{
					weapon = GetWeaponPtr(radarsite);
					if(weapon->iWeaponType == 6)
					{
						sitetype = IPF_LOCAL_AAA_PROVIDER;
					}
					else
					{
						sitetype = IPF_LOCAL_SAM_PROVIDER;
					}
				}
				else
				{
					tprovider = (InfoProviderInstance *)radarsite;
					sitetype = tprovider->Type->ProviderFlags & IPF_PROVIDER_TYPE;
				}

				GDUpdateReportedArea(position, sitetype);

				if(sitetype == IPF_LOCAL_AAA_PROVIDER)
				{
					msgnum = AIC_WSO_AAA_SPIKE;
					msgsnd = AICF_WSO_AAA_SPIKE;
				}
				else if(sitetype == IPF_LOCAL_SAM_PROVIDER)
				{
					msgnum = AIC_WSO_SAM_SPIKE;
					msgsnd = AICF_WSO_SAM_SPIKE;
				}
				else
				{
					msgnum = AIC_WSO_GCI_SPIKE;
					msgsnd = AICF_WSO_GCI_SPIKE;
				}
			}
			else
			{
				if(radardat->lRFlags1 & (GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN))
				{
					return;
				}
				GDUpdateReportedArea(position, -1);
				msgnum = AIC_WSO_MUD_SPIKE;
				msgsnd = AICF_WSO_MUD_SPIKE;
			}
		}
	}

	bearing = AIComputeHeadingToPoint(planepnt, position, &tdist, &dx ,&dy, &dz, 0);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planepnt - Planes, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planepnt - Planes, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
}

//**************************************************************************************
void AIC_Wingman_SpikeCall(PlaneParams *planepnt, void *site, PlaneParams *pingplane, int grndtype)
{
	FPoint position;
	float bearing, fworkval;
	char positionstr[256];
	char placestr[256];
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	GDRadarData *radardat, *actradardat;
	InfoProviderInstance *tprovider;
	int sitetype;
	DBWeaponType *weapon=NULL;
	int placeingroup;
	float tdist, dx, dy, dz;
	BasicInstance *radarsite = NULL;
	MovingVehicleParams *vehiclesite = NULL;
	int planenum = planepnt - Planes;
	double use_radius;
	double dtdist;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

//	if(!AICCheckVoiceNumInGroup(planepnt - Planes, planepnt->AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}


//	if(!LANGGetTransMessage(placestr, 128, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}

	AIC_Get_Callsign_With_Number(planenum, placestr);

	if(pingplane)
	{
		position = pingplane->WorldPosition;
		if((planepnt->AI.lSpeechOverloadTimer < 0) || (!(pingplane->AI.iAIFlags1 & AIWARNINGGIVEN)))
		{
			planepnt->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			pingplane->AI.iAIFlags1 |= AIWARNINGGIVEN;
			lNoticeTimer += 60000;
			if(planepnt->AI.iSide == pingplane->AI.iSide)
			{
				msgnum = AIC_WING_RADAR_DETECT_9;  //  Buddy spike
				msgsnd = AICF_WING_RADAR_DETECT_9;
			}
			else
			{
				tempnum = rand() & 1;
				if(tempnum)
				{
					msgnum = AIC_WING_RADAR_DETECT_5;		//  Spike MiG
					msgsnd = AICF_WING_RADAR_DETECT_5;
				}
				else
				{
					msgnum = AIC_WING_RADAR_DETECT_8;	//  Spike spike
					msgsnd = AICF_WING_RADAR_DETECT_8;
				}
			}
		}
		else
		{
			lNoticeTimer += 60000;
		}
	}
	else if(site)
	{
		if(grndtype == GROUNDOBJECT)
		{
			radarsite = (BasicInstance *)site;
			position.X = radarsite->Position.X;
			position.Y = radarsite->Position.Y;
			position.Z = radarsite->Position.Z;

			dtdist = radarsite->Position - planepnt->WorldPosition;

			actradardat = radardat = GDGetRadarData(radarsite);
		}
		else
		{
			vehiclesite = (MovingVehicleParams *)site;
			position = vehiclesite->WorldPosition;

			dtdist = vehiclesite->WorldPosition - planepnt->WorldPosition;

			if (!vehiclesite->iShipType) // SCOTT FIX
			{
				actradardat = radardat = vehiclesite->RadarWeaponData;
			}
			else
			{
				radardat = vehiclesite->RadarWeaponData;
				actradardat = VGDGetRadarDatAtPlane(vehiclesite, planepnt);
			}
		}

		if(dtdist)
		{
			use_radius = tan(DegToRad(20.0f)) * dtdist;
		}
		else
		{
			use_radius = 2.0f * NMTOWU;
		}

		lNoticeTimer += 60000;
		if(actradardat->lRFlags1 & GD_RADAR_LOCK)
		{
			if((planepnt->AI.lSpeechOverloadTimer < 0) || (!(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)))
			{
				msgnum = AIC_WING_RADAR_DETECT_7;	//  Mud Lock on
				msgsnd = AICF_WING_RADAR_DETECT_7;
				GDUpdateReportedArea(position, -1, use_radius);
				planepnt->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			}
			else
			{
				GDUpdateReportedArea(position, -1, use_radius);
				return;
			}
		}
		else
		{
			if(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)
			{
				return;
			}

			tempnum = rand() & 1;
			if(tempnum)
			{
				if(grndtype == MOVINGVEHICLE)
				{
					if (!vehiclesite->iShipType)
						weapon = get_weapon_ptr(pDBVehicleList[vehiclesite->iVDBIndex].lWeaponType);

					if(!weapon)
					{
						sitetype = IPF_LOCAL_SAM_PROVIDER;
					}
					else if(weapon->iWeaponType == 6)
					{
						sitetype = IPF_LOCAL_AAA_PROVIDER;
					}
					else
					{
						sitetype = IPF_LOCAL_SAM_PROVIDER;
					}
				}
				else if(radarsite->Family == FAMILY_AAWEAPON)
				{
					weapon = GetWeaponPtr(radarsite);
					if(weapon->iWeaponType == 6)
					{
						sitetype = IPF_LOCAL_AAA_PROVIDER;
					}
					else
					{
						sitetype = IPF_LOCAL_SAM_PROVIDER;
					}
				}
				else
				{
					tprovider = (InfoProviderInstance *)radarsite;
					sitetype = tprovider->Type->ProviderFlags & IPF_PROVIDER_TYPE;
				}

				GDUpdateReportedArea(position, sitetype, use_radius);
				if(sitetype == IPF_LOCAL_AAA_PROVIDER)
				{
					msgnum = AIC_WING_RADAR_DETECT_4;	//  Spike AAA
					msgsnd = AICF_WING_RADAR_DETECT_4;
				}
				else if(sitetype == IPF_LOCAL_SAM_PROVIDER)
				{
					msgnum = AIC_WING_RADAR_DETECT_3;	//  Spike SAM
					msgsnd = AICF_WING_RADAR_DETECT_3;
				}
				else
				{
					msgnum = AIC_WING_RADAR_DETECT_2;	//  Spike GCI
					msgsnd = AICF_WING_RADAR_DETECT_2;
				}
			}
			else
			{
				if(radardat->lRFlags1 & (GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN))
				{
					return;
				}
				msgnum = AIC_WING_RADAR_DETECT_1;	//  has Mud Spike at
				msgsnd = AICF_WING_RADAR_DETECT_1;
				GDUpdateReportedArea(position, -1, use_radius);
			}
		}
	}
	else
	{
		planepnt->AI.iAICombatFlags1 |= AI_NAKED;

		msgnum = AIC_WING_NAKED;
		msgsnd = AICF_SEAD_NAKED;
//		msgsnd = AICF_WING_NAKED;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, placestr))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planepnt - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		return;
	}

	bearing = AIComputeHeadingToPoint(planepnt, position, &tdist, &dx ,&dy, &dz, 0);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planepnt - Planes, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, placestr, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planepnt - Planes, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
}

//**************************************************************************************
void AIC_SEAD_SpikeCall(PlaneParams *planepnt, void *site, PlaneParams *pingplane, int grndtype)
{
	AIC_Wingman_SpikeCall(planepnt, site, pingplane, grndtype);
	return;

	FPoint position;
	float bearing, fworkval;
	char bearstr[256];
	char tempstr[1024];
	char callsign[256];
	char awacscallsign[256];
	int msgnum, msgsnd;
	GDRadarData *radardat, *actradardat;
	float tdist, dx, dy, dz;
	int awacsnum;
	int bearing360;
	int placeingroup;
	BasicInstance *radarsite = NULL;
	MovingVehicleParams *vehiclesite = NULL;
	double use_radius;
	double dtdist;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (planepnt->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!AICCheckVoiceNumInGroup(planepnt - Planes, planepnt->AI.lAIVoice, placeingroup))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
	{
		return;
	}

	AICGetCallSign(awacsnum, awacscallsign);

	AIC_Get_Callsign_With_Number(planepnt - Planes, callsign);


	if(pingplane)
	{
		position = pingplane->WorldPosition;
		if((planepnt->AI.lSpeechOverloadTimer < 0) || (!(pingplane->AI.iAIFlags1 & AIWARNINGGIVEN)))
		{
			planepnt->AI.lSpeechOverloadTimer = PING_WARNING_QUIET;
			pingplane->AI.iAIFlags1 |= AIWARNINGGIVEN;
			lNoticeTimer += 60000;
			if(planepnt->AI.iSide == pingplane->AI.iSide)
			{
				msgnum = AIC_SEAD_BUDDY_SPIKE;
				msgsnd = AICF_SEAD_BUDDY_SPIKE;
			}
			else
			{
				msgnum = AIC_SEAD_SPIKE;
				msgsnd = AICF_SEAD_SPIKE;
			}
		}
		else
		{
			lNoticeTimer += 60000;
			return;
		}
	}
	else if(site)
	{
		if(grndtype == GROUNDOBJECT)
		{
			radarsite = (BasicInstance *)site;
			position.X = radarsite->Position.X;
			position.Y = radarsite->Position.Y;
			position.Z = radarsite->Position.Z;

			dtdist = radarsite->Position - planepnt->WorldPosition;
			if(dtdist)
			{
				use_radius = tan(DegToRad(20.0f)) * dtdist;
			}
			else
			{
				use_radius = 2.0f * NMTOWU;
			}

			radardat = GDGetRadarData(radarsite);
			lNoticeTimer += 60000;
			if(radardat->lRFlags1 & GD_RADAR_LOCK)
			{
				return;
			}
			else
			{
				if(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)
				{
					return;
				}
				msgnum = AIC_SEAD_MUD_SPIKE;
				msgsnd = AICF_SEAD_MUD_SPIKE;
				planepnt->AI.iAICombatFlags1 &= ~AI_NAKED;
				GDUpdateReportedArea(position, -1, use_radius);
			}
		}
		else
		{
			vehiclesite = (MovingVehicleParams *)site;
			position = vehiclesite->WorldPosition;

			dtdist = vehiclesite->WorldPosition - planepnt->WorldPosition;
			if(dtdist)
			{
				use_radius = tan(DegToRad(20.0f)) * dtdist;
			}
			else
			{
				use_radius = 2.0f * NMTOWU;
			}


			if (!vehiclesite->iShipType) // SCOTT FIX
			{
				actradardat = radardat = vehiclesite->RadarWeaponData;
			}
			else
			{
				radardat = vehiclesite->RadarWeaponData;
				actradardat = VGDGetRadarDatAtPlane(vehiclesite, planepnt);
			}

			lNoticeTimer += 60000;
			if(actradardat->lRFlags1 & GD_RADAR_LOCK)
			{
				return;
			}
			else
			{
				if(radardat->lRFlags1 & GD_HAS_BEEN_REPORTED)
				{
					return;
				}
				msgnum = AIC_SEAD_MUD_SPIKE;
				msgsnd = AICF_SEAD_MUD_SPIKE;
				planepnt->AI.iAICombatFlags1 &= ~AI_NAKED;
				GDUpdateReportedArea(position, -1, use_radius);
			}
		}
	}
	else
	{
		planepnt->AI.iAICombatFlags1 |= AI_NAKED;

		msgnum = AIC_SEAD_NAKED;
		msgsnd = AICF_SEAD_NAKED;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planepnt - Planes, awacsnum, 0, 0, 0, 0, 0, 0, 0, 0);
		return;
	}

	bearing = AIComputeHeadingToPoint(planepnt, position, &tdist, &dx ,&dy, &dz, 1);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		bearing360 = fworkval + 360;
	}
	else
	{
		bearing360 = fworkval;
	}
	sprintf(bearstr, "%03d", bearing360);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, awacscallsign, callsign, bearstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planepnt - Planes, awacsnum, bearing, 0, 0, 0, 0, 0, 0, 0);
}

//***********************************************************************************************************************************
int AICCheckAllFences(float x, float z)
{
	int tempfence = 0;

#ifdef __DEMO__
	tempfence = 1;
#else
#if 1
	int cnt;

	for(cnt = 0; cnt < g_iNumFenceArea; cnt ++)
	{
		if(lFenceFlags & (1<<cnt))
		{
			tempfence += AICCheckOneFence(FenceArea[cnt].iNumPoints, FenceArea[cnt].point, x, z);
		}
	}
#else
		if(lFenceFlags & (FENCE_IRAQ))
		{
			tempfence += AICCheckOneFence(FenceArea[0].iNumPoints, FenceArea[0].point, x, z);
		}
		if(lFenceFlags & (FENCE_KUWAIT))
		{
			tempfence += AICCheckOneFence(FenceArea[3].iNumPoints, FenceArea[3].point, x, z);
		}
		if(lFenceFlags & (FENCE_IRAN))
		{
			tempfence += AICCheckOneFence(FenceArea[4].iNumPoints, FenceArea[4].point, x, z);
		}
#endif
#endif
	return(tempfence);
}

//***********************************************************************************************************************************
int AICCheckOneFence(int num_polys, MPoint vertexlist[], float x, float z)
{
	int i, j, c = 0;

	for (i = 0, j = num_polys-1; i < num_polys; j = i++)
	{
		if ((((vertexlist[i].z<=z) && (z<vertexlist[j].z)) ||
             ((vertexlist[j].z<=z) && (z<vertexlist[i].z))) &&
            (x < (vertexlist[j].x - vertexlist[i].x) * (z - vertexlist[i].z) / (vertexlist[j].z - vertexlist[i].z) + vertexlist[i].x))
          c = !c;
	}
	return c;
}

//***********************************************************************************************************************************
void AICFenceCall(int fenceval, int planeid)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char callsign[256];
	int planenum = planeid;

	if(planenum == -1)
		planenum = PlayerPlane - Planes;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if(fenceval == 0)
	{
		msgnum = AIC_WSO_FENCE_OUT;
		msgsnd = AICF_WSO_FENCE_OUT;
	}
	else
	{
		msgnum = AIC_WSO_FENCE_IN;
		msgsnd = AICF_WSO_FENCE_IN;

//		AICAddSoundCall(AIC_WSO_RadarCheck, PlayerPlane - Planes, 10000, 50);
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, PlayerPlane - Planes);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_RadarCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	int radarok = !(planepnt->DamageFlags & (1<<WARN_RADAR));

	if((targetnum != 1) && ((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0)))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(radarok)
	{
		msgnum = AIC_WSO_RADAR_BIT_CLEAN;
		msgsnd = AICF_WSO_RADAR_BIT_CLEAN;
	}
	else
	{
		msgnum = AIC_WSO_RADAR_OUT;
		msgsnd = AICF_WSO_RADAR_OUT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_WinderCheck, planenum, 5000, 50);

	return;
}

//**************************************************************************************
void AIC_WSO_WinderCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	int winderok = 0;
	int cnt;
	int seeker, wtlist;
	PlaneParams *planepnt = &Planes[planenum];

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		wtlist = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		if(((wtlist == 2) || (wtlist == 3) || (wtlist == 4)) && (planepnt->WeapLoad[cnt].Count > 0))
		{
			seeker = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iSeekerType;
			if((seeker == 2) || (seeker == 3))
			{
				winderok = 1;
			}
		}
	}

	if(winderok)
	{
		msgnum = AIC_WSO_WINDERS_OK;
		msgsnd = AICF_WSO_WINDERS_OK;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_SparrowCheck, planenum, 5000, 50);
	return;
}


//**************************************************************************************
void AIC_WSO_SparrowCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	int sparrowok = 0;
	int cnt;
	int seeker, wtlist;
	PlaneParams *planepnt = &Planes[planenum];

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		wtlist = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		if(((wtlist == 2) || (wtlist == 3) || (wtlist == 4)) && (planepnt->WeapLoad[cnt].Count > 0))
		{
			seeker = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iSeekerType;
			if(seeker == 7)
			{
				sparrowok = 1;
			}
		}
	}

	if(sparrowok)
	{
		msgnum = AIC_WSO_SPARROWS_GOOD;
		msgsnd = AICF_WSO_SPARROWS_GOOD;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_SlammerCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_SlammerCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	int slammerok = 0;
	int cnt;
	int seeker, wtlist;
	PlaneParams *planepnt = &Planes[planenum];

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		wtlist = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		if(((wtlist == 2) || (wtlist == 3) || (wtlist == 4)) && (planepnt->WeapLoad[cnt].Count > 0))
		{
			seeker = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iSeekerType;
			if(seeker == 1)
			{
				slammerok = 1;
			}
		}
	}

	if(slammerok)
	{
		msgnum = AIC_WSO_SLAMMERS_GOOD;
		msgsnd = AICF_WSO_SLAMMERS_GOOD;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_MudCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_MudCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	int mudok = 0;
	int cnt;
	int wtlist;
	PlaneParams *planepnt = &Planes[planenum];

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	for (cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
	{
		wtlist = pDBWeaponList[planepnt->WeapLoad[cnt].WeapIndex].iWeaponType;
		if(((wtlist == 0) || (wtlist == 1) || (wtlist == 5) || (wtlist == 11)) && (planepnt->WeapLoad[cnt].Count > 0))
		{
			if(planepnt->WeapLoad[cnt].WeapId != 40)
			{
				mudok = 1;
			}
		}
	}

	if(mudok)
	{
		msgnum = AIC_WSO_A2G_OK;
		msgsnd = AICF_WSO_A2G_OK;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_IFFCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_IFFCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int iffok = 1;

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(iffok)
	{
		msgnum = AIC_WSO_IFF_SET;
		msgsnd = AICF_WSO_IFF_SET;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_EngineCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_EngineCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int engineok = 1;

	if((targetnum < 1) && ((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0)))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(lNoCheckPlanelMsgs < 0)
	{
		lBombFlags &= ~(WSO_DEEP_DOO_DOO|WSO_ENGINE1_BAD|WSO_ENGINE2_BAD|WSO_HYD_BAD);
	}
	if((planepnt->DamageFlags & ((DAMAGE_BIT_R_ENGINE)|(DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_R_ENG_CONT)|(DAMAGE_BIT_R_OIL_PRESS)|(DAMAGE_BIT_R_BURNER)|(DAMAGE_BIT_R_FUEL_PUMP)|(DAMAGE_BIT_R_GEN)|(DAMAGE_BIT_R_BLEED_AIR))) && ((targetnum < 1) || (targetnum & 4)))
	{
		msgnum = AIC_WSO_ENGINE_TWO_DAMAGE;
		msgsnd = AICF_WSO_ENGINE_TWO_DAMAGE;
		if(!(lBombFlags & WSO_ENGINE2_BAD))
		{
			lBombFlags |= WSO_ENGINE2_BAD;
			lNoCheckPlanelMsgs = 10000;

			if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
			{
				return;
			}

			AIRGenericSpeech(msgsnd, planenum);

			AICAddAIRadioMsgs(tempstr, 40);
		}
		engineok = 0;
	}
	if((planepnt->DamageFlags & ((DAMAGE_BIT_L_ENGINE)|(DAMAGE_BIT_RO_ENGINE)|(DAMAGE_BIT_L_ENG_CONT)|(DAMAGE_BIT_L_OIL_PRESS)|(DAMAGE_BIT_L_BURNER)|(DAMAGE_BIT_L_FUEL_PUMP)|(DAMAGE_BIT_L_GEN)|(DAMAGE_BIT_L_BLEED_AIR))) && ((targetnum < 1) || (targetnum & 2)))
	{
		msgnum = AIC_WSO_ENGINE_ONE_DAMAGE;
		msgsnd = AICF_WSO_ENGINE_ONE_DAMAGE;
		if(!(lBombFlags & WSO_ENGINE1_BAD))
		{
			lBombFlags |= WSO_ENGINE1_BAD;
			lNoCheckPlanelMsgs = 10000;

			if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
			{
				return;
			}

			AIRGenericSpeech(msgsnd, planenum);

			AICAddAIRadioMsgs(tempstr, 40);
		}
		engineok = 0;
	}


	if(engineok)
	{
		msgnum = AIC_WSO_ENGINES_GOOD;
		msgsnd = AICF_WSO_ENGINES_GOOD;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum < 1)
		AICAddSoundCall(AIC_WSO_TEWSCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_TEWSCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int tewsok = !(planepnt->DamageFlags & (1<<WARN_TEWS));

	if((targetnum != 1) && ((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0)))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(tewsok)
	{
		msgnum = AIC_WSO_TEWS_UP;
		msgsnd = AICF_WSO_TEWS_UP;
	}
	else
	{
		msgnum = AIC_WSO_LOST_TEWS;
		msgsnd = AICF_WSO_LOST_TEWS;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_RadioCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_RadioCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int radiook = 1;

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(radiook)
	{
		msgnum = AIC_WSO_RADIO_SET;
		msgsnd = AICF_WSO_RADIO_SET;

		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AIRGenericSpeech(msgsnd, planenum);

		AICAddAIRadioMsgs(tempstr, 40);
	}

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_LightCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_LightCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int radiook = 1;

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}
	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_CHECK_LIGHTS_OFF;
	msgsnd = AICF_WSO_CHECK_LIGHTS_OFF;

	planepnt->Status &= ~PL_RUNNING_LIGHTS;

	if(planepnt->Status & PL_FORMATION_LIGHTS)
	{
		planepnt->Status &= ~PL_FORMATION_LIGHTS;
		planepnt->Status |= 0x1000;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);

	if(targetnum != 1)
		AICAddSoundCall(AIC_WSO_PlatformCheck, planenum, 5000, 50);
	return;
}

//**************************************************************************************
void AIC_WSO_PlatformCheck(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if((PlayerPlane->AI.AirThreat) || (PlayerPlane->AI.iMissileThreat >= 0))
	{
		return;
	}

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_PLATFORM_TIGHT;
	msgsnd = AICF_WSO_PLATFORM_TIGHT;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	if(targetnum != 1)
		AICAddAIRadioMsgs(tempstr, 40);

//#ifdef __DEMO__
//	AICAddSoundCall(AICDelayPictureRequest, planenum, 2000, 50);
//#endif
	return;
}

//**************************************************************************************
void AICNextTextColor()
{
	int working = 1;

	while(working > 0)
	{
		if(working == 1)
		{
			working = 0;
		}
		else
		{
			working = -1;
		}

		switch(iMsgTextColor)
		{
			case -10:
				iMsgTextColor = 207;
				break;
			case 207:
				iMsgTextColor = 248;
				break;
			case 248:
				iMsgTextColor = 249;
				break;
			case 249:
				iMsgTextColor = 251;
				break;
			case 251:
				iMsgTextColor = 252;
				break;
			case 252:
				iMsgTextColor = 255;
				break;
			case 255:
				iMsgTextColor = -10;
				break;
			default:
				iMsgTextColor = -10;
				break;
		}

		if((iMsgBoxColor == iMsgTextColor) && (working == 0))
		{
			working = 2;
		}
	}
}

//**************************************************************************************
void AICNextBoxColor()
{
	int working = 1;

	while(working > 0)
	{
		if(working == 1)
		{
			working = 0;
		}
		else
		{
			working = -1;
		}

		switch(iMsgBoxColor)
		{
			case -20:
				iMsgBoxColor = 199;
				break;
			case 199:
				iMsgBoxColor = 255;
				break;
			case 255:
				iMsgBoxColor = -1;
				break;
			case -1:
				iMsgBoxColor = 207;
				break;
			case 207:
				iMsgBoxColor = -20;
				break;
			default:
				iMsgBoxColor = -20;
				break;
		}

		if((iMsgBoxColor == iMsgTextColor) && (working == 0))
		{
			working = 2;
		}
	}
}

//************************************************************************************************
void ControlJumpPlanes(void)
{
	PlaneParams *P;

//	PlayerPlane->AI.AOADegPitch = 0;
//	PlayerPlane->Alpha = 0;

	//  Helo added SRE
	//  This stuff HAS to be outside of the main loop because wingmen and other planes need
	//  to reference the true pitch of a plane.
	for (P=Planes; P<=LastPlane; P++)
	{
		if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED))
		{
			if((P->AI.iAIFlags2 & AILANDING) || (P->AI.iAIFlags2 & AI_FAKE_AOA) || (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
					|| ((pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (P->Knots < 120.0f)))
			{
				P->Pitch -= P->AI.AOADegPitch;
			}
		}
	}

	for (P=Planes; P<=LastPlane; P++)
	{
		if (!(P->Status & PL_ACTIVE))  continue;

		if (P->FlightStatus & PL_STATUS_CRASHED)  continue;

		P->DoAttitude(P);

		if(P == PlayerPlane)
		{
			AIResetPercents(P);
			if(P->AI.Behaviorfunc == AIFlyTakeOff)
			{
				AIFlyTakeOff(P);
			}
			else
			{
				AIFlyFormation(P);
			}
			AIControlSurfaces(P);
		}
		else if ((P->Status & PL_DEVICE_DRIVEN) || (P->Status & PL_COMM_DRIVEN))
		{
			P->DoGetPlayerInputs(P);
		   	AIUpdateFormation(P);
		}
		else if (P->Status & PL_AI_DRIVEN)
		{
			AIResetPercents(P);
		   	P->AI.Behaviorfunc(P);
			AIControlSurfaces(P);
		}

		/* I'm sure this shouldn't go here! */

		switch (P->Type->EngineConfig)
		{
			case TWO_REAR_ENGINES:
				if (P->SystemsStatus & PL_ENGINE_REAR_RIGHT)
					P->RightThrustPercent = 0.0f;

				if (P->SystemsStatus & PL_ENGINE_REAR_LEFT)
					P->LeftThrustPercent = 0.0f;
				break;

			case ONE_REAR_ENGINE:
				if (P->SystemsStatus & PL_ENGINE_REAR)
					P->RightThrustPercent = P->LeftThrustPercent = 0.0f;
				break;
		}
	}

	//  This stuff HAS to be outside of the main loop because wingmen and other planes need
	//  to reference the true last frame pitch of a plane.  Planes earlier in the list would
	//  see a different 'last' value then plane later in the list.
	//  Also need to reset the helicopter and landing plane pitches.
	for (P=Planes; P<=LastPlane; P++)
	{
		if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED))
		{
			P->AI.LastPitch = P->Pitch;
			//  Helo added SRE
			if((P->AI.iAIFlags2 & AILANDING) || (pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
					|| ((pDBAircraftList[P->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_VERTICAL) && (P->Knots < 120.0f)))
			{
				P->Pitch += P->AI.AOADegPitch;
			}

			P->AI.LastRoll =  P->Roll;
			P->AI.LastHeading = P->Heading;
			P->AI.LastSymetricalElevator = P->SymetricalElevator;
		}
	}
}

//**************************************************************************************
void RestoreFromJump()
{
	DoubleVector3D clearvector;
	double miny, dy;
	int planenum;
	PlaneParams *planepnt;
	ANGLE heading;
	double workspeed;
	float airdensity, Alt;

	clearvector.X = 0;
	clearvector.Y = 0;
	clearvector.Z = 0;

	iInJump = 0;
	TimeExcel = 0;

	if((!lAfterJump) && ((!UFC.APStatus) || (RealisticAPs)))
	{
		PlayerPlane->Status |= AL_DEVICE_DRIVEN;
		PlayerPlane->Status &= ~AL_AI_DRIVEN;
	//	AICAddAIRadioMsgs("DEVICE", 50);
		PlayerPlane->DoControlSim = CalcF18ControlSurfaces;
		PlayerPlane->DoCASSim     = CalcF18CASUpdates;
		PlayerPlane->DoForces     = CalcAeroForces;
		PlayerPlane->DoPhysics    = CalcAeroDynamics;
		PlayerPlane->dt = HIGH_FREQ;
		PlayerPlane->UpdateRate = HIGH_AERO;
	}

	PlayerPlane->Alpha = 0;
	PlayerPlane->LastAlpha = -999.0;

	PlayerPlane->BfLinVel.Y = 0;
	PlayerPlane->BfLinVel.Z = 0;
	PlayerPlane->BfForce = clearvector;
	PlayerPlane->BfMoment = clearvector;
	PlayerPlane->BfGhostLinVel = clearvector;
	PlayerPlane->BfRotVel = clearvector;
	PlayerPlane->BfLinAccel = clearvector;
	PlayerPlane->BfRotAccel = clearvector;
	PlayerPlane->BfVelocity = clearvector;
	PlayerPlane->IfVelocity = clearvector;
	PlayerPlane->ElevatorTrim = 0;
	PlayerPlane->AltitudeHoldTrim = 0;
	PlayerPlane->AttitudeHoldTrim = 0;

	PlayerPlane->Aileron = 0;				// current position
	PlayerPlane->Rudder = 0;
	PlayerPlane->TotalElevator = 0;			// get rid of
	PlayerPlane->SymetricalElevator = 0;
	PlayerPlane->DifferentialElevator = 0;

	heading = PlayerPlane->Heading;
	AISetOrientation(PlayerPlane, AIConvertAngleTo180Degree(heading));


	if(!lAfterJump)
	{
		JustLeftPausedState = 1;

		Alt = PlayerPlane->WorldPosition.Y * WUTOFT;

		airdensity = sqrt(Pressure[(int)((Alt)/500.0f)]/0.0023769f);

		if((airdensity <= 0) || (airdensity > 1.0f))
		{
			airdensity = 1.0f;
		}

		workspeed = (double) (500.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS)) / airdensity;

		if(workspeed > PlayerPlane->BfLinVel.X)
		{
			PlayerPlane->BfLinVel.X = workspeed;
		}


		OffDiskTerrainInfo->m_Location = PlayerPlane->WorldPosition;
		OffDiskTerrainInfo->GetInfo(TI_WANT_HEIGHT);

		miny = OffDiskTerrainInfo->m_Location.Y + (7.0 * FOOT_MULTER);
		if(PlayerPlane->AI.numwaypts > 1)
		{
			miny += (5000.0f *FTTOWU);
		}
		else
		{
			miny += (2500.0f *FTTOWU);
		}

		if(PlayerPlane->WorldPosition.Y < miny)
		{
			dy = miny - PlayerPlane->WorldPosition.Y;
			PlayerPlane->WorldPosition.Y = miny;
			PlayerPlane->LastWorldPosition = PlayerPlane->WorldPosition;

			if(PlayerPlane->AI.wingman >= 0)
			{
				planepnt = &Planes[PlayerPlane->AI.wingman];
				if((!(planepnt->AI.iAIFlags1 & AINOFORMUPDATE)) && (planepnt->AI.iAIFlags1 & AIINFORMATION))
				{
					planepnt->WorldPosition.Y += dy;
					planepnt->LastWorldPosition = planepnt->WorldPosition;
				}
			}

			planenum = PlayerPlane->AI.nextpair;
			while(planenum >= 0)
			{
				planepnt = &Planes[planenum];
				if((!(planepnt->AI.iAIFlags1 & AINOFORMUPDATE)) && (planepnt->AI.iAIFlags1 & AIINFORMATION))
				{
					planepnt->WorldPosition.Y += dy;
					planepnt->LastWorldPosition = planepnt->WorldPosition;
				}

				if(planepnt->AI.wingman >= 0)
				{
					planepnt = &Planes[planepnt->AI.wingman];
					if((!(planepnt->AI.iAIFlags1 & AINOFORMUPDATE)) && (planepnt->AI.iAIFlags1 & AIINFORMATION))
					{
						planepnt->WorldPosition.Y += dy;
						planepnt->LastWorldPosition = planepnt->WorldPosition;
					}
				}
				planenum = planepnt->AI.nextpair;
			}
		}

		StartEngineSounds();
		char wavestr[_MAX_PATH];
		char *tempstr;
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath("lightspd.wav", drive, dir, fname, ext);
		tempstr = GetRegValue("mission");

		sprintf(wavestr, "%s\\%s%s", tempstr, fname, ext);

		if(SndStreamInit(wavestr))
		{
			SndStreamPlay();
		}

		lAfterJump = 1;

		if(UFC.APStatus)
			lAfterJump = 2;

		PreLoadLand(PlayerPlane->WorldPosition);
	}
}

//***********************************************************************************************************************************
void DoJumpGame(void)
{
	long orgdticks, workticks, finalticks, dticks;
	int boxcolor, textcolor;
	static int pass = 1;
	GrFontBuff *usefont = MessageFont;
	long ltempval;
	long startticks, endticks, moveloops;
	int TimeMult = 1;
	char OutStr[256];
	char JumpStr[256];

	ltempval = lLastAverageMovePlane;

	if(lLastAverageMovePlane <= 0)  //  we don't want TimeExcel to go above the point where Moving the 50Hz stuff takes longer than 1/50th of a second.
	{
		TimeExcel = 5;
		TimeMult = 32;
	}
	else if(lLastAverageMovePlane > 10)
	{
		TimeExcel = 0;
		TimeMult = 1;
	}
	else
	{
		TimeExcel = 1;
		TimeMult = 1;
#if 0
		ltempval <<= 2;
		while(ltempval < 20)
		{
			TimeExcel ++;
			ltempval <<= 1;
		}
#else
		while((ltempval * (TimeMult + 1)) < 20)
		{
			TimeMult ++;
#if 0
			if(TimeMult > 32)
			{
				PauseForDiskHit();
				if(lpDD)
				  lpDD->FlipToGDISurface();
				DebugBreak();
				UnPauseFromDiskHit();
			}
#endif
		}
#endif
	}

	LastTick     = CurrentTick;
	CurrentTick  = GetTickCount();
	dticks = DeltaTicks   = CurrentTick - LastTick;

	pass = 0;

	if(DeltaTicks < 125)
	{
		DeltaTicks = 125;
	}
	orgdticks = DeltaTicks;

	lJumpTimer += DeltaTicks;

	if(orgdticks > 140)
	{
		orgdticks = 140;
#if 0
		if(DeltaTicks > 5000)
		{
			PauseForDiskHit();
			if(lpDD)
			  lpDD->FlipToGDISurface();
			DebugBreak();
			UnPauseFromDiskHit();
		}
#endif
		DeltaTicks = 140;
		TimeExcel = 0;
		TimeMult = 1;
	}

#if 0
	DeltaTicks <<= TimeExcel;
#else
	DeltaTicks *= TimeMult;
#endif
	finalticks = DeltaTicks;

	WorldParams.WorldTime += DeltaTicks * 0.001f;
	if (WorldParams.WorldTime > 86400.0f)
		WorldParams.WorldTime = 0.0f;

	sqrtDeltaTicks = (float)sqrt((float)DeltaTicks);

//	seconds_per_frame = DeltaTicks/1000.0f;
	seconds_per_frame = 0.0f;

	FrameRate = 1000.0f / DeltaTicks;
	OneOverFrameRate = 1.0f / FrameRate;

//	dwDeadX = dwDeadY = dwDeadR = 6000;
//	ReadJoyVarD(dwDeadZone);

	KeyScanButtonLayout();

	// Following redundant variables are both used by flames.cpp
	//
	GameLoopsPerFrame = 0;
	LastGameLoop = GameLoop;  // save for GameLoops this frame

	if(iInJump)
//	if (!SimPause)
	{
		TickCount += DeltaTicks;
		DeltaTicks = orgdticks;
		ControlJumpPlanes();
		VMoveVehicles();
		DeltaTicks = finalticks;
		PlayerPlane->LeftThrustPercent = PlayerPlane->RightThrustPercent = 30.0f;
		PlayerPlane->LeftThrottlePos = PlayerPlane->RightThrottlePos = 30.0f;
		PlayerPlane->LeftCommandedThrottlePos = PlayerPlane->RightCommandedThrottlePos = 30.0f;
		ConsumeFuel(PlayerPlane, DeltaTicks);
		AICDoMessagesAndComms();
		if(PlayerPlane->InternalFuel < 2000)
		{
			PlayerPlane->InternalFuel = 2000;
		}

		if(TimeExcel)
		{
			workticks = orgdticks;
		}

		moveloops = 0;
		startticks = CurrentTick;
		while (TickCount >= 20)
   		{
			GameLoop++;
			GameLoopsPerFrame++;
			GameLoopInTicks += 20;
			moveloops++;
			MovePlanes();
			MoveWeapons();
			MoveAAAStreamVertexes();
			UpdateScheduledExplosions(); //This should, maybe, be moved outside loop.
			//Detect collisions here
			TickCount -= 20;
			if(TimeExcel)
			{
				workticks -= 20;
				if(workticks < 0)
				{
					workticks += orgdticks;
					DeltaTicks = orgdticks;
					ControlJumpPlanes();
					VMoveVehicles();
					AICDoMessagesAndComms();
					DeltaTicks = finalticks;
				}
			}
		}
		seconds_per_frame = GameLoopsPerFrame/50.0f;
		endticks  = GetTickCount();
		if(moveloops)
			lLastAverageMovePlane = (endticks - startticks) / (float)moveloops;
		else
			lLastAverageMovePlane = 0;

		// Send my plane's data, get net planes & move net planes
		//
		void NetWork();
		if (MultiPlayer)  NetWork();

//		SndPauseAllSounds();
		SndEndAllSounds ();				// Kills ALL sounds

		MoveGenerators();
		MoveCanisters();

		MoveSprites();

		UpdateOrphanedSmokeTrails();

//		AICDoMessagesAndComms();
//		FixBridges();

		void NetData();
		if (MultiPlayer) NetData();

		if(iInJump & 4)
		{
			RestoreFromJump();
		}

		if((lJumpTimer > 2000) && (iInJump))
		{
			int boxheight, fontheight;


			boxheight = fontheight = TXTGetMaxFontHeight(usefont);

			if(iInJump & 2)
			{
				boxcolor = 255;
				textcolor = 207;
				iInJump &= ~2;
			}
			else
			{
				boxcolor = 207;
				textcolor = 255;
				iInJump |= 2;
			}

			if(!LANGGetTransMessage(JumpStr, 256, AIC_JUMPING_TO_ACTION, g_iLanguageId))
			{
				return;
			}

//			sprintf(OutStr, "Jumping To Next Action Point : %s", pJumpStr);
			sprintf(OutStr, "%s : %s", JumpStr, pJumpStr);
			TXTDrawBoxFillRel(GrBuffFor3D, 0, 0, 640, boxheight + 2, boxcolor);
			TXTWriteStringInBox(OutStr, GrBuffFor3D, usefont, 1, 1, 638, fontheight, textcolor, 0, NULL, 1, 0);

			GrFlipPage();
			lJumpTimer = 0;
		}
	}

#if 0
	else
	{
		sprintf(tstr,"LP %0.2f",AIConvertAngleTo180Degree(PlayerPlane->Pitch));
		sprintf(OutStr,"LR %0.2f",AIConvertAngleTo180Degree(PlayerPlane->Roll));

		Draw3DWorld(&Camera1);

		GrDrawStringClipped(GrBuffFor3D, MessageFont, 10, 10, tstr, HUDColor);
		GrDrawStringClipped(GrBuffFor3D, MessageFont, 10, 20, OutStr, HUDColor);

		sprintf(OutStr,"P %0.2f",AIConvertAngleTo180Degree(PlayerPlane->Pitch));
		GrDrawStringClipped(GrBuffFor3D, MessageFont, 412, 150, OutStr, HUDColor);
		sprintf(OutStr,"DP %0.2f",AIConvertAngleTo180Degree(PlayerPlane->DesiredPitch));
		GrDrawStringClipped(GrBuffFor3D, MessageFont, 412, 160, OutStr, HUDColor);
		sprintf(OutStr,"R %0.2f",AIConvertAngleTo180Degree(PlayerPlane->Roll));
		GrDrawStringClipped(GrBuffFor3D, MessageFont, 212, 150, OutStr, HUDColor);
		sprintf(OutStr,"DR %0.2f",AIConvertAngleTo180Degree(PlayerPlane->DesiredRoll));
		GrDrawStringClipped(GrBuffFor3D, MessageFont, 212, 160, OutStr, HUDColor);

		sprintf(OutStr,"DT %ld",dticks);
		GrDrawStringClipped(GrBuffFor3D, MessageFont, 212, 170, OutStr, HUDColor);

		if( ShouldCalculateHudCamera() )
		   GrabHudCameraImage();


		UFCCalcCentralComputerData();
		DrawDynamicCockpit();
		DetectCockpitInteractions();
		DisplayHUD((PlaneParams *)Camera1.AttachedObject);

		GrFlipPage();
		SndPauseAllSounds();
		while(!GetAsyncKeyState('O'));
	}
#endif
	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		RestoreFromJump();
	}
}

//**************************************************************************************
void SetUpForPlayerJump()
{
	MBWayPoints *waypnt;
	int numways;

	if(iInJump)
	{
		iInJump |= 4;
		return;
	}

	iInJump = 1;
	lAfterJump = 0;

	PlayerPlane->Status &= ~AL_DEVICE_DRIVEN;
	PlayerPlane->Status |= AL_AI_DRIVEN;

	PlayerPlane->DoControlSim = CalcF18ControlSurfacesSimple;
	PlayerPlane->DoCASSim = CalcF18CASUpdatesSimple;
	PlayerPlane->DoForces = CalcAeroForcesSimple;
	PlayerPlane->DoPhysics = CalcAeroDynamicsSimple;
	PlayerPlane->dt = 0;
	PlayerPlane->UpdateRate = HIGH_AERO;

	PlayerPlane->MaxPitchRate = 10.0;
	PlayerPlane->MaxRollRate = 90.0;
	PlayerPlane->YawRate = pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iMaxYawRate;

	PlayerPlane->MaxSpeedAccel = pDBAircraftList[PlayerPlane->AI.iPlaneIndex].iAcceleration;	//15
	PlayerPlane->SpeedPercentage = 1.0;

	PlayerPlane->BfLinVel.X = PlayerPlane->V;
	PlayerPlane->BfLinVel.Y = 0;
	PlayerPlane->BfLinVel.Z = 0;

	lJumpTimer = 99999;

	if((PlayerPlane->OnGround) || (((PlayerPlane->HeightAboveGround * WUTOFT) < 10.0f) && (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)))
	{
		PlayerPlane->AI.Behaviorfunc = AIFlyTakeOff;
		PlayerPlane->AI.OrgBehave = AIFlyFormation;
	}
	else
	{
		PlayerPlane->AI.Behaviorfunc = AIFlyFormation;
	}

	waypnt = PlayerPlane->AI.CurrWay;
	numways = PlayerPlane->AI.numwaypts;

	while((!waypnt->bJumpPoint) && (numways > 1))
	{
		waypnt ++;
		numways --;
	}
	pJumpStr = waypnt->sName;
}

//**************************************************************************************
void AICWingmanAAAReport(int planenum, int targetnum)
{
	float bearing, fworkval;
	char positionstr[256];
	char callsign[256];
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}
	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	AIC_Get_Callsign_With_Number(planenum, callsign);

	if((Planes[planenum].AI.lAIVoice >= SPCH_ATK_USAF) && (Planes[planenum].AI.lAIVoice <= SPCH_ATK_RUS))
	{
		tempnum = 0;
	}
	else if((Planes[planenum].AI.lAIVoice >= SPCH_WNG1) && (Planes[planenum].AI.lAIVoice <= SPCH_NAVY5))
	{
		tempnum = rand() % 3;
	}
	else
	{
		return;
	}

	if(tempnum == 0)
	{
		msgnum = AIC_WING_AAA_SPOT_1;
		if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
		{
			msgnum = AIC_WING_AAA_SPOT_RAF_1;
		}
		msgsnd = AICF_WING_AAA_SPOT_1;
	}
	else if(tempnum == 1)
	{
		msgnum = AIC_WING_AAA_SPOT_3;
		msgsnd = AICF_WING_AAA_SPOT_3;
	}
	else
	{
		msgnum = AIC_WING_AAA_SPOT_4;
		msgsnd = AICF_WING_AAA_SPOT_4;
	}


	bearing = (float)targetnum;
	bearing += AIConvertAngleTo180Degree(Planes[planenum].Heading);

	bearing = AICapAngle(bearing);

	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planenum, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, callsign, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
}

//**************************************************************************************
void AICWSOAAAReport(int planenum, int targetnum)
{
	return;

	float bearing, fworkval;
	char positionstr[256];
	char tempstr[1024];
	int tempnum, msgnum, msgsnd;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tempnum = rand() & 1;
	if(targetnum == -888)
	{
		tempnum = 1;
		bearing = (float)targetnum;
		fworkval = bearing;
	}
	else
	{
		bearing = (float)targetnum;
		fworkval = -bearing;
		if(fworkval < 0)
		{
			fworkval += 360;
		}
	}

	if(tempnum == 0)
	{
		tempnum = rand() & 1;
		if(tempnum == 0)
		{
			msgnum = AIC_WSO_AAA_SPOT_1;
			msgsnd = AICF_WSO_AAA_SPOT_1;
		}
		else
		{
			msgnum = AIC_WSO_AAA_SPOT_2;
			msgsnd = AICF_WSO_AAA_SPOT_2;
		}
	}
	else
	{
		if(targetnum == -888)
		{
			msgnum = AIC_WSO_AAA_AROUND;
			msgsnd = AICF_WSO_AAA_AROUND;
		}
		else if(fworkval < 45)
		{
			msgnum = AIC_WSO_AAA_AHEAD;
			msgsnd = AICF_WSO_AAA_AHEAD;
		}
		else if(fworkval < 135)
		{
			msgnum = AIC_WSO_AAA_RIGHT;
			msgsnd = AICF_WSO_AAA_RIGHT;
		}
		else if(fworkval < 225)
		{
			msgnum = AIC_WSO_AAA_BEHIND;
			msgsnd = AICF_WSO_AAA_BEHIND;
		}
		else if(fworkval < 315)
		{
			msgnum = AIC_WSO_AAA_LEFT;
			msgsnd = AICF_WSO_AAA_LEFT;
		}
		else
		{
			msgnum = AIC_WSO_AAA_AHEAD;
			msgsnd = AICF_WSO_AAA_AHEAD;
		}
		if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
		{
			return;
		}

		AICAddAIRadioMsgs(tempstr, 50);
		AIRGenericSpeech(msgsnd, planenum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		return;
	}

	bearing = (float)targetnum;
	bearing += AIConvertAngleTo180Degree(Planes[planenum].Heading);

	bearing = AICapAngle(bearing);

	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	AIGetPPositionStr(positionstr, planenum, fworkval);

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, 0, bearing, 0, 0, 0, 0, 0, 0, 0);
}

//**************************************************************************************
void AICReportWingmanDamage(int planenum, int targetnum)
{
	char tempstr[1024];
	char tempstr2[256];
	char damagestr[1024];
	int placeingroup;
	int tempval;
	int numids;
	int sndids[20];
	int channel;
	PlaneParams *planepnt = &Planes[planenum];
	int msgnum;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!AICAllowThisRadio(planenum, 1))
	{
		return;
	}

	numids = 0;

	channel =  AIRGetChannel(planenum);

//	if(channel != CHANNEL_WINGMEN)
//	{
//		return;
//	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
//	if(!AICCheckVoiceNumInGroup(planenum, Planes[planenum].AI.lAIVoice, placeingroup))
//	{
//		return;
//	}

	numids += AIRProcessSpeechVars(&sndids[numids], 2, planenum);
	damagestr[0] = 0;

	if(planepnt->DamageFlags & (DAMAGE_BIT_RADAR))
	{
		msgnum = AI_GADGET_BENT;
		if(planepnt->AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AI_GADGET_BENT_RUSF;
		}

		if(!LANGGetTransMessage(tempstr2, 1024, msgnum, g_iLanguageId))
		{
			return;
		}
		strcat(damagestr, tempstr2);
		strcat(damagestr, " ");
		sndids[numids] = 708 + Planes[planenum].AI.lAIVoice;
		numids ++;
	}
	if(planepnt->DamageFlags & (DAMAGE_BIT_EMER_HYD))
	{
		msgnum = AI_HYDRAULICS_BENT;
		if(planepnt->AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AI_HYDRAULICS_BENT_RUSF;
		}

		if(!LANGGetTransMessage(tempstr2, 1024, msgnum, g_iLanguageId))
		{
			return;
		}
		strcat(damagestr, tempstr2);
		strcat(damagestr, " ");
		sndids[numids] = 710 + Planes[planenum].AI.lAIVoice;
		numids ++;
	}
	if(planepnt->DamageFlags & (DAMAGE_BIT_FUEL_TANKS))
	{
		if(!LANGGetTransMessage(tempstr2, 1024, AI_BINGO_FUEL, g_iLanguageId))
		{
			return;
		}
		strcat(damagestr, tempstr2);
		strcat(damagestr, " ");
		sndids[numids] = 709 + Planes[planenum].AI.lAIVoice;
		numids ++;
	}
	if((planepnt->DamageFlags & (DAMAGE_BIT_L_ENGINE)) || (planepnt->DamageFlags & (DAMAGE_BIT_R_ENGINE)))
	{
		if(!LANGGetTransMessage(tempstr2, 1024, AI_SINGLE_ENGINE, g_iLanguageId))
		{
			return;
		}
		strcat(damagestr, tempstr2);
		strcat(damagestr, " ");
		sndids[numids] = 707 + Planes[planenum].AI.lAIVoice;
		numids ++;
	}

	tempval = strlen(damagestr);
	if(tempval > 0)
	{
		damagestr[tempval - 1] = 0;
	}


	//  Add message to message array.
//	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
//	{
//		return;
//	}
	AIC_Get_Callsign_With_Number(planenum, tempstr2);

	if(!LANGGetTransMessage(tempstr, 1024, AIC_REPORT_PROBLEM, g_iLanguageId, tempstr2, damagestr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 40);
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AIC_WSO_ReportDamage(int planenum, int targetnum)
{
	return;

	PlaneParams *planepnt;
	int engine1 = 0;
	int engine2 = 0;
	int mainhyd = 0;
	int numbad = 0;
	int general = 0;
	int badsystem[5];
	int cnt;
	int maxrep = 3;
	int delaycnt = 3000;
	int tempval, tempval2;


	planepnt = &Planes[planenum];

	if (planepnt->FlightStatus & PL_OUT_OF_CONTROL)
	{
		AICAddSoundCall(AIC_WSO_Eject, planenum, 2000, 50);
		return;
	}

	for(cnt = 0; cnt < 32; cnt ++)
	{
		if(targetnum & (1<<cnt))
		{
			switch(cnt)
			{
				case DAMAGE_L_BLEED_AIR:
				case DAMAGE_L_ENG_CONT:
				case DAMAGE_L_OIL_PRESS:
				case DAMAGE_L_BURNER:
				case DAMAGE_L_FUEL_PUMP:
				case DAMAGE_L_GEN:
				case DAMAGE_L_ENGINE:
					if(!engine1)
					{
						engine1 = 1;
						if(numbad < maxrep)
						{
							badsystem[numbad] = DAMAGE_L_BLEED_AIR;
						}
						numbad ++;
					}
					break;
				case DAMAGE_R_BLEED_AIR:
				case DAMAGE_R_ENG_CONT:
				case DAMAGE_R_OIL_PRESS:
				case DAMAGE_R_BURNER:
				case DAMAGE_R_FUEL_PUMP:
				case DAMAGE_R_GEN:
				case DAMAGE_R_ENGINE:
					if(!engine2)
					{
						engine2 = 1;
						if(numbad < maxrep)
						{
							badsystem[numbad] = DAMAGE_R_BLEED_AIR;
						}
						numbad ++;
					}
					break;
				case DAMAGE_L_MAIN_HYD:
				case DAMAGE_R_MAIN_HYD:
				case DAMAGE_EMER_HYD:
					if(lNoCheckPlanelMsgs < 0)
					{
						lBombFlags &= ~(WSO_DEEP_DOO_DOO|WSO_ENGINE1_BAD|WSO_ENGINE2_BAD|WSO_HYD_BAD);
					}

					if(lBombFlags & WSO_HYD_BAD)
					{
						mainhyd = 1;
					}

					if(!mainhyd)
					{
						mainhyd = 1;
						if(numbad < maxrep)
						{
							badsystem[numbad] = DAMAGE_L_MAIN_HYD;
							lBombFlags |= WSO_HYD_BAD;
							lNoCheckPlanelMsgs = 10000;
						}
						numbad ++;
					}
					break;
				case DAMAGE_LO_ENGINE:
				case DAMAGE_RO_ENGINE:
					break;
				case DAMAGE_FLT_CONT:
				case DAMAGE_ADC:
				case DAMAGE_GUN:
				case DAMAGE_HUD:
					if(!(general))
					{
						general = 1;
						if(numbad < maxrep)
						{
							badsystem[numbad] = cnt;
						}
						numbad ++;
					}
					break;
				default:
					if(numbad < maxrep)
					{
						badsystem[numbad] = cnt;
					}
					numbad ++;
					break;
			}
		}
	}

	if(numbad >= maxrep)
	{
		if(lNoCheckPlanelMsgs < 0)
		{
			lNoCheckPlanelMsgs = 10000;
			lBombFlags &= ~(WSO_DEEP_DOO_DOO|WSO_ENGINE1_BAD|WSO_ENGINE2_BAD|WSO_HYD_BAD);
			AICAddSoundCall(AIC_WSO_CheckPanel, planenum, 6000, 50);  // This will happen after WSO_Trouble call below due to 6000 being greater than 3000.
		}

		if(!(lBombFlags  & WSO_DEEP_DOO_DOO))
		{
			lBombFlags |= WSO_DEEP_DOO_DOO;
			AICAddSoundCall(AIC_WSO_Trouble, planenum, 3000, 50, numbad);
		}
	}
	else if(numbad)
	{
		for(cnt = 0; cnt < numbad; cnt++)
		{
			switch(badsystem[cnt])
			{
				case WARN_RADAR:
					AICAddSoundCall(AIC_WSO_RadarCheck, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_L_BLEED_AIR:
				case WARN_R_BLEED_AIR:
					tempval = engine1 + engine2;
					if(tempval)
					{
						tempval2 = 1;
						tempval2 |= (engine1) ? 2 : 0;
						tempval2 |= (engine2) ? 4 : 0;
						AICAddSoundCall(AIC_WSO_EngineCheck, planenum, delaycnt, 50, tempval2);
						delaycnt += 3000 * tempval;
						engine1 = engine2 = 0;
					}
					break;
				case WARN_TEWS:
					AICAddSoundCall(AIC_WSO_TEWSCheck, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case DAMAGE_FUEL_TANKS:
					AICAddSoundCall(AIC_WSO_FuelLeak, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_MAIN_HYD:
					AICAddSoundCall(AIC_WSO_MainHyd, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_CEN_COMP:
					AICAddSoundCall(AIC_WSO_CentComp, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_TARGET_IR:
					AICAddSoundCall(AIC_WSO_TargetPod, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_NAV_FLIR:
					AICAddSoundCall(AIC_WSO_NavPod, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_AUTOPILOT:
					AICAddSoundCall(AIC_WSO_AutoPilot, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_PACS:
					AICAddSoundCall(AIC_WSO_PACS, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				case WARN_OXYGEN:
					AICAddSoundCall(AIC_WSO_Pressure, planenum, delaycnt, 50, 1);
					delaycnt += 3000;
					break;
				default:
					if(lNoCheckPlanelMsgs < 0)
					{
						lNoCheckPlanelMsgs = 10000;
						lBombFlags &= ~(WSO_DEEP_DOO_DOO|WSO_ENGINE1_BAD|WSO_ENGINE2_BAD|WSO_HYD_BAD);
						AICAddSoundCall(AIC_WSO_CheckPanel, planenum, delaycnt, 50, 1);
						delaycnt += 3000;
					}
					break;
			}
		}
	}
}

//**************************************************************************************
void AIC_WSO_Eject(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(Planes[planenum].FlightStatus & (PL_STATUS_CRASHED|PL_PLANE_BLOWN_UP))
	{
		return;
	}

	msgnum = AIC_WSO_EJECT_EJECT;
	msgsnd = AICF_WSO_EJECT_EJECT;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_Trouble(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int tempval, msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(targetnum < 4)
	{
		tempval = rand() & 1;
	}
	else
	{
		tempval = rand() % 3;
	}

	if(tempval == 0)
	{
		msgnum = AIC_WSO_HURT_BAD;
		msgsnd = AICF_WSO_HURT_BAD;
	}
	else if(tempval == 1)
	{
		msgnum = AIC_WSO_WHAT_WAS_THAT;
		msgsnd = AICF_WSO_WHAT_WAS_THAT;
	}
	else
	{
		msgnum = AIC_WSO_DEEP_DOO;
		msgsnd = AICF_WSO_DEEP_DOO;
	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_CheckPanel(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_CHECK_PANNEL;
	msgsnd = AICF_WSO_CHECK_PANNEL;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_FuelLeak(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_FUEL_LEAK;
	msgsnd = AICF_WSO_FUEL_LEAK;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_On_Fire(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_ON_FIRE;
	msgsnd = AICF_WSO_ON_FIRE;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_MainHyd(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_HYDRAULIC_DAMAGE;
	msgsnd = AICF_WSO_HYDRAULIC_DAMAGE;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_CentComp(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_COMPUTER_GONE;
	msgsnd = AICF_WSO_COMPUTER_GONE;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_TargetPod(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_TARGETING_DEAD;
	msgsnd = AICF_WSO_TARGETING_DEAD;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_NavPod(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_NAV_HIT;
	msgsnd = AICF_WSO_NAV_HIT;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_AutoPilot(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_AUTOPILOT_SHOT;
	msgsnd = AICF_WSO_AUTOPILOT_SHOT;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_PACS(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_PACS_OUT;
	msgsnd = AICF_WSO_PACS_OUT;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_Pressure(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int tempval, msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	tempval = rand() & 1;

	if(planepnt->Altitude < 10000)
	{
		tempval = 1;
	}

	if(tempval)
	{
		msgnum = AIC_WSO_CABIN_PRESSURE;
		msgsnd = AICF_WSO_CABIN_PRESSURE;
	}
	else
	{
		msgnum = AIC_WSO_LOST_PRESSUREIZATION;
		msgsnd = AICF_WSO_LOST_PRESSUREIZATION;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_Laser_On(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_LASER_ON;
	msgsnd = AICF_WSO_LASER_ON;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_Laser_Off(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_LASER_OFF;
	msgsnd = AICF_WSO_LASER_OFF;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_Loosing_Lase(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_LOOSING_LASE;
	msgsnd = AICF_WSO_LOOSING_LASE;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_Pod_Masked(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_POD_MASKED;
	msgsnd = AICF_WSO_POD_MASKED;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_WSO_Music_On(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_MUSIC_ON;
	msgsnd = AICF_WSO_MUSIC_ON;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}


//**************************************************************************************
void AIC_WSO_Music_Off(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	msgnum = AIC_WSO_MUSIC_OFF;
	msgsnd = AICF_WSO_MUSIC_OFF;

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICWAttackMyTarget()
{
	PlaneParams *planepnt;
  	BasicInstance *grndtarget = NULL;
	MovingVehicleParams *vehiclepnt = NULL;
	int grndtype = -1;
	void *target;
	int cnt;
	int movetop;
	PlaneParams *leadplane;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	if(iAICommFrom < 0)
	{
		leadplane = PlayerPlane;
	}
	else
	{
		leadplane = &Planes[iAICommFrom];
	}


	if(leadplane->AI.wingman < 0)
	{
		return;
	}
	else
	{
		planepnt = &Planes[leadplane->AI.wingman];

		if((!AIInPlayerGroup(planepnt)) && (iAICommFrom < 0))
		{
			return;
		}
		else
		{
			if(!(((!(planepnt->AI.iAIFlags2 & AILANDING)) || (planepnt->AI.lTimer2 > 0)) && (!planepnt->OnGround)))
			{
				return;
			}
		}
	}

	if(leadplane->AADesignate)
	{
		if(leadplane->AADesignate->AI.iSide == planepnt->AI.iSide)
		{
			AICAddSoundCall(AIC_Basic_Neg, leadplane->AI.wingman, 1000, 50, 0);
			return;
		}
//		else if(planepnt->AI.Behaviorfunc == AIDoJink)
		else if(planepnt->AI.iAIFlags2 & AIDEFENSIVE)
		{
			AICAddSoundCall(AIC_Help_Me_Neg_Response, leadplane->AI.wingman, 1000, 50, 0);
			return;
		}
		else
		{
			planepnt->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
			planepnt->AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);

			if((planepnt->AI.AirTarget != leadplane->AADesignate) && (!(leadplane->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				planepnt->AI.lCombatTimer = -1;
				planepnt->AI.CombatBehavior = NULL;
			}

			if( GetCurrentAARadarMode() == AA_TWS_MODE )
			{
				WingNumDTWSTargets = NumDTWSTargets;
				movetop = -1;
				for(cnt = 0; cnt < 8; cnt ++)
				{
					WingDTWSTargets[cnt] = DTWSTargets[cnt];
					if(CurTargetInfo.CurTargetId == DTWSTargets[cnt])
					{
						movetop = cnt;
					}
				}

				if(movetop != -1)
				{
					for(cnt = movetop; cnt > 0; cnt --)
					{
						WingDTWSTargets[cnt] = WingDTWSTargets[cnt - 1];
					}
					WingDTWSTargets[0] = CurTargetInfo.CurTargetId;
				}
			}
			else
			{
				GeneralSetNewAirTarget(planepnt, leadplane->AADesignate);
			}

			planepnt->AI.iAIFlags2 |= AIKEEPTARGET;
			if(planepnt->AI.OrgBehave == NULL)
			{
				planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
//				planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
				planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
			}
			else
			{
//				planepnt->AI.Behaviorfunc = AIHeadAtAirTarget;
				planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
			}
			AICAddSoundCall(AIC_Attack_Target_Response, leadplane->AI.wingman, 1000, 50, 1);
		}
	}
	else if(PlayerPlane->AGDesignate.X >= 0)
	{
		target = (BasicInstance *)AIGroundTargetNear(leadplane->AGDesignate, 1500, &grndtype);

		if(!target)
		{
			AICAddSoundCall(AIC_Attack_Target_Response, leadplane->AI.wingman, 1000, 50, 0);
			return;
		}
		if(grndtype == GROUNDOBJECT)
		{
			grndtarget = (BasicInstance *)target;
			if(GDConvertGrndSide(grndtarget) == planepnt->AI.iSide)
			{
				AICAddSoundCall(AIC_Basic_Neg, leadplane->AI.wingman, 1000, 50, 0);
				return;
			}
		}
		else if(grndtype == MOVINGVEHICLE)
		{
			vehiclepnt = (MovingVehicleParams *)target;
			if(vehiclepnt->iSide == planepnt->AI.iSide)
			{
				AICAddSoundCall(AIC_Basic_Neg, leadplane->AI.wingman, 1000, 50, 0);
				return;
			}
		}
//		else if(grndtarget == NULL)
//		{
//			return;
//		}

		if(planepnt->AI.iAIFlags1 & AIENGAGED)
		{
			planepnt->AI.iAIFlags1 &= ~(AIENGAGED|AICANENGAGE);
		}

#if 0
		if(planepnt->AI.OrgBehave == NULL)
		{
			planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
		}
		else
		{
			planepnt->AI.OrgBehave = AIFlyToPopPoint;
		}
#else
		planepnt->AI.Behaviorfunc = AIFlyToPopPoint;
		planepnt->AI.OrgBehave = NULL;
#endif

		planepnt->AI.CurrWay = leadplane->AI.CurrWay;
		planepnt->AI.numwaypts = leadplane->AI.numwaypts;

		planepnt->AI.pGroundTarget = target;
		planepnt->AI.lGroundTargetFlag = grndtype;
		AIDetermineBestBomb(planepnt, planepnt->AI.pGroundTarget, planepnt->AI.lGroundTargetFlag);

		if(planepnt->AI.cActiveWeaponStation == -1)
		{
			AICAddSoundCall(AIC_Basic_Neg, leadplane->AI.wingman, 1000, 50, 1);
			return;
		}

		planepnt->AI.iVar1 = 1;

		if(grndtarget)
		{
			planepnt->AI.WayPosition = grndtarget->Position;
			planepnt->AI.WayPosition.Y = planepnt->WorldPosition.Y;
			planepnt->AI.TargetPos = grndtarget->Position;
		}
		else if(vehiclepnt)
		{
			planepnt->AI.WayPosition = vehiclepnt->WorldPosition;
			planepnt->AI.WayPosition.Y = planepnt->WorldPosition.Y;
			planepnt->AI.TargetPos = vehiclepnt->WorldPosition;
		}
		else
		{
			planepnt->AI.WayPosition = leadplane->AGDesignate;
			planepnt->AI.WayPosition.Y = planepnt->WorldPosition.Y;
			planepnt->AI.TargetPos = leadplane->AGDesignate;
		}

		AICAddSoundCall(AIC_Basic_Ack, leadplane->AI.wingman, 1000, 50);
	}
	else
	{
		AICAddSoundCall(AIC_Attack_Target_Response, leadplane->AI.wingman, 1000, 50, 0);
	}
}

//**************************************************************************************
void AICWHelpMe()
{
	PlaneParams *planepnt;
  	BasicInstance *grndtarget = NULL;
	PlaneParams *leadplane;

#ifdef NO_AI_PLAYER
	if((!(PlayerPlane->Status & AL_DEVICE_DRIVEN)) && (iAICommFrom < 0))
		return;
#endif

	if(iAICommFrom < 0)
	{
		leadplane = PlayerPlane;
	}
	else
	{
		leadplane = &Planes[iAICommFrom];
	}

	if(leadplane->AI.wingman < 0)
	{
		return;
	}
	else
	{
		planepnt = &Planes[leadplane->AI.wingman];
		if((!AIInPlayerGroup(planepnt)) && (iAICommFrom < 0))
		{
			return;
		}
		if(!(((!(planepnt->AI.iAIFlags2 & AILANDING)) || (planepnt->AI.lTimer2 > 0)) && (!planepnt->OnGround)))
		{
			return;
		}
	}

	if(leadplane->AI.AirThreat)
	{
//		if(planepnt->AI.Behaviorfunc == AIDoJink)
		if(planepnt->AI.iAIFlags2 & AIDEFENSIVE)
		{
			AICAddSoundCall(AIC_Help_Me_Neg_Response, leadplane->AI.wingman, 1000, 50, 0);
			return;
		}
		else
		{
			planepnt->AI.iAIFlags1 |= (AIENGAGED|AICANENGAGE);
			planepnt->AI.iAIFlags1 &= ~(AI_ASK_PLAYER_ENGAGE);

			if((planepnt->AI.AirTarget != leadplane->AI.AirThreat) && (!(planepnt->AI.iAICombatFlags1 & AI_FINISH_BEHAVIOR)))
			{
				planepnt->AI.lCombatTimer = -1;
				planepnt->AI.CombatBehavior = NULL;
			}

			GeneralSetNewAirTarget(planepnt, leadplane->AI.AirThreat);
			planepnt->AI.iAIFlags2 |= AIKEEPTARGET;
			AICAddSoundCall(AIC_Help_Me_Response, leadplane->AI.wingman, 1000, 50, 1);
			if(planepnt->AI.Behaviorfunc != AILooseDeuce)
			{
				if(planepnt->AI.OrgBehave == NULL)
				{
					planepnt->AI.OrgBehave = planepnt->AI.Behaviorfunc;
					planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;

				}
				else
				{
					planepnt->AI.Behaviorfunc = AILooseDeuce;  //  AIGaggle;
				}

				//  Temp for now, make HeadAtAir not time Human plane groups;
				planepnt->AI.lTimer1 = 660000 - (planepnt->AI.iSkill * 100000);
			}

		}
	}
	else
	{
	 	AICAddSoundCall(AIC_Help_Me_Neg_Response, leadplane->AI.wingman, 1000, 50, 1);
	}
}

//**************************************************************************************
void *AIGroundTargetNear(FPoint centerpos, int distft, int *grndtype)
{
  	BasicInstance *checkobject;
	float foundrange;
  	void *foundobject = NULL;
	float dx, dz, tdist;
	int pass;
	MovingVehicleParams *vehiclepnt;

	*grndtype = -1;

	foundrange = (distft * FTTOWU);

	BasicInstance ***checklist = &AllTargetTypes[0];
	pass = NumTargetTypes;

	while(pass--)
	{
		checkobject = **checklist++;

		while(checkobject)
		{
			if(InstanceIsBombable(checkobject)) // && (GDConvertGrndSide(checkobject) != searchside))
			{
				dx = (checkobject->Position.X) - centerpos.X;
				dz = (checkobject->Position.Z) - centerpos.Z;
				if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
				{
					tdist = QuickDistance(dx, dz);
					if(tdist < foundrange)
					{
						foundobject = checkobject;
						foundrange = tdist;
						*grndtype = GROUNDOBJECT;
					}
				}
			}
			checkobject = checkobject->NextRelatedInstance;
		}
	}

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if((vehiclepnt->Status & (VL_ACTIVE)) && (!(vehiclepnt->Status & (VL_DESTROYED|VL_INVISIBLE))) && (PlayerPlane->AI.iSide != vehiclepnt->iSide))
		{
			dx = (vehiclepnt->WorldPosition.X) - centerpos.X;
			dz = (vehiclepnt->WorldPosition.Z) - centerpos.Z;
			if((fabs(dx) < foundrange) && (fabs(dz) < foundrange))
			{
				tdist = QuickDistance(dx, dz);
				if(tdist < foundrange)
				{
					foundobject = vehiclepnt;
					foundrange = tdist;
					*grndtype = MOVINGVEHICLE;
				}
			}
		}
	}

	return(foundobject);
}

//**************************************************************************************
void AICFSortBandits()
{
	PlaneParams *planepnt;
	int cnt, flag;

	if(iAICommFrom >= 0)
	{
		planepnt = &Planes[iAICommFrom];
	}
	else
	{
		planepnt = PlayerPlane;
	}

	int firstvalid, lastvalid;

	NetConvertFirstAndLastValid(0, 7, &firstvalid, &lastvalid);

	flag = 0;
	for(cnt = 0; cnt < 8; cnt ++)
	{
		if((cnt >= firstvalid) && (cnt <= lastvalid))
		{
			flag |= 1<<cnt;
		}
	}

	if(planepnt->AI.wingman >= 0)
	{
		AICAddSoundCall(AICDoSortDelay, planepnt->AI.wingman, 1000, 50, flag);
	}
	else if(planepnt->AI.nextpair >= 0)
	{
		AICAddSoundCall(AICDoSortDelay, planepnt->AI.nextpair, 1000, 50, flag);
	}
	return;
}

//**************************************************************************************
void AIC_Basic_Ack(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;
	int tempval;
	char tempstr2[256];
	int placeingroup;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if((targetnum >= 0) && (targetnum <= 2))
	{
		tempval = targetnum;
	}
//	else if(placeingroup != 2)
//	{
//		tempval = rand() & 1;
//	}
	else
	{
		tempval = rand() & 3;
	}

	if(tempval == 0)
	{
		msgnum = AIC_BASIC_ACK;
		msgsnd = AICF_BASIC_ACK;
	}
	else if(tempval == 1)
	{
		msgnum = AIC_BASIC_ACK_2;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_BASIC_ACK_2_RUSF;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_ATK_RAF)
		{
			msgnum = AIC_BASIC_ACK_2_RAF;
		}
		else if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RAF)
		{
			msgnum = AIC_BASIC_ACK_2_RAFF;
		}
		msgsnd = AICF_BASIC_ACK_2;
	}
	else if(tempval == 2)
	{
		if(placeingroup != 2)
		{
			msgnum = AIC_COPY_ACK;
			msgsnd = AICF_COPY;
		}
		else
		{
			msgnum = AIC_COPY_ACK;
			msgsnd = AICF_TWO_COPY_ACK;
		}
	}
	else
	{
		msgnum = AIC_ROGER;
		msgsnd = AICF_ROGER;
	}


	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_Basic_Neg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;
	int tempval;
	char tempstr2[256];
	int placeingroup;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if((targetnum >= 0) && (targetnum <= 1))
	{
		tempval = targetnum;
	}
	else
	{
		tempval = rand() & 1;
	}

	if(tempval == 0)
	{
		msgnum = AIC_BASIC_NEG_1;
		msgsnd = AICF_BASIC_NEG_1;
	}
	else
	{
		msgnum = AIC_BASIC_NEG_2;
		if(Planes[planenum].AI.lAIVoice == SPCH_FTR_RUS)
		{
			msgnum = AIC_BASIC_NEG_2_RUSF;
		}

		msgsnd = AICF_BASIC_NEG_2;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_Attack_Target_Response(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;
	int tempval;
	char tempstr2[256];
	int placeingroup;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if((targetnum >= 0) && (targetnum <= 1))
	{
		tempval = targetnum;
	}
	else
	{
		tempval = rand() & 1;
	}

	if(tempval == 0)
	{
		msgnum = AIC_ATTACK_MY_TARG_NEG_2;
		msgsnd = AICF_ATTACK_MY_TARG_NEG_2;
	}
	else
	{
		msgnum = AIC_ATTACK_MY_TARG_POS_2;
		msgsnd = AICF_ATTACK_MY_TARG_POS_2;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_Help_Me_Response(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;
	int tempval;
	char tempstr2[256];
	int placeingroup;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	tempval = targetnum;

	if(tempval == 0)
	{
		msgnum = AIC_HELP_ME_POS_1_A;
		msgsnd = AICF_HELP_ME_POS_1_A;
	}
	else
	{
		msgnum = AIC_HELP_ME_POS_2_A;
		msgsnd = AICF_HELP_ME_POS_2_A;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AIC_Help_Me_Neg_Response(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	PlaneParams *planepnt = &Planes[planenum];
	int platformok = 1;
	int tempval;
	char tempstr2[256];
	int placeingroup;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	tempval = targetnum;

	if(tempval != 0)
	{
		msgnum = AIC_HELP_ME_NEG_1_A;
		msgsnd = AICF_HELP_ME_NEG_1_A;
	}
	else
	{
		msgnum = AIC_HELP_ME_NEG_2_A;
		msgsnd = AICF_HELP_ME_NEG_2_A;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AIRGenericSpeech(msgsnd, planenum);

	AICAddAIRadioMsgs(tempstr, 40);
	return;
}

//**************************************************************************************
void AICRequestClosestTanker()
{
	int awacsnum;

	awacsnum = AIGetClosestAWACS(PlayerPlane);

	AICAddSoundCall(AIC_AWACS_Acknowledged, PlayerPlane - Planes, 1000, 50, awacsnum);
	AICAddSoundCall(AIC_AWACS_CheckForTanker, PlayerPlane - Planes, 10000, 50);
}

//**************************************************************************************
void AIC_AWACS_CheckForTanker(int planenum, int targetnum)
{
	PlaneParams *planepnt = &Planes[planenum];
	PlaneParams *tanker;
	float dx, dy, dz, range, bearing;
	int awacsnum;
	char bearstr[128];
	char rangestr[128];
	float tbearing;
	float rangenm;
	char callsign[128];
	char awacscallsign[128];
	char tankercallsign[128];
	char tempstr[1024];
	int bearing360;
	int alt;
	char altstr[128];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	awacsnum = AIGetClosestAWACS(planepnt);

	if(awacsnum < 0)
		return;

	AICGetCallSign(awacsnum, awacscallsign);

	AIC_Get_Callsign_With_Number(planenum, callsign);

	tanker = AICheckTankerCloseBy(PlayerPlane, -1);

	if(!tanker)
	{
		AICNoCover(planenum, awacsnum);
		return;
	}

	AICGetCallSign(tanker - Planes, tankercallsign);

	bearing = AIComputeHeadingToPoint(planepnt, tanker->WorldPosition, &range, &dx ,&dy, &dz, 0);

	tbearing = -bearing;
	if(tbearing < 0)
	{
		bearing360 = tbearing + 360;
	}
	else
	{
		bearing360 = tbearing;
	}
	sprintf(bearstr, "%03d", bearing360);

	rangenm = (range * WUTONM);
	if(rangenm < 5)
	{
		rangenm = 5;
	}

	AICGetRangeText(rangestr, rangenm);

	alt = (tanker->WorldPosition.Y * WUTOFT) / 1000.0f;
	sprintf(tempstr, "%d", alt);

	if(!LANGGetTransMessage(altstr, 1024, AIC_THOUSAND, g_iLanguageId, tempstr))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_TANKER_BEARING, g_iLanguageId, callsign, bearstr, rangestr, altstr))
	{
		return;
	}

	AIC_AWACS_Tanker_Request_Speech(planenum, awacsnum, tanker - Planes, bearing, rangenm);

	AICAddAIRadioMsgs(tempstr, 40);
}

//**************************************************************************************
void AIC_AWACS_Tanker_Request_Speech(int planenum, int awacsnum, int tankernum, float bearing, float rangenm)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_TANKER_BEARING, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, awacsnum, planenum, bearing, 0, 0, rangenm, Planes[tankernum].WorldPosition.Y * WUTOFT);
		}
		else if(spchvar == 2)
		{
			if(numval == 4)
			{
				sndids[numids] = AIRGetCallSignID(tankernum) + Planes[awacsnum].AI.lAIVoice;
				if(AIRCheckCallSignSpchExists(sndids[numids]))
				{
					numids ++;
				}
			}
		}
		else
		{
			sndids[numids] = numval + Planes[awacsnum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(awacsnum, numids, sndids);
}

//**************************************************************************************
void AICDoSortDelay(int planenum, int targetnum)
{
	iAISortNumber = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS);

	if((( (GetCurrentPrimaryTarget() != -1)   ) ) && ((targetnum < 0) || (targetnum & (1<<iAISortNumber))))
	//if((((RadarInfo.CurMode != SEARCH_MODE) && (RadarInfo.CurMode != VCTR_MODE) && (RadarInfo.CurMode != VS_MODE)) || (g_Settings.gp.nType == GP_TYPE_CASUAL)) && ((targetnum < 0) || (targetnum & (1<<iAISortNumber))))
	{
		AICDoSort(iAISortNumber, targetnum);
	}
}

//**************************************************************************************
void AICDoSort(int placeingroup, int targetnum)
{
	int cnt;
	int targetcnt = -1;
	int planenum;
	int currid;
	double centerx, centerz;
	float centerdist[AA_MAX_DET_PLANES * 2];
	float radardist[AA_MAX_DET_PLANES * 2];
	int headingid[AA_MAX_DET_PLANES * 2];
	float heading[AA_MAX_DET_PLANES * 2];
	float closest;
	int closestid = -1;
	float furthest = 0.0;
	int furthestid = -1;
	float dx, dz, tdist, theading;
	int headingcnt[8];
	float furthestheading[8];
	int furthestheadingid[8];
	int midnumber = 1;
	int middlecnt = 0;
	int cnt2;
	int maxnum, checkloop;

	if(placeingroup < 0)
	{
		AIC_WSO_Report_Contacts(PlayerPlane - Planes);
		return;
	}

	planenum = (PlayerPlane - Planes) + placeingroup;

	iAISortNumber = 0;

	centerx = centerz = 0;

	if(!(Planes[planenum].AI.iAIFlags1 & AIPLAYERGROUP))
	{
		return;
	}

	if(!(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			currid = CurFramePlanes.Planes[cnt].PlaneId;
			if(Planes[planenum].AI.AirTarget)
			{
				if((Planes[planenum].AI.AirTarget - Planes) == currid)
				{
					targetcnt = cnt;
				}
			}
			centerx += Planes[currid].WorldPosition.X;
			centerz += Planes[currid].WorldPosition.Z;
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
				if(Planes[planenum].AI.AirTarget)
				{
					if((Planes[planenum].AI.AirTarget - Planes) == currid)
					{
						targetcnt = cnt + AA_MAX_DET_PLANES;
					}
				}
				centerx += Planes[currid].WorldPosition.X;
				centerz += Planes[currid].WorldPosition.Z;
			}
		}
	}

	if(targetcnt == -1)
	{
		if(Planes[planenum].AI.wingman >= 0)
		{
			AICDoSortDelay(Planes[planenum].AI.wingman, targetnum);
		}
		else if(Planes[planenum].AI.nextpair >= 0)
		{
			AICDoSortDelay(Planes[planenum].AI.nextpair, targetnum);
		}
		else
		{
			planenum = Planes[planenum].AI.winglead;
			if(planenum >= 0)
			{
				if(Planes[planenum].AI.nextpair >= 0)
				{
					AICDoSortDelay(Planes[planenum].AI.nextpair, targetnum);
				}
			}
		}
		return;
	}


	if( GetCurrentAARadarMode() == AA_TWS_MODE )
	{
		centerx /= (CurFramePlanes.NumPlanes + AgePlanes[0].NumPlanes);
		centerz /= (CurFramePlanes.NumPlanes + AgePlanes[0].NumPlanes);
	}
	else
	{
		centerx /= CurFramePlanes.NumPlanes;
		centerz /= CurFramePlanes.NumPlanes;
	}

	for(cnt = 0; cnt < 8; cnt ++)
	{
		headingcnt[cnt] = 0;
		furthestheading[cnt] = -1;
		furthestheadingid[cnt] = -1;
	}

	if( GetCurrentAARadarMode() == AA_TWS_MODE )
	{
		checkloop = 2;
	}
	else
	{
		checkloop = 1;
	}

	for(cnt2 = 0; cnt2 < checkloop; cnt2 ++)
	{
		if(cnt2)
		{
			maxnum = AgePlanes[0].NumPlanes;
		}
		else
		{
			maxnum = CurFramePlanes.NumPlanes;
		}

		for(cnt = 0; cnt < maxnum; cnt ++)
		{
			if(cnt2)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
			}
			else
			{
				currid = CurFramePlanes.Planes[cnt].PlaneId;
			}
			dx = Planes[currid].WorldPosition.X - centerx;
			dz = Planes[currid].WorldPosition.Z - centerz;
			tdist = QuickDistance(dx, dz);
			theading = -atan2(-dx, -dz) * 57.2958;

			theading = AICapAngle(theading, 0, 360);

			if(cnt2)
			{
				centerdist[cnt + AA_MAX_DET_PLANES] = tdist;
				heading[cnt + AA_MAX_DET_PLANES] = theading;
				headingid[cnt + AA_MAX_DET_PLANES] = (theading + 22.5) / 45;
				if(headingid[cnt + AA_MAX_DET_PLANES] > 7)
				{
					headingid[cnt + AA_MAX_DET_PLANES] = 0;
				}

				if(furthestheading[headingid[cnt + AA_MAX_DET_PLANES]] < tdist)
				{
					furthestheading[headingid[cnt + AA_MAX_DET_PLANES]] = tdist;
					furthestheadingid[headingid[cnt + AA_MAX_DET_PLANES]] = cnt;
				}

				headingcnt[headingid[cnt + AA_MAX_DET_PLANES]] = headingcnt[headingid[cnt + AA_MAX_DET_PLANES]] + 1;
			}
			else
			{
				centerdist[cnt] = tdist;
				heading[cnt] = theading;
				headingid[cnt] = (theading + 22.5) / 45;
				if(headingid[cnt] > 7)
				{
					headingid[cnt] = 0;
				}

				if(furthestheading[headingid[cnt]] < tdist)
				{
					furthestheading[headingid[cnt]] = tdist;
					furthestheadingid[headingid[cnt]] = cnt;
				}

				headingcnt[headingid[cnt]] = headingcnt[headingid[cnt]] + 1;
			}

			dx = Planes[currid].WorldPosition.X - PlayerPlane->WorldPosition.X;
			dz = Planes[currid].WorldPosition.Z - PlayerPlane->WorldPosition.Z;

			tdist = QuickDistance(dx, dz);

			if((closestid == -1) || (closest > tdist))
			{
				closest = tdist;
				if(cnt2)
				{
					closestid = cnt + AA_MAX_DET_PLANES;
				}
				else
				{
					closestid = cnt;
				}
			}
			if((furthestid == -1) || (furthest < tdist))
			{
				furthest = tdist;
				if(cnt2)
				{
					furthestid = cnt + AA_MAX_DET_PLANES;
				}
				else
				{
					furthestid = cnt;
				}
			}
			if(cnt2)
			{
				radardist[cnt + AA_MAX_DET_PLANES] = tdist;
			}
			else
			{
				radardist[cnt] = tdist;
			}
		}
	}

	if(closestid == targetcnt)
	{
		AICDoSortMsg(planenum, 10, midnumber);
	}
	else if(furthestid == targetcnt)
	{
		AICDoSortMsg(planenum, 12, midnumber);
	}
	else if((headingcnt[headingid[targetcnt]] == 1) || (furthestheadingid[headingid[targetcnt]] == targetcnt))
	{
		AICDoSortMsg(planenum, headingid[targetcnt], midnumber);
	}
	else
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			if((headingcnt[headingid[targetcnt]] != 1) && (furthestheadingid[headingid[cnt]] != cnt) && (cnt != targetcnt))
			{
				middlecnt++;
				if(radardist[cnt] < radardist[targetcnt])
				{
					midnumber ++;
				}
			}
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				if((headingcnt[headingid[targetcnt]] != 1) && (furthestheadingid[headingid[cnt + AA_MAX_DET_PLANES]] != (cnt + AA_MAX_DET_PLANES)) && ((cnt + AA_MAX_DET_PLANES) != targetcnt))
				{
					middlecnt++;
					if(radardist[cnt + AA_MAX_DET_PLANES] < radardist[targetcnt])
					{
						midnumber ++;
					}
				}
			}
		}

		if(!middlecnt)
		{
			midnumber = 0;
		}
		AICDoSortMsg(planenum, 11, midnumber);
	}

	if(Planes[planenum].AI.wingman >= 0)
	{
		AICAddSoundCall(AICDoSortDelay, Planes[planenum].AI.wingman, 4000, 50, targetnum);
	}
	else if(Planes[planenum].AI.nextpair >= 0)
	{
		AICAddSoundCall(AICDoSortDelay, Planes[planenum].AI.nextpair, 4000, 50, targetnum);
	}
	else
	{
		planenum = Planes[planenum].AI.winglead;
		if(planenum >= 0)
		{
			if(Planes[planenum].AI.nextpair >= 0)
			{
				AICAddSoundCall(AICDoSortDelay, Planes[planenum].AI.nextpair, 4000, 50, targetnum);
			}
		}
	}
}

//**************************************************************************************
void AICDoSortMsg(int planenum, int sortnum, int number)
{
	char tempstr[1024];
	char tempstr2[256];
	char targettxt[256];
	char headingtxt[256];
	char numberstr[256];
	int placeingroup;
	PlaneParams *targetplane = Planes[planenum].AI.AirTarget;
	int targettype;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (Planes[planenum].AI.iAIFlags1 & AIFLIGHTNUMS) + 1;
	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}
	if(pDBAircraftList[targetplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		if(!LANGGetTransMessage(targettxt, 256, AIC_HELO, g_iLanguageId))
		{
			return;
		}
		targettype = 2;
	}
	else if(pDBAircraftList[targetplane->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_BOMBER)
	{
		if(!LANGGetTransMessage(targettxt, 256, AIC_STRIKER, g_iLanguageId))
		{
			return;
		}
		targettype = 1;
	}
	else
	{
		if(!LANGGetTransMessage(targettxt, 256, AIC_BANDIT, g_iLanguageId))
		{
			return;
		}
		targettype = 0;
	}

	if(sortnum == 10)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_LEAD_POS, g_iLanguageId))
		{
			return;
		}
	}
	else if(sortnum == 11)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_MIDDLE, g_iLanguageId))
		{
			return;
		}
	}
	else if(sortnum == 12)
	{
		if(!LANGGetTransMessage(headingtxt, 256, AIC_TRAIL_POS, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		AICGetTextHeadingGeneral(headingtxt, (float)sortnum * -45);
	}

	if((sortnum == 11) && (number < 10) && (number > 0))
	{
		if(!LANGGetTransMessage(numberstr, 256, AIC_ZERO + number, g_iLanguageId))
		{
			return;
		}

		if(!LANGGetTransMessage(tempstr, 256, AIC_SORTED_MIDDLE, g_iLanguageId, tempstr2, numberstr, targettxt))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 256, AIC_SORTED, g_iLanguageId, tempstr2, headingtxt, targettxt))
		{
			return;
		}
	}
	AICAddAIRadioMsgs(tempstr, 40);

	AIRDoSortSpeech(planenum, sortnum, number, targettype);
}

//**************************************************************************************
void AIRDoSortSpeech(int planenum, int sortnum, int number, int type)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	int midsing = 0;

	numids = 0;

	if((sortnum == 11) && (number < 10) && (number > 0))
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_SORTED_MIDDLE, g_iLanguageId))
		{
			return;
		}
	}
	else
	{
		if(!LANGGetTransMessage(tempstr, 1024, AICF_SORTED, g_iLanguageId))
		{
			return;
		}
		midsing = 1;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				switch(sortnum)
				{
					case 10:
						switch(type)
						{
							case 2:
								sndids[numids] = 411 + Planes[planenum].AI.lAIVoice;  //  "Lead Helo"  581 in F-15
								numids ++;
								break;
							case 1:
								sndids[numids] = 415 + Planes[planenum].AI.lAIVoice;  //  "Lead Striker" 585 in F-15
								numids ++;
								break;
							default:
								sndids[numids] = 407 + Planes[planenum].AI.lAIVoice;  //  "Lead Bandit"  577 in F-15
								numids ++;
								break;
						}
						done = 1;
						break;
					case 11:
						if(midsing)
						{
							switch(type)
							{
								case 2:
									sndids[numids] = 412 + Planes[planenum].AI.lAIVoice;  //  "Middle Helo"  582 in F-15
									numids ++;
									break;
								case 1:
									sndids[numids] = 416 + Planes[planenum].AI.lAIVoice;  //  "Middle Striker"  586 in F-15
									numids ++;
									break;
								default:
									sndids[numids] = 408 + Planes[planenum].AI.lAIVoice;  //  "Middle Bandit"  578 in F-15
									numids ++;
									break;
							}
							done = 1;
						}
						else
						{
							sndids[numids] = 433 + Planes[planenum].AI.lAIVoice;  //  ""?  was 433 in F-15 but can't figure out why
							numids ++;
						}
						break;
					case 12:
						switch(type)
						{
							case 2:
								sndids[numids] = 413 + Planes[planenum].AI.lAIVoice;  //  "Trailing Helo"  583 in F-15
								numids ++;
								break;
							case 1:
								sndids[numids] = 417 + Planes[planenum].AI.lAIVoice;  //  "Trailing Striker"  587 in F-15
								numids ++;
								break;
							default:
								sndids[numids] = 409 + Planes[planenum].AI.lAIVoice;  //  "Trailing Bandit"  579 in F-15
								numids ++;
								break;
						}
						done = 1;
						break;
					default:
						sndids[numids] = SPCH_CARDINAL2 + sortnum + Planes[planenum].AI.lAIVoice;
						numids ++;
						break;
				}
			}
			else if(numval == 3)
			{
				sndids[numids] = number + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
			else if(numval == 4)
			{
				switch(type)
				{
					case 2:
						sndids[numids] = 410 + Planes[planenum].AI.lAIVoice;  //  "Helo"  was 580 in F-15
						numids ++;
						break;
					case 1:
						sndids[numids] = 414 + Planes[planenum].AI.lAIVoice;  //  "Striker"  was 584 in F-15
						numids ++;
						break;
					default:
						sndids[numids] = 406 + Planes[planenum].AI.lAIVoice;  //  "Bandit"  was 576 in F-15
						numids ++;
						break;
				}
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
int AICCheckVoiceNumInGroup(int planenum, int voice, int placeingroup)
{
	if(placeingroup <= 1)
	{
		return(1);
	}

	switch(voice)
	{
//		case SPCH_WSO:
		case SPCH_WNG1:
		case SPCH_WNG2:
		case SPCH_WNG3:
		case SPCH_WNG4:
		case SPCH_WNG5:
		case SPCH_WNG6:
		case SPCH_WNG7:
		case SPCH_WNG8:
		case SPCH_NAVY1:
		case SPCH_NAVY2:
		case SPCH_NAVY3:
		case SPCH_NAVY4:
		case SPCH_NAVY5:
			return(1);
			break;
		case SPCH_LSO:
		case SPCH_AWACS1:
		case SPCH_AWACS2:
		case SPCH_JSTARS1:
		case SPCH_SAR1:
		case SPCH_FTR_USAF:
		case SPCH_FTR_NATO:
		case SPCH_FTR_RAF:
		case SPCH_FTR_RUS:
		case SPCH_ATK_USAF:
		case SPCH_ATK_RAF:
		case SPCH_ATK_RUS:
		case SPCH_TWR_NATO:
		case SPCH_TWR_RUS:
		case SPCH_STRIKE:
		case SPCH_MARSHAL:
		case SPCH_FINAL:
		case SPCH_FAC_AIR:
		case SPCH_FAC_GRND:
		case SPCH_TANK_USN:
		case SPCH_TANK_USMC:
		default:
			return(0);
			break;
	}
	return(1);
}

//**************************************************************************************
float AIGetPlaneJammingMod(PlaneParams *planepnt, float radaranglepitch, float radarangleyaw, float widthangle, float heightangle)
{
	PlaneParams *checkplane;
	float dx, dy, dz, tdist, offangle, offpitch;
	float toffangle, toffpitch;
	float jammingrange = 100.0f * WUTOFT;
	float minrange = jammingrange;
	float fworkval, tempval;
	int widthok;
	int heightok;
	ANGLE wangle;
	float wheading;
	long sojmask = (AI_SO_JAMMING);  //  (AI_SO_JAMMING|AI_SOJ_ON_BOARD);

	for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
	{
		if((checkplane->AI.iAIFlags2 & sojmask) == sojmask)
		{
			if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (checkplane->OnGround == 0))
			{
				widthok = 0;
				heightok = 0;
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
	//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
				if((fabs(dx) < minrange) && (fabs(dz) < minrange))
				{
					tdist = QuickDistance(dx, dz);
	//				if(tdist < nlfoundrange)
					if(tdist < minrange)
					{
						offangle = atan2(-dx, -dz) * 57.2958;
	//					toffangle = offangle - AIConvertAngleTo180Degree(planepnt->Heading);
						toffangle = offangle - radarangleyaw;
						offangle = AICapAngle(offangle);

						wangle = AIConvert180DegreeToAngle(offangle) + 0x8000;
						wangle -= checkplane->Heading;
						wheading = AIConvertAngleTo180Degree(wangle);

						toffangle = AICapAngle(toffangle);

						if((fabs(toffangle) < widthangle) && (fabs(wheading) < 30.0f))
						{
							widthok = 1;
						}

						offpitch = (atan2(dy, tdist) * 57.2958);
	//					toffpitch = offpitch - AIConvertAngleTo180Degree(planepnt->Pitch);
						toffpitch = offpitch - radaranglepitch;
						toffpitch = AICapAngle(toffpitch);
						if(fabs(toffpitch) < heightangle)  //  Plane in radar area
						{
							heightok = 1;
						}

						if(widthok && heightok)
						{
							minrange = tdist;
						}
					}
				}
			}
		}
	}

	if(minrange >= jammingrange)
	{
		return(1.0f);

	}
	else if(minrange < (jammingrange * 0.5f))
	{
		return(0.5f);
	}
	else
	{
		tempval = (minrange * 2.0f) / jammingrange;
		fworkval = 0.5f + (0.5f * (tempval * tempval));
	}
	return(fworkval);
}

//**************************************************************************************
void AIC_Wing_Report_Contacts_Delay(int planenum, int targetnum)
{
	AICWingReportContacts();
}

//**************************************************************************************
void AIC_WSO_Report_Contacts_Delay(int planenum, int targetnum)
{
	iAISortNumber = -2;

	if( GetCurrentPrimaryTarget() != -1  )
	{
		AICDoSort(iAISortNumber);
	}
}

//**************************************************************************************
void AIC_WSO_Report_Contacts_Old(int planenum, int targetnum)
{
	PlaneParams *planepnt = &Planes[planenum];
	PlaneParams *checkplane;
	float foundrange = -1;
	PlaneParams *foundplane = NULL;
	float dx, dy, dz, tdist;
	float nearrange = 10.0f * NMTOWU;
	int cnt, currid;

	if(iAISortNumber == -1)
	{
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			checkplane->AI.iAIFlags2 &= ~(AI_REPORTED_AS_CONTACT);
		}
	}

	if( GetCurrentAARadarMode() == AA_STT_MODE )
	{
		if(CurTargetInfo.CurTargetId >= 0)
		{
			foundplane = &Planes[CurTargetInfo.CurTargetId];
		}
	}
	else
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			currid = CurFramePlanes.Planes[cnt].PlaneId;
			checkplane = &Planes[currid];
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
			{
				dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if((tdist < foundrange) || (foundrange < 0))
				{
					foundrange = tdist;
					foundplane = checkplane;
				}
			}
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
				checkplane = &Planes[currid];
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (planepnt->AI.iSide != checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
				{
					dx = checkplane->WorldPosition.X - planepnt->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - planepnt->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - planepnt->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if((tdist < foundrange) || (foundrange < 0))
					{
						foundrange = tdist;
						foundplane = checkplane;
					}
				}
			}
		}
	}

	if(foundplane == NULL)
	{
		if(iAISortNumber == -1)
		{
			AICCleanCall(planenum);
		}
		AICWingReportContacts();
		iAISortNumber = 0;
		return;
	}
	else if(iAISortNumber == -1)
	{
		AICReportContactPlane(planenum, 0, foundplane);
	}
	else
	{
		AICReportContactPlane(planenum, 1, foundplane);
	}

	if( GetCurrentAARadarMode() == AA_STT_MODE )
	{
	 	foundplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
		for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
		{
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
			{
				dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < nearrange)
				{
					checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
				}
			}
		}
		AICAddSoundCall(AIC_Wing_Report_Contacts_Delay, (int)(planepnt - Planes), 4000, 50);
	}
	else
	{
		for(cnt = 0; cnt < CurFramePlanes.NumPlanes; cnt ++)
		{
			currid = CurFramePlanes.Planes[cnt].PlaneId;
			checkplane = &Planes[currid];
			if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
			{
				dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
				dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
				dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < nearrange)
				{
					checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
				}
			}
		}
		if( GetCurrentAARadarMode() == AA_TWS_MODE )
		{
			for(cnt = 0; cnt < AgePlanes[0].NumPlanes; cnt ++)
			{
				currid = AgePlanes[0].Planes[cnt].PlaneId;
				checkplane = &Planes[currid];
				if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (foundplane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_REPORTED_AS_CONTACT)))
				{
					dx = checkplane->WorldPosition.X - foundplane->WorldPosition.X;
					dy = checkplane->WorldPosition.Y - foundplane->WorldPosition.Y;
					dz = checkplane->WorldPosition.Z - foundplane->WorldPosition.Z;
					tdist = QuickDistance(dx, dz);
					if(tdist < nearrange)
					{
						checkplane->AI.iAIFlags2 |= (AI_REPORTED_AS_CONTACT);
					}
				}
			}
		}
		AICAddSoundCall(AIC_WSO_Report_Contacts_Delay, (int)(planepnt - Planes), 4000, 50);
	}
	iAISortNumber = 0;
}

//**************************************************************************************
int AIRCheckCallSignSpchExists(int sndid)
{
	switch(sndid)
	{
		case 12908:  //  JSTARS no AWACS callsigns
		case 12909:
		case 12910:
		case 18932:	 //  CAP1 no Tanker callsigns
		case 18933:
		case 18934:
		case 18935:
		case 19932:	 //  CAP2 no Tanker callsigns
		case 19933:
		case 19934:
		case 19935:
		case 20932:	 //  CAP3 no Tanker callsigns
		case 20933:
		case 20934:
		case 20935:
		case 21932:	 //  SAR1 no Tanker callsigns
		case 21933:
		case 21934:
		case 21935:
		case 22932:	 //  SAR2 no Tanker callsigns
		case 22933:
		case 22934:
		case 22935:
		case 23932:	  //  SEAD1 no Tanker callsigns
		case 23933:
		case 23934:
		case 23935:
		case 24932:	  //  SEAD2 no Tanker callsigns
		case 24933:
		case 24934:
		case 24935:
		case 25932:	  //  US BOMBER no Tanker callsigns
		case 25933:
		case 25934:
		case 25935:
		case 26932:	  //  UK BOMBER no Tanker callsigns
		case 26933:
		case 26934:
		case 26935:
			return(0);
			break;
	}
	return(1);
}

CameraInstance	 OldCamera;
int OldCockpitSeat;
int OldCurrentCockpit;
int OldCursor;
char OldInteract;

void RenderMPDPopUp( float sx, float sy, BYTE *vars );

void AICDoEndBox(int idnum)
{
	char endstr[1024];
	int fontheight;
	int fontcolor = HUDColorChoices[MAX_HUD_COLOR - 5];
	int mousex, mousey;
	short leftbutton = GetMouseLB();
//	WORD primelang;
	VKCODE hack;

	StopFFForPauseOrEnd();

	if(hNetDebugFile != -1)
	{
		_close(hNetDebugFile);
		hNetDebugFile = -1;
	}

	if(hNetPacketFile != -1)
	{
		double dworkvar;
		char tstr[256];

		dworkvar = lTotalBytes;
		dworkvar /= (double)lTotalSecs;
		sprintf(tstr, "Avg Bytes/Sec %f, Max Bytes/Sec %ld", dworkvar, lMaxBytes);
		_write(hNetPacketFile,tstr,(strlen(tstr)));  // + 1
		sprintf(tstr, "\n\r");
		_write(hNetPacketFile,tstr,(strlen(tstr)));  // + 1
		_close(hNetPacketFile);
		hNetPacketFile = -1;
	}

	fontheight = TXTGetMaxFontHeight(MessageFont);

	int x,y,w,h;
	w = 145;
	h = 145;//MPD_WIDTH
	x = (ScreenSize.cx-w)>>1;
	y = (ScreenSize.cy-h)>>1;
	GrFillRectNoClip(GrBuffFor3D, x, y, w, h, 0, 28, 0, 0);

	x = 0;
	y = GrBuffFor3D->MidY - (ScreenSize.cy>>1) ;
	RenderMPDPopUp( x, y, NULL );

	AICGetEndTextTop(idnum, endstr);

	if(endstr[0])
	{
		x = ((ScreenSize.cx-w)>>1);  //   - 5;
		y = ((ScreenSize.cy-h)>>1) + 30;
		TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, x, y - (fontheight>>1), 143, fontheight * 2, fontcolor, 0, NULL, 1, 0);
		//TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, 249, 202 - (fontheight>>1), 143, fontheight * 2, fontcolor, 0, NULL, 1, 0);
	}

	AICGetEndTextBottom(idnum, endstr);

	if(endstr[0])
	{
		x = ((ScreenSize.cx-w)>>1);  //   + 10;
		y = ((ScreenSize.cy-h)>>1) + 40.0f;
		TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, x, y - (fontheight>>1), 143, fontheight * 2, fontcolor, 0, NULL, 1, 0);
		//TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, 249, 240 - fontheight, 143, 75, fontcolor, 0, NULL, 1, 0);
	}

	if(!LANGGetTransMessage(endstr, 1024, AIC_YES, g_iLanguageId))
		return;

	if(endstr[0])
	{
		x = ((ScreenSize.cx-w)>>1) - 44.0f;
		y = ((ScreenSize.cy-h)>>1) + h - fontheight;
		TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, x, y - (fontheight>>1), 143, fontheight * 2, fontcolor, 0, NULL, 1, 0);
		//TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, 227, 308 - fontheight, 100, fontheight, fontcolor, 0, NULL, 1, 0);
	}

	if((!MultiPlayer) || (idnum < 4))
	{
		if(!LANGGetTransMessage(endstr, 1024, AIC_NO, g_iLanguageId))
			return;

		if(endstr[0])
		{
			x = ((ScreenSize.cx-w)>>1) + 44;
			y = ((ScreenSize.cy-h)>>1) + h - fontheight;
			TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, x, y - (fontheight>>1), 143, fontheight * 2, fontcolor, 0, NULL, 1, 0);
			//TXTWriteStringInBox(endstr, GrBuffFor3D, MessageFont, 315, 308 - fontheight, 100, fontheight, fontcolor, 0, NULL, 1, 0);
		}
	}



	ReadMouse();
	mousex = GetMouseX();
	mousey = GetMouseY();

	if(leftbutton <0)// & 1)
	{
//		if((mousey >= 316) && (mousey <= 328))
		if((mousey >= (((ScreenSize.cy)>>1) + 72)) && (mousey <= (((ScreenSize.cy)>>1) + 86)))
		{
//			if((mousex >= 271) && (mousex <= 283))
			if((mousex >= (((ScreenSize.cx)>>1) - 50)) && (mousex <= (((ScreenSize.cx)>>1) - 36)))
			{
				if((MultiPlayer) && (iEndGameState < 4))
				{
					NetSendPlayerExit(PlayerPlane);
				}
				hack.vkCode = 0;									// virtual key code (may be joy button)
				hack.wFlags = 0;									// vkCode Type
				hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
				do_sim_done(hack);
			}
		}

//		if((mousey >= 316) && (mousey <= 328))
		if(((mousey >= (((ScreenSize.cy)>>1) + 72)) && (mousey <= (((ScreenSize.cy)>>1) + 86))) && ((!MultiPlayer) || (iEndGameState < 4)))
		{
//			if((mousex >= 359) && (mousex <= 371))
			if((mousex >= (((ScreenSize.cx)>>1) + 38)) && (mousex <= (((ScreenSize.cx)>>1) + 52)))
			{
				iEndGameState = 0;
				JustLeftPausedState = 1;
			}
		}
	}

#if 0
	if(GetAsyncKeyState(VK_ESCAPE))
	{
		iEndGameState = 0;
		JustLeftPausedState = 1;
	}
#endif

#if __DEMO__
	if(lForceEndTimer < 0)
	{
		lForceEndTimer = 10000;
	}
	else
	{
		lForceEndTimer -= DeltaTicks;
		if(lForceEndTimer < 0)
		{
			hack.vkCode = 0;									// virtual key code (may be joy button)
			hack.wFlags = 0;									// vkCode Type
			hack.dwExtraCode = 0;							// any extra VK code, eg. SHIFT, CTRL, ALT
			do_sim_done(hack);
		}
	}
#endif

	if (dwPlayerEndGameResponse)
	{

		if(dwPlayerEndGameResponse == EG_WANTS_OUT)
		{
			if((MultiPlayer) && (iEndGameState < 4))
			{
				NetSendPlayerExit(PlayerPlane);
			}

			lForceEndTimer = -1;
			SndStreamClose();
			PostMessage (g_hGameWnd, CWM_SIM_DONE, 0, 0L);
		}
		else
			if ((dwPlayerEndGameResponse == EG_WANTS_STAY) && ((!MultiPlayer) || (idnum < 4)))
			{
				lForceEndTimer = -1;
				iEndGameState = 0;
				JustLeftPausedState = 1;
			}
	}
	else
	{
		if(idnum == 5)
		{
			if(lHostLeftTimer < 0)
			{
				lHostLeftTimer = 3000;
			}
			else
			{
				lHostLeftTimer -= DeltaTicks;
				if(lHostLeftTimer < 0)
				{
					lForceEndTimer = -1;
					SndStreamClose();
					PostMessage (g_hGameWnd, CWM_SIM_DONE, 0, 0L);
				}
			}
		}
	}


	if( !iEndGameState)
		RestorePostExitView();

}


//**************************************************************************************
void AICGetEndTextTop(int idnum, char *endstr)
{
	endstr[0] = 0;

	switch(idnum)
	{
		case 1:
			return;
			break;
		case 2:
			if(!LANGGetTransMessage(endstr, 1024, AIC_MISSION_COMPLETED, g_iLanguageId))
			{
				return;
			}
			break;
		case 3:
			if(!LANGGetTransMessage(endstr, 1024, AIC_MISSION_FAILED, g_iLanguageId))
			{
				return;
			}
			break;
		case 5:
			if(!LANGGetTransMessage(endstr, 1024, AIC_HOST_END_NOTIFY, g_iLanguageId))
			{
				return;
			}
			break;
		case 4:
		default:
			return;
			break;
	}
}

//**************************************************************************************
void AICGetEndTextBottom(int idnum, char *endstr)
{
	endstr[0] = 0;

	switch(idnum)
	{
		case 1:
			if(!LANGGetTransMessage(endstr, 1024, AIC_WISH_END, g_iLanguageId))
			{
				return;
			}
			break;
		case 2:
			if(!LANGGetTransMessage(endstr, 1024, AIC_WISH_END, g_iLanguageId))
			{
				return;
			}
			break;
		case 3:
			if(!LANGGetTransMessage(endstr, 1024, AIC_WISH_END, g_iLanguageId))
			{
				return;
			}
			break;
		case 5:
			if(!LANGGetTransMessage(endstr, 1024, AIC_FORCE_END, g_iLanguageId))
			{
				return;
			}
			break;
		case 4:
		default:
			if(!LANGGetTransMessage(endstr, 1024, AIC_CONFIRM_EXIT, g_iLanguageId))
			{
				return;
			}
			break;
	}
}

//**************************************************************************************
void RestorePostExitView()
{
	lForceEndTimer = -1;

	if((pPlayerChute) && (!SimPause))
		return;

	memcpy( &Camera1, &OldCamera, sizeof( CameraInstance));

	if( OldCursor )							///if( Camera1.SubType == COCKPIT_FRONT ||Camera1.SubType == (COCKPIT_FRONT | COCKPIT_BACK_SEAT) || Camera1.SubType == (COCKPIT_FRONT | COCKPIT_45DOWN) )
		OurShowCursor( TRUE );
	else
		OurShowCursor( FALSE );
	InteractCockpitMode = OldInteract;
	CockpitSeat    = OldCockpitSeat;
	CurrentCockpit = OldCurrentCockpit;
	LoadCockpit( CurrentCockpit );
	CurrentView = -1;
	SetupView( Camera1.SubType );
	return;

}

//**************************************************************************************
long AICGetFtSpeechID(int planenum, float rangenm)
{
	long lrangenm;
	int startspeech = 610;  //  600;

	lrangenm = (long)rangenm / 10;

	if(lrangenm <= 0)
	{
		return(startspeech);
	}
	if(lrangenm < 10)
	{
		return(startspeech + lrangenm - 1);
	}
	else
	{
		return(startspeech + 9);
	}
}


//**************************************************************************************
void AICDoWSOgStuff()
{
	return;

	int numids;
	int sndids[2];
	float fworkval;

	if(PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	numids = 1;

	if(lBombFlags & WSO_G_RECOVER)
	{
		lWSOgTimer -= DeltaTicks;
		if(lWSOgTimer < 0)
		{
			lWSOgTimer = -1;
			lBombFlags &= ~(WSO_G_RECOVER);  //  (WSO_G_RECOVER|WSO_NEG_G_RECOVER);
		}
		else if((lWSOgTimer > (G_RECOVER_TIME - 5000)) && (!(lBombFlags & WSO_NEG_G_RECOVER)))
		{
			if(PlayerPlane->GForce < -2.0f)
			{
				lBombFlags |= (WSO_G_RECOVER|WSO_NEG_G_RECOVER);
				lWSOgTimer = G_RECOVER_TIME;
				if(!(rand() & 3))
				{
					sndids[0] = WSO_VOMIT + (rand() % 5);
					AIRSendSentence(PlayerPlane - Planes, numids, sndids, 0, 0, 0.25f);
				}
			}
		}
	}
	else
	{
		if(PlayerPlane->GForce < 4.5f)
		{
			if(lWSOgTimer >= 0)
			{
				lWSOgTimer -= DeltaTicks;
			}
		}
		else
		{
			fworkval = PlayerPlane->GForce - 4.5f;
			fworkval = 1.0f + ((fworkval / 6.5f) * 7.0f);
			fworkval *= (float)DeltaTicks;
			lWSOgTimer += fworkval;

			if(lWSOgTimer > G_EFFECT_TIME)
			{
				lBombFlags |= (WSO_G_RECOVER);
				lWSOgTimer = G_RECOVER_TIME;
				if(PlayerPlane->GForce < 6.5f)
				{
					sndids[0] = WSO_4_BREATHS_SLOW + (rand() % 3);
				}
				else if(PlayerPlane->GForce < 8.0f)
				{
					sndids[0] = WSO_4_BREATHS_FAST + (rand() & 1);
				}
				else
				{
					sndids[0] = WSO_6_BREATHS_STRAIN + (rand() & 1);
				}
				AIRSendSentence(PlayerPlane - Planes, numids, sndids, 0, 0, 0.25f);
			}
		}
	}
}

//**************************************************************************************
void AICDoWSONoVariablesMsg(int planenum, int targetnum)
{
	return;
	char tempstr[1024];

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, planenum, g_iLanguageId))
	{
		return;
	}

	AIRGenericSpeech(targetnum, PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	AICAddAIRadioMsgs(tempstr, 50);
}

//**************************************************************************************
void AICDoWSOEnemyDisengaging(int planenum, int targetnum)
{
	return;

	char tempstr[1024];
	char headingtxt[256];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;
	int headingval;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	if(Planes[targetnum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	headingval = AICGetTextHeadingGeneral(headingtxt, AIConvertAngleTo180Degree(Planes[targetnum].Heading));

	if(!LANGGetTransMessage(tempstr, 1024, AIC_ENEMY_DISENGAGE, g_iLanguageId, headingtxt))
	{
		return;
	}
	AICAddAIRadioMsgs(tempstr, 50);

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(channel != CHANNEL_WSO)
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AICF_ENEMY_DISENGAGE, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (WSO_CARDINAL_INTENSE + headingval);
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
void AIC_WSO_BanditCall(int planenum, int targetnum)
{
	return;

	FPoint position;
	float bearing, fworkval;
	char positionstr[256];
	char tempstr[1024];
	int msgnum, msgsnd;
	float tdist, dx, dy, dz;
	int highlow = 0;
	PlaneParams *planepnt = &Planes[planenum];
	PlaneParams *target = &Planes[targetnum];
	char enemystr[256];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int spchvar;
	float altdiff;
	int bettyid;
	int workval;

	if(planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	position = target->WorldPosition;

	if(pDBAircraftList[target->AI.iPlaneIndex].lType & AIRCRAFT_TYPE_HELICOPTER)
	{
		msgnum = AIC_ENEMY_CHOPPER;
		msgsnd = WSO_ENEMY_CHOPPER;
		bettyid = 0;
	}
	else
	{
		msgnum = AIC_BANDIT;
		msgsnd = WSO_BANDIT;
		bettyid = 1;
	}

	bearing = AIComputeHeadingToPoint(planepnt, position, &tdist, &dx ,&dy, &dz, 1);
	fworkval = -bearing;
	if(fworkval < 0)
	{
		fworkval += 360;
	}

	workval = (fworkval + 15) / 30;

	if(workval <= 0)
	{
		workval = 12;
	}

	altdiff = (target->WorldPosition.Y - planepnt->WorldPosition.Y) * WUTOFT;
	if(altdiff > 1000)
	{
		highlow = 1;
	}
	else if(altdiff < -1000)
	{
		highlow = -1;
	}


	if(lAdvisorFlags & ADV_CASUAL_BETTY)
	{
		AICGenericBettySpeech(BETTY_ENEMY_POSITION, workval, highlow, bettyid);
	}

	return;

	AIGetPPositionStr(positionstr, planepnt - Planes, fworkval, highlow);

	if(!LANGGetTransMessage(enemystr, 256, msgnum, g_iLanguageId))
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AIC_BANDIT_CALL, g_iLanguageId, enemystr, positionstr))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);

	numids = 0;

	if(!LANGGetTransMessage(tempstr, 1024, AICF_BANDIT_CALL, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum, 0, bearing, 0, 0, 0, highlow);
		}
		else if(spchvar == 2)
		{
			if(numval == 1)
			{
				sndids[numids] = msgsnd;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}

//**************************************************************************************
int AITightenSpacingOK(int planenum, float modifyby)
{
	int wingman, nextsec;

	if(modifyby >= 1.0f)
	{
		return(1);
	}

	if(iAICommFrom == -1)
	{
		wingman = PlayerPlane->AI.wingman;
	}
	else
	{
		wingman = Planes[iAICommFrom].AI.wingman;
	}

	if(wingman >= 0)
	{
		if(!FormSpacingOK(wingman, planenum, modifyby))
		{
			return(0);
		}
	}

	if(iAICommFrom == -1)
	{
		nextsec = PlayerPlane->AI.nextpair;
	}
	else
	{
		nextsec = Planes[iAICommFrom].AI.nextpair;
	}

	if(nextsec >= 0)
	{
		if(!FormSpacingOK(nextsec, planenum, modifyby))
		{
			return(0);
		}
	}

	wingman = Planes[planenum].AI.wingman;
	if(wingman >= 0)
	{
		if(!AITightenSpacingOK(wingman, modifyby))
		{
			return(0);
		}
	}

	nextsec = Planes[planenum].AI.nextpair;
	if(nextsec >= 0)
	{
		return(AITightenSpacingOK(nextsec, modifyby));
	}

	return(1);
}

//**************************************************************************************
int FormSpacingOK(int planenum, int skipnum, float modifyby)
{
	int wingman, nextsec;
	float dx, dy, dz;
	float pdx, pdy, pdz;

	if(planenum != skipnum)
	{
		dx = fabs(Planes[planenum].AI.FormationPosition.X - Planes[skipnum].AI.FormationPosition.X) * FTTOWU * modifyby;
		dy = fabs(Planes[planenum].AI.FormationPosition.Y - Planes[skipnum].AI.FormationPosition.Y) * FTTOWU * modifyby;
		dz = fabs(Planes[planenum].AI.FormationPosition.Z - Planes[skipnum].AI.FormationPosition.Z) * FTTOWU * modifyby;
		pdx = (Planes[planenum].Type->ShadowLRXOff + Planes[skipnum].Type->ShadowLRZOff) * 1.5f;
		pdy = (fabs(Planes[planenum].Type->TailHeight) + fabs(Planes[planenum].Type->GearDownHeight) + fabs(Planes[skipnum].Type->TailHeight) + fabs(Planes[skipnum].Type->GearDownHeight)) * 1.5f;
		if(pdx > pdy)
		{
			pdy = pdx;
		}
		pdz = (Planes[planenum].Type->ShadowLRZOff + Planes[skipnum].Type->ShadowLRZOff) * 1.5f;

		if((dx < pdx) && (dy < pdy) && (dz < pdz))
		{
			return(0);
		}
	}

	wingman = Planes[planenum].AI.wingman;
	if(wingman >= 0)
	{
		if(!FormSpacingOK(wingman, skipnum, modifyby))
		{
			return(0);
		}
	}

	nextsec = Planes[planenum].AI.nextpair;
	if(nextsec >= 0)
	{
		return(FormSpacingOK(nextsec, skipnum, modifyby));
	}
	return(1);
}

//**************************************************************************************
void AIC_Wingman_BreakLRMsg(int planenum, int targetnum)
{
	char tempstr[1024];
	int msgnum, msgsnd;
	char tempstr2[256];
	int placeingroup;

	if(Planes[planenum].FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))
	{
		return;
	}

	placeingroup = (PlayerPlane->AI.iAIFlags1 & AIFLIGHTNUMS) + 1;

	if(!LANGGetTransMessage(tempstr2, 256, AIC_ZERO + placeingroup, g_iLanguageId))
	{
		return;
	}

	if(targetnum == 0)
	{
		msgnum = AIC_WING_BREAK_LEFT;
		msgsnd = AICF_WING_BREAK_LEFT;
	}
	else
	{
		msgnum = AIC_WING_BREAK_RIGHT;
		msgsnd = AICF_WING_BREAK_RIGHT;
	}

	if(!LANGGetTransMessage(tempstr, 1024, msgnum, g_iLanguageId, tempstr2))
	{
		return;
	}

	AICAddAIRadioMsgs(tempstr, 50);
	AIRGenericSpeech(msgsnd, planenum, PlayerPlane - Planes, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}

//**************************************************************************************
void AICAfterEjectionSoundCancel()
{
	SndStopChannel(CHANNEL_BETTY);
	SndStopChannel(CHANNEL_WSO);
	KillSound(&g_dwAIMSndHandle);
	KillSound(&g_dwAOASndHandle);
	KillSound(&g_dwAIMLockSndHandle);
//	SndStopChannel(CHANNEL_WING);
//	SndStopChannel(CHANNEL_SPEECH);
//	SndStopChannel(CHANNEL_ENGINE);
//	SndStopChannel(CHANNEL_LINTAKE);
//	SndStopChannel(CHANNEL_RINTAKE);
//	SndStopChannel(CHANNEL_WNDNOICE);
//	SndStopChannel(CHANNEL_FIRSTGENL);
	return;
}

//**************************************************************************************
int AIIsCampaignJSTARS(PlaneParams *checkplane)
{
	int numwpts;

	if(g_nMissionType!=SIM_CAMPAIGN)
	{
		return(0);
	}

	numwpts = checkplane->AI.numwaypts + (checkplane->AI.CurrWay - &AIWayPoints[checkplane->AI.startwpts]);

	if(numwpts <= 2)
	{
		return(1);
	}

	return(0);
}

#if 0
//**************************************************************************************
void SRESndTest()
{
	int tempint[50];
	DWORD dwHandle;
	char tempstr[256];

	tempint[0] = Wso1415;
	tempint[1] = Wso1389;
	tempint[2] = Wso1003;
	tempint[3] = Wso1147;
	tempint[4] = Wso1282;
	tempint[5] = Wso1286;
	tempint[6] = Wso1287;
	tempint[7] = Wso1323;
	tempint[8] = Wso1382;
	tempint[9] = Wso1385;
	tempint[10] = Wso1141;

	sprintf(tempstr, "Test Sentence 1");
	AICAddAIRadioMsgs(tempstr, 50);
 	dwHandle = SndQueueSentence(CHANNEL_WSO, 11, tempint,g_iBaseSpeechSoundLevel);
#ifdef SRESERVE
	SndServiceSound();
#endif

	tempint[0] = Wso1614;
	tempint[1] = Wso1014;
	tempint[2] = Wso1044;
	tempint[3] = Wso1045;
	tempint[4] = Wso1058;
	tempint[5] = Wso1060;
	tempint[6] = Wso1061;
	tempint[7] = Wso1062;
	tempint[8] = Wso1063;
	tempint[9] = Wso1065;
	tempint[10] = Wso1068;
	tempint[11] = Wso1658;

	sprintf(tempstr, "Test Sentence 2");
	AICAddAIRadioMsgs(tempstr, 50);

 	dwHandle = SndQueueSentence(CHANNEL_WSO, 12, tempint,g_iBaseSpeechSoundLevel);
#ifdef SRESERVE
	SndServiceSound();
#endif

	tempint[0] = Wso1282;
	tempint[1] = Wso1283;
	tempint[2] = Wso1284;
	tempint[3] = Wso1285;
	tempint[4] = Wso1286;
	tempint[5] = Wso1287;
	tempint[6] = Wso1288;
	tempint[7] = Wso1289;
	tempint[8] = Wso1290;
	tempint[9] = Wso1291;

	sprintf(tempstr, "Test Sentence 3");
	AICAddAIRadioMsgs(tempstr, 50);

 	dwHandle = SndQueueSentence(CHANNEL_WSO, 10, tempint,g_iBaseSpeechSoundLevel);
#ifdef SRESERVE
	SndServiceSound();
#endif
}

//**************************************************************************************
void SRESndTest2()
{
	int tempint[50];
	DWORD dwHandle;
	char tempstr[256];

	tempint[0] = Wso1415;
	tempint[1] = Wso1389;
	tempint[2] = Wso1003;
	tempint[3] = Wso1147;
	tempint[4] = Wso1282;
	tempint[5] = Wso1286;
	tempint[6] = Wso1287;
	tempint[7] = Wso1323;
	tempint[8] = Wso1382;
	tempint[9] = Wso1385;
	tempint[10] = Wso1141;

	sprintf(tempstr, "Test2 Sentence 1");
	AICAddAIRadioMsgs(tempstr, 50);

 	dwHandle = SndQueueSentence(CHANNEL_WSO, 11, tempint,g_iBaseSpeechSoundLevel);

	AICAddSoundCall(SRESndTest2A, 0, 2000, 50);
#ifdef SRESERVE
	SndServiceSound();
#endif
}

//**************************************************************************************
void SRESndTest2A(int planenum, int targetnum)
{
	int tempint[50];
	DWORD dwHandle;
	char tempstr[256];

	tempint[0] = Wso1614;
	tempint[1] = Wso1014;
	tempint[2] = Wso1044;
	tempint[3] = Wso1045;
	tempint[4] = Wso1058;
	tempint[5] = Wso1060;
	tempint[6] = Wso1061;
	tempint[7] = Wso1062;
	tempint[8] = Wso1063;
	tempint[9] = Wso1065;
	tempint[10] = Wso1068;
	tempint[11] = Wso1658;

 	dwHandle = SndQueueSentence(CHANNEL_WSO, 12, tempint,g_iBaseSpeechSoundLevel);

	sprintf(tempstr, "Test2 Sentence 2");
	AICAddAIRadioMsgs(tempstr, 50);

	AICAddSoundCall(SRESndTest2B, 0, 2000, 50);
#ifdef SRESERVE
	SndServiceSound();
#endif
}

void SRESndTest2B(int planenum, int targetnum)
{
	int tempint[50];
	DWORD dwHandle;
	char tempstr[256];

	tempint[0] = Wso1282;
	tempint[1] = Wso1283;
	tempint[2] = Wso1284;
	tempint[3] = Wso1285;
	tempint[4] = Wso1286;
	tempint[5] = Wso1287;
	tempint[6] = Wso1288;
	tempint[7] = Wso1289;
	tempint[8] = Wso1290;
	tempint[9] = Wso1291;

 	dwHandle = SndQueueSentence(CHANNEL_WSO, 10, tempint,g_iBaseSpeechSoundLevel);

#ifdef SRESERVE
	SndServiceSound();
#endif

	sprintf(tempstr, "Test2 Sentence 3");
	AICAddAIRadioMsgs(tempstr, 50);
}

//**************************************************************************************
void AIRDisengageSnd(int planenum, int placeingroup, int tempnum)
{
	char tempstr[1024];
	char *tsptr;
	int done = 0;
	int numval;
	int strcnt;
	int numids;
	int sndids[20];
	int channel;
	int spchvar;

	numids = 0;

	channel =  AIRGetChannel(planenum);

	if(channel != CHANNEL_WINGMEN)
	{
		return;
	}

	if(!LANGGetTransMessage(tempstr, 1024, AICF_DISENGAGE_MSG, g_iLanguageId))
	{
		return;
	}

	tsptr = tempstr;
	while(!done)
	{
		numval = AICGetNextSndInt(tsptr, &strcnt, &done, &spchvar);
		tsptr += strcnt;

		if(spchvar == 1)
		{
			numids += AIRProcessSpeechVars(&sndids[numids], numval, planenum);
		}
		else if(spchvar == 2)
		{
			if(numval == 2)
			{
				sndids[numids] = (WING_DISENGAGE + tempnum) + Planes[planenum].AI.lAIVoice;
				numids ++;
			}
		}
		else
		{
			sndids[numids] = numval + Planes[planenum].AI.lAIVoice;
			numids ++;
		}
	}
	AIRSendSentence(planenum, numids, sndids);
}
#endif