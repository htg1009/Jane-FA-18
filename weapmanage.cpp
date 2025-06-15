 #include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"
#include "Resources.h"
#include "SkunkFF.h"

#define ASSERT
//*****************************************************************************************************************************************
// WEAPON MANAGE  -- *Header -- (search "*Header" to step through each section of file)
//*****************************************************************************************************************************************


//*****************************************************************************************************************************************
// EXTERNS  -- *Header -- (search "*Header" to step through each section of file)
//*****************************************************************************************************************************************

extern int g_nMissionType; // from WrapInterface.h (SIM_QUICK, SIM_SINGLE, SIM_TRAIN...)
extern AvionicsType Av;
extern WeapStoresType WeapStores;
long lFFBaseGMag;

extern DetectedPlaneListType CurFramePlanes;
extern DetectedPlaneListType PrevFramePlanes;
extern TargetInfoType *Primary;

int Aim9X_Id;
int Aim9L_Id;
int Aim9M_Id;
int Aim7F_Id;
int Aim7M_Id;
int Aim7MH_Id;
int Aim120_Id;

void SelectNextAAWeaponType();
void SelectNextAAWeapon();
extern DWORD g_dwAIMSndHandle;
extern DWORD g_dwAIMLockSndHandle;
extern void KillSound( DWORD *SndHandle );

//*****************************************************************************************************************************************
// WEAPONS SETUP FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int GetModeForAAWeap(int WeapId)
{
	if( (WeapId == Aim9X_Id) || (WeapId == Aim9L_Id) || (WeapId == Aim9M_Id) )
		 return(AIM9_STAT);
	else if( (WeapId == Aim7F_Id) || (WeapId == Aim7M_Id) || (WeapId == Aim7MH_Id) )
		 return(AIM7_STAT);
	else if(WeapId == Aim120_Id)
		 return(AIM120_STAT);

	return(NO_WEAP_STAT);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsValidWeap(WeaponLoadoutInfoType *W)
{
	// list in switch are all those "weaps" that are not really weaps such as fuel. Add to list any that fit this category
	int Type = pDBWeaponList[W->WeapIndex].iWeaponType;

	switch(Type)
	{
		case FUEL_TANK:
		case DATA_LINK: return(FALSE);
	}

	return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsWeapPod(int Id)
{
	int AAR50ID = GetWeapId(AAR50_ID);
	int AAS3ID  = GetWeapId(AAS38A_ID);
	int AWW13ID = GetWeapId(AWW13_ID);

	return( (Id == AAR50ID) || (Id == AAS3ID) || (Id == AWW13ID) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetWeapPageForWeapId(int WeapId,int Type)
{
	if( (WeapId == GetWeapId(LAU10_ID)) || (WeapId == GetWeapId(LAU68_ID)) )
		return(ROCKET_POD_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM65E_ID))
		return(AGM_65E_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM65F_ID))
		return(AGM_65F_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM65G_ID))
		return(AGM_65_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM88_ID))
		return(HARM_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM84A_ID))
		return(HARPOON_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM84H_ID))
		return(SLAMER_WEAP_PAGE);
	else if(WeapId == GetWeapId(AGM62_ID))
		return(WALLEYE_WEAP_PAGE);
	else if( (WeapId == GetWeapId(AGM154A_ID)) || (WeapId == GetWeapId(AGM154C_ID)) )
		return(JSOW_WEAP_PAGE);
	else if( (WeapId == GetWeapId(GBU29_ID)) || (WeapId == GetWeapId(GBU30_ID)) || (WeapId == GetWeapId(GBU31_ID))  )
		return(JDAM_WEAP_PAGE);
	else if(WeapId == GetWeapId(ADM141_ID))
		return(TALD_WEAP_PAGE);
	else if(Type == CLUSTER)
		return(CLUSTER_WEAP_PAGE);
	else if( (Type == DUMB_AG) || (Type == GUIDED_AG) || (WeapId == GetWeapId(AGM123_ID)) || (WeapId == GetWeapId(MK60_ID)) || (WeapId == GetWeapId(MK65_ID)) )
		return(DUMB_WEAP_PAGE);
	else
	{
		int AAMode = GetModeForAAWeap(WeapId);
		if(AAMode == AIM9_STAT)
			return(AIM9_WEAP_PAGE);
		else if(AAMode == AIM7_STAT)
			return(AIM7_WEAP_PAGE);
		else if(AAMode == AIM120_STAT)
			return(AIM120_WEAP_PAGE);

		ASSERT(TRUE); // there should be a category for every weapon
	}

	return(-1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddWeapToList(WeaponLoadoutInfoType *W,int UseAA)
{
 	AvWeapEntryType *Walk;
	int *NumPtr;

	if(UseAA == 3)  // 3 is for the sequence list, should change to use a #define
	{
	 Walk   = &Av.Weapons.AASeqList[0];
	 NumPtr = &Av.Weapons.NumAASeqWeapons;
 	}
	else
	{
	  Walk   = (UseAA) ? (&Av.Weapons.AAList[0]) : (&Av.Weapons.AGList[0]);
	  NumPtr = (UseAA) ? (&Av.Weapons.NumAAWeapons) : (&Av.Weapons.NumAGWeapons);
	}

	int Found = FALSE;
  int InStation = W - &PlayerPlane->WeapLoad[0];

	// make sures its a valid weap.
	if( !IsValidWeap(W) ) return;

	int Index = *NumPtr;
	while (Index-- > 0)
	{
		int WalkStation = Walk->W - &PlayerPlane->WeapLoad[0];

		if(InStation == WalkStation)
		{
			Found = TRUE;
			break;
		}
		Walk++;
	}

	if(Found) return;

	*NumPtr += 1;
	if(UseAA == 3)
		Walk = &Av.Weapons.AASeqList[*NumPtr - 1];
	else
    Walk = (UseAA) ? (&Av.Weapons.AAList[*NumPtr - 1]) : (&Av.Weapons.AGList[*NumPtr - 1]);
  Walk->W = W;
	Walk->Selected = FALSE;
	Walk->Category = pDBWeaponList[W->WeapIndex].iWeaponType;
	Walk->Station  = W - &PlayerPlane->WeapLoad[0];
	Walk->WeapPage = GetWeapPageForWeapId(W->WeapId,pDBWeaponList[W->WeapIndex].iWeaponType);

 //	char ModelName[50];
 //	strcpy(ModelName,pDBWeaponList[W->WeapIndex].sObjectFileName);
 //	strcat(ModelName,".3dg");
 //	Walk->Model = Load3DObject( RegPath("objects",ModelName) );     // JLM no longer loading weap models for casual mode
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitAASequenceList()
{
 	int FireSeq[11] = {LEFT1_STATION,RIGHT11_STATION,LEFT2_STATION,RIGHT10_STATION,LEFT3_STATION,RIGHT9_STATION,LEFT4_STATION,RIGHT8_STATION,LEFT5_STATION,RIGHT7_STATION,CENTER6_STATION};

	int *Walk = &FireSeq[0];
	int Index = 11;
	while(Index-- > 0)
	{
		if(PlayerPlane->WeapLoad[*Walk].Status == AIM9_STAT)
		{
			AddWeapToList(&PlayerPlane->WeapLoad[*Walk],3);
		}
		Walk++;
  }

  Walk = &FireSeq[0];
	Index = 11;
	while(Index-- > 0)
	{
 		if(PlayerPlane->WeapLoad[*Walk].Status == AIM7_STAT)
		{
			AddWeapToList(&PlayerPlane->WeapLoad[*Walk],3);
		}
		Walk++;
 	}

  Walk = &FireSeq[0];
	Index = 11;
	while(Index-- > 0)
	{
 		if(PlayerPlane->WeapLoad[*Walk].Status == AIM120_STAT)
		{
			AddWeapToList(&PlayerPlane->WeapLoad[*Walk],3);
		}
		Walk++;
 	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitBombStations()
{
	AvWeapEntryType *W = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	int PrevId = -1;
	int Button = 19;

	Av.Weapons.BombStations.Left = NULL;
	Av.Weapons.BombStations.FrontLeft = NULL;
  Av.Weapons.BombStations.Center = NULL;
	Av.Weapons.BombStations.Right = NULL;
	Av.Weapons.BombStations.FrontRight = NULL;

	while(W <= LastW)
	{
		if( (W->W->Count > 0) && (W->W->Status == AIR_GROUND_STAT) && (W->W->WeapId != PrevId))

		if(Av.Weapons.BombStations.Left == NULL)
			 Av.Weapons.BombStations.Left = W;
		else if(Av.Weapons.BombStations.FrontLeft == NULL)
			 Av.Weapons.BombStations.FrontLeft = W;
		else if(Av.Weapons.BombStations.Center == NULL)
			 Av.Weapons.BombStations.Center = W;
		else if(Av.Weapons.BombStations.FrontRight == NULL)
			 Av.Weapons.BombStations.FrontRight = W;
		else if(Av.Weapons.BombStations.Right == NULL)
			 Av.Weapons.BombStations.Right = W;

		PrevId = W->W->WeapId;

		W++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetBombingAttributes(int Qty, int Mult,int Interval,int DetonateHeight)
{
	if( (Mult == 1) && (Qty == 1) )
	  Av.Weapons.Bomb.BombMethod = SNGL_BMB;
	else if( (Mult == 1) && (Qty > 1) )
	  Av.Weapons.Bomb.BombMethod = SNGL_RPL_BMB;
	else if( (Qty > Mult) && (Mult > 1) )
	  Av.Weapons.Bomb.BombMethod = MLT_RPL_BMB;

	Av.Weapons.Bomb.BombInterval = Interval;
	Av.Weapons.Bomb.NumRplBmbs = Qty;
	Av.Weapons.Bomb.CbuDetonateAltIndex = DetonateHeight;
	Av.Weapons.Bomb.Drop.Activate = FALSE;
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitBombingAttributes()
{
	Av.Weapons.Bomb.BombMethod = SNGL_BMB;
	Av.Weapons.Bomb.BombInterval = 320;
	Av.Weapons.Bomb.NumRplBmbs = 1;
	Av.Weapons.Bomb.CbuDetonateAltIndex = 2;
	Av.Weapons.Bomb.Drop.Activate = FALSE;
	Av.Weapons.Bomb.Rocket.Activate = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitWeaponsLoadout()
{
 	int BombSeq[10] = {LEFT1_STATION,RIGHT11_STATION,LEFT2_STATION,RIGHT10_STATION,LEFT3_STATION,RIGHT9_STATION,LEFT4_STATION,RIGHT8_STATION,RIGHT7_STATION,CENTER6_STATION};

	int WeapIndex;

	Av.Weapons.NumAAWeapons = 0;
	Av.Weapons.NumAASeqWeapons = 0;
	Av.Weapons.NumAGWeapons = 0;
	Av.Weapons.UseAASeq     = TRUE;
	Av.Weapons.CurAAWeap    = NULL;
	Av.Weapons.CurAASeqWeap = NULL;
	Av.Weapons.CurAGWeap    = NULL;

	 // create AG
 	for(int i=0; i<10; i++)
	{
		if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
		{
			WeapIndex = PlayerPlane->WeapLoad[i].WeapIndex;
			int *B = &BombSeq[0];
			for(int j=0; j<10; j++)
			{
				if(PlayerPlane->WeapLoad[*B].WeapIndex == WeapIndex)
				{
					if(PlayerPlane->WeapLoad[*B].Status == AIR_GROUND_STAT)
					  AddWeapToList(&PlayerPlane->WeapLoad[*B],FALSE);
				}
				B++;
			}
		}
	}

	// create AA SRM  AIM9
 	for(i=0; i<AV_MAX_WEAPONS; i++)
	{
		if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
		{
			WeapIndex = PlayerPlane->WeapLoad[i].WeapIndex;
			for(int j=0; j<AV_MAX_WEAPONS; j++)
			{
				if(PlayerPlane->WeapLoad[j].WeapIndex == WeapIndex)
				{
					if(PlayerPlane->WeapLoad[j].Status == AIM9_STAT)
						AddWeapToList(&PlayerPlane->WeapLoad[j],TRUE);
				}
			}
		}
	}

	// create AA MRM    AIM7
 	for(i=0; i<AV_MAX_WEAPONS; i++)
	{
		if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
		{
			WeapIndex = PlayerPlane->WeapLoad[i].WeapIndex;
			for(int j=0; j<AV_MAX_WEAPONS; j++)
			{
				if(PlayerPlane->WeapLoad[j].WeapIndex == WeapIndex)
				{
					if(PlayerPlane->WeapLoad[j].Status == AIM7_STAT)
						AddWeapToList(&PlayerPlane->WeapLoad[j],TRUE);
				}
			}
		}
	}

	// create AA MRM   AIM120
 	for(i=0; i<AV_MAX_WEAPONS; i++)
	{
		if(PlayerPlane->WeapLoad[i].Status != NO_WEAP_STAT)
		{
			WeapIndex = PlayerPlane->WeapLoad[i].WeapIndex;
			for(int j=0; j<AV_MAX_WEAPONS; j++)
			{
				if(PlayerPlane->WeapLoad[j].WeapIndex == WeapIndex)
				{
					if(PlayerPlane->WeapLoad[j].Status == AIM120_STAT)
						AddWeapToList(&PlayerPlane->WeapLoad[j],TRUE);
				}
			}
		}
	}

	InitAASequenceList();

	// default to first weapon
	SelectNextAAWeapon();
	SelectNextAGWeapon();

	InitBombingAttributes();
	InitBombStations();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FixupAALoadout()
{
	AvWeapEntryType *Walk,*Last,*Walk2;

	Walk = &Av.Weapons.AASeqList[0];
	Last = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

	int Index = Av.Weapons.NumAASeqWeapons;
	while(Index-- > 0)
	{
		Walk->Selected = FALSE;
		Walk++;
	}

	Walk = &Av.Weapons.AASeqList[0];
	Last = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

	while(Walk <= Last)
	{
		if(Walk->W->Count <= 0)
		{
			if(Last > Walk)
			{
				Walk2 = Walk;
				while(Walk2 < Last)
				{
					memcpy(Walk2,(Walk2+1),sizeof(AvWeapEntryType));
					Walk2++;
				}
			}
			Last--;
			Av.Weapons.NumAASeqWeapons--;
			if(Av.Weapons.NumAASeqWeapons < 0) Av.Weapons.NumAASeqWeapons = 0;
			if(Last < Walk) break;
		}
		else
		{
			if(Av.Weapons.UseAASeq)
			{
				if( (Av.Weapons.CurAASeqWeap) && (Av.Weapons.CurAASeqWeap->W == Walk->W) )
				{
					Av.Weapons.CurAASeqWeap = Walk;
					Walk->Selected = TRUE;
				}
			}
			else
				 Av.Weapons.CurAASeqWeap = NULL;

			Walk++;
		}
	}

	Walk = &Av.Weapons.AAList[0];
	Last = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

	Index = Av.Weapons.NumAAWeapons;
	while(Index-- > 0)
	{
		Walk->Selected = FALSE;
		Walk++;
	}

	Walk = &Av.Weapons.AAList[0];
	Last = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

	while(Walk <= Last)
	{
		if(Walk->W->Count <= 0)
		{
			if(Last > Walk)
			{
				Walk2 = Walk;
				while(Walk2 < Last)
				{
				  memcpy(Walk2,(Walk2+1),sizeof(AvWeapEntryType));
					Walk2++;
				}
			}
			Last--;
			Av.Weapons.NumAAWeapons--;
			if(Av.Weapons.NumAAWeapons < 0) Av.Weapons.NumAAWeapons = 0;
			if(Last < Walk) break;
		}
		else
		{
			if(!Av.Weapons.UseAASeq)
			{
					if( (Av.Weapons.CurAAWeap) && (Av.Weapons.CurAAWeap->W == Walk->W) )
					{
						Av.Weapons.CurAAWeap = Walk;
						Walk->Selected = TRUE;
					}
			}
			else
				Av.Weapons.CurAAWeap = NULL;

			Walk++;
		}
	}

	if(Av.Weapons.UseAASeq)
	{
		 if(Av.Weapons.CurAASeqWeap == NULL)
				SelectNextAAWeaponType();
		 Av.Weapons.CurAAWeap = NULL;
	}
	else
	{
		 if(Av.Weapons.CurAAWeap == NULL)
				SelectNextAAWeapon();
		 Av.Weapons.CurAASeqWeap = NULL;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FixupAGLoadout()
{
	ResetBombProgram(Av.Weapons.CurAGWeap);

	AvWeapEntryType *W = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

	int Found = FALSE;

	while(W <= LastW)
	{
		if(W->Selected)
		{
			Found = TRUE;
			if(Av.Weapons.CurAGWeap->W->Count > 0)
				Av.Weapons.CurAGWeap = W;
			break;
		}
	  W++;
	}

	if(!Found)
	{
		Av.Weapons.CurAGWeap = NULL;
		SelectNextAGWeapon();
		ResetBombProgram(Av.Weapons.CurAGWeap);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void F18LoadPlayerWeaponVars()
{
	int CF6060 = GetWeapId(CHAFF_FLARE6060_ID);
	int CF9030 = GetWeapId(CHAFF_FLARE9030_ID);
	int CF3090 = GetWeapId(CHAFF_FLARE3090_ID);

	ChaffTotal = 0;
	FlareTotal = 0;

	if(PlayerPlane->WeapLoad[CHAFF_FLARE].Count > 0)
	{
	  if(PlayerPlane->WeapLoad[CHAFF_FLARE].WeapId == CF6060)
	  {
		  ChaffTotal = 60;
	    FlareTotal = 60;
	  }
	  else if(PlayerPlane->WeapLoad[CHAFF_FLARE].WeapId == CF9030)
	  {
		 ChaffTotal  = 90;
	    FlareTotal = 30;
	  }
	  else
	  {
		  ChaffTotal = 30;
	    FlareTotal = 90;
	  }
  }

  // set weapon states
  Av.Weapons.CurrWeapMode = WEAP_OFF;
  Av.Weapons.PrevWeapMode = WEAP_OFF;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitF18LoadoutInfo()
{
  Av.Weapons.WeaponsLoaded = FALSE;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void F18LoadPlayerWeaponInfo()
{
	F18Loadout F18Load;
	int Type;
	HANDLE HFile;
	RESOURCEFILE	ResFile;

	PauseForDiskHit();


		// init fuel weight data
	PlayerPlane->AircraftDryWeight = 0;
	PlayerPlane->InternalFuel      = 0;
	PlayerPlane->CenterDropFuel    = 0;
	PlayerPlane->WingDropFuel      = 0;

	// Call this to start resource management; returns file handle & fills struct
  HFile =	LoadResourceFile (g_szResourceFile,&ResFile);

	if(HFile != INVALID_HANDLE_VALUE)
  {
		PlayerPlane->AircraftDryWeight = ResFile.ActiveAircraft[0].nWeight;
		CloseResourceFile (HFile);
	}

  if(!GetPlayerLoadout(g_szResourceFile,&F18Load))
	  return;

	Aim9X_Id  = GetWeapId(AIM9X_ID);
	Aim9L_Id  = GetWeapId(AIM9L_ID);
	Aim9M_Id  = GetWeapId(AIM9M_ID);
	Aim7F_Id  = GetWeapId(AIM7F_ID);
	Aim7M_Id  = GetWeapId(AIM7M_ID);
	Aim7MH_Id = GetWeapId(AIM7MH_ID);
	Aim120_Id = GetWeapId(AIM120_ID);

	Av.Weapons.HasNavFlir     = FALSE;
	Av.Weapons.HasTargetIR = FALSE;
	Av.Weapons.HasDataLink = FALSE;

	int AAR50ID = GetWeapId(AAR50_ID);
	int AAS3ID  = GetWeapId(AAS38A_ID);
	int AWW13ID = GetWeapId(AWW13_ID);

	PlayerPlane->InternalFuel = F18Load.nInternalFuelWeight;

	if((PlayerPlane->InternalFuel <= 0) || (g_nMissionType == SIM_QUICK))
	{
		PlayerPlane->InternalFuel = WEIGHT_MAX_INTERNALFUEL;
	}


	// load ALL weapons into the database if not already loaded
	if(!Av.Weapons.WeaponsLoaded)
	{
	  int *WeapIdPtr = &F18WeapIds[0];
	  while(*WeapIdPtr != -1)
	  {
	    int Index = RegisterWeapon(*WeapIdPtr);
		 WeapIdPtr++;
	  }
	}

	// load players particular loadout
	PlayerPlane->AI.fStoresWeight = 0;
	for (int i=0; i<MAX_F18E_STATIONS; i++)
	{
		PlayerPlane->WeapLoad[i].WeapId	= F18Load.Station[i].iWeaponId;
		if(PlayerPlane->WeapLoad[i].WeapId != -1)
		{
			PlayerPlane->WeapLoad[i].Count	   = F18Load.Station[i].iWeaponCnt;
			if(!Av.Weapons.WeaponsLoaded)
				PlayerPlane->WeapLoad[i].WeapIndex = RegisterWeapon(PlayerPlane->WeapLoad[i].WeapId);
			if(PlayerPlane->WeapLoad[i].Count > 0)
			{
				PlayerPlane->WeapLoad[i].bNumPods = PlayerPlane->WeapLoad[i].Count;
				PlayerPlane->AI.fStoresWeight += (pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeight * PlayerPlane->WeapLoad[i].Count);
				if(pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType == WEAPON_TYPE_FUEL_TANK)
				{
					PlayerPlane->AircraftDryWeight += (pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeight * PlayerPlane->WeapLoad[i].Count);
				}
			}
		}
		else
		{
			PlayerPlane->WeapLoad[i].Count = 0;
		}
	}

	// special case..need to load rocket pods again if it is a reload....scotts code loads them the first time
	if(Av.Weapons.WeaponsLoaded)
	{
			for(i=0; i<MAX_F18E_STATIONS; i++)
  		{
				if(PlayerPlane->WeapLoad[i].Count > 0)
				{
			 		if( PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU10_ID) )
				 		PlayerPlane->WeapLoad[i].Count *= 4;
			 		else if( PlayerPlane->WeapLoad[i].WeapId == GetWeapId(LAU68_ID) )
				 		PlayerPlane->WeapLoad[i].Count *= 19;
				}
			}
	}

	float LI,LO,RI,RO;

	LI = RI = LO = RO = 0;

	for(i=0; i<MAX_F18E_STATIONS; i++)
  {
   	if(PlayerPlane->WeapLoad[i].Count <= 0)
	   	 PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
	  else
	  {
			if( (PlayerPlane->WeapLoad[i].WeapId == Aim9X_Id) || (PlayerPlane->WeapLoad[i].WeapId == Aim9L_Id) || (PlayerPlane->WeapLoad[i].WeapId == Aim9M_Id) )
		     PlayerPlane->WeapLoad[i].Status = AIM9_STAT;
			else if( (PlayerPlane->WeapLoad[i].WeapId == Aim7F_Id) || (PlayerPlane->WeapLoad[i].WeapId == Aim7M_Id) || (PlayerPlane->WeapLoad[i].WeapId == Aim7MH_Id) )
		     PlayerPlane->WeapLoad[i].Status = AIM7_STAT;
			else if(PlayerPlane->WeapLoad[i].WeapId == Aim120_Id)
		     PlayerPlane->WeapLoad[i].Status = AIM120_STAT;

			if(PlayerPlane->WeapLoad[i].WeapId == AAR50ID)
		    Av.Weapons.HasNavFlir = TRUE;
			if(PlayerPlane->WeapLoad[i].WeapId == AAS3ID)
		    Av.Weapons.HasTargetIR = TRUE;
			if(PlayerPlane->WeapLoad[i].WeapId == AWW13ID)
		    Av.Weapons.HasDataLink = TRUE;

			Type = pDBWeaponList[PlayerPlane->WeapLoad[i].WeapIndex].iWeaponType;

			switch (Type)
			{
				case WEAPON_TYPE_AA_SHORT_RANGE_MISSILE:
				case WEAPON_TYPE_AA_MEDIUM_RANGE_MISSILE:
				case WEAPON_TYPE_AA_LONG_RANGE_MISSILE:
				{
					break;
				}
				case WEAPON_TYPE_DUMB_BOMB:
 				case WEAPON_TYPE_GUIDED_BOMB:
				case WEAPON_TYPE_AG_MISSILE:
				case WEAPON_TYPE_CLUSTER_BOMB:
				case WEAPON_TYPE_DATALINK:
				case WEAPON_TYPE_ANTI_SHIP_MISSILE:
				case WEAPON_TYPE_AA_ANTIRADAR_MISSILE:
				case WEAPON_TYPE_NAVAL_MINE:
				case WEAPON_TYPE_AGROCKET:
				{
		      	PlayerPlane->WeapLoad[i].Status = AIR_GROUND_STAT;
		       	break;
				}
		  	case WEAPON_TYPE_FUEL_TANK:
				{
					PlayerPlane->WeapLoad[i].Status = AIR_GROUND_STAT;

					if(PlayerPlane->WeapLoad[i].WeapId == GetWeapId(GALLONTANK330_ID))
					{
						 if(i != CENTER6_STATION)
						 {
							 PlayerPlane->WingDropFuel  += WEIGHT_330_GALLONS_FUEL;

							 if(i == LEFT4_STATION)  LI  += WEIGHT_330_GALLONS_FUEL;
							 if(i == LEFT3_STATION)  LO  += WEIGHT_330_GALLONS_FUEL;
							 if(i == RIGHT8_STATION) RI  += WEIGHT_330_GALLONS_FUEL;
							 if(i == RIGHT9_STATION) RO  += WEIGHT_330_GALLONS_FUEL;
						 }
						 else
						   PlayerPlane->CenterDropFuel += WEIGHT_330_GALLONS_FUEL;
					}

					if(PlayerPlane->WeapLoad[i].WeapId == GetWeapId(GALLONTANK480_ID))
					{
						 if(i != CENTER6_STATION)
						 {
						   PlayerPlane->WingDropFuel += WEIGHT_480_GALLONS_FUEL;

							 if(i == LEFT4_STATION)  LI  += WEIGHT_480_GALLONS_FUEL;
							 if(i == LEFT3_STATION)  LO  += WEIGHT_480_GALLONS_FUEL;
							 if(i == RIGHT8_STATION) RI  += WEIGHT_480_GALLONS_FUEL;
							 if(i == RIGHT9_STATION) RO  += WEIGHT_480_GALLONS_FUEL;
						 }
						 else
						   PlayerPlane->CenterDropFuel += WEIGHT_480_GALLONS_FUEL;
					}
			   	break;
				}

				default:
		  	{
 		    	PlayerPlane->WeapLoad[i].Status = NO_WEAP_STAT;
		    	break;
		  	}
		   }  // switch

		}	  //  if then else
	}	 // for

	// set total weights
	PlayerPlane->TotalWeight = 	PlayerPlane->AircraftDryWeight +
					 	        PlayerPlane->InternalFuel +
					 	        PlayerPlane->CenterDropFuel +
					 	        PlayerPlane->WingDropFuel;

  FillFuelTanks(LI,LO,RI,RO);

	PlayerPlane->TotalMass = PlayerPlane->TotalWeight/32.0;

	if(PlayerPlane->WeapLoad[GUNS_STATION].Count > 0)
	   PlayerPlane->WeapLoad[GUNS_STATION].Count *= 400;

	// Disable rendering; makes life easier in F18Drawer()
	PlayerPlane->WeapLoad[GUNS_STATION].Status = NO_WEAP_STAT;
	PlayerPlane->WeapLoad[CHAFF_FLARE_STATION].Status = NO_WEAP_STAT;
	PlayerPlane->WeapLoad[ALE50_STATION].Status = NO_WEAP_STAT;

	if(!Av.Weapons.HasNavFlir)
     SetBitMessage(AV_NAV_FLIR,AV_BIT_NOT_READY);
	if(!Av.Weapons.HasTargetIR)
     SetBitMessage(AV_TARGET_IR,AV_BIT_NOT_READY);

	F18LoadPlayerWeaponVars();

	UnPauseFromDiskHit();

	Av.Weapons.WeaponsLoaded = TRUE;

	InitWeaponsLoadout();

	InitWeapStores();
}

//*****************************************************************************************************************************************
// WEAP SELECT FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SelectNextAAWeaponType()
{
	AvWeapEntryType *W;
	AvWeapEntryType *LastW = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

 	if(g_dwAIMSndHandle)
 		KillSound( &g_dwAIMSndHandle );
	if(g_dwAIMLockSndHandle)
		KillSound( &g_dwAIMLockSndHandle );

	if(Av.Weapons.CurAAWeap == NULL)    // weap hasn't been selected yet, or has been turned off
	{
		W = &Av.Weapons.AAList[0];

		int Count=0;
		while(W <= LastW)
		{
			if(W->W->Count > 0)
			{
				W->Selected = TRUE;
				Av.Weapons.CurAAWeap = W;
		    Av.Weapons.CurrWeapMode = W->W->Status;
				break;
 			}
 			Count++;
			W++;
		}
 	}
	else                                     // get next available weap on the list
	{
		W  = Av.Weapons.CurAAWeap;
		AvWeapEntryType *Temp = W;
    int Count = 0;
		while(Count <= AV_MAX_WEAPONS)
		{
			if(W->W->Count > 0)
			{
				if(W->W->WeapIndex != Temp->W->WeapIndex)
				{
					Temp->Selected = FALSE;
					W->Selected    = TRUE;
					Av.Weapons.CurAAWeap = W;
				  Av.Weapons.CurrWeapMode = W->W->Status;
					break;
				}
			}
			Count++;
			W++;
			if(W > LastW) W = &Av.Weapons.AAList[0];
		}
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectNextAAWeapon()
{
 	if(g_dwAIMSndHandle)
 		KillSound( &g_dwAIMSndHandle );
	if(g_dwAIMLockSndHandle)
		KillSound( &g_dwAIMLockSndHandle );

	if(!Av.Weapons.UseAASeq)
		 SelectNextAAWeaponType();
	else
	{
		AvWeapEntryType *W;
    AvWeapEntryType *FirstW = &Av.Weapons.AASeqList[0];
	  AvWeapEntryType *LastW  = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

		W = FirstW;
		while(W <= LastW)
		{
		 W->Selected = FALSE;
		 W++;
		}

		Av.Weapons.UseAASeq = FALSE;
		if(Av.Weapons.CurAASeqWeap == NULL)
		{
			SelectNextAAWeaponType();
 		}
		else
		{
			FirstW = &Av.Weapons.AAList[0];
	    LastW  = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

			W = FirstW;
			while(W <= LastW)
			{
				if( (W->W->WeapIndex == Av.Weapons.CurAASeqWeap->W->WeapIndex) && (W->W->Count > 0) )
				{
					Av.Weapons.CurAAWeap = W;
					W->Selected = TRUE;
				  Av.Weapons.CurrWeapMode = W->W->Status;
					break;
				}
				W++;
			}
			SelectNextAAWeaponType();
			Av.Weapons.CurAASeqWeap = NULL;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectMediumOrShortRangeAAWeapon(int Type)
{
  AvWeapEntryType *W,*Walk,*Walk2;
  AvWeapEntryType *FirstW;
	AvWeapEntryType *LastW;

  FirstW = &Av.Weapons.AASeqList[0];
  LastW  = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

	W = FirstW;
	while(W <= LastW)
	{
		if( (W->W->Status == Type) && (W->W->Count > 0) )
		{
			Walk2 = &Av.Weapons.AAList[0];
			while(Walk2 <= &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1])
			{
				Walk2->Selected = FALSE;
				Walk2++;
			}

	   	  Walk = FirstW;
			while(Walk <= LastW)
			{
				Walk->Selected = FALSE;
				Walk++;
			}
 		  Av.Weapons.CurAASeqWeap = W;
		  W->Selected = TRUE;
		  Av.Weapons.UseAASeq = TRUE;
		  Av.Weapons.CurAAWeap = W;
		 Av.Weapons.CurrWeapMode = Type;
		  break;
 		}
		W++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectNextAGWeapon()
{
	AvWeapEntryType *W;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

	if(Av.Weapons.Bomb.Drop.Activate)   return; // can't select AG weap when the bombing sequence is active
	if(Av.Weapons.Bomb.Rocket.Activate) return; // can't select AG weap when the rocket sequence is active

	if(Av.Weapons.CurAGWeap == NULL)    // weap hasn't been selected yet, or has been turned off
	{
		W = &Av.Weapons.AGList[0];

		int Count=0;
		while(W <= LastW)
		{
			if(W->W->Count > 0)
			{
				W->Selected = TRUE;
				Av.Weapons.CurAGWeap = W;
				break;
 			}
 			Count++;
			W++;
		}
 	}
	else                                     // get next available weap on the list
	{
		W  = Av.Weapons.CurAGWeap;
		AvWeapEntryType *Temp = W;
		AvWeapEntryType *Walk = FirstW;

		int Count = 0;
	  while(Count <= AV_MAX_WEAPONS)
		{
			if(W->W->Count > 0)
			{
				if(W->W->WeapIndex != Temp->W->WeapIndex)
				{
					while(Walk <= LastW)       // un-select all weaps
					{
						Walk->Selected = FALSE;
						Walk++;
					}
					W->Selected    = TRUE;
					Av.Weapons.CurAGWeap = W;
					break;
				}
			}
			Count++;
			W++;
			if(W > LastW) W = &Av.Weapons.AGList[0];
		}
	}

}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ManualSelectAGWeapon(int Station)
{
	AvWeapEntryType *Walk,*CurW;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	int Type = -1;

	Walk = FirstW;
	while(Walk <= LastW)      // find the station and get its type
	{
		if( (Walk->Station == Station) && (Walk->W->Count > 0) )
		{
			Type = Walk->W->WeapIndex;
			CurW = Walk;
		}
		Walk++;
	}

	if(Type == -1)
	{
		return; // not valid
	}
	else                // if types are the same, then just select the new box and return
	{
		if(Av.Weapons.CurAGWeap == NULL)
		{
          CurW->Selected = TRUE;
          Av.Weapons.CurAGWeap = CurW;
		  return;
		}
		else if(Type == Av.Weapons.CurAGWeap->W->WeapIndex)
		{
			CurW->Selected = TRUE;
			return;
		}
	}

	Walk = FirstW;          // types are different, so clear the select for all weaps and set the new weapon
	while(Walk <= LastW)
	{
		Walk->Selected = FALSE;
		Walk++;
	}

	CurW->Selected = TRUE;
	Av.Weapons.CurAGWeap = CurW;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ManualUnSelectAGWeapon(int Station)
{
	AvWeapEntryType *Walk;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	int Found = FALSE;

	Walk = FirstW;
	while(Walk <= LastW)      // find the station and get its type
	{
		if( (Walk->Station == Station) && (Walk->W->Count > 0) )
		{
			Walk->Selected = FALSE;
			break;
		}
		Walk++;
	}

	Walk = FirstW;
	while(Walk <= LastW)      // find the station and get its type
	{
		if(Walk->Selected)
		{
      Found = TRUE;
		  Av.Weapons.CurAGWeap = Walk;
 			break;
		}
		Walk++;
	}

	if(!Found)
	{
		Av.Weapons.CurAGWeap = NULL;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectUnSelectAGWeap(int Station)
{
 	AvWeapEntryType *Walk;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	int Type = -1;

	if(Av.Weapons.Bomb.Drop.Activate)   return; // can't select AG weap when the bombing sequence is active
	if(Av.Weapons.Bomb.Rocket.Activate) return; // can't select AG weap when the rocket sequence is active

	Walk = FirstW;
	while(Walk <= LastW)      // find the station and get its type
	{
		if( (Walk->Station == Station) && (Walk->W->Count > 0) )
		{
			if(Walk->Selected == FALSE)
			 ManualSelectAGWeapon(Station);
			else
			 ManualUnSelectAGWeapon(Station);
			 return;
		}
		Walk++;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int AGStationReject()
{
	int WeapId = GetSelectedAGWeaponId();

	if(WeapId == -1) return(FALSE);

	AvWeapEntryType *Walk,*Found,*W;
	AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

	Found = NULL;
	Walk = FirstW;
	while(Walk <= LastW)
	{
		if(Walk->Selected)
		{
			W = Walk;
			int Count = 0;
	    while(Count <= AV_MAX_WEAPONS)
	    {
			  if(W != Walk)
			  {
				  if( (W->W->WeapId == WeapId) && (W->W->Count > 0) )
				  {
					 Found = W;
					 break;
				  }
				}
				Count++;
		    W++;
	      if(W > LastW) W = FirstW;
			}
		}
		if(Found)
		{
			Walk->Selected  = FALSE;
			Found->Selected = TRUE;
			Av.Weapons.CurAGWeap = Found;
			break;
		}
	  Walk++;
	}
	return( (Found != NULL) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectStationsForCount(int WeapId,int Count)
{
 	AvWeapEntryType *Walk;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

	if(Av.Weapons.Bomb.Drop.Activate)   return; // can't select AG weap when the bombing sequence is active
	if(Av.Weapons.Bomb.Rocket.Activate) return; // can't select AG weap when the rocket sequence is active

	int Bombs = Count;
	Walk = FirstW;
	while(Walk <= LastW)
	{
		if(Walk->W->WeapId == WeapId)
		{
			if(Walk->W->Count > 0)
			{
			  if(Bombs > 0)
				{
					Walk->Selected = TRUE;
				}
				else
				{
					Walk->Selected = FALSE;
				}

			  Bombs -= Walk->W->Count;
			}
		}
		Walk++;
	}

	Av.Weapons.CurAGWeap = NULL;

	Walk = FirstW;
	while(Walk <= LastW)
	{
		if(Walk->W->WeapId == WeapId)
		{
			if(Walk->Selected)
			{
				Av.Weapons.CurAGWeap = Walk;
				break;
			}
		}
		Walk++;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetAGWeapon(int WeapId,int NumStations)
{
 	AvWeapEntryType *Walk;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

	if(Av.Weapons.Bomb.Drop.Activate)   return; // can't select AG weap when the bombing sequence is active
	if(Av.Weapons.Bomb.Rocket.Activate) return; // can't select AG weap when the rocket sequence is active

	// unselect all ag weaps
	Walk = FirstW;
	while(Walk <= LastW)
	{
		Walk->Selected = FALSE;
		Walk++;
	}

	Av.Weapons.CurAGWeap = NULL;

	Walk = FirstW;
	while(Walk <= LastW)      // find the station and get its type
	{
		if( (Walk->W->WeapId == WeapId) && (Walk->W->Count > 0) )
		{
			Av.Weapons.CurAGWeap = Walk;
			Walk->Selected = TRUE;

 	    AvWeapEntryType *StationWalk = Walk+1;

			int Count = NumStations-1;
			while( (StationWalk <= LastW) && (StationWalk->W->WeapId == WeapId) && (Count > 0) )
			{
				if(StationWalk->W->Count > 0)
				{
			    StationWalk->Selected = TRUE;
				}
			  Count--;
			  StationWalk++;
			}

			break;
		}
		Walk++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectNextWeapon()
{
 	if(g_dwAIMSndHandle)
 		KillSound( &g_dwAIMSndHandle );
	if(g_dwAIMLockSndHandle)
		KillSound( &g_dwAIMLockSndHandle );

	if(UFC.MasterMode == AA_MODE)
	{
		SelectNextAAWeapon();
	}
	else if(UFC.MasterMode == AG_MODE)
	{
		SelectNextAGWeapon();
		ResetBombProgram(Av.Weapons.CurAGWeap);
	}
}

//*****************************************************************************************************************************************
// MISC WEAP FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int GetWeapId(int Type)
{
	if(Type >= MAX_F18_WEAP_IDS) return -1;

	return( F18WeapIds[Type] );
}

int GetWeapIndexFromId (int nId)
{
	for (int n = 0; n < MAX_F18_WEAP_IDS; n++)
	{
		if (F18WeapIds[n] == nId)
			return (n);
	}

	return (-1);
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int HasAAWeapSelected()
{
	if(Av.Weapons.UseAASeq)
	{
	  AvWeapEntryType *W;
    AvWeapEntryType *FirstW = &Av.Weapons.AASeqList[0];
	  AvWeapEntryType *LastW  = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

		W = FirstW;
		while(W <= LastW)
		{
			if(W->Selected == TRUE)
				return(TRUE);
			W++;
		}
	}
	else
	{
	  AvWeapEntryType *W;
    AvWeapEntryType *FirstW = &Av.Weapons.AAList[0];
	  AvWeapEntryType *LastW  = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

		W = FirstW;
		while(W <= LastW)
		{
			if(W->Selected == TRUE)
				return(TRUE);
			W++;
		}
	}

	return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int HasAGWeapSelected()
{
    AvWeapEntryType *W;
    AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	  AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

		W = FirstW;
		while(W <= LastW)
		{
			if(W->Selected == TRUE)
				return(TRUE);
			W++;
		}

		return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetSelectedAAWeaponId()
{
	if(Av.Weapons.UseAASeq)
	{
		if(Av.Weapons.CurAASeqWeap)
		{
			if(Av.Weapons.CurAASeqWeap->Selected)
				return(Av.Weapons.CurAASeqWeap->W->WeapId);
		}
	}
	else
	{
	  if(Av.Weapons.CurAAWeap)
		{
			if(Av.Weapons.CurAAWeap->Selected)
				return(Av.Weapons.CurAAWeap->W->WeapId);
		}
	}

	return(-1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetSelectedAGWeaponId()
{
 if(Av.Weapons.CurAGWeap)
 {
	 if(Av.Weapons.CurAGWeap->Selected)
		return(Av.Weapons.CurAGWeap->W->WeapId);
 }

	return(-1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsWeaponOnBoard(int Id)
{
	int DatabaseId = GetWeapId(Id);

	for (int i=0; i<MAX_F18E_STATIONS; i++)
	{
		if(PlayerPlane->WeapLoad[i].WeapId == DatabaseId)
		{
			 if(PlayerPlane->WeapLoad[i].Count > 0)
				return(TRUE);
		}
	}

	return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetNumStationsUnselected(int WeapId)
{
    AvWeapEntryType *W;
    AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	  AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
		int Count = 0;

		W = FirstW;
		while(W <= LastW)
		{
			if(W->W->WeapId == WeapId)
			{
				if( (W->W->Count > 0) && (!W->Selected) )
					Count++;
			}
			W++;
		}
		return(Count);
}

//*****************************************************************************************************************************************
// BOMB RELEASE  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int F18CbuDetonateAlt[5] = {300,600,1000,1500,2000};

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int NumBombStationsSelected()
{
	AvWeapEntryType *Weap = &Av.Weapons.AGList[0];
  int Count = 0;

  for(int i=0; i < Av.Weapons.NumAGWeapons; i++)
  {
	if(Weap[i].Selected == TRUE)
      Count++;
  }
  return(Count);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetNextActiveBombStation(int FromIndex)
{
	AvWeapEntryType *Weap = &Av.Weapons.AGList[0];

	int Index = FromIndex;
  int Count = 0;

  if(FromIndex >= Av.Weapons.NumAGWeapons)
	  FromIndex = 0;

  while( (Weap[Index].Selected != TRUE) && (Count <= 20) )
  {
	 Index++;
	 Count++;
	 if(Index >= Av.Weapons.NumAGWeapons)
	   Index = 0;
  }

  if(Count == 20)
	 Index = -1;

  return(Index);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetNumRocketSalvo()
{
 	AvWeapEntryType *Walk;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	int returnval = 0;

	Walk = FirstW;
	while(Walk <= LastW)
	{
		if(Walk->Selected)
		{
			if(Walk->W->Count > 0)
			{
				returnval += Walk->W->Count;
			}
		}
		Walk++;
	}
	return(returnval);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoRocketReleaseSim(int FirstTime)
{
	// this function called at 50hz. from weapons.cpp
 	static int DeltaTicks = 1;
	static int Counter = 0;
//	int rocketactive;
	int weapid = -1;

	WeaponReleaseSimType *Rocket = &Av.Weapons.Bomb.Rocket;
	AvWeapEntryType *Weap = &Av.Weapons.AGList[0];

	if(Weap[0].Selected)
	{
		Rocket->StartIndex = 0;
	}
	else
	{
		Rocket->StartIndex = GetNextActiveBombStation(1);
		if((Rocket->StartIndex <= 0) && (Rocket->TotalReleaseNumber > 1))
		{
			Rocket->TotalReleaseNumber = 1;
		}
	}

  DeltaTicks = 13;  // 4 times a sec at 50hz

	if (Rocket->TotalReleaseNumber > 0)
	{
		if( (++Counter >= DeltaTicks) || (FirstTime) )
		{
			for (int i=0;i<Rocket->OneTimeDropNum;i++)
			{
  			 Rocket->TotalReleaseNumber--;
				 if( (Rocket->StartIndex != -1) && (Rocket->TotalReleaseNumber >= 0.0) )
				 {
					 if(Weap[Rocket->StartIndex].Selected)
					 {
						 weapid = Weap[Rocket->StartIndex].W->WeapId;
	   				if(Weap[Rocket->StartIndex].W->Count > 0)
						{
						  PlayFFEffect(FF_MISSILE_FIRING);
						  InstantiateAGMissile(PlayerPlane,Weap[Rocket->StartIndex].Station, NULL, PlayerPlane->AGDesignate);

						  if(cPlayerLimitWeapons)
							  Weap[Rocket->StartIndex].W->Count--;

						  if(Weap[Rocket->StartIndex].W->Count <= 0)
						  {
								PlaneHasAGWeapons(PlayerPlane);
							  Weap[Rocket->StartIndex].W->Status = NO_WEAP_STAT;
							  Weap[Rocket->StartIndex].Selected = FALSE;
						  }
						}
  				 }
				 }

				 Rocket->StartIndex = GetNextActiveBombStation(Rocket->StartIndex+1);

				 if(Rocket->StartIndex <= 0)
					 break;
			}

			if(Rocket->TotalReleaseNumber <= 0)
			{
				 AvWeapEntryType *W = &Av.Weapons.AGList[0];
				 AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

				 Rocket->Activate = 0;

				 int Found = FALSE;    // if no stations are selected, auto pick the next one

				 /*
				 while(W <= LastW)
				 {
					if(W->Selected)
					{
						Found = TRUE;
			      if(Av.Weapons.CurAGWeap->W->Count > 0)
				      Av.Weapons.CurAGWeap = W;
						break;
					}
					W++;
				 }
					* */

				 if(!Found)
				 {
						ResetBombProgram(Av.Weapons.CurAGWeap);

						AvWeapEntryType *W = &Av.Weapons.AGList[0];
						AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

						while(W <= LastW)
						{
							if(W->Selected)
							{
								Found = TRUE;
			          if(Av.Weapons.CurAGWeap->W->Count > 0)
				          Av.Weapons.CurAGWeap = W;
								break;
							}
	  					W++;
						}
				 }

				 if(!Found)
				 {
					 Av.Weapons.CurAGWeap = NULL;
					 SelectNextAGWeapon();
 		       ResetBombProgram(Av.Weapons.CurAGWeap);
				 }
			}
			Counter = 0;
		}
	}
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DoBombReleaseSim(int CalcInterval)
{
	// this function called at 50hz. from weapons.cpp
 	static int DeltaTicks = 1;
	static int Counter = 0;
	int bombactive;
	int weapid = -1;
	int norelease = 0;

	WeaponReleaseSimType *Bomb = &Av.Weapons.Bomb.Drop;
	AvWeapEntryType *Weap = &Av.Weapons.AGList[0];

	if (CalcInterval)
		DeltaTicks = (int)((Bomb->Interval/PlayerPlane->IfHorzVelocity)/0.020f);

	if (Bomb->TotalReleaseNumber > 0)
	{
		if( (++Counter >= DeltaTicks) || (CalcInterval) )
		{
			for (int i=0;i<Bomb->OneTimeDropNum;i++)
			{
  			 Bomb->TotalReleaseNumber--;
				 if( (Bomb->StartIndex != -1) && (Bomb->TotalReleaseNumber >= 0.0) )
				 {
					 if(Weap[Bomb->StartIndex].Selected)
					 {
						 weapid = Weap[Bomb->StartIndex].W->WeapId;
	   				if(Weap[Bomb->StartIndex].W->Count > 0)
						{
						  if(pDBWeaponList[Weap[Bomb->StartIndex].W->WeapIndex].iWeaponType == 5)  //  Is Air to ground missile
						  {
							  PlayFFEffect(FF_MISSILE_FIRING);
							  InstantiateAGMissile(PlayerPlane,Weap[Bomb->StartIndex].Station, NULL, PlayerPlane->AGDesignate);
						  }
						  else
						  {
							  norelease = 1;
							  if((fabs(RadToDeg(PlayerPlane->Alpha)) < 25.0f) || (PlayerPlane->IndicatedAirSpeed > 100))
							  {
								  norelease = 0;
								  PlayFFEffect(FF_MISSILE_FIRING);
								  InstantiateBomb(PlayerPlane,Weap[Bomb->StartIndex].Station,Av.Weapons.Bomb.CbuDetonateAltIndex*FTTOWU);
							  }
						  }

						  PlayerPlane->AI.iAIFlags2 |= AI_RIPPLE_BOMB;

						  if(!norelease)
						  {
							  if(cPlayerLimitWeapons)
								  Weap[Bomb->StartIndex].W->Count--;

							  if(Weap[Bomb->StartIndex].W->Count <= 0)
							  {
									PlaneHasAGWeapons(PlayerPlane);
								  Weap[Bomb->StartIndex].W->Status = NO_WEAP_STAT;
								  Weap[Bomb->StartIndex].Selected = FALSE;
							  }
						  }
						  norelease = 0;
						}
  				 }
				 }

				 if(((Bomb->OneTimeDropNum - 1) <= i) && (Bomb->OneTimeDropNum > 1) && (Bomb->TotalReleaseNumber))
				 {
					 if((Av.Weapons.CurAGWeap) && (Av.Weapons.CurAGWeap->W))
					 {
						bombactive = Av.Weapons.Bomb.Drop.Activate;
						Av.Weapons.Bomb.Drop.Activate = 0;
					     SelectStationsForCount(Av.Weapons.CurAGWeap->W->WeapId,Bomb->OneTimeDropNum);
						Av.Weapons.Bomb.Drop.Activate = bombactive;
					 }
				 }

				 Bomb->StartIndex = GetNextActiveBombStation(Bomb->StartIndex+1);

				 if((Bomb->StartIndex <= 0) && (weapid != -1) && (Bomb->TotalReleaseNumber))
				 {
					bombactive = Av.Weapons.Bomb.Drop.Activate;
					Av.Weapons.Bomb.Drop.Activate = 0;
					SelectStationsForCount(weapid,Bomb->OneTimeDropNum);
					Av.Weapons.Bomb.Drop.Activate = bombactive;
					Bomb->StartIndex = GetNextActiveBombStation(0);
				 }
			}

			if(Bomb->TotalReleaseNumber <= 0)
			{
				 AvWeapEntryType *W = &Av.Weapons.AGList[0];
				 AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

			   PlayerPlane->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;
				 Bomb->Activate = 0;

				 int Found = FALSE;    // if no stations are selected, auto pick the next one

				 /*
				 while(W <= LastW)
				 {
					if(W->Selected)
					{
						Found = TRUE;
			      if(Av.Weapons.CurAGWeap->W->Count <= 0)
				      Av.Weapons.CurAGWeap = W;
						break;
					}
					W++;
				 }
					* */

				 if(!Found)
				 {
						ResetBombProgram(Av.Weapons.CurAGWeap);

						AvWeapEntryType *W = &Av.Weapons.AGList[0];
						AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

						while(W <= LastW)
						{
							if(W->Selected)
							{
								Found = TRUE;
			          if(Av.Weapons.CurAGWeap->W->Count > 0)
				          Av.Weapons.CurAGWeap = W;
								break;
							}
	  					W++;
						}
				 }

				 if(!Found)
				 {
					 Av.Weapons.CurAGWeap = NULL;
					 SelectNextAGWeapon();
 		       ResetBombProgram(Av.Weapons.CurAGWeap);
				 }
			}
			Counter = 0;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ReleaseSingleBomb()
{
	AvWeapEntryType *Weap = &Av.Weapons.AGList[0];
	WeaponReleaseSimType *Bomb = &Av.Weapons.Bomb.Drop;
	int norelease = 0;

	int BombWeapId = -1;

	for(int i=0; i<Av.Weapons.NumAGWeapons; i++)
	{
		if(Weap[i].Selected)
		{
			if(Weap[i].W->Count > 0)
			{
				if( (pDBWeaponList[Weap[i].W->WeapIndex].iWeaponType == WEAPON_TYPE_AGROCKET) || (pDBWeaponList[Weap[i].W->WeapIndex].iWeaponType == WEAPON_TYPE_AG_MISSILE) )
				{
					PlayFFEffect(FF_MISSILE_FIRING);
					InstantiateAGMissile(PlayerPlane,Weap[i].Station, NULL, PlayerPlane->AGDesignate);
				}
				else
				{
					norelease = 1;
					if((fabs(RadToDeg(PlayerPlane->Alpha)) < 25.0f) || (PlayerPlane->IndicatedAirSpeed > 100))
					{
						norelease = 0;
						PlayFFEffect(FF_MISSILE_FIRING);
						InstantiateBomb(PlayerPlane,Weap[i].Station,Av.Weapons.Bomb.CbuDetonateAltIndex*FTTOWU);
					}
				}

				if(!norelease)
				{
					if(cPlayerLimitWeapons)
						Weap[i].W->Count--;

					PlayerPlane->AI.iAIFlags2 |= AI_RIPPLE_BOMB;

					if(Weap[i].W->Count <= 0)
					{
						PlaneHasAGWeapons(PlayerPlane);
						Weap[i].W->Status = NO_WEAP_STAT;
						Weap[i].Selected = FALSE;
						BombWeapId = Weap[i].W->WeapId;
					}
				}
				norelease = 0;
			}
		}
	} // for

	PlayerPlane->AI.iAIFlags2 &= ~AI_RIPPLE_BOMB;

	AvWeapEntryType *W = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

	int Found = FALSE;    // if no stations are selected, auto pick the next one
	while(W <= LastW)
	{
		if(W->Selected)
		{
			Found = TRUE;
			if(Av.Weapons.CurAGWeap->W->Count > 0)
				Av.Weapons.CurAGWeap = W;
			break;
		}
	  W++;
	}

	if(!Found)
	{
		ResetBombProgram(Av.Weapons.CurAGWeap);

		AvWeapEntryType *W = &Av.Weapons.AGList[0];
		AvWeapEntryType *LastW = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];

		while(W <= LastW)
		{
			if(W->Selected)
			{
				Found = TRUE;
				break;
			}
		W++;
		}
	}

	if(!Found)
	{
		Av.Weapons.CurAGWeap = NULL;
		SelectNextAGWeapon();
		ResetBombProgram(Av.Weapons.CurAGWeap);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FireAGWeap()
{
	int TargetType = -1;
	FPointDouble TargetLoc;
	int GroundTargetType;
	int InKeyhole;
	int MavFired = FALSE;
	void *Trgt;
	int Fired = FALSE;
	int norelease = 0;

	if(Av.Weapons.CurAGWeap == NULL) return;

	if(Av.Weapons.CurAGWeap->WeapPage == AGM_65_WEAP_PAGE || Av.Weapons.CurAGWeap->WeapPage == AGM_65F_WEAP_PAGE )
	{
		FireMaveric(&TargetType,&TargetLoc,&Trgt,&InKeyhole);
		MavFired = TRUE;
	}
	else if(Av.Weapons.CurAGWeap->WeapPage == WALLEYE_WEAP_PAGE)
	{
		FireWalleye(&TargetType,&TargetLoc,&Trgt);
	}

	// if maverick and outside keyhole, roll the dice. If true, missle wanders
	if(MavFired && !InKeyhole)
	{
		float PercentChance = rand() % 100;

		if(PercentChance > 25.0)
		{
			if(TargetType != FLIR_CAM_NO_TARGET)
			{
				FPointDouble Target;
				float NewX,NewZ;
				float RandDistPercent = (float)(rand() % 100)/100.0;
				float RandDegree;

				if(TargetType == FLIR_CAM_GROUND_TARGET)
					Target = ((BasicInstance *)Trgt)->Position;
				else if(TargetType == FLIR_CAM_VEH_TARGET)
					Target = ((MovingVehicleParams *)Trgt)->WorldPosition;
				else if(TargetType == FLIR_CAM_PLANE_TARGET)
					Target = ((PlaneParams *)Trgt)->WorldPosition;
				else
					Target = TargetLoc;

				RandDegree = rand() % 360;

				ProjectPoint(Target.X,Target.Z,RandDegree,1.0*NMTOWU*RandDistPercent, &NewX, &NewZ);
				Target.Y = LandHeight(Target.X,Target.Z);
				TargetType = FLIR_CAM_LOC_TARGET;
				TargetLoc = Target;
			}
		}
	}

	switch(TargetType)
	{
		case FLIR_CAM_GROUND_TARGET:
			GroundTargetType = GROUNDOBJECT;
		break;
		case FLIR_CAM_PLANE_TARGET:
			GroundTargetType = AIRCRAFT;
		break;
		case FLIR_CAM_VEH_TARGET:
			GroundTargetType = MOVINGVEHICLE;
		break;
		case FLIR_CAM_LOC_TARGET:
			Trgt = NULL;
			GroundTargetType = GROUNDOBJECT;
		break;
		case FLIR_CAM_NO_TARGET:
			Trgt = NULL;
			GroundTargetType = GROUNDOBJECT;
			TargetLoc.X = -1; TargetLoc.Y = -1; TargetLoc.Z = -1;
		break;
	}

	if(Av.Weapons.CurAGWeap->WeapPage == AGM_65_WEAP_PAGE ||Av.Weapons.CurAGWeap->WeapPage == AGM_65F_WEAP_PAGE)
	{
		Fired = TRUE;

		if(GroundTargetType == GROUNDOBJECT)
		{
			InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(BasicInstance *)Trgt,TargetLoc,GroundTargetType);

//			if(Trgt)
//				sprintf(TmpStr,"Firing MAV at Ground Object");
//			else
//				sprintf(TmpStr,"Firing MAV at location  %d   %d    %d",(int)TargetLoc.X, (int)TargetLoc.Y, (int)TargetLoc.Z);

		}
		else if(GroundTargetType == AIRCRAFT)
		{
			InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(PlaneParams *)Trgt,TargetLoc,GroundTargetType);

//			sprintf(TmpStr,"Firing MAV at Aircraft %d",(PlaneParams *)Trgt - &Planes[0]);
		}
		else if(GroundTargetType == MOVINGVEHICLE)
		{
			InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(MovingVehicleParams *)Trgt,TargetLoc,GroundTargetType);

//			sprintf(TmpStr,"Firing MAV at Vehicle %d",(MovingVehicleParams *)Trgt - &MovingVehicles[0]);
		}

		if(cPlayerLimitWeapons)
			Av.Weapons.CurAGWeap->W->Count--;

		ResetMaveric();
		Av.Mav.AGDesignate.X = -1.0f;
		Av.Mav.AGDesignate.Y = -1.0f;
		Av.Mav.AGDesignate.Z = -1.0f;

		if(Av.Weapons.CurAGWeap->W->Count <= 0)
		{
			PlaneHasAGWeapons(PlayerPlane);
			int OnBoard = AGStationReject();
			if(!OnBoard)
			{
				Av.Weapons.CurAGWeap->Selected = FALSE;
				Av.Weapons.CurAGWeap = NULL;
				SelectNextAGWeapon();
			}
		}
	}
	else if(Av.Weapons.CurAGWeap->WeapPage == WALLEYE_WEAP_PAGE)
	{
		Fired = TRUE;
		// jlm, need to get height to explode data from Gbu

		if(GroundTargetType == GROUNDOBJECT)
		{
			norelease = 1;
			if((fabs(RadToDeg(PlayerPlane->Alpha)) < 25.0f) || (PlayerPlane->IndicatedAirSpeed > 100))
			{
				norelease = 0;
				InstantiateBomb(PlayerPlane,Av.Weapons.CurAGWeap->Station,100*FTTOWU,(BasicInstance *)Trgt,GroundTargetType,TargetLoc.X,TargetLoc.Y,TargetLoc.Z);
			}

//			if(TargetLoc.X == -1)
//				sprintf(TmpStr,"Firing walleye at Ground Object");
//			else
//				sprintf(TmpStr,"Firing walleye at location  %d   %d    %d",(int)TargetLoc.X, (int)TargetLoc.Y, (int)TargetLoc.Z);
		}
		else if(GroundTargetType == MOVINGVEHICLE)
		{
			norelease = 1;
			if((fabs(RadToDeg(PlayerPlane->Alpha)) < 25.0f) || (PlayerPlane->IndicatedAirSpeed > 100))
			{
				norelease = 0;
				InstantiateBomb(PlayerPlane,Av.Weapons.CurAGWeap->Station,100*FTTOWU,(MovingVehicleParams *)Trgt,GroundTargetType,TargetLoc.X,TargetLoc.Y,TargetLoc.Z);
			}

//			sprintf(TmpStr,"Firing walleye at Vehicle %d",(MovingVehicleParams *)Trgt - &MovingVehicles[0]);
		}

		if(!norelease)
		{
			if(cPlayerLimitWeapons)
				Av.Weapons.CurAGWeap->W->Count--;

			if(Av.Weapons.CurAGWeap->W->Count <= 0)
			{
				PlaneHasAGWeapons(PlayerPlane);
				int OnBoard = AGStationReject();
				if(!OnBoard)
				{
					Av.Weapons.CurAGWeap->Selected = FALSE;
					Av.Weapons.CurAGWeap = NULL;
					SelectNextAGWeapon();
				}
			}
		}
		norelease = 0;
	}

	if(Fired) return;

	int FlightProfile;
	int FireWeap;

	if( (Av.Weapons.CurAGWeap->WeapPage == HARPOON_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == SLAMER_WEAP_PAGE) )
	{
		Fired = TRUE;

		FireWeap = GetWeaponData(Av.Weapons.CurAGWeap->WeapPage,&FlightProfile,&Trgt,&GroundTargetType,&TargetLoc);

		if(FireWeap)
		{
			if(GroundTargetType == GROUNDOBJECT)
			{
				InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(BasicInstance *)Trgt,TargetLoc,GroundTargetType,-1,FlightProfile);

//				if(TargetLoc.X == -1)
//					sprintf(TmpStr,"Firing Harpoon at Ground Object");
//				else
//					sprintf(TmpStr,"Firing Harpoon at location  %d   %d    %d",(int)TargetLoc.X, (int)TargetLoc.Y, (int)TargetLoc.Z);
			}
			else if(GroundTargetType == AIRCRAFT)
			{
				InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(PlaneParams *)Trgt,TargetLoc,GroundTargetType,-1,FlightProfile);

//				sprintf(TmpStr,"Firing Harpoon at Aircraft %d",(PlaneParams *)Trgt - &Planes[0]);
			} else if(GroundTargetType == MOVINGVEHICLE)
			{
				InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(MovingVehicleParams *)Trgt,TargetLoc,GroundTargetType,-1,FlightProfile);
//				sprintf(TmpStr,"Firing Harpoon at Vehicle %d",(MovingVehicleParams *)Trgt - &MovingVehicles[0]);
			}

			if(cPlayerLimitWeapons)
				Av.Weapons.CurAGWeap->W->Count--;

				if(Av.Weapons.CurAGWeap->W->Count <= 0)
				{
					PlaneHasAGWeapons(PlayerPlane);
					int OnBoard = AGStationReject();
					if(!OnBoard)
					{
						Av.Weapons.CurAGWeap->Selected = FALSE;
						Av.Weapons.CurAGWeap = NULL;
						SelectNextAGWeapon();
					}
				}
		}
	}
	else if( (Av.Weapons.CurAGWeap->WeapPage == JSOW_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == JDAM_WEAP_PAGE) || (Av.Weapons.CurAGWeap->WeapPage == HARM_WEAP_PAGE) )
	{
		Fired = TRUE;

		FireWeap = GetWeaponData(Av.Weapons.CurAGWeap->WeapPage,&FlightProfile,&Trgt,&GroundTargetType,&TargetLoc);

		if(FireWeap)
		{
				if(GroundTargetType == GROUNDOBJECT)
				{
					InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(BasicInstance *)Trgt,TargetLoc,GroundTargetType);

//					if(TargetLoc.X == -1)
//						sprintf(TmpStr,"Firing jdam/jsow/slamer at Ground Object");
//					else
//						sprintf(TmpStr,"Firing jdam/jsow/slamer at location  %d   %d    %d",(int)TargetLoc.X, (int)TargetLoc.Y, (int)TargetLoc.Z);
				}
				else if(GroundTargetType == AIRCRAFT)
				{
					InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(PlaneParams *)Trgt,TargetLoc,GroundTargetType);

//					sprintf(TmpStr,"Firing jdam/jsow/slamer at Aircraft %d",(PlaneParams *)Trgt - &Planes[0]);
				}
				else if(GroundTargetType == MOVINGVEHICLE)
				{
					InstantiateAGMissile(PlayerPlane,Av.Weapons.CurAGWeap->Station,(MovingVehicleParams *)Trgt,TargetLoc,GroundTargetType);

//					sprintf(TmpStr,"Firing jdam/jsow/slamer at Vehicle %d",(MovingVehicleParams *)Trgt - &MovingVehicles[0]);
				}

				if(Av.Weapons.CurAGWeap->WeapPage == HARM_WEAP_PAGE)
					 InitHarm();

				if(cPlayerLimitWeapons)
					Av.Weapons.CurAGWeap->W->Count--;

				if(Av.Weapons.CurAGWeap->W->Count <= 0)
				{
					PlaneHasAGWeapons(PlayerPlane);
		  			int OnBoard = AGStationReject();
					if(!OnBoard)
					{
						Av.Weapons.CurAGWeap->Selected = FALSE;
						Av.Weapons.CurAGWeap = NULL;
						SelectNextAGWeapon();
					}
				}
		}
	}
	else if(Av.Weapons.CurAGWeap->WeapPage == ROCKET_POD_WEAP_PAGE)
	{
		 Fired = TRUE;

		 WeaponReleaseSimType *Rocket = &Av.Weapons.Bomb.Rocket;

		 if(WeapStores.RocketPod.FireMethod == ROCKET_POD_SINGLE)
			  ReleaseSingleBomb();

		 else if(WeapStores.RocketPod.FireMethod == ROCKET_POD_SALVO)
		 {
			Rocket->Activate = 1;
			Rocket->TotalReleaseNumber = GetNumRocketSalvo();  //  WeapQuantity(GetWeapId(LAU10_ID)) + WeapQuantity(GetWeapId(LAU68_ID));
			Rocket->StartIndex = GetNextActiveBombStation(0);
			if (Rocket->StartIndex == -1)
				Rocket->Activate = 0;
		    Rocket->OneTimeDropNum = NumBombStationsSelected();
		 }
	}

	if(!Fired)
	{
		WeaponReleaseSimType *Bomb = &Av.Weapons.Bomb.Drop;

		switch (Av.Weapons.Bomb.BombMethod)
		{
			case SNGL_BMB:
				ReleaseSingleBomb();
			break;

			case MLT_RPL_BMB:
				Bomb->Activate = 1;
				Bomb->OneTimeDropNum = NumBombStationsSelected();
				Bomb->Interval = Av.Weapons.Bomb.BombInterval;
				Bomb->TotalReleaseNumber = Av.Weapons.Bomb.NumRplBmbs;
				Bomb->StartIndex = GetNextActiveBombStation(0);
				if (Bomb->StartIndex == -1)
					Bomb->Activate = 0;
			break;
			case SNGL_RPL_BMB:
				Bomb->Activate = 1;
				Bomb->Interval = Av.Weapons.Bomb.BombInterval;
				Bomb->TotalReleaseNumber = Av.Weapons.Bomb.NumRplBmbs;
				Bomb->StartIndex = GetNextActiveBombStation(0);
				if (Bomb->StartIndex == -1)
					Bomb->Activate = 0;
				Bomb->OneTimeDropNum = 1;
			break;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetNextMissileInSequence(AvWeapEntryType *Weap)
{
	AvWeapEntryType *W  = Weap;
	AvWeapEntryType *Walk = &Av.Weapons.AASeqList[0];
	AvWeapEntryType *LastW = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

	while(Walk <= LastW)       // un-select all weaps
	{
	  Walk->Selected = FALSE;
	  Walk++;
	}

	int Count = 0;
	while(Count <= AV_MAX_WEAPONS)
	{
			if(W != Weap)
			{
				if( (W->W->Status == Weap->W->Status) && (W->W->Count > 0) )
				{
				  W->Selected = TRUE;
			      Av.Weapons.CurAASeqWeap = W;
				  return;
				}
		  }
	  	Count++;
		  W++;
	    if(W > LastW) W = &Av.Weapons.AASeqList[0];
	}

	// nothing found
	Av.Weapons.CurAASeqWeap = NULL;
	Av.Weapons.CurrWeapMode = NO_WEAP_STAT;
	Av.Weapons.CurAAWeap = NULL;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CheckEasyModeMissileReject()
{
	if(!Av.Weapons.CurAAWeap) return;

	if(!Av.Weapons.CurAAWeap->Selected) return;

	int TypeToLookFor = Av.Weapons.CurAAWeap->W->Status;

	AvWeapEntryType *W,*Walk,*Walk2;
  AvWeapEntryType *FirstW;
	AvWeapEntryType *LastW;

  FirstW = &Av.Weapons.AASeqList[0];
  LastW  = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

	W = FirstW;
	while(W <= LastW)
	{
		if( (W->W->Status == TypeToLookFor) && (W->W->Count > 0) && (W->W != Av.Weapons.CurAAWeap->W) )
		{
			Walk2 = &Av.Weapons.AAList[0];
			while(Walk2 <= &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1])
			{
				Walk2->Selected = FALSE;
				Walk2++;
			}
	    Av.Weapons.CurAAWeap = NULL;

   	  Walk = FirstW;
			while(Walk <= LastW)
			{
				Walk->Selected = FALSE;
				Walk++;
			}
 		  Av.Weapons.CurAASeqWeap = W;
		  W->Selected = TRUE;
		  Av.Weapons.UseAASeq = TRUE;
		  Av.Weapons.CurAAWeap = NULL;
			Av.Weapons.CurrWeapMode = TypeToLookFor;
		  break;
 		}
		W++;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void MissleReject()
{
 	if(g_dwAIMSndHandle)
 		KillSound( &g_dwAIMSndHandle );
	if(g_dwAIMLockSndHandle)
		KillSound( &g_dwAIMLockSndHandle );

	if(Av.Weapons.CurrWeapMode == NO_WEAP_STAT) return;  // not in mrm or srm

	if(!Av.Weapons.UseAASeq)
	{
    CheckEasyModeMissileReject();
		return;
	}

	int StatToCheck = Av.Weapons.CurrWeapMode;

	AvWeapEntryType *Walk,*Found,*W;
	AvWeapEntryType *FirstW = &Av.Weapons.AASeqList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

	Found = NULL;
	Walk = FirstW;
	while(Walk <= LastW)
	{
		if(Walk->Selected)
		{
			W = Walk;
			int Count = 0;
	    while(Count <= AV_MAX_WEAPONS)
	    {
			  if(W != Walk)
			  {
				  if( (W->W->Status == StatToCheck) && (W->W->Count > 0) )
				  {
					 Found = W;
					 break;
				  }
				}
				Count++;
		    W++;
	      if(W > LastW) W = FirstW;
			}
		}
		if(Found)
		{
			Walk->Selected  = FALSE;
			Found->Selected = TRUE;
			Av.Weapons.CurAASeqWeap = Found;
			break;
		}
	  Walk++;
	}
}

//*****************************************************************************************************************************************
// MISC WEAP FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int WeapQuantity(int WeapId)
{
  AvWeapEntryType *W;
  AvWeapEntryType *FirstW = &Av.Weapons.AGList[0];
	AvWeapEntryType *LastW  = &Av.Weapons.AGList[Av.Weapons.NumAGWeapons-1];
	int Count = 0;

	W = FirstW;
	while(W <= LastW)
	{
		if(W->W->WeapId == WeapId)
		{
			if(W->W->Count > 0)      // not sure if count can go below 0, just in case
				Count += W->W->Count;
		}
		W++;
	}

  FirstW = &Av.Weapons.AAList[0];
	LastW  = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

	W = FirstW;
	while(W <= LastW)
	{
		if(W->W->WeapId == WeapId)
		{
			if(W->W->Count > 0)      // not sure if count can go below 0, just in case
				Count += W->W->Count;
		}
		W++;
	}

	return(Count);

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

BOOL PrimaryIsAOT(DetectedPlaneListType *Plist);

void ProcessSparrowFire()
{
	int CurWeapId = GetSelectedAAWeaponId();
	if( (CurWeapId == GetWeapId(AIM7F_ID)) || (CurWeapId == GetWeapId(AIM7M_ID)) || (CurWeapId == GetWeapId(AIM7MH_ID)))
	{
		if(!PrimaryIsAOT(&CurFramePlanes) && (Av.AARadar.CurMode != AA_STT_MODE))
		{
			int Result = SetAARadarToSTTMode();

			WeapStores.FloodOn = !Result;      // flood is on if no target is found

			if(WeapStores.FloodOn)
			{
	        AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;
					if(CurAAWeap)
					{
						float Time;
            float DistFtPerSec = WGetLaunchAvgSpeed(CurAAWeap->W->WeapIndex,10.0*NMTOFT,PlayerPlane->V,PlayerPlane->Altitude,&Time);
						float TimeToTravel = 10.0*NMTOFT/DistFtPerSec;
						WeapStores.FloodTime = TimeToTravel;
						WeapStores.FloodTimer.Set(5.0,GameLoopInTicks);
					}
			}
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProcessAim120Fire(int Station,int *TargetFound, PlaneParams **Target)
{
   *TargetFound = FALSE;

   if( (UFC.MasterMode != AG_MODE) && /*AreMpdsInGivenMode(RADAR_MODE) &&*/ (Av.AARadar.CurMode == AA_TWS_MODE) )
   {
     if((PlayerPlane->WeapLoad[Station].WeapId == Aim120_Id))
		 {
	     *Target = GetTargetForAim120();
			 *TargetFound = (*Target != NULL);
		 }
   }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FirePlayerMissile(int Station)
{
	int TargetFound;
	PlaneParams *Target = NULL;

	ProcessSparrowFire();
	ProcessAim120Fire(Station,&TargetFound,&Target);

	if(TargetFound)
    InstantiateMissile(PlayerPlane,Station,NULL,0,Target);
	else
	  InstantiateMissile(PlayerPlane, Station);

	if(TargetFound && (Target != NULL ) )
      AddMissleToChasingMissleList(Target);
  else
	{
	  if(PlayerPlane->AADesignate != NULL)
		{
	    AvWeapEntryType *CurAAWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;
			if( !((CurAAWeap->WeapPage == AIM120_WEAP_PAGE) && (WeapStores.Aim120VisualOn)) )
			   AddMissleToChasingMissleList(PlayerPlane->AADesignate);
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FireAAWeap()
{
	if(!Av.Weapons.UseAASeq)
	{
    AvWeapEntryType *Walk;
    AvWeapEntryType *FirstW = &Av.Weapons.AAList[0];
   	AvWeapEntryType *LastW  = &Av.Weapons.AAList[Av.Weapons.NumAAWeapons-1];

		Walk = FirstW;
	  while(Walk <= LastW)
	  {
	   if(Walk->Selected)
	   {
		   PlayFFEffect(FF_MISSILE_FIRING);
			 FirePlayerMissile(Walk->Station);
		   if(cPlayerLimitWeapons)
		     Walk->W->Count--;
		   if(Walk->W->Count <= 0)
		   {
				PlaneHasAAWeapons(PlayerPlane);
			   Walk->Selected = FALSE;
				 if(Walk+1 <= LastW)
				 {
				   if( (Walk->W->WeapIndex == (Walk+1)->W->WeapIndex) && ((Walk+1)->W->Count > 0) )
					 {
				   	 Av.Weapons.CurAAWeap = Walk+1;
						 (Walk+1)->Selected = TRUE;
					 }
					 else
						SelectNextAAWeapon();
				 }
				 else
				 {
				   SelectNextAAWeapon();
				 }
			   break;
		   }
	   }
	   Walk++;
	  }
	}
	else
	{
    AvWeapEntryType *Walk;
    AvWeapEntryType *FirstW = &Av.Weapons.AASeqList[0];
	  AvWeapEntryType *LastW  = &Av.Weapons.AASeqList[Av.Weapons.NumAASeqWeapons-1];

  	Walk = FirstW;
	  while(Walk <= LastW)
	  {
	   if(Walk->Selected)
	   {
		   PlayFFEffect(FF_MISSILE_FIRING);
			 FirePlayerMissile(Walk->Station);
		   if(cPlayerLimitWeapons)
			   Walk->W->Count--;
			 if(Walk->W->Count <= 0)
			 {
				PlaneHasAAWeapons(PlayerPlane);
			   GetNextMissileInSequence(Walk);
			 }
			 break;
	   }
	   Walk++;
	  }
 	}

	if( !HasAAWeapSelected() )
	{
		Av.Weapons.CurAAWeap = NULL;
	  Av.Weapons.CurAASeqWeap = NULL;
	}

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Button2Press()
{
	if( (UFC.MasterArmState != ON) ) return;
	if( (PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) ) return;

	if(UFC.MasterMode == AA_MODE)
	{
		FireAAWeap();
	}
	else if(UFC.MasterMode == AG_MODE)
	{
		if(Av.Gbu.WeapFired) return; // can't fire any agweapons until walleye hits the ground
		FireAGWeap();
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void Button1Press()
{
	if(WeapStores.GunsOn && WeapStores.AGunOn)
	{
	  if(!WeapStores.AGunCanFire)
		  return;
	}

	if( (PlayerPlane->WeapLoad[GUNS_STATION].Count > 0) && (UFC.MasterArmState == ON) && !(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED) )
  {
	  if(cPlayerLimitWeapons)
		  PlayerPlane->WeapLoad[GUNS_STATION].Count-=2;
	  InstantiateBullet(PlayerPlane,TRUE);
//  	  PlayFFEffect(FF_GUN_RECOIL_HI);

		if (!PlayerPlane->MachineGunLight)
		{
			if ((PlayerPlane->Type->MachineGun.Z) && (PlayerPlane->MachineGunLight = (PointLightSource **)GetNewLight()))
			{
				PointLightSource *new_light = new PointLightSource;
				new_light->WorldPosition = PlayerPlane->WorldPosition;
		//		new_light->Color = WhiteishFireColor;
				new_light->Color.SetValues(0.85,0.85,0.75);
				new_light->Flags |= LIGHT_TINY_SPOT;
				new_light->Radius1 = 20.0 FEET;
				new_light->ooRadius1 = 1.0f/(20.0 FEET);
				new_light->Radius2 = 5.0 FEET;
				new_light->ooRadius2 = 1.0f/(5.0 FEET);
				*(PlayerPlane->MachineGunLight) = new_light;
			}
		}
		if (PlayerPlane->MachineGunLight)
			(*PlayerPlane->MachineGunLight)->Intensity=1.0f;

  }
  else
	if (PlayerPlane->MachineGunLight)
	{
		RemoveLight((LightSource **)PlayerPlane->MachineGunLight);
		PlayerPlane->MachineGunLight = NULL;
	}
}

//*****************************************************************************************************************************************
//  FORCE FEEDBACK SIM FUNCTIONS
//*****************************************************************************************************************************************

ForceFeedType ForceFeed;

void InitFFVars()
{
  ForceFeed.DitchScrapeOn = FALSE;
  ForceFeed.DitchScrapeDecOn = FALSE;
  ForceFeed.Button1Press = FALSE;
  ForceFeed.CrashOn = FALSE;
  ForceFeed.OnGround = PlayerPlane->OnGround;
  ForceFeed.LastVel = -1;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FFSimButton1Fire()
{

	int tempval = 0;

   if((PlayerPlane->FlightStatus & PL_STATUS_CRASHED) || (PlayerPlane->AI.iAIFlags1 & AI_HAS_EJECTED)
		|| (!(PlayerPlane->WeapLoad[GUNS_STATION].Count > 0) && (UFC.MasterArmState == ON) && !(PlayerPlane->FlightStatus & PL_GEAR_DOWN_LOCKED)))
   {
	   if(ForceFeed.Button1Press)
	   {
//		 if(PlayerPlane->GunFireRate==1)
			StopFFEffect(FF_GUN_RECOIL_HI);
//		 else
			StopFFEffect(FF_GUN_RECOIL_LO);

		 ForceFeed.Button1Press = FALSE;
	   }
   }
   else if( (PlayerPlane->Trigger1) && (!ForceFeed.Button1Press) )
   {
     ForceFeed.Button1Press = TRUE;

	 if(PlayerPlane->GunFireRate==1)
        PlayFFEffect(FF_GUN_RECOIL_HI);
	 else
        PlayFFEffect(FF_GUN_RECOIL_LO);

   }
   else if( (!PlayerPlane->Trigger1) && (ForceFeed.Button1Press) )
   {
//	 if(PlayerPlane->GunFireRate==1)
        StopFFEffect(FF_GUN_RECOIL_HI);
//	 else
        StopFFEffect(FF_GUN_RECOIL_LO);

     ForceFeed.Button1Press = FALSE;
   }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FFSimDitch()
{
  if( !(PlayerPlane->FlightStatus & PL_PLANE_DITCHING) ) return;

  if(!ForceFeed.DitchScrapeOn)
  {
    PlayFFEffect(FF_DITCH_SCRAPE);
    ForceFeed.DitchScrapeOn = TRUE;
	ForceFeed.LastVel = PlayerPlane->V;
	ForceFeed.LastTick = GameLoopInTicks;
  }
  else
  {
	int   DeltaTick  = GameLoopInTicks - ForceFeed.LastTick;

	if(DeltaTick > 0)
	{
	  float Diff       = PlayerPlane->V - ForceFeed.LastVel;
	  float Dv         = Diff/( (float)DeltaTick/1000.0f );
	  float TimeToStop = PlayerPlane->V / fabs(Dv);

      if(TimeToStop <= 3.5 )
 	  {
        //StopFFEffect(FF_DITCH_SCRAPE);
	    PlayFFEffect(FF_DITCH_SCRAPE_DECREASE);
        ForceFeed.DitchScrapeDecOn = FALSE;
 	  }
      ForceFeed.LastVel = PlayerPlane->V;
      ForceFeed.LastTick = GameLoopInTicks;
	}
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FFLandThump()
{
	static int FrameWait = 24;
	if( FrameWait)
		FrameWait--;

  if( !FrameWait && (PlayerPlane->OnGround) && (!ForceFeed.OnGround)   )
  {
    PlayFFEffect(FF_GROUND_RUNWAY_BUMP);
    ForceFeed.OnGround = TRUE;
	FrameWait = 24;
  }

  ForceFeed.OnGround = PlayerPlane->OnGround;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void FFCrash()
{
  if( !(PlayerPlane->FlightStatus & PL_STATUS_CRASHED) ) return;

  if(!ForceFeed.CrashOn)
  {

	if(PlayerPlane->TerrainType == TT_WATER)
	    PlayFFEffect(FF_DITCH_WATER);
	else
        PlayFFEffect(FF_CRASH);

    ForceFeed.CrashOn = TRUE;

   if(ForceFeed.Button1Press)
   {
	 if(PlayerPlane->GunFireRate==1)
		StopFFEffect(FF_GUN_RECOIL_HI);
	 else
		StopFFEffect(FF_GUN_RECOIL_LO);

	 ForceFeed.Button1Press = FALSE;
   }
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimGForce()
{
	float fworkvar, fworkvar2;
	float TempGs = fabs(PlayerPlane->GForce);
	if(TempGs < 1.0) TempGs = 1.0;
	if(TempGs > 9.0) TempGs = 9.0;

	float Percent = 1.0 - ( (9.0 - TempGs)/8.0 );
	int Magnitude = 100*Percent;

	if(TempGs >= 9.0)
	{
	   PlayFFEffect(FF_TRIM_FORCES);
	}
	else
	{
#if 0
	  if     (TempGs > 7.0) Magnitude = 90;
	  else if(TempGs > 6.0) Magnitude = 60;
	  else if(TempGs > 5.0) Magnitude = 50;
	  else if(TempGs > 4.0) Magnitude = 40;
	  else if(TempGs > 3.0) Magnitude = 20;
	  else Magnitude = 10;
#else
	  if(TempGs <= 3.0)
	  {
		  Magnitude = lFFBaseGMag;
	  }
	  else
	  {
		  fworkvar = (TempGs - 3.0f) / 4.0f;  //  Get percent between 3 and 7g's.  Remember the G limiter
		  fworkvar *= 90.0f;
		  if(fworkvar < 0)
			  fworkvar = 0;
		  else if(fworkvar > 90)
			  fworkvar = 90;

		  fworkvar = 1 - cos(DegToRad(fworkvar));
		  if(fworkvar < 0)
			  fworkvar = 0;
		  else if(fworkvar > 1.0f)
			  fworkvar = 1.0f;

		  fworkvar2 = ((90.0f - (float)lFFBaseGMag) * fworkvar) + (float)lFFBaseGMag;
		  Magnitude = (int)fworkvar2;
	  }
#endif

	  PlayFFEffect(FF_TRIM_FORCES,FF_SET_SPRING,&Magnitude);
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SimFF()
{
	if(!g_Settings.ff.bEnable) return;

	FFCrash();
	FFLandThump();
	FFSimButton1Fire();
	FFSimDitch();
	SimGForce();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************
void StopFFForPauseOrEnd()
{
	if(!g_Settings.ff.bEnable) return;

   if(ForceFeed.Button1Press)
   {
//	 if(PlayerPlane->GunFireRate==1)
		StopFFEffect(FF_GUN_RECOIL_HI);
//	 else
		StopFFEffect(FF_GUN_RECOIL_LO);

	 ForceFeed.Button1Press = FALSE;
   }
}