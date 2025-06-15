//  Multiplayer chat menus

#include "F18.h"
#include "keystuff.h"

#define CHAT_CURSORBLINK		200
#define MAX_CHAT_CHARS			80

extern int XmitChannel;
extern int MySlot;

char XmitText[MAX_PATH];

static DWORD cursorTick;
static BOOL cursorBlink;

char * NetGetChannelName(int nChannel);
void TransmitText(int nChannel, char *text);
void EchoMessage(char *text);

void NetRadioChatMenu (void)
{
	if (!MultiPlayer || !XmitChannel) return;

	int		w, h, hw;
	char	szTemp[MAX_PATH];

	wsprintf(szTemp,"%s: ",NetGetChannelName(XmitChannel));
	// calc sizes
	hw = GrStrLen(szTemp, LgHUDFont);
	w = (MAX_CHAT_CHARS * LgHUDFont->iMaxFontWidth) + hw;
	h = LgHUDFont->iMaxFontHeight + 3;

	// display text
	GrFillRect8(GrBuffFor3D, 0, 0, 640, h, 255);
	GrDrawString(GrBuffFor3D, LgHUDFont, 2 , 2, szTemp, 1);
	GrDrawString(GrBuffFor3D, LgHUDFont, 2 + hw, 2, XmitText, 1);

	// check blink cursor
	if ( GetTickCount() - cursorTick > CHAT_CURSORBLINK)
	{
		cursorTick = GetTickCount();
		cursorBlink ^= 1;
	}

	// display cursor
	if (cursorBlink)
	{
		int lpx = GrStrLen(XmitText, LgHUDFont) + hw;
		GrFillRect8(GrBuffFor3D, lpx + 2, 1, 2, h-1, 1);
	}
}


void AddXmitChar(int ch)
{
	int pos = strlen(XmitText);

	if (pos>=MAX_CHAT_CHARS) return;

	XmitText[pos] = ch;
	XmitText[pos + 1] = NULL;
}


BOOL NetRadioCheckKey (int keyflag, WPARAM wParam)
{
	int pos;

	if (!MultiPlayer || !XmitChannel) return FALSE;

	if (keyflag)
	{
		if (isprint(wParam))
			AddXmitChar(wParam);
	}
	else
	switch (wParam)
	{
		case VK_RETURN:

			if (strlen(XmitText))
				TransmitText(XmitChannel,XmitText);
			
			// echo message to self

			EchoMessage(XmitText);

			// pass thru to next case

		case VK_ESCAPE:
			XmitChannel = 0;
			*XmitText = NULL;
			break;

		case VK_BACK:
			pos = strlen(XmitText);
			if (pos)
				XmitText[pos-1] = NULL;
			break;
	}

	return TRUE;
}
