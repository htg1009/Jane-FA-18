#ifndef __AUTOPLT_H__
#define __AUTOPLT_H__

void DoBankAngleHoldAutoPilot(PlaneParams *P, float HoldBankAngle);
int TerrainObstacle(PlaneParams *P);
void DoTerrainFollowing(PlaneParams *P, float Alt);
void DoRdrAltitudeHoldAutoPilot(PlaneParams *P, float NewRdrAlt, float OldRdrAlt/*, float HoldAltitudeDelta*/);
int AltitudeHoldStabilized(PlaneParams *P);
void DoAltitudeHoldAutoPilot(PlaneParams *P);
void DoAttitudeHoldAutoPilot(PlaneParams *P, float TargetAttitude);
void DoTcnSteeringAutoPilot(PlaneParams *P);
void DoNavSteeringAutoPilot(PlaneParams *P/*, float HoldAltitude*/);
void DoRdrNavSteeringAutoPilot(PlaneParams *P, float NewRdrAlt, float OldRdrAlt, float HoldAltitudeDelta);
void CheckAPDeselect(PlaneParams *P);
void Trim(PlaneParams *P);


extern FPointDouble HudSteerPoint;

#endif

