// vehicles.cpp

#include "F18.h"
#include "particle.h"
#include "GameSettings.h"

extern DBRadarType *GetRadarPtr(long id);
extern DBWeaponType *get_weapon_ptr(long id);
extern CameraInstance *glob_camera;

extern void PlayPositionalExplosion(int iWarheadType, CameraInstance &camera,FPointDouble &SoundPosition);
void UpdateCanisterPos( DWORD hCan, FPointDouble &pos);
void DrawWake(MovingVehicleParams *V);
MovingVehicleParams *MeatBallCarrier;
BYTE MeatBallLights[50];

void VCheckDeckMove(PlaneParams *planepnt);
extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);
extern D3DCOLOR d3dcFogColor;
void ReleaseShadowEntry(void *pt);

#define WAKE_FEET_TO_PIXELS (1.0f/ ((5.0f FEET) * 256.0f))
#define WAKE_TICKS_PER_SEGMENT		256
#define WAKE_TICKS_PER_SEGMENT_M1	255

extern float RWWidthForLines;
extern DWORD DeckShadows;
MovingVehicleParams *VehiclesToDraw;

extern float GetCrossSection(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);
extern float GetCrossSectionFromGround(int CrossSignature, int Prf, float DeltaAlt, float RelVel, float TargetDist, int UseSTT, float orgMaxRange = -1.0f);
extern void LandingBurnerSound(PlaneParams *P);
extern float fBolterAdjust;

//*************************************************************************************************
PlaneParams *GetNewCarrierPlane(PlaneParams *P,MovingVehicleParams *carrier)
{
	PlaneParams *new_p;
	FPointDouble relative_position;
	FMatrix back_r;

	new_p = (PlaneParams *)calloc(1,sizeof(PlaneParams));

	if (new_p)
	{

		memcpy(new_p,P,sizeof(PlaneParams));

		new_p->Status |= PL_CARRIER_REL;

		back_r = carrier->Attitude;
		back_r.Transpose();

		new_p->WorldPosition -= carrier->WorldPosition;

		if((MultiPlayer) && ((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane)))
		{
			FPoint coffset;
			coffset = new_p->WorldPosition;
			NetPutGenericMessage2FPoint(P, GM2FP_PUT_ON_DECK, coffset);
		}

		new_p->WorldPosition *= back_r;

		new_p->Attitude *= back_r;

		new_p->Orientation.I.X = -new_p->Attitude.m_Data.RC.R0C2;
		new_p->Orientation.I.Y = -new_p->Attitude.m_Data.RC.R1C2;
		new_p->Orientation.I.Z = -new_p->Attitude.m_Data.RC.R2C2;

		new_p->Orientation.J.X = new_p->Attitude.m_Data.RC.R0C0;
		new_p->Orientation.J.Y = new_p->Attitude.m_Data.RC.R1C0;
		new_p->Orientation.J.Z = new_p->Attitude.m_Data.RC.R2C0;

		new_p->Orientation.K.X = -new_p->Attitude.m_Data.RC.R0C1;
		new_p->Orientation.K.Y = -new_p->Attitude.m_Data.RC.R1C1;
		new_p->Orientation.K.Z = -new_p->Attitude.m_Data.RC.R2C1;

		new_p->Attitude.GetRPH(&new_p->Roll,&new_p->Pitch,&new_p->Heading);

		/* -----------------4/30/99 9:17AM---------------------------------------------------------------------
			* hopefully this will keep the physics from correcting
			* ----------------------------------------------------------------------------------------------------*/
		new_p->DesiredRoll = new_p->Roll;
		new_p->DesiredPitch = new_p->Pitch;

		new_p->Status &= ~PL_NEED_ATTITUDE;

//		new_p->PlaneCopy = P;

		new_p->IfVelocity *= back_r;

		new_p->LastWorldPosition -= carrier->WorldPosition;
		new_p->LastWorldPosition *= back_r;

		new_p->HeightAboveGround = new_p->WorldPosition.Y;

		P->AI.fVarA[4] = new_p->WorldPosition.X;
		P->AI.fVarA[5] = new_p->WorldPosition.Y;
		P->AI.fVarA[6] = new_p->WorldPosition.Z;

		P->AI.fVarA[7] = (double)new_p->Heading * (180.0/(double)0x8000);
		P->AI.fVarA[8] = new_p->Pitch * (180.0/(double)0x8000);
	}

	return new_p;

}

PlaneParams *GetNewCarrierPlane(PlaneParams *P)
{
	return GetNewCarrierPlane(P,&MovingVehicles[P->AI.iHomeBaseId]);

}

void CopyWorldPlaneToCarrierPlane(PlaneParams *source)
{
	PlaneParams temp,*dest = source->PlaneCopy;

	if (dest)
	{
		memcpy(&temp,dest,sizeof(PlaneParams));
		memcpy(dest,source,sizeof(PlaneParams));

		dest->PlaneCopy = NULL;

		dest->AI.fVarA[4] = temp.AI.fVarA[4];
		dest->AI.fVarA[5] = temp.AI.fVarA[5];
		dest->AI.fVarA[6] = temp.AI.fVarA[6];

		dest->AI.fVarA[7] = temp.AI.fVarA[7];
		dest->AI.fVarA[8] = temp.AI.fVarA[8];

		dest->WorldPosition = temp.WorldPosition;
		dest->Attitude = temp.Attitude;
		dest->Orientation =  temp.Orientation;
		dest->IfVelocity = temp.IfVelocity;
		dest->LastWorldPosition = temp.LastWorldPosition;

		dest->Roll = temp.Roll;
		dest->Pitch = temp.Pitch;
		dest->Heading = temp.Heading;

		dest->HeightAboveGround = temp.HeightAboveGround;

		dest->Status |= PL_CARRIER_REL;

	}
}

void CopyCarrierPlaneToWorldPlane(PlaneParams *dest)
{
	PlaneParams *source = dest->PlaneCopy;
	MovingVehicleParams *carrier;

	if (source)
	{
		source->Status |= PL_NEED_ATTITUDE;
		CalcAttitude(source);
		memcpy(dest,source,sizeof(PlaneParams));

		dest->PlaneCopy = source;
		dest->Status &= ~PL_CARRIER_REL;

		dest->AI.fVarA[4] = source->WorldPosition.X;
		dest->AI.fVarA[5] = source->WorldPosition.Y;
		dest->AI.fVarA[6] = source->WorldPosition.Z;

		dest->AI.fVarA[7] = source->Heading * (180.0/(double)0x8000);
		dest->AI.fVarA[8] = source->Pitch * (180.0/(double)0x8000);

		carrier = &MovingVehicles[dest->AI.iHomeBaseId];

		dest->WorldPosition *= carrier->Attitude;
		dest->WorldPosition += carrier->WorldPosition;

		dest->Attitude.SetRPH(0,dest->Pitch,dest->Heading);

		dest->Attitude *= carrier->Attitude;

		dest->Orientation.I.X = -dest->Attitude.m_Data.RC.R0C2;
		dest->Orientation.I.Y = -dest->Attitude.m_Data.RC.R1C2;
		dest->Orientation.I.Z = -dest->Attitude.m_Data.RC.R2C2;

		dest->Orientation.J.X = dest->Attitude.m_Data.RC.R0C0;
		dest->Orientation.J.Y = dest->Attitude.m_Data.RC.R1C0;
		dest->Orientation.J.Z = dest->Attitude.m_Data.RC.R2C0;

		dest->Orientation.K.X = -dest->Attitude.m_Data.RC.R0C1;
		dest->Orientation.K.Y = -dest->Attitude.m_Data.RC.R1C1;
		dest->Orientation.K.Z = -dest->Attitude.m_Data.RC.R2C1;

		dest->Attitude.GetRPH(&dest->Roll,&dest->Pitch,&dest->Heading);

		/* -----------------4/30/99 9:17AM---------------------------------------------------------------------
			* hopefully this will keep the physics from correcting
			* ----------------------------------------------------------------------------------------------------*/
//		dest->DesiredRoll = dest->Roll;
//		dest->DesiredPitch = dest->Pitch;

		dest->Status &= ~PL_NEED_ATTITUDE;

		dest->IfVelocity *= carrier->Attitude;

		dest->LastWorldPosition *= carrier->Attitude;
		dest->LastWorldPosition += carrier->WorldPosition;

		dest->HeightAboveGround = dest->WorldPosition.Y - LandHeight(dest->WorldPosition.X,dest->WorldPosition.Z);
	}

}

void ApplyCarrierMovementToPlane(PlaneParams *dest)
{
	PlaneParams *source = dest->PlaneCopy;
	MovingVehicleParams *carrier;

	if (source)
	{
		dest->AI.fVarA[4] = source->WorldPosition.X;
		dest->AI.fVarA[5] = source->WorldPosition.Y;
		dest->AI.fVarA[6] = source->WorldPosition.Z;

		dest->AI.fVarA[7] = source->Heading * (180.0/(double)0x8000);
		dest->AI.fVarA[8] = source->Pitch * (180.0/(double)0x8000);

		carrier = &MovingVehicles[dest->AI.iHomeBaseId];

		dest->WorldPosition.RotateInto(source->WorldPosition,carrier->Attitude);
		dest->WorldPosition += carrier->WorldPosition;

		dest->Attitude.SetRPH(0,source->Pitch,source->Heading);

		dest->Attitude *= carrier->Attitude;

		dest->Orientation.I.X = -dest->Attitude.m_Data.RC.R0C2;
		dest->Orientation.I.Y = -dest->Attitude.m_Data.RC.R1C2;
		dest->Orientation.I.Z = -dest->Attitude.m_Data.RC.R2C2;

		dest->Orientation.J.X = dest->Attitude.m_Data.RC.R0C0;
		dest->Orientation.J.Y = dest->Attitude.m_Data.RC.R1C0;
		dest->Orientation.J.Z = dest->Attitude.m_Data.RC.R2C0;

		dest->Orientation.K.X = -dest->Attitude.m_Data.RC.R0C1;
		dest->Orientation.K.Y = -dest->Attitude.m_Data.RC.R1C1;
		dest->Orientation.K.Z = -dest->Attitude.m_Data.RC.R2C1;

		dest->Attitude.GetRPH(&dest->Roll,&dest->Pitch,&dest->Heading);

		/* -----------------4/30/99 9:17AM---------------------------------------------------------------------
			* hopefully this will keep the physics from correcting
			* ----------------------------------------------------------------------------------------------------*/
//		dest->DesiredRoll = dest->Roll;
//		dest->DesiredPitch = dest->Pitch;

		dest->Status &= ~PL_NEED_ATTITUDE;

		dest->IfVelocity.RotateInto(source->IfVelocity,carrier->Attitude);

		dest->LastWorldPosition.RotateInto(source->LastWorldPosition,carrier->Attitude);
		dest->LastWorldPosition += carrier->WorldPosition;

		dest->HeightAboveGround = dest->WorldPosition.Y - LandHeight(dest->WorldPosition.X,dest->WorldPosition.Z);
	}
}


BOOL PlaceVehiclesOnBridges( CameraInstance *camera )
{
//	int return_value = FALSE;
//	StructureInstance *bridge;
//	MovingVehicleParams **bridge_v;

//	int i=0;

//	for (MovingVehicleParams *W=MovingVehicles; W<=LastMovingVehicle; W++)
//	{
//		if(!W->Type)
//			W->Status &= ~(VL_ACTIVE);

//		if((W->Status & VL_ACTIVE) && (!(W->Status & (VL_INVISIBLE))))
//		{
//			if ((W->Status & VL_ON_BRIDGE) && !(W->Bridge->Basics.Flags & BI_DESTROYED))
//			{
//				if (CloseEnoughToDraw(W->WorldPosition))
//				{
//					return_value = TRUE;
//					W->OrdDistanceFromCamera.MakeVectorToFrom(W->WorldPosition,camera->CameraLocation);
//					W->DistanceFromCamera = W->OrdDistanceFromCamera.QuickLength();

//					bridge = W->Bridge;

//					if (W->Status & VL_ON_BRIDGE_RAMP_2)
//						bridge_v = (MovingVehicleParams **)&bridge->SubObject2;
//					else
//						bridge_v = (MovingVehicleParams **)&bridge->SubObject1;

//					while(*bridge_v && ((*bridge_v)->DistanceFromCamera > W->DistanceFromCamera))
//						bridge_v = &((*bridge_v)->NextVehicleOnBridge);

//					W->NextVehicleOnBridge = *bridge_v;
//					*bridge_v = W;
//				}

//			}
//		}
//		i++;
//	}

	return FALSE;
}

void DisplayVehicles( CameraInstance *camera )
{
	FPoint rel;
	BOOL changed_z = FALSE;

	float value;
	float closest = 1000000.0;
	BOOL catch_carrier = TRUE;
	MovingVehicleParams *W;

	BYTE vars[64],*send_vars;

	if (ImagingMethod != IT_NORMAL) catch_carrier = FALSE;
  	else if (CockpitSeat == NO_SEAT)  catch_carrier = FALSE;
	else if (CurrentView & (COCKPIT_45UP | COCKPIT_BACK_SEAT))  catch_carrier = FALSE;
	else if (CurrentView == (COCKPIT_ACM_MODE | COCKPIT_ACM_FRONT | COCKPIT_ACM_DOWN))  catch_carrier = FALSE;
	else if (CurrentView & (COCKPIT_LEFT | COCKPIT_RIGHT))  catch_carrier = FALSE;

	if (ImagingMethod == IT_NORMAL)
		MeatBallCarrier = NULL;

	if (VehiclesToDraw)
	{
		W = VehiclesToDraw;
		while(W)
		{
			if (/*!((W->Status & VL_ON_BRIDGE) && !(W->Bridge->Basics.Flags & BI_DESTROYED)) && */W->Type->iShipType)
				DrawWake(W);
			W = W->NextVehicleToDraw;
		}

		W = VehiclesToDraw;

		while(W)
		{
			rel.MakeVectorToFrom(W->WorldPosition,camera->CameraLocation);

			if (W->Type->iShipType && (!W->Type->TypeNumber) && ((W->Type->Model->Name[0] | 0x20) == 'n') && ((W->Type->Model->Name[1] | 0x20) == 'i') && ((W->Type->Model->Name[2] | 0x20) == 't'))
			{
				send_vars = vars;
				value = SetMeatballVars(camera->CameraLocation,W,vars);
				if (catch_carrier && (value < closest) && (value != 255))
				{
					closest = value;
					MeatBallCarrier = W;
					memcpy(&MeatBallLights[32],&vars[32],17);
				}
			}
			else
				send_vars = NULL;

			if (RadiusInView(rel,GetObjectRadius(W->Type->Model)))
			{
				if (!changed_z)
				{
					lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL);
					D3DStates |= PERM_NO_ZFUNC_CHANGES;
					changed_z = TRUE;
				}

				DrawAVehicle(W,rel,send_vars);
			}

			W = W->NextVehicleToDraw;
		}

		VehiclesToDraw = NULL;

	}
	else
	{
		for (W=MovingVehicles; W<=LastMovingVehicle; W++)
		{
			if(!W->Type)
				W->Status &= ~(VL_ACTIVE);

			W->Status &= ~VL_CLOSE_ENOUGH;

			if((W->Status & VL_ACTIVE) && (!(W->Status & (VL_INVISIBLE))))
			{
//				if (!((W->Status & VL_ON_BRIDGE) && !(W->Bridge->Basics.Flags & BI_DESTROYED)))
//				{
					if (CloseEnoughToDraw(W->WorldPosition))
					{
						W->Status |= VL_CLOSE_ENOUGH;

						if (W->Type->iShipType)
							DrawWake(W);
					}
//				}
			}
		}

		for (W=MovingVehicles; W<=LastMovingVehicle; W++)
		{
			if (W->Status & VL_CLOSE_ENOUGH)
			{
				rel.MakeVectorToFrom(W->WorldPosition,camera->CameraLocation);

				if (W->Type->iShipType && (!W->Type->TypeNumber) && ((W->Type->Model->Name[0] | 0x20) == 'n') && ((W->Type->Model->Name[1] | 0x20) == 'i') && ((W->Type->Model->Name[2] | 0x20) == 't'))
				{
					send_vars = vars;
					value = SetMeatballVars(camera->CameraLocation,W,vars);
					if (catch_carrier && (value < closest) && (value != 255))
					{
						closest = value;
						MeatBallCarrier = W;
						memcpy(&MeatBallLights[32],&vars[32],17);
					}
				}
				else
					send_vars = NULL;

				if (RadiusInView(rel,GetObjectRadius(W->Type->Model)))
				{
					if (!changed_z)
					{
						lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL);
						D3DStates |= PERM_NO_ZFUNC_CHANGES;
						changed_z = TRUE;
					}

					DrawAVehicle(W,rel,send_vars);
				}
			}
		}
	}

	if (changed_z)
	{
		if (!(D3DStates & PRIM_ZFUNC_LE))
	   		lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESS);
		D3DStates &= ~PERM_NO_ZFUNC_CHANGES;
	}

}

float InitialInnerWakeDistance(float z,WakeBounds *bounds,DWORD* ptflags)
{
	float last_x = 0;
	int i;
	WakeBoundPoint *walker;

	walker = bounds->Points;
	i = bounds->NumPoints-1;
	z -= bounds->FirstZ;

	if (z < 0.0f)
	{
		*ptflags &= ~WP_PAST_BOAT;
		return 0.0f;
	}

	while(i-- && (walker->NextZ < z))
	{
		last_x = walker->CurrentX;
		z -= walker->NextZ;
		walker++;
	}

	if (i >= 0)
	{
		*ptflags &= ~WP_PAST_BOAT;
		return last_x+(walker->XSlope*z);
	}
	else
	{
	 	*ptflags |= WP_PAST_BOAT;
		return walker->CurrentX;
	}

}


void InitWake(MovingVehicleParams *V)
{
	WakeBounds*		   	bounds = &V->Type->WakeGen;
	Wake*		   		wake;
	WakePoint*			wpwalk;

	if ((bounds->NumPoints) && !(V->WakeData) && (V->Status & VL_MOVING) && (V->fSpeedFPS))
	{
		int i;
		FPoint right_vector;
		FPointDouble center_point;
		FPointDouble delta_center_point;
		DWORD  irval = rand() & WAKE_TICKS_PER_SEGMENT_M1;
		float  rval = (float)irval/(float)WAKE_TICKS_PER_SEGMENT;
		float  z,delta_z;
		float  v,delta_v,*vwalk;
		FMatrix mat;
		DWORD start_loop;
		float edge_scale;
		float tester;

		wake = (Wake *)calloc(1,sizeof(Wake));

		V->WakeData = wake;

		wake->FirstPoint = 0;
		wpwalk = wake->WakePoints;
		vwalk = wake->Vs;
		i = MAX_WAKE_POINTS;

		mat.SetRPH(0,0,V->Heading);

		FPointDouble p = V->WorldPosition;
		p.Y = LandHeight(p.X,p.Z);

		center_point.SetValues(0.0,0.0,(double)bounds->FirstZ);
		center_point *= mat;
		center_point += p;
		z = bounds->FirstZ;

		right_vector.SetValues(mat.m_Data.RC.R0C0,mat.m_Data.RC.R1C0,mat.m_Data.RC.R2C0);

		delta_center_point.SetValues((double)mat.m_Data.RC.R0C2,(double)mat.m_Data.RC.R1C2,(double)mat.m_Data.RC.R2C2);
		delta_z = (V->fSpeedFPS*(float)WAKE_TICKS_PER_SEGMENT)/50.0f FEET;
		delta_center_point*=(double)delta_z;

		center_point.AddScaledVector((double)rval,delta_center_point);
		z += rval*delta_z;

		start_loop = GameLoop - irval;
		wake->FirstTick = start_loop;

		v = frand();
		delta_v = delta_z * WAKE_FEET_TO_PIXELS;

		while(i--)
		{
			wpwalk->FirstTick = start_loop;
			start_loop -= WAKE_TICKS_PER_SEGMENT;

			edge_scale = (float)(GameLoop - wpwalk->FirstTick)/(WAKE_TICKS_PER_SEGMENT*2.0f);
			if (edge_scale > 1.0f)
				edge_scale = 1.0f;

			wpwalk->CenterPoint =center_point;
			center_point += delta_center_point;

			wpwalk->RightVector = right_vector;

			float the_rand = frand()*0.2f+0.8f;

			if (rand() & 1)
			{
				wpwalk->ShipVelocity = V->fSpeedFPS FEET * the_rand;
				wpwalk->ShipVelocityRight = V->fSpeedFPS FEET * the_rand*((frand()*0.2f)+0.8f);
			}
			else
			{
				wpwalk->ShipVelocity = V->fSpeedFPS FEET * the_rand*((frand()*0.2f)+0.8f);
				wpwalk->ShipVelocityRight = V->fSpeedFPS FEET * the_rand;
			}

			rval = (float)(GameLoop - wpwalk->FirstTick)*(1.0f/50.0f);
			if (rval > MAX_WAKE_EXPANSION_TIME)
			{
				wpwalk->Flags = WP_PAST_EXPANSION | WP_IS_VALID;
				rval = MAX_WAKE_EXPANSION_TIME;
			}
			else
				wpwalk->Flags =WP_IS_VALID;

			wpwalk->InnerLeft = InitialInnerWakeDistance(z,bounds,&wpwalk->Flags);
			wpwalk->InnerRight = -wpwalk->InnerLeft;

			tester = ((rval*MAX_WAKE_EXPANSION_TIME - 0.5*(rval*rval))*(2.0f/MAX_WAKE_EXPANSION_TIME))*bounds->WidestX*(0.1f);

//			tester = edge_scale * 2.0f;
//			if (tester > 1.0f)
//				tester = 1.0f;
//			tester *= 0.25*bounds->WidestX;

			wpwalk->OuterRight = wpwalk->ShipVelocityRight*tester;
			wpwalk->OuterLeft = wpwalk->ShipVelocity*-tester;

			tester = wpwalk->InnerRight + 6.0 FEET *edge_scale;

			if (wpwalk->OuterRight < tester)
				wpwalk->OuterRight = tester;

			tester = wpwalk->InnerLeft - 6.0 FEET *edge_scale;

			if (wpwalk->OuterLeft > tester)
				wpwalk->OuterLeft = tester;

			*vwalk++ = v;
			v+=delta_v;

			z+= delta_z;
			wpwalk++;

		}

	}
}

void AddNewWakePoint(MovingVehicleParams *V,float z,DWORD age,FPointDouble &center,FPoint &right)
{
	float new_v;
	Wake* wd = V->WakeData;
	WakeBounds* bounds = &V->Type->WakeGen;
	WakePoint* first_point;
	float rval;
	float edge_scale = (float)age/(WAKE_TICKS_PER_SEGMENT*2.0f);
	float tester;

	if (edge_scale > 1.0f)
		edge_scale = 1.0f;

	if (wd->WakePoints[wd->FirstPoint].Flags & WP_IS_VALID)
	{
		new_v = V->fSpeedFPS * (1.0f/50.0f) FEET ;
		new_v *= WAKE_FEET_TO_PIXELS * (float)WAKE_TICKS_PER_SEGMENT;
		new_v += wd->Vs[wd->FirstPoint];
	}
	else
		new_v = frand();

	if (wd->FirstPoint)
		wd->FirstPoint--;
	else
		wd->FirstPoint = MAX_WAKE_POINTS_M1;

	wd->Vs[wd->FirstPoint] = new_v;

	first_point = &wd->WakePoints[wd->FirstPoint];
	first_point->Flags = WP_IS_VALID;
	first_point->FirstTick = GameLoop - age;
	first_point->CenterPoint = center;

	float the_rand = frand()*0.3f+0.7f;

	if (rand() & 1)
	{
		first_point->ShipVelocity = V->fSpeedFPS FEET * the_rand;
		first_point->ShipVelocityRight = V->fSpeedFPS FEET * the_rand*((frand()*0.3f)+0.7f);
	}
	else
	{
		first_point->ShipVelocity = V->fSpeedFPS FEET * the_rand*((frand()*0.3f)+0.7f);
		first_point->ShipVelocityRight = V->fSpeedFPS FEET * the_rand;
	}


	first_point->RightVector = right;

	first_point->InnerLeft = InitialInnerWakeDistance(z,bounds,&first_point->Flags);
	first_point->InnerRight = -first_point->InnerLeft;

	rval = (float)age * (1.0f/50.0f);

	if (rval > MAX_WAKE_EXPANSION_TIME)
	{
		first_point->Flags |= WP_PAST_EXPANSION;
		rval = MAX_WAKE_EXPANSION_TIME;
	}


	tester = ((rval*MAX_WAKE_EXPANSION_TIME - 0.5*(rval*rval))*(2.0f/MAX_WAKE_EXPANSION_TIME))*bounds->WidestX*(0.1f);
//	tester = (rval*MAX_WAKE_EXPANSION_TIME - 0.5*(rval*rval))*bounds->WidestX*(1.0f/15.0f);

//	tester = edge_scale * 2.0f;
//	if (tester > 1.0f)
//		tester = 1.0f;
//	tester *= 0.25*bounds->WidestX;

	first_point->OuterRight = first_point->ShipVelocityRight*tester;
	first_point->OuterLeft = first_point->ShipVelocity*-tester;

	tester = first_point->InnerRight + 6.0 FEET *edge_scale;

	if (first_point->OuterRight < tester)
		first_point->OuterRight = tester;

	tester = first_point->InnerLeft - 6.0 FEET *edge_scale;

	if (first_point->OuterLeft > tester)
		first_point->OuterLeft = tester;

}

void UpdateWake(MovingVehicleParams *V)
{
	Wake* wd = V->WakeData;
	WakePoint *wp;
	WakeBounds* bounds = &V->Type->WakeGen;
	int count;
	int i;
	float rval;
	FPoint point;
	int age;
	float edge;
	FMatrix mat,tmat;
	FPointDouble ap_point;
	DWORD f1;
	DWORD new_flags;
	FPointDouble p = V->WorldPosition;
	p.Y = LandHeight(p.X,p.Z);
	float edge_scale;
	float tester;

	tmat = mat.SetRPH(0,0,V->Heading);
	tmat.Transpose();

	/* -----------------6/8/99 4:46PM----------------------------------------------------------------------
	 * first see if we have to lay out a new point
	 * ----------------------------------------------------------------------------------------------------*/

	if ((age = (GameLoop - wd->FirstTick)) > WAKE_TICKS_PER_SEGMENT_M1)
	{
		FPointDouble center,delta;
		FPointDouble tip;
		FPoint right_vector;
		FPoint working_vector;
		float z,delta_z;

		tip.SetValues(0.0,0.0,(double)bounds->FirstZ);
		tip *= mat;
		tip += p;

		if (wd->WakePoints[wd->FirstPoint].Flags)
		{
			center = wd->WakePoints[wd->FirstPoint].CenterPoint;
			delta.MakeVectorToFrom(tip,center);
			delta *= (float)WAKE_TICKS_PER_SEGMENT/(float)age;
		}
		else
		{
			delta.SetValues(0.0,0.0,(double)V->fSpeedFPS/50.0 FEET);
			delta *= mat;
			center = tip;
			center.AddScaledVector((double)age,delta);
			delta *= (float)-WAKE_TICKS_PER_SEGMENT;
		}

		working_vector.MakeVectorToFrom(center,tip);
		working_vector *= tmat;

		if (fabs(working_vector.Z) < 0.1)
		{
			while (age > WAKE_TICKS_PER_SEGMENT_M1)
			{
				age -= WAKE_TICKS_PER_SEGMENT;
				if (wd->FirstPoint)
					wd->FirstPoint--;
				else
					wd->FirstPoint = MAX_WAKE_POINTS_M1;

				wd->WakePoints[wd->FirstPoint].Flags = 0;
				wd->WakePoints[wd->FirstPoint].FirstTick = (DWORD)(GameLoop - age);
			}

		}
		else
		{
			z = working_vector.Z;
			z += bounds->FirstZ;
			delta_z = (z*(float)WAKE_TICKS_PER_SEGMENT)/(float)age;

			right_vector.SetValues(0.0f,-1.0f,0.0f);
			right_vector %= delta;
			right_vector.Normalize();

			while (age > WAKE_TICKS_PER_SEGMENT_M1)
			{
				age -= WAKE_TICKS_PER_SEGMENT;
				center += delta;
				z+=delta_z;
				AddNewWakePoint(V,z,(DWORD)(age),center,right_vector);
			}
		}

		wd->FirstTick = GameLoop - age;
	}

	wp = &wd->WakePoints[wd->FirstPoint];
	i = wd->FirstPoint;
	count= MAX_WAKE_POINTS;

	while(count--)
	{
		if (wp->Flags & WP_IS_VALID)
		{
			new_flags = wp->Flags;

			if (!(wp->Flags & WP_PAST_BOAT))
			{
				ap_point = wp->CenterPoint;
				ap_point.AddScaledVector(wp->InnerLeft,RightVector);
				ap_point -= p;
				ap_point *= tmat;

				edge = InitialInnerWakeDistance(ap_point.Z,bounds,&f1);
				if (edge < ap_point.X)
					wp->InnerLeft = edge;

				ap_point = wp->CenterPoint;
				ap_point.AddScaledVector(wp->InnerRight,RightVector);
				ap_point -= p;
				ap_point *= tmat;

				edge = -InitialInnerWakeDistance(ap_point.Z,bounds,&new_flags);
				if (edge > ap_point.X)
					wp->InnerRight = edge;

				new_flags &= (f1 & WP_PAST_BOAT) | ~WP_PAST_BOAT;

			}

			if (!(wp->Flags & WP_PAST_EXPANSION))
			{
	  			rval = (float)(GameLoop - wp->FirstTick) * (1.0f/50.0f);

				if (rval > MAX_WAKE_EXPANSION_TIME)
				{
					new_flags |= WP_PAST_EXPANSION;
					rval = MAX_WAKE_EXPANSION_TIME;
				}

				tester = ((rval*MAX_WAKE_EXPANSION_TIME - 0.5*(rval*rval))*(2.0f/MAX_WAKE_EXPANSION_TIME))*bounds->WidestX*(0.1f);
//				tester = (rval*MAX_WAKE_EXPANSION_TIME - 0.5*(rval*rval))*bounds->WidestX*(1.0f/15.0f);

//				edge_scale = (float)(GameLoop - wp->FirstTick)/(WAKE_TICKS_PER_SEGMENT*2.0f);

//				if (edge_scale > 1.0f)
//					edge_scale = 1.0f;

//				tester = edge_scale * 2.0f;
//				if (tester > 1.0f)
//					tester = 1.0f;
//				tester *= 0.25*bounds->WidestX;

				wp->OuterRight = wp->ShipVelocityRight*tester;
				wp->OuterLeft = wp->ShipVelocity*-tester;

			}


			if (!(wp->Flags & WP_PAST_BOAT))// |  WP_PAST_EXPANSION)))
			{

//				if (!(wp->Flags & WP_PAST_EXPANSION))
				{
					edge_scale = (float)(GameLoop - wp->FirstTick)/(WAKE_TICKS_PER_SEGMENT*2.0f);
					if (edge_scale > 1.0f)
						edge_scale = 1.0f;
				}

				tester = wp->InnerRight + 6.0 FEET*edge_scale;

				if (wp->OuterRight < tester)
					wp->OuterRight = tester;

				tester = wp->InnerLeft - 6.0 FEET*edge_scale;

				if (wp->OuterLeft > tester)
					wp->OuterLeft = tester;
			}

			wp->Flags = new_flags;
		}


		if (++i > MAX_WAKE_POINTS_M1)
		{
			i = 0;
			wp = &wd->WakePoints[0];
		}
		else
			wp++;
	}

}


RotPoint3D *TopPoints,*BottomPoints;
RotPoint3D WakePoints[10];
extern FPointDouble GlobEye;
#define WAKE_SHRINK_FACTOR 0.95f

int PerspectPointWithAllFlags(RotPoint3D *point);

void MakeBottomPoints(WakePoint *wp,float v)
{
	RotPoint3D *pwalk,*center;
	FPoint right_vector;
	FPoint diffuse_inner;
	FPoint diffuse_outer;
	float alpha;
	float fade_in;
	float age;

	center = &BottomPoints[2];

	fade_in = (float)(GameLoop - wp->FirstTick);
	age = (fade_in * (1.0f/50.0f))-10.0f;

	if (fade_in >= (float)WAKE_TICKS_PER_SEGMENT)
		fade_in = 1.0f;
	else
		fade_in *= 1.0f/((float)WAKE_TICKS_PER_SEGMENT);

	fade_in *= fade_in;

	if (age < 0.0)
	{
		alpha = 1.0f;
//		diffuse_inner.SetValues(fade_in*SunLight.Intensity,SunLight.Color);
		diffuse_inner.SetValues(fade_in,NaturalLight);
	}
	else
	{
		age *= 1.0f/((float)(MAX_WAKE_POINTS*WAKE_TICKS_PER_SEGMENT)/50.0f - 10.0f);
		if (age > 1.0f)
		{
			alpha = 0.0f;
			diffuse_inner.SetValues(0.0f);
		}
		else
		{
			alpha = 1.0f - age;
//			diffuse_inner.SetValues(fade_in*alpha*alpha*alpha*alpha*SunLight.Intensity,SunLight.Color);
			diffuse_inner.SetValues(fade_in*alpha*alpha*alpha*alpha,NaturalLight);
		}

	}


	//diffuse_outer.SetValues(0.0f);//alpha*0.01f*SunLight.Intensity,SunLight.Color);

	center->Rotated.MakeVectorToFrom(wp->CenterPoint,GlobEye);
	center->Rotated *= WAKE_SHRINK_FACTOR;
	center->Rotated *= ViewMatrix;

	right_vector.RotateInto(wp->RightVector,ViewMatrix);
	right_vector *= WAKE_SHRINK_FACTOR;

	pwalk = BottomPoints;

	pwalk->Rotated = center->Rotated;
	pwalk->Rotated.AddScaledVector(wp->OuterLeft,right_vector);
	pwalk->Alpha = 1.0f;
	pwalk->Diffuse.SetValues(0.0f);// = diffuse_outer;
	pwalk->U = 0.0f;
	pwalk->V = v;
	if (pwalk->Rotated.Z > ZClipPlane)
		pwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		pwalk->Flags = PT3_ROTATED;
		PerspectPointWithAllFlags(pwalk);
	}
	pwalk++;

	pwalk->Rotated = center->Rotated;
	pwalk->Rotated.AddScaledVector(wp->InnerLeft,right_vector);
	pwalk->Alpha = 1.0f;
	pwalk->Diffuse = diffuse_inner;
	pwalk->U = 0.5f;
	pwalk->V = v;
	if (pwalk->Rotated.Z > ZClipPlane)
		pwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		pwalk->Flags = PT3_ROTATED;
		PerspectPointWithAllFlags(pwalk);
	}
	pwalk++;

	pwalk->Alpha = 1.0f;
//	pwalk->Diffuse.SetValues(fade_in*alpha*SunLight.Intensity,SunLight.Color);
	pwalk->Diffuse.SetValues(fade_in*alpha,NaturalLight);
	pwalk->U = 126.9f/128.0f;
	pwalk->V = v;
	if (pwalk->Rotated.Z > ZClipPlane)
		pwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		pwalk->Flags = PT3_ROTATED;
		PerspectPointWithAllFlags(pwalk);
	}
	pwalk++;

	pwalk->Rotated = center->Rotated;
	pwalk->Rotated.AddScaledVector(wp->InnerRight,right_vector);
	pwalk->Alpha = 1.0f;
	pwalk->Diffuse = diffuse_inner;
	pwalk->U = 0.5f;
	pwalk->V = v+0.5f;
	if (pwalk->Rotated.Z > ZClipPlane)
		pwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		pwalk->Flags = PT3_ROTATED;
		PerspectPointWithAllFlags(pwalk);
	}
	pwalk++;

	pwalk->Rotated = center->Rotated;
	pwalk->Rotated.AddScaledVector(wp->OuterRight,right_vector);
	pwalk->Alpha = 1.0f;
	pwalk->Diffuse.SetValues(0.0f);// = diffuse_outer;
	pwalk->U = 0.0f;
	pwalk->V = v+0.5f;
	if (pwalk->Rotated.Z > ZClipPlane)
		pwalk->Flags = PT3_RZHIGH | PT3_ROTATED;
	else
	{
		pwalk->Flags = PT3_ROTATED;
		PerspectPointWithAllFlags(pwalk);
	}
	pwalk++;

}

void DrawOneWakePoly(RotPoint3D **vp)
{
	int draw_flags = (vp[0]->Flags & vp[1]->Flags & vp[2]->Flags & vp[3]->Flags) & PT3_NODRAW;

	if (!draw_flags)
	{
		int clip_flags = (vp[0]->Flags | vp[1]->Flags | vp[2]->Flags | vp[3]->Flags) & PT3_CLIP_FLAGS;

		if (clip_flags)
			DrawPolyClipped(PRIM_TEXTURE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_MIPMAP | PRIM_NO_Z_WRITE | PRIM_ZFUNC_LE,CL_CLIP_UV,4,vp,clip_flags);
		else
			DoD3DGop(PRIM_TEXTURE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_MIPMAP | PRIM_NO_Z_WRITE | PRIM_ZFUNC_LE,4,vp);
	}
}

void DrawWakeSection(void)
{
	RotPoint3D *vpoints[4];
	/* -----------------6/8/99 9:53AM----------------------------------------------------------------------
	 * we'll do this unrolled for speed
	 * ----------------------------------------------------------------------------------------------------*/

	vpoints[0] = &BottomPoints[0];
	vpoints[1] = &TopPoints[0];
	vpoints[2] = &TopPoints[1];
	vpoints[3] = &BottomPoints[1];

	DrawOneWakePoly(&vpoints[0]);

	vpoints[0] = &BottomPoints[1];
	vpoints[1] = &TopPoints[1];
	vpoints[2] = &TopPoints[2];
	vpoints[3] = &BottomPoints[2];

	DrawOneWakePoly(&vpoints[0]);

	TopPoints[2].V += 0.5f;
	BottomPoints[2].V += 0.5f;

	vpoints[0] = &BottomPoints[3];
	vpoints[1] = &BottomPoints[2];
	vpoints[2] = &TopPoints[2];
	vpoints[3] = &TopPoints[3];

	DrawOneWakePoly(&vpoints[0]);

	vpoints[0] = &BottomPoints[4];
	vpoints[1] = &BottomPoints[3];
	vpoints[2] = &TopPoints[3];
	vpoints[3] = &TopPoints[4];

	DrawOneWakePoly(&vpoints[0]);

	TopPoints[2].V -= 0.5f;
	BottomPoints[2].V -= 0.5f;


}

void DrawOneWakePeakPoly(RotPoint3D **vp)
{
	int draw_flags = (vp[0]->Flags & vp[1]->Flags & vp[2]->Flags) & PT3_NODRAW;

	if (!draw_flags)
	{
		int clip_flags = (vp[0]->Flags | vp[1]->Flags | vp[2]->Flags) & PT3_CLIP_FLAGS;

		if (clip_flags)
			DrawPolyClipped(PRIM_TEXTURE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_MIPMAP | PRIM_NO_Z_WRITE,CL_CLIP_UV,3,vp,clip_flags);
		else
			DoD3DGop(PRIM_TEXTURE | PRIM_ALPHAIMM | PRIM_ALPHA_ADD | PRIM_MIPMAP | PRIM_NO_Z_WRITE,3,vp);
	}
}

void DrawWakePeakSection(void)
{
	RotPoint3D *vpoints[4];
	/* -----------------6/8/99 9:53AM----------------------------------------------------------------------
	 * we'll do this unrolled for speed
	 * ----------------------------------------------------------------------------------------------------*/

	vpoints[0] = &TopPoints[0];

	vpoints[1] = &BottomPoints[1];
	vpoints[2] = &BottomPoints[0];

	TopPoints[0].U = 0.5f;
//	TopPoints[0].Diffuse.SetValues(1.0f,0.0f,0.0f); //debug

	DrawOneWakePeakPoly(&vpoints[0]);

	vpoints[1] = &BottomPoints[1];
	vpoints[2] = &BottomPoints[2];

	TopPoints[0].U = 126.9f/128.0f;
//	TopPoints[0].Diffuse.SetValues(0.0f,1.0f,0.0f); //debug

	DrawOneWakePeakPoly(&vpoints[0]);

	TopPoints[0].V += 0.5f;
	BottomPoints[2].V += 0.5f;

	vpoints[1] = &BottomPoints[3];
	vpoints[2] = &BottomPoints[2];

	DrawOneWakePeakPoly(&vpoints[0]);

	vpoints[1] = &BottomPoints[4];
	vpoints[2] = &BottomPoints[3];

	TopPoints[0].U = 0.5f;
//	TopPoints[0].Diffuse.SetValues(1.0f,0.0f,0.0f); //debug

	DrawOneWakePeakPoly(&vpoints[0]);

	BottomPoints[2].V -= 0.5f;

}


extern TextureBuff *WakeTexture;
extern FPoint *LastSurfaceNormal;
extern FMatrix *LastObjToViewMatrix;

void DrawWake(MovingVehicleParams *V)
{
	FMatrix mat;
	Wake* wd = V->WakeData;
	WakePoint *wp;
	float *wv;
	FPoint normal(0.0f,1.0f,0.0f);
	BOOL last_was_valid;
	FPointDouble p;
	FPoint rel_pos;

	if (!wd)
	{
		InitWake(V);
		wd = V->WakeData;
	}

	if (wd)
	{

		float point_v = V->fSpeedFPS * (1.0f/50.0f) FEET;
		point_v *= WAKE_FEET_TO_PIXELS * (float)(GameLoop - wd->FirstTick);
		point_v += wd->Vs[wd->FirstPoint];

		p=V->WorldPosition;
		p.Y = LandHeight(p.X,p.Z);

		rel_pos.MakeVectorToFrom(p,GlobEye);

		rel_pos *= WAKE_SHRINK_FACTOR;                // bring everything closer

		TopPoints = WakePoints;
		BottomPoints = &WakePoints[5];

		mat.SetRPH(0,0,V->Heading);

		LastSurfaceNormal = &normal;
		LastObjToViewMatrix = &ViewMatrix;

		CurrentTexture = WakeTexture;

		TopPoints->Rotated.SetValues(0.0f,0.0f,V->Type->WakeGen.FirstZ*WAKE_SHRINK_FACTOR);
		TopPoints->Rotated *= mat;
		TopPoints->Rotated += rel_pos;
		TopPoints->Rotated *= ViewMatrix;

		TopPoints->Alpha = 1.0f;
		TopPoints->Diffuse.SetValues(0.0f);//SunLight.Intensity,SunLight.Color);
		TopPoints->U = 1.0f;
		TopPoints->V = point_v;

		if (TopPoints->Rotated.Z > ZClipPlane)
			TopPoints->Flags = PT3_RZHIGH | PT3_ROTATED;
		else
		{
			TopPoints->Flags = PT3_ROTATED;
			PerspectPointWithAllFlags(TopPoints);
		}

		if (wd->WakePoints[wd->FirstPoint].Flags & WP_IS_VALID)
		{
			MakeBottomPoints(&wd->WakePoints[wd->FirstPoint],wd->Vs[wd->FirstPoint]);
			DrawWakePeakSection();
			last_was_valid = TRUE;
		}
		else
			last_was_valid = FALSE;

		int i = MAX_WAKE_POINTS_M1;
		int j = wd->FirstPoint;

		wv = &wd->Vs[wd->FirstPoint];
		wp = &wd->WakePoints[wd->FirstPoint];


		while(i--)
		{
			Swap4(&TopPoints,&BottomPoints);

			if (++j > MAX_WAKE_POINTS_M1)
			{
				j = 0;
				wv = &wd->Vs[0];
				wp = &wd->WakePoints[0];
			}
			else
			{
				wv++;
				wp++;
			}

			if (wp->Flags & WP_IS_VALID)
			{
				MakeBottomPoints(wp,*wv);
				if (last_was_valid)
					DrawWakeSection();
				last_was_valid = TRUE;
			}
			else
				last_was_valid = FALSE;

		}

	}
}


//*************************************************************************************************
float VDetermineNextWayPoint(RoutePoint	 *currway, int numwayrem, int reverse, RoutePoint *startway, FPoint *newway, int firstwaypoint, MovingVehicleParams *vehiclepnt, int ignoreform)
{
	float dx, dz;
	float dx2, dz2;
	RoutePoint *nextway = NULL;
	RoutePoint *thirdway = NULL;
	float heading;
	float nextheading, dheading;
	float workheading;
	float workx, workz;
	double radhead;
	float plusz;
	int useform = 0;
	double offsetx;

	if(!currway)
	{
		if(vehiclepnt)
		{
			newway->X = vehiclepnt->WorldPosition.X;
			newway->Y = vehiclepnt->WorldPosition.X;
			newway->Z = vehiclepnt->WorldPosition.X;
		}
		else
		{
			newway->X = 0;
			newway->Y = 0;
			newway->Z = 0;
		}
		return(0);
	}

	if(vehiclepnt)
	{
		if(vehiclepnt->iShipType)
		{
			if(!ignoreform)
			{
				useform = 1;
				offsetx = vehiclepnt->FormationOffset.X;
			}
			else
			{
				offsetx = 0;
			}
		}
		else if(vehiclepnt->FormationOffset.X)
		{
			useform = 1;
			offsetx = vehiclepnt->FormationOffset.X + LANE_MIDDLE;
		}
		else
		{
			offsetx = LANE_MIDDLE;
		}
	}
	else
	{
		offsetx = LANE_MIDDLE;
	}

	if(reverse)
	{
		if(currway > startway)
		{
			nextway = currway - 1;
			if(nextway > startway)
			{
				thirdway = nextway - 1;
			}
		}
	}
	else
	{
		if(numwayrem > 1)
		{
			nextway = currway + 1;
			if(numwayrem > 2)
			{
				thirdway = nextway + 1;
			}
		}
	}

	if(nextway == NULL)
	{
		newway->X = currway->lX * FTTOWU;
		newway->Y = currway->lY * FTTOWU;
		newway->Z = currway->lZ * FTTOWU;
		return(0);
	}

	dx = nextway->lX - currway->lX;
	dz = nextway->lZ - currway->lZ;

	heading = atan2(-dx, -dz) * 57.2958;

	while(heading > 180)
		heading -= 360;

	while(heading < -180)
		heading += 360;

	if((thirdway == NULL) || (firstwaypoint))
	{
		workx = nextway->lX;
		workz = nextway->lZ;
		workheading = heading - 90;

		while(workheading > 180)
			workheading -= 360;

		while(workheading < -180)
			workheading += 360;

		radhead = DegToRad(workheading);

		workx -= (sin(radhead) * offsetx);  //  LANE_MIDDLE
		workz -= (cos(radhead) * offsetx);	//  LAND_MIDDLE

		newway->X = workx * FTTOWU;
		newway->Y = nextway->lY * FTTOWU;
		newway->Z = workz * FTTOWU;

		return(heading);
	}

	dx2 = thirdway->lX - nextway->lX;
	dz2 = thirdway->lZ - nextway->lZ;

	nextheading = atan2(-dx2, -dz2) * 57.2958;

	while(nextheading > 180)
		nextheading -= 360;

	while(nextheading < -180)
		nextheading += 360;

	dheading = nextheading - heading;

	while(dheading > 180)
		dheading -= 360;

	while(dheading < -180)
		dheading += 360;

	dheading /= 2;

	workx = nextway->lX;
	workz = nextway->lZ;

	if(fabs(dheading) > 70)
	{
		workheading = heading + 90;
		dheading = 0;
	}
	else
	{
		workheading = heading - 90;
	}

	while(workheading > 180)
		workheading -= 360;

	while(workheading < -180)
		workheading += 360;

	radhead = DegToRad(workheading);

	workx -= (sin(radhead) * offsetx);		//  LANE_MIDDLE
	workz -= (cos(radhead) * offsetx);		//  LANE_MIDDLE

	plusz = (offsetx * tan(DegToRad(dheading)));	//  LAND_MIDDLE * tan

	radhead = DegToRad(heading);

	workx -= (sin(radhead) * plusz);
	workz -= (cos(radhead) * plusz);

	newway->X = workx * FTTOWU;
	newway->Y = thirdway->lY * FTTOWU;
	newway->Z = workz * FTTOWU;

	return(heading);
}

//*************************************************************************************************
void VNextWayPoint(MovingVehicleParams *vehiclepnt, float remaingdist)
{
	float heading;
	float workx, workz;
	double radhead;
	MovingVehicleParams *trail;
	FPointDouble tempfpdouble;
	DBVehicleType *pvehicletype=NULL;
	RoutePoint	 *pLastWP = vehiclepnt->pCurrWP;
	double angxx, angxz, angzx, angzz;
	double dx, dz;
	float formationx, formationz;

	vehiclepnt->Status &= ~(VL_DO_NEXT_WAY);

	if(vehiclepnt->Status & VL_INVISIBLE)
	{
		vehiclepnt->Status &= ~(VL_INVISIBLE);
		trail = (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
		if(trail)
		{
			trail->Status |= (VL_AVOID_JAM);
			trail->Status &= ~(VL_MOVING);
		}
	}

	if(vehiclepnt->Status & VL_FIRE_WEAPONS)
	{
		VClearRadardat(vehiclepnt);
		vehiclepnt->Status &= ~(VL_FIRE_WEAPONS|VL_SURFACE_ATTACK);
	}

	if(vehiclepnt->numwaypts <= 1)
	{
		if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_SETFLAG)
		{
			AIChangeEventFlag(vehiclepnt->pCurrWP->iUserFlag);
		}
	}

	if((vehiclepnt->numwaypts > 1) && (!(vehiclepnt->Status & (VL_REVERSE))))
	{
		heading = VDetermineNextWayPoint(vehiclepnt->pCurrWP, vehiclepnt->numwaypts, (vehiclepnt->Status & VL_REVERSE), vehiclepnt->pStartWP, &vehiclepnt->WayPosition, 0, vehiclepnt);
		vehiclepnt->numwaypts = vehiclepnt->numwaypts - 1;
		vehiclepnt->pCurrWP = vehiclepnt->pCurrWP + 1;
		if(!vehiclepnt->iShipType)
		{
			vehiclepnt->Heading = AIConvert180DegreeToAngle(heading);
			vehiclepnt->Pitch = 0;
  			vehiclepnt->Attitude.SetHPR (vehiclepnt->Heading, vehiclepnt->Pitch, 0);
		}
	}
	else if((vehiclepnt->Status & (VL_REVERSE)) && (vehiclepnt->pCurrWP > vehiclepnt->pStartWP))
	{
		heading = VDetermineNextWayPoint(vehiclepnt->pCurrWP, vehiclepnt->numwaypts, (vehiclepnt->Status & VL_REVERSE), vehiclepnt->pStartWP, &vehiclepnt->WayPosition, 0, vehiclepnt);
		vehiclepnt->numwaypts = vehiclepnt->numwaypts + 1;
		vehiclepnt->pCurrWP = vehiclepnt->pCurrWP - 1;
		if(!vehiclepnt->iShipType)
		{
			vehiclepnt->Heading = AIConvert180DegreeToAngle(heading);
			vehiclepnt->Pitch = 0;
  			vehiclepnt->Attitude.SetHPR (vehiclepnt->Heading, vehiclepnt->Pitch, 0);
		}
	}
	else if(vehiclepnt->Status & (VL_REVERSE))
	{
		vehiclepnt->pCurrWP = vehiclepnt->pStartWP;
		vehiclepnt->numwaypts = g_pMovingVehicleList[vehiclepnt->iVListID].iNumPoints;
		vehiclepnt->Status &= ~(VL_REVERSE);

		heading = VDetermineNextWayPoint(vehiclepnt->pCurrWP, vehiclepnt->numwaypts, (vehiclepnt->Status & VL_REVERSE), vehiclepnt->pStartWP, &vehiclepnt->WayPosition, 1, vehiclepnt);
		workx = vehiclepnt->pCurrWP->lX;
		workz = vehiclepnt->pCurrWP->lZ;
		heading = heading - 90;

		while(heading > 180)
			heading -= 360;

		while(heading < -180)
			heading += 360;

		radhead = DegToRad(heading);

		workx -= (sin(radhead) * LANE_MIDDLE);
		workz -= (cos(radhead) * LANE_MIDDLE);

		vehiclepnt->WayPosition.X = workx * FTTOWU;
		vehiclepnt->WayPosition.Y = vehiclepnt->pCurrWP->lY * FTTOWU;
		vehiclepnt->WayPosition.Z = workz * FTTOWU;

		workx = vehiclepnt->WayPosition.X - vehiclepnt->WorldPosition.X;
		workz = vehiclepnt->WayPosition.Z - vehiclepnt->WorldPosition.Z;

		heading = atan2(-workx, -workz) * 57.2958;

		if(!vehiclepnt->iShipType)
		{
			vehiclepnt->Heading = AIConvert180DegreeToAngle(heading);
			vehiclepnt->Pitch = 0;
  			vehiclepnt->Attitude.SetHPR (vehiclepnt->Heading, vehiclepnt->Pitch, 0);
		}
	}
	else if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_LOOP)
	{
		vehiclepnt->pCurrWP = vehiclepnt->pStartWP;
		vehiclepnt->numwaypts = g_pMovingVehicleList[vehiclepnt->iVListID].iNumPoints;
		vehiclepnt->Status &= ~(VL_REVERSE);

		heading = VDetermineNextWayPoint(vehiclepnt->pCurrWP, vehiclepnt->numwaypts, (vehiclepnt->Status & VL_REVERSE), vehiclepnt->pStartWP, &vehiclepnt->WayPosition, 1, vehiclepnt);
		workx = vehiclepnt->pCurrWP->lX;
		workz = vehiclepnt->pCurrWP->lZ;
		heading = heading - 90;

		while(heading > 180)
			heading -= 360;

		while(heading < -180)
			heading += 360;

		radhead = DegToRad(heading);

		workx -= (sin(radhead) * LANE_MIDDLE);
		workz -= (cos(radhead) * LANE_MIDDLE);

		vehiclepnt->WayPosition.X = workx * FTTOWU;
		vehiclepnt->WayPosition.Y = vehiclepnt->pCurrWP->lY * FTTOWU;
		vehiclepnt->WayPosition.Z = workz * FTTOWU;

		workx = vehiclepnt->WayPosition.X - vehiclepnt->WorldPosition.X;
		workz = vehiclepnt->WayPosition.Z - vehiclepnt->WorldPosition.Z;

		heading = atan2(-workx, -workz) * 57.2958;

		if(!vehiclepnt->iShipType)
		{
			vehiclepnt->Heading = AIConvert180DegreeToAngle(heading);
			vehiclepnt->Pitch = 0;
  			vehiclepnt->Attitude.SetHPR (vehiclepnt->Heading, vehiclepnt->Pitch, 0);
		}
	}
	else if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_TURNAROUND)
	{
		vehiclepnt->Status |= (VL_REVERSE);

		heading = VDetermineNextWayPoint(vehiclepnt->pCurrWP, vehiclepnt->numwaypts, (vehiclepnt->Status & VL_REVERSE), vehiclepnt->pStartWP, &vehiclepnt->WayPosition, 1, vehiclepnt);
		workx = vehiclepnt->pCurrWP->lX;
		workz = vehiclepnt->pCurrWP->lZ;
		heading = heading - 90;

		while(heading > 180)
			heading -= 360;

		while(heading < -180)
			heading += 360;

		radhead = DegToRad(heading);

		workx -= (sin(radhead) * LANE_MIDDLE);
		workz -= (cos(radhead) * LANE_MIDDLE);

		vehiclepnt->WayPosition.X = workx * FTTOWU;
		vehiclepnt->WayPosition.Y = vehiclepnt->pCurrWP->lY * FTTOWU;
		vehiclepnt->WayPosition.Z = workz * FTTOWU;

		workx = vehiclepnt->WayPosition.X - vehiclepnt->WorldPosition.X;
		workz = vehiclepnt->WayPosition.Z - vehiclepnt->WorldPosition.Z;

		heading = atan2(-workx, -workz) * 57.2958;

		if(!vehiclepnt->iShipType)
		{
			vehiclepnt->Heading = AIConvert180DegreeToAngle(heading);
			vehiclepnt->Pitch = 0;
  			vehiclepnt->Attitude.SetHPR (vehiclepnt->Heading, vehiclepnt->Pitch, 0);
		}
	}
	else
	{
		vehiclepnt->Status |= VL_DONE_MOVING;
		vehiclepnt->Status &= ~(VL_MOVING);
		if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_EXPLODE)
		{
			trail = vehiclepnt;
			while(trail->pLeadVehicle)
			{
				trail = (MovingVehicleParams *)trail->pLeadVehicle;
			}
			while(trail)
			{
				// SCOTT FIX THIS

				if (!trail->iShipType)
					pvehicletype = &pDBVehicleList[trail->iVDBIndex];

				DestroyVehicle(trail,TRUE);

				trail = (MovingVehicleParams *)trail->pTrailVehicle;
			}
		}
		else if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_STOPDIS)
		{
			vehiclepnt->Status |= VL_INVISIBLE;
		}
	}

	if(vehiclepnt->WayPosition.Y < 0)
	{
		vehiclepnt->WayPosition.Y = 0;
	}

	if(!(vehiclepnt->Status & VL_DONE_MOVING))
	{
		tempfpdouble = vehiclepnt->WayPosition;
		vehiclepnt->fSinPitch = VGetSinPitchFromWayPoint(tempfpdouble, vehiclepnt->WorldPosition);
	}

	if(vehiclepnt->pLeadVehicle)
	{
		vehiclepnt->Status |= VL_CHECK_LEAD;
	}

	dx = vehiclepnt->pCurrWP->lX - pLastWP->lX;
	dz = vehiclepnt->pCurrWP->lZ - pLastWP->lZ;

	heading = atan2(-dx, -dz) * 57.2958;

	heading = DegToRad(heading);

	angzx = sin(heading);
	angzz = cos(heading);
	angxx = angzz;
	angxz = angzx;

	formationx = vehiclepnt->FormationOffset.X * FTTOWU;
	formationz = vehiclepnt->FormationOffset.Z * FTTOWU;

#if 1
	if((vehiclepnt->numwaypts <= 1) && (vehiclepnt->pCurrWP->iEndOfPathOptions & (STOP_FLAG_STOP)))
	{
		vehiclepnt->WayPosition.X += (angxx * formationx) + (angzx * formationz);
		vehiclepnt->WayPosition.Z += (-(angxz * formationx) + (angzz * formationz));
	}
	else
	{
		vehiclepnt->WayPosition.X += (angxx * formationx);  //   + (angzx * formationz);
		vehiclepnt->WayPosition.Z += (-(angxz * formationx)); //   + (angzz * formationz);
	}
#else
	if((vehiclepnt->numwaypts <= 1) && (vehiclepnt->pCurrWP->iEndOfPathOptions & (STOP_FLAG_STOP)))
	{
		vehiclepnt->WayPosition.X += (angzx * formationz);
		vehiclepnt->WayPosition.Z += (angzz * formationz);
	}
#endif

#if 1  //  Not sure what I need to check here.  SRE
	int cnt;

	if(vehiclepnt->pCurrWP->iRouteActionFlags & (VEHICLE_ACTION_ATTACKALONGPATH|VEHICLE_ACTION_ATTACKSPECIFIC|VEHICLE_ACTION_ATTACKAROUNDSPECIFIC))
	{
		vehiclepnt->Status |= VL_SURFACE_ATTACK;

		if(vehiclepnt->iShipType)
		{
			for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
			{
				if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
						(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
						((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
				{
					vehiclepnt->Status |= VL_SURFACE_ATTACK;

					if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
						vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
				}
			}
		}
		else
		{
			for(cnt = 0; cnt < 2; cnt ++)
			{
				if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
						(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
				  		((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
				{
					vehiclepnt->Status |= VL_SURFACE_ATTACK;
					if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
						vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
				}
			}
		}
	}
	else
	{
		vehiclepnt->Status &= ~VL_SURFACE_ATTACK;
	}
#else
	int cnt;

	vehiclepnt->Status |= VL_SURFACE_ATTACK;

	if(vehiclepnt->iShipType)
	{
		for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
		{
			if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
					(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
					((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
			{
				vehiclepnt->Status |= VL_SURFACE_ATTACK;

				if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
					vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
			}
		}
	}
	else
	{
		for(cnt = 0; cnt < 2; cnt ++)
		{
			if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
					(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
					((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
			{
				vehiclepnt->Status |= VL_SURFACE_ATTACK;
				if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
					vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
			}
		}
	}

#endif
}

//*************************************************************************************************
void FallAVehicle(MovingVehicleParams *vehiclepnt)
{
	float distmove;
	float loops;

	loops = (GameLoop - vehiclepnt->LastDamagedTick)*(4096.0f/50.0f);

	if (vehiclepnt->Status & VL_FALLING_LEFT)
		vehiclepnt->Attitude.SetRPH(loops,vehiclepnt->Pitch - 2.0f*loops,vehiclepnt->Heading);
	else
		vehiclepnt->Attitude.SetRPH(loops,vehiclepnt->Pitch - 2.0f*loops,vehiclepnt->Heading);

	loops =(float)DeltaTicks * (1.0f / 1000.0f);

	distmove = vehiclepnt->fSpeedFPS * loops * FTTOWU;

	vehiclepnt->WorldPosition.X -= FSin(vehiclepnt->Heading) * distmove;
	vehiclepnt->WorldPosition.Z -= FCos(vehiclepnt->Heading) * distmove;
	vehiclepnt->WorldPosition.Y -= (double)(vehiclepnt->fSinPitch + 16.0*loops) * loops;

	vehiclepnt->fSinPitch += 32.0 * loops;

	if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
	{
		loops = LandHeight(vehiclepnt->WorldPosition.X, vehiclepnt->WorldPosition.Z);
		if (vehiclepnt->WorldPosition.Y <= loops)
		{

			vehiclepnt->WorldPosition.Y = loops;

			vehiclepnt->Status &= ~(VL_FALLING | VL_MOVING);
			vehiclepnt->Status |= VL_DONE_MOVING;

			if (!(vehiclepnt->Status & VL_DESTROYED))
				DestroyVehicle(vehiclepnt,TRUE);
		}
	}
	else
	{
		vehiclepnt->Status &= ~(VL_FALLING | VL_MOVING);
		vehiclepnt->Status |= VL_DONE_MOVING;

		if (!(vehiclepnt->Status & VL_DESTROYED))
			DestroyVehicle(vehiclepnt,TRUE);
	}


//	if (vehiclepnt->Smoke)
//		UpdateCanisterPos( vehiclepnt->Smoke, vehiclepnt->WorldPosition);

}

void FallMe(int vnum,MovingVehicleParams *vehiclepnt)
{
	vehiclepnt->Status &= ~(VL_ON_BRIDGE | VL_DONE_MOVING);

	vehiclepnt->Bridge = NULL;

	if ((vnum+GameLoop)&1)
		vehiclepnt->Status |= VL_FALLING_LEFT;
	else
		vehiclepnt->Status |= VL_FALLING_RIGHT;

	vehiclepnt->LastDamagedTick = GameLoop - (int)(DeltaTicks / 50);
	vehiclepnt->fSinPitch = 0.0f;

	FallAVehicle(vehiclepnt);

}

double max_roll = 0.25; // max of 6.38/2.0;
double max_pitch = 0.65; // max of 1.8/2.0;

void CalculateRollPitchandHeight(MovingVehicleParams *vehiclepnt,FPointDouble &WorldPosition,DWORD game_loop,double &roll,double &pitch,double &depression)
{
	double final_value;
	double final_height;
	double accounting;
	double t,x,z;
	double scale;

	/* -----------------4/29/99 4:13PM---------------------------------------------------------------------
	 * first thing we do is get a pitch value based on the location of the ship and the time.  This allows
	 * ships in a line to appear to be getting the same waves.
	 * ----------------------------------------------------------------------------------------------------*/

	t = (double)(game_loop%1200)*(PId*2.0/1200.0);
	x = fmod(WorldPosition.X,1.8f*TILE_SIZE)*(PId*2.0/(1.8f*TILE_SIZE));
	z = fmod(WorldPosition.Z,1.8f*TILE_SIZE)*(PId*2.0/(1.8f*TILE_SIZE));


	final_value = 0.0;
	accounting=0.0;

	switch(vehiclepnt->Type->iSizeType)
	{
		case SHIP_SIZE_HUGE:
			scale = 1.0;
			break;

		case SHIP_SIZE_LARGE:
			scale = 0.7;
			break;

		case SHIP_SIZE_MEDIUM:
			scale = 0.5;
			break;

		case SHIP_SIZE_SMALL:
			scale = 0.3;
			break;

		case SHIP_SIZE_VSMALL:
			scale = 0.2;
			break;

		default:
			scale = 1.0;
			break;

	}

	final_value = (sin(t*2)+sin(x*2)+sin(t*2)) * (1.0/3.0);
	final_height = (cos(t*2)-1.0)*(0.5/3.0);
	final_height += (cos(x*2)-1.0)*(0.5/3.0);
	final_height += (cos(z*2)-1.0)*(0.5/3.0);
	accounting = 1.0;

	roll = (final_value*accounting*max_roll*(PId/180.0)) + DegToRad(vehiclepnt->listroll); //15 degrees
	depression = final_height*sin(max_roll*(PId/180.0))*(24.0 * scale);


	t = (double)(game_loop%1900)*(PId*2.0/1900.0);
	x = fmod(WorldPosition.X,2.1f*TILE_SIZE)*(PId*2.0/(2.1f*TILE_SIZE));
	z = fmod(WorldPosition.Z,2.1f*TILE_SIZE)*(PId*2.0/(2.1f*TILE_SIZE));

	final_value = 0.0;
	accounting=0.0;

	final_value = (sin(t)+sin(x)+sin(t)) * (1.0/3.0);
	final_height = (cos(t)-1.0)*(0.5/3.0);
	final_height += (cos(x)-1.0)*(0.5/3.0);
	final_height += (cos(z)-1.0)*(0.5/3.0);
	accounting = 1.0;

	pitch = final_value*accounting*max_pitch*(PId/180.0); //15 degrees
	depression += final_height*sin(max_pitch*(PId/180.0))*(96.0*scale);
}


void CalculateWaveAttitude(MovingVehicleParams *vehiclepnt)
{
	double depression;
	double roll;
	double pitch;

	CalculateRollPitchandHeight(vehiclepnt,vehiclepnt->WorldPosition,GameLoop,roll,pitch,depression);

	vehiclepnt->Attitude.SetRadRPH((float)roll,(float)pitch,(float)ANGLE_TO_RADIANS(vehiclepnt->Heading));
	vehiclepnt->WorldPosition.Y += depression;
}

BOOL CalculateAttitudeAndHeight(MovingVehicleParams *vehiclepnt,int vnum)
{
	DWORD old_status;
	double land_y;
	ANGLE Roll;
	BOOL ret_val = TRUE;
	FPoint normal;

	if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
	{
		if (!(vehiclepnt->Status & VL_IN_VISUAL_RANGE))
			vehiclepnt->Status |= (VL_IN_VISUAL_RANGE | VL_BRIDGE_TARE);

		old_status = vehiclepnt->Status & VL_ON_BRIDGE;

		if (PointOnAnyBridge(vehiclepnt->Heading,(DWORD *)&vehiclepnt->Status,&vehiclepnt->Bridge,vehiclepnt->WorldPosition,normal))
			vehiclepnt->Attitude.SetFromNormalAndHeading(vehiclepnt->Heading,normal);
		else
		{
			land_y = LandHeight(vehiclepnt->WorldPosition.X, vehiclepnt->WorldPosition.Z);

			if(!vehiclepnt->iShipType)
			{
				DBVehicleType *pvehicletype=NULL;

				pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];

				if(pvehicletype->lVehicleID == 43)
				{
					if(!InWater(vehiclepnt->WorldPosition.X, vehiclepnt->WorldPosition.Z))
					{
						land_y += (2.5f * FTTOWU);
						vehiclepnt->lAIFlags1 &= ~(V_NO_SHADOW);
					}
					else
					{
						land_y -= (2.0f * FTTOWU);
						vehiclepnt->lAIFlags1 |= V_NO_SHADOW;
					}
				}
			}

			if (old_status && (fabs(land_y - vehiclepnt->WorldPosition.Y) > (15 FEET)))
				FallMe(vnum,vehiclepnt);
			else
			{
				/* -----------------4/29/99 11:46AM--------------------------------------------------------------------
				 * if we're a ship, we have to do some work here
				 * ----------------------------------------------------------------------------------------------------*/
				vehiclepnt->WorldPosition.Y = land_y;

				if (vehiclepnt->iShipType)
					CalculateWaveAttitude(vehiclepnt);//->WorldPosition,vehiclepnt->Attitude);
				else
					GetMatrix(vehiclepnt->WorldPosition, vehiclepnt->Heading, vehiclepnt->Attitude);
			}
		}
		vehiclepnt->Attitude.GetRPH(&Roll,&vehiclepnt->Pitch,&vehiclepnt->Heading);
	}
	else
	{
		vehiclepnt->Status &= ~VL_IN_VISUAL_RANGE;
		ret_val = FALSE;
	}

	return ret_val;

}

void VMoveVehicles()
{
	float dx, dz;
	float tdist, heading;
	float workvalx, workvalz;
	float distmove;
	int done;
	double radhead;
	MovingVehicleParams *vehiclepnt, *leadpnt, *trailpnt;
//	ANGLE Roll;
	int vnum;
	DWORD old_status;
//	double land_y;
	ANGLE orgheading;
	DBShipType *pshiptype;
	DBShipType *pleadshiptype;
	int shipturn;
	float secdist;
	RoutePoint	 *pNextWP;
	ANGLE tempheading;
	float offangle;
	float maxturn;
	float turnmod = 1.0f;
	float vehiclespacing;
	float fworkvar, fworkvar2;
	float maxadjust = 10.0f;
	float speedadjust = 1.0f;
	float workangle;
	float correctangle;
	int cnt;
	float fworkval;
	float fdeltamax;
	char cworkval, cworkval2;
	float listval;
	FPoint netfpoint;
	int leadstop;
	DBVehicleType *pvehicletype=NULL;
	PlaneParams *planepnt;
	int didnextway;


	for(vnum = 0, vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++, vnum++)
	{
		didnextway = 0;
		done = 0;
		correctangle = 0;
		if (vehiclepnt->Status & VL_ACTIVE)
		{
			if (vehiclepnt->iShipType)
			{
				if(vehiclepnt->lAIFlags1 & (V_ON_FIRE))
				{
					for(cnt = 0; cnt < MAX_VEHICLE_SMOKE_SLOTS; cnt ++)
					{
						if(vehiclepnt->Smoke[cnt])
							break;
					}

					if(cnt == MAX_VEHICLE_SMOKE_SLOTS)
					{
						vehiclepnt->lAIFlags1 &= ~(V_ON_FIRE);
						if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
						{
							if((lCarrierMsgFlags & (MOTHER_SICK_MSG)) && (vehiclepnt == &MovingVehicles[PlayerPlane->AI.iHomeBaseId]))
							{
								lCarrierMsgFlags &= ~(MOTHER_SICK_MSG);
								AICJustMessage(AIC_STRIKE_GREEN_DECK, AICF_STRIKE_GREEN_DECK, SPCH_STRIKE);
							}
						}
					}
				}

				if (vehiclepnt->bDamageLevel)
				{
					pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];

					switch(vehiclepnt->bDamageLevel)
					{
						case 0:
							listval = 0;
							break;
						case 1:
							listval = 3.0f + (0.5 * (vnum & 0x3));
							break;
						case 2:
							if(pshiptype->lShipType & SHIP_TYPE_CARRIER)
							{
								listval = 4.0f + (0.5 * (vnum & 0x3));
							}
							else
							{
								listval = 5.5f + (0.5 * (vnum & 0x3));
							}
							break;

						case 3:
						default:
							if(pshiptype->lShipType & SHIP_TYPE_CARRIER)
							{
								listval = 4.0f + (0.5 * (vnum & 0x3));
							}
							else
							{
								listval = 8.0f + (0.5 * (vnum & 0x3));
							}
							break;
					}

					fworkval = DeltaTicks / (60000.0f);

					if(vnum & 1)
					{
						listval = -listval;

						if(vehiclepnt->listroll > listval)
						{
							vehiclepnt->listroll -= fworkval;
						}

						if(vehiclepnt->listroll < listval)
						{
							vehiclepnt->listroll = listval;
						}
					}
					else
					{
						if(vehiclepnt->listroll < listval)
						{
							vehiclepnt->listroll += fworkval;
						}

						if(vehiclepnt->listroll > listval)
						{
							vehiclepnt->listroll = listval;
						}
					}
				}
			}
			else
				if(!vehiclepnt->pLeadVehicle) //only gets here if not a ship
				{
					if(!(vehiclepnt->lAIFlags1 & SAR_CHECKED))
					{
						pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];

						if(pvehicletype->lVehicleID == 43)
						{
							AICheckForVehicleSAR(vehiclepnt);
						}
						vehiclepnt->lAIFlags1 |= SAR_CHECKED;
						trailpnt = (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
						while(trailpnt)
						{
							trailpnt->lAIFlags1 |= SAR_CHECKED;
							trailpnt = (MovingVehicleParams *)trailpnt->pTrailVehicle;
						}
					}
				}


			orgheading = vehiclepnt->Heading;
			if((vehiclepnt->Status & VL_FALLING) && !(vehiclepnt->Status & VL_DONE_MOVING))
				FallAVehicle(vehiclepnt);
			else
			{
				if(vehiclepnt->lAITimer1 >= 0)
					vehiclepnt->lAITimer1 -= DeltaTicks;

				if(vehiclepnt->Status & VL_AVOID_JAM)
				{
					VCheckForTrafficJam(vehiclepnt);
				}

				if(vehiclepnt->pLeadVehicle)
				{
					VCheckLeader(vehiclepnt, (MovingVehicleParams *)vehiclepnt->pLeadVehicle);
				}

				for(cnt = 0; cnt < MAX_VEHICLE_FX; cnt ++)
				{
					if(vehiclepnt->bFXArray[cnt] != vehiclepnt->bFXDesiredArray[cnt])
					{
						if(cnt > 1)
						{
							fworkval = vehiclepnt->bFXDesiredArray[cnt] - vehiclepnt->bFXArray[cnt];
							fdeltamax = ((float)DeltaTicks / 1000.0f) * (255.0f / 4.0f);
							if(fabs(fworkval) > fdeltamax)
							{
								cworkval = fdeltamax;
								if(!cworkval)
								{
									cworkval = 1;
								}
								if(fworkval < 0)
								{
									vehiclepnt->bFXArray[cnt] = vehiclepnt->bFXArray[cnt] - cworkval;
								}
								else
								{
									vehiclepnt->bFXArray[cnt] = vehiclepnt->bFXArray[cnt] + cworkval;
								}
							}
							else
							{
								vehiclepnt->bFXArray[cnt] = vehiclepnt->bFXDesiredArray[cnt];
							}
						}
						else
						{
							cworkval = cworkval2 = vehiclepnt->bFXDesiredArray[cnt] - vehiclepnt->bFXArray[cnt];
							if(cworkval > 128)
							{
								cworkval2 = vehiclepnt->bFXArray[cnt] - vehiclepnt->bFXDesiredArray[cnt];
							}

							fworkval = cworkval2;

							fdeltamax = ((float)DeltaTicks / 1000.0f) * (255.0f / 7.0f);
							if(fabs(fworkval) > fdeltamax)
							{
								cworkval2 = fdeltamax;
								if(!cworkval2)
								{
									cworkval2 = 1;
								}
								if(cworkval <= 128)
								{
									vehiclepnt->bFXArray[cnt] = vehiclepnt->bFXArray[cnt] + cworkval2;
								}
								else
								{
									vehiclepnt->bFXArray[cnt] = vehiclepnt->bFXArray[cnt] - cworkval2;
								}
							}
							else
							{
								vehiclepnt->bFXArray[cnt] = vehiclepnt->bFXDesiredArray[cnt];
							}
						}
					}
				}

				if(vehiclepnt->Status & VL_MOVING)
				{
					leadpnt = vehiclepnt;

					while((leadpnt->pLeadVehicle) && (leadpnt != leadpnt->pLeadVehicle))
						leadpnt = (MovingVehicleParams *)leadpnt->pLeadVehicle;

					if(leadpnt == leadpnt->pLeadVehicle)
					{
						leadpnt->pLeadVehicle = NULL;
					}

					leadstop = 0;
					if((leadpnt->lAIFlags1 & V_TEMP_STOP) && (leadpnt != vehiclepnt))
					{
						leadstop = 1;
					}

					if(leadpnt->iShipType)
					{
					 	pleadshiptype = &pDBShipList[leadpnt->iVDBIndex];
						turnmod = 0.125f;
					}

					if(vehiclepnt->iShipType)
					{
						VGDCheckDefenses(vehiclepnt);
					}
					else if(vehiclepnt->Status & VL_SURFACE_ATTACK)
					{
						VGDCheckDefenses(vehiclepnt);
					}
					else if(vehiclepnt->lAIFlags1 & (V_JUST_TO_WAY))
					{
						if(vehiclepnt->RadarWeaponData[3].Target)
						{
							planepnt = (PlaneParams *)vehiclepnt->RadarWeaponData[3].Target;
							vehiclepnt->WayPosition = planepnt->WorldPosition;

							if(planepnt->AI.Behaviorfunc != AISARPickUp)
							{
								vehiclepnt->Status |= (VL_DONE_MOVING|VL_INVISIBLE);
								vehiclepnt->Status &= ~(VL_MOVING);
							}
						}
					}

					vehiclepnt->Status &= ~VL_TURNING;
					speedadjust = 1.0f;

					distmove = vehiclepnt->fSpeedFPS * ((float)DeltaTicks / 1000.0f) * FTTOWU;
					if(!distmove)
					{
						CalculateAttitudeAndHeight(vehiclepnt,vnum);
					}

					while(distmove)
					{
						dx = vehiclepnt->WayPosition.X - vehiclepnt->WorldPosition.X;
						dz = vehiclepnt->WayPosition.Z - vehiclepnt->WorldPosition.Z;

						tdist = sqrt((dx*dx) + (dz*dz));

						shipturn = 0;
						if(vehiclepnt->iShipType)
						{
							secdist = (tdist * WUTOFT) / vehiclepnt->fSpeedFPS;
							pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
							if(!pleadshiptype)
							{
								pleadshiptype = pshiptype;
							}
							if(secdist > (360.0f / ((float)pleadshiptype->iMaxTurnRate * turnmod)))
							{
								shipturn = 0;
							}
							else
							{
								if((vehiclepnt->numwaypts > 1) && (!(vehiclepnt->Status & (VL_REVERSE))))
									pNextWP = vehiclepnt->pCurrWP + 1;
								else if((vehiclepnt->Status & (VL_REVERSE)) && (vehiclepnt->pCurrWP > vehiclepnt->pStartWP))
									pNextWP = vehiclepnt->pCurrWP - 1;
								else if(vehiclepnt->Status & (VL_REVERSE))
									pNextWP = vehiclepnt->pStartWP;
								else if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_LOOP)
									pNextWP = vehiclepnt->pStartWP;
								else if(vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_TURNAROUND)
								{
									pNextWP = vehiclepnt->pCurrWP;
								}
								else
								{
									shipturn = 0;
									pNextWP = vehiclepnt->pCurrWP;
								}

								if((pNextWP == vehiclepnt->pCurrWP) && (!(pNextWP->iEndOfPathOptions & STOP_FLAG_STOP)))
								{
									if(secdist < (90.0f / ((float)pleadshiptype->iMaxTurnRate * turnmod)))
									{
										shipturn = 1;
									}
								}
								else if(pNextWP)
								{
									workvalx = (pNextWP->lX * FTTOWU) - vehiclepnt->WorldPosition.X;
									workvalz = (pNextWP->lZ * FTTOWU) - vehiclepnt->WorldPosition.Z;

									heading = atan2(-workvalx, -workvalz) * 57.2958;

									while(heading > 180)
										heading -= 360;

									while(heading < -180)
										heading += 360;

									tempheading = AIConvert180DegreeToAngle(heading);
									tempheading -= vehiclepnt->Heading;

									offangle = AIConvertAngleTo180Degree(tempheading);
//									maxturn = (float)pshiptype->iMaxTurnRate * 2.0f;
									if(pNextWP == vehiclepnt->pCurrWP)
									{
										maxturn = (float)pleadshiptype->iMaxTurnRate * turnmod;
									}
									else
									{
										maxturn = (float)pleadshiptype->iMaxTurnRate * 2.0f * turnmod;
									}

									if((fabs(offangle) / maxturn) > secdist)
									{
										shipturn = 1;
									}
								}
								else
								{
									OutputDebugString("Scott pNextWP is NULL");
								}
							}
						}
						else
						{
							shipturn = 0;
						}

						if((shipturn) && (!didnextway))
						{
							if (!(vehiclepnt->Status & VL_FALLING))
							{
								VCheckWayPointAction(vehiclepnt);

								if((vehiclepnt->Status & VL_MOVING) && (!didnextway))
								{
									VNextWayPoint(vehiclepnt, tdist);
									if(!(vehiclepnt->Status & (VL_ACTIVE | VL_DESTROYED | VL_DONE_MOVING)))
										distmove = 0;
								}
								else
									distmove = 0;

								CalculateAttitudeAndHeight(vehiclepnt,vnum);

							}
							else
								distmove = 0;

							/* ----------------6/7/99 11:18AM------------------------------------------------------------------
							 * adjust the wake
							 * ------------------------------------------------------------------------------------------------*/

							if (vehiclepnt->iShipType)
							{
								if (!vehiclepnt->WakeData)
									InitWake(vehiclepnt);
								else
									UpdateWake(vehiclepnt);
							}
							didnextway = 1;

						}
						else if(((tdist <= distmove) && (vehiclepnt->lAIFlags1 & V_JUST_TO_WAY)) || (leadstop))
						{
							distmove = 0;

							CalculateAttitudeAndHeight(vehiclepnt,vnum);

							if(vehiclepnt->lAIFlags1 & SAR_CHECKED)
							{
								vehiclepnt->Status |= (VL_DONE_MOVING|VL_INVISIBLE);
								vehiclepnt->Status &= ~(VL_MOVING);
							}
							else if(!leadstop)
							{
								vehiclepnt->lAIFlags1 |= V_TEMP_STOP;
							}
						}
						else if(tdist <= distmove)
						{
							old_status = vehiclepnt->Status & VL_ON_BRIDGE;

							vehiclepnt->WorldPosition.X = (double)vehiclepnt->WayPosition.X;
//							vehiclepnt->WorldPosition.Y = (double)vehiclepnt->WayPosition.Y;
							vehiclepnt->WorldPosition.Z = (double)vehiclepnt->WayPosition.Z;
							distmove -= tdist;
//							vehiclepnt->WorldPosition = vehiclepnt->WayPosition;

							CalculateAttitudeAndHeight(vehiclepnt,vnum);

//							if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
//							{
//								if (!(vehiclepnt->Status & VL_IN_VISUAL_RANGE))
//									vehiclepnt->Status |= (VL_IN_VISUAL_RANGE | VL_BRIDGE_TARE);

//								if (PointOnAnyBridge(vehiclepnt->Heading,(DWORD *)&vehiclepnt->Status,&vehiclepnt->Bridge,vehiclepnt->WorldPosition,normal))
//									vehiclepnt->Attitude.SetFromNormalAndHeading(vehiclepnt->Heading,normal);
//								else
//								{
//									land_y = LandHeight(vehiclepnt->WorldPosition.X, vehiclepnt->WorldPosition.Z);

//									if (old_status && (fabs(land_y - vehiclepnt->WorldPosition.Y) > (15 FEET)))
//										FallMe(vnum,vehiclepnt);
//									else
//									{
//										vehiclepnt->WorldPosition.Y = land_y;
//										GetMatrix(vehiclepnt->WorldPosition, vehiclepnt->Heading, vehiclepnt->Attitude);
//									}
//								}
//								vehiclepnt->Attitude.GetRPH(&Roll,&vehiclepnt->Pitch,&vehiclepnt->Heading);
//							}
//							else
//								vehiclepnt->Status &= ~VL_IN_VISUAL_RANGE;


							if (!(vehiclepnt->Status & VL_FALLING))
							{
								VCheckWayPointAction(vehiclepnt);

								if((vehiclepnt->Status & VL_MOVING) && (!didnextway))
								{
									VNextWayPoint(vehiclepnt, tdist);
									if(!(vehiclepnt->Status & (VL_ACTIVE | VL_DESTROYED | VL_DONE_MOVING)))
									{
										distmove = 0;
									}
								}
								else
								{
									CalculateAttitudeAndHeight(vehiclepnt,vnum);
									distmove = 0;
								}

							}
							else
								distmove = 0;

							didnextway = 1;

							/* ----------------6/7/99 11:18AM------------------------------------------------------------------
							 * Adjust the wake
							 * ------------------------------------------------------------------------------------------------*/

							if (vehiclepnt->iShipType)
							{
								if (!vehiclepnt->WakeData)
									InitWake(vehiclepnt);
								else
									UpdateWake(vehiclepnt);
							}


						}
						else
						{
							heading = atan2(-dx, -dz) * 57.2958;

							if(vehiclepnt->Status & VL_CHECK_LEAD)
							{
								if((leadpnt->pCurrWP == vehiclepnt->pCurrWP) && (!(leadpnt->Status & VL_TURNING)))
								{
									vehiclepnt->Status &= ~VL_CHECK_LEAD;
									FPointDouble relative_position;
									FMatrix de_rotate;

									relative_position.MakeVectorToFrom(vehiclepnt->WorldPosition, leadpnt->WorldPosition);

									de_rotate = leadpnt->Attitude;
									de_rotate.Transpose();

									relative_position *= de_rotate;

									relative_position.X *= WUTOFT;
									relative_position.Z *= WUTOFT;

									vehiclespacing = GetObjectRadius(vehiclepnt->Type->Model) * WUTOFT;
									fworkvar = (relative_position.X - vehiclepnt->FormationOffset.X) / vehiclespacing;

									if(fabs(fworkvar) > 0.5f)
									{
										vehiclepnt->Status |= VL_CHECK_LEAD;
										if(fworkvar > maxadjust)
										{
											fworkvar = maxadjust;
										}
										else if(fworkvar < -maxadjust)
										{
											fworkvar = -maxadjust;
										}

										correctangle = fworkvar;

										heading += fworkvar;
									}

									fworkvar = (relative_position.Z - vehiclepnt->FormationOffset.Z) / vehiclespacing;

									if(fabs(fworkvar) > 0.5f)
									{
										vehiclepnt->Status |= VL_CHECK_LEAD;
										fworkvar = (relative_position.Z - vehiclepnt->FormationOffset.Z) / 15.0f;
										fworkvar2 = fworkvar / vehiclepnt->fSpeedFPS;

										if(fworkvar2 > 0.2f)
										{
											fworkvar2 = 0.2f;
										}
										else if(fworkvar2 < -0.2f)
										{
											fworkvar2 = -0.2f;
										}
										else if(fabs(fworkvar2) < 0.001)
										{
											if(fworkvar2 < 0)
											{
												fworkvar2 = -0.001f;
											}
											else
											{
												fworkvar2 = 0.001f;
											}
										}
										speedadjust = 1.0f + fworkvar2;
									}
								}
							}

							while(heading > 180)
								heading -= 360;

							while(heading < -180)
								heading += 360;

							radhead = DegToRad(heading);

							if(vehiclepnt->iShipType)
							{
								pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
								if(!pleadshiptype)
								{
									pleadshiptype = pshiptype;
								}

								tempheading = RADIANS_TO_ANGLE(radhead);
								tempheading -= vehiclepnt->Heading;

								offangle = AIConvertAngleTo180Degree(tempheading);
								maxturn = (float)pleadshiptype->iMaxTurnRate * ((float)DeltaTicks / 1000.0f) * turnmod;

								if(fabs(offangle) < maxturn)
								{
									vehiclepnt->Heading = RADIANS_TO_ANGLE(radhead);
								}
								else
								{
									if(fabs(offangle) > 2.0f)
									{
										vehiclepnt->Status |= VL_TURNING;
									}

									if((leadpnt != vehiclepnt) && (fabs(fabs(offangle) - fabs(correctangle)) > 2.0f))
									{
										double circledist, leadradius, workradius;

										circledist = (vehiclepnt->fSpeedFPS * (360.0f / ((float)pleadshiptype->iMaxTurnRate * turnmod)));
										leadradius = workradius = circledist / 6.28318530718;

										if(offangle > 0)
										{
											workradius += vehiclepnt->FormationOffset.X;
											workradius -= leadpnt->FormationOffset.X;
										}
										else
										{
											workradius -= vehiclepnt->FormationOffset.X;
											workradius += leadpnt->FormationOffset.X;
										}

										fworkvar = (vehiclepnt->FormationOffset.Z - leadpnt->FormationOffset.Z);

										if(fworkvar)
										{
											workangle = atan2(-workradius, -fworkvar);
											workradius = fabs(fworkvar / cos(workangle));
										}

										if(workradius < (leadradius / 2))
										{
											workradius = leadradius / 2;
										}
										else if(workradius > (leadradius * 2))
										{
											workradius = leadradius * 2;
										}

										distmove *= (workradius / leadradius);
									}

									tempheading = AIConvert180DegreeToAngle(maxturn);
									if(tempheading == 0)  //  Makes sure boat will turn;
									{
										tempheading = 1;
									}

									if(offangle < 0)
									{
										vehiclepnt->Heading -= tempheading;
									}
									else
									{
										vehiclepnt->Heading += tempheading;
									}
									radhead = ANGLE_TO_RADIANS(vehiclepnt->Heading);
								}
							}
							else
							{
								vehiclepnt->Heading = RADIANS_TO_ANGLE(radhead);
							}

							distmove *= speedadjust;

							workvalx = vehiclepnt->WorldPosition.X;
							workvalz = vehiclepnt->WorldPosition.Z;

							workvalx -= (sin(radhead) * distmove);
							workvalz -= (cos(radhead) * distmove);

							vehiclepnt->WorldPosition.X = workvalx;
							vehiclepnt->WorldPosition.Z = workvalz;

							if (!CalculateAttitudeAndHeight(vehiclepnt,vnum))
							{
								tdist -= distmove;
								if (!vehiclepnt->iShipType)
									vehiclepnt->WorldPosition.Y = vehiclepnt->WayPosition.Y + (tdist * vehiclepnt->fSinPitch);
							}

							/* ----------------6/7/99 11:16AM------------------------------------------------------------------
							 * adjust the wake
							 * ------------------------------------------------------------------------------------------------*/

							if (vehiclepnt->iShipType)
							{
								if (!vehiclepnt->WakeData)
									InitWake(vehiclepnt);
								else
									UpdateWake(vehiclepnt);
							}

							distmove = 0;
						}
					}

#if 1 //  WAITING FOR SUBMERGE STUFF TO BE ADDED
					if(vehiclepnt->iShipType)
					{
						pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
						if(pshiptype->lShipType & SHIP_TYPE_SUBMARINE)
						{
							if((vehiclepnt->pCurrWP->iWaitFlags & WAIT_FLAG_SUBMERGE) && (vehiclepnt->bDamageLevel < 2))
							{
								if(vehiclepnt->fSubmergeDepth < (SUBMERGE_DEPTH * FTTOWU))
								{
									vehiclepnt->fSubmergeDepth += (((float)DeltaTicks / 1000.0f) * FTTOWU);
								}
								else
								{
									vehiclepnt->Status |= VL_INVISIBLE;
								}
							}
							else if((vehiclepnt->pCurrWP->iWaitFlags & WAIT_FLAG_SURFACE) || (vehiclepnt->bDamageLevel >= 2))
							{
								vehiclepnt->Status &= ~VL_INVISIBLE;
								if(vehiclepnt->fSubmergeDepth > 0)
								{
									vehiclepnt->fSubmergeDepth -= (((float)DeltaTicks / 1000.0f) * FTTOWU);
								}
								else
								{
									vehiclepnt->fSubmergeDepth = 0;
								}
							}
							vehiclepnt->WorldPosition.Y = vehiclepnt->WorldPosition.Y - vehiclepnt->fSubmergeDepth;
						}
					}
#endif

				}
				else if(vehiclepnt->Status & VL_WAITING)
				{
					VCheckWaiting(vehiclepnt);
					if (!CalculateAttitudeAndHeight(vehiclepnt,vnum))
					{
						tdist -= distmove;
						if (!vehiclepnt->iShipType)
							vehiclepnt->WorldPosition.Y = vehiclepnt->WayPosition.Y + (tdist * vehiclepnt->fSinPitch);
					}

					if(vehiclepnt->iShipType)
					{
						pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
						if(pshiptype->lShipType & SHIP_TYPE_SUBMARINE)
						{
							if((vehiclepnt->pCurrWP->iWaitFlags & WAIT_FLAG_SUBMERGE) && (vehiclepnt->bDamageLevel < 2))
							{
								if(vehiclepnt->fSubmergeDepth < (SUBMERGE_DEPTH * FTTOWU))
								{
									vehiclepnt->fSubmergeDepth += (((float)DeltaTicks / 1000.0f) * FTTOWU);
								}
								else
								{
									vehiclepnt->Status |= VL_INVISIBLE;
								}
							}
							else if((vehiclepnt->pCurrWP->iWaitFlags & WAIT_FLAG_SURFACE) || (vehiclepnt->bDamageLevel >= 2))
							{
								vehiclepnt->Status &= ~VL_INVISIBLE;
								if(vehiclepnt->fSubmergeDepth > 0)
								{
									vehiclepnt->fSubmergeDepth -= (((float)DeltaTicks / 1000.0f) * FTTOWU);
								}
								else
								{
									vehiclepnt->fSubmergeDepth = 0;
								}
							}
							vehiclepnt->WorldPosition.Y = vehiclepnt->WorldPosition.Y - vehiclepnt->fSubmergeDepth;
						}
					}
//					if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
//					{
//						if (!(vehiclepnt->Status & VL_IN_VISUAL_RANGE))
//							vehiclepnt->Status |= (VL_IN_VISUAL_RANGE | VL_BRIDGE_TARE);

//						old_status = vehiclepnt->Status & VL_ON_BRIDGE;

//						if (PointOnAnyBridge(vehiclepnt->Heading,(DWORD *)&vehiclepnt->Status,&vehiclepnt->Bridge,vehiclepnt->WorldPosition,normal))
//							vehiclepnt->Attitude.SetFromNormalAndHeading(vehiclepnt->Heading,normal);
//						else
//						{
//							land_y = LandHeight(vehiclepnt->WorldPosition.X, vehiclepnt->WorldPosition.Z);

//							if (old_status && (fabs(land_y - vehiclepnt->WorldPosition.Y) > (15 FEET)))
//								FallMe(vnum,vehiclepnt);
//							else
//							{
//								vehiclepnt->WorldPosition.Y = land_y;
//								GetMatrix(vehiclepnt->WorldPosition, vehiclepnt->Heading, vehiclepnt->Attitude);
//							}
//						}
//						vehiclepnt->Attitude.GetRPH(&Roll,&vehiclepnt->Pitch,&vehiclepnt->Heading);
//					}
//					else
//					{
//						vehiclepnt->Status &= ~VL_IN_VISUAL_RANGE;
//						tdist -= distmove;
//						vehiclepnt->WorldPosition.Y = vehiclepnt->WayPosition.Y + (tdist * vehiclepnt->fSinPitch);
//					}
				}
				else if(vehiclepnt->Status & (VL_DESTROYED|VL_DONE_MOVING))
				{
					CalculateAttitudeAndHeight(vehiclepnt,vnum);

					if(vehiclepnt->iShipType)
					{
						pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];
						if(pshiptype->lShipType & SHIP_TYPE_SUBMARINE)
						{
							if(((vehiclepnt->pCurrWP->iWaitFlags & WAIT_FLAG_SUBMERGE) || ((vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_SUBMERGE) && (vehiclepnt->numwaypts <= 1))) && (vehiclepnt->bDamageLevel < 2))
							{
								if(vehiclepnt->fSubmergeDepth < (SUBMERGE_DEPTH * FTTOWU))
								{
									vehiclepnt->fSubmergeDepth += (((float)DeltaTicks / 1000.0f) * FTTOWU);
								}
								else
								{
									vehiclepnt->Status |= VL_INVISIBLE;
								}
							}
							else if(((vehiclepnt->pCurrWP->iWaitFlags & WAIT_FLAG_SURFACE) || ((vehiclepnt->pCurrWP->iEndOfPathOptions & STOP_FLAG_SURFACE) && (vehiclepnt->numwaypts <= 1))) || (vehiclepnt->bDamageLevel >= 2))
							{
								vehiclepnt->Status &= ~VL_INVISIBLE;
								if(vehiclepnt->fSubmergeDepth > 0)
								{
									vehiclepnt->fSubmergeDepth -= (((float)DeltaTicks / 1000.0f) * FTTOWU);
								}
								else
								{
									vehiclepnt->fSubmergeDepth = 0;
								}
							}
							vehiclepnt->WorldPosition.Y = vehiclepnt->WorldPosition.Y - vehiclepnt->fSubmergeDepth;
						}
					}
//					if(InHighResTerrainRegion(vehiclepnt->WorldPosition))
//					{
//						if (!(vehiclepnt->Status & VL_IN_VISUAL_RANGE))
//							vehiclepnt->Status |= (VL_IN_VISUAL_RANGE | VL_BRIDGE_TARE);

//						old_status = vehiclepnt->Status & VL_ON_BRIDGE;

//						if (PointOnAnyBridge(vehiclepnt->Heading,(DWORD *)&vehiclepnt->Status,&vehiclepnt->Bridge,vehiclepnt->WorldPosition,normal))
//							vehiclepnt->Attitude.SetFromNormalAndHeading(vehiclepnt->Heading,normal);
//						else
//						{
//							land_y = LandHeight(vehiclepnt->WorldPosition.X, vehiclepnt->WorldPosition.Z);

//							if (old_status && (fabs(land_y - vehiclepnt->WorldPosition.Y) > (15 FEET)))
//								FallMe(vnum,vehiclepnt);
//							else
//							{
//								vehiclepnt->WorldPosition.Y = land_y;
//								GetMatrix(vehiclepnt->WorldPosition, vehiclepnt->Heading, vehiclepnt->Attitude);
//							}
//						}
//						vehiclepnt->Attitude.GetRPH(&Roll,&vehiclepnt->Pitch,&vehiclepnt->Heading);
//					}
//					else
//						vehiclepnt->Status &= ~VL_IN_VISUAL_RANGE;


					if ((vehiclepnt->lAITimer1 < 0) &&  ((vehiclepnt->Status & (VL_SWEPT_UP | VL_DESTROYED)) == VL_DESTROYED))
					{
						if (vehiclepnt->Status & VL_SINKING)
						{
							vehiclepnt->Status |= VL_INVISIBLE | VL_SWEPT_UP;
							vehiclepnt->Status &= ~VL_SINKING;
						}
						else
						{
							if (vehiclepnt->Status & VL_ON_BRIDGE)
							{
								vehiclepnt->Status &= ~VL_ON_BRIDGE;
								vehiclepnt->Status |= VL_INVISIBLE | VL_SWEPT_UP;
							}
							else
							{
								radhead = DegToRad(AIConvertAngleTo180Degree(vehiclepnt->Heading - 0x4000));

								workvalx = vehiclepnt->WorldPosition.X;
								workvalz = vehiclepnt->WorldPosition.Z;

								workvalx -= (sin(radhead) * LANE_WIDTH * FTTOWU);
								workvalz -= (cos(radhead) * LANE_WIDTH * FTTOWU);

								vehiclepnt->WorldPosition.X = workvalx;
								vehiclepnt->WorldPosition.Z = workvalz;
								vehiclepnt->Status |= VL_SWEPT_UP;
							}

						}

					}
				}

				if ((vehiclepnt->Status & VL_ON_BRIDGE) && (vehiclepnt->Bridge->Basics.Flags & BI_DESTROYED))
					FallMe(vnum,vehiclepnt);
			}


			if(vehiclepnt->lAIFlags1 & PLANES_ON_DECK)
			{
				VUpdatePlanesOnDeck(vehiclepnt);
			}
			if(iCarrierWatch != -1)
			{
				if(PlayerPlane->AI.iHomeBaseId == (vehiclepnt - MovingVehicles))
				{
					UpdateCarrierViews();
				}
			}
			if(MultiPlayer & (lAINetFlags1 & NGAI_ACTIVE))
			{
				vehiclepnt->lNetTimer = vehiclepnt->lNetTimer - DeltaTicks;
				if(vehiclepnt->lNetTimer < 0)
				{
					vehiclepnt->lNetTimer = vehiclepnt->lNetTimer + 180000;
					workangle = AIConvertAngleTo180Degree(vehiclepnt->Heading);
					netfpoint.X = vehiclepnt->WorldPosition.X;
					netfpoint.Z = vehiclepnt->WorldPosition.Z;
					netfpoint.Y = workangle;
					NetPutGenericMessage2FPoint(NULL, GM2FP_VEH_POS, netfpoint, vehiclepnt - MovingVehicles);
				}
			}

			if((vehiclepnt->Status & VL_ACTIVE) && !(vehiclepnt->Status & (VL_INVISIBLE)) && vehiclepnt->Type->iShipType && (!vehiclepnt->Type->TypeNumber))
			{
				if (((vehiclepnt->Type->Model->Name[0] | 0x20) == 'n') && ((vehiclepnt->Type->Model->Name[1] | 0x20) == 'i') && ((vehiclepnt->Type->Model->Name[2] | 0x20) == 't'))
				{
					if (vehiclepnt->Status & (VL_SINKING | VL_DESTROYED))
						vehiclepnt->Runwaydata.Flags = RI_DESTROYED;
					else
					{
						vehiclepnt->Runwaydata.Flags = 0;

						vehiclepnt->Runwaydata.ILSFarEndPoint = (vehiclepnt->Runwaydata.ILSLandingPoint = vehiclepnt->Type->CatchWires.Values[0]);

						vehiclepnt->Runwaydata.ILSFarEndPoint.X -= fabs(vehiclepnt->Runwaydata.ILSFarEndPoint.Z)*2.0f*0.1616;  // 9.3 degrees
						vehiclepnt->Runwaydata.ILSFarEndPoint.Z = -vehiclepnt->Runwaydata.ILSFarEndPoint.Z;

						vehiclepnt->Runwaydata.ILSLandingPoint *= vehiclepnt->Attitude;
						vehiclepnt->Runwaydata.ILSLandingPoint += vehiclepnt->WorldPosition;

						vehiclepnt->Runwaydata.ILSFarEndPoint *= vehiclepnt->Attitude;
						vehiclepnt->Runwaydata.ILSFarEndPoint += vehiclepnt->WorldPosition;

						vehiclepnt->Runwaydata.Heading = (float)atan2(vehiclepnt->Runwaydata.ILSFarEndPoint.X - vehiclepnt->Runwaydata.ILSLandingPoint.X ,vehiclepnt->Runwaydata.ILSLandingPoint.Z - vehiclepnt->Runwaydata.ILSFarEndPoint.Z);
						if (vehiclepnt->Runwaydata.Heading < 0.0f)
							vehiclepnt->Runwaydata.Heading += 2.0f*(float)PI;
						vehiclepnt->Runwaydata.Heading *= 180.0f/(float)PI;
					}
				}
			}
		}
		else
			vehiclepnt->Runwaydata.Flags = RI_DESTROYED;

	}
	
	dLastPlayerV = PlayerPlane->V;
	int slotnum;
	if(MultiPlayer)
	{
		for(cnt = 0; cnt < MAX_HUMANS; cnt ++)
		{
			slotnum = iSlotToPlane[cnt];
			if(slotnum >= 0)
			{
				dLastSlotV[cnt] = Planes[slotnum].V;
			}
		}
	}
	
	iFirstTimeInFrame = 0;
}

//*************************************************************************************************
void VCheckWayPointAction(MovingVehicleParams *vehiclepnt)
{
	int waitflags;
	MovingVehicleParams *leader;
	float dx, dz, tdist;
	int cnt;

	if(vehiclepnt->Status & VL_SKIP_NEXT_WAIT)
	{
		vehiclepnt->Status &= ~VL_SKIP_NEXT_WAIT;
	}
	else
	{
		waitflags = vehiclepnt->pCurrWP->iWaitFlags;

		if(waitflags)
		{
			if(waitflags & WAIT_FLAG_TIME)
			{
				vehiclepnt->Status |= VL_DO_NEXT_WAY;
				vehiclepnt->lAITimer1 = vehiclepnt->pCurrWP->iWaitTime * 60000;
				if((vehiclepnt->lAITimer1) || ((vehiclepnt->numwaypts <= 1) && (waitflags & WAIT_FLAG_FIREWEAPON) && (vehiclepnt->pCurrWP->iEndOfPathOptions & (STOP_FLAG_STOP))))
				{
					vehiclepnt->Status |= VL_WAITING;
					vehiclepnt->Status &= ~(VL_CHECK_FLAGS|VL_MOVING);
					leader = (MovingVehicleParams *)vehiclepnt->pLeadVehicle;
					if(leader)
					{
						dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
						dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;

						tdist = sqrt((dx*dx) + (dz*dz)) * WUTOFT;

						if(tdist < vehiclepnt->fVehicleSpacing)
						{
							vehiclepnt->lAITimer1 = leader->lAITimer1;
						}
					}
				}
				else
				{
					vehiclepnt->lAITimer1 = -1;
				}
			}
			else if(waitflags & WAIT_FLAG_FLAG)
			{
				vehiclepnt->Status |= VL_DO_NEXT_WAY;
				if(!AICheckEventFlagStatus(vehiclepnt->pCurrWP->iWaitFlag))
				{
					vehiclepnt->Status |= (VL_WAITING|VL_CHECK_FLAGS);
					vehiclepnt->Status &= ~(VL_MOVING);
					vehiclepnt->iCheckFlag = vehiclepnt->pCurrWP->iWaitFlag;
				}
			}
		}
		if(vehiclepnt->Status & (VL_WAITING))
		{
			if(waitflags & WAIT_FLAG_DISSAPPEAR)
			{
				vehiclepnt->Status |= VL_INVISIBLE;
			}
//			else if((waitflags & WAIT_FLAG_FIREWEAPON) && (vehiclepnt->RadarWeaponData->WType))
			else if(waitflags & WAIT_FLAG_FIREWEAPON)
			{	//  Needs to be updated for NEW stuff SRE
				vehiclepnt->Status |= VL_FIRE_WEAPONS;
				VClearRadardat(vehiclepnt);
				if(vehiclepnt->iShipType)
				{
					for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
					{
						if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
								(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
								((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
						{
							vehiclepnt->Status |= VL_SURFACE_ATTACK;
							if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
								vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
						}
					}
				}
				else
				{
					for(cnt = 0; cnt < 2; cnt ++)
					{
						if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
								(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
								((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
						{
							vehiclepnt->Status |= VL_SURFACE_ATTACK;
							if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
								vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
						}
					}
				}
			}
		}
	}
}

//*************************************************************************************************
void VCheckWaiting(MovingVehicleParams *vehiclepnt)
{
	float dx, dz;

	if(vehiclepnt->Status & VL_AVOID_JAM)
	{
		return;
	}

	if(vehiclepnt->Status & VL_FIRE_WEAPONS)
	{
		VGDCheckDefenses(vehiclepnt);
	}

	if(vehiclepnt->Status & VL_CHECK_FLAGS)
	{
		if(AICheckEventFlagStatus(vehiclepnt->iCheckFlag))
		{
			vehiclepnt->Status |= VL_MOVING;
			vehiclepnt->Status &= ~(VL_WAITING|VL_CHECK_FLAGS);
			if(!(vehiclepnt->Status & VL_WAIT_DESTRUCTION))
			{
				dx = vehiclepnt->WayPosition.X - vehiclepnt->WorldPosition.X;
				dz = vehiclepnt->WayPosition.Z - vehiclepnt->WorldPosition.Z;
				dx *= WUTOFT;
				dz *= WUTOFT;

				if(((fabs(dx) < 2.0f) && (fabs(dz) < 2.0f)) || (vehiclepnt->Status & VL_DO_NEXT_WAY))
				{
					VNextWayPoint(vehiclepnt, 0);
				}
			}
		}
	}
	else if((vehiclepnt->lAITimer1 < 0) && (!((vehiclepnt->numwaypts <= 1) && (vehiclepnt->Status & VL_FIRE_WEAPONS) && (vehiclepnt->pCurrWP->iEndOfPathOptions & (STOP_FLAG_STOP)))))
	{
		vehiclepnt->Status |= VL_MOVING;
		vehiclepnt->Status &= ~(VL_WAITING);
		if(!(vehiclepnt->Status & VL_WAIT_DESTRUCTION))
		{
			dx = vehiclepnt->WayPosition.X - vehiclepnt->WorldPosition.X;
			dz = vehiclepnt->WayPosition.Z - vehiclepnt->WorldPosition.Z;
			dx *= WUTOFT;
			dz *= WUTOFT;

			if(((fabs(dx) < 2.0f) && (fabs(dz) < 2.0f)) || (vehiclepnt->Status & VL_DO_NEXT_WAY))
			{
				VNextWayPoint(vehiclepnt, 0);
			}
		}
	}
}

//*************************************************************************************************
void VCheckLeader(MovingVehicleParams *vehiclepnt, MovingVehicleParams *leader)
{
	float dx, dz;
	int cnt;

	if(leader->Status & (VL_ACTIVE))
	{
		if(!(vehiclepnt->Status & VL_MOVING))
		{
			if((leader->Status & VL_AVOID_JAM) && (!(vehiclepnt->Status & VL_AVOID_JAM)))
			{
				dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
				dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
				dx *= WUTOFT * 0.66667f;
				dz *= WUTOFT * 0.66667f;

				if((fabs(dx) < vehiclepnt->fVehicleSpacing) && (fabs(dz) < vehiclepnt->fVehicleSpacing))
				{
					vehiclepnt->Status &= ~(VL_MOVING);
					vehiclepnt->Status |= (VL_AVOID_JAM);
				}
			}
		}
		else if((leader->Status & (VL_DESTROYED | VL_FALLING)) && (leader->lAITimer1 > 0) && (!(vehiclepnt->Status & (VL_WAIT_DESTRUCTION))))
		{
			dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
			dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
			dx *= WUTOFT * 0.66667f;
			dz *= WUTOFT * 0.66667f;

			if((fabs(dx) < vehiclepnt->fVehicleSpacing) && (fabs(dz) < vehiclepnt->fVehicleSpacing))
			{
				vehiclepnt->lAITimer1 = 120000;
				vehiclepnt->Status |= (VL_WAITING|VL_WAIT_DESTRUCTION);
				vehiclepnt->Status &= ~(VL_CHECK_FLAGS|VL_MOVING);
			}
		}
		else if((leader->Status & VL_WAITING) && (!(leader->Status & VL_INVISIBLE)))
		{
			dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
			dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
			dx *= WUTOFT * 0.66667f;
			dz *= WUTOFT * 0.66667f;

			if((fabs(dx) < vehiclepnt->fVehicleSpacing) && (fabs(dz) < vehiclepnt->fVehicleSpacing))
			{
				vehiclepnt->lAITimer1 = leader->lAITimer1;
				vehiclepnt->iCheckFlag = leader->iCheckFlag;
				vehiclepnt->Status &= ~(VL_CHECK_FLAGS|VL_MOVING|VL_WAIT_DESTRUCTION);
				vehiclepnt->Status |= (VL_WAITING) | (leader->Status & (VL_CHECK_FLAGS|VL_WAIT_DESTRUCTION));
				if(!(vehiclepnt->Status & VL_WAIT_DESTRUCTION))
				{
					vehiclepnt->Status |= VL_SKIP_NEXT_WAIT;
					//  Need to update for multi weapons SRE
					if((leader->Status & VL_FIRE_WEAPONS) && (!(vehiclepnt->Status & VL_FIRE_WEAPONS)) && (vehiclepnt->RadarWeaponData->WType))
					{
						vehiclepnt->Status |= VL_FIRE_WEAPONS;
						VClearRadardat(vehiclepnt);
						if(vehiclepnt->iShipType)
						{
							for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
							{
								if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
										(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
										((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
								{
									vehiclepnt->Status |= VL_SURFACE_ATTACK;
									if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
										vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
								}
							}
						}
						else
						{
							for(cnt = 0; cnt < 2; cnt ++)
							{
								if((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) ||
										(pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
										((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iWeaponType == WEAPON_TYPE_GUN) && ((pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 11) || (pDBWeaponList[vehiclepnt->RadarWeaponData[cnt].iWeaponIndex].iUsageIndex == 12))))
								{
									vehiclepnt->Status |= VL_SURFACE_ATTACK;
									if(vehiclepnt->RadarWeaponData[cnt].lWActionTimer < 0)
										vehiclepnt->RadarWeaponData[cnt].lWActionTimer = 90000 + rand();
								}
							}
						}
					}
				}
			}
		}
		else if(leader->Status & VL_DONE_MOVING)
		{
			dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
			dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
			dx *= WUTOFT * 0.66667f;
			dz *= WUTOFT * 0.66667f;

			if((fabs(dx) < vehiclepnt->fVehicleSpacing) && (fabs(dz) < vehiclepnt->fVehicleSpacing))
			{
				if(!(leader->Status & VL_INVISIBLE))
				{
					vehiclepnt->Status &= ~(VL_MOVING);
					vehiclepnt->Status |= (VL_DONE_MOVING);
				}
			}
		}
		else if(leader->Status & VL_AVOID_JAM)
		{
			dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
			dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
			dx *= WUTOFT * 0.66667f;
			dz *= WUTOFT * 0.66667f;

			if((fabs(dx) < vehiclepnt->fVehicleSpacing) && (fabs(dz) < vehiclepnt->fVehicleSpacing))
			{
				if(!(leader->Status & VL_INVISIBLE))
				{
					vehiclepnt->Status &= ~(VL_MOVING);
					vehiclepnt->Status |= (VL_AVOID_JAM);
				}
			}
		}
	}
}

//*************************************************************************************************
void VCheckForTrafficJam(MovingVehicleParams *vehiclepnt)
{
	float dx, dz, tdist;
	MovingVehicleParams *leader = (MovingVehicleParams *)vehiclepnt->pLeadVehicle;

	if(!leader)
	{
		vehiclepnt->Status &= ~(VL_AVOID_JAM);
		if(!(vehiclepnt->Status & VL_WAITING))
		{
			vehiclepnt->Status |= VL_MOVING;
		}
		return;
	}

	dx = leader->WorldPosition.X - vehiclepnt->WorldPosition.X;
	dz = leader->WorldPosition.Z - vehiclepnt->WorldPosition.Z;

	tdist = sqrt((dx*dx) + (dz*dz)) * WUTOFT;

	if(tdist > vehiclepnt->fVehicleSpacing)
	{
		vehiclepnt->Status &= ~(VL_AVOID_JAM);
		if(!(vehiclepnt->Status & VL_WAITING))
		{
			vehiclepnt->Status |= VL_MOVING;
		}
		return;
	}
}

//*************************************************************************************************
void DestroyShip(MovingVehicleParams *vehiclepnt,BOOL play_sound)
{
	FPoint vel( 0.0f);
	DBShipType *pvehicletype;
	float exp_size;
	float exp_radius;
	CanisterType fire_type;
	float fire_size;
	float fire_duration;
	DWORD sound;
	int cnt;
	BYTE bworkvar;

	if (vehiclepnt->iShipType && !(vehiclepnt->Status & VL_DESTROYED))
	{
		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			if(vehiclepnt == &MovingVehicles[PlayerPlane->AI.iHomeBaseId])
			{
				AICJustMessage(AIC_STRIKE_MOTHER_CLOSED, AICF_STRIKE_MOTHER_CLOSED, SPCH_STRIKE);
				AICJustMessage(AIC_STRIKE_MOTHER_ABANDONED, AICF_STRIKE_MOTHER_ABANDONED, SPCH_STRIKE);

				AICAddSoundCall(AICAlphaCheck, PlayerPlane - Planes, 15000, 50, 6);

			}
		}

		pvehicletype = &pDBShipList[vehiclepnt->iVDBIndex];

		vehiclepnt->Status |= (VL_DESTROYED);
		vehiclepnt->Status &= ~(VL_MOVING|VL_WAITING);
//		vehiclepnt->Type = vehiclepnt->DestroyedType;

		for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
		{
			vehiclepnt->RadarWeaponData[cnt].lRFlags1 &= ~GD_RADAR_PLAYER_PING;
			vehiclepnt->RadarWeaponData[cnt].lRFlags1 |= GD_I_AM_DEAD;
		}

		if (rand() & 1)
		{
			bworkvar = 2 | (play_sound & 0x1);
		 	if(MultiPlayer && (!netCall))
			{
				NetPutGenericMessage3(PlayerPlane, GM3_SINK_SHIP, vehiclepnt - MovingVehicles, bworkvar);
			}
			vehiclepnt->Status |= VL_SINKING_POS;
		}
		else
		{
			bworkvar = (play_sound & 0x1);
		 	if(MultiPlayer && (!netCall))
			{
				NetPutGenericMessage3(PlayerPlane, GM3_SINK_SHIP, vehiclepnt - MovingVehicles, bworkvar);
			}
			vehiclepnt->Status |= VL_SINKING_NEG;
		}

		vehiclepnt->FirstSinkingTick = GameLoop;

		vehiclepnt->SinkingMultiplier = (vehiclepnt->Type->HighLR.Y * 1.3f * (50.0f/ (float)(0.8 FEET)));
		vehiclepnt->lAITimer1 = ( int )(vehiclepnt->SinkingMultiplier*20.0f);
		vehiclepnt->SinkingMultiplier = 1.0f / vehiclepnt->SinkingMultiplier;

		if (Camera1.AttachedObject == (int *)vehiclepnt)
			SwitchCameraSubjectToPoint(vehiclepnt->WorldPosition);

		if (Camera1.TargetObject == (int *)vehiclepnt)
			SwitchCameraTargetToPoint(vehiclepnt->WorldPosition);

		fire_type = CT_NONE;
		exp_size = 0;
		sound = 0;

		switch (pvehicletype->iFireType)
		{
			case FIRE_TYPE_HOUSE:
				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_BLACK_SMOKE;
					fire_size = 0.5f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case FIRE_TYPE_TRUCK:
				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 0.5f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case  FIRE_TYPE_FUELBLADDER:
				exp_size = 100;		//same as mk82
				exp_radius = 100.0f;	//smaller
				sound = WARHEAD_STANDARD_SMALL;

				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 1.0f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case  FIRE_TYPE_GASTANKER:
				exp_size = 200;		//same as mk82
				exp_radius = 200.0f;	//smaller
				sound = WARHEAD_STANDARD_MEDIUM;

				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 1.5f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;
		}

		if (fire_type)
		{
			FPointDouble fire_position;
			PositionFireOnVehicle(vehiclepnt,fire_position,TRUE);
			GetVehicleFireHandle(fire_position,(void *)vehiclepnt,fire_type,fire_size*2.0f,fire_duration);
			if (exp_size)
			{
				NewExplosionForcedWeapon(fire_position,exp_radius,exp_size);
				if (sound && play_sound)
					PlayPositionalExplosion(sound,Camera1,fire_position);
			}
		}

		if(!(vehiclepnt->Status & VL_GOAL_CHECKED))
		{
			AIUpdateGoalStates(vehiclepnt->iVListID, vehiclepnt->iVListID, vehiclepnt->iVListID, MOVINGVEHICLE);
			vehiclepnt->Status |= VL_GOAL_CHECKED;
		}
	}
}

//*************************************************************************************************
void DestroyVehicle(MovingVehicleParams *vehiclepnt,BOOL play_sound)
{
	FPoint vel( 0.0f);
	DBVehicleType *pvehicletype;
	float exp_size;
	float exp_radius;
	CanisterType fire_type;
	float fire_size;
	float fire_duration;
	DWORD sound;
	int cnt;

	if (!vehiclepnt->iShipType && !(vehiclepnt->Status & VL_DESTROYED))
	{
		pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];

		vehiclepnt->Status |= (VL_DESTROYED);
		vehiclepnt->Status &= ~(VL_MOVING|VL_WAITING);
		vehiclepnt->Type = vehiclepnt->DestroyedType;

		//  Say sink ship but works for vehicles too.
		if(MultiPlayer && (!netCall))
		{
			NetPutGenericMessage3(PlayerPlane, GM3_SINK_SHIP, vehiclepnt - MovingVehicles, play_sound & 0x1);
		}

		for(cnt = 0; cnt < 2; cnt ++)
		{
			vehiclepnt->RadarWeaponData[cnt].lRFlags1 &= ~GD_RADAR_PLAYER_PING;
			vehiclepnt->RadarWeaponData[cnt].lRFlags1 |= GD_I_AM_DEAD;
		}

		if (pvehicletype->lVehicleType == VEHICLE_TYPE_SHIP)
		{
			if (rand() & 1)
				vehiclepnt->Status |= VL_SINKING_POS;
			else
				vehiclepnt->Status |= VL_SINKING_NEG;

			vehiclepnt->FirstSinkingTick = GameLoop;

			vehiclepnt->SinkingMultiplier = (vehiclepnt->Type->HighLR.Y * 1.3f * (50.0f/ (float)(0.8 FEET)));
			vehiclepnt->lAITimer1 = ( int )(vehiclepnt->SinkingMultiplier*20.0f);
			vehiclepnt->SinkingMultiplier = 1.0f / vehiclepnt->SinkingMultiplier;
		}
		else
			vehiclepnt->lAITimer1 = 120000;

		if (Camera1.AttachedObject == (int *)vehiclepnt)
			SwitchCameraSubjectToPoint(vehiclepnt->WorldPosition);

		if (Camera1.TargetObject == (int *)vehiclepnt)
			SwitchCameraTargetToPoint(vehiclepnt->WorldPosition);

		fire_type = CT_NONE;
		exp_size = 0;
		sound = 0;

		switch (pvehicletype->iFireType)
		{
			case FIRE_TYPE_HOUSE:
				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_BLACK_SMOKE;
					fire_size = 0.5f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case FIRE_TYPE_TRUCK:
				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 0.5f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case  FIRE_TYPE_FUELBLADDER:
				exp_size = 100;		//same as mk82
				exp_radius = 100.0f;	//smaller
				sound = WARHEAD_STANDARD_SMALL;

				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 1.0f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case  FIRE_TYPE_GASTANKER:
				exp_size = 200;		//same as mk82
				exp_radius = 200.0f;	//smaller
				sound = WARHEAD_STANDARD_MEDIUM;

				if (pvehicletype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 1.5f;
					fire_duration = (float)pvehicletype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;
		}

		if (fire_type)
		{
			FPointDouble fire_position;
			PositionFireOnVehicle(vehiclepnt,fire_position,TRUE);
			GetVehicleFireHandle(fire_position,(void *)vehiclepnt,fire_type,fire_size,fire_duration);
			if (exp_size)
			{
				NewExplosionForcedWeapon(fire_position,exp_radius,exp_size);
				if (sound && play_sound)
					PlayPositionalExplosion(sound,Camera1,fire_position);
			}
		}

		if(!(vehiclepnt->Status & VL_GOAL_CHECKED))
		{
			AIUpdateGoalStates(vehiclepnt->iVListID, vehiclepnt->iVListID, vehiclepnt->iVListID, MOVINGVEHICLE);
			vehiclepnt->Status |= VL_GOAL_CHECKED;
		}
	} else {
		DestroyShip(vehiclepnt,play_sound);
	}
}

//*************************************************************************************************
BOOL VDamageShip(MovingVehicleParams *vehiclepnt, DWORD damage_type, DWORD game_loop, float damage, int *num_secondaries, float *delay_time, MultipleExplosionType *secondaries)
{
	int our_num_seconds = 0;
	BOOL return_value = FALSE;
	int target_type;
	DamageProportions *ratio;
	float our_max_total,our_max_sustainable;
	float add_to_sustain, add_to_total;
	float rand_rat;
	DBShipType *pvehicletype;
	int damagelevel;
	float fworkvar;
	int cnt;

	if (vehiclepnt->iShipType)
	{
		pvehicletype = &pDBShipList[vehiclepnt->iVDBIndex];

		//Sounds weird, but let's get the secondaries first.  That way we know what to do if we're destroyed

		*num_secondaries = 0;

		// Now, if we're blown up or our secondaries haven't gone off yet, keep calculating

		if (!(vehiclepnt->Status & VL_DESTROYED) && (our_max_total = pvehicletype->iPermanentDamage) && (vehiclepnt->fTotalDamage <= our_max_total))
		{
			target_type = pvehicletype->iArmorType;
			switch(target_type)
			{
				case ARMOR_TYPE_SOFT:
					target_type = 0;
					break;
				case ARMOR_TYPE_NORMAL:
					target_type = 1;
					break;
				case ARMOR_TYPE_ARMORED:
					target_type = 2;
					break;
				default:
					target_type = 1;
					break;
			}

			if ((vehiclepnt->Status & VL_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
				target_type = DTE_SOFT_TARGET;
			target_type |= (damage_type & DTE_TARGET_TYPE) << 2;

			ratio = &DamageMultipliers[target_type << 1];
			if (damage_type & DTE_DIRECT_HIT)
				ratio++;

			//Now we have the correct damage amount, and the ratio to use to apply it to the damage on the object.

			//first update amount since last time

			our_max_sustainable = pvehicletype->iSustainedDamage;

			if (!(vehiclepnt->Status & VL_DESTROYED))
			{
				if (vehiclepnt->fSustainableDamage  > 0.0f)
				{
					vehiclepnt->fSustainableDamage -= ((float)game_loop - vehiclepnt->LastDamagedTick) * our_max_sustainable * 1.0f/32.0f;
					switch(vehiclepnt->bDamageLevel)  //  Damage armor as ship gets hurt.
					{
						case 0:
							if (vehiclepnt->fSustainableDamage <0.0f)
								vehiclepnt->fSustainableDamage = 0.0f;
							break;
						case 1:
							fworkvar = (float)our_max_sustainable * 0.25;
							if (vehiclepnt->fSustainableDamage < fworkvar)
								vehiclepnt->fSustainableDamage = fworkvar;
							break;
						case 2:
							fworkvar = (float)our_max_sustainable * 0.50;
							if (vehiclepnt->fSustainableDamage < fworkvar)
								vehiclepnt->fSustainableDamage = fworkvar;
							break;
						case 3:
							fworkvar = (float)our_max_sustainable * 0.75;
							if (vehiclepnt->fSustainableDamage < fworkvar)
								vehiclepnt->fSustainableDamage = fworkvar;
							break;
						default:
							fworkvar = (float)our_max_sustainable;
							if (vehiclepnt->fSustainableDamage < fworkvar)
								vehiclepnt->fSustainableDamage = fworkvar;
							break;
					}
					vehiclepnt->LastDamagedTick=game_loop;
				}

				// then figure out how many points will go to sustainable
				add_to_sustain = damage*ratio->SustainablePercentage;

				add_to_total = (vehiclepnt->fSustainableDamage += add_to_sustain) - our_max_sustainable;

				if (add_to_total < 0.0f)
					add_to_total = 0.0f;
				else
					vehiclepnt->fSustainableDamage = our_max_sustainable;

				// then figure out how many points will go to total
				add_to_total += damage*ratio->TotalPercentage;

				if (add_to_total || add_to_sustain)
					vehiclepnt->LastDamagedTick=game_loop;

				if((add_to_total > 0) && (PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER))
				{
					if(vehiclepnt == &MovingVehicles[PlayerPlane->AI.iHomeBaseId])
					{
						if(lCarrierHitMsgTimer < 0)
						{
							lCarrierHitMsgTimer = 60000;
							int irandval = rand() & 0x1;
							switch(irandval)
							{
								case 1:
									AICJustMessage(AIC_STRIKE_HIT_ALPHA, AICF_STRIKE_HIT_ALPHA, SPCH_STRIKE);
									break;
								default:
									AICJustMessage(AIC_STRIKE_HIT_ALPHA_VAMPS, AICF_STRIKE_HIT_ALPHA_VAMPS, SPCH_STRIKE);
									break;
							}
						}
					}
				}

				vehiclepnt->fTotalDamage +=  add_to_total;

				if(MultiPlayer && (add_to_total))
				{
					NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
				}

				damagelevel = 0;
				if(vehiclepnt->fTotalDamage > pvehicletype->iDamageLevel1)
				{
					damagelevel = 1;
					if(vehiclepnt->fTotalDamage > pvehicletype->iDamageLevel2)
					{
						damagelevel = 2;
						if(vehiclepnt->fTotalDamage > pvehicletype->iDamageLevel3)
						{
							damagelevel = 3;
							if(vehiclepnt->fTotalDamage > pvehicletype->iPermanentDamage)
							{
								damagelevel = 4;
							}
						}
					}
				}

				if(damagelevel > vehiclepnt->bDamageLevel)
				{
					vehiclepnt->bDamageLevel = damagelevel;
					VSetShipDamageLevel(vehiclepnt - MovingVehicles, damagelevel, 0);
				}

				if (vehiclepnt->fTotalDamage > our_max_total)
				{
					return_value = TRUE;

					for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
					{
						vehiclepnt->RadarWeaponData[cnt].lRFlags1 &= ~GD_RADAR_PLAYER_PING;
						vehiclepnt->RadarWeaponData[cnt].lRFlags1 |= GD_I_AM_DEAD;
					}

					our_num_seconds = pvehicletype->iNumExplosions;

					//  I copied this out of DestroyVehicle and DestroyShip.  I think there are conditions where they wouldn't be called.
					if(!(vehiclepnt->Status & VL_GOAL_CHECKED))
					{
						AIUpdateGoalStates(vehiclepnt->iVListID, vehiclepnt->iVListID, vehiclepnt->iVListID, MOVINGVEHICLE);
						vehiclepnt->Status |= VL_GOAL_CHECKED;
					}
				}
			}
			else
			{
				add_to_total = damage;
				vehiclepnt->fTotalDamage +=  add_to_total;
			}

			if(return_value)
			{
	#if 1
				if((our_num_seconds) && (pvehicletype->lExpWeaponType))
				{
					if (add_to_total)
					{
						if (our_max_total == 0.0f)
							our_max_total = 1.0f;

						rand_rat = (vehiclepnt->fTotalDamage - our_max_total)/our_max_total;

						if (frand() < rand_rat)
						{
	//							instance->Physicals.Flags |= IP_BLEW_SECONDARIES;
							*num_secondaries = 1;
							secondaries->WarheadID = pvehicletype->lExpWeaponType;
							secondaries->NumberOfExplosions = pvehicletype->iNumExplosions;
							secondaries->Radius = (BYTE)((float)pvehicletype->iRadius FEET);
							secondaries->Flags = 0;  //  Not sure
							secondaries->TimeBetweenExplosions = pvehicletype->iTicksBetweenBlasts;
							secondaries->Deviation = pvehicletype->iVarianceBetweenBlasts;  // Not sure

							if (add_to_total < (our_max_total*0.5f))
								*delay_time = 500.0f * add_to_total / our_max_total;	//5 seconds *2 to keep from having to .5 our_max_total
							else
								*delay_time = 0.0f;
						}
					}
				}
	//				else
	//					instance->Physicals.Flags |= IP_BLEW_SECONDARIES;  //If we don't have any secondaries, don't keep looking!
	#endif
			}

		}
	}

	return return_value;
}



//*************************************************************************************************
BOOL VDamageVehicle(MovingVehicleParams *vehiclepnt, DWORD damage_type, DWORD game_loop, float damage, int *num_secondaries, float *delay_time, MultipleExplosionType *secondaries)
{
	int our_num_seconds = 0;
	BOOL return_value = FALSE;
	int target_type;
	DamageProportions *ratio;
	float our_max_total,our_max_sustainable;
	float add_to_sustain, add_to_total;
	float rand_rat;
	DBVehicleType *pvehicletype;
	int cnt;

	if (!vehiclepnt->iShipType)
	{
		pvehicletype = &pDBVehicleList[vehiclepnt->iVDBIndex];

		//Sounds weird, but let's get the secondaries first.  That way we know what to do if we're destroyed

		*num_secondaries = 0;

		// Now, if we're blown up or our secondaries haven't gone off yet, keep calculating

		if (!(vehiclepnt->Status & VL_DESTROYED) && (our_max_total = pvehicletype->iPermanentDamage) && (vehiclepnt->fTotalDamage <= our_max_total))
		{
			target_type = pvehicletype->iArmorType;
			switch(target_type)
			{
				case ARMOR_TYPE_SOFT:
					target_type = 0;
					break;
				case ARMOR_TYPE_NORMAL:
					target_type = 1;
					break;
				case ARMOR_TYPE_ARMORED:
					target_type = 2;
					break;
				default:
					target_type = 1;
					break;
			}

			if ((vehiclepnt->Status & VL_DESTROYED) && (target_type != DTE_HARDENED_TARGET))
				target_type = DTE_SOFT_TARGET;
			target_type |= (damage_type & DTE_TARGET_TYPE) << 2;

			ratio = &DamageMultipliers[target_type << 1];
			if (damage_type & DTE_DIRECT_HIT)
				ratio++;

			//Now we have the correct damage amount, and the ratio to use to apply it to the damage on the object.

			//first update amount since last time

			our_max_sustainable = pvehicletype->iSustainedDamage;

			if (!(vehiclepnt->Status & VL_DESTROYED))
			{
				if (vehiclepnt->fSustainableDamage  > 0.0f)
				{
					vehiclepnt->fSustainableDamage -= ((float)game_loop - vehiclepnt->LastDamagedTick) * our_max_sustainable * 1.0f/32.0f;
					if (vehiclepnt->fSustainableDamage <0.0f)
						vehiclepnt->fSustainableDamage = 0.0f;
					vehiclepnt->LastDamagedTick=game_loop;
				}

				// then figure out how many points will go to sustainable
				add_to_sustain = damage*ratio->SustainablePercentage;

				add_to_total = (vehiclepnt->fSustainableDamage += add_to_sustain) - our_max_sustainable;

				if (add_to_total < 0.0f)
					add_to_total = 0.0f;
				else
					vehiclepnt->fSustainableDamage = our_max_sustainable;

				// then figure out how many points will go to total
				add_to_total += damage*ratio->TotalPercentage;

				if (add_to_total || add_to_sustain)
					vehiclepnt->LastDamagedTick=game_loop;

				vehiclepnt->fTotalDamage +=  add_to_total;

				if(MultiPlayer && (add_to_total))
				{
					NetPutGenericMessage2Float(NULL, GM2F_VEH_DAMAGE, vehiclepnt->fTotalDamage, vehiclepnt - MovingVehicles);
				}

				if (vehiclepnt->fTotalDamage > our_max_total)
				{
					return_value = TRUE;

					for(cnt = 0; cnt < 2; cnt ++)
					{
						vehiclepnt->RadarWeaponData[cnt].lRFlags1 &= ~GD_RADAR_PLAYER_PING;
						vehiclepnt->RadarWeaponData[cnt].lRFlags1 |= GD_I_AM_DEAD;
					}

					our_num_seconds = pvehicletype->iNumExplosions;

					//  I copied this out of DestroyVehicle and DestroyShip.  I think there are conditions where they wouldn't be called.
					if(!(vehiclepnt->Status & VL_GOAL_CHECKED))
					{
						AIUpdateGoalStates(vehiclepnt->iVListID, vehiclepnt->iVListID, vehiclepnt->iVListID, MOVINGVEHICLE);
						vehiclepnt->Status |= VL_GOAL_CHECKED;
					}
				}
			}
			else
			{
				add_to_total = damage;
				vehiclepnt->fTotalDamage +=  add_to_total;
			}

			if(return_value)
			{
	#if 1
				if((our_num_seconds) && (pvehicletype->lExpWeaponType))
				{
					if (add_to_total)
					{
						if (our_max_total == 0.0f)
							our_max_total = 1.0f;

						rand_rat = (vehiclepnt->fTotalDamage - our_max_total)/our_max_total;

						if (frand() < rand_rat)
						{
	//							instance->Physicals.Flags |= IP_BLEW_SECONDARIES;
							*num_secondaries = 1;
							secondaries->WarheadID = pvehicletype->lExpWeaponType;
							secondaries->NumberOfExplosions = pvehicletype->iNumExplosions;
							secondaries->Radius = (BYTE)((float)pvehicletype->iRadius FEET);
							secondaries->Flags = 0;  //  Not sure
							secondaries->TimeBetweenExplosions = pvehicletype->iTicksBetweenBlasts;
							secondaries->Deviation = pvehicletype->iVarianceBetweenBlasts;  // Not sure

							if (add_to_total < (our_max_total*0.5f))
								*delay_time = 500.0f * add_to_total / our_max_total;	//5 seconds *2 to keep from having to .5 our_max_total
							else
								*delay_time = 0.0f;
						}
					}
				}
	//				else
	//					instance->Physicals.Flags |= IP_BLEW_SECONDARIES;  //If we don't have any secondaries, don't keep looking!
	#endif
			}

		}
	} else {
		return_value = VDamageShip(vehiclepnt,damage_type,game_loop,damage,num_secondaries, delay_time,secondaries);
	}

	return return_value;
}

//*************************************************************************************************
float VGetSinPitchFromWayPoint(FPointDouble WayPosition, FPointDouble WorldPosition)
{
	double dx, dy, dz;
	double tdist, workpitch;

	dx = WorldPosition.X - WayPosition.X;
	dy = WorldPosition.Y - WayPosition.Y;
	dz = WorldPosition.Z - WayPosition.Z;

	tdist = sqrt((dx*dx) + (dy * dy) + (dz*dz));

	workpitch = asin(dy / tdist);
	workpitch = RadToDeg(workpitch);

	return(workpitch);
}

int iSREBUGCHECK = 0;
//**************************************************************************************
void AISimpleVehicleRadar(MovingVehicleParams *radarsite, GDRadarData *radardat)
{
  	PlaneParams *checkplane = &Planes[0];
//	float widthangle, heightangle;
	float radarrange, rangenm;
//	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	PlaneParams *foundplane = NULL;
	float nlfoundrange;
	PlaneParams *nlfoundplane = NULL;
//	float offangle, toffangle, toffpitch, offpitch;
//	float targbearing, nltargbearing;
//	float targpitch, nltargpitch;
//	PlaneParams *leadplane;
//	PlaneParams *orgtarget;
//	int ifoundplane;
//	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
//	int stationnum;
	int noupdates = 1;
	DBRadarType *radar;
	GDRadarData *wradardat;
	PlaneParams *endplane;
	float fworkval;
	float percsee;
	int foundweapon;
	int itemp;
	int grndside;
	int weaponsearch = 1;
	DBWeaponType *weapon;
	float jammods[36];
	float offangle;
	int cnt;
	int highalert = 0;
	int radarid;
	float visualrange = AI_VISUAL_RANGE_NM * NMTOWU;
	int hours;
	int isnight = 0;
	int inburner = 0;
	int unpaused = 0;
	int minalt = 0;
	int crosssig;

	if(!radarsite)
	{
		return;
	}

	if(radarsite->Status & (VL_DESTROYED))
	{
		return;
	}

	grndside = radarsite->iSide;

	// DONE...XSCOTT FIX
	if (!radarsite->iShipType)
	{
		radarid = pDBVehicleList[radarsite->iVDBIndex].lRadarID;
		radar = GetRadarPtr(radarid);
	}
	else
	{
		AISimpleShipRadar(radarsite, radardat);
		return;
	}

//	radardat = radarsite->RadarWeaponData;

	hours = WorldParams.WorldTime/3600;

	if((hours < 6) || (hours > 18))
	{
		isnight = 1;
	}

	if(radar == NULL)
	{
		if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			rangenm = AI_VISUAL_RANGE_NM;
		}
		else
		{
			radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
			return;
		}
	}
	else
	{
		rangenm = radar->iMaxRange;
		minalt = radar->lMinElevation * FTTOWU;
	}

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	foundplane = NULL;
	nlfoundrange = radarrange;
	nlfoundplane = NULL;

	if(isnight)
	{
		visualrange /= 4;
	}

	radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;

	endplane = LastPlane;

	if(radardat->lRFlags1 & GD_RADAR_LOCK)
	{
		if(radardat->Target == NULL)
		{
			radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
		}
		else
		{
			checkplane = (PlaneParams *)radardat->Target;
			endplane = checkplane;
		}
	}
	else
	{
		radardat->lRLockTimer = -999999;
	}

	AIGetGroundJammingMod(radarsite->WorldPosition, radarsite->iSide, jammods);

	while (checkplane <= endplane)
	{
		if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (checkplane->OnGround == 0) && (checkplane->AI.iSide != grndside))
		{
			dx = checkplane->WorldPosition.X - radarsite->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - radarsite->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - radarsite->WorldPosition.Z;

			if(isnight)
			{
				if((checkplane->RightThrustPercent > 0.75f) || (checkplane->LeftThrustPercent > 0.75f))
				{
					inburner = 1;
				}
				else
				{
					inburner = 0;
				}
			}

//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			if((fabs(dx) < radarrange) && (fabs(dz) < radarrange))
			{
				percsee = GetPlaneVehicleRadarVisibility(radarsite, checkplane, 0, radarid);

				if(percsee)
				{
					tdist = QuickDistance(dx, dz);

					percsee *= GDModifyNoticePercent(radarsite, radardat, checkplane, MOVINGVEHICLE);

					offangle = atan2(-dx, -dz) * 57.2958;
					while(offangle > 359)
						offangle -= 360;
					while(offangle < 0)
						offangle += 360;
					cnt = offangle / 10;
					percsee *= jammods[cnt];

					crosssig = AICheckRadarCrossSig(radarsite, checkplane, dy * WUTOFT, tdist * WUTONM, radar, (radardat->lRFlags1 & GD_RADAR_LOCK));

					if(!crosssig)
					{
						radardat->lRFlags1 &= ~(GD_RADAR_LOCK);

						if(tdist > (visualrange * (1 + (inburner * 4))))
						{
							percsee = 0;
						}
					}

					if((checkplane->AI.lPlaneID == 85) && (tdist < (visualrange * 0.5f)))
					{
						percsee = 0;
					}

					if(tdist < (radarrange * jammods[cnt]))
					{
						if((!(radardat->lRFlags1 & GD_VISUAL_SEARCH)) && (radar))
						{
	 						AIUpdateGroundRadarThreat(radarsite, checkplane, tdist, MOVINGVEHICLE, radardat);
						}

						if((radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) && (checkplane->HeightAboveGround > minalt) && crosssig)  //  may not need to do for lock but just in case.
						{
							if(AISeeIfStillVehicleMissiled(radarsite, checkplane, 1, radardat))
							{
								noupdates = 0;
								if(percsee)
									percsee = 1.0f;
							}
							else if(radardat->lRFlags1 & GD_RADAR_LOCK)
							{
								radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
								radardat->Target = NULL;
							}
						}
					}
					else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
					{
						checkplane->AI.pPaintedBy = NULL;
						checkplane->AI.iPaintedByType = -1;
						checkplane->AI.iPaintDist = -1;
					}

					if((tdist < nlfoundrange) && (tdist < (radarrange * jammods[cnt])) && (checkplane->HeightAboveGround > minalt))
					{
					//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
					//  If was previous target, keep lock regardless of chance.
						if(checkplane->AI.pPaintedBy && (radarsite != (MovingVehicleParams *)checkplane->AI.pPaintedBy))
						{
							fworkval = checkplane->AI.iPaintDist * NMTOWU;
							if(tdist < fworkval)
							{
//								percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);

								percsee *= 2.0f;

								itemp = rand() & 127;

								if(itemp < (percsee * 127))
								{
									foundplane = checkplane;
									foundrange = tdist;
									if(!AIPlaneAlreadyTarget(MOVINGVEHICLE, radarsite, checkplane))
									{
										nlfoundplane = checkplane;
										nlfoundrange = tdist;
									}
								}
							}
						}
						else
						{
//							percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);
							if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
							{
								percsee *= 3.0f;
							}

							itemp = rand() & 127;

							if(itemp < (percsee * 127))
							{
								foundplane = checkplane;
								foundrange = tdist;
								nlfoundplane = checkplane;
								nlfoundrange = tdist;
							}
						}

					}
					else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
					{
//						percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);
						if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
						{
							percsee *= 3.0f;
						}

						itemp = rand() & 127;

						if(itemp < (percsee * 127))
						{
							checkplane->AI.iPaintDist = tdist * WUTONM;
						}
						else
						{
							checkplane->AI.pPaintedBy = NULL;
							checkplane->AI.iPaintedByType = -1;
							checkplane->AI.iPaintDist = -1;
						}
					}
				}
				else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
				{
					checkplane->AI.pPaintedBy = NULL;
					checkplane->AI.iPaintedByType = -1;
					checkplane->AI.iPaintDist = -1;
				}
			}
			else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
			{
				checkplane->AI.pPaintedBy = NULL;
				checkplane->AI.iPaintedByType = -1;
				checkplane->AI.iPaintDist = -1;
			}

		}
		checkplane ++;
	}

	if(noupdates)
	{
		radardat->lRFlags1 &= ~GD_CHECK_MISSILES;
	}

	if(foundplane == NULL)
	{
		for(cnt = 0; cnt < 36; cnt ++)
		{
			if(jammods[cnt] < 0.75)
			{
				highalert = 1;
			}
		}

		radardat->Target = NULL;
		if(highalert)
		{
			if(lNoticeTimer >= 0)
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
			}
			else
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK|GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN);
			}
			radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
			radardat->lRFlags1 |= GD_HIGH_ALERT;
//			radardat->lRActionTimer = 10000;  //  10 was 15 seconds until next sweep
			radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
			if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
			{
				radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
			}
			else
			{
				radardat->lRActionTimer = 10000;  //  10 was 15 seconds until next sweep
			}
		}
		else if(radardat->lRFlags1 & GD_HIGH_ALERT)
		{
//			radardat->lRFlags1 &= ~GD_HIGH_ALERT;
//			radardat->lRFlags1 |= GD_MED_ALERT;
			radardat->lRActionTimer = 10000;  //  10 seconds until next sweep
		}
		else if(radardat->lRFlags1 & GD_MED_ALERT)
		{
//			radardat->lRFlags1 &= ~GD_MED_ALERT;
//			radardat->lRFlags1 |= GD_LOW_ALERT;
			radardat->lRActionTimer = 20000;  //  20 seconds was 1 min until next sweep
		}
		else
		{
			radardat->lRActionTimer = 40000;  //  40 seconds was 3 mins until next sweep
		}

		if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			if(radardat->lRActionTimer > 40000)
			{
				radardat->lRActionTimer = 40000;  //  40 seconds was 1 mins until next sweep
			}
		}

		if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
		{
//			radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
//			radardat->lRFlags1 |= GD_HIGH_ALERT;
			radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		}
		return;
	}

	if(nlfoundplane != NULL)
	{
		foundplane = nlfoundplane;
		foundrange = nlfoundrange;
	}

	foundweapon = 0;

	int maxnum = 3;
	int ignoreweap = 0;
	int targdistnm = foundrange * WUTONM;
	double our_dist;

	if((!MultiPlayer) || (lAINetFlags1 & NGAI_ACTIVE))
	{
		for(cnt = 0; cnt < 2; cnt ++)
		{
			wradardat = &radarsite->RadarWeaponData[cnt];
			if(wradardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
			{
				if(wradardat->Target != radardat->Target)
					ignoreweap = 1;
			}
		}

		//***  Don't shoot at escortable planes (weapons hold) or neutral planes that are not weapons free.
		if((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[grndside]))
		{
			ignoreweap = 1;
		}
		else if((foundplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2))
		{
			ignoreweap = 1;
		}

		for(cnt = 0; ((cnt < 2) && (!ignoreweap)); cnt ++)
		{
			wradardat = &radarsite->RadarWeaponData[cnt];
			if(!cnt)
			{
				weapon = get_weapon_ptr(pDBVehicleList[radarsite->iVDBIndex].lWeaponType);
			}
			else
			{
				weapon = get_weapon_ptr(pDBVehicleList[radarsite->iVDBIndex].lWeaponType2);
			}

			if(!weapon)
			{
				continue;
			}
			if((radarsite->iShipType) && (!VGDCheckVWeapCanFire(radarsite, wradardat, radardat, TARGET_PLANE, foundplane)))
			{
				continue;
			}
			else if(wradardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
			{
				if(wradardat->Target != radardat->Target)
					break;
			}
	//		else if(radarsite->Status & VL_SURFACE_ATTACK)
	//		{
	//		}
			else if(weapon->iWeaponType == WEAPON_TYPE_GUN)
			{
				if(((float)(weapon->iRange<<2) > targdistnm) && (weapon->fMinRange <= targdistnm) && (((foundplane->WorldPosition.Y - radarsite->WorldPosition.Y) * WUTOFT) < weapon->iMaxAlt))
				{
					ignoreweap = 0;
					if((wradardat->lWFlags1 & GD_W_FIRING) && (!(wradardat->lWFlags1 & (GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET))))
					{
						if(wradardat->Target != radardat->Target)
							ignoreweap = 1;
					}
					if((!(wradardat->lRFlags1 & (GD_I_AM_DEAD ))) && (wradardat->lWNumberFired < weapon->iBurstRate) && (ignoreweap == 0))
					{
						our_dist = foundplane->WorldPosition - radarsite->WorldPosition;
//						if((float)(weapon->iRange<<1) >= targdistnm)
						if((float)(weapon->iRange + 1) >= targdistnm)
						{
							if(GDNumAAAFiringAtPlane(foundplane) <= maxnum)
							{
  								VGDSetUpWeaponTargeting(radarsite, foundplane, foundrange, wradardat, radardat);
								break;
							}
	//						else if(!(wradardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))
	//						{
	//							wradardat->lWFlags1 |= GD_W_PREP_RANDOM;
	//							if(wradardat->lWActionTimer < 0)
	//							{
	//								wradardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
	//							}
	//							GDNoticeAAAFire(&radarsite->WorldPosition, radarsite->iSide);
	//						}
						}
	//					else if(!(wradardat->lWFlags1 & (GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM)))
	//					{
	//						wradardat->lWFlags1 |= GD_W_PREP_RANDOM;
	//						if(wradardat->lWActionTimer < 0)
	//						{
	//							wradardat->lWActionTimer = (rand() & 0x7FFF);  //  32 seconds
	//						}
	//						GDNoticeAAAFire(&radarsite->WorldPosition, radarsite->iSide);
	//					}
					}
				}
			}
			else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
			{
				if((GDNumLaunchingAtPlane(foundplane) + AINumMissilesAtPlane(foundplane)) < 3)
				{
					if(((float)weapon->iRange >= targdistnm) && (weapon->fMinRange <= targdistnm) && (((foundplane->WorldPosition.Y - radarsite->WorldPosition.Y) * WUTOFT) < weapon->iMaxAlt))
					{
						if(!((wradardat->lRFlags1 & GD_RADAR_LOCK) && (!((wradardat->lRFlags1 & GD_BURST_MODE) && (wradardat->pRBurstTarget == foundplane)))))
						{
							if((!(wradardat->lRFlags1 & (GD_I_AM_DEAD))) && (wradardat->lWNumberFired < weapon->iBurstRate) && (wradardat->Target == NULL) && (wradardat->lWActionTimer < 0) && (ignoreweap == 0))
							{
			  					VGDSetUpWeaponTargeting(radarsite, foundplane, foundrange, wradardat, radardat);
								break;
							}
						}
					}
				}
			}
			ignoreweap = 0;
		}
	}

	if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
	{
//		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
//		radardat->lRFlags1 |= GD_HIGH_ALERT;
		radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else if(foundweapon && (!(radardat->lRFlags1 & GD_HIGH_ALERT)))
	{
		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_HIGH_ALERT;
		if(weapon->iWeaponType == 6)
		{
			if((!(wradardat->lWFlags1 & GD_W_FIRING)) || (wradardat->lWFlags1 & (GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET)))
			{
				radardat->lRActionTimer = 5000;  //  5 seconds until next sweep
			}
			else
			{
				radardat->lRActionTimer = 10000;  //  10 was 15 seconds until next sweep
			}
		}
		else
		{
			radardat->lRActionTimer = 10000;  //  10 was 15 seconds until next sweep
		}
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else if((radardat->lRFlags1 & GD_HIGH_ALERT) || (foundplane))
	{
		radardat->lRActionTimer = 10000;  //  10 was 15 seconds until next sweep
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else
	{
		radardat->lRFlags1 &= ~(GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_MED_ALERT;
		radardat->lRActionTimer = 20000;  //  20 seconds was 1 min until next sweep
		radardat->lRAlertTimer = 600000;	//  Med Alert for 10 minutes
	}

	if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
	{
		if(radardat->lRActionTimer > 40000)
		{
			radardat->lRActionTimer = 40000;  //  40 seconds was 1 mins until next sweep
		}
	}

	if(unpaused)
	{
		UnPauseFromDiskHit();
	}
}

//**************************************************************************************
int VGDSetUpWeaponTargeting(MovingVehicleParams *foundweapon, PlaneParams *planepnt, float currdist, GDRadarData *radardat, GDRadarData *rradardat)
{
	DBRadarType *radar;
	DBRadarType *sradar;
	DBWeaponType *weapon;
	int isaaa;
	int tempnum;
	int radardatnum = 0;
	int rradardatnum = 0;
	float tdistnm = currdist * WUTONM;
	long lsecstolaunch;

	radardat->Target = planepnt;
	radardat->iTargetDist = currdist * WUTONM;
	lsecstolaunch = radardat->lWActionTimer = 7000;	//  7 seconds to acquire target;
	radardat->lWFlags1 &= ~(GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE);
	radardat->lWFlags1 |= GD_W_ACQUIRING;
	radardatnum = radardat - foundweapon->RadarWeaponData;
	rradardatnum = rradardat - foundweapon->RadarWeaponData;

	if (!foundweapon->iShipType) // DONE XSCOTT FIX
	{
		radar = GetRadarPtr(pDBVehicleList[foundweapon->iVDBIndex].lRadarID);
		if(!radardatnum)
		{
			weapon = get_weapon_ptr(pDBVehicleList[foundweapon->iVDBIndex].lWeaponType);
		}
		else
		{
			weapon = get_weapon_ptr(pDBVehicleList[foundweapon->iVDBIndex].lWeaponType2);
		}
	}
	else
	{
		radar = GetRadarPtr(pDBShipList[foundweapon->iVDBIndex].DefenseList[rradardatnum].lTypeID);
		weapon = get_weapon_ptr(pDBShipList[foundweapon->iVDBIndex].DefenseList[radardatnum].lTypeID);
	}

	if((weapon->iSeekerType == 8) || (weapon->iSeekerType == 9) || (weapon->iSeekerType == 10) || (weapon->iWeaponType == 6))
	{
		isaaa = 1;
		radardat->lWFlags1 &= ~GD_W_ACQUIRING;
	}
	else if((weapon->iSeekerType != 1) && (weapon->iSeekerType != 7))
	{
		isaaa = 0;
	}
	else
	{
		isaaa = 0;
	}

	if(planepnt->AI.pPaintedBy == NULL)
	{
		planepnt->AI.pPaintedBy = (BasicInstance *)foundweapon;
		planepnt->AI.iPaintedByType = MOVINGVEHICLE;
		planepnt->AI.iPaintDist = currdist * WUTONM;
	}
	else if(planepnt->AI.iPaintDist > (currdist * WUTONM))
	{
		planepnt->AI.pPaintedBy = (BasicInstance *)foundweapon;
		planepnt->AI.iPaintedByType = MOVINGVEHICLE;
		planepnt->AI.iPaintDist = currdist * WUTONM;
	}

	if(!foundweapon->iShipType)
	{
//		if(weapon->iSeekerType == 1)
		if(!(rradardat->lRFlags1 & (GD_RADAR_LOCK|GD_CHECK_MISSILES)))
		{
			rradardat->lRActionTimer = lsecstolaunch + 1000;  //  radardat->lWActionTimer + 1000;
		}
		else if(rradardat->lRActionTimer > 3000)
		{
			rradardat->lRActionTimer = 3000;
		}

		if(radar)
		{
			if(radar->iRadarType & RADAR_TYPE_TWS)
			{
				rradardat->lRFlags1 |= GD_CHECK_MISSILES;
			}
			else  //   if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
			{
				rradardat->lRFlags1 |= GD_RADAR_LOCK;
				GDCheckForPingReport(foundweapon, planepnt, NULL, MOVINGVEHICLE);
			}

			//  Might want to check later to see if redundant.
			rradardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
			rradardat->lRFlags1 |= GD_HIGH_ALERT;
			rradardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
			if(planepnt == PlayerPlane)
			{
				rradardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
			}

			if((rradardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
			{
				rradardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
				rradardat->lRFlags1 |= GD_RADAR_TEMP_ON;
			}
			if(rradardat->lRFlags1 & GD_RADAR_TEMP_ON)
			{
				rradardat->lRGenericTimer = 300000;
			}
		}

		rradardat->Target = planepnt;
	}
	else
	{
#if 0
		if(radar)
		{
			if(radar->iRadarType & RADAR_TYPE_TWS)
			{
				rradardat->lRFlags1 |= GD_CHECK_MISSILES;
			}
			else  //   if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
			{
				rradardat->lRFlags1 |= GD_RADAR_LOCK;
				GDCheckForPingReport(foundweapon, planepnt, NULL, MOVINGVEHICLE);
			}
		}
#endif
		rradardat = NULL;   //  Points to self in case we don't find a valid radar.
		rradardatnum = radardatnum;
		for(int slotnum = 0; slotnum < pDBShipList[foundweapon->iVDBIndex].iNumDefenseItems; slotnum ++)
		{
			if(pDBShipList[foundweapon->iVDBIndex].DefenseList[slotnum].lDefenseType == VS_DEFENSE_WEAPON)
			{
				continue;
			}

			sradar = GetRadarPtr(pDBShipList[foundweapon->iVDBIndex].DefenseList[slotnum].lTypeID);

			if(sradar && (sradar->iRadarType & (RADAR_TYPE_TWS | RADAR_TYPE_STT)))
			{
				if((sradar->iMaxRange >= tdistnm) && ((!(foundweapon->RadarWeaponData[slotnum].lRFlags1 & GD_RADAR_LOCK)) || (foundweapon->RadarWeaponData[slotnum].Target == (void *)planepnt)))
				{
					if(AICheckRadarCrossSig(foundweapon, planepnt, (planepnt->WorldPosition.Y - foundweapon->WorldPosition.Y) * WUTOFT, tdistnm, sradar, (!(sradar->iRadarType & RADAR_TYPE_TWS))))
					{
						rradardat = &foundweapon->RadarWeaponData[slotnum];
						rradardatnum = slotnum;
						if(foundweapon->RadarWeaponData[slotnum].lRFlags1 & GD_RADAR_LOCK)
							break;
					}
				}
			}
		}

		if(!rradardat)
		{
			radardat->lWActionTimer = -1;
			radardat->lWFlags1 &= ~(GD_W_ACQUIRING);
//			rradardat = radardat;   //  Points to self in case we don't find a valid radar.
		}
		else
		{
			if(!(rradardat->lRFlags1 & (GD_RADAR_LOCK|GD_CHECK_MISSILES)))
			{
				rradardat->lRActionTimer = lsecstolaunch + 2000;  //  radardat->lWActionTimer + 2000;
			}
			else if(rradardat->lRActionTimer > 3000)
			{
				rradardat->lRActionTimer = 3000;
			}
			if(sradar)
			{
				if(sradar->iRadarType & RADAR_TYPE_TWS)
				{
					rradardat->lRFlags1 |= GD_CHECK_MISSILES;
				}
				else  //   if((weapon->iSeekerType == 7) || (weapon->iSeekerType == 8) || (weapon->iSeekerType == 9))
				{
					rradardat->lRFlags1 |= GD_RADAR_LOCK;
					GDCheckForPingReport(foundweapon, planepnt, NULL, MOVINGVEHICLE);
				}

				//  Might want to check later to see if redundant.
				rradardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
				rradardat->lRFlags1 |= GD_HIGH_ALERT;
				rradardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
				if(planepnt == PlayerPlane)
				{
					rradardat->lRFlags1 |= GD_RADAR_PLAYER_PING;
				}

				if((rradardat->lRFlags1 & GD_RADAR_TEMP_OFF) || (!(rradardat->lRFlags1 & (GD_RADAR_TEMP_ON|GD_RADAR_ON))))
				{
					rradardat->lRFlags1 &= ~GD_RADAR_TEMP_OFF;
					rradardat->lRFlags1 |= GD_RADAR_TEMP_ON;
				}
				if(rradardat->lRFlags1 & GD_RADAR_TEMP_ON)
				{
					rradardat->lRGenericTimer = 300000;
				}
			}
			rradardat->Target = planepnt;
		}
	}
	radardat->pWRadarSite = rradardat;

	if(MultiPlayer)
	{
		NetPutVehicleGroundLock(foundweapon - MovingVehicles, radardatnum, rradardatnum, planepnt);
	}

	if(isaaa)
	{
		GDFireInitialAAA(foundweapon, planepnt, MOVINGVEHICLE, radardatnum);
	}
	else if((!(radardat->lRFlags1 & GD_BURST_MODE)) && (radar))
	{
		if(radar)
		{
			if(radar->iSearchRate > 1)
			{
				tempnum = rand() & 127;
				tempnum = ((tempnum * radar->iSearchRate) / 127);
				if(tempnum)
				{
					radardat->iRBurstCounter = tempnum;
					radardat->lRBurstTimer = 4000 + ((rand() & 3) * 1000);
					radardat->pRBurstTarget = planepnt;
					radardat->lRFlags1 |= GD_BURST_MODE;
				}
			}
		}
	}
	return(1);
}

//**************************************************************************************
void VGDCheckDefenses(MovingVehicleParams *vehiclepnt)
{
	GDRadarData *radardat;
	float currdist, tdist;
	PlaneParams *planepnt;
	float dx, dz;
	int reloading;
	int slotnum;
	void *watchtarget = NULL;
	int	fxindex = -1;
	BasicInstance *walker;
	MovingVehicleParams *tvehiclepnt;
	GDRadarData *rradardat;
	FPoint position;
	float offangle;
	BYTE bdesiredangle;
	DBWeaponType *weapon=NULL;

	for(slotnum = 0; slotnum < NUM_DEFENSE_LIST_ITEMS; slotnum ++)
	{
		radardat = &vehiclepnt->RadarWeaponData[slotnum];

		if(lNoticeTimer < 0)
		{
			radardat->lRFlags1 &= ~(GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN);
		}
		reloading = 0;
		if(radardat->lWReloadTimer >= 0)
		{
			reloading = 1;
			radardat->lWReloadTimer -= DeltaTicks;
			if(radardat->lWReloadTimer < 0)
			{
				if(vehiclepnt->iShipType)
				{
					weapon = VGDGetShipWeaponForRDNum(vehiclepnt, slotnum);
				}
				else
				{
					if(slotnum == 0)
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
					else
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
				}

				if(!weapon)
				{
					radardat->lWNumberFired = 0;
				}
				else
				{
					if(weapon->iWeaponType != WEAPON_TYPE_GUN)
					{
						radardat->lWNumberFired = 0;
					}
					else if(radardat->lWNumberFired >= weapon->iBurstRate)
					{
						radardat->lWNumberFired = 0;
					}
				}

				radardat->lWNumberFired = 0;
			}
		}
		if(radardat->lWGenericTimer >= 0)
		{
			radardat->lWGenericTimer -= DeltaTicks;

			if(radardat->lWFlags1 & (GD_W_STREAM))
			{
				if(radardat->lWGenericTimer > 3000)
				{
					radardat->lWGenericTimer = 3000;
				}
			}
		}
		if((!reloading) && (!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && ((vehiclepnt->Status & VL_SURFACE_ATTACK) || (radardat->lWFlags1 & (GD_W_ACQUIRING|GD_W_FIRING|GD_W_PREP_RANDOM))))
		{
			watchtarget = NULL;
			fxindex = -1;
			if(vehiclepnt->iShipType)
			{
			}
			else if(slotnum == 0)
			{
				fxindex = 1;
				if(radardat->Target)
				{
					watchtarget = radardat->Target;
				}
				else if(radardat->pRBurstTarget)
				{
					watchtarget = radardat->pRBurstTarget;
				}
				else if(radardat->pWRadarSite)
				{
					rradardat = (GDRadarData *)radardat->pWRadarSite;
					watchtarget = rradardat->Target;
				}
				else
				{
					watchtarget = NULL;
				}
			}

			if((fxindex != -1) && (watchtarget))
			{
				if((watchtarget >= Planes) && (watchtarget <= LastPlane))
				{
					planepnt = (PlaneParams *)watchtarget;
					position = planepnt->WorldPosition;
				}
				else if((watchtarget >= MovingVehicles) && (watchtarget <= LastMovingVehicle))
				{
					tvehiclepnt = (MovingVehicleParams *)watchtarget;
					position = tvehiclepnt->WorldPosition;
				}
				else
				{
					walker = (BasicInstance *)watchtarget;
					position = walker->Position;
				}

				dx = position.X - vehiclepnt->WorldPosition.X;
				dz = position.Z - vehiclepnt->WorldPosition.Z;

		  		offangle = atan2(-dx, -dz) * 57.2958;

				offangle -= AIConvertAngleTo180Degree(vehiclepnt->Heading);

				offangle = AICapAngle(offangle);

				while(offangle < 0)
				{
					offangle += 180;
				}

				offangle = (offangle / 360.0f) * 255.0f;

				bdesiredangle = offangle;
				bdesiredangle += 128;
				vehiclepnt->bFXDesiredArray[fxindex] = bdesiredangle;
			}

			if(radardat->lWActionTimer >= 0)
			{
				radardat->lWActionTimer -= DeltaTicks;
				if(radardat->lWActionTimer < 0)
				{
					VAIGroundWeaponAction(vehiclepnt, radardat);
				}
				else if(radardat->lWFlags1 & (GD_W_SINGLE|GD_W_BURST|GD_W_STREAM))
				{
					GDUpdateImpactPointLite(vehiclepnt, MOVINGVEHICLE, slotnum);
				}
			}
			else
			{
				radardat->lWFlags1 &= ~(GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM);
			}
		}
		else if((radardat->lWActionTimer >= 0) && (!reloading))
		{
			radardat->lWActionTimer -= DeltaTicks;
		}

		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF|GD_VISUAL_SEARCH)))
		{
			if(radardat->lRActionTimer >= 0)
			{
				if(radardat->lRFlags1 & GD_RADAR_LOCK)
				{
					AICheckForGateStealerV(vehiclepnt, radardat, (PlaneParams *)radardat->Target);
				}

				radardat->lRActionTimer -= DeltaTicks;
				if(radardat->lRActionTimer < 0)
				{
					AISimpleVehicleRadar(vehiclepnt, radardat);
				}
			}
			if(radardat->lRAlertTimer >= 0)
			{
				radardat->lRAlertTimer -= DeltaTicks;
				if(radardat->lRAlertTimer < 0)
				{
					if(radardat->lRFlags1 & GD_HIGH_ALERT)
					{
						radardat->lRFlags1 &= ~GD_HIGH_ALERT;
						radardat->lRFlags1 |= GD_MED_ALERT;
						radardat->lRAlertTimer = 600000;	//  10 minutes at Med alert
					}
					else if(radardat->lRFlags1 & GD_MED_ALERT)
					{
						radardat->lRFlags1 &= ~GD_MED_ALERT;
						radardat->lRFlags1 |= GD_LOW_ALERT;
					}
					else
					{
						radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_HIGH_ALERT);
						radardat->lRFlags1 |= GD_LOW_ALERT;
					}
				}
			}
			if(radardat->lRBurstTimer >= 0)
			{
				radardat->lRBurstTimer -= DeltaTicks;
				if(radardat->lRBurstTimer < 0)
				{
					if((radardat->iRBurstCounter) && (radardat->pRBurstTarget))
					{
						planepnt = (PlaneParams *)radardat->pRBurstTarget;
						if(((iInJump) && ((planepnt == PlayerPlane) || (planepnt->AI.iAIFlags1 & AIPLAYERGROUP))) || (planepnt->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED)))
						{
							if(!(radardat->lRFlags1 & GD_KEEP_BURST_TARGET))
							{
								radardat->pRBurstTarget = NULL;
							}
						}
						else
						{
							dx = planepnt->WorldPosition.X - vehiclepnt->WorldPosition.X;
							dz = planepnt->WorldPosition.Z - vehiclepnt->WorldPosition.Z;
							tdist = QuickDistance(dx, dz);

							currdist = -1;
							if(!VGDSetUpWeaponTargeting(vehiclepnt, planepnt, currdist, radardat, radardat))
							{
								if(!(radardat->lRFlags1 & GD_KEEP_BURST_TARGET))
								{
									radardat->pRBurstTarget = NULL;
								}
							}

							radardat->iRBurstCounter = radardat->iRBurstCounter - 1;
							radardat->lRBurstTimer = 4000 + ((rand() & 3) * 1000);
						}
					}
					if((radardat->iRBurstCounter <= 0) || (radardat->pRBurstTarget == NULL))
					{
						radardat->iRBurstCounter = -1;
						radardat->lRFlags1 &= ~(GD_BURST_MODE);
						radardat->lWFlags1 &= ~(GD_W_FIRING);
						radardat->pRBurstTarget = NULL;
					}
				}
			}
		}
	}
}

//**************************************************************************************
void VAIGroundWeaponAction(MovingVehicleParams *currweapon, GDRadarData *radardat)
{
	DBWeaponType *weapon = NULL;
	void *surfacetarget;
	int surfacetype;
	FPoint tpos;
	DBRadarType *radar;
	int tempnum;
	int radardatnum = radardat - currweapon->RadarWeaponData;
	int surfaceOK = 0;
	int weapontype;
	float fworkvar;
	float launchchance = 100.0f;
	float randchance;

	switch(currweapon->bDamageLevel)
	{
		case 0:
			launchchance = 100.0f;
			break;
		case 1:
			launchchance = 50.0f;
			break;
		case 2:
			launchchance = 0.0f;
			break;
		case 3:
		default:
//			launchchance = 0.0f;
			return;
			break;
	}

	if(currweapon->Status & VL_SURFACE_ATTACK)
	{
		if(currweapon->iShipType)
		{
			if(pDBShipList[currweapon->iVDBIndex].DefenseList[radardatnum].lDefenseType != VS_DEFENSE_RADAR)
			{
				weapon = &pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex];
			}
		}
		else
		{
			weapon = &pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex];  // DONE XSCOTT FIX THIS
		}

		if(weapon)
		{
			if((weapon->iWeaponType == WEAPON_TYPE_AG_MISSILE) || (weapon->iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE) ||
								((weapon->iWeaponType == WEAPON_TYPE_GUN) && ((weapon->iUsageIndex == 11) || (weapon->iUsageIndex == 12))))
			{
				surfaceOK = 1;
			}
		}
	}

	if((currweapon->Status & VL_SURFACE_ATTACK) && (surfaceOK))
	{
		if(radardat->lRFlags1 & GD_BURST_MODE)
		{
			surfacetarget = radardat->pRBurstTarget;
			surfacetype = radardat->lTimeDiff;  //  OK, OK, so the name doesn't make sense.  It is only used by AAA weapons, which this isn't.
		}
		else
		{
			surfacetarget = VCheckForSurfaceAttack(currweapon, &surfacetype, radardat);
		}

		if(surfacetarget)
		{
			if (!currweapon->iShipType) // SCOTT FIX
				radar = GetRadarPtr(pDBVehicleList[currweapon->iVDBIndex].lRadarID);
			else
			{
				radar = NULL;  //  For now,  SCOTT FIX
			}

			tpos.X = tpos.Y = tpos.Z = -1.0f;
			weapontype = pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex].iWeaponType;
			if(weapontype == WEAPON_TYPE_GUN)
			{
				InstantiateCannon(currweapon, MOVINGVEHICLE, surfacetarget, surfacetype, tpos, -1, (radardat - currweapon->RadarWeaponData));
			}
			else
			{
				randchance = frand() * 100;

				if(randchance < launchchance)
				{
					InstantiateSSMissile(currweapon, MOVINGVEHICLE, surfacetarget, surfacetype, tpos, -1, (radardat - currweapon->RadarWeaponData));
				}
			}

			radardat->lWNumberFired = radardat->lWNumberFired + 1;
			weapon = &pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex];  // SCOTT FIX THIS
//			if (!currweapon->iShipType)	// SCOTT FIX
//				weapon = get_weapon_ptr(pDBVehicleList[currweapon->iVDBIndex].lWeaponType);
//			else
//				return;

			if(radardat->lWNumberFired >= weapon->iBurstRate)
			{
				if(weapontype == WEAPON_TYPE_GUN)
				{
					radardat->lWReloadTimer = 4 * 60000;
				}
				else
				{
					radardat->lWReloadTimer = weapon->iRateOfFire * 60000;
				}
				radardat->iRBurstCounter = -1;
				radardat->lRFlags1 &= ~(GD_BURST_MODE|GD_KEEP_BURST_TARGET);
				radardat->pRBurstTarget = NULL;
//				currweapon->RadarWeaponData->lWActionTimer = 90000 + rand();  // SCOTT FIX THIS
				radardat->lWActionTimer = 90000 + rand();  // SHOULD BE OK XSCOTT FIX THIS
			}
			else
			{
				if(radardat->iRBurstCounter > 0)
				{
					radardat->iRBurstCounter = radardat->iRBurstCounter - 1;
//					currweapon->RadarWeaponData->lWActionTimer = 4000 + ((rand() & 3) * 1000);  // SCOTT FIX THIS
					radardat->lWActionTimer = 4000 + ((rand() & 3) * 1000);  // SHOULD BE OK XSCOTT FIX THIS
				}
				else
				{
					if(radar)
					{
						tempnum = rand() & 127;
						tempnum = ((tempnum * radar->iSearchRate) / 128);
					}
					else
					{
						tempnum = 0;
						radardat->iRBurstCounter = 0;
						radardat->pRBurstTarget = NULL;
					}

					if(tempnum)
					{
						radardat->iRBurstCounter = tempnum;
						radardat->pRBurstTarget = surfacetarget;
						radardat->lTimeDiff = surfacetype;
						radardat->lRFlags1 |= (GD_BURST_MODE|GD_KEEP_BURST_TARGET);
//						currweapon->RadarWeaponData->lWActionTimer = 4000 + ((rand() & 3) * 1000);  // SCOTT FIX THIS
						if(weapontype == WEAPON_TYPE_GUN)
						{
							fworkvar = (60.0f / (float)pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex].iRateOfFire) * 1000.0f;
							radardat->lWActionTimer = fworkvar;
						}
						else
						{
							radardat->lWActionTimer = 4000 + ((rand() & 3) * 1000);  // SHOULD BE OK XSCOTT FIX THIS
						}
					}
					else if(weapontype == WEAPON_TYPE_GUN)
					{
						fworkvar = (60.0f / (float)pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex].iRateOfFire) * 1000.0f;
						radardat->lWActionTimer = fworkvar;
					}
					else
					{
//						currweapon->RadarWeaponData->lWActionTimer = 90000 + rand();  // SCOTT FIX THIS
						radardat->lWActionTimer = 90000 + rand();  // SHOULD BE OK XSCOTT FIX THIS
					}
				}
				if((radardat->iRBurstCounter <= 0) || (radardat->pRBurstTarget == NULL))
				{
					radardat->iRBurstCounter = -1;
					radardat->lRFlags1 &= ~(GD_BURST_MODE|GD_KEEP_BURST_TARGET);
					radardat->pRBurstTarget = NULL;
//					currweapon->RadarWeaponData->lWActionTimer = 90000 + rand();  // SCOTT FIX THIS
					if(radardat->lWActionTimer < 0)
						radardat->lWActionTimer = 90000 + rand();  // SHOULD BE OK XSCOTT FIX THIS
				}
			}
		}
		else
		{
			radardat->iRBurstCounter = -1;
			radardat->lRFlags1 &= ~(GD_BURST_MODE|GD_KEEP_BURST_TARGET);
			radardat->pRBurstTarget = NULL;
//			currweapon->RadarWeaponData->lWActionTimer = 90000 + rand();  // SCOTT FIX THIS
			radardat->lWActionTimer = 90000 + rand();  // SHOULD BE OK XSCOTT FIX THIS
		}
	}

	if(radardat->lWFlags1 & GD_W_PREP_RANDOM)
	{
		GDSetUpRandomAAA(currweapon, 1, MOVINGVEHICLE);
	}
	else if(radardat->lWFlags1 & (GD_W_SINGLE|GD_W_BURST|GD_W_STREAM))
	{
		GDUpdateImpactPoint(currweapon, MOVINGVEHICLE, (radardat - currweapon->RadarWeaponData));
		GDFireAAA(currweapon, MOVINGVEHICLE, (radardat - currweapon->RadarWeaponData));
		if((radardat->lWGenericTimer < 0) || (radardat->Target == NULL))
		{
			radardat->lWGenericTimer = -1;
			if(radardat->lWFlags1 & (GD_W_STREAM))
			{
				radardat->lWReloadTimer = 4000 + (rand() & 0x7FF);
			}

			radardat->lWFlags1 &= ~(GD_W_FIRING|GD_W_RANDOM_FIRE|GD_W_PREP_RANDOM|GD_W_SINGLE|GD_W_BURST|GD_W_STREAM);
			if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)))
			{
				radardat->Target = NULL;
			}
		}
	}
	else if(radardat->lWFlags1 & (GD_W_ACQUIRING))
	{
		randchance = frand() * 100;

		if(randchance < launchchance)
		{
			if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
			{
				if(currweapon == &MovingVehicles[PlayerPlane->AI.iHomeBaseId])
				{
					if(lCarrierBirdsAfirmTimer < 0)
					{
						lCarrierBirdsAfirmTimer = 90000;
						int irandval;
						if(lCarrierMsgFlags & BIRDS_AFIRM_MSG)
						{
							irandval = rand() % 3;
						}
						else
						{
							lCarrierMsgFlags |= BIRDS_AFIRM_MSG;
							irandval = rand() & 0x1;
						}
						switch(irandval)
						{
							case 1:
								AICJustMessage(AIC_STRIKE_INBOUND_VAMPIRES, AICF_STRIKE_INBOUND_VAMPIRES, SPCH_STRIKE);
								break;
							case 2:
								AICJustMessage(AIC_STRIKE_BIRDS_AWAY, AICF_STRIKE_BIRDS_AWAY, SPCH_STRIKE);
								break;
							default:
								AICJustMessage(AIC_STIKE_INBOUND_HOSTILES, AICF_STIKE_INBOUND_HOSTILES, SPCH_STRIKE);
								break;
						}
					}
				}
			}

			InstantiateMissile(NULL, 0, currweapon, MOVINGVEHICLE, NULL, -1, (radardat - currweapon->RadarWeaponData));
			radardat->lWNumberFired = radardat->lWNumberFired + 1;
		}
		radardat->lWFlags1 &= ~(GD_W_ACQUIRING);
		radardat->lWFlags1 |= (GD_W_FIRING);
		if(!(radardat->lRFlags1 & (GD_BURST_MODE)))
		{
			radardat->lWFlags1 &= ~(GD_W_FIRING);
		}

		weapon = &pDBWeaponList[currweapon->RadarWeaponData[radardatnum].iWeaponIndex];

		if(radardat->lWNumberFired >= weapon->iBurstRate)
		{
			if(weapon->iWeaponType == WEAPON_TYPE_GUN)
			{
				radardat->lWReloadTimer = 60000 + (rand() & 0x7FFF);  //  was 1000;
			}
			else
			{
				radardat->lWReloadTimer = weapon->iRateOfFire * 60000;  //  was 1000;
			}
		}
	}
}

//**************************************************************************************
void VClearRadardat(MovingVehicleParams *vehiclepnt)
{
	GDRadarData *radardat=NULL;
	DBRadarType *radar=NULL;
	DBWeaponType *weapon;
	int maxdef = NUM_DEFENSE_LIST_ITEMS;

	if(vehiclepnt->iShipType)
	{
		maxdef = pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems;
	}
	else
	{
		maxdef = 2;
	}

	for(int cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
	{
		radardat = &vehiclepnt->RadarWeaponData[cnt];
		if((vehiclepnt->Status & VL_DESTROYED) || (cnt >= maxdef))
		{
			radardat->lRFlags1 &= ~(GD_RADAR_PLAYER_PING);
			radardat->lRFlags1 |= GD_I_AM_DEAD;
		}
		else
		{
			radardat->iRBurstCounter = 0;
			radardat->lRBurstTimer = -1;
			radardat->pRBurstTarget = NULL;
			radardat->lRGenericTimer = -1;
			radardat->lRLockTimer = -999999;
			radardat->lRFlags1 = GD_HIGH_ALERT | (radardat->lRFlags1 & GD_VISUAL_SEARCH);
			radardat->lRActionTimer = 30000;  //  30 seconds until next sweep
			radardat->lRAlertTimer = 210000;	//  Red Alert for 3.5 minutes
			radardat->iConfidence = 0;
			radardat->lWActionTimer = -1;
			radardat->lWGenericTimer = -1;
			radardat->pWRadarSite = NULL;
			radardat->lWReloadTimer = -1;
			radardat->lWNumberFired = 0;
			radardat->lWFlags1 = 0;
			radardat->Target = NULL;
			radardat->iTargetDist = -1;
			radardat->FireHeadingANG = 0;
			radardat->HeadingOffsetANG = 0;
			radardat->FirePitchANG = 0;
			radardat->PitchOffsetANG = 0;;
			radardat->pAAAStream = 0;
			radardat->fpAimPoint.SetValues(-1.0f, -1.0f, -1.0f);
			radardat->lTimeDiff = 0;
			radardat->lTimeDiffOffset = 0;

			if(!vehiclepnt->iShipType) // DONE XSCOTT FIX
			{
				if(!cnt)
				{
					radar = GetRadarPtr(pDBVehicleList[vehiclepnt->iVDBIndex].lRadarID);
					weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
				}
				else
				{
					weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
					radar = NULL;
				}
			}
			else
			{
				radar = NULL;
				weapon = NULL;
				if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_RADAR)
				{
					radar = GetRadarPtr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID);
				}
				else
				{
					weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID);
				}
			}

			if((radar == NULL) && (weapon == NULL))
			{
				radardat->lRFlags1 &= ~(GD_RADAR_PLAYER_PING);
				radardat->lRFlags1 |= GD_I_AM_DEAD;
			}
			else if(radar != NULL)
			{
				radardat->lRFlags1 = GD_HIGH_ALERT | (radardat->lRFlags1 & GD_VISUAL_SEARCH);
				radardat->lRActionTimer = 30000 + (rand() & 0x3FFF);  //  30 seconds until next sweep
				radardat->lWActionTimer = radardat->lRActionTimer;  //  30 seconds until next sweep
				radardat->lRAlertTimer = 210000;	//  Red Alert for 3.5 minutes
				radardat->lRGenericTimer = -1;
				radardat->lRLockTimer = -999999;
				radardat->lRFlags1 = (GD_RADAR_ON|GD_HIGH_ALERT);
				radardat->iConfidence = 0;
			}
			else
			{
				radardat->lRFlags1 = GD_HIGH_ALERT | (radardat->lRFlags1 & GD_VISUAL_SEARCH);
				radardat->lRLockTimer = -999999;
				radardat->lRActionTimer = 30000 + (rand() & 0x3FFF);  //  30 seconds until next sweep
				radardat->lWActionTimer = radardat->lRActionTimer;  //  30 seconds until next sweep
				radardat->lRAlertTimer = 210000;	//  Red Alert for 3.5 minutes
				radardat->lRGenericTimer = -1;
			}
		}
	}
}

//**************************************************************************************
void *VCheckForSurfaceAttack(MovingVehicleParams *vehiclepnt, int *returntype, GDRadarData *radardat)
{
  	BasicInstance *checkobject;
	float foundrange;
  	void *foundobject = NULL;
	int foundtype = 0;
	float dx, dz, tdist;
	int pass;
	int searchside;
	float distnm;
	MovingVehicleParams *checkvehicle;
//	DBWeaponType *weapon;
	int radardatnum = radardat - vehiclepnt->RadarWeaponData;
	int weapontype;
	float radarrangenm = 0;
	float temprange;
	int cnt;
	DBRadarType *radar;
	int radarid;
	int max_at_one_time = 3;
	FPoint attack_area;
	BasicInstance *attack_object = NULL;
	MovingVehicleParams *attack_vehicle = NULL;
	float searchrange;
	int iscannon = 0;
	int usage = 0;
	int shipsonly = 0;
	float minrangewu = 0;

	attack_area.SetValues(-1.0f, -1.0f, -1.0f);

	if(vehiclepnt->iShipType)
	{
		if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lDefenseType == VS_DEFENSE_RADAR)
			return(NULL);
	}

	weapontype = pDBWeaponList[vehiclepnt->RadarWeaponData[radardatnum].iWeaponIndex].iWeaponType;  // DONE XSCOTT FIX THIS
	if((weapontype != WEAPON_TYPE_AG_MISSILE) && (weapontype != WEAPON_TYPE_ANTI_SHIP_MISSILE))
	{
		usage = pDBWeaponList[vehiclepnt->RadarWeaponData[radardatnum].iWeaponIndex].iUsageIndex;
		if(weapontype != WEAPON_TYPE_GUN)
			return(NULL);
		else if((usage != 11) && (usage != 12))  //  Not general ground or close air/ground
			return(NULL);
		iscannon = 1;
	}

	if(weapontype == WEAPON_TYPE_ANTI_SHIP_MISSILE)
	{
		shipsonly = 1;
	}

	if((iscannon) && (usage == 11))
	{
		distnm = FTTONM * (float)pDBWeaponList[vehiclepnt->RadarWeaponData[radardatnum].iWeaponIndex].iRange;
	}
	else
	{
		distnm = pDBWeaponList[vehiclepnt->RadarWeaponData[radardatnum].iWeaponIndex].iRange;  // DONE XSCOTT FIX THIS
		minrangewu = pDBWeaponList[vehiclepnt->RadarWeaponData[radardatnum].iWeaponIndex].fMinRange * NMTOWU;
	}

	//  Do check for SS radar here eventually
	if(vehiclepnt->iShipType)
	{
		for(cnt = 0; cnt < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; cnt ++)
		{
			if(radardatnum == cnt)
				continue;

			if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_RADAR)
			{
//				if(!(radardat->lRFlags1 & (GD_I_AM_DEAD )))
// FOR JAY TO LOOK AT				if((!(vehiclepnt->RadarWeaponData[cnt].lRFlags1 & (GD_I_AM_DEAD))) && (vehiclepnt->RadarWeaponData[cnt].lRFlags1 & (GD_RADAR_PLAYER_PING)))
				if(!(vehiclepnt->RadarWeaponData[cnt].lRFlags1 & (GD_I_AM_DEAD )))
				{
					radarid = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[cnt].lTypeID;
					radar = GetRadarPtr(radarid);
					if(radar)
					{
						if(!strcmp("SU", radar->sNTEWS))
						{
							temprange = radar->iMaxRange;
							if(temprange > radarrangenm)
							{
								radarrangenm = temprange;
							}
						}
					}
				}
			}
		}

		if(iscannon)
		{
			if(radarrangenm < 20)
			{
				radarrangenm = 20;
			}
		}

		if(radarrangenm == 0)
		{
			return(NULL);
		}

		if(radarrangenm < distnm)
		{
			distnm = radarrangenm;
		}
	}

	searchrange = foundrange = (distnm * NMTOWU);

	searchside = (vehiclepnt->iSide == AI_FRIENDLY) ? AI_ENEMY : AI_FRIENDLY;

	BasicInstance ***checklist = &AllTargetTypes[0];
	pass = NumTargetTypes;

	if(vehiclepnt->pCurrWP->iRouteActionFlags & (VEHICLE_ACTION_ATTACKSPECIFIC|VEHICLE_ACTION_ATTACKAROUNDSPECIFIC))
	{
		if(vehiclepnt->pCurrWP->iRouteActionFlags & VEHICLE_ACTION_GROUNDOBJECT)
		{
			attack_object = FindInstance(AllInstances, vehiclepnt->pCurrWP->iObjectID);
		}
		else if(vehiclepnt->pCurrWP->iRouteActionFlags & VEHICLE_ACTION_MOVINGVEHICLE)
		{
			int vnum;

			vnum = VConvertVGListNumToVNum(vehiclepnt->pCurrWP->iObjectID);
			if(vnum < 0)  //  shouldn't happen,  means bad thing has happened
			{
				attack_vehicle = &MovingVehicles[vehiclepnt->pCurrWP->iObjectID];
			}
			else
			{
				attack_vehicle = &MovingVehicles[vnum];
			}
		}
	}

	while(pass--)
	{
		if(shipsonly)
			break;

		checkobject = **checklist++;

		if((vehiclepnt->pCurrWP->iRouteActionFlags & (VEHICLE_ACTION_ATTACKSPECIFIC|VEHICLE_ACTION_ATTACKALONGPATH)) == VEHICLE_ACTION_ATTACKSPECIFIC)
		{
			checkobject = attack_object;
		}

		while(checkobject)
		{
	//		if ((checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (planepnt->AI.iSide != checkplane->AI.iSide))
			//  if(object is alive and object is a valid target)
			if(InstanceIsBombable(checkobject) && ((GDConvertGrndSide(checkobject) == searchside) || (attack_object == checkobject)))
			{
				dx = (checkobject->Position.X) - vehiclepnt->WorldPosition.X;
				dz = (checkobject->Position.Z) - vehiclepnt->WorldPosition.Z;
				if(((fabs(dx) < foundrange) && (fabs(dz) < foundrange)) || (checkobject == attack_object))
				{
					tdist = QuickDistance(dx, dz);
					if(((tdist < foundrange) || ((checkobject == attack_object) && (tdist < searchrange))) && (tdist >= minrangewu))
					{
						if((checkobject == attack_object) || (attack_area.X < 0.0f))
						{
							tdist = 0;
						}
						else
						{
							dx = (checkobject->Position.X) - attack_area.X;
							dz = (checkobject->Position.Z) - attack_area.Z;

							tdist = QuickDistance(dx, dz);
						}

						if((tdist < (NMTOWU * 0.5f)) && (AISeeIfAlreadyTarget(GROUNDOBJECT, checkobject) < max_at_one_time))
						{
							foundobject = checkobject;
							foundtype = GROUNDOBJECT;
							foundrange = tdist;
							if(checkobject == attack_object)
								break;
						}
					}
				}
			}

			checkobject = checkobject->NextRelatedInstance;

			if((vehiclepnt->pCurrWP->iRouteActionFlags & (VEHICLE_ACTION_ATTACKSPECIFIC|VEHICLE_ACTION_ATTACKALONGPATH)) == VEHICLE_ACTION_ATTACKSPECIFIC)
			{
				checkobject = NULL;
			}
		}
	}

	for(checkvehicle = MovingVehicles; checkvehicle <= LastMovingVehicle; checkvehicle ++)
	{
		if((vehiclepnt->pCurrWP->iRouteActionFlags & (VEHICLE_ACTION_ATTACKSPECIFIC|VEHICLE_ACTION_ATTACKALONGPATH)) == VEHICLE_ACTION_ATTACKSPECIFIC)
		{
			checkvehicle = attack_vehicle;
		}

		if(!checkvehicle)
			break;

		if(((checkvehicle->Status & (VL_ACTIVE | VL_FALLING | VL_DESTROYED|VL_INVISIBLE)) == VL_ACTIVE) && ((searchside == checkvehicle->iSide) || (checkvehicle == attack_vehicle)) && ((checkvehicle->iShipType) || (!shipsonly)))
		{
			dx = (checkvehicle->WorldPosition.X) - vehiclepnt->WorldPosition.X;
			dz = (checkvehicle->WorldPosition.Z) - vehiclepnt->WorldPosition.Z;
			if(((fabs(dx) < foundrange) && (fabs(dz) < foundrange)) || (checkvehicle == attack_vehicle))
			{
				tdist = QuickDistance(dx, dz);
				if(((tdist < foundrange) || ((checkvehicle == attack_vehicle) && (tdist < searchrange))) && (tdist >= minrangewu))
				{
					if((checkvehicle == attack_vehicle) || (attack_area.X < 0.0f))
					{
						tdist = 0;
					}
					else
					{
						dx = (checkvehicle->WorldPosition.X) - attack_area.X;
						dz = (checkvehicle->WorldPosition.Z) - attack_area.Z;

						tdist = QuickDistance(dx, dz);
					}

					if((tdist < (NMTOWU * 0.5f)) && (AISeeIfAlreadyTarget(MOVINGVEHICLE, checkvehicle) < max_at_one_time))
					{
						foundobject = checkvehicle;
						foundtype = MOVINGVEHICLE;
						foundrange = tdist;
						if(checkvehicle == attack_vehicle)
							break;
					}
				}
			}
		}

		if((vehiclepnt->pCurrWP->iRouteActionFlags & (VEHICLE_ACTION_ATTACKSPECIFIC|VEHICLE_ACTION_ATTACKALONGPATH)) == VEHICLE_ACTION_ATTACKSPECIFIC)
		{
			break;
		}
	}

	*returntype = foundtype;
	return(foundobject);
}

//**************************************************************************************
int AISeeIfAlreadyTarget(int targettype, void *targobj)
{
	int converttarget;
	WeaponParams *W = &Weapons[0];
	PlaneParams *planepnt;
	int numat = 0;


	if(targobj == NULL)
	{
		return(0);
	}

	switch(targettype)
	{
		case AIRCRAFT:
			converttarget =  TARGET_PLANE;
			break;
		case GROUNDOBJECT:
			converttarget = TARGET_GROUND;
			break;
		case MOVINGVEHICLE:
			converttarget = TARGET_VEHICLE;
			break;
		default:
			converttarget = -1;
			break;
	}

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(W->iTargetType == converttarget)
			{
				if(W->pTarget == targobj)
				{
					numat ++;
//					return(1);
				}
			}
		}
		W++;
	}

	for(planepnt = Planes; planepnt <= LastPlane; planepnt ++)
	{
		if((planepnt->Status & PL_ACTIVE) && (planepnt->AI.Behaviorfunc == WeaponAsPlane))
		{
			if(targobj == planepnt->AI.pGroundTarget)
			{
				numat ++;
			}
		}
	}

//	return(0);
	return(numat);
}

#define GTIX (GRAVITY*FTTOWU/2500.0)

//**************************************************************************************
int DrawAVehicle(MovingVehicleParams *W, FPoint &relative_position,BYTE *carrierlights)
{
	BYTE vars[64];
	BOOL ret_value;
	FPoint sunken;
	FMatrix mat;
	float pangle;
	float ticks;
	float t;
	FPoint *dest,*source;
	FPoint meatpoint;
	BYTE btempval;
	int isnight = 0;
	long hours = (int)WorldParams.WorldTime/3600;
	long lworkvar;
	float old_line_width;
	float radius;

	if((hours < 6) || (hours > 18))
	{
		isnight = 1;
	}

	MovingVehicleType *type = W->Type;

	//	if (!type->iShipType) then I am a moving vehicle
	//	Special little note: to support the old moving vehicle types, a ship possibly could
	//  exist and the above condition would not be true - If this is the case, then
	//  the !type->iVehicleType will be VEHICLE_TYPE_SHIP.  This is only here to support
	//  old F-15 missions.  This does not have to be supported in F-18.
	dest =NULL;

	if (type->Model)
	{
		*(unsigned long *)&vars[10] = 0xF8FBFDFD;

		if(!W->iShipType)
			*(unsigned short *)&vars[4] = 0xFDF8;

		if (type->iShipType)	// I am a ship
		{
			switch(type->TypeNumber)
			{
				case 0:	// Carrier
					vars[5] = W->bFXArray[2];	//  Blast door 1
					vars[6] = W->bFXArray[3];	//  Blast door 2
					vars[7] = W->bFXArray[4];	//  Blast door 3
					vars[8] = W->bFXArray[5];	//  Blast door 4

					if(isnight)
					{
						lworkvar = GameLoop & 0x3F;
						vars[11] = (lworkvar < 0x10) ? 0xFF : 0;
						vars[12] = ((lworkvar < 0x20) && (lworkvar >= 0x10)) ? 0xFF : 0;
						vars[13] = ((lworkvar < 0x30) && (lworkvar >= 0x20)) ? 255 : 0;
						vars[14] = ((lworkvar < 0x40) && (lworkvar >= 0x30)) ? 255 : 0;
						vars[10] = 0xFF;


						radius = GetObjectRadius(type->Model);
						radius *= radius * 2.0f;
						radius = relative_position.PartialLength() - radius;
						if (radius <= 0.0)
						{
							vars[15] = 0xFF;
							vars[16] = 0xFF;
						}
						else
						{
							radius = 1.0f - (radius *  (1.0f/ ((2.5 * TILE_SIZE) * (2.5 *TILE_SIZE))));

							if (radius < 0.0f)
							{
								vars[15] = 0x0;
								vars[16] = 0x0;
							}
							else
							{
								vars[15] = (BYTE)(radius*0xFF);
								vars[16] = vars[15];
							}
						}


						vars[50] = 0xF8;
						vars[52] = ((GameLoop % 50) < 25) ? 0xF8 : 0;
					}
					else
					{
						vars[15] = 0;
						vars[16] = 0;
						vars[11] = 0;
						vars[12] = 0;
						vars[13] = 0;
						vars[14] = 0;
						vars[50] = 0;
						vars[52] = 0;
					}

					btempval = (BYTE)((43 * GameLoop) / 50);		//  Not sure if is used for radar
//					if(btempval == 0)
//						btempval = 1;
					W->bFXArray[0] = /*W->bFXArray[0] +*/ btempval;
					W->bFXDesiredArray[0] = W->bFXArray[0];
					vars[0] = W->bFXArray[0];

					// Do special carrier animations here (or somplace else, I don't care)
					if (W->CatchWire.Flags & CW_PLANE_CAUGHT)
					{
						dest= W->Type->CatchWires.Wires[W->CatchWire.Flags&CW_WIRE];
						source=&W->Type->CatchWires.Values[W->CatchWire.Flags&CW_WIRE];

						*dest = W->CatchWire.LastPlace;
					}
					else
						if (W->CatchWire.Flags & CW_RETURNING)
						{
							float move;
							float dist;
							ticks = (float)(GameLoop - W->CatchWire.Tick);

							dest= W->Type->CatchWires.Wires[W->CatchWire.Flags&CW_WIRE];
							source=&W->Type->CatchWires.Values[W->CatchWire.Flags&CW_WIRE];

							if (ticks < 250.0f)
							{
								*dest = W->CatchWire.LastPlace;
								dist = dest->Y - source->Y;

								if (dist > 0.0f)
								{
									move = ticks*ticks*0.5*GTIX;
									if (move > dist)
										dest->Y = source->Y;
									else
										dest->Y -= move;
								}
								else
									dest->Y = source->Y;
							}
							else
							{
								ticks = (ticks-250.0f)*(1.0f/500.0f);
								W->CatchWire.LastPlace.Y = source->Y;

								if (ticks < 1.0f)
									dest->RangeVectorToFrom(ticks,*source,W->CatchWire.LastPlace);
								else
									W->CatchWire.Flags = 0;
							}

						}
						if (carrierlights)
							memcpy(&vars[32],&carrierlights[32],17);

					if ((TerrainYesNos & (YN_DRAW_LIGHT_MAPS | YN_DRAW_POINT_LIGHTS)) == (YN_DRAW_LIGHT_MAPS | YN_DRAW_POINT_LIGHTS))
						UseLightMapsOnObjects = TRUE;
				break;

				case 1:		//  Ticonderoga
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];

					vars[10] = W->bFXArray[0];	//  Front Gun
					vars[11] = W->bFXArray[1];	//  Rear Gun
					break;
				case 2:		//  Burke
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					break;
				case 3:		//  Spruance
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					vars[10] = W->bFXArray[0];	//  Front Gun
//					vars[10] = 64;	//  Front Gun
					vars[11] = W->bFXArray[1];	//  Rear Gun
//					vars[11] = 64;	//  Rear Gun
					break;
				case 4:		//  Perry
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					vars[10] = W->bFXArray[0];	//  Front Gun
					vars[10] = 64;	//  Front Gun
					break;
				case 5:		//  San Antonio
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					break;
				case 6:		//  Kuznetsov
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];

						btempval = (BYTE)((64 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[4] = W->bFXArray[4] + btempval;
						W->bFXDesiredArray[4] = W->bFXArray[4];
					}
					vars[0] = W->bFXArray[3];  //  Radar 1
					vars[1] = W->bFXArray[4];	//  Radar 2
					break;
				case 7:		//  Slava
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];	//  Radar
					vars[10] = W->bFXArray[0];	//  Front Gun
					break;
				case 8:		//  Sovremenny
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];	//  Radar
					vars[10] = W->bFXArray[0];	//  Front Gun
					vars[11] = W->bFXArray[1];	//  Rear Gun
					break;
				case 9:		//  Krivak
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					break;
				case 10:		//  Oil Tanker
					vars[5] = isnight ? 255 : 0;
					break;
				case 11:		//  Cargo
					vars[5] = isnight ? 255 : 0;
					break;
				case 12:		//  OSA II
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];	//  Radar
					vars[10] = W->bFXArray[0];	//  Front Gun
					break;
				case 13:		//  Typhoon
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					break;
				case 14:		//  LCAC
					if(!SimPause)
					{
						btempval = (BYTE)((43 * DeltaTicks) / 1000);
						if(btempval == 0)
							btempval = 1;
						W->bFXArray[3] = W->bFXArray[3] + btempval;
						W->bFXDesiredArray[3] = W->bFXArray[3];
					}
					vars[0] = W->bFXArray[3];
					break;
			}

		} else {	// I am not a ship
			vars[1] = W->bFXArray[0];
			switch(type->TypeNumber)
			{
				case 4:	// Camel
	//				long workval = (W - MovingVehicles);
	//				workval = ((workval + (workval>>1)) & 0x7) * 100;
	//				vars[0] = ((CurrentTick + (workval)) & 1024) ? 1 : 0;
					if((W->Status & VL_MOVING) && (!(W->Status & VL_DESTROYED)) && (!SimPause))
					{
						W->iAnimationValue += DeltaTicks;
						while(W->iAnimationValue >= (W->iAnimationWork<<1))
						{
							W->iAnimationValue -= (W->iAnimationWork<<1);
						}
					}
					vars[0] = W->iAnimationValue / W->iAnimationWork;
					break;
				case 13:	//  Police car
				case 22:	//  Firetruck
					if((W->Status & VL_MOVING) && (!(W->Status & VL_DESTROYED)) && (!SimPause))
					{
						vars[0] = ((GameLoop + (int)W) &  0x20);
					}
					else
					{
						vars[0] = 0;
					}
					break;
				case 25:	// C-130 (Vehicle)
					vars[24] = ((!(W->Status & VL_MOVING)) || (W->Status & VL_DESTROYED)) ? 3 : (FrameCount % 3);
					vars[23] = ((!(W->Status & VL_MOVING)) || (W->Status & VL_DESTROYED)) ? 3 : (FrameCount % 3);
					vars[22] = ((!(W->Status & VL_MOVING)) || (W->Status & VL_DESTROYED)) ? 3 : (FrameCount % 3);
					vars[21] = ((!(W->Status & VL_MOVING)) || (W->Status & VL_DESTROYED)) ? 3 : (FrameCount % 3);
					break;
				case 26:	// F-15 (Vehicle)
					vars[0] = 0;
					break;
				case 39:	// SCUD Launcher
					if((W->Status & VL_FIRE_WEAPONS) && (W->iAnimationValue > 0) && (!SimPause))
					{
						if(W->RadarWeaponData->lWActionTimer > 0)  // SCOTT FIX THIS
						{
							W->RadarWeaponData->lWActionTimer += DeltaTicks;  // SCOTT FIX THIS
						}
						W->iAnimationWork += DeltaTicks;
						while(W->iAnimationWork >= 500)
						{
							W->iAnimationValue = W->iAnimationValue - 1;
							if(W->iAnimationValue <= 0)
							{
								W->iAnimationValue = 0;
								W->iAnimationWork = 0;
							}
							else
							{
								W->iAnimationWork = W->iAnimationWork - 500;
							}
						}
					}
					else if((!(W->Status & VL_FIRE_WEAPONS)) && (W->iAnimationValue < 255) && (!SimPause))
					{
						W->iAnimationWork += DeltaTicks;
						while(W->iAnimationWork >= 500)
						{
							W->iAnimationValue = W->iAnimationValue + 1;
							if(W->iAnimationValue >= 255)
							{
								W->iAnimationValue = 255;
								W->iAnimationWork = 0;
							}
							else
							{
								W->iAnimationWork = W->iAnimationWork - 500;
							}
						}
					}
					vars[0] = W->iAnimationValue;
					break;
			}
		}

		//  MOVED TO TOP OF FUNCTION
//		*(unsigned long *)&vars[10] = 0xF8FBFDFD;

//		if(!W->iShipType)
//			*(unsigned short *)&vars[4] = 0xFDF8;

		if (W->Status & VL_SINKING)
		{
			sunken.SetValues(0.0f,1.0f,0.0f);
			SetUpArbClipping(&relative_position,&sunken);

			ticks = (float)(GameLoop - W->FirstSinkingTick);
			t = ticks * W->SinkingMultiplier;

			sunken.Y = -(ticks*(float)((0.8/50.0) FEET));
			sunken += relative_position;

			pangle = (float)(0.15*PI)*t;

			if (W->Status & VL_SINKING_NEG)
				pangle = -pangle;

			mat.SetHPR(0,RADIANS_TO_ANGLE(pangle),RADIANS_TO_ANGLE(pangle*0.5));

			mat *= W->Attitude;


			if (carrierlights)
			{
				old_line_width = RWWidthForLines;
				RWWidthForLines = 1 FEET;
				DeckShadows = (DWORD)W->PlaneShadows;
			}

			ret_value = Draw3DObjectMatrix(type->Model,sunken,mat,&vars[0]);
			UseLightMapsOnObjects = FALSE;
			if (carrierlights)
			{
				FMatrix meatmat;

				DeckShadows = 0;
				RWWidthForLines = old_line_width;

				meatpoint.SetValues(-121.5 FEET,60 FEET,55 FEET);
				meatpoint *= mat;
				meatmat.SetRadRPH(0.0f,0.0f,8.0f*(PI/180.0f));
				meatmat *= mat;
				meatpoint += sunken;
				ZeroMemory(vars,32);
				Draw3DObjectMatrix(Meatball,meatpoint,meatmat,vars);
			}
			DoArbClip = 0;
		}
		else
		{
			if (carrierlights)
			{
				old_line_width = RWWidthForLines;
				RWWidthForLines = 1 FEET;
				DeckShadows = (DWORD)W->PlaneShadows;
			}

			ret_value = Draw3DObjectMatrix(type->Model,relative_position,W->Attitude,&vars[0]);
			UseLightMapsOnObjects = FALSE;
			if (carrierlights)
			{
				FMatrix meatmat;

				DeckShadows = 0;
				RWWidthForLines = old_line_width;

				meatpoint.SetValues(-121.5 FEET,60 FEET,55 FEET);
				meatpoint *= W->Attitude;
				meatpoint += relative_position;
				meatmat.SetRadRPH(0.0f,0.0f,8.0f*(PI/180.0f)+(float)ANGLE_TO_RADIANS(W->Heading));
				ZeroMemory(vars,32);
				Draw3DObjectMatrix(Meatball,meatpoint,meatmat,vars);
			}
		}

		if (dest)
			*dest = *source;

		return ret_value;
	}
	else
		return 0;
}


//**************************************************************************************
int DrawSubVehicles(void *object)
{
	int return_value = 0;
	MovingVehicleParams *W = (MovingVehicleParams *)object;

	while(W)
	{
		return_value |= DrawAVehicle(W,W->OrdDistanceFromCamera);
		W = W->NextVehicleOnBridge;
	}

	return return_value;

}

PlaneParams *carrwatch = Planes;
double gftx, gfty, gftz;
double ganghead;
int gmoveaction = 1;

//**************************************************************************************
void VUpdatePlanesOnDeck(MovingVehicleParams *vehiclepnt)
{
	PlaneParams *planepnt;
	int updated = 0;
//	double planeheight;
//	double dx, dy, dz;
//	FPointDouble orgpos;
	int vehiclenum = vehiclepnt - MovingVehicles;
	FPointDouble new_off;

	for (planepnt=Planes; planepnt<=LastPlane; planepnt++)
	{
		if (!((planepnt->Status & PL_ACTIVE) || (planepnt->AI.iAIFlags2 & AIINVISIBLE)))  continue;

		if (planepnt->FlightStatus & PL_STATUS_CRASHED)  continue;

		if((planepnt->OnGround == 2) && (planepnt->AI.iAICombatFlags1 & AI_HOME_CARRIER) && (planepnt->AI.iHomeBaseId == vehiclenum))
		{

			if (planepnt->PlaneCopy)
				ApplyCarrierMovementToPlane(planepnt);

			updated = 1;
		}
	}

	if(!updated)
	{
		vehiclepnt->lAIFlags1 &= ~PLANES_ON_DECK;
	}
}

//************************************************************************************************
float CalcHeightAboveDeck( PlaneParams *P, MovingVehicleParams *vehiclepnt)
{
	float deckheight = 64.0f * FTTOWU;
	FPoint carr_rel_down;
	FPoint carr_rel_fwd;
	FPoint carr_rel_position;
	FMatrix inv_carrier;
	// Check for contact with ground, update "OnGround" and return new "HeightAboveGround"
	// clip world position here and do ground collision

	// Seems to be a problem here


	double  clip_height;
	double  gear_height;
	double  land_height;

//#if 0
//	if (InHighResTerrainRegion(P->WorldPosition))
//		land_height = LandHeight(P->WorldPosition.X, P->WorldPosition.Z);
//	else
//		land_height = P->LastWorldPosition.Y - P->HeightAboveGround;
//#else

	inv_carrier = vehiclepnt->Attitude;
	inv_carrier.Transpose();

	carr_rel_down.RotateInto(P->Orientation.K,inv_carrier);

	carr_rel_fwd.RotateInto(P->Orientation.J,inv_carrier);

	carr_rel_position.MakeVectorToFrom(P->WorldPosition,vehiclepnt->WorldPosition);
	carr_rel_position *= inv_carrier;

//	land_height = carr_rel_position.Y;
//	land_height = vehiclepnt->WorldPosition.Y + deckheight;
	land_height = deckheight;
//#endif

	if (carr_rel_down.Y < 0)
	{
		if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
			gear_height = P->Type->GearDownHeight;
		else
			if (P->LandingGear)
				gear_height = P->Type->GearUpHeight + FSinTimes((P->LandingGear << 6),(P->Type->GearDownHeight-P->Type->GearUpHeight));
			else
				gear_height = P->Type->GearUpHeight;
	}
	else
		gear_height = P->Type->TailHeight;

	gear_height *= fabs((double)carr_rel_down.Y);
	clip_height  = fabs(carr_rel_fwd.Y * P->Type->ShadowLRXOff);

	if (clip_height < gear_height)	// use gear if larger
		clip_height = gear_height;

	clip_height += land_height;		// clip_height is now ABSOLUTE

	if(P->OnGround != 2)
	{
		if ((carr_rel_position.Y <= (clip_height+0.01)) && OverRunway(vehiclepnt->Type->Model,carr_rel_position))
		{
			P->OnGround = 2;
			vehiclepnt->lAIFlags1 |= PLANES_ON_DECK;
		}
		else
			P->OnGround = 0;
	}

	land_height = carr_rel_position.Y - land_height;
	if (P->OnGround == 2)
	{
//		carr_rel_position.Y = clip_height;
//		carr_rel_position *= vehiclepnt->Attitude;
//		P->WorldPosition = carr_rel_position;
//		P->WorldPosition += vehiclepnt->WorldPosition;

		if (!P->PlaneCopy)	/*on carrier*/
			P->PlaneCopy = GetNewCarrierPlane(P,vehiclepnt);


	}

	return land_height;	// new HeightAboveGround
}

//************************************************************************************************
void VCheckDeckMove(PlaneParams *planepnt)
{
	if(gmoveaction & 0x2)	//  right
	{
		gftx += (gmoveaction & 0x20) ? 10.0f : 1.0f;
	}
	if(gmoveaction & 0x4)	//  left
	{
		gftx -= (gmoveaction & 0x20) ? 10.0f : 1.0f;
	}
	if(gmoveaction & 0x8)	//  up
	{
		gftz -= (gmoveaction & 0x20) ? 10.0f : 1.0f;
	}
	if(gmoveaction & 0x10)	//  down
	{
		gftz += (gmoveaction & 0x20) ? 10.0f : 1.0f;
	}

	if(gmoveaction & 0x100)	//  clockwise
	{
		ganghead -= (gmoveaction & 0x400) ? 10.0f : 1.0f;
		ganghead = AICapAngle(ganghead);
	}
	if(gmoveaction & 0x200)	//  counterclockwise
	{
		ganghead += (gmoveaction & 0x400) ? 10.0f : 1.0f;
		ganghead = AICapAngle(ganghead);
	}
}


//****************************************************************************************
//**************************************************************************************
void AISimpleShipRadar(MovingVehicleParams *radarsite, GDRadarData *radardat)
{
  	PlaneParams *checkplane = &Planes[0];
//	float widthangle, heightangle;
	float radarrange, rangenm;
//	float visualrange, visualrangenm;
	float dx, dy, dz, tdist;
	float foundrange;
	PlaneParams *foundplane = NULL;
	float nlfoundrange;
	PlaneParams *nlfoundplane = NULL;
//	float offangle, toffangle, toffpitch, offpitch;
//	float targbearing, nltargbearing;
//	float targpitch, nltargpitch;
//	PlaneParams *leadplane;
//	PlaneParams *orgtarget;
//	int ifoundplane;
//	float radarangleyaw, radaranglepitch;
	int lockedorg = 0;
//	int stationnum;
	int noupdates = 1;
	DBRadarType *radar;
//	GDRadarData *radardat;
	PlaneParams *endplane;
	float fworkval;
	float percsee;
	int foundweapon;
	int itemp;
	int grndside;
	int weaponsearch = 1;
	float jammods[36];
	float offangle;
	int cnt;
	int highalert = 0;
//	int slotnum;
	int radardatnum = -1;
	int lookradar = radardat - radarsite->RadarWeaponData;
	int radarid;
	int cnt2;
	int maxdefs = pDBShipList[radarsite->iVDBIndex].iNumDefenseItems;
	float minrange = -1.0f;
	float mintemp;
	int minalt = 0;
	int crosssig;

	int unpaused = 0;

	if(!radarsite)
	{
		return;
	}

	if(radarsite->bDamageLevel > 1)
		return;

	grndside = radarsite->iSide;

	for(cnt = 0; cnt < maxdefs; cnt ++)
	{
		if(!(radarsite->RadarWeaponData[cnt].lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE)))
		{
			if(pDBShipList[radarsite->iVDBIndex].DefenseList[cnt].lDefenseType == VS_DEFENSE_WEAPON)
			{
				mintemp = pDBWeaponList[radarsite->RadarWeaponData[cnt].iWeaponIndex].fMinRange;
				if((minrange < 0) | (minrange > mintemp))
				{
					minrange = mintemp;
				}
			}
		}
	}
	minrange *= NMTOWU;

#if 0
	// SCOTT FIX
	if (radarsite->iShipType)
	{
		radar = NULL;
		for(slotnum = 0; ((slotnum < pDBShipList[radarsite->iVDBIndex].iNumDefenseItems) && (radardatnum < lookradar)); slotnum ++)
		{
			if(pDBShipList[radarsite->iVDBIndex].DefenseList[slotnum].lDefenseType == VS_DEFENSE_WEAPON)
			{
				continue;
			}
			radarid = pDBShipList[radarsite->iVDBIndex].DefenseList[slotnum].lTypeID;
			radar = GetRadarPtr(radarid);
			radardatnum ++;
		}
	}
	else
		return;
#else
	// DONE XSCOTT FIX
	if (radarsite->iShipType)
	{
		radar = NULL;
		radardatnum = radardat - radarsite->RadarWeaponData;

		radarid = pDBShipList[radarsite->iVDBIndex].DefenseList[radardatnum].lTypeID;
		radar = GetRadarPtr(radarid);
	}
	else
		return;
#endif

//		radardat = &radarsite->RadarWeaponData;
//	radardat = &radarsite->RadarWeaponData[radardatnum];

	if(radar == NULL)
	{
		if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			rangenm = AI_VISUAL_RANGE_NM;
		}
		else
		{
			radardat->lRFlags1 &= ~(GD_RADAR_ON|GD_RADAR_TEMP_ON|GD_RADAR_TEMP_OFF);
			return;
		}
	}
	else
	{
		rangenm = radar->iMaxRange;
		minalt = radar->lMinElevation * FTTOWU;
	}

	radarrange = rangenm * NMTOWU;
	foundrange = radarrange;
	foundplane = NULL;
	nlfoundrange = radarrange;
	nlfoundplane = NULL;

	radardat->lRFlags1 &= ~GD_RADAR_PLAYER_PING;

	endplane = LastPlane;

	if(radardat->lRFlags1 & GD_RADAR_LOCK)
	{
		if(radardat->Target == NULL)
		{
			radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
		}
		else
		{
			if(!AISeeIfStillVehicleMissiled(radarsite, (PlaneParams *)radardat->Target, 1, radardat))
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
				radardat->Target = NULL;
			}
			else   //  if(!(radar->iRadarType & RADAR_TYPE_TWS))
			{
				if(AICheckForGateStealerV(radarsite, radardat, (PlaneParams *)radardat->Target))
				{
					return;
				}
				checkplane = (PlaneParams *)radardat->Target;
				endplane = checkplane;
			}
		}
	}
	else
	{
		radardat->lRLockTimer = -999999;
	}


	AIGetGroundJammingMod(radarsite->WorldPosition, radarsite->iSide, jammods);

	while (checkplane <= endplane)
	{
		if (((!iInJump) || ((checkplane != PlayerPlane) && (!(checkplane->AI.iAIFlags1 & AIPLAYERGROUP)))) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED))) && (checkplane->OnGround == 0) && (checkplane->AI.iSide != grndside))
		{
			dx = checkplane->WorldPosition.X - radarsite->WorldPosition.X;
			dy = checkplane->WorldPosition.Y - radarsite->WorldPosition.Y;
			dz = checkplane->WorldPosition.Z - radarsite->WorldPosition.Z;
//			if((fabs(dx) < nlfoundrange) && (fabs(dz) < nlfoundrange))
			if((fabs(dx) < radarrange) && (fabs(dz) < radarrange))
			{
				percsee = GetPlaneVehicleRadarVisibility(radarsite, checkplane, 0, radarid);

				if(percsee)
				{
					tdist = QuickDistance(dx, dz);

					percsee *= GDModifyNoticePercent(radarsite, radardat, checkplane, MOVINGVEHICLE);

					offangle = atan2(-dx, -dz) * 57.2958;
					while(offangle > 359)
						offangle -= 360;
					while(offangle < 0)
						offangle += 360;
					cnt = offangle / 10;
					percsee *= jammods[cnt];

					crosssig = AICheckRadarCrossSig(radarsite, checkplane, dy * WUTOFT, tdist * WUTONM, radar, (radardat->lRFlags1 & GD_RADAR_LOCK));

					if(!crosssig)
					{
						radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
					}

//					if(!crosssig)
//					{
//						if(tdist > (visualrange * (1 + (inburner * 4))))
//						{
//							percsee = 0;
//						}
//					}

					if(tdist < (radarrange * jammods[cnt]))
					{
						if((!(radardat->lRFlags1 & GD_VISUAL_SEARCH)) && (radar))
						{
	 						AIUpdateGroundRadarThreat(radarsite, checkplane, tdist, MOVINGVEHICLE, radardat);
						}

						if((radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) && (checkplane->HeightAboveGround > minalt) && crosssig)  // may not need to do for lock but just in case
						{
							if(AISeeIfStillVehicleMissiled(radarsite, checkplane, 1, radardat))
							{
								noupdates = 0;
								if(percsee)
									percsee = 1.0f;
							}
						}
					}
					else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
					{
						checkplane->AI.pPaintedBy = NULL;
						checkplane->AI.iPaintedByType = -1;
						checkplane->AI.iPaintDist = -1;
					}

					if((tdist < nlfoundrange) && (tdist < (radarrange * jammods[cnt])) && (checkplane->HeightAboveGround > minalt))
					{
					//  Check probability of detection due to distance, cross sig, target aspect, and target alt.
					//  If was previous target, keep lock regardless of chance.
						if(checkplane->AI.pPaintedBy && (radarsite != (MovingVehicleParams *)checkplane->AI.pPaintedBy))
						{
							fworkval = checkplane->AI.iPaintDist * NMTOWU;
							if(tdist < fworkval)
							{
//								percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);

								percsee *= 2.0f;

								itemp = rand() & 127;

								if(itemp < (percsee * 127))
								{
									foundplane = checkplane;
									foundrange = tdist;

									for(cnt2 = 0; cnt2 < maxdefs; cnt2 ++)
									{
										if(radarsite->RadarWeaponData[cnt].lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
										{
											if(radarsite->RadarWeaponData[cnt2].Target == checkplane)
											{
												itemp = 128;
												break;
											}
										}
									}

//									if(itemp < 128)
//									{
//										if(AISeeIfAlreadyTarget(AIRCRAFT, checkplane))
//										{
//									 		itemp = 128;
//										}
//									}

									if(((itemp != 128) && (!AIPlaneAlreadyTarget(MOVINGVEHICLE, radarsite, checkplane))) && (tdist >= minrange))
									{
										nlfoundplane = checkplane;
										nlfoundrange = tdist;
									}
								}
							}
						}
						else
						{
//							percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);
							if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
							{
								percsee *= 3.0f;
							}

							itemp = rand() & 127;

							if(itemp < (percsee * 127))
							{
								foundplane = checkplane;
								foundrange = tdist;

								for(cnt2 = 0; cnt2 < maxdefs; cnt2 ++)
								{
									if(radarsite->RadarWeaponData[cnt].lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
									{
										if(radarsite->RadarWeaponData[cnt2].Target == checkplane)
										{
											itemp = 128;
											break;
										}
									}
								}

								if(itemp < 128)
								{
									if(AISeeIfAlreadyTarget(AIRCRAFT, checkplane))
									{
								 		itemp = 128;
									}
								}

								if((itemp != 128) && (tdist >= minrange))
								{
									nlfoundplane = checkplane;
									nlfoundrange = tdist;
								}
							}
						}

					}
					else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
					{
//						percsee = GetPlaneGroundRadarVisibility(radarsite, checkplane, 0);
						if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
						{
							percsee *= 3.0f;
						}

						itemp = rand() & 127;

						if(itemp < (percsee * 127))
						{
							checkplane->AI.iPaintDist = tdist * WUTONM;
						}
						else if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK))) // We have missiles flying we need to keep eye on.
						{
							checkplane->AI.pPaintedBy = NULL;
							checkplane->AI.iPaintedByType = -1;
							checkplane->AI.iPaintDist = -1;
						}
					}
				}
				else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
				{
					checkplane->AI.pPaintedBy = NULL;
					checkplane->AI.iPaintedByType = -1;
					checkplane->AI.iPaintDist = -1;
				}
			}
			else if(radarsite == (MovingVehicleParams *)checkplane->AI.pPaintedBy)
			{
				checkplane->AI.pPaintedBy = NULL;
				checkplane->AI.iPaintedByType = -1;
				checkplane->AI.iPaintDist = -1;
			}

		}
		checkplane ++;
	}

	if(noupdates)
	{
		radardat->lRFlags1 &= ~GD_CHECK_MISSILES;
	}

	if(foundplane == NULL)
	{
		for(cnt = 0; cnt < 36; cnt ++)
		{
			if(jammods[cnt] < 0.75)
			{
				highalert = 1;
			}
		}

		radardat->Target = NULL;
		if(highalert)
		{
			if(lNoticeTimer >= 0)
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK);
			}
			else
			{
				radardat->lRFlags1 &= ~(GD_RADAR_LOCK|GD_HAS_BEEN_REPORTED|GD_HAS_BEEN_REP_GEN);
			}
			radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
			radardat->lRFlags1 |= GD_HIGH_ALERT;
			radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
			radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
			if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
			{
				radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
			}
			else
			{
				radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
			}
		}
		else if(radardat->lRFlags1 & GD_HIGH_ALERT)
		{
//			radardat->lRFlags1 &= ~GD_HIGH_ALERT;
//			radardat->lRFlags1 |= GD_MED_ALERT;
			radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
		}
		else if(radardat->lRFlags1 & GD_MED_ALERT)
		{
//			radardat->lRFlags1 &= ~GD_MED_ALERT;
//			radardat->lRFlags1 |= GD_LOW_ALERT;
			radardat->lRActionTimer = 60000;  //  1 min until next sweep
		}
		else
		{
			radardat->lRActionTimer = 180000;  //  3 mins until next sweep
		}
		if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			if(radardat->lRActionTimer > 60000)
			{
				radardat->lRActionTimer = 60000;  //  1 mins until next sweep
			}
		}

		if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
		{
//			radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
//			radardat->lRFlags1 |= GD_HIGH_ALERT;
			radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		}
		return;
	}

	if(nlfoundplane != NULL)
	{
		foundplane = nlfoundplane;
		foundrange = nlfoundrange;
	}

	foundweapon = 0;

	int maxnum = 3;
	int ignoreweap = 0;
	int targdistnm = foundrange * WUTONM;

	if((!MultiPlayer) || (lAINetFlags1 & NGAI_ACTIVE))
	{
		//***  Don't shoot at escortable planes (weapons hold) or neutral planes that are not weapons free.
		if(!((foundplane->AI.iAICombatFlags2 & AI_ESCORTABLE) && (!iAI_ROE[grndside])))
		{
			if(!((foundplane->AI.iSide == AI_NEUTRAL) && (iAI_ROE[AI_NEUTRAL] < 2)))
			{
				foundweapon = VGDAssignShipWeapon(radarsite, TARGET_PLANE, foundplane, targdistnm, radardat);
			}
		}
	}

	if(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK)) // We have missiles flying we need to keep eye on.
	{
//		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
//		radardat->lRFlags1 |= GD_HIGH_ALERT;
		if(radardat->lRActionTimer < 0)
		{
			radardat->lRActionTimer = 3000;  //  3 seconds until next sweep
		}
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else if(foundweapon && (!(radardat->lRFlags1 & GD_HIGH_ALERT)))
	{
		radardat->lRFlags1 &= ~(GD_MED_ALERT|GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_HIGH_ALERT;
#if 0
		if(weapon->iWeaponType == 6)
		{
			if((!(radardat->lWFlags1 & GD_W_FIRING)) || (radardat->lWFlags1 & (GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET)))
			{
				radardat->lRActionTimer = 5000;  //  5 seconds until next sweep
			}
			else
			{
				radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
			}
		}
		else
		{
			radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
		}
#else
		radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
#endif
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else if((radardat->lRFlags1 & GD_HIGH_ALERT) || (foundplane))
	{
		radardat->lRActionTimer = 15000;  //  15 seconds until next sweep
		radardat->lRAlertTimer = 180000;	//  Red Alert for 3 minutes
	}
	else
	{
		radardat->lRFlags1 &= ~(GD_LOW_ALERT);
		radardat->lRFlags1 |= GD_MED_ALERT;
		radardat->lRActionTimer = 60000;  //  1 min until next sweep
		radardat->lRAlertTimer = 600000;	//  Med Alert for 10 minutes
	}

	if(radardat->lRFlags1 & GD_VISUAL_SEARCH)
	{
		if(radardat->lRActionTimer > 60000)
		{
			radardat->lRActionTimer = 60000;  //  1 mins until next sweep
		}
	}

	if(unpaused)
	{
		UnPauseFromDiskHit();
	}
}

//**************************************************************************************
int VGDAssignShipWeapon(MovingVehicleParams *vehiclepnt, int targettype, void *target, int targdistnm, GDRadarData *rradardat)
{
	int slotnum;
	GDRadarData *radardat;
	DBWeaponType *weapon;
	int selectweapon = -1;
	int selectmaxrange = -1;
	PlaneParams *foundplane = (PlaneParams *)target;
	int ignoreweap;
	int maxnum = 3;
	int weaponclass = 0;
	int startslot = 0;
	int endslot = pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems;

	if(rradardat->lRFlags1 & GD_VISUAL_SEARCH)
	{
		startslot = rradardat - vehiclepnt->RadarWeaponData;
		endslot = startslot;
	}

	for(slotnum = startslot; slotnum < endslot; slotnum ++)
	{
		double our_dist;


		ignoreweap = 0;
		if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lDefenseType != VS_DEFENSE_WEAPON)
		{
			continue;
		}


		weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID);

		radardat = &vehiclepnt->RadarWeaponData[slotnum];

		if(selectweapon != -1)
		{
			if((weapon->iRange >= selectmaxrange) && ((weapon->iWeaponType != WEAPON_TYPE_SAM) || (weaponclass < 2)))
			{
				continue;
			}
		}

		if((weapon->iWeaponType == WEAPON_TYPE_AG_MISSILE) || (weapon->iWeaponType == WEAPON_TYPE_ANTI_SHIP_MISSILE))
		{
			continue;
		}
		else if((weapon->iWeaponType == WEAPON_TYPE_GUN) && (weapon->iUsageIndex != 11))
		{
			if(((float)(weapon->iRange<<2) > targdistnm) && (weapon->fMinRange <= targdistnm) && (((foundplane->WorldPosition.Y - vehiclepnt->WorldPosition.Y) * WUTOFT) < weapon->iMaxAlt))
			{
				ignoreweap = 0;
				if(((radardat->lWFlags1 & GD_W_FIRING) && (!(radardat->lWFlags1 & (GD_W_PREP_RANDOM|GD_W_RANDOM_FIRE|GD_W_DUPLICATE_TARGET)))) || (weaponclass > 1))
				{
					ignoreweap = 1;
				}
				if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (ignoreweap == 0))
				{
					our_dist = foundplane->WorldPosition - vehiclepnt->WorldPosition;
					if((float)(weapon->iRange<<1) >= targdistnm)
					{
						if(GDNumAAAFiringAtPlane(foundplane) <= maxnum)
						{
							selectweapon = slotnum;
							selectmaxrange = weapon->iRange;
							weaponclass = 1;
						}
					}
				}
			}
		}
		else if(weapon->iWeaponType == WEAPON_TYPE_SAM)
		{
			if((GDNumLaunchingAtPlane(foundplane) + AINumMissilesAtPlane(foundplane)) < 3)
			{
				if(((float)weapon->iRange >= targdistnm) && (weapon->fMinRange <= targdistnm) && (((foundplane->WorldPosition.Y - vehiclepnt->WorldPosition.Y) * WUTOFT) < weapon->iMaxAlt))
				{
					if(!((rradardat->lRFlags1 & GD_RADAR_LOCK) && (!((radardat->lRFlags1 & GD_BURST_MODE) && (radardat->pRBurstTarget == foundplane)))))
					{
//						if((!(rradardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (radardat->Target == NULL) && (radardat->lWActionTimer < 0) && (ignoreweap == 0))
						if((!(rradardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lWNumberFired < weapon->iBurstRate) && (VGDCheckVWeapCanFire(vehiclepnt, radardat, rradardat, targettype, target)) && (radardat->lWActionTimer < 0) && (ignoreweap == 0))
						{
							selectweapon = slotnum;
							selectmaxrange = weapon->iRange;
							weaponclass = 2;
						}
					}
				}
			}
		}
	}

	if(selectweapon != -1)
	{
		radardat = &vehiclepnt->RadarWeaponData[selectweapon];
  	 	VGDSetUpWeaponTargeting(vehiclepnt, foundplane, targdistnm * NMTOWU, radardat, rradardat);
		return(1);
	}

	return(0);
}

//**************************************************************************************
int VGDRadarOn(MovingVehicleParams *vehiclepnt)
{
	GDRadarData *radardat;
	int cnt;

	if (!vehiclepnt->iShipType) // SCOTT FIX
	{
		radardat = vehiclepnt->RadarWeaponData;
		if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
		{
			return(1);
		}
		return(0);
	}
	else
	{
		for(cnt = 0; cnt < NUM_DEFENSE_LIST_ITEMS; cnt ++)
		{
			radardat = &vehiclepnt->RadarWeaponData[cnt];
			if(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))
			{
				return(1);
			}
		}
	}
	return(0);
}

//**************************************************************************************
DBWeaponType *VGDGetShipWeaponForRDNum(MovingVehicleParams *vehiclepnt, int radardatnum)
{
#if 0
	int slotnum;
	int weaponnum = 0;

	for(slotnum = 0; slotnum < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; slotnum ++)
	{
		if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lDefenseType == VS_DEFENSE_WEAPON)
		{
			if(weaponnum == radardatnum)
			{
				return(get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID));
			}
			weaponnum ++;
		}
	}
	return(NULL);
#else
	return(get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID));
#endif
}


//**************************************************************************************
GDRadarData *VGDGetRadarDatAtPlane(MovingVehicleParams *vehiclepnt, PlaneParams *planepnt, int *found)
{
	int slotnum;
	int weaponnum = 0;

	for(slotnum = 0; slotnum < NUM_DEFENSE_LIST_ITEMS; slotnum ++)
	{
		if((vehiclepnt->RadarWeaponData[slotnum].Target == planepnt) && (pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lDefenseType == VS_DEFENSE_RADAR))
		{
			if(found)
			{
				*found = 1;
			}
			return(&vehiclepnt->RadarWeaponData[slotnum]);
		}
	}

	if(found)
	{
		*found = 0;
	}
	return(vehiclepnt->RadarWeaponData);
}

//**************************************************************************************
int VGDCheckVWeapCanFire(MovingVehicleParams *vehiclepnt, GDRadarData *radardat, GDRadarData *rradardat, int converttarget, void *target)
{
	DBRadarType *radar;
	DBWeaponType *weapon;
	PlaneParams *foundplane = (PlaneParams *)target;
	int radardatnum, rradardatnum;
	GDRadarData *tempdat;
	int numtargets = 0;
	int targettype;
	int maxdef = 2;
	int ignoreguns = 0;

	switch(converttarget)
	{
		case TARGET_PLANE:
			targettype =  AIRCRAFT;
			break;
		case TARGET_GROUND:
			targettype = GROUNDOBJECT;
			break;
		case TARGET_VEHICLE:
			targettype = MOVINGVEHICLE;
			break;
		default:
			targettype = -1;
			break;
	}

	//  See if weapons are already at target (this includes bullets)
	if(AISeeIfAlreadyTarget(targettype, target))
	{
		return(0);
	}

	if (vehiclepnt->iShipType)
	{
		maxdef = pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems;
	}

	radardatnum = radardat - vehiclepnt->RadarWeaponData;
	rradardatnum = rradardat - vehiclepnt->RadarWeaponData;

	if (!vehiclepnt->iShipType) // SCOTT FIX
	{
		if(!radardatnum)
		{
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
		}
		else
		{
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
		}
	}
	else
	{
		weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID);
	}

	if(weapon->iWeaponType == WEAPON_TYPE_SAM)
	{
		ignoreguns = 1;
	}

	for(int slotnum = 0; (slotnum < maxdef); slotnum ++)
	{
//		if(vehiclepnt->RadarWeaponData[slotnum].lWFlags1 & GD_W_ACQUIRING)
		if(vehiclepnt->RadarWeaponData[slotnum].lWFlags1 & (GD_W_ACQUIRING|GD_W_PREP_RANDOM|GD_W_FIRING|GD_W_RANDOM_FIRE))
		{
			if(vehiclepnt->RadarWeaponData[slotnum].Target == target)
			{
				if (!vehiclepnt->iShipType) // SCOTT FIX
				{
					if(!slotnum)
					{
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
					}
					else
					{
						weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
					}
				}
				else
				{
					weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID);
				}

				if((weapon->iWeaponType == WEAPON_TYPE_SAM) || (!ignoreguns))
				{
					return(0);
				}
			}
		}
	}

	if (!vehiclepnt->iShipType) // SCOTT FIX
	{
		radar = GetRadarPtr(pDBVehicleList[vehiclepnt->iVDBIndex].lRadarID);
		if(!radardatnum)
		{
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType);
		}
		else
		{
			weapon = get_weapon_ptr(pDBVehicleList[vehiclepnt->iVDBIndex].lWeaponType2);
		}
	}
	else
	{
		radar = GetRadarPtr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[rradardatnum].lTypeID);
		weapon = get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].lTypeID);
	}


	WeaponParams *W = &Weapons[0];

	if(target == NULL)
	{
		return(0);
	}

	if(radar == NULL)
	{
		if(rradardat->lRFlags1 & GD_VISUAL_SEARCH)
		{
			return(1);
		}

		return(0);
	}

	while(W <= LastWeapon)
	{
		if (W->Flags & WEAPON_INUSE)
		{
			if(W->iTargetType == converttarget)
			{
				if(W->LauncherType == MOVINGVEHICLE)
				{
					if(vehiclepnt == (MovingVehicleParams *)W->Launcher)
					{
						if(W->LaunchStation != -1)
						{
							tempdat = &vehiclepnt->RadarWeaponData[W->LaunchStation];
							if((tempdat == radardat) && (weapon->iSeekerType == 7))
							{
								return(0);
							}
							if(rradardat == (GDRadarData *)tempdat->pWRadarSite)
							{
								numtargets++;
							}
						}
					}
				}
			}
		}
		W++;
	}

	if(numtargets > radar->iNumberOfTracks)
	{
		return(0);
	}
	return(1);
}

//**************************************************************************************
void VGDGetWeaponPosition(void *ocurrweapon, int sitetype, int radardatnum, FPointDouble *weaponoffset)
{
	double dwx, dwy, dwz;
	MovingVehicleParams *vehiclepnt;
	if(sitetype != MOVINGVEHICLE)
	{
		return;
	}
	vehiclepnt = (MovingVehicleParams *)ocurrweapon;
	if(!vehiclepnt->iShipType)
	{
		return;
	}

	dwx = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].fLocationX*FOOT_MULTER;
	dwy = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].fLocationY*FOOT_MULTER;
	dwz = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[radardatnum].fLocationZ*FOOT_MULTER;

	AIOffsetFromPoint(weaponoffset, AIConvertAngleTo180Degree(vehiclepnt->Heading), dwx, dwy, dwz);
	return;
}

//**************************************************************************************
void VSetUpInitialFormations()
{
	MovingVehicleParams *vehiclepnt, *grouppnt;
	int placecnt;
	int formation;
	RoutePoint	 *pNextWP;		//  CurrentWayPoint
	float fmax_group_spd;
	FPoint formation_offset, workpos;
	double angxx, angxz, angzx, angzz;
	double heading;

	for(vehiclepnt = MovingVehicles; vehiclepnt <= LastMovingVehicle; vehiclepnt ++)
	{
		if(!vehiclepnt->pLeadVehicle)
		{
			vehiclepnt->iPlaceInGroup = 0;
			if(vehiclepnt->pTrailVehicle)
			{
				placecnt = 1;

				if(vehiclepnt->numwaypts > 1)
				{
					pNextWP = vehiclepnt->pCurrWP + 1;
					formation = pNextWP->iRouteActionFlags & (VEHICLE_ACTION_FORMATION1|VEHICLE_ACTION_FORMATION2|VEHICLE_ACTION_FORMATION3|VEHICLE_ACTION_FORMATION4|VEHICLE_ACTION_FORMATION5);
				}
				else
				{
					//  Mike's fix should be fine.  SRE
					//********// Bug Here Scott - pNextWP has not been initialized
					//********// formation = pNextWP->iRouteActionFlags & (VEHICLE_ACTION_FORMATION1|VEHICLE_ACTION_FORMATION2|VEHICLE_ACTION_FORMATION3|VEHICLE_ACTION_FORMATION4|VEHICLE_ACTION_FORMATION5);
					pNextWP = vehiclepnt->pCurrWP;
					formation=0;
				}

				grouppnt = (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
				fmax_group_spd = vehiclepnt->fSpeedFPS;

				heading = DegToRad(((double)vehiclepnt->Heading / DEGREE));

				angzx = sin(heading);
				angzz = cos(heading);
				angxx = angzz;
				angxz = angzx;

				while(grouppnt)
				{
					grouppnt->iPlaceInGroup = placecnt;
					VAIGetFormationOffset(grouppnt, placecnt, &formation_offset, vehiclepnt, formation);

//					workpos = vehiclepnt->WorldPosition;
					grouppnt->FormationOffset.X = formation_offset.X;
					grouppnt->FormationOffset.Y = formation_offset.Y;
					grouppnt->FormationOffset.Z = formation_offset.Z;

					formation_offset.X *= FTTOWU;
					formation_offset.Z *= FTTOWU;

					workpos.X = vehiclepnt->WorldPosition.X + (angxx * formation_offset.X) + (angzx * formation_offset.Z);
					workpos.Y = vehiclepnt->WorldPosition.Y;
					workpos.Z = vehiclepnt->WorldPosition.Z + (-(angxz * formation_offset.X) + (angzz * formation_offset.Z));

					grouppnt->WorldPosition.X = workpos.X;
					grouppnt->WorldPosition.Z = workpos.Z;

					if(formation_offset.Z < 0)
					{
						grouppnt->WayPosition.X = vehiclepnt->WayPosition.X + (angxx * formation_offset.X) + (angzx * formation_offset.Z);
						grouppnt->WayPosition.Z = vehiclepnt->WayPosition.Z + (-(angxz * formation_offset.X) + (angzz * formation_offset.Z));
					}
					else
					{
						grouppnt->WayPosition.X = vehiclepnt->WayPosition.X + (angxx * formation_offset.X);  //   + (angzx * formation_offset.Z);
						grouppnt->WayPosition.Z = vehiclepnt->WayPosition.Z + (-(angxz * formation_offset.X)); //   + (angzz * formation_offset.Z);
					}

					if(fmax_group_spd > grouppnt->fSpeedFPS)
					{
						fmax_group_spd = grouppnt->fSpeedFPS;
					}
					placecnt ++;
					grouppnt = (MovingVehicleParams *)grouppnt->pTrailVehicle;
				}

				grouppnt = vehiclepnt;  //  (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
				while(grouppnt)
				{
					grouppnt->fSpeedFPS = fmax_group_spd;
					grouppnt = (MovingVehicleParams *)grouppnt->pTrailVehicle;
					//  Do stuff for submerged vehicles here
				}
			}
		}
	}

	return;
}

//**************************************************************************************
void VAIGetFormationOffset(MovingVehicleParams *vehiclepnt, int placeingroup, FPoint *formation_offset, MovingVehicleParams *leadpnt, int formationid)
{
	if(vehiclepnt->iShipType)
	{
		VAIGetShipFormationOffset(vehiclepnt, placeingroup, formation_offset, leadpnt, formationid);
		return;
	}

	float vehiclespacing;
	int modval, multval;
	FPoint offset;

	if(placeingroup <= 0)
	{
		formation_offset->X = 0;
		formation_offset->Y = 0;
		formation_offset->Z = 0;
		return;
	}

	vehiclespacing = GetObjectRadius(vehiclepnt->Type->Model) * WUTOFT;

	if(vehiclepnt->iShipType)
	{
		vehiclespacing *= 8.0f;
	}

	offset.SetValues(0.0f, 0.0f, 0.0f);
	if((formationid & VEHICLE_ACTION_FORMATION1) || (!(formationid & (VEHICLE_ACTION_FORMATION1|VEHICLE_ACTION_FORMATION2|VEHICLE_ACTION_FORMATION3|VEHICLE_ACTION_FORMATION4|VEHICLE_ACTION_FORMATION5))))
	{
		offset.Z = vehiclespacing * 4.0f * placeingroup;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION2)
	{
		offset.X = (vehiclespacing * 4.0f) * ((placeingroup + 1) / 2);
		if(placeingroup & 1)
		{
			offset.X *= -1;
		}
	}
	else if(formationid & VEHICLE_ACTION_FORMATION3)
	{
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(-8.0f, 0.0f, 0.0f);
				break;
			case 2:
				offset.SetValues(-3.0f, 0.0f, 4.0f);
				break;
			case 3:
				offset.SetValues(-6.0f, 0.0f, 4.0f);
				break;
			case 4:
				offset.SetValues(-1.0f, 0.0f, 12.0f);
				break;
			case 5:
				offset.SetValues(-9.0f, 0.0f, 12.0f);
				break;
			case 6:
				offset.SetValues(-5.0f, 0.0f, 16.0f);
				break;
			case 7:
				offset.SetValues(-7.0f, 0.0f, 16.0f);
				break;
			default:
				offset.SetValues(-6.0f, 0.0f, 0.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION4)
	{
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(2.0f, 0.0f, 2.0f);
				break;
			case 2:
				offset.SetValues(6.0f, 0.0f, 6.0f);
				break;
			case 3:
				offset.SetValues(4.0f, 0.0f, 4.0f);
				break;
			case 4:
				offset.SetValues(3.0f, 0.0f, 20.0f);
				break;
			case 5:
				offset.SetValues(5.0f, 0.0f, 22.0f);
				break;
			case 6:
				offset.SetValues(9.0f, 0.0f, 26.0f);
				break;
			case 7:
				offset.SetValues(7.0f, 0.0f, 24.0f);
				break;
			default:
				offset.SetValues(0.0f, 0.0f, 8.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION5)
	{
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(-3.0f, 0.0f, 6.0f);
				break;
			case 2:
				offset.SetValues(6.0f, 0.0f, 6.0f);
				break;
			case 3:
				offset.SetValues(3.0f, 0.0f, 3.0f);
				break;
			case 4:
				offset.SetValues(-1.5f, 0.0f, 3.0f);
				break;
			case 5:
				offset.SetValues(9.0f, 0.0f, 9.0f);
				break;
			case 6:
				offset.SetValues(-4.5f, 0.0f, 9.0f);
				break;
			case 7:
				offset.SetValues(12.0f, 0.0f, 12.0f);
				break;
			default:
				offset.SetValues(0.0f, 0.0f, 12.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}

	formation_offset->X = offset.X;
	formation_offset->Y = offset.Y;
	formation_offset->Z = offset.Z;
}

//**************************************************************************************
void VAIGetShipFormationOffset(MovingVehicleParams *vehiclepnt, int placeingroup, FPoint *formation_offset, MovingVehicleParams *leadpnt, int formationid)
{
	float vehiclespacing;
	int modval, multval;
	FPoint offset;

	if(placeingroup <= 0)
	{
		formation_offset->X = 0;
		formation_offset->Y = 0;
		formation_offset->Z = 0;
		return;
	}

	vehiclespacing = GetObjectRadius(vehiclepnt->Type->Model) * WUTOFT;

	if(vehiclepnt->iShipType)
	{
		vehiclespacing *= 8.0f;
	}

	offset.SetValues(0.0f, 0.0f, 0.0f);
	if((formationid & VEHICLE_ACTION_FORMATION1) || (!(formationid & (VEHICLE_ACTION_FORMATION1|VEHICLE_ACTION_FORMATION2|VEHICLE_ACTION_FORMATION3|VEHICLE_ACTION_FORMATION4|VEHICLE_ACTION_FORMATION5))))
	{
//		offset.Z = vehiclespacing * 4.0f * placeingroup;
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(2.0f, 0.0f, -4.0f);
				break;
			case 2:
				offset.SetValues(-2.0f, 0.0f, 4.0f);
				break;
			case 3:
				offset.SetValues(1.0f, 0.0f, 8.0f);
				break;
			case 4:
				offset.SetValues(-3.0f, 0.0f, -8.0f);
				break;
			case 5:
				offset.SetValues(-1.5f, 0.0f, 12.0f);
				break;
			case 6:
				offset.SetValues(-1.0f, 0.0f, -12.0f);
				break;
			case 7:
				offset.SetValues(2.0f, 0.0f, 16.0f);
				break;
			default:
				offset.SetValues(-6.0f, 0.0f, 0.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION2)
	{
//		offset.X = (vehiclespacing * 4.0f) * ((placeingroup + 1) / 2);
//		if(placeingroup & 1)
//		{
//			offset.X *= -1;
//		}
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(-8.0f, 0.0f, -4.0f);
				break;
			case 2:
				offset.SetValues(6.0f, 0.0f, 2.0f);
				break;
			case 3:
				offset.SetValues(10.0f, 0.0f, -4.0f);
				break;
			case 4:
				offset.SetValues(-8.0f, 0.0f, 4.0f);
				break;
			case 5:
				offset.SetValues(-16.0f, 0.0f, 0.0f);
				break;
			case 6:
				offset.SetValues(16.0f, 0.0f, 0.0f);
				break;
			case 7:
				offset.SetValues(24.0f, 0.0f, 4.0f);
				break;
			default:
				offset.SetValues(4.0f, 0.0f, 4.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION3)
	{
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(8.0f, 0.0f, -8.0f);
				break;
			case 2:
				offset.SetValues(-2.0f, 0.0f, 8.0f);
				break;
			case 3:
				offset.SetValues(-8.0f, 0.0f, -1.0f);
				break;
			case 4:
				offset.SetValues(7.0f, 0.0f, 8.0f);
				break;
			case 5:
				offset.SetValues(-4.0f, 0.0f, -8.0f);
				break;
			case 6:
				offset.SetValues(8.0f, 0.0f, 0.0f);
				break;
			case 7:
				offset.SetValues(-6.0f, 0.0f, 6.0f);
				break;
			default:
				offset.SetValues(-6.0f, 0.0f, 0.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION4)
	{
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(-8.0f, 0.0f, -6.0f);
				break;
			case 2:
				offset.SetValues(8.0f, 0.0f, 6.0f);
				break;
			case 3:
				offset.SetValues(-12.0f, 0.0f, -12.0f);
				break;
			case 4:
				offset.SetValues(4.0f, 0.0f, 9.0f);
				break;
			case 5:
				offset.SetValues(16.0f, 0.0f, 12.0f);
				break;
			case 6:
				offset.SetValues(-18.0f, 0.0f, -15.0f);
				break;
			case 7:
				offset.SetValues(24.0f, 0.0f, 18.0f);
				break;
			default:
				offset.SetValues(-4.0f, 0.0f, 4.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}
	else if(formationid & VEHICLE_ACTION_FORMATION5)
	{
		modval = placeingroup % 8;
		multval = (placeingroup / 8) + 1;

		switch(modval)
		{
			case 1:
				offset.SetValues(6.0f, 0.0f, -3.0f);
				break;
			case 2:
				offset.SetValues(-6.0f, 0.0f, -3.0f);
				break;
			case 3:
				offset.SetValues(0.0f, 0.0f, -6.0f);
				break;
			case 4:
				offset.SetValues(12.0f, 0.0f, 4.0f);
				break;
			case 5:
				offset.SetValues(-12.0f, 0.0f, 4.0f);
				break;
			case 6:
				offset.SetValues(15.0f, 0.0f, 10.0f);
				break;
			case 7:
				offset.SetValues(-15.0f, 0.0f, 10.0f);
				break;
			default:
				offset.SetValues(0.0f, 0.0f, 6.0f);
				break;
		}
		offset.X *= multval * vehiclespacing;
		offset.Z *= multval * vehiclespacing;
	}

	formation_offset->X = offset.X;
	formation_offset->Y = offset.Y;
	formation_offset->Z = offset.Z;
}

//**************************************************************************************

BOOL AngleForTailHook(PlaneType *ptype,FPoint &cooef,float D,float &rangle,BYTE &angle)
{
	float L2=ptype->TailHookLength*ptype->TailHookLength;

	float divisor;
	float part1;
	float part2;
	float sol1,sol2;

	divisor = -cooef.Z*ptype->TailHookLength-D;

	if (divisor == 0.0f)
		return FALSE;

	part1 = -cooef.Y*ptype->TailHookLength;

	part2 = L2*(cooef.Y*cooef.Y+cooef.Z*cooef.Z)-D*D;

	if (part2 < 0.0f)
		return FALSE;

	part2 = sqrt(part2);

	sol1 = 2*atan2(part1-part2,divisor);
	sol2 = 2*atan2(part1+part2,divisor);

	if ((sol1 > 0.0f) && (sol2 > 0.0f))
		return FALSE;

	if ((sol1 <= 0.0f) && (sol2 <= 0.0f))
	{
		if (sol2 > sol1)
			sol1 = sol2;
	}
	else
		if (sol2 <= 0.0f)
			sol1 = sol2;

	if (sol1 < -(PI*0.5f))
		return FALSE;

	rangle = -sol1*(180.0f/PI);

	sol1*=1.0f/(-PI*0.5f);

	angle = (BYTE)(255.0f * sol1);

	return TRUE;
}

//**************************************************************************************

void PlaneReleaseTailhook(PlaneParams *P,MovingVehicleParams *V)
{
	if (P->TailHookFlags & TH_HOOKED)
	{
		if (!V)
			V =  &MovingVehicles[P->AI.iHomeBaseId];
		P->TailHookFlags &= ~TH_HOOKED;
		V->CatchWire.CaughtPlane = NULL;
		V->CatchWire.Flags &= ~CW_PLANE_CAUGHT;
		V->CatchWire.Flags |= CW_RETURNING;
		V->CatchWire.Tick = GameLoop;    // 10 seconds;
		P->AI.Behaviorfunc = AIUnhookFromTrapWire;
		P->AI.lVar3 = 0;
		P->AI.lTimer2 = 2000;
	}

}

#define LANDING_OK			(130.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS))
#define LANDING_CRASH_START	(150.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS))
#define LANDING_NO_LAND	    (200.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS))
#define LANDING_NO_BOLTER   (250.0f / (FTSEC_TO_MLHR*MLHR_TO_KNOTS))

//**************************************************************************************

BOOL UpdateTailHookAndCheckForWireCatch(PlaneParams *P,MovingVehicleParams *V)
{
	FPoint deck_point;
	FPoint deck_normal;
	FPointDouble deck_pos;

	FPoint tail_hook_vec;
	float  wire_rangle;

	float hook_angle;

	FMatrix hook_mat;
	FMatrix hook_off;
	FMatrix attitude;

	float coll_rangle;
	BYTE angle;
	float our_d;

	BOOL get_tail_collision = FALSE;
	BOOL mix_tail_positions = FALSE;
	BOOL collided;
	float mix_amount = 0.0f;
	BOOL carrier_plane = P->Status & PL_CARRIER_REL;
	PlaneParams *checkplane;
	float land_chance;
	float crash_chance;
	float landing_roll;
	BOOL continue_check;
	int iWireCaught=-1;

	if (V)
	{
		if (P->TailHookFlags & TH_HOOKED)
			deck_point = V->Type->CatchWires.Values[V->CatchWire.Flags & CW_WIRE];
		else
			deck_point = V->Type->CatchWires.Values[0];

		if (carrier_plane)
		{
			deck_pos = deck_point;
			deck_normal.SetValues(0.0f,1.0f,0.0f);
		}
		else
		{
			deck_point *= V->Attitude;
			deck_pos = deck_point;
			deck_pos += V->WorldPosition;
			deck_normal.SetValues(V->Attitude.m_Data.RC.R0C1,V->Attitude.m_Data.RC.R1C1,V->Attitude.m_Data.RC.R2C1);
		}
	}
	else
	{
		deck_pos.X=P->WorldPosition.X;
		deck_pos.Z=P->WorldPosition.Z;
		deck_pos.Y = LandHeight(P->WorldPosition.X,P->WorldPosition.Z);
		LandNormal(deck_pos,deck_normal);
	}


	deck_point.MakeVectorToFrom(deck_pos,P->WorldPosition);

	if(!carrier_plane)
		attitude = P->Attitude;
	else
		attitude.SetRPH(0,0,P->Heading);

	attitude.Transpose();
	deck_point *= attitude;

	deck_point -= P->Type->TailHookPivot;

	hook_mat.SetRadRPH(0.0f,P->Type->TailHookRestAngle,0.0f);

	deck_point *= hook_mat;

	deck_normal*=attitude;
	deck_normal*=hook_mat;

	P->TailHookFlags = (P->TailHookFlags & 0xFFFF);
	P->TailHookFlags |= P->TailHookFlags << 16;
	P->TailHookFlags &= ~(TH_IN_CHANNEL | TH_WIRE_FLAGS);
	P->LastTailHookPosition = P->TailHookPosition;

	if (P->TailHookFlags & TH_HOOKED)
	{
		tail_hook_vec = deck_point;
		wire_rangle = -(float)atan2(deck_point.Y,deck_point.Z);
		wire_rangle *= 180.0f/PI;
		if(P->AI.lVar3 == 2)
		{
			get_tail_collision = TRUE;
			mix_tail_positions = TRUE;
			mix_amount = P->V / AIConvertSpeedToFtPS(P->Altitude, -10, 0);
			if (mix_amount <= 0.0001f)
			{
				mix_amount = 0.0f;
				get_tail_collision = TRUE;

				if((MultiPlayer) && ((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane)))
				{
					if(P->Status & PL_CARRIER_REL)
					{
						for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
						{
							if(checkplane->PlaneCopy == P)
							{
								break;
							}
						}
						if(checkplane > LastPlane)
						{
							checkplane = P;
						}
					}
					else
						checkplane = P;

					NetPutGenericMessage1(checkplane, GM_RELEASE_HOOK);
				}

				PlaneReleaseTailhook(P,V);
			}
			else
				if (mix_amount > 1.0f)
					mix_amount = 1.0f;
		}
	}
	else
		get_tail_collision = TRUE;

//	if (deck_point.Z < 0.0f)
//		get_tail_collision = TRUE;
//	else
//		collided = TRUE;

	collided = (get_tail_collision && AngleForTailHook(P->Type,deck_normal,(float)(deck_normal*deck_point),coll_rangle,angle));

	if (P->TailHookFlags & TH_HOOKED)
	{
		if (mix_tail_positions)
		{
			if (collided && (coll_rangle <= P->TailHook))
				hook_angle = coll_rangle;
			else
				hook_angle = P->TailHook;

			hook_angle *= 1.0f-mix_amount;
			hook_angle += mix_amount*wire_rangle;
		}
		else
		{
			hook_angle = wire_rangle;
			P->TailHookFlags &= ~TH_TOUCHED_DOWN;
		}

	}
	else
	{
		if (collided && ((P->TailHookFlags & TH_TOUCHED_DOWN) || (coll_rangle <= P->TailHook)))
		{
			hook_angle = coll_rangle;
			if (P->TailHookCommandedPos >= P->TailHook)
				P->TailHookFlags |= TH_TOUCHED_DOWN;
			else
				P->TailHookFlags &= ~TH_TOUCHED_DOWN;
		}
		else
		{
			hook_angle = P->TailHook;
			P->TailHookFlags &= ~TH_TOUCHED_DOWN;
		}
	}

	P->TailHook = hook_angle;

	hook_off.SetRadRPH(0.0f,hook_angle*(PI/180.0f),0.0f);

	P->TailHookPosition.RotateInto(P->Type->TailHookPoint,hook_off);
	P->TailHookPosition += P->Type->TailHookPivot;

	attitude.Transpose();

	P->TailHookPosition *= attitude;
	P->TailHookPosition += P->WorldPosition;

	if (V && (V->CatchWire.CaughtPlane) && (V->CatchWire.CaughtPlane->PlaneCopy == P))
		V->CatchWire.CaughtPlane = P;

	if (!carrier_plane && V)
	{
		FMatrix ship_mat = V->Attitude;
		ship_mat.Transpose();

		P->TailHookPosition -= V->WorldPosition;
		P->TailHookPosition *= ship_mat;
	}

	if((MultiPlayer) && (!((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane))))
	{
		collided = 0;  //  Make sure with John that we don't have to go through if statement below.
	}

	/* -----------------6/22/99 12:13PM--------------------------------------------------------------------
	 * now we have to figure out if we've snagged a wire or not
	 * ----------------------------------------------------------------------------------------------------*/

	if (!(P->TailHookFlags & TH_HOOKED) && !(P->Status & PL_SPEED_BOLTER) && collided && V && !V->CatchWire.Flags && ((P->Status & AL_DEVICE_DRIVEN) || (P->AI.Behaviorfunc == AIWaitForTrapOrBolter) || (P->AI.Behaviorfunc == AIFlyCarrierFinal)))
	{
		/* -----------------6/22/99 12:30PM--------------------------------------------------------------------
		 * we should just attach to the first wire here to test the system so far.
		 * ----------------------------------------------------------------------------------------------------*/

		if (((P->TailHookPosition * V->Type->CatchWires.RightNormal) > V->Type->CatchWires.RightNegD) &&
			((P->TailHookPosition * V->Type->CatchWires.LeftNormal) > V->Type->CatchWires.LeftNegD))
			P->TailHookFlags |= TH_IN_CHANNEL;

		our_d = P->TailHookPosition * V->Type->CatchWires.WireNormal;

		if (our_d < V->Type->CatchWires.WireDs[3])
		{
			P->TailHookFlags |= 4;
			iWireCaught=4;
		}
		else
			if (our_d < V->Type->CatchWires.WireDs[2])
			{
				P->TailHookFlags |= 3;
				iWireCaught=3;
			}
			else
				if (our_d < V->Type->CatchWires.WireDs[1])
				{
					P->TailHookFlags |= 2;
					iWireCaught=2;
				}
				else
					if (our_d < V->Type->CatchWires.WireDs[0])
					{
						P->TailHookFlags |= 1;
						iWireCaught=1;
					}

		if (((P->TailHookFlags & TH_BOLTER_FLAG) == TH_BOLTER_FLAG) || ((P->TailHookFlags & 4) && ((P->TailHookFlags & 0x670000) != 0x630000)))
		{
			// past the last hook
			if(((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P))) // && (PlayerPlane->Status & PL_DEVICE_DRIVEN))
			{
				/* -----------------6/24/99 11:13AM--------------------------------------------------------------------
				 * bolter sound call
				 * ----------------------------------------------------------------------------------------------------*/
				AIC_ACLS_Switch(PlayerPlane, 0);
				if(!(PlayerPlane->AI.iAICombatFlags2 & AI_BOLTERED))
				{
					AICJustMessage(AIC_BOLTER, AICF_BOLTER, SPCH_LSO);
					AICAddSoundCall(AIC_BolterStart, PlayerPlane - Planes, 4000, 50);
					PlayerPlane->AI.iAICombatFlags2 |= AI_BOLTERED;
				}
				PlayerPlane->AI.lTimer2 = 120000;
				PlayerPlane->AI.lVar2 = 11;
			}
			else
			{
				if(P->Status & PL_CARRIER_REL)
				{
					for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
					{
						if(checkplane->PlaneCopy == P)
						{
							break;
						}
					}
					if(checkplane > LastPlane)
					{
						checkplane = P;
					}
				}
				else
					checkplane = P;

				fBolterAdjust += (3.0f * FTTOWU);
				checkplane->AI.Behaviorfunc = AIFlyBolter;
				checkplane->AI.lTimer3 = 120000;
				P->AI.Behaviorfunc = AIFlyBolter;
				P->AI.lTimer3 = 120000;

				if((AICAllowThisRadio(checkplane - Planes, 1)) && (!((checkplane->AI.iAICombatFlags2 & AI_BOLTERED) || (P->AI.iAICombatFlags2 & AI_BOLTERED))))
				{
					AICJustMessage(AIC_BOLTER, AICF_BOLTER, SPCH_LSO);
					AICAddSoundCall(AIC_BolterStart, checkplane - Planes, 4000, 50);
					checkplane->AI.iAICombatFlags2 |= AI_BOLTERED;
					P->AI.iAICombatFlags2 |= AI_BOLTERED;
				}
			}
		}
		else
			if ((P->OnGround == 2) && (P->TailHookFlags & (TH_TOUCHED_DOWN<< 16)) && (P->TailHookFlags | TH_TOUCHED_DOWN) && (P->TailHookFlags & (TH_IN_CHANNEL<< 16)) && (P->TailHookFlags | TH_IN_CHANNEL) && (((P->TailHookFlags & 0x70000) >> 16) == ((P->TailHookFlags & 0x7)-1)))
			{

				if(((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P))) // && (PlayerPlane->Status & PL_DEVICE_DRIVEN))
				{
					if((P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED|PL_PLANE_DITCHING)) || (PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED|PL_PLANE_DITCHING)))
						continue_check = FALSE;
					else
						continue_check = TRUE;
				}
				else
					continue_check = TRUE;


				if (((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P)) && (g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_LANDINGS) && (P->V > LANDING_OK))            // 77.0 fps = 130 knots
				{
					// too fast for trap wires.  chance of landing, crashing or bolter.
					if (P->V > LANDING_CRASH_START)
						crash_chance = ((P->V - LANDING_CRASH_START)/(LANDING_NO_BOLTER - LANDING_CRASH_START));    //@250knots 100%chance of crash, no landing
					else
						crash_chance = 0.0f;

					// if we're over weight, give us a chance of being damaged.
					if (PlayerPlane->TotalWeight > 46000)
						crash_chance += (PlayerPlane->TotalWeight - 46000) * 1.0f/6000.0f;

					if (crash_chance > 1.0f)
					{
						crash_chance = 1.0f;
						land_chance = 0.0f;
					}
					else
					{
						if (crash_chance < 0.0f)
							crash_chance = 0.0f;

						land_chance = ((P->V - LANDING_OK)/(LANDING_NO_LAND - LANDING_OK));	//no chance of success at 200 knots
						land_chance = 1.0f - (land_chance*land_chance);
						if (land_chance < 0.0f)
							land_chance = 0.0f;
						land_chance *= 1.0f - crash_chance;
						crash_chance += land_chance;
					}

					landing_roll = frand();

					if (landing_roll > land_chance)
					{
						continue_check = FALSE;
						P->Status |= PL_SPEED_BOLTER;

						if (landing_roll <= crash_chance)
						{
							if(((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P))) //  && (PlayerPlane->Status & PL_DEVICE_DRIVEN))
								AIC_ACLS_Switch(PlayerPlane, 0);
							/* ----------------6/27/99 1:25PM------------------------------------------------------------------
							 * need to put real damage stuff in here
							 * ------------------------------------------------------------------------------------------------*/
							FPoint vel;
							vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
							NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);

							float total_damage = 0.0f;

							if (P->V > LANDING_CRASH_START)
							{
							 	total_damage = (crash_chance - landing_roll)*(P->V - LANDING_CRASH_START)*(1.0f/1.6f);  // around 105 pts damage at 250 knots

								if (total_damage < 0.0f)
									total_damage = 0.0f;
							}

							if (PlayerPlane->TotalWeight > 46000)
								total_damage += ((PlayerPlane->TotalWeight - 46000) * 1.0f/6000.0f) * 50.0f;

							if (total_damage > 100.0f)
								total_damage = 100.0f;

							DistributePlayerDamage(PlayerPlane,total_damage, DamageMultipliers, 0, 0, 15.0f FEET, WEP_SPREAD_DAMAGE);
							if (total_damage > (40.0f + frand()*40.0f))
							{
								PlayerPlane->FlightStatus |= PL_OUT_OF_CONTROL;
								P->FlightStatus |= PL_OUT_OF_CONTROL;
							}
						}
						else
						{
							if(((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P))) //  && (PlayerPlane->Status & PL_DEVICE_DRIVEN))
								AIC_ACLS_Switch(PlayerPlane, 0);
							/* -----------------6/24/99 11:13AM--------------------------------------------------------------------
				 			* bolter sound call for player. AI can't get in here
				 			* ----------------------------------------------------------------------------------------------------*/
							if(!(PlayerPlane->AI.iAICombatFlags2 & AI_BOLTERED))
							{
								AICJustMessage(AIC_BOLTER, AICF_BOLTER, SPCH_LSO);
								AICAddSoundCall(AIC_BolterStart, PlayerPlane - Planes, 4000, 50);
								PlayerPlane->AI.iAICombatFlags2 |= AI_BOLTERED;
							}
							PlayerPlane->AI.lTimer2 = 120000;
							PlayerPlane->AI.lVar2 = 11;
						}
					}


				}

				if (continue_check)
				{
					LandingBurnerSound(P);

					V->CatchWire.CaughtPlane = P;
					V->CatchWire.Flags = CW_PLANE_CAUGHT | ((P->TailHookFlags & 0x70000) >> 16);
 					V->lAIFlags1 |= PLANES_ON_DECK;
					P->TailHookFlags |= TH_HOOKED;

					if ((P == PlayerPlane) || ((P->Status & PL_CARRIER_REL) && (PlayerPlane->PlaneCopy == P)))
					{
						P->Status |= PL_SET_PLAYER_DECEL;
						StartCasualAutoPilot();

						if (iWireCaught!=-1)
						{
							LogMissionStatistic(LOG_WIRE_CATCH,(PlayerPlane-Planes),iWireCaught,0);
						}

						LogMissionStatistic(LOG_FLIGHT_LANDING_TIME,(PlayerPlane-Planes),GameLoop,0);

						AIChangeEventFlag(MISSION_FLAG_START + 11);

						if(V->iSide != PlayerPlane->AI.iSide)
						{
							LogMissionStatistic(LOG_FLIGHT_LANDED_ENEMY,(PlayerPlane-Planes),1,2);
						}
						else
						{
							LogMissionStatistic(LOG_FLIGHT_LANDED_FRIENDLY,(PlayerPlane-Planes),1,2);
						}
					}
					else
					{
						if(P->Status & PL_CARRIER_REL)
						{
							for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
							{
								if(checkplane->PlaneCopy == P)
								{
									break;
								}
							}
							if(checkplane > LastPlane)
							{
								checkplane = P;
							}
						}
						else
							checkplane = P;

						LogMissionStatistic(LOG_FLIGHT_LANDED_FRIENDLY,(checkplane-Planes),1,0);

						if (iWireCaught!=-1)
						{
							LogMissionStatistic(LOG_WIRE_CATCH,(checkplane-Planes),iWireCaught,0);
						}
					}

					P->AI.iHomeBaseId = V-MovingVehicles;
					P->AI.Behaviorfunc = AICarrierTrap;
					P->AI.lVar3 = 0;
					P->UpdateRate = HIGH_FREQ;

					if(MultiPlayer)
					{
						if(P->Status & PL_CARRIER_REL)
						{
							for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
							{
								if(checkplane->PlaneCopy == P)
								{
									break;
								}
							}
							if(checkplane > LastPlane)
							{
								checkplane = P;
							}
						}
						else
							checkplane = P;

						NetPutGenericMessage3Long(checkplane, GM3L_HOOK_WIRE, V - MovingVehicles, P->TailHookFlags);
					}
				}
			}
	}
	else if((V) && ((P->Status & AL_DEVICE_DRIVEN) || (P->AI.Behaviorfunc == AIWaitForTrapOrBolter) || (P->AI.Behaviorfunc == AIFlyCarrierFinal)))
	{
		if((((P->AI.lPlaneID == 64) || (P->AI.lPlaneID == 67)) && (P->OnGround == 2)) || ((pDBShipList[V->iVDBIndex].lShipID == 6) && (!(P->Status & PL_SPEED_BOLTER)) && (P->OnGround == 2)))
		{
			if(((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P))) // && (PlayerPlane->Status & PL_DEVICE_DRIVEN))
			{
				if((P->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED|PL_PLANE_DITCHING)) || (PlayerPlane->FlightStatus & (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED|PL_PLANE_DITCHING)))
					continue_check = FALSE;
				else
					continue_check = TRUE;
			}
			else
				continue_check = TRUE;

			if (((P == PlayerPlane) || (PlayerPlane->PlaneCopy == P)) && (g_Settings.gp.dwFlight & GP_FLIGHT_REALISTIC_LANDINGS) && (P->V > LANDING_OK))            // 77.0 fps = 130 knots
			{
				if(PlayerPlane->AI.iAICombatFlags2 & AI_BOLTERED)
				{
					continue_check = FALSE;
				}

				// too fast for trap wires.  chance of landing, crashing or bolter.
				if (P->V > LANDING_CRASH_START)
					crash_chance = ((P->V - LANDING_CRASH_START)/(LANDING_NO_BOLTER - LANDING_CRASH_START));    //@250knots 100%chance of crash, no landing
				else
					crash_chance = 0.0f;

				if (PlayerPlane->TotalWeight > 46000)
					crash_chance += (PlayerPlane->TotalWeight - 46000) * 1.0f/6000.0f;

				if (crash_chance > 1.0f)
				{
					crash_chance = 1.0f;
					land_chance = 0.0f;
				}
				else
				{
					if (crash_chance < 0.0f)
						crash_chance = 0.0f;

					land_chance = ((P->V - LANDING_OK)/(LANDING_NO_LAND - LANDING_OK));	//no chance of success at 200 knots
					land_chance = 1.0f - (land_chance*land_chance);
					if (land_chance < 0.0f)
						land_chance = 0.0f;
					land_chance *= 1.0f - crash_chance;
					crash_chance += land_chance;
				}

				landing_roll = frand();

				if (landing_roll > land_chance)
				{
					continue_check = FALSE;
					P->Status |= PL_SPEED_BOLTER;

					if (landing_roll <= crash_chance)
					{
						/* ----------------6/27/99 1:25PM------------------------------------------------------------------
						 * need to put real damage stuff in here
						 * ------------------------------------------------------------------------------------------------*/
						FPoint vel;
						vel.MakeVectorToFrom(PlayerPlane->WorldPosition,PlayerPlane->LastWorldPosition);
						NewGenerator(PLANE_EXPLODES,PlayerPlane->WorldPosition,vel,2.0f,25);

						float total_damage = 0.0f;

						if (P->V > LANDING_CRASH_START)
						{
							total_damage = (crash_chance - landing_roll)*(P->V - LANDING_CRASH_START)*(1.0f/1.6f);  // around 105 pts damage at 250 knots

							if (total_damage < 0.0f)
								total_damage = 0.0f;
						}

						if (PlayerPlane->TotalWeight > 46000)
							total_damage += ((PlayerPlane->TotalWeight - 46000) * 1.0f/6000.0f) * 50.0f;

						if (total_damage > 100.0f)
							total_damage = 100.0f;

						DistributePlayerDamage(PlayerPlane,total_damage, DamageMultipliers, 0, 0, 15.0f FEET, WEP_SPREAD_DAMAGE);
						if (total_damage > (40.0f + frand()*40.0f))
						{
							PlayerPlane->FlightStatus |= PL_OUT_OF_CONTROL;
							P->FlightStatus |= PL_OUT_OF_CONTROL;
						}
					}
					else
					{
						/* -----------------6/24/99 11:13AM--------------------------------------------------------------------
				 		* bolter sound call for player. AI can't get in here
				 		* ----------------------------------------------------------------------------------------------------*/
						if(!(PlayerPlane->AI.iAICombatFlags2 & AI_BOLTERED))
						{
							AICJustMessage(AIC_BOLTER, AICF_BOLTER, SPCH_LSO);
							AICAddSoundCall(AIC_BolterStart, PlayerPlane - Planes, 4000, 50);
							PlayerPlane->AI.iAICombatFlags2 |= AI_BOLTERED;
						}
						PlayerPlane->AI.lTimer2 = 120000;
						PlayerPlane->AI.lVar2 = 11;
					}
				}
			}

			if (continue_check)
			{
				LandingBurnerSound(P);

				V->CatchWire.CaughtPlane = P;
				V->CatchWire.Flags = CW_PLANE_CAUGHT | ((P->TailHookFlags & 0x70000) >> 16);
				V->lAIFlags1 |= PLANES_ON_DECK;
				P->TailHookFlags |= TH_HOOKED;

				if ((P == PlayerPlane) || ((P->Status & PL_CARRIER_REL) && (PlayerPlane->PlaneCopy == P)))
				{
					P->Status |= PL_SET_PLAYER_DECEL;
					StartCasualAutoPilot();
				}

				P->AI.iHomeBaseId = V-MovingVehicles;
				P->AI.Behaviorfunc = AICarrierTrap;
				P->AI.lVar3 = 0;
				P->UpdateRate = HIGH_FREQ;

				if(MultiPlayer)
				{
					if(P->Status & PL_CARRIER_REL)
					{
						for(checkplane = Planes; checkplane <= LastPlane; checkplane ++)
						{
							if(checkplane->PlaneCopy == P)
							{
								break;
							}
						}
						if(checkplane > LastPlane)
						{
							checkplane = P;
						}
					}
					else
						checkplane = P;

					NetPutGenericMessage3Long(checkplane, GM3L_HOOK_WIRE, V - MovingVehicles, P->TailHookFlags);
				}
			}
		}
	}

	if ((P->TailHookFlags & TH_HOOKED) && V && (V->CatchWire.CaughtPlane == P))
	 	V->CatchWire.LastPlace = P->TailHookPosition;

	return FALSE;
}

//**************************************************************************************

double GetPlaneLandHeightWithCarriers(PlaneParams *P,MovingVehicleParams **v,double *addnl_height)
{
	BOOL above_a_carrier = FALSE;
	FPoint rel;
	double return_val = 0;
	FMatrix tmat;
	float deck_height;
	double  clip_height;
	double  gear_height;

	if (InHighResTerrainRegion(P->WorldPosition))
		return_val = LandHeight(P->WorldPosition.X, P->WorldPosition.Z);
	else
		return_val = P->WorldPosition.Y - (double)P->HeightAboveGround;

	if (return_val < 200 FEET)
	{
		MovingVehicleParams *W;

		if (v && *v)
			W = *v;
		else
			W = NULL;

		if(W && (W->Status & VL_ACTIVE) && (W->Type->iShipType == SHIP_TYPE_CARRIER) && W->Type && (!(W->Status & (VL_INVISIBLE | VL_DESTROYED))))
		{
			rel.X = (float)(P->WorldPosition.X-W->WorldPosition.X);
			if (rel.X < (5280.0*0.25 FEET))  // 1/4 mile
			{
				rel.Z = (float)(P->WorldPosition.Z - W->WorldPosition.Z);
				if (rel.Z < (5280.0*0.25 FEET))  // 1/4 mile
				{
					rel.Y = *(float *)((int)W->Type->Model + sizeof(ObjectHandler));

					if ((rel.X*rel.X + rel.Z*rel.Z) < (rel.Y*rel.Y))
					{
						rel.Y =(float)(P->WorldPosition.Y - W->WorldPosition.Y);
						tmat = W->Attitude;
						tmat.Transpose();
						rel *= tmat;

						if (OverRunway(W->Type->Model,rel,&deck_height))
						{
							rel.Y = deck_height;

							if (addnl_height)
							{
								FPoint carr_rel_down;
								FPoint carr_rel_fwd;
								FPoint carr_rel_rgt;
								FPoint rel2;

								rel2 = rel;

								carr_rel_down.RotateInto(P->Orientation.K,tmat);
								carr_rel_rgt.RotateInto(P->Orientation.J,tmat);
								carr_rel_fwd.RotateInto(P->Orientation.I,tmat);

								if (carr_rel_down.Y < 0)
								{
									if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
										gear_height = P->Type->GearDownHeight;
									else
										if (P->LandingGear)
											gear_height = P->Type->GearUpHeight + FSinTimes((P->LandingGear << 6),(P->Type->GearDownHeight-P->Type->GearUpHeight));
										else
											gear_height = P->Type->GearUpHeight;

//									if (carr_rel_fwd.Y < 0)
										gear_height += 1.5f*P->Type->GearDownHeight*fabs(carr_rel_fwd.Y); //front gear
//									else
//										gear_height += 0.8f*P->Type->GearDownHeight*carr_rel_fwd.Y; //back gear


								}
								else
									gear_height = P->Type->TailHeight;

								gear_height *= (float)fabs(carr_rel_down.Y * (1.0f - carr_rel_rgt.Y*carr_rel_rgt.Y));

								clip_height  = fabs(carr_rel_fwd.Y * P->Type->ShadowLRXOff);

								if (clip_height < gear_height)	// use gear if larger
									rel2.Y += gear_height;
								else
									rel2.Y += clip_height;

								rel *= W->Attitude;
								rel2 *= W->Attitude;

								*addnl_height = (double)(rel2.Y - rel.Y);

							}
							else
								rel *= W->Attitude;

							if (v)
								*v=W;
							return_val = (double)rel.Y + W->WorldPosition.Y;
							above_a_carrier = TRUE;
						}
					}
				}
			}
		}
		else
			for (MovingVehicleParams *W=MovingVehicles; (W<=LastMovingVehicle) && !(above_a_carrier); W++)
			{
				if((W->Status & VL_ACTIVE) && (W->Type->iShipType == SHIP_TYPE_CARRIER) && W->Type && (!(W->Status & (VL_INVISIBLE | VL_DESTROYED))))
				{
					rel.X = (float)(P->WorldPosition.X-W->WorldPosition.X);
					if (rel.X < (5280.0*0.25 FEET))  // 1/4 mile
					{
						rel.Z = (float)(P->WorldPosition.Z - W->WorldPosition.Z);
						if (rel.Z < (5280.0*0.25 FEET))  // 1/4 mile
						{
							rel.Y = *(float *)((int)W->Type->Model + sizeof(ObjectHandler));

							if ((rel.X*rel.X + rel.Z*rel.Z) < (rel.Y*rel.Y))
							{
								rel.Y =(float)(P->WorldPosition.Y - W->WorldPosition.Y);
								tmat = W->Attitude;
								tmat.Transpose();
								rel *= tmat;

								if (OverRunway(W->Type->Model,rel,&deck_height))
								{
									rel.Y = deck_height;

									if (addnl_height)
									{
										FPoint carr_rel_down;
										FPoint carr_rel_fwd;
										FPoint carr_rel_rgt;
										FPoint rel2;
										rel2 = rel;

										carr_rel_down.RotateInto(P->Orientation.K,tmat);
										carr_rel_rgt.RotateInto(P->Orientation.J,tmat);
										carr_rel_fwd.RotateInto(P->Orientation.I,tmat);

										if (carr_rel_down.Y < 0)
										{
											if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
												gear_height = P->Type->GearDownHeight;
											else
												if (P->LandingGear)
													gear_height = P->Type->GearUpHeight + FSinTimes((P->LandingGear << 6),(P->Type->GearDownHeight-P->Type->GearUpHeight));
												else
													gear_height = P->Type->GearUpHeight;

//											if (carr_rel_fwd.Y < 0)
												gear_height += 1.5f*P->Type->GearDownHeight*fabs(carr_rel_fwd.Y); //front gear
//											else
//												gear_height += 0.8f*P->Type->GearDownHeight*carr_rel_fwd.Y; //back gear


										}
										else
											gear_height = P->Type->TailHeight;

										gear_height *= (float)fabs(carr_rel_down.Y * (1.0f - carr_rel_rgt.Y*carr_rel_rgt.Y));

										clip_height  = fabs(carr_rel_rgt.Y * P->Type->ShadowLRXOff);

										if (clip_height < gear_height)	// use gear if larger
											clip_height = gear_height;

										rel2.Y += clip_height;

										rel *= W->Attitude;
										rel2 *= W->Attitude;

										*addnl_height = rel2.Y - rel.Y;

									}
									else
										rel *= W->Attitude;

									if (v)
										*v=W;
									return_val = (double)rel.Y + W->WorldPosition.Y;
									above_a_carrier = TRUE;
								}
							}
						}
					}
				}
			}
	}

	if (!above_a_carrier)
	{
		if (addnl_height)
		{
			if (P->Orientation.K.Y < 0)
			{
				if (P->FlightStatus & PL_GEAR_DOWN_LOCKED)
					gear_height = P->Type->GearDownHeight;
				else
					if (P->LandingGear)
						gear_height = P->Type->GearUpHeight + FSinTimes((P->LandingGear << 6),(P->Type->GearDownHeight-P->Type->GearUpHeight));
					else
						gear_height = P->Type->GearUpHeight;

//				if (P->Orientation.I.Y < 0)
 					gear_height += 1.5f*P->Type->GearDownHeight*fabs(P->Orientation.I.Y); //front gear
//				else
//					gear_height += 0.8f*P->Type->GearDownHeight*P->Orientation.I.Y; //back gear

			}
			else
				gear_height = P->Type->TailHeight;

			gear_height *= (float)fabs(P->Orientation.K.Y * (1.0f - (P->Orientation.J.Y*P->Orientation.J.Y)));

			clip_height  = fabs(P->Orientation.J.Y * P->Type->ShadowLRXOff);

			if (clip_height < gear_height)	// use gear if larger
				*addnl_height = gear_height;
			else
				*addnl_height = clip_height;
		}

		if (v)
			*v = NULL;
	}

	return return_val;
}

//**************************************************************************************

float SetMeatballVars(FPointDouble &location,MovingVehicleParams *carrier,BYTE *varslist)
{
	double dworkvar;
	double h,v;
	double brightness;
	double amount_wide;
	double amount_high;
	double cur_dist;
//	double secs;
	double flash_brightness;
	BOOL too_high;
	FPointDouble new_carrier_position;
	FPointDouble direction;
	BYTE *varwalk;
	int i;
	float ret_val = 255.0;
	double displayed_glide_slope = dGlideSlope + 0.5;
	float landx = LAND_POINT_X;
	float landy = LAND_POINT_Y;
	float landz = LAND_POINT_Z;

	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
//		deckangle = SOV_DECK_ANGLE;
		landx = SOV_LAND_POINT_X;
		landy = SOV_LAND_POINT_Y;
		landz = SOV_LAND_POINT_Z;
	}

	new_carrier_position.MakeVectorToFrom(location,carrier->WorldPosition);

	float ftoffset = 125.0f + fMeatBallOffsetFT;
	float coneslide = 1000.0f;
#if 1
	if(pDBShipList[carrier->iVDBIndex].lShipID == 6)
	{
		//  Slide cone back 500 ft alot 3.5 degree line.
		//  offset y by 500 * sin(3.5);
		landy -= (coneslide *  0.061f * FTTOWU);

		//  offset x and z by (500 * cos(3.5)) * (cos/sin (deckangle))  deckangle = 7.565f for Sov
		landx -= (coneslide *  0.998f * 0.13f * FTTOWU);
		landz -= (coneslide *  0.998f * 0.9913f * FTTOWU);

		//  Slide ball forward/back
		landx -= (ftoffset * 0.13f * FTTOWU);  //  (8.08f * FTTOWU);
		landz -= (ftoffset * 0.9913f * FTTOWU);  //  (49.343f * FTTOWU);
	}
	else
	{
		//  Slide cone back 500 ft alot 3.5 degree line.
		//  offset y by 500 * sin(3.5);
		landy -= (coneslide *  0.061f * FTTOWU);

		//  offset x and z by (500 * cos(3.5)) * (cos/sin (deckangle))  deckangle = 9.5 for US
		landx -= (coneslide * 0.998f *  0.1616f * FTTOWU);
		landz -= (coneslide * 0.998f *  0.987f * FTTOWU);

		//  Slide ball forward/back
		landx -= (ftoffset * 0.1616f * FTTOWU);  //  (8.08f * FTTOWU);
		landz -= (ftoffset * 0.987f * FTTOWU);  //  (49.343f * FTTOWU);
	}
#else
	if (((PlayerPlane->TailHookState) || (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)) && MeatBallCarrier)
	{
		double gear_height;
		double add_height;

		gear_height = PlayerPlane->Type->GearDownHeight * cos(DegToRad(dGlideSlope));

		add_height = PlayerPlane->Type->ShadowLRXOff * fabs(sin(DegToRad(dGlideSlope)));

		if(add_height < gear_height)
			add_height = gear_height;

//		new_carrier_position.Y -= 50 FEET;
		new_carrier_position.Y += add_height * 2.0f;
	}
#endif

	cur_dist = new_carrier_position.Length();

//	secs = (cur_dist * WUTOFT)/( (130.0 * NMTOFT)/3600.0 - carrier->fSpeedFPS );
	ZeroMemory(varslist+32,17);
//	if (secs >= 0.0)
//	{
		cur_dist *= WUTONM;

		if (cur_dist < 2.0)
		{
			FMatrix mat;

			if (cur_dist > 1.2)
				brightness = (2.0 -cur_dist)/0.8;
			else
				brightness = 1.0;

			dworkvar = (double)ANGLE_TO_RADIANS(carrier->Heading);

			mat.SetRadRPH(0.0,0.0,-dworkvar);

//			direction.SetValues(-sin(dworkvar),0.0,cos(dworkvar));
//			new_carrier_position.SetValues(carrier->fSpeedFPS*secs*FTTOWU,direction);
//			new_carrier_position += carrier->WorldPosition;
//			new_carrier_position.Y = LandHeight(carrier->WorldPosition.X, carrier->WorldPosition.Z);

//			new_carrier_position = carrier->WorldPosition;

//			new_carrier_position.MakeVectorToFrom(location,new_carrier_position);

			new_carrier_position *= mat;
			new_carrier_position.Z -= landz;  //  LAND_POINT_Z;
			new_carrier_position.Y -= landy;  //  LAND_POINT_Y;

			h = fabs(atan2(new_carrier_position.X,new_carrier_position.Z) * (180.0/PId));
			v = atan2(new_carrier_position.Y,new_carrier_position.Z) * (180.0/PId);
			amount_high = fabs(v-(double)displayed_glide_slope);

			if ((amount_high < 21.0) && (amount_high >= 1.0))
			{
				if (amount_high > 11.0)
					too_high = TRUE;
				else
					too_high = FALSE;
				amount_high = fabs(amount_high - 11.0);
				amount_high = 1.0 - (amount_high/10.0);
			}
			else
			{
				if (amount_high >= 21.0)
				{
					too_high = TRUE;
					brightness = 0.0;
				}
				else
					too_high = FALSE;
				amount_high = 0.0;
			}

			if (v < displayed_glide_slope)
				flash_brightness = 1.0 - (double)(GameLoop & 0x1F)/31.0;
			else
				flash_brightness = 1.0 - (double)(GameLoop & 0x3F)/63.0;

			if (flash_brightness > 0.90)
				flash_brightness = (1.0 - flash_brightness)*10;
			else
				flash_brightness = flash_brightness/0.9;

			if (h < 25.0)
			{
				if (h > 20)
					amount_wide = (h-20.0)*0.2;
				else
					amount_wide = 0.0;

				if (amount_high > amount_wide)
					varslist[45]=(BYTE)(brightness * 255.0 * amount_high * flash_brightness);
				else
					varslist[45]=(BYTE)(brightness * 255.0 * amount_wide * flash_brightness);

				ret_val = (1.0-brightness) * h/45 * fabs(v-displayed_glide_slope)/21.0;

				brightness *= 255.0 * (1.0 - amount_high)*(1.0 - amount_wide);

				if (brightness)
					ret_val = TRUE;

				if (!too_high)
					varslist[46]=(BYTE)brightness;

				h = displayed_glide_slope-0.875;
				varwalk = varslist+32;

				i = 32;
				while(i < 44)
				{
					dworkvar = fabs(v-h);
					if (i < 34)
					{
						dworkvar *= 2.0;
						if (i == 34)
							h += 0.25;
						else
							h += 0.125;
					}
					else
					{
						dworkvar *= 4.0;
						h += 0.125;
					}

					if (dworkvar < 1.0)
						*varwalk = (BYTE)(brightness * (1.0-dworkvar));
					i++;
					varwalk++;
				}

			}
			else
				if (h < 45)
				{
					amount_wide = (45.0-h)/20.0;
					if (amount_high > amount_wide)
						varslist[45]=(BYTE)(brightness * 255.0 * amount_high * flash_brightness);
					else
						varslist[45]=(BYTE)(brightness * 255.0 * amount_wide * flash_brightness);

					ret_val = (1.0-brightness) * h/45 * fabs(v-displayed_glide_slope)/21.0;
				}
			if(carrier->lAIFlags1 & V_FLASH_E_WAVEOFF)
			{
				varslist[45]=(BYTE)(brightness * 255.0 * flash_brightness);
			}
			else if(carrier->lAIFlags1 & V_FLASH_WAVEOFF)
			{
				varslist[45]=(BYTE)(brightness * 255.0 * 0.75f * flash_brightness);
			}
		}
//	}

	return ret_val;
}

//**************************************************************************************

float mx = 18.0f FEET;
float my = -7.5f FEET;
float mz = -2.4;

void DrawMeatballForPlane()
{
	GrBuff old_3d_buff;
	FMatrix tempmatrix;
	FMatrix matrix;
	int w,h;
	FPoint location;

	old_3d_buff = *GrBuffFor3D;
	tempmatrix = ViewMatrix;

	matrix.Identity();

	matrix.SetViewMatrix();

	w = (int)((float)ScreenSize.cx * 0.32f);
	h = (int)((float)w * 0.45);

	GrBuffFor3D->MidX =ScreenSize.cx >>1;
	GrBuffFor3D->ClipLeft = (ScreenSize.cx-w) >> 1;
	GrBuffFor3D->ClipRight = GrBuffFor3D->ClipLeft + w-1;
//	GrBuffFor3D->MidX =  GrBuffFor3D->ClipRight-1;

	GrBuffFor3D->MidY = (3*ScreenSize.cy) >> 2;
	GrBuffFor3D->ClipTop = GrBuffFor3D->MidY - (h >> 1);
	GrBuffFor3D->ClipBottom = GrBuffFor3D->ClipTop + h-1;


//	GrBuffFor3D->MidY = GrBuffFor3D->ClipBottom-1;// - (h >> 4);

	Set3DScreenClip();

	D3DRECT rect;

	rect.lX1 = GrBuffFor3D->ClipLeft;
	rect.lY1 = GrBuffFor3D->ClipTop;

	rect.lX2 = GrBuffFor3D->ClipRight;
	rect.lY2 = GrBuffFor3D->ClipBottom;

	lpD3DViewport->Clear2(1,&rect,/*D3DCLEAR_TARGET  |*/ D3DCLEAR_ZBUFFER ,d3dcFogColor,1.0f,0);

//	location.SetValues((float)(18.0 FEET),(float)(-7.5 FEET),(float)-2.4*GetObjectRadius(Meatball));
	location.SetValues(mx,my,mz*GetObjectRadius(Meatball));

	Draw3DObjectMatrix(Meatball,location,matrix,MeatBallLights);

	*GrBuffFor3D = old_3d_buff;

	Set3DScreenClip();

	tempmatrix.SetViewMatrix();
}


//**************************************************************************************
int VGetVGListNumFromResourceID(int resourceid)
{
	int cnt;

	for (cnt=0; cnt<g_iNumMovingVehicle; cnt++)
	{
		if(g_pMovingVehicleList[cnt].lResourceNumber == resourceid)
		{
			return(cnt);
		}
	}
	return(-1);
}

//**************************************************************************************
int VConvertVGListNumToVNum(int vglistnum)
{
	int cnt;
	int vnum = 0;

	if(vglistnum < 0)
	{
		return(-1);
	}

	for(cnt = 0; ((cnt < vglistnum) && (cnt < g_iNumMovingVehicle)); cnt ++)
	{
		vnum += g_pMovingVehicleList[cnt].iNumGroup;
	}

	if(cnt >= g_iNumMovingVehicle)
	{
		return(-1);
	}
	return(vnum);
}

//**************************************************************************************
void VSetShipDamageLevel(int vnum, int level, int initialize_damage)
{
	MovingVehicleParams *vehiclepnt = &MovingVehicles[vnum];
	MovingVehicleParams *leadvehiclepnt;
	MovingVehicleParams *trailvehiclepnt;
	int orgdamage = vehiclepnt->bDamageLevel & 0x7;
	DBShipType *pshiptype;
	int lowdam, highdam;
	int num_fires;
	DWORD *smoke_walker;
	int i;
	float newspeed;
	MovingVehicleParams *tempvehiclepnt;

	if(!vehiclepnt->iShipType)
		return;

	if(!level)
		return;

	pshiptype = &pDBShipList[vehiclepnt->iVDBIndex];

	vehiclepnt->bDamageLevel &= ~orgdamage;

	vehiclepnt->bDamageLevel |= level;

	num_fires = 0;
	smoke_walker = vehiclepnt->Smoke;
	i = MAX_VEHICLE_SMOKE_SLOTS;
	while(i-- && (num_fires < level))
		if (*smoke_walker++)
			num_fires++;

	if (num_fires < level)
	{
		if(PlayerPlane->AI.iAICombatFlags1 & AI_HOME_CARRIER)
		{
			if(vehiclepnt == &MovingVehicles[PlayerPlane->AI.iHomeBaseId])
			{
				if(!(lCarrierMsgFlags & MOTHER_SICK_MSG))
				{
					lCarrierMsgFlags |= MOTHER_SICK_MSG;
					AICJustMessage(AIC_STRIKE_MOTHER_SICK, AICF_STRIKE_MOTHER_SICK, SPCH_STRIKE);

					if(PlayerPlane->AI.iAICombatFlags1 & AI_CARRIER_LANDING)
						AICAddSoundCall(AICAlphaCheck, PlayerPlane - Planes, 10000, 50, 6);
				}
			}
		}

		CanisterType fire_type;
		float fire_size;
		float fire_duration;

		fire_type = CT_NONE;

		switch (pshiptype->iFireType)
		{
			case FIRE_TYPE_HOUSE:
				if (pshiptype->iFireDuration)
				{
					fire_type = CT_BLACK_SMOKE;
					fire_size = 0.5f;
					fire_duration = (float)pshiptype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case FIRE_TYPE_TRUCK:
				if (pshiptype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 0.5f;
					fire_duration = (float)pshiptype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case  FIRE_TYPE_FUELBLADDER:
				if (pshiptype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 1.0f;
					fire_duration = (float)pshiptype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;

			case  FIRE_TYPE_GASTANKER:
				if (pshiptype->iFireDuration)
				{
					fire_type = CT_OIL_FIRE;
					fire_size = 1.5f;
					fire_duration = (float)pshiptype->iFireDuration*(1.5f - frand()) * 60.0f;
				}
				break;
		}

		if (fire_type)
		{
			FPointDouble fire_position;

			while(num_fires++ < level)
			{
				fire_position =0.0;
				PositionFireOnVehicle(vehiclepnt,fire_position,TRUE);
				GetVehicleFireHandle(fire_position,(void *)vehiclepnt,fire_type,fire_size*2.0f,fire_duration);
			}

		}
	}

	//  do speed stuff here.
	newspeed = (float)pshiptype->iMaxSpeed / FTSEC_TO_KNOTS;
	switch(level)
	{
		case 1:
			newspeed *= 0.50f;
			break;
		case 2:
			newspeed *= 0.25f;
			break;
		case 3:
			newspeed = 0;
			break;
		default:
			newspeed = 0;
			break;
	}

	if(newspeed < vehiclepnt->fSpeedFPS)
	{
		if(!vehiclepnt->pLeadVehicle)
		{
			if(pshiptype->lShipType == (SHIP_TYPE_CARRIER))
			{
				trailvehiclepnt = (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
				while(trailvehiclepnt)
				{
					if(newspeed < trailvehiclepnt->fSpeedFPS)
						trailvehiclepnt->fSpeedFPS = newspeed;
					trailvehiclepnt = (MovingVehicleParams *)trailvehiclepnt->pTrailVehicle;
				}
			}
			else
			{
				tempvehiclepnt = vehiclepnt;
				while(tempvehiclepnt->pTrailVehicle)
				{
					trailvehiclepnt = (MovingVehicleParams *)tempvehiclepnt->pTrailVehicle;
					if(trailvehiclepnt->fSpeedFPS <= newspeed)
					{
						tempvehiclepnt = trailvehiclepnt;
					}
					else
					{
						tempvehiclepnt->pTrailVehicle = NULL;
						trailvehiclepnt->pLeadVehicle = NULL;
					}
				}
			}
		}
		vehiclepnt->fSpeedFPS = newspeed;
	}

	//  do listing stuff here.

	//  do smoke stuff here.

	if(initialize_damage)
	{
		switch(level)
		{
			case 1:
				lowdam = 0;
				highdam = pshiptype->iDamageLevel1;
				break;
			case 2:
				lowdam = pshiptype->iDamageLevel1;
				highdam = pshiptype->iDamageLevel2;
				break;
			case 3:
				lowdam = pshiptype->iDamageLevel2;
				highdam = pshiptype->iDamageLevel3;
				break;
			default:		//  Dead, dead, dead.
				vehiclepnt->Status |= VL_DESTROYED|VL_INVISIBLE | VL_SWEPT_UP;
				leadvehiclepnt = (MovingVehicleParams *)vehiclepnt->pLeadVehicle;
				trailvehiclepnt = (MovingVehicleParams *)vehiclepnt->pTrailVehicle;
				if(leadvehiclepnt)
				{
					leadvehiclepnt->pTrailVehicle = trailvehiclepnt;
				}
				if(trailvehiclepnt)
				{
					trailvehiclepnt->pLeadVehicle = leadvehiclepnt;
				}
				break;
		}
		vehiclepnt->fTotalDamage = (float)(lowdam + highdam) / 2.0f;
	}
}

//**************************************************************************************
void VGetShipGroupDamageState(int resourceid, int *iship_damage_array)
{
	int cnt;
	int vglistnum;
	int vnum;

	for(cnt = 0; cnt < 8; cnt ++)
	{
		iship_damage_array[cnt] = 0;
	}

	vglistnum = VGetVGListNumFromResourceID(resourceid);

	if(vglistnum == -1)
	{
		return;
	}
	vnum = VConvertVGListNumToVNum(vglistnum);

	for(cnt = 0; cnt < g_pMovingVehicleList[vglistnum].iNumGroup; cnt ++)
	{
		iship_damage_array[cnt] = MovingVehicles[vnum + cnt].bDamageLevel & 0x7;
	}

}

//**************************************************************************************
void VSetShipGroupDamageState(int resourceid, int *iship_damage_array)
{
	int cnt;
	int vglistnum;
	int vnum;
	int damagelevel;

	vglistnum = VGetVGListNumFromResourceID(resourceid);

	if(vglistnum == -1)
	{
		return;
	}
	vnum = VConvertVGListNumToVNum(vglistnum);

	for(cnt = 0; cnt < g_pMovingVehicleList[vglistnum].iNumGroup; cnt ++)
	{
		damagelevel = iship_damage_array[cnt];
		VSetShipDamageLevel(vnum + cnt, damagelevel, 1);
	}
}

//**************************************************************************************
void VDamageVehicleRadars(MovingVehicleParams *vehiclepnt, FPointDouble *position, float fMultiplier, float fBlastRadius)
{
	float fDistance;
	float fDamageValue;
	GDRadarData *radardat;
	int slotnum;
	int kill;
	FPointDouble vrw_position;
	int bitflags = 0;

	if(vehiclepnt->iShipType)
	{
		for(slotnum = 0; slotnum < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; slotnum ++)
		{
			kill = 0;
			radardat = &vehiclepnt->RadarWeaponData[slotnum];
			if(!(radardat->lRFlags1 & (GD_I_AM_DEAD)))
			{
				vrw_position = vehiclepnt->WorldPosition;
				if(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lDefenseType == VS_DEFENSE_RADAR)
				{
					if(GetRadarPtr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID))
					{
						VGDGetWeaponPosition(vehiclepnt, MOVINGVEHICLE, slotnum, &vrw_position);
						fDistance = vrw_position - *position;
						fDamageValue = fBlastRadius - fDistance;
						fDamageValue = fDamageValue * fDamageValue * fMultiplier;

						if(fDamageValue >= 25.0f)
						{
							kill = 1;
						}
					}
				}
				else
				{
					if(get_weapon_ptr(pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID))
					{
						VGDGetWeaponPosition(vehiclepnt, MOVINGVEHICLE, slotnum, &vrw_position);
						fDistance = vrw_position - *position;
						fDamageValue = fBlastRadius - fDistance;
						fDamageValue = fDamageValue * fDamageValue * fMultiplier;

						if(fDamageValue >= 50.0f)
						{
							kill = 1;
						}
					}
				}

				if(kill)
				{
					radardat = (GDRadarData *)&vehiclepnt->RadarWeaponData[slotnum];
					if(!(radardat->lRFlags1 & GD_I_AM_DEAD))
					{
						bitflags |= 1<<slotnum;
					}
					radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
					radardat->lRFlags1 |= (GD_I_AM_DEAD);
				}
			}
		}
	}
	else
	{
		fDistance = vehiclepnt->WorldPosition - *position;
		fDamageValue = fBlastRadius - fDistance;
		fDamageValue = fDamageValue * fDamageValue * fMultiplier;

		if(fDamageValue >= 25.0f)
		{
			radardat = (GDRadarData *)&vehiclepnt->RadarWeaponData[0];
			if(!(radardat->lRFlags1 & GD_I_AM_DEAD))
			{
				bitflags = 1;
			}
			radardat->lRFlags1 &= ~(GD_RADAR_TEMP_ON|GD_RADAR_ON|GD_RADAR_TEMP_OFF|GD_RADAR_PLAYER_PING);
			radardat->lRFlags1 |= (GD_I_AM_DEAD);
		}
	}

	if((MultiPlayer) && (bitflags))
	{
		NetPutGenericMessage2Long(NULL, GM2L_V_RADARS_DEAD, bitflags, vehiclepnt - MovingVehicles);
	}
}

/* -----------------7/26/99 9:01AM---------------------------------------------------------------------
/* shadows
/* ----------------------------------------------------------------------------------------------------*/

void SetupVehicleShadows(FPointDouble &eye)
{
	BOOL drew;
	FPoint sunken;
	FPoint upvec(0.0f,1.0f,0.0f);
	FPointDouble fsunk;
	FMatrix mat;
	float pangle;
	float ticks;
	float t;

	VehiclesToDraw = NULL;

	for (MovingVehicleParams *W=MovingVehicles; W<=LastMovingVehicle; W++)
	{
		drew = FALSE;
		W->PlaneShadows = NULL;

		if(!W->Type)
			W->Status &= ~(VL_ACTIVE);

		if((W->Status & VL_ACTIVE) && (!(W->Status & (VL_INVISIBLE))))
		{
			if (CloseEnoughToDraw(W->WorldPosition))
			{
				W->NextVehicleToDraw = VehiclesToDraw;
				VehiclesToDraw = W;

				if (/*!((W->Status & VL_ON_BRIDGE) && !(W->Bridge->Basics.Flags & BI_DESTROYED)) &&*/ (!(W->lAIFlags1 & V_NO_SHADOW)))
				{
					drew = TRUE;

					if (W->Status & VL_SINKING)
					{
						ticks = (float)(GameLoop - W->FirstSinkingTick);
						t = ticks * W->SinkingMultiplier;

						sunken.SetValues(0.0f,ticks*(float)((0.8/50.0) FEET),0.0f);

						pangle = (float)(0.15*PI)*t;

						if (W->Status & VL_SINKING_NEG)
							pangle = -pangle;

						mat.SetHPR(0,RADIANS_TO_ANGLE(pangle),RADIANS_TO_ANGLE(pangle*0.5));

						mat *= W->Attitude;

						ShadowArbClip(sunken,upvec,mat);

						GetObjectShadowEntry(&W->Shadow,W->Type->Model,mat,TRUE,NULL,0xFF,(sunken.Y / 5.0f FEET));
						if (W->Shadow)
						{
							fsunk.MakeVectorToFrom(W->WorldPosition,sunken);
							InsertShadow(0,(DWORD)W->Shadow,fsunk,eye);
						}

						ShadowArbClip();
					}
					else
					{
						GetObjectShadowEntry(&W->Shadow,W->Type->Model,W->Attitude);
						if (W->Shadow)
							InsertShadow(0,(DWORD)W->Shadow,W->WorldPosition,eye);
					}



				}
			}
		}

		if (!drew && W->Shadow)
		{
			ReleaseShadowEntry(&W->Shadow);
			W->Shadow = NULL;
		}
	}
}

//**************************************************************************************
int AICheckRadarCrossSig(MovingVehicleParams *vehiclepnt, PlaneParams *checkplane, float daltft, float distnm, DBRadarType *radar, int inSTT)
{
	int crossval = 0;
	int workval;
	int crosssig = pDBAircraftList[checkplane->AI.iPlaneIndex].iRadarCrossSignature;
	int checkedone = 0;
	int checkaspect = 0;
	int useHPRF = 1;
	int useLPRF = 0;

	if(!radar)
	{
		return(0);
	}

	if(radar->iRadarType & RADAR_TYPE_MPRF)
	{
//	   	crossval = GetCalculatedCrossSection(crosssig, MPRF, daltft, AGClosureRate(vehiclepnt, checkplane), distnm, inSTT);
	   	crossval = GetCrossSectionFromGround(crosssig, MPRF, daltft, AGClosureRate(vehiclepnt, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		checkedone = 1;
		useHPRF = 0;
	}


	if(radar->iRadarType & RADAR_TYPE_LPRF)
	{
//	   	workval = GetCalculatedCrossSection(crosssig, HPRF, daltft, AGClosureRate(vehiclepnt, checkplane), distnm, inSTT);
	   	workval = GetCrossSectionFromGround(crosssig, LPRF, daltft, AGClosureRate(vehiclepnt, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		if(workval > crossval)
		{
			crossval = workval;
		}
		useHPRF = 0;
		useLPRF = 1;
		checkedone = 1;
	}

	if((radar->iRadarType & RADAR_TYPE_HPRF) || (!checkedone))
	{
//	   	workval = GetCalculatedCrossSection(crosssig, HPRF, daltft, AGClosureRate(vehiclepnt, checkplane), distnm, inSTT);
	   	workval = GetCrossSectionFromGround(crosssig, HPRF, daltft, AGClosureRate(vehiclepnt, checkplane), distnm, inSTT, (float)radar->iMaxRange);
		if(workval > crossval)
		{
			crossval = workval;
		}
		useHPRF = 1;
	}

	if(crossval >= 4)
	{
		checkaspect = 1;;
	}
	if((crossval >= 3) && (inSTT))
	{
		checkaspect = 1;
	}

	if(!checkaspect)
	{
		return(FALSE);
	}
	if(useLPRF)
	{
		return(TRUE);
	}

	FPointDouble FromRadarToTarget;

	FromRadarToTarget.MakeVectorToFrom(checkplane->WorldPosition,vehiclepnt->WorldPosition);

	FromRadarToTarget.Normalize();  //  I think we need to do this

  FPointDouble ProjPoint;
//	ProjPoint.SetValues(-500.0*checkplane->Attitude.m_Data.RC.R0C2,-500.0*checkplane->Attitude.m_Data.RC.R1C2,
//							                     -500.0*checkplane->Attitude.m_Data.RC.R2C2);
	ProjPoint.SetValues(checkplane->Attitude.m_Data.RC.R0C2,checkplane->Attitude.m_Data.RC.R1C2,
							                     checkplane->Attitude.m_Data.RC.R2C2);


	float Dot = FromRadarToTarget * ProjPoint;

	float Threshold;
	if(radar->iGeneration == 1)
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G1 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G1;
	}
	else if(radar->iGeneration == 2)
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G2 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G2;
	}
	else
	{
		Threshold = (useHPRF) ? AI_CRITICAL_ASPECT_THRESHOLD_HPRF_G3 : AI_CRITICAL_ASPECT_THRESHOLD_MPRF_G3;
	}

	return( fabs(Dot) > Threshold );
}

/*-----------------------------------------------------------------------------
 *
 *	AGClosureRate()
 *
 *		Return TRUE rate of closure between
 *		a ground object and an aircraft target.  (+/- KTS)
 *
 */
float AGClosureRate(MovingVehicleParams *vehiclepnt, PlaneParams *T )
{
	FPoint tVector;

	tVector  = T->WorldPosition;
	tVector -= vehiclepnt->WorldPosition;
	tVector.Normalize();		// unit vector ME -> target

	float V2 = T->IfVelocity * tVector;	// target's speed away from me

	return (-V2) * FTSEC_TO_KNOTS;
}

//*******************************************************************************************************************
int VGetMaxShipRadarRange(MovingVehicleParams *vehiclepnt)
{
	GDRadarData *radardat;
	int maxrange = 0;
	DBRadarType *radar;
	int radarid;
	int slotnum;

	if(!vehiclepnt->iShipType)
	{
		return(0);
	}

	for(slotnum = 0; slotnum < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; slotnum ++)
	{
		radardat = &vehiclepnt->RadarWeaponData[slotnum];
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
		{
			radarid = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID;
			radar = GetRadarPtr(radarid);
			if(radar->iMaxRange > maxrange)
			{
				maxrange = radar->iMaxRange;
			}
		}
	}
	return(maxrange);
}

//*******************************************************************************************************************
DBRadarType *VGetMaxShipRadarPtr(MovingVehicleParams *vehiclepnt)
{
	GDRadarData *radardat;
	int maxrange = 0;
	DBRadarType *radar;
	DBRadarType *maxradar = NULL;
	int radarid;
	int slotnum;

	if(!vehiclepnt->iShipType)
	{
		return(NULL);
	}

	for(slotnum = 0; slotnum < pDBShipList[vehiclepnt->iVDBIndex].iNumDefenseItems; slotnum ++)
	{
		radardat = &vehiclepnt->RadarWeaponData[slotnum];
		if((!(radardat->lRFlags1 & (GD_I_AM_DEAD))) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)))
		{
			radarid = pDBShipList[vehiclepnt->iVDBIndex].DefenseList[slotnum].lTypeID;
			radar = GetRadarPtr(radarid);
			if(radar->iMaxRange > maxrange)
			{
				maxrange = radar->iMaxRange;
				maxradar = radar;
			}
		}
	}
	return(maxradar);
}

//*******************************************************************************************************************

void GetLandingPoint(MovingVehicleParams *V,FPointDouble &location)
{

	FPoint spot = V->Type->CatchWires.Values[1];   //  was Values[1]
	FMatrix mat;

//	spot.X -= (8.08f * FTTOWU);
//	spot.Z -= (49.343f * FTTOWU);

	mat = V->Attitude;
	mat.Transpose();
	spot *= mat;
	location = V->WorldPosition;
	location += spot;
}

//*******************************************************************************************************************

void PositionFireOnVehicle(MovingVehicleParams *V,FPointDouble &world_position,BOOL random)
{
	FMatrix mat;
	FPoint p1,p2;
	FPoint vel(0.0f,-1010.0f,0.0f);
	FPoint normal;
	FPoint step;
	int i;

	if (random)
	{
		p1.Z = p1.X = -GetObjectRadius(V->Type->Model);
		p1.X += 2.0*frand()*(-p1.X);
		p1.X *= 0.15;

		p1.Z += 2.0*frand()*(-p1.Z);
		p1.Z *= 0.80;
	}
	else
	{
		mat = V->Attitude;
		mat.Transpose();

		p1.MakeVectorToFrom(world_position,V->WorldPosition);

		p1 *= mat;
	}

	p1.Y = 1000.0f;
	p2 = p1;
	p2.Y = -10.0f;

	if (!LineCollidesWithModel(V->Type->Model,p1,p2,vel,world_position,normal,FALSE))
	{
		/* -----------------8/13/99 3:00PM---------------------------------------------------------------------
		/* we'll walk toward the center of the ship until we find a place
		/* ----------------------------------------------------------------------------------------------------*/
		step.MakeVectorToFrom(vel,p2);
		step.Y = 0.0f;
		step *= 1.0f/40.0f;
		i = 0;

		do
		{
			i++;
			p2 += step;
			p1 += step;
		} while ((i < 20) && !LineCollidesWithModel(V->Type->Model,p1,p2,vel,world_position,normal,FALSE));

		if (i == 20)
		{
			p2.Y = 0.0f;
			world_position = p2;
		}

	}

	world_position*=V->Attitude;
	world_position+=V->WorldPosition;

}


/* -----------------8/13/99 3:49PM---------------------------------------------------------------------
/* this is called when a FAE goes off nearby
/* ----------------------------------------------------------------------------------------------------*/

void VehicleStartHeatFire(MovingVehicleParams *V,FPointDouble &position,float fDistance,float fBlastRadius,void *plane)
{
	DBVehicleType *pvehicletype;
	float time_to_burn;
	CanisterType smoketype;
	FPoint p1,p2,vel;
	FPoint normal;
	FPointDouble world_position;
	float scale;
	FMatrix mat;

	scale  = 1.0f - (fDistance/fBlastRadius - 0.5);

	if (scale >= (frand()*0.2))
	{
		if (scale > 1.0f)
			scale = 1.0f;

		mat = V->Attitude;
		mat.Transpose();

		p1.MakeVectorToFrom(position,V->WorldPosition);

		p1.Y += 300 FEET;
		p1 *= mat;
		vel.SetValues(-p1.X,-p1.Y,-p1.Z);

		if (V->iShipType)
		{
			FPoint tp1;
			FPoint rads;
			int i;

			i = (int)(frand()*scale*(float)MAX_VEHICLE_SMOKE_SLOTS);
			rads.X = -GetObjectRadius(V->Type->Model)*0.1;
			rads.Y = -GetObjectRadius(V->Type->Model)*0.2;
			rads.Z = -GetObjectRadius(V->Type->Model)*0.4;
			fBlastRadius *= fBlastRadius;

			while(i--)
			{
				p2.X = (-2.0f*frand() + 1.0f);
				p2.Y = (-2.0f*frand() + 1.0f);
				p2.Z = (-2.0f*frand() + 1.0f);

				p2 *= rads;
				tp1 = p1;
				tp1 += p2;
				if (LineCollidesWithModel(V->Type->Model,tp1,p2,vel,world_position,normal,FALSE))
				{
					scale  = 1.0f - (world_position.PartialLength()/fBlastRadius - 0.5);

					if (scale >= (frand()*0.2))
					{
						if (scale > 1.0f)
							scale = 1.0f;

	 					time_to_burn = 2.0f+(frand()*6.0f * (scale + frand()*(1.0f - scale)));

						world_position*=V->Attitude;
						world_position+=V->WorldPosition;

 						GetVehicleFireHandle(world_position,(void *)V,CT_BLACK_SMOKE,2.0f,time_to_burn);
					}
				}
			}
		}
		else
		{
			if (!(V->Status & VL_DESTROYED))
			{
				pvehicletype = &pDBVehicleList[V->iVDBIndex];

				switch(pvehicletype->iArmorType)
				{
					case ARMOR_TYPE_SOFT:
						smoketype = CT_OIL_FIRE;
	 					time_to_burn = (5.0f * 60.0f)*(scale + frand()*(1.0f - scale));
						break;
					case ARMOR_TYPE_NORMAL:
						smoketype = CT_OIL_FIRE;
	 					time_to_burn = (5.0f * 60.0f)*(scale*scale + frand()*(1.0f - scale*scale));
						break;
					case ARMOR_TYPE_ARMORED:
						smoketype = CT_BLACK_SMOKE;
						plane = NULL;
	 					time_to_burn = 2.0f+(frand()*6.0f * (scale + frand()*(1.0f - scale)));
						break;
					default:
						smoketype = CT_BLACK_SMOKE;
						plane = NULL;
	 					time_to_burn = 2.0f+(frand()*6.0f * (scale*scale + frand()*(1.0f - scale*scale)));
						break;
				}


				p2 = 0.0f;

				if (!LineCollidesWithModel(V->Type->Model,p1,p2,vel,world_position,normal,FALSE))
				{
					world_position*=V->Attitude;
					world_position+=V->WorldPosition;
				}
				else
					world_position = V->WorldPosition;

 				GetVehicleFireHandle(world_position,(void *)V,smoketype,1.0f,time_to_burn,plane);
			}

		}

	}
}

void VehicleStartNapalmFire(MovingVehicleParams *V,float scale,void *plane)
{
	DBVehicleType *pvehicletype;
	FPointDouble world_position;
	float time_to_burn;

	if (!(V->Status & VL_DESTROYED) && (!V->iShipType))
	{
		pvehicletype = &pDBVehicleList[V->iVDBIndex];

		switch(pvehicletype->iArmorType)
		{
			case ARMOR_TYPE_SOFT:
	 			time_to_burn = (5.0f * 60.0f)*(scale + frand()*(1.0f - scale));
				break;

			case ARMOR_TYPE_NORMAL:
	 			time_to_burn = (5.0f * 60.0f)*(scale*scale + frand()*(1.0f - scale*scale));
				break;

			case ARMOR_TYPE_ARMORED:
	 			time_to_burn = (scale*3.0f+2.0f*frand())*10.0f;
				plane = NULL;
				break;

			default:
	 			time_to_burn = (scale*3.0f+2.0f*frand())*10.0f;
				plane = NULL;
				break;
		}

		PositionFireOnVehicle(V,world_position,TRUE);
 		GetVehicleFireHandle(world_position,(void *)V,CT_OIL_FIRE,1.0f,time_to_burn,plane);
	}
}

//**************************************************************************************
int AIPutVehicleRadarInTEWSCenter(MovingVehicleParams *vehiclepnt, GDRadarData *radardat)
{
	if(radardat->lRFlags1 & (GD_I_AM_DEAD))
		return(0);

	if(vehiclepnt->Status & (VL_DESTROYED))
	{
		return(0);
	}

	if((radardat->lRFlags1 & GD_RADAR_LOCK) && (radardat->Target == PlayerPlane) && (radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) && (!(radardat->lRFlags1 & (GD_I_AM_DEAD|GD_RADAR_TEMP_OFF))))
	{
		return(1);
	}

	if((!(radardat->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON))) || (radardat->lRFlags1 & (GD_I_AM_DEAD|GD_RADAR_TEMP_OFF)))
		return(0);

	if(!(radardat->lRFlags1 & (GD_CHECK_MISSILES|GD_RADAR_LOCK))) // We have missiles flying we need to keep eye on.
		return(0);

	return(AISeeIfStillVehicleMissiled(vehiclepnt, PlayerPlane, 0, radardat));
}

//**************************************************************************************
int AICheckForGateStealerV(MovingVehicleParams *radarsite, GDRadarData *radardat, PlaneParams *target)
{
	float fworkvar;
	int iworkvar;
	DBRadarType *radar;
	int breaklock;

	if(MultiPlayer)
	{
//		if((MultiPlayer) && ((P->AI.iAICombatFlags1 & AI_MULTI_ACTIVE) || (P == PlayerPlane)))
  		if(!(lAINetFlags1 & NGAI_ACTIVE))
			return(0);
	}

	if(!target)
		return(0);

	if(!(target->AI.iAICombatFlags2 & AI_GATE_STEALER))
	{
		return(0);
	}

	if(!(radardat->lRFlags1 & GD_RADAR_LOCK))
		return(0);

	if(target->Status & PL_AI_DRIVEN)
	{
		target->AI.iAICombatFlags2 |= AI_STT_THREAT;
	}

	// MAY HAVE TO DO SOMETHING HUMAN SPECIFIC
	if(!(target->AI.iAIFlags1 & AIJAMMINGON))
		return(0);

	if(radardat->lRLockTimer > -99999)
	{
		if(radardat->lRLockTimer >= 0)
		{
			radardat->lRLockTimer -= DeltaTicks;
			if(radardat->lRLockTimer < 0)
			{
				if (!radarsite->iShipType) // DONE XSCOTT FIX
				{
					radar = GetRadarPtr(pDBVehicleList[radarsite->iVDBIndex].lRadarID);
				}
				else
				{
					radar = GetRadarPtr(pDBShipList[radarsite->iVDBIndex].DefenseList[radardat - radarsite->RadarWeaponData].lTypeID);
				}
				fworkvar = frand() * 100.0f;

				breaklock = 0;
				switch(radar->iGeneration)
				{
					case 1:
						if(fworkvar < 40.0f)
							breaklock = 1;
						break;
					case 2:
						if(fworkvar < 30.0f)
							breaklock = 1;
						break;
					default:
						if(fworkvar < 20.0f)
							breaklock = 1;
						break;
				}
				if(breaklock)
				{
					radardat->lRFlags1 &= ~(GD_RADAR_LOCK|GD_CHECK_MISSILES);
					radardat->Target = NULL;
					if(radardat->lRActionTimer < 15000)
						radardat->lRActionTimer = 15000;

					if(MultiPlayer)
					{
					}
					return(1);
				}
			}
		}
	}
	else
	{
		fworkvar = frand() * 20000;
		iworkvar = target->AI.iSkill * 6000;
		radardat->lRLockTimer = 10000 + fworkvar + iworkvar;
	}

	return(0);
}

//**************************************************************************************
void VGetShipGroupRadarDamageState(int resourceid, int *iship_radar_damage_array)
{
	int cnt, cnt2;
	int vglistnum;
	int vnum;
	int bitflags;

	for(cnt = 0; cnt < 8; cnt ++)
	{
		iship_radar_damage_array[cnt] = 0;
	}

	vglistnum = VGetVGListNumFromResourceID(resourceid);

	if(vglistnum == -1)
	{
		return;
	}
	vnum = VConvertVGListNumToVNum(vglistnum);

	for(cnt = 0; cnt < g_pMovingVehicleList[vglistnum].iNumGroup; cnt ++)
	{
		bitflags = 0;
		for(cnt2 = 0; cnt2 < NUM_DEFENSE_LIST_ITEMS; cnt2 ++)
		{
			if(MovingVehicles[vnum + cnt].RadarWeaponData[cnt2].lRFlags1 & GD_I_AM_DEAD)
			{
				bitflags |= (1<<cnt2);
			}
		}
		iship_radar_damage_array[cnt] = bitflags;
	}

}

//**************************************************************************************
void VSetShipGroupRadarDamageState(int resourceid, int *iship_radar_damage_array)
{
	int cnt;
	int vglistnum;
	int vnum;
	int cnt2;

	vglistnum = VGetVGListNumFromResourceID(resourceid);

	if(vglistnum == -1)
	{
		return;
	}
	vnum = VConvertVGListNumToVNum(vglistnum);

	for(cnt = 0; cnt < g_pMovingVehicleList[vglistnum].iNumGroup; cnt ++)
	{
		for(cnt2 = 0; cnt2 < NUM_DEFENSE_LIST_ITEMS; cnt2 ++)
		{
			if(iship_radar_damage_array[cnt] & (1<<cnt2))
			{
				MovingVehicles[vnum + cnt].RadarWeaponData[cnt2].lRFlags1 |= GD_I_AM_DEAD;
				MovingVehicles[vnum + cnt].RadarWeaponData[cnt2].lRFlags1 &= ~(GD_RADAR_PLAYER_PING);
			}
		}
	}
}

