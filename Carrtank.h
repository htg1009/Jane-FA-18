//*****************************************************************************************
//  CarrTank.H
//
//	This holds functions dealing with Tanking and with Plane/Carrier interactions.
//		Scott E  3/26/99
//*****************************************************************************************

#define DECK_ANGLE		9.3f
#define HELO_ANGLE		79.3f
#define MAX_GATES		10
#define LAND_POINT_X	(-10.445905 * FOOT_MULTER)
#define LAND_POINT_Y	(64 * FOOT_MULTER)
#define LAND_POINT_Z	(300.0271 * FOOT_MULTER)

#define SOV_DECK_ANGLE		7.565f
#define SOV_HELO_ANGLE		79.3f
#define SOV_LAND_POINT_X	(-41.976 * FOOT_MULTER)
#define SOV_LAND_POINT_Y	(60 * FOOT_MULTER)
#define SOV_LAND_POINT_Z	(279.714 * FOOT_MULTER)

#define OFFSET_UNKNOWN						10

#define OFFSET_PROBE						11
#define OFFSET_WING_UP_BOX					12
#define OFFSET_HOOK							13
#define OFFSET_FRONT_WHEEL					14

#define MAX_TRAP_WEIGHT		44000.0f

void AICheckTankerCondition(PlaneParams *planepnt);
void AIFlyToTankerPoint(PlaneParams *planepnt);
void AIFlyTanker(PlaneParams *planepnt);
void AITankingBreakaway(PlaneParams *planepnt);
void AITankingFlight(PlaneParams *planepnt);
void AITankingFlight2(PlaneParams *planepnt);
void AISetUpTankerFlyBy(PlaneParams *planepnt);
void AITankingFlight3(PlaneParams *planepnt);
void AISetUpTankerReturnView(PlaneParams *planepnt);
void AITankingFlight4(PlaneParams *planepnt);
void AITankerDisconnect(PlaneParams *planepnt);
void AITankingFlight5(PlaneParams *planepnt);
void AIResetTanker(PlaneParams *planepnt);
void AICheckMilesOffBoom(PlaneParams *planepnt);
void AICheckOffBoom(PlaneParams *planepnt, PlaneParams *tanker);
void AISetTankerLights(PlaneParams *planepnt, PlaneParams *tanker);
int AICheckClosure(PlaneParams *planepnt, PlaneParams *tanker, int maxspeeddiff, long delayinc);
void AICheckForOnRequest(PlaneParams *planepnt);
void AIUpdateTankerBoomLoc(PlaneParams *planepnt);
void AIUpdateTankerFormationLoc(PlaneParams *planepnt, FPoint leadposition, double angzx, double angzz);
void AIFlyTankerFormation(PlaneParams *planepnt);
void AISetTankerFormationPos(PlaneParams *planepnt, PlaneParams *tanker);
void AIGetOffsetFromTanker(PlaneParams *planepnt, PlaneParams *tanker);
void AITankerTakeoff(PlaneParams *planepnt);
void AIChangeFXArray(PlaneParams *planepnt, int arraynum, int desiredval, long ticksrange);

//  NEW STUFF, MUST TAKE INTO ACCOUNT MULTI BEHAVIOR UPDATE
int AICheckProbeDroguePos(PlaneParams *planepnt, PlaneParams *tanker);
void AISetUpToTank(PlaneParams *planepnt);
void AIFlyToTankerMeeting(PlaneParams *planepnt);
void AITankingFlight2AI(PlaneParams *planepnt);
void AITankingFlight3AI(PlaneParams *planepnt);
void AITankerDisconnectAI(PlaneParams *planepnt);
void RefuelFuelPlane(PlaneParams *planepnt, int fuelrequest = 0);
void CTGetNextToTank(PlaneParams *planepnt, PlaneParams *tanker);
float CTGetTankingAmount(PlaneParams *planepnt, int refuelrequest = 0);
int CTAddFuel(PlaneParams *tanker, PlaneParams *planepnt, float fuelamount);
void AITankingFlight2HumanLong(PlaneParams *planepnt);
void AITankingFlight3HumanLong(PlaneParams *planepnt);


int AICheckPlaneLandingFlyToCarrier(PlaneParams *planepnt, TakeOffActionType *pActionTakeOff);
void AISetPlaneLandingFlyToCarrier(PlaneParams *planepnt);
void AIPlaneLandingFlyToCarrier(PlaneParams *planepnt);
void AIContactCarrierMarshallPattern(PlaneParams *planepnt);
void AIMarshallRestOfGroup(PlaneParams *planepnt, float *maxheight, long *pushtime, long *delaycnt);
void AIFlyCarrierMarshallPattern(PlaneParams *planepnt);
void AISetOffCarrierRelPoint(PlaneParams *planepnt, float xoff, float yoff, float zoff, int setvar, double addangle = 0);
void AIFlyToDME10(PlaneParams *planepnt);
void AIFlyToDME6(PlaneParams *planepnt);
void AIFlyToDME3(PlaneParams *planepnt);
void AIFlyToDME3Q4(PlaneParams *planepnt);
void AIFlyCarrierFinal(PlaneParams *planepnt);
void AIFlyBolter(PlaneParams *planepnt);
ANGLE AIGetPitchANGLEForFPMDescent(PlaneParams *planepnt, double fpm);
double AIDegreesOffGlideSlope(PlaneParams *planepnt, double *rdist, double *pitch = NULL, FPoint *deckpoint = NULL);
void AIOffsetFromPoint(FPoint *location, double offangle, float xoff, float yoff, float zoff);
void AIOffsetFromPoint(FPointDouble *location, double offangle, float xoff, float yoff, float zoff);
void ShowLandingGates(PlaneParams *planepnt, int showgates = 4);
void ClearLandingGates();
void AICarrierTrap(PlaneParams *planepnt);
void AIMovePlaneToDeArmZone(PlaneParams *planepnt);
void AIMovePlaneToDeArmZone(PlaneParams *planepnt);
void AIGetCarrierParkingSpace(PlaneParams *planepnt);
void AIGetSovCarrierParkingSpace(PlaneParams *planepnt);
void AIMoveToParkingSpace(PlaneParams *planepnt);
int CTCheckForCarrierTakeoff(PlaneParams *planepnt, FPointDouble worldpos, FPointDouble *fieldpos, ANGLE *runwayheading, TakeOffActionType *ActionTakeOff, double minfar, int ignoreside, int allow_both_ends, int ignore_destroy);
void CTGetCarrierTakeOffSpace(PlaneParams *planepnt, int useslot = -1);
void CTGetSovCarrierTakeOffSpace(PlaneParams *planepnt, int useslot = -1);
void CTWaitingForLaunch(PlaneParams *planepnt);
void CTDoCarrierLaunch(PlaneParams *planepnt);
void CTCheckTakeOffQueue(PlaneParams *planepnt);
void CTReplacePlaneInSlot(PlaneParams *planepnt, int slotnum);

void AIWaitForTrapOrBolter(PlaneParams *planepnt);
void AIUnhookFromTrapWire(PlaneParams *planepnt);

void StartCasualAutoPilotPlane(PlaneParams *planepnt);
void EndCasualAutoPilotPlane(PlaneParams *planepnt);
int CTFindSlotForPlane(PlaneParams *planepnt, int searchtype = 0);
int CTFindSlotForSovPlane(PlaneParams *planepnt, int searchtype = 0);

void AISetUpQuickCarrierLanding(PlaneParams *planepnt);
void CTCheckForRelaunch(PlaneParams *planepnt);
void AIQuickCarrierLanding(PlaneParams *planepnt);
void UpdateCarrierViews();

void AIFlyCarrierHeloMarshallPattern(PlaneParams *planepnt);
void AIFlyToHeloDME3(PlaneParams *planepnt);
void AIFlyHeloCarrierFinal(PlaneParams *planepnt);
void CTAddToLaunchQueue(PlaneParams *planepnt);
void CTSetUpAddToQueue(PlaneParams *planepnt, int waiting_area);
void CTSetPlaneReadyToLaunch(PlaneParams *planepnt);
int CT_ACLS_Ready();

