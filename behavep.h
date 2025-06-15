//*****************************************************************************************
//  behavep.H
//
//  Holds the prototypes for behave.cpp.  This needed to be split out of the behave.h
//  file because the plane structure needed to access the behavior structure so the behave.h
//  had to be listed first, but the behave.cpp functions accessed the plane structure so
//  it had to be before the prototype defines.  I didn't want to put all the structure
//  together since I know that the behavior structure is not going to be stable for a while
//  and I didn't want to tie up the plane structure while I was doing AI work.
//		Scott E  6/16/96
//*****************************************************************************************

#define PYAW_G_MOD 0.25f
#define WYAW_G_MOD 0.25f


//   MOVE THIS TO FLIGHT.H
#define PT_KC135	16

//  #define AIDESIREDAOA 13.5f  //  From F-15
#define AIDESIREDAOA 8.2f
#define AISLOWDESIREDAOA 3.5f





//*****************************************************************************************
//  Defines for behave.cpp
//*****************************************************************************************
//  Flags for iAIFlags1
#define AIFLIGHTNUMS	 0xF
#define AIDETAILALTCHECK 0x10
#define AIFASTPITCHROLL 0x20
#define AIDESIREDSPDCALC 0x40
#define AIDESIREDSPDMACH 0x80
#define AIRADARON		 0x100
#define AIJAMMINGON		 0x200
#define AIENGAGED		 0x400
#define AIBOMBHOTMSG	 0x800
#define AICANENGAGE		 0x1000
#define AIWARNINGGIVEN	 0x2000
#define AINOFORMUPDATE	 0x4000
#define AIINFORMATION	 0x8000
#define AIPLAYERGROUP	 0x10000
#define AI_WSO_MISSILE_WARN 0x20000
#define AI_WSO_ENGAGE_DEFENSIVE 0x40000
#define AI_AWACS_IGNORE	 0x80000
#define AI_FORMED_PLAYER 0x100000
#define AI_ESCORTED_PLAYER 0x200000
#define AI_SEEN_PLAYER 0x400000
#define AI_CHECK_PLAYER_RELEASE 0x800000
#define AI_RELEASED_PLAYER 0x1000000
#define AI_ASK_PLAYER_ATTACK 0x2000000
#define AI_ASK_PLAYER_ENGAGE 0x4000000
#define AI_GIVE_CAS_RESULT 0x8000000
#define AI_BOMBS_HIT 0x10000000
#define AI_HAS_EJECTED 0x20000000
#define AI_KEEP_JAMMER_ON 0x40000000
#define AI_INTERNAL_JAMMER 0x80000000

//  Flags for iAIFlags2
#define AILOCKEDUP		 0x1
#define AISUPPORT		 0x2
#define AIDEFENSIVE		 0x4
#define AIMISSILEDODGE   0x8
#define AIGROUNDAVOID    0x10
#define AICONTROLATTACK  0x20
#define AISUPPORTPAIR	 0x40
#define AIWINGMANTARGET  0x80
#define AIKEEPTARGET	 0x100
#define AIIGNOREWPY		 0x200
#define AIINVISIBLE		 0x400
#define AILANDING		 0x800
#define AI_STAY_ON_GROUND 0x1000
#define AI_CAS_ATTACK	 0x2000
#define AI_SEAD_ATTACK	 0x4000
#define AI_SEAD_COVER	 0x8000
#define AI_RETURN_SEAD_COVER 0x10000
#define AI_RIPPLE_BOMB	 0x20000
#define AI_RADAR_PING	 0x40000
#define AI_CHECK_MISSILES 0x80000
#define AI_CONTINUE_ORG_BEHAVE 0x100000
#define AI_FAKE_AOA		 0x200000
#define AI_PADLOCKED	 0x400000
#define AI_BOMBED_PRIME	 0x800000
#define AI_BOMBED_SECOND	 0x1000000
#define AI_SO_JAMMING	 0x2000000
#define AI_SOJ_ON_BOARD	 0x4000000
#define AI_REPORTED_AS_CONTACT	0x8000000
#define AI_CAN_CRASH	0x10000000
#define AI_GUN_PAUSE	0x20000000
#define AI_IN_DOGFIGHT  0x40000000
#define AI_STALLING		0x80000000

//  Flags for tanker->AI.cFXarray[2]
#define TANKER_D_DOWN		0x01
#define TANKER_WAY_DOWN		0x02
#define TANKER_DOWN			0x04
#define TANKER_UD_OK		0x08
#define TANKER_UP			0x10
#define TANKER_WAY_UP		0x20
#define TANKER_U_UP			0x40
//  #define TANKER_				0x80

//  Flags for tanker->AI.cFXarray[3]
#define TANKER_A_AFT		0x01
#define TANKER_WAY_AFT		0x02
#define TANKER_AFT			0x04
#define TANKER_FA_OK		0x08
#define TANKER_FORE			0x10
#define TANKER_WAY_FORE		0x20
#define TANKER_F_FORE		0x40
//  #define TANKER_				0x80

//   Flags for iAIHumanTankerFlags
#define AI_TANKER_UP	0x1
#define AI_TANKER_DOWN	0x2
#define AI_TANKER_RIGHT 0x4
#define AI_TANKER_LEFT  0x8
#define AI_TANKER_PROBE_OK	0x10
#define AI_TANKER_P_DONE	0x20

//  Flags for iAICombatFlags1
#define	AI_BLOW_THROUGH			0x00000001
#define AI_OFFENSIVE			0x00000002
#define AI_FINISH_BEHAVIOR		0x00000004
#define AI_ESCORT_PART_LEAD		0x00000008
#define AI_NAKED				0x00000010
#define AI_FUEL_TANKS			0x00000020
#define AI_FORMON_SEAD			0x00000040
#define AI_FORMON_SEAD_SEARCH	0x00000080
#define AI_HOME_AIRPORT			0x00000100
#define AI_HOME_CARRIER			0x00000200
#define AI_MULTI_ACTIVE			0x00000400
#define AI_MULTI_FIRE_GUN		0x00000800
#define AI_MULTI_REMOVEABLE		0x00001000  //  Set for planes that are removed when Player leaves;
#define AI_MULTI_SEND_CPOS		0x00002000	//  Set so that I know to send CPOS with CombatBehavior Update
#define AI_CARRIER_LANDING		0x00004000	//  Set so I know a plane is nearing carrier to land (Mainly for Player).
#define AI_HUMAN_CONTROLLED		0x00008000	//  For times when I need to make planes AIs in multiplayer and want to be able to switch back.
#define AI_READY_5				0x00010000  //  So I know a plane is not just waiting to take off.
#define AI_S2_RACK				0x00020000	//  Show Weapon Rack for station 2
#define AI_S3_RACK				0x00040000	//  Show Weapon Rack for station 3
#define AI_S4_RACK				0x00080000	//	Show Weapon Rack for station 4
#define AI_S8_RACK				0x00100000	//	Show Weapon Rack for station 8
#define AI_S9_RACK				0x00200000	//	Show Weapon Rack for station 9
#define AI_S10_RACK				0x00400000	//	Show Weapon Rack for station 10
#define AI_MULTI_TOWED_DECOY	0x00800000	//  Tells if a multiplayer player is using his towed decoy.
#define AI_HELO_BULLET_MASK		0x0F000000  //  Helos Only
#define AI_WINCHESTER_AG		0x40000000
#define AI_WINCHESTER_AA		0x80000000

//  Flags for iAICombatFlags2
#define AI_ANTI_SHIP			0x00000001
#define AI_ANTI_SUB				0x00000002
#define AI_DRONE				0x00000004
#define AI_AWACS_DETECT			0x00000008
#define AI_FAC					0x00000010
#define AI_CARRIER_REL			0x00000020
#define AI_ESCORTABLE			0x00000040
#define AI_UPDATE_POSITION		0x00000080
#define AI_CHECK_TAKEOFF		0x00000100
#define AI_HELP_MOVE			0x00000200
#define AI_READY_FOR_LAUNCH		0x00000400
#define AI_ESCORT_STAY			0x00000800
#define AI_ESCORT_CAS			0x00001000
#define AI_ESCORT_BUSY			0x00002000
#define AI_FAC_JUST_LOOK		0x00004000
#define AI_FAC_REDO_TARGET		0x00008000
#define AI_KEEP_HIGH_RATE		0x00010000
#define AI_TALDS_DONE			0x00020000
#define AI_C6_RACK				0x00040000
#define AI_SAR_CALLED			0x00080000
#define AI_BOLTERED				0x00100000
#define AI_EMERGENCY			0x00200000
#define AI_EARLY_LOCK			0x00400000
#define AI_NOISE_JAMMER			0x00800000
#define AI_GATE_STEALER			0x01000000
#define AI_STT_THREAT			0x02000000
#define AI_FAR_TARGET_BOMB		0x04000000
#define AI_LEADER_MASK			0x70000000
#define AI_TEMP_IGNORE_CHECK	0x80000000  //  This is a variable I use for Contact Calls so I know what planes I can ignore.

//  General Defines
#define WUTOIN (0.478625f)
#define AIHEIGHTBUFFER (0.0095725f)
#define AI_RIPPLE_SPREAD 350
#define AI_AWACS_SWEEP_DELAY	20

#define AI_ENGINE_LO	0
#define AI_ENGINE_LI	1
#define AI_ENGINE_RI	2
#define AI_ENGINE_RO	3
#define AI_RADAR_SLOT	4
#define AI_HYDRAULICS	5
#define AI_FUEL_SLOT	6
#define AI_GUN_SLOT		7
#define AI_WEAPON_SLOT	8
#define AI_REAR_GUN		9
#define AI_MAX_DAMAGE_SLOTS 10

//*****************************************************************************************
//  Prototypes
//*****************************************************************************************
int InitWaypoints(void);
int InitWaypointsMID1(int MissionFile);
//void IGGetPlanes(int numobjs, int MissionFile);
void AIInitGlobals(int firsttime = 0);
void LoadWingmenWeaponInfo();
void IGGetPlanes(int numobjs);
void IGGetPlanesMID1(int numobjs, int MissionFile);
void IGAddMover(int planenum, MBObject oneobject);
PlaneType *IGLoadPlaneType(int typeidnum);

#if 0
WeaponType *IGLoadWeaponType(int typeidnum);
RadarType *IGLoadRadarType(int typeidnum);
#endif

int IGReadScenarioData(char *file_name);
int IGReadOldScenarioDataMID1(int MissionFile);

#if 0
void IGAddScenarioObj(int staticnum, MBStaticObject oneobject);
ScenarioType *IGLoadScenarioType(int typeidnum);
#endif

void AIFlyFormation(PlaneParams *planepnt);
void FlyToWayPoint(PlaneParams *planepnt);
float AIComputeWayPointHeading(PlaneParams *planepnt, float *rtdist, float *rdx, float *rdy, float *rdz, int isrelative);
void AINextWayPoint(PlaneParams *planepnt);
void AINextWayPointNoActions(PlaneParams *planepnt);
void AIChangeDir(PlaneParams *planepnt, double offangle, double dy);
ANGLE AIGetDesiredPitchOffdy(double dy, PlaneParams *planepnt);
ANGLE AIGetDesiredRollOffdyaw(PlaneParams *planepnt, double dyaw, int ignoresecofyaw);
extern "C" __declspec( dllexport ) float ConvertWayLoc(long wayloc);
double AIConvertAngleTo180Degree(ANGLE angleval);
ANGLE AIConvert180DegreeToAngle(double degreeval);
void MBAdjustAIThrust(PlaneParams *planepnt, double desiredspeed, int doindicated);
void MBAISBrakeOn(PlaneParams *planepnt, float percentopen);
void AIInitFirstWayPt();
void AIUpdateGroupSpeed(int planenum, double newspeed);
void AISetUpWayPointActions(PlaneParams *planepnt, MBWayPoints *wayptr);
int AISetUpWayPointEndActions(PlaneParams *planepnt, MBWayPoints *wayptr);
void AIInitFormation(PlaneParams *planepnt, long formationid, int actionnum);
void AIUpdateParadeFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateCruiseFormation(PlaneParams *planepnt, int *planecnt, int oneplane = 0);
void AIUpdateWedgeFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateCombatSpreadFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateWallFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateBoxFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateLadderFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateTrailFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateEchelonFormation(PlaneParams *planepnt, int *planecnt);
void AIUpdateFormation(PlaneParams *planepnt);
void AIUpdateFormationLoc(PlaneParams *planepnt, FPointDouble &leadposition, double angxx, double angxz, double angzx, double angzz, PlaneParams *leadplane = NULL);
PlaneParams *AIGetLeader(PlaneParams *planepnt, int gettop = 0);
PlaneParams *AIGetLeaderNoEscort(PlaneParams *planepnt, int leadplace = 0);
PlaneParams *AIGetLeaderEscort(PlaneParams *planepnt);
void AIFormationFlying(PlaneParams *planepnt);
void AIFormationFlyingLead(PlaneParams *planepnt, PlaneParams *leadplane);
void FlyToWayNoDist(PlaneParams *planepnt);
void AIInitFormationStart(PlaneParams *planepnt);
void AISetOrientation(PlaneParams *planepnt, float heading);
int AIGetNumInGroup(PlaneParams *lookplane, PlaneParams *currplane, int *cntnum);
int AIGetTotalNumInGroup(PlaneParams *currplane);
int AIIsTakeOff(MBWayPoints *waypnt);
void AINearMiss(PlaneParams *planepnt, PlaneParams *shooter);
void AIRemoveFromFormation(PlaneParams *planepnt);
void AISetGroupEvade(PlaneParams *planepnt, PlaneParams *leadplane);
void AIDoJink(PlaneParams *planepnt);
void AIHeadAtAirTarget(PlaneParams *planepnt);
void AIGainAlt(PlaneParams *planepnt);
float AICalculateMach(float Altitude, float vel);
float MachAToFtSec(float Altitude, float Mach);
void AIControlSurfaces(PlaneParams *planepnt);
void AIDetermineAIHz(PlaneParams *planepnt);
void AIChangeDirInFormation(PlaneParams *planepnt, PlaneParams *leadplane, double offangle, double dy, double dhead, double dleadhead);
int MBDoDebugBreak();
int AICheckFBG(PlaneParams *planepnt, PlaneParams *leadplane);
double AICheckHeightInPath(PlaneParams *planepnt, int minheight);
double AIGetDesiredPitch(PlaneParams *planepnt, double dy);
double AIGetPitchForBomb(double bx, double by, double bz, double tx, double ty, double tz, float vel, double planepitch);
float AITimeToRelease(PlaneParams *planepnt);
float AITimeToGround(PlaneParams *planepnt, double VertVel, float dy);
float AICheckForAGMissileRelease(PlaneParams *planepnt, double bx, double by, double bz, double tx, double ty, double tz);
float AIVarTimeToRelease(PlaneParams *planepnt, double bx, double by, double bz, double tx, double ty, double tz, float predict_pitch = -999, int ignorehighdrag = 0);
void AIGroupLevelBomb(PlaneParams *planepnt);
void AILevelBomb(PlaneParams *planepnt);
void AIGroupFlyToDivePoint(PlaneParams *planepnt);
void AIFlyToDivePoint(PlaneParams *planepnt);
int AIInvertedDive(PlaneParams *planepnt);
void AIStartDiveBomb(PlaneParams *planepnt);
void AIDiveBomb(PlaneParams *planepnt);
void AIAfterDiveBomb(PlaneParams *planepnt);
void AIGroupFlyToLoftPoint(PlaneParams *planepnt);
void AIFlyToLoftPoint(PlaneParams *planepnt);
void AILoftBomb(PlaneParams *planepnt);
void AIGroupFlyToPopPoint(PlaneParams *planepnt);
void AIFlyToPopPoint(PlaneParams *planepnt);
void AIPopUpBomb(PlaneParams *planepnt);
void AIPopUpPullIn(PlaneParams *planepnt);
void AIChangeDirPitch(PlaneParams *planepnt, double offangle, ANGLE desiredpitch);
void AIUpdateWaypointBehavior(int planenum, MBWayPoints *waypnt, int numwaypts);
void AIChangeGroupBehavior(PlaneParams *planepnt, void (*orgbehavior)(PlaneParams *planepnt), void (*newbehavior)(PlaneParams *planepnt), int checkbehave = 1, int clearorg = 0);
void AIGetPlaceInAttack(PlaneParams *planepnt);
void AIGetAttackersInGroup(PlaneParams *planepnt, PlaneParams *skipplane, int *attacknum);
long AIAdjustAttackSpeed(PlaneParams *planepnt, float tdist);
PlaneParams *AIGetAttackLeader(PlaneParams *planepnt, PlaneParams *baseplane);
void AIMovePlaneToEndOfAttack(PlaneParams *planepnt);
void AIRedistributeAttackers(PlaneParams *planepnt, void (*attackbehavior)(PlaneParams *planepnt), int vacantnum);
void AIResetPercents(PlaneParams *planepnt);
void AIAltCheck(PlaneParams *planepnt);
double AICheckSafeAlt(PlaneParams *planepnt, double altval);
long AIGetMinSafeAlt(PlaneParams *planepnt, float *distance);
ANGLE AIGetPlanesFlightPitch(PlaneParams *planepnt, int noerrorcheck = 0);
void AISwitchLeader(PlaneParams *planepnt);
void AISetUpEgress(PlaneParams *planepnt);
void AIEgressToWayPt(PlaneParams *planepnt);
void AIEgressToHeading(PlaneParams *planepnt);
float AIConvertSpeedToFtPS(float altitude, long desiredspeed, int flags);
long AIConvertFtPSToSpeed(float altitude, float desiredspeed, int flags);
void AIBreakRight();
void AIBreakLeft();
void AIBreakHigh();
void AIBreakLow();
void AISetUpBreak(int breaktype);
void AIBreakToHeading(PlaneParams *planepnt);
void AIMissileBreakToHeading(PlaneParams *planepnt);
void AIBreakToPitch(PlaneParams *planepnt);
void AIWSanitizeRight();
void AIWSanitizeLeft();
void AIESanitizeRight();
void AIESanitizeLeft();
void AIDSanitizeRight();
void AIDSanitizeLeft();
void AISetUpSanitize(int firstvalid, int lastvalid, int sanitizetype);
void AISetUpSanitizePlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int sanitizetype);
void AISanitizing(PlaneParams *planepnt);
void AIFlyTakeOff(PlaneParams *planepnt);
void AISetUpCAPStart(PlaneParams *orgplanepnt);
void AISetUpCAPStartSinglePair(PlaneParams *orgplanepnt);
void AISetUpCAPPlayer(PlaneParams *orgplanepnt, int firstvalid, int lastvalid);
void AISetUpCAP1(PlaneParams *planepnt);
void AISetUpCAP2(PlaneParams *planepnt);
void AIFlyCAPSide1(PlaneParams *planepnt);
void AIFlyCAPSide2(PlaneParams *planepnt);
void AIFlyCAPSide3(PlaneParams *planepnt);
void AIFlyCAPSide4(PlaneParams *planepnt);
void AIStandardCAPStuff(PlaneParams *planepnt);
void AICheckCAPCondition(PlaneParams *planepnt);
void AICheckAWACSCondition(PlaneParams *planepnt, int awacsact);
void AICheckSOJCondition(PlaneParams *planepnt, int sojact);
float FlyToPoint(PlaneParams *planepnt, FPoint pointpos);
float AIComputeHeadingToPoint(PlaneParams *planepnt, FPoint pointpos, float *rtdist, float *rdx, float *rdy, float *rdz, int isrelative);
int AIConvert8ToHeading(int headingval);
void AIUpdateBehaviorFormation(PlaneParams *planepnt);
void AIUpdateBehaviorFormationLoc(PlaneParams *planepnt, PlaneParams *leadplane, FPointDouble &leadposition, double angxx, double angxz, double angzx, double angzz);
void AIGetOrbitOffset(PlaneParams *planepnt, FPoint *orbitpoint, int orbittype = 0);
void AICheckOrbitCondition(PlaneParams *planepnt, int islead);
void AIFlyToOrbitPoint(PlaneParams *planepnt);
void AIFlyOrbit(PlaneParams *planepnt);
float AIGetTurnRateForOrbit(float radft, long desiredspeed, float altitude);
void AIDrawSimpleRadar();
int AIAAStandardChecks(PlaneParams *planepnt);
void AIBracketTargetRight(PlaneParams *planepnt);
void AIBracketTargetLeft(PlaneParams *planepnt);
void AIDragTargetRight(PlaneParams *planepnt);
void AIDragTargetLeft(PlaneParams *planepnt);
float AIBracketTargetHeadingOff(PlaneParams *planepnt, float headingoff);
float AIDragTargetHeadingOff(PlaneParams *planepnt, float headingoff);
void AISplitHighTarget(PlaneParams *planepnt);
void AISplitLowTarget(PlaneParams *planepnt);
float AISplitTargetOffset(PlaneParams *planepnt, int offsetval);
PlaneParams *AIGetAirTargetPosition(PlaneParams *planepnt, FPoint *targetpos, int allowthreat = 1, int allowtarget = 1, int allowWFP = 1);
void AIAirInvestigate(PlaneParams *planepnt);
void AIAirEngage(PlaneParams *planepnt);
void AIPostHole(PlaneParams *planepnt, float rollval, float pitchval, long minalt);
void AIAirEngageSetPlane(PlaneParams *planepnt);
void AIGetFormationArea(PlaneParams *planepnt, FPoint *minform, FPoint *maxform, int *numgroup);
int AIAATargetChecks(PlaneParams *planepnt);
void AIGainAltitude(PlaneParams *planepnt);
void AIMissileAvoid(PlaneParams *planepnt, float *roffangle, float *rtdist);
void AIDefensiveACM(PlaneParams *planepnt, float *roffangle, float *rtdist);
void AISupportACM(PlaneParams *planepnt, float *roffangle, float *rtdist);
void AIOffensiveACM(PlaneParams *planepnt, float *roffangle, float *rtdist);
void AICoreAttack(PlaneParams *planepnt, float *roffangle, float *rtdist);
void AIAllOutAttack(PlaneParams *planepnt);
void AIFightingWing(PlaneParams *planepnt);
void AIDoubleAttack(PlaneParams *planepnt);
int AIGoodTargetEngage(PlaneParams *planepnt, float roffangle, float rtdist);
void AILooseDeuce(PlaneParams *planepnt);
void AIFluidFour(PlaneParams *planepnt);
void AIGaggle(PlaneParams *planepnt);
void AIDoMessageAction(MessageActionType *pMessageAction);
void AIDoMessageEvent(EventActionMessageType *pMessageAction);
void AIUpdateGoalStates(int planenum, int replacenum = 0, int leadnum = 0, int objecttype = 3);
void AICheckEvents(int changeid, long changeval, long extraflag = 0);
void AIDoDelayedEvents(int eventnum);
void AICheckTimeEvent(TimeEventType *TimeEvent, int eventnum);
void AICheckAreaEvent(AreaEventType *AreaEvent, int eventnum);
void AICheckMovingAreaEvent(MovingAreaEventType *AreaEvent, int eventnum);
void AICheckGoalEvent(GoalEventType *GoalEvent, int eventnum);
void AICheckObjectEvent(ObjectEventType *ObjectEvent, int eventnum);
void AICheckEndEvent(MissionEndEventType *EndEvent, int eventnum);
int AICheckGeneralEvent(int inumconditions, MBCondition *EventConition);
void AIDoTimeEvent(TimeEventType *TimeEvent);
void AIDoAreaEvent(AreaEventType *AreaEvent);
void AIDoMovingAreaEvent(MovingAreaEventType *AreaEvent);
void AIDoGoalEvent(GoalEventType *GoalEvent);
void AIDoObjectEvent(ObjectEventType *ObjectEvent);
void AIDoGeneralEvent(int numactions, MBEvents *EventAction);
void AIDoEndEvent(MissionEndEventType *EndEvent);
void AIDamagePlane(long planenum, int damageamount);
void AIChangeEventFlag(int flagval);
int AICheckEventFlagStatus(int flagval);
int AIScanEventArea(AreaEventType *AreaEvent, int eventnum);
int AIScanMovingEventArea(MovingAreaEventType *AreaEvent, int eventnum);
int AICheckGroupForAreaEvent(PlaneParams *planepnt, int checkval, long event_bit_flag, long event_bit_flag2);
void AIAlterPlanePath(EventActionAltPathType *pAltPathAction);
void AIUpdatePathObjLinks(long planenum, long replacenum);
void AIDamageObjectGroup(long planenum, long damageamount);
void AIAddPlaneGroupToFormation(PlaneParams *planepnt, int planenum);
long AIGetFormationMaxZ(int planenum, long currmax);
long AIGetFormationMinZ(int planenum, long currmin);
void AIAddToFormPos(PlaneParams *planepnt, long addamount);
void AIReleasePlaneGroupFromFormation(PlaneParams *planepnt, int planenum, int newwaynum);
void AIReleaseEscort(PlaneParams *planepnt, int planenum, int newwaynum);
void AIReleaseEscort2(int planenum, int leadnum);
void AISetUpNewWayPoint(PlaneParams *planepnt);
void AIUpdatePlanesInActions(int planenum, int replacenum, int leadnum);
void AIFlyEscort(PlaneParams *planepnt);
int AIGetNumEscorting(PlaneParams *planepnt, PlaneParams *leadprotect, int *escortpos, int *numsead, int *numcas = NULL, int *seadstay = NULL);
void AIGetFormationMinsAndMaxs(PlaneParams *planepnt, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz);
void AIUpdatePlaneLinks(PlaneParams *planepnt, PlaneParams *replaceplane);
void AISetEscortPos(PlaneParams *planepnt, PlaneParams *escorted, FPoint escortpos);
void AISetUpEscort(PlaneParams *planepnt, int escortplane);
void AIUpdateGroupPlaneLink(int planenum, PlaneParams *linkedplane);
void AIFlyToEscortMeeting(PlaneParams *planepnt);
void AICheckEscortRelease(PlaneParams *planepnt);
void AICheckEscortRestore(PlaneParams *planepnt);
void AIEscortOrbitCCSet(PlaneParams *planepnt);
void AIEscortOrbitCSet(PlaneParams *planepnt);
void AIFlyToEscortOrbitPoint(PlaneParams *planepnt, int orbittype = 0);
void AIFlyEscortOrbit(PlaneParams *planepnt);
void AIUpdateEscortFormation(PlaneParams *planepnt);
PlaneParams *AIGetEscortLeader(PlaneParams *planepnt);
void AIResumeEscort(PlaneParams *planepnt);
void AICheckHumanBombWaypoint(PlaneParams *humanplane, PlaneParams *planepnt);
void AICheckHumanNonBombWaypoint(PlaneParams *humanplane, PlaneParams *planepnt);
int AIWaitingForTakeOffFlag(PlaneParams *planepnt);
void AIFlyToHoverPoint(PlaneParams *planepnt);
void AIFlyHover(PlaneParams *planepnt);
void AIFlyToLandingPoint(PlaneParams *planepnt);
void AIFlyHeloLanding(PlaneParams *planepnt);
void AIJustHeloLand(PlaneParams *planepnt);
void AIHeloSinkToGround(PlaneParams *planepnt);
void AIParked(PlaneParams *planepnt);
void AISetUpSweepAction(PlaneParams *planepnt, FighterSweepActionType *pSweepAction);
void AIFlySweep(PlaneParams *planepnt);
void AICheckSweepConditions(PlaneParams *planepnt);
int AICheckSweepFlags(PlaneParams *planepnt);
void AIGetNextSweepWayPt(PlaneParams *planepnt);
void AIAfterSweepWayPt(PlaneParams *planepnt);
void AIGetXZOffset(float offangle, float orgx, float orgz, long xftoff, long zftoff, float *newx, float *newz, PlaneParams *updateplane = NULL);
void AISetPlaneLandingFlyToField(PlaneParams *planepnt, TakeOffActionType *pActionTakeOff, ANGLE runwayheading = 0);
void AIPlaneLandingFlyToField(PlaneParams *planepnt);
void AISetEnterLandingPattern(PlaneParams *planepnt);
void AIUpdateLandingFormation(PlaneParams *planepnt, int *planecnt, PlaneParams *leadplane);
void AIEnterLandingPattern(PlaneParams *planepnt);
void AISetLandingApproachBreak(PlaneParams *planepnt);
void AILandingApproachBreak(PlaneParams *planepnt);
void AISetUpLandingTiming(PlaneParams *planepnt);
void AISetUpLandingTimingPlane(PlaneParams *planepnt, int *planecnt, PlaneParams *leadplane, int dowingpairs);
void AIFormationFlyingLinked(PlaneParams *planepnt);
void AIFormationFlyingLinkedOffset(PlaneParams *planepnt);
void AISetLandingLevelBreak(PlaneParams *planepnt);
void AILandingLevelBreak(PlaneParams *planepnt);
void AISetLandingLevelBreak2(PlaneParams *planepnt);
void AILandingLevelBreak2(PlaneParams *planepnt);
void AISetLandingDownwind(PlaneParams *planepnt);
void AILandingDownwind(PlaneParams *planepnt);
void AISetLandingBaseLeg(PlaneParams *planepnt);
void AILandingBaseLeg(PlaneParams *planepnt);
void AISetLandingBaseLeg2(PlaneParams *planepnt);
void AILandingBaseLeg2(PlaneParams *planepnt);
void AISetLandingFinal(PlaneParams *planepnt);
void AILandingFinal(PlaneParams *planepnt);
void AISetLandingTouchdown(PlaneParams *planepnt);
void AILandingTouchdown(PlaneParams *planepnt);
void AISetLandingRollout(PlaneParams *planepnt);
void AILandingRollout(PlaneParams *planepnt);
void AISetLandingTaxi(PlaneParams *planepnt);
void AILandingTaxi(PlaneParams *planepnt);
float AIGetMarshallHeight(PlaneParams *planepnt);
int AICheckMarshallPattern(PlaneParams *planepnt);
void AIFlyMarshallPattern(PlaneParams *planepnt);
void AIFlySlowMarshallPattern(PlaneParams *planepnt);
void AISetUpSlowLanderPattern(PlaneParams *planepnt);
int AIMoreInSlowLanding(PlaneParams *planepnt);
void AIOffsetFormationPositions(int planenum, FPoint offsetval);
void AIAllowNextLander(PlaneParams *planepnt);
void AISetLandingSlowBaseLeg2(PlaneParams *planepnt);
void AILandingSlowBaseLeg2(PlaneParams *planepnt);
int AICheckHumanMarshallPattern();
void AIFlyToCASArea(PlaneParams *planepnt);
void AIFlyCAS(PlaneParams *planepnt);
long AIGetCASorSEADTime(PlaneParams *planepnt);
void AICheckCASorSEADCondition(PlaneParams *planepnt);
void AIRestoreCASorSEAD(PlaneParams *planepnt);
void AISimpleGroundRadar(PlaneParams *planepnt);
void AISEADSearch(PlaneParams *planepnt);
void AITransferCASLead(PlaneParams *planepnt);
void AIUpdateCASLead(PlaneParams *planepnt, PlaneParams *oldlead, PlaneParams **newlead);
void AIUpdateLinkedLead(PlaneParams *planepnt, PlaneParams *oldlead, PlaneParams **newlead);
int AICheckIfAlreadyGroundTarget(PlaneParams *searchplane, PlaneParams *planepnt, void *object_id, long object_flag, int check_stays = 0);
void AIDelayCASRadar(PlaneParams *planepnt);
void AIAwayFromTarget(PlaneParams *planepnt);
void AIGetMaxDamage(BasicInstance *instance, float *sustainable, float *total);
DamageProportions *AIGetDamageRatio(BasicInstance *instance,DWORD damage_type);
void AIDetermineBestBomb(PlaneParams *planepnt, void *pGroundTarget, long groundtype = 3);
int AIIsGrndWeaponNear(PlaneParams *planepnt);
void AISelectGroupAttackWeapons(PlaneParams *planepnt, BombTarget *pActionBombTarget);
void AIGetAttackWeapon(PlaneParams *planepnt, BombTarget *pActionBombTarget);
int AISelectNextWeaponStation(PlaneParams *planepnt);
int AIGetNumWeaponIDOnBoard(PlaneParams *planepnt, long weaponid);
int AIIsSEADPlane(PlaneParams *planepnt);
void AIReplaceCAPorSweep(PlaneParams *planepnt);
void AICoveredPlaneLanding(PlaneParams *planepnt, PlaneParams *linkedplane);
MBWayPoints	*AIGetWaypointAfterLastEscort(PlaneParams *planepnt, int escortplanenum, int *numleft);
int AICheckForWayPointsAfterEscort(PlaneParams *planepnt, int escortplanenum);
int AICheckForEscortsArriving(PlaneParams *planepnt);
int AISeeIfPossibleEscort(PlaneParams *planepnt, PlaneParams *linkedplane);
void AISARWait(PlaneParams *planepnt);
void AICheckSARCondition(PlaneParams *planepnt);
void AIFlyToSARPoint(PlaneParams *planepnt);
void AISARPickUp(PlaneParams *planepnt);
void *AIGetClosestAirField(PlaneParams *planepnt, FPointDouble worldpos, FPointDouble *fieldpos, ANGLE *runwayheading, TakeOffActionType *ActionTakeOff, double minfar = -1.0f, int ignoreside = 0, int allow_both_ends = 0, int ignore_destroy = 0);
int AIIsClosestAirFieldOpen(PlaneParams *planepnt, FPointDouble worldpos, double minfar);
int AICheckForSAR(PlaneParams *planepnt);
void AIUpdateSAR(PlaneParams *planepnt);
void AIGetSARCAP(PlaneParams *planepnt);
void AISetUpSARCAP(PlaneParams *planepnt);
void AIFlySARCAPSide1(PlaneParams *planepnt);
void AIFlySARCAPSide2(PlaneParams *planepnt);
void AIFlySARCAPSide3(PlaneParams *planepnt);
void AIFlySARCoverSide1(PlaneParams *planepnt);
void AIFlySARCoverSide2(PlaneParams *planepnt);
void AIReleaseSARCover(PlaneParams *planepnt);
int AIGetWeaponIndex(int itypenum);
int AIInNextRange(PlaneParams *planepnt, float ftdist, int okdist, float maxroll = 90.0f);
float AIGetTurnRadFt(PlaneParams *planepnt);
float AIComputeHeadingFromBullseye(FPoint pointpos, float *rtdist, float *rdx, float *rdz);
int AICanFireMissile(PlaneParams *planepnt, PlaneParams *targplane, float range);
int AINumMissilesAtPlane(PlaneParams *targplane);
void AIRestoreWaypointBehavior(int planenum, MBWayPoints *waypnt, int numwaypts);
int AIInPlayerGroup(PlaneParams *planepnt);
void AICheckMaxGs(PlaneParams *planepnt);
void AICheckRearGuns(PlaneParams *planepnt);
void AISwitchToAttack(PlaneParams *planepnt, int defensive = 0, int missile = 0);
void AI_ACM(PlaneParams *planepnt, PlaneParams *target, double offangle, double toffpitch, double tist, double toffangle);
int AICheckForGuns(PlaneParams *planepnt, PlaneParams *target, float offangle, float offpitch, float planedist);
void AICounterTargetMovement(PlaneParams *planepnt, PlaneParams *target, float offangle, float offpitch, FPoint aimpoint, double desiredyaw);
void AICheckHumanThreats(PlaneParams *planepnt, PlaneParams *checkplane, float dx, float dy, float dz, float visualrange);
void AICheckForNewOffensiveTactic(PlaneParams *planepnt, PlaneParams *target, double offangle, double toffpitch, double tdist, double toffangle);
void AIDetermineMerge(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIGetSeperation(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIAfterSeperation(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
int AIAAGunsOnly(PlaneParams *planepnt);
void AIJoust(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIBarrelAvoid(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIPitchAndTurn(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIPrepareLeadTurn(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIDoLeadTurn(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AISetUpLagPursuitRoll(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIStartLagPursuitRoll(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIFinishLagPursuitRoll(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIComputeInterceptPoint(PlaneParams *planepnt, FPointDouble attackerposition, FPoint *aimpoint, float attackervel, float timemod = 0);
void AILowYoYo(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIHighYoYo(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AIHighYoYo2(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AICheckForLiftVectorUsage(PlaneParams *planepnt, PlaneParams *target, FPointDouble targetposition, double offangle, double toffpitch, double yawval = -10000.0f);
void AIImmelmann(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AISplitS(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AICuban8(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
double AICapAngle(double angle, double minval = -180.0f, double maxval = 180.0f);
void AICheckForNewDefensiveTactic(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double tdist, double toffangle);
void AITurnAway(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
void AICheckForChutePickup(PlaneParams *planepnt);
void AICheckForVehicleSARPickup(PlaneParams *planepnt);
void AI_SAR_Mark(PlaneParams *planepnt);
int AICheckIfGroupAAClear(PlaneParams *planepnt);
void AICheckIfDefensive(PlaneParams *planepnt, float *roffangle, float *rtdist);
int AINumberOfSideInArea(FPointDouble position, int side, float maxrange, int fightersonly);
int AIIsBeingEscorted(PlaneParams *planepnt);
void AISetUpEscortInterceptGroup(PlaneParams *planepnt, PlaneParams *target);
int AISendEscortsStartingWith(int numingroup, PlaneParams *currplane, PlaneParams *target, int currcnt);
int AIGetTotalNumPlayerGroupCovering(PlaneParams *currplane);
PlaneParams *AIGetEscortLead(PlaneParams *planepnt);
void FlyCruiseMissile(PlaneParams *planepnt);
void AISetUpPlayerCAPIntercept(PlaneParams *planepnt, PlaneParams *target);
void AIRemoveTopWingDTWS();
int AILOS(FPointDouble planepos, FPointDouble targetpos);
int AIAllGroupAGWinchester(PlaneParams *planepnt);
int AIAllGroupAAWinchester(PlaneParams *planepnt);
void AIDisengage(PlaneParams *planepnt);
void AISetGroupDisengage(PlaneParams *planepnt, PlaneParams *leadplane);
void AIExtend(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double dist, double toffangle);
int AIGetNumTankingWithPlayer(PlaneParams *planepnt);
PlaneParams *AIAvoidLandingCrash(PlaneParams *planepnt);
void AIDeathSpiral(PlaneParams *planepnt);
void AIVanishPlane(PlaneParams *planepnt);
void StartCasualAutoPilot();
void EndCasualAutoPilot();
int AICheckRadarCrossSig(PlaneParams *planepnt, PlaneParams *checkplane, float daltft, float distnm, DBRadarType *radar, int inSTT);
int AICheckPlayerBreak(PlaneParams *planepnt, PlaneParams *target, double offangle, double offpitch, double tdist, double toffangle);
void AIFlyToCASOrbitPoint(PlaneParams *planepnt);
void AIFlyCASOrbit(PlaneParams *planepnt);
void AICheckForAC_130Guns(PlaneParams *planepnt, float offangle, float offpitch, float targetdist);
float AIGetTimeBulletInterceptGround(FPointDouble targetpos, float speedfps, FPointDouble gunposition, float bulletvel);
void GDComputeGunHPGround(void *ptarget, long targettype, FPointDouble gunposition, FPoint *aimpoint, float bulletvel, float *heading, float *pitch, float *usetime, float timemod, int truepitch);
void AISendGroupHome(PlaneParams *planepnt);
void AIDropTanks(PlaneParams *planepnt);
void AISetFighterDisengage(PlaneParams *planepnt);

#if 0
int AIPlaneAlreadyTarget(PlaneParams *planepnt, PlaneParams *targplane);
#else
int AIPlaneAlreadyTarget(int launchertype, void *launchpnt, PlaneParams *targplane);
#endif

void AIFireMissile(PlaneParams *planepnt, PlaneParams *targplane, int stationnum);
int AISeeIfStillMissiled(PlaneParams *planepnt, PlaneParams *targplane, int maintain, int missilesonly = 0);
int AISeeIfStillGroundMissiled(BasicInstance *radarsite, PlaneParams *targplane, int maintain, int missilesonly = 0);
//  int AISeeIfStillVehicleMissiled(MovingVehicleParams *radarsite, PlaneParams *targplane, int maintain);
void AISetUpFormOnMeeting(PlaneParams *planepnt, int formonplane);
void AIFlyToFormOnMeeting(PlaneParams *planepnt);
void AIUpdateGroupFlags(int planenum, int flag1, int flag2, int doinv = 0, int combatflags1 = 0);
void AICheckIfPlayerDoingEscort(int planenum);
int AICheckAutoPlayerRelease(PlaneParams *planepnt);
int AINoBombingLeft(int planenum);
void EVNextEyePoint();
void EVActivateEyePoint(int eyenum);
void EVSetNewEyePoint();
int AICheckNearBombWaypt(PlaneParams *planepnt);
void AICheckROE(PlaneParams *planepnt, int action);
void AIUpdateAttackOnVehicle(PlaneParams *planepnt);
void AIDoViewAction(ViewActionType *pViewAction);
void AIRestoreViewActionCamera();
void AILetFormedOnLand(PlaneParams *planepnt);
int AIIsCurrentlyBombing(PlaneParams *planepnt);
int AIGroundAlreadyTarget(void *target);

void LogError(char *sErrorDesc, char *sFile, int iLine, int iTerminate);


// DataBase - This should be in a InitDB.h file

extern void InitDB();
extern void FreeDB();
extern int GetPlaneIndex(long lPlaneID);
extern void EvaluateAircraft();
extern int RegisterWeapon(long lWeaponId);
extern int RegisterAircraft(long lAircraftId);

//  New For F/A-18
void WeaponAsPlane(PlaneParams *planepnt);
void TALDAsPlane(PlaneParams *planepnt);
void FlyDrone(PlaneParams *planepnt);
void AISetUpAttackTarget(PlaneParams *planepnt, AttackTargetActionType *attack_action);
void AISetUpPlayerGroupChuteDrop(PlaneParams *planepnt, MBWayPoints *bombway);
void AIDropChuteMines(PlaneParams *planepnt);
float AICheckForStraightAttack(PlaneParams *planepnt, FPointDouble worldpos, FPoint targetpos);
void AIStraightRocketAttack(PlaneParams *planepnt);
void AIStraightGunAttack(PlaneParams *planepnt);
void AIUpdateStraightAttackOnVehicle(PlaneParams *planepnt);
void AIWeaponPlaneShipSearch(PlaneParams *planepnt);
void AICheckFACTarget(PlaneParams *planepnt);
int AICheckFACSmoke(PlaneParams *planepnt);
void AICheckIsEscorted(PlaneParams *planepnt);
void AIAfterEscortableWayPts(PlaneParams *planepnt);
int AIConvertVGroupToVNum(int vgroup);
int AIPutPlaneInTEWSCenter(PlaneParams *planepnt);
int AICheckForGateStealerP(PlaneParams *planepnt, PlaneParams *target);

//  In sflight.cpp
void GetAISimpleFuelUsage(PlaneParams *P);


