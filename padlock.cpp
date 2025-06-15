//*******************************************************************************
//*  Padlock.cpp
//*
//*  This file contains functions dealing with the tracking padlocks.
//*******************************************************************************
#define SREPADLOCKSTUFF
#include "F18.h"
#include "resources.h"

//**************************************************************************************
void PLGetTrackingVC()
{
	FPointDouble worldposition;
	PlaneParams *planepnt;
	WeaponParams *weaponpnt;
	BasicInstance *walker;
	float heading, pitch;
	int clipped;
	int authentic = 0;
	int groundtarget = 0;

	if(lPadLockFlags & PAD_GLANCE)
	{
		lPadLockFlags &= ~(PAD_GLANCE);
		VC.HeadingOffset = 0;
		VC.PitchOffset = 0;
		return;
	}

	switch(iPadLockType)
	{
		case TARGET_PLANE:
			if(pPadLockTarget == NULL)
			{
				VC.HeadingOffset = 0;
				VC.PitchOffset = 0;
				return;
			}
			planepnt = (PlaneParams *)pPadLockTarget;
			worldposition = planepnt->WorldPosition;
			break;
		case TARGET_GROUND:
			if(pPadLockTarget == NULL)
			{
				VC.HeadingOffset = 0;
				VC.PitchOffset = 0;
				return;
			}
			walker = (BasicInstance *)pPadLockTarget;
			worldposition = walker->Position;
			break;
		case TARGET_WEAPON:
			if(pPadLockTarget == NULL)
			{
				VC.HeadingOffset = 0;
				VC.PitchOffset = 0;
				return;
			}
			weaponpnt = (WeaponParams *)pPadLockTarget;
			worldposition = weaponpnt->Pos;
			break;
		case TARGET_LOCATION:
			if(fpdPadLockLocation.X == -1.0f)
			{
				VC.HeadingOffset = 0;
				VC.PitchOffset = 0;
				return;
			}
			worldposition = fpdPadLockLocation;
			groundtarget = 1;
			break;
		default:
			VC.HeadingOffset = 0;
			VC.PitchOffset = 0;
			Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
			return;
			break;
	}

	PLGetRelativeHP(PlayerPlane, worldposition, &heading, &pitch);
	clipped = PLClipViewHP(&heading, &pitch, authentic);
	if(!(clipped) || groundtarget)
	{
		lPadLockTimer = PAD_LOST_TIME;
	}
	else if(lPadLockTimer < 0)
	{
		pPadLockTarget = NULL;
		fpdPadLockLocation.SetValues(-1.0f, -1.0f, -1.0f);
		iPadLockType = 0;
		VC.HeadingOffset = 0;
		VC.PitchOffset = 0;
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_FREE;
		return;
	}

	if((!(clipped && authentic)) || (groundtarget))
	{
		VC.HeadingOffset = AIConvert180DegreeToAngle(heading);
		VC.PitchOffset = AIConvert180DegreeToAngle(pitch);
	}
}

//**************************************************************************************
void PLGetRelativeHP(PlaneParams *planepnt, FPointDouble worldposition, float *heading, float *pitch)
{
	FPointDouble relative_position;
	FMatrix de_rotate;

	relative_position.MakeVectorToFrom(worldposition,planepnt->WorldPosition);

	de_rotate = planepnt->Attitude;
	de_rotate.Transpose();

	relative_position *= de_rotate;
	relative_position.Normalize();

	*heading = atan2(-relative_position.X,-relative_position.Z) * 57.2958;
	*pitch = asin(relative_position.Y) * 57.2958;
}

//**************************************************************************************
void PLGetRelativeHPR(PlaneParams *planepnt, FPointDouble worldposition, float *heading, float *pitch, float *roll)
{
	FPointDouble relative_position;
	FMatrix de_rotate;

	relative_position.MakeVectorToFrom(worldposition,planepnt->WorldPosition);

	de_rotate = planepnt->Attitude;
	de_rotate.Transpose();

	relative_position *= de_rotate;
	relative_position.Normalize();

	if(heading)
	{
		*heading = atan2(-relative_position.X,-relative_position.Z) * 57.2958;
	}

	if(pitch)
	{
		*pitch = asin(relative_position.Y) * 57.2958;
	}

	if(roll)
	{
		*roll = atan2(-relative_position.X,relative_position.Y) * 57.2958;
	}
}

//**************************************************************************************
int PLClipViewHP(float *heading, float *pitch, int clipdown)
{
	ANGLE headingoffset, pitchoffset;
	ANGLE  maxpitch;
	int retval = 0;

	headingoffset = AIConvert180DegreeToAngle(*heading);
	pitchoffset = AIConvert180DegreeToAngle(*pitch);

	if( headingoffset > 0x8000)			//180DEG
	{
		if( headingoffset< 0x9200)
		{
			headingoffset = 0x9200;
			retval = 1;
		}
	}
	else
	{
		if( headingoffset > 0x6800)
		{
			headingoffset = 0x6800;		// max head tile up
			retval = 1;
		}
	}

	maxpitch = (ANGLE)0xe000 + (ANGLE)((float) 0x1400 * cos( ANGLE_TO_RADIANS( headingoffset) ));
	maxpitch += 0x2000;
	if( headingoffset > 0xc000)
		maxpitch = (ANGLE)0xe800 + (ANGLE)((float) 0x0c00 * -sin( ANGLE_TO_RADIANS( headingoffset) ));
	else
		if( headingoffset < 0x4000)
			maxpitch = (ANGLE)0xe800 + (ANGLE)((float) 0x0c00 * sin( ANGLE_TO_RADIANS( headingoffset) ));
		else
			maxpitch = 0xf400;

	if( pitchoffset > 0x8000)
	{
		if( pitchoffset< maxpitch )
		{
			pitchoffset = maxpitch;		// max head tile down
			retval = 1;
		}
	}
	else
	{
		if( pitchoffset > 0x3800)
		{
			pitchoffset = 0x3800;		// max head tilt up
			retval = 1;
		}
	}

	*heading = AIConvertAngleTo180Degree(headingoffset);
	*pitch = AIConvertAngleTo180Degree(pitchoffset);

	return(retval);
}

//**************************************************************************************
void PLWatchWSOWarn()
{
	PlaneParams *checkplane;
	WeaponParams *checkweapon;

	if(Camera1.SubType != (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK))
	{
		Camera1.CameraMode = CAMERA_COCKPIT;
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		LoadCockpit( VIRTUAL_MODEL );
		SetupView( Camera1.SubType );
		CockpitSeat = FRONT_SEAT;
		OurShowCursor( FALSE );
	}

	iPadLockType = iLastWSOWarnType;
	pPadLockTarget = pLastWSOWarnObj;
	lPadLockTimer = PAD_LOST_TIME;

	if(pPadLockTarget)
	{
		if(iPadLockType == TARGET_WEAPON)
		{
			checkweapon = (WeaponParams *)pPadLockTarget;
			checkweapon->Flags |= WEP_PADLOCKED;
		}
		else if(iPadLockType == TARGET_PLANE)
		{
			checkplane = (PlaneParams *)pPadLockTarget;
			checkplane->AI.iAIFlags2 |= AI_PADLOCKED;
		}
	}
}

//**************************************************************************************
void PLGetNextClosestFriendly()
{
	PlaneParams *checkplane = &Planes[0];
	float foundrange = -1;
	float dx, dz, tdist;
	float visualrange;
	int done = 0;
	int checkflag = 1;

	if(Camera1.SubType != (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK))
	{
		Camera1.CameraMode = CAMERA_COCKPIT;
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		LoadCockpit( VIRTUAL_MODEL );
		SetupView( Camera1.SubType );
		CockpitSeat = FRONT_SEAT;
		OurShowCursor( FALSE );
	}

	visualrange = (AI_VISUAL_RANGE_NM * NMTOWU);
	foundrange = visualrange;

	iPadLockType = 0;
	pPadLockTarget = NULL;
	fpdPadLockLocation.SetValues(-1.0f, -1.0f, -1.0f);

	while(!done)
	{
		checkplane = &Planes[0];
		while (checkplane <= LastPlane)
		{
			if(!checkflag)
			{
				checkplane->AI.iAIFlags2 &= ~AI_PADLOCKED;
			}
			if((checkplane != PlayerPlane) && (checkplane->Status & PL_ACTIVE) && (!(checkplane->FlightStatus & PL_STATUS_CRASHED)) && (PlayerPlane->AI.iSide == checkplane->AI.iSide) && (!(checkplane->AI.iAIFlags2 & AI_PADLOCKED)))
			{
				dx = checkplane->WorldPosition.X - Camera1.CameraLocation.X;
				dz = checkplane->WorldPosition.Z - Camera1.CameraLocation.Z;
				tdist = QuickDistance(dx, dz);
				if(tdist < foundrange)
				{
					iPadLockType = TARGET_PLANE;
					pPadLockTarget = checkplane;
					done = 1;
					lPadLockTimer = PAD_LOST_TIME;
				}
			}
			checkplane ++;
		}
		if(!done)
		{
			if(checkflag)
			{
				checkflag = 0;
			}
			else
			{
				done = 1;
			}
		}
	}

	if(pPadLockTarget)
	{
		checkplane = (PlaneParams *)pPadLockTarget;
		checkplane->AI.iAIFlags2 |= AI_PADLOCKED;
	}
}

//**************************************************************************************
void PLWatchAGDesignated()
{
	if(Camera1.SubType != (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK))
	{
		Camera1.CameraMode = CAMERA_COCKPIT;
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		LoadCockpit( VIRTUAL_MODEL );
		SetupView( Camera1.SubType );
		CockpitSeat = FRONT_SEAT;
		OurShowCursor( FALSE );
	}

	if(UFC.MasterMode == AG_MODE)
	{
		if(fpdPadLockLocation.X != -1.0f)
		{
			fpdPadLockLocation = PlayerPlane->AGDesignate;
			iPadLockType = TARGET_LOCATION;
		}
		else
		{
			pPadLockTarget = PlayerPlane->AADesignate;
			iPadLockType = TARGET_PLANE;
		}
		lPadLockTimer = PAD_LOST_TIME;
	}
	else
	{
		if(PlayerPlane->AADesignate)
		{
			pPadLockTarget = PlayerPlane->AADesignate;
			iPadLockType = TARGET_PLANE;
		}
		else
		{
			fpdPadLockLocation = PlayerPlane->AGDesignate;
			iPadLockType = TARGET_LOCATION;
		}
		lPadLockTimer = PAD_LOST_TIME;
	}
}

//**************************************************************************************
void PLGetNextObjectToCenter()
{
	PlaneParams *checkplane;
	WeaponParams *checkweapon;
	int done = 0;
	int checkflag = 1;
	float largest = 0.87f;  //  about 30 degrees
	float current;
	float dx, dz;
	float visualrange;
	FPointDouble vector_to_obj;

	if(Camera1.SubType != (COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK))
	{
		Camera1.CameraMode = CAMERA_COCKPIT;
		Camera1.SubType = COCKPIT_VIRTUAL_SEAT | COCKPIT_PADLOCK;
		LoadCockpit( VIRTUAL_MODEL );
		SetupView( Camera1.SubType );
		CockpitSeat = FRONT_SEAT;
		OurShowCursor( FALSE );
	}

	visualrange = (AI_VISUAL_RANGE_NM * NMTOWU);

	iPadLockType = 0;
	pPadLockTarget = NULL;
	fpdPadLockLocation.SetValues(-1.0f, -1.0f, -1.0f);

	while(!done)
	{
		checkplane = &Planes[0];
		while(checkplane <= LastPlane)
		{
			if(!checkflag)
			{
				checkplane->AI.iAIFlags2 &= ~AI_PADLOCKED;
			}
			if((!(checkplane->AI.iAIFlags2 & AI_PADLOCKED)) && (checkplane != PlayerPlane))
			{
				dx = checkplane->WorldPosition.X - Camera1.CameraLocation.X;
				dz = checkplane->WorldPosition.Z - Camera1.CameraLocation.Z;
				if(visualrange > QuickDistance(dx, dz))
				{
					vector_to_obj.MakeVectorToFrom(checkplane->WorldPosition, Camera1.CameraLocation);
					vector_to_obj.Normalize();
					current = ViewVector * vector_to_obj;
					if(current > largest)
					{
						iPadLockType = TARGET_PLANE;
						pPadLockTarget = checkplane;
						done = 1;
						lPadLockTimer = PAD_LOST_TIME;
					}
				}
			}
			checkplane ++;
		}

		checkweapon = &Weapons[0];
		while(checkweapon <= LastWeapon)
		{
			if(!checkflag)
			{
				checkweapon->Flags &= ~WEP_PADLOCKED;
			}
			if((!(checkweapon->Flags & WEP_PADLOCKED)) && (checkweapon->Flags & WEAPON_INUSE) && (((checkweapon->Kind == BOMB) && (checkweapon->WeaponMove != MoveCounterMeasure)) || (checkweapon->Kind == MISSILE)))
			{
				dx = checkweapon->Pos.X - Camera1.CameraLocation.X;
				dz = checkweapon->Pos.Z - Camera1.CameraLocation.Z;
				if(visualrange > QuickDistance(dx, dz))
				{
					vector_to_obj.MakeVectorToFrom(checkweapon->Pos, Camera1.CameraLocation);
					vector_to_obj.Normalize();
					current = ViewVector * vector_to_obj;
					if(current > largest)
					{
						iPadLockType = TARGET_WEAPON;
						pPadLockTarget = checkweapon;
						done = 1;
						lPadLockTimer = PAD_LOST_TIME;
					}
				}
			}
			checkweapon ++;
		}
		if(!done)
		{
			if(checkflag)
			{
				checkflag = 0;
			}
			else
			{
				done = 1;
			}
		}
	}
	if(pPadLockTarget)
	{
		if(iPadLockType == TARGET_WEAPON)
		{
			checkweapon = (WeaponParams *)pPadLockTarget;
			checkweapon->Flags |= WEP_PADLOCKED;
		}
		else if(iPadLockType == TARGET_PLANE)
		{
			checkplane = (PlaneParams *)pPadLockTarget;
			checkplane->AI.iAIFlags2 |= AI_PADLOCKED;
		}
	}
}