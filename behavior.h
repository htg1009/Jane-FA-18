// Behavior.h

class PlaneParams;  

typedef struct _Behavior	//  This holds the structure the AI planes use "in game"
{
	void		(*Behaviorfunc)(PlaneParams *planepnt);		//  This is a pointer to the AI function the plane is currently using.
	void		(*OrgBehave)(PlaneParams *planepnt);		//  This holds the pointer to the AI function the plane was using before getting attacked, or something else which caused him to deviate from his last behavior.
	void		(*CombatBehavior)(PlaneParams *planepnt, PlaneParams *target, double offangle, double toffpitch, double tdist, double toffangle);	//  This holds the pointer to the AI function the plane wants to used to enhance his combat performance.
	MBWayPoints	*CurrWay;						//  This is a pointer to the current waypoint that the obj is flying towards.
	FPoint		WayPosition;					//  This holds the World Position that the AI is flying towards.
	int			numwaypts;						//  This holds the number of waypoints left to fly.
	int			startwpts;						//  This holds the number of the first waypoint this plane was using when he started his mission.
	int			winglead;						//  This tells what object is the wingleader for this object (-1 if this plane is the wingleader).
	int			wingman;						//  This tells what object is the wingman for this object (-1 if he is the wingman).
	int			prevpair;						//  This tells what object is the wingleader for the previous wing pair (-1 represents this is the lead wingpair).
	int			nextpair;						//  This tells what object is the wingleader for the next wing pair (-1 if none).
	int			iSide;							//  This identifies what side (or country) this object belongs.
	int			iSkill;							//  This holds the skill level of the AI.  Might be broken into catagories (shifting bits)
	int			iFuelLoad;						//  This identifies how much fuel this object is carrying (may not be needed).
	int			cnt1;							//  This is used as a generic count variable when an AI needs to keep track of something.
	ANGLE		DesiredRoll;					//  This holds the value of the roll the object is trying to obtain.
	ANGLE		DesiredPitch;					//  This holds the value of the pitch the object is trying to obtain.
	ANGLE		DesiredHeading;					//  This holds the value of the heading the object is trying to obtain.
	ANGLE		LastRoll;						//  This tells what was the last roll the object had (for various delta calculations).
	ANGLE		LastPitch;						//  This tells what was the last pitch the object had (for various delta calculations).
	ANGLE		LastHeading;					//  This tells what was the last heading the object had (for various delta calculations).
	float		LastSymetricalElevator;	//  When gone from flight.cpp we can remove
	long		lDesiredSpeed;					//  This identifies what speed the plane is trying to reach.
	PlaneParams *AirTarget;						//  Is a pointer to a plane we are attacking.
	PlaneParams *AirThreat;						//  Is a pointer to a plane we are being attacked by.
	PlaneParams *LastAirThreat;					//  Is a pointer to a plane we are being attacked by last frame, used for Network code.
	PlaneParams *LinkedPlane;					//  Is a pointer to a plane we are working with (tanking, escorting, etc;
	void		*pGroundTarget;					//  Pointer to the Ground Target.
	long		lGroundTargetFlag;				//  Might be needed to tell is something is a vehicle or building or ....
	FPoint		FormationPosition;				//  This holds the offset off the lead plane which is where this plane wants to be when flying formation.  If plane is lead, then it holds Sin and Cos info which will be used by other planes during formation calculation.
	FPoint		WorldFormationPos;				//  This holds the position in the world where a plane should be if he is in formation.
	FPoint		TargetPos;						//  This holds the position in the world where a target last was.
	FPoint		CombatPos;						//  This holds data to be used by A2A combat functions.
	long		lTimer1;						//  This is a generic timer used when the AI needs to keep track of elapsed time.  Counts down to 0 using DeltaTicks.
	long		lTimer2;						//  This is a generic timer used when the AI needs to keep track of elapsed time.  Counts down to 0 using DeltaTicks.
	long		lTimer3;						//  This is a generic timer used when the AI needs to keep track of elapsed time.  Counts down to 0 using DeltaTicks.
	long		lTimerAWACS;					//  This is a generic timer used when the AI needs to keep track of elapsed time.  Counts down to 0 using DeltaTicks.
	int			iVar1;							//  This is a generic variable that can be used by various behaviors.
	long		lVar2;							//  This is a generic variable that can be used by various behaviors.
	long		lVar3;							//  This is a generic variable that can be used by various behaviors.
	float		fVarA[10];						//  This is an array of generic float variables that can be used by various behaviors.
	int			iAIFlags1;						//  Holder for bit flags
	int			iAIFlags2;						//  Holder for bit flags
	int			iAICombatFlags1;				//  Holder for bit flags
	int			iAICombatFlags2;				//  Holder for bit flags
	long		lAltCheckDelay;					//  Time until height is checked next
	long		lMinAltFt[4];					//  Holds the maximum value for the minimum altitude a plane can fly for the next 20 seconds (one variable for every 5 seconds).
	float		fDistToHeight[4];				//  Holds the distance needed to travel to reach this height
	long		lRadarDelay;					//  Holds the delay until the next radar check.
	float		fRadarAngleYaw;					//  Holds the angle that the radar is yawed off center;
	float		fRadarAnglePitch;				//  Holds the angle that the radar is pitched off center;
	long		lCallSign;						// 	Index into callsign list for this type of object
	BYTE		cCallSignNum;					//  Flight number NOT number in flight Cobra 3 1 this would be the 3.		
	double		dRollAccumulator;				//  Accumulator to smooth out desired roll.
	double		dPitchAccumulator;				//  Accumulator to smooth out desired roll.
	long		lAreaEventFlags;				//  1 means in area 0 out of area.
	long		l2AreaEventFlags;				//  1 means in area 0 out of area.
	BYTE		cFXarray[4];					//  holds the value from 0-0xFF used for rotations.
	ANGLE		AOADegPitch;					//  holds the degree of difference (in ANGLE) of pitch caused by the AOA of aircraft.  Can't use pure AOA since if plane is rotated this value is not correct.
	char		cActiveWeaponStation;			//  Tells which Weapon the AI is thinking of using.
	char		cNumWeaponRelease;				//  This tells how many weapons to release (used for ripple releases.
	int			iMissileThreat;				    //  This is an index into the weapons array for the weapon that is the biggest threat.
	int			lLockTimer;						//  This is a timer so that planes have to wait a bit before they can fire a missile.
	long		lCMTimer;						//  This is a timer for when to drop the next set of counter measures.
	long		lHumanTimer;					//  This is a timer to use as a delay when checking for things regarding the Player (such him meeting to escort us).
	long		lSpeechOverloadTimer;			//  This is a timer so that highly used speech doesn't get overused.
	long		lInterceptTimer;				//  This is a timer to say that planes have been vectored to intercept this plane.
	long		lCombatTimer;					//  This is used to time various things during A2A combat.
	long		lEffectTimer;					//  This is used to keep speedbrakes and afterburners from flicking on and off.
	long		lGunTimer;						//  This is a timer to keep track of how long the plane has fired its gun/let it rest.
	long		lAIVoice;						//  This is a value representing which speech a plane is to use.
	void		*pPaintedBy;					//  This will hold a pointer to the Instance of the Ground object who is targeting this plane.
	int			iPaintedByType;					//  This will identify if pPaintedBy is a vehicle or a ground object.
	int			iPaintDist;						//  This holds the range from the Painting ground object to the plane in nms.
	char		cArmorDamage[10];				//  This holds how much repairable damage has been done.
	char		cInteriorDamage[10];			//  This holds how much interior damage has been done.
	float		fStoresWeight;					//  Total weight of all the stores.
	int			iHomeBaseId;					//  Depending on bits in CombatFlags this will either hold the index into moving vehicles or Serial Number of an airbase.
	int			iMultiPlayerDist[8];			//  Distance in NM to Player # in multiplayer games
	int			iMultiPlayerAIController;		//  Identifies which Player is the closest for multiplayer games.
	BYTE		cMultiCheckFlags;				//  Used to check last frame states as such things a SUPPORT, DEFENSIVE, MISSILEAVOID.
	BYTE		cMultiLastCombatBehaveID;		//  Holds last behave id sent across the net.
	BYTE		cUseWeapon;						//  Identifies which weapon type/amount the Player wants the AI to use.
//	long		lDamageFlags;					//  This tells what systems are out.

	// DataBase - This should be moved to PlaneParams at some time
	long		lPlaneID;
	int			iPlaneIndex;
	long		lGateTimer;						//  Set this timer when you've locked up someone with a gatestealer jammer on.  When timer < 0 then check for STT lock.

} Behavior;


