#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "SkunkFF.h"

RotPoint3D ArrowPoints[4];
RotPoint3D *lpArrowPoints[4] = {&ArrowPoints[0],&ArrowPoints[1],&ArrowPoints[2],&ArrowPoints[3]};
int DestructoMessage = 0;


extern CameraInstance OldCamera;
extern int OldCockpitSeat, OldCurrentCockpit;
extern AvionicsType Av;
extern TextureBuff *pArrowTexture;
extern BOOL NoFogging;

extern AvRGB AvPalette[AV_PALETTE_MAX_COLORS][AV_PALETTE_NUM_SHADES];
int PerspectPointWithAllFlags(RotPoint3D *point);


//*****************************************************************************************************************************************
void InitDirectionArrow()
{
//    //memset(DirArrow2D,0,sizeof(FPoint)*14);
//	DirArrow2D[0].X = -9;  DirArrow2D[0].Z = 47;
//	DirArrow2D[1].X = -9;  DirArrow2D[1].Z = 23;
//	DirArrow2D[2].X = -23; DirArrow2D[2].Z = 23;
//	DirArrow2D[3].X = 0;   DirArrow2D[3].Z = 0;
//	DirArrow2D[4].X = 23;  DirArrow2D[4].Z = 23;
//	DirArrow2D[5].X = 9;   DirArrow2D[5].Z = 23;
//	DirArrow2D[6].X = 9;   DirArrow2D[6].Z = 47;
//	DirArrow2D[7].X = -8;  DirArrow2D[7].Z = 47;
//	DirArrow2D[8].X = -8;  DirArrow2D[8].Z = 22;
//	DirArrow2D[9].X = -20; DirArrow2D[9].Z = 22;
//	DirArrow2D[10].X = 0;  DirArrow2D[10].Z = 2;
//	DirArrow2D[11].X = 20; DirArrow2D[11].Z = 22;
//	DirArrow2D[12].X = 8;  DirArrow2D[12].Z = 22;
//	DirArrow2D[13].X = 8;  DirArrow2D[13].Z = 47;

	int i;

	ArrowPoints[0].U = ArrowPoints[1].U = ArrowPoints[1].V = ArrowPoints[2].V = 1.0f;
	ArrowPoints[0].V = ArrowPoints[3].U = ArrowPoints[3].V = ArrowPoints[2].U = 0.0f;

	i = 4;
	while(i--)
	{
		ArrowPoints[i].Specular = 0.0f;
		ArrowPoints[i].fog = 1.0f;
		ArrowPoints[i].Alpha = 0.5f;
	}
}

//*****************************************************************************************************************************************
void DrawDirectionArrow(int Type)
{
	HelpArrowType *arrow;
	float scale;
	int color_index;
	int omidx,omidy;

	if(Type == AV_TARGET_DIR_ARROW)
	{
		arrow = &Av.MH.TargetArrow;
		scale = 1.0f;
		color_index = AV_RED;
	}
	else
	{
		arrow = &Av.MH.WaypointArrow;
		scale = 0.5f;
		color_index = AV_GREEN;
	}

	if (arrow->Draw)
	{
  		FPoint color;
		FPoint lead_point;
		float fwork;
		RotPoint3D *walker;
		int i;


		color = *(FPoint *)&AvPalette[color_index][AV_PALETTE_NUM_SHADES-1];
		color *= 1.0f/255.0f;



		lead_point.Z = ZClipPlane * 2.0f;

		fwork = lead_point.Z*OneOverEyeToScreen;

		lead_point.X = ((float)arrow->ScrX)*fwork;
		lead_point.Y = ((float)arrow->ScrY)*fwork;

		float length = sqrt(lead_point.X * lead_point.X + lead_point.Y*lead_point.Y);

		ArrowPoints[0].Rotated = lead_point;
		ArrowPoints[2].Rotated.SetValues(1.0f - ((0.4f*scale)/length),lead_point);
		ArrowPoints[2].Rotated.Z = lead_point.Z;

		lead_point.MakeVectorToFrom(ArrowPoints[0].Rotated,ArrowPoints[2].Rotated);
		lead_point *= 0.5f;

		ArrowPoints[3].Rotated = ArrowPoints[2].Rotated;
		ArrowPoints[3].Rotated += lead_point;

		ArrowPoints[1].Rotated = ArrowPoints[3].Rotated;

		ArrowPoints[1].Rotated.Y -= lead_point.X;
		ArrowPoints[1].Rotated.X += lead_point.Y;

		ArrowPoints[3].Rotated.Y += lead_point.X;
		ArrowPoints[3].Rotated.X -= lead_point.Y;

		int draw_flags,clip_flags;

		draw_flags = PT3_NODRAW;
		clip_flags = 0;

		omidx = GrBuffFor3D->MidX;
		omidy = GrBuffFor3D->MidY;
		GrBuffFor3D->MidX = ScreenSize.cx >> 1;
		GrBuffFor3D->MidY = ScreenSize.cy >> 1;
		Set3DScreenClip();

		i = 4;
		walker=ArrowPoints;
		while(i--)
		{
			walker->Flags = PT3_ROTATED;
			PerspectPointWithAllFlags(walker);
			walker->Diffuse = color;
			draw_flags &= walker->Flags;
			clip_flags |= walker->Flags;
			walker++;
		}

		if (!draw_flags)
		{
    		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 		   FALSE);
    		lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 	  	   FALSE);
			NoFogging = TRUE;

			CurrentTexture = pArrowTexture;

			DrawPolyClipped(PRIM_TEXTURE | PRIM_NO_Z_WRITE | PRIM_ALPHAIMM,CL_CLIP_UV,4,lpArrowPoints,clip_flags & PT3_CLIP_FLAGS);

			NoFogging = FALSE;
    		lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 	  	   TRUE);
    		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 		   TRUE);

		}

		GrBuffFor3D->MidX = omidx;
		GrBuffFor3D->MidY = omidy;
		Set3DScreenClip();
	}
}


//*****************************************************************************************************************************************
void FixArrow(HelpArrowType &arrow)
{
	int sx = (ScreenSize.cx >> 1)-1;
	int sy = (ScreenSize.cy >> 1)-1;

	if (arrow.ScrX > sx)
	{
		arrow.ScrY = (int)(((float)sx/(float)arrow.ScrX)*(float)arrow.ScrY);
		arrow.ScrX = sx;
	}

	if (arrow.ScrX < -sx)
	{
		arrow.ScrY = (int)(((float)-sx/(float)arrow.ScrX)*(float)arrow.ScrY);
		arrow.ScrX = -sx;
	}

	if (arrow.ScrY > sy)
	{
		arrow.ScrX = (int)(((float)sy/(float)arrow.ScrY)*(float)arrow.ScrX);
		arrow.ScrY = sy;
	}

	if (arrow.ScrY < -sy)
	{
		arrow.ScrX = (int)(((float)-sy/(float)arrow.ScrY)*(float)arrow.ScrX);
		arrow.ScrY = -sy;
	}
}

//*****************************************************************************************************************************************
BOOL CalculateArrowDirection(HelpArrowType &arrow,FPointDouble &target)
{
 	float	PosX, PosY;
 	FPoint	Vec;

	Vec.MakeVectorToFrom(target,PlayerPlane->WorldPosition);
 	Vec *= ViewMatrix;

	BOOL ret_val = FALSE;

	if (!Vec.Perspect(&PosX,&PosY) || (PosX >= ScreenSize.cx) || (PosX < 0.0f) || (PosY >= ScreenSize.cy) || (PosY < 0.0f))
	{
		float length = sqrt(Vec.X * Vec.X + Vec.Y*Vec.Y);
		float tan_angle;

		if (length || Vec.Z)
		{
			if (Vec.Z)
				tan_angle = length/fabs(Vec.Z);
			else
				tan_angle = 1.0f;

			if (tan_angle > 0.0437) //5 degrees off center
			{
				length = 2000.0f/length;
				arrow.ScrX = (int)(length*Vec.X);
				arrow.ScrY = (int)(length*Vec.Y);
			}
			else
			{

				tan_angle = tan_angle * (1.0f/0.0437);
				if (tan_angle == 0.0f)
				{
					arrow.ScrX = 0;
					arrow.ScrY = -(ScreenSize.cy >> 1);
				}
				else
				{
					float angle;
					angle = atan2(Vec.X,-Vec.Y)*tan_angle;
					arrow.ScrX = (int)(2000.0f*sin(angle));
					arrow.ScrY = (int)(-2000.0f*cos(angle));
				}

			}
			FixArrow(arrow);
  			ret_val = TRUE;
		}
	}

	return ret_val;
}

//*****************************************************************************************************************************************
void DirectionalHelp()
{
 	FPointDouble target;
	BOOL got_target = FALSE;

	switch(UFC.MasterMode)
	{
		case AA_MODE:
  			if(PlayerPlane->AADesignate != NULL)
			{
				target = PlayerPlane->AADesignate->WorldPosition;
				got_target = TRUE;
			}
			break;

		case AG_MODE:
			if(PlayerPlane->AGDesignate.X != -1)
			{
				target = PlayerPlane->AGDesignate;
				got_target = TRUE;
			}
			break;
	}

  	Av.MH.TargetArrow.Draw = (BOOL)(got_target && CalculateArrowDirection(Av.MH.TargetArrow,target));


 	if(PlayerPlane->AI.CurrWay != NULL)
	{
 		target.X = PlayerPlane->AI.CurrWay->lWPx*FTTOWU;
 		target.Z = PlayerPlane->AI.CurrWay->lWPz*FTTOWU;
 		target.Y = PlayerPlane->AI.CurrWay->lWPy*FTTOWU;

  		Av.MH.WaypointArrow.Draw = CalculateArrowDirection(Av.MH.WaypointArrow,target);
	}
	else
    	Av.MH.WaypointArrow.Draw = FALSE;

}

//*****************************************************************************************************************************************
void SetGPSTarget()
{
	FPointDouble Loc;

	if(UFC.MasterMode != AG_MODE) return;

	if(Av.Weapons.CurAGWeap == NULL)
	{
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_GPS)
			PlayerPlane->AGDesignate.X = -1;
		return;
	}

	int GpsTargetExists = GetGPSLocation(Av.Weapons.CurAGWeap->WeapPage,&Loc);

	if(GpsTargetExists)
	{
		BreakAGLock();
		PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_GPS;
		PlayerPlane->AGDesignate  = Loc;
		PlayerPlane->AGDesignate.Y += 2 FEET;
	}
	else
	{
		if(PlayerPlane->AGDesignator == AG_DESIGNATE_FROM_GPS)
			PlayerPlane->AGDesignate.X = -1;
	}
}

//*****************************************************************************************************************************************
void SuperImposeAGTargetSymbology()
{
	if(Av.MissionAGTargets.NumAGTargets <= 0) return;

	AvObjListEntryType *Walk = &Av.MissionAGTargets.List[0];
	AvObjListEntryType *End  = &Av.MissionAGTargets.List[Av.MissionAGTargets.NumAGTargets-1];

	AvRGB Triple;

	FPointDouble TargetLoc;
	FPoint Vec;
	float ScrX,ScrY;
	char *Str;
	char bColor=AV_RED;

	while(Walk <= End)
	{
		if (Walk->Obj)
		{
			if(Walk->Type == GROUNDOBJECT)
				 TargetLoc = ((BasicInstance *)Walk->Obj)->Position;
			else if(Walk->Type == MOVINGVEHICLE)
				 TargetLoc = ((MovingVehicleParams *)Walk->Obj)->WorldPosition;
			else if(Walk->Type == SHIP)
				 TargetLoc = ((MovingVehicleParams *)Walk->Obj)->WorldPosition;
			else if(Walk->Type == AIRCRAFT)
				 TargetLoc = ((PlaneParams *)Walk->Obj)->WorldPosition;

			Vec  = TargetLoc;
			Vec -= Camera1.CameraLocation;
			Vec *= ViewMatrix;

			int Result = Vec.Perspect(&ScrX,&ScrY);
			if(Result)
			{
				Set2DZValue(-(TargetLoc - Camera1.CameraLocation)/4.0);

				float Dist = PlayerPlane->WorldPosition / TargetLoc;
				float Frac = (1.0 - (Dist*WUTONM)/100.0);
				if(Frac > 1.0) Frac = 1.0;

				if(Walk->Type == GROUNDOBJECT)
				{
					if (!(((BasicInstance *)Walk->Obj)->Flags & BI_DESTROYED))
					{
						if(Walk->Category == AV_PRIMARY)
						{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY, g_iLanguageId))
							{
								return;
							}
						}
						else if(Walk->Category == AV_SECONDARY)
						{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECONDARY, g_iLanguageId))
							{
								return;
							}
						} else	{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_TARGET, g_iLanguageId))
							{
								return;
							}
						}

						GetRGBFromAVPalette(AV_RED,Frac,&Triple);

						GrDrawLineClipped( GrBuffFor3D, (ScrX-2),(ScrY-2),(ScrX+2),(ScrY-2), Triple.Red,Triple.Green,Triple.Blue,0);
						GrDrawLineClipped( GrBuffFor3D, (ScrX+2),(ScrY-2),(ScrX+2),(ScrY+2), Triple.Red,Triple.Green,Triple.Blue,0);
						GrDrawLineClipped( GrBuffFor3D, (ScrX+2),(ScrY+2),(ScrX-2),(ScrY+2), Triple.Red,Triple.Green,Triple.Blue,0);
						GrDrawLineClipped( GrBuffFor3D, (ScrX-2),(ScrY+2),(ScrX-2),(ScrY-2), Triple.Red,Triple.Green,Triple.Blue,0);

   	  					DrawTextAtLoc(ScrX+10,ScrY-5,TmpStr,AV_RED,Frac);
					}
				} else if( (Walk->Type == MOVINGVEHICLE) || (Walk->Type == SHIP) )
				{
					if (!(((MovingVehicleParams *)Walk->Obj)->Status & VL_DESTROYED))
					{
						if ((((MovingVehicleParams *)Walk->Obj)->Status & VL_ACTIVE) && !(((MovingVehicleParams *)Walk->Obj)->Status & VL_INVISIBLE))
						{
							int Index  = ((MovingVehicleParams *)Walk->Obj)->iVDBIndex;
							int IsShip = ((MovingVehicleParams *)Walk->Obj)->iShipType;
							
							bColor=AV_RED;

							if(Walk->Category == AV_PRIMARY)
							{
								if(((MovingVehicleParams *)Walk->Obj)->iSide == AI_FRIENDLY)
								{
									bColor=AV_GREEN;

									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY_OBJ, g_iLanguageId))
									{
										return;
									}
								}
								else if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY, g_iLanguageId))
								{
									return;
								}
							} else if(Walk->Category == AV_SECONDARY)
							{
								if(((MovingVehicleParams *)Walk->Obj)->iSide == AI_FRIENDLY)
								{
									bColor=AV_GREEN;

									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECOND_OBJ, g_iLanguageId))
									{
										return;
									}
								}
								else if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECONDARY, g_iLanguageId))
								{
									return;
								}
							} else
							{
								if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_TARGET, g_iLanguageId))
								{
									return;
								}
							}

							if(!IsShip)
								Str = pDBVehicleList[((MovingVehicleParams *)Walk->Obj)->iVDBIndex].sName;
							else
								Str = pDBShipList[((MovingVehicleParams *)Walk->Obj)->iVDBIndex].sName;

							GetRGBFromAVPalette(bColor,Frac,&Triple);

							GrDrawLineClipped( GrBuffFor3D, (ScrX-2),(ScrY-2),(ScrX+2),(ScrY-2), Triple.Red,Triple.Green,Triple.Blue,0);
							GrDrawLineClipped( GrBuffFor3D, (ScrX+2),(ScrY-2),(ScrX+2),(ScrY+2), Triple.Red,Triple.Green,Triple.Blue,0);
							GrDrawLineClipped( GrBuffFor3D, (ScrX+2),(ScrY+2),(ScrX-2),(ScrY+2), Triple.Red,Triple.Green,Triple.Blue,0);
							GrDrawLineClipped( GrBuffFor3D, (ScrX-2),(ScrY+2),(ScrX-2),(ScrY-2), Triple.Red,Triple.Green,Triple.Blue,0);

   	  						DrawTextAtLoc(ScrX+10,ScrY-5,TmpStr,bColor,Frac);
   	  						DrawTextAtLoc(ScrX+10,ScrY-5+7,Str,bColor,Frac);
						}
					}
				}
			}
		}
		Walk++;
	}
	Set2DZValue();
}

//*****************************************************************************************************************************************
BOOL AmIACounter(int iWeaponIndex)
{
	BOOL bReturnValue=FALSE;

	switch(pDBWeaponList[iWeaponIndex].iWeaponType)
	{
		case WEAPON_TYPE_FLARE:
		case WEAPON_TYPE_CHAFF:
		case WEAPON_TYPE_CHAFF_FLARE_DISP:
		case WEAPON_TYPE_STANDOFF_JAMMER:
		case WEAPON_TYPE_IJAM_NOISE:
		case WEAPON_TYPE_DECOY:
		case WEAPIN_TYPE_IJAM_DECEPT:
			bReturnValue=TRUE;
		break;
	}
	return(bReturnValue);
}

//*****************************************************************************************************************************************
void SuperImposeMissilesFiredAtPlayer()
{
	FPointDouble TargetLoc;
	FPoint Vec;
	float ScrX,ScrY=0;
	char *Str=NULL;
	char szMissileName[120];
	AvRGB Triple;
	int iWeaponIndex=-1;

	for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if ((W->iTargetType==TARGET_PLANE) && (W->Kind != BULLET))
			{
				if (W->pTarget==PlayerPlane)
				{
					TargetLoc = W->Pos;

					Vec  = TargetLoc;
					Vec -= Camera1.CameraLocation;
					Vec *= ViewMatrix;

					int Result = Vec.Perspect(&ScrX,&ScrY);
					if(Result)
					{
						Set2DZValue(-(TargetLoc - Camera1.CameraLocation)/4.0);

						float Dist = PlayerPlane->WorldPosition / TargetLoc;
						float Frac = (1.0 - (Dist*WUTONM)/100.0);

						if(Frac > 1.0) Frac = 1.0;

						GetRGBFromAVPalette(AV_RED,Frac,&Triple);

						if ((int)W->Type < 0x200)
						{
							if (W->Type>=0)
							{
								iWeaponIndex=AIGetWeaponIndex((int)W->Type);

								if (iWeaponIndex!=-1)
								{
									if (!AmIACounter(iWeaponIndex))
									{
										strcpy(szMissileName,pDBWeaponList[iWeaponIndex].sName);
   	  									DrawTextAtLoc(ScrX+10,ScrY-5,szMissileName,AV_RED,Frac);

										GrDrawLineClipped( GrBuffFor3D, (ScrX-3),(ScrY),(ScrX),(ScrY-3), Triple.Red,Triple.Green,Triple.Blue,0);
										GrDrawLineClipped( GrBuffFor3D, (ScrX),(ScrY-3),(ScrX+3),(ScrY), Triple.Red,Triple.Green,Triple.Blue,0);
										GrDrawLineClipped( GrBuffFor3D, (ScrX+3),(ScrY),(ScrX),(ScrY+3), Triple.Red,Triple.Green,Triple.Blue,0);
										GrDrawLineClipped( GrBuffFor3D, (ScrX),(ScrY+3),(ScrX-3),(ScrY), Triple.Red,Triple.Green,Triple.Blue,0);
									}
								}
							}
						}
						else
						{
							if (W->Type->TypeNumber>=0)
							{
								iWeaponIndex=AIGetWeaponIndex((int)W->Type->TypeNumber);

								if (iWeaponIndex!=-1)
								{
									if (!AmIACounter(iWeaponIndex))
									{
										strcpy(szMissileName,pDBWeaponList[iWeaponIndex].sName);
   	  									DrawTextAtLoc(ScrX+10,ScrY-5,szMissileName,AV_RED,Frac);	 

										GrDrawLineClipped( GrBuffFor3D, (ScrX-3),(ScrY),(ScrX),(ScrY-3), Triple.Red,Triple.Green,Triple.Blue,0);
										GrDrawLineClipped( GrBuffFor3D, (ScrX),(ScrY-3),(ScrX+3),(ScrY), Triple.Red,Triple.Green,Triple.Blue,0);
										GrDrawLineClipped( GrBuffFor3D, (ScrX+3),(ScrY),(ScrX),(ScrY+3), Triple.Red,Triple.Green,Triple.Blue,0);
										GrDrawLineClipped( GrBuffFor3D, (ScrX),(ScrY+3),(ScrX-3),(ScrY), Triple.Red,Triple.Green,Triple.Blue,0);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

extern void AICGetCallSign(int planenum, char *callsign, int putlast);
extern int iMHShowCallSign;

//*****************************************************************************************************************************************
void SuperImposePlaneSymbology()
{
	PlaneParams *P = &Planes[0];
	FPoint	Vec;
	int YPos = 7;
	char szCallSign[260];

	while(P <= LastPlane)
	{
		if( (P->Status & PL_ACTIVE) && (P != PlayerPlane) && (P->OnGround!=2) && !(P->FlightStatus & (PL_STATUS_CRASHED)) )
		{
			double Dist = Dist2D(&PlayerPlane->WorldPosition,&P->WorldPosition);

			if ((Dist*WUTONM)<100)
			{
				Vec  = P->WorldPosition;
				Vec -= Camera1.CameraLocation;
				Vec *= ViewMatrix;
				int iSrcX,iSrcY;
				int inView=Vec.Perspect(&iSrcX,&iSrcY);
				YPos = 7;

				if(inView)
				{
					Set2DZValue(-(P->WorldPosition - Camera1.CameraLocation)/4.0);

					// shade according to distance
					float Frac = (1.0 - (Dist*WUTONM)/100.0);
					if(Frac > 1.0) Frac = 1.0;
					if(Frac < 0.0) Frac = 0.0;

					int Status = IsPlanePrimaryOrSecondary(P);

					if( (iSrcX >= 0) && (iSrcX < ScreenSize.cx) && (iSrcY >= 0) && (iSrcY < ScreenSize.cy) )
					{
						TruncatePlaneName(TmpStr,pDBAircraftList[P->AI.iPlaneIndex].sName);

						if(P->AI.iSide == AI_FRIENDLY)
						{
							DrawTextAtLoc(iSrcX+10,iSrcY-5,TmpStr,AV_GREEN,Frac);

							if ((Status != -1) && (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID!=85))
							{
								if(Status == AV_PRIMARY)
								{
									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY_OBJ, g_iLanguageId))
									{
										return;
									}
								}
								else
								{
									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECOND_OBJ, g_iLanguageId))
									{
										return;
									}
								}
								DrawTextAtLoc(iSrcX+10,iSrcY-5+YPos,TmpStr,AV_GREEN,Frac);
								YPos += 7;
							}
							if((MultiPlayer) && (!WIsWeaponPlane(P)))
							{
								 NetGetMultiPlayerName(P - &Planes[0],TmpStr);
								 DrawTextAtLoc(iSrcX+10,iSrcY-5+YPos,TmpStr,AV_GREEN,Frac);
							}
							if (iMHShowCallSign)
							{
								if (!WIsWeaponPlane(P))
								{
									AICGetCallSign((P - &Planes[0]),szCallSign,1);
									DrawTextAtLoc(iSrcX+10,(iSrcY-5+YPos),szCallSign,AV_GREEN,Frac);
								}
							}
						} else if(P->AI.iSide == AI_ENEMY)
						{
							DrawTextAtLoc(iSrcX+10,iSrcY-5,TmpStr,AV_RED,Frac);

							if ((Status != -1) && (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID!=85))
							{
								if(Status == AV_PRIMARY)
								{
									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY, g_iLanguageId))
									{
										return;
									}
								}
								else
								{
									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECONDARY, g_iLanguageId))
									{
										return;
									}
								}

		//										DrawTextAtLoc(Plane->SrcX+10,Plane->SrcY-5+YPos,(Status == AV_PRIMARY) ? "PRIMARY_TARGET" : "SECONDARY_TARGET",AV_RED,Frac);
								DrawTextAtLoc(iSrcX+10,iSrcY-5+YPos,TmpStr,AV_RED,Frac);
								YPos += 7;
							}
							if((MultiPlayer) && (!WIsWeaponPlane(P)))
							{
								 NetGetMultiPlayerName(P - &Planes[0],TmpStr);
								 DrawTextAtLoc(iSrcX+10,iSrcY-5+YPos,TmpStr,AV_RED,Frac);
							}
						} else {
							DrawTextAtLoc(iSrcX+10,iSrcY-5,TmpStr,AV_BLUE,Frac);

							if ((Status != -1) && (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID!=85))
							{
								if(Status == AV_PRIMARY)
								{
									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY, g_iLanguageId))
									{
										return;
									}
								}
								else
								{
									if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECONDARY, g_iLanguageId))
									{
										return;
									}
								}
	//										DrawTextAtLoc(Plane->SrcX+10,Plane->SrcY-5+YPos,(Status == AV_PRIMARY) ? "PRIMARY_TARGET" : "SECONDARY_TARGET",AV_BLUE,Frac);
								DrawTextAtLoc(iSrcX+10,iSrcY-5+YPos,TmpStr,AV_BLUE,Frac);
								YPos += 7;
							}
							if((MultiPlayer) && (!WIsWeaponPlane(P)))
							{
								 NetGetMultiPlayerName(P - &Planes[0],TmpStr);
								 DrawTextAtLoc(iSrcX+10,iSrcY-5+YPos,TmpStr,AV_BLUE,Frac);
							}
						}
					}
				}
			}
		}
  		P++;
	}
	Set2DZValue();
}

#if 0
//*****************************************************************************************************************************************
void SuperImposePlaneSymbology()
{
	AvObjListEntryType *Plane;
	int YPos = 7;

	Plane = &Av.Planes.List[0];
	int Index = Av.Planes.NumPlanes;

	while(Index-- > 0)
	{
		PlaneParams *P = (PlaneParams *)Plane->Obj;

		if(Plane->InView)
		{
			Set2DZValue(-(P->WorldPosition - Camera1.CameraLocation)/4.0);

			// shade according to distance
			float Frac = (1.0 - (Plane->Dist*WUTONM)/100.0);
			if(Frac > 1.0) Frac = 1.0;
			if(Frac < 0.0) Frac = 0.0;

			int Status = IsPlanePrimaryOrSecondary(P);

			if( (Plane->ScrX >= 0) && (Plane->ScrX < ScreenSize.cx) && (Plane->ScrY >= 0) && (Plane->ScrY < ScreenSize.cy) )
			{
				TruncatePlaneName(TmpStr,pDBAircraftList[P->AI.iPlaneIndex].sName);

				if(P->AI.iSide == AI_FRIENDLY)
				{
					DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5,TmpStr,AV_GREEN,Frac);

					if ((Status != -1) && (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID!=85))
					{
						if(Status == AV_PRIMARY)
						{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY_OBJ, g_iLanguageId))
							{
								return;
							}
						}
						else
						{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECOND_OBJ, g_iLanguageId))
							{
								return;
							}
						}
						DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,TmpStr,AV_GREEN,Frac);
						YPos += 7;
					}
					if((MultiPlayer) && (!WIsWeaponPlane(P)))
					{
						 NetGetMultiPlayerName(P - &Planes[0],TmpStr);
						 DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,TmpStr,AV_GREEN,Frac);
					}
				} else if(P->AI.iSide == AI_ENEMY)
				{
					DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5,TmpStr,AV_RED,Frac);

					if ((Status != -1) && (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID!=85))
					{
						if(Status == AV_PRIMARY)
						{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY, g_iLanguageId))
							{
								return;
							}
						}
						else
						{
							if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECONDARY, g_iLanguageId))
							{
								return;
							}
						}

//										DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,(Status == AV_PRIMARY) ? "PRIMARY_TARGET" : "SECONDARY_TARGET",AV_RED,Frac);
						DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,TmpStr,AV_RED,Frac);
						YPos += 7;
					}
					if((MultiPlayer) && (!WIsWeaponPlane(P)))
					{
						 NetGetMultiPlayerName(P - &Planes[0],TmpStr);
						 DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,TmpStr,AV_RED,Frac);
					}
				} else
					{
						DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5,TmpStr,AV_BLUE,Frac);

						if ((Status != -1) && (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID!=85))
						{
							if(Status == AV_PRIMARY)
							{
								if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_PRIMARY, g_iLanguageId))
								{
									return;
								}
							}
							else
							{
								if(!LANGGetTransMessage(TmpStr, 128, MSG_PILOTAID_SECONDARY, g_iLanguageId))
								{
									return;
								}
							}
//										DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,(Status == AV_PRIMARY) ? "PRIMARY_TARGET" : "SECONDARY_TARGET",AV_BLUE,Frac);
							DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,TmpStr,AV_BLUE,Frac);
							YPos += 7;
						}
						if((MultiPlayer) && (!WIsWeaponPlane(P)))
						{
							 NetGetMultiPlayerName(P - &Planes[0],TmpStr);
							 DrawTextAtLoc(Plane->ScrX+10,Plane->ScrY-5+YPos,TmpStr,AV_BLUE,Frac);
						}
					}
			}
		}
  		Plane++;
	}
	Set2DZValue();
}
#endif

//*****************************************************************************************************************************************
void SuperImposeSymbology()
{
	if(g_Settings.gp.dwCheats & GP_CHEATS_AIRCRAFT_LABELS)
	{
		SuperImposePlaneSymbology();
		SuperImposeMissilesFiredAtPlayer();
	}

	if(g_Settings.gp.dwCheats & GP_CHEATS_GRND_TARG_LABELS)
		SuperImposeAGTargetSymbology();
}

//*****************************************************************************************************************************************
void InitMissionHelpData()
{
	Av.MH.AGTargSelectFilter = 0;
	Av.MH.PrevWaypoint = NULL;
	Av.MH.StartPos.X = Av.MH.StartPos.Y = Av.MH.StartPos.Z = -1;
}

//*****************************************************************************************************************************************
void MissionHelpTick()
{
 	SuperImposeSymbology();
  	SetGPSTarget();
}

//*****************************************************************************************************************************************
void DrawDirectionArrows()
{
	if( (g_Settings.gp.dwCheats & GP_CHEATS_NAV_VISUAL) || (g_Settings.gp.dwCheats & GP_CHEATS_TARG_VISUAL) )
	{
		DirectionalHelp();

		if(UFC.MasterMode == AG_MODE)
		{
			if(g_Settings.gp.dwCheats & GP_CHEATS_TARG_VISUAL)
			  DrawDirectionArrow(AV_TARGET_DIR_ARROW);
		}
		else if(UFC.MasterMode == NAV_MODE)
		{
			if(g_Settings.gp.dwCheats & GP_CHEATS_NAV_VISUAL)
			  DrawDirectionArrow(AV_WAYPOINT_DIR_ARROW);
		}
	}
}

//*****************************************************************************************************************************************
// DESTRUCTOCAM  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

#define DESTRUCTO_OFF                 0
#define DESTRUCTO_COMMAND_OFF         1
#define DESTRUCTO_WAITING_TO_START    2
#define DESTRUCTO_ON                  3
#define DESTRUCTO_WAITING_TO_END      4


//*****************************************************************************************************************************************
void InitDestructoCam()
{
	Av.Destructo.CurWeap     = NULL;
	Av.Destructo.PrevView    = -1;
	Av.Destructo.Status      = DESTRUCTO_OFF;
	DestructoMessage = 0;
}

//*****************************************************************************************************************************************
BOOL IsEjected()
{
	if (PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
	{
		AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

		Camera1.CameraMode = CAMERA_COCKPIT;
		Camera1.SubType = COCKPIT_FRONT | COCKPIT_NOART;
		LoadCockpit(FRONT_NO_COCKPIT);
		SetupView( Camera1.SubType );
		CockpitSeat = NO_SEAT;
		OurShowCursor( FALSE );

		AssignCameraSubject((void *)pPlayerChute,CO_WEAPON);

		return TRUE;
	}
	else
		return FALSE;

}

//*****************************************************************************************************************************************
void SetupNoCockpitArt()
{
	if ((Camera1.CameraMode == CAMERA_COCKPIT) || (CockpitSeat != NO_SEAT))
	{
		LoadCockpit(FRONT_NO_COCKPIT);
		SetupView(COCKPIT_NOART);
		CockpitSeat = NO_SEAT;
	}
}

//*****************************************************************************************************************************************
void virtual_free_look( VKCODE vk );

//*****************************************************************************************************************************************
void SetupFrontCamera()
{
	VKCODE vk;

	ZeroMemory(&vk,sizeof(vk));

	virtual_free_look(vk);

//	if (!IsEjected())
//	{
//		if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
//		{
//			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);


//			Camera1.CameraMode = CAMERA_COCKPIT;
//			Camera1.SubType = COCKPIT_VIRTUAL_SEAT;
//	 		LoadCockpit(FRONT_FORWARD_COCKPIT);
//			SetupView( Camera1.SubType );
//			CockpitSeat = FRONT_SEAT;
//			OurShowCursor( TRUE );
//		}
//		else
//		{
//			AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
//			SetupNoCockpitArt();
//			ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
//		}
//	}

}


//*****************************************************************************************************************************************
void SetDestructoView(WeaponParams *AttachWeap)
{
	OldCockpitSeat    = CockpitSeat;
	OldCurrentCockpit = CurrentCockpit;
	memcpy( &OldCamera, &Camera1, sizeof( CameraInstance));

	WeaponParams *Weap = NULL;

	if ( (Weap == NULL) && (Camera1.AttachedObjectType == CO_WEAPON) )
		return;

	Weap = AttachWeap;

	SetupNoCockpitArt();

	int old_status = Av.Destructo.Status;
	WeaponParams *old_weapon = Av.Destructo.CurWeap;

	AssignCameraSubject((void *)Weap,CO_WEAPON);

	if ((Camera1.CameraMode == CAMERA_COCKPIT) || (Camera1.CameraMode == CAMERA_TACTICAL) || (Camera1.CameraMode == CAMERA_REVERSE_TACTICAL))
	{
		SetupNoCockpitArt();
		Camera1.CameraMode = CAMERA_FIXED_CHASE;
		ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
	}
	else
		if (Camera1.CameraMode == CAMERA_FREE)
	 		ChangeViewModeOrSubject(CAMERA_RC_VIEW);
		else
	 		ChangeViewModeOrSubject(Camera1.CameraMode);
	Av.Destructo.Status = old_status;
	Av.Destructo.CurWeap = old_weapon;

}

//*****************************************************************************************************************************************
int CheckForWeaponAtTarget(WeaponParams *Weap,int Type,FPointDouble SubjectLoc,FPointDouble SubjectVelocity)
{
	if (Type == CO_NADA)
		return(Weap->Altitude < (Weap->Vel.Y*-300.0));
	else
		return( (float)(Weap->Pos - SubjectLoc) < (float)(SubjectVelocity.QuickLength()*300.0) );
}

//*****************************************************************************************************************************************
void CheckDestructoCam()
{
	DestructoMessage = 0;

	if(g_Settings.gp.dwCheats & GP_CHEATS_DESTRUCTO_CAM)
	{
		double ClosestToTarget = 10000000;
		WeaponParams *ClosestWeap = NULL;
		FPointDouble  SubjectVelocity,SubjectLocation;
		int Found = FALSE;

		WeaponParams *Weap = &Weapons[0];

		while(Weap <= LastWeapon)
		{
			if( (Weap->Flags & WEAPON_INUSE) && (Weap->Kind != BULLET) && (Weap->P == PlayerPlane) && (Weap->Type != pFlareType) && (Weap->Type != pChaffType) && (Weap->Kind != EJECTION_SEAT) )
			{
				int TargetType = Weap->iTargetType;

				if(Weap->pTarget == NULL)
					TargetType = NONE;

				if(TargetType == TARGET_PLANE)
				{
					PlaneParams *P = (PlaneParams *)Weap->pTarget;
					if( (P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED) )
						SubjectVelocity.SetValues(HIGH_FREQ * FTTOWU, P->IfVelocity);
					else
						SubjectVelocity.SetValues(0.0f);
					Found = CheckForWeaponAtTarget(Weap,CO_PLANE,P->WorldPosition,SubjectVelocity);
					if(Found) break;
				}
				else
					if(TargetType == TARGET_GROUND)
					{
						BasicInstance *Inst = (BasicInstance *)Weap->pTarget;
						SubjectVelocity.SetValues(0.0f);
						Found = CheckForWeaponAtTarget(Weap,CO_GROUND_OBJECT,Inst->Position,SubjectVelocity);
						if(Found) break;
					}
					else
						if(TargetType == TARGET_VEHICLE)
						{
							MovingVehicleParams *Vehicle = (MovingVehicleParams *)Weap->pTarget;
							if( (Vehicle->Status & VL_ACTIVE) && !(Vehicle->Status & (VL_DESTROYED | VL_DONE_MOVING)) )
							{
								SubjectVelocity.SetValues(0.0f,0.0f,-Vehicle->fSpeedFPS * HIGH_FREQ * FTTOWU);
								SubjectVelocity *= Vehicle->Attitude;
							}
							else
								SubjectVelocity.SetValues(0.0f);

							Found = CheckForWeaponAtTarget(Weap,CO_VEHICLE,Vehicle->WorldPosition,SubjectVelocity);
							if(Found) break;
						}
						else
						{
							SubjectVelocity.SetValues(0.0f);
							Found = CheckForWeaponAtTarget(Weap,CO_NADA,SubjectLocation,SubjectVelocity);
							if(Found) break;
						}
			}
			Weap++;
		}

		if( (Found) && (Av.Destructo.Status != DESTRUCTO_COMMAND_OFF) )
		{
			if(Av.Destructo.Status == DESTRUCTO_OFF)
					Av.Destructo.Status = DESTRUCTO_WAITING_TO_START;

 			if (Av.Destructo.Status == DESTRUCTO_WAITING_TO_START)
				DestructoMessage = MSG_PILOTAID_HIT_DESTRUCTO;

			if(Av.Destructo.Status == DESTRUCTO_ON)
			{
				if(Av.Destructo.CurWeap != Weap)
				{
    				Av.Destructo.CurWeap = Weap;
					SetDestructoView(Weap);
					Av.Destructo.Status = DESTRUCTO_WAITING_TO_END;
 				}
			}
		}
		else
		{
			if(Av.Destructo.CurWeap)
			{
				DestructoMessage = MSG_PILOTAID_HIT_DESTRUCTO_RET;

				if(Av.Destructo.Status == DESTRUCTO_COMMAND_OFF)
				{
					Av.Destructo.CurWeap = NULL;

					memcpy( &Camera1, &OldCamera, sizeof( CameraInstance));
					if( Camera1.SubType & COCKPIT_VIRTUAL_SEAT)
						OurShowCursor( TRUE );
					else
						OurShowCursor( FALSE );
					CockpitSeat    = OldCockpitSeat;
					CurrentCockpit = OldCurrentCockpit;
					LoadCockpit( CurrentCockpit );
					CurrentView = -1;
					SetupView( Camera1.SubType );

					Av.Destructo.Status = DESTRUCTO_OFF;



					// JP CHANGE? Set front mode...need to restore old mode if we haven'tchanged modes 
    				//Av.Destructo.CurWeap = NULL;
    				//SetupFrontCamera();
					//Av.Destructo.Status = DESTRUCTO_OFF;	
				}
			}
		}
	}
}

//*****************************************************************************************************************************************
void DrawDestructoText()
{
	char tempstr[128];

	//if(DestructoMessage && LANGGetTransMessage(tempstr, 128, MSG_PILOTAID_HIT_DESTRUCTO, g_iLanguageId))
	if(DestructoMessage && LANGGetTransMessage(tempstr, 128, DestructoMessage, g_iLanguageId))
	{
		AvRGB Triple;
		GetRGBFromAVPalette(AV_RED,1.0,&Triple);

		DrawTextAtLocation(ScreenSize.cx - 110,2,tempstr,Triple.Red,Triple.Green,Triple.Blue,0);
	}
	DestructoMessage = 0;
}

//*****************************************************************************************************************************************
void ToggleDestructoCam()
{
	if(Av.Destructo.Status == DESTRUCTO_WAITING_TO_START)
		Av.Destructo.Status = DESTRUCTO_ON;
	else if(Av.Destructo.Status != DESTRUCTO_OFF)
		Av.Destructo.Status = DESTRUCTO_COMMAND_OFF;
}

