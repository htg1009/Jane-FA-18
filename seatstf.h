//**************************************************************************************************************
//*  seatstf.h
//**************************************************************************************************************

#ifndef __IVIBE_HEADER__
#define __IVIBE_HEADER__

typedef struct stIVIBESeatData {		//  Struture holds the data which the intelliVIBE seat will use
	float			IndicatedAirSpeed;  //  Indicated airspeed in Knots
	float			Knots;				//  True airspeed in Knots
	unsigned short	Roll;				//  Aircraft roll 0-65535
	unsigned short	Pitch;				//  Aircraft pitch 0-65535
	unsigned short	Heading;			//  Aircraft heading 0-65535
	float			Altitude;   		//  Altitude in feet
	float			GForce;				//  g force 1.0 = 1 g
	float			LeftRPM;			//  Left engine RPMs
	float			RightRPM;			//  Right engine RPMs
	float			LeftThrustPercent;  //  0 - 100.0f
	float			RightThrustPercent;	//  0 - 100.0f
	int				SimPause;			//  Is sim paused 0 no, 1 yes
	int				GearState;			//  0 retracted, 1 moving, 2 extended
	int				Counter;			//  lets us know when we are on the next frame.
	int				OnGround;			//  0 in air, 1 on ground, 2 on carrier, 3 catapulting, 4 trapping
	int				Gun;				//  0 not firing, 1 high rate, 2 low rate
	float			Mach;				//  1.0 = speed of sound, based on altitude
	float			VerticalVel;		//  Vertical velocity in ft/sec
	float			SpeedBrake;			//  How much it is extended
	float			Flaps;				//  Flap position
	float			DamageHeading;		//  relative heading to damage 0-360 or +/- 0-180
	float			DamagePitch;		//  relative pitch to damage 0-360 or +/- 0-180
	float			DamageDone;			//  Amount of damage done
	int				DamageType;
	int				WeaponReleaseID;	//  ID of weapon launched
	int				WeaponReleaseSide;	//  0 no side (no release), 1 right side, 2 left side, 3 both
	float			WeaponReleaseWeight; //  Weight of weapons released
	int				ProbeConnection;	//  0 no connection, 1 connection
	int				CrashState;			//  0 not crashed
	long			SystemInactive;		//  Bit flags for systems turned off or damaged
	float			Rudder;
	int				Brakes;
} IVIBESeatData;

extern IVIBESeatData *pSeatData;

#define	IVIBE_EXIT_MESSAGE			0xEEEE
#define IVIBE_HANDLE_MESSAGE		0xEEEF
#define INTELLIVIBE_REG_STRING		"intelliVIBE"	// this string is the registy value that is used to check for ivibe functionality
#define IVIBE_DATAFILE				"IVibe.dat"		// this string is the filename used as the memory mapped file

#endif
