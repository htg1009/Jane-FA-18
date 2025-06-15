#include "F18.h"
#include "3dfxF18.h"
#include "gamesettings.h"

#define ASSERT

//*****************************************************************************************************************************************
// AVIONICS SENSOR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// RUNWAY GLOBAL  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

int AvNumGlobalRunways;
RunwayInfo *AvGlobalRunwayList[300];

//*****************************************************************************************************************************************
// GLOBAL AVIONICS VARS  -- *Header -- (search "*Header" to step through each section of file)
//*****************************************************************************************************************************************

AvionicsType Av;

extern BasicInstance *AllInstances;

// prototype
//FPoint PlanePos,EnemyPos;
//Dist = PlanePos.Dist2DTo(&EnemyPos);

//*****************************************************************************************************************************************
// RUNWAY FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

extern RunwayInfo *AllRunways;

int IsUniqueRunway(FPointDouble Ref)
{
  RunwayInfo **Walk = &AvGlobalRunwayList[0];
  int Index = AvNumGlobalRunways;
  while(Index-- > 0)
  {
    double Dx = fabs(Ref.X - (*Walk)->ILSLandingPoint.X);
    double Dz = fabs(Ref.Z - (*Walk)->ILSLandingPoint.Z);

    if( (Dx*WUTONM < 2.0) && (Dz*WUTONM < 2.0) )
      return(FALSE);
    Walk++;
  }

  return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CreateRunwayList()
{
  RunwayInfo *Runway = AllRunways;
  RunwayInfo **Walk = &AvGlobalRunwayList[0];

  AvNumGlobalRunways = 0;
  while(Runway)
  {
    if(IsUniqueRunway(Runway->ILSLandingPoint))
    {
      AvNumGlobalRunways++;
      *Walk = Runway;
      Walk++;
    }
    Runway = Runway->NextRunway;
  }
}

//*****************************************************************************************************************************************
// GET PRIMARY GROUND TARGET  -- *Header -- (search "*Header" to step through each section of file)
//*****************************************************************************************************************************************

int IsPlanePrimaryOrSecondary(PlaneParams *P)
{
	if(Av.MissionAATargets.NumAATargets <= 0) return(-1);

   AvObjListEntryType *Walk = &Av.MissionAATargets.List[0];

	 int Index = Av.MissionAATargets.NumAATargets;
	 while(Index-- > 0)
	 {
		 if(Walk->Obj == P)
		 {
			 return(Walk->Category);
		 }

		 Walk++;
	 }

	 return(-1);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddPlaneToMissionTargetList(int Category, int Type, void *Obj)
{
	if(Av.MissionAATargets.NumAATargets+1 >= AV_MAX_AG_TARGETS) return;

	if(Category == AV_PRIMARY)
	{
		if(Av.MissionAATargets.NumAATargets <= 0)
		{
			Av.MissionAATargets.NumAATargets = 1;
	    Av.MissionAATargets.List[0].Obj      = Obj;
	    Av.MissionAATargets.List[0].Type     = Type;
	    Av.MissionAATargets.List[0].Category = Category;
		}
		else
		{
      AvObjListEntryType *Ptr = &Av.MissionAATargets.List[0];
      AvObjListEntryType *End = &Av.MissionAATargets.List[Av.MissionAATargets.NumAATargets-1];

			while(End >= Ptr)
			{
				memcpy((End+1),End,sizeof(AvObjListEntryType));
				End--;
			}

	    Ptr->Obj      = Obj;
	    Ptr->Type     = Type;
	    Ptr->Category = Category;

			Av.MissionAATargets.NumAATargets++;
		}
	}
	else
	{
		Av.MissionAATargets.NumAATargets++;

		AvObjListEntryType *Ptr = &Av.MissionAATargets.List[Av.MissionAATargets.NumAATargets-1];

		Ptr->Obj      = Obj;
	  Ptr->Type     = Type;
	  Ptr->Category = Category;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddObjToMissionTargetList(int Category, int Type, void *Obj)
{
	if(Av.MissionAGTargets.NumAGTargets+1 >= AV_MAX_AG_TARGETS) return;

	if(Category == AV_PRIMARY)
	{
		if(Av.MissionAGTargets.NumAGTargets <= 0)
		{
			Av.MissionAGTargets.NumAGTargets = 1;
	    Av.MissionAGTargets.List[0].Obj      = Obj;
	    Av.MissionAGTargets.List[0].Type     = Type;
	    Av.MissionAGTargets.List[0].Category = Category;
		}
		else
		{
      AvObjListEntryType *Ptr = &Av.MissionAGTargets.List[0];
      AvObjListEntryType *End = &Av.MissionAGTargets.List[Av.MissionAGTargets.NumAGTargets-1];

			while(End >= Ptr)
			{
				memcpy((End+1),End,sizeof(AvObjListEntryType));
				End--;
			}

	    Ptr->Obj      = Obj;
	    Ptr->Type     = Type;
	    Ptr->Category = Category;

			Av.MissionAGTargets.NumAGTargets++;
		}
	}
	else
	{
		Av.MissionAGTargets.NumAGTargets++;

		AvObjListEntryType *Ptr = &Av.MissionAGTargets.List[Av.MissionAGTargets.NumAGTargets-1];

		Ptr->Obj      = Obj;
	  Ptr->Type     = Type;
	  Ptr->Category = Category;
	}
}

//*****************************************************************************************************************************************
extern int VConvertVNumToGVListNum(int);

//*****************************************************************************************************************************************
void GetPrimaryAndSecondaryGroundTargets()
{
	Av.MissionAGTargets.NumAGTargets = 0;
	Av.AG.PrimeTarget = NULL;
	Av.MissionAATargets.NumAATargets = 0;
	int iParamIndex=0;

	MBGoalInfo *Walk = GoalList;

	for(int i=0; i<iNumGoals; i++)
	{
		if(Walk->lGoalRatingType)
		{
			if(Walk->lObjectType == GROUNDOBJECT)
			{
				BasicInstance *BInst = AllInstances;
				while(BInst)
				{
					if(Walk->dwSerialNumber == BInst->SerialNumber)
					{
						AddObjToMissionTargetList( (Walk->lGoalRatingType == 1) ? AV_PRIMARY : AV_SECONDARY, Walk->lObjectType, BInst);
						break;
					}
  					BInst = (BasicInstance *)BInst->NextInstance;
				}
			}
			else if(Walk->lObjectType == MOVINGVEHICLE)
			{
				iParamIndex=VConvertVGListNumToVNum(Walk->dwSerialNumber);
				MovingVehicleParams *V = &MovingVehicles[iParamIndex];
				if ((V->Status & VL_ACTIVE) && !(V->Status & VL_INVISIBLE))
				{
					AddObjToMissionTargetList( (Walk->lGoalRatingType == 1) ? AV_PRIMARY : AV_SECONDARY, Walk->lObjectType,V);
				}
			 }
			 else if(Walk->lObjectType == SHIP)
			 {
				iParamIndex=VConvertVGListNumToVNum(Walk->dwSerialNumber);
				MovingVehicleParams *S = &MovingVehicles[iParamIndex];
				if ((S->Status & VL_ACTIVE) && !(S->Status & VL_INVISIBLE))
				{
					AddObjToMissionTargetList( (Walk->lGoalRatingType == 1) ? AV_PRIMARY : AV_SECONDARY,MOVINGVEHICLE,S);
				}
			 }
			 else if(Walk->lObjectType == AIRCRAFT)
			 {
				PlaneParams *P = (PlaneParams *)&Planes[Walk->dwSerialNumber];
				AddPlaneToMissionTargetList((Walk->lGoalRatingType == 1) ? AV_PRIMARY : AV_SECONDARY,Walk->lObjectType,P);
			 }
		}
		Walk++;
	}

	if(!g_iAreaGoal) return;

	float Left,Right,Top,Bottom;

	Left   = AreaGoal.x0*FTTOWU;
	Right  = AreaGoal.x1*FTTOWU;
	Top    = AreaGoal.z0*FTTOWU;
	Bottom = AreaGoal.z1*FTTOWU;

	BasicInstance *BInst = AllInstances;
	while(BInst)
	{
		int Inside = IsPointInside(BInst->Position.X,BInst->Position.Z,Top,Bottom,Left,Right);

		if(Inside)
		{
			AddObjToMissionTargetList(AV_NORMAL,GROUNDOBJECT,BInst);
		}
		BInst = (BasicInstance *)BInst->NextInstance;
	}

	MovingVehicleParams *Veh = &MovingVehicles[0];
	while(Veh <= LastMovingVehicle)
	{
		if ((Veh->Status & VL_ACTIVE) && !(Veh->Status & VL_INVISIBLE))
		{
			int Inside = IsPointInside(Veh->WorldPosition.X,Veh->WorldPosition.Z,Top,Bottom,Left,Right);

			if(Inside)
			{
				AddObjToMissionTargetList(AV_NORMAL,MOVINGVEHICLE,Veh);
			}
		}
		Veh++;
	}
}

//*****************************************************************************************************************************************
// SENSOR  INIT  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void InitMissionHelp()
{
 Av.MH.MissileLaunch.NumMissileLaunch = 0;
 InitDirectionArrow();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitSensor()
{
 Av.Sensor.Range = 100.0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitAvCamera()
{
	Av.Camera.Range = 40.0;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitAvSensorData()
{
 Av.AA.UseAARadarForTargets = FALSE;
 Av.AA.PrimeTarget.Obj = NULL;
 Av.AG.UseAGRadarForTargets = FALSE;
 Av.AG.PrimeTarget = NULL;

 InitMissionHelp();
 InitSensor();
 InitAvCamera();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void InitAvionicsSensor()
{
  CreateRunwayList();
	GetPrimaryAndSecondaryGroundTargets();
	InitAvSensorData();
	InitAvionicsModes();
	CreateAvColorRamp();
	InitMpdInteractions();
  InitF18Cockpit();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void CleanupAvionicsSensor()
{
}

//*****************************************************************************************************************************************
// SITUATIONAL AWARENESS FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void GetPrimaryAAData(float Range)
{
  FPoint	Vec;
	int Found = FALSE;

  if(Av.AA.PrimeTarget.Obj == NULL) return;

  float Left,Right,Top,Bottom;
  Left   = PlayerPlane->WorldPosition.X - Range*NMTOWU;
  Right  = PlayerPlane->WorldPosition.X + Range*NMTOWU;
  Top    = PlayerPlane->WorldPosition.Z - Range*NMTOWU;
  Bottom = PlayerPlane->WorldPosition.Z + Range*NMTOWU;

  PlaneParams *P = (PlaneParams *)Av.AA.PrimeTarget.Obj;

  if( !(P->Status & PL_ACTIVE) || (P->FlightStatus & PL_STATUS_CRASHED) )
	{
		 Av.AA.PrimeTarget.Obj = NULL;
		 return;
	}

  if(P->WorldPosition.X > Left)
    if(P->WorldPosition.X < Right)
      if(P->WorldPosition.Z < Bottom)
        if(P->WorldPosition.Z > Top)
		    {
          Av.AA.PrimeTarget.Dist = Dist2D(&PlayerPlane->WorldPosition,&P->WorldPosition);
          Av.AA.PrimeTarget.RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,P->WorldPosition,TRUE);

			    Av.AA.PrimeTarget.Ping    = (P->AI.iAIFlags2 & AI_RADAR_PING);
   			  Av.AA.PrimeTarget.HasLock = ( (P->AI.AirTarget == PlayerPlane) &&
				                          (P->AI.iAIFlags2 & AI_RADAR_PING) &&
				                          (P->AI.iAIFlags2 & AILOCKEDUP) );
					Vec  = P->WorldPosition;
			    Vec -= PlayerPlane->WorldPosition;
			    Vec *= ViewMatrix;
			    Vec.Perspect(&Av.AA.PrimeTarget.ScrX,&Av.AA.PrimeTarget.ScrY);
					Found = TRUE;
	      }

	if(!Found)
	{
		Av.AA.PrimeTarget.Obj = NULL;
		return;
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetPlanesInRadius(float Range)
{
  PlaneParams *P = &Planes[0];
  FPoint	Vec;

  float Left,Right,Top,Bottom;
  Left   = PlayerPlane->WorldPosition.X - Range*NMTOWU;
  Right  = PlayerPlane->WorldPosition.X + Range*NMTOWU;
  Top    = PlayerPlane->WorldPosition.Z - Range*NMTOWU;
  Bottom = PlayerPlane->WorldPosition.Z + Range*NMTOWU;

  AvObjListEntryType *TargetPlane = &Av.Planes.List[0];
  Av.Planes.NumPlanes = 0;

  while(P <= LastPlane)
  {
   if( (P->Status & PL_ACTIVE) && (P != PlayerPlane) && (!P->OnGround) && !(P->FlightStatus & (PL_STATUS_CRASHED)) )
   {
       if(P->WorldPosition.X > Left)
         if(P->WorldPosition.X < Right)
           if(P->WorldPosition.Z < Bottom)
             if(P->WorldPosition.Z > Top)
		         {
		           if(Av.Planes.NumPlanes+1 >= AV_MAX_PLANES) return;
			         Av.Planes.NumPlanes++;
			         TargetPlane->Obj  = (PlaneParams *)P;
							 TargetPlane->Index = P - &Planes[0];
							 TargetPlane->Type = AV_PLANE;
               TargetPlane->Dist = Dist2D(&PlayerPlane->WorldPosition,&P->WorldPosition);
               TargetPlane->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,P->WorldPosition,TRUE);

			         TargetPlane->Ping    = (P->AI.iAIFlags2 & AI_RADAR_PING);
   			       TargetPlane->HasLock = ( (P->AI.AirTarget == PlayerPlane) &&
				                                (P->AI.iAIFlags2 & AI_RADAR_PING) &&
				                                (P->AI.iAIFlags2 & AILOCKEDUP) );
							 Vec  = P->WorldPosition;
			         Vec -= Camera1.CameraLocation;
			         Vec *= ViewMatrix;
			         TargetPlane->InView = Vec.Perspect(&TargetPlane->ScrX,&TargetPlane->ScrY);


							 TargetPlane++;
	           }
	  }
  	P++;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetVehiclesInRadius(float Range)
{
  MovingVehicleParams *VehiclePtr = MovingVehicles;
  FPoint	Vec;

  float Left,Right,Top,Bottom;
  Left   = PlayerPlane->WorldPosition.X - Range*NMTOWU;
  Right  = PlayerPlane->WorldPosition.X + Range*NMTOWU;
  Top    = PlayerPlane->WorldPosition.Z - Range*NMTOWU;
  Bottom = PlayerPlane->WorldPosition.Z + Range*NMTOWU;

  AvObjListEntryType *Vehicle = &Av.Vehicles.List[0];
  Av.Vehicles.NumVehicles = 0;

  while(VehiclePtr <= LastMovingVehicle)
  {
    if( (VehiclePtr->Status & VL_ACTIVE) && (!(VehiclePtr->Status & VL_INVISIBLE)) )
	  {
         if(VehiclePtr->WorldPosition.X > Left)
          if(VehiclePtr->WorldPosition.X < Right)
           if(VehiclePtr->WorldPosition.Z < Bottom)
            if(VehiclePtr->WorldPosition.Z > Top)
		        {
		          if(Av.Vehicles.NumVehicles+1 >= AV_MAX_VEHICLES) return;
			         Av.Vehicles.NumVehicles++;
			         Vehicle->Obj  = (MovingVehicleParams *)VehiclePtr;
							 Vehicle->Type = AV_VEHICLE;
							 Vehicle->Dist = Dist2D(&PlayerPlane->WorldPosition,&VehiclePtr->WorldPosition);
               Vehicle->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,VehiclePtr->WorldPosition,TRUE);
			         Vehicle->Ping = FALSE;
   			       Vehicle->HasLock = FALSE;

							 Vec  = VehiclePtr->WorldPosition;
			         Vec -= Camera1.CameraLocation;
			         Vec *= ViewMatrix;
			         Vec.Perspect(&Vehicle->ScrX,&Vehicle->ScrY);

			         Vehicle++;
		          }
	   }
	   VehiclePtr++;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetGroundObjectsInRadius(float Range)
{
	RegisteredObject *Walk;
  BasicInstance *Binst;
  int Inside;
  FPoint	Vec;

  AvObjListEntryType *AGTarget = &Av.AGTargets.List[0];
  Av.AGTargets.NumAGTargets = 0;

	Walk = &RegisteredObjects[0];

  while(Walk <= LastRegisteredObject)
  {
		Inside = FALSE;
	  if (Walk->Flags & RO_OBJECT_FALLING)
	  	Binst = ((FallingObject *)Walk->Instance)->Instance;
	  else
		  Binst = Walk->Instance;

	  if(Binst)
	  {

			if (Walk->DistanceFromCamera)
				Inside = Walk->DistanceFromCamera < Range*NMTOWU;
			else
		  	{
        		float Left,Right,Top,Bottom;
        		Left   = PlayerPlane->WorldPosition.X - Range*NMTOWU;
        		Right  = PlayerPlane->WorldPosition.X + Range*NMTOWU;
        		Top    = PlayerPlane->WorldPosition.Z - Range*NMTOWU;
        		Bottom = PlayerPlane->WorldPosition.Z + Range*NMTOWU;

        		if(Binst->Position.X > Left)
          		if(Binst->Position.X < Right)
            		if(Binst->Position.Z < Bottom)
              		if(Binst->Position.Z > Top)
							  		Inside = TRUE;
			}


			if(Inside)
			{
				if(Av.AGTargets.NumAGTargets+1 >= AV_MAX_AG_TARGETS) return;
				Av.AGTargets.NumAGTargets++;
				AGTarget->Obj  = (RegisteredObject *)Walk;
				AGTarget->Type = AV_REGISTERED_OBJECT;
				AGTarget->Dist = Dist2D(&PlayerPlane->WorldPosition,&Binst->Position);
      	AGTarget->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Binst->Position,TRUE);
				AGTarget->Ping = FALSE;
   	  	AGTarget->HasLock = FALSE;

				Vec  = Binst->Position;
			  Vec -= Camera1.CameraLocation;
			  Vec *= ViewMatrix;
			  Vec.Perspect(&AGTarget->ScrX,&AGTarget->ScrY);

				AGTarget++;
			}

		} // if BI Required

		Walk++;
	}   // while

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetMissilesInRadius(float Range)
{
  WeaponParams *W = &Weapons[0];
  FPoint	Vec;

  float Left,Right,Top,Bottom;
  Left   = PlayerPlane->WorldPosition.X - Range*NMTOWU;
  Right  = PlayerPlane->WorldPosition.X + Range*NMTOWU;
  Top    = PlayerPlane->WorldPosition.Z - Range*NMTOWU;
  Bottom = PlayerPlane->WorldPosition.Z + Range*NMTOWU;

  AvObjListEntryType *Missile = &Av.Missiles.List[0];
  Av.Missiles.NumMissiles = 0;

  while(W <= LastWeapon)
  {
	   if(W->Pos.X > Left)
	     if(W->Pos.X < Right)
	      if(W->Pos.Z < Bottom)
		      if(W->Pos.Z > Top)
             if( (W->Kind == MISSILE) && (W->Flags & WEAPON_INUSE) )
             {
		           if(Av.Missiles.NumMissiles+1 >= AV_MAX_MISSILES) return;
               Av.Missiles.NumMissiles++;
	             Missile->Obj = (WeaponParams *)W;
							 Missile->Type = AV_MISSILE;
			         Missile->Dist = Dist2D(&PlayerPlane->WorldPosition,&W->Pos);
			         Missile->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,W->Pos,TRUE);

               if( (W->iTargetType == TARGET_PLANE) && ((PlaneParams *)W->pTarget == PlayerPlane) )
			         {
   			         int WeaponIndex,Seeker;
				         WeaponIndex = AIGetWeaponIndex(W->Type->TypeNumber);
		             Seeker = pDBWeaponList[WeaponIndex].iSeekerType;
				         Missile->HasLock = ( (W->Flags & FINAL_PHASE) && (Seeker == 1) );
				         Missile->Ping = FALSE;

							 	 Vec  = W->Pos;
			         	 Vec -= Camera1.CameraLocation;
			         	 Vec *= ViewMatrix;
			         	 Vec.Perspect(&Missile->ScrX,&Missile->ScrY);
			         }
	             Missile++;
	          }
	          W++;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetRadarSites()
{
  InfoProviderInstance *Prov = &AllInfoProviders[0];
  GDRadarData *Radar;
  FPoint	Vec;

  AvObjListEntryType *RadarSite = &Av.RadarSites.List[0];
  Av.RadarSites.NumRadarSites = 0;

  while(Prov)
  {
    Radar = (GDRadarData *)Prov->AIDataBuf;

	  if(Radar->lRFlags1 & GD_RADAR_PLAYER_PING)
	  {
		    if(Av.RadarSites.NumRadarSites+1 >= AV_MAX_RADAR_SITES) return;
        Av.RadarSites.NumRadarSites++;
        RadarSite->Obj = (InfoProviderInstance *)Prov;
				RadarSite->Type = AV_RADAR_SITE;
		    RadarSite->Dist = Dist2D(&PlayerPlane->WorldPosition,&Prov->Basics.Position);
        RadarSite->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Prov->Basics.Position,TRUE);
		    RadarSite->Ping = TRUE;
		    RadarSite->HasLock = ( (Radar->lRFlags1 & GD_RADAR_LOCK) && (Radar->Target == PlayerPlane) &&
			                         (Radar->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) &&
							                 (!(Radar->lRFlags1 & GD_I_AM_DEAD)));

				Vec  = Prov->Basics.Position;
			  Vec -= Camera1.CameraLocation;
			  Vec *= ViewMatrix;
			  Vec.Perspect(&RadarSite->ScrX,&RadarSite->ScrY);

				RadarSite++;
	  }
    Prov = (InfoProviderInstance *)Prov->Basics.NextRelatedInstance;
  }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetSams()
{
  AAWeaponInstance *Weap = &AllAAWeapons[0];
  GDRadarData *Radar;
  FPoint	Vec;

  AvObjListEntryType *Sams = &Av.Sams.List[0];
  Av.Sams.NumSams = 0;

  while(Weap)
  {
    Radar = (GDRadarData *)Weap->AIDataBuf;

	  if(Radar->lRFlags1 & GD_RADAR_PLAYER_PING)
	  {
	    if(Av.Sams.NumSams+1 >= AV_MAX_SAMS) return;
        Av.Sams.NumSams++;
	    Sams->Obj  = (AAWeaponInstance *)Weap;
			Sams->Type = AV_SAM;
	    Sams->Dist = Dist2D(&PlayerPlane->WorldPosition,&Weap->Basics.Position);
      Sams->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,Weap->Basics.Position,TRUE);
	    Sams->Ping    = TRUE;
	    Sams->HasLock = ( (Radar->lRFlags1 & GD_RADAR_LOCK) && (Radar->Target == PlayerPlane) &&
		                    (Radar->lRFlags1 & (GD_RADAR_ON|GD_RADAR_TEMP_ON)) &&
						            (!(Radar->lRFlags1 & GD_I_AM_DEAD)));

			Vec  = Weap->Basics.Position;
			Vec -= Camera1.CameraLocation;
			Vec *= ViewMatrix;
			Vec.Perspect(&Sams->ScrX,&Sams->ScrY);

			Sams++;
	  }
   	Weap = (AAWeaponInstance *)Weap->Basics.NextRelatedInstance;
  }

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetRunwaysInRadius(float Range)
{
  AvObjListEntryType *Runway = &Av.Runways.List[0];
  RunwayInfo **Walk   = &AvGlobalRunwayList[0];
  FPoint	Vec;

  float Left,Right,Top,Bottom;
  Left   = PlayerPlane->WorldPosition.X - Range*NMTOWU;
  Right  = PlayerPlane->WorldPosition.X + Range*NMTOWU;
  Top    = PlayerPlane->WorldPosition.Z - Range*NMTOWU;
  Bottom = PlayerPlane->WorldPosition.Z + Range*NMTOWU;

  Av.Runways.NumRunways = 0;

  int Index = AvNumGlobalRunways;
  while(Index-- > 0)
  {
    if((*Walk)->ILSLandingPoint.X > Left)
	   if((*Walk)->ILSLandingPoint.X < Right)
	    if((*Walk)->ILSLandingPoint.Z < Bottom)
		   if((*Walk)->ILSLandingPoint.Z > Top)
		   {
		     if(Av.Runways.NumRunways+1 >= AV_MAX_RUNWAYS) return;
			   Av.Runways.NumRunways++;
		     Runway->Obj = (RunwayInfo *)(*Walk);
				 Runway->Type = AV_RUNWAY;
	       Runway->Dist = Dist2D(&PlayerPlane->WorldPosition,&(*Walk)->ILSLandingPoint);
         Runway->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,(*Walk)->ILSLandingPoint,TRUE);

				 Vec  = (*Walk)->ILSLandingPoint;
				 Vec -= Camera1.CameraLocation;
				 Vec *= ViewMatrix;
				 Vec.Perspect(&Runway->ScrX,&Runway->ScrY);

	       Runway++;
	      }
				Walk++;
  }
}


//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsObjInSamList(BasicInstance *Obj)
{
  AvObjListEntryType *Sam = &Av.Sams.List[0];

	int Index = Av.Sams.NumSams;
	while(Index-- > 0)
	{
		AAWeaponInstance *Weap = (AAWeaponInstance *)Sam->Obj;
		if(Obj == Weap->Registration->Instance)
			return(TRUE);
		Sam++;
	}
  return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsObjInRadarSiteList(BasicInstance *Obj)
{
  AvObjListEntryType *RadarSite = &Av.RadarSites.List[0];

	int Index = Av.RadarSites.NumRadarSites;
	while(Index-- > 0)
	{
		InfoProviderInstance *Prov = (InfoProviderInstance *)RadarSite->Obj;
		if(Obj == Prov->Registration->Instance)
			return(TRUE);
		RadarSite++;
	}
  return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int IsObjInMissionObjectList(int Type, void *Obj,AvObjListEntryType **Entry)
{
	if(Type == AV_SAM)
		Obj = (BasicInstance *)((AAWeaponInstance *)Obj)->Registration->Instance;
	else if(Type == AV_RADAR_SITE)
		Obj = (BasicInstance *)((InfoProviderInstance *)Obj)->Registration->Instance;

  AvObjListEntryType *MissionTarget = &Av.MissionAGTargets.List[0];

	int Index = Av.MissionAGTargets.NumAGTargets;
	while(Index-- > 0)
	{
		if(MissionTarget->Type == AV_REGISTERED_OBJECT)
		{
			if((BasicInstance *)Obj == (BasicInstance *)MissionTarget->Obj)
			{
				*Entry = MissionTarget;
				return(TRUE);
			}
		}
		else if(MissionTarget->Type == AV_VEHICLE)
		{
			if( (MovingVehicleParams *)Obj == (MovingVehicleParams *)MissionTarget->Obj)
			{
				*Entry = MissionTarget;
			  return(TRUE);
			}
		}
		MissionTarget++;
	}
  return(FALSE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetMissionObjectData()
{
   AvObjListEntryType *MissionTarget;

	 FPointDouble TargPos;
	 FPoint       Vec;

	 // JLM NOTE : ALL REGISTERED OBJECTS WILL NOT WORK....USE ALL_INSTANCES

   MissionTarget  = &Av.MissionAGTargets.List[0];
   int Index          = Av.MissionAGTargets.NumAGTargets;
   while(Index-- > 0)
   {
		 int InSamList  = FALSE;
		 int InProvList = FALSE;

		 // dont draw if already an object with radar
		 if(MissionTarget->Type == AV_REGISTERED_OBJECT)
		 {
		 	 InSamList  = IsObjInSamList((BasicInstance *)MissionTarget->Obj);
			 InProvList = IsObjInRadarSiteList((BasicInstance *)MissionTarget->Obj);
		 }

		 if( (!InSamList) && (!InProvList) && (MissionTarget->Type != AV_VEHICLE) )
		 {
				if(MissionTarget->Type == AV_REGISTERED_OBJECT)
				{
					TargPos = ((BasicInstance *)MissionTarget->Obj)->Position;
				}
				else
				{
           int Bug = 4;
				   break;
				}

	  		MissionTarget->Dist     = Dist2D(&PlayerPlane->WorldPosition,&TargPos);
	  		MissionTarget->RelAngle = ComputeHeadingToPointDouble(PlayerPlane->WorldPosition,PlayerPlane->Heading,TargPos,TRUE);

  			MissionTarget->HasLock = FALSE;
				MissionTarget->Ping    = FALSE;

				Vec  = TargPos;
				Vec -= PlayerPlane->WorldPosition;
				Vec *= ViewMatrix;
				Vec.Perspect(&MissionTarget->ScrX,&MissionTarget->ScrY);
		 }
     MissionTarget++;
   }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

RegisteredObject *GetRegisteredObjectForInstance(BasicInstance *Inst)
{
	RegisteredObject *Walk;
  BasicInstance *Binst;

	Walk = &RegisteredObjects[0];

  while(Walk <= LastRegisteredObject)
  {
	  if (Walk->Flags & RO_OBJECT_FALLING)
	  	Binst = ((FallingObject *)Walk->Instance)->Instance;
	  else
		  Binst = Walk->Instance;

	  if(Binst == Inst)
			return(Walk);

		Walk++;
	}
	return(NULL);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SetSensorAwareness()
{
  float MaxRange;

  //MaxRange = GetPlayerMaxRange();
  //MaxRange = 160.0; // JLM temporary
	MaxRange = Av.Sensor.Range;

  GetPlanesInRadius(MaxRange);
  GetVehiclesInRadius(MaxRange);
  GetMissilesInRadius(MaxRange);
  GetRunwaysInRadius(MaxRange);
  GetSams();
  GetRadarSites();
	GetGroundObjectsInRadius(MaxRange);
	GetMissionObjectData();
	GetPrimaryAAData(MaxRange);
}

//*****************************************************************************************************************************************
// DO ALL SENSOR  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

typedef struct
{
  AvObjListEntryType *Obj;
	int  XOff;
	int  YOff;
	int  Type;
	int  Category;
} SensorObjectType;

#define AV_MAX_SENSOR_OBJECTS 300

int NumSensorObjects;
SensorObjectType SensorObjects[AV_MAX_SENSOR_OBJECTS];

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AddObjectToSensorList( AvObjListEntryType *Obj,int Type,int Category,int XOff,int YOff)
{
  SensorObjectType *Ptr;

	if(NumSensorObjects+1 >= AV_MAX_SENSOR_OBJECTS)         return;
	if( (UFC.MasterMode == AA_MODE) && (Type != AV_PLANE) ) return;
	if( (UFC.MasterMode == AG_MODE) && (Type == AV_PLANE) ) return;

	NumSensorObjects++;

	Ptr = &SensorObjects[NumSensorObjects - 1];

	Ptr->Obj  = Obj;
	Ptr->XOff = XOff;
	Ptr->YOff = YOff;
	Ptr->Type = Type;
	Ptr->Category = Category;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AvSensorClick(int XOff, int YOff)
{
	int Found = FALSE;
	SensorObjectType *Walk = &SensorObjects[0];
	SensorObjectType *Last = &SensorObjects[NumSensorObjects - 1];
  SensorObjectType *Obj;

	while(Walk <= Last)
	{
		int Inside = IsPointInside(Walk->XOff,Walk->YOff,YOff - 10,YOff + 10,XOff - 10,XOff + 10);
		if(Inside)
		{
			Found = TRUE;
			Obj = Walk;
			break;
		}
		Walk++;
	}

	if(!Found) return;

	if(Obj->Type == AV_PLANE)
	{
	  Av.AA.PrimeTarget = *(Obj->Obj);
	}
	else if( (Obj->Type == AV_VEHICLE) || (Obj->Type == AV_SAM) || (Obj->Type == AV_RADAR_SITE) || (Obj->Type == AV_REGISTERED_OBJECT) )
	{
		AvObjListEntryType *Entry;
		int Result = IsObjInMissionObjectList(Obj->Type,Obj->Obj->Obj,&Entry);  // ok. I know it looks weird, obj->obj->obj....
	  if(Result)
		{
			Av.AG.PrimeTarget = Entry;
		}
	}
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void DrawDoAllSensor(int CenterX, int CenterY)
{
   FPointDouble PlanePos,MissilePos,SamPos,RadarSitePos,BoxPos,VehPos,Origin;
   float  DeltaAzim;
   float  NewX, NewZ;
   float  PixelsPerMile = 55.0/Av.Sensor.Range;
   int    NumPixels;
   float  Dist;
   float  EnemyHeading;
   int    Index,ObjIndex;

   AvObjListEntryType *Plane,*Sam,*RadarSite,*Missile,*Vehicle,*Runway,*MissionTarget;

   NumSensorObjects = 0;

   Plane = &Av.Planes.List[0];
   Index = Av.Planes.NumPlanes;
   while(Index-- > 0)
   {
     Dist = Plane->Dist*WUTONM;
     NumPixels = (int)(Dist)*PixelsPerMile;
   	 if(NumPixels <= 55)
	   {
       ProjectPoint(CenterX,CenterY,NormDegree(Plane->RelAngle),NumPixels,&NewX,&NewZ);
	     EnemyHeading = ((PlaneParams *)Plane->Obj)->Heading/DEGREE;
	     DeltaAzim = NormDegree(EnemyHeading + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));
	     if(DeltaAzim < 0) DeltaAzim = NormDegree(DeltaAzim);
	     ObjIndex = (DeltaAzim > 358.0) ? 0 : (int)( round(((360.0 - DeltaAzim) / 360.0)*15.0) );
       ObjIndex = TEWS_AIRCRAFT0 + ObjIndex;
	     DrawRadarSymbol(NewX,NewZ,ObjIndex,63,GrBuffFor3D);
			 if( (UFC.MasterMode == AA_MODE) && (Plane->Obj == Av.AA.PrimeTarget.Obj) )
			 {
			   PlaneParams *P = (PlaneParams *)Plane->Obj;
				 TruncatePlaneName(TmpStr,pDBAircraftList[P->AI.iPlaneIndex].sName);
    	   DrawBox(GrBuffFor3D,NewX - RDRSym[ObjIndex].AnchX,NewZ - RDRSym[ObjIndex].AnchY,14,14,63);
		     DrawTextAtLocation(NewX - RDRSym[ObjIndex].AnchX,NewZ - RDRSym[ObjIndex].AnchY + 16,TmpStr,63);
 			 }
	     AddObjectToSensorList(Plane,AV_PLANE,-1,NewX - CenterX,NewZ - CenterY);
	   }
	   Plane++;
   }

   Missile = &Av.Missiles.List[0];
   Index  = Av.Missiles.NumMissiles;
   while(Index-- > 0)
   {
     Dist = Missile->Dist*WUTONM;
     NumPixels = (int)(Dist)*PixelsPerMile;
   	 if(NumPixels <= 55)
	   {
       ProjectPoint(CenterX,CenterY,NormDegree(Missile->RelAngle),NumPixels,&NewX,&NewZ);
	     EnemyHeading = ((WeaponParams *)Missile->Obj)->Heading/DEGREE;
       DeltaAzim = NormDegree(EnemyHeading + NormDegree(360.0 - (PlayerPlane->Heading/DEGREE)));
	     ObjIndex = (DeltaAzim > 358.0) ? 0 : (int)( round(((360.0 - DeltaAzim) / 360.0)*11.0) );
	     ObjIndex = TEWS_MISSLE0  + ObjIndex;
			 WeaponParams *W = (WeaponParams *)Missile->Obj;
			 int GoingForPlayer = ( (W->iTargetType == TARGET_PLANE) && (W->pTarget == PlayerPlane) );
	     DrawRadarSymbol(NewX,NewZ,ObjIndex,(GoingForPlayer) ? 5 : 63,GrBuffFor3D);
	   }
	   Missile++;
   }

   Sam = &Av.Sams.List[0];
   Index  = Av.Sams.NumSams;
   while(Index-- > 0)
   {
     Dist = Sam->Dist*WUTONM;
     NumPixels = (int)(Dist)*PixelsPerMile;
   	 if(NumPixels <= 55)
	   {
       ProjectPoint(CenterX,CenterY,NormDegree(Sam->RelAngle),NumPixels,&NewX,&NewZ);
	     DrawRadarSymbol(NewX,NewZ,TEWS_SAM,63,GrBuffFor3D);
			 if( (UFC.MasterMode == AG_MODE) && (Av.AG.PrimeTarget) && (Av.AG.PrimeTarget->Type == AV_REGISTERED_OBJECT) )
			 {
			  	if(Sam->Obj == Av.AG.PrimeTarget->Obj)
            	DrawBox(GrBuffFor3D,NewX - RDRSym[TEWS_SAM].AnchX,NewZ - RDRSym[TEWS_SAM].AnchY,14,14,63);
			 }
	     AddObjectToSensorList(Sam,AV_SAM,-1,NewX - CenterX,NewZ - CenterY);
	   }
     Sam++;
   }

   RadarSite = &Av.RadarSites.List[0];
   Index  = Av.RadarSites.NumRadarSites;
   while(Index-- > 0)
   {
     Dist = RadarSite->Dist*WUTONM;
     NumPixels = (int)(Dist)*PixelsPerMile;
   	 if(NumPixels <= 55)
	   {
       ProjectPoint(CenterX,CenterY,NormDegree(RadarSite->RelAngle),NumPixels,&NewX,&NewZ);
	     DrawRadarSymbol(NewX,NewZ,TEWS_SAM,63,GrBuffFor3D);
			 if( (UFC.MasterMode == AG_MODE) && (Av.AG.PrimeTarget) && (Av.AG.PrimeTarget->Type == AV_REGISTERED_OBJECT) )
			 {
			  	if(RadarSite->Obj == Av.AG.PrimeTarget->Obj)
            	DrawBox(GrBuffFor3D,NewX - RDRSym[TEWS_SAM].AnchX,NewZ - RDRSym[TEWS_SAM].AnchY,14,14,63);

					AddObjectToSensorList(RadarSite,AV_RADAR_SITE,-1,NewX - CenterX,NewZ - CenterY);
			 }
	   }
     RadarSite++;
   }

   Vehicle = &Av.Vehicles.List[0];
   Index   = Av.Vehicles.NumVehicles;
   while(Index-- > 0)
   {
    Dist = Vehicle->Dist*WUTONM;
    NumPixels = (int)(Dist)*PixelsPerMile;
   	if(NumPixels <= 55)
	  {
      	ProjectPoint(CenterX,CenterY,NormDegree(Vehicle->RelAngle),NumPixels,&NewX,&NewZ);


		int IsShip=0;

		if (((MovingVehicleParams *)Vehicle->Obj)->iShipType)
			IsShip=1;
		else
			IsShip=0;

	    	DrawRadarSymbol(NewX,NewZ,STAR_OF_DAVID,(IsShip) ? 12 : 63,GrBuffFor3D);
				if( (UFC.MasterMode == AG_MODE) && (Av.AG.PrimeTarget) && (Av.AG.PrimeTarget->Type == AV_VEHICLE) )
				{
			  	if(Vehicle->Obj == Av.AG.PrimeTarget->Obj)
            	DrawBox(GrBuffFor3D,NewX - RDRSym[STAR_OF_DAVID].AnchX,NewZ - RDRSym[STAR_OF_DAVID].AnchY,14,14,63);
				}
 			  AddObjectToSensorList(Vehicle,AV_VEHICLE,-1,NewX - CenterX,NewZ - CenterY);
		 }
     Vehicle++;
   }

   Runway  = &Av.Runways.List[0];
   Index   = Av.Runways.NumRunways;
   while(Index-- > 0)
   {
     Dist = Runway->Dist*WUTONM;
     NumPixels = (int)(Dist)*PixelsPerMile;
   	 if(NumPixels <= 55)
	   {
       ProjectPoint(CenterX,CenterY,NormDegree(Runway->RelAngle),NumPixels,&NewX,&NewZ);
	     DrawRadarSymbol(NewX,NewZ,BULLSEYE_SYMBOL,63,GrBuffFor3D);
	   }
     Runway++;
   }

	 FPointDouble TargPos;
	 FPoint       Vec;

   MissionTarget  = &Av.MissionAGTargets.List[0];
   Index          = Av.MissionAGTargets.NumAGTargets;
   while(Index-- > 0)
   {
		 int InSamList  = FALSE;
		 int InProvList = FALSE;

		 // dont draw if already an object with radar
		 if(MissionTarget->Type == AV_REGISTERED_OBJECT)
		 {
		 	 InSamList  = IsObjInSamList((BasicInstance *)MissionTarget->Obj);
			 InProvList = IsObjInRadarSiteList((BasicInstance *)MissionTarget->Obj);
		 }

		 if( (!InSamList) && (!InProvList) && (MissionTarget->Type != AV_VEHICLE) )
		 {
				if(MissionTarget->Type == AV_REGISTERED_OBJECT)
					TargPos = ((BasicInstance *)MissionTarget->Obj)->Position;
				else
				{
				 // bug
	        ASSERT(TRUE);
				}

    		Dist = MissionTarget->Dist*WUTONM;
    		NumPixels = (int)(Dist)*PixelsPerMile;
   			if(NumPixels <= 55)
	  		{
      		ProjectPoint(CenterX,CenterY,NormDegree(MissionTarget->RelAngle),NumPixels,&NewX,&NewZ);
	    		DrawRadarSymbol(NewX,NewZ,(MissionTarget->Type == AV_REGISTERED_OBJECT) ? BULLSEYE_SYMBOL : STAR_OF_DAVID,5,GrBuffFor3D);
					if( (UFC.MasterMode == AG_MODE) && (Av.AG.PrimeTarget) && (Av.AG.PrimeTarget == MissionTarget) )
					{
        		DrawBox(GrBuffFor3D,NewX - RDRSym[BULLSEYE_SYMBOL].AnchX,NewZ - RDRSym[BULLSEYE_SYMBOL].AnchY,14,14,63);
					}
	        AddObjectToSensorList(MissionTarget,AV_REGISTERED_OBJECT,-1,NewX - CenterX,NewZ - CenterY);
	  		}
		 }
     MissionTarget++;
   }
}

//*****************************************************************************************************************************************
// OBJECT CAMERA  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void DrawObjectCamera(int CenterX, int CenterY)
{
	FPointDouble RelativePosition;
	float Radius;
	double ProjDist,ProjDistW,ProjDistH,AdjustHeight;
	int TempX,TempY;
	FMatrix TempMatrix,Mat,TiltMat;
	ObjectHandler *Model;

	// return on all bad conditions
	if( (UFC.MasterMode != AA_MODE) && (UFC.MasterMode != AG_MODE) ) return;

	Set3DLighting(0.0,-1.0,0.0,0.8,0.5);

  TempX = GrBuffFor3D->MidX;
	TempY = GrBuffFor3D->MidY;
	GrBuffFor3D->MidX = CenterX;
	GrBuffFor3D->MidY = CenterY;

	TempMatrix = ViewMatrix;
	ViewMatrix.SetRPH(0,0,0);

	if(UFC.MasterMode == AA_MODE)
	{
			if(Av.AA.PrimeTarget.Obj)
			{
					PlaneParams *P = (PlaneParams *)Av.AA.PrimeTarget.Obj;
			  	TruncatePlaneName(TmpStr,pDBAircraftList[P->AI.iPlaneIndex].sName);
		    	DrawTextAtLocation(CenterX - 50,CenterY - 48,TmpStr,63);

					int Dir;
					float HeadingAdd;
					float TargHeading;

					TargHeading = Av.AA.PrimeTarget.RelAngle;
					float TargPitch   = -ComputePitchToPoint(PlayerPlane,((PlaneParams *)Av.AA.PrimeTarget.Obj)->WorldPosition );

					if( (TargHeading >= 45) && (TargHeading <= 90.0) )
					{
						Dir = LEFT;
						HeadingAdd = 90.0;
					}
					else if( (TargHeading <= -45) && (TargHeading >= -90.0) )
					{
						Dir = RIGHT;
						HeadingAdd = 270.0;
					}
					else if( (TargHeading > 90) || (TargHeading < -90.0) )
					{
						Dir = BOTTOM;
					  HeadingAdd = 180.0;
					}
					else
					{
						Dir = TOP;
						HeadingAdd = 0;
					}

					HeadingAdd = NormDegree(TargHeading);

			  	FMatrix TargAtt;
					ANGLE Roll,Pitch,Heading;
	      	TargHeading = NormDegree( (P->Heading/DEGREE) + NormDegree(360.0 - NormDegree( (PlayerPlane->Heading/DEGREE)+HeadingAdd)  ));
					P->Attitude.GetRPH(&Roll,&Pitch,&Heading);

					TargAtt.SetRPH(Roll,Pitch,TargHeading*DEGREE);
	      	Radius = GetObjectRadius(P->Type->Model);
					ProjDist = (OBJ_PROJ_DIST*(Radius/512.0) ) / (12.0/512.0) ;

					AdjustHeight = 0;
				  if(TargPitch >= 0.0)
				    AdjustHeight = -90.0*(TargPitch/90.0);
				  else
					AdjustHeight = 90.0*(-TargPitch/90.0);

					AdjustHeight = NormDegree(AdjustHeight);

					TiltMat.SetRPH(0,AdjustHeight*DEGREE,0);
					TargAtt *= TiltMat;

					RelativePosition.SetValues(0.0,0,-ProjDist);
    			Draw3DObjectMatrix(P->Type->Model,RelativePosition,TargAtt,NULL);

					TargHeading = Av.AA.PrimeTarget.RelAngle;

					switch(Dir)
					{
						case LEFT:    sprintf(TmpStr,"CAMERA LEFT");  break;
						case RIGHT:   sprintf(TmpStr,"CAMERA RIGHT"); break;
						case TOP:     sprintf(TmpStr,"CAMERA FRONT"); break;
						case BOTTOM:  sprintf(TmpStr,"CAMERA BACK");  break;
					}

					DrawTextAtLocation(CenterX - 25,CenterY - 60,TmpStr,63);

					TargHeading = NormDegree( (PlayerPlane->Heading/DEGREE) + Av.AA.PrimeTarget.RelAngle );
					float Dist = Av.AA.PrimeTarget.Dist*WUTONM;
					sprintf(TmpStr,"BEARING  : %d",(int)TargHeading);
					DrawTextAtLocation(CenterX + 10,CenterY - 48,TmpStr,63);
					sprintf(TmpStr, "DISTANCE : %d.%d", (int)Dist,(int)((Dist-floor(Dist))*10.0));
					DrawTextAtLocation(CenterX + 10,CenterY - 42,TmpStr,63);
			}
	}
	else if(UFC.MasterMode == AG_MODE)
	{
			if(Av.AG.PrimeTarget)
			{
					FPointDouble Loc;

					if(Av.AG.PrimeTarget->Type == AV_REGISTERED_OBJECT)
					{
            RegisteredObject *R = GetRegisteredObjectForInstance((BasicInstance *)Av.AG.PrimeTarget->Obj);
						if(R)
						{
								//RegisteredObject *R = (RegisteredObject *)Av.AG.PrimeTarget->Obj;
								ProjDistH = 7.314*(R->ExtentHeight);   // 7.314 = (512/70) = eyedist/70pixels    70/512 = height/projdist   ratio
								ProjDistW = 4.26*(fabs(R->ExtentLeft) + fabs(R->ExtentRight));

								if(ProjDistH > ProjDistW)
								{
									ProjDist = ProjDistH;
						  		GrBuffFor3D->MidY = CenterY + 35;
								}
								else
								{
									ProjDist = ProjDistW;
									GrBuffFor3D->MidY = CenterY + 10;
								}

								Model = ((RegisteredObject *)Av.AG.PrimeTarget->Obj)->Model;
								DrawTextAtLocation(CenterX - 51,CenterY - 42,((RegisteredObject *)Av.AG.PrimeTarget->Obj)->Model->Name,5);
								Loc = ((RegisteredObject *)Av.AG.PrimeTarget->Obj)->Instance->Position;
						}
					}
					else if(Av.AG.PrimeTarget->Type == AV_VEHICLE)
					{
						Model = ((MovingVehicleParams *)Av.AG.PrimeTarget->Obj)->Type->Model;
						ProjDistH = 0; // don't worry about the height of vehicles

						MovingVehicleParams *Veh = (MovingVehicleParams *)Av.AG.PrimeTarget->Obj;

						char *Str=NULL;

						if (!Veh->iShipType)
							Str = pDBVehicleList[Veh->iVDBIndex].sName;
						else
							Str = pDBShipList[Veh->iVDBIndex].sName;

						DrawTextAtLocation(CenterX - 51,CenterY - 42,Str,5);
						Loc = Veh->WorldPosition;

						Radius = GetObjectRadius(Model);
						ProjDist = 4.26*(Radius*2.0);
						GrBuffFor3D->MidY = CenterY + 10;
					}

					RelativePosition.SetValues(0.0,0.0,-ProjDist);
					Mat.SetRPH(0,0,90*DEGREE);
    			Draw3DObjectMatrix(Model,RelativePosition,Mat,NULL);

					float Heading = Av.AG.PrimeTarget->RelAngle;
					float Pitch   = -ComputePitchToPoint(PlayerPlane,Loc);

					if( (Heading >= 45) && (Heading <= 90.0) )
						sprintf(TmpStr,"TARGET LEFT");
					else if( (Heading <= -45) && (Heading >= -90.0) )
						sprintf(TmpStr,"TARGET RIGHT");
					else if( (Heading > 90) || (Heading < -90.0) )
						sprintf(TmpStr,"TARGET BACK");
					else
						sprintf(TmpStr,"TARGET FRONT");

					DrawTextAtLocation(CenterX - 25,CenterY - 50,TmpStr,63);
			}
	}

	FMatrix WeapAtt;
	int Total;

	WeapAtt.SetRPH(0,0,90*DEGREE);

	GrBuffFor3D->MidY = CenterY+40;

	AvWeapEntryType *CurWeap;

	if(UFC.MasterMode == AA_MODE)
	{
	  CurWeap = (Av.Weapons.UseAASeq) ? Av.Weapons.CurAASeqWeap : Av.Weapons.CurAAWeap;
	}
	else
	{
	  CurWeap = Av.Weapons.CurAGWeap;
	}

	if(CurWeap != NULL)
	{

			Total = WeapQuantity(CurWeap->W->WeapId);
			sprintf(TmpStr,"%s : %d ",pDBWeaponList[CurWeap->W->WeapIndex].sName,Total);
			DrawTextAtLocation(CenterX - 65, CenterY + 51,TmpStr,63);

			int Type = pDBWeaponList[CurWeap->W->WeapIndex].iWeaponType;

			switch (Type)
			{
				case SHORT_RANGE:
				case MED_RANGE:
														sprintf(TmpStr,"AIR-TO-AIR MISSILE");
			                  		break;
 				case DUMB_AG:
														sprintf(TmpStr,"DUMB BOMB");
			                  		break;

    		case DATA_LINK:
														sprintf(TmpStr,"DATA LINK");
			                  		break;

				case GUIDED_AG:
														sprintf(TmpStr,"GUIDED AIR TO GROUND BOMB");
			                  		break;

				case AG_MISSILE:
														sprintf(TmpStr,"AIR-TO-GROUND MISSILE");
			                  		break;

				case CLUSTER:
														sprintf(TmpStr,"CLUSTER BOMB");
			                  		break;
			}
			DrawTextAtLocation(CenterX - 65, CenterY + 57,TmpStr,63);
	}

	ViewMatrix = TempMatrix;
	GrBuffFor3D->MidX = TempX;
	GrBuffFor3D->MidY = TempY;
}

//*****************************************************************************************************************************************
// TARGET SELECT FUNCS  -- Header -- (search keyword to step through each section of file)
//*****************************************************************************************************************************************

void SelectClosestAATarget()
{
   AvObjListEntryType *P,*Closest;

	 if(Av.AA.UseAARadarForTargets)
	 {
		 Av.AA.UseAARadarForTargets = FALSE;
		 // may need some radar reset calls here later......
	 }

	 float MinDist;
	 Closest = NULL;

	 MinDist = 100000000;

	 P = &Av.Planes.List[0];
   int Index = Av.Planes.NumPlanes;
   while(Index-- > 0)
	 {
		  if(P->Dist < MinDist)
			{
			  MinDist = P->Dist;
				Closest = P;
			}
			P++;
	 }

	 Av.AA.PrimeTarget = *Closest;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectNextAATarget()
{
   AvObjListEntryType *P,*Closest;

	 if(Av.AA.UseAARadarForTargets)
	 {
		 Av.AA.UseAARadarForTargets = FALSE;
		 // may need some radar reset calls here later......
	 }

	 float CurTargetDist,MinDist;

	 CurTargetDist = (Av.AA.PrimeTarget.Obj == NULL) ? 0 : Av.AA.PrimeTarget.Dist;
	 Closest = NULL;
	 MinDist = 100000000;

	 P = &Av.Planes.List[0];
   int Index = Av.Planes.NumPlanes;
   while(Index-- > 0)
	 {
		 if(P->Obj != Av.AA.PrimeTarget.Obj)
		 {
			 if(P->Dist > CurTargetDist)
			 {
				 float Delta = P->Dist - CurTargetDist;
				 if(Delta < MinDist)
				 {
					 MinDist = Delta;
					 Closest = P;
				 }
			 }
 		 }
		 P++;
	 }

	 if(Closest)
	   Av.AA.PrimeTarget = *Closest;
	 else
		 SelectClosestAATarget();
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AutoGrabAATarget()
{
	 if(UFC.MasterMode == AA_MODE)
	 {
	   if( (Av.Planes.NumPlanes > 0) && (Av.AA.PrimeTarget.Obj == NULL) && (!Av.AA.UseAARadarForTargets) )
	 	   	SelectClosestAATarget();
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectNextAGTarget()
{
	if(Av.MissionAGTargets.NumAGTargets <= 0) return;

	FPointDouble Loc;

	if(Av.AG.PrimeTarget == NULL)
	{
		Av.AG.PrimeTarget = &Av.MissionAGTargets.List[0];
	}
	else
	{
		Av.AG.PrimeTarget++;
		AvObjListEntryType *LastAGTarget = &Av.MissionAGTargets.List[Av.MissionAGTargets.NumAGTargets-1];

		if(Av.AG.PrimeTarget > LastAGTarget)
		{
			Av.AG.PrimeTarget =  &Av.MissionAGTargets.List[0];
		}
	}

	BreakAGLock();
	GetAGTargetLocation(&Loc);
	PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_EASY_RADAR;
	PlayerPlane->AGDesignate = Loc;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectPrevAGTarget()
{
	if(Av.MissionAGTargets.NumAGTargets <= 0) return;

	FPointDouble Loc;

	if(Av.AG.PrimeTarget == NULL)
	{
		Av.AG.PrimeTarget = &Av.MissionAGTargets.List[0];
	}
	else
	{
		Av.AG.PrimeTarget--;
		AvObjListEntryType *LastAGTarget = &Av.MissionAGTargets.List[Av.MissionAGTargets.NumAGTargets-1];
		AvObjListEntryType *FirstAGTarget = &Av.MissionAGTargets.List[0];

		if(Av.AG.PrimeTarget < FirstAGTarget)
		{
			Av.AG.PrimeTarget =  LastAGTarget;
		}
	}

	BreakAGLock();
	GetAGTargetLocation(&Loc);
	PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_EASY_RADAR;
	PlayerPlane->AGDesignate = Loc;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectFirstAGTarget()
{
	if(Av.MissionAGTargets.NumAGTargets <= 0) return;

	Av.AG.PrimeTarget = &Av.MissionAGTargets.List[0];

	FPointDouble Loc;

	BreakAGLock();
	GetAGTargetLocation(&Loc);
	PlayerPlane->AGDesignator = AG_DESIGNATE_FROM_EASY_RADAR;
	PlayerPlane->AGDesignate = Loc;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetAGTargetLocation(FPointDouble *Location)
{
	if(Av.AG.PrimeTarget == NULL) return(FALSE);

	switch(Av.AG.PrimeTarget->Type)
	{
	 case GROUNDOBJECT:  *Location = ((BasicInstance *)Av.AG.PrimeTarget->Obj)->Position;                      break;
	 case MOVINGVEHICLE: *Location = ((MovingVehicleParams *)Av.AG.PrimeTarget->Obj)->WorldPosition;           break;
	}
	return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

int GetObjectLocation(AvObjListEntryType *Obj,FPointDouble *Location)
{
	if(Obj == NULL) return(FALSE);

	switch(Obj->Type)
	{
	 case AV_REGISTERED_OBJECT:  *Location = ((RegisteredObject *)Obj)->Instance->Position;   break;
	 case AV_PLANE:              *Location = ((PlaneParams *)Obj)->WorldPosition;             break;
	 case AV_SAM:                *Location = ((AAWeaponInstance *)Obj)->Basics.Position;      break;
	 case AV_RADAR_SITE:         *Location = ((InfoProviderInstance *)Obj)->Basics.Position;  break;
	 case AV_VEHICLE:            *Location = ((MovingVehicleParams *)Obj)->WorldPosition;     break;
	 case AV_MISSILE:            *Location = ((WeaponParams *)Obj)->Pos;                      break;
	 case AV_RUNWAY :            *Location = ((RunwayInfo *)Obj)->ILSLandingPoint;            break;

	  default:
		 return(FALSE);
	}

	return(TRUE);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void AutoGrabAGTarget()
{
	 if(UFC.MasterMode == AG_MODE)
	 {
	   if( (Av.AG.PrimeTarget == NULL) )
	 	   	SelectNextAGTarget();
	 }
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void SelectNextTarget()
{
  if(UFC.MasterMode == AA_MODE)
	{
		SelectNextAATarget();
	}
	else if(UFC.MasterMode == AG_MODE)
	{
		SelectNextAGTarget();
	}
}

//*****************************************************************************************************************************************
// MISC FUNCS  -- Header -- (search keyword to step through each section of file)
// Any generic functions needed by avionics code goes here at the bottom of avsensor.cpp
//*****************************************************************************************************************************************

void TruncatePlaneName(char *StrTo, char *StrFrom)
{
  char *Ptr  = &StrFrom[0];
  char *Ptr2 = &StrTo[0];
  int  Index = 0;

  while(*Ptr != 0x00)
  {
    if(*Ptr == '(')
	  {
      *Ptr2 = 0x00;
	    break;
		}
    *(Ptr2++) = *(Ptr++);

	  Index++;
	  if(Index > 60) break;
  }

  *Ptr2 = 0x00;

}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float Dist2D(FPointDouble *P1, FPointDouble *P2)
{
  return(sqrt( (P1->X - P2->X)*(P1->X - P2->X) + (P1->Z - P2->Z)*(P1->Z - P2->Z) ) );
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

double ComputeHeadingToPoint(FPoint WorldPosition, double Heading, FPoint pointpos, int isrelative)
{
	double dx, dz;
	double offangle;

	dx = pointpos.X - WorldPosition.X;
	dz = pointpos.Z - WorldPosition.Z;

	offangle = atan2(-dx, -dz) * RADIANS_TO_DEGREES;

	if(isrelative)
		offangle -= fANGLE_TO_DEGREES(Heading);

	while(offangle > 180)
		offangle -= 360;

	while(offangle < -180)
		offangle += 360;

	return((float) offangle);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

double ComputeHeadingToPointDouble(FPointDouble WorldPosition, double Heading, FPointDouble pointpos, int isrelative)
{
	double dx, dz;
	double offangle;

	dx = pointpos.X - WorldPosition.X;
	dz = pointpos.Z - WorldPosition.Z;

	offangle = atan2(-dx, -dz) * RADIANS_TO_DEGREES;

	if(isrelative)
		offangle -= fANGLE_TO_DEGREES(Heading);

	while(offangle > 180)
		offangle -= 360;

	while(offangle < -180)
		offangle += 360;

	return((float) offangle);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float ComputePitchToPoint(PlaneParams *planepnt, FPoint pointpos)
{
	double dx, dy, dz;
	double offangle;
	double Distance;

	dx = pointpos.X - planepnt->WorldPosition.X;
	dy = pointpos.Y - planepnt->WorldPosition.Y;
	dz = pointpos.Z - planepnt->WorldPosition.Z;

	Distance = QuickDistance(dx,dz);

	offangle = atan2(-dy, -Distance) * RADIANS_TO_DEGREES;

	//offangle = atan2(-dy, -dz) * 57.2958;

	while(offangle > 180)
		offangle -= 360;

	while(offangle < -180)
		offangle += 360;

	if(offangle > 90.0)
	  offangle = 180 - offangle;

	if(offangle < -90.0)
	  offangle = -(180 + offangle);

  	return((float) offangle);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

float ComputePitchFromPointToPoint(FPointDouble FromPos, FPoint pointpos)
{
	double dx, dy, dz;
	double offangle;
	double Distance;

	dx = pointpos.X - FromPos.X;
	dy = pointpos.Y - FromPos.Y;
	dz = pointpos.Z - FromPos.Z;

	Distance = sqrt(dx*dx + dz*dz);

	offangle = atan2(-dy, -Distance) * RADIANS_TO_DEGREES;

	while(offangle > 180)
		offangle -= 360;

	while(offangle < -180)
		offangle += 360;

	if(offangle > 90.0)
	  offangle = 180 - offangle;

	if(offangle < -90.0)
	  offangle = -(180 + offangle);

  	return((double) offangle);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

double ComputePitchFromPointToPointDouble(FPointDouble FromPos, FPointDouble pointpos)
{
	double dx, dy, dz;
	double offangle;
	double Distance;

	dx = pointpos.X - FromPos.X;
	dy = pointpos.Y - FromPos.Y;
	dz = pointpos.Z - FromPos.Z;

	Distance = sqrt(dx*dx + dz*dz);

	offangle = atan2(-dy, -Distance) * RADIANS_TO_DEGREES;

	while(offangle > 180)
		offangle -= 360;

	while(offangle < -180)
		offangle += 360;

	if(offangle > 90.0)
	  offangle = 180 - offangle;

	if(offangle < -90.0)
	  offangle = -(180 + offangle);

  	return((double) offangle);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void GetRelativeHPFromLoc(PlaneParams *planepnt,FPointDouble Loc, FPointDouble worldposition, float *heading, float *pitch)
{
	FPointDouble relative_position;
	FMatrix de_rotate;

	relative_position.MakeVectorToFrom(worldposition,Loc);

	de_rotate = planepnt->Attitude;
	de_rotate.Transpose();

	relative_position *= de_rotate;
	relative_position.Normalize();

	*heading = atan2(-relative_position.X,-relative_position.Z) * RADIANS_TO_DEGREES;
	*pitch = asin(relative_position.Y) * RADIANS_TO_DEGREES;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void ProjectPointDouble(double X, double Z, float Degrees, float Magnitude, double *NewX, double *NewZ)
{
   double XOff,ZOff;

   Degrees *= DEGREES_TO_RADIANS;

   //Degrees = NormDegree(Degrees + 90.0);

   XOff =  -sin(Degrees)*Magnitude;
   ZOff =  -cos(Degrees)*Magnitude;

   *NewX = X + XOff;
   *NewZ = Z + ZOff;
}

void ProjectPoint(float X, float Z, float Degrees, float Magnitude, float *NewX, float *NewZ)
{
   float XOff,ZOff;

   Degrees = NormDegree(Degrees + 90.0);

   XOff =  FCosTimes(Degrees*DEGREE, Magnitude);
   ZOff = -FSinTimes(Degrees*DEGREE, Magnitude);

   *NewX = X + XOff;
   *NewZ = Z + ZOff;
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void IntTo3DigitFont(int Val, char *Str)
{
	int TempVal;

	TempVal = abs(Val);

	if( TempVal >= 100000)
		*Str = 0;
	else
		sprintf(Str,"%03d",TempVal);
//	int TempVal;

//	TempVal = abs(Val);

//	else if(TempVal < 10)                           sprintf(Str,"00%d", TempVal);
//	else if((TempVal >= 10) && (TempVal < 100))	    sprintf(Str,"0%d", TempVal);
//	else if((TempVal >= 100) && (TempVal < 1000))   sprintf(Str,"%d", TempVal);
}

//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

void IntTo5DigitFont(int Val, char *Str)
{
	int TempVal;

	TempVal = abs(Val);

	if( TempVal >= 100000)
		*Str = 0;
	else
		sprintf(Str,"%05d",TempVal);


//	else if(TempVal < 10)                             sprintf(Str,"0000%d", TempVal);
//	else if((TempVal >= 10) && (TempVal < 100))	      sprintf(Str,"000%d", TempVal);
//	else if((TempVal >= 100) && (TempVal < 1000))     sprintf(Str,"00%d", TempVal);
//	else if((TempVal >= 1000) && (TempVal < 10000))   sprintf(Str,"0%d", TempVal);
//	else if((TempVal >= 10000) && (TempVal < 100000)) sprintf(Str,"%d", TempVal);

}