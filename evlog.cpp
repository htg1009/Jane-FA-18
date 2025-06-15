#include "F18.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "gamesettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------------------------------------
#define SEC	* 50
#define EVENT_MAX_FILE_SIZE		8000000

//---------------------------------------------------------------------------------------------------------------------
int iTrackBullets=0;
unsigned long lAircraftSampleRate =	3 SEC;
unsigned long lWeaponSampleRate =	3 SEC;
unsigned long lVehicleSampleRate = 	5 SEC;
EventLogType EventLog = { 0,0,-1,0,NULL };
unsigned long prevAircraftTimer1=0;
unsigned long prevWeaponTimer1=0;
unsigned long prevVehicleTimer1=0;
long g_iWeaponID=0;
char szCurrentCampaignFile[260];


MissionStatisticsType *MissionStatistics=NULL;
//---------------------------------------------------------------------------------------------------------------------
void LogMissionInfo();

extern int VConvertVNumToGVListNum(int vnum);

extern GAMESETTINGS g_Settings;							// Global settings


//***********************************************************************************************************************************
char *GetCurrentCampaignFile()
{
	return(szCurrentCampaignFile);
};

//***********************************************************************************************************************************
void SetCurrentCampaignFile(char *szFile)
{
	strcpy(szCurrentCampaignFile,szFile);
}


//***********************************************************************************************************************************
int InitEventLog()
{
	EventLog.pEventBuffer=NULL;
	EventLog.lCurrentEventBufferPos=0;
	EventLog.iInitialized=0;
	EventLog.iSuspend=0;
	EventLog.iEventLogFile=-1;
	EventLog.iLogLevel=1;

	prevAircraftTimer1=0;
	prevWeaponTimer1=0;

	g_iWeaponID=0;

	char	*pszPath;
	char	szFullPath[260];

	pszPath = GetRegValue ("resource");
	sprintf (szFullPath, "%s\\%s", pszPath,"f18log.mlf");

	if ((EventLog.iEventLogFile=_open(szFullPath,_O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _S_IWRITE | _S_IREAD)) == -1)
	{
		return(0);
	}

	EventLog.pEventBuffer = (unsigned char *)malloc(EVENT_BUFFER_SIZE);
	if (!EventLog.pEventBuffer)
	{
		_close(EventLog.iEventLogFile);
		return(0);
	}
	EventLog.iInitialized=1;
	memset(EventLog.pEventBuffer,0,EVENT_BUFFER_SIZE);

	EventLog.iLogLevel = GetRegValueL("LogLevel");
	if (!EventLog.iLogLevel)
	{
		EventLog.iLogLevel=1;
	}
	LogEvent(EVL_HOUSEKEEPING,EVL_HOUSEKEEPING_LOGSTART);
	LogMissionInfo();
	
	return(1);
}



//***********************************************************************************************************************************
BOOL CheckValidEvent(long lEventId)
{
	return TRUE;
}


//***********************************************************************************************************************************
void UpdateEventLog(long lEventId,long lUserId1,long lUserId2,void *pStuff, int iObjectSize)
{
	EventHeader EventItemHeader;
	int iHeaderSize;

	if (!EventLog.iInitialized) return;
	if (EventLog.iSuspend) return;
	if (!CheckValidEvent(lEventId)) return;
	if (iObjectSize>=EVENT_BUFFER_SIZE) return;	

	if (EventLog.lTotalSize>=EVENT_MAX_FILE_SIZE)
	{
		TerminateEventLog();
		return;
	}

	EventItemHeader.lEventId = lEventId;
	EventItemHeader.lUserId1 = lUserId1;
	EventItemHeader.lUserId2 = lUserId2;
	EventItemHeader.lTickCount = GameLoop;

	iHeaderSize = sizeof(EventItemHeader);

	if ((iObjectSize + iHeaderSize + EventLog.lCurrentEventBufferPos)>=EVENT_BUFFER_SIZE)
	{
		_write(EventLog.iEventLogFile,EventLog.pEventBuffer,EventLog.lCurrentEventBufferPos);
		memset(EventLog.pEventBuffer,0,EVENT_BUFFER_SIZE);
		EventLog.lCurrentEventBufferPos=0;
		LogEvent(EVL_HOUSEKEEPING,EVL_HOUSEKEEPING_CACHEWRITE);
		EventLog.lTotalSize+=EVENT_BUFFER_SIZE;
	}

	memcpy(&(EventLog.pEventBuffer[EventLog.lCurrentEventBufferPos]),&EventItemHeader,iHeaderSize);
	EventLog.lCurrentEventBufferPos+=iHeaderSize;
	if (iObjectSize)
	{
		memcpy(&(EventLog.pEventBuffer[EventLog.lCurrentEventBufferPos]),pStuff,iObjectSize);
		EventLog.lCurrentEventBufferPos+=iObjectSize;
	}
}

//***********************************************************************************************************************************
void SuspendEventLogging()
{
	if (!EventLog.iInitialized) return;
	EventLog.iSuspend=1;
}

//***********************************************************************************************************************************
void ResumeEventLogging()
{
	if (!EventLog.iInitialized) return;
	EventLog.iSuspend=0;
}


//***********************************************************************************************************************************
void TerminateEventLog()
{
	if (!EventLog.iInitialized) return;

	_write(EventLog.iEventLogFile,EventLog.pEventBuffer,EventLog.lCurrentEventBufferPos);
	_close(EventLog.iEventLogFile);
	
	if (EventLog.pEventBuffer)
	{
		free(EventLog.pEventBuffer);
		EventLog.pEventBuffer=NULL;
	}

	EventLog.pEventBuffer=NULL;
	EventLog.lCurrentEventBufferPos=0;
	EventLog.iInitialized=0;
	EventLog.iSuspend=0;
	EventLog.iEventLogFile=-1;
}

//***********************************************************************************************************************************
void ShutdownEventLog()
{
	LogEvent(EVL_HOUSEKEEPING,EVL_HOUSEKEEPING_LOGEND);
	TerminateEventLog();
}

//***********************************************************************************************************************************
void LogEvent(long lEventId, int iMode)
{
	UpdateEventLog(lEventId,iMode,0,NULL,0);
}


//***********************************************************************************************************************************
void LogAircraftEvent(long lEventId, PlaneParams *pPlaneTrack)
{
	AircraftPositionInfo PosInfo;

	PosInfo.lAircraftType = pPlaneTrack->AI.lPlaneID;
	PosInfo.lAircraftIndex = pPlaneTrack-Planes;

	PosInfo.lX=(pPlaneTrack->WorldPosition.X);
	PosInfo.lY=(pPlaneTrack->WorldPosition.Y);
	PosInfo.lZ=(pPlaneTrack->WorldPosition.Z);
	PosInfo.Roll=pPlaneTrack->Roll;
	PosInfo.Pitch=pPlaneTrack->Pitch;
	PosInfo.Heading=pPlaneTrack->Heading;
	PosInfo.Velocity=pPlaneTrack->V;

	void *pDataPtr = &PosInfo;
	int iDataSize = sizeof(PosInfo);

	UpdateEventLog(lEventId,EVL_OBJECT_POS_AIRCRAFT,0,pDataPtr,iDataSize);
}


//************************************************************************************************
void LogPlaneMovements(void)
{
	if (!lAircraftSampleRate) return;

	PlaneParams *P = &Planes[0];

	if ((GameLoop - prevAircraftTimer1) > lAircraftSampleRate)
	{
		while (P <= LastPlane) 
		{
			if (P->Status & PL_ACTIVE)
			{
				if (!(P->FlightStatus & PL_STATUS_CRASHED))
				{
					LogAircraftEvent(EVL_OBJECT_POS,P);
				}
			}
			P++;
		}
		prevAircraftTimer1=GameLoop;
	}
}


//***********************************************************************************************************************************
void LogWeaponEvent(long lEventId, WeaponParams *pWeapon)
{
	WeaponPositionInfo PosInfo;

	memset(&PosInfo,0,sizeof(WeaponPositionInfo));

	if (!pWeapon) return;

	if (!iTrackBullets)
	{
		if (pWeapon->Kind == BULLET) return;
	}

	if ((int)pWeapon->Type < 0x200)
	{
		PosInfo.lType=pDBWeaponList[AIGetWeaponIndex((int)pWeapon->Type)].lWeaponID;
	}
	else
	{
		PosInfo.lType=pDBWeaponList[AIGetWeaponIndex((int)pWeapon->Type->TypeNumber)].lWeaponID;
	}

	PosInfo.lWeaponId=pWeapon->iNetID;

	PosInfo.lLauncher=pWeapon->LauncherType;

	if (pWeapon->P)
	{
		PlaneParams *P = (PlaneParams *)pWeapon->P;
		if (P)
		{
			PosInfo.lLauncher=AIRCRAFT;
			PosInfo.lLauncherId = P->AI.lPlaneID;
			PosInfo.lLauncherIndex = P-Planes;
		}
	} else {
		switch(pWeapon->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)pWeapon->Launcher;
				if (P)
				{
					PosInfo.lLauncher=AIRCRAFT;
					PosInfo.lLauncherId = P->AI.lPlaneID;
					PosInfo.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)pWeapon->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						PosInfo.lLauncher=MOVINGVEHICLE;
						PosInfo.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						PosInfo.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						PosInfo.lLauncher=SHIP;
						PosInfo.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						PosInfo.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)pWeapon->Launcher;
				if (pGroundInstance)
				{
					PosInfo.lLauncherId = -1;
					PosInfo.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}

	PosInfo.iTargetType=pWeapon->iTargetType;
	PosInfo.iTargetIndex=-1;

	switch(pWeapon->iTargetType)
	{
		case TARGET_PLANE:
		{
			PosInfo.iTargetType=AIRCRAFT;
			PlaneParams *P = (PlaneParams *)pWeapon->pTarget;
			if (P)
			{
				PosInfo.iTargetId=P->AI.lPlaneID;
				PosInfo.iTargetIndex=P-Planes;
			}
		}
		break;
		case TARGET_GROUND:
			PosInfo.iTargetType=GROUNDOBJECT;
			BasicInstance *pGroundInstance = (BasicInstance *)pWeapon->pTarget;
			if (pGroundInstance)
			{
				PosInfo.iTargetId=-1;
				PosInfo.iTargetIndex=pGroundInstance->SerialNumber;
			}
		break;
	}

	PosInfo.lX=(pWeapon->Pos.X);
	PosInfo.lY=(pWeapon->Pos.Y);
	PosInfo.lZ=(pWeapon->Pos.Z);
	PosInfo.Roll=pWeapon->Roll;
	PosInfo.Pitch=pWeapon->Pitch;
	PosInfo.Heading=pWeapon->Heading;


	void *pDataPtr = &PosInfo;
	int iDataSize = sizeof(PosInfo);

	UpdateEventLog(lEventId,EVL_OBJECT_POS_WEAPON,0,pDataPtr,iDataSize);
}


//***********************************************************************************************************************************
void LogVehicleEvent(long lEventId, MovingVehicleParams *pVehicle)
{
	VehiclePositionInfo PosInfo;

	if (pVehicle->iShipType)
	{
		PosInfo.lType = SHIP;
		PosInfo.lVehicleID = pDBShipList[pVehicle->iVDBIndex].lShipID;
	}
	else
	{
		PosInfo.lType = MOVINGVEHICLE;
		PosInfo.lVehicleID = pDBVehicleList[pVehicle->iVDBIndex].lVehicleID;
	}
	PosInfo.lVehicleIndex = VConvertVNumToGVListNum(pVehicle-MovingVehicles);

	PosInfo.lX=(pVehicle->WorldPosition.X);
	PosInfo.lY=(pVehicle->WorldPosition.Y);
	PosInfo.lZ=(pVehicle->WorldPosition.Z);
	PosInfo.Pitch=pVehicle->Pitch;
	PosInfo.Heading=pVehicle->Heading;

	void *pDataPtr = &PosInfo;
	int iDataSize = sizeof(PosInfo);

	UpdateEventLog(lEventId,EVL_OBJECT_POS_VEHICLE,0,pDataPtr,iDataSize);
}


//***********************************************************************************************************************************
void LogWeaponMovements()
{
	if (!lWeaponSampleRate) return;

	if ((GameLoop - prevWeaponTimer1) > lWeaponSampleRate)
	{
		for (WeaponParams *W=Weapons; W<=LastWeapon; W++)
		{
			if (W->Flags & WEAPON_INUSE)
			{
				LogWeaponEvent(EVL_OBJECT_POS,W);
			}
		}
		prevWeaponTimer1=GameLoop;
	}
}

//***********************************************************************************************************************************
void LogVehicleMovements()
{
	MovingVehicleParams *pVehicle;

	if (!lVehicleSampleRate) return;

	if ((GameLoop - prevVehicleTimer1) > lVehicleSampleRate)
	{
		for(pVehicle = MovingVehicles; pVehicle <= LastMovingVehicle; pVehicle++)
		{
			if (pVehicle->Status & VL_ACTIVE)
			{
				LogVehicleEvent(EVL_OBJECT_POS,pVehicle);
			}
		}
		prevVehicleTimer1=GameLoop;
	}
}

extern char	g_szMissionFileName[260];

//**************************************************************************************
void LogMissionInfo()
{
	EventLogMissionInfo LogMissionInfo;

	strcpy(LogMissionInfo.sMissionFileName,g_szMissionFileName);
	LogMissionInfo.iReserved1=0;
	LogMissionInfo.iReserved2=0;
	LogMissionInfo.iReserved3=0;

	void *pDataPtr = &LogMissionInfo;
	int iDataSize = sizeof(LogMissionInfo);

	UpdateEventLog(EVL_HOUSEKEEPING,EVL_HOUSEKEEPING_MISSIONINFO,0,pDataPtr,iDataSize);
}



//**************************************************************************************
void LogSentenceEvent(int planenum,int groundvoice,int iChannel,int iVolume,int numids,int *sndids)
{
	SoundSentenceInfo SentenceInfo;

	SentenceInfo.iAircraftIndex = planenum;
	SentenceInfo.iGroundVoice = groundvoice;
	SentenceInfo.iNumIds = numids;
	SentenceInfo.iChannel= iChannel;
	SentenceInfo.iVolume = iVolume;

	for (int i=0; ((i<numids) && (i<30)); i++)
		SentenceInfo.SoundId[i]=sndids[i];

	void *pDataPtr = &SentenceInfo;
	int iDataSize = sizeof(SoundSentenceInfo);

	UpdateEventLog(EVL_OBJECT_INSTANTIATE,EVL_OBJECT_INSTANTIATE_SENTENCE,0,pDataPtr,iDataSize);
}


//************************************************************************************************
void LogEventPlaneHit(PlaneParams *P,WeaponParams *W,DWORD damage_type)
{
	ObjectDamageInfo ObjectDamage;

	if (!P) return;
	if (!W) return;

	ObjectDamage.iObjectType = AIRCRAFT;
	ObjectDamage.iObjectID = P->AI.lPlaneID;
	ObjectDamage.iObjectIndex = P-Planes;
	ObjectDamage.dwDamageType = damage_type;
	ObjectDamage.lWeaponId = W->iNetID;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}

	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}
	
	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_HIT,0,pDataPtr,iDataSize);
}

//************************************************************************************************
void LogEventPlaneDamage(PlaneParams *P,WeaponParams *W,DWORD damage_type)
{
	ObjectDamageInfo ObjectDamage;

	if (!P) return;
	if (!W) return;

	ObjectDamage.iObjectType = AIRCRAFT;
	ObjectDamage.iObjectID = P->AI.lPlaneID;
	ObjectDamage.iObjectIndex = P-Planes;
	ObjectDamage.dwDamageType = damage_type;
	ObjectDamage.lWeaponId = W->iNetID;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}

	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}

	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_DAMAGED,0,pDataPtr,iDataSize);
}

//************************************************************************************************
void LogEventPlaneDestroyed(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane)
{
	ObjectDamageInfo ObjectDamage;

	if (!P) return;
	if (!W) return;

	ObjectDamage.iObjectType = AIRCRAFT;
	ObjectDamage.iObjectID = P->AI.lPlaneID;
	ObjectDamage.iObjectIndex = P-Planes;
	ObjectDamage.dwDamageType = -1;
	ObjectDamage.lWeaponId = -1;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}

	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}

	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_DESTROYED,0,pDataPtr,iDataSize);
}

//************************************************************************************************
void LogEventGroundObjectDestroyed(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID)
{
	ObjectDamageInfo ObjectDamage;

	if (!W) return;

	ObjectDamage.iObjectType = GROUNDOBJECT;
	ObjectDamage.iObjectIndex = iID;
	ObjectDamage.dwDamageType = damage_type;
	ObjectDamage.lWeaponId = W->iNetID;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}

	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}

	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_DESTROYED,0,pDataPtr,iDataSize);
}

//************************************************************************************************
void LogEventGroundObjectDamaged(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID)
{
	ObjectDamageInfo ObjectDamage;

	if (!W) return;

	ObjectDamage.iObjectType = GROUNDOBJECT;
	ObjectDamage.iObjectIndex = iID;
	ObjectDamage.dwDamageType = damage_type;
	ObjectDamage.lWeaponId = W->iNetID;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}

	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}

	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_DAMAGED,0,pDataPtr,iDataSize);
}


//************************************************************************************************
void LogEventVehicleDestroyed(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID,int iVehicleType,int iVehicleID)
{
	ObjectDamageInfo ObjectDamage;

	if (!W) return;

	ObjectDamage.iObjectType = iVehicleType;
	ObjectDamage.iObjectID = iVehicleID;
	ObjectDamage.iObjectIndex = iID;
	ObjectDamage.dwDamageType = damage_type;
	ObjectDamage.lWeaponId = W->iNetID;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}


	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}

	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_DESTROYED,0,pDataPtr,iDataSize);
}

//************************************************************************************************
void LogEventVehicleDamaged(PlaneParams *P,WeaponParams *W,DWORD damage_type,int iCrashedPlane,int iID,int iVehicleType,int iVehicleID)
{
	ObjectDamageInfo ObjectDamage;

	if (!W) return;

	ObjectDamage.iObjectType = iVehicleType;
	ObjectDamage.iObjectID = iVehicleID;
	ObjectDamage.iObjectIndex = iID;
	ObjectDamage.dwDamageType = damage_type;
	ObjectDamage.lWeaponId = W->iNetID;
	ObjectDamage.lX=(W->Pos.X);
	ObjectDamage.lY=(W->Pos.Y);
	ObjectDamage.lZ=(W->Pos.Z);

	ObjectDamage.lLauncher=W->LauncherType;

	if (W->P)
	{
		PlaneParams *P = (PlaneParams *)W->P;
		if (P)
		{
			ObjectDamage.lLauncherId = P->AI.lPlaneID;
			ObjectDamage.lLauncherIndex = P-Planes;
		}
	} else {
		switch(W->LauncherType)
		{
			case AIRCRAFT:
			{
				PlaneParams *P = (PlaneParams *)W->Launcher;
				if (P)
				{
					ObjectDamage.lLauncherId = P->AI.lPlaneID;
					ObjectDamage.lLauncherIndex = P-Planes;
				}
			}
			break;
			case MOVINGVEHICLE:
			{
				MovingVehicleParams *vehiclepnt = (MovingVehicleParams *)W->Launcher;
				if (vehiclepnt)
				{
					if (!vehiclepnt->iShipType)
					{
						ObjectDamage.lLauncher=MOVINGVEHICLE;
						ObjectDamage.lLauncherId = pDBVehicleList[vehiclepnt->iVDBIndex].lVehicleID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					} else {
						ObjectDamage.lLauncher=SHIP;
						ObjectDamage.lLauncherId = pDBShipList[vehiclepnt->iVDBIndex].lShipID;
						ObjectDamage.lLauncherIndex = VConvertVNumToGVListNum(vehiclepnt-MovingVehicles);
					}
				}
			}
			break;
			case GROUNDOBJECT:
			{
				BasicInstance *pGroundInstance = (BasicInstance *)W->Launcher;
				if (pGroundInstance)
				{
					ObjectDamage.lLauncherId = -1;
					ObjectDamage.lLauncherIndex = pGroundInstance->SerialNumber;
				}
			}
			break;
		}
	}


	if ((int)W->Type < 0x200)
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type)].lWeaponID;
	}
	else
	{
		ObjectDamage.lWeaponType=pDBWeaponList[AIGetWeaponIndex((int)W->Type->TypeNumber)].lWeaponID;
	}

	void *pDataPtr = &ObjectDamage;
	int iDataSize = sizeof(ObjectDamageInfo);

	UpdateEventLog(EVL_OBJECT_STATECHANGE,EVL_OBJECT_STATECHANGE_DAMAGED,0,pDataPtr,iDataSize);
}





//************************************************************************************************
void LogData()
{
	LogPlaneMovements();
	LogWeaponMovements();
	LogVehicleMovements();
}

//*********************************************************************************************************************
// Should Only be called after mission is loaded
void InitMissionStatistics()
{
	if (iNumAIObjs<=0) return;
	if (iNumAIObjs>MAXAIOBJS) return;

	MissionStatistics=NULL;

	MissionStatistics = (MissionStatisticsType *)malloc(iNumAIObjs * sizeof(MissionStatisticsType));

	if (!MissionStatistics) return;

	memset(MissionStatistics,0,(iNumAIObjs * sizeof(MissionStatisticsType)));
	for (int i=0; i<iNumAIObjs; i++) 
	{
		MissionStatistics[i].iAircraftIndex=i;
		MissionStatistics[i].iNumInFlight=AIObjects[i].iNumInFlight;
		MissionStatistics[i].iAircraftType=AIObjects[i].iType;
	}
}

//*********************************************************************************************************************
void CleanupMissionStatistics()
{
	if (MissionStatistics)
	{
		free(MissionStatistics);
		MissionStatistics=NULL;
	}
}

//*********************************************************************************************************************
void DumpFlightStatistics()
{
	char *pszPath;
	char szFullPath[260];
	int hDebriefFile=-1;

	if (!MissionStatistics) return;

	pszPath = GetRegValue("resource");
	sprintf (szFullPath, "%s\\mission.st1", pszPath);

	if ((hDebriefFile=_open(szFullPath,_O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _S_IWRITE | _S_IREAD)) != -1)
	{
		_write(hDebriefFile,&iNumAIObjs,sizeof(int));

		for (int i=0; i<iNumAIObjs; i++)
		{
			_write(hDebriefFile,&(MissionStatistics[i]),sizeof(MissionStatisticsType));
			if (MissionStatistics[i].iNumAircraft)
				_write(hDebriefFile,MissionStatistics[i].piAircraftList,(sizeof(int)*MissionStatistics[i].iNumAircraft));
			if (MissionStatistics[i].iNumMovingVehicle)
				_write(hDebriefFile,MissionStatistics[i].piMovingVehicleList,(sizeof(int)*MissionStatistics[i].iNumMovingVehicle));
			if (MissionStatistics[i].iNumGroundObject)
				_write(hDebriefFile,MissionStatistics[i].piGroundObjectList,(sizeof(int)*MissionStatistics[i].iNumGroundObject));
			if (MissionStatistics[i].iNumShip)
				_write(hDebriefFile,MissionStatistics[i].piShipList,(sizeof(int)*MissionStatistics[i].iNumShip));
		}
		_close(hDebriefFile);
	}
}

//*********************************************************************************************************************
void LogMissionStatistic(int iType, int iAircraftIndex, int iValue, int iOperator)
{				  
	if (iAircraftIndex<0) return;			// Check for invalid aircraft index
	if (iAircraftIndex>=iNumAIObjs) return;

	if (iType<0) return;					// Check for invalid event log id
	if (iType>MAX_NUM_EVENT_FLAGS) return;

	if (!MissionStatistics) return;			// Make sure we are still around

	PlaneParams *P = &Planes[iAircraftIndex];

	if (P==PlayerPlane)
	{
		if (g_Settings.gp.dwFlight & GP_FLIGHT_CRASHES)
		{
			switch(iType)
			{
				case LOG_FLIGHT_CRASHED_GROUND:
				case LOG_FLIGHT_CRASHED_WATER:
				case LOG_FLIGHT_CRASHED_RUNWAY:
				case LOG_FLIGHT_CRASHED_MOUNTAIN:
				case LOG_FLIGHT_CRASHED_GROUND_OBJECT:
				case LOG_FLIGHT_CRASHED_AIRCRAFT:
					return;
				break;
			}
		}

		if (g_Settings.gp.dwCheats & GP_CHEATS_INVULNERABLE)
		{
			switch(iType)
			{
				case LOG_FLIGHT_DAMAGED:
				case LOG_FLIGHT_SHOTDOWN_OWN_FRAG:
				case LOG_FLIGHT_SHOTDOWN_ENEMY_AAA:
				case LOG_FLIGHT_SHOTDOWN_ENEMY_SAM:
				case LOG_FLIGHT_SHOTDOWN_ENEMY_MISSILE:
				case LOG_FLIGHT_SHOTDOWN_ENEMY_GUNS:
				case LOG_FLIGHT_SHOTDOWN_ENEMY_BOMB:
				case LOG_FLIGHT_SHOTDOWN_ENEMY_FRAG:
				case LOG_FLIGHT_SHOTDOWN_FRIENDLY_AAA:
				case LOG_FLIGHT_SHOTDOWN_FRIENDLY_SAM:
				case LOG_FLIGHT_SHOTDOWN_FRIENDLY_MISSILE:
				case LOG_FLIGHT_SHOTDOWN_FRIENDLY_GUNS:
				case LOG_FLIGHT_SHOTDOWN_FRIENDLY_BOMB:
				case LOG_FLIGHT_SHOTDOWN_FRIENDLY_FRAG:
					return;
				break;
			}
		}
	}

	switch(iOperator)
	{
		case 0:		// Replace
			MissionStatistics[iAircraftIndex].iFlagList[iType] = iValue;
		break;
		case 1:		// Add
			MissionStatistics[iAircraftIndex].iFlagList[iType] += iValue;
		break;
		default:	// I'm not sure what to do, so lets replace the value
			MissionStatistics[iAircraftIndex].iFlagList[iType] = iValue;
		break;
	}
}

//*********************************************************************************************************************
void LogMissionStatisticForAll(int iType, int iValue, int iOperator)
{				  
	if (iType<0) return;					// Check for invalid event log id
	if (iType>MAX_NUM_EVENT_FLAGS) return;

	if (!MissionStatistics) return;			// Make sure we are still around


	for (int i=0; i<iNumAIObjs; i++)
	{
		switch(iOperator)
		{
			case 0:		// Replace
				MissionStatistics[i].iFlagList[iType] = iValue;
			break;
			case 1:		// Add
				MissionStatistics[i].iFlagList[iType] += iValue;
			break;
			default:	// I'm not sure what to do, so lets replace the value
				MissionStatistics[i].iFlagList[iType] = iValue;
			break;
		}
	}
}


//*********************************************************************************************************************
void UpdateRatiosForAll()
{
	float fTmpVal1=0.0f;
	float fTmpVal2=0.0f;
	int iValue=0;

	for (int i=0; i<iNumAIObjs; i++)
	{
		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_BOMBS_RELEASED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_BOMBS_HIT];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_BOMB_HIT_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_MISSILES_RELEASED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_MISSILE_HITS];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_MISSILE_HIT_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_GUN_ROUNDS_FIRED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_GUN_ROUNDS_HIT];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_GUN_HIT_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_GUN_ROUNDS_FIRED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_GUN_ROUND_KILLS];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_GUN_KILL_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_AA_MISSILES_RELEASED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_AA_MISSILES_KILLS];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_AA_MISSILE_KILL_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_AA_MISSILES_RELEASED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_AA_MISSILES_HIT];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_AA_MISSILE_HIT_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_BOMBS_RELEASED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_BOMB_KILLS];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_BOMB_KILL_RATIO] = iValue;

		// 
		iValue=0;
		fTmpVal1=MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_MISSILES_RELEASED];
		fTmpVal2=MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_MISSILE_KILLS];

		if ((fTmpVal1) && (fTmpVal2))
			iValue = (float)(fTmpVal2/fTmpVal1)*100;
		if (iValue<0) iValue=0;
		if (iValue>100) iValue=100;
					
		MissionStatistics[i].iFlagList[LOG_FLIGHT_AG_MISSILE_KILL_RATIO] = iValue;
	}
}


//*********************************************************************************************************************
void LogMissionStatisticKill(int iAircraftIndex, int iType, long lObjectID)
{				  
	if (iAircraftIndex<0) return;			// Check for invalid aircraft index
	if (iAircraftIndex>=iNumAIObjs) return;

	if (iType<0) return;					// Check for invalid
	if (iType>10) return;

	if (!MissionStatistics) return;			// Make sure we are still around

	switch(iType)
	{
		case AIRCRAFT:
			MissionStatistics[iAircraftIndex].piAircraftList = (int *)realloc(MissionStatistics[iAircraftIndex].piAircraftList,
				(sizeof(int) * (MissionStatistics[iAircraftIndex].iNumAircraft+1)));

			if (lObjectID<=iNumAIObjs)
				MissionStatistics[iAircraftIndex].piAircraftList[MissionStatistics[iAircraftIndex].iNumAircraft]=AIObjects[lObjectID].iType;
			else
				MissionStatistics[iAircraftIndex].piAircraftList[MissionStatistics[iAircraftIndex].iNumAircraft]=-1;

			MissionStatistics[iAircraftIndex].iNumAircraft++;
		break;
		case MOVINGVEHICLE:
			MissionStatistics[iAircraftIndex].piMovingVehicleList = (int *)realloc(MissionStatistics[iAircraftIndex].piMovingVehicleList,
				(sizeof(int) * (MissionStatistics[iAircraftIndex].iNumMovingVehicle+1)));

			MissionStatistics[iAircraftIndex].piMovingVehicleList[MissionStatistics[iAircraftIndex].iNumMovingVehicle]=MovingVehicles[lObjectID].iType;

			MissionStatistics[iAircraftIndex].iNumMovingVehicle++;
		break;
		case GROUNDOBJECT:
			MissionStatistics[iAircraftIndex].piGroundObjectList = (int *)realloc(MissionStatistics[iAircraftIndex].piGroundObjectList,
				(sizeof(int) * (MissionStatistics[iAircraftIndex].iNumGroundObject+1)));
			MissionStatistics[iAircraftIndex].piGroundObjectList[MissionStatistics[iAircraftIndex].iNumGroundObject]=lObjectID;
			MissionStatistics[iAircraftIndex].iNumGroundObject++;
		break;
		case SHIP:
			MissionStatistics[iAircraftIndex].piShipList = (int *)realloc(MissionStatistics[iAircraftIndex].piShipList,
				(sizeof(int) * (MissionStatistics[iAircraftIndex].iNumShip+1)));

			MissionStatistics[iAircraftIndex].piShipList[MissionStatistics[iAircraftIndex].iNumShip]=MovingVehicles[lObjectID].iType;

			MissionStatistics[iAircraftIndex].iNumShip++;
		break;
	}
}

