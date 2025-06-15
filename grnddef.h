//*******************************************************************************
//*  Grnddef.h
//*
//*  This file contains defines, types, prototypes, and globals
//*  dealing with ground defenses.
//*******************************************************************************

// lRFlags1 Defines
#define GD_RADAR_ON				0x00000001
#define GD_RADAR_TEMP_ON		0x00000002
#define GD_RADAR_TEMP_OFF		0x00000004
#define GD_RADAR_PLAYER_PING	0x00000008
#define GD_RADAR_LOCK			0x00000010
#define GD_LOW_ALERT			0x00000020
#define GD_MED_ALERT			0x00000040
#define GD_HIGH_ALERT			0x00000080
#define GD_CHECK_MISSILES		0x00000100
#define	GD_I_AM_DEAD			0x00000200
#define GD_VISUAL_SEARCH		0x00000400
#define GD_BURST_MODE			0x00000800
#define GD_HAS_BEEN_REPORTED	0x00001000
#define GD_HAS_BEEN_REP_GEN		0x00002000
#define GD_KEEP_BURST_TARGET	0x00004000
#define GD_RADAR_OFF_FOR_HARM	0x00008000

//  lWFlags1
#define GD_W_ACQUIRING			0x00000001
#define GD_W_FIRING				0x00000002
#define GD_W_RANDOM_FIRE		0x00000004
#define GD_W_DUPLICATE_TARGET   0x00000008
#define GD_W_STREAM				0x00000010
#define GD_W_BURST				0x00000020
#define GD_W_SINGLE				0x00000040
#define GD_W_TIME_INC			0x00000080
#define GD_W_TIME_DEC			0x00000100
#define GD_W_HEADING_INC		0x00000200
#define GD_W_HEADING_DEC		0x00000400
#define GD_W_PITCH_INC			0x00000800
#define GD_W_PITCH_DEC			0x00001000
#define GD_W_PREP_RANDOM		0x00002000


typedef struct stGDRadarData {
	long	lRActionTimer;
	long	lRLockTimer;
	long	lRGenericTimer;
	long	lRAlertTimer;
	long	lRFlags1;
	void	*pRBurstTarget;
	long	lRBurstTimer;
	int		iRBurstCounter;
	int		iConfidence;
	long	lWActionTimer;
	long	lWGenericTimer;
	long	lWReloadTimer;
	int		lWNumberFired;
	long	lWFlags1;
	void	*pWRadarSite;
	void	*Target;
	int		iTargetDist;
	ANGLE	FireHeadingANG;
	ANGLE	HeadingOffsetANG;
	ANGLE	FirePitchANG;
	ANGLE	PitchOffsetANG;
	WeaponType *WType;
	int		iWeaponIndex;
	AAAStreamVertex *pAAAStream;
	FPoint	fpAimPoint;
	float	fImpactTime;
	long	lTimeDiff;
	long	lTimeDiffOffset;
	int 	iSkill;
	int		iShotsTillNextCheck;
} GDRadarData;

void AICheckGrndRadar();
void GDInitDefenses();
void GDInitRuwaySides();
void GDCheckDefenses();
void AIGroundWeaponAction(AAWeaponInstance *currweapon, GDRadarData *radardat);
void AISimpleGroundRadar(BasicInstance *radarsite);
int GDCheckForAAAFire(BasicInstance *radarsite, PlaneParams *planepnt, float foundrange);
int GDLookForFreeSAM(BasicInstance *radarsite, PlaneParams *planepnt, float foundrange);
AAWeaponInstance *GDCheckProviderForFreeSAM(BasicInstance *currinst, PlaneParams *planepnt, float targdist, float *currdist);
AAWeaponInstance *GDCheckTreeForFreeSAM(InfoProviderInstance *currinfo, BasicInstance *skipinst, PlaneParams *planepnt, float targdist, float *currdist);
int GDSetUpWeaponTargeting(AAWeaponInstance *foundweapon, PlaneParams *planepnt, float currdist);
int GDCheckChildrensRadar(AAWeaponInstance *foundweapon, PlaneParams *planepnt, float currdist, InfoProviderInstance *treetop);
int GDAParentOfB(BasicInstance *radarsite, BasicInstance *cradarsite);
void GDCheckForClients(BasicInstance *walker);
int GDSameAncestor(BasicInstance *radarsite, BasicInstance *cradarsite);
void GDClearWeaponLauncher(WeaponParams *W);
int AIRadarOnCheck(BasicInstance *checkobject);
GDRadarData *GDGetRadarData(BasicInstance *checkobject);
void GDNoticeExplosion(WeaponParams *W);
void GDNoticeAAAFire(FPointDouble *position, int side);
InfoProviderInstance *GDGetInfoLeader(BasicInstance *radarsite);
void GDUpgradeTreeAlert(InfoProviderInstance *currinfo, BasicInstance *skipinst, FPoint position);
int GDCheckForPingReport(void *radarsite, PlaneParams *planepnt, PlaneParams *pingplane = NULL, int sitetype = 3);
int GDNumLaunchingAtPlane(PlaneParams *planepnt);
int GDNumAAAFiringAtPlane(PlaneParams *planepnt);
float GDModifyNoticePercent(void *radarsite, GDRadarData *radardat, PlaneParams *checkplane, int sitetype = 3);
int GDConvertGrndSide(BasicInstance *walker);
int GDConvertRunwaySide(RunwayInfo *walker);
float GDGetTimeBulletIntercept(PlaneParams *planepnt, FPointDouble gunposition, float bulletvel);
void GDComputeGunHP(PlaneParams *planepnt, FPointDouble gunposition, FPoint *aimpoint, float bulletvel, float *heading, float *pitch, float *usetime, float timemod, int truepitch = 0);
void GDFireInitialAAA(void *gun, PlaneParams *planepnt, int sitetype = 3, int radardatnum = 0);
void GDSetUpRandomAAA(void *gun, int firsttime = 1, int sitetype = 3, int radardatnum = 0);
void GDFireAAA(void *gun, int sitetype = 3, int radardatnum = 0);
AAWeaponInstance *GDCheckProviderForFreeAAA(BasicInstance *currinst, PlaneParams *planepnt, float targdist, float *currdist);
AAWeaponInstance *GDCheckTreeForFreeAAA(InfoProviderInstance *currinfo, BasicInstance *skipinst, PlaneParams *planepnt, float targdist, float *currdist);
void GDUpdateImpactPoint(void *currweapon, int sitetype = 3, int radardatnum = 0);
void GDUpdateImpactPointLite(void *currweapon, int sitetype = 3, int radardatnum = 0);
float GDGetWeaponVelocity(DBWeaponType *weapon);
long GDGetAAADuration(DBWeaponType *weapon);
void GDUpdateReportedArea(FPointDouble Position, int sitetype, double use_radius = -1.0f);
void GDAAASeen(PlaneParams *planepnt);
int AICheckIfAnyInterception(PlaneParams *planepnt, int side);
void AIContactAirCommand(BasicInstance *radarsite, PlaneParams *foundplane, int side);
void AIGetGroundJammingMod(FPointDouble position, int grndside, float *modifiers);
void GDCheckForRandomAAAHit(FPointDouble Pos, DBWeaponType *pweapon_type, WeaponParams *W = NULL, AAAStreamVertex *AAA = NULL);
float AGClosureRate( BasicInstance *walker, PlaneParams *T );
int AICheckRadarCrossSig(BasicInstance *walker, PlaneParams *checkplane, float daltft, float distnm, DBRadarType *radar, int inSTT);
int GDCheckNumRadarTracks(BasicInstance *radarsite, DBRadarType *radar, int converttarget);
int AIPutInfoProviderInTEWSCenter(InfoProviderInstance *radarsite, GDRadarData *radardat);
int AIPutWeaponProviderInTEWSCenter(AAWeaponInstance *radarsite, GDRadarData *radardat);
int AICheckForGateStealerG(BasicInstance *radarsite, GDRadarData *radardat, PlaneParams *target);

//  In dogfite.cpp
void AIUpdateGroundRadarThreat(void *radarsite, PlaneParams *checkplane, float tdist, int sitetype = 3, GDRadarData *orgradardat = NULL);

WeaponParams *InstantiatePlaceHolder(FPointDouble position);
void DeletePlaceHolder(WeaponParams *W);
void MovePlaceHolder(WeaponParams *W);
