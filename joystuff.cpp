//**************************************************************************************
//  JOYSTUFF.CPP
//**************************************************************************************

#include "F18.h"
#include "gamesettings.h"

#define NOJOY 32768		// center stick value or half-scale value
#define HALF_SCREEN_CX 320
#define HALF_SCREEN_CY 240

int  JoyOK;		// Joystick init'd and working ? (T/F)
int  JoyID;		// current init'd joystick

int JoyDeadX;	// joystick deadband [0..32767]
int JoyDeadY;
int JoyDeadR;

JOYINFOEX *J1 = &JoyInfoEx1;

JOYINFOEX oldJoy;
JOYINFOEX newJoy;



//*****************************************************************************
//=============================================================================
//		J O Y    S T I C K     F U N C T I O N S
//=============================================================================
void ZeroJoy( JOYINFOEX *J )
{
	J->dwXpos = NOJOY;
	J->dwYpos = NOJOY;
	J->dwZpos = NOJOY;
	J->dwRpos = NOJOY;
	J->dwUpos = NOJOY;
	J->dwVpos = NOJOY;
	J->dwPOV  = JOY_POVCENTERED;
	J->dwButtons = 0;
	J->dwButtonNumber = 0;
}

//*****************************************************************************
int InitJoy(void)
{
	JoyOK = 0;
	JoyID = JOYSTICKID1;

	int stix = joyGetNumDevs();

	for (int i=0; i<stix; i++)
	{
		JOYINFO jInfo;

		if (joyGetPos( JOYSTICKID1+i, &jInfo ) == JOYERR_NOERROR)
		{
			JoyOK  = 1;
			JoyID += i;

			joyGetDevCaps( JoyID, &JoyCap1, sizeof(JOYCAPS) );

			ZeroJoy( J1 );
			ZeroJoy( &oldJoy );
			ZeroJoy( &newJoy );

			// Init the dead zones from config/wrappers.
			JoyDeadY = g_Settings.misc.axes[0].byDeband * 128;
			JoyDeadX = g_Settings.misc.axes[1].byDeband * 128;
			JoyDeadR = g_Settings.misc.axes[2].byDeband * 128;

			break;
		}
	}

	return JoyOK;
}

/*-----------------------------------------------------------------------------
 *
 *	CenterJoy()
 *
 */
void CenterJoy()
{
}


int JoyDir;
float JoyAngle;
float JoyHorz;
float JoyVert;
int   IsLeft,IsUp;

float GetJoystickDirection()
{
  int Dx,Dy;

  if( (fabs(32768 - oldJoy.dwXpos) < 30000) && (fabs(32768 - oldJoy.dwYpos) < 30000) )
	  return(-1);

  Dx = oldJoy.dwXpos - 32768;
  Dy = oldJoy.dwYpos - 32768;

  float OffAngle = atan2(-Dx,-Dy)*57.2957795;

  while(OffAngle > 180.0)
	OffAngle -= 360.0;

  while(OffAngle < -180)
	OffAngle += 360.0;

  return( NormDegree(OffAngle) );

}


void GetJoyStickExtentsInPercent(int *IsLeft, int *IsUp, float *HorzPercent, float *VertPercent)
{
  *IsLeft = (oldJoy.dwXpos < 32768);
  *IsUp   = (oldJoy.dwYpos < 32768);

  if( *IsLeft )
    *HorzPercent = (32768.0f - oldJoy.dwXpos)/32768.0f;
  else
    *HorzPercent = (oldJoy.dwXpos - 32768.0f)/32768.0f;

  if( *IsUp )
    *VertPercent = (32768.0f - oldJoy.dwYpos)/32768.0f;
  else
    *VertPercent = (oldJoy.dwYpos - 32768.0f)/32768.0f;

  // rescale for deadzone
  if(*HorzPercent <= 0.2)
	*HorzPercent = 0.0;
  else
    *HorzPercent = (*HorzPercent - 0.2)/0.8;

  // rescale for deadzone
  if(*VertPercent <= 0.2)
	*VertPercent = 0.0;
  else
    *VertPercent = (*VertPercent - 0.2)/0.8;
}

// JLM CONTROL PLANE WITH KEY COMMANDS

typedef struct
{
  int Status;
  class Timer LeftTimer;
  class Timer RightTimer;
  class Timer UpTimer;
  class Timer DownTimer;
} KeyFlyType;

KeyFlyType KeyFly;
int KeyFlyInit = FALSE;

#define KEYFLY_LEFT  2
#define KEYFLY_RIGHT 4
#define KEYFLY_UP    8
#define KEYFLY_DOWN  16

VKCODE KeyFlyUpVk;
VKCODE KeyFlyDownVk;
VKCODE KeyFlyLeftVk;
VKCODE KeyFlyRightVk;
extern VKCODE CaptBarsVk;

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int DoKeyBoardFly()
{
  if(!KeyFlyInit)
  {
    KeyFlyInit = TRUE;
    KeyFly.Status = 0;
  }

  DWORD PrevLeftUp   = !(KeyFly.Status & KEYFLY_LEFT);
  DWORD PrevRightUp  = !(KeyFly.Status & KEYFLY_RIGHT);
  DWORD PrevUpUp     = !(KeyFly.Status & KEYFLY_UP);
  DWORD PrevDownUp   = !(KeyFly.Status & KEYFLY_DOWN);

  float TimeElapsed;

  if(PrevLeftUp && PrevRightUp)
	 newJoy.dwXpos = NOJOY;

  if(PrevUpUp && PrevDownUp)
	 newJoy.dwYpos = NOJOY;

  if(GetVkStatus(KeyFlyLeftVk))
    KeyFly.Status |= KEYFLY_LEFT;
  else
    KeyFly.Status &= ~KEYFLY_LEFT;

  if( (PrevLeftUp) && (KeyFly.Status & KEYFLY_LEFT) )
  {
    KeyFly.LeftTimer.Set(1.0,GameLoopInTicks);
  }

  if(KeyFly.Status & KEYFLY_LEFT)
  {
    TimeElapsed = KeyFly.LeftTimer.TimeElapsed(GameLoopInTicks);
	oldJoy.dwXpos = NOJOY;

	if(TimeElapsed > 0.3)
      newJoy.dwXpos = 1; //TimeElapsed*32768.0; //0.5*32768.0*(TimeElapsed*TimeElapsed);
	else
      newJoy.dwXpos = 15000;
  }

  if(GetVkStatus(KeyFlyRightVk))
    KeyFly.Status |= KEYFLY_RIGHT;
  else
    KeyFly.Status &= ~KEYFLY_RIGHT;

  if( (PrevRightUp) && (KeyFly.Status & KEYFLY_RIGHT) )
  {
    KeyFly.RightTimer.Set(1.0,GameLoopInTicks);
  }

  if(KeyFly.Status & KEYFLY_RIGHT)
  {
    TimeElapsed = KeyFly.RightTimer.TimeElapsed(GameLoopInTicks);
	oldJoy.dwXpos = NOJOY;
	if(TimeElapsed > 0.3)
       newJoy.dwXpos = 64000; //32768.0 + TimeElapsed*32768.0; //0.5*32768.0*(TimeElapsed*TimeElapsed);
	else
        newJoy.dwXpos = 49000;

  }

  if(GetVkStatus(KeyFlyUpVk))
    KeyFly.Status |= KEYFLY_UP;
  else
    KeyFly.Status &= ~KEYFLY_UP;

  if( (PrevUpUp) && (KeyFly.Status & KEYFLY_UP) )
  {
    KeyFly.UpTimer.Set(1.0,GameLoopInTicks);
  }

  if(KeyFly.Status & KEYFLY_UP)
  {
    TimeElapsed = KeyFly.UpTimer.TimeElapsed(GameLoopInTicks);
    newJoy.dwYpos = 500; //32768.0 + TimeElapsed*32768.0; //0.5*32768.0*(TimeElapsed*TimeElapsed);
  }

  if(GetVkStatus(KeyFlyDownVk))
    KeyFly.Status |= KEYFLY_DOWN;
  else
    KeyFly.Status &= ~KEYFLY_DOWN;

  if( (PrevDownUp) && (KeyFly.Status & KEYFLY_DOWN) )
  {
    KeyFly.DownTimer.Set(1.0,GameLoopInTicks);
  }

  if(KeyFly.Status & KEYFLY_DOWN)
  {
    TimeElapsed = KeyFly.DownTimer.TimeElapsed(GameLoopInTicks);
    newJoy.dwYpos = 64000; //32768.0 + TimeElapsed*32768.0; //0.5*32768.0*(TimeElapsed*TimeElapsed);
  }

	return( (KeyFly.Status & KEYFLY_LEFT) || (KeyFly.Status & KEYFLY_RIGHT) || (KeyFly.Status & KEYFLY_UP) || (KeyFly.Status & KEYFLY_DOWN) );
}


//**************************************************************************************
void ReadJoyEx( DWORD flags )
{
	int JoyResult = FALSE;
	int CaptBarsOn = FALSE;
  JOYINFOEX TempJoy;


	newJoy.dwSize  = sizeof(JOYINFOEX);
	newJoy.dwFlags = flags | JOY_RETURNALL | JOY_RETURNCENTERED | JOY_RETURNPOV;

  int KeyFlyResult = DoKeyBoardFly();

	// somewhat of a kludge....if capt bars are on, we want to calculate joystick data as normal, but we don't want to update the flight model
	// so, we'll set it to 0 (stick is centered), do normal calculations, then write it back
	if( GetVkStatus(CaptBarsVk) )
	{
		CaptBarsOn = TRUE;
         memcpy(&TempJoy,&JoyInfoEx1,sizeof(JOYINFOEX));
	}

	if(!KeyFlyResult)
	   JoyResult = (joyGetPosEx( JoyID, &newJoy ) == JOYERR_NOERROR);

	if(KeyFlyResult || JoyResult || CaptBarsOn)
	{
		int dx = newJoy.dwXpos - oldJoy.dwXpos;
		int FilterOn = g_Settings.misc.bFilter;

		if ((abs(dx) < 10000) || !FilterOn)
			J1->dwXpos = (newJoy.dwXpos & 0xFFFF) + (newJoy.dwXpos==0);

		int dy = newJoy.dwYpos - oldJoy.dwYpos;
		if ((abs(dy) < 10000) || !FilterOn)
			J1->dwYpos = (newJoy.dwYpos & 0xFFFF) + (newJoy.dwYpos==0);

		int dz = newJoy.dwZpos - oldJoy.dwZpos;
		if ((abs(dz) < 10000) || !FilterOn)
		{
			// Clip MIL setting to at least 20%
			DWORD MilSet = max(0xffff - g_Settings.misc.dwMIL, 0x3333);

			newJoy.dwZpos = 0xffff - newJoy.dwZpos;

			//
			// do a fake (2 line) curve at MIL setting 80% (0xcccc)
			//

			if (newJoy.dwZpos<=MilSet)
			{
				// Scale less MilSet to 80% (0-0xcccc)
				newJoy.dwZpos = (newJoy.dwZpos * 0xcccc) / MilSet + (newJoy.dwZpos==0);
			}
			else
			{
				// Scale greater MilSet (0xcccc - 0xffff)
				newJoy.dwZpos = ((newJoy.dwZpos - MilSet) * 0x3333) / (0xffff - MilSet) + 0xcccc;
			}

			J1->dwZpos = 0xffff - newJoy.dwZpos;

			// J1->dwZpos = (newJoy.dwZpos & 0xFFFF) + (newJoy.dwZpos==0);
		}

		int dr = newJoy.dwRpos - oldJoy.dwRpos;
		if ((abs(dr) < 10000) || !FilterOn)
			J1->dwRpos = (newJoy.dwRpos & 0xFFFF) + (newJoy.dwRpos==0);

		int du = newJoy.dwUpos - oldJoy.dwUpos;
		if ((abs(du) < 10000) || !FilterOn)
			J1->dwUpos = (newJoy.dwUpos & 0xFFFF) + (newJoy.dwUpos==0);

		int dv = newJoy.dwVpos - oldJoy.dwVpos;
		if ((abs(dv) < 10000) || !FilterOn)
			J1->dwVpos = (newJoy.dwVpos & 0xFFFF) + (newJoy.dwVpos==0);

		J1->dwPOV			= newJoy.dwPOV;
		J1->dwButtons		= newJoy.dwButtons;
		J1->dwButtonNumber	= newJoy.dwButtons;

		oldJoy = newJoy;

		// copy the temp version back
		if(CaptBarsOn)
		{
      memcpy(&JoyInfoEx1,&TempJoy,sizeof(JOYINFOEX));
		}
	}
}

//**************************************************************************************
void ReadJoy(void)		{ ReadJoyEx(        0        ); }
void ReadJoyWDead(void)	{ ReadJoyEx( JOY_USEDEADZONE ); }

void ReadJoyVarD( DWORD dead ) { dead ? ReadJoyWDead() : ReadJoy(); }

//**************************************************************************************
DWORD GetJoyDead( int joy, int dead )
{
	if (!dead)  return joy;	// read: If not dead, return with joy.

	joy -= NOJOY;	// convert to [-32767..0..32767]

	if (abs(joy) < dead)  return NOJOY;	// read: If your absolute joy is less than dead,
										//		 return with nothing.
	joy -= (joy>0) ? dead : -dead;

	joy = (NOJOY * joy) / (NOJOY - dead);

	return NOJOY + joy;	// convert back to [1..65535]
}

/*-----------------------------------------------------------------------------
 *
 *	ProfileJoy()  --  [10..90] 9-mark / half-scale / scaling-factor
 *
 */
int ProfileJoy( int joy, int axis )
{
	joy -= NOJOY;	// convert to [-32767..0..32767]

	int J10 = 10 * abs( joy );

	int bin = J10 / NOJOY;		// bin# [0..9]
	int rem = J10 & NOJOY-1;	// rem  [0..32767]

	BYTE *bins = g_Settings.misc.axes[axis].byProfile;

	int bin0 = (bin>0) ? bins[bin-1] : bins[0];	// bin below JoyPos
	int bin1 = (bin<9) ? bins[bin  ] : bins[8];	// bin above JoyPos

	int mult = (bin0*NOJOY + (bin1-bin0)*rem) / 256;	// scaling factor << 15

	int nuJoy = joy * mult / NOJOY;

	return NOJOY + nuJoy;	// convert back to [1..65535]
}

/*-----------------------------------------------------------------------------
 *
 *	ProfileJoy()  --  [10..90] 9-mark / half-scale / response-curve
 *
 */
int ProfileJoy1( int joy, int axis )
{
	joy -= NOJOY;	// convert to [-32768..0..32767]

	int J10 = 10 * abs( joy );

	int bin = J10 / NOJOY;		// bin# [0..9]
	int rem = J10 & NOJOY-1;	// rem  [0..32767]

	BYTE *bins = g_Settings.misc.axes[axis].byProfile;

	int bin0 = (bin>0) ? bins[bin-1] :   0;	// bin below JoyPos
	int bin1 = (bin<9) ? bins[bin  ] : 255;	// bin above JoyPos

	int nuJoy = (bin0*NOJOY + (bin1-bin0)*rem) / 256;

	if (joy < 0)  nuJoy = -nuJoy;	// new signed value

	return NOJOY + nuJoy;	// convert back to [0..65535]
}

/*-----------------------------------------------------------------------------
 *
 *	ProfileJoy()  --  [10..90] 9-mark / half-scale / scaling-factor
 *
 */
int ProfileJoy2( int joyPos, int axis )
{
	int filter = g_Settings.misc.bFilter;

	BYTE *bins = g_Settings.misc.axes[axis].byProfile;

	int bin = joyPos * 10 / 65536;	// bin# [0..9]
	int rem = joyPos * 10 & 65535;	// rem  [0..65535]

	int bin0 = (bin>0) ? bins[bin-1] : bins[0];	// bin below JoyPos
	int bin1 = (bin<9) ? bins[bin  ] : bins[8];	// bin above JoyPos

	int mult = (bin0*65536 + (bin1-bin0)*rem) / 256;	// scaling factor << 16

	int joy = 32768 + (joyPos-32768) * mult / 65536;

	return joy;
}

/*-----------------------------------------------------------------------------
 *
 *	ProfileJoy()  --  [1..0..1] 9-mark / full-scale / scaling-factor
 *
 */
int ProfileJoy3( int joyPos, int axis )
{
	int filter = g_Settings.misc.bFilter;

	BYTE *bins = g_Settings.misc.axes[axis].byProfile;

	int bin = joyPos * 8 / 65536;	// bin# [0..7]
	int rem = joyPos * 8 & 65535;	// rem  [0..65535]

	int bin0 = bins[bin+0];	// bin below JoyPos
	int bin1 = bins[bin+1];	// bin above JoyPos

	int mult = (bin0*65536 + (bin1-bin0)*rem) / 256;	// interpolate & scale

	int joy = 32768 + (joyPos-32768) * mult / 65536;

	return joy;
}

//**************************************************************************************
//**************************************************************************************

 // Return joystick values with dead-zone and profile post-processing.
//																	// screen order
int GetJoyPosY(void) { return ProfileJoy( GetJoyDeadY(), 0 ); }	// pitch
int GetJoyPosX(void) { return ProfileJoy( GetJoyDeadX(), 1 ); }	// roll
int GetJoyPosR(void) { return ProfileJoy( GetJoyDeadR(), 2 ); }	// yaw

 //	Return joystick values with dead-zone post-processing
//
DWORD GetJoyDeadX(void) { return GetJoyDead( JoyInfoEx1.dwXpos, JoyDeadX ); }
DWORD GetJoyDeadY(void) { return GetJoyDead( JoyInfoEx1.dwYpos, JoyDeadY ); }
DWORD GetJoyDeadR(void) { return GetJoyDead( JoyInfoEx1.dwRpos, JoyDeadR ); }

 //	Return RAW joystick values from last read.
//
DWORD GetJoyX(void) { return JoyInfoEx1.dwXpos; }
DWORD GetJoyY(void) { return JoyInfoEx1.dwYpos; }
DWORD GetJoyZ(void) { return JoyInfoEx1.dwZpos; }
DWORD GetJoyR(void) { return JoyInfoEx1.dwRpos; }
DWORD GetJoyU(void) { return JoyInfoEx1.dwUpos; }
DWORD GetJoyV(void) { return JoyInfoEx1.dwVpos; }

 // Return buttons.
//
DWORD GetJoyB1(void) { return JoyInfoEx1.dwButtons & JOY_BUTTON1; }
DWORD GetJoyB2(void) { return JoyInfoEx1.dwButtons & JOY_BUTTON2; }
DWORD GetJoyB3(void) { return JoyInfoEx1.dwButtons & JOY_BUTTON3; }
DWORD GetJoyB4(void) { return JoyInfoEx1.dwButtons & JOY_BUTTON4; }

DWORD GetJoyPOV(void) {	return JoyInfoEx1.dwPOV; }

DWORD GetJoyButtons(void) {	return JoyInfoEx1.dwButtons; }

 // Inc/dec the dead-zones.
//
void DecDeadX(void) { if (JoyDeadX >=  1000) JoyDeadX -= 1000; }
void DecDeadY(void) { if (JoyDeadY >=  1000) JoyDeadY -= 1000; }
void DecDeadR(void) { if (JoyDeadR >=  1000) JoyDeadR -= 1000; }
void IncDeadX(void) { if (JoyDeadX <= 31000) JoyDeadX += 1000; }
void IncDeadY(void) { if (JoyDeadY <= 31000) JoyDeadY += 1000; }
void IncDeadR(void) { if (JoyDeadR <= 31000) JoyDeadR += 1000; }

//**************************************************************************************
//**************************************************************************************
DWORD GetJoyBtn( DWORD dwButtonNum )
{
	if (dwButtonNum <= 0)  return(0);

	int joybit = JOY_BUTTON1 << (dwButtonNum - 1);

#ifdef USE_ALL_MMSYSTEM_JOY_BUTTON_DEFINES
	   //*  this code was made using the defines from MMSYSTEM.H.  These codes for buttons *
	   //*  5 - 32 were incorrect since the 1 bit is not set for these buttons.  This was  *
	   //*  discovered using the MicroSoft Sidewinder Pro, I think it SHOULD be correct (I *
	   //*  had wondered about that 1 bit myself since it would assume the trigger had been *
	   //*  pressed anytime a button between 5 and 32 had been pressed.                    *
	if(dwButtonNum < 5)  return (JoyInfoEx1.dwButtons & joybit);

	if(dwButtonNum <= 32)
	{
		DWORD dwButtonCheck = joybit + 1;
		return (dwButtonCheck == (JoyInfoEx1.dwButtons & dwButtonCheck));
	}
#else
	if (dwButtonNum <= 32)  return (JoyInfoEx1.dwButtons & joybit);
#endif

	return (JoyInfoEx1.dwButtonNumber == dwButtonNum);
}


//*****************************************************************************
//=============================================================================
//		K E Y B O A R D     F U N C T I O N S
//=============================================================================
//*****************************************************************************

void ReadKeyboard(void)
{
	//**********************************************************************************
	//*  GetAsyncKeyState will tell you if a key on the keyboard is down.
	//*  If the return value is < 0 then the key is down.  If the 1 bit is
	//*  set then the key was just press (or the key has been held down
	//*  and the repeat key has kicked in), otherwise the key was already
	//*  down.  So if the key was just pushed it would equal 0xFFFF8001
	//*  otherwise if it was already pushed it would equal 0xFFFF8000
	//*
	//*  VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39)
	//*  VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)
	//*
	//*  GetAsyncKeyState doesn't care what other keys are down besides the
	//*  one you are checking.  So to check for a SHIFT B, you need to look
	//*  for the SHIFT key and the B key.
	//*
	//*  NOTE:  You can't look for lower case letters (or symbols above the
	//*         numbers) on the keyboard.  To check for these you need to
	//*         see if the SHIFT key is down or not.
	//*
	//*  ReadKeyboard does not have to be used to use GetAsyncKeyState.  Therefore
	//*  if it is more convient to check a key state in the middle of a function
	//*  then feel free to do so.  This function would be used if you want to
	//*  group all the key effects together.
	//*
	//*  If you want to use Messages for key effects then you will want to use
	//*  WM_KEYDOWN, and WM_SYSKEYDOWN(used for ALT key combos), where the wParam
	//*  will hold the VK value of the key hit.
	//*
	//*
	//*  If you want to do combonations of ALT/CTRL/SHIFT in this function then you
	//*  will have to check for the other keys within one of the if statements.
	//*		IE:
	//*
	//*		if(GetAsyncKeyState(VK_MENU) < 0) {
	//*			if(GetAsyncKeyState(VK_CONTROL) < 0) {
	//*				if((GetAsyncKeyState(VK_DELETE) < 0) || (GetAsyncKeyState(VK_DECIMAL) < 0)) {
	//*					/*  Reboot Computer  */
	//*				}
	//*			}
	//*		}
	//*
	//*	 Remember that the order you check is important.  If you put the check for VK_MENU
	//*  in the VK_CONTROL branch in the code below, it would never get executed since
	//*  VK_MENU is looked for first.
	//*
	//**********************************************************************************
	//*  IMPORTANT SUGGESTION:
	//*		Check the keyboard after updating mouse and joystick position (especially if
	//*     you are using VK_(L/R/M)BUTTON to get the mouse buttons.  In this way you
	//*     won't have people clicking on the wrong place.
	//*
	//**********************************************************************************

	if(GetAsyncKeyState(VK_SHIFT) < 0) {
		if(GetAsyncKeyState(0xBB) == 0xFFFF8001)
		{
			IncDeadR();
		}
		if(GetAsyncKeyState(0xBD) == 0xFFFF8001)
		{
			DecDeadR();
		}
	}
	else if(GetAsyncKeyState(VK_MENU) < 0)   //*  ALT KEY  *
	{
		if(GetAsyncKeyState('C') == 0xFFFF8001)
		{
			if(dwDeadZone)
				dwDeadZone = 0;
			else
				dwDeadZone = 1;
		}
	}
	else if(GetAsyncKeyState(VK_CONTROL) < 0)   //*  CTRL KEY  *
	{
	}
	else
	{
		if(GetAsyncKeyState(0xBB) == 0xFFFF8001)
		{
			IncDeadX();
		}
		if(GetAsyncKeyState(0xBD) == 0xFFFF8001)
		{
			DecDeadX();
		}
		if(GetAsyncKeyState(VK_ADD) == 0xFFFF8001)
		{
			IncDeadY();
		}
		if(GetAsyncKeyState(VK_SUBTRACT) == 0xFFFF8001)
		{
			DecDeadY();
		}
	}
	return;
}

//*****************************************************************************
//=============================================================================
//		M O U S E     F U N C T I O N S
//=============================================================================
//*****************************************************************************
TextureBuff *pCursorTexture = NULL;

//I know but nothing is ever in to check out!
void GrDrawSprite( TextureBuff *pTexture,int x, int y, float  r, float g, float b, int orientation, float u, float v, float u2, float v2);

void grDrawCursor( void )
{
	if( pCursorTexture)
		GrDrawSprite( pCursorTexture, GetMouseX()-16, GetMouseY()-16, 1.0f, 1.0f, 1.0f, 0, 0.0f, 0.0f, 32.0f, 32.0f);
}



void InitMouse(void)
{
	if( !pCursorTexture)
	{
		TextureRef temp_ref;
		temp_ref.CellColumns = 0;
		temp_ref.TotalCells = 0;
		temp_ref.CellWidth = 1.0f;
		temp_ref.CellHeight = 1.0f;

		SetTextureFormat( FT_16BIT_DATA );
		pCursorTexture = CreateTexture( &temp_ref,  32,	32,	UT_PURE_ALPHA,0 );

		HDC hDC;
		pCursorTexture->D3DSurface->GetDC( &hDC );
		DrawIcon(  hDC,     0, 0, LoadCursor( NULL, IDC_CROSS ));
		pCursorTexture->D3DSurface->ReleaseDC( hDC );

		ReSetTextureFormat();

	}


	SetCursorPos(HALF_SCREEN_CX,HALF_SCREEN_CY); // GetSystemMetrics(SM_CXSCREEN)>>1, GetSystemMetrics(SM_CYSCREEN)>>1);
	GetCursorPos(&ppCurrMouse);
	ppLastMouse = ppCurrMouse;
}

void ShutdownMouse()
{
	if( pCursorTexture )
	{
		RemoveTexture( pCursorTexture   );
		EasyFree(pCursorTexture   );
		pCursorTexture = NULL;
	}

}

//*****************************************************************************
BOOL MouseChanged()
{
	wLastMouseLB = wMouseLB;
	wLastMouseRB = wMouseRB;

	wMouseLB = GetMouseLB();
	wMouseRB = GetMouseRB();

	return (wMouseLB || wMouseRB);
}

//**************************************************************************************
void ReadMouse(void)
{
	//**********************************************************************************
	//*  The x and y values are in Screen Coordinates.  If you want them in Client
	//*  coordinates then you will probably want to copy the coordinates and pass them
	//*  through ScreenToClient(&ppXXX).  You could use ppCurrMouse, but then you
	//*  could lose track of which coordinate ppCurrMouse is in.
	//*
	//*  To check the mouse buttons you can use the GetAsyncKeyState to check.  The
	//*  defines you would use are:
	//*			VK_LBUTTON,
	//*			VK_RBUTTON,
	//*			VK_MBUTTON.
	//*
	//*  You could put this in the ReadKeyboard function if you wanted, I just didn't
	//*  want to right a fuction which had one line in it to use GetAsyncKeyState.
	//*
	//**********************************************************************************
	ppLastMouse = ppCurrMouse;
	GetCursorPos(&ppCurrMouse);
}

//**************************************************************************************
void ReadMouseRelative(void)
{
	//**********************************************************************************
	//*  The x and y values are in Screen Coordinates.  If you want them in Client
	//*  coordinates then you will probably want to copy the coordinates and pass them
	//*  through ScreenToClient(&ppXXX).  You could use ppCurrMouse, but then you
	//*  could lose track of which coordinate ppCurrMouse is in.
	//*
	//*  To check the mouse buttons you can use the GetAsyncKeyState to check.  The
	//*  defines you would use are:
	//*			VK_LBUTTON,
	//*			VK_RBUTTON,
	//*			VK_MBUTTON.
	//*
	//*  You could put this in the ReadKeyboard function if you wanted, I just didn't
	//*  want to right a fuction which had one line in it to use GetAsyncKeyState.
	//*
	//**********************************************************************************
	LONG x, y;

	x = HALF_SCREEN_CX; // GetSystemMetrics(SM_CXSCREEN)>>1;
	y = HALF_SCREEN_CY; // GetSystemMetrics(SM_CYSCREEN)>>1;
	GetCursorPos(&ppCurrMouse);
	ppLastMouse = ppCurrMouse;
	ppCurrMouse.x -= x;
	ppCurrMouse.y -= y;
	SetCursorPos(x, y);
}

//**************************************************************************************

LONG GetMouseX(void) { return ppCurrMouse.x; }
LONG GetMouseY(void) { return ppCurrMouse.y; }

short GetMouseLB(void) { return GetAsyncKeyState(VK_LBUTTON); }
short GetMouseRB(void) { return GetAsyncKeyState(VK_RBUTTON); }
short GetMouseMB(void) { return GetAsyncKeyState(VK_MBUTTON); }

LONG  GetMouseDX(void) { return (ppCurrMouse.x - ppLastMouse.x); }
LONG  GetMouseDY(void) { return (ppCurrMouse.y - ppLastMouse.y); }

//**********************************************************************************
//*  These are used only if you want to see how much the mouse has moved since it was
//*  last check AND you did not call ReadMouseRelative().  Also realize that edges
//*  of the screen can cause you problems since the mouse will stop once it hits
//*  an edge.  This is why ReadMouseRelative() resets the mouse to the center of
//*  the screen.
//**********************************************************************************