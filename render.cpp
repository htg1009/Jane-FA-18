#include "F18.h"
#include "F18weap.h"
#include "particle.h"
#include "gamesettings.h"
#include "3dfxF18.h"
#include "clouds.h"
#include "flightfx.h"

CameraInstance *glob_camera;
void FixMaxDrawDistances(float zoom);
extern unsigned char GreenPal[];
extern unsigned char BWPal[];
extern unsigned char WBWPal[];
extern unsigned char WGreenPal[];
extern unsigned char AdjustedGreenMissilePal[];
extern unsigned char AdjustedBWMissilePal[];
extern BOOL SpecialDraw;
extern float SunIntensity;
int SkipThisDot;
WeaponParams *SkipThisWeap;
BYTE bSubVars[MAX_SUBPARTS][20];

void DrawLand(const FPointDouble &eye_point);
void InitShadows(FPointDouble &eye);


void DrawAdiBall( void );
void SetupVehicleShadows(FPointDouble &eye);


extern FPoint *LastSurfaceNormal;
extern FMatrix *LastObjToViewMatrix;
extern float Visibility;
PlaneParams *VisiblePlanes;
extern FPoint ShadowWorldZ;
void ReleaseShadowEntry(void *pt);
extern ShadowInstance Shadows[],*LastShadow;
extern FPoint ShadowWorldX;
extern FPoint ShadowWorldY;
extern FPoint ShadowWorldZ;

extern TextureBuff *MuzzleFlash;
extern TextureBuff *Tracer1;
extern TextureBuff *Tracer2;
extern BOOL SunOnScreen;
extern BOOL MoonOnScreen;
void DrawSunGlare(BOOL sun = TRUE);
extern FPoint PositionToDrawSun;
extern FPoint PositionToDrawMoon;

extern float D3DMaxZ;
extern float ooD3DMaxZ;
extern float D3DMinFog;
extern float D3DMaxFog;
extern float D3DFogMulter;
extern float StupidQ;

void DrawUnlitAddBillboardRel(FPoint &btm,FPoint &top,float width,TextureBuff *texture,float alpha = 1.0f,FPoint *color = NULL,BOOL mip_maps = FALSE);
void DrawUnlitAddBillboard(FPointDouble &eye,FPointDouble &btm,FPointDouble &top,float width,TextureBuff *texture,float alpha = 1.0f,FPoint *color = NULL,BOOL mip_maps = FALSE);

FPointDouble SortVector;
double SortResult;
PlaneParams *SortPlane;
FPoint Green(0.7f,1.0f,0.7f);

void F18Sorter(ObjectSortHeader **tree,ObjectSortHeader *object)
{
	while(*tree)
	{
#ifdef _DEBUG
	if (*tree == object)
		return;
#endif
		tree = &(*tree)->InFront;
	}

	*tree = object;
}


void RenderBullet(FPoint &location,FPointDouble &vel,BOOL red_tracer)
{
	FPoint p1,p2;

	p1 = location;
	p1.AddScaledVector(0.2,vel);
	p2 = p1;

	if (red_tracer)
	{
		p2.AddScaledVector(-4.0,vel);
		DrawUnlitAddBillboardRel(p2,p1,3.0f FEET,Tracer2,1.0f,&Green,TRUE);
	}
	else
	{
		p2.AddScaledVector(-0.5,vel);
		DrawUnlitAddBillboardRel(p2,p1,1.0f FEET,Tracer1,1.0f,NULL,TRUE);
	}
}


extern int SimJettisonOn;

extern ObjectHandler *S2Rack, *S3Rack, *S4Rack, *S6Rack, *S8Rack, *S9Rack, *S10Rack;
extern SubObjectHandler SourceSubs[MAX_SUBPARTS];
SubObjectHandler OurSubs[MAX_SUBPARTS];

SubObjectHandler *GetSubObjects(PlaneParams *P)
{
	int i;
	int numinpod;
	int cnt;
	int numshow;
	int ix;

	memcpy(OurSubs,SourceSubs,sizeof(OurSubs));

#if 0
	if (P->WeapLoad[AG_LEFT].WeapId == 3)
	{
		OurSubs[12].Object  = LeftMavRack;
	}

	if (P->WeapLoad[AG_RIGHT].WeapId == 3)
	{
		OurSubs[9].Object  = RightMavRack;
	}
#else
#if 0  //  Moved top part to aiflight LoadWingmenWeaponInfo to handle rocket pods
	if (P->WeapLoad[LEFT2_STATION].Count > 1)
	{
		P->AI.iAICombatFlags1 |= AI_S2_RACK;
	}
	if (P->WeapLoad[LEFT3_STATION].Count > 1)
	{
		P->AI.iAICombatFlags1 |= AI_S3_RACK;
	}
	if (P->WeapLoad[LEFT4_STATION].Count > 1)
	{
		P->AI.iAICombatFlags1 |= AI_S4_RACK;
	}
	if (P->WeapLoad[RIGHT8_STATION].Count > 1)
	{
		P->AI.iAICombatFlags1 |= AI_S8_RACK;
	}
	if (P->WeapLoad[RIGHT9_STATION].Count > 1)
	{
		P->AI.iAICombatFlags1 |= AI_S9_RACK;
	}
	if (P->WeapLoad[RIGHT10_STATION].Count > 1)
	{
		P->AI.iAICombatFlags1 |= AI_S10_RACK;
	}
#endif

	if(P->AI.iAICombatFlags1 & AI_S2_RACK)
	{
		OurSubs[11].Object  = S2Rack;
	}
	if(P->AI.iAICombatFlags1 & AI_S3_RACK)
	{
		OurSubs[14].Object  = S3Rack;
	}
	if(P->AI.iAICombatFlags1 & AI_S4_RACK)
	{
		OurSubs[17].Object  = S4Rack;
	}
	if(P->AI.iAICombatFlags2 & AI_C6_RACK)
	{
		OurSubs[23].Object  = S6Rack;
	}
	if(P->AI.iAICombatFlags1 & AI_S8_RACK)
	{
		OurSubs[29].Object  = S8Rack;
	}
	if(P->AI.iAICombatFlags1 & AI_S9_RACK)
	{
		OurSubs[32].Object  = S9Rack;
	}
	if(P->AI.iAICombatFlags1 & AI_S10_RACK)
	{
		OurSubs[35].Object  = S10Rack;
	}


#endif

	for (i=0; i<MAX_F18E_STATIONS; i++)
	{
		if((pDBWeaponList[P->WeapLoad[i].WeapIndex].lWeaponID == 133) || (pDBWeaponList[P->WeapLoad[i].WeapIndex].lWeaponID == 134) || (pDBWeaponList[P->WeapLoad[i].WeapIndex].lWeaponID == 176))
		{
//			if(!SimJettisonOn)
//			{
				if(pDBWeaponList[P->WeapLoad[i].WeapIndex].lWeaponID == 133)
				{
					numinpod = 4;
				}
				else //  if((pDBWeaponList[P->WeapLoad[i].WeapIndex].lWeaponID == 134) || (pDBWeaponList[P->WeapLoad[i].WeapIndex].lWeaponID == 176))
				{
					numinpod = 19;
				}

				for (int count=P->WeapLoad[i].bNumPods; count>0; count--)
				{
					ix = GetHardPoint( P, i, count );
					if ((ix == 0) || ((P->Type->TypeNumber == 61) && ((ix == 8) || (ix == 38)))) break;

					OurSubs[ix].Object  = P->WeapLoad[i].Type->Model;

//					if(!OurSubs[ix].AllVars)
//					{
						OurSubs[ix].AllVars = bSubVars[ix];
//					}

					if(OurSubs[ix].AllVars)
					{
						if(count == 2)
						{
							numshow = (P->WeapLoad[i].Count > numinpod) ? P->WeapLoad[i].Count - numinpod : 0;
						}
						else
						{
							numshow = (P->WeapLoad[i].Count > numinpod) ? numinpod : P->WeapLoad[i].Count;
						}
						for(cnt = 0; cnt < numinpod; cnt ++)
						{
							OurSubs[ix].AllVars[cnt] = (cnt >= (numinpod - numshow)) ? 0 : 1;
						}
					}
				}
//			}
		}
		else
		{
			if (P->WeapLoad[i].Status == NONE_STAT) continue; //these keep missiles off the wingtips of the f18

			for (int count=P->WeapLoad[i].Count; count>0; count--)
			{
				ix = GetHardPoint( P, i, count );
				if ((ix == 0) || ((P->Type->TypeNumber == 61) && ((ix == 8) || (ix == 38)))) break;

				OurSubs[ix].Object  = P->WeapLoad[i].Type->Model;
//				if(OurSubs[ix].AllVars)
//				{
//					for(cnt = 0; cnt < MAX_SUBPARTS; cnt ++)
//					{
//						OurSubs[ix].AllVars[cnt] = 0;
//					}
//				}
			}
		}
	}

	return OurSubs;
}

FPoint DecoyCableColor(0.3f,0.3f,0.3f);
float DecoyCableShininess = 0.1f;

FPoint RefuelHoseColor(0.2f,0.2f,0.4f);
float RefuelHoseShininess = 0.5f;

void InterpolateCurve(FPoint &pfrom,FPoint &pto,FPoint &vfrom,FPoint &vto,float t,FPoint &result,FPoint &pvector)
{
	result.SetValues(t,vfrom);
	result += pfrom;

	pvector.SetValues(1.0-t,vto);
	pvector += pto;

	pvector -= result;
	result.AddScaledVector(t,pvector);

	pvector *= -1.0f;
}

void GetViewpointLineLight(FPoint &normal,FPoint &point,FPoint &returner)
{
	LightSource **light_walker = GlobalViewpointLights;

	while(light_walker <= LastViewpointLight)
	{
		if (*light_walker)
			(*light_walker)->CalculateDiffuse(point,normal,returner);

		light_walker++;
	}

	returner.Limit(1.0f);
}

void GetViewpointLineSpecularLight(FPoint &normal,FPoint &point,float shininess,FPoint &returner)
{
	LightSource **light_walker = GlobalViewpointLights;
	FPoint zero = 0.0f;

	while(light_walker <= LastViewpointLight)
	{
		if (*light_walker)
			(*light_walker)->CalculateSpecular(shininess,zero,point,normal,returner);

		light_walker++;
	}

	returner.Limit(1.0f);
}
					 //up   ,right
float hex_table[] = { 0.866  , 0.5,
					  0.0    , 1.0,
					 -0.866	 , 0.5,
					 -0.866  ,-0.5,
					  0.0	 ,-1.0,
					  0.866  ,-0.5};


void DrawCurvedTube(FPoint &pfrom,FPoint &pto,FPoint &vfrom,FPoint &vto,FPoint &color,FPoint &right_ptr,float shininess,float radius)
{
	RotPoint3D Points[14];
	RotPoint3D *points[2];
	RotPoint3D *poly_points[4];
	RotPoint3D *rpwalk;
	RotPoint3D *hex1from,*hex1to;
	RotPoint3D *hex2from,*hex2to;
	int draw_flags;
	FPoint new_point;
	FPoint line_normal;
	FPoint line_vector;
	FPoint light_value;
	FPoint to_eye;
	FPoint pvector;
	FPoint slice_up;
	FPoint vertex_vector;
	int i,j;
	DWORD gop_type;
	float z;
	float *fwalk;

	BOOL try_polys = FALSE;
	DWORD point1_poly,point2_poly;

	right_ptr *= ViewMatrix;


	z = pfrom * *(FPoint *)(&ViewMatrix.m_Data.RC.R2C0);

	if (z && ( 2*radius*EyeToScreen/z >= 1.2f))
		try_polys = TRUE;
	else
	{
		z = pto * *(FPoint *)(&ViewMatrix.m_Data.RC.R2C0);

		if (z && ( 2*radius*EyeToScreen/z >= 1.2f))
			try_polys = TRUE;
	}

	if (!try_polys)
	{

		points[0] = &Points[0];
		points[1] = &Points[1];

		InterpolateCurve(pfrom,pto,vfrom,vto,0.0,new_point,line_vector);
		line_vector *= ViewMatrix;

		Points[0].Rotated.RotateInto(new_point,ViewMatrix);

		if (Points[0].Rotated.Z > ZClipPlane)
			Points[0].Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			Points[0].Flags = PT3_ROTATED;
			PerspectPoint(points[0]);
		}

		line_normal = line_vector;
		to_eye = points[0]->Rotated;
		to_eye *= -1.0f;

		line_normal %= to_eye;
		line_normal %= line_vector;
		line_normal.Normalize();

		points[0]->Diffuse = color;
		light_value.SetValues(0.0f);

		GetViewpointLineLight(line_normal,points[0]->Rotated,light_value);

		points[0]->Diffuse *= light_value;

		points[0]->Specular.SetValues(0.0f);

		if (!(TerrainYesNos & YN_NO_SPECTRAL) && shininess)
			GetViewpointLineSpecularLight(line_normal,points[0]->Rotated,shininess,points[0]->Specular);

		if (points[0]->Rotated.Z < 0.0)
		{
			points[0]->Alpha = 2*radius*EyeToScreen/points[0]->Rotated.Z;
			if (points[0]->Alpha > 1.0f)
				points[0]->Alpha = 1.0f;
//			else
//				points[0]->Alpha *= points[0]->Alpha;

		}
		else
			points[0]->Alpha = 1.0f;


		i = 10;
		while(i--)
		{

			InterpolateCurve(pfrom,pto,vfrom,vto,1.0f - (float)i/10.0f,new_point,line_vector);
			line_vector *= ViewMatrix;

			points[1]->Rotated.RotateInto(new_point,ViewMatrix);

			if (points[1]->Rotated.Z > ZClipPlane)
				points[1]->Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				points[1]->Flags = PT3_ROTATED;
				PerspectPoint(points[1]);
			}

			line_normal = line_vector;
			to_eye = points[1]->Rotated;
			to_eye *= -1.0f;

			line_normal %= to_eye;
			line_normal %= line_vector;
			line_normal.Normalize();

			points[1]->Diffuse = color;
			light_value.SetValues(0.0f);

			GetViewpointLineLight(line_normal,points[1]->Rotated,light_value);

			points[1]->Diffuse *= light_value;

			points[1]->Specular.SetValues(0.0f);

			if (!(TerrainYesNos & YN_NO_SPECTRAL) && shininess)
				GetViewpointLineSpecularLight(line_normal,points[1]->Rotated,shininess,points[1]->Specular);

			if (points[1]->Rotated.Z < 0.0)
			{
				points[1]->Alpha = 2*radius*EyeToScreen/points[1]->Rotated.Z;
				if (points[1]->Alpha > 1.0f)
					points[1]->Alpha = 1.0f;
//				else
//					points[1]->Alpha *= points[1]->Alpha;
			}
			else
				points[1]->Alpha = 1.0f;

			draw_flags = points[0]->Flags & points[1]->Flags;

			if (!(draw_flags & PT3_NODRAW))
				DrawPolyClipped(PRIM_ALPHA,CL_CLIP_NONE,2,(RotPoint3D **)&points[0],(points[0]->Flags | points[1]->Flags) & PT3_CLIP_FLAGS);

			Swap4(&points[0],&points[1]);

		}
	}
	else
	{
		points[0] = &Points[0];
		points[1] = &Points[7];

		InterpolateCurve(pfrom,pto,vfrom,vto,0.0,new_point,line_vector);

		line_vector *= ViewMatrix;

		slice_up = right_ptr;
		slice_up %= line_vector;
		slice_up.Normalize();

		points[0]->Rotated.RotateInto(new_point,ViewMatrix);

		if (points[0]->Rotated.Z > ZClipPlane)
			points[0]->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			points[0]->Flags = PT3_ROTATED;
			PerspectPoint(points[0]);
		}

		line_normal = line_vector;
		to_eye = points[0]->Rotated;
		to_eye *= -1.0f;

		line_normal %= to_eye;
		line_normal %= line_vector;
		line_normal.Normalize();

		points[0]->Diffuse = color;
		light_value.SetValues(0.0f);

		GetViewpointLineLight(line_normal,points[0]->Rotated,light_value);

		points[0]->Diffuse *= light_value;

		points[0]->Specular.SetValues(0.0f);

		if (!(TerrainYesNos & YN_NO_SPECTRAL) && shininess)
			GetViewpointLineSpecularLight(line_normal,points[0]->Rotated,shininess,points[0]->Specular);

		point1_poly = FALSE;
		if (points[0]->Rotated.Z < 0.0)
		{
			points[0]->Alpha = 2*radius*EyeToScreen/points[0]->Rotated.Z;
			if (points[0]->Alpha > 1.0f)
			{
				if (points[0]->Alpha >= 1.2f)
					point1_poly = TRUE;
				points[0]->Alpha = 1.0f;
			}
//			else
//				points[0]->Alpha *= points[0]->Alpha;
		}
		else
			points[0]->Alpha = 1.0f;

		fwalk = hex_table;
		rpwalk = points[0];

		j = 6;
		while(j--)
		{
			rpwalk++;
			vertex_vector.SetValues(*fwalk++,slice_up);
			vertex_vector.AddScaledVector(*fwalk++,right_ptr);

			rpwalk->Rotated.SetValues(radius,vertex_vector);

			rpwalk->Rotated += points[0]->Rotated;

			if (rpwalk->Rotated.Z > ZClipPlane)
				rpwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				rpwalk->Flags = PT3_ROTATED;
				PerspectPoint(rpwalk);
			}

			rpwalk->Diffuse = color;
			light_value.SetValues(0.0f);

			GetViewpointLineLight(vertex_vector,rpwalk->Rotated,light_value);

			rpwalk->Diffuse *= light_value;

			rpwalk->Specular.SetValues(0.0f);

			if (!(TerrainYesNos & YN_NO_SPECTRAL) && shininess)
				GetViewpointLineSpecularLight(vertex_vector,rpwalk->Rotated,shininess,rpwalk->Specular);

			rpwalk->Alpha = points[0]->Alpha;
		}

		i = 10;
		while(i--)
		{

			InterpolateCurve(pfrom,pto,vfrom,vto,1.0f - (float)i/10.0f,new_point,line_vector);

			line_vector *= ViewMatrix;

			slice_up = right_ptr;
			slice_up %= line_vector;
			slice_up.Normalize();

			points[1]->Rotated.RotateInto(new_point,ViewMatrix);

			if (points[1]->Rotated.Z > ZClipPlane)
				points[1]->Flags = PT3_RZHIGH | PT3_ROTATED;
			else
			{
				points[1]->Flags = PT3_ROTATED;
				PerspectPoint(points[1]);
			}

			line_normal = line_vector;
			to_eye = points[1]->Rotated;
			to_eye *= -1.0f;

			line_normal %= to_eye;
			line_normal %= line_vector;
			line_normal.Normalize();

			points[1]->Diffuse = color;
			light_value.SetValues(0.0f);

			GetViewpointLineLight(line_normal,points[1]->Rotated,light_value);

			points[1]->Diffuse *= light_value;

			points[1]->Specular.SetValues(0.0f);

			if (!(TerrainYesNos & YN_NO_SPECTRAL) && shininess)
				GetViewpointLineSpecularLight(line_normal,points[1]->Rotated,shininess,points[1]->Specular);

			point2_poly = FALSE;
			if (points[1]->Rotated.Z < 0.0)
			{
				points[1]->Alpha = 2*radius*EyeToScreen/points[1]->Rotated.Z;
				if (points[1]->Alpha > 1.0f)
				{
					if (points[1]->Alpha >= 1.2f)
						point2_poly = TRUE;
					points[1]->Alpha = 1.0f;
				}
//				else
//					points[1]->Alpha *= points[1]->Alpha;

			}
			else
				points[1]->Alpha = 1.0f;

			fwalk = hex_table;
			rpwalk = points[1];
			j = 6;
			while(j--)
			{
				rpwalk++;
				vertex_vector.SetValues(*fwalk++,slice_up);
				vertex_vector.AddScaledVector(*fwalk++,right_ptr);

				rpwalk->Rotated.SetValues(radius,vertex_vector);

				rpwalk->Rotated += points[1]->Rotated;

				if (rpwalk->Rotated.Z > ZClipPlane)
					rpwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
				else
				{
					rpwalk->Flags = PT3_ROTATED;
					PerspectPoint(rpwalk);
				}

				rpwalk->Diffuse = color;
				light_value.SetValues(0.0f);

				GetViewpointLineLight(vertex_vector,rpwalk->Rotated,light_value);

				rpwalk->Diffuse *= light_value;

				rpwalk->Specular.SetValues(0.0f);

				if (!(TerrainYesNos & YN_NO_SPECTRAL) && shininess)
					GetViewpointLineSpecularLight(vertex_vector,rpwalk->Rotated,shininess,rpwalk->Specular);

				rpwalk->Alpha = points[1]->Alpha;
			}

			if (point1_poly || point2_poly)
			{
				if (point1_poly && point2_poly)
					gop_type = PRIM_DEFAULT;
				else
					gop_type = PRIM_ALPHA;

				j = 6;
				hex1to = points[0]+1;
				hex2to = points[1]+1;

				while(j--)
				{
					hex1from = hex1to++;
					hex2from = hex2to++;
					if (!j)
					{
						hex1to = points[0]+1;
						hex2to = points[1]+1;
					}

					poly_points[0] = hex1from;
					poly_points[1] = hex2from;
					poly_points[2] = hex2to;
					poly_points[3] = hex1to;

					draw_flags = hex1from->Flags & hex1to->Flags & hex2from->Flags & hex2to->Flags;

					if (!(draw_flags & PT3_NODRAW))
						DrawPolyClipped(gop_type,CL_CLIP_NONE,4,(RotPoint3D **)&poly_points[0],(hex1from->Flags | hex1to->Flags | hex2from->Flags | hex2to->Flags) & PT3_CLIP_FLAGS);

				}
			}
			else
			{
				draw_flags = points[0]->Flags & points[1]->Flags;

				if (!(draw_flags & PT3_NODRAW))
					DrawPolyClipped(PRIM_ALPHA,CL_CLIP_NONE,2,(RotPoint3D **)&points[0],(points[0]->Flags | points[1]->Flags) & PT3_CLIP_FLAGS);
			}

			Swap4(&points[0],&points[1]);
			Swap4(&point1_poly,&point2_poly);

		}


	}


}

void DrawTowedDecoy(PlaneParams *P,FPoint &rel)
{
	/* -----------------7/13/99 4:50PM---------------------------------------------------------------------
	/* first thing we have to do is calculate the position of the little guy
	/* ----------------------------------------------------------------------------------------------------*/

	FPoint decoy_offset;
	FPoint vector_down;
	FPoint vector_back;
	FPoint tail_point;
	FPoint right_ptr;
	FMatrix mat;
	float angle;
	float model_angle;

	angle = P->Knots * 1.0f/400.0;
	if (angle > 1.0f)
		angle = 1.0f;

	model_angle = 1.0 - angle;
	model_angle *= model_angle*PI*0.5;

	angle = 1.0-angle;
	angle *= angle;
	angle = 1.0-angle;
	angle *= PI*0.48f;

	tail_point = P->Type->TowedDecoyPosition;
	tail_point *= P->Attitude;
	decoy_offset = tail_point;
	tail_point += rel;

	vector_back.SetValues((float)(sin(angle)*75.0 FEET) * P->TowedDecoyPosition,P->Orientation.I);
	vector_down.SetValues(0.0f,-(float)(cos(angle)*75.0 FEET) * P->TowedDecoyPosition,0.0f);

	decoy_offset+=vector_down;
	decoy_offset-=vector_back;
	decoy_offset += rel;

	right_ptr.SetValues(P->Attitude.m_Data.RC.R0C0,P->Attitude.m_Data.RC.R1C0,P->Attitude.m_Data.RC.R2C0);

	DrawCurvedTube(tail_point,decoy_offset,vector_down,vector_back,DecoyCableColor,right_ptr,DecoyCableShininess,(float)(0.25 INCHES));

	mat.SetRadRPH(0.0f,model_angle,0.0f);
	mat *= P->Attitude;

	decoy_offset.X += mat.m_Data.RC.R0C2*0.75 FEET;
	decoy_offset.Y += mat.m_Data.RC.R1C2*0.75 FEET;
	decoy_offset.Z += mat.m_Data.RC.R2C2*0.75 FEET;

	Draw3DObjectMatrix(TowedDecoy,decoy_offset,mat);
}

void CalculateRefuelHosePosition(PlaneParams *P,int number)
{
	RefuelHoseData *rhd = &P->RefuelingHoses[number];

	rhd->ComputedLocation.SetValues(-0.9848f,P->Orientation.I);
	rhd->ComputedLocation.AddScaledVector(0.1736f,P->Orientation.K);

	rhd->ComputedLocation *= (float)(60 FEET) * rhd->Position;

	/* -----------------7/16/99 9:41AM---------------------------------------------------------------------
	/* have to do this because the attitude matrix may not have been set up yet.
	/* ----------------------------------------------------------------------------------------------------*/
	rhd->ComputedLocation.AddScaledVector( P->Type->RefuelingHosePositions[number].X,P->Orientation.J);
	rhd->ComputedLocation.AddScaledVector(-P->Type->RefuelingHosePositions[number].Y,P->Orientation.K);
	rhd->ComputedLocation.AddScaledVector(-P->Type->RefuelingHosePositions[number].Z,P->Orientation.I);

}

BOOL RedrawRefuelHose;
FPoint Safe_to_vector;
FPoint Safe_from_vector;
FPoint Safe_decoy_offset;
FPoint Safe_tail_point;
FPoint Safe_right_ptr;
FMatrix Safe_attitude;


void RedrawLastRefuelHose()
{
	/* -----------------10/15/99 4:46PM--------------------------------------------------------------------
	/* draw the fuel hose and basket really close without clipping or z_buffer
	/* ----------------------------------------------------------------------------------------------------*/
	SetD3DStates(0);

	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	float old_ZClipPlane = ZClipPlane;

	ZClipPlane = -0.01f;
	Set3DScreenClip();
	StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);


	DrawCurvedTube(Safe_tail_point,Safe_decoy_offset,Safe_from_vector,Safe_to_vector,RefuelHoseColor,Safe_right_ptr,RefuelHoseShininess,(float)(1.5 INCHES));
	Draw3DObjectMatrix(FuelBasket,Safe_decoy_offset,Safe_attitude);

	ZClipPlane = old_ZClipPlane;
	Set3DScreenClip();
	StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);

   	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	RedrawRefuelHose = FALSE;
}

void DrawRefuelHose(PlaneParams *P,int number,FPoint &rel)
{
	/* -----------------7/13/99 4:50PM---------------------------------------------------------------------
	/* first thing we have to do is calculate the position of the little guy
	/* ----------------------------------------------------------------------------------------------------*/
	FPoint to_vector;
	FPoint from_vector;
	FPoint decoy_offset;
	FPoint tail_point;
	FPoint right_ptr;

	RefuelHoseData *rhd = &P->RefuelingHoses[number];

	tail_point = P->Type->RefuelingHosePositions[number];
	tail_point *= P->Attitude;
	tail_point += rel;

	decoy_offset = rhd->ComputedLocation;
	decoy_offset += rel;

	if (rhd->ConnectedPosition)
		decoy_offset.AddScaledVector(rhd->ConnectedPosition,rhd->OffsetToConnectedPlane);

	to_vector.SetValues(15 FEET,P->Orientation.I);

	from_vector.SetValues(-0.9848f * (float)(15.0 FEET),P->Orientation.I);  //60 degrees
	from_vector.AddScaledVector(0.1736f * (float)(15.0 FEET),P->Orientation.K);

	right_ptr = P->Orientation.J;

	if ((decoy_offset.PartialLength() < (100 FEET)) && (ImagingMethod == IT_NORMAL) && (Camera1.CameraMode == CAMERA_COCKPIT))
	{
		RedrawRefuelHose = TRUE;
		Safe_to_vector =   to_vector;
		Safe_from_vector= from_vector;
		Safe_decoy_offset=decoy_offset;
		Safe_tail_point=  tail_point;
		Safe_right_ptr=   right_ptr;
		Safe_attitude=    P->Attitude;
	}
	else
	{
		DrawCurvedTube(tail_point,decoy_offset,from_vector,to_vector,RefuelHoseColor,right_ptr,RefuelHoseShininess,(float)(1.5 INCHES));

		Draw3DObjectMatrix(FuelBasket,decoy_offset,P->Attitude);
	}
}



FPoint MuzzleFlashColor(252.0f/255.0f,251.0f/255.0f,211.0f/255.0f);

void DrawMuzzleFlash(PlaneParams *P)
{

	if (P->MachineGunLight)
	{
		FPoint p1,p2;
		float intense = 0.75*(*P->MachineGunLight)->Intensity;

		p1.MakeVectorToFrom(P->WorldPosition,glob_camera->CameraLocation);
		p1.AddScaledVector(P->Type->MachineGun.X,P->Orientation.J); //rgt
		p1.AddScaledVector(-P->Type->MachineGun.Y,P->Orientation.K); //down
		p1.AddScaledVector(-P->Type->MachineGun.Z,P->Orientation.I); //fwd

		p2 = p1;
		p2.AddScaledVector(2.0f FEET * (*P->MachineGunLight)->Intensity + 3.0f FEET,P->Orientation.I);

		DrawUnlitAddBillboardRel(p1,p2,0.4f FEET,MuzzleFlash);

		p2 = p1;
		p2.AddScaledVector((-0.75f - intense) FEET,UpVector);
		p1.AddScaledVector((0.75f + intense) FEET,UpVector);

		DrawUnlitAddBillboardRel(p1,p2,(0.75f + intense) FEET,SpriteTypes[SPT_FLASH].Texture,0.25+0.25*intense);

	}

}



/* -----------------7/14/99 3:04PM---------------------------------------------------------------------
/*
/* ----------------------------------------------------------------------------------------------------*/

//  BYTE FormationLightColors[] = {0x75,0xF6,0xF5,0xF4};
BYTE FormationLightColors[] = {0x0,0x55,0xAA,0xFF};

void DrawPlane(PlaneParams  *P,FPoint &rel)
{
	BYTE vars[128];
	PlaneType *type = P->Type;
	ObjectHandler *plane_model;
	int i;
	float fworkvar;

	SetFLIRIntensity(0.75f);

	if ((P->FlightStatus & PL_PLANE_BLOWN_UP) && (plane_model = type->DestroyedModel))
		Draw3DObjectMatrix(plane_model,rel,P->Attitude);
	else
		if (plane_model = type->Model)
		{


			long jobtype = pDBAircraftList[P->AI.iPlaneIndex].lType;
			BYTE btempval;

			if(!SimPause && ((P->AI.Behaviorfunc == CTWaitingForLaunch) && (P->AI.lVar3 == 100)) || ((P->AI.Behaviorfunc == CTDoCarrierLaunch) && (P->AI.lVar3 <= 1) && (P->HeightAboveGround < (225.0f * FTTOWU))))
			{
				P->SBRudderOffset = Integrate(0.0f,0.0f, P->SBRudderOffset,
												   P->CS->RudderRange,
												   P->CS->SpeedBrakeRate,
												   DeltaTicks, -45.0f/2.0f);

				P->SBAileronOffset = Integrate(0.0f,0.0f, P->SBAileronOffset,
						 						   P->CS->AileronRange,
						 						   P->CS->SpeedBrakeRate,
						 						   DeltaTicks, -27.0f);  // was 45/8
			}
			else if(!SimPause && ((P->AI.Behaviorfunc == AIFlyTakeOff) && (P->Flaps)))
			{
				P->SBAileronOffset = Integrate(0.0f,0.0f, P->SBAileronOffset,
						 						   P->CS->AileronRange,
						 						   P->CS->SpeedBrakeRate,
						 						   DeltaTicks, -27.0f);  // was 45/8
			}

			if(jobtype & AIRCRAFT_TYPE_HELICOPTER)
			{
#if 0
				if(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 28)
				{
					vars[20] = (FrameCount & 1);
					vars[21] = 2 - (FrameCount % 3);  // prop was rotating wrong way
					vars[22] = (FrameCount & 3);

					if(vars[20] == 0)
					{
						vars[20] = 2;
					}
					if(vars[21] == 0)
					{
						vars[21] = 3;
					}
					//  Sometime put stuff here to turn off props when they are destroyed.  SRE
					vars[30] = 1;
					vars[31] = 1;
				}
#endif
				if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 86) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 87))
				{
					vars[21] = ((((P->V == 0) && (P->OnGround)) && (P->AI.Behaviorfunc != AISARPickUp)) || (P->FlightStatus & PL_STATUS_CRASHED)) ? 3 : (FrameCount % 3);
				}
				else
				{
					vars[24] = ((((P->V == 0) && (P->OnGround)) && (P->AI.Behaviorfunc != AISARPickUp)) || (P->FlightStatus & PL_STATUS_CRASHED)) ? 3 : (FrameCount % 3);
				}
			}
			else if(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 82)   //  MV-22
			{
				vars[23] = ((((P->V == 0) && (P->OnGround)) && (P->AI.Behaviorfunc != AISARPickUp)) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_LEFT_INBOARD)) ? 3 : 2 - (FrameCount % 3);
				vars[24] = ((((P->V == 0) && (P->OnGround)) && (P->AI.Behaviorfunc != AISARPickUp)) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_RIGHT_INBOARD)) ? 3 : 2 - (FrameCount % 3);
				if(P->IndicatedAirSpeed < 120.0f)
				{
					fworkvar = 255 - ((P->IndicatedAirSpeed / 120.0f) * 255.0f);
					vars[1] = (BYTE)fworkvar;
				}
				else
				{
					vars[1] = 0;
				}
			}
			else if(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 63)
			{
				vars[24] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_LEFT_INBOARD)) ? 3 : 2 - (FrameCount % 3);
				vars[23] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_RIGHT_INBOARD)) ? 3 : 2 - (FrameCount % 3);
			}
			else if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 14) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 47) ||(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 48) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 49) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 79))
			{
//				vars[24] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_LEFT_OUTBOARD)) ? 3 : (FrameCount % 3);
//				vars[23] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_LEFT_INBOARD)) ? 3 : (FrameCount % 3);
				vars[24] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_LEFT_OUTBOARD)) ? 3 : 2 - (FrameCount % 3);
				vars[23] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_LEFT_INBOARD)) ? 3 : 2 - (FrameCount % 3);
				vars[22] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_RIGHT_INBOARD)) ? 3 : (FrameCount % 3);
				vars[21] = ((P->V == 0) || (P->FlightStatus & PL_STATUS_CRASHED) || (P->SystemsStatus & PL_ENGINE_WING_RIGHT_OUTBOARD)) ? 3 : (FrameCount % 3);
			}
			else
			{
				vars[20] = FrameCount & 1;
				vars[21] = FrameCount % 3;
				vars[22] = FrameCount & 3;
			}

			if (type->TypeNumber == PT_F18e) //really f15
			{
				char	szBuff[32];

				sprintf (szBuff, "%04d", P->TailSerial);

				/* left and right pods respectively */
				vars[3] = P->WeapLoad[LANTIRN_L].Count;	// AN/AAQ-14
				vars[4] = P->WeapLoad[LANTIRN_R].Count;	// AN/AAQ-13

				/* Tail designation */
				vars[40] = MultiPlayer ? P->AI.iSide : P->TailBase;

				/* Tail color swatch */
				vars[46] = P->TailSquadron;

				/* Tail numbers */
				vars[41] = P->TailYear - 1977;	//((P-Planes)/2)%4+10;		//10
				vars[42] = szBuff[0] - '0';
				vars[43] = szBuff[1] - '0';
				vars[44] = szBuff[2] - '0';
				vars[45] = szBuff[3] - '0';

				/* formation lights off */

				vars[47] = FormationLightColors[(P->Status & PL_FORMATION_LIGHTS) >> 12];

				/* make wheels roll */

				if (P->OnGround && (P->V > 1.0))
					vars[20] = FrameCount & 1;
				else
					vars[20] = 0;

				/* turn nose wheel based on rudder position */
				//vars[18] = 256.0 - (P->RudderPedalsCommandedPos + 128.0);

				float TmpRudder = P->RudderPedalsCommandedPos;
				if (TmpRudder > 114.0)
					TmpRudder = 114.0;
				else
				if (TmpRudder < -114.0)
					TmpRudder = -114.0;

				vars[18] = 228.0 - (TmpRudder + 114.0);

			}
			else if (type->TypeNumber == 61)	// F/A-18E
			{
				char	szBuff[32];

				sprintf (szBuff, "%04d", P->TailSerial);

				/* left and right pods respectively */
//				vars[3] = P->WeapLoad[LANTIRN_L].Count;	// AN/AAQ-14

				// WING UP ROTATION
				vars[4] = (BYTE)(255.0 * (P->WingUp)/(120.0f));

				//  HOOK
				vars[9] = (BYTE)(255.0 * (P->TailHook)/(90.0f));

				if(P->RefuelProbeCommandedPos > 1.0f)
				{
					vars[10] = 255;	// PROBE OUT
				}
				else
				{
					vars[10] = 0;	// PROBE IN
				}

				//  CANOPY
				vars[14] = (BYTE)(255.0 * (P->Canopy)/(90.0f));

 				//  LEADING EDGE FLAPS
				vars[17] = (BYTE)(255.0 * (P->LeadingEdge)/(30.0f));

				/* Tail designation */
				vars[40] = MultiPlayer ? P->AI.iSide : P->TailBase;

				/* Tail color swatch */
				vars[30] = P->TailSquadron;  // was 40

				/* Tail numbers */
				vars[31] = P->TailSquadron;
				vars[32] = P->TailSquadron;
				vars[41] = P->TailYear - 1977;	//((P-Planes)/2)%4+10;		//10
				vars[42] = szBuff[0] - '0';  // was 42
				vars[33] = szBuff[1] - '0';	 // was 43
				vars[34] = szBuff[2] - '0';  // was 44
				vars[35] = szBuff[3] - '0';  // was 45

				/* formation lights off */

				vars[20] = FormationLightColors[(P->Status & PL_FORMATION_LIGHTS) >> 12];  //  was vars[47]

				/* make wheels roll */

				if (P->OnGround && (P->V > 1.0))
					vars[38] = FrameCount & 1;
				else
					vars[38] = 2;

				/* turn nose wheel based on rudder position */
				//vars[13] = 256.0 - (P->RudderPedalsCommandedPos + 128.0);  //  18

				float TmpRudder = P->RudderPedalsCommandedPos;
				if (TmpRudder > 114.0)
					TmpRudder = 114.0;
				else
				if (TmpRudder < -114.0)
					TmpRudder = -114.0;

			//	vars[13] = 228.0 - (TmpRudder + 114.0);
			vars[13] = 228.0 - (TmpRudder + 114.0);


				if(P->WeapLoad[RIGHT11_STATION].Count)
				{
					if(pDBWeaponList[P->WeapLoad[RIGHT11_STATION].WeapIndex].lWeaponID == 109)
					{
						vars[64] = 1;
					}
					else
					{
						vars[64] = 2;
					}
				}
				else
				{
					vars[64] = 0;
				}
				if(P->WeapLoad[LEFT1_STATION].Count)
				{
					if(pDBWeaponList[P->WeapLoad[LEFT1_STATION].WeapIndex].lWeaponID == 109)
					{
						vars[65] = 1;
					}
					else
					{
						vars[65] = 2;
					}
				}
				else
				{
					vars[65] = 0;
				}
			}
			else if((type->TypeNumber == 4) || (type->TypeNumber == 62))	// F-14s
			{
				vars[1] = P->AI.cFXarray[0];
			}


			/* Landing gear, control surfaces and after burners are the same on all planes */

			if(jobtype & AIRCRAFT_TYPE_TANKER)
			{
				vars[5] = P->AI.cFXarray[0];
				vars[6] = P->AI.cFXarray[1];
				if(!vars[6])
					vars[7] = 0;
				else
					vars[7] = 1;


#if 0
				float fworkval;
				int workval;

				fworkval = (float)(((signed int)GetJoyYWithDeadY()-32768) >> 8);
				fworkval = (fworkval / 128) * 30;

				if(fworkval < -5)
				{
					workval = fworkval + 5;
					if(workval > -5)
					{
						P->AI.cFXarray[2] = (TANKER_DOWN|TANKER_UD_OK|TANKER_U_UP|TANKER_D_DOWN);
					}
					else if(workval > -10)
					{
						P->AI.cFXarray[2] = (TANKER_DOWN|TANKER_U_UP|TANKER_D_DOWN);
					}
					else if(workval > -15)
					{
						P->AI.cFXarray[2] = (TANKER_WAY_DOWN|TANKER_DOWN|TANKER_U_UP|TANKER_D_DOWN);
					}
					else if(workval > -20)
					{
						P->AI.cFXarray[2] = (TANKER_WAY_DOWN|TANKER_U_UP|TANKER_D_DOWN);
					}
					else
					{
						P->AI.cFXarray[2] = (TANKER_U_UP|TANKER_D_DOWN);
					}
				}
				else if(fworkval > 5)
				{
					workval = fworkval - 5;
					if(workval < 5)
					{
						P->AI.cFXarray[2] = (TANKER_UP|TANKER_UD_OK|TANKER_U_UP|TANKER_D_DOWN);
					}
					else if(workval < 10)
					{
						P->AI.cFXarray[2] = (TANKER_UP|TANKER_U_UP|TANKER_D_DOWN);
					}
					else if(workval < 15)
					{
						P->AI.cFXarray[2] = (TANKER_WAY_UP|TANKER_UP|TANKER_U_UP|TANKER_D_DOWN);
					}
					else if(workval < 20)
					{
						P->AI.cFXarray[2] = (TANKER_WAY_UP|TANKER_U_UP|TANKER_D_DOWN);
					}
					else
					{
						P->AI.cFXarray[2] = (TANKER_U_UP|TANKER_D_DOWN);
					}
				}
				else
				{
					P->AI.cFXarray[2] = (TANKER_UD_OK|TANKER_U_UP|TANKER_D_DOWN);
				}

				fworkval = (float)((GetJoyZ())>>9);

				fworkval = (((128.0f-fworkval)/128.0f)*60.0f) - 30;

				if(fabs(fworkval) < 5)
				{
					P->AI.cFXarray[3] = (TANKER_FA_OK|TANKER_F_FORE|TANKER_A_AFT);
				}
				else if(fworkval < 0)
				{
					workval = fworkval + 5;
					if(workval > -5)
					{
						P->AI.cFXarray[3] = (TANKER_AFT|TANKER_FA_OK|TANKER_F_FORE|TANKER_A_AFT);
					}
					else if(workval > -10)
					{
						P->AI.cFXarray[3] = (TANKER_AFT|TANKER_F_FORE|TANKER_A_AFT);
					}
					else if(workval > -15)
					{
						P->AI.cFXarray[3] = (TANKER_WAY_AFT|TANKER_AFT|TANKER_F_FORE|TANKER_A_AFT);
					}
					else if(workval > -20)
					{
						P->AI.cFXarray[3] = (TANKER_WAY_AFT|TANKER_F_FORE|TANKER_A_AFT);
					}
					else
					{
						P->AI.cFXarray[3] = (TANKER_F_FORE|TANKER_A_AFT);
					}
				}
				else
				{
					workval = fworkval - 5;
					if(workval < 5)
					{
						P->AI.cFXarray[3] = (TANKER_FORE|TANKER_FA_OK|TANKER_F_FORE|TANKER_A_AFT);
					}
					else if(workval < 10)
					{
						P->AI.cFXarray[3] = (TANKER_FORE|TANKER_F_FORE|TANKER_A_AFT);
					}
					else if(workval < 15)
					{
						P->AI.cFXarray[3] = (TANKER_WAY_FORE|TANKER_FORE|TANKER_F_FORE|TANKER_A_AFT);
					}
					else if(workval < 20)
					{
						P->AI.cFXarray[3] = (TANKER_WAY_FORE|TANKER_F_FORE|TANKER_A_AFT);
					}
					else
					{
						P->AI.cFXarray[3] = (TANKER_F_FORE|TANKER_A_AFT);
					}
				}
#endif

				vars[12] = (P->AI.cFXarray[2] & TANKER_D_DOWN) ? 1 : 0;
				vars[19] = (P->AI.cFXarray[2] & TANKER_WAY_DOWN) ? 248 : 207;
				vars[20] = (P->AI.cFXarray[2] & TANKER_DOWN) ? 249 : 207;
				vars[21] = (P->AI.cFXarray[2] & TANKER_UD_OK) ? 249 : 207;
				vars[22] = (P->AI.cFXarray[2] & TANKER_UP) ? 249 : 207;
				vars[23] = (P->AI.cFXarray[2] & TANKER_WAY_UP) ? 248 : 207;
				vars[13] = (P->AI.cFXarray[2] & TANKER_U_UP) ? 1 : 0;

				vars[11] = (P->AI.cFXarray[3] & TANKER_A_AFT) ? 1 : 0;
				vars[14] = (P->AI.cFXarray[3] & TANKER_WAY_AFT) ? 248 : 207;
				vars[15] = (P->AI.cFXarray[3] & TANKER_AFT) ? 249 : 207;
				vars[16] = (P->AI.cFXarray[3] & TANKER_FA_OK) ? 249 : 207;
				vars[17] = (P->AI.cFXarray[3] & TANKER_FORE) ? 249 : 207;
				vars[18] = (P->AI.cFXarray[3] & TANKER_WAY_FORE) ? 248 : 207;
				vars[10] = (P->AI.cFXarray[3] & TANKER_F_FORE) ? 1 : 0;

#if 0
				//  All On
				vars[12] = 1;
				vars[19] = 248;
				vars[20] = 249;
				vars[21] = 249;
				vars[22] = 249;
				vars[23] = 248;
				vars[13] = 1;

				vars[11] = 1;
				vars[14] = 248;
				vars[15] = 249;
				vars[16] = 249;
				vars[17] = 249;
				vars[18] = 248;
				vars[10] = 1;

				// all off
				vars[12] = 0;
				vars[19] = 0;
				vars[20] = 0;
				vars[21] = 0;
				vars[22] = 0;
				vars[23] = 0;
				vars[13] = 0;

				vars[11] = 0;
				vars[14] = 0;
				vars[15] = 0;
				vars[16] = 0;
				vars[17] = 0;
				vars[18] = 0;
				vars[10] = 0;
#endif

			}
			else if(jobtype & AIRCRAFT_TYPE_C3)
			{
				if (!SimPause)
				{
					btempval = (BYTE)((43 * DeltaTicks) / 1000);
					if(btempval == 0)
						btempval = 1;
					P->AI.cFXarray[0] = P->AI.cFXarray[0] + btempval;
				}
				vars[5] = P->AI.cFXarray[0];
			}
			else
			{
				SetFLIRIntensity(0.5f + (P->LeftThrustPercent + P->RightThrustPercent)* (1.0f/400.0f));
				/* burners */
				if (P->LeftThrustPercent > 80.0f)
				{
					vars[7] = 1;
					vars[5] = (BYTE)(255.0f - ((100.0f - P->LeftThrustPercent) * 255.0f/20.0f));
				}
				else
				{
					vars[7] = 0;
					vars[5] = 0;
				}

				if (P->RightThrustPercent > 80.0f)
				{
					vars[8] = 1;
					vars[6] = (BYTE)(255.0f - ((100.0f - P->RightThrustPercent) * 255.0f/21.0f));
				}
				else
				{
					vars[8] = 0;
					vars[6] = 0;
				}
			}

			if(!(jobtype & AIRCRAFT_TYPE_TANKER))
			{
				if((type->TypeNumber == 61) || (type->TypeNumber == 58))
				{
					/* speed brake */
					vars[25] = (BYTE)((P->SpeedBrake*1.0f/90.0f)*(float)0xFF);
				}

				/* turkey flaps */
				vars[15] = 0x0080 - (int)((P->SymetricalElevator + P->DifferentialElevator)*((float)0x00FF/180.0));
				vars[16] = 0x0080 - (int)((P->SymetricalElevator - P->DifferentialElevator)*((float)0x00FF/180.0));

				/* ailerons */
//original				vars[12] = 0xFF-(BYTE)(255.0 * (( P->Aileron - P->SBAileronOffset+P->FLPSAileronOffset) - P->CS->AileronRange.Low)/(P->CS->AileronRange.High - P->CS->AileronRange.Low));
//original				vars[18] = 0xFF-(BYTE)(255.0 * ((-P->Aileron - P->SBAileronOffset+P->FLPSAileronOffset) - P->CS->AileronRange.Low)/(P->CS->AileronRange.High - P->CS->AileronRange.Low));


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

				/* Flaps */
				vars[11] = (BYTE)(255.0 * (P->Flaps - P->CS->FlapRange.Low)/(P->CS->FlapRange.High - P->CS->FlapRange.Low));

				// leading edge extensions
				vars[17] = (BYTE)(255.0 * (P->LEFlaps - P->CS->LEFlapRange.Low)/(P->CS->LEFlapRange.High - P->CS->LEFlapRange.Low));

				/* Rudder */
				vars[13] = 0xFF-(BYTE)(255.0 * ((P->Rudder + P->SBRudderOffset) - P->CS->RudderRange.Low)/(P->CS->RudderRange.High - P->CS->RudderRange.Low));
				vars[19] = 0xFF-(BYTE)(255.0 * ((P->Rudder - P->SBRudderOffset) - P->CS->RudderRange.Low)/(P->CS->RudderRange.High - P->CS->RudderRange.Low));

			}

			if (type->TypeNumber == 61)
			{
				if (P->LandingGearStatus)
					vars[1] = 1;        // opens gear doors
				else
					vars[1] = 0;
			}
			else
			{
				if (P->LandingGearStatus)
					vars[2] = 0;
				else
					vars[2] = 0xFF;
			}

			//  HOOK
			vars[9] = (BYTE)(255.0 * (P->TailHook)/(90.0f));

			if (P->LandingGearStatus || (P->FlightStatus & PL_GEAR_DOWN_LOCKED))
			{
				/* Lights */
				//  For the Patch.  Lights of the same type as regular planes were added to the F/A-18 so we need to
				//  Activate those as well as the glows
//				if(type->TypeNumber != 61)
//				{
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
//				}
//				else
				if(type->TypeNumber == 61)
				{
					vars[23] = 0xF9; /* green light */
					vars[21] = 0xF8; /* red light */
					if ((GameLoop + (int)P) &  0x20)
						vars[22] = 0xF8;
					else
						vars[22] = 0x0;
					vars[24] = 0xFD; /* white red light */

					if ((P->Attitude.m_Data.RC.R0C0*ViewVector.X + P->Attitude.m_Data.RC.R1C0*ViewVector.Y + P->Attitude.m_Data.RC.R2C0*ViewVector.Z) < 0.0)
						vars[54] = 0xF9;
					else
						vars[54] = 0xF8;
				}
				if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 61))
				{
					vars[ 1] = 0xFF;
				}
				else if(!((type->TypeNumber == 4) || (type->TypeNumber == 62) || (type->TypeNumber == 82)))	// F-14s and MV-22
				{
					vars[1] = 0;
				}
			}
			else
			{
				if (P->Status & PL_RUNNING_LIGHTS)
				{
//					if(type->TypeNumber != 61)
//					{
						vars[50] = 0xF9; /* green light */
						vars[51] = 0xF8; /* red light */
						if ((GameLoop + (int)P) &  0x20)
							vars[52] = 0xF8;
						else
							vars[52] = 0x0;  //  0xA9
						vars[53] = 0xFD; /* white red light */
						if ((P->Attitude.m_Data.RC.R0C0*ViewVector.X + P->Attitude.m_Data.RC.R1C0*ViewVector.Y + P->Attitude.m_Data.RC.R2C0*ViewVector.Z) < 0.0)
							vars[54] = 0xF9;
						else
							vars[54] = 0x0;
//					}
//					else
					if(type->TypeNumber == 61)
					{
						vars[23] = 0xF9; /* green light */
						vars[21] = 0xF8; /* red light */
						if ((GameLoop + (int)P) &  0x20)
							vars[22] = 0xF8;
						else
							vars[22] = 0x0;  // 0xA9
						vars[24] = 0xFD; /* white red light */
						if ((P->Attitude.m_Data.RC.R0C0*ViewVector.X + P->Attitude.m_Data.RC.R1C0*ViewVector.Y + P->Attitude.m_Data.RC.R2C0*ViewVector.Z) < 0.0)
							vars[54] = 0xF9;
						else
							vars[54] = 0xF8;
					}
				}
				else
				{
//					if(type->TypeNumber != 61)
//					{
						vars[50] = 0x0;//x6C  xDD; /* green light */
						vars[51] = 0x0;//xA9  xDD; /* red light */
						vars[52] = 0x0;//xA9  xDD; /* flash red light */
						vars[53] = 0x0;//xCB  xDD; /* white red light */
						vars[54] = 0;
//					}
//					else
					if(type->TypeNumber == 61)
					{
						vars[23] = 0x0;//x6C  xDD; /* green light */
						vars[21] = 0x0;//xA9  xDD; /* red light */
						vars[22] = 0x0;//xA9  xDD; /* flash red light */
						vars[24] = 0x0;//xCB  xDD; /* white red light */
						vars[54] = 0;
					}
				}
				if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 61))
				{
					vars[ 1] = 0;
				}
				else if(!((type->TypeNumber == 4) || (type->TypeNumber == 62) || (type->TypeNumber == 82)))	// F-14s and MV-22
				{
					vars[ 1] = 0xFF;
				}
			}

			if(pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 58)
			{
				vars[0] = 0xFF - P->LandingGear;
			}
			else if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 0) || (pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 11))
			{
//				vars[0] = 0xFF - P->LandingGear;
				vars[0] = P->LandingGear;
			}
			else if((pDBAircraftList[P->AI.iPlaneIndex].lPlaneID == 61))  // the f18
			{
				/* -----------------7/31/99 11:41AM--------------------------------------------------------------------
				/* here's where the magic goes
				/* ----------------------------------------------------------------------------------------------------*/
				vars[0] = (P->LandingGear) ? 1 : 0;

				if (vars[0])
				{
					vars[28] = 256.0 - (P->RudderPedalsCommandedPos + 128.0);


					vars[27] = 0xFF - P->LandingGear;

					if (P->OnGround)
					{
						vars[26] = 255;
					 	vars[29] = (BOOL)((P->OnGround == 2) && ((P->AI.Behaviorfunc == CTWaitingForLaunch) || (P->AI.Behaviorfunc == CTDoCarrierLaunch)));
					}
					else
						if (!vars[27])
						{
							if (P->HeightForGear && (P->HeightAboveGround <= P->HeightForGear))
							{
								float fwork = (P->HeightForGear - P->HeightAboveGround) / (P->Type->GearDownHeight*0.5f);
								if (fwork > 1.0f)
									vars[26] = 255;
								else
									vars[26] = (BYTE)(255.0f * fwork);
							}
							else
								vars[26] = 0;

							vars[29] = 0;
						}

				}

				vars[36] = vars[37] = (BYTE)(P->VaporAmnt * 255.0f);

			}
			else
			{
				vars[0] = (P->LandingGear) ? 1 : 0;
			}

			if(WIsWeaponPlane(P))
			{
				if((P->AI.lPlaneID == 70) || (P->AI.lPlaneID == 77) || (P->AI.lPlaneID == 85))
				{
					vars[0] = 1;	// Weapon Planes Extend Stuff.
				}
				else if(P->Smoke[SMOKE_ENGINE_REAR] && (P->AI.Behaviorfunc == WeaponAsPlane))
				{
					vars[0] = 1;	// Weapon Planes Extend Stuff.
				}
				else
				{
					vars[0] = 0;	// Weapon Planes Extend Stuff.
				}
			}

			if ((type->TypeNumber == PT_F18e) && (g_Settings.gr.dwGraph & GP_GRAPH_WEAPONS_ON_PLANE))
			{
				for(i=0; i<MAX_SUBPARTS; i++)
				{
					OurSubs[i].AllVars = bSubVars[i];
				}
				GetSubObjects(P);
			}
			else if ((type->TypeNumber == 61) && (g_Settings.gr.dwGraph & GP_GRAPH_WEAPONS_ON_PLANE))
			{
				for(i=0; i<MAX_SUBPARTS; i++)
				{
					OurSubs[i].AllVars = bSubVars[i];
				}
				GetSubObjects(P);
			}
			else
				memcpy(OurSubs,SourceSubs,sizeof(OurSubs));

			for (i=0; i<MAX_CONNECTED_PARTS; i++)
			{
				OurSubs[i].Object  = type->SubParts[i];
				OurSubs[i].AllVars = vars;
			}


			if ((P == PlayerPlane) && SkipThisDot)
			{
				OurSubs[SkipThisDot].Flags = NULL;
				OurSubs[SkipThisDot].Object = NULL;
			}

			Draw3DObjectMatrix(plane_model,rel,P->Attitude/* P->Roll,P->Pitch,P->Heading*/,vars,OurSubs);

			if (P->TowedDecoyPosition)
				DrawTowedDecoy(P,rel);

			i = MAX_REFUELING_HOSES;
			while (i--)
			{
				if (P->RefuelingHoses[i].Position)
				{
					if (P->RefuelingHoses[i].ConnectedPlane)
						AttachPlanePointToRefuelingHose(P,P->RefuelingHoses[i].ConnectedPlane,i);

					DrawRefuelHose(P,i,rel);
				}

			}

			DrawMuzzleFlash(P);


		}

}

void DrawWeapon(void *w,FPoint &location)
{
	BYTE vars[4];
	WeaponParams *W = (WeaponParams *)w;
	WeaponType *type;
	type = W->Type;

	//  If phantom weapon for WeaponAsPlane then don't draw.
	if(W->WeaponMove == WMovePhantomWeapon)
		return;

	if (W->SortType == BULLET_OBJECT)
		RenderBullet(location,W->Vel,(BOOL)(W->Flags & RED_TRACER));
	else
	{
		*(DWORD *)&vars[0] = 0;

		SetFLIRIntensity(1.0f);
		int Index = AIGetWeaponIndex(W->Type->TypeNumber);
		if (pDBWeaponList[Index].iWeaponType == WEAPON_TYPE_CHAFF)
			return;

		if(pDBWeaponList[Index].iWeaponType == WEAPON_TYPE_FLARE)
		{
			RotPoint3D *point, p;
			point = &p;

			if (p.Rotated.RotateAndPerspect(&p.fSX,&p.fSY,&ViewMatrix,&location))
			{
		  		FPoint flare_color(1.0,0.9,0.95);

		  		RenderCounterMeasureFlare(point,flare_color);
			}

			return;
		}

		if (type->Model)
		{
	//				FMatrix tempmatrix;
	//				tempmatrix.Identity();
	//				Draw3DObjectMatrix(type->Model,draw_guy->RelativePosition,tempmatrix,&vars[0]);

			if(W->Kind == EJECTION_SEAT)
			{
				if(pDBWeaponList[Index].lWeaponID == 104)
				{
					if(!(W->Flags & WEP_GUY_IN_CHAIR))
						vars[0] = 1;
				}
				else
				{
					if(W->Flags & WEP_CHUTE_OUT)
					{
						vars[0] = 1;
					}
					else if (!(W->Flags & WEP_CHUTE_HALF_OUT))
					{
						vars[0] = 2;
					}
				}
			}
			else
			{
				if((W->Flags & WEP_JETTISONED) || (W->Flags2 & BOMB_FORCE_FREEFALL))
				{
					vars[0] = 0;
				}
				else
				{
					if (!(W->Flags & DURANDEL_STAGE2))
					{
						if ((W->SortType != BOMB_OBJECT) || (W->lBurnTimer < 0))
							vars[0] = 1;
						else
							vars[0] = 0;
					}
					else
						vars[0] = 2;
				}
			}

			if ((W->SortType == BOMB_OBJECT) || ((W->SortType == MISSILE_OBJECT) && (W->Kind != EJECTION_SEAT)))
			{
				FPoint x,y,z;

				z.SetValues(-W->Vel.X,-W->Vel.Y,-W->Vel.Z);
				z.Normalize();

				if (fabs(z.Y) < 0.98f)
					y.SetValues(0.0f,1.0f,0.0f);
				else
					y.SetValues(0.0f,0.0f,1.0f);

				x = y;
				x %= z;
				x.Normalize();

				y = z;
				y %= x;
				y.Normalize();

				W->Attitude.m_Data.RC.R0C0 = x.X;
				W->Attitude.m_Data.RC.R1C0 = x.Y;
				W->Attitude.m_Data.RC.R2C0 = x.Z;

				W->Attitude.m_Data.RC.R0C1 = y.X;
				W->Attitude.m_Data.RC.R1C1 = y.Y;
				W->Attitude.m_Data.RC.R2C1 = y.Z;

				W->Attitude.m_Data.RC.R0C2 = z.X;
				W->Attitude.m_Data.RC.R1C2 = z.Y;
				W->Attitude.m_Data.RC.R2C2 = z.Z;

				if((W->SortType != BOMB_OBJECT) && (pDBWeaponList[Index].lWeaponID != 116))
				{
					if (W->lBurnTimer < 0)
						vars[0] = 0;
				}
			}
			else if(W->Kind != EJECTION_SEAT)
			{
				if (W->lBurnTimer < 0)
					vars[0] = 0;
				else
					vars[1] = (BYTE)(GameLoop&0xFF);  // Run the flame sprite through it's animation

			}

			if((pDBWeaponList[Index].lWeaponID == 133) && (!(W->Flags & WEP_JETTISONED)))  //  5 Inch Rockets
			{
				Draw3DObjectMatrix(pRocket5,location,W->Attitude,&vars[0]);
			}
			else if(((pDBWeaponList[Index].lWeaponID == 134) || (pDBWeaponList[Index].lWeaponID == 176)) && (!(W->Flags & WEP_JETTISONED)))  //  2.75 Inch Rockets
			{
				Draw3DObjectMatrix(pRocket275,location,W->Attitude,&vars[0]);
			}
			else
			{
				Draw3DObjectMatrix(type->Model,location,W->Attitude,&vars[0]);
			}
		}
	}

}

void F18Drawer(ObjectSortHeader *draw_guy)
{
//	float old_man_made;
	switch(draw_guy->Type)
	{
		case PLANE_OBJECT:
			break;

		case PARTICLE_OBJECT:
			//DrawParticle( (Particle *) draw_guy->Object);
			break;

	    case WAYPOINT_OBJECT:
			break;

		case LANDING_GATE_OBJECT:
			break;

		case SMOKE_TRAIL_OBJECT:

//			DrawSmokeTrail((SmokeDrawer *)draw_guy->Object);
			break;

		case SPRITE_OBJECT:
//			DrawSprite((Sprite *)draw_guy->Object);
			break;

		case SHADOW_OBJECT:
			break;

		case AAA_OBJECT :
			break;

		case MISSILE_OBJECT:
		case BOMB_OBJECT:
			break;

		case SHOCKWAVE_OBJECT :
			break;

		case CRATER_OBJECT :
			break;

		case VEHICLE_OBJECT:
		break;

		case BULLET_OBJECT:
			break;
	}
}

void CockpitLightOn()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	_splitpath(FadingFilename,drive,dir,fname,NULL);

	if (fname[strlen(fname)-2] != '-')
	{
		strcat(fname,"-l");
		_makepath(FadingFilename,drive,dir,fname,"clr");

//		ReadFadeTable();
		WorldParams.CurrentPaletteNumber = -1;
	}
}

void CockpitLightOff()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	_splitpath(FadingFilename,drive,dir,fname,NULL);

	if (fname[strlen(fname)-2] == '-')
	{
		fname[strlen(fname)-2] = 0;
		_makepath(FadingFilename,drive,dir,fname,"clr");

//		ReadFadeTable();
		WorldParams.CurrentPaletteNumber = -1;
	}
}

void SetupPlaneShadows(FPointDouble &eye)
{
	PlaneParams *P;
	FPointDouble shadow_vector;
	FPointDouble shadow_cast_point;
	FPointDouble shadow_ground_point;
	FPoint rel_pos;
	float alpha;
	BOOL drew_it;
	BYTE vars[128];
	ShadowInstance *our_shad;
	DWORD value;

	ZeroMemory(vars,sizeof(vars));

	P = &Planes[0];
	VisiblePlanes = NULL;
	shadow_vector.SetValues(-6000.0 FEET,ShadowWorldZ);

	while (P <= LastPlane)
	{
		drew_it = FALSE;
		if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED) && (P->Type && P->Type->Model) && (!(P->AI.iAIFlags2 & AIINVISIBLE)))
		{
			if (CloseEnoughToDraw(P->WorldPosition))
			{
				P->DoAttitude(P);

				P->NextVisiblePlane = VisiblePlanes;
				VisiblePlanes = P;

				if (P->OnGround != 2)
				{
					shadow_cast_point = P->WorldPosition;
					shadow_cast_point += shadow_vector;
					if (!LOS(&P->WorldPosition,&shadow_cast_point,&shadow_ground_point,HI_RES_LOS,TRUE))
					{
						shadow_cast_point.MakeVectorToFrom(shadow_ground_point,P->WorldPosition);

						alpha = 1.0f - (float)shadow_cast_point.PartialLength() * (1.0f/(5000.0 FEET*5000.0 FEET));

						if (alpha > 0.0f)
						{
							vars[0] = (P->LandingGear) ? 1 : 0;
					   		vars[4] = (BYTE)(255.0 * (P->WingUp)/(120.0f));

							value = vars[0]| ((DWORD)vars[4] << 7);

							GetObjectShadowEntry(&P->Shadow,P->Type->Model,P->Attitude,FALSE,vars,0x1FF,value);

							if(P->Shadow)
								InsertShadow(0,(DWORD)P->Shadow,shadow_ground_point,eye,alpha);

							drew_it = TRUE;
						}
					}
				}
				else
				{
					vars[0] = (P->LandingGear) ? 1 : 0;
					vars[4] = (BYTE)(255.0 * (P->WingUp)/(120.0f));

					value = vars[0]| ((DWORD)vars[4] << 7);

 					GetObjectShadowEntry(&P->Shadow,P->Type->Model,P->Attitude,FALSE,vars,0x1FF,value);

					if (P->Shadow)
					{
						rel_pos.MakeVectorToFrom(P->WorldPosition,eye);

						our_shad = LastShadow++;
						our_shad->XD = -(rel_pos * ShadowWorldX);
						our_shad->YD = -(rel_pos * ShadowWorldY);
						our_shad->AlphaMod = 1.0f;
						our_shad->Map= P->Shadow;
						our_shad->Next = MovingVehicles[P->AI.iHomeBaseId].PlaneShadows;
						our_shad->Flags = 0;
						MovingVehicles[P->AI.iHomeBaseId].PlaneShadows = our_shad;
					}
					drew_it = TRUE;
				}

			}
		}

		if ((!drew_it) && (P->Shadow))
		{
			ReleaseShadowEntry(&P->Shadow);
			P->Shadow = NULL;
		}

		P++;
	}

}

void DisplayPlanes(CameraInstance *camera,void *our_object)
{
	PlaneParams *P;
	FPoint vector;

	if (VisiblePlanes)
	{
		P = VisiblePlanes;
		while(P)
		{
			if (P != our_object)
			{
				vector.MakeVectorToFrom(P->WorldPosition,camera->CameraLocation);
				DrawPlane(P,vector);
			}
			else
			{
				if (P->TowedDecoyPosition)
				{
					vector.MakeVectorToFrom(P->WorldPosition,camera->CameraLocation);
					DrawTowedDecoy(P,vector);
				}
			}
			P = P->NextVisiblePlane;
		}
		VisiblePlanes = NULL;
	}
	else
	{
		P = &Planes[0];

		while (P <= LastPlane)
		{
			if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED) && (P->Type && P->Type->Model) && (!(P->AI.iAIFlags2 & AIINVISIBLE)))
			{
				P->DoAttitude(P);

				if (P != our_object)
				{
					if (CloseEnoughToDraw(P->WorldPosition))
					{
						vector.MakeVectorToFrom(P->WorldPosition,camera->CameraLocation);
	//					if (RadiusInView(vector,GetObjectRadius(P->Type->Model)))
							DrawPlane(P,vector);

//						if ((P->OnGround != 2) && (vector.PartialLength() < ((5000.0f*5000.0f) FEET)) && (P->HeightAboveGround < (5000.0f*FOOT_MULTER)) && (P->Type->Shadow))
//							DrawPlaneShadow(P);
					}
				}
				else
				{
//					if ((P->HeightAboveGround < (5000.0f*FOOT_MULTER)) && (P->Type->Shadow))
//						DrawPlaneShadow(P);
					vector.MakeVectorToFrom(P->WorldPosition,camera->CameraLocation);
					if (P->TowedDecoyPosition)
						DrawTowedDecoy(P,vector);

				}
			}

			P++;
		}
	}

}

void Draw3DWorld(CameraInstance *camera)
{
	void *our_object;
	//BOOL do_lighting;
	float pilot_g;

 	CalculateCamera(camera);	  // sets view matrix

	Set3DDistanceModifier(64.011f,(float)ScreenSize.cx);

	glob_camera = camera;

	PrimeWorld(camera->CameraLocation);

	if (camera->CameraMode == CAMERA_COCKPIT)
	{
		our_object = (void *)camera->AttachedObject;
		pilot_g = ((PlaneParams *)our_object)->PilotGLoad;
	}
	else
	{
		our_object = NULL;
		pilot_g = 0.0f;
	}


	//InsertCanisters( camera);

//	if (WorldParams.SunY < 0.05f)
//	{
//		FPointDouble SunPos( WorldParams.SunX, WorldParams.SunY, 0.0f);
//		SunPos *= -20000 FEET;
//		SunPos += Camera1.CameraLocation;
//		if( !LOS( &Camera1.CameraLocation, &SunPos, NULL, MED_RES_LOS, FALSE) || (!SunIntensity ) )
//			Camera1.Flags &= ~CF_SUN_VISIBLE;
//		else
//			Camera1.Flags |= CF_SUN_VISIBLE;
//	}
//	else
//	 	Camera1.Flags &= ~CF_SUN_VISIBLE;

	SetTimeOfDay(&WorldParams,camera->CameraLocation.Y);

//	StartRenderingFunctions();

	if(  _3dxl==_3DXL_GLIDE )
		SetClipWin3Dfx();

	SimpleLandDrawID++;
	if ((TerrainYesNos & YN_DRAW_SHADOWS) && !(WorldParams.Weather & WR_FLATCLOUD_OVERCAST) && !(ImagingMethod & IT_FLIR))
	{
		InitShadows(camera->CameraLocation);
		SetupVehicleShadows(camera->CameraLocation);
		SetupPlaneShadows(camera->CameraLocation);
	}
	else
		LastShadow = Shadows;

	DisplayCraters(camera);
	SimpleLandDrawID--;

	SunOnScreen = FALSE;
	MoonOnScreen = FALSE;

	DrawLand(camera->CameraLocation);
	DisplayShockWaves(camera);
	DisplayVehicles(camera);
    //DisplayWayPoints(camera);
	//DisplayLandingGates(camera);
	DisplayPlanes(camera,our_object);
	DisplayWeapons(camera);
	DisplayAAAStreams(camera);
	DisplaySmokeTrails(camera);
	DisplaySprites(camera);
	DisplayCanisters( camera );



//	RenderWorld(camera->CameraLocation);
	if(  _3dxl==_3DXL_GLIDE)
		UnSetClipWin3Dfx();

//	StopRenderingFunctions();
	SetD3DStates(0);
	DisplayClouds( camera);

	if (RedrawRefuelHose)
		RedrawLastRefuelHose();

	if (((WorldParams.Weather & WR_CLOUD1) != WR_CLOUD1_OVERCAST) || (RelativeCloudHeight <= 0.0))
		RenderSunLensFlare();

	if( g_Settings.gr.dwGraph & GP_GRAPH_SUNGLARE )
		DrawSunGlare();

	RemoveViewpointLights();

}

void FixBridges()
{
//	if (fix_bridges)
//	{
//		int count;
//		StructureInstance *bwalker;
//		BasicInstance ***lwalker;

//		/* ------------------------------------------1/22/98 10:39AM-------------------------------------------
//	 	* sounds stupid, but we have to go through the bridges and make sure they don't think they have any
//	 	* vehicles on them.
//	 	* ----------------------------------------------------------------------------------------------------*/

//		count = 2;
//		lwalker = &AllTargetTypes[1];
//		while(count--)
//		{
//			bwalker =  (StructureInstance *)**lwalker++;

//			while(bwalker)
//			{
//				bwalker->SubObject1 = NULL;
//				bwalker->SubObject2 = NULL;
//				bwalker = (StructureInstance *)bwalker->Basics.NextRelatedInstance;
//			}
//		}
//	}

}

/* ------------------------------------------12/23/97 5:06PM-------------------------------------------
 * This draws a limited view image for use with the FLIR and missile cameras
 * ----------------------------------------------------------------------------------------------------*/


void GetMatrixForMissileCam(ANGLE r,ANGLE p,ANGLE h,FMatrix &platform_mat,FMatrix &result)
{
	result.SetRPH(r,p,h);
	result *= platform_mat;
}

void GrDrawZRectPoly( GrBuff *dest, float x, float y,float w,  float h);
void GrDrawZRectPoly( GrBuff *dest);

void DrawLimitedViewMatrixAndPlane(GrBuff *buffer,FPointDouble &eye,FMatrix &matrix,ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot,BOOL high_detail_only,BOOL draw_textures,int dot,void *weap,void *plane)
{
	BYTE old_draw_amount;
	BYTE old_texture_amount;
	GrBuff *old_3d_buff;
	FMatrix tempmatrix;
	float angle;
	int w,h;
	_3dxlType old_render;
	DWORD old_terrain_values;
	CameraInstance cam;
	float old_vis;
	PlaneParams *our_plane;

	angle = ANGLE_TO_RADIANS(aperture);

	if ((angle < PI*0.75f) && (angle > PI/750.0f))
	{

		old_vis = Visibility;

		Visibility *= 0.75f;

		old_terrain_values = TerrainYesNos;
		TerrainYesNos  &= ~(YN_DRAW_WATER_EFFECTS | YN_DRAW_DETAIL_MAPS | YN_DRAW_LIGHT_MAPS | YN_DRAW_POINT_LIGHTS);

		SpecialDraw = TRUE;
		old_render = _3dxl;

		SkipThisDot = dot;
		SkipThisWeap = (WeaponParams *)weap;

		if (weap)
			our_plane = NULL;
		else
			our_plane = PlayerPlane;


		//DoingFlir = draw_as_flir;
		if (draw_as_flir)
		{
			ImagingMethod = IT_GREEN_FLIR;// Is this the right flir mode?
			if (!draw_green_hot)
				ImagingMethod = IT_BLACK_HOT_GREEN_FLIR;
		}
		else
			ImagingMethod = IT_GREEN;// Is this the right flir mode?

		if ((!plane) || (ImagingMethod == IT_BLACK_HOT_GREEN_FLIR))
			TerrainYesNos |= YN_NO_TRANSITIONS;

		old_draw_amount = AmountToDraw;

		if (high_detail_only)
			AmountToDraw = DETAIL_HIGH_ONLY;
		else
			AmountToDraw = DETAIL_MISSILE_MODE;

		old_texture_amount = AmountToTexture;
//		AmountToTexture = draw_textures;

		old_3d_buff = GrBuffFor3D;

		tempmatrix = ViewMatrix;

		matrix.SetViewMatrix();

		GrBuffFor3D = buffer;

		w = buffer->ClipRight - buffer->ClipLeft+1;
		h = buffer->ClipBottom - buffer->ClipTop+1;

		Set3DDistanceModifier((float)RADIANS_TO_DEGREES * angle * 2.0f,(float)w);

		if (w > h)
			EyeToScreen = (float)-w;
		else
			EyeToScreen = (float)-h;

		EyeToScreen *= 0.5f;

		angle = (float)tan(angle);

		OneOverEyeToScreen = angle/EyeToScreen;
		EyeToScreen = EyeToScreen/angle;

//		FixMaxDrawDistances(EyeToScreen / -512.0f);

		Set3DScreenClip();

   	 	lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_ALWAYS);
		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   TRUE);

//		D3DStates |= PERM_NO_ZBUFFER;
//	 	GrDrawZRectPoly(GrBuffFor3D);

		// reset the z states
		D3DStates |= PRIM_ZFUNC_LE | PRIM_NO_Z_WRITE;
//		D3DStates &= ~PERM_NO_ZBUFFER;

		SetD3DStates(0);

		D3DStates |= PERM_NO_ZBUFFER;

		cam.CameraLocation = eye;

		glob_camera = &cam;

		PrimeWorld(eye);

//		fix_bridges = PlaceVehiclesOnBridges(&cam);
		DisplayCraters(&cam);
		//if (buffer == BackSurface)
		//{
		//	InsertCanisters(&cam);
		//}
		SunOnScreen = FALSE;
		MoonOnScreen = FALSE;

		DrawLand( eye);
		DisplayVehicles(&cam);
		DisplayPlanes(&cam,our_plane);
 		DisplayWeapons(&cam);
	   	DisplayAAAStreams(&cam);
		DisplaySmokeTrails(&cam);
		DisplaySprites(&cam);
		DisplayCanisters( &cam);

		// draw our plane in front of everybody else

		if ((!plane) && our_plane)
		{

			// first clear the z-buffer
   	 		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_ALWAYS);
			lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   TRUE);

			D3DStates |= PERM_NO_ZBUFFER;
	 		GrDrawZRectPoly(GrBuffFor3D);

			// reset the z states
			D3DStates |= PRIM_ZFUNC_LE | PRIM_NO_Z_WRITE;
			D3DStates &= ~PERM_NO_ZBUFFER;

			SetD3DStates(0);

			D3DStates |= PERM_NO_ZBUFFER;

			// move the z_clipping plane in really far

			float old_ZClipPlane = ZClipPlane;

			ZClipPlane = -0.01f;
			Set3DScreenClip();
			StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);

			//Now draw our plane

 			FPoint vector;
			vector.MakeVectorToFrom(our_plane->WorldPosition,eye);
			DrawPlane(our_plane,vector);

			// now set everything back again

			ZClipPlane = old_ZClipPlane;
			Set3DScreenClip();
			StupidQ = D3DMaxZ/(D3DMaxZ - ZClipPlane);
		}


		if( g_Settings.gr.dwGraph & GP_GRAPH_SUNGLARE )
			DrawSunGlare();

		// set up to always write to buffer

		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_ALWAYS);
		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   TRUE);

		D3DStates |= PERM_NO_ZBUFFER;
	 	GrDrawZRectPoly(GrBuffFor3D);

		// reset the z states
		D3DStates |= PRIM_ZFUNC_LE | PRIM_NO_Z_WRITE;
		D3DStates &= ~PERM_NO_ZBUFFER;

		SetD3DStates(0);

		D3DStates |= PERM_NO_ZBUFFER;
		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 	   FALSE);
		TerrainYesNos  = old_terrain_values;

		Visibility = old_vis;

		AmountToTexture = old_texture_amount;
		AmountToDraw = old_draw_amount;
		//DoingFlir = FALSE;
		ImagingMethod = IT_NORMAL;

		Set3DGrBuff(old_3d_buff);

		ViewMatrix = tempmatrix;

		SpecialDraw = FALSE;
		 _3dxlChangeStatus(old_render);
	}

	SkipThisDot = 0;
	SkipThisWeap = NULL;

}

void DrawLimitedViewMatrix(GrBuff *buffer,FPointDouble &eye,FMatrix &matrix,ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot,BOOL high_detail_only,BOOL draw_textures,int dot,void *weap)
{
	DrawLimitedViewMatrixAndPlane(buffer,eye,matrix,aperture,process_to_green,draw_as_flir,draw_green_hot,high_detail_only,draw_textures,dot,weap,NULL);
}

void DrawLimitedView(GrBuff *buffer,FPointDouble &eye,float heading, float pitch, ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot,BOOL high_detail_only,BOOL draw_textures,int dot,void *weap)
{
	buffer->MidX = (buffer->ClipLeft + buffer->ClipRight) >> 1;
	buffer->MidY = (buffer->ClipTop + buffer->ClipBottom) >> 1;

	DrawLimitedViewWithRoll(buffer,eye,heading,pitch,0.0f,aperture,process_to_green,draw_as_flir,draw_green_hot,high_detail_only,draw_textures,dot,weap);
}


void DrawLimitedViewWithRoll(GrBuff *buffer,FPointDouble &eye,float heading, float pitch, float roll,ANGLE aperture, BOOL process_to_green, BOOL draw_as_flir, BOOL draw_green_hot,BOOL high_detail_only,BOOL draw_textures,int dot,void *weap)
{
	FMatrix mat;

	mat.SetRadRPH(DEGREES_TO_RADIANS * roll,DEGREES_TO_RADIANS * pitch,DEGREES_TO_RADIANS * heading);

	DrawLimitedViewMatrix(buffer,eye,mat,aperture,process_to_green,draw_as_flir,draw_green_hot,high_detail_only,draw_textures,dot,weap);
}

BOOL InittedSunGlarePoints;

RotPoint3D SunGlarePoints[9];

RotPoint3D *GlarePointptrs[] = {&SunGlarePoints[0],&SunGlarePoints[1],&SunGlarePoints[4],&SunGlarePoints[3],
								&SunGlarePoints[2],&SunGlarePoints[5],&SunGlarePoints[4],&SunGlarePoints[1],
								&SunGlarePoints[6],&SunGlarePoints[3],&SunGlarePoints[4],&SunGlarePoints[7],
								&SunGlarePoints[8],&SunGlarePoints[7],&SunGlarePoints[4],&SunGlarePoints[5]};


extern int SunSX, SunSY,MoonSX,MoonSY;
void DrawSunGlare( BOOL sun)
{
	RotPoint3D *pwalk;
	float intensity;
	float body_intensity;
	int bodysx,bodysy;
	FPoint *bvec;

	if (sun)
	{
		if (!SunOnScreen || !SunIntensity)
			return;

		body_intensity = SunIntensity;
		bodysx = SunSX;
		bodysy = SunSY;
		bvec = &PositionToDrawSun;
	}
	else
	{
		if (!MoonOnScreen || !MoonIntensity)
			return;

		body_intensity = MoonIntensity;
		bodysx = MoonSX;
		bodysy = MoonSY;
		bvec = &PositionToDrawMoon;
	}

	intensity = body_intensity * (ViewVector * *bvec);
	if (intensity > 0.95f)
	{
		intensity = (intensity - 0.95f) * 10.0f;
		if (ImagingMethod & IT_FLIR)
			intensity *= 20.0f;

		if (intensity > 1.0f)
			intensity = 1.0f;

		if (!sun && (intensity < 0.5f))
			return;

		if (!InittedSunGlarePoints)
		{
			pwalk = &SunGlarePoints[9];
			while(pwalk-- != SunGlarePoints)
			{
				pwalk->Specular = 0.0f;
				pwalk->fog = 1.0f;
				pwalk->oow  = 1.0f;
				pwalk->ooz  = 0.0f;
			}

			InittedSunGlarePoints = TRUE;
		}

		pwalk = &SunGlarePoints[9];
		while(pwalk-- != SunGlarePoints)
		{
			pwalk->Flags = PT3_ROTATED | PT3_PERSPECTED | PT3_OOW_SET;
			pwalk->Diffuse = 0.0f;
			pwalk->Alpha = 1.0f;
		}

		SunGlarePoints[0].fSX = SunGlarePoints[3].fSX = SunGlarePoints[6].fSX = (float)GrBuffFor3D->ClipLeft;
		SunGlarePoints[1].fSX = SunGlarePoints[4].fSX = SunGlarePoints[7].fSX = (float)bodysx;
		SunGlarePoints[2].fSX = SunGlarePoints[5].fSX = SunGlarePoints[8].fSX = (float)GrBuffFor3D->ClipRight;

		SunGlarePoints[0].fSY = SunGlarePoints[1].fSY = SunGlarePoints[2].fSY = (float)GrBuffFor3D->ClipTop;
		SunGlarePoints[3].fSY = SunGlarePoints[4].fSY = SunGlarePoints[5].fSY = (float)bodysy;
		SunGlarePoints[6].fSY = SunGlarePoints[7].fSY = SunGlarePoints[8].fSY = (float)GrBuffFor3D->ClipBottom;

		if (sun)
			SunGlarePoints[4].Diffuse.SetValues(intensity,SunLight.Color);
		else
			SunGlarePoints[4].Diffuse = (intensity-0.5f)  * 0.2f;


		DoD3DGop(PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_NO_Z_WRITE,4,GlarePointptrs);
		DoD3DGop(PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_NO_Z_WRITE,4,&GlarePointptrs[4]);
		DoD3DGop(PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_NO_Z_WRITE,4,&GlarePointptrs[8]);
		DoD3DGop(PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_NO_Z_WRITE,4,&GlarePointptrs[12]);
	}

}

void DrawGEffects(float pilot_g)
{
	RotPoint3D *pwalk;
	float outer_alpha;
	float inner_alpha;
	FPoint diffuse;

	if ((PlayerPlane->Status & PL_ACTIVE) && !(PlayerPlane->FlightStatus & (PL_STATUS_CRASHED | PL_PLANE_BLOWN_UP)))
		pilot_g += (PlayerPlane->TicksWithoutOxygen / 30.0f);

	if (!pilot_g)
		return;

	float abspilot_g = fabs(pilot_g);


	if (abspilot_g > 2.0f)
	{
		if (pilot_g > 0.0)
			diffuse = 0.0f;
		else
			diffuse.SetValues(NaturalLight.PartialLength()/3.0f,0.0f,0.0f);

		inner_alpha = (abspilot_g - 5.0f)/3.0f;
		if (inner_alpha > 1.0f)
			inner_alpha = 1.0f;

		outer_alpha = inner_alpha;
		inner_alpha *= inner_alpha*inner_alpha*inner_alpha;//* 2.0f;
		if (outer_alpha > 1.0f)
			outer_alpha = 1.0f;

		if (outer_alpha > 0.0f)
		{
			if (!InittedSunGlarePoints)
			{
				pwalk = &SunGlarePoints[9];
				while(pwalk-- != SunGlarePoints)
				{
					pwalk->Specular = 0.0f;
					pwalk->fog = 1.0f;
					pwalk->oow  = 1.0f;
					pwalk->ooz  = 0.0f;
				}

				InittedSunGlarePoints = TRUE;
			}

			pwalk = &SunGlarePoints[9];
			while(pwalk-- != SunGlarePoints)
			{
				pwalk->Flags = PT3_ROTATED | PT3_PERSPECTED | PT3_OOW_SET;
				pwalk->Diffuse = diffuse;
				pwalk->Alpha = outer_alpha;
			}

			SunGlarePoints[0].fSX = SunGlarePoints[3].fSX = SunGlarePoints[6].fSX = (float)GrBuffFor3D->ClipLeft;
			SunGlarePoints[1].fSX = SunGlarePoints[4].fSX = SunGlarePoints[7].fSX = (float)(GrBuffFor3D->ClipLeft + GrBuffFor3D->ClipRight+1)*0.5f;
			SunGlarePoints[2].fSX = SunGlarePoints[5].fSX = SunGlarePoints[8].fSX = (float)GrBuffFor3D->ClipRight+1;

			SunGlarePoints[0].fSY = SunGlarePoints[1].fSY = SunGlarePoints[2].fSY = (float)GrBuffFor3D->ClipTop;
			SunGlarePoints[3].fSY = SunGlarePoints[4].fSY = SunGlarePoints[5].fSY = (float)(GrBuffFor3D->ClipTop + GrBuffFor3D->ClipBottom+1)*0.5f;
			SunGlarePoints[6].fSY = SunGlarePoints[7].fSY = SunGlarePoints[8].fSY = (float)GrBuffFor3D->ClipBottom+1;

			SunGlarePoints[4].Alpha = inner_alpha;

    		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 		   FALSE);
			DoD3DGop(PRIM_ALPHA | PRIM_ALPHAIMM | PRIM_NO_Z_WRITE,4,GlarePointptrs);
			DoD3DGop(PRIM_ALPHA | PRIM_ALPHAIMM | PRIM_NO_Z_WRITE,4,&GlarePointptrs[4]);
			DoD3DGop(PRIM_ALPHA | PRIM_ALPHAIMM | PRIM_NO_Z_WRITE,4,&GlarePointptrs[8]);
			DoD3DGop(PRIM_ALPHA | PRIM_ALPHAIMM | PRIM_NO_Z_WRITE,4,&GlarePointptrs[12]);
    		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 		   TRUE);
		}
	}

}