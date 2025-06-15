//*****************************************************************************************
// OldMiss.h

// Version 1.0

struct stCAPV00
{
	int iHeading;
	long lRange;
	long lTimer;
};

typedef struct stCAPV00 CAPActionTypeV00;

struct stCASV00
{
	long lTimer;
	long lRange;
};

typedef struct stCASV00 CASActionTypeV00;

struct stFighterSweepV00
{
	long lTimer;
	long lRange;
};

typedef struct stFighterSweepV00 FighterSweepActionTypeV00;

struct stOrbitV00
{
	long lTimer;
	int iUntil;
};

typedef struct stOrbitV00 OrbitActionTypeV00;

struct stSEADV00
{
	long lRange;
	long lTimer;
};

typedef struct stSEADV00 SEADActionTypeV00;

struct stAWACSPatternV00
{
	long lDuration;
};

typedef struct stAWACSPatternV00 AWACSPatternActionTypeV00;

struct stHoverV00
{
	long lTimer;
};

typedef struct stHoverV00 HoverActionTypeV00;

struct stRefuelPatternV00
{
	long lDuration;
};

typedef struct stRefuelPatternV00 RefaulPatternActionTypeV00;

struct stRunwayV00
{
	long iGroundObjectId;
	long lRunwayStartX;
	long lRunwayStartY;
	long lRunwayStartZ;
	long lRunwayEndX;
	long lRunwayEndY;
	long lRunwayEndZ;
	long lTimer;
	long lRange;
};

typedef struct stRunwayV00 TakeOffActionTypeV00;


struct stGroundObjectV00
{			   
	long X;							//  Holds the starting X position of the object, in feet, from the upper left corner of the map.
	long Y;							//  Holds the starting Y position of the object in feet.
	long Z;							//  Holds the starting X position of the object, in feet, from the upper left corner of the map.
	long luniqueID;					//  Holds a unique ID for each object.  John may want to switch this to 64 bits to have enough space to incorporate information about ground vehicles.
	int iSide;						//  Holds the value for which side this object is on (could be switched to country)
	int iType;						//  Vehicle Type
	int iState;
	int iRandomChance;				//  Tells what the change is this object is going to appear.
	int iRandomGroup;				//  Tells which random group this object belongs.  Random groups can be used to set up such things as a random CAP, where each object of this group number is flying a CAP mission but in a different area.  Only one element/object of a group is picked for a mission.
	int iRandomElement;				//  Tells which random element this object belongs.  Random elements are used to "attach" objects together within a random group so that if one appears they all appear.  For example, if a attack group is picked, you would want to have its cover appear as well.
	char sBriefingLabel[NAMESIZE];	//  Holds a text string that can be used to identify an object with in the mission builder.  This is just in the builder and has no effect "in game".
	char bBriefingMap;
	char bBriefingTheat;
	int iCampaignGroundObject;
};

typedef struct stGroundObjectV00 GroundObjectV00;


