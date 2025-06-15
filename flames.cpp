
#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"

SmokeTrail SmokeTrails[MAX_SMOKE_TRAILS],*LastSmokeTrail;
SmokeType SmokeTypes[MAX_SMOKE_TYPES];

SpriteType SpriteTypes[MAX_SPRITE_TYPES];
Generator Generators[MAX_GENERATORS],*LastGenerator;
Sprite Sprites[MAX_SPRITES],*LastSprite;

extern float StupidQ;
extern BOOL NoFogging;
extern float D3DMinFog;
extern float D3DMaxFog;
extern float D3DFogMulter;

int PerspectPointWithAllFlags(RotPoint3D *point);


/* -----------------8/7/99 1:33PM----------------------------------------------------------------------
/* new code for smoke
/* ----------------------------------------------------------------------------------------------------*/

void SmokeType::Init(TextureBuff *texture,DWORD flags,FPoint &color,float alpha,float stretch,float radius,int time_to_fade,float dormant,float alpha_in,float min_wait,float rand_wait,float alpha_out,short min_ticks)
{
	float add_to_next;
	float fvalue;
	float tsize;
	int min_rate;

	tsize = alpha_in+min_wait+rand_wait+alpha_out+dormant;
	if (tsize)
	{
		tsize = 1.0f/tsize;

		dormant *= tsize;
		alpha_in *= tsize;
		min_wait *= tsize;
		rand_wait *= tsize;
		alpha_out *= tsize;
	}
	else
		return;

	Texture = texture;
	VMulter = texture->Width/texture->Height;
	VMulter *= 1.0f/(radius*(2.0f*stretch));
	Alpha = alpha;
	Flags = flags;


	Color = color;
	Radius = radius;
	LoopsToFade = time_to_fade;
	DropRate = (LoopsToFade + (MAX_POINTS_PER_TRAIL-2)) / (MAX_POINTS_PER_TRAIL-1);

	if (DropRate < min_ticks)
		DropRate = min_ticks;

	time_to_fade -= DropRate;

	min_rate = DropRate;

	add_to_next = 0.0f;

	if (dormant == 0.0)
	{
		LoopsDormant = 0;
	}
	else
	{
		fvalue = dormant * (float)time_to_fade;
		LoopsDormant = (int)(fvalue+0.5f);
		LoopsDormant -= LoopsDormant % DropRate;

		add_to_next += fvalue - (float)LoopsDormant;
	}


	fvalue = alpha_in * (float)time_to_fade;
	LoopsIn = (int)(fvalue+0.5f);
	if (LoopsIn < min_rate)
		LoopsIn = min_rate;

	ooLoopsIn = 1.0f/(float)LoopsIn;

	add_to_next += fvalue - (float)LoopsIn;

	fvalue = alpha_out * (float)time_to_fade;
	LoopsOut = (int)(fvalue+0.5f);
	if (LoopsOut < min_rate)
		LoopsOut = min_rate;

	ooLoopsOut = 1.0f/(float)LoopsOut;

	add_to_next += fvalue - (float)LoopsOut;

	if (rand_wait == 0.0)
		LoopsRand = 0.0;
	else
	{
		fvalue = rand_wait * (float)time_to_fade;
		LoopsRand = (int)(fvalue+0.5f);

		add_to_next += fvalue - (float)LoopsRand;
	}

	fvalue = min_wait * (float)time_to_fade + add_to_next;
	LoopsWait = (int)(fvalue+0.5f);

	if (LoopsWait > (time_to_fade - (LoopsIn+LoopsOut+LoopsRand + LoopsDormant)))
	{
		LoopsWait = (time_to_fade - (LoopsIn+LoopsOut+LoopsRand + LoopsDormant));
		if (LoopsWait <= 0)
		{
			LoopsToFade -= LoopsWait-1;
			LoopsWait = 1;
		}
	}
}


void *SmokeTrail::NewSmokeTrail(void *object,int offset,int smoke_type,BOOL has_light,FPoint *light_color,float radius1, float radius2)
{
	SmokeTrail *walker;
	DWORD type;

	if (1)//g_Settings.gr.nWeaponSmoke)
	{
		walker = &SmokeTrails[0];

		while((walker <= LastSmokeTrail) && (walker->Flags & ST_ACTIVE))
			walker++;

		if (walker == &SmokeTrails[MAX_SMOKE_TRAILS])
			return NULL;

		if (walker > LastSmokeTrail)
			LastSmokeTrail = walker;

		walker->Flags = ST_ACTIVE | offset;
		walker->LastPoint = &walker->Points[-1];
		walker->Type = &SmokeTypes[smoke_type];
		walker->Ticks = walker->Type->DropRate;

		if ((object >= (void *)&Planes[0]) && (object < (void *)&Planes[MAX_PLANES]))
			type = PLANE_OBJECT;
		else
			if ((object >= (void *)&Weapons[0]) && (object < (void *)&Weapons[MAX_WEAPON_SLOTS]))
				type = MISSILE_OBJECT;
			else
				type = 0;

		if (type == PLANE_OBJECT)
		{
			walker->SmokingObject.SmokingPlane = (PlaneParams *)object;
			walker->Points[0].Location = walker->SmokingObject.SmokingPlane->WorldPosition;
			walker->Flags |= ST_PLANE;
		}
		else
			if ((type == MISSILE_OBJECT) || (type == BOMB_OBJECT))
			{
				walker->SmokingObject.SmokingWeapon = (WeaponParams *)object;
				walker->Points[0].Location = walker->SmokingObject.SmokingWeapon->Pos;
				walker->Flags |= ST_MISSILE;
			}
			else
			{
				walker->SmokingObject.SmokingVoid = object;
				walker->Flags |= ST_VOID;
			}

		if (has_light)
		{
			if (walker->Light = (PointLightSource **)GetNewLight())
			{
				PointLightSource *new_light = new PointLightSource;
				new_light->WorldPosition = walker->Points[0].Location;
		//		new_light->Color = WhiteishFireColor;
				new_light->Color = *light_color;
				new_light->Flags |= LIGHT_LARGE_SPOT;
				new_light->Radius1 = radius1*0.75;
				new_light->ooRadius1 = 1.0f/radius1;
				new_light->Radius2 = radius2*0.75;
				new_light->ooRadius2 = 1.0f/radius2;
				*(walker->Light) = new_light;
			}
		}
	}
	else
		walker = NULL;

	return (void *)walker;

}

void SmokeTrail::RemoveSmokeTrail()
{
	if (this)
	{

		Flags = 0;

		if (Light)
		{
			RemoveLight((LightSource **)Light);
			Light = NULL;
		}

		if (this == LastSmokeTrail)
		{
			while((LastSmokeTrail > &SmokeTrails[-1]) && (!LastSmokeTrail->Flags))
				LastSmokeTrail--;
		}
	}

}

void SmokeTrail::UpdateOrphan( void )
{
	if (this && (Flags & ST_ACTIVE))
	{
		BOOL done;
		SmokePoint *pwalk;
		WORD ltfm1 = Type->LoopsToFade-GameLoopsPerFrame;

		done = FALSE;

		/* -----------------8/7/99 2:17PM----------------------------------------------------------------------
		/* get rid of anything which is already dead
		/* ----------------------------------------------------------------------------------------------------*/

		while((LastPoint >= Points) && (LastPoint->TotalTicks >= ltfm1))
			LastPoint--;

		if (LastPoint < Points)
		{
		 	RemoveSmokeTrail();
		 	return;
		}
		else
			LastPoint->Alpha = 0.0f;

		/* -----------------8/7/99 2:18PM----------------------------------------------------------------------
		/* and move the rest to the next state if necessary
		/* ----------------------------------------------------------------------------------------------------*/

		pwalk = LastPoint;

		while(pwalk >= Points)
		{
			pwalk->TotalTicks+=GameLoopsPerFrame;
			pwalk->StateTicks += GameLoopsPerFrame;
			while(pwalk->StateTicks >= pwalk->Data)
			{
				switch (pwalk->Flags & SP_STATE)
				{
					case SP_STATE_0:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_1;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = Type->LoopsIn;
						break;

					case SP_STATE_1:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_2;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = Type->LoopsWait;
						if (Type->LoopsRand)
						{
							float r = frand() + frand() + frand() + frand();
							r *= 0.25f * (float)Type->LoopsRand;
							pwalk->Data += (int)r;
						}
						break;

					case SP_STATE_2:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_3;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = Type->LoopsOut;
						break;

					case SP_STATE_3:
					case SP_STATE_4:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_4;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = 0x7FFF;
						pwalk->Alpha = 0.0f;
						break;
				}
			}
			pwalk--;
		}
	}
}


void SmokeTrail::Update(float alpha)
{
	if (this && (Flags & ST_ACTIVE))
	{
		BOOL done;
		SmokePoint *pwalk;
		WORD ltfm1 = Type->LoopsToFade-1;

		done = FALSE;

		/* -----------------8/7/99 2:17PM----------------------------------------------------------------------
		/* get rid of anything which is already dead
		/* ----------------------------------------------------------------------------------------------------*/

		while((LastPoint >= Points) && (LastPoint->TotalTicks >= ltfm1))
			LastPoint--;

		if (LastPoint >= Points)
			LastPoint->Alpha = 0.0f;

		/* -----------------8/7/99 2:18PM----------------------------------------------------------------------
		/* and move the rest to the next state if necessary
		/* ----------------------------------------------------------------------------------------------------*/

		pwalk = LastPoint;

		while(pwalk > Points)
		{
			pwalk->TotalTicks++;
			pwalk->StateTicks++;
			while(pwalk->StateTicks >= pwalk->Data)
			{
				switch (pwalk->Flags & SP_STATE)
				{
					case SP_STATE_0:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_1;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = Type->LoopsIn;
						break;

					case SP_STATE_1:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_2;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = Type->LoopsWait;
						if (Type->LoopsRand)
						{
							float r = frand() + frand() + frand() + frand();
							r *= 0.25f * (float)Type->LoopsRand;
							pwalk->Data += (int)r;
						}
						break;

					case SP_STATE_2:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_3;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = Type->LoopsOut;
						break;

					case SP_STATE_3:
					case SP_STATE_4:
						pwalk->Flags &= ~SP_STATE;
						pwalk->Flags |= SP_STATE_4;
						pwalk->StateTicks -= pwalk->Data;
						pwalk->Data = 0x7FFF;
						pwalk->Alpha = 0.0f;
						break;
				}
			}
			pwalk--;
		}

		/* -----------------8/7/99 2:25PM----------------------------------------------------------------------
		/* now figure out if we need to drop a new point
		/* ----------------------------------------------------------------------------------------------------*/

		if (++Ticks >= Type->DropRate)
		{
			Ticks -= Type->DropRate;
			if (LastPoint != &Points[MAX_POINTS_PER_TRAIL-1])
			{
				LastPoint++;

				if (Type->LoopsDormant)
				{
					Points->Flags = 0;
					Points->Data = Type->LoopsDormant;
				}
				else
				{
					Points->Flags = 1;
					Points->Data = Type->LoopsIn;
				}


				Points->TotalTicks = Ticks;
				Points->StateTicks = Ticks;
				Points->Alpha = alpha;

				if (LastPoint != Points)
					memmove(&Points[1],Points,(int)LastPoint-(int)&Points[0]);

				Points->Flags = 1;

			}
		}


		/* -----------------8/7/99 3:17PM----------------------------------------------------------------------
		/* move the first point to the attached object's location
		/* ----------------------------------------------------------------------------------------------------*/


		if (Flags & ST_PLANE)
		{
			PlaneParams *P = SmokingObject.SmokingPlane;
			FPoint *offset;
			offset = &P->Type->Offsets[Flags&ST_OFFSET];
			Points->Location = P->WorldPosition;
			Points->Location.AddScaledVector(offset->X,P->Orientation.J);
			Points->Location.AddScaledVector(offset->Y,P->Orientation.K);
			Points->Location.AddScaledVector(offset->Z,P->Orientation.I);
		}
		else
			if (Flags & ST_MISSILE)
			{


				WeaponParams *weapon = SmokingObject.SmokingWeapon;
				Points->Location = weapon->Vel;
				Points->Location.Normalize();

				Points->Location *= GetObjectRadius(weapon->Type->Model) * -0.65;  //Just slightly less than 2^-0.5 (0.707) should put us near the edge
				Points->Location += weapon->Pos;
			}

		if (LastPoint != Points)
			Points->V = Points[1].V + (Points[1].Location / Points[0].Location) * Type->VMulter;
		else
			Points->V = 0;

		/* -----------------8/7/99 3:17PM----------------------------------------------------------------------
		/* Make Sure the end fades off
		/* ----------------------------------------------------------------------------------------------------*/

		if (Light)
			(*Light)->WorldPosition = Points[0].Location;
	}
}

void SmokeTrail::MakeOrphan()
{
	if (this)
	{
		if (Light)
		{
			RemoveLight((LightSource **)Light);
			Light = NULL;
		}

		Points->Alpha = 0.0f;
		Flags |= ST_ORPHANED;
	}
}

BOOL DidSmokeTrailSetup;
RotPoint3D SmokeTrailDrawingPoints[6];
RotPoint3D *lpSmokeTrailPoints1[6] = {&SmokeTrailDrawingPoints[0],&SmokeTrailDrawingPoints[1],&SmokeTrailDrawingPoints[3],&SmokeTrailDrawingPoints[2],&SmokeTrailDrawingPoints[4],&SmokeTrailDrawingPoints[5]};
RotPoint3D *lpSmokeTrailPoints2[6] = {&SmokeTrailDrawingPoints[2],&SmokeTrailDrawingPoints[3],&SmokeTrailDrawingPoints[1],&SmokeTrailDrawingPoints[0],&SmokeTrailDrawingPoints[5],&SmokeTrailDrawingPoints[4]};
RotPoint3D **lpSmokeTrailDrawingPoints[2] = {lpSmokeTrailPoints1,lpSmokeTrailPoints2};

void SmokeTrail::Draw(FPointDouble Eye)
{
	SmokePoint *pfrom,*pto;
	RotPoint3D *p1,*p2;
	RotPoint3D *p3,*p4;
	RotPoint3D *lp,*lp2;
	RotPoint3D *pwalk,**ppwalk;
	RotPoint3D **our_ptrs;
	LightSource **light_walker;
	FPoint specular;
	int num_points;
	int start_point;
	DWORD from_wants_line,to_wants_line;
	FPoint toright;
	FPoint delta;
	float float_worker;
	float fw;
	float alpha_comp;
	int i;
	int draw_flags;
	int clip_flags;
	int poly_flags;

	if (Type->Flags & ST_ADD)
		poly_flags = PRIM_TEXTURE | PRIM_ALPHA | PRIM_ALPHA_ADD | PRIM_NO_Z_WRITE | PRIM_MIPMAP | PRIM_ZFUNC_LE;
	else
		poly_flags = PRIM_TEXTURE | PRIM_ALPHA | PRIM_NO_Z_WRITE | PRIM_MIPMAP | PRIM_ZFUNC_LE;


	specular.SetValues(0.0f);

	if (!DidSmokeTrailSetup)
	{
		i = 6;
		pwalk = SmokeTrailDrawingPoints;

		while(i--)
		{
			pwalk->Specular = 0.0f;
			pwalk++->fog = 1.0f;
		}

		SmokeTrailDrawingPoints[0].U = 0.0f;
		SmokeTrailDrawingPoints[1].U = 1.0f;
		SmokeTrailDrawingPoints[2].U = 0.0f;
		SmokeTrailDrawingPoints[3].U = 1.0f;
		SmokeTrailDrawingPoints[4].U = 0.5f;
		SmokeTrailDrawingPoints[5].U = 0.5f;

		DidSmokeTrailSetup = TRUE;
	}


	pto = LastPoint+1;
	from_wants_line = FALSE;

	while(pto > Points)
	{
		pfrom = pto--;

		our_ptrs = lpSmokeTrailDrawingPoints[0];
		Swap4(lpSmokeTrailDrawingPoints,&lpSmokeTrailDrawingPoints[1]);

		lp = our_ptrs[4];
		lp2 = our_ptrs[5];

		p2 = our_ptrs[0];
		p3 = our_ptrs[2];

		p1 = p2++;
		p4 = p3--;


		lp->Rotated.MakeVectorToFrom(pto->Location,Eye);
		lp->Rotated *= ViewMatrix;

		if (lp->Rotated.Z && ( 2*Type->Radius*EyeToScreen/lp->Rotated.Z >= 1.2f))
			to_wants_line = FALSE;
		else
			to_wants_line = TRUE;


		/* -----------------8/7/99 5:29PM----------------------------------------------------------------------
		/* now, we'll perspect this point and figure out how alpha'd it should be
		/* ----------------------------------------------------------------------------------------------------*/

		if (lp->Rotated.Z > ZClipPlane)
			lp->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			lp->Flags = PT3_ROTATED;

			PerspectPointWithAllFlags(lp);

			lp->Flags |= PT3_OOW_SET;

			lp->oow = -1.0f/lp->Rotated.Z;

			lp->ooz = StupidQ * (1.0f- -ZClipPlane * lp->oow);

			if( lp->ooz > 1.0f)
				lp->ooz = 1.0f;
			else
				if (lp->ooz < 0.0f)
					lp->ooz = 0.0f;
		}

		if ((pto != Points) || (Type->Flags & ST_NO_FADEIN))
		{
			float_worker = lp->Rotated.PartialLength();

			if (float_worker <= D3DMinFog)
				float_worker = 1.0f;
			else
				if (float_worker >= D3DMaxFog)
				{
					float_worker = 0.0f;
					lp->Flags |= PT3_FOGGED_OUT;
				}
				else
				{
					float_worker -=  D3DMinFog;
					float_worker *= D3DFogMulter;
					float_worker = 1.0f - float_worker;
				}

			if (!float_worker)
			{
				lp->Flags |= PT3_FOGGED_OUT;
				fw = 0.0f;
			}
			else
			{
				if (lp->Rotated.Z < 0.0)
				{
					fw = 4*Type->Radius*EyeToScreen/lp->Rotated.Z;
					if (fw > 1.0f)
						fw = 1.0f;
				}
				else
					fw = 1.0f;
			}

			switch (pto->Flags & SP_STATE)
			{
				case SP_STATE_0:
					if (Type->Flags & ST_NO_FADEIN)
						alpha_comp = 1.0f;
					else
						alpha_comp = 0.0f;
					break;

				case SP_STATE_1:
					if (Type->Flags & ST_NO_FADEIN)
						alpha_comp = 1.0f;
					else
						alpha_comp = ((float)(pto->StateTicks) * Type->ooLoopsIn);
					break;

				case SP_STATE_2:
					alpha_comp = 1.0f;
					break;

				case SP_STATE_3:
					alpha_comp = 1.0f - ((float)(pto->StateTicks) * Type->ooLoopsOut);
					break;

				case SP_STATE_4:
					alpha_comp = 0.0f;
					break;

			}

			// not multing in fw here, we'll do it after we copy to the other points
			lp->Alpha = float_worker*pto->Alpha*Type->Alpha*alpha_comp;
		}
		else
			lp->Alpha = 0.0f;

		light_walker = GlobalLights;

		if (Type->Flags & ST_NOLIGHT)
			lp->Diffuse = Type->Color;
		else
		{
			if ((ImagingMethod == IT_FLIR) && (Type->Flags & ST_HOT))
			{
				if (pto == Points)
					lp->Diffuse = 1.0f;
				else
					lp->Diffuse = 1.0f - ((float)pto->TotalTicks/(float)Type->LoopsToFade);
			}
			else
			{
				lp->Diffuse = 0.0f;
				while(light_walker <= LastLight)
				{
					if (*light_walker)
						(*light_walker)->CalculateSaturation(pto->Location,lp->Diffuse,specular);
					light_walker++;
				}

				lp->Diffuse *= Type->Color;
				lp->Diffuse += specular;
				lp->Diffuse.Limit(1.0f);
			}
		}

		/* -----------------8/7/99 5:30PM----------------------------------------------------------------------
		/* now we past in the rest of the stuff
		/* ----------------------------------------------------------------------------------------------------*/

		p1->Rotated = (p2->Rotated = lp->Rotated);
		p1->Light = p2->Light = lp->Light = lp->Alpha;

		if (poly_flags & PRIM_ALPHA_ADD)
		{
			lp->Diffuse *= lp->Alpha;

			p1->Diffuse = p2->Diffuse = lp->Diffuse;
			p1->Alpha = p2->Alpha = lp->Alpha = 1.0f;
		}
		else
		{
			p1->Diffuse = (p2->Diffuse = lp->Diffuse);
			p1->Alpha = (p2->Alpha = lp->Alpha);
		}


		if (pto != LastPoint)
		{
			p3->V = p4->V = (lp2->V = fmod(lp2->V,1.0f));
			p1->V = p2->V = (lp->V = lp2->V + (pto->V - pfrom->V));
		}
		else
			p1->V = p2->V = (lp->V = fmod(pto->V,1.0f));


		if (pto == Points)
		{
			if (Flags & ST_PLANE)
			{
				PlaneParams *P = SmokingObject.SmokingPlane;

				delta = P->Orientation.I;
				delta.SetValues(-P->Orientation.I.X,-P->Orientation.I.Y,-P->Orientation.I.Z);
				delta *= ViewMatrix;
			}
			else
				if (Flags & ST_MISSILE)
				{
					WeaponParams *weapon = SmokingObject.SmokingWeapon;
					delta.SetValues(weapon->Attitude.m_Data.RC.R0C2,weapon->Attitude.m_Data.RC.R1C2,weapon->Attitude.m_Data.RC.R2C2);
					delta *= ViewMatrix;
				}
				// if not these, then use the delta from last time

		}
		else
		{
			if (poly_flags & PRIM_ALPHA_ADD)
				lp->Diffuse*=fw;
			else
				lp->Alpha  *= fw;

			lp->Light  *= fw;

			delta.MakeVectorToFrom(pto->Location,pto[-1].Location);
			delta *= ViewMatrix;
		}

		toright = lp->Rotated;
		toright %= delta;

		toright.Normalize();

		toright *= Type->Radius;

		p1->Rotated -= toright;
		p2->Rotated += toright;

		if (p1->Rotated.Z > ZClipPlane)
			p1->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			p1->Flags = PT3_ROTATED;

			PerspectPointWithAllFlags(p1);

			p1->Flags |= PT3_OOW_SET;

			p1->oow = -1.0f/p1->Rotated.Z;

			p1->ooz = StupidQ * (1.0f- -ZClipPlane * p1->oow);

			if( p1->ooz > 1.0f)
				p1->ooz = 1.0f;
			else
				if (p1->ooz < 0.0f)
					p1->ooz = 0.0f;
		}


		if (p2->Rotated.Z > ZClipPlane)
			p2->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			p2->Flags = PT3_ROTATED;

			PerspectPointWithAllFlags(p2);

			p2->Flags |= PT3_OOW_SET;

			p2->oow = -1.0f/p2->Rotated.Z;

			p2->ooz = StupidQ * (1.0f- -ZClipPlane * p2->oow);

			if( p2->ooz > 1.0f)
				p2->ooz = 1.0f;
			else
				if (p2->ooz < 0.0f)
					p2->ooz = 0.0f;
		}

		if (p1->Light == 0.0f)
		{
			p1->Flags |= PT3_FOGGED_OUT;
			p2->Flags |= PT3_FOGGED_OUT;
		}

		if (lp->Light == 0.0f)
			lp->Flags |= PT3_FOGGED_OUT;

		if (pto != LastPoint)
		{
			if (from_wants_line & to_wants_line)
			{
				start_point=4;
				num_points = 2;
			}
			else
			{
				start_point = 0;
				num_points = 4;
			}

			ppwalk = &our_ptrs[start_point];
			i = num_points;
			draw_flags = -1;
			clip_flags = 0;

			while(i--)
			{
				pwalk = *ppwalk++;
				clip_flags |= pwalk->Flags;
				draw_flags &= pwalk++->Flags;
			}


			if (!(draw_flags & (PT3_NODRAW | PT3_FOGGED_OUT)))
			{
				CurrentTexture = Type->Texture;
				NoFogging = TRUE;
				DrawPolyClipped(poly_flags,CL_CLIP_UV,num_points,&our_ptrs[start_point],clip_flags & PT3_CLIP_FLAGS);
				NoFogging = FALSE;
			}
		}

		from_wants_line = to_wants_line;

	}
}

int SmokeTrail::Identify( void )
{
	return Type - SmokeTypes;
}

int SmokeTrail::operator==( int type )
{
	return (BOOL)(Type == &SmokeTypes[type]);
}


void UpdateOrphanedSmokeTrails(void)
{
	SmokeTrail *walker;

//	if (!g_Settings.gr.nWeaponSmoke)
//		return;

	walker = &SmokeTrails[0];
	while(walker <= LastSmokeTrail)
	{
		if ((walker->Flags & ST_ACTIVE) && (walker->Flags & ST_ORPHANED))
			walker->UpdateOrphan();

		walker++;
	}
}

void DisplaySmokeTrails(CameraInstance *camera)
{
	SmokeTrail *walker;

	walker = SmokeTrails;

	while(walker <= LastSmokeTrail)
	{
		if (walker->Flags & ST_ACTIVE)
			walker->Draw(camera->CameraLocation);

		walker++;
	}

}





/* -----------------8/7/99 1:33PM----------------------------------------------------------------------
/* end of new smoke code
/* ----------------------------------------------------------------------------------------------------*/

//************************************************************************************************************************

void *NewGenerator(int sprite_type,const FPointDouble &start_position,const FPointDouble &velocity,float frequency,int loops_left)
{
	Generator *walker;


	walker = &Generators[0];

	while((walker <= LastGenerator) && (walker->Type))
		walker++;

	if (walker == &Generators[MAX_GENERATORS])
		return NULL;

	if (walker > LastGenerator)
		LastGenerator = walker;

	walker->Type = &SpriteTypes[sprite_type];
	walker->PreviousLoop = GameLoop;
	walker->LoopsLeft = (float)loops_left;
	walker->Vel = velocity;
	walker->Vel *= frequency;
	walker->FreqRem = 0.0f;
	walker->Frequency = frequency;
	walker->Position = start_position;

	if (walker->Light = (PointLightSource **)GetNewLight())
	{
		PointLightSource *new_light = new PointLightSource;
		new_light->WorldPosition = start_position;
//		new_light->Color = WhiteishFireColor;
		new_light->Color.SetValues(1.0,0.75,0.21);
//		new_light->Color.SetValues(1.0f,0.92f,0.88f);
//		new_light->Color *= 1.0f/255.0f;
		new_light->Flags |= LIGHT_LARGE_SPOT;
		new_light->Radius1 = 75.0f;
		new_light->ooRadius1 = 1.0f/75.0f;
		new_light->Radius2 = 15.0f FEET;
		new_light->ooRadius2 = 1.0f/new_light->Radius2;
		*(walker->Light) = new_light;
	}

	return walker;
}

//************************************************************************************************************************
void MoveGenerators(void)
{
	Generator *walker;
	float loops_left;
	float loop_number;

	walker = &Generators[0];
	while(walker <= LastGenerator)
	{
		if (walker->Type)
		{
			loops_left = walker->FreqRem + GameLoop-walker->PreviousLoop;
			if (loops_left > walker->LoopsLeft)
				loops_left = walker->LoopsLeft;
			walker->LoopsLeft -= loops_left;

			loop_number = walker->PreviousLoop + walker->FreqRem;
			walker->PreviousLoop = GameLoop;

			while(loops_left > walker->Frequency)
			{
				NewSprite(walker->Type,walker->Position,(unsigned int)(floor(loop_number)+0.4));
				walker->Position += walker->Vel;
				loops_left -= walker->Frequency;
				loop_number += walker->Frequency;
			}
			walker->FreqRem = loops_left;

			if (walker->LoopsLeft <= 0.0f)
				FreeGenerator(walker);
			else
				if (walker->Light)
					(*(walker->Light))->WorldPosition = walker->Position;

		}
		walker++;
	}
}
//************************************************************************************************************************

void FreeGenerator(Generator *generator)
{
	if (generator)
	{
		generator->Type = 0;
		if (generator->Light)
		{
			RemoveLight((LightSource **)generator->Light);
			generator->Light = NULL;
		}

		if (generator == LastGenerator)
		{
			while((LastGenerator > &Generators[-1]) && (!LastGenerator->Type))
				LastGenerator--;
		}
	}
}

//************************************************************************************************************************

FPoint WhiteishFireColor(1.0f,1.0f,0.8f);
FPoint RedishFireColor(0.0f,-0.8f,-0.8f);

void *NewSprite(SpriteType *type,const FPointDouble &position,unsigned int loop_number,int orientation)
{
	Sprite *walker;

	if (!loop_number)
		loop_number = GameLoop;

	walker = &Sprites[0];

	while((walker <= LastSprite) && (walker->Type))
		walker++;

	if (walker == &Sprites[MAX_SPRITES])
		return NULL;

	if (walker > LastSprite)
		LastSprite = walker;

	walker->Type = type;
	walker->Position = position;
	if (!orientation && (type->Flags & RANDOM_ORIENTATION))
		walker->Orientation = rand()&0xF;
	else
		walker->Orientation = orientation;
	walker->FirstLoopNumber = loop_number;

	return walker;
}

//************************************************************************************************************************

void FreeSprite(Sprite *sprite)
{
	if (sprite)
	{
		sprite->Type = 0;

		UnsortObject(sprite);

		if (sprite == LastSprite)
		{
			while((LastSprite > &Sprites[-1]) && (!LastSprite->Type))
				LastSprite--;
		}
	}
}

//************************************************************************************************************************



void MoveSprites()
{
	Sprite *walker;
	float frame;

	walker = &Sprites[0];
	while(walker <= LastSprite)
	{
		if (walker->Type)
		{
			frame = (float)(GameLoop - walker->FirstLoopNumber)*walker->Type->FrameMultiplier;
			if (frame >= 0.0)
			{

				walker->Frame = (int)(floor(frame)+0.4);
				if (walker->Frame >= walker->Type->NumFrames)
					FreeSprite(walker);
				else
					if (walker->Type->XVel || walker->Type->YVel)
					{
						frame = (float)(GameLoop - LastGameLoop);
						walker->Position.X += walker->Type->XVel*frame;
						walker->Position.Y += walker->Type->YVel*frame;
					}
			}
		}
		walker++;
	}
}

//************************************************************************************************************************

void DisplaySprites(CameraInstance *camera)
{
	Sprite *walker;
	FPoint rel;

	walker = &Sprites[0];
	while(walker <= LastSprite)
	{
		if (walker->Type)
		{
			rel.MakeVectorToFrom(walker->Position,camera->CameraLocation);

			if (CloseEnoughToDraw(walker->Position) && RadiusInView(rel,walker->Type->RWWidth))
			{
				walker->Rotated.Z = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R2C0);

				if (walker->Rotated.Z < ZClipPlane)
				{
					walker->Rotated.X = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R0C0);
					walker->Rotated.Y = rel * (*(FPoint *)&ViewMatrix.m_Data.RC.R1C0);

					walker->Fade = walker->Type->NumFrames - ((float)(GameLoop - walker->FirstLoopNumber)*walker->Type->FrameMultiplier);

					if (walker->Fade <= (float)walker->Type->FramesToFade)
					{
						if (walker->Fade > 0.0f)
							walker->Fade = walker->Fade*walker->Type->OneOverFramesToFade;
						else
							walker->Fade = 0.0f;
					}
					else
						walker->Fade = 1.0f;

					DrawSprite(walker);

				}
			}
		}
		walker++;
	}
}

//************************************************************************************************************************

void DrawSprite(Sprite *sprite)
{
	RotPoint3D Points[4];
	RotPoint3D *walker;
	int clip_flags,draw_flags;
	float w;
	SpriteType *type;

	type = sprite->Type;

	w = type->RWWidth;

	Points[0].Rotated = sprite->Rotated;
	Points[1].Rotated = sprite->Rotated;
	Points[2].Rotated = sprite->Rotated;
	Points[3].Rotated = sprite->Rotated;

	Points[0].Rotated.Y += w;

	Points[1].Rotated.X += w;

	Points[2].Rotated.Y -= w;

	Points[3].Rotated.X -= w;

	walker = &Points[0];
	int i = 4;
	while(i--)
	{
		walker->Flags = PT3_ROTATED;
		walker->Light = sprite->Fade;
		PerspectPoint(walker++);
	}

	draw_flags = Points[0].Flags & Points[1].Flags & Points[2].Flags & Points[3].Flags;

	if (!(draw_flags & PT3_NODRAW))
	{
		RotPoint3D *points[4];
		float ULU,ULV,LRU,LRV;

		ULU= type->ULU[sprite->Frame];
		ULV= type->ULV[sprite->Frame];

		LRU= ULU+type->UWidth;
		LRV= ULV+type->VHeight;

		if (sprite->Orientation & ORIENTATION_FLIP_U)
			Swap4(&ULU,&LRU);

		if (sprite->Orientation & ORIENTATION_FLIP_V)
			Swap4(&ULV,&LRV);

		switch(sprite->Orientation & ORIENTATION_ROTATION)
		{
			case ORIENTATION_ROTATE_0:
				Points[0].U = ULU;
				Points[0].V = ULV;
				Points[2].U = LRU;
				Points[2].V = LRV;
				break;

			case ORIENTATION_ROTATE_90:
				Points[0].U = ULU;
				Points[0].V = LRV;
				Points[2].U = LRU;
				Points[2].V = ULV;
				break;

			case ORIENTATION_ROTATE_180:
				Points[0].U = LRU;
				Points[0].V = LRV;
				Points[2].U = ULU;
				Points[2].V = ULV;
				break;

			case ORIENTATION_ROTATE_270:
				Points[0].U = LRU;
				Points[0].V = ULV;
				Points[2].U = ULU;
				Points[2].V = LRV;
				break;
		}

		Points[1].U = Points[2].U;
		Points[1].V = Points[0].V;
		Points[3].U = Points[0].U;
		Points[3].V = Points[2].V;


		CurrentTexture = sprite->Type->Texture;

		clip_flags = Points[0].Flags | Points[1].Flags | Points[2].Flags | Points[3].Flags;

		points[0] = &Points[0];
		points[1] = &Points[1];
		points[2] = &Points[2];
		points[3] = &Points[3];

		if (sprite->Type->PolyType & PRIM_ALPHA_ADD)
		{
			Points[0].Diffuse = Points[1].Diffuse = Points[2].Diffuse = Points[3].Diffuse.SetValues(sprite->Fade,SkyCurrentColors[0]);
			points[0]->Alpha = points[1]->Alpha = points[2]->Alpha = points[3]->Alpha = 1.0f;//color;
		}
		else
		{
			Points[0].Diffuse = Points[1].Diffuse = Points[2].Diffuse = Points[3].Diffuse = SkyCurrentColors[0];
			points[0]->Alpha = points[1]->Alpha = points[2]->Alpha = points[3]->Alpha = sprite->Fade;//color;
		}

		Points[0].Specular = Points[1].Specular = Points[2].Specular = Points[3].Specular.SetValues(0.0f);

		DrawPolyClipped(sprite->Type->PolyType,CL_CLIP_UV,4, (RotPoint3D **)&points[0],  clip_flags & PT3_CLIP_FLAGS);
	}
}