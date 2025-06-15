//*****************************************************************************************
// LOADMISS.CPP
//*****************************************************************************************
#include "F18.h"
#include "resources.h"
#include "MultiDefs.h"
#include "MSAPI.h"
#include "SkunkNet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define		LOCATION_MODE_OFF		0
#define		LOCATION_MODE_EXTERNAL	1
#define		LOCATION_MODE_MB		2

extern NetSlot Slotarray[MAX_HUMANS + 1];

//---------------------------------------------------------------------------------------------------------------------
struct stBuilderIdTag  //  This is needed for loading the fence area.  SRE
{
	char sTag[4];
	long lBuilderFormatIdTag;
	long lVersion;
	long lSubVersion;
};
typedef struct stBuilderIdTag BuilderIdTagType;

int		g_iCampaignMode=0;
long	g_lTimeMin=0;
long	g_lTimeHour=0;

//---------------------------------------------------------------------------------------------------------------------
extern "C" __declspec( dllexport ) void SetCampaignMode(int iMode);

void HandleRandomPlanes();
void InitGroundSides();


//---------------------------------------------------------------------------------------------------------------------
extern void		LoadMovingVehicleInfo();
extern void		TranslateMissionText(char *szFileName,int iMode);
extern int		g_iLocationMode;
extern char		g_szLocationFileName[260];
extern int		g_nMissionType;
extern char		sDebugTxt[512];
extern int		g_iNumAroundGoalObjectsHit;
extern int		g_iNumWeaponReleasedInAreaGoal;
void			WriteDumpLog (char *pszString);
extern int g_iNumWeaponAreaGoal;
extern int g_iNumWeaponAreaGoalHit;

extern void		InitMissionFlagStuff();
extern void DisplayInitText(char *sTxt, int SkipLine, int showrelease=0);


extern float	Visibility;
//*********************************************************************************************************************
void SetCampaignMode(int iMode)
{
	g_iCampaignMode=iMode;
}

MissionInfoType MissionInfo;

//***********************************************************************************************************************************
int LoadMissionVersion201(char *sFileName)
{
	int hMissionFile;
	long lTempSize;
	void *pTemp=NULL;
	long lVersion=-1;
	long lSubVersion=-1;

#if 0
	// Need to clean-up previous mission file first
	// Free up previously alloc'd memory and other ptr stuff

   	*LastPointer = pfnLoadInstancesFromMissionFile(sFileName,FALSE);
	pfnFixupInstanceDefLinks(WorldFamilyDefs,*LastPointer);

	typedef struct _MissionFileIndex
	{
		DWORD SizeOfInstances;
		DWORD SizeOfCommands;
	} MissionFileIndex;

	typedef struct _MissionFileHeader
	{
		char sTag[4];
		long lMissionFormatIdTag;
		long lVersion;
		long lSubVersion;
		MissionFileIndex FileIndex;
	} MissionFileHeader;
#endif

	if ((hMissionFile=_open(sFileName,_O_BINARY | _O_RDONLY)) == -1)
	{
		sprintf(sDebugTxt,"%s:Error - Could not open File %s",__FILE__,sFileName);
		DisplayInitText(sDebugTxt,1);

		LogError("Could not open file",__FILE__,__LINE__,0);
		return(0);
	}


	sprintf(sDebugTxt,"Loading Mission - %s",sFileName);
	DisplayInitText(sDebugTxt,1);


	MissionFileHeader header;
	EditorInfoType EditorInfo;

	_read(hMissionFile,&header,sizeof(MissionFileHeader));
	_lseek(hMissionFile,(header.FileIndex.SizeOfInstances+header.FileIndex.SizeOfCommands),SEEK_CUR);


	_read(hMissionFile,&EditorInfo,sizeof(EditorInfoType));
	_read(hMissionFile,&EditorInfo.lMissionTitleSize,sizeof(EditorInfo.lMissionTitleSize));
	if (EditorInfo.lMissionTitleSize)
	{
		EditorInfo.psTitle = (char *)malloc(EditorInfo.lMissionTitleSize);
		if (!EditorInfo.psTitle)
			LogError("Memory allocation failed",__FILE__,__LINE__,1);
		_read(hMissionFile,EditorInfo.psTitle,EditorInfo.lMissionTitleSize);
		free(EditorInfo.psTitle);
		EditorInfo.psTitle=NULL;
	}
	_read(hMissionFile,&EditorInfo.lAuthorSize,sizeof(EditorInfo.lAuthorSize));
	if (EditorInfo.lAuthorSize)
	{
		EditorInfo.psAuthor = (char *)malloc(EditorInfo.lAuthorSize);
		if (!EditorInfo.psAuthor)
			LogError("Memory allocation failed",__FILE__,__LINE__,1);
		_read(hMissionFile,EditorInfo.psAuthor,EditorInfo.lAuthorSize);
		free(EditorInfo.psAuthor);
		EditorInfo.psAuthor=NULL;
	}
	_read(hMissionFile,&EditorInfo.lMissionSummarySize,sizeof(EditorInfo.lMissionSummarySize));
	if (EditorInfo.lMissionSummarySize)
	{
		EditorInfo.psMissionSummary = (char *)malloc(EditorInfo.lMissionSummarySize);
		if (!EditorInfo.psMissionSummary)
			LogError("Memory allocation failed",__FILE__,__LINE__,1);
		_read(hMissionFile,EditorInfo.psMissionSummary,EditorInfo.lMissionSummarySize);
		free(EditorInfo.psMissionSummary);
		EditorInfo.psMissionSummary=NULL;
	}
	_read(hMissionFile,&EditorInfo.lCommentsSize,sizeof(EditorInfo.lCommentsSize));
	if (EditorInfo.lCommentsSize)
	{
		EditorInfo.psComments = (char *)malloc(EditorInfo.lCommentsSize);
		if (!EditorInfo.psComments)
			LogError("Memory allocation failed",__FILE__,__LINE__,1);
		_read(hMissionFile,EditorInfo.psComments,EditorInfo.lCommentsSize);
		free(EditorInfo.psComments);
		EditorInfo.psComments=NULL;
	}

	_read(hMissionFile,&MissionInfo,sizeof(MissionInfoType));

	iAI_ROE[AI_ENEMY] = MissionInfo.lEnemyROE;
	iAI_ROE[AI_FRIENDLY] = MissionInfo.lFriendlyROE;
	iAI_ROE[AI_NEUTRAL] = 0;  //  Weapons Hold (Green)

	//***********************************************************************************************
	//  Here I'm converting from Mission builder fence order to FenceArea Fence order
	//
	//  In the builder the order is
	//
	//  Norway = FENCE1 = 0x1
	//	Sweden = FENCE2	= 0x2
	//	Finland = FENCE3 = 0x4
	//  Russia = FENCE4 = 0x8
	//
	//
	//	In the game the FenceArea order is
	//
	//	Russia = FenceArea[0]	(sim fence1)
	//	Norway = FenceArea[1]	(sim fence2)
	//  Finland = FenceArea[2]	(sim fence3)
	//	Sweden	= FenceArea[3]	(sim fence4)
	//
	//  So	Sim		=	Wrapper
	//		------------------------------
	//		FENCE1	=	FENCE4		Russia
	//		FENCE2	=	FENCE1		Norway
	//		FENCE3	=	FENCE3		Finland
	//		FENCE4	=	FENCE2		Sweden
	//
	//  And	Wrapper	=	Sim
	//		------------------------------
	//		FENCE1	=	FENCE2		Norway
	//		FENCE2	=	FENCE4		Sweden
	//		FENCE3	=	FENCE3		Finland
	//		FENCE4	=	FENCE1		Russia

//	lFenceFlags = MissionInfo.lFence;

	lFenceFlags = 0;
	if(MissionInfo.lFence & FENCE1)
	{
		lFenceFlags |= FENCE2;
	}

	if(MissionInfo.lFence & FENCE2)
	{
		lFenceFlags |= FENCE4;
	}

	if(MissionInfo.lFence & FENCE3)
	{
		lFenceFlags |= FENCE3;
	}

	if(MissionInfo.lFence & FENCE4)
	{
		lFenceFlags |= FENCE1;
	}

	_read(hMissionFile,&MissionInfo.lBriefingSize,sizeof(MissionInfo.lBriefingSize));
	if (MissionInfo.lBriefingSize)
	{
		MissionInfo.psBriefing = (char *)malloc(MissionInfo.lBriefingSize);
		if (!MissionInfo.psBriefing)
			LogError("Memory allocation failed",__FILE__,__LINE__,1);
		_read(hMissionFile,MissionInfo.psBriefing,MissionInfo.lBriefingSize);
		free(MissionInfo.psBriefing);
		MissionInfo.psBriefing=NULL;
	}

	_read(hMissionFile,&iNumAIObjs,sizeof(iNumAIObjs));
	if (iNumAIObjs)
		_read(hMissionFile,AIObjects,(sizeof(MBObject) * iNumAIObjs));

	sprintf(sDebugTxt,"        Number of Aircraft: %d",iNumAIObjs);
	DisplayInitText(sDebugTxt,1);


	_read(hMissionFile,&iNumGroundObjects,sizeof(iNumGroundObjects));
	if (iNumGroundObjects)
	{
		pGroundObjectList = (GroundObject *)malloc(iNumGroundObjects*sizeof(GroundObject));
		_read(hMissionFile,pGroundObjectList,(sizeof(GroundObject) * iNumGroundObjects));
	}

	sprintf(sDebugTxt,"        Number of Ground Mission Plugins: %d",iNumGroundObjects);
	DisplayInitText(sDebugTxt,1);


	_read(hMissionFile,&g_iNumMovingVehicle,sizeof(g_iNumMovingVehicle));

	sprintf(sDebugTxt,"        Number of Moving Vehicles: %d",g_iNumMovingVehicle);
	DisplayInitText(sDebugTxt,1);

	if (g_iNumMovingVehicle)
	{
		g_pMovingVehicleList=(MovingVehicle *)malloc(sizeof(MovingVehicle)*g_iNumMovingVehicle);

		for (int i=0; i<g_iNumMovingVehicle; i++)
		{
			_read(hMissionFile,&g_pMovingVehicleList[i],sizeof(MovingVehicle));
			g_pMovingVehicleList[i].pRoutePointList=(RoutePoint *)malloc(sizeof(RoutePoint)*g_pMovingVehicleList[i].iNumPoints);

			for (int j=0; j<g_pMovingVehicleList[i].iNumPoints; j++)
			{
				_read(hMissionFile,&g_pMovingVehicleList[i].pRoutePointList[j],sizeof(RoutePoint));
			}
		}
	}

	_read(hMissionFile,&iNumWPPaths,sizeof(iNumWPPaths));
	sprintf(sDebugTxt,"        Number of Waypoint Paths: %d",iNumWPPaths);
	DisplayInitText(sDebugTxt,1);

	if (iNumWPPaths)
		_read(hMissionFile,AIWPPaths,(sizeof(MBWPPaths) * iNumWPPaths));

	_read(hMissionFile,&iNumWayPts,sizeof(iNumWayPts));
	sprintf(sDebugTxt,"        Number of Waypoints: %d",iNumWayPts);
	DisplayInitText(sDebugTxt,1);

	if (iNumWayPts)
		_read(hMissionFile,AIWayPoints,(sizeof(MBWayPoints) * iNumWayPts));

	_read(hMissionFile,&iNumActions,sizeof(iNumActions));

	sprintf(sDebugTxt,"        Number of AI Actions: %d",iNumActions);
	DisplayInitText(sDebugTxt,1);


	for (int i=0; i<iNumActions; i++)
	{
		_read(hMissionFile,&(AIActions[i]),sizeof(MBActions));

		lTempSize=0;
		pTemp=NULL;

		switch(AIActions[i].ActionID)
		{
			case ACTION_AWACS_PATTERN:
				pTemp = (AWACSPatternActionType *)AIActions[i].pAction;
				lTempSize = sizeof(AWACSPatternActionType);
			break;
			case ACTION_BOMB_TARGET:
				pTemp = (BombTarget *)AIActions[i].pAction;
				lTempSize = sizeof(BombTarget);
			break;
			case ACTION_CAP:
				pTemp = (CAPActionType *)AIActions[i].pAction;
				lTempSize = sizeof(CAPActionType);
			break;
			case ACTION_SOJ:
				pTemp = (SOJActionType *)AIActions[i].pAction;
				lTempSize = sizeof(SOJActionType);
			break;
			case ACTION_VIEW:
				pTemp = (ViewActionType *)AIActions[i].pAction;
				lTempSize = sizeof(ViewActionType);
			break;
			case ACTION_SAR:
				pTemp = (SARActionType *)AIActions[i].pAction;
				lTempSize = sizeof(SARActionType);
			break;
			case ACTION_ESCORT:
				pTemp = (Escort *)AIActions[i].pAction;
				lTempSize = sizeof(Escort);
			break;
			case ACTION_DAMAGE_THIS_OBJECT:
				pTemp = (DamageActionType *)AIActions[i].pAction;
				lTempSize = sizeof(DamageActionType);
			break;
			case ACTION_FORMON:
				pTemp = (FormOn *)AIActions[i].pAction;
				lTempSize = sizeof(FormOn);
			break;
			case ACTION_HOVER:
				pTemp = (HoverActionType *)AIActions[i].pAction;
				lTempSize = sizeof(HoverActionType);
			break;
			case ACTION_LAND:
				pTemp = (Landing *)AIActions[i].pAction;
				lTempSize = sizeof(Landing);
			break;
			case ACTION_MESSAGE:
				pTemp = (MessageActionType *)AIActions[i].pAction;
				lTempSize = sizeof(MessageActionType);
			break;
			case ACTION_CHANGE_FORMATION:
				pTemp = (FormationActionType *)AIActions[i].pAction;
				lTempSize = sizeof(FormationActionType);
			break;
			case ACTION_ALTERNATE_PATH:
				pTemp = (EventActionAltPathType *)AIActions[i].pAction;
				lTempSize = sizeof(EventActionAltPathType);
			break;
			case ACTION_REFUEL_PATTERN:
				pTemp = (TankerActionType *)AIActions[i].pAction;
				lTempSize = sizeof(TankerActionType);
			break;
			case ACTION_RELEASE_ESCORT:
				pTemp = (Escort *)AIActions[i].pAction;
				lTempSize = sizeof(Escort);
			break;
			case ACTION_RELEASE_FORMON:
				pTemp = (FormOn *)AIActions[i].pAction;
				lTempSize = sizeof(FormOn);
			break;
			case ACTION_TAKEOFF:
				pTemp = (TakeOffActionType *)AIActions[i].pAction;
				lTempSize = sizeof(TakeOffActionType);
			break;
			case ACTION_SEAD:
				pTemp = (SEADActionType *)AIActions[i].pAction;
				lTempSize = sizeof(SEADActionType);
			break;
			case ACTION_ALERT_INTERCEPT:
				pTemp = (AlertIntercept *)AIActions[i].pAction;
				lTempSize = sizeof(AlertIntercept);
			break;
			case ACTION_CAS:
				pTemp = (CASActionType *)AIActions[i].pAction;
				lTempSize = sizeof(CASActionType);
			break;
			case ACTION_FIGHTER_SWEEP:
				pTemp = (FighterSweepActionType *)AIActions[i].pAction;
				lTempSize = sizeof(FighterSweepActionType);
			break;
			case ACTION_ORBIT:
				pTemp = (OrbitActionType *)AIActions[i].pAction;
				lTempSize = sizeof(OrbitActionType);
			break;
			case ACTION_ASW:
				pTemp = (ASWActionType *)AIActions[i].pAction;
				lTempSize = sizeof(ASWActionType);
			break;
			case ACTION_RELEASE_CHUTES:
				pTemp = (ReleaseChutesActionType *)AIActions[i].pAction;
				lTempSize = sizeof(ReleaseChutesActionType);
			break;
			case ACTION_FAC:
				pTemp = (FACActionType *)AIActions[i].pAction;
				lTempSize = sizeof(FACActionType);
			break;
			case ACTION_ANTISHIP:
				pTemp = (AntiShipActionType *)AIActions[i].pAction;
				lTempSize = sizeof(AntiShipActionType);
			break;
			case ACTION_DRONE:
				pTemp = (DroneActionType *)AIActions[i].pAction;
				lTempSize = sizeof(DroneActionType);
			break;
			case ACTION_ATTACKTARGET:
				pTemp = (AttackTargetActionType *)AIActions[i].pAction;
				lTempSize = sizeof(AttackTargetActionType);
			break;
			case ACTION_SETTRIGGER:
				pTemp = (SetTriggerActionType *)AIActions[i].pAction;
				lTempSize = sizeof(SetTriggerActionType);
			break;
			case ACTION_REFUEL:
				pTemp = (RefuelActionType *)AIActions[i].pAction;
				lTempSize = sizeof(RefuelActionType);
			break;


			default:
				lTempSize=0;
			break;
		}

		if (lTempSize)
		{
			void *buffer = malloc(lTempSize);
			if (!buffer)
				LogError("Memory allocation failed",__FILE__,__LINE__,1);

			_read(hMissionFile,buffer,lTempSize);
			AIActions[i].pAction = (unsigned long)buffer;
		} else {
			AIActions[i].pAction = 0;
		}
	}

	_read(hMissionFile,&iNumGoals,sizeof(iNumGoals));
	sprintf(sDebugTxt,"        Number of Goals: %d",iNumGoals);
	DisplayInitText(sDebugTxt,1);

	_read(hMissionFile,GoalList,(sizeof(MBGoalInfo) * iNumGoals));

	_read(hMissionFile,&g_iAreaGoal,sizeof(g_iAreaGoal));
	_read(hMissionFile,&AreaGoal,sizeof(AreaGoalType));

	_read(hMissionFile,&g_iNumEvents,sizeof(g_iNumEvents));
	sprintf(sDebugTxt,"        Number of Events: %d",g_iNumEvents);
	DisplayInitText(sDebugTxt,1);


	TimeEventType *TimeEvent;
	AreaEventType *AreaEvent;
	GoalEventType *GoalEvent;
	ObjectEventType *ObjectEvent;
	MissionEndEventType *MissionEndEvent;
	EventActionMessageType *MessageAction;
	EventActionDamageType *DamageAction;
	EventActionAltPathType *AltPathAction;
	EventActionSetFlagType *SetFlagAction;
	EventActionFACType *FACAction;
	MovingAreaEventType *MovingAreaEvent;

	int j;

	for (i=0; i<g_iNumEvents; i++)
	{
		_read(hMissionFile,&(EventList[i]),sizeof(MBEvents));

		lTempSize=0;
		pTemp=NULL;

		switch(EventList[i].lEventID)
		{
			case EVENT_TIME:
				TimeEvent = (TimeEventType *)malloc(sizeof(TimeEventType));
				_read(hMissionFile,TimeEvent,sizeof(TimeEventType));
				EventList[i].pEvent = (unsigned long)TimeEvent;
				for (j=0; j<TimeEvent->iNumEventActions; j++)
				{
					if (TimeEvent->EventAction[j].pEvent)
					{
						switch(TimeEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_NONE:
							break;
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)malloc(sizeof(EventActionMessageType));
								_read(hMissionFile,MessageAction,sizeof(EventActionMessageType));
								TimeEvent->EventAction[j].pEvent=(unsigned long)MessageAction;
							break;
							case EVENT_ACTION_DAMAGE:
								DamageAction = (EventActionDamageType *)malloc(sizeof(EventActionDamageType));
								_read(hMissionFile,DamageAction,sizeof(EventActionDamageType));
								TimeEvent->EventAction[j].pEvent=(unsigned long)DamageAction;
							break;
							case EVENT_ACTION_ALTPATH:
								AltPathAction = (EventActionAltPathType *)malloc(sizeof(EventActionAltPathType));
								_read(hMissionFile,AltPathAction,sizeof(EventActionAltPathType));
								TimeEvent->EventAction[j].pEvent=(unsigned long)AltPathAction;
							break;
							case EVENT_ACTION_SETFLAG:
								SetFlagAction = (EventActionSetFlagType *)malloc(sizeof(EventActionSetFlagType));
								_read(hMissionFile,SetFlagAction,sizeof(EventActionSetFlagType));
								TimeEvent->EventAction[j].pEvent=(unsigned long)SetFlagAction;
							break;
							case EVENT_ACTION_FAC:
								FACAction = (EventActionFACType *)malloc(sizeof(EventActionFACType));
								_read(hMissionFile,FACAction,sizeof(EventActionFACType));
								TimeEvent->EventAction[j].pEvent=(unsigned long)FACAction;
							break;
						}
					}
				}
			break;
			case EVENT_AREA:
				AreaEvent = (AreaEventType *)malloc(sizeof(AreaEventType));
				_read(hMissionFile,AreaEvent,sizeof(AreaEventType));
				EventList[i].pEvent = (unsigned long)AreaEvent;
				for (j=0; j<AreaEvent->iNumEventActions; j++)
				{
					if (AreaEvent->EventAction[j].pEvent)
					{
						switch(AreaEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_NONE:
							break;
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)malloc(sizeof(EventActionMessageType));
								_read(hMissionFile,MessageAction,sizeof(EventActionMessageType));
								AreaEvent->EventAction[j].pEvent=(unsigned long)MessageAction;
							break;
							case EVENT_ACTION_DAMAGE:
								DamageAction = (EventActionDamageType *)malloc(sizeof(EventActionDamageType));
								_read(hMissionFile,DamageAction,sizeof(EventActionDamageType));
								AreaEvent->EventAction[j].pEvent=(unsigned long)DamageAction;
							break;
							case EVENT_ACTION_ALTPATH:
								AltPathAction = (EventActionAltPathType *)malloc(sizeof(EventActionAltPathType));
								_read(hMissionFile,AltPathAction,sizeof(EventActionAltPathType));
								AreaEvent->EventAction[j].pEvent=(unsigned long)AltPathAction;
							break;
							case EVENT_ACTION_SETFLAG:
								SetFlagAction = (EventActionSetFlagType *)malloc(sizeof(EventActionSetFlagType));
								_read(hMissionFile,SetFlagAction,sizeof(EventActionSetFlagType));
								AreaEvent->EventAction[j].pEvent=(unsigned long)SetFlagAction;
							break;
							case EVENT_ACTION_FAC:
								FACAction = (EventActionFACType *)malloc(sizeof(EventActionFACType));
								_read(hMissionFile,FACAction,sizeof(EventActionFACType));
								AreaEvent->EventAction[j].pEvent=(unsigned long)FACAction;
							break;
						}
					}
				}
			break;
			case EVENT_GOAL:
				GoalEvent = (GoalEventType *)malloc(sizeof(GoalEventType));
				_read(hMissionFile,GoalEvent,sizeof(GoalEventType));
				EventList[i].pEvent = (unsigned long)GoalEvent;
				for (j=0; j<GoalEvent->iNumEventActions; j++)
				{
					if (GoalEvent->EventAction[j].pEvent)
					{
						switch(GoalEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_NONE:
							break;
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)malloc(sizeof(EventActionMessageType));
								_read(hMissionFile,MessageAction,sizeof(EventActionMessageType));
								GoalEvent->EventAction[j].pEvent=(unsigned long)MessageAction;
							break;
							case EVENT_ACTION_DAMAGE:
								DamageAction = (EventActionDamageType *)malloc(sizeof(EventActionDamageType));
								_read(hMissionFile,DamageAction,sizeof(EventActionDamageType));
								GoalEvent->EventAction[j].pEvent=(unsigned long)DamageAction;
							break;
							case EVENT_ACTION_ALTPATH:
								AltPathAction = (EventActionAltPathType *)malloc(sizeof(EventActionAltPathType));
								_read(hMissionFile,AltPathAction,sizeof(EventActionAltPathType));
								GoalEvent->EventAction[j].pEvent=(unsigned long)AltPathAction;
							break;
							case EVENT_ACTION_SETFLAG:
								SetFlagAction = (EventActionSetFlagType *)malloc(sizeof(EventActionSetFlagType));
								_read(hMissionFile,SetFlagAction,sizeof(EventActionSetFlagType));
								GoalEvent->EventAction[j].pEvent=(unsigned long)SetFlagAction;
							break;
							case EVENT_ACTION_FAC:
								FACAction = (EventActionFACType *)malloc(sizeof(EventActionFACType));
								_read(hMissionFile,FACAction,sizeof(EventActionFACType));
								GoalEvent->EventAction[j].pEvent=(unsigned long)FACAction;
							break;
						}
					}
				}
			break;
			case EVENT_OBJECT:
				ObjectEvent = (ObjectEventType *)malloc(sizeof(ObjectEventType));
				_read(hMissionFile,ObjectEvent,sizeof(ObjectEventType));
				EventList[i].pEvent = (unsigned long)ObjectEvent;
				for (j=0; j<ObjectEvent->iNumEventActions; j++)
				{
					if (ObjectEvent->EventAction[j].pEvent)
					{
						switch(ObjectEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_NONE:
							break;
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)malloc(sizeof(EventActionMessageType));
								_read(hMissionFile,MessageAction,sizeof(EventActionMessageType));
								ObjectEvent->EventAction[j].pEvent=(unsigned long)MessageAction;
							break;
							case EVENT_ACTION_DAMAGE:
								DamageAction = (EventActionDamageType *)malloc(sizeof(EventActionDamageType));
								_read(hMissionFile,DamageAction,sizeof(EventActionDamageType));
								ObjectEvent->EventAction[j].pEvent=(unsigned long)DamageAction;
							break;
							case EVENT_ACTION_ALTPATH:
								AltPathAction = (EventActionAltPathType *)malloc(sizeof(EventActionAltPathType));
								_read(hMissionFile,AltPathAction,sizeof(EventActionAltPathType));
								ObjectEvent->EventAction[j].pEvent=(unsigned long)AltPathAction;
							break;
							case EVENT_ACTION_SETFLAG:
								SetFlagAction = (EventActionSetFlagType *)malloc(sizeof(EventActionSetFlagType));
								_read(hMissionFile,SetFlagAction,sizeof(EventActionSetFlagType));
								ObjectEvent->EventAction[j].pEvent=(unsigned long)SetFlagAction;
							break;
							case EVENT_ACTION_FAC:
								FACAction = (EventActionFACType *)malloc(sizeof(EventActionFACType));
								_read(hMissionFile,FACAction,sizeof(EventActionFACType));
								ObjectEvent->EventAction[j].pEvent=(unsigned long)FACAction;
							break;
						}
					}
				}
			break;
			case EVENT_MISSION_END:
			{
				MissionEndEvent = (MissionEndEventType *)malloc(sizeof(MissionEndEventType));
				_read(hMissionFile,MissionEndEvent,sizeof(MissionEndEventType));
				EventList[i].pEvent = (unsigned long)MissionEndEvent;
			}
			break;
			case EVENT_MOVING_AREA:
				MovingAreaEvent = (MovingAreaEventType *)malloc(sizeof(MovingAreaEventType));
				_read(hMissionFile,MovingAreaEvent,sizeof(MovingAreaEventType));
				EventList[i].pEvent = (unsigned long)MovingAreaEvent;
				for (j=0; j<MovingAreaEvent->iNumEventActions; j++)
				{
					if (MovingAreaEvent->EventAction[j].pEvent)
					{
						switch(MovingAreaEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_NONE:
							break;
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)malloc(sizeof(EventActionMessageType));
								_read(hMissionFile,MessageAction,sizeof(EventActionMessageType));
								MovingAreaEvent->EventAction[j].pEvent=(unsigned long)MessageAction;
							break;
							case EVENT_ACTION_DAMAGE:
								DamageAction = (EventActionDamageType *)malloc(sizeof(EventActionDamageType));
								_read(hMissionFile,DamageAction,sizeof(EventActionDamageType));
								MovingAreaEvent->EventAction[j].pEvent=(unsigned long)DamageAction;
							break;
							case EVENT_ACTION_ALTPATH:
								AltPathAction = (EventActionAltPathType *)malloc(sizeof(EventActionAltPathType));
								_read(hMissionFile,AltPathAction,sizeof(EventActionAltPathType));
								MovingAreaEvent->EventAction[j].pEvent=(unsigned long)AltPathAction;
							break;
							case EVENT_ACTION_SETFLAG:
								SetFlagAction = (EventActionSetFlagType *)malloc(sizeof(EventActionSetFlagType));
								_read(hMissionFile,SetFlagAction,sizeof(EventActionSetFlagType));
								MovingAreaEvent->EventAction[j].pEvent=(unsigned long)SetFlagAction;
							break;
							case EVENT_ACTION_FAC:
								FACAction = (EventActionFACType *)malloc(sizeof(EventActionFACType));
								_read(hMissionFile,FACAction,sizeof(EventActionFACType));
								MovingAreaEvent->EventAction[j].pEvent=(unsigned long)FACAction;
							break;
						}
					}
				}
			break;

			default:
			break;
		}
	}

	_read(hMissionFile,&BullsEye,sizeof(MPoint));
	_read(hMissionFile,&g_iNumJSTARObjects,sizeof(int));
	if (g_iNumJSTARObjects)
		_read(hMissionFile,&JSTARList,sizeof(JSTARType));
	_read(hMissionFile,&g_iNumTACANObjects,sizeof(int));
	if (g_iNumTACANObjects)
	{
		pTACANTypeList = (TACANType *)malloc(g_iNumTACANObjects * sizeof(TACANType));
		_read(hMissionFile,pTACANTypeList,(g_iNumTACANObjects * sizeof(TACANType)));
	}
	_read(hMissionFile,&g_iNumEyePoints,sizeof(int));
	_read(hMissionFile,EyePointList,(g_iNumEyePoints*sizeof(EyePointActionType)));
	_read(hMissionFile,&g_iNumGroups,sizeof(int));
	_read(hMissionFile,GroupList,(g_iNumGroups * sizeof(GroupType)));

	_read(hMissionFile,&DebriefData,sizeof(DebriefType));

	for (i=0; i<DebriefData.iNumDebriefGroups; i++)
	{
		DebriefData.pDebriefGroup[i]=(DebriefGroups *)malloc(sizeof(DebriefGroups));
		_read(hMissionFile,DebriefData.pDebriefGroup[i],sizeof(DebriefGroups));
		for (int j=0; j<DebriefData.pDebriefGroup[i]->iNumStatements; j++)
		{
			DebriefData.pDebriefGroup[i]->pDebriefStatement[j]=(DebriefStatement *)malloc(sizeof(DebriefStatement));
			_read(hMissionFile,DebriefData.pDebriefGroup[i]->pDebriefStatement[j],sizeof(DebriefStatement));
			if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize)
			{
				hMissionFile,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage=
					(char *)malloc(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize);
				_read(hMissionFile,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize);
			}
			for (int k=0; k<DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iNumItems; k++)
			{
				DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]=
					(DebriefStatementItem *)malloc(sizeof(DebriefStatementItem));
				_read(hMissionFile,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k],sizeof(DebriefStatementItem));
			}
		}
	}

	_read(hMissionFile,&FACObject,sizeof(FACType));
	_read(hMissionFile,&g_iNumMissionLabels,sizeof(int));

	if (g_iNumMissionLabels)
	{
		pMissionLabelList=(MissionLabelType *)malloc(g_iNumMissionLabels*sizeof(MissionLabelType));
		_read(hMissionFile,pMissionLabelList,(g_iNumMissionLabels*sizeof(MissionLabelType)));
	}

	_close(hMissionFile);

	int hFenceFile=-1;
	BuilderIdTagType FenceIdTag;

	g_iNumFenceArea=0;
	//  memset(FenceArea,0,(sizeof(FenceTypeV1)*5));  //  Should already exist.

	if ((hFenceFile=_open(RegPath("data","fence.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFenceFile,&FenceIdTag,sizeof(BuilderIdTagType));
		_read(hFenceFile,&g_iNumFenceArea,sizeof(int));
		_read(hFenceFile,FenceArea,(sizeof(FenceTypeV1)*g_iNumFenceArea));
		_close(hFenceFile);
	}

	g_lTimeMin = (MissionInfo.lMissionTime & 0x00FF);
	g_lTimeHour = ((MissionInfo.lMissionTime & 0xFF00) >> 8);

	i = MissionInfo.lMissionWeather;

	if (i == 4) //random;
	{
		i = rand() & 0xFF;

		if (i < (0xC0))
			i = rand()&1;
		else
			i = ((rand()&3)>>1)+2;
	}

	switch ( i )
	{
		case 0	: //Clear
			WorldParams.Weather = WR_VIS_NRM;
			WorldParams.CloudAlt = 10000 FEET + frand()*20000 FEET;
			WorldParams.Visibility = 1.0f;
			break;

		case 1:  //Partly cloudy Choose between scattered and broken
			WorldParams.Weather = WR_VIS_NRM;

			if (rand()&1)
			{
				WorldParams.Weather |= WR_FLATCLOUD_SCATTERED;
				WorldParams.Visibility = 1.0f;
			}
			else
			{
				WorldParams.Weather |= WR_FLATCLOUD_CLOUDY;
				WorldParams.Visibility = 0.9f + 0.1*frand();
			}

			WorldParams.CloudAlt = 10000 FEET + frand()*20000 FEET;

			WorldParams.Weather |= WR_PUFFCLOUD_SCATTERED;
			break;

		case 2: //Overcast
			WorldParams.Weather = WR_VIS_MED | WR_FLATCLOUD_OVERCAST | WR_PUFFCLOUD_SCATTERED;
			WorldParams.CloudAlt = 7000 FEET + frand()*6000 FEET;
			WorldParams.Visibility = 0.4040f + 0.3*frand();
			break;

		case 3: //Stormy
			WorldParams.Weather = WR_VIS_LOW | WR_FLATCLOUD_STORMY | WR_PUFFCLOUD_SCATTERED;
			WorldParams.CloudAlt = 4000 FEET + frand()*6000 FEET;
			if (rand()&7)
				WorldParams.Visibility = 0.304f + 0.2*frand();
			else
				WorldParams.Visibility = 0.17 - frand()*(1.0/10.0);
			break;

	}

	return(1);
}




//***********************************************************************************************************************************
long GetVersionInfo(char *sFileName,long *lSubVersion)
{
	int hMissionFile;
	MissionIdTagType FileIDTagType;

	if ((hMissionFile=_open(sFileName,_O_BINARY | _O_RDONLY)) == -1)
	{
		LogError("Could not open file",__FILE__,__LINE__,1);
		return(0);
	}

	_read(hMissionFile,&FileIDTagType,sizeof(MissionIdTagType));
	_close(hMissionFile);

	if (strcmp(FileIDTagType.sTag,"SMF")) return(-1);

	if (FileIDTagType.lMissionFormatIdTag!=FileIDTagType.lMissionFormatIdTag)
		return(-1);
	*lSubVersion = FileIDTagType.lSubVersion;
	return(FileIDTagType.lVersion);

}

//***********************************************************************************************************************************
int LoadMissionFile(char *sFileName)
{
	long lVersion=-1;
	long lSubVersion=-1;

	lVersion = GetVersionInfo(sFileName,&lSubVersion);
	if (lVersion==-1)
	{
		// Bad File
		return(0);
	}

	switch(lVersion)
	{
		case 1:
		    MessageBox(hwnd, "Sorry...Old Mission format versions not supported.", "Error", MB_OK);
			return(0);
		break;
		case 2:
			switch(lSubVersion)
			{
				case 0:
					MessageBox(hwnd, "Sorry...Old Mission format versions not supported.", "Error", MB_OK);
					return(0);
				break;
				case 1:
					LoadMissionVersion201(sFileName);
				break;
			}
		break;
	}
	TranslateMissionText(sFileName,0);
	return(1);
}

//**************************************************************************************
void InitAIMissionVariables()
{
	for(int cnt = 0; cnt < NUMGOALS; cnt ++)
	{
		cAIGoalSame[cnt] = 1;
	}

	for(cnt = 0; cnt < iNumGoals; cnt ++)
	{
		if(GoalList[cnt].lObjectType == 0)
		{
			cAIGoalSame[cnt] = AIObjects[GoalList[cnt].dwSerialNumber].iNumInFlight;
#if 0  //  Add if we have percentages
			cAIGoalSame[cnt] *= percentage of group;
			if(cAIGoalSame[cnt] < 1)
			{
				cAIGoalSame[cnt] = 1;
			}
#endif
		}
		else if(GoalList[cnt].lObjectType == MOVINGVEHICLE)
		{
			cAIGoalSame[cnt] = g_pMovingVehicleList[GoalList[cnt].dwSerialNumber].iNumGroup;
			if(cAIGoalSame[cnt] < 1)
			{
				cAIGoalSame[cnt] = 1;
			}
		}
		else
			cAIGoalSame[cnt] = 1;

	}

	for(cnt = 0; cnt < NUMEVENTS; cnt ++)
	{
		iAIEventDelay[cnt] = -1;
	}

	for(cnt = 0; cnt < 30; cnt ++)
	{
		iAIChangedEventFlags[cnt] = 0;
	}
}

int g_iCurrentLocationView=0;

extern void setup_no_cockpit_art();



//**************************************************************************************
int LoadLocationFile(char *szLocationFile)
{
	g_iCurrentLocationView=0;

#if 1

	int hMissionFile=-1;
	if ((hMissionFile=_open(szLocationFile,_O_BINARY | _O_RDONLY)) == -1)
	{
		return(0);
	}

	_read(hMissionFile,&g_iNumLocations,sizeof(int));
	g_pLocationList = (LocationData *)malloc(g_iNumLocations*sizeof(LocationData));
	_read(hMissionFile,g_pLocationList,(g_iNumLocations*sizeof(LocationData)));

	_close(hMissionFile);
#endif

#if 0
	g_iNumLocations=3;
	g_pLocationList = (LocationData *)malloc(3*sizeof(LocationData));

	g_pLocationList[0].H=0;
	g_pLocationList[0].P=-30;
	g_pLocationList[0].R=0;
	g_pLocationList[0].X=4275718*FTTOWU;
	g_pLocationList[0].Y=3000*FTTOWU;
	g_pLocationList[0].Z=5822602*FTTOWU;
	strcpy(g_pLocationList[0].sName,"Location 1");

	g_pLocationList[1].H=0;
	g_pLocationList[1].P=-30;
	g_pLocationList[1].R=0;
	g_pLocationList[1].X=6275718*FTTOWU;
	g_pLocationList[1].Y=3000*FTTOWU;
	g_pLocationList[1].Z=8022602*FTTOWU;
	strcpy(g_pLocationList[1].sName,"Location 2");

	g_pLocationList[2].H=0;
	g_pLocationList[2].P=-30;
	g_pLocationList[2].R=0;
	g_pLocationList[2].X=4275718*FTTOWU;
	g_pLocationList[2].Y=3000*FTTOWU;
	g_pLocationList[2].Z=7022602*FTTOWU;
	strcpy(g_pLocationList[2].sName,"Location 3");


	int iLocationFile=_open(szLocationFile,_O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _S_IWRITE | _S_IREAD);

	_write(iLocationFile,&g_iNumLocations,sizeof(int));
	_write(iLocationFile,g_pLocationList,(sizeof(LocationData)*g_iNumLocations));
	_close(iLocationFile);
#endif

	return(1);
}

//**************************************************************************************
void InitLocations()
{
	if (g_iLocationMode==LOCATION_MODE_EXTERNAL)
	{
		LoadLocationFile(g_szLocationFileName);
	}
}


//**************************************************************************************
void SetInitialLocationView()
{
	if (!g_iLocationMode) return;
	if (!g_iNumLocations) return;

	setup_no_cockpit_art();

	ChangeViewModeOrSubject(CAMERA_FREE);

	g_iCurrentLocationView = 0;

	Camera1.Heading = AIConvert180DegreeToAngle(g_pLocationList[g_iCurrentLocationView].H);
	Camera1.Pitch = AIConvert180DegreeToAngle(g_pLocationList[g_iCurrentLocationView].P);
	Camera1.Roll = AIConvert180DegreeToAngle(g_pLocationList[g_iCurrentLocationView].R);

	camera_setup_initial_relative_quats();

	Camera1.CameraLocation.X = g_pLocationList[g_iCurrentLocationView].X;
	Camera1.CameraLocation.Y = g_pLocationList[g_iCurrentLocationView].Y;
	Camera1.CameraLocation.Z = g_pLocationList[g_iCurrentLocationView].Z;

	camera_setup_initial_location(Camera1.CameraLocation);

	WorldParams.WorldTime=g_pLocationList[g_iCurrentLocationView].fWorldTime;

	SimPause=1;
}

//**************************************************************************************
void SetNextViewLocation()
{
	if (!g_iLocationMode) return;
	if (!g_iNumLocations) return;

	FPointDouble point;
	point.X = g_pLocationList[g_iCurrentLocationView].X;
	point.Y = g_pLocationList[g_iCurrentLocationView].Y;
	point.Z = g_pLocationList[g_iCurrentLocationView].Z;

	ChangeViewModeOrSubject(CAMERA_FREE);

	Camera1.Heading = AIConvert180DegreeToAngle(g_pLocationList[g_iCurrentLocationView].H);
	Camera1.Pitch = AIConvert180DegreeToAngle(g_pLocationList[g_iCurrentLocationView].P);
	Camera1.Roll = AIConvert180DegreeToAngle(g_pLocationList[g_iCurrentLocationView].R);

	camera_setup_initial_relative_quats();

	Camera1.CameraLocation.X = g_pLocationList[g_iCurrentLocationView].X;
	Camera1.CameraLocation.Y = g_pLocationList[g_iCurrentLocationView].Y;
	Camera1.CameraLocation.Z = g_pLocationList[g_iCurrentLocationView].Z;

	camera_setup_initial_location(Camera1.CameraLocation);

	WorldParams.WorldTime=g_pLocationList[g_iCurrentLocationView].fWorldTime;

	AICAddAIRadioMsgs(g_pLocationList[g_iCurrentLocationView].szName, -1);

	g_iCurrentLocationView++;
	if (g_iCurrentLocationView>=g_iNumLocations) g_iCurrentLocationView=0;
}


extern void InitLocations();



//**************************************************************************************
void CheckLocationMode()
{
	if (!g_iLocationMode) return;

	if (g_iLocationMode==LOCATION_MODE_EXTERNAL)
	{
		// Load Location File Data
		if (LoadLocationFile(g_szLocationFileName))
		{
			sprintf(sDebugTxt,"*************************  GAME RUNNING IN LOCATION MODE **********************************");
			DisplayInitText(sDebugTxt,1);

		}
	}
}

//**************************************************************************************
int InitWaypoints(void)
{
	for(int cnt = 0; cnt < MAX_PLANE_TYPES; cnt ++)
	{
		PlaneTypes[cnt].TypeNumber = -1;
	}

	sprintf(sDebugTxt,"AIInitGlobals");
	DisplayInitText(sDebugTxt,1);

	AIInitGlobals(1);

	sprintf(sDebugTxt,"Load Mission File");
	DisplayInitText(sDebugTxt,1);


	if (!LoadMissionFile(RegPath("resource", "mission.mmf")))
	{
		WriteDumpLog ("LoadMissionFile Failed! Exiting!\n");
		exit(-1);
	}

	sprintf(sDebugTxt,"Init Ground Sides");
	DisplayInitText(sDebugTxt,1);

	InitGroundSides();

	GDInitRuwaySides();  //  Needed for takeoff actions

	InitAIMissionVariables();

	// DataBase Stuff
	// This should only be called after LoadMissionFile is called
	// The AIObjects list MUST be initialized

	sprintf(sDebugTxt,"Evaluate Aircraft");
	DisplayInitText(sDebugTxt,1);
	EvaluateAircraft();



	sprintf(sDebugTxt,"LoadMovingVehicleInfo");
	DisplayInitText(sDebugTxt,1);
	LoadMovingVehicleInfo();

	sprintf(sDebugTxt,"IGGetPlanes");
	DisplayInitText(sDebugTxt,1);
	IGGetPlanes(iNumAIObjs);
	HandleRandomPlanes();
	AIInitFirstWayPt();

	InitMissionFlagStuff();


	WorldParams.WorldTime = g_lTimeHour HOURS;
	WorldParams.WorldTime += g_lTimeMin MINUTES;

	CheckLocationMode();

	return(1);
}


/* should go to XVARS.BLD whenever it's free */
extern GenericFamilyDef *WorldFamilyDefs;
extern GenericInstanceDef *WorldInstanceDefs;
extern GenericInstanceDef *MissionInstanceDefs;

extern "C" __declspec( dllexport ) GenericFamilyDef *ExpLoadFamilyFile(const char *world_path,const char *world_name);
extern "C" __declspec( dllexport ) GenericInstanceDef *ExpLoadWorldInstanceDefs(const char *world_path,const char *world_name);
extern "C" __declspec( dllexport ) int OpenMikeFile();

//*********************************************************************************************************************
GenericFamilyDef *ExpLoadFamilyFile(const char *world_path,const char *world_name)
{
	WorldFamilyDefs = LoadFamilyFile(world_path,world_name);
	return(WorldFamilyDefs);
}

//*********************************************************************************************************************
GenericInstanceDef *ExpLoadWorldInstanceDefs(const char *world_path,const char *world_name)
{
	WorldInstanceDefs = LoadWorldInstanceDefs(world_path,world_name);
	return(WorldInstanceDefs);
}

//*********************************************************************************************************************
void CleanUpMissionData()
{
	iNumAIObjs=0;
	memset(AIObjects,0,(sizeof(MBObject) * MAXAIOBJS));

	PlaneParams *P = &Planes[0];

	memset(Planes,0,(sizeof(PlaneParams)*MAX_PLANES));
	LastPlane=NULL;
	PlayerPlane=NULL;
	CurrentPlane=NULL;
	memset(PlaneTypes,0,(sizeof(PlaneType)*MAX_PLANE_TYPES));

	memset(Weapons,0,(sizeof(WeaponParams)*MAX_WEAPON_SLOTS));
	CurrentWeapon=NULL;
	LastWeapon=NULL;
	memset(WeaponTypes,0,(sizeof(WeaponType)*MAX_WEAPON_TYPES));

	memset(MovingVehicles,0,(sizeof(MovingVehicleParams)*MAX_MOVINGVEHICLE_SLOTS));

	CurrentMovingVehicle=NULL;
	LastMovingVehicle=NULL;

	memset(MovingVehicleTypeList,0,(sizeof(MovingVehicleType)*MAX_MOVINGVEHICLE_TYPES));


	iNumGroundObjects=0;

	if (pGroundObjectList)
	{
		free(pGroundObjectList);
		pGroundObjectList=NULL;
	}

	g_iNumTACANObjects=0;
	if (pTACANTypeList)
	{
		free(pTACANTypeList);
		pTACANTypeList=NULL;
	}

	g_iNumMovingVehicle=0;
	if (g_pMovingVehicleList)
	{
		free(g_pMovingVehicleList);
		g_pMovingVehicleList=NULL;
	}

	iNumWPPaths=0;
	memset(AIWPPaths,0,(sizeof(MBWPPaths) * MAXAIOBJS));

	iNumWayPts=0;
	memset(AIWayPoints,0,(sizeof(MBWayPoints) * MAXWPTS));

	iNumGoals=0;
	memset(GoalList,0,(sizeof(MBGoalInfo) * NUMGOALS));

	g_iAreaGoal=0;
	memset(&AreaGoal,0,sizeof(AreaGoalType));

	g_iNumEyePoints=0;
	memset(EyePointList,0,(sizeof(EyePointActionType) * MAX_NUM_EYEPOINTS));

	memset(&BullsEye,0,sizeof(MPoint));

	memset(&FACObject,0,sizeof(FACType));
	FACObject.iType=-1;

	memset(&JSTARList,0,sizeof(JSTARType));

	for (int i=0; i<MAX_NUM_JSTAR_OBJECTS; i++)
	{
		JSTARList.JSTARObjectList[i].iObjectType=-1;
	}

	if (pMissionLabelList)
	{
		free(pMissionLabelList);
		pMissionLabelList=NULL;
	}
	g_iNumMissionLabels=0;

	for (i=0; i<iNumActions; i++)
	{
		if ((AIActions[i].ActionID) && (AIActions[i].ActionID!=-1))
		{
			if (AIActions[i].pAction)
				free((void *)AIActions[i].pAction);
		}
	}

	iNumActions=0;
	memset(AIActions,0,(sizeof(MBActions) * MAXACTS));

	TimeEventType *TimeEvent;
	AreaEventType *AreaEvent;
	GoalEventType *GoalEvent;
	ObjectEventType *ObjectEvent;
	MovingAreaEventType *MovingAreaEvent;


	for (int iEventNum=0; iEventNum<g_iNumEvents; iEventNum++)
	{
		switch(EventList[iEventNum].lEventID)
		{
			case EVENT_TIME:
				TimeEvent = (TimeEventType *)EventList[iEventNum].pEvent;
				for (i=0; i<TimeEvent->iNumEventActions; i++)
				{
					if (TimeEvent->EventAction[i].pEvent)
					{
						free((void *)TimeEvent->EventAction[i].pEvent);
						TimeEvent->EventAction[i].pEvent=NULL;
					}
				}
			break;
			case EVENT_AREA:
				AreaEvent = (AreaEventType *)EventList[iEventNum].pEvent;
				for (i=0; i<AreaEvent->iNumEventActions; i++)
				{
					if (AreaEvent->EventAction[i].pEvent)
					{
						free((void *)AreaEvent->EventAction[i].pEvent);
						AreaEvent->EventAction[i].pEvent=NULL;
					}
				}
			break;
			case EVENT_GOAL:
				GoalEvent = (GoalEventType *)EventList[iEventNum].pEvent;
				for (i=0; i<GoalEvent->iNumEventActions; i++)
				{
					if (GoalEvent->EventAction[i].pEvent)
					{
						free((void *)GoalEvent->EventAction[i].pEvent);
						GoalEvent->EventAction[i].pEvent=NULL;
					}
				}
			break;
			case EVENT_OBJECT:
				ObjectEvent = (ObjectEventType *)EventList[iEventNum].pEvent;
				for (i=0; i<ObjectEvent->iNumEventActions; i++)
				{
					if (ObjectEvent->EventAction[i].pEvent)
					{
						free((void *)ObjectEvent->EventAction[i].pEvent);
						ObjectEvent->EventAction[i].pEvent=NULL;
					}
				}
			break;
			case EVENT_MISSION_END:
			break;
			case EVENT_MOVING_AREA:
				MovingAreaEvent = (MovingAreaEventType *)EventList[iEventNum].pEvent;
				for (i=0; i<MovingAreaEvent->iNumEventActions; i++)
				{
					if (MovingAreaEvent->EventAction[i].pEvent)
					{
						free((void *)MovingAreaEvent->EventAction[i].pEvent);
						MovingAreaEvent->EventAction[i].pEvent=NULL;
					}
				}
			break;


		}
		if (EventList[iEventNum].pEvent)
		{
			free((void *)EventList[iEventNum].pEvent);
			EventList[iEventNum].pEvent=NULL;
		}
		EventList[iEventNum].lEventID=0;
	}

	g_iNumEvents=0;
	memset(EventList,0,(sizeof(MBEvents) * NUMEVENTS));

	g_iNumEvents=0;
	memset(EventList,0,(sizeof(MBEvents) * NUMEVENTS));

	for (i=0; i<DebriefData.iNumDebriefGroups; i++)
	{
		for (int j=0; j<DebriefData.pDebriefGroup[i]->iNumStatements; j++)
		{
			if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize)
			{
				if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage)
				{
					free(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage);
					DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage=NULL;
				}
			}
			for (int k=0; k<DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iNumItems; k++)
			{
				if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k])
				{
					free(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]);
					DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]=NULL;
				}
			}
			if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j])
			{
				free(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]);
				DebriefData.pDebriefGroup[i]->pDebriefStatement[j]=NULL;
			}
		}
		if (DebriefData.pDebriefGroup[i])
		{
			free(DebriefData.pDebriefGroup[i]);
			DebriefData.pDebriefGroup[i]=NULL;
		}
	}
	DebriefData.iNumDebriefGroups=0;

	g_iNumAroundGoalObjectsHit=0;
	g_iNumWeaponReleasedInAreaGoal=0;

	g_iNumWeaponAreaGoal=0;
	g_iNumWeaponAreaGoalHit=0;

	return;
}

#if 0
	{ 0, "None" },
	{ 1, "Must Destroy" },
	{ 2, "Must Survive" },


	long lGoalRating;				//  Identifies how important a goal is 1= Primary, 2 = Secondary.
	long lGoalType;					//  Identifies type of goal (Must Protext, Must Destroy, ...)
	long lObjectType;				//  Aircraft, Moving Vehicle, Plugin, Fixed Object
	DWORD dwSerialNumber;			//  Used to be lObjectIndex
	char sLabel[LONGERNAMESIZE];

	GoalList[iNumGoals].lGoalRating=0xFF;
	GoalList[iNumGoals].lGoalType=0;
	GoalList[iNumGoals].lObjectType = 0;
	GoalList[iNumGoals].dwSerialNumber= 0;
#endif



//**************************************************************************************
void HandleRandomPlanes()
{
	int cnt;
	int cnt2;
	int cnt3;
	float randomroll;
	int foundpackage;
	int currrand;
	int vnum;
	int nummovers = LastMovingVehicle - MovingVehicles;

	if(MultiPlayer)
	{
		srand((unsigned int) Slotarray[0].dpid);
	}

	for(cnt = 0; cnt < iNumAIObjs; cnt ++)
	{
		if((AIObjects[cnt].iRandomGroup == -1) && (AIObjects[cnt].iRandomChance < 100))
		{
			randomroll = frand() * 100.0f;
			if(randomroll >= AIObjects[cnt].iRandomChance)
			{
				for(cnt2 = cnt; cnt2 < (cnt + AIObjects[cnt].iNumInFlight); cnt2 ++)
				{
					Planes[cnt2].Status = 0;
					Planes[cnt2].FlightStatus |= (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED);
					Planes[cnt2].FlightStatus |= (PL_OUT_OF_CONTROL|PL_STATUS_CRASHED);
					Planes[cnt2].WorldPosition.SetValues(1.0f,1.0f,1.0f);
				}
			}
			cnt += (AIObjects[cnt].iNumInFlight - 1);
		}
	}

	for(cnt = 0; cnt < g_iNumMovingVehicle; cnt ++)
	{
		if((g_pMovingVehicleList[cnt].iRandomGroup == -1) && (g_pMovingVehicleList[cnt].iRandomChance < 100))
		{
			randomroll = frand() * 100.0f;
			if(randomroll >= g_pMovingVehicleList[cnt].iRandomChance)
			{
				for(cnt2 = cnt; cnt2 < (cnt + g_pMovingVehicleList[cnt].iNumGroup); cnt2 ++)
				{
					MovingVehicles[cnt2].Status = 0;
				}
			}
			cnt += (g_pMovingVehicleList[cnt].iNumGroup - 1);
		}
	}

	for(cnt2 = 0; cnt2 < g_iNumGroups; cnt2 ++)
	{
		randomroll = frand() * 100.0f;
		if(randomroll == 100.0f)
		{
			randomroll = 99.0f;
		}
		currrand = 0;
		foundpackage = -1;
		for(cnt = 0; ((cnt < GroupList[cnt2].iNumPackage) && (foundpackage == -1)); cnt ++)
		{
			currrand += GroupList[cnt2].Package[cnt].iChance;
			if(currrand > randomroll)
			{
				foundpackage = cnt;
			}
		}

		for(cnt = 0; cnt < iNumAIObjs; cnt ++)
		{
			if((AIObjects[cnt].iRandomGroup == GroupList[cnt2].iID) && (foundpackage != AIObjects[cnt].iRandomElement))
			{
				Planes[cnt].Status = 0;
			}
		}

		for(cnt = 0; cnt < g_iNumMovingVehicle; cnt ++)
		{
			if((g_pMovingVehicleList[cnt].iRandomGroup == GroupList[cnt2].iID) && (foundpackage != g_pMovingVehicleList[cnt].iRandomElement))
			{
				vnum = 0;
				for(cnt3 = 0; cnt3 < cnt; cnt3 ++)
				{
					vnum += g_pMovingVehicleList[cnt3].iNumGroup;
				}

				if(vnum <= nummovers)
				{
					for(cnt3 = 0; cnt3 < g_pMovingVehicleList[cnt].iNumGroup; cnt3 ++)
					{
						MovingVehicles[vnum].Status = 0;
						vnum ++;
					}
				}
			}
		}
	}
}

#define LNG_MISSION_TITLE						1
#define LNG_MISSION_SUMMARY						2
#define LNG_MISSION_BRIEFING					3
#define LNG_MISSION_ACTION_MESSAGE				4
#define LNG_MISSION_EVENT_MESSAGE				5
#define LNG_MISSION_DEBRIEF_STATEMENT			6
#define LNG_MISSION_INTELLIGENCE_LABEL			7
#define	LNG_MISSION_INTELLIGENCE_TITLE_LABEL	8
#define LNG_MISSION_BRIEFING_SUMMARY			9
#define LNG_MISSION_MISSION_LABEl				10


TokenList *g_pForeignTokenList=NULL;
int g_iNumForeignTokens=0;

//*********************************************************************************************************************
int GetTokenDataExt(long lTokenID, long lVal1,long lVal2,long lVal3,long lVal4,int iLanguageNum,int *iIndex)
{
	for (int i=0; i<g_iNumForeignTokens; i++)
	{
		if ((g_pForeignTokenList[i].lTokenID == lTokenID) &&
			(g_pForeignTokenList[i].lLanguageID == (iLanguageNum+1))
			&& (g_pForeignTokenList[i].lVal1 == lVal1)
			&& (g_pForeignTokenList[i].lVal2 == lVal2)
			&& (g_pForeignTokenList[i].lVal3 == lVal3)
			&& (g_pForeignTokenList[i].lVal4 == lVal4))
		{
			*iIndex=i;
			return(g_pForeignTokenList[i].lTextSize);
		}
	}
	return(0);
}


//*********************************************************************************************************************
int GetTokenData(long lTokenID, int iLanguageNum,int *iIndex)
{
	for (int i=0; i<g_iNumForeignTokens; i++)
	{
		if ((g_pForeignTokenList[i].lTokenID == lTokenID) &&
			(g_pForeignTokenList[i].lLanguageID == (iLanguageNum+1)))
		{
			*iIndex=i;
			return(g_pForeignTokenList[i].lTextSize);
		}
	}
	return(0);
}

//*********************************************************************************************************************
int GetLanguageNumber()
{
	int iReturnValue=-1;	// English

	int iGermanId = MAKELANGID (LANG_GERMAN, SUBLANG_GERMAN);
	int iFrenchId = MAKELANGID (LANG_FRENCH, SUBLANG_FRENCH);
	int iSpanishId = MAKELANGID (LANG_SPANISH, SUBLANG_FRENCH);

	if (g_iLanguageId==iGermanId)
	{
		iReturnValue=0;
	}
	if (g_iLanguageId==iFrenchId)
	{
		iReturnValue=1;
	}
	if (g_iLanguageId==iSpanishId)
	{
		iReturnValue=2;
	}
	return(iReturnValue);
}


//*********************************************************************************************************************
void CopyLanguageText(int iMode)
{
	int iIndex=0;
	int iLanguageNum = GetLanguageNumber();
	int iSize;

#if 0
	int iSize = GetTokenData(MISSION_TITLE,iLanguageNum,&iIndex);
	if (iSize)
	{
		if (EditorInfo.psTitle)
		{
			free(EditorInfo.psTitle);
		}
		EditorInfo.psTitle = (char *)malloc(iSize);
		EditorInfo.lMissionTitleSize = iSize;
		strcpy(EditorInfo.psTitle,g_pForeignTokenList[iIndex].pText);
	}
	iSize = GetTokenData(MISSION_SUMMARY,iLanguageNum,&iIndex);
	if (iSize)
	{
		if (EditorInfo.psMissionSummary)
		{
			free(EditorInfo.psMissionSummary);
		}
		EditorInfo.psMissionSummary = (char *)malloc(iSize);
		EditorInfo.lMissionSummarySize = iSize;
		strcpy(EditorInfo.psMissionSummary,g_pForeignTokenList[iIndex].pText);
	}
	iSize = GetTokenData(MISSION_BRIEFING,iLanguageNum,&iIndex);
	if (iSize)
	{
		if (MissionInfo.psBriefing)
		{
			free(MissionInfo.psBriefing);
		}
		MissionInfo.psBriefing = (char *)malloc(iSize);
		MissionInfo.lBriefingSize = iSize;
		strcpy(MissionInfo.psBriefing,g_pForeignTokenList[iIndex].pText);
	}
	if (iMode) return;
#endif

	iSize=0;
	for (int i=0; i<iNumActions; i++)
	{
		switch(AIActions[i].ActionID)
		{
			case ACTION_MESSAGE:
			{
				iSize = GetTokenDataExt(LNG_MISSION_ACTION_MESSAGE,i,-1,-1,-1,iLanguageNum,&iIndex);
				if (iSize)
				{
					MessageActionType *pMessageAction = (MessageActionType *)AIActions[i].pAction;
					strncpy(pMessageAction->sMessage,g_pForeignTokenList[iIndex].pText,320);
				}
			}
			break;
		}
	}

#if 0
	int iPlaneNum=0;
	BOOL bValue=FALSE;

	iPlaneNum = GetPlayerIndex();

	BombTarget *pBombTargetAction;

	for (int waycnt=AIWPPaths[AIObjects[iPlaneNum].iWPPathNum].iStartWP; waycnt<AIWPPaths[AIObjects[iPlaneNum].iWPPathNum].iStartWP+AIWPPaths[AIObjects[iPlaneNum].iWPPathNum].iNumWP; waycnt++)
	{
		if (AIWayPoints[waycnt].iStartAct>=0)
		{
			for (int j=AIWayPoints[waycnt].iStartAct; j<(AIWayPoints[waycnt].iStartAct+AIWayPoints[waycnt].iNumActs); j++)
			{
				if (AIActions[j].ActionID==ACTION_BOMB_TARGET)
				{
					iSize = GetTokenDataExt(MISSION_INTELLIGENCE_TITLE_LABEL,i,-1,-1,-1,iLanguageNum,&iIndex);
					if (iSize)
					{
						pBombTargetAction = (BombTarget *)AIActions[j].pAction;
						strncpy(pBombTargetAction->IntelligenceArea.sLabel,g_pForeignTokenList[iIndex].pText,LONGERNAMESIZE);

						for (int k=0; k<pBombTargetAction->IntelligenceArea.iNumLabels; k++)
						{
							iSize = GetTokenDataExt(MISSION_INTELLIGENCE_LABEL,i,k,-1,-1,iLanguageNum,&iIndex);
							if (iSize)
							{
								strncpy(pBombTargetAction->IntelligenceArea.IntelLabels[k].sLabel,g_pForeignTokenList[iIndex].pText,NAMESIZE);
							}
						}
					}
				}
			}
		}
	}
#endif

	TimeEventType *TimeEvent;
	AreaEventType *AreaEvent;
	MovingAreaEventType *MovingAreaEvent;
	GoalEventType *GoalEvent;
	ObjectEventType *ObjectEvent;
	int j;
	EventActionMessageType *MessageAction;

	for (i=0; i<g_iNumEvents; i++)
	{
		switch(EventList[i].lEventID)
		{
			case EVENT_TIME:
				TimeEvent = (TimeEventType *)EventList[i].pEvent;
				for (j=0; j<TimeEvent->iNumEventActions; j++)
				{
					if (TimeEvent->EventAction[j].pEvent)
					{
						switch(TimeEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)TimeEvent->EventAction[j].pEvent;

								iSize = GetTokenDataExt(LNG_MISSION_EVENT_MESSAGE,i,j,-1,-1,iLanguageNum,&iIndex);
								if (iSize)
								{
									strncpy(MessageAction->sMessage,g_pForeignTokenList[iIndex].pText,320);
								}
							break;
						}
					}
				}
			break;
			case EVENT_AREA:
				AreaEvent = (AreaEventType *)EventList[i].pEvent;
				for (j=0; j<AreaEvent->iNumEventActions; j++)
				{
					if (AreaEvent->EventAction[j].pEvent)
					{
						switch(AreaEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)AreaEvent->EventAction[j].pEvent;

								iSize = GetTokenDataExt(LNG_MISSION_EVENT_MESSAGE,i,j,-1,-1,iLanguageNum,&iIndex);
								if (iSize)
								{
									strncpy(MessageAction->sMessage,g_pForeignTokenList[iIndex].pText,320);
								}
							break;
						}
					}
				}
			break;
			case EVENT_GOAL:
				GoalEvent = (GoalEventType *)EventList[i].pEvent;
				for (j=0; j<GoalEvent->iNumEventActions; j++)
				{
					if (GoalEvent->EventAction[j].pEvent)
					{
						switch(GoalEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)GoalEvent->EventAction[j].pEvent;

								iSize = GetTokenDataExt(LNG_MISSION_EVENT_MESSAGE,i,j,-1,-1,iLanguageNum,&iIndex);
								if (iSize)
								{
									strncpy(MessageAction->sMessage,g_pForeignTokenList[iIndex].pText,320);
								}
							break;
						}
					}
				}
			break;
			case EVENT_OBJECT:
				ObjectEvent = (ObjectEventType *)EventList[i].pEvent;
				for (j=0; j<ObjectEvent->iNumEventActions; j++)
				{
					if (ObjectEvent->EventAction[j].pEvent)
					{
						switch(ObjectEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)ObjectEvent->EventAction[j].pEvent;

								iSize = GetTokenDataExt(LNG_MISSION_EVENT_MESSAGE,i,j,-1,-1,iLanguageNum,&iIndex);
								if (iSize)
								{
									strncpy(MessageAction->sMessage,g_pForeignTokenList[iIndex].pText,320);
								}
							break;
						}
					}
				}
			break;
			case EVENT_MOVING_AREA:
			{
				MovingAreaEvent = (MovingAreaEventType *)EventList[i].pEvent;
				for (j=0; j<MovingAreaEvent->iNumEventActions; j++)
				{
					if (MovingAreaEvent->EventAction[j].pEvent)
					{
						switch(MovingAreaEvent->EventAction[j].lEventID)
						{
							case EVENT_ACTION_MESSAGE:
								MessageAction = (EventActionMessageType *)MovingAreaEvent->EventAction[j].pEvent;

								iSize = GetTokenDataExt(LNG_MISSION_EVENT_MESSAGE,i,j,-1,-1,iLanguageNum,&iIndex);
								if (iSize)
								{
									strncpy(MessageAction->sMessage,g_pForeignTokenList[iIndex].pText,320);
								}
							break;
						}
					}
				}
			}
			case EVENT_MISSION_END:
			break;
			default:
			break;
		}
	}

	for (i=0; i<DebriefData.iNumDebriefGroups; i++)
	{
		for (int j=0; j<DebriefData.pDebriefGroup[i]->iNumStatements; j++)
		{
			if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage)
			{
				iSize = GetTokenDataExt(LNG_MISSION_DEBRIEF_STATEMENT,i,j,-1,-1,iLanguageNum,&iIndex);
				if (iSize)
				{
					if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage)
					{
						free(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage);
					}
					DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage=(char *)malloc(iSize);
					DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize=iSize;
					strcpy(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage,g_pForeignTokenList[iIndex].pText);
				}
			}
		}
	}

}

//***********************************************************************************************************************************
void TranslateMissionText(char *szFileName,int iMode)
{
	// Things we need to do
	// First we will look in the mission directory
	int hLanguageFile=-1;

	char szFilePath[260];
	char szLanguageFile[260];
	char *pTemp;
	int iFoundIt=0;

	int iLanguageNum = GetLanguageNumber();
	if (iLanguageNum==-1) return;

	char *pszPath = GetRegValue ("mission");
	strcpy(szFilePath,pszPath);
	strcat(szFilePath,"\\");
	strcat(szFilePath,szFileName);

  	pTemp = strrchr (szFileName, '\\');
	if (pTemp)
	{
		pTemp++;
		strcpy (szLanguageFile, pTemp);
		pTemp = strchr (szLanguageFile, '.');
		if (pTemp)
			strcpy (pTemp, ".lng");
		else
			strcat (szLanguageFile, ".lng");
	} else {
		strcpy (szLanguageFile,szFileName);
		pTemp = strchr (szLanguageFile, '.');
		if (pTemp)
			strcpy (pTemp, ".lng");
		else
			strcat (szLanguageFile, ".lng");
	}

	strcpy(szFilePath,pszPath);
	strcat(szFilePath,"\\");
	strcat(szFilePath,szLanguageFile);

	if ((hLanguageFile=_open(szFilePath,_O_BINARY | _O_RDONLY)) != -1)
	{
		// We found a language file in the mission directory
		iFoundIt=1;
	}


	if (!iFoundIt)
	{
		// We did not find it in the mission directory, so
		// lets check in the language directory
		pszPath=NULL;
		pszPath = GetRegValue ("Translations");
		if (pszPath)
		{
  			pTemp = strrchr (szFileName, '\\');
			if (pTemp)
			{
				pTemp++;
				strcpy (szLanguageFile, pTemp);
				pTemp = strchr (szLanguageFile, '.');
				if (pTemp)
					strcpy (pTemp, ".lng");
				else
					strcat (szLanguageFile, ".lng");
			} else {
				strcpy (szLanguageFile,szFileName);
				pTemp = strchr (szLanguageFile, '.');
				if (pTemp)
					strcpy (pTemp, ".lng");
				else
					strcat (szLanguageFile, ".lng");
			}

			strcpy(szFilePath,pszPath);
			strcat(szFilePath,"\\");
			strcat(szFilePath,szLanguageFile);

			if ((hLanguageFile=_open(szFilePath,_O_BINARY | _O_RDONLY)) != -1)
			{
				// We found a language file in the mission directory
				iFoundIt=1;
			}
		}
	}


	if (iFoundIt)
	{
		// Okay, we found the language file
		// we can now just read from it since its already been opened
		// for us.
		MissionIdTagType LanguageIDTag;

		_read(hLanguageFile,&LanguageIDTag,sizeof(MissionIdTagType));

		if (strcmp(LanguageIDTag.sTag,"LNG")) return;

		char sTmpName[260];
		_read(hLanguageFile,sTmpName,260);

		_read(hLanguageFile,&g_iNumForeignTokens,sizeof(int));

		g_pForeignTokenList = (TokenList *)malloc(g_iNumForeignTokens * sizeof(TokenList));

		for (int i=0; i<g_iNumForeignTokens; i++)
		{
			_read(hLanguageFile,&g_pForeignTokenList[i],sizeof(TokenList));
			if (g_pForeignTokenList[i].lTextSize)
			{
				g_pForeignTokenList[i].pText=(char *)malloc(g_pForeignTokenList[i].lTextSize);
				_read(hLanguageFile,g_pForeignTokenList[i].pText,g_pForeignTokenList[i].lTextSize);
			}
		}
		_close(hLanguageFile);
		CopyLanguageText(iMode);

		for (i=0; i<g_iNumForeignTokens; i++)
		{
			if (g_pForeignTokenList[i].pText)
			{
				free(g_pForeignTokenList[i].pText);
				g_pForeignTokenList[i].pText=NULL;
			}
		}
		if (g_pForeignTokenList)
		{
			free(g_pForeignTokenList);
			g_pForeignTokenList=NULL;
			g_iNumForeignTokens=0;
		}
	}
}


#if 0
		AddTokenToList(MISSION_TITLE,-1,-1,-1,-1);
		AddTokenToList(MISSION_SUMMARY,-1,-1,-1,-1);
		AddTokenToList(MISSION_BRIEFING,-1,-1,-1,-1);

		for (int i=0; i<iNumActions; i++)
		{
			switch(AIActions[i].ActionID)
			{
				case ACTION_MESSAGE:
				{
					AddTokenToList(MISSION_ACTION_MESSAGE,i,-1,-1,-1);
				}
				break;
			}
		}

		int iPlaneNum=0;
		BOOL bValue=FALSE;

		iPlaneNum = GetPlayerIndex();

		for (int waycnt=AIWPPaths[AIObjects[iPlaneNum].iWPPathNum].iStartWP; waycnt<AIWPPaths[AIObjects[iPlaneNum].iWPPathNum].iStartWP+AIWPPaths[AIObjects[iPlaneNum].iWPPathNum].iNumWP; waycnt++)
		{
			if (AIWayPoints[waycnt].iStartAct>=0)
			{
				for (int j=AIWayPoints[waycnt].iStartAct; j<(AIWayPoints[waycnt].iStartAct+AIWayPoints[waycnt].iNumActs); j++)
				{
					if (AIActions[j].ActionID==ACTION_BOMB_TARGET)
					{
						BombTarget *pBombTargetAction = (BombTarget *)AIActions[j].pAction;
						AddTokenToList(MISSION_INTELLIGENCE_TITLE_LABEL,iPlaneNum,j,-1,-1);

						for (int k=0; k<pBombTargetAction->IntelligenceArea.iNumLabels; k++)
						{
							AddTokenToList(MISSION_INTELLIGENCE_LABEL,iPlaneNum,j,k,-1);
						}
					}
				}
			}
		}


		TimeEventType *TimeEvent;
		AreaEventType *AreaEvent;
		GoalEventType *GoalEvent;
		ObjectEventType *ObjectEvent;
		int j;

		for (i=0; i<g_iNumEvents; i++)
		{
			switch(EventList[i].lEventID)
			{
				case EVENT_TIME:
					TimeEvent = (TimeEventType *)EventList[i].pEvent;
					for (j=0; j<TimeEvent->iNumEventActions; j++)
					{
						if (TimeEvent->EventAction[j].pEvent)
						{
							switch(TimeEvent->EventAction[j].lEventID)
							{
								case EVENT_ACTION_MESSAGE:
									AddTokenToList(MISSION_EVENT_MESSAGE,i,j,-1,-1);
								break;
							}
						}
					}
				break;
				case EVENT_AREA:
					AreaEvent = (AreaEventType *)EventList[i].pEvent;
					for (j=0; j<AreaEvent->iNumEventActions; j++)
					{
						if (AreaEvent->EventAction[j].pEvent)
						{
							switch(AreaEvent->EventAction[j].lEventID)
							{
								case EVENT_ACTION_MESSAGE:
									AddTokenToList(MISSION_EVENT_MESSAGE,i,j,-1,-1);
								break;
							}
						}
					}
				break;
				case EVENT_GOAL:
					GoalEvent = (GoalEventType *)EventList[i].pEvent;
					for (j=0; j<GoalEvent->iNumEventActions; j++)
					{
						if (GoalEvent->EventAction[j].pEvent)
						{
							switch(GoalEvent->EventAction[j].lEventID)
							{
								case EVENT_ACTION_MESSAGE:
									AddTokenToList(MISSION_EVENT_MESSAGE,i,j,-1,-1);
								break;
							}
						}
					}
				break;
				case EVENT_OBJECT:
					ObjectEvent = (ObjectEventType *)EventList[i].pEvent;
					for (j=0; j<ObjectEvent->iNumEventActions; j++)
					{
						if (ObjectEvent->EventAction[j].pEvent)
						{
							switch(ObjectEvent->EventAction[j].lEventID)
							{
								case EVENT_ACTION_MESSAGE:
									AddTokenToList(MISSION_EVENT_MESSAGE,i,j,-1,-1);
								break;
							}
						}
					}
				break;
				default:
				break;
			}
		}

		for (i=0; i<DebriefData.iNumDebriefGroups; i++)
		{
			for (int j=0; j<DebriefData.pDebriefGroup[i]->iNumStatements; j++)
			{
				if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage)
				{
					AddTokenToList(MISSION_DEBRIEF_STATEMENT,i,j,-1,-1);
				}
			}
		}
	}
#endif


//*********************************************************************************************************************
int AreaGoalExists(WeaponParams *W)
{
	if (!g_iAreaGoal) return(0);

	FPointDouble UL;
	FPointDouble LR;

	UL.X = ConvertWayLoc(AreaGoal.x0);
	UL.Z = ConvertWayLoc(AreaGoal.z0);
	LR.X = ConvertWayLoc(AreaGoal.x1);
	LR.Z = ConvertWayLoc(AreaGoal.z1);

	if ((W->Pos.X>UL.X) && (W->Pos.X<LR.X) && (W->Pos.Z>UL.Z) && (W->Pos.Z<LR.Z))
	{
		return(1);
	}
	return(0);
}

//*********************************************************************************************************************
int WeaponAreaGoalExists(WeaponParams *W)
{
	int iWeaponType=-1;

	if (MissionInfo.lWeaponType==-1) return(0);

	if ((int)W->Type < 0x200)
		iWeaponType=(int)W->Type;
	else
		iWeaponType=(int)W->Type->TypeNumber;

	if (iWeaponType!=MissionInfo.lWeaponType) return(0);

	FPointDouble UL;
	FPointDouble LR;

	UL.X = ConvertWayLoc(MissionInfo.lWeaponGoalAreaX0);
	UL.Z = ConvertWayLoc(MissionInfo.lWeaponGoalAreaZ0);
	LR.X = ConvertWayLoc(MissionInfo.lWeaponGoalAreaX1);
	LR.Z = ConvertWayLoc(MissionInfo.lWeaponGoalAreaZ1);

	if ((W->Pos.X>UL.X) && (W->Pos.X<LR.X) && (W->Pos.Z>UL.Z) && (W->Pos.Z<LR.Z))
	{
		g_iNumWeaponAreaGoalHit++;
		return(1);
	} else {
		g_iNumWeaponAreaGoal++;
		return(0);
	}
}




extern BasicInstance *FindInstance(BasicInstance *first_instance,DWORD number);

//*********************************************************************************************************************
void InitGroundSides()
{
	int cnt;
	GroundObject *pgroundobj=NULL;
	long sideval;
	BasicInstance *walker;

	BasicInstance *checkobject;
	int friendly;

	if(g_nMissionType != SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
	{
		checkobject = walker = AllInstances;
		while(checkobject)
		{
			if(!(checkobject->SerialNumber & SN_PLUG_IN_MASK))
			{
				if(!AICCheckAllFences((checkobject->Position.X * WUTOFT), (checkobject->Position.Z * WUTOFT)))
				{
					checkobject->Flags |= BI_FRIENDLY;
					friendly = 1;
				}
				else
				{
					friendly = 0;
				}
				walker = checkobject;
				checkobject = (BasicInstance *)checkobject->NextInstance;
				while(checkobject)
				{
					if((fabs(checkobject->Position.X - walker->Position.X) > (10.0f * NMTOWU)) || (fabs(checkobject->Position.Z - walker->Position.Z) > (10.0f * NMTOWU)))
					{
						break;
					}
					else if(!(checkobject->SerialNumber & SN_PLUG_IN_MASK))
					{
						if(friendly)
						{
							checkobject->Flags |= BI_FRIENDLY;
						}
					}
					else
					{
						break;
					}
					checkobject = checkobject->NextInstance;
				}
			}
			else
			{
				checkobject = (BasicInstance *)checkobject->NextInstance;
			}
		}
	}

	if (iNumGroundObjects)
	{
		pgroundobj = pGroundObjectList;
		for(cnt = 0; cnt < iNumGroundObjects; cnt ++)
		{
			if(g_nMissionType == SIM_QUICK) // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
			{
				sideval = 0;
			}
			else if(pgroundobj->iSide == AI_FRIENDLY)
			{
				sideval = BI_FRIENDLY;
			}
			else if(pgroundobj->iSide == AI_NEUTRAL)
			{
				sideval = BI_NEUTRAL;
			}
			else
			{
				sideval = 0;
			}

			walker = FindInstance(AllInstances, pgroundobj->dwSerialNumber);
			if(walker)
			{
				if(sideval)
				{
					walker->Flags |= sideval;
				}
				else
				{
					walker->Flags &= ~(BI_FRIENDLY|BI_NEUTRAL);;
				}
				if(pgroundobj->dwSerialNumber & SN_PLUG_IN_INC)
				{
					walker = (BasicInstance *)walker->NextInstance;
					while((walker->SerialNumber & SN_PLUG_IN_MASK) == (pgroundobj->dwSerialNumber & SN_PLUG_IN_MASK))
					{
						if(sideval)
						{
							walker->Flags |= sideval;
						}
						else
						{
							walker->Flags &= ~(BI_FRIENDLY|BI_NEUTRAL);;
						}
						walker = (BasicInstance *)walker->NextInstance;
					}
				}
			}
			else if(pgroundobj->dwSerialNumber & SN_PLUG_IN_INC)
			{
				walker = AllInstances;
				while (walker && (walker->SerialNumber < pgroundobj->dwSerialNumber))
				{
					walker = (BasicInstance *)walker->NextInstance;
				}
				if(walker)
				{
					while(walker && ((walker->SerialNumber & SN_PLUG_IN_MASK) == (pgroundobj->dwSerialNumber & SN_PLUG_IN_MASK)))
					{
						if(sideval)
						{
							walker->Flags |= sideval;
						}
						else
						{
							walker->Flags &= ~(BI_FRIENDLY|BI_NEUTRAL);;
						}
//							tempval = GDConvertGrndSide(walker);
						walker = (BasicInstance *)walker->NextInstance;
					}
				}
			}
			pgroundobj ++;
		}
	}
}