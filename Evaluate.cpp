//*****************************************************************************************
// Evaluate.cpp - MJM 6/11/99
//*****************************************************************************************
#include "F18.h"
#include "resources.h"
#include "MultiDefs.h"
#include "MSAPI.h"
#include "SkunkNet.h"
#include "time.h"
#include "particle.h"
#include "MultiSetupDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_NUM_MISSION_FLAGS	100

//---------------------------------------------------------------------------------------------------------------------
MissionEventFlag MissionEventFlagList[MAX_NUM_MISSION_FLAGS];

struct stCampaignUsage
{
	int iUsageType;
	int iUsageSubType;
	int iCount;
};
typedef struct stCampaignUsage CampaignUsageType;

long	lCampaignGoalList[32];
long	lCampaignGoalCount=0;
CampaignUsageType *pCampaignUsageList=NULL;
int		g_iNumUsage=0;
int		g_iNumMissionFlags=0;
int		g_iNumAroundGoalObjectsHit=0;
int		g_iNumWeaponReleasedInAreaGoal=0;
int		g_iNumWeaponAreaGoal=0;
int		g_iNumWeaponAreaGoalHit=0;



//---------------------------------------------------------------------------------------------------------------------
void	EvaluateGroundObjects();
void	InitCampaignUsageFile();
int		GetFlightFlagStatus(int iMissionFlagId,int iFlight);
void	AddUsageToCampaignList(int iUsageType, int iWeaponID, int iCount);
void	WriteCampaignUsageFile();
void	CleanUpCampaignFile();
BOOL	GetGoalEvaluation(int iGoalNum);
int		GetMissionStatistic(int iAircraftIndex,int iType);
void	WriteCampaignGoalList();
BOOL	WriteMultiPlayerKillBoardFile (void);
BOOL	InstanceHasTempObject(BasicInstance *instance,DamageTypeExt *DamageTmp);

int GetFlightGroupStatistic(int iFlightGroup,int iType);

//---------------------------------------------------------------------------------------------------------------------
extern int g_nMissionType;
extern void ReturnAircrewToInventory(RESOURCEFILE *pResFile, int nSrcBay);
extern JOGCPLAYER	g_jogcPlayer;
extern MissionStatisticsType *MissionStatistics;

extern int VGetVGListNumFromResourceID(int);

void EvaluateVehiclesResources();

extern int g_iWrapperMode;

extern void LogMissionStatisticForAll(int iType, int iValue, int iOperator);

extern void UpdateRatiosForAll();

//*********************************************************************************************************************
int GetVariableStatus(int iVarID)
{
	for (int i=0; i<30; i++)
	{
		if (iAIChangedEventFlags[i]==(iVarID+USER_FLAG_START))
			return(1);
	}
	return(0);
}

extern void ReturnAvailableAirframe (RESOURCEFILE *pRes, int nID);

//*********************************************************************************************************************
void EvaluateMission()
{
	char szFullPath[260];
	int hDebriefFile=-1;
	char sTextLine[256];
	lCampaignGoalCount=0;
	PlaneParams *P;
	P=PlayerPlane;
	RESOURCEFILE resFile;
	RESOURCEFILE TempResFile;
	BOOL			bRemoveCrew;

	LogMissionStatisticForAll(LOG_FLIGHT_TOTAL_MISSION_TIME,GameLoop,0);
	UpdateRatiosForAll();

	memset(&resFile,0,sizeof(RESOURCEFILE));
	memset(&TempResFile,0,sizeof(RESOURCEFILE));

	HANDLE hfile = LoadResourceFile(g_szResourceFile, &resFile);

	if (hfile<0)
	{
		LPVOID lpMsgBuf;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);

		// Display the string.
		//MessageBox( NULL, (char *)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

		OutputDebugString((char *)lpMsgBuf);

		// Free the buffer.
		LocalFree( lpMsgBuf );

		OutputDebugString(g_szResourceFile);
		OutputDebugString("Resource File Load Failed During Debrief - Tell Mike\n");
	}
	memcpy(&TempResFile,&resFile,sizeof(RESOURCEFILE));

	if ((g_nMissionType==SIM_CAMPAIGN) || (g_nMissionType==SIM_SINGLE)
			|| (g_nMissionType==SIM_TRAIN))
	{
		if (g_nMissionType==SIM_CAMPAIGN)
		{
			EvaluateGroundObjects();
		}

		int iStartingWeaponID=0;
		int iStartingWeaponCount=0;
		int iEndingWeaponID=0;
		int iEndingWeaponCount=0;
		int iNumLost=0;

		InitCampaignUsageFile();

		while ((P->AI.iAIFlags1&AIPLAYERGROUP) || (P==PlayerPlane))
		{
			int iNumFlight = (P->AI.iAIFlags1&AIFLIGHTNUMS); // 0 - 7
			iStartingWeaponID=0;
			iStartingWeaponCount=0;
			iEndingWeaponID=0;
			iEndingWeaponCount=0;
			iNumLost=0;

			if ((P->FlightStatus & PL_STATUS_CRASHED) || (P->FlightStatus & PL_OUT_OF_CONTROL)
				|| (GetMissionStatistic((P-Planes),LOG_FLIGHT_EJECTED_IN_FRIENDLY))
				|| (GetMissionStatistic((P-Planes),LOG_FLIGHT_EJECTED_IN_ENEMY)))
			{
				// Ok, you died, so I you obviously will not return
				// anything that I loaned you.

				if(resFile.ActiveAircraft[iNumFlight].AirFrame.nID != EMPTY_SLOT)
				{
					for (int cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
					{
						iStartingWeaponID = resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponId;
						iStartingWeaponCount = resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponCnt;
						// We lost iStartingCount of iStartingWeaponID
						// We have 0 to put back into the stores

						AddUsageToCampaignList(2,iStartingWeaponID,iStartingWeaponCount);	// Weapon

						resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponId=0;
						resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponCnt=0;
					}
					ModifyAirframeStatus(resFile.ActiveAircraft[iNumFlight].AirFrame.nID,
						AFSTATUS_DESTROYED, &resFile);
					AddUsageToCampaignList(1,0,1);	// Aircraft

//					LogMissionStatistic(LOG_FLIGHT_DESTROYED,(iNumFlight+1),1,2);

   					if (GetMissionStatistic((P-Planes),LOG_FLIGHT_FLIGHTCREW_KIA))
					{
// Mike - This function no longer exists...you need to track dead pilot GUIDS in your campaign
//						ModifyFlightCrewStatus(resFile.ActiveAircraft[iNumFlight].FlightCrew.nID,
//							FCSTATUS_KIA, &resFile);
						//AddUsageToCampaignList(1,0,1);	// Flight Crew
					} else 	{
						if (GetMissionStatistic((P-Planes),LOG_FLIGHT_FLIGHTCREW_MIA))
						{
// Mike - This function no longer exists...you need to track MIA GUIDS in your campaign
//							ModifyFlightCrewStatus(resFile.ActiveAircraft[iNumFlight].FlightCrew.nID,
//								FCSTATUS_MIA, &resFile);
						  //	AddUsageToCampaignList(1,0,1);	// Flight Crew
						} else {
							LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_SURVIVED,(P-Planes),1,0);

							ReturnAircrewToInventory(&resFile,iNumFlight);
						}
					}
				}
			} else {
				// Ok, your alive so lets if you have returned what I
				// have loaned you.
				if(resFile.ActiveAircraft[iNumFlight].AirFrame.nID != EMPTY_SLOT)
				{
					for (int cnt=0; cnt<MAX_F18E_STATIONS; cnt++)
					{
						iStartingWeaponID = resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponId;
						iStartingWeaponCount = resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponCnt;
						iEndingWeaponID = P->WeapLoad[cnt].WeapId;
						iEndingWeaponCount = P->WeapLoad[cnt].Count;
						if (iStartingWeaponID!=iEndingWeaponID)
						{
							OutputDebugString("We have a problem here - loadmiss\n");
							// We have a problem where
						} else {
							iNumLost = iStartingWeaponCount-iEndingWeaponCount;

							// We lost iNumLost of iStartingWeaponId
							AddUsageToCampaignList(2,iStartingWeaponID,iNumLost);	// Weapon
							// We have iEndingWeaponCount to put back into the stores
							if (resFile.anInventory[iStartingWeaponID]!=-1)
								resFile.anInventory[iStartingWeaponID] += iEndingWeaponCount;
						}
						resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponId=0;
						resFile.ActiveAircraft[iNumFlight].Loadout.Station[cnt].iWeaponCnt=0;
					}

					bRemoveCrew = FALSE;

   					if (GetMissionStatistic((P-Planes),LOG_FLIGHT_DAMAGED))
					{
						ModifyAirframeStatus(resFile.ActiveAircraft[iNumFlight].AirFrame.nID,
							AFSTATUS_DAMAGED, &resFile);
						AddUsageToCampaignList(4,0,1);	// Damaged Aircraft
						bRemoveCrew = TRUE;
					}
   					if (GetMissionStatistic((P-Planes),LOG_FLIGHT_FLIGHTCREW_KIA))
					{
						ModifyAirframeStatus(resFile.ActiveAircraft[iNumFlight].AirFrame.nID,
							AFSTATUS_DESTROYED, &resFile);
						AddUsageToCampaignList(1,0,1);	// Aircraft
					}
					else
					{
						if (GetMissionStatistic((P-Planes),LOG_FLIGHT_FLIGHTCREW_KIA))
						{
							ModifyAirframeStatus(resFile.ActiveAircraft[iNumFlight].AirFrame.nID,
								AFSTATUS_DESTROYED, &resFile);
							AddUsageToCampaignList(1,0,1);	// Aircraft
						}
						else
						{
							LogMissionStatistic(LOG_FLIGHT_FLIGHTCREW_SURVIVED,(P-Planes),1,0);

							if (bRemoveCrew)
								ReturnAircrewToInventory(&resFile,iNumFlight);
						}
					}
				}
			}
			P++;
		}

		WriteResourceFile(hfile,&resFile);

		WriteCampaignUsageFile();
		CleanUpCampaignFile();
	}

	CloseResourceFile(hfile);


	char *pszPath = GetRegValue("resource");
	sprintf (szFullPath, "%s\\mission.mdf", pszPath);

	char szSummaryFullPath[260];

	pszPath = GetRegValue("resource");
	sprintf (szSummaryFullPath, "%s\\mission.mds", pszPath);

	// Need to process mission variables
	// Need to process resources
	// Need to process goals
	int hDebriefSummaryFile=_open(szSummaryFullPath,_O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _S_IWRITE | _S_IREAD);

	if ((hDebriefFile=_open(szFullPath,_O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _S_IWRITE | _S_IREAD)) != -1)
	{
#if 0
		for(int cnt = 0; cnt < iNumGoals; cnt++)
		{
			if (cAIGoalSame[cnt])
			{
				if (GoalList[cnt].lGoalType==1)
				{
					// Goal is FALSE, thing not destroyed
					if (!GoalList[cnt].lObjectType)
						sprintf(sTextLine,"Goal %d FALSE, Aircraft %s Not Destroyed.\r\r\n",cnt,AIObjects[GoalList[cnt].dwSerialNumber].sName);
					else
						sprintf(sTextLine,"Goal %d FALSE, Ground Object Not Destroyed.\r\r\n",cnt);
				}
				if (GoalList[cnt].lGoalType==2)
				{
					// Goal is True, thing survived
					if (!GoalList[cnt].lObjectType)
						sprintf(sTextLine,"Goal %d TRUE, Aircraft %s Survived.\r\r\n",cnt,AIObjects[GoalList[cnt].dwSerialNumber].sName);
					else
						sprintf(sTextLine,"Goal %d TRUE, Ground Object Survived.\r\r\n",cnt);
				}
			} else {
				if (GoalList[cnt].lGoalType==1)
				{
					// Goal is TRUE, thing destroyed
					if (!GoalList[cnt].lObjectType)
						sprintf(sTextLine,"Goal %d TRUE, Aircraft %s Destroyed.\r\r\n",cnt,AIObjects[GoalList[cnt].dwSerialNumber].sName);
					else
						sprintf(sTextLine,"Goal %d TRUE, Ground Object Destroyed.\r\r\n",cnt);
				}
				if (GoalList[cnt].lGoalType==2)
				{
					// Goal is FALSE, thing did not survived
					if (!GoalList[cnt].lObjectType)
						sprintf(sTextLine,"Goal %d FALSE, Aircraft %s Did Not Survive.\r\r\n",cnt,AIObjects[GoalList[cnt].dwSerialNumber].sName);
					else
						sprintf(sTextLine,"Goal %d FALSE, Ground Object Did Not Survive.\r\r\n",cnt);
				}
			}
			_write(hDebriefFile,sTextLine,(strlen(sTextLine)+1));
		}
#endif

		BOOL bItemResults[MAX_NUM_DEBRIEF_STATEMENTS];
		BOOL bCondition=FALSE;
		BOOL bStatementCondition=FALSE;
		int iVarValue=0;
		BOOL bGroupTrue=FALSE;

		int iStopOutput=0;

		int iPlayerIndex = GetPlayerIndex();


		for (int i=0; i<DebriefData.iNumDebriefGroups; i++)
		{
			bStatementCondition=FALSE;
			bGroupTrue=FALSE;
			for (int j=0; j<DebriefData.pDebriefGroup[i]->iNumStatements; j++)
			{
				memset(bItemResults,0,(sizeof(BOOL)*MAX_NUM_DEBRIEF_STATEMENTS));

				bStatementCondition=TRUE;

				for (int k=0; k<DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iNumItems; k++)
				{
					iVarValue=0;
					switch(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iType)
					{
						case 0:
							// Goals
						   iVarValue = GetGoalEvaluation(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
						break;
						case 1:
						{
							BOOL bFound=FALSE;
							// Mission Variables

							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_PLAYER)
							{
								bFound=TRUE;
								iVarValue = GetMissionStatistic(iPlayerIndex,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_A)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(1,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_B)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(2,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}																														   
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_C)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(3,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_D)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(4,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_E)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(5,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_F)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(6,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_G)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(7,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->lDebriefFlightFlag & DEBRIEF_FLIGHT_FLAG_GROUP_H)
							{
								bFound=TRUE;
								iVarValue+=GetFlightGroupStatistic(8,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
							if (!bFound)
							{
								iVarValue = GetMissionStatistic(iPlayerIndex,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
							}
						}
						break;
						case 2:
							// User Variables
							iVarValue = GetVariableStatus(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iID);
						break;
						default:
							iVarValue=0;
						break;
					}
					bCondition=FALSE;
					if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag==-1)
					{
						switch(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iOperator)
						{
							case 0:	// !=
								if (iVarValue != DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iValue)
									bCondition=TRUE;
							break;
							case 1:	// =
								if (iVarValue == DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iValue)
									bCondition=TRUE;
							break;
							case 2:	// <
								if (iVarValue < DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iValue)
									bCondition=TRUE;
							break;
							case 3:	// >
								if (iVarValue > DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iValue)
									bCondition=TRUE;
							break;
							case 4:	// <=
								if (iVarValue <= DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iValue)
									bCondition=TRUE;
							break;
							case 5:	// >=
								if (iVarValue >= DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iValue)
									bCondition=TRUE;
							break;
							default:
							break;
						}
					} else {
						switch(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iOperator)
						{
							case 0:	// !=
								if (iVarValue != DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag)
									bCondition=TRUE;
							break;
							case 1:	// =
								if (iVarValue == DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag)
									bCondition=TRUE;
							break;
							case 2:	// <
								if (iVarValue < DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag)
									bCondition=TRUE;
							break;
							case 3:	// >
								if (iVarValue > DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag)
									bCondition=TRUE;
							break;
							case 4:	// <=
								if (iVarValue <= DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag)
									bCondition=TRUE;
							break;
							case 5:	// >=
								if (iVarValue >= DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pDebriefStatementItem[k]->iFlag)
									bCondition=TRUE;
							break;
							default:
							break;
						}
					}
					bItemResults[k] = bCondition;
				}
				// Evaluate Statements
				for (k=0; k<DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iNumItems; k++)
				{
					bStatementCondition &= bItemResults[k];
				}
				if (bStatementCondition)
				{
					if (!iStopOutput)
					{
						if (!DebriefData.pDebriefGroup[i]->lDebriefSummary)
						{
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage)
							{
								_write(hDebriefFile,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage,
									(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize));
								sprintf(sTextLine,"\r\r\n");
								_write(hDebriefFile,sTextLine,(strlen(sTextLine)));
							}
						} else {
							if (DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage)
							{
								_write(hDebriefSummaryFile,DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->pszMessage,
									(DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iMessageSize));
								sprintf(sTextLine,"\r\r\n");
								_write(hDebriefSummaryFile,sTextLine,(strlen(sTextLine)));
							}
						   //_write(hDebriefSummaryFile,sTextLine,(strlen(sTextLine)+1));
						}
					}
					// This really sucks and I hate doing this...
					if (!DebriefData.pDebriefGroup[i]->pDebriefStatement[j]->iNumItems)
					{
						bStatementCondition=FALSE;
					}
				}
				if (bStatementCondition)
				{
					bGroupTrue=TRUE;
				}
			}
			if (DebriefData.pDebriefGroup[i]->lCampaignGoalNumber!=-1)
			{
				if (bGroupTrue)
				{
					char *bCheck = GetRegValue ("reece");

					if (bCheck)
					{
						sprintf(sTextLine,"Campaign Goal: %d = TRUE",DebriefData.pDebriefGroup[i]->lCampaignGoalNumber);
						_write(hDebriefFile,sTextLine,(strlen(sTextLine)+1));
					}

					lCampaignGoalList[lCampaignGoalCount]=DebriefData.pDebriefGroup[i]->lCampaignGoalNumber;
					lCampaignGoalCount++;
				}
			}
			if ((DebriefData.pDebriefGroup[i]->iStopProcessing)
				&& bGroupTrue)
			{
				iStopOutput=1;
			}
		}

		char sNullStr[3];
		memset(sNullStr,0,3);
		_write(hDebriefFile,sNullStr,3);
		_write(hDebriefSummaryFile,sNullStr,3);
		_close(hDebriefFile);
		_close(hDebriefSummaryFile);
	}
	WriteCampaignGoalList();

	if (g_iWrapperMode==SIM_CAMPAIGN)
	{
		EvaluateVehiclesResources();
	}

	// If this was an online matchup, send results to server and reset playername to null
	if (g_nMissionType == SIM_MULTI && g_jogcPlayer.szPlayerName[0])
		WriteMultiPlayerKillBoardFile ();
}

//*********************************************************************************************************************
void EvaluateVehiclesResources()
{
	char	szFileName[260];
	char	*pTemp;
	char *pszPath;
	char szFullPath[260];

  	pTemp = strrchr (g_szResourceFile, '\\');
	if (pTemp)
	{
		pTemp++;
		strcpy (szFileName, pTemp);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".sdf");
		else
			strcat (szFileName, ".sdf");
	} else {
		strcpy (szFileName, g_szResourceFile);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".sdf");
		else
			strcat (szFileName, ".sdf");
	}
	pszPath = GetRegValue ("resource");
	sprintf (szFullPath, "%s\\%s", pszPath, szFileName);

 	int iVehicleNum=-1;
	int iVehicleIndex=-1;


	struct stResourceVehicleType
	{
		int iReserved1;
		int iDamageValue[8];
		int iDamageSystems[8];
		CanisterSaver CanSave[8];
	};
	typedef struct stResourceVehicleType ResourceVehicleType;

	ResourceVehicleType *pResourceVehicleList=NULL;

	pResourceVehicleList = (ResourceVehicleType *)malloc(sizeof(ResourceVehicleType)*128);
	memset(pResourceVehicleList,0,(sizeof(ResourceVehicleType)*128));

	if (pResourceVehicleList)
	{
		for (int i=0; i<128; i++)
		{
			if ((iVehicleIndex=VGetVGListNumFromResourceID((i+1)))!=-1)
			{
				VGetShipGroupDamageState((i+1),pResourceVehicleList[i].iDamageValue);
				VGetShipGroupRadarDamageState((i+1),pResourceVehicleList[i].iDamageSystems);
				iVehicleNum=VConvertVGListNumToVNum(iVehicleIndex);
				if (iVehicleNum!=-1)
				{
					for (int j=0; j<g_pMovingVehicleList[iVehicleIndex].iNumGroup; j++)
					{
						SaveVehicleSmoke(&MovingVehicles[iVehicleNum+j],pResourceVehicleList[i+j].CanSave[j]);
					}
				}
			}
		}

		int iFileHandle = _open(szFullPath,_O_WRONLY|_O_BINARY|_O_CREAT|
			O_TRUNC,_S_IREAD | _S_IWRITE);

		if (iFileHandle!=-1)
		{
			_write(iFileHandle,pResourceVehicleList,(sizeof(ResourceVehicleType)*128));
			_close(iFileHandle);
		}

		if (pResourceVehicleList)
		{
			free(pResourceVehicleList);
			pResourceVehicleList=NULL;
		}
	}
}



//*********************************************************************************************************************
void WriteCampaignGoalList()
{
	char	szFileName[260];
	char	*pTemp;
	char *pszPath;
	char szFullPath[260];

  	pTemp = strrchr (g_szResourceFile, '\\');
	if (pTemp)
	{
		pTemp++;
		strcpy (szFileName, pTemp);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".cgf");
		else
			strcat (szFileName, ".cgf");
	} else {
		strcpy (szFileName, g_szResourceFile);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".cgf");
		else
			strcat (szFileName, ".cgf");
	}
	pszPath = GetRegValue ("resource");
	sprintf (szFullPath, "%s\\%s", pszPath, szFileName);

	int iFileHandle = _open(szFullPath,_O_WRONLY|_O_BINARY|_O_CREAT|
		O_TRUNC,_S_IREAD | _S_IWRITE);

	if (iFileHandle!=-1)
	{
		_write(iFileHandle,&lCampaignGoalCount,sizeof(long));
		_write(iFileHandle,lCampaignGoalList,(32*sizeof(long)));

		_close(iFileHandle);
	}
}

//*********************************************************************************************************************
BOOL WriteMultiPlayerKillBoardFile (void)
{
#if 0
	char		szBuffer[10 * 1024];
	char		szTemp[1024];
	char		szNewLine[8];
	int			nGunsFired = GetFlightFlagStatus (FLIGHT_GUN_ROUNDS_FIRED, 1);
	int			nGunsHit = GetFlightFlagStatus (FLIGHT_GUN_ROUNDS_HIT, 1);

	int			nAALaunched = GetFlightFlagStatus (FLIGHT_AA_MISSILES_RELEASED,1);
	int			nAAHit = GetFlightFlagStatus(FLIGHT_AA_MISSILES_HIT,1);
	int			nAAMiss = nAALaunched - nAAHit;
	int			nAAHitPct;
	int			nAAMissPct;
	int			nAASpoofed = 0;
	int			nAAJammed = 0;

	int			nAGHit = 0;
	int			nAGMiss = 0;
	int			nAGSpoofed = 0;
	int			nAGJammed = 0;

	int			nBombHitPct = 0;

	int			nHumanDamage = 0;
	int			nAIDamage = 0;
	int			nHumanKills = GetFlightFlagStatus(FLIGHT_ENEMY_AIRCRAFT_KILLS,1) + GetFlightFlagStatus(FLIGHT_NEUTRAL_AIRCRAFT_KILLS, 1);
	int			nAIKills = 0;
	int			nEnemyFightersKilled = 0;
	int			nEnemyBombersKilled = 0;
	int			nEnemyHelicoptersKilled = 0;
	int			nEnemyShipsKilled = 0;
	int			nEnemySAMsKilled = 0;
	int			nEnemyAAAsKilled = 0;
	int			nEnemyTanksKilled = 0;
	int			nEnemyVehiclesKilled = 0;
	int			nEnemyStructuresKilled = 0;
	int			nEnemyOtherKilled = 0;
//	void **		pDesc;
	int			nFriendliesKilled = GetFlightFlagStatus(FLIGHT_FRIENDLY_AIRCRAFT_KILLS,1);
	int			nHumanDeaths;
	int			nScenarioSuccess = -1;
	int			nAASuccess = -1;
	int			nGunHitPct = nGunsHit ? (nGunsHit * 100) / nGunsFired : 0;

	size_t		ret;
	long		lTime;
	struct tm *	pHereAndNow;
	extern		int MySlot;

	sprintf (szNewLine, "%c%c", 0x0d, 0x0a);

	// If player hit at least once, but pct came out to 0%, give them a break and give them 1%
	if (!nGunHitPct && nGunsHit)
		nGunHitPct = 1;

	// Same with A-A Launched...if player hit at least once, but pct came out to 0%, give them a break and give them 1%
	nAAHitPct = nAAHit ? (nAAHit * 100) / nAALaunched : 0;
	if (!nAAHitPct && nAAHit)
		nAAHitPct = 1;

	nAAMissPct = 100 - nAAHitPct;

	// Init strings
	szBuffer[0] = szTemp[0] = NULL;

	// Get number of times player was killed
	dpSlot *NetGetSlotData( int ix );

	dpSlot *lpSlot = NetGetSlotData(MySlot);
	char   *pTemp = (char *) lpSlot;
	pTemp += sizeof (dpSlot);
	pTemp += 20;	// sizeof name in NetSlot
	pTemp += sizeof (int);	// dpid in NetSlot
	pTemp += sizeof (int);	// lasthit in NetSlot

	int *pnDeaths = (int *)pTemp;

	nHumanDeaths = (*pnDeaths) ? (*pnDeaths) - 1 : 0;

	// Write callsign
	sprintf (szBuffer, "%c%s%c,%s", '"', g_jogcPlayer.szPlayerName, '"', szNewLine);

	// Write GMT time
	time (&lTime);
	pHereAndNow = gmtime (&lTime);
	ret = strftime (szTemp, 1024, "%m,%d,%Y,", pHereAndNow);
	strcat (szBuffer, szTemp);
	strcat (szBuffer, szNewLine);

	ret = strftime (szTemp, 1024, "%H,%M,%S,", pHereAndNow);
	strcat (szBuffer, szTemp);
	strcat (szBuffer, szNewLine);
	
	// Write AA Launch info
	sprintf (szTemp, "%03d,%03d,%03d,000,000,%s", nAALaunched, nAAHitPct, nAAMissPct, szNewLine);// not writing spoofed or jammed
	strcat (szBuffer, szTemp);

	// Write bogus placeholder stats for A/G Launch info
	sprintf (szTemp, "000,000,000,000,000,%s", szNewLine);	//AG Launch,AGHit,AGMiss,AGSpoofed,AGJammed
	strcat (szBuffer, szTemp);

	// Write Gun Hit Pct / Bomb Hit Pct
	sprintf (szTemp, "%03d,%s000,%s", nGunHitPct, szNewLine, szNewLine);
	strcat (szBuffer, szTemp);

	// Write Human Damage / Computer Damage / Human Kills / AI Kills / Player Deaths
	sprintf (szTemp, "00000,00000,%03d,000,%03d,%s", nHumanKills, nHumanDeaths, szNewLine);
	strcat (szBuffer, szTemp);

	// Write Enemy stats (not applicable),
	sprintf (szTemp, "000,000,000,000,000,000,000,000,000,000,");
	strcat (szBuffer, szTemp);

	// Write friendly aircraft killed, scenario success (n/a), AA success (n/a)
	sprintf (szTemp, "%03d,%s-1,-1,%s%s", nFriendliesKilled, szNewLine, szNewLine, szNewLine);
	strcat (szBuffer, szTemp);

	///////////////////////////////////////////////////////////////////////
	// Send data to JOGC server
	///////////////////////////////////////////////////////////////////////

	MSFILTER		DummyData;

	int one=1;
	int sizeofone=sizeof(one);

	DummyData.totalintfields=1;
	DummyData.intfieldlengths=&sizeofone;
	DummyData.intfieldweights=&one;
	DummyData.intfieldvariance=&one;
	DummyData.totalcharfields=0;
	DummyData.charfieldlengths=NULL;
	DummyData.charfieldweights=NULL;
	DummyData.charfieldvariance=NULL;

	int nLength = strlen (szBuffer);

	// First Command
	if (connectMS ("IP206.251.23.215:9371") != MS_NO_ERROR)
	{
		//Trap Error Here ( error's are in MSAPI.H )
		return FALSE;
	}

	// Second Command
	if (initializeMS ((char *)&one,&DummyData,sizeofone,60) != MS_NO_ERROR)
	{
		//Trap Error Here ( error's are in MSAPI.H )
		return FALSE;
	}

	// Third Command
	if (sendMSresults (NULL, szBuffer, nLength) != MS_NO_ERROR)
	{
		//Trap Error Here ( error's are in MSAPI.H )
		return FALSE;
	}

	// Fourth Command
	if (closeMS() != MS_NO_ERROR)
	{
		//Trap Error Here
		return FALSE;
	};
#endif
	return (TRUE);
}


BOOL TrackObject(long lSerialNumber)
{
	for (int i=0; i<iNumGroundObjects; i++)
	{
		if ((long)pGroundObjectList[i].dwSerialNumber==(long)(lSerialNumber & SN_PLUG_IN_MASK))
		{		
			if (pGroundObjectList[i].lCampaignResourceFlags & CR_DONOTTRACK)
			{
				return FALSE;
			} else {
				return TRUE;
			}
		}
	}
	return TRUE;
}


void EvaluateGroundObjects()
{
	BasicInstance *walker;
	char	szFileName[260];
	char	*pTemp;
	char szFullPath[260];
	char *pszPath;

//	if (!g_iCampaignMode) return;

  	pTemp = strrchr (g_szResourceFile, '\\');
	if (pTemp)
	{
		pTemp++;
		strcpy (szFileName, pTemp);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".dmg");
		else
			strcat (szFileName, ".dmg");
	} else {
		strcpy (szFileName, g_szResourceFile);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".dmg");
		else
			strcat (szFileName, ".dmg");
	}
	pszPath = GetRegValue ("resource");
	sprintf (szFullPath, "%s\\%s", pszPath, szFileName);



	// Read in damaged ground object list
	// Decrement the count for objects that need it
	// Format:  SerialNumber, status, count, replacement
	// After reading file in, go through the list and decrement the
	// count for everything (excluding -1's).
	// Status would indicate if this is a replacement object

	// When something is destroyed its added to the list
	// If the object has a replacement object, then we look to see
	// how long we need before the replacement arrives.
	// So we record the serial number and the count until replacement
	// object arrives.
	// When a replacement object arrives, the serial numbers change,
	// so we probably want to record this fact

#if 0
struct stCampaignDamageType
{
	DWORD dwSerialNumber;
	int iStatus;
	int MissionsTillRepaired;
	DWORD dwReplacementSerial;
	int MissionsTillNoFire;
	int iBurnType;
	ExtraDamageInfo DamagePlacement;
};
typedef struct stCampaignDamageType CampaignDamageType;
#endif

	int iNumCampaignDamageObjects=0;
	CampaignDamageType *pCampaignDamageList=NULL;

	int hDamageObjectFile=-1;

	if ((hDamageObjectFile=_open(szFullPath,_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hDamageObjectFile,&iNumCampaignDamageObjects,sizeof(int));
		pCampaignDamageList = (CampaignDamageType *)malloc(iNumCampaignDamageObjects*sizeof(CampaignDamageType));
		_read(hDamageObjectFile,pCampaignDamageList,(iNumCampaignDamageObjects*sizeof(CampaignDamageType)));
		_close(hDamageObjectFile);
	}

	for (int i=0; i<iNumCampaignDamageObjects; i++)
	{
		if (pCampaignDamageList[i].iStatus)
		{
			if (pCampaignDamageList[i].MissionsTillRepaired>0)
			{
				pCampaignDamageList[i].MissionsTillRepaired--;
			}
		}
		if (pCampaignDamageList[i].MissionsTillNoFire>0)
		{
			pCampaignDamageList[i].MissionsTillNoFire--;
		}
	}

	walker = AllInstances;

	BOOL bFound=FALSE;

	while (walker)
	{
		bFound=FALSE;
		if (walker->Flags & BI_DESTROYED)
		{
			// Find us in the list
			for (int i=0; i<iNumCampaignDamageObjects; i++)
			{
				if (pCampaignDamageList[i].dwSerialNumber==walker->SerialNumber)
				{
					bFound=TRUE;
					if (pCampaignDamageList[i].dwReplacementSerial) bFound=FALSE;
				}
			}
			if (!bFound)
			{
				if (TrackObject(walker->SerialNumber))
				{
					if (walker->Flags & BI_REPLACEMENT)
					{
						// Find us in the list
						for (int i=0; i<iNumCampaignDamageObjects; i++)
						{
							if (pCampaignDamageList[i].dwSerialNumber==walker->SerialNumber)
							{
								pCampaignDamageList[i].iStatus=1;
								pCampaignDamageList[i].MissionsTillRepaired=pCampaignDamageList[i].OriginalMissionsTillRepaired;
								pCampaignDamageList[i].dwReplacementSerial=0;
								pCampaignDamageList[i].MissionsTillNoFire=0;
								pCampaignDamageList[i].iBurnType=0;
								pCampaignDamageList[i].DamagePlacement.TempLocation.X=0;
								pCampaignDamageList[i].DamagePlacement.TempLocation.Y=0;
								pCampaignDamageList[i].DamagePlacement.TempLocation.Z=0;
								pCampaignDamageList[i].DamagePlacement.Heading=0;
								pCampaignDamageList[i].DamagePlacement.Pitch=0;
								pCampaignDamageList[i].DamagePlacement.Roll=0;
							}
						}
					} else {
						if (!(walker->SerialNumber & SN_DAMAGED_OBJECT))
						{
							// Okay, lets add this guy to the list
							pCampaignDamageList = (CampaignDamageType *)realloc(pCampaignDamageList,(sizeof(CampaignDamageType) * (iNumCampaignDamageObjects+1)));
							pCampaignDamageList[iNumCampaignDamageObjects].dwSerialNumber = walker->SerialNumber;
							pCampaignDamageList[iNumCampaignDamageObjects].iStatus=0;
							pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillRepaired=0;
							pCampaignDamageList[iNumCampaignDamageObjects].dwReplacementSerial=0;
							pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillNoFire=0;
							pCampaignDamageList[iNumCampaignDamageObjects].iBurnType=0;
							pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.TempLocation.X=0;
							pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.TempLocation.Y=0;
							pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.TempLocation.Z=0;
							pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.Heading=0;
							pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.Pitch=0;
							pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.Roll=0;

							if (walker->Family == FAMILY_RUNWAYPIECE)
							{
								pCampaignDamageList[iNumCampaignDamageObjects].iBurnType=((RunwayPieceInstance *)walker)->DamageAndVars;
							}

							DamageTypeExt DamageTmp;

							if (InstanceHasTempObject(walker,&DamageTmp))
							{
								pCampaignDamageList[iNumCampaignDamageObjects].iStatus=1;

								switch(DamageTmp.MissionsTillRepaired)
								{
									case REPAIRED_NEXT_MISSION:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillRepaired=0;
									break;
									case REPAIRED_IN_2_MISSIONS:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillRepaired=2;
									break;
									case REPAIRED_IN_5_MISSIONS:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillRepaired=5;
									break;
									case REPAIRED_NEVER:
									default:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillRepaired=0;
									break;
								}
								pCampaignDamageList[iNumCampaignDamageObjects].OriginalMissionsTillRepaired=pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillRepaired;
								pCampaignDamageList[iNumCampaignDamageObjects].dwReplacementSerial=0;

								switch(DamageTmp.MissionsTillNoFire)
								{
									case FIRE_BURNS_1_MISSION:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillNoFire=1;
									break;
									case FIRE_BURNS_3_MISSIONS:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillNoFire=3;
									break;
									case FIRE_BURNS_5_MISSIONS:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillNoFire=5;
									break;
									case FIRE_BURNS_FOREVER:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillNoFire=100;	// One Hundred missions
									break;
									default:
										pCampaignDamageList[iNumCampaignDamageObjects].MissionsTillNoFire=0;
									break;
								}
								pCampaignDamageList[iNumCampaignDamageObjects].iBurnType=0;
								pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.TempLocation.X=0;
								pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.TempLocation.Y=0;
								pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.TempLocation.Z=0;
								pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.Heading=0;
								pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.Pitch=0;
								pCampaignDamageList[iNumCampaignDamageObjects].DamagePlacement.Roll=0;
							}
							iNumCampaignDamageObjects++;
						}
					}
				}
			}
		}
		walker = (BasicInstance *)walker->NextInstance;
	}

	int iFileHandle = _open(szFullPath,_O_WRONLY|_O_BINARY|_O_CREAT|
		O_TRUNC,_S_IREAD | _S_IWRITE);

	if (iFileHandle!=-1)
	{
		_write(iFileHandle,&iNumCampaignDamageObjects,sizeof(int));
		_write(iFileHandle,pCampaignDamageList,(sizeof(CampaignDamageType)*iNumCampaignDamageObjects));
		_close(iFileHandle);
	}
	if (pCampaignDamageList)
	{
		free(pCampaignDamageList);
		pCampaignDamageList=NULL;
	}
}



#if 0
void LogMissionFlag(int iMissionFlagId, int iFlight, int iValue, int iOperator)
{				  
	BOOL bFound=FALSE;

	for (int i=0; i<g_iNumMissionFlags; i++)
	{
		if ((iMissionFlagId==MissionEventFlagList[i].iID) && (iFlight==MissionEventFlagList[i].iFlight))
		{
			switch(iOperator)
			{
				case 0:	// Replace
					MissionEventFlagList[i].iValue = iValue;
				break;
				case 1:	// Add
					MissionEventFlagList[i].iValue += iValue;
				break;
				case 2:	// One Time
				break;
			}
			bFound=TRUE;
		}
		if (bFound) break;
	}
	if (!bFound)
	{
		if (g_iNumMissionFlags<MAX_NUM_MISSION_FLAGS)
		{
			MissionEventFlagList[g_iNumMissionFlags].iID=iMissionFlagId;
			MissionEventFlagList[g_iNumMissionFlags].iValue=iValue;
			MissionEventFlagList[g_iNumMissionFlags].iFlight=iFlight;
			g_iNumMissionFlags++;
		}
	}
}
#endif

int GetPlayerIndex()
{
	for (int i=0; i<iNumAIObjs; i++)
	{
		if (AIObjects[i].iFlags & AL_DEVICE_DRIVEN)
		{
			return(i);
		}
	}
	return(-1);
}

int GetFlagStatus(int iMissionFlagId)
{
	int iReturnValue=0;

	for (int i=0; i<g_iNumMissionFlags; i++)
	{
		if (iMissionFlagId==MissionEventFlagList[i].iID)
		{
			iReturnValue=MissionEventFlagList[i].iValue;
			break;
		}
	}
	return(iReturnValue);
}

void InitCampaignUsageFile()
{
	g_iNumUsage=0;
	pCampaignUsageList = (CampaignUsageType *)malloc(sizeof(CampaignUsageType)*256);
}

void WriteCampaignUsageFile()
{
	char	szFileName[260];
	char	*pTemp;
	char szFullPath[260];
	char *pszPath;

  	pTemp = strrchr (g_szResourceFile, '\\');
	if (pTemp)
	{
		pTemp++;
		strcpy (szFileName, pTemp);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".cuf");
		else
			strcat (szFileName, ".cuf");
	} else {
		strcpy (szFileName, g_szResourceFile);
		pTemp = strchr (szFileName, '.');
		if (pTemp)
			strcpy (pTemp, ".cuf");
		else
			strcat (szFileName, ".cuf");
	}
	pszPath = GetRegValue ("resource");
	sprintf (szFullPath, "%s\\%s", pszPath, szFileName);

	int iFileHandle = _open(szFullPath,_O_WRONLY|_O_BINARY|_O_CREAT|
		O_TRUNC,_S_IREAD | _S_IWRITE);

	if (iFileHandle!=-1)
	{
		_write(iFileHandle,&g_iNumUsage,sizeof(int));
		_write(iFileHandle,pCampaignUsageList,(sizeof(CampaignUsageType)*g_iNumUsage));
		_close(iFileHandle);
	}
}

void CleanUpCampaignFile()
{
	if (pCampaignUsageList)
	{
		free(pCampaignUsageList);
		pCampaignUsageList=NULL;
	}
	g_iNumUsage=0;
}

void AddUsageToCampaignList(int iUsageType, int iWeaponID, int iCount)
{
	BOOL bFound=FALSE;

	for (int i=0; i<g_iNumUsage; i++)
	{
		if (pCampaignUsageList[i].iUsageType==iUsageType)
		{
			if (pCampaignUsageList[i].iUsageSubType==iWeaponID)
			{
				pCampaignUsageList[i].iCount+=iCount;
				bFound=TRUE;
				break;
			}
		}
	}
	if (!bFound)
	{
		if (g_iNumUsage<255)
		{
			pCampaignUsageList[g_iNumUsage].iUsageType=iUsageType;
			pCampaignUsageList[g_iNumUsage].iUsageSubType=iWeaponID;
			pCampaignUsageList[g_iNumUsage].iCount=iCount;
			g_iNumUsage++;
		}
	}
}

//*********************************************************************************************************************
BOOL GetGoalEvaluation(int iGoalNum)
{
	int bValue=0;

	if ((GoalList[iGoalNum].lGoalRating==0xFF)
		&& (!GoalList[iGoalNum].lGoalType)
		&& (!GoalList[iGoalNum].lObjectType)
		&& (!strcmp(GoalList[iGoalNum].sLabel,"AREA GOAL")))
	{
		float fPercent = (float)g_iNumAroundGoalObjectsHit/(float)g_iNumWeaponReleasedInAreaGoal;
		fPercent *= 100;
		if (fPercent>100) fPercent=100.0f;
		if (fPercent<0) fPercent=0.0f;

		return((int)fPercent);
	}

	if ((GoalList[iGoalNum].lGoalRating==0xFF)
		&& (!GoalList[iGoalNum].lGoalType)
		&& (!GoalList[iGoalNum].lObjectType)
		&& (!strcmp(GoalList[iGoalNum].sLabel,"WEAPON AREA GOAL")))
	{
		float fPercent = (float)g_iNumWeaponAreaGoalHit/(float)g_iNumWeaponAreaGoal;
		fPercent *= 100;
		if (fPercent>100) fPercent=100.0f;
		if (fPercent<0) fPercent=0.0f;

		return((int)fPercent);
	}

	if (cAIGoalSame[iGoalNum])
	{
		if (GoalList[iGoalNum].lGoalType==1)
		{
			// Goal is FALSE, thing not destroyed
			bValue=0;
		}
		if (GoalList[iGoalNum].lGoalType==2)
		{
			// Goal is True, thing survived
			bValue=1;
		}
	} else {
		if (GoalList[iGoalNum].lGoalType==1)
		{
			// Goal is TRUE, thing destroyed
			bValue=1;
		}
		if (GoalList[iGoalNum].lGoalType==2)
		{
			// Goal is FALSE, thing did not survived
			bValue=0;
		}
	}
	return(bValue);
}

//*********************************************************************************************************************
int GetFlightGroupStatistic(int iFlightGroup,int iType)
{
	if (iFlightGroup<1) return(0);			// Check for invalid aircraft index
	if (iFlightGroup>8) return(0);

	if (iType<0) return(0);					// Check for invalid event log id
	if (iType>MAX_NUM_EVENT_FLAGS) return(0);

	if (!MissionStatistics) return(0);			// Make sure we are still around

	int iValue=0;

	for (int i=0; i<iNumAIObjs; i++)
	{
		if (AIObjects[i].iBriefingGroup==iFlightGroup)
		{
			iValue+=MissionStatistics[i].iFlagList[iType];
		}
	}

	return(iValue);
}


//*********************************************************************************************************************
int GetMissionStatistic(int iAircraftIndex,int iType)
{
	if (iAircraftIndex<0) return(0);			// Check for invalid aircraft index
	if (iAircraftIndex>=iNumAIObjs) return(0);

	if (iType<0) return(0);					// Check for invalid event log id
	if (iType>MAX_NUM_EVENT_FLAGS) return(0);

	if (!MissionStatistics) return(0);			// Make sure we are still around

	return(MissionStatistics[iAircraftIndex].iFlagList[iType]);
}

//*********************************************************************************************************************
int GetFlightFlagStatus(int iMissionFlagId,int iFlight)
{
	int iReturnValue=0;

	return(iReturnValue);
}

//*********************************************************************************************************************
BOOL InstanceHasTempObject(BasicInstance *instance,DamageTypeExt *DamageTmp)
{
	BOOL bStatus=FALSE;

	if (!(instance->Flags & BI_NO_MODEL)) //This should weed out the foundation classes
	{
		switch(instance->Family)
		{
			case FAMILY_STRUCTURE:
			{
				StructureFamily *pTmp = (StructureFamily *)((StructureInstance *)instance)->Type;
				if (pTmp->Physicals.Flags & PTE_HAS_TEMP_OBJECT) bStatus=TRUE;
				*DamageTmp=pTmp->Physicals.Damage;
			}
			break;
			case FAMILY_INFOPROVIDER:
			{
				InfoProviderFamily *pTmp = (InfoProviderFamily *)((InfoProviderInstance *)instance)->Type;
				if (pTmp->Physicals.Flags & PTE_HAS_TEMP_OBJECT) bStatus=TRUE;
				*DamageTmp=pTmp->Physicals.Damage;
			}
			break;

			case FAMILY_AAWEAPON:
			{
				AAWeaponFamily *pTmp = (AAWeaponFamily *)((AAWeaponInstance *)instance)->Type;
				if (pTmp->Physicals.Flags & PTE_HAS_TEMP_OBJECT) bStatus=TRUE;
				*DamageTmp=pTmp->Physicals.Damage;
			}
			break;
		}
	}

	return(bStatus);
}


//*********************************************************************************************************************
void InitMissionFlagStuff()
{
	memset(MissionEventFlagList,0,(MAX_NUM_MISSION_FLAGS*sizeof(MissionEventFlag)));
	g_iNumMissionFlags=0;
	g_iNumAroundGoalObjectsHit=0;
	g_iNumWeaponReleasedInAreaGoal=0;
	g_iNumWeaponAreaGoal=0;
	g_iNumWeaponAreaGoalHit=0;
}

char g_szDumpLogFile[260];
extern int GetF18Version( char* );

//*******************************************************************************************************************************
void InitDumpLog()
{
	char szTxt[260];
	char szVersion[256];

	FILE *fp=NULL;
	time_t ltime;
	
	time( &ltime );

	strcpy(g_szDumpLogFile,(RegPath("resource", "f18dev.log")));

	fp = fopen(g_szDumpLogFile,"w+t");
	if (fp)
	{
		sprintf(szTxt,"%s","Jane's F-18 - Program Log Start\n");
		fwrite(szTxt,strlen(szTxt),1,fp);
		
		sprintf(szTxt,"%s",ctime(&ltime));
		fwrite(szTxt,strlen(szTxt),1,fp);

		GetF18Version(szVersion);
		strcpy(szTxt,szVersion);
		strcat(szTxt,"\n");
		fwrite(szTxt,strlen(szTxt),1,fp);

		fclose(fp);
	}
}

//*******************************************************************************************************************************
void WriteDumpLog(char *szString)
{
	FILE *fp=NULL;
	
	fp = fopen(g_szDumpLogFile,"a+t");

	if (fp)
	{
		fwrite(szString,strlen(szString),1,fp);
		fclose(fp);
	}
}

//*******************************************************************************************************************************
void CloseDumpLog()
{
	WriteDumpLog("Jane's F-18 - End Of Program");
}


//*******************************************************************************************************************************
void InitDamageVehicles()
{
	char					szFileName[260];
	char	*				pTemp;
	char *					pszPath;
	char					szFullPath[260];
	extern MULTIOPTIONS		g_MultiOptions;

	// change file name here if multijoiner to spoofed file
	if (MultiPlayer && g_MultiOptions.dpidMe && !g_MultiOptions.bHost)
	{
		pTemp = GetRegValue ("resource");
		sprintf (szFullPath, "%s\\multisdf.sdf", pTemp);
	}
	else
	{
  		pTemp = strrchr (g_szResourceFile, '\\');
		if (pTemp)
		{
			pTemp++;
			strcpy (szFileName, pTemp);
			pTemp = strchr (szFileName, '.');
			if (pTemp)
				strcpy (pTemp, ".sdf");
			else
				strcat (szFileName, ".sdf");
		} else {
			strcpy (szFileName, g_szResourceFile);
			pTemp = strchr (szFileName, '.');
			if (pTemp)
				strcpy (pTemp, ".sdf");
			else
				strcat (szFileName, ".sdf");
		}
		pszPath = GetRegValue ("resource");
		sprintf (szFullPath, "%s\\%s", pszPath, szFileName);
	}

 	int iVehicleNum=-1;
	int iVehicleIndex=-1;


	struct stResourceVehicleType
	{
		int iReserved1;
		int iDamageValue[8];
		int iDamageSystems[8];
		CanisterSaver CanSave[8];
	};
	typedef struct stResourceVehicleType ResourceVehicleType;

	ResourceVehicleType *pResourceVehicleList=NULL;

	pResourceVehicleList = (ResourceVehicleType *)malloc(sizeof(ResourceVehicleType)*128);
	memset(pResourceVehicleList,0,(sizeof(ResourceVehicleType)*128));

	if (pResourceVehicleList)
	{
		int iFileId = _open(szFullPath,_O_RDONLY | _O_BINARY);

		if (iFileId!=-1)
		{
			_read(iFileId,pResourceVehicleList,(sizeof(ResourceVehicleType)*128));
			_close(iFileId);

			for (int i=0; i<128; i++)
			{
				if ((iVehicleIndex=VGetVGListNumFromResourceID((i+1)))!=-1)
				{
					VSetShipGroupDamageState((i+1),pResourceVehicleList[i].iDamageValue);
					VSetShipGroupRadarDamageState((i+1),pResourceVehicleList[i].iDamageSystems);
					iVehicleNum=VConvertVGListNumToVNum(iVehicleIndex);
					if (iVehicleNum!=-1)
					{
						for (int j=0; j<g_pMovingVehicleList[iVehicleIndex].iNumGroup; j++)
						{
							if (pResourceVehicleList[i].iDamageValue[j]<3)
							{
								RestoreVehicleSmoke(&MovingVehicles[iVehicleNum+j],pResourceVehicleList[i+j].CanSave[j]);
							}
						}
					}
				}
			}
		}

		if (pResourceVehicleList)
		{
			free(pResourceVehicleList);
			pResourceVehicleList=NULL;
		}
	}
}


