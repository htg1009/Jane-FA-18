#ifndef __HUD_H
#define __HUD_H

//////////////////////////
//					    //
//    Icon Structure    //
//						//
//////////////////////////

typedef struct _GrIcon
{
	GrBuff *buff;
	int		offX, offY;	// T/L corner in PCX
	int		endX, endY;	// B/R corner in PCX
	int		hotX, hotY;	// hot-spot offset
	int		wide, high;	// calc'd true size
	TextureBuff *pTexture;		// Texture

} GrIcon;


#define FTSEC_TO_MLHR	(3600.0/5280.0)
#define MLHR_TO_KNOTS	(0.86897624)

#define FTSEC_TO_KNOTS	(3600.0/NMTOFT)

#define NUM_PITCH_LADDERS	72

typedef struct _HUDLadderType
{
	FPoint	RawHUDPoint[6];
	RotPoint3D HUDPoints[6];
	int PitchNum;

} HUDLadderType;

/*
#define HUD_TOP		 62	//51
//#define HUD_TOP		 42
#define HUD_LEFT	229	//	231	//179
#define HUD_BOTTOM	222	//236
//#define HUD_BOTTOM	257
#define HUD_RIGHT	410	//461
#define HUD_MIDY	116
#define HUD_MIDX	320
*/

// These are for 640x480  They get mapped to other resolutions
#define HUD_LEFT	232
#define HUD_TOP		 60
#define HUD_RIGHT	410	
#define HUD_BOTTOM	231
#define HUD_MIDY	116
#define HUD_MIDX	320



#define AIM_9P	    41
#define AIM_9L	    36
#define AIM_9M	    37
#define AIM_7F	    34
#define AIM_7M	    35
#define AIM_120A	38

#define SMALLHUD 0
#define LARGEHUD 1

#define MAX_NUM_MODES 4

#define NAV_MODE  0
#define AA_MODE   1
#define AG_MODE   2
#define INST_MODE 3

void GrCopyRectMask8ColorCmpVal(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color, int CmpVal);
void GrCopyRectMaskColor(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color);
void GrCopyRectMask8Color(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh, int Color);
void GrCopyRectMask8Fast(GrBuff *dest,int DDx,int DDy, GrBuff *source, int SSx,int SSy,int SSw,int SSh);
void DisplayPitchLadder(PlaneParams *P);
void DisplayHeadingBar(PlaneParams *P);
inline void DisplayGunCross(void);
inline void DisplayWaterMark(void);
inline void DisplayInCmd(void);
void DisplayAltitude(PlaneParams *P);
void DisplayIAS(PlaneParams *P);
void DisplayAOA(PlaneParams *P);
void DisplayMach(PlaneParams *P);
void DisplayGForce(PlaneParams *P);
void DisplayVelocityVector(PlaneParams *P);
void SetHUDClipWindow();
void UndoHUDClipWindow();
void InitHUD(void);
void DisplayNavHUD(PlaneParams *P);
void DisplayHUD(PlaneParams *P);
void CleanupHUDSprites(void);
void DebugPoint(void);
int  CheckInHUD(int Mx, int My);
void DesignateAGPoint(PlaneParams *P, int Sx, int Sy);

void DoAGCDIPManMode(PlaneParams *P);
void DoAGAutoMode(PlaneParams *P);
void DoAGAutoLoftMode(PlaneParams *P);
void DoAGGunsMode(PlaneParams *P);

float AAClosureRate( PlaneParams *P, PlaneParams  *T );
float AGClosureRate( PlaneParams *P, FPointDouble &T );

int  AGDesignateInCone(PlaneParams *P);
void DisplayTGTDataBlock(PlaneParams *P);
void DisplayNAVDataBlock(PlaneParams *P);
void DisplayAASearchModeSymbology(PlaneParams *P);

void DisplayBankAngle(PlaneParams *P);
int CheckDesignateInHUD( int PosX, int PosY );

void SetBrokeLockHudTimer(float Time,FPointDouble LastKnownPos); // puts target in "memory" TD box will use this current position
void ResetBrokeLockHudTimer(); // puts target in "memory" TD box will use this current position

void SetHudTDInfo(FPointDouble Loc,int Type);


/*-----------------------------------------------------------------------------
 *
 *	hypot()
 *
 *		Calculate the hypotenuse of a 2D or 3D line inline and fast.
 *
 */
inline float hypot( float xx, float yy, float zz=0 )
{
	return sqrt( xx*xx + yy*yy + zz*zz );
}

inline double hypot( double xx, double yy, double zz=0 )
{
	return sqrt( xx*xx + yy*yy + zz*zz );
}

inline int hypot( int xx, int yy, int zz=0 )
{
	int rval;
	_asm
	{
		fild  xx
		fmul  st,st
		fild  yy
		fmul  st,st
		fadd
		fild  zz
		fmul  st,st
		fadd
		fsqrt
		fistp rval
	}
	return rval;
}

/*-----------------------------------------------------------------------------
 *
 *	round()
 *
 *		Round a float/double to nearest/even integer.
 *
 */
inline int round( float xx )
{
	int rval;
	_asm
	{
		fld		xx
		fistp	rval
	}
	return rval;
}

inline int round( double xx )
{
	int rval;
	_asm
	{
		fld		xx
		fistp	rval
	}
	return rval;
}

/*-----------------------------------------------------------------------------
 *
 *	sincosA()
 *
 *		Calc both SIN and COS of input ANGLE
 *
 */
inline void sincosA( double *dSin, double *dCos, ANGLE angle )
{
	double radian = ANGLE_TO_RADIANS( angle );

	_asm {
		fld   radian
		fsincos
		mov   eax,dCos
		fstp  qword ptr [eax]
		mov   eax,dSin
		fstp  qword ptr [eax]
	}
}

inline void sincosA( float *dSin, float *dCos, ANGLE angle )
{
	double radian = ANGLE_TO_RADIANS( angle );

	_asm {
		fld   radian
		fsincos
		mov   eax,dCos
		fstp  dword ptr [eax]
		mov   eax,dSin
		fstp  dword ptr [eax]
	}
}

inline void sincos( double *dSin, double *dCos, double radian )
{
	_asm {
		fld   radian
		fsincos
		mov   eax,dCos
		fstp  qword ptr [eax]
		mov   eax,dSin
		fstp  qword ptr [eax]
	}
}

void ToggleAim7Scan();
void ToggleAim120Scan();

extern GrIcon HUDsym[];

extern int HUDColor;
// Hud prims  hudprims.cpp
void HUDtextSm( int X, int Y, PCSTR fmt, ... );
void HUDtextLg( int X, int Y, PCSTR fmt, ... );
int HUDclip( int xPos, int yPos );
void HUDline( int X1, int Y1, int X2, int Y2, int color=HUDColor );
void HUDbox( int X1, int Y1, int X2, int Y2, int color=HUDColor );
void HUDdash( int X1, int Y1, int X2, int Y2, int color=HUDColor );
void HUDcircle( int X, int Y, int R, int color=HUDColor );
void HUDcircleDash( int X, int Y, int R, int color=HUDColor );
void HUDicon( int X, int Y, int icon, int skewX=0, int skewY=0, int color=HUDColor );
void HUD_icon( int X, int Y, int icon, int skewX=0, int skewY=0, int color=HUDColor );
void HUDiconCmp( int X, int Y, int icon, int value, int color=HUDColor );
void CRTline( int X1, int Y1, int X2, int Y2, int color=HUDColor );
void CRT_line( int X1, int Y1, int X2, int Y2, int color=HUDColor );
void CRTbox( int X1, int Y1, int X2, int Y2, int color=HUDColor );
void CRTcircle( int X, int Y, int R, int color=HUDColor );
void CRTicon( int X, int Y, int icon, int skewX=0, int skewY=0, int color=HUDColor );
void CRT_icon( int X, int Y, int icon, int skewX=0, int skewY=0, int color=HUDColor );
void CRTiconCmp( int X, int Y, int icon, int value, int color=HUDColor );
void ClipToHUD( int *X, int *Y );


void GrDrawSprite( TextureBuff *pTexture,int x, int y, float  r, float g, float b, int orientation, float u, float v, float uwidth, float vwidth);
void GrDrawSpritePolyBuf( GrBuff *dest, TextureBuff *pTexture, float x, float y, float r, float g, float b, int orientation);


#endif