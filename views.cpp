#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"

DWORD ViewKeyFlags;

VKCODE vkUp;
VKCODE vkDown;
VKCODE vkLeft;
VKCODE vkRight;
VKCODE vkIn;
VKCODE vkOut;
VKCODE vkIn_slow;
VKCODE vkOut_slow;
VKCODE vkSlew_north;
VKCODE vkSlew_north_slow;
VKCODE vkSlew_south;
VKCODE vkSlew_south_slow;
VKCODE vkSlew_east;
VKCODE vkSlew_east_slow;
VKCODE vkSlew_west;
VKCODE vkSlew_west_slow;
VKCODE vkSlew_up;
VKCODE vkSlew_up_slow;
VKCODE vkSlew_down;
VKCODE vkSlew_down_slow;

VKCODE vkSlide_left;
VKCODE vkSlide_left_slow;

VKCODE vkSlide_right;
VKCODE vkSlide_right_slow;

VKCODE vkZoom_in;
VKCODE vkZoom_out;

extern VKCODE vkGlanceFrontACM;
extern VKCODE vkVirtualMPD;
extern float RTDeltaTicks;
extern float RTSqrtDeltaTicks;
extern AvionicsType Av;

extern int DrawPopMpds;
void ToggleVirtualMpds();
DWORD ScanACMKeyViews(void);
BOOL InACMMode;
float PreACMHeading;
float PreACMPitch;
float PreACMRoll;

extern void setup_no_cockpit_art();
extern BOOL CheckForEjected();

void SP_Set3DGrBuff(GrBuff *source);

#define CAMERA_MIN_HEIGHT (15.0 FEET)
#define MAX_CAM_ALT (70000.0 FEET)

void SetCameraTs()
{
	if (!(Camera1.Flags & CF_TS_SET))
	{
		Camera1.CHASE_T = 0.10f;
		Camera1.RELATIVE_T = 0.25;
		Camera1.RELATIVE_T_DIST = 0.25;
		Camera1.DIST_T = 0.015f;
		Camera1.Flags |= CF_TS_SET;
	}
}

void SwitchCameraSubjectToPoint(FPointDouble &point,BOOL reset_ts)
{
	float otd;
	int old_status = Av.Destructo.Status;
	WeaponParams *old_weapon = Av.Destructo.CurWeap;

	otd = Camera1.RELATIVE_T_DIST;
	Camera1.SubjectPoint = point;
	AssignCameraSubject((void *)&Camera1.SubjectPoint,CO_POINT);
	AssignCameraTarget(NULL,CO_NADA);
	if (!reset_ts)
		Camera1.RELATIVE_T_DIST = otd;

	Av.Destructo.Status = old_status;
	Av.Destructo.CurWeap = old_weapon;

}

void SwitchCameraTargetToPoint(FPointDouble &point)
{
	Camera1.SubjectPoint = point;
	AssignCameraTarget((void *)&Camera1.SubjectPoint,CO_POINT);
}

void DropWeaponCamera()
{
	double speed;
	int old_status = Av.Destructo.Status;
	WeaponParams *old_weapon = Av.Destructo.CurWeap;

	ChangeViewModeOrSubject(CAMERA_RC_VIEW);

	Av.Destructo.Status = old_status;
	Av.Destructo.CurWeap = old_weapon;

	speed = (double)Camera1.SubjectVelocity.Length() * (5.5 + frand()*5.5);

	if (rand()&1)
		speed = -speed;

	Camera1.CameraTargetLocation.SetValues(12.0f+frand()*12.0f,Camera1.SubjectVelocity);

	Camera1.CameraTargetLocation.X += (double)Camera1.SubjectMatrix.m_Data.RC.R0C0 * speed;
	Camera1.CameraTargetLocation.Y += (double)Camera1.SubjectMatrix.m_Data.RC.R1C0 * speed;
	Camera1.CameraTargetLocation.Z += (double)Camera1.SubjectMatrix.m_Data.RC.R2C0 * speed;

	Camera1.CameraTargetLocation += Camera1.SubjectLocation;

	Camera1.RELATIVE_T_DIST = 0.8;
	Camera1.Flags &= ~CF_TS_SET;
}

void CheckForDropWeaponCamera(CameraInstance *camera)
{
	WeaponParams *weap = (WeaponParams *)camera->AttachedObject;

	if (camera->TargetObjectType == CO_NADA)
	{
		if (weap->Altitude < (weap->Vel.Y*-200.0))
			DropWeaponCamera();
	}
	else
		if ((float)(camera->TargetLocation - camera->SubjectLocation) < (float)(camera->SubjectVelocity.QuickLength()*200.0))
			DropWeaponCamera();
}


void AssignCameraSubject(void *sub,CameraObjectType type)
{
	PlaneParams *P;
	BasicInstance *inst;
	MovingVehicleParams *vehicle;
	WeaponParams *weap;
	FPointDouble *fpd;

	if (!sub)
		return;

	if (Av.Destructo.Status )//== 4 /*DESTRUCTO_WAITING_TO_END*/)
	{
		Av.Destructo.Status = 0 /*DESTRUCTO_OFF*/;
		Av.Destructo.CurWeap = NULL;
	}


	Camera1.AttachedObject = (int *)sub;
	Camera1.AttachedObjectType = type;

	SetCameraTs();

	switch (Camera1.AttachedObjectType)
	{
		case CO_PLANE :
			P = (PlaneParams *)Camera1.AttachedObject;
			P->DoAttitude(P);

			Camera1.SubjectMatrix = P->Attitude;
			Camera1.SubjectLocation = P->WorldPosition;

			if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED))
				Camera1.SubjectVelocity.SetValues(HIGH_FREQ * FTTOWU, P->IfVelocity );
			else
				Camera1.SubjectVelocity.SetValues(0.0f);

			if (P->Type && P->Type->Model)
			{
				Camera1.SubjectRadius = GetObjectRadius(P->Type->Model);
				if (P->FlightStatus & PL_STATUS_CRASHED)
					Camera1.SubjectRadius*=10.0;
			}
			else
				Camera1.SubjectRadius = 100.0f FEET;
			break;

		case CO_VEHICLE :
			vehicle = (MovingVehicleParams *)Camera1.AttachedObject;
			Camera1.SubjectMatrix = vehicle->Attitude;
			Camera1.SubjectLocation = vehicle->WorldPosition;

			if ((vehicle->Status & VL_ACTIVE) && !(vehicle->Status & (VL_DESTROYED | VL_DONE_MOVING)))
			{
				Camera1.SubjectVelocity.SetValues(0.0f,0.0f,-vehicle->fSpeedFPS * HIGH_FREQ * FTTOWU);
				Camera1.SubjectVelocity *= Camera1.SubjectMatrix;
			}
			else
				Camera1.SubjectVelocity.SetValues(0.0f);

			if (vehicle->Type && vehicle->Type->Model)
				Camera1.SubjectRadius = GetObjectRadius(vehicle->Type->Model);
			else
				Camera1.SubjectRadius = 30.0f FEET;

			break;

        case CO_WEAPON :
			weap = (WeaponParams *)Camera1.AttachedObject;

			if (weap->Kind != EJECTION_SEAT)
			{
				FPoint x,y,z;

				z.SetValues(-weap->Vel.X,-weap->Vel.Y,-weap->Vel.Z);
				z.Normalize();

				y.SetValues(0.0f,1.0f,0.0f);

				x = y;
				x %= z;
				x.Normalize();

				y = z;
				y %= x;
				y.Normalize();

				weap->Attitude.m_Data.RC.R0C0 = x.X;
				weap->Attitude.m_Data.RC.R1C0 = x.Y;
				weap->Attitude.m_Data.RC.R2C0 = x.Z;

				weap->Attitude.m_Data.RC.R0C1 = y.X;
				weap->Attitude.m_Data.RC.R1C1 = y.Y;
				weap->Attitude.m_Data.RC.R2C1 = y.Z;

				weap->Attitude.m_Data.RC.R0C2 = z.X;
				weap->Attitude.m_Data.RC.R1C2 = z.Y;
				weap->Attitude.m_Data.RC.R2C2 = z.Z;
			}

			Camera1.SubjectMatrix = weap->Attitude;
			Camera1.SubjectLocation = weap->Pos;
			Camera1.SubjectVelocity = weap->Vel;
			if (weap->Type && weap->Type->Model)
				Camera1.SubjectRadius = GetObjectRadius(weap->Type->Model);
			else
				Camera1.SubjectRadius = 30.0f FEET;

			if ((weap->pTarget) && (weap->Kind == MISSILE))
			{
				/* ------------------------------------------1/30/98 10:05PM-------------------------------------------
			 	* because missiles have targets, and we want to do something special when it
			 	* gets close, we'll set up the target here as well
			 	* ----------------------------------------------------------------------------------------------------*/

				if (weap->iTargetType == TARGET_PLANE)
					AssignCameraTarget((void *)weap->pTarget,CO_PLANE);
				else
					if (weap->iTargetType == TARGET_GROUND)
						AssignCameraTarget((void *)weap->pTarget,CO_GROUND_OBJECT);
					else
						if (weap->iTargetType == TARGET_VEHICLE)
							AssignCameraTarget((void *)weap->pTarget,CO_VEHICLE);
						else
							AssignCameraTarget(NULL,CO_NADA);

			}
			else
				AssignCameraTarget(NULL,CO_NADA);
			break;

		case CO_GROUND_OBJECT :
			inst = (BasicInstance *)Camera1.AttachedObject;
			Camera1.SubjectLocation = inst->Position;
			Camera1.SubjectMatrix.Identity();
			Camera1.SubjectVelocity.SetValues(0.0f);
			if ((((StructureInstance *)inst)->Registration) && (((StructureInstance *)inst)->Registration->Model))
				Camera1.SubjectRadius = GetObjectRadius(((StructureInstance *)inst)->Registration->Model);
			else
				Camera1.SubjectRadius = 500.0f FEET;

			break;

		case CO_POINT :
			fpd = (FPointDouble *)Camera1.AttachedObject;
			Camera1.SubjectLocation = *fpd;
			Camera1.SubjectMatrix.Identity();
			Camera1.SubjectVelocity.SetValues(0.0f);
			Camera1.SubjectRadius = 10.0f FEET;
			break;

		case CO_NADA :
			break;
	}
}

void AssignCameraTarget(void *sub,CameraObjectType type)
{
	PlaneParams *P;
	BasicInstance *inst;
	MovingVehicleParams *vehicle;
	WeaponParams *weap;
	FPointDouble *fpd;

	Camera1.TargetObject = (int *)sub;
	Camera1.TargetObjectType = type;

	SetCameraTs();

	switch (Camera1.TargetObjectType)
	{
		case CO_PLANE :
			P = (PlaneParams *)Camera1.TargetObject;
			P->DoAttitude(P);
			Camera1.TargetLocation = P->WorldPosition;
			if (P->Type && P->Type->Model)
				Camera1.TargetRadius = GetObjectRadius(P->Type->Model);
			else
				Camera1.TargetRadius = 100.0f FEET;
			break;

		case CO_VEHICLE :
			vehicle = (MovingVehicleParams *)Camera1.TargetObject;
			Camera1.TargetLocation = vehicle->WorldPosition;
			if (vehicle->Type && vehicle->Type->Model)
				Camera1.TargetRadius = GetObjectRadius(vehicle->Type->Model);
			else
				Camera1.TargetRadius = 30.0f FEET;
			break;

        case CO_WEAPON :
			weap = (WeaponParams *)Camera1.TargetObject;
			Camera1.TargetLocation = weap->Pos;
			if (weap->Type && weap->Type->Model)
				Camera1.TargetRadius = GetObjectRadius(weap->Type->Model);
			else
				Camera1.TargetRadius = 30.0f FEET;
			break;

		case CO_GROUND_OBJECT :
			inst = (BasicInstance *)Camera1.TargetObject;
			Camera1.TargetLocation = inst->Position;
			if ((((StructureInstance *)inst)->Registration) && (((StructureInstance *)inst)->Registration->Model))
				Camera1.TargetRadius = GetObjectRadius(((StructureInstance *)inst)->Registration->Model);
			else
				Camera1.TargetRadius = 500.0f FEET;
			break;

		case CO_POINT :
			fpd = (FPointDouble *)Camera1.TargetObject;
			Camera1.TargetLocation = *fpd;
			Camera1.TargetRadius = 10.0f FEET;
			break;

		case CO_NADA :
			break;
	}
}

void FixToChaseMatrix()
{
	FMatrix temp;

	temp = Camera1.SubjectMatrix;
	temp.Transpose();

	Camera1.CameraRelativeMatrix = Camera1.CameraMatrix;
	Camera1.CameraRelativeMatrix *= temp;
	Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
}

void FixFromChaseMatrix()
{
	Camera1.CameraRelativeMatrix = Camera1.CameraMatrix;
}

void NewFlybyState()
{
	ANGLE roll,pitch,heading;
	ANGLE new_heading,new_pitch;
	ANGLE r;
	FMatrix offm;
	FPointDouble offset;
	FPointDouble two;
	float speed;
	float fr;


	if ((speed = Camera1.SubjectVelocity.QuickLength()) < (float)(10.0 MILES_PER_HOUR))
	{
		Camera1.State = CAMERA_STATE_PARABOLIC;
		Camera1.ParabolicConstant = Camera1.SubjectRadius*3.0;
		Camera1.OneOverParabolicConstant = 1.0/Camera1.ParabolicConstant;
		Camera1.SubjectMatrix.GetRPH(&roll,&pitch,&heading);

		r = rand()%12;
		new_heading = (r / 3) << 14;
		new_heading |= ((r % 3)+1)<<12;
		new_pitch = rand()%0x800;

		Camera1.ParabolaOrientation.SetRPH(0,new_pitch,new_heading);

		// Adding this to the camera position will make sure that the camera doesn't come down below the
		// subject;

		Camera1.ParabolaYOffset = (FSin(new_pitch)+frand()) * Camera1.ParabolicConstant;
		Camera1.InitialX = (3.0 + 5.0 * (frand() * frand()));

		Camera1.TotalLoops = (int)(Camera1.InitialX * 200.0);  // 4 seconds to swing past the subject

		if (rand()&1)
			Camera1.InitialX *= -Camera1.ParabolicConstant;
		else
			Camera1.InitialX *= Camera1.ParabolicConstant;

		Camera1.XStep = -(Camera1.InitialX/(double)(Camera1.TotalLoops));

		Camera1.BirthLoop = GameLoop;
		Camera1.TotalLoops += 600;
		Camera1.LoopsLeft = Camera1.TotalLoops;

		Camera1.CameraRelativeMatrix = Camera1.ParabolaOrientation;

		Camera1.CameraLocation.X = Camera1.InitialX;
		Camera1.CameraLocation.Y = 0.0f;
		Camera1.CameraLocation.Z = -(Camera1.ParabolicConstant*(Camera1.InitialX*Camera1.InitialX)-Camera1.ParabolicConstant);

		Camera1.CameraLocation *= Camera1.CameraRelativeMatrix;
		Camera1.CameraLocation.Y += Camera1.ParabolaYOffset;
		Camera1.CameraLocation +=  Camera1.SubjectLocation;

		offset.Y = LandHeight(Camera1.CameraLocation.X,Camera1.CameraLocation.Z)+CAMERA_MIN_HEIGHT;

		if (Camera1.CameraLocation.Y < offset.Y)
			Camera1.CameraLocation.Y = offset.Y;

		if (Camera1.CameraLocation.Y > MAX_CAM_ALT)
			Camera1.CameraLocation.Y = MAX_CAM_ALT;

		Camera1.CameraTargetLocation =  Camera1.CameraLocation;

		offset.MakeVectorToFrom(Camera1.SubjectLocation,Camera1.CameraLocation);
		Camera1.Roll = 0;
		Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
		Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

		camera_setup_initial_relative_quats();

	}
	else
	{
		/* ------------------------------------------2/8/98 2:29PM---------------------------------------------
		 * first we have to determine where this shot will come from.  The choices are pretty simple.
		 *
		 * 1) Faraway 15% (10 seconds away):
		 * 		a. On the Ground
		 * 		b. From Above
		 *
		 * 2) RaceDay View 15% (5 seconds away subject runs past non-panning camera)
		 * 		a. low to ground (left or right)
		 * 		b. Just above (left or right)
		 * 		c. Camera Rises as objects pass.
		 *
		 * 3) TrackCamera	70
		 * 		a. Placed ahead and slightly off axis.
		 * ----------------------------------------------------------------------------------------------------*/

		fr = (float)frand();

		if (fr < 0.2f)
		{
			Camera1.State = CAMERA_STATE_PARABOLIC;
			Camera1.ParabolicConstant = Camera1.SubjectRadius*3.0;
			Camera1.OneOverParabolicConstant = 1.0/Camera1.ParabolicConstant;
			Camera1.SubjectMatrix.GetRPH(&roll,&pitch,&heading);

			r = rand()%12;
			new_heading = (r / 3) << 14;
			new_heading |= ((r % 3)+1)<<12;
			new_pitch = rand()%0x800;

			Camera1.ParabolaOrientation.SetRPH(0,new_pitch,new_heading);

			// Adding this to the camera position will make sure that the camera doesn't come down below the
			// subject;

			Camera1.ParabolaYOffset = (FSin(new_pitch)+frand()) * Camera1.ParabolicConstant;
			Camera1.InitialX = (3.0 + 5.0 * (frand() * frand()));

			Camera1.TotalLoops = (int)(Camera1.InitialX * 200.0);  // 4 seconds to swing past the subject

			if (rand()&1)
				Camera1.InitialX *= -Camera1.ParabolicConstant;
			else
				Camera1.InitialX *= Camera1.ParabolicConstant;

			Camera1.XStep = -(Camera1.InitialX/(double)(Camera1.TotalLoops));

			Camera1.BirthLoop = GameLoop;
			Camera1.TotalLoops += 600;
			Camera1.LoopsLeft = Camera1.TotalLoops;

			Camera1.CameraRelativeMatrix = Camera1.ParabolaOrientation;

			Camera1.CameraLocation.X = Camera1.InitialX;
			Camera1.CameraLocation.Y = 0.0f;
			Camera1.CameraLocation.Z = -(Camera1.ParabolicConstant*(Camera1.InitialX*Camera1.InitialX)-Camera1.ParabolicConstant);

			Camera1.CameraLocation *= Camera1.CameraRelativeMatrix;
			Camera1.CameraLocation.Y += Camera1.ParabolaYOffset;
			Camera1.CameraLocation +=  Camera1.SubjectLocation;

			offset.Y = LandHeight(Camera1.CameraLocation.X,Camera1.CameraLocation.Z)+CAMERA_MIN_HEIGHT;

			if (Camera1.CameraLocation.Y < offset.Y)
				Camera1.CameraLocation.Y = offset.Y;

			if (Camera1.CameraLocation.Y > MAX_CAM_ALT)
				Camera1.CameraLocation.Y = MAX_CAM_ALT;

			Camera1.CameraTargetLocation =  Camera1.CameraLocation;

			offset.MakeVectorToFrom(Camera1.SubjectLocation,Camera1.CameraLocation);
			Camera1.Roll = 0;
			Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
			Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

			camera_setup_initial_relative_quats();

		}
		else
			if (fr < 0.6f )                   // simple tracking option
			{
				Camera1.State = CAMERA_STATE_SIMPLE_RC;

				Camera1.TotalLoops = 3.0 * 50.0 + (5.0 * 50.0)*frand();

				Camera1.CameraLocation.SetValues((double)Camera1.TotalLoops,Camera1.SubjectVelocity);

				offset.SetValues(Camera1.SubjectRadius*(1.0+frand()*4.0),0.0,0.0);

				r = rand() & 0xFFFF;

				if ((Camera1.AttachedObjectType == CO_VEHICLE) || (speed < (250 MILES_PER_HOUR)))
					r &= 0x7FFF;

				offm.SetRPH(r,0,0);
				offm *= Camera1.SubjectMatrix;

				offset *= offm;

				Camera1.CameraLocation += offset;
				Camera1.CameraLocation += Camera1.SubjectLocation;
				Camera1.CameraLocation.Y += FOOT_MULTER;

				offset.Y = LandHeight(Camera1.CameraLocation.X,Camera1.CameraLocation.Z)+CAMERA_MIN_HEIGHT;

				if (Camera1.CameraLocation.Y < offset.Y)
					Camera1.CameraLocation.Y = offset.Y;

				if (Camera1.CameraLocation.Y > MAX_CAM_ALT)
					Camera1.CameraLocation.Y = MAX_CAM_ALT;

				Camera1.CameraTargetLocation = Camera1.CameraLocation;

				offset.MakeVectorToFrom(Camera1.SubjectLocation,Camera1.CameraLocation);

				Camera1.Roll = 0;
				Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
				Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

				camera_setup_initial_relative_quats();

				Camera1.TotalLoops += 250;

				Camera1.BirthLoop = GameLoop;
				Camera1.LoopsLeft = Camera1.TotalLoops;

				Camera1.SubType = 0;
			}
			else
				if (fr < 0.80f)
				{
					Camera1.State = CAMERA_STATE_STATIC_VIEW;

					Camera1.TotalLoops = 3.0 * 50.0 + (5.0 * 50.0)*frand();

					offset.SetValues((double)Camera1.TotalLoops,Camera1.SubjectVelocity);
					Camera1.CameraLocation = offset;
					offset.Normalize();

					if (rand() & 1)
						r = 0x4000;
					else
						r = 0xE000;

					Camera1.UpVelocity.SetValues(0.0);

					if (!(rand()&3))
					{
						Camera1.TargetUpVelocity.SetValues(0.0,0.0,-0.2*Camera1.SubjectVelocity.Length());
						offm.SetRPH(0,0x4000,0);
						offm *= Camera1.SubjectMatrix;
						Camera1.TargetUpVelocity *= offm;
					}
					else
						Camera1.TargetUpVelocity.SetValues(0.0);

					if (rand()&2)
						offm.SetRPH(0,0,r);
					else
						offm.SetRPH(0,0x4000,r);

					offset *= offm;
					offset *= (double)Camera1.SubjectRadius*1.5;

					Camera1.CameraLocation += offset;
					Camera1.CameraLocation += Camera1.SubjectLocation;
					Camera1.CameraLocation.Y += FOOT_MULTER;

					offset.Y = LandHeight(Camera1.CameraLocation.X,Camera1.CameraLocation.Z)+CAMERA_MIN_HEIGHT;

					if (Camera1.CameraLocation.Y < offset.Y)
						Camera1.CameraLocation.Y = offset.Y;

					if (Camera1.CameraLocation.Y > MAX_CAM_ALT)
						Camera1.CameraLocation.Y = MAX_CAM_ALT;

					Camera1.CameraTargetLocation = Camera1.CameraLocation;

					offset.MakeVectorToFrom(Camera1.SubjectLocation,Camera1.CameraLocation);

					Camera1.Roll = 0;
					Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
					Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

					camera_setup_initial_relative_quats();

					Camera1.TotalLoops += 250;

					Camera1.BirthLoop = GameLoop;
					Camera1.LoopsLeft = Camera1.TotalLoops;
				}
				else
				{
					Camera1.State = CAMERA_STATE_SIMPLE_RC;

					Camera1.TotalLoops = 3.0 * 50.0 + (5.0 * 50.0)*frand();

					Camera1.CameraLocation.SetValues((double)Camera1.TotalLoops,Camera1.SubjectVelocity);
					two = (Camera1.CameraLocation += Camera1.SubjectLocation);

					offset.SetValues(-(double)speed*(double)Camera1.TotalLoops,0.0,0.0);

					r = rand() & 0x3FFF;

					if ((Camera1.AttachedObjectType == CO_VEHICLE) || (speed < (250 MILES_PER_HOUR)))
						r &= 0x1FFF;

					r -= 0x1FFF;

					if (rand()&1)
						offm.SetRPH(r,0,0);
					else
						offm.SetRPH(0x8000-r,0,0);

					offm *= Camera1.SubjectMatrix;
					offset *= offm;

					Camera1.CameraLocation += offset;
					Camera1.CameraLocation.Y += FOOT_MULTER;

					if (!LOS(&two,&Camera1.CameraLocation,&offset,MED_RES_LOS,TRUE))
						Camera1.CameraLocation = offset;

					offset.Y = LandHeight(Camera1.CameraLocation.X,Camera1.CameraLocation.Z)+CAMERA_MIN_HEIGHT;

					if (Camera1.CameraLocation.Y < offset.Y)
						Camera1.CameraLocation.Y = offset.Y;

					if (Camera1.CameraLocation.Y > MAX_CAM_ALT)
						Camera1.CameraLocation.Y = MAX_CAM_ALT;


					Camera1.CameraTargetLocation = Camera1.CameraLocation;

					offset.MakeVectorToFrom(Camera1.SubjectLocation,Camera1.CameraLocation);

					Camera1.Roll = 0;
					Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
					Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

					camera_setup_initial_relative_quats();

					Camera1.TotalLoops *= 2;

					Camera1.BirthLoop = GameLoop;
					Camera1.LoopsLeft = Camera1.TotalLoops;

				}

	}


}


void ChangeViewModeOrSubject(DWORD to_mode)
{
	FPointDouble vector,vector2;

	SetCameraTs();

	if (Av.Destructo.Status )// == 4 /*DESTRUCTO_WAITING_TO_END*/)
	{
		Av.Destructo.Status = 0 /*DESTRUCTO_OFF*/;
		Av.Destructo.CurWeap = NULL;
	}

	switch (to_mode)
	{
		case CAMERA_COCKPIT :
			break;

		case CAMERA_CHASE :

			if ((Camera1.CameraLocation - Camera1.SubjectLocation) > (12.0 * TILE_SIZE))
				Camera1.CameraMode = CAMERA_CHASE;

			switch(Camera1.CameraMode)
			{
				case CAMERA_FIXED_CHASE :
					FixToChaseMatrix();
					Camera1.CameraRelativeMatrix.GetRPH(&Camera1.Roll,&Camera1.Pitch,&Camera1.Heading);
					Camera1.Roll = 0;
					camera_setup_relative_quats();
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;
					break;

				case CAMERA_RC_VIEW :
				case CAMERA_FLYBY :
				case CAMERA_CINEMATIC :
				case CAMERA_TACTICAL :
					// set up where we want to be
					Camera1.CameraRelativeMatrix.Identity();
					Camera1.Roll = 0;
					Camera1.Pitch = 0;
					Camera1.Heading = 0;
					Camera1.CameraRelativeToQuat = Camera1.CameraRelativeMatrix;
					Camera1.TargetDistanceFromObject = Camera1.SubjectRadius * 5.0f;

					// set up where we are
					FixToChaseMatrix();
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;
					Camera1.DistanceFromObject = Camera1.CameraLocation / Camera1.SubjectLocation;
					break;


				default:
					Camera1.CameraRelativeMatrix.Identity();
					Camera1.CameraRelativeToQuat = Camera1.CameraRelativeMatrix;
					Camera1.Roll = 0;
					Camera1.Pitch = 0;
					Camera1.Heading = 0;

					Camera1.CameraRelativeMatrix.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) + 0x7000);
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;

					Camera1.DistanceFromObject = Camera1.SubjectRadius*15.0f;
					Camera1.TargetDistanceFromObject = Camera1.SubjectRadius * 5.0f;
					break;
			}
			Camera1.SubType = 0;
			break;


		case CAMERA_FIXED_CHASE :
			if ((Camera1.CameraLocation - Camera1.SubjectLocation) > (12.0 * TILE_SIZE))
				Camera1.CameraMode = CAMERA_FIXED_CHASE;

			switch(Camera1.CameraMode)
			{
				case CAMERA_CHASE :
					FixFromChaseMatrix();
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;
					Camera1.CameraRelativeMatrix.GetRPH(&Camera1.Roll,&Camera1.Pitch,&Camera1.Heading);
					Camera1.Roll = 0;
					camera_setup_relative_quats();
					break;

				case CAMERA_RC_VIEW :
				case CAMERA_FLYBY :
				case CAMERA_CINEMATIC :
				case CAMERA_TACTICAL :
					// set up where we want to be
					Camera1.CameraRelativeMatrix = Camera1.SubjectMatrix;
					Camera1.CameraRelativeMatrix.GetRPH(&Camera1.Roll,&Camera1.Pitch,&Camera1.Heading);

					Camera1.Roll = 0;
					Camera1.Pitch = 0;
					camera_setup_relative_quats();

					Camera1.TargetDistanceFromObject = Camera1.SubjectRadius * 5.0f;

					// set up where we are
					FixFromChaseMatrix();
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;
					Camera1.DistanceFromObject = Camera1.CameraLocation / Camera1.SubjectLocation;
					break;


				default:
					Camera1.SubjectMatrix.GetRPH(&Camera1.Roll,&Camera1.Pitch,&Camera1.Heading);
					Camera1.Roll = 0;
					Camera1.Pitch = 0;
					Camera1.CameraRelativeQuatMatrix.SetRPH(0,0,Camera1.Heading);
					Camera1.CameraRelativeToQuat = Camera1.CameraRelativeQuatMatrix;

					Camera1.CameraRelativeMatrix.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) + 0x7000);
					Camera1.CameraRelativeMatrix *= Camera1.SubjectMatrix;
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;

					Camera1.DistanceFromObject = Camera1.SubjectRadius*15.0f;
					Camera1.TargetDistanceFromObject = Camera1.SubjectRadius * 5.0f;
					break;
			}
			Camera1.SubType = 0;
			break;


		case CAMERA_FLYBY :
			Camera1.SubType = 0;

			NewFlybyState();


			break;


		case CAMERA_CINEMATIC :
			Camera1.SubType = 0;
			break;


		case CAMERA_TACTICAL :

			/* ------------------------------------------1/29/98 4:35PM--------------------------------------------
			 * this will figure out where we want to be
			 * ----------------------------------------------------------------------------------------------------*/

			vector2 = Camera1.SubjectLocation;
			vector2.Y += Camera1.DistanceFromObject * 0.20f;

			vector.MakeVectorToFrom(Camera1.TargetLocation,vector2);

			Camera1.Roll = 0;
			Camera1.Pitch = (ANGLE)(atan2(vector.Y,QuickDistance(vector.X,vector.Z))*(32768.0f/PI));
			Camera1.Heading = (ANGLE)(atan2(-vector.X,-vector.Z)*(32768.0f/PI));

			camera_setup_relative_quats();

			if ((Camera1.CameraLocation - Camera1.SubjectLocation) > (12.0 * TILE_SIZE))
				Camera1.CameraMode = CAMERA_COCKPIT;

			switch(Camera1.CameraMode)
			{
				case CAMERA_RC_VIEW :
				case CAMERA_FLYBY :
				case CAMERA_CINEMATIC :
				case CAMERA_TACTICAL :
					Camera1.TargetDistanceFromObject = Camera1.SubjectRadius * 5.0f;
					Camera1.DistanceFromObject = Camera1.CameraLocation / Camera1.SubjectLocation;

				case CAMERA_FIXED_CHASE :
				case CAMERA_CHASE :
					// set up where we are
					FixFromChaseMatrix();
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;
					break;

				default:
					Camera1.CameraRelativeMatrix.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) + 0x7000);
					Camera1.CameraRelativeMatrix *= Camera1.SubjectMatrix;
					Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
					Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;

					Camera1.DistanceFromObject = Camera1.SubjectRadius*15.0f;
					Camera1.TargetDistanceFromObject = Camera1.SubjectRadius * 5.0f;
					break;
			}
			Camera1.SubType = 0;
			break;


		case CAMERA_REVERSE_TACTICAL :
			/* ------------------------------------------1/29/98 4:35PM--------------------------------------------
			* this will figure out where we want to be
			* ----------------------------------------------------------------------------------------------------*/

			vector2 = Camera1.TargetLocation;
			vector2.Y += Camera1.DistanceFromObject * 0.20f;

			vector.MakeVectorToFrom(Camera1.SubjectLocation,vector2);

			Camera1.Roll = 0;
			Camera1.Pitch = (ANGLE)(atan2(vector.Y,QuickDistance(vector.X,vector.Z))*(32768.0f/PI));
			Camera1.Heading = (ANGLE)(atan2(-vector.X,-vector.Z)*(32768.0f/PI));

			camera_setup_relative_quats();

			Camera1.CameraRelativeMatrix.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) + 0x7000);
			Camera1.CameraRelativeMatrix *= Camera1.SubjectMatrix;
			Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;

			Camera1.DistanceFromObject = Camera1.TargetRadius*15.0f;
			Camera1.TargetDistanceFromObject = Camera1.TargetRadius * 5.0f;

			Camera1.SubType = 0;
			break;


		case CAMERA_RC_VIEW :
			if (Camera1.CameraMode == CAMERA_COCKPIT)
			{
				FMatrix offm;
				FPointDouble offset;
				PlaneParams *p;
				WeaponParams *weap;

				if(Camera1.AttachedObjectType == CO_PLANE)
				{
					p = (PlaneParams *)Camera1.AttachedObject;

					offm.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) - 0x1000);
					offset.SetValues( HIGH_FREQ * FTTOWU * 150.0, p->IfVelocity ); //3 seconds
					offset *= offm;
					offset += p->WorldPosition;
					camera_setup_initial_location(offset);

					offset.MakeVectorToFrom(p->WorldPosition,Camera1.CameraLocation);
				}
				else
				{
					weap = (WeaponParams *)Camera1.AttachedObject;

					offm.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) - 0x1000);
					offset.SetValues( 150.0f, weap->Vel); //3 seconds
					offset *= offm;
					offset += weap->Pos;
					camera_setup_initial_location(offset);

					offset.MakeVectorToFrom(weap->Pos,Camera1.CameraLocation);
				}

				Camera1.Roll = 0;
				Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
				Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

				camera_setup_initial_relative_quats();
				Camera1.SubType = 0;

			}
			else
			{
				FixFromChaseMatrix();
				Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
				camera_setup_initial_location(Camera1.CameraLocation);
			}
			Camera1.SubType = 0;
			break;

		case CAMERA_FREE :
			if (Camera1.CameraMode == CAMERA_COCKPIT)
			{
				FMatrix offm;
				FPointDouble offset;
				PlaneParams *p;
				WeaponParams *weap;

				if(Camera1.AttachedObjectType == CO_PLANE)
				{
					p = (PlaneParams *)Camera1.AttachedObject;

					offm.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) - 0x1000);
					offset.SetValues( HIGH_FREQ * FTTOWU * 150.0, p->IfVelocity ); //3 seconds
					offset *= offm;
					offset += p->WorldPosition;
					camera_setup_initial_location(offset);

					offset.MakeVectorToFrom(p->WorldPosition,Camera1.CameraLocation);
				}
				else
				{
					weap = (WeaponParams *)Camera1.AttachedObject;

					offm.SetRPH(0,(ANGLE)(rand() & 0x1FFF) - 0x1000,(ANGLE)(rand() & 0x1FFF) - 0x1000);
					offset.SetValues( 150.0f, weap->Vel); //3 seconds
					offset *= offm;
					offset += weap->Pos;
					camera_setup_initial_location(offset);

					offset.MakeVectorToFrom(weap->Pos,Camera1.CameraLocation);
				}

				Camera1.Roll = 0;
				Camera1.Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
				Camera1.Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

				camera_setup_initial_relative_quats();
			}
			else
			{
				FixFromChaseMatrix();
				Camera1.CameraRelativeMatrix.GetRPH(&Camera1.Roll,&Camera1.Pitch,&Camera1.Heading);
				Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeMatrix;
	   			Camera1.CameraCurrentQuat = Camera1.CameraRelativeCurrentQuat;
				Camera1.Roll = 0;
				camera_setup_relative_quats();
				camera_setup_initial_location(Camera1.CameraLocation);
			}
			Camera1.SubType = 0;
			break;


	}


	Camera1.CameraMode = to_mode;
}




void camera_setup_initial_relative_quats()
{
	Camera1.CameraRelativeQuatMatrix.SetRPH(0,Camera1.Pitch,Camera1.Heading);
	Camera1.CameraRelativeCurrentQuat = Camera1.CameraRelativeQuatMatrix;
	Camera1.CameraRelativeToQuat = Camera1.CameraRelativeCurrentQuat;
}

void camera_setup_relative_quats()
{
	Camera1.CameraRelativeQuatMatrix.SetRPH(0,Camera1.Pitch,Camera1.Heading);
	Camera1.CameraRelativeToQuat = Camera1.CameraRelativeQuatMatrix;
}

void camera_setup_initial_distance(float dist)
{
	Camera1.DistanceFromObject = dist;
	Camera1.TargetDistanceFromObject = dist;
}

void camera_setup_relative_distance(float dist)
{
	Camera1.TargetDistanceFromObject = dist;
}

void camera_setup_initial_location(FPointDouble &location)
{
	Camera1.CameraLocation = location;
	Camera1.CameraTargetLocation = location;
}

void camera_setup_relative_location(FPointDouble &location)
{
	Camera1.CameraTargetLocation = location;
}


void view_up_pressed(BOOL held)
{
	double angle;
	if (held)
		angle = (double)0x2000 * (double)RTDeltaTicks * 0.001;
	else
		angle = (double)0x200;

	if (Camera1.CameraMode != CAMERA_COCKPIT)
	{
		if (Camera1.CameraMode != CAMERA_FREE)
			Camera1.Pitch -= angle;
		else
			Camera1.Pitch += angle;

		camera_setup_relative_quats();
	}
	else
		Camera1.TargetPitch += angle;
}

void view_down_pressed(BOOL held)
{
	double angle;
	if (held)
		angle = (double)0x2000 * (double)RTDeltaTicks * 0.001;
	else
		angle = (double)0x100;

	if (Camera1.CameraMode != CAMERA_COCKPIT)
	{
		if (Camera1.CameraMode != CAMERA_FREE)
			Camera1.Pitch += angle;
		else
			Camera1.Pitch -= angle;

		camera_setup_relative_quats();
	}
	else
		Camera1.TargetPitch -= angle;
}

void view_left_pressed(BOOL held)
{
	double angle;
	if (held)
		angle = (double)0x3000 * (double)RTDeltaTicks * 0.001;
	else
		angle = (double)0x300;

	if (Camera1.CameraMode != CAMERA_COCKPIT)
	{
		if (Camera1.CameraMode != CAMERA_FREE)
			Camera1.Heading -= angle;
		else
			Camera1.Heading += angle;

		camera_setup_relative_quats();
	}
	else
		Camera1.TargetHeading += angle;
}

void view_right_pressed(BOOL held)
{
	double angle;
	if (held)
		angle = (double)0x3000 * (double)RTDeltaTicks * 0.001;
	else
		angle = (double)0x300;

	if (Camera1.CameraMode != CAMERA_COCKPIT)
	{
		if (Camera1.CameraMode != CAMERA_FREE)
			Camera1.Heading += angle;
		else
			Camera1.Heading -= angle;

		camera_setup_relative_quats();
	}
	else
		Camera1.TargetHeading -= angle;
}

void view_in_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(150.0f*FOOT_MULTER*RTDeltaTicks*0.01f,ViewVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
	else
	{
		Camera1.TargetDistanceFromObject *= 1.0f/(1.0f + __min(Camera1.DIST_T*RTSqrtDeltaTicks,0.3f));

		if (Camera1.TargetDistanceFromObject < (Camera1.SubjectRadius*1.1-ZClipPlane))
			Camera1.TargetDistanceFromObject = (Camera1.SubjectRadius*1.1-ZClipPlane);
	}

}

void view_out_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(-150.0f*FOOT_MULTER*RTDeltaTicks*0.01f,ViewVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
	else
	{
		Camera1.TargetDistanceFromObject *= 1.0f + __min(Camera1.DIST_T*RTSqrtDeltaTicks,0.3f);

		if (Camera1.TargetDistanceFromObject > (float)(12.5 * TILE_SIZE))
			Camera1.TargetDistanceFromObject = (float)(12.5 * TILE_SIZE);
	}
}

void view_in_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
	 	Camera1.CameraTargetLocation.AddScaledVector(50.0f*FOOT_MULTER*RTDeltaTicks*0.01f,ViewVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
	else
	{
	 	Camera1.TargetDistanceFromObject *= 1.0f/(1.0f + __min(Camera1.DIST_T*0.25f*RTSqrtDeltaTicks,0.15f));

		if (Camera1.TargetDistanceFromObject < (Camera1.SubjectRadius*1.1-ZClipPlane))
			Camera1.TargetDistanceFromObject = (Camera1.SubjectRadius*1.1-ZClipPlane);
	}

}

void view_out_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(-50.0f*FOOT_MULTER*RTDeltaTicks*0.01f,ViewVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
	else
	{
		Camera1.TargetDistanceFromObject *= 1.0f + __min(Camera1.DIST_T*0.25f*RTSqrtDeltaTicks,0.15f);

		if (Camera1.TargetDistanceFromObject > (float)(12.5 * TILE_SIZE))
			Camera1.TargetDistanceFromObject = (float)(12.5 * TILE_SIZE);
	}
}

void view_slew_north_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.Z -= 150.0f*FOOT_MULTER*RTDeltaTicks*0.01f;

}

void view_slew_north_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.Z -= 50.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}

void view_slew_south_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.Z += 150.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}


void view_slew_south_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.Z += 50.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}


void view_slew_east_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.X += 150.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}


void view_slew_east_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.X += 50.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}


void view_slew_west_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.X -= 150.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}


void view_slew_west_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
		Camera1.CameraTargetLocation.X -= 50.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
}


void view_slew_up_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.Y += 50.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}

}


void view_slew_up_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.Y += 10.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}

}


void view_slew_down_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.Y -= 50.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
}


void view_slew_down_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.Y -= 10.0f*FOOT_MULTER*RTDeltaTicks*0.01f;
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
}

void view_slide_left_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(-150.0*FOOT_MULTER*RTDeltaTicks*0.01f,RightVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}

}

void view_slide_left_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(-50.0*FOOT_MULTER*RTDeltaTicks*0.01f,RightVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}

}

void view_slide_right_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(150.0*FOOT_MULTER*RTDeltaTicks*0.01f,RightVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}

}

void view_slide_right_slow_pressed(BOOL held)
{
	if ((Camera1.CameraMode == CAMERA_FREE) || (Camera1.CameraMode == CAMERA_RC_VIEW))
	{
		Camera1.CameraTargetLocation.AddScaledVector(50.0*FOOT_MULTER*RTDeltaTicks*0.01f,RightVector);
		if (Camera1.CameraTargetLocation.Y > MAX_CAM_ALT)
			Camera1.CameraTargetLocation.Y = MAX_CAM_ALT;
	}
}

void view_zoom_in_pressed(BOOL held)
{
}

void view_zoom_out_pressed(BOOL held)
{
}

void SetupClipAreas( void )
{
	if (CockpitSeat != NO_SEAT)
		GrBuffFor3D->MidY = (int)((float)ScreenSize.cy * 0.24375f);//117;
	else
		GrBuffFor3D->MidY = (int)((float)ScreenSize.cy * 0.5f);//117;
	GrBuffFor3D->ClipBottom = ScreenSize.cy-1;
	GrBuffFor3D->MidX = ScreenSize.cx >> 1;

}




//***********************************************************************************************************************************
void SetupView(int View)
{
	if (View == CurrentView)
		return;

	CurrentView = View;

	SetupClipAreas();

	Set3DScreenClip();
	SP_Set3DGrBuff( GrBuffFor3D );
}



float debug_amount = -1.0f FEET;

void process_camera_relative_quat(CameraInstance *camera)
{
	Quat from_quat;

	from_quat = camera->CameraRelativeCurrentQuat;

	camera->CameraRelativeCurrentQuat.SLERP(from_quat,camera->CameraRelativeToQuat,__min(camera->RELATIVE_T * RTSqrtDeltaTicks * 0.09f,1.0f));
	camera->CameraRelativeMatrix = camera->CameraRelativeCurrentQuat;
}

void process_camera_relative_distance(CameraInstance *camera)
{
	float delta;
	delta = camera->TargetDistanceFromObject - camera->DistanceFromObject;
	if (fabs(delta) < 0.00001)
		camera->DistanceFromObject = camera->TargetDistanceFromObject;
	else
		camera->DistanceFromObject += delta * __min(camera->RELATIVE_T_DIST * RTSqrtDeltaTicks * 0.09f,1.0f);
}

void process_camera_location(CameraInstance *camera)
{
	FPointDouble delta;

	delta.Y = LandHeight(camera->CameraTargetLocation.X,camera->CameraTargetLocation.Z)+CAMERA_MIN_HEIGHT;

	if (camera->CameraTargetLocation.Y < delta.Y)
		camera->CameraTargetLocation.Y = delta.Y;

	if (camera->CameraTargetLocation.Y > MAX_CAM_ALT)
		camera->CameraTargetLocation.Y = MAX_CAM_ALT;


	delta.MakeVectorToFrom(camera->CameraTargetLocation,camera->CameraLocation);

	if (delta.QuickLength() < 0.05f)
		camera->CameraLocation = camera->CameraTargetLocation;
	else
	{
		delta *= __min(camera->RELATIVE_T_DIST * RTSqrtDeltaTicks * 0.09f,1.0f);
		camera->CameraLocation += delta;
	}

}

void process_camera_mpd_zoom_t(CameraInstance *camera)
{
}

void process_flyby_views(CameraInstance *camera)
{
	int loops;
	FPointDouble offset;

	loops = (int)(GameLoop - camera->BirthLoop);
	if (loops >= camera->TotalLoops)
	{
		loops -= camera->TotalLoops;
		NewFlybyState();
		camera->BirthLoop -= loops;
	}

	if (camera->State == CAMERA_STATE_PARABOLIC)
	{
		camera->CameraLocation.X = camera->InitialX + ((double)loops)*camera->XStep;
		camera->CameraLocation.Y = 0.0f;
		camera->CameraLocation.Z = -(camera->OneOverParabolicConstant*(camera->CameraLocation.X*camera->CameraLocation.X)-camera->ParabolicConstant);

		camera->CameraLocation *= camera->ParabolaOrientation;
		camera->CameraLocation.Y += camera->ParabolaYOffset;
		camera->CameraLocation +=  camera->SubjectLocation;

		offset.Y = LandHeight(camera->CameraLocation.X,camera->CameraLocation.Z)+CAMERA_MIN_HEIGHT;

		if (camera->CameraLocation.Y < offset.Y)
			camera->CameraLocation.Y = offset.Y;

		if (camera->CameraLocation.Y > MAX_CAM_ALT)
			camera->CameraLocation.Y = MAX_CAM_ALT;

		camera->CameraTargetLocation =  camera->CameraLocation;

		offset.MakeVectorToFrom(camera->SubjectLocation,camera->CameraLocation);

		camera->Roll = 0;
		camera->Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0f/PI));
		camera->Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0f/PI));

		camera_setup_relative_quats();
		process_camera_relative_quat(camera);

	}
	else
		if (camera->State == CAMERA_STATE_SIMPLE_RC)
		{
			offset.MakeVectorToFrom(camera->SubjectLocation,camera->CameraLocation);

			camera->Roll = 0;
			camera->Pitch = (ANGLE)(atan2(offset.Y,QuickDistance(offset.X,offset.Z))*(32768.0/(double)PI));
			camera->Heading = (ANGLE)(atan2(-offset.X,-offset.Z)*(32768.0/(double)PI));

			camera_setup_relative_quats();
			process_camera_relative_quat(camera);
		}
		else
			if (camera->State = CAMERA_STATE_STATIC_VIEW)
			{
				if ((camera->TotalLoops - (GameLoop - camera->BirthLoop)) < 350.0)
				{
					if (loops = GameLoop - LastGameLoop)
					{
						offset.MakeVectorToFrom(camera->TargetUpVelocity,camera->UpVelocity);
						camera->UpVelocity.AddScaledVector((double)(__min(camera->RELATIVE_T_DIST * RTSqrtDeltaTicks * 0.09f,1.0f)),offset);
						camera->CameraLocation.AddScaledVector((double)loops,camera->UpVelocity);
						camera->CameraTargetLocation = camera->CameraLocation;
					}
				}
				process_camera_relative_quat(camera);
			}

}

#define KEYCHECK(def,variable,fn) 	if (ViewKeyFlags & def) \
					   				{\
					   					if (GetVkStatus(variable))\
					   					{\
					   						if (Camera1.RegisteredFlags & def)\
												fn(TRUE);\
										}\
										else\
											ViewKeyFlags &= ~def;\
									}

float xmove = 0.06f;
float ymove = 0.08f;
float zmove = 0.09f;
ANGLE pmove = 0xE400;

//***********************************************************************************************************************************
void CalculateCamera(CameraInstance *camera)
{
	PlaneParams *P;
	BasicInstance *inst;
	MovingVehicleParams *vehicle;
	WeaponParams *weap;
	FPoint vector,vector2;
	FPoint *offset;
	FPointDouble *fpd;
	FMatrix mat;
	float camhead;
	ANGLE Pitch=0;
	DWORD commands;
	Quat from_quat,to_quat;
	float acm_speed_mult;
	int resetpadlocktimer = 1;

	/* ------------------------------------------2/4/98 10:14AM--------------------------------------------
	 * this may seem out of place, but we're going to figure out wether to draw the virtual MPDS here.
	 * ----------------------------------------------------------------------------------------------------*/

	if (!LastGameLoop)
		AssignCameraSubject((void *)camera->AttachedObject,camera->AttachedObjectType);

	camera->LastSubjectLocation = camera->SubjectLocation;
	camera->LastTargetLocation = camera->TargetLocation;
	camera->LastCameraLocation = camera->CameraLocation;

	commands = 0;

	if (camera->TargetObjectType)
	{
		switch (camera->TargetObjectType)
		{
			case CO_PLANE :
				P = (PlaneParams *)camera->TargetObject;
				P->DoAttitude(P);
				camera->TargetLocation = P->WorldPosition;
				break;

			case CO_VEHICLE :
				vehicle = (MovingVehicleParams *)camera->TargetObject;
				camera->TargetLocation = vehicle->WorldPosition;
				//camera->SubjectVelocity = P->IfVelocity;
				break;

        	case CO_WEAPON :
				weap = (WeaponParams *)camera->TargetObject;
				camera->TargetLocation = weap->Pos;
				break;

			case CO_GROUND_OBJECT :
				inst = (BasicInstance *)camera->TargetObject;
				camera->TargetLocation = inst->Position;
				break;

			case CO_POINT :
				fpd = (FPointDouble *)camera->TargetObject;
				camera->TargetLocation = *fpd;
				break;

			case CO_NADA :
				break;
		}
	}


	if (camera->AttachedObjectType)
	{
		switch (camera->AttachedObjectType)
		{
			case CO_PLANE :
				P = (PlaneParams *)camera->AttachedObject;
				P->DoAttitude(P);
				camera->SubjectMatrix = P->Attitude;
				camera->SubjectLocation = P->WorldPosition;

				if ((P->Status & PL_ACTIVE) && !(P->FlightStatus & PL_STATUS_CRASHED))
					camera->SubjectVelocity.SetValues(HIGH_FREQ * FTTOWU, P->IfVelocity );
				else
					camera->SubjectVelocity.SetValues(0.0f);
				break;

			case CO_VEHICLE :
				vehicle = (MovingVehicleParams *)camera->AttachedObject;
				camera->SubjectMatrix = vehicle->Attitude;
				camera->SubjectLocation = vehicle->WorldPosition;

				if ((vehicle->Status & VL_ACTIVE) && !(vehicle->Status & (VL_DESTROYED | VL_DONE_MOVING)))
				{
					camera->SubjectVelocity.SetValues(0.0f,0.0f,-vehicle->fSpeedFPS * HIGH_FREQ * FTTOWU);
					camera->SubjectVelocity *= camera->SubjectMatrix;
				}
				else
					camera->SubjectVelocity.SetValues(0.0f);
				break;

        	case CO_WEAPON :
				weap = (WeaponParams *)camera->AttachedObject;

				if (weap->Kind != EJECTION_SEAT)
				{
					FPoint x,y,z;

					z.SetValues(-weap->Vel.X,-weap->Vel.Y,-weap->Vel.Z);
					z.Normalize();

					y.SetValues(0.0f,1.0f,0.0f);

					x = y;
					x %= z;
					x.Normalize();

					y = z;
					y %= x;
					y.Normalize();

					weap->Attitude.m_Data.RC.R0C0 = x.X;
					weap->Attitude.m_Data.RC.R1C0 = x.Y;
					weap->Attitude.m_Data.RC.R2C0 = x.Z;

					weap->Attitude.m_Data.RC.R0C1 = y.X;
					weap->Attitude.m_Data.RC.R1C1 = y.Y;
					weap->Attitude.m_Data.RC.R2C1 = y.Z;

					weap->Attitude.m_Data.RC.R0C2 = z.X;
					weap->Attitude.m_Data.RC.R1C2 = z.Y;
					weap->Attitude.m_Data.RC.R2C2 = z.Z;
				}

				camera->SubjectMatrix = weap->Attitude;
				camera->SubjectLocation = weap->Pos;
				camera->SubjectVelocity = weap->Vel;

				if ((camera->CameraMode == CAMERA_FIXED_CHASE) || (camera->CameraMode == CAMERA_CHASE))
				{
					if(weap->Kind != EJECTION_SEAT)
					{
						CheckForDropWeaponCamera(camera);
					}
					else if(weap->Altitude > (10.0f * FTTOWU))
					{
						CheckForDropWeaponCamera(camera);
					}
				}
				break;

			case CO_GROUND_OBJECT :
				inst = (BasicInstance *)camera->AttachedObject;
				camera->SubjectLocation = inst->Position;
				break;

			case CO_POINT :
				fpd = (FPointDouble *)camera->AttachedObject;
				camera->SubjectLocation = *fpd;
				break;

			case CO_NADA :
				break;
		}
	}


	if (ViewKeyFlags)
	{

		SetCameraTs();

		KEYCHECK(CAMERA_UP_PRESSED,vkUp,view_up_pressed)

		KEYCHECK(CAMERA_DOWN_PRESSED,vkDown,view_down_pressed)

		KEYCHECK(CAMERA_LEFT_PRESSED,vkLeft,view_left_pressed)

		KEYCHECK(CAMERA_RIGHT_PRESSED,vkRight,view_right_pressed)

		KEYCHECK(CAMERA_IN_PRESSED,vkIn,view_in_pressed);

		KEYCHECK(CAMERA_OUT_PRESSED,vkOut,view_out_pressed);

		KEYCHECK(CAMERA_IN_SLOW_PRESSED,vkIn_slow,view_in_slow_pressed);

		KEYCHECK(CAMERA_OUT_SLOW_PRESSED,vkOut_slow,view_out_slow_pressed);

		KEYCHECK(CAMERA_SLEW_NORTH_PRESSED,vkSlew_north,view_slew_north_pressed);

		KEYCHECK(CAMERA_SLEW_NORTH_SLOW_PRESSED,vkSlew_north_slow,view_slew_north_slow_pressed);

		KEYCHECK(CAMERA_SLEW_SOUTH_PRESSED,vkSlew_south,view_slew_south_pressed);

		KEYCHECK(CAMERA_SLEW_SOUTH_SLOW_PRESSED,vkSlew_south_slow,view_slew_south_slow_pressed);

		KEYCHECK(CAMERA_SLEW_EAST_PRESSED,vkSlew_east,view_slew_east_pressed);

		KEYCHECK(CAMERA_SLEW_EAST_SLOW_PRESSED,vkSlew_east_slow,view_slew_east_slow_pressed);

		KEYCHECK(CAMERA_SLEW_WEST_PRESSED,vkSlew_west,view_slew_west_pressed);

		KEYCHECK(CAMERA_SLEW_WEST_SLOW_PRESSED,vkSlew_west_slow,view_slew_west_slow_pressed);

		KEYCHECK(CAMERA_SLEW_UP_PRESSED,vkSlew_up,view_slew_up_pressed);

		KEYCHECK(CAMERA_SLEW_UP_SLOW_PRESSED,vkSlew_up_slow,view_slew_up_slow_pressed);

		KEYCHECK(CAMERA_SLEW_DOWN_PRESSED,vkSlew_down,view_slew_down_pressed);

		KEYCHECK(CAMERA_SLEW_DOWN_SLOW_PRESSED,vkSlew_down_slow,view_slew_down_slow_pressed);

		KEYCHECK(CAMERA_SLIDE_LEFT_PRESSED,vkSlide_left,view_slide_left_pressed);

		KEYCHECK(CAMERA_SLIDE_LEFT_SLOW_PRESSED,vkSlide_left_slow,view_slide_left_slow_pressed);

		KEYCHECK(CAMERA_SLIDE_RIGHT_PRESSED,vkSlide_right,view_slide_right_pressed);

		KEYCHECK(CAMERA_SLIDE_RIGHT_SLOW_PRESSED,vkSlide_right_slow,view_slide_right_slow_pressed);

		Camera1.RegisteredFlags = ViewKeyFlags;
	}

//	if (camera->SubType & COCKPIT_ACM_MODE)
//		ScanACMKeyViews();

	if (camera->CameraMode == CAMERA_COCKPIT)
	{
		DWORD ACM_mode = ScanACMKeyViews();

		/* ------------------------------------------1/24/98 11:27AM-------------------------------------------
		 * since we're inside the cockpit
		 * ----------------------------------------------------------------------------------------------------*/
//		Camera1.TargetObjectType = CO_NADA;
		camera->CameraLocation = camera->SubjectLocation;
	   	VC.HeadPosition = 0.0f;
		*(FPoint *)&VC.xdelta = 0.0f;

/* -----------------8/18/99 12:28PM--------------------------------------------------------------------
/* this is a test for screen shaking
/* ----------------------------------------------------------------------------------------------------*/
//SetupClipAreas();

//GrBuffFor3D->MidX += (int)(frand()*8.0f) - 4;
//GrBuffFor3D->MidY += (int)(frand()*8.0f) - 4;

//Set3DScreenClip();
//SP_Set3DGrBuff( GrBuffFor3D );
/* -----------------8/18/99 12:28PM--------------------------------------------------------------------
/* this is a test for screen shaking
/* ----------------------------------------------------------------------------------------------------*/


		switch(camera->AttachedObjectType)
		{
			case CO_PLANE:

				offset = &P->Type->Offsets[OFFSET_COCKPIT_FRONT];

				camera->CameraLocation.AddScaledVector(offset->Z,P->Orientation.I);
				camera->CameraLocation.AddScaledVector(offset->Y,P->Orientation.K);
				camera->CameraLocation.AddScaledVector(offset->X,P->Orientation.J);


				if ((camera->SubType & COCKPIT_ZOOM_MPD) && !ACM_mode)
				{
					InACMMode = FALSE;

					camera->TargetHeading = 0;
					camera->TargetPitch = 0;
					camera->TargetRoll = 0;

					camera->CurrentHeading = 0;
					camera->CurrentPitch = 0;
					camera->CurrentRoll = 0;

					commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;
					camera->Heading = 0;
					camera->Roll = 0;

					switch (camera->SubType & COCKPIT_ZOOM_MPD)
					{
						case COCKPIT_ZOOM_LEFT_MPD:
							camera->Heading = 0;
							camera->Pitch = 0;
							camera->Roll = 0;

							VC.xdelta = xmove;
							VC.ydelta = ymove;
							VC.zdelta = zmove;
							VC.HeadPosition.SetValues( VC.xdelta, VC.ydelta, VC.zdelta );
							break;

						case COCKPIT_ZOOM_CENTER_MPD:
							camera->Heading = 0;
							camera->Pitch = 0;
							camera->Roll = 0;

							VC.xdelta = -xmove;
							VC.ydelta = ymove;
							VC.zdelta = zmove;
							VC.HeadPosition.SetValues( VC.xdelta, VC.ydelta, VC.zdelta );
							break;

						case COCKPIT_ZOOM_RIGHT_MPD:
							camera->Heading = 0;
							camera->Pitch = 0;
							camera->Roll = 0;

							VC.xdelta = 0.0;
							VC.ydelta = ymove;
							VC.zdelta = zmove;
							VC.HeadPosition.SetValues( VC.xdelta, VC.ydelta, VC.zdelta );
							break;

						case COCKPIT_ZOOM_DOWN_MPD:
							camera->Pitch = pmove;

							VC.xdelta = 0.0;
							VC.ydelta = ymove;
							VC.zdelta = zmove;
							VC.HeadPosition.SetValues( VC.xdelta, VC.ydelta, VC.zdelta );
							break;
					}

					VC.HeadPosition *= camera->SubjectMatrix;
					camera->CameraLocation -= VC.HeadPosition;

				}
				else
					if (camera->SubType & COCKPIT_VIRTUAL_SEAT)
					{
						float  maxpitch;

						if (!(g_Settings.misc.dwMisc & GP_MISC_VIEW_MPDS_POPUPS_STICKY))
							if (GetVkStatus(vkVirtualMPD))
							{
								if (!DrawPopMpds)
								{
									ToggleVirtualMpds();
									OurShowCursor( TRUE );
								}
							}
							else
							{
								if (DrawPopMpds)
								{
									ToggleVirtualMpds();
									/*
									LONG x = GetSystemMetrics(SM_CXSCREEN)>>1;
									LONG y = GetSystemMetrics(SM_CYSCREEN)>>1;

									SetCursorPos(x, y);
									ppLastMouse.x = x;
									ppLastMouse.y = y;
									ppCurrMouse.x = 0;
									ppCurrMouse.y = 0;

									OurShowCursor( FALSE );
									*/
								}
							}

						VC.HeadPosition.SetValues( 0.0f, 0.0f, 0.0f );

						if (ACM_mode)//(camera->SubType & COCKPIT_ACM_MODE)
						{
							if (!InACMMode)
							{
								InACMMode = TRUE;

								PreACMHeading = camera->TargetHeading;
								PreACMPitch = camera->TargetPitch;
								PreACMRoll = camera->TargetRoll;
							}

							commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

//							float *index = &AcmViewAngles[(camera->SubType & COCKPIT_ACM_INDEX) >> 9];
							float *index = &AcmViewAngles[ACM_mode >> 9];

							camera->TargetHeading = *index++;
							camera->TargetPitch = *index;
							camera->TargetRoll = 0;

						 	resetpadlocktimer = 0;
						}
						else
						{
							if (InACMMode)
							{
								InACMMode = FALSE;

								camera->TargetHeading = PreACMHeading;
								camera->TargetPitch =   PreACMPitch;
								camera->TargetRoll =    PreACMRoll;
							}

//							if (GetVkStatus(vkGlanceFrontACM))
//							{
//								camera->TargetHeading = 0.0f;
//								camera->TargetPitch = 0.0f;
//								camera->TargetRoll = 0.0f;
//								resetpadlocktimer = 0;
//							}
//							else
								if (camera->SubType & COCKPIT_FREE)
								{
									CheckVirtualMouse();

									camera->TargetPitch += VC.PitchOffset;
									camera->TargetHeading += VC.HeadingOffset;
									camera->TargetRoll = 0;

									VC.PitchOffset = 0;
									VC.HeadingOffset = 0;
								}
								else
									if (camera->SubType & COCKPIT_PADLOCK)
									{
										if (camera->TargetObjectType == CO_NADA)
											camera->SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
										else
										{
											FMatrix subtrans(camera->SubjectMatrix);
											subtrans.Transpose();

											vector.MakeVectorToFrom(camera->TargetLocation,camera->SubjectLocation);
											vector *= subtrans;

											camera->TargetHeading = atan2(-vector.X,-vector.Z)*((float)0x8000/3.14159265359f);
											camera->TargetPitch = atan2(vector.Y,sqrt(vector.X*vector.X + vector.Z*vector.Z))*(float)0x8000/3.14159265359f;
											camera->TargetRoll = 0;

											FPointDouble fake;

											if (!LOS(&camera->TargetLocation,&camera->SubjectLocation, &fake, HI_RES_LOS, FALSE))
												resetpadlocktimer = 0;
										}
									}
						}


						camhead = fabs(camera->TargetHeading);

						if ((g_Settings.misc.dwMisc & GP_MISC_VIEW_PADLOCK_FOV_LIMITS) && ((camera->SubType & (COCKPIT_ACM_MODE | COCKPIT_ACM_INDEX)) != (COCKPIT_ACM_MODE | COCKPIT_ACM_BACK | COCKPIT_ACM_UP)))
						{
							if (camhead > (float)0x6D00)
							{
								camhead = (float)0x6D00;
								if (camera->TargetHeading < 0.0f)
									camera->TargetHeading = (float)-0x6D00;

								else
									camera->TargetHeading = (float)0x6D00;
							}
						}
						else
						{
							if (camera->TargetHeading > (float)0x08000)
								camera->TargetHeading = -(float)0x10000 + camera->TargetHeading;
							else
								if (camera->TargetHeading < (float)-0x8000)
									camera->TargetHeading = (float)0x10000 + camera->TargetHeading;
						}

						if(camhead > (float)0x4000)
						{
							if (camhead > (float)0x6800)
								maxpitch = -0x1200;
							else
								maxpitch = (float)-0x600 - (float)0x1200*fabs(sin(camhead * (3.14159265359f/(float)0x8000)));
						}
						else
							maxpitch = (float)-0x1800;

						if( camera->TargetPitch< maxpitch)
						{
							if((maxpitch - camera->TargetPitch) > 0x800)
							{
								resetpadlocktimer = 0;
							}
							camera->TargetPitch = maxpitch;		// max head tile down
						}
						else
							if (camera->TargetPitch > (float)0x3800)
								camera->TargetPitch = (float)0x3800;

						if ((g_Settings.misc.dwMisc & GP_MISC_VIEW_PADLOCK_FOV_LIMITS) && ((camera->SubType & (COCKPIT_ACM_MODE | COCKPIT_ACM_INDEX)) != (COCKPIT_ACM_MODE | COCKPIT_ACM_BACK | COCKPIT_ACM_UP)))
						{
							if (Camera1.TargetObjectType != CO_PLANE)
							{
								resetpadlocktimer = 1;
							}
							else
							{
								if(camhead < (float)0x1000)
								{
									if(camera->TargetPitch < (float)-0xC00)
									{
										resetpadlocktimer = 0;
										camera->TargetPitch = (float)-0xC00;
									}
								}
							}
						}
						else
						{
							resetpadlocktimer = 1;
						}

						if(resetpadlocktimer)
						{
							lPadLockTimer = PAD_LOST_TIME;
						}
						else if(lPadLockTimer < 0)
						{
							pPadLockTarget = NULL;
							fpdPadLockLocation.SetValues(-1.0f, -1.0f, -1.0f);
							iPadLockType = 0;
							AssignCameraTarget(NULL,CO_NADA);
							Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
							camera->TargetPitch = 0;
							camera->TargetHeading = 0;
						}

						// now for the slewing

						commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

						acm_speed_mult = camera->RELATIVE_T + camera->RELATIVE_T*((float)g_Settings.misc.byACMPanSpeed - 128.0f)*1.0f/256.0f;

						if (!(g_Settings.misc.dwMisc & GP_MISC_VIEW_PADLOCK_FOV_LIMITS))
						{
							float offset;

							offset = (camera->TargetHeading - camera->CurrentHeading);
							if (fabs(offset) > (float)0x8000)
							{
								if (offset < 0.0f)
									offset = (float)0x10000+offset;
								else
									offset = (float)-0x10000+offset;
							}
							camera->CurrentHeading += offset * __min(acm_speed_mult * RTSqrtDeltaTicks * 0.20f,1.0f);

							if (camera->CurrentHeading > (float)0x08000)
								camera->CurrentHeading = (float)-0x10000 + camera->CurrentHeading;
							else
								if (camera->CurrentHeading < (float)-0x08000)
									camera->CurrentHeading = (float)0x10000 + camera->CurrentHeading;

						}
						else
							camera->CurrentHeading += (camera->TargetHeading - camera->CurrentHeading) * __min(acm_speed_mult * RTSqrtDeltaTicks * 0.20f,1.0f);

						camera->CurrentPitch += (camera->TargetPitch - camera->CurrentPitch) * __min(acm_speed_mult * RTSqrtDeltaTicks * 0.20f,1.0f);

						camera->Heading = (ANGLE)(((int)camera->CurrentHeading) & 0xFFFF);
						camera->Pitch = (ANGLE)(((int)camera->CurrentPitch) & 0xFFFF);
						camera->Roll = 0;

						if (camhead > 0x2000)
						{
							if (camhead < 0x4000)
							{
								float nang = ANGLE_TO_RADIANS(camera->Heading);
								if (camera->Heading & 0x8000)
								{
									nang -= PI*2.0f;
									nang+=ANGLE_TO_RADIANS(0x2000);
								}
								else
									nang-=ANGLE_TO_RADIANS(0x2000);

								nang *= 2.0f;

								VC.xdelta = sin( nang );
							}
							else
							{
								if (g_Settings.misc.dwMisc & GP_MISC_VIEW_PADLOCK_FOV_LIMITS)
								{
									if (camera->Heading & 0x8000)
										VC.xdelta =  -1.0f;
									else
										VC.xdelta = 1.0f;
								}
								else
									VC.xdelta = sin( ANGLE_TO_RADIANS( camera->Heading) );
							}
							VC.xdelta *= 4 INCHES;
						}
						else
							VC.xdelta = 0;

						if (camhead > 0x6800)
							VC.ydelta = -cos(ANGLE_TO_RADIANS(((float)0x08000 - camhead)*((float)0x4000/(float)0x1800)))*(float)(0.50 FEET);
						else
							VC.ydelta = 0.0f;

						/* -----------------8/11/99 10:40AM--------------------------------------------------------------------
						/* this brings the UFC into sharpest focus
						/* ----------------------------------------------------------------------------------------------------*/
						VC.zdelta -= 0.000001;

						VC.HeadPosition.SetValues( VC.xdelta, VC.ydelta, VC.zdelta );

						VC.HeadPosition *= camera->SubjectMatrix;

						// This may have to go
						camera->CameraLocation -= VC.HeadPosition;

						if((iReturnACMView) && (!InACMMode))
						{
							if(fabs(AIConvertAngleTo180Degree(camera->Heading - camera->TargetHeading)) < 2.0f)
							{
								if(fabs(AIConvertAngleTo180Degree(camera->Pitch - camera->TargetPitch)) < 2.0f)
								{
									if(fabs(AIConvertAngleTo180Degree(camera->Roll - camera->TargetRoll)) < 2.0f)
									{
										if (!CheckForEjected())
										{
											if (!(PlayerPlane->FlightStatus & PL_STATUS_CRASHED))
											{
												AssignCameraSubject((void *)PlayerPlane,CO_PLANE);

												Camera1.CameraMode = CAMERA_COCKPIT;
												Camera1.SubType = COCKPIT_FRONT | COCKPIT_NOART;
												LoadCockpit(FRONT_NO_COCKPIT);
												CockpitSeat = FRONT_SEAT;
												SetupView( Camera1.SubType );
												OurShowCursor( FALSE );

												if((iReturnACMView & 2) && (!DrawPopMpds))
												{
													DrawPopMpds = !DrawPopMpds;
												}
											}
											else
											{
												AssignCameraSubject((void *)PlayerPlane,CO_PLANE);
												setup_no_cockpit_art();
												ChangeViewModeOrSubject(CAMERA_FIXED_CHASE);
											}
										}
										iReturnACMView = 0;
									}
								}
							}
						}
					}
					else
					{
						ReadMouse();

						if( camera->SubType & COCKPIT_45UP)
							Pitch = 0x2000;
						else if( camera->SubType & COCKPIT_45DOWN)
							Pitch = (ANGLE)-0x1000;

					//VC.HeadPosition.SetValues( 0.0f, -(float)(12.0f INCHES), 0.0f );

						switch (camera->SubType & ~(COCKPIT_VIEW_PITCH | COCKPIT_ACM_MODE))
						{
							case COCKPIT_LEFT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0x4000;

								break;

							case COCKPIT_FRONT_LEFT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0x2000;
								break;

							case COCKPIT_RIGHT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0xC000;
								break;


							case COCKPIT_FRONT_RIGHT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0xE000;
								break;


							case COCKPIT_BACK_LEFT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0x6000;
								break;

							case COCKPIT_BACK_RIGHT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = (ANGLE)-0x6000;
								break;

							case COCKPIT_BACK://FRONT_REAR_COCKPIT:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0x8000;
								break;


							case COCKPIT_FRONT://FRONT_FORWARD_COCKPIT:
							default:
								commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;

								if ( Pitch == (ANGLE)-0x1000)
									Pitch = 0x0;;

								camera->Roll = 0;
								camera->Pitch = Pitch;
								camera->Heading = 0;
								break;

						}
					}

				if (commands & CC_MAKE_OFFSET_MAT)
				{
					commands |= CC_USE_OFFSET_MAT;
					camera->CameraRelativeMatrix.SetRPH(camera->Roll,camera->Pitch,camera->Heading);
				}

				if (commands & CC_USE_OFFSET_MAT)
					camera->CameraMatrix = camera->CameraRelativeMatrix;

				if (commands & CC_CONCAT_SUBJECT_MAT)
					camera->CameraMatrix *= camera->SubjectMatrix;
				else
					if (commands & CC_USE_SUBJECT_MAT)
						camera->CameraMatrix = camera->SubjectMatrix;

				camera->CameraMatrix.SetViewMatrix();
				break;

			case CO_WEAPON:
				if(weap->Kind == EJECTION_SEAT)
				{
					FPointDouble headoffset;

					commands = CC_MAKE_OFFSET_MAT | CC_CONCAT_SUBJECT_MAT;
					headoffset.SetValues(0, 2.5f * FTTOWU, -1.0f * FTTOWU);
					headoffset *= weap->Attitude;
					camera->CameraLocation += headoffset;

//					weap->Attitude.GetRPH(&camera->Roll,&camera->Pitch,&camera->Heading);
					camera->Heading = camera->Pitch = camera->Roll = 0;

					if (commands & CC_MAKE_OFFSET_MAT)
					{
						commands |= CC_USE_OFFSET_MAT;
						camera->CameraRelativeMatrix.SetRPH(camera->Roll,camera->Pitch,camera->Heading);
					}

					if (commands & CC_USE_OFFSET_MAT)
						camera->CameraMatrix = camera->CameraRelativeMatrix;

					if (commands & CC_CONCAT_SUBJECT_MAT)
						camera->CameraMatrix *= camera->SubjectMatrix;
					else
						if (commands & CC_USE_SUBJECT_MAT)
							camera->CameraMatrix = camera->SubjectMatrix;

					camera->CameraMatrix.SetViewMatrix();
				}
				break;

			case CO_NADA:
			default:
				break;

		}
	}
	else
	{
		InACMMode = FALSE;

		PreACMHeading = 0.0f;
		PreACMPitch = 0.0f;
		PreACMRoll = 0.0f;

		camera->SubType = 0;

 		if (DrawPopMpds)
		{
			ToggleVirtualMpds();
			/*
			LONG x = GetSystemMetrics(SM_CXSCREEN)>>1;
			LONG y = GetSystemMetrics(SM_CYSCREEN)>>1;

			SetCursorPos(x, y);
			ppLastMouse.x = x;
			ppLastMouse.y = y;
			ppCurrMouse.x = 0;
			ppCurrMouse.y = 0;
			*/
		}

		//OurShowCursor( FALSE );
		CheckVirtualMouse();

		if (Camera1.CameraMode != CAMERA_FREE)
		{
			camera->Pitch -= VC.PitchOffset;
			camera->Heading -= VC.HeadingOffset;
		}
		else
		{
			camera->Pitch += VC.PitchOffset;
			camera->Heading += VC.HeadingOffset;
		}

		if (VC.PitchOffset || VC.HeadingOffset)
			camera_setup_relative_quats();

		VC.PitchOffset = 0;
		VC.HeadingOffset = 0;

		switch (camera->CameraMode)
		{
//#define CAMERA_FLYBY			5
//#define CAMERA_CINEMATIC		6

			case CAMERA_FIXED_CHASE:
				process_camera_relative_quat(camera);

				camera->CameraMatrix = camera->CameraRelativeMatrix;

			fix_matrix:
				process_camera_relative_distance(camera);

				camera->CameraMatrix.SetViewMatrix();

	  			camera->CameraLocation.SetValues(0.0f,0.0f,camera->DistanceFromObject);

				camera->CameraLocation *= camera->CameraMatrix;
				camera->CameraLocation += camera->SubjectLocation;
				camera->CameraTargetLocation = camera->CameraLocation;
				break;


			case CAMERA_CHASE :
				process_camera_relative_quat(camera);

				from_quat = camera->CameraCurrentQuat;
				to_quat = camera->SubjectMatrix;

				camera->CameraCurrentQuat.SLERP(from_quat,to_quat,Camera1.CHASE_T);
				camera->CameraQuatMatrix = camera->CameraCurrentQuat;
				camera->CameraMatrix = camera->CameraRelativeMatrix;
				camera->CameraMatrix *= camera->CameraQuatMatrix;

				goto fix_matrix;

			case CAMERA_FLYBY:
				process_flyby_views(camera);

				camera->CameraMatrix = camera->CameraRelativeMatrix;
				camera->CameraMatrix.SetViewMatrix();
				break;


			case CAMERA_RC_VIEW:
				process_camera_location(camera);

				vector.MakeVectorToFrom(camera->SubjectLocation,camera->CameraLocation);

				camera->Roll = 0;
				camera->Pitch = (ANGLE)(atan2(vector.Y,QuickDistance(vector.X,vector.Z))*(32768.0f/PI));
				camera->Heading = (ANGLE)(atan2(-vector.X,-vector.Z)*(32768.0f/PI));

				camera_setup_relative_quats();
				process_camera_relative_quat(camera);

				camera->CameraMatrix = camera->CameraRelativeMatrix;

				camera->CameraMatrix.SetViewMatrix();
				break;

			case CAMERA_FREE:
				process_camera_relative_quat(camera);
				process_camera_location(camera);
				camera->CameraMatrix = camera->CameraRelativeMatrix;
				camera->CameraMatrix.SetViewMatrix();
				break;

			case CAMERA_TACTICAL :
				process_camera_relative_distance(camera);
				vector2 = camera->SubjectLocation;
				vector2.Y += camera->DistanceFromObject * 0.20f;

				vector.MakeVectorToFrom(camera->TargetLocation,vector2);

				camera->Roll = 0;
				camera->Pitch = (ANGLE)(atan2(vector.Y,QuickDistance(vector.X,vector.Z))*(32768.0f/PI));
				camera->Heading = (ANGLE)(atan2(-vector.X,-vector.Z)*(32768.0f/PI));

			fix_matrix_for_tactical:
				camera_setup_relative_quats();
				process_camera_relative_quat(camera);

				camera->CameraMatrix = camera->CameraRelativeMatrix;

				camera->CameraMatrix.SetViewMatrix();

	  			camera->CameraLocation.SetValues(0.0f,0.0f,camera->DistanceFromObject);

				camera->CameraLocation *= camera->CameraMatrix;
				camera->CameraLocation += vector2;
				camera->CameraTargetLocation = camera->CameraLocation;
				break;

			case CAMERA_REVERSE_TACTICAL :
				process_camera_relative_distance(camera);
				vector2 = camera->TargetLocation;
				vector2.Y += camera->DistanceFromObject * 0.20f;

				vector.MakeVectorToFrom(camera->SubjectLocation,vector2);

				camera->Roll = 0;
				camera->Pitch = (ANGLE)(atan2(vector.Y,QuickDistance(vector.X,vector.Z))*(32768.0f/PI));
				camera->Heading = (ANGLE)(atan2(-vector.X,-vector.Z)*(32768.0f/PI));

				goto fix_matrix_for_tactical;

		}

		if (camera->CameraMode != CAMERA_FLYBY)
		{
			vector.Y = LandHeight(camera->CameraLocation.X,camera->CameraLocation.Z)+CAMERA_MIN_HEIGHT;

			if (camera->CameraLocation.Y < vector.Y)
				camera->CameraLocation.Y = vector.Y;

			if (camera->CameraLocation.Y > MAX_CAM_ALT)
				camera->CameraLocation.Y = MAX_CAM_ALT;
		}


	}
}

//--------- DEBUG STUFF -------------
void LockBack(void)
{

	// this is mach 64 test
	VisibleSurface->Buffers = (unsigned char *)ddLock(lpDDSBack );
	ddUnlock (lpDDSBack, VisibleSurface->Buffers );
}

void LockDisplay(void)
{

	// this is mach 64 test
	VisibleSurface->Buffers = (unsigned char *)ddLock(lpDDSPrimary);
	ddUnlock (lpDDSPrimary, VisibleSurface->Buffers );
}


extern 	int DB_LocksPerFrame;
extern int	DB_UnLocksPerFrame;


void DisplayDebugText( void )
{


	// DEBUG CODE
	//debugfont = GrLoadFont(RegPath("Cockpits","HUDsm.fnt"));
	sprintf(TmpStr,"xoffset: 0x%4x  yoffset: 0x%4X", VC.HeadingOffset, VC.PitchOffset );
	GrDrawString(GrBuffFor3D, LgHUDFont, 100, 50, TmpStr,  3);
	sprintf(TmpStr,"VC.Headmovement = %d", VC.HeadMovement);
	GrDrawString(GrBuffFor3D, LgHUDFont, 100, 70, TmpStr,  3);

	/*
	sprintf(TmpStr,"Locks/frame = %d", DB_LocksPerFrame );
	GrDrawString(GrBuffFor3D, LgHUDFont, 100, 90, TmpStr,  3);
	DB_LocksPerFrame = 0;

	sprintf(TmpStr,"UnLocks/frame = %d", DB_UnLocksPerFrame );
	GrDrawString(GrBuffFor3D, LgHUDFont, 100, 100, TmpStr,  3);
	DB_UnLocksPerFrame = 0;



	int i, start, end, ms;
	start= GetTickCount();
	for( i= 0; i<100; i++)
		LockBack();
	end = GetTickCount();
	ms = (end - start); /// 1000;
	//ms /= 100;
	sprintf(TmpStr,"Back ms/Locks = %d", ms );
	GrDrawString(GrBuffFor3D, LgHUDFont, 100, 110, TmpStr,  3);


	start= GetTickCount();
	for( i= 0; i<100; i++)
		LockDisplay();
	end = GetTickCount();

	ms = (end - start); /// 1000;
	//ms /= 100;
	sprintf(TmpStr,"Disp ms/Locks = %d", ms );
	GrDrawString(GrBuffFor3D, LgHUDFont, 100, 120, TmpStr,  3);
*/

}