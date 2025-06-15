#ifndef __FLIGHTFX_H__
#define __FLIGHTFX_H__

void StartWingConTrails(PlaneParams *P);
void StopWingConTrails(PlaneParams *P);
void CalcPilotGLoading(PlaneParams *P);
void DoYawRelatedStuff(PlaneParams *P);
void DoGRelatedStuff(PlaneParams *P);
int CheckForDeadlySinkRate(PlaneParams *P);
int CheckLandingAttitude(PlaneParams *P);
float GetGroundSurfaceDragCoefficient(PlaneParams *P);
void ToggleFlaps(void);
void ToggleSpeedBrake(void);
void ToggleGear(void);
void MoveLandingGear(PlaneParams* P);
void SetLandingGearUpDown(PlaneParams* P,int gear_down);
void RaiseLowerLandingGear(PlaneParams* P,int gear_down);
void DoSmoke(PlaneParams *P);
void CheckReportingStats(PlaneParams *P);
void CheckEasyFlightStuff(PlaneParams *P);
void UpdateSpeedBrakePosition(PlaneParams *P);
void KeyThrottleOff(void);
void KeyThrottleIdle(void);
void KeyThrottleMil(void);
void KeyThrottleAB(void);
void KeyThrottleUp(void);
void KeyThrottleDown(void);
void ForceKeyboardThrust(float Value);
void DefaultTrim(void);
void AileronTrimLeft(void);
void AileronTrimRight(void);
void RudderTrimLeft(void);
void RudderTrimRight(void);
void NoseTrimUp(void);
void NoseTrimDown(void);
float DoKeyBoardRudderMotion(PlaneParams *P);
void GetF18StickPos(PlaneParams *P);
float CalcHeightAboveGround( PlaneParams *P ,MovingVehicleParams *carrier = NULL);
void SetUpDitch(PlaneParams *P);
void UpdatePlaneLightPositions(PlaneParams *P);
void FreePlaneLights(PlaneParams *P);

void ToggleHook(void);
void ToggleRefuelingProbe(void);
void CalcCarrierBasedVisuals(PlaneParams *P);

void DeployTowedDecoy(PlaneParams *P);
void RetractTowedDecoy(PlaneParams *P);
void CutTowedDecoyCable(PlaneParams *P);
void ClearTowedDecoySettings(PlaneParams *P);

void DeployFuelHose(PlaneParams *P,int number);
void RetractFuelHose(PlaneParams *P,int number);
void ClearFuelHoseSettings(PlaneParams *P,int number);
void RemovePlaneFromRefuelingHose(PlaneParams *P,int number);
void AttachPlanePointToRefuelingHose(PlaneParams *P,DWORD plane,int number);
void ABSmoke(PlaneParams *P);
void FuelDumpSmoke(PlaneParams *P);
void CalcOxygen(PlaneParams *P);



#endif