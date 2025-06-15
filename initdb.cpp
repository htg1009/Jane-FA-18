#define __THEBASE__
#include "F18.h"
#include "f18weap.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int g_RadarTypes[256];
int iNumRadarTypes=0;
int g_WeaponTypes[256];
int iNumWeaponTypes=0;
extern GenericInstanceDef *WorldInstanceDefs;


int GetPlaneIndex(long lPlaneID);
int GetMovingVehicleIndex(long lVehicleID);
int GetShipIndex(long lShipID);

extern DBWeaponType *get_weapon_ptr(long id);

void InitDB()
{
	// Read Planes
	// Read Radar
	// Read Loadouts
	// Read Weapons

	int hFileHandle;

	DataBaseIdTagType DataBaseTag;

	if ((hFileHandle=_open(RegPath("data","fairdb.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFileHandle,&DataBaseTag,sizeof(DataBaseIdTagType));
		_read(hFileHandle,&g_iNumAircraft,sizeof(int));
		pAircraftDB = (DBAircraftType *)malloc(sizeof(DBAircraftType) * g_iNumAircraft);
		_read(hFileHandle,pAircraftDB,(sizeof(DBAircraftType) * g_iNumAircraft));
		_close(hFileHandle);
	}  else {
		MessageBox(hwnd,"fairdb.dat - File Not Found","Warning",MB_OK);
	}

	if ((hFileHandle=_open(RegPath("data","fraddb.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFileHandle,&DataBaseTag,sizeof(DataBaseIdTagType));
		_read(hFileHandle,&g_iNumRadar,sizeof(int));
		pRadarDB = (DBRadarType *)malloc(sizeof(DBRadarType) * g_iNumRadar);
		_read(hFileHandle,pRadarDB,(sizeof(DBRadarType) * g_iNumRadar));
		_close(hFileHandle);
	}  else {
		MessageBox(hwnd,"fraddb.dat - File Not Found","Warning",MB_OK);
	}

	if ((hFileHandle=_open(RegPath("data","fnloaddb.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFileHandle,&DataBaseTag,sizeof(DataBaseIdTagType));
		_read(hFileHandle,&g_iNumLoadOut,sizeof(int));
		pLoadOutDB = (DBLoadOutType *)malloc((sizeof(DBLoadOutType) * g_iNumLoadOut));
		_read(hFileHandle,pLoadOutDB,(sizeof(DBLoadOutType) * g_iNumLoadOut));
		_close(hFileHandle);
	}  else {
		MessageBox(hwnd,"floaddb.dat - File Not Found","Warning",MB_OK);
	}

	if ((hFileHandle=_open(RegPath("data","fweapdb.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFileHandle,&DataBaseTag,sizeof(DataBaseIdTagType));
		_read(hFileHandle,&g_iNumWeapon,sizeof(int));
		pWeaponDB = (DBWeaponType *)malloc((sizeof(DBWeaponType) * g_iNumWeapon));
		_read(hFileHandle,pWeaponDB,(sizeof(DBWeaponType) * g_iNumWeapon));
		_close(hFileHandle);
	}  else {
		MessageBox(hwnd,"fweapdb.dat - File Not Found","Warning",MB_OK);
	}
	if ((hFileHandle=_open(RegPath("data","fvehidb.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFileHandle,&DataBaseTag,sizeof(DataBaseIdTagType));
		_read(hFileHandle,&g_iNumVehicle,sizeof(int));
		pVehicleDB = (DBVehicleType *)malloc((sizeof(DBVehicleType) * g_iNumVehicle));
		_read(hFileHandle,pVehicleDB,(sizeof(DBVehicleType) * g_iNumVehicle));
		_close(hFileHandle);
	}  else {
		MessageBox(hwnd,"fvehidb.dat - File Not Found","Warning",MB_OK);
	}
	if ((hFileHandle=_open(RegPath("data","fshipdb.dat"),_O_BINARY | _O_RDONLY)) != -1)
	{
		_read(hFileHandle,&DataBaseTag,sizeof(DataBaseIdTagType));
		_read(hFileHandle,&g_iNumShip,sizeof(int));
		pShipDB = (DBShipType *)malloc((sizeof(DBShipType) * g_iNumShip));
		_read(hFileHandle,pShipDB,(sizeof(DBShipType) * g_iNumShip));
		_close(hFileHandle);
	}  else {
		MessageBox(hwnd,"fshipdb.dat - File Not Found","Warning",MB_OK);
	}
}

void FreeLoadDB()
{
	if (pAircraftDB)
	{
		free(pAircraftDB);
		pAircraftDB=NULL;
	}
	if (pRadarDB)
	{
		free(pRadarDB);
		pRadarDB=NULL;
	}
	if (pLoadOutDB)
	{
		free(pLoadOutDB);
		pLoadOutDB=NULL;
	}
	if (pWeaponDB)
	{
		free(pWeaponDB);
		pWeaponDB=NULL;
	}
	if (pVehicleDB)
	{
		free(pVehicleDB);
		pVehicleDB=NULL;
	}
	if (pShipDB)
	{
		free(pShipDB);
		pShipDB=NULL;
	}
}

void FreeDB()
{
	if (pDBAircraftList)
	{
		free(pDBAircraftList);
		pDBAircraftList=NULL;
	}
	if (pDBRadarList)
	{
		free(pDBRadarList);
		pDBRadarList=NULL;
	}
	if (pDBWeaponList)
	{
		free(pDBWeaponList);
		pDBWeaponList=NULL;
	}
	if (pDBVehicleList)
	{
		free(pDBVehicleList);
		pDBVehicleList=NULL;
	}
	if (pDBShipList)
	{
		free(pDBShipList);
		pDBShipList=NULL;
	}
}


int GetPlaneIndex(long lPlaneID)
{
	for (int i=0; i<iNumAircraftList; i++)
	{
		if (pDBAircraftList[i].lPlaneID == lPlaneID)
		{
			return(i);
		}
	}
	return(-1);
}

int GetWeaponIndex(long lWeaponID)
{
	for (int i=0; i<g_iNumWeapon; i++)
	{
		if (pWeaponDB[i].lWeaponID == lWeaponID)
		{
			return(i);
		}
	}
	return(-1);
}

void EvaluateAircraft()
{
	int cnt;
	int iFound;
	int j;

	int *pTypes = (int *)malloc(sizeof(int)*256);
	int iNumTypes=0;

	for(cnt = 0; cnt < iNumAIObjs; cnt ++)
	{
		iFound=0;

		for (int j=0; j<iNumTypes; j++)
		{
			if (pTypes[j]==AIObjects[cnt].iType)
			{
				iFound++;
				break;
			}
		}
		if (!iFound)
		{
			if (iNumTypes<256)
			{
				pTypes[iNumTypes]=AIObjects[cnt].iType;
				iNumTypes++;
			}
		}
	}

	//  Add for putting AG missile planes in
	for(cnt = 69; cnt < 79; cnt ++)
	{
		iFound=0;

		for (j=0; j<iNumTypes; j++)
		{
			if (pTypes[j]==cnt)
			{
				iFound++;
				break;
			}
		}
		if (!iFound)
		{
			if (iNumTypes<256)
			{
				pTypes[iNumTypes]=cnt;
				iNumTypes++;
			}
		}
	}

	cnt = 84;
	iFound=0;

	for (j=0; j<iNumTypes; j++)
	{
		if (pTypes[j]==cnt)
		{
			iFound++;
			break;
		}
	}
	if (!iFound)
	{
		if (iNumTypes<256)
		{
			pTypes[iNumTypes]=cnt;
			iNumTypes++;
		}
	}

	cnt = 85;
	iFound=0;

	for (j=0; j<iNumTypes; j++)
	{
		if (pTypes[j]==cnt)
		{
			iFound++;
			break;
		}
	}
	if (!iFound)
	{
		if (iNumTypes<256)
		{
			pTypes[iNumTypes]=cnt;
			iNumTypes++;
		}
	}

	cnt = 88;
	iFound=0;

	for (j=0; j<iNumTypes; j++)
	{
		if (pTypes[j]==cnt)
		{
			iFound++;
			break;
		}
	}
	if (!iFound)
	{
		if (iNumTypes<256)
		{
			pTypes[iNumTypes]=cnt;
			iNumTypes++;
		}
	}

	// Allocate Memory for Needed planes data

	if (iNumTypes)
	{
		pDBAircraftList = (DBAircraftType *)malloc(sizeof(DBAircraftType) * iNumTypes);
		iNumAircraftList=0;

		for (int i=0; i<iNumTypes; i++)
		{
			for (int j=0; j<g_iNumAircraft; j++)
			{
				if (pTypes[i]==pAircraftDB[j].lPlaneID)
				{
					pDBAircraftList[iNumAircraftList] = pAircraftDB[j];
					if(pDBAircraftList[iNumAircraftList].iCeiling >= 30000)
					{
						float fceiling = pDBAircraftList[iNumAircraftList].iCeiling;
						fceiling *= 0.80f;
						pDBAircraftList[iNumAircraftList].iCeiling = fceiling;
					}
					iNumAircraftList++;
				}
			}
		}
	}

	memset(pTypes,0,sizeof(int)*256);
	iNumTypes=0;

	for (cnt=0; cnt<iNumAircraftList; cnt++)
	{
		iFound=0;

		for (int j=0; j<iNumTypes; j++)
		{
			if (pTypes[j]==pDBAircraftList[cnt].lRadarID)
			{
				iFound++;
				break;
			}
		}
		if (!iFound)
		{
			if (iNumTypes<256)
			{
				pTypes[iNumTypes]=pDBAircraftList[cnt].lRadarID;
				iNumTypes++;
			}
		}
	}


//	memset(g_RadarTypes,0,sizeof(int)*256);
//	iNumRadarTypes=0;
//	memset(g_WeaponTypes,0,sizeof(int)*256);
//	iNumWeaponTypes=0;

	// Account for world object stuff
	for (cnt=0; cnt<iNumRadarTypes; cnt++)
	{
		iFound=0;
		for (int j=0; j<iNumTypes; j++)
		{
			if (g_RadarTypes[cnt]==pTypes[j])
			{
				iFound++;
				break;
			}
		}
		if (!iFound)
		{
			if (iNumTypes<256)
			{
				pTypes[iNumTypes]=g_RadarTypes[cnt];
				iNumTypes++;
			}
		}
	}

	//

	if (iNumTypes)
	{
		pDBRadarList = (DBRadarType *)malloc(sizeof(DBRadarType) * iNumTypes);
		iNumRadarList=0;

		for (int i=0; i<iNumTypes; i++)
		{
			for (int j=0; j<g_iNumRadar; j++)
			{
				if (pTypes[i]==pRadarDB[j].lRadarID)
				{
					pDBRadarList[iNumRadarList] = pRadarDB[j];
					iNumRadarList++;
				}
			}
		}
	}

	memset(pTypes,0,sizeof(int)*256);
	iNumTypes=0;
	int iTemplate=0;
	int iLoadOutIndex=0;

	for (int i=0; i<g_iNumWeapon; i++)
	{
		if (pWeaponDB[i].iF18Resource)
		{
			pTypes[iNumTypes]=pWeaponDB[i].lWeaponID;
			iNumTypes++;
		}
	}


	for(cnt = 0; cnt < iNumAIObjs; cnt ++)
	{
		// First we need to find the loadout
		// then we need to find the loadout
		// then we need to find the weapons
		// that will be needed
		iTemplate=0;
		iLoadOutIndex=0;

		for (int j=0; j<iNumAircraftList; j++)
		{
			if (pDBAircraftList[j].lPlaneID==AIObjects[cnt].iType)
			{
				// Ok, we got the aircraft and we know what
				// template has been selected, now search
				// for the loadout within the template
				iTemplate = pDBAircraftList[j].lLoadOutId;
				iLoadOutIndex = AIObjects[cnt].lLoadOut;
				if (iLoadOutIndex<0)
					iLoadOutIndex=0;	// Ok, this sucks
//				iLoadOutIndex = pDBAircraftList[j].lLoadOutIndex;

				for (int k=0; k<pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].iNumLoadOut; k++)
				{
					iFound=0;	// This was not here, now it is
					for (int n=0; n<iNumTypes; n++)
					{
						if (pTypes[n]==pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].lWeaponId)
						{
							iFound++;
							break;
						}
					}
					if (!iFound)
					{
						if (iNumTypes<256)
						{
							pTypes[iNumTypes]=pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].lWeaponId;
							iNumTypes++;
						}
					}
				}
			}
		}

	}

	// Account for world object stuff
	for (cnt=0; cnt<iNumWeaponTypes; cnt++)
	{
		iFound=0;
		for (int j=0; j<iNumTypes; j++)
		{
			if (g_WeaponTypes[cnt]==pTypes[j])
			{
				iFound++;
				break;
			}
		}
		if (!iFound)
		{
			if (iNumTypes<256)
			{
				pTypes[iNumTypes]=g_WeaponTypes[cnt];
				iNumTypes++;
			}
		}
	}

	if (iNumTypes)
	{
		pDBWeaponList = (DBWeaponType *)malloc(sizeof(DBWeaponType) * iNumTypes);
		iNumWeaponList=0;

		for (int i=0; i<iNumTypes; i++)
		{
			for (int j=0; j<g_iNumWeapon; j++)
			{
				if (pTypes[i]==pWeaponDB[j].lWeaponID)
				{
					pDBWeaponList[iNumWeaponList] = pWeaponDB[j];
					iNumWeaponList++;
				}
			}
		}
	}

	if (pTypes)
	{
		free(pTypes);
		pTypes=NULL;
	}

}

void LoadSubWeapons()
{
	int iTempNum = iNumWeaponList;
	for (int i=0; i<iTempNum; i++)
	{
		if (pDBWeaponList[i].lSubWeaponID)
		{
			RegisterWeapon(pDBWeaponList[i].lSubWeaponID);
		}
	}
}


void FixUpPlaneWeapons()
{
	int iTemplate;
	int iLoadOutIndex;
	int reargun;

	for (int i=0; i<iNumAIObjs; i++)
	{
		reargun = 0;
		if (pDBAircraftList[Planes[i].AI.iPlaneIndex].lLoadOutId!=0)
		{
			iTemplate = pDBAircraftList[Planes[i].AI.iPlaneIndex].lLoadOutId;
			iLoadOutIndex = AIObjects[i].lLoadOut;
			if (iLoadOutIndex<0)
				iLoadOutIndex=0;

			for (int k=0; ((k<pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].iNumLoadOut) && (k<MAX_F18E_STATIONS)); k++)
			{
				if (Planes[i].AI.lPlaneID == MBF18ETYPE)
				{
					Planes[i].AI.iAIFlags1 |= AI_INTERNAL_JAMMER;
				}

				if (k<MAX_F18E_STATIONS)
				{
					Planes[i].WeapLoad[k].WeapId = pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].lWeaponId;
					Planes[i].WeapLoad[k].Count = pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].iWeaponCount;
					Planes[i].WeapLoad[k].WeapIndex=RegisterWeapon(Planes[i].WeapLoad[k].WeapId);

					if ((Planes[i].WeapLoad[k].WeapId == 75) || (Planes[i].WeapLoad[k].WeapId == 76) || (Planes[i].WeapLoad[k].WeapId == 95))  //   || (Planes[i].WeapLoad[k].WeapId == 96) || (Planes[i].WeapLoad[k].WeapId == 97))
					{
						reargun = 1;
					}

					if ((Planes[i].WeapLoad[k].WeapId == 102) ||
						(Planes[i].WeapLoad[k].WeapId == 103) ||
						(Planes[i].WeapLoad[k].WeapId == 177) ||
						(Planes[i].WeapLoad[k].WeapId == 178))

					{
						Planes[i].AI.iAIFlags1 |= AI_INTERNAL_JAMMER;
					}
				}
			}
			if (Planes[i].AI.lPlaneID == MBF18ETYPE)
			{
				for (int k=0; (k<pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].iNumLoadOut); k++)
				{
					for (int j=0; (j<MAX_F18E_STATIONS); j++)
					{
						if (pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].iStation==j)
						{
							Planes[i].WeapLoad[j].WeapId = pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].lWeaponId;
							Planes[i].WeapLoad[j].Count = pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].iWeaponCount;
							Planes[i].WeapLoad[j].WeapIndex=RegisterWeapon(Planes[i].WeapLoad[k].WeapId);
						}
					}
				}
			}
		}
		if(!reargun)
		{
			Planes[i].AI.cInteriorDamage[AI_REAR_GUN] = -1;
		}
		Planes[i].AI.lAIVoice = AICRGetAIVoice(&Planes[i]);
	}

#if 0
	for (int i=0; i<iNumAIObjs; i++)
	{
		if (pDBAircraftList[Planes[i].AI.iPlaneIndex].lLoadOutId!=0)
		{
			iTemplate = pDBAircraftList[Planes[i].AI.iPlaneIndex].lLoadOutId;
			iLoadOutIndex = pDBAircraftList[Planes[i].AI.iPlaneIndex].lLoadOutIndex;

			for (int k=0; ((k<pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].iNumLoadOut) && (k<15)); k++)
			{
				Planes[i].WeapLoad[k].WeapId = pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].lWeaponId;
				Planes[i].WeapLoad[k].Count = pLoadOutDB[iTemplate].LoadOutTemplate[iLoadOutIndex].Loadouts[k].iWeaponCount;
				Planes[i].WeapLoad[k].WeapIndex=RegisterWeapon(Planes[i].WeapLoad[k].WeapId);
			}
		}
	}
#endif
}

int RegisterWeapon(long lWeaponId)
{

	for (int i=0; i<g_iNumWeapon; i++)
	{
		if (pWeaponDB[i].lWeaponID == lWeaponId)
		{
			for (int j=0; j<iNumWeaponList; j++)
			{
				if (pDBWeaponList[j].lWeaponID == lWeaponId)
				{
					return(j);
				}
			}
			// Need to add an additional weapon to pDBWeaponList

			pDBWeaponList = (DBWeaponType *)realloc(pDBWeaponList,(sizeof(DBWeaponType) * (iNumWeaponList+1)));
			pDBWeaponList[iNumWeaponList] = pWeaponDB[i];
			return(iNumWeaponList++);
		}
	}
	return(-1);
}
#if 0

			pDBWeaponList[iNumWeaponList] = pWeaponDB[i];
			iNumWeaponList++;
			return(iNumWeaponList-1);
		}
	}
	return(-1);
}
#endif

int RegisterAircraft(long lPlaneId)
{
	for (int i=0; i<g_iNumAircraft; i++)
	{
		if (pAircraftDB[i].lPlaneID == lPlaneId)
	 		{
			for (int j=0; j<iNumAircraftList; j++)
			{
				if (pDBAircraftList[j].lPlaneID == lPlaneId)
				{
					return(j);
				}
			}		// "pDBAircraftList" already init'd
		    //pDBAircraftList[iNumAircraftList] = pAircraftDB[i];
			//return(iNumAircraftList++);
		}
	}
	return(-1);
}


int RegisterRadar(long lRadarId)
{

	for (int i=0; i<g_iNumRadar; i++)
	{
		if (pRadarDB[i].lRadarID == lRadarId)
		{
			for (int j=0; j<iNumRadarList; j++)
			{
				if (pDBRadarList[j].lRadarID == lRadarId)
				{
					return(j);
				}
			}
			// Need to add an additional Radar to pDBRadarList

			pDBRadarList = (DBRadarType *)realloc(pDBRadarList,(sizeof(DBRadarType) * (iNumRadarList+1)));
			pDBRadarList[iNumRadarList] = pRadarDB[i];
			return(iNumRadarList++);
		}
	}
	return(-1);
}

extern int LoadWeaponModel(WeaponType *type);



//**************************************************************************************
WeaponType *IGLoadWeaponType(int typeidnum)
{
	int cnt;

	for(cnt = 0; cnt < MAX_WEAPON_TYPES; cnt ++)
	{
		if(WeaponTypes[cnt].TypeNumber == typeidnum)
			return(&WeaponTypes[cnt]);
		else if(WeaponTypes[cnt].TypeNumber == -1)
			break;
	}
	if(cnt == MAX_WEAPON_TYPES)
	{
		return(&WeaponTypes[cnt -1]);
	}
	WeaponTypes[cnt].TypeNumber = typeidnum;

	LoadWeaponModel(&WeaponTypes[cnt]);

	return(&WeaponTypes[cnt]);
}

//**************************************************************************************
void LoadWeaponObjects()
{
	// This needs to be moved, but because I could not
	// get my hands on simmain.cpp I put this here

	LoadSubWeapons();

	for(int cnt = 0; cnt < MAX_WEAPON_TYPES; cnt ++)
	{
		WeaponTypes[cnt].TypeNumber =-1;
	}

	for (int i=0; i<iNumWeaponList; i++)
	{
		IGLoadWeaponType(pDBWeaponList[i].lWeaponID);
	}

	for (i=0; i<iNumAIObjs; i++)
	{
		for (int j=0; j<15; j++)
		{
			Planes[i].WeapLoad[j].Type = IGLoadWeaponType(Planes[i].WeapLoad[j].WeapId);
		}
	}
}

//**************************************************************************************
void LoadWorldDBNeeds()
{
	// Okay, this really sucks, but right now, its what I am going
	// to do to get this done.  I am going to scan all of the ground objects
	// in the world and record if they are a weapon or radar.
	// I will then save this information in global form to be latter consumed
	// by UpdateWorldDBNeeds()

	GenericInstanceDef *pWalker;
	int iRadarType=-1;
	int iWeaponType=-1;
	int iFound=0;
	memset(g_RadarTypes,0,sizeof(int)*256);
	iNumRadarTypes=0;
	memset(g_WeaponTypes,0,sizeof(int)*256);
	iNumWeaponTypes=0;


	pWalker = WorldInstanceDefs;

	while (pWalker)
	{
		iFound=0;
		iRadarType=GetRadarIDFromDef(pWalker);
		if ((iRadarType!=-1) && (iRadarType))
		{
			for (int j=0; j<iNumRadarTypes; j++)
			{
				if (g_RadarTypes[j]==iRadarType)
				{
					iFound++;
					break;
				}
			}
			if (!iFound)
			{
				if (iNumRadarTypes<256)
				{
					g_RadarTypes[iNumRadarTypes]=iRadarType;
					iNumRadarTypes++;
				}
			}
		}
		iFound=0;
//		if (pWalker->Basics.Family == FAMILY_AAWEAPON)
//			iWeaponType = ((AAWeaponFamilyDef *)pWalker->Basics.Type)->WeaponID;

//		iWeaponType =
		iWeaponType = GetWeaponIDFromDef(pWalker);
		if ((iWeaponType!=-1) && (iWeaponType))
		{
			for (int j=0; j<iNumWeaponTypes; j++)
			{
				if (g_WeaponTypes[j]==iWeaponType)
				{
					iFound++;
					break;
				}
			}
			if (!iFound)
			{
				if (iNumWeaponTypes<256)
				{
					g_WeaponTypes[iNumWeaponTypes]=iWeaponType;
					iNumWeaponTypes++;
				}
			}
		}

		MultipleExplosionTypeDef *pMultiExplosion;
		int iNumSecondary=0;

		pMultiExplosion = GetSecondariesFromDef(pWalker,&iNumSecondary);

		while (iNumSecondary)
		{
			iWeaponType = pMultiExplosion->WarheadID;
			if ((iWeaponType!=-1) && (iWeaponType))
			{
				iFound=0;
				for (int j=0; j<iNumWeaponTypes; j++)
				{		
					if (g_WeaponTypes[j]==iWeaponType)
					{
						iFound++;
						break;
					}
				}
				if (!iFound)
				{
					if (iNumWeaponTypes<256)
					{
						g_WeaponTypes[iNumWeaponTypes]=iWeaponType;
						iNumWeaponTypes++;
					}
				}
			}
			iNumSecondary--;
			pMultiExplosion++;
		}
		pWalker = (GenericInstanceDef *)pWalker->Basics.NextInstance;
	}
}

extern int LoadVehicleModel(MovingVehicleType *type);
extern int LoadDestroyedVehicleModel(MovingVehicleType *type);

extern int LoadShipModel(MovingVehicleType *type);
extern int LoadDestroyedShipModel(MovingVehicleType *type);


//**************************************************************************************
MovingVehicleType *IGLoadMovingVehicleType(int typeidnum,int iShipType,int iSizeType)
{
	int cnt;

	for(cnt = 0; cnt < MAX_MOVINGVEHICLE_TYPES; cnt ++)
	{
		if ((MovingVehicleTypeList[cnt].TypeNumber == typeidnum) && (MovingVehicleTypeList[cnt].iShipType == iShipType))
			return(&MovingVehicleTypeList[cnt]);
		else if(MovingVehicleTypeList[cnt].TypeNumber == -1)
			break;
	}
	if(cnt == MAX_MOVINGVEHICLE_TYPES)
	{
		return(&MovingVehicleTypeList[cnt -1]);
	}

	MovingVehicleTypeList[cnt].TypeNumber = typeidnum;
	MovingVehicleTypeList[cnt].iShipType = iShipType;
	MovingVehicleTypeList[cnt].iSizeType = iSizeType;

	if (!iShipType)
		LoadVehicleModel(&MovingVehicleTypeList[cnt]);
	else
		LoadShipModel(&MovingVehicleTypeList[cnt]);

	return(&MovingVehicleTypeList[cnt]);
}

//**************************************************************************************
MovingVehicleType *IGLoadDestroyedMovingVehicleType(int typeidnum,int iShipType,int iSizeType)
{
	int cnt;
	int skipfirst = 0;

	for(cnt = 0; cnt < MAX_MOVINGVEHICLE_TYPES; cnt ++)
	{
		if ((MovingVehicleTypeList[cnt].TypeNumber == typeidnum) && (MovingVehicleTypeList[cnt].iShipType == iShipType))
		{
			if(skipfirst)
			{
				return(&MovingVehicleTypeList[cnt]);
			}
			skipfirst = 1;
		}
		else if(MovingVehicleTypeList[cnt].TypeNumber == -1)
			break;
	}
	if(cnt == MAX_MOVINGVEHICLE_TYPES)
	{
		return(&MovingVehicleTypeList[cnt -1]);
	}
	MovingVehicleTypeList[cnt].TypeNumber = typeidnum;
	MovingVehicleTypeList[cnt].iShipType = iShipType;
	MovingVehicleTypeList[cnt].iSizeType = iSizeType;

	if (!iShipType)
		LoadDestroyedVehicleModel(&MovingVehicleTypeList[cnt]);
	else
		LoadDestroyedShipModel(&MovingVehicleTypeList[cnt]);

	return(&MovingVehicleTypeList[cnt]);
}

//**************************************************************************************
MovingVehicleParams *VGetStartingLeader(int vehiclenum, int *offsetcnt, int mvnum)
{
	int leader;
	int existcnt = 1;
	int vnum;

	leader = g_pMovingVehicleList[vehiclenum].iLeader;

	if((leader < 0) || (leader > (g_iNumMovingVehicle)))
	{
		return(NULL);
	}

	vnum = 0;
	for(int cnt3 = 0; cnt3 < leader; cnt3 ++)
	{
		vnum += g_pMovingVehicleList[cnt3].iNumGroup;
	}

	leader = vnum;

	if((leader < 0) || (leader == mvnum))
	{
		return(NULL);
	}

	vnum = leader;
	while(MovingVehicles[vnum].pLeadVehicle)
	{
		vnum = (MovingVehicleParams *)MovingVehicles[vnum].pLeadVehicle - MovingVehicles;
		if(vnum == vehiclenum)  //  we have a circle
		{
			return(NULL);
		}
		existcnt ++;
	}

	vnum = leader;

	while(MovingVehicles[vnum].pTrailVehicle)
	{
		existcnt ++;
		vnum = (MovingVehicleParams *)MovingVehicles[vnum].pTrailVehicle - MovingVehicles;
	}

	*offsetcnt = existcnt;
	return(&MovingVehicles[vnum]);
}

//**************************************************************************************
int VConvertVNumToGVListNum(int vnum)
{
	int cnt = 0;

	for (int i=0; i<g_iNumMovingVehicle; i++)
	{
		cnt += g_pMovingVehicleList[i].iNumGroup;
		if(vnum < cnt)
		{
			return(i);
		}

	}
	return(g_iNumMovingVehicle - 1);
}

//**************************************************************************************
void LoadMovingVehicleInfo()
{
	int *pTypes = (int *)malloc(sizeof(int)*MAX_MOVINGVEHICLE_TYPES);
	int iNumTypes=0;
	int iFound=0;
	int vehiclecnt = 0;
	float heading;
	float workx, workz, workval;
	double radhead;
	int done;
	float speedfps;
	int speedtype;
	int offsetcnt;
	float vehiclespacing;
	int iworkvar;
	int lead_gvlistnum;
	int fxcnt;


	memset(pTypes,0,(sizeof(int)*MAX_MOVINGVEHICLE_TYPES));

	// Check for Vehicles (not ships)
	for (int i=0; i<g_iNumMovingVehicle; i++)
	{
		iFound=0;

		if (!(g_pMovingVehicleList[i].iFlags & VEHICLE_SHIP_TYPE))
		{
			for (int j=0; j<iNumTypes; j++)
			{
				if (pTypes[j]==g_pMovingVehicleList[i].iType)
				{
					iFound++;
					break;
				}
			}
			if (!iFound)
			{
				if (iNumTypes<MAX_MOVINGVEHICLE_TYPES)
				{
					pTypes[iNumTypes]=g_pMovingVehicleList[i].iType;
					iNumTypes++;
				}
			}
		}
	}


	// Setup database list for vehicles (not ships)
	iNumVehicleList=0;
	if (iNumTypes)
	{
		pDBVehicleList = (DBVehicleType *)malloc(sizeof(DBVehicleType) * iNumTypes);

		for (int i=0; i<iNumTypes; i++)
		{
			for (int j=0; j<g_iNumVehicle; j++)
			{
				if (pTypes[i]==pVehicleDB[j].lVehicleID) // If my type is in the database, then remember...
				{
					pDBVehicleList[iNumVehicleList] = pVehicleDB[j];
					iNumVehicleList++;
				}
			}
		}
	}

	// Load Weapons, radars, and exp. types for vehicles (not ships)
	for (i=0; i<iNumVehicleList; i++)
	{
		RegisterWeapon(pDBVehicleList[i].lWeaponType);
		RegisterWeapon(pDBVehicleList[i].lExpWeaponType);
		RegisterRadar(pDBVehicleList[i].lRadarID);

		// New - Second weapon type
		RegisterWeapon(pDBVehicleList[i].lWeaponType2);
	}

	if (pTypes)
	{
		free(pTypes);
		pTypes=NULL;
	}


	iNumTypes=0;
	pTypes = (int *)malloc(sizeof(int)*MAX_MOVINGVEHICLE_TYPES);
	memset(pTypes,0,(sizeof(int)*MAX_MOVINGVEHICLE_TYPES));

	// Check for Ship (not vehicles)

	for (i=0; i<g_iNumMovingVehicle; i++)
	{
		iFound=0;

		if (g_pMovingVehicleList[i].iFlags & VEHICLE_SHIP_TYPE)
		{
			for (int j=0; j<iNumTypes; j++)
			{
				if (pTypes[j]==g_pMovingVehicleList[i].iType)
				{
					iFound++;
					break;
				}
			}
			if (!iFound)
			{
				if (iNumTypes<MAX_MOVINGVEHICLE_TYPES)
				{
					pTypes[iNumTypes]=g_pMovingVehicleList[i].iType;
					iNumTypes++;
				}
			}
		}
	}


	// Setup database list for Ship (not vehicles)
	iNumShipList=0;
	if (iNumTypes)
	{
		pDBShipList = (DBShipType *)malloc(sizeof(DBShipType) * iNumTypes);

		for (int i=0; i<iNumTypes; i++)
		{
			for (int j=0; j<g_iNumShip; j++)
			{
				if (pTypes[i]==pShipDB[j].lShipID) // If my type is in the database, then remember...
				{
					pDBShipList[iNumShipList] = pShipDB[j];
					iNumShipList++;
				}
			}
		}
	}

	// Load Weapons, radars, and exp. types for Ship (not ships)
	for (i=0; i<iNumShipList; i++)
	{
		for (int j=0; j<pDBShipList[i].iNumDefenseItems; j++)
		{

			if (!pDBShipList[i].DefenseList[j].lDefenseType)
				RegisterWeapon(pDBShipList[i].DefenseList[j].lTypeID);
			else
				RegisterRadar(pDBShipList[i].DefenseList[j].lTypeID);
		}
	}

	if (pTypes)
	{
		free(pTypes);
		pTypes=NULL;
	}


	for(int cnt = 0; cnt < MAX_MOVINGVEHICLE_TYPES; cnt ++)
	{
		MovingVehicleTypeList[cnt].TypeNumber =-1;
	}

	LastMovingVehicle = NULL;

	i = 0;
	for (cnt=0; cnt<g_iNumMovingVehicle; cnt++)
	{
		done = 0;
		offsetcnt = 0;
		while(vehiclecnt || (!done))
		{
			done = 1;
			MovingVehicles[i].Status = VL_ACTIVE|VL_MOVING;
			MovingVehicles[i].iType = g_pMovingVehicleList[cnt].iType;

			if (g_pMovingVehicleList[cnt].iFlags & VEHICLE_SHIP_TYPE)
			{
				MovingVehicles[i].iVDBIndex = GetShipIndex(MovingVehicles[i].iType);
				MovingVehicles[i].iShipType = pDBShipList[MovingVehicles[i].iVDBIndex].lShipType;
				MovingVehicles[i].iVehicleType=0;		// I am a ship
				MovingVehicles[i].Type = IGLoadMovingVehicleType(MovingVehicles[i].iType,MovingVehicles[i].iShipType,pDBShipList[MovingVehicles[i].iVDBIndex].lSizeType);
				MovingVehicles[i].DestroyedType = IGLoadDestroyedMovingVehicleType(MovingVehicles[i].iType,MovingVehicles[i].iShipType,pDBShipList[MovingVehicles[i].iVDBIndex].lSizeType);
			} else {
				MovingVehicles[i].iVDBIndex = GetMovingVehicleIndex(MovingVehicles[i].iType);
				MovingVehicles[i].iVehicleType = pDBVehicleList[MovingVehicles[i].iVDBIndex].lVehicleType;
				MovingVehicles[i].iShipType=0;		// I am a moving vehicle
				MovingVehicles[i].Type = IGLoadMovingVehicleType(MovingVehicles[i].iType,MovingVehicles[i].iShipType,0);
				MovingVehicles[i].DestroyedType = IGLoadDestroyedMovingVehicleType(MovingVehicles[i].iType,MovingVehicles[i].iShipType,0);
			}

			MovingVehicles[i].iVListID = cnt;
			MovingVehicles[i].iSide = g_pMovingVehicleList[cnt].iSide;

			MovingVehicles[i].pStartWP = g_pMovingVehicleList[cnt].pRoutePointList;
			MovingVehicles[i].pCurrWP = g_pMovingVehicleList[cnt].pRoutePointList;
			MovingVehicles[i].numwaypts = g_pMovingVehicleList[cnt].iNumPoints;
			heading = VDetermineNextWayPoint(MovingVehicles[i].pCurrWP, MovingVehicles[i].numwaypts, 0, MovingVehicles[i].pStartWP, &MovingVehicles[i].WayPosition, 1);
			MovingVehicles[i].Heading = AIConvert180DegreeToAngle(heading);
			MovingVehicles[i].Pitch = 0;
	  		MovingVehicles[i].Attitude.SetHPR (MovingVehicles[i].Heading, MovingVehicles[i].Pitch, 0);
			MovingVehicles[i].fSinPitch = 0.0f;
			MovingVehicles[i].fSubmergeDepth = 0.0f;
			MovingVehicles[i].listroll = 0.0f;
			MovingVehicles[i].lNetTimer = 30000 + (i * 5000);
			for(fxcnt = 0; fxcnt < 6; fxcnt ++)
			{
				if(fxcnt < 2)
				{
					MovingVehicles[i].bFXArray[fxcnt] = 128;
					MovingVehicles[i].bFXDesiredArray[fxcnt] = 128;
				}
				else
				{
					MovingVehicles[i].bFXArray[fxcnt] = 0;
					MovingVehicles[i].bFXDesiredArray[fxcnt] = 0;
				}
			}

			workx = MovingVehicles[i].pCurrWP->lX;
			workz = MovingVehicles[i].pCurrWP->lZ;
			heading = heading - 90;

			while(heading > 180)
				heading -= 360;

			while(heading < -180)
				heading += 360;

			radhead = DegToRad(heading);

			workx -= (sin(radhead) * LANE_MIDDLE);
			workz -= (cos(radhead) * LANE_MIDDLE);

			MovingVehicles[i].WorldPosition.X = ConvertWayLoc(workx);
			MovingVehicles[i].WorldPosition.Y = 0;
			MovingVehicles[i].WorldPosition.Z = ConvertWayLoc(workz);

			MovingVehicles[i].iAnimationValue = 0;
			if(MovingVehicles[i].Type->TypeNumber == 4)
			{
				MovingVehicles[i].iAnimationWork = 500 + (rand() & 1023);
			}
			else if(MovingVehicles[i].Type->TypeNumber == 39)
			{
				MovingVehicles[i].iAnimationValue = 255;
				MovingVehicles[i].iAnimationWork = 0;
			}


			if(!vehiclecnt)
			{
				vehiclecnt = g_pMovingVehicleList[cnt].iNumGroup;
//				MovingVehicles[i].pLeadVehicle = NULL;
				MovingVehicles[i].pLeadVehicle = VGetStartingLeader(cnt, &offsetcnt, i);
				if(MovingVehicles[i].pLeadVehicle)
				{
					iworkvar = ((MovingVehicleParams *)MovingVehicles[i].pLeadVehicle - MovingVehicles);
					MovingVehicles[iworkvar].pTrailVehicle = &MovingVehicles[i];
				}

				if(MovingVehicles[i].pTrailVehicle)
				{
					iworkvar = ((MovingVehicleParams *)MovingVehicles[i].pTrailVehicle - MovingVehicles);
					if((iworkvar < 0) || (iworkvar >= g_iNumMovingVehicle) || (MovingVehicles[iworkvar].pLeadVehicle != &MovingVehicles[i]))
					{
						MovingVehicles[i].pTrailVehicle = NULL;
					}
				}
				else
					MovingVehicles[i].pTrailVehicle = NULL;

				if(!MovingVehicles[i].iShipType)
				{
					speedtype = pDBVehicleList[MovingVehicles[i].iVDBIndex].iSpeed;

					if(speedtype & VEHICLE_SPEED_VSLOW)
					{
						speedfps = 7.333333f;		//  5 mph
					}
					else if(speedtype & VEHICLE_SPEED_SLOW)
					{
						speedfps = 36.66667f;		//  25 mph
					}
					else if(speedtype & VEHICLE_SPEED_NORMAL)
					{
						speedfps = 58.66667f;		//  40 mph
					}
					else if(speedtype & VEHICLE_SPEED_FAST)
					{
						speedfps = 88.0f;		//  60 mph
					}
					else if(speedtype & VEHICLE_SPEED_VFAST)
					{
						speedfps = 117.33333f;		//  80 mph
					}
					else
					{
						speedfps = 58.66667f;		//  40 mph
					}
				}
				else
				{
					speedfps = (float)pDBShipList[MovingVehicles[i].iVDBIndex].iMaxSpeed / FTSEC_TO_KNOTS;
				}
				MovingVehicles[i].fWorkingFPS = MovingVehicles[i].fSpeedFPS = speedfps;

				vehiclespacing = GetObjectRadius(MovingVehicles[i].Type->Model) * 4.0f * WUTOFT;
				if(vehiclespacing < VEHICLE_SPACING)
				{
					vehiclespacing = VEHICLE_SPACING;
				}

				MovingVehicles[i].fVehicleSpacing = vehiclespacing;
				MovingVehicles[i].WayPosition = MovingVehicles[i].WorldPosition;
			}
			else
			{
				if(MovingVehicles[i - 1].pTrailVehicle)
				{
					iworkvar = ((MovingVehicleParams *)MovingVehicles[i - 1].pTrailVehicle - MovingVehicles);
					if((iworkvar > 0) && (iworkvar < g_iNumMovingVehicle) && (MovingVehicles[iworkvar].pLeadVehicle == &MovingVehicles[i - 1]))
					{
						MovingVehicles[iworkvar].pLeadVehicle = &MovingVehicles[i];
						MovingVehicles[i].pTrailVehicle = &MovingVehicles[iworkvar];
					}
					else
					{
						MovingVehicles[i].pTrailVehicle = NULL;
					}
				}
				else
				{
					MovingVehicles[i].pTrailVehicle = NULL;
				}

				MovingVehicles[i].pLeadVehicle = &MovingVehicles[i - 1];
				MovingVehicles[i - 1].pTrailVehicle = &MovingVehicles[i];
				MovingVehicles[i].fWorkingFPS = MovingVehicles[i].fSpeedFPS = speedfps;
			}

			if(MovingVehicles[i].pLeadVehicle)
			{
				iworkvar = ((MovingVehicleParams *)MovingVehicles[i].pLeadVehicle - MovingVehicles);

				MovingVehicles[i].pStartWP = MovingVehicles[iworkvar].pStartWP;
				MovingVehicles[i].pCurrWP = MovingVehicles[iworkvar].pCurrWP;
				MovingVehicles[i].numwaypts = MovingVehicles[iworkvar].numwaypts;
				if(!MovingVehicles[i].pCurrWP)
				{
					lead_gvlistnum = VConvertVNumToGVListNum(iworkvar);
					MovingVehicles[i].pStartWP = g_pMovingVehicleList[lead_gvlistnum].pRoutePointList;
					MovingVehicles[i].pCurrWP = g_pMovingVehicleList[lead_gvlistnum].pRoutePointList;
					MovingVehicles[i].numwaypts = g_pMovingVehicleList[lead_gvlistnum].iNumPoints;
				}
				heading = VDetermineNextWayPoint(MovingVehicles[i].pCurrWP, MovingVehicles[i].numwaypts, 0, MovingVehicles[i].pStartWP, &MovingVehicles[i].WayPosition, 1);
				MovingVehicles[i].Heading = AIConvert180DegreeToAngle(heading);
				MovingVehicles[i].Pitch = 0;
	  			MovingVehicles[i].Attitude.SetHPR (MovingVehicles[i].Heading, MovingVehicles[i].Pitch, 0);
				MovingVehicles[i].fSinPitch = 0.0f;


				workx = MovingVehicles[iworkvar].WorldPosition.X * WUTOFT;
				workz = MovingVehicles[iworkvar].WorldPosition.Z * WUTOFT;

				heading = heading - 90;

				while(heading > 180)
					heading -= 360;

				while(heading < -180)
					heading += 360;

				radhead = DegToRad(heading);

				workx -= (sin(radhead) * LANE_MIDDLE);
				workz -= (cos(radhead) * LANE_MIDDLE);

				heading = heading - 90;

				while(heading > 180)
					heading -= 360;

				while(heading < -180)
					heading += 360;

				radhead = DegToRad(heading);

				vehiclespacing = GetObjectRadius(MovingVehicles[i].Type->Model) * 4.0f * WUTOFT;
				if(vehiclespacing < VEHICLE_SPACING)
				{
					vehiclespacing = VEHICLE_SPACING;
				}

//				workval = (sin(radhead) * vehiclespacing * offsetcnt);
				workval = (sin(radhead) * vehiclespacing);
				workx -= workval;
//				workval = (cos(radhead) * vehiclespacing * offsetcnt);
				workval = (cos(radhead) * vehiclespacing);
				workz -= workval;
				MovingVehicles[i].fVehicleSpacing = vehiclespacing;

				MovingVehicles[i].WorldPosition.X = ConvertWayLoc(workx);
				MovingVehicles[i].WorldPosition.Y = 0;
				MovingVehicles[i].WorldPosition.Z = ConvertWayLoc(workz);
				MovingVehicles[i].WayPosition = MovingVehicles[iworkvar].WayPosition;

				lead_gvlistnum = VConvertVNumToGVListNum(iworkvar);
			}
			else
			{
				lead_gvlistnum = cnt;
			}

			vehiclecnt --;
			offsetcnt ++;

			if(g_pMovingVehicleList[lead_gvlistnum].iStartingCondition == 0)
			{
				MovingVehicles[i].lAITimer1 = g_pMovingVehicleList[lead_gvlistnum].iStartingTime * 60000;
				if(MovingVehicles[i].lAITimer1)
				{
					MovingVehicles[i].Status |= VL_WAITING;
					MovingVehicles[i].Status &= ~(VL_CHECK_FLAGS|VL_MOVING);
				}
				else
				{
					MovingVehicles[i].lAITimer1 = -1;
				}
			}
			else
			{
				if(!AICheckEventFlagStatus(g_pMovingVehicleList[lead_gvlistnum].iStartingFlag))
				{
					MovingVehicles[i].Status |= (VL_WAITING|VL_CHECK_FLAGS);
					MovingVehicles[i].Status &= ~(VL_MOVING);
					MovingVehicles[i].iCheckFlag = g_pMovingVehicleList[lead_gvlistnum].iStartingFlag;
				}
			}

			if(g_pMovingVehicleList[lead_gvlistnum].iOtherStartFlags)
			{
				MovingVehicles[i].Status |= VL_INVISIBLE;
			}

			GetHeightWithInstances(MovingVehicles[i].WorldPosition,AllFoundations);

			LastMovingVehicle = &MovingVehicles[i];

			i++;
		}
	}

	VSetUpInitialFormations();
}

int GetMovingVehicleIndex(long lVehicleID)
{
	for (int i=0; i<iNumVehicleList; i++)
	{
		if (pDBVehicleList[i].lVehicleID == lVehicleID)
		{
			return(i);
		}
	}
	return(-1);
}

int GetShipIndex(long lShipID)
{
	for (int i=0; i<iNumShipList; i++)
	{
		if (pDBShipList[i].lShipID == lShipID)
		{
			return(i);
		}
	}
	return(-1);
}


