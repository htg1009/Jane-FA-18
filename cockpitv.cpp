// Virtual Cockpit Code
//


#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "F18weap.h"
#include "clouds.h"


void VirtualUFC( int slot, int offx, int offy,  char *str);
void DrawVirtualCockpit( void );
void RecacheTexture(TexturePal *walker);
void RecacheTexture(TextureBuff *walker);
void RenderMPDPopUp( float sx, float sy );



extern int DrawPopMpds;
extern int InteractCockpitMode;
extern hspot *pLastHSpot;
extern BOOL MirrorLocalLights;

extern float D3DMaxZ;
extern float ooD3DMaxZ;
extern float StupidQ;

VirtualCockpit VC;
int ufcx, ufcy;

TextureBuff *pAdiTexture;

extern BOOL GlarePoints;

extern AvionicsType Av;
extern CPitType Cpit;


ObjectHandler *pMPDPopUp;
ObjectHandler *pADIBall;

BYTE LiftLineFade = 255;

unsigned char fadevar = 44; /// add key for max
unsigned char greenfadevar = 20;

void LoadAdiTexture( void)
{
	TextureRef temp_ref;

	ZeroMemory(&temp_ref, sizeof(temp_ref));
	temp_ref.CellColumns = 0;
	temp_ref.TotalCells = 0;
	temp_ref.CellWidth = 1.0f;
	temp_ref.CellHeight = 1.0f;
	sprintf( temp_ref.Name, "band.pcx");

//	SetTextureFormat( FT_16BIT_DATA );
//	pAdiTexture = Load3DTexture(&temp_ref, UT_DEFAULT);
//	ReSetTextureFormat();
}


void LoadVirtualCockpit( BOOL force_load )
{
	if( force_load || !VC.frontseat)
	{
		ObjectHandler *fs,*plane,*tr,*tl,*js,*rp,*ab,*mpd, *cr,*ll;

		/* -----------------8/12/99 4:36PM---------------------------------------------------------------------
		/* we're gonna load first so we don't have to reload textures we're reusing
		/* ----------------------------------------------------------------------------------------------------*/
		switch (g_Settings.gr.dwGraph & (GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT |GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT| GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT))
		{
			case GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT:
			default:
				/* -----------------8/12/99 4:27PM---------------------------------------------------------------------
				/* high
				/* ----------------------------------------------------------------------------------------------------*/
				if( g_Settings.gr.dwGraph & GP_GRAPH_CANOPY_REFLECTION)
					cr = Load3DObject( RegPath("objects","virgls2.3dg"));
				else
					cr = NULL;
				fs = Load3DObject( RegPath("objects","virx05.3dg"));
				plane =  Load3DObject( RegPath("objects","f18eout9.3dg"));
				tr = Load3DObject( RegPath("objects","vtrot1.3dg"));
				tl = Load3DObject( RegPath("objects","vtrot2.3dg"));
				js = Load3DObject( RegPath("objects","vstick.3dg"));
				rp = Load3DObject(RegPath("objects","vrefuel.3dg"));
				ab = Load3DObject(RegPath("objects","virball2.3dg"));
				mpd = Load3DObject(RegPath("objects","vframe2.3dg"));
				ll = Load3DObject(RegPath("objects","strpf01.3dg"));
				break;

			case GP_GRAPH_MEDIUM_DETAIL_VIRTUAL_COCKPIT:
				/* -----------------8/12/99 4:27PM---------------------------------------------------------------------
				/* medium
				/* ----------------------------------------------------------------------------------------------------*/
				if( g_Settings.gr.dwGraph & GP_GRAPH_CANOPY_REFLECTION)
					cr = Load3DObject( RegPath("objects","virgls2.3dg"));
				else
					cr = NULL;
				fs = Load3DObject( RegPath("objects","virmed05.3dg"));
				plane =  Load3DObject( RegPath("objects","f18eoutl.3dg"));
				tr = Load3DObject( RegPath("objects","vtrot1.3dg"));
				tl = Load3DObject( RegPath("objects","vtrot2.3dg"));
				js = NULL;
				rp = Load3DObject(RegPath("objects","vrefuel.3dg"));
				ab = Load3DObject(RegPath("objects","virbalm5.3dg"));
				mpd = Load3DObject(RegPath("objects","vframe2.3dg"));
				ll = Load3DObject(RegPath("objects","strpf01.3dg"));
				break;

			case GP_GRAPH_LOW_DETAIL_VIRTUAL_COCKPIT:
				/* -----------------8/12/99 4:27PM---------------------------------------------------------------------
				/* low
				/* ----------------------------------------------------------------------------------------------------*/
				if( g_Settings.gr.dwGraph & GP_GRAPH_CANOPY_REFLECTION)
					cr = Load3DObject( RegPath("objects","virgls2.3dg"));
				else
					cr = NULL;
				fs = Load3DObject( RegPath("objects","virlow06.3dg"));
				plane =  Load3DObject( RegPath("objects","f18eoutx.3dg"));
				tr = NULL;
				tl = NULL;
				js = NULL;
				rp = Load3DObject(RegPath("objects","vrefuel.3dg"));
				ab = Load3DObject(RegPath("objects","virbalm5.3dg"));
				mpd = Load3DObject(RegPath("objects","vframe2.3dg"));
				ll = Load3DObject(RegPath("objects","strpf01.3dg"));
				break;
		}

		if (VC.frontseat)
			CleanupVirutalCockpit();

		VC.CanopyReflection = cr;
		VC.frontseat = fs;
		VC.airplane =  plane;
		VC.ThrottleRight = tr;
		VC.ThrottleLeft = tl;
		VC.JoyStick  = js;
		VC.refueling_probe = rp;
		VC.LiftLine = ll;
		pADIBall = ab;
		pMPDPopUp = mpd;
	}
}

void CleanupVirutalCockpit( void )
{
//	if( pAdiTexture )
//	{
//		Free3DTexture( pAdiTexture );
//		pAdiTexture = NULL;
//	}

	if( VC.CanopyReflection)
		Free3DObject( VC.CanopyReflection);

	if( VC.frontseat)
		Free3DObject( VC.frontseat );

	if (VC.airplane)
		Free3DObject(VC.airplane);

	if( VC.ThrottleRight)
		Free3DObject( VC.ThrottleRight );

	if( VC.ThrottleLeft)
		Free3DObject( VC.ThrottleLeft );

	if( VC.JoyStick)
		Free3DObject( VC.JoyStick );

	if (VC.refueling_probe)
		Free3DObject(VC.refueling_probe);

	if (VC.LiftLine)
		Free3DObject(VC.LiftLine);

	if( pMPDPopUp )
		Free3DObject(pMPDPopUp );

	if( pADIBall )
		Free3DObject(pADIBall);

	VC.frontseat= NULL;
	VC.JoyStick	    = NULL;
	VC.ThrottleRight= NULL;
	VC.ThrottleLeft = NULL;
	VC.airplane = NULL;
	VC.refueling_probe = NULL;
	VC.CanopyReflection = NULL;
	VC.LiftLine = NULL;
	pMPDPopUp = NULL;
	pADIBall= NULL;
}


void UpdateVirtualMpd( void )
{
	if( !(CurrentView & COCKPIT_VIRTUAL_SEAT ) )
		return;
}


void DisplayCurrentUFCTextVirtual( void )
{
	int i;
	int x, y;


	LedPtr = &UFCLed[0];
	LastLedPtr = &UFCLed[9];
	i=0;
	while (LedPtr <= LastLedPtr)
	{
		int Len = GrStrLen(LedPtr->Text, LgLEDFont);

		switch( LedPtr->JustifyType)
		{
			case LEFT_JUSTIFY:
				x = 2;//LedPtr->XOfs[LedPtr->JustifyType];
				//y = LedPtr->YOfs-LgLEDFont->iMaxFontHeight+4;
				break;

			case  CENTERED:
				//x = LedPtr->XOfs[LedPtr->JustifyType]-(Len/2);
				x = 64-(Len/2);
				//y = LedPtr->YOfs-LgLEDFont->iMaxFontHeight+4;
				break;

			case RIGHT_JUSTIFY:
				x = 126-Len;//LedPtr->XOfs[LedPtr->JustifyType]
				//y = LedPtr->YOfs-LgLEDFont->iMaxFontHeight+4;
				break;
		}
		y=0;

		if( i<5)
			VirtualUFC( i, x, y, LedPtr->Text);
		else
			VirtualUFC( 9-i, x, y, LedPtr->Text);
		LedPtr++;
		i++;
	}

}

void ClearUFCDisplaysVirtual( void )
{
//	GrFillRectNoClip(VC.UFC->Texture, ufcx, ufcy, 127, 95,	1);
	//GrFillRectNoClip(VC.UFC->Texture, ufcx, ufcy +12*1, 126, 11,	1);
	//GrFillRectNoClip(VC.UFC->Texture, ufcx, ufcy +12*2, 126, 11,	1);
	//GrFillRectNoClip(VC.UFC->Texture, ufcx, ufcy +12*3, 126, 11,	1);
	//GrFillRectNoClip(VC.UFC->Texture, ufcx, ufcy +12*4, 126, 11,	1);
	//GrFillRectNoClip(VC.UFC->Texture, ufcx, ufcy +12*5, 126, 11,	1);
}


void VirtualUFC( int slot, int offx, int offy,  char *str)
{
	int x, y;

	x = ufcx;
	y = ufcy +12*slot;

//	GrDrawStringClipped( VC.UFC->Texture, LgLEDFont, x+offx, y+offy, str, 3);

}


void RenderVirtualHandArm( void )
{
	FPoint  pos;
	FPoint HeadOffset;

	HeadOffset.SetValues(VC.xdelta,VC.ydelta,VC.zdelta);
	HeadOffset *= 100.0f;


	if( pADIBall)
	{

		BYTE vars[256];
		ZeroMemory( &vars[0], sizeof(vars));

		// Calc Night Glow Color
		float glow_light = (255.0f - (float)DaylightLeft)/255.0f;
		glow_light *= 1.25;
		if (glow_light > 1.0f)
			glow_light = 1.0f;
		glow_light *= 255.0f;

		glow_light = 255.0f * 0.75f;

		vars[64] =  (unsigned char)glow_light;

		pos.SetValues( 42.312f FEET, -113.801f FEET, -132.014f FEET);
		pos += HeadOffset;
		pos *= PlayerPlane->Attitude;

		FMatrix tmp;
		FMatrix tmp2;

		tmp2.SetRPH(  0, -7828, 0);  //43 deg= 65536/360
		//tmp2.SetRPH(  0, -0x1000, 0);
		tmp.SetRPH(  -PlayerPlane->Roll, PlayerPlane->Pitch, 0);
		tmp *= tmp2;
		tmp *= PlayerPlane->Attitude;

		Draw3DObjectMatrix( pADIBall, pos, tmp, vars, NULL );
	}


	if (VC.ThrottleLeft)
	{
		pos.SetValues( (-88.337 + 5.625)  FEET , -136.458 FEET, -51.552 FEET);		//These numbers are from the model
		pos.Z += ( 40.274f FEET ) * ((50.0f - PlayerPlane->RightThrottlePos )*0.01f );//LeftThrottlePos
		pos += HeadOffset;
		pos *= PlayerPlane->Attitude;
		Draw3DObjectMatrix( VC.ThrottleLeft, pos, PlayerPlane->Attitude, NULL, NULL );
	}

	if (VC.ThrottleRight)
	{
		pos.SetValues( (-88.337 - 5.625)FEET, -136.458 FEET, -51.552 FEET);
		pos.Z += ( 40.274f FEET ) * ((50.0f - PlayerPlane->LeftThrottlePos)*0.01f );//RightThrottlePos
		pos += HeadOffset;
		pos *= PlayerPlane->Attitude;
		Draw3DObjectMatrix( VC.ThrottleRight, pos, PlayerPlane->Attitude, NULL, NULL );
	}


	if (VC.JoyStick)
	{
		pos.SetValues( 0.0f FEET, (-140.0f FEET) * 1.25, (-34.0f FEET)* 1.25);  //0.75 scale it down a bit
		pos.X += 0.50f * (PlayerPlane->StickX/128.0f);
		pos.Z += 0.50f * (PlayerPlane->StickY/128.0f);
		pos += HeadOffset;
		pos *= PlayerPlane->Attitude;
		Draw3DObjectMatrix( VC.JoyStick, pos, PlayerPlane->Attitude, NULL, NULL );
	}

}




extern BYTE FormationLightColors[];
extern SubObjectHandler SourceSubs[];
extern SubObjectHandler OurSubs[];
SubObjectHandler *GetSubObjects(PlaneParams *P);

float AirspeedAngle( void)
{
	float Airspeed = PlayerPlane->IndicatedAirSpeed;

	if(Airspeed > 800.0) Airspeed = 800.0;
	if(Airspeed <= 300)
		return( ((Airspeed/300.0)*191.25) ); // 191.25 is 3/4's 255
	else
		return( 191.5 + ( ((Airspeed - 300.0)/500.0)*63.75) ); // 63.75 = 1/4 of 255
}

float HourHandAngle( void)
{
	float Hours   = WorldParams.WorldTime* (1.0f/3600.0f);

	if(Hours >= 24.0)
		Hours = 0.0;
	if(Hours > 12.0)
		Hours = Hours - 12.0;

	return ( (Hours* (1.0f/12.0)) * 255.0 );
}

float MinuteHandAngle( void)
{
	float Minutes = ( fmod(WorldParams.WorldTime,3600.0f) )/60.0;

	if(Minutes >= 60.0)
		Minutes = 0.0;
	return(  (Minutes/60.0)*255.0 );
}

float VertVelAngle( void)
{
	// vertical vel. gauge
	float VertVel,VertVelAngle;
	VertVel = PlayerPlane->IfVelocity.Y * (60.0);
	if(VertVel > 5000.0) VertVel = 5000.0;
	if(VertVel < -5000.0) VertVel = -5000.0;

	VertVelAngle = (VertVel/5000.0)*127.5; // 127.5 = 255/2
	VertVelAngle = 191.5 + VertVelAngle;
	if(VertVelAngle > 255.0)
		VertVelAngle = VertVelAngle - 255.0;
	else if(VertVelAngle < 0)
		VertVelAngle = 255.0 + VertVelAngle;

	return VertVelAngle;
}

int GetAOAState( void )
{
	int state = 0;
	PlaneParams *P = PlayerPlane;

	if((P->FlightStatus & PL_GEAR_DOWN_LOCKED) && !P->OnGround)
	{
		float AOA = RadToDeg(P->Alpha);
		if( AOA <90.0f && AOA >9.3f)
			state = 1;
		else
		if( AOA <9.3f && AOA >8.8f)
			state = 2;
		else
		if( AOA <8.8f && AOA>7.4f)
			state = 3;
		else
		if( AOA <7.4f && AOA>6.9f)
			state = 4;
		else
		if( AOA <6.9f && AOA>0.0f)
			state = 5;
	}
	return state;
}




void RenderVirtualCockpit( void)
{
	BYTE vars[256];//, *pVar;
	PlaneParams *P = PlayerPlane;
	SubObjectHandler *swalker;
	float old_max_z;
	float *old_light_walker,old_light_values[512];
	FPoint plane_up;

	// Virtual has 2 models.  Interior and Exerior.

	//--------------------------------------------------------------
	//Exterior Vars-----------------------------------------------------
	//--------------------------------------------------------------

	ZeroMemory( &vars[0], sizeof(vars));

	// Don't Know What these Are?
	vars[20] = FrameCount & 1;
	vars[21] = FrameCount % 3;
	vars[22] = FrameCount & 3;



	//	case PT_F18e:
	/* Tail SJ designation for Seymor Johnson */
	vars[40] = 3;

	/* Tail color swatch */
	vars[46] = 5;

	/* Tail numbers */
	vars[41] = ((P-Planes)/2)%4+10;
	vars[42] = ((int)P%15)%10;
	vars[44] = ((int)P%72)%10;
	vars[43] = (vars[44]+3)%10;
	vars[45] = (vars[42]+7)%10;

	/* formation lights off */
	 vars[47] = FormationLightColors[(P->Status & PL_FORMATION_LIGHTS) >> 12];


	// No Landing gear in VC
	// control surfaces and after burners are the same on all planes */

	// burners
	if (P->LeftThrustPercent > 80.0f)
	{
		vars[7] = 1;
		vars[5] = (BYTE)(255.0f - ((100.0f - P->LeftThrustPercent) * 255.0f/20.0f));
	}
	else
		vars[7] = 0;

	if (P->RightThrustPercent > 80.0f)
	{
		vars[8] = 1;
		vars[6] = (BYTE)(255.0f - ((100.0f - P->RightThrustPercent) * 255.0f/21.0f));
	}
	else
		vars[8] = 0;

	/* speed brake */
	// vars[10] = (BYTE)((1.0f-(P->SpeedBrake*1.0f/90.0f))*(float)0xFF);
	vars[10] = (P->SpeedBrakeState);

	/* turkey flaps */
	vars[15] = 0x0080 - (int)((P->SymetricalElevator + P->DifferentialElevator)*((float)0x00FF/180.0));
	vars[16] = 0x0080 - (int)((P->SymetricalElevator - P->DifferentialElevator)*((float)0x00FF/180.0));

	/* ailerons */
	vars[12] = 0xFF-(BYTE)(255.0 * (P->Aileron  - P->CS->AileronRange.Low)/(P->CS->AileronRange.High - P->CS->AileronRange.Low));
	vars[18] = 0xFF-(BYTE)(255.0 * ( 1.0f - (P->Aileron - P->CS->AileronRange.Low)/(P->CS->AileronRange.High - P->CS->AileronRange.Low)));

	/*TODO: Ask Scott if this is ok..
	int TmpAileron1 = (P->Aileron - P->SBAileronOffset+P->FLPSAileronOffset);
	if (TmpAileron1 > P->CS->AileronRange.High) TmpAileron1 = P->CS->AileronRange.High;
	else
		if (TmpAileron1 < P->CS->AileronRange.Low) TmpAileron1 = P->CS->AileronRange.Low;

	int TmpAileron2 = (-P->Aileron - P->SBAileronOffset+P->FLPSAileronOffset);
	if (TmpAileron2 > P->CS->AileronRange.High) TmpAileron2 = P->CS->AileronRange.High;
	else
		if (TmpAileron2 < P->CS->AileronRange.Low) TmpAileron2 = P->CS->AileronRange.Low;

	vars[12] = 0xFF-(BYTE)(255.0 * (TmpAileron1 - P->CS->AileronRange.Low)/(P->CS->AileronRange.High - P->CS->AileronRange.Low));
	vars[18] = 0xFF-(BYTE)(255.0 * (TmpAileron2 - P->CS->AileronRange.Low)/(P->CS->AileronRange.High - P->CS->AileronRange.Low));

*/





	/* Flaps */
	vars[11] = (BYTE)(255.0 * (P->Flaps - P->CS->FlapRange.Low)/(P->CS->FlapRange.High - P->CS->FlapRange.Low));

	// leading edge extensions
	vars[17] = (BYTE)(255.0 * (P->LEFlaps - P->CS->LEFlapRange.Low)/(P->CS->LEFlapRange.High - P->CS->LEFlapRange.Low));

	/* Rudder */
	vars[13] = 0xFF-(BYTE)(255.0 * (P->Rudder - P->CS->RudderRange.Low)/(P->CS->RudderRange.High - P->CS->RudderRange.Low));

	if (P->LandingGearStatus || (P->FlightStatus & PL_GEAR_DOWN_LOCKED))
	{
		/* Lights */
		vars[50] = 0xF9; /* green light */
		vars[51] = 0xF8; /* red light */
		if ((GameLoop + (int)P) &  0x20)
			vars[52] = 0xF8;
		else
			vars[52] = 0xA9;
		vars[53] = 0xFD; /* white red light */
		if ((P->Attitude.m_Data.RC.R0C0*ViewVector.X + P->Attitude.m_Data.RC.R1C0*ViewVector.Y + P->Attitude.m_Data.RC.R2C0*ViewVector.Z) < 0.0)
			vars[54] = 0xF9;
		else
			vars[54] = 0xF8;
		vars[1] = 0;
	}
	else
	{
		if (P->Status & PL_RUNNING_LIGHTS)
		{
			vars[50] = 0xF9; /* green light */
			vars[51] = 0xF8; /* red light */
			if ((GameLoop + (int)P) &  0x20)
				vars[52] = 0xF8;
			else
				vars[52] = 0xA9;
			vars[53] = 0xFD; /* white red light */
			if ((P->Attitude.m_Data.RC.R0C0*ViewVector.X + P->Attitude.m_Data.RC.R1C0*ViewVector.Y + P->Attitude.m_Data.RC.R2C0*ViewVector.Z) < 0.0)
				vars[54] = 0xF9;
			else
				vars[54] = 0xF8;
		}
		else
		{
			vars[50] = 0x6C;//xDD; /* green light */
			vars[51] = 0xA9;//xDD; /* red light */
			vars[52] = 0xA9;//xDD; /* flash red light */
			vars[53] = 0xCB;//xDD; /* white red light */
			vars[54] = 0;
		}
		vars[ 1] = 0xFF;
	}



	//vars[64] = (P->WeapLoad[RIGHT11_STATION].Count) ? 0 : 1;
	//vars[65] = (P->WeapLoad[LEFT1_STATION].Count) ? 0 : 255;
	if(P->WeapLoad[RIGHT11_STATION].Count)
	{
		if(pDBWeaponList[P->WeapLoad[RIGHT11_STATION].WeapIndex].lWeaponID == 109)
			vars[64] = 1;
		else
			vars[64] = 2;
	}
	else
		vars[64] = 0;

	if(P->WeapLoad[LEFT1_STATION].Count)
	{
		if(pDBWeaponList[P->WeapLoad[LEFT1_STATION].WeapIndex].lWeaponID == 109)
			vars[65] = 1;
		else
			vars[65] = 2;
	}
	else
		vars[65] = 0;






	if ((g_Settings.gr.dwGraph & (GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_WEAPONS_ON_PLANE)) == (GP_GRAPH_HIGH_DETAIL_VIRTUAL_COCKPIT | GP_GRAPH_WEAPONS_ON_PLANE))
		GetSubObjects(P);
	else
		memcpy(OurSubs,SourceSubs,sizeof(SubObjectHandler)*MAX_SUBPARTS);

	PlaneType *type = P->Type;

	swalker = OurSubs;

	for (int i=0; i<MAX_CONNECTED_PARTS; i++)
	{
		swalker->Object  = type->SubParts[i];
		swalker->AllVars = vars;
		swalker->Flags = SO_SCALE_IN_BY_10;
		swalker++;
	}

	while(i++ < MAX_SUBPARTS)
	{
		if ((i != 13) && (i!= 10))
			swalker->Flags = SO_SCALE_IN_BY_10;
		swalker++;
	}



	void SetScale(float new_scale);
	SetScale(100.0f);
	ZClipPlane = (-1.0f/12.0f) FEET;
	old_max_z = D3DMaxZ;
	D3DMaxZ = (-150.0f FEET)/0.9999f;
	ooD3DMaxZ = 1.0f/D3DMaxZ;
	StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);



	if (VC.airplane)
		Draw3DObjectMatrix( VC.airplane, VC.HeadPosition, PlayerPlane->Attitude,vars, OurSubs );

	if (VC.LiftLine && (g_Settings.gr.dwGraph & GP_GRAPH_LIFT_LINE))
		Draw3DObjectMatrix( VC.LiftLine, VC.HeadPosition, PlayerPlane->Attitude,&LiftLineFade);

	if((PlayerPlane->RefuelProbeCommandedPos > 1.0f) && VC.refueling_probe)
	{
		FPoint pos;
		pos.SetValues(VC.xdelta,VC.ydelta,VC.zdelta);
		pos *= 100.0f;
		pos *= PlayerPlane->Attitude;
		Draw3DObjectMatrix( VC.refueling_probe, pos, PlayerPlane->Attitude,NULL, NULL );//VC.HeadPosition
	}

	if( VC.CanopyReflection && g_Settings.gr.dwGraph & GP_GRAPH_CANOPY_REFLECTION)
	{
		vars[0] = fadevar;
		vars[1] = greenfadevar;  //  0..16 sunset 16..22 night
#ifdef _DEBUG
		//char	szTxt[320];
		//sprintf( szTxt, "FadeVar: %d", fadevar);
		//GrDrawStringClipped( GrBuffFor3D, LgHUDFont, 50, 50, szTxt, 1.0, 0.0f, 0.0f, 0, 1.0f );
		//sprintf( szTxt, "GreenFadeVar: %d", greenfadevar);
		//GrDrawStringClipped( GrBuffFor3D, LgHUDFont, 50, 80, szTxt, 1.0, 0.0f, 0.0f, 0, 1.0f );

#endif

		MirrorLocalLights = TRUE;
		Draw3DObjectMatrix( VC.CanopyReflection, VC.HeadPosition, PlayerPlane->Attitude, vars, NULL );//VC.HeadPosition
		MirrorLocalLights = FALSE;

	}

	//--------------------------------------------------------------
	// Interior-----------------------------------------------------
	//--------------------------------------------------------------


	//Interior of plane, may want to change the lighting.
	ZeroMemory( &vars[0], sizeof(vars));


	vars[9] = (int)PlayerPlane->ThrottlePos << 1;  //throttle rotation [0..200]

 	// calcs for needles

	// alt needle
	float AltSmallAngle,AltLargeAngle;//,AltTickAngle;
	float AltSmall,AltLarge;//,AltTick;

	AltSmall = ((int)PlayerPlane->Altitude) % 1000;
	AltLarge = PlayerPlane->Altitude/1000.0;
	AltLarge = ((int)AltLarge % 10);

	AltSmallAngle = (AltSmall/1000.0)*255.0;
	AltLargeAngle = (AltLarge*25.5) + ( (AltSmall/1000.0)*25.5);


	// fake..for now its a constant. later we need to add code to "jiggle" the needle for effect  jlm
	float FakeAngle = 11;

	vars[122] = Cpit.ShootVar;
	vars[121] = Cpit.LockVar;


	// Gagues Lower Right F18
	vars[16] = AirspeedAngle();			// Air Speed
	vars[17] = AltSmallAngle;			// Large Needle			//Altimeter
	vars[18] = AltLargeAngle;			// Small Needle
	vars[19] = 11;						// Tick
	vars[20] = VertVelAngle();			// wind speed

	vars[21] = MinuteHandAngle();
	vars[22] = HourHandAngle();			// clock


	vars[220] = Cpit.NoseVar;
	vars[221] =	Cpit.LeftVar;
	vars[222] =	Cpit.RightVar;
	vars[223] =	Cpit.FlapsVar;
	vars[224] = Cpit.HalfVar;
	vars[225] = Cpit.FullVar;

	// Left Top Lights F18
	vars[228] = Cpit.LBleedVar;
	vars[229] = Cpit.SpeedBrakeVar;
	vars[230] = Cpit.BrakeVar;
	vars[231] = Cpit.LBar;
 	vars[232] = Cpit.RBleedVar;
	vars[233] = Cpit.StbyVar;
	vars[234] = Cpit.RecVar;
	vars[235] = Cpit.XmitVar;
	vars[236] = Cpit.APVar;
	vars[242] = Cpit.DispVar;

	vars[251] = Cpit.CSelVar;
	vars[252] = Cpit.HSelVar;


	// Right Top Lights F18
	vars[237] = Cpit.LaserVar;				// Laser (rcdr on)
	vars[238] = Cpit.DecoyDeployVar;				// Dply
	vars[239] = PlayerPlane->TailHookState;			// Tailhook
	vars[240] = Cpit.AIVar;				// ai
	vars[241] = 0;				// cw
	vars[243] = Cpit.DecoyOnVar;				// dcoy on
	vars[244] = Cpit.SAMVar;				// sam
	vars[245] = Cpit.AAAVar;				// aaa
	vars[248] = Cpit.LeftFireReadyVar;
	vars[250] = GetAOAState();		// AOA Indexer

	//if( P->FlightStatus & PL_GEAR_DOWN_LOCKED)
	//	if( P->LandingGear >= 0x7F )
	//{
	//	vars[253] = 1;
	//	SetCockpitItemState(253,1);
//	}



	GetCockpitItemVars( (char *)&vars[0]);			//JLM keeps track of several buttons.





	// Calc Night Glow Color
	float glow_light = (255.0f - (float)DaylightLeft)/255.0f;
	glow_light *= 1.25;
	if (glow_light > 1.0f)
		glow_light = 1.0f;
	glow_light *= 255.0f;

	glow_light = 255.0f * 0.75f;


	vars[64] =  (unsigned char)glow_light;
	vars[65] =  (unsigned char)glow_light;

	if( !DrawPopMpds)
	{
		if(  pLastHSpot && pLastHSpot->Var>=128 && pLastHSpot->Var<=187)
			vars[pLastHSpot->Var] = 1;
		pLastHSpot = NULL;
	}


	vars[116] = 1; //MAX TEST For turning off polys on f6 zoom

	/* -----------------6/11/99 11:51AM--------------------------------------------------------------------
	 * dim the light in the cockpit so things are less bright.
	 * ----------------------------------------------------------------------------------------------------*/
	LightSource** light_walker;
	light_walker = GlobalLights;
	old_light_walker = old_light_values;

	plane_up.SetValues(-1.0,PlayerPlane->Orientation.K);

	while(light_walker <= LastLight)
	{
		if ((*light_walker))
		{
			*old_light_walker++ = (*light_walker)->Intensity;

			if ((PlayerPlane->ABLight == (PointLightSource **)light_walker) || (PlayerPlane->MachineGunLight == (PointLightSource **)light_walker) || (PlayerPlane->FuelProbeLight == (PointLightSource **)light_walker))
				(*light_walker)->Intensity *= 0.05f;

			(*light_walker)->Attenuate(plane_up,PlayerPlane->WorldPosition);
		}

		light_walker++;
	}


	GlarePoints = FALSE;

	Draw3DObjectMatrix( VC.frontseat, VC.HeadPosition, PlayerPlane->Attitude,vars, NULL );

	SetScale(1.0f);
	ZClipPlane = -1.9f;
	D3DMaxZ = old_max_z;
	ooD3DMaxZ = 1.0f/D3DMaxZ;
	StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);
	RenderVirtualHandArm( );

	light_walker = GlobalLights;
	old_light_walker = old_light_values;

	while(light_walker <= LastLight)
	{
		if ((*light_walker))
			(*light_walker)->Intensity = *old_light_walker++;

		light_walker++;
	}

	GlarePoints = TRUE;

}


extern int HudLeft;
extern int HudRight;

extern int HudTop;
extern int HudBottom;

extern int TranX, TranY;


//void DrawAdiBall( void )
//{
//	int segs = 8;
//	FPoint Position, RelPosition;
//	float alpha= 1.0f;
//	FPoint diffuse;
//	FMatrix Mat;
//	float radius = 9.5f;//8.5f;//17;
//	int gop =  PRIM_TEXTURE ;

//	static short i = 0x0;
//	i+= 0x100;

//	Mat.Identity();
//	Mat.SetHPR( (ANGLE)0, (ANGLE)0x2000, (ANGLE)0x4000);

//	Mat = PlayerPlane->Attitude;//ViewMatrix;

//	Position.SetValues(42.312, -113.801, -132.014 ); // from Max and Virtual Model

//	SetUVScaleOffset( 1.0f, 0.0f, 1.0f, 0.0f);	//reset

//	BuildSphere(  segs );

//	RelPosition =  Position;
//	RelPosition *= PlayerPlane->Attitude;
//	RelPosition += VC.HeadPosition;


//	CurrentTexture  = pAdiTexture;

//	diffuse.SetValues( 0.0f, 0.95f, 0.20f);
//	if( RadiusInView( RelPosition, radius) )
//		DrawSpherePreBuilt( radius, 1.0f, RelPosition, alpha, diffuse, gop, &Mat  );

//}


void DrawVirtualCockpit( void )
{

	// Set Lighting
	FLIRObjectIntensity = 1.0f;

	ShortenFPU();


	RenderVirtualCockpit( );


	RestoreFPU();


}

void CheckVirtualMouse( void )
{
	InteractCockpitMode = 0;

	if(iEndGameState)
	{
		OurShowCursor( TRUE );
	}
	else if( GetMouseRB() && !DrawPopMpds)
	{
		OurShowCursor( FALSE );

		ReadMouseRelative();

		if (g_Settings.misc.dwMisc & GP_MISC_VIEW_REVERSE_YAW)
			VC.HeadingOffset += (GetMouseX()<<4);
		else
			VC.HeadingOffset -= (GetMouseX()<<4);

		if (g_Settings.misc.dwMisc & GP_MISC_VIEW_REVERSE_PITCH)
			VC.PitchOffset += (GetMouseY()<<4);
		else
			VC.PitchOffset -= (GetMouseY()<<4);

	}
	else //if( GetMouseRB())//InteractCockpitMode)
	{
		InteractCockpitMode = 1;
		OurShowCursor( TRUE );
		ReadMouse();
	}

}




