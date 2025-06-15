#ifndef __EVLOG_H

#define __EVLOG_H

//***********************************************************************************************************************************
// EventId Defines

#define EVL_HOUSEKEEPING				0x00000000
#define		EVL_HOUSEKEEPING_LOGSTART		0x00000001
#define		EVL_HOUSEKEEPING_LOGEND			0x00000002
#define		EVL_HOUSEKEEPING_CACHEWRITE		0x00000003
#define		EVL_HOUSEKEEPING_MAXFILESIZE	0x00000004
#define		EVL_HOUSEKEEPING_MISSIONINFO	0x00000005

#define	EVL_OBJECT_POS					0x00000010
#define		EVL_OBJECT_POS_AIRCRAFT			0x00000001
#define		EVL_OBJECT_POS_VEHICLE			0x00000002
#define		EVL_OBJECT_POS_WEAPON			0x00000003
#define		EVL_OBJECT_POS_COUNTER			0x00000004
#define		EVL_OBJECT_POS_PARACHUTE		0x00000005

#define EVL_OBJECT_INSTANTIATE			0x00000020
#define		EVL_OBJECT_INSTANTIATE_BOMB		0x00000001
#define		EVL_OBJECT_INSTANTIATE_MISSILE	0x00000002
#define		EVL_OBJECT_INSTANTIATE_GUN		0x00000003
#define		EVL_OBJECT_INSTANTIATE_COUNTER	0x00000004
#define		EVL_OBJECT_INSTANTIATE_EJECT	0x00000005
#define		EVL_OBJECT_INSTANTIATE_SENTENCE	0x00000006

#define EVL_OBJECT_STATECHANGE			0x00000030
#define		EVL_OBJECT_STATECHANGE_HIT			0x00000001
#define		EVL_OBJECT_STATECHANGE_DAMAGED		0x00000002
#define		EVL_OBJECT_STATECHANGE_DESTROYED	0x00000003
#define		EVL_OBJECT_STATECHANGE_CRASHED		0x00000004

#define EVL_DEV_STUFF					0x00000040
#define		EVL_DEV_STUFF_ERROR					0x00000001
#define		EVL_DEV_STUFF_WARNING	   			0x00000002
#define		EVL_DEV_STUFF_INFO					0x00000003

//***********************************************************************************************************************************

#define EVENT_BUFFER_SIZE		128000

//***********************************************************************************************************************************
struct stEventLogType
{
	int iInitialized;
	int iSuspend;
	int iEventLogFile;
	int iLogLevel;
	long lCurrentEventBufferPos;
	long lTotalSize;
	unsigned char *pEventBuffer;
};
typedef struct stEventLogType EventLogType;

struct stEventHeader
{
	long lEventId;
	long lUserId1;
	long lUserId2;
	unsigned long lTickCount;
};
typedef struct stEventHeader EventHeader;


struct stSentenceInfo
{
	int iAircraftIndex;
	int iGroundVoice;
	int iChannel;
	int iVolume;
	int iNumIds;
	int SoundId[30];
};
typedef struct stSentenceInfo SoundSentenceInfo;


struct stAircraftPosInfo 
{
	long lAircraftType;			// Database ID of Weapon (see F18base)
	long lAircraftIndex;		// Plane Index - Index into the planes list
	double lX;
	double lY;
	double lZ;
	unsigned short Roll;
	unsigned short Pitch;
	unsigned short Heading;
	double Velocity;	// feet per sec
};

typedef struct stAircraftPosInfo AircraftPositionInfo;

struct stWeaponPositionInfo 
{
	long lType;					// Database ID of Weapon (see F18base)
	long lWeaponId;				// Unique weapon identifier (random)
	long lLauncher;				// Launcher Type (AIRCRAFT,MOVINGVEHICLE,SHIP,GROUNDOBJECT)
	long lLauncherId;			// Database ID of Launcher Type
	long lLauncherIndex;		// Index or serial number of launcher
								// If AIRCRAFT then look in planes list - Planes[Index]
								// If SHIP or MOVINGVEHICLE then look in MovingVehicleList[Index]
								// If GROUNDOBJECT, then you got some work to do - it's the serial number
								// of the object
	int iTargetType;			// Target Type (AIRCRAFT,MOVINGVEHICLE,SHIP,GROUNDOBJECT)
	int iTargetId;				// Database ID of Target Type (-1 if GROUNDOBJECT)
	int iTargetIndex;			// Index or serial number of Target
	double lX;
	double lY;
	double lZ;
	unsigned short Roll;
	unsigned short Pitch;
	unsigned short Heading;
};
typedef struct stWeaponPositionInfo WeaponPositionInfo;


struct stObjectDamageInfo
{
	int iObjectType;			// AIRCRAFT,MOVINGVEHICLE,SHIP,GROUNDOBJECT	
	int iObjectID;				// Database ID
	int iObjectIndex;			// Index
	DWORD dwDamageType;			
	long lWeaponId;				// Unique ID
	long lWeaponType;			// Database ID of Weapon
	long lLauncher;				// AIRCRAFT,MOVINGVEHICLE,SHIP,GROUNDOBJECT
	long lLauncherId;			// Database ID of Launcher, -1 if GROUNDOBJECT
	long lLauncherIndex;		// Index or serial number 
	double lX;
	double lY;
	double lZ;
};
typedef struct stObjectDamageInfo ObjectDamageInfo;

struct stVehiclePositionInfo 
{
	long lType;					// SHIP or MOVINGVEHICLE
	long lVehicleID;			// Database ID
	long lVehicleIndex;			// Index into vehicle list
	double lX;
	double lY;
	double lZ;
	unsigned short Roll;
	unsigned short Pitch;
	unsigned short Heading;
};
typedef struct stVehiclePositionInfo VehiclePositionInfo;

struct stEventLogMissionInfo
{
	char sMissionFileName[260];
	int iReserved1;
	int iReserved2;
	int iReserved3;
};
typedef struct stEventLogMissionInfo EventLogMissionInfo;



//***********************************************************************************************************************************
// Event Log supporting functions

int InitEventLog();
void SuspendEventLogging();
void ResumeEventLogging();
void TerminateEventLog();
BOOL CheckValidEvent(long lEventId);
void UpdateEventLog(long lEventId,long lUserId1,long lUserId2,void *pStuff, int iObjectSize);
void LogPlaneMovements(void);
void LogEvent(long lEventId, int iMode);
char *GetCurrentCampaignFile();
void SetCurrentCampaignFile(char *szFile);



//***********************************************************************************************************************************
// Statisitcs

struct stMissionStatistics
{
	int iAircraftIndex;
	int iBriefingGroupId;
	int iNumInFlight;
	int iAircraftType;
	int iReserved[10];
	int iFlagList[MAX_NUM_EVENT_FLAGS];
	int iNumAircraft;
	int *piAircraftList;
	int iNumMovingVehicle;
	int *piMovingVehicleList;
	int iNumShip;
	int *piShipList;
	int iNumGroundObject;
	int *piGroundObjectList;
};
typedef struct stMissionStatistics MissionStatisticsType;


#endif

